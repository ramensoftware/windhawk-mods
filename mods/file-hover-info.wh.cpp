// ==WindhawkMod==
// @id              file-hover-info
// @name            File Hover Info
// @description     A themeable info card with size, dates and more when you hover files on the desktop or in File Explorer
// @version         1.1.2
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32 -ld2d1 -ldwrite -ldwmapi -lshlwapi -lgdi32 -luser32 -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Hover Info

Rest the cursor on a file, folder, drive or shortcut — on the desktop or in a
File Explorer window — and a card appears with the details you choose: size,
folder size, free space on drives, creation / modification / access dates,
attributes, image dimensions, media duration, file version, shortcut target,
folder contents and location.

![File Hover Info card](https://i.imgur.com/Gk2ZcFl.png)

The card follows the Windows display language (English, Italian, Spanish,
French, German, Portuguese and Simplified Chinese are included; other
languages fall back to English), or you can pick one.

Everything is configurable: which fields to show, date style (absolute,
relative or both), theme (follow Windows, dark, light or fully custom colors),
solid / acrylic / blur background, accent color and bar, corner radius,
shadow, fade-in, font, size, icon or thumbnail.

## Windows' own popup
By default the mod hides Windows' built-in hover popup, so only the card is
shown. This only affects Explorer while the mod is running: the Windows
setting itself is not changed, and the popup comes back as soon as the option
or the mod is turned off. The option also hides the small tooltip Windows
shows for desktop icon names that are cut off, since the card shows the full
name anyway.

## Folder size
Folder sizes are summed on a background thread, so Explorer is never blocked.
The card shows "Calculating…" and updates when the total is ready. The
calculation stops as soon as the cursor moves away, and after the time limit
set in the options (the card then shows a partial total). Drives show their
free space instead of being scanned.

## Notes
- Acrylic and blur use an undocumented Windows API; on some builds the blur
  may not follow the rounded corners.
- Items are matched by their displayed name. If two items in the same folder
  show the same name (e.g. `photo.jpg` and `photo.png` with extensions
  hidden), the first one is shown.
- Pages that aren't classic folder views (Home, Gallery) are not supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delay: 450
  $name: Hover delay (ms)
  $description: How long the cursor must rest on an item before the card appears
- showOnDesktop: true
  $name: Show on the desktop
- showInExplorer: true
  $name: Show in File Explorer windows
- hideNativeTooltip: true
  $name: Hide Windows' own hover popup
  $description: Hides the built-in popup so only this card appears. The Windows setting is not changed.
- language: auto
  $name: Card language
  $options:
  - auto: Same as Windows (English if not available)
  - en: English
  - it: Italiano
  - es: Español
  - fr: Français
  - de: Deutsch
  - pt: Português
  - zh: 简体中文 (Simplified Chinese)
- folderSizeLimit: 10
  $name: Folder size - maximum calculation time (seconds)
  $description: Large folders are summed in the background. When the limit is hit the card shows a partial total.
- dateStyle: shortRelative
  $name: Date format
  $options:
  - long: Long date and time
  - short: Short date and time
  - relative: Relative (e.g. "2 hours ago")
  - shortRelative: Short date + relative
- fields:
  - type: true
    $name: Type
  - size: true
    $name: Size
  - exactBytes: false
    $name: Exact size in bytes
  - created: true
    $name: Created
  - modified: true
    $name: Modified
  - accessed: false
    $name: Last accessed
  - attributes: true
    $name: Attributes (hidden, read-only, online-only...)
  - dimensions: true
    $name: Image dimensions
  - duration: true
    $name: Audio/video duration
  - version: true
    $name: File version (exe, dll)
  - target: true
    $name: Shortcut target
  - folderSize: true
    $name: Folder size (calculated in the background)
  - contents: true
    $name: Folder contents (number of items)
  - folderPreview: true
    $name: Folder preview (names of the first subfolders and files)
  - location: false
    $name: Location (parent folder)
  $name: Information to show
- appearance:
  - theme: auto
    $name: Theme
    $options:
    - auto: Follow Windows
    - dark: Dark
    - light: Light
    - custom: Custom colors
  - background: solid
    $name: Background
    $options:
    - solid: Solid
    - acrylic: Acrylic
    - blur: Blur
  - opacity: 94
    $name: Background opacity (%)
  - accentColor: ""
    $name: Accent color
    $description: "#RRGGBB. Leave empty to use the Windows accent color"
  - accentBar: true
    $name: Accent bar on the left
  - shadow: true
    $name: Drop shadow (solid background)
  - animation: true
    $name: Fade-in (solid background)
  - cornerRadius: 8
    $name: Corner radius (solid background)
  - fontFamily: Segoe UI Variable Text
    $name: Font
  - fontSize: 13
    $name: Font size
  - maxWidth: 380
    $name: Maximum width
  - icon: true
    $name: Show icon
  - thumbnail: true
    $name: Use thumbnails for images and videos
  - iconSize: 40
    $name: Icon size
  - customBackground: "#1E1E2E"
    $name: Custom background color
    $description: Used when the theme is "Custom colors"
  - customText: "#CDD6F4"
    $name: Custom text color
  - customLabel: "#9399B2"
    $name: Custom label color
  $name: Appearance
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <servprov.h>
#include <propsys.h>
#include <oleacc.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <memory>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Small COM smart pointer
// ---------------------------------------------------------------------------
template <class T>
class CPtr {
   public:
    T* p = nullptr;
    CPtr() = default;
    CPtr(const CPtr&) = delete;
    CPtr& operator=(const CPtr&) = delete;
    CPtr(CPtr&& o) noexcept : p(o.p) { o.p = nullptr; }
    CPtr& operator=(CPtr&& o) noexcept {
        if (this != std::addressof(o)) {
            Reset();
            p = o.p;
            o.p = nullptr;
        }
        return *this;
    }
    ~CPtr() { Reset(); }
    void Reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** operator&() {
        Reset();
        return &p;
    }
    T* operator->() const { return p; }
    operator T*() const { return p; }
};

// ---------------------------------------------------------------------------
// GUIDs and property keys (defined locally to avoid linker surprises)
// ---------------------------------------------------------------------------
static const CLSID kCLSID_ShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
static const GUID kSID_STopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

#define FMTID_STORAGE_ \
    {0xb725f130, 0x47ef, 0x101a, {0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac}}
static const PROPERTYKEY kPK_ItemTypeText = {FMTID_STORAGE_, 4};
static const PROPERTYKEY kPK_Size = {FMTID_STORAGE_, 12};
static const PROPERTYKEY kPK_DateModified = {FMTID_STORAGE_, 14};
static const PROPERTYKEY kPK_DateCreated = {FMTID_STORAGE_, 15};
static const PROPERTYKEY kPK_DateAccessed = {FMTID_STORAGE_, 16};
static const PROPERTYKEY kPK_ImageDimensions = {
    {0x6444048f, 0x4c8b, 0x11d1, {0x8b, 0x70, 0x08, 0x00, 0x36, 0xb1, 0x1a, 0x03}},
    13};
static const PROPERTYKEY kPK_MediaDuration = {
    {0x64440490, 0x4c8b, 0x11d1, {0x8b, 0x70, 0x08, 0x00, 0x36, 0xb1, 0x1a, 0x03}},
    3};
static const PROPERTYKEY kPK_LinkTarget = {
    {0xb9b4b3fc, 0x2b51, 0x4a42, {0xb5, 0xd8, 0x32, 0x41, 0x46, 0xaf, 0xcf, 0x25}},
    2};
static const PROPERTYKEY kPK_FileVersion = {
    {0x0cef7d53, 0xfa64, 0x11d1, {0xa2, 0x03, 0x00, 0x00, 0xf8, 0x1f, 0xed, 0xee}},
    4};

#ifndef FILE_ATTRIBUTE_PINNED
#define FILE_ATTRIBUTE_PINNED 0x00080000
#endif
#ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#endif

#define WM_APP_RELOAD (WM_APP + 1)
#define WM_APP_QUIT (WM_APP + 2)
#define WM_APP_SIZEDONE (WM_APP + 3)
#define TIMER_POLL 1
#define TIMER_FADE 2

static const wchar_t kMsgClass[] = L"WhFileHoverInfoMsg";
static const wchar_t kCardClass[] = L"WhFileHoverInfoCard";

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
enum Theme { ThemeAuto, ThemeDark, ThemeLight, ThemeCustom };
enum Background { BgSolid, BgAcrylic, BgBlur };
enum DateStyle { DateLong, DateShort, DateRelative, DateShortRelative };

struct Settings {
    int delay;
    bool onDesktop, inExplorer;
    int lang;
    int folderSizeLimit;
    int dateStyle;
    struct {
        bool type, size, folderSize, folderPreview, exactBytes, created, modified, accessed, attributes,
            dimensions, duration, version, target, contents, location;
    } f;
    int theme, background, opacity;
    std::wstring accentColor;
    bool accentBar, shadow, animation;
    int cornerRadius;
    std::wstring fontFamily;
    int fontSize, maxWidth;
    bool icon, thumbnail;
    int iconSize;
    std::wstring customBg, customText, customLabel;
} g_set;

struct Strings {
    const wchar_t *size, *created, *modified, *accessed, *attributes,
        *dimensions, *duration, *version, *target, *contents, *location;
    const wchar_t *justNow, *minAgo, *minsAgo, *hourAgo, *hoursAgo, *yesterday,
        *daysAgo, *monthAgo, *monthsAgo, *yearAgo, *yearsAgo;
    const wchar_t *file1, *files, *folder1, *folders, *empty, *itemsMore,
        *bytes;
    const wchar_t *aReadOnly, *aHidden, *aSystem, *aCompressed, *aEncrypted,
        *aLink, *aOnline, *aPinned;
    const wchar_t *calculating, *partial;
    const wchar_t *space, *freeOf;
    const wchar_t *foldersLbl, *filesLbl;
};

static const Strings kIt = {
    L"Dimensione", L"Creato", L"Modificato", L"Ultimo accesso", L"Attributi",
    L"Risoluzione", L"Durata", L"Versione", L"Destinazione", L"Contenuto",
    L"Percorso",
    L"adesso", L"1 minuto fa", L"%d minuti fa", L"1 ora fa", L"%d ore fa",
    L"ieri", L"%d giorni fa", L"1 mese fa", L"%d mesi fa", L"1 anno fa",
    L"%d anni fa",
    L"1 file", L"%ls file", L"1 cartella", L"%ls cartelle", L"Vuota",
    L"%ls+ elementi", L"byte",
    L"Sola lettura", L"Nascosto", L"Sistema", L"Compresso", L"Crittografato",
    L"Collegamento", L"Solo online", L"Sempre disponibile",
    L"Calcolo\u2026", L"parziale",
    L"Spazio", L"%ls liberi su %ls",
    L"Cartelle", L"File"};

static const Strings kEn = {
    L"Size", L"Created", L"Modified", L"Accessed", L"Attributes",
    L"Dimensions", L"Duration", L"Version", L"Target", L"Contents",
    L"Location",
    L"just now", L"1 minute ago", L"%d minutes ago", L"1 hour ago",
    L"%d hours ago", L"yesterday", L"%d days ago", L"1 month ago",
    L"%d months ago", L"1 year ago", L"%d years ago",
    L"1 file", L"%ls files", L"1 folder", L"%ls folders", L"Empty",
    L"%ls+ items", L"bytes",
    L"Read-only", L"Hidden", L"System", L"Compressed", L"Encrypted",
    L"Link", L"Online-only", L"Always available",
    L"Calculating\u2026", L"partial",
    L"Space", L"%ls free of %ls",
    L"Folders", L"Files"};

static const Strings kEs = {
    L"Tama\u00F1o", L"Creado", L"Modificado", L"\u00DAltimo acceso",
    L"Atributos", L"Dimensiones", L"Duraci\u00F3n", L"Versi\u00F3n",
    L"Destino", L"Contenido", L"Ubicaci\u00F3n",
    L"ahora mismo", L"hace 1 minuto", L"hace %d minutos", L"hace 1 hora",
    L"hace %d horas", L"ayer", L"hace %d d\u00EDas", L"hace 1 mes",
    L"hace %d meses", L"hace 1 a\u00F1o", L"hace %d a\u00F1os",
    L"1 archivo", L"%ls archivos", L"1 carpeta", L"%ls carpetas", L"Vac\u00EDa",
    L"%ls+ elementos", L"bytes",
    L"Solo lectura", L"Oculto", L"Sistema", L"Comprimido", L"Cifrado",
    L"V\u00EDnculo", L"Solo en l\u00EDnea", L"Siempre disponible",
    L"Calculando\u2026", L"parcial",
    L"Espacio", L"%ls libres de %ls",
    L"Carpetas", L"Archivos"};

static const Strings kFr = {
    L"Taille", L"Cr\u00E9\u00E9", L"Modifi\u00E9", L"Dernier acc\u00E8s",
    L"Attributs", L"Dimensions", L"Dur\u00E9e", L"Version", L"Cible",
    L"Contenu", L"Emplacement",
    L"\u00E0 l'instant", L"il y a 1 minute", L"il y a %d minutes",
    L"il y a 1 heure", L"il y a %d heures", L"hier", L"il y a %d jours",
    L"il y a 1 mois", L"il y a %d mois", L"il y a 1 an", L"il y a %d ans",
    L"1 fichier", L"%ls fichiers", L"1 dossier", L"%ls dossiers", L"Vide",
    L"%ls+ \u00E9l\u00E9ments", L"octets",
    L"Lecture seule", L"Masqu\u00E9", L"Syst\u00E8me", L"Compress\u00E9",
    L"Chiffr\u00E9", L"Lien", L"En ligne uniquement", L"Toujours disponible",
    L"Calcul\u2026", L"partiel",
    L"Espace", L"%ls libres sur %ls",
    L"Dossiers", L"Fichiers"};

static const Strings kDe = {
    L"Gr\u00F6\u00DFe", L"Erstellt", L"Ge\u00E4ndert", L"Letzter Zugriff",
    L"Attribute", L"Abmessungen", L"Dauer", L"Version", L"Ziel", L"Inhalt",
    L"Speicherort",
    L"gerade eben", L"vor 1 Minute", L"vor %d Minuten", L"vor 1 Stunde",
    L"vor %d Stunden", L"gestern", L"vor %d Tagen", L"vor 1 Monat",
    L"vor %d Monaten", L"vor 1 Jahr", L"vor %d Jahren",
    L"1 Datei", L"%ls Dateien", L"1 Ordner", L"%ls Ordner", L"Leer",
    L"%ls+ Elemente", L"Bytes",
    L"Schreibgesch\u00FCtzt", L"Versteckt", L"System", L"Komprimiert",
    L"Verschl\u00FCsselt", L"Verkn\u00FCpfung", L"Nur online",
    L"Immer verf\u00FCgbar",
    L"Wird berechnet\u2026", L"unvollst\u00E4ndig",
    L"Speicher", L"%ls frei von %ls",
    L"Ordner", L"Dateien"};

static const Strings kPt = {
    L"Tamanho", L"Criado", L"Modificado", L"\u00DAltimo acesso",
    L"Atributos", L"Dimens\u00F5es", L"Dura\u00E7\u00E3o", L"Vers\u00E3o",
    L"Destino", L"Conte\u00FAdo", L"Local",
    L"agora mesmo", L"h\u00E1 1 minuto", L"h\u00E1 %d minutos",
    L"h\u00E1 1 hora", L"h\u00E1 %d horas", L"ontem", L"h\u00E1 %d dias",
    L"h\u00E1 1 m\u00EAs", L"h\u00E1 %d meses", L"h\u00E1 1 ano",
    L"h\u00E1 %d anos",
    L"1 arquivo", L"%ls arquivos", L"1 pasta", L"%ls pastas", L"Vazia",
    L"%ls+ itens", L"bytes",
    L"Somente leitura", L"Oculto", L"Sistema", L"Compactado",
    L"Criptografado", L"Link", L"Somente online", L"Sempre dispon\u00EDvel",
    L"Calculando\u2026", L"parcial",
    L"Espa\u00E7o", L"%ls livres de %ls",
    L"Pastas", L"Arquivos"};

// Simplified Chinese by nekOStorm
static const Strings kChS = {
    L"大小", L"创建时间", L"修改时间", L"访问时间", L"属性",
    L"尺寸", L"时长", L"版本", L"目标", L"内容",
    L"位置",
    L"刚刚", L"1 分钟前", L"%d 分钟前", L"1 小时前",
    L"%d 小时前", L"昨天", L"%d 天前", L"1 个月前",
    L"%d 个月前", L"1 年前", L"%d 年前",
    L"1 个文件", L"%ls 个文件", L"1 个文件夹", L"%ls 个文件夹", L"空",
    L"%ls+ 个项目", L"字节",
    L"只读", L"隐藏", L"系统", L"已压缩", L"已加密",
    L"链接", L"仅联机", L"始终可用",
    L"正在计算\u2026", L"部分结果",
    L"磁盘空间", L"%ls 可用，共 %ls",
    L"文件夹", L"文件"};

static const Strings& S() {
    static const Strings* tables[] = {&kEn, &kIt, &kEs, &kFr, &kDe, &kPt, &kChS};
    return *tables[std::clamp(g_set.lang, 0, 6)];
}

static std::wstring GetStr(PCWSTR name) {
    return WindhawkUtils::StringSetting(Wh_GetStringSetting(name)).get();
}

static bool GetBool(PCWSTR name) { return Wh_GetIntSetting(name) != 0; }

static void LoadSettings() {
    g_set.delay = std::clamp(Wh_GetIntSetting(L"delay"), 0, 5000);
    g_set.onDesktop = GetBool(L"showOnDesktop");
    g_set.inExplorer = GetBool(L"showInExplorer");
    {
        static const wchar_t* codes[] = {L"en", L"it", L"es", L"fr", L"de", L"pt", L"zh"};
        std::wstring lang = GetStr(L"language");
        g_set.lang = -1;
        for (int i = 0; i < 7; i++)
            if (lang == codes[i]) g_set.lang = i;
        if (g_set.lang < 0) {  // auto: follow the Windows display language
            switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
                case LANG_ITALIAN: g_set.lang = 1; break;
                case LANG_SPANISH: g_set.lang = 2; break;
                case LANG_FRENCH: g_set.lang = 3; break;
                case LANG_GERMAN: g_set.lang = 4; break;
                case LANG_PORTUGUESE: g_set.lang = 5; break;
                case LANG_CHINESE: {
                    // LANG_CHINESE covers both scripts: use Simplified only
                    // for mainland China and Singapore. Traditional Chinese
                    // (Taiwan, Hong Kong, Macau) falls back to English.
                    WORD sub = SUBLANGID(GetUserDefaultUILanguage());
                    g_set.lang = (sub == SUBLANG_CHINESE_SIMPLIFIED ||
                                  sub == SUBLANG_CHINESE_SINGAPORE)
                                     ? 6
                                     : 0;
                    break;
                }
                default: g_set.lang = 0;
            }
        }
    }
    g_set.folderSizeLimit =
        std::clamp(Wh_GetIntSetting(L"folderSizeLimit"), 1, 120);

    std::wstring ds = GetStr(L"dateStyle");
    g_set.dateStyle = ds == L"long"       ? DateLong
                      : ds == L"short"    ? DateShort
                      : ds == L"relative" ? DateRelative
                                          : DateShortRelative;

    g_set.f.type = GetBool(L"fields.type");
    g_set.f.size = GetBool(L"fields.size");
    g_set.f.folderSize = GetBool(L"fields.folderSize");
    g_set.f.exactBytes = GetBool(L"fields.exactBytes");
    g_set.f.created = GetBool(L"fields.created");
    g_set.f.modified = GetBool(L"fields.modified");
    g_set.f.accessed = GetBool(L"fields.accessed");
    g_set.f.attributes = GetBool(L"fields.attributes");
    g_set.f.dimensions = GetBool(L"fields.dimensions");
    g_set.f.duration = GetBool(L"fields.duration");
    g_set.f.version = GetBool(L"fields.version");
    g_set.f.target = GetBool(L"fields.target");
    g_set.f.contents = GetBool(L"fields.contents");
    g_set.f.folderPreview = GetBool(L"fields.folderPreview");
    g_set.f.location = GetBool(L"fields.location");

    std::wstring th = GetStr(L"appearance.theme");
    g_set.theme = th == L"dark"     ? ThemeDark
                  : th == L"light"  ? ThemeLight
                  : th == L"custom" ? ThemeCustom
                                    : ThemeAuto;
    std::wstring bg = GetStr(L"appearance.background");
    g_set.background = bg == L"acrylic" ? BgAcrylic
                       : bg == L"blur"  ? BgBlur
                                        : BgSolid;
    g_set.opacity = std::clamp(Wh_GetIntSetting(L"appearance.opacity"), 0, 100);
    g_set.accentColor = GetStr(L"appearance.accentColor");
    g_set.accentBar = GetBool(L"appearance.accentBar");
    g_set.shadow = GetBool(L"appearance.shadow");
    g_set.animation = GetBool(L"appearance.animation");
    g_set.cornerRadius =
        std::clamp(Wh_GetIntSetting(L"appearance.cornerRadius"), 0, 24);
    g_set.fontFamily = GetStr(L"appearance.fontFamily");
    g_set.fontSize = std::clamp(Wh_GetIntSetting(L"appearance.fontSize"), 8, 28);
    g_set.maxWidth =
        std::clamp(Wh_GetIntSetting(L"appearance.maxWidth"), 200, 900);
    g_set.icon = GetBool(L"appearance.icon");
    g_set.thumbnail = GetBool(L"appearance.thumbnail");
    g_set.iconSize =
        std::clamp(Wh_GetIntSetting(L"appearance.iconSize"), 16, 256);
    g_set.customBg = GetStr(L"appearance.customBackground");
    g_set.customText = GetStr(L"appearance.customText");
    g_set.customLabel = GetStr(L"appearance.customLabel");
}

