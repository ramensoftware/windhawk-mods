// ==WindhawkMod==
// @id              win10-taskbar-context-menu-fix-24h2
// @name            Windows 10 Taskbar Context Menu Fix for Win11 24H2+
// @description     Fixes context menu on Windows 10 taskbar running on Windows 11 24H2, 25H2 and later
// @version         1.3
// @author          Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
The taskbar context menu (when clicking on the taskbar empty area) does not appear when
running the Windows 10 taskbar on Windows 11 24H2 or Windows 11 25H2.
This mod restores the menu.

## Where the labels come from (1.3)

The menu entries are the Windows 10 ones, and their labels are taken, in order, from:

1. `%ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore\<language>\<module>.mui` -
   the language folder of the Windows 10 modules downloaded by the Windows 10 taskbar
   mod (share the same folder: the labels are then the genuine Windows 10 ones);
2. `%ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore\<module>` - the file itself,
   for the strings that live in the module and not in its `.mui`;
3. the module loaded in this process (`shell32.dll`, `bthprops.cpl`,
   `explorerframe.dll`);
4. the text in the language detected at runtime (`GetUserDefaultUILanguage`), hardcoded
   in this file for 15 languages: English, Italian, German, French, Spanish, Portuguese,
   Russian, Simplified Chinese, Traditional Chinese, Japanese, Korean, Dutch, Polish,
   Turkish, Czech. English is the fallback for any other language.


The menu prior to this update had four entries; the Windows 10 taskbar menu has three more:

    Cascade windows | Show windows stacked | Show windows side by side

They are added above "Show the desktop". Explorer of Windows 11 does not know their
Windows 10 ids, so the mod answers `WM_COMMAND` for them on the taskbar window and runs
`CascadeWindows`/`TileWindows` itself.


*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>

static HMODULE g_explorerModule;
static HMODULE g_shell32Module;
static HMODULE g_bthpropsModule;
static HMODULE g_explorerframeModule;

#define IDM_SHOWDESKTOP     0x197
#define IDM_TASKMANAGER     0x1A4
#define IDM_LOCKTASKBAR     0x1A8
#define IDM_SETTINGS        0x19D
#define IDM_LOCKTOOLBARS    41484

#define IDM_CASCADE          0x7C71
#define IDM_STACKED          0x7C72
#define IDM_SIDEBYSIDE       0x7C73

#define IDM_ORIG_CASCADE     0x193
#define IDM_ORIG_SIDEBYSIDE  0x194
#define IDM_ORIG_STACKED     0x195

#define IDS_SHOWDESKTOP     10113
#define IDS_TASKMANAGER     24743
#define IDS_SETTINGS        2128

// ---------------------------------------------------------------------------
// Language detection and hardcoded translations (1.3)
//
// The enum values are prefixed with UL_ because LANG_ENGLISH, LANG_ITALIAN, ...
// are already macros in winnt.h. Without the prefix the preprocessor replaces
// the enum names with their numeric values and the compiler fails.
// ---------------------------------------------------------------------------

enum UserLanguage {
    UL_UNKNOWN = 0,
    UL_ENGLISH,
    UL_ITALIAN,
    UL_GERMAN,
    UL_FRENCH,
    UL_SPANISH,
    UL_PORTUGUESE,
    UL_RUSSIAN,
    UL_CHINESE_SIMPLIFIED,
    UL_CHINESE_TRADITIONAL,
    UL_JAPANESE,
    UL_KOREAN,
    UL_DUTCH,
    UL_POLISH,
    UL_TURKISH,
    UL_CZECH,
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
    HSTR_SHOWDESKTOP = 0,
    HSTR_TASKMANAGER,
    HSTR_LOCKTOOLBARS,
    HSTR_SETTINGS,
    HSTR_CASCADE,
    HSTR_STACKED,
    HSTR_SIDEBYSIDE,
    HSTR_COUNT
};

