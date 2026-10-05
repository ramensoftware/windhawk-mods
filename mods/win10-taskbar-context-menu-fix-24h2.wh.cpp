// ==WindhawkMod==
// @id              win10-taskbar-context-menu-fix-24h2
// @name            Windows 10 Taskbar Context Menu Fix for Win11 24H2+
// @description     Fixes context menu on Windows 10 taskbar running on Windows 11 24H2, 25H2 and later
// @version         1.7.0
// @author          Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>

#include <algorithm>
#include <vector>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

static HMODULE g_explorerModule;
static HMODULE g_shell32Module;
static HMODULE g_bthpropsModule;
static HMODULE g_explorerframeModule;

#define IDM_SHOWDESKTOP     0x197
#define IDM_TASKMANAGER     0x1A4
#define IDM_LOCKTASKBAR     0x1A8
#define IDM_SETTINGS        0x19D
#define IDM_LOCKTOOLBARS    41484

#define IDM_ORIG_CASCADE     0x193
#define IDM_ORIG_STACKED     0x194
#define IDM_ORIG_SIDEBYSIDE  0x195

#define IDM_LEGACY_CASCADE    0x7C71
#define IDM_LEGACY_STACKED    0x7C72
#define IDM_LEGACY_SIDEBYSIDE 0x7C73

#define IDS_SHOWDESKTOP     10113
#define IDS_TASKMANAGER     24743
#define IDS_SETTINGS        2128

// ---------------------------------------------------------------------------
// Language detection and hardcoded translations
// ---------------------------------------------------------------------------

enum UserLanguage {
    UL_UNKNOWN = 0,
    UL_ENGLISH, UL_ITALIAN, UL_GERMAN, UL_FRENCH, UL_SPANISH, UL_PORTUGUESE,
    UL_RUSSIAN, UL_CHINESE_SIMPLIFIED, UL_CHINESE_TRADITIONAL, UL_JAPANESE,
    UL_KOREAN, UL_DUTCH, UL_POLISH, UL_TURKISH, UL_CZECH,
    UL_COUNT
};

static UserLanguage g_userLang = UL_UNKNOWN;

static void DetectUserLanguage() {
    if (g_userLang != UL_UNKNOWN) return;
    LANGID langid = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(langid)) {
    case LANG_ENGLISH:    g_userLang = UL_ENGLISH;    break;
    case LANG_ITALIAN:    g_userLang = UL_ITALIAN;    break;
    case LANG_GERMAN:     g_userLang = UL_GERMAN;     break;
    case LANG_FRENCH:     g_userLang = UL_FRENCH;     break;
    case LANG_SPANISH:    g_userLang = UL_SPANISH;    break;
    case LANG_PORTUGUESE: g_userLang = UL_PORTUGUESE; break;
    case LANG_RUSSIAN:    g_userLang = UL_RUSSIAN;    break;
    case LANG_CHINESE:
        if (SUBLANGID(langid) == SUBLANG_CHINESE_SIMPLIFIED ||
            SUBLANGID(langid) == SUBLANG_CHINESE_SINGAPORE)
            g_userLang = UL_CHINESE_SIMPLIFIED;
        else
            g_userLang = UL_CHINESE_TRADITIONAL;
        break;
    case LANG_JAPANESE:   g_userLang = UL_JAPANESE;   break;
    case LANG_KOREAN:     g_userLang = UL_KOREAN;     break;
    case LANG_DUTCH:      g_userLang = UL_DUTCH;      break;
    case LANG_POLISH:     g_userLang = UL_POLISH;     break;
    case LANG_TURKISH:    g_userLang = UL_TURKISH;    break;
    case LANG_CZECH:      g_userLang = UL_CZECH;      break;
    default:              g_userLang = UL_ENGLISH;    break;
    }
    Wh_Log(L"[locale] detected language id: %u -> index %d", langid, (int)g_userLang);
}

enum HardcodedStringId {
    HSTR_SHOWDESKTOP = 0, HSTR_TASKMANAGER, HSTR_LOCKTOOLBARS, HSTR_SETTINGS,
    HSTR_CASCADE, HSTR_STACKED, HSTR_SIDEBYSIDE, HSTR_COUNT
};

