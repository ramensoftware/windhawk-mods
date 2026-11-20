// ==WindhawkMod==
// @id              explorer-navigation-pane-filter
// @name            File Explorer Navigation Pane Filter
// @name:ru-RU      Фильтр панели навигации Проводника
// @description     Choose which sections and pinned folders to hide from the Windows 11 File Explorer navigation pane.
// @description:ru-RU Выберите, какие разделы и закреплённые папки скрыть в панели навигации Проводника Windows 11.
// @version         1.0
// @author          Kirill Shchetinnikov
// @github          https://github.com/KirillShchetinnikov
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Navigation Pane Filter

Hide selected entries from the left-hand navigation pane in Windows 11 File
Explorer. All filters start off. Enable the ones you want in **Settings**.

* Choose Home, Gallery, OneDrive, This PC, Network, Linux, Libraries or
  Recycle Bin individually. The presets recognize English and Russian labels.
* Use **Additional names** for pinned folders, other languages or OneDrive
  accounts displayed under a different name. Separate names with semicolons.
  The matching option applies to additional names only.
* The filter covers top-level entries and entries directly under the Desktop
  container when **Show all folders** groups them there. Hiding a section
  also hides its branch from the pane. You can still open the locations from
  the address bar, and pinned folders stay pinned.

The mod applies when Windows populates the pane. To bring hidden entries back,
turn off their filters, then close and reopen the affected Explorer windows.
This also applies when disabling the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- home: false
  $name: Hide Home
  $name:ru-RU: Скрыть «Главная»
- gallery: false
  $name: Hide Gallery
  $name:ru-RU: Скрыть «Галерея»
- oneDrive: false
  $name: Hide OneDrive
  $name:ru-RU: Скрыть OneDrive
- thisPC: false
  $name: Hide This PC
  $name:ru-RU: Скрыть «Этот компьютер»
- network: false
  $name: Hide Network
  $name:ru-RU: Скрыть «Сеть»
- linux: false
  $name: Hide Linux
  $name:ru-RU: Скрыть Linux
- libraries: false
  $name: Hide Libraries
  $name:ru-RU: Скрыть «Библиотеки»
- recycleBin: false
  $name: Hide Recycle Bin
  $name:ru-RU: Скрыть «Корзина»
- additionalNames: ""
  $name: Additional names
  $name:ru-RU: Дополнительные названия
  $description: Separate the names as shown in the navigation pane with semicolons.
  $description:ru-RU: Введите названия из панели навигации через точку с запятой.