static const wchar_t* g_hardcodedTranslations[HSTR_COUNT][UL_COUNT] = {
    // HSTR_SHOWDESKTOP
    {
        L"Show the desktop",              // UL_UNKNOWN
        L"Show the desktop",              // UL_ENGLISH
        L"Mostra desktop",                // UL_ITALIAN
        L"Desktop anzeigen",              // UL_GERMAN
        L"Afficher le bureau",            // UL_FRENCH
        L"Mostrar el escritorio",         // UL_SPANISH
        L"Mostrar a área de trabalho",    // UL_PORTUGUESE
        L"Показать рабочий стол",         // UL_RUSSIAN
        L"显示桌面",                       // UL_CHINESE_SIMPLIFIED
        L"顯示桌面",                       // UL_CHINESE_TRADITIONAL
        L"デスクトップの表示",              // UL_JAPANESE
        L"바탕 화면 표시",                  // UL_KOREAN
        L"Bureaublad weergeven",          // UL_DUTCH
        L"Pokaż pulpit",                  // UL_POLISH
        L"Masaüstünü göster",             // UL_TURKISH
        L"Zobrazit plochu"                // UL_CZECH
    },
    // HSTR_TASKMANAGER
    {
        L"Task Manager",
        L"Task Manager",
        L"Gestione attività",
        L"Task-Manager",
        L"Gestionnaire des tâches",
        L"Administrador de tareas",
        L"Gerenciador de Tarefas",
        L"Диспетчер задач",
        L"任务管理器",
        L"工作管理員",
        L"タスク マネージャー",
        L"작업 관리자",
        L"Taakbeheer",
        L"Menedżer zadań",
        L"Görev Yöneticisi",
        L"Správce úloh"
    },
    // HSTR_LOCKTOOLBARS
    {
        L"Lock the toolbars",
        L"Lock the toolbars",
        L"Blocca le barre degli strumenti",
        L"Symbolleisten fixieren",
        L"Verrouiller les barres d'outils",
        L"Bloquear las barras de herramientas",
        L"Bloquear as barras de ferramentas",
        L"Закрепить панели инструментов",
        L"锁定工具栏",
        L"鎖定工具列",
        L"ツールバーを固定する",
        L"도구 모음 잠금",
        L"Werkbalken vergrendelen",
        L"Zablokuj paski narzędzi",
        L"Araç çubuklarını kilitle",
        L"Uzamknout panely nástrojů"
    },
    // HSTR_SETTINGS
    {
        L"Taskbar settings",
        L"Taskbar settings",
        L"Impostazioni della barra delle applicazioni",
        L"Taskleiste-Einstellungen",
        L"Paramètres de la barre des tâches",
        L"Configuración de la barra de tareas",
        L"Configurações da barra de tarefas",
        L"Параметры панели задач",
        L"任务栏设置",
        L"工作列設定",
        L"タスクバーの設定",
        L"작업 표시줄 설정",
        L"Taakbalkinstellingen",
        L"Ustawienia paska zadań",
        L"Görev çubuğu ayarları",
        L"Nastavení hlavního panelu"
    },
    // HSTR_CASCADE
    {
        L"Cascade windows",
        L"Cascade windows",
        L"Finestre a cascata",
        L"Fenster überlappend anordnen",
        L"Fenêtres en cascade",
        L"Ventanas en cascada",
        L"Janelas em cascata",
        L"Окна каскадом",
        L"层叠窗口",
        L"重疊顯示視窗",
        L"重ねて表示",
        L"창 계단식 배열",
        L"Vensters trapsgewijs schikken",
        L"Kaskadowe okna",
        L"Pencereleri basamakla",
        L"Kaskádově uspořádat okna"
    },
    // HSTR_STACKED
    {
        L"Show windows stacked",
        L"Show windows stacked",
        L"Finestre sovrapposte",
        L"Fenster gestapelt anzeigen",
        L"Afficher les fenêtres empilées",
        L"Mostrar ventanas apiladas",
        L"Mostrar janelas empilhadas",
        L"Отображать окна стопкой",
        L"堆叠显示窗口",
        L"堆疊顯示視窗",
        L"ウィンドウを重ねて表示",
        L"창 누적 배열",
        L"Vensters gestapeld weergeven",
        L"Pokaż okna kaskadowo",
        L"Pencereleri üst üste göster",
        L"Zobrazit okna skládaně"
    },
    // HSTR_SIDEBYSIDE
    {
        L"Show windows side by side",
        L"Show windows side by side",
        L"Finestre affiancate",
        L"Fenster nebeneinander anzeigen",
        L"Afficher les fenêtres côte à côte",
        L"Mostrar ventanas en paralelo",
        L"Mostrar janelas lado a lado",
        L"Отображать окна рядом",
        L"并排显示窗口",
        L"並排顯示視窗",
        L"ウィンドウを並べて表示",
        L"창 나란히 배열",
        L"Vensters naast elkaar weergeven",
        L"Pokaż okna obok siebie",
        L"Pencereleri yan yana göster",
        L"Zobrazit okna vedle sebe"
    }
};

