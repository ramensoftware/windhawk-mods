// ==WindhawkMod==
// @id start-menu-category-manager
// @name Start Menu Category Manager
// @name:pt-BR Gerenciador de Categorias do Menu Iniciar
// @description Adds a "Manage Categories" item to the Windows 11 Start menu's right-click menu, for customising app categories.
// @description:pt-BR Adiciona o item "Gerenciar Categorias" ao menu de contexto (botão direito) do Menu Iniciar do Windows 11, para personalizar as categorias dos aplicativos.
// @version 1.0.0
// @author Feuer
// @github https://github.com/feuerdraws
// @twitter https://x.com/feuerdraws
// @include explorer.exe
// @include StartMenuExperienceHost.exe
// @architecture x86-64
// @compilerOptions -lcomctl32 -lshell32 -lole32 -loleaut32 -lshlwapi -lruntimeobject -luuid -lgdi32 -luser32 -lcomdlg32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Category Manager

![Demonstration:](https://raw.githubusercontent.com/feuerdraws/start-menu-category-manager-assets/207a45adaff2a86e29031de2c1bafdd93ea6810f/Assets/Start_Menu_Category_Manager_Demonstration.gif)

> **Disclaimer:** this mod is vibe coded. It was written with an AI assistant and tested only by its author on
> one setup. It changes what the Start menu reads from its category settings and restarts the Start menu process, so
> use it at your own risk and keep a backup of your layout (**Export** saves one).

Adds a **Manage Categories** item to the Windows 11 Start menu's context menu
(right-click an empty area of the Start menu, next to **Start settings**).

Choosing **Manage Categories** closes the Start menu and opens the manager window.

The manager window always runs inside `explorer.exe`. The small part of the mod that runs
inside the Start menu process only adds the menu item and asks Explorer to open the window.
If the item does not show up, the Windhawk log (lines starting with `[start]`) shows whether
the Start menu process found the **Start settings** item.

Whenever the mod is loaded (installed, updated, switched on again, or Explorer restarted), it restarts the Start menu
process if that process started before the mod. The new process loads with the mod from its start, so the new menu
item appears without restarting Explorer.

**Your layout lives in this mod.** The mod does not rewrite Windows' own category data. It keeps the categories you
choose in its own storage, and the part of the mod that runs inside the Start menu process gives Start your category
when Start reads an app's category. If you disable or uninstall the mod, every category goes back to the one Windows
chose, so keep the mod installed to keep your layout. **Export** saves a copy of it to a file.

The one thing the mod still writes under `TileProperties` is an empty key (no values) for an app that Start has no key
for yet, so that Start lists the app. Those empty keys are harmless and stay when the mod is removed.

**Known issue:** if you move every app out of the *Other* category, Windows 11 still shows an empty *Other* container
in the Start menu. That is Start's own behavior. Please report it to Microsoft through the Feedback Hub.

**Export** saves the category changes the mod applies (not Windows' own categories), including apps that are not
installed, to a text file. **Import** adds such a file to your current changes. Export, Import and Reset all ask for
confirmation first.

**Reset all** removes every change the mod applies, so every app goes back to the category Windows chose. It first
writes a copy of your current layout to the mod's storage folder, which **Import** can load. Windhawk removes that
folder together with the mod, so use **Export** if you want a copy that stays.

The manager lists every app the Start menu knows about (desktop apps and Microsoft
Store apps, taken from `shell:AppsFolder`) and lets you assign a Start menu category
to an app or to a whole Start menu folder.

## Notes
- Shortcuts inside a folder that contains two or more shortcuts are treated as a
  group: changing one changes the whole folder, because the Start menu groups them.
- Choosing **Windows default** removes the mod's change for the app, so it goes back to the category Windows chose.
- Applying a change restarts StartMenuExperienceHost so it takes effect immediately.
- Windows keeps its categories under `HKCU\Software\Microsoft\Windows\CurrentVersion\Start\TileProperties`. The mod
  only reads that key (apart from the empty keys described above); its own changes are in the mod's storage.
- Start sometimes keeps several keys for the same app (a `P~` and a `W~` twin, and one
  key per Start menu shortcut). The mod gives the app's main key and all of its other keys the same category,
  so they can never disagree. Apps whose keys currently disagree (Windows' own values, for apps you have not changed)
  get a yellow **\*** after their name; hover it for details.
- A heading with a red **\*** after its name has 1 or 2 apps assigned to it, so Start folds those apps into a broader
  heading (see **Category IDs** below). A Start folder counts as one app, and *Other* is never marked. Hover the name
  for the same explanation. The list reloads by itself when a change makes a heading reach or leave that range.
- A folder whose apps are in different categories shows **Mixed** in its drop-down. Click a folder's card, or its
  arrow, to expand or collapse it.
- Press **F5** (or click the refresh button) to reload the app list. The refresh icon turns once and the status text
  blinks once, so you can see it went through.
- Hover any drop-down and turn the mouse wheel to go through its options. In a row's drop-down the new category is
  applied once the wheel has rested for about a second.
- The ID box at the bottom only appears after you choose **Custom ID...** in the bottom drop-down. Press **Enter**
  in it to apply the ID to the ticked apps straight away.
- Click an app's row (not its check box or drop-down) to see the registry key it uses in the status bar, below
  `TileProperties`, and how many other keys it has. A Start folder has no key of its own, so clicking its card shows how
  many apps it holds; each app inside shows its own key.
- The window follows the Windows display language: English, or Brazilian Portuguese
  (including the right-click menu item, **Gerenciar Categorias**).
- The drop-down next to the search box filters the list by the category assigned to each
  app. **Check all** ticks or unticks everything shown, and **Expand all** / **Collapse all**
  opens or closes every folder.
- Each entry of the category filter shows how many apps are assigned to it. The numbers count every
  app, not only the ones the search currently shows. Apps whose stored ID is not in the list are
  counted only under *All categories*.
- The **Sort** drop-down orders the list: A to Z, Z to A, by category ID (low to high or high to
  low), by category name, or with apps whose keys disagree first. Apps that have no override
  (*Windows default*) have no ID and are listed last in the ID and category orders. A folder is
  placed by the lowest ID among its apps (the highest for "high to low"). The order also applies
  inside each folder.
- Tick the boxes next to apps or folders to change many at once, then pick a category in the bottom drop-down and
  press **Apply to ticked apps**. It acts on every ticked app.
- Right-click an app or a folder card and choose **Open file location** to open the Start menu folder that holds its
  shortcut in File Explorer. For an app, the shortcut is selected. Apps without a Start menu shortcut (most Microsoft
  Store apps) have the item greyed out; clicking it says why in the status bar.
- Start stores a number per app. Choose **Custom ID...** to try any number from 0 to 999. The list shows
  each app's stored ID, so you can see which number Start itself used for an app that already sits under
  the heading you want.

## Category IDs

Microsoft publishes no list of the numbers Start uses, so these were found by testing on Windows 11 26H2
(build 26300). The pt-BR names are the headings exactly as Start shows them; the English names are
translations, so they may differ from what Start shows in English.

| Heading (English) | Heading (pt-BR, as Start shows it) | IDs |
|---|---|---|
| Accessibility | Acessibilidade | 1 |
| Business | Comercial | 20 |
| Creativity | Criatividade | 6, 7, 22, 23, 30 |
| Developer Tools | Ferramentas de Desenvolvedor | 15 |
| Education | Educação | 19 |
| Entertainment | Entretenimento | 16 |
| Food & Dining | Bares e Restaurantes | 29 |
| Games | Jogos | 3 |
| Health & Fitness | Saúde e Bem-estar | 12 |
| Information & Reading | Informações e Leitura | 14, 21, 24, 25, 31 |
| Kids & Family | Crianças + Família | 10 |
| Lifestyle | Estilo de vida | 27 |
| Medical | Medicina | 11 |
| Music | Música | 17 |
| Other | Outro | 0 |
| Personal Finance | Finanças Pessoais | 18 |
| Productivity | Produtividade | 2, 5, 13 |
| Shopping | Compras | 28 |
| Social | Social | 8 |
| Sports | Esportes | 26 |
| Travel | Viagem | 4 |
| Utilities & Tools | Utilitários e Ferramentas | 9 |

- Several IDs lead to the same heading (for example 2, 5 and 13 all show under *Productivity*).
- **Start only shows a heading as its own tile when at least 3 apps are assigned to it.** With fewer, those apps
  are shown under a broader heading, even though their ID is unchanged: *Games*, *Music* and *Sports* fall under
  *Entertainment*, and *Travel*, *Medical*, *Health & Fitness*, *Personal Finance*, *Shopping* and
  *Food & Dining* fall under *Lifestyle*. Give a third app the same ID and the tile appears by itself.
- An app inside a Start menu folder seems to count as one app, however many apps the folder holds.
- IDs above 31 have no name. An ID above 31 with 3 or more apps gets a heading with no title, and with fewer apps
  those apps fall under *Other*, so stay with the IDs above.

## Start menu restarts

Applying a change, **Import**, **Reset all**, loading the mod and disabling or removing it all restart the Start menu
process, so Start can flash or close for a moment each time. A restart is skipped when nothing actually changed, and
a Start process that started within the last second is left alone.

## Uninstalling or disabling

The mod makes no permanent change to Windows' own category values. When it is disabled, uninstalled or updated, the
Start menu process restarts and reads its own values again, so every category goes back to the one Windows chose. Your
layout stays in the mod's storage while the mod is installed, so disabling and enabling the mod (or updating it) brings
your categories back. Windhawk removes the storage together with the mod, so use **Export** first if you want to keep
your layout. The empty keys the mod created (see above) stay; they hold no values.

## Tested on

Only Windows 11 build 26300, on one computer, with Windows in Brazilian Portuguese. On any other build the mod may
fail to attach to the Start menu (see below).

## If a Windows update breaks the mod

The mod depends on private parts of the Start menu that Microsoft can change without notice. It was written and tested
on Windows 11 build 26300. When something stops matching, the mod keeps running and only the affected part stops
working. What you may see:

- **No "Manage Categories" item in the Start menu's right-click menu.** The menu hook did not attach. Windows then also
  shows a one-time message per Windows build.
- **Your categories stop applying** (the manager still opens and lists them). The hook on `RegGetValueW` in
  `kernelbase.dll`, which gives Start your category, is not being used by this build of Start. The log lines starting
  with `[start]` say whether it was installed and whether it served a category.

Where to look: the Windhawk log (lines starting with `[start]`) says whether the hooks were found. The only function the
mod hooks in the Start menu DLL (`StartMenu.dll`, or `StartUI.dll` on some builds) is
`consume_..._IVector<IVector<MenuFlyoutItemBase>>::Append`, which adds the menu item. To find its new name after an
update, search the Start menu DLL's symbols in Windhawk for that class and function name. The mod also hooks
`RegGetValueW` in `kernelbase.dll` inside the Start menu process, which `StartTileData.dll` uses to read each app's
`Category` value.
*/
// ==/WindhawkModReadme==

// ===========================================================================
// CODE MAP. Search for these banner titles; line numbers move.
//   Constants, Data, Helpers          shared constants, category tables, small helpers
//   Hook status                       what the Start menu process managed to hook (shared through named events)
//   Scanning                          registry and shell:AppsFolder
//   UI population
//   Category overrides                your choices, kept in the mod's storage (shared with Start)
//   Applying categories / Start menu restart   setting overrides, restarting Start
//   Reset all, Export and Import
//   XAML island hosting               the manager's list, toolbar and bottom bar
//   Custom title bar, Snap layouts    caption buttons, drag bar, hit testing
//   Confirmation dialog
//   Manager window procedure
//   Creating and showing the manager window
//   Opening the manager from the Start menu process   the named event between the two processes
//   Start menu context menu injection runs inside StartMenuExperienceHost.exe
//   Wh_ModInit and friends            entry points
// ===========================================================================

#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <cwchar>
#include <map>
#include <functional>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <cstring>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/base.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <propsys.h>
#include <objbase.h>
#include <cwctype>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <string_view>
#include <memory>
#include <sddl.h>
#include <commdlg.h>
#include <dwmapi.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxa = winrt::Windows::UI::Xaml::Media::Animation;

namespace {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

constexpr PROPERTYKEY kPkeyAppUserModelId = {
    {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};

constexpr wchar_t kRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Start\\TileProperties";
constexpr wchar_t kItemName[] = L"WindhawkStartCategoryManagerItem";
constexpr wchar_t kSeparatorName[] = L"WindhawkStartCategoryManagerSeparator";

// Icon of the "Manage Categories" item: the View (grid) glyph from the desktop context menu.
// If the wrong icon shows, look up the code of "View" in Character Map (Segoe Fluent Icons).
constexpr wchar_t kManageItemGlyph[] = L"\xE8A9";
// Start's own "Start settings" item uses the cog, which the injection looks for.
constexpr wchar_t kStartSettingsGlyph[] = L"\xE713";


constexpr UINT WM_APP_SHOW = WM_APP + 2;
constexpr UINT WM_APP_CONFIRM_DONE = WM_APP + 7;  // The confirmation dialog was answered.
constexpr UINT_PTR kConfirmTimerId = 0x434F4E46;
constexpr UINT WM_APP_THEME_CHANGED = WM_APP + 6;  // The light / dark mode of Windows changed under an open manager.

constexpr int kMaxScanDepth = 10;


// ---------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------

struct AppEntry {
    std::wstring relPath;  // Path relative to the Programs root (used to merge user + common).
    std::wstring primaryRegistryKey;
    std::vector<std::wstring> aliasRegistryKeys;
    std::wstring displayName;
    std::wstring folderPath;  // Empty = not grouped in a folder.
    DWORD category = 0;
    bool hasCategory = false;
    bool checked = false;   // Ticked for a batch change.
    bool conflict = false;  // Some of this app's keys resolve to a different category.
    int iconIndex = 0;      // Index into the shared system small image list.
};

struct ShortcutInfo {
    std::wstring name;
    std::wstring folder;    // Start menu subfolder, empty = directly in Programs.
    std::wstring fullPath;  // Full path of the .lnk/.url (Start keys are "W~" + this path).
};

// Start stores a numeric ID per app. Microsoft publishes no list, and several IDs can map to
// the same heading (sub-categories), so kCategories lists one ID per heading and CanonicalCategoryId maps the others
// onto it. IDs 0 to 31 are all mapped (tested on Windows 11 26H2, build 26300; the pt-BR names are Start's own, the English
// names are translations). Start shows a category as its own tile only when at least 3 apps carry it; with fewer, the
// apps fall under a broader one (Games, Music and Sports under Entertainment; Travel, Medical, Health & Fitness,
// Personal Finance, Shopping and Food & Dining under Lifestyle). An app inside a Start folder counts as one app. An ID
// above 31 has no heading of its own (with 3 or more apps it gets a blank one). Use "Custom ID..." to try others.
struct NativeCategory {
    const wchar_t* name;
    const wchar_t* namePt;  // The heading as Start shows it in Brazilian Portuguese.
    DWORD id;
};

const NativeCategory kCategories[] = {
    {L"Accessibility", L"Acessibilidade", 1},
    {L"Business", L"Comercial", 20},
    {L"Creativity", L"Criatividade", 7},
    {L"Developer Tools", L"Ferramentas de Desenvolvedor", 15},
    {L"Education", L"Educa\u00e7\u00e3o", 19},
    {L"Entertainment", L"Entretenimento", 16},
    {L"Food & Dining", L"Bares e Restaurantes", 29},
    {L"Games", L"Jogos", 3},
    {L"Health & Fitness", L"Sa\u00fade e Bem-estar", 12},
    {L"Information & Reading", L"Informa\u00e7\u00f5es e Leitura", 14},
    {L"Kids & Family", L"Crian\u00e7as + Fam\u00edlia", 10},
    {L"Lifestyle", L"Estilo de vida", 27},
    {L"Medical", L"Medicina", 11},
    {L"Music", L"M\u00fasica", 17},
    {L"Other", L"Outro", 0},
    {L"Personal Finance", L"Finan\u00e7as Pessoais", 18},
    {L"Productivity", L"Produtividade", 5},
    {L"Shopping", L"Compras", 28},
    {L"Social", L"Social", 8},
    {L"Sports", L"Esportes", 26},
    {L"Travel", L"Viagem", 4},
    {L"Utilities & Tools", L"Utilit\u00e1rios e Ferramentas", 9},
};

constexpr LONG_PTR kComboDefault = -1;  // "Windows default": remove the override.
constexpr DWORD kMaxCustomId = 999;

bool g_isDark = false;
int g_folderIconIndex = 0;
std::vector<AppEntry> g_apps;
std::vector<ShortcutInfo> g_shortcuts;    // Every Start menu shortcut found on disk.
int g_conflictsLogged = 0;

std::atomic<HWND> g_xamlListWindow{nullptr};  // The manager window; it is reused, not opened again.
std::atomic<bool> g_shuttingDown{false};
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
SRWLOCK g_uiLock = SRWLOCK_INIT;

std::atomic<bool> g_startModuleHooked = false;
bool g_isStartHost = false;  // True inside StartMenuExperienceHost.exe, false inside explorer.exe.
HANDLE g_openEvent = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_openThread = nullptr;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// English, or Brazilian Portuguese when Windows itself is displayed in Brazilian Portuguese.
// True when Windows apps are set to dark mode.
bool SystemUsesDarkApps() {
    DWORD light = 1;
    DWORD size = sizeof(light);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size) != ERROR_SUCCESS) {
        return false;
    }
    return light == 0;
}

bool IsPortuguese() {
    static const bool pt =
        GetUserDefaultUILanguage() == MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
    return pt;
}

const wchar_t* Tr(const wchar_t* en, const wchar_t* pt) {
    return IsPortuguese() ? pt : en;
}

std::wstring Lower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

bool ContainsInsensitive(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) return true;
    return Lower(haystack).find(Lower(needle)) != std::wstring::npos;
}

// Returns CSTR_LESS_THAN / CSTR_EQUAL / CSTR_GREATER_THAN.
int CompareNoCase(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), static_cast<int>(a.size()),
                                b.c_str(), static_cast<int>(b.size()), TRUE);
}

struct FolderLess {
    bool operator()(const std::wstring& a, const std::wstring& b) const {
        return CompareNoCase(a, b) == CSTR_LESS_THAN;
    }
};


// Start can show several IDs under one heading; this returns the ID that kCategories lists for that heading.
DWORD CanonicalCategoryId(DWORD id) {
    switch (id) {
        case 2: case 13: return 5;                 // Productivity
        case 6: case 22: case 23: case 30: return 7;  // Creativity
        case 21: case 24: case 25: case 31: return 14;  // Information & Reading
        default: return id;
    }
}

const NativeCategory* FindNativeCategory(DWORD id) {
    const DWORD canonical = CanonicalCategoryId(id);
    for (const auto& cat : kCategories) {
        if (cat.id == canonical) return &cat;
    }
    return nullptr;
}

std::wstring CategoryNameEn(DWORD id, bool hasCategory) {
    if (!hasCategory) return L"Windows default";
    if (const NativeCategory* cat = FindNativeCategory(id)) return cat->name;
    return L"Unknown ID (" + std::to_wstring(id) + L")";
}

// The heading name in the display language (Brazilian Portuguese names are Start's own).
std::wstring CategoryName(DWORD id, bool hasCategory) {
    if (!IsPortuguese()) return CategoryNameEn(id, hasCategory);
    if (!hasCategory) return L"Padr\u00e3o do Windows";
    if (const NativeCategory* cat = FindNativeCategory(id)) return cat->namePt;
    return L"ID desconhecido (" + std::to_wstring(id) + L")";
}

// Name plus the raw ID, so the list shows exactly what is stored in the registry.
std::wstring CategoryLabel(DWORD id, bool hasCategory) {
    const std::wstring name = CategoryName(id, hasCategory);
    if (!hasCategory) return name;
    if (CategoryNameEn(id, true).rfind(L"Unknown", 0) == 0) return name;
    return name + L", ID " + std::to_wstring(id);
}


// Sort orders offered by the Sort drop-down.
enum SortMode {
    kSortNameAsc = 0,
    kSortNameDesc,
    kSortIdAsc,
    kSortIdDesc,
    kSortHeadingAsc,
    kSortConflictsFirst,
};

struct SortKey {
    std::wstring name;
    bool hasId = false;  // False = no override ("Windows default"): listed last in the ID orders.
    DWORD id = 0;
    std::wstring heading;  // The category heading in the display language.
    bool conflict = false;
};


SortKey MakeSortKey(const AppEntry& app) {
    SortKey key;
    key.name = app.displayName;
    key.hasId = app.hasCategory;
    key.id = app.category;
    key.heading = app.hasCategory ? CategoryName(app.category, true) : std::wstring();
    key.conflict = app.conflict;
    return key;
}

// Strict weak ordering. Ties always fall back to the name, A to Z.
bool SortKeyLess(const SortKey& a, const SortKey& b, int mode) {
    auto byName = [&](bool descending) {
        const int c = CompareNoCase(a.name, b.name);
        return descending ? c == CSTR_GREATER_THAN : c == CSTR_LESS_THAN;
    };
    switch (mode) {
        case kSortNameDesc:
            return byName(true);
        case kSortIdAsc:
        case kSortIdDesc:
            if (a.hasId != b.hasId) return a.hasId;
            if (a.hasId && a.id != b.id) return mode == kSortIdAsc ? a.id < b.id : a.id > b.id;
            return byName(false);
        case kSortHeadingAsc: {
            if (a.hasId != b.hasId) return a.hasId;
            const int c = CompareNoCase(a.heading, b.heading);
            if (c != CSTR_EQUAL) return c == CSTR_LESS_THAN;
            return byName(false);
        }
        case kSortConflictsFirst:
            if (a.conflict != b.conflict) return a.conflict;
            return byName(false);
        default:
            return byName(false);
    }
}

bool ReadDword(HKEY key, const wchar_t* valueName, DWORD* value) {
    DWORD type = 0;
    DWORD size = sizeof(*value);
    const LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &type,
                                            reinterpret_cast<BYTE*>(value), &size);
    return status == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(*value);
}

std::wstring GetKnownFolder(REFKNOWNFOLDERID id) {
    std::wstring result;
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path)) && path) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

// This mod's own module handle. Window classes are registered under it, so they cannot clash with Explorer's own
// classes or with another copy of the mod.
HMODULE ModuleInstance() {
    static HMODULE module = nullptr;
    if (!module) {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModuleInstance), &module);
    }
    return module;
}

bool ReadTextFile(const std::wstring& path, std::wstring& out) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    GetFileSizeEx(file, &size);
    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD read = 0;
    const bool ok = ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read, nullptr) &&
                    read == bytes.size();
    CloseHandle(file);
    if (!ok) return false;
    if (bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB && static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
    }
    const int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), nullptr, 0);
    out.assign(length > 0 ? static_cast<size_t>(length) : 0, L'\0');
    if (length > 0) MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), out.data(), length);
    return true;
}

bool WriteTextFile(const std::wstring& path, const std::wstring& text) {
    const int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0,
                                           nullptr, nullptr);
    std::string bytes(length > 0 ? static_cast<size_t>(length) : 0, '\0');
    if (length > 0) {
        WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), bytes.data(), length,
                            nullptr, nullptr);
    }
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool ok = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr) &&
                    written == bytes.size();
    CloseHandle(file);
    return ok;
}

void SetXamlStatus(const std::wstring& text);

void SetStatus(const std::wstring& text) {
    SetXamlStatus(text);
}

// ---------------------------------------------------------------------------
// Hook status
// ---------------------------------------------------------------------------
// The Start menu process is sandboxed, so it cannot write to the registry or to Explorer's files. Named events are the
// channel that already works (it is how Start asks Explorer to open the manager). Explorer creates one manual-reset
// event per question and Start signals the ones whose answer is "yes". Start resets all of them when it starts, so an
// old answer is never kept. "Done" means Start has finished trying to hook the menu item.
enum HookPart { kHookMenu = 0, kHookDone, kHookPartCount };
constexpr const wchar_t* kHookPartNames[kHookPartCount] = {L"Menu", L"Done"};
HANDLE g_hookEvents[kHookPartCount] = {};
HANDLE g_hookWatchThread = nullptr;

std::wstring HookEventName(int part) {
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    return L"Global\\WindhawkCategoryManagerHook-" + std::to_wstring(session) + L"-" + kHookPartNames[part];
}

// For example "26300.1234": the build number and the update revision.
std::wstring WindowsBuildString() {
    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
        auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion) rtlGetVersion(&info);
    }
    std::wstring build = std::to_wstring(info.dwBuildNumber);
    DWORD revision = 0;
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE,
                      &key) == ERROR_SUCCESS) {
        ReadDword(key, L"UBR", &revision);
        RegCloseKey(key);
    }
    if (revision != 0) build += L"." + std::to_wstring(revision);
    return build;
}

// Explorer: creates the events. Called before Start is restarted, so they exist when the new Start process reports.
void CreateHookStatusEvents() {
    SECURITY_ATTRIBUTES sa{sizeof(sa)};
    PSECURITY_DESCRIPTOR sd = nullptr;
    // The same permissions as the open-request event: Everyone and all app packages may wait on it and signal it.
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;0x100002;;;WD)(A;;0x100002;;;AC)", SDDL_REVISION_1,
                                                              &sd, nullptr)) {
        sa.lpSecurityDescriptor = sd;
    }
    for (int part = 0; part < kHookPartCount; ++part) {
        const std::wstring name = HookEventName(part);
        HANDLE handle = CreateEventW(&sa, TRUE, FALSE, name.c_str());
        if (!handle && GetLastError() == ERROR_ACCESS_DENIED) {
            // The event of a previous Explorer that is still shutting down: join it, and drop its old answer.
            handle = OpenEventW(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name.c_str());
            if (handle) ResetEvent(handle);
        }
        g_hookEvents[part] = handle;
    }
    if (sd) LocalFree(sd);
}

void CloseHookStatusEvents() {
    for (int part = 0; part < kHookPartCount; ++part) {
        if (g_hookEvents[part]) CloseHandle(g_hookEvents[part]);
        g_hookEvents[part] = nullptr;
    }
}

struct HookStatus {
    bool known = false;     // Start has finished trying to hook the menu item.
    bool menuItem = false;  // The "Manage Categories" menu item hook is in.
};

HookStatus ReadHookStatus() {
    auto on = [](int part) { return g_hookEvents[part] && WaitForSingleObject(g_hookEvents[part], 0) == WAIT_OBJECT_0; };
    HookStatus status;
    status.known = on(kHookDone);
    status.menuItem = on(kHookMenu);
    return status;
}

// Start menu process: clears every answer when it starts.
void ResetHookReport() {
    for (int part = 0; part < kHookPartCount; ++part) {
        if (HANDLE handle = OpenEventW(EVENT_MODIFY_STATE, FALSE, HookEventName(part).c_str())) {
            ResetEvent(handle);
            CloseHandle(handle);
        }
    }
}

void SignalHookPart(int part) {
    if (HANDLE handle = OpenEventW(EVENT_MODIFY_STATE, FALSE, HookEventName(part).c_str())) {
        SetEvent(handle);
        CloseHandle(handle);
    }
}

// Start menu process: the result of hooking the menu item.
void ReportHooks(bool menuItem) {
    if (menuItem) SignalHookPart(kHookMenu);
    SignalHookPart(kHookDone);
    Wh_Log(L"[start] Windows build %ls: menu item hook %ls", WindowsBuildString().c_str(),
           menuItem ? L"ok" : L"NOT FOUND");
}

// Explorer: if the menu item hook failed, the manager cannot be opened at all, so a warning inside it would never be
// seen. This shows one message box per Windows build instead.
void WarnMenuItemMissing() {
    const std::wstring build = WindowsBuildString();
    wchar_t warned[64]{};
    Wh_GetStringValue(L"WarnedBuild", warned, ARRAYSIZE(warned));
    if (build == warned) return;
    // Recorded before the message is shown, so a crash cannot make it appear on every start.
    Wh_SetStringValue(L"WarnedBuild", build.c_str());

    const std::wstring text =
        std::wstring(Tr(L"Start Menu Category Manager could not add its \"Manage Categories\" item to the Start menu "
                        L"on Windows build ",
                        L"O Start Menu Category Manager n\u00e3o conseguiu adicionar o item \"Gerenciar Categorias\" "
                        L"ao Menu Iniciar no Windows build ")) +
        build +
        Tr(L".\n\nStart's internals most likely changed in a Windows update, so the mod needs an update. The Windhawk "
           L"log (lines starting with [start]) has the details.\n\nThis message is shown once per Windows build.",
           L".\n\nO Menu Iniciar provavelmente mudou em uma atualiza\u00e7\u00e3o do Windows, ent\u00e3o o mod precisa "
           L"ser atualizado. O log do Windhawk (linhas que come\u00e7am com [start]) tem os detalhes.\n\nEsta "
           L"mensagem aparece uma vez por build do Windows.");
    MessageBoxW(nullptr, text.c_str(), L"Start Menu Category Manager",
                MB_OK | MB_ICONWARNING | MB_SETFOREGROUND | MB_TOPMOST);
}

// Explorer: waits (up to 3 minutes) for Start's answer about the menu item, and warns if that hook did not attach. The
// manager cannot be opened then, so a warning inside it would never be seen.
DWORD WINAPI HookWatchThreadProc(LPVOID) {
    if (!g_stopEvent) return 0;
    for (int second = 0; second < 180; ++second) {
        // Anything but a timeout means the stop event was set, or it was closed while a message box was open.
        if (WaitForSingleObject(g_stopEvent, 1000) != WAIT_TIMEOUT || g_shuttingDown) return 0;
        const HookStatus status = ReadHookStatus();
        if (status.known) {
            if (!status.menuItem) WarnMenuItemMissing();
            return 0;
        }
    }
    return 0;  // Start never answered. Stop waiting for it.
}

void StartHookWatch() {
    g_hookWatchThread = CreateThread(nullptr, 0, HookWatchThreadProc, nullptr, 0, nullptr);
}

BOOL CALLBACK CloseThreadWindow(HWND hwnd, LPARAM) {
    PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}