// ---------------------------------------------------------------------------
// Colors
// ---------------------------------------------------------------------------
struct Palette {
    D2D1_COLOR_F bg, text, label, border, accent;
    bool dark;
} g_pal;

static D2D1_COLOR_F Rgba(float r, float g, float b, float a = 1.f) {
    return D2D1_COLOR_F{r, g, b, a};
}

static bool ParseColor(const std::wstring& s, D2D1_COLOR_F* out) {
    std::wstring t;
    for (wchar_t c : s) {
        if (c != L'#' && c != L' ')
            t += c;
    }
    if (t.size() != 6 && t.size() != 8)
        return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(t.c_str(), &end, 16);
    if (!end || *end)
        return false;
    float a = t.size() == 8 ? ((v >> 24) & 0xff) / 255.f : 1.f;
    *out = Rgba(((v >> 16) & 0xff) / 255.f, ((v >> 8) & 0xff) / 255.f,
                (v & 0xff) / 255.f, a);
    return true;
}

static bool IsSystemDark() {
    DWORD v = 1, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &sz) != ERROR_SUCCESS)
        return false;
    return v == 0;
}

static D2D1_COLOR_F SystemAccent() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &v,
                     &sz) == ERROR_SUCCESS) {
        // ABGR
        return Rgba((v & 0xff) / 255.f, ((v >> 8) & 0xff) / 255.f,
                    ((v >> 16) & 0xff) / 255.f);
    }
    return Rgba(0x00 / 255.f, 0x78 / 255.f, 0xd4 / 255.f);
}