static const wchar_t* GetHardcodedText(HardcodedStringId id) {
    if (id >= 0 && id < HSTR_COUNT && g_userLang >= 0 && g_userLang < UL_COUNT) {
        const wchar_t* text = g_hardcodedTranslations[id][g_userLang];
        if (text) return text;
    }
    return g_hardcodedTranslations[id][UL_ENGLISH];
}

// ---------------------------------------------------------------------------
// Existing label loading (store / live module)
// ---------------------------------------------------------------------------

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

static wchar_t* LoadStr(HMODULE hMod, UINT id, wchar_t* buf, int size) {
    if (hMod && LoadStringW(hMod, id, buf, size) > 0) return buf;
    return nullptr;
}

static const wchar_t kLegacyStore[] =
    L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore";

struct DataFileCacheEntry {
    wchar_t name[64];
    HMODULE module;
};

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
    for (int i = 0; i < g_dataFileCount; i++) {
        if (_wcsicmp(g_dataFiles[i].name, fileName) == 0) return g_dataFiles[i].module;
    }
    if (g_dataFileCount >= (int)(sizeof(g_dataFiles) / sizeof(g_dataFiles[0]))) return nullptr;

    wchar_t expanded[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(kLegacyStore, expanded, _countof(expanded)) == 0 ||
        !expanded[0])
        return nullptr;
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
            _snwprintf_s(path, _countof(path), _TRUNCATE, L"%s\\%s\\%s.mui", expanded, folder,
                         fileName);
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
    if (LoadStr(store, id, buf, size)) {
        if (source) *source = L"the Windows 10 store";
        return buf;
    }
    if (LoadStr(liveModule, id, buf, size)) {
        if (source) *source = L"the module of this process";
        return buf;
    }
    if (source) *source = L"the hardcoded text of this mod";
    return nullptr;
}

static wchar_t* GetLockToolbarsText(wchar_t* buf, int size) {
    const wchar_t* sources[2] = {L"explorerframe.dll", L"explorerframe.dll"};
    HMODULE modules[2] = {LoadStoreFile(L"explorerframe.dll"), g_explorerframeModule};
    for (int s = 0; s < 2; s++) {
        HMODULE module = modules[s];
        if (!module || (s == 1 && module == modules[0])) continue;
        HMENU hMenu = LoadMenuW_Original(module, MAKEINTRESOURCEW(264));
        if (!hMenu) continue;
        for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
            MENUITEMINFOW mii = {};
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_SUBMENU;
            if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii) || !mii.hSubMenu) continue;
            for (int j = 0; j < GetMenuItemCount(mii.hSubMenu); j++) {
                MENUITEMINFOW subMii = {};
                subMii.cbSize = sizeof(subMii);
                subMii.fMask = MIIM_ID | MIIM_STRING;
                wchar_t text[256] = {};
                subMii.dwTypeData = text;
                subMii.cch = 255;
                if (GetMenuItemInfoW(mii.hSubMenu, j, TRUE, &subMii) &&
                    subMii.wID == IDM_LOCKTOOLBARS) {
                    wcsncpy_s(buf, size, text, _TRUNCATE);
                    DestroyMenu(hMenu);
                    Wh_Log(L"[labels] \"Lock the toolbars\" read from %s (menu 264)",
                           sources[s]);
                    return buf;
                }
            }
        }
        DestroyMenu(hMenu);
    }
    Wh_Log(L"[labels] \"Lock the toolbars\": the hardcoded text of this mod is used");
    return nullptr;
}