// Must run before g_stopEvent is closed. The watch thread can be blocked in a message box (WarnMenuItemMissing), so that
// box is closed first. The thread runs this mod's code, so it is waited for without a time limit.
void StopHookWatch() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_hookWatchThread) {
        const DWORD threadId = GetThreadId(g_hookWatchThread);
        while (WaitForSingleObject(g_hookWatchThread, 50) == WAIT_TIMEOUT) {
            EnumThreadWindows(threadId, CloseThreadWindow, 0);
        }
        CloseHandle(g_hookWatchThread);
        g_hookWatchThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Category overrides
// ---------------------------------------------------------------------------

// The mod never rewrites Start's own category values. The categories you choose are kept as "overrides" in the mod's
// storage, and the copy of the mod inside the Start menu process answers Start's read of an app's Category value with
// the override (see RegGetValueW_Hook). Without the mod, Start reads its own values again.
// There is one override per registry key below TileProperties (for example "P~Vendor.App_abc!App"): the key and a
// category ID. Registry key names are not case sensitive, so the map is not either.
struct CaseInsensitiveLess {
    bool operator()(const std::wstring& a, const std::wstring& b) const { return _wcsicmp(a.c_str(), b.c_str()) < 0; }
};
using OverrideMap = std::map<std::wstring, DWORD, CaseInsensitiveLess>;

constexpr wchar_t kOverridesValueName[] = L"overrides";
constexpr size_t kOverridesBufferChars = 1u << 20;  // 2 MB of text: far more than any layout needs.

OverrideMap g_overrides;  // The manager's copy (Explorer): it works on this one and saves it.

// One "<key><TAB><category id>" line per override.
std::wstring SerializeOverrides(const OverrideMap& overrides) {
    std::wstring text;
    for (const auto& item : overrides) text += item.first + L"\t" + std::to_wstring(item.second) + L"\n";
    return text;
}

void ParseOverrides(const std::wstring& text, OverrideMap& out) {
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find(L'\n', pos);
        if (end == std::wstring::npos) end = text.size();
        std::wstring line = text.substr(pos, end - pos);
        pos = end + 1;
        while (!line.empty() && (line.back() == L'\r' || line.back() == L' ')) line.pop_back();
        const size_t tab = line.rfind(L'\t');
        if (line.empty() || line[0] == L'#' || tab == std::wstring::npos || tab == 0) continue;
        wchar_t* parseEnd = nullptr;
        const unsigned long value = wcstoul(line.c_str() + tab + 1, &parseEnd, 10);
        if (parseEnd == line.c_str() + tab + 1 || *parseEnd != L'\0') continue;
        out[line.substr(0, tab)] = static_cast<DWORD>(value);
    }
}

// Reads the overrides from the mod's storage. Both the manager (Explorer) and the Start menu process use this.
bool LoadOverridesFromStorage(OverrideMap& out) {
    out.clear();
    std::vector<wchar_t> buffer(kOverridesBufferChars, L'\0');
    Wh_GetStringValue(kOverridesValueName, buffer.data(), buffer.size());
    if (wcsnlen(buffer.data(), buffer.size()) + 1 >= buffer.size()) {
        Wh_Log(L"The saved category overrides are too long to read; none were loaded");
        return false;
    }
    ParseOverrides(std::wstring(buffer.data()), out);
    return true;
}

bool FindOverride(const std::wstring& key, DWORD* value) {
    const auto it = g_overrides.find(key);
    if (it == g_overrides.end()) return false;
    *value = it->second;
    return true;
}


// ---------------------------------------------------------------------------
// Scanning
// ---------------------------------------------------------------------------

bool ReadCategoryAt(HKEY root, const std::wstring& subKey, DWORD* value) {
    if (FindOverride(subKey, value)) return true;  // The category the mod gives this key.
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) return false;
    const bool ok = ReadDword(key, L"Category", value);
    RegCloseKey(key);
    return ok;
}

// The main key decides what is shown. If it has no Category, the first alias that has one is used instead.
// Any alias that resolves to a different heading marks the app as conflicting.
void CheckRegistryCategory(AppEntry& entry) {
    HKEY root = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryPath, 0, KEY_READ, &root) != ERROR_SUCCESS) return;

    entry.hasCategory = ReadCategoryAt(root, entry.primaryRegistryKey, &entry.category);
    entry.conflict = false;

    for (const auto& alias : entry.aliasRegistryKeys) {
        DWORD other = 0;
        if (!ReadCategoryAt(root, alias, &other)) continue;
        if (!entry.hasCategory) {
            entry.hasCategory = true;
            entry.category = other;
        } else if (CategoryName(other, true) != CategoryName(entry.category, true)) {
            entry.conflict = true;
            if (g_conflictsLogged++ < 25) {
                Wh_Log(L"Keys disagree for %ls: %ls=%lu but %ls=%lu", entry.displayName.c_str(),
                       entry.primaryRegistryKey.c_str(), entry.category, alias.c_str(), other);
            }
        }
    }
    RegCloseKey(root);
}

// Used to recover Start menu folder grouping, and to find the per-shortcut keys
// ("W~<path of the .lnk>") that Start also keeps. The main registry key of each app comes
// from shell:AppsFolder (see ScanAppsFolder), which is what the Start menu itself uses.
void ScanFolder(const std::wstring& rootDir, const std::wstring& currentRelPath,
                std::vector<ShortcutInfo>& out, int depth) {
    if (depth > kMaxScanDepth) return;

    std::wstring dirPath = rootDir;
    if (!currentRelPath.empty()) {
        dirPath.push_back(L'\\');
        dirPath += currentRelPath;
    }

    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileW((dirPath + L"\\*").c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

        std::wstring itemRelPath = currentRelPath;
        if (!itemRelPath.empty()) itemRelPath.push_back(L'\\');
        itemRelPath += fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanFolder(rootDir, itemRelPath, out, depth + 1);
            continue;
        }

        const size_t len = wcslen(fd.cFileName);
        const bool isShortcut = len > 4 && (_wcsicmp(fd.cFileName + len - 4, L".lnk") == 0 ||
                                            _wcsicmp(fd.cFileName + len - 4, L".url") == 0);
        if (!isShortcut) continue;

        ShortcutInfo info;
        info.folder = currentRelPath;
        info.fullPath = dirPath + L"\\" + fd.cFileName;
        // The shell is asked for each shortcut's display name only once; a reload reuses the answer. Asking again
        // for several hundred files on every reload was slow and made Explorer's memory grow.
        static std::unordered_map<std::wstring, std::wstring> s_shortcutNames;
        auto cachedName = s_shortcutNames.find(info.fullPath);
        if (cachedName != s_shortcutNames.end()) {
            info.name = cachedName->second;
        } else {
            SHFILEINFOW sfi{};
            if (SHGetFileInfoW(info.fullPath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME)) {
                info.name = sfi.szDisplayName;
            }
            if (!info.name.empty()) s_shortcutNames.emplace(info.fullPath, info.name);
        }
        if (info.name.empty()) info.name.assign(fd.cFileName, len - 4);
        out.push_back(std::move(info));
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}

std::wstring GetItemString(IShellItem* item, SIGDN type) {
    std::wstring result;
    PWSTR str = nullptr;
    if (SUCCEEDED(item->GetDisplayName(type, &str)) && str) result = str;
    CoTaskMemFree(str);
    return result;
}

// Enumerates every app the Start menu knows about (Store apps and desktop apps).
void ScanAppsFolder(std::vector<AppEntry>& out) {
    IShellItem* folder = nullptr;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder))) || !folder) {
        Wh_Log(L"Could not open shell:AppsFolder");
        return;
    }

    IEnumShellItems* enumItems = nullptr;
    if (SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&enumItems))) && enumItems) {
        IShellItem* item = nullptr;
        ULONG fetched = 0;
        while (enumItems->Next(1, &item, &fetched) == S_OK && item) {
            AppEntry entry;
            entry.displayName = GetItemString(item, SIGDN_NORMALDISPLAY);
            const std::wstring parsing = GetItemString(item, SIGDN_PARENTRELATIVEPARSING);

            std::wstring aumid;
            IShellItem2* item2 = nullptr;
            if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&item2))) && item2) {
                PWSTR str = nullptr;
                if (SUCCEEDED(item2->GetString(kPkeyAppUserModelId, &str)) && str) aumid = str;
                CoTaskMemFree(str);
                item2->Release();
            }

            const std::wstring id = parsing.empty() ? aumid : parsing;
            if (!entry.displayName.empty() && !id.empty()) {
                // Packaged apps use "P~<AUMID>"; desktop apps use "W~<known-folder-GUID>\path".
                const bool packaged = id.find(L'!') != std::wstring::npos;
                entry.primaryRegistryKey = (packaged ? L"P~" : L"W~") + id;
                // Start sometimes also keeps the same app under the other prefix.
                entry.aliasRegistryKeys.push_back((packaged ? L"W~" : L"P~") + id);
                if (!aumid.empty() && aumid != id) {
                    entry.aliasRegistryKeys.push_back(L"P~" + aumid);
                }
                entry.relPath = Lower(id);

                // The system icon index of an app is looked up once; a reload reuses it. Asking the shell for the
                // icon of every app again on each reload is the slowest part of it, and each ask can make the
                // shell extract and cache the icon again.
                static std::unordered_map<std::wstring, int> s_iconIndexByApp;
                auto cachedIcon = s_iconIndexByApp.find(entry.relPath);
                if (cachedIcon != s_iconIndexByApp.end()) {
                    entry.iconIndex = cachedIcon->second;
                } else {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
                        SHFILEINFOW sfi{};
                        if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(pidl), 0, &sfi, sizeof(sfi),
                                           SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON)) {
                            entry.iconIndex = sfi.iIcon;
                            s_iconIndexByApp.emplace(entry.relPath, entry.iconIndex);
                        }
                        CoTaskMemFree(pidl);
                    }
                }

                // The registry is read later (LoadEntries), once shortcut keys are known.
                out.push_back(std::move(entry));
            }
            item->Release();
            item = nullptr;
        }
        enumItems->Release();
    }
    folder->Release();
}

void LoadEntries() {
    g_apps.clear();
    g_shortcuts.clear();
    g_conflictsLogged = 0;

    std::vector<AppEntry> found;
    ScanAppsFolder(found);

    // Recover Start menu folder grouping by matching display names to Start menu shortcuts.
    const std::wstring userPrograms = GetKnownFolder(FOLDERID_Programs);
    const std::wstring commonPrograms = GetKnownFolder(FOLDERID_CommonPrograms);
    if (!userPrograms.empty()) ScanFolder(userPrograms, L"", g_shortcuts, 0);
    if (!commonPrograms.empty()) ScanFolder(commonPrograms, L"", g_shortcuts, 0);

    std::unordered_map<std::wstring, std::vector<const ShortcutInfo*>> shortcutsByName;
    for (const auto& sc : g_shortcuts) {
        shortcutsByName[Lower(sc.name)].push_back(&sc);
    }

    std::unordered_set<std::wstring> seen;
    for (auto& e : found) {
        if (!seen.insert(e.relPath).second) continue;

        // Same name in two different folders is ambiguous: no grouping, no shortcut keys.
        // Same name in the same folder is just the per-user and all-users copy of one shortcut.
        auto it = shortcutsByName.find(Lower(e.displayName));
        if (it != shortcutsByName.end()) {
            const auto& matches = it->second;
            bool sameFolder = true;
            for (const ShortcutInfo* sc : matches) {
                if (CompareNoCase(sc->folder, matches[0]->folder) != CSTR_EQUAL) sameFolder = false;
            }
            if (sameFolder) {
                e.folderPath = matches[0]->folder;
                for (const ShortcutInfo* sc : matches) {
                    e.aliasRegistryKeys.push_back(L"W~" + sc->fullPath);
                }
            }
        }

        CheckRegistryCategory(e);
        g_apps.push_back(std::move(e));
    }

    // Normalise folder name casing (Windows folders are case-insensitive).
    std::unordered_map<std::wstring, std::wstring> canonical;
    for (auto& app : g_apps) {
        if (app.folderPath.empty()) continue;
        auto res = canonical.emplace(Lower(app.folderPath), app.folderPath);
        app.folderPath = res.first->second;
    }

    // A folder with a single app is not a group.
    std::unordered_map<std::wstring, int> folderCounts;
    for (const auto& app : g_apps) {
        if (!app.folderPath.empty()) folderCounts[app.folderPath]++;
    }
    for (auto& app : g_apps) {
        if (!app.folderPath.empty() && folderCounts[app.folderPath] < 2) {
            app.folderPath.clear();
        }
    }

    std::sort(g_apps.begin(), g_apps.end(), [](const AppEntry& a, const AppEntry& b) {
        const int c = CompareNoCase(a.folderPath, b.folderPath);
        if (c != CSTR_EQUAL) return c == CSTR_LESS_THAN;
        return CompareNoCase(a.displayName, b.displayName) == CSTR_LESS_THAN;
    });

    unsigned long disagreeing = 0;
    for (const auto& app : g_apps) {
        if (app.conflict) ++disagreeing;
    }
    Wh_Log(L"Loaded %lu apps, %lu Start menu shortcuts, %lu apps with disagreeing keys",
           static_cast<unsigned long>(g_apps.size()), static_cast<unsigned long>(g_shortcuts.size()),
           disagreeing);

    if (g_apps.empty()) {
        SetStatus(Tr(L"No apps found. Check the Windhawk log.", L"Nenhum app encontrado. Verifique o log do Windhawk."));
    } else {
        SetStatus(IsPortuguese()
            ? std::to_wstring(g_apps.size()) + L" aplicativos instalados carregados."
            : L"Loaded " + std::to_wstring(g_apps.size()) + L" installed applications.");
    }
}

// ---------------------------------------------------------------------------
// UI population
// ---------------------------------------------------------------------------


// How many apps each filter entry holds: All, Windows default, then one per kCategories entry.
// Uses the same rule as MatchesCategoryFilter, so a count equals what the filter then shows.
std::vector<size_t> ComputeFilterCounts() {
    std::vector<size_t> counts(2 + ARRAYSIZE(kCategories), 0);
    counts[0] = g_apps.size();
    for (const auto& app : g_apps) {
        if (!app.hasCategory) {
            ++counts[1];
            continue;
        }
        const std::wstring heading = CategoryNameEn(app.category, true);
        for (size_t i = 0; i < ARRAYSIZE(kCategories); ++i) {
            if (CategoryNameEn(kCategories[i].id, true) == heading) {
                ++counts[2 + i];
                break;
            }
        }
    }
    return counts;
}


void InitFolderIconIndex() {
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(L"folder", FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi),
                       SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES)) {
        g_folderIconIndex = sfi.iIcon;
    }
}


// ---------------------------------------------------------------------------
// Applying categories / Start menu restart
// ---------------------------------------------------------------------------

// The mod's storage folder (Reset all keeps its backups there). Windhawk removes it together with the mod. Empty if it
// cannot be found.
std::wstring ModStorageDirectory() {
    wchar_t path[MAX_PATH]{};
    if (Wh_GetModStoragePath(path, ARRAYSIZE(path)) == 0) return L"";
    CreateDirectoryW(path, nullptr);  // Harmless when it already exists.
    return path;
}

// Saves g_overrides to the mod's storage, where the copy of the mod inside Start reads it when Start starts.
bool SaveOverrides() {
    if (!Wh_SetStringValue(kOverridesValueName, SerializeOverrides(g_overrides).c_str())) {
        Wh_Log(L"Could not save the category overrides");
        return false;
    }
    return true;
}

// Start only reads a key that exists. For an app it has no key for yet, an empty key (no values) is created, so Start
// lists the app and asks for its category. This is the only thing the mod writes under TileProperties.
bool EnsureKeyExists(const std::wstring& regKey) {
    const std::wstring fullPath = std::wstring(kRegistryPath) + L"\\" + regKey;
    HKEY key = nullptr;
    const LSTATUS status = RegCreateKeyExW(HKEY_CURRENT_USER, fullPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                           KEY_QUERY_VALUE, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS) return false;
    RegCloseKey(key);
    return true;
}

enum class WriteResult { Failed, Unchanged, Changed };

// Sets the override of one key. categoryValue < 0 removes it, which gives the key back the category Windows chose.
WriteResult SetOverrideForKey(const std::wstring& regKey, LONG_PTR categoryValue) {
    const auto it = g_overrides.find(regKey);
    if (categoryValue < 0) {
        if (it == g_overrides.end()) return WriteResult::Unchanged;
        g_overrides.erase(it);
        return WriteResult::Changed;
    }
    const DWORD value = static_cast<DWORD>(categoryValue);
    if (it != g_overrides.end() && it->second == value) return WriteResult::Unchanged;
    g_overrides[regKey] = value;
    return WriteResult::Changed;
}

// The app's main key and every other key it is known by get the same override, so they can never disagree. The main key
// is created (empty) if Start has none yet. Nothing is saved here: ApplyCategoryToTargets saves once.
WriteResult WriteCategoryValueDual(const AppEntry& app, LONG_PTR categoryValue) {
    if (categoryValue >= 0 && !EnsureKeyExists(app.primaryRegistryKey)) return WriteResult::Failed;
    bool anyChanged = SetOverrideForKey(app.primaryRegistryKey, categoryValue) == WriteResult::Changed;
    for (const auto& alias : app.aliasRegistryKeys) {
        if (SetOverrideForKey(alias, categoryValue) == WriteResult::Changed) anyChanged = true;
    }
    return anyChanged ? WriteResult::Changed : WriteResult::Unchanged;
}

// When acting on a whole Start menu folder, also cover shortcuts in it that did not match an app (for example
// "Uninstall"). Returns true if any override changed.
bool WriteFolderShortcutKeys(const std::wstring& folder, LONG_PTR categoryValue) {
    if (folder.empty()) return false;
    bool anyChanged = false;
    for (const auto& sc : g_shortcuts) {
        if (CompareNoCase(sc.folder, folder) != CSTR_EQUAL) continue;
        if (SetOverrideForKey(L"W~" + sc.fullPath, categoryValue) == WriteResult::Changed) anyChanged = true;
    }
    return anyChanged;
}

enum class StartModState { Absent, Late, FromStart };

// Whether the process has this copy of the mod, and whether it has had it since it started. A copy loaded into a Start
// process that was already running comes after StartMenu.dll (or StartUI.dll) in the module list, which is in load
// order; Start has built its menus by then, so such a copy cannot add items to them. The DLL's file name carries the
// version, so an updated mod is a different file. If the module list cannot be read the answer is Absent.
StartModState ProcessModState(DWORD pid) {
    wchar_t selfPath[MAX_PATH]{};
    if (!GetModuleFileNameW(ModuleInstance(), selfPath, ARRAYSIZE(selfPath))) return StartModState::Absent;
    const wchar_t* selfName = PathFindFileNameW(selfPath);
    // The snapshot can fail with ERROR_BAD_LENGTH while the process is loading or unloading modules; retry a few times.
    HANDLE snapshot = INVALID_HANDLE_VALUE;
    DWORD snapshotError = ERROR_SUCCESS;
    for (int attempt = 0; attempt < 10; ++attempt) {
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snapshot != INVALID_HANDLE_VALUE) break;
        snapshotError = GetLastError();
        if (snapshotError != ERROR_BAD_LENGTH) break;
        Sleep(10);
    }
    if (snapshot == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Could not read the module list of Start (process %lu, error %lu); treating it as without this mod",
               pid, snapshotError);
        return StartModState::Absent;
    }
    StartModState state = StartModState::Absent;
    bool sawStartMenuDll = false;
    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Module32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szModule, L"StartMenu.dll") == 0 || _wcsicmp(entry.szModule, L"StartUI.dll") == 0) {
                sawStartMenuDll = true;
            } else if (_wcsicmp(entry.szModule, selfName) == 0) {
                state = sawStartMenuDll ? StartModState::Late : StartModState::FromStart;
                break;
            }
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return state;
}

// Ends the Start menu process so it starts again with the new registry values. A process that started within the last
// second is left alone: it was started by an earlier restart, has not read the layout yet, and killing it again would
// only make Start flap. Only the current session's Start process is touched. Calls from different threads run one
// after the other, so a request that arrives during a restart is not lost.
void RestartStartMenuHost(bool onlyIfWithoutMod = false) {
    static SRWLOCK s_lock = SRWLOCK_INIT;
    AcquireSRWLockExclusive(&s_lock);
    DWORD ownSession = 0;
    const bool haveSession = ProcessIdToSessionId(GetCurrentProcessId(), &ownSession) != 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        FILETIME nowFt{};
        GetSystemTimeAsFileTime(&nowFt);
        const ULONGLONG now = (static_cast<ULONGLONG>(nowFt.dwHighDateTime) << 32) | nowFt.dwLowDateTime;
        constexpr ULONGLONG kMinAge = 10000000ull;  // 1 s in 100 ns units.

        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, L"StartMenuExperienceHost.exe") != 0) continue;
                DWORD session = 0;
                if (haveSession && (!ProcessIdToSessionId(entry.th32ProcessID, &session) || session != ownSession)) continue;
                if (onlyIfWithoutMod) {
                    const StartModState modState = ProcessModState(entry.th32ProcessID);
                    if (modState == StartModState::FromStart) {
                        Wh_Log(L"Start (process %lu) has had this mod since it started; leaving it alone", entry.th32ProcessID);
                        continue;
                    }
                    Wh_Log(L"Start (process %lu) %ls; restarting it", entry.th32ProcessID,
                           modState == StartModState::Late ? L"got this mod only after it had started"
                                                           : L"does not have this mod");
                }
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                              entry.th32ProcessID);
                if (!hProcess) continue;
                FILETIME created{}, exited{}, kernel{}, user{};
                bool tooYoung = false;
                if (GetProcessTimes(hProcess, &created, &exited, &kernel, &user)) {
                    const ULONGLONG start = (static_cast<ULONGLONG>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
                    tooYoung = now > start && now - start < kMinAge;
                }
                if (!tooYoung) {
                    TerminateProcess(hProcess, 0);
                }
                CloseHandle(hProcess);
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    ReleaseSRWLockExclusive(&s_lock);
}

// Restarts the Start menu process when this mod loads (installed, updated or switched on again), but only if that
// process does not have this copy of the mod yet: a Start menu process without it has no "Manage Categories" item. One
// that already has it (Explorer was restarted, or the user signed in) is left alone. The new process starts with the
// hook in place. A process that started within the last second is also left alone (RestartStartMenuHost). Only
// Explorer's copy calls this: the Start menu copy would be killed by its own restart.
void RestartStartMenuStartedBeforeMod() {
    Wh_Log(L"Restarting the Start menu process if it does not have this mod yet");
    RestartStartMenuHost(true);
}

// Sets the category on every target app (kept as overrides, see "Category overrides"), saves them, re-reads what is
// shown now, and restarts Start only if something really changed.
std::wstring ApplyCategoryToTargets(const std::vector<size_t>& targets, const std::vector<std::wstring>& folders,
                                    LONG_PTR categoryValue, bool clearChecksOnSuccess) {
    const OverrideMap before = g_overrides;
    size_t changed = 0, unchanged = 0, failed = 0;
    for (size_t appIndex : targets) {
        const WriteResult result = WriteCategoryValueDual(g_apps[appIndex], categoryValue);
        if (result == WriteResult::Failed) {
            ++failed;
        } else if (result == WriteResult::Changed) {
            ++changed;
        } else {
            ++unchanged;
        }
    }

    bool folderChanged = false;
    if (changed + unchanged > 0) {
        for (const auto& f : folders) {
            if (WriteFolderShortcutKeys(f, categoryValue)) folderChanged = true;
        }
    }

    bool saveFailed = false;
    if (changed > 0 || folderChanged) {
        if (!SaveOverrides()) {
            saveFailed = true;
            g_overrides = before;  // What the list shows must match what is stored.
        }
    }
    for (size_t appIndex : targets) CheckRegistryCategory(g_apps[appIndex]);

    const bool restarted = !saveFailed && (changed > 0 || folderChanged);
    if (restarted) RestartStartMenuHost();
    if (clearChecksOnSuccess && !saveFailed && changed + unchanged > 0) {
        for (auto& app : g_apps) app.checked = false;
    }

    std::wstring msg;
    if (IsPortuguese()) {
        msg = std::to_wstring(saveFailed ? 0 : changed) + L" item(ns) atualizado(s)";
        if (unchanged > 0) msg += L"; " + std::to_wstring(unchanged) + L" sem alteração";
        if (restarted) msg += L"; Menu Iniciar reiniciado";
        if (saveFailed) msg += L"; não foi possível salvar as alterações";
        if (failed > 0) msg += L"; " + std::to_wstring(failed) + L" falha(s)";
    } else {
        msg = L"Updated " + std::to_wstring(saveFailed ? 0 : changed) + L" item(s)";
        if (unchanged > 0) msg += L"; " + std::to_wstring(unchanged) + L" already set";
        if (restarted) msg += L"; Start menu restarted";
        if (saveFailed) msg += L"; the changes could not be saved";
        if (failed > 0) msg += L"; " + std::to_wstring(failed) + L" failed";
    }
    SetStatus(msg + L".");
    return msg + L".";
}


// ---------------------------------------------------------------------------
// Reset all, Export and Import
// ---------------------------------------------------------------------------

// Dialogs. Each action asks first. The question is asked without waiting inside the button's click handler (a nested
// message loop there crashed Explorer): ConfirmAsync returns at once and runs onYes later, if the answer is "Yes".
// The dialog itself is drawn by ConfirmAsync, further down. With infoOnly it is a notice with a single OK button.
void ConfirmAsync(const wchar_t* titleEn, const wchar_t* titlePt, const wchar_t* en, const wchar_t* pt, UINT icon,
                  std::function<void()> onYes, bool infoOnly = false);

// The folder for the backups Reset all makes. Empty if it cannot be made.
std::wstring LayoutDirectory() { return ModStorageDirectory(); }

// Writes the current layout to a text file (the same format Export uses) so a reset can be undone
// with Import. Returns the file's path, or an empty string if it could not be written.
// Keeps only the newest few Reset all backups, so they do not pile up.
void PruneLayoutBackups(const std::wstring& dir) {
    constexpr size_t kKeep = 5;
    std::vector<std::wstring> names;
    WIN32_FIND_DATAW fd{};
    HANDLE find = FindFirstFileW((dir + L"\\backup-before-reset-*.txt").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        names.push_back(fd.cFileName);
    } while (FindNextFileW(find, &fd));
    FindClose(find);
    // The names carry the date and time, so sorting them puts the oldest first.
    std::sort(names.begin(), names.end());
    for (size_t i = 0; i + kKeep < names.size(); ++i) {
        DeleteFileW((dir + L"\\" + names[i]).c_str());
    }
}

// The text of a layout file (the format Export writes): the category changes the mod applies.
std::wstring LayoutFileText() {
    std::wstring text = L"# Start Menu Category Manager export\r\n# <key><TAB><category id>\r\n";
    for (const auto& item : g_overrides) text += item.first + L"\t" + std::to_wstring(item.second) + L"\r\n";
    return text;
}

std::wstring SaveLayoutBackup() {
    const std::wstring dir = LayoutDirectory();
    if (dir.empty()) return L"";

    SYSTEMTIME now{};
    GetLocalTime(&now);
    auto two = [](int v) { return (v < 10 ? std::wstring(L"0") : std::wstring()) + std::to_wstring(v); };
    const std::wstring file = dir + L"\\backup-before-reset-" + std::to_wstring(now.wYear) + two(now.wMonth) +
                              two(now.wDay) + L"-" + two(now.wHour) + two(now.wMinute) + two(now.wSecond) + L".txt";

    if (!WriteTextFile(file, LayoutFileText())) return std::wstring();
    PruneLayoutBackups(dir);
    return file;
}

void ResetAllCategoriesNow() {
    if (g_overrides.empty()) {
        SetStatus(Tr(L"Nothing to reset: the mod has not changed any category.",
                     L"Nada para redefinir: o mod ainda não alterou nenhuma categoria."));
        return;
    }
    const std::wstring backup = SaveLayoutBackup();
    if (backup.empty()) {
        SetStatus(Tr(L"Nothing was changed: the backup could not be saved.",
                     L"Nada foi alterado: não foi possível salvar o backup."));
        return;
    }
    const OverrideMap previous = g_overrides;
    g_overrides.clear();
    if (!SaveOverrides()) {
        g_overrides = previous;
        SetStatus(Tr(L"Nothing was changed: the layout could not be saved.",
                     L"Nada foi alterado: não foi possível salvar o layout."));
        return;
    }
    RestartStartMenuHost();
    LoadEntries();
    const std::wstring where = std::wstring(L" Backup: ") + backup;
    SetStatus(IsPortuguese() ? std::to_wstring(previous.size()) + L" alterações removidas; Menu Iniciar reiniciado." + where
                             : std::to_wstring(previous.size()) + L" changes removed; Start menu restarted." + where);
}

void ResetAllCategories(std::function<void()> onDone) {
    ConfirmAsync(L"Reset all", L"Redefinir tudo",
            L"Remove every category change the mod applies, so every app goes back to the category Windows chose?\n\n"
            L"A copy of your current layout is saved first, and the Start menu restarts.",
            L"Remover todas as alterações de categoria aplicadas pelo mod, para que todos os apps voltem à "
            L"categoria escolhida pelo Windows?\n\nUma cópia do layout atual é salva antes, e o Menu Iniciar reinicia.",
            MB_ICONWARNING,
                 [onDone]() {
                     ResetAllCategoriesNow();
                     if (onDone) onDone();
                 });
}

void ExportCategoriesNow() {
    HWND owner = g_xamlListWindow;
    wchar_t file[MAX_PATH] = L"StartMenuCategories.txt";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    if (!GetSaveFileNameW(&ofn)) return;

    if (!WriteTextFile(file, LayoutFileText())) {
        SetStatus(Tr(L"Could not write the file.", L"Não foi possível gravar o arquivo."));
        return;
    }
    SetStatus(IsPortuguese() ? std::to_wstring(g_overrides.size()) + L" entradas exportadas."
                             : std::to_wstring(g_overrides.size()) + L" entries exported.");
}

void ExportCategories(std::function<void()> onDone) {
    ConfirmAsync(L"Export", L"Exportar",
            L"Export the category changes the mod applies to a text file?\n\nThe file lists every change, including "
            L"apps that are not installed. Apps you did not change are not included.",
            L"Exportar para um arquivo de texto as alterações de categoria que o mod aplica?\n\nO arquivo lista "
            L"todas as alterações, inclusive de apps que não estão instalados. Apps que você não "
            L"alterou não são incluídos.",
            MB_ICONQUESTION,
                 [onDone]() {
                     ExportCategoriesNow();
                     if (onDone) onDone();
                 });
}

// A key path from an import file: non-empty segments separated by backslashes, no "." or ".." segments.
bool IsValidRelativeKey(const std::wstring& key) {
    if (key.empty() || key.size() > 2000 || key.front() == L'\\' || key.back() == L'\\') return false;
    size_t start = 0;
    while (start <= key.size()) {
        size_t end = key.find(L'\\', start);
        if (end == std::wstring::npos) end = key.size();
        const std::wstring segment = key.substr(start, end - start);
        if (segment.empty() || segment == L"." || segment == L"..") return false;
        start = end + 1;
    }
    return true;
}