static void BuildPalette() {
    bool dark;
    if (g_set.theme == ThemeCustom) {
        D2D1_COLOR_F bg = Rgba(0.12f, 0.12f, 0.18f), tx = Rgba(0.8f, 0.84f, 0.96f),
                     lb = Rgba(0.58f, 0.6f, 0.7f);
        ParseColor(g_set.customBg, &bg);
        ParseColor(g_set.customText, &tx);
        ParseColor(g_set.customLabel, &lb);
        float lum = 0.2126f * bg.r + 0.7152f * bg.g + 0.0722f * bg.b;
        dark = lum < 0.5f;
        g_pal.bg = bg;
        g_pal.text = tx;
        g_pal.label = lb;
        g_pal.border = Rgba(tx.r, tx.g, tx.b, 0.14f);
    } else {
        dark = g_set.theme == ThemeDark ||
               (g_set.theme == ThemeAuto && IsSystemDark());
        if (dark) {
            g_pal.bg = Rgba(0.125f, 0.125f, 0.13f);
            g_pal.text = Rgba(1, 1, 1, 0.96f);
            g_pal.label = Rgba(1, 1, 1, 0.6f);
            g_pal.border = Rgba(1, 1, 1, 0.1f);
        } else {
            g_pal.bg = Rgba(0.976f, 0.976f, 0.98f);
            g_pal.text = Rgba(0, 0, 0, 0.9f);
            g_pal.label = Rgba(0, 0, 0, 0.58f);
            g_pal.border = Rgba(0, 0, 0, 0.1f);
        }
    }
    g_pal.bg.a *= g_set.opacity / 100.f;
    g_pal.dark = dark;
    if (!ParseColor(g_set.accentColor, &g_pal.accent))
        g_pal.accent = SystemAccent();
}

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static HINSTANCE g_hInst;
static HANDLE g_thread;
static std::atomic<HWND> g_msgWnd;
static HWND g_cardWnd;
static std::atomic<bool> g_quit;
static std::atomic<bool> g_hideNative{true};
static ID2D1Factory* g_d2d;
static IDWriteFactory* g_dw;
static ID2D1DCRenderTarget* g_rt;
static IShellWindows* g_shellWindows;

static POINT g_lastPt = {LONG_MIN, LONG_MIN};
static ULONGLONG g_stillSince;
static bool g_evaluated, g_visible;
static RECT g_itemRect;
static HWND g_cardDefView;
static int g_fade;
static std::wstring g_cardHitName;
static ULONGLONG g_lastVerify;
// COM calls made while resolving an item can pump messages, which would run
// the timer again in the middle of building the card. These guard against it.
static bool g_busy, g_reloadPending;

// ---------------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------------
static std::wstring Fmt(const wchar_t* f, long long n) {
    wchar_t buf[128];
    swprintf(buf, 128, f, (int)n);
    return buf;
}

static std::wstring FmtS(const wchar_t* f, const std::wstring& s) {
    wchar_t buf[128];
    swprintf(buf, 128, f, s.c_str());
    return buf;
}

static std::wstring FmtS2(const wchar_t* f, const std::wstring& a,
                          const std::wstring& b) {
    wchar_t buf[256];
    swprintf(buf, 256, f, a.c_str(), b.c_str());
    return buf;
}

static std::wstring GroupDigits(unsigned long long v) {
    wchar_t sep[8] = L".";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, sep, 8);
    std::wstring digits = std::to_wstring(v), out;
    int n = (int)digits.size();
    for (int i = 0; i < n; i++) {
        out += digits[i];
        int rem = n - i - 1;
        if (rem > 0 && rem % 3 == 0)
            out += sep;
    }
    return out;
}

static std::wstring StripBidi(const std::wstring& s) {
    std::wstring r;
    for (wchar_t c : s) {
        if (c == 0x200e || c == 0x200f || (c >= 0x202a && c <= 0x202e))
            continue;
        r += c;
    }
    return r;
}

static std::wstring RelativeTime(const FILETIME& ft) {
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a, b;
    a.LowPart = ft.dwLowDateTime;
    a.HighPart = ft.dwHighDateTime;
    b.LowPart = now.dwLowDateTime;
    b.HighPart = now.dwHighDateTime;
    if (b.QuadPart < a.QuadPart)
        return L"";
    long long sec = (long long)((b.QuadPart - a.QuadPart) / 10000000ULL);
    const Strings& s = S();
    if (sec < 60)
        return s.justNow;
    if (sec < 3600)
        return sec < 120 ? s.minAgo : Fmt(s.minsAgo, sec / 60);
    if (sec < 86400)
        return sec < 7200 ? s.hourAgo : Fmt(s.hoursAgo, sec / 3600);
    long long days = sec / 86400;
    if (days < 2)
        return s.yesterday;
    if (days < 30)
        return Fmt(s.daysAgo, days);
    if (days < 365)
        return days < 60 ? s.monthAgo : Fmt(s.monthsAgo, days / 30);
    return days < 730 ? s.yearAgo : Fmt(s.yearsAgo, days / 365);
}

static std::wstring FormatDate(const FILETIME& ft) {
    SYSTEMTIME utc, loc;
    if (!FileTimeToSystemTime(&ft, &utc) ||
        !SystemTimeToTzSpecificLocalTime(nullptr, &utc, &loc))
        return L"";
    std::wstring abs;
    if (g_set.dateStyle != DateRelative) {
        wchar_t d[128] = L"", t[64] = L"";
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT,
                        g_set.dateStyle == DateLong ? DATE_LONGDATE
                                                    : DATE_SHORTDATE,
                        &loc, nullptr, d, 128, nullptr);
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &loc, nullptr,
                        t, 64);
        abs = std::wstring(d) + L"  " + t;
    }
    if (g_set.dateStyle == DateLong || g_set.dateStyle == DateShort)
        return abs;
    std::wstring rel = RelativeTime(ft);
    if (g_set.dateStyle == DateRelative)
        return rel.empty() ? abs : rel;
    return rel.empty() ? abs : abs + L"  \u00B7  " + rel;
}

static std::wstring FormatDuration(unsigned long long hns) {
    unsigned long long t = hns / 10000000ULL;
    wchar_t buf[64];
    if (t >= 3600)
        swprintf(buf, 64, L"%d:%02d:%02d", (int)(t / 3600), (int)(t / 60 % 60),
                 (int)(t % 60));
    else
        swprintf(buf, 64, L"%d:%02d", (int)(t / 60), (int)(t % 60));
    return buf;
}