static const wchar_t* g_hardcodedTranslations[HSTR_COUNT][UL_COUNT] = {
    { L"Show the desktop", L"Show the desktop", L"Mostra desktop", L"Desktop anzeigen",
      L"Afficher le bureau", L"Mostrar el escritorio", L"Mostrar a área de trabalho",
      L"Показать рабочий стол", L"显示桌面", L"顯示桌面", L"デスクトップの表示",
      L"바탕 화면 표시", L"Bureaublad weergeven", L"Pokaż pulpit", L"Masaüstünü göster",
      L"Zobrazit plochu" },
    { L"Task Manager", L"Task Manager", L"Gestione attività", L"Task-Manager",
      L"Gestionnaire des tâches", L"Administrador de tareas", L"Gerenciador de Tarefas",
      L"Диспетчер задач", L"任务管理器", L"工作管理員", L"タスク マネージャー",
      L"작업 관리자", L"Taakbeheer", L"Menedżer zadań", L"Görev Yöneticisi",
      L"Správce úloh" },
    { L"Lock the toolbars", L"Lock the toolbars", L"Blocca le barre degli strumenti",
      L"Symbolleisten fixieren", L"Verrouiller les barres d'outils",
      L"Bloquear las barras de herramientas", L"Bloquear as barras de ferramentas",
      L"Закрепить панели инструментов", L"锁定工具栏", L"鎖定工具列",
      L"ツールバーを固定する", L"도구 모음 잠금", L"Werkbalken vergrendelen",
      L"Zablokuj paski narzędzi", L"Araç çubuklarını kilitle",
      L"Uzamknout panely nástrojů" },
    { L"Taskbar settings", L"Taskbar settings",
      L"Impostazioni della barra delle applicazioni", L"Taskleiste-Einstellungen",
      L"Paramètres de la barre des tâches", L"Configuración de la barra de tareas",
      L"Configurações da barra de tarefas", L"Параметры панели задач", L"任务栏设置",
      L"工作列設定", L"タスクバーの設定", L"작업 표시줄 설정", L"Taakbalkinstellingen",
      L"Ustawienia paska zadań", L"Görev çubuğu ayarları",
      L"Nastavení hlavního panelu" },
    { L"Cascade windows", L"Cascade windows", L"Finestre a cascata",
      L"Fenster überlappend anordnen", L"Fenêtres en cascade", L"Ventanas en cascada",
      L"Janelas em cascata", L"Окна каскадом", L"层叠窗口", L"重疊顯示視窗",
      L"重ねて表示", L"창 계단식 배열", L"Vensters trapsgewijs schikken",
      L"Kaskadowe okna", L"Pencereleri basamakla",
      L"Kaskádově uspořádat okna" },
    { L"Show windows stacked", L"Show windows stacked", L"Finestre sovrapposte",
      L"Fenster gestapelt anzeigen", L"Afficher les fenêtres empilées",
      L"Mostrar ventanas apiladas", L"Mostrar janelas empilhadas",
      L"Отображать окна стопкой", L"堆叠显示窗口", L"堆疊顯示視窗",
      L"ウィンドウを重ねて表示", L"창 누적 배열", L"Vensters gestapeld weergeven",
      L"Pokaż okna w stosach", L"Pencereleri üst üste göster",
      L"Zobrazit okna skládaně" },
    { L"Show windows side by side", L"Show windows side by side", L"Finestre affiancate",
      L"Fenster nebeneinander anzeigen", L"Afficher les fenêtres côte à côte",
      L"Mostrar ventanas en paralelo", L"Mostrar janelas lado a lado",
      L"Отображать окна рядом", L"并排显示窗口", L"並排顯示視窗",
      L"ウィンドウを並べて表示", L"창 나란히 배열", L"Vensters naast elkaar weergeven",
      L"Pokaż okna obok siebie", L"Pencereleri yan yana göster",
      L"Zobrazit okna vedle sebe" }
};

static const wchar_t* GetHardcodedText(HardcodedStringId id) {
    if (id >= 0 && id < HSTR_COUNT && g_userLang >= 0 && g_userLang < UL_COUNT) {
        const wchar_t* text = g_hardcodedTranslations[id][g_userLang];
        if (text) return text;
    }
    return g_hardcodedTranslations[id][UL_ENGLISH];
}

// ---------------------------------------------------------------------------
// Label loading
// ---------------------------------------------------------------------------

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

static wchar_t* LoadStr(HMODULE hMod, UINT id, wchar_t* buf, int size) {
    if (hMod && LoadStringW(hMod, id, buf, size) > 0) return buf;
    return nullptr;
}

static const wchar_t kLegacyStore[] =
    L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore";

struct DataFileCacheEntry { wchar_t name[64]; HMODULE module; };
static DataFileCacheEntry g_dataFiles[12] = {};
static int g_dataFileCount = 0;
static bool g_storeLogged = false;
static wchar_t g_localeName[LOCALE_NAME_MAX_LENGTH] = {};
static wchar_t g_languageName[LOCALE_NAME_MAX_LENGTH] = {};

static void DetectLocaleNames() {
    if (g_localeName[0]) return;
    const LANGID langid = GetUserDefaultUILanguage();
    if (LCIDToLocaleName(MAKELCID(langid, SORT_DEFAULT), g_localeName,
                         LOCALE_NAME_MAX_LENGTH, 0) == 0)
        wcscpy_s(g_localeName, LOCALE_NAME_MAX_LENGTH, L"en-US");
    wcsncpy_s(g_languageName, LOCALE_NAME_MAX_LENGTH, g_localeName, _TRUNCATE);
    wchar_t* dash = wcschr(g_languageName, L'-');
    if (dash) *dash = 0;
}