static wchar_t* ClassicEntryLabel(UINT originalId, wchar_t* buf, int size,
                                  const wchar_t** source) {
    HMODULE modules[8] = {};
    int count = 0;
    int storeCount = 0;
    const wchar_t* storeFiles[] = {L"explorer.exe", L"explorer.exe.mui", L"shell32.dll",
                                   L"shell32.dll.mui"};
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
                MENUITEMINFOW mii = {};
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_ID | MIIM_STRING;
                wchar_t text[256] = {};
                mii.dwTypeData = text;
                mii.cch = _countof(text) - 1;
                if (GetMenuItemInfoW(popup, j, TRUE, &mii) && mii.wID == originalId &&
                    text[0]) {
                    wcsncpy_s(buf, size, text, _TRUNCATE);
                    found = buf;
                    break;
                }
            }
        }
        DestroyMenu(menu);
        if (found) {
            if (source)
                *source = (i < storeCount) ? L"the Windows 10 store"
                                          : L"the module of this process";
            return found;
        }
    }
    if (source) *source = L"the hardcoded text of this mod";
    return nullptr;
}

static void AppendClassicEntry(HMENU popup, UINT id, UINT originalId,
                               HardcodedStringId hstrId) {
    wchar_t buf[256] = {};
    const wchar_t* source = nullptr;
    wchar_t* label = ClassicEntryLabel(originalId, buf, _countof(buf), &source);
    static bool logged[4] = {};
    const int slot = (id == IDM_CASCADE) ? 0 : (id == IDM_STACKED) ? 1 : 2;
    if (!logged[slot]) {
        logged[slot] = true;
        Wh_Log(L"[labels] classic entry (Windows 10 id %#x): %s", originalId,
               source ? source : L"no source");
    }
    AppendMenuW(popup, MF_STRING, id, label ? label : GetHardcodedText(hstrId));
}

static bool RunClassicEntry(UINT id) {
    try {
        switch (id) {
        case IDM_CASCADE:
            return CascadeWindows(nullptr, 0, nullptr, 0, nullptr) != 0;
        case IDM_STACKED:
            return TileWindows(nullptr, MDITILE_VERTICAL, nullptr, 0, nullptr) != 0;
        case IDM_SIDEBYSIDE:
            return TileWindows(nullptr, MDITILE_HORIZONTAL, nullptr, 0, nullptr) != 0;
        default:
            return false;
        }
    } catch (...) {
        Wh_Log(L"[menu] exception while running a classic entry");
    }
    return false;
}

static LRESULT CALLBACK TrayCommandSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                               LPARAM lParam, DWORD_PTR ref) {
    (void)ref;
    (void)lParam;
    if (msg == WM_COMMAND) {
        const UINT id = LOWORD(wParam);
        if (id == IDM_CASCADE || id == IDM_STACKED || id == IDM_SIDEBYSIDE) {
            const bool ok = RunClassicEntry(id);
            Wh_Log(L"[menu] classic entry: %s", ok ? L"done" : L"failed");
            return 0;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static HWND g_trayWindow = nullptr;

static void EnsureTraySubclass() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindow(tray)) return;
    if (g_trayWindow == tray) return;
    if (g_trayWindow && IsWindow(g_trayWindow))
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_trayWindow, TrayCommandSubclassProc);
    g_trayWindow = nullptr;
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TrayCommandSubclassProc, 0)) {
        Wh_Log(L"[menu] the taskbar subclass could not be installed (%lu)", GetLastError());
        return;
    }
    g_trayWindow = tray;
    Wh_Log(L"[menu] taskbar subclass ready");
}

static HANDLE g_stopEvent = nullptr;
static HANDLE g_servicesThread = nullptr;