static std::wstring FormatAttributes(DWORD a) {
    const Strings& s = S();
    // On folders, read-only and system just mark a customized folder (custom
    // icon, desktop.ini); they don't protect anything, so don't show them.
    if (a & FILE_ATTRIBUTE_DIRECTORY)
        a &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
    std::vector<const wchar_t*> parts;
    if (a & FILE_ATTRIBUTE_READONLY) parts.push_back(s.aReadOnly);
    if (a & FILE_ATTRIBUTE_HIDDEN) parts.push_back(s.aHidden);
    if (a & FILE_ATTRIBUTE_SYSTEM) parts.push_back(s.aSystem);
    if (a & FILE_ATTRIBUTE_COMPRESSED) parts.push_back(s.aCompressed);
    if (a & FILE_ATTRIBUTE_ENCRYPTED) parts.push_back(s.aEncrypted);
    if (a & FILE_ATTRIBUTE_REPARSE_POINT &&
        !(a & (FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS | FILE_ATTRIBUTE_PINNED |
               FILE_ATTRIBUTE_OFFLINE)))
        parts.push_back(s.aLink);
    if (a & (FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS | FILE_ATTRIBUTE_OFFLINE))
        parts.push_back(s.aOnline);
    else if (a & FILE_ATTRIBUTE_PINNED)
        parts.push_back(s.aPinned);
    std::wstring r;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i) r += L", ";
        r += parts[i];
    }
    return r;
}

struct FolderInfo {
    bool ok = false, truncated = false;
    unsigned long long files = 0, dirs = 0;
    std::vector<std::wstring> fileNames, dirNames;
};

static const size_t kPreviewNames = 6;
static const size_t kMaxCollectedNames = 1000;

// One-level scan of a folder: counts and the first names, counting only what
// Explorer shows (respects "show hidden files" and "hide protected operating
// system files").
static FolderInfo ScanFolder(const std::wstring& dir) {
    FolderInfo info;
    std::wstring pattern = dir;
    if (!pattern.empty() && pattern.back() != L'\\')
        pattern += L'\\';
    pattern += L'*';
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                FindExSearchNameMatch, nullptr,
                                FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE)
        return info;
    info.ok = true;
    SHELLSTATE ss = {};
    SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN, FALSE);
    ULONGLONG start = GetTickCount64();
    do {
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L".."))
            continue;
        DWORD a = fd.dwFileAttributes;
        if ((a & FILE_ATTRIBUTE_HIDDEN) && (a & FILE_ATTRIBUTE_SYSTEM)) {
            if (!ss.fShowSuperHidden)
                continue;
        } else if ((a & FILE_ATTRIBUTE_HIDDEN) && !ss.fShowAllObjects) {
            continue;
        }
        if (a & FILE_ATTRIBUTE_DIRECTORY) {
            info.dirs++;
            if (info.dirNames.size() < kMaxCollectedNames)
                info.dirNames.push_back(fd.cFileName);
        } else {
            info.files++;
            if (info.fileNames.size() < kMaxCollectedNames)
                info.fileNames.push_back(fd.cFileName);
        }
        if (info.files + info.dirs >= 20000 || GetTickCount64() - start > 300) {
            info.truncated = true;
            break;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return info;
}

static std::wstring FormatCounts(const FolderInfo& info) {
    const Strings& s = S();
    if (!info.ok)
        return L"";
    if (info.truncated)
        return FmtS(s.itemsMore, GroupDigits(info.files + info.dirs));
    if (!info.files && !info.dirs)
        return s.empty;
    std::wstring r;
    if (info.files)
        r = info.files == 1 ? s.file1 : FmtS(s.files, GroupDigits(info.files));
    if (info.dirs) {
        if (!r.empty()) r += L", ";
        r += info.dirs == 1 ? s.folder1
                            : FmtS(s.folders, GroupDigits(info.dirs));
    }
    return r;
}

// First names in Explorer's order (natural sort: "2" before "10"), with an
// ellipsis when there are more.
static std::wstring PreviewNames(std::vector<std::wstring> names,
                                 unsigned long long total) {
    if (names.empty())
        return L"";
    size_t n = std::min(kPreviewNames, names.size());
    std::partial_sort(names.begin(), names.begin() + n, names.end(),
                      [](const std::wstring& a, const std::wstring& b) {
                          return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
                      });
    std::wstring r;
    for (size_t i = 0; i < n; i++) {
        if (i) r += L", ";
        r += names[i];
    }
    if (total > n)
        r += L", \u2026";
    return r;
}

// ---------------------------------------------------------------------------
// Folder size, summed on a background thread
// ---------------------------------------------------------------------------
struct SizeJob {
    std::wstring path;
    LONG gen;
    ULONGLONG deadline;
};
struct SizeResult {
    LONG gen;
    ULONGLONG bytes;
    bool partial;
};
static std::atomic<LONG> g_sizeGen;
// Handles of the folder-size threads. Only the worker thread touches this.
static std::vector<HANDLE> g_sizeThreads;

static bool SizeCancelled(LONG gen) {
    return g_sizeGen.load() != gen;
}

static DWORD WINAPI SizeThread(LPVOID param) {
    SizeJob* job = (SizeJob*)param;
    ULONGLONG total = 0;
    bool partial = false;
    // Extended-length prefix so paths longer than MAX_PATH are counted too.
    std::wstring root = job->path;
    if (root.rfind(L"\\\\?\\", 0) != 0) {
        if (root.rfind(L"\\\\", 0) == 0)
            root = L"\\\\?\\UNC\\" + root.substr(2);
        else
            root = L"\\\\?\\" + root;
    }
    std::vector<std::wstring> stack{root};
    while (!stack.empty() && !partial) {
        if (SizeCancelled(job->gen) || GetTickCount64() > job->deadline) {
            partial = true;
            break;
        }
        std::wstring dir = std::move(stack.back());
        stack.pop_back();
        std::wstring pattern = dir;
        if (pattern.empty() || pattern.back() != L'\\')
            pattern += L'\\';
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW((pattern + L"*").c_str(), FindExInfoBasic,
                                    &fd, FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
        if (h == INVALID_HANDLE_VALUE)
            continue;
        do {
            if (SizeCancelled(job->gen) || GetTickCount64() > job->deadline) {
                partial = true;
                break;
            }
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L".."))
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Don't follow junctions/symlinks (avoids loops and
                // double counting).
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                    stack.push_back(pattern + fd.cFileName);
            } else {
                total += ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (!SizeCancelled(job->gen)) {
        SizeResult* r = new SizeResult{job->gen, total, partial};
        HWND w = g_msgWnd;
        if (!w || !PostMessageW(w, WM_APP_SIZEDONE, 0, (LPARAM)r))
            delete r;
    }
    delete job;
    return 0;
}

// Closes the handles of size threads that have finished.
static void PruneSizeThreads() {
    for (auto it = g_sizeThreads.begin(); it != g_sizeThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_sizeThreads.erase(it);
        } else {
            ++it;
        }
    }
}

// Tells running size threads to stop. With `join`, waits until all of them
// have exited, interrupting any blocking file-system call (e.g. a slow
// network share) with CancelSynchronousIo.
static void CancelFolderSize(bool join = false) {
    g_sizeGen++;
    for (HANDLE h : g_sizeThreads)
        CancelSynchronousIo(h);
    if (!join) {
        PruneSizeThreads();
        return;
    }
    while (!g_sizeThreads.empty()) {
        for (HANDLE h : g_sizeThreads)
            CancelSynchronousIo(h);
        WaitForSingleObject(g_sizeThreads.front(), 100);
        PruneSizeThreads();
    }
}

static void StartFolderSize(const std::wstring& path) {
    CancelFolderSize();
    SizeJob* job = new SizeJob{
        path, g_sizeGen.load(),
        GetTickCount64() + (ULONGLONG)g_set.folderSizeLimit * 1000};
    HANDLE h = CreateThread(nullptr, 0, SizeThread, job, 0, nullptr);
    if (h)
        g_sizeThreads.push_back(h);
    else
        delete job;
}

static std::wstring FormatBytes(ULONGLONG size) {
    wchar_t buf[64] = L"";
    StrFormatByteSizeEx(size, SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT, buf,
                        64);
    std::wstring v = buf;
    if (g_set.f.exactBytes && size >= 1024)
        v += L"  (" + GroupDigits(size) + L" " + S().bytes + L")";
    return v;
}

// ---------------------------------------------------------------------------
// Card model and layout
// ---------------------------------------------------------------------------
struct Row {
    std::wstring label, value;
    CPtr<IDWriteTextLayout> lab, val;
    float labBaseline = 0, valBaseline = 0, h = 0;
};

struct Card {
    std::wstring name, type;
    std::vector<Row> rows;
    CPtr<IDWriteTextLayout> nameL, typeL;
    CPtr<ID2D1Bitmap> icon;
    float iconW = 0, iconH = 0;
    float s = 1, pad = 12, gap = 10, iconBox = 0, headerH = 0, nameH = 0,
          typeH = 0, labelColW = 0, rowGap = 5, sepPad = 10, cardW = 0,
          cardH = 0, margin = 0;
    int sizeRow = -1;
    std::wstring sizePath;  // folder to sum once the card is on screen
};
// Holds DWrite/D2D objects, which must not be released by the automatic
// destructor at process exit. Created and reset on the worker thread.
[[clang::no_destroy]] static std::optional<Card> g_card;

static float FirstBaseline(IDWriteTextLayout* l) {
    UINT32 count = 0;
    l->GetLineMetrics(nullptr, 0, &count);
    if (!count)
        return 0;
    std::vector<DWRITE_LINE_METRICS> lm(count);
    if (FAILED(l->GetLineMetrics(lm.data(), count, &count)))
        return 0;
    return lm[0].baseline;
}

static std::wstring ResolveFontFamily() {
    CPtr<IDWriteFontCollection> fc;
    if (FAILED(g_dw->GetSystemFontCollection(&fc, FALSE)))
        return L"Segoe UI";
    for (const std::wstring& f :
         {g_set.fontFamily, std::wstring(L"Segoe UI Variable Text"),
          std::wstring(L"Segoe UI")}) {
        UINT32 idx;
        BOOL exists = FALSE;
        if (!f.empty() && SUCCEEDED(fc->FindFamilyName(f.c_str(), &idx, &exists)) &&
            exists)
            return f;
    }
    return L"Segoe UI";
}

