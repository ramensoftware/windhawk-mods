// ==WindhawkMod==
// @id              custom-cached-context-menu
// @name            Custom Cached Context Menu
// @description     Instantly-opening cached Explorer context menu that is fully customizable: themes, layout, colors, animations, rules, and custom commands.
// @version         1.0.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -luuid -lcomctl32 -ladvapi32 -lgdi32 -luxtheme -lversion -ld3d11 -ld2d1 -ldwrite -ldcomp
// @license         MIT
// @author          BOLT
// @github          https://github.com/Bolt-Scripts
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Cached Context Menu

Replaces the Windows Explorer context menu with a self-rendered menu that opens
instantly and can be customized extensively with fun effects.

![The bundled themes](https://raw.githubusercontent.com/Bolt-Scripts/custom-cached-context-menu/master/assets/theme-collage.png)

The native menu is slow because every registered shell extension is loaded
synchronously before it can be shown. This mod shows a cached menu right away
and discovers extension items in the background, so repeated opens are instant.

## Highlights

- **Instant, cached menus** — the first open of a context discovers and caches
  the shell's items; later opens render from the cache, and common contexts are
  pre-built in the background at startup.
- **Custom rendering** — the menu is drawn with Direct2D: rounded corners,
  blur, shadows, per-item icons, hover feedback, and animations, instead of the
  classic owner-drawn menu.
- **Themes** — bundled themes (Windows 11/10 dark and light, Nord, Dracula,
  Solarized, Gruvbox, One Dark, Cyberpunk, Synthwave, Terminal Green, Amber
  CRT, Tokyo Night, AMOLED Black, High Contrast) plus full `menu.ini` control.
  Each theme is a self-contained file you can edit or copy.
- **Animations** — combinable open/close effects (`fade`, `slide`, `scale`,
  `dissolve`, `crt`, `unfold`) with easing, timing, direction, and optional
  submenu animation.
- **Overlays** — combinable effects drawn over the menu content (`noise`,
  `plasma`, `hue`, `glow`, `scanlines`, `vignette`) with intensity, animation
  speed, size, and frame interval.
  `overlayAnimate` keeps them moving while the menu stays open (off by
  default); otherwise they animate only during the open/close. The Terminal
  Green and Amber CRT themes ship with scanlines.
- **Settings menu** — in the advanced ("More options") submenu, or via the
  optional global hotkey. Appearance is edited live (sliders with typed values,
  a color picker, a font-face field with installed-font validation, instant
  preview) and written back to the active theme file or `menu.ini`.
- **No code needed** — `menu.ini` supports rules (`hide`/`keep`/`move`),
  custom commands, custom submenus, per-item overrides, and an icon library.

## Settings

| Setting | Default | Description |
|---|---|---|
| Bypass keys | on | Hold Shift while right-clicking to bypass the mod and get the stock classic menu, or Ctrl for the untouched native menu. |
| Menu mode | Custom (recommended) | Custom draws the self-rendered menu and falls back to the classic menu after repeated failures; Classic always uses the owner-drawn menu. |
| Theme | Custom (menu.ini) | Loads the appearance from `<mod storage>\themes\<name>.ini` (created from the bundled preset on first use). `menu.ini` is never modified by theme selection. |
| Show classic menu item | on | Adds a "Show classic menu" entry at the bottom of the menu. |
| Warm-up extensions | common list | Comma-separated file types pre-built at Explorer startup. |
| Warm-up delay | 5 s | Delay before background warm-up starts. |
| Clear cache | off | Turn on to delete cached models; they rebuild on next use. |
| Debug logging | off | Logs timings and diagnostics. |
| Instant menu open | on | Shows native fallback menus without the system open animation. |
| Submenu open delay | 150 ms | Hover delay before a submenu opens; 0 = instant, -1 = keep the Windows setting. |
| Move Windows extras | on | Moves the configured Windows extras into the More options submenu. |
| Move third-party handlers | on | Moves third-party shell extension entries into the More options submenu. |
| Keep in the main menu | empty | Comma-separated labels or verbs that stay in the main menu. |
| More options submenu label | `More options` | Label of that submenu. |
| Windows items to move | common list | Comma-separated labels or verbs of Windows items to move into the submenu. |
| Settings hotkey | empty | Optional global hotkey that opens the settings menu (for example `Ctrl+Alt+M`). |

## Configuration

`menu.ini` lives in the mod's storage directory (Windhawk → the mod's details →
Storage). It is created on first run as a fully commented settings list, with
the range and meaning of every key next to it. Invalid values never stop the
file from loading: they are clamped or fall back to defaults, logged as
`menu.ini:<line>: warning: <message>`, and the corrected file is rewritten.

Themes are complete `[appearance]` blocks under `themes\<name>.ini`; they never
inherit from `menu.ini`, so a theme is always self-contained. Edit one directly,
or use the in-menu settings browser.

## Limitations

- Targets `explorer.exe`; other file managers and file dialogs are untouched.
- Menus that contain owner-drawn items, and non-filesystem namespaces, fall
  back to the native menu.
- The custom menu is not exposed to UI Automation; use the classic menu mode
  with a screen reader.
- Tall menus do not scroll; they are capped to the work area.
- Nav-pane menus are cached per tree node for the session and refresh after
  use, so the first open of each node pays the shell's population cost.
- Core menu labels are English; cached extension labels come from the shell
  and are localized.
- Native fallback menus apply the configured submenu delay through the system
  `MenuShowDelay` value while the menu is open (restored on close); Windows
  has no per-menu delay API.

## Feedback

Source code, issues, and the full configuration guide:
<https://github.com/Bolt-Scripts/custom-cached-context-menu>

## License

MIT
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: "Custom (menu.ini)"
  $name: Theme
  $description: Loads the appearance from <storage>\themes\<name>.ini (created from the bundled preset on first use). menu.ini is never modified.
  $options:
  - Custom (menu.ini): Use menu.ini as-is
  - Windows 11 Dark: Fluent dark with translucent blur
  - Windows 11 Light: Fluent light with translucent blur
  - Windows 10 Dark: Flat dark, square corners
  - Windows 10 Light: Flat light, square corners
  - Nord: Arctic blue-grey palette
  - Dracula: Purple and green dark palette
  - Solarized Dark: Low-contrast teal dark palette
  - Gruvbox Dark: Warm retro dark palette
  - One Dark: Atom-style dark palette
  - Cyberpunk: Neon yellow, cyan, and magenta on near-black
  - Synthwave: Retro purple and pink with teal accents
  - Terminal Green: Phosphor terminal green on black
  - Amber CRT: Amber terminal on black
  - Tokyo Night: Blue night palette with violet accents
  - AMOLED Black: True black, no blur or shadow
  - High Contrast: White on black, thick border
- menuMode: "custom"
  $name: Menu mode
  $description: Custom draws the self-rendered menu and falls back to the classic menu after repeated failures. Classic always uses the owner-drawn menu.
  $options:
  - custom: Custom (recommended)
  - classic: Classic owner-drawn
- enableShiftBypass: true
  $name: Bypass keys
  $description: Hold Shift while right-clicking to bypass the mod and get the stock classic menu, or Ctrl for the untouched native menu.
- instantMenuFade: true
  $name: Instant menu open
  $description: Shows native fallback menus (classic mode, fallbacks) without the system open animation, via TPM_NOANIMATION.
- showMoreOptionsItem: true
  $name: Show classic menu item
  $description: Add a "Show classic menu" entry at the bottom of the replacement menu.
- submenuDelayMs: 150
  $name: Submenu open delay
  $description: Milliseconds before a hovered submenu opens while the replacement menu is shown. 0 opens instantly; -1 keeps the Windows setting.
- advancedSubmenuWindows: true
  $name: Move Windows extras
  $description: Move the configured Windows extras into the More options submenu.
- advancedSubmenuThirdParty: true
  $name: Move third-party handlers
  $description: Move third-party shell extension entries into the More options submenu.
- advancedSubmenuItems: "Share, Add to Favorites, Cast to Device, Give access to, Restore previous versions, Pin to Start, Pin to Quick access, Open in Terminal"
  $name: Windows items to move
  $description: Comma-separated labels or verbs of Windows items to move into the submenu.
- advancedSubmenuExclude: ""
  $name: Keep in the main menu
  $description: Comma-separated third-party labels or verbs that stay in the main menu instead of moving into the submenu.
- advancedSubmenuLabel: More options
  $name: More options submenu label
  $description: Label of the submenu that collects extra items.
- settingsHotkey: ""
  $name: Settings hotkey
  $description: Optional global hotkey that opens the appearance settings menu (for example Ctrl+Alt+M). Empty disables it.
- debugLogging: false
  $name: Debug logging
  $description: Log timing and diagnostics for troubleshooting.
- clearCache: false
  $name: Clear cache
  $description: Turn on to delete the cached menu models; they rebuild on next use.
- warmupExtensions: ".txt, .pdf, .zip, .rar, .7z, .jpg, .png, .mp4, .mp3, .docx, .xlsx, .exe, .lnk"
  $name: Warm-up extensions
  $description: Comma-separated file types whose menus are pre-built at Explorer startup.
- warmupDelaySeconds: 5
  $name: Warm-up delay
  $description: Seconds to wait after Explorer starts before warming the cache.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <exdisp.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include <commctrl.h>
#include <commoncontrols.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Windhawk's compiler is clang; other compilers (the local test build) ignore
// the attribute with a warning, so guard it.
#if defined(__clang__)
#define CMO_NO_DESTROY [[clang::no_destroy]]
#else
#define CMO_NO_DESTROY
#endif

// ===========================================================================
// [CMO:Settings] User settings.
// ===========================================================================
namespace cmo {

// The mod's own module handle. GetModuleHandleW(nullptr) would return
// Explorer's, while the window classes and hook procedures live in the mod
// DLL.
HMODULE ModModuleHandle() {
    static HMODULE module = [] {
        HMODULE result = nullptr;
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&ModModuleHandle), &result);
        return result;
    }();
    return module;
}

struct Settings {
    bool enableShiftBypass = true;
    std::wstring theme = L"Custom (menu.ini)";
    int themeIndex = 0;
    std::wstring settingsHotkey;
    int menuMode = 0;
    bool showMoreOptionsItem = true;
    int submenuDelayMs = 150;
    int warmupDelaySeconds = 5;
    bool clearCache = false;
    bool debugLogging = false;
    bool instantMenuFade = true;
    bool advancedSubmenuWindows = true;
    bool advancedSubmenuThirdParty = true;
    std::wstring advancedSubmenuLabel = L"More options";
    std::vector<std::wstring> advancedSubmenuItems;
    std::vector<std::wstring> advancedSubmenuExclude;
};

// Settings are published as immutable snapshots: LoadSettings runs on the
// engine thread while UI and warm-up threads read, and swapping a snapshot
// avoids torn reads of the strings and vectors. The operator-> temporary
// keeps the snapshot alive for the whole expression.
class SettingsRef {
public:
    explicit SettingsRef(std::shared_ptr<const Settings> snapshot)
        : snapshot_(std::move(snapshot)) {}

    const Settings* operator->() const { return snapshot_.get(); }

private:
    std::shared_ptr<const Settings> snapshot_;
};

class SettingsPublisher {
public:
    SettingsPublisher() { Publish(Settings{}); }

    SettingsRef operator->() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return SettingsRef(current_);
    }

    Settings Snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return *current_;
    }

    void Publish(Settings next) {
        auto snapshot = std::make_shared<const Settings>(std::move(next));
        std::lock_guard<std::mutex> lock(mutex_);
        current_ = std::move(snapshot);
    }

private:
    mutable std::mutex mutex_;
    std::shared_ptr<const Settings> current_;
};

inline SettingsPublisher g_settings;

// Set in Wh_ModInit; only the UI thread may touch the render device/caches.

// Defined in [CMO:Themes]; declared here for the settings and theme store.
int ThemeIndexFromName(const std::wstring& name);
inline int g_lastAppliedTheme = 0;

std::wstring TrimWhitespace(const std::wstring& text) {
    const size_t first = text.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return L"";
    }
    const size_t last = text.find_last_not_of(L" \t\r\n");
    return text.substr(first, last - first + 1);
}

// The shell's raw menu labels carry accelerator markers ("Add to &Favorites")
// and trailing ellipses that are never visible; normalize them before
// comparing against anything the user sees or types.
std::wstring NormalizeMenuLabel(std::wstring text) {
    std::wstring out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == L'&') {
            if (i + 1 < text.size() && text[i + 1] == L'&') {
                out += L'&';
                ++i;
            }
            continue;
        }
        out += text[i];
    }
    out = TrimWhitespace(out);
    while (!out.empty() && (out.back() == L'.' || out.back() == 0x2026)) {
        out.pop_back();
    }
    return TrimWhitespace(out);
}

std::vector<std::wstring> ParseAdvancedItems(const std::wstring& text) {
    std::vector<std::wstring> items;
    size_t start = 0;
    while (start <= text.size()) {
        const size_t comma = text.find(L',', start);
        const std::wstring token =
            TrimWhitespace(text.substr(start, comma == std::wstring::npos
                                                  ? std::wstring::npos
                                                  : comma - start));
        if (!token.empty()) {
            items.push_back(token);
        }
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }
    return items;
}

void LoadSettings() {
    Settings next;
    next.enableShiftBypass = Wh_GetIntSetting(L"enableShiftBypass") != 0;

    WindhawkUtils::StringSetting theme =
        WindhawkUtils::StringSetting::make(L"theme");
    next.theme = theme.get()[0] ? theme.get() : L"Custom (menu.ini)";

    WindhawkUtils::StringSetting hotkey =
        WindhawkUtils::StringSetting::make(L"settingsHotkey");
    next.settingsHotkey = hotkey.get();
    next.themeIndex = ThemeIndexFromName(next.theme);

    WindhawkUtils::StringSetting menuMode =
        WindhawkUtils::StringSetting::make(L"menuMode");
    next.menuMode = _wcsicmp(menuMode.get(), L"classic") == 0 ? 1 : 0;

    next.showMoreOptionsItem = Wh_GetIntSetting(L"showMoreOptionsItem") != 0;
    next.submenuDelayMs = Wh_GetIntSetting(L"submenuDelayMs");
    next.warmupDelaySeconds = Wh_GetIntSetting(L"warmupDelaySeconds");
    next.clearCache = Wh_GetIntSetting(L"clearCache") != 0;
    next.debugLogging = Wh_GetIntSetting(L"debugLogging") != 0;
    next.instantMenuFade = Wh_GetIntSetting(L"instantMenuFade") != 0;
    next.advancedSubmenuWindows =
        Wh_GetIntSetting(L"advancedSubmenuWindows") != 0;
    next.advancedSubmenuThirdParty =
        Wh_GetIntSetting(L"advancedSubmenuThirdParty") != 0;

    WindhawkUtils::StringSetting advancedLabel =
        WindhawkUtils::StringSetting::make(L"advancedSubmenuLabel");
    next.advancedSubmenuLabel =
        advancedLabel.get()[0] ? advancedLabel.get() : L"More options";

    WindhawkUtils::StringSetting advancedItems =
        WindhawkUtils::StringSetting::make(L"advancedSubmenuItems");
    next.advancedSubmenuItems = ParseAdvancedItems(advancedItems.get());

    WindhawkUtils::StringSetting advancedExclude =
        WindhawkUtils::StringSetting::make(L"advancedSubmenuExclude");
    next.advancedSubmenuExclude = ParseAdvancedItems(advancedExclude.get());

    g_settings.Publish(std::move(next));
}

}  // namespace cmo

// ===========================================================================
// [CMO:Signature] Context signature: what identifies a cached menu model.
// ===========================================================================
namespace cmo {

enum class Scope : uint8_t { Files, Folders, Background, Desktop, Drive, NavPane, Other };
enum class Shape : uint8_t { Single, Multi };
enum class Variant : uint8_t { Normal, Extended };

inline uint64_t HashCombine(uint64_t seed, uint64_t value) {
    value += 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    return seed ^ value;
}

inline uint64_t HashString(std::wstring_view text) {
    // FNV-1a 64-bit.
    uint64_t hash = 1469598103934665603ULL;
    for (wchar_t c : text) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

struct ContextSignature {
    Scope scope;
    std::wstring typeKey;
    Shape shape;
    Variant variant;

    bool operator==(const ContextSignature&) const = default;

    uint64_t Hash() const {
        uint64_t hash = HashCombine(static_cast<uint64_t>(scope), HashString(typeKey));
        hash = HashCombine(hash, static_cast<uint64_t>(shape));
        hash = HashCombine(hash, static_cast<uint64_t>(variant));
        return hash;
    }
};

// Lowercased extension including the dot (".txt"), or "*" when the name has
// no usable extension: leading-dot names, trailing dots, or no dot at all.
inline std::wstring MakeExtensionKey(std::wstring_view path) {
    size_t nameStart = path.find_last_of(L"\\/");
    nameStart = (nameStart == std::wstring_view::npos) ? 0 : nameStart + 1;

    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos || dot < nameStart || dot == nameStart ||
        dot + 1 >= path.size()) {
        return L"*";
    }

    std::wstring extension(path.substr(dot));
    CharLowerBuffW(extension.data(), static_cast<DWORD>(extension.size()));
    return extension;
}

// The shared type key of a selection: the common extension, "mixed" when the
// selection spans several extensions, or "*" for an empty selection.
inline std::wstring MakeTypeKey(const std::vector<std::wstring>& paths) {
    if (paths.empty()) {
        return L"*";
    }

    std::wstring first = MakeExtensionKey(paths.front());
    for (size_t i = 1; i < paths.size(); ++i) {
        if (MakeExtensionKey(paths[i]) != first) {
            return L"mixed";
        }
    }
    return first;
}

}  // namespace cmo

// ===========================================================================
// [CMO:Model] Menu item / menu model definitions.
// ===========================================================================
namespace cmo {

enum class ItemKind : uint8_t { Command, Submenu, Separator, Header };
enum class ActionKind : uint8_t {
    ViewAction,
    ShellVerb,
    Fallback,
    Submenu,
    NewItem,
    SortBy,
    SortDirection,
    GroupBy,
    GroupDirection,
    CustomCommand,
    Builtin,
};

enum class BuiltinAction : uint8_t {
    None,
    CopyPath,
    OpenNewWindow,
    OpenNewProcess,
    Properties,
    OpenSettings,
};

// Documented view operations, dispatched through IFolderView2 / IShellView.
// The old FCIDM_* view command IDs are not defined by the Windows SDK and
// must not be guessed.
enum class ViewAction : uint32_t {
    None = 0,
    Rename,
    Refresh,
    ViewExtraLargeIcons,
    ViewLargeIcons,
    ViewMediumIcons,
    ViewSmallIcons,
    ViewList,
    ViewDetails,
    ViewTiles,
    ViewContent,
    AutoArrange,
    AlignToGrid,
};

enum ModelFlags : uint32_t {
    kModelNone = 0,
    kModelDefault = 1u << 0,
    kModelChecked = 1u << 1,
    kModelRadio = 1u << 2,
    kModelDisabled = 1u << 3,
    kModelOwnerDraw = 1u << 4,
    kModelExtension = 1u << 6,
    kModelHasOffset = 1u << 7,
    kModelWarmup = 1u << 8,
    kModelThirdParty = 1u << 9,
};

// Settings-UI controls attached to menu items. ControlKind::None is an
// ordinary menu item.
enum class ControlKind : uint8_t {
    None,
    Toggle,
    IntSlider,
    Enum,
    ColorSwatch,
    TextField,
    TextInput,
    ColorArea,
    HueStrip,
    AlphaStrip,
    Action,
    Info,
};

struct ControlSpec {
    ControlKind kind = ControlKind::None;
    std::wstring key;  // schema key, or a reserved @-id
    int minValue = 0;
    int maxValue = 0;
    int step = 1;
    int coarseStep = 10;
    bool unsetCapable = false;
    std::vector<std::wstring> options;  // enums
};

struct MenuItem {
    uint32_t id = 0;
    ItemKind kind = ItemKind::Command;
    ActionKind action = ActionKind::ViewAction;
    std::wstring label;
    std::wstring canonicalVerb;
    uint32_t viewAction = static_cast<uint32_t>(ViewAction::None);
    uint32_t verbOffset = 0;
    uint32_t flags = kModelNone;
    std::wstring iconRef;
    std::wstring targetPath;
    // Index into the ShellNew template list for ActionKind::NewItem.
    uint32_t newIndex = 0;
    // Index into RulesConfig::commands for ActionKind::CustomCommand.
    uint32_t customCommandIndex = 0;
    // Transient display overrides (not serialized): per-item label and marker.
    std::wstring displayLabel;
    int markerOverride = -1;
    // Built-in action for ActionKind::Builtin.
    BuiltinAction builtinAction = BuiltinAction::None;
    // Sort/group field index into kShellPropertyKeys, and sort direction.
    uint32_t sortIndex = 0;
    bool sortAscending = true;
    // Icon size for icon view modes (-1 for the shell default).
    int32_t iconSize = -1;
    // 16x16 BGRA icon captured from the shell's own menu bitmap.
    std::vector<uint8_t> iconPixels;
    // Settings-UI control state (ControlKind::None for ordinary items).
    ControlSpec control;
    std::wstring controlText;
    int controlValue = 0;
    uint32_t controlColor = 0;
    int controlHeight = -1;  // row height override, -1 = metrics.itemHeight
    std::vector<MenuItem> children;
};

struct MenuModel {
    ContextSignature sig;
    std::vector<MenuItem> items;
    uint32_t flags = kModelNone;
    std::vector<std::wstring> handlerModules;
    uint64_t sourceStamp = 0;
};

void FlattenIdsInto(const std::vector<MenuItem>& items, std::vector<uint32_t>& out) {
    for (const MenuItem& item : items) {
        out.push_back(item.id);
        FlattenIdsInto(item.children, out);
    }
}


const MenuItem* FindByIdIn(const std::vector<MenuItem>& items, uint32_t id) {
    for (const MenuItem& item : items) {
        if (item.id == id) {
            return &item;
        }
        if (const MenuItem* found = FindByIdIn(item.children, id)) {
            return found;
        }
    }
    return nullptr;
}

const MenuItem* FindById(const MenuModel& model, uint32_t id) {
    return FindByIdIn(model.items, id);
}

// Verb when the item has one, otherwise the offset to invoke.
std::pair<std::wstring, uint32_t> ChooseInvokeDescriptor(const MenuItem& item) {
    if (!item.canonicalVerb.empty()) {
        return {item.canonicalVerb, 0};
    }
    return {L"", item.verbOffset};
}

std::wstring FormatMultiLabel(std::wstring_view verb, size_t count) {
    if (count <= 1) {
        return std::wstring(verb);
    }
    return std::wstring(verb) + L" " + std::to_wstring(count) + L" items";
}

// Prunes items that cannot be shown faithfully: non-separator items without
// a label, and submenus left with no children after their contents were
// pruned. Runs bottom-up so a submenu whose children are all pruned is
// removed together with them.
void PruneMenuItems(std::vector<MenuItem>& items) {
    for (MenuItem& item : items) {
        PruneMenuItems(item.children);
    }
    std::erase_if(items, [](const MenuItem& item) {
        if (item.kind == ItemKind::Separator) {
            return false;
        }
        if (item.label.empty()) {
            return true;
        }
        return item.kind == ItemKind::Submenu && item.children.empty();
    });
}

bool EqualsIgnoreCase(const std::wstring& left, const std::wstring& right) {
    return !left.empty() && !right.empty() &&
           _wcsicmp(left.c_str(), right.c_str()) == 0;
}

bool LabelsMatchIgnoreCase(const std::wstring& left, const std::wstring& right) {
    if (left.empty() || right.empty()) {
        return false;
    }
    const std::wstring a = NormalizeMenuLabel(left);
    const std::wstring b = NormalizeMenuLabel(right);
    return !a.empty() && !b.empty() && _wcsicmp(a.c_str(), b.c_str()) == 0;
}

// Verbs used by Windows' own menu items. Items with any other verb come from
// third-party shell extensions and belong in the advanced submenu. Windows
// extras that should stay on top are protected by this list and move only
// when named in the setting.
bool IsKnownWindowsVerb(const std::wstring& verb) {
    if (verb.empty()) {
        return false;
    }
    static const wchar_t* kVerbs[] = {
        L"open",       L"opennew",       L"opennewtab",  L"opennewwindow",
        L"openas",     L"openwith",      L"edit",        L"print",
        L"printto",    L"runas",         L"preview",     L"cut",
        L"copy",       L"paste",         L"pastelink",   L"delete",
        L"rename",     L"properties",    L"createshortcut", L"link",
        L"copyaspath", L"sortby",        L"refresh",     L"new",
        L"display",    L"personalize",   L"viewlarge",   L"viewsmall",
        L"viewlist",   L"viewdetails",   L"pintohome",   L"pintohomefile",
        L"pintostartscreen", L"previousversions", L"windows.modernshare",
        L"casttodevice", L"giveaccess",  L"share",       L"includeinlibrary",
    };
    for (const wchar_t* known : kVerbs) {
        if (_wcsicmp(verb.c_str(), known) == 0) {
            return true;
        }
    }
    return false;
}

// Windows extras (configured list or a known Windows verb) sort above
// third-party handlers inside the More options submenu.
bool IsBuiltinExtra(const MenuItem& item,
                    const std::vector<std::wstring>& windowsItems) {
    for (const std::wstring& token : windowsItems) {
        if (LabelsMatchIgnoreCase(item.label, token) ||
            EqualsIgnoreCase(item.canonicalVerb, token)) {
            return true;
        }
    }
    return IsKnownWindowsVerb(item.canonicalVerb);
}

// Removes separators left dangling or duplicated by moving items out.
void CollapseSeparators(std::vector<MenuItem>& items) {
    std::vector<MenuItem> out;
    out.reserve(items.size());
    for (MenuItem& item : items) {
        if (item.kind == ItemKind::Separator &&
            (out.empty() || out.back().kind == ItemKind::Separator)) {
            continue;
        }
        out.push_back(std::move(item));
    }
    while (!out.empty() && out.back().kind == ItemKind::Separator) {
        out.pop_back();
    }
    items = std::move(out);
}

// Moves advanced items into one submenu placed just above the native fallback
// entry. Runs at open time so the setting applies without a rediscovery.

// ===========================================================================
// [CMO:NewMenu] The New submenu is built from ShellNew templates.
// ===========================================================================

// One entry of the New submenu. Enumerated once from the registry (Folder and
// Shortcut are synthetic) and referenced by index from the core model.
struct NewTemplate {
    enum class Kind : uint8_t { Folder, Shortcut, NullFile, Data, FileName, Command };
    Kind kind = Kind::NullFile;
    std::wstring displayName;
    std::wstring extension;      // includes the leading dot
    std::wstring fileName;       // FileName templates
    std::wstring command;        // Command/Shortcut templates
    std::vector<uint8_t> data;   // Data templates
};

std::mutex g_newTemplatesMutex;
std::atomic<bool> g_newTemplatesReady{false};
std::vector<NewTemplate> g_newTemplates;
std::wstring g_shortcutCommand;

std::wstring ExpandEnv(const std::wstring& text) {
    wchar_t buffer[1024] = {};
    if (ExpandEnvironmentStringsW(text.c_str(), buffer, ARRAYSIZE(buffer))) {
        return buffer;
    }
    return text;
}

bool RegKeyExists(HKEY root, const std::wstring& subkey) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS) {
        RegCloseKey(key);
        return true;
    }
    return false;
}

std::wstring ReadRegString(HKEY root, const std::wstring& subkey, const wchar_t* name) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return L"";
    }
    wchar_t buffer[1024] = {};
    DWORD bytes = sizeof(buffer);
    DWORD type = 0;
    const LONG rc =
        RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<LPBYTE>(buffer),
                         &bytes);
    RegCloseKey(key);
    if (rc != ERROR_SUCCESS) {
        return L"";
    }
    if (type == REG_EXPAND_SZ) {
        return ExpandEnv(buffer);
    }
    return type == REG_SZ ? buffer : L"";
}

void AddShellNewTemplate(HKEY root, const std::wstring& classesPrefix,
                         const std::wstring& ext, std::vector<NewTemplate>& out) {
    const std::wstring extKey = classesPrefix + ext;
    std::wstring shellNew = extKey + L"\\ShellNew";
    if (!RegKeyExists(root, shellNew)) {
        const std::wstring progId = ReadRegString(root, extKey, nullptr);
        if (progId.empty()) {
            return;
        }
        shellNew = classesPrefix + progId + L"\\ShellNew";
        if (!RegKeyExists(root, shellNew)) {
            return;
        }
    }

    NewTemplate tmpl;
    tmpl.extension = ext;

    SHFILEINFOW info = {};
    if (SHGetFileInfoW(ext.c_str(), FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                       SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME) &&
        info.szTypeName[0]) {
        tmpl.displayName = info.szTypeName;
    } else {
        tmpl.displayName = ext;
    }
    if (tmpl.displayName.empty()) {
        return;
    }

    HKEY key = nullptr;
    if (RegOpenKeyExW(root, shellNew.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return;
    }

    // Existence checks pass a size pointer; a NULL/NULL query is not reliable
    // for this on all Windows versions.
    DWORD valueType = 0;
    DWORD valueSize = 0;
    if (RegQueryValueExW(key, L"NullFile", nullptr, &valueType, nullptr, &valueSize) ==
        ERROR_SUCCESS) {
        tmpl.kind = NewTemplate::Kind::NullFile;
        out.push_back(std::move(tmpl));
    } else if (RegQueryValueExW(key, L"Data", nullptr, &valueType, nullptr,
                                &valueSize) == ERROR_SUCCESS &&
               valueSize > 0 && valueSize <= 65536) {
        if (valueType == REG_BINARY) {
            tmpl.kind = NewTemplate::Kind::Data;
            tmpl.data.resize(valueSize);
            if (RegQueryValueExW(key, L"Data", nullptr, nullptr, tmpl.data.data(),
                                 &valueSize) == ERROR_SUCCESS) {
                out.push_back(std::move(tmpl));
            }
        } else if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
            // Some templates store the payload as text; the shell converts it
            // to ANSI bytes.
            const std::wstring text = ReadRegString(root, shellNew, L"Data");
            if (!text.empty()) {
                const int needed = WideCharToMultiByte(CP_ACP, 0, text.c_str(), -1,
                                                       nullptr, 0, nullptr, nullptr);
                if (needed > 1) {
                    std::string narrow(static_cast<size_t>(needed - 1), '\0');
                    WideCharToMultiByte(CP_ACP, 0, text.c_str(), -1, narrow.data(),
                                        needed, nullptr, nullptr);
                    tmpl.kind = NewTemplate::Kind::Data;
                    tmpl.data.assign(narrow.begin(), narrow.end());
                    out.push_back(std::move(tmpl));
                }
            }
        }
    } else if (!(tmpl.fileName = ReadRegString(root, shellNew, L"FileName")).empty()) {
        tmpl.kind = NewTemplate::Kind::FileName;
        tmpl.fileName = ExpandEnv(tmpl.fileName);
        out.push_back(std::move(tmpl));
    } else if (!(tmpl.command = ReadRegString(root, shellNew, L"Command")).empty()) {
        tmpl.kind = NewTemplate::Kind::Command;
        out.push_back(std::move(tmpl));
    }
    RegCloseKey(key);
}

// ShellNew keys are looked up exactly like the shell does: the merged
// HKEY_CLASSES_ROOT view first, then the two hives directly (Wine does not
// merge HKCU into HKCR, and a hive key without ShellNew must not shadow the
// other hive's template).
struct ShellNewRoot {
    HKEY root;
    const wchar_t* classesPrefix;  // "" for HKCR, "Software\Classes\" otherwise
};

const ShellNewRoot kShellNewRoots[] = {
    {HKEY_CLASSES_ROOT, L""},
    {HKEY_CURRENT_USER, L"Software\\Classes\\"},
    {HKEY_LOCAL_MACHINE, L"Software\\Classes\\"},
    // Office and other 32-bit installers register here.
    {HKEY_LOCAL_MACHINE, L"Software\\Classes\\WOW6432Node\\"},
};

std::vector<NewTemplate> EnumerateShellNewTemplates() {
    std::vector<NewTemplate> templates;
    std::unordered_set<std::wstring> seenExtensions;
    for (const ShellNewRoot& entry : kShellNewRoots) {
        HKEY classes = entry.root;
        const bool opened = entry.classesPrefix[0] != L'\0';
        if (opened) {
            if (RegOpenKeyExW(entry.root, entry.classesPrefix, 0, KEY_READ,
                              &classes) != ERROR_SUCCESS) {
                continue;
            }
        }
        for (DWORD i = 0;; ++i) {
            wchar_t ext[256] = {};
            DWORD length = ARRAYSIZE(ext);
            const LONG rc = RegEnumKeyExW(classes, i, ext, &length, nullptr, nullptr,
                                          nullptr, nullptr);
            if (rc == ERROR_NO_MORE_ITEMS) {
                break;
            }
            if (rc != ERROR_SUCCESS) {
                continue;  // skip overlong names instead of aborting
            }
            if (ext[0] != L'.') {
                continue;
            }
            std::wstring lower = ext;
            std::transform(lower.begin(), lower.end(), lower.begin(), towlower);
            if (seenExtensions.count(lower)) {
                continue;  // earlier root wins
            }
            const size_t before = templates.size();
            AddShellNewTemplate(entry.root, entry.classesPrefix, ext, templates);
            if (templates.size() > before) {
                seenExtensions.insert(lower);
            }
        }
        if (opened) {
            RegCloseKey(classes);
        }
    }

    std::sort(templates.begin(), templates.end(),
              [](const NewTemplate& a, const NewTemplate& b) {
                  return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
              });

    // Windows shows one entry per type name; keep the first.
    std::vector<NewTemplate> unique;
    std::unordered_set<std::wstring> seenNames;
    for (NewTemplate& tmpl : templates) {
        std::wstring nameKey = tmpl.displayName;
        std::transform(nameKey.begin(), nameKey.end(), nameKey.begin(), towlower);
        if (!seenNames.insert(nameKey).second) {
            continue;
        }
        unique.push_back(std::move(tmpl));
    }
    return unique;
}

// Debug: logs where ShellNew data for the standard types actually lives.
void LogShellNewProbe(const wchar_t* ext) {
    if (!g_settings->debugLogging) {
        return;
    }
    for (const ShellNewRoot& entry : kShellNewRoots) {
        const std::wstring extKey = std::wstring(entry.classesPrefix) + ext;
        const std::wstring shellNew = extKey + L"\\ShellNew";
        const bool extExists = RegKeyExists(entry.root, extKey);
        const bool shellNewExists = RegKeyExists(entry.root, shellNew);
        const std::wstring progId = ReadRegString(entry.root, extKey, nullptr);
        bool progShellNew = false;
        if (!progId.empty()) {
            progShellNew = RegKeyExists(
                entry.root, entry.classesPrefix + progId + L"\\ShellNew");
        }
        const wchar_t* rootName =
            entry.classesPrefix[0] == L'\0'
                ? L"HKCR"
                : (entry.root == HKEY_CURRENT_USER ? L"HKCU" : L"HKLM");
        Wh_Log(L"ShellNew probe %s root=%s ext=%d shellnew=%d prog='%s' progShellNew=%d",
               ext, rootName, extExists, shellNewExists, progId.c_str(), progShellNew);
        if (shellNewExists) {
            HKEY key = nullptr;
            if (RegOpenKeyExW(entry.root, shellNew.c_str(), 0, KEY_READ, &key) ==
                ERROR_SUCCESS) {
                for (DWORD i = 0;; ++i) {
                    wchar_t name[256] = {};
                    DWORD nameLength = ARRAYSIZE(name);
                    DWORD type = 0;
                    DWORD size = 0;
                    if (RegEnumValueW(key, i, name, &nameLength, nullptr, &type, nullptr,
                                      &size) != ERROR_SUCCESS) {
                        break;
                    }
                    Wh_Log(L"  value '%s' type=%lu size=%lu", name, type, size);
                }
                RegCloseKey(key);
            }
        }
    }
}

// Builds the template list once. Warm-up prebuilds it; a first open can build
// it synchronously as a one-time fallback.
void EnsureNewTemplates() {
    if (g_newTemplatesReady.load(std::memory_order_acquire)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_newTemplatesMutex);
    if (g_newTemplatesReady.load(std::memory_order_relaxed)) {
        return;
    }

    if (g_settings->debugLogging) {
        for (const wchar_t* probe : {L".txt", L".bmp", L".rtf", L".pptx"}) {
            LogShellNewProbe(probe);
        }
    }

    std::vector<NewTemplate> templates = EnumerateShellNewTemplates();
    std::wstring shortcutCommand =
        L"%SystemRoot%\\System32\\rundll32.exe appwiz.cpl,NewLinkHere %1";
    std::vector<NewTemplate> filtered;
    for (NewTemplate& tmpl : templates) {
        if (_wcsicmp(tmpl.extension.c_str(), L".lnk") == 0) {
            if (tmpl.kind == NewTemplate::Kind::Command && !tmpl.command.empty()) {
                shortcutCommand = tmpl.command;
            }
            continue;
        }
        filtered.push_back(std::move(tmpl));
    }

    // "Compressed (zipped) Folder" is provided by the zip folder extension,
    // not always by a ShellNew key. Add it ourselves when the registry has no
    // .zip template: an empty ZIP end-of-central-directory record.
    bool hasZip = false;
    for (const NewTemplate& tmpl : filtered) {
        if (_wcsicmp(tmpl.extension.c_str(), L".zip") == 0) {
            hasZip = true;
            break;
        }
    }
    if (!hasZip) {
        NewTemplate zip;
        zip.kind = NewTemplate::Kind::Data;
        zip.displayName = L"Compressed (zipped) Folder";
        zip.extension = L".zip";
        zip.data = {0x50, 0x4B, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        filtered.push_back(std::move(zip));
    }

    // Windows 11 registers some built-in New items through the AppX/MRT
    // system instead of ShellNew; add the standard Windows types when the
    // registry provides nothing for them.
    struct BuiltinTemplate {
        const wchar_t* extension;
        const wchar_t* displayName;
        NewTemplate::Kind kind;
        const char* data;  // optional payload
    };
    const BuiltinTemplate kBuiltins[] = {
        {L".txt", L"Text Document", NewTemplate::Kind::NullFile, nullptr},
        {L".bmp", L"Bitmap image", NewTemplate::Kind::NullFile, nullptr},
        {L".rtf", L"Rich Text Document", NewTemplate::Kind::Data, "{\\rtf1}"},
    };
    for (const BuiltinTemplate& builtin : kBuiltins) {
        bool present = false;
        for (const NewTemplate& tmpl : filtered) {
            if (_wcsicmp(tmpl.extension.c_str(), builtin.extension) == 0) {
                present = true;
                break;
            }
        }
        if (present) {
            continue;
        }
        NewTemplate tmpl;
        tmpl.kind = builtin.kind;
        tmpl.displayName = builtin.displayName;
        tmpl.extension = builtin.extension;
        if (builtin.data) {
            tmpl.data.assign(builtin.data, builtin.data + strlen(builtin.data));
        }
        filtered.push_back(std::move(tmpl));
    }

    std::sort(filtered.begin(), filtered.end(),
              [](const NewTemplate& a, const NewTemplate& b) {
                  return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
              });

    std::vector<NewTemplate> built;
    NewTemplate folder;
    folder.kind = NewTemplate::Kind::Folder;
    folder.displayName = L"Folder";
    built.push_back(std::move(folder));
    NewTemplate shortcut;
    shortcut.kind = NewTemplate::Kind::Shortcut;
    shortcut.displayName = L"Shortcut";
    shortcut.command = ExpandEnv(shortcutCommand);
    built.push_back(std::move(shortcut));
    for (NewTemplate& tmpl : filtered) {
        built.push_back(std::move(tmpl));
    }

    g_shortcutCommand = std::move(shortcutCommand);
    g_newTemplates = std::move(built);
    g_newTemplatesReady.store(true, std::memory_order_release);

    if (g_settings->debugLogging) {
        for (const NewTemplate& tmpl : g_newTemplates) {
            Wh_Log(L"New template: ext='%s' name='%s' kind=%d", tmpl.extension.c_str(),
                   tmpl.displayName.c_str(), static_cast<int>(tmpl.kind));
        }
    }
}

void BuildNewMenuChildren(std::vector<MenuItem>& children, uint32_t& nextId) {
    // Never build templates on the open path: the warm-up thread prebuilds
    // them, and blocking on its registry walk can stall the menu right after
    // Explorer starts. New appears once the list is ready.
    if (!g_newTemplatesReady.load(std::memory_order_acquire)) {
        return;
    }
    for (size_t i = 0; i < g_newTemplates.size(); ++i) {
        const NewTemplate& tmpl = g_newTemplates[i];
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Command;
        item.action = ActionKind::NewItem;
        item.label = tmpl.displayName;
        item.newIndex = static_cast<uint32_t>(i);
        if (tmpl.kind == NewTemplate::Kind::Folder) {
            item.iconRef = L"@ext:folder";
        } else if (tmpl.kind == NewTemplate::Kind::Shortcut) {
            item.iconRef = L"@ext:.lnk";
        } else {
            item.iconRef = L"@ext:" + tmpl.extension;
        }
        children.push_back(std::move(item));
    }
}

// Returns a path in `directory` that does not exist yet, using Windows'
// "New folder (2)" naming pattern.
std::wstring MakeUniquePath(const std::wstring& directory, const std::wstring& baseName,
                            const std::wstring& extension) {
    std::wstring candidate = directory + L"\\" + baseName + extension;
    if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return candidate;
    }
    for (int i = 2; i < 1000; ++i) {
        candidate = directory + L"\\" + baseName + L" (" + std::to_wstring(i) + L")" +
                    extension;
        if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return candidate;
        }
    }
    return L"";
}

// Creates one New item in `directory`; `createdPath` is empty for commands
// that create their item themselves (the shortcut wizard).
bool CreateNewItemInFolder(const NewTemplate& tmpl, const std::wstring& directory,
                           std::wstring& createdPath) {
    createdPath.clear();
    if (directory.empty()) {
        return false;
    }

    if (tmpl.kind == NewTemplate::Kind::Folder) {
        const std::wstring path = MakeUniquePath(directory, L"New folder", L"");
        if (path.empty() || !CreateDirectoryW(path.c_str(), nullptr)) {
            return false;
        }
        createdPath = path;
        return true;
    }

    if (tmpl.kind == NewTemplate::Kind::Shortcut ||
        tmpl.kind == NewTemplate::Kind::Command) {
        // Explorer creates the new item first and passes its full path to the
        // ShellNew command, quoted so paths with spaces survive.
        std::wstring itemPath;
        if (tmpl.kind == NewTemplate::Kind::Shortcut) {
            itemPath = MakeUniquePath(directory, L"New shortcut", L".lnk");
            if (itemPath.empty()) {
                return false;
            }
            HANDLE file = CreateFileW(itemPath.c_str(), GENERIC_WRITE, 0, nullptr,
                                      CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE) {
                return false;
            }
            CloseHandle(file);
        }

        std::wstring command = ExpandEnv(tmpl.command);
        const size_t placeholder = command.find(L"%1");
        if (placeholder != std::wstring::npos) {
            const std::wstring argument = itemPath.empty() ? directory : itemPath;
            command.replace(placeholder, 2, L"\"" + argument + L"\"");
        }
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        std::vector<wchar_t> mutableCommand(command.begin(), command.end());
        mutableCommand.push_back(L'\0');
        const BOOL ok = CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr,
                                       FALSE, 0, nullptr, directory.c_str(), &si, &pi);
        if (ok) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
            if (!itemPath.empty()) {
                createdPath = itemPath;
            }
        } else if (!itemPath.empty()) {
            // Do not leave the empty placeholder behind when the wizard
            // could not be started.
            DeleteFileW(itemPath.c_str());
        }
        return ok != FALSE;
    }

    const std::wstring baseName = L"New " + tmpl.displayName;
    const std::wstring path = MakeUniquePath(directory, baseName, tmpl.extension);
    if (path.empty()) {
        return false;
    }

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (tmpl.kind == NewTemplate::Kind::Data && !tmpl.data.empty()) {
        DWORD written = 0;
        WriteFile(file, tmpl.data.data(), static_cast<DWORD>(tmpl.data.size()), &written,
                  nullptr);
    }
    CloseHandle(file);

    if (tmpl.kind == NewTemplate::Kind::FileName && !tmpl.fileName.empty()) {
        if (!CopyFileW(tmpl.fileName.c_str(), path.c_str(), FALSE)) {
            DeleteFileW(path.c_str());
            return false;
        }
    }
    createdPath = path;
    return true;
}

// Logs any unlabeled item with its full descriptor so the extension behavior
// can be identified from a single run.
void DumpSuspiciousItems(const std::vector<MenuItem>& items, int depth) {
    if (!g_settings->debugLogging) {
        return;
    }
    for (const MenuItem& item : items) {
        if (item.kind != ItemKind::Separator && item.label.empty()) {
            Wh_Log(L"[suspicious d%d] kind=%d action=%d flags=%04X offset=%u "
                   L"verb='%s' children=%zu",
                   depth, static_cast<int>(item.kind), static_cast<int>(item.action),
                   item.flags, item.verbOffset, item.canonicalVerb.c_str(),
                   item.children.size());
        }
        DumpSuspiciousItems(item.children, depth + 1);
    }
}

// ===========================================================================
// [CMO:CustomConfig] v2 config file structures and parser.
// ===========================================================================

// Animation effects combine as a bitmask; easings shape the progress curve.
constexpr uint32_t kAnimFade = 1u << 0;
constexpr uint32_t kAnimSlide = 1u << 1;
constexpr uint32_t kAnimScale = 1u << 2;
constexpr uint32_t kAnimDissolve = 1u << 3;
constexpr uint32_t kAnimCrt = 1u << 4;
constexpr uint32_t kAnimUnfold = 1u << 5;

// Overlay effects combine the same way; they are drawn over the menu content.
constexpr uint32_t kOverlayNoise = 1u << 0;
constexpr uint32_t kOverlayPlasma = 1u << 1;
constexpr uint32_t kOverlayHue = 1u << 2;
constexpr uint32_t kOverlayGlow = 1u << 3;
constexpr uint32_t kOverlayScanlines = 1u << 4;
constexpr uint32_t kOverlayVignette = 1u << 5;

enum class AnimEasing : uint8_t { Linear, EaseOut, EaseInOut, Back, Bounce, Elastic };
enum class MarkerStyle : uint8_t { Dot, Check, Bar, None };
enum class FontWeightKind : uint8_t { Normal, Semibold, Bold };
enum class FontStyleKind : uint8_t { Normal, Italic };
enum class AcceleratorMode : uint8_t { Underline, Strip, Raw };

struct CornerRadii {
    int topLeft = 0;
    int topRight = 0;
    int bottomRight = 0;
    int bottomLeft = 0;
};

struct Appearance {
    uint32_t background = 0xF01E1E1E;
    bool blur = true;
    int blurStrength = 12;
    int cornerRadius = 8;
    uint32_t border = 0x22FFFFFF;
    int borderWidth = 1;
    bool shadow = true;
    int shadowSize = 0;
    int shadowOffsetX = 0;
    int shadowOffsetY = 4;
    std::wstring fontFace = L"Segoe UI";
    float fontSize = 11.0f;
    int itemHeight = 28;
    int iconSize = 16;
    int padding = 6;
    uint32_t separator = 0x18FFFFFF;
    uint32_t hoverBackground = 0x14FFFFFF;
    uint32_t pressedBackground = 0x22FFFFFF;
    uint32_t textColor = 0xFFFFFFFF;
    uint32_t disabledTextColor = 0x66FFFFFF;
    uint32_t submenuArrow = 0x99FFFFFF;
    uint32_t animationOpen = kAnimFade;
    uint32_t animationClose = kAnimFade;
    // Parser state for the deprecated `animation` alias; not schema values.
    bool hasAnimationOpen = false;
    bool hasAnimationClose = false;
    int animationDuration = 120;
    int animationCloseDuration = 0;
    int animationFrameMs = 15;
    AnimEasing animationEasing = AnimEasing::Linear;
    int slideOffsetX = 12;
    int slideOffsetY = 0;
    int scaleFrom = 92;
    bool animationAnchorAtCursor = true;
    bool animateSubmenus = false;
    int verticalPadding = 4;
    int minWidth = 0;
    int maxWidth = 0;
    int itemPadding = -1;
    int separatorSpacing = 0;
    int markerWidth = 14;
    FontWeightKind fontWeight = FontWeightKind::Normal;
    FontStyleKind fontStyle = FontStyleKind::Normal;
    CornerRadii cornerRadii;
    bool hasCornerRadii = false;
    int shadowOpacity = 120;
    uint32_t shadowColor = 0xFF000000;
    bool shadowAdaptive = true;
    int shadowBlur = 18;
    MarkerStyle marker = MarkerStyle::Dot;
    uint32_t markerColor = 0xFFFFFFFF;
    bool hasMarkerColor = false;
    uint32_t headerColor = 0x66FFFFFF;
    bool hasHeaderColor = false;
    AcceleratorMode acceleratorMode = AcceleratorMode::Underline;
    // Overlays drawn over the menu content.
    uint32_t overlay = 0;
    int overlayIntensity = 50;
    int overlaySpeed = 100;
    int overlaySize = 100;
    int overlayFrameMs = 16;
    bool overlayAnimate = false;
};

// One animation frame: opacities, translation, and scale for the panel.
struct AnimationFrame {
    float opacity = 1.0f;
    float contentOpacity = 1.0f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    // CRT-style flash/glow intensity (0 = none).
    float brightness = 0.0f;
    // Scale about the panel center instead of the configured anchor.
    bool center = false;
};

struct ConfigParseError {
    int line = 0;
    std::wstring message;
};

enum class PredicateField : uint8_t { Label, Verb, Ext, Scope, Multi, ThirdParty };

struct Predicate {
    PredicateField field = PredicateField::Label;
    std::vector<std::wstring> values;
};

struct PredicateExpr {
    std::vector<Predicate> all;
};

enum class RuleKind : uint8_t { Hide, Keep, Move };

struct Rule {
    RuleKind kind = RuleKind::Hide;
    PredicateExpr match;
    std::wstring destination;
};

enum class RunAs : uint8_t { None, Admin };
enum class ShowWindow : uint8_t { Normal, Maximized, Minimized, Hidden };
enum class CommandSeparator : uint8_t { None, Before, After };
enum class CommandType : uint8_t { Command, Separator, Header };
enum class SubmenuPositionKind : uint8_t { Top, Bottom, After, Before };

struct CustomCommand {
    std::wstring label;
    std::wstring command;
    CommandType type = CommandType::Command;
    BuiltinAction action = BuiltinAction::None;
    std::wstring workingDir;
    std::wstring iconRef;
    PredicateExpr match;
    RunAs runAs = RunAs::None;
    ShowWindow showWindow = ShowWindow::Normal;
    CommandSeparator separator = CommandSeparator::None;
    std::wstring menuPath;
};

struct CustomSubmenu {
    std::wstring name;
    std::wstring iconRef;
    SubmenuPositionKind position = SubmenuPositionKind::Bottom;
    std::wstring positionLabel;
    PredicateExpr match;
};

struct ItemOverride {
    PredicateExpr match;
    std::wstring iconRef;
    std::wstring label;
    bool hasIcon = false;
    bool hasLabel = false;
    bool hasMarker = false;
    MarkerStyle marker = MarkerStyle::Dot;
};

struct RulesConfig {
    Appearance appearance;
    Appearance lightAppearance;
    Appearance darkAppearance;
    bool hasLightAppearance = false;
    bool hasDarkAppearance = false;
    std::vector<Rule> rules;
    std::vector<CustomCommand> commands;
    std::vector<CustomSubmenu> submenus;
    std::vector<ItemOverride> overrides;
    int schemaVersion = 0;
    uint64_t revision = 0;
};

std::wstring ToLowerCopy(const std::wstring& text) {
    std::wstring lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), towlower);
    return lower;
}

std::wstring FormatColorRgba(uint32_t argb) {
    return std::to_wstring((argb >> 16) & 0xFF) + L", " +
           std::to_wstring((argb >> 8) & 0xFF) + L", " +
           std::to_wstring(argb & 0xFF) + L", " +
           std::to_wstring((argb >> 24) & 0xFF);
}

// Accepts "#RRGGBB"/"#AARRGGBB" or readable "R, G, B" / "R, G, B, A" decimals.
bool ParseColor(const std::wstring& text, uint32_t& argb) {
    const std::wstring trimmed = TrimWhitespace(text);
    if (!trimmed.empty() && trimmed[0] == L'#') {
        if (trimmed.size() != 7 && trimmed.size() != 9) {
            return false;
        }
        uint32_t value = 0;
        for (size_t i = 1; i < trimmed.size(); ++i) {
            const wchar_t c = trimmed[i];
            uint32_t digit = 0;
            if (c >= L'0' && c <= L'9') {
                digit = static_cast<uint32_t>(c - L'0');
            } else if (c >= L'a' && c <= L'f') {
                digit = 10u + static_cast<uint32_t>(c - L'a');
            } else if (c >= L'A' && c <= L'F') {
                digit = 10u + static_cast<uint32_t>(c - L'A');
            } else {
                return false;
            }
            value = (value << 4) | digit;
        }
        argb = trimmed.size() == 7 ? (0xFF000000u | value) : value;
        return true;
    }

    int values[4] = {0, 0, 0, 255};
    int count = 0;
    size_t start = 0;
    while (start <= trimmed.size() && count < 4) {
        const size_t comma = trimmed.find(L',', start);
        const std::wstring part = TrimWhitespace(
            trimmed.substr(start, comma == std::wstring::npos ? std::wstring::npos
                                                              : comma - start));
        if (part.empty()) {
            return false;
        }
        wchar_t* end = nullptr;
        const long parsed = wcstol(part.c_str(), &end, 10);
        if (end == part.c_str() || *end != L'\0' || parsed < 0 || parsed > 255) {
            return false;
        }
        values[count++] = static_cast<int>(parsed);
        if (comma == std::wstring::npos) {
            break;
        }
        if (count == 4) {
            return false;  // a fifth component follows
        }
        start = comma + 1;
    }
    if (count != 3 && count != 4) {
        return false;
    }
    if (count == 3) {
        values[3] = 255;
    }
    argb = (static_cast<uint32_t>(values[3]) << 24) |
           (static_cast<uint32_t>(values[0]) << 16) |
           (static_cast<uint32_t>(values[1]) << 8) |
           static_cast<uint32_t>(values[2]);
    return true;
}

bool ParseBool(const std::wstring& text, bool& value) {
    const std::wstring trimmed = ToLowerCopy(TrimWhitespace(text));
    if (trimmed == L"true" || trimmed == L"1" || trimmed == L"yes") {
        value = true;
        return true;
    }
    if (trimmed == L"false" || trimmed == L"0" || trimmed == L"no") {
        value = false;
        return true;
    }
    return false;
}

bool ParseIntValue(const std::wstring& text, int& value) {
    const std::wstring trimmed = TrimWhitespace(text);
    if (trimmed.empty()) {
        return false;
    }
    wchar_t* end = nullptr;
    const long parsed = wcstol(trimmed.c_str(), &end, 10);
    if (end == trimmed.c_str() || *end != L'\0') {
        return false;
    }
    value = static_cast<int>(parsed);
    return true;
}

bool ParseFont(const std::wstring& text, std::wstring& face, float& size) {
    const size_t comma = text.find(L',');
    if (comma == std::wstring::npos) {
        return false;
    }
    face = TrimWhitespace(text.substr(0, comma));
    if (face.size() >= 2 && face.front() == L'"' && face.back() == L'"') {
        face = face.substr(1, face.size() - 2);
    }
    const std::wstring sizeText = TrimWhitespace(text.substr(comma + 1));
    if (face.empty() || sizeText.empty()) {
        return false;
    }
    wchar_t* end = nullptr;
    const double parsed = wcstod(sizeText.c_str(), &end);
    if (end == sizeText.c_str() || *end != L'\0' || parsed <= 0.0) {
        return false;
    }
    size = static_cast<float>(parsed);
    return true;
}

// Parses a comma-separated effect list ("fade, slide"). Returns false when a
// token is unknown; recognized tokens are still stored so the caller can warn
// and keep them.
bool ParseAnimationEffects(const std::wstring& text, uint32_t& effects) {
    effects = 0;
    const std::wstring trimmed = TrimWhitespace(text);
    if (trimmed.empty() || ToLowerCopy(trimmed) == L"none") {
        return true;
    }
    bool allKnown = true;
    size_t pos = 0;
    while (pos <= trimmed.size()) {
        const size_t comma = trimmed.find(L',', pos);
        const std::wstring token = ToLowerCopy(TrimWhitespace(
            comma == std::wstring::npos ? trimmed.substr(pos)
                                        : trimmed.substr(pos, comma - pos)));
        if (token == L"fade") {
            effects |= kAnimFade;
        } else if (token == L"slide") {
            effects |= kAnimSlide;
        } else if (token == L"scale") {
            effects |= kAnimScale;
        } else if (token == L"dissolve") {
            effects |= kAnimDissolve;
        } else if (token == L"crt") {
            effects |= kAnimCrt;
        } else if (token == L"unfold") {
            effects |= kAnimUnfold;
        } else if (token != L"none" && !token.empty()) {
            allKnown = false;
        }
        if (comma == std::wstring::npos) {
            break;
        }
        pos = comma + 1;
    }
    return allKnown;
}

std::wstring AnimationEffectsText(uint32_t effects) {
    std::wstring text;
    auto append = [&](uint32_t bit, const wchar_t* name) {
        if ((effects & bit) == 0) {
            return;
        }
        if (!text.empty()) {
            text += L", ";
        }
        text += name;
    };
    append(kAnimFade, L"fade");
    append(kAnimSlide, L"slide");
    append(kAnimScale, L"scale");
    append(kAnimDissolve, L"dissolve");
    append(kAnimCrt, L"crt");
    append(kAnimUnfold, L"unfold");
    return text.empty() ? std::wstring(L"none") : text;
}

bool ParseOverlayEffects(const std::wstring& text, uint32_t& effects) {
    effects = 0;
    const std::wstring trimmed = TrimWhitespace(text);
    if (trimmed.empty() || ToLowerCopy(trimmed) == L"none") {
        return true;
    }
    bool allKnown = true;
    size_t pos = 0;
    while (pos <= trimmed.size()) {
        const size_t comma = trimmed.find(L',', pos);
        const std::wstring token = ToLowerCopy(TrimWhitespace(
            comma == std::wstring::npos ? trimmed.substr(pos)
                                        : trimmed.substr(pos, comma - pos)));
        if (token == L"noise") {
            effects |= kOverlayNoise;
        } else if (token == L"plasma") {
            effects |= kOverlayPlasma;
        } else if (token == L"hue") {
            effects |= kOverlayHue;
        } else if (token == L"glow") {
            effects |= kOverlayGlow;
        } else if (token == L"scanlines") {
            effects |= kOverlayScanlines;
        } else if (token == L"vignette") {
            effects |= kOverlayVignette;
        } else if (token != L"none" && !token.empty()) {
            allKnown = false;
        }
        if (comma == std::wstring::npos) {
            break;
        }
        pos = comma + 1;
    }
    return allKnown;
}

std::wstring OverlayEffectsText(uint32_t effects) {
    std::wstring text;
    auto append = [&](uint32_t bit, const wchar_t* name) {
        if ((effects & bit) == 0) {
            return;
        }
        if (!text.empty()) {
            text += L", ";
        }
        text += name;
    };
    append(kOverlayNoise, L"noise");
    append(kOverlayPlasma, L"plasma");
    append(kOverlayHue, L"hue");
    append(kOverlayGlow, L"glow");
    append(kOverlayScanlines, L"scanlines");
    append(kOverlayVignette, L"vignette");
    return text.empty() ? std::wstring(L"none") : text;
}

bool ParseAnimEasing(const std::wstring& text, AnimEasing& easing) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"linear") {
        easing = AnimEasing::Linear;
        return true;
    }
    if (lower == L"easeout") {
        easing = AnimEasing::EaseOut;
        return true;
    }
    if (lower == L"easeinout") {
        easing = AnimEasing::EaseInOut;
        return true;
    }
    if (lower == L"back") {
        easing = AnimEasing::Back;
        return true;
    }
    if (lower == L"bounce") {
        easing = AnimEasing::Bounce;
        return true;
    }
    if (lower == L"elastic") {
        easing = AnimEasing::Elastic;
        return true;
    }
    return false;
}

bool ParseAnimationAnchor(const std::wstring& text, bool& atCursor) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"cursor") {
        atCursor = true;
        return true;
    }
    if (lower == L"center") {
        atCursor = false;
        return true;
    }
    return false;
}

bool ParseMarkerStyle(const std::wstring& text, MarkerStyle& style) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"dot") {
        style = MarkerStyle::Dot;
        return true;
    }
    if (lower == L"check") {
        style = MarkerStyle::Check;
        return true;
    }
    if (lower == L"bar") {
        style = MarkerStyle::Bar;
        return true;
    }
    if (lower == L"none") {
        style = MarkerStyle::None;
        return true;
    }
    return false;
}

bool ParseFontWeight(const std::wstring& text, FontWeightKind& weight) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"normal") {
        weight = FontWeightKind::Normal;
        return true;
    }
    if (lower == L"semibold") {
        weight = FontWeightKind::Semibold;
        return true;
    }
    if (lower == L"bold") {
        weight = FontWeightKind::Bold;
        return true;
    }
    return false;
}

bool ParseFontStyle(const std::wstring& text, FontStyleKind& style) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"normal") {
        style = FontStyleKind::Normal;
        return true;
    }
    if (lower == L"italic") {
        style = FontStyleKind::Italic;
        return true;
    }
    return false;
}

bool ParseAcceleratorMode(const std::wstring& text, AcceleratorMode& mode) {
    const std::wstring lower = ToLowerCopy(TrimWhitespace(text));
    if (lower == L"underline") {
        mode = AcceleratorMode::Underline;
        return true;
    }
    if (lower == L"strip") {
        mode = AcceleratorMode::Strip;
        return true;
    }
    if (lower == L"raw") {
        mode = AcceleratorMode::Raw;
        return true;
    }
    return false;
}

bool ParseBoundedInt(const std::wstring& text, int& value, int minValue,
                     int maxValue) {
    return ParseIntValue(text, value) && value >= minValue && value <= maxValue;
}

bool ParseCornerRadii(const std::wstring& value, CornerRadii& radii) {
    int values[4] = {};
    size_t start = 0;
    int count = 0;
    while (start <= value.size() && count < 4) {
        const size_t comma = value.find(L',', start);
        const std::wstring part = TrimWhitespace(
            value.substr(start, comma == std::wstring::npos ? std::wstring::npos
                                                            : comma - start));
        if (part.empty() ||
            !ParseBoundedInt(part, values[count], 0, 256)) {
            return false;
        }
        ++count;
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }
    if (count != 4) {
        return false;
    }
    // Reject a trailing extra value.
    if (value.find(L',', start) != std::wstring::npos) {
        return false;
    }
    radii.topLeft = values[0];
    radii.topRight = values[1];
    radii.bottomRight = values[2];
    radii.bottomLeft = values[3];
    return true;
}

enum class SettingType : uint8_t { Bool, Int, Color, Font, Enum, IntList, EffectList };

// The build's schema version; bump when a row is added.
constexpr int kConfigSchemaVersion = 8;

struct ConfigSchemaEntry {
    const wchar_t* section;
    const wchar_t* key;
    const wchar_t* group;
    SettingType type;
    const wchar_t* defaultValue;
    const wchar_t* validValues;  // Enum/IntList values, else nullptr
    int minValue;                // Int only
    int maxValue;                // Int only
    const wchar_t* description;
    int sinceVersion;
    bool unset;  // the default means "unset"; emitted commented
};

const ConfigSchemaEntry kAppearanceSchema[] = {
    {L"appearance", L"background", L"Colors", SettingType::Color, L"#F01E1E1E", nullptr, 0, 0,
     L"Panel background color; also the blur tint.", 1, false},
    {L"appearance", L"border", L"Colors", SettingType::Color, L"#22FFFFFF", nullptr, 0, 0,
     L"Border color.", 1, false},
    {L"appearance", L"separator", L"Colors", SettingType::Color, L"#18FFFFFF", nullptr, 0, 0,
     L"Separator line color.", 1, false},
    {L"appearance", L"hoverBackground", L"Colors", SettingType::Color, L"#14FFFFFF", nullptr, 0, 0,
     L"Hovered item background.", 1, false},
    {L"appearance", L"pressedBackground", L"Colors", SettingType::Color, L"#22FFFFFF", nullptr, 0, 0,
     L"Pressed item background.", 1, false},
    {L"appearance", L"textColor", L"Colors", SettingType::Color, L"#FFFFFFFF", nullptr, 0, 0,
     L"Item text color.", 1, false},
    {L"appearance", L"disabledTextColor", L"Colors", SettingType::Color, L"#66FFFFFF", nullptr, 0, 0,
     L"Disabled item text color.", 1, false},
    {L"appearance", L"submenuArrow", L"Colors", SettingType::Color, L"#99FFFFFF", nullptr, 0, 0,
     L"Submenu arrow color.", 1, false},
    {L"appearance", L"headerColor", L"Colors", SettingType::Color, L"#66FFFFFF", nullptr, 0, 0,
     L"Header text color; defaults to disabledTextColor.", 1, true},
    {L"appearance", L"cornerRadius", L"Layout", SettingType::Int, L"8", nullptr, 0, 256,
     L"Corner radius in pixels.", 1, false},
    {L"appearance", L"cornerRadii", L"Layout", SettingType::IntList, L"2, 4, 6, 8", L"tl,tr,br,bl", 0, 0,
     L"Per-corner radii; overrides cornerRadius.", 1, true},
    {L"appearance", L"borderWidth", L"Layout", SettingType::Int, L"1", nullptr, 0, 64,
     L"Border width in pixels (0 hides it).", 1, false},
    {L"appearance", L"padding", L"Layout", SettingType::Int, L"6", nullptr, 0, 256,
     L"Base padding inside the panel.", 1, false},
    {L"appearance", L"itemPadding", L"Layout", SettingType::Int, L"6", nullptr, 0, 256,
     L"Horizontal inset of item content; defaults to padding.", 1, true},
    {L"appearance", L"verticalPadding", L"Layout", SettingType::Int, L"4", nullptr, 0, 256,
     L"Padding above and below the items.", 1, false},
    {L"appearance", L"itemHeight", L"Layout", SettingType::Int, L"28", nullptr, 1, 256,
     L"Item height in pixels.", 1, false},
    {L"appearance", L"iconSize", L"Layout", SettingType::Int, L"16", nullptr, 1, 256,
     L"Icon size in pixels.", 1, false},
    {L"appearance", L"minWidth", L"Layout", SettingType::Int, L"0", nullptr, 0, 4096,
     L"Minimum panel width (0 = automatic).", 1, false},
    {L"appearance", L"maxWidth", L"Layout", SettingType::Int, L"0", nullptr, 0, 4096,
     L"Maximum panel width (0 = unlimited).", 1, false},
    {L"appearance", L"separatorSpacing", L"Layout", SettingType::Int, L"0", nullptr, 0, 256,
     L"Extra space above and below separators.", 1, false},
    {L"appearance", L"marker", L"Selection marker", SettingType::Enum, L"dot", L"dot|check|bar|none", 0, 0,
     L"Selection marker style.", 1, false},
    {L"appearance", L"markerWidth", L"Selection marker", SettingType::Int, L"14", nullptr, 0, 256,
     L"Width of the selection marker column.", 1, false},
    {L"appearance", L"markerColor", L"Selection marker", SettingType::Color, L"#FFFFFFFF", nullptr, 0, 0,
     L"Marker color; defaults to textColor.", 1, true},
    {L"appearance", L"font", L"Text", SettingType::Font, L"Segoe UI, 11", nullptr, 0, 0,
     L"Text font face and size (size is in pixels at 96 DPI).", 1, false},
    {L"appearance", L"fontWeight", L"Text", SettingType::Enum, L"normal", L"normal|semibold|bold", 0, 0,
     L"Text weight.", 1, false},
    {L"appearance", L"fontStyle", L"Text", SettingType::Enum, L"normal", L"normal|italic", 0, 0,
     L"Text style.", 1, false},
    {L"appearance", L"showAccelerators", L"Text", SettingType::Enum, L"underline", L"underline|strip|raw", 0, 0,
     L"Mnemonic handling.", 1, false},
    {L"appearance", L"shadow", L"Shadow", SettingType::Bool, L"true", nullptr, 0, 0,
     L"Draw the drop shadow.", 1, false},
    {L"appearance", L"shadowSize", L"Shadow", SettingType::Int, L"0", nullptr, 0, 64,
     L"Shadow spread in pixels.", 1, false},
    {L"appearance", L"shadowOffsetX", L"Shadow", SettingType::Int, L"0", nullptr, -32,
     32, L"Shadow horizontal offset in pixels.", 3, false},
    {L"appearance", L"shadowOffsetY", L"Shadow", SettingType::Int, L"4", nullptr, -32,
     32, L"Shadow vertical offset in pixels.", 3, false},
    {L"appearance", L"shadowOpacity", L"Shadow", SettingType::Int, L"120", nullptr, 0, 255,
     L"Shadow alpha (0-255).", 1, false},
    {L"appearance", L"shadowAdaptive", L"Shadow", SettingType::Bool, L"true",
     nullptr, 0, 0,
     L"Fade the shadow and tint it toward dark backdrops instead of a black halo.",
     8, false},
    {L"appearance", L"shadowColor", L"Shadow", SettingType::Color, L"#FF000000", nullptr, 0, 0,
     L"Shadow color; its alpha multiplies shadowOpacity.", 5, false},
    {L"appearance", L"shadowBlur", L"Shadow", SettingType::Int, L"18", nullptr, 0, 64,
     L"Shadow blur radius in pixels.", 1, false},
    {L"appearance", L"blur", L"Effects", SettingType::Bool, L"true", nullptr, 0, 0,
     L"Blur the screen behind the menu.", 1, false},
    {L"appearance", L"blurStrength", L"Effects", SettingType::Int, L"12", nullptr, 0, 64,
     L"Blur strength.", 1, false},
    {L"appearance", L"overlay", L"Effects", SettingType::EffectList, L"none",
     L"none|noise|plasma|hue|glow|scanlines|vignette", 0, 0,
     L"Overlay effects drawn over the menu content; combinable.", 8, false},
    {L"appearance", L"overlayIntensity", L"Effects", SettingType::Int, L"50",
     nullptr, 0, 100, L"Overlay strength (opacity), 0-100.", 8, false},
    {L"appearance", L"overlaySpeed", L"Effects", SettingType::Int, L"100",
     nullptr, 0, 200, L"Overlay animation speed, 100 = normal.", 8, false},
    {L"appearance", L"overlaySize", L"Effects", SettingType::Int, L"100",
     nullptr, 25, 400,
     L"Overlay scale in percent: line spacing, grain size, glow reach.",
     8, false},
    {L"appearance", L"overlayFrameMs", L"Effects", SettingType::Int, L"16",
     nullptr, 1, 100, L"Milliseconds between overlay frames (lower = smoother).",
     8, false},
    {L"appearance", L"overlayAnimate", L"Effects", SettingType::Bool, L"false",
     nullptr, 0, 0,
     L"Keep overlays animating while the menu stays open (uses a frame timer).",
     8, false},
    {L"appearance", L"animationOpen", L"Animation", SettingType::EffectList, L"fade",
     L"none|fade|slide|scale|dissolve|crt|unfold", 0, 0,
     L"Open animation effects (comma separated, combinable).", 6, false},
    {L"appearance", L"animationClose", L"Animation", SettingType::EffectList, L"fade",
     L"none|fade|slide|scale|dissolve|crt|unfold", 0, 0,
     L"Close animation effects (comma separated, combinable).", 6, false},
    {L"appearance", L"animationDuration", L"Animation", SettingType::Int, L"120", nullptr, 0, 10000,
     L"Open animation duration in milliseconds.", 1, false},
    {L"appearance", L"animationCloseDuration", L"Animation", SettingType::Int, L"0", nullptr, 0, 10000,
     L"Close animation duration; 0 uses the open duration.", 6, false},
    {L"appearance", L"animationFrameMs", L"Animation", SettingType::Int, L"15", nullptr, 1, 100,
     L"Milliseconds between animation frames (lower = smoother).", 6, false},
    {L"appearance", L"animationEasing", L"Animation", SettingType::Enum, L"linear",
     L"linear|easeOut|easeInOut|back|bounce|elastic", 0, 0,
     L"Animation easing curve.", 6, false},
    {L"appearance", L"slideOffsetX", L"Animation", SettingType::Int, L"12", nullptr, -400, 400,
     L"Horizontal distance the menu slides from, in pixels.", 6, false},
    {L"appearance", L"slideOffsetY", L"Animation", SettingType::Int, L"0", nullptr, -400, 400,
     L"Vertical distance the menu slides from, in pixels.", 6, false},
    {L"appearance", L"scaleFrom", L"Animation", SettingType::Int, L"92", nullptr, 10, 200,
     L"Scale percentage scale/unfold/crt start from.", 6, false},
    {L"appearance", L"animationAnchor", L"Animation", SettingType::Enum, L"cursor",
     L"cursor|center", 0, 0,
     L"Scale origin: nearest panel corner to the cursor, or the panel center.", 6, false},
    {L"appearance", L"animateSubmenus", L"Animation", SettingType::Bool, L"false", nullptr, 0, 0,
     L"Run the open animation for submenus too.", 7, false},
    {L"appearance", L"animation", L"Animation", SettingType::Enum, L"none",
     L"none|fade|slide", 0, 0,
     L"Deprecated; maps to animationOpen and animationClose when they are unset.",
     1, true},
};
const ConfigSchemaEntry* SchemaFind(const std::wstring& key) {
    for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
        if (_wcsicmp(entry.key, key.c_str()) == 0) {
            return &entry;
        }
    }
    return nullptr;
}

bool SchemaHasKey(const std::wstring& key) {
    return SchemaFind(key) != nullptr;
}


// Applies one appearance key/value. Known keys always apply something
// (clamped or default); `warning` is set when the value was adjusted.
// Returns false only for a key with no schema row.
bool ApplyAppearanceValue(Appearance& appearance, const std::wstring& key,
                          const std::wstring& value,
                          std::wstring* warning = nullptr) {
    if (!SchemaHasKey(key)) {
        return false;
    }
    const std::wstring normalized = ToLowerCopy(key);
    const ConfigSchemaEntry* row = SchemaFind(normalized);
    auto warn = [&](const std::wstring& message) {
        if (warning) {
            *warning = message;
        }
    };
    auto applyInt = [&](int& field, int fallback) {
        int parsed = 0;
        if (!ParseIntValue(value, parsed)) {
            field = fallback;
            warn(L"invalid integer, using " + std::to_wstring(fallback));
            return true;
        }
        if (parsed < row->minValue || parsed > row->maxValue) {
            const int clamped = std::clamp(parsed, row->minValue, row->maxValue);
            field = clamped;
            warn(L"value " + std::to_wstring(parsed) + L" clamped to " +
                 std::to_wstring(clamped) + L" (range " +
                 std::to_wstring(row->minValue) + L"-" +
                 std::to_wstring(row->maxValue) + L")");
            return true;
        }
        field = parsed;
        return true;
    };
    auto applyBool = [&](bool& field, bool fallback) {
        bool parsed = false;
        if (!ParseBool(value, parsed)) {
            field = fallback;
            warn(L"invalid boolean, using the default");
            return true;
        }
        field = parsed;
        return true;
    };
    auto applyColor = [&](uint32_t& field, bool* hasFlag) {
        uint32_t parsed = 0;
        if (!ParseColor(value, parsed)) {
            if (hasFlag) {
                *hasFlag = false;
                warn(L"invalid color, leaving it unset");
            } else {
                uint32_t fallback = 0;
                ParseColor(row->defaultValue, fallback);
                field = fallback;
                warn(L"invalid color, using the default");
            }
            return true;
        }
        field = parsed;
        if (hasFlag) {
            *hasFlag = true;
        }
        return true;
    };
    auto applyEnum = [&](auto parse, auto& field, const auto& fallback) {
        if (!parse(value, field)) {
            field = fallback;
            warn(std::wstring(L"invalid value, using the default; expected ") +
                 row->validValues);
            return true;
        }
        return true;
    };
    auto applyEffects = [&](uint32_t& field, bool* hasFlag) {
        uint32_t parsed = 0;
        if (!ParseAnimationEffects(value, parsed)) {
            warn(L"unknown animation effect ignored; known effects kept");
        }
        field = parsed;
        if (hasFlag) {
            *hasFlag = true;
        }
        return true;
    };

    if (normalized == L"background") return applyColor(appearance.background, nullptr);
    if (normalized == L"blur") {
        return applyBool(appearance.blur, wcscmp(row->defaultValue, L"true") == 0);
    }
    if (normalized == L"blurstrength") {
        return applyInt(appearance.blurStrength, _wtoi(row->defaultValue));
    }
    if (normalized == L"overlay") {
        uint32_t parsed = 0;
        if (!ParseOverlayEffects(value, parsed)) {
            warn(L"unknown overlay effect ignored; known effects kept");
        }
        appearance.overlay = parsed;
        return true;
    }
    if (normalized == L"overlayintensity") {
        return applyInt(appearance.overlayIntensity, _wtoi(row->defaultValue));
    }
    if (normalized == L"overlayspeed") {
        return applyInt(appearance.overlaySpeed, _wtoi(row->defaultValue));
    }
    if (normalized == L"overlaysize") {
        return applyInt(appearance.overlaySize, _wtoi(row->defaultValue));
    }
    if (normalized == L"overlayframems") {
        return applyInt(appearance.overlayFrameMs, _wtoi(row->defaultValue));
    }
    if (normalized == L"overlayanimate") {
        return applyBool(appearance.overlayAnimate,
                         wcscmp(row->defaultValue, L"true") == 0);
    }
    if (normalized == L"cornerradius") {
        return applyInt(appearance.cornerRadius, _wtoi(row->defaultValue));
    }
    if (normalized == L"border") return applyColor(appearance.border, nullptr);
    if (normalized == L"borderwidth") {
        return applyInt(appearance.borderWidth, _wtoi(row->defaultValue));
    }
    if (normalized == L"shadow") {
        return applyBool(appearance.shadow, wcscmp(row->defaultValue, L"true") == 0);
    }
    if (normalized == L"shadowsize") {
        return applyInt(appearance.shadowSize, _wtoi(row->defaultValue));
    }
    if (normalized == L"shadowoffsetx") {
        return applyInt(appearance.shadowOffsetX, _wtoi(row->defaultValue));
    }
    if (normalized == L"shadowoffsety") {
        return applyInt(appearance.shadowOffsetY, _wtoi(row->defaultValue));
    }
    if (normalized == L"font") {
        if (!ParseFont(value, appearance.fontFace, appearance.fontSize)) {
            ParseFont(row->defaultValue, appearance.fontFace, appearance.fontSize);
            warn(L"invalid font, using the default");
        }
        return true;
    }
    if (normalized == L"itemheight") {
        return applyInt(appearance.itemHeight, _wtoi(row->defaultValue));
    }
    if (normalized == L"iconsize") {
        return applyInt(appearance.iconSize, _wtoi(row->defaultValue));
    }
    if (normalized == L"padding") {
        return applyInt(appearance.padding, _wtoi(row->defaultValue));
    }
    if (normalized == L"separator") return applyColor(appearance.separator, nullptr);
    if (normalized == L"hoverbackground") {
        return applyColor(appearance.hoverBackground, nullptr);
    }
    if (normalized == L"pressedbackground") {
        return applyColor(appearance.pressedBackground, nullptr);
    }
    if (normalized == L"textcolor") return applyColor(appearance.textColor, nullptr);
    if (normalized == L"disabledtextcolor") {
        return applyColor(appearance.disabledTextColor, nullptr);
    }
    if (normalized == L"submenuarrow") {
        return applyColor(appearance.submenuArrow, nullptr);
    }
    if (normalized == L"animationopen") {
        return applyEffects(appearance.animationOpen, &appearance.hasAnimationOpen);
    }
    if (normalized == L"animationclose") {
        return applyEffects(appearance.animationClose, &appearance.hasAnimationClose);
    }
    if (normalized == L"animation") {
        // Deprecated alias: applies to open/close only while those keys have
        // not been seen, so order in the file does not matter.
        uint32_t effects = 0;
        if (!ParseAnimationEffects(value, effects)) {
            warn(L"unknown animation effect ignored");
        }
        if (!appearance.hasAnimationOpen) {
            appearance.animationOpen = effects;
        }
        if (!appearance.hasAnimationClose) {
            appearance.animationClose = effects;
        }
        return true;
    }
    if (normalized == L"animationduration") {
        return applyInt(appearance.animationDuration, _wtoi(row->defaultValue));
    }
    if (normalized == L"animationcloseduration") {
        return applyInt(appearance.animationCloseDuration, _wtoi(row->defaultValue));
    }
    if (normalized == L"animationframems") {
        return applyInt(appearance.animationFrameMs, _wtoi(row->defaultValue));
    }
    if (normalized == L"animationeasing") {
        AnimEasing fallback = AnimEasing::Linear;
        ParseAnimEasing(row->defaultValue, fallback);
        return applyEnum(ParseAnimEasing, appearance.animationEasing, fallback);
    }
    if (normalized == L"slideoffsetx") {
        return applyInt(appearance.slideOffsetX, _wtoi(row->defaultValue));
    }
    if (normalized == L"slideoffsety") {
        return applyInt(appearance.slideOffsetY, _wtoi(row->defaultValue));
    }
    if (normalized == L"scalefrom") {
        return applyInt(appearance.scaleFrom, _wtoi(row->defaultValue));
    }
    if (normalized == L"animationanchor") {
        bool fallback = true;
        ParseAnimationAnchor(row->defaultValue, fallback);
        return applyEnum(ParseAnimationAnchor, appearance.animationAnchorAtCursor,
                         fallback);
    }
    if (normalized == L"animatesubmenus") {
        return applyBool(appearance.animateSubmenus,
                         wcscmp(row->defaultValue, L"true") == 0);
    }
    if (normalized == L"verticalpadding") {
        return applyInt(appearance.verticalPadding, _wtoi(row->defaultValue));
    }
    if (normalized == L"minwidth") {
        return applyInt(appearance.minWidth, _wtoi(row->defaultValue));
    }
    if (normalized == L"maxwidth") {
        return applyInt(appearance.maxWidth, _wtoi(row->defaultValue));
    }
    if (normalized == L"itempadding") {
        int parsed = 0;
        if (!ParseIntValue(value, parsed)) {
            appearance.itemPadding = -1;
            warn(L"invalid integer, leaving it unset (defaults to padding)");
        } else if (parsed < row->minValue || parsed > row->maxValue) {
            const int clamped = std::clamp(parsed, row->minValue, row->maxValue);
            appearance.itemPadding = clamped;
            warn(L"value " + std::to_wstring(parsed) + L" clamped to " +
                 std::to_wstring(clamped) + L" (range " +
                 std::to_wstring(row->minValue) + L"-" +
                 std::to_wstring(row->maxValue) + L")");
        } else {
            appearance.itemPadding = parsed;
        }
        return true;
    }
    if (normalized == L"separatorspacing") {
        return applyInt(appearance.separatorSpacing, _wtoi(row->defaultValue));
    }
    if (normalized == L"markerwidth") {
        return applyInt(appearance.markerWidth, _wtoi(row->defaultValue));
    }
    if (normalized == L"fontweight") {
        FontWeightKind fallback = FontWeightKind::Normal;
        ParseFontWeight(row->defaultValue, fallback);
        return applyEnum(ParseFontWeight, appearance.fontWeight, fallback);
    }
    if (normalized == L"fontstyle") {
        FontStyleKind fallback = FontStyleKind::Normal;
        ParseFontStyle(row->defaultValue, fallback);
        return applyEnum(ParseFontStyle, appearance.fontStyle, fallback);
    }
    if (normalized == L"cornerradii") {
        if (!ParseCornerRadii(value, appearance.cornerRadii)) {
            appearance.hasCornerRadii = false;
            warn(L"invalid radii, leaving them unset; expected tl,tr,br,bl");
        } else {
            appearance.hasCornerRadii = true;
        }
        return true;
    }
    if (normalized == L"shadowopacity") {
        return applyInt(appearance.shadowOpacity, _wtoi(row->defaultValue));
    }
    if (normalized == L"shadowcolor") {
        return applyColor(appearance.shadowColor, nullptr);
    }
    if (normalized == L"shadowadaptive") {
        return applyBool(appearance.shadowAdaptive,
                         wcscmp(row->defaultValue, L"true") == 0);
    }
    if (normalized == L"shadowblur") {
        return applyInt(appearance.shadowBlur, _wtoi(row->defaultValue));
    }
    if (normalized == L"marker") {
        MarkerStyle fallback = MarkerStyle::Dot;
        ParseMarkerStyle(row->defaultValue, fallback);
        return applyEnum(ParseMarkerStyle, appearance.marker, fallback);
    }
    if (normalized == L"markercolor") {
        return applyColor(appearance.markerColor, &appearance.hasMarkerColor);
    }
    if (normalized == L"headercolor") {
        return applyColor(appearance.headerColor, &appearance.hasHeaderColor);
    }
    if (normalized == L"showaccelerators") {
        AcceleratorMode fallback = AcceleratorMode::Underline;
        ParseAcceleratorMode(row->defaultValue, fallback);
        return applyEnum(ParseAcceleratorMode, appearance.acceleratorMode, fallback);
    }
    return false;
}

std::wstring CanonicalizeConfig(const std::wstring& text, int toVersion);

std::wstring GenerateDefaultConfigText() {
    std::wstring text = CanonicalizeConfig(L"", kConfigSchemaVersion);
    const std::wstring examples =
        L"\n; --- Examples: https://github.com/Bolt-Scripts/custom-cached-context-menu ---\n"
        L"; [rules]                             ; predicates: label / verb / ext / thirdParty / rule\n"
        L"; hide = label:\"Cast to Device\"       ; drop matching items\n"
        L"; keep = label:Share                  ; protect from the built-in grouping\n"
        L"; move = thirdParty -> \"More options\"  ; move matches into a submenu\n"
        L";\n"
        L"; [command \"Open in VS Code\"]         ; custom command (%1 file, %* all, %dir% folder)\n"
        L"; command = code.exe \"%1\"\n"
        L"; workingDir = %dir%\n"
        L"; match.ext = .cs, .cpp               ; only for these extensions\n"
        L"; menu = Tools                        ; place inside a custom submenu\n"
        L";\n"
        L"; [submenu \"Tools\"]                   ; custom submenu\n"
        L"; icon = @glyph:E712\n"
        L"; position = top                      ; top / bottom / after:\"X\" / before:\"X\"\n"
        L";\n"
        L"; [item \"TortoiseSVN*\"]               ; per-item override; the last match wins\n"
        L"; label = SVN\n"
        L"; icon = C:\\Tools\\svn.ico,0\n"
        L"; marker = bar                        ; dot / check / bar / none\n";
    const size_t metaPos = text.rfind(L"\n[meta]\n");
    if (metaPos != std::wstring::npos) {
        text.insert(metaPos, examples);
    } else {
        text += examples;
    }
    return text;
}

// Declared here and defined in [CMO:RulesEngine] below.
bool ParsePredicateExpr(const std::wstring& text, PredicateExpr& out,
                        std::wstring& error);

std::wstring ExtractQuoted(const std::wstring& text) {
    const std::wstring trimmed = TrimWhitespace(text);
    if (trimmed.size() < 2 || trimmed.front() != L'"' || trimmed.back() != L'"') {
        return L"";
    }
    return trimmed.substr(1, trimmed.size() - 2);
}

// Strips a ';' comment that is not inside a quoted value.
std::wstring StripInlineComment(const std::wstring& line) {
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == L'"') {
            inQuotes = !inQuotes;
        } else if (line[i] == L';' && !inQuotes) {
            return line.substr(0, i);
        }
    }
    return line;
}

bool AppendMatchValue(PredicateExpr& expr, const std::wstring& field,
                      const std::wstring& value) {
    if (field == L"multi" || field == L"thirdparty") {
        bool enabled = true;
        if (!ParseBool(value, enabled)) {
            return false;
        }
        if (enabled) {
            Predicate pred;
            pred.field = field == L"multi" ? PredicateField::Multi
                                           : PredicateField::ThirdParty;
            expr.all.push_back(std::move(pred));
        }
        return true;
    }
    if (field != L"label" && field != L"verb" && field != L"ext" &&
        field != L"scope") {
        return false;
    }
    std::wstring error;
    PredicateExpr parsed;
    if (!ParsePredicateExpr(field + L":" + value, parsed, error)) {
        return false;
    }
    for (Predicate& pred : parsed.all) {
        expr.all.push_back(std::move(pred));
    }
    return true;
}

bool ApplyCommandValue(CustomCommand& command, const std::wstring& key,
                       const std::wstring& value) {
    if (key == L"command") {
        command.command = value;
        return true;
    }
    if (key == L"workingdir") {
        command.workingDir = value;
        return true;
    }
    if (key == L"icon") {
        command.iconRef = value;
        return true;
    }
    if (key == L"menu") {
        command.menuPath = value;
        return true;
    }
    if (key == L"runas") {
        const std::wstring lower = ToLowerCopy(value);
        if (lower == L"none") {
            command.runAs = RunAs::None;
            return true;
        }
        if (lower == L"admin") {
            command.runAs = RunAs::Admin;
            return true;
        }
        return false;
    }
    if (key == L"showwindow") {
        const std::wstring lower = ToLowerCopy(value);
        if (lower == L"normal") {
            command.showWindow = ShowWindow::Normal;
            return true;
        }
        if (lower == L"maximized") {
            command.showWindow = ShowWindow::Maximized;
            return true;
        }
        if (lower == L"minimized") {
            command.showWindow = ShowWindow::Minimized;
            return true;
        }
        if (lower == L"hidden") {
            command.showWindow = ShowWindow::Hidden;
            return true;
        }
        return false;
    }
    if (key == L"separator") {
        const std::wstring lower = ToLowerCopy(value);
        if (lower == L"none") {
            command.separator = CommandSeparator::None;
            return true;
        }
        if (lower == L"before") {
            command.separator = CommandSeparator::Before;
            return true;
        }
        if (lower == L"after") {
            command.separator = CommandSeparator::After;
            return true;
        }
        return false;
    }
    if (key == L"action") {
        const std::wstring lower = ToLowerCopy(TrimWhitespace(value));
        if (lower == L"run") {
            command.action = BuiltinAction::None;
            return true;
        }
        if (lower == L"copypath") {
            command.action = BuiltinAction::CopyPath;
            return true;
        }
        if (lower == L"opennewwindow") {
            command.action = BuiltinAction::OpenNewWindow;
            return true;
        }
        if (lower == L"properties") {
            command.action = BuiltinAction::Properties;
            return true;
        }
        return false;
    }
    if (key == L"type") {
        const std::wstring lower = ToLowerCopy(TrimWhitespace(value));
        if (lower == L"command") {
            command.type = CommandType::Command;
            return true;
        }
        if (lower == L"separator") {
            command.type = CommandType::Separator;
            return true;
        }
        if (lower == L"header") {
            command.type = CommandType::Header;
            return true;
        }
        return false;
    }
    if (key.rfind(L"match.", 0) == 0) {
        return AppendMatchValue(command.match, key.substr(6), value);
    }
    return false;
}

bool ApplySubmenuValue(CustomSubmenu& submenu, const std::wstring& key,
                       const std::wstring& value) {
    if (key == L"icon") {
        submenu.iconRef = value;
        return true;
    }
    if (key == L"position") {
        const std::wstring lower = ToLowerCopy(TrimWhitespace(value));
        if (lower == L"top") {
            submenu.position = SubmenuPositionKind::Top;
            return true;
        }
        if (lower == L"bottom") {
            submenu.position = SubmenuPositionKind::Bottom;
            return true;
        }
        if (lower.rfind(L"after:", 0) == 0) {
            submenu.position = SubmenuPositionKind::After;
            submenu.positionLabel = ExtractQuoted(value.substr(6));
            return !submenu.positionLabel.empty();
        }
        if (lower.rfind(L"before:", 0) == 0) {
            submenu.position = SubmenuPositionKind::Before;
            submenu.positionLabel = ExtractQuoted(value.substr(7));
            return !submenu.positionLabel.empty();
        }
        return false;
    }
    if (key.rfind(L"match.", 0) == 0) {
        return AppendMatchValue(submenu.match, key.substr(6), value);
    }
    return false;
}

bool ApplyItemOverrideValue(ItemOverride& override, const std::wstring& key,
                            const std::wstring& value) {
    if (key == L"icon") {
        override.iconRef = value;
        override.hasIcon = true;
        return true;
    }
    if (key == L"label") {
        override.label = value;
        override.hasLabel = true;
        return true;
    }
    if (key == L"marker") {
        if (!ParseMarkerStyle(value, override.marker)) {
            return false;
        }
        override.hasMarker = true;
        return true;
    }
    if (key.rfind(L"match.", 0) == 0) {
        return AppendMatchValue(override.match, key.substr(6), value);
    }
    return false;
}

bool ApplyRuleLine(RulesConfig& config, const std::wstring& key,
                   const std::wstring& value, std::wstring& error) {
    RuleKind kind = RuleKind::Hide;
    if (key == L"hide") {
        kind = RuleKind::Hide;
    } else if (key == L"keep") {
        kind = RuleKind::Keep;
    } else if (key == L"move") {
        kind = RuleKind::Move;
    } else {
        error = L"unknown rule '" + key + L"'";
        return false;
    }

    Rule rule;
    rule.kind = kind;
    std::wstring predicateText = value;
    if (kind == RuleKind::Move) {
        const size_t arrow = value.find(L"->");
        if (arrow == std::wstring::npos) {
            error = L"move needs '-> \"Destination\"'";
            return false;
        }
        predicateText = TrimWhitespace(value.substr(0, arrow));
        rule.destination = ExtractQuoted(value.substr(arrow + 2));
        if (rule.destination.empty()) {
            error = L"move needs a quoted destination";
            return false;
        }
    }
    if (!ParsePredicateExpr(predicateText, rule.match, error)) {
        return false;
    }
    config.rules.push_back(std::move(rule));
    return true;
}

bool ParseRulesConfig(const std::wstring& text, RulesConfig& out,
                      std::vector<ConfigParseError>& errors) {
    errors.clear();

    enum class Section : uint8_t {
        None,
        Appearance,
        AppearanceLight,
        AppearanceDark,
        Rules,
        Command,
        Submenu,
        Item,
        Meta,
        Ignored,
    };
    Section section = Section::None;

    RulesConfig config;
    int currentCommand = -1;
    int currentSubmenu = -1;
    int currentOverride = -1;

    std::vector<std::pair<std::wstring, std::wstring>> baseValues;
    std::vector<std::pair<std::wstring, std::wstring>> lightValues;
    std::vector<std::pair<std::wstring, std::wstring>> darkValues;
    bool hasLight = false;
    bool hasDark = false;

    int lineNumber = 0;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        std::wstring line =
            text.substr(pos, newline == std::wstring::npos ? std::wstring::npos
                                                           : newline - pos);
        pos = newline == std::wstring::npos ? text.size() + 1 : newline + 1;
        ++lineNumber;
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }

        const std::wstring trimmed = TrimWhitespace(StripInlineComment(line));
        if (trimmed.empty() || trimmed[0] == L';' || trimmed[0] == L'#') {
            continue;
        }

        if (trimmed[0] == L'[') {
            if (trimmed.back() != L']') {
                errors.push_back({lineNumber, L"malformed section header"});
                section = Section::Ignored;
                continue;
            }
            const std::wstring rawName =
                TrimWhitespace(trimmed.substr(1, trimmed.size() - 2));
            const std::wstring name = ToLowerCopy(rawName);
            currentCommand = -1;
            currentSubmenu = -1;
            currentOverride = -1;
            if (name == L"appearance") {
                section = Section::Appearance;
            } else if (name == L"appearance.light") {
                section = Section::AppearanceLight;
                hasLight = true;
            } else if (name == L"appearance.dark") {
                section = Section::AppearanceDark;
                hasDark = true;
            } else if (name == L"rules") {
                section = Section::Rules;
            } else if (name.rfind(L"command ", 0) == 0) {
                const std::wstring label = ExtractQuoted(rawName.substr(8));
                if (label.empty()) {
                    errors.push_back(
                        {lineNumber, L"command section needs a quoted label"});
                    section = Section::Ignored;
                } else {
                    config.commands.push_back(CustomCommand{});
                    config.commands.back().label = label;
                    currentCommand = static_cast<int>(config.commands.size()) - 1;
                    section = Section::Command;
                }
            } else if (name.rfind(L"item ", 0) == 0) {
                const std::wstring glob = ExtractQuoted(rawName.substr(5));
                if (glob.empty()) {
                    errors.push_back(
                        {lineNumber, L"item section needs a quoted label glob"});
                    section = Section::Ignored;
                } else {
                    ItemOverride override;
                    Predicate predicate;
                    predicate.field = PredicateField::Label;
                    predicate.values.push_back(NormalizeMenuLabel(glob));
                    override.match.all.push_back(std::move(predicate));
                    config.overrides.push_back(std::move(override));
                    currentOverride =
                        static_cast<int>(config.overrides.size()) - 1;
                    section = Section::Item;
                }
            } else if (name == L"meta") {
                section = Section::Meta;
            } else if (name.rfind(L"submenu ", 0) == 0) {
                const std::wstring submenuName = ExtractQuoted(rawName.substr(8));
                if (submenuName.empty()) {
                    errors.push_back(
                        {lineNumber, L"submenu section needs a quoted name"});
                    section = Section::Ignored;
                } else {
                    config.submenus.push_back(CustomSubmenu{});
                    config.submenus.back().name = submenuName;
                    currentSubmenu = static_cast<int>(config.submenus.size()) - 1;
                    section = Section::Submenu;
                }
            } else {
                errors.push_back({lineNumber, L"unknown section '" + name + L"'"});
                section = Section::Ignored;
            }
            continue;
        }

        const size_t equals = trimmed.find(L'=');
        if (equals == std::wstring::npos) {
            errors.push_back({lineNumber, L"expected key = value"});
            continue;
        }
        const std::wstring key = ToLowerCopy(TrimWhitespace(trimmed.substr(0, equals)));
        const std::wstring value = TrimWhitespace(trimmed.substr(equals + 1));

        if (section == Section::Appearance) {
            Appearance scratch = Appearance{};
            std::wstring adjustment;
            if (!ApplyAppearanceValue(scratch, key, value, &adjustment)) {
                errors.push_back(
                    {lineNumber, L"unknown key '" + key + L"', ignored"});
            } else {
                if (!adjustment.empty()) {
                    errors.push_back(
                        {lineNumber, L"'" + key + L"': " + adjustment});
                }
                baseValues.emplace_back(key, value);
            }
        } else if (section == Section::AppearanceLight) {
            Appearance scratch = Appearance{};
            std::wstring adjustment;
            if (!ApplyAppearanceValue(scratch, key, value, &adjustment)) {
                errors.push_back(
                    {lineNumber, L"unknown key '" + key + L"', ignored"});
            } else {
                if (!adjustment.empty()) {
                    errors.push_back(
                        {lineNumber, L"'" + key + L"': " + adjustment});
                }
                lightValues.emplace_back(key, value);
            }
        } else if (section == Section::AppearanceDark) {
            Appearance scratch = Appearance{};
            std::wstring adjustment;
            if (!ApplyAppearanceValue(scratch, key, value, &adjustment)) {
                errors.push_back(
                    {lineNumber, L"unknown key '" + key + L"', ignored"});
            } else {
                if (!adjustment.empty()) {
                    errors.push_back(
                        {lineNumber, L"'" + key + L"': " + adjustment});
                }
                darkValues.emplace_back(key, value);
            }
        } else if (section == Section::Rules) {
            std::wstring error;
            if (!ApplyRuleLine(config, key, value, error)) {
                errors.push_back({lineNumber, error});
            }
        } else if (section == Section::Command) {
            if (currentCommand < 0 ||
                !ApplyCommandValue(config.commands[currentCommand], key, value)) {
                errors.push_back({lineNumber,
                                  L"invalid command value for '" + key + L"'"});
            }
        } else if (section == Section::Submenu) {
            if (currentSubmenu < 0 ||
                !ApplySubmenuValue(config.submenus[currentSubmenu], key, value)) {
                errors.push_back({lineNumber,
                                  L"invalid submenu value for '" + key + L"'"});
            }
        } else if (section == Section::Item) {
            if (currentOverride < 0 ||
                !ApplyItemOverrideValue(config.overrides[currentOverride], key,
                                        value)) {
                errors.push_back({lineNumber,
                                  L"invalid item value for '" + key + L"'"});
            }
        } else if (section == Section::Meta) {
            if (key == L"schemaversion") {
                if (!ParseBoundedInt(value, config.schemaVersion, 0, 1000)) {
                    errors.push_back(
                        {lineNumber, L"invalid value for 'schemaVersion'"});
                }
            } else {
                errors.push_back(
                    {lineNumber, L"unknown key '" + key + L"' in [meta]"});
            }
        } else if (section == Section::Ignored) {
            // Intentionally ignored.
        } else {
            errors.push_back({lineNumber, L"key outside a section"});
        }
    }

    for (const auto& pair : baseValues) {
        ApplyAppearanceValue(config.appearance, pair.first, pair.second, nullptr);
    }
    config.hasLightAppearance = hasLight;
    if (hasLight) {
        config.lightAppearance = config.appearance;
        for (const auto& pair : lightValues) {
            ApplyAppearanceValue(config.lightAppearance, pair.first, pair.second, nullptr);
        }
    }
    config.hasDarkAppearance = hasDark;
    if (hasDark) {
        config.darkAppearance = config.appearance;
        for (const auto& pair : darkValues) {
            ApplyAppearanceValue(config.darkAppearance, pair.first, pair.second, nullptr);
        }
    }

    out = std::move(config);
    return true;
}

int ReadSchemaVersion(const std::wstring& text) {
    bool inMeta = false;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        std::wstring line =
            text.substr(pos, newline == std::wstring::npos ? std::wstring::npos
                                                           : newline - pos);
        pos = newline == std::wstring::npos ? text.size() + 1 : newline + 1;
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        const std::wstring trimmed = TrimWhitespace(StripInlineComment(line));
        if (trimmed.empty() || trimmed[0] == L';') {
            continue;
        }
        if (trimmed[0] == L'[') {
            if (trimmed.back() != L']') {
                inMeta = false;
                continue;
            }
            const std::wstring name = ToLowerCopy(
                TrimWhitespace(trimmed.substr(1, trimmed.size() - 2)));
            inMeta = (name == L"meta");
            continue;
        }
        if (!inMeta) {
            continue;
        }
        const size_t equals = trimmed.find(L'=');
        if (equals == std::wstring::npos) {
            continue;
        }
        const std::wstring key =
            ToLowerCopy(TrimWhitespace(trimmed.substr(0, equals)));
        if (key != L"schemaversion") {
            continue;
        }
        int version = 0;
        if (ParseBoundedInt(TrimWhitespace(trimmed.substr(equals + 1)), version, 0,
                            1000)) {
            return version;
        }
        return 0;
    }
    return 0;
}


std::wstring FormatFontSize(float size) {
    wchar_t buffer[32] = {};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.4g", static_cast<double>(size));
    return buffer;
}

bool AppearanceValueIsValid(const ConfigSchemaEntry& entry,
                            const std::wstring& value) {
    switch (entry.type) {
        case SettingType::Bool: {
            bool parsed = false;
            return ParseBool(value, parsed);
        }
        case SettingType::Int: {
            int parsed = 0;
            return ParseIntValue(value, parsed);
        }
        case SettingType::Color: {
            uint32_t parsed = 0;
            return ParseColor(value, parsed);
        }
        case SettingType::Font: {
            std::wstring face;
            float size = 0;
            return ParseFont(value, face, size);
        }
        case SettingType::Enum: {
            const std::wstring valid(entry.validValues);
            size_t start = 0;
            while (start <= valid.size()) {
                const size_t bar = valid.find(L'|', start);
                const std::wstring part =
                    valid.substr(start, bar == std::wstring::npos
                                            ? std::wstring::npos
                                            : bar - start);
                if (_wcsicmp(part.c_str(), TrimWhitespace(value).c_str()) == 0) {
                    return true;
                }
                if (bar == std::wstring::npos) {
                    break;
                }
                start = bar + 1;
            }
            return false;
        }
        case SettingType::IntList: {
            CornerRadii radii;
            return ParseCornerRadii(value, radii);
        }
        case SettingType::EffectList: {
            uint32_t effects = 0;
            return ToLowerCopy(entry.key) == L"overlay"
                       ? ParseOverlayEffects(value, effects)
                       : ParseAnimationEffects(value, effects);
        }
    }
    return false;
}

std::wstring SchemaValueHint(const ConfigSchemaEntry& entry) {
    if (entry.validValues) {
        std::wstring spaced;
        for (const wchar_t* p = entry.validValues; *p; ++p) {
            if (*p == L'|') {
                spaced += L" | ";
            } else {
                spaced += *p;
            }
        }
        return spaced;
    }
    if (entry.type == SettingType::Int) {
        return std::to_wstring(entry.minValue) + L"-" +
               std::to_wstring(entry.maxValue);
    }
    return L"";
}

std::wstring NormalizeAppearanceValue(const ConfigSchemaEntry& entry,
                                      const std::wstring& value) {
    switch (entry.type) {
        case SettingType::Color: {
            uint32_t argb = 0;
            if (ParseColor(value, argb)) {
                return FormatColorRgba(argb);
            }
            break;
        }
        case SettingType::Int: {
            int parsed = 0;
            if (ParseIntValue(value, parsed)) {
                return std::to_wstring(
                    std::clamp(parsed, entry.minValue, entry.maxValue));
            }
            break;
        }
        case SettingType::Bool: {
            bool parsed = false;
            if (ParseBool(value, parsed)) {
                return parsed ? L"true" : L"false";
            }
            break;
        }
        case SettingType::Enum: {
            const std::wstring valid(entry.validValues);
            size_t start = 0;
            while (start <= valid.size()) {
                const size_t bar = valid.find(L'|', start);
                const std::wstring part =
                    valid.substr(start, bar == std::wstring::npos
                                            ? std::wstring::npos
                                            : bar - start);
                if (_wcsicmp(part.c_str(), TrimWhitespace(value).c_str()) == 0) {
                    return part;
                }
                if (bar == std::wstring::npos) {
                    break;
                }
                start = bar + 1;
            }
            break;
        }
        case SettingType::Font: {
            std::wstring face;
            float size = 0;
            if (ParseFont(value, face, size)) {
                return face + L", " + FormatFontSize(size);
            }
            break;
        }
        case SettingType::IntList: {
            CornerRadii radii;
            if (ParseCornerRadii(value, radii)) {
                return std::to_wstring(radii.topLeft) + L", " +
                       std::to_wstring(radii.topRight) + L", " +
                       std::to_wstring(radii.bottomRight) + L", " +
                       std::to_wstring(radii.bottomLeft);
            }
            break;
        }
        case SettingType::EffectList: {
            // Keep the recognized subset so the rewrite matches what the
            // parser actually applied (invalid tokens are dropped).
            uint32_t effects = 0;
            if (ToLowerCopy(entry.key) == L"overlay") {
                ParseOverlayEffects(value, effects);
                return OverlayEffectsText(effects);
            }
            ParseAnimationEffects(value, effects);
            return AnimationEffectsText(effects);
        }
    }
    if (entry.type == SettingType::Color) {
        uint32_t fallback = 0;
        if (ParseColor(entry.defaultValue, fallback)) {
            return FormatColorRgba(fallback);
        }
    }
    return entry.defaultValue;
}

// Canonical text for enum values, matching the schema's validValues casing.
const wchar_t* FontWeightText(FontWeightKind value) {
    switch (value) {
        case FontWeightKind::Semibold:
            return L"semibold";
        case FontWeightKind::Bold:
            return L"bold";
        default:
            return L"normal";
    }
}

const wchar_t* FontStyleText(FontStyleKind value) {
    return value == FontStyleKind::Italic ? L"italic" : L"normal";
}

const wchar_t* MarkerStyleText(MarkerStyle value) {
    switch (value) {
        case MarkerStyle::Check:
            return L"check";
        case MarkerStyle::Bar:
            return L"bar";
        case MarkerStyle::None:
            return L"none";
        default:
            return L"dot";
    }
}

const wchar_t* AcceleratorModeText(AcceleratorMode value) {
    switch (value) {
        case AcceleratorMode::Strip:
            return L"strip";
        case AcceleratorMode::Raw:
            return L"raw";
        default:
            return L"underline";
    }
}

const wchar_t* AnimEasingText(AnimEasing value) {
    switch (value) {
        case AnimEasing::EaseOut:
            return L"easeOut";
        case AnimEasing::EaseInOut:
            return L"easeInOut";
        case AnimEasing::Back:
            return L"back";
        case AnimEasing::Bounce:
            return L"bounce";
        case AnimEasing::Elastic:
            return L"elastic";
        default:
            return L"linear";
    }
}

// The reverse of ApplyAppearanceValue: canonical value text for one schema key
// from an appearance snapshot. Returns false when the key is unset (so callers
// emit a delete override) or when the row is the deprecated `animation` alias.
bool AppearanceValueText(const Appearance& appearance,
                         const ConfigSchemaEntry& entry, std::wstring& out) {
    const std::wstring key = ToLowerCopy(entry.key);
    auto color = [&](uint32_t value) {
        out = FormatColorRgba(value);
        return true;
    };
    auto number = [&](int value) {
        out = std::to_wstring(value);
        return true;
    };
    auto boolean = [&](bool value) {
        out = value ? L"true" : L"false";
        return true;
    };

    if (key == L"background") return color(appearance.background);
    if (key == L"blur") return boolean(appearance.blur);
    if (key == L"blurstrength") return number(appearance.blurStrength);
    if (key == L"overlay") {
        out = OverlayEffectsText(appearance.overlay);
        return true;
    }
    if (key == L"overlayintensity") return number(appearance.overlayIntensity);
    if (key == L"overlayspeed") return number(appearance.overlaySpeed);
    if (key == L"overlaysize") return number(appearance.overlaySize);
    if (key == L"overlayframems") return number(appearance.overlayFrameMs);
    if (key == L"overlayanimate") return boolean(appearance.overlayAnimate);
    if (key == L"cornerradius") return number(appearance.cornerRadius);
    if (key == L"border") return color(appearance.border);
    if (key == L"borderwidth") return number(appearance.borderWidth);
    if (key == L"shadow") return boolean(appearance.shadow);
    if (key == L"shadowsize") return number(appearance.shadowSize);
    if (key == L"shadowoffsetx") return number(appearance.shadowOffsetX);
    if (key == L"shadowoffsety") return number(appearance.shadowOffsetY);
    if (key == L"font") {
        out = appearance.fontFace + L", " + FormatFontSize(appearance.fontSize);
        return true;
    }
    if (key == L"itemheight") return number(appearance.itemHeight);
    if (key == L"iconsize") return number(appearance.iconSize);
    if (key == L"padding") return number(appearance.padding);
    if (key == L"separator") return color(appearance.separator);
    if (key == L"hoverbackground") return color(appearance.hoverBackground);
    if (key == L"pressedbackground") return color(appearance.pressedBackground);
    if (key == L"textcolor") return color(appearance.textColor);
    if (key == L"disabledtextcolor") return color(appearance.disabledTextColor);
    if (key == L"submenuarrow") return color(appearance.submenuArrow);
    if (key == L"animationopen") {
        out = AnimationEffectsText(appearance.animationOpen);
        return true;
    }
    if (key == L"animationclose") {
        out = AnimationEffectsText(appearance.animationClose);
        return true;
    }
    if (key == L"animation") {
        return false;  // deprecated alias, not editable
    }
    if (key == L"animationduration") return number(appearance.animationDuration);
    if (key == L"animationcloseduration") {
        return number(appearance.animationCloseDuration);
    }
    if (key == L"animationframems") return number(appearance.animationFrameMs);
    if (key == L"animationeasing") {
        out = AnimEasingText(appearance.animationEasing);
        return true;
    }
    if (key == L"slideoffsetx") return number(appearance.slideOffsetX);
    if (key == L"slideoffsety") return number(appearance.slideOffsetY);
    if (key == L"scalefrom") return number(appearance.scaleFrom);
    if (key == L"animationanchor") {
        out = appearance.animationAnchorAtCursor ? L"cursor" : L"center";
        return true;
    }
    if (key == L"animatesubmenus") return boolean(appearance.animateSubmenus);
    if (key == L"verticalpadding") return number(appearance.verticalPadding);
    if (key == L"minwidth") return number(appearance.minWidth);
    if (key == L"maxwidth") return number(appearance.maxWidth);
    if (key == L"itempadding") {
        if (appearance.itemPadding < 0) {
            return false;
        }
        return number(appearance.itemPadding);
    }
    if (key == L"separatorspacing") return number(appearance.separatorSpacing);
    if (key == L"markerwidth") return number(appearance.markerWidth);
    if (key == L"fontweight") {
        out = FontWeightText(appearance.fontWeight);
        return true;
    }
    if (key == L"fontstyle") {
        out = FontStyleText(appearance.fontStyle);
        return true;
    }
    if (key == L"cornerradii") {
        if (!appearance.hasCornerRadii) {
            return false;
        }
        out = std::to_wstring(appearance.cornerRadii.topLeft) + L", " +
              std::to_wstring(appearance.cornerRadii.topRight) + L", " +
              std::to_wstring(appearance.cornerRadii.bottomRight) + L", " +
              std::to_wstring(appearance.cornerRadii.bottomLeft);
        return true;
    }
    if (key == L"shadowopacity") return number(appearance.shadowOpacity);
    if (key == L"shadowcolor") return color(appearance.shadowColor);
    if (key == L"shadowadaptive") return boolean(appearance.shadowAdaptive);
    if (key == L"shadowblur") return number(appearance.shadowBlur);
    if (key == L"marker") {
        out = MarkerStyleText(appearance.marker);
        return true;
    }
    if (key == L"markercolor") {
        if (!appearance.hasMarkerColor) {
            return false;
        }
        return color(appearance.markerColor);
    }
    if (key == L"headercolor") {
        if (!appearance.hasHeaderColor) {
            return false;
        }
        return color(appearance.headerColor);
    }
    if (key == L"showaccelerators") {
        out = AcceleratorModeText(appearance.acceleratorMode);
        return true;
    }
    return false;
}

struct CanonicalSource {
    std::unordered_map<std::wstring, std::wstring> baseValues;
    std::unordered_map<std::wstring, std::wstring> lightValues;
    std::unordered_map<std::wstring, std::wstring> darkValues;
    std::vector<std::wstring> preservedBlocks;
};

CanonicalSource SplitConfigForCanonical(const std::wstring& text) {
    CanonicalSource out;
    std::wstring currentSection;
    std::wstring currentBlock;

    auto flush = [&]() {
        if (currentSection.empty()) {
            return;
        }
        const std::wstring lower = ToLowerCopy(currentSection);
        if (lower == L"appearance" || lower == L"appearance.light" ||
            lower == L"appearance.dark") {
            std::unordered_map<std::wstring, std::wstring>* values =
                lower == L"appearance"        ? &out.baseValues
                : lower == L"appearance.light" ? &out.lightValues
                                               : &out.darkValues;
            size_t pos = 0;
            while (pos <= currentBlock.size()) {
                const size_t newline = currentBlock.find(L'\n', pos);
                const size_t lineEnd = newline == std::wstring::npos
                                           ? currentBlock.size()
                                           : newline;
                const std::wstring line = currentBlock.substr(pos, lineEnd - pos);
                pos = lineEnd + 1;
                const std::wstring trimmed = TrimWhitespace(StripInlineComment(line));
                if (trimmed.empty() || trimmed[0] == L';' || trimmed[0] == L'#') {
                    continue;
                }
                const size_t equals = trimmed.find(L'=');
                if (equals == std::wstring::npos) {
                    continue;
                }
                (*values)[ToLowerCopy(TrimWhitespace(trimmed.substr(0, equals)))] =
                    TrimWhitespace(trimmed.substr(equals + 1));
            }
        } else if (lower != L"meta") {
            out.preservedBlocks.push_back(currentBlock);
        }
        currentSection.clear();
        currentBlock.clear();
    };

    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd = newline == std::wstring::npos ? text.size() : newline;
        std::wstring line = text.substr(pos, lineEnd - pos);
        pos = lineEnd + 1;
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        const std::wstring trimmed = TrimWhitespace(StripInlineComment(line));
        if (!trimmed.empty() && trimmed[0] == L'[') {
            flush();
            if (trimmed.back() == L']') {
                currentSection =
                    TrimWhitespace(trimmed.substr(1, trimmed.size() - 2));
                currentBlock = line + L"\n";
            }
            continue;
        }
        if (!currentSection.empty()) {
            currentBlock += line + L"\n";
        }
    }
    flush();
    return out;
}

// Rewrites the file into the canonical layout: one [appearance] section with
// grouped active settings, theme overrides only when set, structured blocks
// preserved, and a fresh [meta] version.
std::wstring EmitCanonicalConfig(const CanonicalSource& source, int toVersion) {
    std::wstring out;
    out += L"; Custom Cached Context Menu configuration (schema ";
    out += std::to_wstring(toVersion);
    out += L")\n";
    out += L"; UTF-8. Reloaded when a menu opens. Settings are active; edit the values.\n";
    out += L"; Colors are R, G, B, A (0-255 each; A optional). ';' starts a comment.\n";
    out += L"; Errors are logged as menu.ini:<line>: warning: <message>; bad values clamp\n";
    out += L"; or fall back to their defaults and never stop the file from loading.\n";
    out += L"; Updates rewrite this file in this layout and keep your values. Theme overrides\n";
    out += L"; live in [appearance.light] / [appearance.dark] (see docs/CONFIG.md).\n";
    out += L"\n[appearance]\n";

    std::wstring lastGroup;
    for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
        if (wcscmp(entry.section, L"appearance") != 0) {
            continue;
        }
        if (lastGroup != entry.group) {
            out += L"\n; --- ";
            out += entry.group;
            out += L" ---\n";
            lastGroup = entry.group;
        }
        const auto it = source.baseValues.find(ToLowerCopy(entry.key));
        const bool hasValue =
            it != source.baseValues.end() &&
            (!entry.unset || AppearanceValueIsValid(entry, it->second));
        if (entry.unset && !hasValue) {
            out += L"; ";
            out += entry.key;
            out += L" = ";
            out += NormalizeAppearanceValue(entry, entry.defaultValue);
            out += L"   ; unset: ";
            const std::wstring hint = SchemaValueHint(entry);
            if (!hint.empty()) {
                out += hint;
                out += L" \u2014 ";
            }
            out += entry.description;
            out += L"\n";
            continue;
        }
        out += entry.key;
        out += L" = ";
        out += NormalizeAppearanceValue(
            entry, hasValue ? it->second : std::wstring(entry.defaultValue));
        out += L"   ; ";
        const std::wstring hint = SchemaValueHint(entry);
        if (!hint.empty()) {
            out += hint;
            out += L" \u2014 ";
        }
        out += entry.description;
        out += L"\n";
    }

    auto emitTheme = [&](const wchar_t* section,
                         const std::unordered_map<std::wstring, std::wstring>& values) {
        std::vector<const ConfigSchemaEntry*> emitted;
        for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
            const auto it = values.find(ToLowerCopy(entry.key));
            if (it == values.end()) {
                continue;
            }
            if (entry.unset && !AppearanceValueIsValid(entry, it->second)) {
                continue;  // stay unset rather than pinning a derived default
            }
            emitted.push_back(&entry);
        }
        if (emitted.empty()) {
            return;
        }
        out += L"\n[";
        out += section;
        out += L"]\n";
        for (const ConfigSchemaEntry* entry : emitted) {
            out += entry->key;
            out += L" = ";
            out += NormalizeAppearanceValue(
                *entry, values.find(ToLowerCopy(entry->key))->second);
            out += L"   ; ";
            const std::wstring hint = SchemaValueHint(*entry);
            if (!hint.empty()) {
                out += hint;
                out += L" \u2014 ";
            }
            out += entry->description;
            out += L"\n";
        }
    };
    emitTheme(L"appearance.light", source.lightValues);
    emitTheme(L"appearance.dark", source.darkValues);

    for (const std::wstring& block : source.preservedBlocks) {
        std::wstring trimmed = block;
        while (!trimmed.empty() &&
               (trimmed.back() == L'\n' || trimmed.back() == L'\r')) {
            trimmed.pop_back();
        }
        if (trimmed.empty()) {
            continue;
        }
        out += L"\n";
        out += trimmed;
        out += L"\n";
    }
    out += L"\n[meta]\nschemaVersion = ";
    out += std::to_wstring(toVersion);
    out += L"\n";
    return out;
}

std::wstring CanonicalizeConfig(const std::wstring& text, int toVersion) {
    return EmitCanonicalConfig(SplitConfigForCanonical(text), toVersion);
}

// One change for CanonicalizeConfigWithOverrides: a set in a section, or a
// remove (restoring inheritance/default) when `remove` is true.
struct ConfigOverride {
    std::wstring section;
    std::wstring key;
    std::wstring value;
    bool remove = false;
};

// Canonical rewrite with explicit set/remove changes applied to the parsed
// base/light/dark value maps before emission. Unknown sections or keys are
// ignored, so a stale override can never corrupt the file.
std::wstring CanonicalizeConfigWithOverrides(
    const std::wstring& text, int toVersion,
    const std::vector<ConfigOverride>& overrides) {
    CanonicalSource source = SplitConfigForCanonical(text);
    for (const ConfigOverride& change : overrides) {
        std::unordered_map<std::wstring, std::wstring>* values = nullptr;
        const std::wstring section = ToLowerCopy(change.section);
        if (section == L"appearance") {
            values = &source.baseValues;
        } else if (section == L"appearance.light") {
            values = &source.lightValues;
        } else if (section == L"appearance.dark") {
            values = &source.darkValues;
        }
        if (!values) {
            continue;
        }
        const std::wstring key = ToLowerCopy(change.key);
        if (change.remove) {
            values->erase(key);
            continue;
        }
        const ConfigSchemaEntry* entry = SchemaFind(key);
        if (!entry) {
            continue;
        }
        (*values)[key] = NormalizeAppearanceValue(*entry, change.value);
    }
    return EmitCanonicalConfig(source, toVersion);
}

// ===========================================================================
// [CMO:RulesEngine] v2 predicates and rule matching.
// ===========================================================================

struct ItemContext {
    Scope scope = Scope::Files;
    Shape shape = Shape::Single;
    std::vector<std::wstring> paths;
};

bool GlobMatches(const std::wstring& pattern, const std::wstring& text) {
    size_t p = 0;
    size_t t = 0;
    size_t star = std::wstring::npos;
    size_t mark = 0;
    while (t < text.size()) {
        if (p < pattern.size() && pattern[p] == L'*') {
            star = p++;
            mark = t;
        } else if (p < pattern.size() &&
                   towlower(pattern[p]) == towlower(text[t])) {
            ++p;
            ++t;
        } else if (star != std::wstring::npos) {
            p = star + 1;
            t = ++mark;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == L'*') {
        ++p;
    }
    return p == pattern.size();
}

bool ScopeFromName(const std::wstring& name, Scope& scope) {
    const std::wstring lower = ToLowerCopy(name);
    if (lower == L"files") {
        scope = Scope::Files;
        return true;
    }
    if (lower == L"folders") {
        scope = Scope::Folders;
        return true;
    }
    if (lower == L"background") {
        scope = Scope::Background;
        return true;
    }
    if (lower == L"desktop") {
        scope = Scope::Desktop;
        return true;
    }
    if (lower == L"drive") {
        scope = Scope::Drive;
        return true;
    }
    return false;
}

bool ItemIsThirdParty(const MenuItem& item) {
    return (item.flags & kModelThirdParty) != 0;
}

bool PredicateMatches(const Predicate& pred, const MenuItem& item,
                      const ItemContext& ctx) {
    switch (pred.field) {
        case PredicateField::Label: {
            const std::wstring label = NormalizeMenuLabel(item.label);
            for (const std::wstring& value : pred.values) {
                if (GlobMatches(value, label)) {
                    return true;
                }
            }
            return false;
        }
        case PredicateField::Verb:
            for (const std::wstring& value : pred.values) {
                if (_wcsicmp(value.c_str(), item.canonicalVerb.c_str()) == 0) {
                    return true;
                }
            }
            return false;
        case PredicateField::Ext:
            for (const std::wstring& path : ctx.paths) {
                const size_t dot = path.find_last_of(L'.');
                const size_t slash = path.find_last_of(L"\\/");
                if (dot == std::wstring::npos ||
                    (slash != std::wstring::npos && dot < slash)) {
                    continue;
                }
                const std::wstring ext = path.substr(dot);
                for (const std::wstring& value : pred.values) {
                    if (_wcsicmp(ext.c_str(), value.c_str()) == 0) {
                        return true;
                    }
                }
            }
            return false;
        case PredicateField::Scope:
            for (const std::wstring& value : pred.values) {
                Scope scope = Scope::Other;
                if (ScopeFromName(value, scope) && scope == ctx.scope) {
                    return true;
                }
            }
            return false;
        case PredicateField::Multi:
            return ctx.shape == Shape::Multi;
        case PredicateField::ThirdParty:
            return ItemIsThirdParty(item);
    }
    return false;
}

bool PredicateExprMatches(const PredicateExpr& expr, const MenuItem& item,
                          const ItemContext& ctx) {
    if (expr.all.empty()) {
        return false;
    }
    for (const Predicate& pred : expr.all) {
        if (!PredicateMatches(pred, item, ctx)) {
            return false;
        }
    }
    return true;
}

std::vector<std::wstring> SplitPredicateAnd(const std::wstring& text) {
    std::vector<std::wstring> parts;
    std::wstring current;
    bool inQuotes = false;
    for (size_t i = 0; i < text.size(); ++i) {
        const wchar_t c = text[i];
        if (c == L'"') {
            inQuotes = !inQuotes;
        }
        if (!inQuotes && i > 0 && i + 3 < text.size() &&
            iswspace(text[i - 1]) &&
            (c == L'a' || c == L'A') &&
            (text[i + 1] == L'n' || text[i + 1] == L'N') &&
            (text[i + 2] == L'd' || text[i + 2] == L'D') &&
            iswspace(text[i + 3])) {
            parts.push_back(current);
            current.clear();
            i += 2;
            continue;
        }
        current += c;
    }
    parts.push_back(current);
    return parts;
}

bool ParsePredicateExpr(const std::wstring& text, PredicateExpr& out,
                        std::wstring& error) {
    PredicateExpr expr;
    for (const std::wstring& rawPart : SplitPredicateAnd(text)) {
        const std::wstring part = TrimWhitespace(rawPart);
        if (part.empty()) {
            error = L"empty predicate";
            return false;
        }

        Predicate pred;
        const size_t colon = part.find(L':');
        if (colon == std::wstring::npos) {
            const std::wstring name = ToLowerCopy(part);
            if (name == L"multi") {
                pred.field = PredicateField::Multi;
            } else if (name == L"thirdparty") {
                pred.field = PredicateField::ThirdParty;
            } else {
                error = L"unknown predicate '" + part + L"'";
                return false;
            }
            expr.all.push_back(std::move(pred));
            continue;
        }

        const std::wstring fieldName =
            ToLowerCopy(TrimWhitespace(part.substr(0, colon)));
        if (fieldName == L"label") {
            pred.field = PredicateField::Label;
        } else if (fieldName == L"verb") {
            pred.field = PredicateField::Verb;
        } else if (fieldName == L"ext") {
            pred.field = PredicateField::Ext;
        } else if (fieldName == L"scope") {
            pred.field = PredicateField::Scope;
        } else {
            error = L"unknown predicate field '" + fieldName + L"'";
            return false;
        }

        std::vector<std::wstring> values;
        std::wstring current;
        bool inQuotes = false;
        const std::wstring valuesText = part.substr(colon + 1);
        for (size_t i = 0; i < valuesText.size(); ++i) {
            const wchar_t c = valuesText[i];
            if (c == L'"') {
                inQuotes = !inQuotes;
                continue;
            }
            if (c == L',' && !inQuotes) {
                values.push_back(TrimWhitespace(current));
                current.clear();
                continue;
            }
            current += c;
        }
        values.push_back(TrimWhitespace(current));

        for (std::wstring& value : values) {
            if (value.empty()) {
                error = L"empty value in '" + part + L"'";
                return false;
            }
            if (pred.field == PredicateField::Scope) {
                Scope scope = Scope::Other;
                if (!ScopeFromName(value, scope)) {
                    error = L"unknown scope '" + value + L"'";
                    return false;
                }
                value = ToLowerCopy(value);
            } else if (pred.field == PredicateField::Label) {
                value = NormalizeMenuLabel(value);
                if (value.empty()) {
                    error = L"empty label in '" + part + L"'";
                    return false;
                }
            }
            pred.values.push_back(std::move(value));
        }
        expr.all.push_back(std::move(pred));
    }

    if (expr.all.empty()) {
        error = L"empty predicate";
        return false;
    }
    out = std::move(expr);
    error.clear();
    return true;
}

struct RulesApplication {
    bool hasMoveRules = false;
};

void HideMatchingItems(std::vector<MenuItem>& items, const PredicateExpr& match,
                       const ItemContext& ctx) {
    std::erase_if(items, [&](const MenuItem& item) {
        return item.action != ActionKind::Fallback &&
               PredicateExprMatches(match, item, ctx);
    });
    for (MenuItem& item : items) {
        if (item.kind == ItemKind::Submenu) {
            HideMatchingItems(item.children, match, ctx);
        }
    }
}

void CollectProtectedIds(const std::vector<MenuItem>& items,
                         const PredicateExpr& match, const ItemContext& ctx,
                         std::unordered_set<uint32_t>& out) {
    for (const MenuItem& item : items) {
        if (PredicateExprMatches(match, item, ctx)) {
            out.insert(item.id);
        }
        CollectProtectedIds(item.children, match, ctx, out);
    }
}

RulesApplication ApplyRulesToModel(MenuModel& model, const RulesConfig& config,
                                   const ItemContext& ctx) {
    RulesApplication application;
    for (const Rule& rule : config.rules) {
        if (rule.kind == RuleKind::Move) {
            application.hasMoveRules = true;
            break;
        }
    }

    for (const Rule& rule : config.rules) {
        if (rule.kind == RuleKind::Hide) {
            HideMatchingItems(model.items, rule.match, ctx);
        }
    }

    std::unordered_set<uint32_t> protectedIds;
    for (const Rule& rule : config.rules) {
        if (rule.kind == RuleKind::Keep) {
            CollectProtectedIds(model.items, rule.match, ctx, protectedIds);
        }
    }

    std::vector<std::wstring> destinations;
    std::unordered_map<std::wstring, std::vector<MenuItem>> movedByDestination;
    for (const Rule& rule : config.rules) {
        if (rule.kind != RuleKind::Move) {
            continue;
        }
        for (size_t i = 0; i < model.items.size();) {
            MenuItem& item = model.items[i];
            if (item.action != ActionKind::Fallback &&
                protectedIds.count(item.id) == 0 &&
                PredicateExprMatches(rule.match, item, ctx)) {
                std::vector<MenuItem>& bucket = movedByDestination[rule.destination];
                if (bucket.empty()) {
                    destinations.push_back(rule.destination);
                }
                bucket.push_back(std::move(item));
                model.items.erase(model.items.begin() +
                                  static_cast<std::ptrdiff_t>(i));
                continue;
            }
            ++i;
        }
    }

    uint32_t destinationId = 0xF700;
    for (const std::wstring& destination : destinations) {
        auto bucket = movedByDestination.find(destination);
        if (bucket == movedByDestination.end() || bucket->second.empty()) {
            continue;
        }

        MenuItem* existing = nullptr;
        for (MenuItem& item : model.items) {
            if (item.kind == ItemKind::Submenu && item.label == destination) {
                existing = &item;
                break;
            }
        }
        if (existing) {
            for (MenuItem& child : bucket->second) {
                existing->children.push_back(std::move(child));
            }
            continue;
        }

        MenuItem submenu{};
        submenu.id = destinationId++;
        submenu.kind = ItemKind::Submenu;
        submenu.action = ActionKind::Submenu;
        submenu.label = destination;
        submenu.children = std::move(bucket->second);

        auto insertAt = model.items.end();
        for (auto it = model.items.begin(); it != model.items.end(); ++it) {
            if (it->action == ActionKind::Fallback) {
                insertAt = it;
                break;
            }
        }
        model.items.insert(insertAt, std::move(submenu));
    }

    CollapseSeparators(model.items);
    return application;
}

bool CommandMatchesContext(const PredicateExpr& expr, const ItemContext& ctx) {
    if (expr.all.empty()) {
        return true;
    }
    MenuItem dummy{};
    return PredicateExprMatches(expr, dummy, ctx);
}

std::vector<MenuItem>::iterator FallbackInsertPoint(std::vector<MenuItem>& items) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->action == ActionKind::Fallback) {
            return it;
        }
    }
    return items.end();
}

struct PendingCustomSubmenu {
    std::wstring name;
    std::wstring iconRef;
    SubmenuPositionKind position = SubmenuPositionKind::Bottom;
    std::wstring positionLabel;
    std::vector<MenuItem> children;
};

void InsertCustomItems(MenuModel& model, const RulesConfig& config,
                       const ItemContext& ctx) {
    auto findSubmenuConfig =
        [&](const std::wstring& name) -> const CustomSubmenu* {
        for (const CustomSubmenu& submenu : config.submenus) {
            if (_wcsicmp(submenu.name.c_str(), name.c_str()) == 0) {
                return &submenu;
            }
        }
        return nullptr;
    };

    std::unordered_map<std::wstring, PendingCustomSubmenu> pending;
    std::vector<std::wstring> pendingOrder;
    std::unordered_set<std::wstring> filteredNames;
    uint32_t syntheticId = 0xF100;
    auto ensureTopLevel = [&](const std::wstring& name) -> PendingCustomSubmenu* {
        auto it = pending.find(name);
        if (it != pending.end()) {
            return filteredNames.count(name) ? nullptr : &it->second;
        }
        PendingCustomSubmenu entry;
        entry.name = name;
        if (const CustomSubmenu* submenu = findSubmenuConfig(name)) {
            if (!CommandMatchesContext(submenu->match, ctx)) {
                filteredNames.insert(name);
                return nullptr;
            }
            entry.iconRef = submenu->iconRef;
            entry.position = submenu->position;
            entry.positionLabel = submenu->positionLabel;
        }
        it = pending.emplace(name, std::move(entry)).first;
        pendingOrder.push_back(name);
        return &it->second;
    };

    auto resolveContainer =
        [&](const std::wstring& path) -> std::vector<MenuItem>* {
        std::vector<std::wstring> segments;
        size_t start = 0;
        while (start <= path.size()) {
            const size_t slash = path.find(L'/', start);
            std::wstring segment = TrimWhitespace(
                path.substr(start, slash == std::wstring::npos
                                        ? std::wstring::npos
                                        : slash - start));
            if (!segment.empty()) {
                segments.push_back(std::move(segment));
            }
            if (slash == std::wstring::npos) {
                break;
            }
            start = slash + 1;
        }
        if (segments.empty() || segments.size() > 3) {
            return nullptr;
        }

        PendingCustomSubmenu* top = ensureTopLevel(segments[0]);
        if (!top) {
            return nullptr;
        }
        std::vector<MenuItem>* current = &top->children;
        for (size_t s = 1; s < segments.size(); ++s) {
            MenuItem* nested = nullptr;
            for (MenuItem& child : *current) {
                if (child.kind == ItemKind::Submenu &&
                    _wcsicmp(child.label.c_str(), segments[s].c_str()) == 0) {
                    nested = &child;
                    break;
                }
            }
            if (!nested) {
                MenuItem submenu{};
                submenu.id = syntheticId++;
                submenu.kind = ItemKind::Submenu;
                submenu.action = ActionKind::Submenu;
                submenu.label = segments[s];
                if (const CustomSubmenu* sc = findSubmenuConfig(segments[s])) {
                    if (!CommandMatchesContext(sc->match, ctx)) {
                        return nullptr;
                    }
                    submenu.iconRef = sc->iconRef;
                }
                current->push_back(std::move(submenu));
                nested = &current->back();
            }
            current = &nested->children;
        }
        return current;
    };

    std::vector<MenuItem> topLevelCommands;
    for (size_t i = 0; i < config.commands.size(); ++i) {
        const CustomCommand& command = config.commands[i];
        if (!CommandMatchesContext(command.match, ctx)) {
            continue;
        }

        if (command.type == CommandType::Command &&
            command.action == BuiltinAction::None && command.command.empty()) {
            // An empty example or accidental command must not become a dead item.
            continue;
        }

        MenuItem item{};
        item.id = 0xF200 + static_cast<uint32_t>(i);
        item.label = command.label;
        if (command.type == CommandType::Separator) {
            item.kind = ItemKind::Separator;
            item.action = ActionKind::ViewAction;
        } else if (command.type == CommandType::Header) {
            item.kind = ItemKind::Header;
            item.action = ActionKind::ViewAction;
        } else if (command.action != BuiltinAction::None) {
            item.kind = ItemKind::Command;
            item.action = ActionKind::Builtin;
            item.builtinAction = command.action;
            item.iconRef = command.iconRef;
        } else {
            item.kind = ItemKind::Command;
            item.action = ActionKind::CustomCommand;
            item.iconRef = command.iconRef;
            item.customCommandIndex = static_cast<uint32_t>(i);
        }

        std::vector<MenuItem>* container = nullptr;
        if (!command.menuPath.empty()) {
            container = resolveContainer(command.menuPath);
            if (!container) {
                // Context-filtered or invalid destination: skip the command.
                continue;
            }
        }
        if (!container) {
            topLevelCommands.push_back(std::move(item));
            continue;
        }

        if (command.type == CommandType::Command &&
            command.separator == CommandSeparator::Before) {
            MenuItem separator{};
            separator.id = 0xF500 + static_cast<uint32_t>(i);
            separator.kind = ItemKind::Separator;
            container->push_back(std::move(separator));
        }
        container->push_back(std::move(item));
        if (command.type == CommandType::Command &&
            command.separator == CommandSeparator::After) {
            MenuItem separator{};
            separator.id = 0xF501 + static_cast<uint32_t>(i);
            separator.kind = ItemKind::Separator;
            container->push_back(std::move(separator));
        }
    }

    // Configured submenus with no referencing command still exist in matching
    // contexts (empty ones are pruned later).
    for (const CustomSubmenu& submenu : config.submenus) {
        if (CommandMatchesContext(submenu.match, ctx)) {
            ensureTopLevel(submenu.name);
        }
    }

    uint32_t pendingId = syntheticId;
    for (const std::wstring& name : pendingOrder) {
        PendingCustomSubmenu& entry = pending[name];
        MenuItem submenu{};
        submenu.id = pendingId++;
        submenu.kind = ItemKind::Submenu;
        submenu.action = ActionKind::Submenu;
        submenu.label = entry.name;
        submenu.iconRef = entry.iconRef;
        submenu.children = std::move(entry.children);

        std::vector<MenuItem>::iterator insertAt = model.items.end();
        if (entry.position == SubmenuPositionKind::Top) {
            insertAt = model.items.begin();
        } else if (entry.position == SubmenuPositionKind::After ||
                   entry.position == SubmenuPositionKind::Before) {
            const std::wstring target = NormalizeMenuLabel(entry.positionLabel);
            for (size_t i = 0; i < model.items.size(); ++i) {
                if (NormalizeMenuLabel(model.items[i].label) == target) {
                    insertAt = model.items.begin() + static_cast<std::ptrdiff_t>(
                        entry.position == SubmenuPositionKind::After ? i + 1 : i);
                    break;
                }
            }
            if (insertAt == model.items.end()) {
                insertAt = FallbackInsertPoint(model.items);
            }
        } else {
            insertAt = FallbackInsertPoint(model.items);
        }
        model.items.insert(insertAt, std::move(submenu));
    }

    for (MenuItem& item : topLevelCommands) {
        const size_t index = item.id - 0xF200;
        const bool separatorBefore =
            index < config.commands.size() &&
            config.commands[index].separator == CommandSeparator::Before;
        const bool separatorAfter =
            index < config.commands.size() &&
            config.commands[index].separator == CommandSeparator::After;
        if (separatorBefore) {
            MenuItem separator{};
            separator.id = 0xF600 + static_cast<uint32_t>(index);
            separator.kind = ItemKind::Separator;
            model.items.insert(FallbackInsertPoint(model.items), std::move(separator));
        }
        model.items.insert(FallbackInsertPoint(model.items), std::move(item));
        if (separatorAfter) {
            MenuItem separator{};
            separator.id = 0xF601 + static_cast<uint32_t>(index);
            separator.kind = ItemKind::Separator;
            model.items.insert(FallbackInsertPoint(model.items), std::move(separator));
        }
    }
}

// ===========================================================================
// [CMO:ConfigStore] Live-reloaded menu.ini.
// ===========================================================================

std::vector<uint8_t> EncodeConfigText(const std::wstring& text) {
    std::vector<uint8_t> bytes;
    bytes.push_back(0xEF);
    bytes.push_back(0xBB);
    bytes.push_back(0xBF);
    if (text.empty()) {
        return bytes;
    }
    const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(),
                                         static_cast<int>(text.size()), nullptr, 0,
                                         nullptr, nullptr);
    if (size <= 0) {
        return bytes;
    }
    const size_t offset = bytes.size();
    bytes.resize(offset + static_cast<size_t>(size));
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                        reinterpret_cast<char*>(bytes.data() + offset), size,
                        nullptr, nullptr);
    return bytes;
}

bool DecodeConfigBytes(const std::vector<uint8_t>& bytes, std::wstring& text) {
    if (bytes.empty()) {
        text.clear();
        return true;
    }

    auto decodeUtf8 = [&](size_t offset, std::wstring& out) {
        const int length = static_cast<int>(bytes.size() - offset);
        if (length <= 0) {
            out.clear();
            return true;
        }
        const int wide = MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            reinterpret_cast<const char*>(bytes.data() + offset), length, nullptr, 0);
        if (wide <= 0) {
            return false;
        }
        out.resize(static_cast<size_t>(wide));
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                            reinterpret_cast<const char*>(bytes.data() + offset),
                            length, out.data(), wide);
        return true;
    };
    auto decodeUtf16 = [&](size_t offset, bool bigEndian, std::wstring& out) {
        const size_t count = (bytes.size() - offset) / 2;
        out.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const uint8_t first = bytes[offset + i * 2];
            const uint8_t second = bytes[offset + i * 2 + 1];
            out[i] = static_cast<wchar_t>(bigEndian
                                              ? (first << 8) | second
                                              : first | (second << 8));
        }
        return true;
    };

    if (bytes.size() >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        return decodeUtf8(3, text);
    }
    if (bytes.size() >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        return decodeUtf16(2, false, text);
    }
    if (bytes.size() >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) {
        return decodeUtf16(2, true, text);
    }

    // Strict UTF-8, but reject results containing embedded NULs: those are
    // almost certainly UTF-16 read as bytes.
    std::wstring utf8Text;
    if (decodeUtf8(0, utf8Text) &&
        utf8Text.find(L'\0') == std::wstring::npos) {
        text = std::move(utf8Text);
        return true;
    }

    // BOM-less UTF-16LE heuristic: even length and enough NUL high bytes.
    if (bytes.size() % 2 == 0) {
        size_t zeros = 0;
        for (size_t i = 1; i < bytes.size(); i += 2) {
            if (bytes[i] == 0) {
                ++zeros;
            }
        }
        if (zeros >= bytes.size() / 4) {
            return decodeUtf16(0, false, text);
        }
    }

    text.clear();
    return false;
}

bool ReadConfigFile(const std::wstring& path, std::wstring& text) {
    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file, &size) || size.QuadPart > 1024 * 1024) {
        CloseHandle(file);
        return false;
    }
    std::vector<uint8_t> bytes(static_cast<size_t>(size.QuadPart));
    DWORD read = 0;
    const BOOL ok =
        bytes.empty() ||
        ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read,
                 nullptr);
    CloseHandle(file);
    if (!ok) {
        return false;
    }
    bytes.resize(read);
    return DecodeConfigBytes(bytes, text);
}

// Atomic: write a sibling temp file, flush it, then replace the destination.
// A crash or full disk can never leave a truncated config behind.
bool WriteConfigFile(const std::wstring& path, const std::wstring& text) {
    const std::vector<uint8_t> bytes = EncodeConfigText(text);
    const std::wstring tempPath = path + L".tmp";
    HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    const BOOL ok =
        bytes.empty() ||
        WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written,
                  nullptr);
    if (ok && written == bytes.size()) {
        FlushFileBuffers(file);
    }
    CloseHandle(file);
    if (!ok || written != bytes.size()) {
        DeleteFileW(tempPath.c_str());
        return false;
    }
    if (!MoveFileExW(tempPath.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tempPath.c_str());
        return false;
    }
    return true;
}

std::wstring ConfigFilePath() {
    wchar_t storagePath[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        return L"";
    }
    return std::wstring(storagePath) + L"\\menu.ini";
}

class ConfigStore {
public:
    void EnsureLoaded() {
        if (loaded_.load()) {
            return;
        }
        loaded_.store(true);
        const std::wstring path = ConfigFilePath();
        if (path.empty()) {
            return;
        }
        const size_t slash = path.find_last_of(L'\\');
        if (slash != std::wstring::npos) {
            CreateDirectoryW(path.substr(0, slash).c_str(), nullptr);
        }
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            WriteConfigFile(path, GenerateDefaultConfigText());
        }
        std::wstring text;
        if (ReadConfigFile(path, text)) {
            ApplyAndMigrate(path, text);
        } else if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"menu.ini: unable to read or decode the file");
        }
        UpdateStamp(path);
    }

    // Called at menu open: reloads only when the file changed.
    void RefreshIfChanged() {
        EnsureLoaded();
        const std::wstring path = ConfigFilePath();
        if (path.empty()) {
            return;
        }
        WIN32_FILE_ATTRIBUTE_DATA attributes = {};
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes)) {
            return;  // Deleted: keep the last good snapshot.
        }
        const uint64_t size =
            (static_cast<uint64_t>(attributes.nFileSizeHigh) << 32) |
            attributes.nFileSizeLow;
        {
            // The management thread can call this too (settings change), so the
            // stamp fields are guarded.
            std::lock_guard<std::mutex> lock(mutex_);
            if (stampValid_ && size == stampSize_ &&
                attributes.ftLastWriteTime.dwLowDateTime == stampTimeLow_ &&
                attributes.ftLastWriteTime.dwHighDateTime == stampTimeHigh_) {
                return;
            }
            stampValid_ = true;
            stampSize_ = size;
            stampTimeLow_ = attributes.ftLastWriteTime.dwLowDateTime;
            stampTimeHigh_ = attributes.ftLastWriteTime.dwHighDateTime;
        }

        std::wstring text;
        if (ReadConfigFile(path, text)) {
            ApplyAndMigrate(path, text);
        } else if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"menu.ini: unable to read or decode the file");
        }
        UpdateStamp(path);
    }

    std::shared_ptr<const RulesConfig> Snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }

    uint64_t Revision() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_ ? config_->revision : 0;
    }

#ifdef CMO_TESTING
    bool ApplyTextForTesting(const std::wstring& text);
#endif

private:
    void ApplyAndMigrate(const std::wstring& path, const std::wstring& text) {
        std::vector<ConfigParseError> warnings;
        ApplyText(text, &warnings);
        const bool older = ReadSchemaVersion(text) < kConfigSchemaVersion;
        if (older || !warnings.empty()) {
            // Rewrite only when it changes the file: corrected values become
            // visible, while unparseable structured content is preserved
            // without rewrite churn.
            const std::wstring canonical =
                CanonicalizeConfig(text, kConfigSchemaVersion);
            if (canonical != text) {
                WriteConfigFile(path, canonical);
            }
        }
    }

    void UpdateStamp(const std::wstring& path) {
        WIN32_FILE_ATTRIBUTE_DATA attributes = {};
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes)) {
            std::lock_guard<std::mutex> lock(mutex_);
            stampValid_ = false;
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        stampValid_ = true;
        stampSize_ = (static_cast<uint64_t>(attributes.nFileSizeHigh) << 32) |
                     attributes.nFileSizeLow;
        stampTimeLow_ = attributes.ftLastWriteTime.dwLowDateTime;
        stampTimeHigh_ = attributes.ftLastWriteTime.dwHighDateTime;
    }

    bool ApplyText(const std::wstring& text,
                   std::vector<ConfigParseError>* outWarnings = nullptr) {
        RulesConfig parsed;
        std::vector<ConfigParseError> warnings;
        ParseRulesConfig(text, parsed, warnings);
        for (const ConfigParseError& warning : warnings) {
            Wh_Log(L"menu.ini:%d: warning: %s", warning.line,
                   warning.message.c_str());
        }
        if (outWarnings) {
            *outWarnings = warnings;
        }
        uint64_t previousRevision = 0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            previousRevision = config_ ? config_->revision : 0;
        }
        parsed.revision = previousRevision + 1;
        auto snapshot = std::make_shared<const RulesConfig>(std::move(parsed));
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = std::move(snapshot);
        return true;
    }

    mutable std::mutex mutex_;
    std::shared_ptr<const RulesConfig> config_;
    std::atomic<bool> loaded_{false};
    bool stampValid_ = false;
    uint64_t stampSize_ = 0;
    DWORD stampTimeLow_ = 0;
    DWORD stampTimeHigh_ = 0;
};

inline ConfigStore g_configStore;

void ApplyItemOverrides(std::vector<MenuItem>& items, const RulesConfig& config,
                        const ItemContext& ctx) {
    for (MenuItem& item : items) {
        for (const ItemOverride& override : config.overrides) {
            if (!PredicateExprMatches(override.match, item, ctx)) {
                continue;
            }
            if (override.hasIcon) {
                item.iconRef = override.iconRef;
            }
            if (override.hasLabel) {
                item.displayLabel = override.label;
            }
            if (override.hasMarker) {
                item.markerOverride = static_cast<int>(override.marker);
            }
        }
        ApplyItemOverrides(item.children, config, ctx);
    }
}

RulesApplication ApplyRulesConfigToModel(MenuModel& model,
                                         const RulesConfig& config,
                                         const ItemContext& ctx) {
    RulesApplication application = ApplyRulesToModel(model, config, ctx);
    ApplyItemOverrides(model.items, config, ctx);
    InsertCustomItems(model, config, ctx);
    return application;
}

struct AdvancedGroupingOptions {
    bool moveWindows = true;
    bool moveThirdParty = true;
    std::wstring label = L"More options";
    std::vector<std::wstring> windowsItems;
    std::vector<std::wstring> exclude;
    const RulesConfig* rules = nullptr;
    ItemContext ctx;
};

bool IsThirdPartyItem(const MenuItem& item) {
    if (item.kind == ItemKind::Separator || item.action == ActionKind::Fallback) {
        return false;
    }
    if (item.flags & kModelThirdParty) {
        return true;
    }
    // The unknown-verb heuristic applies to command items only: Windows
    // submenus such as New or Give access to carry no verb and must not be
    // swept into the advanced submenu.
    return item.kind == ItemKind::Command &&
           (item.flags & kModelExtension) != 0 &&
           !IsKnownWindowsVerb(item.canonicalVerb);
}

bool MatchesAnyToken(const MenuItem& item,
                     const std::vector<std::wstring>& tokens) {
    for (const std::wstring& token : tokens) {
        if (LabelsMatchIgnoreCase(item.label, token) ||
            EqualsIgnoreCase(item.canonicalVerb, token)) {
            return true;
        }
    }
    return false;
}

bool IsKeptByRules(const MenuItem& item,
                   const AdvancedGroupingOptions& options) {
    if (!options.rules) {
        return false;
    }
    for (const Rule& rule : options.rules->rules) {
        if (rule.kind == RuleKind::Keep &&
            PredicateExprMatches(rule.match, item, options.ctx)) {
            return true;
        }
    }
    return false;
}

void ReorganizeAdvancedItems(std::vector<MenuItem>& items,
                             const AdvancedGroupingOptions& options) {
    if ((!options.moveWindows && !options.moveThirdParty) ||
        options.label.empty()) {
        return;
    }

    std::vector<MenuItem> advanced;
    std::vector<MenuItem> kept;
    kept.reserve(items.size());
    for (MenuItem& item : items) {
        const bool selectable =
            item.kind != ItemKind::Separator &&
            item.action != ActionKind::Fallback;
        const bool windowsExtra =
            options.moveWindows && selectable &&
            MatchesAnyToken(item, options.windowsItems);
        const bool thirdParty =
            options.moveThirdParty && selectable && IsThirdPartyItem(item);
        const bool protectedItem =
            MatchesAnyToken(item, options.exclude) ||
            IsKeptByRules(item, options);
        if ((windowsExtra || thirdParty) && !protectedItem) {
            advanced.push_back(std::move(item));
        } else {
            kept.push_back(std::move(item));
        }
    }
    if (g_settings->debugLogging) {
        for (const MenuItem& item : advanced) {
            Wh_Log(L"More options: moved '%s' (verb '%s')", item.label.c_str(),
                   item.canonicalVerb.c_str());
        }
        for (const MenuItem& item : kept) {
            if (item.kind != ItemKind::Separator &&
                item.action != ActionKind::Fallback &&
                (item.flags & kModelExtension)) {
                Wh_Log(L"More options: kept '%s' (verb '%s')", item.label.c_str(),
                       item.canonicalVerb.c_str());
            }
        }
    }
    if (advanced.empty()) {
        items = std::move(kept);
        return;
    }

    const auto customStart = std::stable_partition(
        advanced.begin(), advanced.end(), [&](const MenuItem& item) {
            return IsBuiltinExtra(item, options.windowsItems);
        });
    if (customStart != advanced.begin() && customStart != advanced.end()) {
        MenuItem separator{};
        separator.id = 0xF001;
        separator.kind = ItemKind::Separator;
        advanced.insert(customStart, std::move(separator));
    }

    MenuItem submenu{};
    submenu.id = 0xF000;
    submenu.kind = ItemKind::Submenu;
    submenu.action = ActionKind::Submenu;
    submenu.label = options.label;
    submenu.iconRef = L"@glyph:E712";
    submenu.children = std::move(advanced);

    size_t insertAt = kept.size();
    for (size_t i = 0; i < kept.size(); ++i) {
        if (kept[i].action == ActionKind::Fallback) {
            insertAt = i;
            break;
        }
    }
    while (insertAt > 0 && kept[insertAt - 1].kind == ItemKind::Separator) {
        --insertAt;
    }
    kept.insert(kept.begin() + static_cast<std::ptrdiff_t>(insertAt),
                std::move(submenu));
    CollapseSeparators(kept);
    items = std::move(kept);
}

// ===========================================================================
// [CMO:Icons] Named symbol and Windows stock icon sources.
// ===========================================================================

struct NamedIcon {
    const wchar_t* name;
    const wchar_t* glyph;
};

const NamedIcon kNamedIcons[] = {
    {L"copy", L"E8C8"},        {L"cut", L"E8C6"},
    {L"paste", L"E77F"},       {L"delete", L"E74D"},
    {L"rename", L"E8AC"},      {L"properties", L"E713"},
    {L"refresh", L"E72C"},     {L"open", L"E8E5"},
    {L"openwith", L"E7AC"},    {L"folder", L"E8B7"},
    {L"file", L"E8A5"},        {L"drive", L"EDA2"},
    {L"network", L"E968"},     {L"share", L"E72D"},
    {L"pin", L"E718"},         {L"unpin", L"E77A"},
    {L"new", L"E710"},         {L"link", L"E71B"},
    {L"terminal", L"E756"},    {L"run", L"E768"},
    {L"admin", L"E7EF"},       {L"search", L"E721"},
    {L"filter", L"E71C"},      {L"check", L"E73E"},
    {L"star", L"E734"},        {L"lock", L"E72E"},
    {L"info", L"E946"},        {L"warning", L"E7BA"},
    {L"error", L"E783"},       {L"up", L"E74A"},
    {L"down", L"E74B"},        {L"left", L"E72B"},
    {L"right", L"E72A"},       {L"more", L"E712"},
    {L"close", L"E8BB"},       {L"settings", L"E713"},
    {L"personalize", L"E790"}, {L"display", L"E7F4"},
    {L"selectall", L"E8B3"},   {L"sort", L"E8CB"},
    {L"group", L"E902"},       {L"view-xlarge", L"E7C4"},
    {L"view-large", L"E739"},  {L"view-medium", L"E80A"},
    {L"view-small", L"E8A9"},  {L"view-list", L"E8FD"},
    {L"view-details", L"E8EF"}, {L"view-tiles", L"E8B3"},
    {L"view-content", L"E71D"},
};

bool ResolveNamedIcon(const std::wstring& name, std::wstring& glyph) {
    for (const NamedIcon& icon : kNamedIcons) {
        if (_wcsicmp(icon.name, name.c_str()) == 0) {
            glyph = icon.glyph;
            return true;
        }
    }
    return false;
}

struct StockIcon {
    const wchar_t* name;
    int id;
    bool shellStock;  // true: SHSTOCKICONID, false: IDI_* resource id
};

const StockIcon kStockIcons[] = {
    {L"info", 32516, false},      // IDI_INFORMATION
    {L"warning", 32515, false},   // IDI_WARNING
    {L"error", 32513, false},     // IDI_ERROR
    {L"question", 32514, false},  // IDI_QUESTION
    {L"shield", 32518, false},    // IDI_SHIELD
    {L"folder", SIID_FOLDER, true},
    {L"drive", SIID_DRIVEFIXED, true},
    {L"network", SIID_MYNETWORK, true},
    {L"computer", SIID_DESKTOPPC, true},
    {L"desktop", SIID_FOLDER, true},
    {L"documents", SIID_DOCNOASSOC, true},
    {L"downloads", SIID_FOLDER, true},
    {L"music", SIID_AUDIOFILES, true},
    {L"pictures", SIID_IMAGEFILES, true},
    {L"videos", SIID_VIDEOFILES, true},
    {L"recycle", SIID_RECYCLER, true},
};

bool ResolveStockIcon(const std::wstring& name, int& id, bool& shellStock) {
    for (const StockIcon& icon : kStockIcons) {
        if (_wcsicmp(icon.name, name.c_str()) == 0) {
            id = icon.id;
            shellStock = icon.shellStock;
            return true;
        }
    }
    return false;
}

// Glyph codepoint for @glyph:XXXX or a resolved @icon:<name>; empty otherwise.
std::wstring IconRefGlyph(const std::wstring& iconRef) {
    if (iconRef.rfind(L"@glyph:", 0) == 0) {
        return iconRef.substr(7);
    }
    if (iconRef.rfind(L"@icon:", 0) == 0) {
        std::wstring glyph;
        if (ResolveNamedIcon(iconRef.substr(6), glyph)) {
            return glyph;
        }
    }
    return L"";
}

// ===========================================================================
// [CMO:Themes] Bundled appearance presets.
// ===========================================================================

struct ThemePreset {
    const wchar_t* name;
    const wchar_t* snippet;
};

const ThemePreset kThemes[] = {    {L"Custom (menu.ini)", L""},    {L"Windows 11 Dark",
     LR"INI([appearance]
background = 32, 32, 32, 242
border = 255, 255, 255, 34
separator = 255, 255, 255, 24
hoverBackground = 255, 255, 255, 20
pressedBackground = 255, 255, 255, 34
textColor = 255, 255, 255, 255
disabledTextColor = 255, 255, 255, 102
submenuArrow = 255, 255, 255, 153
headerColor = 255, 255, 255, 102
markerColor = 96, 165, 250, 255
cornerRadius = 8
shadowOpacity = 120
shadowBlur = 12
animationOpen = fade
animationClose = fade
animationDuration = 100
animationEasing = easeOut
)INI"},    {L"Windows 11 Light",
     LR"INI([appearance]
background = 243, 243, 243, 242
border = 0, 0, 0, 22
separator = 0, 0, 0, 18
hoverBackground = 0, 0, 0, 12
pressedBackground = 0, 0, 0, 20
textColor = 32, 32, 32, 255
disabledTextColor = 96, 96, 96, 255
submenuArrow = 0, 0, 0, 120
headerColor = 96, 96, 96, 255
markerColor = 0, 120, 212, 255
cornerRadius = 8
shadowOpacity = 90
shadowBlur = 12
animationOpen = fade
animationClose = fade
animationDuration = 100
animationEasing = easeOut
)INI"},    {L"Windows 10 Dark",
     LR"INI([appearance]
background = 31, 31, 31, 255
border = 70, 70, 70, 255
separator = 70, 70, 70, 255
hoverBackground = 51, 51, 51, 255
pressedBackground = 70, 70, 70, 255
textColor = 255, 255, 255, 255
disabledTextColor = 130, 130, 130, 255
submenuArrow = 200, 200, 200, 255
headerColor = 130, 130, 130, 255
markerColor = 0, 153, 255, 255
cornerRadius = 0
shadow = false
blur = false
animationOpen = none
animationClose = none
animationDuration = 90
)INI"},    {L"Windows 10 Light",
     LR"INI([appearance]
background = 240, 240, 240, 255
border = 160, 160, 160, 255
separator = 200, 200, 200, 255
hoverBackground = 220, 220, 220, 255
pressedBackground = 200, 200, 200, 255
textColor = 0, 0, 0, 255
disabledTextColor = 120, 120, 120, 255
submenuArrow = 60, 60, 60, 255
headerColor = 120, 120, 120, 255
markerColor = 0, 102, 204, 255
cornerRadius = 0
shadow = false
blur = false
animationOpen = none
animationClose = none
animationDuration = 90
)INI"},    {L"Nord",
     LR"INI([appearance]
background = 46, 52, 64, 242
border = 76, 86, 106, 255
separator = 76, 86, 106, 180
hoverBackground = 59, 66, 82, 255
pressedBackground = 76, 86, 106, 255
textColor = 216, 222, 233, 255
disabledTextColor = 143, 188, 187, 140
submenuArrow = 216, 222, 233, 170
headerColor = 143, 188, 187, 255
markerColor = 136, 192, 208, 255
cornerRadius = 6
shadowOpacity = 130
animationOpen = fade
animationClose = fade
animationDuration = 100
)INI"},    {L"Dracula",
     LR"INI([appearance]
background = 40, 42, 54, 242
border = 68, 71, 90, 255
separator = 68, 71, 90, 180
hoverBackground = 68, 71, 90, 255
pressedBackground = 98, 114, 164, 255
textColor = 248, 248, 242, 255
disabledTextColor = 98, 114, 164, 180
submenuArrow = 189, 147, 249, 255
headerColor = 139, 233, 253, 255
markerColor = 80, 250, 123, 255
cornerRadius = 6
shadowOpacity = 130
animationOpen = fade, slide
animationClose = fade, slide
animationDuration = 100
slideOffsetX = 8
)INI"},    {L"Solarized Dark",
     LR"INI([appearance]
background = 0, 43, 54, 242
border = 7, 54, 66, 255
separator = 88, 110, 117, 160
hoverBackground = 7, 54, 66, 255
pressedBackground = 88, 110, 117, 255
textColor = 147, 161, 161, 255
disabledTextColor = 88, 110, 117, 180
submenuArrow = 131, 148, 150, 255
headerColor = 38, 139, 210, 255
markerColor = 181, 137, 0, 255
cornerRadius = 4
shadowOpacity = 140
animationOpen = fade
animationClose = fade
animationDuration = 100
)INI"},    {L"Gruvbox Dark",
     LR"INI([appearance]
background = 40, 40, 40, 242
border = 80, 73, 69, 255
separator = 80, 73, 69, 180
hoverBackground = 60, 56, 54, 255
pressedBackground = 80, 73, 69, 255
textColor = 235, 219, 178, 255
disabledTextColor = 146, 131, 116, 180
submenuArrow = 213, 196, 161, 255
headerColor = 215, 153, 33, 255
markerColor = 215, 153, 33, 255
cornerRadius = 6
shadowOpacity = 130
animationOpen = fade
animationClose = fade
animationDuration = 100
animationEasing = easeOut
)INI"},    {L"One Dark",
     LR"INI([appearance]
background = 40, 44, 52, 242
border = 62, 68, 81, 255
separator = 62, 68, 81, 180
hoverBackground = 50, 56, 66, 255
pressedBackground = 62, 68, 81, 255
textColor = 171, 178, 191, 255
disabledTextColor = 92, 99, 112, 200
submenuArrow = 152, 160, 174, 255
headerColor = 97, 175, 239, 255
markerColor = 97, 175, 239, 255
cornerRadius = 6
shadowOpacity = 130
animationOpen = fade, slide
animationClose = fade, slide
animationDuration = 100
slideOffsetX = 6
)INI"},    {L"Cyberpunk",
     LR"INI([appearance]
background = 8, 10, 20, 245
border = 255, 234, 0, 90
separator = 0, 240, 255, 60
hoverBackground = 255, 0, 128, 45
pressedBackground = 255, 234, 0, 70
textColor = 0, 245, 255, 255
disabledTextColor = 120, 130, 160, 200
submenuArrow = 255, 0, 128, 255
headerColor = 255, 234, 0, 255
markerColor = 255, 234, 0, 255
cornerRadius = 4
shadowColor = 255, 0, 128, 255
shadowOpacity = 200
shadowBlur = 16
animationOpen = fade, slide
animationClose = fade, slide
animationDuration = 110
animationEasing = easeOut
slideOffsetX = 18
)INI"},    {L"Synthwave",
     LR"INI([appearance]
background = 26, 16, 48, 245
border = 255, 110, 199, 110
separator = 0, 240, 255, 70
hoverBackground = 255, 110, 199, 40
pressedBackground = 0, 240, 255, 55
textColor = 255, 200, 240, 255
disabledTextColor = 150, 120, 190, 200
submenuArrow = 0, 240, 255, 255
headerColor = 0, 240, 255, 255
markerColor = 255, 110, 199, 255
cornerRadius = 8
shadowColor = 160, 60, 255, 255
shadowOpacity = 190
shadowBlur = 18
animationOpen = unfold
animationClose = unfold
animationDuration = 120
)INI"},    {L"Terminal Green",
     LR"INI([appearance]
background = 4, 12, 6, 250
border = 0, 255, 120, 90
separator = 0, 200, 90, 70
hoverBackground = 0, 255, 120, 35
pressedBackground = 0, 255, 120, 60
textColor = 140, 255, 170, 255
disabledTextColor = 60, 130, 80, 220
submenuArrow = 0, 255, 120, 255
headerColor = 0, 255, 120, 255
markerColor = 0, 255, 120, 255
cornerRadius = 0
shadowColor = 0, 255, 120, 255
shadowOpacity = 150
shadowBlur = 14
blur = false
font = Consolas, 9
overlay = scanlines
overlayIntensity = 30
animationOpen = crt
animationClose = crt
animationDuration = 110
)INI"},    {L"Amber CRT",
     LR"INI([appearance]
background = 16, 10, 2, 250
border = 255, 176, 0, 100
separator = 255, 176, 0, 70
hoverBackground = 255, 176, 0, 35
pressedBackground = 255, 176, 0, 60
textColor = 255, 200, 90, 255
disabledTextColor = 150, 110, 50, 220
submenuArrow = 255, 176, 0, 255
headerColor = 255, 176, 0, 255
markerColor = 255, 176, 0, 255
cornerRadius = 0
shadowColor = 255, 176, 0, 255
shadowOpacity = 150
shadowBlur = 14
blur = false
font = Consolas, 9
overlay = scanlines
overlayIntensity = 30
animationOpen = crt
animationClose = crt
animationDuration = 110
)INI"},    {L"Tokyo Night",
     LR"INI([appearance]
background = 26, 27, 38, 242
border = 61, 89, 161, 255
separator = 61, 89, 161, 160
hoverBackground = 41, 46, 66, 255
pressedBackground = 61, 89, 161, 255
textColor = 169, 177, 214, 255
disabledTextColor = 86, 95, 137, 200
submenuArrow = 122, 162, 247, 255
headerColor = 122, 162, 247, 255
markerColor = 187, 154, 247, 255
cornerRadius = 6
shadowColor = 30, 40, 90, 255
shadowOpacity = 160
shadowBlur = 14
animationOpen = fade, slide
animationClose = fade, slide
animationDuration = 100
slideOffsetX = 8
)INI"},    {L"AMOLED Black",
     LR"INI([appearance]
background = 0, 0, 0, 252
border = 40, 40, 40, 255
separator = 40, 40, 40, 180
hoverBackground = 30, 30, 30, 255
pressedBackground = 45, 45, 45, 255
textColor = 255, 255, 255, 255
disabledTextColor = 110, 110, 110, 255
submenuArrow = 180, 180, 180, 255
headerColor = 150, 150, 150, 255
markerColor = 0, 200, 255, 255
cornerRadius = 8
shadow = false
shadowOpacity = 0
blur = false
animationOpen = fade
animationClose = fade
animationDuration = 90
)INI"},    {L"High Contrast",
     LR"INI([appearance]
background = 0, 0, 0, 255
border = 255, 255, 255, 255
borderWidth = 2
separator = 255, 255, 255, 200
hoverBackground = 255, 255, 255, 40
pressedBackground = 255, 255, 255, 80
textColor = 255, 255, 255, 255
disabledTextColor = 180, 180, 180, 255
submenuArrow = 255, 255, 255, 255
headerColor = 255, 255, 255, 255
markerColor = 255, 255, 0, 255
cornerRadius = 0
shadow = false
blur = false
font = Segoe UI, 11
animationOpen = none
animationClose = none
)INI"},};

constexpr size_t kThemesCount = ARRAYSIZE(kThemes);

int ThemeIndexFromName(const std::wstring& name) {
    for (size_t i = 0; i < kThemesCount; ++i) {
        if (_wcsicmp(kThemes[i].name, name.c_str()) == 0) {
            return static_cast<int>(i);
        }
    }
    return 0;
}

// Canonical [appearance] block extracted from canonicalized config text.
// The [appearance] block of an already-canonical config text, trimmed to the
// section and terminated with a newline.
std::wstring ExtractAppearanceBlock(const std::wstring& canonical) {
    const size_t start = canonical.find(L"[appearance]\n");
    if (start == std::wstring::npos) {
        return L"";
    }
    size_t end = canonical.find(L"\n[", start + 1);
    if (end == std::wstring::npos) {
        end = canonical.size();
    }
    std::wstring block = canonical.substr(start, end - start);
    while (!block.empty() && (block.back() == L'\n' || block.back() == L'\r')) {
        block.pop_back();
    }
    block += L'\n';
    return block;
}

std::wstring CanonicalAppearanceBlock(const std::wstring& configText) {
    return ExtractAppearanceBlock(
        CanonicalizeConfig(configText, kConfigSchemaVersion));
}

// Canonical, complete [appearance] block for a theme: canonicalizing the
// preset snippet fills every missing key from the schema defaults, and only
// the appearance section is kept.
std::wstring GenerateThemeText(int themeIndex) {
    if (themeIndex <= 0 || themeIndex >= static_cast<int>(kThemesCount)) {
        return L"";
    }
    return CanonicalAppearanceBlock(kThemes[themeIndex].snippet);
}

// Removes [appearance.light]/[.dark] blocks so a new theme cannot inherit a
// previous theme's overrides.
std::wstring StripThemeSections(const std::wstring& text) {
    std::wstring out;
    bool skipping = false;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd = newline == std::wstring::npos ? text.size() : newline;
        const std::wstring line = text.substr(pos, lineEnd - pos);
        pos = lineEnd + 1;
        const std::wstring trimmed = TrimWhitespace(StripInlineComment(line));
        if (!trimmed.empty() && trimmed[0] == L'[') {
            std::wstring name;
            if (trimmed.back() == L']') {
                name = ToLowerCopy(
                    TrimWhitespace(trimmed.substr(1, trimmed.size() - 2)));
            }
            skipping = name == L"appearance.light" || name == L"appearance.dark";
        }
        if (!skipping) {
            out += line;
            out += L"\n";
        }
    }
    return out;
}

std::wstring ThemeSlug(const wchar_t* name) {
    std::wstring slug;
    bool pendingDash = false;
    for (const wchar_t* p = name; p && *p; ++p) {
        const wchar_t c = *p;
        const bool alnum = (c >= L'a' && c <= L'z') ||
                           (c >= L'A' && c <= L'Z') ||
                           (c >= L'0' && c <= L'9');
        if (alnum) {
            if (pendingDash && !slug.empty()) {
                slug += L'-';
            }
            pendingDash = false;
            slug += static_cast<wchar_t>(towlower(c));
        } else {
            pendingDash = true;
        }
    }
    return slug;
}

std::wstring ThemeDirPath() {
    wchar_t storagePath[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        return L"";
    }
    return std::wstring(storagePath) + L"\\themes";
}

std::wstring ThemeFilePath(int themeIndex) {
    if (themeIndex <= 0 || themeIndex >= static_cast<int>(kThemesCount)) {
        return L"";
    }
    const std::wstring dir = ThemeDirPath();
    if (dir.empty()) {
        return L"";
    }
    return dir + L"\\" + ThemeSlug(kThemes[themeIndex].name) + L".ini";
}

// Drops the lines the parser warned about so an invalid or unknown theme
// value does not override the template value it falls back to.
std::wstring DropWarnedLines(const std::wstring& text,
                             const std::vector<ConfigParseError>& warnings) {
    if (warnings.empty() || text.empty()) {
        return text;
    }
    std::wstring out;
    int line = 0;
    bool droppingSection = false;
    size_t pos = 0;
    while (pos < text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd =
            newline == std::wstring::npos ? text.size() : newline;
        ++line;
        bool warned = false;
        for (const ConfigParseError& warning : warnings) {
            if (warning.line == line) {
                warned = true;
                break;
            }
        }
        const std::wstring trimmed = TrimWhitespace(
            StripInlineComment(text.substr(pos, lineEnd - pos)));
        const bool isHeader = !trimmed.empty() && trimmed[0] == L'[';
        bool drop = warned;
        if (isHeader) {
            // A dropped section header takes its whole body with it; otherwise
            // the remaining keys would attach to the previous section.
            droppingSection = warned;
        } else if (droppingSection) {
            drop = true;
        }
        if (!drop) {
            out.append(text, pos, lineEnd - pos);
            out += L'\n';
        }
        if (newline == std::wstring::npos) {
            break;
        }
        pos = newline + 1;
    }
    return out;
}

// True when the text contains an [appearance.light] or [appearance.dark]
// section. Theme files are single-palette; the store logs this so copied
// menu.ini overrides are not dropped silently.
// True when the text declares any section other than the appearance block
// (and [meta]); those are dropped when a theme file is rewritten.
bool ThemeTextHasNonAppearanceSections(const std::wstring& text) {
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd =
            newline == std::wstring::npos ? text.size() : newline;
        const std::wstring trimmed = TrimWhitespace(
            StripInlineComment(text.substr(pos, lineEnd - pos)));
        if (!trimmed.empty() && trimmed[0] == L'[' && trimmed.back() == L']') {
            const std::wstring name = ToLowerCopy(
                TrimWhitespace(trimmed.substr(1, trimmed.size() - 2)));
            if (name != L"appearance" && name != L"appearance.light" &&
                name != L"appearance.dark" && name != L"meta") {
                return true;
            }
        }
        if (newline == std::wstring::npos) {
            break;
        }
        pos = newline + 1;
    }
    return false;
}

bool ThemeTextHasSubThemeSections(const std::wstring& text) {
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd =
            newline == std::wstring::npos ? text.size() : newline;
        const std::wstring trimmed = TrimWhitespace(
            StripInlineComment(text.substr(pos, lineEnd - pos)));
        if (!trimmed.empty() && trimmed[0] == L'[' && trimmed.back() == L']') {
            const std::wstring name = ToLowerCopy(
                TrimWhitespace(trimmed.substr(1, trimmed.size() - 2)));
            if (name == L"appearance.light" || name == L"appearance.dark") {
                return true;
            }
        }
        if (newline == std::wstring::npos) {
            break;
        }
        pos = newline + 1;
    }
    return false;
}

// Loads the selected theme from <storage>\themes\<slug>.ini. Theme files are
// complete, self-contained [appearance] blocks: missing keys take the preset
// value, invalid values fall back to it too, and menu.ini is never touched.
class ThemeStore {
public:
    void EnsureLoaded() {
        std::lock_guard<std::mutex> lock(mutex_);
        const int index = g_settings->themeIndex;
        if (index <= 0) {
            ClearLocked();
            return;
        }
        if (loaded_ && loadedIndex_ == index && StampMatchesLocked()) {
            return;
        }
        LoadLocked(index);
    }

    void RefreshIfChanged() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (loadedIndex_ <= 0 || StampMatchesLocked()) {
            return;
        }
        LoadLocked(loadedIndex_);
    }

    std::shared_ptr<const Appearance> Snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return appearance_;
    }

    uint64_t Revision() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return revision_;
    }

    void ApplySelectedTheme(int themeIndex) {
        g_lastAppliedTheme = themeIndex;
        std::lock_guard<std::mutex> lock(mutex_);
        if (themeIndex <= 0) {
            ClearLocked();
            return;
        }
        LoadLocked(themeIndex);
    }

private:
    void ClearLocked() {
        const bool hadTheme = appearance_ != nullptr || loaded_;
        appearance_.reset();
        loaded_ = false;
        loadedIndex_ = -1;
        stampValid_ = false;
        path_.clear();
        if (hadTheme) {
            ++revision_;  // only signal a real appearance change
        }
    }

    bool StampMatchesLocked() {
        if (!stampValid_ || path_.empty()) {
            return false;
        }
        WIN32_FILE_ATTRIBUTE_DATA attributes = {};
        if (!GetFileAttributesExW(path_.c_str(), GetFileExInfoStandard,
                                  &attributes)) {
            return false;
        }
        const uint64_t size =
            (static_cast<uint64_t>(attributes.nFileSizeHigh) << 32) |
            attributes.nFileSizeLow;
        return size == stampSize_ &&
               attributes.ftLastWriteTime.dwLowDateTime == stampTimeLow_ &&
               attributes.ftLastWriteTime.dwHighDateTime == stampTimeHigh_;
    }

    void UpdateStampLocked() {
        WIN32_FILE_ATTRIBUTE_DATA attributes = {};
        if (!GetFileAttributesExW(path_.c_str(), GetFileExInfoStandard,
                                  &attributes)) {
            stampValid_ = false;
            return;
        }
        stampValid_ = true;
        stampSize_ = (static_cast<uint64_t>(attributes.nFileSizeHigh) << 32) |
                     attributes.nFileSizeLow;
        stampTimeLow_ = attributes.ftLastWriteTime.dwLowDateTime;
        stampTimeHigh_ = attributes.ftLastWriteTime.dwHighDateTime;
    }

    void LoadLocked(int themeIndex) {
        const std::wstring path = ThemeFilePath(themeIndex);
        if (path.empty()) {
            ClearLocked();
            return;
        }
        const std::wstring dir = ThemeDirPath();
        if (!dir.empty()) {
            CreateDirectoryW(dir.c_str(), nullptr);
        }

        const std::wstring templateText = GenerateThemeText(themeIndex);
        std::wstring fileText;
        const bool fileExists =
            GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
        const bool hasFile = ReadConfigFile(path, fileText);
        const std::wstring slug = ThemeSlug(kThemes[themeIndex].name);
        std::vector<ConfigParseError> warnings;
        if (hasFile) {
            RulesConfig fileOnly;
            ParseRulesConfig(fileText, fileOnly, warnings);
            for (const ConfigParseError& warning : warnings) {
                Wh_Log(L"themes\\%s.ini:%d: warning: %s", slug.c_str(),
                       warning.line, warning.message.c_str());
            }
            if (ThemeTextHasSubThemeSections(fileText)) {
                Wh_Log(L"themes\\%s.ini: [appearance.light]/[appearance.dark] "
                       L"are ignored in theme files",
                       slug.c_str());
            }
            if (ThemeTextHasNonAppearanceSections(fileText)) {
                Wh_Log(L"themes\\%s.ini: non-appearance sections are ignored "
                       L"and removed on rewrite",
                       slug.c_str());
            }
        } else if (fileExists) {
            // Unreadable file: keep it instead of replacing it with a template.
            Wh_Log(L"themes\\%s.ini: unable to read; leaving the file untouched",
                   slug.c_str());
        }

        std::wstring combined = templateText;
        combined += L'\n';
        combined += StripThemeSections(
            hasFile ? DropWarnedLines(fileText, warnings) : L"");
        RulesConfig effective;
        std::vector<ConfigParseError> combinedWarnings;
        ParseRulesConfig(combined, effective, combinedWarnings);
        for (size_t i = warnings.size(); i < combinedWarnings.size(); ++i) {
            Wh_Log(L"themes\\%s.ini: warning: %s", slug.c_str(),
                   combinedWarnings[i].message.c_str());
        }

        const std::wstring canonical = CanonicalAppearanceBlock(combined);
        const bool writeSafe = !fileExists || hasFile;
        if (writeSafe && !canonical.empty() &&
            (!hasFile || fileText != canonical)) {
            WriteConfigFile(path, canonical);
        }

        appearance_ = std::make_shared<const Appearance>(effective.appearance);
        loaded_ = true;
        loadedIndex_ = themeIndex;
        path_ = path;
        ++revision_;
        UpdateStampLocked();
    }

    mutable std::mutex mutex_;
    std::shared_ptr<const Appearance> appearance_;
    uint64_t revision_ = 0;
    bool loaded_ = false;
    int loadedIndex_ = -1;
    std::wstring path_;
    bool stampValid_ = false;
    uint64_t stampSize_ = 0;
    DWORD stampTimeLow_ = 0;
    DWORD stampTimeHigh_ = 0;
};

inline ThemeStore g_themeStore;

// ===========================================================================
// [CMO:SettingsUI] Schema-driven appearance settings menu.
// ===========================================================================

enum class SettingsTargetKind : uint8_t {
    ThemeFile,
    MenuIniBase,
    MenuIniLight,
    MenuIniDark,
};

struct SettingsTarget {
    SettingsTargetKind kind = SettingsTargetKind::MenuIniBase;
    int themeIndex = 0;
    std::wstring section = L"appearance";
};

// The section the user is currently seeing: the active override when it
// exists, else the base section.
SettingsTargetKind DefaultSettingsTargetKind(bool hasLight, bool hasDark,
                                             bool darkSystemTheme) {
    if (darkSystemTheme && hasDark) {
        return SettingsTargetKind::MenuIniDark;
    }
    if (!darkSystemTheme && hasLight) {
        return SettingsTargetKind::MenuIniLight;
    }
    return SettingsTargetKind::MenuIniBase;
}

SettingsTarget ResolveSettingsTarget(int themeIndex, SettingsTargetKind kind) {
    SettingsTarget target;
    if (themeIndex > 0) {
        target.kind = SettingsTargetKind::ThemeFile;
        target.themeIndex = themeIndex;
        target.section = L"appearance";
        return target;
    }
    target.kind = kind;
    switch (kind) {
        case SettingsTargetKind::MenuIniLight:
            target.section = L"appearance.light";
            break;
        case SettingsTargetKind::MenuIniDark:
            target.section = L"appearance.dark";
            break;
        default:
            target.section = L"appearance";
            break;
    }
    return target;
}


// Typed numeric field: optional '-', digits only; clamps into [min, max].
bool ParseIntField(const std::wstring& text, int minValue, int maxValue,
                   int& out) {
    const std::wstring trimmed = TrimWhitespace(text);
    if (trimmed.empty()) {
        return false;
    }
    size_t pos = 0;
    bool negative = false;
    if (trimmed[pos] == L'-') {
        negative = true;
        ++pos;
    }
    if (pos >= trimmed.size()) {
        return false;
    }
    long long value = 0;
    for (; pos < trimmed.size(); ++pos) {
        const wchar_t c = trimmed[pos];
        if (c < L'0' || c > L'9') {
            return false;
        }
        value = value * 10 + (c - L'0');
        if (value > 0x7FFFFFFFLL) {
            value = 0x7FFFFFFFLL;  // saturate, then clamp
        }
    }
    if (negative) {
        value = -value;
    }
    out = static_cast<int>(std::clamp(
        value, static_cast<long long>(minValue),
        static_cast<long long>(maxValue)));
    return true;
}

// Hex (#RRGGBB / #AARRGGBB) or decimal "R, G, B[, A]" via the config parser.
bool ParseColorField(const std::wstring& text, uint32_t& argb) {
    return ParseColor(TrimWhitespace(text), argb);
}

int SliderValueFromX(int x, int trackLeft, int trackWidth, int minValue,
                     int maxValue, int step) {
    if (trackWidth <= 0 || maxValue <= minValue) {
        return minValue;
    }
    const double fraction =
        static_cast<double>(x - trackLeft) / static_cast<double>(trackWidth);
    const double raw = minValue + fraction * (maxValue - minValue);
    long long value = 0;
    if (step > 0) {
        const long long steps =
            std::lround((raw - minValue) / static_cast<double>(step));
        value = minValue + steps * step;
    } else {
        value = std::lround(raw);
    }
    return std::clamp(static_cast<int>(value), minValue, maxValue);
}

int SliderXFromValue(int value, int trackLeft, int trackWidth, int minValue,
                     int maxValue) {
    if (maxValue <= minValue) {
        return trackLeft;
    }
    const double fraction =
        static_cast<double>(value - minValue) /
        static_cast<double>(maxValue - minValue);
    return trackLeft + static_cast<int>(std::lround(fraction * trackWidth));
}

struct HsvColor {
    float h = 0.0f;  // [0, 360)
    float s = 0.0f;  // [0, 1]
    float v = 1.0f;  // [0, 1]
};

HsvColor RgbToHsv(uint32_t argb) {
    const float r = ((argb >> 16) & 0xFF) / 255.0f;
    const float g = ((argb >> 8) & 0xFF) / 255.0f;
    const float b = (argb & 0xFF) / 255.0f;
    const float maxC = std::max({r, g, b});
    const float minC = std::min({r, g, b});
    const float delta = maxC - minC;
    HsvColor hsv;
    hsv.v = maxC;
    hsv.s = maxC <= 0.0f ? 0.0f : delta / maxC;
    if (delta <= 0.0f) {
        hsv.h = 0.0f;
        return hsv;
    }
    if (maxC == r) {
        hsv.h = 60.0f * std::fmod((g - b) / delta, 6.0f);
    } else if (maxC == g) {
        hsv.h = 60.0f * (((b - r) / delta) + 2.0f);
    } else {
        hsv.h = 60.0f * (((r - g) / delta) + 4.0f);
    }
    if (hsv.h < 0.0f) {
        hsv.h += 360.0f;
    }
    return hsv;
}

uint32_t HsvToRgb(const HsvColor& hsv, uint8_t alpha) {
    const float h = std::fmod(std::fmod(hsv.h, 360.0f) + 360.0f, 360.0f);
    const float s = std::clamp(hsv.s, 0.0f, 1.0f);
    const float v = std::clamp(hsv.v, 0.0f, 1.0f);
    const float c = v * s;
    const float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    const float m = v - c;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    if (h < 60.0f) {
        r = c;
        g = x;
    } else if (h < 120.0f) {
        r = x;
        g = c;
    } else if (h < 180.0f) {
        g = c;
        b = x;
    } else if (h < 240.0f) {
        g = x;
        b = c;
    } else if (h < 300.0f) {
        r = x;
        b = c;
    } else {
        r = c;
        b = x;
    }
    auto channel = [&](float value) {
        return static_cast<uint32_t>(std::lround((value + m) * 255.0f));
    };
    return (static_cast<uint32_t>(alpha) << 24) | (channel(r) << 16) |
           (channel(g) << 8) | channel(b);
}

// BGRA pixels for the saturation/value square: x is saturation, y is value
// (top row is v=1). Top-down row order.
std::vector<uint8_t> BuildSvSquarePixels(float hue, int width, int height) {
    std::vector<uint8_t> pixels;
    if (width <= 0 || height <= 0) {
        return pixels;
    }
    pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
    const float wDen = width > 1 ? static_cast<float>(width - 1) : 1.0f;
    const float hDen = height > 1 ? static_cast<float>(height - 1) : 1.0f;
    for (int y = 0; y < height; ++y) {
        const float v = 1.0f - static_cast<float>(y) / hDen;
        for (int x = 0; x < width; ++x) {
            const float s = static_cast<float>(x) / wDen;
            const uint32_t argb = HsvToRgb(HsvColor{hue, s, v}, 255);
            const size_t offset =
                (static_cast<size_t>(y) * static_cast<size_t>(width) +
                 static_cast<size_t>(x)) *
                4;
            pixels[offset + 0] = static_cast<uint8_t>(argb & 0xFF);
            pixels[offset + 1] = static_cast<uint8_t>((argb >> 8) & 0xFF);
            pixels[offset + 2] = static_cast<uint8_t>((argb >> 16) & 0xFF);
            pixels[offset + 3] = static_cast<uint8_t>((argb >> 24) & 0xFF);
        }
    }
    return pixels;
}

struct HotkeySpec {
    UINT modifiers = 0;   // MOD_CONTROL / MOD_ALT / MOD_SHIFT / MOD_WIN
    UINT virtualKey = 0;  // VK code
};

// "Ctrl+Alt+M" / "Win+Shift+F12". At least one modifier and exactly one key
// are required, so a bare "F12" cannot hijack a global key.
bool ParseHotkey(const std::wstring& text, HotkeySpec& out) {
    HotkeySpec spec;
    bool haveKey = false;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t plus = text.find(L'+', pos);
        const std::wstring token = TrimWhitespace(
            plus == std::wstring::npos ? text.substr(pos)
                                       : text.substr(pos, plus - pos));
        pos = plus == std::wstring::npos ? text.size() + 1 : plus + 1;
        if (token.empty()) {
            return false;
        }
        const std::wstring lower = ToLowerCopy(token);
        if (lower == L"ctrl" || lower == L"control") {
            spec.modifiers |= MOD_CONTROL;
        } else if (lower == L"alt") {
            spec.modifiers |= MOD_ALT;
        } else if (lower == L"shift") {
            spec.modifiers |= MOD_SHIFT;
        } else if (lower == L"win") {
            spec.modifiers |= MOD_WIN;
        } else if (haveKey) {
            return false;
        } else if (token.size() == 1) {
            const wchar_t c = static_cast<wchar_t>(towupper(token[0]));
            if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
                spec.virtualKey = c;
                haveKey = true;
            } else {
                return false;
            }
        } else if (lower.size() >= 2 && lower[0] == L'f') {
            for (size_t i = 1; i < lower.size(); ++i) {
                if (lower[i] < L'0' || lower[i] > L'9') {
                    return false;
                }
            }
            const int number = _wtoi(lower.c_str() + 1);
            if (number < 1 || number > 24) {
                return false;
            }
            spec.virtualKey = VK_F1 + static_cast<UINT>(number - 1);
            haveKey = true;
        } else {
            return false;
        }
    }
    if (!haveKey || spec.modifiers == 0) {
        return false;
    }
    out = spec;
    return true;
}

// Multi-select effect rows: mask 0 is "none" and clears everything; a real
// effect toggles its bit, and clearing the last bit leaves "none".
uint32_t ToggleAnimationEffect(uint32_t effects, uint32_t effect) {
    if (effect == 0) {
        return 0;
    }
    return effects ^ effect;
}

// --- Settings model mapping ------------------------------------------------

uint32_t AppearanceColorValue(const Appearance& appearance,
                              const std::wstring& key) {
    const std::wstring k = ToLowerCopy(key);
    if (k == L"background") return appearance.background;
    if (k == L"border") return appearance.border;
    if (k == L"separator") return appearance.separator;
    if (k == L"hoverbackground") return appearance.hoverBackground;
    if (k == L"pressedbackground") return appearance.pressedBackground;
    if (k == L"textcolor") return appearance.textColor;
    if (k == L"disabledtextcolor") return appearance.disabledTextColor;
    if (k == L"submenuarrow") return appearance.submenuArrow;
    if (k == L"shadowcolor") return appearance.shadowColor;
    if (k == L"markercolor") return appearance.markerColor;
    if (k == L"headercolor") return appearance.headerColor;
    return 0;
}

bool AppearanceBoolValue(const Appearance& appearance,
                        const std::wstring& key) {
    const std::wstring k = ToLowerCopy(key);
    if (k == L"blur") return appearance.blur;
    if (k == L"shadow") return appearance.shadow;
    if (k == L"animatesubmenus") return appearance.animateSubmenus;
    if (k == L"shadowadaptive") return appearance.shadowAdaptive;
    if (k == L"overlayanimate") return appearance.overlayAnimate;
    return false;
}

int AppearanceIntValue(const Appearance& appearance, const std::wstring& key) {
    const std::wstring k = ToLowerCopy(key);
    if (k == L"blurstrength") return appearance.blurStrength;
    if (k == L"overlayintensity") return appearance.overlayIntensity;
    if (k == L"overlayspeed") return appearance.overlaySpeed;
    if (k == L"overlaysize") return appearance.overlaySize;
    if (k == L"overlayframems") return appearance.overlayFrameMs;
    if (k == L"cornerradius") return appearance.cornerRadius;
    if (k == L"borderwidth") return appearance.borderWidth;
    if (k == L"shadowsize") return appearance.shadowSize;
    if (k == L"shadowoffsetx") return appearance.shadowOffsetX;
    if (k == L"shadowoffsety") return appearance.shadowOffsetY;
    if (k == L"itemheight") return appearance.itemHeight;
    if (k == L"iconsize") return appearance.iconSize;
    if (k == L"padding") return appearance.padding;
    if (k == L"animationduration") return appearance.animationDuration;
    if (k == L"animationcloseduration") {
        return appearance.animationCloseDuration;
    }
    if (k == L"animationframems") return appearance.animationFrameMs;
    if (k == L"slideoffsetx") return appearance.slideOffsetX;
    if (k == L"slideoffsety") return appearance.slideOffsetY;
    if (k == L"scalefrom") return appearance.scaleFrom;
    if (k == L"verticalpadding") return appearance.verticalPadding;
    if (k == L"minwidth") return appearance.minWidth;
    if (k == L"maxwidth") return appearance.maxWidth;
    if (k == L"itempadding") return appearance.itemPadding;
    if (k == L"separatorspacing") return appearance.separatorSpacing;
    if (k == L"markerwidth") return appearance.markerWidth;
    if (k == L"shadowopacity") return appearance.shadowOpacity;
    if (k == L"shadowblur") return appearance.shadowBlur;
    return 0;
}

std::wstring FormatColorHex(uint32_t argb) {
    wchar_t buffer[16] = {};
    if ((argb >> 24) == 0xFF) {
        swprintf(buffer, 16, L"#%02X%02X%02X", (argb >> 16) & 0xFF,
                 (argb >> 8) & 0xFF, argb & 0xFF);
    } else {
        swprintf(buffer, 16, L"#%02X%02X%02X%02X", (argb >> 24) & 0xFF,
                 (argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF);
    }
    return buffer;
}

ControlSpec MakeControlSpec(const ConfigSchemaEntry& entry) {
    ControlSpec spec;
    spec.key = entry.key;
    spec.unsetCapable = entry.unset;
    if (ToLowerCopy(entry.key) == L"animation") {
        spec.kind = ControlKind::Info;
        return spec;
    }
    switch (entry.type) {
        case SettingType::Bool:
            spec.kind = ControlKind::Toggle;
            break;
        case SettingType::Int:
            spec.kind = ControlKind::IntSlider;
            spec.minValue = entry.minValue;
            spec.maxValue = entry.maxValue;
            spec.step = 1;
            spec.coarseStep =
                (entry.maxValue - entry.minValue) > 512 ? 10 : 1;
            break;
        case SettingType::Enum: {
            spec.kind = ControlKind::Enum;
            const std::wstring valid(entry.validValues);
            size_t start = 0;
            while (start <= valid.size()) {
                const size_t bar = valid.find(L'|', start);
                spec.options.push_back(valid.substr(
                    start, bar == std::wstring::npos ? std::wstring::npos
                                                     : bar - start));
                if (bar == std::wstring::npos) {
                    break;
                }
                start = bar + 1;
            }
            break;
        }
        case SettingType::Color:
            spec.kind = ControlKind::ColorSwatch;
            break;
        case SettingType::Font:
        case SettingType::IntList:
        case SettingType::EffectList:
            spec.kind = ControlKind::Info;
            break;
    }
    return spec;
}

struct SettingsModelInputs {
    Appearance working;
    SettingsTarget target;
    bool themeActive = false;
    int themeIndex = 0;
    std::wstring themeName;
    bool darkSystemTheme = false;
    bool hasLight = false;
    bool hasDark = false;
    uint32_t selectedEffects = 0;
    std::wstring statusText;
    std::vector<std::pair<std::wstring, std::wstring>> windhawkHints;
};

MenuItem MakeSettingsItem(ItemKind kind, std::wstring label) {
    MenuItem item;
    item.kind = kind;
    item.action = ActionKind::Builtin;
    item.builtinAction = BuiltinAction::None;
    item.label = std::move(label);
    return item;
}

struct SettingsEffectName {
    const wchar_t* name;
    uint32_t bit;
};

const SettingsEffectName kSettingsEffectNames[] = {
    {L"none", 0},           {L"fade", kAnimFade},
    {L"slide", kAnimSlide}, {L"scale", kAnimScale},
    {L"dissolve", kAnimDissolve}, {L"crt", kAnimCrt},
    {L"unfold", kAnimUnfold},
};

const SettingsEffectName kSettingsOverlayNames[] = {
    {L"none", 0},
    {L"noise", kOverlayNoise},
    {L"plasma", kOverlayPlasma},
    {L"hue", kOverlayHue},
    {L"glow", kOverlayGlow},
    {L"scanlines", kOverlayScanlines},
    {L"vignette", kOverlayVignette},
};

MenuItem MakeIntSliderRow(const wchar_t* label, std::wstring key, int minValue,
                          int maxValue, int value) {
    MenuItem item = MakeSettingsItem(ItemKind::Command, label);
    item.control.kind = ControlKind::IntSlider;
    item.control.key = std::move(key);
    item.control.minValue = minValue;
    item.control.maxValue = maxValue;
    item.controlValue = value;
    item.controlText = std::to_wstring(value);
    return item;
}

// Human-friendly presentation name for a schema key.
std::wstring SettingsDisplayLabel(const std::wstring& key) {
    struct LabelOverride {
        const wchar_t* key;
        const wchar_t* label;
    };
    static const LabelOverride kOverrides[] = {
        {L"animationopen", L"Open animation"},
        {L"animationclose", L"Close animation"},
        {L"animationduration", L"Open duration"},
        {L"animationcloseduration", L"Close duration"},
        {L"animationframems", L"Animation frame interval"},
        {L"animationeasing", L"Animation easing"},
        {L"animationanchor", L"Animation anchor"},
        {L"animatesubmenus", L"Animate submenus"},
        {L"showaccelerators", L"Accelerator display"},
        {L"blurstrength", L"Blur strength"},
        {L"overlay", L"Overlay effects"},
        {L"overlayintensity", L"Overlay intensity"},
        {L"overlayspeed", L"Overlay speed"},
        {L"overlaysize", L"Overlay size"},
        {L"overlayframems", L"Overlay frame interval"},
        {L"overlayanimate", L"Animate overlays while open"},
        {L"cornerradius", L"Corner radius"},
        {L"cornerradii", L"Corner radii"},
        {L"borderwidth", L"Border width"},
        {L"shadowsize", L"Shadow size"},
        {L"shadowoffsetx", L"Shadow offset X"},
        {L"shadowoffsety", L"Shadow offset Y"},
        {L"shadowopacity", L"Shadow opacity"},
        {L"shadowcolor", L"Shadow color"},
        {L"shadowadaptive", L"Adapt shadow to backdrop"},
        {L"shadowblur", L"Shadow blur"},
        {L"itemheight", L"Item height"},
        {L"iconsize", L"Icon size"},
        {L"hoverbackground", L"Hover background"},
        {L"pressedbackground", L"Pressed background"},
        {L"textcolor", L"Text color"},
        {L"disabledtextcolor", L"Disabled text color"},
        {L"submenuarrow", L"Submenu arrow"},
        {L"separatorspacing", L"Separator spacing"},
        {L"markerwidth", L"Marker width"},
        {L"markercolor", L"Marker color"},
        {L"headercolor", L"Header color"},
        {L"minwidth", L"Minimum width"},
        {L"maxwidth", L"Maximum width"},
        {L"fontweight", L"Font weight"},
        {L"fontstyle", L"Font style"},
        {L"itempadding", L"Item padding"},
        {L"verticalpadding", L"Vertical padding"},
        {L"slideoffsetx", L"Slide offset X"},
        {L"slideoffsety", L"Slide offset Y"},
        {L"scalefrom", L"Scale from"},
    };
    const std::wstring lower = ToLowerCopy(key);
    for (const LabelOverride& entry : kOverrides) {
        if (lower == entry.key) {
            return entry.label;
        }
    }
    std::wstring out;
    for (size_t i = 0; i < key.size(); ++i) {
        const wchar_t c = key[i];
        if (i > 0 && c >= L'A' && c <= L'Z') {
            out += L' ';
        }
        out += (i == 0) ? static_cast<wchar_t>(towupper(c)) : c;
    }
    return out;
}

MenuItem BuildSettingsRow(const Appearance& working,
                          const ConfigSchemaEntry& entry) {
    const std::wstring key = ToLowerCopy(entry.key);
    if (key == L"animation") {
        MenuItem row = MakeSettingsItem(ItemKind::Command, entry.key);
        row.control.kind = ControlKind::Info;
        row.control.key = entry.key;
        row.controlText = L"deprecated \u2014 use animationOpen/Close";
        return row;
    }
    const ControlSpec spec = MakeControlSpec(entry);
    if (spec.kind == ControlKind::ColorSwatch) {
        MenuItem row =
            MakeSettingsItem(ItemKind::Submenu, SettingsDisplayLabel(entry.key));
        row.action = ActionKind::Submenu;
        row.control = spec;
        row.controlColor = AppearanceColorValue(working, entry.key);
        std::wstring text;
        if (AppearanceValueText(working, entry, text)) {
            row.controlText = text;
        }
        const std::wstring parentKey = entry.key;
        auto colorChild = [&](const wchar_t* part, const wchar_t* label,
                              ControlKind kind, int height) {
            MenuItem child = MakeSettingsItem(ItemKind::Command, label);
            child.control.kind = kind;
            child.control.key = L"@color:" + parentKey + L":" + part;
            child.controlColor = row.controlColor;
            child.controlHeight = height;
            return child;
        };
        row.children.push_back(colorChild(L"area", L"Saturation / value",
                                          ControlKind::ColorArea, 120));
        row.children.push_back(
            colorChild(L"hue", L"Hue", ControlKind::HueStrip, 24));
        row.children.push_back(
            colorChild(L"alpha", L"Alpha", ControlKind::AlphaStrip, 24));
        MenuItem hex = MakeSettingsItem(ItemKind::Command, L"Hex");
        hex.control.kind = ControlKind::TextField;
        hex.control.key = L"@color:" + parentKey + L":hex";
        hex.controlText = FormatColorHex(row.controlColor);
        row.children.push_back(std::move(hex));
        const wchar_t* channelParts[] = {L"r", L"g", L"b", L"a"};
        const wchar_t* channelLabels[] = {L"Red", L"Green", L"Blue",
                                          L"Alpha value"};
        const int channels[] = {
            static_cast<int>((row.controlColor >> 16) & 0xFF),
            static_cast<int>((row.controlColor >> 8) & 0xFF),
            static_cast<int>(row.controlColor & 0xFF),
            static_cast<int>((row.controlColor >> 24) & 0xFF)};
        for (int i = 0; i < 4; ++i) {
            MenuItem channel = MakeIntSliderRow(
                channelLabels[i], L"@color:" + parentKey + L":" + channelParts[i],
                0, 255, channels[i]);
            row.children.push_back(std::move(channel));
        }
        if (spec.unsetCapable) {
            MenuItem unset =
                MakeSettingsItem(ItemKind::Command, L"Default (unset)");
            unset.control.kind = ControlKind::Toggle;
            unset.control.key = L"@color:" + parentKey + L":unset";
            const bool has = key == L"markercolor" ? working.hasMarkerColor
                                                   : working.hasHeaderColor;
            unset.controlValue = has ? 0 : 1;
            unset.controlText = has ? L"off" : L"on";
            row.children.push_back(std::move(unset));
        }
        return row;
    }
    if (spec.kind == ControlKind::Enum) {
        MenuItem row =
            MakeSettingsItem(ItemKind::Submenu, SettingsDisplayLabel(entry.key));
        row.action = ActionKind::Submenu;
        row.control = spec;
        std::wstring current;
        AppearanceValueText(working, entry, current);
        row.controlText = current;
        for (const std::wstring& option : spec.options) {
            MenuItem child = MakeSettingsItem(ItemKind::Command, option);
            child.control.kind = ControlKind::Action;
            child.control.key =
                L"@enum:" + std::wstring(entry.key) + L":" + option;
            if (_wcsicmp(option.c_str(), current.c_str()) == 0) {
                child.flags |= kModelChecked;
            }
            row.children.push_back(std::move(child));
        }
        return row;
    }
    if (entry.type == SettingType::EffectList) {
        MenuItem row =
            MakeSettingsItem(ItemKind::Submenu, SettingsDisplayLabel(entry.key));
        row.action = ActionKind::Submenu;
        row.control = spec;
        const bool overlay = key == L"overlay";
        const bool opening = key == L"animationopen";
        const uint32_t effects =
            overlay ? working.overlay
                    : (opening ? working.animationOpen : working.animationClose);
        row.controlText = overlay ? OverlayEffectsText(effects)
                                  : AnimationEffectsText(effects);
        const SettingsEffectName* names =
            overlay ? kSettingsOverlayNames : kSettingsEffectNames;
        const size_t nameCount = overlay ? ARRAYSIZE(kSettingsOverlayNames)
                                         : ARRAYSIZE(kSettingsEffectNames);
        for (size_t i = 0; i < nameCount; ++i) {
            const SettingsEffectName& effect = names[i];
            MenuItem child = MakeSettingsItem(ItemKind::Command, effect.name);
            child.control.kind = ControlKind::Action;
            child.control.key =
                std::wstring(overlay
                                 ? L"@effect:overlay:"
                                 : (opening ? L"@effect:open:"
                                            : L"@effect:close:")) +
                effect.name;
            const bool checked =
                effect.bit == 0 ? effects == 0 : (effects & effect.bit) != 0;
            if (checked) {
                child.flags |= kModelChecked;
            }
            row.children.push_back(std::move(child));
        }
        return row;
    }
    if (entry.type == SettingType::IntList) {
        MenuItem row =
            MakeSettingsItem(ItemKind::Submenu, SettingsDisplayLabel(entry.key));
        row.action = ActionKind::Submenu;
        row.control = spec;
        CornerRadii radii = working.cornerRadii;
        if (!working.hasCornerRadii) {
            ParseCornerRadii(entry.defaultValue, radii);
        }
        row.controlText = working.hasCornerRadii ? L"custom" : L"default";
        const wchar_t* ids[] = {L"tl", L"tr", L"br", L"bl"};
        const wchar_t* labels[] = {L"Top left", L"Top right", L"Bottom right",
                                   L"Bottom left"};
        const int values[] = {radii.topLeft, radii.topRight, radii.bottomRight,
                              radii.bottomLeft};
        for (int i = 0; i < 4; ++i) {
            MenuItem child = MakeIntSliderRow(
                labels[i], std::wstring(L"@cornerRadii:") + ids[i], 0, 256,
                values[i]);
            row.children.push_back(std::move(child));
        }
        MenuItem unset = MakeSettingsItem(
            ItemKind::Command, L"Use cornerRadius (unset)");
        unset.control.kind = ControlKind::Toggle;
        unset.control.key = L"@cornerRadii:unset";
        unset.controlValue = working.hasCornerRadii ? 0 : 1;
        unset.controlText = working.hasCornerRadii ? L"off" : L"on";
        row.children.push_back(std::move(unset));
        return row;
    }
    if (entry.type == SettingType::Font) {
        MenuItem row =
            MakeSettingsItem(ItemKind::Submenu, SettingsDisplayLabel(entry.key));
        row.action = ActionKind::Submenu;
        row.control = spec;
        row.controlText =
            working.fontFace + L", " + FormatFontSize(working.fontSize);
        MenuItem face = MakeSettingsItem(ItemKind::Command, L"Face");
        face.control.kind = ControlKind::TextInput;
        face.control.key = L"@font:face";
        face.controlText = working.fontFace;
        row.children.push_back(std::move(face));
        MenuItem size = MakeIntSliderRow(
            L"Size", L"@font:size", 6, 72,
            static_cast<int>(std::lround(working.fontSize)));
        row.children.push_back(std::move(size));
        return row;
    }
    MenuItem row =
        MakeSettingsItem(ItemKind::Command, SettingsDisplayLabel(entry.key));
    row.control = spec;
    if (spec.kind == ControlKind::Toggle) {
        const bool value = AppearanceBoolValue(working, entry.key);
        row.controlValue = value ? 1 : 0;
        row.controlText = value ? L"on" : L"off";
    } else if (spec.kind == ControlKind::IntSlider) {
        row.controlValue = AppearanceIntValue(working, entry.key);
        row.controlText = std::to_wstring(row.controlValue);
    }
    return row;
}

std::vector<MenuItem> BuildSettingsTree(const SettingsModelInputs& inputs) {
    const Appearance& working = inputs.working;
    std::vector<MenuItem> root;

    root.push_back(MakeSettingsItem(ItemKind::Header, L"Menu settings"));

    MenuItem status = MakeSettingsItem(ItemKind::Command, L"Status");
    status.control.kind = ControlKind::Info;
    status.control.key = L"@status";
    status.controlText =
        inputs.statusText.empty() ? L"Saved" : inputs.statusText;
    root.push_back(std::move(status));

    if (inputs.themeActive) {
        MenuItem theme = MakeSettingsItem(ItemKind::Command, L"Theme");
        theme.control.kind = ControlKind::Info;
        theme.control.key = L"@target";
        theme.controlText =
            inputs.themeName + L" \u2014 editing theme file";
        root.push_back(std::move(theme));
    } else {
        MenuItem target = MakeSettingsItem(ItemKind::Submenu, L"Target");
        target.action = ActionKind::Submenu;
        target.control.kind = ControlKind::Info;
        target.control.key = L"@target";
        struct TargetChoice {
            const wchar_t* id;
            const wchar_t* label;
            SettingsTargetKind kind;
        };
        const TargetChoice choices[] = {
            {L"@target:base", L"Base appearance",
             SettingsTargetKind::MenuIniBase},
            {L"@target:light", L"Light appearance",
             SettingsTargetKind::MenuIniLight},
            {L"@target:dark", L"Dark appearance",
             SettingsTargetKind::MenuIniDark},
        };
        for (const TargetChoice& choice : choices) {
            MenuItem child = MakeSettingsItem(ItemKind::Command, choice.label);
            child.control.kind = ControlKind::Action;
            child.control.key = choice.id;
            if (choice.kind == inputs.target.kind) {
                child.flags |= kModelChecked;
                target.controlText = choice.label;
            }
            target.children.push_back(std::move(child));
        }
        root.push_back(std::move(target));
    }

    // Groups in the schema's first-occurrence order.
    std::vector<std::wstring> groups;
    for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
        const std::wstring group = entry.group;
        bool found = false;
        for (const std::wstring& existing : groups) {
            if (existing == group) {
                found = true;
                break;
            }
        }
        if (!found) {
            groups.push_back(group);
        }
    }

    for (const std::wstring& group : groups) {
        MenuItem groupItem = MakeSettingsItem(ItemKind::Submenu, group);
        groupItem.action = ActionKind::Submenu;
        for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
            if (group != entry.group) {
                continue;
            }
            const std::wstring key = ToLowerCopy(entry.key);
            if (key == L"animation") {
                continue;  // deprecated alias, never shown
            }
            if (key == L"slideoffsetx" || key == L"slideoffsety" ||
                key == L"scalefrom") {
                continue;  // conditional pages below
            }
            groupItem.children.push_back(BuildSettingsRow(working, entry));
        }
        if (group == L"Animation") {
            if ((inputs.selectedEffects & kAnimSlide) != 0) {
                MenuItem page = MakeSettingsItem(ItemKind::Submenu,
                                                 L"Slide options\u2026");
                page.action = ActionKind::Submenu;
                page.control.kind = ControlKind::Info;
                page.control.key = L"@page:slide";
                page.children.push_back(
                    BuildSettingsRow(working, *SchemaFind(L"slideOffsetX")));
                page.children.push_back(
                    BuildSettingsRow(working, *SchemaFind(L"slideOffsetY")));
                groupItem.children.push_back(std::move(page));
            }
            if ((inputs.selectedEffects &
                 (kAnimScale | kAnimCrt | kAnimUnfold)) != 0) {
                MenuItem page = MakeSettingsItem(ItemKind::Submenu,
                                                 L"Scale options\u2026");
                page.action = ActionKind::Submenu;
                page.control.kind = ControlKind::Info;
                page.control.key = L"@page:scale";
                page.children.push_back(
                    BuildSettingsRow(working, *SchemaFind(L"scaleFrom")));
                groupItem.children.push_back(std::move(page));
            }
            MenuItem replay =
                MakeSettingsItem(ItemKind::Command, L"Replay open animation");
            replay.control.kind = ControlKind::Action;
            replay.control.key = L"@preview-open";
            groupItem.children.push_back(std::move(replay));
        }
        MenuItem reset =
            MakeSettingsItem(ItemKind::Command, L"Reset " + group + L"\u2026");
        reset.control.kind = ControlKind::Action;
        reset.control.key = L"@reset:" + group;
        groupItem.children.push_back(std::move(reset));
        root.push_back(std::move(groupItem));
    }

    MenuItem windhawk =
        MakeSettingsItem(ItemKind::Submenu, L"Windhawk settings\u2026");
    windhawk.action = ActionKind::Submenu;
    for (const std::pair<std::wstring, std::wstring>& hint :
         inputs.windhawkHints) {
        MenuItem row = MakeSettingsItem(ItemKind::Command, hint.first);
        row.control.kind = ControlKind::Info;
        row.control.key = L"@hint:" + hint.first;
        row.controlText = hint.second;
        windhawk.children.push_back(std::move(row));
    }
    MenuItem openWindhawk = MakeSettingsItem(ItemKind::Command, L"Open Windhawk");
    openWindhawk.control.kind = ControlKind::Action;
    openWindhawk.control.key = L"@open:windhawk";
    windhawk.children.push_back(std::move(openWindhawk));
    root.push_back(std::move(windhawk));

    MenuItem resetAll =
        MakeSettingsItem(ItemKind::Submenu, L"Reset all appearance\u2026");
    resetAll.action = ActionKind::Submenu;
    MenuItem resetAllAction =
        MakeSettingsItem(ItemKind::Command, L"Reset all appearance");
    resetAllAction.control.kind = ControlKind::Action;
    resetAllAction.control.key = L"@reset:all";
    resetAll.children.push_back(std::move(resetAllAction));
    root.push_back(std::move(resetAll));

    MenuItem openIni = MakeSettingsItem(ItemKind::Command, L"Open menu.ini");
    openIni.control.kind = ControlKind::Action;
    openIni.control.key = L"@open:ini";
    root.push_back(std::move(openIni));

    if (inputs.themeActive) {
        MenuItem openTheme =
            MakeSettingsItem(ItemKind::Command, L"Open theme file");
        openTheme.control.kind = ControlKind::Action;
        openTheme.control.key = L"@open:theme";
        root.push_back(std::move(openTheme));
    }

    MenuItem about = MakeSettingsItem(ItemKind::Command, L"About");
    about.control.kind = ControlKind::Info;
    about.control.key = L"@about";
    about.controlText = L"schema " + std::to_wstring(kConfigSchemaVersion);
    root.push_back(std::move(about));

    return root;
}

// Removes any existing built-in entry (and its separator) from the tree, so
// repeated assembly never duplicates or leaves a stale copy behind.
bool StripBuiltinAction(std::vector<MenuItem>& items, BuiltinAction action) {
    bool removed = false;
    for (size_t i = 0; i < items.size();) {
        if (items[i].action == ActionKind::Builtin &&
            items[i].builtinAction == action) {
            if (i > 0 && items[i - 1].kind == ItemKind::Separator) {
                items.erase(items.begin() + (i - 1), items.begin() + (i + 1));
            } else {
                items.erase(items.begin() + i);
            }
            removed = true;
            continue;
        }
        if (StripBuiltinAction(items[i].children, action)) {
            removed = true;
        }
        ++i;
    }
    return removed;
}

// Built-in folder action: opens the selection in an isolated Explorer process
// (explorer.exe /separate). Appended with the other built-ins.
void AppendOpenNewProcessEntry(std::vector<MenuItem>& items) {
    StripBuiltinAction(items, BuiltinAction::OpenNewProcess);
    MenuItem separator = MakeSettingsItem(ItemKind::Separator, L"");
    separator.action = ActionKind::ViewAction;
    items.push_back(std::move(separator));
    MenuItem row = MakeSettingsItem(ItemKind::Command, L"Open in new process");
    row.id = 0xF301;
    row.action = ActionKind::Builtin;
    row.builtinAction = BuiltinAction::OpenNewProcess;
    row.iconRef = L"@icon:open";
    items.push_back(std::move(row));
}

// Appends the "Menu settings..." entry (and a separator) to a list. Called
// after rules, pruning, and advanced-submenu reorganization, so nothing can
// hide it and the advanced submenu exists when it should.
void AppendSettingsEntry(std::vector<MenuItem>& items) {
    StripBuiltinAction(items, BuiltinAction::OpenSettings);
    MenuItem separator = MakeSettingsItem(ItemKind::Separator, L"");
    separator.action = ActionKind::ViewAction;
    items.push_back(std::move(separator));
    MenuItem row = MakeSettingsItem(ItemKind::Command, L"Menu settings\u2026");
    row.id = 0xF300;  // unique: FindById must resolve this row, not id 0
    row.action = ActionKind::Builtin;
    row.builtinAction = BuiltinAction::OpenSettings;
    row.iconRef = L"@icon:settings";
    items.push_back(std::move(row));
}

// --- Settings persistence helpers ------------------------------------------

struct SettingsWriteState {
    bool pending = false;
    bool failed = false;
    uint64_t deadlineMs = 0;
};

void SettingsMarkDirty(SettingsWriteState& state, uint64_t nowMs,
                       int debounceMs) {
    state.pending = true;
    state.failed = false;
    state.deadlineMs = nowMs + static_cast<uint64_t>(debounceMs);
}

bool SettingsWriteDue(const SettingsWriteState& state, uint64_t nowMs) {
    return state.pending && nowMs >= state.deadlineMs;
}

void SettingsWriteFinished(SettingsWriteState& state, bool ok, uint64_t nowMs,
                           int retryMs) {
    if (ok) {
        state.pending = false;
        state.failed = false;
        return;
    }
    state.pending = true;
    state.failed = true;
    state.deadlineMs = nowMs + static_cast<uint64_t>(retryMs);
}


// One override per dirty key in the target section; unset values remove the
// key so the target's inheritance (theme preset, light/dark base, derived
// default) applies again.
std::vector<ConfigOverride> BuildChangeOverrides(
    const Appearance& working, const std::vector<std::wstring>& dirtyKeys,
    const SettingsTarget& target) {
    std::vector<ConfigOverride> changes;
    changes.reserve(dirtyKeys.size());
    for (const std::wstring& key : dirtyKeys) {
        const ConfigSchemaEntry* entry = SchemaFind(key);
        if (!entry) {
            continue;
        }
        ConfigOverride change;
        change.section = target.section;
        change.key = entry->key;
        std::wstring value;
        if (AppearanceValueText(working, *entry, value)) {
            change.value = value;
        } else {
            change.remove = true;
        }
        changes.push_back(std::move(change));
    }
    return changes;
}

std::wstring BuildMenuIniTextWithChanges(
    const std::wstring& fileText,
    const std::vector<ConfigOverride>& changes) {
    return CanonicalizeConfigWithOverrides(fileText, kConfigSchemaVersion,
                                           changes);
}

// Theme files are complete [appearance] blocks; the preset snippet supplies
// the fallback values for keys the file does not set (matching ThemeStore's
// load merge), and only the [appearance] section is written back.
std::wstring BuildThemeTextWithChanges(
    const std::wstring& presetSnippet, const std::wstring& fileText,
    const std::vector<ConfigOverride>& changes) {
    const std::wstring combined = presetSnippet + L"\n" + fileText;
    const std::wstring canonical = CanonicalizeConfigWithOverrides(
        combined, kConfigSchemaVersion, changes);
    return ExtractAppearanceBlock(canonical);
}


// ===========================================================================
// [CMO:Layout] Appearance resolution, metrics, and render-ready layout.
// ===========================================================================

Appearance ResolveAppearance(const RulesConfig& config, bool darkTheme) {
    if (darkTheme) {
        return config.hasDarkAppearance ? config.darkAppearance : config.appearance;
    }
    return config.hasLightAppearance ? config.lightAppearance : config.appearance;
}

// The active theme wins over menu.ini's appearance while it is selected.
Appearance EffectiveAppearance(const RulesConfig& config, bool darkTheme) {
    const std::shared_ptr<const Appearance> theme = g_themeStore.Snapshot();
    return theme ? *theme : ResolveAppearance(config, darkTheme);
}

int BlurPasses(int amount) {
    return std::clamp(amount / 4, 0, 16);
}

// Capture the backdrop when the blur needs it, or when the adaptive shadow
// needs the background's average color.
bool WantsBackdropCapture(const Appearance& appearance) {
    const bool shadowDrawn =
        appearance.shadow &&
        (appearance.shadowSize > 0 || appearance.shadowBlur > 0);
    return (appearance.blur && appearance.blurStrength > 0) ||
           (shadowDrawn && appearance.shadowAdaptive);
}

int BackdropBlurPasses(const Appearance& appearance) {
    return appearance.blur ? BlurPasses(appearance.blurStrength) : 0;
}

int ShadowMargin(int spread, int blur, int offsetX, int offsetY) {
    const int offset =
        std::max(std::max(offsetX, -offsetX), std::max(offsetY, -offsetY));
    return std::min(spread + blur + offset, 160);
}

struct LayoutMetrics {
    int itemHeight = 28;
    int separatorHeight = 7;
    int iconSize = 16;
    int padding = 6;
    int gutterWidth = 22;
    int submenuArrowWidth = 16;
    int cornerRadius = 8;
    int borderWidth = 1;
    int shadowSize = 0;
    float fontSize = 9.0f;
    std::wstring fontFace = L"Segoe UI";
    uint32_t textColor = 0xFFFFFFFF;
    uint32_t disabledTextColor = 0x66FFFFFF;
    uint32_t hoverBackground = 0x14FFFFFF;
    uint32_t pressedBackground = 0x22FFFFFF;
    uint32_t separator = 0x18FFFFFF;
    uint32_t submenuArrow = 0x99FFFFFF;
    int shadowOffsetX = 0;
    int shadowOffsetY = 4;
    int blurPasses = 2;
    int verticalPadding = 4;
    int itemPadding = 6;
    int separatorSpacing = 0;
    int minWidth = 0;
    int maxWidth = 0;
    int markerWidth = 14;
    FontWeightKind fontWeight = FontWeightKind::Normal;
    FontStyleKind fontStyle = FontStyleKind::Normal;
    CornerRadii cornerRadii;
    bool hasCornerRadii = false;
    int shadowOpacity = 120;
    uint32_t shadowColor = 0xFF000000;
    int shadowBlur = 18;
    MarkerStyle marker = MarkerStyle::Dot;
    uint32_t markerColor = 0xFFFFFFFF;
    uint32_t headerColor = 0x66FFFFFF;
    AcceleratorMode acceleratorMode = AcceleratorMode::Underline;
};

DWRITE_FONT_WEIGHT FontWeightToDwrite(FontWeightKind weight) {
    switch (weight) {
        case FontWeightKind::Semibold:
            return DWRITE_FONT_WEIGHT_SEMI_BOLD;
        case FontWeightKind::Bold:
            return DWRITE_FONT_WEIGHT_BOLD;
        default:
            return DWRITE_FONT_WEIGHT_NORMAL;
    }
}

DWRITE_FONT_STYLE FontStyleToDwrite(FontStyleKind style) {
    return style == FontStyleKind::Italic ? DWRITE_FONT_STYLE_ITALIC
                                          : DWRITE_FONT_STYLE_NORMAL;
}

LayoutMetrics ResolveLayoutMetrics(const Appearance& appearance, uint32_t dpi,
                                   bool darkTheme) {
    (void)darkTheme;
    const int scale = dpi == 0 ? 96 : static_cast<int>(dpi);

    LayoutMetrics metrics;
    metrics.itemHeight = MulDiv(appearance.itemHeight, scale, 96);
    metrics.iconSize = MulDiv(appearance.iconSize, scale, 96);
    metrics.padding = MulDiv(appearance.padding, scale, 96);
    metrics.gutterWidth =
        metrics.markerWidth + metrics.padding / 2 + metrics.iconSize;
    metrics.separatorHeight = MulDiv(7, scale, 96);
    metrics.submenuArrowWidth = MulDiv(16, scale, 96);
    metrics.cornerRadius = MulDiv(appearance.cornerRadius, scale, 96);
    metrics.borderWidth = std::max(1, MulDiv(appearance.borderWidth, scale, 96));
    metrics.shadowSize = MulDiv(appearance.shadowSize, scale, 96);
    metrics.fontFace = appearance.fontFace;
    metrics.fontSize = appearance.fontSize * (static_cast<float>(scale) / 96.0f);
    metrics.textColor = appearance.textColor;
    metrics.disabledTextColor = appearance.disabledTextColor;
    metrics.hoverBackground = appearance.hoverBackground;
    metrics.pressedBackground = appearance.pressedBackground;
    metrics.separator = appearance.separator;
    metrics.submenuArrow = appearance.submenuArrow;
    metrics.verticalPadding = MulDiv(appearance.verticalPadding, scale, 96);
    metrics.itemPadding = appearance.itemPadding >= 0
                              ? MulDiv(appearance.itemPadding, scale, 96)
                              : metrics.padding;
    metrics.separatorSpacing = MulDiv(appearance.separatorSpacing, scale, 96);
    metrics.minWidth = MulDiv(appearance.minWidth, scale, 96);
    metrics.maxWidth = MulDiv(appearance.maxWidth, scale, 96);
    metrics.markerWidth = MulDiv(appearance.markerWidth, scale, 96);
    metrics.fontWeight = appearance.fontWeight;
    metrics.fontStyle = appearance.fontStyle;
    metrics.cornerRadii = {
        MulDiv(appearance.cornerRadii.topLeft, scale, 96),
        MulDiv(appearance.cornerRadii.topRight, scale, 96),
        MulDiv(appearance.cornerRadii.bottomRight, scale, 96),
        MulDiv(appearance.cornerRadii.bottomLeft, scale, 96)};
    metrics.hasCornerRadii = appearance.hasCornerRadii;
    metrics.shadowOpacity = appearance.shadowOpacity;
    metrics.shadowColor = appearance.shadowColor;
    metrics.shadowBlur = MulDiv(appearance.shadowBlur, scale, 96);
    metrics.shadowOffsetX = MulDiv(appearance.shadowOffsetX, scale, 96);
    metrics.shadowOffsetY = MulDiv(appearance.shadowOffsetY, scale, 96);
    metrics.blurPasses = BlurPasses(MulDiv(appearance.blurStrength, scale, 96));
    metrics.marker = appearance.marker;
    metrics.markerColor = appearance.hasMarkerColor ? appearance.markerColor
                                                    : appearance.textColor;
    metrics.headerColor = appearance.hasHeaderColor ? appearance.headerColor
                                                    : appearance.disabledTextColor;
    metrics.acceleratorMode = appearance.acceleratorMode;
    return metrics;
}

struct InvocationDescriptor {
    uint32_t id = 0;
    ActionKind action = ActionKind::ViewAction;
    uint32_t viewAction = 0;
    uint32_t verbOffset = 0;
    uint32_t sortIndex = 0;
    uint32_t customCommandIndex = 0;
    uint32_t newIndex = 0;
    std::wstring canonicalVerb;
    std::wstring targetPath;
    bool hasOffset = false;
};

struct LayoutItemResources {
    IDWriteTextLayout* text = nullptr;
    IDWriteTextLayout* glyph = nullptr;
    ID2D1Bitmap* icon = nullptr;
    // Cached saturation/value square for the color editor, keyed by hue.
    ID2D1Bitmap* svSquare = nullptr;
    float svHue = -1.0f;

    ~LayoutItemResources() {
        if (text) {
            text->Release();
        }
        if (glyph) {
            glyph->Release();
        }
        if (icon) {
            icon->Release();
        }
        if (svSquare) {
            svSquare->Release();
        }
    }
};

struct LayoutItem {
    RECT rect = {};
    RECT gutterRect = {};
    RECT markerRect = {};
    RECT iconRect = {};
    RECT textRect = {};
    int markerStyle = -1;
    ItemKind kind = ItemKind::Command;
    std::wstring label;
    std::wstring iconRef;
    std::vector<uint8_t> iconPixels;
    uint32_t flags = 0;
    uint32_t textColor = 0;
    uint32_t hoverTextColor = 0;
    int submenuIndex = -1;
    InvocationDescriptor invocation;
    std::shared_ptr<LayoutItemResources> resources;
    // Settings-UI control (ControlKind::None for ordinary items).
    ControlSpec control;
    std::wstring controlText;
    int controlValue = 0;
    uint32_t controlColor = 0;
    RECT controlRect = {};
    RECT fieldRect = {};
    RECT trackRect = {};
    RECT thumbRect = {};
    RECT swatchRect = {};
    RECT areaRect = {};
    RECT stripRect = {};
};

struct LayoutPanel {
    SIZE size = {};
    std::vector<LayoutItem> items;
    std::vector<LayoutPanel> children;
};

struct AcceleratorText {
    std::wstring text;
    std::vector<std::pair<size_t, size_t>> underlineRanges;
};

// Parses shell mnemonics: && -> literal &, a single & underlines the next
// character and is removed, a trailing & is literal.
AcceleratorText StripAccelerators(const std::wstring& label) {
    AcceleratorText result;
    result.text.reserve(label.size());
    for (size_t i = 0; i < label.size(); ++i) {
        const wchar_t c = label[i];
        if (c != L'&') {
            result.text.push_back(c);
            continue;
        }
        if (i + 1 < label.size() && label[i + 1] == L'&') {
            result.text.push_back(L'&');
            ++i;
            continue;
        }
        if (i + 1 < label.size()) {
            result.underlineRanges.push_back(
                {result.text.size(), result.text.size() + 1});
            result.text.push_back(label[i + 1]);
            ++i;
            continue;
        }
        result.text.push_back(L'&');
    }
    return result;
}

// Renderer supplies a DirectWrite-based measurer; the default estimate keeps
// the builder pure and testable.
using TextMeasureFn = int (*)(const wchar_t*, size_t, const LayoutMetrics&);

int EstimateTextWidth(const wchar_t* label, size_t length,
                      const LayoutMetrics& metrics) {
    const std::wstring raw(label, length);
    const size_t displayLength = StripAccelerators(raw).text.size();
    return static_cast<int>(static_cast<float>(displayLength) * metrics.fontSize *
                            0.6f) +
           4;
}

LayoutPanel BuildLayoutPanel(const std::vector<MenuItem>& items,
                             const LayoutMetrics& metrics,
                             TextMeasureFn measure = nullptr) {
    LayoutPanel panel;
    const int markerLeft = metrics.itemPadding;
    const int iconLeft =
        metrics.padding + metrics.markerWidth + metrics.padding / 2;
    const int textLeft = iconLeft + metrics.iconSize + metrics.padding;
    const int textGap = metrics.padding;

    auto measureText = [&](const std::wstring& text) {
        return text.empty()
                   ? 0
                   : (measure ? measure(text.c_str(), text.size(), metrics)
                              : EstimateTextWidth(text.c_str(), text.size(),
                                                  metrics));
    };
    auto controlReserve = [&](const MenuItem& item) {
        switch (item.control.kind) {
            case ControlKind::Toggle:
                return 44;
            case ControlKind::IntSlider:
                return 200;
            case ControlKind::Enum:
                return measureText(item.controlText) + 18;
            case ControlKind::ColorSwatch:
                return 48;
            case ControlKind::TextField:
            case ControlKind::TextInput:
                return 140;
            case ControlKind::Info:
                return measureText(item.controlText);
            default:
                return 0;
        }
    };
    auto rowHeight = [&](const MenuItem& item) {
        if (item.kind == ItemKind::Separator) {
            return metrics.separatorHeight + 2 * metrics.separatorSpacing;
        }
        if (item.control.kind == ControlKind::ColorArea) {
            return item.controlHeight > 0 ? item.controlHeight : 120;
        }
        if (item.control.kind == ControlKind::HueStrip ||
            item.control.kind == ControlKind::AlphaStrip) {
            return item.controlHeight > 0 ? item.controlHeight : 24;
        }
        if (item.controlHeight > 0) {
            return item.controlHeight;
        }
        return metrics.itemHeight;
    };

    int y = metrics.verticalPadding;
    int contentWidth = 0;
    for (const MenuItem& item : items) {
        const int height = rowHeight(item);
        if (item.kind != ItemKind::Separator) {
            const std::wstring& measureLabel =
                item.displayLabel.empty() ? item.label : item.displayLabel;
            const int textWidth = measureText(measureLabel);
            const int arrowSpace = item.kind == ItemKind::Submenu
                                       ? metrics.submenuArrowWidth
                                       : 0;
            const int reserve = controlReserve(item);
            const int labelGap = reserve > 0 ? 12 : 0;
            contentWidth =
                std::max(contentWidth, textLeft + textWidth + arrowSpace +
                                           labelGap + reserve + textGap);
        }
        y += height;
    }
    int panelWidth = std::max(contentWidth, 80);
    if (metrics.minWidth > 0) {
        panelWidth = std::max(panelWidth, metrics.minWidth);
    }
    if (metrics.maxWidth > 0) {
        // maxWidth is a soft cap: never narrower than the widest item.
        panelWidth =
            std::min(panelWidth, std::max(metrics.maxWidth, contentWidth));
    }
    panel.size = {panelWidth, y + metrics.verticalPadding};

    int offset = metrics.verticalPadding;
    for (const MenuItem& item : items) {
        LayoutItem layout{};
        layout.kind = item.kind;
        layout.label = item.displayLabel.empty() ? item.label : item.displayLabel;
        layout.iconRef = item.iconRef;
        layout.iconPixels = item.iconPixels;
        layout.flags = item.flags;
        layout.textColor = (item.flags & kModelDisabled) ? metrics.disabledTextColor
                                                         : metrics.textColor;
        layout.hoverTextColor = metrics.textColor;
        layout.invocation.id = item.id;
        layout.invocation.action = item.action;
        layout.invocation.viewAction = item.viewAction;
        layout.invocation.verbOffset = item.verbOffset;
        layout.invocation.sortIndex = item.sortIndex;
        layout.invocation.customCommandIndex = item.customCommandIndex;
        layout.invocation.newIndex = item.newIndex;
        layout.invocation.canonicalVerb = item.canonicalVerb;
        layout.invocation.targetPath = item.targetPath;
        layout.invocation.hasOffset = (item.flags & kModelHasOffset) != 0;

        const int height = rowHeight(item);
        layout.rect = {0, offset, panel.size.cx, offset + height};

        if (item.kind != ItemKind::Separator) {
            layout.markerRect = {markerLeft, offset,
                                 markerLeft + metrics.markerWidth, offset + height};
            layout.markerStyle =
                item.markerOverride >= 0
                    ? item.markerOverride
                    : static_cast<int>(metrics.marker);
            layout.gutterRect = {metrics.padding, offset, textLeft - metrics.padding,
                                 offset + height};
            layout.iconRect = {iconLeft, offset + (height - metrics.iconSize) / 2,
                               iconLeft + metrics.iconSize,
                               offset + (height + metrics.iconSize) / 2};
            const int arrowSpace =
                item.kind == ItemKind::Submenu ? metrics.submenuArrowWidth : 0;
            layout.textRect = {textLeft, offset,
                               panel.size.cx - textGap - arrowSpace, offset + height};
        }

        if (item.control.kind != ControlKind::None) {
            layout.control = item.control;
            layout.controlText = item.controlText;
            layout.controlValue = item.controlValue;
            layout.controlColor = item.controlColor;
            const int reserve = controlReserve(item);
            // A submenu chevron owns the right edge; keep the control (value
            // text, swatch, field) left of it so they never overlap.
            const int arrowSpace =
                item.kind == ItemKind::Submenu ? metrics.submenuArrowWidth : 0;
            const int controlRight = panel.size.cx - textGap - arrowSpace;
            const int controlLeft = controlRight - reserve;
            layout.controlRect = {controlLeft, offset, controlRight,
                                  offset + height};
            switch (item.control.kind) {
                case ControlKind::IntSlider: {
                    const int fieldWidth = 56;
                    layout.fieldRect = {layout.controlRect.right - fieldWidth,
                                        offset + (height - 20) / 2,
                                        layout.controlRect.right,
                                        offset + (height + 20) / 2};
                    layout.trackRect = {layout.controlRect.left,
                                        offset + height / 2 - 2,
                                        layout.fieldRect.left - 6,
                                        offset + height / 2 + 2};
                    break;
                }
                case ControlKind::ColorSwatch:
                    layout.swatchRect = {controlLeft,
                                         offset + (height - 18) / 2,
                                         controlLeft + 44,
                                         offset + (height + 18) / 2};
                    break;
                case ControlKind::TextField:
                case ControlKind::TextInput:
                    layout.fieldRect = {controlLeft, offset + (height - 20) / 2,
                                        layout.controlRect.right,
                                        offset + (height + 20) / 2};
                    break;
                case ControlKind::ColorArea:
                case ControlKind::HueStrip:
                case ControlKind::AlphaStrip: {
                    // Leave the label its own space so the bar does not cover
                    // or clip it, with a minimum bar width.
                    const int labelWidth = measureText(layout.label);
                    int stripLeft = textLeft + labelWidth + 12;
                    const int minBarWidth = 120;
                    stripLeft = std::min(
                        stripLeft, static_cast<int>(panel.size.cx) - textGap -
                                       minBarWidth);
                    stripLeft = std::max(stripLeft, textLeft);
                    layout.areaRect = {stripLeft, offset,
                                       panel.size.cx - textGap, offset + height};
                    layout.stripRect = layout.areaRect;
                    break;
                }
                default:
                    break;
            }
        }

        if (item.kind == ItemKind::Submenu) {
            layout.submenuIndex = static_cast<int>(panel.children.size());
            panel.children.push_back(
                BuildLayoutPanel(item.children, metrics, measure));
        }
        panel.items.push_back(std::move(layout));
        offset += height;
    }
    return panel;
}

// --- Control hit-testing and field editing ---------------------------------

enum class ControlPart : uint8_t {
    None,
    Row,
    Track,
    Thumb,
    Field,
    Swatch,
    Area,
    HueStrip,
    AlphaStrip,
    Toggle,
};

bool PointInRect(const RECT& rect, POINT pt) {
    return pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top &&
           pt.y < rect.bottom;
}

ControlPart HitTestControlPart(const LayoutItem& item, POINT pt) {
    if (!PointInRect(item.rect, pt)) {
        return ControlPart::None;
    }
    switch (item.control.kind) {
        case ControlKind::IntSlider:
            if (PointInRect(item.thumbRect, pt)) {
                return ControlPart::Thumb;
            }
            if (PointInRect(item.fieldRect, pt)) {
                return ControlPart::Field;
            }
            if (PointInRect(item.trackRect, pt)) {
                return ControlPart::Track;
            }
            return ControlPart::Row;
        case ControlKind::ColorSwatch:
            if (PointInRect(item.swatchRect, pt)) {
                return ControlPart::Swatch;
            }
            return ControlPart::Row;
        case ControlKind::TextField:
        case ControlKind::TextInput:
            if (PointInRect(item.fieldRect, pt)) {
                return ControlPart::Field;
            }
            return ControlPart::Row;
        case ControlKind::ColorArea:
            if (PointInRect(item.areaRect, pt)) {
                return ControlPart::Area;
            }
            return ControlPart::Row;
        case ControlKind::HueStrip:
            if (PointInRect(item.stripRect, pt)) {
                return ControlPart::HueStrip;
            }
            return ControlPart::Row;
        case ControlKind::AlphaStrip:
            if (PointInRect(item.stripRect, pt)) {
                return ControlPart::AlphaStrip;
            }
            return ControlPart::Row;
        case ControlKind::Toggle:
            if (PointInRect(item.controlRect, pt)) {
                return ControlPart::Toggle;
            }
            return ControlPart::Row;
        default:
            return ControlPart::Row;
    }
}

enum class FieldInputMode : uint8_t { IntSigned, IntUnsigned, Hex, FreeText };

// Field editing: control keys move the caret/delete; a typed character is
// passed with key == 0 and ch set. Returns true when the buffer changed.
bool ApplyFieldKey(std::wstring& buffer, size_t& caret, UINT key, wchar_t ch,
                   FieldInputMode mode) {
    if (caret > buffer.size()) {
        caret = buffer.size();
    }
    switch (key) {
        case VK_BACK:
            if (caret == 0) {
                return false;
            }
            buffer.erase(caret - 1, 1);
            --caret;
            return true;
        case VK_DELETE:
            if (caret >= buffer.size()) {
                return false;
            }
            buffer.erase(caret, 1);
            return true;
        case VK_LEFT:
            if (caret == 0) {
                return false;
            }
            --caret;
            return true;
        case VK_RIGHT:
            if (caret >= buffer.size()) {
                return false;
            }
            ++caret;
            return true;
        case VK_HOME:
            if (caret == 0) {
                return false;
            }
            caret = 0;
            return true;
        case VK_END:
            if (caret == buffer.size()) {
                return false;
            }
            caret = buffer.size();
            return true;
        default:
            break;
    }
    if (key != 0 || ch == 0) {
        return false;
    }
    if (mode == FieldInputMode::FreeText) {
        if (ch < 0x20 || ch == 0x7F) {
            return false;  // control characters
        }
        buffer.insert(caret, 1, ch);
        ++caret;
        return true;
    }
    if (ch == L'-') {
        if (mode != FieldInputMode::IntSigned ||
            buffer.find(L'-') != std::wstring::npos) {
            return false;
        }
        buffer.insert(0, 1, L'-');
        ++caret;
        return true;
    }
    if (ch == L'#') {
        if (mode != FieldInputMode::Hex ||
            buffer.find(L'#') != std::wstring::npos) {
            return false;
        }
        buffer.insert(0, 1, L'#');
        ++caret;
        return true;
    }
    const bool digit = ch >= L'0' && ch <= L'9';
    const bool hexDigit =
        digit || (ch >= L'a' && ch <= L'f') || (ch >= L'A' && ch <= L'F');
    if (mode == FieldInputMode::Hex ? !hexDigit : !digit) {
        return false;
    }
    buffer.insert(caret, 1, ch);
    ++caret;
    return true;
}

bool CommitFieldBuffer(const ControlSpec& control, const std::wstring& buffer,
                       std::wstring& canonicalOut) {
    if (control.kind == ControlKind::IntSlider) {
        int value = 0;
        if (!ParseIntField(buffer, control.minValue, control.maxValue, value)) {
            return false;
        }
        canonicalOut = std::to_wstring(value);
        return true;
    }
    if (control.kind == ControlKind::TextField) {
        uint32_t argb = 0;
        if (!ParseColorField(buffer, argb)) {
            return false;
        }
        canonicalOut = FormatColorRgba(argb);
        return true;
    }
    if (control.kind == ControlKind::TextInput) {
        canonicalOut = TrimWhitespace(buffer);
        return !canonicalOut.empty();
    }
    return false;
}

POINT ClampPanelPosition(POINT anchor, SIZE panelSize, const RECT& workArea) {
    int x = anchor.x;
    int y = anchor.y;
    if (x + panelSize.cx > workArea.right) {
        x = anchor.x - panelSize.cx;
    }
    if (y + panelSize.cy > workArea.bottom) {
        y = anchor.y - panelSize.cy;
    }
    const int left = static_cast<int>(workArea.left);
    const int top = static_cast<int>(workArea.top);
    x = std::clamp(x, left, std::max(left, static_cast<int>(workArea.right) - static_cast<int>(panelSize.cx)));
    y = std::clamp(y, top, std::max(top, static_cast<int>(workArea.bottom) - static_cast<int>(panelSize.cy)));
    return POINT{x, y};
}

POINT SubmenuPosition(const RECT& parentItemScreenRect, SIZE childSize,
                      const RECT& workArea, int overlapPx) {
    int x = parentItemScreenRect.right - overlapPx;
    if (x + childSize.cx > workArea.right) {
        x = parentItemScreenRect.left - childSize.cx;
    }
    const int left = static_cast<int>(workArea.left);
    const int top = static_cast<int>(workArea.top);
    x = std::clamp(x, left, std::max(left, static_cast<int>(workArea.right) - static_cast<int>(childSize.cx)));

    int y = parentItemScreenRect.top;
    if (y + childSize.cy > workArea.bottom) {
        y = workArea.bottom - childSize.cy;
    }
    y = std::clamp(y, top, std::max(top, static_cast<int>(workArea.bottom) - static_cast<int>(childSize.cy)));
    return POINT{x, y};
}

uint64_t ModelFingerprint(const std::vector<MenuItem>& items) {
    uint64_t hash = 1469598103934665603ull;
    for (const MenuItem& item : items) {
        hash = HashCombine(hash, item.id);
        hash = HashCombine(hash, static_cast<uint64_t>(item.kind));
        hash = HashCombine(hash, static_cast<uint64_t>(item.action));
        hash = HashCombine(hash, item.flags);
        hash = HashCombine(hash, static_cast<uint64_t>(item.children.size()));
        for (wchar_t c : item.label) {
            hash = HashCombine(hash, static_cast<uint64_t>(c));
        }
        hash = HashCombine(hash, ModelFingerprint(item.children));
    }
    return hash;
}

struct LayoutKey {
    ContextSignature sig;
    uint64_t rulesRevision = 0;
    uint64_t appearanceRevision = 0;
    uint64_t themeRevision = 0;
    uint32_t dpi = 96;
    bool darkTheme = false;
    uint64_t modelFingerprint = 0;

    bool operator==(const LayoutKey&) const = default;

    uint64_t Hash() const {
        uint64_t hash = sig.Hash();
        hash = HashCombine(hash, rulesRevision);
        hash = HashCombine(hash, appearanceRevision);
        hash = HashCombine(hash, themeRevision);
        hash = HashCombine(hash, dpi);
        hash = HashCombine(hash, darkTheme ? 1 : 0);
        hash = HashCombine(hash, modelFingerprint);
        return hash;
    }
};

LayoutKey MakeLayoutKey(const ContextSignature& sig, const RulesConfig& config,
                        uint32_t dpi, bool darkTheme, const MenuModel& model,
                        uint64_t themeRevision) {
    LayoutKey key{};
    key.sig = sig;
    key.rulesRevision = config.revision;
    key.appearanceRevision = config.revision;
    key.themeRevision = themeRevision;
    key.dpi = dpi;
    key.darkTheme = darkTheme;
    key.modelFingerprint = ModelFingerprint(model.items);
    return key;
}

class LayoutCache {
public:
    std::shared_ptr<const LayoutPanel> Find(const LayoutKey& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key.Hash());
        if (it == map_.end() || !(it->second.key == key)) {
            return nullptr;
        }
        order_.splice(order_.begin(), order_, it->second.orderIt);
        return it->second.panel;
    }

    void Put(const LayoutKey& key, std::shared_ptr<const LayoutPanel> panel) {
        std::lock_guard<std::mutex> lock(mutex_);
        const uint64_t hash = key.Hash();
        auto it = map_.find(hash);
        if (it != map_.end() && it->second.key == key) {
            it->second.panel = std::move(panel);
            order_.splice(order_.begin(), order_, it->second.orderIt);
            return;
        }

        order_.push_front(hash);
        Entry entry;
        entry.key = key;
        entry.panel = std::move(panel);
        entry.orderIt = order_.begin();
        map_[hash] = std::move(entry);

        while (map_.size() > maxEntries_ && !order_.empty()) {
            const uint64_t victim = order_.back();
            order_.pop_back();
            map_.erase(victim);
        }
    }

    void InvalidateDevice() { Clear(); }

#ifdef CMO_TESTING
    void SetMaxEntries(size_t maxEntries);
#endif

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.size();
    }

private:
    struct Entry {
        LayoutKey key;
        std::shared_ptr<const LayoutPanel> panel;
        std::list<uint64_t>::iterator orderIt;
    };

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        map_.clear();
        order_.clear();
    }

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, Entry> map_;
    std::list<uint64_t> order_;
    size_t maxEntries_ = 32;
};

// Resource-owning globals: their destructors must not run at process exit
// (Explorer restart/sign-out), where the threads and subsystems they assume
// are gone. They are released explicitly in Wh_ModUninit instead.
CMO_NO_DESTROY inline std::optional<LayoutCache> g_layoutCache{std::in_place};

// Bounded least-recently-used map; Find promotes, Insert evicts the oldest
// and releases its value.
template <typename T>
class LruMap {
public:
    void SetMaxEntries(size_t maxEntries) {
        maxEntries_ = maxEntries == 0 ? 1 : maxEntries;
    }

    size_t Size() const { return map_.size(); }

    T* Find(const std::wstring& key) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return nullptr;
        }
        order_.splice(order_.begin(), order_, it->second.orderIt);
        return &it->second.value;
    }

    template <typename F>
    void Insert(const std::wstring& key, T value, F release) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second.value = std::move(value);
            order_.splice(order_.begin(), order_, it->second.orderIt);
            return;
        }
        order_.push_front(key);
        Entry entry;
        entry.value = std::move(value);
        entry.orderIt = order_.begin();
        map_.emplace(key, std::move(entry));

        while (map_.size() > maxEntries_ && !order_.empty()) {
            const std::wstring victim = order_.back();
            order_.pop_back();
            auto victimIt = map_.find(victim);
            if (victimIt != map_.end()) {
                release(victimIt->second.value);
                map_.erase(victimIt);
            }
        }
    }

    template <typename F>
    void Clear(F release) {
        for (auto& pair : map_) {
            release(pair.second.value);
        }
        map_.clear();
        order_.clear();
    }

private:
    struct Entry {
        T value;
        std::list<std::wstring>::iterator orderIt;
    };

    std::unordered_map<std::wstring, Entry> map_;
    std::list<std::wstring> order_;
    size_t maxEntries_ = 256;
};

// ===========================================================================
// [CMO:Mode] Custom vs HMENU mode selection and failure fallback.
// ===========================================================================

enum class MenuMode : uint8_t { Custom, HMenu };

MenuMode ResolveMenuMode(int settingValue, int consecutiveCustomFailures) {
    if (settingValue != 0) {
        return MenuMode::HMenu;
    }
    if (consecutiveCustomFailures >= 3) {
        return MenuMode::HMenu;
    }
    return MenuMode::Custom;
}

class ModeController {
public:
    MenuMode Current() const { return mode_; }

    void RecordSuccess() { consecutiveFailures_ = 0; }

    void RecordFailure() {
        if (consecutiveFailures_ < 3) {
            ++consecutiveFailures_;
        }
        if (consecutiveFailures_ >= 3) {
            mode_ = MenuMode::HMenu;
        }
    }

    int ConsecutiveFailures() const { return consecutiveFailures_; }

private:
    MenuMode mode_ = MenuMode::Custom;
    int consecutiveFailures_ = 0;
};

inline ModeController g_modeController;

// ===========================================================================
// [CMO:RenderDevice] Shared D3D11/D2D/DWrite/DComp device.
// ===========================================================================

// MinGW declares the interfaces but does not export the IID symbols from the
// import libraries, so carry local copies.
const GUID kIidD2D1Factory = {
    0x06152247, 0x6f50, 0x465a, {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};
const GUID kIidD2D1Factory1 = {
    0xbb12d362, 0xdaee, 0x4b9a, {0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f}};
const GUID kIidDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b, {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};
const GUID kIidDCompositionDevice = {
    0xc37ea93a, 0xe7aa, 0x450d, {0xb1, 0x6f, 0x97, 0x46, 0xcb, 0x04, 0x07, 0xf3}};
const GUID kIidIDXGIDevice = {
    0x54ec77fa, 0x1377, 0x44e6, {0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c}};

class RenderDevice {
public:
    bool Initialize() {
        if (ready_) {
            return true;
        }
        if (failed_) {
            return false;
        }

        static const D3D_FEATURE_LEVEL kLevels[] = {
            D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
        D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_10_0;
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            kLevels, ARRAYSIZE(kLevels), D3D11_SDK_VERSION, &d3d_, &level, nullptr);
        if (FAILED(hr) || !d3d_) {
            hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                kLevels, ARRAYSIZE(kLevels), D3D11_SDK_VERSION, &d3d_, &level, nullptr);
        }
        if (FAILED(hr) || !d3d_) {
            failed_ = true;
            return false;
        }

        if (FAILED(d3d_->QueryInterface(kIidIDXGIDevice,
                                        reinterpret_cast<void**>(&dxgi_))) ||
            !dxgi_) {
            Shutdown();
            failed_ = true;
            return false;
        }

        D2D1_FACTORY_OPTIONS options = {};
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, kIidD2D1Factory,
                                     &options,
                                     reinterpret_cast<void**>(&d2dFactory_))) ||
            !d2dFactory_) {
            Shutdown();
            failed_ = true;
            return false;
        }
        if (FAILED(d2dFactory_->QueryInterface(kIidD2D1Factory1,
                                               reinterpret_cast<void**>(&d2dFactory1_))) ||
            !d2dFactory1_) {
            Shutdown();
            failed_ = true;
            return false;
        }
        if (FAILED(d2dFactory1_->CreateDevice(dxgi_, &d2dDevice_)) || !d2dDevice_) {
            Shutdown();
            failed_ = true;
            return false;
        }

        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, kIidDWriteFactory,
                                       reinterpret_cast<IUnknown**>(&dwrite_))) ||
            !dwrite_) {
            Shutdown();
            failed_ = true;
            return false;
        }

        if (FAILED(DCompositionCreateDevice(dxgi_, kIidDCompositionDevice,
                                            reinterpret_cast<void**>(&comp_))) ||
            !comp_) {
            Shutdown();
            failed_ = true;
            return false;
        }

        ready_ = true;
        return true;
    }

    bool IsReady() const { return ready_; }

    void HandleDeviceLost() {
        Shutdown();
        failed_ = false;
    }

    void Shutdown() {
        ready_ = false;
        if (comp_) {
            comp_->Release();
            comp_ = nullptr;
        }
        if (dwrite_) {
            dwrite_->Release();
            dwrite_ = nullptr;
        }
        if (d2dDevice_) {
            d2dDevice_->Release();
            d2dDevice_ = nullptr;
        }
        if (d2dFactory1_) {
            d2dFactory1_->Release();
            d2dFactory1_ = nullptr;
        }
        if (d2dFactory_) {
            d2dFactory_->Release();
            d2dFactory_ = nullptr;
        }
        if (dxgi_) {
            dxgi_->Release();
            dxgi_ = nullptr;
        }
        if (d3d_) {
            d3d_->Release();
            d3d_ = nullptr;
        }
    }

    ID3D11Device* D3DDevice() const { return d3d_; }
    IDXGIDevice* DxgiDevice() const { return dxgi_; }
    ID2D1Factory* D2DFactory() const { return d2dFactory_; }
    ID2D1Factory1* D2DFactory1() const { return d2dFactory1_; }
    ID2D1Device* D2DDevice() const { return d2dDevice_; }
    IDWriteFactory* DWriteFactory() const { return dwrite_; }
    IDCompositionDevice* CompDevice() const { return comp_; }

private:
    bool ready_ = false;
    bool failed_ = false;
    ID3D11Device* d3d_ = nullptr;
    IDXGIDevice* dxgi_ = nullptr;
    ID2D1Factory* d2dFactory_ = nullptr;
    ID2D1Factory1* d2dFactory1_ = nullptr;
    ID2D1Device* d2dDevice_ = nullptr;
    IDWriteFactory* dwrite_ = nullptr;
    IDCompositionDevice* comp_ = nullptr;
};

// RenderDevice holds raw COM pointers and releases them in Shutdown(); its
// destructor is trivial, so it needs neither the attribute nor reset().
inline RenderDevice g_renderDevice;

// ===========================================================================
// [CMO:MenuWindow] DirectComposition-backed popup windows.
// ===========================================================================

const GUID kIidIDXGIFactory2 = {
    0x50c83a1c, 0xe072, 0x4c48, {0x87, 0xb0, 0x36, 0x30, 0xfa, 0x36, 0xa6, 0xd0}};

const wchar_t kMenuWindowClass[] = L"ContextMenuOverhaulV2Window";

DWORD MenuWindowStyle() { return WS_POPUP; }

DWORD MenuWindowExStyle(bool activate = false) {
    DWORD style = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOREDIRECTIONBITMAP;
    // Menus normally never activate so the owner keeps its active look. The
    // hotkey-opened settings menu must activate: it runs on the hotkey thread
    // and needs keyboard input delivered to that thread's queue.
    if (!activate) {
        style |= WS_EX_NOACTIVATE;
    }
    return style;
}

class MenuWindow;

using MenuWindowMessageFn = LRESULT (*)(MenuWindow*, HWND, UINT, WPARAM, LPARAM);
inline MenuWindowMessageFn g_menuWindowMessageHook = nullptr;

class MenuWindow {
public:
    ~MenuWindow() { Destroy(); }

    bool Create(HWND owner, const LayoutPanel* panel, bool isRoot,
                int margin = 0, bool activate = false) {
        // Defensive: release anything from a failed earlier attempt so Create
        // never leaks an HWND, swap chain, target, or visual.
        Destroy();

        owner_ = owner;
        panel_ = panel;
        isRoot_ = isRoot;
        margin_ = margin > 0 ? margin : 0;

        if (!RegisterClassOnce()) {
            return false;
        }

        const int width = (panel && panel->size.cx > 0 ? panel->size.cx : 100) +
                          2 * margin_;
        const int height = (panel && panel->size.cy > 0 ? panel->size.cy : 100) +
                           2 * margin_;
        hwnd_ = CreateWindowExW(MenuWindowExStyle(activate), kMenuWindowClass,
                                L"", MenuWindowStyle(), 0, 0, width, height,
                                owner, nullptr, ModModuleHandle(), this);
        if (!hwnd_) {
            return false;
        }
        SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        if (!CreateSurface(width, height)) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
            return false;
        }
        return true;
    }

    void Move(POINT screenPos) {
        if (!hwnd_) {
            return;
        }
        SetWindowPos(hwnd_, HWND_TOPMOST, screenPos.x, screenPos.y, 0, 0,
                     SWP_NOSIZE | SWP_NOACTIVATE);
    }

    void Show() {
        if (!hwnd_) {
            return;
        }
        // Never activate: the owner keeps its active look, like native menus.
        ::ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    }

    void Destroy() {
        if (visual_) {
            visual_->Release();
            visual_ = nullptr;
        }
        if (target_) {
            target_->Release();
            target_ = nullptr;
        }
        if (swapChain_) {
            swapChain_->Release();
            swapChain_ = nullptr;
        }
        if (hwnd_) {
            SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    HWND Handle() const { return hwnd_; }
    const LayoutPanel* Panel() const { return panel_; }
    bool IsRoot() const { return isRoot_; }
    int Margin() const { return margin_; }
    IDXGISwapChain1* SwapChain() const { return swapChain_; }
    IDCompositionTarget* CompTarget() const { return target_; }
    IDCompositionVisual* CompVisual() const { return visual_; }

    void Present() {
        if (swapChain_) {
            swapChain_->Present(1, 0);
        }
    }

    void Resize(int width, int height) {
        if (!swapChain_ || width <= 0 || height <= 0) {
            return;
        }
        swapChain_->ResizeBuffers(0, static_cast<UINT>(width),
                                  static_cast<UINT>(height), DXGI_FORMAT_UNKNOWN, 0);
    }

    // Relayout support: swap in a rebuilt panel and resize/move the window.
    void SetPanel(const LayoutPanel* panel) { panel_ = panel; }

    void SetBounds(POINT screenPos, int width, int height) {
        if (!hwnd_ || width <= 0 || height <= 0) {
            return;
        }
        Resize(width, height);
        SetWindowPos(hwnd_, HWND_TOPMOST, screenPos.x, screenPos.y, width,
                     height, SWP_NOACTIVATE);
    }

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (g_menuWindowMessageHook) {
            return g_menuWindowMessageHook(this, hwnd, msg, wParam, lParam);
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

private:
    static bool RegisterClassOnce() {
        static bool registered = false;
        if (registered) {
            return true;
        }
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &MenuWindow::WindowProc;
        wc.hInstance = ModModuleHandle();
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        wc.lpszClassName = kMenuWindowClass;
        if (!RegisterClassExW(&wc)) {
            // A class left behind by a previous load would keep pointing at
            // the old window procedure; reclaim it instead of using it.
            UnregisterClassW(kMenuWindowClass, ModModuleHandle());
            if (!RegisterClassExW(&wc)) {
                return false;
            }
        }
        registered = true;
        return true;
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam) {
        auto* window =
            reinterpret_cast<MenuWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (window) {
            return window->HandleMessage(hwnd, msg, wParam, lParam);
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    bool CreateSurface(int width, int height) {
        if (!g_renderDevice.IsReady() || width <= 0 || height <= 0) {
            return false;
        }

        IDXGIDevice* dxgi = g_renderDevice.DxgiDevice();
        IDXGIAdapter* adapter = nullptr;
        if (FAILED(dxgi->GetAdapter(&adapter)) || !adapter) {
            return false;
        }
        IDXGIFactory2* factory = nullptr;
        const HRESULT factoryHr =
            adapter->GetParent(kIidIDXGIFactory2, reinterpret_cast<void**>(&factory));
        adapter->Release();
        if (FAILED(factoryHr) || !factory) {
            return false;
        }

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = static_cast<UINT>(width);
        desc.Height = static_cast<UINT>(height);
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        const HRESULT chainHr = factory->CreateSwapChainForComposition(
            g_renderDevice.D3DDevice(), &desc, nullptr, &swapChain_);
        factory->Release();
        if (FAILED(chainHr) || !swapChain_) {
            return false;
        }

        IDCompositionDevice* comp = g_renderDevice.CompDevice();
        if (FAILED(comp->CreateTargetForHwnd(hwnd_, TRUE, &target_)) || !target_) {
            return false;
        }
        if (FAILED(comp->CreateVisual(&visual_)) || !visual_) {
            return false;
        }
        if (FAILED(visual_->SetContent(swapChain_))) {
            return false;
        }
        if (FAILED(target_->SetRoot(visual_))) {
            return false;
        }
        comp->Commit();
        return true;
    }

    HWND hwnd_ = nullptr;
    HWND owner_ = nullptr;
    const LayoutPanel* panel_ = nullptr;
    bool isRoot_ = false;
    int margin_ = 0;
    IDXGISwapChain1* swapChain_ = nullptr;
    IDCompositionTarget* target_ = nullptr;
    IDCompositionVisual* visual_ = nullptr;
};

// Set while Wh_ModUninit tears the mod down. New sessions are refused and
// releases destroy their window immediately, because the DLL (and its window
// procedures) is about to be unloaded.
inline std::atomic<bool> g_unloading{false};
// Custom menu sessions currently running on the UI thread. Uninit waits for
// this to reach zero before releasing UI-thread resources.
inline std::atomic<long> g_activeSessions{0};
// UI-thread work (menu preparation, native replay) in progress. Uninit waits
// for it so the DLL is not unloaded while a stack frame points into it.
inline std::atomic<long> g_uiBusy{0};

struct UiBusyScope {
    UiBusyScope() { g_uiBusy.fetch_add(1); }
    ~UiBusyScope() { g_uiBusy.fetch_sub(1); }
    UiBusyScope(const UiBusyScope&) = delete;
    UiBusyScope& operator=(const UiBusyScope&) = delete;
};
// Hidden message-only windows, one per UI thread that shows a menu.
// Wh_ModUninit posts a teardown request to each and waits for all of them:
// DestroyWindow cannot destroy another thread's window, and a window left
// behind would keep a class procedure that points into the unloaded DLL.
inline std::mutex g_controlWindowsMutex;
inline std::unordered_map<DWORD, HWND> g_controlWindows;

// Windows live for one menu session and are destroyed on the thread that owns
// them. Create() rebuilds the HWND and surface every time, so keeping them
// around saved nothing and its process-wide cap leaked a slot per Explorer
// thread until the custom menu stopped working for the rest of the session.
class MenuWindowSet {
public:
    static constexpr size_t kMaxWindows = 8;

    MenuWindow* Acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (all_.size() >= kMaxWindows) {
            return nullptr;
        }
        auto window = std::make_unique<MenuWindow>();
        MenuWindow* raw = window.get();
        all_.push_back(std::move(window));
        return raw;
    }

    // Runs on the session (owning) thread, which may destroy its windows.
    void Release(MenuWindow* window) {
        if (!window) {
            return;
        }
        window->Destroy();
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = all_.begin(); it != all_.end(); ++it) {
            if (it->get() == window) {
                all_.erase(it);
                break;
            }
        }
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return all_.size();
    }

private:
    mutable std::mutex mutex_;
    std::vector<std::unique_ptr<MenuWindow>> all_;
};

CMO_NO_DESTROY inline std::optional<MenuWindowSet> g_menuWindowSet{std::in_place};

// ===========================================================================
// [CMO:MenuWindow] Input state, content caches, and Direct2D drawing.
// ===========================================================================

struct MenuInputState {
    int hoverIndex = -1;
    int keyboardIndex = -1;
    int scrollOffset = 0;
    int openSubmenu = -1;
    // Settings UI state (empty/None for ordinary menus).
    std::wstring focusedControl;
    std::wstring editBuffer;
    size_t caretPos = 0;
    bool caretVisible = false;
    std::wstring dragControl;
    ControlPart dragPart = ControlPart::None;
    ControlPart hoverPart = ControlPart::None;
    // Live value shown while dragging a slider (the setting itself may be
    // deferred to release for geometry keys).
    int dragValue = 0;
};

struct BackdropBitmap {
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;
    // Average of the captured pixels (ARGB), used to adapt the shadow.
    uint32_t averageColor = 0xFF000000u;
};

void DownscaleAndBlur(const uint32_t* src, int srcW, int srcH, int factor,
                      int passes, std::vector<uint32_t>& out, int& outW,
                      int& outH) {
    out.clear();
    outW = 0;
    outH = 0;
    if (!src || srcW <= 0 || srcH <= 0 || factor <= 0) {
        return;
    }

    outW = (srcW + factor - 1) / factor;
    outH = (srcH + factor - 1) / factor;
    out.assign(static_cast<size_t>(outW) * static_cast<size_t>(outH), 0);

    for (int y = 0; y < outH; ++y) {
        for (int x = 0; x < outW; ++x) {
            uint32_t a = 0;
            uint32_t r = 0;
            uint32_t g = 0;
            uint32_t b = 0;
            uint32_t count = 0;
            for (int dy = 0; dy < factor && y * factor + dy < srcH; ++dy) {
                for (int dx = 0; dx < factor && x * factor + dx < srcW; ++dx) {
                    const uint32_t pixel =
                        src[static_cast<size_t>(y * factor + dy) * srcW +
                            (x * factor + dx)];
                    a += (pixel >> 24) & 0xFF;
                    r += (pixel >> 16) & 0xFF;
                    g += (pixel >> 8) & 0xFF;
                    b += pixel & 0xFF;
                    ++count;
                }
            }
            if (count == 0) {
                continue;
            }
            out[static_cast<size_t>(y) * outW + x] =
                ((a / count) << 24) | ((r / count) << 16) | ((g / count) << 8) |
                (b / count);
        }
    }

    for (int pass = 0; pass < passes; ++pass) {
        std::vector<uint32_t> blurred = out;
        for (int y = 0; y < outH; ++y) {
            for (int x = 0; x < outW; ++x) {
                uint32_t a = 0;
                uint32_t r = 0;
                uint32_t g = 0;
                uint32_t b = 0;
                uint32_t count = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    const int sy = y + dy;
                    if (sy < 0 || sy >= outH) {
                        continue;
                    }
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int sx = x + dx;
                        if (sx < 0 || sx >= outW) {
                            continue;
                        }
                        const uint32_t pixel =
                            out[static_cast<size_t>(sy) * outW + sx];
                        a += (pixel >> 24) & 0xFF;
                        r += (pixel >> 16) & 0xFF;
                        g += (pixel >> 8) & 0xFF;
                        b += pixel & 0xFF;
                        ++count;
                    }
                }
                if (count == 0) {
                    continue;
                }
                blurred[static_cast<size_t>(y) * outW + x] =
                    ((a / count) << 24) | ((r / count) << 16) |
                    ((g / count) << 8) | (b / count);
            }
        }
        out.swap(blurred);
    }
}

void BuildRoundedRectMask(int width, int height, int radius,
                          std::vector<uint8_t>& alpha) {
    if (width <= 0 || height <= 0) {
        alpha.clear();
        return;
    }
    alpha.assign(static_cast<size_t>(width) * static_cast<size_t>(height), 0);
    if (radius <= 0) {
        std::fill(alpha.begin(), alpha.end(), 255);
        return;
    }

    const float r = static_cast<float>(radius);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float px = static_cast<float>(x) + 0.5f;
            const float py = static_cast<float>(y) + 0.5f;
            const float cx = std::clamp(px, r, std::max(r, w - r));
            const float cy = std::clamp(py, r, std::max(r, h - r));
            const float dx = px - cx;
            const float dy = py - cy;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const float coverage = std::clamp(r - distance + 0.5f, 0.0f, 1.0f);
            alpha[static_cast<size_t>(y) * width + x] =
                static_cast<uint8_t>(coverage * 255.0f + 0.5f);
        }
    }
}

void BuildRoundedRectMaskRadii(int width, int height, int topLeft, int topRight,
                               int bottomRight, int bottomLeft,
                               std::vector<uint8_t>& alpha) {
    if (width <= 0 || height <= 0) {
        alpha.clear();
        return;
    }
    alpha.assign(static_cast<size_t>(width) * static_cast<size_t>(height), 255);

    const int maxRadius = std::min(width, height);
    auto clampRadius = [maxRadius](int radius) {
        return std::clamp(radius, 0, maxRadius);
    };
    const int tl = clampRadius(topLeft);
    const int tr = clampRadius(topRight);
    const int br = clampRadius(bottomRight);
    const int bl = clampRadius(bottomLeft);
    if (tl == 0 && tr == 0 && br == 0 && bl == 0) {
        return;
    }

    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float px = static_cast<float>(x) + 0.5f;
            const float py = static_cast<float>(y) + 0.5f;
            float radius = 0.0f;
            float cx = px;
            float cy = py;
            if (px < static_cast<float>(tl) && py < static_cast<float>(tl)) {
                radius = static_cast<float>(tl);
                cx = static_cast<float>(tl);
                cy = static_cast<float>(tl);
            } else if (px > w - static_cast<float>(tr) &&
                       py < static_cast<float>(tr)) {
                radius = static_cast<float>(tr);
                cx = w - static_cast<float>(tr);
                cy = static_cast<float>(tr);
            } else if (px > w - static_cast<float>(br) &&
                       py > h - static_cast<float>(br)) {
                radius = static_cast<float>(br);
                cx = w - static_cast<float>(br);
                cy = h - static_cast<float>(br);
            } else if (px < static_cast<float>(bl) &&
                       py > h - static_cast<float>(bl)) {
                radius = static_cast<float>(bl);
                cx = static_cast<float>(bl);
                cy = h - static_cast<float>(bl);
            }
            if (radius <= 0.0f) {
                continue;
            }
            const float dx = px - cx;
            const float dy = py - cy;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const float coverage = std::clamp(radius - distance + 0.5f, 0.0f, 1.0f);
            alpha[static_cast<size_t>(y) * width + x] =
                static_cast<uint8_t>(coverage * 255.0f + 0.5f);
        }
    }
}

// Keeps the configured shadow on light backdrops; on dark ones it fades the
// shadow and blends its color toward the backdrop, so it reads as depth
// instead of a black halo. Darkness ramps from luminance 0.55 down to 0.15.
void AdaptShadowToBackdrop(uint32_t averageColor, int configuredOpacity,
                           uint32_t configuredColor, int& outOpacity,
                           uint32_t& outColor) {
    outOpacity = configuredOpacity;
    outColor = configuredColor;

    // The adaptation is for dark, neutral shadows that would read as a black
    // halo. Colored or bright shadows (theme glows) are intentional effects
    // and keep their look on every backdrop.
    const float sr = ((configuredColor >> 16) & 0xFF) / 255.0f;
    const float sg = ((configuredColor >> 8) & 0xFF) / 255.0f;
    const float sb = (configuredColor & 0xFF) / 255.0f;
    const float shadowMax = std::max({sr, sg, sb});
    const float shadowMin = std::min({sr, sg, sb});
    const float shadowSaturation =
        shadowMax <= 0.0f ? 0.0f : (shadowMax - shadowMin) / shadowMax;
    const float shadowLuminance =
        0.2126f * sr + 0.7152f * sg + 0.0722f * sb;
    // Dark neutral shadows are the ones that read as a black halo and get the
    // full treatment; colored or bright shadows (theme glows) keep their hue.
    const bool neutral = shadowSaturation <= 0.25f && shadowLuminance <= 0.5f;

    const float r = ((averageColor >> 16) & 0xFF) / 255.0f;
    const float g = ((averageColor >> 8) & 0xFF) / 255.0f;
    const float b = (averageColor & 0xFF) / 255.0f;
    const float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    const float darkness =
        std::clamp((0.55f - luminance) / 0.40f, 0.0f, 1.0f);
    if (darkness <= 0.0f) {
        return;
    }
    // Every shadow fades on dark backdrops so the setting always has a
    // visible effect; the fade is gentler for colored glows.
    const float fade = neutral ? 0.70f : 0.35f;
    outOpacity = static_cast<int>(std::lround(
        static_cast<float>(configuredOpacity) * (1.0f - fade * darkness)));
    if (!neutral) {
        return;
    }
    const float mix = 0.6f * darkness;
    auto blend = [&](int channel, float target) {
        return static_cast<uint32_t>(std::lround(
            static_cast<float>(channel) * (1.0f - mix) + target * 255.0f * mix));
    };
    outColor = (configuredColor & 0xFF000000u) |
               (blend((configuredColor >> 16) & 0xFF, r) << 16) |
               (blend((configuredColor >> 8) & 0xFF, g) << 8) |
               blend(configuredColor & 0xFF, b);
}

bool BuildShadowBitmap(int width, int height, const CornerRadii& radii,
                       int spread, int blur, int opacity, uint32_t color,
                       int downscale, std::vector<uint32_t>& pixels, int& outW,
                       int& outH) {
    pixels.clear();
    outW = 0;
    outH = 0;
    if (width <= 0 || height <= 0 || spread < 0 || blur < 0 || downscale <= 0) {
        return false;
    }

    const int silhouetteW = width + 2 * spread;
    const int silhouetteH = height + 2 * spread;
    const int maskW = silhouetteW + 2 * blur;
    const int maskH = silhouetteH + 2 * blur;
    if (maskW <= 0 || maskH <= 0 ||
        static_cast<int64_t>(maskW) * maskH > 16 * 1024 * 1024) {
        return false;
    }
    std::vector<uint8_t> silhouette;
    BuildRoundedRectMaskRadii(silhouetteW, silhouetteH, radii.topLeft + spread,
                              radii.topRight + spread, radii.bottomRight + spread,
                              radii.bottomLeft + spread, silhouette);
    std::vector<uint8_t> mask(static_cast<size_t>(maskW) * maskH, 0);
    for (int y = 0; y < silhouetteH; ++y) {
        memcpy(&mask[static_cast<size_t>(y + blur) * maskW + blur],
               &silhouette[static_cast<size_t>(y) * silhouetteW],
               static_cast<size_t>(silhouetteW));
    }
    const uint32_t colorR = (color >> 16) & 0xFF;
    const uint32_t colorG = (color >> 8) & 0xFF;
    const uint32_t colorB = color & 0xFF;
    const uint32_t colorA = (color >> 24) & 0xFF;
    std::vector<uint32_t> argb(static_cast<size_t>(maskW) * maskH);
    for (size_t i = 0; i < argb.size(); ++i) {
        const uint32_t alpha =
            (static_cast<uint32_t>(mask[i]) * static_cast<uint32_t>(opacity) / 255) *
            colorA / 255;
        // Premultiplied shadow color.
        argb[i] = (alpha << 24) | ((colorR * alpha / 255) << 16) |
                  ((colorG * alpha / 255) << 8) | (colorB * alpha / 255);
    }

    std::vector<uint32_t> blurred;
    DownscaleAndBlur(argb.data(), maskW, maskH, downscale, BlurPasses(blur),
                     blurred, outW, outH);
    if (outW <= 0 || outH <= 0) {
        return false;
    }
    pixels = std::move(blurred);
    return true;
}

// Reuses the software shadow bitmap when the same panel is drawn again: every
// hover repaint redraws the panel, and the software blur is the expensive
// part. Only the serialized renderer touches this cache.
class ShadowBitmapCache {
public:
    bool Get(uint64_t key, std::vector<uint32_t>& pixels, int& w, int& h) {
        for (Entry& entry : entries_) {
            if (entry.valid && entry.key == key) {
                pixels = entry.pixels;
                w = entry.w;
                h = entry.h;
                entry.lastUse = ++clock_;
                return true;
            }
        }
        return false;
    }

    void Put(uint64_t key, const std::vector<uint32_t>& pixels, int w, int h) {
        if (pixels.size() > kMaxCachedPixels) {
            return;  // too large to keep resident
        }
        Entry* victim = &entries_[0];
        for (Entry& entry : entries_) {
            if (!entry.valid) {
                victim = &entry;
                break;
            }
            if (entry.lastUse < victim->lastUse) {
                victim = &entry;
            }
        }
        victim->valid = true;
        victim->key = key;
        victim->pixels = pixels;
        victim->w = w;
        victim->h = h;
        victim->lastUse = ++clock_;
    }

private:
    struct Entry {
        bool valid = false;
        uint64_t key = 0;
        int w = 0;
        int h = 0;
        std::vector<uint32_t> pixels;
        uint64_t lastUse = 0;
    };

    static constexpr size_t kEntries = 2;
    static constexpr size_t kMaxCachedPixels = 4 * 1024 * 1024;  // ~16 MB

    Entry entries_[kEntries];
    uint64_t clock_ = 0;
};

ShadowBitmapCache g_shadowCache;

bool GetCachedShadowBitmap(int width, int height, const CornerRadii& radii,
                           int spread, int blur, int opacity, uint32_t color,
                           int downscale, std::vector<uint32_t>& pixels,
                           int& outW, int& outH) {
    uint64_t key = HashCombine(
        1469598103934665603ULL,
        (static_cast<uint64_t>(width) << 32) | static_cast<uint64_t>(height));
    key = HashCombine(key,
                      (static_cast<uint64_t>(spread) << 32) |
                          static_cast<uint64_t>(blur));
    key = HashCombine(key,
                      (static_cast<uint64_t>(opacity) << 32) |
                          static_cast<uint64_t>(color));
    key = HashCombine(key, static_cast<uint64_t>(downscale));
    key = HashCombine(key,
                      (static_cast<uint64_t>(radii.topLeft) << 48) |
                          (static_cast<uint64_t>(radii.topRight) << 32) |
                          (static_cast<uint64_t>(radii.bottomRight) << 16) |
                          static_cast<uint64_t>(radii.bottomLeft));

    if (g_shadowCache.Get(key, pixels, outW, outH)) {
        return true;
    }
    if (!BuildShadowBitmap(width, height, radii, spread, blur, opacity, color,
                           downscale, pixels, outW, outH)) {
        return false;
    }
    g_shadowCache.Put(key, pixels, outW, outH);
    return true;
}

constexpr int kBackdropDownscaleFactor = 4;

bool CaptureBackdrop(const RECT& screenRect, int factor, int passes,
                     BackdropBitmap& out) {
    const int width = screenRect.right - screenRect.left;
    const int height = screenRect.bottom - screenRect.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    HDC memory = CreateCompatibleDC(screen);
    if (!memory) {
        ReleaseDC(nullptr, screen);
        return false;
    }

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) {
            DeleteObject(dib);
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return false;
    }

    HGDIOBJ oldBitmap = SelectObject(memory, dib);
    const BOOL copied =
        BitBlt(memory, 0, 0, width, height, screen, screenRect.left,
               screenRect.top, SRCCOPY);
    SelectObject(memory, oldBitmap);

    bool result = false;
    if (copied) {
        std::vector<uint32_t> src(static_cast<size_t>(width) *
                                  static_cast<size_t>(height));
        memcpy(src.data(), bits, src.size() * sizeof(uint32_t));
        int outW = 0;
        int outH = 0;
        std::vector<uint32_t> blurred;
        DownscaleAndBlur(src.data(), width, height, factor > 0 ? factor : 1,
                         passes, blurred, outW, outH);
        if (outW > 0 && outH > 0) {
            out.pixels = std::move(blurred);
            out.width = outW;
            out.height = outH;
            uint64_t r = 0;
            uint64_t g = 0;
            uint64_t b = 0;
            for (uint32_t pixel : out.pixels) {
                r += (pixel >> 16) & 0xFF;
                g += (pixel >> 8) & 0xFF;
                b += pixel & 0xFF;
            }
            const uint64_t count = out.pixels.size();
            out.averageColor =
                0xFF000000u | (static_cast<uint32_t>(r / count) << 16) |
                (static_cast<uint32_t>(g / count) << 8) |
                static_cast<uint32_t>(b / count);
            result = true;
        }
    }

    DeleteObject(dib);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
    return result;
}

D2D1_COLOR_F ColorFromArgb(uint32_t argb) {
    D2D1_COLOR_F color = {};
    color.a = static_cast<float>((argb >> 24) & 0xFF) / 255.0f;
    color.r = static_cast<float>((argb >> 16) & 0xFF) / 255.0f;
    color.g = static_cast<float>((argb >> 8) & 0xFF) / 255.0f;
    color.b = static_cast<float>(argb & 0xFF) / 255.0f;
    return color;
}

// Defined after IconCache below (declaration order); content caches call it.
HBITMAP GetIconBitmapForMenu(const std::wstring& iconRef,
                             const std::vector<uint8_t>& iconPixels, int sizePx);
HBITMAP GetAlphaIconBitmapForMenu(const std::wstring& iconRef,
                                  const std::vector<uint8_t>& iconPixels,
                                  int sizePx);

class ContentCaches {
public:
    ContentCaches() {
        text_.SetMaxEntries(256);
        icons_.SetMaxEntries(256);
        glyphs_.SetMaxEntries(128);
    }

    // The maps hold raw D2D/DWrite pointers; only Clear() releases them.
    ~ContentCaches() { Clear(); }

    void SetDevice(ID2D1DeviceContext* dc, IDWriteFactory* dwrite) {
        dc_ = dc;
        dwrite_ = dwrite;
        iconFontUnavailable_ = false;  // the collection may have changed
    }

    void Bind(LayoutPanel& panel, const LayoutMetrics& metrics,
              ID2D1DeviceContext* dc) {
        SetDevice(dc, g_renderDevice.DWriteFactory());
        BindPanel(panel, metrics);
    }

    void Clear() {
        text_.Clear([](IDWriteTextLayout* layout) {
            if (layout) {
                layout->Release();
            }
        });
        icons_.Clear([](ID2D1Bitmap* bitmap) {
            if (bitmap) {
                bitmap->Release();
            }
        });
        glyphs_.Clear([](IDWriteTextLayout* layout) {
            if (layout) {
                layout->Release();
            }
        });
        dc_ = nullptr;
    }

    size_t TextCount() const { return text_.Size(); }
    size_t IconCount() const { return icons_.Size(); }

private:
    void BindPanel(LayoutPanel& panel, const LayoutMetrics& metrics) {
        for (LayoutItem& item : panel.items) {
            if (item.kind == ItemKind::Separator) {
                continue;
            }
            auto resources = std::make_shared<LayoutItemResources>();
            resources->text = GetOrCreateText(item, metrics);
            resources->glyph = GetOrCreateGlyph(item, metrics);
            resources->icon =
                resources->glyph ? nullptr : GetOrCreateIcon(item, metrics);
            // The cache owns one reference; each item's resources own their own.
            if (resources->text) {
                resources->text->AddRef();
            }
            if (resources->glyph) {
                resources->glyph->AddRef();
            }
            if (resources->icon) {
                resources->icon->AddRef();
            }
            item.resources = std::move(resources);
        }
        for (LayoutPanel& child : panel.children) {
            BindPanel(child, metrics);
        }
    }

    IDWriteTextLayout* GetOrCreateText(const LayoutItem& item,
                                       const LayoutMetrics& metrics) {
        const int width = static_cast<int>(item.textRect.right - item.textRect.left);
        if (width <= 0) {
            return nullptr;
        }
        const std::wstring key =
            item.label + L'\x1f' + metrics.fontFace + L'\x1f' +
            std::to_wstring(static_cast<int>(metrics.fontSize * 4.0f)) + L'\x1f' +
            std::to_wstring(width) + L'\x1f' +
            std::to_wstring(static_cast<int>(metrics.acceleratorMode)) + L'\x1f' +
            std::to_wstring(static_cast<int>(metrics.fontWeight)) + L'\x1f' +
            std::to_wstring(static_cast<int>(metrics.fontStyle));
        if (IDWriteTextLayout** cached = text_.Find(key)) {
            return *cached;
        }
        if (!dc_ || !dwrite_) {
            return nullptr;
        }

        IDWriteTextFormat* format = nullptr;
        if (FAILED(dwrite_->CreateTextFormat(
                metrics.fontFace.c_str(), nullptr,
                FontWeightToDwrite(metrics.fontWeight),
                FontStyleToDwrite(metrics.fontStyle),
                DWRITE_FONT_STRETCH_NORMAL, metrics.fontSize, L"", &format)) ||
            !format) {
            return nullptr;
        }

        const AcceleratorText accelerated = StripAccelerators(item.label);
        const std::wstring& display =
            metrics.acceleratorMode == AcceleratorMode::Raw ? item.label
                                                            : accelerated.text;
        IDWriteTextLayout* layout = nullptr;
        dwrite_->CreateTextLayout(display.c_str(),
                                  static_cast<UINT32>(display.size()), format,
                                  static_cast<float>(width),
                                  static_cast<float>(metrics.itemHeight), &layout);
        if (layout) {
            if (metrics.acceleratorMode == AcceleratorMode::Underline) {
                for (const auto& range : accelerated.underlineRanges) {
                    const DWRITE_TEXT_RANGE textRange = {
                        static_cast<UINT32>(range.first),
                        static_cast<UINT32>(range.second - range.first)};
                    layout->SetUnderline(TRUE, textRange);
                }
            }
            layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            IDWriteInlineObject* ellipsis = nullptr;
            dwrite_->CreateEllipsisTrimmingSign(format, &ellipsis);
            if (ellipsis) {
                layout->SetTrimming(&trimming, ellipsis);
                ellipsis->Release();
            }
            text_.Insert(key, layout, [](IDWriteTextLayout* value) {
                if (value) {
                    value->Release();
                }
            });
        }
        format->Release();
        return layout;
    }

    IDWriteTextFormat* CreateIconTextFormat(float size) {
        if (!dwrite_ || iconFontUnavailable_) {
            return nullptr;  // negative cache: no icon font on this system
        }
        static const wchar_t* kFamilies[] = {L"Segoe Fluent Icons",
                                             L"Segoe MDL2 Assets"};
        IDWriteFontCollection* collection = nullptr;
        dwrite_->GetSystemFontCollection(&collection, FALSE);
        for (const wchar_t* family : kFamilies) {
            BOOL exists = FALSE;
            UINT32 index = 0;
            if (collection &&
                SUCCEEDED(collection->FindFamilyName(family, &index, &exists)) &&
                exists) {
                IDWriteTextFormat* format = nullptr;
                if (SUCCEEDED(dwrite_->CreateTextFormat(
                        family, nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
                        L"", &format)) &&
                    format) {
                    collection->Release();
                    return format;
                }
            }
        }
        if (collection) {
            collection->Release();
        }
        iconFontUnavailable_ = true;
        return nullptr;
    }

    IDWriteTextLayout* GetOrCreateGlyph(const LayoutItem& item,
                                        const LayoutMetrics& metrics) {
        const std::wstring codepoint = IconRefGlyph(item.iconRef);
        if (codepoint.empty() || !dwrite_) {
            return nullptr;
        }
        const std::wstring key = L"g#" + codepoint + L"#" +
                                 std::to_wstring(metrics.iconSize);
        if (IDWriteTextLayout** cached = glyphs_.Find(key)) {
            return *cached;
        }
        IDWriteTextFormat* format =
            CreateIconTextFormat(static_cast<float>(metrics.iconSize));
        if (!format) {
            return nullptr;
        }
        const wchar_t character =
            static_cast<wchar_t>(wcstoul(codepoint.c_str(), nullptr, 16));
        const wchar_t text[2] = {character, 0};
        IDWriteTextLayout* layout = nullptr;
        dwrite_->CreateTextLayout(text, 1, format,
                                  static_cast<float>(metrics.iconSize),
                                  static_cast<float>(metrics.iconSize), &layout);
        format->Release();
        if (layout) {
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            glyphs_.Insert(key, layout, [](IDWriteTextLayout* value) {
                if (value) {
                    value->Release();
                }
            });
        }
        return layout;
    }

    ID2D1Bitmap* GetOrCreateIcon(const LayoutItem& item,
                                 const LayoutMetrics& metrics) {
        if (item.iconRef.empty() && item.iconPixels.empty()) {
            return nullptr;
        }
        std::wstring key =
            item.iconRef + L'#' + std::to_wstring(metrics.iconSize);
        if (!item.iconPixels.empty()) {
            uint64_t hash = 1469598103934665603ull;
            for (uint8_t byte : item.iconPixels) {
                hash ^= byte;
                hash *= 1099511628211ull;
            }
            key += L'#' + std::to_wstring(hash);
        }
        if (ID2D1Bitmap** cached = icons_.Find(key)) {
            return *cached;
        }
        if (!dc_) {
            return nullptr;
        }
        HBITMAP hbitmap = GetAlphaIconBitmapForMenu(item.iconRef, item.iconPixels,
                                                    metrics.iconSize);
        if (!hbitmap) {
            return nullptr;
        }
        ID2D1Bitmap* bitmap = BitmapFromHBITMAP(hbitmap);
        if (bitmap) {
            icons_.Insert(key, bitmap, [](ID2D1Bitmap* value) {
                if (value) {
                    value->Release();
                }
            });
        }
        return bitmap;
    }

    ID2D1Bitmap* BitmapFromHBITMAP(HBITMAP hbitmap) {
        BITMAP bm = {};
        if (!GetObjectW(hbitmap, sizeof(bm), &bm) || bm.bmBitsPixel != 32) {
            return nullptr;
        }
        const int width = bm.bmWidth;
        const int height = bm.bmHeight;
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        std::vector<uint32_t> pixels(static_cast<size_t>(width) *
                                     static_cast<size_t>(height));
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        HDC screen = GetDC(nullptr);
        const int lines = GetDIBits(screen, hbitmap, 0, static_cast<UINT>(height),
                                    pixels.data(), &info, DIB_RGB_COLORS);
        ReleaseDC(nullptr, screen);
        if (lines == 0) {
            return nullptr;
        }

        ID2D1Bitmap* bitmap = nullptr;
        const D2D1_SIZE_U size = {static_cast<UINT32>(width),
                                  static_cast<UINT32>(height)};
        const D2D1_BITMAP_PROPERTIES props = {
            {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED}, 96.0f,
            96.0f};
        if (FAILED(dc_->CreateBitmap(size, pixels.data(),
                                     static_cast<UINT32>(width * sizeof(uint32_t)),
                                     props, &bitmap))) {
            return nullptr;
        }
        return bitmap;
    }

    ID2D1DeviceContext* dc_ = nullptr;
    IDWriteFactory* dwrite_ = nullptr;
    LruMap<IDWriteTextLayout*> text_;
    LruMap<ID2D1Bitmap*> icons_;
    LruMap<IDWriteTextLayout*> glyphs_;
    bool iconFontUnavailable_ = false;
};

CMO_NO_DESTROY inline std::optional<ContentCaches> g_contentCaches{std::in_place};

void DrawCheckmark(ID2D1DeviceContext* dc, const RECT& gutterRect,
                   uint32_t color) {
    ID2D1Factory* factory = g_renderDevice.D2DFactory();
    if (!factory) {
        return;
    }
    ID2D1PathGeometry* geometry = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geometry)) || !geometry) {
        return;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (SUCCEEDED(geometry->Open(&sink)) && sink) {
        const float left = static_cast<float>(gutterRect.left);
        const float top = static_cast<float>(gutterRect.top);
        const float width = static_cast<float>(gutterRect.right - gutterRect.left);
        const float height = static_cast<float>(gutterRect.bottom - gutterRect.top);
        sink->BeginFigure(
            D2D1_POINT_2F{left + width * 0.22f, top + height * 0.52f},
            D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1_POINT_2F{left + width * 0.42f, top + height * 0.72f});
        sink->AddLine(D2D1_POINT_2F{left + width * 0.78f, top + height * 0.30f});
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        sink->Release();

        ID2D1SolidColorBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) &&
            brush) {
            dc->DrawGeometry(geometry, brush, 1.5f);
            brush->Release();
        }
    }
    geometry->Release();
}

void DrawMarkerDot(ID2D1DeviceContext* dc, const RECT& markerRect,
                   uint32_t color) {
    ID2D1SolidColorBrush* brush = nullptr;
    if (FAILED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) || !brush) {
        return;
    }
    const float cx = (static_cast<float>(markerRect.left) +
                      static_cast<float>(markerRect.right)) / 2.0f;
    const float cy = (static_cast<float>(markerRect.top) +
                      static_cast<float>(markerRect.bottom)) / 2.0f;
    const float radius = std::max(
        2.0f, (static_cast<float>(markerRect.right) -
               static_cast<float>(markerRect.left)) * 0.18f);
    const D2D1_ELLIPSE ellipse = {D2D1_POINT_2F{cx, cy}, radius, radius};
    dc->FillEllipse(&ellipse, brush);
    brush->Release();
}

void DrawMarkerBar(ID2D1DeviceContext* dc, const RECT& markerRect,
                   uint32_t color) {
    ID2D1SolidColorBrush* brush = nullptr;
    if (FAILED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) || !brush) {
        return;
    }
    const float width = std::max(
        2.0f, (static_cast<float>(markerRect.right) -
               static_cast<float>(markerRect.left)) * 0.2f);
    const float cx = (static_cast<float>(markerRect.left) +
                      static_cast<float>(markerRect.right)) / 2.0f;
    const float cy = (static_cast<float>(markerRect.top) +
                      static_cast<float>(markerRect.bottom)) / 2.0f;
    const float half =
        (static_cast<float>(markerRect.bottom) -
         static_cast<float>(markerRect.top)) * 0.25f;
    const D2D1_ROUNDED_RECT bar = {{cx - width / 2, cy - half, cx + width / 2,
                                     cy + half},
                                    width / 2, width / 2};
    dc->FillRoundedRectangle(&bar, brush);
    brush->Release();
}

// Per-corner rounded panel geometry.
ID2D1PathGeometry* BuildPanelGeometry(ID2D1Factory* factory, float width,
                                      float height, const CornerRadii& radii) {
    if (!factory) {
        return nullptr;
    }
    ID2D1PathGeometry* geometry = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geometry)) || !geometry) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geometry->Open(&sink)) || !sink) {
        geometry->Release();
        return nullptr;
    }
    const float tl = static_cast<float>(radii.topLeft);
    const float tr = static_cast<float>(radii.topRight);
    const float br = static_cast<float>(radii.bottomRight);
    const float bl = static_cast<float>(radii.bottomLeft);
    sink->BeginFigure(D2D1_POINT_2F{tl, 0.0f}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1_POINT_2F{width - tr, 0.0f});
    if (tr > 0) {
        const D2D1_ARC_SEGMENT arc = {D2D1_POINT_2F{width, tr}, {tr, tr}, 0.0f,
                                      D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL};
        sink->AddArc(&arc);
    }
    sink->AddLine(D2D1_POINT_2F{width, height - br});
    if (br > 0) {
        const D2D1_ARC_SEGMENT arc = {D2D1_POINT_2F{width - br, height}, {br, br},
                                      0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL};
        sink->AddArc(&arc);
    }
    sink->AddLine(D2D1_POINT_2F{bl, height});
    if (bl > 0) {
        const D2D1_ARC_SEGMENT arc = {D2D1_POINT_2F{0.0f, height - bl}, {bl, bl},
                                      0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL};
        sink->AddArc(&arc);
    }
    sink->AddLine(D2D1_POINT_2F{0.0f, tl});
    if (tl > 0) {
        const D2D1_ARC_SEGMENT arc = {D2D1_POINT_2F{tl, 0.0f}, {tl, tl}, 0.0f,
                                      D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL};
        sink->AddArc(&arc);
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geometry;
}

void DrawSubmenuArrow(ID2D1DeviceContext* dc, const LayoutItem& item,
                      int panelWidth, const LayoutMetrics& metrics,
                      uint32_t color) {
    ID2D1SolidColorBrush* brush = nullptr;
    if (FAILED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) || !brush) {
        return;
    }
    const float right = static_cast<float>(panelWidth - metrics.padding);
    const float left = right - static_cast<float>(metrics.submenuArrowWidth) * 0.4f;
    const float midY = (static_cast<float>(item.rect.top) +
                        static_cast<float>(item.rect.bottom)) / 2.0f;
    const float half = static_cast<float>(metrics.submenuArrowWidth) * 0.22f;
    dc->DrawLine(D2D1_POINT_2F{left, midY - half}, D2D1_POINT_2F{right, midY},
                 brush, 1.5f);
    dc->DrawLine(D2D1_POINT_2F{right, midY}, D2D1_POINT_2F{left, midY + half},
                 brush, 1.5f);
    brush->Release();
}

// --- Settings control drawing ----------------------------------------------

D2D1_RECT_F RectF(const RECT& rect) {
    return D2D1::RectF(static_cast<float>(rect.left),
                       static_cast<float>(rect.top),
                       static_cast<float>(rect.right),
                       static_cast<float>(rect.bottom));
}

float SettingsTextWidth(const std::wstring& text, const LayoutMetrics& metrics) {
    IDWriteFactory* dwrite = g_renderDevice.DWriteFactory();
    if (!dwrite || text.empty()) {
        return 0.0f;
    }
    IDWriteTextFormat* format = nullptr;
    if (FAILED(dwrite->CreateTextFormat(
            metrics.fontFace.c_str(), nullptr,
            FontWeightToDwrite(metrics.fontWeight),
            FontStyleToDwrite(metrics.fontStyle), DWRITE_FONT_STRETCH_NORMAL,
            metrics.fontSize, L"", &format)) ||
        !format) {
        return 0.0f;
    }
    IDWriteTextLayout* layout = nullptr;
    float width = 0.0f;
    if (SUCCEEDED(dwrite->CreateTextLayout(text.c_str(),
                                           static_cast<UINT32>(text.size()),
                                           format, 4096.0f, 64.0f, &layout)) &&
        layout) {
        DWRITE_TEXT_METRICS textMetrics = {};
        if (SUCCEEDED(layout->GetMetrics(&textMetrics))) {
            width = textMetrics.widthIncludingTrailingWhitespace;
        }
        layout->Release();
    }
    format->Release();
    return width;
}

void DrawSettingsText(ID2D1DeviceContext* dc, const std::wstring& text,
                      const RECT& rect, uint32_t color,
                      const LayoutMetrics& metrics,
                      DWRITE_TEXT_ALIGNMENT alignment) {
    IDWriteFactory* dwrite = g_renderDevice.DWriteFactory();
    if (!dwrite || text.empty() || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return;
    }
    IDWriteTextFormat* format = nullptr;
    if (FAILED(dwrite->CreateTextFormat(
            metrics.fontFace.c_str(), nullptr,
            FontWeightToDwrite(metrics.fontWeight),
            FontStyleToDwrite(metrics.fontStyle), DWRITE_FONT_STRETCH_NORMAL,
            metrics.fontSize, L"", &format)) ||
        !format) {
        return;
    }
    format->SetTextAlignment(alignment);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    IDWriteTextLayout* layout = nullptr;
    if (SUCCEEDED(dwrite->CreateTextLayout(
            text.c_str(), static_cast<UINT32>(text.size()), format,
            static_cast<float>(rect.right - rect.left),
            static_cast<float>(rect.bottom - rect.top), &layout)) &&
        layout) {
        ID2D1SolidColorBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) &&
            brush) {
            dc->DrawTextLayout(
                D2D1::Point2F(static_cast<float>(rect.left),
                              static_cast<float>(rect.top)),
                layout, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            brush->Release();
        }
        layout->Release();
    }
    format->Release();
}

void DrawSettingsCheckerboard(ID2D1DeviceContext* dc, const D2D1_RECT_F& rect,
                              float cell) {
    ID2D1SolidColorBrush* light = nullptr;
    ID2D1SolidColorBrush* dark = nullptr;
    if (FAILED(dc->CreateSolidColorBrush(D2D1::ColorF(0.78f, 0.78f, 0.78f),
                                         &light)) ||
        !light) {
        return;
    }
    if (FAILED(dc->CreateSolidColorBrush(D2D1::ColorF(0.55f, 0.55f, 0.55f),
                                         &dark)) ||
        !dark) {
        light->Release();
        return;
    }
    int row = 0;
    for (float y = rect.top; y < rect.bottom; y += cell, ++row) {
        int col = 0;
        for (float x = rect.left; x < rect.right; x += cell, ++col) {
            const D2D1_RECT_F cellRect = {
                x, y, std::min(x + cell, rect.right),
                std::min(y + cell, rect.bottom)};
            dc->FillRectangle(cellRect,
                              ((row + col) % 2) != 0 ? dark : light);
        }
    }
    light->Release();
    dark->Release();
}

void DrawSettingsToggle(ID2D1DeviceContext* dc, const LayoutItem& item,
                        const LayoutMetrics& metrics,
                        const Appearance& appearance) {
    const bool on = item.controlValue != 0;
    const float height = 18.0f;
    const float width = 36.0f;
    const float left = static_cast<float>(item.controlRect.left);
    const float top =
        (static_cast<float>(item.rect.top) + static_cast<float>(item.rect.bottom)) /
            2.0f -
        height / 2.0f;
    const D2D1_ROUNDED_RECT pill = {
        D2D1::RectF(left, top, left + width, top + height), height / 2.0f,
        height / 2.0f};
    ID2D1SolidColorBrush* brush = nullptr;
    if (on) {
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.textColor),
                                                &brush)) &&
            brush) {
            dc->FillRoundedRectangle(pill, brush);
            brush->Release();
        }
    } else if (SUCCEEDED(dc->CreateSolidColorBrush(
                   ColorFromArgb(appearance.border), &brush)) &&
               brush) {
        dc->DrawRoundedRectangle(pill, brush, 1.0f);
        brush->Release();
    }
    const float knobX = on ? left + width - height / 2.0f : left + height / 2.0f;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.background),
                                            &brush)) &&
        brush) {
        dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(knobX, top + height / 2.0f),
                                      6.0f, 6.0f),
                        brush);
        brush->Release();
    }
}

void DrawSettingsField(ID2D1DeviceContext* dc, const LayoutItem& item,
                       const MenuInputState& state,
                       const LayoutMetrics& metrics,
                       const Appearance& appearance) {
    if (item.fieldRect.right <= item.fieldRect.left) {
        return;
    }
    const bool focused = !state.focusedControl.empty() &&
                         state.focusedControl == item.control.key;
    ID2D1SolidColorBrush* brush = nullptr;
    const D2D1_ROUNDED_RECT field = {RectF(item.fieldRect), 4.0f, 4.0f};
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.hoverBackground),
                                            &brush)) &&
        brush) {
        dc->FillRoundedRectangle(field, brush);
        brush->Release();
    }
    if (focused &&
        SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.border),
                                            &brush)) &&
        brush) {
        dc->DrawRoundedRectangle(field, brush, 1.0f);
        brush->Release();
    }
    std::wstring text = focused ? state.editBuffer : item.controlText;
    if (!focused && state.dragControl == item.control.key) {
        text = std::to_wstring(state.dragValue);
    }
    DrawSettingsText(dc, text, item.fieldRect, metrics.textColor, metrics,
                     DWRITE_TEXT_ALIGNMENT_TRAILING);
    if (focused && state.caretVisible) {
        const std::wstring prefix =
            state.editBuffer.substr(0, std::min(state.caretPos,
                                                state.editBuffer.size()));
        const float fullWidth = SettingsTextWidth(text, metrics);
        const float prefixWidth = SettingsTextWidth(prefix, metrics);
        // The field text is right-aligned to the field's right edge, so the
        // caret needs no extra inset; six pixels here was about one digit.
        const float caretX =
            static_cast<float>(item.fieldRect.right) - fullWidth + prefixWidth;
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.textColor),
                                                &brush)) &&
            brush) {
            dc->DrawLine(
                D2D1::Point2F(caretX,
                              static_cast<float>(item.fieldRect.top) + 3.0f),
                D2D1::Point2F(caretX,
                              static_cast<float>(item.fieldRect.bottom) - 3.0f),
                brush, 1.0f);
            brush->Release();
        }
    }
}

void DrawSettingsSlider(ID2D1DeviceContext* dc, const LayoutItem& item,
                        const MenuInputState& state,
                        const LayoutMetrics& metrics,
                        const Appearance& appearance) {
    ID2D1SolidColorBrush* brush = nullptr;
    const float trackY =
        (static_cast<float>(item.trackRect.top) +
         static_cast<float>(item.trackRect.bottom)) /
        2.0f;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.submenuArrow),
                                            &brush)) &&
        brush) {
        dc->DrawLine(
            D2D1::Point2F(static_cast<float>(item.trackRect.left), trackY),
            D2D1::Point2F(static_cast<float>(item.trackRect.right), trackY),
            brush, 3.0f);
        brush->Release();
    }
    const int trackWidth = item.trackRect.right - item.trackRect.left;
    const int shownValue = state.dragControl == item.control.key
                               ? state.dragValue
                               : item.controlValue;
    const int thumbX =
        SliderXFromValue(shownValue, item.trackRect.left, trackWidth,
                         item.control.minValue, item.control.maxValue);
    const float centerY =
        (static_cast<float>(item.rect.top) + static_cast<float>(item.rect.bottom)) /
        2.0f;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.textColor),
                                            &brush)) &&
        brush) {
        dc->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(static_cast<float>(thumbX), centerY),
                          6.0f, 6.0f),
            brush);
        brush->Release();
    }
    DrawSettingsField(dc, item, state, metrics, appearance);
}

void DrawSettingsEnum(ID2D1DeviceContext* dc, const LayoutItem& item,
                      const LayoutMetrics& metrics) {
    // The row's submenu chevron is drawn by the shared submenu pass; only the
    // current value goes here.
    DrawSettingsText(dc, item.controlText, item.controlRect, metrics.textColor,
                     metrics, DWRITE_TEXT_ALIGNMENT_TRAILING);
}

void DrawSettingsSwatch(ID2D1DeviceContext* dc, const LayoutItem& item,
                        const LayoutMetrics& metrics,
                        const Appearance& appearance) {
    const D2D1_RECT_F rect = RectF(item.swatchRect);
    if ((item.controlColor >> 24) != 0xFF) {
        DrawSettingsCheckerboard(dc, rect, 8.0f);
    }
    ID2D1SolidColorBrush* brush = nullptr;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(item.controlColor),
                                            &brush)) &&
        brush) {
        dc->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brush);
        brush->Release();
    }
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.border),
                                            &brush)) &&
        brush) {
        dc->DrawRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brush,
                                 1.0f);
        brush->Release();
    }
}

void DrawSettingsColorArea(ID2D1DeviceContext* dc, const LayoutItem& item,
                           const LayoutMetrics& metrics,
                           const Appearance& appearance) {
    const D2D1_RECT_F rect = RectF(item.areaRect);
    const int width = item.areaRect.right - item.areaRect.left;
    const int height = item.areaRect.bottom - item.areaRect.top;
    if (width <= 0 || height <= 0 || !item.resources) {
        return;
    }
    const HsvColor hsv = RgbToHsv(item.controlColor);
    LayoutItemResources* resources = item.resources.get();
    if (!resources->svSquare || resources->svHue != hsv.h) {
        const std::vector<uint8_t> pixels =
            BuildSvSquarePixels(hsv.h, width, height);
        ID2D1Bitmap* bitmap = nullptr;
        const D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
        if (!pixels.empty() &&
            SUCCEEDED(dc->CreateBitmap(
                D2D1::SizeU(static_cast<UINT32>(width),
                            static_cast<UINT32>(height)),
                pixels.data(), static_cast<UINT32>(width * 4), props,
                &bitmap)) &&
            bitmap) {
            if (resources->svSquare) {
                resources->svSquare->Release();
            }
            resources->svSquare = bitmap;
            resources->svHue = hsv.h;
        }
    }
    if (resources->svSquare) {
        dc->DrawBitmap(resources->svSquare, rect, 1.0f,
                       D2D1_INTERPOLATION_MODE_LINEAR);
    }
    ID2D1SolidColorBrush* brush = nullptr;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.border),
                                            &brush)) &&
        brush) {
        dc->DrawRectangle(rect, brush, 1.0f);
        brush->Release();
    }
    const float markerX =
        static_cast<float>(item.areaRect.left) +
        hsv.s * static_cast<float>(width);
    const float markerY =
        static_cast<float>(item.areaRect.top) +
        (1.0f - hsv.v) * static_cast<float>(height);
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(0xFFFFFFFF), &brush)) &&
        brush) {
        dc->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(markerX, markerY), 5.0f,
                                      5.0f),
                        brush, 2.0f);
        brush->Release();
    }
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(0xFF000000), &brush)) &&
        brush) {
        dc->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(markerX, markerY), 6.5f,
                                      6.5f),
                        brush, 1.0f);
        brush->Release();
    }
}

void DrawSettingsStrip(ID2D1DeviceContext* dc, const LayoutItem& item,
                       const LayoutMetrics& metrics,
                       const Appearance& appearance, bool alpha) {
    const D2D1_RECT_F rect = RectF(item.stripRect);
    if (alpha) {
        DrawSettingsCheckerboard(dc, rect, 8.0f);
    }
    D2D1_GRADIENT_STOP stops[7] = {};
    UINT32 stopCount = 0;
    if (alpha) {
        stopCount = 2;
        stops[0] = {0.0f, ColorFromArgb(item.controlColor & 0x00FFFFFF)};
        stops[1] = {1.0f, ColorFromArgb(item.controlColor | 0xFF000000)};
    } else {
        stopCount = 7;
        stops[0] = {0.0f, ColorFromArgb(0xFFFF0000)};
        stops[1] = {1.0f / 6.0f, ColorFromArgb(0xFFFFFF00)};
        stops[2] = {2.0f / 6.0f, ColorFromArgb(0xFF00FF00)};
        stops[3] = {3.0f / 6.0f, ColorFromArgb(0xFF00FFFF)};
        stops[4] = {4.0f / 6.0f, ColorFromArgb(0xFF0000FF)};
        stops[5] = {5.0f / 6.0f, ColorFromArgb(0xFFFF00FF)};
        stops[6] = {1.0f, ColorFromArgb(0xFFFF0000)};
    }
    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(dc->CreateGradientStopCollection(stops, stopCount, &collection)) ||
        !collection) {
        return;
    }
    ID2D1LinearGradientBrush* brush = nullptr;
    if (SUCCEEDED(dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(rect.left, rect.top),
                D2D1::Point2F(rect.right, rect.top)),
            collection, &brush)) &&
        brush) {
        dc->FillRectangle(rect, brush);
        brush->Release();
    }
    collection->Release();

    const HsvColor hsv = RgbToHsv(item.controlColor);
    const float markerX =
        alpha
            ? rect.left + static_cast<float>((item.controlColor >> 24) & 0xFF) /
                              255.0f * (rect.right - rect.left)
            : rect.left + hsv.h / 360.0f * (rect.right - rect.left);
    ID2D1SolidColorBrush* marker = nullptr;
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(0xFFFFFFFF), &marker)) &&
        marker) {
        dc->DrawLine(D2D1::Point2F(markerX, rect.top - 1.0f),
                     D2D1::Point2F(markerX, rect.bottom + 1.0f), marker, 2.0f);
        marker->Release();
    }
    if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(0xFF000000), &marker)) &&
        marker) {
        dc->DrawRectangle(rect, marker, 1.0f);
        marker->Release();
    }
}

void DrawSettingsInfo(ID2D1DeviceContext* dc, const LayoutItem& item,
                      const LayoutMetrics& metrics) {
    DrawSettingsText(dc, item.controlText, item.controlRect,
                     metrics.disabledTextColor, metrics,
                     DWRITE_TEXT_ALIGNMENT_TRAILING);
}

// --- Menu overlays ---------------------------------------------------------

// The effect CLSIDs are not exported by MinGW's d2d1 import library; the
// values match d2d1effects.h.
const GUID kClsidD2D1ColorMatrix = {
    0x921f03d6, 0x641c, 0x47df,
    {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};
const GUID kClsidD2D1Turbulence = {
    0xcf2bb6ae, 0x889a, 0x4ad7,
    {0xba, 0x29, 0xa2, 0xfd, 0x73, 0x2c, 0x9f, 0xc9}};

struct OverlayResources {
    ID2D1Device* device = nullptr;
    ID2D1Effect* turbulence = nullptr;
    ID2D1Effect* colorMatrix = nullptr;
    ID2D1Bitmap* scanlineBitmap = nullptr;
    ID2D1BitmapBrush1* scanlineBrush = nullptr;

    void Release() {
        if (scanlineBrush) {
            scanlineBrush->Release();
            scanlineBrush = nullptr;
        }
        if (scanlineBitmap) {
            scanlineBitmap->Release();
            scanlineBitmap = nullptr;
        }
        if (colorMatrix) {
            colorMatrix->Release();
            colorMatrix = nullptr;
        }
        if (turbulence) {
            turbulence->Release();
            turbulence = nullptr;
        }
        device = nullptr;
    }
};

inline OverlayResources g_overlayResources;

void ReleaseOverlayResources() {
    g_overlayResources.Release();
}

bool EnsureOverlayResources(ID2D1DeviceContext* dc) {
    ID2D1Device* device = g_renderDevice.D2DDevice();
    if (!device) {
        return false;
    }
    if (g_overlayResources.device == device) {
        return true;
    }
    g_overlayResources.Release();
    if (FAILED(dc->CreateEffect(kClsidD2D1Turbulence,
                                &g_overlayResources.turbulence)) ||
        !g_overlayResources.turbulence) {
        g_overlayResources.Release();
        return false;
    }
    if (FAILED(dc->CreateEffect(kClsidD2D1ColorMatrix,
                                &g_overlayResources.colorMatrix)) ||
        !g_overlayResources.colorMatrix) {
        g_overlayResources.Release();
        return false;
    }
    // 1x4 scanline profile: a one-pixel core with a feathered edge on each
    // side. The soft edges let the lines scroll subpixel-smoothly under linear
    // interpolation; a hard two-pixel-period pattern can only toggle. This is
    // one notch finer than the previous profile at the default size.
    const uint32_t scanlinePixels[4] = {0x80000000u, 0x40000000u, 0x00000000u,
                                        0x40000000u};
    const D2D1_BITMAP_PROPERTIES props = {
        {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED}, 96.0f,
        96.0f};
    if (FAILED(dc->CreateBitmap(D2D1::SizeU(1, 4), scanlinePixels, 4, props,
                                &g_overlayResources.scanlineBitmap)) ||
        !g_overlayResources.scanlineBitmap) {
        g_overlayResources.Release();
        return false;
    }
    D2D1_BITMAP_BRUSH_PROPERTIES1 brushProps = {};
    brushProps.extendModeX = D2D1_EXTEND_MODE_WRAP;
    brushProps.extendModeY = D2D1_EXTEND_MODE_WRAP;
    // Linear interpolation is what makes subpixel scrolling smooth; the soft
    // profile keeps the lines from smearing into gradients.
    brushProps.interpolationMode = D2D1_INTERPOLATION_MODE_LINEAR;
    if (FAILED(dc->CreateBitmapBrush(g_overlayResources.scanlineBitmap,
                                     &brushProps, nullptr,
                                     &g_overlayResources.scanlineBrush)) ||
        !g_overlayResources.scanlineBrush) {
        g_overlayResources.Release();
        return false;
    }
    g_overlayResources.device = device;
    return true;
}

// Overlay size as a multiplier: 1.0 at the default 100%.
float OverlaySizeFactor(const Appearance& appearance) {
    return static_cast<float>(std::clamp(appearance.overlaySize, 25, 400)) /
           100.0f;
}

// Scanlines get a finer ladder: the base profile is small and the scale can
// drop to half size, one step below what the other effects allow.
float ScanlineScale(const Appearance& appearance) {
    return std::clamp(OverlaySizeFactor(appearance), 0.5f, 4.0f);
}

void DrawOverlay(ID2D1DeviceContext* dc, const LayoutPanel& panel,
                 const Appearance& appearance, float overlayTime,
                 ID2D1PathGeometry* panelGeometry) {
    if (appearance.overlay == 0) {
        return;
    }
    const float intensity = static_cast<float>(
                                std::clamp(appearance.overlayIntensity, 0, 100)) /
                            100.0f;
    if (intensity <= 0.0f) {
        return;
    }
    const float speed =
        static_cast<float>(std::clamp(appearance.overlaySpeed, 0, 200)) /
        100.0f;
    const float t = overlayTime * speed;
    const D2D1_RECT_F rect =
        D2D1::RectF(0.0f, 0.0f, static_cast<float>(panel.size.cx),
                    static_cast<float>(panel.size.cy));
    const float diagonal =
        std::sqrt((rect.right - rect.left) * (rect.right - rect.left) +
                  (rect.bottom - rect.top) * (rect.bottom - rect.top));

    // The clip must follow the panel's rounding. DrawPanel only builds the
    // geometry for per-corner radii, so the plain cornerRadius case builds one
    // here; otherwise the overlay spills past the rounded corners.
    ID2D1PathGeometry* maskGeometry = panelGeometry;
    if (!maskGeometry &&
        (appearance.hasCornerRadii || appearance.cornerRadius > 0)) {
        CornerRadii radii;
        if (appearance.hasCornerRadii) {
            radii = appearance.cornerRadii;
        } else {
            radii.topLeft = radii.topRight = radii.bottomRight =
                radii.bottomLeft = appearance.cornerRadius;
        }
        maskGeometry = BuildPanelGeometry(g_renderDevice.D2DFactory(),
                                          static_cast<float>(panel.size.cx),
                                          static_cast<float>(panel.size.cy),
                                          radii);
    }

    if (maskGeometry) {
        D2D1_LAYER_PARAMETERS1 layer = {};
        layer.contentBounds = D2D1::InfiniteRect();
        layer.geometricMask = maskGeometry;
        layer.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        layer.maskTransform = D2D1::IdentityMatrix();
        layer.opacity = 1.0f;
        layer.layerOptions = D2D1_LAYER_OPTIONS1_NONE;
        dc->PushLayer(layer, nullptr);
    } else {
        dc->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    // Hue drift: a soft gradient whose hues cycle; the size factor sets how
    // many color bands span the panel.
    if ((appearance.overlay & kOverlayHue) != 0) {
        const float hue = std::fmod(t * 40.0f, 360.0f);
        const uint8_t alpha =
            static_cast<uint8_t>(std::lround(intensity * 90.0f));
        const int bands = std::clamp(
            static_cast<int>(std::lround(OverlaySizeFactor(appearance))), 1, 8);
        std::vector<D2D1_GRADIENT_STOP> stops(
            static_cast<size_t>(bands) + 1);
        for (int i = 0; i <= bands; ++i) {
            stops[static_cast<size_t>(i)] = {
                static_cast<float>(i) / static_cast<float>(bands),
                ColorFromArgb(HsvToRgb(
                    HsvColor{std::fmod(hue + i * 140.0f, 360.0f), 0.7f, 1.0f},
                    alpha))};
        }
        ID2D1GradientStopCollection* collection = nullptr;
        ID2D1LinearGradientBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateGradientStopCollection(
                stops.data(), static_cast<UINT32>(stops.size()),
                &collection)) &&
            collection &&
            SUCCEEDED(dc->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(rect.left, rect.top),
                    D2D1::Point2F(rect.right, rect.bottom)),
                collection, &brush)) &&
            brush) {
            dc->FillRectangle(rect, brush);
            brush->Release();
        }
        if (collection) {
            collection->Release();
        }
    }

    // Noise / plasma: turbulence through a color matrix (gray grain, or a
    // hue-rotating rainbow).
    const bool wantsNoise = (appearance.overlay & kOverlayNoise) != 0;
    const bool wantsPlasma = (appearance.overlay & kOverlayPlasma) != 0;
    if ((wantsNoise || wantsPlasma) && EnsureOverlayResources(dc) &&
        g_overlayResources.turbulence && g_overlayResources.colorMatrix) {
        const float frequency =
            std::clamp((wantsPlasma ? 0.02f : 0.08f) /
                           OverlaySizeFactor(appearance),
                       0.001f, 1.0f);
        g_overlayResources.turbulence->SetValue(
            D2D1_TURBULENCE_PROP_SIZE,
            D2D1::Vector2F(rect.right - rect.left, rect.bottom - rect.top));
        g_overlayResources.turbulence->SetValue(
            D2D1_TURBULENCE_PROP_BASE_FREQUENCY,
            D2D1::Vector2F(frequency, frequency));
        g_overlayResources.turbulence->SetValue(
            D2D1_TURBULENCE_PROP_NUM_OCTAVES, 2u);
        g_overlayResources.turbulence->SetValue(
            D2D1_TURBULENCE_PROP_SEED,
            static_cast<UINT32>(std::fmod(t * 60.0f, 1000.0f)));
        D2D1_MATRIX_5X4_F matrix = {};
        if (wantsPlasma) {
            const float angle =
                std::fmod(t * 60.0f, 360.0f) * 3.14159265f / 180.0f;
            const float c = std::cos(angle);
            const float s2 = std::sin(angle);
            const float w = intensity;
            matrix = D2D1::Matrix5x4F(
                w * (0.213f + 0.787f * c - 0.213f * s2),
                w * (0.715f - 0.715f * c - 0.715f * s2),
                w * (0.072f - 0.072f * c + 0.928f * s2), 0, 
                w * (0.213f - 0.213f * c + 0.143f * s2),
                w * (0.715f + 0.285f * c + 0.140f * s2),
                w * (0.072f - 0.072f * c - 0.283f * s2), 0, 
                w * (0.213f - 0.213f * c - 0.787f * s2),
                w * (0.715f - 0.715f * c + 0.715f * s2),
                w * (0.072f + 0.928f * c + 0.072f * s2), 0, 
                0, 0, 0, 0, 
                0, 0, 0, w);
        } else {
            const float w = intensity * 0.35f / 3.0f;
            matrix = D2D1::Matrix5x4F(
                w, w, w, 0, 
                w, w, w, 0, 
                w, w, w, 0, 
                0, 0, 0, 0, 
                0, 0, 0, intensity * 0.35f);
        }
        g_overlayResources.colorMatrix->SetValue(
            D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix);
        ID2D1Image* turbulenceOutput = nullptr;
        g_overlayResources.turbulence->GetOutput(&turbulenceOutput);
        g_overlayResources.colorMatrix->SetInput(0, turbulenceOutput);
        if (turbulenceOutput) {
            turbulenceOutput->Release();
        }
        ID2D1Image* overlayOutput = nullptr;
        g_overlayResources.colorMatrix->GetOutput(&overlayOutput);
        if (overlayOutput) {
            dc->DrawImage(overlayOutput, D2D1_INTERPOLATION_MODE_LINEAR,
                          D2D1_COMPOSITE_MODE_SOURCE_OVER);
            overlayOutput->Release();
        }
    }

    // Glow pulse: a soft additive radial bloom from the panel center.
    if ((appearance.overlay & kOverlayGlow) != 0) {
        const float cx = (rect.left + rect.right) / 2.0f;
        const float cy = (rect.top + rect.bottom) / 2.0f;
        const float pulse = 0.75f + 0.25f * std::sin(t * 3.0f);
        const float glowRadius = std::min(
            diagonal * 0.5f * pulse * OverlaySizeFactor(appearance), diagonal);
        const uint32_t innerAlpha =
            static_cast<uint32_t>(std::lround(intensity * 70.0f));
        const D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, ColorFromArgb((innerAlpha << 24) | 0x00FFFFFFu)},
            {1.0f, ColorFromArgb(0x00FFFFFFu)}};
        ID2D1GradientStopCollection* collection = nullptr;
        ID2D1RadialGradientBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateGradientStopCollection(stops, 2, &collection)) &&
            collection &&
            SUCCEEDED(dc->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(cx, cy), D2D1::Point2F(0, 0), glowRadius,
                    glowRadius),
                collection, &brush)) &&
            brush) {
            dc->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
            dc->FillRectangle(rect, brush);
            dc->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
            brush->Release();
        }
        if (collection) {
            collection->Release();
        }
    }

    // Scanlines: a tiled 1x4 pattern, scrolling slowly.
    if ((appearance.overlay & kOverlayScanlines) != 0 &&
        EnsureOverlayResources(dc) && g_overlayResources.scanlineBrush) {
        // A slow, even drift: 10 px/s at normal speed, scrolling in subpixel
        // steps. The size factor scales both line thickness and spacing, down
        // to half size.
        const float lineScale = ScanlineScale(appearance);
        const float period = 4.0f * lineScale;
        const float offset = std::fmod(t * 10.0f, period);
        g_overlayResources.scanlineBrush->SetTransform(
            D2D1::Matrix3x2F::Scale(1.0f, lineScale) *
            D2D1::Matrix3x2F::Translation(0.0f, offset));
        g_overlayResources.scanlineBrush->SetOpacity(intensity);
        dc->FillRectangle(rect, g_overlayResources.scanlineBrush);
    }

    // Vignette: darkened edges, breathing slightly.
    if ((appearance.overlay & kOverlayVignette) != 0) {
        const float cx = (rect.left + rect.right) / 2.0f;
        const float cy = (rect.top + rect.bottom) / 2.0f;
        const float breathe = 0.9f + 0.1f * std::sin(t * 1.5f);
        const uint8_t edgeAlpha =
            static_cast<uint8_t>(std::lround(intensity * 180.0f));
        // A larger size pushes the clear center outward, so the dark edge
        // reaches further in.
        const float innerStop = std::clamp(
            0.55f / OverlaySizeFactor(appearance), 0.1f, 0.9f);
        const D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, ColorFromArgb(0x00000000u)},
            {innerStop, ColorFromArgb(0x00000000u)},
            {1.0f, ColorFromArgb(static_cast<uint32_t>(edgeAlpha) << 24)}};
        ID2D1GradientStopCollection* collection = nullptr;
        ID2D1RadialGradientBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateGradientStopCollection(stops, 3, &collection)) &&
            collection &&
            SUCCEEDED(dc->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(cx, cy), D2D1::Point2F(0, 0),
                    diagonal * 0.5f * breathe, diagonal * 0.5f * breathe),
                collection, &brush)) &&
            brush) {
            dc->FillRectangle(rect, brush);
            brush->Release();
        }
        if (collection) {
            collection->Release();
        }
    }

    if (maskGeometry) {
        dc->PopLayer();
    } else {
        dc->PopAxisAlignedClip();
    }
    if (maskGeometry && maskGeometry != panelGeometry) {
        maskGeometry->Release();
    }
}

void DrawPanel(ID2D1DeviceContext* dc, const LayoutPanel& panel,
               const MenuInputState& state, const LayoutMetrics& metrics,
               const Appearance& appearance, const BackdropBitmap* backdrop,
               int margin = 0, const RECT* shadowClipRect = nullptr,
               const AnimationFrame& frame = AnimationFrame{},
               POINT anchor = POINT{0, 0}, float overlayTime = 0.0f) {
    if (!dc) {
        return;
    }

    // Animation frames are drawn by the UI thread (DirectComposition
    // animations are not evaluated for this target): the transform carries
    // slide/scale, the outer layer the panel opacity, and the content layer
    // the dissolve opacity.
    if (margin > 0 || frame.translateX != 0.0f || frame.translateY != 0.0f ||
        frame.scaleX != 1.0f || frame.scaleY != 1.0f) {
        const float anchorX = frame.center
                                  ? static_cast<float>(panel.size.cx) / 2.0f
                                  : static_cast<float>(anchor.x);
        const float anchorY = frame.center
                                  ? static_cast<float>(panel.size.cy) / 2.0f
                                  : static_cast<float>(anchor.y);
        const D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(
            frame.scaleX, frame.scaleY, D2D1::Point2F(anchorX, anchorY));
        const D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Translation(
            static_cast<float>(margin) + frame.translateX,
            static_cast<float>(margin) + frame.translateY);
        const D2D1_MATRIX_3X2_F transform = scale * translate;
        dc->SetTransform(&transform);
    }

    const bool fading = frame.opacity < 0.999f;
    if (fading) {
        D2D1_LAYER_PARAMETERS1 layer = {};
        layer.contentBounds = D2D1::InfiniteRect();
        layer.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        layer.maskTransform = D2D1::IdentityMatrix();
        layer.opacity = frame.opacity;
        layer.layerOptions = D2D1_LAYER_OPTIONS1_NONE;
        dc->PushLayer(layer, nullptr);
    }

    const D2D1_RECT_F rect = {0.0f, 0.0f, static_cast<float>(panel.size.cx),
                              static_cast<float>(panel.size.cy)};
    const float radius = static_cast<float>(metrics.cornerRadius);
    const D2D1_ROUNDED_RECT rounded = {rect, radius, radius};

    // Blurred drop shadow drawn in the margin outside the panel.
    if (appearance.shadow && margin > 0 &&
        (metrics.shadowSize > 0 || metrics.shadowBlur > 0)) {
        const int spread = metrics.shadowSize;
        const int blur = metrics.shadowBlur;
        CornerRadii radii;
        if (metrics.hasCornerRadii) {
            radii = metrics.cornerRadii;
        } else {
            radii.topLeft = radii.topRight = radii.bottomRight =
                radii.bottomLeft = metrics.cornerRadius;
        }
        int shadowOpacity = metrics.shadowOpacity;
        uint32_t shadowColor = metrics.shadowColor;
        if (appearance.shadowAdaptive && backdrop && backdrop->width > 0) {
            AdaptShadowToBackdrop(backdrop->averageColor,
                                  metrics.shadowOpacity, metrics.shadowColor,
                                  shadowOpacity, shadowColor);
        }
        std::vector<uint32_t> shadowPixels;
        int shadowW = 0;
        int shadowH = 0;
        if (GetCachedShadowBitmap(panel.size.cx, panel.size.cy, radii, spread,
                                  blur, shadowOpacity, shadowColor, 4,
                                  shadowPixels, shadowW, shadowH)) {
            const float maskW = static_cast<float>(
                panel.size.cx + 2 * spread + 2 * blur);
            const float maskH = static_cast<float>(
                panel.size.cy + 2 * spread + 2 * blur);
            // DrawPanel runs under a translate(margin, margin) transform,
            // so these are panel-relative coordinates.
            const float left =
                static_cast<float>(-spread - blur + metrics.shadowOffsetX);
            const float top =
                static_cast<float>(-spread - blur + metrics.shadowOffsetY);
            if (shadowClipRect && shadowClipRect->right > shadowClipRect->left &&
                shadowClipRect->bottom > shadowClipRect->top && shadowW > 0 &&
                shadowH > 0) {
                // Zero only the part of the shadow that overlaps the parent
                // menu's own bounds, so it never spills onto the parent while
                // the rest of the shadow (including below and above the parent)
                // still renders normally. A small feather keeps the transition
                // at the parent's edges from showing as a hard line.
                constexpr float kFeather = 3.0f;
                const float scaleX = maskW / static_cast<float>(shadowW);
                const float scaleY = maskH / static_cast<float>(shadowH);
                for (int y = 0; y < shadowH; ++y) {
                    const float py =
                        top + (static_cast<float>(y) + 0.5f) * scaleY;
                    const float dy = std::max(
                        std::max(static_cast<float>(shadowClipRect->top) - py,
                                 py - static_cast<float>(shadowClipRect->bottom)),
                        0.0f);
                    if (dy >= kFeather) {
                        continue;
                    }
                    uint32_t* row =
                        &shadowPixels[static_cast<size_t>(y) * shadowW];
                    for (int x = 0; x < shadowW; ++x) {
                        const float px =
                            left + (static_cast<float>(x) + 0.5f) * scaleX;
                        const float dx = std::max(
                            std::max(static_cast<float>(shadowClipRect->left) - px,
                                     px - static_cast<float>(shadowClipRect->right)),
                            0.0f);
                        const float distance = std::max(dx, dy);
                        if (distance >= kFeather) {
                            continue;
                        }
                        uint32_t pixel = row[x];
                        if (distance <= 0.0f) {
                            row[x] = 0;
                            continue;
                        }
                        const float coverage = distance / kFeather;
                        const uint32_t a = static_cast<uint32_t>(
                            static_cast<float>((pixel >> 24) & 0xFF) * coverage);
                        const uint32_t r = static_cast<uint32_t>(
                            static_cast<float>((pixel >> 16) & 0xFF) * coverage);
                        const uint32_t g = static_cast<uint32_t>(
                            static_cast<float>((pixel >> 8) & 0xFF) * coverage);
                        const uint32_t b = static_cast<uint32_t>(
                            static_cast<float>(pixel & 0xFF) * coverage);
                        row[x] = (a << 24) | (r << 16) | (g << 8) | b;
                    }
                }
            }
            ID2D1Bitmap* bitmap = nullptr;
            const D2D1_SIZE_U size = {static_cast<UINT32>(shadowW),
                                      static_cast<UINT32>(shadowH)};
            const D2D1_BITMAP_PROPERTIES props = {
                {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED}, 96.0f,
                96.0f};
            if (SUCCEEDED(dc->CreateBitmap(
                    size, shadowPixels.data(),
                    static_cast<UINT32>(shadowW * sizeof(uint32_t)), props,
                    &bitmap)) &&
                bitmap) {
                const D2D1_RECT_F dest = {left, top, left + maskW, top + maskH};
                dc->DrawBitmap(bitmap, dest, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR);
                bitmap->Release();
            }
        }
    }

    if (appearance.blur && appearance.blurStrength > 0 && backdrop &&
        backdrop->width > 0 && backdrop->height > 0 &&
        !backdrop->pixels.empty()) {
        const float scaleX = static_cast<float>(backdrop->width) /
                             static_cast<float>(std::max(1L, panel.size.cx));
        const int maskRadius = static_cast<int>(
            static_cast<float>(metrics.cornerRadius) * scaleX);
        std::vector<uint8_t> mask;
        if (metrics.hasCornerRadii) {
            BuildRoundedRectMaskRadii(
                backdrop->width, backdrop->height,
                static_cast<int>(metrics.cornerRadii.topLeft * scaleX),
                static_cast<int>(metrics.cornerRadii.topRight * scaleX),
                static_cast<int>(metrics.cornerRadii.bottomRight * scaleX),
                static_cast<int>(metrics.cornerRadii.bottomLeft * scaleX), mask);
        } else {
            BuildRoundedRectMask(backdrop->width, backdrop->height, maskRadius, mask);
        }
        std::vector<uint32_t> pixels = backdrop->pixels;
        for (size_t i = 0; i < pixels.size() && i < mask.size(); ++i) {
            const uint32_t alpha = (((pixels[i] >> 24) & 0xFF) * mask[i]) / 255;
            const uint32_t r = ((((pixels[i] >> 16) & 0xFF) * alpha) / 255) << 16;
            const uint32_t g = ((((pixels[i] >> 8) & 0xFF) * alpha) / 255) << 8;
            const uint32_t b = ((pixels[i] & 0xFF) * alpha) / 255;
            pixels[i] = (alpha << 24) | r | g | b;
        }

        ID2D1Bitmap* bitmap = nullptr;
        const D2D1_SIZE_U size = {static_cast<UINT32>(backdrop->width),
                                  static_cast<UINT32>(backdrop->height)};
        const D2D1_BITMAP_PROPERTIES props = {
            {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED}, 96.0f,
            96.0f};
        if (SUCCEEDED(dc->CreateBitmap(
                size, pixels.data(),
                static_cast<UINT32>(backdrop->width * sizeof(uint32_t)), props,
                &bitmap)) &&
            bitmap) {
            dc->DrawBitmap(bitmap, rect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR);
            bitmap->Release();
        }
    }

    ID2D1PathGeometry* panelGeometry = nullptr;
    if (metrics.hasCornerRadii) {
        panelGeometry = BuildPanelGeometry(g_renderDevice.D2DFactory(),
                                           static_cast<float>(panel.size.cx),
                                           static_cast<float>(panel.size.cy),
                                           metrics.cornerRadii);
    }

    if ((appearance.background >> 24) != 0) {
        ID2D1SolidColorBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.background),
                                                &brush)) &&
            brush) {
            if (panelGeometry) {
                dc->FillGeometry(panelGeometry, brush);
            } else {
                dc->FillRoundedRectangle(&rounded, brush);
            }
            brush->Release();
        }
    }

    const bool contentFading = frame.contentOpacity < 0.999f;
    if (contentFading) {
        D2D1_LAYER_PARAMETERS1 layer = {};
        layer.contentBounds = D2D1::InfiniteRect();
        layer.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        layer.maskTransform = D2D1::IdentityMatrix();
        layer.opacity = frame.contentOpacity;
        layer.layerOptions = D2D1_LAYER_OPTIONS1_NONE;
        dc->PushLayer(layer, nullptr);
    }
    for (size_t i = 0; i < panel.items.size(); ++i) {
        const LayoutItem& item = panel.items[i];
        const bool hovered = static_cast<int>(i) == state.hoverIndex ||
                             static_cast<int>(i) == state.keyboardIndex;

        if (item.kind == ItemKind::Separator) {
            ID2D1SolidColorBrush* brush = nullptr;
            if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(metrics.separator),
                                                    &brush)) &&
                brush) {
                const float y = (static_cast<float>(item.rect.top) +
                                 static_cast<float>(item.rect.bottom)) /
                                2.0f;
                const float inset =
                    static_cast<float>(metrics.padding + metrics.gutterWidth / 2);
                dc->DrawLine(D2D1_POINT_2F{inset, y},
                             D2D1_POINT_2F{static_cast<float>(panel.size.cx) - inset, y},
                             brush, 1.0f);
                brush->Release();
            }
            continue;
        }

        if (item.kind == ItemKind::Header) {
            ID2D1SolidColorBrush* headerBrush = nullptr;
            if (item.resources && item.resources->text &&
                SUCCEEDED(dc->CreateSolidColorBrush(
                    ColorFromArgb(metrics.headerColor), &headerBrush)) &&
                headerBrush) {
                dc->DrawTextLayout(
                    D2D1_POINT_2F{static_cast<float>(item.rect.left) +
                                      static_cast<float>(metrics.padding),
                                  static_cast<float>(item.textRect.top)},
                    item.resources->text, headerBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                headerBrush->Release();
            }
            continue;
        }

        if (hovered && (appearance.hoverBackground >> 24) != 0) {
            ID2D1SolidColorBrush* brush = nullptr;
            if (SUCCEEDED(dc->CreateSolidColorBrush(
                    ColorFromArgb(appearance.hoverBackground), &brush)) &&
                brush) {
                const float inset = static_cast<float>(metrics.itemPadding) / 2.0f;
                const D2D1_RECT_F hoverRect = {
                    static_cast<float>(item.rect.left) + inset,
                    static_cast<float>(item.rect.top) + 1.0f,
                    static_cast<float>(item.rect.right) - inset,
                    static_cast<float>(item.rect.bottom) - 1.0f};
                dc->FillRectangle(hoverRect, brush);
                brush->Release();
            }
        }

        if ((item.flags & kModelChecked) != 0 &&
            item.markerStyle != static_cast<int>(MarkerStyle::None)) {
            const MarkerStyle markerStyle =
                static_cast<MarkerStyle>(item.markerStyle);
            if (markerStyle == MarkerStyle::Dot) {
                DrawMarkerDot(dc, item.markerRect, metrics.markerColor);
            } else if (markerStyle == MarkerStyle::Bar) {
                DrawMarkerBar(dc, item.markerRect, metrics.markerColor);
            } else {
                DrawCheckmark(dc, item.markerRect, metrics.markerColor);
            }
        }

        if (item.resources && item.resources->glyph) {
            ID2D1SolidColorBrush* glyphBrush = nullptr;
            const uint32_t color = hovered ? item.hoverTextColor : item.textColor;
            if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(color),
                                                    &glyphBrush)) &&
                glyphBrush) {
                dc->DrawTextLayout(
                    D2D1_POINT_2F{static_cast<float>(item.iconRect.left),
                                  static_cast<float>(item.iconRect.top)},
                    item.resources->glyph, glyphBrush,
                    D2D1_DRAW_TEXT_OPTIONS_CLIP);
                glyphBrush->Release();
            }
        }

        if (item.resources && item.resources->icon) {
            const D2D1_RECT_F iconRect = {
                static_cast<float>(item.iconRect.left),
                static_cast<float>(item.iconRect.top),
                static_cast<float>(item.iconRect.right),
                static_cast<float>(item.iconRect.bottom)};
            dc->DrawBitmap(item.resources->icon, iconRect, 1.0f,
                           D2D1_INTERPOLATION_MODE_LINEAR);
        }

        if (item.resources && item.resources->text) {
            ID2D1SolidColorBrush* brush = nullptr;
            const uint32_t color = hovered ? item.hoverTextColor : item.textColor;
            if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(color), &brush)) &&
                brush) {
                dc->DrawTextLayout(
                    D2D1_POINT_2F{static_cast<float>(item.textRect.left),
                                  static_cast<float>(item.textRect.top)},
                    item.resources->text, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                brush->Release();
            }
        }

        if (item.control.kind != ControlKind::None) {
            switch (item.control.kind) {
                case ControlKind::Toggle:
                    DrawSettingsToggle(dc, item, metrics, appearance);
                    break;
                case ControlKind::IntSlider:
                    DrawSettingsSlider(dc, item, state, metrics, appearance);
                    break;
                case ControlKind::Enum:
                    DrawSettingsEnum(dc, item, metrics);
                    break;
                case ControlKind::ColorSwatch:
                    DrawSettingsSwatch(dc, item, metrics, appearance);
                    break;
                case ControlKind::TextField:
                case ControlKind::TextInput:
                    DrawSettingsField(dc, item, state, metrics, appearance);
                    break;
                case ControlKind::ColorArea:
                    DrawSettingsColorArea(dc, item, metrics, appearance);
                    break;
                case ControlKind::HueStrip:
                    DrawSettingsStrip(dc, item, metrics, appearance, false);
                    break;
                case ControlKind::AlphaStrip:
                    DrawSettingsStrip(dc, item, metrics, appearance, true);
                    break;
                case ControlKind::Info:
                    DrawSettingsInfo(dc, item, metrics);
                    break;
                default:
                    break;
            }
        }

        if (item.kind == ItemKind::Submenu) {
            DrawSubmenuArrow(dc, item, panel.size.cx, metrics, metrics.submenuArrow);
        }
    }
    if (appearance.overlay != 0) {
        DrawOverlay(dc, panel, appearance, overlayTime, panelGeometry);
    }
    if (contentFading) {
        dc->PopLayer();
    }

    if (metrics.borderWidth > 0 && (appearance.border >> 24) != 0) {
        ID2D1SolidColorBrush* brush = nullptr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(ColorFromArgb(appearance.border),
                                                &brush)) &&
            brush) {
            if (panelGeometry) {
                dc->DrawGeometry(panelGeometry, brush,
                                 static_cast<float>(metrics.borderWidth));
            } else {
                dc->DrawRoundedRectangle(&rounded, brush,
                                         static_cast<float>(metrics.borderWidth));
            }
            brush->Release();
        }
    }
    // CRT-style brightness and glow: additive white halo strokes plus a flash
    // over the panel, fading as the animation completes.
    if (frame.brightness > 0.001f) {
        const float brightness = std::clamp(frame.brightness, 0.0f, 1.0f);
        dc->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        const float widths[3] = {2.0f + 3.0f * brightness,
                                 6.0f + 10.0f * brightness,
                                 14.0f + 18.0f * brightness};
        const float alphas[3] = {0.45f * brightness, 0.25f * brightness,
                                 0.12f * brightness};
        for (int i = 0; i < 3; ++i) {
            ID2D1SolidColorBrush* brush = nullptr;
            if (SUCCEEDED(dc->CreateSolidColorBrush(
                    D2D1::ColorF(1.0f, 1.0f, 1.0f, alphas[i]), &brush)) &&
                brush) {
                if (panelGeometry) {
                    dc->DrawGeometry(panelGeometry, brush, widths[i]);
                } else {
                    dc->DrawRoundedRectangle(&rounded, brush, widths[i]);
                }
                brush->Release();
            }
        }
        ID2D1SolidColorBrush* flash = nullptr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.35f * brightness),
                &flash)) &&
            flash) {
            if (panelGeometry) {
                dc->FillGeometry(panelGeometry, flash);
            } else {
                dc->FillRoundedRectangle(&rounded, flash);
            }
            flash->Release();
        }
        dc->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    if (panelGeometry) {
        panelGeometry->Release();
    }
    if (fading) {
        dc->PopLayer();
    }
}

// ===========================================================================
// [CMO:MenuInput] Keyboard and mouse state machine.
// ===========================================================================

enum class MenuInputEvent : uint8_t {
    MouseMove,
    MouseLeave,
    WheelUp,
    WheelDown,
    KeyUp,
    KeyDown,
    KeyLeft,
    KeyRight,
    KeyEnter,
    KeyEscape,
    KeyHome,
    KeyEnd,
};

bool MenuItemIsSelectable(const LayoutItem& item) {
    return item.kind != ItemKind::Separator && item.kind != ItemKind::Header &&
           (item.flags & kModelDisabled) == 0;
}

void MenuStateMouseMove(MenuInputState& state, const LayoutPanel& panel,
                        int itemIndex) {
    state.keyboardIndex = -1;
    if (itemIndex < 0 || itemIndex >= static_cast<int>(panel.items.size()) ||
        !MenuItemIsSelectable(panel.items[itemIndex])) {
        state.hoverIndex = -1;
        return;
    }
    state.hoverIndex = itemIndex;
}

void MenuStateMouseLeave(MenuInputState& state) {
    state.hoverIndex = -1;
}

void MenuStateWheel(MenuInputState& state, const LayoutPanel& panel, int delta,
                    int maxHeight = 0) {
    const int itemCount = static_cast<int>(panel.items.size());
    if (itemCount == 0) {
        state.scrollOffset = 0;
        return;
    }

    int visibleCount = itemCount;
    if (maxHeight > 0) {
        visibleCount = 0;
        int y = 0;
        for (const LayoutItem& item : panel.items) {
            const int height = static_cast<int>(item.rect.bottom - item.rect.top);
            if (y + height > maxHeight) {
                break;
            }
            y += height;
            ++visibleCount;
        }
    }

    const int maxOffset = std::max(0, itemCount - visibleCount);
    const int step = delta > 0 ? 1 : -1;
    state.scrollOffset = std::clamp(state.scrollOffset + step, 0, maxOffset);

    if (state.hoverIndex >= 0) {
        if (state.hoverIndex < state.scrollOffset) {
            state.hoverIndex = state.scrollOffset;
        } else if (state.hoverIndex >= state.scrollOffset + visibleCount) {
            state.hoverIndex = state.scrollOffset + visibleCount - 1;
        }
    }
    if (state.keyboardIndex >= 0) {
        if (state.keyboardIndex < state.scrollOffset) {
            state.keyboardIndex = state.scrollOffset;
        } else if (state.keyboardIndex >= state.scrollOffset + visibleCount) {
            state.keyboardIndex = state.scrollOffset + visibleCount - 1;
        }
    }
}

void MenuStateKey(MenuInputState& state, const LayoutPanel& panel,
                  MenuInputEvent event) {
    const int count = static_cast<int>(panel.items.size());
    if (count == 0) {
        state.keyboardIndex = -1;
        return;
    }

    auto findNext = [&](int from, int direction) {
        for (int step = 1; step <= count; ++step) {
            const int index =
                ((from + direction * step) % count + count) % count;
            if (MenuItemIsSelectable(panel.items[index])) {
                return index;
            }
        }
        return -1;
    };
    auto findEdge = [&](int direction) {
        for (int i = 0; i < count; ++i) {
            const int candidate = direction > 0 ? i : count - 1 - i;
            if (MenuItemIsSelectable(panel.items[candidate])) {
                return candidate;
            }
        }
        return -1;
    };

    switch (event) {
        case MenuInputEvent::KeyDown: {
            const int from = state.keyboardIndex >= 0 ? state.keyboardIndex : -1;
            state.keyboardIndex = findNext(from, +1);
            state.hoverIndex = -1;
            break;
        }
        case MenuInputEvent::KeyUp: {
            const int from = state.keyboardIndex >= 0 ? state.keyboardIndex : count;
            state.keyboardIndex = findNext(from, -1);
            state.hoverIndex = -1;
            break;
        }
        case MenuInputEvent::KeyHome:
            state.keyboardIndex = findEdge(+1);
            state.hoverIndex = -1;
            break;
        case MenuInputEvent::KeyEnd:
            state.keyboardIndex = findEdge(-1);
            state.hoverIndex = -1;
            break;
        case MenuInputEvent::KeyRight: {
            const int index = state.keyboardIndex >= 0 ? state.keyboardIndex
                                                       : state.hoverIndex;
            if (index >= 0 && index < count &&
                panel.items[index].kind == ItemKind::Submenu) {
                state.openSubmenu = panel.items[index].submenuIndex;
            }
            break;
        }
        case MenuInputEvent::KeyLeft:
            state.openSubmenu = -1;
            break;
        default:
            break;
    }
}

const LayoutItem* MenuStateActiveItem(const LayoutPanel& panel,
                                      const MenuInputState& state) {
    const int index = state.hoverIndex >= 0 ? state.hoverIndex : state.keyboardIndex;
    if (index < 0 || index >= static_cast<int>(panel.items.size())) {
        return nullptr;
    }
    return &panel.items[index];
}

int MenuStateVisibleItems(const LayoutPanel& panel, const MenuInputState& state,
                          int maxHeight) {
    int y = 0;
    int count = 0;
    for (int i = state.scrollOffset; i < static_cast<int>(panel.items.size()); ++i) {
        const int height =
            static_cast<int>(panel.items[i].rect.bottom - panel.items[i].rect.top);
        if (y + height > maxHeight) {
            break;
        }
        y += height;
        ++count;
    }
    return count;
}

int MenuStateItemAt(const LayoutPanel& panel, const MenuInputState& state,
                    POINT clientPoint) {
    if (clientPoint.x < 0 || clientPoint.x >= panel.size.cx) {
        return -1;
    }
    for (size_t i = 0; i < panel.items.size(); ++i) {
        const LayoutItem& item = panel.items[i];
        if (clientPoint.y >= item.rect.top && clientPoint.y < item.rect.bottom) {
            if (!MenuItemIsSelectable(item)) {
                return -1;
            }
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Highest (deepest) open level whose screen rect contains the point, or -1.
int SessionLevelAtPoint(const std::vector<RECT>& windowScreenRects,
                        POINT screenPoint) {
    for (size_t i = windowScreenRects.size(); i > 0; --i) {
        const RECT& rect = windowScreenRects[i - 1];
        if (screenPoint.x >= rect.left && screenPoint.x < rect.right &&
            screenPoint.y >= rect.top && screenPoint.y < rect.bottom) {
            return static_cast<int>(i - 1);
        }
    }
    return -1;
}

POINT PanelPointForWindow(const RECT& windowScreenRect, int margin,
                          POINT screenPoint) {
    return POINT{screenPoint.x - windowScreenRect.left - margin,
                 screenPoint.y - windowScreenRect.top - margin};
}

// ===========================================================================
// [CMO:MenuWindow] Custom menu session: window wiring and invocation.
// ===========================================================================

const GUID kIidIDXGISurface = {
    0xcafcb56c, 0x6ac3, 0x4889, {0xbf, 0x47, 0x9e, 0x23, 0xbb, 0xd2, 0x60, 0xec}};

// Defined later in [CMO:Theme]; declared here for ShowCustomMenu.
bool IsDarkThemeActive();

uint32_t DpiForWindow(HWND window) {
    if (window) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
            auto getDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
                GetProcAddress(user32, "GetDpiForWindow"));
            if (getDpiForWindow) {
                const UINT dpi = getDpiForWindow(window);
                if (dpi >= 48) {
                    return dpi;
                }
            }
        }
    }
    return 96;
}

RECT WorkAreaForPoint(POINT pt) {
    RECT workArea = {0, 0, GetSystemMetrics(SM_CXSCREEN),
                     GetSystemMetrics(SM_CYSCREEN)};
    MONITORINFO info = {};
    info.cbSize = sizeof(info);
    const HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (monitor && GetMonitorInfoW(monitor, &info)) {
        workArea = info.rcWork;
    }
    return workArea;
}

struct CustomMenuResult {
    std::optional<uint32_t> chosenItemId;
    bool handled = false;
    bool failed = false;
    bool busy = false;  // another thread owns the custom renderer
};

bool FindMenuItemById(const std::vector<MenuItem>& items, uint32_t id,
                      MenuItem& out) {
    for (const MenuItem& item : items) {
        if (item.id == id) {
            out = item;
            return true;
        }
        if (FindMenuItemById(item.children, id, out)) {
            return true;
        }
    }
    return false;
}


struct MenuSession {
    CustomMenuResult result;
    std::shared_ptr<const RulesConfig> config;
    const MenuModel* model = nullptr;
    std::vector<MenuWindow*> windows;
    // Windows currently running their own open animation; repaints must not
    // interleave identity frames with the animation's frames.
    std::vector<uint8_t> windowAnimating;
    std::vector<MenuInputState> states;
    std::vector<BackdropBitmap> backdrops;
    // Per level: the parent menu's panel rect in this panel's local
    // coordinates; the shadow is zeroed only inside it.
    std::vector<RECT> shadowClipRects;
    int active = 0;
    int maxHeight = 0;
    int margin = 0;
    int submenuDelayMs = 150;
    int hoverCandidate = -1;
    int hoverLevel = -1;
    bool ownsMouseHook = false;
    bool submenuTimerActive = false;
    bool done = false;
    // Current render-driven animation frame (1.0 = fully visible, 0 offset).
    AnimationFrame animationFrame;
    POINT animationAnchor = {0, 0};
    LayoutMetrics metrics;
    Appearance appearance;
    // Overlay animation clock: seconds since the session opened.
    ULONGLONG overlayStartTick = 0;
    float OverlaySeconds() const {
        if (!appearance.overlayAnimate) {
            return 0.0f;  // static overlays never advance, even on repaints
        }
        return static_cast<float>(GetTickCount64() - overlayStartTick) / 1000.0f;
    }
    // Non-null for a settings session; see [CMO:SettingsUI].
    struct SettingsSessionContext* settings = nullptr;
};

// Per thread: only the thread running the session reads or clears it, and
// the unload teardown on other threads must not write into the session
// thread's stack.
inline thread_local MenuSession* g_menuSession = nullptr;

// One custom menu at a time, process-wide: Explorer runs folder windows on
// separate UI threads, and the render device, window pool, overlay resources
// and session state are shared and not thread-safe. A second custom menu falls
// back to the native menu instead of racing them.
std::atomic<bool> g_customMenuBusy{false};

void OnDeviceLost();

int MeasureTextWidthDirectWrite(const wchar_t* label, size_t length,
                                const LayoutMetrics& metrics) {
    const std::wstring display = StripAccelerators(std::wstring(label, length)).text;
    label = display.c_str();
    length = display.size();
    IDWriteFactory* dwrite = g_renderDevice.DWriteFactory();
    if (!dwrite || length == 0) {
        return EstimateTextWidth(label, length, metrics);
    }
    IDWriteTextFormat* format = nullptr;
    if (FAILED(dwrite->CreateTextFormat(
            metrics.fontFace.c_str(), nullptr,
            FontWeightToDwrite(metrics.fontWeight),
            FontStyleToDwrite(metrics.fontStyle), DWRITE_FONT_STRETCH_NORMAL,
            metrics.fontSize, L"", &format)) ||
        !format) {
        return EstimateTextWidth(label, length, metrics);
    }
    int width = EstimateTextWidth(label, length, metrics);
    IDWriteTextLayout* layout = nullptr;
    if (SUCCEEDED(dwrite->CreateTextLayout(label, static_cast<UINT32>(length),
                                           format, 4096.0f,
                                           static_cast<float>(metrics.itemHeight),
                                           &layout)) &&
        layout) {
        DWRITE_TEXT_METRICS textMetrics = {};
        if (SUCCEEDED(layout->GetMetrics(&textMetrics))) {
            width = static_cast<int>(textMetrics.widthIncludingTrailingWhitespace) + 2;
        }
        layout->Release();
    }
    format->Release();
    return width;
}

// --- Settings session state and input --------------------------------------

struct SettingsSessionContext {
    SettingsTarget target;
    SettingsModelInputs inputs;
    Appearance working;
    std::vector<MenuItem> model;
    std::vector<std::wstring> dirtyKeys;
    SettingsWriteState write;
    bool themeActive = false;
    int themeIndex = 0;
    std::wstring themeName;
    bool darkSystemTheme = false;
    bool hasLight = false;
    bool hasDark = false;
    std::wstring statusText;
    uint32_t dpi = 96;
    POINT panelOrigin = {0, 0};
    // Owns the rebuilt panels; windows point into it.
    std::shared_ptr<LayoutPanel> panel;
    // ShellExecute target for @open:* rows, launched after teardown.
    std::wstring pendingLaunch;
};

constexpr UINT_PTR kSettingsSaveTimerId = 2;
constexpr UINT_PTR kSettingsCaretTimerId = 3;

void SettingsArmSaveTimer(MenuSession& session) {
    if (!session.windows.empty() && session.windows[0]) {
        SetTimer(session.windows[0]->Handle(), kSettingsSaveTimerId, 100,
                 nullptr);
    }
}

bool SettingsKeyIsGeometry(const std::wstring& key) {
    const std::wstring k = ToLowerCopy(key);
    return k == L"itemheight" || k == L"padding" || k == L"verticalpadding" ||
           k == L"itempadding" || k == L"iconsize" || k == L"markerwidth" ||
           k == L"separatorspacing" || k == L"minwidth" || k == L"maxwidth" ||
           k == L"font" || k == L"fontweight" || k == L"fontstyle" ||
           k == L"showaccelerators" || k == L"@font:size";
}

const LayoutItem* SettingsFindItem(MenuSession& session, int level,
                                   const std::wstring& key) {
    if (level < 0 || level >= static_cast<int>(session.windows.size())) {
        return nullptr;
    }
    const LayoutPanel& panel = *session.windows[level]->Panel();
    for (const LayoutItem& item : panel.items) {
        if (item.control.key == key) {
            return &item;
        }
    }
    return nullptr;
}

void SettingsFocusField(MenuSession& session, int level,
                        const LayoutItem& item) {
    MenuInputState& state = session.states[level];
    state.focusedControl = item.control.key;
    state.editBuffer = item.controlText;
    state.caretPos = state.editBuffer.size();
    state.caretVisible = true;
    if (level >= 0 && level < static_cast<int>(session.windows.size()) &&
        session.windows[level]) {
        SetTimer(session.windows[level]->Handle(), kSettingsCaretTimerId, 530,
                 nullptr);
    }
}

// Values of the base [appearance] section of an appearance text (theme preset
// or menu.ini), used to display inherited values after a reset. Commented rows
// stay unset.
Appearance AppearanceFromAppearanceText(const std::wstring& text) {
    Appearance appearance;
    bool inAppearance = false;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t newline = text.find(L'\n', pos);
        const size_t lineEnd =
            newline == std::wstring::npos ? text.size() : newline;
        const std::wstring line =
            text.substr(pos, lineEnd - pos);
        const std::wstring trimmed =
            TrimWhitespace(StripInlineComment(line));
        if (!trimmed.empty() && trimmed[0] == L'[' && trimmed.back() == L']') {
            const std::wstring name = ToLowerCopy(
                TrimWhitespace(trimmed.substr(1, trimmed.size() - 2)));
            inAppearance = name == L"appearance";
        } else if (inAppearance) {
            const size_t equals = trimmed.find(L'=');
            if (equals != std::wstring::npos) {
                const std::wstring key =
                    TrimWhitespace(trimmed.substr(0, equals));
                const std::wstring value =
                    TrimWhitespace(trimmed.substr(equals + 1));
                if (!key.empty() && !value.empty()) {
                    ApplyAppearanceValue(appearance, key, value, nullptr);
                }
            }
        }
        if (newline == std::wstring::npos) {
            break;
        }
        pos = newline + 1;
    }
    return appearance;
}

void RelayoutSession(MenuSession& session);
void SettingsApplyReset(MenuSession& session, const std::wstring& key);
void SettingsPerformWrite(SettingsSessionContext* settings);
void RepaintMenuWindow(MenuSession* session, int index);
void SettingsSyncOverlayTimer(MenuSession& session);

// Rebuilds the settings model from the working appearance and re-lays out the
// open windows so changes show live.
void SettingsRefreshSession(MenuSession& session) {
    SettingsSessionContext* settings = session.settings;
    if (!settings) {
        return;
    }
    settings->inputs.working = settings->working;
    settings->inputs.statusText = settings->statusText;
    settings->model = BuildSettingsTree(settings->inputs);
    // Recompute the live metrics so geometry changes actually take effect.
    session.appearance = settings->working;
    session.metrics = ResolveLayoutMetrics(settings->working, settings->dpi,
                                           settings->darkSystemTheme);
    session.margin =
        settings->working.shadow &&
                (session.metrics.shadowSize > 0 || session.metrics.shadowBlur > 0)
            ? ShadowMargin(session.metrics.shadowSize, session.metrics.shadowBlur,
                           session.metrics.shadowOffsetX,
                           session.metrics.shadowOffsetY)
            : 0;
    RelayoutSession(session);
}

void SettingsApplyControlChange(MenuSession& session, const std::wstring& key,
                                const std::wstring& canonicalText,
                                bool commitGeometry);
void SettingsReplayOpenAnimation(MenuSession& session);

// True when the face exists in the DirectWrite system font collection. When
// the factory or collection is unavailable the value is accepted rather than
// blocking the edit.
bool FontFaceInstalled(const std::wstring& face) {
    if (face.empty()) {
        return false;
    }
    IDWriteFactory* dwrite = g_renderDevice.DWriteFactory();
    if (!dwrite) {
        return true;
    }
    IDWriteFontCollection* collection = nullptr;
    if (FAILED(dwrite->GetSystemFontCollection(&collection, FALSE)) ||
        !collection) {
        return true;
    }
    UINT32 index = 0;
    BOOL exists = FALSE;
    const HRESULT hr = collection->FindFamilyName(face.c_str(), &index, &exists);
    collection->Release();
    return SUCCEEDED(hr) && exists;
}

// Commits (Enter or blur) or discards (Esc) the field edit on one level.
void SettingsCommitFieldAt(MenuSession& session, int level) {
    if (!session.settings || level < 0 ||
        level >= static_cast<int>(session.states.size())) {
        return;
    }
    MenuInputState& state = session.states[level];
    if (state.focusedControl.empty()) {
        return;
    }
    const LayoutItem* item =
        SettingsFindItem(session, level, state.focusedControl);
    if (item) {
        std::wstring canonical;
        if (CommitFieldBuffer(item->control, state.editBuffer, canonical)) {
            SettingsApplyControlChange(session, item->control.key, canonical,
                                       true);
        }
    }
    state.focusedControl.clear();
    state.editBuffer.clear();
    state.caretVisible = false;
    if (level < static_cast<int>(session.windows.size()) &&
        session.windows[level]) {
        KillTimer(session.windows[level]->Handle(), kSettingsCaretTimerId);
    }
    SettingsRefreshSession(session);
}

void SettingsCancelFieldAt(MenuSession& session, int level) {
    if (!session.settings || level < 0 ||
        level >= static_cast<int>(session.states.size())) {
        return;
    }
    MenuInputState& state = session.states[level];
    if (state.focusedControl.empty()) {
        return;
    }
    state.focusedControl.clear();
    state.editBuffer.clear();
    state.caretVisible = false;
    if (level < static_cast<int>(session.windows.size()) &&
        session.windows[level]) {
        KillTimer(session.windows[level]->Handle(), kSettingsCaretTimerId);
    }
    SettingsRefreshSession(session);
}

void SettingsHandleReservedAction(MenuSession& session,
                                  const std::wstring& key,
                                  const std::wstring& canonicalText) {
    SettingsSessionContext* settings = session.settings;
    if (!settings) {
        return;
    }
    if (key.rfind(L"@target:", 0) == 0) {
        const std::wstring which = key.substr(8);
        SettingsTargetKind kind = SettingsTargetKind::MenuIniBase;
        if (which == L"light") {
            kind = SettingsTargetKind::MenuIniLight;
        } else if (which == L"dark") {
            kind = SettingsTargetKind::MenuIniDark;
        }
        settings->target = ResolveSettingsTarget(0, kind);
        settings->inputs.target = settings->target;
        SettingsRefreshSession(session);
        return;
    }
    if (key.rfind(L"@enum:", 0) == 0) {
        const size_t first = key.find(L':', 6);
        if (first != std::wstring::npos) {
            SettingsApplyControlChange(session, key.substr(6, first - 6),
                                       key.substr(first + 1), true);
        }
        return;
    }
    if (key.rfind(L"@effect:", 0) == 0) {
        const size_t first = key.find(L':', 8);
        if (first == std::wstring::npos) {
            return;
        }
        const std::wstring which = key.substr(8, first - 8);
        const std::wstring name = key.substr(first + 1);
        const bool overlay = which == L"overlay";
        const bool opening = which == L"open";
        const SettingsEffectName* names =
            overlay ? kSettingsOverlayNames : kSettingsEffectNames;
        const size_t nameCount = overlay ? ARRAYSIZE(kSettingsOverlayNames)
                                         : ARRAYSIZE(kSettingsEffectNames);
        uint32_t bit = 0;
        for (size_t i = 0; i < nameCount; ++i) {
            if (name == names[i].name) {
                bit = names[i].bit;
                break;
            }
        }
        const uint32_t effects =
            ToggleAnimationEffect(overlay
                                      ? settings->working.overlay
                                      : (opening
                                             ? settings->working.animationOpen
                                             : settings->working.animationClose),
                                  bit);
        SettingsApplyControlChange(
            session,
            overlay ? L"overlay"
                    : (opening ? L"animationOpen" : L"animationClose"),
            overlay ? OverlayEffectsText(effects) : AnimationEffectsText(effects),
            true);
        return;
    }
    if (key.rfind(L"@color:", 0) == 0 && key.size() > 6 &&
        key.compare(key.size() - 6, 6, L":unset") == 0) {
        // @color:<parent>:unset toggle: on = remove the key (inherit).
        const size_t first = key.find(L':', 7);
        if (first == std::wstring::npos) {
            return;
        }
        const std::wstring parent = key.substr(7, first - 7);
        const bool wantUnset = canonicalText == L"true";
        const std::wstring lower = ToLowerCopy(parent);
        if (lower == L"markercolor") {
            settings->working.hasMarkerColor = !wantUnset;
        } else if (lower == L"headercolor") {
            settings->working.hasHeaderColor = !wantUnset;
        }
        if (std::find(settings->dirtyKeys.begin(), settings->dirtyKeys.end(),
                      parent) == settings->dirtyKeys.end()) {
            settings->dirtyKeys.push_back(parent);
        }
        SettingsMarkDirty(settings->write, GetTickCount64(), 400);
        SettingsArmSaveTimer(session);
        SettingsRefreshSession(session);
        return;
    }
    if (key == L"@preview-open") {
        SettingsReplayOpenAnimation(session);
        return;
    }
    if (key.rfind(L"@reset:", 0) == 0) {
        SettingsApplyReset(session, key);
        return;
    }
    if (key.rfind(L"@cornerRadii:", 0) == 0 &&
        key.compare(key.size() - 6, 6, L":unset") == 0) {
        const bool wantUnset = canonicalText == L"true";
        settings->working.hasCornerRadii = !wantUnset;
        if (std::find(settings->dirtyKeys.begin(), settings->dirtyKeys.end(),
                      std::wstring(L"cornerRadii")) ==
            settings->dirtyKeys.end()) {
            settings->dirtyKeys.push_back(L"cornerRadii");
        }
        SettingsMarkDirty(settings->write, GetTickCount64(), 400);
        SettingsArmSaveTimer(session);
        SettingsRefreshSession(session);
        return;
    }
    if (key == L"@open:ini") {
        settings->pendingLaunch = ConfigFilePath();
        session.done = true;
        return;
    }
    if (key == L"@open:theme") {
        settings->pendingLaunch = ThemeFilePath(settings->target.themeIndex);
        session.done = true;
        return;
    }
    if (key == L"@open:windhawk") {
        settings->pendingLaunch = L"windhawk.exe";
        session.done = true;
        return;
    }
}

void SettingsApplyControlChange(MenuSession& session, const std::wstring& key,
                                const std::wstring& canonicalText,
                                bool commitGeometry) {
    SettingsSessionContext* settings = session.settings;
    if (!settings || key.empty()) {
        return;
    }
    if (key == L"@font:size") {
        int size = 0;
        if (ParseIntField(canonicalText, 6, 72, size)) {
            settings->working.fontSize = static_cast<float>(size);
            if (std::find(settings->dirtyKeys.begin(),
                          settings->dirtyKeys.end(),
                          std::wstring(L"font")) == settings->dirtyKeys.end()) {
                settings->dirtyKeys.push_back(L"font");
            }
            SettingsMarkDirty(settings->write, GetTickCount64(), 400);
            SettingsArmSaveTimer(session);
            if (commitGeometry) {
                SettingsRefreshSession(session);
            }
        }
        return;
    }
    if (key == L"@font:face") {
        const std::wstring face = TrimWhitespace(canonicalText);
        if (!face.empty() && FontFaceInstalled(face)) {
            session.settings->working.fontFace = face;
            if (std::find(session.settings->dirtyKeys.begin(),
                          session.settings->dirtyKeys.end(),
                          std::wstring(L"font")) ==
                session.settings->dirtyKeys.end()) {
                session.settings->dirtyKeys.push_back(L"font");
            }
            SettingsMarkDirty(session.settings->write, GetTickCount64(), 400);
            SettingsArmSaveTimer(session);
            session.settings->statusText = L"Saved";
        } else {
            session.settings->statusText = L"Font not found";
        }
        SettingsRefreshSession(session);
        return;
    }
    if (key[0] == L'@') {
        SettingsHandleReservedAction(session, key, canonicalText);
        return;
    }
    const ConfigSchemaEntry* entry = SchemaFind(key);
    if (!entry) {
        return;
    }
    ApplyAppearanceValue(settings->working, entry->key, canonicalText, nullptr);
    if (std::find(settings->dirtyKeys.begin(), settings->dirtyKeys.end(),
                  std::wstring(entry->key)) == settings->dirtyKeys.end()) {
        settings->dirtyKeys.push_back(entry->key);
    }
    SettingsMarkDirty(settings->write, GetTickCount64(), 400);
    SettingsArmSaveTimer(session);
    const std::wstring appliedKey = ToLowerCopy(entry->key);
    if (appliedKey == L"overlay" || appliedKey == L"overlayanimate" ||
        appliedKey == L"overlayframems") {
        SettingsSyncOverlayTimer(session);
    }
    if (!SettingsKeyIsGeometry(entry->key) || commitGeometry) {
        SettingsRefreshSession(session);
    }
}

// The schema key a dragged control edits: color-editor parts edit their parent
// color key.
std::wstring SettingsDragSchemaKey(const std::wstring& controlKey) {
    if (controlKey.rfind(L"@color:", 0) == 0) {
        const size_t first = controlKey.find(L':', 7);
        if (first != std::wstring::npos) {
            return controlKey.substr(7, first - 7);
        }
    }
    return controlKey;
}

bool SettingsComputeDragValue(SettingsSessionContext& settings,
                              const LayoutItem& item, POINT pt,
                              std::wstring& out) {
    const ControlSpec& control = item.control;
    const std::wstring& key = control.key;
    if (control.kind == ControlKind::IntSlider) {
        const int width = item.trackRect.right - item.trackRect.left;
        const int value = SliderValueFromX(pt.x, item.trackRect.left, width,
                                           control.minValue, control.maxValue,
                                           control.step);
        out = std::to_wstring(value);
        return true;
    }
    if (key.rfind(L"@color:", 0) == 0) {
        const size_t first = key.find(L':', 7);
        if (first == std::wstring::npos) {
            return false;
        }
        const std::wstring parent = key.substr(7, first - 7);
        const std::wstring part = key.substr(first + 1);
        const uint32_t current = AppearanceColorValue(settings.working, parent);
        const HsvColor hsv = RgbToHsv(current);
        const uint8_t alpha = static_cast<uint8_t>((current >> 24) & 0xFF);
        const float stripFraction =
            std::clamp(static_cast<float>(pt.x - item.stripRect.left) /
                           static_cast<float>(std::max(
                               1, static_cast<int>(item.stripRect.right -
                                                   item.stripRect.left))),
                       0.0f, 1.0f);
        const float areaS =
            std::clamp(static_cast<float>(pt.x - item.areaRect.left) /
                           static_cast<float>(std::max(
                               1, static_cast<int>(item.areaRect.right -
                                                   item.areaRect.left))),
                       0.0f, 1.0f);
        const float areaV =
            1.0f - std::clamp(static_cast<float>(pt.y - item.areaRect.top) /
                                  static_cast<float>(std::max(
                                      1, static_cast<int>(item.areaRect.bottom -
                                                          item.areaRect.top))),
                              0.0f, 1.0f);
        uint32_t updated = current;
        if (part == L"hue") {
            updated = HsvToRgb(HsvColor{stripFraction * 360.0f, hsv.s, hsv.v},
                               alpha);
        } else if (part == L"alpha") {
            updated = (current & 0x00FFFFFF) |
                      (static_cast<uint32_t>(std::lround(stripFraction * 255.0f))
                       << 24);
        } else if (part == L"area") {
            updated = HsvToRgb(HsvColor{hsv.h, areaS, areaV}, alpha);
        } else {
            return false;
        }
        out = FormatColorRgba(updated);
        return true;
    }
    if (key.rfind(L"@cornerRadii:", 0) == 0) {
        const int width = item.trackRect.right - item.trackRect.left;
        const int value = SliderValueFromX(pt.x, item.trackRect.left, width, 0,
                                           256, 1);
        const std::wstring part = key.substr(14);
        if (part == L"tl") {
            settings.working.cornerRadii.topLeft = value;
        } else if (part == L"tr") {
            settings.working.cornerRadii.topRight = value;
        } else if (part == L"br") {
            settings.working.cornerRadii.bottomRight = value;
        } else if (part == L"bl") {
            settings.working.cornerRadii.bottomLeft = value;
        } else {
            return false;
        }
        out = std::to_wstring(value);
        return true;
    }
    return false;
}

bool SettingsHandleMouseMove(MenuSession& session, int level, POINT panelPoint) {
    if (!session.settings || level < 0 ||
        level >= static_cast<int>(session.windows.size())) {
        return false;
    }
    MenuInputState& state = session.states[level];
    if (state.dragControl.empty()) {
        return false;
    }
    const LayoutItem* item =
        SettingsFindItem(session, level, state.dragControl);
    if (!item) {
        return false;
    }
    std::wstring text;
    if (SettingsComputeDragValue(*session.settings, *item, panelPoint, text)) {
        if (item->control.kind == ControlKind::IntSlider) {
            int value = 0;
            if (ParseIntField(text, item->control.minValue,
                              item->control.maxValue, value)) {
                state.dragValue = value;
            }
        }
        const std::wstring schemaKey = SettingsDragSchemaKey(item->control.key);
        if (schemaKey.rfind(L"@cornerRadii:", 0) == 0) {
            // The drag wrote the radius directly; dirty and refresh it.
            session.settings->working.hasCornerRadii = true;
            if (std::find(session.settings->dirtyKeys.begin(),
                          session.settings->dirtyKeys.end(),
                          std::wstring(L"cornerRadii")) ==
                session.settings->dirtyKeys.end()) {
                session.settings->dirtyKeys.push_back(L"cornerRadii");
            }
            SettingsMarkDirty(session.settings->write, GetTickCount64(), 400);
            SettingsArmSaveTimer(session);
            SettingsRefreshSession(session);
        } else {
            SettingsApplyControlChange(session, schemaKey, text,
                                       !SettingsKeyIsGeometry(schemaKey));
        }
        // Deferred geometry keys skip the relayout; repaint so the thumb and
        // the value readout still follow the cursor.
        RepaintMenuWindow(&session, level);
    }
    return true;
}

bool SettingsHandleMouseDown(MenuSession& session, int level,
                             POINT panelPoint) {
    if (!session.settings || level < 0 ||
        level >= static_cast<int>(session.windows.size())) {
        return false;
    }
    const LayoutPanel& panel = *session.windows[level]->Panel();
    MenuInputState& state = session.states[level];
    const int hit = MenuStateItemAt(panel, state, panelPoint);
    if (hit < 0) {
        return false;
    }
    const std::wstring clickedKey = panel.items[hit].control.key;
    if (panel.items[hit].control.kind == ControlKind::None) {
        return false;
    }
    // Leaving a field commits its edit; clicking the field itself keeps the
    // caret where it is. The commit may relayout and rebuild panels, so the
    // clicked item is re-resolved afterwards.
    for (size_t i = 0; i < session.states.size(); ++i) {
        if (session.states[i].focusedControl.empty()) {
            continue;
        }
        if (static_cast<int>(i) == level &&
            session.states[i].focusedControl == clickedKey) {
            continue;
        }
        SettingsCommitFieldAt(session, static_cast<int>(i));
    }
    const LayoutItem* clicked = SettingsFindItem(session, level, clickedKey);
    if (!clicked) {
        return false;
    }
    const LayoutItem& item = *clicked;
    const ControlPart part = HitTestControlPart(item, panelPoint);
    switch (item.control.kind) {
        case ControlKind::Toggle:
            SettingsApplyControlChange(session, item.control.key,
                                       item.controlValue ? L"false" : L"true",
                                       true);
            return true;
        case ControlKind::IntSlider:
            if (part == ControlPart::Field) {
                if (state.focusedControl != item.control.key) {
                    SettingsFocusField(session, level, item);
                }
                return true;
            }
            state.dragControl = item.control.key;
            state.dragPart = part;
            SetCapture(session.windows[level]->Handle());
            SettingsHandleMouseMove(session, level, panelPoint);
            return true;
        case ControlKind::ColorArea:
        case ControlKind::HueStrip:
        case ControlKind::AlphaStrip:
            state.dragControl = item.control.key;
            state.dragPart = part;
            SetCapture(session.windows[level]->Handle());
            SettingsHandleMouseMove(session, level, panelPoint);
            return true;
        case ControlKind::TextField:
        case ControlKind::TextInput:
            if (state.focusedControl != item.control.key) {
                SettingsFocusField(session, level, item);
            }
            return true;
        default:
            return false;  // submenu rows open through the generic path
    }
}

bool SettingsHandleMouseUp(MenuSession& session, int level, POINT panelPoint) {
    if (!session.settings || level < 0 ||
        level >= static_cast<int>(session.windows.size())) {
        return false;
    }
    MenuInputState& state = session.states[level];
    if (!state.dragControl.empty()) {
        const LayoutItem* item =
            SettingsFindItem(session, level, state.dragControl);
        if (item) {
            std::wstring text;
            if (SettingsComputeDragValue(*session.settings, *item, panelPoint,
                                         text)) {
                const std::wstring schemaKey =
                    SettingsDragSchemaKey(item->control.key);
                if (schemaKey.rfind(L"@cornerRadii:", 0) == 0) {
                    session.settings->working.hasCornerRadii = true;
                    if (std::find(session.settings->dirtyKeys.begin(),
                                  session.settings->dirtyKeys.end(),
                                  std::wstring(L"cornerRadii")) ==
                        session.settings->dirtyKeys.end()) {
                        session.settings->dirtyKeys.push_back(L"cornerRadii");
                    }
                    SettingsMarkDirty(session.settings->write,
                                      GetTickCount64(), 400);
                    SettingsArmSaveTimer(session);
                    SettingsRefreshSession(session);
                } else {
                    SettingsApplyControlChange(session, schemaKey, text, true);
                }
            }
        }
        state.dragControl.clear();
        state.dragPart = ControlPart::None;
        ReleaseCapture();
        return true;
    }
    const LayoutPanel& panel = *session.windows[level]->Panel();
    const int hit = MenuStateItemAt(panel, state, panelPoint);
    if (hit < 0) {
        return false;
    }
    const LayoutItem& item = panel.items[hit];
    if (item.kind == ItemKind::Submenu) {
        return false;  // the generic path opens it
    }
    if (item.control.kind == ControlKind::None) {
        return false;
    }
    if (!item.control.key.empty() && item.control.key[0] == L'@') {
        const std::wstring text =
            item.control.kind == ControlKind::Toggle
                ? (item.controlValue ? L"false" : L"true")
                : std::wstring();
        SettingsHandleReservedAction(session, item.control.key, text);
    }
    return true;  // settings rows never close the session
}

bool SettingsHandleKey(MenuSession& session, UINT key, wchar_t ch) {
    SettingsSessionContext* settings = session.settings;
    if (!settings || session.active < 0 ||
        session.active >= static_cast<int>(session.states.size())) {
        return false;
    }
    MenuInputState& state = session.states[session.active];
    const LayoutPanel& panel = *session.windows[session.active]->Panel();
    if (!state.focusedControl.empty()) {
        const LayoutItem* item =
            SettingsFindItem(session, session.active, state.focusedControl);
        if (!item) {
            state.focusedControl.clear();
            return false;
        }
        const ControlSpec control = item->control;
        if (key == VK_RETURN) {
            SettingsCommitFieldAt(session, session.active);
            return true;
        }
        if (key == VK_ESCAPE) {
            SettingsCancelFieldAt(session, session.active);
            return true;
        }
        if (key == VK_UP || key == VK_DOWN) {
            int value = item->controlValue + (key == VK_UP ? 1 : -1);
            value = std::clamp(value, control.minValue, control.maxValue);
            SettingsApplyControlChange(session, control.key,
                                       std::to_wstring(value), true);
            return true;
        }
        const FieldInputMode mode =
            control.kind == ControlKind::TextInput
                ? FieldInputMode::FreeText
                : control.kind == ControlKind::TextField
                      ? FieldInputMode::Hex
                      : (control.minValue < 0 ? FieldInputMode::IntSigned
                                              : FieldInputMode::IntUnsigned);
        ApplyFieldKey(state.editBuffer, state.caretPos, key, ch, mode);
        SettingsRefreshSession(session);
        return true;
    }
    if (key == VK_TAB) {
        SettingsCommitFieldAt(session, session.active);
        const LayoutPanel& tabPanel = *session.windows[session.active]->Panel();
        if (tabPanel.items.empty()) {
            return true;
        }
        for (size_t i = 1; i <= tabPanel.items.size(); ++i) {
            const int candidate = (std::max(0, state.keyboardIndex) +
                                   static_cast<int>(i)) %
                                  static_cast<int>(tabPanel.items.size());
            const LayoutItem& item = tabPanel.items[candidate];
            if (item.control.kind == ControlKind::IntSlider ||
                item.control.kind == ControlKind::TextField ||
                item.control.kind == ControlKind::TextInput) {
                state.keyboardIndex = candidate;
                SettingsFocusField(session, session.active, item);
                SettingsRefreshSession(session);
                return true;
            }
        }
        return true;
    }
    if (key == VK_LEFT || key == VK_RIGHT) {
        const LayoutItem* item = MenuStateActiveItem(panel, state);
        if (!item || item->control.kind == ControlKind::None) {
            return false;
        }
        const ControlSpec& control = item->control;
        if (control.kind == ControlKind::Toggle) {
            SettingsApplyControlChange(session, control.key,
                                       item->controlValue ? L"false" : L"true",
                                       true);
            return true;
        }
        if (control.kind == ControlKind::IntSlider) {
            const int step =
                (GetKeyState(VK_SHIFT) & 0x8000) ? control.coarseStep
                                                 : control.step;
            const int value = std::clamp(
                item->controlValue + (key == VK_RIGHT ? step : -step),
                control.minValue, control.maxValue);
            SettingsApplyControlChange(session, control.key,
                                       std::to_wstring(value), true);
            return true;
        }
        if (control.kind == ControlKind::Enum && !control.options.empty()) {
            int index = 0;
            for (size_t i = 0; i < control.options.size(); ++i) {
                if (_wcsicmp(control.options[i].c_str(),
                             item->controlText.c_str()) == 0) {
                    index = static_cast<int>(i);
                    break;
                }
            }
            const int count = static_cast<int>(control.options.size());
            index = (index + (key == VK_RIGHT ? 1 : count - 1)) % count;
            SettingsApplyControlChange(session, control.key,
                                       control.options[index], true);
            return true;
        }
        return false;
    }
    if (key == VK_RETURN) {
        const LayoutItem* item = MenuStateActiveItem(panel, state);
        if (!item || item->control.kind == ControlKind::None) {
            return false;
        }
        const ControlSpec& control = item->control;
        if (control.kind == ControlKind::Toggle) {
            SettingsApplyControlChange(session, control.key,
                                       item->controlValue ? L"false" : L"true",
                                       true);
            return true;
        }
        if (control.kind == ControlKind::IntSlider ||
            control.kind == ControlKind::TextField ||
            control.kind == ControlKind::TextInput) {
            SettingsFocusField(session, session.active, *item);
            SettingsRefreshSession(session);
            return true;
        }
        return false;  // submenus open through the generic path
    }
    if (ch != 0) {
        const LayoutItem* item = MenuStateActiveItem(panel, state);
        if (item) {
            const ControlSpec control = item->control;
            FieldInputMode mode = FieldInputMode::FreeText;
            bool startsEdit = false;
            if (control.kind == ControlKind::IntSlider && ch >= L'0' &&
                ch <= L'9') {
                mode = control.minValue < 0 ? FieldInputMode::IntSigned
                                            : FieldInputMode::IntUnsigned;
                startsEdit = true;
            } else if (control.kind == ControlKind::TextInput && ch >= 0x20 &&
                       ch != 0x7F) {
                startsEdit = true;
            }
            if (startsEdit) {
                SettingsFocusField(session, session.active, *item);
                MenuInputState& focused = session.states[session.active];
                ApplyFieldKey(focused.editBuffer, focused.caretPos, 0, ch,
                              mode);
                SettingsRefreshSession(session);
                return true;
            }
        }
    }
    return false;
}

struct AnimationSpec {
    uint32_t effects = 0;
    int durationMs = 0;
    int frameMs = 15;
    AnimEasing easing = AnimEasing::Linear;
    int slideOffsetX = 12;
    int slideOffsetY = 0;
    int scaleFrom = 92;
    bool anchorAtCursor = true;
    bool animate = false;
};

AnimationSpec ResolveAnimationSpec(const Appearance& appearance, bool opening) {
    AnimationSpec spec;
    spec.effects = opening ? appearance.animationOpen : appearance.animationClose;
    int duration = opening ? appearance.animationDuration
                           : appearance.animationCloseDuration;
    if (duration <= 0) {
        duration = appearance.animationDuration;
    }
    spec.durationMs = duration < 0 ? 0 : duration;
    spec.frameMs = std::clamp(appearance.animationFrameMs, 1, 100);
    spec.easing = appearance.animationEasing;
    spec.slideOffsetX = appearance.slideOffsetX;
    spec.slideOffsetY = appearance.slideOffsetY;
    spec.scaleFrom = std::clamp(appearance.scaleFrom, 10, 200);
    spec.anchorAtCursor = appearance.animationAnchorAtCursor;
    spec.animate = spec.effects != 0 && spec.durationMs > 0;
    return spec;
}

float ApplyAnimationEasing(AnimEasing easing, float t) {
    if (t <= 0.0f) {
        return 0.0f;
    }
    if (t >= 1.0f) {
        return 1.0f;
    }
    switch (easing) {
        case AnimEasing::Linear:
            return t;
        case AnimEasing::EaseOut: {
            const float u = 1.0f - t;
            return 1.0f - u * u * u;
        }
        case AnimEasing::EaseInOut:
            return t < 0.5f
                       ? 4.0f * t * t * t
                       : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
        case AnimEasing::Back: {
            constexpr float c1 = 1.70158f;
            constexpr float c3 = c1 + 1.0f;
            const float u = t - 1.0f;
            return 1.0f + c3 * u * u * u + c1 * u * u;
        }
        case AnimEasing::Bounce: {
            constexpr float n1 = 7.5625f;
            constexpr float d1 = 2.75f;
            if (t < 1.0f / d1) {
                return n1 * t * t;
            }
            if (t < 2.0f / d1) {
                t -= 1.5f / d1;
                return n1 * t * t + 0.75f;
            }
            if (t < 2.5f / d1) {
                t -= 2.25f / d1;
                return n1 * t * t + 0.9375f;
            }
            t -= 2.625f / d1;
            return n1 * t * t + 0.984375f;
        }
        case AnimEasing::Elastic: {
            constexpr float c4 = 2.0943951023931953f;  // 2*pi/3
            return std::pow(2.0f, -10.0f * t) *
                       std::sin((t * 10.0f - 0.75f) * c4) +
                   1.0f;
        }
    }
    return t;
}

// Visibility for a frame: open ramps 0->1, close mirrors it. Effects combine:
// opacities multiply, scales multiply, translations add.
AnimationFrame ComputeAnimationFrame(const AnimationSpec& spec, float t,
                                     bool opening) {
    AnimationFrame frame;
    if (!spec.animate) {
        return frame;
    }
    t = std::clamp(t, 0.0f, 1.0f);
    const float eased = ApplyAnimationEasing(spec.easing, t);
    const float v = opening ? eased : 1.0f - eased;
    if (spec.effects & kAnimFade) {
        frame.opacity *= v;
    }
    if (spec.effects & kAnimSlide) {
        frame.translateX += static_cast<float>(spec.slideOffsetX) * (1.0f - v);
        frame.translateY += static_cast<float>(spec.slideOffsetY) * (1.0f - v);
    }
    if (spec.effects & kAnimScale) {
        const float s =
            (static_cast<float>(spec.scaleFrom) +
             (100.0f - static_cast<float>(spec.scaleFrom)) * v) /
            100.0f;
        frame.scaleX *= s;
        frame.scaleY *= s;
    }
    if (spec.effects & kAnimUnfold) {
        frame.scaleX *= (2.0f + 98.0f * v) / 100.0f;
        frame.opacity *= std::min(1.0f, v * 4.0f);
    }
    if (spec.effects & kAnimCrt) {
        // CRT power-on: a bright line in the panel center blooms open, with a
        // slight horizontal settle and a flash that decays as it opens (and
        // returns as it closes).
        frame.center = true;
        frame.scaleY *= (2.0f + 98.0f * v) / 100.0f;
        frame.scaleX *= (85.0f + 15.0f * v) / 100.0f;
        frame.brightness =
            std::max(frame.brightness, (1.0f - v) * (1.0f - v));
    }
    if (spec.effects & kAnimDissolve) {
        frame.contentOpacity *= std::clamp((v - 0.35f) / 0.65f, 0.0f, 1.0f);
    }
    frame.opacity = std::clamp(frame.opacity, 0.0f, 1.0f);
    frame.contentOpacity = std::clamp(frame.contentOpacity, 0.0f, 1.0f);
    frame.scaleX = std::max(frame.scaleX, 0.0f);
    frame.scaleY = std::max(frame.scaleY, 0.0f);
    return frame;
}

void RenderMenuWindow(MenuWindow* window, const LayoutPanel& panel,
                      const MenuInputState& state, const LayoutMetrics& metrics,
                      const Appearance& appearance,
                      const BackdropBitmap* backdrop, int margin,
                      const RECT* shadowClipRect = nullptr,
                      const AnimationFrame& frame = AnimationFrame{},
                      POINT anchor = POINT{0, 0}, float overlayTime = 0.0f) {
    if (!window || !window->SwapChain() || !g_renderDevice.D2DDevice()) {
        return;
    }    IDXGISurface* surface = nullptr;
    if (FAILED(window->SwapChain()->GetBuffer(
            0, kIidIDXGISurface, reinterpret_cast<void**>(&surface))) ||
        !surface) {
        return;
    }
    ID2D1DeviceContext* dc = nullptr;
    if (FAILED(g_renderDevice.D2DDevice()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc)) ||
        !dc) {
        surface->Release();
        return;
    }
    const D2D1_BITMAP_PROPERTIES1 props = {
        {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
        96.0f,
        96.0f,
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        nullptr};
    ID2D1Bitmap1* target = nullptr;
    if (SUCCEEDED(dc->CreateBitmapFromDxgiSurface(surface, &props, &target)) &&
        target) {
        dc->SetTarget(target);
        dc->BeginDraw();
        dc->Clear(nullptr);
        DrawPanel(dc, panel, state, metrics, appearance, backdrop, margin,
                  shadowClipRect, frame, anchor, overlayTime);
        const HRESULT drawResult = dc->EndDraw();
        target->Release();
        if (drawResult == static_cast<HRESULT>(D2DERR_RECREATE_TARGET)) {
            OnDeviceLost();
        }
    }
    dc->Release();
    surface->Release();
    window->Present();
}

// Renders every window of the session at one animation frame.
void RenderSessionFrame(MenuSession& session, const AnimationFrame& frame) {
    session.animationFrame = frame;
    for (size_t i = 0; i < session.windows.size(); ++i) {
        MenuWindow* window = session.windows[i];
        if (!window || !window->Panel()) {
            continue;
        }
        const BackdropBitmap* backdrop = nullptr;
        if (i < session.backdrops.size() && session.backdrops[i].width > 0) {
            backdrop = &session.backdrops[i];
        }
        const RECT* shadowClipRect =
            i < session.shadowClipRects.size() ? &session.shadowClipRects[i]
                                               : nullptr;
        RenderMenuWindow(window, *window->Panel(), session.states[i],
                         session.metrics, session.appearance, backdrop,
                         session.margin, shadowClipRect, frame,
                         session.animationAnchor, session.OverlaySeconds());
    }
}

// True for keyboard messages, which a menu always consumes.
bool IsKeyboardMessage(UINT message) {
    switch (message) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_DEADCHAR:
        case WM_SYSDEADCHAR:
            return true;
        default:
            return false;
    }
}

// True for mouse input. While a session is dismissing, these must stay queued
// for Explorer: dispatching the replayed right-click (or the matching up)
// while the menu is still alive breaks the shell's context-menu gesture
// arming, which is the "a right-click just closes the menu" failure.
bool IsMouseMessage(UINT message) {
    if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) {
        return true;
    }
    // WM_NCMOUSEMOVE..WM_NCXBUTTONDBLCLK (MinGW does not define the FIRST/LAST
    // macros).
    if (message >= 0x00A0 && message <= 0x00AD) {
        return true;
    }
    return false;
}

// Render-driven fade/slide. DirectComposition animations are not evaluated for
// this target (every HRESULT succeeds and nothing moves), so the UI thread
// draws and presents the frames itself. Messages are pumped so the menu stays
// responsive while the frames run.
void RunSessionAnimation(MenuSession& session, const AnimationSpec& spec,
                         bool opening) {
    if (!spec.animate || spec.durationMs <= 0) {
        return;
    }
    if (g_settings->debugLogging) {
        Wh_Log(L"Menu animation: opening=%d effects=%08X duration=%d frame=%d "
               L"easing=%d",
               opening ? 1 : 0, spec.effects, spec.durationMs, spec.frameMs,
               static_cast<int>(spec.easing));
    }
    const ULONGLONG start = GetTickCount64();
    for (;;) {
        const ULONGLONG elapsed = GetTickCount64() - start;
        float t = static_cast<float>(elapsed) /
                  static_cast<float>(spec.durationMs);
        if (t > 1.0f) {
            t = 1.0f;
        }
        RenderSessionFrame(session, ComputeAnimationFrame(spec, t, opening));
        // A closing animation must run to the end even though the session is
        // already marked done.
        if (t >= 1.0f || (opening && session.done)) {
            break;
        }
        MSG msg = {};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
            if (msg.message == WM_QUIT) {
                // Leave the quit request queued for the thread's own loop;
                // dispatching it here would swallow it. A long configured
                // animation must not delay the thread's exit either.
                return;
            }
            if (IsKeyboardMessage(msg.message)) {
                // Consume keys while the menu is open, like native menus.
                if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    break;
                }
                continue;
            }
            if ((!opening || session.done) && IsMouseMessage(msg.message)) {
                // Leave the dismissing/reopening click queued for Explorer.
                break;
            }
            if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        // Wait out the frame while staying responsive to the low-level mouse
        // hook, which the system dispatches when this thread pumps messages.
        MsgWaitForMultipleObjectsEx(
            0, nullptr, static_cast<DWORD>(std::clamp(spec.frameMs, 1, 100)),
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    }
}

void SettingsReplayOpenAnimation(MenuSession& session) {
    if (session.settings) {
        RunSessionAnimation(session,
                            ResolveAnimationSpec(session.settings->working, true),
                            true);
    }
}

// Runs the open animation for a single session window (a submenu). A nested
// action can close the animating submenu while frames pump, so every iteration
// re-checks that the window is still part of the session.
void RunWindowAnimation(MenuSession& session, size_t index,
                        const AnimationSpec& spec, POINT anchor) {
    if (!spec.animate || spec.durationMs <= 0 ||
        index >= session.windows.size()) {
        return;
    }
    MenuWindow* window = session.windows[index];
    if (!window || !window->Panel()) {
        return;
    }
    const LayoutPanel* panel = window->Panel();
    const BackdropBitmap* backdrop =
        index < session.backdrops.size() && session.backdrops[index].width > 0
            ? &session.backdrops[index]
            : nullptr;
    if (index < session.windowAnimating.size()) {
        session.windowAnimating[index] = 1;
    }
    const ULONGLONG start = GetTickCount64();
    for (;;) {
        if (session.done || index >= session.windows.size() ||
            session.windows[index] != window || window->Panel() != panel) {
            break;
        }
        const ULONGLONG elapsed = GetTickCount64() - start;
        float t = static_cast<float>(elapsed) /
                  static_cast<float>(spec.durationMs);
        if (t > 1.0f) {
            t = 1.0f;
        }
        const RECT* shadowClipRect =
            index < session.shadowClipRects.size()
                ? &session.shadowClipRects[index]
                : nullptr;
        RenderMenuWindow(window, *panel, session.states[index], session.metrics,
                         session.appearance, backdrop, session.margin,
                         shadowClipRect, ComputeAnimationFrame(spec, t, true),
                         anchor, session.OverlaySeconds());
        if (t >= 1.0f) {
            break;
        }
        MSG msg = {};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
            if (msg.message == WM_QUIT) {
                // Leave the quit request queued for the thread's own loop;
                // dispatching it here would swallow it. Marking the session
                // done ends this animation on the next loop check.
                session.done = true;
                break;
            }
            if (IsKeyboardMessage(msg.message)) {
                // Consume keys while the menu is open, like native menus.
                if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    break;
                }
                continue;
            }
            if (session.done && IsMouseMessage(msg.message)) {
                // The session is dismissing: leave input queued for Explorer.
                break;
            }
            if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        // Wait out the frame while staying responsive to the low-level mouse
        // hook, which the system dispatches when this thread pumps messages.
        MsgWaitForMultipleObjectsEx(
            0, nullptr, static_cast<DWORD>(std::clamp(spec.frameMs, 1, 100)),
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    }
    if (index < session.windowAnimating.size() &&
        index < session.windows.size() && session.windows[index] == window) {
        session.windowAnimating[index] = 0;
    }
}

void RepaintMenuWindow(MenuSession* session, int index) {
    if (!session || index < 0 ||
        index >= static_cast<int>(session->windows.size())) {
        return;
    }
    if (index < static_cast<int>(session->windowAnimating.size()) &&
        session->windowAnimating[index]) {
        return;  // the window animation owns this window's frames
    }
    MenuWindow* window = session->windows[index];
    if (!window || !window->Panel()) {
        return;
    }
    const BackdropBitmap* backdrop = nullptr;
    if (index < static_cast<int>(session->backdrops.size()) &&
        session->backdrops[index].width > 0) {
        backdrop = &session->backdrops[index];
    }
    const RECT* shadowClipRect =
        index < static_cast<int>(session->shadowClipRects.size())
            ? &session->shadowClipRects[index]
            : nullptr;
    RenderMenuWindow(window, *window->Panel(), session->states[index],
                     session->metrics, session->appearance, backdrop,
                     session->margin, shadowClipRect, session->animationFrame,
                     session->animationAnchor, session->OverlaySeconds());
}

void CloseSubmenusBelow(MenuSession* session, int index) {
    while (static_cast<int>(session->windows.size()) > index + 1) {
        MenuWindow* window = session->windows.back();
        // A closed level must not keep blinking its caret.
        if (!session->states.back().focusedControl.empty()) {
            KillTimer(window->Handle(), kSettingsCaretTimerId);
            session->states.back().focusedControl.clear();
            session->states.back().editBuffer.clear();
            session->states.back().caretVisible = false;
        }
        session->windows.pop_back();
        if (!session->windowAnimating.empty()) {
            session->windowAnimating.pop_back();
        }
        session->states.pop_back();
        if (!session->shadowClipRects.empty()) {
            session->shadowClipRects.pop_back();
        }
        if (session->backdrops.size() >= session->windows.size() + 1) {
            session->backdrops.pop_back();
        }
        g_menuWindowSet->Release(window);
    }
    session->active = index;
    if (index >= 0 && index < static_cast<int>(session->states.size())) {
        session->states[index].openSubmenu = -1;
    }
}

void OpenSubmenu(MenuSession* session, int index, int itemIndex) {
    if (!session || index < 0 ||
        index >= static_cast<int>(session->windows.size())) {
        return;
    }
    const LayoutPanel* parent = session->windows[index]->Panel();
    if (!parent || itemIndex < 0 ||
        itemIndex >= static_cast<int>(parent->items.size())) {
        return;
    }
    const LayoutItem& item = parent->items[itemIndex];
    if (item.kind != ItemKind::Submenu || item.submenuIndex < 0 ||
        item.submenuIndex >= static_cast<int>(parent->children.size())) {
        return;
    }

    CloseSubmenusBelow(session, index);
    const LayoutPanel& childPanel = parent->children[item.submenuIndex];

    MenuWindow* child = g_menuWindowSet->Acquire();
    if (!child ||
        !child->Create(session->windows[index]->Handle(), &childPanel, false,
                       session->margin)) {
        if (child) {
            g_menuWindowSet->Release(child);
        }
        return;
    }

    POINT topLeft = {item.rect.left + session->margin,
                     item.rect.top + session->margin};
    ClientToScreen(session->windows[index]->Handle(), &topLeft);
    const RECT itemScreen = {topLeft.x, topLeft.y,
                             topLeft.x + (item.rect.right - item.rect.left),
                             topLeft.y + (item.rect.bottom - item.rect.top)};
    const RECT workArea = WorkAreaForPoint(topLeft);
    POINT childPos =
        SubmenuPosition(itemScreen, childPanel.size, workArea, 4);
    childPos.x -= session->margin;
    childPos.y -= session->margin;
    child->Move(childPos);

    BackdropBitmap backdrop;
    const POINT panelTopLeft = {childPos.x + session->margin,
                                childPos.y + session->margin};
    const RECT captureRect = {panelTopLeft.x, panelTopLeft.y,
                              panelTopLeft.x + childPanel.size.cx,
                              panelTopLeft.y + childPanel.size.cy};
    const bool hasBackdrop =
        WantsBackdropCapture(session->appearance) &&
        CaptureBackdrop(captureRect, kBackdropDownscaleFactor,
                        BackdropBlurPasses(session->appearance), backdrop);

    session->windows.push_back(child);
    session->windowAnimating.push_back(0);
    session->states.push_back(MenuInputState{});
    // Clip the submenu's shadow only where it overlaps the parent menu itself;
    // the parent's own shadow is left untouched.
    RECT parentWindow = {};
    GetWindowRect(session->windows[index]->Handle(), &parentWindow);
    const POINT childPanelPos = {childPos.x + session->margin,
                                 childPos.y + session->margin};
    session->shadowClipRects.push_back(
        RECT{parentWindow.left + session->margin - childPanelPos.x,
             parentWindow.top + session->margin - childPanelPos.y,
             parentWindow.right - session->margin - childPanelPos.x,
             parentWindow.bottom - session->margin - childPanelPos.y});
    session->backdrops.push_back(hasBackdrop ? std::move(backdrop)
                                             : BackdropBitmap{});
    session->active = static_cast<int>(session->windows.size()) - 1;
    session->states[index].openSubmenu = item.submenuIndex;
    session->states[index].hoverIndex = itemIndex;

    // Optional submenu animation: the same effects as the main open, anchored
    // to the child panel corner nearest the parent item.
    const AnimationSpec childSpec = ResolveAnimationSpec(session->appearance, true);
    const bool animateChild = session->appearance.animateSubmenus &&
                              childSpec.animate && childSpec.durationMs > 0;
    POINT childAnchor = {childPanel.size.cx / 2, childPanel.size.cy / 2};
    if (session->appearance.animationAnchorAtCursor) {
        const int panelLeft = childPos.x + session->margin;
        const int panelTop = childPos.y + session->margin;
        const int itemCenterX = (itemScreen.left + itemScreen.right) / 2;
        const int itemCenterY = (itemScreen.top + itemScreen.bottom) / 2;
        childAnchor = {itemCenterX < panelLeft + childPanel.size.cx / 2
                           ? 0
                           : childPanel.size.cx,
                       itemCenterY < panelTop + childPanel.size.cy / 2
                           ? 0
                           : childPanel.size.cy};
    }

    RepaintMenuWindow(session, index);
    RenderMenuWindow(child, childPanel, session->states.back(), session->metrics,
                     session->appearance,
                     hasBackdrop ? &session->backdrops.back() : nullptr,
                     session->margin, &session->shadowClipRects.back(),
                     animateChild
                         ? ComputeAnimationFrame(childSpec, 0.0f, true)
                         : AnimationFrame{},
                     childAnchor, session->OverlaySeconds());
    child->Show();
    if (animateChild) {
        RunWindowAnimation(*session, session->windows.size() - 1, childSpec,
                           childAnchor);
    }
}

constexpr UINT_PTR kMenuSubmenuTimerId = 1;
constexpr UINT_PTR kMenuOverlayTimerId = 4;

UINT OverlayTimerInterval(const Appearance& appearance) {
    return static_cast<UINT>(std::clamp(appearance.overlayFrameMs, 1, 100));
}

// While the settings menu is open, the overlay timer follows the setting so
// toggling it takes effect immediately instead of on the next open.
void SettingsSyncOverlayTimer(MenuSession& session) {
    if (!session.settings || session.windows.empty() || !session.windows[0]) {
        return;
    }
    const bool animate = session.settings->working.overlay != 0 &&
                         session.settings->working.overlayAnimate;
    if (animate) {
        SetTimer(session.windows[0]->Handle(), kMenuOverlayTimerId,
                 OverlayTimerInterval(session.settings->working), nullptr);
    } else {
        KillTimer(session.windows[0]->Handle(), kMenuOverlayTimerId);
    }
}

std::vector<RECT> SessionWindowRects(const MenuSession* session);
std::vector<RECT> SessionPanelRects(const MenuSession* session);
bool IsSessionWindow(const MenuSession* session, HWND hwnd);
HWND TargetWindowUnderPoint(const MenuSession* session, POINT pt);

inline std::atomic<HHOOK> g_menuMouseHook{nullptr};

// Observes clicks while a menu is open without capturing the mouse, so hover
// feedback keeps working everywhere. An outside right-click is replayed to the
// target so it opens its menu at the new point; outside left/middle clicks
// pass through so the target still acts on them.
LRESULT CALLBACK MenuMouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    MenuSession* session = g_menuSession;
    if (code == HC_ACTION && session && !session->done &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
         wParam == WM_MBUTTONDOWN)) {
        const auto* info = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        // Panel rects: the shadow margin is click-through and counts as
        // outside the menu for dismissal.
        const std::vector<RECT> rects = SessionPanelRects(session);
        if (SessionLevelAtPoint(rects, info->pt) < 0) {
            session->done = true;
            if (g_settings->debugLogging) {
                Wh_Log(L"Outside click closes the session (hook)");
            }
            // Wake the modal loop first so the session tears down immediately.
            PostThreadMessageW(GetCurrentThreadId(), WM_NULL, 0, 0);
            if (wParam == WM_RBUTTONDOWN) {
                // Explorer targets need the replay: the real button-down can
                // be the message the modal loop unwinds on, and the target
                // would never record the gesture. Clicks into other processes
                // must reach them untouched - consuming and re-posting would
                // lose non-client clicks and clicks into elevated windows.
                const HWND target = TargetWindowUnderPoint(session, info->pt);
                DWORD targetProcess = 0;
                if (target) {
                    GetWindowThreadProcessId(target, &targetProcess);
                }
                if (target && targetProcess == GetCurrentProcessId()) {
                    POINT client = info->pt;
                    ScreenToClient(target, &client);
                    PostMessageW(target, WM_RBUTTONDOWN, MK_RBUTTON,
                                 MAKELPARAM(client.x, client.y));
                    if (g_settings->debugLogging) {
                        Wh_Log(L"Replaying the right-click down for %p", target);
                    }
                    return 1;
                }
                return CallNextHookEx(nullptr, code, wParam, lParam);
            }
            // Left/middle clicks pass through: the menu closes and the target
            // still receives the click, so the clicked item is selected just
            // like it would be without a menu.
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

std::vector<RECT> SessionWindowRects(const MenuSession* session) {
    std::vector<RECT> rects;
    rects.reserve(session->windows.size());
    for (MenuWindow* menuWindow : session->windows) {
        RECT rect = {};
        GetWindowRect(menuWindow->Handle(), &rect);
        rects.push_back(rect);
    }
    return rects;
}

// Window rectangles inset by the shadow margin: the visible panel area. The
// margin is click-through (see WM_NCHITTEST) and counts as outside the menu
// for dismissal.
std::vector<RECT> SessionPanelRects(const MenuSession* session) {
    std::vector<RECT> rects = SessionWindowRects(session);
    const int margin = session->margin;
    for (RECT& rect : rects) {
        rect.left += margin;
        rect.top += margin;
        rect.right -= margin;
        rect.bottom -= margin;
    }
    return rects;
}

bool IsSessionWindow(const MenuSession* session, HWND hwnd) {
    for (MenuWindow* menuWindow : session->windows) {
        if (menuWindow->Handle() == hwnd) {
            return true;
        }
    }
    return false;
}

// Finds the window that should receive a replayed click at a screen point.
// WindowFromPoint normally returns the right window, but in a shadow margin it
// can report one of our topmost windows; then walk down the z-order and return
// the deepest window below that contains the point.
HWND TargetWindowUnderPoint(const MenuSession* session, POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd || !IsSessionWindow(session, hwnd)) {
        return hwnd;
    }
    for (HWND candidate = GetWindow(hwnd, GW_HWNDNEXT); candidate;
         candidate = GetWindow(candidate, GW_HWNDNEXT)) {
        if (!IsWindowVisible(candidate) || IsSessionWindow(session, candidate)) {
            continue;
        }
        RECT rect = {};
        if (!GetWindowRect(candidate, &rect) || !PtInRect(&rect, pt)) {
            continue;
        }
        HWND deepest = candidate;
        for (int depth = 0; depth < 16; ++depth) {
            POINT client = pt;
            ScreenToClient(deepest, &client);
            HWND child = ChildWindowFromPointEx(
                deepest, client,
                CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
            if (!child || child == deepest) {
                break;
            }
            deepest = child;
        }
        return deepest;
    }
    return nullptr;
}

// Handles a key press for the active (deepest open) panel. The modal loop
// calls this for keyboard messages while the owner keeps focus, so Explorer
// never loses its active look; the window procedure calls it as a fallback if
// a key is ever sent directly to one of our windows.
bool HandleMenuKey(MenuSession* session, UINT vk) {
    if (!session || session->done) {
        return false;
    }
    const int level = session->active;
    if (level < 0 || level >= static_cast<int>(session->windows.size()) ||
        !session->windows[level]->Panel()) {
        return false;
    }
    const LayoutPanel& panel = *session->windows[level]->Panel();
    MenuInputState& state = session->states[level];

    switch (vk) {
        case VK_ESCAPE:
            if (level > 0) {
                CloseSubmenusBelow(session, level - 1);
            } else {
                session->done = true;
            }
            return true;
        case VK_LEFT:
            if (level > 0) {
                CloseSubmenusBelow(session, level - 1);
            } else {
                MenuStateKey(state, panel, MenuInputEvent::KeyLeft);
            }
            return true;
        case VK_RIGHT: {
            const int activeIndex =
                state.hoverIndex >= 0 ? state.hoverIndex : state.keyboardIndex;
            if (activeIndex >= 0 &&
                activeIndex < static_cast<int>(panel.items.size()) &&
                panel.items[activeIndex].kind == ItemKind::Submenu) {
                OpenSubmenu(session, level, activeIndex);
            }
            return true;
        }
        case VK_RETURN: {
            const LayoutItem* activeItem = MenuStateActiveItem(panel, state);
            if (activeItem) {
                const int activeIndex =
                    static_cast<int>(activeItem - &panel.items[0]);
                if (activeItem->kind == ItemKind::Submenu) {
                    OpenSubmenu(session, level, activeIndex);
                } else {
                    session->result.chosenItemId = activeItem->invocation.id;
                    session->done = true;
                }
            }
            return true;
        }
        default:
            break;
    }

    MenuInputEvent event;
    switch (vk) {
        case VK_UP:
            event = MenuInputEvent::KeyUp;
            break;
        case VK_DOWN:
            event = MenuInputEvent::KeyDown;
            break;
        case VK_HOME:
            event = MenuInputEvent::KeyHome;
            break;
        case VK_END:
            event = MenuInputEvent::KeyEnd;
            break;
        default:
            return false;
    }
    MenuStateKey(state, panel, event);
    RepaintMenuWindow(session, level);
    return true;
}

LRESULT CustomMenuWindowProc(MenuWindow* window, HWND hwnd, UINT msg,
                             WPARAM wParam, LPARAM lParam) {
    MenuSession* session = g_menuSession;
    if (!session) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    if (session->done) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    int index = -1;
    for (size_t i = 0; i < session->windows.size(); ++i) {
        if (session->windows[i] == window) {
            index = static_cast<int>(i);
            break;
        }
    }
    if (index < 0 || !window->Panel()) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    const POINT clientPoint = {static_cast<short>(LOWORD(lParam)),
                               static_cast<short>(HIWORD(lParam))};

    switch (msg) {
        case WM_NCHITTEST: {
            // The shadow margin is decorative: make it click-through so items
            // behind it stay reachable.
            if (session->margin <= 0) {
                return HTCLIENT;
            }
            const POINT screen = {static_cast<short>(LOWORD(lParam)),
                                  static_cast<short>(HIWORD(lParam))};
            POINT client = screen;
            ScreenToClient(hwnd, &client);
            RECT rect = {};
            GetClientRect(hwnd, &rect);
            if (client.x < session->margin || client.y < session->margin ||
                client.x >= rect.right - session->margin ||
                client.y >= rect.bottom - session->margin) {
                return HTTRANSPARENT;
            }
            return HTCLIENT;
        }
        case WM_MOUSEMOVE: {
            if (session->settings && session->active >= 0 &&
                session->active < static_cast<int>(session->states.size()) &&
                !session->states[session->active].dragControl.empty()) {
                const std::vector<RECT> windowRects = SessionWindowRects(session);
                POINT screen = clientPoint;
                ClientToScreen(hwnd, &screen);
                const int level = session->active;
                const POINT levelPoint = PanelPointForWindow(
                    windowRects[level], session->margin, screen);
                SettingsHandleMouseMove(*session, level, levelPoint);
                return 0;
            }
            const std::vector<RECT> windowRects = SessionWindowRects(session);
            const std::vector<RECT> panelRects = SessionPanelRects(session);
            POINT screen = clientPoint;
            ClientToScreen(hwnd, &screen);
            // The hovered level comes from the panels: a window's shadow
            // margin is click-through and must not claim the cursor.
            const int level = SessionLevelAtPoint(panelRects, screen);
            if (level < 0) {
                if (session->active >= 0 &&
                    session->active < static_cast<int>(session->states.size())) {
                    MenuStateMouseLeave(session->states[session->active]);
                    RepaintMenuWindow(session, session->active);
                }
                CloseSubmenusBelow(session, 0);
                if (session->submenuTimerActive) {
                    KillTimer(hwnd, kMenuSubmenuTimerId);
                    session->submenuTimerActive = false;
                    session->hoverCandidate = -1;
                }
                return 0;
            }

            session->active = level;
            const LayoutPanel& levelPanel = *session->windows[level]->Panel();
            MenuInputState& levelState = session->states[level];
            const POINT levelPoint =
                PanelPointForWindow(windowRects[level], session->margin, screen);

            const int hit = MenuStateItemAt(levelPanel, levelState, levelPoint);
            const bool isSubmenu =
                hit >= 0 && levelPanel.items[hit].kind == ItemKind::Submenu;
            const bool ownsOpenChild =
                levelState.openSubmenu >= 0 && hit >= 0 &&
                levelPanel.items[hit].submenuIndex == levelState.openSubmenu;
            if (!ownsOpenChild) {
                CloseSubmenusBelow(session, level);
            }

            if (hit != levelState.hoverIndex) {
                MenuStateMouseMove(levelState, levelPanel, hit);
                RepaintMenuWindow(session, level);
            }

            if (isSubmenu) {
                if (!ownsOpenChild &&
                    (session->hoverCandidate != hit ||
                     !session->submenuTimerActive)) {
                    session->hoverCandidate = hit;
                    session->hoverLevel = level;
                    session->submenuTimerActive = true;
                    SetTimer(hwnd, kMenuSubmenuTimerId,
                             static_cast<UINT>(session->submenuDelayMs <= 0
                                                   ? 1
                                                   : session->submenuDelayMs),
                             nullptr);
                }
            } else if (session->submenuTimerActive) {
                KillTimer(hwnd, kMenuSubmenuTimerId);
                session->submenuTimerActive = false;
                session->hoverCandidate = -1;
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            // Clicks inside our windows land here; outside clicks are handled
            // by the low-level hook, which replays right-clicks to the target.
            const std::vector<RECT> panelRects = SessionPanelRects(session);
            POINT screen = clientPoint;
            ClientToScreen(hwnd, &screen);
            const int level = SessionLevelAtPoint(panelRects, screen);
            if (session->settings && level >= 0) {
                const std::vector<RECT> windowRects =
                    SessionWindowRects(session);
                const POINT levelPoint = PanelPointForWindow(
                    windowRects[level], session->margin, screen);
                if (SettingsHandleMouseDown(*session, level, levelPoint)) {
                    return 0;
                }
            }
            if (level < 0) {
                session->done = true;
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            const std::vector<RECT> windowRects = SessionWindowRects(session);
            const std::vector<RECT> panelRects = SessionPanelRects(session);
            POINT screen = clientPoint;
            ClientToScreen(hwnd, &screen);
            const int level = SessionLevelAtPoint(panelRects, screen);
            if (level >= 0) {
                const LayoutPanel& levelPanel = *session->windows[level]->Panel();
                MenuInputState& levelState = session->states[level];
                const POINT levelPoint =
                    PanelPointForWindow(windowRects[level], session->margin, screen);
                if (session->settings &&
                    SettingsHandleMouseUp(*session, level, levelPoint)) {
                    return 0;
                }
                const int hit = MenuStateItemAt(levelPanel, levelState, levelPoint);
                if (hit >= 0) {
                    const LayoutItem& item = levelPanel.items[hit];
                    if (item.kind == ItemKind::Submenu) {
                        OpenSubmenu(session, level, hit);
                    } else {
                        session->result.chosenItemId = item.invocation.id;
                        session->done = true;
                    }
                }
            }
            return 0;
        }
        case WM_RBUTTONUP:
            return 0;
        case WM_MOUSEWHEEL: {
            const std::vector<RECT> panelRects = SessionPanelRects(session);
            const POINT screen = {static_cast<short>(LOWORD(lParam)),
                                  static_cast<short>(HIWORD(lParam))};
            const int level = SessionLevelAtPoint(panelRects, screen);
            if (level >= 0) {
                const LayoutPanel& levelPanel = *session->windows[level]->Panel();
                MenuStateWheel(session->states[level], levelPanel,
                               static_cast<short>(HIWORD(wParam)),
                               session->maxHeight);
                RepaintMenuWindow(session, level);
            }
            return 0;
        }
        case WM_TIMER: {
            if (wParam == kMenuOverlayTimerId) {
                // Never repaint mid-teardown: a timer frame interleaved with
                // the close animation would stall it on a frozen frame.
                if (!session->done) {
                    for (size_t i = 0; i < session->windows.size(); ++i) {
                        RepaintMenuWindow(session, static_cast<int>(i));
                    }
                }
                return 0;
            }
            if (wParam == kSettingsCaretTimerId && session->settings) {
                if (session->active >= 0 &&
                    session->active <
                        static_cast<int>(session->states.size())) {
                    MenuInputState& caretState =
                        session->states[session->active];
                    if (!caretState.focusedControl.empty()) {
                        caretState.caretVisible = !caretState.caretVisible;
                        RepaintMenuWindow(session, session->active);
                    }
                }
                return 0;
            }
            if (wParam == kSettingsSaveTimerId && session->settings) {
                if (SettingsWriteDue(session->settings->write,
                                     GetTickCount64())) {
                    SettingsPerformWrite(session->settings);
                    SettingsRefreshSession(*session);
                    if (!session->settings->write.pending) {
                        KillTimer(hwnd, kSettingsSaveTimerId);
                    }
                }
                return 0;
            }
            if (wParam == kMenuSubmenuTimerId) {
                session->submenuTimerActive = false;
                KillTimer(hwnd, kMenuSubmenuTimerId);
                const int level = session->hoverLevel;
                if (level >= 0 && level == session->active &&
                    level < static_cast<int>(session->windows.size()) &&
                    session->hoverCandidate >= 0 &&
                    session->hoverCandidate ==
                        session->states[level].hoverIndex) {
                    OpenSubmenu(session, level, session->hoverCandidate);
                }
            }
            return 0;
        }
        case WM_KEYDOWN:
            if (session->settings &&
                SettingsHandleKey(*session, static_cast<UINT>(wParam), 0)) {
                return 0;
            }
            HandleMenuKey(session, static_cast<UINT>(wParam));
            return 0;
        case WM_MOUSEACTIVATE:
            // Never activate on click; the owner stays active.
            return MA_NOACTIVATE;
        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE && index == 0) {
                const HWND newActive = reinterpret_cast<HWND>(lParam);
                bool ours = false;
                for (MenuWindow* menuWindow : session->windows) {
                    if (menuWindow->Handle() == newActive) {
                        ours = true;
                        break;
                    }
                }
                if (!ours) {
                    session->done = true;
                }
            }
            return 0;
        }
        case WM_ACTIVATEAPP: {
            if (!wParam) {
                session->done = true;
            }
            return 0;
        }
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

CustomMenuResult ShowCustomMenu(const MenuModel& model, const LayoutKey& key,
                                HWND owner, POINT pt,
                                SettingsSessionContext* settings = nullptr,
                                bool activate = false) {
    CustomMenuResult result;
    if (g_unloading.load()) {
        result.failed = true;
        return result;
    }

    if (g_customMenuBusy.exchange(true)) {
        result.busy = true;
        return result;
    }
    struct BusyGuard {
        ~BusyGuard() { g_customMenuBusy.store(false); }
    } busyGuard;

    if (!g_renderDevice.IsReady() && !g_renderDevice.Initialize()) {
        result.failed = true;
        return result;
    }

    std::shared_ptr<const RulesConfig> config = g_configStore.Snapshot();
    const RulesConfig emptyConfig;
    const RulesConfig& effective = config ? *config : emptyConfig;
    const Appearance appearance =
        settings ? settings->working
                 : EffectiveAppearance(effective, key.darkTheme);
    const LayoutMetrics metrics =
        ResolveLayoutMetrics(appearance, key.dpi, key.darkTheme);

    std::shared_ptr<const LayoutPanel> panel;
    if (!settings) {
        panel = g_layoutCache->Find(key);
    }
    if (!panel) {
        auto built = std::make_shared<LayoutPanel>(
            BuildLayoutPanel(model.items, metrics, &MeasureTextWidthDirectWrite));
        bool bound = false;
        ID2D1DeviceContext* bindDc = nullptr;
        if (SUCCEEDED(g_renderDevice.D2DDevice()->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &bindDc)) &&
            bindDc) {
            g_contentCaches->Bind(*built, metrics, bindDc);
            bindDc->Release();
            bound = true;
        }
        if (bound && !settings) {
            g_layoutCache->Put(key, built);
        }
        panel = built;
    }
    if (!panel) {
        result.failed = true;
        return result;
    }

    MenuWindow* root = g_menuWindowSet->Acquire();
    const int margin =
        appearance.shadow && (metrics.shadowSize > 0 || metrics.shadowBlur > 0)
            ? ShadowMargin(metrics.shadowSize, metrics.shadowBlur,
                           metrics.shadowOffsetX, metrics.shadowOffsetY)
            : 0;
    if (!root || !root->Create(owner, panel.get(), true, margin, activate)) {
        if (root) {
            g_menuWindowSet->Release(root);
        }
        result.failed = true;
        return result;
    }

    const RECT workArea = WorkAreaForPoint(pt);
    const POINT panelPos = ClampPanelPosition(pt, panel->size, workArea);
    if (settings) {
        settings->panelOrigin = panelPos;
    }

    MenuSession session;
    session.config = config;
    session.model = &model;
    session.metrics = metrics;
    session.appearance = appearance;
    session.overlayStartTick = GetTickCount64();
    session.settings = settings;
    if (settings) {
        settings->panel = std::const_pointer_cast<LayoutPanel>(panel);
        settings->inputs.working = settings->working;
    }
    session.submenuDelayMs = g_settings->submenuDelayMs;
    if (session.submenuDelayMs < 0) {
        DWORD systemDelay = 400;
        session.submenuDelayMs =
            SystemParametersInfoW(SPI_GETMENUSHOWDELAY, 0, &systemDelay, 0)
                ? static_cast<int>(systemDelay)
                : 400;
    }
    session.maxHeight = workArea.bottom - workArea.top;
    session.margin = margin;
    session.windows.push_back(root);
    session.windowAnimating.push_back(0);
    session.states.push_back(MenuInputState{});
    session.shadowClipRects.push_back(RECT{});

    BackdropBitmap backdrop;
    const RECT captureRect = {panelPos.x, panelPos.y,
                              panelPos.x + panel->size.cx,
                              panelPos.y + panel->size.cy};
    const bool hasBackdrop =
        WantsBackdropCapture(appearance) &&
        CaptureBackdrop(captureRect, kBackdropDownscaleFactor,
                        BackdropBlurPasses(appearance), backdrop);
    session.backdrops.push_back(hasBackdrop ? std::move(backdrop)
                                            : BackdropBitmap{});

    g_activeSessions.fetch_add(1);
    g_menuSession = &session;
    g_menuWindowMessageHook = &CustomMenuWindowProc;
    if (g_settings->debugLogging) {
        Wh_Log(L"Custom menu session start (margin=%d dpi=%u)", margin, key.dpi);
    }

    const AnimationSpec opening = ResolveAnimationSpec(appearance, true);
    // Anchor for scale/crt/unfold: the panel corner nearest the invocation
    // point, or the panel center.
    if (appearance.animationAnchorAtCursor) {
        session.animationAnchor = {
            pt.x < panelPos.x + panel->size.cx / 2 ? 0 : panel->size.cx,
            pt.y < panelPos.y + panel->size.cy / 2 ? 0 : panel->size.cy};
    } else {
        session.animationAnchor = {panel->size.cx / 2, panel->size.cy / 2};
    }
    root->Move(POINT{panelPos.x - margin, panelPos.y - margin});
    // Draw the first animation frame before showing so the menu never flashes
    // at full opacity.
    RenderSessionFrame(session, ComputeAnimationFrame(opening, 0.0f, true));
    root->Show();
    if (activate) {
        // WM_HOTKEY grants this thread the right to set the foreground window.
        SetForegroundWindow(root->Handle());
    }
    RunSessionAnimation(session, opening, true);
    // Install the mouse hook after the open animation: while it is installed,
    // a sleeping animation loop delays mouse input system-wide.
    if (!g_menuMouseHook.load()) {
        HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, MenuMouseHookProc,
                                       ModModuleHandle(), 0);
        g_menuMouseHook.store(hook);
        session.ownsMouseHook = hook != nullptr;
        if (!hook && g_settings->debugLogging) {
            Wh_Log(L"Failed to install the menu mouse hook");
        }
    }
    if (appearance.overlay != 0 && appearance.overlayAnimate) {
        // Opt-in: keep redrawing while the menu is open so the overlay moves.
        SetTimer(root->Handle(), kMenuOverlayTimerId,
                 OverlayTimerInterval(appearance), nullptr);
    }

    MSG msg = {};
    while (!session.done) {
        if (g_unloading.load()) {
            // The control window asked this thread to tear down; leave the
            // modal loop so the session teardown can run.
            session.done = true;
            break;
        }
        const BOOL got = GetMessageW(&msg, nullptr, 0, 0);
        if (got <= 0) {
            if (got == 0) {
                PostQuitMessage(static_cast<int>(msg.wParam));
            }
            break;
        }
        // The owner keeps focus while the menu is open, so keyboard messages
        // are addressed to it. Handle them here, exactly like a native menu
        // loop, and swallow the rest instead of stealing focus from the owner.
        if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
            if (session.settings) {
                // Generate WM_CHAR for the field editor; the modal loop
                // consumes it on the next iteration.
                TranslateMessage(&msg);
                if (SettingsHandleKey(session, static_cast<UINT>(msg.wParam),
                                      0)) {
                    continue;
                }
            }
            HandleMenuKey(&session, static_cast<UINT>(msg.wParam));
            continue;
        }
        if (msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP ||
            msg.message == WM_CHAR || msg.message == WM_SYSCHAR ||
            msg.message == WM_DEADCHAR) {
            if (msg.message == WM_CHAR && session.settings &&
                SettingsHandleKey(session, 0,
                                  static_cast<wchar_t>(msg.wParam))) {
                continue;
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (session.submenuTimerActive) {
        KillTimer(root->Handle(), kMenuSubmenuTimerId);
        session.submenuTimerActive = false;
    }
    KillTimer(root->Handle(), kMenuOverlayTimerId);
    const AnimationSpec closing = ResolveAnimationSpec(appearance, false);
    RunSessionAnimation(session, closing, false);
    if (session.settings) {
        if (session.settings->write.pending) {
            SettingsPerformWrite(session.settings);
        }
        KillTimer(root->Handle(), kSettingsCaretTimerId);
        KillTimer(root->Handle(), kSettingsSaveTimerId);
    }
    if (session.ownsMouseHook) {
        HHOOK hook = g_menuMouseHook.exchange(nullptr);
        if (hook) {
            UnhookWindowsHookEx(hook);
        }
    }
    for (size_t i = session.windows.size(); i > 1; --i) {
        g_menuWindowSet->Release(session.windows[i - 1]);
    }
    g_menuWindowSet->Release(root);
    if (g_menuSession == &session) {
        g_menuWindowMessageHook = nullptr;
        g_menuSession = nullptr;
    }
    g_activeSessions.fetch_sub(1);
    if (session.settings && !session.settings->pendingLaunch.empty()) {
        const std::wstring target = session.settings->pendingLaunch;
        session.settings->pendingLaunch.clear();
        SHELLEXECUTEINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_FLAG_NO_UI;
        info.lpVerb = L"open";
        info.lpFile = target.c_str();
        info.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&info);
    }
    if (g_settings->debugLogging) {
        Wh_Log(L"Custom menu session end (chosen=%d)",
               session.result.chosenItemId.has_value() ? 1 : 0);
    }

    result = session.result;
    result.handled = true;
    return result;
}

void RelayoutSession(MenuSession& session) {
    SettingsSessionContext* settings = session.settings;
    if (!settings || session.windows.empty() || !session.windows[0]) {
        return;
    }
    auto panel = std::make_shared<LayoutPanel>(BuildLayoutPanel(
        settings->model, session.metrics, &MeasureTextWidthDirectWrite));
    ID2D1DeviceContext* bindDc = nullptr;
    if (SUCCEEDED(g_renderDevice.D2DDevice()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &bindDc)) &&
        bindDc) {
        g_contentCaches->Bind(*panel, session.metrics, bindDc);
        bindDc->Release();
    }
    settings->panel = panel;

    // Resolve each open level's panel through the open-submenu chain.
    std::vector<const LayoutPanel*> levelPanels(session.windows.size(), nullptr);
    levelPanels[0] = panel.get();
    for (size_t i = 1; i < session.windows.size(); ++i) {
        const LayoutPanel* parent = levelPanels[i - 1];
        const int childIndex = session.states[i - 1].openSubmenu;
        if (!parent || childIndex < 0 ||
            childIndex >= static_cast<int>(parent->children.size())) {
            levelPanels[i] = parent;
            continue;
        }
        levelPanels[i] = &parent->children[childIndex];
    }

    for (size_t i = 0; i < session.windows.size(); ++i) {
        const LayoutPanel* levelPanel = levelPanels[i];
        if (!levelPanel) {
            continue;
        }
        session.windows[i]->SetPanel(levelPanel);
        const int width = levelPanel->size.cx + 2 * session.margin;
        const int height = levelPanel->size.cy + 2 * session.margin;
        POINT pos = {0, 0};
        if (i == 0) {
            // Keep the panel anchored; only the shadow margin may change.
            pos = {settings->panelOrigin.x - session.margin,
                   settings->panelOrigin.y - session.margin};
        } else {
            const LayoutPanel* parentPanel = levelPanels[i - 1];
            int itemIndex = -1;
            for (size_t j = 0; j < parentPanel->items.size(); ++j) {
                if (parentPanel->items[j].submenuIndex ==
                    session.states[i - 1].openSubmenu) {
                    itemIndex = static_cast<int>(j);
                    break;
                }
            }
            if (itemIndex < 0) {
                continue;
            }
            const LayoutItem& parentItem = parentPanel->items[itemIndex];
            RECT parentRect = {};
            GetWindowRect(session.windows[i - 1]->Handle(), &parentRect);
            const int panelLeft = parentRect.left + session.margin;
            const int panelTop = parentRect.top + session.margin;
            const RECT itemScreen = {
                panelLeft + parentItem.rect.left,
                panelTop + parentItem.rect.top,
                panelLeft + parentItem.rect.right,
                panelTop + parentItem.rect.bottom};
            const RECT workArea =
                WorkAreaForPoint(POINT{itemScreen.left, itemScreen.top});
            POINT childPos =
                SubmenuPosition(itemScreen, levelPanel->size, workArea, 4);
            childPos.x -= session.margin;
            childPos.y -= session.margin;
            pos = childPos;
        }
        session.windows[i]->SetBounds(pos, width, height);

        if (i >= 1 && i < session.shadowClipRects.size()) {
            // The parent may have moved with the edit; recompute the parent's
            // panel rect in this panel's coordinates.
            RECT parentRect = {};
            GetWindowRect(session.windows[i - 1]->Handle(), &parentRect);
            const POINT childPanelPos = {pos.x + session.margin,
                                         pos.y + session.margin};
            session.shadowClipRects[i] =
                RECT{parentRect.left + session.margin - childPanelPos.x,
                     parentRect.top + session.margin - childPanelPos.y,
                     parentRect.right - session.margin - childPanelPos.x,
                     parentRect.bottom - session.margin - childPanelPos.y};
        }

        // Refresh the blur backdrop only when the panel size changed.
        const int expectedWidth =
            levelPanel->size.cx / kBackdropDownscaleFactor;
        const int expectedHeight =
            levelPanel->size.cy / kBackdropDownscaleFactor;
        const bool wantsBackdrop = WantsBackdropCapture(session.appearance);
        const bool backdropStale =
            i >= session.backdrops.size() ||
            (wantsBackdrop && session.backdrops[i].width == 0) ||
            session.backdrops[i].width != expectedWidth ||
            session.backdrops[i].height != expectedHeight;
        if (backdropStale) {
            const POINT panelTopLeft = {pos.x + session.margin,
                                        pos.y + session.margin};
            const RECT captureRect = {
                panelTopLeft.x, panelTopLeft.y,
                panelTopLeft.x + levelPanel->size.cx,
                panelTopLeft.y + levelPanel->size.cy};
            BackdropBitmap backdrop;
            const bool hasBackdrop =
                WantsBackdropCapture(session.appearance) &&
                CaptureBackdrop(captureRect, kBackdropDownscaleFactor,
                                BackdropBlurPasses(session.appearance),
                                backdrop);
            if (i < session.backdrops.size()) {
                session.backdrops[i] =
                    hasBackdrop ? std::move(backdrop) : BackdropBitmap{};
            }
        }

        RepaintMenuWindow(&session, static_cast<int>(i));
    }
}

void SettingsPerformWrite(SettingsSessionContext* settings) {
    if (!settings) {
        return;
    }
    if (settings->dirtyKeys.empty()) {
        settings->write.pending = false;
        return;
    }
    const std::vector<ConfigOverride> changes = BuildChangeOverrides(
        settings->working, settings->dirtyKeys, settings->target);
    bool ok = false;
    if (settings->target.kind == SettingsTargetKind::ThemeFile) {
        const std::wstring path = ThemeFilePath(settings->target.themeIndex);
        if (!path.empty()) {
            std::wstring existing;
            ReadConfigFile(path, existing);
            const std::wstring preset =
                GenerateThemeText(settings->target.themeIndex);
            const std::wstring text =
                BuildThemeTextWithChanges(preset, existing, changes);
            ok = !text.empty() && WriteConfigFile(path, text);
            if (ok) {
                g_themeStore.RefreshIfChanged();
            }
        }
    } else {
        const std::wstring path = ConfigFilePath();
        std::wstring existing;
        ReadConfigFile(path, existing);
        const std::wstring text =
            BuildMenuIniTextWithChanges(existing, changes);
        ok = !text.empty() && WriteConfigFile(path, text);
    }
    if (ok) {
        settings->dirtyKeys.clear();
        settings->statusText = L"Saved";
    } else {
        settings->statusText = L"Save failed";
    }
    SettingsWriteFinished(settings->write, ok, GetTickCount64(), 1000);
}

void SettingsApplyReset(MenuSession& session, const std::wstring& key) {
    SettingsSessionContext* settings = session.settings;
    if (!settings) {
        return;
    }
    const bool all = key == L"@reset:all";
    const std::wstring group = all ? std::wstring() : key.substr(7);

    // What the reset keys inherit: the theme preset, or menu.ini's base
    // section. Without this the menu would show the old value until reopen.
    Appearance inherited;
    bool haveInherited = false;
    if (settings->target.kind == SettingsTargetKind::ThemeFile) {
        inherited = AppearanceFromAppearanceText(
            GenerateThemeText(settings->target.themeIndex));
        haveInherited = true;
    } else if (settings->target.kind != SettingsTargetKind::MenuIniBase) {
        std::wstring fileText;
        ReadConfigFile(ConfigFilePath(), fileText);
        inherited = AppearanceFromAppearanceText(fileText);
        haveInherited = true;
    }

    for (const ConfigSchemaEntry& entry : kAppearanceSchema) {
        if (!all && group != entry.group) {
            continue;
        }
        if (settings->target.kind == SettingsTargetKind::MenuIniBase &&
            !entry.unset) {
            ApplyAppearanceValue(settings->working, entry.key,
                                 entry.defaultValue, nullptr);
        } else if (haveInherited) {
            std::wstring inheritedText;
            if (AppearanceValueText(inherited, entry, inheritedText)) {
                ApplyAppearanceValue(settings->working, entry.key,
                                     inheritedText, nullptr);
            } else {
                ApplyAppearanceValue(settings->working, entry.key,
                                     entry.defaultValue, nullptr);
            }
        }
        if (std::find(settings->dirtyKeys.begin(), settings->dirtyKeys.end(),
                      std::wstring(entry.key)) == settings->dirtyKeys.end()) {
            settings->dirtyKeys.push_back(entry.key);
        }
    }
    SettingsMarkDirty(settings->write, GetTickCount64(), 400);
    SettingsArmSaveTimer(session);
    SettingsRefreshSession(session);
}

void OpenSettingsMenu(HWND owner, POINT pt, bool activate = false) {
    if (g_unloading.load() || g_settings->menuMode != 0) {
        return;
    }
    // The hotkey path activates the menu; remember where the user was so
    // focus can return there when the menu closes.
    HWND previousForeground = activate ? GetForegroundWindow() : nullptr;
    SettingsSessionContext settings;
    settings.themeIndex = g_settings->themeIndex;
    settings.themeActive = settings.themeIndex > 0;
    settings.themeName = g_settings->theme;
    settings.darkSystemTheme = IsDarkThemeActive();
    std::wstring iniText;
    ReadConfigFile(ConfigFilePath(), iniText);
    const std::wstring lower = ToLowerCopy(iniText);
    settings.hasLight =
        lower.find(L"[appearance.light]") != std::wstring::npos;
    settings.hasDark = lower.find(L"[appearance.dark]") != std::wstring::npos;
    std::shared_ptr<const RulesConfig> config = g_configStore.Snapshot();
    const RulesConfig emptyConfig;
    settings.working = EffectiveAppearance(config ? *config : emptyConfig,
                                            settings.darkSystemTheme);
    settings.target = ResolveSettingsTarget(
        settings.themeIndex,
        DefaultSettingsTargetKind(settings.hasLight, settings.hasDark,
                                  settings.darkSystemTheme));
    settings.inputs.working = settings.working;
    settings.inputs.target = settings.target;
    settings.inputs.themeActive = settings.themeActive;
    settings.inputs.themeIndex = settings.themeIndex;
    settings.inputs.themeName = settings.themeName;
    settings.inputs.darkSystemTheme = settings.darkSystemTheme;
    settings.inputs.hasLight = settings.hasLight;
    settings.inputs.hasDark = settings.hasDark;
    settings.inputs.selectedEffects =
        settings.working.animationOpen | settings.working.animationClose;
    settings.inputs.statusText = L"Saved";
    settings.inputs.windhawkHints = {
        {L"Theme", g_settings->theme},
        {L"Menu mode", g_settings->menuMode == 0 ? L"Custom" : L"Classic"},
        {L"Show classic menu item",
         g_settings->showMoreOptionsItem ? L"on" : L"off"},
        {L"Submenu delay",
         std::to_wstring(g_settings->submenuDelayMs) + L" ms"},
        {L"Bypass keys", g_settings->enableShiftBypass ? L"on" : L"off"},
        {L"Debug logging", g_settings->debugLogging ? L"on" : L"off"},
        {L"Warmup delay",
         std::to_wstring(g_settings->warmupDelaySeconds) + L" s"},
    };
    settings.model = BuildSettingsTree(settings.inputs);

    MenuModel model;
    model.items = settings.model;
    LayoutKey key;
    key.dpi = owner ? DpiForWindow(owner) : 96;
    key.darkTheme = settings.darkSystemTheme;
    settings.dpi = key.dpi;
    const CustomMenuResult result =
        ShowCustomMenu(model, key, owner, pt, &settings, activate);
    if (result.busy && g_settings->debugLogging) {
        Wh_Log(L"Settings menu not shown; another menu is open");
    }
    if (result.handled && previousForeground && IsWindow(previousForeground)) {
        SetForegroundWindow(previousForeground);
    }
}

// Defined with the pending-capture queue later in this file.
void ReleasePendingCapturesForCurrentThread();

// ---------------------------------------------------------------------------
// Unload coordination. Every mod window belongs to the thread that created
// it, and Wh_ModUninit runs on Windhawk's management thread, where
// DestroyWindow cannot destroy another thread's window; survivors would keep
// window procedures that point into the unloaded DLL. Each thread that shows
// a menu therefore gets its own hidden message-only control window, and
// Wh_ModUninit posts a teardown request to every one of them and waits until
// they are all gone.
constexpr wchar_t kControlWindowClass[] = L"ContextMenuOverhaulV2Control";
constexpr UINT kControlUninitMessage = WM_APP + 37;
constexpr int kSettingsHotkeyId = 0x5E77;

// The optional global hotkey lives on its own message-only window and thread,
// so it works before any menu has been shown and RegisterHotKey /
// UnregisterHotKey always run on the thread that owns the window.
constexpr wchar_t kHotkeyWindowClass[] = L"ContextMenuOverhaulV2Hotkey";
constexpr UINT kHotkeyReregisterMessage = WM_APP + 39;

// Prefer an Explorer window of this process as the owner; a foreground window
// from another process would attach the two input queues.
HWND SettingsMenuOwner() {
    HWND foreground = GetForegroundWindow();
    DWORD processId = 0;
    if (foreground) {
        GetWindowThreadProcessId(foreground, &processId);
    }
    if (foreground && processId == GetCurrentProcessId()) {
        return foreground;
    }
    return GetShellWindow();
}

void ApplySettingsHotkey(HWND window) {
    if (!window || !IsWindow(window)) {
        return;
    }
    UnregisterHotKey(window, kSettingsHotkeyId);
    HotkeySpec spec;
    if (g_settings->menuMode != 0 ||
        !ParseHotkey(g_settings->settingsHotkey, spec)) {
        return;
    }
    if (!RegisterHotKey(window, kSettingsHotkeyId, spec.modifiers,
                        spec.virtualKey) &&
        g_settings->debugLogging) {
        Wh_Log(L"Failed to register the settings hotkey");
    }
}

LRESULT CALLBACK HotkeyWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam) {
    if (msg == WM_HOTKEY && wParam == kSettingsHotkeyId) {
        POINT pt = {};
        GetCursorPos(&pt);
        OpenSettingsMenu(SettingsMenuOwner(), pt, /*activate=*/true);
        return 0;
    }
    if (msg == kHotkeyReregisterMessage) {
        ApplySettingsHotkey(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

class HotkeyHost {
public:
    void Start() {
        if (!Configured()) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        StartLocked();
    }

    // Applies the current settings; starts the thread if a hotkey was just
    // configured.
    void SettingsChanged() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!thread_) {
            if (!Configured()) {
                return;
            }
            StartLocked();
        }
        const HWND window = window_.load();
        if (window) {
            PostMessageW(window, kHotkeyReregisterMessage, 0, 0);
        }
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!thread_) {
            return;
        }
        PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
        WaitForSingleObject(thread_, INFINITE);
        CloseHandle(thread_);
        thread_ = nullptr;
        window_.store(nullptr);
    }

private:
    static bool Configured() {
        HotkeySpec spec;
        return g_settings->menuMode == 0 &&
               ParseHotkey(g_settings->settingsHotkey, spec);
    }

    void StartLocked() {
        thread_ = CreateThread(nullptr, 0, &HotkeyHost::ThreadProc, this, 0,
                               &threadId_);
        if (!thread_ && g_settings->debugLogging) {
            Wh_Log(L"Failed to start the hotkey thread");
        }
    }

    static DWORD WINAPI ThreadProc(LPVOID param) {
        static_cast<HotkeyHost*>(param)->Run();
        return 0;
    }

    void Run() {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &HotkeyWindowProc;
        wc.hInstance = ModModuleHandle();
        wc.lpszClassName = kHotkeyWindowClass;
        if (!RegisterClassExW(&wc)) {
            // Reclaim a class left behind by a previous load.
            UnregisterClassW(kHotkeyWindowClass, ModModuleHandle());
            if (!RegisterClassExW(&wc)) {
                return;
            }
        }
        HWND window = CreateWindowExW(0, kHotkeyWindowClass, L"", 0, 0, 0, 0, 0,
                                      HWND_MESSAGE, nullptr,
                                      ModModuleHandle(), nullptr);
        if (window) {
            window_.store(window);
        }
        ApplySettingsHotkey(window);

        MSG msg = {};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (window) {
            DestroyWindow(window);
        }
        UnregisterClassW(kHotkeyWindowClass, ModModuleHandle());
    }

    std::mutex mutex_;
    HANDLE thread_ = nullptr;
    DWORD threadId_ = 0;
    std::atomic<HWND> window_{nullptr};
};

inline HotkeyHost g_hotkeyHost;

// Defined after OwnerSubclass; removes every window subclass this DLL
// installed so comctl32 cannot call into unloaded code.
void RemoveOwnerSubclasses();

LRESULT CALLBACK ControlWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (msg == kControlUninitMessage) {
        g_unloading.store(true);
        // Captures belong to the thread that parked them; release them here,
        // on that thread, while the shell objects are still valid.
        ReleasePendingCapturesForCurrentThread();
        if (g_menuSession) {
            g_menuSession->done = true;
            PostThreadMessageW(GetCurrentThreadId(), WM_NULL, 0, 0);
        }
        // Close any native menu still running on this thread (HMENU fallback
        // or native replay); its modal loop is what keeps mod code on the
        // stack while the DLL is unloaded.
        EndMenu();
        RemoveOwnerSubclasses();
        HHOOK hook = g_menuMouseHook.exchange(nullptr);
        if (hook) {
            UnhookWindowsHookEx(hook);
        }
        DestroyWindow(hwnd);
        {
            std::lock_guard<std::mutex> lock(g_controlWindowsMutex);
            const auto it = g_controlWindows.find(GetCurrentThreadId());
            if (it != g_controlWindows.end() && it->second == hwnd) {
                g_controlWindows.erase(it);
            }
        }
        UnregisterClassW(kMenuWindowClass, ModModuleHandle());
        UnregisterClassW(kControlWindowClass, ModModuleHandle());
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_controlWindowsMutex);
        const auto it = g_controlWindows.find(GetCurrentThreadId());
        if (it != g_controlWindows.end() && it->second == hwnd) {
            g_controlWindows.erase(it);
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Must be called on the UI thread (the TrackPopupMenu hook thread).
bool EnsureControlWindow() {
    if (g_unloading.load()) {
        return false;
    }
    const DWORD threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lock(g_controlWindowsMutex);
        const auto it = g_controlWindows.find(threadId);
        if (it != g_controlWindows.end() && IsWindow(it->second)) {
            return true;
        }
    }

    static std::mutex registerMutex;
    static bool registered = false;
    {
        std::lock_guard<std::mutex> lock(registerMutex);
        if (!registered) {
            WNDCLASSEXW wc = {};
            wc.cbSize = sizeof(wc);
            wc.lpfnWndProc = &ControlWindowProc;
            wc.hInstance = ModModuleHandle();
            wc.lpszClassName = kControlWindowClass;
            if (!RegisterClassExW(&wc)) {
                // Reclaim a class left behind by a previous load.
                UnregisterClassW(kControlWindowClass, ModModuleHandle());
                if (!RegisterClassExW(&wc)) {
                    if (g_settings->debugLogging) {
                        Wh_Log(L"Failed to register the control window class");
                    }
                    return false;
                }
            }
            registered = true;
        }
    }

    HWND control = CreateWindowExW(0, kControlWindowClass, L"", 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, ModModuleHandle(),
                                   nullptr);
    if (!control) {
        if (g_settings->debugLogging) {
            Wh_Log(L"Failed to create the control window");
        }
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_controlWindowsMutex);
        g_controlWindows[threadId] = control;
    }
    return true;
}

void PrebuildLayoutsForWarmup(const std::vector<MenuModel>& models, uint32_t dpi,
                              bool darkTheme) {
    if (g_unloading.load()) {
        return;
    }
    // The render device, layout cache and content caches are shared with the
    // menus; serialize with the same gate so no two threads touch them.
    if (g_customMenuBusy.exchange(true)) {
        return;
    }
    struct BusyGuard {
        ~BusyGuard() { g_customMenuBusy.store(false); }
    } busyGuard;
    if (!g_renderDevice.IsReady() && !g_renderDevice.Initialize()) {
        return;
    }
    std::shared_ptr<const RulesConfig> config = g_configStore.Snapshot();
    const RulesConfig emptyConfig;
    const RulesConfig& effective = config ? *config : emptyConfig;
    const Appearance appearance = EffectiveAppearance(effective, darkTheme);
    const LayoutMetrics metrics =
        ResolveLayoutMetrics(appearance, dpi, darkTheme);

    ID2D1DeviceContext* dc = nullptr;
    if (FAILED(g_renderDevice.D2DDevice()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc)) ||
        !dc) {
        return;
    }
    for (const MenuModel& model : models) {
        const LayoutKey key =
            MakeLayoutKey(model.sig, effective, dpi, darkTheme, model,
                          g_themeStore.Revision());
        if (g_layoutCache->Find(key)) {
            continue;
        }
        auto panel =
            std::make_shared<LayoutPanel>(BuildLayoutPanel(
                model.items, metrics, &MeasureTextWidthDirectWrite));
        g_contentCaches->Bind(*panel, metrics, dc);
        g_layoutCache->Put(key, panel);
    }
    dc->Release();
}

void OnDeviceLost() {
    g_layoutCache->InvalidateDevice();
    g_contentCaches->Clear();
    ReleaseOverlayResources();
    g_renderDevice.HandleDeviceLost();
}

// Shell property keys used by the Sort by and Group by submenus (all in the
// shell's System property set, defined here so no SDK propkey.h is needed).
const PROPERTYKEY kShellPropertyKeys[] = {
    {{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
     10},  // Name
    {{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
     14},  // Date modified
    {{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
     4},  // Type
    {{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
     12},  // Size
};
const wchar_t* const kShellPropertyLabels[] = {L"Name", L"Date modified", L"Type",
                                               L"Size"};
constexpr uint32_t kShellPropertyKeyCount = ARRAYSIZE(kShellPropertyKeys);
constexpr uint32_t kGroupNoneIndex = 0xFFFFFFFF;

// Core model for a context. The common commands come first, cached extension
// items are merged in later, and the native fallback stays last.
MenuModel BuildCoreModel(Scope scope, const std::vector<std::wstring>& paths, Shape shape) {
    MenuModel model{};
    const std::wstring typeKey =
        scope == Scope::Files ? MakeTypeKey(paths) : std::wstring(L"*");
    model.sig = ContextSignature{scope, typeKey, shape, Variant::Normal};

    uint32_t nextId = 1;
    auto makeCommand = [&](std::wstring label, std::wstring verb, uint32_t flags,
                           std::wstring iconRef = L"") {
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Command;
        item.label = std::move(label);
        item.canonicalVerb = std::move(verb);
        item.action = ActionKind::ShellVerb;
        item.flags = flags;
        item.iconRef = std::move(iconRef);
        return item;
    };
    auto makeViewAction = [&](std::wstring label, std::wstring dedupVerb,
                              ViewAction action, uint32_t flags,
                              std::wstring iconRef = L"", int32_t iconSize = -1) {
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Command;
        item.label = std::move(label);
        item.canonicalVerb = std::move(dedupVerb);
        item.action = ActionKind::ViewAction;
        item.viewAction = static_cast<uint32_t>(action);
        item.flags = flags;
        item.iconRef = std::move(iconRef);
        item.iconSize = iconSize;
        return item;
    };
    auto addCommand = [&](std::wstring label, std::wstring verb,
                          uint32_t flags = kModelNone, std::wstring iconRef = L"") {
        model.items.push_back(
            makeCommand(std::move(label), std::move(verb), flags, std::move(iconRef)));
    };
    auto addViewAction = [&](std::wstring label, std::wstring dedupVerb,
                             ViewAction action, uint32_t flags = kModelNone,
                             std::wstring iconRef = L"") {
        model.items.push_back(makeViewAction(std::move(label), std::move(dedupVerb),
                                             action, flags, std::move(iconRef)));
    };
    auto addSeparator = [&]() {
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Separator;
        model.items.push_back(std::move(item));
    };
    auto addSubmenu = [&](std::wstring label) -> MenuItem& {
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Submenu;
        item.action = ActionKind::Submenu;
        item.label = std::move(label);
        model.items.push_back(std::move(item));
        return model.items.back();
    };
    auto addFallback = [&]() {
        MenuItem item{};
        item.id = nextId++;
        item.kind = ItemKind::Command;
        item.action = ActionKind::Fallback;
        item.label = L"Show classic menu";
        model.items.push_back(std::move(item));
    };

    const bool multi = shape == Shape::Multi;
    const uint32_t multiDisabled = multi ? kModelDisabled : kModelNone;
    const std::wstring openLabel = FormatMultiLabel(L"Open", paths.size());

    if (scope == Scope::Background || scope == Scope::Desktop) {
        MenuItem& viewMenu = addSubmenu(L"View");
        viewMenu.iconRef = L"@glyph:E890";
        viewMenu.children.push_back(makeViewAction(L"Extra large icons", L"viewxlarge",
                                                   ViewAction::ViewExtraLargeIcons,
                                                   kModelNone, L"@icon:view-xlarge", 256));
        viewMenu.children.push_back(makeViewAction(L"Large icons", L"viewlarge",
                                                   ViewAction::ViewLargeIcons, kModelNone,
                                                   L"@icon:view-large", 96));
        viewMenu.children.push_back(makeViewAction(L"Medium icons", L"viewmedium",
                                                   ViewAction::ViewMediumIcons, kModelNone,
                                                   L"@icon:view-medium", 48));
        viewMenu.children.push_back(makeViewAction(L"Small icons", L"viewsmall",
                                                   ViewAction::ViewSmallIcons, kModelNone,
                                                   L"@icon:view-small"));
        viewMenu.children.push_back(makeViewAction(L"List", L"viewlist",
                                                   ViewAction::ViewList, kModelNone,
                                                   L"@icon:view-list"));
        viewMenu.children.push_back(makeViewAction(L"Details", L"viewdetails",
                                                   ViewAction::ViewDetails, kModelNone,
                                                   L"@icon:view-details"));
        viewMenu.children.push_back(makeViewAction(L"Tiles", L"viewtiles",
                                                   ViewAction::ViewTiles, kModelNone,
                                                   L"@icon:view-tiles"));
        viewMenu.children.push_back(makeViewAction(L"Content", L"viewcontent",
                                                   ViewAction::ViewContent, kModelNone,
                                                   L"@icon:view-content"));
        {
            MenuItem separator{};
            separator.id = nextId++;
            separator.kind = ItemKind::Separator;
            viewMenu.children.push_back(std::move(separator));
        }
        viewMenu.children.push_back(makeViewAction(L"Auto arrange icons", L"autoarrange",
                                                   ViewAction::AutoArrange, kModelNone));
        viewMenu.children.push_back(makeViewAction(L"Align icons to grid", L"aligngrid",
                                                   ViewAction::AlignToGrid, kModelNone));

        MenuItem& sortMenu = addSubmenu(L"Sort by");
        sortMenu.iconRef = L"@glyph:E8CB";
        for (uint32_t i = 0; i < kShellPropertyKeyCount; ++i) {
            MenuItem sortItem{};
            sortItem.id = nextId++;
            sortItem.kind = ItemKind::Command;
            sortItem.label = kShellPropertyLabels[i];
            sortItem.action = ActionKind::SortBy;
            sortItem.sortIndex = i;
            sortItem.sortAscending = true;
            sortMenu.children.push_back(std::move(sortItem));
        }
        {
            MenuItem separator{};
            separator.id = nextId++;
            separator.kind = ItemKind::Separator;
            sortMenu.children.push_back(std::move(separator));
        }
        for (bool ascending : {true, false}) {
            MenuItem directionItem{};
            directionItem.id = nextId++;
            directionItem.kind = ItemKind::Command;
            directionItem.label = ascending ? L"Ascending" : L"Descending";
            directionItem.action = ActionKind::SortDirection;
            directionItem.sortAscending = ascending;
            sortMenu.children.push_back(std::move(directionItem));
        }

        MenuItem& groupMenu = addSubmenu(L"Group by");
        groupMenu.iconRef = L"@glyph:E902";
        {
            MenuItem noneItem{};
            noneItem.id = nextId++;
            noneItem.kind = ItemKind::Command;
            noneItem.label = L"(None)";
            noneItem.action = ActionKind::GroupBy;
            noneItem.sortIndex = kGroupNoneIndex;
            groupMenu.children.push_back(std::move(noneItem));
        }
        for (uint32_t i = 0; i < kShellPropertyKeyCount; ++i) {
            MenuItem groupItem{};
            groupItem.id = nextId++;
            groupItem.kind = ItemKind::Command;
            groupItem.label = kShellPropertyLabels[i];
            groupItem.action = ActionKind::GroupBy;
            groupItem.sortIndex = i;
            groupMenu.children.push_back(std::move(groupItem));
        }
        {
            MenuItem separator{};
            separator.id = nextId++;
            separator.kind = ItemKind::Separator;
            groupMenu.children.push_back(std::move(separator));
        }
        for (bool ascending : {true, false}) {
            MenuItem directionItem{};
            directionItem.id = nextId++;
            directionItem.kind = ItemKind::Command;
            directionItem.label = ascending ? L"Ascending" : L"Descending";
            directionItem.action = ActionKind::GroupDirection;
            directionItem.sortAscending = ascending;
            groupMenu.children.push_back(std::move(directionItem));
        }

        addViewAction(L"Refresh", L"refresh", ViewAction::Refresh, kModelNone,
                      L"@glyph:E72C");
        addSeparator();
        addCommand(L"Paste", L"paste", kModelNone, L"@glyph:E77F");
        addCommand(L"Paste shortcut", L"pastelink");
        addSeparator();
        MenuItem& newMenu = addSubmenu(L"New");
        BuildNewMenuChildren(newMenu.children, nextId);
        if (scope == Scope::Desktop) {
            addSeparator();
            addCommand(L"Display settings", L"display", kModelNone, L"@glyph:E7F4");
            addCommand(L"Personalize", L"personalize", kModelNone, L"@glyph:E790");
        }
        addSeparator();
        addFallback();
        return model;
    }

    if (scope == Scope::Drive) {
        addCommand(openLabel, L"open", kModelDefault);
        addCommand(L"Open in new window", L"opennew");
        addCommand(L"Pin to Quick access", L"pintohome");
        addSeparator();
        addCommand(L"Properties", L"properties", kModelNone, L"@glyph:E713");
        addSeparator();
        addFallback();
        return model;
    }

    if (scope == Scope::Folders) {
        addCommand(openLabel, L"open", kModelDefault);
        addCommand(L"Open in new window", L"opennew");
        addCommand(L"Pin to Quick access", L"pintohome");
        addSeparator();
    } else {
        addCommand(openLabel, L"open", kModelDefault);
        addCommand(L"Open with", L"openwith", kModelNone, L"@glyph:E8E5");
        addSeparator();
    }

    addCommand(L"Cut", L"cut", kModelNone, L"@glyph:E8C6");
    addCommand(L"Copy", L"copy", kModelNone, L"@glyph:E8C8");
    addViewAction(L"Rename", L"rename", ViewAction::Rename, multiDisabled,
                  L"@glyph:E8AC");
    addCommand(L"Delete", L"delete", kModelNone, L"@glyph:E74D");
    addSeparator();
    addCommand(L"Create shortcut", L"createshortcut", multiDisabled);
    addSubmenu(L"Send to");
    addCommand(L"Copy as path", L"copyaspath");
    addSeparator();
    addCommand(L"Properties", L"properties", kModelNone, L"@glyph:E713");
    addSeparator();
    addFallback();

    return model;
}


// Merges cached extension items into a freshly built core model: core items
// keep their order, native invocation descriptors are adopted for matching
// items, duplicates are dropped, cached separators are skipped, and the
// fallback item stays last.
// Items that belong with the Open group at the top of a folder menu. Used to
// place shell-only entries (for example "Open in new tab") correctly instead
// of appending them after the tail group.
bool IsOpenGroupItem(const MenuItem& item) {
    static const wchar_t* kOpenVerbs[] = {
        L"open",   L"opennew",  L"opennewtab", L"opennewwindow",
        L"openwith", L"openas", L"edit",       L"print",
        L"printto", L"preview",
    };
    if (!item.canonicalVerb.empty()) {
        for (const wchar_t* verb : kOpenVerbs) {
            if (_wcsicmp(item.canonicalVerb.c_str(), verb) == 0) {
                return true;
            }
        }
    }
    const std::wstring normalized = NormalizeMenuLabel(item.label);
    return normalized.rfind(L"Open", 0) == 0;
}

MenuModel MergeCoreWithCached(const MenuModel& core, const MenuModel& cached) {
    MenuModel result = core;

    MenuItem fallback{};
    bool hadFallback = false;
    if (!result.items.empty() && result.items.back().action == ActionKind::Fallback) {
        fallback = result.items.back();
        result.items.pop_back();
        hadFallback = true;
    }

    auto matches = [](const MenuItem& left, const MenuItem& right) {
        if (LabelsMatchIgnoreCase(left.label, right.label)) {
            return true;
        }
        return !left.canonicalVerb.empty() && left.canonicalVerb == right.canonicalVerb;
    };

    std::unordered_set<std::wstring> coreLabels;
    std::unordered_set<std::wstring> coreVerbs;
    for (const MenuItem& item : result.items) {
        if (!item.label.empty()) {
            const std::wstring normalized = NormalizeMenuLabel(item.label);
            if (!normalized.empty()) {
                coreLabels.insert(normalized);
            }
        }
        if (!item.canonicalVerb.empty()) {
            coreVerbs.insert(item.canonicalVerb);
        }
    }

    std::vector<MenuItem> added;
    std::vector<MenuItem> addedOpen;
    for (const MenuItem& item : cached.items) {
        if (item.kind == ItemKind::Separator) {
            continue;
        }

        bool matchedCore = false;
        for (MenuItem& coreItem : result.items) {
            // A matching submenu donates its children (e.g. the shell's Send
            // to list) instead of being dropped as a duplicate, which would
            // leave the core submenu empty and pruned.
            if (item.kind == ItemKind::Submenu &&
                coreItem.kind == ItemKind::Submenu &&
                coreItem.children.empty() && matches(coreItem, item)) {
                coreItem.children = item.children;
                matchedCore = true;
                break;
            }
            // Only invokable commands donate a descriptor; a matching submenu
            // has none and would otherwise clear the core verb and point the
            // item at offset 0.
            if (coreItem.action != ActionKind::ShellVerb ||
                item.action != ActionKind::ShellVerb ||
                item.kind != ItemKind::Command || !matches(coreItem, item)) {
                continue;
            }
            // Adopt the native invocation descriptor: the shell rejects some
            // canonical verb strings but its own offsets always dispatch.
            coreItem.canonicalVerb = item.canonicalVerb;
            coreItem.verbOffset = item.verbOffset;
            coreItem.flags |= kModelHasOffset;
            matchedCore = true;
            break;
        }
        if (matchedCore) {
            continue;
        }

        const std::wstring normalizedLabel = NormalizeMenuLabel(item.label);
        if ((!normalizedLabel.empty() && coreLabels.count(normalizedLabel)) ||
            (!item.canonicalVerb.empty() && coreVerbs.count(item.canonicalVerb))) {
            continue;
        }
        if (IsOpenGroupItem(item)) {
            addedOpen.push_back(item);
        } else {
            added.push_back(item);
        }
    }

    if (!addedOpen.empty()) {
        size_t insertAt = 0;
        bool found = false;
        for (size_t i = 0; i < result.items.size(); ++i) {
            if (IsOpenGroupItem(result.items[i])) {
                insertAt = i + 1;
                found = true;
            }
        }
        if (!found) {
            insertAt = std::min<size_t>(1, result.items.size());
        }
        result.items.insert(result.items.begin() + insertAt, addedOpen.begin(),
                            addedOpen.end());
    }

    if (!added.empty()) {
        result.items.insert(result.items.end(), added.begin(), added.end());
    }
    if (hadFallback) {
        if (!added.empty()) {
            MenuItem separator{};
            separator.id = 9999;
            separator.kind = ItemKind::Separator;
            result.items.push_back(std::move(separator));
        }
        result.items.push_back(std::move(fallback));
    }
    return result;
}

}  // namespace cmo

// ===========================================================================
// [CMO:Cache] In-memory and persistent menu model cache.
// ===========================================================================
namespace cmo {

constexpr uint32_t kCacheMagic = 0x434F4D4F;  // "COMO"
constexpr uint32_t kCacheVersion = 15;
constexpr uint32_t kMaxCacheEntries = 1024;
constexpr uint32_t kMaxModelItems = 4096;
constexpr uint32_t kMaxMenuDepth = 16;
constexpr uint32_t kMaxStringChars = 65536;

std::wstring CacheFilePath() {
    wchar_t storagePath[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        return L"";
    }
    return std::wstring(storagePath) + L"\\menu-cache.bin";
}

namespace {

void WriteU8(std::vector<uint8_t>& out, uint8_t value) {
    out.push_back(value);
}

void WriteU32(std::vector<uint8_t>& out, uint32_t value) {
    out.push_back(static_cast<uint8_t>(value & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void WriteU64(std::vector<uint8_t>& out, uint64_t value) {
    WriteU32(out, static_cast<uint32_t>(value & 0xFFFFFFFFu));
    WriteU32(out, static_cast<uint32_t>(value >> 32));
}

void WriteString(std::vector<uint8_t>& out, const std::wstring& text) {
    WriteU32(out, static_cast<uint32_t>(text.size()));
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(text.data());
    out.insert(out.end(), bytes, bytes + text.size() * sizeof(wchar_t));
}

void WriteBytes(std::vector<uint8_t>& out, const std::vector<uint8_t>& bytes) {
    WriteU32(out, static_cast<uint32_t>(bytes.size()));
    out.insert(out.end(), bytes.begin(), bytes.end());
}

void WriteItem(std::vector<uint8_t>& out, const MenuItem& item) {
    WriteU32(out, item.id);
    WriteU8(out, static_cast<uint8_t>(item.kind));
    WriteU8(out, static_cast<uint8_t>(item.action));
    WriteU32(out, item.flags);
    WriteU32(out, static_cast<uint32_t>(item.viewAction));
    WriteU32(out, item.verbOffset);
    WriteString(out, item.label);
    WriteString(out, item.canonicalVerb);
    WriteString(out, item.iconRef);
    WriteString(out, item.targetPath);
    WriteBytes(out, item.iconPixels);
    WriteU32(out, static_cast<uint32_t>(item.children.size()));
    for (const MenuItem& child : item.children) {
        WriteItem(out, child);
    }
}

void WriteSignature(std::vector<uint8_t>& out, const ContextSignature& sig) {
    WriteU8(out, static_cast<uint8_t>(sig.scope));
    WriteString(out, sig.typeKey);
    WriteU8(out, static_cast<uint8_t>(sig.shape));
    WriteU8(out, static_cast<uint8_t>(sig.variant));
}

void WriteModel(std::vector<uint8_t>& out, const MenuModel& model) {
    WriteSignature(out, model.sig);
    WriteU32(out, model.flags);
    WriteU64(out, model.sourceStamp);
    WriteU32(out, static_cast<uint32_t>(model.handlerModules.size()));
    for (const std::wstring& module : model.handlerModules) {
        WriteString(out, module);
    }
    WriteU32(out, static_cast<uint32_t>(model.items.size()));
    for (const MenuItem& item : model.items) {
        WriteItem(out, item);
    }
}

uint32_t ReadU32At(std::span<const uint8_t> data, size_t offset) {
    return static_cast<uint32_t>(data[offset]) |
           (static_cast<uint32_t>(data[offset + 1]) << 8) |
           (static_cast<uint32_t>(data[offset + 2]) << 16) |
           (static_cast<uint32_t>(data[offset + 3]) << 24);
}

uint32_t Crc32(std::span<const uint8_t> data) {
    uint32_t crc = 0xFFFFFFFFu;
    for (uint8_t byte : data) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

uint64_t ComputeModuleStamp(const std::vector<std::wstring>& modules) {
    uint64_t stamp = 1469598103934665603ULL;
    for (const std::wstring& path : modules) {
        WIN32_FILE_ATTRIBUTE_DATA data = {};
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data)) {
            continue;
        }
        const uint64_t size =
            (static_cast<uint64_t>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
        const uint64_t modified =
            (static_cast<uint64_t>(data.ftLastWriteTime.dwHighDateTime) << 32) |
            data.ftLastWriteTime.dwLowDateTime;
        stamp = HashCombine(stamp, HashString(path));
        stamp = HashCombine(stamp, size);
        stamp = HashCombine(stamp, modified);
    }
    return stamp;
}

class Reader {
public:
    explicit Reader(std::span<const uint8_t> data) : data_(data) {}

    bool ReadU8(uint8_t& value) {
        if (pos_ + 1 > data_.size()) {
            return false;
        }
        value = data_[pos_++];
        return true;
    }

    bool ReadU32(uint32_t& value) {
        if (pos_ + 4 > data_.size()) {
            return false;
        }
        value = ReadU32At(data_, pos_);
        pos_ += 4;
        return true;
    }

    bool ReadU64(uint64_t& value) {
        uint32_t low = 0;
        uint32_t high = 0;
        if (!ReadU32(low) || !ReadU32(high)) {
            return false;
        }
        value = (static_cast<uint64_t>(high) << 32) | low;
        return true;
    }

    bool ReadString(std::wstring& text) {
        uint32_t length = 0;
        if (!ReadU32(length) || length > kMaxStringChars) {
            return false;
        }
        const size_t bytes = static_cast<size_t>(length) * sizeof(wchar_t);
        if (pos_ + bytes > data_.size()) {
            return false;
        }
        // Copy into aligned storage; the serialized offset may be unaligned.
        text.resize(length);
        if (bytes > 0) {
            memcpy(text.data(), data_.data() + pos_, bytes);
        }
        pos_ += bytes;
        return true;
    }

    bool ReadBytes(std::vector<uint8_t>& bytes, uint32_t maxBytes) {
        uint32_t length = 0;
        if (!ReadU32(length) || length > maxBytes) {
            return false;
        }
        if (pos_ + length > data_.size()) {
            return false;
        }
        bytes.assign(data_.begin() + pos_, data_.begin() + pos_ + length);
        pos_ += length;
        return true;
    }

    bool ReadSignature(ContextSignature& sig) {
        uint8_t scope = 0;
        uint8_t shape = 0;
        uint8_t variant = 0;
        if (!ReadU8(scope) || !ReadString(sig.typeKey) || !ReadU8(shape) ||
            !ReadU8(variant)) {
            return false;
        }
        sig.scope = static_cast<Scope>(scope);
        sig.shape = static_cast<Shape>(shape);
        sig.variant = static_cast<Variant>(variant);
        return true;
    }

    bool ReadItem(MenuItem& item, uint32_t depth = 0) {
        if (depth > kMaxMenuDepth) {
            return false;
        }
        uint8_t kind = 0;
        uint8_t action = 0;
        uint32_t viewAction = 0;
        uint32_t childCount = 0;
        if (!ReadU32(item.id) || !ReadU8(kind) || !ReadU8(action) ||
            !ReadU32(item.flags) || !ReadU32(viewAction) ||
            !ReadU32(item.verbOffset) || !ReadString(item.label) ||
            !ReadString(item.canonicalVerb) || !ReadString(item.iconRef) ||
            !ReadString(item.targetPath) || !ReadBytes(item.iconPixels, 4096) ||
            !ReadU32(childCount)) {
            return false;
        }
        item.kind = static_cast<ItemKind>(kind);
        item.action = static_cast<ActionKind>(action);
        item.viewAction = viewAction;
        for (uint32_t i = 0; i < childCount; ++i) {
            MenuItem child{};
            if (!ReadItem(child, depth + 1)) {
                return false;
            }
            item.children.push_back(std::move(child));
        }
        return true;
    }

    bool ReadModel(MenuModel& model) {
        uint32_t modelFlags = 0;
        uint32_t moduleCount = 0;
        uint32_t itemCount = 0;
        if (!ReadSignature(model.sig) || !ReadU32(modelFlags) ||
            !ReadU64(model.sourceStamp) || !ReadU32(moduleCount) ||
            moduleCount > kMaxCacheEntries) {
            return false;
        }
        model.flags = modelFlags;
        for (uint32_t i = 0; i < moduleCount; ++i) {
            std::wstring module;
            if (!ReadString(module)) {
                return false;
            }
            model.handlerModules.push_back(std::move(module));
        }
        if (!ReadU32(itemCount) || itemCount > kMaxModelItems) {
            return false;
        }
        for (uint32_t i = 0; i < itemCount; ++i) {
            MenuItem item{};
            if (!ReadItem(item)) {
                return false;
            }
            model.items.push_back(std::move(item));
        }
        return true;
    }

private:
    std::span<const uint8_t> data_;
    size_t pos_ = 0;
};

}  // namespace

class Cache {
public:
    // NavPane menus can change from outside (pin/unpin, renames), so those
    // entries expire; file menus are stable and stay.
    static constexpr uint64_t kNavPaneCacheTtlMs = 60000;

    std::optional<MenuModel> Find(const ContextSignature& signature) {
        return FindAt(signature, GetTickCount64());
    }

    // Expiry hook (also used by tests): a NavPane entry older than the TTL is
    // treated as absent and dropped.
    std::optional<MenuModel> FindAt(const ContextSignature& signature,
                                    uint64_t nowMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = entries_.find(signature.Hash());
        if (it == entries_.end() || !(it->second.model.sig == signature)) {
            return std::nullopt;
        }
        if (it->second.model.sig.scope == Scope::NavPane &&
            nowMs - it->second.putTick > kNavPaneCacheTtlMs) {
            entries_.erase(it);
            dirty_ = true;
            return std::nullopt;
        }
        it->second.lastUsed = nowMs;
        return it->second.model;
    }

    void Remove(const ContextSignature& signature) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (entries_.erase(signature.Hash()) > 0) {
            dirty_ = true;
        }
    }

    bool Has(const ContextSignature& signature) {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.find(signature.Hash()) != entries_.end();
    }

    void Put(MenuModel model) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = entries_.find(model.sig.Hash());
        if (it != entries_.end() && (model.flags & kModelWarmup) &&
            !(it->second.model.flags & kModelWarmup)) {
            // A live model is authoritative; never let warm-up overwrite it.
            return;
        }
        PutLocked(std::move(model));
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
        dirty_ = true;
    }

#ifdef CMO_TESTING
    void SetMaxEntries(size_t maxEntries);
#endif

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.size();
    }

    std::vector<uint8_t> Serialize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return SerializeLocked();
    }

    static bool Deserialize(std::span<const uint8_t> data, Cache& out) {
        std::vector<MenuModel> models;
        if (!DeserializeModels(data, models)) {
            return false;
        }
        out.ReplaceAll(std::move(models));
        return true;
    }

    bool Load(const std::wstring& path) {
        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return false;
        }

        LARGE_INTEGER size = {};
        if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
            size.QuadPart > 16 * 1024 * 1024) {
            CloseHandle(file);
            return false;
        }

        std::vector<uint8_t> data(static_cast<size_t>(size.QuadPart));
        DWORD read = 0;
        BOOL ok = ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &read,
                           nullptr);
        CloseHandle(file);
        if (!ok || read != data.size()) {
            return false;
        }

        std::vector<MenuModel> models;
        if (!DeserializeModels(data, models)) {
            return false;
        }
        ReplaceAll(std::move(models));
        return true;
    }

    bool Save(const std::wstring& path) {
        std::vector<uint8_t> data = Serialize();
        const std::wstring tempPath = path + L".tmp";

        HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, nullptr,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD written = 0;
        BOOL ok = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written,
                            nullptr);
        CloseHandle(file);
        if (!ok || written != data.size()) {
            DeleteFileW(tempPath.c_str());
            return false;
        }

        if (!MoveFileExW(tempPath.c_str(), path.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            DeleteFileW(tempPath.c_str());
            return false;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        dirty_ = false;
        lastSaveTick_ = GetTickCount64();
        return true;
    }

    bool MaybeSave(const std::wstring& path) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!dirty_ || GetTickCount64() - lastSaveTick_ < 5000) {
                return true;
            }
        }
        return Save(path);
    }

    // Clears the cache when any recorded handler module changed on disk.
    bool RevalidateStamps() {
        struct Check {
            uint64_t stamp;
            std::vector<std::wstring> modules;
        };
        std::vector<Check> checks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& pair : entries_) {
                if (pair.second.model.handlerModules.empty()) {
                    continue;
                }
                checks.push_back(
                    {pair.second.model.sourceStamp, pair.second.model.handlerModules});
            }
        }

        bool stale = false;
        for (const Check& check : checks) {
            if (ComputeModuleStamp(check.modules) != check.stamp) {
                stale = true;
                break;
            }
        }
        if (stale) {
            std::lock_guard<std::mutex> lock(mutex_);
            entries_.clear();
            dirty_ = true;
        }
        return stale;
    }

private:
    struct Entry {
        MenuModel model;
        uint64_t lastUsed = 0;
        uint64_t putTick = 0;
    };

    void PutLocked(MenuModel model) {
        Entry entry{};
        entry.model = std::move(model);
        entry.lastUsed = GetTickCount64();
        entry.putTick = entry.lastUsed;
        entries_[entry.model.sig.Hash()] = std::move(entry);
        EvictIfNeededLocked();
        dirty_ = true;
    }

    void ReplaceAll(std::vector<MenuModel> models) {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
        for (MenuModel& model : models) {
            Entry entry{};
            entry.model = std::move(model);
            entry.lastUsed = GetTickCount64();
            entry.putTick = entry.lastUsed;
            entries_[entry.model.sig.Hash()] = std::move(entry);
        }
        EvictIfNeededLocked();
        dirty_ = false;
    }

    void EvictIfNeededLocked() {
        while (entries_.size() > maxEntries_ && !entries_.empty()) {
            auto oldest = entries_.begin();
            for (auto it = entries_.begin(); it != entries_.end(); ++it) {
                if (it->second.lastUsed < oldest->second.lastUsed) {
                    oldest = it;
                }
            }
            entries_.erase(oldest);
        }
    }

    std::vector<uint8_t> SerializeLocked() const {
        std::vector<uint8_t> out;
        WriteU32(out, kCacheMagic);
        WriteU32(out, kCacheVersion);
        uint32_t count = 0;
        for (const auto& pair : entries_) {
            if (pair.second.model.sig.scope != Scope::NavPane) {
                ++count;
            }
        }
        WriteU32(out, count);
        for (const auto& pair : entries_) {
            // NavPane menus are session-only: their state can change from
            // outside (pin/unpin), so they are never restored across runs.
            if (pair.second.model.sig.scope == Scope::NavPane) {
                continue;
            }
            WriteModel(out, pair.second.model);
        }
        WriteU32(out, Crc32(out));
        return out;
    }

    static bool DeserializeModels(std::span<const uint8_t> data,
                                  std::vector<MenuModel>& out) {
        if (data.size() < 12) {
            return false;
        }
        if (ReadU32At(data, 0) != kCacheMagic) {
            return false;
        }
        if (ReadU32At(data, 4) != kCacheVersion) {
            return false;
        }

        const size_t payloadSize = data.size() - 4;
        if (ReadU32At(data, payloadSize) != Crc32(data.subspan(0, payloadSize))) {
            return false;
        }

        Reader reader(data.subspan(0, payloadSize));
        uint32_t magic = 0;
        uint32_t version = 0;
        uint32_t entryCount = 0;
        if (!reader.ReadU32(magic) || !reader.ReadU32(version) ||
            !reader.ReadU32(entryCount) || entryCount > kMaxCacheEntries) {
            return false;
        }

        std::vector<MenuModel> models;
        for (uint32_t i = 0; i < entryCount; ++i) {
            MenuModel model{};
            if (!reader.ReadModel(model)) {
                return false;
            }
            models.push_back(std::move(model));
        }

        out = std::move(models);
        return true;
    }

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, Entry> entries_;
    size_t maxEntries_ = 256;
    bool dirty_ = false;
    uint64_t lastSaveTick_ = 0;
};

inline Cache g_cache;

}  // namespace cmo

// ===========================================================================
// [CMO:Classify] Popup owner classification.
// ===========================================================================
namespace cmo {

enum class ShellViewKind : uint8_t { None, Desktop, ShellDefView, NavPane, Other };

inline ShellViewKind ClassifyClassChain(const std::vector<std::wstring>& ancestors,
                                        bool isDesktopRoot) {
    if (isDesktopRoot) {
        return ShellViewKind::Desktop;
    }
    for (const std::wstring& name : ancestors) {
        if (name == L"SHELLDLL_DefView") {
            return ShellViewKind::ShellDefView;
        }
    }
    for (const std::wstring& name : ancestors) {
        if (name == L"NamespaceTreeControl") {
            return ShellViewKind::NavPane;
        }
    }
    return ShellViewKind::None;
}

inline bool IsReplaceableKind(ShellViewKind kind) {
    return kind == ShellViewKind::Desktop || kind == ShellViewKind::ShellDefView ||
           kind == ShellViewKind::NavPane;
}

// Maps the popup owner to a scope. A replaceable owner with an empty
// selection is a background menu; Desktop background and desktop icons share
// the same owner kind.
inline Scope ScopeFromKind(ShellViewKind kind, bool background) {
    switch (kind) {
        case ShellViewKind::Desktop:
            return background ? Scope::Desktop : Scope::Files;
        case ShellViewKind::ShellDefView:
            return background ? Scope::Background : Scope::Files;
        case ShellViewKind::NavPane:
            return Scope::NavPane;
        default:
            return Scope::Other;
    }
}

inline bool AllPathsAreDrives(const std::vector<std::wstring>& paths) {
    if (paths.empty()) {
        return false;
    }
    for (const std::wstring& path : paths) {
        if (path.size() != 3 || path[1] != L':' ||
            (path[2] != L'\\' && path[2] != L'/')) {
            return false;
        }
    }
    return true;
}

inline bool IsFilesystemContext(bool folderIsFilesystem, bool allItemsAreFilesystem) {
    return folderIsFilesystem && allItemsAreFilesystem;
}

// Refines a files-scope selection using shell attributes: drive roots first,
// then folders, otherwise files.
inline Scope RefineScope(Scope scope, bool allFolders, bool allDrives) {
    if (scope != Scope::Files) {
        return scope;
    }
    if (allDrives) {
        return Scope::Drive;
    }
    if (allFolders) {
        return Scope::Folders;
    }
    return Scope::Files;
}

// True when two path sets contain the same items in any order.
inline bool PathSetsEqual(std::vector<std::wstring> left,
                          std::vector<std::wstring> right) {
    if (left.size() != right.size()) {
        return false;
    }
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());
    return left == right;
}

inline bool ShouldShowNativeReplay(uint32_t modelFlags) {
    return (modelFlags & kModelOwnerDraw) != 0;
}

// True when the user's apps use the dark theme.
inline bool IsDarkThemeActive() {
    DWORD lightTheme = 1;
    DWORD size = sizeof(lightTheme);
    HKEY key = nullptr;
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
            KEY_READ, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&lightTheme), &size);
        RegCloseKey(key);
    }
    return lightTheme == 0;
}

// The actual menu background color for the owner window's theme. The theme
// lookup can return light colors even in dark mode, so the result is sanity
// checked against the registry theme setting.
inline COLORREF MenuBackgroundColor(HWND owner) {
    COLORREF color = 0;
    bool haveColor = false;
    HTHEME theme = OpenThemeData(owner, L"Menu");
    if (theme) {
        if (SUCCEEDED(GetThemeColor(theme, MENU_POPUPBACKGROUND, 0, TMT_FILLCOLOR,
                                    &color))) {
            haveColor = true;
        }
        CloseThemeData(theme);
    }
    if (!haveColor) {
        color = GetSysColor(COLOR_MENU);
    }

    const bool darkTheme = IsDarkThemeActive();
    const int luminance =
        (GetRValue(color) * 30 + GetGValue(color) * 59 + GetBValue(color) * 11) / 100;
    if (darkTheme && luminance > 128) {
        return RGB(44, 44, 44);
    }
    if (!darkTheme && luminance < 128) {
        return GetSysColor(COLOR_MENU);
    }
    return color;
}

inline bool IsDesktopRootWindow(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }
    if (root == GetShellWindow()) {
        return true;
    }
    wchar_t className[64] = {};
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return false;
    }
    if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
        return false;
    }
    return FindWindowExW(root, nullptr, L"SHELLDLL_DefView", nullptr) != nullptr;
}

inline ShellViewKind ClassifyOwner(HWND owner) {
    if (!owner) {
        return ShellViewKind::None;
    }
    std::vector<std::wstring> ancestors;
    for (HWND window = owner; window; window = GetAncestor(window, GA_PARENT)) {
        if (IsDesktopRootWindow(window)) {
            return ClassifyClassChain(ancestors, true);
        }
        wchar_t className[128] = {};
        if (GetClassNameW(window, className, ARRAYSIZE(className))) {
            ancestors.emplace_back(className);
        }
    }
    return ClassifyClassChain(ancestors, false);
}

// Finds the namespace tree view under the nav-pane popup owner (the owner is
// either the tree itself or the NamespaceTreeControl hosting it).
HWND FindNamespaceTreeWindow(HWND owner) {
    if (!owner) {
        return nullptr;
    }
    wchar_t className[64] = {};
    if (GetClassNameW(owner, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"SysTreeView32") == 0) {
        return owner;
    }
    HWND found = nullptr;
    EnumChildWindows(
        owner,
        [](HWND child, LPARAM param) -> BOOL {
            wchar_t name[64] = {};
            if (GetClassNameW(child, name, ARRAYSIZE(name)) &&
                _wcsicmp(name, L"SysTreeView32") == 0) {
                *reinterpret_cast<HWND*>(param) = child;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&found));
    return found;
}

// A stable key for the selected nav-pane node: the shell's tree stores the
// item PIDL in lParam; its desktop-absolute parsing name is stable across
// sessions. Falls back to the ancestor text chain when the PIDL is not
// available. Returns empty when no node can be identified (no caching then).
std::wstring NavPaneNodeKey(HWND owner) {
    HWND tree = FindNamespaceTreeWindow(owner);
    if (!tree) {
        return L"";
    }
    const HTREEITEM item = reinterpret_cast<HTREEITEM>(
        SendMessageW(tree, TVM_GETNEXTITEM, TVGN_CARET, 0));
    if (!item) {
        return L"";
    }
    wchar_t text[512] = {};
    TVITEMW info = {};
    info.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT;
    info.hItem = item;
    info.pszText = text;
    info.cchTextMax = ARRAYSIZE(text);
    if (!SendMessageW(tree, TVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&info))) {
        return L"";
    }
    if (info.lParam) {
        // Explorer's namespace tree stores a PIDL here.
        PWSTR parsing = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(
                reinterpret_cast<PCIDLIST_ABSOLUTE>(info.lParam),
                SIGDN_DESKTOPABSOLUTEPARSING, &parsing)) &&
            parsing && parsing[0]) {
            std::wstring key = parsing;
            CoTaskMemFree(parsing);
            return key;
        }
        if (parsing) {
            CoTaskMemFree(parsing);
        }
    }
    // Fallback: the display text chain (parent\...\item).
    std::wstring key = text;
    HTREEITEM parent = reinterpret_cast<HTREEITEM>(
        SendMessageW(tree, TVM_GETNEXTITEM, TVGN_PARENT,
                     reinterpret_cast<LPARAM>(item)));
    for (int depth = 0; parent && depth < 16; ++depth) {
        wchar_t parentText[512] = {};
        TVITEMW parentInfo = {};
        parentInfo.mask = TVIF_HANDLE | TVIF_TEXT;
        parentInfo.hItem = parent;
        parentInfo.pszText = parentText;
        parentInfo.cchTextMax = ARRAYSIZE(parentText);
        if (SendMessageW(tree, TVM_GETITEMW, 0,
                         reinterpret_cast<LPARAM>(&parentInfo))) {
            key = std::wstring(parentText) + L"\\" + key;
        }
        parent = reinterpret_cast<HTREEITEM>(
            SendMessageW(tree, TVM_GETNEXTITEM, TVGN_PARENT,
                         reinterpret_cast<LPARAM>(parent)));
    }
    return key;
}

}  // namespace cmo

// ===========================================================================
// [CMO:Discovery] Real shell menu population capture and replay.
// ===========================================================================
namespace cmo {

struct PendingCapture {
    IContextMenu* obj = nullptr;
    HMENU menu = nullptr;  // the menu this population was deferred into
    UINT indexMenu = 0;
    UINT idCmdFirst = 0;
    UINT idCmdLast = 0;
    UINT flags = 0;
    HWND owner = nullptr;
    ULONGLONG tick = 0;
    DWORD ownerThread = 0;
    bool used = false;

    // Populated once per open; reused for discovery and the native fallback.
    HMENU populatedMenu = nullptr;
    bool populated = false;
    IContextMenu3* contextMenu3 = nullptr;
    IContextMenu2* contextMenu2 = nullptr;
    std::vector<std::wstring> handlerModules;
    uint64_t sourceStamp = 0;
    bool discoveryDone = false;
    bool menuInitialized = false;
};

// Releases every resource a capture owns.
void ReleaseCapture(PendingCapture& capture) {
    if (capture.populatedMenu) {
        DestroyMenu(capture.populatedMenu);
        capture.populatedMenu = nullptr;
    }
    if (capture.contextMenu3) {
        capture.contextMenu3->Release();
        capture.contextMenu3 = nullptr;
    }
    if (capture.contextMenu2) {
        capture.contextMenu2->Release();
        capture.contextMenu2 = nullptr;
    }
    if (capture.obj) {
        capture.obj->Release();
        capture.obj = nullptr;
    }
    capture.populated = false;
}

class PendingQueue {
public:
    void Push(PendingCapture capture) {
        std::vector<PendingCapture> victims;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            capture.ownerThread = GetCurrentThreadId();
            if (capture.tick == 0) {
                capture.tick = GetTickCount64();
            }
            ExpireOwnedLocked(capture.ownerThread, GetTickCount64(), 60000, victims);
            if (captures_.size() >= kMaxCaptures) {
                // Evict only this thread's oldest capture; another thread's
                // capture must be released on its own thread.
                EvictOldestOwnedLocked(capture.ownerThread, victims);
            }
            captures_.push_back(capture);
        }
        ReleaseAll(victims);
    }

    // Takes the capture whose deferred menu is the one being shown. A capture
    // is only ever handed back to the thread that parked it. The navigation
    // pane can defer more than one menu per click, so for it a recent
    // same-thread capture is accepted even when the handle differs; stale or
    // unrelated captures are released instead of being replayed into a menu
    // they do not belong to.
    bool TakeForMenu(HMENU menu, PendingCapture& out, bool navFallback = false) {
        std::vector<PendingCapture> victims;
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const DWORD self = GetCurrentThreadId();
            const ULONGLONG now = GetTickCount64();

            size_t index = kNoIndex;
            for (size_t i = 0; i < captures_.size(); ++i) {
                if (captures_[i].ownerThread == self && captures_[i].menu == menu) {
                    index = i;
                    break;
                }
            }
            if (index == kNoIndex && navFallback) {
                for (size_t i = captures_.size(); i-- > 0;) {
                    if (captures_[i].ownerThread == self &&
                        now - captures_[i].tick <= kNavFallbackFreshMs) {
                        index = i;
                        break;
                    }
                }
            }
            if (index != kNoIndex) {
                out = captures_[index];
                captures_.erase(captures_.begin() + static_cast<std::ptrdiff_t>(index));
                found = true;
            }
            MoveOwnedLocked(self, victims);
        }
        ReleaseAll(victims);
        return found;
    }

    // Takes the newest capture owned by the calling thread. Returns false when
    // nothing is pending for it.
    bool Take(PendingCapture& out) {
        std::vector<PendingCapture> victims;
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const DWORD self = GetCurrentThreadId();
            for (size_t i = captures_.size(); i-- > 0;) {
                if (captures_[i].ownerThread == self) {
                    out = captures_[i];
                    captures_.erase(captures_.begin() + static_cast<std::ptrdiff_t>(i));
                    found = true;
                    break;
                }
            }
            if (found) {
                MoveOwnedLocked(self, victims);
            }
        }
        ReleaseAll(victims);
        return found;
    }

    // Releases the calling thread's captures; called from that thread's
    // control-window teardown. Captures on threads that cannot be reached are
    // intentionally leaked rather than released from the wrong thread.
    void ReleaseOwnedBy(DWORD ownerThread) {
        std::vector<PendingCapture> victims;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            MoveOwnedLocked(ownerThread, victims);
        }
        ReleaseAll(victims);
    }

    // Drops the calling thread's captures older than maxAgeMs.
    void ExpireOlderThan(ULONGLONG now, ULONGLONG maxAgeMs) {
        std::vector<PendingCapture> victims;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ExpireOwnedLocked(GetCurrentThreadId(), now, maxAgeMs, victims);
        }
        ReleaseAll(victims);
    }

    bool HasPending() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !captures_.empty();
    }

private:
    static constexpr size_t kMaxCaptures = 4;
    static constexpr size_t kNoIndex = static_cast<size_t>(-1);
    static constexpr ULONGLONG kNavFallbackFreshMs = 1000;

    // Moves the given thread's captures into `victims` so they can be released
    // after the lock is dropped.
    void MoveOwnedLocked(DWORD ownerThread, std::vector<PendingCapture>& victims) {
        for (size_t i = 0; i < captures_.size();) {
            if (captures_[i].ownerThread == ownerThread) {
                victims.push_back(captures_[i]);
                captures_.erase(captures_.begin() + static_cast<std::ptrdiff_t>(i));
            } else {
                ++i;
            }
        }
    }

    void ExpireOwnedLocked(DWORD ownerThread, ULONGLONG now, ULONGLONG maxAgeMs,
                           std::vector<PendingCapture>& victims) {
        for (size_t i = 0; i < captures_.size();) {
            if (captures_[i].ownerThread == ownerThread &&
                now - captures_[i].tick > maxAgeMs) {
                victims.push_back(captures_[i]);
                captures_.erase(captures_.begin() + static_cast<std::ptrdiff_t>(i));
            } else {
                ++i;
            }
        }
    }

    void EvictOldestOwnedLocked(DWORD ownerThread,
                                std::vector<PendingCapture>& victims) {
        for (size_t i = 0; i < captures_.size(); ++i) {
            if (captures_[i].ownerThread == ownerThread) {
                victims.push_back(captures_[i]);
                captures_.erase(captures_.begin() + static_cast<std::ptrdiff_t>(i));
                return;
            }
        }
    }

    static void ReleaseAll(std::vector<PendingCapture>& victims) {
        for (PendingCapture& capture : victims) {
            ReleaseCapture(capture);
        }
        victims.clear();
    }

    mutable std::mutex mutex_;
    std::vector<PendingCapture> captures_;
};

// One queue shared by all threads: a capture is only ever handed back to the
// thread that parked it, and it is released from that thread's teardown (or
// leaked if the thread is gone). It is a global rather than thread_local so no
// destructor can run after the module is unmapped.
inline std::optional<PendingQueue> g_pending{std::in_place};

void ReleasePendingCapturesForCurrentThread() {
    g_pending->ReleaseOwnedBy(GetCurrentThreadId());
}

// Undocumented shell32 bit set for every menu that is actually shown as a
// popup. Desktop menus (icons and background) pass the same flags as Explorer
// views but omit CMF_EXPLORE; they still carry this bit, while verb-state
// queries and submenu builds do not.
constexpr UINT kCmfPopupMenu = 0x00020000;

// Ctrl bypasses the mod entirely (when bypass keys are enabled) so the
// untouched native menu can show. The decision sticks briefly so "Show more
// options" from the untouched modern menu also stays untouched. The window is
// armed only where the untouched modern menu is actually let through (the
// Windows 11 suppression hooks); everywhere else the bypass is read-only, so
// unrelated Ctrl use (double-click, clipboard verbs, multi-select queries)
// cannot arm it. Shift is a separate, earlier bypass: while it is held the
// menu is never deferred, so Explorer builds and shows its own stock menu.
constexpr ULONGLONG kCtrlBypassWindowMs = 5000;

inline thread_local ULONGLONG g_ctrlBypassUntil = 0;

bool CtrlBypassKeyDown() {
    return g_settings->enableShiftBypass &&
           (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
}

// Arms the sticky window; only the Windows 11 suppression hooks call this,
// right where the untouched modern menu is shown.
bool ArmCtrlBypass() {
    if (!CtrlBypassKeyDown()) {
        return false;
    }
    g_ctrlBypassUntil = GetTickCount64() + kCtrlBypassWindowMs;
    return true;
}

// Read-only: Ctrl held now, or a window armed earlier is still open.
bool CtrlBypassActive() {
    return CtrlBypassKeyDown() || GetTickCount64() < g_ctrlBypassUntil;
}

// Shift is checked before the menu is built so the shell populates and shows
// its own menu untouched, with no capture or replay. That keeps
// Shift+right-click bit-for-bit stock, including entries the shell menu alone
// cannot reproduce (view commands, Explorer-added items).
bool ShiftBypassHeld() {
    return g_settings->enableShiftBypass &&
           (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}

// Only main popup menus are deferred. CMF_DEFAULTONLY is the default-verb
// resolution used by double-click/open; CMF_NOVERBS builds submenus such as
// Send to; CMF_VERBSONLY builds verb-only menus. Those never show a popup we
// could replace and must reach the shell untouched. The bypass states are
// passed in so this stays a pure predicate: non-popup calls cannot arm the
// Ctrl bypass window (only the Windows 11 suppression hooks do).
bool ShouldDeferContextMenu(UINT flags, bool shiftBypassActive,
                            bool ctrlBypassActive) {
    if (flags & (CMF_DEFAULTONLY | CMF_NOVERBS | CMF_VERBSONLY)) {
        return false;
    }
    if ((flags & (CMF_EXPLORE | kCmfPopupMenu)) == 0) {
        return false;
    }
    return !shiftBypassActive && !ctrlBypassActive;
}

using QueryContextMenu_t =
    HRESULT(STDMETHODCALLTYPE*)(IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
inline QueryContextMenu_t QueryContextMenu_Original = nullptr;

HRESULT STDMETHODCALLTYPE QueryContextMenu_Hook(IContextMenu* pThis, HMENU hmenu,
                                                UINT indexMenu, UINT idCmdFirst,
                                                UINT idCmdLast, UINT uFlags) {
    if (!ShouldDeferContextMenu(uFlags, ShiftBypassHeld(), CtrlBypassActive())) {
        if (g_settings->debugLogging) Wh_Log(L"QueryContextMenu pass-through: flags=%08X", uFlags);
        return QueryContextMenu_Original(pThis, hmenu, indexMenu, idCmdFirst, idCmdLast,
                                         uFlags);
    }

    PendingCapture capture{};
    capture.obj = pThis;
    capture.menu = hmenu;
    capture.indexMenu = indexMenu;
    capture.idCmdFirst = idCmdFirst;
    capture.idCmdLast = idCmdLast;
    capture.flags = uFlags;
    capture.tick = GetTickCount64();
    if (pThis) {
        pThis->AddRef();
    }
    g_pending->Push(capture);

    if (g_settings->debugLogging) Wh_Log(L"QueryContextMenu deferred: this=%p idFirst=%u flags=%08X", pThis,
           idCmdFirst, uFlags);

    // Report an empty menu; the caller shows it and our TrackPopupMenu* hook
    // substitutes the cached one.
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}

// Runs the real population into the given menu.
void ReplayInto(IContextMenu* obj, HMENU hMenu, UINT indexMenu, UINT idCmdFirst,
                UINT idCmdLast, UINT flags) {
    if (!obj || !QueryContextMenu_Original) {
        return;
    }
    QueryContextMenu_Original(obj, hMenu, indexMenu, idCmdFirst, idCmdLast, flags);
}

uint32_t MapMenuState(UINT state) {
    uint32_t flags = kModelNone;
    if (state & MFS_DISABLED) {
        flags |= kModelDisabled;
    }
    if (state & MFS_CHECKED) {
        flags |= kModelChecked;
    }
    if (state & MFS_DEFAULT) {
        flags |= kModelDefault;
    }
    return flags;
}

// Extension items never inherit the native check state: third-party handlers
// sometimes set MFS_CHECKED for their own rendering (Adobe's shim does), and
// the state is a discovery-time snapshot we cannot refresh after a toggle.
uint32_t MapExtensionMenuState(UINT state) {
    return MapMenuState(state) & ~kModelChecked;
}

// Bitmaps attached with SetMenuItemBitmaps cannot be read back from the
// menu, so record them while the shell populates it. Thread-local because
// population happens on the thread that owns the menu.
thread_local std::unordered_map<uintptr_t, std::unordered_map<UINT, HBITMAP>>
    g_recordedItemBitmaps;

void ClearRecordedItemBitmaps() {
    g_recordedItemBitmaps.clear();
}

HBITMAP LookupRecordedItemBitmap(HMENU menu, UINT id) {
    auto menuIt = g_recordedItemBitmaps.find(reinterpret_cast<uintptr_t>(menu));
    if (menuIt == g_recordedItemBitmaps.end()) {
        return nullptr;
    }
    auto itemIt = menuIt->second.find(id);
    return itemIt == menuIt->second.end() ? nullptr : itemIt->second;
}

using SetMenuItemBitmaps_t = decltype(&SetMenuItemBitmaps);
inline SetMenuItemBitmaps_t SetMenuItemBitmaps_Original = nullptr;

BOOL WINAPI SetMenuItemBitmaps_Hook(HMENU hMenu, UINT uPosition, UINT uFlags,
                                    HBITMAP hBitmapUnchecked, HBITMAP hBitmapChecked) {
    if (hMenu && (hBitmapUnchecked || hBitmapChecked)) {
        UINT id = uPosition;
        if ((uFlags & MF_BYPOSITION) != 0) {
            id = GetMenuItemID(hMenu, uPosition);
        }
        if (id != static_cast<UINT>(-1)) {
            // Bound the recording; menus are normally far smaller than this.
            if (g_recordedItemBitmaps.size() > 64) {
                g_recordedItemBitmaps.clear();
            }
            g_recordedItemBitmaps[reinterpret_cast<uintptr_t>(hMenu)][id] =
                hBitmapUnchecked ? hBitmapUnchecked : hBitmapChecked;
        }
    }
    return SetMenuItemBitmaps_Original
               ? SetMenuItemBitmaps_Original(hMenu, uPosition, uFlags,
                                             hBitmapUnchecked, hBitmapChecked)
               : FALSE;
}

// HBMMENU_* sentinels are -1 or 1..13; real GDI bitmap handles are arbitrary
// 32-bit values (sign-extended on 64-bit Windows) and must be captured.
bool IsSentinelMenuBitmap(HBITMAP bitmap) {
    const INT_PTR value = reinterpret_cast<INT_PTR>(bitmap);
    return value == -1 || (value > 0 && value <= 13);
}

// Copies a shell menu bitmap into BGRA pixels (any bit depth, top-down).
void CaptureBitmapPixels(HBITMAP bitmap, std::vector<uint8_t>& out) {
    out.clear();
    BITMAP bitmapInfo = {};
    if (!GetObjectW(bitmap, sizeof(bitmapInfo), &bitmapInfo) ||
        bitmapInfo.bmWidth <= 0 || bitmapInfo.bmHeight <= 0 ||
        bitmapInfo.bmWidth > 64 || bitmapInfo.bmHeight > 64) {
        return;
    }

    const int width = bitmapInfo.bmWidth;
    const int height = bitmapInfo.bmHeight;

    BITMAPINFO dibInfo = {};
    dibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dibInfo.bmiHeader.biWidth = width;
    dibInfo.bmiHeader.biHeight = height;  // bottom-up
    dibInfo.bmiHeader.biPlanes = 1;
    dibInfo.bmiHeader.biBitCount = 32;
    dibInfo.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> buffer(static_cast<size_t>(width) * height * 4);
    HDC screen = GetDC(nullptr);
    const int lines =
        GetDIBits(screen, bitmap, 0, height, buffer.data(), &dibInfo, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (lines != height) {
        Wh_Log(L"Bitmap capture failed: %dx%d %dbpp", width, height,
               bitmapInfo.bmBitsPixel);
        return;
    }

    // Flip to top-down for the rest of the pipeline.
    out.resize(buffer.size());
    for (int y = 0; y < height; ++y) {
        memcpy(out.data() + static_cast<size_t>(y) * width * 4,
               buffer.data() + static_cast<size_t>(height - 1 - y) * width * 4,
               static_cast<size_t>(width) * 4);
    }
}

// Reads a string value from a Classes subkey; HKCU takes precedence over
// HKLM, mirroring the HKEY_CLASSES_ROOT merge order.
std::wstring ReadClassesString(const std::wstring& subKey, const wchar_t* valueName) {
    const std::wstring fullKey = L"Software\\Classes\\" + subKey;
    for (HKEY root : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        HKEY key = nullptr;
        if (RegOpenKeyExW(root, fullKey.c_str(), 0, KEY_READ, &key) !=
            ERROR_SUCCESS) {
            continue;
        }

        wchar_t buffer[512] = {};
        DWORD size = sizeof(buffer);
        DWORD type = 0;
        std::wstring result;
        if (RegQueryValueExW(key, valueName, nullptr, &type,
                             reinterpret_cast<LPBYTE>(buffer),
                             &size) == ERROR_SUCCESS &&
            (type == REG_SZ || type == REG_EXPAND_SZ) && buffer[0]) {
            result.assign(buffer);
        }
        RegCloseKey(key);
        if (!result.empty()) {
            return result;
        }
    }
    return L"";
}

// Candidate shell keys for a context, most specific first.
std::vector<std::wstring> ShellIconBases(const ContextSignature& signature) {
    std::vector<std::wstring> bases;
    if (signature.scope == Scope::Folders || signature.scope == Scope::Drive) {
        bases.push_back(L"Directory");
        bases.push_back(L"Folder");
        bases.push_back(L"AllFilesystemObjects");
        bases.push_back(L"*");
    } else if (signature.scope == Scope::Background ||
               signature.scope == Scope::Desktop) {
        bases.push_back(L"Directory\\Background");
        bases.push_back(L"DesktopBackground");
    } else {
        const std::wstring& typeKey = signature.typeKey;
        if (!typeKey.empty() && typeKey != L"*" && typeKey != L"mixed" &&
            typeKey[0] == L'.') {
            const std::wstring progId = ReadClassesString(typeKey, nullptr);
            if (!progId.empty()) {
                bases.push_back(progId);
            }
            bases.push_back(typeKey);
            bases.push_back(L"SystemFileAssociations\\" + typeKey);
        }
        bases.push_back(L"*");
        bases.push_back(L"AllFilesystemObjects");
    }
    return bases;
}

// Resolves the icon a static verb registers under HKCR for this context.
std::wstring ResolveRegistryIcon(const ContextSignature& signature,
                                 const std::wstring& verb) {
    if (verb.empty()) {
        return L"";
    }

    for (const std::wstring& base : ShellIconBases(signature)) {
        const std::wstring icon =
            ReadClassesString(base + L"\\shell\\" + verb, L"Icon");
        if (!icon.empty()) {
            return icon;
        }
    }
    return L"";
}

// Resolves strings like "@C:\path\file.dll,-123" to their display text.
std::wstring ResolveIndirectString(const std::wstring& value) {
    if (value.empty() || value[0] != L'@') {
        return value;
    }
    wchar_t buffer[512] = {};
    if (SUCCEEDED(SHLoadIndirectString(value.c_str(), buffer, ARRAYSIZE(buffer),
                                       nullptr))) {
        return buffer;
    }
    return value;
}

// Finds a shell verb key whose display text matches the item label, for
// handlers whose key name differs from the canonical verb.
struct ShellLabelIcon {
    std::wstring display;  // normalized, '&' stripped
    std::wstring icon;
};

// Enumerates the shell verb keys once and maps display text to icons.
std::vector<ShellLabelIcon> BuildShellLabelIcons(
    const ContextSignature& signature) {
    std::vector<ShellLabelIcon> icons;
    for (const std::wstring& base : ShellIconBases(signature)) {
        const std::wstring shellKey = base + L"\\shell";
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          (L"Software\\Classes\\" + shellKey).c_str(), 0, KEY_READ,
                          &key) != ERROR_SUCCESS &&
            RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          (L"Software\\Classes\\" + shellKey).c_str(), 0, KEY_READ,
                          &key) != ERROR_SUCCESS) {
            continue;
        }

        for (DWORD i = 0;; ++i) {
            wchar_t name[256] = {};
            DWORD nameLength = ARRAYSIZE(name);
            if (RegEnumKeyExW(key, i, name, &nameLength, nullptr, nullptr, nullptr,
                              nullptr) != ERROR_SUCCESS) {
                break;
            }

            const std::wstring verbKey = shellKey + L"\\" + name;
            std::wstring display = ReadClassesString(verbKey, L"MUIVerb");
            if (display.empty()) {
                display = ReadClassesString(verbKey, nullptr);
            }
            display = ResolveIndirectString(display);
            display.erase(std::remove(display.begin(), display.end(), L'&'),
                          display.end());
            if (display.empty()) {
                continue;
            }
            const std::wstring icon = ReadClassesString(verbKey, L"Icon");
            if (icon.empty()) {
                continue;
            }
            ShellLabelIcon entry;
            entry.display = display;
            entry.icon = icon;
            icons.push_back(std::move(entry));
        }
        RegCloseKey(key);
    }
    return icons;
}

std::wstring MatchShellLabelIcon(const std::vector<ShellLabelIcon>& icons,
                                 const std::wstring& label);


std::wstring MatchShellLabelIcon(const std::vector<ShellLabelIcon>& icons,
                                 const std::wstring& label) {
    std::wstring normalized = label;
    normalized.erase(std::remove(normalized.begin(), normalized.end(), L'&'),
                     normalized.end());
    if (normalized.empty()) {
        return L"";
    }
    for (const ShellLabelIcon& entry : icons) {
        if (_wcsicmp(entry.display.c_str(), normalized.c_str()) == 0) {
            return entry.icon;
        }
    }
    return L"";
}

// Generic vendor/platform words carry no signal when matching version info
// against menu labels; without this, labels containing "Windows" or
// "Microsoft" would match almost every handler.
bool IsGenericVersionWord(const std::wstring& word) {
    static const wchar_t* kStopWords[] = {
        L"Microsoft", L"Windows",     L"Corporation", L"Corp",
        L"Inc",       L"Ltd",         L"LLC",         L"Software",
        L"System",    L"Operating",   L"Company",     L"Product",
        L"Version",   L"Common",      L"File",        L"Files",
        L"Shell",     L"Application", L"Program",     L"Service",
        L"Services",  L"Technologies", L"Technology", L"International",
        L"Limited",   L"Group",       L"Global",
    };
    for (const wchar_t* stop : kStopWords) {
        if (_wcsicmp(word.c_str(), stop) == 0) {
            return true;
        }
    }
    return false;
}

// True when any significant word (5+ characters) of `text` appears in the
// label. Used to match handler DLL version info against menu item labels.
bool LabelMatchesWords(const std::wstring& label, const std::wstring& text) {
    std::wstring word;
    for (size_t i = 0; i <= text.size(); ++i) {
        const wchar_t c = (i < text.size()) ? text[i] : L' ';
        if (iswalnum(c)) {
            word += c;
            continue;
        }
        if (word.size() >= 5 && !IsGenericVersionWord(word) &&
            StrStrIW(label.c_str(), word.c_str()) != nullptr) {
            return true;
        }
        word.clear();
    }
    return false;
}

// Matches a handler DLL by its version-info company/product/description.
// Reads CompanyName/ProductName/FileDescription once per DLL.
void ReadVersionStrings(const std::wstring& path,
                        std::vector<std::wstring>& out) {
    DWORD ignored = 0;
    const DWORD size = GetFileVersionInfoSizeW(path.c_str(), &ignored);
    if (size == 0) {
        return;
    }

    std::vector<uint8_t> data(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, data.data())) {
        return;
    }

    struct Translation {
        WORD language;
        WORD codePage;
    };
    Translation fallback = {0x0409, 0x04B0};
    Translation* translations = nullptr;
    UINT translationBytes = 0;
    if (!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                        reinterpret_cast<LPVOID*>(&translations),
                        &translationBytes) ||
        translationBytes < sizeof(Translation)) {
        translations = &fallback;
        translationBytes = sizeof(fallback);
    }

    static const wchar_t* kFields[] = {L"CompanyName", L"ProductName",
                                       L"FileDescription"};
    const size_t translationCount = translationBytes / sizeof(Translation);
    for (size_t t = 0; t < translationCount; ++t) {
        for (const wchar_t* field : kFields) {
            wchar_t query[128] = {};
            swprintf(query, ARRAYSIZE(query),
                     L"\\StringFileInfo\\%04x%04x\\%s", translations[t].language,
                     translations[t].codePage, field);
            wchar_t* value = nullptr;
            UINT valueChars = 0;
            if (VerQueryValueW(data.data(), query, reinterpret_cast<LPVOID*>(&value),
                               &valueChars) &&
                value && valueChars > 0) {
                out.emplace_back(value);
            }
        }
    }
}


// Last resort: find a ContextMenuHandlers key that matches the item label
// (by key name, handler DLL name, or DLL version info) and extract icon 0
// from its DLL.
// True when the label matches a registered ContextMenuHandlers entry, by key
// name, handler DLL name, or DLL version info. Fills `dllOut` with the
// handler DLL path when matched. Used for icons and for classifying
// third-party items.
struct RegisteredHandlerInfo {
    std::wstring name;
    std::wstring dll;
    std::vector<std::wstring> versionStrings;
};

// Enumerates the registered handlers once; matching per item is string work.
std::vector<RegisteredHandlerInfo> BuildRegisteredHandlers(
    const ContextSignature& signature) {
    std::vector<RegisteredHandlerInfo> handlers;
    for (const std::wstring& base : ShellIconBases(signature)) {
        const std::wstring handlersKey = base + L"\\shellex\\ContextMenuHandlers";
        for (HKEY root : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
            HKEY key = nullptr;
            if (RegOpenKeyExW(root, (L"Software\\Classes\\" + handlersKey).c_str(), 0,
                              KEY_READ, &key) != ERROR_SUCCESS) {
                continue;
            }

            for (DWORD i = 0;; ++i) {
                wchar_t name[256] = {};
                DWORD nameLength = ARRAYSIZE(name);
                if (RegEnumKeyExW(key, i, name, &nameLength, nullptr, nullptr, nullptr,
                                  nullptr) != ERROR_SUCCESS) {
                    break;
                }

                const std::wstring clsid =
                    ReadClassesString(handlersKey + L"\\" + name, nullptr);
                if (clsid.empty()) {
                    continue;
                }
                std::wstring dll = ReadClassesString(
                    L"CLSID\\" + clsid + L"\\InprocServer32", nullptr);
                if (dll.empty()) {
                    continue;
                }
                if (dll.size() >= 2 && dll.front() == L'"' && dll.back() == L'"') {
                    dll = dll.substr(1, dll.size() - 2);
                }
                wchar_t expanded[MAX_PATH] = {};
                if (!ExpandEnvironmentStringsW(dll.c_str(), expanded,
                                              ARRAYSIZE(expanded)) ||
                    GetFileAttributesW(expanded) == INVALID_FILE_ATTRIBUTES) {
                    continue;
                }

                RegisteredHandlerInfo info;
                info.name = name;
                info.dll = expanded;
                ReadVersionStrings(expanded, info.versionStrings);
                handlers.push_back(std::move(info));
            }
            RegCloseKey(key);
        }
    }
    return handlers;
}

bool MatchesRegisteredHandler(const std::vector<RegisteredHandlerInfo>& handlers,
                              const std::wstring& label, std::wstring* dllOut) {
    if (label.empty()) {
        return false;
    }
    for (const RegisteredHandlerInfo& handler : handlers) {
        bool matches = handler.name.size() >= 5 &&
                       StrStrIW(label.c_str(), handler.name.c_str()) != nullptr;
        if (!matches) {
            std::wstring stem = PathFindFileNameW(handler.dll.c_str());
            const size_t dot = stem.find_last_of(L'.');
            if (dot != std::wstring::npos) {
                stem.resize(dot);
            }
            matches = stem.size() >= 5 &&
                      StrStrIW(label.c_str(), stem.c_str()) != nullptr;
        }
        if (!matches) {
            for (const std::wstring& value : handler.versionStrings) {
                if (LabelMatchesWords(label, value)) {
                    matches = true;
                    break;
                }
            }
        }
        if (matches) {
            if (dllOut) {
                *dllOut = handler.dll;
            }
            return true;
        }
    }
    return false;
}


// Logs the registered context menu handlers so an unresolved item can be
// traced to its registration.
void LogHandlerCandidates(const ContextSignature& signature,
                          const std::wstring& label) {
    for (const std::wstring& base : ShellIconBases(signature)) {
        const std::wstring handlersKey = base + L"\\shellex\\ContextMenuHandlers";
        for (HKEY root : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
            HKEY key = nullptr;
            if (RegOpenKeyExW(root, (L"Software\\Classes\\" + handlersKey).c_str(), 0,
                              KEY_READ, &key) != ERROR_SUCCESS) {
                continue;
            }

            std::wstring names;
            for (DWORD i = 0; i < 12; ++i) {
                wchar_t name[256] = {};
                DWORD nameLength = ARRAYSIZE(name);
                if (RegEnumKeyExW(key, i, name, &nameLength, nullptr, nullptr, nullptr,
                                  nullptr) != ERROR_SUCCESS) {
                    break;
                }
                if (!names.empty()) {
                    names += L", ";
                }
                names += name;
            }
            RegCloseKey(key);

            if (!names.empty()) {
                if (g_settings->debugLogging) Wh_Log(L"Handler candidates for '%s' (%s): %s", label.c_str(),
                       base.c_str(), names.c_str());
            }
        }
    }
}

// Fills in registry icons for static verbs that provide no menu bitmap and
// marks items that belong to registered shell extensions (used by the
// advanced submenu).
void ApplyRegistryIconsRecursive(
    std::vector<MenuItem>& items, const ContextSignature& signature,
    const std::vector<RegisteredHandlerInfo>& handlers,
    const std::vector<ShellLabelIcon>& labelIcons) {
    for (MenuItem& item : items) {
        ApplyRegistryIconsRecursive(item.children, signature, handlers, labelIcons);
        if (item.kind == ItemKind::Separator) {
            continue;
        }

        std::wstring handlerDll;
        const bool registered =
            MatchesRegisteredHandler(handlers, item.label, &handlerDll);
        if (registered) {
            item.flags |= kModelThirdParty;
        }

        if (item.action != ActionKind::ShellVerb || !item.iconPixels.empty() ||
            !item.iconRef.empty()) {
            continue;
        }

        std::wstring icon = ResolveRegistryIcon(signature, item.canonicalVerb);
        if (icon.empty()) {
            icon = MatchShellLabelIcon(labelIcons, item.label);
        }
        if (icon.empty() && registered) {
            icon = handlerDll + L",0";
        }
        if (!icon.empty()) {
            item.iconRef = icon;
            if (g_settings->debugLogging) Wh_Log(L"Registry icon for '%s' (verb '%s'): %s", item.label.c_str(),
                   item.canonicalVerb.c_str(), icon.c_str());
        } else if (g_settings->debugLogging) {
            LogHandlerCandidates(signature, item.label);
            Wh_Log(L"No registry icon for '%s' (verb '%s')", item.label.c_str(),
                   item.canonicalVerb.c_str());
        }
    }
}

void ApplyRegistryIcons(std::vector<MenuItem>& items,
                        const ContextSignature& signature) {
    // The registry and DLL version info are read once per open instead of
    // once per item; this is the open-path cost for nav-pane menus.
    const std::vector<RegisteredHandlerInfo> handlers =
        BuildRegisteredHandlers(signature);
    const std::vector<ShellLabelIcon> labelIcons =
        BuildShellLabelIcons(signature);
    ApplyRegistryIconsRecursive(items, signature, handlers, labelIcons);
}

void BuildItemsFromHMenu(HMENU menu, UINT idCmdFirst, IContextMenu* context,
                         uint32_t& nextId, std::vector<MenuItem>& out, bool& ownerDraw) {
    const int count = GetMenuItemCount(menu);
    for (int index = 0; index < count; ++index) {
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_STATE | MIIM_FTYPE | MIIM_SUBMENU | MIIM_BITMAP;
        if (!GetMenuItemInfoW(menu, static_cast<UINT>(index), TRUE, &info)) {
            continue;
        }

        MenuItem item{};
        item.id = nextId++;
        item.flags = MapExtensionMenuState(info.fState) | kModelExtension;

        if (info.fType & MFT_SEPARATOR) {
            item.kind = ItemKind::Separator;
            out.push_back(std::move(item));
            continue;
        }

        wchar_t labelBuffer[512] = {};
        const int labelLength = GetMenuStringW(menu, static_cast<UINT>(index), labelBuffer,
                                               ARRAYSIZE(labelBuffer), MF_BYPOSITION);
        if (labelLength > 0) {
            item.label.assign(labelBuffer, static_cast<size_t>(labelLength));
        }

        if (info.hbmpItem && IsSentinelMenuBitmap(info.hbmpItem)) {
            if (g_settings->debugLogging) Wh_Log(L"Sentinel menu bitmap %lld for '%s'",
                   static_cast<long long>(reinterpret_cast<INT_PTR>(info.hbmpItem)),
                   item.label.c_str());
        }

        if (info.fType & MFT_OWNERDRAW) {
            item.flags |= kModelOwnerDraw;
            ownerDraw = true;
        }

        // Capture icons the shell itself provides, for commands and submenu
        // parents alike.
        if (info.hbmpItem && !IsSentinelMenuBitmap(info.hbmpItem)) {
            CaptureBitmapPixels(info.hbmpItem, item.iconPixels);
        }
        if (item.iconPixels.empty()) {
            if (HBITMAP recorded = LookupRecordedItemBitmap(menu, info.wID)) {
                CaptureBitmapPixels(recorded, item.iconPixels);
            }
        }

        if (info.hSubMenu) {
            item.kind = ItemKind::Submenu;
            item.action = ActionKind::Submenu;
            BuildItemsFromHMenu(info.hSubMenu, idCmdFirst, context, nextId, item.children,
                                ownerDraw);
        } else {
            item.kind = ItemKind::Command;
            item.action = ActionKind::ShellVerb;
            if (info.wID >= idCmdFirst) {
                item.verbOffset = info.wID - idCmdFirst;
                item.flags |= kModelHasOffset;
            }
            if (context) {
                wchar_t verbBuffer[256] = {};
                if (SUCCEEDED(context->GetCommandString(
                        static_cast<UINT_PTR>(item.verbOffset), GCS_VERBW, nullptr,
                        reinterpret_cast<LPSTR>(verbBuffer), ARRAYSIZE(verbBuffer)))) {
                    item.canonicalVerb = verbBuffer;
                }
            }
        }
        out.push_back(std::move(item));
    }
}

MenuModel BuildModelFromHMenu(HMENU menu, UINT idCmdFirst,
                              const ContextSignature& signature, IContextMenu* context) {
    MenuModel model{};
    model.sig = signature;
    uint32_t nextId = 10000;
    bool ownerDraw = false;
    BuildItemsFromHMenu(menu, idCmdFirst, context, nextId, model.items, ownerDraw);
    if (ownerDraw) {
        model.flags |= kModelOwnerDraw;
    }
    return model;
}

// Walks the retained populated menu into the cache. Always called after the
// interactive menu has closed, on the same UI thread. Persistence happens
// later on the invalidation thread.
std::vector<std::wstring> SnapshotLoadedModules();
std::vector<std::wstring> DiffModules(const std::vector<std::wstring>& before,
                                      const std::vector<std::wstring>& after);
bool EnsureContextPopulated(PendingCapture& capture);

// Mimics a menu host: asks the context object to initialize each menu and
// submenu before its items are read. Extensions populate dynamic labels and
// submenu children on WM_INITMENUPOPUP; without this, discovery captures
// empty labels and empty submenus.
void InitializeMenuRecursive(PendingCapture& capture, HMENU menu, UINT position) {
    if (capture.contextMenu3) {
        LRESULT result = 0;
        capture.contextMenu3->HandleMenuMsg2(
            WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(menu),
            MAKELPARAM(position, 0), &result);
    } else if (capture.contextMenu2) {
        capture.contextMenu2->HandleMenuMsg(
            WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(menu), MAKELPARAM(position, 0));
    }

    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_SUBMENU;
        if (GetMenuItemInfoW(menu, static_cast<UINT>(i), TRUE, &info) &&
            info.hSubMenu) {
            InitializeMenuRecursive(capture, info.hSubMenu, static_cast<UINT>(i));
        }
    }
}

// Debug dump of a native menu: labels, ids, offsets and bitmap sentinels.
// Run with Debug logging after the menu closes, when dynamic submenus have
// been populated.
void DumpMenuTree(HMENU menu, UINT idCmdFirst, int depth) {
    if (!g_settings->debugLogging || !menu) {
        return;
    }
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU | MIIM_STATE | MIIM_BITMAP;
        if (!GetMenuItemInfoW(menu, static_cast<UINT>(i), TRUE, &info)) {
            continue;
        }
        wchar_t label[256] = {};
        GetMenuStringW(menu, static_cast<UINT>(i), label, ARRAYSIZE(label),
                       MF_BYPOSITION);
        const long long bitmap = static_cast<long long>(reinterpret_cast<INT_PTR>(
            info.hbmpItem));
        if (info.hSubMenu) {
            Wh_Log(L"[menu d%d] submenu '%s' id=%u state=%04X bmp=%lld children=%d",
                   depth, label, info.wID, info.fState, bitmap,
                   GetMenuItemCount(info.hSubMenu));
            DumpMenuTree(info.hSubMenu, idCmdFirst, depth + 1);
        } else if (!(info.fType & MFT_SEPARATOR)) {
            const long offset = info.wID >= idCmdFirst
                                    ? static_cast<long>(info.wID - idCmdFirst)
                                    : -1;
            Wh_Log(L"[menu d%d] item '%s' id=%u offset=%ld state=%04X bmp=%lld",
                   depth, label, info.wID, offset, info.fState, bitmap);
        }
    }
}

// Debug dump of a discovered model; helps identify unlabeled or unusual items.
void DumpModelItems(const std::vector<MenuItem>& items, int depth) {    for (const MenuItem& item : items) {
        Wh_Log(L"[d%d] kind=%d action=%d flags=%04X offset=%u verb='%s' label='%s' "
               L"children=%zu icons=%zu",
               depth, static_cast<int>(item.kind), static_cast<int>(item.action),
               item.flags, item.verbOffset, item.canonicalVerb.c_str(),
               item.label.c_str(), item.children.size(), item.iconPixels.size());
        DumpModelItems(item.children, depth + 1);
    }
}

// Copies icons captured before host initialization onto the post-init model,
// matching by canonical verb first and label second.
void MergePreInitIcons(std::vector<MenuItem>& post, const std::vector<MenuItem>& pre) {
    for (MenuItem& item : post) {
        const MenuItem* match = nullptr;
        for (const MenuItem& candidate : pre) {
            if (!item.canonicalVerb.empty() && !candidate.canonicalVerb.empty() &&
                candidate.canonicalVerb == item.canonicalVerb) {
                match = &candidate;
                break;
            }
        }
        if (!match) {
            for (const MenuItem& candidate : pre) {
                if (!item.label.empty() && !candidate.label.empty() &&
                    candidate.label == item.label) {
                    match = &candidate;
                    break;
                }
            }
        }

        if (item.iconPixels.empty() && item.action == ActionKind::ShellVerb && match &&
            !match->iconPixels.empty()) {
            item.iconPixels = match->iconPixels;
        }
        if (match) {
            MergePreInitIcons(item.children, match->children);
        }
    }
}

// Asks the extension to draw an item into an offscreen bitmap (the owner-draw
// path) and crops the icon gutter. Covers items that expose no MIM_BITMAP,
// including HBMMENU_CALLBACK items.
bool CaptureOwnerDrawIcon(PendingCapture& capture, const MenuItem& item,
                          std::vector<uint8_t>& out) {
    if (!capture.contextMenu2 && !capture.contextMenu3) {
        return false;
    }

    const int height =
        std::max(24, static_cast<int>(GetSystemMetrics(SM_CYMENU)));
    const int width = height * 4;
    const UINT commandId = capture.idCmdFirst + item.verbOffset;

    HDC screen = GetDC(nullptr);
    HDC memory = CreateCompatibleDC(screen);

    BITMAPINFO dibInfo = {};
    dibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dibInfo.bmiHeader.biWidth = width;
    dibInfo.bmiHeader.biHeight = -height;  // top-down
    dibInfo.bmiHeader.biPlanes = 1;
    dibInfo.bmiHeader.biBitCount = 32;
    dibInfo.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(screen, &dibInfo, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return false;
    }

    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
    const COLORREF background = MenuBackgroundColor(capture.owner);
    RECT rect = {0, 0, width, height};
    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(memory, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);

    DRAWITEMSTRUCT drawInfo = {};
    drawInfo.CtlType = ODT_MENU;
    drawInfo.CtlID = commandId;
    drawInfo.itemID = commandId;
    drawInfo.itemAction = ODA_DRAWENTIRE;
    if (item.flags & kModelDisabled) {
        drawInfo.itemState |= ODS_DISABLED;
    }
    if (item.flags & kModelChecked) {
        drawInfo.itemState |= ODS_CHECKED;
    }
    if (item.flags & kModelDefault) {
        drawInfo.itemState |= ODS_DEFAULT;
    }
    drawInfo.hwndItem = reinterpret_cast<HWND>(capture.populatedMenu);
    drawInfo.hDC = memory;
    drawInfo.rcItem = rect;

    bool handled = false;
    if (capture.contextMenu3) {
        LRESULT result = 0;
        handled = SUCCEEDED(capture.contextMenu3->HandleMenuMsg2(
            WM_DRAWITEM, 0, reinterpret_cast<LPARAM>(&drawInfo), &result));
    } else if (capture.contextMenu2) {
        handled = SUCCEEDED(capture.contextMenu2->HandleMenuMsg(
            WM_DRAWITEM, 0, reinterpret_cast<LPARAM>(&drawInfo)));
    }

    if (handled) {
        // Crop the icon gutter: the bounding box of pixels that differ from
        // the menu background in the left half of the item.
        const uint8_t backgroundBlue = GetBValue(background);
        const uint8_t backgroundGreen = GetGValue(background);
        const uint8_t backgroundRed = GetRValue(background);
        const uint8_t* pixels = static_cast<const uint8_t*>(bits);

        int minX = width;
        int minY = height;
        int maxX = -1;
        int maxY = -1;
        const int scanWidth = width / 2;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < scanWidth; ++x) {
                const uint8_t* pixel = pixels + (static_cast<size_t>(y) * width + x) * 4;
                if (pixel[0] != backgroundBlue || pixel[1] != backgroundGreen ||
                    pixel[2] != backgroundRed) {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }
        if (maxX >= minX && maxY >= minY) {
            const int cropWidth = maxX - minX + 1;
            const int cropHeight = maxY - minY + 1;
            out.resize(static_cast<size_t>(cropWidth) * cropHeight * 4);
            for (int y = 0; y < cropHeight; ++y) {
                memcpy(out.data() + static_cast<size_t>(y) * cropWidth * 4,
                       pixels + (static_cast<size_t>(minY + y) * width + minX) * 4,
                       static_cast<size_t>(cropWidth) * 4);
            }
        }
    }

    SelectObject(memory, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
    return handled && !out.empty();
}

void CaptureOwnerDrawIcons(PendingCapture& capture, std::vector<MenuItem>& items) {
    for (MenuItem& item : items) {
        CaptureOwnerDrawIcons(capture, item.children);
        if (!item.iconPixels.empty() || !item.iconRef.empty() ||
            item.kind != ItemKind::Command || item.action != ActionKind::ShellVerb ||
            !(item.flags & kModelHasOffset)) {
            continue;
        }

        std::vector<uint8_t> pixels;
        if (CaptureOwnerDrawIcon(capture, item, pixels)) {
            item.iconPixels = std::move(pixels);
            if (g_settings->debugLogging) Wh_Log(L"Owner-draw icon captured for '%s'", item.label.c_str());
        } else {
            if (g_settings->debugLogging) Wh_Log(L"Owner-draw draw not handled for '%s' (verb '%s')",
                   item.label.c_str(), item.canonicalVerb.c_str());
        }
    }
}

void DiscoverIntoCache(PendingCapture& capture, const ContextSignature& signature) {
    if (capture.discoveryDone || !capture.obj) {
        return;
    }
    if (!EnsureContextPopulated(capture)) {
        return;
    }

    const ULONGLONG start = GetTickCount64();

    // Capture the menu as populated, before host initialization: extensions
    // often switch their bitmaps to callback-drawn icons on
    // WM_INITMENUPOPUP, which would lose the real images.
    MenuModel preInit = BuildModelFromHMenu(capture.populatedMenu, capture.idCmdFirst,
                                            signature, capture.obj);

    // Initialize like a real host, then rebuild for dynamic labels/children.
    if (!capture.menuInitialized) {
        InitializeMenuRecursive(capture, capture.populatedMenu, 0);
        capture.menuInitialized = true;
    }
    MenuModel model = BuildModelFromHMenu(capture.populatedMenu, capture.idCmdFirst,
                                          signature, capture.obj);
    MergePreInitIcons(model.items, preInit.items);
    ClearRecordedItemBitmaps();

    ApplyRegistryIcons(model.items, signature);
    CaptureOwnerDrawIcons(capture, model.items);
    model.handlerModules = capture.handlerModules;
    model.sourceStamp = capture.sourceStamp;
    capture.discoveryDone = true;

    if (g_settings->debugLogging) Wh_Log(L"Discovered %zu menu items in %llu ms", model.items.size(),
           static_cast<unsigned long long>(GetTickCount64() - start));
    if (g_settings->debugLogging) {
        DumpModelItems(model.items, 0);
    }
    // Warm the render-ready layout for the custom menu while we are already
    // paying discovery cost, so the next open is a cache hit. The prebuild
    // serializes itself with the menus through the shared gate.
    PrebuildLayoutsForWarmup({model}, DpiForWindow(GetDesktopWindow()),
                             IsDarkThemeActive());
    g_cache.Put(std::move(model));
}

// SendTo entries are built once (at mod init) and copied into the model at
// open time, keeping the interactive path free of filesystem I/O.
std::vector<MenuItem> g_sendToChildren;
std::mutex g_sendToMutex;

void RebuildSendToChildren() {
    std::vector<MenuItem> children;

    PWSTR sendToPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SendTo, 0, nullptr, &sendToPath)) &&
        sendToPath) {
        const std::wstring directory(sendToPath);
        const std::wstring pattern = directory + L"\\*";
        WIN32_FIND_DATAW findData = {};
        HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
        if (find != INVALID_HANDLE_VALUE) {
            uint32_t nextChildId = 20000;
            do {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                if (findData.dwFileAttributes &
                    (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
                    continue;
                }
                std::wstring label = findData.cFileName;
                const size_t dot = label.find_last_of(L'.');
                // Only .lnk entries can be "sent" with ShellExecute. The
                // other entries (.ZFSendToTarget, .DeskLink, .MAPIMail,
                // .mydocs) are drop targets that need IDropTarget::Drop; the
                // captured shell menu supplies the real Send to submenu.
                if (dot == std::wstring::npos || dot == 0 ||
                    _wcsicmp(label.c_str() + dot, L".lnk") != 0) {
                    continue;
                }
                label.resize(dot);

                MenuItem child{};
                child.id = nextChildId++;
                child.kind = ItemKind::Command;
                child.action = ActionKind::ShellVerb;
                child.canonicalVerb = L"sendto";
                child.label = std::move(label);
                child.targetPath = directory + L"\\" + findData.cFileName;
                children.push_back(std::move(child));
            } while (FindNextFileW(find, &findData));
            FindClose(find);
        }
        CoTaskMemFree(sendToPath);
    }

    std::lock_guard<std::mutex> lock(g_sendToMutex);
    g_sendToChildren = std::move(children);
}

std::vector<MenuItem> GetSendToChildren() {
    std::lock_guard<std::mutex> lock(g_sendToMutex);
    return g_sendToChildren;
}

void InvalidateSendToChildren() {
    std::lock_guard<std::mutex> lock(g_sendToMutex);
    g_sendToChildren.clear();
}

// Creates a throwaway default context menu to read the shared vtable of
// shell32's default context menu implementation and hook its
// QueryContextMenu slot.
bool InstallVtableHook() {
    HRESULT initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IShellFolder* desktop = nullptr;
    PIDLIST_ABSOLUTE pidl = nullptr;
    IContextMenu* menu = nullptr;
    bool installed = false;

    HRESULT hr = SHGetDesktopFolder(&desktop);
    if (SUCCEEDED(hr) && desktop) {
        hr = SHGetKnownFolderIDList(FOLDERID_Windows, 0, nullptr, &pidl);
        if (SUCCEEDED(hr) && pidl) {
            PCUITEMID_CHILD children[1] = {pidl};
            DEFCONTEXTMENU dcm = {};
            dcm.psf = desktop;
            dcm.cidl = 1;
            dcm.apidl = children;

            hr = SHCreateDefaultContextMenu(&dcm, IID_IContextMenu, (void**)&menu);
            if (SUCCEEDED(hr) && menu) {
                void** vtable = *reinterpret_cast<void***>(menu);
                void* queryContextMenu = vtable[3];
                installed = WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<QueryContextMenu_t>(queryContextMenu),
                    QueryContextMenu_Hook, &QueryContextMenu_Original);
                if (!installed) {
                    Wh_Log(L"SetFunctionHook(QueryContextMenu) failed");
                }
            } else {
                Wh_Log(L"SHCreateDefaultContextMenu failed: %08X", hr);
            }
        } else {
            Wh_Log(L"SHGetKnownFolderIDList failed: %08X", hr);
        }
    } else {
        Wh_Log(L"SHGetDesktopFolder failed: %08X", hr);
    }

    if (menu) {
        menu->Release();
    }
    if (pidl) {
        CoTaskMemFree(pidl);
    }
    if (desktop) {
        desktop->Release();
    }
    // Balancing the call is only safe for S_OK; S_FALSE means this thread was
    // already initialized by someone else.
    if (initResult == S_OK) {
        CoUninitialize();
    }

    return installed;
}

bool InstallPopulationHook() {
    if (QueryContextMenu_Original) {
        return true;
    }
    // The hook is set on the class function itself, so one successful vtable
    // lookup covers every menu instance in the process.
    if (InstallVtableHook()) {
        return true;
    }
    Wh_Log(L"Failed to install the context menu population hook");
    return false;
}

// --- Selection context ------------------------------------------------------
// Reads the current selection for the menu owner. Adapted from the
// remove-context-menu-items mod by Armaninyow (MIT-licensed).

IShellBrowser* GetShellBrowserForWindow(HWND hwnd) {
    struct ShellWindowClass {
        const wchar_t* name;
        bool isFrame;
    };
    static const ShellWindowClass kClasses[] = {
        {L"ShellTabWindowClass", false},
        {L"CabinetWClass", true},
        {L"ExploreWClass", true},
    };

    for (HWND window = hwnd; window; window = GetAncestor(window, GA_PARENT)) {
        wchar_t className[256] = {};
        GetClassNameW(window, className, ARRAYSIZE(className));
        for (const ShellWindowClass& cls : kClasses) {
            if (wcscmp(className, cls.name) != 0) {
                continue;
            }
            LRESULT result =
                SendMessageW(window, WM_USER + 7 /* CWM_GETISHELLBROWSER */, 0, 0);
            IShellBrowser* browser = reinterpret_cast<IShellBrowser*>(result);
            if (browser) {
                return browser;
            }
            if (cls.isFrame) {
                return nullptr;
            }
            break;
        }
    }
    return nullptr;
}

IShellBrowser* GetDesktopShellBrowser() {
    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&shellWindows)) ||
        !shellWindows) {
        return nullptr;
    }

    IShellBrowser* browser = nullptr;
    VARIANT empty = {};
    long hwnd = 0;
    IDispatch* dispatch = nullptr;
    if (SUCCEEDED(shellWindows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &hwnd,
                                             SWFO_NEEDDISPATCH, &dispatch)) &&
        dispatch) {
        IServiceProvider* provider = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_IServiceProvider,
                                               (void**)&provider)) &&
            provider) {
            provider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                   (void**)&browser);
            provider->Release();
        }
        dispatch->Release();
    }

    shellWindows->Release();
    return browser;
}

struct SelectionInfo {
    std::vector<std::wstring> paths;
    bool folderIsFilesystem = false;
    bool allItemsAreFilesystem = true;
    bool allItemsAreFolders = false;
};

SelectionInfo GetSelectionFromShellBrowser(IShellBrowser* browser) {
    SelectionInfo info;
    if (!browser) {
        return info;
    }

    IShellView* view = nullptr;
    if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
        IFolderView* folderView = nullptr;
        if (SUCCEEDED(view->QueryInterface(IID_IFolderView, (void**)&folderView)) &&
            folderView) {
            IShellFolder* folder = nullptr;
            if (SUCCEEDED(folderView->GetFolder(IID_IShellFolder, (void**)&folder)) &&
                folder) {
                SFGAOF folderAttributes = SFGAO_FILESYSTEM;
                info.folderIsFilesystem =
                    SUCCEEDED(folder->GetAttributesOf(0, nullptr, &folderAttributes)) &&
                    (folderAttributes & SFGAO_FILESYSTEM) != 0;

                bool allFolders = true;
                IEnumIDList* enumIds = nullptr;
                if (SUCCEEDED(folderView->Items(SVGIO_SELECTION, IID_IEnumIDList,
                                                (void**)&enumIds)) &&
                    enumIds) {
                    LPITEMIDLIST pidl = nullptr;
                    while (enumIds->Next(1, &pidl, nullptr) == S_OK) {
                        SFGAOF itemAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER;
                        PCUITEMID_CHILD child = pidl;
                        if (SUCCEEDED(
                                folder->GetAttributesOf(1, &child, &itemAttributes))) {
                            if (!(itemAttributes & SFGAO_FILESYSTEM)) {
                                info.allItemsAreFilesystem = false;
                            }
                            if (!(itemAttributes & SFGAO_FOLDER)) {
                                allFolders = false;
                            }
                        } else {
                            info.allItemsAreFilesystem = false;
                            allFolders = false;
                        }

                        STRRET strret = {};
                        if (SUCCEEDED(folder->GetDisplayNameOf(pidl, SHGDN_FORPARSING,
                                                               &strret))) {
                            LPWSTR path = nullptr;
                            if (SUCCEEDED(StrRetToStrW(&strret, pidl, &path)) && path) {
                                if (path[0]) {
                                    info.paths.emplace_back(path);
                                }
                                CoTaskMemFree(path);
                            }
                        }
                        CoTaskMemFree(pidl);
                    }
                    enumIds->Release();
                }
                info.allItemsAreFolders = allFolders && !info.paths.empty();
                folder->Release();
            }
            folderView->Release();
        }
        view->Release();
    }
    return info;
}

SelectionInfo GetSelection(HWND owner, ShellViewKind kind) {
    if (kind == ShellViewKind::Desktop) {
        IShellBrowser* browser = GetDesktopShellBrowser();
        SelectionInfo info = GetSelectionFromShellBrowser(browser);
        if (browser) {
            browser->Release();
        }
        return info;
    }

    if (kind == ShellViewKind::ShellDefView) {
        IShellBrowser* browser = GetShellBrowserForWindow(owner);
        if (!browser) {
            return {};
        }
        // CWM_GETISHELLBROWSER returns a borrowed pointer; hold a reference
        // for the duration of the lookup.
        browser->AddRef();
        SelectionInfo info = GetSelectionFromShellBrowser(browser);
        browser->Release();
        return info;
    }

    return {};
}

std::vector<std::wstring> SnapshotLoadedModules() {
    std::vector<std::wstring> modules;
    HANDLE snapshot =
        CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) {
        return modules;
    }

    MODULEENTRY32W entry = {};
    entry.dwSize = sizeof(entry);
    if (Module32FirstW(snapshot, &entry)) {
        do {
            modules.emplace_back(entry.szExePath);
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return modules;
}

std::vector<std::wstring> DiffModules(const std::vector<std::wstring>& before,
                                      const std::vector<std::wstring>& after) {
    std::vector<std::wstring> added;
    for (const std::wstring& path : after) {
        if (std::find(before.begin(), before.end(), path) == before.end()) {
            added.push_back(path);
        }
    }
    return added;
}

}  // namespace cmo

// ===========================================================================
// [CMO:Invoker] Executing a chosen menu item.
// ===========================================================================
namespace cmo {

enum class InvokeResult : uint8_t { Handled, FallbackNative, Failed };

struct InvocationContext {
    HWND owner = nullptr;
    POINT pt = {};
    std::vector<std::wstring> paths;
    std::wstring directory;
    IContextMenu* liveContext = nullptr;
    UINT idCmdFirst = 0;
    ShellViewKind kind = ShellViewKind::None;
    DWORD clipboardSequence = 0;
    bool clipboardHadData = false;
    std::shared_ptr<const RulesConfig> config;
};

int ShowWindowToShowCmd(ShowWindow showWindow) {
    switch (showWindow) {
        case ShowWindow::Maximized:
            return SW_SHOWMAXIMIZED;
        case ShowWindow::Minimized:
            return SW_SHOWMINIMIZED;
        case ShowWindow::Hidden:
            return SW_HIDE;
        default:
            return SW_SHOWNORMAL;
    }
}

void ReplaceAll(std::wstring& text, const std::wstring& from,
                const std::wstring& to) {
    if (from.empty()) {
        return;
    }
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::wstring::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::wstring QuotePathIfNeeded(const std::wstring& path) {
    if (path.find(L' ') == std::wstring::npos &&
        path.find(L'\t') == std::wstring::npos) {
        return path;
    }
    return L'"' + path + L'"';
}

// Splits an expanded command line into the executable and its parameters for
// ShellExecuteEx (which does not parse a single command-line string).
bool SplitCommandLine(const std::wstring& command, std::wstring& file,
                      std::wstring& parameters) {
    bool inQuotes = false;
    bool split = false;
    for (size_t i = 0; i < command.size(); ++i) {
        const wchar_t c = command[i];
        if (c == L'"') {
            inQuotes = !inQuotes;
        } else if (!inQuotes && (c == L' ' || c == L'\t')) {
            file = command.substr(0, i);
            parameters = TrimWhitespace(command.substr(i + 1));
            split = true;
            break;
        }
    }
    if (!split) {
        file = command;
        parameters.clear();
    }
    if (file.size() >= 2 && file.front() == L'"' && file.back() == L'"') {
        file = file.substr(1, file.size() - 2);
    }
    return !file.empty();
}

std::wstring ExpandCommandPlaceholders(const std::wstring& command,
                                       const InvocationContext& ctx) {
    std::wstring expanded = ExpandEnv(command);

    ReplaceAll(expanded, L"%dir%", ctx.directory);

    std::wstring allPaths;
    for (const std::wstring& path : ctx.paths) {
        if (!allPaths.empty()) {
            allPaths += L' ';
        }
        allPaths += L'"' + path + L'"';
    }
    ReplaceAll(expanded, L"%*", allPaths);

    if (!ctx.paths.empty()) {
        ReplaceAll(expanded, L"%1", QuotePathIfNeeded(ctx.paths.front()));
    }
    return expanded;
}

bool InvokeCustomCommand(const CustomCommand& command,
                         const InvocationContext& ctx) {
    const std::wstring expanded = ExpandCommandPlaceholders(command.command, ctx);
    if (expanded.empty()) {
        return false;
    }
    std::wstring workingDir = ExpandCommandPlaceholders(command.workingDir, ctx);
    if (workingDir.empty()) {
        workingDir = ctx.directory;
    }

    if (command.runAs == RunAs::Admin) {
        std::wstring file;
        std::wstring parameters;
        if (!SplitCommandLine(expanded, file, parameters)) {
            return false;
        }
        SHELLEXECUTEINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        info.hwnd = ctx.owner;
        info.lpVerb = L"runas";
        info.lpFile = file.c_str();
        info.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
        info.lpDirectory = workingDir.empty() ? nullptr : workingDir.c_str();
        info.nShow = ShowWindowToShowCmd(command.showWindow);
        if (ShellExecuteExW(&info)) {
            if (info.hProcess) {
                CloseHandle(info.hProcess);
            }
            return true;
        }
        return false;
    }

    std::vector<wchar_t> mutableCommand(expanded.begin(), expanded.end());
    mutableCommand.push_back(L'\0');
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = static_cast<WORD>(ShowWindowToShowCmd(command.showWindow));
    PROCESS_INFORMATION pi = {};
    const BOOL ok = CreateProcessW(
        nullptr, mutableCommand.data(), nullptr, nullptr, FALSE, 0, nullptr,
        workingDir.empty() ? nullptr : workingDir.c_str(), &si, &pi);
    if (ok) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return true;
    }

    std::wstring file;
    std::wstring parameters;
    if (!SplitCommandLine(expanded, file, parameters)) {
        return false;
    }
    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    info.hwnd = ctx.owner;
    info.lpFile = file.c_str();
    info.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    info.lpDirectory = workingDir.empty() ? nullptr : workingDir.c_str();
    info.nShow = ShowWindowToShowCmd(command.showWindow);
    if (ShellExecuteExW(&info)) {
        if (info.hProcess) {
            CloseHandle(info.hProcess);
        }
        return true;
    }
    return false;
}

// Invokes a menu item through the live context object using its descriptor
// (canonical verb or command offset).
// Fills a CMINVOKECOMMANDINFOEX for the item. Both the ANSI lpVerb and the
// wide lpVerbW are set: the shell reads lpVerb to tell offsets from verbs,
// and wide-only descriptors are ignored (observed on real Windows).
void FillInvokeCommandInfo(const MenuItem& item, const InvocationContext& ctx,
                           std::string& ansiStorage, CMINVOKECOMMANDINFOEX& info);

// Maps a view action to its documented folder view mode.
FOLDERVIEWMODE FolderViewModeFor(ViewAction action) {
    switch (action) {
        case ViewAction::ViewExtraLargeIcons:
        case ViewAction::ViewLargeIcons:
        case ViewAction::ViewMediumIcons:
            return FVM_ICON;
        case ViewAction::ViewSmallIcons:
            return FVM_SMALLICON;
        case ViewAction::ViewList:
            return FVM_LIST;
        case ViewAction::ViewDetails:
            return FVM_DETAILS;
        case ViewAction::ViewTiles:
            return FVM_TILE;
        case ViewAction::ViewContent:
            return FVM_CONTENT;
        default:
            return FVM_ICON;
    }
}

bool IsIconViewMode(ViewAction action) {
    return action == ViewAction::ViewExtraLargeIcons ||
           action == ViewAction::ViewLargeIcons ||
           action == ViewAction::ViewMediumIcons ||
           action == ViewAction::ViewSmallIcons;
}

bool EqualPropertyKey(const PROPERTYKEY& left, const PROPERTYKEY& right) {
    return IsEqualGUID(left.fmtid, right.fmtid) && left.pid == right.pid;
}

const PROPERTYKEY kNullPropertyKey = {};

bool InvokeContextItem(IContextMenu* context, const MenuItem& item,
                       const InvocationContext& ctx) {
    if (!context) {
        return false;
    }
    std::string ansiStorage;
    CMINVOKECOMMANDINFOEX info = {};
    FillInvokeCommandInfo(item, ctx, ansiStorage, info);
    return SUCCEEDED(context->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&info)));
}

// Active shell view for the menu owner, or nullptr.
IShellView* GetActiveShellView(HWND owner, ShellViewKind kind) {
    IShellBrowser* browser = nullptr;
    if (kind == ShellViewKind::Desktop) {
        browser = GetDesktopShellBrowser();
    } else if (kind == ShellViewKind::ShellDefView) {
        browser = GetShellBrowserForWindow(owner);
        if (browser) {
            browser->AddRef();  // borrowed pointer from CWM_GETISHELLBROWSER
        }
    }
    if (!browser) {
        return nullptr;
    }

    IShellView* view = nullptr;
    browser->QueryActiveShellView(&view);
    browser->Release();
    return view;
}

// Current folder shown by the view: the target directory for New items.
std::wstring GetCurrentFolderPath(HWND owner, ShellViewKind kind) {
    IShellView* view = GetActiveShellView(owner, kind);
    if (!view) {
        return L"";
    }

    std::wstring path;
    IFolderView* folderView = nullptr;
    if (SUCCEEDED(view->QueryInterface(IID_IFolderView, (void**)&folderView)) &&
        folderView) {
        IShellFolder* folder = nullptr;
        if (SUCCEEDED(folderView->GetFolder(IID_IShellFolder, (void**)&folder)) &&
            folder) {
            IPersistFolder2* persist = nullptr;
            if (SUCCEEDED(folder->QueryInterface(IID_IPersistFolder2, (void**)&persist)) &&
                persist) {
                PIDLIST_ABSOLUTE pidl = nullptr;
                if (SUCCEEDED(persist->GetCurFolder(&pidl)) && pidl) {
                    wchar_t buffer[MAX_PATH] = {};
                    if (SHGetPathFromIDListW(pidl, buffer)) {
                        path = buffer;
                    }
                    CoTaskMemFree(pidl);
                }
                persist->Release();
            }
            folder->Release();
        }
        folderView->Release();
    }
    view->Release();
    return path;
}

// Selects a newly created item in the view, optionally in rename mode.
void SelectCreatedItem(HWND owner, ShellViewKind kind, const std::wstring& path,
                       bool edit) {
    IShellView* view = GetActiveShellView(owner, kind);
    if (!view) {
        return;
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHILCreateFromPath(path.c_str(), &pidl, nullptr)) && pidl) {
        SVSIF flags =
            SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED | SVSI_SELECT;
        if (edit) {
            flags |= SVSI_EDIT;
        }
        view->SelectItem(ILFindLastID(pidl), flags);
        CoTaskMemFree(pidl);
    }
    view->Release();
}

// Creates the New submenu item chosen from the core model.
bool CreateNewItemFromTemplate(const MenuItem& item, const InvocationContext& ctx) {
    if (item.newIndex >= g_newTemplates.size()) {
        return false;
    }
    const NewTemplate& tmpl = g_newTemplates[item.newIndex];
    const std::wstring directory = GetCurrentFolderPath(ctx.owner, ctx.kind);
    if (directory.empty()) {
        return false;
    }

    std::wstring createdPath;
    if (!CreateNewItemInFolder(tmpl, directory, createdPath)) {
        return false;
    }

    if (!createdPath.empty()) {
        const DWORD event =
            tmpl.kind == NewTemplate::Kind::Folder ? SHCNE_MKDIR : SHCNE_CREATE;
        SHChangeNotify(event, SHCNF_PATHW | SHCNF_FLUSH, createdPath.c_str(), nullptr);
        const bool edit = tmpl.kind != NewTemplate::Kind::Shortcut &&
                          tmpl.kind != NewTemplate::Kind::Command;
        SelectCreatedItem(ctx.owner, ctx.kind, createdPath, edit);
    }
    return true;
}

std::wstring MakeShortcutPath(const std::wstring& directory,
                              const std::wstring& sourceName);

bool CreateShortcutForPaths(const std::vector<std::wstring>& paths) {
    if (paths.empty()) {
        return false;
    }

    bool ok = false;
    for (const std::wstring& path : paths) {
        std::wstring directory = path;
        const size_t slash = directory.find_last_of(L"\\/");
        if (slash != std::wstring::npos) {
            directory.resize(slash);
        } else {
            directory.clear();
        }
        std::wstring name = path.substr(slash == std::wstring::npos ? 0 : slash + 1);
        const size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0) {
            name.resize(dot);
        }

        // Explorer never overwrites an existing shortcut; MakeShortcutPath
        // adds the usual " (2)" suffix when the name is taken.
        std::wstring target =
            directory.empty() ? name + L" - Shortcut.lnk"
                              : MakeShortcutPath(directory, name);

        IShellLinkW* link = nullptr;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_IShellLinkW, (void**)&link)) ||
            !link) {
            continue;
        }
        link->SetPath(path.c_str());
        if (!directory.empty()) {
            link->SetWorkingDirectory(directory.c_str());
        }

        IPersistFile* file = nullptr;
        if (SUCCEEDED(link->QueryInterface(IID_IPersistFile, (void**)&file)) && file) {
            if (SUCCEEDED(file->Save(target.c_str(), TRUE))) {
                ok = true;
            }
            file->Release();
        }
        link->Release();
    }
    return ok;
}

// The active view as IFolderView2, used for sorting and grouping.
IFolderView2* GetFolderView2(HWND owner, ShellViewKind kind) {
    IShellView* view = GetActiveShellView(owner, kind);
    if (!view) {
        return nullptr;
    }
    IFolderView2* folderView = nullptr;
    view->QueryInterface(IID_IFolderView2, (void**)&folderView);
    view->Release();
    return folderView;
}

bool InvokeSortBy(const MenuItem& item, const InvocationContext& ctx) {
    if (item.sortIndex >= kShellPropertyKeyCount) {
        return false;
    }
    IFolderView2* view = GetFolderView2(ctx.owner, ctx.kind);
    if (!view) {
        return false;
    }
    SORTCOLUMN column = {};
    column.propkey = kShellPropertyKeys[item.sortIndex];
    column.direction = item.sortAscending ? 1 : -1;
    const bool ok = SUCCEEDED(view->SetSortColumns(&column, 1));
    view->Release();
    return ok;
}

bool InvokeSortDirection(const MenuItem& item, const InvocationContext& ctx) {
    IFolderView2* view = GetFolderView2(ctx.owner, ctx.kind);
    if (!view) {
        return false;
    }

    SORTCOLUMN column = {};
    column.propkey = kShellPropertyKeys[0];  // default to Name
    int count = 0;
    if (SUCCEEDED(view->GetSortColumnCount(&count)) && count > 0) {
        SORTCOLUMN current = {};
        if (SUCCEEDED(view->GetSortColumns(&current, 1))) {
            column.propkey = current.propkey;
        }
    }
    column.direction = item.sortAscending ? 1 : -1;
    const bool ok = SUCCEEDED(view->SetSortColumns(&column, 1));
    view->Release();
    return ok;
}

bool InvokeGroupBy(const MenuItem& item, const InvocationContext& ctx) {
    IFolderView2* view = GetFolderView2(ctx.owner, ctx.kind);
    if (!view) {
        return false;
    }
    const PROPERTYKEY key = item.sortIndex == kGroupNoneIndex
                                ? kNullPropertyKey
                                : kShellPropertyKeys[item.sortIndex];
    const bool ok = SUCCEEDED(view->SetGroupBy(key, TRUE));
    view->Release();
    return ok;
}

bool InvokeGroupDirection(const MenuItem& item, const InvocationContext& ctx) {
    IFolderView2* view = GetFolderView2(ctx.owner, ctx.kind);
    if (!view) {
        return false;
    }
    PROPERTYKEY key = {};
    WINBOOL ascending = TRUE;
    bool ok = false;
    if (SUCCEEDED(view->GetGroupBy(&key, &ascending)) &&
        !EqualPropertyKey(key, kNullPropertyKey)) {
        ok = SUCCEEDED(view->SetGroupBy(key, item.sortAscending ? TRUE : FALSE));
    }
    view->Release();
    return ok;
}

// Marks the View / Sort by / Group by entries that match the view's current
// state, so the menu shows the same dots as the shell's own submenus.
void ApplyViewStateChecks(std::vector<MenuItem>& items, HWND owner,
                          ShellViewKind kind) {
    IFolderView2* view = GetFolderView2(owner, kind);
    if (!view) {
        return;
    }

    FOLDERVIEWMODE mode = FVM_AUTO;
    int iconSize = -1;
    const bool haveMode = SUCCEEDED(view->GetViewModeAndIconSize(&mode, &iconSize));
    DWORD folderFlags = 0;
    const bool haveFlags = SUCCEEDED(view->GetCurrentFolderFlags(&folderFlags));

    SORTCOLUMN sortColumn = {};
    int sortCount = 0;
    const bool haveSort =
        SUCCEEDED(view->GetSortColumnCount(&sortCount)) && sortCount > 0 &&
        SUCCEEDED(view->GetSortColumns(&sortColumn, 1));

    PROPERTYKEY groupKey = {};
    WINBOOL groupAscending = TRUE;
    const bool haveGroup = SUCCEEDED(view->GetGroupBy(&groupKey, &groupAscending));
    const bool groupActive = haveGroup && !EqualPropertyKey(groupKey, kNullPropertyKey);

    for (MenuItem& submenu : items) {
        if (submenu.kind != ItemKind::Submenu) {
            continue;
        }
        for (MenuItem& child : submenu.children) {
            if (child.action == ActionKind::ViewAction) {
                const ViewAction action = static_cast<ViewAction>(child.viewAction);
                if (!haveMode) {
                    continue;
                }
                if (IsIconViewMode(action) && child.iconSize >= 0) {
                    if (mode == FVM_ICON && iconSize == child.iconSize) {
                        child.flags |= kModelChecked;
                    }
                } else if (action == ViewAction::ViewSmallIcons) {
                    if (mode == FVM_SMALLICON || (mode == FVM_ICON && iconSize == 16)) {
                        child.flags |= kModelChecked;
                    }
                } else if (action == ViewAction::ViewList) {
                    if (mode == FVM_LIST) {
                        child.flags |= kModelChecked;
                    }
                } else if (action == ViewAction::ViewDetails) {
                    if (mode == FVM_DETAILS) {
                        child.flags |= kModelChecked;
                    }
                } else if (action == ViewAction::ViewTiles) {
                    if (mode == FVM_TILE) {
                        child.flags |= kModelChecked;
                    }
                } else if (action == ViewAction::ViewContent) {
                    if (mode == FVM_CONTENT) {
                        child.flags |= kModelChecked;
                    }
                } else if (haveFlags && action == ViewAction::AutoArrange) {
                    if (folderFlags & FWF_AUTOARRANGE) {
                        child.flags |= kModelChecked;
                    }
                } else if (haveFlags && action == ViewAction::AlignToGrid) {
                    if (folderFlags & FWF_SNAPTOGRID) {
                        child.flags |= kModelChecked;
                    }
                }
            } else if (child.action == ActionKind::SortBy) {
                if (haveSort && child.sortIndex < kShellPropertyKeyCount &&
                    EqualPropertyKey(kShellPropertyKeys[child.sortIndex],
                                     sortColumn.propkey)) {
                    child.flags |= kModelChecked;
                }
            } else if (child.action == ActionKind::SortDirection) {
                if (haveSort &&
                    (sortColumn.direction > 0) == child.sortAscending) {
                    child.flags |= kModelChecked;
                }
            } else if (child.action == ActionKind::GroupBy) {
                if (child.sortIndex == kGroupNoneIndex) {
                    if (haveGroup && !groupActive) {
                        child.flags |= kModelChecked;
                    }
                } else if (groupActive && child.sortIndex < kShellPropertyKeyCount &&
                           EqualPropertyKey(kShellPropertyKeys[child.sortIndex],
                                            groupKey)) {
                    child.flags |= kModelChecked;
                }
            } else if (child.action == ActionKind::GroupDirection) {
                if (groupActive && (groupAscending != FALSE) == child.sortAscending) {
                    child.flags |= kModelChecked;
                }
            }
        }
    }
    view->Release();
}

// Dispatches a documented view operation through IFolderView2 / IShellView.
InvokeResult InvokeViewAction(const MenuItem& item, const InvocationContext& ctx) {
    IShellView* view = GetActiveShellView(ctx.owner, ctx.kind);
    if (!view) {
        return InvokeResult::FallbackNative;
    }

    InvokeResult result = InvokeResult::FallbackNative;
    const ViewAction action = static_cast<ViewAction>(item.viewAction);

    if (action == ViewAction::Refresh) {
        result = SUCCEEDED(view->Refresh()) ? InvokeResult::Handled
                                            : InvokeResult::FallbackNative;
    } else {
        IFolderView2* folderView = nullptr;
        if (SUCCEEDED(view->QueryInterface(IID_IFolderView2, (void**)&folderView)) &&
            folderView) {
            switch (action) {
                case ViewAction::Rename:
                    result = SUCCEEDED(folderView->DoRename())
                                 ? InvokeResult::Handled
                                 : InvokeResult::FallbackNative;
                    break;
                case ViewAction::ViewExtraLargeIcons:
                case ViewAction::ViewLargeIcons:
                case ViewAction::ViewMediumIcons:
                case ViewAction::ViewSmallIcons:
                case ViewAction::ViewList:
                case ViewAction::ViewDetails:
                case ViewAction::ViewTiles:
                case ViewAction::ViewContent: {
                    const FOLDERVIEWMODE mode = FolderViewModeFor(action);
                    const int iconSize = IsIconViewMode(action) ? item.iconSize : -1;
                    result = SUCCEEDED(folderView->SetViewModeAndIconSize(mode, iconSize))
                                 ? InvokeResult::Handled
                                 : InvokeResult::FallbackNative;
                    break;
                }
                case ViewAction::AutoArrange:
                case ViewAction::AlignToGrid: {
                    DWORD flags = 0;
                    const DWORD mask = action == ViewAction::AutoArrange
                                           ? FWF_AUTOARRANGE
                                           : FWF_SNAPTOGRID;
                    if (SUCCEEDED(folderView->GetCurrentFolderFlags(&flags))) {
                        result = SUCCEEDED(
                                     folderView->SetCurrentFolderFlags(mask, flags ^ mask))
                                     ? InvokeResult::Handled
                                     : InvokeResult::FallbackNative;
                    }
                    break;
                }
                default:
                    break;
            }
            folderView->Release();
        }
    }

    view->Release();
    return result;
}

bool CopyAsPath(const std::vector<std::wstring>& paths) {
    std::wstring text;
    for (const std::wstring& path : paths) {
        if (!text.empty()) {
            text += L"\r\n";
        }
        text += L'"';
        text += path;
        text += L'"';
    }
    if (text.empty()) {
        return false;
    }

    if (!OpenClipboard(nullptr)) {
        return false;
    }
    bool ok = false;
    if (EmptyClipboard()) {
        const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
        if (HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
            if (void* data = GlobalLock(memory)) {
                memcpy(data, text.c_str(), bytes);
                GlobalUnlock(memory);
                if (SetClipboardData(CF_UNICODETEXT, memory)) {
                    ok = true;
                } else {
                    GlobalFree(memory);
                }
            } else {
                GlobalFree(memory);
            }
        }
    }
    CloseClipboard();
    return ok;
}

bool ClipboardHasFileData() {
    return IsClipboardFormatAvailable(CF_HDROP) != FALSE;
}

// Paste availability: cached state while the clipboard sequence is unchanged,
// otherwise the freshly checked state.
bool ComputePasteEnabled(DWORD sequenceAtOpen, DWORD currentSequence,
                         bool cachedHadData, bool currentHasData) {
    return sequenceAtOpen == currentSequence ? cachedHadData : currentHasData;
}

// Reads the clipboard's CF_HDROP file list plus the preferred drop effect
// (DROPEFFECT_MOVE means the files were cut).
bool ReadClipboardFiles(std::vector<std::wstring>& paths, DWORD& dropEffect) {
    paths.clear();
    dropEffect = DROPEFFECT_COPY;
    if (!OpenClipboard(nullptr)) {
        return false;
    }
    bool ok = false;
    if (HANDLE handle = GetClipboardData(CF_HDROP)) {
        HDROP drop = static_cast<HDROP>(handle);
        const UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
        for (UINT i = 0; i < count; ++i) {
            const UINT length = DragQueryFileW(drop, i, nullptr, 0);
            std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
            if (DragQueryFileW(drop, i, buffer.data(), length + 1) == length) {
                paths.emplace_back(buffer.data());
            }
        }
        ok = !paths.empty();
    }
    if (HANDLE handle = GetClipboardData(
            RegisterClipboardFormatW(L"Preferred DropEffect"))) {
        if (const DWORD* effect =
                static_cast<const DWORD*>(GlobalLock(handle))) {
            dropEffect = *effect;
            GlobalUnlock(handle);
        }
    }
    CloseClipboard();
    return ok;
}

// Unique "<name> - Shortcut.lnk" path in a directory.
std::wstring MakeShortcutPath(const std::wstring& directory,
                              const std::wstring& sourceName) {
    std::wstring base = sourceName;
    const size_t dot = base.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) {
        base.resize(dot);
    }
    std::wstring candidate =
        directory + L"\\" + base + L" - Shortcut.lnk";
    for (int i = 2;
         i < 1000 &&
         GetFileAttributesW(candidate.c_str()) != INVALID_FILE_ATTRIBUTES;
         ++i) {
        candidate = directory + L"\\" + base + L" - Shortcut (" +
                    std::to_wstring(i) + L").lnk";
    }
    return candidate;
}

// Paste: copies (or moves, when the clipboard was cut) the clipboard files
// into the view's current folder. Explorer's view adds the standard Paste item
// itself, so the captured shell menu never contains it and the mod performs
// the operation directly.
bool InvokePaste(const InvocationContext& ctx) {
    if (ctx.directory.empty()) {
        return false;
    }
    std::vector<std::wstring> sources;
    DWORD dropEffect = DROPEFFECT_COPY;
    if (!ReadClipboardFiles(sources, dropEffect)) {
        return false;
    }

    IFileOperation* operation = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_IFileOperation,
                                reinterpret_cast<void**>(&operation))) ||
        !operation) {
        return false;
    }
    operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR);

    IShellItem* destination = nullptr;
    bool ok = false;
    if (SUCCEEDED(SHCreateItemFromParsingName(ctx.directory.c_str(), nullptr,
                                              IID_IShellItem,
                                              reinterpret_cast<void**>(&destination))) &&
        destination) {
        const bool move = (dropEffect & DROPEFFECT_MOVE) != 0;
        ok = true;
        for (const std::wstring& source : sources) {
            IShellItem* item = nullptr;
            if (FAILED(SHCreateItemFromParsingName(
                    source.c_str(), nullptr, IID_IShellItem,
                    reinterpret_cast<void**>(&item))) ||
                !item) {
                ok = false;
                continue;
            }
            const HRESULT hr =
                move ? operation->MoveItem(item, destination, nullptr, nullptr)
                     : operation->CopyItem(item, destination, nullptr, nullptr);
            if (FAILED(hr)) {
                ok = false;
            }
            item->Release();
        }
        destination->Release();
    }
    if (ok) {
        ok = SUCCEEDED(operation->PerformOperations());
        BOOL aborted = FALSE;
        if (ok && SUCCEEDED(operation->GetAnyOperationsAborted(&aborted)) &&
            aborted) {
            ok = false;
        }
    }
    operation->Release();
    if (g_settings->debugLogging) {
        Wh_Log(L"Paste: %zu item(s) -> %s (move=%d, ok=%d)", sources.size(),
               ctx.directory.c_str(), (dropEffect & DROPEFFECT_MOVE) ? 1 : 0,
               ok ? 1 : 0);
    }
    return ok;
}

// Paste shortcut: creates .lnk files in the current folder for the clipboard
// files.
bool InvokePasteShortcut(const InvocationContext& ctx) {
    if (ctx.directory.empty()) {
        return false;
    }
    std::vector<std::wstring> sources;
    DWORD dropEffect = DROPEFFECT_COPY;
    if (!ReadClipboardFiles(sources, dropEffect)) {
        return false;
    }
    bool ok = false;
    for (const std::wstring& source : sources) {
        const size_t slash = source.find_last_of(L"\\/");
        const std::wstring name =
            source.substr(slash == std::wstring::npos ? 0 : slash + 1);
        const std::wstring target = MakeShortcutPath(ctx.directory, name);

        IShellLinkW* link = nullptr;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_IShellLinkW,
                                    reinterpret_cast<void**>(&link))) ||
            !link) {
            continue;
        }
        link->SetPath(source.c_str());
        IPersistFile* file = nullptr;
        if (SUCCEEDED(link->QueryInterface(IID_IPersistFile,
                                           reinterpret_cast<void**>(&file))) &&
            file) {
            if (SUCCEEDED(file->Save(target.c_str(), TRUE))) {
                ok = true;
            }
            file->Release();
        }
        link->Release();
    }
    if (g_settings->debugLogging) {
        Wh_Log(L"Paste shortcut: %zu item(s) -> %s (ok=%d)", sources.size(),
               ctx.directory.c_str(), ok ? 1 : 0);
    }
    return ok;
}

// Open with: shows the shell's "How do you want to open this file?" dialog.
// The shell's classic menu exposes "Open with" as a submenu, which cannot be
// invoked by offset, so the mod shows the dialog directly.
bool InvokeOpenWith(const InvocationContext& ctx) {
    if (ctx.paths.empty()) {
        return false;
    }
    // The shell shows one dialog for the whole selection; SHOpenWithDialog
    // takes a single file, so use the first path like the native verb does.
    OPENASINFO info = {};
    info.pcszFile = ctx.paths.front().c_str();
    info.pcszClass = nullptr;
    info.oaifInFlags = OAIF_ALLOW_REGISTRATION | OAIF_EXEC;
    const bool ok = SUCCEEDED(SHOpenWithDialog(ctx.owner, &info));
    if (g_settings->debugLogging) {
        Wh_Log(L"Open with: %zu file(s), ok=%d", ctx.paths.size(), ok ? 1 : 0);
    }
    return ok;
}

// Executes a SendTo shortcut with the selected paths as arguments.
bool InvokeSendTo(const std::wstring& target, const std::vector<std::wstring>& paths) {
    if (target.empty() || paths.empty()) {
        return false;
    }

    std::wstring parameters;
    for (const std::wstring& path : paths) {
        if (!parameters.empty()) {
            parameters += L' ';
        }
        parameters += L'"';
        parameters += path;
        parameters += L'"';
    }

    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
    info.lpFile = target.c_str();
    info.lpParameters = parameters.c_str();
    info.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&info) != FALSE;
}

// Fills an invocation descriptor. Both fields are populated deliberately:
// the shell's default context menu reads lpVerb (ANSI) to decide between an
// offset (MAKEINTRESOURCE) and a canonical verb, and ignores wide-only input.
void FillInvokeCommandInfo(const MenuItem& item, const InvocationContext& ctx,
                           std::string& ansiStorage, CMINVOKECOMMANDINFOEX& info) {
    info = {};
    info.cbSize = sizeof(info);
    info.fMask = CMIC_MASK_UNICODE;
    info.hwnd = ctx.owner;
    info.nShow = SW_SHOWNORMAL;
    info.ptInvoke = ctx.pt;

    auto descriptor = ChooseInvokeDescriptor(item);
    if (!descriptor.first.empty()) {
        const int length = WideCharToMultiByte(CP_ACP, 0, descriptor.first.c_str(), -1,
                                               nullptr, 0, nullptr, nullptr);
        if (length > 0) {
            ansiStorage.resize(static_cast<size_t>(length));
            WideCharToMultiByte(CP_ACP, 0, descriptor.first.c_str(), -1,
                                ansiStorage.data(), length, nullptr, nullptr);
            ansiStorage.resize(static_cast<size_t>(length - 1));
            info.lpVerb = ansiStorage.c_str();
        }
        info.lpVerbW = descriptor.first.c_str();
    } else {
        info.lpVerb = MAKEINTRESOURCEA(descriptor.second);
        info.lpVerbW = MAKEINTRESOURCEW(descriptor.second);
    }
}


// Runs the real population once per open on the live object, retaining the
// populated menu for discovery and the native fallback. Idempotent.
bool EnsureContextPopulated(PendingCapture& capture) {
    if (capture.populated) {
        return true;
    }
    if (!capture.obj) {
        return false;
    }

    const std::vector<std::wstring> modulesBefore = SnapshotLoadedModules();
    const ULONGLONG start = GetTickCount64();

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return false;
    }
    // Extensions may attach bitmaps with SetMenuItemBitmaps during population;
    // start a fresh recording so lookups match this menu.
    ClearRecordedItemBitmaps();
    ReplayInto(capture.obj, menu, capture.indexMenu, capture.idCmdFirst, capture.idCmdLast,
               capture.flags);

    capture.populatedMenu = menu;
    capture.populated = true;
    capture.used = true;
    capture.handlerModules = DiffModules(modulesBefore, SnapshotLoadedModules());
    capture.sourceStamp = ComputeModuleStamp(capture.handlerModules);

    capture.obj->QueryInterface(IID_IContextMenu3, (void**)&capture.contextMenu3);
    capture.obj->QueryInterface(IID_IContextMenu2, (void**)&capture.contextMenu2);

    if (g_settings->debugLogging) Wh_Log(L"Population: %llu ms, %d items",
           static_cast<unsigned long long>(GetTickCount64() - start),
           GetMenuItemCount(menu));
    return true;
}

// Invokes a cached extension item through the live context object, preferring
// its native offset (verb strings are rejected by the shell's own menu).
std::optional<uint32_t> FindNativeOffsetInMenu(HMENU menu, UINT idCmdFirst,
                                               IContextMenu* context,
                                               const MenuItem& target);

InvokeResult InvokeExtensionItem(const MenuItem& item, const InvocationContext& ctx,
                                 PendingCapture& capture) {
    if (item.flags & kModelOwnerDraw) {
        return InvokeResult::FallbackNative;
    }
    if (!ctx.liveContext || !EnsureContextPopulated(capture)) {
        return InvokeResult::FallbackNative;
    }
    // Submenus are populated lazily; initialize them so items nested in a
    // submenu (e.g. "Give access to" children) can be resolved.
    if (!capture.menuInitialized) {
        InitializeMenuRecursive(capture, capture.populatedMenu, 0);
        capture.menuInitialized = true;
    }

    // Resolve the item in the live menu; the cached offset is from discovery
    // time and a shifted layout would invoke the wrong command (e.g. "Restore
    // previous versions" hitting Properties).
    if (auto nativeOffset = FindNativeOffsetInMenu(
            capture.populatedMenu, capture.idCmdFirst, ctx.liveContext, item)) {
        MenuItem offsetItem = item;
        offsetItem.canonicalVerb.clear();
        offsetItem.verbOffset = *nativeOffset;
        if (InvokeContextItem(ctx.liveContext, offsetItem, ctx)) {
            return InvokeResult::Handled;
        }
    }
    if (!item.canonicalVerb.empty() &&
        InvokeContextItem(ctx.liveContext, item, ctx)) {
        return InvokeResult::Handled;
    }
    if (g_settings->debugLogging) {
        Wh_Log(L"Extension '%s': no live match (verb '%s'); using the native "
               L"menu",
               item.label.c_str(), item.canonicalVerb.c_str());
    }
    return InvokeResult::FallbackNative;
}

// Finds the offset of the native item matching a core item, by canonical
// verb first and label second. Recurses into submenus.
std::optional<uint32_t> FindNativeOffsetInMenu(HMENU menu, UINT idCmdFirst,
                                               IContextMenu* context,
                                               const MenuItem& target) {
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, static_cast<UINT>(i), TRUE, &info)) {
            continue;
        }
        if (info.fType & MFT_SEPARATOR) {
            continue;
        }
        if (info.hSubMenu) {
            if (auto found = FindNativeOffsetInMenu(info.hSubMenu, idCmdFirst, context,
                                                    target)) {
                return found;
            }
            continue;
        }
        if (info.wID < idCmdFirst) {
            continue;
        }
        const uint32_t offset = info.wID - idCmdFirst;

        if (context && !target.canonicalVerb.empty()) {
            wchar_t verb[128] = {};
            if (SUCCEEDED(context->GetCommandString(
                    static_cast<UINT_PTR>(offset), GCS_VERBW, nullptr,
                    reinterpret_cast<LPSTR>(verb), ARRAYSIZE(verb))) &&
                _wcsicmp(verb, target.canonicalVerb.c_str()) == 0) {
                return offset;
            }
        }
        if (!target.label.empty()) {
            wchar_t label[512] = {};
            const int length = GetMenuStringW(menu, static_cast<UINT>(i), label,
                                              ARRAYSIZE(label), MF_BYPOSITION);
            if (length > 0 && _wcsicmp(label, target.label.c_str()) == 0) {
                return offset;
            }
        }
    }
    return std::nullopt;
}

std::vector<std::wstring> BuiltinActionTargets(BuiltinAction action,
                                               const InvocationContext& ctx) {
    std::vector<std::wstring> targets = ctx.paths;
    if (targets.empty() && !ctx.directory.empty()) {
        targets.push_back(ctx.directory);
    }
    if (action == BuiltinAction::Properties && targets.size() > 1) {
        targets.resize(1);
    }
    return targets;
}

bool InvokeBuiltinAction(const MenuItem& item, const InvocationContext& ctx) {
    if (item.builtinAction == BuiltinAction::OpenSettings) {
        POINT pt = {};
        GetCursorPos(&pt);
        OpenSettingsMenu(ctx.owner, pt);
        return true;
    }
    const std::vector<std::wstring> targets =
        BuiltinActionTargets(item.builtinAction, ctx);
    if (targets.empty()) {
        return false;
    }
    switch (item.builtinAction) {
        case BuiltinAction::OpenSettings:
            return false;  // handled before the target check / not invokable
        case BuiltinAction::CopyPath:
            return CopyAsPath(targets);
        case BuiltinAction::OpenNewWindow: {
            bool any = false;
            for (const std::wstring& folder : targets) {
                SHELLEXECUTEINFOW info = {};
                info.cbSize = sizeof(info);
                info.fMask = SEE_MASK_FLAG_NO_UI;
                info.hwnd = ctx.owner;
                info.lpVerb = L"explore";
                info.lpFile = folder.c_str();
                info.nShow = SW_SHOWNORMAL;
                any = ShellExecuteExW(&info) != FALSE || any;
            }
            return any;
        }
        case BuiltinAction::OpenNewProcess: {
            // explorer.exe /separate starts an isolated process for the folder.
            bool any = false;
            for (const std::wstring& folder : targets) {
                const std::wstring parameters =
                    L"/separate,\"" + folder + L"\"";
                SHELLEXECUTEINFOW info = {};
                info.cbSize = sizeof(info);
                info.fMask = SEE_MASK_FLAG_NO_UI;
                info.hwnd = ctx.owner;
                info.lpFile = L"explorer.exe";
                info.lpParameters = parameters.c_str();
                info.nShow = SW_SHOWNORMAL;
                any = ShellExecuteExW(&info) != FALSE || any;
            }
            return any;
        }
        case BuiltinAction::Properties: {
            if (SHObjectProperties(ctx.owner, SHOP_FILEPATH, targets.front().c_str(),
                                   nullptr)) {
                return true;
            }
            SHELLEXECUTEINFOW info = {};
            info.cbSize = sizeof(info);
            info.fMask = SEE_MASK_FLAG_NO_UI;
            info.hwnd = ctx.owner;
            info.lpVerb = L"properties";
            info.lpFile = targets.front().c_str();
            info.nShow = SW_SHOWNORMAL;
            return ShellExecuteExW(&info) != FALSE;
        }
        case BuiltinAction::None:
            return false;
    }
    return false;
}

InvokeResult InvokeItem(const MenuItem& item, const InvocationContext& ctx,
                        PendingCapture& capture) {
    if (item.kind == ItemKind::Separator || (item.flags & kModelDisabled)) {
        return InvokeResult::Handled;
    }

    switch (item.action) {
        case ActionKind::Fallback:
            return InvokeResult::FallbackNative;
        case ActionKind::Submenu:
            return InvokeResult::Handled;
        case ActionKind::NewItem:
            return CreateNewItemFromTemplate(item, ctx) ? InvokeResult::Handled
                                                        : InvokeResult::Failed;
        case ActionKind::SortBy:
            return InvokeSortBy(item, ctx) ? InvokeResult::Handled
                                           : InvokeResult::Failed;
        case ActionKind::SortDirection:
            return InvokeSortDirection(item, ctx) ? InvokeResult::Handled
                                                  : InvokeResult::Failed;
        case ActionKind::GroupBy:
            return InvokeGroupBy(item, ctx) ? InvokeResult::Handled
                                            : InvokeResult::Failed;
        case ActionKind::GroupDirection:
            return InvokeGroupDirection(item, ctx) ? InvokeResult::Handled
                                                   : InvokeResult::Failed;
        case ActionKind::Builtin:
            return InvokeBuiltinAction(item, ctx) ? InvokeResult::Handled
                                                  : InvokeResult::Failed;
        case ActionKind::CustomCommand:
            if (ctx.config &&
                item.customCommandIndex < ctx.config->commands.size() &&
                InvokeCustomCommand(ctx.config->commands[item.customCommandIndex],
                                    ctx)) {
                return InvokeResult::Handled;
            }
            return InvokeResult::Failed;
        case ActionKind::ViewAction:
            return InvokeViewAction(item, ctx);
        case ActionKind::ShellVerb:
            if (item.canonicalVerb == L"copyaspath") {
                return CopyAsPath(ctx.paths) ? InvokeResult::Handled
                                             : InvokeResult::Failed;
            }
            if (item.canonicalVerb == L"sendto") {
                return InvokeSendTo(item.targetPath, ctx.paths)
                           ? InvokeResult::Handled
                           : InvokeResult::FallbackNative;
            }
            if (item.canonicalVerb == L"createshortcut") {
                return CreateShortcutForPaths(ctx.paths) ? InvokeResult::Handled
                                                         : InvokeResult::FallbackNative;
            }
            if (item.canonicalVerb == L"openwith") {
                return InvokeOpenWith(ctx) ? InvokeResult::Handled
                                           : InvokeResult::FallbackNative;
            }
            if (item.canonicalVerb == L"paste") {
                const DWORD currentSequence = GetClipboardSequenceNumber();
                const bool currentHasData =
                    currentSequence == ctx.clipboardSequence ? ctx.clipboardHadData
                                                             : ClipboardHasFileData();
                if (!ComputePasteEnabled(ctx.clipboardSequence, currentSequence,
                                         ctx.clipboardHadData, currentHasData)) {
                    return InvokeResult::Handled;
                }
                // The captured shell menu never contains the view's standard
                // Paste item, so perform it ourselves.
                return InvokePaste(ctx) ? InvokeResult::Handled
                                        : InvokeResult::FallbackNative;
            }
            if (item.canonicalVerb == L"pastelink") {
                return InvokePasteShortcut(ctx) ? InvokeResult::Handled
                                                : InvokeResult::FallbackNative;
            }
            // The shell only knows its own commands once the object has been
            // populated. Verb strings are rejected by the default context
            // menu on real Windows, while its own offsets always dispatch,
            // so prefer the offset and fall back to the verb last.
            if (!EnsureContextPopulated(capture)) {
                return InvokeResult::FallbackNative;
            }
            if (!capture.menuInitialized) {
                InitializeMenuRecursive(capture, capture.populatedMenu, 0);
                capture.menuInitialized = true;
            }
            // The cached offset comes from the discovery-time menu layout,
            // which can differ from the current population (dynamic items
            // shift positions), so resolve the item in the live menu first and
            // only fall back to the cached offset.
            if (auto nativeOffset = FindNativeOffsetInMenu(
                    capture.populatedMenu, capture.idCmdFirst, ctx.liveContext,
                    item)) {
                MenuItem offsetItem = item;
                offsetItem.canonicalVerb.clear();
                offsetItem.verbOffset = *nativeOffset;
                if (InvokeContextItem(ctx.liveContext, offsetItem, ctx)) {
                    return InvokeResult::Handled;
                }
            }
            if (!item.canonicalVerb.empty() &&
                InvokeContextItem(ctx.liveContext, item, ctx)) {
                return InvokeResult::Handled;
            }
            if (g_settings->debugLogging) {
                Wh_Log(L"Invoke '%s': no live match (verb '%s'); using the "
                       L"native menu",
                       item.label.c_str(), item.canonicalVerb.c_str());
            }
            return InvokeResult::FallbackNative;
    }
    return InvokeResult::Failed;
}

}  // namespace cmo

// ===========================================================================
// [CMO:View] Rendering abstraction and the native implementation.
// ===========================================================================
namespace cmo {

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
inline TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
inline TrackPopupMenu_t TrackPopupMenu_Original = nullptr;

// Native menus open instantly via TPM_NOANIMATION on the individual
// TrackPopupMenuEx call; no global SPI_SETMENUANIMATION change is needed.

// Temporarily lowers the system submenu show delay (MenuShowDelay, default
// 400 ms) while the replacement menu is open; the previous value is restored
// on scope exit. The delay is never lengthened, and a configured value of -1
// leaves the system setting alone.
// Several Explorer UI threads can show native fallback menus at the same
// time; the system MenuShowDelay is a single global value, so the first menu
// saves the original and the last one restores it.
std::mutex g_menuDelayMutex;
int g_menuDelayUsers = 0;
bool g_menuDelayActive = false;
DWORD g_menuDelayOriginal = 400;

class MenuDelaySuppressor {
public:
    MenuDelaySuppressor() {
        if (g_settings->submenuDelayMs < 0) {
            return;
        }
        std::lock_guard<std::mutex> lock(g_menuDelayMutex);
        if (g_menuDelayActive) {
            ++g_menuDelayUsers;
            active_ = true;
            return;
        }
        DWORD current = 0;
        if (!SystemParametersInfoW(SPI_GETMENUSHOWDELAY, 0, &current, 0)) {
            return;
        }
        if (current <= static_cast<DWORD>(g_settings->submenuDelayMs)) {
            return;
        }
        if (!SystemParametersInfoW(SPI_SETMENUSHOWDELAY,
                                   static_cast<UINT>(g_settings->submenuDelayMs),
                                   nullptr, 0)) {
            return;
        }
        g_menuDelayActive = true;
        g_menuDelayOriginal = current;
        ++g_menuDelayUsers;
        active_ = true;
    }

    ~MenuDelaySuppressor() {
        if (!active_) {
            return;
        }
        std::lock_guard<std::mutex> lock(g_menuDelayMutex);
        if (--g_menuDelayUsers == 0 && g_menuDelayActive) {
            SystemParametersInfoW(SPI_SETMENUSHOWDELAY, g_menuDelayOriginal, nullptr,
                                  0);
            g_menuDelayActive = false;
        }
    }

private:
    bool active_ = false;
};

// Defensive restore in case a suppressor was active when the mod unloaded.
void RestoreMenuDelay() {
    std::lock_guard<std::mutex> lock(g_menuDelayMutex);
    if (g_menuDelayActive) {
        SystemParametersInfoW(SPI_SETMENUSHOWDELAY, g_menuDelayOriginal, nullptr, 0);
        g_menuDelayActive = false;
        g_menuDelayUsers = 0;
    }
}

// Parses a shell icon reference: "file,index", "file", or a quoted path with
// an optional trailing ",index". Environment expansion happens at load time.
bool ParseIconRef(std::wstring_view ref, std::wstring& path, int& index) {
    path.clear();
    index = 0;

    std::wstring_view text = ref;
    while (!text.empty() && (text.front() == L' ' || text.front() == L'\t')) {
        text.remove_prefix(1);
    }
    while (!text.empty() && (text.back() == L' ' || text.back() == L'\t')) {
        text.remove_suffix(1);
    }
    if (text.empty()) {
        return false;
    }

    if (text.front() == L'"') {
        text.remove_prefix(1);
        const size_t quote = text.find(L'"');
        if (quote == std::wstring_view::npos) {
            return false;
        }
        path.assign(text.substr(0, quote));
        text.remove_prefix(quote + 1);
        if (!text.empty() && text.front() == L',') {
            text.remove_prefix(1);
            index = _wtoi(std::wstring(text).c_str());
        }
        return !path.empty();
    }

    const size_t comma = text.find_last_of(L',');
    if (comma != std::wstring_view::npos) {
        const std::wstring_view suffix = text.substr(comma + 1);
        bool numeric = !suffix.empty();
        for (wchar_t c : suffix) {
            if (c < L'0' || c > L'9') {
                numeric = false;
                break;
            }
        }
        if (numeric) {
            path.assign(text.substr(0, comma));
            index = _wtoi(std::wstring(suffix).c_str());
            return !path.empty();
        }
    }

    path.assign(text);
    return true;
}

// Cached menu bitmaps and icons. Bitmaps are owned by this cache and must not
// be destroyed by callers; they are released in Clear() at mod unload.
// Icon references understood here:
//   "@file"       - the selected item's type icon
//   "@folder"     - the stock folder icon
//   "@glyph:XXXX" - an icon-font glyph rendered to a bitmap
//   anything else - a shell icon reference for ParseIconRef
const GUID kIidIImageList = {
    0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

class IconCache {
public:
    ~IconCache() { Clear(); }

    // Returns a menu bitmap for the item's captured icon, or nullptr. The
    // bitmap is composited over the menu background so classic (alpha-less)
    // menu drawing shows it correctly.
    HBITMAP GetBitmap(const MenuItem& item, int sizePx) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return GetBitmapFor(item.iconRef, item.iconPixels, sizePx);
    }

    // Custom-renderer entry point: no MenuItem required.
    HBITMAP GetBitmapFor(const std::wstring& iconRef,
                         const std::vector<uint8_t>& iconPixels, int sizePx) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (!iconPixels.empty()) {
            return BitmapFromPixels(iconPixels, sizePx);
        }
        if (iconRef.empty()) {
            return nullptr;
        }
        return BitmapFromRef(iconRef, sizePx);
    }

    // Custom-renderer entry point: true-alpha bitmap (no compositing).
    HBITMAP GetAlphaBitmapFor(const std::wstring& iconRef,
                              const std::vector<uint8_t>& iconPixels, int sizePx) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (!iconPixels.empty()) {
            const std::wstring key =
                L"a#px#" + std::to_wstring(HashBytes(iconPixels)) + L"#" +
                std::to_wstring(sizePx);
            auto it = bitmaps_.find(key);
            if (it != bitmaps_.end()) {
                return it->second;
            }
            const int side = static_cast<int>(
                std::sqrt(static_cast<double>(iconPixels.size()) / 4.0));
            HBITMAP bitmap = side > 0 &&
                                     static_cast<size_t>(side) * side * 4 ==
                                         iconPixels.size()
                                 ? AlphaBitmapFromBgra(iconPixels, side, side,
                                                       sizePx)
                                 : nullptr;
            bitmaps_[key] = bitmap;
            return bitmap;
        }
        if (iconRef.empty()) {
            return nullptr;
        }
        const std::wstring key = L"a#" + iconRef + L"#" + std::to_wstring(sizePx);
        auto it = bitmaps_.find(key);
        if (it != bitmaps_.end()) {
            return it->second;
        }
        HBITMAP bitmap = nullptr;
        if (iconRef.rfind(L"@stock:", 0) == 0) {
            int id = 0;
            bool shellStock = false;
            if (ResolveStockIcon(iconRef.substr(7), id, shellStock)) {
                if (shellStock) {
                    SHSTOCKICONINFO info = {};
                    info.cbSize = sizeof(info);
                    if (SUCCEEDED(SHGetStockIconInfo(
                            static_cast<SHSTOCKICONID>(id),
                            SHGSI_ICON | SHGSI_LARGEICON, &info)) &&
                        info.hIcon) {
                        bitmap = AlphaBitmapFromIcon(info.hIcon, sizePx);
                        DestroyIcon(info.hIcon);
                    }
                } else {
                    HICON icon = LoadIconW(nullptr, MAKEINTRESOURCEW(id));
                    if (icon) {
                        bitmap = AlphaBitmapFromIcon(icon, sizePx);
                    }
                }
            }
        } else if (iconRef.rfind(L"@ext:", 0) == 0) {
            const std::wstring spec = iconRef.substr(5);
            const bool isFolder = _wcsicmp(spec.c_str(), L"folder") == 0;
            SHFILEINFOW info = {};
            if (SHGetFileInfoW(isFolder ? L"folder" : spec.c_str(),
                               isFolder ? FILE_ATTRIBUTE_DIRECTORY
                                        : FILE_ATTRIBUTE_NORMAL,
                               &info, sizeof(info),
                               SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX)) {
                IImageList* list = nullptr;
                if (SUCCEEDED(SHGetImageList(SHIL_LARGE, kIidIImageList,
                                             reinterpret_cast<void**>(&list))) &&
                    list) {
                    HICON icon = nullptr;
                    if (SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT,
                                                &icon)) &&
                        icon) {
                        bitmap = AlphaBitmapFromIcon(icon, sizePx);
                        DestroyIcon(icon);
                    }
                    list->Release();
                }
            }
        } else {
            HICON icon = GetIcon(iconRef, sizePx);
            if (icon) {
                bitmap = AlphaBitmapFromIcon(icon, sizePx);
            }
        }
        bitmaps_[key] = bitmap;
        return bitmap;
    }

    // The owner window whose theme determines the menu background color.
    void SetThemeOwner(HWND owner) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        themeOwner_ = owner;
    }

    // Pre-renders the core action glyphs, so the open path only attaches
    // already-cached bitmaps.
    void PreloadCoreIcons(int sizePx) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        static const wchar_t* kGlyphRefs[] = {
            L"@glyph:E8C6", L"@glyph:E8C8", L"@glyph:E8AC", L"@glyph:E74D",
            L"@glyph:E713", L"@glyph:E8E5", L"@glyph:E72C", L"@glyph:E77F",
        };
        for (const wchar_t* ref : kGlyphRefs) {
            BitmapFromRef(ref, sizePx);
        }
    }

    void Clear() {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (auto& pair : bitmaps_) {
            if (pair.second) {
                DeleteObject(pair.second);
            }
        }
        bitmaps_.clear();
        for (auto& pair : icons_) {
            if (pair.second) {
                DestroyIcon(pair.second);
            }
        }
        icons_.clear();
    }

private:
    mutable std::recursive_mutex mutex_;

    HBITMAP BitmapFromRef(const std::wstring& ref, int sizePx) {
        const std::wstring key = ref + L"#" + std::to_wstring(sizePx);
        auto it = bitmaps_.find(key);
        if (it != bitmaps_.end()) {
            return it->second;
        }

        HBITMAP bitmap = nullptr;
        if (ref.rfind(L"@ext:", 0) == 0) {
            const std::wstring spec = ref.substr(5);
            const bool isFolder = _wcsicmp(spec.c_str(), L"folder") == 0;
            SHFILEINFOW info = {};
            if (SHGetFileInfoW(isFolder ? L"folder" : spec.c_str(),
                               isFolder ? FILE_ATTRIBUTE_DIRECTORY
                                        : FILE_ATTRIBUTE_NORMAL,
                               &info, sizeof(info),
                               SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON) &&
                info.hIcon) {
                bitmap = BitmapFromIcon(info.hIcon, sizePx);
                DestroyIcon(info.hIcon);
            }
        } else if (!IconRefGlyph(ref).empty()) {
            const std::wstring glyph = IconRefGlyph(ref);
            const wchar_t codepoint =
                static_cast<wchar_t>(wcstoul(glyph.c_str(), nullptr, 16));
            std::vector<uint8_t> pixels;
            if (GlyphPixels(codepoint, sizePx, pixels)) {
                bitmap = OpaqueBitmapFromBgra(pixels, sizePx, sizePx, sizePx);
            }
        } else if (ref.rfind(L"@stock:", 0) == 0) {
            int id = 0;
            bool shellStock = false;
            if (ResolveStockIcon(ref.substr(7), id, shellStock)) {
                HICON icon = nullptr;
                if (shellStock) {
                    SHSTOCKICONINFO info = {};
                    info.cbSize = sizeof(info);
                    if (SUCCEEDED(SHGetStockIconInfo(
                            static_cast<SHSTOCKICONID>(id),
                            SHGSI_ICON | SHGSI_SMALLICON, &info))) {
                        icon = info.hIcon;
                    }
                } else {
                    icon = LoadIconW(nullptr, MAKEINTRESOURCEW(id));
                }
                if (icon) {
                    bitmap = BitmapFromIcon(icon, sizePx);
                    if (shellStock) {
                        DestroyIcon(icon);
                    }
                }
            }
        } else {
            HICON icon = GetIcon(ref, sizePx);
            if (icon) {
                bitmap = BitmapFromIcon(icon, sizePx);
            }
        }
        bitmaps_[key] = bitmap;
        return bitmap;
    }

    HBITMAP BitmapFromPixels(const std::vector<uint8_t>& pixels, int sizePx) {
        if (pixels.size() < 4) {
            return nullptr;
        }
        const int side =
            static_cast<int>(std::sqrt(static_cast<double>(pixels.size()) / 4.0));
        if (side <= 0 ||
            static_cast<size_t>(side) * side * 4 != pixels.size()) {
            return nullptr;
        }

        const std::wstring key = L"px#" + std::to_wstring(HashBytes(pixels)) + L"#" +
                                 std::to_wstring(sizePx);
        auto it = bitmaps_.find(key);
        if (it != bitmaps_.end()) {
            return it->second;
        }
        HBITMAP bitmap = OpaqueBitmapFromBgra(pixels, side, side, sizePx);
        bitmaps_[key] = bitmap;
        return bitmap;
    }

    HICON GetIcon(std::wstring_view ref, int sizePx) {
        const std::wstring key = std::wstring(ref) + L"#" + std::to_wstring(sizePx);
        auto it = icons_.find(key);
        if (it != icons_.end()) {
            return it->second;
        }

        HICON icon = nullptr;
        std::wstring path;
        int index = 0;
        if (ParseIconRef(ref, path, index)) {
            wchar_t expanded[MAX_PATH] = {};
            if (ExpandEnvironmentStringsW(path.c_str(), expanded,
                                          ARRAYSIZE(expanded))) {
                HICON large = nullptr;
                if (SUCCEEDED(SHDefExtractIconW(expanded, index, 0, &large, nullptr,
                                                MAKELONG(sizePx, sizePx)))) {
                    icon = large;
                }
            }
        }
        icons_[key] = icon;
        return icon;
    }

    static uint64_t HashBytes(const std::vector<uint8_t>& bytes) {
        uint64_t hash = 1469598103934665603ULL;
        for (uint8_t byte : bytes) {
            hash ^= byte;
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    static void FillBitmapHeader(BITMAPV5HEADER& header, int width, int height) {
        header = {};
        header.bV5Size = sizeof(header);
        header.bV5Width = width;
        header.bV5Height = -height;  // top-down
        header.bV5Planes = 1;
        header.bV5BitCount = 32;
        header.bV5Compression = BI_BITFIELDS;
        header.bV5RedMask = 0x00FF0000;
        header.bV5GreenMask = 0x0000FF00;
        header.bV5BlueMask = 0x000000FF;
        header.bV5AlphaMask = 0xFF000000;
    }

    HBITMAP BitmapFromIcon(HICON icon, int sizePx) {
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        BITMAPV5HEADER header = {};
        FillBitmapHeader(header, sizePx, sizePx);
        void* bits = nullptr;
        HBITMAP bitmap = CreateDIBSection(
            screen, reinterpret_cast<BITMAPINFO*>(&header), DIB_RGB_COLORS, &bits,
            nullptr, 0);
        if (bitmap && bits) {
            HGDIOBJ old = SelectObject(memory, bitmap);
            // Fill with the menu background first: menus draw bitmaps without
            // alpha, so the icon must be composited here.
            RECT rect = {0, 0, sizePx, sizePx};
            HBRUSH backgroundBrush = CreateSolidBrush(MenuBackgroundColor(themeOwner_));
            FillRect(memory, &rect, backgroundBrush);
            DeleteObject(backgroundBrush);
            DrawIconEx(memory, 0, 0, icon, sizePx, sizePx, 0, nullptr, DI_NORMAL);
            SelectObject(memory, old);
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return bitmap;
    }

    // Renders an icon onto a transparent 32bpp DIB for the custom renderer.
    HBITMAP AlphaBitmapFromIcon(HICON icon, int sizePx) {
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        BITMAPV5HEADER header = {};
        FillBitmapHeader(header, sizePx, sizePx);
        void* bits = nullptr;
        HBITMAP bitmap = CreateDIBSection(
            screen, reinterpret_cast<BITMAPINFO*>(&header), DIB_RGB_COLORS, &bits,
            nullptr, 0);
        if (bitmap && bits) {
            memset(bits, 0, static_cast<size_t>(sizePx) * sizePx * 4);
            HGDIOBJ old = SelectObject(memory, bitmap);
            DrawIconEx(memory, 0, 0, icon, sizePx, sizePx, 0, nullptr, DI_NORMAL);
            SelectObject(memory, old);

            auto* pixels = static_cast<uint32_t*>(bits);
            const size_t count = static_cast<size_t>(sizePx) * sizePx;
            bool hasAlpha = false;
            for (size_t i = 0; i < count; ++i) {
                if ((pixels[i] >> 24) != 0) {
                    hasAlpha = true;
                    break;
                }
            }
            if (!hasAlpha) {
                // Legacy icons carry no alpha; the AND mask left RGB 0 outside
                // the shape, so opaque pixels form the silhouette.
                for (size_t i = 0; i < count; ++i) {
                    if ((pixels[i] & 0x00FFFFFF) != 0) {
                        pixels[i] |= 0xFF000000u;
                    }
                }
            } else {
                // AlphaBlend wrote premultiplied color; keep it consistent.
                for (size_t i = 0; i < count; ++i) {
                    const uint32_t alpha = pixels[i] >> 24;
                    if (alpha == 0 || alpha == 255) {
                        continue;
                    }
                    const uint32_t r = ((pixels[i] >> 16) & 0xFF) * alpha / 255;
                    const uint32_t g = ((pixels[i] >> 8) & 0xFF) * alpha / 255;
                    const uint32_t b = (pixels[i] & 0xFF) * alpha / 255;
                    pixels[i] = (alpha << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return bitmap;
    }

    // Straight-alpha BGRA pixels to a premultiplied bitmap, no background.
    HBITMAP AlphaBitmapFromBgra(const std::vector<uint8_t>& pixels, int width,
                                int height, int sizePx) {
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        BITMAPV5HEADER header = {};
        FillBitmapHeader(header, sizePx, sizePx);
        void* bits = nullptr;
        HBITMAP bitmap = CreateDIBSection(
            screen, reinterpret_cast<BITMAPINFO*>(&header), DIB_RGB_COLORS, &bits,
            nullptr, 0);
        if (bitmap && bits) {
            auto* target = static_cast<uint32_t*>(bits);
            const size_t count = static_cast<size_t>(sizePx) * sizePx;
            for (size_t i = 0; i < count; ++i) {
                target[i] = 0;
            }
            for (int y = 0; y < height && y < sizePx; ++y) {
                for (int x = 0; x < width && x < sizePx; ++x) {
                    const uint32_t pixel = reinterpret_cast<const uint32_t*>(
                        pixels.data())[static_cast<size_t>(y) * width + x];
                    const uint32_t alpha = pixel >> 24;
                    const uint32_t r = ((pixel >> 16) & 0xFF) * alpha / 255;
                    const uint32_t g = ((pixel >> 8) & 0xFF) * alpha / 255;
                    const uint32_t b = (pixel & 0xFF) * alpha / 255;
                    target[static_cast<size_t>(y) * sizePx + x] =
                        (alpha << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return bitmap;
    }

    // Composites captured BGRA pixels over the actual themed menu background
    // into an opaque 32bpp bitmap. Classic menus draw MIM_BITMAP bitmaps
    // without alpha blending, so transparency must be baked in here.
    HBITMAP OpaqueBitmapFromBgra(const std::vector<uint8_t>& pixels, int width,
                                 int height, int sizePx) {
        HDC screen = GetDC(nullptr);
        HDC sourceDc = CreateCompatibleDC(screen);
        HDC targetDc = CreateCompatibleDC(screen);

        BITMAPV5HEADER sourceHeader = {};
        FillBitmapHeader(sourceHeader, width, height);
        void* sourceBits = nullptr;
        HBITMAP source = CreateDIBSection(
            screen, reinterpret_cast<BITMAPINFO*>(&sourceHeader), DIB_RGB_COLORS,
            &sourceBits, nullptr, 0);
        if (source && sourceBits) {
            memcpy(sourceBits, pixels.data(), pixels.size());
        }

        BITMAPV5HEADER targetHeader = {};
        FillBitmapHeader(targetHeader, sizePx, sizePx);
        void* targetBits = nullptr;
        HBITMAP target = CreateDIBSection(
            screen, reinterpret_cast<BITMAPINFO*>(&targetHeader), DIB_RGB_COLORS,
            &targetBits, nullptr, 0);

        if (source && target && targetBits) {
            HGDIOBJ oldSource = SelectObject(sourceDc, source);
            HGDIOBJ oldTarget = SelectObject(targetDc, target);

            RECT rect = {0, 0, sizePx, sizePx};
            HBRUSH backgroundBrush = CreateSolidBrush(MenuBackgroundColor(themeOwner_));
            FillRect(targetDc, &rect, backgroundBrush);
            DeleteObject(backgroundBrush);

            if (width == sizePx && height == sizePx) {
                // Composite in software: menus draw bitmaps without alpha, so
                // transparency has to be resolved here. Zero-alpha pixels keep
                // the background; when the bitmap has no alpha channel at all,
                // black is treated as the classic color key.
                bool allZeroAlpha = true;
                for (size_t i = 3; i < pixels.size(); i += 4) {
                    if (pixels[i] != 0) {
                        allZeroAlpha = false;
                        break;
                    }
                }

                uint8_t* target = static_cast<uint8_t*>(targetBits);
                for (int i = 0; i < sizePx * sizePx; ++i) {
                    const uint8_t blue = pixels[i * 4];
                    const uint8_t green = pixels[i * 4 + 1];
                    const uint8_t red = pixels[i * 4 + 2];
                    const uint8_t alpha = pixels[i * 4 + 3];

                    if (allZeroAlpha) {
                        if (blue == 0 && green == 0 && red == 0) {
                            continue;  // color key
                        }
                        target[i * 4] = blue;
                        target[i * 4 + 1] = green;
                        target[i * 4 + 2] = red;
                        continue;
                    }
                    if (alpha == 0) {
                        continue;
                    }
                    if (alpha == 255) {
                        target[i * 4] = blue;
                        target[i * 4 + 1] = green;
                        target[i * 4 + 2] = red;
                        continue;
                    }
                    const uint32_t inverse = 255 - alpha;
                    target[i * 4] = static_cast<uint8_t>(
                        (blue * alpha + target[i * 4] * inverse) / 255);
                    target[i * 4 + 1] = static_cast<uint8_t>(
                        (green * alpha + target[i * 4 + 1] * inverse) / 255);
                    target[i * 4 + 2] = static_cast<uint8_t>(
                        (red * alpha + target[i * 4 + 2] * inverse) / 255);
                }
            } else {
                bool hasAlpha = false;
                for (size_t i = 3; i < pixels.size(); i += 4) {
                    if (pixels[i] != 0 && pixels[i] != 255) {
                        hasAlpha = true;
                        break;
                    }
                }

                if (hasAlpha) {
                    // Premultiply for GdiAlphaBlend with AC_SRC_ALPHA.
                    std::vector<uint8_t> premultiplied = pixels;
                    for (size_t i = 0; i < premultiplied.size(); i += 4) {
                        const uint32_t alpha = premultiplied[i + 3];
                        premultiplied[i] = static_cast<uint8_t>(
                            premultiplied[i] * alpha / 255);
                        premultiplied[i + 1] = static_cast<uint8_t>(
                            premultiplied[i + 1] * alpha / 255);
                        premultiplied[i + 2] = static_cast<uint8_t>(
                            premultiplied[i + 2] * alpha / 255);
                    }
                    memcpy(sourceBits, premultiplied.data(), premultiplied.size());

                    BLENDFUNCTION blend = {};
                    blend.BlendOp = AC_SRC_OVER;
                    blend.SourceConstantAlpha = 255;
                    blend.AlphaFormat = AC_SRC_ALPHA;
                    GdiAlphaBlend(targetDc, 0, 0, sizePx, sizePx, sourceDc, 0, 0,
                                  width, height, blend);
                } else {
                    SetStretchBltMode(targetDc, HALFTONE);
                    StretchBlt(targetDc, 0, 0, sizePx, sizePx, sourceDc, 0, 0, width,
                               height, SRCCOPY);
                }
            }

            SelectObject(targetDc, oldTarget);
            SelectObject(sourceDc, oldSource);
        } else if (target) {
            DeleteObject(target);
            target = nullptr;
        }

        if (source) {
            DeleteObject(source);
        }
        DeleteDC(sourceDc);
        DeleteDC(targetDc);
        ReleaseDC(nullptr, screen);
        return target;
    }

    static bool FontExists(const wchar_t* faceName) {
        HDC dc = GetDC(nullptr);
        LOGFONTW logFont = {};
        logFont.lfCharSet = DEFAULT_CHARSET;
        wcsncpy(logFont.lfFaceName, faceName, LF_FACESIZE - 1);
        bool found = false;
        EnumFontFamiliesExW(
            dc, &logFont,
            [](const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM lParam) -> int {
                *reinterpret_cast<bool*>(lParam) = true;
                return 0;
            },
            reinterpret_cast<LPARAM>(&found), 0);
        ReleaseDC(nullptr, dc);
        return found;
    }

    // Renders an icon-font glyph into straight BGRA pixels (alpha =
    // coverage) in the menu text color. The caller composites it over the
    // menu background.
    static bool GlyphPixels(wchar_t codepoint, int sizePx,
                            std::vector<uint8_t>& out) {
        static const wchar_t* kFonts[] = {L"Segoe Fluent Icons",
                                          L"Segoe MDL2 Assets"};
        for (const wchar_t* font : kFonts) {
            if (!FontExists(font)) {
                continue;
            }

            HDC screen = GetDC(nullptr);
            HDC memory = CreateCompatibleDC(screen);
            BITMAPV5HEADER header = {};
            FillBitmapHeader(header, sizePx, sizePx);
            void* bits = nullptr;
            HBITMAP bitmap = CreateDIBSection(
                screen, reinterpret_cast<BITMAPINFO*>(&header), DIB_RGB_COLORS,
                &bits, nullptr, 0);
            if (!bitmap || !bits) {
                if (bitmap) {
                    DeleteObject(bitmap);
                }
                DeleteDC(memory);
                ReleaseDC(nullptr, screen);
                continue;
            }

            memset(bits, 0, static_cast<size_t>(sizePx) * sizePx * 4);
            HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
            HFONT fontHandle = CreateFontW(
                -sizePx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, font);
            HGDIOBJ oldFont = SelectObject(memory, fontHandle);
            SetBkMode(memory, TRANSPARENT);
            SetTextColor(memory, RGB(255, 255, 255));
            RECT rect = {0, 0, sizePx, sizePx};
            DrawTextW(memory, &codepoint, 1, &rect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(memory, oldFont);
            DeleteObject(fontHandle);
            SelectObject(memory, oldBitmap);
            DeleteDC(memory);
            ReleaseDC(nullptr, screen);

            const bool darkTheme = IsDarkThemeActive();
            const COLORREF textColor = GetSysColor(COLOR_MENUTEXT);
            const uint8_t red = darkTheme ? 255 : GetRValue(textColor);
            const uint8_t green = darkTheme ? 255 : GetGValue(textColor);
            const uint8_t blue = darkTheme ? 255 : GetBValue(textColor);

            const uint8_t* pixels = static_cast<const uint8_t*>(bits);
            out.assign(static_cast<size_t>(sizePx) * sizePx * 4, 0);
            bool anyCoverage = false;
            for (int i = 0; i < sizePx * sizePx; ++i) {
                uint8_t coverage = pixels[i * 4];
                if (pixels[i * 4 + 1] > coverage) {
                    coverage = pixels[i * 4 + 1];
                }
                if (pixels[i * 4 + 2] > coverage) {
                    coverage = pixels[i * 4 + 2];
                }
                if (coverage > 0) {
                    anyCoverage = true;
                }
                out[i * 4] = blue;
                out[i * 4 + 1] = green;
                out[i * 4 + 2] = red;
                out[i * 4 + 3] = coverage;
            }
            DeleteObject(bitmap);
            if (anyCoverage) {
                return true;
            }
        }
        return false;
    }

    HWND themeOwner_ = nullptr;
    std::unordered_map<std::wstring, HBITMAP> bitmaps_;
    std::unordered_map<std::wstring, HICON> icons_;
};

inline std::optional<IconCache> g_iconCache{std::in_place};

HBITMAP GetIconBitmapForMenu(const std::wstring& iconRef,
                             const std::vector<uint8_t>& iconPixels, int sizePx) {
    return g_iconCache->GetBitmapFor(iconRef, iconPixels, sizePx);
}

HBITMAP GetAlphaIconBitmapForMenu(const std::wstring& iconRef,
                                  const std::vector<uint8_t>& iconPixels,
                                  int sizePx) {
    return g_iconCache->GetAlphaBitmapFor(iconRef, iconPixels, sizePx);
}

class NativeMenuView {
public:
    static std::optional<uint32_t> Show(const MenuModel& model, HWND owner, POINT pt,
                                        bool* creationFailed = nullptr) {
        if (creationFailed) {
            *creationFailed = false;
        }

        HMENU menu = CreatePopupMenu();
        if (!menu) {
            if (creationFailed) {
                *creationFailed = true;
            }
            return std::nullopt;
        }
        const int iconSize = GetSystemMetrics(SM_CXSMICON);
        g_iconCache->SetThemeOwner(owner);
        AppendItems(menu, model.items, iconSize);

        if (!TrackPopupMenuEx_Original) {
            if (creationFailed) {
                *creationFailed = true;
            }
            DestroyMenu(menu);
            return std::nullopt;
        }

        const UINT flags = TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN |
                           TPM_TOPALIGN |
                           (g_settings->instantMenuFade ? TPM_NOANIMATION : 0);
        MenuDelaySuppressor delaySuppressor;
        int command = TrackPopupMenuEx_Original(menu, flags, pt.x, pt.y, owner, nullptr);
        DestroyMenu(menu);

        if (command == 0) {
            return std::nullopt;
        }
        return static_cast<uint32_t>(command);
    }

private:
    static void AppendItems(HMENU menu, const std::vector<MenuItem>& items,
                            int iconSize) {
        int index = 0;
        for (const MenuItem& item : items) {
            if (item.kind == ItemKind::Separator) {
                AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
                ++index;
                continue;
            }

            UINT flags = MF_STRING;
            if (item.flags & kModelDisabled) {
                flags |= MF_GRAYED;
            }
            if (item.flags & kModelChecked) {
                flags |= MF_CHECKED;
            }
            if (item.flags & kModelDefault) {
                flags |= MF_DEFAULT;
            }

            if (item.kind == ItemKind::Submenu) {
                HMENU submenu = CreatePopupMenu();
                if (!submenu) {
                    ++index;
                    continue;
                }
                AppendItems(submenu, item.children, iconSize);
                AppendMenuW(menu, flags | MF_POPUP,
                            reinterpret_cast<UINT_PTR>(submenu), item.label.c_str());
            } else {
                AppendMenuW(menu, flags, item.id, item.label.c_str());
            }

            // Checked items keep the checkmark gutter; everything else may
            // carry a cached icon bitmap.
            if (!(item.flags & kModelChecked)) {
                HBITMAP bitmap = g_iconCache->GetBitmap(item, iconSize);
                if (bitmap) {
                    MENUITEMINFOW bitmapInfo = {};
                    bitmapInfo.cbSize = sizeof(bitmapInfo);
                    bitmapInfo.fMask = MIIM_BITMAP;
                    bitmapInfo.hbmpItem = bitmap;
                    SetMenuItemInfoW(menu, static_cast<UINT>(index), TRUE, &bitmapInfo);
                }
            }
            ++index;
        }
    }
};

// Subclasses the menu owner while a menu is displayed. Optionally forwards
// menu messages to the shell context object (required for dynamic submenus
// and owner-draw items on the native menu), and optionally warms the cache
// after a short delay while the menu is still open.
constexpr UINT_PTR kOwnerSubclassId = 0xC0DE;

// Windows the native replay has subclassed. UI-thread only; the unload
// cleanup removes them so a subclass proc can never outlive the DLL.
inline thread_local std::vector<HWND> g_subclassOwners;

class OwnerSubclass {
public:
    OwnerSubclass(HWND owner, PendingCapture* capture, bool forwardMenuMessages)
        : owner_(owner), capture_(capture), forward_(forwardMenuMessages) {
        if (SetWindowSubclass(owner, &OwnerSubclass::Proc, kOwnerSubclassId,
                              reinterpret_cast<DWORD_PTR>(this))) {
            subclassed_ = true;
            g_subclassOwners.push_back(owner);
        }
    }

    ~OwnerSubclass() {
        if (subclassed_) {
            RemoveWindowSubclass(owner_, &OwnerSubclass::Proc, kOwnerSubclassId);
            for (size_t i = 0; i < g_subclassOwners.size(); ++i) {
                if (g_subclassOwners[i] == owner_) {
                    g_subclassOwners.erase(g_subclassOwners.begin() + i);
                    break;
                }
            }
        }
    }

    // Called from the UI-thread unload cleanup: removes every subclass this
    // DLL installed, so comctl32 can never call into unloaded code.
    static void RemoveAllInstalled() {
        for (HWND hwnd : g_subclassOwners) {
            if (IsWindow(hwnd)) {
                RemoveWindowSubclass(hwnd, &OwnerSubclass::Proc, kOwnerSubclassId);
            }
        }
        g_subclassOwners.clear();
    }

private:
    static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                 UINT_PTR idSubclass, DWORD_PTR refData) {
        auto* self = reinterpret_cast<OwnerSubclass*>(refData);
        return self->Handle(hwnd, msg, wParam, lParam, idSubclass);
    }

    LRESULT Handle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                   UINT_PTR idSubclass) {
        if (forward_ && capture_) {
            switch (msg) {
                case WM_INITMENUPOPUP:
                case WM_DRAWITEM:
                case WM_MEASUREITEM:
                case WM_MENUCHAR: {
                    LRESULT result = 0;
                    bool handled = false;
                    if (capture_->contextMenu3) {
                        handled = SUCCEEDED(capture_->contextMenu3->HandleMenuMsg2(
                            msg, wParam, lParam, &result));
                    } else if (capture_->contextMenu2) {
                        handled = SUCCEEDED(capture_->contextMenu2->HandleMenuMsg(
                            msg, wParam, lParam));
                    }
                    if (msg == WM_MENUCHAR && handled) {
                        return result;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    HWND owner_ = nullptr;
    PendingCapture* capture_ = nullptr;
    bool forward_ = false;
    bool subclassed_ = false;
};

// Free-function bridge so the early unload cleanup can remove subclasses
// without knowing about OwnerSubclass.
void RemoveOwnerSubclasses() {
    OwnerSubclass::RemoveAllInstalled();
}

// Shows the retained, really-populated native menu with menu-message
// forwarding and invokes the selection through the live object. Used by the
// fallback item and, as a safety net, the late-Shift bypass.
std::optional<uint32_t> ShowNativeReplay(PendingCapture& capture, HWND owner, POINT pt) {
    if (!EnsureContextPopulated(capture) || !capture.populatedMenu ||
        !TrackPopupMenuEx_Original) {
        return std::nullopt;
    }

    // Submenus are populated on WM_INITMENUPOPUP; initialize the retained menu
    // before showing it or placeholders (such as the New submenu's) would be
    // displayed and their invocation would fail.
    if (!capture.menuInitialized) {
        InitializeMenuRecursive(capture, capture.populatedMenu, 0);
        capture.menuInitialized = true;
    }
    DumpMenuTree(capture.populatedMenu, capture.idCmdFirst, 0);

    OwnerSubclass subclass(owner, &capture, /*forwardMenuMessages=*/true);

    const UINT flags = TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN |
                       TPM_TOPALIGN |
                       (g_settings->instantMenuFade ? TPM_NOANIMATION : 0);
    int command = TrackPopupMenuEx_Original(capture.populatedMenu, flags, pt.x, pt.y, owner,
                                            nullptr);
    if (command == 0 || !capture.obj) {
        return std::nullopt;
    }

    CMINVOKECOMMANDINFOEX info = {};
    info.cbSize = sizeof(info);
    info.fMask = CMIC_MASK_UNICODE;
    info.hwnd = owner;
    const UINT offset = static_cast<UINT>(command) - capture.idCmdFirst;
    info.lpVerb = MAKEINTRESOURCEA(offset);
    info.lpVerbW = MAKEINTRESOURCEW(offset);
    info.nShow = SW_SHOWNORMAL;
    if (FAILED(capture.obj->InvokeCommand(
            reinterpret_cast<CMINVOKECOMMANDINFO*>(&info)))) {
        Wh_Log(L"Native menu invocation failed for offset %u", offset);
    }

    return static_cast<uint32_t>(command);
}

// Open-path timing, active only with debugLogging.
class Perf {
public:
    void MarkOpenPathStart() {
        if (g_settings->debugLogging) {
            startTick_ = GetTickCount64();
        }
    }

    uint64_t OpenPathElapsedMs() {
        if (!g_settings->debugLogging || startTick_ == 0) {
            return 0;
        }
        return GetTickCount64() - startTick_;
    }

private:
    uint64_t startTick_ = 0;
};

inline Perf g_perf;

}  // namespace cmo

// ===========================================================================
// [CMO:Warmup] Background cache warm-up.
// ===========================================================================
namespace cmo {

std::vector<std::wstring> BuildWarmupTypes(
    std::span<const std::wstring> configuredExtensions) {
    std::vector<std::wstring> types;

    auto addUnique = [&](std::wstring value) {
        if (std::find(types.begin(), types.end(), value) == types.end()) {
            types.push_back(std::move(value));
        }
    };

    addUnique(L"*");
    addUnique(L"Directory");
    addUnique(L"Directory\\Background");
    addUnique(L"Desktop");
    addUnique(L"Drive");

    for (const std::wstring& raw : configuredExtensions) {
        const size_t begin = raw.find_first_not_of(L" \t");
        if (begin == std::wstring::npos) {
            continue;
        }
        const size_t end = raw.find_last_not_of(L" \t");
        std::wstring extension = raw.substr(begin, end - begin + 1);
        if (extension.empty()) {
            continue;
        }
        if (extension[0] != L'.') {
            extension.insert(extension.begin(), L'.');
        }
        CharLowerBuffW(extension.data(), static_cast<DWORD>(extension.size()));
        addUnique(std::move(extension));
    }
    return types;
}

// Creates a shell context menu object for a path on the warm-up thread.
IContextMenu* CreateContextMenuForPath(const std::wstring& path, bool background) {
    PIDLIST_ABSOLUTE absolute = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &absolute, 0, nullptr)) ||
        !absolute) {
        return nullptr;
    }

    IContextMenu* menu = nullptr;
    if (background) {
        DEFCONTEXTMENU dcm = {};
        dcm.pidlFolder = absolute;
        dcm.cidl = 0;
        SHCreateDefaultContextMenu(&dcm, IID_IContextMenu, (void**)&menu);
    } else {
        IShellFolder* parent = nullptr;
        PCUITEMID_CHILD child = ILFindLastID(absolute);
        if (SUCCEEDED(SHBindToParent(absolute, IID_IShellFolder, (void**)&parent, &child)) &&
            parent) {
            parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu, nullptr,
                                  (void**)&menu);
            parent->Release();
        }
    }

    CoTaskMemFree(absolute);
    return menu;
}

const wchar_t* WarmupMutexName() {
    return L"Local\\ContextMenuOverhaulWarmup";
}

// Small per-process jitter so a cold start with many Explorer processes does
// not have them all populate at the same instant.
int WarmupJitterMs(uint32_t seed) {
    return static_cast<int>(seed % 3001);
}

class Warmup {
public:
    void Start() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (thread_ || g_unloading.load()) {
            return;
        }
        {
            std::lock_guard<std::mutex> eventsLock(eventsMutex_);
            stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            resumeEvent_ = CreateEventW(nullptr, TRUE, TRUE, nullptr);
            if (!stopEvent_ || !resumeEvent_) {
                if (stopEvent_) CloseHandle(stopEvent_);
                if (resumeEvent_) CloseHandle(resumeEvent_);
                stopEvent_ = nullptr;
                resumeEvent_ = nullptr;
                return;
            }
        }

        thread_ = CreateThread(nullptr, 0, &Warmup::ThreadProc, this, 0, nullptr);
        if (thread_) {
            SetThreadPriority(thread_, THREAD_PRIORITY_BELOW_NORMAL);
        } else {
            std::lock_guard<std::mutex> eventsLock(eventsMutex_);
            CloseHandle(stopEvent_);
            CloseHandle(resumeEvent_);
            stopEvent_ = nullptr;
            resumeEvent_ = nullptr;
        }
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        {
            std::lock_guard<std::mutex> eventsLock(eventsMutex_);
            if (stopEvent_) {
                SetEvent(stopEvent_);
            }
            // Wake a paused worker so it can observe the stop event.
            if (resumeEvent_) {
                SetEvent(resumeEvent_);
            }
            paused_.store(false);
        }

        // The worker must be joined before the module can unload; there is no
        // safe give-up path while its code is still on the stack. The events
        // lock is not held here, so a menu opening on a UI thread can still
        // pause the worker while this join runs.
        if (thread_) {
            WaitForSingleObject(thread_, INFINITE);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
        std::lock_guard<std::mutex> eventsLock(eventsMutex_);
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
        if (resumeEvent_) {
            CloseHandle(resumeEvent_);
            resumeEvent_ = nullptr;
        }
    }

    // Pauses warm-up between contexts while a menu is open.
    void SetMenuOpen(bool open) {
        // Only the small events lock: opening a menu must not block behind a
        // Stop() that is joining the worker.
        std::lock_guard<std::mutex> lock(eventsMutex_);
        paused_.store(open);
        if (resumeEvent_) {
            if (open) {
                ResetEvent(resumeEvent_);
            } else {
                SetEvent(resumeEvent_);
            }
        }
    }

#ifdef CMO_TESTING
    bool IsPaused() const;
#endif

private:
    static DWORD WINAPI ThreadProc(LPVOID param) {
        static_cast<Warmup*>(param)->Run();
        return 0;
    }

    void Run() {
        // The New submenu is built from these templates; prebuild them so the
        // first right-click does not pay for the registry walk.
        EnsureNewTemplates();

        const int delaySeconds =
            g_settings->warmupDelaySeconds > 0 ? g_settings->warmupDelaySeconds : 0;
        if (WaitForSingleObject(stopEvent_, static_cast<DWORD>(delaySeconds) * 1000) ==
            WAIT_OBJECT_0) {
            LogEarlyStop();
            return;
        }

        const uint32_t seed = static_cast<uint32_t>(GetTickCount64()) ^
                              (GetCurrentProcessId() * 2654435761u);
        const int jitter = WarmupJitterMs(seed);
        if (jitter > 0 &&
            WaitForSingleObject(stopEvent_, static_cast<DWORD>(jitter)) ==
                WAIT_OBJECT_0) {
            LogEarlyStop();
            return;
        }

        std::vector<std::wstring> configured;
        WindhawkUtils::StringSetting warmupList =
            WindhawkUtils::StringSetting::make(L"warmupExtensions");
        const std::wstring text = warmupList.get();
        if (!text.empty()) {
            size_t pos = 0;
            while (pos <= text.size()) {
                const size_t comma = text.find(L',', pos);
                const std::wstring item = TrimWhitespace(
                    comma == std::wstring::npos
                        ? text.substr(pos)
                        : text.substr(pos, comma - pos));
                if (!item.empty()) {
                    configured.push_back(item);
                }
                if (comma == std::wstring::npos) {
                    break;
                }
                pos = comma + 1;
            }
        }
        const std::vector<std::wstring> types = BuildWarmupTypes(configured);

        wchar_t storagePath[MAX_PATH] = {};
        if (!Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
            return;
        }
        const std::wstring warmupDir = std::wstring(storagePath) + L"\\warmup";
        CreateDirectoryW(storagePath, nullptr);
        CreateDirectoryW(warmupDir.c_str(), nullptr);

        // The shared cache may already hold everything another process warmed.
        if (g_cache.Size() == 0) {
            g_cache.Load(CacheFilePath());
        }

        HANDLE mutex = CreateMutexW(nullptr, FALSE, WarmupMutexName());
        const bool haveMutex =
            mutex && WaitForSingleObject(mutex, 10000) == WAIT_OBJECT_0;
        if (!haveMutex && g_cache.Size() == 0) {
            // Another process is warming; take whatever it has saved.
            g_cache.Load(CacheFilePath());
        }

        // Rebuild the SendTo entries off the UI thread (also after handler
        // registry invalidation).
        RebuildSendToChildren();

        // The worker never pumps messages, so it must use the MTA; an STA
        // without a message loop is exactly the thread that gets stuck in
        // third-party shell code.
        const bool comInitialized =
            SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

        int warmed = 0;
        int skipped = 0;
        bool completed = true;
        for (const std::wstring& type : types) {
            if (WaitForSingleObject(stopEvent_, 0) == WAIT_OBJECT_0) {
                completed = false;
                break;
            }
            // Wait for resume or stop: a menu opening concurrently may reset
            // the resume event, and the stop signal must still wake us.
            HANDLE waitEvents[2] = {stopEvent_, resumeEvent_};
            WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);
            if (WaitForSingleObject(stopEvent_, 0) == WAIT_OBJECT_0) {
                completed = false;
                break;
            }
            if (WarmOneType(type, warmupDir)) {
                ++warmed;
            } else {
                ++skipped;
            }
            Sleep(50);
        }

        if (comInitialized) {
            CoUninitialize();
        }
        if (completed && warmed > 0) {
            g_cache.Save(CacheFilePath());
        }
        if (g_settings->debugLogging) {
            if (completed) {
                Wh_Log(L"Warm-up finished (%d warmed, %d already cached)", warmed,
                       skipped);
            } else {
                Wh_Log(L"Warm-up stopped early (%d warmed)", warmed);
            }
        }
        if (haveMutex) {
            ReleaseMutex(mutex);
        }
        if (mutex) {
            CloseHandle(mutex);
        }
    }

    static void LogEarlyStop() {
        if (g_settings->debugLogging) {
            Wh_Log(L"Warm-up stopped early (0 warmed)");
        }
    }

    bool WarmOneType(const std::wstring& type, const std::wstring& warmupDir) {
        if (type == L"*") {
            const std::wstring path = warmupDir + L"\\warmup";
            EnsureScratchFile(path);
            return WarmPathIfMissing(
                path, false,
                ContextSignature{Scope::Files, L"*", Shape::Single, Variant::Normal});
        }
        if (type == L"Directory") {
            const std::wstring path = warmupDir + L"\\warmup-folder";
            CreateDirectoryW(path.c_str(), nullptr);
            return WarmPathIfMissing(
                path, false,
                ContextSignature{Scope::Folders, L"*", Shape::Single, Variant::Normal});
        }
        if (type == L"Directory\\Background") {
            const std::wstring path = warmupDir + L"\\warmup-folder";
            CreateDirectoryW(path.c_str(), nullptr);
            return WarmPathIfMissing(
                path, true, ContextSignature{Scope::Background, L"*", Shape::Single,
                                             Variant::Normal});
        }
        if (type == L"Drive") {
            wchar_t windowsDir[MAX_PATH] = {};
            if (GetWindowsDirectoryW(windowsDir, ARRAYSIZE(windowsDir))) {
                const std::wstring drive(windowsDir, 3);  // "C:\"
                return WarmPathIfMissing(
                    drive, false,
                    ContextSignature{Scope::Drive, L"*", Shape::Single,
                                     Variant::Normal});
            }
            return false;
        }
        if (type == L"Desktop") {
            wchar_t desktopPath[MAX_PATH] = {};
            if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOP, nullptr, 0,
                                           desktopPath))) {
                return WarmPathIfMissing(
                    desktopPath, false,
                    ContextSignature{Scope::Desktop, L"*", Shape::Single,
                                     Variant::Normal});
            }
            return false;
        }
        const std::wstring path = warmupDir + L"\\warmup" + type;
        EnsureScratchFile(path);
        return WarmPathIfMissing(
            path, false,
            ContextSignature{Scope::Files, type, Shape::Single, Variant::Normal});
    }

    bool WarmPathIfMissing(const std::wstring& path, bool background,
                           const ContextSignature& signature) {
        if (g_cache.Has(signature)) {
            return false;
        }
        WarmPath(path, background, signature);
        return true;
    }

    static void EnsureScratchFile(const std::wstring& path) {
        HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            CloseHandle(file);
        }
    }

    void WarmPath(const std::wstring& path, bool background,
                  const ContextSignature& signature) {
        IContextMenu* menu = CreateContextMenuForPath(path, background);
        if (!menu) {
            if (g_settings->debugLogging) {
                Wh_Log(L"Warm-up: no context menu for %s", path.c_str());
            }
            return;
        }

        const std::vector<std::wstring> modulesBefore = SnapshotLoadedModules();
        HMENU offscreen = CreatePopupMenu();
        if (offscreen) {
            ClearRecordedItemBitmaps();
            ReplayInto(menu, offscreen, 0, 1, 0x7FFF, CMF_NORMAL);
            MenuModel model = BuildModelFromHMenu(offscreen, 1, signature, menu);
            ClearRecordedItemBitmaps();
            DestroyMenu(offscreen);
            if (!model.items.empty()) {
                ApplyRegistryIcons(model.items, signature);
                model.handlerModules =
                    DiffModules(modulesBefore, SnapshotLoadedModules());
                model.sourceStamp = ComputeModuleStamp(model.handlerModules);
                model.flags |= kModelWarmup;
                g_cache.Put(std::move(model));
            }
        }
        menu->Release();
    }

    std::mutex lifecycleMutex_;
    std::mutex eventsMutex_;
    HANDLE thread_ = nullptr;
    HANDLE stopEvent_ = nullptr;
    HANDLE resumeEvent_ = nullptr;
    std::atomic<bool> paused_{false};
};

inline Warmup g_warmup;

}  // namespace cmo

// ===========================================================================
// [CMO:Invalidation] Cache invalidation on handler registration changes.
// ===========================================================================
namespace cmo {

struct SourceStamp {
    uint64_t registryStamp = 0;
    uint64_t dllStamp = 0;
};


class Invalidation {
public:
    void Start() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (thread_ || g_unloading.load()) {
            return;
        }
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        checkEvent_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!stopEvent_ || !checkEvent_) {
            if (stopEvent_) {
                CloseHandle(stopEvent_);
            }
            if (checkEvent_) {
                CloseHandle(checkEvent_);
            }
            stopEvent_ = nullptr;
            checkEvent_ = nullptr;
            return;
        }
        thread_ = CreateThread(nullptr, 0, &Invalidation::ThreadProc, this, 0, nullptr);
        if (!thread_) {
            CloseHandle(stopEvent_);
            CloseHandle(checkEvent_);
            stopEvent_ = nullptr;
            checkEvent_ = nullptr;
        }
    }

    // Asks for a handler check. Called when a menu is opened: the registry is
    // only inspected while menus are actually being used, not on a timer.
    // Cheap (SetEvent) and safe from any thread.
    void RequestCheck() {
        if (checkEvent_) {
            SetEvent(checkEvent_);
        }
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (stopEvent_) {
            SetEvent(stopEvent_);
        }
        // The worker must be joined before the module can unload; there is no
        // safe give-up path while its code is still on the stack.
        if (thread_) {
            WaitForSingleObject(thread_, INFINITE);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
    }

    // UI threads can still call RequestCheck until the unload wait is over,
    // so the handles are closed separately, after that wait.
    void CloseHandles() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
        if (checkEvent_) {
            CloseHandle(checkEvent_);
            checkEvent_ = nullptr;
        }
    }

private:
    static DWORD WINAPI ThreadProc(LPVOID param) {
        static_cast<Invalidation*>(param)->Run();
        return 0;
    }

    // Fingerprints the watched handler keys using their last-write times and
    // subkey stamps. Cheap registry reads; unlike change notifications, this
    // cannot storm when unrelated shell activity touches these keys.
    static uint64_t ComputeRegistryFingerprint() {
        static const wchar_t* kKeys[] = {
            L"*\\shellex\\ContextMenuHandlers",
            L"AllFilesystemObjects\\shellex\\ContextMenuHandlers",
            L"Directory\\shellex\\ContextMenuHandlers",
            L"Directory\\Background\\shellex\\ContextMenuHandlers",
            L"Folder\\shellex\\ContextMenuHandlers",
            L"Drive\\shellex\\ContextMenuHandlers",
            L"DesktopBackground\\shellex\\ContextMenuHandlers",
        };
        constexpr DWORD kKeyCount = ARRAYSIZE(kKeys);

        uint64_t fingerprint = 1469598103934665603ULL;
        for (DWORD i = 0; i < kKeyCount; ++i) {
            fingerprint = HashCombine(fingerprint, HashString(kKeys[i]));

            HKEY key = nullptr;
            if (RegOpenKeyExW(HKEY_CLASSES_ROOT, kKeys[i], 0, KEY_READ, &key) !=
                ERROR_SUCCESS) {
                fingerprint = HashCombine(fingerprint, 0);
                continue;
            }

            DWORD subKeys = 0;
            DWORD values = 0;
            FILETIME lastWrite = {};
            RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, &subKeys, nullptr, nullptr,
                             &values, nullptr, nullptr, nullptr, &lastWrite);
            fingerprint = HashCombine(fingerprint, subKeys);
            fingerprint = HashCombine(fingerprint, values);
            fingerprint = HashCombine(
                fingerprint,
                (static_cast<uint64_t>(lastWrite.dwHighDateTime) << 32) |
                    lastWrite.dwLowDateTime);

            // A change inside a handler's own subkey does not update the
            // parent's last-write time; stamp each subkey as well.
            for (DWORD j = 0; j < subKeys; ++j) {
                wchar_t name[256] = {};
                DWORD nameLength = ARRAYSIZE(name);
                FILETIME subLastWrite = {};
                if (RegEnumKeyExW(key, j, name, &nameLength, nullptr, nullptr, nullptr,
                                  &subLastWrite) != ERROR_SUCCESS) {
                    continue;
                }
                fingerprint = HashCombine(fingerprint, HashString(name));
                fingerprint = HashCombine(
                    fingerprint,
                    (static_cast<uint64_t>(subLastWrite.dwHighDateTime) << 32) |
                        subLastWrite.dwLowDateTime);
            }
            RegCloseKey(key);
        }
        return fingerprint;
    }

    void InvalidateNow(const wchar_t* reason) {
        g_cache.Clear();
        InvalidateSendToChildren();
        // The warm-up thread rebuilds the SendTo entries and pre-built models.
        // Never restart it once the mod is unloading.
        if (!g_unloading.load()) {
            g_warmup.Stop();
            g_warmup.Start();
        }
        Wh_Log(L"%s; cache invalidated", reason);
    }

    void Run() {
        uint64_t registryFingerprint = ComputeRegistryFingerprint();
        ULONGLONG lastCheck = GetTickCount64();

        for (;;) {
            HANDLE events[] = {stopEvent_, checkEvent_};
            const DWORD wait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
            if (wait == WAIT_OBJECT_0) {
                break;
            }
            if (wait != WAIT_OBJECT_0 + 1) {
                continue;
            }

            // Menus can open in bursts; never fingerprint more than once per
            // debounce window.
            const ULONGLONG now = GetTickCount64();
            if (now - lastCheck < 5000) {
                continue;
            }
            lastCheck = now;

            const uint64_t current = ComputeRegistryFingerprint();
            if (current != registryFingerprint) {
                registryFingerprint = current;
                InvalidateNow(L"Context menu handlers changed");
            }

            if (now >= nextRevalidationTick_) {
                nextRevalidationTick_ = now + 3600000;
                if (g_cache.RevalidateStamps()) {
                    Wh_Log(L"Handler modules changed; cache invalidated");
                    if (!g_unloading.load()) {
                        g_warmup.Stop();
                        g_warmup.Start();
                    }
                }
            }

            g_cache.MaybeSave(CacheFilePath());
        }
    }

    std::mutex lifecycleMutex_;
    HANDLE thread_ = nullptr;
    HANDLE stopEvent_ = nullptr;
    HANDLE checkEvent_ = nullptr;
    uint64_t nextRevalidationTick_ = 0;
};

inline Invalidation g_invalidation;

}  // namespace cmo

// ===========================================================================
// [CMO:Hooks] Hook functions and interception state.
// ===========================================================================
namespace cmo {

enum class MenuPath : uint8_t { Ours, NativeBypass, Passthrough };

// Decides which menu a popup owner gets. A held Shift normally bypasses the
// replacement before the menu is built (ShouldDeferContextMenu), so the
// NativeBypass branch only covers the rare case of Shift being pressed after
// the menu was already deferred; it shows the retained native menu.
MenuPath DecidePath(bool shiftHeld, ShellViewKind kind, bool hasPendingCapture,
                    bool enableShiftBypass) {
    if (!hasPendingCapture || !IsReplaceableKind(kind)) {
        return MenuPath::Passthrough;
    }
    if (shiftHeld && enableShiftBypass) {
        return MenuPath::NativeBypass;
    }
    return MenuPath::Ours;
}

std::wstring DirectoryOfPath(const std::wstring& path) {
    const size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) {
        return L"";
    }
    return path.substr(0, slash);
}

std::wstring SelectionDirectory(const std::vector<std::wstring>& paths,
                                Scope scope) {
    if (paths.empty()) {
        return L"";
    }
    if (scope == Scope::Folders || scope == Scope::Drive) {
        return paths.front();
    }
    return DirectoryOfPath(paths.front());
}

bool ShowReplacementMenu(PendingCapture& capture, ShellViewKind kind, HWND owner, POINT pt) {
    UiBusyScope uiBusy;
    g_perf.MarkOpenPathStart();
    // Handler changes are checked while menus are used, not on a timer.
    g_invalidation.RequestCheck();
    // Register this thread for unload teardown before any menu can block it,
    // so Wh_ModUninit can end the menu and destroy the windows on this thread.
    const bool haveControlWindow = EnsureControlWindow();
    if (!haveControlWindow) {
        Wh_Log(L"Control window unavailable; native menus only");
    }

    SelectionInfo info = GetSelection(owner, kind);
    std::vector<std::wstring>& paths = info.paths;
    Shape shape = paths.size() > 1 ? Shape::Multi : Shape::Single;

    // The navigation pane has no resolvable selection path; its model comes
    // from the captured shell menu instead.
    const bool capturedMenuContext = kind == ShellViewKind::NavPane;

    if (!capturedMenuContext &&
        !IsFilesystemContext(info.folderIsFilesystem, info.allItemsAreFilesystem)) {
        if (g_settings->debugLogging) Wh_Log(L"Non-filesystem namespace: using the native menu");
        ShowNativeReplay(capture, owner, pt);
        g_warmup.SetMenuOpen(false);
        return true;
    }

    Scope scope = capturedMenuContext
                      ? Scope::NavPane
                      : RefineScope(ScopeFromKind(kind, paths.empty()),
                                    info.allItemsAreFolders, AllPathsAreDrives(paths));
    std::wstring typeKey =
        scope == Scope::Files ? MakeTypeKey(paths) : std::wstring(L"*");
    if (scope == Scope::NavPane) {
        const std::wstring nodeKey = NavPaneNodeKey(owner);
        if (!nodeKey.empty()) {
            typeKey = L"nav:" + ToLowerCopy(nodeKey);
        }
    }
    ContextSignature signature{scope, typeKey, shape, Variant::Normal};

    const DWORD clipboardSequence = GetClipboardSequenceNumber();
    const bool clipboardHadData = ClipboardHasFileData();

    g_warmup.SetMenuOpen(true);

    while (true) {
        std::optional<MenuModel> cached;
        bool needsDiscovery = false;
        MenuModel model;
        if (capturedMenuContext) {
            const bool navKeyed = typeKey != L"*";
            if (navKeyed) {
                cached = g_cache.Find(signature);
            }
            if (cached) {
                model = *cached;
                if (g_settings->debugLogging) {
                    Wh_Log(L"NavPane: cache hit key=%s (%zu items)",
                           typeKey.c_str(), model.items.size());
                }
            } else {
                // Populate the retained menu (also QIs IContextMenu2/3) and
                // initialize it, exactly like the normal discovery path;
                // otherwise dynamic labels and submenus are empty and get
                // pruned.
                if (g_settings->debugLogging) {
                    Wh_Log(L"NavPane: capture idFirst=%u obj=%p flags=%08X",
                           capture.idCmdFirst, capture.obj, capture.flags);
                }
                if (EnsureContextPopulated(capture)) {
                    if (!capture.menuInitialized) {
                        InitializeMenuRecursive(capture, capture.populatedMenu,
                                                0);
                        capture.menuInitialized = true;
                    }
                    model = BuildModelFromHMenu(capture.populatedMenu,
                                                capture.idCmdFirst, signature,
                                                capture.obj);
                    if (g_settings->debugLogging) {
                        Wh_Log(L"NavPane: captured %zu items (key=%s)",
                               model.items.size(), typeKey.c_str());
                        DumpModelItems(model.items, 0);
                    }
                    ApplyRegistryIcons(model.items, signature);
                    if (navKeyed && !model.items.empty()) {
                        g_cache.Put(model);  // copy for the next open
                    }
                } else if (g_settings->debugLogging) {
                    Wh_Log(L"NavPane: population failed");
                }
            }
        } else {
            cached = g_cache.Find(signature);
            needsDiscovery = !cached || (cached->flags & kModelWarmup);
            if (g_settings->debugLogging) Wh_Log(L"Cache %s: scope=%d key=%s shape=%d paths=%zu",
                   cached ? (needsDiscovery ? L"warm" : L"hit") : L"miss",
                   static_cast<int>(scope), typeKey.c_str(),
                   static_cast<int>(shape), paths.size());

            // A true cache miss has no menu to show. Populate first: the shell's
            // population blocks the UI thread, so a placeholder menu would sit
            // blank until the real menu replaced it. The native menu pays the
            // same first-open cost. Warm entries are shown provisionally and
            // refreshed after the menu closes.
            if (!cached && !capture.discoveryDone) {
                if (g_settings->debugLogging) Wh_Log(L"Populating before showing the menu");
                DiscoverIntoCache(capture, signature);
                cached = g_cache.Find(signature);
                needsDiscovery = !cached || (cached->flags & kModelWarmup);
            }

            model = cached
                        ? MergeCoreWithCached(BuildCoreModel(scope, paths, shape),
                                              *cached)
                        : BuildCoreModel(scope, paths, shape);
        }
        if (!g_settings->showMoreOptionsItem) {
            std::erase_if(model.items, [](const MenuItem& item) {
                return item.action == ActionKind::Fallback;
            });
        }

        g_configStore.EnsureLoaded();
        g_configStore.RefreshIfChanged();
        g_themeStore.RefreshIfChanged();
        ItemContext itemCtx{};
        itemCtx.scope = scope;
        itemCtx.shape = shape;
        itemCtx.paths = paths;
        std::shared_ptr<const RulesConfig> rules = g_configStore.Snapshot();
        bool hasMoveRules = false;
        if (rules) {
            hasMoveRules =
                ApplyRulesConfigToModel(model, *rules, itemCtx).hasMoveRules;
        }

        // Fill the Send to submenu before pruning: an empty submenu is
        // removed, and a cached shell submenu already donated its children
        // during the merge.
        for (MenuItem& item : model.items) {
            if (item.canonicalVerb == L"paste" && !clipboardHadData) {
                item.flags |= kModelDisabled;
            }
            if (item.kind == ItemKind::Submenu && item.label == L"Send to" &&
                item.children.empty()) {
                item.children = GetSendToChildren();
            }
        }

        DumpSuspiciousItems(model.items, 0);
        PruneMenuItems(model.items);

        if (ShouldShowNativeReplay(model.flags)) {
            if (g_settings->debugLogging) Wh_Log(L"Owner-draw context: using the native menu");
            ShowNativeReplay(capture, owner, pt);
            break;
        }

        if (capturedMenuContext && g_settings->debugLogging) {
            Wh_Log(L"NavPane: %zu items after prune", model.items.size());
        }
        if (!hasMoveRules) {
            AdvancedGroupingOptions grouping;
            grouping.moveWindows = g_settings->advancedSubmenuWindows;
            grouping.moveThirdParty = g_settings->advancedSubmenuThirdParty;
            grouping.label = g_settings->advancedSubmenuLabel;
            grouping.windowsItems = g_settings->advancedSubmenuItems;
            grouping.exclude = g_settings->advancedSubmenuExclude;
            grouping.rules = rules ? rules.get() : nullptr;
            grouping.ctx = itemCtx;
            ReorganizeAdvancedItems(model.items, grouping);
        }

        if (g_settings->menuMode == 0) {
            std::vector<MenuItem>* container = &model.items;
            for (MenuItem& item : model.items) {
                if (item.kind == ItemKind::Submenu &&
                    item.label == g_settings->advancedSubmenuLabel) {
                    container = &item.children;
                    break;
                }
            }
            if (scope == Scope::Folders || scope == Scope::Drive) {
                AppendOpenNewProcessEntry(*container);
            }
            AppendSettingsEntry(*container);
        }

        if (scope == Scope::Background || scope == Scope::Desktop) {
            ApplyViewStateChecks(model.items, owner, kind);
        }
        if (g_settings->debugLogging) Wh_Log(L"Menu prep: %llu ms",
               static_cast<unsigned long long>(g_perf.OpenPathElapsedMs()));

        bool creationFailed = false;
        std::optional<uint32_t> chosen;
        bool customShown = false;
        const MenuMode mode = ResolveMenuMode(
            g_settings->menuMode, g_modeController.ConsecutiveFailures());
        if (mode == MenuMode::Custom && !g_unloading.load()) {
            if (!haveControlWindow) {
                // Without the control window the UI thread cannot be asked to
                // tear the windows down before unload; stay native instead of
                // risking windows that outlive the DLL.
                Wh_Log(L"Control window unavailable; using the HMENU path");
                g_modeController.RecordFailure();
            } else {
                const RulesConfig emptyConfig;
                const RulesConfig& effectiveRules = rules ? *rules : emptyConfig;
                const LayoutKey layoutKey =
                    MakeLayoutKey(signature, effectiveRules, DpiForWindow(owner),
                                  IsDarkThemeActive(), model,
                                  g_themeStore.Revision());

                const CustomMenuResult custom =
                    ShowCustomMenu(model, layoutKey, owner, pt);
                if (custom.busy) {
                    // Another Explorer UI thread owns the renderer; show the
                    // real native menu rather than racing its device, windows
                    // and session state.
                    if (g_settings->debugLogging) {
                        Wh_Log(L"Custom menu busy; using the native menu");
                    }
                    ShowNativeReplay(capture, owner, pt);
                    g_warmup.SetMenuOpen(false);
                    return true;
                }
                if (custom.failed) {
                    Wh_Log(L"Custom menu failed; using the HMENU path");
                    g_modeController.RecordFailure();
                } else {
                    g_modeController.RecordSuccess();
                    customShown = true;
                    chosen = custom.chosenItemId;
                }
            }
        }
        if (!customShown) {
            chosen = NativeMenuView::Show(model, owner, pt, &creationFailed);
        }

        if (creationFailed) {
            Wh_Log(L"Menu creation failed; using the native menu");
            ShowNativeReplay(capture, owner, pt);
            if (needsDiscovery && !capture.discoveryDone) {
                DiscoverIntoCache(capture, signature);
            }
            break;
        }

        if (chosen) {
            const MenuItem* item = FindById(model, *chosen);
            if (item) {
                InvocationContext ctx{};
                ctx.owner = owner;
                ctx.pt = pt;
                ctx.paths = paths;
                ctx.liveContext = capture.obj;
                ctx.idCmdFirst = capture.idCmdFirst;
                ctx.kind = kind;
                ctx.clipboardSequence = clipboardSequence;
                ctx.clipboardHadData = clipboardHadData;
                ctx.config = rules;
                ctx.directory = SelectionDirectory(paths, scope);
                if (ctx.directory.empty()) {
                    ctx.directory = GetCurrentFolderPath(owner, kind);
                }

                InvokeResult result = InvokeResult::Failed;
                if (item->flags & kModelExtension) {
                    result = (model.flags & kModelOwnerDraw)
                                 ? InvokeResult::FallbackNative
                                 : InvokeExtensionItem(*item, ctx, capture);
                } else if (item->action == ActionKind::ViewAction) {
                    // View actions act on the view's current selection; never
                    // act on a different selection than the one captured.
                    SelectionInfo current = GetSelection(owner, kind);
                    result = PathSetsEqual(current.paths, paths)
                                 ? InvokeItem(*item, ctx, capture)
                                 : InvokeResult::FallbackNative;
                } else {
                    result = InvokeItem(*item, ctx, capture);
                }

                if (g_settings->debugLogging) Wh_Log(L"Invoke '%s' -> %d", item->label.c_str(),
                       static_cast<int>(result));

                if (capturedMenuContext && typeKey != L"*") {
                    // An invocation can change the node (pin/unpin), so the
                    // next open repopulates instead of showing a stale menu.
                    g_cache.Remove(signature);
                }

                if (result == InvokeResult::FallbackNative) {
                    ShowNativeReplay(capture, owner, pt);
                }
            }
            break;
        }

        // Warm-up entries are provisional: refresh them from the real context
        // now that the menu has closed. Live entries never populate again.
        if (needsDiscovery && !capture.discoveryDone) {
            DiscoverIntoCache(capture, signature);
        }
        break;
    }

    g_warmup.SetMenuOpen(false);
    return true;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd,
                                  LPTPMPARAMS lptpm) {
    ShellViewKind kind = ClassifyOwner(hWnd);
    g_pending->ExpireOlderThan(GetTickCount64(), 60000);
    PendingCapture pending{};
    const bool hasPending =
        g_pending->TakeForMenu(hMenu, pending, kind == ShellViewKind::NavPane);
    const bool shiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const MenuPath path =
        DecidePath(shiftHeld, kind, hasPending, g_settings->enableShiftBypass);

    if (path == MenuPath::Ours && hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Replacing context menu: kind=%d", static_cast<int>(kind));
        pending.owner = hWnd;
        ShowReplacementMenu(pending, kind, hWnd, POINT{x, y});
        ReleaseCapture(pending);
        return 0;
    }

    if (path == MenuPath::NativeBypass && hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Shift bypass: showing the native menu");
        // The hook frame stays on this thread's stack while the native menu
        // runs, so unload must wait for it.
        UiBusyScope uiBusy;
        EnsureControlWindow();
        ShowNativeReplay(pending, hWnd, POINT{x, y});
        ReleaseCapture(pending);
        return 0;
    }

    if (hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Passing through: kind=%d", static_cast<int>(kind));
        ReplayInto(pending.obj, hMenu, pending.indexMenu, pending.idCmdFirst,
                   pending.idCmdLast, pending.flags);
        ReleaseCapture(pending);
    }
    {
        UiBusyScope uiBusy;
        EnsureControlWindow();
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    }
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved,
                                HWND hWnd, const RECT* prcRect) {
    ShellViewKind kind = ClassifyOwner(hWnd);
    g_pending->ExpireOlderThan(GetTickCount64(), 60000);
    PendingCapture pending{};
    const bool hasPending =
        g_pending->TakeForMenu(hMenu, pending, kind == ShellViewKind::NavPane);
    const bool shiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const MenuPath path =
        DecidePath(shiftHeld, kind, hasPending, g_settings->enableShiftBypass);

    if (path == MenuPath::Ours && hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Replacing context menu: kind=%d", static_cast<int>(kind));
        pending.owner = hWnd;
        ShowReplacementMenu(pending, kind, hWnd, POINT{x, y});
        ReleaseCapture(pending);
        return 0;
    }

    if (path == MenuPath::NativeBypass && hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Shift bypass: showing the native menu");
        // The hook frame stays on this thread's stack while the native menu
        // runs, so unload must wait for it.
        UiBusyScope uiBusy;
        EnsureControlWindow();
        ShowNativeReplay(pending, hWnd, POINT{x, y});
        ReleaseCapture(pending);
        return 0;
    }

    if (hasPending) {
        if (g_settings->debugLogging) Wh_Log(L"Passing through: kind=%d", static_cast<int>(kind));
        ReplayInto(pending.obj, hMenu, pending.indexMenu, pending.idCmdFirst,
                   pending.idCmdLast, pending.flags);
        ReleaseCapture(pending);
    }
    {
        UiBusyScope uiBusy;
        EnsureControlWindow();
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    }
}

// --- Windows 11 modern menu suppression ------------------------------------
// Technique adapted from the explorer-context-menu-classic mod by m417z
// (MIT-licensed Windhawk mod collection): fail the presenter lookup so
// Explorer falls back to the classic IContextMenu path, and block the
// navigation pane's mini menu, whose tree control makes its own decision
// that never goes through the presenter lookup. The Shift key opts out,
// matching stock behavior.

bool IsWindows11OrGreater() {
    static const bool isWin11 = [] {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        if (!ntdll) {
            return false;
        }
        auto rtlGetVersion =
            (LONG(WINAPI*)(OSVERSIONINFOW*))GetProcAddress(ntdll, "RtlGetVersion");
        if (!rtlGetVersion) {
            return false;
        }
        OSVERSIONINFOW info = {};
        info.dwOSVersionInfoSize = sizeof(info);
        if (rtlGetVersion(&info) != 0) {
            return false;
        }
        return info.dwMajorVersion >= 10 && info.dwBuildNumber >= 22000;
    }();
    return isWin11;
}

// GUIDs from shell32.dll, CDefView::TryGetContextMenuPresenter.
constexpr GUID kContextMenuPresenterService = {
    0xb306c5b1, 0xb4f2, 0x473c, {0xb6, 0xff, 0x70, 0x1b, 0x24, 0x6c, 0xe2, 0xd2}};
constexpr GUID kContextMenuPresenterIid = {
    0x706461d1, 0xac5f, 0x4730, {0xbf, 0xe3, 0xca, 0xc6, 0xca, 0xd5, 0xef, 0x5e}};
// Changed to this version in update KB5052093 of Windows 11 version 24H2.
constexpr GUID kContextMenuPresenterIid24H2 = {
    0x37a472f7, 0x63cf, 0x4ccf, {0xa8, 0x8b, 0x52, 0x31, 0xa3, 0xc7, 0xd8, 0xb6}};

using IUnknown_QueryService_t = decltype(&IUnknown_QueryService);
inline IUnknown_QueryService_t IUnknown_QueryService_Original = nullptr;

HRESULT WINAPI IUnknown_QueryService_Hook(IUnknown* punk, REFGUID guidService,
                                          REFIID riid, void** ppvOut) {
    if (IsEqualGUID(guidService, kContextMenuPresenterService) &&
        (IsEqualGUID(riid, kContextMenuPresenterIid) ||
         IsEqualGUID(riid, kContextMenuPresenterIid24H2))) {
        if (ArmCtrlBypass() || ShiftBypassHeld()) {
            // Ctrl: let the untouched modern menu through and keep the sticky
            // window armed for its "Show more options". Shift: let the stock
            // Shift behavior (the classic menu) through untouched.
            return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
        }
        // Suppressed otherwise so the classic path (and our replacement) is
        // used by default.
        if (g_settings->debugLogging) Wh_Log(L"Blocking modern context menu presenter");
        if (ppvOut) {
            *ppvOut = nullptr;
        }
        return E_FAIL;
    }
    return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
}

using ShouldShowMiniMenu_t = bool(WINAPI*)(void*, void*);
inline ShouldShowMiniMenu_t ShouldShowMiniMenu_Original = nullptr;

bool WINAPI ShouldShowMiniMenu_Hook(void* pThis, void* param) {
    if (ArmCtrlBypass() || ShiftBypassHeld()) {
        // Ctrl: let the untouched mini menu through. Shift: stock behavior.
        return ShouldShowMiniMenu_Original
                   ? ShouldShowMiniMenu_Original(pThis, param)
                   : true;
    }
    // Suppressed otherwise so the classic path (and our replacement) always
    // handles navigation pane menus.
    if (g_settings->debugLogging) Wh_Log(L"Blocking modern navigation pane mini menu");
    return false;
}

void InstallWin11Suppression() {
    if (!IsWindows11OrGreater()) {
        return;
    }

    // shcore.dll is a KnownDLL and is already loaded.
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (shcore) {
        auto queryService =
            (IUnknown_QueryService_t)GetProcAddress(shcore, "IUnknown_QueryService");
        if (queryService) {
            if (!WindhawkUtils::SetFunctionHook(queryService,
                                                IUnknown_QueryService_Hook,
                                                &IUnknown_QueryService_Original)) {
                Wh_Log(L"Failed to hook IUnknown_QueryService");
            }
        }
    } else {
        Wh_Log(L"Failed to load shcore.dll");
    }

    HMODULE explorerFrame =
        LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (explorerFrame) {
        WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
            {
                {LR"(private: bool __cdecl CNscTree::ShouldShowMiniMenu(struct _TREEITEM *))"},
                (void**)&ShouldShowMiniMenu_Original,
                (void*)ShouldShowMiniMenu_Hook,
                false,
            },
        };
        if (!WindhawkUtils::HookSymbols(explorerFrame, explorerFrameDllHooks,
                                        ARRAYSIZE(explorerFrameDllHooks))) {
            Wh_Log(L"Failed to hook CNscTree::ShouldShowMiniMenu");
        }
    } else {
        Wh_Log(L"Failed to load explorerframe.dll");
    }
}

}  // namespace cmo

// ===========================================================================
// [CMO:ModLifecycle] Windhawk entry points.
// ===========================================================================

namespace {
bool g_populationHookDeferred = false;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Custom Cached Context Menu init");

    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx,
                                        cmo::TrackPopupMenuEx_Hook,
                                        &cmo::TrackPopupMenuEx_Original)) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu,
                                        cmo::TrackPopupMenu_Hook,
                                        &cmo::TrackPopupMenu_Original)) {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }

    // Record bitmaps extensions attach with SetMenuItemBitmaps; the API has
    // no getter, and this is how the native menu gets their icons.
    if (!WindhawkUtils::SetFunctionHook(SetMenuItemBitmaps,
                                        cmo::SetMenuItemBitmaps_Hook,
                                        &cmo::SetMenuItemBitmaps_Original)) {
        Wh_Log(L"Failed to hook SetMenuItemBitmaps");
    }

    cmo::LoadSettings();
    cmo::g_themeStore.EnsureLoaded();
    cmo::g_lastAppliedTheme = cmo::g_settings->themeIndex;
    cmo::g_iconCache->PreloadCoreIcons(GetSystemMetrics(SM_CXSMICON));
    cmo::g_hotkeyHost.Start();

    const std::wstring cachePath = cmo::CacheFilePath();
    if (!cachePath.empty()) {
        if (cmo::g_settings->clearCache) {
            DeleteFileW(cachePath.c_str());
        } else if (cmo::g_cache.Load(cachePath)) {
            Wh_Log(L"Loaded menu cache");
        } else {
            // Corrupt or unreadable cache: remove it and rebuild.
            DeleteFileW(cachePath.c_str());
        }
    }
    cmo::g_invalidation.Start();
    cmo::g_warmup.Start();
    cmo::RebuildSendToChildren();

    if (cmo::InstallPopulationHook()) {
        Wh_Log(L"Population hook installed");
        // Only hide the modern menu once the replacement can actually run.
        cmo::InstallWin11Suppression();
    } else {
        Wh_Log(L"Population hook deferred to Wh_ModAfterInit");
        g_populationHookDeferred = true;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_populationHookDeferred && cmo::InstallPopulationHook()) {
        g_populationHookDeferred = false;
        // Register every hook before applying: operations registered after
        // Wh_ModInit returns are not applied automatically.
        cmo::InstallWin11Suppression();
        Wh_ApplyHookOperations();
        Wh_Log(L"Population hook installed after init");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Custom Cached Context Menu uninit");
    cmo::g_unloading.store(true);

    // Workers use the caches and the render device; stop them before anything
    // is released. The invalidation thread first: it can restart warm-up.
    cmo::g_invalidation.Stop();
    cmo::g_warmup.Stop();


    // The hotkey thread may be running a settings session; join it before the
    // per-thread teardown below.
    cmo::g_hotkeyHost.Stop();

    // Destroy the windows (and unregister the classes) on the UI thread that
    // owns them. DestroyWindow from this thread cannot destroy another
    // thread's windows, and the survivors would keep window procedures that
    // point into the unloaded DLL.
    // Ask every UI thread that has a control window to tear down its windows
    // and end any menu, then wait until they are all gone and no menu work
    // remains on any stack. There is no safe give-up path: unloading with mod
    // frames still on a stack crashes on return.
    std::vector<HWND> controls;
    {
        std::lock_guard<std::mutex> lock(cmo::g_controlWindowsMutex);
        for (const auto& pair : cmo::g_controlWindows) {
            controls.push_back(pair.second);
        }
    }
    for (HWND control : controls) {
        PostMessageW(control, cmo::kControlUninitMessage, 0, 0);
    }
    for (;;) {
        size_t remaining = 0;
        {
            std::lock_guard<std::mutex> lock(cmo::g_controlWindowsMutex);
            // A thread can exit and have its window destroyed before the
            // teardown message is processed; drop those entries here.
            for (auto it = cmo::g_controlWindows.begin();
                 it != cmo::g_controlWindows.end();) {
                if (IsWindow(it->second)) {
                    ++it;
                } else {
                    it = cmo::g_controlWindows.erase(it);
                }
            }
            remaining = cmo::g_controlWindows.size();
        }
        if (remaining == 0 && cmo::g_activeSessions.load() == 0 &&
            cmo::g_uiBusy.load() == 0) {
            break;
        }
        Sleep(10);
    }

    // Safety net in case a control window could not be created.
    HHOOK hook = cmo::g_menuMouseHook.exchange(nullptr);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    // Captures on unreachable threads stay leaked (their COM references are
    // intentionally not released from the wrong thread); the queue's own
    // memory can go.
    cmo::g_pending.reset();
    cmo::g_invalidation.CloseHandles();
    cmo::ReleaseOverlayResources();
    // Best effort: the per-thread cleanup may have raced an open session and
    // failed to unregister the classes; all windows are gone by now.
    UnregisterClassW(cmo::kMenuWindowClass, cmo::ModModuleHandle());
    UnregisterClassW(cmo::kControlWindowClass, cmo::ModModuleHandle());

    // Fully release the resource-owning globals; the CMO_NO_DESTROY ones
    // would otherwise hold their resources until process exit.
    cmo::g_menuWindowSet.reset();
    cmo::g_layoutCache.reset();
    cmo::g_contentCaches.reset();
    cmo::g_iconCache.reset();
    cmo::g_renderDevice.Shutdown();
    cmo::RestoreMenuDelay();

    const std::wstring cachePath = cmo::CacheFilePath();
    if (!cachePath.empty()) {
        cmo::g_cache.Save(cachePath);
    }
}

void Wh_ModSettingsChanged() {
    if (cmo::g_unloading.load()) {
        return;
    }
    Wh_Log(L"Custom Cached Context Menu settings changed");
    cmo::LoadSettings();
    cmo::g_hotkeyHost.SettingsChanged();

    if (cmo::g_settings->themeIndex != cmo::g_lastAppliedTheme) {
        cmo::g_themeStore.ApplySelectedTheme(cmo::g_settings->themeIndex);
        cmo::g_configStore.RefreshIfChanged();
    }

    if (cmo::g_settings->clearCache) {
        cmo::g_cache.Clear();
        const std::wstring cachePath = cmo::CacheFilePath();
        if (!cachePath.empty()) {
            DeleteFileW(cachePath.c_str());
        }
    }
}