// Applies the lines of a layout file (the format Export writes) as overrides. Returns how many entries were applied.
// skipped counts the lines that were invalid.
size_t ApplyLayoutText(const std::wstring& text, size_t* skipped) {
    size_t applied = 0;
    size_t skippedLines = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'\n', start);
        if (end == std::wstring::npos) end = text.size();
        std::wstring line = text.substr(start, end - start);
        start = end + 1;
        while (!line.empty() && (line.back() == L'\r' || line.back() == L' ')) line.pop_back();
        if (line.empty() || line[0] == L'#') continue;

        const size_t tab = line.rfind(L'\t');
        if (tab == std::wstring::npos || tab == 0 || !IsValidRelativeKey(line.substr(0, tab))) {
            ++skippedLines;
            continue;
        }
        const std::wstring key = line.substr(0, tab);
        const std::wstring value = line.substr(tab + 1);
        wchar_t* parseEnd = nullptr;
        const unsigned long category = wcstoul(value.c_str(), &parseEnd, 10);
        if (value.empty() || (parseEnd && *parseEnd != L'\0') || category > kMaxCustomId) {
            ++skippedLines;
            continue;
        }
        g_overrides[key] = static_cast<DWORD>(category);
        ++applied;
    }
    if (skipped) *skipped = skippedLines;
    return applied;
}

// Start only reads keys that exist, so an imported category needs the app's main key. Creates the (empty) ones that are
// missing for installed apps.
void EnsurePrimaryKeys() {
    for (const auto& app : g_apps) {
        if (!app.primaryRegistryKey.empty() && g_overrides.count(app.primaryRegistryKey)) {
            EnsureKeyExists(app.primaryRegistryKey);
        }
    }
}

void ImportCategoriesNow() {
    HWND owner = g_xamlListWindow;
    wchar_t file[MAX_PATH]{};
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    if (!GetOpenFileNameW(&ofn)) return;

    std::wstring text;
    if (!ReadTextFile(file, text)) {
        SetStatus(Tr(L"Could not read the file.", L"Não foi possível ler o arquivo."));
        return;
    }

    const OverrideMap previous = g_overrides;
    size_t skipped = 0;
    const size_t applied = ApplyLayoutText(text, &skipped);
    if (applied > 0 && !SaveOverrides()) {
        g_overrides = previous;
        SetStatus(Tr(L"Nothing was imported: the layout could not be saved.",
                     L"Nada foi importado: não foi possível salvar o layout."));
        return;
    }
    if (applied > 0) {
        EnsurePrimaryKeys();
        RestartStartMenuHost();
    }
    LoadEntries();
    SetStatus(IsPortuguese()
        ? std::to_wstring(applied) + L" entradas importadas, " + std::to_wstring(skipped) + L" ignoradas" +
              (applied > 0 ? L"; Menu Iniciar reiniciado." : L".")
        : std::to_wstring(applied) + L" entries imported, " + std::to_wstring(skipped) + L" skipped" +
              (applied > 0 ? L"; Start menu restarted." : L"."));
}

void ImportCategories(std::function<void()> onDone) {
    ConfirmAsync(L"Import", L"Importar",
            L"Import the categories from this file?\n\nThe file is merged into your current settings: the apps listed "
            L"in it are assigned, and everything else is left as it is.",
            L"Importar as categorias deste arquivo?\n\nO arquivo é mesclado às suas configurações atuais: os apps listados "
            L"nele são atribuídos, e o restante fica como está.",
            MB_ICONWARNING,
                 [onDone]() {
                     ImportCategoriesNow();
                     if (onDone) onDone();
                 });
}

// Dark mode: the title bar, and the controls' themes and colours.
void ApplyDarkTitleBar(HWND hwnd) {
    // Set explicitly in both modes: the acrylic backdrop behind the window follows this attribute.
    BOOL on = g_isDark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, 20, &on, sizeof(on));  // DWMWA_USE_IMMERSIVE_DARK_MODE
}


// ---------------------------------------------------------------------------
// XAML island hosting
// ---------------------------------------------------------------------------

namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

// Lets a Win32 window host XAML content. Declared here because the SDK header is not always present.
MIDL_INTERFACE("3CBCF1BF-2F76-4E9C-96AB-E84B37972554")
IDesktopWindowXamlSourceNative : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE AttachToWindow(HWND parentWindow) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* hwnd) = 0;
};

// Makes the XAML islands on the current thread transparent. By default a system XAML island paints an opaque
// background, so nothing behind it (such as the window's acrylic) can show. Declared here as it is not in the SDK.
MIDL_INTERFACE("06636c29-5a17-458d-8ea2-2422d997a922")
IXamlSourceTransparencyNative : public IUnknown {
    // The three IInspectable methods, spelled out so the vtable layout is right without the global IInspectable.
    virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(void** className) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(int* trustLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsBackgroundTransparent(BOOLEAN* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBackgroundTransparent(BOOLEAN value) = 0;
};

// Affects every island on the calling thread only, so call it on the manager's own thread.
void MakeXamlIslandsTransparent() {
    try {
        auto window = winrt::Windows::UI::Xaml::Window::Current();
        if (!window) {
            Wh_Log(L"[xaml] transparent islands: there is no XAML window on this thread");
            return;
        }
        const IID iid = {0x06636c29, 0x5a17, 0x458d, {0x8e, 0xa2, 0x24, 0x22, 0xd9, 0x97, 0xa9, 0x22}};
        IXamlSourceTransparencyNative* transparency = nullptr;
        HRESULT hr = winrt::get_unknown(window)->QueryInterface(iid, reinterpret_cast<void**>(&transparency));
        if (SUCCEEDED(hr) && transparency) {
            hr = transparency->put_IsBackgroundTransparent(TRUE);
            transparency->Release();
        }
        if (FAILED(hr)) Wh_Log(L"[xaml] could not make the XAML islands transparent: 0x%08X", static_cast<unsigned>(hr));
    } catch (...) {
        Wh_Log(L"[xaml] transparent islands: exception");
    }
}

[[clang::no_destroy]] wuxh::WindowsXamlManager g_xamlManager{nullptr};


// ---------------------------------------------------------------------------
// The manager window: a XAML TreeView of the app list hosted in a Win32 window
// ---------------------------------------------------------------------------

[[clang::no_destroy]] wuxh::DesktopWindowXamlSource g_xamlTreeSource{nullptr};
HWND g_xamlTreeChild = nullptr;
std::wstring g_xamlTreeClass = L"WindhawkStartCategoryManager.Window";
bool g_xamlTreeClassRegistered = false;
HICON g_classIcon = nullptr;  // The window class icons; destroyed after the class is unregistered.
HICON g_classIconSmall = nullptr;
void ReleaseClassIcons() {
    if (g_classIcon) DestroyIcon(g_classIcon);
    if (g_classIconSmall) DestroyIcon(g_classIconSmall);
    g_classIcon = g_classIconSmall = nullptr;
}
[[clang::no_destroy]] wuxc::TextBlock g_xamlStatus{nullptr};
// True from the moment the manager window starts closing until the next one is created. XAML events that
// still arrive in that gap must not touch the old window's controls.
bool g_xamlClosing = false;

void SetXamlStatus(const std::wstring& text) {
    if (g_xamlStatus) g_xamlStatus.Text(text);
}

[[clang::no_destroy]] wuxc::TreeView g_xamlTree{nullptr};
[[clang::no_destroy]] wuxc::TextBox g_xamlSearch{nullptr};
[[clang::no_destroy]] wuxc::ComboBox g_xamlFilter{nullptr};
[[clang::no_destroy]] wuxc::ComboBox g_xamlSort{nullptr};
[[clang::no_destroy]] wuxc::CheckBox g_xamlCheckAll{nullptr};
[[clang::no_destroy]] wuxc::Button g_xamlExpand{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_xamlBar{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_xamlSearchTimer{nullptr};
std::wstring g_xamlSearchText;
std::vector<size_t> g_xamlFilterCounts;
bool g_xamlExpanded = true;
std::map<std::wstring, bool> g_folderExpanded;  // Folders the user opened or closed one by one; the rest follow g_xamlExpanded.
bool g_xamlBusy = false;  // Set while the filter drop-down is being refilled.
constexpr int kXamlFilterAll = -100;
[[clang::no_destroy]] wuxc::Button g_xamlBulkPicker{nullptr};
[[clang::no_destroy]] wuxc::TextBox g_xamlCustomId{nullptr};

// Set to false to turn system icons off. The XAML tree then shows the placeholder glyphs.
constexpr bool kShowSystemIcons = true;

// The system's large icon list (32 px). Its indices match the small list, so the stored indices work with it.
HIMAGELIST SystemLargeImageList() {
    static HIMAGELIST himl = nullptr;
    if (!himl) {
        HIMAGELIST large = nullptr;
        HIMAGELIST small = nullptr;
        if (Shell_GetImageLists(&large, &small)) himl = large;
    }
    return himl;
}

// Turns an icon into a bitmap XAML can show, and destroys the icon. Returns null when that is not possible.
wuxm::ImageSource MakeIconSourceFromHicon(HICON icon) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info) || !info.hbmColor) {
        DestroyIcon(icon);
        return nullptr;
    }
    BITMAP bm{};
    GetObjectW(info.hbmColor, sizeof(bm), &bm);
    const int w = bm.bmWidth;
    const int h = bm.bmHeight;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // Top-down rows.
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    std::vector<BYTE> pixels(static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
    HDC dc = GetDC(nullptr);
    GetDIBits(dc, info.hbmColor, 0, h, pixels.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    DestroyIcon(icon);

    // Icons without an alpha channel would come out fully transparent: treat them as opaque.
    bool anyAlpha = false;
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p] != 0) {
            anyAlpha = true;
            break;
        }
    }
    for (size_t p = 0; p < pixels.size(); p += 4) {
        if (!anyAlpha) pixels[p + 3] = 255;
        const unsigned a = pixels[p + 3];
        pixels[p] = static_cast<BYTE>(pixels[p] * a / 255);      // Premultiplied alpha.
        pixels[p + 1] = static_cast<BYTE>(pixels[p + 1] * a / 255);
        pixels[p + 2] = static_cast<BYTE>(pixels[p + 2] * a / 255);
    }

    // DataWriter fills the buffer through the documented API, so no raw pointers are written.
    winrt::Windows::Storage::Streams::DataWriter writer;
    writer.WriteBytes(winrt::array_view<uint8_t const>(pixels.data(), pixels.data() + pixels.size()));
    winrt::Windows::Storage::Streams::IBuffer buffer = writer.DetachBuffer();
    winrt::Windows::Graphics::Imaging::SoftwareBitmap bitmap =
        winrt::Windows::Graphics::Imaging::SoftwareBitmap::CreateCopyFromBuffer(
            buffer, winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8, w, h,
            winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied);

    wuxm::Imaging::SoftwareBitmapSource source;
    source.SetBitmapAsync(bitmap);  // Completes in the background; the image appears when it is ready.
    return source;
}

// Turns a system icon into a bitmap XAML can show. Returns null when that is not possible.
// One bitmap source per system icon index, shared by every row that shows it and kept across rebuilds of the list.
// Cleared when the manager window closes, because the sources belong to that window's XAML thread.
[[clang::no_destroy]] std::map<int, wuxm::ImageSource> g_iconCache;

wuxm::ImageSource MakeSystemIconSource(int iconIndex) {
    if (!kShowSystemIcons) return nullptr;
    HIMAGELIST himl = SystemLargeImageList();
    if (!himl || iconIndex < 0) return nullptr;
    auto cached = g_iconCache.find(iconIndex);
    if (cached != g_iconCache.end()) return cached->second;
    HICON icon = ImageList_GetIcon(himl, iconIndex, ILD_TRANSPARENT);
    if (!icon) return nullptr;
    wuxm::ImageSource source = MakeIconSourceFromHicon(icon);
    if (source) g_iconCache.insert_or_assign(iconIndex, source);
    return source;
}

HICON CreateCogIcon(int size, COLORREF color);
HICON CreateSettingsAppIcon(int size);
COLORREF TaskbarIconColor();

// The yellow "*" after the name of an app whose Start keys disagree (see AppEntry::conflict).
wuxm::SolidColorBrush ConflictStarBrush() {
    return wuxm::SolidColorBrush(g_isDark ? winrt::Windows::UI::Color{255, 252, 225, 0}
                                          : winrt::Windows::UI::Color{255, 200, 140, 0});
}

void SetConflictTip(wux::DependencyObject const& element) {
    wuxc::ToolTipService::SetToolTip(
        element, winrt::box_value(winrt::hstring(Tr(
            L"Some of this app's Start keys hold different categories. Choosing a category here sets all of them.",
            L"Algumas chaves do Menu Iniciar deste aplicativo t\u00eam categorias diferentes. Escolher uma categoria aqui "
            L"define todas elas."))));
}

// The status bar text for an app: the registry key it uses (below TileProperties) and how many other keys it has.
std::wstring AppKeyStatus(const AppEntry& app) {
    std::wstring text =
        std::wstring(Tr(L"Registry key: ", L"Chave do registro: ")) + L"TileProperties\\" + app.primaryRegistryKey;
    const size_t others = app.aliasRegistryKeys.size();
    if (others > 0) {
        text += IsPortuguese()
                    ? (others == 1 ? std::wstring(L" (+1 outra chave)")
                                   : L" (+" + std::to_wstring(others) + L" outras chaves)")
                    : (others == 1 ? std::wstring(L" (+1 other key)")
                                   : L" (+" + std::to_wstring(others) + L" other keys)");
    }
    return text;
}

// A Start folder has no key of its own (each shortcut inside has one), so say that instead.
std::wstring FolderKeyStatus(const std::wstring& folder) {
    size_t apps = 0;
    for (const auto& app : g_apps) {
        if (app.folderPath == folder) ++apps;
    }
    return IsPortuguese()
               ? L"A pasta \"" + folder + L"\" tem " + std::to_wstring(apps) +
                     L" aplicativo(s); cada um tem a sua chave. Clique em um aplicativo para ver."
               : L"Folder \"" + folder + L"\" holds " + std::to_wstring(apps) +
                     L" app(s); each has its own key. Click an app to see it.";
}

// One tree row: a check box, an icon and the name. The category picker is added by the caller. conflict adds a yellow
// "*" after the name (the row's fourth child; UpdateRowWidths leaves room for it).
wuxc::StackPanel MakeTreeRow(const std::wstring& name, const wchar_t* glyph, bool checked, int iconIndex, int iconSize,
                             bool conflict = false) {
    wuxc::StackPanel row;
    row.Orientation(wuxc::Orientation::Horizontal);
    row.Spacing(8);

    wuxc::CheckBox check;
    check.IsChecked(checked);
    check.VerticalAlignment(wux::VerticalAlignment::Center);
    check.MinWidth(0);  // The default (about 120) reserves an empty label area, which pushed the icon away.

    // The real system icon when it can be shown, otherwise the placeholder glyph.
    wux::UIElement iconElement{nullptr};
    if (auto source = MakeSystemIconSource(iconIndex)) {
        wuxc::Image image;
        image.Source(source);
        image.Width(iconSize);
        image.Height(iconSize);
        image.VerticalAlignment(wux::VerticalAlignment::Center);
        iconElement = image;
    } else {
        wuxc::FontIcon icon;
        icon.Glyph(glyph);
        icon.FontSize(iconSize);
        icon.VerticalAlignment(wux::VerticalAlignment::Center);
        iconElement = icon;
    }

    wuxc::TextBlock nameText;
    nameText.Text(name);
    nameText.VerticalAlignment(wux::VerticalAlignment::Center);
    nameText.TextTrimming(wux::TextTrimming::CharacterEllipsis);  // Long names end in "..." (see UpdateRowWidths).

    row.Children().Append(check);
    row.Children().Append(iconElement);
    row.Children().Append(nameText);
    if (conflict) {
        wuxc::TextBlock star;
        star.Text(L"*");
        star.VerticalAlignment(wux::VerticalAlignment::Center);
        star.Margin(wux::Thickness{-6, 0, 0, 0});  // Closes most of the row's 8 px gap, so it sits right after the name.
        star.Foreground(ConflictStarBrush());
        SetConflictTip(star);
        SetConflictTip(nameText);
        row.Children().Append(star);
    }
    return row;
}

// The category drop-down of a row. Its items: "Windows default" first, then the categories.
// Every drop-down has this width, so they line up. Set from the longest name before the rows are built.
double g_pickerWidth = 260.0;

// The width of a text as the font draws it. Falls back to a rough estimate if the text cannot be measured.
double MeasureText(std::wstring const& text) {
    wuxc::TextBlock block;
    block.Text(text);
    block.Measure(wf::Size{100000.0f, 100000.0f});
    const double width = block.DesiredSize().Width;
    return width > 0 ? width : static_cast<double>(text.size()) * 7.0;
}

// The width every drop-down gets: the longest name in the list, plus the button's padding.
double ComputePickerWidth() {
    double widest = MeasureText(Tr(L"Windows default", L"Padr\u00e3o do Windows"));
    for (const auto& cat : kCategories) {
        widest = std::max(widest, MeasureText(CategoryName(static_cast<DWORD>(cat.id), true) + L"*"));
    }
    return widest + 24.0;
}

// True when the category is one of the known categories in the list.
bool IsListedCategory(DWORD id) {
    for (const auto& cat : kCategories) {
        if (static_cast<DWORD>(cat.id) == id) return true;
    }
    return false;
}

// The indices of kCategories ordered by the name shown in the current language, for the drop-down lists.
std::vector<size_t> SortedCategoryOrder() {
    std::vector<size_t> order;
    for (size_t i = 0; i < ARRAYSIZE(kCategories); ++i) order.push_back(i);
    const wchar_t* locale = IsPortuguese() ? L"pt-BR" : L"en-US";
    std::sort(order.begin(), order.end(), [locale](size_t a, size_t b) {
        const std::wstring nameA = CategoryName(static_cast<DWORD>(kCategories[a].id), true);
        const std::wstring nameB = CategoryName(static_cast<DWORD>(kCategories[b].id), true);
        return CompareStringEx(locale, LINGUISTIC_IGNORECASE, nameA.c_str(), -1, nameB.c_str(), -1, nullptr, nullptr,
                               0) == CSTR_LESS_THAN;
    });
    return order;
}

// A custom ID whose name matches a known category maps to that category (for example ID 13 is "Produtividade",
// the same as ID 5). Returns -1 when nothing matches.
int MappedKnownId(DWORD id) {
    const std::wstring name = CategoryName(id, true);
    for (const auto& cat : kCategories) {
        if (CategoryName(static_cast<DWORD>(cat.id), true) == name) return static_cast<int>(cat.id);
    }
    return -1;
}

// Start shows a heading as its own tile only when at least 3 apps are assigned to it, and it counts a Start folder
// (a group of shortcuts) as one app. A heading with 1 or 2 is written in red with a * after its name, everywhere.
constexpr size_t kMinAppsPerHeading = 3;
std::map<std::wstring, size_t> g_headingCounts;  // Apps per heading, keyed by the English name.

void RefreshHeadingCounts() {
    g_headingCounts.clear();
    std::set<std::pair<std::wstring, std::wstring>> seenFolders;
    for (const auto& app : g_apps) {
        if (!app.hasCategory) continue;
        const std::wstring heading = CategoryNameEn(app.category, true);
        if (!app.folderPath.empty() && !seenFolders.insert({heading, app.folderPath}).second) continue;
        ++g_headingCounts[heading];
    }
}

// True when 1 or 2 apps are assigned to the heading of this ID. A heading with no apps is not marked, and "Other" is
// where the rest goes, so it is never marked.
bool IsLowHeading(DWORD id) {
    const std::wstring heading = CategoryNameEn(id, true);
    if (heading == L"Other") return false;
    const auto found = g_headingCounts.find(heading);
    const size_t count = found == g_headingCounts.end() ? size_t{0} : found->second;
    return count >= 1 && count < kMinAppsPerHeading;
}

// The heading's name, with a * after it when it is marked.
std::wstring HeadingName(DWORD id) {
    return CategoryName(id, true) + (IsLowHeading(id) ? L"*" : L"");
}

wuxm::SolidColorBrush LowHeadingBrush() {
    return wuxm::SolidColorBrush(g_isDark ? winrt::Windows::UI::Color{255, 255, 99, 99}
                                          : winrt::Windows::UI::Color{255, 196, 43, 28});
}

// The headings that are marked right now (1 or 2 apps assigned).
std::set<std::wstring> CurrentLowSet() {
    std::set<std::wstring> low;
    for (const auto& entry : g_headingCounts) {
        if (entry.first != L"Other" && entry.second >= 1 && entry.second < kMinAppsPerHeading) low.insert(entry.first);
    }
    return low;
}
std::set<std::wstring> g_xamlFilterLowSet;  // The marked headings the filter list was last filled with.

// Puts the text in a text block, with the "*" of a marked heading in red and the rest in the normal colour.
void FillMarkedText(wuxc::TextBlock const& block, std::wstring const& text) {
    const size_t star = text.find(L'*');
    if (star == std::wstring::npos) {
        block.Text(text);
        return;
    }
    block.Inlines().Clear();
    auto add = [&block](std::wstring const& part, bool red) {
        if (part.empty()) return;
        winrt::Windows::UI::Xaml::Documents::Run run;
        run.Text(part);
        if (red) run.Foreground(LowHeadingBrush());
        block.Inlines().Append(run);
    };
    add(text.substr(0, star), false);
    add(L"*", true);
    add(text.substr(star + 1), false);
}

// Menu items and combo box entries only take plain text, so their "*" is coloured after they are on screen, by
// splitting the text of the text blocks inside them.
void ColorStarInside(wux::DependencyObject const& root) {
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto block = child.try_as<wuxc::TextBlock>()) {
            const std::wstring text = block.Text().c_str();
            if (text.find(L'*') != std::wstring::npos && block.Inlines().Size() <= 1) FillMarkedText(block, text);
        } else {
            ColorStarInside(child);
        }
    }
}

// The tip on a marked heading (and nothing on the others).
void SetMarkTip(wux::DependencyObject const& element, bool low) {
    if (low) {
        wuxc::ToolTipService::SetToolTip(
            element, winrt::box_value(winrt::hstring(Tr(
                L"Categories with fewer than 3 apps aren't shown on their own tile in the Start menu. A folder counts as one app.",
                L"Categorias com menos de 3 aplicativos n\u00e3o s\u00e3o exibidas em um bloco pr\u00f3prio no Menu Iniciar. Uma pasta conta como um aplicativo."))));
    } else {
        wuxc::ToolTipService::SetToolTip(element, nullptr);
    }
}

// The option the mouse wheel moves a drop-down to: its entries in the order of its list (Windows default, then the
// categories by name), wrapping around. step is -1 for wheel up and +1 for wheel down.
int NextPickerTag(int current, int step) {
    std::vector<int> options{-1};
    for (const size_t index : SortedCategoryOrder()) options.push_back(static_cast<int>(kCategories[index].id));
    int look = current;
    if (current >= 0 && !IsListedCategory(static_cast<DWORD>(current))) {
        const int mapped = MappedKnownId(static_cast<DWORD>(current));
        if (mapped >= 0) {
            look = mapped;
        } else {
            options.push_back(current);  // A custom ID with no known heading is the last entry, as in the list.
        }
    }
    size_t position = 0;
    for (size_t i = 0; i < options.size(); ++i) {
        if (options[i] == look) {
            position = i;
            break;
        }
    }
    const int count = static_cast<int>(options.size());
    return options[(static_cast<int>(position) + step + count) % count];
}

// The box text of a drop-down: the name. A custom ID adds "(Personalizado)".
std::wstring PickerLabel(int tag) {
    if (tag < 0) return Tr(L"Windows default", L"Padr\u00e3o do Windows");
    const DWORD id = static_cast<DWORD>(tag);
    if (IsListedCategory(id)) return HeadingName(id);
    return HeadingName(id) + L" (" + Tr(L"Custom", L"Personalizado") + L")";
}

// Sets a drop-down's value and the text in its box. The "*" of a marked heading is red (it is a text run of its own,
// so hovering or pressing the button does not change it), and the box gets a tip that says what the mark means.
void SetPickerValue(wuxc::Button const& button, int tag) {
    button.Tag(winrt::box_value(tag));
    const bool low = tag >= 0 && IsLowHeading(static_cast<DWORD>(tag));
    if (low) {
        wuxc::TextBlock block;
        FillMarkedText(block, PickerLabel(tag));
        button.Content(block);
    } else {
        button.Content(winrt::box_value(PickerLabel(tag)));
    }
    SetMarkTip(button, low);
}

// The value a drop-down holds (-1 is "Windows default").
int PickerTag(wuxc::Button const& button) {
    auto tag = button.Tag();
    return tag ? winrt::unbox_value<int>(tag) : -1;
}

// A drop-down: a button showing the name. Its flyout lists "Name (ID n)", then a separator and the ID prompt.
// Defined further down; the drop-down handlers below log their failures through it.
void LogXamlHandlerFailure(const wchar_t* name);

// Marks "nothing highlighted" (used for a folder whose apps differ).
constexpr int kNoneHighlighted = -99;

// The wheel state of one drop-down: the value it was last scrolled to, and the timer that applies it.
struct PickerWheel {
    wux::DispatcherTimer timer{nullptr};
    int tag = -1;
};

// With debounceWheel, scrolling the wheel over the button shows each option at once but only picks the last one
// after the wheel has rested for 0.7 s (picking a row's category restarts the Start menu, which must not happen on
// every notch).
wuxc::Button MakePicker(bool hasCategory, DWORD category, bool mixed,
                        std::function<void(int)> onPick,
                        std::function<void(wuxc::Button const&)> onPrompt,
                        bool debounceWheel = true) {
    wuxc::Button button;
    button.Width(g_pickerWidth);
    button.VerticalAlignment(wux::VerticalAlignment::Center);
    button.HorizontalContentAlignment(wux::HorizontalAlignment::Left);
    SetPickerValue(button, (!mixed && hasCategory) ? static_cast<int>(category) : -1);
    if (mixed) {
        button.Content(winrt::box_value(std::wstring(Tr(L"Mixed", L"Misto"))));  // The folder's apps differ.
        SetMarkTip(button, false);
    }

    // The entry to highlight. A custom ID highlights the known category it maps to, or its own entry if none matches.
    const bool custom = !mixed && hasCategory && !IsListedCategory(category);
    const int mapped = custom ? MappedKnownId(category) : -1;
    const int highlighted = mixed ? kNoneHighlighted
                                  : (mapped >= 0 ? mapped : (hasCategory ? static_cast<int>(category) : -1));

    // The drop-down list is built only when the button is clicked, and thrown away when it closes. Building it
    // for every row up front meant about 15 XAML objects per row (over 2000 for 156 apps) on every rebuild of
    // the list, which is what made Explorer's memory creep up on Expand all, search and reload. The button does
    // not own a flyout, so the handlers below hold the button only weakly and nothing can form a cycle.
    auto weakButton = winrt::make_weak(button);
    auto wheel = std::make_shared<PickerWheel>();
    button.PointerWheelChanged([weakButton, onPick, wheel, debounceWheel](wf::IInspectable const&,
                                                                          wuxi::PointerRoutedEventArgs const& e) {
        try {
            if (g_xamlClosing) return;
            auto picker = weakButton.get();
            if (!picker) return;
            const int delta = e.GetCurrentPoint(picker).Properties().MouseWheelDelta();
            if (delta == 0) return;
            e.Handled(true);  // The list behind must not scroll.
            const int next = NextPickerTag(PickerTag(picker), delta > 0 ? -1 : 1);
            SetPickerValue(picker, next);
            if (!debounceWheel) {
                onPick(next);
                return;
            }
            wheel->tag = next;
            if (!wheel->timer) {
                wheel->timer = wux::DispatcherTimer{};
                wheel->timer.Interval(wf::TimeSpan{7000000});  // 700 ms
                std::weak_ptr<PickerWheel> weakWheel = wheel;
                wheel->timer.Tick([weakWheel, onPick](wf::IInspectable const&, wf::IInspectable const&) {
                    try {
                        auto state = weakWheel.lock();
                        if (!state) return;
                        state->timer.Stop();
                        if (!g_xamlClosing) onPick(state->tag);
                    } catch (...) {
                        LogXamlHandlerFailure(L"picker wheel Tick");
                    }
                });
            }
            wheel->timer.Stop();
            wheel->timer.Start();
        } catch (...) {
            LogXamlHandlerFailure(L"picker wheel");
        }
    });
    button.Click([weakButton, onPick, onPrompt, category, custom, mapped, highlighted](
                     wf::IInspectable const&, wux::RoutedEventArgs const&) {
        try {
            if (g_xamlClosing) return;
            auto picker = weakButton.get();
            if (!picker) return;

            RefreshHeadingCounts();
            wuxc::MenuFlyout flyout;
            auto addItem = [&flyout, weakButton, onPick](std::wstring text, int tag, bool checked, bool low = false) {
                wuxc::ToggleMenuFlyoutItem item;
                item.Text(text);
                item.IsChecked(checked);
                if (low) {
                    SetMarkTip(item, true);
                    item.Loaded([](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
                        try {
                            ColorStarInside(sender.as<wux::DependencyObject>());
                        } catch (...) {
                            LogXamlHandlerFailure(L"menu item star");
                        }
                    });
                }
                item.Click([weakButton, onPick, tag](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    try {
                        if (auto target = weakButton.get()) SetPickerValue(target, tag);
                        onPick(tag);
                    } catch (...) {
                        LogXamlHandlerFailure(L"picker item Click");
                    }
                });
                flyout.Items().Append(item);
            };

            addItem(Tr(L"Windows default (remove change)", L"Padr\u00e3o do Windows (remover altera\u00e7\u00e3o)"), -1,
                    highlighted == -1);
            for (const size_t index : SortedCategoryOrder()) {
                const int id = static_cast<int>(kCategories[index].id);
                const std::wstring text = (id == mapped)
                    ? HeadingName(static_cast<DWORD>(id)) + L" (" + Tr(L"Custom ID", L"ID personalizado") +
                          L" " + std::to_wstring(category) + L")"
                    : HeadingName(static_cast<DWORD>(id)) + L" (ID " + std::to_wstring(id) + L")";
                addItem(text, id, highlighted == id, IsLowHeading(static_cast<DWORD>(id)));
            }
            if (custom && mapped < 0) {
                addItem(HeadingName(category) + L" (" + Tr(L"Custom ID", L"ID personalizado") + L" " +
                            std::to_wstring(category) + L")",
                        static_cast<int>(category), true, IsLowHeading(category));
            }
            flyout.Items().Append(wuxc::MenuFlyoutSeparator{});

            wuxc::MenuFlyoutItem prompt;
            prompt.Text(Tr(L"Custom ID...", L"ID personalizado..."));
            prompt.Click([weakButton, onPrompt](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                try {
                    if (auto target = weakButton.get()) onPrompt(target);
                } catch (...) {
                    LogXamlHandlerFailure(L"picker prompt Click");
                }
            });
            flyout.Items().Append(prompt);

            flyout.ShowAt(picker);
        } catch (...) {
            LogXamlHandlerFailure(L"picker Click");
        }
    });
    return button;
}