static CPtr<IDWriteTextFormat> MakeFormat(const std::wstring& family,
                                          DWRITE_FONT_WEIGHT w, float size,
                                          bool wrap) {
    CPtr<IDWriteTextFormat> f;
    if (SUCCEEDED(g_dw->CreateTextFormat(family.c_str(), nullptr, w,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL, size, L"",
                                         &f))) {
        f->SetWordWrapping(wrap ? (DWRITE_WORD_WRAPPING)2 /*EMERGENCY_BREAK*/
                                : DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    return f;
}

static CPtr<IDWriteTextLayout> MakeLayout(const std::wstring& text,
                                          IDWriteTextFormat* fmt,
                                          float maxW,
                                          DWRITE_TEXT_METRICS* m) {
    CPtr<IDWriteTextLayout> l;
    if (!fmt || FAILED(g_dw->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                              fmt, maxW, 10000.f, &l))) {
        *m = {};
        return l;
    }
    if (FAILED(l->GetMetrics(m)))
        *m = {};
    return l;
}

static void LoadIconBitmap(IShellItem* item, int px) {
    CPtr<IShellItemImageFactory> f;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&f))))
        return;
    HBITMAP hbmp = nullptr;
    SIZE sz = {px, px};
    HRESULT hr = E_FAIL;
    if (g_set.thumbnail)
        hr = f->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_BIGGERSIZEOK, &hbmp);
    if (FAILED(hr))
        hr = f->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp);
    if (FAILED(hr) || !hbmp)
        return;

    BITMAP bm = {};
    GetObjectW(hbmp, sizeof(bm), &bm);
    int w = bm.bmWidth, h = bm.bmHeight;
    if (w > 0 && h > 0) {
        std::vector<BYTE> bits((size_t)w * h * 4);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int got = GetDIBits(dc, hbmp, 0, h, bits.data(), &bi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (got) {
            bool anyAlpha = false;
            for (size_t i = 3; i < bits.size(); i += 4) {
                if (bits[i]) {
                    anyAlpha = true;
                    break;
                }
            }
            if (!anyAlpha) {
                for (size_t i = 3; i < bits.size(); i += 4)
                    bits[i] = 255;
            }
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            if (SUCCEEDED(g_rt->CreateBitmap(D2D1::SizeU(w, h), bits.data(),
                                             w * 4, props, &g_card->icon))) {
                g_card->iconW = (float)w;
                g_card->iconH = (float)h;
            }
        }
    }
    DeleteObject(hbmp);
}

static void AddRow(const wchar_t* label, const std::wstring& value) {
    if (value.empty())
        return;
    Row r;
    r.label = label;
    r.value = value;
    g_card->rows.push_back(std::move(r));
}

static void LayoutCard();

static bool BuildCard(IShellItem2* item, UINT dpi) {
    *g_card = Card();
    Card& c = *g_card;
    const Strings& s = S();

    PWSTR raw = nullptr;
    std::wstring path;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) && raw) {
        path = raw;
        CoTaskMemFree(raw);
    }
    raw = nullptr;
    if (!path.empty() && !PathIsRootW(path.c_str())) {
        c.name = PathFindFileNameW(path.c_str());
    } else if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &raw)) &&
               raw) {
        c.name = raw;
        CoTaskMemFree(raw);
    }
    if (c.name.empty())
        return false;

    if (g_set.f.type && SUCCEEDED(item->GetString(kPK_ItemTypeText, &raw)) &&
        raw) {
        c.type = raw;
        CoTaskMemFree(raw);
    }

    // Read the dates first: scanning the folder below counts as an access and
    // would otherwise make "Last accessed" always say "just now".
    std::vector<Row> dateRows;
    {
        FILETIME ft;
        const PROPERTYKEY* keys[] = {&kPK_DateCreated, &kPK_DateModified,
                                     &kPK_DateAccessed};
        const wchar_t* labels[] = {s.created, s.modified, s.accessed};
        bool on[] = {g_set.f.created, g_set.f.modified, g_set.f.accessed};
        for (int i = 0; i < 3; i++) {
            if (!on[i] || FAILED(item->GetFileTime(*keys[i], &ft)))
                continue;
            std::wstring v = FormatDate(ft);
            if (v.empty())
                continue;
            Row r;
            r.label = labels[i];
            r.value = v;
            dateRows.push_back(std::move(r));
        }
    }

    DWORD attrs = path.empty() ? INVALID_FILE_ATTRIBUTES
                               : GetFileAttributesW(path.c_str());
    bool isDir = attrs != INVALID_FILE_ATTRIBUTES &&
                 (attrs & FILE_ATTRIBUTE_DIRECTORY);

    if (g_set.f.size && !isDir) {
        ULONGLONG size = 0;
        if (SUCCEEDED(item->GetUInt64(kPK_Size, &size)))
            AddRow(s.size, FormatBytes(size));
    }

    bool isRoot = !path.empty() && PathIsRootW(path.c_str());
    if (g_set.f.size && isRoot) {
        // Drives: free space instead of scanning the whole volume.
        ULARGE_INTEGER avail, total, freeAll;
        if (GetDiskFreeSpaceExW(path.c_str(), &avail, &total, &freeAll))
            AddRow(s.space, FmtS2(s.freeOf, FormatBytes(avail.QuadPart),
                                  FormatBytes(total.QuadPart)));
    } else if (g_set.f.folderSize && isDir) {
        AddRow(s.size, s.calculating);
        c.sizeRow = (int)c.rows.size() - 1;
        c.sizePath = path;
    }

    if (isDir && (g_set.f.contents || g_set.f.folderPreview)) {
        FolderInfo info = ScanFolder(path);
        if (g_set.f.contents)
            AddRow(s.contents, FormatCounts(info));
        if (g_set.f.folderPreview && !info.truncated) {
            AddRow(s.foldersLbl, PreviewNames(info.dirNames, info.dirs));
            AddRow(s.filesLbl, PreviewNames(info.fileNames, info.files));
        }
    }

    if (g_set.f.dimensions && SUCCEEDED(item->GetString(kPK_ImageDimensions, &raw)) &&
        raw) {
        AddRow(s.dimensions, StripBidi(raw));
        CoTaskMemFree(raw);
    }

    if (g_set.f.duration) {
        ULONGLONG d = 0;
        if (SUCCEEDED(item->GetUInt64(kPK_MediaDuration, &d)) && d)
            AddRow(s.duration, FormatDuration(d));
    }

    if (g_set.f.version && SUCCEEDED(item->GetString(kPK_FileVersion, &raw)) &&
        raw) {
        AddRow(s.version, raw);
        CoTaskMemFree(raw);
    }

    if (g_set.f.target && SUCCEEDED(item->GetString(kPK_LinkTarget, &raw)) &&
        raw) {
        AddRow(s.target, raw);
        CoTaskMemFree(raw);
    }

    if (!dateRows.empty())
        for (Row& r : dateRows)
            c.rows.push_back(std::move(r));

    if (g_set.f.attributes && attrs != INVALID_FILE_ATTRIBUTES)
        AddRow(s.attributes, FormatAttributes(attrs));

    if (g_set.f.location && !path.empty()) {
        std::vector<wchar_t> buf(path.begin(), path.end());
        buf.push_back(0);
        if (PathRemoveFileSpecW(buf.data()) && buf[0])
            AddRow(s.location, buf.data());
    }

    float sc = dpi / 96.f;
    c.s = sc;
    c.pad = std::round(14 * sc);
    c.gap = std::round(12 * sc);
    c.rowGap = std::round(5 * sc);
    c.sepPad = std::round(10 * sc);
    c.margin = (g_set.background == BgSolid && g_set.shadow)
                   ? std::round(14 * sc)
                   : 0;

    if (g_set.icon) {
        int px = (int)std::round(g_set.iconSize * sc);
        LoadIconBitmap(item, px);
        if (g_card->icon)
            c.iconBox = (float)px;
    }

    LayoutCard();
    return true;
}

static void LayoutCard() {
    Card& c = *g_card;
    float sc = c.s;
    float fs = g_set.fontSize * sc;
    c.labelColW = 0;
    c.typeH = 0;
    c.typeL.Reset();

    std::wstring family = ResolveFontFamily();
    auto nameF = MakeFormat(family, DWRITE_FONT_WEIGHT_SEMI_BOLD, fs * 1.1f, true);
    auto typeF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs * 0.9f, true);
    auto labF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs * 0.9f, false);
    auto valF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs, true);

    float maxW = g_set.maxWidth * sc;
    float inner = maxW - 2 * c.pad;
    float headTextMax =
        std::max(inner - (c.iconBox ? c.iconBox + c.gap : 0), 80 * sc);

    DWRITE_TEXT_METRICS m;
    c.nameL = MakeLayout(c.name, nameF, headTextMax, &m);
    float headTextW = m.width;
    c.nameH = m.height;
    if (!c.type.empty()) {
        c.typeL = MakeLayout(c.type, typeF, headTextMax, &m);
        headTextW = std::max(headTextW, m.width);
        c.typeH = m.height;
    }
    float textBlock = c.nameH + (c.typeL ? std::round(2 * sc) + c.typeH : 0);
    c.headerH = std::max(c.iconBox, textBlock);
    float contentW = (c.iconBox ? c.iconBox + c.gap : 0) + headTextW;

    for (Row& r : c.rows) {
        r.lab = MakeLayout(r.label, labF, inner / 2, &m);
        c.labelColW = std::max(c.labelColW, m.width);
        r.h = m.height;
    }
    float valMax = std::max(inner - c.labelColW - c.gap, 60 * sc);
    float rowsH = 0, maxValW = 0;
    for (Row& r : c.rows) {
        r.val = MakeLayout(r.value, valF, valMax, &m);
        maxValW = std::max(maxValW, m.width);
        if (r.lab) r.labBaseline = FirstBaseline(r.lab);
        if (r.val) r.valBaseline = FirstBaseline(r.val);
        float labTop = std::max(0.f, r.valBaseline - r.labBaseline);
        r.h = std::max(m.height, labTop + r.h);
        rowsH += r.h;
    }
    if (!c.rows.empty()) {
        rowsH += c.rowGap * (c.rows.size() - 1);
        contentW = std::max(contentW, c.labelColW + c.gap + maxValW);
    }

    c.cardW = std::ceil(std::max(contentW + 2 * c.pad, 180 * sc));
    c.cardH = std::ceil(c.pad + c.headerH +
                        (c.rows.empty() ? 0 : 2 * c.sepPad + 1 + rowsH) + c.pad);
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
static void FillRR(ID2D1RenderTarget* rt, D2D1_RECT_F r, float rad,
                   D2D1_COLOR_F col) {
    CPtr<ID2D1SolidColorBrush> b;
    if (FAILED(rt->CreateSolidColorBrush(col, &b)))
        return;
    rt->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), b);
}

