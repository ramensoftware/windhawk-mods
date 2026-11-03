// ==WindhawkMod==
// @id start-menu-hide-empty-categories
// @name Start Menu Hide Empty Categories
// @name:pt-BR Ocultar Categorias Vazias do Menu Iniciar
// @description Hides the empty category tiles of the Windows 11 Start menu's "All apps" category view and closes the gaps they leave.
// @description:pt-BR Oculta os blocos de categoria vazios da visualização por categorias de "Todos os aplicativos" do Menu Iniciar do Windows 11 e fecha os espaços que eles deixam.
// @version 1.0.0
// @author Feuer
// @github https://github.com/feuerdraws
// @twitter https://x.com/feuerdraws
// @include explorer.exe
// @include StartMenuExperienceHost.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lruntimeobject -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Hide Empty Categories

> **Disclaimer:** this mod is vibe coded. It was written with an AI assistant and tested only by its author on
> one setup. It hooks private parts of the Start menu and restarts the Start menu process, so use it at your own risk.

In the Windows 11 Start menu's category view of **All apps**, a heading that has no apps still shows up as an empty
tile. This mod hides those tiles and shows them again as soon as an app is assigned to the heading. The tiles after
them move up to fill the gap.

It is the empty-tile part of **Start Menu Category Manager**, split out so it can be used on its own, for example
after uninstalling the manager. It does not change any category, does not touch the registry, and has no settings.
**Do not run it together with the manager unless you turn off its *Hide empty category tiles* setting:** the manager
already does the same thing, and both would work on the same tiles. If both are hiding tiles at the same time, the
manager notices (this mod tells it that it is active) and shows a warning.

Whenever the mod is loaded (installed, updated, switched on again, or Explorer restarted), it restarts the Start menu
process if that process started before the mod. The new process loads with the mod from its start, so empty tiles are
hidden from the first time the menu is opened. When the mod is switched off or removed, the Start menu process is
restarted once more so the tiles it had hidden come back. Start can flash or close for a moment each time. A Start
process that started within the last second is left alone.

**The *Other* category.** If *Other* is empty, Windows 11 still shows an empty *Other* container. This mod hides it
too, but only while the mod is loaded.

**If you uninstall the manager first.** With its default settings the manager puts every app it changed back to the
category it had before, and then this mod keeps hiding the tiles that end up empty. If you turned on **Keep modified
categories when this mod is unloaded** in the manager, your categories stay as they are.

## Tested on

Only Windows 11 build 26300, on one computer, with Windows in Brazilian Portuguese. On any other build the mod may
fail to attach; it then switches only the affected part off (see below).

**Languages.** The mod recognises category headings in English and Brazilian Portuguese only. The Brazilian
Portuguese headings were taken from Start itself. The English ones are translations that have not been checked against
Start in English, so some may not match, and an empty tile whose heading does not match stays visible. With other
display languages the mod does nothing.

## If a Windows update breaks the mod

The mod depends on private parts of the Start menu that Microsoft can change without notice. It was written and tested
on Windows 11 build 26300. When something stops matching, the mod keeps running and switches only the affected part
off.

- **Empty category tiles stay visible.** The hooks did not attach. The Windhawk log (lines starting with `[start]`)
  says so.
- **Empty tiles are hidden, but the other tiles do not move up.** Only the optional `ReplaceAll` hooks were not found.

Where to look: the Windhawk log. The functions the mod hooks, all inside the Start menu DLL (`StartMenu.dll`, or
`StartUI.dll` on some builds), are:

1. `StartBlendedFlexFrame::OnContextRequested`, `CategoryProvider::PruneAndCombineCategories` and
   `CategoryProvider::CombineCategories`, which find the empty tiles.
2. Two `ReplaceAll` functions of the category list (`AllAppsGroupViewModel`), which are optional and only make the
   tiles close up.

To find the new names after an update, search the Start menu DLL's symbols in Windhawk for those class and function
names. The mod also reads Start's internal category map at fixed byte offsets (the constants `kCategoryMap...` and
`kCategoryNode...` in the source), checked on build 26300. If the log reports a "Category map walk mismatch", those
offsets no longer fit and need to be re-checked for the new build.
*/
// ==/WindhawkModReadme==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <sddl.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <cwchar>
#include <cwctype>
#include <set>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Data;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