static HMODULE LoadStoreFile(const wchar_t* fileName) {
    for (int i = 0; i < g_dataFileCount; i++)
        if (_wcsicmp(g_dataFiles[i].name, fileName) == 0) return g_dataFiles[i].module;
    if (g_dataFileCount >= (int)(sizeof(g_dataFiles) / sizeof(g_dataFiles[0]))) return nullptr;

    wchar_t expanded[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(kLegacyStore, expanded, _countof(expanded)) == 0 ||
        !expanded[0]) return nullptr;
    if (!g_storeLogged) {
        g_storeLogged = true;
        Wh_Log(L"[labels] looking for the Windows 10 labels in %s", expanded);
    }

    wchar_t path[MAX_PATH] = {};
    HMODULE module = nullptr;
    const wchar_t* folders[] = {g_localeName, g_languageName, L""};
    for (const wchar_t* folder : folders) {
        if (!folder[0]) {
            _snwprintf_s(path, _countof(path), _TRUNCATE, L"%s\\%s", expanded, fileName);
        } else {
            _snwprintf_s(path, _countof(path), _TRUNCATE, L"%s\\%s\\%s.mui",
                         expanded, folder, fileName);
        }
        module = LoadLibraryExW(path, nullptr,
                                LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (module) break;
        if (!folder[0]) continue;
        _snwprintf_s(path, _countof(path), _TRUNCATE, L"%s\\%s\\%s", expanded, folder, fileName);
        module = LoadLibraryExW(path, nullptr,
                                LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (module) break;
    }

    DataFileCacheEntry* entry = &g_dataFiles[g_dataFileCount++];
    wcsncpy_s(entry->name, _countof(entry->name), fileName, _TRUNCATE);
    entry->module = module;
    if (module) Wh_Log(L"[labels] %s found in the Windows 10 store", fileName);
    return module;
}

static wchar_t* LoadLabel(const wchar_t* storeFileName, HMODULE liveModule, UINT id,
                          wchar_t* buf, int size, const wchar_t** source) {
    HMODULE store = LoadStoreFile(storeFileName);
    if (LoadStr(store, id, buf, size)) { if (source) *source = L"the Windows 10 store"; return buf; }
    if (LoadStr(liveModule, id, buf, size)) { if (source) *source = L"the module of this process"; return buf; }
    if (source) *source = L"the hardcoded text of this mod";
    return nullptr;
}

static wchar_t* GetLockToolbarsText(wchar_t* buf, int size) {
    HMODULE modules[2] = {LoadStoreFile(L"explorerframe.dll"), g_explorerframeModule};
    const wchar_t* sources[2] = {L"explorerframe.dll (store)", L"explorerframe.dll (live)"};
    for (int s = 0; s < 2; s++) {
        HMODULE module = modules[s];
        if (!module || (s == 1 && module == modules[0])) continue;
        HMENU hMenu = LoadMenuW_Original(module, MAKEINTRESOURCEW(264));
        if (!hMenu) continue;
        for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
            MENUITEMINFOW mii = {}; mii.cbSize = sizeof(mii); mii.fMask = MIIM_SUBMENU;
            if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii) || !mii.hSubMenu) continue;
            for (int j = 0; j < GetMenuItemCount(mii.hSubMenu); j++) {
                MENUITEMINFOW sub = {}; sub.cbSize = sizeof(sub);
                sub.fMask = MIIM_ID | MIIM_STRING;
                wchar_t text[256] = {}; sub.dwTypeData = text; sub.cch = 255;
                if (GetMenuItemInfoW(mii.hSubMenu, j, TRUE, &sub) && sub.wID == IDM_LOCKTOOLBARS) {
                    wcsncpy_s(buf, size, text, _TRUNCATE);
                    DestroyMenu(hMenu);
                    Wh_Log(L"[labels] \"Lock the toolbars\" read from %s (menu 264)", sources[s]);
                    return buf;
                }
            }
        }
        DestroyMenu(hMenu);
    }
    return nullptr;
}

static wchar_t* ClassicEntryLabel(UINT originalId, wchar_t* buf, int size, const wchar_t** source) {
    HMODULE modules[8] = {}; int count = 0; int storeCount = 0;
    const wchar_t* storeFiles[] = {L"explorer.exe", L"explorer.exe.mui",
                                   L"shell32.dll", L"shell32.dll.mui"};
    for (const wchar_t* fileName : storeFiles) {
        HMODULE module = LoadStoreFile(fileName);
        if (!module) continue;
        if (count < (int)(sizeof(modules) / sizeof(modules[0]))) modules[count++] = module;
        storeCount = count;
    }
    if (g_shell32Module && count < (int)(sizeof(modules) / sizeof(modules[0])))
        modules[count++] = g_shell32Module;

    for (int i = 0; i < count; i++) {
        HMENU menu = LoadMenuW_Original(modules[i], MAKEINTRESOURCEW(205));
        if (!menu) continue;
        wchar_t* found = nullptr;
        for (int m = 0; m < GetMenuItemCount(menu) && !found; m++) {
            HMENU popup = GetSubMenu(menu, m);
            if (!popup) continue;
            for (int j = 0; j < GetMenuItemCount(popup); j++) {
                MENUITEMINFOW mii = {}; mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_ID | MIIM_STRING;
                wchar_t text[256] = {}; mii.dwTypeData = text; mii.cch = _countof(text) - 1;
                if (GetMenuItemInfoW(popup, j, TRUE, &mii) && mii.wID == originalId && text[0]) {
                    wcsncpy_s(buf, size, text, _TRUNCATE);
                    found = buf; break;
                }
            }
        }
        DestroyMenu(menu);
        if (found) {
            if (source) *source = (i < storeCount) ? L"the Windows 10 store"
                                                   : L"the module of this process";
            return found;
        }
    }
    if (source) *source = L"the hardcoded text of this mod";
    return nullptr;
}

enum ArrangeAction { ARRANGE_CASCADE = 0, ARRANGE_STACKED, ARRANGE_SIDEBYSIDE, ARRANGE_ACTION_COUNT };

static const UINT kClassicIds[ARRANGE_ACTION_COUNT] = {IDM_ORIG_CASCADE, IDM_ORIG_STACKED, IDM_ORIG_SIDEBYSIDE};
static const UINT kLegacyIds[ARRANGE_ACTION_COUNT]  = {IDM_LEGACY_CASCADE, IDM_LEGACY_STACKED, IDM_LEGACY_SIDEBYSIDE};

static const wchar_t* ArrangeActionName(int action) {
    switch (action) {
    case ARRANGE_CASCADE: return L"cascade windows";
    case ARRANGE_STACKED: return L"windows stacked";
    case ARRANGE_SIDEBYSIDE: return L"windows side by side";
    default: return L"unknown";
    }
}

static int ClassicCommandSlot(UINT id) {
    for (int i = 0; i < ARRANGE_ACTION_COUNT; i++)
        if (id == kClassicIds[i] || id == kLegacyIds[i]) return i;
    return -1;
}

static bool IsInterestingCommandId(UINT id) {
    return ClassicCommandSlot(id) >= 0 || (id >= 0x190 && id <= 0x1B0) ||
           (id >= 0x7C70 && id <= 0x7C7F);
}

static void AppendClassicEntry(HMENU popup, int slot, HardcodedStringId hstrId) {
    const UINT id = kClassicIds[slot];
    wchar_t buf[256] = {}; const wchar_t* source = nullptr;
    wchar_t* label = ClassicEntryLabel(id, buf, _countof(buf), &source);
    const wchar_t* text = label ? label : GetHardcodedText(hstrId);
    Wh_Log(L"[labels] entry %d: id %#x, action \"%s\", text \"%s\" (%s)", slot, id,
           ArrangeActionName(slot), text, source ? source : L"hardcoded");
    AppendMenuW(popup, MF_STRING, id, text);
}