static void DrawCard(ID2D1RenderTarget* rt, bool solid) {
    Card& c = *g_card;
    float s = c.s, ox = c.margin, oy = c.margin;
    D2D1_RECT_F card = {ox, oy, ox + c.cardW, oy + c.cardH};
    float rad = solid ? g_set.cornerRadius * s : 0;

    rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    rt->Clear(Rgba(0, 0, 0, 0));

    if (solid) {
        if (g_set.shadow) {
            for (int i = 7; i >= 1; i--) {
                float e = i * 1.6f * s;
                D2D1_RECT_F r = {card.left - e, card.top - e + 3 * s,
                                 card.right + e, card.bottom + e + 3 * s};
                FillRR(rt, r, rad + e, Rgba(0, 0, 0, g_pal.dark ? 0.05f : 0.022f));
            }
        }
        FillRR(rt, card, rad, g_pal.bg);
        CPtr<ID2D1SolidColorBrush> b;
        if (SUCCEEDED(rt->CreateSolidColorBrush(g_pal.border, &b))) {
            D2D1_RECT_F r = {card.left + 0.5f, card.top + 0.5f,
                             card.right - 0.5f, card.bottom - 0.5f};
            rt->DrawRoundedRectangle(D2D1::RoundedRect(r, rad, rad), b, 1.f);
        }
    } else if (g_set.background == BgBlur) {
        FillRR(rt, card, 0, Rgba(g_pal.bg.r, g_pal.bg.g, g_pal.bg.b,
                                 g_pal.bg.a * 0.55f));
    }

    if (g_set.accentBar) {
        float x = ox + std::round(c.pad * 0.4f);
        D2D1_RECT_F r = {x, oy + c.pad, x + 3 * s, oy + c.cardH - c.pad};
        FillRR(rt, r, 1.5f * s, g_pal.accent);
    }

    CPtr<ID2D1SolidColorBrush> textB, labB, sepB;
    rt->CreateSolidColorBrush(g_pal.text, &textB);
    rt->CreateSolidColorBrush(g_pal.label, &labB);
    D2D1_COLOR_F sepC = g_pal.accent;
    sepC.a = 0.45f;
    rt->CreateSolidColorBrush(sepC, &sepB);
    if (!textB || !labB || !sepB)
        return;

    float x = ox + c.pad, y = oy + c.pad;

    // Header
    if (c.icon && c.iconBox) {
        float k = std::min(c.iconBox / c.iconW, c.iconBox / c.iconH);
        float w = c.iconW * k, h = c.iconH * k;
        float ix = x + (c.iconBox - w) / 2;
        float iy = y + (c.headerH - h) / 2;
        rt->DrawBitmap(c.icon, D2D1::RectF(ix, iy, ix + w, iy + h), 1.f,
                       D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    float tx = x + (c.iconBox ? c.iconBox + c.gap : 0);
    float textBlock = c.nameH + (c.typeL ? std::round(2 * s) + c.typeH : 0);
    float ty = y + (c.headerH - textBlock) / 2;
    if (c.nameL)
        rt->DrawTextLayout(D2D1::Point2F(tx, ty), c.nameL, textB);
    if (c.typeL)
        rt->DrawTextLayout(
            D2D1::Point2F(tx, ty + c.nameH + std::round(2 * s)), c.typeL, labB);
    y += c.headerH;

    if (c.rows.empty())
        return;

    // Separator
    y += c.sepPad;
    rt->DrawLine(D2D1::Point2F(x, y + 0.5f),
                 D2D1::Point2F(ox + c.cardW - c.pad, y + 0.5f), sepB, 1.f);
    y += 1 + c.sepPad;

    // Rows
    float vx = x + c.labelColW + c.gap;
    for (Row& r : c.rows) {
        float labTop = std::max(0.f, r.valBaseline - r.labBaseline);
        if (r.lab)
            rt->DrawTextLayout(D2D1::Point2F(x, y + labTop), r.lab, labB);
        if (r.val)
            rt->DrawTextLayout(D2D1::Point2F(vx, y), r.val, textB);
        y += r.h + c.rowGap;
    }
}

static void RecreateRenderTarget() {
    if (g_card)
        g_card->icon.Reset();  // resources belong to the old render target
    if (g_rt) {
        g_rt->Release();
        g_rt = nullptr;
    }
    D2D1_RENDER_TARGET_PROPERTIES p = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.f, 96.f);
    g_d2d->CreateDCRenderTarget(&p, &g_rt);
}

// ---------------------------------------------------------------------------
// Card window
// ---------------------------------------------------------------------------
struct ACCENTPOLICY {
    int accentState;
    int flags;
    DWORD gradientColor;
    int animationId;
};
struct WINCOMPATTRDATA {
    int attribute;
    PVOID data;
    SIZE_T size;
};
using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINCOMPATTRDATA*);

static void ApplyBackdrop() {
    if (!g_cardWnd || g_set.background == BgSolid)
        return;
    auto fn = (SetWindowCompositionAttribute_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!fn)
        return;
    BYTE a = (BYTE)std::clamp((int)(g_pal.bg.a * 255 * 0.85f), 1, 255);
    BYTE r = (BYTE)(g_pal.bg.r * 255), g = (BYTE)(g_pal.bg.g * 255),
         b = (BYTE)(g_pal.bg.b * 255);
    ACCENTPOLICY ap = {g_set.background == BgAcrylic ? 4 : 3, 2,
                       (DWORD)a << 24 | (DWORD)b << 16 | (DWORD)g << 8 | r, 0};
    WINCOMPATTRDATA d = {19 /*WCA_ACCENT_POLICY*/, &ap, sizeof(ap)};
    fn(g_cardWnd, &d);
    BOOL dark = g_pal.dark;
    DwmSetWindowAttribute(g_cardWnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark,
                          sizeof(dark));
}

static LRESULT CALLBACK CardWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            if (g_rt && g_set.background != BgSolid && g_visible) {
                RECT rc;
                GetClientRect(h, &rc);
                g_rt->BindDC(dc, &rc);
                g_rt->BeginDraw();
                DrawCard(g_rt, false);
                if (g_rt->EndDraw() == D2DERR_RECREATE_TARGET)
                    RecreateRenderTarget();
            }
            EndPaint(h, &ps);
            return 0;
        }
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static void CreateCardWindow() {
    bool solid = g_set.background == BgSolid;
    DWORD ex = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
               (solid ? (WS_EX_LAYERED | WS_EX_TRANSPARENT) : 0);
    g_cardWnd = CreateWindowExW(ex, kCardClass, L"", WS_POPUP, 0, 0, 1, 1,
                                nullptr, nullptr, g_hInst, nullptr);
    if (!g_cardWnd)
        return;
    DWORD corner = solid ? 1 /*DONOTROUND*/ : 2 /*ROUND*/;
    DwmSetWindowAttribute(g_cardWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/,
                          &corner, sizeof(corner));
}

static void DestroyCardWindow() {
    if (g_cardWnd) {
        DestroyWindow(g_cardWnd);
        g_cardWnd = nullptr;
    }
}

static void HideCard() {
    CancelFolderSize();
    KillTimer(g_msgWnd, TIMER_FADE);
    if (g_cardWnd && g_visible)
        ShowWindow(g_cardWnd, SW_HIDE);
    g_visible = false;
    if (g_card)
        g_card->icon.Reset();
}

static void PresentLayered(int x, int y, int w, int h, BYTE alpha) {
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib) {
        HGDIOBJ old = SelectObject(mem, dib);
        RECT rc = {0, 0, w, h};
        g_rt->BindDC(mem, &rc);
        g_rt->BeginDraw();
        DrawCard(g_rt, true);
        HRESULT hr = g_rt->EndDraw();
        POINT dst = {x, y}, src = {0, 0};
        SIZE sz = {w, h};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_cardWnd, screen, &dst, &sz, mem, &src, 0, &bf,
                            ULW_ALPHA);
        SelectObject(mem, old);
        DeleteObject(dib);
        if (hr == D2DERR_RECREATE_TARGET)
            RecreateRenderTarget();
    }
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

static POINT g_cardPt;

