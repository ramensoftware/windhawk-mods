// ==WindhawkMod==
// @id             win7-language-switcher-restorer
// @name           Windows 7/8.1 Language Switcher Restorer
// @description    This mod restores the classic Windows 7 and Windows 8.1 language switcher on Windows 10 and 11
// @version        1.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @architecture   x86-64
// @compilerOptions -lgdi32 -ldwmapi -luxtheme -lole32 -lshell32 -luser32 -lcomctl32 -lshlwapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 Language Switcher Restorer

## About

This mod restores the classic **Windows 7 and 8.1 language switcher** on Windows 10 and Windows 11, enabling rapid switching between keyboard layouts and input languages.

**Important**: This is a **best-effort visual and functional recreation**. The mod intercepts clicks on the taskbar language indicator, as well as the **Win+Space** and **Alt+Shift / Ctrl+Shift** keyboard shortcuts, replacing the modern Windows 10/11 flyout with a fast, lightweight, classic switcher.

The mod has been tested on Windows 10 1809, Windows 10 21H2, Windows 11 23H2, and Windows 11 24H2.

## Screenshots

## Windows 7 Theme

![win7language.PNG](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7language.PNG)

## Windows 8.1 Theme

![win8language.PNG](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8language.PNG)


## Main Features

- **Two classic styles**: selection between the Windows 7 classic menu and the Windows 8.1 modern flyout.
- **Light and dark theme support**: automatic adaptation to system colors and theme.
- **Quick dismissal**: closes upon clicking outside or pressing Escape.
- **Translated into 27 languages**: fully localized interface.
- **Win+Space cycling**: holding Win and pressing Space cycles through layouts without popping up the Start Menu.
- **Alt+Shift and Ctrl+Shift support**: fast switching with auto-repeat suppression and clean modifier sequence tracking.
- **Robust Multi-Threaded Architecture**: modeled on win7-network-flyout-recreation for rock-solid stability and clean reloading.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- switcherStyle: win8
  $name: Switcher Style
  $description: This setting chooses between the Windows 8.1 Modern Flyout or the Windows 7 Classic Menu.
  $options:
    - win8: Windows 8.1 Modern Flyout
    - win7: Windows 7 Classic Menu

- language: auto
  $name: UI Language
  $description: This setting chooses the interface language for options, links, and shortcut tips.
  $options:
    - auto: Automatic (Windows display language)
    - it: Italiano (Italian)
    - en: English (United States)
    - tr: Türkçe (Turkish)
    - fr: Français (French)
    - es: Español (Spanish)
    - pt: Português (Portuguese)
    - zh: 中文 (Simplified Chinese)
    - pl: Polski (Polish)
    - nl: Nederlands (Dutch)
    - de: Deutsch (German)
    - ru: Русский (Russian)
    - ja: 日本語 (Japanese)
    - ko: 한국어 (Korean)
    - ar: العربية (Arabic)
    - sv: Svenska (Swedish)
    - cs: Čeština (Czech)
    - da: Dansk (Danish)
    - fi: Suomi (Finnish)
    - el: Ελληνικά (Greek)
    - he: עברית (Hebrew)
    - hu: Magyar (Hungarian)
    - nb: Norsk (Norwegian)
    - ro: Română (Romanian)
    - sk: Slovenčina (Slovak)
    - uk: Українська (Ukrainian)
    - af: Afrikaans

- themeMode: win8_purple
  $name: Color Theme
  $description: This setting chooses the color scheme used for the selected item and flyout styling.
  $options:
    - win8_purple: Windows 8.1 Purple (#5B2C82)
    - auto: Follow Windows Accent & Theme
    - dark: Dark Mode
    - light: Light Mode
    - custom: Custom Accent Color

- customAccentColor: "#5B2C82"
  $name: Custom Accent Color (Hex)
  $description: This setting sets the hex color used when theme is set to 'Custom' (e.g. #5B2C82 or #0078D7).

- enableWinSpace: true
  $name: Enable Win+Space Cycling
  $description: This setting intercepts Win+Space to cycle through keyboard layouts, applying the selection upon releasing the Windows key.

- enableAltShift: true
  $name: Enable Alt+Shift / Ctrl+Shift Toggle
  $description: This setting intercepts Alt+Shift and Ctrl+Shift to toggle through keyboard layouts.

- hookTrayClicks: true
  $name: Intercept Taskbar Language Button
  $description: This setting intercepts clicks on the taskbar language button / input indicator to show this classic switcher.

- showShortcutHint: true
  $name: Show Shortcut Hint in Footer
  $description: This setting displays 'To switch, press Windows key + Space' at the bottom of the flyout.

- customPreferencesCmd: "ms-settings:regionlanguage"
  $name: Language Preferences Command
  $description: This setting sets the command or URL executed when clicking 'Language preferences'.

- enableCustomHotkey: false
  $name: Enable Ctrl+Shift+L Shortcut
  $description: This setting intercepts Ctrl+Shift+L to open the switcher without changing the current layout.
*/
// ==/WindhawkModSettings==

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WINVER
#define WINVER 0x0602
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <msctf.h>
#include <objbase.h>
#include <string>
#include <vector>
#include <atomic>
#include <algorithm>

#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#include <windhawk_utils.h>
#else
#ifdef __cplusplus
extern "C" {
#endif
void Wh_Log(const wchar_t* format, ...);
int Wh_GetIntSetting(const wchar_t* name);
const wchar_t* Wh_GetStringSetting(const wchar_t* name);
void Wh_FreeStringSetting(const wchar_t* string);
BOOL Wh_SetFunctionHook(void* target, void* hook, void** original);
int Wh_GetModStoragePath(wchar_t* buffer, int max_len);
#ifdef __cplusplus
}
namespace WindhawkUtils {
    template <typename T>
    inline BOOL SetFunctionHook(T target, T hook, T* original) {
        return Wh_SetFunctionHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(hook), reinterpret_cast<void**>(original));
    }
    using WH_SUBCLASSPROC = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
    inline BOOL SetWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass, DWORD_PTR dwRefData = 0) {
        return SetWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(reinterpret_cast<void*>(pfnSubclass)), reinterpret_cast<UINT_PTR>(pfnSubclass), dwRefData);
    }
    inline BOOL RemoveWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass) {
        return RemoveWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(reinterpret_cast<void*>(pfnSubclass)), reinterpret_cast<UINT_PTR>(pfnSubclass));
    }
}
#endif
#endif

#ifndef MDT_EFFECTIVE_DPI
#define MDT_EFFECTIVE_DPI 0
#endif

#define CLICK_DEBOUNCE_MS        400
#define WM_SAFE_CLOSE            (WM_USER + 101)
#define WM_SHOW_FLYOUT           (WM_USER + 102)
#define WM_TOGGLE_FLYOUT_REQUEST (WM_USER + 111)
#define WM_APP_CYCLE_SWITCHER    (WM_USER + 112)
#define WM_APP_CYCLE_AND_SWITCH  (WM_USER + 113)
#define WM_APP_APPLY_SELECTION   (WM_USER + 114)
#define WM_APP_HIDE_SWITCHER     (WM_USER + 115)
#define WM_APP_CLICK_OUTSIDE_TEST (WM_USER + 116)
#define WM_APP_REFRESH_INSTALL   (WM_USER + 117)

static const wchar_t* kFlyoutClassName = L"Windhawk_Win78LanguageFlyout";

// Each load gets a fresh HINSTANCE. Never register the window class against
// explorer.exe — that registration would survive unload with lpfnWndProc
// pointing into the unmapped image.
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

typedef struct {
    HANDLE hWorkerThread;
    DWORD dwWorkerThreadId;
    HANDLE hHookThread;
    DWORD dwHookThreadId;
    HANDLE hWorkerReadyEvent;
    HANDLE hHookReadyEvent;
    volatile LONG refCount;
    volatile LONG isUninitializing;
    CRITICAL_SECTION csLock;
} ModContext;

// Per-load flag: the flyout window class must only be registered once per
// mod load. If a previous load's UnregisterClassW failed (a window of the
// class was still alive), the next load's RegisterClassW will fail with
// ERROR_CLASS_ALREADY_EXISTS — in that case we must NOT reuse the stale
// class, since its WNDPROC points into a previous, by-then-unmapped image.
static bool g_flyoutClassRegistered = false;

static ModContext g_Ctx;
static BOOL g_Initialized = FALSE;

static std::atomic<HWND> g_hFlyoutWnd{nullptr};
static std::atomic<DWORD> g_dwFlyoutOwnerThreadId{0};
static std::atomic<HWND> g_targetWindow{nullptr};
static std::atomic<DWORD> g_lastInactiveTick{0};
static std::atomic<HWND> g_hClickedTaskbar{nullptr};

static HWND G_hSubclassedToolbar = NULL;
static HWND G_hSubclassedIndicator = NULL;
// All secondary taskbars (one per additional monitor) that we've subclassed.
// The primary taskbar lives in G_hSubclassedToolbar; every secondary taskbar's
// toolbar needs its own entry so multi-monitor setups all get interception.
static std::vector<HWND> G_hSubclassedSecToolbars;

static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;

typedef int (WINAPI *GdiplusStartupFunc)(ULONG_PTR*, const void*, void*);
typedef void (WINAPI *GdiplusShutdownFunc)(ULONG_PTR);
typedef int (WINAPI *GdipCreateFromHDCFunc)(HDC, void**);
typedef int (WINAPI *GdipDeleteGraphicsFunc)(void*);
typedef int (WINAPI *GdipSetSmoothingModeFunc)(void*, int);
typedef int (WINAPI *GdipSetPixelOffsetModeFunc)(void*, int);
typedef int (WINAPI *GdipCreatePathFunc)(int, void**);
typedef int (WINAPI *GdipDeletePathFunc)(void*);
typedef int (WINAPI *GdipAddPathPolygonFunc)(void*, const void*, int);
typedef int (WINAPI *GdipCreateSolidFillFunc)(DWORD, void**);
typedef int (WINAPI *GdipDeleteBrushFunc)(void*);
typedef int (WINAPI *GdipFillPathFunc)(void*, void*, void*);
typedef int (WINAPI *GdipCreatePen1Func)(DWORD, float, int, void**);
typedef int (WINAPI *GdipDeletePenFunc)(void*);
typedef int (WINAPI *GdipSetPenLineJoinFunc)(void*, int);
typedef int (WINAPI *GdipDrawPathFunc)(void*, void*, void*);

static GdipCreateFromHDCFunc pGdipCreateFromHDC = NULL;
static GdipDeleteGraphicsFunc pGdipDeleteGraphics = NULL;
static GdipSetSmoothingModeFunc pGdipSetSmoothingMode = NULL;
static GdipSetPixelOffsetModeFunc pGdipSetPixelOffsetMode = NULL;
static GdipCreatePathFunc pGdipCreatePath = NULL;
static GdipDeletePathFunc pGdipDeletePath = NULL;
static GdipAddPathPolygonFunc pGdipAddPathPolygon = NULL;
static GdipCreateSolidFillFunc pGdipCreateSolidFill = NULL;
static GdipDeleteBrushFunc pGdipDeleteBrush = NULL;
static GdipFillPathFunc pGdipFillPath = NULL;
static GdipCreatePen1Func pGdipCreatePen1 = NULL;
static GdipDeletePenFunc pGdipDeletePen = NULL;
static GdipSetPenLineJoinFunc pGdipSetPenLineJoin = NULL;
static GdipDrawPathFunc pGdipDrawPath = NULL;

static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryW(L"gdiplus.dll");
    if (!g_hGdiPlus) return FALSE;

    pGdipCreateFromHDC = (GdipCreateFromHDCFunc)GetProcAddress(g_hGdiPlus, "GdipCreateFromHDC");
    pGdipDeleteGraphics = (GdipDeleteGraphicsFunc)GetProcAddress(g_hGdiPlus, "GdipDeleteGraphics");
    pGdipSetSmoothingMode = (GdipSetSmoothingModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetSmoothingMode");
    pGdipSetPixelOffsetMode = (GdipSetPixelOffsetModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetPixelOffsetMode");
    pGdipCreatePath = (GdipCreatePathFunc)GetProcAddress(g_hGdiPlus, "GdipCreatePath");
    pGdipDeletePath = (GdipDeletePathFunc)GetProcAddress(g_hGdiPlus, "GdipDeletePath");
    pGdipAddPathPolygon = (GdipAddPathPolygonFunc)GetProcAddress(g_hGdiPlus, "GdipAddPathPolygon");
    pGdipCreateSolidFill = (GdipCreateSolidFillFunc)GetProcAddress(g_hGdiPlus, "GdipCreateSolidFill");
    pGdipDeleteBrush = (GdipDeleteBrushFunc)GetProcAddress(g_hGdiPlus, "GdipDeleteBrush");
    pGdipFillPath = (GdipFillPathFunc)GetProcAddress(g_hGdiPlus, "GdipFillPath");
    pGdipCreatePen1 = (GdipCreatePen1Func)GetProcAddress(g_hGdiPlus, "GdipCreatePen1");
    pGdipDeletePen = (GdipDeletePenFunc)GetProcAddress(g_hGdiPlus, "GdipDeletePen");
    pGdipSetPenLineJoin = (GdipSetPenLineJoinFunc)GetProcAddress(g_hGdiPlus, "GdipSetPenLineJoin");
    pGdipDrawPath = (GdipDrawPathFunc)GetProcAddress(g_hGdiPlus, "GdipDrawPath");

    GdiplusStartupFunc pStartup = (GdiplusStartupFunc)GetProcAddress(g_hGdiPlus, "GdiplusStartup");
    if (!pStartup) {
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        return FALSE;
    }

    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        return FALSE;
    }
    return TRUE;
}

static void ShutdownGdiPlusRendering() {
    if (g_hGdiPlus) {
        GdiplusShutdownFunc pShutdown = (GdiplusShutdownFunc)GetProcAddress(g_hGdiPlus, "GdiplusShutdown");
        if (pShutdown && g_gdiplusToken) pShutdown(g_gdiplusToken);
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL;
        g_gdiplusToken = 0;
    }
}

struct LangAbbrevEntry {
    WORD langId;
    const wchar_t* abbrev;
};