- additionalMatch: exact
  $name: Match additional names
  $name:ru-RU: Сопоставление дополнительных названий
  $options:
    - exact: Exact
    - prefix: Starts with
    - contains: Contains
  $options:ru-RU:
    - exact: Точное совпадение
    - prefix: Начинается с
    - contains: Содержит
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace {

using Text = std::wstring_view;

enum class Match { Exact, Prefix, Contains, OneDrive };

struct Rule {
    std::wstring name;
    Match mode;
};

using Rules = std::vector<Rule>;

constexpr wchar_t kRequestName[] =
    L"Windhawk.ExplorerNavigationPaneFilter.Apply";
constexpr wchar_t kPendingProperty[] =
    L"Windhawk.ExplorerNavigationPaneFilter.Pending";

std::mutex g_rulesMutex;
std::shared_ptr<const Rules> g_rules;
UINT g_applyMessage;
decltype(&SendMessageW) g_sendMessage;
decltype(&DispatchMessageW) g_dispatchMessage;

bool EqualText(Text first, Text second) {
    return first.size() == second.size() &&
           CompareStringOrdinal(first.data(), static_cast<int>(first.size()),
                                second.data(), static_cast<int>(second.size()),
                                TRUE) == CSTR_EQUAL;
}

bool Matches(Text label, const Rule& rule) {
    const Text name = rule.name;
    if (rule.mode == Match::Exact) {
        return EqualText(label, name);
    }
    if (label.size() < name.size()) {
        return false;
    }
    if (rule.mode == Match::Contains) {
        for (size_t offset = 0; offset <= label.size() - name.size();
             ++offset) {
            if (EqualText(label.substr(offset, name.size()), name)) {
                return true;
            }
        }
        return false;
    }
    if (!EqualText(label.substr(0, name.size()), name)) {
        return false;
    }
    return rule.mode == Match::Prefix || label.size() == name.size() ||
           Text(L" -\u2013\u2014").find(label[name.size()]) != Text::npos;
}

std::shared_ptr<const Rules> ReadRules() {
    constexpr struct {
        PCWSTR key;
        PCWSTR english;
        PCWSTR russian;
    } presets[] = {
        {L"home", L"Home", L"Главная"},
        {L"gallery", L"Gallery", L"Галерея"},
        {L"thisPC", L"This PC", L"Этот компьютер"},
        {L"network", L"Network", L"Сеть"},
        {L"linux", L"Linux", nullptr},
        {L"libraries", L"Libraries", L"Библиотеки"},
        {L"recycleBin", L"Recycle Bin", L"Корзина"},
    };

    auto rules = std::make_shared<Rules>();
    for (const auto& preset : presets) {
        if (Wh_GetIntSetting(preset.key)) {
            rules->push_back({preset.english, Match::Exact});
            if (preset.russian) {
                rules->push_back({preset.russian, Match::Exact});
            }
        }
    }
    if (Wh_GetIntSetting(L"oneDrive")) {
        rules->push_back({L"OneDrive", Match::OneDrive});
    }

    auto matchSetting = WindhawkUtils::StringSetting::make(L"additionalMatch");
    Text match = matchSetting.get();
    Match mode = Match::Exact;
    if (match == L"prefix") {
        mode = Match::Prefix;
    } else if (match == L"contains") {
        mode = Match::Contains;
    }

    auto namesSetting = WindhawkUtils::StringSetting::make(L"additionalNames");
    Text names = namesSetting.get();
    constexpr Text whitespace = L" \t\r\n";
    while (!names.empty()) {
        size_t separator = names.find(L';');
        Text field = names.substr(0, separator);
        size_t start = field.find_first_not_of(whitespace);
        if (start != Text::npos) {
            size_t end = field.find_last_not_of(whitespace);
            rules->push_back({std::wstring(field.substr(start, end - start + 1)),
                              mode});
        }
        if (separator == Text::npos) {
            break;
        }
        names.remove_prefix(separator + 1);
    }
    return rules->empty() ? nullptr : rules;
}

bool HasClass(HWND hwnd, Text expected) {
    wchar_t name[64];
    int length = GetClassNameW(hwnd, name, std::size(name));
    return length > 0 && Text(name, length) == expected;
}

bool IsNavigationTree(HWND hwnd) {
    return HasClass(hwnd, L"SysTreeView32") &&
           HasClass(GetParent(hwnd), L"NamespaceTreeControl") &&
           HasClass(GetAncestor(hwnd, GA_ROOT), L"CabinetWClass");
}

HTREEITEM RelatedItem(HWND tree, WPARAM relation, HTREEITEM item = nullptr) {
    return reinterpret_cast<HTREEITEM>(
        SendMessageW(tree, TVM_GETNEXTITEM, relation,
                     reinterpret_cast<LPARAM>(item)));
}

std::wstring ItemLabel(HWND tree, HTREEITEM item) {
    // Leave room to detect a truncated label before applying a prefix rule.
    wchar_t name[2048]{};
    TVITEMW request{};
    request.mask = TVIF_TEXT;
    request.hItem = item;
    request.pszText = name;
    request.cchTextMax = std::size(name);
    if (!SendMessageW(tree, TVM_GETITEMW, 0,
                      reinterpret_cast<LPARAM>(&request)) ||
        name[std::size(name) - 2] != L'\0') {
        return {};
    }
    return name;
}

void FindHiddenEntries(HWND tree,
                       HTREEITEM first,
                       bool desktopMayContainSections,
                       const Rules& rules,
                       std::vector<HTREEITEM>& hidden) {
    for (HTREEITEM item = first; item; item = RelatedItem(tree, TVGN_NEXT, item)) {
        std::wstring label = ItemLabel(tree, item);
        if (desktopMayContainSections &&
            (EqualText(label, L"Desktop") || EqualText(label, L"Рабочий стол"))) {
            if (HTREEITEM child = RelatedItem(tree, TVGN_CHILD, item)) {
                FindHiddenEntries(tree, child, false, rules, hidden);
                continue;
            }
        }
        if (!label.empty() && std::ranges::any_of(rules, [&](const Rule& rule) {
                return Matches(label, rule);
            })) {
            hidden.push_back(item);
        }
    }
}

void ApplyFilters(HWND tree, const Rules& rules) {
    std::vector<HTREEITEM> hidden;
    FindHiddenEntries(tree, RelatedItem(tree, TVGN_ROOT), true, rules, hidden);
    for (HTREEITEM item : hidden) {
        // Leave viewport and selection handling to the TreeView.
        SendMessageW(tree, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(item));
    }
}

// This helper is used both by the GUI thread and during settings reload.
void QueueFilter(HWND tree) {
    DWORD processId = 0;
    GetWindowThreadProcessId(tree, &processId);
    if (processId != GetCurrentProcessId() || !IsNavigationTree(tree) ||
        GetPropW(tree, kPendingProperty)) {
        return;
    }

    // The message runs on the owning GUI thread after Explorer finishes
    // processing the insertion or label update that triggered it.
    HANDLE marker = reinterpret_cast<HANDLE>(1);
    if (SetPropW(tree, kPendingProperty, marker) &&
        !PostMessageW(tree, g_applyMessage, 0, 0)) {
        RemovePropW(tree, kPendingProperty);
    }
}

bool CanUpdateLabels(UINT message) {
    switch (message) {
        case TVM_INSERTITEMW:
        case TVM_INSERTITEMA:
        case TVM_SETITEMW:
        case TVM_SETITEMA:
            return true;
        default:
            return false;
    }
}

std::shared_ptr<const Rules> CurrentRules() {
    std::lock_guard lock(g_rulesMutex);
    return g_rules;
}

LRESULT WINAPI SendMessageHook(HWND hwnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam) {
    LRESULT result = g_sendMessage(hwnd, message, wParam, lParam);
    if (result && CanUpdateLabels(message) && CurrentRules()) {
        DWORD lastError = GetLastError();
        QueueFilter(hwnd);
        SetLastError(lastError);
    }
    return result;
}

LRESULT WINAPI DispatchMessageHook(const MSG* message) {
    if (message && message->message == g_applyMessage && message->hwnd) {
        DWORD lastError = GetLastError();
        HWND tree = message->hwnd;
        if (GetWindowThreadProcessId(tree, nullptr) == GetCurrentThreadId() &&
            RemovePropW(tree, kPendingProperty)) {
            if (IsNavigationTree(tree)) {
                if (const auto rules = CurrentRules()) {
                    ApplyFilters(tree, *rules);
                }
            }
            SetLastError(lastError);
            return 0;
        }
        SetLastError(lastError);
    }
    return g_dispatchMessage(message);
}

void UpdateOpenWindows(bool clearPending = false) {
    EnumWindows(
        [](HWND window, LPARAM clearPending) -> BOOL {
            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);
            if (processId == GetCurrentProcessId() &&
                HasClass(window, L"CabinetWClass")) {
                EnumChildWindows(
                    window,
                    [](HWND child, LPARAM clearPending) -> BOOL {
                        if (clearPending) {
                            RemovePropW(child, kPendingProperty);
                        } else {
                            QueueFilter(child);
                        }
                        return TRUE;
                    },
                    clearPending);
            }
            return TRUE;
        },
        clearPending);
}

}  // namespace

BOOL Wh_ModInit() {
    g_applyMessage = RegisterWindowMessageW(kRequestName);
    return g_applyMessage &&
           WindhawkUtils::SetFunctionHook(SendMessageW, SendMessageHook,
                                          &g_sendMessage) &&
           WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageHook,
                                          &g_dispatchMessage);
}

void Wh_ModAfterInit() {
    auto rules = ReadRules();
    {
        std::lock_guard lock(g_rulesMutex);
        g_rules = rules;
    }
    if (rules) {
        UpdateOpenWindows();
    }
}

void Wh_ModSettingsChanged() {
    Wh_ModAfterInit();
}

void Wh_ModBeforeUninit() {
    std::lock_guard lock(g_rulesMutex);
    g_rules.reset();
}

void Wh_ModUninit() {
    UpdateOpenWindows(true);
}