namespace {

using IInspectableList = wf::Collections::IVector<wf::IInspectable>;

std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_startModuleHooked = false;
bool g_isStartHost = false;  // True inside StartMenuExperienceHost.exe, false inside explorer.exe.

// The Windows build the category map offsets (kCategoryMap...Offset and kCategoryNode...Offset) were checked on.
constexpr wchar_t kOffsetsVerifiedBuild[] = L"26300";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

std::wstring WindowsBuildString() {
    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
        auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion) rtlGetVersion(&info);
    }
    return std::to_wstring(info.dwBuildNumber);
}

// Lets Start Menu Category Manager know that this mod is hiding empty tiles, so it can warn about both doing it. The
// Start menu process is sandboxed, so it reports through a named event, the same way the manager's own hook status
// works: Explorer creates the event, and the Start menu copy of this mod signals it once its tile hooks are in. The
// name and the security descriptor must match what the manager expects.
HANDLE g_activeEvent = nullptr;  // Explorer only.

std::wstring ActiveEventName() {
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    return L"Global\\WindhawkHideEmptyCategories-" + std::to_wstring(session) + L"-Active";
}

// Explorer: creates the event. Called before Start is restarted, so the new Start process finds it.
void CreateActiveEvent() {
    SECURITY_ATTRIBUTES sa{sizeof(sa)};
    PSECURITY_DESCRIPTOR sd = nullptr;
    // Everyone and all app packages may wait on it and signal it.
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;0x100002;;;WD)(A;;0x100002;;;AC)", SDDL_REVISION_1,
                                                              &sd, nullptr)) {
        sa.lpSecurityDescriptor = sd;
    }
    const std::wstring name = ActiveEventName();
    g_activeEvent = CreateEventW(&sa, TRUE, FALSE, name.c_str());
    if (!g_activeEvent && GetLastError() == ERROR_ACCESS_DENIED) {
        // The event of a previous Explorer that is still shutting down: join it, and drop its old answer.
        g_activeEvent = OpenEventW(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name.c_str());
        if (g_activeEvent) ResetEvent(g_activeEvent);
    }
    if (sd) LocalFree(sd);
}

// Start menu process: sets the event when the tile hooks are in, and clears it when Start starts.
void SetActiveEvent(bool active) {
    if (HANDLE handle = OpenEventW(EVENT_MODIFY_STATE, FALSE, ActiveEventName().c_str())) {
        if (active) {
            SetEvent(handle);
        } else {
            ResetEvent(handle);
        }
        CloseHandle(handle);
    }
}

// Terminates the Start menu process so Windows starts a fresh one. A process that started within the last second
// is left alone.
void RestartStartMenuHost() {
    static std::atomic_flag s_busy = ATOMIC_FLAG_INIT;
    if (s_busy.test_and_set()) return;

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
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                              entry.th32ProcessID);
                if (!hProcess) continue;
                FILETIME created{}, exited{}, kernel{}, user{};
                bool tooYoung = false;
                if (GetProcessTimes(hProcess, &created, &exited, &kernel, &user)) {
                    const ULONGLONG start = (static_cast<ULONGLONG>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
                    tooYoung = now > start && now - start < kMinAge;
                }
                if (!tooYoung) TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    s_busy.clear();
}

// ---------------------------------------------------------------------------
// Category headings
// ---------------------------------------------------------------------------

// Every heading a category tile can have, in English and in Brazilian Portuguese. A tile is only ever touched when
// its title is one of these, so app tiles and anything else in the grid are never hidden.
struct Heading {
    const wchar_t* en;
    const wchar_t* pt;
};

constexpr Heading kHeadings[] = {
    {L"Other", L"Outro"},
    {L"Accessibility", L"Acessibilidade"},
    {L"Games", L"Jogos"},
    {L"Productivity", L"Produtividade"},
    {L"Creativity", L"Criatividade"},
    {L"Utilities & Tools", L"Utilitários e Ferramentas"},
    {L"Social", L"Social"},
    {L"Music", L"Música"},
    {L"Sports", L"Esportes"},
    {L"Information & Reading", L"Informações e Leitura"},
    {L"Developer Tools", L"Ferramentas de Desenvolvedor"},
    {L"Developer Tools", L"Ferramentas de Desenvolvimento"},
    {L"Business", L"Comercial"},
    {L"Education", L"Educação"},
    {L"Entertainment", L"Entretenimento"},
    {L"Food & Dining", L"Bares e Restaurantes"},
    {L"Kids + Family", L"Crianças + Família"},
    {L"Lifestyle", L"Estilo de vida"},
    {L"Travel", L"Viagem"},
    {L"Medicine", L"Medicina"},
    {L"Health & Wellness", L"Saúde e Bem-estar"},
    {L"Personal Finance", L"Finanças Pessoais"},
    {L"Shopping", L"Compras"},
};