// An exception that escapes a XAML event handler makes XAML fail-fast (0xC000027B) and takes Explorer down
// with it. Handlers catch their own errors and call this from the catch block, so the failure is logged.
void LogXamlHandlerFailure(const wchar_t* name) {
    try {
        throw;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"[xaml] %ls failed: 0x%08X %ls", name, static_cast<unsigned int>(error.code()),
               error.message().c_str());
    } catch (...) {
        Wh_Log(L"[xaml] %ls failed (unknown exception)", name);
    }
}

struct RowWidth {
    wuxc::Border grid;
    double last;
    int depth;  // 0 for a folder or a loose app, 1 for an app inside a folder.
};
[[clang::no_destroy]] std::vector<RowWidth> g_rowWidths;

// The gap between the list's left or right edge and a top-level card, in DIPs: the same on both sides.
constexpr double kCardSideGap = 36;
constexpr double kTreeLeftMargin = 2;    // tree.Margin().Left
constexpr double kTreeRightMargin = 12;  // tree.Margin().Right
constexpr double kChildIndent = 20;      // How far the tree indents an app inside a folder.

// Places every row and sets its width: the left edge of a top-level card is kCardSideGap from the list's left edge
// (the tree reserves room for its own expand button, so the card is pulled back with a negative margin), apps inside
// a folder stay kChildIndent further in, and the right edge is kCardSideGap from the list's right edge, so the picker
// reaches it. Runs after each layout pass.
void UpdateRowWidths(wuxc::TreeView const& tree) {
    // A layout pass of a window that is closing (or already replaced) must not touch its rows.
    if (g_xamlClosing || !g_xamlTree || winrt::get_abi(g_xamlTree) != winrt::get_abi(tree)) return;
    // This handler changes margins and widths, which starts another layout pass, which runs this handler again.
    // Normally that settles in a pass or two. If it keeps changing things (rows being recycled while the list is
    // scrolled fast), XAML gives up with a "layout cycle" and fails fast. After 8 passes in a row that changed
    // something, one pass is skipped, so no change is made and the loop ends.
    static int s_changingPasses = 0;
    if (s_changingPasses >= 8) {
        s_changingPasses = 0;
        return;
    }
    bool changed = false;
    const double treeWidth = tree.ActualWidth();
    for (auto& row : g_rowWidths) {
        // A row scrolled out of view (the list recycles its rows) is not in the tree, and TransformToVisual
        // throws for it. Skip it: it is placed again when it comes back into view.
        try {
        // A row that has not been arranged yet has no real position: measuring it would give a wrong margin.
        if (row.grid.ActualWidth() <= 0 || row.grid.ActualHeight() <= 0) continue;
        const auto origin = row.grid.TransformToVisual(tree).TransformPoint(wf::Point{0, 0});
        const wux::Thickness margin = row.grid.Margin();
        const double natural = origin.X - margin.Left;  // Where the tree would put the row with no margin.
        const double wanted = kCardSideGap - kTreeLeftMargin + row.depth * kChildIndent;
        const double newLeft = wanted - natural;
        if (std::abs(newLeft - margin.Left) > 0.5) {
            row.grid.Margin(wux::Thickness{newLeft, margin.Top, margin.Right, margin.Bottom});
            changed = true;
        }
        double width = treeWidth - wanted - (kCardSideGap - kTreeRightMargin);
        if (width < 200) width = 200;
        // The name gets only the room left of the picker, so a long name is cut with "..." and the picker keeps its size.
        if (auto grid = row.grid.Child().try_as<wuxc::Grid>(); grid && grid.Children().Size() >= 2) {
            auto left = grid.Children().GetAt(0).try_as<wuxc::StackPanel>();
            auto picker = grid.Children().GetAt(1).try_as<wux::FrameworkElement>();
            if (left && left.Children().Size() >= 3) {
                auto text = left.Children().GetAt(2).try_as<wuxc::TextBlock>();
                const double pickerWidth = picker && picker.ActualWidth() > 0 ? picker.ActualWidth() : g_pickerWidth;
                const double checkWidth = left.Children().GetAt(0).as<wux::FrameworkElement>().ActualWidth();
                const double iconWidth = left.Children().GetAt(1).as<wux::FrameworkElement>().ActualWidth();
                // Card padding (16 + 16), the gap before the picker, and the two 8 px gaps inside the name part.
                double limit = width - 32 - pickerWidth - 12 - 16 - (checkWidth > 0 ? checkWidth : 32) - (iconWidth > 0 ? iconWidth : 24);
                // The yellow star after the name (an app whose keys disagree) is the fourth child: leave room for it.
                if (left.Children().Size() >= 4) {
                    auto star = left.Children().GetAt(3).try_as<wux::FrameworkElement>();
                    limit -= (star && star.ActualWidth() > 0 ? star.ActualWidth() : 8.0) + 2.0;  // 8 px gap, -6 px margin.
                }
                if (limit < 40) limit = 40;
                if (text && std::abs(text.MaxWidth() - limit) > 0.5) {
                    text.MaxWidth(limit);
                    changed = true;
                }
            }
        }
        const double change = width > row.last ? width - row.last : row.last - width;
        if (change > 0.5) {
            row.grid.Width(width);
            row.last = width;
            changed = true;
        }
        } catch (...) {
        }
    }
    if (changed) ++s_changingPasses;
    else s_changingPasses = 0;
}

// A row: the name part on the left, the category picker in a column at the right edge.
// The manager window's colours (RGB only), picked from the Windows Settings app with transparency off. Opacity is NOT
// part of this: every fill keeps the opacity it already had. The window background is the translucent tint set where
// the XAML root is built (OpenXamlTreeWindow).
winrt::Windows::UI::Color PaletteColor(BYTE alpha, BYTE dark, BYTE light) {
    const BYTE v = g_isDark ? dark : light;
    return winrt::Windows::UI::Color{alpha, v, v, v};
}
// Card opacity: the same as before (dark 22, light 178). The list background is new; it uses the window tint's opacity.
winrt::Windows::UI::Color ListColor() { return PaletteColor(g_isDark ? 18 : 153, 132, 249); }
winrt::Windows::UI::Color CardColor() { return PaletteColor(g_isDark ? 31 : 178, 132, 253); }
winrt::Windows::UI::Color CardHoverColor() { return PaletteColor(g_isDark ? 54 : 226, 132, 253); }
// The circle behind a folder's chevron: clear at rest, light (dark) in the dark (light) theme under the mouse.
winrt::Windows::UI::Color ChevronRestColor() {
    return g_isDark ? winrt::Windows::UI::Color{0, 255, 255, 255} : winrt::Windows::UI::Color{0, 0, 0, 0};
}
winrt::Windows::UI::Color ChevronHoverColor() {
    return g_isDark ? winrt::Windows::UI::Color{46, 255, 255, 255} : winrt::Windows::UI::Color{34, 0, 0, 0};
}

// A card's fill and the animation that is changing it.
struct CardFade {
    wuxm::SolidColorBrush brush{nullptr};
    wuxa::Storyboard story{nullptr};
};

// Fades a card's fill to another colour (150 ms).
void FadeCardTo(std::shared_ptr<CardFade> const& fade, winrt::Windows::UI::Color color) {
    if (!fade || !fade->brush) return;
    wuxa::ColorAnimation animation;
    animation.To(winrt::box_value(color).as<wf::IReference<winrt::Windows::UI::Color>>());
    animation.Duration(wux::Duration{wf::TimeSpan{1500000}, wux::DurationType::TimeSpan});
    wuxa::Storyboard::SetTarget(animation, fade->brush);
    wuxa::Storyboard::SetTargetProperty(animation, L"Color");
    wuxa::Storyboard story;
    story.Children().Append(animation);
    fade->story = story;  // Kept until the next fade replaces it.
    story.Begin();
}

// The card highlight of a row, and whether the mouse is on one of the row's own controls (the drop-down, the
// folder chevron), which light up by themselves and take the highlight away from the card.
struct RowHover {
    std::shared_ptr<CardFade> fade;
    std::shared_ptr<bool> overControl;
};

// True when the mouse is inside the element (a pointer that left a child may still be over the card).
bool PointerInside(wuxc::Border const& element, wuxi::PointerRoutedEventArgs const& e) {
    const auto position = e.GetCurrentPoint(element).Position();
    return position.X >= 0 && position.Y >= 0 && position.X <= element.ActualWidth() &&
           position.Y <= element.ActualHeight();
}

// True when a tap landed on a control that handles clicks itself (a check box, the drop-down, a text box).
bool TappedOnControl(wux::DependencyObject start) {
    while (start) {
        if (start.try_as<wuxc::Primitives::ButtonBase>() || start.try_as<wuxc::TextBox>()) return true;
        start = wuxm::VisualTreeHelper::GetParent(start);
    }
    return false;
}

// A click on an app's row (not on its check box or drop-down) shows the registry key it uses in the status bar.
void AttachKeyStatus(wuxc::Border const& rowGrid, size_t index) {
    rowGrid.Tapped([index](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
        try {
            if (g_xamlClosing || TappedOnControl(e.OriginalSource().try_as<wux::DependencyObject>())) return;
            if (index < g_apps.size()) SetStatus(AppKeyStatus(g_apps[index]));
        } catch (...) {
            LogXamlHandlerFailure(L"app row Tapped");
        }
    });
}

// ---------------------------------------------------------------------------
// Open file location (right-click menu of the cards)
// ---------------------------------------------------------------------------

// The glyph (U+ED43) Start uses for "Open file location".
constexpr wchar_t kOpenLocationGlyph[] = L"\xED43";

// Desktop apps are listed as "{known-folder-GUID}\\path\\app.exe". Returns the real path, or empty.
std::wstring ExpandKnownFolderPath(const std::wstring& id) {
    if (id.size() < 3 || id[0] != L'{') return std::wstring();
    const size_t close = id.find(L'}');
    if (close == std::wstring::npos) return std::wstring();
    GUID guid{};
    if (FAILED(CLSIDFromString(id.substr(0, close + 1).c_str(), &guid))) return std::wstring();
    std::wstring result;
    PWSTR folder = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(guid, 0, nullptr, &folder)) && folder) {
        result = folder;
        result += id.substr(close + 1);
    }
    CoTaskMemFree(folder);
    return result;
}

// The program a .lnk shortcut starts (empty when it cannot be read, e.g. for .url files). Cached per file.
std::wstring ShortcutTarget(const std::wstring& lnkPath) {
    static std::unordered_map<std::wstring, std::wstring> s_targets;
    auto cached = s_targets.find(lnkPath);
    if (cached != s_targets.end()) return cached->second;

    std::wstring target;
    IShellLinkW* link = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link))) && link) {
        IPersistFile* file = nullptr;
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file))) && file) {
            if (SUCCEEDED(file->Load(lnkPath.c_str(), STGM_READ))) {
                wchar_t buffer[MAX_PATH * 2] = {};
                if (SUCCEEDED(link->GetPath(buffer, ARRAYSIZE(buffer), nullptr, 0))) target = buffer;
            }
            file->Release();
        }
        link->Release();
    }
    s_targets[lnkPath] = target;
    return target;
}

// The Start menu shortcut (.lnk / .url) of an app, or empty when it has none. The shortcut is found by name. When
// the same name exists in different folders (or the name differs from the file name), the program the shortcut
// starts is compared with the program the app is listed under.
std::wstring FindShortcutPath(const AppEntry& app) {
    const std::wstring wanted = Lower(app.displayName);
    std::vector<const ShortcutInfo*> byName;
    for (const auto& sc : g_shortcuts) {
        if (Lower(sc.name) == wanted) byName.push_back(&sc);
    }

    bool sameFolder = true;
    for (const ShortcutInfo* sc : byName) {
        if (CompareNoCase(sc->folder, byName[0]->folder) != CSTR_EQUAL) sameFolder = false;
    }
    // One shortcut, or the per-user and all-users copy of one: the first is the user's own.
    if (!byName.empty() && sameFolder) return byName[0]->fullPath;

    // Microsoft Store apps (their ID holds a "!") have no shortcut file.
    if (app.relPath.find(L'!') != std::wstring::npos) return std::wstring();

    const std::wstring wantedTarget = Lower(ExpandKnownFolderPath(app.relPath));
    if (!wantedTarget.empty()) {
        std::vector<const ShortcutInfo*> pool = byName;
        if (pool.empty()) {
            for (const auto& sc : g_shortcuts) pool.push_back(&sc);
        }
        for (const ShortcutInfo* sc : pool) {
            if (Lower(ShortcutTarget(sc->fullPath)) == wantedTarget) return sc->fullPath;
        }
    }
    return byName.empty() ? std::wstring() : byName[0]->fullPath;
}

// The Start menu folder (on disk) that holds a folder card's shortcuts, or empty when it cannot be found.
std::wstring FindFolderDirectory(const std::wstring& folderName) {
    std::map<std::wstring, int, FolderLess> counts;
    for (const auto& app : g_apps) {
        if (app.folderPath != folderName) continue;
        const std::wstring lnk = FindShortcutPath(app);
        const size_t slash = lnk.find_last_of(L"\\/");
        if (slash != std::wstring::npos) counts[lnk.substr(0, slash)]++;
    }
    std::wstring best;
    int bestCount = 0;
    for (const auto& entry : counts) {
        if (entry.second > bestCount) {
            best = entry.first;
            bestCount = entry.second;
        }
    }
    if (!best.empty()) return best;

    for (const std::wstring& root : {GetKnownFolder(FOLDERID_Programs), GetKnownFolder(FOLDERID_CommonPrograms)}) {
        if (root.empty()) continue;
        const std::wstring dir = root + L"\\" + folderName;
        if (PathIsDirectoryW(dir.c_str())) return dir;
    }
    return std::wstring();
}

// Opens File Explorer on the file's folder with the file selected. Falls back to just opening the folder.
void SelectInExplorer(const std::wstring& path) {
    bool done = false;
    PIDLIST_ABSOLUTE full = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &full, 0, nullptr)) && full) {
        PIDLIST_ABSOLUTE parent = ILClone(full);
        if (parent) {
            if (ILRemoveLastID(parent)) {
                PCUITEMID_CHILD child = ILFindLastID(full);
                done = SUCCEEDED(SHOpenFolderAndSelectItems(parent, 1, &child, 0));
            }
            ILFree(parent);
        }
        ILFree(full);
    }
    if (!done) {
        const size_t slash = path.find_last_of(L"\\/");
        const std::wstring dir = slash == std::wstring::npos ? path : path.substr(0, slash);
        ShellExecuteW(nullptr, L"open", dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

// Shows the card's right-click menu: one "Open file location" item, in the same style as Start's own menu.
// The path is the shortcut (an app, which gets selected) or the folder (a folder card); empty greys the item out.
void ShowLocationMenu(wuxc::Border const& card, wf::Point position, const std::wstring& path, bool isFolder) {
    wuxc::MenuFlyout flyout;
    wuxc::MenuFlyoutItem item;
    item.Text(Tr(L"Open file location", L"Abrir local do arquivo"));

    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(kOpenLocationGlyph);
    icon.FontSize(16);
    item.Icon(icon);

    if (path.empty()) {
        // A disabled item shows no tooltip, so the item only looks greyed out. Clicking it explains in the status bar.
        // The menu item's template sets its colours from theme resources in every visual state, so a Foreground set
        // on the item itself is overridden. The resources are replaced for this one item instead: grey text in every
        // state, and no highlight under the mouse or when pressed.
        const BYTE shade = g_isDark ? 255 : 0;
        auto grey = wuxm::SolidColorBrush(winrt::Windows::UI::Color{110, shade, shade, shade});
        auto clear = wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
        auto resources = item.Resources();
        for (const wchar_t* key : {L"MenuFlyoutItemForeground", L"MenuFlyoutItemForegroundPointerOver",
                                   L"MenuFlyoutItemForegroundPressed", L"MenuFlyoutItemForegroundDisabled"}) {
            resources.Insert(winrt::box_value(winrt::hstring{key}), grey);
        }
        for (const wchar_t* key : {L"MenuFlyoutItemBackgroundPointerOver", L"MenuFlyoutItemBackgroundPressed",
                                   L"MenuFlyoutItemBackgroundDisabled"}) {
            resources.Insert(winrt::box_value(winrt::hstring{key}), clear);
        }
        item.Foreground(grey);
        icon.Foreground(grey);
        const std::wstring reason = isFolder
            ? Tr(L"This folder was not found on disk.", L"Esta pasta n\u00e3o foi encontrada no disco.")
            : Tr(L"This app has no Start menu shortcut.", L"Este aplicativo n\u00e3o tem atalho no Menu Iniciar.");
        wuxc::ToolTipService::SetToolTip(item, winrt::box_value(winrt::hstring{reason}));
        item.Click([reason](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            try {
                if (!g_xamlClosing) SetStatus(reason);
            } catch (...) {
                LogXamlHandlerFailure(L"open file location (none) Click");
            }
        });
    } else {
        item.Click([path, isFolder](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            try {
                if (g_xamlClosing) return;
                if (isFolder) {
                    ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                } else {
                    SelectInExplorer(path);
                }
            } catch (...) {
                LogXamlHandlerFailure(L"open file location Click");
            }
        });
    }
    flyout.Items().Append(item);
    flyout.ShowAt(card, position);
}

// Right-click on an app's card (not on its check box or drop-down).
void AttachLocationMenu(wuxc::Border const& card, size_t index) {
    card.RightTapped([weakCard = winrt::make_weak(card), index](wf::IInspectable const&,
                                                                wuxi::RightTappedRoutedEventArgs const& e) {
        try {
            auto strongCard = weakCard.get();
            if (g_xamlClosing || !strongCard || index >= g_apps.size()) return;
            if (TappedOnControl(e.OriginalSource().try_as<wux::DependencyObject>())) return;
            e.Handled(true);
            ShowLocationMenu(strongCard, e.GetPosition(strongCard), FindShortcutPath(g_apps[index]), false);
        } catch (...) {
            LogXamlHandlerFailure(L"app card RightTapped");
        }
    });
}

// Right-click on a Start folder's card.
void AttachFolderLocationMenu(wuxc::Border const& card, std::wstring folderName) {
    card.RightTapped([weakCard = winrt::make_weak(card), folderName](wf::IInspectable const&,
                                                                     wuxi::RightTappedRoutedEventArgs const& e) {
        try {
            auto strongCard = weakCard.get();
            if (g_xamlClosing || !strongCard) return;
            if (TappedOnControl(e.OriginalSource().try_as<wux::DependencyObject>())) return;
            e.Handled(true);
            ShowLocationMenu(strongCard, e.GetPosition(strongCard), FindFolderDirectory(folderName), true);
        } catch (...) {
            LogXamlHandlerFailure(L"folder card RightTapped");
        }
    });
}

wuxc::Border MakeRowGrid(wuxc::StackPanel const& left, wuxc::Button const& picker, bool card, int depth = 0,
                         std::shared_ptr<RowHover>* hoverOut = nullptr) {
    wuxc::Grid grid;
    wuxc::ColumnDefinition nameColumn;
    nameColumn.Width(wux::GridLength{1, wux::GridUnitType::Auto});
    wuxc::ColumnDefinition spacerColumn;
    spacerColumn.Width(wux::GridLength{1, wux::GridUnitType::Star});
    wuxc::ColumnDefinition pickerColumn;
    pickerColumn.Width(wux::GridLength{1, wux::GridUnitType::Auto});
    grid.ColumnDefinitions().Append(nameColumn);
    grid.ColumnDefinitions().Append(spacerColumn);
    grid.ColumnDefinitions().Append(pickerColumn);
    wuxc::Grid::SetColumn(left, 0);
    wuxc::Grid::SetColumn(picker, 2);
    grid.Children().Append(left);
    grid.Children().Append(picker);

    // The card's background is a separate rounded element behind the content (it reaches over the cell's padding with
    // a negative margin). A rounded Border clips what it holds, which would hide the folder chevron that sits in the
    // gap to the left of the card.
    wuxc::Border cardBackground{nullptr};
    if (card) {
        cardBackground = wuxc::Border{};
        cardBackground.CornerRadius(wux::CornerRadius{4, 4, 4, 4});
        cardBackground.Margin(wux::Thickness{-16, -16, -16, -16});
        cardBackground.Background(wuxm::SolidColorBrush(CardColor()));
        wuxc::Grid::SetColumnSpan(cardBackground, 3);
        wuxc::Canvas::SetZIndex(cardBackground, -1);
        grid.Children().Append(cardBackground);
    }

    wuxc::Border cell;
    cell.Padding(wux::Thickness{16, 16, 16, 16});
    cell.Margin(wux::Thickness{0, 4, 0, 4});
    cell.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cell.Child(grid);
    if (cardBackground) {
        // Only the card lights up under the mouse (the tree's own highlight, which spans the whole list, is switched
        // off). The colour fades. Over the row's drop-down the card fades back, so the drop-down stands out.
        auto hover = std::make_shared<RowHover>();
        hover->fade = std::make_shared<CardFade>();
        hover->fade->brush = cardBackground.Background().as<wuxm::SolidColorBrush>();
        hover->overControl = std::make_shared<bool>(false);
        auto weakCell = winrt::make_weak(cell);
        cell.PointerEntered([hover](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
            if (!*hover->overControl) FadeCardTo(hover->fade, CardHoverColor());
        });
        cell.PointerExited([hover](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
            *hover->overControl = false;
            FadeCardTo(hover->fade, CardColor());
        });
        picker.PointerEntered([hover](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
            *hover->overControl = true;
            FadeCardTo(hover->fade, CardColor());
        });
        picker.PointerExited([hover, weakCell](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& e) {
            *hover->overControl = false;
            auto strongCell = weakCell.get();
            if (strongCell && PointerInside(strongCell, e)) FadeCardTo(hover->fade, CardHoverColor());
        });
        if (hoverOut) *hoverOut = hover;
    }
    g_rowWidths.push_back({cell, 0.0, depth});
    return cell;
}

// The check box of a tree row (inside the name part, its first child).
// The expand / collapse button of a folder. It sits in the gap to the left of the card (the tree's own button is
// hidden), and is the fourth child of the row's grid, after the name part, the picker and the card background.
constexpr wchar_t kChevronExpanded[] = L"\xE70D";
constexpr wchar_t kChevronCollapsed[] = L"\xE76C";

void RebuildXamlTree(wuxc::TreeView const& tree);

wuxc::ScrollViewer FindScrollViewer(wux::DependencyObject const& parent) {
    namespace wuxmedia = winrt::Windows::UI::Xaml::Media;
    const int count = wuxmedia::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wuxmedia::VisualTreeHelper::GetChild(parent, i);
        if (auto viewer = child.try_as<wuxc::ScrollViewer>()) return viewer;
        if (auto found = FindScrollViewer(child)) return found;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Keeping the list's scroll bar expanded while it is held down (experimental)
// ---------------------------------------------------------------------------
// XAML shows a thin scroll bar and widens it while the mouse is on it. In this window the wide form falls back to
// the thin one after a few seconds even though the button is still held on the bar. While the left button is held
// on the bar, this puts the bar back into its wide state every 250 ms. It is only a visual workaround; scrolling
// itself is not touched.
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;

[[clang::no_destroy]] wuxcp::ScrollBar g_scrollBar{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_scrollBarTimer{nullptr};
bool g_scrollBarHooked = false;
bool g_scrollBarHeld = false;

wuxcp::ScrollBar FindVerticalScrollBar(wux::DependencyObject const& parent, int depth = 0) {
    if (depth > 30) return nullptr;
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        if (auto bar = child.try_as<wuxcp::ScrollBar>()) {
            if (bar.Orientation() == wuxc::Orientation::Vertical) return bar;
        }
        if (auto found = FindVerticalScrollBar(child, depth + 1)) return found;
    }
    return nullptr;
}

void ForceScrollBarExpanded() {
    if (!g_scrollBar) return;
    // A name that does not exist in the bar's template is ignored, so both guesses are safe to try.
    wux::VisualStateManager::GoToState(g_scrollBar, L"MouseIndicatorFull", false);
    wux::VisualStateManager::GoToState(g_scrollBar, L"Expanded", false);
}

void StartScrollBarHold() {
    g_scrollBarHeld = true;
    ForceScrollBarExpanded();
    if (!g_scrollBarTimer) {
        g_scrollBarTimer = wux::DispatcherTimer{};
        g_scrollBarTimer.Interval(wf::TimeSpan{2500000});  // 250 ms
        g_scrollBarTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                if (!g_scrollBarTimer) return;
                // The button state is read from Windows itself, so the hold also ends if the release is never seen.
                if (g_xamlClosing || !g_scrollBarHeld || !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    g_scrollBarHeld = false;
                    g_scrollBarTimer.Stop();
                    return;
                }
                ForceScrollBarExpanded();
            } catch (...) {
                LogXamlHandlerFailure(L"scroll bar Tick");
            }
        });
    }
    g_scrollBarTimer.Start();
}

// Finds the list's vertical scroll bar once its template exists, and listens for a press on it. Called from the
// list's layout pass until it succeeds; after that it returns at once.
void HookTreeScrollBar(wuxc::TreeView const& tree) {
    if (g_scrollBarHooked || g_xamlClosing) return;
    auto viewer = FindScrollViewer(tree);
    if (!viewer) return;
    auto bar = FindVerticalScrollBar(viewer);
    if (!bar) return;
    g_scrollBarHooked = true;
    g_scrollBar = bar;
    // handledEventsToo: the thumb handles the press itself, and the bar must still hear about it.
    bar.AddHandler(wux::UIElement::PointerPressedEvent(),
                   winrt::box_value(wuxi::PointerEventHandler(
                       [](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
                           try {
                               if (g_xamlClosing) return;
                               auto self = sender.as<wux::UIElement>();
                               if (e.GetCurrentPoint(self).Properties().IsLeftButtonPressed()) StartScrollBarHold();
                           } catch (...) {
                               LogXamlHandlerFailure(L"scroll bar PointerPressed");
                           }
                       })),
                   true);
}

// Opens or closes one folder. The tree's own expand / collapse can lose some of a folder's apps when it is used quickly
// (they stay blank until the folder is toggled again), so the list is rebuilt instead, as "Expand all" does, with the
// scroll position kept.
void ToggleFolder(std::wstring name, bool nowExpanded) {
    if (!g_xamlTree) return;
    g_folderExpanded[name] = nowExpanded;
    auto viewer = FindScrollViewer(g_xamlTree);
    const double offset = viewer ? viewer.VerticalOffset() : 0.0;
    RebuildXamlTree(g_xamlTree);
    if (viewer) {
        g_xamlTree.UpdateLayout();
        viewer.ChangeView(nullptr, winrt::box_value(offset).as<wf::IReference<double>>(), nullptr, true);
    }
}

void AddFolderChevron(wuxc::Border const& card, wuxc::TreeViewNode const& node, std::wstring name,
                      std::shared_ptr<RowHover> hover) {
    wuxc::FontIcon icon;
    icon.Glyph(node.IsExpanded() ? kChevronExpanded : kChevronCollapsed);
    icon.FontSize(12);
    wuxc::Border chevron;
    chevron.Width(28);
    chevron.Height(28);
    chevron.CornerRadius(wux::CornerRadius{14, 14, 14, 14});  // A circle lights up around it under the mouse.
    auto chevronFade = std::make_shared<CardFade>();
    chevronFade->brush = wuxm::SolidColorBrush(ChevronRestColor());  // Clear, but it still gets the mouse.
    chevron.Background(chevronFade->brush);
    chevron.HorizontalAlignment(wux::HorizontalAlignment::Left);
    chevron.VerticalAlignment(wux::VerticalAlignment::Center);
    // 16 is the card's padding, then the gap minus 4 px of breathing room next to the card.
    chevron.Margin(wux::Thickness{-(16 + (kCardSideGap - 4)), 0, 0, 0});
    chevron.Child(icon);
    // The chevron sits in the row's grid, so the card also hears the mouse over it. While the mouse is on the
    // chevron only the chevron lights up, and the card is left alone.
    chevron.PointerEntered([chevronFade, hover](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
        FadeCardTo(chevronFade, ChevronHoverColor());
        if (hover) {
            *hover->overControl = true;
            FadeCardTo(hover->fade, CardColor());
        }
    });
    chevron.PointerExited([chevronFade, hover, weakCard = winrt::make_weak(card)](wf::IInspectable const&,
                                                                                 wuxi::PointerRoutedEventArgs const& e) {
        FadeCardTo(chevronFade, ChevronRestColor());
        if (!hover) return;
        *hover->overControl = false;
        auto strongCard = weakCard.get();
        if (strongCard && PointerInside(strongCard, e)) FadeCardTo(hover->fade, CardHoverColor());
    });
    chevron.Tapped([weakNode = winrt::make_weak(node), name](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
        e.Handled(true);
        if (g_xamlClosing) return;
        try {
            if (auto strongNode = weakNode.get()) ToggleFolder(name, !strongNode.IsExpanded());
        } catch (...) {
            LogXamlHandlerFailure(L"chevron Tapped");
        }
    });
    // A click on the folder's card expands or collapses it too, unless it landed on the check box or the drop-down.
    card.Tapped([weakNode = winrt::make_weak(node), name](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
        try {
            if (g_xamlClosing || TappedOnControl(e.OriginalSource().try_as<wux::DependencyObject>())) return;
            e.Handled(true);
            SetStatus(FolderKeyStatus(name));
            if (auto strongNode = weakNode.get()) ToggleFolder(name, !strongNode.IsExpanded());
        } catch (...) {
            LogXamlHandlerFailure(L"folder card Tapped");
        }
    });
    wuxc::Grid::SetColumn(chevron, 0);
    card.Child().as<wuxc::Grid>().Children().Append(chevron);
}

// Keeps a folder's chevron in step with its state (the Expand / Collapse all button changes it from code).
void SetFolderChevron(wuxc::TreeViewNode const& node, bool expanded) {
    auto card = node.Content().try_as<wuxc::Border>();
    if (!card) return;
    auto grid = card.Child().try_as<wuxc::Grid>();
    if (!grid || grid.Children().Size() < 4) return;
    if (auto chevron = grid.Children().GetAt(3).try_as<wuxc::Border>()) {
        if (auto icon = chevron.Child().try_as<wuxc::FontIcon>()) icon.Glyph(expanded ? kChevronExpanded : kChevronCollapsed);
    }
}

wuxc::CheckBox CheckOf(wuxc::TreeViewNode const& node) {
    return node.Content().as<wuxc::Border>().Child().as<wuxc::Grid>().Children().GetAt(0).as<wuxc::StackPanel>().Children().GetAt(0).as<wuxc::CheckBox>();
}

// The drop-down of a tree row (the second child of the row grid).
wuxc::Button ButtonOf(wuxc::TreeViewNode const& node) {
    return node.Content().as<wuxc::Border>().Child().as<wuxc::Grid>().Children().GetAt(1).as<wuxc::Button>();
}

bool IsOn(wuxc::CheckBox const& check) {
    auto value = check.IsChecked();
    return value && value.Value();
}

[[clang::no_destroy]] wux::DispatcherTimer g_markReloadTimer{nullptr};