// ---------------------------------------------------------------------------
// Arrange core
// ---------------------------------------------------------------------------

struct ArrangePlacement { HWND hwnd; RECT rect; };
struct ArrangeContext { HWND anchor; std::vector<HWND> windows; std::vector<HMONITOR> monitors; };

static bool SameMonitor(HWND hwnd, HMONITOR monitor) {
    return MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) == monitor;
}

// Permissive: only excludes windows that are definitely not meant to be
// rearranged by the classic menu entries. Everything else is included.
static bool IsArrangeableWindow(HWND hwnd, HWND anchor) {
    if (!hwnd || hwnd == anchor) return false;
    if (hwnd == GetShellWindow() || hwnd == GetDesktopWindow()) return false;

    // Top-level only.
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return false;

    // Must be a normal visible window.
    if (!IsWindowVisible(hwnd)) return false;
    if (IsIconic(hwnd)) return false;
    if (!IsWindowEnabled(hwnd)) return false;

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (exStyle & WS_EX_NOACTIVATE) return false;

    // Skip the shell's own hosts.
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, _countof(cls))) {
        if (!_wcsicmp(cls, L"Shell_TrayWnd") ||
            !_wcsicmp(cls, L"Shell_SecondaryTrayWnd") ||
            !_wcsicmp(cls, L"Progman") ||
            !_wcsicmp(cls, L"WorkerW") ||
            !_wcsicmp(cls, L"Windows.UI.Core.CoreWindow"))
            return false;
    }

    // Skip cloaked (other virtual desktops, suspended UWP).
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
        return false;

    return true;
}

static BOOL CALLBACK CollectArrangeableWindows(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<ArrangeContext*>(lParam);
    if (IsArrangeableWindow(hwnd, ctx->anchor)) {
        ctx->windows.push_back(hwnd);
        const HMONITOR own = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (std::find(ctx->monitors.begin(), ctx->monitors.end(), own) == ctx->monitors.end())
            ctx->monitors.push_back(own);
    }
    return TRUE;
}

static void ClampCascadeSize(HWND hwnd, const RECT& workArea, int* pcx, int* pcy) {
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME)) return;
    const int cxWork = workArea.right - workArea.left;
    const int cyWork = workArea.bottom - workArea.top;
    int cx = *pcx, cy = *pcy;
    cx = std::max(std::min(cx, cxWork * 5 / 7), cxWork * 4 / 7);
    cy = std::max(std::min(cy, cyWork * 5 / 7), cyWork * 4 / 7);
    MINMAXINFO mmi = {}; DWORD_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETMINMAXINFO, 0, (LPARAM)&mmi,
                            SMTO_ABORTIFHUNG, 1000, &result)) {
        if (mmi.ptMinTrackSize.x > 0) cx = std::max(cx, (int)mmi.ptMinTrackSize.x);
        if (mmi.ptMinTrackSize.y > 0) cy = std::max(cy, (int)mmi.ptMinTrackSize.y);
        if (mmi.ptMaxTrackSize.x > 0) cx = std::min(cx, (int)mmi.ptMaxTrackSize.x);
        if (mmi.ptMaxTrackSize.y > 0) cy = std::min(cy, (int)mmi.ptMaxTrackSize.y);
    }
    *pcx = cx; *pcy = cy;
}

static int CascadeGroup(const std::vector<HWND>& windows, const RECT& workArea,
                        std::vector<ArrangePlacement>& placements) {
    const int stepX = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXSIZE);
    const int stepY = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYSIZE);
    const bool resize = windows.size() > 1;
    int x = workArea.left, y = workArea.top, arranged = 0;
    for (int i = (int)windows.size() - 1; i >= 0; i--) {
        HWND hwnd = windows[i];
        if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);
        RECT rc = {}; if (!GetWindowRect(hwnd, &rc)) continue;
        int cx = rc.right - rc.left, cy = rc.bottom - rc.top;
        if (resize) ClampCascadeSize(hwnd, workArea, &cx, &cy);
        if (x + cx > workArea.right) x = workArea.left;
        if (y + cy > workArea.bottom) y = workArea.top;
        ArrangePlacement p; p.hwnd = hwnd;
        p.rect.left = x; p.rect.top = y;
        p.rect.right = x + cx; p.rect.bottom = y + cy;
        placements.push_back(p);
        x += stepX; y += stepY; arranged++;
    }
    return arranged;
}

static void ComputeTileCells(int count, const RECT& workArea, bool horizontal,
                             int* pColumns, int* pRows) {
    const int cxWork = workArea.right - workArea.left;
    const int cyWork = workArea.bottom - workArea.top;
    int columns = 1;
    if (horizontal) {
        const int minBand = 3 * GetSystemMetrics(SM_CYMIN);
        while (columns < count) {
            const int rows = (count + columns - 1) / columns;
            if (rows <= 1) break;
            if (cyWork / rows >= minBand && cxWork / columns >= GetSystemMetrics(SM_CXMIN)) break;
            columns++;
        }
    } else {
        while (columns * columns < count) columns++;
        while (columns > 1 && cxWork / columns < GetSystemMetrics(SM_CXMIN)) columns--;
    }
    *pColumns = std::max(1, std::min(columns, count));
    *pRows = (count + *pColumns - 1) / *pColumns;
}