// The English heading of a category id, or nullptr for an id that is not known.
const wchar_t* CategoryNameEn(DWORD id) {
    switch (id) {
        case 0: return L"Other";
        case 1: return L"Accessibility";
        case 2: case 5: case 13: return L"Productivity";
        case 3: return L"Games";
        case 27: return L"Lifestyle";
        case 4: return L"Travel";
        case 11: return L"Medicine";
        case 12: return L"Health & Wellness";
        case 18: return L"Personal Finance";
        case 28: return L"Shopping";
        case 6: case 7: case 22: case 23: case 30: return L"Creativity";
        case 8: return L"Social";
        case 9: return L"Utilities & Tools";
        case 10: return L"Kids + Family";
        case 14: case 21: case 24: case 25: case 31: return L"Information & Reading";
        case 15: return L"Developer Tools";
        case 16: return L"Entertainment";
        case 17: return L"Music";
        case 26: return L"Sports";
        case 19: return L"Education";
        case 20: return L"Business";
        case 29: return L"Food & Dining";
        default: return nullptr;
    }
}

// Titles are compared using only plain letters, digits and spaces. Start may store accented
// letters differently from this file, and the accent is not needed to tell the headings apart.
std::wstring TileKey(const std::wstring& text) {
    std::wstring key;
    for (wchar_t c : text) {
        if (c < 128 && (iswalnum(c) || c == L' ')) key.push_back(c);
    }
    return key;
}

bool IsTileHeading(const std::wstring& text) {
    const std::wstring key = TileKey(text);
    for (const Heading& heading : kHeadings) {
        if (key == TileKey(heading.en) || key == TileKey(heading.pt)) return true;
    }
    return false;
}

// Adds the keys of a category's heading, in both languages, to `keys`.
void AddHeadingKeys(const wchar_t* en, std::set<std::wstring>& keys) {
    for (const Heading& heading : kHeadings) {
        if (wcscmp(heading.en, en) != 0) continue;
        keys.insert(TileKey(heading.en));
        keys.insert(TileKey(heading.pt));
    }
}

// ---------------------------------------------------------------------------
// Hiding empty category tiles (runs inside StartMenuExperienceHost.exe)
// ---------------------------------------------------------------------------

std::wstring ElementClassName(wux::DependencyObject const& obj) {
    try {
        return std::wstring(winrt::get_class_name(obj).c_str());
    } catch (...) {
        return L"";
    }
}

// The images inside a tile, and its title (the first text block).
struct TileContent {
    std::wstring title;
    int images = 0;
};

void CountTileContent(wux::DependencyObject const& obj, int depth, TileContent& out) {
    if (depth > 40) return;
    if (ElementClassName(obj) == L"Windows.UI.Xaml.Controls.Image") {
        ++out.images;
    } else if (auto textBlock = obj.try_as<wuxc::TextBlock>()) {
        if (out.title.empty()) out.title = std::wstring(textBlock.Text().c_str());
    }
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(obj);
    for (int i = 0; i < count; ++i) {
        CountTileContent(wuxm::VisualTreeHelper::GetChild(obj, i), depth + 1, out);
    }
}

// Start keeps each category's apps in a map, and prunes it every time it rebuilds the categories.
// Layout: the entry count is at offset 16 and the first list node at
// offset 8. Each node holds its next pointer at offset 0, its category key at offset 16, and the
// begin and end pointers of its app list at offsets 24 and 32 (8 bytes per app).
constexpr size_t kCategoryMapHeadOffset = 8;
constexpr size_t kCategoryMapSizeOffset = 16;
constexpr size_t kCategoryNodeNextOffset = 0;
constexpr size_t kCategoryNodeKeyOffset = 16;
constexpr size_t kCategoryNodeBeginOffset = 24;
constexpr size_t kCategoryNodeEndOffset = 32;
constexpr int kMaxCategoryNodes = 512;