// Rebuilds the list a moment later (outside the event that asked for it), keeping the scroll position, so that every
// place that shows a heading's name gets or loses its mark.
void ScheduleMarkReload() {
    if (g_markReloadTimer) return;
    wux::DispatcherTimer timer;
    timer.Interval(wf::TimeSpan{800000});  // 80 ms
    timer.Tick([](wf::IInspectable const& sender, wf::IInspectable const&) {
        sender.as<wux::DispatcherTimer>().Stop();
        g_markReloadTimer = nullptr;
        try {
            if (g_xamlClosing || !g_xamlTree) return;
            auto viewer = FindScrollViewer(g_xamlTree);
            const double offset = viewer ? viewer.VerticalOffset() : 0.0;
            RebuildXamlTree(g_xamlTree);
            if (viewer) {
                g_xamlTree.UpdateLayout();
                viewer.ChangeView(nullptr, winrt::box_value(offset).as<wf::IReference<double>>(), nullptr, true);
            }
        } catch (...) {
            LogXamlHandlerFailure(L"mark reload");
        }
    });
    g_markReloadTimer = timer;
    timer.Start();
}

// Applies a category to some apps at once and shows the result.
void ApplyRowCategory(const std::vector<size_t>& indices, int tag, const std::vector<std::wstring>& folders) {
    std::vector<size_t> targets;
    for (size_t i : indices) {
        if (i < g_apps.size()) targets.push_back(i);
    }
    if (targets.empty()) return;
    const LONG_PTR value = tag < 0 ? static_cast<LONG_PTR>(kComboDefault) : static_cast<LONG_PTR>(tag);
    RefreshHeadingCounts();
    const auto lowBefore = CurrentLowSet();
    const std::wstring msg = ApplyCategoryToTargets(targets, folders, value, false);
    if (g_xamlStatus) g_xamlStatus.Text(msg);
    // When the change makes a heading reach or leave the 1 to 2 apps range, reload the list.
    RefreshHeadingCounts();
    if (CurrentLowSet() != lowBefore) ScheduleMarkReload();
}

// A folder ticks when all of its apps are ticked.
void SyncFolderCheck(wuxc::TreeViewNode const& folderNode, wuxc::CheckBox const& folderCheck,
                     std::shared_ptr<bool> busy) {
    if (*busy) return;
    bool all = true;
    for (auto child : folderNode.Children()) {
        if (!IsOn(CheckOf(child))) {
            all = false;
            break;
        }
    }
    *busy = true;
    folderCheck.IsChecked(all);
    *busy = false;
}

// Ticking or unticking a folder does the same to every app under it.
void SetFolderApps(wuxc::TreeViewNode const& folderNode, bool value, std::shared_ptr<bool> busy,
                   std::vector<size_t> const& indices) {
    *busy = true;
    size_t n = 0;
    for (auto child : folderNode.Children()) {
        CheckOf(child).IsChecked(value);
        if (n < indices.size() && indices[n] < g_apps.size()) g_apps[indices[n]].checked = value;
        ++n;
    }
    *busy = false;
}

void ShowCustomIdInline(wuxc::TreeView const& tree, wuxc::Button const& picker, std::vector<size_t> const indices,
                        std::vector<std::wstring> const folders);

void UpdateXamlCheckAll();
void RebuildXamlTree(wuxc::TreeView const& tree);
void RefreshXamlFilterCounts();

// Wrapping rows. The toolbar and the bottom bar are lists of controls laid out left to right; when the window is
// too narrow for them, the ones that do not fit move to a new row underneath.
struct WrapGroup {
    wuxc::StackPanel host{nullptr};  // Vertical: one horizontal row per line.
    std::vector<wux::FrameworkElement> items;
    double spacing = 8;
};
[[clang::no_destroy]] WrapGroup g_toolbarGroup;
[[clang::no_destroy]] WrapGroup g_barGroup;
double g_xamlContentWidth = 0;  // The width of the XAML content, in DIPs; 0 until the first layout.
std::map<void*, double> g_itemWidths;
constexpr double kWrapSideMargins = 24;  // The 12 DIP margin on each side of the toolbar and the bottom bar.

double ItemWidth(wux::FrameworkElement const& item) {
    const double fixedWidth = item.Width();
    if (!std::isnan(fixedWidth)) return fixedWidth;
    void* key = winrt::get_abi(item);
    const double actual = item.ActualWidth();
    if (actual > 0) {
        g_itemWidths[key] = actual;
        return actual;
    }
    auto found = g_itemWidths.find(key);
    if (found != g_itemWidths.end()) return found->second;
    item.Measure(winrt::Windows::Foundation::Size{(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)()});
    return item.DesiredSize().Width;
}

double WrapAvailableWidth() {
    return g_xamlContentWidth > kWrapSideMargins ? g_xamlContentWidth - kWrapSideMargins : 1e9;
}

void ReflowGroup(WrapGroup& group, double available) {
    if (!group.host) return;
    std::vector<double> widths;
    for (auto const& item : group.items) widths.push_back(ItemWidth(item));  // Measure while still in the tree.
    for (auto const& child : group.host.Children()) child.as<wuxc::StackPanel>().Children().Clear();
    group.host.Children().Clear();
    wuxc::StackPanel row{nullptr};
    double used = 0;
    for (size_t i = 0; i < group.items.size(); ++i) {
        auto const& item = group.items[i];
        if (item.Visibility() == wux::Visibility::Collapsed) continue;
        const double width = (std::min)(widths[i], available);
        if (!row || (used > 0 && used + group.spacing + width > available)) {
            row = wuxc::StackPanel{};
            row.Orientation(wuxc::Orientation::Horizontal);
            row.Spacing(group.spacing);
            group.host.Children().Append(row);
            used = 0;
        }
        row.Children().Append(item);
        used += (used > 0 ? group.spacing : 0) + width;
    }
}

bool g_xamlReflowing = false;

void ReflowXamlBars() {
    if (g_xamlReflowing || g_xamlClosing) return;
    g_xamlReflowing = true;
    try {
        const double available = WrapAvailableWidth();
        if (g_xamlSearch) g_xamlSearch.Width((std::min)(260.0, (std::max)(120.0, available)));
        if (g_xamlStatus) g_xamlStatus.MaxWidth(available);
        ReflowGroup(g_toolbarGroup, available);
        ReflowGroup(g_barGroup, available);
    } catch (...) {
        LogXamlHandlerFailure(L"ReflowXamlBars");
    }
    g_xamlReflowing = false;  // Always cleared, so one failure cannot stop every later reflow.
}

// The ID box of the bottom bar is only there while "Custom ID..." is chosen in the bottom drop-down.
void SetCustomIdBoxVisible(bool visible) {
    if (!g_xamlCustomId) return;
    g_xamlCustomId.Visibility(visible ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (!visible) g_xamlCustomId.Text(winrt::hstring{});
    ReflowXamlBars();
}

// The filter drop-down's value (kXamlFilterAll, -1 for Windows default, or a category ID).
int XamlFilterSelection() {
    if (!g_xamlFilter) return kXamlFilterAll;
    auto item = g_xamlFilter.SelectedItem().try_as<wuxc::ComboBoxItem>();
    return item ? winrt::unbox_value<int>(item.Tag()) : kXamlFilterAll;
}

[[clang::no_destroy]] wux::DispatcherTimer g_filterStarTimer{nullptr};

// The closed filter box shows the chosen entry as plain text: give it the tip and, once the box has updated,
// colour the "*" of a marked heading.
void UpdateFilterForeground() {
    if (!g_xamlFilter) return;
    const int tag = XamlFilterSelection();
    const bool low = tag >= 0 && IsLowHeading(static_cast<DWORD>(tag));
    SetMarkTip(g_xamlFilter, low);
    if (!low) return;
    wux::DispatcherTimer timer;
    timer.Interval(wf::TimeSpan{400000});  // 40 ms
    timer.Tick([](wf::IInspectable const& sender, wf::IInspectable const&) {
        sender.as<wux::DispatcherTimer>().Stop();
        g_filterStarTimer = nullptr;
        try {
            if (!g_xamlClosing && g_xamlFilter) ColorStarInside(g_xamlFilter);
        } catch (...) {
            LogXamlHandlerFailure(L"filter box star");
        }
    });
    g_filterStarTimer = timer;
    timer.Start();
}

// The mouse wheel over a closed combo box moves its selection to the previous or next entry, wrapping around.
void AttachWheelCycle(wuxc::ComboBox const& combo) {
    combo.PointerWheelChanged([](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
        try {
            if (g_xamlClosing) return;
            auto box = sender.as<wuxc::ComboBox>();
            if (box.IsDropDownOpen()) return;
            const int delta = e.GetCurrentPoint(box).Properties().MouseWheelDelta();
            const int count = static_cast<int>(box.Items().Size());
            if (delta == 0 || count == 0) return;
            e.Handled(true);
            const int step = delta > 0 ? -1 : 1;
            box.SelectedIndex((box.SelectedIndex() + step + count) % count);
        } catch (...) {
            LogXamlHandlerFailure(L"combo wheel");
        }
    });
}

// The sort drop-down's value (one of the SortMode values).
int XamlSortMode() {
    if (!g_xamlSort) return kSortNameAsc;
    auto item = g_xamlSort.SelectedItem().try_as<wuxc::ComboBoxItem>();
    return item ? winrt::unbox_value<int>(item.Tag()) : kSortNameAsc;
}

// The category filter, then the search text.
bool XamlAppMatches(const AppEntry& app, int filter, const std::wstring& search) {
    if (filter != kXamlFilterAll) {
        if (filter == -1) {
            if (app.hasCategory) return false;
        } else if (!app.hasCategory ||
                   CategoryNameEn(app.category, true) != CategoryNameEn(static_cast<DWORD>(filter), true)) {
            return false;
        }
    }
    if (search.empty()) return true;
    return ContainsInsensitive(app.displayName, search) || ContainsInsensitive(app.folderPath, search) ||
           ContainsInsensitive(CategoryLabel(app.category, app.hasCategory), search) ||
           ContainsInsensitive(CategoryNameEn(app.category, app.hasCategory), search);
}

// Standalone apps and folders in one list, sorted by the chosen order; folders show their matching apps.
void BuildXamlTree(wuxc::TreeView const& tree) {
    RefreshHeadingCounts();
    g_rowWidths.clear();
    // The width only depends on the category names and the font, so measure it once instead of creating
    // and measuring a dozen text blocks on every rebuild.
    static double s_measuredPickerWidth = 0.0;
    if (s_measuredPickerWidth <= 0.0) s_measuredPickerWidth = ComputePickerWidth();
    g_pickerWidth = s_measuredPickerWidth;

    const int filter = XamlFilterSelection();
    const std::wstring search = g_xamlSearchText;
    const int sortMode = XamlSortMode();
    auto visible = [&](const AppEntry& app) { return XamlAppMatches(app, filter, search); };

    // A folder's ID range comes from all of its apps, not only the visible ones
    struct FolderInfo {
        bool anyId = false;
        DWORD minId = 0;
        DWORD maxId = 0;
        bool anyConflict = false;
    };
    std::map<std::wstring, FolderInfo, FolderLess> infos;
    for (const auto& app : g_apps) {
        if (app.folderPath.empty()) continue;
        FolderInfo& info = infos[app.folderPath];
        if (app.conflict) info.anyConflict = true;
        if (app.hasCategory) {
            if (!info.anyId) {
                info.anyId = true;
                info.minId = info.maxId = app.category;
            } else {
                info.minId = std::min(info.minId, app.category);
                info.maxId = std::max(info.maxId, app.category);
            }
        }
    }

    struct Root {
        std::wstring name;
        bool folder;
        size_t index;
        SortKey key;
    };
    std::vector<Root> roots;
    std::set<std::wstring, FolderLess> shownFolders;
    for (const auto& app : g_apps) {
        if (!app.folderPath.empty() && visible(app)) shownFolders.insert(app.folderPath);
    }
    for (const auto& f : shownFolders) {
        const FolderInfo& info = infos[f];
        SortKey key;
        key.name = f;
        key.hasId = info.anyId;
        key.id = sortMode == kSortIdDesc ? info.maxId : info.minId;
        key.heading = info.anyId ? CategoryName(info.minId, true) : std::wstring();
        key.conflict = info.anyConflict;
        roots.push_back({f, true, 0, key});
    }
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (g_apps[i].folderPath.empty() && visible(g_apps[i])) {
            roots.push_back({g_apps[i].displayName, false, i, MakeSortKey(g_apps[i])});
        }
    }
    std::stable_sort(roots.begin(), roots.end(), [sortMode](const Root& x, const Root& y) {
        return SortKeyLess(x.key, y.key, sortMode);
    });

    for (const auto& root : roots) {
        if (!root.folder) {
            const size_t index = root.index;
            const AppEntry& app = g_apps[index];

            wuxc::StackPanel row = MakeTreeRow(app.displayName, L"\xE774", app.checked, app.iconIndex, 24, app.conflict);
            wuxc::Button picker = MakePicker(app.hasCategory, app.category, false,
                [index](int tag) { ApplyRowCategory({index}, tag, {}); },
                [index, weakTree = winrt::make_weak(tree)](wuxc::Button const& self) {
                    if (auto strongTree = weakTree.get()) ShowCustomIdInline(strongTree, self, {index}, {});
                });
            wuxc::Border rowGrid = MakeRowGrid(row, picker, true);
            AttachKeyStatus(rowGrid, index);
            AttachLocationMenu(rowGrid, index);

            wuxc::TreeViewNode node;
            node.Content(rowGrid);
            CheckOf(node).Checked([index](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (index < g_apps.size()) g_apps[index].checked = true; UpdateXamlCheckAll();
            });
            CheckOf(node).Unchecked([index](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (index < g_apps.size()) g_apps[index].checked = false; UpdateXamlCheckAll();
            });
            tree.RootNodes().Append(node);
            continue;
        }

        // Folder: its apps, then the folder's own row with a picker that sets all of them.
        std::vector<size_t> indices;
        for (size_t i = 0; i < g_apps.size(); ++i) {
            if (g_apps[i].folderPath == root.name && visible(g_apps[i])) indices.push_back(i);
        }
        std::stable_sort(indices.begin(), indices.end(), [sortMode](size_t x, size_t y) {
            return SortKeyLess(MakeSortKey(g_apps[x]), MakeSortKey(g_apps[y]), sortMode);
        });

        bool mixed = false;
        bool firstHas = false;
        DWORD firstCategory = 0;
        for (size_t n = 0; n < indices.size(); ++n) {
            const AppEntry& app = g_apps[indices[n]];
            if (n == 0) {
                firstHas = app.hasCategory;
                firstCategory = app.category;
            } else if (app.hasCategory != firstHas || (app.hasCategory && app.category != firstCategory)) {
                mixed = true;
            }
        }

        wuxc::TreeViewNode folderNode;
        wuxc::StackPanel folderRow = MakeTreeRow(root.name, L"\xE8B7", false, g_folderIconIndex, 24);
        wuxc::Button folderPicker = MakePicker(firstHas, firstCategory, mixed,
            [weakNode = winrt::make_weak(folderNode), indices, folderName = root.name](int tag) {
                if (auto node = weakNode.get()) {
                    for (auto child : node.Children()) SetPickerValue(ButtonOf(child), tag);
                }
                ApplyRowCategory(indices, tag, {folderName});
            },
            [indices, folderName = root.name, weakTree = winrt::make_weak(tree)](wuxc::Button const& self) {
                if (auto strongTree = weakTree.get()) ShowCustomIdInline(strongTree, self, indices, {folderName});
            });
        std::shared_ptr<RowHover> folderHover;
        wuxc::Border folderCard = MakeRowGrid(folderRow, folderPicker, true, 0, &folderHover);
        AttachFolderLocationMenu(folderCard, root.name);
        folderNode.Content(folderCard);
        auto savedState = g_folderExpanded.find(root.name);
        folderNode.IsExpanded(savedState != g_folderExpanded.end() ? savedState->second : g_xamlExpanded);
        AddFolderChevron(folderCard, folderNode, root.name, folderHover);

        for (size_t n = 0; n < indices.size(); ++n) {
            const size_t index = indices[n];
            const AppEntry& app = g_apps[index];
            wuxc::StackPanel childRow = MakeTreeRow(app.displayName, L"\xE774", app.checked, app.iconIndex, 24, app.conflict);
            wuxc::Button picker = MakePicker(app.hasCategory, app.category, false,
                [index](int tag) { ApplyRowCategory({index}, tag, {}); },
                [index, weakTree = winrt::make_weak(tree)](wuxc::Button const& self) {
                    if (auto strongTree = weakTree.get()) ShowCustomIdInline(strongTree, self, {index}, {});
                });
            wuxc::TreeViewNode child;
            wuxc::Border childGrid = MakeRowGrid(childRow, picker, true, 1);
            AttachKeyStatus(childGrid, index);
            AttachLocationMenu(childGrid, index);
            child.Content(childGrid);
            folderNode.Children().Append(child);
        }

        // Ticking the folder ticks its apps, and each app's tick updates the folder.
        auto busy = std::make_shared<bool>(false);
        wuxc::CheckBox folderCheck = CheckOf(folderNode);
        // Weak, for the same reason as in MakePicker: the node owns the check box, the check box owns its handlers.
        auto weakFolder = winrt::make_weak(folderNode);
        auto weakFolderCheck = winrt::make_weak(folderCheck);
        folderCheck.Checked([weakFolder, busy, indices](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            if (auto node = weakFolder.get(); node && !*busy) SetFolderApps(node, true, busy, indices);
        });
        folderCheck.Unchecked([weakFolder, busy, indices](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            if (auto node = weakFolder.get(); node && !*busy) SetFolderApps(node, false, busy, indices);
        });
        for (size_t n = 0; n < indices.size(); ++n) {
            const size_t index = indices[n];
            wuxc::TreeViewNode child = folderNode.Children().GetAt(static_cast<uint32_t>(n));
            CheckOf(child).Checked([weakFolder, weakFolderCheck, busy, index](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (index < g_apps.size()) g_apps[index].checked = true; UpdateXamlCheckAll();
                auto node = weakFolder.get();
                auto check = weakFolderCheck.get();
                if (node && check) SyncFolderCheck(node, check, busy);
            });
            CheckOf(child).Unchecked([weakFolder, weakFolderCheck, busy, index](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (index < g_apps.size()) g_apps[index].checked = false; UpdateXamlCheckAll();
                auto node = weakFolder.get();
                auto check = weakFolderCheck.get();
                if (node && check) SyncFolderCheck(node, check, busy);
            });
        }


        tree.RootNodes().Append(folderNode);
    }
    UpdateXamlCheckAll();
}

// The bottom drop-down shows its chosen heading with a mark that follows the counts.
void UpdateBulkPickerMark() {
    if (!g_xamlBulkPicker) return;
    if (g_xamlCustomId && g_xamlCustomId.Visibility() == wux::Visibility::Visible) return;  // It reads "Custom ID...".
    SetPickerValue(g_xamlBulkPicker, PickerTag(g_xamlBulkPicker));
}

// Rebuilds the tree from the manager's current data, after a change that touched the registry.
void RebuildXamlTree(wuxc::TreeView const& tree) {
    tree.RootNodes().Clear();
    BuildXamlTree(tree);
    RefreshXamlFilterCounts();  // The counts change when apps change category.
    UpdateBulkPickerMark();
}

// Puts a text box in place of the drop-down. Enter applies the ID, Esc or leaving
// the box cancels. The tree is rebuilt afterwards, so the drop-down shows the real category again.
void ShowCustomIdInline(wuxc::TreeView const& tree, wuxc::Button const& picker, std::vector<size_t> const indices,
                        std::vector<std::wstring> const folders) {
    auto grid = picker.Parent().try_as<wuxc::Grid>();
    if (!grid) return;
    uint32_t position = 0;
    if (!grid.Children().IndexOf(picker, position)) return;

    auto done = std::make_shared<bool>(false);
    wuxc::TextBox box;
    box.PlaceholderText(L"ID (0-999)");
    box.Width(g_pickerWidth);  // The same width as the drop-down buttons above and below it.
    box.VerticalAlignment(wux::VerticalAlignment::Center);
    wuxc::Grid::SetColumn(box, 2);
    grid.Children().RemoveAt(position);
    grid.Children().InsertAt(position, box);

    // The box and the tree are reached through the event sender and a weak reference: capturing them strongly would
    // keep both alive for as long as the box exists.
    box.KeyDown([weakTree = winrt::make_weak(tree), indices, folders, done](wf::IInspectable const& sender, wux::Input::KeyRoutedEventArgs const& e) {
        auto box = sender.as<wuxc::TextBox>();
        auto tree = weakTree.get();
        if (!tree) return;
        if (e.Key() == winrt::Windows::System::VirtualKey::Escape) {
            e.Handled(true);
            if (*done) return;
            *done = true;
            RebuildXamlTree(tree);
            return;
        }
        if (e.Key() != winrt::Windows::System::VirtualKey::Enter) return;
        e.Handled(true);
        if (*done) return;
        const std::wstring typed(box.Text().c_str());
        wchar_t* end = nullptr;
        const unsigned long id = wcstoul(typed.c_str(), &end, 10);
        if (typed.empty() || (end && *end != L'\0') || id > kMaxCustomId) {
            if (g_xamlStatus) {
                g_xamlStatus.Text(IsPortuguese()
                    ? L"Digite um n\u00famero inteiro de 0 a " + std::to_wstring(kMaxCustomId) + L"."
                    : L"Type a whole number from 0 to " + std::to_wstring(kMaxCustomId) + L".");
            }
            return;
        }
        *done = true;
        ApplyRowCategory(indices, static_cast<int>(id), folders);
        RebuildXamlTree(tree);
    });

    box.LostFocus([weakTree = winrt::make_weak(tree), done](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        if (*done) return;
        *done = true;
        if (auto strongTree = weakTree.get()) RebuildXamlTree(strongTree);
    });

    box.Focus(wux::FocusState::Programmatic);
}

// Applies the bottom bar's category to every ticked app, then clears the ticks.
void ApplyTickedFromXaml(wuxc::TreeView const& tree) {
    std::vector<size_t> targets;
    std::set<std::wstring, FolderLess> folderSet;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (!g_apps[i].checked) continue;
        targets.push_back(i);
        if (!g_apps[i].folderPath.empty()) folderSet.insert(g_apps[i].folderPath);
    }
    if (targets.empty()) {
        if (g_xamlStatus) g_xamlStatus.Text(Tr(L"Tick some apps first.", L"Marque alguns apps primeiro."));
        return;
    }

    int tag = -1;
    const bool customMode = g_xamlCustomId && g_xamlCustomId.Visibility() == wux::Visibility::Visible;
    const std::wstring typed = customMode ? std::wstring(g_xamlCustomId.Text().c_str()) : std::wstring();
    if (customMode && typed.empty()) {
        if (g_xamlStatus) g_xamlStatus.Text(Tr(L"Type an ID in the box.", L"Digite um ID na caixa."));
        return;
    }
    if (customMode) {
        wchar_t* end = nullptr;
        const unsigned long id = wcstoul(typed.c_str(), &end, 10);
        if ((end && *end != L'\0') || id > kMaxCustomId) {
            if (g_xamlStatus) {
                g_xamlStatus.Text(IsPortuguese()
                    ? L"Digite um n\u00famero inteiro de 0 a " + std::to_wstring(kMaxCustomId) + L" na caixa de ID."
                    : L"Type a whole number from 0 to " + std::to_wstring(kMaxCustomId) + L" in the ID box.");
            }
            return;
        }
        tag = static_cast<int>(id);
    } else {
        tag = PickerTag(g_xamlBulkPicker);
    }
    const LONG_PTR value = tag < 0 ? static_cast<LONG_PTR>(kComboDefault) : static_cast<LONG_PTR>(tag);

    const std::vector<std::wstring> folders(folderSet.begin(), folderSet.end());
    const std::wstring msg = ApplyCategoryToTargets(targets, folders, value, true);
    if (g_xamlStatus) g_xamlStatus.Text(msg);
    RebuildXamlTree(tree);
}

// Ticks or unticks every app that is shown in the tree.
void SetVisibleXamlChecks(wuxc::TreeView const& tree, bool on) {
    for (auto root : tree.RootNodes()) {
        if (root.Children().Size() > 0) {
            for (auto child : root.Children()) CheckOf(child).IsChecked(on);
        } else {
            CheckOf(root).IsChecked(on);
        }
    }
}

// Only records the new state: every caller rebuilds the list right after, and the rebuild creates the folders
// already open or closed. Expanding the old nodes first made the tree create rows for every app (when expanding)
// just to throw them away a moment later.
void SetXamlFoldersExpanded(wuxc::TreeView const& /*tree*/, bool expand) {
    g_xamlExpanded = expand;
    g_folderExpanded.clear();  // Every folder follows the new setting.
    if (g_xamlExpand) {
        g_xamlExpand.Content(winrt::box_value(std::wstring(
            expand ? Tr(L"Collapse all", L"Recolher tudo") : Tr(L"Expand all", L"Expandir tudo"))));
    }
}

// The Check all box is ticked when every shown app is ticked.
void UpdateXamlCheckAll() {
    if (!g_xamlCheckAll || !g_xamlTree) return;
    size_t total = 0;
    size_t ticked = 0;
    for (auto root : g_xamlTree.RootNodes()) {
        if (root.Children().Size() > 0) {
            for (auto child : root.Children()) {
                ++total;
                if (IsOn(CheckOf(child))) ++ticked;
            }
        } else {
            ++total;
            if (IsOn(CheckOf(root))) ++ticked;
        }
    }
    g_xamlCheckAll.IsChecked(total > 0 && ticked == total);
}

// Fills the category filter with counts, keeping the chosen entry.
void PopulateXamlFilter() {
    if (!g_xamlFilter) return;
    const int previous = XamlFilterSelection();
    const std::vector<size_t> counts = ComputeFilterCounts();
    auto label = [](std::wstring const& name, size_t n) { return name + L"  (" + std::to_wstring(n) + L")"; };
    RefreshHeadingCounts();
    g_xamlFilterLowSet = CurrentLowSet();
    auto add = [](std::wstring const& text, int tag, bool low = false) {
        wuxc::ComboBoxItem item;
        item.Content(winrt::box_value(text));
        item.Tag(winrt::box_value(tag));
        if (low) {
            SetMarkTip(item, true);
            item.Loaded([](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
                try {
                    ColorStarInside(sender.as<wux::DependencyObject>());
                } catch (...) {
                    LogXamlHandlerFailure(L"filter item star");
                }
            });
        }
        g_xamlFilter.Items().Append(item);
    };

    g_xamlBusy = true;
    g_xamlFilter.Items().Clear();
    add(label(Tr(L"All categories", L"Todas as categorias"), counts[0]), kXamlFilterAll);
    add(label(PickerLabel(-1), counts[1]), -1);
    for (const size_t i : SortedCategoryOrder()) {
        add(label(HeadingName(static_cast<DWORD>(kCategories[i].id)), counts[2 + i]),
            static_cast<int>(kCategories[i].id), IsLowHeading(static_cast<DWORD>(kCategories[i].id)));
    }
    int selected = 0;
    for (uint32_t i = 0; i < g_xamlFilter.Items().Size(); ++i) {
        const int tag = winrt::unbox_value<int>(g_xamlFilter.Items().GetAt(i).as<wuxc::ComboBoxItem>().Tag());
        if (tag == previous) {
            selected = static_cast<int>(i);
            break;
        }
    }
    g_xamlFilter.SelectedIndex(selected);
    g_xamlBusy = false;
    g_xamlFilterCounts = counts;
    UpdateFilterForeground();
}

// Refills the filter only when a count changed.
void RefreshXamlFilterCounts() {
    if (ComputeFilterCounts() != g_xamlFilterCounts || CurrentLowSet() != g_xamlFilterLowSet) PopulateXamlFilter();
}