static int TileGroup(const std::vector<HWND>& windows, const RECT& workArea, bool horizontal,
                     std::vector<ArrangePlacement>& placements) {
    const int count = (int)windows.size();
    if (count == 0) return 0;
    int columns = 1, rows = 1;
    ComputeTileCells(count, workArea, horizontal, &columns, &rows);
    const int cxWork = workArea.right - workArea.left;
    const int cyWork = workArea.bottom - workArea.top;
    const int cellW = cxWork / columns, cellH = cyWork / rows;
    int arranged = 0;
    for (int i = count - 1; i >= 0; i--) {
        HWND hwnd = windows[i];
        if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);
        const int index = count - 1 - i;
        const int column = horizontal ? (index / rows) : (index % columns);
        const int row = horizontal ? (index % rows) : (index / columns);
        int left = workArea.left + column * cellW;
        int top = workArea.top + row * cellH;
        int right = (column == columns - 1) ? workArea.right : left + cellW;
        int bottom = (row == rows - 1) ? workArea.bottom : top + cellH;
        int cx = right - left, cy = bottom - top;
        if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME) {
            MINMAXINFO mmi = {}; DWORD_PTR result = 0;
            if (SendMessageTimeoutW(hwnd, WM_GETMINMAXINFO, 0, (LPARAM)&mmi,
                                    SMTO_ABORTIFHUNG, 1000, &result)) {
                if (mmi.ptMinTrackSize.x > 0) cx = std::max(cx, (int)mmi.ptMinTrackSize.x);
                if (mmi.ptMinTrackSize.y > 0) cy = std::max(cy, (int)mmi.ptMinTrackSize.y);
            }
        }
        ArrangePlacement p; p.hwnd = hwnd;
        p.rect.left = left; p.rect.top = top;
        p.rect.right = left + cx; p.rect.bottom = top + cy;
        placements.push_back(p);
        arranged++;
    }
    return arranged;
}

// ---------------------------------------------------------------------------
// Local arrangement (primary path)
// ---------------------------------------------------------------------------

static HWND g_trayWindow = nullptr;

// Always uses the self-contained layout: the shell API on 24H2 does not
// enumerate explorer's top-level windows, so we build the placements ourselves
// and apply them with SetWindowPos. Verbose logging is left in on purpose, so
// the diagnostic is complete if something still does not work.
static int ArrangeLocally(int action, HWND anchor) {
    ArrangeContext ctx; ctx.anchor = anchor;
    EnumWindows(CollectArrangeableWindows, reinterpret_cast<LPARAM>(&ctx));

    Wh_Log(L"[arrange] %s: %d candidate(s) on %d monitor(s)",
           ArrangeActionName(action), (int)ctx.windows.size(), (int)ctx.monitors.size());
    for (HWND h : ctx.windows) {
        wchar_t cls[64] = {}; GetClassNameW(h, cls, _countof(cls));
        wchar_t title[128] = {}; GetWindowTextW(h, title, _countof(title));
        Wh_Log(L"[arrange]   candidate %p [%s] \"%s\"", h, cls, title);
    }

    if (ctx.windows.empty()) {
        Wh_Log(L"[arrange] %s: no arrangeable window", ArrangeActionName(action));
        return 0;
    }

    std::vector<HMONITOR> monitors = ctx.monitors;
    if (monitors.empty()) monitors.push_back(MonitorFromWindow(anchor, MONITOR_DEFAULTTOPRIMARY));

    std::vector<ArrangePlacement> placements;
    for (HMONITOR monitor : monitors) {
        std::vector<HWND> group;
        for (HWND hwnd : ctx.windows)
            if (SameMonitor(hwnd, monitor)) group.push_back(hwnd);
        if (group.empty()) continue;

        MONITORINFO mi = {sizeof(mi)};
        if (!GetMonitorInfoW(monitor, &mi)) {
            Wh_Log(L"[arrange] GetMonitorInfoW failed for monitor %p", monitor);
            continue;
        }

        std::vector<ArrangePlacement> groupPlacements;
        if (action == ARRANGE_CASCADE)
            CascadeGroup(group, mi.rcWork, groupPlacements);
        else
            TileGroup(group, mi.rcWork, action == ARRANGE_STACKED, groupPlacements);

        Wh_Log(L"[arrange] monitor %p: %d window(s) -> %d placement(s)",
               monitor, (int)group.size(), (int)groupPlacements.size());
        placements.insert(placements.end(), groupPlacements.begin(), groupPlacements.end());
    }

    if (placements.empty()) {
        Wh_Log(L"[arrange] %s: no placement produced", ArrangeActionName(action));
        return 0;
    }

    int moved = 0;
    for (const ArrangePlacement& p : placements) {
        if (SetWindowPos(p.hwnd, HWND_TOP, p.rect.left, p.rect.top,
                         p.rect.right - p.rect.left, p.rect.bottom - p.rect.top,
                         SWP_NOACTIVATE)) {
            moved++;
        } else {
            Wh_Log(L"[arrange] SetWindowPos(%p) failed, error %lu", p.hwnd, GetLastError());
        }
    }
    Wh_Log(L"[arrange] %s: moved %d of %d", ArrangeActionName(action), moved,
           (int)placements.size());
    if (moved > 0) SetForegroundWindow(placements.back().hwnd);
    return moved;
}

static WORD CallArrangeApi(int action) {
    switch (action) {
    case ARRANGE_CASCADE:
        return CascadeWindows(GetDesktopWindow(), MDITILE_SKIPDISABLED | MDITILE_ZORDER,
                              nullptr, 0, nullptr);
    case ARRANGE_STACKED:
        return TileWindows(GetDesktopWindow(), MDITILE_HORIZONTAL, nullptr, 0, nullptr);
    case ARRANGE_SIDEBYSIDE:
        return TileWindows(GetDesktopWindow(), MDITILE_VERTICAL, nullptr, 0, nullptr);
    default: return 0;
    }
}