static const LangAbbrevEntry g_LangAbbrevs[] = {
    { LANG_AFRIKAANS,   L"AFR" },
    { LANG_ALBANIAN,    L"ALB" },
    { LANG_ARABIC,      L"ARA" },
    { LANG_ARMENIAN,    L"ARM" },
    { LANG_ASSAMESE,    L"ASM" },
    { LANG_AZERI,       L"AZE" },
    { LANG_BASQUE,      L"BSQ" },
    { LANG_BELARUSIAN,  L"BEL" },
    { LANG_BENGALI,     L"BEN" },
    { LANG_BULGARIAN,   L"BGR" },
    { LANG_CATALAN,     L"CAT" },
    { LANG_CHINESE,     L"CHS" },
    { LANG_CROATIAN,    L"HRV" },
    { LANG_CZECH,       L"CSY" },
    { LANG_DANISH,      L"DAN" },
    { LANG_DUTCH,       L"NLD" },
    { LANG_ENGLISH,     L"ENG" },
    { LANG_ESTONIAN,    L"ETI" },
    { LANG_FAEROESE,    L"FRO" },
    { LANG_FARSI,       L"FAR" },
    { LANG_FINNISH,     L"FIN" },
    { LANG_FRENCH,      L"FRA" },
    { LANG_GEORGIAN,    L"GEO" },
    { LANG_GERMAN,      L"DEU" },
    { LANG_GREEK,       L"ELL" },
    { LANG_GUJARATI,    L"GUJ" },
    { LANG_HEBREW,      L"HEB" },
    { LANG_HINDI,       L"HIN" },
    { LANG_HUNGARIAN,   L"HUN" },
    { LANG_ICELANDIC,   L"ISL" },
    { LANG_INDONESIAN,  L"IND" },
    { LANG_ITALIAN,     L"ITA" },
    { LANG_JAPANESE,    L"JPN" },
    { LANG_KANNADA,     L"KAN" },
    { LANG_KASHMIRI,    L"KSH" },
    { LANG_KAZAK,       L"KAZ" },
    { LANG_KONKANI,     L"KOK" },
    { LANG_KOREAN,      L"KOR" },
    { LANG_LATVIAN,     L"LVI" },
    { LANG_LITHUANIAN,  L"LTH" },
    { LANG_MACEDONIAN,  L"MKI" },
    { LANG_MALAY,       L"MSL" },
    { LANG_MALAYALAM,   L"MAL" },
    { LANG_MANIPURI,    L"MPI" },
    { LANG_MARATHI,     L"MAR" },
    { LANG_NEPALI,      L"NEP" },
    { LANG_NORWEGIAN,   L"NOR" },
    { LANG_ORIYA,       L"ORI" },
    { LANG_POLISH,      L"PLK" },
    { LANG_PORTUGUESE,  L"PTG" },
    { LANG_PUNJABI,     L"PAN" },
    { LANG_ROMANIAN,    L"ROM" },
    { LANG_RUSSIAN,     L"RUS" },
    { LANG_SANSKRIT,    L"SAN" },
    { LANG_SERBIAN,     L"SRB" },
    { LANG_SLOVAK,      L"SLK" },
    { LANG_SLOVENIAN,   L"SLV" },
    { LANG_SPANISH,     L"ESP" },
    { LANG_SWAHILI,     L"SWK" },
    { LANG_SWEDISH,     L"SVE" },
    { LANG_TAMIL,       L"TAM" },
    { LANG_TATAR,       L"TTT" },
    { LANG_TELUGU,      L"TEL" },
    { LANG_THAI,        L"THA" },
    { LANG_TURKISH,     L"TRK" },
    { LANG_UKRAINIAN,   L"UKR" },
    { LANG_URDU,        L"URD" },
    { LANG_UZBEK,       L"UZB" },
    { LANG_VIETNAMESE,  L"VIT" },
};

struct LocalizedUiText {
    const wchar_t* langTag;
    const wchar_t* preferences;
    const wchar_t* shortcutHint;
    const wchar_t* showLanguageBar;
};

static const LocalizedUiText kLocalizedStrings[] = {
    { L"it", L"Preferenze lingua", L"Per passare da una lingua all'altra, premi tasto Windows + Spazio", L"Mostra barra della lingua" },
    { L"en", L"Language preferences", L"To switch, press Windows key + Space", L"Show the Language bar" },
    { L"tr", L"Dil tercihleri", L"Geçiş yapmak için Windows tuşu + Boşluk tuşuna basın", L"Dil çubuğunu göster" },
    { L"fr", L"Préférences linguistiques", L"Pour basculer, appuyez sur la touche Windows + Espace", L"Afficher la barre des langues" },
    { L"es", L"Preferencias de idioma", L"Para cambiar, presione la tecla Windows + Barra espaciadora", L"Mostrar la barra de idioma" },
    { L"pt", L"Preferências de idioma", L"Para alternar, pressione a tecla Windows + Espaço", L"Mostrar a barra de idiomas" },
    { L"zh", L"语言首选项", L"若要切换，请按 Windows 徽标键 + 空格键", L"显示语言栏" },
    { L"pl", L"Preferencje językowe", L"Aby przełączyć, naciśnij klawisz Windows + Spacja", L"Pokaż pasek języka" },
    { L"nl", L"Taalvoorkeuren", L"Druk op Windows-toets + Spatiebalk om te wisselen", L"Taalbalk weergeven" },
    { L"de", L"Spracheinstellungen", L"Drücken Sie Windows-Taste + Leertaste, um zu wechseln", L"Sprachenleiste anzeigen" },
    { L"ru", L"Настройки языка", L"Для переключения нажмите клавишу Windows + Пробел", L"Отобразить языковую панель" },
    { L"ja", L"言語の設定", L"切り替えるには、Windows ロゴ キー + Space キーを押します", L"言語バーを表示" },
    { L"ko", L"언어 기본 설정", L"전환하려면 Windows 키 + 스페이스바를 누르세요", L"언어 표시줄 표시" },
    { L"ar", L"تفضيلات اللغة", L"للتبديل، اضغط على مفتاح Windows + المسافة", L"إظهار شريط اللغة" },
    { L"sv", L"Språkinställningar", L"Tryck på Windows-tangenten + Blanksteg för att växla", L"Visa språkfältet" },
    { L"cs", L"Jazykové předvolby", L"Chcete-li přepnout, stiskněte klávesu Windows + Mezerník", L"Zobrazit panel jazyků" },
    { L"da", L"Sprogindstillinger", L"Tryk på Windows-tasten + Mellemrum for at skifte", L"Vis proceslinjen Sprog" },
    { L"fi", L"Kieliasetukset", L"Vaihda painamalla Windows-näppäintä + välilyöntiä", L"Näytä kielipalkki" },
    { L"el", L"Προτιμήσεις γλώσσας", L"Για εναλλαγή, πατήστε το πλήκτρο Windows + Διαστήματος", L"Εμφάνιση της γραμμής γλώσσας" },
    { L"he", L"העדפות שפה", L"כדי לעבור, לחץ על מקש Windows + רווח", L"הצג את סרגל השפה" },
    { L"hu", L"Nyelvi beállítások", L"A váltáshoz nyomja le a Windows billentyű + Szóköz billentyűt", L"Nyelvi sáv megjelenítése" },
    { L"nb", L"Språkinnstillinger", L"Trykk på Windows-tasten + Mellomrom for å bytte", L"Vis språklinjen" },
    { L"ro", L"Preferințe de limbă", L"Pentru a comuta, apăsați tasta Windows + Spațiu", L"Afișare bară de limbă" },
    { L"sk", L"Jazykové predvoľby", L"Ak chcete prepnúť, stlačte kláves s logom Windows + Medzerník", L"Zobraziť panel jazykov" },
    { L"uk", L"Мовні параметри", L"Щоб переключити, натисніть клавішу Windows + Пробіл", L"Відобразити мовну панель" },
    { L"af", L"Taalvoorkeure", L"Vir maklike wisseling, druk Windows-sleutel + Spasie", L"Wys die taalbalk" }
};

struct KeyboardLayoutItem {
    HKL hkl = nullptr;
    LANGID langId = 0;
    std::wstring langName;
    std::wstring langAbbrev;
    std::wstring subAbbrev;
    std::wstring layoutName;
    std::wstring klid;
    bool isCurrent = false;
    size_t sameLangCount = 1;
};

enum class SwitcherStyle { Win8, Win7 };
enum class ThemeMode { Win8Purple, Auto, Dark, Light, Custom };

struct ModSettings {
    SwitcherStyle switcherStyle = SwitcherStyle::Win8;
    ThemeMode themeMode = ThemeMode::Win8Purple;
    std::wstring uiLanguage = L"auto";
    COLORREF customAccentColor = RGB(91, 44, 130);
    std::wstring customPreferencesCmd = L"ms-settings:regionlanguage";
    bool enableWinSpace = true;
    bool enableAltShift = true;
    bool hookTrayClicks = true;
    bool showShortcutHint = true;
    bool enableCustomHotkey = false;
};

static ModSettings g_settings;
static std::vector<KeyboardLayoutItem> g_layouts;
static size_t g_selectedIndex = 0;
static int g_hoveredIndex = -1;
static bool g_hoveredFooter = false;
static int g_hoveredWin7Index = -1;

// Hot-path flags: read without taking csLock or copying wstrings.
static std::atomic<bool> g_enableWinSpace{true};
static std::atomic<bool> g_enableAltShift{true};
static std::atomic<bool> g_hookTrayClicks{true};
static std::atomic<bool> g_enableCustomHotkey{false};

// Set after the worker thread confirms this explorer instance is the one that
// owns Shell_TrayWnd. Re-evaluated periodically (the owner can change during an
// Explorer/taskbar restart), and gates the LL hooks and ShowWindow hook so a
// second explorer instance never double-handles input.
static std::atomic<bool> g_isMainShell{false};
static std::atomic<bool> g_isWinSpaceCycling{false};
static std::atomic<bool> g_altPressed{false};
static std::atomic<bool> g_shiftPressed{false};
static std::atomic<bool> g_ctrlPressed{false};
static std::atomic<bool> g_interveningKeyPressed{false};
static std::atomic<bool> g_altShiftChordArmed{false};
static std::atomic<bool> g_ctrlShiftChordArmed{false};

// Forward declarations: these are defined further down but are used by the
// worker / hook threads above their definitions.
static bool IsExplorerProcess();
static bool IsMainExplorerShell();
static void EvaluateShellRole();
static bool WinKeysReleased();

class ScopedHookHolder {
public:
    ScopedHookHolder() = default;
    ~ScopedHookHolder() { reset(); }
    void reset(HHOOK h = NULL) {
        if (h_ && h_ != h) { UnhookWindowsHookEx(h_); }
        h_ = h;
    }
    HHOOK get() const { return h_; }
private:
    HHOOK h_ = NULL;
};

[[clang::no_destroy]] static ScopedHookHolder g_keyboardHook;
[[clang::no_destroy]] static ScopedHookHolder g_mouseHook;

static HINSTANCE GetModInstance() {
    return HINST_THISCOMPONENT;
}

static ModSettings GetSettingsSnapshot() {
    EnterCriticalSection(&g_Ctx.csLock);
    ModSettings s = g_settings;
    LeaveCriticalSection(&g_Ctx.csLock);
    return s;
}

static HWND AtomicLoadHwnd(const std::atomic<HWND>& a) {
    return a.load(std::memory_order_acquire);
}

static COLORREF ParseHexColor(const std::wstring& hexStr, COLORREF fallback) {
    try {
        if (hexStr.empty()) return fallback;
        const wchar_t* p = hexStr.c_str();
        while (*p == L' ' || *p == L'\t') p++;
        if (*p == L'#') p++;
        if (!*p) return fallback;

        wchar_t* endPtr = nullptr;
        unsigned long val = wcstoul(p, &endPtr, 16);
        if (endPtr == p) return fallback;

        BYTE r = static_cast<BYTE>((val >> 16) & 0xFF);
        BYTE g = static_cast<BYTE>((val >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>(val & 0xFF);
        return RGB(r, g, b);
    } catch (...) {
        return fallback;
    }
}

static COLORREF GetSystemAccentColor() {
    try {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
            BYTE r = static_cast<BYTE>((color >> 16) & 0xFF);
            BYTE g = static_cast<BYTE>((color >> 8) & 0xFF);
            BYTE b = static_cast<BYTE>(color & 0xFF);
            return RGB(r, g, b);
        }
    } catch (...) {}
    return RGB(91, 44, 130);
}

static bool IsDarkModeActive() {
    try {
        HKEY hKey = NULL;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD appsUseLightTheme = 1;
            DWORD size = sizeof(appsUseLightTheme);
            RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&appsUseLightTheme), &size);
            RegCloseKey(hKey);
            return appsUseLightTheme == 0;
        }
    } catch (...) {}
    return false;
}

static int ScaleForDpi(int value, UINT dpi) {
    if (dpi == 0) dpi = 96;
    return MulDiv(value, static_cast<int>(dpi), 96);
}

static UINT GetMonitorDpi(HMONITOR hMon) {
    if (hMon) {
        HMODULE hShcore = LoadLibraryW(L"shcore.dll");
        if (hShcore) {
            using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
            auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
                GetProcAddress(hShcore, "GetDpiForMonitor"));
            if (pGetDpiForMonitor) {
                UINT dpiX = 0, dpiY = 0;
                if (SUCCEEDED(pGetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiY > 0) {
                    FreeLibrary(hShcore);
                    return dpiY;
                }
            }
            FreeLibrary(hShcore);
        }
    }
    HDC dc = GetDC(NULL);
    if (dc) {
        int dpi = GetDeviceCaps(dc, LOGPIXELSY);
        ReleaseDC(NULL, dc);
        if (dpi > 0) return static_cast<UINT>(dpi);
    }
    return 96;
}

static UINT GetWindowDpi(HWND hwnd) {
    try {
        HMODULE hUser = GetModuleHandleW(L"user32.dll");
        if (hUser) {
            auto pGetDpi = reinterpret_cast<UINT(WINAPI*)(HWND)>(reinterpret_cast<void*>(GetProcAddress(hUser, "GetDpiForWindow")));
            if (pGetDpi && hwnd && IsWindow(hwnd)) {
                UINT dpi = pGetDpi(hwnd);
                if (dpi > 0) return dpi;
            }
        }
    } catch (...) {}
    HMONITOR hMon = (hwnd && IsWindow(hwnd)) ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) : NULL;
    return GetMonitorDpi(hMon);
}
static bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    WCHAR className[64] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) == 0) return false;
    return (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}
static void GetLocalizedFooterStrings(std::wstring& outPreferences, std::wstring& outHint, std::wstring* outShowBar = nullptr) {
    try {
        ModSettings settings = GetSettingsSnapshot();
        std::wstring targetTag = settings.uiLanguage;
        if (targetTag.empty() || targetTag == L"auto") {
            wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
            if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName)) > 0) {
                targetTag = localeName;
            } else {
                targetTag = L"en";
            }
        }

        for (const auto& item : kLocalizedStrings) {
            if (item.langTag && wcsncmp(targetTag.c_str(), item.langTag, wcslen(item.langTag)) == 0) {
                outPreferences = item.preferences ? item.preferences : L"Language preferences";
                outHint = item.shortcutHint ? item.shortcutHint : L"To switch, press Windows key + Space";
                if (outShowBar) *outShowBar = item.showLanguageBar ? item.showLanguageBar : L"Show the Language bar";
                return;
            }
        }

        outPreferences = L"Language preferences";
        outHint = L"To switch, press Windows key + Space";
        if (outShowBar) *outShowBar = L"Show the Language bar";
    } catch (...) {
        outPreferences = L"Language preferences";
        outHint = L"To switch, press Windows key + Space";
        if (outShowBar) *outShowBar = L"Show the Language bar";
    }
}

static bool IsCaseInsensitiveSubstr(const std::wstring& str, const std::wstring& sub) {
    if (sub.empty()) return true;
    if (sub.length() > str.length()) return false;
    auto it = std::search(
        str.begin(), str.end(),
        sub.begin(), sub.end(),
        [](wchar_t ch1, wchar_t ch2) { return towlower(ch1) == towlower(ch2); }
    );
    return it != str.end();
}

static std::wstring FormatWin7LayoutItemText(const KeyboardLayoutItem& item, size_t sameLangCount) {
    std::wstring text = item.langAbbrev + L"  " + item.langName;
    if (sameLangCount > 1 && !item.layoutName.empty()) {
        if (!IsCaseInsensitiveSubstr(item.langName, item.layoutName) &&
            !IsCaseInsensitiveSubstr(item.layoutName, item.langName)) {
            text += L" (" + item.layoutName + L")";
        }
    }
    return text;
}