[[clang::no_destroy]] wuxm::RotateTransform g_refreshRotate{nullptr};  // On the refresh icon.
[[clang::no_destroy]] wuxa::Storyboard g_spinStory{nullptr};
[[clang::no_destroy]] wuxa::Storyboard g_flickerStory{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_refreshTimer{nullptr};
ULONGLONG g_lastReloadEnd = 0;
bool g_refreshPending = false;

wux::Duration MillisecondsDuration(int64_t milliseconds) {
    return wux::Duration{wf::TimeSpan{milliseconds * 10000}, wux::DurationType::TimeSpan};
}

// One turn of the refresh icon, to show the command went through.
void SpinRefreshIcon() {
    if (!g_refreshRotate) return;
    wuxa::DoubleAnimation spin;
    spin.From(0.0);
    spin.To(360.0);
    spin.Duration(MillisecondsDuration(550));
    wuxa::CubicEase ease;
    ease.EasingMode(wuxa::EasingMode::EaseInOut);
    spin.EasingFunction(ease);
    wuxa::Storyboard::SetTarget(spin, g_refreshRotate);
    wuxa::Storyboard::SetTargetProperty(spin, L"Angle");
    wuxa::Storyboard story;
    story.Children().Append(spin);
    g_spinStory = story;
    story.Begin();
}

// The status text fades out and back once, so a refresh is noticed even when the text stays the same.
void FlickerStatus() {
    if (!g_xamlStatus) return;
    wuxa::DoubleAnimation blink;
    blink.From(1.0);
    blink.To(0.0);
    blink.Duration(MillisecondsDuration(140));
    blink.AutoReverse(true);
    wuxa::Storyboard::SetTarget(blink, g_xamlStatus);
    wuxa::Storyboard::SetTargetProperty(blink, L"Opacity");
    wuxa::Storyboard story;
    story.Children().Append(blink);
    g_flickerStory = story;
    story.Begin();
}

// Reloads the app list (the refresh button and the F5 key). The icon starts turning first and the reload runs a
// moment later, because a reload blocks the window while it works. Clicks and key presses that pile up meanwhile are
// delivered right after it finishes, so anything within 1.5 s of the last reload is ignored: a fast double click no
// longer queues several reloads in a row (which froze the window).
void RefreshXamlList(wuxc::TreeView const& tree) {
    if (g_refreshPending) return;
    if (g_lastReloadEnd != 0 && GetTickCount64() - g_lastReloadEnd < 1500) return;
    g_refreshPending = true;
    SpinRefreshIcon();
    wux::DispatcherTimer timer;
    timer.Interval(wf::TimeSpan{600000});  // 60 ms
    timer.Tick([weakTree = winrt::make_weak(tree)](wf::IInspectable const& sender, wf::IInspectable const&) {
        auto self = sender.as<wux::DispatcherTimer>();
        self.Stop();
        try {
            if (!g_xamlClosing) {
                if (auto strongTree = weakTree.get()) {
                    LoadEntries();
                    PopulateXamlFilter();
                    RebuildXamlTree(strongTree);
                }
            }
        } catch (...) {
            LogXamlHandlerFailure(L"refresh");
        }
        g_lastReloadEnd = GetTickCount64();
        g_refreshPending = false;
        g_refreshTimer = nullptr;
        if (!g_xamlClosing) FlickerStatus();
    });
    g_refreshTimer = timer;  // Kept alive until it has fired.
    timer.Start();
}

// ---------------------------------------------------------------------------
// Custom title bar
// ---------------------------------------------------------------------------
// The manager window has no Windows-drawn title bar: Windows paints it outside the XAML content, so the window
// tint could never reach it. The XAML content draws the title and the three caption buttons instead, which puts
// the tint over the whole window. Windows still handles moving, resizing, snapping and double-click maximise
// (through WM_NCHITTEST below).
constexpr int kTitleBarHeight = 32;      // DIPs
constexpr int kCaptionButtonWidth = 46;  // DIPs
constexpr int kResizeBorder = 6;         // DIPs
constexpr int kMinWindowWidth = 560;     // DIPs: the widest single control plus margins; the rows wrap below this
constexpr int kMinWindowHeight = 480;    // DIPs: room for wrapped toolbars and a few rows of the list

[[clang::no_destroy]] wuxc::TextBlock g_xamlMaxGlyph{nullptr};  // The maximise / restore glyph, updated when the window changes state.

int ScaleForWindow(HWND hwnd, int value) {
    return MulDiv(value, static_cast<int>(GetDpiForWindow(hwnd)), 96);
}

// ---------------------------------------------------------------------------
// Snap layouts (the layout chooser that Windows 11 shows over a maximise button)
// ---------------------------------------------------------------------------
// Windows shows it when the window under the mouse answers "this is the maximise button" to the hit test
// (WM_NCHITTEST). The XAML island covers the whole window and answers first, so a small invisible window of our own
// is laid over the maximise button (like the drag bar over the title strip) and gives that answer. XAML then never
// sees the mouse over that button, so this window handles its hover, press and click from the non-client mouse
// messages. If Windows ignores it, nothing is lost but the layout chooser: the XAML button underneath is unchanged.
winrt::Windows::UI::Color CaptionHoverColor(bool isClose);  // Defined with the caption buttons, further down.
[[clang::no_destroy]] wuxc::Border g_maxBorder{nullptr};
bool g_maxHot = false;
bool g_maxPressed = false;
HWND g_maxBar = nullptr;
std::wstring g_maxBarClass = L"WindhawkStartCategoryManager.MaxBar";
bool g_maxBarClassRegistered = false;

void SetMaxButtonHot(bool hot) {
    if (hot == g_maxHot) return;
    g_maxHot = hot;
    try {
        if (g_maxBorder && !g_xamlClosing) {
            g_maxBorder.Background(wuxm::SolidColorBrush(hot ? CaptionHoverColor(false)
                                                              : winrt::Windows::UI::Colors::Transparent()));
        }
    } catch (...) {
        LogXamlHandlerFailure(L"maximise button hover");
    }
}

LRESULT CALLBACK MaxBarWndProc(HWND bar, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTMAXBUTTON;
        case WM_NCMOUSEMOVE: {
            if (g_xamlClosing) return 0;
            SetMaxButtonHot(true);
            TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE | TME_NONCLIENT, bar, 0};
            TrackMouseEvent(&track);
            return 0;
        }
        case WM_NCMOUSELEAVE:
            SetMaxButtonHot(false);
            g_maxPressed = false;
            return 0;
        case WM_NCLBUTTONDOWN:
            g_maxPressed = true;
            return 0;
        case WM_NCLBUTTONUP: {
            const bool clicked = g_maxPressed;
            g_maxPressed = false;
            HWND top = GetParent(bar);
            if (clicked && top && !g_xamlClosing) {
                PostMessageW(top, WM_SYSCOMMAND, IsZoomed(top) ? SC_RESTORE : SC_MAXIMIZE, 0);
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(bar, &ps);
            EndPaint(bar, &ps);
            return 0;
        }
    }
    return DefWindowProcW(bar, msg, wParam, lParam);
}

// Sizes the overlay to the maximise button: the middle one of the three, below the top resize strip.
void PositionMaxBar(HWND top) {
    if (!g_maxBar) return;
    RECT rc{};
    GetClientRect(top, &rc);
    const int border = IsZoomed(top) ? 0 : ScaleForWindow(top, kResizeBorder);
    const int buttonWidth = ScaleForWindow(top, kCaptionButtonWidth);
    const int height = ScaleForWindow(top, kTitleBarHeight) - border;
    SetWindowPos(g_maxBar, HWND_TOP, rc.right - 2 * buttonWidth, border, buttonWidth, (std::max)(height, 0),
                 SWP_NOACTIVATE);
}

void CreateMaxBar(HWND top) {
    if (!g_maxBarClassRegistered) {
        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = MaxBarWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = g_maxBarClass.c_str();
        g_maxBarClassRegistered = RegisterClassExW(&wc) != 0;
    }
    if (!g_maxBarClassRegistered) {
        Wh_Log(L"[xaml] could not register the maximise bar class (%lu)", GetLastError());
        return;
    }
    // The same kind of window as the drag bar: layered, fully opaque, no drawing surface. Invisible, but it still
    // receives the mouse.
    g_maxBar = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP, g_maxBarClass.c_str(), L"",
                               WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, top, nullptr, ModuleInstance(), nullptr);
    if (!g_maxBar) {
        Wh_Log(L"[xaml] could not create the maximise bar (%lu)", GetLastError());
        return;
    }
    SetLayeredWindowAttributes(g_maxBar, 0, 255, LWA_ALPHA);
    PositionMaxBar(top);
}

// Decides what each part of the window does. The client area is the whole window.
LRESULT TitleBarHitTest(HWND hwnd, LPARAM lParam) {
    POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
    ScreenToClient(hwnd, &pt);
    RECT rc{};
    GetClientRect(hwnd, &rc);
    const int border = IsZoomed(hwnd) ? 0 : ScaleForWindow(hwnd, kResizeBorder);
    const bool left = pt.x < border;
    const bool right = pt.x >= rc.right - border;
    const bool top = pt.y < border;
    const bool bottom = pt.y >= rc.bottom - border;
    if (top && left) return HTTOPLEFT;
    if (top && right) return HTTOPRIGHT;
    if (bottom && left) return HTBOTTOMLEFT;
    if (bottom && right) return HTBOTTOMRIGHT;
    if (left) return HTLEFT;
    if (right) return HTRIGHT;
    if (top) return HTTOP;
    if (bottom) return HTBOTTOM;
    if (pt.y < ScaleForWindow(hwnd, kTitleBarHeight)) {
        // The three caption buttons are XAML controls; the rest of the strip drags the window.
        if (pt.x >= rc.right - 3 * ScaleForWindow(hwnd, kCaptionButtonWidth)) return HTCLIENT;
        return HTCAPTION;
    }
    return HTCLIENT;
}

// The drag bar: a see-through child window laid over the title strip, between the window edge and the caption
// buttons. The XAML island swallows the mouse, so without it Windows never sees a press on the title bar and cannot
// start its own move loop, which is what provides snapping (screen edges, corners, top edge to maximise) and
// dragging a maximised window back down. Pressing it starts Windows' real move from this plain window procedure,
// not from a XAML event, which is what made the move loop crash before.
HWND g_dragBar = nullptr;
std::wstring g_dragBarClass = L"WindhawkStartCategoryManager.DragBar";
bool g_dragBarClassRegistered = false;

// An icon for a menu item: a glyph of the Segoe icon font, drawn in the colour of the menu text. The bitmap is a
// 32-bit image with transparency; the brightness of the drawn glyph becomes its opacity.
HBITMAP MakeMenuGlyphBitmap(HWND hwnd, const wchar_t* glyph, bool enabled) {
    const int size = ScaleForWindow(hwnd, 16);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;  // Top-down.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!dc || !bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    std::memset(bits, 0, static_cast<size_t>(size) * size * 4);
    const int fontHeight = -MulDiv(size, 10, 16);
    HFONT font = CreateFontW(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe Fluent Icons");
    wchar_t face[LF_FACESIZE] = {};
    HGDIOBJ oldFont = SelectObject(dc, font);
    GetTextFaceW(dc, LF_FACESIZE, face);
    if (_wcsicmp(face, L"Segoe Fluent Icons") != 0) {
        // Windows 10: the older icon font.
        SelectObject(dc, oldFont);
        DeleteObject(font);
        font = CreateFontW(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe MDL2 Assets");
        oldFont = SelectObject(dc, font);
    }
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    RECT area{0, 0, size, size};
    DrawTextW(dc, glyph, -1, &area, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    GdiFlush();
    const uint32_t shade = g_isDark ? 255 : 0;  // The menu text colour.
    auto* pixels = static_cast<uint32_t*>(bits);
    for (int i = 0; i < size * size; ++i) {
        uint32_t alpha = pixels[i] & 0xFF;  // The drawn glyph is white on black, so any channel is its coverage.
        if (!enabled) alpha = alpha * 40 / 100;
        const uint32_t premultiplied = shade * alpha / 255;
        pixels[i] = (alpha << 24) | (premultiplied << 16) | (premultiplied << 8) | premultiplied;
    }
    SelectObject(dc, oldFont);
    DeleteObject(font);
    SelectObject(dc, oldBitmap);
    DeleteDC(dc);
    return bitmap;
}

// The window has no system menu of its own (it would make Windows draw its own caption buttons), so the same menu
// is built here from the standard commands. Used for a right click on the title bar and for Alt+Space.
void ShowWindowMenu(HWND top, int x, int y) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    const bool zoomed = IsZoomed(top) != FALSE;
    const UINT zoomedOnly = zoomed ? MF_ENABLED : MF_GRAYED;
    const UINT normalOnly = zoomed ? MF_GRAYED : MF_ENABLED;
    AppendMenuW(menu, MF_STRING | zoomedOnly, SC_RESTORE, Tr(L"Restore", L"Restaurar"));
    AppendMenuW(menu, MF_STRING | normalOnly, SC_MOVE, Tr(L"Move", L"Mover"));
    AppendMenuW(menu, MF_STRING | normalOnly, SC_SIZE, Tr(L"Size", L"Tamanho"));
    AppendMenuW(menu, MF_STRING, SC_MINIMIZE, Tr(L"Minimize", L"Minimizar"));
    AppendMenuW(menu, MF_STRING | normalOnly, SC_MAXIMIZE, Tr(L"Maximize", L"Maximizar"));
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, SC_CLOSE, Tr(L"Close\tAlt+F4", L"Fechar\tAlt+F4"));
    // Icons, as in the menu of other windows. Move and Size have none there either.
    std::vector<HBITMAP> bitmaps;
    auto setIcon = [&](UINT command, const wchar_t* glyph, bool enabled) {
        if (HBITMAP bitmap = MakeMenuGlyphBitmap(top, glyph, enabled)) {
            MENUITEMINFOW item{};
            item.cbSize = sizeof(item);
            item.fMask = MIIM_BITMAP;
            item.hbmpItem = bitmap;
            SetMenuItemInfoW(menu, command, FALSE, &item);
            bitmaps.push_back(bitmap);
        }
    };
    setIcon(SC_RESTORE, L"\xE923", zoomed);
    setIcon(SC_MINIMIZE, L"\xE921", true);
    setIcon(SC_MAXIMIZE, L"\xE922", !zoomed);
    setIcon(SC_CLOSE, L"\xE8BB", true);
    SetForegroundWindow(top);
    const int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, x, y, 0, top, nullptr);
    DestroyMenu(menu);
    for (HBITMAP bitmap : bitmaps) DeleteObject(bitmap);
    if (command) PostMessageW(top, WM_SYSCOMMAND, command, 0);
}

LRESULT CALLBACK DragBarWndProc(HWND bar, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND top = GetParent(bar);
    switch (msg) {
        case WM_LBUTTONDOWN: {
            if (!top) break;
            POINT pt{};
            GetCursorPos(&pt);
            ReleaseCapture();
            SendMessageW(top, WM_NCLBUTTONDOWN, HTCAPTION,
                         MAKELPARAM(static_cast<WORD>(pt.x), static_cast<WORD>(pt.y)));
            return 0;
        }
        case WM_LBUTTONDBLCLK: {
            if (!top) break;
            PostMessageW(top, WM_SYSCOMMAND, IsZoomed(top) ? SC_RESTORE : SC_MAXIMIZE, 0);  // Press twice.
            return 0;
        }
        case WM_RBUTTONUP: {
            if (!top) break;
            POINT pt{};
            GetCursorPos(&pt);
            ShowWindowMenu(top, pt.x, pt.y);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(bar, &ps);
            EndPaint(bar, &ps);
            return 0;
        }
    }
    return DefWindowProcW(bar, msg, wParam, lParam);
}

// Sizes the drag bar to the title strip: below the top resize strip, clear of the three caption buttons.
void PositionDragBar(HWND top) {
    if (!g_dragBar) return;
    RECT rc{};
    GetClientRect(top, &rc);
    const int border = IsZoomed(top) ? 0 : ScaleForWindow(top, kResizeBorder);
    const int height = ScaleForWindow(top, kTitleBarHeight) - border;
    const int width = rc.right - 2 * border - 3 * ScaleForWindow(top, kCaptionButtonWidth);
    SetWindowPos(g_dragBar, HWND_TOP, border, border, (std::max)(width, 0), (std::max)(height, 0), SWP_NOACTIVATE);
}

void CreateDragBar(HWND top) {
    if (!g_dragBarClassRegistered) {
        WNDCLASSEXW wc{sizeof(wc)};
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = DragBarWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = g_dragBarClass.c_str();
        g_dragBarClassRegistered = RegisterClassExW(&wc) != 0;
    }
    if (!g_dragBarClassRegistered) {
        Wh_Log(L"[xaml] could not register the drag bar class (%lu)", GetLastError());
        return;
    }
    // Layered, fully opaque, with no drawing surface: invisible, but it still receives the mouse.
    g_dragBar = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP, g_dragBarClass.c_str(), L"",
                                WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, top, nullptr, ModuleInstance(), nullptr);
    if (!g_dragBar) {
        Wh_Log(L"[xaml] could not create the drag bar (%lu)", GetLastError());
        return;
    }
    SetLayeredWindowAttributes(g_dragBar, 0, 255, LWA_ALPHA);
    PositionDragBar(top);
}

// Moving and resizing the window by hand. The XAML island swallows the mouse, so Windows' own move and size loop cannot
// be used (it also crashes when started from a XAML event). Instead a XAML element captures the pointer and the window
// follows the cursor in screen coordinates.
struct ManualDrag {
    bool active = false;
    int hitTest = 0;  // HTCAPTION to move, HTLEFT, HTTOPLEFT and so on to resize.
    POINT startCursor{};
    RECT startRect{};
};
ManualDrag g_drag;

void BeginManualDrag(HWND hwnd, int hitTest) {
    g_drag.active = true;
    g_drag.hitTest = hitTest;
    GetCursorPos(&g_drag.startCursor);
    GetWindowRect(hwnd, &g_drag.startRect);
}

void UpdateManualDrag(HWND hwnd) {
    if (!g_drag.active) return;
    POINT now{};
    GetCursorPos(&now);
    const int dx = now.x - g_drag.startCursor.x;
    const int dy = now.y - g_drag.startCursor.y;
    RECT r = g_drag.startRect;
    if (g_drag.hitTest == HTCAPTION) {
        OffsetRect(&r, dx, dy);
    } else {
        const int hit = g_drag.hitTest;
        const int minWidth = ScaleForWindow(hwnd, kMinWindowWidth);
        const int minHeight = ScaleForWindow(hwnd, kMinWindowHeight);
        if (hit == HTLEFT || hit == HTTOPLEFT || hit == HTBOTTOMLEFT) r.left = (std::min)(r.left + dx, r.right - minWidth);
        if (hit == HTRIGHT || hit == HTTOPRIGHT || hit == HTBOTTOMRIGHT) r.right = (std::max)(r.right + dx, r.left + minWidth);
        if (hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT) r.top = (std::min)(r.top + dy, r.bottom - minHeight);
        if (hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT) r.bottom = (std::max)(r.bottom + dy, r.top + minHeight);
    }
    SetWindowPos(hwnd, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
}

// Makes a XAML element move or resize the window while the left button is held on it. `accept` says whether this
// press should start it (it can also do something else with the press, such as maximise).
void AttachManualDrag(wux::UIElement const& element, HWND hwnd, int hitTest,
                      std::function<bool(wux::UIElement const&, wuxi::PointerRoutedEventArgs const&)> accept) {
    element.PointerPressed([hwnd, hitTest, accept](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
        auto self = sender.as<wux::UIElement>();
        if (!e.GetCurrentPoint(self).Properties().IsLeftButtonPressed()) return;
        if (!accept(self, e)) return;
        if (self.CapturePointer(e.Pointer())) {
            BeginManualDrag(hwnd, hitTest);
            e.Handled(true);
        }
    });
    element.PointerMoved([hwnd](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
        if (g_drag.active) UpdateManualDrag(hwnd);
    });
    element.PointerReleased([](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
        if (!g_drag.active) return;
        g_drag.active = false;
        sender.as<wux::UIElement>().ReleasePointerCapture(e.Pointer());
    });
    element.PointerCaptureLost([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) { g_drag.active = false; });
}

// The colours that depend on the light / dark mode are asked for when they are used, never kept, so a switch of the
// mode only has to update the few brushes below (see ApplyThemeLive).
winrt::Windows::UI::Color CaptionNormalColor() {
    const uint8_t shade = g_isDark ? 255 : 0;
    return winrt::Windows::UI::Color{255, shade, shade, shade};
}
winrt::Windows::UI::Color CaptionHoverColor(bool isClose) {
    const uint8_t shade = g_isDark ? 255 : 0;
    return isClose ? winrt::Windows::UI::Color{255, 232, 17, 35}
                   : winrt::Windows::UI::Color{static_cast<uint8_t>(g_isDark ? 28 : 22), shade, shade, shade};
}
// A subtle tint over the backdrop: AMOLED black in dark mode, soft white in light mode. Translucent on purpose, so
// the blur stays visible. Tune the alpha (first value, 0-255) to taste.
winrt::Windows::UI::Color WindowTintColor() {
    return g_isDark ? winrt::Windows::UI::Color{157, 0, 0, 0} : winrt::Windows::UI::Color{153, 243, 243, 243};
}

// The parts of the window that are kept so the mode can change under an open window.
[[clang::no_destroy]] wuxc::Grid g_xamlRoot{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_tintBrush{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_listBrush{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_titleText{nullptr};
[[clang::no_destroy]] std::vector<wuxc::TextBlock> g_captionGlyphs;

// ---------------------------------------------------------------------------
// Confirmation dialog
// ---------------------------------------------------------------------------
// A Fluent-style confirmation dialog (a XAML ContentDialog) shown over the manager window. It does not wait: the
// answer arrives as a posted message (WM_APP_CONFIRM_DONE), and the action runs a moment later from a timer, once the
// dialog has finished closing. Waiting in a nested message loop inside the click handler crashed Explorer. If the
// dialog cannot be shown, the old message box is used instead. "No" is the default button, as before.
struct PendingConfirm {
    wuxc::ContentDialog dialog{nullptr};
    wf::IAsyncOperation<wuxc::ContentDialogResult> operation{nullptr};
    wuxc::ContentDialogResult result = wuxc::ContentDialogResult::None;
    std::function<void()> onYes;
};
[[clang::no_destroy]] std::shared_ptr<PendingConfirm> g_pendingConfirm;
[[clang::no_destroy]] std::function<void()> g_confirmedAction;

void ConfirmAsync(const wchar_t* titleEn, const wchar_t* titlePt, const wchar_t* en, const wchar_t* pt, UINT icon,
                  std::function<void()> onYes, bool infoOnly) {
    const wchar_t* title = Tr(titleEn, titlePt);  // The name of the action, not of the window.
    const wchar_t* text = Tr(en, pt);
    if (g_pendingConfirm) return;  // A question is already open.
    auto fallback = [&]() {
        if (infoOnly) {
            MessageBoxW(g_xamlListWindow, text, title, MB_OK | icon);
            return;
        }
        if (MessageBoxW(g_xamlListWindow, text, title, MB_YESNO | icon | MB_DEFBUTTON2) == IDYES && onYes) onYes();
    };
    if (!g_xamlRoot || g_xamlClosing || !g_xamlListWindow) return fallback();
    try {
        wuxc::FontIcon symbol;
        symbol.Glyph(icon == MB_ICONWARNING ? L"\xE7BA" : L"\xE897");  // Warning / Help
        symbol.FontSize(28);
        symbol.VerticalAlignment(wux::VerticalAlignment::Top);
        if (icon == MB_ICONWARNING) {
            symbol.Foreground(wuxm::SolidColorBrush(winrt::Windows::UI::Color{255, 252, 189, 0}));
        }

        wuxc::TextBlock body;
        body.Text(text);
        body.TextWrapping(wux::TextWrapping::Wrap);
        body.MaxWidth(380);

        wuxc::StackPanel content;
        content.Orientation(wuxc::Orientation::Horizontal);
        content.Spacing(16);
        content.Children().Append(symbol);
        content.Children().Append(body);

        auto pending = std::make_shared<PendingConfirm>();
        pending->onYes = std::move(onYes);
        pending->dialog = wuxc::ContentDialog();
        wuxc::ContentDialog dialog = pending->dialog;
        dialog.Title(winrt::box_value(winrt::hstring(title)));
        dialog.Content(content);
        if (!infoOnly) dialog.PrimaryButtonText(Tr(L"Yes", L"Sim"));
        dialog.CloseButtonText(infoOnly ? L"OK" : Tr(L"No", L"N\u00e3o"));
        dialog.DefaultButton(wuxc::ContentDialogButton::Close);
        dialog.RequestedTheme(g_isDark ? wux::ElementTheme::Dark : wux::ElementTheme::Light);
        dialog.XamlRoot(g_xamlRoot.XamlRoot());

        g_pendingConfirm = pending;
        const HWND window = g_xamlListWindow;
        pending->operation = dialog.ShowAsync();
        // The callback only records the answer and posts a message; it must not touch the dialog or the window.
        pending->operation.Completed([pending, window](wf::IAsyncOperation<wuxc::ContentDialogResult> const& info,
                                                       wf::AsyncStatus status) {
            if (status == wf::AsyncStatus::Completed) {
                try {
                    pending->result = info.GetResults();
                } catch (...) {
                }
            }
            PostMessageW(window, WM_APP_CONFIRM_DONE, 0, 0);
        });
    } catch (...) {
        g_pendingConfirm = nullptr;
        LogXamlHandlerFailure(L"confirm dialog");
        fallback();
    }
}

// Runs on WM_APP_CONFIRM_DONE: lets go of the dialog and, for "Yes", schedules the action.
void FinishConfirm(HWND window) {
    std::shared_ptr<PendingConfirm> pending = std::move(g_pendingConfirm);
    g_pendingConfirm = nullptr;
    if (!pending) return;
    const bool yes = pending->result == wuxc::ContentDialogResult::Primary;
    std::function<void()> action = std::move(pending->onYes);
    pending->operation = nullptr;  // Breaks the loop between the operation, its callback and this state.
    pending->dialog = nullptr;
    if (yes && action && !g_xamlClosing) {
        g_confirmedAction = std::move(action);
        SetTimer(window, kConfirmTimerId, 200, nullptr);  // After the dialog has finished closing.
    }
}

void RunConfirmedAction(HWND window) {
    KillTimer(window, kConfirmTimerId);
    std::function<void()> action = std::move(g_confirmedAction);
    g_confirmedAction = nullptr;
    if (!action || g_xamlClosing) return;
    try {
        action();
    } catch (...) {
        LogXamlHandlerFailure(L"confirmed action");
    }
}

wuxc::Border MakeCaptionButton(const wchar_t* glyph, bool isClose, std::function<void()> onClick,
                               wuxc::TextBlock* glyphOut) {
    namespace wuxi = winrt::Windows::UI::Xaml::Input;
    const winrt::Windows::UI::Color normal = CaptionNormalColor();
    wuxc::TextBlock text;
    text.Text(glyph);
    text.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    text.FontSize(10);
    text.Foreground(wuxm::SolidColorBrush(normal));
    text.HorizontalAlignment(wux::HorizontalAlignment::Center);
    text.VerticalAlignment(wux::VerticalAlignment::Center);
    text.IsHitTestVisible(false);
    if (glyphOut) *glyphOut = text;
    g_captionGlyphs.push_back(text);

    wuxc::Border border;
    border.Width(kCaptionButtonWidth);
    border.Height(kTitleBarHeight);
    border.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));  // Transparent, but it still gets the mouse.
    border.Child(text);
    border.PointerEntered([isClose](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const&) {
        auto self = sender.as<wuxc::Border>();
        self.Background(wuxm::SolidColorBrush(CaptionHoverColor(isClose)));
        if (isClose) {
            self.Child().as<wuxc::TextBlock>().Foreground(
                wuxm::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 255, 255}));
        }
    });
    border.PointerExited([](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const&) {
        auto self = sender.as<wuxc::Border>();
        self.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
        self.Child().as<wuxc::TextBlock>().Foreground(wuxm::SolidColorBrush(CaptionNormalColor()));
    });
    border.Tapped([onClick](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const&) { onClick(); });
    return border;
}

// The cursor over the resize strips.
void SetManagerCursor(LPCWSTR cursorId, winrt::Windows::UI::Core::CoreCursorType type) {
    (void)type;
    SetCursor(LoadCursorW(nullptr, cursorId));
}

// An invisible strip along a window edge or corner. Pressing it and dragging resizes the window.
wuxc::Border MakeResizeEdge(HWND hwnd, wux::HorizontalAlignment horizontal, wux::VerticalAlignment vertical,
                            double width, double height, int hitTest, LPCWSTR cursorId,
                            winrt::Windows::UI::Core::CoreCursorType cursorType) {
    wuxc::Border edge;
    edge.HorizontalAlignment(horizontal);
    edge.VerticalAlignment(vertical);
    if (width > 0) edge.Width(width);
    if (height > 0) edge.Height(height);
    edge.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));  // Transparent, but it still gets the mouse.
    auto setCursor = [hwnd, cursorId, cursorType](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
        if (!IsZoomed(hwnd)) SetManagerCursor(cursorId, cursorType);
    };
    edge.PointerEntered(setCursor);
    edge.PointerMoved(setCursor);
    edge.PointerExited([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
        if (!g_drag.active) SetManagerCursor(IDC_ARROW, winrt::Windows::UI::Core::CoreCursorType::Arrow);
    });
    AttachManualDrag(edge, hwnd, hitTest,
                     [hwnd](wux::UIElement const&, wuxi::PointerRoutedEventArgs const&) { return !IsZoomed(hwnd); });
    return edge;
}

// The root layout with the eight resize strips over it.
wuxc::Grid AddResizeEdges(HWND hwnd, wux::UIElement const& content) {
    using Cursor = winrt::Windows::UI::Core::CoreCursorType;
    const double side = 6;     // DIPs
    const double corner = 10;  // DIPs
    wuxc::Grid outer;
    outer.Children().Append(content);
    const auto left = wux::HorizontalAlignment::Left;
    const auto right = wux::HorizontalAlignment::Right;
    const auto hStretch = wux::HorizontalAlignment::Stretch;
    const auto top = wux::VerticalAlignment::Top;
    const auto bottom = wux::VerticalAlignment::Bottom;
    const auto vStretch = wux::VerticalAlignment::Stretch;
    outer.Children().Append(MakeResizeEdge(hwnd, left, vStretch, side, 0, HTLEFT, IDC_SIZEWE, Cursor::SizeWestEast));
    outer.Children().Append(MakeResizeEdge(hwnd, right, vStretch, side, 0, HTRIGHT, IDC_SIZEWE, Cursor::SizeWestEast));
    outer.Children().Append(MakeResizeEdge(hwnd, hStretch, top, 0, 4, HTTOP, IDC_SIZENS, Cursor::SizeNorthSouth));
    outer.Children().Append(MakeResizeEdge(hwnd, hStretch, bottom, 0, side, HTBOTTOM, IDC_SIZENS, Cursor::SizeNorthSouth));
    outer.Children().Append(MakeResizeEdge(hwnd, left, top, corner, corner, HTTOPLEFT, IDC_SIZENWSE, Cursor::SizeNorthwestSoutheast));
    outer.Children().Append(MakeResizeEdge(hwnd, right, top, corner, corner, HTTOPRIGHT, IDC_SIZENESW, Cursor::SizeNortheastSouthwest));
    outer.Children().Append(MakeResizeEdge(hwnd, left, bottom, corner, corner, HTBOTTOMLEFT, IDC_SIZENESW, Cursor::SizeNortheastSouthwest));
    outer.Children().Append(MakeResizeEdge(hwnd, right, bottom, corner, corner, HTBOTTOMRIGHT, IDC_SIZENWSE, Cursor::SizeNorthwestSoutheast));
    return outer;
}

// Follows a switch between light and dark the way the Settings app does: nothing is closed or rebuilt as a whole.
// The theme of the XAML tree is changed, which updates every themed control by itself, and the few colours this
// window chooses itself are updated in place. The rows are drawn again because each card carries its own colour.
void ApplyThemeLive(HWND hwnd) {
    g_isDark = SystemUsesDarkApps();
    const auto theme = g_isDark ? wux::ElementTheme::Dark : wux::ElementTheme::Light;
    ApplyDarkTitleBar(hwnd);  // Also switches the acrylic behind the window.
    if (g_xamlRoot) g_xamlRoot.RequestedTheme(theme);
    if (g_xamlTree) g_xamlTree.RequestedTheme(theme);
    if (g_tintBrush) g_tintBrush.Color(WindowTintColor());
    if (g_listBrush) g_listBrush.Color(ListColor());
    if (g_titleText) g_titleText.Foreground(wuxm::SolidColorBrush(CaptionNormalColor()));
    for (auto const& glyph : g_captionGlyphs) glyph.Foreground(wuxm::SolidColorBrush(CaptionNormalColor()));
    if (g_xamlTree) RebuildXamlTree(g_xamlTree);
}

