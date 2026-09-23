// ==WindhawkMod==
// @id              true-windows-11-explorer
// @name            True Windows 11 Explorer
// @description     Completely replaces the Windows 11 File Explorer interface with one built from scratch that works instantly, unlike the stock one. Customizable.
// @description:ru-RU Мод полностью заменяет интерфейс проводника Windows 11 на созданный мною с нуля, который работает мгновенно, в отличие от стандартного. С возможностью кастомизации.
// @version         1.36
// @author          Yevhenii
// @github          https://github.com/Leshugan
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -lole32 -lshell32 -luuid -ld2d1 -ldwrite -ldwmapi -lmsimg32 -lshlwapi -luxtheme -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- tabs:
  - enabled: true
    $name: Show tabs
    $name:ru-RU: Показывать вкладки
    $description: Turn off to remove tabs (along with the “Open in new tab” menu item and middle-click opening); the top of the window becomes lower. Reopen open windows afterwards.
    $description:ru-RU: Выключите — вкладок не будет (и пункта «Открыть в новой вкладке», и открытия средней кнопкой), верх окна станет ниже. Открытые окна лучше закрыть и открыть заново.
  - width: 248
    $name: Tab width
    $name:ru-RU: Ширина вкладки
    $description: In points (Windows 11 uses 248). With dynamic width, this is the maximum width.
    $description:ru-RU: В точках (у Windows 11 — 248). При динамической ширине — наибольшая ширина.
  - dynamicWidth: false
    $name: Dynamic width
    $name:ru-RU: Динамическая ширина
    $description: Each tab is as wide as its title; the close button appears on hover.
    $description:ru-RU: Ширина каждой вкладки — по длине её названия, крестик появляется при наведении.
  - icons: true
    $name: Folder icon on tabs
    $name:ru-RU: Значок папки на вкладке
    $description: Turn off to make tabs narrower.
    $description:ru-RU: Выключите — вкладки станут уже.
  - closeButtons: all
    $name: Close button on tabs
    $name:ru-RU: Крестик на вкладках
    $description: Where to show the close button (tabs can also be closed with the middle mouse button).
    $description:ru-RU: Где показывать крестик закрытия (вкладку можно закрыть и средней кнопкой мыши).
    $options:
    - all: On all tabs (like Windows 11)
    - active: Only on the selected tab and on hover
    - none: Hidden
    $options:ru-RU:
    - all: На всех вкладках (как в Windows 11)
    - active: Только на выбранной и под указателем
    - none: Не показывать
  - menuItem: true
    $name: “Open in new tab” in the folder menu
    $name:ru-RU: Пункт «Открыть в новой вкладке» в меню папки
    $description: Turn off to remove the item from the right-click menu (middle-clicking a folder still opens it in a tab).
    $description:ru-RU: Выключите — пункта в меню правой кнопки мыши не будет (средняя кнопка мыши по-прежнему открывает папку во вкладке).
  - scroll: false
    $name: Scrollable tabs
    $name:ru-RU: Прокрутка вкладок
    $description: When tabs don't fit, show scroll arrows instead of shrinking them further (like Windows 11). The mouse wheel scrolls them too.
    $description:ru-RU: Если вкладки не помещаются — не сужать их дальше, а показывать стрелки прокрутки (как у Windows 11). Листаются и колёсиком мыши.
  - remember: false
    $name: Remember tabs
    $name:ru-RU: Запоминать вкладки
    $description: When a window closes, remember its tabs and reopen them in the next new window. Stored in Windhawk's own storage, not in the system.
    $description:ru-RU: При закрытии окна запоминать его вкладки и открывать их в следующем новом окне. Хранится в настройках самого Windhawk, не в системе.
  - newTabFolder: computer
    $name: New tab folder
    $name:ru-RU: Папка новой вкладки
    $description: What "+" and Ctrl+T open.
    $description:ru-RU: Что открывать по «+» и Ctrl+T.
    $options:
    - computer: This PC
    - downloads: Downloads
    - custom: Custom folder (path below)
    $options:ru-RU:
    - computer: Этот компьютер
    - downloads: Загрузки
    - custom: Своя папка (путь ниже)
  - newTabPath: ""
    $name: Custom folder path
    $name:ru-RU: Путь своей папки
    $description: Copy a folder path (for example, D:\Programs), paste it here and click Save.
    $description:ru-RU: Скопируйте путь папки (например, D:\Programs), вставьте сюда и нажмите «Сохранить».
  $name: Tabs
  $name:ru-RU: Вкладки
- address:
  - upButton: true
    $name: Up button
    $name:ru-RU: Кнопка «Вверх»
    $description: Turn off to hide the Up button (Alt+Up Arrow still works); the address bar becomes wider.
    $description:ru-RU: Выключите — кнопки «Вверх» не будет (Alt+Стрелка вверх по-прежнему работает), адресная строка станет шире.
  - navGap: 8
    $name: Gap between Back / Forward / Up / Refresh
    $name:ru-RU: Промежуток между кнопками «Назад / Вперёд / Вверх / Обновить»
    $description: In points (Windows 11 uses 16). The smaller the gap, the wider the address bar.
    $description:ru-RU: В точках (у Windows 11 — 16). Чем меньше, тем шире адресная строка.
  - height: "32"
    $name: Address bar and search height
    $name:ru-RU: Высота адресной строки и поиска
    $description: In points. The font adapts but stays readable. Can't be taller than the row height below allows.
    $description:ru-RU: В точках. Шрифт подстраивается, но не мельче читаемого. Не выше, чем позволяет высота строки ниже.
    $options:
    - "24": 24 (smallest)
    - "26": "26"
    - "28": "28"
    - "30": "30"
    - "32": 32 (like Windows 11)
    - "36": "36"
    - "40": "40"
    - "44": 44 (largest)
    $options:ru-RU:
    - "24": 24 (наименьшая)
    - "26": "26"
    - "28": "28"
    - "30": "30"
    - "32": 32 (как в Windows 11)
    - "36": "36"
    - "40": "40"
    - "44": 44 (наибольшая)
  - areaHeight: "49"
    $name: Address row height
    $name:ru-RU: Высота строки с адресной строкой
    $description: The whole row with Back / Forward / Up / Refresh, the address bar and search, in points.
    $description:ru-RU: Вся полоса с кнопками «Назад / Вперёд / Вверх / Обновить», адресной строкой и поиском, в точках.
    $options:
    - "36": 36 (smallest)
    - "40": "40"
    - "44": "44"
    - "49": 49 (like Windows 11)
    - "52": "52"
    - "56": "56"
    - "60": 60 (largest)
    $options:ru-RU:
    - "36": 36 (наименьшая)
    - "40": "40"
    - "44": "44"
    - "49": 49 (как в Windows 11)
    - "52": "52"
    - "56": "56"
    - "60": 60 (наибольшая)
  - search: true
    $name: Search box
    $name:ru-RU: Поле поиска
    $description: Turn off to remove search; the address bar becomes full width.
    $description:ru-RU: Выключите — поиска не будет, адресная строка станет во всю ширину.
  - everything: false
    $name: Instant search with Everything
    $name:ru-RU: Мгновенный поиск через Everything
    $description: Show results from Everything while typing in the search box (Everything must be running). Enter opens a search window with categories.
    $description:ru-RU: При вводе в поле поиска сразу показывать найденное программой Everything (она должна быть запущена). Enter — окно поиска с категориями.
  $name: Address bar and search
  $name:ru-RU: Адресная строка и поиск
- toolbar:
  - enabled: true
    $name: Show command bar
    $name:ru-RU: Показывать панель кнопок
    $description: Turn off to remove the New / Cut / Copy… bar; the file list starts right below the address bar.
    $description:ru-RU: Выключите — панели «Создать / Вырезать / Копировать…» не будет, список файлов начнётся сразу под адресной строкой.
  - height: "47"
    $name: Command bar height
    $name:ru-RU: Высота панели кнопок
    $description: The bar with New, Cut, Copy and other buttons, in points.
    $description:ru-RU: Полоса с кнопками «Создать», «Вырезать», «Копировать» и т. д., в точках.
    $options:
    - "36": 36 (smallest)
    - "38": "38"
    - "40": "40"
    - "42": "42"
    - "44": "44"
    - "47": 47 (like Windows 11)
    $options:ru-RU:
    - "36": 36 (наименьшая)
    - "38": "38"
    - "40": "40"
    - "42": "42"
    - "44": "44"
    - "47": 47 (как в Windows 11)
  $name: Command bar
  $name:ru-RU: Панель кнопок
- window:
  - animations: false
    $name: Smooth animations
    $name:ru-RU: Плавные анимации
    $description: Smooth hover highlights and smooth opening, closing and reordering of tabs.
    $description:ru-RU: Плавное появление подсветки при наведении, плавное открытие, закрытие и перестановка вкладок.
  - snapLayouts: true
    $name: Snap layouts on the Maximize button
    $name:ru-RU: Раскладки окна при наведении на «Развернуть»
    $description: The Windows snap layouts flyout (and group suggestions) when hovering the Maximize button.
    $description:ru-RU: Панель Windows с раскладками окна (и предложениями групп окон) при наведении на кнопку «Развернуть».
  $name: Window
  $name:ru-RU: Окно
- navpane:
  - noIndent: false
    $name: No left indent
    $name:ru-RU: Без отступа слева
    $description: Icons and names start at the left edge. Nested folders are also barely indented.
    $description:ru-RU: Значки и названия — вплотную к левому краю. Вложенные папки тоже почти без сдвига вправо.
  - hidePins: false
    $name: Hide pin icons
    $name:ru-RU: Скрыть булавки у закреплённых папок
    $description: Names also use the space where the pin was.
    $description:ru-RU: Названия папок при этом занимают и место, где была булавка.
  $name: Navigation pane
  $name:ru-RU: Область навигации
- debug:
  - info: false
    $name: Show debug info
    $name:ru-RU: Показывать отладку
    $description: Show what the mod sees in the window, in the tab row (click the text to copy it).
    $description:ru-RU: Показывать в полосе вкладок, что мод видит в окне (щелчок по надписи — скопировать).
  $name: Debugging
  $name:ru-RU: Отладка
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# True Windows 11 Explorer
Completely replaces the Windows 11 File Explorer interface with one built from scratch that works instantly, unlike the stock one. Customizable.

The stock Windows 11 top part of the window (tabs, address bar, command bar) is built with XAML: it loads visibly and slows down opening a window. The mod doesn't let Explorer load it, so a window opens instantly, like in Windows 10, and draws its own top part in the Windows 11 style instead:
- tabs (drag to reorder, drag out to a new window or into another window, pin, remember on close);
- window buttons, Back / Forward / Up / Refresh, the address bar and search (optionally instant search via Everything);
- the command bar with New, Cut, Copy, Paste, Rename, Delete, Undo / Redo, Sort, View and options, with Windows icons for the current theme;
- optional tweaks for the navigation pane (no left indent, no pin icons).

Developed and tested on Windows 11 26H2. How it works on other versions is unknown — testing is needed.

What the mod stores: remembered and pinned tabs — in Windhawk's own storage. The Compact view, Navigation pane and Status bar options in the command bar menu change the same Windows settings as Explorer's own menus. Everything else is kept in memory only; disabling the mod restores the stock Explorer.

License: MIT — forks and modifications are welcome.

---

## По-русски
Мод полностью заменяет интерфейс проводника Windows 11 на созданный мною с нуля, который работает мгновенно, в отличие от стандартного. С возможностью кастомизации.

Стандартный верх окна Windows 11 (вкладки, адресная строка, панель кнопок) сделан на XAML: он подгружается на глазах и тормозит открытие окна. Мод не даёт Проводнику его загружать — окно открывается мгновенно, как в Windows 10, — и рисует вместо него свой верх в стиле Windows 11:
- вкладки (перестановка, вытаскивание в новое окно или в другое окно, закрепление, запоминание при закрытии);
- кнопки окна, «Назад / Вперёд / Вверх / Обновить», адресная строка и поиск (по желанию — мгновенный через Everything);
- панель кнопок «Создать», «Вырезать», «Копировать», «Вставить», «Переименовать», «Удалить», «Отменить / Повторить», «Сортировать», «Вид» и параметры — со значками Windows под тему;
- по желанию — настройка области навигации (без отступа слева, без булавок).

Мод разрабатывался и проверялся на Windows 11 26H2. Как он работает на других версиях — неизвестно, нужны тесты.

Что мод сохраняет: запомненные и закреплённые вкладки — в настройках самого Windhawk. Пункты «Компактное представление», «Область навигации» и «Строка состояния» в меню панели кнопок меняют те же настройки Windows, что и собственные меню Проводника. Всё остальное — только в памяти; выключение мода возвращает стандартный Проводник.

Лицензия — MIT: форки и доработки разрешены.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shdeprecated.h>
#include <shlwapi.h>
#include <shldisp.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <cwchar>
#include <cstdarg>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cwctype>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <climits>
#include <type_traits>

// ---------------- общее ----------------
static SRWLOCK g_lock = SRWLOCK_INIT;
static HANDLE g_thread;
static HINSTANCE g_inst;
static const wchar_t* STRIP_CLASS = L"EIT_Strip";
static const wchar_t* TIP_CLASS = L"EIT_Tip";
static const wchar_t* PROP_OLD = L"EIT_OldProc";   // метка «окно Проводника под модом»
static const wchar_t* PROP_STRIP = L"EIT_Strip";
static const wchar_t* PROP_TOPBAR = L"EIT_TopBar";
static const wchar_t* TOPBAR_CLASS = L"EIT_TopBar";
static const wchar_t* PROP_CPANEL = L"EIT_CPanel";
static const wchar_t* PROP_INACTIVE = L"EIT_Inactive";   // окно не в фокусе (по сообщению Windows, как у встроенного верха)   // окно сейчас показывает панель управления — панель кнопок не нужна
static UINT g_msgCreate, g_msgDestroy, g_msgPlace, g_msgNav;
static const wchar_t* g_face = L"Segoe Fluent Icons";
static bool g_lastLight = true;


// ---------------- настройки мода (из Windhawk) ----------------
static bool g_ru = false;   // язык интерфейса Windows — русский (иначе надписи мода по-английски)
static inline const wchar_t* TR(const wchar_t* ru, const wchar_t* en) { return g_ru ? ru : en; }
static struct { int tabWidth = 248; bool dynamicTabs = false; int navGap = 8; bool snap = true; int closeBtn = 0; bool everything = false; bool tabScroll = false; bool remember = false; bool anim = false; bool search = true; int newTab = 0; bool debug = false; bool tabs = true; bool tabIcons = true; std::wstring newTabPath; int fieldH = 32; double navH = 49.333, stripH = 46.667; bool toolbar = true; bool tabMenu = true; bool navNoIndent = false, navNoPins = false; bool upButton = true; } g_cfg;   // closeBtn: 0 — все, 1 — выбранная/под указателем, 2 — нет
static void LoadSettings() {
    auto str = [](const wchar_t* k) { std::wstring r; if (const wchar_t* v = Wh_GetStringSetting(k)) { r = v; Wh_FreeStringSetting(v); } return r; };
    int w = Wh_GetIntSetting(L"tabs.width"); g_cfg.tabWidth = w >= 60 && w <= 800 ? w : 248;
    g_cfg.tabs = Wh_GetIntSetting(L"tabs.enabled") != 0;
    g_cfg.dynamicTabs = Wh_GetIntSetting(L"tabs.dynamicWidth") != 0;
    g_cfg.tabIcons = Wh_GetIntSetting(L"tabs.icons") != 0;
    g_cfg.tabMenu = Wh_GetIntSetting(L"tabs.menuItem") != 0;
    g_cfg.tabScroll = Wh_GetIntSetting(L"tabs.scroll") != 0;
    g_cfg.remember = Wh_GetIntSetting(L"tabs.remember") != 0;
    std::wstring cb = str(L"tabs.closeButtons");
    g_cfg.closeBtn = cb == L"active" ? 1 : cb == L"none" ? 2 : 0;
    std::wstring nt = str(L"tabs.newTabFolder");
    g_cfg.newTab = nt == L"downloads" ? 1 : nt == L"custom" ? 2 : 0;
    {   // путь своей папки: без кавычек и пробелов по краям, %ПЕРЕМЕННЫЕ% раскрываются
        std::wstring p = str(L"tabs.newTabPath");
        while (!p.empty() && (p.back() == L' ' || p.back() == L'"')) p.pop_back();
        while (!p.empty() && (p[0] == L' ' || p[0] == L'"')) p.erase(0, 1);
        wchar_t ex[2048] = {};
        if (!p.empty() && ExpandEnvironmentStringsW(p.c_str(), ex, 2048)) p = ex;
        g_cfg.newTabPath = p;
    }
    int g = Wh_GetIntSetting(L"address.navGap"); g_cfg.navGap = g >= 0 && g <= 64 ? g : 8;
    g_cfg.search = Wh_GetIntSetting(L"address.search") != 0;
    g_cfg.upButton = Wh_GetIntSetting(L"address.upButton") != 0;
    auto num = [&](const wchar_t* k, int def, int lo, int hi) { int v = _wtoi(str(k).c_str()); return v <= 0 ? def : v < lo ? lo : v > hi ? hi : v; };   // ниже наименьшего — наименьшее
    int nh = num(L"address.areaHeight", 49, 36, 60); g_cfg.navH = nh == 49 ? 49.333 : nh;
    int fh = num(L"address.height", 32, 24, 44);
    if (fh > g_cfg.navH - 4) fh = (int)(g_cfg.navH - 4);   // поле не выше своей строки
    g_cfg.fieldH = fh;
    g_cfg.toolbar = Wh_GetIntSetting(L"toolbar.enabled") != 0;
    int sh = num(L"toolbar.height", 47, 36, 47); g_cfg.stripH = sh == 47 ? 46.667 : sh;
    g_cfg.everything = Wh_GetIntSetting(L"address.everything") != 0;
    g_cfg.anim = Wh_GetIntSetting(L"window.animations") != 0;
    g_cfg.snap = Wh_GetIntSetting(L"window.snapLayouts") != 0;
    g_cfg.debug = Wh_GetIntSetting(L"debug.info") != 0;
    {
        g_cfg.navNoIndent = Wh_GetIntSetting(L"navpane.noIndent") != 0;
        g_cfg.navNoPins = Wh_GetIntSetting(L"navpane.hidePins") != 0;
    }
}

static int g_animBusy = 0;   // сколько верхов сейчас анимируются — остальным пока не нагружать поток
static float FieldFont() { float f = g_cfg.fieldH * 14.f / 32.f; return f < 12.f ? 12.f : f > 16.f ? 16.f : f; }   // шрифт полей: 14 при высоте 32, от 12 до 16
static volatile LONG g_frameThreads = 0;   // сколько потоков кадров сейчас работает
static volatile LONG g_frameStop = 0;      // мод выгружается — потокам кадров пора выйти
static DWORD WINAPI FrameThread(LPVOID p) {   // ждёт кадр монитора и просит окно нарисовать следующий; окно отвечает, идёт ли ещё анимация
    HWND h = (HWND)p;
    for (;;) {
        if (g_frameStop || !IsWindow(h)) break;
        DwmFlush();
        DWORD_PTR more = 0;
        if (!SendMessageTimeoutW(h, WM_APP + 9, 0, 0, SMTO_NORMAL, 200, &more) || !more) break;
    }
    InterlockedDecrement(&g_frameThreads);
    return 0;
}
static void StartFrames(HWND h) {
    InterlockedIncrement(&g_frameThreads);
    HANDLE t = CreateThread(nullptr, 0, FrameThread, h, 0, nullptr);
    if (t) CloseHandle(t); else InterlockedDecrement(&g_frameThreads);
}
static UINT DpiOf(HWND h) { UINT d = h ? GetDpiForWindow(h) : 0; return d ? d : 96; }   // масштаб экрана окна (96 — 100%)
static bool TabsOn(HWND top) { return g_cfg.tabs && !(top && GetPropW(top, PROP_CPANEL)); }   // вкладки есть — кроме панели управления и выключенных в настройках
static double NavShiftFor(HWND top) { return TabsOn(top) ? 0.0 : 16.0; }   // без вкладок верх ниже на 16 точек: остаётся узкая полоска с кнопками окна
static double NavC() {   // середина строки навигации (с вкладками): у Windows при 49⅓ — на ⅔ ниже середины (снизу черта); в узкой строке — точно посередине
    double line = g_cfg.toolbar ? 1.333 : 0.0;
    double k = (g_cfg.navH - 36) / (49.333 - 36); k = k < 0 ? 0 : k > 1 ? 1 : k;
    return 40.0 + (g_cfg.navH - line) / 2 + line / 2 * k;
}                               // середина строки навигации (с вкладками)
static double StripH() { return g_cfg.toolbar ? g_cfg.stripH : 0.0; }                 // высота панели кнопок (0 — выключена)
static double StripHFor(HWND top) { return top && GetPropW(top, PROP_CPANEL) ? 0.0 : StripH(); }   // в панели управления панели кнопок нет — и места под неё тоже
static double TopTotalFor(HWND top) { return 40.0 + g_cfg.navH + StripHFor(top) - NavShiftFor(top); }   // где начинается список файлов (у Windows 11 — 136)
static double TopTotalFor(HWND top);   // где начинается список файлов (у Windows 11 — 136)
                       // сдвиг содержимого панели кнопок по высоте   // без вкладок верх ниже на 16 точек: остаётся узкая полоска с кнопками окна
static double QpcSec() {   // точное время в секундах — для анимаций по времени, а не по кадрам
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER c; QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
}
static bool IsLight() {
    DWORD v = 1, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v != 0;
}

// ---------------- кнопки ----------------
enum Accent { AC_NONE, AC_BOTTOM, AC_TOPRIGHT, AC_RIGHT, AC_INNER };
enum Act { A_SEP, A_SPACER, A_NEW, A_PROPS, A_OPTIONS, A_VIEWMENU, A_UNDO, A_REDO, A_CUT, A_COPY, A_PASTE, A_RENAME, A_DELETE, A_SORT, A_VIEW };
struct Btn { Act act; wchar_t glyph; Accent ac; };
static const Btn g_btns[] = {
    {A_NEW, 0xECC8, AC_NONE},         // «Создать» — как у Windows 11: значок, надпись, стрелочка
    {A_SEP, 0, AC_NONE},
    {A_CUT, 0xE8C6, AC_BOTTOM}, {A_COPY, 0xE8C8, AC_TOPRIGHT}, {A_PASTE, 0xE77F, AC_INNER},
    {A_RENAME, 0xE8AC, AC_RIGHT}, {A_DELETE, 0xE74D, AC_NONE},
    {A_SEP, 0, AC_NONE},
    {A_UNDO, 0xE7A7, AC_NONE}, {A_REDO, 0xE7A6, AC_NONE},
    {A_SEP, 0, AC_NONE},
    {A_PROPS, 0xE946, AC_NONE},       // «Свойства» файла или папки — круг с «i»
    {A_SEP, 0, AC_NONE},
    {A_SORT, 0xE8CB, AC_NONE},        // сортировка: значок со стрелочкой, одна кнопка
    {A_VIEW, 0xE700, AC_NONE},        // вид: значок со стрелочкой, одна кнопка
    {A_SPACER, 0, AC_NONE},           // дальше — прижато к правому краю
    {A_VIEWMENU, 0xE70D, AC_NONE},    // стрелочка — меню представления (теперь слева от значка)
    {A_OPTIONS, 0xE713, AC_NONE},     // значок — открывает «Параметры папок»
};
static const int BTN_COUNT = sizeof(g_btns) / sizeof(g_btns[0]);

static bool InAccent(Accent ac, double x, double y) {
    switch (ac) {
        case AC_BOTTOM:   return y > 0.52;
        case AC_TOPRIGHT: return x > 0.37 && y < 0.63;
        case AC_RIGHT:    return x > 0.73;
        case AC_INNER:    return x > 0.44 && y > 0.40;
        default:          return false;
    }
}

static void PickFace() {
    HDC dc = CreateCompatibleDC(nullptr);
    HFONT f = CreateFontW(-16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe Fluent Icons");
    HGDIOBJ o = SelectObject(dc, f);
    wchar_t n[64] = {};
    GetTextFaceW(dc, 64, n);
    SelectObject(dc, o); DeleteObject(f); DeleteDC(dc);
    g_face = _wcsicmp(n, L"Segoe Fluent Icons") == 0 ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
}

// значок рисуется в памяти из системного шрифта значков Windows 11, без файлов
static std::vector<BYTE> RenderGlyphGdi(wchar_t g, Accent ac, int s, bool light, bool disabled) {
    COLORREF col = light ? RGB(72, 72, 72) : RGB(214, 214, 214);
    COLORREF acc = light ? RGB(0, 95, 184) : RGB(76, 176, 224);
    double amul = 1.0;
    if (disabled) { acc = col; amul = 0.36; }   // как в Windows 11: серый, без синего, полупрозрачный
    std::vector<BYTE> px((size_t)s * s * 4, 0);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return px; }
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, light ? 0xFF : 0x00, (size_t)s * s * 4);
    int em = ((int)(s * 0.8 + 2.0) / 4) * 4; if (em < 8) em = 8;
    HFONT f = CreateFontW(-em, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, g_face);
    HGDIOBJ of = SelectObject(dc, f);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, light ? RGB(0, 0, 0) : RGB(255, 255, 255));
    RECT r = {0, 0, s, s};
    DrawTextW(dc, &g, 1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);
    GdiFlush();
    BYTE* sp = (BYTE*)bits;
    for (int y = 0; y < s; ++y)
        for (int x = 0; x < s; ++x) {
            BYTE* p = sp + ((size_t)y * s + x) * 4;
            BYTE a;
            if (light) { BYTE m = p[0]; if (p[1] < m) m = p[1]; if (p[2] < m) m = p[2]; a = 255 - m; }
            else       { BYTE m = p[0]; if (p[1] > m) m = p[1]; if (p[2] > m) m = p[2]; a = m; }
            a = (BYTE)(a * amul + 0.5);
            COLORREF c = InAccent(ac, (x + 0.5) / s, (y + 0.5) / s) ? acc : col;
            BYTE* o = px.data() + ((size_t)y * s + x) * 4;
            o[0] = GetBValue(c); o[1] = GetGValue(c); o[2] = GetRValue(c); o[3] = a;
        }
    SelectObject(dc, of); DeleteObject(f);
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    return px;
}


// ---------- чёткие значки: рисуем тем же движком текста, что и сама Windows 11 (DirectWrite) ----------
static ID2D1Factory* g_d2d = nullptr;
static IDWriteFactory* g_dw = nullptr;
static SRWLOCK g_fxLock = SRWLOCK_INIT;

static bool InitD2D() {
    AcquireSRWLockExclusive(&g_fxLock);
    if (!g_d2d) D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), nullptr, (void**)&g_d2d);
    if (!g_dw) DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_dw);
    bool ok = g_d2d && g_dw;
    ReleaseSRWLockExclusive(&g_fxLock);
    return ok;
}

// s — размер квадрата значка; сам знак занимает 80% (у Windows 11: 16 из 20 точек)
static std::vector<BYTE> RenderGlyph(wchar_t g, Accent ac, int s, bool light, bool disabled) {
    if (!InitD2D()) return RenderGlyphGdi(g, ac, s, light, disabled);
    COLORREF col = light ? RGB(72, 72, 72) : RGB(214, 214, 214);
    COLORREF acc = light ? RGB(0, 95, 184) : RGB(76, 176, 224);
    double amul = 1.0;
    if (disabled) { acc = col; amul = 0.36; }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return RenderGlyphGdi(g, ac, s, light, disabled); }
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, 0, (size_t)s * s * 4);

    bool drawn = false;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    ID2D1DCRenderTarget* rt = nullptr;
    IDWriteTextFormat* fmt = nullptr;
    ID2D1SolidColorBrush* br = nullptr;
    if (SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && rt &&
        SUCCEEDED(g_dw->CreateTextFormat(g_face, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL, s * 0.8f, L"", &fmt)) && fmt) {
        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        RECT rc = {0, 0, s, s};
        if (SUCCEEDED(rt->BindDC(dc, &rc)) &&
            SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &br)) && br) {
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            rt->DrawText(&g, 1, fmt, D2D1::RectF(0.f, 0.f, (float)s, (float)s), br,
                         D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
            drawn = SUCCEEDED(rt->EndDraw());
        }
    }
    if (br) br->Release();
    if (fmt) fmt->Release();
    if (rt) rt->Release();

    std::vector<BYTE> px((size_t)s * s * 4, 0);
    if (drawn) {
        GdiFlush();
        BYTE* sp = (BYTE*)bits;
        for (int y = 0; y < s; ++y)
            for (int x = 0; x < s; ++x) {
                BYTE a = sp[((size_t)y * s + x) * 4 + 3];   // белый знак: прозрачность = покрытие
                a = (BYTE)(a * amul + 0.5);
                COLORREF c = InAccent(ac, (x + 0.5) / s, (y + 0.5) / s) ? acc : col;
                BYTE* o = px.data() + ((size_t)y * s + x) * 4;
                o[0] = GetBValue(c); o[1] = GetGValue(c); o[2] = GetRValue(c); o[3] = a;
            }
    }
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    return drawn ? px : RenderGlyphGdi(g, ac, s, light, disabled);
}

static HBITMAP MakeDib(const std::vector<BYTE>& px, int s, bool premultiply) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP b = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!b || !bits) return b;
    BYTE* d = (BYTE*)bits;
    for (size_t i = 0; i < px.size(); i += 4) {
        BYTE a = px[i + 3];
        if (premultiply) { d[i] = px[i] * a / 255; d[i + 1] = px[i + 1] * a / 255; d[i + 2] = px[i + 2] * a / 255; }
        else { d[i] = px[i]; d[i + 1] = px[i + 1]; d[i + 2] = px[i + 2]; }
        d[i + 3] = a;
    }
    return b;
}

static HICON IconFromPx(const std::vector<BYTE>& px, int s) {
    HBITMAP color = MakeDib(px, s, false);
    if (!color) return nullptr;
    HBITMAP mask = CreateBitmap(s, s, 1, 1, nullptr);
    ICONINFO ii = {}; ii.fIcon = TRUE; ii.hbmColor = color; ii.hbmMask = mask;
    HICON ic = CreateIconIndirect(&ii);
    DeleteObject(color); DeleteObject(mask);
    return ic;
}

static HICON MakeIcon(wchar_t g, Accent ac, int s, bool light, bool disabled) {
    std::vector<BYTE> px = RenderGlyph(g, ac, s, light, disabled);
    HBITMAP color = MakeDib(px, s, false);
    if (!color) return nullptr;
    HBITMAP mask = CreateBitmap(s, s, 1, 1, nullptr);
    ICONINFO ii = {}; ii.fIcon = TRUE; ii.hbmColor = color; ii.hbmMask = mask;
    HICON ic = CreateIconIndirect(&ii);
    DeleteObject(color); DeleteObject(mask);
    return ic;
}


// ================= настоящие значки Проводника Windows 11 =================
// Windows хранит значки своей панели готовыми картинками (SVG) в системной папке.
// Мод только ЧИТАЕТ их и рисует тем же способом, что и Windows. Ничего не записывает.
struct SvgMat { float a = 1, b = 0, c = 0, d = 1, e = 0, f = 0; };
static SvgMat Mul(const SvgMat& m, const SvgMat& n) {   // сначала m, потом n
    SvgMat r;
    r.a = m.a * n.a + m.b * n.c;  r.b = m.a * n.b + m.b * n.d;
    r.c = m.c * n.a + m.d * n.c;  r.d = m.c * n.b + m.d * n.d;
    r.e = m.e * n.a + m.f * n.c + n.e;  r.f = m.e * n.b + m.f * n.d + n.f;
    return r;
}

struct SvgShape {
    std::string d; bool fill; float r, g, b, a; bool evenodd; SvgMat m;
    bool stroke; float sr, sg, sb, sa, sw; int cap, join;   // обводка: цвет, толщина, концы, углы
};
struct SvgDoc { float vx = 0, vy = 0, vw = 16, vh = 16; std::vector<SvgShape> shapes; bool ok = false; };

static std::string AttrOf(const std::map<std::string, std::string>& at, const char* k) {
    auto it = at.find(k); return it != at.end() ? it->second : std::string();
}
static std::vector<float> Nums(const std::string& s) {
    std::vector<float> v; const char* p = s.c_str();
    while (*p) {
        if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+' || *p == '.') { char* e; float f = strtof(p, &e); if (e == p) { ++p; continue; } v.push_back(f); p = e; }
        else ++p;
    }
    return v;
}
static SvgMat ParseTransform(const std::string& t) {
    SvgMat m; size_t i = 0;
    while (i < t.size()) {
        size_t o = t.find('(', i); if (o == std::string::npos) break;
        size_t c = t.find(')', o); if (c == std::string::npos) break;
        std::string name = t.substr(i, o - i);
        name.erase(0, name.find_first_not_of(" ,\t\r\n"));
        while (!name.empty() && (name.back() == ' ')) name.pop_back();
        std::vector<float> n = Nums(t.substr(o + 1, c - o - 1));
        SvgMat x;
        if (name == "translate" && n.size() >= 1) { x.e = n[0]; x.f = n.size() > 1 ? n[1] : 0; }
        else if (name == "scale" && n.size() >= 1) { x.a = n[0]; x.d = n.size() > 1 ? n[1] : n[0]; }
        else if (name == "matrix" && n.size() >= 6) { x.a = n[0]; x.b = n[1]; x.c = n[2]; x.d = n[3]; x.e = n[4]; x.f = n[5]; }
        else if (name == "rotate" && n.size() >= 1) {
            float r = n[0] * 3.14159265f / 180.f, cs = cosf(r), sn = sinf(r);
            SvgMat rr; rr.a = cs; rr.b = sn; rr.c = -sn; rr.d = cs;
            if (n.size() >= 3) { SvgMat t1; t1.e = -n[1]; t1.f = -n[2]; SvgMat t2; t2.e = n[1]; t2.f = n[2]; x = Mul(Mul(t1, rr), t2); }
            else x = rr;
        }
        m = Mul(x, m);   // преобразования применяются справа налево
        i = c + 1;
    }
    return m;
}
static bool ParseColor(std::string s, float& r, float& g, float& b) {
    while (!s.empty() && isspace((unsigned char)s.front())) s.erase(0, 1);
    while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
    for (auto& ch : s) ch = (char)tolower((unsigned char)ch);
    if (s.empty() || s == "none" || s == "transparent") return false;
    if (s[0] == '#') {
        unsigned v = (unsigned)strtoul(s.c_str() + 1, nullptr, 16);
        if (s.size() == 4) { r = ((v >> 8) & 15) / 15.f; g = ((v >> 4) & 15) / 15.f; b = (v & 15) / 15.f; return true; }
        if (s.size() >= 7) { if (s.size() == 9) v >>= 8; r = ((v >> 16) & 255) / 255.f; g = ((v >> 8) & 255) / 255.f; b = (v & 255) / 255.f; return true; }
        return false;
    }
    if (s.rfind("rgb", 0) == 0) { auto n = Nums(s); if (n.size() >= 3) { r = n[0] / 255.f; g = n[1] / 255.f; b = n[2] / 255.f; return true; } }
    if (s == "white") { r = g = b = 1; return true; }
    if (s == "black") { r = g = b = 0; return true; }
    return false;
}

struct SvgStyle {
    std::string fill = "#000", stroke = "none"; float opacity = 1, fillOpacity = 1, strokeOpacity = 1, strokeWidth = 1;
    bool evenodd = false; int cap = 0, join = 0; SvgMat m;
};

static SvgDoc ParseSvg(const std::string& x) {
    SvgDoc doc;
    std::vector<SvgStyle> stack(1);
    size_t i = 0;
    while ((i = x.find('<', i)) != std::string::npos) {
        size_t e = x.find('>', i); if (e == std::string::npos) break;
        std::string tag = x.substr(i + 1, e - i - 1);
        i = e + 1;
        if (tag.empty() || tag[0] == '?' || tag[0] == '!') continue;
        if (tag[0] == '/') { if (tag.rfind("/g", 0) == 0 && stack.size() > 1) stack.pop_back(); continue; }
        bool selfClose = !tag.empty() && tag.back() == '/';
        if (selfClose) tag.pop_back();
        size_t sp = tag.find_first_of(" \t\r\n");
        std::string name = tag.substr(0, sp);
        std::map<std::string, std::string> at;
        if (sp != std::string::npos) {
            size_t p = sp;
            while (p < tag.size()) {
                size_t eq = tag.find('=', p); if (eq == std::string::npos) break;
                std::string k = tag.substr(p, eq - p);
                k.erase(0, k.find_first_not_of(" \t\r\n"));
                while (!k.empty() && isspace((unsigned char)k.back())) k.pop_back();
                size_t q = tag.find_first_of("\"'", eq); if (q == std::string::npos) break;
                size_t q2 = tag.find(tag[q], q + 1); if (q2 == std::string::npos) break;
                at[k] = tag.substr(q + 1, q2 - q - 1);
                p = q2 + 1;
            }
        }
        // style="fill:...;opacity:..."
        std::string st = AttrOf(at, "style");
        for (size_t a = 0; a < st.size();) {
            size_t sc = st.find(';', a); std::string kv = st.substr(a, sc == std::string::npos ? std::string::npos : sc - a);
            size_t c = kv.find(':');
            if (c != std::string::npos) {
                std::string k = kv.substr(0, c), v = kv.substr(c + 1);
                k.erase(0, k.find_first_not_of(" ")); while (!k.empty() && k.back() == ' ') k.pop_back();
                at[k] = v;
            }
            if (sc == std::string::npos) break; a = sc + 1;
        }

        SvgStyle cur = stack.back();
        if (at.count("fill")) cur.fill = at["fill"];
        if (at.count("stroke")) cur.stroke = at["stroke"];
        if (at.count("stroke-width")) cur.strokeWidth = strtof(at["stroke-width"].c_str(), nullptr);
        if (at.count("stroke-opacity")) cur.strokeOpacity = strtof(at["stroke-opacity"].c_str(), nullptr);
        if (at.count("stroke-linecap")) { auto v = at["stroke-linecap"]; cur.cap = v.find("round") != std::string::npos ? 1 : v.find("square") != std::string::npos ? 2 : 0; }
        if (at.count("stroke-linejoin")) { auto v = at["stroke-linejoin"]; cur.join = v.find("round") != std::string::npos ? 1 : v.find("bevel") != std::string::npos ? 2 : 0; }
        if (at.count("fill-rule")) cur.evenodd = at["fill-rule"].find("evenodd") != std::string::npos;
        if (at.count("fill-opacity")) cur.fillOpacity = strtof(at["fill-opacity"].c_str(), nullptr);
        if (at.count("opacity")) cur.opacity *= strtof(at["opacity"].c_str(), nullptr);
        if (at.count("transform")) cur.m = Mul(ParseTransform(at["transform"]), cur.m);

        if (name == "svg") {
            auto vb = Nums(AttrOf(at, "viewBox"));
            if (vb.size() == 4) { doc.vx = vb[0]; doc.vy = vb[1]; doc.vw = vb[2]; doc.vh = vb[3]; }
            else {
                float w = strtof(AttrOf(at, "width").c_str(), nullptr), h = strtof(AttrOf(at, "height").c_str(), nullptr);
                if (w > 0 && h > 0) { doc.vw = w; doc.vh = h; }
            }
            stack[0] = cur; stack[0].m = SvgMat();
            continue;
        }
        if (name == "g") { if (!selfClose) stack.push_back(cur); continue; }
        if (name == "defs" || name == "clipPath" || name == "mask") {   // служебное — пропускаем содержимое
            size_t close = x.find("</" + name, i); if (close != std::string::npos) i = close;
            continue;
        }

        std::string d;
        char buf[512];
        if (name == "path") d = AttrOf(at, "d");
        else if (name == "rect") {
            float X = strtof(AttrOf(at, "x").c_str(), nullptr), Y = strtof(AttrOf(at, "y").c_str(), nullptr);
            float W = strtof(AttrOf(at, "width").c_str(), nullptr), H = strtof(AttrOf(at, "height").c_str(), nullptr);
            float rx = strtof(AttrOf(at, "rx").c_str(), nullptr), ry = strtof(AttrOf(at, "ry").c_str(), nullptr);
            if (rx <= 0) rx = ry; if (ry <= 0) ry = rx;
            if (rx > W / 2) rx = W / 2; if (ry > H / 2) ry = H / 2;
            if (rx > 0)
                snprintf(buf, sizeof(buf), "M%g %gH%gA%g %g 0 0 1 %g %gV%gA%g %g 0 0 1 %g %gH%gA%g %g 0 0 1 %g %gV%gA%g %g 0 0 1 %g %gZ",
                         X + rx, Y, X + W - rx, rx, ry, X + W, Y + ry, Y + H - ry, rx, ry, X + W - rx, Y + H, X + rx, rx, ry, X, Y + H - ry, Y + ry, rx, ry, X + rx, Y);
            else snprintf(buf, sizeof(buf), "M%g %gH%gV%gH%gZ", X, Y, X + W, Y + H, X);
            d = buf;
        } else if (name == "circle" || name == "ellipse") {
            float cx = strtof(AttrOf(at, "cx").c_str(), nullptr), cy = strtof(AttrOf(at, "cy").c_str(), nullptr);
            float rx = name == "circle" ? strtof(AttrOf(at, "r").c_str(), nullptr) : strtof(AttrOf(at, "rx").c_str(), nullptr);
            float ry = name == "circle" ? rx : strtof(AttrOf(at, "ry").c_str(), nullptr);
            snprintf(buf, sizeof(buf), "M%g %gA%g %g 0 1 0 %g %gA%g %g 0 1 0 %g %gZ", cx - rx, cy, rx, ry, cx + rx, cy, rx, ry, cx - rx, cy);
            d = buf;
        } else if (name == "line") {
            snprintf(buf, sizeof(buf), "M%g %gL%g %g",
                     strtof(AttrOf(at, "x1").c_str(), nullptr), strtof(AttrOf(at, "y1").c_str(), nullptr),
                     strtof(AttrOf(at, "x2").c_str(), nullptr), strtof(AttrOf(at, "y2").c_str(), nullptr));
            d = buf;
        } else if (name == "polygon" || name == "polyline") {
            auto n = Nums(AttrOf(at, "points"));
            for (size_t k = 0; k + 1 < n.size(); k += 2) { snprintf(buf, sizeof(buf), "%c%g %g", k ? 'L' : 'M', n[k], n[k + 1]); d += buf; }
            if (name == "polygon") d += "Z";
        }
        if (d.empty()) continue;
        SvgShape sh = {};
        sh.d = d; sh.m = cur.m; sh.evenodd = cur.evenodd;
        sh.fill = ParseColor(cur.fill, sh.r, sh.g, sh.b); sh.a = cur.opacity * cur.fillOpacity;
        sh.stroke = cur.strokeWidth > 0 && ParseColor(cur.stroke, sh.sr, sh.sg, sh.sb);
        sh.sa = cur.opacity * cur.strokeOpacity; sh.sw = cur.strokeWidth; sh.cap = cur.cap; sh.join = cur.join;
        if (sh.fill || sh.stroke) doc.shapes.push_back(sh);
    }
    doc.ok = !doc.shapes.empty() && doc.vw > 0 && doc.vh > 0;
    return doc;
}

// разбор команд контура SVG и построение фигуры Direct2D
static void BuildPath(ID2D1GeometrySink* k, const std::string& d) {
    const char* p = d.c_str();
    auto skip = [&]() { while (*p && (isspace((unsigned char)*p) || *p == ',')) ++p; };
    auto num = [&](float& out) -> bool {
        skip();
        if (!*p) return false;
        char* e; out = strtof(p, &e);
        if (e == p) return false;
        p = e; return true;
    };
    auto flag = [&](int& out) -> bool { skip(); if (*p == '0' || *p == '1') { out = *p - '0'; ++p; return true; } return false; };
    char cmd = 0;
    float cx = 0, cy = 0, sx = 0, sy = 0, lcx = 0, lcy = 0;
    bool open = false; char prev = 0;
    auto ensure = [&]() { if (!open) { k->BeginFigure(D2D1::Point2F(cx, cy), D2D1_FIGURE_BEGIN_FILLED); open = true; sx = cx; sy = cy; } };
    while (true) {
        skip();
        if (!*p) break;
        if (isalpha((unsigned char)*p)) { cmd = *p++; }
        else if (!cmd) break;
        bool rel = islower((unsigned char)cmd);
        char C = (char)toupper((unsigned char)cmd);
        float a[7];
        if (C == 'Z') {
            if (open) { k->EndFigure(D2D1_FIGURE_END_CLOSED); open = false; }
            cx = sx; cy = sy; prev = 'Z'; cmd = 0; continue;
        }
        if (C == 'M') {
            if (!num(a[0]) || !num(a[1])) break;
            if (open) { k->EndFigure(D2D1_FIGURE_END_OPEN); open = false; }
            cx = rel ? cx + a[0] : a[0]; cy = rel ? cy + a[1] : a[1];
            k->BeginFigure(D2D1::Point2F(cx, cy), D2D1_FIGURE_BEGIN_FILLED); open = true; sx = cx; sy = cy;
            cmd = rel ? 'l' : 'L'; prev = 'M'; continue;
        }
        if (C == 'L') { if (!num(a[0]) || !num(a[1])) break; ensure(); cx = rel ? cx + a[0] : a[0]; cy = rel ? cy + a[1] : a[1]; k->AddLine(D2D1::Point2F(cx, cy)); }
        else if (C == 'H') { if (!num(a[0])) break; ensure(); cx = rel ? cx + a[0] : a[0]; k->AddLine(D2D1::Point2F(cx, cy)); }
        else if (C == 'V') { if (!num(a[0])) break; ensure(); cy = rel ? cy + a[0] : a[0]; k->AddLine(D2D1::Point2F(cx, cy)); }
        else if (C == 'C' || C == 'S') {
            float x1, y1;
            if (C == 'C') { if (!num(a[0]) || !num(a[1])) break; x1 = rel ? cx + a[0] : a[0]; y1 = rel ? cy + a[1] : a[1]; }
            else { bool refl = prev == 'C' || prev == 'S'; x1 = refl ? 2 * cx - lcx : cx; y1 = refl ? 2 * cy - lcy : cy; }
            if (!num(a[2]) || !num(a[3]) || !num(a[4]) || !num(a[5])) break;
            ensure();
            float x2 = rel ? cx + a[2] : a[2], y2 = rel ? cy + a[3] : a[3], x = rel ? cx + a[4] : a[4], y = rel ? cy + a[5] : a[5];
            k->AddBezier(D2D1::BezierSegment(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), D2D1::Point2F(x, y)));
            lcx = x2; lcy = y2; cx = x; cy = y;
        }
        else if (C == 'Q' || C == 'T') {
            float x1, y1;
            if (C == 'Q') { if (!num(a[0]) || !num(a[1])) break; x1 = rel ? cx + a[0] : a[0]; y1 = rel ? cy + a[1] : a[1]; }
            else { bool refl = prev == 'Q' || prev == 'T'; x1 = refl ? 2 * cx - lcx : cx; y1 = refl ? 2 * cy - lcy : cy; }
            if (!num(a[2]) || !num(a[3])) break;
            ensure();
            float x = rel ? cx + a[2] : a[2], y = rel ? cy + a[3] : a[3];
            k->AddQuadraticBezier(D2D1::QuadraticBezierSegment(D2D1::Point2F(x1, y1), D2D1::Point2F(x, y)));
            lcx = x1; lcy = y1; cx = x; cy = y;
        }
        else if (C == 'A') {
            int la, sw;
            if (!num(a[0]) || !num(a[1]) || !num(a[2]) || !flag(la) || !flag(sw) || !num(a[3]) || !num(a[4])) break;
            ensure();
            float x = rel ? cx + a[3] : a[3], y = rel ? cy + a[4] : a[4];
            if (a[0] == 0 || a[1] == 0) k->AddLine(D2D1::Point2F(x, y));
            else {
                D2D1_ARC_SEGMENT arc = {D2D1::Point2F(x, y), D2D1::SizeF(fabsf(a[0]), fabsf(a[1])), a[2],
                                        sw ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                                        la ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL};
                k->AddArc(arc);
            }
            cx = x; cy = y;
        }
        else break;
        prev = C;
    }
    if (open) k->EndFigure(D2D1_FIGURE_END_OPEN);
}

// нарисовать значок-картинку: glyph — размер самого знака, s — размер квадрата
static std::vector<BYTE> RenderSvg(const SvgDoc& doc, int s, float glyph, double amul, bool mirror = false) {
    std::vector<BYTE> px;
    if (!doc.ok || !InitD2D()) return px;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return px; }
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, 0, (size_t)s * s * 4);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    ID2D1DCRenderTarget* rt = nullptr;
    bool drawn = false;
    RECT rc = {0, 0, s, s};
    if (SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && rt && SUCCEEDED(rt->BindDC(dc, &rc))) {
        float sc = glyph / (doc.vw > doc.vh ? doc.vw : doc.vh);
        float off = floorf((s - glyph) / 2.f + 0.5f);   // целые пиксели — края чёткие
        SvgMat base; base.a = sc; base.d = sc; base.e = -doc.vx * sc + off; base.f = -doc.vy * sc + off;
        if (mirror) { base.a = -sc; base.e = (doc.vx + doc.vw) * sc + off; }   // зеркально по горизонтали
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        for (auto& sh : doc.shapes) {
            ID2D1PathGeometry* geo = nullptr; ID2D1GeometrySink* sink = nullptr;
            if (FAILED(g_d2d->CreatePathGeometry(&geo)) || !geo) continue;
            if (SUCCEEDED(geo->Open(&sink)) && sink) {
                sink->SetFillMode(sh.evenodd ? D2D1_FILL_MODE_ALTERNATE : D2D1_FILL_MODE_WINDING);
                BuildPath(sink, sh.d);
                sink->Close(); sink->Release();
                SvgMat m = Mul(sh.m, base);
                rt->SetTransform(D2D1::Matrix3x2F(m.a, m.b, m.c, m.d, m.e, m.f));
                ID2D1SolidColorBrush* br = nullptr;
                if (sh.fill && SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(sh.r, sh.g, sh.b, sh.a), &br)) && br) {
                    rt->FillGeometry(geo, br); br->Release(); br = nullptr;
                }
                if (sh.stroke && SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(sh.sr, sh.sg, sh.sb, sh.sa), &br)) && br) {
                    D2D1_CAP_STYLE cap = sh.cap == 1 ? D2D1_CAP_STYLE_ROUND : sh.cap == 2 ? D2D1_CAP_STYLE_SQUARE : D2D1_CAP_STYLE_FLAT;
                    D2D1_LINE_JOIN join = sh.join == 1 ? D2D1_LINE_JOIN_ROUND : sh.join == 2 ? D2D1_LINE_JOIN_BEVEL : D2D1_LINE_JOIN_MITER;
                    ID2D1StrokeStyle* ss = nullptr;
                    g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(cap, cap, cap, join), nullptr, 0, &ss);
                    rt->DrawGeometry(geo, br, sh.sw, ss);
                    if (ss) ss->Release();
                    br->Release();
                }
            }
            geo->Release();
        }
        drawn = SUCCEEDED(rt->EndDraw());
    }
    if (rt) rt->Release();
    if (drawn) {
        GdiFlush();
        px.resize((size_t)s * s * 4);
        BYTE* sp = (BYTE*)bits;
        for (size_t i = 0; i < px.size(); i += 4) {
            BYTE a = sp[i + 3];
            if (a) {   // из «умноженных» цветов обратно в обычные
                px[i] = (BYTE)(sp[i] * 255 / a); px[i + 1] = (BYTE)(sp[i + 1] * 255 / a); px[i + 2] = (BYTE)(sp[i + 2] * 255 / a);
            }
            px[i + 3] = (BYTE)(a * amul + 0.5);
        }
    }
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    return px;
}

// где лежат значки: разные сборки Windows 11 держат их в разных системных пакетах
static std::wstring g_svgDir[2];     // [0] тёмная тема, [1] светлая
static std::vector<std::wstring> g_svgFiles[2];
static bool g_svgScanned = false;

static void ScanSvgDirs() {
    if (g_svgScanned) return;
    g_svgScanned = true;
    wchar_t win[MAX_PATH]; GetWindowsDirectoryW(win, MAX_PATH);
    // все системные пакеты, где есть папка значков Проводника (номер пакета меняется от сборки к сборке); берём ту, где картинок больше
    std::vector<std::wstring> roots;
    for (const wchar_t* base : {L"\\SystemApps\\", L"\\SystemApps\\SxS\\"}) {
        std::wstring b = std::wstring(win) + base;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((b + L"*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fd.cFileName[0] != L'.')
                roots.push_back(b + fd.cFileName + L"\\FileExplorerExtensions\\Assets\\images\\contrast-standard\\");
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    for (int t = 0; t < 2; ++t) {
        size_t best = 0;
        for (auto& r : roots) {
            std::wstring dir = r + (t ? L"theme-light" : L"theme-dark");
            std::vector<std::wstring> files;
            WIN32_FIND_DATAW fd;
            HANDLE h = FindFirstFileW((dir + L"\\*.svg").c_str(), &fd);
            if (h == INVALID_HANDLE_VALUE) continue;
            do { files.push_back(fd.cFileName); } while (FindNextFileW(h, &fd));
            FindClose(h);
            if (files.size() > best) { best = files.size(); g_svgFiles[t] = files; g_svgDir[t] = dir; }
        }
        Wh_Log(L"icons folder (%s): %s, %zu files", t ? L"light" : L"dark", g_svgDir[t].c_str(), g_svgFiles[t].size());
    }
}

static std::string Lower(const std::wstring& w) {
    std::string s;
    for (wchar_t c : w) if (iswalnum(c)) s += (char)towlower(c);
    return s;
}

// подобрать файл по ключевым словам: точное совпадение имени важнее, короткое имя важнее
static std::wstring FindSvg(bool light, const std::vector<const char*>& keys, bool exactOnly = false) {
    ScanSvgDirs();
    int t = light ? 1 : 0;
    std::wstring best; int bestScore = 0;
    for (auto& f : g_svgFiles[t]) {
        std::wstring stem = f.substr(0, f.rfind(L'.'));
        std::string n = Lower(stem);
        for (auto k : keys) {
            int score = 0;
            if (n == k) score = 1000;
            else if (exactOnly) score = 0;
            else if (n.rfind(k, 0) == 0) score = 500 - (int)n.size();
            else if (n.find(k) != std::string::npos) score = 200 - (int)n.size();
            if (score > bestScore) { bestScore = score; best = g_svgDir[t] + L"\\" + f; }
        }
    }
    return best;
}

static std::map<std::wstring, SvgDoc> g_svgCache;
static const SvgDoc* LoadSvg(const std::wstring& path) {
    if (path.empty()) return nullptr;
    auto it = g_svgCache.find(path);
    if (it != g_svgCache.end()) return it->second.ok ? &it->second : nullptr;
    std::string data;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD sz = GetFileSize(h, nullptr), rd = 0;
        if (sz && sz < 1 << 20) { data.resize(sz); ReadFile(h, &data[0], sz, &rd, nullptr); data.resize(rd); }
        CloseHandle(h);
    }
    SvgDoc doc = ParseSvg(data);
    Wh_Log(L"svg %s: %zu shapes", path.c_str(), doc.shapes.size());
    auto& slot = g_svgCache[path] = doc;
    return slot.ok ? &slot : nullptr;
}

// точные имена картинок из папки Проводника (присланный архив): windows.cut.svg → «windowscut» и т. д.
static std::vector<const char*> KeysFor(Act a) {
    switch (a) {
        case A_CUT:     return {"windowscut", "cut"};
        case A_COPY:    return {"windowscopy", "copy"};
        case A_PASTE:   return {"windowspaste", "paste"};
        case A_RENAME:  return {"windowsrename", "rename"};
        case A_DELETE:  return {"windowsribbondelete", "delete"};
        case A_UNDO:    return {"windowsundo", "undo"};
        case A_REDO:    return {"windowsredo", "redo"};
        case A_OPTIONS: return {"displaysettings", "settings"};
        case A_SORT:    return {"sortby", "sort"};
        case A_PROPS:   return {"windowsproperties", "properties"};
        case A_NEW:     return {"windowsnewitem"};
        case A_VIEW:    return {"view"};
        default:        return {};
    }
}


// серый цвет значков Windows — берём у её собственного значка корзины, чтобы совпадал
static COLORREF StockGray(bool light) {   // замер со стоковых скринов: тёмная тема 224, светлая 85
    return light ? RGB(85, 85, 85) : RGB(224, 223, 223);
}

// «Создать»: серый круг и синий плюс, линии в 1 точку — как у Windows 11
static std::vector<BYTE> RenderNewIcon(int s, bool light, bool disabled) {
    std::vector<BYTE> px;
    if (!InitD2D()) return px;
    COLORREF gray = StockGray(light);
    COLORREF blue = light ? RGB(0, 120, 212) : RGB(76, 194, 255);
    double amul = disabled ? 0.36 : 1.0;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return px; }
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, 0, (size_t)s * s * 4);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    ID2D1DCRenderTarget* rt = nullptr;
    RECT rc = {0, 0, s, s};
    bool ok = false;
    if (SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && rt && SUCCEEDED(rt->BindDC(dc, &rc))) {
        float u = s / 20.f;                 // 1 точка при размере квадрата 20
        // центр ставим так, чтобы линии плюса ложились на целые пиксели (у Windows плюс чёткий, не размытый)
        // по замеру стоковой кнопки (150%): круг 23 пикселя, плюс 11 пикселей, всё на 1 пиксель левее прежнего
        float cy = floorf(s / 2.f) + 1.f - u / 2.f, cx = cy - 2.f * u / 3.f, rad = 7.17f * u;
        float c = cy;
        ID2D1SolidColorBrush *bg = nullptr, *bb = nullptr;
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(gray) / 255.f, GetGValue(gray) / 255.f, GetBValue(gray) / 255.f), &bg);
        rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(blue) / 255.f, GetGValue(blue) / 255.f, GetBValue(blue) / 255.f), &bb);
        if (bg && bb) {
            rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), bg, u);
            float h = 3.67f * u;
            rt->DrawLine(D2D1::Point2F(cx - h, cy), D2D1::Point2F(cx + h, cy), bb, u);
            rt->DrawLine(D2D1::Point2F(cx, cy - h), D2D1::Point2F(cx, cy + h), bb, u);
            (void)c;
        }
        if (bg) bg->Release();
        if (bb) bb->Release();
        ok = SUCCEEDED(rt->EndDraw());
    }
    if (rt) rt->Release();
    if (ok) {
        GdiFlush();
        px.resize((size_t)s * s * 4);
        BYTE* sp = (BYTE*)bits;
        for (size_t i = 0; i < px.size(); i += 4) {
            BYTE a = sp[i + 3];
            if (a) { px[i] = (BYTE)(sp[i] * 255 / a); px[i + 1] = (BYTE)(sp[i + 1] * 255 / a); px[i + 2] = (BYTE)(sp[i + 2] * 255 / a); }
            px[i + 3] = (BYTE)(a * amul + 0.5);
        }
    }
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    return px;
}


// «Сведения»: серый круг и синяя «i» — в том же стиле, что значок «Создать»
static std::vector<BYTE> RenderInfoIcon(int s, bool light, bool disabled) {
    std::vector<BYTE> px;
    if (!InitD2D()) return px;
    COLORREF gray = StockGray(light);
    COLORREF blue = light ? RGB(0, 120, 212) : RGB(76, 194, 255);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s; bi.bmiHeader.biHeight = -s;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return px; }
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, 0, (size_t)s * s * 4);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    ID2D1DCRenderTarget* rt = nullptr;
    RECT rc = {0, 0, s, s};
    bool ok = false;
    if (SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && rt && SUCCEEDED(rt->BindDC(dc, &rc))) {
        float u = s / 20.f;
        float c = floorf(s / 2.f) + 1.f - u / 2.f, rad = 7.6f * u;   // чуть крупнее, чем у «Создать»
        ID2D1SolidColorBrush *bg = nullptr, *bb = nullptr;
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(gray) / 255.f, GetGValue(gray) / 255.f, GetBValue(gray) / 255.f), &bg);
        rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(blue) / 255.f, GetGValue(blue) / 255.f, GetBValue(blue) / 255.f), &bb);
        if (bg && bb) {
            rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(c, c), rad, rad), bg, u);
            rt->DrawLine(D2D1::Point2F(c, c - 1.f * u), D2D1::Point2F(c, c + 4.f * u), bb, u);    // палочка «i»
            float dr = 0.85f * u;
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c, c - 3.5f * u), dr, dr), bb);           // точка «i»
        }
        if (bg) bg->Release();
        if (bb) bb->Release();
        ok = SUCCEEDED(rt->EndDraw());
    }
    if (rt) rt->Release();
    if (ok) {
        GdiFlush();
        double amul = disabled ? 0.36 : 1.0;
        px.resize((size_t)s * s * 4);
        BYTE* sp = (BYTE*)bits;
        for (size_t i = 0; i < px.size(); i += 4) {
            BYTE a = sp[i + 3];
            if (a) { px[i] = (BYTE)(sp[i] * 255 / a); px[i + 1] = (BYTE)(sp[i + 1] * 255 / a); px[i + 2] = (BYTE)(sp[i + 2] * 255 / a); }
            px[i + 3] = (BYTE)(a * amul + 0.5);
        }
    }
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    return px;
}

// у стрелок «Отменить/Повторить» синим остаётся только наконечник, линия — серая, как грани остальных значков

static std::map<std::tuple<int, int, bool, bool>, HICON> g_icons;   // (кнопка, размер, тема, неактивна) -> значок
static HICON GetIcon(int i, int s, bool light, bool disabled) {
    auto key = std::make_tuple(i, s, light, disabled);
    AcquireSRWLockExclusive(&g_lock);
    auto it = g_icons.find(key);
    HICON ic = it != g_icons.end() ? it->second : nullptr;
    if (!ic) {
        if (i != 100) {   // сначала — настоящий значок Windows из системной папки
            Act ba = g_btns[i].act;
            const SvgDoc* doc = LoadSvg(FindSvg(light, KeysFor(ba), ba == A_NEW));
            if (ba == A_PROPS) {   // «Свойства» — круг с «i» (по твоему выбору, вместо гаечного ключа Windows)
                std::vector<BYTE> px = RenderInfoIcon(s, light, disabled);
                if (!px.empty()) ic = IconFromPx(px, s);
                doc = nullptr;
            }
            if (!doc && ba == A_NEW) {
                std::vector<BYTE> px = RenderNewIcon(s, light, disabled);
                if (!px.empty()) ic = IconFromPx(px, s);
            }
            SvgDoc recol;
            if (doc && (ba == A_UNDO || ba == A_REDO) && doc->shapes.size() >= 2) {
                // у Windows стрелка из двух частей: наконечник и линия; линию делаем серой, как грани остальных значков
                recol = *doc;
                COLORREF g = StockGray(light);
                auto& ln = recol.shapes.back();
                ln.r = GetRValue(g) / 255.f; ln.g = GetGValue(g) / 255.f; ln.b = GetBValue(g) / 255.f;
                doc = &recol;
            }
            if (doc) {
                std::vector<BYTE> px = RenderSvg(*doc, s, s * 0.8f, disabled ? 0.36 : 1.0);
                if (!px.empty()) ic = IconFromPx(px, s);
            }
        }
        if (!ic) ic = i == 100 ? MakeIcon(0xE96E, AC_NONE, s, light, disabled)
                               : MakeIcon(g_btns[i].glyph, g_btns[i].ac, s, light, disabled);
        g_icons[key] = ic;
    }
    ReleaseSRWLockExclusive(&g_lock);
    return ic;
}
static void ClearIcons() {
    AcquireSRWLockExclusive(&g_lock);
    for (auto& kv : g_icons) if (kv.second) DestroyIcon(kv.second);
    g_icons.clear();
    ReleaseSRWLockExclusive(&g_lock);
}

// ---------------- где стоит встроенная панель команд ----------------
struct Place { int top = 0, h = 0, left = 0, rmargin = 0; bool classic = false; };

// быстрый вид (Windows 10): у окна есть строка «Назад/адрес/поиск» Windows 10, а нового верха нет
static bool IsClassicTop(HWND top) { return FindWindowExW(top, nullptr, L"WorkerW", nullptr) != nullptr; }
static HWND ShellTabOf(HWND top) { return FindWindowExW(top, nullptr, L"ShellTabWindowClass", nullptr); }
// без ленты Проводник показывает над списком файлов старую полосу кнопок Windows 7 («Упорядочить», «Новая папка»…).
// Её высоту мод замеряет (на сколько список файлов ниже верха своей вкладки) и поднимает вкладку на эту высоту —
// полоса уходит под наш верх и панель кнопок, а список начинается ровно там же, где и раньше
static const wchar_t* PROP_CMDH = L"EIT_CmdH";   // высота этой полосы в пикселях (+1)
static int CmdBarH(HWND top) { INT_PTR v = (INT_PTR)GetPropW(top, PROP_CMDH); return v > 0 ? (int)v - 1 : 0; }

static Place GetPlace(HWND top) {
    UINT dpi = DpiOf(top);
    if (IsClassicTop(top)) {   // наша панель — сразу над списком файлов, высотой как у Windows 11 (44⅔ + полоса 2)
        Place c; c.classic = true;
        HWND st = ShellTabOf(top);
        RECT r = {};
        if (st) { GetWindowRect(st, &r); MapWindowPoints(nullptr, top, (POINT*)&r, 2); }
        int h = (int)lround(StripHFor(top) * dpi / 96.0);
        c.top = (st ? r.top + CmdBarH(top) : (int)lround(TopTotalFor(top) * dpi / 96.0)) - h;   // над списком файлов (вкладка поднята на высоту старой полосы)
        c.h = h;
        c.left = c.rmargin = 0;   // во всю ширину — без полосок фона окна по краям
        return c;
    }
    Place p;   // обычный вид (не должен встречаться — новый верх Windows мы не загружаем): примерное место
    p.top = MulDiv(89, dpi, 96); p.h = MulDiv(48, dpi, 96);
    return p;
}

// ---------------- рисование строки ----------------
// подписка на сообщения Windows: «папка открылась», «выделение изменилось». Windows сама сообщает о событии — опрашивать не нужно
static const GUID DIID_WBEvents2 = {0x34A715A0, 0x6587, 0x11D0, {0x92, 0x4A, 0x00, 0x20, 0xAF, 0xC7, 0xAC, 0x4D}};   // события окна Проводника
static const GUID DIID_SFVEvents = {0x62112AA2, 0xEBE4, 0x11CF, {0xA5, 0xFB, 0x00, 0x20, 0xAF, 0xE7, 0x29, 0x2D}};   // события списка файлов
static const GUID SID_WBApp = {0x0002DF05, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_WB2 = {0xD30C1661, 0xCDAF, 0x11D0, {0x8A, 0x3E, 0x00, 0xC0, 0x4F, 0xC9, 0xE2, 0x6E}};
struct EvSink final : IDispatch {   // получатель событий: любое событие — сообщение нашему окну (номер события — в wParam)
    LONG ref = 1; HWND target; UINT msg;
    EvSink(HWND t, UINT m) : target(t), msg(m) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** pv) override {
        if (riid == IID_IUnknown || riid == IID_IDispatch || riid == DIID_WBEvents2 || riid == DIID_SFVEvents) { *pv = static_cast<IDispatch*>(this); AddRef(); return S_OK; }
        *pv = nullptr; return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref); }
    ULONG STDMETHODCALLTYPE Release() override { LONG r = InterlockedDecrement(&ref); if (!r) delete this; return r; }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* p) override { if (p) *p = 0; return S_OK; }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID id, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) override {
        if (target) PostMessageW(target, msg, (WPARAM)id, 0);
        return S_OK;
    }
};
struct EvHook { IConnectionPoint* cp = nullptr; DWORD cookie = 0; EvSink* sink = nullptr; };
static void Unhook(EvHook& k) {
    if (k.cp) { k.cp->Unadvise(k.cookie); k.cp->Release(); }
    if (k.sink) { k.sink->target = nullptr; k.sink->Release(); }
    k = EvHook();
}
static bool HookEvents(EvHook& k, IUnknown* src, REFIID diid, HWND target, UINT msg) {
    Unhook(k);
    IConnectionPointContainer* cpc = nullptr;
    if (!src || FAILED(src->QueryInterface(IID_PPV_ARGS(&cpc))) || !cpc) return false;
    IConnectionPoint* cp = nullptr;
    bool ok = false;
    if (SUCCEEDED(cpc->FindConnectionPoint(diid, &cp)) && cp) {
        EvSink* sk = new EvSink(target, msg); DWORD c = 0;
        if (SUCCEEDED(cp->Advise(sk, &c))) { k.cp = cp; k.cookie = c; k.sink = sk; ok = true; }
        else { sk->Release(); cp->Release(); }
    }
    cpc->Release();
    return ok;
}

// цвет фона списка файлов — из темы оформления Windows (там же его берёт сам Проводник); не вышло — замер с Windows 11
static COLORREF g_listBg[2] = {CLR_INVALID, CLR_INVALID};   // [0] тёмная тема, [1] светлая
static bool g_listBgTheme[2] = {false, false};              // взят ли из темы (для отладки)
static COLORREF ListBg(bool light) {
    int k = light ? 1 : 0;
    if (g_listBg[k] == CLR_INVALID) {
        COLORREF c = CLR_INVALID;
        HTHEME t = OpenThemeData(nullptr, light ? L"ItemsView::ListView" : L"DarkMode_ItemsView::ListView");
        if (!t) t = OpenThemeData(nullptr, light ? L"ItemsView" : L"DarkMode_ItemsView");
        if (t) { COLORREF v = 0; if (SUCCEEDED(GetThemeColor(t, 0, 0, TMT_FILLCOLOR, &v))) c = v; CloseThemeData(t); }
        if (c != CLR_INVALID) {   // здравый смысл: у тёмной темы фон тёмный, у светлой — светлый
            int lum = (GetRValue(c) * 3 + GetGValue(c) * 6 + GetBValue(c)) / 10;
            if (light ? lum < 180 : lum > 90) c = CLR_INVALID;
        }
        g_listBgTheme[k] = c != CLR_INVALID;
        g_listBg[k] = c != CLR_INVALID ? c : (light ? RGB(255, 255, 255) : RGB(25, 25, 25));
    }
    return g_listBg[k];
}
static void ResetListBg() { g_listBg[0] = g_listBg[1] = CLR_INVALID; }

struct StripState { int hover = -1, pressed = -1, prevHover = -1; float animT = 1.f; bool animating = false; double animLast = 0; unsigned enabled = 0xFFFFFFFF; HWND tip = nullptr; EvHook viewHook; COLORREF viewBg = CLR_INVALID; ULONG notifyId = 0; HDC memDC = nullptr; HBITMAP memBmp = nullptr; HGDIOBJ memOld = nullptr; int memW = 0, memH = 0; };

struct Slot { int idx; RECT rc; };

// у шестерёнки нажимается только стрелочка

// у шестерёнки нажимается только стрелочка (правая часть кнопки)
// середина содержимого панели кнопок (в пикселях) по её настоящей высоте hPx.
// У Windows при высоте 47 знаки на 1⅓ точки ниже середины (снизу черта); чем ниже панель, тем ближе к точной середине
static int StripCenterPx(int hPx, UINT dpi) {
    double H = hPx * 96.0 / dpi, line = 1.333;
    double k = (H - 36) / (46.667 - 36); k = k < 0 ? 0 : k > 1 ? 1 : k;
    return (int)lround(((H - line) / 2 + line * k) * dpi / 96.0);
}
static RECT ActiveRect(int idx, const RECT& rc, UINT dpi) {   // подсветка: на 2 точки уже кнопки, высота 36 вокруг середины панели
    (void)idx;
    double H = rc.bottom * 96.0 / dpi, hh = H - 8 < 36 ? H - 8 : 36;   // высота 36, в узкой панели — с отступом 4 от краёв
    int hp = (int)lround(hh * dpi / 96.0), cy = StripCenterPx(rc.bottom, dpi);
    return {rc.left + (int)lround(2.0 * dpi / 96), cy - hp / 2, rc.right - (int)lround(2.0 * dpi / 96), cy - hp / 2 + hp};
}

static COLORREF MixColor(COLORREF a, COLORREF b, double t) {
    auto m = [&](int x, int y) { return (int)(x + (y - x) * t + 0.5); };
    return RGB(m(GetRValue(a), GetRValue(b)), m(GetGValue(a), GetGValue(b)), m(GetBValue(a), GetBValue(b)));
}

// подсветка как у Windows 11: полупрозрачный белый (тёмная тема) или чёрный (светлая) поверх фона окна
static COLORREF HoverColor(COLORREF bg, bool light, bool down) {
    double a = light ? (down ? 0.0235 : 0.0373) : (down ? 0.0392 : 0.0605);
    int t = light ? 0 : 255;
    auto mix = [&](int c) { return (int)(c + (t - c) * a + 0.5); };
    return RGB(mix(GetRValue(bg)), mix(GetGValue(bg)), mix(GetBValue(bg)));
}

static COLORREF SelColor(COLORREF bg, bool light) { return HoverColor(HoverColor(HoverColor(bg, light, false), light, false), light, false); }   // выбранное — заметнее наведения

// скруглённый прямоугольник с гладкими краями (радиус 4 точки, как у Windows 11)
static void FillRoundAA(HDC dc, const RECT& r, float radius, COLORREF c) {
    if (!InitD2D()) return;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.f, 96.f);
    ID2D1DCRenderTarget* rt = nullptr;
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &rt)) || !rt) return;
    RECT b = r;
    if (SUCCEEDED(rt->BindDC(dc, &b))) {
        ID2D1SolidColorBrush* br = nullptr;
        rt->BeginDraw();
        if (SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(c) / 255.f, GetGValue(c) / 255.f, GetBValue(c) / 255.f, 1.f), &br)) && br) {
            D2D1_ROUNDED_RECT rr = {D2D1::RectF(0.f, 0.f, (float)(r.right - r.left), (float)(r.bottom - r.top)), radius, radius};
            rt->FillRoundedRectangle(rr, br);
            br->Release();
        }
        rt->EndDraw();
    }
    rt->Release();
}


// готовые настройки шрифта для всех надписей мода: создаются один раз и дальше берутся из запаса
struct TfKey { std::wstring face; int weight; float size; int align; bool dots; bool operator<(const TfKey& o) const {
    return std::tie(face, weight, size, align, dots) < std::tie(o.face, o.weight, o.size, o.align, o.dots); } };
static std::map<TfKey, std::pair<IDWriteTextFormat*, IDWriteInlineObject*>> g_tf;
static SRWLOCK g_tfLock = SRWLOCK_INIT;
static IDWriteTextFormat* TextFormat(const wchar_t* face, int weight, float size, int align, bool dots) {   // align: 0 — влево, 1 — по центру, 2 — вправо
    TfKey k{face, weight, size, align, dots};
    AcquireSRWLockExclusive(&g_tfLock);
    auto it = g_tf.find(k);
    IDWriteTextFormat* f = it != g_tf.end() ? it->second.first : nullptr;
    if (!f && g_dw && SUCCEEDED(g_dw->CreateTextFormat(face, nullptr, (DWRITE_FONT_WEIGHT)weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"ru-ru", &f)) && f) {
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (align) f->SetTextAlignment(align == 1 ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_TRAILING);
        IDWriteInlineObject* d = nullptr;
        if (dots && SUCCEEDED(g_dw->CreateEllipsisTrimmingSign(f, &d)) && d) { DWRITE_TRIMMING tr = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0}; f->SetTrimming(&tr, d); }
        g_tf[k] = {f, d};
    }
    ReleaseSRWLockExclusive(&g_tfLock);
    return f;
}
static void ClearTextFormats() {
    AcquireSRWLockExclusive(&g_tfLock);
    for (auto& e : g_tf) { if (e.second.second) e.second.second->Release(); if (e.second.first) e.second.first->Release(); }
    g_tf.clear();
    ReleaseSRWLockExclusive(&g_tfLock);
}

// настройки сглаживания букв (как у Windows 11) — тоже один раз на всё время работы
static IDWriteRenderingParams1* g_rp = nullptr;
static SRWLOCK g_rpLock = SRWLOCK_INIT;
static IDWriteRenderingParams1* TextParams() {
    AcquireSRWLockExclusive(&g_rpLock);
    if (!g_rp && g_dw) {
        IDWriteFactory1* f1 = nullptr;
        if (SUCCEEDED(g_dw->QueryInterface(__uuidof(IDWriteFactory1), (void**)&f1)) && f1) {
            f1->CreateCustomRenderingParams(1.8f, 0.f, 0.f, 0.f, DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_GDI_CLASSIC, &g_rp);   // у Windows вертикальные штрихи букв ровно по пикселям
            f1->Release();
        }
    }
    ReleaseSRWLockExclusive(&g_rpLock);
    return g_rp;
}

// надпись тем же движком, что и у Windows 11 (DirectWrite): те же толщина и сглаживание букв
static void DrawTextDWSize(HDC dc, const RECT& r, const wchar_t* text, COLORREF col, UINT dpi, float pt, int weight = 400, bool ellipsis = false,
                           const wchar_t* face = L"Segoe UI", int align = 0) {   // align: 0 — влево, 1 — по центру, 2 — вправо
    bool done = false;
    if (InitD2D()) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.f, 96.f);
        ID2D1DCRenderTarget* rt = nullptr;
        IDWriteTextFormat* fmt = TextFormat(face, weight, pt * dpi / 96.f, align, ellipsis);   // из запаса — не освобождать
        ID2D1SolidColorBrush* br = nullptr;
        RECT b = r;
        if (fmt && SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && rt && SUCCEEDED(rt->BindDC(dc, &b)) &&
            SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(GetRValue(col) / 255.f, GetGValue(col) / 255.f, GetBValue(col) / 255.f), &br)) && br) {
            // без «усиления контраста»: оно утолщает светлые буквы на тёмном фоне, у Windows их нет
            if (IDWriteRenderingParams1* rp = TextParams()) rt->SetTextRenderingParams(rp);
            rt->BeginDraw();
            rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            rt->DrawText(text, (UINT32)wcslen(text), fmt, D2D1::RectF(0.f, 0.f, (float)(r.right - r.left), (float)(r.bottom - r.top)), br,
                         D2D1_DRAW_TEXT_OPTIONS_CLIP);
            done = SUCCEEDED(rt->EndDraw());
        }
        if (br) br->Release();
        if (rt) rt->Release();
    }
    if (!done) {   // запасной путь
        HFONT f = CreateFontW(-(int)lround(pt * dpi / 96.0), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, face);
        HGDIOBJ of = SelectObject(dc, f);
        SetBkMode(dc, TRANSPARENT); SetTextColor(dc, col);
        RECT t = r;
        DrawTextW(dc, text, -1, &t, (align == 1 ? DT_CENTER : align == 2 ? DT_RIGHT : DT_LEFT) | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, of); DeleteObject(f);
    }
}

static void DrawTextDW(HDC dc, const RECT& r, const wchar_t* text, COLORREF col, UINT dpi) { DrawTextDWSize(dc, r, text, col, dpi, 12.f); }
static int TextWidthDWSize(const wchar_t* t, UINT dpi, float pt) {
    if (!InitD2D()) return 0;
    IDWriteTextFormat* fmt = TextFormat(L"Segoe UI", 400, pt * dpi / 96.f, 0, false);   // из запаса — не освобождать
    IDWriteTextLayout* lay = nullptr; int w = 0;
    if (fmt && SUCCEEDED(g_dw->CreateTextLayout(t, (UINT32)wcslen(t), fmt, 10000.f, 1000.f, &lay)) && lay) {
        DWRITE_TEXT_METRICS m = {};
        if (SUCCEEDED(lay->GetMetrics(&m))) w = (int)ceilf(m.widthIncludingTrailingWhitespace);
    }
    if (lay) lay->Release();
    return w;
}

static const wchar_t* NewText() { return TR(L"Создать", L"New"); }   // надпись на кнопке — на языке Windows
static HFONT BarFont(UINT dpi) {   // надпись как у Windows: 12 точек, сглаживание без цветных краёв
    return CreateFontW(-MulDiv(12, dpi, 96), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                       L"Segoe UI");   // Windows сама сообщила: Segoe UI, 12, обычная толщина
}
static int TextWidthDW(const wchar_t* t, UINT dpi) { return TextWidthDWSize(t, dpi, 12.f); }
static int NewTextWidth(UINT dpi) {
    int dw = TextWidthDW(NewText(), dpi);
    if (dw > 0) return dw;
    HDC dc = GetDC(nullptr);
    HFONT f = BarFont(dpi); HGDIOBJ of = SelectObject(dc, f);
    SIZE sz = {}; GetTextExtentPoint32W(dc, NewText(), (int)wcslen(NewText()), &sz);
    SelectObject(dc, of); DeleteObject(f); ReleaseDC(nullptr, dc);
    return sz.cx;
}
// ---------- размеры — точно такие, какие сообщила сама Windows (в точках при 100%) ----------
// кнопка со значком: ширина 40, значок 16×16 с отступом 12 слева и 16 сверху; шаг между кнопками 48 (промежуток 8)
// «Создать»: отступ 12, значок 16, 8, надпись (Segoe UI 12), 5⅓, стрелочка 8×8 (сверху 20), отступ 12
// разделитель: через 2⅔ после кнопки, затем 7⅓ до следующей; высота линии ≈30⅔; середина панели по высоте — 24 от верха
static const double BTN_W = 40, GAP = 8, PAD_L = 9.333, PAD_R = 9.333, SEP_BEFORE = 4.0, SEP_AFTER = 6.0;   // замер по пикселям Windows
static const double ICON_X = 12, TEXT_GAP = 8, CHEV_GAP = 5.333, CHEV = 8;

static double NewTextW96(UINT dpi) { return ceil(NewTextWidth(dpi) * 96.0 / dpi - 0.01); }   // Windows округляет ширину надписи вверх (44 для «Создать»)
static double SlotW96(int i, UINT dpi) {
    switch (g_btns[i].act) {
        case A_NEW: return ICON_X + 16 + TEXT_GAP + NewTextW96(dpi) + CHEV_GAP + CHEV + 12;
        case A_SORT: case A_VIEW: return ICON_X + 16 + TEXT_GAP + CHEV + 12;   // значок + стрелочка, без надписи
        case A_VIEWMENU: return 8 + CHEV + 8;
        default: return BTN_W;
    }
}
static int P(double v, UINT dpi) { return (int)lround(v * dpi / 96.0); }   // точки → пиксели


static double GapAfter(Act a) {   // промежуток после кнопки: стрелочка вплотную к значку, «Отменить» и «Повторить» — парой, ближе друг к другу
    return a == A_VIEWMENU ? 0 : a == A_UNDO ? 0 : GAP;
}
static std::vector<Slot> Layout(HWND strip) {
    RECT cr; GetClientRect(strip, &cr);
    UINT dpi = DpiOf(strip);
    int lt = MulDiv(1, dpi, 96); if (lt < 1) lt = 1;
    double lt96 = lt * 96.0 / dpi;
    std::vector<Slot> v;
    double x = PAD_L;
    int i = 0;
    for (; i < BTN_COUNT && g_btns[i].act != A_SPACER; ++i) {
        if (g_btns[i].act == A_SEP) {
            x -= GAP;                                   // вместо обычного промежутка — место под разделитель
            double lx = x + SEP_BEFORE;
            double H = cr.bottom * 96.0 / dpi, sl = H - 12 < 32 ? H - 12 : 32;   // черта: 32, в узкой панели короче
            int sp = (int)lround(sl * dpi / 96.0), cy = StripCenterPx(cr.bottom, dpi);
            v.push_back({i, {P(lx, dpi), cy - sp / 2, P(lx, dpi) + lt, cy - sp / 2 + sp}});   // у Windows линия с 8 до 40 точки
            x = lx + lt96 + SEP_AFTER;
            continue;
        }
        double w = SlotW96(i, dpi);
        v.push_back({i, {P(x, dpi), 0, P(x + w, dpi), cr.bottom}});
        x += w + GapAfter(g_btns[i].act);
    }
    // правая группа — прижата к правому краю
    double total = 0;
    for (int j = i + 1; j < BTN_COUNT; ++j) total += SlotW96(j, dpi) + (j + 1 < BTN_COUNT ? GapAfter(g_btns[j].act) : 0);
    x = cr.right * 96.0 / dpi - PAD_R - total;
    for (int j = i + 1; j < BTN_COUNT; ++j) {
        double w = SlotW96(j, dpi);
        v.push_back({j, {P(x, dpi), 0, P(x + w, dpi), cr.bottom}});
        x += w + GapAfter(g_btns[j].act);
    }
    return v;
}

// поправки по замеру настоящей панели Windows (в пикселях), считаются автоматически один раз

static void PaintContent(HWND h, StripState* st, HDC dc, RECT cr);
static void PaintStrip(HWND h, StripState* st) {
    PAINTSTRUCT ps; HDC wdc = BeginPaint(h, &ps);
    RECT cr; GetClientRect(h, &cr);
    if (!st->memDC || st->memW != cr.right || st->memH != cr.bottom) {   // размер поменялся — новая картинка
        if (st->memDC) { SelectObject(st->memDC, st->memOld); DeleteObject(st->memBmp); DeleteDC(st->memDC); }
        st->memDC = CreateCompatibleDC(wdc);
        st->memBmp = CreateCompatibleBitmap(wdc, cr.right, cr.bottom);
        st->memOld = SelectObject(st->memDC, st->memBmp);
        st->memW = cr.right; st->memH = cr.bottom;
    }
    PaintContent(h, st, st->memDC, cr);
    BitBlt(wdc, 0, 0, cr.right, cr.bottom, st->memDC, 0, 0, SRCCOPY);
    EndPaint(h, &ps);
}

static void PaintContent(HWND h, StripState* st, HDC dc, RECT cr) {
    bool light = IsLight();
    UINT dpi = DpiOf(h);

    COLORREF bg = st->viewBg != CLR_INVALID ? st->viewBg : ListBg(light);   // цвет самого списка файлов (или из темы)
    HBRUSH bgb = CreateSolidBrush(bg); FillRect(dc, &cr, bgb); DeleteObject(bgb);
    {   // черта между панелью и списком файлов — от края до края окна (замер с Windows 11)
        RECT ln = {0, cr.bottom - P(1.333, dpi), cr.right, cr.bottom};
        HBRUSH lb = CreateSolidBrush(light ? RGB(201, 201, 202) : RGB(58, 58, 58)); FillRect(dc, &ln, lb); DeleteObject(lb);
    }

    int s = P(20, dpi);                  // квадрат значка 20, сам знак 16
    int cs = P(8.667, dpi);   // стрелочка: маленький знак, 8 пикселей в ширину при 150% — как у Windows
    for (auto& sl : Layout(h)) {
        const RECT& b = sl.rc;
        if (g_btns[sl.idx].act == A_SEP) {
            RECT sb = b;
            HBRUSH lb = CreateSolidBrush(MixColor(bg, light ? RGB(0, 0, 0) : RGB(255, 255, 255), 0.0826));
            FillRect(dc, &sb, lb); DeleteObject(lb);
            continue;
        }
        bool en = (st->enabled >> sl.idx) & 1;
        float k = sl.idx == st->hover ? st->animT : sl.idx == st->prevHover ? 1.f - st->animT : 0.f;   // плавное наведение
        if (en && (k > 0.f || sl.idx == st->pressed)) {
            bool down = sl.idx == st->pressed && sl.idx == st->hover;
            FillRoundAA(dc, ActiveRect(sl.idx, b, dpi), 4.f * dpi / 96.f, down ? HoverColor(bg, light, true) : MixColor(bg, HoverColor(bg, light, false), k));
        }
        Act a = g_btns[sl.idx].act;
        int cy = StripCenterPx(cr.bottom, dpi);   // все знаки — ровно по одной середине
        auto iconAt = [&](HICON ic, double x96, int box) {   // знак 16 внутри квадрата 20 → квадрат на 2 точки левее
            if (ic) DrawIconEx(dc, b.left + P(x96 - 2, dpi), cy - box / 2, ic, box, box, 0, nullptr, DI_NORMAL);
        };
        auto chevAt = [&](double x96) {   // x96 — левый край поля 8×8, как у Windows; знак ставим по его середине
            HICON cv = GetIcon(100, cs, light, !en);
            if (cv) DrawIconEx(dc, b.left + P(x96 + CHEV / 2, dpi) - cs / 2, cy - cs / 2, cv, cs, cs, 0, nullptr, DI_NORMAL);
        };
        if (a == A_VIEWMENU) {
            chevAt(8);
        } else if (a == A_NEW) {
            iconAt(GetIcon(sl.idx, s, light, !en), ICON_X, s);
            double tx = ICON_X + 16 + TEXT_GAP, tw = NewTextW96(dpi);
            int th = P(16, dpi);
            RECT tr = {b.left + P(tx, dpi), cy - th / 2, b.left + P(tx + tw + 2, dpi), cy - th / 2 + th};
            DrawTextDW(dc, tr, NewText(), light ? RGB(27, 27, 27) : RGB(255, 255, 255), dpi);
            chevAt(tx + tw + CHEV_GAP);
        } else if (a == A_SORT || a == A_VIEW) {
            iconAt(GetIcon(sl.idx, s, light, !en), ICON_X, s);
            chevAt(ICON_X + 16 + TEXT_GAP);
        } else {
            iconAt(GetIcon(sl.idx, s, light, !en), ICON_X, s);
        }
    }
}

static int HitTest(HWND h, POINT pt) {
    UINT dpi = DpiOf(h);
    for (auto& sl : Layout(h)) {
        if (g_btns[sl.idx].act == A_SEP || g_btns[sl.idx].act == A_SPACER) continue;
        RECT a = ActiveRect(sl.idx, sl.rc, dpi);
        if (PtInRect(&a, pt)) return sl.idx;
    }
    return -1;
}


// ---------------- состояние кнопок (как в Windows 11) ----------------
struct ChildFind { const wchar_t* cls; bool visible; HWND found; };
static BOOL CALLBACK ChildFindCb(HWND h, LPARAM lp) {
    ChildFind* c = (ChildFind*)lp;
    wchar_t cls[64];
    if (GetClassNameW(h, cls, 64) && wcscmp(cls, c->cls) == 0 && (!c->visible || IsWindowVisible(h))) { c->found = h; return FALSE; }
    return TRUE;
}
static HWND FindChild(HWND parent, const wchar_t* cls, bool visibleOnly) {   // первое вложенное окно этого вида (на любой глубине)
    ChildFind c = {cls, visibleOnly, nullptr};
    if (parent) EnumChildWindows(parent, ChildFindCb, (LPARAM)&c);
    return c.found;
}
// окно Проводника отдаёт свою «начинку» (открытую папку и управление ею) по особому сообщению — сначала у видимой вкладки
static IShellBrowser* BrowserOf(HWND top);
static bool HookNav(HWND bar, EvHook& k, HWND top) {   // окно Проводника сообщит нашему верху о каждом открытии папки
    IShellBrowser* sb = BrowserOf(top);
    if (!sb) return false;
    IServiceProvider* sp = nullptr; IUnknown* wb = nullptr; bool ok = false;
    if (SUCCEEDED(sb->QueryInterface(IID_PPV_ARGS(&sp))) && sp) {
        if (SUCCEEDED(sp->QueryService(SID_WBApp, IID_WB2, (void**)&wb)) && wb) { ok = HookEvents(k, wb, DIID_WBEvents2, bar, WM_APP + 40); wb->Release(); }
        sp->Release();
    }
    return ok;
}
static COLORREF ViewBg(HWND top) {   // цвет фона открытого списка файлов — прямо у него (CLR_INVALID — не отдал)
    IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
    IShellView* sv = nullptr; COLORREF c = CLR_INVALID;
    if (sb && SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        IVisualProperties* vp = nullptr;
        if (SUCCEEDED(sv->QueryInterface(IID_PPV_ARGS(&vp))) && vp) {
            COLORREF v = 0;
            if (SUCCEEDED(vp->GetColor(VPCF_BACKGROUND, &v)) && v != CLR_DEFAULT && v != CLR_NONE && !(v & 0xFF000000)) {
                int lum = (GetRValue(v) * 3 + GetGValue(v) * 6 + GetBValue(v)) / 10;
                if (IsLight() ? lum >= 180 : lum <= 90) c = v;   // здравый смысл: цвет должен подходить к теме
            }
            vp->Release();
        }
        sv->Release();
    }
    return c;
}
static bool HookView(HWND strip, EvHook& k) {   // список файлов сообщит панели кнопок о каждой смене выделения
    HWND top = GetAncestor(strip, GA_ROOT);
    IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
    bool ok = false;
    IShellView* sv = nullptr;
    if (sb && SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        IDispatch* d = nullptr;
        if (SUCCEEDED(sv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&d))) && d) { ok = HookEvents(k, d, DIID_SFVEvents, strip, WM_APP + 42); d->Release(); }
        sv->Release();
    }
    if (!ok) Unhook(k);
    return ok;
}
static IShellBrowser* BrowserOf(HWND top) {   // не увеличивает счётчик — не освобождать
    HWND tab = nullptr;   // вкладка Проводника — прямо внутри окна (без обхода всех вложенных окон: это вызывается много раз в секунду)
    for (HWND c = FindWindowExW(top, nullptr, L"ShellTabWindowClass", nullptr); c; c = FindWindowExW(top, c, L"ShellTabWindowClass", nullptr))
        if (IsWindowVisible(c)) { tab = c; break; }
    HWND target = tab ? tab : top;
    IShellBrowser* sb = (IShellBrowser*)SendMessageW(target, WM_USER + 7, 0, 0);
    if (!sb && target != top) sb = (IShellBrowser*)SendMessageW(top, WM_USER + 7, 0, 0);
    return sb;
}

// сколько выделено в открытой вкладке (-1 — узнать не удалось)
static int SelectedCount(HWND top) {
    IShellBrowser* sb = BrowserOf(top);
    if (!sb) return -1;
    IShellView* sv = nullptr;
    int n = -1;
    if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        IFolderView* fv = nullptr;
        if (SUCCEEDED(sv->QueryInterface(IID_PPV_ARGS(&fv))) && fv) {
            int c = 0;
            if (SUCCEEDED(fv->ItemCount(SVGIO_SELECTION, &c))) n = c;
            fv->Release();
        }
        sv->Release();
    }
    return n;
}

static bool ClipboardHasFiles() {
    static UINT fIdList = RegisterClipboardFormatW(L"Shell IDList Array");
    static UINT fDesc = RegisterClipboardFormatW(L"FileGroupDescriptorW");
    return IsClipboardFormatAvailable(CF_HDROP) || IsClipboardFormatAvailable(fIdList) || IsClipboardFormatAvailable(fDesc);
}

// можно ли сейчас «Отменить» / «Повторить»: спрашиваем то же меню, что по правой кнопке на пустом месте папки —
// Windows сама кладёт туда «Отменить …» и «Повторить …», когда есть что отменять или повторять
static IFolderView2* GetFolderView2(HWND top);
static bool UndoRedoState(HWND top, bool& canU, bool& canR) {
    IFolderView2* fv = GetFolderView2(top);
    if (!fv) return false;
    bool ok = false;
    IShellView* sv = nullptr;
    if (SUCCEEDED(fv->QueryInterface(IID_PPV_ARGS(&sv))) && sv) {
        IContextMenu* cm = nullptr;
        if (SUCCEEDED(sv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&cm))) && cm) {
            HMENU m = CreatePopupMenu();
            if (m && SUCCEEDED(cm->QueryContextMenu(m, 0, 1, 0x7FFF, CMF_NORMAL))) {
                ok = true; canU = canR = false;
                int n = GetMenuItemCount(m);
                for (int i = 0; i < n; ++i) {
                    UINT id = GetMenuItemID(m, i);
                    UINT stt = GetMenuState(m, i, MF_BYPOSITION);
                    bool en = !(stt & (MF_GRAYED | MF_DISABLED));
                    wchar_t verb[64] = {}, txt[256] = {};
                    if (id >= 1 && id <= 0x7FFF) cm->GetCommandString(id - 1, GCS_VERBW, nullptr, (LPSTR)verb, 64);
                    GetMenuStringW(m, i, txt, 256, MF_BYPOSITION);
                    std::wstring t;
                    for (wchar_t* c = txt; *c; ++c) if (*c != L'&') t += *c;
                    auto starts = [&](const wchar_t* p) { return _wcsnicmp(t.c_str(), p, wcslen(p)) == 0; };
                    if (_wcsicmp(verb, L"undo") == 0 || starts(L"Отменить") || starts(L"Undo")) canU = canU || en;
                    if (_wcsicmp(verb, L"redo") == 0 || starts(L"Повторить") || starts(L"Вернуть") || starts(L"Redo")) canR = canR || en;
                }
            }
            if (m) DestroyMenu(m);
            cm->Release();
        }
        sv->Release();
    }
    fv->Release();
    return ok;
}

static bool g_canUndo = true, g_canRedo = true;   // список отмены у Windows один на все окна Проводника
static DWORD g_undoTick = 0;
static void CheckUndoNow(HWND strip) {   // спросить Windows прямо сейчас
    HWND top = GetAncestor(strip, GA_ROOT);
    if (!top) return;
    bool u = true, r = true;
    if (UndoRedoState(top, u, r)) { g_canUndo = u; g_canRedo = r; }   // узнать не удалось — кнопки не гасим
    g_undoTick = GetTickCount();
}

static unsigned ComputeEnabled(HWND strip) {
    HWND top = GetAncestor(strip, GA_ROOT);
    if (top && GetForegroundWindow() == top && GetTickCount() - g_undoTick > 5000) CheckUndoNow(strip);   // запасная проверка — раз в 5 с
    int sel = top ? SelectedCount(top) : -1;
    bool hasSel = sel != 0;          // если узнать не удалось — не гасим кнопки
    bool paste = ClipboardHasFiles();
    unsigned m = 0;
    for (int i = 0; i < BTN_COUNT; ++i) {
        bool en = true;
        switch (g_btns[i].act) {
            case A_CUT: case A_COPY: case A_RENAME: case A_DELETE: en = hasSel; break;
            case A_PASTE: en = paste; break;
            case A_UNDO: en = g_canUndo; break;
            case A_REDO: en = g_canRedo; break;
            default: break;
        }
        if (en) m |= 1u << i;
    }
    return m;
}


// ---------------- цвет фона берём у самого окна Проводника ----------------

// цвет серых граней берём у рамки самого окна Проводника (слева от панели)

// ---------------- меню шестерёнки (как у встроенной панели) ----------------
static DWORD WINAPI NotifyThread(LPVOID) {
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ShellState", SMTO_ABORTIFHUNG, 1000, nullptr);
    return 0;
}
static void NotifyShell() {
    HANDLE t = CreateThread(nullptr, 0, NotifyThread, nullptr, 0, nullptr);
    if (t) CloseHandle(t);
}
static const wchar_t* ADV_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static bool GetStatusBar() {   // строка состояния внизу окна («Элементов: …»); нет записи — значит показана
    DWORD v = 1, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER, ADV_KEY, L"ShowStatusBar", RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v != 0;
}
static bool GetCompact() {
    DWORD v = 0, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER, ADV_KEY, L"UseCompactMode", RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v != 0;
}

enum { M_OPTIONS = 1, M_COMPACT = 10, M_CHECKBOXES, M_EXTENSIONS, M_HIDDEN, M_NAVPANE, M_STATUSBAR };

// Меню рисуем сами, как у Windows 11: галочка слева, значок, текст, тонкие серые разделители.
// Размеры сняты со стокового меню: строка 32 точки, галочка по центру 19, значок по центру 50, текст с 72.
static bool g_menuCompact = false;   // меню «Создать»: без колонки галочек, размеры как у Windows

static COLORREF MenuBg(bool light) { return light ? RGB(249, 249, 249) : RGB(43, 43, 43); }   // замер стокового меню (43)


// ---------- наше собственное меню: список пунктов ----------
struct MItem { UINT id = 0; std::wstring text; int mark = 0; HICON icon = nullptr; bool sep = false; std::vector<MItem> sub; bool hasSub = false; };
using MList = std::vector<MItem>;

static void AddItem(MList* m, UINT id, const wchar_t* text, int mark, wchar_t glyph, int s, bool light,
                    const std::vector<const char*>& keys) {
    std::vector<BYTE> px;
    AcquireSRWLockExclusive(&g_lock);
    const SvgDoc* doc = LoadSvg(FindSvg(light, keys));
    if (doc) px = RenderSvg(*doc, s, (float)s, 1.0);
    ReleaseSRWLockExclusive(&g_lock);
    if (px.empty() && glyph) px = RenderGlyph(glyph, AC_NONE, s, light, false);
    MItem it; it.id = id; it.text = text; it.mark = mark; it.icon = px.empty() ? nullptr : IconFromPx(px, s);
    m->push_back(std::move(it));
}


static void AddSep(MList* m) { MItem it; it.sep = true; m->push_back(std::move(it)); }

static void FreeMList(MList& m) {
    for (auto& it : m) { if (it.icon) DestroyIcon(it.icon); it.icon = nullptr; FreeMList(it.sub); }
    m.clear();
}


// открыть наше меню под кнопкой; build — заполняет пункты; возвращает выбранный пункт


// ---------- наши окна меню (вместо системных): как всплывающие меню Windows 11 ----------
// правила Windows 11: рамка 1, поля сверху/снизу 2, у пункта отступы 4 по бокам и содержимое с 16 от края;
// колонка под галочку/точку и колонка значков — по 28; строка 31⅓; разделитель — строка 4 с линией посередине;
// скругление окна 8, подсветка пункта со скруглением 4; шрифт Segoe UI 14
static const wchar_t* MENU_CLASS = L"EIT_Menu";
struct MWin {
    HWND h = nullptr; MList* items = nullptr; int hover = -1; bool icons = false, marks = false, anySub = false;
    std::vector<RECT> rows; int w = 0, hgt = 0; int openSub = -1;
    int ml = 0, mt = 0, mr = 0, mb = 0;   // прозрачные поля вокруг меню под тень (как у окна меню Windows)
};
static std::vector<MWin*> g_mstack;
static RECT BodyRect(MWin* mw) {   // тело меню на экране (без прозрачных полей)
    RECT wr; GetWindowRect(mw->h, &wr);
    return {wr.left + mw->ml, wr.top + mw->mt, wr.left + mw->ml + mw->w, wr.top + mw->mt + mw->hgt};
}

   // открытые окна меню: корень и вложенные

static double MI(double v, UINT dpi) { return v * dpi / 96.0; }

static void MeasureMWin(MWin* mw, UINT dpi) {
    mw->icons = false; mw->marks = !g_menuCompact; mw->anySub = false;
    for (auto& it : *mw->items) { if (it.icon) mw->icons = true; if (it.hasSub) mw->anySub = true; }
    HDC dc = GetDC(nullptr);
    double textX = (mw->icons && mw->marks) ? 72 : 44;
    double maxW = 0;
    for (auto& it : *mw->items) {
        if (it.sep) continue;
        double tw = TextWidthDWSize(it.text.c_str(), dpi, 14.f) * 96.0 / dpi;
        if (tw > maxW) maxW = tw;
    }
    ReleaseDC(nullptr, dc);
    double w96 = textX + maxW + 16 + (mw->anySub ? 24 : 0);
    if (w96 < 120) w96 = 120;
    mw->w = (int)lround(MI(w96, dpi));
    mw->rows.clear();
    double y = 1 + 2;
    for (auto& it : *mw->items) {
        double rh = it.sep ? 4 : 31.333;
        mw->rows.push_back({0, (int)lround(MI(y, dpi)), mw->w, (int)lround(MI(y + rh, dpi))});
        y += rh;
    }
    mw->hgt = (int)lround(MI(y + 2 + 1, dpi));
    // замер окна стокового меню при 150%: поля 16 слева и справа, 4 сверху, 28 снизу (пиксели)
    mw->ml = mw->mr = (int)lround(MI(10.667, dpi)); mw->mt = (int)lround(MI(2.667, dpi)); mw->mb = (int)lround(MI(18.667, dpi));
}

static void PaintMWin(MWin* mw, HDC wdc) {
    UINT dpi = DpiOf(mw->h);
    bool light = IsLight();
    RECT cr = {0, 0, mw->w, mw->hgt};
    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, cr.right, cr.bottom);
    HGDIOBJ ob = SelectObject(dc, bmp);
    COLORREF bg = MenuBg(light);
    HBRUSH bb = CreateSolidBrush(bg); FillRect(dc, &cr, bb); DeleteObject(bb);
    COLORREF fg = light ? RGB(27, 27, 27) : RGB(255, 255, 255);
    bool one = !(mw->icons && mw->marks);
    for (size_t i = 0; i < mw->items->size(); ++i) {
        MItem& it = (*mw->items)[i];
        RECT r = mw->rows[i];
        int cy = (r.top + r.bottom) / 2;
        if (it.sep) {   // линия на всю ширину, толщиной в 1 точку
            int lt = MulDiv(1, dpi, 96); if (lt < 1) lt = 1;
            RECT ln = {r.left, cy - lt / 2, r.right, cy - lt / 2 + lt};
            HBRUSH lb = CreateSolidBrush(MixColor(bg, light ? RGB(0, 0, 0) : RGB(255, 255, 255), 0.0826));
            FillRect(dc, &ln, lb); DeleteObject(lb);
            continue;
        }
        if ((int)i == mw->hover || (int)i == mw->openSub) {   // подсветка: отступы 4 по бокам и 2 сверху/снизу, скругление 4
            RECT hr = {r.left + (int)lround(MI(5, dpi)), r.top + (int)lround(MI(2, dpi)), r.right - (int)lround(MI(5, dpi)), r.bottom - (int)lround(MI(2, dpi))};
            FillRoundAA(dc, hr, 4.f * dpi / 96.f, HoverColor(bg, light, false));
        }
        double colC = 22.67;   // середина первой колонки
        if (it.mark == 2) {
            int ds = MulDiv(5, dpi, 96); if (ds < 3) ds = 3;
            int cx = (int)lround(MI(colC, dpi));
            RECT dot = {cx - ds / 2, cy - ds / 2, cx - ds / 2 + ds, cy - ds / 2 + ds};
            FillRoundAA(dc, dot, ds / 2.f, light ? RGB(27, 27, 27) : RGB(214, 214, 214));
        } else if (it.mark == 1) {
            int cs = MulDiv(16, dpi, 96);
            HICON ck = MakeIcon(0xE73E, AC_NONE, cs, light, false);
            if (ck) { DrawIconEx(dc, (int)lround(MI(colC, dpi)) - cs / 2, cy - cs / 2, ck, cs, cs, 0, nullptr, DI_NORMAL); DestroyIcon(ck); }
        }
        if (it.icon) {
            int is = MulDiv(16, dpi, 96);
            double icx = one ? colC : 50.67;
            DrawIconEx(dc, (int)lround(MI(icx, dpi)) - is / 2, cy - is / 2, it.icon, is, is, 0, nullptr, DI_NORMAL);
        }
        RECT tr = {(int)lround(MI(one ? 44 : 72, dpi)), r.top, r.right - (int)lround(MI(16, dpi)), r.bottom};
        DrawTextDWSize(dc, tr, it.text.c_str(), fg, dpi, 14.f);
        if (it.hasSub) {   // «›» у правого края
            int cs = MulDiv(12, dpi, 96);
            HICON cv = MakeIcon(0xE76C, AC_NONE, cs, light, false);
            if (cv) { DrawIconEx(dc, r.right - (int)lround(MI(21, dpi)) - cs / 2, cy - cs / 2, cv, cs, cs, 0, nullptr, DI_NORMAL); DestroyIcon(cv); }
        }
    }
    BitBlt(wdc, 0, 0, cr.right, cr.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
}


// окно меню рисуем целиком сами, как Проводник: прозрачное окно, внутри — тень, скруглённое тело с рамкой и пункты
static float RRDist(float px, float py, float x0, float y0, float x1, float y1, float r) {   // расстояние до скруглённого прямоугольника (<0 внутри)
    float cx = (x0 + x1) / 2, cy = (y0 + y1) / 2, hx = (x1 - x0) / 2 - r, hy = (y1 - y0) / 2 - r;
    float dx = fabsf(px - cx) - hx, dy = fabsf(py - cy) - hy;
    float ox = dx > 0 ? dx : 0, oy = dy > 0 ? dy : 0;
    float in = (dx > dy ? dx : dy); if (in > 0) in = 0;
    return sqrtf(ox * ox + oy * oy) + in - r;
}

static void RenderMWin(MWin* mw) {
    if (!mw || !mw->h) return;
    UINT dpi = DpiOf(mw->h);
    bool light = IsLight();
    int W = mw->w + mw->ml + mw->mr, H = mw->hgt + mw->mt + mw->mb;
    // 1) пункты меню — на непрозрачной картинке размером с тело меню
    BITMAPINFO bi = {}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biWidth = mw->w; bi.bmiHeader.biHeight = -mw->hgt;
    void* cb = nullptr;
    HDC sdc = GetDC(nullptr);
    HDC cdc = CreateCompatibleDC(sdc);
    HBITMAP cbmp = CreateDIBSection(cdc, &bi, DIB_RGB_COLORS, &cb, nullptr, 0);
    if (!cbmp) { DeleteDC(cdc); ReleaseDC(nullptr, sdc); return; }
    HGDIOBJ cob = SelectObject(cdc, cbmp);
    PaintMWin(mw, cdc);   // рисует фон, подсветку, значки, текст
    float rad = 8.f * dpi / 96.f;
    GdiFlush();
    // 2) итоговая прозрачная картинка: мягкая тень снизу и по бокам + тело меню со скруглёнными краями
    bi.bmiHeader.biWidth = W; bi.bmiHeader.biHeight = -H;
    void* fb = nullptr;
    HDC fdc = CreateCompatibleDC(sdc);
    HBITMAP fbmp = CreateDIBSection(fdc, &bi, DIB_RGB_COLORS, &fb, nullptr, 0);
    if (fbmp) {
        HGDIOBJ fob = SelectObject(fdc, fbmp);
        BYTE* dst = (BYTE*)fb; BYTE* src = (BYTE*)cb;
        float x0 = (float)mw->ml, y0 = (float)mw->mt, x1 = x0 + mw->w, y1 = y0 + mw->hgt;
        // по твоему скрину стокового меню (150%): тень сдвинута вниз и сужена с боков — внизу ровная полоса и плавный хвост,
        // по бокам едва заметна, сверху нет; край меню — тонкое тёмное кольцо снаружи, сверху — светлое
        float u = dpi / 96.f;
        // подобрано по пикселям твоего скрина стокового меню: сдвиг 3, сужение с боков 7, мягкость 6, сила 18%; кольцо рамки 18%
        float sOff = 3.f * u, sIn = 7.f * u, sSig = 6.f * u, sMax = light ? 0.12f : 0.18f;
        float bw = 1.f * u;
        float ringA = light ? 0.10f : 0.18f, topA = light ? 0.0f : 0.11f;
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x) {
                float px = x + 0.5f, py = y + 0.5f;
                float d = RRDist(px, py, x0, y0, x1, y1, rad);
                float covO = 0.5f - d; if (covO < 0) covO = 0; if (covO > 1) covO = 1;            // до внешнего края меню
                float covI = 0.5f - (d + bw); if (covI < 0) covI = 0; if (covI > 1) covI = 1;     // тело внутри кольца
                float ds = RRDist(px, py, x0 + sIn, y0 + sOff, x1 - sIn, y1 + sOff, rad);
                float sa = ds <= 0 ? sMax : sMax * expf(-(ds * ds) / (2 * sSig * sSig));
                if (py < y0) sa = 0;   // над меню у Windows тени нет
                // слои снизу вверх: тень (чёрная) → кольцо рамки → тело меню; всё в «умноженных» цветах
                float A = sa, C[3] = {0, 0, 0};
                float r = covO - covI;
                if (r > 0) {
                    bool top = py < y0 + rad && py < (y0 + y1) / 2;
                    float ra = (top ? topA : ringA) * r;
                    float rc = top ? 255.f : 0.f;
                    for (int k = 0; k < 3; ++k) C[k] = rc * ra + C[k] * (1 - ra);
                    A = ra + A * (1 - ra);
                }
                if (covI > 0) {
                    int cx = x - mw->ml, cy = y - mw->mt;
                    if (cx < 0) cx = 0; if (cy < 0) cy = 0; if (cx >= mw->w) cx = mw->w - 1; if (cy >= mw->hgt) cy = mw->hgt - 1;
                    BYTE* sp = src + ((size_t)cy * mw->w + cx) * 4;
                    for (int k = 0; k < 3; ++k) C[k] = sp[k] * covI + C[k] * (1 - covI);
                    A = covI + A * (1 - covI);
                }
                BYTE* o = dst + ((size_t)y * W + x) * 4;
                o[0] = (BYTE)(C[0] + 0.5f); o[1] = (BYTE)(C[1] + 0.5f); o[2] = (BYTE)(C[2] + 0.5f); o[3] = (BYTE)(A * 255.f + 0.5f);
            }
        RECT wr; GetWindowRect(mw->h, &wr);
        POINT pos = {wr.left, wr.top}, zero = {0, 0};
        SIZE sz = {W, H};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(mw->h, sdc, &pos, &sz, fdc, &zero, 0, &bf, ULW_ALPHA);
        SelectObject(fdc, fob); DeleteObject(fbmp);
    }
    DeleteDC(fdc);
    SelectObject(cdc, cob); DeleteObject(cbmp); DeleteDC(cdc);
    ReleaseDC(nullptr, sdc);
}

LRESULT CALLBACK MenuProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    MWin* mw = (MWin*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
        case WM_NCCALCSIZE: if (w) return 0; break;          // без системной рамки — всё окно наше
        case WM_NCHITTEST: return HTCLIENT;
        case WM_NCACTIVATE: return TRUE;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC dc = BeginPaint(h, &ps);
            if (mw) PaintMWin(mw, dc);
            EndPaint(h, &ps);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

static MWin* OpenMWin(HWND owner, MList* items, POINT anchor, bool alignRight, const RECT* avoid) {
    UINT dpi = DpiOf(owner);
    MWin* mw = new MWin(); mw->items = items;
    MeasureMWin(mw, dpi);
    int x = alignRight ? anchor.x - mw->w : anchor.x, y = anchor.y;
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST), &mi);
    RECT wa = mi.rcWork;
    if (x + mw->w > wa.right) x = avoid ? avoid->left - mw->w : wa.right - mw->w;   // вложенное меню не влезло справа — открываем слева
    if (x < wa.left) x = wa.left;
    if (y + mw->hgt > wa.bottom) y = (avoid ? avoid->bottom : anchor.y) - mw->hgt;
    if (y < wa.top) y = wa.top;
    // прозрачное окно больше самого меню на поля под тень — как окно меню Windows
    mw->h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, MENU_CLASS, L"", WS_POPUP,
                            x - mw->ml, y - mw->mt, mw->w + mw->ml + mw->mr, mw->hgt + mw->mt + mw->mb, owner, nullptr, g_inst, nullptr);
    if (!mw->h) { delete mw; return nullptr; }
    SetWindowLongPtrW(mw->h, GWLP_USERDATA, (LONG_PTR)mw);
    RenderMWin(mw);
    ShowWindow(mw->h, SW_SHOWNOACTIVATE);
    return mw;
}

static void CloseMWinFrom(size_t level) {   // закрыть окно этого уровня и все вложенные
    while (g_mstack.size() > level) {
        MWin* mw = g_mstack.back(); g_mstack.pop_back();
        if (mw->h) DestroyWindow(mw->h);
        delete mw;
    }
    if (level > 0 && level <= g_mstack.size()) { g_mstack[level - 1]->openSub = -1; RenderMWin(g_mstack[level - 1]); }
}

static void OpenSubOf(size_t level, int idx) {
    MWin* p = g_mstack[level];
    if (p->openSub == idx) return;
    CloseMWinFrom(level + 1);
    MItem& it = (*p->items)[idx];
    if (!it.hasSub) return;
    RECT pr = BodyRect(p);
    RECT row = p->rows[idx];
    UINT dpi = DpiOf(p->h);
    POINT a = {pr.right - (int)lround(MI(4, dpi)), pr.top + row.top - (int)lround(MI(3, dpi))};
    MWin* c = OpenMWin(GetAncestor(p->h, GA_ROOTOWNER), &it.sub, a, false, &pr);
    if (c) { p->openSub = idx; g_mstack.push_back(c); RenderMWin(p); }
}

static int RowAt(MWin* mw, POINT sp) {
    RECT wr = BodyRect(mw);
    if (!PtInRect(&wr, sp)) return -2;   // не в этом окне
    POINT cp = {sp.x - wr.left, sp.y - wr.top};
    for (size_t i = 0; i < mw->rows.size(); ++i) if (!(*mw->items)[i].sep && PtInRect(&mw->rows[i], cp)) return (int)i;
    return -1;
}

static int NextRow(MWin* mw, int from, int dir) {
    int n = (int)mw->items->size();
    for (int k = 1; k <= n; ++k) {
        int i = ((from < 0 ? (dir > 0 ? -1 : n) : from) + dir * k + n * 2) % n;
        if (!(*mw->items)[i].sep) return i;
    }
    return -1;
}

// показать меню и дождаться выбора; возвращает номер пункта (0 — ничего не выбрано)
static UINT TrackOurMenu(HWND owner, MList* root, POINT anchor, bool alignRight) {
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.style = CS_DBLCLKS; wc.lpfnWndProc = MenuProc; wc.hInstance = g_inst; wc.lpszClassName = MENU_CLASS;
        wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
        RegisterClassExW(&wc);
        registered = true;
    }
    MWin* r = OpenMWin(owner, root, anchor, alignRight, nullptr);
    if (!r) return 0;
    g_mstack.push_back(r);
    SetCapture(r->h);
    UINT result = 0;
    bool done = false;
    HWND fgStart = GetForegroundWindow();
    while (!done) {
        if (GetCapture() != r->h) SetCapture(r->h);
        DWORD wr = MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);
        if (wr == WAIT_TIMEOUT) { if (GetForegroundWindow() != fgStart) done = true; continue; }   // переключились на другое окно
        MSG msg;
        while (!done && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { PostQuitMessage((int)msg.wParam); done = true; break; }
            bool mouse = msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST;
            bool ncmouse = msg.message >= WM_NCMOUSEMOVE && msg.message <= WM_NCXBUTTONDBLCLK;
            if (mouse || ncmouse) {
                POINT sp = msg.pt;
                int lvl = -1, row = -1;
                for (int k = (int)g_mstack.size() - 1; k >= 0; --k) {
                    int rr = RowAt(g_mstack[k], sp);
                    if (rr != -2) { lvl = k; row = rr; break; }
                }
                if (msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE) {
                    if (lvl >= 0) {
                        MWin* mw = g_mstack[lvl];
                        if (mw->hover != row) { mw->hover = row; RenderMWin(mw); }
                        if (row >= 0 && (*mw->items)[row].hasSub) OpenSubOf(lvl, row);
                        else if (row >= 0 && mw->openSub >= 0 && mw->openSub != row) CloseMWinFrom(lvl + 1);
                    }
                } else if (msg.message == WM_LBUTTONUP) {
                    if (lvl >= 0 && row >= 0) {
                        MItem& it = (*g_mstack[lvl]->items)[row];
                        if (it.hasSub) OpenSubOf(lvl, row);
                        else { result = it.id; done = true; }
                    }
                } else if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN || msg.message == WM_MBUTTONDOWN ||
                           msg.message == WM_NCLBUTTONDOWN || msg.message == WM_NCRBUTTONDOWN) {
                    if (lvl < 0) done = true;   // щелчок мимо меню — закрываем
                }
                continue;
            }
            if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
                MWin* mw = g_mstack.back();
                switch (msg.wParam) {
                    case VK_ESCAPE: if (g_mstack.size() > 1) CloseMWinFrom(g_mstack.size() - 1); else done = true; break;
                    case VK_UP: case VK_DOWN: mw->hover = NextRow(mw, mw->hover, msg.wParam == VK_DOWN ? 1 : -1); RenderMWin(mw); break;
                    case VK_RIGHT: if (mw->hover >= 0 && (*mw->items)[mw->hover].hasSub) { OpenSubOf(g_mstack.size() - 1, mw->hover); MWin* c = g_mstack.back(); c->hover = NextRow(c, -1, 1); RenderMWin(c); } break;
                    case VK_LEFT: if (g_mstack.size() > 1) CloseMWinFrom(g_mstack.size() - 1); break;
                    case VK_RETURN: case VK_SPACE:
                        if (mw->hover >= 0) {
                            MItem& it = (*mw->items)[mw->hover];
                            if (it.hasSub) OpenSubOf(g_mstack.size() - 1, mw->hover); else { result = it.id; done = true; }
                        }
                        break;
                    case VK_MENU: case VK_F10: done = true; break;
                }
                continue;
            }
            if (msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP || msg.message == WM_CHAR || msg.message == WM_SYSCHAR) continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    ReleaseCapture();
    CloseMWinFrom(0);
    return result;
}

template <typename F>
static UINT RunMenu(HWND strip, const RECT& btn, F build, bool alignRight = false) {
    bool light = IsLight();
    UINT dpi = DpiOf(strip);
    int s = MulDiv(16, dpi, 96);
    MList root;
    build(&root, s, light);
    POINT pt = {alignRight ? btn.right : btn.left, btn.bottom + (int)lround(MI(4, dpi))};   // у Windows меню в 4 точках под кнопкой
    ClientToScreen(strip, &pt);
    UINT cmd = TrackOurMenu(strip, &root, pt, alignRight);
    FreeMList(root);
    return cmd;
}

static void BuildShowItems(MList* m, int s, bool light, const std::remove_pointer_t<LPSHELLSTATE>& ss, bool nav) {
    // группы как у Windows: «Область навигации», «Компактное представление» отдельно, остальное вместе
    AddItem(m, M_NAVPANE, TR(L"Область навигации", L"Navigation pane"), nav ? 1 : 0, 0xE8A0, s, light, {"windowsnavigationpane", "navigationpane"});
    AddItem(m, M_STATUSBAR, TR(L"Строка состояния", L"Status bar"), GetStatusBar() ? 1 : 0, 0xE9F9, s, light, {"windowsstatusbar", "statusbar"});
    AddSep(m);
    AddItem(m, M_COMPACT, TR(L"Компактное представление", L"Compact view"), GetCompact() ? 1 : 0, 0xE8FD, s, light, {"windowscompactmode"});
    AddSep(m);
    AddItem(m, M_CHECKBOXES, TR(L"Флажки элементов", L"Item check boxes"), ss.fAutoCheckSelect ? 1 : 0, 0xE73A, s, light, {"windowsselectioncheckboxes"});
    AddItem(m, M_EXTENSIONS, TR(L"Расширения имен файлов", L"File name extensions"), ss.fShowExtensions ? 1 : 0, 0xE8A5, s, light, {"windowsshowfileextensions"});
    AddItem(m, M_HIDDEN, TR(L"Скрытые элементы", L"Hidden items"), ss.fShowAllObjects ? 1 : 0, 0xE7B3, s, light, {"windowsshowhiddenfiles"});
}

// это те же настройки Проводника, что меняет его собственное меню
// область навигации (дерево папок слева): тот же переключатель Windows, что в её меню «Показать»; окно сразу перечитывает его
static void SetNavPane(HWND top, bool show) {
    const wchar_t* key = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Modules\\GlobalSettings\\Sizer";
    BYTE v[16] = {0x10, 0x01, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0x36, 0x07, 0, 0};   // ширина области, видна ли, …
    DWORD sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"PageSpaceControlSizer", RRF_RT_REG_BINARY, nullptr, v, &sz) != ERROR_SUCCESS || sz < 16) {
        BYTE d[16] = {0x10, 0x01, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0x36, 0x07, 0, 0};
        memcpy(v, d, 16);
    }
    v[4] = show ? 1 : 0;   // пятый байт — видна ли область
    RegSetKeyValueW(HKEY_CURRENT_USER, key, L"PageSpaceControlSizer", REG_BINARY, v, 16);
    if (!top) return;
    // окно перечитывает раскладку: сообщение «настройки изменились» и короткий переход в папку выше и обратно —
    // Проводник проверяет, показывать ли область навигации, при переходе (сам по себе этот переключатель он не перечитывает)
    SendMessageTimeoutW(top, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 200, nullptr);
    PostMessageW(top, WM_COMMAND, 28931, 0);
    if (HWND bar = (HWND)GetPropW(top, PROP_TOPBAR)) PostMessageW(bar, WM_APP + 34, 0, 0);
}
static void ApplyShowItem(UINT cmd, std::remove_pointer_t<LPSHELLSTATE>& ss, HWND top, bool nav) {
    switch (cmd) {
        case M_NAVPANE: SetNavPane(top, !nav); break;
        case M_STATUSBAR: {   // тот же переключатель, что «Показывать строку состояния» в параметрах папок
            DWORD v = GetStatusBar() ? 0 : 1;
            RegSetKeyValueW(HKEY_CURRENT_USER, ADV_KEY, L"ShowStatusBar", REG_DWORD, &v, sizeof(v));
            NotifyShell();
            if (top) if (HWND bar = (HWND)GetPropW(top, PROP_TOPBAR)) PostMessageW(bar, WM_APP + 34, 0, 0);   // окно перестраивается незаметно, как с областью навигации
            break;
        }
        case M_CHECKBOXES: ss.fAutoCheckSelect = !ss.fAutoCheckSelect; SHGetSetSettings(&ss, SSF_AUTOCHECKSELECT, TRUE); NotifyShell(); break;
        case M_EXTENSIONS: ss.fShowExtensions = !ss.fShowExtensions;   SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, TRUE); NotifyShell(); break;
        case M_HIDDEN:     ss.fShowAllObjects = !ss.fShowAllObjects;   SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, TRUE); NotifyShell(); break;
        case M_COMPACT: {
            DWORD v = GetCompact() ? 0 : 1;
            RegSetKeyValueW(HKEY_CURRENT_USER, ADV_KEY, L"UseCompactMode", REG_DWORD, &v, sizeof(v));
            NotifyShell();
            break;
        }
    }
}

static void ShowOptionsMenu(HWND strip, const RECT& btn) {
    std::remove_pointer_t<LPSHELLSTATE> ss = {};   // тот же тип, что ждёт функция
    SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS | SSF_SHOWEXTENSIONS | SSF_AUTOCHECKSELECT, FALSE);
    HWND top = GetAncestor(strip, GA_ROOT);
    bool nav = FindChild(top, L"SysTreeView32", true) != nullptr;   // дерево папок сейчас на экране
    UINT cmd = RunMenu(strip, btn, [&](MList* m, int s, bool light) { BuildShowItems(m, s, light, ss, nav); }, true);   // меню у правого края
    ApplyShowItem(cmd, ss, top, nav);
}

// ---------------- сортировка и вид — через саму открытую папку ----------------
static IFolderView2* GetFolderView2(HWND top) {
    IShellBrowser* sb = BrowserOf(top);
    if (!sb) return nullptr;
    IShellView* sv = nullptr;
    IFolderView2* fv = nullptr;
    if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        sv->QueryInterface(IID_PPV_ARGS(&fv));
        sv->Release();
    }
    return fv;
}

static PROPERTYKEY SysKey(DWORD pid) {   // основные свойства файлов (имя, тип, размер, дата)
    PROPERTYKEY k; k.fmtid = {0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}}; k.pid = pid; return k;
}
static bool SameKey(const PROPERTYKEY& a, const PROPERTYKEY& b) { return a.pid == b.pid && IsEqualGUID(a.fmtid, b.fmtid); }

enum { S_NAME = 20, S_DATE, S_TYPE, S_SIZE, S_ASC = 30, S_DESC };

static void ShowSortMenu(HWND strip, const RECT& btn) {
    HWND top = GetAncestor(strip, GA_ROOT);
    IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
    if (!fv) return;
    SORTCOLUMN cur = {SysKey(10), SORT_ASCENDING};
    fv->GetSortColumns(&cur, 1);
    struct Col { UINT id; const wchar_t* text; DWORD pid; } cols[] = {
        {S_NAME, TR(L"Имя", L"Name"), 10}, {S_DATE, TR(L"Дата изменения", L"Date modified"), 14}, {S_TYPE, TR(L"Тип", L"Type"), 4}, {S_SIZE, TR(L"Размер", L"Size"), 12}};
    UINT cmd = RunMenu(strip, btn, [&](MList* m, int s, bool light) {
        for (auto& c : cols) AddItem(m, c.id, c.text, SameKey(cur.propkey, SysKey(c.pid)) ? 2 : 0, 0, s, light, {});
        AddSep(m);
        AddItem(m, S_ASC, TR(L"По возрастанию", L"Ascending"), cur.direction == SORT_ASCENDING ? 2 : 0, 0, s, light, {});
        AddItem(m, S_DESC, TR(L"По убыванию", L"Descending"), cur.direction == SORT_DESCENDING ? 2 : 0, 0, s, light, {});
    });
    SORTCOLUMN nc = cur;
    for (auto& c : cols) if (cmd == c.id) nc.propkey = SysKey(c.pid);
    if (cmd == S_ASC) nc.direction = SORT_ASCENDING;
    if (cmd == S_DESC) nc.direction = SORT_DESCENDING;
    if (cmd) fv->SetSortColumns(&nc, 1);
    fv->Release();
}


// ---------------- «Создать»: список берём у самой Windows (тот же, что в меню «Создать» Проводника) ----------------
static IContextMenu3* g_shellCm3 = nullptr;   // пока открыто системное меню — сюда передаём его сообщения
static IContextMenu2* g_shellCm2 = nullptr;
static bool g_nativeMenuOpen = false;         // сообщения передаём Windows только когда открыто её собственное меню

static const CLSID CLSID_NewMenuObj = {0xD969A300, 0xE7FF, 0x11d0, {0xA9, 0x3B, 0x00, 0xA0, 0xC9, 0x0F, 0x27, 0x19}};

// значок пункта рисует сама Windows — снимаем его дважды, на чёрном и на белом, и получаем прозрачность
static HICON CaptureShellIcon(HMENU sub, const MENUITEMINFOW& mii, int is) {
    if (!mii.hbmpItem) return nullptr;
    HICON result = nullptr;
    std::vector<BYTE> shots[2];
    for (int pass = 0; pass < 2; ++pass) {
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = is; bi.bmiHeader.biHeight = -is;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HDC dc = CreateCompatibleDC(nullptr);
        HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp || !bits) { if (bmp) DeleteObject(bmp); DeleteDC(dc); return nullptr; }
        HGDIOBJ ob = SelectObject(dc, bmp);
        memset(bits, pass ? 0xFF : 0x00, (size_t)is * is * 4);
        RECT rc = {0, 0, is, is};
        if (mii.hbmpItem == HBMMENU_CALLBACK) {
            DRAWITEMSTRUCT di = {};
            di.CtlType = ODT_MENU; di.itemID = mii.wID; di.itemAction = ODA_DRAWENTIRE;
            di.hwndItem = (HWND)sub; di.hDC = dc; di.rcItem = rc; di.itemData = mii.dwItemData;
            LRESULT res = 0;
            if (g_shellCm3) g_shellCm3->HandleMenuMsg2(WM_DRAWITEM, 0, (LPARAM)&di, &res);
            else if (g_shellCm2) g_shellCm2->HandleMenuMsg(WM_DRAWITEM, 0, (LPARAM)&di);
        } else if ((ULONG_PTR)mii.hbmpItem > 11) {   // обычная картинка пункта
            BITMAP bm = {};
            if (GetObjectW(mii.hbmpItem, sizeof(bm), &bm)) {
                HDC src = CreateCompatibleDC(nullptr);
                HGDIOBJ os = SelectObject(src, mii.hbmpItem);
                BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                AlphaBlend(dc, 0, 0, is, is, src, 0, 0, bm.bmWidth, bm.bmHeight, bf);
                SelectObject(src, os); DeleteDC(src);
            }
        }
        GdiFlush();
        shots[pass].assign((BYTE*)bits, (BYTE*)bits + (size_t)is * is * 4);
        SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    }
    std::vector<BYTE> px((size_t)is * is * 4, 0);
    bool any = false;
    for (size_t i = 0; i < px.size(); i += 4) {
        int dw = (shots[1][i] - shots[0][i] + shots[1][i + 1] - shots[0][i + 1] + shots[1][i + 2] - shots[0][i + 2]) / 3;
        int a = 255 - dw; if (a < 0) a = 0; if (a > 255) a = 255;
        if (a) {
            any = true;
            for (int c = 0; c < 3; ++c) { int v = shots[0][i + c] * 255 / a; px[i + c] = (BYTE)(v > 255 ? 255 : v); }
        }
        px[i + 3] = (BYTE)a;
    }
    if (any) result = IconFromPx(px, is);
    return result;
}

static void AddItemIcon(MList* m, UINT id, const std::wstring& text, HICON icon) {
    MItem it; it.id = id; it.text = text; it.icon = icon;
    m->push_back(std::move(it));
}

static void ShowNewMenu(HWND strip, const RECT& btn) {
    HWND top = GetAncestor(strip, GA_ROOT);
    IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
    if (!fv) return;
    PIDLIST_ABSOLUTE pidl = nullptr;
    IPersistFolder2* pf = nullptr;
    if (SUCCEEDED(fv->GetFolder(IID_PPV_ARGS(&pf))) && pf) { pf->GetCurFolder(&pidl); pf->Release(); }
    IContextMenu* cm = nullptr;
    if (!pidl || FAILED(CoCreateInstance(CLSID_NewMenuObj, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&cm))) || !cm) {
        if (pidl) CoTaskMemFree(pidl);
        fv->Release(); return;
    }
    IShellExtInit* sei = nullptr;
    if (SUCCEEDED(cm->QueryInterface(IID_PPV_ARGS(&sei))) && sei) { sei->Initialize(pidl, nullptr, nullptr); sei->Release(); }
    IObjectWithSite* ows = nullptr;
    if (SUCCEEDED(cm->QueryInterface(IID_PPV_ARGS(&ows))) && ows) ows->SetSite(fv);   // чтобы новый файл сразу стал переименовываться
    cm->QueryInterface(IID_PPV_ARGS(&g_shellCm3));
    if (!g_shellCm3) cm->QueryInterface(IID_PPV_ARGS(&g_shellCm2));

    HMENU host = CreatePopupMenu();
    const UINT FIRST = 1;
    cm->QueryContextMenu(host, 0, FIRST, 0x7FFF, CMF_NORMAL);
    HMENU sub = nullptr;
    for (int i = 0; i < GetMenuItemCount(host) && !sub; ++i) sub = GetSubMenu(host, i);
    UINT cmd = 0;
    if (sub) {
        LRESULT res = 0;
        if (g_shellCm3) g_shellCm3->HandleMenuMsg2(WM_INITMENUPOPUP, (WPARAM)sub, 0, &res);   // Windows заполняет список
        else if (g_shellCm2) g_shellCm2->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)sub, 0);
        UINT dpi = DpiOf(strip);
        int is = MulDiv(16, dpi, 96);
        struct Row { UINT id; std::wstring text; bool sep; HICON icon; };
        std::vector<Row> rows;
        bool textOk = true;
        for (int i = 0; i < GetMenuItemCount(sub); ++i) {
            wchar_t buf[260] = {};
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_ID | MIIM_FTYPE | MIIM_BITMAP | MIIM_DATA | MIIM_STRING;
            mii.dwTypeData = buf; mii.cch = 259;
            if (!GetMenuItemInfoW(sub, i, TRUE, &mii)) continue;
            if (mii.fType & MFT_SEPARATOR) { rows.push_back({0, L"", true, nullptr}); continue; }
            std::wstring t;
            for (wchar_t* c = buf; *c; ++c) if (*c != L'&') t += *c;
            if (t.empty()) { textOk = false; break; }
            rows.push_back({mii.wID, t, false, CaptureShellIcon(sub, mii, is)});
        }
        if (textOk && !rows.empty()) {
            // наше меню в стиле Windows 11, пункты — от Windows
            g_menuCompact = true;
            cmd = RunMenu(strip, btn, [&](MList* m, int, bool) {
                for (auto& r : rows) {
                    if (r.sep) continue;   // у Windows в этом меню разделителей нет
                    AddItemIcon(m, r.id, r.text, r.icon); r.icon = nullptr;
                }
            });
            g_menuCompact = false;
        } else {
            // запасной путь: показать системное меню Windows как есть
            for (auto& r : rows) if (r.icon) DestroyIcon(r.icon);
            POINT pt = {btn.left, btn.bottom};
            ClientToScreen(strip, &pt);
            SetForegroundWindow(top);
            g_nativeMenuOpen = true;
            cmd = TrackPopupMenuEx(sub, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, strip, nullptr);
            g_nativeMenuOpen = false;
        }
        for (auto& r : rows) if (r.icon) DestroyIcon(r.icon);
    }
    if (cmd >= FIRST) {
        CMINVOKECOMMANDINFOEX ici = {sizeof(ici)};
        ici.fMask = CMIC_MASK_UNICODE;
        ici.hwnd = top;
        ici.lpVerb = MAKEINTRESOURCEA(cmd - FIRST);
        ici.lpVerbW = MAKEINTRESOURCEW(cmd - FIRST);
        ici.nShow = SW_SHOWNORMAL;
        cm->InvokeCommand((LPCMINVOKECOMMANDINFO)&ici);
    }
    if (g_shellCm3) { g_shellCm3->Release(); g_shellCm3 = nullptr; }
    if (g_shellCm2) { g_shellCm2->Release(); g_shellCm2 = nullptr; }
    if (ows) { ows->SetSite(nullptr); ows->Release(); }
    DestroyMenu(host);
    cm->Release();
    CoTaskMemFree(pidl);
    fv->Release();
}


enum { V_HUGE = 40, V_LARGE, V_MEDIUM, V_SMALL, V_LIST, V_DETAILS, V_TILES, V_CONTENT };


static void ShowViewMenu(HWND strip, const RECT& btn) {
    HWND top = GetAncestor(strip, GA_ROOT);
    IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
    if (!fv) return;
    FOLDERVIEWMODE mode = FVM_DETAILS; int size = 16;
    fv->GetViewModeAndIconSize(&mode, &size);
    UINT curId = V_DETAILS;
    switch (mode) {
        case FVM_ICON: curId = size >= 256 ? V_HUGE : size >= 96 ? V_LARGE : size >= 48 ? V_MEDIUM : V_SMALL; break;
        case FVM_SMALLICON: curId = V_SMALL; break;
        case FVM_LIST: curId = V_LIST; break;
        case FVM_TILE: curId = V_TILES; break;
        case FVM_CONTENT: curId = V_CONTENT; break;
        default: curId = V_DETAILS; break;
    }
    struct V { UINT id; const wchar_t* text; wchar_t glyph; std::vector<const char*> keys; } vs[] = {
        {V_HUGE, TR(L"Огромные значки", L"Extra large icons"), 0xE8A9, {"windowsiconsizeextralarge"}},
        {V_LARGE, TR(L"Крупные значки", L"Large icons"), 0xE8A9, {"windowsiconsizelarge"}},
        {V_MEDIUM, TR(L"Обычные значки", L"Medium icons"), 0xE8A9, {"windowsiconsizemedium"}},
        {V_SMALL, TR(L"Мелкие значки", L"Small icons"), 0xE8A9, {"windowsiconsizesmallicon"}},
        {V_LIST, TR(L"Список", L"List"), 0xE8FD, {"windowsiconsizelist"}},
        {V_DETAILS, TR(L"Таблица", L"Details"), 0xE8FD, {"windowsiconsizedetails"}},
        {V_TILES, TR(L"Плитка", L"Tiles"), 0xE8FD, {"windowsiconsizetile"}},
        {V_CONTENT, TR(L"Содержимое", L"Content"), 0xE8FD, {"windowsiconsizecontent"}},
    };
    UINT cmd = RunMenu(strip, btn, [&](MList* m, int s, bool light) {
        for (auto& v : vs) AddItem(m, v.id, v.text, v.id == curId ? 2 : 0, v.glyph, s, light, v.keys);
    });
    switch (cmd) {
        case V_HUGE:    fv->SetViewModeAndIconSize(FVM_ICON, 256); break;
        case V_LARGE:   fv->SetViewModeAndIconSize(FVM_ICON, 96); break;
        case V_MEDIUM:  fv->SetViewModeAndIconSize(FVM_ICON, 48); break;
        case V_SMALL:   fv->SetViewModeAndIconSize(FVM_SMALLICON, 16); break;
        case V_LIST:    fv->SetViewModeAndIconSize(FVM_LIST, 16); break;
        case V_DETAILS: fv->SetViewModeAndIconSize(FVM_DETAILS, 16); break;
        case V_TILES:   fv->SetViewModeAndIconSize(FVM_TILE, 48); break;
        case V_CONTENT: fv->SetViewModeAndIconSize(FVM_CONTENT, 32); break;
    }
    fv->Release();
}


// ---------------- подсказка при наведении (как у Windows 11) ----------------
static const wchar_t* TipText(Act a) {
    switch (a) {
        case A_OPTIONS: return TR(L"Параметры", L"Options");
        case A_VIEWMENU: return TR(L"Параметры представления", L"View options");
        case A_NEW:     return TR(L"Создать", L"New");
        case A_SORT:    return TR(L"Сортировать", L"Sort");
        case A_PROPS:   return TR(L"Свойства", L"Properties");
        case A_VIEW:    return TR(L"Просмотреть", L"View");
        case A_UNDO:    return TR(L"Отменить", L"Undo");
        case A_REDO:    return TR(L"Повторить", L"Redo");
        case A_CUT:     return TR(L"Вырезать", L"Cut");
        case A_COPY:    return TR(L"Копировать", L"Copy");
        case A_PASTE:   return TR(L"Вставить", L"Paste");
        case A_RENAME:  return TR(L"Переименовать", L"Rename");
        case A_DELETE:  return TR(L"Удалить", L"Delete");
        default:        return L"";
    }
}

static HFONT TipFont(UINT dpi) {
    return CreateFontW(-MulDiv(12, dpi, 96), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                       L"Segoe UI Variable Text");
}

LRESULT CALLBACK TipProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_ERASEBKGND) return 1;
    if (m == WM_PAINT) {
        PAINTSTRUCT ps; HDC dc = BeginPaint(h, &ps);
        RECT cr; GetClientRect(h, &cr);
        bool light = IsLight();
        HBRUSH b = CreateSolidBrush(light ? RGB(249, 249, 249) : RGB(46, 46, 46));   // снято со стоковой подсказки
        FillRect(dc, &cr, b); DeleteObject(b);
        wchar_t text[128] = {}; GetWindowTextW(h, text, 128);
        UINT dpi = DpiOf(h);
        HFONT f = TipFont(dpi); HGDIOBJ of = SelectObject(dc, f);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, light ? RGB(26, 26, 26) : RGB(255, 255, 255));
        DrawTextW(dc, text, -1, &cr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, of); DeleteObject(f);
        EndPaint(h, &ps);
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void HideTip(StripState* st) {
    if (st && st->tip && IsWindowVisible(st->tip)) ShowWindow(st->tip, SW_HIDE);
}

// подсказка у указателя: над кнопкой (below = false) или под ней; anchor — кнопка в координатах owner
static void ShowTipText(HWND owner, HWND& tip, const wchar_t* text, RECT anchor, bool below) {
    if (!text || !*text) return;
    UINT dpi = DpiOf(owner);
    if (!tip) {
        tip = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOPMOST, TIP_CLASS, L"",
                              WS_POPUP, 0, 0, 0, 0, owner, nullptr, g_inst, nullptr);
        if (!tip) return;
        DWORD corner = 3; DwmSetWindowAttribute(tip, 33, &corner, sizeof(corner));
    }
    BOOL dark = !IsLight();
    DwmSetWindowAttribute(tip, 20, &dark, sizeof(dark));
    COLORREF border = dark ? RGB(34, 33, 37) : RGB(229, 229, 229);
    DwmSetWindowAttribute(tip, 34, &border, sizeof(border));
    SetWindowTextW(tip, text);
    HDC dc = GetDC(owner);
    HFONT f = TipFont(dpi); HGDIOBJ of = SelectObject(dc, f);
    SIZE sz = {}; GetTextExtentPoint32W(dc, text, (int)wcslen(text), &sz);
    SelectObject(dc, of); DeleteObject(f); ReleaseDC(owner, dc);
    int w = sz.cx + MulDiv(22, dpi, 96), hgt = MulDiv(32, dpi, 96);
    MapWindowPoints(owner, nullptr, (POINT*)&anchor, 2);
    POINT cur; GetCursorPos(&cur);
    int x = cur.x - w / 2;
    int y = below ? anchor.bottom + MulDiv(8, dpi, 96) : anchor.top - hgt - MulDiv(12, dpi, 96);
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST), &mi)) {   // не за край экрана
        if (x + w > mi.rcWork.right) x = mi.rcWork.right - w;
        if (x < mi.rcWork.left) x = mi.rcWork.left;
        if (y < mi.rcWork.top) y = anchor.bottom + MulDiv(8, dpi, 96);
    }
    SetWindowPos(tip, HWND_TOPMOST, x, y, w, hgt, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(tip, nullptr, TRUE);
}

static void ShowTip(HWND strip, StripState* st, int idx) {   // подсказка кнопки панели — над кнопкой, у указателя (общая с верхом окна)
    RECT btn = {};
    for (auto& sl : Layout(strip)) if (sl.idx == idx) { btn = ActiveRect(idx, sl.rc, DpiOf(strip)); break; }
    ShowTipText(strip, st->tip, TipText(g_btns[idx].act), btn, false);
}

// ---------------- действия ----------------

static void SendKeys(WORD mod, WORD key) {
    INPUT in[4] = {}; int n = 0;
    if (mod) { in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = mod; ++n; }
    in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = key; ++n;
    in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = key; in[n].ki.dwFlags = KEYEVENTF_KEYUP; ++n;
    if (mod) { in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = mod; in[n].ki.dwFlags = KEYEVENTF_KEYUP; ++n; }
    SendInput(n, in, sizeof(INPUT));
}

static void DoAction(HWND strip, int idx) {
    SetTimer(strip, 3, 150, nullptr);   // после любого действия — пересчитать «Отменить / Повторить»
    Act a = g_btns[idx].act;
    if (a == A_OPTIONS) {   // значок — сразу «Параметры папок»
        ShellExecuteW(nullptr, L"open", L"rundll32.exe", L"shell32.dll,Options_RunDLL 0", nullptr, SW_SHOWNORMAL);
        return;
    }
    if (a == A_NEW) {
        for (auto& sl : Layout(strip)) if (sl.idx == idx) { ShowNewMenu(strip, sl.rc); break; }
        return;
    }
    if (a == A_VIEWMENU || a == A_SORT || a == A_VIEW) {   // меню открывается под кнопкой
        RECT at = {};
        for (auto& sl : Layout(strip)) if (sl.idx == idx) { at = sl.rc; break; }
        if (a == A_VIEWMENU) {   // вместе со значком шестерёнки — чтобы меню встало ровно под обеими
            for (auto& sl : Layout(strip)) if (g_btns[sl.idx].act == A_OPTIONS) at.right = sl.rc.right;
            ShowOptionsMenu(strip, at);
        }
        else if (a == A_SORT) ShowSortMenu(strip, at);
        else ShowViewMenu(strip, at);
        return;
    }
    HWND top = GetAncestor(strip, GA_ROOT);
    if (!top) return;
    if (HWND dv = FindChild(top, L"SHELLDLL_DefView", true)) {
        HWND items = FindWindowExW(dv, nullptr, L"DirectUIHWND", nullptr);
        SetFocus(items ? items : dv);
    }
    switch (a) {
        case A_UNDO:   SendKeys(VK_CONTROL, 'Z'); break;
        case A_PROPS:  SendKeys(VK_MENU, VK_RETURN); break;   // как Alt+Enter: свойства выделенного или самой папки
        case A_REDO:   SendKeys(VK_CONTROL, 'Y'); break;
        case A_CUT:    SendKeys(VK_CONTROL, 'X'); break;
        case A_COPY:   SendKeys(VK_CONTROL, 'C'); break;
        case A_PASTE:  SendKeys(VK_CONTROL, 'V'); break;
        case A_RENAME: SendKeys(0, VK_F2); break;
        case A_DELETE: SendKeys(0, VK_DELETE); break;
        default: break;
    }
}

LRESULT CALLBACK StripProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    StripState* st = (StripState*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
        case WM_NCCREATE:
            SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)new StripState());
            break;
        case WM_CREATE:
            AddClipboardFormatListener(h);   // Windows сообщит, когда в буфере обмена что-то поменяется
            PostMessageW(h, WM_APP + 41, 0, 0);   // подписаться на выделение в списке файлов
            if (st) {   // сообщения Windows о действиях с файлами — только от самого Проводника (не от всех программ), по всем папкам
                SHChangeNotifyEntry e = {nullptr, TRUE};
                st->notifyId = SHChangeNotifyRegister(h, SHCNRF_ShellLevel | SHCNRF_NewDelivery, SHCNE_ALLEVENTS, WM_APP + 20, 1, &e);
            }
            break;
        case WM_APP + 20: {   // что-то сделали с файлами — через миг (когда Windows запишет это в список отмены) проверить
            LONG ev = 0; PIDLIST_ABSOLUTE* pp = nullptr;
            if (HANDLE lk = SHChangeNotification_Lock((HANDLE)w, (DWORD)l, &pp, &ev)) SHChangeNotification_Unlock(lk);
            SetTimer(h, 3, 60, nullptr);
            return 0;
        }
        case WM_APP + 41:   // открылась папка (новый список файлов) — подписаться на его выделение
            if (st) {
                if (HookView(h, st->viewHook)) KillTimer(h, 1);
                else SetTimer(h, 1, 150, nullptr);   // не вышло — запасной путь: проверять само, как раньше
                COLORREF vb = ViewBg(GetAncestor(h, GA_ROOT));   // цвет фона этого списка
                if (vb != st->viewBg) { st->viewBg = vb; InvalidateRect(h, nullptr, FALSE); }
            }
            [[fallthrough]];
        case WM_APP + 42: case WM_CLIPBOARDUPDATE:   // выделение или буфер обмена изменились — пересчитать, какие кнопки активны
            if (st) {
                unsigned en = ComputeEnabled(h);
                if (en != st->enabled) { st->enabled = en; InvalidateRect(h, nullptr, FALSE); }
            }
            return 0;
        case WM_APP + 21:   // окно получило фокус — проверить сразу
            SetTimer(h, 3, 10, nullptr);
            return 0;
        case WM_TIMER:
            if (st && w == 3) {   // пересчёт «Отменить / Повторить»
                KillTimer(h, 3);
                if (GetForegroundWindow() == GetAncestor(h, GA_ROOT)) CheckUndoNow(h);   // спрашивает только окно в фокусе — остальные берут общий ответ
                unsigned en = ComputeEnabled(h);
                if (en != st->enabled) { st->enabled = en; InvalidateRect(h, nullptr, FALSE); }
                return 0;
            }
            if (st && w == 2) {
                KillTimer(h, 2);
                if (st->hover >= 0 && st->pressed < 0) ShowTip(h, st, st->hover);
                return 0;
            }
            if (st && w == 1 && IsWindowVisible(h) && !g_animBusy) {   // запасной путь, если подписка не удалась
                unsigned en = ComputeEnabled(h);
                if (en != st->enabled) { st->enabled = en; InvalidateRect(h, nullptr, FALSE); }
            }
            return 0;
        case WM_NCDESTROY:
            KillTimer(h, 1); KillTimer(h, 2); KillTimer(h, 3);
            if (st && st->memDC) { SelectObject(st->memDC, st->memOld); DeleteObject(st->memBmp); DeleteDC(st->memDC); }
            if (st && st->notifyId) SHChangeNotifyDeregister(st->notifyId);
            RemoveClipboardFormatListener(h);
            if (st) Unhook(st->viewHook);
            delete st; SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;
        case WM_MEASUREITEM: case WM_DRAWITEM: case WM_INITMENUPOPUP: case WM_MENUCHAR:   // запасное системное меню «Создать»
            if (!g_nativeMenuOpen) break;
            if (g_shellCm3) { LRESULT res = 0; if (SUCCEEDED(g_shellCm3->HandleMenuMsg2(m, w, l, &res))) return res; }
            else if (g_shellCm2) { if (SUCCEEDED(g_shellCm2->HandleMenuMsg(m, w, l))) return m == WM_MENUCHAR ? 0 : TRUE; }
            break;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: if (st) { PaintStrip(h, st); return 0; } break;
        case WM_APP + 9:   // кадр плавной подсветки — с частотой монитора
            if (st && st->animating) {
                double now = QpcSec();
                float dt = (float)(now - st->animLast); st->animLast = now;
                if (dt > 0.05f) dt = 0.05f;
                st->animT += dt / 0.12f;
                bool more = st->animT < 1.f;
                if (!more) { st->animT = 1.f; st->prevHover = -1; }
                RedrawWindow(h, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
                if (!more) st->animating = false;
                return more ? 1 : 0;
            }
            return 0;
        case WM_MOUSEMOVE: {
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int hit = HitTest(h, pt);
            if (hit >= 0 && !((st->enabled >> hit) & 1)) hit = -1;   // неактивные не подсвечиваем
            if (st->hover < 0 && hit >= 0) SetTimer(h, 3, 10, nullptr);   // навели на панель — освежить «Отменить / Повторить»
            if (hit != st->hover) {
                st->prevHover = st->hover; st->hover = hit; st->animT = g_cfg.anim ? 0.f : 1.f;
                if (g_cfg.anim && !st->animating) { st->animating = true; st->animLast = QpcSec(); StartFrames(h); }
                InvalidateRect(h, nullptr, FALSE);
                HideTip(st); KillTimer(h, 2);
                if (hit >= 0) SetTimer(h, 2, 600, nullptr);   // подсказка появляется после короткой паузы
            }
            TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0}; TrackMouseEvent(&t);
            return 0;
        }
        case WM_MOUSELEAVE:
            KillTimer(h, 2); HideTip(st);
            if (st && st->hover != -1) {
                st->prevHover = st->hover; st->hover = -1; st->animT = g_cfg.anim ? 0.f : 1.f;
                if (g_cfg.anim && !st->animating) { st->animating = true; st->animLast = QpcSec(); StartFrames(h); }
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_LBUTTONDOWN: {
            if (!st) break;
            KillTimer(h, 2); HideTip(st);
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            st->pressed = HitTest(h, pt);
            if (st->pressed >= 0 && !((st->enabled >> st->pressed) & 1)) st->pressed = -1;
            if (st->pressed >= 0) SetCapture(h);
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int hit = HitTest(h, pt), was = st->pressed;
            st->pressed = -1;
            if (GetCapture() == h) ReleaseCapture();
            InvalidateRect(h, nullptr, FALSE);
            if (was >= 0 && hit == was && ((st->enabled >> was) & 1)) DoAction(h, was);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}


// ======================= наш верх окна в виде Windows 11 (для быстрого вида) =======================
// размеры — из замеров Windows (в точках, от левого верхнего угла видимой рамки окна):
// кнопки окна 47⅓/46⅔/47⅓ × 29⅓ справа; «Назад/Вперёд/Вверх/Обновить» 32×32 с шагом 48 от 11⅓, сверху 48⅔;
// адресная строка от 201⅓, высота 32; поиск 166 × 32⅔ в 11⅓ от правого края; значки — знаки шрифта Segoe Fluent Icons размером 12
enum { T_NONE = -1, T_MIN = 1, T_MAX, T_CLOSE, T_BACK, T_FWD, T_UP, T_REFRESH, T_ROOTCHEV, T_ADDR, T_SEARCH, T_ADDTAB, T_TABL, T_TABR, T_DBG,
       T_CRUMB = 100, T_CRUMBCHEV = 200, T_TAB = 300, T_TABX = 400 };
// вкладка: своя папка и своя история «Назад/Вперёд» (как у QTTabBar: одно окно, при переключении — переход в папку вкладки)
struct Tab { PIDLIST_ABSOLUTE pidl = nullptr; std::wstring name; HICON icon = nullptr; std::vector<PIDLIST_ABSOLUTE> back, fwd; std::vector<PITEMID_CHILD> sel; float grow = 1.f; bool closing = false; bool pinned = false, preview = false; float slide = 0.f, slideFrom = 0.f, slideT = 1.f; float tw = 0.f; UINT twDpi = 0; };
struct Crumb { std::wstring name; PIDLIST_ABSOLUTE pidl = nullptr; float w = 0.f; UINT wDpi = 0; float wFs = 0.f; };
struct TopBarState {
    int hover = T_NONE, pressed = T_NONE;
    std::vector<Crumb> crumbs; PIDLIST_ABSOLUTE cur = nullptr; HICON rootIcon = nullptr;
    HWND edit = nullptr; int editKind = T_NONE; HFONT editFont = nullptr; HBRUSH editBrush = nullptr;
    bool canBack = true, canFwd = true;
    std::vector<Tab> tabs; int active = 0; bool wasZoomed = false;
    DWORD newTabUntil = 0;               // до этого времени следующий переход открывается в новой вкладке
    bool cpanel = false; int cpTicks = 0;
    int tabOff = 0, seenActive = -1; bool canL = false, canR = false;   // прокрутка вкладок: первая видимая, можно ли листать
    bool restored = false;                                               // запомненные вкладки уже проверены
    int prevHover = T_NONE; float animT = 1.f;                           // плавная подсветка
    std::wstring dbg;                                                    // что мод видит в окне (настройка «Отладка»)
    EvHook navHook; int startTries = 0;                                  // подписка на открытие папок
    DWORD quietUntil = 0, frozeAt = 0; PIDLIST_ABSOLUTE navBack = nullptr; bool locked = false;            // служебный переход «выше и обратно» — не в историю вкладки
    HDC memDC = nullptr; HBITMAP memBmp = nullptr; HGDIOBJ memOld = nullptr; void* memBits = nullptr; int memW = 0, memH = 0;   // готовая картинка верха
    ID2D1DCRenderTarget* rt = nullptr;                                   // «холст» для неё
    bool animating = false; double animLast = 0;                         // идут кадры анимации
    bool dragging = false; int downX = 0, downY = 0, grabX = 0, dragX = 0;
    bool winDrag = false, cloaked = false; POINT winOff = {}; HWND previewIn = nullptr;   // единственную вкладку тянут вместе с окном; где сейчас «примеряется»          // перетаскивание вкладки: где нажали, за какую точку взяли
    HWND tip = nullptr, ghost = nullptr;   // подсказка кнопки; «призрак» вкладки при вытаскивании
    bool dragOut = false;                                         // вкладку тянут за пределы полосы вкладок
    std::vector<PITEMID_CHILD> pendSel; PIDLIST_ABSOLUTE pendFor = nullptr; int pendTries = 0;   // выделение, которое вернуть после перехода   // панель управления: состояние и сколько раз подряд оно поменялось
    PIDLIST_ABSOLUTE expect = nullptr;   // куда мы сами переходим (переключение вкладки, «Назад/Вперёд») — это не новая запись истории
};
struct TEl { int id; RECT r; };
static bool XShown(TopBarState* st, HWND bar, int i) {   // виден ли крестик вкладки
    if (g_cfg.closeBtn == 2) return false;
    if (i >= 0 && i < (int)st->tabs.size() && st->tabs[i].pinned) return false;   // у закреплённой вместо крестика — булавка
    bool hov = st->hover == T_TAB + i || st->hover == T_TABX + i;
    if (g_cfg.dynamicTabs) return hov;   // узкие вкладки по названию — крестик только при наведении
    return g_cfg.closeBtn == 0 || i == st->active || hov;
}
static float TabEase(const Tab& t) {   // ширина при раскрытии/сжатии: быстро в начале, мягко в конце
    float g = t.grow < 0.f ? 0.f : t.grow > 1.f ? 1.f : t.grow;
    if (t.closing) return g * g * g;
    float r = 1.f - g; return 1.f - r * r * r;
}
static void TopKick(HWND bar, TopBarState* st) {   // запустить кадры анимации (с частотой монитора)
    if (!g_cfg.anim || st->animating) return;
    st->animating = true; st->animLast = QpcSec(); g_animBusy++;
    StartFrames(bar);
}


static PIDLIST_ABSOLUTE CurrentFolder(HWND top) {
    IFolderView2* fv = GetFolderView2(top);
    if (!fv) return nullptr;
    PIDLIST_ABSOLUTE pidl = nullptr;
    IPersistFolder2* pf = nullptr;
    if (SUCCEEDED(fv->GetFolder(IID_PPV_ARGS(&pf))) && pf) { pf->GetCurFolder(&pidl); pf->Release(); }
    fv->Release();
    return pidl;
}

// панель управления: сама папка и всё внутри неё (значки, страницы вроде «Программы и компоненты»)
static const IID IID_BrowserSvc = {0x02BA3B52, 0x0547, 0x11D1, {0xB8, 0x33, 0x00, 0xC0, 0x4F, 0xC9, 0xB3, 0x1F}};   // IBrowserService
static PIDLIST_ABSOLUTE BrowserPidl(HWND top) {   // что сейчас открыто в окне — даже если это не обычная папка
    IShellBrowser* sb = BrowserOf(top);
    if (!sb) return nullptr;
    IBrowserService* bs = nullptr; LPITEMIDLIST p = nullptr;
    if (SUCCEEDED(sb->QueryInterface(IID_BrowserSvc, (void**)&bs)) && bs) { bs->GetPidl(&p); bs->Release(); }
    return (PIDLIST_ABSOLUTE)p;
}
static bool UnderControlPanel(PCIDLIST_ABSOLUTE p) {   // по внутреннему адресу: панель управления, все её элементы и страницы
    if (!p) return false;
    PWSTR nm = nullptr; bool r = false;
    if (SUCCEEDED(SHGetNameFromIDList(p, SIGDN_DESKTOPABSOLUTEPARSING, &nm)) && nm) {
        std::wstring s = nm; CoTaskMemFree(nm);
        for (auto& ch : s) ch = towupper(ch);
        r = s.find(L"26EE0668-A00A-44D7-9371-BEB064C98683") != std::wstring::npos ||
            s.find(L"21EC2020-3AEA-1069-A2DD-08002B30309D") != std::wstring::npos ||
            s.find(L"5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0") != std::wstring::npos;
    }
    return r;
}
static std::wstring PidlText(PCIDLIST_ABSOLUTE p) {
    if (!p) return TR(L"нет", L"no");
    PWSTR nm = nullptr; std::wstring r = L"?";
    if (SUCCEEDED(SHGetNameFromIDList(p, SIGDN_DESKTOPABSOLUTEPARSING, &nm)) && nm) { r = nm; CoTaskMemFree(nm); }
    return r;
}
static std::wstring ChildDebug(HWND top) {   // отладка: что лежит прямо внутри окна Проводника (вид, видно ли, где по высоте)
    std::wstring r = TR(L" · части окна:", L" · window parts:");
    int n = 0;
    for (HWND c = GetWindow(top, GW_CHILD); c && n < 14; c = GetWindow(c, GW_HWNDNEXT), ++n) {
        wchar_t cls[48] = {}; GetClassNameW(c, cls, 48);
        RECT rc; GetWindowRect(c, &rc); MapWindowPoints(nullptr, top, (POINT*)&rc, 2);
        wchar_t b[100]; swprintf(b, 100, L" %ls%ls %d-%d", cls, IsWindowVisible(c) ? L"" : TR(L"(скрыто)", L"(hidden)"), (int)rc.top, (int)rc.bottom);
        r += b;
    }
    return r;
}
static std::wstring NavDebug(HWND top) {   // область навигации для отладки
    HWND tv = FindChild(top, L"SysTreeView32", true);
    if (!tv) return TR(L" · дерево слева: не найдено", L" · navigation tree: not found");
    wchar_t b[120];
    swprintf(b, 120, TR(L" · дерево слева: отступ %d, под присмотром мода %ls", L" · navigation tree: indent %d, handled by mod %ls"), (int)SendMessageW(tv, TVM_GETINDENT, 0, 0), GetPropW(tv, L"EIT_NavOld") ? TR(L"да", L"yes") : TR(L"нет", L"no"));
    return b;
}
static std::wstring DebugText(HWND top, PCIDLIST_ABSOLUTE cur, bool cpanel) {
    PIDLIST_ABSOLUTE bp = BrowserPidl(top);
    std::wstring a = PidlText(bp);
    if (bp) ILFree(bp);
    bool has = FindChild(ShellTabOf(top), L"SHELLDLL_DefView", false) != nullptr;
    HWND strip = (HWND)GetPropW(top, PROP_STRIP);
    return std::wstring(TR(L"панель управления: ", L"control panel: ")) + (cpanel ? TR(L"да", L"yes") : TR(L"нет", L"no")) + TR(L" · адрес окна: ", L" · window location: ") + a + TR(L" · папка списка: ", L" · view folder: ") + PidlText(cur) +
           TR(L" · список файлов: ", L" · file list: ") + (has ? TR(L"есть", L"present") : TR(L"нет", L"no")) + TR(L" · панель кнопок: ", L" · command bar: ") + (strip && IsWindowVisible(strip) ? TR(L"видна", L"visible") : TR(L"скрыта", L"hidden")) +
           [top] {
               StripState* ss = (StripState*)GetWindowLongPtrW((HWND)GetPropW(top, PROP_STRIP), GWLP_USERDATA);
               bool l = IsLight(); bool fromView = ss && ss->viewBg != CLR_INVALID;
               COLORREF c = fromView ? ss->viewBg : ListBg(l); wchar_t b[80];
               swprintf(b, 80, TR(L" · фон панели: %u,%u,%u (%ls)", L" · bar background: %u,%u,%u (%ls)"), GetRValue(c), GetGValue(c), GetBValue(c),
                        fromView ? TR(L"у списка файлов", L"from file list") : g_listBgTheme[l ? 1 : 0] ? TR(L"из темы", L"from theme") : TR(L"запасной", L"fallback"));
               return std::wstring(b); }() + NavDebug(top) + ChildDebug(top);
}
static bool ShowsControlPanel(HWND top, PCIDLIST_ABSOLUTE cur) {
    PIDLIST_ABSOLUTE bp = BrowserPidl(top);
    if (bp) { bool r = UnderControlPanel(bp); ILFree(bp); return r; }
    if (cur) return UnderControlPanel(cur);
    HWND st = ShellTabOf(top);   // адреса нет и обычного списка файлов нет — это панель управления или её страница
    if (!st) return false;
    return FindChild(st, L"SHELLDLL_DefView", false) == nullptr;
}

static void FreeCrumbs(TopBarState* st) {
    for (auto& c : st->crumbs) if (c.pidl) ILFree(c.pidl);
    st->crumbs.clear();
    if (st->rootIcon) { DestroyIcon(st->rootIcon); st->rootIcon = nullptr; }
}

static void BuildCrumbs(TopBarState* st, PIDLIST_ABSOLUTE pidl) {
    FreeCrumbs(st);
    if (!pidl) return;
    // цепочка от верхнего места (например, «Этот компьютер») до текущей папки; сам рабочий стол не показываем
    std::vector<PIDLIST_ABSOLUTE> chain;
    PIDLIST_ABSOLUTE p = ILClone(pidl);
    while (p && !ILIsEmpty(p)) { chain.push_back(ILClone(p)); if (!ILRemoveLastID(p)) break; }
    if (p) ILFree(p);
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        PWSTR nm = nullptr;
        Crumb c; c.pidl = *it;
        if (SUCCEEDED(SHGetNameFromIDList(*it, SIGDN_NORMALDISPLAY, &nm)) && nm) { c.name = nm; CoTaskMemFree(nm); }
        st->crumbs.push_back(c);
    }
    if (!st->crumbs.empty()) {
        SHFILEINFOW fi = {};
        PCIDLIST_ABSOLUTE ip = st->crumbs.front().pidl;
        PIDLIST_ABSOLUTE pc = nullptr;
        PWSTR fsp = nullptr;   // у текущей папки есть путь на диске («Загрузки», D:\Programs…) — значок «Этого компьютера», как у Windows
        bool onDisk = SUCCEEDED(SHGetNameFromIDList(st->crumbs.back().pidl, SIGDN_FILESYSPATH, &fsp)) && fsp;
        if (fsp) CoTaskMemFree(fsp);
        if (onDisk &&
            SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr, &pc)) && pc) ip = pc;   // «Загрузки» и т. п. — значок «Этого компьютера»
        if (SHGetFileInfoW((LPCWSTR)ip, 0, &fi, sizeof(fi), SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON)) st->rootIcon = fi.hIcon;
        if (pc) ILFree(pc);
    }
}


static std::wstring UrlPart(const std::wstring& t) {   // для строки поиска Windows
    std::wstring o;
    for (wchar_t c : t) {
        if (c < 128 && !iswalnum(c) && c != L'-' && c != L'_' && c != L'.' && c != L'~') { wchar_t b[8]; swprintf(b, 8, L"%%%02X", (unsigned)c); o += b; }
        else o += c;
    }
    return o;
}

// меню папок для стрелочки в адресной строке: вложенные папки выбранного места (или верхние места — для первой стрелочки)
static void ShowFolderMenu(HWND bar, TopBarState* st, const RECT& btn, PCIDLIST_ABSOLUTE parent) {
    IShellFolder* desk = nullptr;
    if (FAILED(SHGetDesktopFolder(&desk)) || !desk) return;
    IShellFolder* sf = nullptr;
    if (!parent || ILIsEmpty(parent)) { sf = desk; sf->AddRef(); }
    else desk->BindToObject(parent, nullptr, IID_PPV_ARGS(&sf));
    desk->Release();
    if (!sf) return;
    std::vector<PIDLIST_ABSOLUTE> items;
    IEnumIDList* en = nullptr;
    HWND top = GetAncestor(bar, GA_ROOT);
    if (SUCCEEDED(sf->EnumObjects(top, SHCONTF_FOLDERS, &en)) && en) {
        PITEMID_CHILD c = nullptr;
        while (en->Next(1, &c, nullptr) == S_OK && items.size() < 400) {
            items.push_back(parent && !ILIsEmpty(parent) ? ILCombine(parent, c) : (PIDLIST_ABSOLUTE)ILClone(c));
            CoTaskMemFree(c);
        }
        en->Release();
    }
    sf->Release();
    items.erase(std::remove(items.begin(), items.end(), nullptr), items.end());
    // сколько строк влезает по высоте экрана (у меню нет прокрутки)
    UINT dpi = DpiOf(bar);
    MONITORINFO mi = {sizeof(mi)}; GetMonitorInfoW(MonitorFromWindow(bar, MONITOR_DEFAULTTONEAREST), &mi);
    size_t maxRows = (size_t)((mi.rcWork.bottom - mi.rcWork.top) * 96.0 / dpi / 31.333) - 2;
    if (items.size() > maxRows) items.resize(maxRows);
    g_menuCompact = true;   // без пустого места под галочки слева от значков
    UINT cmd = RunMenu(bar, btn, [&](MList* m, int s, bool) {
        for (size_t i = 0; i < items.size(); ++i) {
            SHFILEINFOW fi = {};
            SHGetFileInfoW((LPCWSTR)items[i], 0, &fi, sizeof(fi), SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME);
            AddItemIcon(m, 5000 + (UINT)i, fi.szDisplayName, fi.hIcon);
            (void)s;
        }
    });
    g_menuCompact = false;
    if (cmd >= 5000 && cmd - 5000 < items.size()) {
        IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
        if (sb) sb->BrowseObject(items[cmd - 5000], SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
    }
    for (auto p2 : items) ILFree(p2);
    (void)st;
}


// ---------- вкладки ----------
static void TabSetFolder(Tab& t, PCIDLIST_ABSOLUTE pidl) {
    if (t.pidl) ILFree(t.pidl);
    t.pidl = ILCloneFull(pidl);
    PWSTR nm = nullptr;
    t.name.clear(); t.twDpi = 0;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &nm)) && nm) { t.name = nm; CoTaskMemFree(nm); }
    if (t.icon) DestroyIcon(t.icon);
    SHFILEINFOW fi = {};
    t.icon = SHGetFileInfoW((LPCWSTR)pidl, 0, &fi, sizeof(fi), SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON) ? fi.hIcon : nullptr;
}
static void TabFree(Tab& t) {
    if (t.pidl) ILFree(t.pidl);
    if (t.icon) DestroyIcon(t.icon);
    for (auto p : t.back) ILFree(p);
    for (auto p : t.fwd) ILFree(p);
    for (auto p : t.sel) ILFree(p);
    t = Tab();
}
static void GoTo(HWND bar, TopBarState* st, PCIDLIST_ABSOLUTE pidl) {   // переход без записи в историю Проводника
    HWND top = GetAncestor(bar, GA_ROOT);
    IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
    if (!sb || !pidl) return;
    if (st->expect) ILFree(st->expect);
    st->expect = ILCloneFull(pidl);
    sb->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER | SBSP_WRITENOHISTORY);
}
static PIDLIST_ABSOLUTE HomeFolder() {   // новая вкладка — по настройке: «Этот компьютер», «Загрузки» или своя папка
    PIDLIST_ABSOLUTE p = nullptr;
    if (g_cfg.newTab == 1) SHGetKnownFolderIDList(FOLDERID_Downloads, 0, nullptr, &p);
    else if (g_cfg.newTab == 2 && !g_cfg.newTabPath.empty()) SHParseDisplayName(g_cfg.newTabPath.c_str(), nullptr, &p, 0, nullptr);
    if (!p) SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr, &p);   // путь не найден — «Этот компьютер»
    return p;
}
// выделенные файлы вкладки: запомнить при уходе, вернуть при возвращении
static void FreeChildren(std::vector<PITEMID_CHILD>& v) { for (auto p : v) ILFree(p); v.clear(); }
static void SaveSel(HWND bar, Tab& t) {
    FreeChildren(t.sel);
    HWND top = GetAncestor(bar, GA_ROOT);
    IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
    if (!fv) return;
    IShellItemArray* a = nullptr;
    if (SUCCEEDED(fv->GetSelection(FALSE, &a)) && a) {
        DWORD n = 0; a->GetCount(&n);
        for (DWORD k = 0; k < n && k < 1000; ++k) {
            IShellItem* si = nullptr;
            if (FAILED(a->GetItemAt(k, &si)) || !si) continue;
            PIDLIST_ABSOLUTE p = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(si, &p)) && p) { t.sel.push_back(ILClone(ILFindLastID(p))); ILFree(p); }
            si->Release();
        }
        a->Release();
    }
    fv->Release();
}
static void ApplyPendingSel(HWND bar, TopBarState* st) {   // из таймера: папка открылась и файлы уже в списке — выделяем
    if (st->pendSel.empty()) return;
    auto drop = [&] { FreeChildren(st->pendSel); if (st->pendFor) { ILFree(st->pendFor); st->pendFor = nullptr; } };
    if (++st->pendTries > 30) { drop(); return; }   // 3 секунды не вышло — забываем
    if (st->pendTries < 2 || !st->cur || !st->pendFor || !ILIsEqual(st->cur, st->pendFor)) return;
    HWND top = GetAncestor(bar, GA_ROOT);
    IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
    if (!fv) return;
    int cnt = 0; fv->ItemCount(SVGIO_ALLVIEW, &cnt);
    IShellView* sv = nullptr;
    if (cnt > 0 && SUCCEEDED(fv->QueryInterface(IID_PPV_ARGS(&sv))) && sv) {
        bool first = true, any = false;
        for (auto c : st->pendSel) {
            UINT fl = first ? (SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED) : SVSI_SELECT;
            if (SUCCEEDED(sv->SelectItem(c, fl))) { first = false; any = true; }
        }
        sv->Release();
        if (any) drop();
    }
    fv->Release();
}

static void SwitchTab(HWND bar, TopBarState* st, int i) {
    if (i < 0 || i >= (int)st->tabs.size() || i == st->active || st->tabs[i].closing) return;
    if (st->active >= 0 && st->active < (int)st->tabs.size()) SaveSel(bar, st->tabs[st->active]);
    st->active = i;
    FreeChildren(st->pendSel); if (st->pendFor) ILFree(st->pendFor);
    st->pendSel.swap(st->tabs[i].sel); st->pendFor = ILCloneFull(st->tabs[i].pidl); st->pendTries = 0;
    if (!st->pendSel.empty()) SetTimer(bar, 12, 100, nullptr);   // выделение вернём, как только файлы появятся в списке
    GoTo(bar, st, st->tabs[i].pidl);
    InvalidateRect(bar, nullptr, FALSE);
}
static void AddTab(HWND bar, TopBarState* st, PCIDLIST_ABSOLUTE where) {
    PIDLIST_ABSOLUTE home = where ? nullptr : HomeFolder();
    PCIDLIST_ABSOLUTE p = where ? where : home;
    if (!p) return;
    Tab t; TabSetFolder(t, p);
    if (g_cfg.anim) t.grow = 0.f;   // вкладка плавно раскрывается
    if (!st->tabs.empty()) SaveSel(bar, st->tabs[st->active]);
    if (where) { st->tabs.insert(st->tabs.begin() + st->active + 1, t); st->active++; }   // папка в новой вкладке — рядом с текущей
    else { st->tabs.push_back(t); st->active = (int)st->tabs.size() - 1; }            // плюс — в конец
    GoTo(bar, st, p);
    if (home) ILFree(home);
    TopKick(bar, st);
    InvalidateRect(bar, nullptr, FALSE);
}
static int LiveTabs(TopBarState* st) { int n = 0; for (auto& t : st->tabs) if (!t.closing) n++; return n; }
static void EraseTab(TopBarState* st, int i) {
    TabFree(st->tabs[i]);
    st->tabs.erase(st->tabs.begin() + i);
    if (i < st->active) st->active--;
    if (st->active >= (int)st->tabs.size()) st->active = (int)st->tabs.size() - 1;
}
static void AddNewTab(HWND bar, TopBarState* st) {   // «+» и Ctrl+T
    AddTab(bar, st, nullptr);
}
static void SavePinned(TopBarState* st, bool evenIfNone);
static void CloseTab(HWND bar, TopBarState* st, int i) {
    if (i < 0 || i >= (int)st->tabs.size() || st->tabs[i].closing) return;
    HWND top = GetAncestor(bar, GA_ROOT);
    if (LiveTabs(st) == 1) { if (top) PostMessageW(top, WM_CLOSE, 0, 0); return; }   // последняя — закрываем окно, как Windows 11
    if (i == st->active) {   // выбранной становится соседняя: справа, иначе слева
        int nb = -1;
        for (int k = i + 1; k < (int)st->tabs.size() && nb < 0; ++k) if (!st->tabs[k].closing) nb = k;
        for (int k = i - 1; k >= 0 && nb < 0; --k) if (!st->tabs[k].closing) nb = k;
        st->active = nb;
        GoTo(bar, st, st->tabs[nb].pidl);
    }
    bool wasPinned = st->tabs[i].pinned;
    if (g_cfg.anim) { st->tabs[i].closing = true; TopKick(bar, st); }   // плавно сжимается, потом убирается
    else EraseTab(st, i);
    if (wasPinned) SavePinned(st, true);
    InvalidateRect(bar, nullptr, FALSE);
}
static struct { POINT pt; DWORD until; } g_drop = {};   // куда поставить окно, открытое из вытащенной вкладки
static const wchar_t* PROP_DROP = L"EIT_Drop";
static const wchar_t* PROP_FRESH = L"EIT_Fresh";   // окно только что открыто (не было до включения мода) — ему можно вернуть запомненные вкладки

// запомненные вкладки: «номер выбранной», затем по строке на вкладку (внутренний адрес папки); хранит Windhawk
static void SaveSession(TopBarState* st) {
    if (!g_cfg.remember || !g_cfg.tabs || st->tabs.empty()) return;
    std::wstring s = std::to_wstring(st->active);
    for (auto& t : st->tabs) {
        PWSTR nm = nullptr;
        if (!t.closing && !t.preview && t.pidl && SUCCEEDED(SHGetNameFromIDList(t.pidl, SIGDN_DESKTOPABSOLUTEPARSING, &nm)) && nm) { s += L"\n"; s += nm; CoTaskMemFree(nm); }
    }
    Wh_SetStringValue(L"session", s.c_str());
}
static void RestoreSession(HWND bar, TopBarState* st) {
    if (!g_cfg.remember || !g_cfg.tabs || st->tabs.size() != 1 || !st->cur) return;
    std::vector<wchar_t> buf(65536);
    Wh_GetStringValue(L"session", buf.data(), buf.size());
    std::wstring s = buf.data();
    if (s.empty()) return;
    Wh_SetStringValue(L"session", L"");   // одно окно забирает их себе
    std::vector<Tab> tabs;
    int act = _wtoi(s.c_str());
    size_t p = s.find(L'\n');
    while (p != std::wstring::npos) {
        size_t q = s.find(L'\n', p + 1);
        std::wstring nm = s.substr(p + 1, q == std::wstring::npos ? std::wstring::npos : q - p - 1);
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (!nm.empty() && SUCCEEDED(SHParseDisplayName(nm.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) { Tab t; TabSetFolder(t, pidl); tabs.push_back(t); ILFree(pidl); }
        p = q;
    }
    if (tabs.empty()) return;
    if (act < 0 || act >= (int)tabs.size()) act = 0;
    PIDLIST_ABSOLUTE home = HomeFolder();
    bool isHome = home && ILIsEqual(home, st->cur);
    if (home) ILFree(home);
    int same = -1;
    for (size_t i = 0; i < tabs.size(); ++i) if (ILIsEqual(tabs[i].pidl, st->cur)) same = (int)i;
    if (same >= 0) act = same;                                                         // открыли папку, что уже среди запомненных
    else if (!isHome) { Tab t; TabSetFolder(t, st->cur); tabs.push_back(t); act = (int)tabs.size() - 1; }   // открыли конкретную папку — она последней и выбрана
    for (auto& t : st->tabs) TabFree(t);
    st->tabs = tabs; st->active = act;
    if (!ILIsEqual(st->tabs[act].pidl, st->cur)) GoTo(bar, st, st->tabs[act].pidl);
    InvalidateRect(bar, nullptr, FALSE);
}
static void DetachTab(HWND bar, TopBarState* st, int i, POINT sp) {   // вкладка → новое окно Проводника там, где её отпустили
    if (i < 0 || i >= (int)st->tabs.size() || LiveTabs(st) < 2 || st->tabs[i].closing) return;
    HWND top = GetAncestor(bar, GA_ROOT);
    IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
    PIDLIST_ABSOLUTE p = ILCloneFull(st->tabs[i].pidl);
    if (!p) return;
    g_drop.pt = sp; g_drop.until = GetTickCount() + 5000;
    bool ok = sb && SUCCEEDED(sb->BrowseObject(p, SBSP_ABSOLUTE | SBSP_NEWBROWSER));
    if (!ok) {   // запасной путь — открыть папку, как двойным щелчком
        SHELLEXECUTEINFOW ei = {sizeof(ei)}; ei.fMask = SEE_MASK_IDLIST; ei.lpIDList = p; ei.nShow = SW_SHOWNORMAL; ei.hwnd = top;
        ok = ShellExecuteExW(&ei) != FALSE;
    }
    ILFree(p);
    if (ok) CloseTab(bar, st, i); else g_drop.until = 0;
}

// перенос вкладки в другое окно (как в Chrome): её отпустили над верхом другого окна Проводника
struct TabDrop { const Tab* src; POINT sp; };
struct TargetFind { HWND self; POINT sp; HWND bar; bool done; };
static BOOL CALLBACK TargetFindCb(HWND w, LPARAM lp) {   // окна — сверху вниз; решает самое верхнее под указателем
    TargetFind* f = (TargetFind*)lp;
    if (w == f->self || !IsWindowVisible(w) || IsIconic(w)) return TRUE;
    BOOL cl = FALSE; DwmGetWindowAttribute(w, 14 /*DWMWA_CLOAKED*/, &cl, sizeof(cl));
    if (cl) return TRUE;
    wchar_t cls[64] = {}; GetClassNameW(w, cls, 64);
    if (wcscmp(cls, TIP_CLASS) == 0) return TRUE;   // наш «призрак» вкладки и подсказки
    RECT r; GetWindowRect(w, &r);
    if (!PtInRect(&r, f->sp)) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(w, &pid);
    if (pid == GetCurrentProcessId() && wcscmp(cls, L"CabinetWClass") == 0 && TabsOn(w)) {
        HWND tb = (HWND)GetPropW(w, PROP_TOPBAR);
        RECT br = {};
        if (tb && IsWindowVisible(tb)) GetWindowRect(tb, &br);
        if (PtInRect(&br, f->sp)) f->bar = tb;   // над верхом (вкладки, адресная строка) другого окна Проводника
    }
    f->done = true;
    return FALSE;
}
static HWND FindTargetBar(HWND self, POINT sp) {
    TargetFind f = {self, sp, nullptr, false};
    EnumWindows(TargetFindCb, (LPARAM)&f);
    return f.bar;
}
static void SetCloak(HWND top, TopBarState* st, bool on) {   // окно «растворяется», пока его вкладка примеряется к другому окну
    if (st->cloaked == on) return;
    st->cloaked = on;
    BOOL v = on; DwmSetWindowAttribute(top, 13 /*DWMWA_CLOAK*/, &v, sizeof(v));
}
static void UpdatePreview(HWND bar, TopBarState* st, HWND tb, POINT sp) {   // показать / убрать вкладку-«примерку» в другом окне
    if (st->previewIn && st->previewIn != tb) { SendMessageTimeoutW(st->previewIn, WM_APP + 32, 0, 0, SMTO_NORMAL, 500, nullptr); st->previewIn = nullptr; }
    int i = st->pressed - T_TAB;
    if (!tb || i < 0 || i >= (int)st->tabs.size()) return;
    TabDrop d = {&st->tabs[i], sp};
    SendMessageTimeoutW(tb, WM_APP + 31, 0, (LPARAM)&d, SMTO_NORMAL, 500, nullptr);
    st->previewIn = tb;
    (void)bar;
}
static bool DropTabToBar(HWND bar, TopBarState* st, int i, HWND tb, POINT sp) {
    HWND top = GetAncestor(bar, GA_ROOT);
    TabDrop d = {&st->tabs[i], sp};
    DWORD_PTR ok = 0;
    if (!SendMessageTimeoutW(tb, WM_APP + 30, 0, (LPARAM)&d, SMTO_NORMAL, 2000, &ok) || !ok) return false;
    st->previewIn = nullptr;   // «примерка» стала настоящей вкладкой
    if (LiveTabs(st) > 1) CloseTab(bar, st, i);
    else PostMessageW(top, WM_CLOSE, 0, 0);   // это была единственная вкладка — окно больше не нужно
    return true;
}

// закреплённые вкладки: номер места и внутренний адрес папки, по строке на вкладку; хранит Windhawk
static void SavePinned(TopBarState* st, bool evenIfNone) {
    std::wstring s;
    int k = 0;
    for (size_t i = 0; i < st->tabs.size(); ++i) {
        const Tab& t = st->tabs[i];
        if (t.closing || t.preview || !t.pinned || !t.pidl) continue;
        PWSTR nm = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(t.pidl, SIGDN_DESKTOPABSOLUTEPARSING, &nm)) && nm) { s += std::to_wstring(i) + L"\t" + nm + L"\n"; CoTaskMemFree(nm); ++k; }
    }
    if (k || evenIfNone) Wh_SetStringValue(L"pinned", s.c_str());   // окно без закреплённых при закрытии список не стирает
}
static void RestorePinned(HWND bar, TopBarState* st) {   // новое окно: закреплённые вкладки — на свои места
    std::vector<wchar_t> buf(65536);
    Wh_GetStringValue(L"pinned", buf.data(), buf.size());
    std::wstring s = buf.data();
    if (s.empty() || st->tabs.empty()) return;
    PIDLIST_ABSOLUTE activePidl = st->tabs[st->active].pidl;   // выбранная остаётся выбранной (узнаём её по адресу)
    size_t p = 0;
    while (p < s.size()) {
        size_t e = s.find(L'\n', p); if (e == std::wstring::npos) e = s.size();
        std::wstring line = s.substr(p, e - p); p = e + 1;
        size_t tab = line.find(L'\t'); if (tab == std::wstring::npos) continue;
        int idx = _wtoi(line.c_str());
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (FAILED(SHParseDisplayName(line.c_str() + tab + 1, nullptr, &pidl, 0, nullptr)) || !pidl) continue;
        int at = idx < 0 ? 0 : idx > (int)st->tabs.size() ? (int)st->tabs.size() : idx;
        int same = -1;
        for (size_t i = 0; i < st->tabs.size(); ++i) if (!st->tabs[i].pinned && ILIsEqual(st->tabs[i].pidl, pidl)) { same = (int)i; break; }
        if (same >= 0) {   // такая папка уже открыта — её и закрепляем, на своё место
            Tab t = st->tabs[same]; t.pinned = true;
            st->tabs.erase(st->tabs.begin() + same);
            if (at > (int)st->tabs.size()) at = (int)st->tabs.size();
            st->tabs.insert(st->tabs.begin() + at, t);
        } else { Tab t; TabSetFolder(t, pidl); t.pinned = true; st->tabs.insert(st->tabs.begin() + at, t); }
        ILFree(pidl);
    }
    for (size_t i = 0; i < st->tabs.size(); ++i) if (st->tabs[i].pidl == activePidl) st->active = (int)i;
    InvalidateRect(bar, nullptr, FALSE);
}

static void TabBack(HWND bar, TopBarState* st, bool back) {   // «Назад/Вперёд» — по истории этой вкладки
    if (st->tabs.empty()) return;
    Tab& t = st->tabs[st->active];
    auto& from = back ? t.back : t.fwd;
    auto& to = back ? t.fwd : t.back;
    if (from.empty()) return;
    PIDLIST_ABSOLUTE target = from.back(); from.pop_back();
    if (t.pidl) to.push_back(ILCloneFull(t.pidl));
    GoTo(bar, st, target);
    ILFree(target);
}
// Проводник сменил папку: если это не наш переход — новая запись истории этой вкладки
static void OnFolderChanged(TopBarState* st, PCIDLIST_ABSOLUTE now) {
    if (!now) return;
    if (st->quietUntil && GetTickCount() < st->quietUntil) return;   // служебный переход «выше и обратно» — вкладка и её история не меняются
    if (st->tabs.empty()) { Tab t; TabSetFolder(t, now); st->tabs.push_back(t); st->active = 0; return; }
    if (st->newTabUntil && GetTickCount() < st->newTabUntil) {   // «открыть в новой вкладке»: старая вкладка остаётся как была
        st->newTabUntil = 0;
        Tab nt; TabSetFolder(nt, now);
        st->tabs.insert(st->tabs.begin() + st->active + 1, nt);
        st->active++;
        return;
    }
    st->newTabUntil = 0;
    Tab& t = st->tabs[st->active];
    bool ours = st->expect && ILIsEqual(st->expect, now);
    if (st->expect) { ILFree(st->expect); st->expect = nullptr; }
    if (!ours && t.pidl && !ILIsEqual(t.pidl, now)) {
        t.back.push_back(ILCloneFull(t.pidl));
        if (t.back.size() > 100) { ILFree(t.back.front()); t.back.erase(t.back.begin()); }
        for (auto p : t.fwd) ILFree(p);
        t.fwd.clear();
    }
    TabSetFolder(t, now);
}

static std::vector<TEl> TopLayout(HWND bar, TopBarState* st, bool natural = false) {   // natural — без сдвигов перетаскивания
    RECT cr; GetClientRect(bar, &cr);
    UINT dpi = DpiOf(bar);
    double W = cr.right * 96.0 / dpi;
    std::vector<TEl> v;
    auto R = [&](double x, double y, double w, double h) { return RECT{P(x, dpi), P(y, dpi), P(x + w, dpi), P(y + h, dpi)}; };
    HWND ltop = GetAncestor(bar, GA_ROOT);
    double ny = -NavShiftFor(ltop);
    double FH = g_cfg.fieldH, fy = NavC() - FH / 2, ch = FH - 8 < 24 ? FH - 8 : 24, cy = NavC() - ch / 2;   // поле по центру строки навигации; крошки — не выше 24   // строка навигации выше, когда вкладок нет
    // вкладки: как у Windows 11 — высота 32 (сверху 8), ширина поровну, не больше 248; плюс — в 3⅓ правее, 32×24
    if (TabsOn(ltop)) {
        size_t n = st->tabs.size();
        double avail = W - 4 - 141.333 - 35.333 - 40;   // до кнопок окна, с местом под плюс и за что тянуть окно
        std::vector<double> ws(n, (double)g_cfg.tabWidth);
        if (g_cfg.dynamicTabs)   // по длине названия: значок и отступы 38 слева, крестик 44 справа
            for (size_t i = 0; i < n; ++i) {
                Tab& tb = st->tabs[i];
                if (tb.twDpi != dpi) { tb.tw = (float)(TextWidthDWSize(tb.name.c_str(), dpi, 12.f) * 96.0 / dpi); tb.twDpi = dpi; }   // замер — один раз
                double t = tb.tw * 1.06 + (g_cfg.tabIcons ? 38 : 12) + 12;   // без места под крестик: он появляется при наведении
                ws[i] = t < 64 ? 64 : t > g_cfg.tabWidth ? g_cfg.tabWidth : t;
            }
        for (size_t i = 0; i < n; ++i) ws[i] *= TabEase(st->tabs[i]);
        double sum = 0; for (double w : ws) sum += w;
        bool scroll = false;
        double minW = g_cfg.tabScroll ? 120 : 72;   // с прокруткой сужаем меньше — дальше листаем стрелками
        if (sum > avail && sum > 0) {   // не влезают — сужаем все поровну
            double k = avail / sum; sum = 0;
            for (size_t i = 0; i < n; ++i) { ws[i] *= k; if (ws[i] < minW * TabEase(st->tabs[i])) ws[i] = minW * TabEase(st->tabs[i]); sum += ws[i]; }
            scroll = g_cfg.tabScroll && sum > avail + 0.5;
        }
        double x = 4;
        size_t from = 0, to = n;
        st->canL = st->canR = false;
        if (scroll) {
            const double AW = 28;
            double room = avail - 2 * (AW + 2);
            auto endFrom = [&](size_t off) { double s2 = 0; size_t k = off; while (k < n && (k == off || s2 + ws[k] <= room)) s2 += ws[k++]; return k; };
            if (st->tabOff < 0) st->tabOff = 0;
            if (st->tabOff >= (int)n) st->tabOff = (int)n - 1;
            if (st->active != st->seenActive) {   // выбранную вкладку всегда видно
                st->seenActive = st->active;
                if (st->tabOff > st->active) st->tabOff = st->active;
                while (st->tabOff < st->active && (int)endFrom(st->tabOff) <= st->active) st->tabOff++;
            }
            while (st->tabOff > 0 && endFrom(st->tabOff - 1) == n) st->tabOff--;   // без пустоты в конце
            from = st->tabOff; to = endFrom(from);
            st->canL = from > 0; st->canR = to < n;
            v.push_back({T_TABL, R(x, 12, AW, 24)}); x += AW + 2;
        } else st->tabOff = 0;
        for (size_t i = from; i < to; ++i) {
            v.push_back({T_TAB + (int)i, R(x, 8, ws[i], 32)});
            if (g_cfg.closeBtn != 2 && st->tabs[i].grow >= 1.f && !st->tabs[i].closing) v.push_back({T_TABX + (int)i, R(x + ws[i] - 40, 12, 32, 24)});   // крестик — по настройке
            x += ws[i];
        }
        if (scroll) { v.push_back({T_TABR, R(x + 2, 12, 28, 24)}); x += 30; }
        v.push_back({T_ADDTAB, R(x + 3.333, 12.667, 32, 24)});
        if (!natural) {   // вкладку ведут мышью — рисуется у указателя; соседние доезжают на своё место
            int lo = INT_MAX, hi = INT_MIN, ddx = 0, di = st->dragging ? st->pressed - T_TAB : -1;
            for (auto& e : v) if (e.id >= T_TAB && e.id < T_TABX) { if (e.r.left < lo) lo = e.r.left; if (e.r.right > hi) hi = e.r.right; }
            for (auto& e : v) if (e.id == T_TAB + di) {
                int want = st->dragX - st->grabX, w = e.r.right - e.r.left;
                if (want > hi - w) want = hi - w;
                if (want < lo) want = lo;
                ddx = want - e.r.left;
            }
            for (auto& e : v) {
                int i = e.id >= T_TABX && e.id < T_TABX + 100 ? e.id - T_TABX : e.id >= T_TAB && e.id < T_TABX ? e.id - T_TAB : -1;
                if (i < 0 || i >= (int)st->tabs.size()) continue;
                int dx = i == di ? ddx : (int)lround(st->tabs[i].slide);
                OffsetRect(&e.r, dx, 0);
            }
        }
        if (g_cfg.debug) { double dr = W - 141.333 - 12; if (dr > x + 48) v.push_back({T_DBG, R(x + 44, 8, dr - x - 44, 32)}); }
    }
    v.push_back({T_CLOSE, R(W - 47.333, 0.667, 47.333, 29.333)});
    v.push_back({T_MAX, R(W - 47.333 - 46.667, 0.667, 46.667, 29.333)});
    v.push_back({T_MIN, R(W - 47.333 - 46.667 - 47.333, 0.667, 47.333, 29.333)});
    double bs = g_cfg.navH - 6 < 32 ? g_cfg.navH - 6 : 32;   // кнопки навигации: 32, в узкой строке ниже
    double pitch = 32 + g_cfg.navGap;   // у Windows шаг 48 (кнопка 32 + промежуток 16)
    v.push_back({T_BACK, R(11.333, NavC() - bs / 2 + ny, 32, bs)});
    v.push_back({T_FWD, R(11.333 + pitch, NavC() - bs / 2 + ny, 32, bs)});
    int slot = 2;   // «Вверх» — по настройке; без неё «Обновить» встаёт на его место
    if (g_cfg.upButton) v.push_back({T_UP, R(11.333 + pitch * slot++, NavC() - bs / 2 + ny, 32, bs)});
    v.push_back({T_REFRESH, R(11.333 + pitch * slot, NavC() - bs / 2 + ny, 32, bs)});
    double sx = g_cfg.search ? W - 11.333 - 166 : W - 11.333 + 8;   // без поиска адресная строка — до правого края
    if (g_cfg.search) v.push_back({T_SEARCH, R(sx, fy - 0.667 + ny, 166, FH + 0.667)});
    double ax = 11.333 + pitch * slot + 32 + 14, aw = sx - 8 - ax; if (aw < 60) aw = 60;   // у Windows адрес в 14 после «Обновить»
    v.push_back({T_ADDR, R(ax, fy + ny, aw, FH)});
    // хлебные крошки внутри адресной строки: значок, стрелочка, затем «папка ›» … (не влезающие первые — пропускаем)
    v.push_back({T_ROOTCHEV, R(ax + 38, cy + ny, 26.667, ch)});
    double x = ax + 64.667, limit = ax + aw - 8;
    std::vector<double> widths;
    for (auto& c : st->crumbs) {
        if (c.wDpi != dpi || c.wFs != FieldFont()) { c.w = (float)(TextWidthDWSize(c.name.c_str(), dpi, FieldFont()) * 96.0 / dpi); c.wDpi = dpi; c.wFs = FieldFont(); }   // замер — один раз
        widths.push_back(c.w + 18.667 + 24);
    }
    size_t first = 0; double total = 0;
    for (double w : widths) total += w;
    while (first < widths.size() && x + total > limit) total -= widths[first++];
    for (size_t i = first; i < st->crumbs.size(); ++i) {
        double tw = widths[i] - 24;
        v.push_back({T_CRUMB + (int)i, R(x, cy + ny, tw, ch)});
        v.push_back({T_CRUMBCHEV + (int)i, R(x + tw, cy + ny, 24, ch)});
        x += widths[i];
    }
    return v;
}


// ---------- рисование верха с прозрачностью: под ним — фон Windows 11 «Слюда» (Mica), как у настоящего Проводника ----------
struct ARGBc { float a, r, g, b; };
static ARGBc Hex(DWORD argb) { return {((argb >> 24) & 0xFF) / 255.f, ((argb >> 16) & 0xFF) / 255.f, ((argb >> 8) & 0xFF) / 255.f, (argb & 0xFF) / 255.f}; }
struct Canvas {
    ID2D1DCRenderTarget* rt = nullptr; UINT dpi = 96;
    ID2D1SolidColorBrush* sb = nullptr;   // одна кисть на всё рисование, меняется только цвет
    ~Canvas() { if (sb) sb->Release(); }
    ID2D1SolidColorBrush* B(ARGBc c) {
        D2D1_COLOR_F col = D2D1::ColorF(c.r, c.g, c.b, c.a);
        if (!sb) rt->CreateSolidColorBrush(col, &sb); else sb->SetColor(col);
        if (sb) sb->AddRef();   // вызывающий отпускает, как раньше
        return sb;
    }
    void Fill(const RECT& r, ARGBc c) { if (auto br = B(c)) { rt->FillRectangle(D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom), br); br->Release(); } }
    void Round(const RECT& r, float rad, ARGBc c) {
        if (auto br = B(c)) { D2D1_ROUNDED_RECT rr = {D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom), rad, rad}; rt->FillRoundedRectangle(rr, br); br->Release(); }
    }
    void Foot(int x, int bottom, float r, bool left, ARGBc c) {   // вогнутая «ножка» выбранной вкладки
        ID2D1PathGeometry* g = nullptr; ID2D1GeometrySink* k = nullptr;
        if (FAILED(g_d2d->CreatePathGeometry(&g)) || !g || FAILED(g->Open(&k)) || !k) { if (g) g->Release(); return; }
        float fx = (float)x, fb = (float)bottom;
        k->BeginFigure(D2D1::Point2F(fx, fb - r), D2D1_FIGURE_BEGIN_FILLED);
        k->AddLine(D2D1::Point2F(fx, fb));
        k->AddLine(D2D1::Point2F(left ? fx - r : fx + r, fb));
        k->AddArc(D2D1::ArcSegment(D2D1::Point2F(fx, fb - r), D2D1::SizeF(r, r), 0,
                                   left ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        k->EndFigure(D2D1_FIGURE_END_CLOSED); k->Close();
        if (auto br = B(c)) { rt->FillGeometry(g, br); br->Release(); }
        k->Release(); g->Release();
    }
    void Text(const RECT& r, const wchar_t* t, ARGBc c, float pt, int weight, bool ellipsis, const wchar_t* face = L"Segoe UI", bool center = false) {
        IDWriteTextFormat* f = TextFormat(face, weight, pt * dpi / 96.f, center ? 1 : 0, ellipsis);
        if (!f) return;
        if (auto br = B(c)) {
            rt->DrawText(t, (UINT32)wcslen(t), f, D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom), br, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            br->Release();
        }
    }
    void Glyph(const RECT& r, wchar_t g, ARGBc c, float pt) { wchar_t t[2] = {g, 0}; Text(r, t, c, pt, 400, false, L"Segoe Fluent Icons", true); }
};

static float HA(TopBarState* st, int id) {   // насколько подсвечен элемент (0…1) — с учётом плавного появления/угасания
    if (id == T_NONE) return 0.f;
    if (id == st->hover) return st->animT;
    if (id == st->prevHover) return 1.f - st->animT;
    return 0.f;
}
static ARGBc Ax(ARGBc c, float k) { c.a *= k; return c; }
static void PaintTopBar(HWND h, TopBarState* st) {
    PAINTSTRUCT ps; HDC wdc = BeginPaint(h, &ps);
    RECT cr; GetClientRect(h, &cr);
    bool light = IsLight();
    UINT dpi = DpiOf(h);
    HWND top = GetAncestor(h, GA_ROOT);
    bool active = top && !GetPropW(top, PROP_INACTIVE);
    // цвета — правила оформления Windows 11 (открытые файлы WinUI), с прозрачностью поверх «Слюды»
    ARGBc layer   = Hex(light ? 0xB3FFFFFF : 0x733A3A3A);   // строка навигации и выбранная вкладка
    ARGBc tabHov  = Hex(light ? 0x0A000000 : 0x0FFFFFFF);   // наведение на невыбранную вкладку
    ARGBc divider = Hex(light ? 0x0F000000 : 0x15FFFFFF);   // черта между вкладками
    ARGBc sub1    = Hex(light ? 0x09000000 : 0x0FFFFFFF);   // наведение на кнопку
    ARGBc sub2    = Hex(light ? 0x06000000 : 0x0AFFFFFF);   // нажатие
    ARGBc boxF    = Hex(light ? 0xB3FFFFFF : 0x0FFFFFFF);   // поле ввода
    ARGBc boxH    = Hex(light ? 0x80F9F9F9 : 0x15FFFFFF);   // поле ввода: наведение
    ARGBc boxA    = Hex(light ? 0xFFFFFFFF : 0xB31E1E1E);   // поле ввода: ввод
    ARGBc fg      = Hex(light ? 0xE4000000 : 0xFFFFFFFF);
    ARGBc fg2     = Hex(light ? 0x9E000000 : 0xC5FFFFFF);
    ARGBc fgOff   = Hex(light ? 0x5C000000 : 0x5DFFFFFF);
    ARGBc navOff = fgOff, navOn = fg;   // значки «Назад / Вперёд / Вверх / Обновить»
    ARGBc capFg = active ? fg : Hex(light ? 0x5C000000 : 0x66FFFFFF);   // окно не в фокусе — у Windows тусклеют только кнопки окна
    float rad = 4.f * dpi / 96.f, radTab = 8.f * dpi / 96.f;
    int r8 = P(8, dpi);

    BITMAPINFO bi = {}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth = cr.right; bi.bmiHeader.biHeight = -cr.bottom;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    if (!st->memDC || st->memW != cr.right || st->memH != cr.bottom) {   // размер поменялся — новая картинка
        if (st->memDC) { SelectObject(st->memDC, st->memOld); if (st->memBmp) DeleteObject(st->memBmp); DeleteDC(st->memDC); }
        st->memDC = CreateCompatibleDC(wdc); st->memBits = nullptr;
        st->memBmp = CreateDIBSection(st->memDC, &bi, DIB_RGB_COLORS, &st->memBits, nullptr, 0);
        st->memOld = st->memBmp ? SelectObject(st->memDC, st->memBmp) : nullptr;
        st->memW = cr.right; st->memH = cr.bottom;
    }
    HDC dc = st->memDC; HBITMAP bmp = st->memBmp; void* bits = st->memBits;
    if (bits) {   // перерисовываемый кусок — прозрачный (сквозь видна «Слюда»)
        int x0 = ps.rcPaint.left < 0 ? 0 : ps.rcPaint.left, x1 = ps.rcPaint.right > cr.right ? cr.right : ps.rcPaint.right;
        int y0 = ps.rcPaint.top < 0 ? 0 : ps.rcPaint.top, y1 = ps.rcPaint.bottom > cr.bottom ? cr.bottom : ps.rcPaint.bottom;
        for (int y = y0; y < y1 && x1 > x0; ++y) memset((BYTE*)bits + ((size_t)y * cr.right + x0) * 4, 0, (size_t)(x1 - x0) * 4);
    }
    auto layout = TopLayout(h, st);
    std::vector<std::pair<HICON, POINT>> icons;   // значки — после, поверх (у них своя прозрачность)
    std::pair<HICON, POINT> heldIcon = {nullptr, {}};   // значок взятой мышью вкладки — самым последним
    Canvas cv; cv.dpi = dpi;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    if (bmp && InitD2D() && !st->rt) g_d2d->CreateDCRenderTarget(&props, &st->rt);
    cv.rt = st->rt;
    if (bmp && cv.rt && SUCCEEDED(cv.rt->BindDC(dc, &cr))) {
        cv.rt->BeginDraw();
        cv.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        cv.rt->PushAxisAlignedClip(D2D1::RectF((float)ps.rcPaint.left, (float)ps.rcPaint.top, (float)ps.rcPaint.right, (float)ps.rcPaint.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
        cv.Fill({0, TabsOn(top) ? P(40, dpi) : 0, cr.right, cr.bottom}, layer);   // строка навигации (без вкладок — весь верх)
        if (StripHFor(top) > 0) cv.Fill({0, cr.bottom - P(1.333, dpi), cr.right, cr.bottom}, Hex(light ? 0xFFC8C9CD : 0xFF3A3A3A));   // черта над панелью кнопок (замер с Windows 11)
        // ---- вкладки: сначала остальные, потом выбранная ----
        RECT heldR = {};   // взятая мышью вкладка — её значок рисуется последним, поверх всех
        if (st->dragging) for (auto& e : layout) if (e.id == st->pressed) heldR = e.r;
        for (int pass = 0; pass < 2; ++pass)
            for (auto& e : layout) {
                if (e.id < T_TAB || e.id >= T_TABX) continue;
                int i = e.id - T_TAB;
                if (st->tabs[i].closing) continue;   // закрываемая — пустое место, которое плавно схлопывается
                bool sel = st->dragging ? e.id == st->pressed : i == st->active;   // пока тянут — выбранной выглядит взятая
                if ((pass == 0) == sel) continue;
                bool hov = st->hover == e.id || st->hover == T_TABX + i;
                bool held = st->dragging && e.id == st->pressed;
                if (sel) {
                    if (held) {   // взятую вкладку рисуем плотной — соседние, проезжая под ней, не просвечивают
                        ARGBc solid = Hex(light ? 0xFFF5F3F5 : 0xFF2C2C2C);   // цвет выбранной вкладки (замер с Windows 11)
                        cv.Round({e.r.left, e.r.top, e.r.right, e.r.bottom}, radTab, solid);
                    }
                    // тело вкладки — только выше строки навигации (там она уже закрашена тем же цветом), скругление сверху
                    cv.rt->PushAxisAlignedClip(D2D1::RectF((float)e.r.left, (float)e.r.top, (float)e.r.right, (float)e.r.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
                    cv.Round({e.r.left, e.r.top, e.r.right, e.r.bottom + r8}, radTab, layer);
                    cv.rt->PopAxisAlignedClip();
                    cv.Foot(e.r.left, e.r.bottom, radTab, true, layer);
                    cv.Foot(e.r.right, e.r.bottom, radTab, false, layer);
                } else {
                    bool nearL = i - 1 == st->active, nearR = i + 1 == st->active;
                    float th = HA(st, e.id) > HA(st, T_TABX + i) ? HA(st, e.id) : HA(st, T_TABX + i);
                    if (th > 0.f) cv.Round({e.r.left + P(nearL ? 10 : 2, dpi), e.r.top + P(3, dpi), e.r.right - P(nearR ? 10 : 2, dpi), e.r.bottom - P(5, dpi)}, rad, Ax(tabHov, th));
                    bool nextHov = st->hover == T_TAB + i + 1 || st->hover == T_TABX + i + 1;
                    if (!hov && !nearR && !nextHov && i + 1 < (int)st->tabs.size())
                        cv.Fill({e.r.right - P(1, dpi), e.r.top + P(8, dpi), e.r.right, e.r.bottom - P(8, dpi)}, divider);
                }
                Tab& t = st->tabs[i];
                if (g_cfg.tabIcons && t.icon && e.r.right - e.r.left > P(40, dpi)) {
                    POINT ip = {e.r.left + P(12, dpi), e.r.top + P(8, dpi)};
                    bool under = !held && heldR.right > heldR.left && ip.x + P(16, dpi) > heldR.left && ip.x < heldR.right;   // значок соседки под взятой вкладкой — не рисуем
                    if (!under) { if (held) heldIcon = {t.icon, ip}; else icons.push_back({t.icon, ip}); }
                }   // узкая (раскрывается/сжимается) — без значка
                bool xShown = XShown(st, h, i);
                if (t.pinned) {   // закреплённая: булавка справа
                    cv.Glyph({e.r.right - P(28, dpi), e.r.top + P(8, dpi), e.r.right - P(10, dpi), e.r.top + P(24, dpi)}, 0xE718, fg2, 10.f);
                    xShown = true;   // место под булавку — как под крестик
                }
                RECT tr = {e.r.left + P(g_cfg.tabIcons ? 38 : 12, dpi), e.r.top + P(8, dpi), e.r.right - P(xShown ? 44 : 10, dpi), e.r.top + P(23.333, dpi)};
                cv.Text(tr, t.name.c_str(), sel ? fg : fg2, 12.f, sel ? 600 : 400, true);
            }
        for (auto& e : layout) {
            if (e.id >= T_TABX && e.id < T_TABX + 100) {
                int i = e.id - T_TABX;
                if (!XShown(st, h, i)) continue;
                if (float k = HA(st, e.id)) cv.Round(e.r, rad, Ax(st->pressed == e.id ? sub2 : sub1, k));
                cv.Glyph(e.r, 0xE711, fg, 10.f);
            } else if (e.id == T_ADDTAB) {
                if (float k = HA(st, e.id)) cv.Round(e.r, rad, Ax(st->pressed == e.id ? sub2 : sub1, k));
                cv.Glyph(e.r, 0xE710, fg, 12.f);
            } else if (e.id == T_DBG) {   // отладка — щелчок копирует
                if (float k = HA(st, e.id)) cv.Round(e.r, rad, Ax(sub1, k));
                cv.Text({e.r.left + P(6, dpi), e.r.top, e.r.right - P(6, dpi), e.r.bottom}, st->dbg.c_str(), fg2, 11.f, 400, true);
            } else if (e.id == T_TABL || e.id == T_TABR) {   // стрелки прокрутки вкладок
                bool en = e.id == T_TABL ? st->canL : st->canR;
                if (float k = en ? HA(st, e.id) : 0.f) cv.Round(e.r, rad, Ax(st->pressed == e.id ? sub2 : sub1, k));
                cv.Glyph(e.r, e.id == T_TABL ? 0xE76B : 0xE76C, en ? fg : fg2, 10.f);
            }
        }
        // ---- кнопки окна, навигация, поля ----
        for (auto& e : layout) {
            bool hov = e.id == st->hover, down = hov && e.id == st->pressed;
            switch (e.id) {
                case T_CLOSE: case T_MAX: case T_MIN: {
                    if (float k = HA(st, e.id)) cv.Fill(e.r, Ax(e.id == T_CLOSE ? Hex(down ? 0xE6C42B1C : 0xFFC42B1C) : (down ? sub2 : sub1), k));
                    wchar_t g = e.id == T_CLOSE ? 0xE8BB : e.id == T_MIN ? 0xE921 : (IsZoomed(top) ? 0xE923 : 0xE922);
                    cv.Glyph(e.r, g, (hov && e.id == T_CLOSE) ? Hex(0xFFFFFFFF) : capFg, 10.f);
                    break;
                }
                case T_BACK: case T_FWD: case T_UP: case T_REFRESH: {
                    bool en = e.id == T_BACK ? st->canBack : e.id == T_FWD ? st->canFwd : e.id == T_UP ? st->crumbs.size() > 0 : true;
                    if (float k = en ? HA(st, e.id) : 0.f) cv.Round(e.r, rad, Ax(down ? sub2 : sub1, k));
                    wchar_t g = e.id == T_BACK ? 0xE72B : e.id == T_FWD ? 0xE72A : e.id == T_UP ? 0xE74A : 0xE72C;
                    cv.Glyph(e.r, g, en ? navOn : navOff, 12.f);
                    break;
                }
                case T_ADDR: case T_SEARCH: {
                    bool focus = st->editKind == e.id;
                    cv.Round(e.r, rad, focus ? boxA : hov ? boxH : boxF);
                    if (e.id == T_ADDR) {
                        if (st->rootIcon && !focus) icons.push_back({st->rootIcon, {e.r.left + P(13.333, dpi), (e.r.top + e.r.bottom - P(16, dpi)) / 2}});
                    } else {
                        int gc = (e.r.top + e.r.bottom) / 2;
                        RECT gr = {e.r.right - P(34.667, dpi), gc - P(11, dpi), e.r.right - P(4.667, dpi), gc + P(11, dpi)};
                        if (!focus) {
                            std::wstring ph = TR(L"Поиск в: ", L"Search ") + (st->crumbs.empty() ? std::wstring() : st->crumbs.back().name);
                            cv.Text({e.r.left + P(10.667, dpi), e.r.top, e.r.right - P(36, dpi), e.r.bottom}, ph.c_str(), fg2, FieldFont(), 400, true);
                        }
                        cv.Glyph(gr, 0xE721, fg, 12.f);
                    }
                    break;
                }
                default: break;
            }
        }
        // ---- крошки адресной строки ----
        if (st->editKind != T_ADDR)
            for (auto& e : layout) {
                bool hov = e.id == st->hover, down = hov && e.id == st->pressed;
                if (e.id == T_ROOTCHEV || (e.id >= T_CRUMBCHEV && e.id < T_TAB)) {
                    if (float k = HA(st, e.id)) cv.Round(e.r, rad, Ax(down ? sub2 : sub1, k));
                    cv.Glyph(e.r, 0xE974, fg, 12.f);
                } else if (e.id >= T_CRUMB && e.id < T_CRUMBCHEV) {
                    if (float k = HA(st, e.id)) cv.Round(e.r, rad, Ax(down ? sub2 : sub1, k));
                    cv.Text({e.r.left + P(8.667, dpi), e.r.top, e.r.right, e.r.bottom}, st->crumbs[e.id - T_CRUMB].name.c_str(), fg, FieldFont(), 400, false);
                }
            }
        cv.rt->PopAxisAlignedClip();
        if (cv.rt->EndDraw() == (HRESULT)D2DERR_RECREATE_TARGET) { st->rt->Release(); st->rt = nullptr; }   // «холст» испорчен (сменилась видеокарта и т. п.) — в следующий раз новый
    }
    int is = P(16, dpi);
    for (auto& ic : icons) DrawIconEx(dc, ic.second.x, ic.second.y, ic.first, is, is, 0, nullptr, DI_NORMAL);
    if (heldIcon.first) DrawIconEx(dc, heldIcon.second.x, heldIcon.second.y, heldIcon.first, is, is, 0, nullptr, DI_NORMAL);
    GdiFlush();
    // прозрачность сохраняется: окно Проводника с «Слюдой» просвечивает там, где мы ничего не нарисовали
    BitBlt(wdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
           dc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);   // готовая картинка одним движением (с прозрачностью), только изменившийся кусок
    EndPaint(h, &ps);
}

static int TopHit(HWND h, TopBarState* st, POINT pt) {
    auto layout = TopLayout(h, st);
    // сначала мелкие (крошки и стрелочки), потом крупные поля
    for (auto& e : layout) if (e.id >= T_TABX && PtInRect(&e.r, pt)) {
        int i = e.id - T_TABX;
        if (!XShown(st, h, i)) continue;   // невидимый крестик не ловит щелчок
        return e.id;
    }
    for (auto& e : layout) if ((e.id >= T_CRUMB || e.id == T_ROOTCHEV || e.id == T_ADDTAB) && PtInRect(&e.r, pt)) return e.id;
    for (auto& e : layout) if (PtInRect(&e.r, pt)) return e.id;
    return T_NONE;
}

static void TopAction(HWND h, TopBarState* st, int id) {
    HWND top = GetAncestor(h, GA_ROOT);
    if (!top) return;
    IShellBrowser* sb = BrowserOf(top);
    switch (id) {
        case T_MIN: ShowWindow(top, SW_MINIMIZE); break;
        case T_MAX: ShowWindow(top, IsZoomed(top) ? SW_RESTORE : SW_MAXIMIZE); break;
        case T_CLOSE: PostMessageW(top, WM_CLOSE, 0, 0); break;
        case T_BACK: TabBack(h, st, true); break;
        case T_FWD: TabBack(h, st, false); break;
        case T_ADDTAB: AddNewTab(h, st); break;
        case T_DBG:
            if (OpenClipboard(h)) {
                EmptyClipboard();
                size_t b = (st->dbg.size() + 1) * sizeof(wchar_t);
                if (HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, b)) { memcpy(GlobalLock(g), st->dbg.c_str(), b); GlobalUnlock(g); SetClipboardData(CF_UNICODETEXT, g); }
                CloseClipboard();
            }
            break;
        case T_TABL: if (st->canL) { st->tabOff--; InvalidateRect(h, nullptr, FALSE); } break;
        case T_TABR: if (st->canR) { st->tabOff++; InvalidateRect(h, nullptr, FALSE); } break;
        case T_UP: if (sb) sb->BrowseObject(nullptr, SBSP_PARENT); break;
        case T_REFRESH: if (sb) { IShellView* sv = nullptr; if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) { sv->Refresh(); sv->Release(); } } break;
        default:
            if (id >= T_TABX && id < T_TABX + 100) { CloseTab(h, st, id - T_TABX); break; }
            if (id >= T_TAB && id < T_TABX) { SwitchTab(h, st, id - T_TAB); break; }
            if (id >= T_CRUMB && id < T_CRUMBCHEV && sb && (size_t)(id - T_CRUMB) < st->crumbs.size())
                sb->BrowseObject(st->crumbs[id - T_CRUMB].pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
            break;
    }
}


static const wchar_t* TopTipText(HWND bar, TopBarState* st, int id) {
    if (id >= T_TAB && id < T_TABX) { int i = id - T_TAB; return i < (int)st->tabs.size() ? st->tabs[i].name.c_str() : L""; }
    if (id >= T_TABX && id < T_TABX + 100) return TR(L"Закрыть вкладку (Ctrl+W)", L"Close tab (Ctrl+W)");
    switch (id) {
        case T_BACK: return TR(L"Назад (Alt+Стрелка влево)", L"Back (Alt+Left Arrow)");
        case T_FWD: return TR(L"Вперёд (Alt+Стрелка вправо)", L"Forward (Alt+Right Arrow)");
        case T_UP: return TR(L"Вверх (Alt+Стрелка вверх)", L"Up to parent folder (Alt+Up Arrow)");
        case T_REFRESH: return TR(L"Обновить (F5)", L"Refresh (F5)");
        case T_ADDTAB: return TR(L"Новая вкладка (Ctrl+T)", L"New tab (Ctrl+T)");
        case T_DBG: return TR(L"Скопировать", L"Copy");
        case T_TABL: return st->canL ? TR(L"Прокрутить вкладки влево", L"Scroll tabs left") : L"";
        case T_TABR: return st->canR ? TR(L"Прокрутить вкладки вправо", L"Scroll tabs right") : L"";
        case T_MIN: return TR(L"Свернуть", L"Minimize");
        case T_MAX: { HWND top = GetAncestor(bar, GA_ROOT); return top && IsZoomed(top) ? TR(L"Свернуть в окно", L"Restore Down") : TR(L"Развернуть", L"Maximize"); }
        case T_CLOSE: return TR(L"Закрыть", L"Close");
    }
    return L"";
}
static void TopHoverTo(HWND bar, TopBarState* st, int id) {
    st->prevHover = st->hover; st->hover = id;
    st->animT = g_cfg.anim ? 0.f : 1.f;
    TopKick(bar, st);
}
static bool TopAnimStep(TopBarState* st, float dt) {   // один кадр за dt секунд; true — ещё не закончилось
    bool more = false;
    if (st->animT < 1.f) { st->animT += dt / 0.12f; if (st->animT >= 1.f) { st->animT = 1.f; st->prevHover = T_NONE; } else more = true; }
    for (int i = (int)st->tabs.size() - 1; i >= 0; --i) {
        Tab& t = st->tabs[i];
        if (t.slideT < 1.f) {   // 0,15 с, быстро в начале и мягко в конце
            t.slideT += dt / 0.15f;
            if (t.slideT >= 1.f) { t.slideT = 1.f; t.slide = 0.f; }
            else { float r = 1.f - t.slideT; t.slide = t.slideFrom * r * r * r; more = true; }
        }
        if (t.closing) { t.grow -= dt / 0.18f; if (t.grow <= 0.f) EraseTab(st, i); else more = true; }
        else if (t.grow < 1.f) { t.grow += dt / 0.2f; if (t.grow >= 1.f) t.grow = 1.f; else more = true; }
    }
    return more;
}
static void TopHideTip(HWND bar, TopBarState* st) { KillTimer(bar, 3); if (st->tip) ShowWindow(st->tip, SW_HIDE); }

static void EndEdit(HWND bar, TopBarState* st, bool apply);
// ======================= мгновенный поиск через Everything (тот же обмен сообщениями, что у EverythingToolbar / Everything SDK) =======================
#pragma pack(push, 1)
struct EvQuery2 { DWORD reply_hwnd, reply_copydata_message, search_flags, offset, max_results, request_flags, sort_type; };
struct EvList2 { DWORD totitems, numitems, offset, request_flags, sort_type; };
struct EvItem2 { DWORD flags, data_offset; };
#pragma pack(pop)
static const DWORD EV_REPLY_ID = 0x45495431;   // ответ для выпадающего списка
static const DWORD SW_REPLY_ID = 0x45495432;   // ответ для окна поиска
struct EvResult { std::wstring name, path; bool folder = false; HICON icon = nullptr; bool iconDone = false; ULONGLONG size = ~0ull; FILETIME mod = {}; };
// категории: значок, название, фильтр Everything
struct EvCat { wchar_t glyph; const wchar_t* ru; const wchar_t* macro; const wchar_t* en; const wchar_t* Name() const { return TR(ru, en); } };
static const EvCat EV_CATS[] = {
    {0xE71D, L"Все файлы", L"", L"All files"},        {0xECAA, L"Программы", L"exe: ", L"Programs"}, {0xE8B7, L"Папки", L"folder: ", L"Folders"},
    {0xE8A5, L"Документы", L"doc: ", L"Documents"},   {0xE91B, L"Изображения", L"pic: ", L"Pictures"}, {0xE714, L"Видео", L"video: ", L"Videos"},
    {0xE8D6, L"Музыка", L"audio: ", L"Music"},    {0xF012, L"Архивы", L"zip: ", L"Archives"}};
static const int EV_NCAT = sizeof(EV_CATS) / sizeof(EV_CATS[0]);
struct EvPopup { HWND h = nullptr, bar = nullptr; std::vector<EvResult> items; int sel = -1, hover = -1, cat = 0, chipHover = -1; DWORD total = 0; std::wstring text, scope; };
static const wchar_t* EV_CLASS = L"EIT_Everything";
static EvPopup* g_ev = nullptr;

static HWND EverythingWindow() {
    HWND w = FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION", nullptr);
    if (!w) w = FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)", nullptr);   // Everything 1.5
    return w;
}

static void EvSendEx(HWND reply, DWORD replyId, const std::wstring& q, DWORD maxN, DWORD req) {
    HWND ev = EverythingWindow();
    if (!ev || q.empty()) return;
    ChangeWindowMessageFilterEx(reply, WM_COPYDATA, MSGFLT_ALLOW, nullptr);   // ответ приходит от другой программы — разрешаем его принять
    size_t sz = sizeof(EvQuery2) + (q.size() + 1) * sizeof(wchar_t);
    std::vector<BYTE> buf(sz);
    EvQuery2* e = (EvQuery2*)buf.data();
    e->reply_hwnd = (DWORD)(DWORD_PTR)reply; e->reply_copydata_message = replyId;
    e->search_flags = 0; e->offset = 0; e->max_results = maxN;
    e->request_flags = req;
    e->sort_type = 1;               // по имени — у Everything это всегда мгновенно
    memcpy(e + 1, q.c_str(), (q.size() + 1) * sizeof(wchar_t));
    COPYDATASTRUCT cds = {18 /*запрос 2, юникод*/, (DWORD)sz, buf.data()};
    DWORD_PTR res = 0;
    SendMessageTimeoutW(ev, WM_COPYDATA, (WPARAM)reply, (LPARAM)&cds, SMTO_ABORTIFHUNG, 300, &res);
}

static std::wstring EvScope(const std::wstring& dir) {   // условие Everything: в полном пути есть эта папка (значит, она и подпапки)
    if (dir.empty()) return L"";
    return L"\"" + dir + (dir.back() == L'\\' ? L"" : L"\\") + L"\" ";
}
static std::wstring FolderPath(PCIDLIST_ABSOLUTE p) {   // путь на диске; у «Этого компьютера» и т. п. его нет — тогда поиск везде
    if (!p) return L"";
    PWSTR nm = nullptr; std::wstring r;
    if (SUCCEEDED(SHGetNameFromIDList(p, SIGDN_FILESYSPATH, &nm)) && nm) { r = nm; CoTaskMemFree(nm); }
    return r;
}
static void EvSend(HWND bar, const std::wstring& text) {   // выпадающий список: первые 12, имя и путь, с учётом категории
    int c = 0; std::wstring sc;
    if (g_ev) { g_ev->text = text; c = g_ev->cat; sc = EvScope(g_ev->scope); }
    EvSendEx(bar, EV_REPLY_ID, EV_CATS[c].macro + sc + text, 12, 0x1 | 0x2);
}

// разбор ответа Everything: поля идут в порядке флагов (имя, путь, …, размер, …, дата изменения)
static DWORD EvParse(const COPYDATASTRUCT* cds, size_t maxN, std::vector<EvResult>& out) {
    if (cds->cbData < sizeof(EvList2)) return 0;
    const BYTE* base = (const BYTE*)cds->lpData, *end = base + cds->cbData;
    const EvList2* l = (const EvList2*)base;
    const EvItem2* it = (const EvItem2*)(l + 1);
    for (DWORD i = 0; i < l->numitems && out.size() < maxN; ++i) {
        if ((const BYTE*)(it + i + 1) > end) break;
        const BYTE* p = base + it[i].data_offset;
        EvResult r; r.folder = (it[i].flags & 1) != 0;
        bool bad = false;
        for (DWORD bit = 1; bit <= 0x40 && !bad; bit <<= 1) {
            if (!(l->request_flags & bit)) continue;
            if (bit <= 0x8) {   // текст: длина, затем буквы с нулём на конце
                if (p + 4 > end) { bad = true; break; }
                DWORD len = *(const DWORD*)p; p += 4;
                if (p + (len + 1) * 2 > end) { bad = true; break; }
                if (bit == 1) r.name.assign((const wchar_t*)p, len); else if (bit == 2) r.path.assign((const wchar_t*)p, len);
                p += (len + 1) * 2;
            } else {            // число 8 байт: размер или дата
                if (p + 8 > end) { bad = true; break; }
                if (bit == 0x10) memcpy(&r.size, p, 8); else if (bit == 0x40) memcpy(&r.mod, p, 8);
                p += 8;
            }
        }
        if (bad) break;
        out.push_back(r);
    }
    return l->totitems;
}

static std::wstring EvFull(const EvResult& r) { return r.path.empty() ? r.name : r.path + (r.path.back() == L'\\' ? L"" : L"\\") + r.name; }
static HICON EvIcon(EvResult& r) {   // значок по типу файла — берётся только когда строку видно
    if (!r.iconDone) {
        r.iconDone = true;
        SHFILEINFOW fi = {};
        if (SHGetFileInfoW(EvFull(r).c_str(), r.folder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &fi, sizeof(fi),
                           SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES)) r.icon = fi.hIcon;
    }
    return r.icon;
}
static void EvFreeList(std::vector<EvResult>& v) { for (auto& r : v) if (r.icon) DestroyIcon(r.icon); v.clear(); }
static void EvFree(EvPopup* p) { EvFreeList(p->items); }
static void EvHide() { if (g_ev && g_ev->h) ShowWindow(g_ev->h, SW_HIDE); }

// открыть найденное: папка — в этом окне Проводника; файл — запустить, как двойным щелчком
static bool EvOpenResult(HWND top, const EvResult& r) {
    std::wstring full = EvFull(r);
    if (r.folder && top && IsWindow(top)) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        IShellBrowser* sb = BrowserOf(top);
        if (sb && SUCCEEDED(SHParseDisplayName(full.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) { sb->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER); ILFree(pidl); return true; }
    }
    ShellExecuteW(top, nullptr, full.c_str(), nullptr, r.path.c_str(), SW_SHOWNORMAL);
    return false;
}
static void EvOpen(int i) {
    if (!g_ev || i < 0 || i >= (int)g_ev->items.size()) return;
    EvResult r = g_ev->items[i]; r.icon = nullptr;
    EvHide();
    EvOpenResult(GetAncestor(g_ev->bar, GA_ROOT), r);
}

// размеры выпадающего списка: строка значков категорий сверху, под ней результаты
static int EvHead(UINT dpi) { return P(44, dpi); }
static RECT EvChip(int i, UINT dpi) { return {P(8, dpi) + i * P(36, dpi), P(6, dpi), P(8, dpi) + i * P(36, dpi) + P(32, dpi), P(38, dpi)}; }

static void EvReceive(HWND bar, const COPYDATASTRUCT* cds) {
    if (!g_ev) return;
    EvFree(g_ev);
    g_ev->total = EvParse(cds, 12, g_ev->items); g_ev->sel = -1; g_ev->hover = -1; g_ev->bar = bar;
    for (auto& r : g_ev->items) EvIcon(r);
    // окно результатов под полем поиска, выровнено по его правому краю
    RECT sr = {};
    TopBarState* st = (TopBarState*)GetWindowLongPtrW(bar, GWLP_USERDATA);
    if (st) for (auto& e : TopLayout(bar, st)) if (e.id == T_SEARCH) sr = e.r;
    MapWindowPoints(bar, nullptr, (POINT*)&sr, 2);
    UINT dpi = DpiOf(bar);
    int w = P(520, dpi), rowH = P(44, dpi), n = (int)g_ev->items.size();
    int h = EvHead(dpi) + (n ? P(8, dpi) + n * rowH : P(44, dpi));
    int x = sr.right - w; if (x < 0) x = sr.left;
    if (!g_ev->h) {
        g_ev->h = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, EV_CLASS, L"", WS_POPUP, 0, 0, 0, 0,
                                  GetAncestor(bar, GA_ROOT), nullptr, g_inst, nullptr);
        DWORD corner = 2; DwmSetWindowAttribute(g_ev->h, 33, &corner, sizeof(corner));
    }
    SetWindowPos(g_ev->h, HWND_TOPMOST, x, sr.bottom + P(4, dpi), w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_ev->h, nullptr, FALSE);
}

static void EvPaint(HWND h) {
    PAINTSTRUCT ps; HDC wdc = BeginPaint(h, &ps);
    RECT cr; GetClientRect(h, &cr);
    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, cr.right, cr.bottom);
    HGDIOBJ ob = SelectObject(dc, bmp);
    bool light = IsLight();
    UINT dpi = DpiOf(h);
    COLORREF bg = MenuBg(light), fg = light ? RGB(27, 27, 27) : RGB(255, 255, 255), fg2 = light ? RGB(96, 96, 96) : RGB(170, 170, 170);
    HBRUSH b = CreateSolidBrush(bg); FillRect(dc, &cr, b); DeleteObject(b);
    // значки категорий
    for (int i = 0; i < EV_NCAT; ++i) {
        RECT r = EvChip(i, dpi);
        if (i == g_ev->cat) FillRoundAA(dc, r, 4.f * dpi / 96.f, SelColor(bg, light));
        else if (i == g_ev->chipHover) FillRoundAA(dc, r, 4.f * dpi / 96.f, HoverColor(bg, light, false));
        wchar_t g[2] = {EV_CATS[i].glyph, 0};
        DrawTextDWSize(dc, r, g, i == g_ev->cat ? fg : fg2, dpi, 16.f, 400, false, g_face, 1);
    }
    int ci = g_ev->chipHover >= 0 ? g_ev->chipHover : g_ev->cat;
    RECT nr = {EvChip(EV_NCAT - 1, dpi).right + P(12, dpi), 0, cr.right - P(16, dpi), EvHead(dpi)};
    DrawTextDWSize(dc, nr, EV_CATS[ci].Name(), fg2, dpi, 12.f, 400, true, L"Segoe UI", 2);
    RECT ln = {0, EvHead(dpi) - 1, cr.right, EvHead(dpi)};
    HBRUSH lb = CreateSolidBrush(light ? RGB(229, 229, 229) : RGB(60, 60, 60)); FillRect(dc, &ln, lb); DeleteObject(lb);
    int rowH = P(44, dpi), top = EvHead(dpi) + P(4, dpi), is = P(16, dpi);
    if (g_ev->items.empty()) {
        RECT tr = {P(16, dpi), EvHead(dpi), cr.right - P(16, dpi), cr.bottom};
        DrawTextDWSize(dc, tr, EverythingWindow() ? TR(L"Ничего не найдено", L"No results") : TR(L"Everything не запущена", L"Everything is not running"), fg2, dpi, 14.f);
    }
    for (int i = 0; i < (int)g_ev->items.size(); ++i) {
        RECT r = {P(4, dpi), top + i * rowH, cr.right - P(4, dpi), top + (i + 1) * rowH};
        if (i == g_ev->sel || i == g_ev->hover) FillRoundAA(dc, r, 4.f * dpi / 96.f, HoverColor(bg, light, false));
        EvResult& it = g_ev->items[i];
        if (HICON ic = EvIcon(it)) DrawIconEx(dc, r.left + P(12, dpi), (r.top + r.bottom - is) / 2, ic, is, is, 0, nullptr, DI_NORMAL);
        RECT t1 = {r.left + P(40, dpi), r.top + P(4, dpi), r.right - P(12, dpi), r.top + P(24, dpi)};
        RECT t2 = {r.left + P(40, dpi), r.top + P(22, dpi), r.right - P(12, dpi), r.bottom - P(4, dpi)};
        DrawTextDWSize(dc, t1, it.name.c_str(), fg, dpi, 14.f, 400, true);
        DrawTextDWSize(dc, t2, it.path.c_str(), fg2, dpi, 12.f, 400, true);
    }
    BitBlt(wdc, 0, 0, cr.right, cr.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    EndPaint(h, &ps);
}

LRESULT CALLBACK EvProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: if (g_ev) EvPaint(h); else ValidateRect(h, nullptr); return 0;
        case WM_MOUSEMOVE: {
            if (!g_ev) break;
            UINT dpi = DpiOf(h);
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int ch = -1, i = -1;
            if (pt.y < EvHead(dpi)) { for (int k = 0; k < EV_NCAT; ++k) { RECT r = EvChip(k, dpi); if (PtInRect(&r, pt)) ch = k; } }
            else { int y = pt.y - EvHead(dpi) - P(4, dpi); i = y >= 0 ? y / P(44, dpi) : -1; if (i >= (int)g_ev->items.size()) i = -1; }
            if (i != g_ev->hover || ch != g_ev->chipHover) { g_ev->hover = i; g_ev->chipHover = ch; InvalidateRect(h, nullptr, FALSE); }
            TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0}; TrackMouseEvent(&t);
            return 0;
        }
        case WM_MOUSELEAVE: if (g_ev) { g_ev->hover = -1; g_ev->chipHover = -1; InvalidateRect(h, nullptr, FALSE); } return 0;
        case WM_LBUTTONUP:
            if (g_ev && g_ev->chipHover >= 0) {   // категория — отфильтровать, поле ввода остаётся открытым
                if (g_ev->chipHover != g_ev->cat) { g_ev->cat = g_ev->chipHover; g_ev->sel = -1; EvSend(g_ev->bar, g_ev->text); InvalidateRect(h, nullptr, FALSE); }
                return 0;
            }
            if (g_ev && g_ev->hover >= 0) { int i = g_ev->hover; if (g_ev->bar) { TopBarState* st = (TopBarState*)GetWindowLongPtrW(g_ev->bar, GWLP_USERDATA); if (st) EndEdit(g_ev->bar, st, false); } EvOpen(i); }
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}


// ======================= окно поиска (по Enter): поле, вкладки категорий, таблица результатов =======================
struct SearchWin {
    HWND h = nullptr, edit = nullptr, owner = nullptr;
    HFONT font = nullptr; HBRUSH editBr = nullptr;
    std::vector<EvResult> items; DWORD total = 0;
    int cat = 0, sel = -1, hover = -1, tabHover = -1, scroll = 0; bool drag = false;
    std::wstring text, scope;
};
static const wchar_t* SW_CLASS = L"EIT_SearchWin";
static SearchWin* g_sw = nullptr;

// разметка в точках: поле 12..48, вкладки 58..92, заголовки 100..128, список с 128, строка 32
static int SwListTop(UINT dpi) { return P(128, dpi); }
static int SwRowH(UINT dpi) { return P(32, dpi); }
static RECT SwField(const RECT& cr, UINT dpi) { return {P(16, dpi), P(12, dpi), cr.right - P(16, dpi), P(48, dpi)}; }
static COLORREF SwBg(bool light) { return light ? RGB(243, 243, 243) : RGB(32, 32, 32); }
static COLORREF SwFieldBg(bool light) { return light ? RGB(255, 255, 255) : RGB(45, 45, 45); }
static std::vector<RECT> SwTabs(HWND h, UINT dpi) {   // ширина вкладки — по её названию
    std::vector<RECT> v;
    HDC dc = GetDC(h);
    HFONT f = CreateFontW(-P(14, dpi), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    HGDIOBJ of = SelectObject(dc, f);
    int x = P(16, dpi);
    for (int i = 0; i < EV_NCAT; ++i) {
        SIZE s = {}; GetTextExtentPoint32W(dc, EV_CATS[i].Name(), (int)wcslen(EV_CATS[i].Name()), &s);
        int w = P(12 + 16 + 8 + 12, dpi) + s.cx;
        v.push_back({x, P(58, dpi), x + w, P(92, dpi)});
        x += w + P(4, dpi);
    }
    SelectObject(dc, of); DeleteObject(f); ReleaseDC(h, dc);
    return v;
}
static int SwVisible(HWND h, UINT dpi) { RECT cr; GetClientRect(h, &cr); int v = (cr.bottom - P(8, dpi) - SwListTop(dpi)) / SwRowH(dpi); return v < 1 ? 1 : v; }
static void SwClamp(UINT dpi) {
    int n = (int)g_sw->items.size(), vis = SwVisible(g_sw->h, dpi);
    if (g_sw->scroll > n - vis) g_sw->scroll = n - vis;
    if (g_sw->scroll < 0) g_sw->scroll = 0;
}
static void SwEnsureSel(UINT dpi) {
    int vis = SwVisible(g_sw->h, dpi);
    if (g_sw->sel < g_sw->scroll) g_sw->scroll = g_sw->sel;
    if (g_sw->sel >= g_sw->scroll + vis) g_sw->scroll = g_sw->sel - vis + 1;
    SwClamp(dpi);
}

static void SwQuery() {
    if (!g_sw || !g_sw->h) return;
    std::wstring t = TR(L"Поиск", L"Search") + (g_sw->scope.empty() ? std::wstring() : TR(L" в ", L" in ") + g_sw->scope) + (g_sw->text.empty() ? std::wstring() : L": " + g_sw->text);
    SetWindowTextW(g_sw->h, t.c_str());
    if (g_sw->text.empty()) { EvFreeList(g_sw->items); g_sw->total = 0; g_sw->sel = -1; g_sw->scroll = 0; InvalidateRect(g_sw->h, nullptr, FALSE); return; }
    EvSendEx(g_sw->h, SW_REPLY_ID, EV_CATS[g_sw->cat].macro + EvScope(g_sw->scope) + g_sw->text, 2000, 0x1 | 0x2 | 0x10 | 0x40);   // имя, путь, размер, дата изменения
    InvalidateRect(g_sw->h, nullptr, FALSE);
}

static void SwOpen(int i) {
    if (!g_sw || i < 0 || i >= (int)g_sw->items.size()) return;
    EvResult r = g_sw->items[i]; r.icon = nullptr;
    if (EvOpenResult(g_sw->owner, r)) { SetForegroundWindow(g_sw->owner); PostMessageW(g_sw->h, WM_CLOSE, 0, 0); }   // папка открыта в Проводнике — окно поиска закрываем
}

static std::wstring SwSize(const EvResult& r) {   // как в Проводнике: «2 124 КБ»
    if (r.folder || r.size == ~0ull) return L"";
    ULONGLONG kb = (r.size + 1023) / 1024;
    std::wstring d = std::to_wstring(kb), o;
    for (size_t k = 0; k < d.size(); ++k) { if (k && (d.size() - k) % 3 == 0) o += L'\u00A0'; o += d[k]; }
    return o + TR(L" КБ", L" KB");
}
static std::wstring SwDate(const EvResult& r) {
    if (!r.mod.dwLowDateTime && !r.mod.dwHighDateTime) return L"";
    FILETIME lf; SYSTEMTIME s;
    if (!FileTimeToLocalFileTime(&r.mod, &lf) || !FileTimeToSystemTime(&lf, &s)) return L"";
    wchar_t b[32]; swprintf(b, 32, L"%02u.%02u.%04u %02u:%02u", s.wDay, s.wMonth, s.wYear, s.wHour, s.wMinute);
    return b;
}

struct SwCols { int icon, name, nameR, folder, folderR, size, sizeR, date, dateR; };
static SwCols SwColumns(const RECT& cr, UINT dpi) {
    SwCols c;
    c.dateR = cr.right - P(28, dpi); c.date = c.dateR - P(130, dpi);
    c.sizeR = c.date - P(16, dpi);  c.size = c.sizeR - P(90, dpi);
    c.icon = P(28, dpi); c.name = P(52, dpi);
    c.folder = c.name + (c.size - c.name) * 2 / 5; c.nameR = c.folder - P(12, dpi); c.folderR = c.size - P(12, dpi);
    return c;
}

static void SwPaint(HWND h) {
    PAINTSTRUCT ps; HDC wdc = BeginPaint(h, &ps);
    RECT cr; GetClientRect(h, &cr);
    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, cr.right, cr.bottom);
    HGDIOBJ ob = SelectObject(dc, bmp);
    bool light = IsLight();
    UINT dpi = DpiOf(h);
    COLORREF bg = SwBg(light), fg = light ? RGB(27, 27, 27) : RGB(255, 255, 255), fg2 = light ? RGB(96, 96, 96) : RGB(170, 170, 170);
    COLORREF line = light ? RGB(229, 229, 229) : RGB(60, 60, 60);
    HBRUSH b = CreateSolidBrush(bg); FillRect(dc, &cr, b); DeleteObject(b);
    float rad = 4.f * dpi / 96.f;
    // поле поиска с лупой
    RECT fr = SwField(cr, dpi);
    FillRoundAA(dc, fr, rad, SwFieldBg(light));
    RECT mg = {fr.right - P(36, dpi), fr.top, fr.right - P(8, dpi), fr.bottom};
    DrawTextDWSize(dc, mg, L"\uE721", fg2, dpi, 14.f, 400, false, g_face, 1);
    // вкладки категорий
    auto tabs = SwTabs(h, dpi);
    for (int i = 0; i < EV_NCAT; ++i) {
        RECT r = tabs[i];
        if (i == g_sw->cat) FillRoundAA(dc, r, rad, SelColor(bg, light));
        else if (i == g_sw->tabHover) FillRoundAA(dc, r, rad, HoverColor(bg, light, false));
        COLORREF c = i == g_sw->cat ? fg : fg2;
        wchar_t g[2] = {EV_CATS[i].glyph, 0};
        RECT gr = {r.left + P(12, dpi), r.top, r.left + P(28, dpi), r.bottom};
        DrawTextDWSize(dc, gr, g, c, dpi, 16.f, 400, false, g_face, 1);
        RECT tr = {r.left + P(36, dpi), r.top, r.right, r.bottom};
        DrawTextDWSize(dc, tr, EV_CATS[i].Name(), c, dpi, 14.f, i == g_sw->cat ? 600 : 400);
    }
    if (g_sw->total || !g_sw->scope.empty()) {   // где ищем и сколько нашли
        wchar_t cnt[600];
        if (g_sw->total) swprintf(cnt, 600, g_sw->scope.empty() ? TR(L"Найдено: %lu%ls", L"Found: %lu%ls") : TR(L"Найдено: %lu в %ls", L"Found: %lu in %ls"), (unsigned long)g_sw->total, g_sw->scope.c_str());
        else swprintf(cnt, 600, TR(L"Поиск в %ls", L"Search in %ls"), g_sw->scope.c_str());
        RECT tr = {tabs.back().right + P(12, dpi), P(58, dpi), cr.right - P(16, dpi), P(92, dpi)};
        DrawTextDWSize(dc, tr, cnt, fg2, dpi, 12.f, 400, true, L"Segoe UI", 2);
    }
    // заголовки столбцов
    SwCols c = SwColumns(cr, dpi);
    int hy = P(100, dpi), lt = SwListTop(dpi);
    DrawTextDWSize(dc, {c.name, hy, c.nameR, lt}, TR(L"Имя", L"Name"), fg2, dpi, 12.f);
    DrawTextDWSize(dc, {c.folder, hy, c.folderR, lt}, TR(L"Папка", L"Folder"), fg2, dpi, 12.f);
    DrawTextDWSize(dc, {c.size, hy, c.sizeR, lt}, TR(L"Размер", L"Size"), fg2, dpi, 12.f, 400, false, L"Segoe UI", 2);
    DrawTextDWSize(dc, {c.date, hy, c.dateR, lt}, TR(L"Дата изменения", L"Date modified"), fg2, dpi, 12.f);
    RECT ln = {P(16, dpi), lt - 1, cr.right - P(16, dpi), lt};
    HBRUSH lb = CreateSolidBrush(line); FillRect(dc, &ln, lb); DeleteObject(lb);
    // строки
    int rh = SwRowH(dpi), vis = SwVisible(h, dpi), n = (int)g_sw->items.size(), is = P(16, dpi);
    const wchar_t* empty = g_sw->text.empty() ? TR(L"Введите запрос", L"Type to search") : !EverythingWindow() ? TR(L"Everything не запущена", L"Everything is not running") : n ? nullptr : TR(L"Ничего не найдено", L"No results");
    if (empty) DrawTextDWSize(dc, {P(16, dpi), lt, cr.right - P(16, dpi), lt + P(64, dpi)}, empty, fg2, dpi, 14.f, 400, false, L"Segoe UI", 1);
    for (int k = 0; k < vis + 1 && g_sw->scroll + k < n; ++k) {
        int i = g_sw->scroll + k;
        RECT r = {P(16, dpi), lt + P(4, dpi) + k * rh, cr.right - P(20, dpi), lt + P(4, dpi) + (k + 1) * rh};
        if (r.top >= cr.bottom) break;
        if (i == g_sw->sel) FillRoundAA(dc, r, rad, SelColor(bg, light));
        else if (i == g_sw->hover) FillRoundAA(dc, r, rad, HoverColor(bg, light, false));
        EvResult& it = g_sw->items[i];
        if (HICON ic = EvIcon(it)) DrawIconEx(dc, c.icon, (r.top + r.bottom - is) / 2, ic, is, is, 0, nullptr, DI_NORMAL);
        DrawTextDWSize(dc, {c.name, r.top, c.nameR, r.bottom}, it.name.c_str(), fg, dpi, 14.f, 400, true);
        DrawTextDWSize(dc, {c.folder, r.top, c.folderR, r.bottom}, it.path.c_str(), fg2, dpi, 12.f, 400, true);
        std::wstring sz = SwSize(it), dt = SwDate(it);
        if (!sz.empty()) DrawTextDWSize(dc, {c.size, r.top, c.sizeR, r.bottom}, sz.c_str(), fg2, dpi, 12.f, 400, false, L"Segoe UI", 2);
        if (!dt.empty()) DrawTextDWSize(dc, {c.date, r.top, c.dateR, r.bottom}, dt.c_str(), fg2, dpi, 12.f);
    }
    // полоса прокрутки — тонкая, как в Windows 11
    if (n > vis) {
        int top = lt + P(4, dpi), hgt = cr.bottom - P(8, dpi) - top;
        int th = hgt * vis / n; if (th < P(24, dpi)) th = P(24, dpi);
        int ty = top + (int)((long long)(hgt - th) * g_sw->scroll / (n - vis));
        RECT tr = {cr.right - P(12, dpi), ty, cr.right - P(8, dpi), ty + th};
        FillRoundAA(dc, tr, 2.f * dpi / 96.f, light ? RGB(138, 138, 138) : RGB(159, 159, 159));
    }
    BitBlt(wdc, 0, 0, cr.right, cr.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, ob); DeleteObject(bmp); DeleteDC(dc);
    EndPaint(h, &ps);
}

static void SwLayoutEdit(HWND h) {
    if (!g_sw || !g_sw->edit) return;
    UINT dpi = DpiOf(h);
    RECT cr; GetClientRect(h, &cr);
    RECT fr = SwField(cr, dpi);
    int eh = P(20, dpi);
    MoveWindow(g_sw->edit, fr.left + P(12, dpi), (fr.top + fr.bottom - eh) / 2, fr.right - fr.left - P(52, dpi), eh, TRUE);
}

static void SwSetCat(int c) {
    if (!g_sw || c == g_sw->cat) return;
    g_sw->cat = c; g_sw->sel = -1; g_sw->scroll = 0;
    SwQuery();
}

LRESULT CALLBACK SwEditProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_KEYDOWN && g_sw && g_sw->h) {
        UINT dpi = DpiOf(g_sw->h);
        int n = (int)g_sw->items.size(), vis = SwVisible(g_sw->h, dpi), step = 0;
        if (w == VK_DOWN) step = 1; else if (w == VK_UP) step = -1; else if (w == VK_NEXT) step = vis; else if (w == VK_PRIOR) step = -vis;
        if (step && n) {
            int s = g_sw->sel < 0 ? (step > 0 ? 0 : n - 1) : g_sw->sel + step;
            g_sw->sel = s < 0 ? 0 : s >= n ? n - 1 : s;
            SwEnsureSel(dpi); InvalidateRect(g_sw->h, nullptr, FALSE);
            return 0;
        }
        if (w == VK_RETURN) { SwOpen(g_sw->sel >= 0 ? g_sw->sel : 0); return 0; }
        if (w == VK_ESCAPE) { PostMessageW(g_sw->h, WM_CLOSE, 0, 0); return 0; }
        if (w == VK_TAB) { bool back = GetKeyState(VK_SHIFT) < 0; SwSetCat((g_sw->cat + (back ? EV_NCAT - 1 : 1)) % EV_NCAT); return 0; }
    }
    if (m == WM_CHAR && (w == VK_RETURN || w == VK_ESCAPE || w == VK_TAB)) return 0;   // без системного «бип»
    return DefSubclassProc(h, m, w, l);
}

LRESULT CALLBACK SwProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (!g_sw) return DefWindowProcW(h, m, w, l);
    UINT dpi = DpiOf(h);
    switch (m) {
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: SwPaint(h); return 0;
        case WM_SIZE: SwLayoutEdit(h); SwClamp(dpi); InvalidateRect(h, nullptr, FALSE); return 0;
        case WM_GETMINMAXINFO: { MINMAXINFO* mi = (MINMAXINFO*)l; mi->ptMinTrackSize.x = P(640, dpi); mi->ptMinTrackSize.y = P(320, dpi); return 0; }
        case WM_SETFOCUS: if (g_sw->edit) SetFocus(g_sw->edit); return 0;
        case WM_ACTIVATE: if (LOWORD(w) != WA_INACTIVE && g_sw->edit) SetFocus(g_sw->edit); return 0;
        case WM_CTLCOLOREDIT: {
            bool light = IsLight();
            HDC d = (HDC)w;
            SetTextColor(d, light ? RGB(27, 27, 27) : RGB(255, 255, 255));
            SetBkColor(d, SwFieldBg(light));
            if (g_sw->editBr) DeleteObject(g_sw->editBr);
            g_sw->editBr = CreateSolidBrush(SwFieldBg(light));
            return (LRESULT)g_sw->editBr;
        }
        case WM_COMMAND:
            if (HIWORD(w) == EN_CHANGE && (HWND)l == g_sw->edit) {
                wchar_t buf[512] = {}; GetWindowTextW(g_sw->edit, buf, 512);
                g_sw->text = buf; g_sw->sel = -1; g_sw->scroll = 0;
                SwQuery();
            }
            return 0;
        case WM_COPYDATA: {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)l;
            if (cds && cds->dwData == SW_REPLY_ID && !g_sw->text.empty()) {
                EvFreeList(g_sw->items);
                g_sw->total = EvParse(cds, 2000, g_sw->items);
                g_sw->sel = g_sw->items.empty() ? -1 : 0; g_sw->scroll = 0; g_sw->hover = -1;
                InvalidateRect(h, nullptr, FALSE);
            }
            return TRUE;
        }
        case WM_MOUSEWHEEL: {
            int d = GET_WHEEL_DELTA_WPARAM(w);
            g_sw->scroll -= d / WHEEL_DELTA * 3; SwClamp(dpi);
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        }
        case WM_MOUSEMOVE: {
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            RECT cr; GetClientRect(h, &cr);
            int n = (int)g_sw->items.size(), vis = SwVisible(h, dpi), lt = SwListTop(dpi) + P(4, dpi);
            if (g_sw->drag) {   // перетаскивание полосы прокрутки
                int hgt = cr.bottom - P(8, dpi) - lt;
                if (hgt > 0) { g_sw->scroll = (int)((long long)(pt.y - lt) * n / hgt) - vis / 2; SwClamp(dpi); InvalidateRect(h, nullptr, FALSE); }
                return 0;
            }
            int hv = -1, th = -1;
            if (pt.y >= lt && pt.x < cr.right - P(16, dpi)) { int i = g_sw->scroll + (pt.y - lt) / SwRowH(dpi); if (i < n) hv = i; }
            auto tabs = SwTabs(h, dpi);
            for (int k = 0; k < EV_NCAT; ++k) if (PtInRect(&tabs[k], pt)) th = k;
            if (hv != g_sw->hover || th != g_sw->tabHover) { g_sw->hover = hv; g_sw->tabHover = th; InvalidateRect(h, nullptr, FALSE); }
            TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0}; TrackMouseEvent(&t);
            return 0;
        }
        case WM_MOUSELEAVE: g_sw->hover = -1; g_sw->tabHover = -1; InvalidateRect(h, nullptr, FALSE); return 0;
        case WM_LBUTTONDOWN: {
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            RECT cr; GetClientRect(h, &cr);
            if (pt.y >= SwListTop(dpi) && pt.x >= cr.right - P(16, dpi)) { g_sw->drag = true; SetCapture(h); SendMessageW(h, WM_MOUSEMOVE, w, l); return 0; }
            if (g_sw->tabHover >= 0) { SwSetCat(g_sw->tabHover); return 0; }
            if (g_sw->hover >= 0) { g_sw->sel = g_sw->hover; InvalidateRect(h, nullptr, FALSE); }
            if (g_sw->edit) SetFocus(g_sw->edit);
            return 0;
        }
        case WM_LBUTTONUP: if (g_sw->drag) { g_sw->drag = false; ReleaseCapture(); } return 0;
        case WM_LBUTTONDBLCLK: if (g_sw->hover >= 0) SwOpen(g_sw->hover); return 0;
        case WM_SETTINGCHANGE: {
            BOOL dark = !IsLight(); DwmSetWindowAttribute(h, 20, &dark, sizeof(dark));
            InvalidateRect(h, nullptr, FALSE);
            break;
        }
        case WM_DESTROY:
            EvFreeList(g_sw->items); g_sw->total = 0;
            if (g_sw->font) { DeleteObject(g_sw->font); g_sw->font = nullptr; }
            if (g_sw->editBr) { DeleteObject(g_sw->editBr); g_sw->editBr = nullptr; }
            g_sw->h = nullptr; g_sw->edit = nullptr;
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void SwShow(HWND owner, const std::wstring& text, int cat, const std::wstring& scope) {   // открыть окно поиска (или обновить уже открытое)
    if (!g_sw) g_sw = new SearchWin();
    if (g_sw->h && g_sw->owner != owner) DestroyWindow(g_sw->h);
    g_sw->owner = owner; g_sw->cat = cat; g_sw->scope = scope;
    UINT dpi = owner ? GetDpiForWindow(owner) : 96; if (!dpi) dpi = 96;
    if (!g_sw->h) {
        RECT orc = {}; if (owner) GetWindowRect(owner, &orc);
        int w = P(960, dpi), hh = P(640, dpi);
        int x = (orc.left + orc.right - w) / 2, y = (orc.top + orc.bottom - hh) / 2;
        HMONITOR mon = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(mon, &mi)) {   // целиком на экране
            if (x + w > mi.rcWork.right) x = mi.rcWork.right - w; if (y + hh > mi.rcWork.bottom) y = mi.rcWork.bottom - hh;
            if (x < mi.rcWork.left) x = mi.rcWork.left; if (y < mi.rcWork.top) y = mi.rcWork.top;
        }
        g_sw->h = CreateWindowExW(0, SW_CLASS, TR(L"Поиск", L"Search"), WS_OVERLAPPEDWINDOW, x, y, w, hh, owner, nullptr, g_inst, nullptr);
        if (!g_sw->h) return;
        BOOL dark = !IsLight(); DwmSetWindowAttribute(g_sw->h, 20, &dark, sizeof(dark));
        g_sw->font = CreateFontW(-P(14, dpi), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        g_sw->edit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 0, 0, g_sw->h, nullptr, g_inst, nullptr);
        SendMessageW(g_sw->edit, WM_SETFONT, (WPARAM)g_sw->font, TRUE);
        WindhawkUtils::SetWindowSubclassFromAnyThread(g_sw->edit, SwEditProc, 0);
        SwLayoutEdit(g_sw->h);
        ShowWindow(g_sw->h, SW_SHOWNORMAL);
    }
    g_sw->text = text;
    SetWindowTextW(g_sw->edit, text.c_str());   // сам вызывает поиск (изменение текста)
    SendMessageW(g_sw->edit, EM_SETSEL, text.size(), text.size());
    SetForegroundWindow(g_sw->h);
    SetFocus(g_sw->edit);
}

// прошлые запросы поиска — только в памяти, пока работает Проводник
static std::vector<std::wstring> g_searchHist;
struct HistEnum final : IEnumString {
    LONG ref = 1; size_t pos = 0; std::vector<std::wstring> items;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** pv) override {
        if (riid == IID_IUnknown || riid == IID_IEnumString) { *pv = static_cast<IEnumString*>(this); AddRef(); return S_OK; }
        *pv = nullptr; return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref); }
    ULONG STDMETHODCALLTYPE Release() override { LONG r = InterlockedDecrement(&ref); if (!r) delete this; return r; }
    HRESULT STDMETHODCALLTYPE Next(ULONG n, LPOLESTR* out, ULONG* got) override {
        ULONG k = 0;
        for (; k < n && pos < items.size(); ++k, ++pos) {
            size_t b = (items[pos].size() + 1) * sizeof(wchar_t);
            out[k] = (LPOLESTR)CoTaskMemAlloc(b);
            if (!out[k]) break;
            memcpy(out[k], items[pos].c_str(), b);
        }
        if (got) *got = k;
        return k == n ? S_OK : S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE Skip(ULONG n) override { pos += n; return pos <= items.size() ? S_OK : S_FALSE; }
    HRESULT STDMETHODCALLTYPE Reset() override { pos = 0; return S_OK; }
    HRESULT STDMETHODCALLTYPE Clone(IEnumString** pp) override { HistEnum* e = new HistEnum(); e->items = items; e->pos = pos; *pp = e; return S_OK; }
};
static void AddSearchHist(const std::wstring& q) {
    for (size_t i = 0; i < g_searchHist.size(); ++i) if (_wcsicmp(g_searchHist[i].c_str(), q.c_str()) == 0) { g_searchHist.erase(g_searchHist.begin() + i); break; }
    g_searchHist.insert(g_searchHist.begin(), q);
    if (g_searchHist.size() > 30) g_searchHist.resize(30);
}

static void EndEdit(HWND bar, TopBarState* st, bool apply);
static void EvHide();
LRESULT CALLBACK FieldEditProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    HWND bar = GetParent(h);
    TopBarState* st = (TopBarState*)GetWindowLongPtrW(bar, GWLP_USERDATA);
    if (m == WM_KEYDOWN && st) {
        bool evOn = g_ev && g_ev->h && IsWindowVisible(g_ev->h) && st->editKind == T_SEARCH;
        if (evOn && (w == VK_DOWN || w == VK_UP) && !g_ev->items.empty()) {
            int n = (int)g_ev->items.size();
            g_ev->sel = w == VK_DOWN ? (g_ev->sel + 1) % n : (g_ev->sel <= 0 ? n - 1 : g_ev->sel - 1);
            InvalidateRect(g_ev->h, nullptr, FALSE);
            return 0;
        }
        if (w == VK_RETURN && evOn && g_ev->sel >= 0) { int i = g_ev->sel; EndEdit(bar, st, false); EvOpen(i); return 0; }
        if (w == VK_RETURN && st->editKind == T_SEARCH && g_cfg.everything) {   // Enter — полноценное окно поиска с категориями
            wchar_t buf[512] = {}; GetWindowTextW(h, buf, 512);
            int cat = g_ev ? g_ev->cat : 0;
            std::wstring scope = g_ev ? g_ev->scope : L"";
            HWND top = GetAncestor(bar, GA_ROOT);
            EndEdit(bar, st, false);
            if (buf[0]) SwShow(top, buf, cat, scope);
            return 0;
        }
        if (w == VK_RETURN) { EndEdit(bar, st, true); return 0; }
        if (w == VK_ESCAPE) { EndEdit(bar, st, false); return 0; }
    }
    if (m == WM_CHAR && (w == VK_RETURN || w == VK_ESCAPE)) return 0;   // без системного «бип»
    if (m == WM_KILLFOCUS && st && st->edit == h) { LRESULT r = DefSubclassProc(h, m, w, l); PostMessageW(bar, WM_APP + 1, 0, 0); return r; }
    return DefSubclassProc(h, m, w, l);
}

static void BeginEdit(HWND bar, TopBarState* st, int kind) {
    if (st->edit) return;
    UINT dpi = DpiOf(bar);
    RECT r = {};
    for (auto& e : TopLayout(bar, st)) if (e.id == kind) r = e.r;
    if (r.right <= r.left) return;
    // поле ввода внутри рамки: слева 10⅔ (у поиска) / 11⅓ (у адреса), справа до лупы
    double eh = 19.0 * FieldFont() / 14.0, et = (g_cfg.fieldH - eh) / 2 + 0.5;   // у Windows при 32: сверху 7, высота 19
    RECT er = {r.left + P(kind == T_SEARCH ? 10.667 : 11.333, dpi), r.top + P(et, dpi),
               r.right - P(kind == T_SEARCH ? 36 : 10, dpi), r.top + P(et + eh, dpi)};
    st->editFont = CreateFontW(-P(FieldFont(), dpi), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    st->edit = CreateWindowExW(WS_EX_LAYERED, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, er.left, er.top, er.right - er.left, er.bottom - er.top,
                               bar, nullptr, g_inst, nullptr);
    if (!st->edit) return;
    st->editKind = kind;
    if (kind == T_SEARCH) {   // каждый новый поиск — с «Все файлы» и в папке, где нажат поиск
        if (!g_ev) g_ev = new EvPopup();
        g_ev->cat = 0; g_ev->text.clear(); g_ev->scope = FolderPath(st->cur);
    }
    SetLayeredWindowAttributes(st->edit, 0, 255, LWA_ALPHA);   // поле ввода — непрозрачное поверх «Слюды»
    SendMessageW(st->edit, WM_SETFONT, (WPARAM)st->editFont, TRUE);
    WindhawkUtils::SetWindowSubclassFromAnyThread(st->edit, FieldEditProc, 0);
    if (kind == T_ADDR)   // подсказки пути — встроенные в Windows, как у её адресной строки
        SHAutoComplete(st->edit, SHACF_FILESYSTEM | SHACF_AUTOSUGGEST_FORCE_ON | SHACF_AUTOAPPEND_FORCE_OFF);
    else if (kind == T_SEARCH && !g_cfg.everything && !g_searchHist.empty()) {   // поиск Windows: прошлые запросы
        IAutoComplete2* ac = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_AutoComplete, nullptr, CLSCTX_INPROC_SERVER, IID_IAutoComplete2, (void**)&ac)) && ac) {
            HistEnum* he = new HistEnum(); he->items = g_searchHist;
            if (SUCCEEDED(ac->Init(st->edit, he, nullptr, nullptr))) ac->SetOptions(ACO_AUTOSUGGEST | ACO_UPDOWNKEYDROPSLIST);
            he->Release(); ac->Release();
        }
    }
    if (kind == T_ADDR && st->cur) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(st->cur, SIGDN_DESKTOPABSOLUTEEDITING, &path)) && path) { SetWindowTextW(st->edit, path); CoTaskMemFree(path); }
        SendMessageW(st->edit, EM_SETSEL, 0, -1);
    }
    SetFocus(st->edit);
    InvalidateRect(bar, nullptr, FALSE);
}

static void EndEdit(HWND bar, TopBarState* st, bool apply) {
    if (!st->edit) return;
    EvHide();
    HWND e = st->edit; st->edit = nullptr;
    int kind = st->editKind; st->editKind = T_NONE;
    wchar_t buf[2048] = {}; GetWindowTextW(e, buf, 2048);
    DestroyWindow(e);
    if (st->editFont) { DeleteObject(st->editFont); st->editFont = nullptr; }
    HWND top = GetAncestor(bar, GA_ROOT);
    if (top) { if (HWND st2 = ShellTabOf(top)) { HWND v = FindWindowExW(st2, nullptr, nullptr, nullptr); SetFocus(v ? v : st2); } }
    InvalidateRect(bar, nullptr, FALSE);
    if (!apply || !top || !buf[0]) return;
    IShellBrowser* sb = BrowserOf(top);
    std::wstring target = buf;
    if (kind == T_SEARCH) {   // поиск Windows в текущей папке
        AddSearchHist(buf);
        PWSTR loc = nullptr;
        std::wstring where;
        if (st->cur && SUCCEEDED(SHGetNameFromIDList(st->cur, SIGDN_DESKTOPABSOLUTEPARSING, &loc)) && loc) { where = loc; CoTaskMemFree(loc); }
        target = L"search-ms:displayname=" + UrlPart(TR(L"Результаты поиска", L"Search results")) + L"&crumb=System.Generic.String%3A" + UrlPart(buf) +
                 (where.empty() ? L"" : L"&crumb=location%3A" + UrlPart(where));
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (sb && SUCCEEDED(SHParseDisplayName(target.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) {
        sb->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
        ILFree(pidl);
    } else if (kind == T_ADDR) {
        ShellExecuteW(top, nullptr, buf, nullptr, nullptr, SW_SHOWNORMAL);   // не папка — как в адресной строке Windows: запустить
    }
}


// ---------- область навигации (дерево папок слева) ----------
static const wchar_t* PROP_TVINDENT = L"EIT_TvIndent";   // исходный отступ дерева (+1), чтобы вернуть
static const wchar_t* PROP_TVROOT = L"EIT_TvRoot";       // мы убирали место под стрелочки верхнего уровня
static const wchar_t* PROP_NAVOLD = L"EIT_NavOld";       // метка «дерево под модом»
// Булавки у закреплённых папок. Отладка показала, как Windows их рисует: полупрозрачной картинкой у правого края строки,
// а перед названием вырезает это место из области рисования (поэтому конец названия пропадал).
// Пока дерево рисуется, мод пропускает эту картинку и не даёт вырезать место — название идёт до края
static thread_local HWND t_navPaint = nullptr;   // дерево, которое сейчас рисуется в этом потоке
static thread_local int t_navRight = 0, t_navFull = 0;   // где начинается место булавки и правый край дерева (пиксели)
LRESULT CALLBACK NavTreeProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_PAINT && g_cfg.navNoIndent) {   // Windows могла вернуть свой отступ — поставить наш снова
        if ((int)SendMessageW(h, TVM_GETINDENT, 0, 0) > 1 && !GetPropW(h, L"EIT_TvBusy")) { SetPropW(h, L"EIT_TvBusy", (HANDLE)1); SendMessageW(h, TVM_SETINDENT, 0, 0); RemovePropW(h, L"EIT_TvBusy"); }
        LONG stl = GetWindowLongW(h, GWL_STYLE);
        if (stl & TVS_LINESATROOT) SetWindowLongW(h, GWL_STYLE, stl & ~TVS_LINESATROOT);
    }
    if ((m == WM_PAINT || m == WM_PRINTCLIENT) && g_cfg.navNoPins) {
        RECT cr; GetClientRect(h, &cr);
        HWND prev = t_navPaint; int prevR = t_navRight, prevF = t_navFull;
        t_navPaint = h; t_navRight = cr.right - MulDiv(34, DpiOf(h), 96); t_navFull = cr.right;
        LRESULT r = DefSubclassProc(h, m, w, l);
        t_navPaint = prev; t_navRight = prevR; t_navFull = prevF;
        return r;
    }
    return DefSubclassProc(h, m, w, l);
}
static bool InPinSpot(int x, int w) {   // узкое — в месте булавки (последние 34 точки строки); совсем узкое дерево не трогаем — там значки папок
    if (!t_navPaint) return false;
    UINT d = DpiOf(t_navPaint);
    return t_navFull >= MulDiv(70, d, 96) && x >= t_navRight - MulDiv(4, d, 96) && w <= MulDiv(40, d, 96);
}
using GAB_t = BOOL (WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static GAB_t GAB_orig;
BOOL WINAPI GAB_hook(HDC d, int x, int y, int w, int h, HDC s2, int xs, int ys, int ws, int hs, BLENDFUNCTION bf) {
    if (InPinSpot(x, w)) return TRUE;   // сама булавка
    return GAB_orig(d, x, y, w, h, s2, xs, ys, ws, hs, bf);
}
using ECR_t = int (WINAPI*)(HDC, int, int, int, int);
static ECR_t ECR_orig;
int WINAPI ECR_hook(HDC d, int l, int t, int r, int b) {
    if (InPinSpot(l, r - l)) return SIMPLEREGION;   // не вырезать место булавки
    return ECR_orig(d, l, t, r, b);
}
using ICR_t = int (WINAPI*)(HDC, int, int, int, int);
static ICR_t ICR_orig;
int WINAPI ICR_hook(HDC d, int l, int t, int r, int b) {
    if (t_navPaint && t_navFull >= MulDiv(70, DpiOf(t_navPaint), 96) &&
        r >= t_navRight - MulDiv(8, DpiOf(t_navPaint), 96) && r < t_navFull && l < t_navRight - MulDiv(4, DpiOf(t_navPaint), 96))
        return ICR_orig(d, l, t, t_navFull, b);   // область рисования — до края дерева, а не до места булавки
    return ICR_orig(d, l, t, r, b);
}
static void NavTreeApply(HWND top, bool restoreOnly = false) {
    HWND tv = FindChild(top, L"SysTreeView32", true);   // видимое дерево (невидимые копии могут быть в скрытых частях окна)
    if (!tv) tv = FindChild(top, L"SysTreeView32", false);
    if (!tv) return;
    HWND par = GetParent(tv);
    bool flat = !restoreOnly && g_cfg.navNoIndent;
    // отступ
    HANDLE saved = GetPropW(tv, PROP_TVINDENT);
    if (flat) {   // наименьший сдвиг уровней (закреплённые папки у Windows — первый уровень, поэтому сдвинуты и без стрелочек)
        if (!saved) { int ind = (int)SendMessageW(tv, TVM_GETINDENT, 0, 0); SetPropW(tv, PROP_TVINDENT, (HANDLE)(INT_PTR)(ind + 1)); }
        SendMessageW(tv, TVM_SETINDENT, 0, 0);   // Windows сама поставит наименьший допустимый
    } else if (saved) {
        SendMessageW(tv, TVM_SETINDENT, (int)(INT_PTR)saved - 1, 0);
        RemovePropW(tv, PROP_TVINDENT);
    }
    // место под стрелочки у верхнего уровня
    LONG stl = GetWindowLongW(tv, GWL_STYLE);
    if (flat && (stl & TVS_LINESATROOT)) { SetPropW(tv, PROP_TVROOT, (HANDLE)1); SetWindowLongW(tv, GWL_STYLE, stl & ~TVS_LINESATROOT); InvalidateRect(tv, nullptr, TRUE); }
    else if (!flat && GetPropW(tv, PROP_TVROOT)) { RemovePropW(tv, PROP_TVROOT); SetWindowLongW(tv, GWL_STYLE, stl | TVS_LINESATROOT); InvalidateRect(tv, nullptr, TRUE); }
    // булавки
    (void)par;
    bool hooked = GetPropW(tv, PROP_NAVOLD) != nullptr;
    bool want = !restoreOnly && (g_cfg.navNoPins || g_cfg.navNoIndent);
    if (want && !hooked) {
        SetPropW(tv, PROP_NAVOLD, (HANDLE)1);   // метка: дерево под модом
        WindhawkUtils::SetWindowSubclassFromAnyThread(tv, NavTreeProc, 0);
        InvalidateRect(tv, nullptr, TRUE);
    } else if (!want && hooked) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(tv, NavTreeProc);
        RemovePropW(tv, PROP_NAVOLD);
        InvalidateRect(tv, nullptr, TRUE);
    }
}

static void RelayoutTop(HWND h);
static void MeasureCmdBar(HWND top, bool cpanel) {   // замер старой полосы кнопок; изменилась — окно раскладывается заново
    HWND tab = ShellTabOf(top);
    HWND dv = tab && !cpanel ? FindChild(tab, L"SHELLDLL_DefView", true) : nullptr;
    if (!dv && !cpanel) return;   // список файлов ещё не появился — ждём (прежний замер остаётся)
    int off = 0;   // в панели управления полосы нет — ничего не поднимаем
    if (dv) { RECT tr, dr; GetWindowRect(tab, &tr); GetWindowRect(dv, &dr); off = dr.top - tr.top; }
    if (off < 0 || off > MulDiv(90, DpiOf(top), 96)) off = 0;   // что-то другое — не трогаем
    if (off == CmdBarH(top)) return;
    if (off) SetPropW(top, PROP_CMDH, (HANDLE)(INT_PTR)(off + 1)); else RemovePropW(top, PROP_CMDH);
    RelayoutTop(top);
}
LRESULT CALLBACK TopBarProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    TopBarState* st = (TopBarState*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
        case WM_CREATE:
            st = new TopBarState();
            SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)st);
            SetTimer(h, 9, 100, nullptr);   // только пока окно не готово: подписаться на его события (не дольше 5 с)
            PostMessageW(h, WM_TIMER, 11, 0);
            return 0;
        case WM_TIMER:
            if (st && w == 7) {   // вернуться в папку после перехода «выше и обратно»
                KillTimer(h, 7);
                HWND top = GetAncestor(h, GA_ROOT);
                IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
                if (sb && st->navBack) sb->BrowseObject(st->navBack, SBSP_ABSOLUTE | SBSP_SAMEBROWSER | SBSP_WRITENOHISTORY);
                st->quietUntil = GetTickCount() + 1500;   // ещё немного тишины, пока возвращаемся
                SetTimer(h, 8, 30, nullptr);   // ждём, когда снова откроется своя папка, — и «размораживаем» окно
                return 0;
            }
            if (st && w == 8) {
                HWND top = GetAncestor(h, GA_ROOT);
                PIDLIST_ABSOLUTE now = top ? CurrentFolder(top) : nullptr;
                bool back = now && st->navBack && ILIsEqual(now, st->navBack);
                if (now) ILFree(now);
                if (back || GetTickCount() - st->frozeAt > 1500) {   // вернулись (или на всякий случай — не дольше 1,5 с)
                    KillTimer(h, 8);
                    if (st->navBack) { ILFree(st->navBack); st->navBack = nullptr; }
                    if (st->locked) { LockWindowUpdate(nullptr); st->locked = false; }   // «размораживаем» только своё — чужую заморозку не трогаем
                    if (top) RedrawWindow(top, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
                }
                return 0;
            }
            if (st && w == 3) {   // пауза под указателем прошла — подсказка
                KillTimer(h, 3);
                if (st->hover != T_NONE && st->pressed == T_NONE && !st->edit) {
                    RECT r = {};
                    for (auto& e : TopLayout(h, st)) if (e.id == st->hover) r = e.r;
                    ShowTipText(h, st->tip, TopTipText(h, st, st->hover), r, true);
                }
                return 0;
            }
            if (st && w == 12) {   // вернуть выделение вкладки, как только файлы появились в списке
                ApplyPendingSel(h, st);
                if (st->pendSel.empty()) KillTimer(h, 12);
                return 0;
            }
            if (st && w == 10) KillTimer(h, 10);   // разовая перепроверка
            if (st && st->animating) { SetTimer(h, 10, 150, nullptr); return 0; }   // во время анимации — проверим чуть позже, кадры не должны запинаться
            if (st && w == 1 && (!IsWindowVisible(h) || IsIconic(GetAncestor(h, GA_ROOT))) && st->cur) return 0;   // (запасной опрос) окно свёрнуто — незачем
            if (st) {   // проверка: какая папка открыта, панель управления ли это, что показывать
                HWND top = GetAncestor(h, GA_ROOT);
                if (!st->navHook.cp && top && HookNav(h, st->navHook, top)) KillTimer(h, 1);   // подписались — запасной опрос не нужен
                if (w == 9 && ((st->navHook.cp && st->cur) || ++st->startTries > 50)) {
                    KillTimer(h, 9);
                    if (!st->navHook.cp) SetTimer(h, 1, 100, nullptr);   // подписаться так и не вышло — запасной путь: опрос, как раньше
                }
                PIDLIST_ABSOLUTE cur = top ? CurrentFolder(top) : nullptr;
                bool changed = (cur && (!st->cur || !ILIsEqual(cur, st->cur))) || (!cur && st->cur);
                if (changed) {
                    if (st->cur) ILFree(st->cur);
                    st->cur = cur; cur = nullptr;
                    BuildCrumbs(st, st->cur);
                    OnFolderChanged(st, st->cur);
                    if (!st->restored && top) {   // новое окно — вернуть запомненные вкладки
                        st->restored = true;
                        if (GetPropW(top, PROP_FRESH)) { RemovePropW(top, PROP_FRESH); RestoreSession(h, st); if (TabsOn(top)) RestorePinned(h, st); }
                        NavTreeApply(top);
                    }
                }
                {   // панель управления: убрать / вернуть панель кнопок (без папки — ждём 3 проверки, чтобы не мигало при переходах)
                    bool cp = st->cpanel;
                    if (top && (changed || !st->cur || st->cpTicks > 0)) cp = ShowsControlPanel(top, st->cur);   // та же обычная папка — ответ тот же, не спрашиваем заново
                    if (cp != st->cpanel) {
                        if (++st->cpTicks >= (cp && !st->cur ? 3 : 1)) {
                            st->cpanel = cp; st->cpTicks = 0;
                            if (cp) SetPropW(top, PROP_CPANEL, (HANDLE)1); else RemovePropW(top, PROP_CPANEL);
                            RelayoutTop(top);   // список файлов — вверх на место панели кнопок (или обратно вниз)
                        }
                        if (cp != st->cpanel) SetTimer(h, 10, 150, nullptr);   // ещё не уверены — перепроверить через миг
                    } else st->cpTicks = 0;
                }
                if (top) MeasureCmdBar(top, st->cpanel);   // старая полоса кнопок Windows 7 — под наш верх (в панели управления её нет)
                if (g_cfg.debug && top) {
                    std::wstring d = DebugText(top, st->cur, st->cpanel);
                    if (d != st->dbg) { st->dbg = d; InvalidateRect(h, nullptr, FALSE); }
                }
                bool cb = st->canBack, cf = st->canFwd;
                if (!st->tabs.empty()) { st->canBack = !st->tabs[st->active].back.empty(); st->canFwd = !st->tabs[st->active].fwd.empty(); }
                if (!changed && cb == st->canBack && cf == st->canFwd && !IsZoomed(top) == !st->wasZoomed) { if (cur) ILFree(cur); return 0; }
                st->wasZoomed = IsZoomed(top) != 0;
                if (cur) ILFree(cur);
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_NCHITTEST: {
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)}; ScreenToClient(h, &pt);
            if (st && g_cfg.snap && TopHit(h, st, pt) == T_MAX) return HTTRANSPARENT;   // без раскладок — кнопка наша обычная
            break;
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: if (st) PaintTopBar(h, st); else ValidateRect(h, nullptr); return 0;
        case WM_APP + 9:   // кадр анимации: считаем по прошедшему времени, рисуем сразу и ждём следующего кадра монитора
            if (st && st->animating) {
                double now = QpcSec();
                float dt = (float)(now - st->animLast); st->animLast = now;
                if (dt > 0.05f) dt = 0.05f;
                RECT rr = {};   // что перерисовать: строку вкладок (если они двигаются) и подсвечиваемые кнопки
                {
                    bool tabsMove = false;
                    for (auto& t : st->tabs) if (t.closing || t.grow < 1.f || t.slideT < 1.f) tabsMove = true;
                    UINT fdpi = DpiOf(h);
                    RECT cr; GetClientRect(h, &cr);
                    if (tabsMove) rr = {0, 0, cr.right, P(44, fdpi)};
                    if (st->animT < 1.f)
                        for (auto& e : TopLayout(h, st))
                            if (e.id == st->hover || e.id == st->prevHover || e.id == st->hover + (T_TABX - T_TAB) || e.id == st->prevHover + (T_TABX - T_TAB) ||
                                e.id == st->hover - (T_TABX - T_TAB) || e.id == st->prevHover - (T_TABX - T_TAB)) {
                                RECT er = e.r; InflateRect(&er, P(2, fdpi), P(2, fdpi));
                                UnionRect(&rr, &rr, &er);
                            }
                }
                bool more = TopAnimStep(st, dt);
                RedrawWindow(h, IsRectEmpty(&rr) ? nullptr : &rr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
                if (!more) { st->animating = false; if (g_animBusy > 0) g_animBusy--; }
                return more ? 1 : 0;
            }
            return 0;
        case WM_MOUSEWHEEL: {   // колёсико над вкладками — листать их
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)}; ScreenToClient(h, &pt);
            UINT wdpi = DpiOf(h);
            if (pt.y < P(44, wdpi) && (st->canL || st->canR)) {
                if (GET_WHEEL_DELTA_WPARAM(w) > 0 ? st->canL : st->canR) st->tabOff += GET_WHEEL_DELTA_WPARAM(w) > 0 ? -1 : 1;
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            if (GetCapture() == h && st->pressed >= T_TAB && st->pressed < T_TABX) {   // перетаскивание вкладки — меняем местами с соседней
                UINT ddpi = DpiOf(h);
                RECT bcr; GetClientRect(h, &bcr);
                if (LiveTabs(st) == 1) {   // единственная вкладка — тянем окно целиком, как в Chrome
                    POINT sp; GetCursorPos(&sp);
                    HWND top = GetAncestor(h, GA_ROOT);
                    if (!st->winDrag) {
                        if (abs(pt.x - st->downX) < P(4, ddpi) && abs(pt.y - st->downY) < P(4, ddpi)) return 0;
                        st->winDrag = true; st->dragging = true;
                        TopHideTip(h, st);
                        if (IsZoomed(top)) {   // развёрнутое — сначала вернуть обычный размер, вкладка остаётся под указателем
                            RECT zr; GetWindowRect(top, &zr);
                            double fx = (double)(sp.x - zr.left) / (zr.right - zr.left > 0 ? zr.right - zr.left : 1);
                            ShowWindow(top, SW_RESTORE);
                            RECT wr; GetWindowRect(top, &wr);
                            st->winOff = {(int)((wr.right - wr.left) * fx), sp.y - zr.top};
                        } else { RECT wr; GetWindowRect(top, &wr); st->winOff = {sp.x - wr.left, sp.y - wr.top}; }
                    }
                    HWND tb = FindTargetBar(top, sp);
                    UpdatePreview(h, st, tb, sp);
                    SetCloak(top, st, tb != nullptr);
                    if (!tb) SetWindowPos(top, nullptr, sp.x - st->winOff.x, sp.y - st->winOff.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                    return 0;
                }
                bool out = (pt.y < -P(24, ddpi) || pt.y > P(72, ddpi) || pt.x < -P(24, ddpi) || pt.x > bcr.right + P(24, ddpi));
                if (out) {   // вытаскиваем из окна: над другим окном — «примерка» там, иначе «призрак» с названием у указателя
                    TopHideTip(h, st);
                    st->dragOut = true;
                    POINT sp = pt; ClientToScreen(h, &sp);
                    HWND tb = FindTargetBar(GetAncestor(h, GA_ROOT), sp);
                    UpdatePreview(h, st, tb, sp);
                    if (tb) { if (st->ghost) ShowWindow(st->ghost, SW_HIDE); }
                    else { RECT a = {pt.x, pt.y, pt.x, pt.y}; ShowTipText(h, st->ghost, st->tabs[st->pressed - T_TAB].name.c_str(), a, true); }
                    return 0;
                }
                if (st->previewIn) UpdatePreview(h, st, nullptr, pt);
                if (st->dragOut) { st->dragOut = false; if (st->ghost) ShowWindow(st->ghost, SW_HIDE); }
                if (!st->dragging) {   // до сдвига на 4 точки — это просто щелчок
                    if (abs(pt.x - st->downX) < P(4, ddpi)) return 0;
                    st->dragging = true;
                    // выбранной взятая вкладка станет, когда её отпустят: переход в папку сейчас пересоздал бы список файлов и отобрал мышь
                }
                st->dragX = pt.x;
                st->hover = st->pressed; st->prevHover = T_NONE; st->animT = 1.f;
                for (int guard = 0; guard < 50; ++guard) {   // меняемся с соседней, когда вкладка заходит за её середину
                    int di = st->pressed - T_TAB;
                    RECT dr = {}, lr = {}, rr = {}; bool hl = false, hr = false;
                    for (auto& e : TopLayout(h, st, true)) {
                        if (e.id == T_TAB + di) dr = e.r;
                        if (e.id == T_TAB + di - 1) { lr = e.r; hl = true; }
                        if (e.id == T_TAB + di + 1) { rr = e.r; hr = true; }
                    }
                    int visL = pt.x - st->grabX, dw = dr.right - dr.left, to = -1, slide = 0, gap = P(6, ddpi);   // запас: не меняться туда-обратно на самой границе
                    if (hr && visL + dw > (rr.left + rr.right) / 2 + gap) { to = di + 1; slide = rr.left - dr.left; }
                    else if (hl && visL < (lr.left + lr.right) / 2 - gap) { to = di - 1; slide = -dw; }
                    if (to < 0) break;
                    std::swap(st->tabs[di], st->tabs[to]);
                    if (st->active == di) st->active = to; else if (st->active == to) st->active = di;
                    st->pressed = T_TAB + to; st->hover = st->pressed;
                    if (g_cfg.anim) { Tab& nb = st->tabs[di]; nb.slideFrom = nb.slide + (float)slide; nb.slide = nb.slideFrom; nb.slideT = 0.f; TopKick(h, st); }   // соседняя плавно доезжает
                }
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            int hv = TopHit(h, st, pt);
            if (hv != st->hover) {
                TopHoverTo(h, st, hv); InvalidateRect(h, nullptr, FALSE);
                TopHideTip(h, st);
                if (hv != T_NONE && GetCapture() != h && *TopTipText(h, st, hv)) SetTimer(h, 3, 600, nullptr);   // как у Windows — после паузы
            }
            TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0}; TrackMouseEvent(&t);
            return 0;
        }
        case WM_MOUSELEAVE: if (st) { TopHideTip(h, st); if (GetCapture() != h) { if (st->hover != T_NONE) TopHoverTo(h, st, T_NONE); st->pressed = T_NONE; } InvalidateRect(h, nullptr, FALSE); } return 0;
        case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: {
            if (!st) break;
            TopHideTip(h, st);
            HWND top = GetAncestor(h, GA_ROOT);
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int id = TopHit(h, st, pt);
            if (st->edit && id != st->editKind) EndEdit(h, st, false);
            if (id == T_ADDR || id == T_SEARCH) { BeginEdit(h, st, id); return 0; }
            // верхний край — тянуть размер окна, как за рамку
            UINT dpi0 = DpiOf(h);
            if (id == T_NONE && top && !IsZoomed(top) && pt.y < P(4, dpi0)) {
                POINT sp = pt; ClientToScreen(h, &sp);
                ReleaseCapture();
                SendMessageW(top, WM_NCLBUTTONDOWN, HTTOP, MAKELPARAM(sp.x, sp.y));
                return 0;
            }
            if (id == T_NONE) {
                if (!top) return 0;
                if (id == T_NONE && m == WM_LBUTTONDBLCLK) { ShowWindow(top, IsZoomed(top) ? SW_RESTORE : SW_MAXIMIZE); return 0; }
                if (id == T_NONE) {   // пустое место — перетаскивание окна, как за заголовок
                    POINT sp = pt; ClientToScreen(h, &sp);
                    SetForegroundWindow(top);
                    ReleaseCapture();
                    SendMessageW(top, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(sp.x, sp.y));
                }
                return 0;
            }
            if (top) SetForegroundWindow(top);
            st->pressed = id; SetCapture(h); InvalidateRect(h, nullptr, FALSE);
            if (id >= T_TAB && id < T_TABX) {   // запоминаем, за какую точку взяли вкладку
                st->dragging = false; st->downX = pt.x; st->downY = pt.y; st->dragX = pt.x;
                for (auto& e : TopLayout(h, st)) if (e.id == id) st->grabX = pt.x - e.r.left;
            }
            return 0;
        }
        case WM_RBUTTONUP: {   // правая кнопка по вкладке — меню: закрепить / открепить, закрыть
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int id = TopHit(h, st, pt);
            if (id >= T_TABX && id < T_TABX + 100) id = T_TAB + (id - T_TABX);
            if (id < T_TAB || id >= T_TABX) break;
            int i = id - T_TAB;
            if (i >= (int)st->tabs.size() || st->tabs[i].closing) return 0;
            TopHideTip(h, st);
            RECT r = {pt.x, pt.y, pt.x, pt.y};   // меню — у указателя
            bool pin = st->tabs[i].pinned;
            g_menuCompact = true;
            UINT cmd = RunMenu(h, r, [&](MList* ml, int s, bool light) {
                AddItem(ml, 1, pin ? TR(L"Открепить вкладку", L"Unpin tab") : TR(L"Закрепить вкладку", L"Pin tab"), 0, pin ? 0xE77A : 0xE718, s, light, {});
                AddItem(ml, 2, TR(L"Закрыть вкладку", L"Close tab"), 0, 0xE711, s, light, {});
            });
            g_menuCompact = false;
            if (cmd == 1 && i < (int)st->tabs.size()) { st->tabs[i].pinned = !pin; SavePinned(st, true); InvalidateRect(h, nullptr, FALSE); }
            if (cmd == 2) CloseTab(h, st, i);
            return 0;
        }
        case WM_MBUTTONUP: {
            if (!st) break;
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int id = TopHit(h, st, pt);
            if (id >= T_TABX && id < T_TABX + 100) id = T_TAB + (id - T_TABX);
            if (id >= T_TAB && id < T_TABX) CloseTab(h, st, id - T_TAB);
            return 0;
        }
        case WM_COMMAND:
            if (st && HIWORD(w) == EN_CHANGE && st->editKind == T_SEARCH && g_cfg.everything && st->edit) {
                wchar_t buf[512] = {}; GetWindowTextW(st->edit, buf, 512);
                if (!g_ev) g_ev = new EvPopup();
                g_ev->bar = h;
                if (!buf[0]) EvHide(); else EvSend(h, buf);
            }
            return 0;
        case WM_COPYDATA: {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)l;
            if (st && cds && cds->dwData == EV_REPLY_ID && st->editKind == T_SEARCH) EvReceive(h, cds);
            return TRUE;
        }
        case WM_APP + 3:   // средняя кнопка по элементу списка: если выделена папка — открыть её в новой вкладке
            if (st) {
                HWND top = GetAncestor(h, GA_ROOT);
                IFolderView2* fv = top ? GetFolderView2(top) : nullptr;
                if (fv) {
                    IShellItemArray* sel = nullptr;
                    if (SUCCEEDED(fv->GetSelection(FALSE, &sel)) && sel) {
                        DWORD n = 0; sel->GetCount(&n);
                        for (DWORD k = 0; k < n && k < 20; ++k) {
                            IShellItem* si = nullptr;
                            if (FAILED(sel->GetItemAt(k, &si)) || !si) continue;
                            SFGAOF at = 0; si->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &at);
                            if ((at & SFGAO_FOLDER) && !(at & SFGAO_STREAM)) {   // папка, но не архив
                                PIDLIST_ABSOLUTE pid = nullptr;
                                if (SUCCEEDED(SHGetIDListFromObject(si, &pid)) && pid) { AddTab(h, st, pid); ILFree(pid); }
                            }
                            si->Release();
                        }
                        sel->Release();
                    }
                    fv->Release();
                }
            }
            return 0;
        case WM_APP + 31: {   // к нам «примеряют» вкладку из другого окна: показать её там, куда встанет
            if (!st || !l) return 0;
            const TabDrop* d = (const TabDrop*)l;
            if (!d->src || !d->src->pidl) return 0;
            POINT pt = d->sp; ScreenToClient(h, &pt);
            int pi = -1, k = 0, cur = 0;
            for (size_t i = 0; i < st->tabs.size(); ++i) if (st->tabs[i].preview) pi = (int)i;
            for (auto& e : TopLayout(h, st, true)) {   // место среди настоящих вкладок — по середине вкладки под указателем
                if (e.id < T_TAB || e.id >= T_TABX) continue;
                int i = e.id - T_TAB;
                if (i == pi || st->tabs[i].closing) continue;
                if ((e.r.left + e.r.right) / 2 < pt.x) ++k;
            }
            if (pi >= 0) { for (int i = 0; i < pi; ++i) if (!st->tabs[i].preview && !st->tabs[i].closing) ++cur; if (cur == k) return 1; }   // уже на месте
            if (pi >= 0) EraseTab(st, pi);
            int at = 0, seen = 0;
            while (at < (int)st->tabs.size() && (seen < k || st->tabs[at].closing)) { if (!st->tabs[at].closing) ++seen; ++at; }
            Tab t; TabSetFolder(t, d->src->pidl); t.preview = true;
            if (g_cfg.anim) t.grow = 0.f;
            st->tabs.insert(st->tabs.begin() + at, t);
            if (at <= st->active) st->active++;
            TopKick(h, st);
            InvalidateRect(h, nullptr, FALSE);
            return 1;
        }
        case WM_CAPTURECHANGED:
            if (st && st->pressed >= T_TAB && st->pressed < T_TABX && (st->dragging || st->winDrag) && (HWND)l != h && GetAsyncKeyState(VK_LBUTTON) < 0 &&
                (!l || GetAncestor((HWND)l, GA_ROOT) == GetAncestor(h, GA_ROOT))) {   // отобрало само окно Проводника — вернуть мышь себе, перетаскивание продолжается
                PostMessageW(h, WM_APP + 33, 0, 0);
                return 0;
            }
            if (st && (st->winDrag || st->previewIn) && (HWND)l != h && GetAsyncKeyState(VK_LBUTTON) < 0) {   // кнопка ещё зажата — захват отобрали посреди перетаскивания
                POINT sp; GetCursorPos(&sp);
                UpdatePreview(h, st, nullptr, sp);
                SetCloak(GetAncestor(h, GA_ROOT), st, false);
                st->winDrag = false; st->dragging = false; st->dragOut = false; st->pressed = T_NONE;
                if (st->ghost) ShowWindow(st->ghost, SW_HIDE);
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_APP + 33:   // вернуть себе мышь, если кнопка всё ещё зажата
            if (st && GetAsyncKeyState(VK_LBUTTON) < 0 && GetCapture() != h) SetCapture(h);
            return 0;
        case WM_APP + 32:   // «примерку» увели — убрать
            if (st) for (size_t i = 0; i < st->tabs.size(); ++i) if (st->tabs[i].preview) { EraseTab(st, (int)i); InvalidateRect(h, nullptr, FALSE); break; }
            return 0;
        case WM_APP + 30:   // к нам перетащили вкладку из другого окна
            if (st && l) {
                const TabDrop* d = (const TabDrop*)l;
                if (!d->src || !d->src->pidl) return 0;
                POINT pt = d->sp; ScreenToClient(h, &pt);
                int at = (int)st->tabs.size(), pv = -1;
                for (size_t i = 0; i < st->tabs.size(); ++i) if (st->tabs[i].preview) pv = (int)i;
                if (pv >= 0) { at = pv; EraseTab(st, pv); }   // встаёт ровно туда, где была «примерка»
                else {
                    for (auto& e : TopLayout(h, st, true))
                        if (e.id >= T_TAB && e.id < T_TABX && pt.x < (e.r.left + e.r.right) / 2 && e.id - T_TAB < at) at = e.id - T_TAB;
                }
                if (!st->tabs.empty()) SaveSel(h, st->tabs[st->active]);
                Tab t; TabSetFolder(t, d->src->pidl);
                for (auto p : d->src->back) t.back.push_back(ILCloneFull(p));
                for (auto p : d->src->fwd) t.fwd.push_back(ILCloneFull(p));
                if (g_cfg.anim && pv < 0) t.grow = 0.f;   // после «примерки» — уже раскрыта
                if (at > (int)st->tabs.size()) at = (int)st->tabs.size();
                st->tabs.insert(st->tabs.begin() + at, t);
                st->active = at;
                GoTo(h, st, t.pidl);
                TopKick(h, st);
                InvalidateRect(h, nullptr, FALSE);
                if (HWND tt = GetAncestor(h, GA_ROOT)) SetForegroundWindow(tt);
                return 1;
            }
            return 0;
        case WM_APP + 40:   // Windows сообщила: в окне открылась папка (или список файлов готов)
            if (st) {
                SendMessageW(h, WM_TIMER, 11, 0);
                HWND top = GetAncestor(h, GA_ROOT);
                if (HWND sp = top ? (HWND)GetPropW(top, PROP_STRIP) : nullptr) PostMessageW(sp, WM_APP + 41, 0, 0);   // панели кнопок — новый список файлов
                if (top) NavTreeApply(top);   // дерево слева могло появиться заново
                if (top) PostMessageW(top, g_msgPlace, 0, 0);
            }
            return 0;
        case WM_APP + 34:   // область навигации переключили: перейти в папку выше и сразу обратно, чтобы окно перестроилось
            if (st && st->cur) {
                HWND top = GetAncestor(h, GA_ROOT);
                IShellBrowser* sb = top ? BrowserOf(top) : nullptr;
                if (!sb) return 0;
                PIDLIST_ABSOLUTE up = nullptr;
                {   // папка на диске — её настоящая папка выше (у «Загрузок» — папка пользователя)
                    PWSTR fp = nullptr;
                    if (SUCCEEDED(SHGetNameFromIDList(st->cur, SIGDN_FILESYSPATH, &fp)) && fp) {
                        std::wstring p = fp; CoTaskMemFree(fp);
                        size_t k = p.find_last_of(L'\\');
                        if (k != std::wstring::npos && k > 2) SHParseDisplayName(p.substr(0, k).c_str(), nullptr, &up, 0, nullptr);
                    }
                }
                if (!up) { up = ILCloneFull(st->cur); if (up && !ILRemoveLastID(up)) { ILFree(up); up = nullptr; } }
                if (!up || ILIsEmpty(up)) {   // выше некуда (рабочий стол) — через «Этот компьютер»
                    if (up) ILFree(up);
                    up = nullptr; SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr, &up);
                    if (up && ILIsEqual(up, st->cur)) { ILFree(up); up = nullptr; SHGetKnownFolderIDList(FOLDERID_Desktop, 0, nullptr, &up); }
                }
                if (!up) return 0;
                if (st->navBack) ILFree(st->navBack);
                st->navBack = ILCloneFull(st->cur);
                st->quietUntil = GetTickCount() + 2000;
                st->locked = LockWindowUpdate(top) != FALSE;   // окно «замирает» на экране — перехода туда-обратно не видно (если «заморозить» можно: одновременно — только одно окно в системе)
                st->frozeAt = GetTickCount();
                sb->BrowseObject(up, SBSP_ABSOLUTE | SBSP_SAMEBROWSER | SBSP_WRITENOHISTORY);
                ILFree(up);
                SetTimer(h, 7, 150, nullptr);   // и обратно — чуть погодя, когда окно перестроится
            }
            return 0;
        case WM_APP + 5:   // открыть текущую папку ещё раз в новой вкладке (средняя кнопка по ней в дереве)
            if (st && st->cur) AddTab(h, st, st->cur);
            return 0;
        case WM_APP + 4:   // следующий переход в этом окне — в новую вкладку (средняя кнопка по дереву, пункт меню)
            if (st) st->newTabUntil = GetTickCount() + 3000;
            return 0;
        case WM_APP + 2:   // клавиши вкладок из окна Проводника
            if (st) {
                int n = (int)st->tabs.size();
                switch (w) {
                    case 1: AddNewTab(h, st); break;                                         // Ctrl+T
                    case 2: CloseTab(h, st, st->active); break;                               // Ctrl+W
                    case 3: if (n > 1) SwitchTab(h, st, (st->active + 1) % n); break;        // Ctrl+Tab
                    case 4: if (n > 1) SwitchTab(h, st, (st->active + n - 1) % n); break;    // Ctrl+Shift+Tab
                    case 5: TabBack(h, st, true); break;                                      // Alt+← / кнопка мыши «назад»
                    case 6: TabBack(h, st, false); break;                                     // Alt+→ / «вперёд»
                }
            }
            return 0;
        case WM_LBUTTONUP: {
            if (!st) break;
            int pr = st->pressed;
            if (st->winDrag) {   // тянули окно за единственную вкладку
                st->winDrag = false; st->dragging = false; st->pressed = T_NONE;
                if (GetCapture() == h) ReleaseCapture();
                HWND top = GetAncestor(h, GA_ROOT);
                POINT sp; GetCursorPos(&sp);
                HWND tb = st->previewIn;
                if (!(tb && pr >= T_TAB && pr < T_TABX && DropTabToBar(h, st, pr - T_TAB, tb, sp))) {   // не присоединили — окно остаётся там, куда его принесли
                    UpdatePreview(h, st, nullptr, sp);
                    SetCloak(top, st, false);
                    SetWindowPos(top, nullptr, sp.x - st->winOff.x, sp.y - st->winOff.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            if (st->dragging && pr >= T_TAB && pr < T_TABX && g_cfg.anim) {   // отпущенная вкладка плавно встаёт на место
                int vis = 0, nat = 0;
                for (auto& e : TopLayout(h, st)) if (e.id == pr) vis = e.r.left;
                for (auto& e : TopLayout(h, st, true)) if (e.id == pr) nat = e.r.left;
                { Tab& dt2 = st->tabs[pr - T_TAB]; dt2.slideFrom = dt2.slide = (float)(vis - nat); dt2.slideT = 0.f; }
                TopKick(h, st);
            }
            bool wasDrag = st->dragging;
            if (wasDrag) SavePinned(st, false);
            st->dragging = false;
            st->pressed = T_NONE;
            if (GetCapture() == h) ReleaseCapture();
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            if (st->dragOut) {   // вкладку отпустили за пределами — отдельное окно
                st->dragOut = false;
                if (st->ghost) ShowWindow(st->ghost, SW_HIDE);
                if (pr >= T_TAB && pr < T_TABX) {
                    POINT sp = pt; ClientToScreen(h, &sp);
                    HWND tb = st->previewIn;
                    if (!(tb && DropTabToBar(h, st, pr - T_TAB, tb, sp))) {   // не над другим окном — в своё новое окно (если вкладок больше одной)
                        UpdatePreview(h, st, nullptr, sp);
                        DetachTab(h, st, pr - T_TAB, sp);
                    }
                }
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            if (wasDrag && !st->dragOut && pr >= T_TAB && pr < T_TABX) SwitchTab(h, st, pr - T_TAB);   // перетащили внутри полосы — теперь выбрать
            if (pr != T_NONE && !wasDrag && TopHit(h, st, pt) == pr) {
                if (pr == T_ROOTCHEV || (pr >= T_CRUMBCHEV && pr < T_TAB)) {
                    RECT br = {};
                    for (auto& e : TopLayout(h, st)) if (e.id == pr) br = e.r;
                    PCIDLIST_ABSOLUTE parent = pr == T_ROOTCHEV ? nullptr : st->crumbs[pr - T_CRUMBCHEV].pidl;
                    InvalidateRect(h, nullptr, FALSE);
                    ShowFolderMenu(h, st, br, parent);
                } else TopAction(h, st, pr);
            }
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        }
        case WM_APP + 1: if (st && st->edit && GetFocus() != st->edit) EndEdit(h, st, false); return 0;   // ушли из поля — отменить ввод
        case WM_CTLCOLOREDIT: {
            bool light = IsLight();
            HDC edc = (HDC)w;
            SetTextColor(edc, light ? RGB(27, 27, 27) : RGB(255, 255, 255));
            SetBkColor(edc, light ? RGB(255, 255, 255) : RGB(31, 31, 31));
            if (st) { if (st->editBrush) DeleteObject(st->editBrush); st->editBrush = CreateSolidBrush(light ? RGB(255, 255, 255) : RGB(31, 31, 31)); return (LRESULT)st->editBrush; }
            break;
        }
        case WM_SETCURSOR: {   // у верхнего края — стрелка изменения размера
            POINT pt; GetCursorPos(&pt); ScreenToClient(h, &pt);
            UINT dpi0 = DpiOf(h);
            HWND top = GetAncestor(h, GA_ROOT);
            if (LOWORD(l) == HTCLIENT && top && !IsZoomed(top) && pt.y < P(4, dpi0) && st && TopHit(h, st, pt) == T_NONE) { SetCursor(LoadCursorW(nullptr, (LPCWSTR)IDC_SIZENS)); return TRUE; }
            if (LOWORD(l) == HTCLIENT && st && st->edit == nullptr && st && (TopHit(h, st, pt) == T_ADDR || TopHit(h, st, pt) == T_SEARCH)) { SetCursor(LoadCursorW(nullptr, (LPCWSTR)IDC_IBEAM)); return TRUE; }
            break;
        }
        case WM_DESTROY:
            if (st && st->animating && g_animBusy > 0) g_animBusy--;   // окно закрыли посреди анимации
            if (st) Unhook(st->navHook);
            if (st && st->navBack) { ILFree(st->navBack); st->navBack = nullptr; }
            if (st && st->locked) { LockWindowUpdate(nullptr); st->locked = false; }
            if (st) {   // картинка и «холст» верха
                if (st->rt) st->rt->Release();
                if (st->memDC) { SelectObject(st->memDC, st->memOld); if (st->memBmp) DeleteObject(st->memBmp); DeleteDC(st->memDC); }
            }
            if (st) { KillTimer(h, 3); if (st->tip) DestroyWindow(st->tip); if (st->ghost) DestroyWindow(st->ghost); FreeChildren(st->pendSel); if (st->pendFor) ILFree(st->pendFor);
                      for (auto& t : st->tabs) TabFree(t); if (st->expect) ILFree(st->expect); if (st->edit) DestroyWindow(st->edit); if (st->editFont) DeleteObject(st->editFont); if (st->editBrush) DeleteObject(st->editBrush); FreeCrumbs(st); if (st->cur) ILFree(st->cur); delete st; SetWindowLongPtrW(h, GWLP_USERDATA, 0); }
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

// положение нашего верха: вся видимая рамка окна сверху — до нашей панели кнопок
static void PlaceTopBar(HWND top) {
    HWND bar = (HWND)GetPropW(top, PROP_TOPBAR);
    if (!bar) return;
    if (IsIconic(top) || !IsWindowVisible(top) || !IsClassicTop(top)) { ShowWindow(bar, SW_HIDE); return; }
    RECT cr; GetClientRect(top, &cr);
    Place p = GetPlace(top);
    if (p.top < 10) { ShowWindow(bar, SW_HIDE); return; }
    bool wasHidden = !IsWindowVisible(bar);
    if (wasHidden) SendMessageW(bar, WM_TIMER, 11, 0);   // путь и кнопки — сразу, до появления
    SetWindowPos(bar, HWND_TOP, 0, 0, cr.right, p.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);   // от верха окна до нашей панели кнопок
    {   // фон «Слюда» (как у вкладок Windows 11) — в полосе нашего верха
        BOOL dark = !IsLight();
        DwmSetWindowAttribute(top, 20, &dark, sizeof(dark));
        DWORD bd = 4;   // «Слюда» для окон со вкладками
        DwmSetWindowAttribute(top, 38, &bd, sizeof(bd));
        MARGINS mg = {0, 0, p.top, 0};
        DwmExtendFrameIntoClientArea(top, &mg);
    }
    // строка Windows 10 под нашим верхом не нужна: без неё указатель над «Развернуть» доходит до самого окна
    // (и Windows показывает раскладки), а место списка файлов мы задаём сами
    if (HWND ww = FindWindowExW(top, nullptr, L"WorkerW", nullptr)) if (IsWindowVisible(ww)) ShowWindow(ww, SW_HIDE);
    InvalidateRect(bar, nullptr, FALSE);
    if (wasHidden) UpdateWindow(bar);
}


static const wchar_t* PROP_OWNFRAME = L"EIT_OwnFrame";
static void UseOwnFrame(HWND h) {   // один раз, когда окно в быстром виде: убираем заголовок Windows 10 и просим скругление
    if (GetPropW(h, PROP_OWNFRAME) || !IsClassicTop(h)) return;
    SetPropW(h, PROP_OWNFRAME, (HANDLE)1);
    if (!g_cfg.snap) SetWindowLongW(h, GWL_STYLE, GetWindowLongW(h, GWL_STYLE) & ~WS_MAXIMIZEBOX);   // раскладки выключены
    DWORD corner = 2;   // скруглённые углы Windows 11
    DwmSetWindowAttribute(h, 33, &corner, sizeof(corner));
    SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
static RECT MaxButtonRect(HWND top) {   // «Развернуть» нашего верха — в координатах экрана
    RECT r = {};
    HWND bar = (HWND)GetPropW(top, PROP_TOPBAR);
    if (!bar || !IsWindowVisible(bar)) return r;
    RECT cr; GetClientRect(bar, &cr);
    UINT dpi = DpiOf(bar);
    double W = cr.right * 96.0 / dpi;
    r = {P(W - 47.333 - 46.667, dpi), P(0.667, dpi), P(W - 47.333, dpi), P(30, dpi)};
    MapWindowPoints(bar, nullptr, (POINT*)&r, 2);
    return r;
}
static void SetBarHover(HWND top, int id, int pressed) {
    HWND bar = (HWND)GetPropW(top, PROP_TOPBAR);
    TopBarState* st = bar ? (TopBarState*)GetWindowLongPtrW(bar, GWLP_USERDATA) : nullptr;
    if (!st) return;
    if (st->hover != id || (pressed != -2 && st->pressed != pressed)) {
        st->hover = id; if (pressed != -2) st->pressed = pressed;
        InvalidateRect(bar, nullptr, FALSE);
    }
}

static void PlaceStrip(HWND top) {
    HWND strip = (HWND)GetPropW(top, PROP_STRIP);
    if (!strip) return;
    if (IsIconic(top) || !IsWindowVisible(top)) { ShowWindow(strip, SW_HIDE); return; }
    if (GetPropW(top, PROP_CPANEL) || !g_cfg.toolbar) { ShowWindow(strip, SW_HIDE); UseOwnFrame(top); PlaceTopBar(top); return; }   // в панели управления панели кнопок нет
    RECT cr; GetClientRect(top, &cr);
    Place p = GetPlace(top);
    POINT o = {p.left, p.top};
    int w = cr.right - p.left - p.rmargin;
    if (w <= 0) w = cr.right;
    UINT pdpi = DpiOf(top);
    int hh = p.classic ? p.h : p.h - MulDiv(4, pdpi, 288);
    if (hh < 8) hh = p.h;
    bool wasHidden = !IsWindowVisible(strip);
    if (wasHidden) {   // какие кнопки активны — узнаём до показа, чтобы значки не «мигали» из яркого в бледный
        StripState* sst = (StripState*)GetWindowLongPtrW(strip, GWLP_USERDATA);
        if (sst) sst->enabled = ComputeEnabled(strip);
    }
    SetWindowPos(strip, HWND_TOP, o.x, o.y, w, hh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(strip, nullptr, FALSE);
    if (wasHidden) UpdateWindow(strip);
    UseOwnFrame(top);
    PlaceTopBar(top);
}


// ---------- клавиши вкладок (как в Windows 11): Ctrl+T, Ctrl+W, Ctrl+Tab, Ctrl+Shift+Tab, Alt+←/→, Backspace ----------
static SRWLOCK g_kbLock = SRWLOCK_INIT;
static std::vector<std::pair<DWORD, HHOOK>> g_kbHooks;
static LRESULT CALLBACK KbProc(int code, WPARAM vk, LPARAM l) {
    if (code == HC_ACTION && !(l & 0x80000000)) {
        HWND fg = GetForegroundWindow();
        HWND bar = fg ? (HWND)GetPropW(fg, PROP_TOPBAR) : nullptr;
        if (bar && IsWindowVisible(bar)) {
            bool ctrl = GetKeyState(VK_CONTROL) < 0, shift = GetKeyState(VK_SHIFT) < 0, alt = (l & (1 << 29)) != 0;
            wchar_t fc[32] = {}; HWND foc = GetFocus(); if (foc) GetClassNameW(foc, fc, 32);
            bool typing = wcscmp(fc, L"Edit") == 0;   // в поле ввода (переименование, адрес, поиск) клавиши не трогаем
            int cmd = 0;
            if (!TabsOn(fg) && ctrl && !alt && (vk == 'T' || vk == 'W' || vk == VK_TAB)) cmd = 0;   // без вкладок — как у Windows
            else if (ctrl && !alt && vk == 'T') cmd = 1;
            else if (ctrl && !alt && vk == 'W') cmd = 2;
            else if (ctrl && !alt && vk == VK_TAB) cmd = shift ? 4 : 3;
            else if (alt && !ctrl && vk == VK_LEFT) cmd = 5;
            else if (alt && !ctrl && vk == VK_RIGHT) cmd = 6;
            else if (!ctrl && !alt && vk == VK_BACK && !typing) cmd = 5;
            if (cmd) { PostMessageW(bar, WM_APP + 2, cmd, 0); return 1; }
        }
    }
    return CallNextHookEx(nullptr, code, vk, l);
}

// ---------- средняя кнопка мыши по папке — открыть в новой вкладке (как в QTTabBar) ----------
static std::vector<std::pair<DWORD, HHOOK>> g_msHooks;
static LRESULT CALLBACK MouseProc(int code, WPARAM msg, LPARAM l) {
    if (code == HC_ACTION && g_cfg.tabs && (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)) {   // без вкладок — средняя кнопка как у Windows
        MOUSEHOOKSTRUCT* mh = (MOUSEHOOKSTRUCT*)l;
        HWND w = mh->hwnd;
        HWND top = w ? GetAncestor(w, GA_ROOT) : nullptr;
        HWND bar = top ? (HWND)GetPropW(top, PROP_TOPBAR) : nullptr;
        wchar_t cls[64] = {}, pcls[64] = {};
        if (bar && w) { GetClassNameW(w, cls, 64); if (HWND pw = GetParent(w)) GetClassNameW(pw, pcls, 64); }
        bool list = wcscmp(cls, L"DirectUIHWND") == 0 && wcscmp(pcls, L"SHELLDLL_DefView") == 0;
        bool tree = wcscmp(cls, L"SysTreeView32") == 0;
        if (bar && TabsOn(top) && (list || tree)) {
            if (msg == WM_MBUTTONUP) {
                POINT pt = mh->pt; ScreenToClient(w, &pt);
                LPARAM lp = MAKELPARAM(pt.x, pt.y);
                if (list) {   // щелчок левой — элемент выделится; потом смотрим, папка ли это
                    PostMessageW(w, WM_LBUTTONDOWN, MK_LBUTTON, lp);
                    PostMessageW(w, WM_LBUTTONUP, 0, lp);
                    PostMessageW(bar, WM_APP + 3, 0, 0);
                } else {      // в дереве щелчок открывает папку — этот переход и станет новой вкладкой
                    TVHITTESTINFO hi = {}; hi.pt = pt;
                    HTREEITEM it = (HTREEITEM)SendMessageW(w, TVM_HITTEST, 0, (LPARAM)&hi);
                    if (!it || !(hi.flags & TVHT_ONITEM)) return 1;   // мимо папки — ничего
                    wchar_t txt[260] = {};
                    TVITEMW tv = {}; tv.mask = TVIF_TEXT; tv.hItem = it; tv.pszText = txt; tv.cchTextMax = 260;
                    SendMessageW(w, TVM_GETITEMW, 0, (LPARAM)&tv);
                    TopBarState* bs = (TopBarState*)GetWindowLongPtrW(bar, GWLP_USERDATA);
                    if (bs && bs->cur && !bs->tabs.empty() && _wcsicmp(txt, bs->tabs[bs->active].name.c_str()) == 0) {   // это уже открытая папка — перехода не будет, открываем её копию сами
                        PostMessageW(bar, WM_APP + 5, 0, 0);
                        return 1;
                    }
                    SendMessageW(bar, WM_APP + 4, 0, 0);
                    PostMessageW(w, WM_LBUTTONDOWN, MK_LBUTTON, lp);
                    PostMessageW(w, WM_LBUTTONUP, 0, lp);
                }
            }
            return 1;   // сами обработали
        }
    }
    return CallNextHookEx(nullptr, code, msg, l);
}

// ---------- пункт «Открыть в новой вкладке» в меню папки ----------
static const UINT ID_OPEN_NEW_TAB = 0xE17A;
static std::wstring MenuText(HMENU m, int i) {   // текст пункта без «&» (подчёркнутая буква)
    wchar_t t[256] = {};
    MENUITEMINFOW mi = {sizeof(mi)}; mi.fMask = MIIM_STRING; mi.dwTypeData = t; mi.cch = 255;
    if (!GetMenuItemInfoW(m, i, TRUE, &mi)) return L"";
    std::wstring r;
    for (wchar_t* c = t; *c; ++c) if (*c != L'&') r += *c;
    return r;
}
static int FindOpenNewWindow(HMENU m) {   // позиция пункта «Открыть в новом окне» (или -1)
    int n = GetMenuItemCount(m);
    for (int i = 0; i < n; ++i) {
        std::wstring t = MenuText(m, i);
        if (t.find(L"Открыть в новом окне") != std::wstring::npos || t.find(L"Open in new window") != std::wstring::npos) return i;
    }
    return -1;
}
using TPMEx_t = BOOL (WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TPMEx_t TPMEx_orig;
using TPM_t = BOOL (WINAPI*)(HMENU, UINT, int, int, int, HWND, const RECT*);
static TPM_t TPM_orig;
static BOOL ShowFolderMenuWithTab(HMENU m, UINT f, int x, int y, HWND owner, LPTPMPARAMS pp, bool ex, int reserved, const RECT* rc) {
    auto call = [&](UINT fl) { return ex ? TPMEx_orig(m, fl, x, y, owner, pp) : TPM_orig(m, fl, x, y, reserved, owner, rc); };
    HWND top = owner ? GetAncestor(owner, GA_ROOT) : nullptr;
    HWND bar = top ? (HWND)GetPropW(top, PROP_TOPBAR) : nullptr;
    if (!bar || !m || !TabsOn(top) || !g_cfg.tabMenu) return call(f);   // без вкладок — меню как у Windows
    int at = FindOpenNewWindow(m);
    if (at < 0) return call(f);   // не меню папки
    InsertMenuW(m, at + 1, MF_BYPOSITION | MF_STRING, ID_OPEN_NEW_TAB, TR(L"Открыть в новой вкладке", L"Open in new tab"));
    UINT defId = GetMenuDefaultItem(m, FALSE, 0);
    BOOL r = call(f | TPM_RETURNCMD);
    RemoveMenu(m, ID_OPEN_NEW_TAB, MF_BYCOMMAND);
    UINT cmd = (UINT)r;
    if (cmd == ID_OPEN_NEW_TAB) {   // выполняем обычное «Открыть», а переход превращаем в новую вкладку
        SendMessageW(bar, WM_APP + 4, 0, 0);
        cmd = defId != (UINT)-1 ? defId : 0;
    }
    if (f & TPM_RETURNCMD) return (BOOL)cmd;
    if (cmd) PostMessageW(owner, WM_COMMAND, cmd, 0);
    return cmd != 0;
}
BOOL WINAPI TPMEx_hook(HMENU m, UINT f, int x, int y, HWND owner, LPTPMPARAMS pp) { return ShowFolderMenuWithTab(m, f, x, y, owner, pp, true, 0, nullptr); }
BOOL WINAPI TPM_hook(HMENU m, UINT f, int x, int y, int res, HWND owner, const RECT* rc) { return ShowFolderMenuWithTab(m, f, x, y, owner, nullptr, false, res, rc); }

static void HookKeysForThisThread() {
    DWORD tid = GetCurrentThreadId();
    AcquireSRWLockExclusive(&g_kbLock);
    bool have = false;
    for (auto& e : g_kbHooks) if (e.first == tid) have = true;
    if (!have) {
        HHOOK hk = SetWindowsHookExW(WH_KEYBOARD, KbProc, nullptr, tid); if (hk) g_kbHooks.push_back({tid, hk});
        HHOOK hm = SetWindowsHookExW(WH_MOUSE, MouseProc, nullptr, tid); if (hm) g_msHooks.push_back({tid, hm});
    }
    ReleaseSRWLockExclusive(&g_kbLock);
}

LRESULT CALLBACK TopProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {   // встроен в окно Проводника способом Windhawk (в очередь с другими модами)
    if (m == g_msgCreate) {
        if (!GetPropW(h, PROP_STRIP)) {
            // отдельное окошко, привязанное к окну Проводника: оно всегда лежит над ним,
            // а встроенная панель Windows рисуется внутри окна и перекрыть его не может
            // наша панель — часть самого окна Проводника: обрезается его скруглёнными углами и не отстаёт при перемещении
            HWND strip = CreateWindowExW(0, STRIP_CLASS, L"", WS_CHILD | WS_CLIPSIBLINGS,
                                         0, 0, 0, 0, h, nullptr, g_inst, nullptr);
            if (strip) SetPropW(h, PROP_STRIP, strip);
        }
        if (!GetPropW(h, PROP_TOPBAR)) {
            HWND tb = CreateWindowExW(0, TOPBAR_CLASS, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                      0, 0, 0, 0, h, nullptr, g_inst, nullptr);
            if (tb) SetPropW(h, PROP_TOPBAR, tb);
        }
        HookKeysForThisThread();
        if (GetForegroundWindow() != h) SetPropW(h, PROP_INACTIVE, (HANDLE)1);
        PlaceStrip(h);
        return 0;
    }
    if (m == g_msgPlace) { PlaceStrip(h); return 0; }
    if (m == WM_SETTINGCHANGE || m == WM_THEMECHANGED) {   // сменилась тема (светлая / тёмная) — значки заново, всё перерисовать
        bool light = IsLight();
        if (light != g_lastLight) { g_lastLight = light; ClearIcons(); }
        ResetListBg();   // тема могла смениться — цвет фона перечитать из неё
        if (HWND s3 = (HWND)GetPropW(h, PROP_STRIP)) PostMessageW(s3, WM_APP + 41, 0, 0);   // и у списка файлов
        if (HWND s1 = (HWND)GetPropW(h, PROP_STRIP)) InvalidateRect(s1, nullptr, FALSE);
        if (HWND s2 = (HWND)GetPropW(h, PROP_TOPBAR)) InvalidateRect(s2, nullptr, FALSE);
    }
    if (m == WM_NCACTIVATE) {   // окно получило / потеряло фокус — верх сразу меняет вид, как у Windows
        if (w) RemovePropW(h, PROP_INACTIVE); else SetPropW(h, PROP_INACTIVE, (HANDLE)1);
        if (w) if (HWND sp = (HWND)GetPropW(h, PROP_STRIP)) PostMessageW(sp, WM_APP + 21, 0, 0);
        if (HWND bar = (HWND)GetPropW(h, PROP_TOPBAR)) InvalidateRect(bar, nullptr, FALSE);
    }
    if (m == WM_DESTROY) {   // окно закрывают — запомнить его вкладки
        if (HWND bar = (HWND)GetPropW(h, PROP_TOPBAR))
            if (TopBarState* bst = (TopBarState*)GetWindowLongPtrW(bar, GWLP_USERDATA)) { SaveSession(bst); SavePinned(bst, false); }
    }
    if (m == WM_SHOWWINDOW && w && GetPropW(h, PROP_DROP)) {   // окно из вытащенной вкладки: вкладка — под указателем
        RemovePropW(h, PROP_DROP);
        LRESULT r = DefSubclassProc(h, m, w, l);
        UINT dpi = DpiOf(h);
        SetWindowPos(h, nullptr, g_drop.pt.x - MulDiv(120, dpi, 96), g_drop.pt.y - MulDiv(24, dpi, 96), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return r;
    }
    if (m == WM_APPCOMMAND && GetPropW(h, PROP_TOPBAR)) {   // боковые кнопки мыши «назад/вперёд» — по истории вкладки
        int c = GET_APPCOMMAND_LPARAM(l);
        HWND bar = (HWND)GetPropW(h, PROP_TOPBAR);
        if (c == APPCOMMAND_BROWSER_BACKWARD) { PostMessageW(bar, WM_APP + 2, 5, 0); return TRUE; }
        if (c == APPCOMMAND_BROWSER_FORWARD) { PostMessageW(bar, WM_APP + 2, 6, 0); return TRUE; }
    }
    if (GetPropW(h, PROP_OWNFRAME)) {
        if (m == WM_NCCALCSIZE && w) {   // внутренняя часть окна — от самого верха: там наш верх вместо заголовка Windows 10
            NCCALCSIZE_PARAMS* pp = (NCCALCSIZE_PARAMS*)l;
            int oldTop = pp->rgrc[0].top;
            LRESULT r = DefSubclassProc(h, m, w, l);
            pp->rgrc[0].top = oldTop;
            if (IsZoomed(h)) {   // развёрнутое окно чуть выходит за экран — не прячем верх за край
                UINT dpi = DpiOf(h);
                pp->rgrc[0].top += GetSystemMetricsForDpi(SM_CYFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
            }
            return r;
        }
        if (m == WM_NCHITTEST) {   // над нашей «Развернуть» — как над настоящей: Windows покажет свои раскладки окна
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            RECT mr = MaxButtonRect(h);
            if (g_cfg.snap && PtInRect(&mr, pt)) return HTMAXBUTTON;
        }
        if (m == WM_NCMOUSEMOVE) {
            if (w == HTMAXBUTTON) { SetBarHover(h, T_MAX, -2); TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE | TME_NONCLIENT, h, 0}; TrackMouseEvent(&t); }
            else SetBarHover(h, T_NONE, T_NONE);
        }
        if (m == WM_NCMOUSELEAVE) SetBarHover(h, T_NONE, T_NONE);
        if (m == WM_NCLBUTTONDOWN && w == HTMAXBUTTON) { SetBarHover(h, T_MAX, T_MAX); return 0; }
        if (m == WM_NCLBUTTONUP && w == HTMAXBUTTON) {
            SetBarHover(h, T_MAX, T_NONE);
            ShowWindow(h, IsZoomed(h) ? SW_RESTORE : SW_MAXIMIZE);
            return 0;
        }
    }
    if (m == g_msgNav) { NavTreeApply(h); return 0; }   // настройки поменяли — применить к дереву слева (в потоке окна)
    if (m == g_msgDestroy || m == WM_NCDESTROY) {
        if (m == g_msgDestroy) NavTreeApply(h, true);   // мод выключают — дерево как было
        HWND strip = (HWND)GetPropW(h, PROP_STRIP);
        if (strip) DestroyWindow(strip);
        RemovePropW(h, PROP_STRIP);
        HWND tb = (HWND)GetPropW(h, PROP_TOPBAR);
        if (tb) DestroyWindow(tb);
        RemovePropW(h, PROP_TOPBAR);
        RemovePropW(h, PROP_CPANEL);
        RemovePropW(h, PROP_INACTIVE);
        RemovePropW(h, PROP_DROP);
        RemovePropW(h, PROP_FRESH);
        RemovePropW(h, PROP_CMDH);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, TopProc);
        RemovePropW(h, PROP_OLD);
        if (GetPropW(h, PROP_OWNFRAME)) {
            RemovePropW(h, PROP_OWNFRAME);
            SetWindowLongW(h, GWL_STYLE, GetWindowLongW(h, GWL_STYLE) | WS_MAXIMIZEBOX);
            if (m == g_msgDestroy) SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (m == g_msgDestroy) return 0;
        return DefSubclassProc(h, m, w, l);
    }
    LRESULT r = DefSubclassProc(h, m, w, l);
    if (m == WM_SIZE || m == WM_MOVE || m == WM_DPICHANGED || m == WM_WINDOWPOSCHANGED || m == WM_SHOWWINDOW)
        PlaceStrip(h);
    if (m == WM_ACTIVATE || m == WM_NCACTIVATE) { HWND tb = (HWND)GetPropW(h, PROP_TOPBAR); if (tb) InvalidateRect(tb, nullptr, FALSE); }
    return r;
}

static BOOL CALLBACK TopCb(HWND h, LPARAM) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64) || wcscmp(cls, L"CabinetWClass") != 0) return TRUE;
    if (!GetPropW(h, PROP_OLD)) {
        SetPropW(h, PROP_OLD, (HANDLE)1);   // метка: окно уже под модом
        WindhawkUtils::SetWindowSubclassFromAnyThread(h, TopProc, 0);
        PostMessageW(h, g_msgCreate, 0, 0);
    }
    return TRUE;
}


// ---------------- быстрый вид Проводника: не даём загрузить медленный верх Windows 11 ----------------
// Проводник просит у системы часть {6480100b-…}; отвечаем «такой нет» — и окно открывается в быстром виде
// (тот же приём, что у мода «Classic Explorer navigation bar» автора Windhawk). Только в памяти, в систему ничего не пишется.
static const CLSID CLSID_XamlIslandViewAdapter = {0x6480100b, 0x5a83, 0x4d1e, {0x9f, 0x69, 0x8a, 0xe5, 0xa8, 0x8e, 0x9a, 0x33}};
// лента Windows 10 (её раньше отключал OldNewExplorer): у мода свой верх — ленту Проводнику тоже не даём создать,
// иначе она рисует свою строку с заголовком и значками прямо поверх окна
static const CLSID CLSID_RibbonFw = {0x926749fa, 0x2615, 0x4987, {0x88, 0x45, 0xc3, 0x3e, 0x65, 0xf2, 0xb9, 0x57}};
static bool Blocked(REFCLSID c) { return IsEqualCLSID(c, CLSID_XamlIslandViewAdapter) || IsEqualCLSID(c, CLSID_RibbonFw); }

using CoCI_t = HRESULT (WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCI_t CoCI_orig, CoCI2_orig;
HRESULT WINAPI CoCI_hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID iid, LPVOID* pv) {
    if (Blocked(c)) { if (pv) *pv = nullptr; return REGDB_E_CLASSNOTREG; }
    return CoCI_orig(c, o, ctx, iid, pv);
}
HRESULT WINAPI CoCI2_hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID iid, LPVOID* pv) {
    if (Blocked(c)) { if (pv) *pv = nullptr; return REGDB_E_CLASSNOTREG; }
    return CoCI2_orig(c, o, ctx, iid, pv);
}
using CoCIEx_t = HRESULT (WINAPI*)(REFCLSID, IUnknown*, DWORD, COSERVERINFO*, DWORD, MULTI_QI*);
static CoCIEx_t CoCIEx_orig;
HRESULT WINAPI CoCIEx_hook(REFCLSID c, IUnknown* o, DWORD ctx, COSERVERINFO* si, DWORD n, MULTI_QI* r) {
    if (Blocked(c)) {
        for (DWORD i = 0; i < n; ++i) { r[i].pItf = nullptr; r[i].hr = REGDB_E_CLASSNOTREG; }
        return REGDB_E_CLASSNOTREG;
    }
    return CoCIEx_orig(c, o, ctx, si, n, r);
}
using CoGCO_t = HRESULT (WINAPI*)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*);
static CoGCO_t CoGCO_orig;
HRESULT WINAPI CoGCO_hook(REFCLSID c, DWORD ctx, LPVOID info, REFIID iid, LPVOID* pv) {
    if (Blocked(c)) { if (pv) *pv = nullptr; return REGDB_E_CLASSNOTREG; }
    return CoGCO_orig(c, ctx, info, iid, pv);
}

// ---------------- при включении мода: значки заранее и окна, открытые до включения ----------------
static DWORD WINAPI Worker(LPVOID) {
    g_lastLight = IsLight();
    {   // значки кнопок и стрелочек — заранее, для текущей темы и масштаба экрана
        UINT sd = GetDpiForSystem(); if (!sd) sd = 96;
        int s20 = MulDiv(20, sd, 96), s9 = (int)lround(8.667 * sd / 96.0);
        for (int i = 0; i < BTN_COUNT; ++i) { GetIcon(i, s20, g_lastLight, false); GetIcon(i, s20, g_lastLight, true); }
        GetIcon(100, s9, g_lastLight, false); GetIcon(100, s9, g_lastLight, true);
        InitD2D();
    }
    EnumWindows(TopCb, 0);   // окна, открытые до включения мода (новые мод ловит в момент создания)
    return 0;
}


// ---------- быстрый вид: раздвигаем верх окна до высоты Windows 11 ----------
// Проводник сам ставит список файлов сразу под строку адреса; мы добавляем к этому месту столько, сколько занимает
// верх Windows 11 сверх строки Windows 10 (56⅔ точки) — туда встанут наша панель кнопок и наш верх
static int WantTop(HWND h) {   // сколько сверху должен начинаться список файлов (0 — не трогаем)
    wchar_t cls[32];
    if (!GetClassNameW(h, cls, 32) || wcscmp(cls, L"ShellTabWindowClass") != 0) return 0;
    HWND par = GetParent(h);
    if (!par || !GetClassNameW(par, cls, 32) || wcscmp(cls, L"CabinetWClass") != 0 || !IsClassicTop(par)) return 0;
    UINT dpi = DpiOf(par);
    return (int)lround(TopTotalFor(par) * dpi / 96.0) - CmdBarH(par);   // старая полоса кнопок — под нашим верхом
}
using SWP_t = BOOL (WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SWP_t SWP_orig;
BOOL WINAPI SWP_hook(HWND h, HWND after, int x, int y, int cx, int cy, UINT f) {
    if (!(f & SWP_NOMOVE)) { int t = WantTop(h); if (t && y != t) { if (!(f & SWP_NOSIZE)) cy -= t - y; y = t; } }   // список файлов — ровно под нашим верхом (и не ниже: без пустой полосы)
    return SWP_orig(h, after, x, y, cx, cy, f);
}
using DWP_t = HDWP (WINAPI*)(HDWP, HWND, HWND, int, int, int, int, UINT);
static DWP_t DWP_orig;
HDWP WINAPI DWP_hook(HDWP d, HWND h, HWND after, int x, int y, int cx, int cy, UINT f) {
    if (!(f & SWP_NOMOVE)) { int t = WantTop(h); if (t && y != t) { if (!(f & SWP_NOSIZE)) cy -= t - y; y = t; } }
    return DWP_orig(d, h, after, x, y, cx, cy, f);
}
using MW_t = BOOL (WINAPI*)(HWND, int, int, int, int, BOOL);
static MW_t MW_orig;
BOOL WINAPI MW_hook(HWND h, int x, int y, int cx, int cy, BOOL rp) {
    int t = WantTop(h); if (t && y != t) { cy -= t - y; y = t; }
    return MW_orig(h, x, y, cx, cy, rp);
}


// окно Проводника подхватываем в момент его создания — наши строки готовы ещё до того, как окно появится на экране
using CWEx_t = HWND (WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CWEx_t CWEx_orig;
LRESULT CALLBACK TopProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
HWND WINAPI CWEx_hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int cx, int cy, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND h = CWEx_orig(ex, cls, name, style, x, y, cx, cy, parent, menu, inst, param);
    if (h && !parent) {
        wchar_t c[64];
        if (GetClassNameW(h, c, 64) && wcscmp(c, L"CabinetWClass") == 0 && !GetPropW(h, PROP_OLD)) {
            if (g_drop.until && GetTickCount() < g_drop.until) { SetPropW(h, PROP_DROP, (HANDLE)1); g_drop.until = 0; }
            else SetPropW(h, PROP_FRESH, (HANDLE)1);   // окно из вытащенной вкладки — без запомненных вкладок
            SetPropW(h, PROP_OLD, (HANDLE)1);   // метка: окно уже под модом
            WindhawkUtils::SetWindowSubclassFromAnyThread(h, TopProc, 0);
            SendMessageW(h, g_msgCreate, 0, 0);
        }
    }
    return h;
}

template <typename T>
static void HookFn(HMODULE m, const char* name, void* hook, T* orig) {
    if (!m) return;
    void* p = (void*)GetProcAddress(m, name);
    if (p) Wh_SetFunctionHook(p, hook, (void**)orig);
}

BOOL Wh_ModInit() {
    g_ru = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_RUSSIAN;   // надписи мода — на языке Windows
    LoadSettings();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, (HMODULE*)&g_inst);
    g_msgCreate = RegisterWindowMessageW(L"EIT_Create");
    g_msgNav = RegisterWindowMessageW(L"EIT_NavApply");   // своё сообщение окну Проводника (чужие номера WM_APP у него могут быть заняты)
    g_msgDestroy = RegisterWindowMessageW(L"EIT_Destroy");
    g_msgPlace = RegisterWindowMessageW(L"EIT_Place");
    PickFace();
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = StripProc; wc.hInstance = g_inst; wc.lpszClassName = STRIP_CLASS;
    wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&wc);
    WNDCLASSEXW tc = {sizeof(tc)};
    tc.lpfnWndProc = TipProc; tc.hInstance = g_inst; tc.lpszClassName = TIP_CLASS;
    tc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&tc);
    WNDCLASSEXW bc = {sizeof(bc)};
    bc.style = CS_DBLCLKS; bc.lpfnWndProc = TopBarProc; bc.hInstance = g_inst; bc.lpszClassName = TOPBAR_CLASS;
    bc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&bc);
    WNDCLASSEXW ec = {sizeof(ec)};
    ec.lpfnWndProc = EvProc; ec.hInstance = g_inst; ec.lpszClassName = EV_CLASS; ec.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&ec);
    WNDCLASSEXW sc = {sizeof(sc)};
    sc.style = CS_DBLCLKS; sc.lpfnWndProc = SwProc; sc.hInstance = g_inst; sc.lpszClassName = SW_CLASS; sc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&sc);
    {   // быстрый вид Проводника — перехват запроса нового верха
        HMODULE cb = GetModuleHandleW(L"combase.dll");
        if (!cb) cb = LoadLibraryW(L"combase.dll");
        HookFn(cb, "CoCreateInstance", (void*)CoCI_hook, &CoCI_orig);
        HookFn(cb, "CoCreateInstanceEx", (void*)CoCIEx_hook, &CoCIEx_orig);
        HookFn(cb, "CoGetClassObject", (void*)CoGCO_hook, &CoGCO_orig);
        {   // та же точка входа, как её видит сам мод через ole32 (если это отдельная копия — перехватываем и её)
            void* viaImport = (void*)&CoCreateInstance;
            void* inCombase = cb ? (void*)GetProcAddress(cb, "CoCreateInstance") : nullptr;
            if (viaImport && viaImport != inCombase) Wh_SetFunctionHook(viaImport, (void*)CoCI2_hook, (void**)&CoCI2_orig);
        }
        HMODULE u32 = GetModuleHandleW(L"user32.dll");
        HookFn(u32, "SetWindowPos", (void*)SWP_hook, &SWP_orig);
        HookFn(u32, "CreateWindowExW", (void*)CWEx_hook, &CWEx_orig);
        HookFn(u32, "DeferWindowPos", (void*)DWP_hook, &DWP_orig);
        HookFn(u32, "MoveWindow", (void*)MW_hook, &MW_orig);
        HookFn(u32, "TrackPopupMenuEx", (void*)TPMEx_hook, &TPMEx_orig);
        HookFn(u32, "TrackPopupMenu", (void*)TPM_hook, &TPM_orig);
        HookFn(GetModuleHandleW(L"gdi32.dll"), "GdiAlphaBlend", (void*)GAB_hook, &GAB_orig);      // булавки в области навигации (действуют, только пока рисуется дерево)
        HookFn(GetModuleHandleW(L"gdi32.dll"), "ExcludeClipRect", (void*)ECR_hook, &ECR_orig);
        HookFn(GetModuleHandleW(L"gdi32.dll"), "IntersectClipRect", (void*)ICR_hook, &ICR_orig);
    }
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return TRUE;
}

static BOOL CALLBACK UnhookCb(HWND h, LPARAM) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetPropW(h, PROP_OLD)) SendMessageW(h, g_msgDestroy, 0, 0);
    return TRUE;
}

void Wh_ModUninit() {
    g_frameStop = 1;   // потоки кадров — выйти до выгрузки мода
    for (int i = 0; i < 100 && g_frameThreads > 0; ++i) Sleep(10);
    if (g_thread) { WaitForSingleObject(g_thread, 5000); CloseHandle(g_thread); }
    AcquireSRWLockExclusive(&g_kbLock);
    for (auto& e : g_kbHooks) UnhookWindowsHookEx(e.second);
    g_kbHooks.clear();
    for (auto& e : g_msHooks) UnhookWindowsHookEx(e.second);
    g_msHooks.clear();
    ReleaseSRWLockExclusive(&g_kbLock);
    EnumWindows(UnhookCb, 0);
    UnregisterClassW(STRIP_CLASS, g_inst);
    UnregisterClassW(TIP_CLASS, g_inst);
    UnregisterClassW(TOPBAR_CLASS, g_inst);
    if (g_ev) { if (g_ev->h) DestroyWindow(g_ev->h); EvFree(g_ev); delete g_ev; g_ev = nullptr; }
    UnregisterClassW(EV_CLASS, g_inst);
    if (g_sw) { if (g_sw->h) SendMessageW(g_sw->h, WM_CLOSE, 0, 0); delete g_sw; g_sw = nullptr; }
    UnregisterClassW(SW_CLASS, g_inst);
    ClearIcons();
    ClearTextFormats();
    if (g_rp) { g_rp->Release(); g_rp = nullptr; }
    if (g_d2d) { g_d2d->Release(); g_d2d = nullptr; }
    if (g_dw) { g_dw->Release(); g_dw = nullptr; }
}

static BOOL CALLBACK RedrawBarsCb(HWND h, LPARAM) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId()) if (HWND b = (HWND)GetPropW(h, PROP_TOPBAR)) InvalidateRect(b, nullptr, FALSE);
    return TRUE;
}
static BOOL CALLBACK ApplySnapCb(HWND h, LPARAM) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetPropW(h, PROP_TOPBAR)) {
        LONG st = GetWindowLongW(h, GWL_STYLE);
        LONG ns = g_cfg.snap ? (st | WS_MAXIMIZEBOX) : (st & ~WS_MAXIMIZEBOX);
        if (ns != st) SetWindowLongW(h, GWL_STYLE, ns);
    }
    return TRUE;
}
static void RelayoutTop(HWND h) {   // высоты поменялись — Проводник заново раскладывает окно, наш верх и панель встают по новой
    RECT cr; GetClientRect(h, &cr);
    SendMessageW(h, WM_SIZE, IsZoomed(h) ? SIZE_MAXIMIZED : SIZE_RESTORED, MAKELPARAM(cr.right, cr.bottom));
    if (!IsZoomed(h) && !IsIconic(h)) {   // на всякий случай — лёгкое «пошевеливание» размера, после которого Проводник точно пересчитывает место списка
        RECT wr; GetWindowRect(h, &wr);
        UINT f = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;
        SetWindowPos(h, nullptr, 0, 0, wr.right - wr.left, wr.bottom - wr.top + 1, f);
        SetWindowPos(h, nullptr, 0, 0, wr.right - wr.left, wr.bottom - wr.top, f);
    }
    SendMessageW(h, g_msgPlace, 0, 0);
}
static BOOL CALLBACK RelayoutCb(HWND h, LPARAM) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetPropW(h, PROP_TOPBAR)) { RelayoutTop(h); SendMessageW(h, g_msgNav, 0, 0); }
    return TRUE;
}
void Wh_ModSettingsChanged() {   // настройки поменяли в Windhawk — сразу перерисовать верх
    LoadSettings();
    EnumWindows(ApplySnapCb, 0);
    EnumWindows(RelayoutCb, 0);
    EnumWindows(RedrawBarsCb, 0);
}