static std::wstring GetLangAbbrev(LANGID langId) {
    WORD priLang = PRIMARYLANGID(langId);
    for (const auto& item : g_LangAbbrevs) {
        if (item.langId == priLang) {
            return item.abbrev;
        }
    }
    wchar_t abbrBuf[16] = {};
    if (GetLocaleInfoW(langId, LOCALE_SABBREVLANGNAME, abbrBuf, ARRAYSIZE(abbrBuf)) > 0) {
        for (wchar_t* p = abbrBuf; *p; ++p) *p = towupper(*p);
        return abbrBuf;
    }
    return L"ENG";
}

static std::wstring GetLayoutDisplayName(const std::wstring& klid) {
    try {
        if (klid.empty()) return L"";
        std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\" + klid;
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t displayBuf[512] = {};
            DWORD cbData = sizeof(displayBuf);
            if (RegQueryValueExW(hKey, L"Layout Display Name", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(displayBuf), &cbData) == ERROR_SUCCESS && displayBuf[0]) {
                wchar_t resolvedBuf[512] = {};
                if (SUCCEEDED(SHLoadIndirectString(displayBuf, resolvedBuf, ARRAYSIZE(resolvedBuf), nullptr)) && resolvedBuf[0]) {
                    RegCloseKey(hKey);
                    return resolvedBuf;
                }
            }

            cbData = sizeof(displayBuf);
            if (RegQueryValueExW(hKey, L"Layout Text", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(displayBuf), &cbData) == ERROR_SUCCESS && displayBuf[0]) {
                RegCloseKey(hKey);
                return displayBuf;
            }
            RegCloseKey(hKey);
        }
    } catch (...) {}
    return L"";
}

static std::wstring GetSubstituteKlid(const std::wstring& klid) {
    try {
        if (klid.empty()) return klid;
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Keyboard Layout\\Substitutes", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t subBuf[64] = {};
            DWORD cb = sizeof(subBuf);
            if (RegQueryValueExW(hKey, klid.c_str(), nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(subBuf), &cb) == ERROR_SUCCESS && subBuf[0]) {
                RegCloseKey(hKey);
                return subBuf;
            }
            RegCloseKey(hKey);
        }
    } catch (...) {}
    return klid;
}

// Some keyboard layouts carry a hardware "Layout Id" in the HKL's high word
// (top nibble == 0xF) rather than a KLID prefix. Those must be resolved by
// scanning HKLM\SYSTEM\CurrentControlSet\Control\Keyboard Layouts for a
// subkey whose "Layout Id" value matches devId & 0x0FFF; the subkey name is
// the real KLID. Returns empty when no match is found.
static std::wstring FindKlidByLayoutId(WORD layoutId) {
    std::wstring result;
    try {
        if (layoutId == 0) return result;
        HKEY hRoot = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
                          0, KEY_READ, &hRoot) != ERROR_SUCCESS) {
            return result;
        }

        for (DWORD i = 0; ; ++i) {
            wchar_t subKey[256] = {};
            DWORD subLen = ARRAYSIZE(subKey);
            FILETIME ft{};
            if (RegEnumKeyExW(hRoot, i, subKey, &subLen, nullptr, nullptr, nullptr, &ft) != ERROR_SUCCESS) {
                break;
            }

            HKEY hSub = nullptr;
            if (RegOpenKeyExW(hRoot, subKey, 0, KEY_READ, &hSub) == ERROR_SUCCESS) {
                wchar_t idBuf[16] = {};
                DWORD cb = sizeof(idBuf);
                if (RegQueryValueExW(hSub, L"Layout Id", nullptr, nullptr,
                                     reinterpret_cast<LPBYTE>(idBuf), &cb) == ERROR_SUCCESS && idBuf[0]) {
                    wchar_t* end = nullptr;
                    unsigned long parsed = wcstoul(idBuf, &end, 16);
                    if (end != idBuf && (parsed & 0xFFFF) == layoutId) {
                        result = subKey;
                        RegCloseKey(hSub);
                        break;
                    }
                }
                RegCloseKey(hSub);
            }
        }
        RegCloseKey(hRoot);
    } catch (...) {}
    return result;
}

static void RefreshKeyboardLayouts() {
    try {
        UINT count = GetKeyboardLayoutList(0, nullptr);
        if (count == 0 || count > 64) return;

        std::vector<HKL> hkls(count);
        UINT fetched = GetKeyboardLayoutList(count, hkls.data());
        if (fetched == 0) return;
        hkls.resize(fetched);

        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
        HWND hFore = AtomicLoadHwnd(g_targetWindow);
        if (!hFore || !IsWindow(hFore) || hFore == hFlyout) {
            hFore = GetForegroundWindow();
            if (hFore == hFlyout) hFore = nullptr;
        }

        DWORD dwTid = (hFore ? GetWindowThreadProcessId(hFore, nullptr) : 0);
        HKL activeHkl = (dwTid != 0 ? GetKeyboardLayout(dwTid) : GetKeyboardLayout(0));

        std::vector<KeyboardLayoutItem> newLayouts;
        newLayouts.reserve(hkls.size());
        size_t foundActiveIndex = 0;

        for (size_t i = 0; i < hkls.size(); ++i) {
            HKL hkl = hkls[i];
            if (!hkl) continue;

            KeyboardLayoutItem item;
            item.hkl = hkl;
            item.langId = LOWORD(reinterpret_cast<uintptr_t>(hkl));
            item.isCurrent = (hkl == activeHkl);

            if (item.isCurrent) {
                foundActiveIndex = i;
            }

            // Derive the KLID from the HKL. The low word is the LANGID, the
            // high word is the device/layout id. Standard layouts have a zero
            // (or language-equal) high word; a 0xFxxx high word is a hardware
            // "Layout Id" that must be resolved through the registry.
            WORD dev = HIWORD(reinterpret_cast<uintptr_t>(hkl));
            WORD lang = LOWORD(reinterpret_cast<uintptr_t>(hkl));
            if ((dev & 0xF000) == 0xF000) {
                // Layout-Id variant: resolve the real KLID from the registry.
                std::wstring klid = FindKlidByLayoutId(dev & 0x0FFF);
                if (klid.empty()) {
                    wchar_t buf[16] = {};
                    wsprintfW(buf, L"%08X", static_cast<UINT>(lang));
                    klid = buf;
                }
                item.klid = klid;
            } else if (dev == 0 || dev == lang) {
                wchar_t klidBuf[16] = {};
                wsprintfW(klidBuf, L"%08X", static_cast<UINT>(lang));
                item.klid = klidBuf;
            } else {
                wchar_t klidBuf[16] = {};
                wsprintfW(klidBuf, L"%04X%04X", static_cast<UINT>(dev), static_cast<UINT>(lang));
                item.klid = klidBuf;
            }

            std::wstring effectiveKlid = GetSubstituteKlid(item.klid);

            wchar_t langNameBuf[256] = {};
            if (GetLocaleInfoW(item.langId, LOCALE_SLOCALIZEDDISPLAYNAME, langNameBuf, ARRAYSIZE(langNameBuf)) > 0) {
                item.langName = langNameBuf;
            } else if (GetLocaleInfoW(item.langId, LOCALE_SENGLISHDISPLAYNAME, langNameBuf, ARRAYSIZE(langNameBuf)) > 0) {
                item.langName = langNameBuf;
            } else {
                item.langName = L"Language";
            }

            if (!item.langName.empty() && iswlower(item.langName[0])) {
                item.langName[0] = towupper(item.langName[0]);
            }

            item.langAbbrev = GetLangAbbrev(item.langId);

            std::wstring layoutDesc = GetLayoutDisplayName(effectiveKlid);
            if (layoutDesc.empty()) {
                layoutDesc = GetLayoutDisplayName(item.klid);
            }
            if (layoutDesc.empty()) {
                layoutDesc = item.langName;
            }
            item.layoutName = layoutDesc;

            newLayouts.push_back(std::move(item));
        }

        for (size_t i = 0; i < newLayouts.size(); ++i) {
            size_t sameLangCount = 0;
            for (size_t j = 0; j < newLayouts.size(); ++j) {
                if (newLayouts[i].langAbbrev == newLayouts[j].langAbbrev ||
                    PRIMARYLANGID(newLayouts[i].langId) == PRIMARYLANGID(newLayouts[j].langId)) {
                    sameLangCount++;
                }
            }
            newLayouts[i].sameLangCount = sameLangCount;

            if (sameLangCount > 1) {
                std::wstring upperLayout = newLayouts[i].layoutName;
                for (auto& c : upperLayout) c = towupper(c);

                if (upperLayout.find(L"INTERNATIONAL") != std::wstring::npos || upperLayout.find(L"INTL") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"INTL";
                } else if (upperLayout.find(L"UNITED STATES") != std::wstring::npos || upperLayout.find(L"US") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"US";
                } else if (upperLayout.find(L"DVORAK") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"DV";
                } else if (upperLayout.find(L"UNITED KINGDOM") != std::wstring::npos || upperLayout.find(L"UK") != std::wstring::npos) {
                    newLayouts[i].subAbbrev = L"UK";
                } else {
                    std::wstring tag;
                    bool newWord = true;
                    for (wchar_t ch : newLayouts[i].layoutName) {
                        if (iswalpha(ch)) {
                            if (newWord && tag.size() < 4) {
                                tag.push_back(towupper(ch));
                                newWord = false;
                            }
                        } else {
                            newWord = true;
                        }
                    }
                    newLayouts[i].subAbbrev = tag.empty() ? L"1" : tag;
                }
            }
        }

        EnterCriticalSection(&g_Ctx.csLock);
        g_layouts = std::move(newLayouts);
        // While a Win+Space cycle is in progress, the target app's layout
        // hasn't actually changed yet (switchImmediately is false for that
        // path), so re-syncing here would keep snapping g_selectedIndex
        // back to the still-active layout and make cycling a no-op. Only
        // re-sync from the system's active layout outside of a cycle.
        if (!g_isWinSpaceCycling.load(std::memory_order_relaxed)) {
            g_selectedIndex = (foundActiveIndex < g_layouts.size()) ? foundActiveIndex : 0;
        }
        LeaveCriticalSection(&g_Ctx.csLock);
    } catch (...) {}
}