static bool RunClassicEntry(int action, HWND anchor) {
    if (!anchor || !IsWindow(anchor)) anchor = g_trayWindow;
    if (!anchor || !IsWindow(anchor)) anchor = GetShellWindow();

    // Primary: build the layout locally. The shell API on 24H2 is not reliable.
    const int localCount = ArrangeLocally(action, anchor);
    if (localCount > 0) {
        Wh_Log(L"[menu] %s: local arranged %d window(s)", ArrangeActionName(action),
               localCount);
        return true;
    }

    // Fallback: the API path, just in case the local enumeration is broken for
    // some exotic reason.
    const WORD apiCount = CallArrangeApi(action);
    Wh_Log(L"[menu] %s: local 0, user32 arranged %u window(s)",
           ArrangeActionName(action), (unsigned)apiCount);
    return apiCount > 0;
}

// ---------------------------------------------------------------------------
// Menu command dispatch
// ---------------------------------------------------------------------------

static UINT g_lastDispatchedId = 0;
static DWORD g_lastDispatchedTick = 0;

static bool HandleMenuCommandFromShell(UINT id, HWND owner, const wchar_t* how) {
    const int slot = ClassicCommandSlot(id);
    if (slot < 0) return false;

    const DWORD now = GetTickCount();
    if (id == g_lastDispatchedId && (now - g_lastDispatchedTick) < 1500) {
        Wh_Log(L"[menu] %s: id %#x already dispatched, skipping", how, id);
        return true;
    }
    g_lastDispatchedId = id;
    g_lastDispatchedTick = now;

    Wh_Log(L"[menu] %s: id %#x (%s), owner %p", how, id, ArrangeActionName(slot), owner);
    const bool ok = RunClassicEntry(slot, owner);
    Wh_Log(L"[menu] %s: id %#x -> %s", how, id, ok ? L"done" : L"failed");
    return true;
}

// ---------------------------------------------------------------------------
// Subclass on the taskbar window
// ---------------------------------------------------------------------------

static HWND g_subclassed[4] = {};
static int g_subclassedCount = 0;

static UINT g_pendingMenuId = 0;
static DWORD g_pendingMenuTick = 0;

static LRESULT CALLBACK TrayCommandSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                                LPARAM lParam, DWORD_PTR ref) {
    (void)ref;
    switch (msg) {
    case WM_COMMAND:
        if (HIWORD(wParam) == 0) {
            const UINT id = LOWORD(wParam);
            if (IsInterestingCommandId(id))
                Wh_Log(L"[menu] WM_COMMAND %#x on %p", id, hwnd);
            if (HandleMenuCommandFromShell(id, hwnd, L"WM_COMMAND"))
                g_pendingMenuId = 0;
        }
        break;

    case WM_MENUCOMMAND: {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_ID};
        const HMENU menu = (HMENU)lParam;
        if (menu && GetMenuItemInfoW(menu, (UINT)wParam, TRUE, &mii)) {
            Wh_Log(L"[menu] WM_MENUCOMMAND %#x on %p", mii.wID, hwnd);
            if (HandleMenuCommandFromShell(mii.wID, hwnd, L"WM_MENUCOMMAND"))
                g_pendingMenuId = 0;
        }
        break;
    }

    case WM_MENUSELECT: {
        const UINT flags = HIWORD(wParam);
        const UINT id = LOWORD(wParam);

        if (flags == 0xFFFF && lParam == 0) {
            if (g_pendingMenuId) {
                const DWORD now = GetTickCount();
                const bool escaped   = (GetAsyncKeyState(VK_ESCAPE)  & 0x8000) != 0;
                const bool rightDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
                const bool fresh     = (now - g_pendingMenuTick) < 3000;
                if (!escaped && !rightDown && fresh) {
                    Wh_Log(L"[menu] menu closed with pending %#x, running command",
                           g_pendingMenuId);
                    HandleMenuCommandFromShell(g_pendingMenuId, hwnd, L"WM_MENUSELECT-close");
                } else {
                    Wh_Log(L"[menu] menu closed, pending %#x dropped (esc=%d rmb=%d fresh=%d)",
                           g_pendingMenuId, escaped, rightDown, fresh);
                }
                g_pendingMenuId = 0;
            }
            break;
        }

        if ((flags & MF_POPUP) == 0 && id && id != 0xFFFF) {
            if (IsInterestingCommandId(id))
                Wh_Log(L"[menu] WM_MENUSELECT %#x on %p", id, hwnd);
            if (ClassicCommandSlot(id) >= 0) {
                g_pendingMenuId = id;
                g_pendingMenuTick = GetTickCount();
            }
        }
        break;
    }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void SubclassWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId()) return;

    int kept = 0;
    for (int i = 0; i < g_subclassedCount; i++) {
        if (!IsWindow(g_subclassed[i])) continue;
        if (g_subclassed[i] == hwnd) return;
        g_subclassed[kept++] = g_subclassed[i];
    }
    g_subclassedCount = kept;
    if (g_subclassedCount >= (int)_countof(g_subclassed)) return;

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TrayCommandSubclassProc, 0)) {
        Wh_Log(L"[menu] subclass failed on %p (%lu)", hwnd, GetLastError());
        return;
    }
    g_subclassed[g_subclassedCount++] = hwnd;
    Wh_Log(L"[menu] subclass ready on %p", hwnd);
}

static BOOL CALLBACK FindTaskbarWindows(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId()) return TRUE;
    wchar_t className[64] = {};
    if (!GetClassNameW(hwnd, className, _countof(className))) return TRUE;
    if (wcscmp(className, L"Shell_TrayWnd") == 0) {
        if (!g_trayWindow) g_trayWindow = hwnd;
        SubclassWindow(hwnd);
    } else if (wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        SubclassWindow(hwnd);
    }
    (void)lParam;
    return TRUE;
}

static void EnsureTraySubclass() { EnumWindows(FindTaskbarWindows, 0); }

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

static HMENU g_enhancedMenus[8] = {};
static int g_enhancedMenuCount = 0;

static void RememberEnhancedMenu(HMENU menu) {
    if (!menu) return;
    if (g_enhancedMenuCount == (int)_countof(g_enhancedMenus)) {
        for (int i = 1; i < g_enhancedMenuCount; i++)
            g_enhancedMenus[i - 1] = g_enhancedMenus[i];
        g_enhancedMenuCount--;
    }
    g_enhancedMenus[g_enhancedMenuCount++] = menu;
}