// ---------------------------------------------------------------------------
// Manager window procedure
// ---------------------------------------------------------------------------
LRESULT CALLBACK XamlTreeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            g_xamlClosing = false;
            try {
                if (!g_xamlManager) g_xamlManager = wuxh::WindowsXamlManager::InitializeForCurrentThread();
                MakeXamlIslandsTransparent();
                g_xamlTreeSource = wuxh::DesktopWindowXamlSource{};
                const IID nativeIid = {0x3CBCF1BF, 0x2F76, 0x4E9C, {0x96, 0xAB, 0xE8, 0x4B, 0x37, 0x97, 0x25, 0x54}};
                IDesktopWindowXamlSourceNative* native = nullptr;
                winrt::check_hresult(winrt::get_unknown(g_xamlTreeSource)->QueryInterface(nativeIid, reinterpret_cast<void**>(&native)));
                native->AttachToWindow(hwnd);
                native->get_WindowHandle(&g_xamlTreeChild);
                native->Release();

                wuxc::TreeView tree;
                tree.Margin(wux::Thickness{2, 12, 12, 12});
                tree.RequestedTheme(g_isDark ? wux::ElementTheme::Dark : wux::ElementTheme::Light);
                // Without a template the tree shows each node's type name. This template shows each node's content as a layout.
                auto rowTemplate = winrt::Windows::UI::Xaml::Markup::XamlReader::Load(
                    L"<DataTemplate xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
                    L"<ContentPresenter Content='{Binding Content}'/></DataTemplate>");
                tree.ItemTemplate(rowTemplate.as<wux::DataTemplate>());
                wux::Style rowStyle;
                winrt::Windows::UI::Xaml::Interop::TypeName itemType;
                itemType.Name = L"Windows.UI.Xaml.Controls.TreeViewItem";
                itemType.Kind = winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata;
                rowStyle.TargetType(itemType);
                rowStyle.Setters().Append(wux::Setter(wuxc::Control::HorizontalContentAlignmentProperty(),
                                                      winrt::box_value(wux::HorizontalAlignment::Stretch)));
                // The tree's own expand button is hidden; each folder row draws its own in the gap left of the card.
                rowStyle.Setters().Append(wux::Setter(wuxc::TreeViewItem::GlyphOpacityProperty(), winrt::box_value(0.0)));
                rowStyle.Setters().Append(wux::Setter(wuxc::TreeViewItem::GlyphBrushProperty(),
                                                      winrt::box_value(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()))));
                rowStyle.Setters().Append(wux::Setter(wuxc::TreeViewItem::CollapsedGlyphProperty(), winrt::box_value(winrt::hstring(L" "))));
                rowStyle.Setters().Append(wux::Setter(wuxc::TreeViewItem::ExpandedGlyphProperty(), winrt::box_value(winrt::hstring(L" "))));
                tree.ItemContainerStyle(rowStyle);
                // The tree's own hover, press and selection highlights are as wide as the whole list; the cards draw their own.
                {
                    const auto none = wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent());
                    for (const wchar_t* key : {L"TreeViewItemBackground", L"TreeViewItemBackgroundPointerOver",
                                               L"TreeViewItemBackgroundPressed", L"TreeViewItemBackgroundSelected",
                                               L"TreeViewItemBackgroundSelectedPointerOver",
                                               L"TreeViewItemBackgroundSelectedPressed", L"TreeViewItemBackgroundDisabled",
                                               L"TreeViewItemBackgroundSelectedDisabled"}) {
                        tree.Resources().Insert(winrt::box_value(winrt::hstring(key)), none);
                    }
                }
                tree.Expanding([](wuxc::TreeView const&, wuxc::TreeViewExpandingEventArgs const& e) {
                    try {
                        SetFolderChevron(e.Node(), true);
                    } catch (...) {
                        LogXamlHandlerFailure(L"Expanding");
                    }
                });
                tree.Collapsed([](wuxc::TreeView const&, wuxc::TreeViewCollapsedEventArgs const& e) {
                    try {
                        SetFolderChevron(e.Node(), false);
                    } catch (...) {
                        LogXamlHandlerFailure(L"Collapsed");
                    }
                });
                g_xamlTree = tree;
                tree.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
                BuildXamlTree(tree);
                tree.LayoutUpdated([weakTree = winrt::make_weak(tree)](wf::IInspectable const&, wf::IInspectable const&) {
                    try {
                        if (auto strongTree = weakTree.get()) {
                            UpdateRowWidths(strongTree);
                            HookTreeScrollBar(strongTree);
                        }
                    } catch (...) {
                        LogXamlHandlerFailure(L"LayoutUpdated");
                    }
                });
                // The bottom bar: a category for the ticked apps, Apply, and the other tools.
                g_xamlBulkPicker = MakePicker(false, 0, false, [](int) { SetCustomIdBoxVisible(false); },
                                              [](wuxc::Button const& picker) {
                    picker.Content(winrt::box_value(std::wstring(Tr(L"Custom ID...", L"ID personalizado..."))));
                    SetCustomIdBoxVisible(true);
                    if (g_xamlCustomId) {
                        g_xamlCustomId.UpdateLayout();
                        g_xamlCustomId.Focus(wux::FocusState::Programmatic);
                    }
                }, false);  // The bottom drop-down only fills the box for Apply: no waiting after the wheel.

                wuxc::Button applyButton;
                applyButton.Content(winrt::box_value(std::wstring(Tr(L"Apply to ticked apps", L"Aplicar aos marcados"))));
                applyButton.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    ApplyTickedFromXaml(tree);
                });

                wuxc::Button resetButton;
                resetButton.Content(winrt::box_value(std::wstring(Tr(L"Reset all", L"Redefinir tudo"))));
                resetButton.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    ResetAllCategories([tree]() { RebuildXamlTree(tree); });
                });

                wuxc::Button importButton;
                importButton.Content(winrt::box_value(std::wstring(Tr(L"Import", L"Importar"))));
                importButton.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    ImportCategories([tree]() { RebuildXamlTree(tree); });
                });

                wuxc::Button exportButton;
                exportButton.Content(winrt::box_value(std::wstring(Tr(L"Export", L"Exportar"))));
                exportButton.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    ExportCategories(nullptr);
                });

                g_xamlStatus = wuxc::TextBlock{};
                g_xamlStatus.VerticalAlignment(wux::VerticalAlignment::Center);

                wuxc::StackPanel bar;
                bar.Orientation(wuxc::Orientation::Vertical);
                bar.Spacing(8);
                bar.Margin(wux::Thickness{12, 8, 12, 12});
                g_xamlStatus.TextWrapping(wux::TextWrapping::Wrap);
                g_xamlCustomId = wuxc::TextBox{};
                g_xamlCustomId.PlaceholderText(L"ID (0-999)");
                g_xamlCustomId.Width(MeasureText(L"ID (0-999)") + 36.0);
                g_xamlCustomId.VerticalAlignment(wux::VerticalAlignment::Center);
                g_xamlCustomId.Visibility(wux::Visibility::Collapsed);  // Shown while "Custom ID..." is chosen.
                g_xamlCustomId.KeyDown([tree](wf::IInspectable const&, wuxi::KeyRoutedEventArgs const& e) {
                    try {
                        if (e.Key() == winrt::Windows::System::VirtualKey::Enter) {
                            e.Handled(true);
                            ApplyTickedFromXaml(tree);
                        }
                    } catch (...) {
                        LogXamlHandlerFailure(L"ID box KeyDown");
                    }
                });
                g_barGroup = WrapGroup{};
                g_barGroup.host = bar;
                g_barGroup.items = {g_xamlBulkPicker, g_xamlCustomId, applyButton, resetButton, importButton, exportButton,
                                    g_xamlStatus};
                ReflowGroup(g_barGroup, 1e9);  // One row for now; the real width is known after the first layout.
                // The status text changes width as messages come and go: wrap again when it does.
                g_xamlStatus.SizeChanged([](wf::IInspectable const& sender, wux::SizeChangedEventArgs const& e) {
                    if (g_xamlClosing) return;
                    try {
                        auto found = g_itemWidths.find(winrt::get_abi(sender.as<wux::FrameworkElement>()));
                        const double known = found == g_itemWidths.end() ? 0 : found->second;
                        if (std::abs(e.NewSize().Width - known) > 1) ReflowXamlBars();
                    } catch (...) {
                        LogXamlHandlerFailure(L"status SizeChanged");
                    }
                });
                g_xamlBar = bar;

                // The toolbar above the list.
                wuxc::StackPanel toolbar;
                toolbar.Orientation(wuxc::Orientation::Vertical);
                toolbar.Spacing(8);
                toolbar.Margin(wux::Thickness{12, 12, 12, 8});


                g_xamlSearch = wuxc::TextBox{};
                g_xamlSearch.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                g_xamlSearch.PlaceholderText(Tr(L"Search apps", L"Pesquisar aplicativos"));
                g_xamlSearch.VerticalAlignment(wux::VerticalAlignment::Center);
                g_xamlSearchTimer = wux::DispatcherTimer{};
                g_xamlSearchTimer.Interval(wf::TimeSpan{1500000});  // 150 ms.
                g_xamlSearchTimer.Tick([tree](wf::IInspectable const&, wf::IInspectable const&) {
                    if (g_xamlClosing || !g_xamlSearchTimer) return;
                    try {
                        g_xamlSearchTimer.Stop();
                        RebuildXamlTree(tree);
                    } catch (...) {
                        LogXamlHandlerFailure(L"search Tick");
                    }
                });
                g_xamlSearch.TextChanged([](wf::IInspectable const&, wuxc::TextChangedEventArgs const&) {
                    if (g_xamlClosing || !g_xamlSearch || !g_xamlSearchTimer) return;
                    try {
                        g_xamlSearchText = std::wstring(g_xamlSearch.Text().c_str());
                        g_xamlSearchTimer.Stop();
                        g_xamlSearchTimer.Start();
                    } catch (...) {
                        LogXamlHandlerFailure(L"search TextChanged");
                    }
                });

                g_xamlFilter = wuxc::ComboBox{};
                g_xamlFilter.Width(260);
                // The drop-down is capped at a fixed height by default, which cut off the last of the 14 entries and
                // added a scroll bar for it. Raise the cap; Windows still keeps the list on the screen.
                g_xamlFilter.MaxDropDownHeight(1000.0);
                g_xamlFilter.VerticalAlignment(wux::VerticalAlignment::Center);
                g_xamlFilter.SelectionChanged([tree](wf::IInspectable const&, wuxc::SelectionChangedEventArgs const&) {
                    UpdateFilterForeground();
                    if (!g_xamlBusy) RebuildXamlTree(tree);
                });
                AttachWheelCycle(g_xamlFilter);
                PopulateXamlFilter();

                g_xamlSort = wuxc::ComboBox{};
                g_xamlSort.Width(200);
                AttachWheelCycle(g_xamlSort);
                g_xamlSort.VerticalAlignment(wux::VerticalAlignment::Center);
                auto addSort = [](std::wstring const& text, int mode) {
                    wuxc::ComboBoxItem item;
                    item.Content(winrt::box_value(text));
                    item.Tag(winrt::box_value(mode));
                    g_xamlSort.Items().Append(item);
                };
                addSort(Tr(L"Sort: A to Z", L"Ordem: A a Z"), kSortNameAsc);
                addSort(Tr(L"Sort: Z to A", L"Ordem: Z a A"), kSortNameDesc);
                addSort(Tr(L"Sort: ID low to high", L"Ordem: ID crescente"), kSortIdAsc);
                addSort(Tr(L"Sort: ID high to low", L"Ordem: ID decrescente"), kSortIdDesc);
                addSort(Tr(L"Sort: category name", L"Ordem: nome da categoria"), kSortHeadingAsc);
                addSort(Tr(L"Sort: disagreeing keys first", L"Ordem: chaves divergentes primeiro"), kSortConflictsFirst);
                g_xamlSort.SelectedIndex(0);
                g_xamlSort.SelectionChanged([tree](wf::IInspectable const&, wuxc::SelectionChangedEventArgs const&) {
                    if (!g_xamlBusy) RebuildXamlTree(tree);
                });

                g_xamlCheckAll = wuxc::CheckBox{};
                g_xamlCheckAll.Content(winrt::box_value(std::wstring(Tr(L"Check all", L"Marcar tudo"))));
                g_xamlCheckAll.VerticalAlignment(wux::VerticalAlignment::Center);
                g_xamlCheckAll.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    SetVisibleXamlChecks(tree, g_xamlCheckAll.IsChecked().Value());
                    UpdateXamlCheckAll();
                });

                g_xamlExpand = wuxc::Button{};
                g_xamlExpand.VerticalAlignment(wux::VerticalAlignment::Center);
                g_xamlExpand.Content(winrt::box_value(std::wstring(Tr(L"Collapse all", L"Recolher tudo"))));
                g_xamlExpand.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    SetXamlFoldersExpanded(tree, !g_xamlExpanded);
                    RebuildXamlTree(tree);
                });

                // The refresh button is an icon; its tooltip gives the name, as the user asked.
                wuxc::Button refresh;
                refresh.VerticalAlignment(wux::VerticalAlignment::Center);
                wuxc::FontIcon refreshIcon;
                refreshIcon.Glyph(L"\xE72C");
                g_refreshRotate = wuxm::RotateTransform{};
                refreshIcon.RenderTransform(g_refreshRotate);
                refreshIcon.RenderTransformOrigin(wf::Point{0.5f, 0.5f});
                refresh.Content(refreshIcon);
                wuxc::ToolTipService::SetToolTip(refresh, winrt::box_value(std::wstring(Tr(L"Refresh list (F5)", L"Atualizar lista (F5)"))));
                refresh.Click([tree](wf::IInspectable const&, wux::RoutedEventArgs const&) { RefreshXamlList(tree); });

                g_toolbarGroup = WrapGroup{};
                g_toolbarGroup.host = toolbar;
                g_toolbarGroup.items = {g_xamlSearch, g_xamlFilter, g_xamlSort, g_xamlCheckAll, g_xamlExpand, refresh};
                ReflowGroup(g_toolbarGroup, 1e9);  // One row for now; the real width is known after the first layout.
                // The custom title bar: the title on the left, minimise / maximise / close on the right.
                wuxc::TextBlock titleText;
                titleText.Text(Tr(L"Start Menu Category Manager", L"Gerenciador de Categorias do Menu Iniciar"));
                titleText.FontSize(12);
                titleText.VerticalAlignment(wux::VerticalAlignment::Center);
                titleText.IsHitTestVisible(false);
                titleText.Foreground(wuxm::SolidColorBrush(CaptionNormalColor()));
                g_titleText = titleText;
                // The Settings icon (the same one as the taskbar button), then the title.
                wuxc::StackPanel titleLeft;
                titleLeft.Orientation(wuxc::Orientation::Horizontal);
                titleLeft.Spacing(8);
                titleLeft.Margin(wux::Thickness{12, 0, 0, 0});
                titleLeft.VerticalAlignment(wux::VerticalAlignment::Center);
                titleLeft.IsHitTestVisible(false);
                {
                    const int iconPixels = (std::max)(32, ScaleForWindow(hwnd, 16));
                    HICON titleIcon = CreateSettingsAppIcon(iconPixels);
                    if (!titleIcon) titleIcon = CreateCogIcon(iconPixels, TaskbarIconColor());
                    if (titleIcon) {
                        if (auto iconSource = MakeIconSourceFromHicon(titleIcon)) {
                            wuxc::Image titleImage;
                            titleImage.Source(iconSource);
                            titleImage.Width(16);
                            titleImage.Height(16);
                            titleImage.VerticalAlignment(wux::VerticalAlignment::Center);
                            titleLeft.Children().Append(titleImage);
                        }
                    }
                }
                titleLeft.Children().Append(titleText);
                auto minimizeButton = MakeCaptionButton(L"\xE921", false,
                                                        [hwnd]() { PostMessageW(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0); }, nullptr);
                auto maximizeButton = MakeCaptionButton(L"\xE922", false, [hwnd]() {
                    PostMessageW(hwnd, WM_SYSCOMMAND, IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
                }, &g_xamlMaxGlyph);
                g_maxBorder = maximizeButton;
                auto closeButton = MakeCaptionButton(L"\xE8BB", true,
                                                     [hwnd]() { PostMessageW(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0); }, nullptr);
                wuxc::Grid titleBar;
                titleBar.Height(kTitleBarHeight);
                for (int column = 0; column < 4; ++column) {
                    wuxc::ColumnDefinition definition;
                    definition.Width(wux::GridLength{1, column == 0 ? wux::GridUnitType::Star : wux::GridUnitType::Auto});
                    titleBar.ColumnDefinitions().Append(definition);
                }
                wuxc::Grid::SetColumn(titleLeft, 0);
                wuxc::Grid::SetColumn(minimizeButton, 1);
                wuxc::Grid::SetColumn(maximizeButton, 2);
                wuxc::Grid::SetColumn(closeButton, 3);
                titleBar.Children().Append(titleLeft);
                titleBar.Children().Append(minimizeButton);
                titleBar.Children().Append(maximizeButton);
                titleBar.Children().Append(closeButton);
                wuxc::RowDefinition titleRow;
                titleRow.Height(wux::GridLength{1, wux::GridUnitType::Auto});
                wuxc::RowDefinition toolbarRow;
                toolbarRow.Height(wux::GridLength{1, wux::GridUnitType::Auto});
                wuxc::RowDefinition treeRow;
                treeRow.Height(wux::GridLength{1, wux::GridUnitType::Star});
                wuxc::RowDefinition barRow;
                barRow.Height(wux::GridLength{1, wux::GridUnitType::Auto});
                wuxc::Grid root;
                root.RowDefinitions().Append(titleRow);
                root.RowDefinitions().Append(toolbarRow);
                root.RowDefinitions().Append(treeRow);
                root.RowDefinitions().Append(barRow);
                wuxc::Grid::SetRow(titleBar, 0);
                wuxc::Grid::SetRow(toolbar, 1);
                wuxc::Grid::SetRow(tree, 2);
                wuxc::Grid::SetRow(bar, 3);
                // The list background: square, the full width of the window, and exactly as tall as the tree (the tree's
                // own top and bottom margins, 12 px each, are left out so it ends where the list is clipped).
                wuxc::Border listBackground;
                g_listBrush = wuxm::SolidColorBrush(ListColor());
                listBackground.Background(g_listBrush);
                listBackground.Margin(wux::Thickness{0, 12, 0, 12});
                wuxc::Grid::SetRow(listBackground, 2);
                root.Children().Append(titleBar);
                root.Children().Append(toolbar);
                root.Children().Append(listBackground);
                root.Children().Append(tree);
                root.Children().Append(bar);
                // Transparent layers, so the acrylic backdrop shows behind the list. Buttons and text boxes keep their fills.
                const auto clear = wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent());
                // The window tint (WindowTintColor): translucent on purpose, so the blur stays visible.
                g_tintBrush = wuxm::SolidColorBrush(WindowTintColor());
                root.Background(g_tintBrush);
                // The theme is set explicitly, and changed by ApplyThemeLive when the system mode changes.
                root.RequestedTheme(g_isDark ? wux::ElementTheme::Dark : wux::ElementTheme::Light);
                g_xamlRoot = root;
                // F5 reloads the list, whatever control has the focus (key presses bubble up to the root).
                root.KeyDown([tree](wf::IInspectable const&, wuxi::KeyRoutedEventArgs const& e) {
                    try {
                        if (e.Key() == winrt::Windows::System::VirtualKey::F5) {
                            e.Handled(true);
                            RefreshXamlList(tree);
                        }
                    } catch (...) {
                        LogXamlHandlerFailure(L"F5 KeyDown");
                    }
                });
                titleBar.Background(clear);
                toolbar.Background(clear);
                bar.Background(clear);
                root.Resources().Insert(winrt::box_value(winrt::hstring(L"ApplicationPageBackgroundThemeBrush")), clear);
                winrt::Windows::UI::Xaml::Interop::TypeName scrollType;
                scrollType.Name = L"Windows.UI.Xaml.Controls.ScrollViewer";
                scrollType.Kind = winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata;
                wux::Style scrollStyle;
                scrollStyle.TargetType(scrollType);
                scrollStyle.Setters().Append(wux::Setter(wuxc::Control::BackgroundProperty(), winrt::box_value(clear)));
                root.Resources().Insert(winrt::box_value(scrollType), scrollStyle);
                root.SizeChanged([](wf::IInspectable const&, wux::SizeChangedEventArgs const& e) {
                    if (g_xamlClosing) return;
                    try {
                        const double width = e.NewSize().Width;
                        if (std::abs(width - g_xamlContentWidth) < 0.5) return;
                        g_xamlContentWidth = width;
                        ReflowXamlBars();
                    } catch (...) {
                        LogXamlHandlerFailure(L"root SizeChanged");
                    }
                });
                g_xamlTreeSource.Content(AddResizeEdges(hwnd, root));
                UpdateXamlCheckAll();
            } catch (winrt::hresult_error const& error) {
                wchar_t code[32]{};
                wsprintfW(code, L"0x%08X", static_cast<unsigned int>(error.code()));
                Wh_Log(L"[xaml] could not create the XAML tree: %ls", code);
                return -1;
            } catch (...) {
                Wh_Log(L"[xaml] could not create the XAML tree (unknown error)");
                return -1;
            }
            return 0;
        }
        case WM_APP_CONFIRM_DONE:
            FinishConfirm(hwnd);
            return 0;
        case WM_TIMER:
            if (wParam == kConfirmTimerId) {
                RunConfirmedAction(hwnd);
                return 0;
            }
            break;
        case WM_SETTINGCHANGE:
            // Windows broadcasts "ImmersiveColorSet" when the colour mode or accent colour changes. Only a switch
            // between light and dark matters here. It is applied from a posted message, not from inside this
            // broadcast, and Windows sends it several times, so the check is repeated there.
            if (lParam != 0 && wcsncmp(reinterpret_cast<const wchar_t*>(lParam), L"ImmersiveColorSet", 17) == 0 &&
                !g_xamlClosing && SystemUsesDarkApps() != g_isDark) {
                PostMessageW(hwnd, WM_APP_THEME_CHANGED, 0, 0);
            }
            break;
        case WM_APP_THEME_CHANGED:
            if (g_xamlClosing || SystemUsesDarkApps() == g_isDark) return 0;
            try {
                ApplyThemeLive(hwnd);
            } catch (...) {
                LogXamlHandlerFailure(L"theme change");
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;  // Paint nothing.
        case WM_NCCALCSIZE:
            // No Windows-drawn frame or title bar: the client area is the whole window. (The maximised size is set
            // exactly in WM_GETMINMAXINFO below, so nothing overhangs the screen and nothing needs pulling back in.)
            // Answered the same way whether or not Windows passes the new rectangles: with WS_CAPTION kept, the
            // default handling would otherwise reserve room for a title bar.
            return 0;
        case WM_NCHITTEST:
            return TitleBarHitTest(hwnd, lParam);
        case WM_SYSCOMMAND:
            // Alt+Space: Windows opens the window's system menu for it, and this window has none.
            if ((wParam & 0xFFF0) == SC_KEYMENU && lParam == ' ') {
                RECT windowRect{};
                GetWindowRect(hwnd, &windowRect);
                ShowWindowMenu(hwnd, windowRect.left + (IsZoomed(hwnd) ? 0 : 1),
                               windowRect.top + ScaleForWindow(hwnd, kTitleBarHeight));
                return 0;
            }
            break;
        case WM_GETMINMAXINFO: {
            const UINT dpi = GetDpiForWindow(hwnd);
            auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = MulDiv(kMinWindowWidth, dpi, 96);
            info->ptMinTrackSize.y = MulDiv(kMinWindowHeight, dpi, 96);
            // The maximised rectangle, set exactly: the work area of the monitor, with no frame overhang (Windows adds
            // one to a resizable window, which left a 1 px untinted edge). An auto-hide taskbar only appears when
            // the cursor touches its screen edge, and Windows treats a window that covers the whole monitor as
            // full screen, so that edge is left one pixel uncovered, as other maximised apps do.
            MONITORINFO monitor{sizeof(monitor)};
            if (GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor)) {
                RECT area = monitor.rcWork;
                APPBARDATA appBar{sizeof(appBar)};
                appBar.rc = monitor.rcMonitor;
                auto hidesAt = [&](UINT edge) {
                    appBar.uEdge = edge;
                    return SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &appBar) != 0;
                };
                if (hidesAt(ABE_LEFT)) area.left += 1;
                if (hidesAt(ABE_TOP)) area.top += 1;
                if (hidesAt(ABE_RIGHT)) area.right -= 1;
                if (hidesAt(ABE_BOTTOM)) area.bottom -= 1;
                info->ptMaxPosition.x = area.left - monitor.rcMonitor.left;
                info->ptMaxPosition.y = area.top - monitor.rcMonitor.top;
                info->ptMaxSize.x = area.right - area.left;
                info->ptMaxSize.y = area.bottom - area.top;
                info->ptMaxTrackSize = info->ptMaxSize;
            }
            return 0;
        }
        case WM_SIZE:
            if (g_xamlTreeChild) {
                SetWindowPos(g_xamlTreeChild, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam),
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            PositionDragBar(hwnd);
            PositionMaxBar(hwnd);
            if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) {
                // The 1 px frame that keeps the shadow is not tinted, so it shows as a border when maximised.
                const MARGINS margins = wParam == SIZE_MAXIMIZED ? MARGINS{0, 0, 0, 0} : MARGINS{1, 1, 1, 1};
                DwmExtendFrameIntoClientArea(hwnd, &margins);
            }
            if (g_xamlMaxGlyph && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)) {
                g_xamlMaxGlyph.Text(wParam == SIZE_MAXIMIZED ? L"\xE923" : L"\xE922");  // Restore / maximise.
            }
            return 0;
        case WM_DESTROY:
            // First stop every handler from touching this window's controls and rows, then close the island.
            g_xamlClosing = true;
            g_maxBar = nullptr;
            g_maxBorder = nullptr;
            g_maxHot = false;
            g_maxPressed = false;
            g_rowWidths.clear();
            g_iconCache.clear();
            try {
                if (g_xamlTreeSource) g_xamlTreeSource.Close();
            } catch (...) {
            }
            if (g_xamlSearchTimer) {
                g_xamlSearchTimer.Stop();
                g_xamlSearchTimer = nullptr;
            }
            if (g_scrollBarTimer) {
                g_scrollBarTimer.Stop();
                g_scrollBarTimer = nullptr;
            }
            g_scrollBar = nullptr;
            g_scrollBarHooked = false;
            g_scrollBarHeld = false;
            g_xamlRoot = nullptr;
            g_tintBrush = nullptr;
            g_listBrush = nullptr;
            g_titleText = nullptr;
            g_captionGlyphs.clear();
            g_xamlTree = nullptr;
            g_xamlStatus = nullptr;
            g_xamlSearch = nullptr;
            g_xamlFilter = nullptr;
            g_xamlSort = nullptr;
            g_xamlExpand = nullptr;
            g_xamlBulkPicker = nullptr;
            g_xamlCustomId = nullptr;
            g_refreshRotate = nullptr;
            g_markReloadTimer = nullptr;
            g_filterStarTimer = nullptr;
            g_spinStory = nullptr;
            g_flickerStory = nullptr;
            g_refreshTimer = nullptr;
            g_refreshPending = false;
            g_xamlCheckAll = nullptr;
            g_xamlBar = nullptr;
            g_toolbarGroup = WrapGroup{};
            g_barGroup = WrapGroup{};
            g_itemWidths.clear();
            g_xamlContentWidth = 0;
            g_xamlListWindow = nullptr;
            g_xamlTreeSource = nullptr;
            g_xamlTreeChild = nullptr;
            g_dragBar = nullptr;
            g_xamlMaxGlyph = nullptr;
            // A confirmation left open by a closing window would block every later Reset all, Import and Export.
            if (g_pendingConfirm) {
                g_pendingConfirm->operation = nullptr;  // Breaks the loop between the operation, its callback and this state.
                g_pendingConfirm->dialog = nullptr;
                g_pendingConfirm = nullptr;
            }
            g_confirmedAction = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HICON CreateCogIcon(int size, COLORREF color);
HICON CreateSettingsAppIcon(int size);
COLORREF TaskbarIconColor();
void GiveWindowOwnTaskbarIdentity(HWND hwnd);

// The composition API that the system uses for its own windows.
struct XamlAccentPolicy {
    int accentState;
    int accentFlags;
    COLORREF gradientColor;  // AABBGGRR
    int animationId;
};
struct XamlCompositionData {
    int attribute;
    void* data;
    SIZE_T size;
};
using XamlSetCompositionAttribute = BOOL(WINAPI*)(HWND, XamlCompositionData*);

bool ApplyCompositionAccent(HWND hwnd, int accentState) {
    auto set = reinterpret_cast<XamlSetCompositionAttribute>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));
    if (!set) return false;
    // Nearly no tint here: the window tint comes from the XAML root background, so this fallback does not double it.
    const COLORREF tint = g_isDark ? 0x10000000 : 0x10F3F3F3;
    XamlAccentPolicy accent{accentState, 2, tint, 0};
    XamlCompositionData data{19 /* WCA_ACCENT_POLICY */, &accent, sizeof(accent)};
    return set(hwnd, &data) != FALSE;
}

// Acrylic behind the manager window. The XAML content is transparent (see MakeXamlIslandsTransparent) and the
// window has no drawing surface of its own, so the backdrop shows through the whole client area.
void ApplyXamlWindowBackdrop(HWND hwnd) {
    const int backdropType = 3;  // DWMSBT_TRANSIENTWINDOW: acrylic (Windows 11 22H2 and later).
    const HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));  // DWMWA_SYSTEMBACKDROP_TYPE
    if (SUCCEEDED(hr)) return;
    // Windows 10 and early Windows 11 do not know that attribute: use the composition acrylic instead.
    if (!ApplyCompositionAccent(hwnd, 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */)) {
        Wh_Log(L"[xaml] could not apply the acrylic backdrop (0x%08X)", static_cast<unsigned>(hr));
    }
}

// Centres the window on the work area of the primary monitor.
void CentreXamlWindow(HWND hwnd, int width, int height) {
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY), &info)) return;
    const RECT& work = info.rcWork;
    const int x = work.left + (work.right - work.left - width) / 2;
    const int y = work.top + (work.bottom - work.top - height) / 2;
    SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

// The narrowest window width, in DIPs, at which the whole toolbar fits on one row (the same rule ReflowGroup uses).
double ComputeToolbarFitWidth() {
    const wf::Size unbounded{(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)()};
    double total = 0;
    int shown = 0;
    for (auto const& item : g_toolbarGroup.items) {
        if (item.Visibility() == wux::Visibility::Collapsed) continue;
        double width;
        if (g_xamlSearch && winrt::get_abi(item) == winrt::get_abi(g_xamlSearch.as<wux::FrameworkElement>())) {
            width = 260.0;  // The search box is capped at this width (ReflowXamlBars).
        } else if (g_xamlExpand && winrt::get_abi(item) == winrt::get_abi(g_xamlExpand.as<wux::FrameworkElement>())) {
            // The label changes between "Collapse all" and "Expand all": the wider one decides.
            const auto original = g_xamlExpand.Content();
            width = 0;
            for (const wchar_t* label : {Tr(L"Collapse all", L"Recolher tudo"), Tr(L"Expand all", L"Expandir tudo")}) {
                g_xamlExpand.Content(winrt::box_value(std::wstring(label)));
                g_xamlExpand.Measure(unbounded);
                width = (std::max)(width, static_cast<double>(g_xamlExpand.DesiredSize().Width));
            }
            g_xamlExpand.Content(original);
        } else {
            width = ItemWidth(item);
        }
        total += width;
        ++shown;
    }
    if (shown == 0) return 0;
    return total + (shown - 1) * g_toolbarGroup.spacing + kWrapSideMargins;
}