static void SwitchToLayout(size_t index) {
    HKL targetHkl = nullptr;
    HWND hTarget = nullptr;

    try {
        EnterCriticalSection(&g_Ctx.csLock);
        if (index >= g_layouts.size()) {
            LeaveCriticalSection(&g_Ctx.csLock);
            return;
        }
        targetHkl = g_layouts[index].hkl;
        g_selectedIndex = index;
        for (size_t i = 0; i < g_layouts.size(); ++i) {
            g_layouts[i].isCurrent = (i == index);
        }
        LeaveCriticalSection(&g_Ctx.csLock);

        if (!targetHkl) return;

        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
        hTarget = AtomicLoadHwnd(g_targetWindow);
        
        if (!hTarget || !IsWindow(hTarget) || hTarget == hFlyout || IsTaskbarWindow(hTarget)) {
            hTarget = GetForegroundWindow();
            if (!hTarget || IsTaskbarWindow(hTarget) || hTarget == hFlyout) {
                hTarget = GetActiveWindow();
                if (!hTarget || IsTaskbarWindow(hTarget) || hTarget == hFlyout) {
                    GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
                    if (GetGUIThreadInfo(0, &gti)) {
                        if (gti.hwndActive && !IsTaskbarWindow(gti.hwndActive) && gti.hwndActive != hFlyout) {
                            hTarget = gti.hwndActive;
                        } else if (gti.hwndFocus && !IsTaskbarWindow(gti.hwndFocus) && gti.hwndFocus != hFlyout) {
                            hTarget = gti.hwndFocus;
                        }
                    }
                }
            }
            
            if (hTarget && !IsTaskbarWindow(hTarget) && hTarget != hFlyout) {
                g_targetWindow.store(hTarget, std::memory_order_release);
            } else {
                Wh_Log(L"SwitchToLayout: No valid target window found");
                return;
            }
        }

        DWORD dwTargetThreadId = GetWindowThreadProcessId(hTarget, nullptr);
        
        if (hTarget && IsWindow(hTarget) && dwTargetThreadId != 0) {
            HKL currentLayout = GetKeyboardLayout(dwTargetThreadId);
            
            if (currentLayout != targetHkl) {
                // Try ActivateKeyboardLayout (returns HKL, not BOOL)
                ActivateKeyboardLayout(targetHkl, KLF_SETFORPROCESS);
                
                // Verify if change succeeded
                if (GetKeyboardLayout(dwTargetThreadId) != targetHkl) {
                    // Fallback: try message-based approach
                    if (!PostMessageW(hTarget, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(targetHkl))) {
                        SendMessageW(hTarget, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(targetHkl));
                    }
                    // Final fallback with reset flag
                    if (GetKeyboardLayout(dwTargetThreadId) != targetHkl) {
                        ActivateKeyboardLayout(targetHkl, KLF_SETFORPROCESS | KLF_RESET);
                    }
                }
                
                Wh_Log(L"SwitchToLayout: Changed to layout %p for thread %d (window: %p)", 
                       targetHkl, dwTargetThreadId, hTarget);
            }
        }
    } catch (...) {
        Wh_Log(L"SwitchToLayout: Exception occurred");
    }
}
void ToggleFlyoutWindow();
void ShowFlyoutWindow();
static void CycleSwitcher(bool forward, bool switchImmediately, bool showFlyout);
static void PaintWin8Flyout(HWND hwnd, HDC hdc) {
    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) return;

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0) return;

    UINT dpi = GetWindowDpi(hwnd);
    const int itemHeight = ScaleForDpi(58, dpi);
    const int badgeWidth = ScaleForDpi(62, dpi);
    const int paddingX = ScaleForDpi(16, dpi);
    const int separatorHeight = ScaleForDpi(1, dpi);

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) return;
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, width, height);
    if (!memBmp) { DeleteDC(memDC); return; }
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    SetBkMode(memDC, TRANSPARENT);

    ModSettings settings = GetSettingsSnapshot();
    bool isDark = (settings.themeMode == ThemeMode::Dark) ||
                  (settings.themeMode == ThemeMode::Auto && IsDarkModeActive());

    COLORREF colBg = isDark ? RGB(32, 32, 32) : RGB(242, 242, 242);
    COLORREF colTextNormal = isDark ? RGB(240, 240, 240) : RGB(0, 0, 0);
    COLORREF colTextSub = isDark ? RGB(160, 160, 160) : RGB(96, 96, 96);
    COLORREF colHoverBg = isDark ? RGB(50, 50, 50) : RGB(220, 220, 220);
    COLORREF colBorder = isDark ? RGB(60, 60, 60) : RGB(190, 190, 190);
    COLORREF colSeparator = isDark ? RGB(55, 55, 55) : RGB(200, 200, 200);

    COLORREF colSelectedBg = RGB(91, 44, 130);
    if (settings.themeMode == ThemeMode::Auto) {
        colSelectedBg = GetSystemAccentColor();
    } else if (settings.themeMode == ThemeMode::Custom) {
        colSelectedBg = settings.customAccentColor;
    }
    COLORREF colSelectedText = RGB(255, 255, 255);
    COLORREF colSelectedSubText = RGB(235, 235, 235);

    COLORREF colLink = (settings.themeMode == ThemeMode::Win8Purple) ? RGB(91, 44, 130) :
                       (isDark ? RGB(100, 160, 255) : RGB(0, 102, 204));
    COLORREF colTipText = isDark ? RGB(160, 160, 160) : RGB(96, 96, 96);

    HBRUSH bgBrush = CreateSolidBrush(colBg);
    FillRect(memDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    HFONT fontAbbr = CreateFontW(
        ScaleForDpi(18, dpi), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    HFONT fontSubAbbr = CreateFontW(
        ScaleForDpi(11, dpi), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    HFONT fontTitle = CreateFontW(
        ScaleForDpi(14, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    HFONT fontSub = CreateFontW(
        ScaleForDpi(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    HFONT fontLink = CreateFontW(
        ScaleForDpi(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, g_hoveredFooter ? TRUE : FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    HFONT fontHint = CreateFontW(
        ScaleForDpi(11, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    std::vector<KeyboardLayoutItem> layoutsCopy;
    size_t selIndex = 0;
    int hovIndex = -1;
    EnterCriticalSection(&g_Ctx.csLock);
    layoutsCopy = g_layouts;
    selIndex = g_selectedIndex;
    hovIndex = g_hoveredIndex;
    LeaveCriticalSection(&g_Ctx.csLock);

    int currentY = 0;
    for (size_t i = 0; i < layoutsCopy.size(); ++i) {
        RECT itemRect{0, currentY, width, currentY + itemHeight};
        bool isSelected = (i == selIndex);
        bool isHovered = (static_cast<int>(i) == hovIndex);

        if (isSelected) {
            HBRUSH selBrush = CreateSolidBrush(colSelectedBg);
            FillRect(memDC, &itemRect, selBrush);
            DeleteObject(selBrush);
        } else if (isHovered) {
            HBRUSH hovBrush = CreateSolidBrush(colHoverBg);
            FillRect(memDC, &itemRect, hovBrush);
            DeleteObject(hovBrush);
        }

        COLORREF itemTextColor = isSelected ? colSelectedText : colTextNormal;
        COLORREF itemSubTextColor = isSelected ? colSelectedSubText : colTextSub;

        if (layoutsCopy[i].subAbbrev.empty()) {
            HGDIOBJ oldF = SelectObject(memDC, fontAbbr);
            SetTextColor(memDC, itemTextColor);
            RECT abbrRect{paddingX, currentY, badgeWidth + paddingX, currentY + itemHeight};
            DrawTextW(memDC, layoutsCopy[i].langAbbrev.c_str(), -1, &abbrRect,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(memDC, oldF);
        } else {
            HGDIOBJ oldF = SelectObject(memDC, fontAbbr);
            SetTextColor(memDC, itemTextColor);
            RECT abbrRect1{paddingX, currentY + ScaleForDpi(6, dpi), badgeWidth + paddingX, currentY + ScaleForDpi(30, dpi)};
            DrawTextW(memDC, layoutsCopy[i].langAbbrev.c_str(), -1, &abbrRect1,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

            SelectObject(memDC, fontSubAbbr);
            RECT abbrRect2{paddingX, currentY + ScaleForDpi(28, dpi), badgeWidth + paddingX, currentY + ScaleForDpi(48, dpi)};
            DrawTextW(memDC, layoutsCopy[i].subAbbrev.c_str(), -1, &abbrRect2,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(memDC, oldF);
        }

        int rightX = badgeWidth + paddingX + ScaleForDpi(6, dpi);
        int textRight = width - paddingX;

        {
            HGDIOBJ oldF = SelectObject(memDC, fontTitle);
            SetTextColor(memDC, itemTextColor);
            RECT textRect1{rightX, currentY + ScaleForDpi(8, dpi), textRight, currentY + ScaleForDpi(30, dpi)};
            DrawTextW(memDC, layoutsCopy[i].langName.c_str(), -1, &textRect1,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(memDC, oldF);
        }

        {
            HGDIOBJ oldF = SelectObject(memDC, fontSub);
            SetTextColor(memDC, itemSubTextColor);
            RECT textRect2{rightX, currentY + ScaleForDpi(30, dpi), textRight, currentY + ScaleForDpi(50, dpi)};
            DrawTextW(memDC, layoutsCopy[i].layoutName.c_str(), -1, &textRect2,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(memDC, oldF);
        }

        currentY += itemHeight;
    }

    RECT sepRect{paddingX, currentY + ScaleForDpi(4, dpi), width - paddingX, currentY + ScaleForDpi(4, dpi) + separatorHeight};
    HBRUSH sepBrush = CreateSolidBrush(colSeparator);
    FillRect(memDC, &sepRect, sepBrush);
    DeleteObject(sepBrush);
    currentY += ScaleForDpi(8, dpi);

    std::wstring prefStr, hintStr;
    GetLocalizedFooterStrings(prefStr, hintStr);

    {
        HGDIOBJ oldF = SelectObject(memDC, fontLink);
        SetTextColor(memDC, colLink);
        RECT linkRect{paddingX, currentY, width - paddingX, currentY + ScaleForDpi(22, dpi)};
        DrawTextW(memDC, prefStr.c_str(), -1, &linkRect,
                  DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(memDC, oldF);
    }

    if (settings.showShortcutHint) {
        HGDIOBJ oldF = SelectObject(memDC, fontHint);
        SetTextColor(memDC, colTipText);
        RECT tipRect{paddingX, currentY + ScaleForDpi(22, dpi), width - paddingX, currentY + ScaleForDpi(44, dpi)};
        DrawTextW(memDC, hintStr.c_str(), -1, &tipRect,
                  DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(memDC, oldF);
    }

    HPEN borderPen = CreatePen(PS_SOLID, 1, colBorder);
    HGDIOBJ oldPen = SelectObject(memDC, borderPen);
    HGDIOBJ oldNullBr = SelectObject(memDC, GetStockObject(NULL_BRUSH));
    Rectangle(memDC, 0, 0, width, height);
    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldNullBr);
    DeleteObject(borderPen);

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    DeleteObject(fontAbbr);
    DeleteObject(fontSubAbbr);
    DeleteObject(fontTitle);
    DeleteObject(fontSub);
    DeleteObject(fontLink);
    DeleteObject(fontHint);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}

static void DrawWin7GdiFallbackCheck(HDC hdc, const RECT& gutter, COLORREF color, UINT dpi) {
    if (!hdc) return;
    int thickness = ScaleForDpi(2, dpi);
    if (thickness < 2) thickness = 2;
    LOGBRUSH lb{};
    lb.lbStyle = BS_SOLID;
    lb.lbColor = color;
    HPEN pen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_MITER,
                            thickness, &lb, 0, nullptr);
    if (!pen) return;
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    const int cx = (gutter.left + gutter.right) / 2;
    const int cy = (gutter.top + gutter.bottom) / 2 + ScaleForDpi(1, dpi);
    MoveToEx(hdc, cx - ScaleForDpi(5, dpi), cy, nullptr);
    LineTo(hdc, cx - ScaleForDpi(1, dpi), cy + ScaleForDpi(4, dpi));
    LineTo(hdc, cx + ScaleForDpi(6, dpi), cy - ScaleForDpi(5, dpi));
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void DrawWin7MenuCheckmark(HDC hdc, const RECT& gutter, COLORREF color, UINT dpi) {
    if (!hdc) return;

    if (!g_hGdiPlus || !pGdipCreateFromHDC || !pGdipFillPath) {
        DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
        return;
    }

    const BYTE r = GetRValue(color);
    const BYTE gch = GetGValue(color);
    const BYTE b = GetBValue(color);

    void* graphics = NULL;
    if (pGdipCreateFromHDC(hdc, &graphics) != 0 || !graphics) {
        DrawWin7GdiFallbackCheck(hdc, gutter, color, dpi);
        return;
    }

    pGdipSetSmoothingMode(graphics, 2);
    pGdipSetPixelOffsetMode(graphics, 2);

    const float boxL = static_cast<float>(gutter.left);
    const float boxT = static_cast<float>(gutter.top);
    const float boxW = static_cast<float>(gutter.right - gutter.left);
    const float boxH = static_cast<float>(gutter.bottom - gutter.top);

    float size = boxW * 0.70f;
    if (size > boxH * 0.50f) size = boxH * 0.50f;
    if (size < 8.0f) size = (boxH < boxW ? boxH : boxW) * 0.48f;

    const float originX = boxL + (boxW - size) * 0.42f;
    const float originY = boxT + (boxH - size) * 0.54f;

    struct PointF { float X; float Y; };
    const PointF pts[] = {
        { originX + size * 0.06f, originY + size * 0.50f },
        { originX + size * 0.18f, originY + size * 0.38f },
        { originX + size * 0.38f, originY + size * 0.62f },
        { originX + size * 0.82f, originY + size * 0.08f },
        { originX + size * 0.96f, originY + size * 0.20f },
        { originX + size * 0.38f, originY + size * 0.90f },
    };

    void* path = NULL;
    if (pGdipCreatePath(0, &path) == 0 && path) {
        pGdipAddPathPolygon(path, pts, 6);
        DWORD argb = (255 << 24) | (r << 16) | (gch << 8) | b;
        void* brush = NULL;
        if (pGdipCreateSolidFill(argb, &brush) == 0 && brush) {
            pGdipFillPath(graphics, brush, path);
            pGdipDeleteBrush(brush);
        }
        void* pen = NULL;
        float outlineW = (dpi >= 144) ? 0.90f * (static_cast<float>(dpi) / 96.0f) : 0.70f;
        if (pGdipCreatePen1(argb, outlineW, 2, &pen) == 0 && pen) {
            pGdipSetPenLineJoin(pen, 2);
            pGdipDrawPath(graphics, pen, path);
            pGdipDeletePen(pen);
        }
        pGdipDeletePath(path);
    }

    pGdipDeleteGraphics(graphics);
}

static void PaintWin7Menu(HWND hwnd, HDC hdc) {
    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) return;

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0) return;

    UINT dpi = GetWindowDpi(hwnd);
    const int itemHeight = ScaleForDpi(26, dpi);
    const int paddingLeft = ScaleForDpi(28, dpi);
    const int paddingRight = ScaleForDpi(16, dpi);

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) return;
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, width, height);
    if (!memBmp) { DeleteDC(memDC); return; }
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    SetBkMode(memDC, TRANSPARENT);

    ModSettings settings = GetSettingsSnapshot();
    bool isDark = (settings.themeMode == ThemeMode::Dark) ||
                  (settings.themeMode == ThemeMode::Auto && IsDarkModeActive());

    COLORREF colBg = isDark ? RGB(36, 36, 36) : RGB(242, 242, 242);
    COLORREF colTextNormal = isDark ? RGB(245, 245, 245) : RGB(0, 0, 0);
    COLORREF colHoverBg = isDark ? RGB(56, 56, 56) : RGB(185, 215, 251);
    COLORREF colHoverBorder = isDark ? RGB(80, 80, 80) : RGB(125, 162, 206);
    COLORREF colBorder = isDark ? RGB(70, 70, 70) : RGB(160, 160, 160);
    COLORREF colSeparator = isDark ? RGB(55, 55, 55) : RGB(210, 210, 210);

    HBRUSH bgBrush = CreateSolidBrush(colBg);
    FillRect(memDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    HFONT fontMenu = CreateFontW(
        ScaleForDpi(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    std::vector<KeyboardLayoutItem> layoutsCopy;
    size_t activeIdx = 0;
    int hovIdx = -1;
    EnterCriticalSection(&g_Ctx.csLock);
    layoutsCopy = g_layouts;
    activeIdx = g_selectedIndex;
    hovIdx = g_hoveredWin7Index;
    LeaveCriticalSection(&g_Ctx.csLock);

    int currentY = ScaleForDpi(3, dpi);

    for (size_t i = 0; i < layoutsCopy.size(); ++i) {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        bool isHovered = (static_cast<int>(i) == hovIdx);
        bool isActive = (i == activeIdx);

        if (isHovered) {
            HBRUSH hovBrush = CreateSolidBrush(colHoverBg);
            FillRect(memDC, &itemRect, hovBrush);
            DeleteObject(hovBrush);

            HPEN hovPen = CreatePen(PS_SOLID, 1, colHoverBorder);
            HGDIOBJ oldPen = SelectObject(memDC, hovPen);
            HGDIOBJ oldNull = SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Rectangle(memDC, itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
            SelectObject(memDC, oldPen);
            SelectObject(memDC, oldNull);
            DeleteObject(hovPen);
        }

        if (isActive && settings.switcherStyle == SwitcherStyle::Win7) {
            RECT checkRect{ScaleForDpi(4, dpi), currentY, paddingLeft, currentY + itemHeight};
            DrawWin7MenuCheckmark(memDC, checkRect, colTextNormal, dpi);
        }

        std::wstring itemText = FormatWin7LayoutItemText(layoutsCopy[i], layoutsCopy[i].sameLangCount);

        HGDIOBJ oldF = SelectObject(memDC, fontMenu);
        SetTextColor(memDC, colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        DrawTextW(memDC, itemText.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(memDC, oldF);

        currentY += itemHeight;
    }

    currentY += ScaleForDpi(2, dpi);
    RECT sepRect{ScaleForDpi(6, dpi), currentY + ScaleForDpi(3, dpi), width - ScaleForDpi(6, dpi), currentY + ScaleForDpi(4, dpi)};
    HBRUSH sepBrush = CreateSolidBrush(colSeparator);
    FillRect(memDC, &sepRect, sepBrush);
    DeleteObject(sepBrush);
    currentY += ScaleForDpi(7, dpi);

    std::wstring prefStr, hintStr, showBarStr;
    GetLocalizedFooterStrings(prefStr, hintStr, &showBarStr);

    int footer1Index = static_cast<int>(layoutsCopy.size());
    int footer2Index = footer1Index + 1;

    {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        if (hovIdx == footer1Index) {
            HBRUSH hovBrush = CreateSolidBrush(colHoverBg);
            FillRect(memDC, &itemRect, hovBrush);
            DeleteObject(hovBrush);
            HPEN hovPen = CreatePen(PS_SOLID, 1, colHoverBorder);
            HGDIOBJ oldPen = SelectObject(memDC, hovPen);
            HGDIOBJ oldNull = SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Rectangle(memDC, itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
            SelectObject(memDC, oldPen);
            SelectObject(memDC, oldNull);
            DeleteObject(hovPen);
        }
        HGDIOBJ oldF = SelectObject(memDC, fontMenu);
        SetTextColor(memDC, colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        DrawTextW(memDC, showBarStr.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(memDC, oldF);
        currentY += itemHeight;
    }

    {
        RECT itemRect{ScaleForDpi(2, dpi), currentY, width - ScaleForDpi(2, dpi), currentY + itemHeight};
        if (hovIdx == footer2Index) {
            HBRUSH hovBrush = CreateSolidBrush(colHoverBg);
            FillRect(memDC, &itemRect, hovBrush);
            DeleteObject(hovBrush);
            HPEN hovPen = CreatePen(PS_SOLID, 1, colHoverBorder);
            HGDIOBJ oldPen = SelectObject(memDC, hovPen);
            HGDIOBJ oldNull = SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Rectangle(memDC, itemRect.left, itemRect.top, itemRect.right, itemRect.bottom);
            SelectObject(memDC, oldPen);
            SelectObject(memDC, oldNull);
            DeleteObject(hovPen);
        }
        HGDIOBJ oldF = SelectObject(memDC, fontMenu);
        SetTextColor(memDC, colTextNormal);
        RECT textRect{paddingLeft, currentY, width - paddingRight, currentY + itemHeight};
        std::wstring prefWithDots = prefStr + L"...";
        DrawTextW(memDC, prefWithDots.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(memDC, oldF);
    }

    HPEN borderPen = CreatePen(PS_SOLID, 1, colBorder);
    HGDIOBJ oldPen = SelectObject(memDC, borderPen);
    HGDIOBJ oldNullBrush = SelectObject(memDC, GetStockObject(NULL_BRUSH));
    Rectangle(memDC, 0, 0, width, height);
    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldNullBrush);
    DeleteObject(borderPen);

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    DeleteObject(fontMenu);
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}

static void PaintSwitcher(HWND hwnd, HDC hdc) {
    static thread_local bool s_inPaint = false;
    if (s_inPaint || !hwnd || !hdc) return;
    s_inPaint = true;

    try {
        ModSettings settings = GetSettingsSnapshot();
        if (settings.switcherStyle == SwitcherStyle::Win7) {
            PaintWin7Menu(hwnd, hdc);
        } else {
            PaintWin8Flyout(hwnd, hdc);
        }
    } catch (...) {}

    s_inPaint = false;
}

static HWND FindAncestorTaskbar(HWND hwnd) {
    HWND h = hwnd;
    while (h) {
        WCHAR cls[64] = {};
        if (GetClassNameW(h, cls, ARRAYSIZE(cls)) > 0) {
            if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
                return h;
            }
        }
        HWND parent = GetAncestor(h, GA_PARENT);
        if (!parent || parent == h) break;
        h = parent;
    }
    return hwnd;
}

static void PositionWindowNearTray(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    HWND hAnchor = AtomicLoadHwnd(g_hClickedTaskbar);
    if (!hAnchor || !IsWindow(hAnchor)) {
        hAnchor = FindWindowW(L"Shell_TrayWnd", NULL);
    }

    HMONITOR hMon = MonitorFromWindow(hAnchor ? hAnchor : hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork{};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    UINT dpi = GetMonitorDpi(hMon);

    int flyoutWidth = 0;
    int totalHeight = 0;

    size_t count = 0;
    std::vector<KeyboardLayoutItem> layoutsCopy;
    EnterCriticalSection(&g_Ctx.csLock);
    count = g_layouts.size();
    layoutsCopy = g_layouts;
    LeaveCriticalSection(&g_Ctx.csLock);

    if (count == 0) return;

    ModSettings settings = GetSettingsSnapshot();
    if (settings.switcherStyle == SwitcherStyle::Win7) {
        int itemHeight = ScaleForDpi(26, dpi);
        int sepHeight = ScaleForDpi(9, dpi);
        int footerHeight = itemHeight * 2;
        totalHeight = ScaleForDpi(6, dpi) + static_cast<int>(count) * itemHeight + sepHeight + footerHeight;

        int calculatedWidth = ScaleForDpi(260, dpi);
        HDC dc = GetDC(NULL);
        if (dc) {
            HFONT fontMenu = CreateFontW(
                ScaleForDpi(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
            );
            HGDIOBJ oldF = SelectObject(dc, fontMenu);

            for (const auto& item : layoutsCopy) {
                std::wstring itemText = FormatWin7LayoutItemText(item, item.sameLangCount);
                RECT rcCalc{0, 0, 0, 0};
                DrawTextW(dc, itemText.c_str(), -1, &rcCalc, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
                int itemW = rcCalc.right - rcCalc.left + ScaleForDpi(48, dpi);
                if (itemW > calculatedWidth) calculatedWidth = itemW;
            }

            std::wstring prefStr, hintStr, showBarStr;
            GetLocalizedFooterStrings(prefStr, hintStr, &showBarStr);
            RECT rcShowBar{0, 0, 0, 0};
            DrawTextW(dc, showBarStr.c_str(), -1, &rcShowBar, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
            int sbW = rcShowBar.right - rcShowBar.left + ScaleForDpi(48, dpi);
            if (sbW > calculatedWidth) calculatedWidth = sbW;

            std::wstring prefWithDots = prefStr + L"...";
            RECT rcPref{0, 0, 0, 0};
            DrawTextW(dc, prefWithDots.c_str(), -1, &rcPref, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
            int prW = rcPref.right - rcPref.left + ScaleForDpi(48, dpi);
            if (prW > calculatedWidth) calculatedWidth = prW;

            SelectObject(dc, oldF);
            DeleteObject(fontMenu);
            ReleaseDC(NULL, dc);
        }
        flyoutWidth = (calculatedWidth > ScaleForDpi(420, dpi)) ? ScaleForDpi(420, dpi) : calculatedWidth;
    } else {
        int itemHeight = ScaleForDpi(58, dpi);
        int footerHeight = ScaleForDpi(62, dpi);
        totalHeight = static_cast<int>(count) * itemHeight + footerHeight;
        flyoutWidth = ScaleForDpi(330, dpi);
    }

    APPBARDATA abd = { sizeof(APPBARDATA) };
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);

    // Prefer the actual taskbar window on this monitor for edge detection.
    RECT rcTaskbar{};
    UINT edge = abd.uEdge;
    if (hAnchor && GetWindowRect(hAnchor, &rcTaskbar)) {
        int monW = mi.rcMonitor.right - mi.rcMonitor.left;
        int monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
        int tbW = rcTaskbar.right - rcTaskbar.left;
        int tbH = rcTaskbar.bottom - rcTaskbar.top;
        if (tbW >= monW * 0.8 && tbH < monH / 2) {
            edge = (rcTaskbar.top <= mi.rcMonitor.top + 8) ? ABE_TOP : ABE_BOTTOM;
        } else if (tbH >= monH * 0.8 && tbW < monW / 2) {
            edge = (rcTaskbar.left <= mi.rcMonitor.left + 8) ? ABE_LEFT : ABE_RIGHT;
        }
    }

    int x = rcWork.right - flyoutWidth - 8;
    int y = rcWork.bottom - totalHeight - 8;
    if (edge == ABE_TOP)        y = (hAnchor ? rcTaskbar.bottom : abd.rc.bottom) + 8;
    else if (edge == ABE_LEFT)  x = (hAnchor ? rcTaskbar.right : abd.rc.right) + 8;
    else if (edge == ABE_RIGHT) x = (hAnchor ? rcTaskbar.left : abd.rc.left) - flyoutWidth - 8;
    else if (hAnchor)           y = rcTaskbar.top - totalHeight - 8;

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, flyoutWidth, totalHeight, SWP_SHOWWINDOW);
}

static LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (hdc) {
                PaintSwitcher(hwnd, hdc);
                EndPaint(hwnd, &ps);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            UINT dpi = GetWindowDpi(hwnd);

            size_t count = 0;
            EnterCriticalSection(&g_Ctx.csLock);
            count = g_layouts.size();
            LeaveCriticalSection(&g_Ctx.csLock);

            bool needsRepaint = false;
            ModSettings settings = GetSettingsSnapshot();

            if (settings.switcherStyle == SwitcherStyle::Win7) {
                const int itemHeight = ScaleForDpi(26, dpi);
                const int sepHeight = ScaleForDpi(9, dpi);
                int newHov = -1;

                if (y >= ScaleForDpi(3, dpi) && y < ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight) {
                    newHov = (y - ScaleForDpi(3, dpi)) / itemHeight;
                } else if (y >= ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight) {
                    int footerY = y - (ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight);
                    int fIndex = footerY / itemHeight;
                    if (fIndex == 0) newHov = static_cast<int>(count);
                    else if (fIndex == 1) newHov = static_cast<int>(count) + 1;
                }

                EnterCriticalSection(&g_Ctx.csLock);
                if (newHov != g_hoveredWin7Index) {
                    g_hoveredWin7Index = newHov;
                    needsRepaint = true;
                }
                LeaveCriticalSection(&g_Ctx.csLock);
            } else {
                const int itemHeight = ScaleForDpi(58, dpi);
                const int paddingX = ScaleForDpi(16, dpi);
                int newHoveredIndex = -1;
                bool newHoveredFooter = false;

                if (itemHeight > 0 && y >= 0 && y < static_cast<int>(count) * itemHeight) {
                    newHoveredIndex = y / itemHeight;
                    if (newHoveredIndex >= static_cast<int>(count)) {
                        newHoveredIndex = -1;
                    }
                } else if (y >= static_cast<int>(count) * itemHeight) {
                    int linkY = static_cast<int>(count) * itemHeight + ScaleForDpi(8, dpi);
                    if (y >= linkY && y <= linkY + ScaleForDpi(24, dpi) && x >= paddingX && x <= ScaleForDpi(250, dpi)) {
                        newHoveredFooter = true;
                    }
                }

                EnterCriticalSection(&g_Ctx.csLock);
                if (newHoveredIndex != g_hoveredIndex || newHoveredFooter != g_hoveredFooter) {
                    g_hoveredIndex = newHoveredIndex;
                    g_hoveredFooter = newHoveredFooter;
                    needsRepaint = true;
                }
                LeaveCriticalSection(&g_Ctx.csLock);
            }

            if (needsRepaint && IsWindow(hwnd)) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }

            TRACKMOUSEEVENT tme{};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE: {
            EnterCriticalSection(&g_Ctx.csLock);
            g_hoveredIndex = -1;
            g_hoveredFooter = false;
            g_hoveredWin7Index = -1;
            LeaveCriticalSection(&g_Ctx.csLock);

            if (IsWindow(hwnd)) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_SETCURSOR: {
            bool isFooter = false;
            EnterCriticalSection(&g_Ctx.csLock);
            isFooter = g_hoveredFooter;
            LeaveCriticalSection(&g_Ctx.csLock);

            ModSettings settings = GetSettingsSnapshot();
            if (isFooter && settings.switcherStyle != SwitcherStyle::Win7) {
                SetCursor(LoadCursor(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONUP: {
            int y = GET_Y_LPARAM(lParam);
            int x = GET_X_LPARAM(lParam);
            UINT dpi = GetWindowDpi(hwnd);

            size_t count = 0;
            EnterCriticalSection(&g_Ctx.csLock);
            count = g_layouts.size();
            LeaveCriticalSection(&g_Ctx.csLock);

            ModSettings settings = GetSettingsSnapshot();

            if (settings.switcherStyle == SwitcherStyle::Win7) {
                const int itemHeight = ScaleForDpi(26, dpi);
                const int sepHeight = ScaleForDpi(9, dpi);

                if (y >= ScaleForDpi(3, dpi) && y < ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight) {
                    size_t clickedIndex = static_cast<size_t>((y - ScaleForDpi(3, dpi)) / itemHeight);
                    if (clickedIndex < count) {
                        SwitchToLayout(clickedIndex);
                    }
                    ShowWindow(hwnd, SW_HIDE);
                    return 0;
                } else if (y >= ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight) {
                    int footerY = y - (ScaleForDpi(3, dpi) + static_cast<int>(count) * itemHeight + sepHeight);
                    int fIndex = footerY / itemHeight;
                    ShowWindow(hwnd, SW_HIDE);
                    if (fIndex == 0) {
                        ShellExecuteW(nullptr, L"open", L"control.exe", L"/name Microsoft.Language", nullptr, SW_SHOWNORMAL);
                    } else if (fIndex == 1) {
                        std::wstring cmd = settings.customPreferencesCmd;
                        if (cmd.empty()) cmd = L"ms-settings:regionlanguage";
                        ShellExecuteW(nullptr, L"open", cmd.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    }
                    return 0;
                }
            } else {
                int itemHeight = ScaleForDpi(58, dpi);
                int paddingX = ScaleForDpi(16, dpi);

                if (itemHeight > 0 && y >= 0 && y < static_cast<int>(count) * itemHeight) {
                    size_t clickedIndex = static_cast<size_t>(y / itemHeight);
                    if (clickedIndex < count) {
                        SwitchToLayout(clickedIndex);
                    }
                    ShowWindow(hwnd, SW_HIDE);
                    return 0;
                } else if (y >= static_cast<int>(count) * itemHeight) {
                    int linkY = static_cast<int>(count) * itemHeight + ScaleForDpi(8, dpi);
                    if (y >= linkY && y <= linkY + ScaleForDpi(26, dpi) && x >= paddingX && x <= ScaleForDpi(250, dpi)) {
                        ShowWindow(hwnd, SW_HIDE);
                        std::wstring cmd = settings.customPreferencesCmd;
                        if (cmd.empty()) cmd = L"ms-settings:regionlanguage";
                        ShellExecuteW(nullptr, L"open", cmd.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                        return 0;
                    }
                }
            }
            break;
        }

        case WM_KEYDOWN: {
            size_t count = 0;
            EnterCriticalSection(&g_Ctx.csLock);
            count = g_layouts.size();
            LeaveCriticalSection(&g_Ctx.csLock);

            if (count == 0) break;

            if (wParam == VK_ESCAPE) {
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            } else if (wParam == VK_DOWN || wParam == VK_TAB) {
                EnterCriticalSection(&g_Ctx.csLock);
                g_selectedIndex = (g_selectedIndex + 1) % count;
                LeaveCriticalSection(&g_Ctx.csLock);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_UP) {
                EnterCriticalSection(&g_Ctx.csLock);
                g_selectedIndex = (g_selectedIndex + count - 1) % count;
                LeaveCriticalSection(&g_Ctx.csLock);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (wParam == VK_RETURN || wParam == VK_SPACE) {
                size_t sel = 0;
                EnterCriticalSection(&g_Ctx.csLock);
                sel = g_selectedIndex;
                LeaveCriticalSection(&g_Ctx.csLock);
                SwitchToLayout(sel);
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            break;
        }

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                g_lastInactiveTick.store(GetTickCount(), std::memory_order_relaxed);
                if (!g_isWinSpaceCycling.load(std::memory_order_relaxed)) {
                    ShowWindow(hwnd, SW_HIDE);
                }
            }
            break;

        case WM_SAFE_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;

        case WM_DESTROY:
            InterlockedDecrement(&g_Ctx.refCount);
            g_hFlyoutWnd.store(NULL, std::memory_order_release);
            g_dwFlyoutOwnerThreadId.store(0, std::memory_order_release);
            return 0;
        }
    } catch (...) {}
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void ShowFlyoutWindow() {
    if (g_Ctx.isUninitializing) return;

    DWORD dwCurrentThreadId = GetCurrentThreadId();
    HWND hExisting = AtomicLoadHwnd(g_hFlyoutWnd);
    BOOL flyoutAlreadyExists = (hExisting && IsWindow(hExisting));
    DWORD dwTargetOwnerThreadId = flyoutAlreadyExists
        ? g_dwFlyoutOwnerThreadId.load(std::memory_order_acquire)
        : g_Ctx.dwWorkerThreadId;
    // A owner thread id of 0 means SafeCleanup already zeroed it out (the
    // worker thread is gone or going away) — never fall through and create
    // a window on the calling thread in that case, or it will outlive
    // Wh_ModUninit and get orphaned when the image is unmapped.
    if (dwTargetOwnerThreadId == 0) {
        return;
    }
    if (dwTargetOwnerThreadId != dwCurrentThreadId) {
        PostThreadMessageW(dwTargetOwnerThreadId, WM_SHOW_FLYOUT, 0, 0);
        return;
    }

    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    if (!hFlyout || !IsWindow(hFlyout)) {
        HINSTANCE hInst = GetModInstance();
        if (!g_flyoutClassRegistered) {
            WNDCLASSW wc = {0};
            wc.lpfnWndProc   = FlyoutWndProc;
            wc.hInstance     = hInst;
            wc.lpszClassName = kFlyoutClassName;
            wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = NULL;
            if (!RegisterClassW(&wc)) {
                // Do NOT treat ERROR_CLASS_ALREADY_EXISTS as success here: it
                // means a previous load's class (and stale WNDPROC pointing
                // into an unmapped image) is still registered because that
                // load's UnregisterClassW failed. Reusing it would crash
                // Explorer on the first message to the new window.
                Wh_Log(L"Win78LangSwitcher: RegisterClassW failed (%lu)", GetLastError());
                return;
            }
            g_flyoutClassRegistered = true;
        }

        DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
        DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_BORDER;

        hFlyout = CreateWindowExW(dwExStyle, kFlyoutClassName, L"Windows Language Switcher", dwStyle,
            0, 0, 100, 100,
            NULL, NULL, hInst, NULL);
        if (hFlyout) {
            g_hFlyoutWnd.store(hFlyout, std::memory_order_release);
            g_dwFlyoutOwnerThreadId.store(GetCurrentThreadId(), std::memory_order_release);
            InterlockedIncrement(&g_Ctx.refCount);

            ModSettings settings = GetSettingsSnapshot();
            bool isDark = (settings.themeMode == ThemeMode::Dark) ||
                          (settings.themeMode == ThemeMode::Auto && IsDarkModeActive());
            BOOL useDarkMode = isDark ? TRUE : FALSE;
            DwmSetWindowAttribute(hFlyout, 20, &useDarkMode, sizeof(useDarkMode));
            DwmSetWindowAttribute(hFlyout, 19, &useDarkMode, sizeof(useDarkMode));

            enum { DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL = 33, DWMWCP_ROUND_LOCAL = 2 };
            DWORD cornerPref = DWMWCP_ROUND_LOCAL;
            DwmSetWindowAttribute(hFlyout, DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL, &cornerPref, sizeof(cornerPref));
        }
    }

    if (hFlyout && IsWindow(hFlyout)) {
        RefreshKeyboardLayouts();
        PositionWindowNearTray(hFlyout);
        ShowWindow(hFlyout, SW_SHOW);
        SetForegroundWindow(hFlyout);
        InvalidateRect(hFlyout, NULL, TRUE);
    }
}

void ToggleFlyoutWindow() {
    if (g_Ctx.isUninitializing) return;

    DWORD dwCurrentThreadId = GetCurrentThreadId();
    HWND hExisting = AtomicLoadHwnd(g_hFlyoutWnd);
    BOOL flyoutAlreadyExists = (hExisting && IsWindow(hExisting));
    DWORD dwTargetOwnerThreadId = flyoutAlreadyExists
        ? g_dwFlyoutOwnerThreadId.load(std::memory_order_acquire)
        : g_Ctx.dwWorkerThreadId;
    if (dwTargetOwnerThreadId == 0) {
        return;
    }
    if (dwTargetOwnerThreadId != dwCurrentThreadId) {
        PostThreadMessageW(dwTargetOwnerThreadId, WM_TOGGLE_FLYOUT_REQUEST, 0, 0);
        return;
    }

    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    if (hFlyout && IsWindow(hFlyout) && IsWindowVisible(hFlyout)) {
        ShowWindow(hFlyout, SW_HIDE);
    } else {
        ShowFlyoutWindow();
    }
}

static void CycleSwitcher(bool forward, bool switchImmediately, bool showFlyout = true) {
    try {
        RefreshKeyboardLayouts();
        size_t count = 0;
        size_t sel = 0;
        EnterCriticalSection(&g_Ctx.csLock);
        count = g_layouts.size();
        if (count > 0) {
            if (forward) {
                g_selectedIndex = (g_selectedIndex + 1) % count;
            } else {
                g_selectedIndex = (g_selectedIndex + count - 1) % count;
            }
            sel = g_selectedIndex;
        }
        LeaveCriticalSection(&g_Ctx.csLock);
        if (count == 0) return;

        if (switchImmediately) {
            SwitchToLayout(sel);
        }

        // Windows 7/8.1 switch silently on Alt+Shift / Ctrl+Shift and only
        // show the overlay for Win+Space. Showing it here too would leave a
        // topmost window sitting over the app with no reliable auto-hide,
        // since SetForegroundWindow can't steal focus from whatever the
        // user was typing into.
        if (!showFlyout) return;

        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
        if (hFlyout && IsWindow(hFlyout) && IsWindowVisible(hFlyout)) {
            InvalidateRect(hFlyout, nullptr, FALSE);
        } else {
            ShowFlyoutWindow();
        }
    } catch (...) {}
}

static void ApplySelection() {
    try {
        size_t sel = 0;
        EnterCriticalSection(&g_Ctx.csLock);
        sel = g_selectedIndex;
        LeaveCriticalSection(&g_Ctx.csLock);
        SwitchToLayout(sel);
        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
        if (hFlyout && IsWindow(hFlyout)) {
            ShowWindow(hFlyout, SW_HIDE);
        }
    } catch (...) {}
}

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_Ctx.isUninitializing) {
        return ShowWindow_Original ? ShowWindow_Original(hWnd, nCmdShow) : ShowWindow(hWnd, nCmdShow);
    }

    // Only the shell's explorer may suppress/replace the modern input-switch
    // overlay; a second instance must leave Shows alone.
    if (!g_isMainShell.load(std::memory_order_relaxed)) {
        return ShowWindow_Original ? ShowWindow_Original(hWnd, nCmdShow) : ShowWindow(hWnd, nCmdShow);
    }

    if (g_hookTrayClicks.load(std::memory_order_relaxed) && hWnd && IsWindow(hWnd) && nCmdShow != SW_HIDE) {
        WCHAR className[128] = {0};
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
            if (_wcsicmp(className, L"Shell_InputSwitchDismissOverlay") == 0) {
                return TRUE;
            }

           if (_wcsicmp(className, L"Shell_InputSwitchTopLevelWindow") == 0) {
    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    HWND hFore = GetForegroundWindow();
    if (hFore != hWnd && hFore != hFlyout) {
        if (!IsTaskbarWindow(hFore)) {
            g_targetWindow.store(hFore, std::memory_order_release);
        }
    }
    ShowFlyoutWindow();
    return TRUE;
            }
        }
    }

    return ShowWindow_Original ? ShowWindow_Original(hWnd, nCmdShow) : ShowWindow(hWnd, nCmdShow);
}

static std::wstring TrimWs(std::wstring s) {
    size_t b = 0;
    while (b < s.size() && iswspace(s[b])) ++b;
    size_t e = s.size();
    while (e > b && iswspace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

static bool IsLanguageButton(HWND hToolbar, int idCommand) {
    LRESULT needed = SendMessageW(hToolbar, TB_GETBUTTONTEXT, (WPARAM)idCommand, (LPARAM)NULL);
    if (needed <= 0 || needed > 1024) {
        return false;
    }

    std::wstring text(static_cast<size_t>(needed) + 1, L'\0');
    LRESULT copied = SendMessageW(hToolbar, TB_GETBUTTONTEXT, (WPARAM)idCommand, (LPARAM)text.data());
    if (copied <= 0) {
        return false;
    }
    text.resize(static_cast<size_t>(copied));
    text = TrimWs(text);
    for (auto& c : text) c = towupper(c);

    std::vector<std::wstring> abbrevs;
    EnterCriticalSection(&g_Ctx.csLock);
    abbrevs.reserve(g_layouts.size());
    for (const auto& l : g_layouts) {
        if (!l.langAbbrev.empty()) {
            std::wstring a = l.langAbbrev;
            for (auto& c : a) c = towupper(c);
            abbrevs.push_back(std::move(a));
        }
    }
    LeaveCriticalSection(&g_Ctx.csLock);

    for (const auto& a : abbrevs) {
        if (text == a) {
            return true;
        }
    }
    return false;
}

static LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    (void)uIdSubclass;
    if (msg == WM_NCDESTROY) {
        EnterCriticalSection(&g_Ctx.csLock);
        if (hWnd == G_hSubclassedToolbar) G_hSubclassedToolbar = NULL;
        for (size_t i = 0; i < G_hSubclassedSecToolbars.size(); ) {
            if (G_hSubclassedSecToolbars[i] == hWnd) {
                G_hSubclassedSecToolbars.erase(G_hSubclassedSecToolbars.begin() + i);
            } else {
                ++i;
            }
        }
        LeaveCriticalSection(&g_Ctx.csLock);
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    if (g_Ctx.isUninitializing) {
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    if (g_hookTrayClicks.load(std::memory_order_relaxed)) {
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK || msg == WM_MOUSEACTIVATE) {
            POINT pt;
            if (msg == WM_MOUSEACTIVATE) {
                DWORD dwPos = GetMessagePos();
                pt.x = GET_X_LPARAM(dwPos);
                pt.y = GET_Y_LPARAM(dwPos);
                ScreenToClient(hWnd, &pt);
            } else {
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
            }

            LRESULT btnIdx = SendMessageW(hWnd, TB_HITTEST, 0, (LPARAM)&pt);
            if (btnIdx >= 0) {
                TBBUTTON tb = {0};
                if (SendMessageW(hWnd, TB_GETBUTTON, (WPARAM)btnIdx, (LPARAM)&tb)) {
                    if (IsLanguageButton(hWnd, tb.idCommand)) {
                        if (msg == WM_LBUTTONUP) {
    static DWORD lastClickTime = 0;
    DWORD currentTime = GetTickCount();
    if (currentTime - g_lastInactiveTick.load(std::memory_order_relaxed) < 350) {
        return 0;
    }
    if (currentTime - lastClickTime > CLICK_DEBOUNCE_MS) {
        lastClickTime = currentTime;
        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
        HWND hFore = GetForegroundWindow();
        // FIX: Se hFore è la taskbar, usa l'ultima finestra non-taskbar valida
        if (hFore != hWnd && hFore != hFlyout) {
            if (IsTaskbarWindow(hFore)) {
                // Cerca la finestra attiva nella stessa sessione
                HWND hRealFore = GetForegroundWindow();
                // Se è ancora la taskbar, mantieni il target esistente
                if (IsTaskbarWindow(hRealFore)) {
                    // Non aggiornare, mantieni il target precedente
                } else {
                    g_targetWindow.store(hRealFore, std::memory_order_release);
                }
            } else {
                g_targetWindow.store(hFore, std::memory_order_release);
            }
        }
        g_hClickedTaskbar.store(FindAncestorTaskbar(hWnd), std::memory_order_release);
        ToggleFlyoutWindow();
    }
}
                        if (msg == WM_MOUSEACTIVATE) return MA_ACTIVATE;
                        return 0;
                    }
                }
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

static LRESULT CALLBACK InputIndicatorButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    (void)uIdSubclass;
    if (msg == WM_NCDESTROY) {
        EnterCriticalSection(&g_Ctx.csLock);
        if (hWnd == G_hSubclassedIndicator) G_hSubclassedIndicator = NULL;
        LeaveCriticalSection(&g_Ctx.csLock);
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    if (g_Ctx.isUninitializing) {
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    if (g_hookTrayClicks.load(std::memory_order_relaxed)) {
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK || msg == WM_MOUSEACTIVATE) {
            if (msg == WM_LBUTTONUP) {
                static DWORD lastClickTime = 0;
                DWORD currentTime = GetTickCount();
                if (currentTime - g_lastInactiveTick.load(std::memory_order_relaxed) < 350) {
                    return 0;
                }
                if (currentTime - lastClickTime > CLICK_DEBOUNCE_MS) {
                    lastClickTime = currentTime;
                    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
                    HWND hFore = GetForegroundWindow();
                    if (hFore != hWnd && hFore != hFlyout) {
                        g_targetWindow.store(hFore, std::memory_order_release);
                    }
                    g_hClickedTaskbar.store(FindAncestorTaskbar(hWnd), std::memory_order_release);
                    ToggleFlyoutWindow();
                }
            }
            if (msg == WM_MOUSEACTIVATE) return MA_ACTIVATE;
            return 0;
        } else if (msg == WM_KEYDOWN && (wParam == VK_RETURN || wParam == VK_SPACE)) {
            HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
            HWND hFore = GetForegroundWindow();
            if (hFore != hWnd && hFore != hFlyout) {
                g_targetWindow.store(hFore, std::memory_order_release);
            }
            g_hClickedTaskbar.store(FindAncestorTaskbar(hWnd), std::memory_order_release);
            ToggleFlyoutWindow();
            return 0;
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

static BOOL InstallTrayInterceptionInternal() {
    if (g_Ctx.isUninitializing) return FALSE;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return FALSE;

    // The tray tree (and the WNDPROC subclasses we attach, which point into
    // this image) can only be subclassed if the taskbar belongs to this
    // process. FindWindow returns whichever Explorer currently owns the
    // taskbar, which need not be us (e.g. during a restart). Subclassing a
    // foreign window would fail at install time but we'd still record the
    // HWND — and then unload would SendMessage a dangling subclass across
    // processes. Refuse foreign trays up front.
    DWORD trayPid = 0;
    GetWindowThreadProcessId(hTray, &trayPid);
    if (trayPid != GetCurrentProcessId()) {
        return FALSE;
    }

    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    HWND hSysPager = hNotify ? FindWindowExW(hNotify, NULL, L"SysPager", NULL) : NULL;
    HWND hToolbar = hSysPager ? FindWindowExW(hSysPager, NULL, L"ToolbarWindow32", NULL) : NULL;
    if (!hToolbar && hNotify) {
        hToolbar = FindWindowExW(hNotify, NULL, L"ToolbarWindow32", NULL);
    }

    HWND oldToolbar = NULL;
    bool needToolbarSubclass = false;

    EnterCriticalSection(&g_Ctx.csLock);
    if (hToolbar && hToolbar != G_hSubclassedToolbar) {
        oldToolbar = G_hSubclassedToolbar;
        G_hSubclassedToolbar = hToolbar;
        needToolbarSubclass = true;
    }
    LeaveCriticalSection(&g_Ctx.csLock);

    if (needToolbarSubclass) {
        if (oldToolbar) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(oldToolbar, ToolbarWndProc);
        }
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hToolbar, ToolbarWndProc, 0)) {
            Wh_Log(L"Win78LangSwitcher: Subclassed ToolbarWindow32 (0x%p)", hToolbar);
        }
    }

    if (hNotify) {
        HWND hChild = NULL;
        while ((hChild = FindWindowExW(hNotify, hChild, NULL, NULL)) != NULL) {
            WCHAR cls[128] = {0};
            if (GetClassNameW(hChild, cls, ARRAYSIZE(cls)) > 0) {
                if (_wcsicmp(cls, L"TrayInputIndicatorWClass") == 0 ||
                    _wcsicmp(cls, L"InputIndicatorButton") == 0 ||
                    _wcsicmp(cls, L"InputIndicator") == 0 ||
                    _wcsicmp(cls, L"TipBandNotificationArea") == 0) {
                    HWND prev = NULL;
                    bool needSubclass = false;
                    EnterCriticalSection(&g_Ctx.csLock);
                    if (hChild != G_hSubclassedIndicator) {
                        prev = G_hSubclassedIndicator;
                        G_hSubclassedIndicator = hChild;
                        needSubclass = true;
                    }
                    LeaveCriticalSection(&g_Ctx.csLock);
                    if (needSubclass) {
                        if (prev) {
                            WindhawkUtils::RemoveWindowSubclassFromAnyThread(prev, InputIndicatorButtonProc);
                        }
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hChild, InputIndicatorButtonProc, 0)) {
                            Wh_Log(L"Win78LangSwitcher: Subclassed Input Indicator %s (0x%p)", cls, hChild);
                        }
                    }
                    break;
                }
            }
        }
    }

    // Enumerate every secondary taskbar (one per additional monitor) instead
    // of a single FindWindow, otherwise only one is intercepted and the
    // "old vs new" tracking below would thrash each time this runs.
    std::vector<HWND> newSec;       // to subclass outside the lock
    std::vector<HWND> staleSec;     // to unsubclass outside the lock
    EnterCriticalSection(&g_Ctx.csLock);
    {
        HWND hEnum = NULL;
        while ((hEnum = FindWindowExW(NULL, hEnum, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
            HWND hSecToolbar = FindWindowExW(hEnum, NULL, L"ToolbarWindow32", NULL);
            if (!hSecToolbar) continue;

            bool alreadyPresent = false;
            for (HWND h : G_hSubclassedSecToolbars) {
                if (h == hSecToolbar) { alreadyPresent = true; break; }
            }
            if (!alreadyPresent) {
                G_hSubclassedSecToolbars.push_back(hSecToolbar);
                newSec.push_back(hSecToolbar);
            }
        }
        // Drop recorded secondary toolbars that no longer exist.
        for (auto it = G_hSubclassedSecToolbars.begin(); it != G_hSubclassedSecToolbars.end(); ) {
            if (!IsWindow(*it)) {
                staleSec.push_back(*it);
                it = G_hSubclassedSecToolbars.erase(it);
            } else {
                ++it;
            }
        }
    }
    LeaveCriticalSection(&g_Ctx.csLock);

    for (HWND h : staleSec) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, ToolbarWndProc);
    }
    for (HWND h : newSec) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(h, ToolbarWndProc, 0)) {
            Wh_Log(L"Win78LangSwitcher: Subclassed secondary toolbar (0x%p)", h);
        }
    }

    // Report success only when at least one intended target is actually
    // subclassed. The caller keeps retrying otherwise, so a layout indicator
    // or tray tree rebuilt later is still picked up.
    EnterCriticalSection(&g_Ctx.csLock);
    const bool anySubclassed =
        (G_hSubclassedToolbar != NULL && IsWindow(G_hSubclassedToolbar)) ||
        (G_hSubclassedIndicator != NULL && IsWindow(G_hSubclassedIndicator)) ||
        !G_hSubclassedSecToolbars.empty();
    LeaveCriticalSection(&g_Ctx.csLock);
    return anySubclassed;
}

static void RemoveTrayInterception() {
    HWND hToolbar = NULL, hIndicator = NULL;
    std::vector<HWND> secToolbars;
    EnterCriticalSection(&g_Ctx.csLock);
    hToolbar = G_hSubclassedToolbar;
    hIndicator = G_hSubclassedIndicator;
    secToolbars.swap(G_hSubclassedSecToolbars);
    G_hSubclassedToolbar = NULL;
    G_hSubclassedIndicator = NULL;
    LeaveCriticalSection(&g_Ctx.csLock);

    if (hToolbar) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hToolbar, ToolbarWndProc);
    }
    if (hIndicator) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hIndicator, InputIndicatorButtonProc);
    }
    for (HWND hSec : secToolbars) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hSec, ToolbarWndProc);
    }
}

static bool IsModifierVk(DWORD vk) {
    return vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU ||
           vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT ||
           vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL ||
           vk == VK_LWIN || vk == VK_RWIN;
}

static void RememberForegroundTarget() {
    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    HWND hFore = GetForegroundWindow();
    if (hFore && hFore != hFlyout) {
        if (!IsTaskbarWindow(hFore)) {
            g_targetWindow.store(hFore, std::memory_order_release);
        } else {
            HWND hCurrent = AtomicLoadHwnd(g_targetWindow);
            if (hCurrent && IsWindow(hCurrent) && !IsTaskbarWindow(hCurrent)) {
            } else {
                HWND hReal = GetWindow(GetDesktopWindow(), GW_CHILD);
                while (hReal) {
                    if (IsWindowVisible(hReal) && !IsTaskbarWindow(hReal)) {
                        g_targetWindow.store(hReal, std::memory_order_release);
                        break;
                    }
                    hReal = GetWindow(hReal, GW_HWNDNEXT);
                }
            }
        }
    }
}

static bool WinKeysReleased() {
    return ((GetAsyncKeyState(VK_LWIN) & 0x8000) == 0) &&
           ((GetAsyncKeyState(VK_RWIN) & 0x8000) == 0);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !lParam || g_Ctx.isUninitializing) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // A second explorer instance must never process input on our behalf.
    if (!g_isMainShell.load(std::memory_order_relaxed)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (kbd->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    static thread_local bool s_inKbdHook = false;
    if (s_inKbdHook) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    s_inKbdHook = true;

    try {
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        // Fallback: if a Win+Space cycle is stuck (the VK_LWIN/RWIN key-up was
        // swallowed by an earlier low-level hook, or the mod was enabled while
        // the Win key was held), clear the flag as soon as both Win keys are
        // observably released. Otherwise the topmost flyout can't auto-hide on
        // focus loss and layout re-syncing stays frozen. (The worker's retry
        // timer mirrors this as a safety net even if no further key events
        // arrive.)
        if (g_isWinSpaceCycling.load(std::memory_order_relaxed) && WinKeysReleased()) {
            g_isWinSpaceCycling.store(false, std::memory_order_relaxed);
        }

        HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);

        if (isKeyDown && kbd->vkCode == VK_ESCAPE) {
            if (hFlyout && IsWindow(hFlyout) && IsWindowVisible(hFlyout)) {
                if (g_Ctx.dwWorkerThreadId) {
                    PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_HIDE_SWITCHER, 0, 0);
                }
                s_inKbdHook = false;
                return 1;
            }
        }

        bool isWinDown = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

        if (g_enableWinSpace.load(std::memory_order_relaxed) && kbd->vkCode == VK_SPACE && isWinDown) {
            if (isKeyDown) {
                if (!g_isWinSpaceCycling.load(std::memory_order_relaxed)) {
                    g_isWinSpaceCycling.store(true, std::memory_order_relaxed);
                    RememberForegroundTarget();
                    keybd_event(VK_CONTROL, 0, 0, 0);
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                }

                bool isShiftHeld = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
                if (g_Ctx.dwWorkerThreadId) {
                    PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_CYCLE_SWITCHER, isShiftHeld ? 0 : 1, 0);
                }
                s_inKbdHook = false;
                return 1;
            }
        }

        if (g_isWinSpaceCycling.load(std::memory_order_relaxed) && isKeyUp &&
            (kbd->vkCode == VK_LWIN || kbd->vkCode == VK_RWIN)) {
            g_isWinSpaceCycling.store(false, std::memory_order_relaxed);
            if (g_Ctx.dwWorkerThreadId) {
                PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_APPLY_SELECTION, 0, 0);
            }
            s_inKbdHook = false;
            return 0;
        }

        // Alt+Shift / Ctrl+Shift: arm on the second modifier down, fire on
        // modifier-up only if no intervening non-modifier key was pressed.
        if (g_enableAltShift.load(std::memory_order_relaxed)) {
            if (kbd->vkCode == VK_LMENU || kbd->vkCode == VK_RMENU || kbd->vkCode == VK_MENU) {
                if (isKeyDown) {
                    g_altPressed.store(true, std::memory_order_relaxed);
                    if (g_shiftPressed.load(std::memory_order_relaxed) &&
                        !g_interveningKeyPressed.load(std::memory_order_relaxed)) {
                        g_altShiftChordArmed.store(true, std::memory_order_relaxed);
                    }
                } else if (isKeyUp) {
                    bool fire = g_altShiftChordArmed.exchange(false) &&
                                !g_interveningKeyPressed.load(std::memory_order_relaxed);
                    g_altPressed.store(false, std::memory_order_relaxed);
                    if (fire) {
                        RememberForegroundTarget();
                        if (g_Ctx.dwWorkerThreadId) {
                            PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_CYCLE_AND_SWITCH, 1, 0);
                        }
                    }
                    if (!g_shiftPressed.load(std::memory_order_relaxed) && !g_ctrlPressed.load(std::memory_order_relaxed)) {
                        g_interveningKeyPressed.store(false, std::memory_order_relaxed);
                    }
                }
            } else if (kbd->vkCode == VK_LSHIFT || kbd->vkCode == VK_RSHIFT || kbd->vkCode == VK_SHIFT) {
                if (isKeyDown) {
                    g_shiftPressed.store(true, std::memory_order_relaxed);
                    if (g_altPressed.load(std::memory_order_relaxed) &&
                        !g_interveningKeyPressed.load(std::memory_order_relaxed)) {
                        g_altShiftChordArmed.store(true, std::memory_order_relaxed);
                    } else if (g_ctrlPressed.load(std::memory_order_relaxed) &&
                               !g_interveningKeyPressed.load(std::memory_order_relaxed)) {
                        g_ctrlShiftChordArmed.store(true, std::memory_order_relaxed);
                    }
                } else if (isKeyUp) {
                    bool fireAlt = g_altShiftChordArmed.exchange(false) &&
                                   !g_interveningKeyPressed.load(std::memory_order_relaxed);
                    bool fireCtrl = g_ctrlShiftChordArmed.exchange(false) &&
                                    !g_interveningKeyPressed.load(std::memory_order_relaxed);
                    g_shiftPressed.store(false, std::memory_order_relaxed);
                    if (fireAlt || fireCtrl) {
                        RememberForegroundTarget();
                        if (g_Ctx.dwWorkerThreadId) {
                            PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_CYCLE_AND_SWITCH, 1, 0);
                        }
                    }
                    if (!g_altPressed.load(std::memory_order_relaxed) && !g_ctrlPressed.load(std::memory_order_relaxed)) {
                        g_interveningKeyPressed.store(false, std::memory_order_relaxed);
                    }
                }
            } else if (kbd->vkCode == VK_LCONTROL || kbd->vkCode == VK_RCONTROL || kbd->vkCode == VK_CONTROL) {
                if (isKeyDown) {
                    g_ctrlPressed.store(true, std::memory_order_relaxed);
                    if (g_shiftPressed.load(std::memory_order_relaxed) &&
                        !g_interveningKeyPressed.load(std::memory_order_relaxed)) {
                        g_ctrlShiftChordArmed.store(true, std::memory_order_relaxed);
                    }
                } else if (isKeyUp) {
                    bool fire = g_ctrlShiftChordArmed.exchange(false) &&
                                !g_interveningKeyPressed.load(std::memory_order_relaxed);
                    g_ctrlPressed.store(false, std::memory_order_relaxed);
                    if (fire) {
                        RememberForegroundTarget();
                        if (g_Ctx.dwWorkerThreadId) {
                            PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_CYCLE_AND_SWITCH, 1, 0);
                        }
                    }
                    if (!g_altPressed.load(std::memory_order_relaxed) && !g_shiftPressed.load(std::memory_order_relaxed)) {
                        g_interveningKeyPressed.store(false, std::memory_order_relaxed);
                    }
                }
            } else if (isKeyDown && !IsModifierVk(kbd->vkCode)) {
                g_interveningKeyPressed.store(true, std::memory_order_relaxed);
                g_altShiftChordArmed.store(false, std::memory_order_relaxed);
                g_ctrlShiftChordArmed.store(false, std::memory_order_relaxed);
            }
        }

        static bool s_customHotkeyActive = false;
        if (g_enableCustomHotkey.load(std::memory_order_relaxed) && kbd->vkCode == 'L') {
            bool isCtrlDown = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
            bool isShiftDown = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
            bool isAltDown = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
            if (isKeyDown && isCtrlDown && isShiftDown && !isAltDown) {
                g_interveningKeyPressed.store(true, std::memory_order_relaxed);
                g_ctrlShiftChordArmed.store(false, std::memory_order_relaxed);
                if (!s_customHotkeyActive) {
                    s_customHotkeyActive = true;
                    RememberForegroundTarget();
                    if (g_Ctx.dwWorkerThreadId) {
                        PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_SHOW_FLYOUT, 0, 0);
                    }
                }
                s_inKbdHook = false;
                return 1;
            }
            if (isKeyUp) s_customHotkeyActive = false;
        }
    } catch (...) {}

    s_inKbdHook = false;
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool IsPointOnHookedTrayControl(const POINT& pt) {
    HWND hHit = WindowFromPoint(pt);
    if (!hHit) {
        return false;
    }
    HWND hToolbar = NULL, hIndicator = NULL;
    std::vector<HWND> secToolbars;
    EnterCriticalSection(&g_Ctx.csLock);
    hToolbar = G_hSubclassedToolbar;
    hIndicator = G_hSubclassedIndicator;
    secToolbars = G_hSubclassedSecToolbars;
    LeaveCriticalSection(&g_Ctx.csLock);

    if (hHit == hToolbar || hHit == hIndicator) {
        return true;
    }
    for (HWND hSec : secToolbars) {
        if (hHit == hSec) return true;
    }
    HWND hParent = GetAncestor(hHit, GA_PARENT);
    return (hParent == hIndicator);
}

static void HideFlyoutIfClickOutside(const POINT& pt) {
    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    if (!hFlyout || !IsWindow(hFlyout) || !IsWindowVisible(hFlyout)) {
        return;
    }

    RECT rc = {0};
    if (!GetWindowRect(hFlyout, &rc)) {
        return;
    }
    if (PtInRect(&rc, pt)) {
        return;
    }

    if (IsPointOnHookedTrayControl(pt)) {
        return;
    }

    g_lastInactiveTick.store(GetTickCount(), std::memory_order_relaxed);

    if (g_Ctx.dwWorkerThreadId) {
        PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_HIDE_SWITCHER, 0, 0);
    }
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Low-level hooks run on the installing thread and block all system input
    // until they return, so never do anything heavy (WindowFromPoint hit-tests
    // by sending WM_NCHITTEST across processes, which can stall the whole mouse
    // on a hung app). Just pack the point and hand it to the worker thread.
    if (nCode == HC_ACTION && lParam && !g_Ctx.isUninitializing &&
        g_isMainShell.load(std::memory_order_relaxed)) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN ||
            wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN) {
            auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            if (ms && !(ms->flags & LLMHF_INJECTED)) {
                if (g_Ctx.dwWorkerThreadId) {
                    PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_CLICK_OUTSIDE_TEST,
                                       (WPARAM)(LONG)ms->pt.x, (LPARAM)(LONG)ms->pt.y);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Dedicated hook thread: only pumps low-level hooks. Never does UI work.
static DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    ModContext* ctx = (ModContext*)lpParam;
    if (!ctx) return 1;

    // Force the thread message queue into existence before signalling, so
    // SafeCleanup's PostThreadMessageW(WM_QUIT) can never fail with
    // ERROR_INVALID_THREAD_ID (a thread has no queue until it first calls a
    // message API).
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(ctx->hHookReadyEvent);

    HHOOK hKbd = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModInstance(), 0);
    if (hKbd) {
        g_keyboardHook.reset(hKbd);
    }

    HHOOK hMouse = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModInstance(), 0);
    if (hMouse) {
        g_mouseHook.reset(hMouse);
    } else {
        Wh_Log(L"Win78LangSwitcher: WH_MOUSE_LL install failed, outside-click dismissal will rely on WM_ACTIVATE only");
    }

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_keyboardHook.reset();
    g_mouseHook.reset();
    return 0;
}

// Dedicated worker thread: owns the flyout window, the layout state, and the
// tray interception. It re-evaluates the shell role on a periodic tick because
// the owner of Shell_TrayWnd (and the shape of the tray tree) can change at any
// time, and because TaskbarCreated is a HWND_BROADCAST message that is never
// delivered to a thread's message queue — so a timer is the only reliable way
// to pick up a rebuilt tray / late-appearing layout indicator.
static DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    ModContext* ctx = (ModContext*)lpParam;
    if (!ctx) return 1;

    // Force the thread message queue into existence before signalling (see
    // HookThreadProc).
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(ctx->hWorkerReadyEvent);

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    InitGdiPlusRendering();
    RefreshKeyboardLayouts();

    // Establish the shell role immediately, then keep re-evaluating it. This is
    // intentionally never baked in once at Wh_ModInit time.
    EvaluateShellRole();
    UINT_PTR trayRetryTimer = SetTimer(NULL, 0, 1500, NULL);

    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TIMER && msg.wParam == trayRetryTimer) {
            if (!ctx->isUninitializing) {
                EvaluateShellRole();
            }
        } else if (msg.message == WM_TOGGLE_FLYOUT_REQUEST && !ctx->isUninitializing) {
            ToggleFlyoutWindow();
        } else if (msg.message == WM_SHOW_FLYOUT && !ctx->isUninitializing) {
            ShowFlyoutWindow();
        } else if (msg.message == WM_APP_CLICK_OUTSIDE_TEST && !ctx->isUninitializing) {
            POINT pt;
            pt.x = (LONG)msg.wParam;
            pt.y = (LONG)msg.lParam;
            HideFlyoutIfClickOutside(pt);
        } else if (msg.message == WM_APP_CYCLE_SWITCHER && !ctx->isUninitializing) {
            bool forward = (msg.wParam != 0);
            CycleSwitcher(forward, false);
        } else if (msg.message == WM_APP_CYCLE_AND_SWITCH && !ctx->isUninitializing) {
            bool forward = (msg.wParam != 0);
            CycleSwitcher(forward, true, /*showFlyout=*/false);
        } else if (msg.message == WM_APP_APPLY_SELECTION && !ctx->isUninitializing) {
            ApplySelection();
        } else if (msg.message == WM_APP_HIDE_SWITCHER && !ctx->isUninitializing) {
            HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
            if (hFlyout && IsWindow(hFlyout)) {
                ShowWindow(hFlyout, SW_HIDE);
            }
        } else if (msg.message == WM_APP_REFRESH_INSTALL && !ctx->isUninitializing) {
            EvaluateShellRole();
        } else if (msg.message == WM_SAFE_CLOSE) {
            HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
            if (hFlyout && IsWindow(hFlyout)) {
                DestroyWindow(hFlyout);
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    if (hFlyout && IsWindow(hFlyout)) {
        DestroyWindow(hFlyout);
    }

    // Safety net: if SafeCleanup's RemoveTrayInterception ran just before this
    // thread finished installing subclasses, remove them here so no WNDPROC
    // pointing into the soon-to-be-unmapped image survives this thread.
    RemoveTrayInterception();

    if (trayRetryTimer) KillTimer(NULL, trayRetryTimer);
    CoUninitialize();
    return 0;
}

static void LoadSettings() {
    try {
        ModSettings newSettings;

        const wchar_t* styleSetting = Wh_GetStringSetting(L"switcherStyle");
        if (styleSetting) {
            if (_wcsicmp(styleSetting, L"win7") == 0) {
                newSettings.switcherStyle = SwitcherStyle::Win7;
            } else {
                newSettings.switcherStyle = SwitcherStyle::Win8;
            }
            Wh_FreeStringSetting(styleSetting);
        }

        const wchar_t* langSetting = Wh_GetStringSetting(L"language");
        if (langSetting) {
            newSettings.uiLanguage = langSetting;
            Wh_FreeStringSetting(langSetting);
        }

        const wchar_t* themeSetting = Wh_GetStringSetting(L"themeMode");
        if (themeSetting) {
            if (_wcsicmp(themeSetting, L"auto") == 0) {
                newSettings.themeMode = ThemeMode::Auto;
            } else if (_wcsicmp(themeSetting, L"dark") == 0) {
                newSettings.themeMode = ThemeMode::Dark;
            } else if (_wcsicmp(themeSetting, L"light") == 0) {
                newSettings.themeMode = ThemeMode::Light;
            } else if (_wcsicmp(themeSetting, L"custom") == 0) {
                newSettings.themeMode = ThemeMode::Custom;
            } else {
                newSettings.themeMode = ThemeMode::Win8Purple;
            }
            Wh_FreeStringSetting(themeSetting);
        }

        const wchar_t* customColorSetting = Wh_GetStringSetting(L"customAccentColor");
        if (customColorSetting) {
            newSettings.customAccentColor = ParseHexColor(customColorSetting, RGB(91, 44, 130));
            Wh_FreeStringSetting(customColorSetting);
        }

        const wchar_t* prefCmdSetting = Wh_GetStringSetting(L"customPreferencesCmd");
        if (prefCmdSetting) {
            newSettings.customPreferencesCmd = prefCmdSetting;
            Wh_FreeStringSetting(prefCmdSetting);
        }

        newSettings.enableWinSpace = (Wh_GetIntSetting(L"enableWinSpace") != 0);
        newSettings.enableAltShift = (Wh_GetIntSetting(L"enableAltShift") != 0);
        newSettings.hookTrayClicks = (Wh_GetIntSetting(L"hookTrayClicks") != 0);
        newSettings.showShortcutHint = (Wh_GetIntSetting(L"showShortcutHint") != 0);
        newSettings.enableCustomHotkey = (Wh_GetIntSetting(L"enableCustomHotkey") != 0);

        EnterCriticalSection(&g_Ctx.csLock);
        g_settings = newSettings;
        LeaveCriticalSection(&g_Ctx.csLock);

        g_enableWinSpace.store(newSettings.enableWinSpace, std::memory_order_relaxed);
        g_enableAltShift.store(newSettings.enableAltShift, std::memory_order_relaxed);
        g_hookTrayClicks.store(newSettings.hookTrayClicks, std::memory_order_relaxed);
        g_enableCustomHotkey.store(newSettings.enableCustomHotkey, std::memory_order_relaxed);
    } catch (...) {}
}

static void SafeCleanup() {
    if (InterlockedExchange(&g_Ctx.isUninitializing, 1L)) return;

    RemoveTrayInterception();

    HWND hFlyout = AtomicLoadHwnd(g_hFlyoutWnd);
    DWORD ownerTid = g_dwFlyoutOwnerThreadId.load(std::memory_order_acquire);
    if (hFlyout && IsWindow(hFlyout) && ownerTid) {
        PostThreadMessageW(ownerTid, WM_SAFE_CLOSE, 0, 0);
    }

    // Wh_ModInit waited on the ready events before returning, so both threads
    // are guaranteed to have created a message queue (via PeekMessage) by now
    // and these WM_QUIT posts cannot be silently dropped with
    // ERROR_INVALID_THREAD_ID.
    if (g_Ctx.dwHookThreadId) {
        PostThreadMessageW(g_Ctx.dwHookThreadId, WM_QUIT, 0, 0);
    }
    if (g_Ctx.dwWorkerThreadId) {
        PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_QUIT, 0, 0);
    }

    // Must wait forever: returning while either thread still runs leaves
    // Windhawk free to FreeLibrary the image under executing code.
    if (g_Ctx.hHookThread) {
        WaitForSingleObject(g_Ctx.hHookThread, INFINITE);
        CloseHandle(g_Ctx.hHookThread);
        g_Ctx.hHookThread = NULL;
        g_Ctx.dwHookThreadId = 0;
    }
    if (g_Ctx.hWorkerThread) {
        WaitForSingleObject(g_Ctx.hWorkerThread, INFINITE);
        CloseHandle(g_Ctx.hWorkerThread);
        g_Ctx.hWorkerThread = NULL;
        g_Ctx.dwWorkerThreadId = 0;
    }

    if (g_Ctx.hWorkerReadyEvent) { CloseHandle(g_Ctx.hWorkerReadyEvent); g_Ctx.hWorkerReadyEvent = NULL; }
    if (g_Ctx.hHookReadyEvent) { CloseHandle(g_Ctx.hHookReadyEvent); g_Ctx.hHookReadyEvent = NULL; }

    ShutdownGdiPlusRendering();
    g_hFlyoutWnd.store(NULL, std::memory_order_release);
    g_dwFlyoutOwnerThreadId.store(0, std::memory_order_release);
    g_Initialized = FALSE;
}

static bool IsExplorerProcess() {
    WCHAR exePath[MAX_PATH] = {};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* name = wcsrchr(exePath, L'\\');
    name = name ? name + 1 : exePath;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

static DWORD GetTrayOwnerPid() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) tray = FindWindowW(L"Progman", nullptr);
    DWORD pid = 0;
    if (tray) GetWindowThreadProcessId(tray, &pid);
    return pid;
}

// Only the explorer that owns the taskbar may actively handle input: the input
// indicator, its flyout and the tray toolbars all live in that process. A
// second explorer instance in the same session (e.g. a stale one still running
// after a taskbar restart) must not install its own low-level keyboard/mouse
// hooks, or it would double-handle every Win+Space / Alt+Shift / click. The
// decision is re-checked over time (see EvaluateShellRole), never baked in.
static bool IsMainExplorerShell() {
    DWORD owner = GetTrayOwnerPid();
    if (!owner) return true;
    return owner == GetCurrentProcessId();
}

// The owner of Shell_TrayWnd and the shape of the tray tree are not fixed for
// the lifetime of a process, so this is re-run by the worker thread's retry
// tick (and on settings changes). It installs tray interception and enables
// input handling only while this explorer instance is the shell; otherwise it
// tears everything down so a second explorer never double-handles input.
static void EvaluateShellRole() {
    if (g_Ctx.isUninitializing) return;

    // Safety-net for a stuck Win+Space cycle (see LowLevelKeyboardProc).
    if (g_isWinSpaceCycling.load(std::memory_order_relaxed) && WinKeysReleased()) {
        g_isWinSpaceCycling.store(false, std::memory_order_relaxed);
    }

    // Re-check this explorer's identity rather than trusting a value baked in
    // at Wh_ModInit time.
    const bool isShell = IsExplorerProcess() && IsMainExplorerShell();
    g_isMainShell.store(isShell, std::memory_order_relaxed);

    if (isShell) {
        // Idempotent; keeps retrying until at least one target is subclassed.
        InstallTrayInterceptionInternal();
    } else {
        RemoveTrayInterception();
    }
}
BOOL Wh_ModInit() {
    Wh_Log(L"=== Win78LangSwitcher: Wh_ModInit v1.0.0 ===");

    ZeroMemory(&g_Ctx, sizeof(g_Ctx));
    InitializeCriticalSection(&g_Ctx.csLock);

    LoadSettings();

    if (!IsExplorerProcess()) {
        g_Initialized = TRUE;
        return TRUE;
    }

    // FIX: Unload in secondary explorer processes
    if (!IsMainExplorerShell()) {
        Wh_Log(L"Win78LangSwitcher: Not main explorer shell (pid=%lu, owner=%lu), unloading", 
               GetCurrentProcessId(), GetTrayOwnerPid());
        g_Initialized = TRUE;
        DeleteCriticalSection(&g_Ctx.csLock);
        return FALSE;
    }

    const DWORD dwTrayOwner = GetTrayOwnerPid();
    const bool mainShell = IsMainExplorerShell();
    Wh_Log(L"Win78LangSwitcher: pid=%lu trayOwner=%lu mainShell=%d", GetCurrentProcessId(), dwTrayOwner, mainShell ? 1 : 0);
    // Seed the shell flag before any thread runs. This is NOT a permanent
    // decision: the worker thread re-evaluates it on its retry tick, because
    // the owner of Shell_TrayWnd can change later (e.g. after an Explorer /
    // taskbar restart). We always start our threads so this instance can
    // become (or stop being) the active shell without a reload.
    g_isMainShell.store(mainShell, std::memory_order_relaxed);

    // Readiness events let us wait until each thread has created its message
    // queue (PeekMessage) before Wh_ModInit returns — which is what makes the
    // unconditional waits in SafeCleanup safe.
    g_Ctx.hWorkerReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_Ctx.hHookReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_Ctx.hWorkerReadyEvent || !g_Ctx.hHookReadyEvent) {
        Wh_Log(L"Win78LangSwitcher: CreateEvent failed");
        InterlockedExchange(&g_Ctx.isUninitializing, 1L);
        if (g_Ctx.hWorkerReadyEvent) CloseHandle(g_Ctx.hWorkerReadyEvent);
        if (g_Ctx.hHookReadyEvent) CloseHandle(g_Ctx.hHookReadyEvent);
        g_Ctx.hWorkerReadyEvent = g_Ctx.hHookReadyEvent = NULL;
        DeleteCriticalSection(&g_Ctx.csLock);
        return FALSE;
    }

    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        void* pShowWindow = reinterpret_cast<void*>(reinterpret_cast<FARPROC>(GetProcAddress(hUser, "ShowWindow")));
        if (pShowWindow) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShowWindow_t>(pShowWindow),
                                           ShowWindow_Hook, &ShowWindow_Original);
            Wh_Log(L"Win78LangSwitcher: ShowWindow hook installed successfully");
        }
    }

    g_Ctx.hWorkerThread = CreateThread(NULL, 0, WorkerThreadProc, &g_Ctx, 0, &g_Ctx.dwWorkerThreadId);
    if (!g_Ctx.hWorkerThread) {
        Wh_Log(L"Win78LangSwitcher: worker thread creation failed");
        InterlockedExchange(&g_Ctx.isUninitializing, 1L);
        if (g_Ctx.hWorkerReadyEvent) CloseHandle(g_Ctx.hWorkerReadyEvent);
        if (g_Ctx.hHookReadyEvent) CloseHandle(g_Ctx.hHookReadyEvent);
        g_Ctx.hWorkerReadyEvent = g_Ctx.hHookReadyEvent = NULL;
        DeleteCriticalSection(&g_Ctx.csLock);
        return FALSE;
    }
    WaitForSingleObject(g_Ctx.hWorkerReadyEvent, 5000);

    g_Ctx.hHookThread = CreateThread(NULL, 0, HookThreadProc, &g_Ctx, 0, &g_Ctx.dwHookThreadId);
    if (!g_Ctx.hHookThread) {
        Wh_Log(L"Win78LangSwitcher: hook thread creation failed");
        // The worker's queue is guaranteed to exist by now, so WM_QUIT will be
        // delivered and the wait below cannot hang.
        InterlockedExchange(&g_Ctx.isUninitializing, 1L);
        PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_Ctx.hWorkerThread, INFINITE);
        CloseHandle(g_Ctx.hWorkerThread);
        g_Ctx.hWorkerThread = NULL;
        g_Ctx.dwWorkerThreadId = 0;
        if (g_Ctx.hWorkerReadyEvent) CloseHandle(g_Ctx.hWorkerReadyEvent);
        if (g_Ctx.hHookReadyEvent) CloseHandle(g_Ctx.hHookReadyEvent);
        g_Ctx.hWorkerReadyEvent = g_Ctx.hHookReadyEvent = NULL;
        DeleteCriticalSection(&g_Ctx.csLock);
        return FALSE;
    }
    WaitForSingleObject(g_Ctx.hHookReadyEvent, 5000);

    // Tray installation happens on the worker thread (EvaluateShellRole).
    g_Initialized = TRUE;
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (!IsExplorerProcess()) {
        return;
    }
    // Re-evaluate the shell role / tray installation immediately so a settings
    // change (e.g. toggling callbacks off) is applied without waiting for the
    // 1.5s retry tick. The worker thread is always started for an explorer
    // process, and EvaluateShellRole itself re-verifies tray ownership, so we
    // never subclass a foreign taskbar here.
    if (g_Ctx.dwWorkerThreadId && !g_Ctx.isUninitializing) {
        PostThreadMessageW(g_Ctx.dwWorkerThreadId, WM_APP_REFRESH_INSTALL, 0, 0);
    }
}

void Wh_ModBeforeUninit() {
    SafeCleanup();
}

void Wh_ModUninit() {
    SafeCleanup();
    DeleteCriticalSection(&g_Ctx.csLock);
    UnregisterClassW(kFlyoutClassName, HINST_THISCOMPONENT);
}