// The headings (in both languages) of the categories that currently have no apps.
// Only tiles with these titles are ever hidden.
std::set<std::wstring> g_emptyHeadings;
// False until Start has rebuilt its categories once in this session.
bool g_mapKnown = false;
// How many times Start has rebuilt its categories since this mod loaded, and whether the first rebuild was the one
// Start does when it starts up (the process was young), before anyone opened the menu.
std::atomic<int> g_rebuildCount{0};
bool g_startupCleanupAllowed = false;

// Reads through ReadProcessMemory instead of dereferencing after a VirtualQuery check. A check and a read cannot be
// made atomic, so memory that is freed in between would still fault. ReadProcessMemory on this process copies inside
// the kernel and reports bad memory as a failure instead of raising an exception.
bool ReadWordAt(const void* base, size_t offset, unsigned long long* out) {
    if (!base) return false;
    const void* p = static_cast<const unsigned char*>(base) + offset;
    unsigned long long value = 0;
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), p, &value, sizeof(value), &got) || got != sizeof(value)) return false;
    *out = value;
    return true;
}

// Reads Start's category map and records the headings of the categories with no apps.
void RecordEmptyCategories(void* map) {
    unsigned long long head = 0;
    unsigned long long declaredSize = 0;
    if (!ReadWordAt(map, kCategoryMapHeadOffset, &head) ||
        !ReadWordAt(map, kCategoryMapSizeOffset, &declaredSize)) {
        Wh_Log(L"[start] Could not read the category map");
        return;
    }

    std::set<std::wstring> empty;
    int walked = 0;
    unsigned long long node = 0;
    if (ReadWordAt(reinterpret_cast<void*>(head), kCategoryNodeNextOffset, &node)) {
        while (node != head && node != 0 && walked < kMaxCategoryNodes) {
            const void* n = reinterpret_cast<void*>(node);
            unsigned long long key = 0, begin = 0, end = 0, next = 0;
            if (!ReadWordAt(n, kCategoryNodeKeyOffset, &key) ||
                !ReadWordAt(n, kCategoryNodeBeginOffset, &begin) ||
                !ReadWordAt(n, kCategoryNodeEndOffset, &end) ||
                !ReadWordAt(n, kCategoryNodeNextOffset, &next)) {
                break;
            }
            const unsigned long long count = end >= begin ? (end - begin) / 8 : 0;
            if (count == 0) {
                if (const wchar_t* en = CategoryNameEn(static_cast<DWORD>(key & 0xFFFFFFFFull))) {
                    AddHeadingKeys(en, empty);
                }
            }
            node = next;
            ++walked;
        }
    }

    if (static_cast<unsigned long long>(walked) != declaredSize) {
        Wh_Log(L"[start] Category map walk mismatch: declared %ls, walked %ls (Windows build %ls; the offsets were checked on build %ls)",
               std::to_wstring(declaredSize).c_str(), std::to_wstring(walked).c_str(),
               WindowsBuildString().c_str(), kOffsetsVerifiedBuild);
        return;  // Keep the last good result rather than guess.
    }
    g_emptyHeadings = empty;
    g_mapKnown = true;
}

// Shows a category tile when it holds apps, and hides it when it is empty.
void ApplyCategoryTiles(wux::DependencyObject const& grid) {
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(grid);
    for (int i = 0; i < count; ++i) {
        auto item = wuxm::VisualTreeHelper::GetChild(grid, i);
        if (ElementClassName(item) != L"Windows.UI.Xaml.Controls.GridViewItem") continue;

        TileContent content;
        CountTileContent(item, 0, content);
        if (!IsTileHeading(content.title)) continue;

        auto element = item.try_as<wux::UIElement>();
        if (!element) continue;

        // Until Start has rebuilt its categories in this session, the map has not been read, so fall
        // back to the icon count. Once the map is known, it decides.
        const bool empty = g_mapKnown ? g_emptyHeadings.count(TileKey(content.title)) > 0
                                      : content.images == 0;
        const wux::Visibility wanted = empty ? wux::Visibility::Collapsed : wux::Visibility::Visible;
        if (element.Visibility() != wanted) element.Visibility(wanted);
    }
}