// ---------------------------------------------------------------------------
// Creating and showing the manager window
// ---------------------------------------------------------------------------
void OpenXamlTreeWindow() {
    if (g_xamlListWindow && IsWindow(g_xamlListWindow)) {
        ShowWindow(g_xamlListWindow, IsIconic(g_xamlListWindow) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(g_xamlListWindow);
        return;
    }
    if (!g_xamlTreeClassRegistered) {
        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = XamlTreeWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;  // Never paint a fill: the backdrop must show through.
        const UINT iconDpi = GetDpiForSystem();
        const COLORREF iconColor = TaskbarIconColor();
        const int bigSize = GetSystemMetricsForDpi(SM_CXICON, iconDpi);
        const int smallSize = GetSystemMetricsForDpi(SM_CXSMICON, iconDpi);
        wc.hIcon = CreateSettingsAppIcon(bigSize);
        if (!wc.hIcon) wc.hIcon = CreateCogIcon(bigSize, iconColor);
        wc.hIconSm = CreateSettingsAppIcon(smallSize);
        if (!wc.hIconSm) wc.hIconSm = CreateCogIcon(smallSize, iconColor);
        wc.lpszClassName = g_xamlTreeClass.c_str();
        g_classIcon = wc.hIcon;
        g_classIconSmall = wc.hIconSm;
        g_xamlTreeClassRegistered = RegisterClassExW(&wc) != 0;
        if (!g_xamlTreeClassRegistered) ReleaseClassIcons();
    }
    if (!g_xamlTreeClassRegistered) {
        Wh_Log(L"[xaml] could not register the tree window class (%lu)", GetLastError());
        return;
    }
    // Read the colour mode every time a window is built: this thread outlives the window, so a mode read at start-up
    // would be out of date after the user switches between light and dark.
    g_isDark = SystemUsesDarkApps();
    const UINT sysDpi = GetDpiForSystem();
    const int width = MulDiv(1100, sysDpi, 96);
    const int height = MulDiv(640, sysDpi, 96);
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const int x = work.left + (work.right - work.left - width) / 2;
    const int y = work.top + (work.bottom - work.top - height) / 2;
    // No drawing surface of its own: the client area is only what XAML draws, so it can be transparent.
    HWND tree = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, g_xamlTreeClass.c_str(),
                                Tr(L"Start Menu Category Manager", L"Gerenciador de Categorias do Menu Iniciar"),
                                // No WS_SYSMENU: with it, Windows draws its own minimise / maximise / close buttons
                                // over the window's top right corner, next to the ones this window draws itself.
                                // The caption style stays (it is what gives the minimise animation), and the window
                                // is still minimised, maximised and closed by the SC_ commands the buttons send.
                                WS_OVERLAPPEDWINDOW & ~WS_SYSMENU, x, y, width, height, nullptr, nullptr,
                                ModuleInstance(), nullptr);
    g_xamlListWindow = tree;
    if (tree) {
        GiveWindowOwnTaskbarIdentity(tree);
        ApplyDarkTitleBar(tree);
        ApplyXamlWindowBackdrop(tree);
        // The window keeps WS_CAPTION: Windows only plays its minimise, maximise and restore animations (the window
        // shrinking into its taskbar button and back) for windows that have it. No title bar is drawn anyway,
        // because WM_NCCALCSIZE makes the client area the whole window. Make Windows measure the frame again.
        const MARGINS shadowMargins{1, 1, 1, 1};  // Keeps the window shadow now that there is no title bar.
        DwmExtendFrameIntoClientArea(tree, &shadowMargins);
        SetWindowPos(tree, nullptr, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        int openWidth = width;
        const double fitDip = ComputeToolbarFitWidth();
        if (fitDip > 0) {
            const UINT dpi = GetDpiForWindow(tree);
            // Two DIPs of slack, so a rounding difference cannot push the last button onto a second row.
            openWidth = static_cast<int>(std::ceil((fitDip + 2.0) * dpi / 96.0));
            openWidth = (std::max)(openWidth, MulDiv(kMinWindowWidth, dpi, 96));
            openWidth = (std::min)(openWidth, static_cast<int>(work.right - work.left));
        }
        CentreXamlWindow(tree, openWidth, height);
        {
            RECT windowRect{}, clientRect{};
            GetWindowRect(tree, &windowRect);
            GetClientRect(tree, &clientRect);
            Wh_Log(L"[xaml] title bar check: style=0x%08X window=%ldx%ld client=%ldx%ld",
                   static_cast<unsigned>(GetWindowLongPtrW(tree, GWL_STYLE)),
                   windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                   clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
        }
    }
    if (!tree) {
        Wh_Log(L"[xaml] could not create the tree window (%lu)", GetLastError());
        return;
    }
    ShowWindow(tree, SW_SHOW);
    SetForegroundWindow(tree);
    if (g_xamlTreeChild) {
        RECT rc{};
        GetClientRect(tree, &rc);
        ShowWindow(g_xamlTreeChild, SW_SHOW);
        SetWindowPos(g_xamlTreeChild, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
    CreateDragBar(tree);  // After the island, so it sits on top of it.
    CreateMaxBar(tree);   // Likewise: over the maximise button, for the snap layouts.
}


// Draws the Settings cog (Segoe Fluent Icons E713) into an icon of the given size.
HICON CreateCogIcon(int size, COLORREF color) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // Top-down.
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP colorBmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!dc || !colorBmp || !bits) {
        if (colorBmp) DeleteObject(colorBmp);
        if (dc) DeleteDC(dc);
        return nullptr;
    }

    HGDIOBJ oldBmp = SelectObject(dc, colorBmp);
    memset(bits, 0, static_cast<size_t>(size) * size * 4);

    HFONT font = CreateFontW(-(size * 90 / 100), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, L"Segoe Fluent Icons");
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    RECT rc{0, 0, size, size};
    DrawTextW(dc, kStartSettingsGlyph, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    GdiFlush();

    // GDI text leaves alpha at 0: use the glyph's brightness as alpha, tinted with `color`.
    BYTE* px = static_cast<BYTE*>(bits);
    for (int i = 0; i < size * size; ++i, px += 4) {
        const BYTE a = std::max(px[0], std::max(px[1], px[2]));
        px[0] = static_cast<BYTE>(GetBValue(color) * a / 255);  // Premultiplied.
        px[1] = static_cast<BYTE>(GetGValue(color) * a / 255);
        px[2] = static_cast<BYTE>(GetRValue(color) * a / 255);
        px[3] = a;
    }

    SelectObject(dc, oldFont);
    DeleteObject(font);
    SelectObject(dc, oldBmp);
    DeleteDC(dc);

    HBITMAP maskBmp = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmMask = maskBmp;
    ii.hbmColor = colorBmp;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(maskBmp);
    DeleteObject(colorBmp);
    return icon;
}

// The real, colored icon of the Settings app, fetched from the shell. Null if unavailable.
HICON CreateSettingsAppIcon(int size) {
    IShellItemImageFactory* factory = nullptr;
    if (FAILED(SHCreateItemFromParsingName(
            L"shell:AppsFolder\\windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel",
            nullptr, IID_PPV_ARGS(&factory))) || !factory) {
        return nullptr;
    }
    HBITMAP bitmap = nullptr;
    const SIZE wanted{size, size};
    const HRESULT hr = factory->GetImage(wanted, SIIGBF_ICONONLY, &bitmap);
    factory->Release();
    if (FAILED(hr) || !bitmap) return nullptr;

    BITMAP bm{};
    GetObjectW(bitmap, sizeof(bm), &bm);
    HBITMAP mask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmMask = mask;
    ii.hbmColor = bitmap;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(bitmap);
    return icon;
}

// Light text on a dark taskbar, dark text on a light one.
COLORREF TaskbarIconColor() {
    DWORD light = 0;
    DWORD size = sizeof(light);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size) == ERROR_SUCCESS &&
        light != 0) {
        return RGB(32, 32, 32);
    }
    return RGB(255, 255, 255);
}

// Windows in explorer.exe share Explorer's taskbar identity by default, so the manager would
// be folded into the File Explorer button (or missing). Give it an identity of its own.
void GiveWindowOwnTaskbarIdentity(HWND hwnd) {
    IPropertyStore* store = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&store))) && store) {
        PROPVARIANT pv{};
        const wchar_t id[] = L"Windhawk.StartMenuCategoryManager";
        pv.vt = VT_LPWSTR;
        pv.pwszVal = static_cast<LPWSTR>(CoTaskMemAlloc(sizeof(id)));
        if (pv.pwszVal) {
            memcpy(pv.pwszVal, id, sizeof(id));
            store->SetValue(kPkeyAppUserModelId, pv);
            PropVariantClear(&pv);
        }
        store->Release();
    }
}


// After the manager window is gone: lets go of what the XAML globals still hold. This runs on the manager thread,
// which owns those objects. The globals are marked [[clang::no_destroy]], so nothing else would release them.
void ReleaseManagerUiState() {
    std::map<int, wuxm::ImageSource>().swap(g_iconCache);
    std::vector<RowWidth>().swap(g_rowWidths);
    std::vector<wuxc::TextBlock>().swap(g_captionGlyphs);
    g_toolbarGroup = WrapGroup{};
    g_barGroup = WrapGroup{};
    g_pendingConfirm = nullptr;
    g_confirmedAction = nullptr;
}

// After the manager thread has ended: no window of these classes exists any more, and their window procedures are code
// of this mod, so they must be unregistered before the DLL is unloaded.
void UnregisterManagerWindowClasses() {
    const HMODULE module = ModuleInstance();
    if (g_maxBarClassRegistered && UnregisterClassW(g_maxBarClass.c_str(), module)) g_maxBarClassRegistered = false;
    if (g_dragBarClassRegistered && UnregisterClassW(g_dragBarClass.c_str(), module)) g_dragBarClassRegistered = false;
    if (g_xamlTreeClassRegistered && UnregisterClassW(g_xamlTreeClass.c_str(), module)) {
        g_xamlTreeClassRegistered = false;
        ReleaseClassIcons();
    }
}

DWORD WINAPI ManagerThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_isDark = SystemUsesDarkApps();
    InitFolderIconIndex();
    LoadEntries();
    OpenXamlTreeWindow();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_SHOW && msg.hwnd == nullptr) {
            OpenXamlTreeWindow();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_xamlListWindow && IsWindow(g_xamlListWindow)) DestroyWindow(g_xamlListWindow);
    ReleaseManagerUiState();
    g_xamlTreeSource = nullptr;
    g_xamlManager = nullptr;
    if (SUCCEEDED(hrCo)) CoUninitialize();
    Wh_Log(L"Manager thread ended");
    return 0;
}

void ShowManager() {
    if (g_shuttingDown) {
        Wh_Log(L"Open request ignored: the mod is shutting down");
        return;
    }

    AcquireSRWLockExclusive(&g_uiLock);
    if (g_uiThread) {
        if (WaitForSingleObject(g_uiThread, 0) == WAIT_TIMEOUT) {
            // Already running: ask it to bring the window forward.
            Wh_Log(L"Manager thread already running; asking it to show the window");
            PostThreadMessageW(g_uiThreadId, WM_APP_SHOW, 0, 0);
            ReleaseSRWLockExclusive(&g_uiLock);
            return;
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    Wh_Log(L"Starting the manager thread");
    g_uiThread = CreateThread(nullptr, 0, ManagerThreadProc, nullptr, 0, &g_uiThreadId);
    if (!g_uiThread) Wh_Log(L"Failed to create manager thread (%lu)", GetLastError());
    ReleaseSRWLockExclusive(&g_uiLock);
}

// ---------------------------------------------------------------------------
// Opening the manager from the Start menu process
// ---------------------------------------------------------------------------

// The Start menu process cannot host the manager (applying a change restarts that process),
// so it signals this event and the copy of the mod inside explorer.exe opens the window.
// The event gets a permissive ACL because the Start menu process may be sandboxed.
// One event per Windows session, so two signed-in users never signal each other's Explorer. Both
// processes (Start and Explorer) run in the same session, so both compute the same name.
std::wstring GetOpenEventName() {
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    return L"Global\\WindhawkCategoryManagerOpen-" + std::to_wstring(session);
}

// Creates the event the Start menu process signals. Returns 0 on success, or the Windows error code.
DWORD CreateOpenRequestEvent() {
    SECURITY_ATTRIBUTES sa{sizeof(sa)};
    PSECURITY_DESCRIPTOR sd = nullptr;
    // Everyone + all app packages: SYNCHRONIZE | EVENT_MODIFY_STATE only (0x100002), so they can
    // signal the event but not change its security or delete it.
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;0x100002;;;WD)(A;;0x100002;;;AC)", SDDL_REVISION_1,
                                                              &sd, nullptr)) {
        sa.lpSecurityDescriptor = sd;
    }
    const std::wstring name = GetOpenEventName();
    g_openEvent = CreateEventW(&sa, FALSE, FALSE, name.c_str());
    DWORD error = g_openEvent ? ERROR_SUCCESS : GetLastError();  // Read before any other call resets it.
    if (sd) LocalFree(sd);
    if (!g_openEvent && error == ERROR_ACCESS_DENIED) {
        // The event of a previous Explorer that is still shutting down exists, and its permissions only allow waiting
        // and signalling. Join it: it stays alive while this handle is open, and Start signals it by name.
        g_openEvent = OpenEventW(SYNCHRONIZE, FALSE, name.c_str());
        if (g_openEvent) {
            Wh_Log(L"Joined the open-request event of the previous Explorer");
            error = ERROR_SUCCESS;
        }
    }
    return error;
}

// The event is created here, not in Wh_ModInit, so that a failure can be retried without holding up Explorer's start.
DWORD WINAPI OpenRequestThreadProc(LPVOID) {
    DWORD error = CreateOpenRequestEvent();
    for (int attempt = 0; error != ERROR_SUCCESS && attempt < 40; ++attempt) {
        if (WaitForSingleObject(g_stopEvent, 250) == WAIT_OBJECT_0) return 0;
        error = CreateOpenRequestEvent();
    }
    if (error != ERROR_SUCCESS) {
        Wh_Log(L"Could not create the open-request event (%lu)", error);
        return 0;
    }
    HANDLE handles[2] = {g_openEvent, g_stopEvent};
    while (WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
        Wh_Log(L"Open request received from the Start menu");
        ShowManager();
    }
    return 0;
}

void StartOpenRequestListener() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"Could not create the stop event (%lu)", GetLastError());
        return;
    }
    g_openThread = CreateThread(nullptr, 0, OpenRequestThreadProc, nullptr, 0, nullptr);
}

void StopOpenRequestListener() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_openThread) {
        WaitForSingleObject(g_openThread, INFINITE);  // It runs this mod's code, so it must be gone first.
        CloseHandle(g_openThread);
        g_openThread = nullptr;
    }
    if (g_openEvent) CloseHandle(g_openEvent);
    if (g_stopEvent) CloseHandle(g_stopEvent);
    g_openEvent = g_stopEvent = nullptr;
}

void SignalExplorerToOpenManager() {
    HANDLE ev = OpenEventW(EVENT_MODIFY_STATE, FALSE, GetOpenEventName().c_str());
    if (!ev) {
        Wh_Log(L"[start] Could not reach explorer.exe to open the manager (error %lu)", GetLastError());
        return;
    }
    // Start has the foreground at this moment, so it can hand that right to Explorer. Without it Explorer's
    // SetForegroundWindow for the manager window may be refused.
    DWORD shellProcess = 0;
    if (HWND shell = GetShellWindow()) GetWindowThreadProcessId(shell, &shellProcess);
    if (shellProcess != 0) AllowSetForegroundWindow(shellProcess);
    SetEvent(ev);
    CloseHandle(ev);
    Wh_Log(L"[start] Asked Explorer to open the manager");
}

// Presses Escape, which closes the Start menu (the same as pressing it by hand). Used when the
// manager is opened from Start: otherwise the Start menu stays open on top of the manager.
void DismissStartMenu() {
    // Only press Escape when Start itself has the keyboard focus. A synthetic key goes to whatever window is in front,
    // so if something else is, it is better to leave Start open (the manager still opens) than to press Escape there.
    DWORD foregroundProcess = 0;
    if (HWND foreground = GetForegroundWindow()) GetWindowThreadProcessId(foreground, &foregroundProcess);
    if (foregroundProcess != GetCurrentProcessId()) {
        Wh_Log(L"[start] The Start menu is not the foreground window; not sending Escape");
        return;
    }
    INPUT inputs[2]{};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_ESCAPE;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_ESCAPE;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void OnManagerClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    // Only the Start menu process has the item. Close Start first, then let Explorer bring the manager window forward.
    DismissStartMenu();
    SignalExplorerToOpenManager();
}

// ---------------------------------------------------------------------------
// Start menu context menu injection (runs inside StartMenuExperienceHost.exe)
// ---------------------------------------------------------------------------

bool HStringEquals(winrt::hstring const& value, const wchar_t* text) {
    return std::wstring_view(value.c_str(), value.size()) == text;
}

bool IsNamedItem(wuxc::MenuFlyoutItemBase const& baseItem, const wchar_t* name) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            return HStringEquals(frameworkElement.Name(), name);
        }
    } catch (...) {}
    return false;
}

// Brazilian Portuguese gets a translated label; every other language gets English.
const wchar_t* GetMenuItemText() {
    return Tr(L"Manage Categories", L"Gerenciar Categorias");
}

void AppendInjectedItems(void* vectorThis);

using MenuFlyoutItemBaseVector_Append_t = void(__cdecl*)(void*, wuxc::MenuFlyoutItemBase const&);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original = nullptr;

// The items this mod put into Start's menus. The Click handler is code in this mod, so it has to be taken off the
// items before the mod unloads; otherwise Start would call (or release) it after it is gone.
struct InjectedItem {
    winrt::weak_ref<wuxc::MenuFlyoutItem> item;
    winrt::weak_ref<wuxc::MenuFlyoutSeparator> separator;
    winrt::event_token clickToken;
};
SRWLOCK g_injectedLock = SRWLOCK_INIT;
// Weak references and tokens only, which are safe to destroy on any thread.
std::vector<InjectedItem> g_injectedItems;
// The thread the items were created on (Start's UI thread), recorded by AppendInjectedItems.
std::atomic<DWORD> g_startUiThreadId{0};

// Runs on Start's UI thread, which owns the items.
void ReleaseInjectedItems() {
    std::vector<InjectedItem> items;
    AcquireSRWLockExclusive(&g_injectedLock);
    items.swap(g_injectedItems);
    ReleaseSRWLockExclusive(&g_injectedLock);
    for (auto& injected : items) {
        try {
            if (auto item = injected.item.get()) {
                item.Click(injected.clickToken);
                item.Visibility(wux::Visibility::Collapsed);
            }
            if (auto separator = injected.separator.get()) separator.Visibility(wux::Visibility::Collapsed);
        } catch (...) {
        }
    }
}

// Runs `proc` on the thread that owns `window` and returns only when it has finished, so no code of this mod is left
// queued on that thread afterwards. A WH_CALLWNDPROC hook for that thread runs the procedure when a registered message
// is sent to the window; sending the message blocks until the thread has handled it. This is the same pattern as
// RunFromWindowThread in the Taskbar Labels mod.
using RunOnWindowThreadProc_t = void (*)(void*);

bool RunOnWindowThread(HWND window, RunOnWindowThreadProc_t proc, void* procParam) {
    static const UINT runMessage = RegisterWindowMessageW(L"WindhawkStartCategoryManager.RunOnWindowThread");

    struct Request {
        RunOnWindowThreadProc_t proc;
        void* procParam;
    };

    const DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (threadId == 0) return false;
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runMessage) {
                    auto* request = reinterpret_cast<Request*>(cwp->lParam);
                    request->proc(request->procParam);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Request request{proc, procParam};
    SendMessageW(window, runMessage, 0, reinterpret_cast<LPARAM>(&request));
    UnhookWindowsHookEx(hook);
    return true;
}

struct ThreadWindows {
    HWND core = nullptr;  // Start's CoreWindow, preferred.
    HWND any = nullptr;   // Any other window of the thread.
};

BOOL CALLBACK FindThreadWindowProc(HWND hwnd, LPARAM lParam) {
    auto* windows = reinterpret_cast<ThreadWindows*>(lParam);
    if (!windows->any) windows->any = hwnd;
    wchar_t className[64]{};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) && wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        windows->core = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Called from Wh_ModUninit, which does not run on Start's UI thread. Waits, with no timeout, until the cleanup has run
// on that thread, so Start cannot call or release the Click handler (code in this mod) after the mod is unloaded.
void ReleaseInjectedItemsOnUiThread() {
    const DWORD threadId = g_startUiThreadId;
    if (threadId == 0) return;  // No item was ever injected.

    ThreadWindows windows;
    EnumThreadWindows(threadId, FindThreadWindowProc, reinterpret_cast<LPARAM>(&windows));
    const HWND window = windows.core ? windows.core : windows.any;
    if (!window || !RunOnWindowThread(window, [](void*) { ReleaseInjectedItems(); }, nullptr)) {
        Wh_Log(L"[start] Could not release the injected menu items on Start's UI thread");
    }
}

void AppendInjectedItems(void* vectorThis) {
    try {
        wuxc::MenuFlyoutItem item;
        item.Name(kItemName);
        item.Tag(winrt::box_value(winrt::hstring{kItemName}));
        item.Text(GetMenuItemText());

        wuxc::FontIcon icon;
        icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
        icon.Glyph(kManageItemGlyph);
        icon.FontSize(16);
        item.Icon(icon);

        const winrt::event_token clickToken = item.Click(wux::RoutedEventHandler{&OnManagerClick});

        wuxc::MenuFlyoutSeparator separator;
        separator.Name(kSeparatorName);
        separator.Tag(winrt::box_value(winrt::hstring{kSeparatorName}));

        {
            AcquireSRWLockExclusive(&g_injectedLock);
            g_startUiThreadId = GetCurrentThreadId();
            // Menus are rebuilt each time they open; forget the items Start has already let go of.
            g_injectedItems.erase(std::remove_if(g_injectedItems.begin(), g_injectedItems.end(),
                                                 [](const InjectedItem& injected) { return !injected.item.get(); }),
                                  g_injectedItems.end());
            g_injectedItems.push_back({winrt::make_weak(item), winrt::make_weak(separator), clickToken});
            ReleaseSRWLockExclusive(&g_injectedLock);
        }

        MenuFlyoutItemBaseVector_Append_Original(vectorThis, item);
        MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
    } catch (...) {
        Wh_Log(L"[start] Failed to inject the menu item");
    }
}

// The context menu of an empty area of the Start menu has a single item, "Start settings",
// with the Settings cog. Recognise it by the icon (works in any language) or by its text.
bool IsStartSettingsItem(wuxc::MenuFlyoutItemBase const& base) {
    try {
        auto mi = base.try_as<wuxc::MenuFlyoutItem>();
        if (!mi) return false;

        if (auto icon = mi.Icon()) {
            if (auto fontIcon = icon.try_as<wuxc::FontIcon>()) {
                if (HStringEquals(fontIcon.Glyph(), kStartSettingsGlyph)) return true;
            }
        }

        const std::wstring text = Lower(std::wstring(mi.Text().c_str()));
        return text.find(L"start settings") != std::wstring::npos ||
               text.find(L"configura\u00e7\u00f5es do iniciar") != std::wstring::npos;
    } catch (...) {}
    return false;
}

void __cdecl StartMenuVectorAppend_Hook(void* pThis, wuxc::MenuFlyoutItemBase const& item) {
    try {
        if (!IsNamedItem(item, kItemName) && !IsNamedItem(item, kSeparatorName) && IsStartSettingsItem(item)) {
            AppendInjectedItems(pThis);
        }
    } catch (...) {}
    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

bool HookStartMenuDllSymbols(HMODULE module) {
    // StartMenu.dll, StartUI.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            StartMenuVectorAppend_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"[start] Could not hook the menu item list in the Start menu DLL");
        ReportHooks(false);
        return false;
    }
    Wh_Log(L"[start] Hooking the Start menu DLL");
    ReportHooks(true);
    return true;
}

// ---------------------------------------------------------------------------
// Category overrides inside the Start menu process
// ---------------------------------------------------------------------------

// Start's StartTileData.dll reads each app's category with RegGetValueW, from
// HKCU\Software\Microsoft\Windows\CurrentVersion\Start\TileProperties\<key>, value "Category". This hook answers that
// read with the override for the key, when there is one. Every other call goes to the real function.
OverrideMap g_startOverrides;  // Loaded once while the mod loads in Start; only read afterwards.
std::atomic<int> g_overridesServed{0};

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original = nullptr;

// The part of a registry path after "...\Start\TileProperties\", or null if the path is not below that key.
const wchar_t* TileKeyOf(const wchar_t* subKey) {
    constexpr wchar_t kMarker[] = L"\\Start\\TileProperties\\";
    constexpr size_t kMarkerLength = ARRAYSIZE(kMarker) - 1;
    for (const wchar_t* p = subKey; *p; ++p) {
        if (_wcsnicmp(p, kMarker, kMarkerLength) == 0) return p + kMarkerLength;
    }
    return nullptr;
}

LSTATUS WINAPI RegGetValueW_Hook(HKEY key, LPCWSTR subKey, LPCWSTR value, DWORD flags, LPDWORD type, PVOID data,
                                 LPDWORD size) {
    if (key == HKEY_CURRENT_USER && subKey && value && _wcsicmp(value, L"Category") == 0) {
        const DWORD typeFlags = flags & RRF_RT_ANY;
        const wchar_t* tileKey = (typeFlags == 0 || (typeFlags & RRF_RT_REG_DWORD)) ? TileKeyOf(subKey) : nullptr;
        if (tileKey) {
            const auto found = g_startOverrides.find(tileKey);
            if (found != g_startOverrides.end()) {
                if (g_overridesServed.fetch_add(1) == 0) {
                    Wh_Log(L"[start] The first category override was served (key %ls)", tileKey);
                }
                if (type) *type = REG_DWORD;
                if (!data) {
                    if (size) *size = sizeof(DWORD);
                    return ERROR_SUCCESS;
                }
                if (!size) return ERROR_INVALID_PARAMETER;
                const DWORD capacity = *size;
                *size = sizeof(DWORD);
                if (capacity < sizeof(DWORD)) return ERROR_MORE_DATA;
                *static_cast<DWORD*>(data) = found->second;
                return ERROR_SUCCESS;
            }
        }
    }
    return RegGetValueW_Original(key, subKey, value, flags, type, data, size);
}

HMODULE GetStartMenuModuleHandle() {
    HMODULE module = GetModuleHandleW(L"StartMenu.dll");
    if (!module) module = GetModuleHandleW(L"StartUI.dll");
    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// Start's menu DLL may load after the mod, so watch for it (Start host only).
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_startModuleHooked && GetStartMenuModuleHandle() == module) {
        if (!g_startModuleHooked.exchange(true)) {
            if (HookStartMenuDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }
    return module;
}

}  // namespace

// ---------------------------------------------------------------------------
// Wh_ModInit and friends
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Start Menu Category Manager %ls", WH_MOD_VERSION);
    wchar_t exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    g_isStartHost = _wcsicmp(PathFindFileNameW(exePath), L"StartMenuExperienceHost.exe") == 0;

    // Explorer can run folder windows in separate processes, and each of them loads the mod. Only the shell process
    // (the one that owns the desktop) does the work.
    if (!g_isStartHost) {
        DWORD shellProcess = 0;
        if (HWND shell = GetShellWindow()) GetWindowThreadProcessId(shell, &shellProcess);
        if (shellProcess != 0 && shellProcess != GetCurrentProcessId()) {
            Wh_Log(L"Not the shell Explorer process; the mod does nothing here");
            return FALSE;
        }
    }

    if (g_isStartHost) {
        ResetHookReport();
        Wh_Log(L"[start] Windows build %ls", WindowsBuildString().c_str());
        if (HMODULE startModule = GetStartMenuModuleHandle()) {
            g_startModuleHooked = true;
            HookStartMenuDllSymbols(startModule);
        }

        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibraryExW = kernelBase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"))
            : nullptr;
        if (loadLibraryExW) {
            WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        }

        // Start reads each app's category through RegGetValueW. The overrides are loaded before the hook is installed.
        LoadOverridesFromStorage(g_startOverrides);
        Wh_Log(L"[start] %lu category override(s) loaded", static_cast<unsigned long>(g_startOverrides.size()));
        void* regGetValueW = kernelBase ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegGetValueW")) : nullptr;
        if (regGetValueW && Wh_SetFunctionHook(regGetValueW, reinterpret_cast<void*>(RegGetValueW_Hook),
                                               reinterpret_cast<void**>(&RegGetValueW_Original))) {
            Wh_Log(L"[start] RegGetValueW hook set");
        } else {
            Wh_Log(L"[start] Could not hook RegGetValueW; your categories will not be applied");
        }
    } else {
        // explorer.exe only hosts the manager window and listens for requests from Start.
        LoadOverridesFromStorage(g_overrides);  // Before Start is restarted below, which loads the same overrides.
        CreateHookStatusEvents();  // Before Start is restarted below, so the new Start process can report to them.
        StartOpenRequestListener();
        StartHookWatch();
        // Restart the Start menu if it does not have this mod yet, so its new item appears.
        RestartStartMenuStartedBeforeMod();
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isStartHost || g_startModuleHooked) return;

    if (HMODULE startModule = GetStartMenuModuleHandle()) {
        if (!g_startModuleHooked.exchange(true)) {
            if (HookStartMenuDllSymbols(startModule)) {
                Wh_ApplyHookOperations();
            }
        }
    }
}

void Wh_ModUninit() {
    g_shuttingDown = true;
    if (g_isStartHost) {
        // Besides the hooks, only the menu items' Click handler lives in Start. It is removed on Start's UI thread,
        // and this returns only after that has happened.
        ReleaseInjectedItemsOnUiThread();
        return;
    }

    StopHookWatch();
    StopOpenRequestListener();

    AcquireSRWLockExclusive(&g_uiLock);
    HANDLE thread = g_uiThread;
    const DWORD threadId = g_uiThreadId;
    g_uiThread = nullptr;
    ReleaseSRWLockExclusive(&g_uiLock);

    if (thread) {
        // Ask the manager thread to quit and wait for it, so no mod code is
        // running (or has a live window) when Windhawk unloads the DLL.
        for (;;) {
            PostThreadMessageW(threadId, WM_QUIT, 0, 0);
            if (WaitForSingleObject(thread, 100) == WAIT_OBJECT_0) break;
        }
        CloseHandle(thread);
    }
    UnregisterManagerWindowClasses();  // No window of these classes exists any more, and their code is about to unload.

    // Start keeps the categories it has already built until it restarts, so restart it to show Windows' own categories
    // again. Nothing to do when the mod had no overrides.
    if (!g_overrides.empty()) RestartStartMenuHost();
    CloseHookStatusEvents();
}