static void ShowCardAt(POINT pt, bool update = false) {
    g_cardPt = pt;
    if (!g_cardWnd || !g_rt)
        return;
    Card& c = *g_card;
    int w = (int)(c.cardW + 2 * c.margin), h = (int)(c.cardH + 2 * c.margin);

    // Card goes below-right of the cursor, flipped if it doesn't fit.
    int cx = pt.x + (int)(12 * c.s), cy = pt.y + (int)(22 * c.s);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    RECT wa = mi.rcWork;
    if (cx + (int)c.cardW > wa.right)
        cx = std::max((int)wa.left, (int)(wa.right - c.cardW));
    if (cy + (int)c.cardH > wa.bottom)
        cy = pt.y - (int)c.cardH - (int)(10 * c.s);
    cy = std::max(cy, (int)wa.top);
    int x = cx - (int)c.margin, y = cy - (int)c.margin;

    g_visible = true;
    if (g_set.background == BgSolid && update) {
        PresentLayered(x, y, w, h, (BYTE)g_fade);
    } else if (g_set.background == BgSolid) {
        bool anim = g_set.animation;
        g_fade = anim ? 0 : 255;
        PresentLayered(x, y, w, h, (BYTE)g_fade);
        SetWindowPos(g_cardWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        if (anim)
            SetTimer(g_msgWnd, TIMER_FADE, 15, nullptr);
    } else {
        ApplyBackdrop();
        SetWindowPos(g_cardWnd, HWND_TOPMOST, x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        RedrawWindow(g_cardWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

static void FadeStep() {
    g_fade = std::min(255, g_fade + 48);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)g_fade, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_cardWnd, nullptr, nullptr, nullptr, nullptr, nullptr,
                        0, &bf, ULW_ALPHA);
    if (g_fade >= 255)
        KillTimer(g_msgWnd, TIMER_FADE);
}

// ---------------------------------------------------------------------------
// Finding the hovered item
// ---------------------------------------------------------------------------
static HWND AncestorWithClass(HWND h, PCWSTR cls) {
    wchar_t buf[64];
    HWND desktop = GetDesktopWindow();
    for (; h && h != desktop; h = GetAncestor(h, GA_PARENT)) {
        if (GetClassNameW(h, buf, 64) && !wcscmp(buf, cls))
            return h;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Muting Windows' own infotips. The popup is a tooltip control owned by the
// item view (the desktop list view, or the DirectUI view in File Explorer).
// We deactivate those tooltips while the option is on, and turn them back on
// when the option or the mod is turned off.
// ---------------------------------------------------------------------------
static std::vector<HWND> g_mutedTips, g_mutedLists;
static HWND g_lastTipCheck;

struct TipEnum {
    HWND view;
    std::vector<HWND>* out;
};

static BOOL CALLBACK EnumTipsProc(HWND w, LPARAM lp) {
    TipEnum* e = (TipEnum*)lp;
    wchar_t cls[64];
    if (GetClassNameW(w, cls, 64) && !wcscmp(cls, L"tooltips_class32")) {
        HWND owner = GetWindow(w, GW_OWNER);
        if (owner && (owner == e->view || IsChild(e->view, owner)))
            e->out->push_back(w);
    }
    return TRUE;
}

// True if `w` is still a window of this process with the given class. HWND
// values can be reused by unrelated windows after the original is destroyed.
static bool IsOurWindowOfClass(HWND w, PCWSTR cls) {
    if (!IsWindow(w))
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    wchar_t buf[64] = L"";
    return pid == GetCurrentProcessId() && GetClassNameW(w, buf, 64) &&
           !wcscmp(buf, cls);
}

static void PruneDeadHandles(std::vector<HWND>& v, PCWSTR cls) {
    v.erase(std::remove_if(v.begin(), v.end(),
                           [cls](HWND w) { return !IsOurWindowOfClass(w, cls); }),
            v.end());
}

static void MuteNativeTips(HWND hovered, bool force) {
    if (!g_hideNative || !hovered || (!force && hovered == g_lastTipCheck))
        return;
    g_lastTipCheck = hovered;
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hovered, &pid);
    if (pid != GetCurrentProcessId())
        return;
    HWND defView = AncestorWithClass(hovered, L"SHELLDLL_DefView");
    if (!defView)
        return;

    PruneDeadHandles(g_mutedTips, L"tooltips_class32");
    PruneDeadHandles(g_mutedLists, L"SysListView32");

    std::vector<HWND> tips;
    wchar_t cls[64] = L"";
    GetClassNameW(hovered, cls, 64);
    if (!wcscmp(cls, L"SysListView32")) {
        DWORD_PTR ex = 0;
        if (SendMessageTimeoutW(hovered, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0,
                                SMTO_ABORTIFHUNG, 500, &ex) &&
            (ex & LVS_EX_INFOTIP)) {
            SendMessageTimeoutW(hovered, LVM_SETEXTENDEDLISTVIEWSTYLE,
                                LVS_EX_INFOTIP, 0, SMTO_ABORTIFHUNG, 500,
                                nullptr);
            if (std::find(g_mutedLists.begin(), g_mutedLists.end(), hovered) ==
                g_mutedLists.end())
                g_mutedLists.push_back(hovered);
        }
        DWORD_PTR tt = 0;
        if (SendMessageTimeoutW(hovered, LVM_GETTOOLTIPS, 0, 0,
                                SMTO_ABORTIFHUNG, 500, &tt) &&
            tt)
            tips.push_back((HWND)tt);
    }
    TipEnum e = {defView, &tips};
    EnumThreadWindows(tid, EnumTipsProc, (LPARAM)&e);

    for (HWND t : tips) {
        SendMessageTimeoutW(t, TTM_ACTIVATE, FALSE, 0, SMTO_ABORTIFHUNG, 500,
                            nullptr);
        if (std::find(g_mutedTips.begin(), g_mutedTips.end(), t) ==
            g_mutedTips.end())
            g_mutedTips.push_back(t);
    }
}

static void RestoreNativeTips() {
    for (HWND t : g_mutedTips) {
        if (IsOurWindowOfClass(t, L"tooltips_class32"))
            SendMessageTimeoutW(t, TTM_ACTIVATE, TRUE, 0, SMTO_ABORTIFHUNG,
                                500, nullptr);
    }
    for (HWND lv : g_mutedLists) {
        if (IsOurWindowOfClass(lv, L"SysListView32"))
            SendMessageTimeoutW(lv, LVM_SETEXTENDEDLISTVIEWSTYLE,
                                LVS_EX_INFOTIP, LVS_EX_INFOTIP,
                                SMTO_ABORTIFHUNG, 500, nullptr);
    }
    g_mutedTips.clear();
    g_mutedLists.clear();
    g_lastTipCheck = nullptr;
}

static bool ViewFromDispatch(IDispatch* d, IShellView** out) {
    CPtr<IServiceProvider> sp;
    CPtr<IShellBrowser> sb;
    if (!d || FAILED(d->QueryInterface(IID_PPV_ARGS(&sp))) ||
        FAILED(sp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&sb))))
        return false;
    return SUCCEEDED(sb->QueryActiveShellView(out)) && *out;
}

static bool GetShellView(HWND defView, bool isDesktop, IShellView** out) {
    *out = nullptr;
    if (!g_shellWindows &&
        FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&g_shellWindows))))
        return false;

    if (isDesktop) {
        VARIANT loc, root;
        VariantInit(&loc);
        VariantInit(&root);
        loc.vt = VT_I4;
        loc.lVal = CSIDL_DESKTOP;
        long hwnd = 0;
        CPtr<IDispatch> disp;
        if (FAILED(g_shellWindows->FindWindowSW(&loc, &root, SWC_DESKTOP, &hwnd,
                                                SWFO_NEEDDISPATCH, &disp)))
            return false;
        return ViewFromDispatch(disp, out);
    }

    long count = 0;
    if (FAILED(g_shellWindows->get_Count(&count)))
        return false;
    for (long i = 0; i < count; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        CPtr<IDispatch> disp;
        if (FAILED(g_shellWindows->Item(v, &disp)) || !disp)
            continue;
        CPtr<IShellView> view;
        if (!ViewFromDispatch(disp, &view))
            continue;
        HWND hv = nullptr;
        if (SUCCEEDED(view->GetWindow(&hv)) && hv == defView) {
            *out = view.p;
            view.p = nullptr;
            return true;
        }
    }
    return false;
}

// Finds the view's item whose display name matches.
static bool FindItemByName(IFolderView* fv, const std::wstring& name,
                           IShellItem2** out) {
    CPtr<IShellItemArray> arr;
    HRESULT hr = fv->Items(SVGIO_ALLVIEW, IID_PPV_ARGS(&arr));
    if (SUCCEEDED(hr) && arr) {
        DWORD n = 0;
        arr->GetCount(&n);
        for (DWORD i = 0; i < n; i++) {
            CPtr<IShellItem> it;
            if (FAILED(arr->GetItemAt(i, &it)))
                continue;
            PWSTR dn = nullptr;
            if (FAILED(it->GetDisplayName(SIGDN_NORMALDISPLAY, &dn)) || !dn)
                continue;
            bool match = name == dn;
            CoTaskMemFree(dn);
            if (match)
                return SUCCEEDED(it->QueryInterface(IID_PPV_ARGS(out)));
        }
        return false;
    }
    Wh_Log(L"IFolderView::Items failed 0x%08X", (unsigned)hr);
    return false;
}

// Desktop: ask the icon list view directly (we're in the same process).
static bool ListViewHit(HWND lv, POINT pt, std::wstring* name, RECT* rc) {
    LVHITTESTINFO ht = {};
    ht.pt = pt;
    ScreenToClient(lv, &ht.pt);
    int idx = (int)SendMessageW(lv, LVM_HITTEST, 0, (LPARAM)&ht);
    if (idx < 0 || !(ht.flags & LVHT_ONITEM))
        return false;
    wchar_t buf[MAX_PATH] = L"";
    LVITEMW it = {};
    it.iSubItem = 0;
    it.pszText = buf;
    it.cchTextMax = MAX_PATH;
    SendMessageW(lv, LVM_GETITEMTEXTW, idx, (LPARAM)&it);
    *name = buf;
    RECT r = {};
    r.left = LVIR_BOUNDS;
    if (SendMessageW(lv, LVM_GETITEMRECT, idx, (LPARAM)&r)) {
        MapWindowPoints(lv, nullptr, (POINT*)&r, 2);
        *rc = r;
    } else {
        SetRect(rc, pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
    }
    return !name->empty();
}

// File Explorer: MSAA hit-test (works on the DirectUI item view).
static bool MsaaHit(POINT pt, std::wstring* name, RECT* rc) {
    CPtr<IAccessible> acc;
    VARIANT child;
    VariantInit(&child);
    HRESULT hr = AccessibleObjectFromPoint(pt, &acc, &child);
    if (FAILED(hr) || !acc) {
        return false;
    }
    for (int i = 0; i < 4 && acc; i++) {
        VARIANT role;
        VariantInit(&role);
        acc->get_accRole(child, &role);
        long r = role.vt == VT_I4 ? role.lVal : -1;
        VariantClear(&role);
        if (r == ROLE_SYSTEM_LISTITEM) {
            BSTR b = nullptr;
            acc->get_accName(child, &b);
            *name = b ? b : L"";
            SysFreeString(b);
            long l = 0, t = 0, w = 0, h = 0;
            if (SUCCEEDED(acc->accLocation(&l, &t, &w, &h, child)) && w && h)
                SetRect(rc, l, t, l + w, t + h);
            else
                SetRect(rc, pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
            return !name->empty();
        }
        // Move to the parent object.
        if (child.vt == VT_I4 && child.lVal != CHILDID_SELF) {
            child.lVal = CHILDID_SELF;
            continue;
        }
        CPtr<IDispatch> pd;
        if (FAILED(acc->get_accParent(&pd)) || !pd)
            break;
        CPtr<IAccessible> parent;
        if (FAILED(pd->QueryInterface(IID_PPV_ARGS(&parent))))
            break;
        acc = std::move(parent);
        VariantInit(&child);
        child.vt = VT_I4;
        child.lVal = CHILDID_SELF;
    }
    return false;
}

// Name and bounds of the list item under the cursor in window `h`.
static bool HitTestItem(HWND h, POINT pt, std::wstring* name, RECT* rc) {
    wchar_t cls[64] = L"";
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"SysListView32"))
        return ListViewHit(h, pt, name, rc);
    return MsaaHit(pt, name, rc);
}

