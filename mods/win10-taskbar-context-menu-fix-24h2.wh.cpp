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

## Menu labels (1.3)

Labels are read from the loaded Windows modules where available; otherwise the mod
uses translations for 15 languages and falls back to English for other languages.
No files from other mods are required.

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
#define IDM_ORIG_STACKED     0x194
#define IDM_ORIG_SIDEBYSIDE  0x195

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
// Label loading from live modules
// ---------------------------------------------------------------------------

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

static wchar_t* LoadStr(HMODULE hMod, UINT id, wchar_t* buf, int size) {
    if (hMod && LoadStringW(hMod, id, buf, size) > 0) return buf;
    return nullptr;
}

static wchar_t* LoadLabel(HMODULE liveModule, UINT id, wchar_t* buf, int size,
                          const wchar_t** source) {
    if (LoadStr(liveModule, id, buf, size)) {
        if (source) *source = L"the module of this process";
        return buf;
    }
    if (source) *source = L"the hardcoded text of this mod";
    return nullptr;
}

static wchar_t* GetLockToolbarsText(wchar_t* buf, int size) {
    if (g_explorerframeModule) {
        HMENU hMenu = LoadMenuW_Original(g_explorerframeModule, MAKEINTRESOURCEW(264));
        if (hMenu) {
            for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
                HMENU popup = GetSubMenu(hMenu, i);
                if (!popup) continue;
                for (int j = 0; j < GetMenuItemCount(popup); j++) {
                    MENUITEMINFOW mii = {};
                    mii.cbSize = sizeof(mii);
                    mii.fMask = MIIM_ID | MIIM_STRING;
                    wchar_t text[256] = {};
                    mii.dwTypeData = text;
                    mii.cch = _countof(text) - 1;
                    if (GetMenuItemInfoW(popup, j, TRUE, &mii) &&
                        mii.wID == IDM_LOCKTOOLBARS && text[0]) {
                        wcsncpy_s(buf, size, text, _TRUNCATE);
                        DestroyMenu(hMenu);
                        return buf;
                    }
                }
            }
            DestroyMenu(hMenu);
        }
    }
    Wh_Log(L"[labels] \"Lock the toolbars\": the hardcoded text of this mod is used");
    return nullptr;
}

static wchar_t* ClassicEntryLabel(UINT originalId, wchar_t* buf, int size,
                                  const wchar_t** source) {
    // Windows 11 may not have these classic menu items; use the fallback then.
    if (g_shell32Module) {
        HMENU menu = LoadMenuW_Original(g_shell32Module, MAKEINTRESOURCEW(205));
        if (menu) {
            bool found = false;
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
                    if (GetMenuItemInfoW(popup, j, TRUE, &mii) &&
                        mii.wID == originalId && text[0]) {
                        wcsncpy_s(buf, size, text, _TRUNCATE);
                        found = true;
                        break;
                    }
                }
            }
            DestroyMenu(menu);
            if (found) {
                if (source) *source = L"the module of this process";
                return buf;
            }
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
            return TileWindows(nullptr, MDITILE_HORIZONTAL, nullptr, 0, nullptr) != 0;
        case IDM_SIDEBYSIDE:
            return TileWindows(nullptr, MDITILE_VERTICAL, nullptr, 0, nullptr) != 0;
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

static BOOL CALLBACK FindCurrentProcessTaskbarWnd(HWND hwnd, LPARAM result) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId()) return TRUE;
    wchar_t className[64] = {};
    if (GetClassNameW(hwnd, className, _countof(className)) &&
        wcscmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(result) = hwnd;
        return FALSE;
    }
    return TRUE;
}

static void EnsureTraySubclass() {
    HWND tray = nullptr;
    EnumWindows(FindCurrentProcessTaskbarWnd, reinterpret_cast<LPARAM>(&tray));
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
    wchar_t* str = LoadLabel(g_shell32Module, IDS_SHOWDESKTOP, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Show the desktop\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SHOWDESKTOP,
                str ? str : GetHardcodedText(HSTR_SHOWDESKTOP));

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    source = nullptr;
    str = LoadLabel(g_shell32Module, IDS_TASKMANAGER, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Task Manager\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_TASKMANAGER,
                str ? str : GetHardcodedText(HSTR_TASKMANAGER));

    AppendMenuW(hPopup, MF_SEPARATOR, 0, nullptr);

    wchar_t lockBuf[256];
    str = GetLockToolbarsText(lockBuf, 256);
    AppendMenuW(hPopup, MF_STRING, IDM_LOCKTASKBAR,
                str ? str : GetHardcodedText(HSTR_LOCKTOOLBARS));

    source = nullptr;
    str = LoadLabel(g_bthpropsModule, IDS_SETTINGS, buf, 256, &source);
    if (source) Wh_Log(L"[labels] \"Taskbar settings\": %s", source);
    AppendMenuW(hPopup, MF_STRING, IDM_SETTINGS,
                str ? str : GetHardcodedText(HSTR_SETTINGS));
}

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == g_explorerModule) {
        UINT menuId = (UINT)(ULONG_PTR)lpMenuName;
        if ((menuId == 205 || menuId == 206) && g_shell32Module) {
            EnsureTraySubclass();
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

    g_explorerModule = GetModuleHandleW(nullptr);
    g_shell32Module = GetModuleHandleW(L"shell32.dll");
    g_bthpropsModule = LoadLibraryW(L"bthprops.cpl");
    g_explorerframeModule = GetModuleHandleW(L"explorerframe.dll");

    if (!Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                            (void**)&LoadMenuW_Original)) {
        Wh_Log(L"[menu] the LoadMenuW hook could not be installed");
        return TRUE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_trayWindow && IsWindow(g_trayWindow))
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_trayWindow, TrayCommandSubclassProc);
    g_trayWindow = nullptr;

    if (g_bthpropsModule) FreeLibrary(g_bthpropsModule);
}