// Removes the entry of an empty category from the list the grid is bound to, so the other tiles move up. Only done for
// the list Start builds when it starts up, while nobody has opened the menu: the same removal from a timer right
// after the rebuild that happens when the menu opens crashed Start (access violation inside StartMenu.dll, 30 to
// 50 ms later). Those later lists are cleaned inside Start's own ReplaceAll call instead.
void RemoveEmptyTilesFromSource(wux::DependencyObject const& grid) {
    if (!g_startupCleanupAllowed || g_rebuildCount != 1) return;
    if (!g_mapKnown) return;
    try {
        wux::DependencyObject walker = grid;
        wux::DependencyObject gridView{nullptr};
        for (int i = 0; i < 10 && !gridView; ++i) {
            walker = wuxm::VisualTreeHelper::GetParent(walker);
            if (!walker) return;
            if (ElementClassName(walker) == L"Windows.UI.Xaml.Controls.GridView") gridView = walker;
        }
        if (!gridView) return;
        auto items = gridView.try_as<wuxc::ItemsControl>();
        if (!items) return;
        auto source = items.ItemsSource();
        auto list = source ? source.try_as<IInspectableList>() : nullptr;
        if (!list) return;

        std::vector<wf::IInspectable> toRemove;
        const int count = wuxm::VisualTreeHelper::GetChildrenCount(grid);
        for (int i = 0; i < count; ++i) {
            auto child = wuxm::VisualTreeHelper::GetChild(grid, i);
            if (ElementClassName(child) != L"Windows.UI.Xaml.Controls.GridViewItem") continue;
            TileContent content;
            CountTileContent(child, 0, content);
            if (!IsTileHeading(content.title) || g_emptyHeadings.count(TileKey(content.title)) == 0) continue;
            auto element = child.try_as<wux::FrameworkElement>();
            auto entry = element ? element.DataContext() : nullptr;
            if (entry) toRemove.push_back(entry);
        }
        for (const auto& entry : toRemove) {
            for (uint32_t j = 0; j < list.Size(); ++j) {
                if (list.GetAt(j) == entry) {
                    list.RemoveAt(j);
                    break;
                }
            }
        }
    } catch (...) {
        static bool loggedFailure = false;
        if (!loggedFailure) {
            loggedFailure = true;
            Wh_Log(L"[start] Could not remove the empty category from the grid's list");
        }
    }
}

void FindItemGrids(wux::DependencyObject const& obj, int depth) {
    if (depth > 60) return;
    if (ElementClassName(obj) == L"Windows.UI.Xaml.Controls.ItemsWrapGrid") {
        ApplyCategoryTiles(obj);
        RemoveEmptyTilesFromSource(obj);
    }
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(obj);
    for (int i = 0; i < count; ++i) {
        FindItemGrids(wuxm::VisualTreeHelper::GetChild(obj, i), depth + 1);
    }
}

wux::DependencyObject g_startRoot{nullptr};

void UpdateCategoryTiles() {
    try {
        wux::DependencyObject root{nullptr};
        if (auto window = wux::Window::Current()) {
            if (auto content = window.Content()) root = content;
        }
        if (!root && g_startRoot) root = g_startRoot;  // Fallback: the frame from the last right-click.
        if (!root) return;
        FindItemGrids(root, 0);
    } catch (...) {
        g_startRoot = nullptr;  // The tree was torn down; look for a fresh root next time.
    }
}

// The check runs on a timer inside Start's own window, so it also works when the menu was already
// open before the mod loaded. The window's procedure is replaced with a plain one that handles the
// timer and passes everything else on. Only basic Windows calls are used (no comctl32 subclassing,
// which needs a newer library version than Windhawk mods load by default).
constexpr UINT_PTR kTileTimerId = 0x5448;
constexpr UINT kTileTimerMs = 250;
HWND g_tileWindow = nullptr;
WNDPROC g_tileOriginalProc = nullptr;

// SetTimer needs the window's own thread, and Wh_ModInit may run on another one. In that case a
// private message makes the window's thread (through TileWindowProc) start the timer itself.
UINT TileStartMessage() {
    static const UINT message = RegisterWindowMessageW(L"WindhawkStartHideEmptyCategories.TileTimerStart");
    return message;
}