static bool IsEnhancedMenu(HMENU menu) {
    if (!menu) return false;
    for (int i = 0; i < g_enhancedMenuCount; i++)
        if (g_enhancedMenus[i] == menu) return true;
    if (GetMenuState(menu, IDM_ORIG_CASCADE, MF_BYCOMMAND) != (UINT)-1) return true;
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        HMENU sub = GetSubMenu(menu, i);
        if (sub && GetMenuState(sub, IDM_ORIG_CASCADE, MF_BYCOMMAND) != (UINT)-1) return true;
    }
    return false;
}

static void LogMenuStyle(HMENU menu, const wchar_t* what) {
    MENUINFO mi = {sizeof(mi), MIM_STYLE};
    if (GetMenuInfo(menu, &mi)) {
        Wh_Log(L"[menu] %s %p style %#x (%s)", what, menu, mi.dwStyle,
               (mi.dwStyle & MNS_NOTIFYBYPOS) ? L"MNS_NOTIFYBYPOS: WM_MENUCOMMAND"
                                              : L"items notify by WM_COMMAND");
    }
}

static const wchar_t* WindowClassName(HWND hwnd) {
    static wchar_t cls[64]; cls[0] = 0;
    if (hwnd) GetClassNameW(hwnd, cls, _countof(cls));
    return cls;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

template <typename Track>
static BOOL TrackEnhancedMenu(UINT uFlags, HWND hwnd, Track track) {
    Wh_Log(L"[menu] popup on %p (%s), flags %#x (%s)", hwnd, WindowClassName(hwnd), uFlags,
           (uFlags & TPM_RETURNCMD) ? L"TPM_RETURNCMD" : L"no TPM_RETURNCMD");
    SubclassWindow(hwnd);
    const BOOL result = track(uFlags);
    if (uFlags & TPM_RETURNCMD) {
        const UINT id = (UINT)result;
        Wh_Log(L"[menu] popup closed: id %#x", id);
        if (id) HandleMenuCommandFromShell(id, hwnd, L"TPM_RETURNCMD");
    }
    return result;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd,
                                  LPTPMPARAMS lptpm) {
    if (!IsEnhancedMenu(hMenu))
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hwnd, lptpm);
    LogMenuStyle(hMenu, L"tracked menu:");
    return TrackEnhancedMenu(uFlags, hwnd, [&](UINT flags) {
        return TrackPopupMenuEx_Original(hMenu, flags, x, y, hwnd, lptpm);
    });
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved,
                                HWND hwnd, const RECT* prcRect) {
    if (!IsEnhancedMenu(hMenu))
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hwnd, prcRect);
    LogMenuStyle(hMenu, L"tracked menu:");
    return TrackEnhancedMenu(uFlags, hwnd, [&](UINT flags) {
        return TrackPopupMenu_Original(hMenu, flags, x, y, nReserved, hwnd, prcRect);
    });
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && lpMsg->message == WM_COMMAND && HIWORD(lpMsg->wParam) == 0 && lpMsg->hwnd) {
        const UINT id = LOWORD(lpMsg->wParam);
        if (ClassicCommandSlot(id) >= 0) {
            wchar_t cls[64] = {};
            GetClassNameW(lpMsg->hwnd, cls, _countof(cls));
            if (!_wcsicmp(cls, L"Shell_TrayWnd") ||
                !_wcsicmp(cls, L"Shell_SecondaryTrayWnd")) {
                Wh_Log(L"[menu] DispatchMessageW WM_COMMAND %#x on %p", id, lpMsg->hwnd);
                HandleMenuCommandFromShell(id, lpMsg->hwnd, L"DispatchMessageW");
            }
        }
    }
    return DispatchMessageW_Original(lpMsg);
}

static HHOOK g_callWndProcHook = nullptr;
static DWORD g_trayThreadId = 0;

static LRESULT CALLBACK TrayCallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == WM_COMMAND && HIWORD(cwp->wParam) == 0) {
            const UINT id = LOWORD(cwp->wParam);
            if (ClassicCommandSlot(id) >= 0) {
                wchar_t cls[64] = {};
                GetClassNameW(cwp->hwnd, cls, _countof(cls));
                if (!_wcsicmp(cls, L"Shell_TrayWnd") ||
                    !_wcsicmp(cls, L"Shell_SecondaryTrayWnd")) {
                    Wh_Log(L"[menu] CALLWNDPROC WM_COMMAND %#x on %p", id, cwp->hwnd);
                    HandleMenuCommandFromShell(id, cwp->hwnd, L"CALLWNDPROC");
                }
            }
        }
    }
    return CallNextHookEx(g_callWndProcHook, nCode, wParam, lParam);
}

static HWND FixArrangeParent(HWND hwndParent) {
    if (hwndParent && hwndParent == GetShellWindow()) return GetDesktopWindow();
    return hwndParent;
}

using CascadeWindows_t = decltype(&CascadeWindows);
CascadeWindows_t CascadeWindows_Original;
using TileWindows_t = decltype(&TileWindows);
TileWindows_t TileWindows_Original;

WORD WINAPI CascadeWindows_Hook(HWND hwndParent, UINT wHow, const RECT* lpRect, UINT cKids,
                                const HWND* lpKids) {
    return CascadeWindows_Original(FixArrangeParent(hwndParent), wHow, lpRect, cKids, lpKids);
}

WORD WINAPI TileWindows_Hook(HWND hwndParent, UINT wHow, const RECT* lpRect, UINT cKids,
                             const HWND* lpKids) {
    return TileWindows_Original(FixArrangeParent(hwndParent), wHow, lpRect, cKids, lpKids);
}