static bool ResolveHoveredItem(POINT pt, IShellItem2** out, RECT* itemRect,
                               UINT* dpi, HWND* defViewOut) {
    *out = nullptr;
    HWND h = WindowFromPoint(pt);
    if (!h)
        return false;
    wchar_t hcls[64] = L"";
    GetClassNameW(h, hcls, 64);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId())
        return false;
    HWND defView = AncestorWithClass(h, L"SHELLDLL_DefView");
    if (!defView)
        return false;
    wchar_t cls[64] = L"";
    GetClassNameW(GetAncestor(defView, GA_ROOT), cls, 64);
    bool isDesktop = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
    if (isDesktop ? !g_set.onDesktop : !g_set.inExplorer)
        return false;

    std::wstring name;
    if (!HitTestItem(h, pt, &name, itemRect))
        return false;
    g_cardHitName = name;

    CPtr<IShellView> view;
    CPtr<IFolderView> fv;
    if (!GetShellView(defView, isDesktop, &view)) {
        Wh_Log(L"Shell view not found (ShellWindows=%p)", g_shellWindows);
        return false;
    }
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&fv));
    if (FAILED(hr)) {
        Wh_Log(L"IFolderView failed 0x%08X", (unsigned)hr);
        return false;
    }
    if (!FindItemByName(fv, name, out)) {
        Wh_Log(L"Item not found for '%s'", name.c_str());
        return false;
    }
    *dpi = GetDpiForWindow(h);
    if (!*dpi)
        *dpi = 96;
    *defViewOut = defView;
    return true;
}

// ---------------------------------------------------------------------------
// Hover polling
// ---------------------------------------------------------------------------
static void ReloadSettingsNow();

static void Tick() {
    if (g_busy)
        return;
    POINT pt;
    if (!GetCursorPos(&pt))
        return;
    ULONGLONG now = GetTickCount64();

    bool button = (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
                   GetAsyncKeyState(VK_MBUTTON)) &
                  0x8000;
    if (button) {
        if (g_visible)
            HideCard();
        g_lastPt = pt;
        g_stillSince = now;
        g_evaluated = true;  // wait for the next movement
        return;
    }

    if (pt.x != g_lastPt.x || pt.y != g_lastPt.y) {
        g_lastPt = pt;
        g_stillSince = now;
        g_evaluated = false;
        // Mute Windows' own popup as soon as the cursor enters a view, before
        // its own delay runs out.
        MuteNativeTips(WindowFromPoint(pt), false);
        if (g_visible && !PtInRect(&g_itemRect, pt))
            HideCard();
        return;
    }

    if (g_visible) {
        // The cursor is still, but what's under it can change (scrolling with
        // the wheel, renaming, a window closing or covering the view). Check
        // now and then that it's still the same item.
        if (now - g_lastVerify < 300)
            return;
        g_lastVerify = now;
        HWND h = WindowFromPoint(pt);
        bool same = h && AncestorWithClass(h, L"SHELLDLL_DefView") == g_cardDefView;
        if (same) {
            g_busy = true;
            std::wstring name;
            RECT rc;
            same = HitTestItem(h, pt, &name, &rc) && name == g_cardHitName &&
                   EqualRect(&rc, &g_itemRect);
            g_busy = false;
        }
        if (!same) {
            HideCard();
            g_stillSince = now;  // show the new item after the usual delay
            g_evaluated = false;
        }
        if (g_reloadPending) {
            g_reloadPending = false;
            ReloadSettingsNow();
        }
        return;
    }

    if (g_evaluated || now - g_stillSince < (ULONGLONG)g_set.delay)
        return;
    g_evaluated = true;

    g_busy = true;
    CPtr<IShellItem2> item;
    RECT rc;
    UINT dpi = 96;
    HWND defView = nullptr;
    if (ResolveHoveredItem(pt, &item, &rc, &dpi, &defView)) {
        BuildPalette();
        if (BuildCard(item, dpi)) {
            g_itemRect = rc;
            g_cardDefView = defView;
            // Explorer may have re-enabled its tooltip; apply again.
            MuteNativeTips(WindowFromPoint(pt), true);
            g_lastVerify = GetTickCount64();
            ShowCardAt(pt);
            if (g_card->sizeRow >= 0)
                StartFolderSize(g_card->sizePath);
        } else {
            Wh_Log(L"BuildCard failed");
        }
    }
    g_busy = false;

    if (g_reloadPending) {
        g_reloadPending = false;
        ReloadSettingsNow();
    }
}

static void ReloadSettingsNow() {
    if (!g_hideNative)
        RestoreNativeTips();
    else
        g_lastTipCheck = nullptr;
    HideCard();
    DestroyCardWindow();
    LoadSettings();
    CreateCardWindow();
}

static LRESULT CALLBACK MsgWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_POLL)
                Tick();
            else if (wp == TIMER_FADE)
                FadeStep();
            return 0;
        case WM_APP_SIZEDONE: {
            SizeResult* r = (SizeResult*)lp;
            if (r->gen == g_sizeGen.load() &&
                g_visible && g_card->sizeRow >= 0 &&
                g_card->sizeRow < (int)g_card->rows.size()) {
                std::wstring v = FormatBytes(r->bytes);
                if (r->partial)
                    v = L"\u2265 " + v + L"  (" + S().partial + L")";
                g_card->rows[g_card->sizeRow].value = v;
                LayoutCard();
                ShowCardAt(g_cardPt, true);
            }
            delete r;
            return 0;
        }
        case WM_APP_RELOAD:
            if (g_busy)
                g_reloadPending = true;
            else
                ReloadSettingsNow();
            return 0;
        case WM_APP_QUIT:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            KillTimer(h, TIMER_POLL);
            KillTimer(h, TIMER_FADE);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
static DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    LoadSettings();
    g_card.emplace();

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown**>(&g_dw));
    if (g_d2d)
        RecreateRenderTarget();

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = kMsgClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW cc = {sizeof(cc)};
    cc.lpfnWndProc = CardWndProc;
    cc.hInstance = g_hInst;
    cc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    cc.lpszClassName = kCardClass;
    RegisterClassExW(&cc);

    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, g_hInst, nullptr);
    if (g_msgWnd && g_d2d && g_dw && g_rt && !g_quit) {
        CreateCardWindow();
        SetTimer(g_msgWnd, TIMER_POLL, 40, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else if (!g_quit) {
        Wh_Log(L"Initialization failed: msgWnd=%p d2d=%p dw=%p rt=%p",
               g_msgWnd.load(), g_d2d, g_dw, g_rt);
    }

    CancelFolderSize(true);
    RestoreNativeTips();
    HideCard();
    DestroyCardWindow();
    if (g_msgWnd && IsWindow(g_msgWnd))
        DestroyWindow(g_msgWnd);
    g_msgWnd = nullptr;
    g_card.reset();
    if (g_shellWindows) { g_shellWindows->Release(); g_shellWindows = nullptr; }
    if (g_rt) { g_rt->Release(); g_rt = nullptr; }
    if (g_dw) { g_dw->Release(); g_dw = nullptr; }
    if (g_d2d) { g_d2d->Release(); g_d2d = nullptr; }
    UnregisterClassW(kCardClass, g_hInst);
    UnregisterClassW(kMsgClass, g_hInst);
    if (SUCCEEDED(hrCo))
        CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Hiding Windows' own infotip: Explorer asks shell32 whether "Show pop-up
// description" is on. We answer "no" while the option is enabled.
// ---------------------------------------------------------------------------
using SHGetSetSettings_t = void(WINAPI*)(LPSHELLSTATE, DWORD, BOOL);
static SHGetSetSettings_t SHGetSetSettings_orig;
static void WINAPI SHGetSetSettings_hook(LPSHELLSTATE lpss, DWORD mask,
                                         BOOL set) {
    SHGetSetSettings_orig(lpss, mask, set);
    if (!set && lpss && (mask & SSF_SHOWINFOTIP) && g_hideNative)
        lpss->fShowInfoTip = FALSE;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_hideNative = Wh_GetIntSetting(L"hideNativeTooltip") != 0;
    // shell32 is always loaded in Explorer.
    if (HMODULE shell32 = GetModuleHandleW(L"shell32.dll")) {
        if (auto p = (SHGetSetSettings_t)GetProcAddress(shell32,
                                                        "SHGetSetSettings"))
            WindhawkUtils::SetFunctionHook(p, SHGetSetSettings_hook,
                                           &SHGetSetSettings_orig);
    }
    // Module handle for the window classes, without taking a reference.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WorkerThread), &g_hInst);
    g_quit = false;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_thread) {
        g_quit = true;
        // The worker may still be starting up, so keep posting until it exits.
        do {
            if (HWND w = g_msgWnd)
                PostMessageW(w, WM_APP_QUIT, 0, 0);
        } while (WaitForSingleObject(g_thread, 100) == WAIT_TIMEOUT);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    g_hideNative = Wh_GetIntSetting(L"hideNativeTooltip") != 0;
    if (g_msgWnd)
        PostMessageW(g_msgWnd, WM_APP_RELOAD, 0, 0);
}