void StartTileTimer(HWND hwnd) {
    if (!SetTimer(hwnd, kTileTimerId, kTileTimerMs, nullptr)) {
        Wh_Log(L"[start] Could not start the tile timer (error %lu)", GetLastError());
    }
}

bool IsStartCoreWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    wchar_t cls[128]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

LRESULT CALLBACK TileWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == kTileTimerId && !g_shuttingDown) {
        UpdateCategoryTiles();  // Runs on the window's own thread, which may use the UI.
    } else if (msg == TileStartMessage() && !g_shuttingDown) {
        StartTileTimer(hwnd);
        return 0;
    }
    return CallWindowProcW(g_tileOriginalProc, hwnd, msg, wParam, lParam);
}

BOOL CALLBACK AttachTileWindowProc(HWND hwnd, LPARAM) {
    if (g_tileWindow || !IsStartCoreWindow(hwnd)) return TRUE;
    WNDPROC previous = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TileWindowProc)));
    if (!previous) return TRUE;
    g_tileOriginalProc = previous;
    g_tileWindow = hwnd;
    if (GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId()) {
        StartTileTimer(hwnd);
    } else {
        PostMessageW(hwnd, TileStartMessage(), 0, 0);
    }
    return TRUE;
}

void StartCategoryTileWatch() {
    if (g_tileWindow) return;
    EnumWindows(AttachTileWindowProc, 0);
}

void StopCategoryTileWatch() {
    if (!g_tileWindow) return;
    KillTimer(g_tileWindow, kTileTimerId);
    // Put Start's window procedure back, but only if ours is still the one in place. If another mod
    // (for example Start Menu Styler) replaced the procedure after ours, it holds a pointer to ours
    // as its "previous" one, so taking ours out would break its chain. In that case ours stays: it only
    // passes messages on (g_shuttingDown is set), and this module stays loaded (see Wh_ModInit).
    if (reinterpret_cast<WNDPROC>(GetWindowLongPtrW(g_tileWindow, GWLP_WNDPROC)) == TileWindowProc) {
        SetWindowLongPtrW(g_tileWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_tileOriginalProc));
    }
    g_tileWindow = nullptr;
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using StartFrameContextRequested_t = void(__cdecl*)(void*, void*, void*);
StartFrameContextRequested_t StartFrameContextRequested_Original = nullptr;

// Start's right-click handler runs on the UI thread, so it also starts the tile watch.
void __cdecl StartFrameContextRequested_Hook(void* self, void* sender, void* args) {
    try {
        auto const& element = *static_cast<wux::UIElement const*>(sender);
        g_startRoot = element;
    } catch (...) {
    }
    StartCategoryTileWatch();
    StartFrameContextRequested_Original(self, sender, args);
}

using PruneAndCombine_t = void(__cdecl*)(void*, void*);
PruneAndCombine_t PruneAndCombine_Original = nullptr;

// Start rebuilds its categories when it opens, on the UI thread, so this is where the tile check
// starts without a right-click.
void __cdecl PruneAndCombine_Hook(void* self, void* map) {
    if (++g_rebuildCount == 1) {
        FILETIME created{}, exited{}, kernel{}, user{}, now{};
        GetSystemTimeAsFileTime(&now);
        if (GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
            const ULONGLONG start = (static_cast<ULONGLONG>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
            const ULONGLONG current = (static_cast<ULONGLONG>(now.dwHighDateTime) << 32) | now.dwLowDateTime;
            const ULONGLONG ageMs = current > start ? (current - start) / 10000 : 0;
            g_startupCleanupAllowed = ageMs < 10000;
        }
    }
    StartCategoryTileWatch();
    PruneAndCombine_Original(self, map);
    RecordEmptyCategories(map);
    UpdateCategoryTiles();
}

using CombineCategories_t = void(__cdecl*)(void*, void*, void*, bool);
CombineCategories_t CombineCategories_Original = nullptr;

void __cdecl CombineCategories_Hook(void* self, void* map, void* merges, bool flag) {
    CombineCategories_Original(self, map, merges, flag);
    RecordEmptyCategories(map);
}

// Removing entries from Start's list from a timer right after the rebuild that happens when the menu opens crashes
// Start: it faults inside StartMenu.dll 30 to 50 ms later. Removing them inside Start's own ReplaceAll call, before it
// does anything else with the list, does not. So that is where empty entries are removed from those lists. The one
// list that never goes through such a call here is the one built when Start starts up (see RemoveEmptyTilesFromSource).

// Removes empty category groups from the list that Start has just filled, before the menu draws it.
// `abi` is the ABI pointer of the list.
void RemoveEmptyGroupsFromAbi(void* abi) {
    try {
        unsigned long long vtable = 0;
        if (!ReadWordAt(abi, 0, &vtable)) return;
        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, abi);
        auto items = inspectable.try_as<IInspectableList>();
        if (!items) {
            static bool loggedNotList = false;
            if (!loggedNotList) {
                loggedNotList = true;
                Wh_Log(L"[start] ReplaceAll was given something that is not a list; the hook's layout assumption may be wrong for this build");
            }
            return;
        }
        for (uint32_t i = items.Size(); i-- > 0;) {
            auto group = items.GetAt(i).try_as<wuxd::ICollectionViewGroup>();
            if (group && group.GroupItems().Size() == 0) items.RemoveAt(i);
        }
    } catch (...) {
        Wh_Log(L"[start] Removing the empty groups failed with an exception");
    }
}