static void EnhanceTaskbarMenu(HMENU hMenu) {
    HMENU hPopup = GetSubMenu(hMenu, 0);
    if (!hPopup) hPopup = hMenu;

    LogMenuStyle(hMenu, L"menu 205:");
    LogMenuStyle(hPopup, L"menu 205 popup:");

    for (int i = GetMenuItemCount(hPopup) - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU};
        if (GetMenuItemInfoW(hPopup, i, TRUE, &mii) &&
            !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu)
            DeleteMenu(hPopup, i, MF_BYPOSITION);
    }

    AppendClassicEntry(hPopup, ARRANGE_CASCADE, HSTR_CASCADE);
    AppendClassicEntry(hPopup, ARRANGE_STACKED, HSTR_STACKED);
    AppendClassicEntry(hPopup, ARRANGE_SIDEBYSIDE, HSTR_SIDEBYSIDE);

    if (hPopup != hMenu) RememberEnhancedMenu(hPopup);
    RememberEnhancedMenu(hMenu);

    wchar_t buf[256]; const wchar_t* source = nullptr;
    wchar_t* str = LoadLabel(L"shell32.dll", g_shell32Module, IDS_SHOWDESKTOP, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Show the desktop\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP, str ? str : GetHardcodedText(HSTR_SHOWDESKTOP));
    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    source = nullptr;
    str = LoadLabel(L"shell32.dll", g_shell32Module, IDS_TASKMANAGER, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Task Manager\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_TASKMANAGER, str ? str : GetHardcodedText(HSTR_TASKMANAGER));
    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    wchar_t lockBuf[256];
    str = GetLockToolbarsText(lockBuf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR, str ? str : GetHardcodedText(HSTR_LOCKTOOLBARS));

    source = nullptr;
    str = LoadLabel(L"bthprops.cpl", g_bthpropsModule, IDS_SETTINGS, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Taskbar settings\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS, str ? str : GetHardcodedText(HSTR_SETTINGS));
}

typedef struct _WH_OSVERSIONINFOW {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId;
    WCHAR szCSDVersion[128];
} WH_OSVERSIONINFOW;

static DWORD GetExplorerBuildNumber() {
    using RtlGetVersion_t = LONG(WINAPI*)(WH_OSVERSIONINFOW*);
    static RtlGetVersion_t rtlGetVersion =
        (RtlGetVersion_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
    WH_OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (rtlGetVersion && rtlGetVersion(&vi) == 0) return vi.dwBuildNumber;
    return 0;
}

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == g_explorerModule) {
        UINT menuId = (UINT)(ULONG_PTR)lpMenuName;
        if ((menuId == 205 || menuId == 206) && g_shell32Module) {
            EnsureTraySubclass();
            HMENU result = LoadMenuW_Original(g_shell32Module, MAKEINTRESOURCEW(205));
            if (result) { EnhanceTaskbarMenu(result); return result; }
        }
    }
    return LoadMenuW_Original(hInstance, lpMenuName);
}

BOOL Wh_ModInit() {
    DetectUserLanguage();
    DetectLocaleNames();
    Wh_Log(L"[menu] mod 1.7.0 init, explorer build %lu", GetExplorerBuildNumber());

    g_explorerModule = GetModuleHandleW(nullptr);
    g_shell32Module = GetModuleHandleW(L"shell32.dll");
    g_bthpropsModule = LoadLibraryExW(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_explorerframeModule = GetModuleHandleW(L"explorerframe.dll");

    Wh_Log(L"[menu] %s: LoadMenuW",
           Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook, (void**)&LoadMenuW_Original)
               ? L"hook installed" : L"HOOK FAILED");
    Wh_Log(L"[menu] %s: TrackPopupMenuEx",
           Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook,
                              (void**)&TrackPopupMenuEx_Original)
               ? L"hook installed" : L"HOOK FAILED");
    Wh_Log(L"[menu] %s: TrackPopupMenu",
           Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook,
                              (void**)&TrackPopupMenu_Original)
               ? L"hook installed" : L"HOOK FAILED");
    Wh_Log(L"[menu] %s: CascadeWindows",
           Wh_SetFunctionHook((void*)CascadeWindows, (void*)CascadeWindows_Hook,
                              (void**)&CascadeWindows_Original)
               ? L"hook installed" : L"HOOK FAILED");
    Wh_Log(L"[menu] %s: TileWindows",
           Wh_SetFunctionHook((void*)TileWindows, (void*)TileWindows_Hook,
                              (void**)&TileWindows_Original)
               ? L"hook installed" : L"HOOK FAILED");
    Wh_Log(L"[menu] %s: DispatchMessageW",
           Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook,
                              (void**)&DispatchMessageW_Original)
               ? L"hook installed" : L"HOOK FAILED");

    EnsureTraySubclass();
    if (g_trayWindow) {
        g_trayThreadId = GetWindowThreadProcessId(g_trayWindow, nullptr);
        if (g_trayThreadId) {
            g_callWndProcHook = SetWindowsHookExW(WH_CALLWNDPROC, TrayCallWndProcHook,
                                                  nullptr, g_trayThreadId);
            Wh_Log(L"[menu] %s: WH_CALLWNDPROC on thread %lu",
                   g_callWndProcHook ? L"hook installed" : L"HOOK FAILED", g_trayThreadId);
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_callWndProcHook) {
        UnhookWindowsHookEx(g_callWndProcHook);
        g_callWndProcHook = nullptr;
    }
    for (int i = 0; i < g_subclassedCount; i++) {
        if (IsWindow(g_subclassed[i]))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_subclassed[i],
                                                             TrayCommandSubclassProc);
    }
    g_subclassedCount = 0;
    g_trayWindow = nullptr;
    for (int i = 0; i < g_dataFileCount; i++)
        if (g_dataFiles[i].module) FreeLibrary(g_dataFiles[i].module);
    if (g_bthpropsModule) FreeLibrary(g_bthpropsModule);
}