static DWORD WINAPI MenuCommandThread(LPVOID) {
    for (;;) {
        if (WaitForSingleObject(g_stopEvent, 2000) == WAIT_OBJECT_0) break;
        try {
            EnsureTraySubclass();
        } catch (...) {
            Wh_Log(L"[menu] exception while arming the taskbar subclass");
        }
    }
    return 0;
}

static void EnhanceTaskbarMenu(HMENU hMenu) {
    HMENU hPopup = GetSubMenu(hMenu, 0);
    if (!hPopup) hPopup = hMenu;

    for (int i = GetMenuItemCount(hPopup) - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {sizeof(mii), MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU};
        if (GetMenuItemInfoW(hPopup, i, TRUE, &mii) &&
            !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu) {
            DeleteMenu(hPopup, i, MF_BYPOSITION);
        }
    }

    AppendClassicEntry(hPopup, IDM_CASCADE, IDM_ORIG_CASCADE, HSTR_CASCADE);
    AppendClassicEntry(hPopup, IDM_STACKED, IDM_ORIG_STACKED, HSTR_STACKED);
    AppendClassicEntry(hPopup, IDM_SIDEBYSIDE, IDM_ORIG_SIDEBYSIDE, HSTR_SIDEBYSIDE);


    wchar_t buf[256];
    const wchar_t* source = nullptr;
    wchar_t* str = LoadLabel(L"shell32.dll", g_shell32Module, IDS_SHOWDESKTOP, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Show the desktop\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP,
                str ? str : GetHardcodedText(HSTR_SHOWDESKTOP));

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    source = nullptr;
    str = LoadLabel(L"shell32.dll", g_shell32Module, IDS_TASKMANAGER, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Task Manager\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_TASKMANAGER,
                str ? str : GetHardcodedText(HSTR_TASKMANAGER));

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    wchar_t lockBuf[256];
    str = GetLockToolbarsText(lockBuf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR,
                str ? str : GetHardcodedText(HSTR_LOCKTOOLBARS));

    source = nullptr;
    str = LoadLabel(L"bthprops.cpl", g_bthpropsModule, IDS_SETTINGS, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Taskbar settings\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS,
                str ? str : GetHardcodedText(HSTR_SETTINGS));
}

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == g_explorerModule) {
        UINT menuId = (UINT)(ULONG_PTR)lpMenuName;
        if ((menuId == 205 || menuId == 206) && g_shell32Module) {
            HMENU result = LoadMenuW_Original(g_shell32Module, MAKEINTRESOURCEW(205));
            if (result) {
                EnhanceTaskbarMenu(result);
                return result;
            }
        }
    }
    return LoadMenuW_Original(hInstance, lpMenuName);
}

BOOL Wh_ModInit() {
    DetectUserLanguage();
    DetectLocaleNames();

    g_explorerModule = GetModuleHandleW(nullptr);
    g_shell32Module = GetModuleHandleW(L"shell32.dll");
    g_bthpropsModule = LoadLibraryW(L"bthprops.cpl");
    g_explorerframeModule = GetModuleHandleW(L"explorerframe.dll");

    if (!Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                            (void**)&LoadMenuW_Original)) {
        Wh_Log(L"[menu] the LoadMenuW hook could not be installed");
        return TRUE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"[menu] the stop event could not be created");
        return TRUE;
    }
    EnsureTraySubclass();
    g_servicesThread = CreateThread(nullptr, 0, MenuCommandThread, nullptr, 0, nullptr);
    if (!g_servicesThread)
        Wh_Log(L"[menu] the command thread could not be created (%lu)", GetLastError());

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_servicesThread) {
        WaitForSingleObject(g_servicesThread, 3000);
        CloseHandle(g_servicesThread);
        g_servicesThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_trayWindow && IsWindow(g_trayWindow))
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_trayWindow, TrayCommandSubclassProc);
    g_trayWindow = nullptr;

    for (int i = 0; i < g_dataFileCount; i++) {
        if (g_dataFiles[i].module) FreeLibrary(g_dataFiles[i].module);
    }
    if (g_bthpropsModule) FreeLibrary(g_bthpropsModule);
}