// The C++ wrapper of the category list's ReplaceAll. It only covers callers that use the out-of-line copy of the
// wrapper. `list` is the address of the vector wrapper, whose first member is the ABI pointer; this layout is an
// assumption about Start's build, so it is read safely.
using ReplaceAll_t = void(__cdecl*)(void*, void*);
ReplaceAll_t ReplaceAll_Original = nullptr;

void __cdecl ReplaceAll_Hook(void* self, void* items) {
    ReplaceAll_Original(self, items);
    unsigned long long abi = 0;
    if (ReadWordAt(self, 0, &abi)) RemoveEmptyGroupsFromAbi(reinterpret_cast<void*>(abi));
}

// The ABI function every caller of ReplaceAll ends up in, including call sites where the wrapper above was compiled
// inline.
using ProduceReplaceAll_t = int(__cdecl*)(void*, uint32_t, void*);
ProduceReplaceAll_t ProduceReplaceAll_Original = nullptr;

int __cdecl ProduceReplaceAll_Hook(void* self, uint32_t count, void* items) {
    const int result = ProduceReplaceAll_Original(self, count, items);
    if (result >= 0) RemoveEmptyGroupsFromAbi(self);
    return result;
}

bool HookStartMenuDllSymbols(HMODULE module) {
    // StartMenu.dll, StartUI.dll
    WindhawkUtils::SYMBOL_HOOK startMenuDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::StartMenu::implementation::StartBlendedFlexFrame::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))",
            },
            &StartFrameContextRequested_Original,
            StartFrameContextRequested_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::StartMenu::implementation::CategoryProvider::PruneAndCombineCategories(class std::unordered_map<enum winrt::StartMenu::AppCategory,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> >,struct std::hash<enum winrt::StartMenu::AppCategory>,struct std::equal_to<enum winrt::StartMenu::AppCategory>,class std::allocator<struct std::pair<enum winrt::StartMenu::AppCategory const ,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> > > > > &))",
            },
            &PruneAndCombine_Original,
            PruneAndCombine_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::StartMenu::implementation::CategoryProvider::CombineCategories(class std::unordered_map<enum winrt::StartMenu::AppCategory,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> >,struct std::hash<enum winrt::StartMenu::AppCategory>,struct std::equal_to<enum winrt::StartMenu::AppCategory>,class std::allocator<struct std::pair<enum winrt::StartMenu::AppCategory const ,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> > > > > &,class std::map<enum winrt::StartMenu::AppCategory,class std::vector<enum winrt::StartMenu::AppCategory,class std::allocator<enum winrt::StartMenu::AppCategory> >,struct std::less<enum winrt::StartMenu::AppCategory>,class std::allocator<struct std::pair<enum winrt::StartMenu::AppCategory const ,class std::vector<enum winrt::StartMenu::AppCategory,class std::allocator<enum winrt::StartMenu::AppCategory> > > > > &,bool))",
            },
            &CombineCategories_Original,
            CombineCategories_Hook,
        },
        {
            // Optional. Windhawk may spell the pointer parts of this name in more than one way, so both
            // spellings are given.
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::impl::convertible_observable_vector<struct winrt::StartMenu::AllAppsGroupViewModel,class std::vector<struct winrt::StartMenu::AllAppsGroupViewModel,class std::allocator<struct winrt::StartMenu::AllAppsGroupViewModel> >,struct winrt::impl::single_threaded_collection_base>,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::StartMenu::AllAppsGroupViewModel> >::ReplaceAll(unsigned int,void * *) )",
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::impl::convertible_observable_vector<struct winrt::StartMenu::AllAppsGroupViewModel,class std::vector<struct winrt::StartMenu::AllAppsGroupViewModel,class std::allocator<struct winrt::StartMenu::AllAppsGroupViewModel> >,struct winrt::impl::single_threaded_collection_base>,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::StartMenu::AllAppsGroupViewModel> >::ReplaceAll(unsigned int,void *  * ) )",
            },
            &ProduceReplaceAll_Original,
            ProduceReplaceAll_Hook,
            true,
        },
        {
            // Optional. If a Windows update renames it, the other hooks keep working and only the gaps return.
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::StartMenu::AllAppsGroupViewModel>,struct winrt::StartMenu::AllAppsGroupViewModel>::ReplaceAll(struct winrt::array_view<struct winrt::StartMenu::AllAppsGroupViewModel const >)const )",
            },
            &ReplaceAll_Original,
            ReplaceAll_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, startMenuDllHooks, ARRAYSIZE(startMenuDllHooks))) {
        Wh_Log(L"[start] Windows build %ls: could not hook the Start menu; empty category tiles will not be hidden",
               WindowsBuildString().c_str());
        return false;
    }
    Wh_Log(L"[start] Windows build %ls: empty-tile hooks ok", WindowsBuildString().c_str());
    SetActiveEvent(true);
    if (!ProduceReplaceAll_Original) {
        Wh_Log(L"[start] The ABI ReplaceAll function was not found; empty tiles will be hidden but may leave gaps");
    }
    if (!ReplaceAll_Original) {
        Wh_Log(L"[start] The category list's ReplaceAll was not found; empty tiles will be hidden but may leave gaps");
    }
    return true;
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
    // Without this reset, the tile timer would stay switched off after the mod is disabled and enabled again
    // (the module stays loaded in the Start menu process, see below, so its old globals are reused).
    g_shuttingDown = false;

    wchar_t exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    g_isStartHost = _wcsicmp(PathFindFileNameW(exePath), L"StartMenuExperienceHost.exe") == 0;

    if (!g_isStartHost) {
        // explorer.exe only restarts the Start menu process, so it loads with the hooks from its start. A Start menu
        // process that was already running built its first category list before any hook was in place, so its empty
        // tiles would leave a gap until the menu was opened twice.
        CreateActiveEvent();  // Before Start is restarted below, so the new Start process can signal it.
        Wh_Log(L"Restarting the Start menu process (if it started before this mod) so it loads with the mod from the start");
        RestartStartMenuHost();
        return TRUE;
    }

    SetActiveEvent(false);  // Clears an old answer. It is set again once the tile hooks are in.

    // Keep this module loaded for the life of the Start menu process, so that Start's window procedure and timer can
    // never end up pointing into unloaded code.
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit), &self);

    Wh_Log(L"[start] Windows build %ls; the category map offsets were checked on build %ls",
           WindowsBuildString().c_str(), kOffsetsVerifiedBuild);
    if (HMODULE startModule = GetStartMenuModuleHandle()) {
        g_startModuleHooked = true;
        HookStartMenuDllSymbols(startModule);
    }

    StartCategoryTileWatch();

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
        ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"))
        : nullptr;
    if (loadLibraryExW) {
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
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
        StopCategoryTileWatch();
        return;  // Nothing else but the hooks lives in the Start menu process.
    }

    // Explorer: the tiles hidden so far, and the empty groups removed from Start's list, stay that way until Start
    // rebuilds them. Restarting the Start menu process brings everything back the way Windows made it.
    RestartStartMenuHost();

    // The event goes away with this handle, which tells the manager that this mod is no longer hiding tiles.
    if (g_activeEvent) CloseHandle(g_activeEvent);
    g_activeEvent = nullptr;
}
