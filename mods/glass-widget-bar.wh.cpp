// ==WindhawkMod==
// @id              glass-widget-bar
// @name            Glass Widget Bar
// @name:tr-TR      Cam Widget Barı
// @description     A frosted-glass desktop bar with an analog clock, a calendar and music controls
// @description:tr-TR Buzlu cam görünümlü masaüstü barı: analog saat, takvim ve müzik denetimi
// @version         0.3.0
// @author          TheRealAlperN
// @github          https://github.com/TheRealAlperN
// @license         GPL-3.0
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lole32 -loleaut32 -ladvapi32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Glass Widget Bar

A desktop widget bar that sits in a frosted-glass frame: the blurred wallpaper
shows through a soft, dark-tinted rounded panel with thin light edges. The
widgets have no glass of their own; they are drawn straight onto the frame and
separated by thin dividers.

![Glass Widget Bar in English, Turkish, German and Japanese](https://raw.githubusercontent.com/TheRealAlperN/glass-widget-bar/main/screenshots/glass-widget-bar.gif)

| Widget | What it does |
| --- | --- |
| **Clock** | Analog clock with an orange second hand |
| **Calendar** | Weekday, large day number, month grid; today is highlighted in red |
| **Music** | The track that is playing right now (Spotify, browser, Media Player...), cover art, progress and ⏮ ⏯ ⏭ buttons |

## Languages
Labels, day names and month names follow your **Windows display language**
automatically. You can also pick a language in the settings.

English, Türkçe, Deutsch, Français, Español, Italiano, Português (Brasil),
Nederlands, Polski, Русский, Українська, Čeština, Svenska, Dansk, Norsk bokmål,
Suomi, Ελληνικά, Magyar, Română, Български, Bahasa Indonesia, Tiếng Việt, ไทย,
हिन्दी, 日本語, 한국어, 简体中文, 繁體中文.

Other display languages fall back to English. The first day of the week in
the calendar follows your Windows regional format (or the chosen language).
Right-to-left languages are not supported yet. The translations are
machine-assisted, corrections are welcome.

## Notes
- The default layer is the **desktop**: windows cover the bar. Press the
  hotkey (default **Ctrl+Alt+W**) to bring it to the front (press again to
  send it back), or set "Always on top" in the settings.
- The hotkey, its modifier keys and its key can be changed or turned off in
  the settings. Windows reports AltGr as Ctrl+Alt, so if your keyboard layout
  types characters with AltGr, pick a combination without Alt. If another app
  already owns the combination it cannot be registered; the Windhawk log then
  says so.
- Works on the primary monitor only. A picture wallpaper is expected.
- The defaults of the frame (opacity 15, blur 10) match the dock mod this
  frame comes from. If your wallpaper is very bright, raise the opacity.
- The music widget reads the Windows media session, so every app that
  publishes one is shown.
- Developed on Windows 11 (64-bit).

## Credits and license
The frame (rounded "squircle" corners, edge glow and edge lines) is ported
from **MacOS Style Dock** by Zach Vaughn (GPL-3.0). This mod is therefore
released under **GPL-3.0** as well.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $name:tr-TR: Dil
  $description: Language of the labels, day names and month names. "Automatic" follows the Windows display language.
  $description:tr-TR: Etiketlerin, gün ve ay adlarının dili. "Otomatik" Windows görüntüleme dilini izler.
  $options:
  - auto: Automatic (Windows display language)
  - en: English
  - tr: Türkçe
  - de: Deutsch
  - fr: Français
  - es: Español
  - it: Italiano
  - pt: Português (Brasil)
  - nl: Nederlands
  - pl: Polski
  - ru: Русский
  - ja: 日本語
  - ko: 한국어
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - uk: Українська
  - cs: Čeština
  - sv: Svenska
  - da: Dansk
  - nb: Norsk bokmål
  - fi: Suomi
  - el: Ελληνικά
  - hu: Magyar
  - ro: Română
  - id: Bahasa Indonesia
  - vi: Tiếng Việt
  - th: ไทย
  - hi: हिन्दी
  - bg: Български
  $options:tr-TR:
  - auto: Otomatik (Windows görüntüleme dili)
  - en: English
  - tr: Türkçe
  - de: Deutsch
  - fr: Français
  - es: Español
  - it: Italiano
  - pt: Português (Brasil)
  - nl: Nederlands
  - pl: Polski
  - ru: Русский
  - ja: 日本語
  - ko: 한국어
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - uk: Українська
  - cs: Čeština
  - sv: Svenska
  - da: Dansk
  - nb: Norsk bokmål
  - fi: Suomi
  - el: Ελληνικά
  - hu: Magyar
  - ro: Română
  - id: Bahasa Indonesia
  - vi: Tiếng Việt
  - th: ไทย
  - hi: हिन्दी
  - bg: Български
- position: top
  $name: Position
  $name:tr-TR: Konum
  $options:
  - top: Top center
  - bottom: Bottom center
  $options:tr-TR:
  - top: Üst orta
  - bottom: Alt orta
- layer: desktop
  $name: Layer
  $name:tr-TR: Katman
  $options:
  - desktop: Desktop (below windows)
  - top: Always on top
  $options:tr-TR:
  - desktop: Masaüstü (pencerelerin altında)
  - top: Hep üstte
- margin: 16
  $name: Distance from the screen edge (px)
  $name:tr-TR: Ekran kenarına uzaklık (px)
- opacity: 15
  $name: Frame opacity (0-100, dock default 15)
  $name:tr-TR: Çerçeve opaklığı (0-100, dock varsayılanı 15)
- blur: 10
  $name: Frame blur (0-100, dock default 10)
  $name:tr-TR: Çerçeve bulanıklığı (0-100, dock varsayılanı 10)
- hotkeyEnabled: true
  $name: Enable the "bring to front" hotkey
  $name:tr-TR: '"Öne getir" kısayolunu etkinleştir'
  $description: Turn this off if the key combination is needed by another app. A hotkey that another app already owns cannot be registered; the reason is written to the Windhawk log.
  $description:tr-TR: Tuş birleşimi başka bir uygulamada gerekiyorsa kapatın. Başka bir uygulamanın kullandığı kısayol kaydedilemez; sebep Windhawk günlüğüne yazılır.
- hotkeyModifiers: ctrl_alt
  $name: Hotkey modifier keys
  $name:tr-TR: Kısayol değiştirici tuşları
  $description: Windows reports AltGr as Ctrl+Alt. If your keyboard layout types characters with AltGr, pick a combination without Alt.
  $description:tr-TR: Windows, AltGr tuşunu Ctrl+Alt olarak bildirir. Klavye düzeniniz AltGr ile karakter yazıyorsa Alt içermeyen bir birleşim seçin.
  $options:
  - ctrl_alt: Ctrl + Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt_shift: Ctrl + Alt + Shift
  - win_shift: Win + Shift
  $options:tr-TR:
  - ctrl_alt: Ctrl + Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt_shift: Ctrl + Alt + Shift
  - win_shift: Win + Shift
- hotkeyKey: W
  $name: Hotkey key (a letter A-Z or a digit 0-9)
  $name:tr-TR: Kısayol tuşu (A-Z arası harf veya 0-9 arası rakam)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shellscalingapi.h>
#include <objidl.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>

using namespace Gdiplus;
namespace wmc = winrt::Windows::Media::Control;
namespace wss = winrt::Windows::Storage::Streams;

extern "C" IMAGE_DOS_HEADER __ImageBase;  // this mod's own module

namespace {

// ---------------------------------------------------------------- language
struct Lang {
    const wchar_t* id;      // value of the "language" setting
    const wchar_t* locale;  // locale name for day and month names
    const wchar_t* music;
    const wchar_t* playing;
    const wchar_t* paused;
    const wchar_t* idle;
};

const Lang kLangs[] = {
    {L"en", L"en-US", L"Music", L"Now playing", L"Paused", L"Nothing playing"},
    {L"tr", L"tr-TR", L"Müzik", L"Şu an çalıyor", L"Duraklatıldı", L"Çalan bir şey yok"},
    {L"de", L"de-DE", L"Musik", L"Läuft gerade", L"Pausiert", L"Keine Wiedergabe"},
    {L"fr", L"fr-FR", L"Musique", L"En cours de lecture", L"En pause", L"Aucune lecture"},
    {L"es", L"es-ES", L"Música", L"Reproduciendo", L"En pausa", L"Nada sonando"},
    {L"it", L"it-IT", L"Musica", L"In riproduzione", L"In pausa", L"Nessuna riproduzione"},
    {L"pt", L"pt-BR", L"Música", L"Tocando agora", L"Pausado", L"Nada tocando"},
    {L"nl", L"nl-NL", L"Muziek", L"Nu aan het afspelen", L"Gepauzeerd", L"Er speelt niets"},
    {L"pl", L"pl-PL", L"Muzyka", L"Teraz odtwarzane", L"Wstrzymano", L"Brak odtwarzania"},
    {L"ru", L"ru-RU", L"Музыка", L"Сейчас играет", L"Пауза", L"Ничего не играет"},
    {L"ja", L"ja-JP", L"ミュージック", L"再生中", L"一時停止中", L"再生中の曲はありません"},
    {L"ko", L"ko-KR", L"음악", L"지금 재생 중", L"일시 정지됨", L"재생 중인 항목 없음"},
    {L"zh-CN", L"zh-CN", L"音乐", L"正在播放", L"已暂停", L"没有正在播放的内容"},
    {L"zh-TW", L"zh-TW", L"音樂", L"正在播放", L"已暫停", L"沒有正在播放的內容"},
    {L"uk", L"uk-UA", L"Музика", L"Зараз грає", L"Пауза", L"Нічого не грає"},
    {L"cs", L"cs-CZ", L"Hudba", L"Právě hraje", L"Pozastaveno", L"Nic nehraje"},
    {L"sv", L"sv-SE", L"Musik", L"Spelas nu", L"Pausad", L"Inget spelas"},
    {L"da", L"da-DK", L"Musik", L"Afspiller nu", L"På pause", L"Intet spiller"},
    {L"nb", L"nb-NO", L"Musikk", L"Spilles nå", L"Satt på pause", L"Ingenting spilles"},
    {L"fi", L"fi-FI", L"Musiikki", L"Nyt soi", L"Tauolla", L"Ei soitettavaa"},
    {L"el", L"el-GR", L"Μουσική", L"Αναπαραγωγή τώρα", L"Σε παύση", L"Καμία αναπαραγωγή"},
    {L"hu", L"hu-HU", L"Zene", L"Most szól", L"Szüneteltetve", L"Nem szól semmi"},
    {L"ro", L"ro-RO", L"Muzică", L"Se redă acum", L"În pauză", L"Nimic în redare"},
    {L"id", L"id-ID", L"Musik", L"Sedang diputar", L"Dijeda", L"Tidak ada yang diputar"},
    {L"vi", L"vi-VN", L"Nhạc", L"Đang phát", L"Đã tạm dừng", L"Không có gì đang phát"},
    {L"th", L"th-TH", L"เพลง", L"กำลังเล่น", L"หยุดชั่วคราว", L"ไม่มีเพลงที่เล่นอยู่"},
    {L"hi", L"hi-IN", L"संगीत", L"अभी चल रहा है", L"रुका हुआ", L"कुछ नहीं चल रहा"},
    {L"bg", L"bg-BG", L"Музика", L"Възпроизвежда се", L"На пауза", L"Нищо не се възпроизвежда"},
};

const Lang* g_lang = &kLangs[0];
const wchar_t* g_dateLoc = L"en-US";
int g_firstDow = 0;  // first calendar column: 0 = Monday ... 6 = Sunday

const Lang* FindLang(const wchar_t* id) {
    for (const Lang& l : kLangs)
        if (wcscmp(l.id, id) == 0) return &l;
    return nullptr;
}

// Windows display language -> one of the supported languages (English if none).
const Lang* SystemLang() {
    LANGID id = GetUserDefaultUILanguage();
    const wchar_t* pick = L"en";
    switch (PRIMARYLANGID(id)) {
        case LANG_TURKISH: pick = L"tr"; break;
        case LANG_GERMAN: pick = L"de"; break;
        case LANG_FRENCH: pick = L"fr"; break;
        case LANG_SPANISH: pick = L"es"; break;
        case LANG_ITALIAN: pick = L"it"; break;
        case LANG_PORTUGUESE: pick = L"pt"; break;
        case LANG_DUTCH: pick = L"nl"; break;
        case LANG_POLISH: pick = L"pl"; break;
        case LANG_RUSSIAN: pick = L"ru"; break;
        case LANG_JAPANESE: pick = L"ja"; break;
        case LANG_KOREAN: pick = L"ko"; break;
        case LANG_UKRAINIAN: pick = L"uk"; break;
        case LANG_CZECH: pick = L"cs"; break;
        case LANG_SWEDISH: pick = L"sv"; break;
        case LANG_DANISH: pick = L"da"; break;
        case LANG_NORWEGIAN: pick = L"nb"; break;
        case LANG_FINNISH: pick = L"fi"; break;
        case LANG_GREEK: pick = L"el"; break;
        case LANG_HUNGARIAN: pick = L"hu"; break;
        case LANG_ROMANIAN: pick = L"ro"; break;
        case LANG_INDONESIAN: pick = L"id"; break;
        case LANG_VIETNAMESE: pick = L"vi"; break;
        case LANG_THAI: pick = L"th"; break;
        case LANG_HINDI: pick = L"hi"; break;
        case LANG_BULGARIAN: pick = L"bg"; break;
        case LANG_CHINESE: {
            WORD sub = SUBLANGID(id);
            bool traditional = sub == SUBLANG_CHINESE_TRADITIONAL ||
                               sub == SUBLANG_CHINESE_HONGKONG || sub == SUBLANG_CHINESE_MACAU;
            pick = traditional ? L"zh-TW" : L"zh-CN";
            break;
        }
    }
    return FindLang(pick);
}

void SelectLanguage(const wchar_t* setting) {
    const Lang* l = nullptr;
    if (setting && *setting && wcscmp(setting, L"auto") != 0) l = FindLang(setting);
    bool automatic = !l;
    if (!l) l = SystemLang();
    g_lang = l;
    g_dateLoc = l->locale;

    // The week start is a Windows regional setting; follow it in automatic mode.
    DWORD v = 0;
    if (GetLocaleInfoEx(automatic ? LOCALE_NAME_USER_DEFAULT : l->locale,
                        LOCALE_IFIRSTDAYOFWEEK | LOCALE_RETURN_NUMBER, (LPWSTR)&v,
                        sizeof(v) / sizeof(wchar_t)) &&
        v <= 6)
        g_firstDow = (int)v;
    else
        g_firstDow = 0;
}

std::wstring Upper(const wchar_t* s) {
    wchar_t out[96] = L"";
    if (!LCMapStringEx(g_dateLoc, LCMAP_UPPERCASE | LCMAP_LINGUISTIC_CASING, s, -1, out, 96,
                       nullptr, nullptr, 0))
        return s;
    return out;
}

// ---------------------------------------------------------------- settings
struct Settings {
    bool bottom = false;
    bool topmost = false;
    int margin = 16;
    int opacity = 15;
    int blur = 10;
    bool hotkeyOn = true;
    UINT hotkeyMods = MOD_CONTROL | MOD_ALT;
    UINT hotkeyVk = 'W';
} g_set;

void LoadSettings() {
    PCWSTR pos = Wh_GetStringSetting(L"position");
    g_set.bottom = pos && wcscmp(pos, L"bottom") == 0;
    Wh_FreeStringSetting(pos);

    PCWSTR layer = Wh_GetStringSetting(L"layer");
    g_set.topmost = layer && wcscmp(layer, L"top") == 0;
    Wh_FreeStringSetting(layer);

    g_set.margin = Wh_GetIntSetting(L"margin");
    if (g_set.margin <= 0) g_set.margin = 16;
    if (g_set.margin > 400) g_set.margin = 400;

    g_set.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    g_set.blur = std::clamp(Wh_GetIntSetting(L"blur"), 0, 100);

    g_set.hotkeyOn = Wh_GetIntSetting(L"hotkeyEnabled") != 0;

    PCWSTR mods = Wh_GetStringSetting(L"hotkeyModifiers");
    g_set.hotkeyMods = MOD_CONTROL | MOD_ALT;
    if (mods) {
        if (wcscmp(mods, L"ctrl_shift") == 0) g_set.hotkeyMods = MOD_CONTROL | MOD_SHIFT;
        else if (wcscmp(mods, L"ctrl_alt_shift") == 0)
            g_set.hotkeyMods = MOD_CONTROL | MOD_ALT | MOD_SHIFT;
        else if (wcscmp(mods, L"win_shift") == 0) g_set.hotkeyMods = MOD_WIN | MOD_SHIFT;
    }
    Wh_FreeStringSetting(mods);

    PCWSTR key = Wh_GetStringSetting(L"hotkeyKey");
    g_set.hotkeyVk = 'W';
    if (key) {
        wchar_t c = key[0];
        if (c >= L'a' && c <= L'z') c = (wchar_t)(c - L'a' + L'A');
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) g_set.hotkeyVk = (UINT)c;
    }
    Wh_FreeStringSetting(key);

    PCWSTR lang = Wh_GetStringSetting(L"language");
    SelectLanguage(lang);
    Wh_FreeStringSetting(lang);
}

// ---------------------------------------------------------------- state
enum { C_CLOCK, C_CAL, C_MUSIC, C_COUNT };
const float kCellW[C_COUNT] = {116, 236, 290};
enum { B_PREV, B_PLAY, B_NEXT, B_COUNT };

std::atomic<HWND> g_hwnd{nullptr};
HANDLE g_uiReady = nullptr;  // manual-reset event: set once the window exists
HANDLE g_thread = nullptr;
ULONG_PTR g_gdiTok = 0;

float S = 1.0f;       // DPI scale
float M = 8;          // window margin (px)
int W = 0, H = 0;     // window size
int g_winX = 0, g_winY = 0;
RectF g_bar;          // bar in window coordinates
RectF g_cell[C_COUNT];
RectF g_btn[B_COUNT]; // click areas of the music buttons
RECT g_mon = {0, 0, 1920, 1080};

HDC g_memDC = nullptr;
HBITMAP g_dib = nullptr, g_oldBmp = nullptr;
void* g_bits = nullptr;
std::unique_ptr<Bitmap> g_frame, g_glass;
std::unique_ptr<Font> fNum, fTitle, fLabel, fSmall, fSmallB, fTiny, fNumeral, fSym;

int g_hoverBtn = -1;
bool g_tracking = false;
bool g_peek = false;
int g_lastSec = -1;

// ---- music
struct MediaState {
    bool have = false;
    bool playing = false;
    bool canPrev = false, canNext = false, canToggle = false;
    bool hasTimeline = false;
    double pos = 0, dur = 0;
    ULONGLONG posTick = 0;
    std::wstring title, artist;
    int artGen = 0;
    std::vector<BYTE> art;
};
MediaState g_media;
std::mutex g_mediaMx;
// A global strong reference to an out-of-process WinRT object: never destroyed at
// process exit (releasing the proxy during shutdown can hang).
[[clang::no_destroy]] wmc::GlobalSystemMediaTransportControlsSession g_session{nullptr};
HANDLE g_mediaThread = nullptr;
HANDLE g_mediaEvt = nullptr;

// copy on the UI side
MediaState g_m;
int g_artSeen = -1;
std::unique_ptr<Bitmap> g_artBmp;
bool g_dirty = true;

// ---------------------------------------------------------------- helpers
inline Color Ca(int a, int r, int g, int b) {
    return Color((BYTE)std::clamp(a, 0, 255), (BYTE)r, (BYTE)g, (BYTE)b);
}
const int RED[3] = {255, 69, 58};
const int ORANGE[3] = {255, 159, 10};

void RRect(GraphicsPath& p, float x, float y, float w, float h, float r) {
    r = (std::min)(r, (std::min)(w, h) / 2);
    float d = r * 2;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

Font* MakeFont(std::initializer_list<const wchar_t*> names, float px, int style = FontStyleRegular) {
    for (const wchar_t* n : names) {
        FontFamily ff(n);
        if (ff.GetLastStatus() == Ok && ff.IsStyleAvailable(style))
            return new Font(&ff, px, style, UnitPixel);
    }
    return new Font(FontFamily::GenericSansSerif(), px, style, UnitPixel);
}

void Txt(Graphics& g, const std::wstring& s, Font* f, Color c, RectF r,
         StringAlignment ha = StringAlignmentNear,
         StringAlignment va = StringAlignmentNear, bool ellipsis = false) {
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetAlignment(ha);
    sf.SetLineAlignment(va);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    if (ellipsis) sf.SetTrimming(StringTrimmingEllipsisCharacter);
    SolidBrush b(c);
    g.DrawString(s.c_str(), -1, f, r, &sf, &b);
}

// ---------------------------------------------------------------- Apple corner (from dock)
// Dock-mode "squircle": an arc close to round, with flat edges smoothed by Beziers
// smoothed. Points are in radius units from the corner (along the edge, inward and outward).
constexpr float kCornerReach = 1.52866f;
const float kCorner[16][2] = {
    {kCornerReach, 0},
    {1.08849f, 0}, {0.86841f, 0}, {0.66993f, 0.06550f},
    {0.66993f, 0.06550f}, {0.63149f, 0.07491f}, {0.63149f, 0.07491f},
    {0.37282f, 0.16906f}, {0.16906f, 0.37282f}, {0.07491f, 0.63149f},
    {0.07491f, 0.63149f}, {0.06550f, 0.66993f}, {0.06550f, 0.66993f},
    {0, 0.86841f}, {0, 1.08849f}, {0, kCornerReach},
};

float SquircleRadius(float w, float h, float r) {
    return std::clamp((std::min)(w, h) / 2 / kCornerReach, 0.0f, r);
}

// Closed frame: one start point, then cubic curves of three points each (70 points).
std::vector<PointF> SquirclePts(float x, float y, float w, float h, float rx, float ry) {
    struct Corner { float x, y, inX, inY, outX, outY; };
    Corner corners[] = {{x + w, y, -1, 0, 0, 1}, {x + w, y + h, 0, -1, -1, 0},
                        {x, y + h, 1, 0, 0, -1}, {x, y, 0, 1, 1, 0}};
    std::vector<PointF> pts;
    for (auto& c : corners) {
        for (int i = 0; i < 16; i++) {
            float in = kCorner[i][0], out = kCorner[i][1];
            PointF p(c.x + (in * c.inX + out * c.outX) * rx,
                     c.y + (in * c.inY + out * c.outY) * ry);
            if (i == 0 && !pts.empty()) {
                PointF back = pts.back();
                pts.push_back(back);  // flat edge: control points at the ends
                pts.push_back(p);
            }
            pts.push_back(p);
        }
    }
    return pts;
}

void AddSquircle(GraphicsPath& gp, float x, float y, float w, float h, float rx, float ry) {
    auto pts = SquirclePts(x, y, w, h, rx, ry);
    gp.StartFigure();
    gp.AddBeziers(pts.data(), (INT)pts.size());
    gp.CloseFigure();
}

// ---------------------------------------------------------------- wallpaper
void BoxBlur(BYTE* px, int w, int h, int stride, int r) {
    if (r < 1 || w < 2 || h < 2) return;
    std::vector<BYTE> tmp((size_t)stride * h);
    int win = r * 2 + 1;
    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < h; y++) {
            BYTE* row = px + (size_t)y * stride;
            BYTE* out = tmp.data() + (size_t)y * stride;
            for (int c = 0; c < 4; c++) {
                int sum = 0;
                for (int k = -r; k <= r; k++)
                    sum += row[std::clamp(k, 0, w - 1) * 4 + c];
                for (int x = 0; x < w; x++) {
                    out[x * 4 + c] = (BYTE)(sum / win);
                    sum += row[(std::min)(x + r + 1, w - 1) * 4 + c];
                    sum -= row[(std::max)(x - r, 0) * 4 + c];
                }
            }
        }
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 4; c++) {
                int sum = 0;
                for (int k = -r; k <= r; k++)
                    sum += tmp[(size_t)std::clamp(k, 0, h - 1) * stride + x * 4 + c];
                for (int y = 0; y < h; y++) {
                    px[(size_t)y * stride + x * 4 + c] = (BYTE)(sum / win);
                    sum += tmp[(size_t)(std::min)(y + r + 1, h - 1) * stride + x * 4 + c];
                    sum -= tmp[(size_t)(std::max)(y - r, 0) * stride + x * 4 + c];
                }
            }
        }
    }
}

std::unique_ptr<Bitmap> LoadWallpaperScreen(int mw, int mh) {
    std::unique_ptr<Bitmap> scr(new Bitmap(mw, mh, PixelFormat32bppARGB));
    Graphics g(scr.get());
    COLORREF bg = GetSysColor(COLOR_DESKTOP);
    g.Clear(Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));

    wchar_t path[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    if (!path[0]) return scr;

    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return scr;
    DWORD sz = GetFileSize(f, nullptr);
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, sz);
    if (!hg) { CloseHandle(f); return scr; }
    void* mem = GlobalLock(hg);
    DWORD rd = 0;
    ReadFile(f, mem, sz, &rd, nullptr);
    GlobalUnlock(hg);
    CloseHandle(f);

    IStream* st = nullptr;
    if (FAILED(CreateStreamOnHGlobal(hg, TRUE, &st)) || !st) {
        GlobalFree(hg);
        return scr;
    }
    {
        Bitmap img(st);
        if (img.GetLastStatus() == Ok) {
            int iw = img.GetWidth(), ih = img.GetHeight();
            wchar_t style[16] = L"10", tile[16] = L"0";
            DWORD n = sizeof(style);
            RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop",
                         L"WallpaperStyle", RRF_RT_REG_SZ, nullptr, style, &n);
            n = sizeof(tile);
            RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop",
                         L"TileWallpaper", RRF_RT_REG_SZ, nullptr, tile, &n);
            int st_ = _wtoi(style);
            bool tiled = _wtoi(tile) != 0;
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            g.SetPixelOffsetMode(PixelOffsetModeHalf);
            auto draw = [&](float dx, float dy, float dw, float dh) {
                g.DrawImage(&img, RectF(dx, dy, dw, dh), 0.f, 0.f, (REAL)iw,
                            (REAL)ih, UnitPixel);
            };
            if (tiled && st_ == 0) {
                for (int y = 0; y < mh; y += ih)
                    for (int x = 0; x < mw; x += iw) draw((float)x, (float)y, (float)iw, (float)ih);
            } else if (st_ == 2) {
                draw(0, 0, (float)mw, (float)mh);
            } else if (st_ == 6) {
                float k = (std::min)((float)mw / iw, (float)mh / ih);
                draw((mw - iw * k) / 2, (mh - ih * k) / 2, iw * k, ih * k);
            } else if (st_ == 0) {
                draw((mw - iw) / 2.f, (mh - ih) / 2.f, (float)iw, (float)ih);
            } else {  // 10 fill, 22 span
                float k = (std::max)((float)mw / iw, (float)mh / ih);
                draw((mw - iw * k) / 2, (mh - ih * k) / 2, iw * k, ih * k);
            }
        }
    }
    st->Release();
    return scr;
}

// Takes a piece of the wallpaper and blurs it (at full resolution).
std::unique_ptr<Bitmap> BlurCrop(Bitmap* scr, int cx, int cy, int cw, int ch, int radius) {
    std::unique_ptr<Bitmap> out(new Bitmap(cw, ch, PixelFormat32bppARGB));
    {
        Graphics g(out.get());
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.DrawImage(scr, Rect(0, 0, cw, ch), cx, cy, cw, ch, UnitPixel);
    }
    BitmapData bd;
    Rect rc(0, 0, cw, ch);
    if (out->LockBits(&rc, ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB,
                      &bd) == Ok) {
        BoxBlur((BYTE*)bd.Scan0, cw, ch, bd.Stride, radius);
        out->UnlockBits(&bd);
    }
    return out;
}

// ---------------------------------------------------------------- wallpaper change
// The blurred plate is only built in BuildGlass. WM_SETTINGCHANGE does not fire
// on every change (slideshow, theme), or it fires before the file is written, so
// the wallpaper "signature" (path, file time/size, layout, solid color) is polled
// regularly; when it changes, the plate is rebuilt.
struct WpSig {
    std::wstring path;
    ULONGLONG mtime = 0, size = 0;
    int style = 0, tile = 0;
    DWORD bg = 0;
    bool operator==(const WpSig& o) const {
        return path == o.path && mtime == o.mtime && size == o.size && style == o.style &&
               tile == o.tile && bg == o.bg;
    }
};
WpSig g_wpSig, g_wpPending;
bool g_wpHavePending = false;

WpSig ReadWpSig() {
    WpSig s;
    wchar_t path[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    s.path = path;
    if (path[0]) {
        WIN32_FILE_ATTRIBUTE_DATA fd;
        if (GetFileAttributesExW(path, GetFileExInfoStandard, &fd)) {
            s.mtime = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                      fd.ftLastWriteTime.dwLowDateTime;
            s.size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
        }
    }
    wchar_t v[16] = L"";
    DWORD n = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle",
                     RRF_RT_REG_SZ, nullptr, v, &n) == ERROR_SUCCESS)
        s.style = _wtoi(v);
    v[0] = 0;
    n = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"TileWallpaper",
                     RRF_RT_REG_SZ, nullptr, v, &n) == ERROR_SUCCESS)
        s.tile = _wtoi(v);
    s.bg = GetSysColor(COLOR_DESKTOP);
    return s;
}

void BuildGlass();

// True if the signature changed and is still the same at the next check (file write done).
bool CheckWallpaper() {
    WpSig cur = ReadWpSig();
    if (cur == g_wpSig) {
        g_wpHavePending = false;
        return false;
    }
    if (g_wpHavePending && cur == g_wpPending) {
        g_wpHavePending = false;
        BuildGlass();
        return true;
    }
    g_wpPending = cur;
    g_wpHavePending = true;
    return false;
}

// ---------------------------------------------------------------- glass frame (dock recipe)
// Dock's plate: blurred background → color → inward fading glow → top/bottom
// thin light line, side dark line. Dock settings: dark (#333), "mac" edge.
void BuildGlass() {
    g_wpSig = ReadWpSig();  // wallpaper this plate was built from
    g_wpHavePending = false;
    g_glass.reset(new Bitmap(W, H, PixelFormat32bppPARGB));
    Graphics g(g_glass.get());
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.Clear(Color(0, 0, 0, 0));

    float bx = g_bar.X, by = g_bar.Y, bw = g_bar.Width, bh = g_bar.Height;
    float r = SquircleRadius(bw, bh, (float)(int)(bh * 0.3f + 0.5f));
    float in = (std::max)(0.0f, r - 1);

    GraphicsPath plate;
    AddSquircle(plate, bx, by, bw, bh, r, r);

    // --- blurred wallpaper (dock: Blur 10 → offset 6 px)
    int mw = g_mon.right - g_mon.left, mh = g_mon.bottom - g_mon.top;
    std::unique_ptr<Bitmap> scr = LoadWallpaperScreen(mw, mh);
    int barSX = g_winX + (int)bx - g_mon.left;
    int barSY = g_winY + (int)by - g_mon.top;
    float sigma = g_set.blur * 0.6f * S;
    int rad = sigma < 0.6f ? 0 : (std::max)(1, (int)std::lround(sigma - 0.5f));
    int pad = rad * 3 + 6;
    int cx0 = (std::max)(0, barSX - pad), cy0 = (std::max)(0, barSY - pad);
    int cx1 = (std::min)(mw, barSX + (int)bw + pad);
    int cy1 = (std::min)(mh, barSY + (int)bh + pad);
    int cw = cx1 - cx0, ch = cy1 - cy0;
    if (cw < 8 || ch < 8) return;
    std::unique_ptr<Bitmap> blur = BlurCrop(scr.get(), cx0, cy0, cw, ch, rad);
    {
        TextureBrush tb(blur.get(), WrapModeClamp);
        tb.TranslateTransform((float)(cx0 - barSX) + bx, (float)(cy0 - barSY) + by);
        g.FillPath(&tb, &plate);
    }

    // --- color: dark #333, at dock opacity
    int fillA = g_set.opacity * 255 / 100;
    {
        SolidBrush fb(Ca(fillA, 0x33, 0x33, 0x33));
        g.FillPath(&fb, &plate);
    }

    // --- glow fading inward from top and bottom (half Gaussian)
    {
        const int steps = 10;
        const float falloff = 4.5f;
        float depth = 6.0f * 1.15f * S;  // dock: 6 px, scales with the dock
        float reach = std::clamp(depth / bh, 0.0f, 0.5f);
        int glowA = (int)std::lround(fillA * 0.6f);  // dock: glow opacity 60
        Color cols[steps * 2];
        REAL pos[steps * 2];
        float end = std::exp(-falloff);
        for (int i = 0; i < steps; i++) {
            float t = i / (steps - 1.0f);
            float strength = (std::exp(-falloff * t * t) - end) / (1 - end);
            Color c = Ca((int)std::lround(glowA * strength), 194, 194, 194);
            cols[i] = c;
            pos[i] = reach * t;
            cols[steps * 2 - 1 - i] = c;
            pos[steps * 2 - 1 - i] = 1 - reach * t;
        }
        LinearGradientBrush glow(PointF(0, by), PointF(0, by + bh), cols[0], cols[0]);
        glow.SetInterpolationColors(cols, pos, steps * 2);
        g.FillPath(&glow, &plate);
    }

    // --- edges: top/bottom light inside, sides dark outside
    {
        float t = std::clamp((float)g_set.opacity / 30.0f, 0.0f, 1.0f);
        int edgeA = (int)std::lround(255 * (0.20f + (0.65f - 0.20f) * t));
        GraphicsPath inner;
        inner.SetFillMode(FillModeAlternate);
        AddSquircle(inner, bx, by, bw, bh, r, r);
        AddSquircle(inner, bx, by + 1, bw, bh - 2, r, in);
        SolidBrush lb(Ca((int)std::lround(edgeA * 0.5f), 194, 194, 194));
        g.FillPath(&lb, &inner);

        GraphicsPath outer;
        outer.SetFillMode(FillModeAlternate);
        AddSquircle(outer, bx - 1, by, bw + 2, bh, r + 1, r);
        AddSquircle(outer, bx, by, bw, bh, r, r);
        SolidBrush db(Ca(edgeA, 15, 15, 15));
        g.FillPath(&db, &outer);
    }
}

// ---------------------------------------------------------------- layout
std::wstring g_wdInit[7];  // first letters of the weekdays, starting Monday

void BuildWeekdayInitials() {
    for (int i = 0; i < 7; i++) {
        SYSTEMTIME st = {};
        // 1 Jan 2024 is a Monday; column i shows weekday (g_firstDow + i)
        st.wYear = 2024; st.wMonth = 1; st.wDay = (WORD)(1 + (g_firstDow + i) % 7);
        wchar_t buf[32] = L"";
        GetDateFormatEx(g_dateLoc, 0, &st, L"ddd", buf, 32, nullptr);
        std::wstring up = Upper(buf);
        // Vietnamese uses "T2".."T7": the first letter alone would be ambiguous
        bool digits = up.find_first_of(L"0123456789") != std::wstring::npos;
        g_wdInit[i] = up.empty() ? std::wstring(L"?") : (digits ? up : up.substr(0, 1));
    }
}

void Layout() {
    // The bar lives on the primary monitor: use that monitor's current scale, so a
    // change of the display scaling (or of the primary monitor) is followed.
    HMONITOR hm = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(hm, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) || dpiX == 0) dpiX = 96;
    S = dpiX / 96.0f;
    M = 8 * S;
    float pad = 20 * S, gap = 36 * S;
    float barH = 148 * S;
    float inner = 0;
    for (int i = 0; i < C_COUNT; i++) inner += kCellW[i] * S;
    float barW = inner + gap * (C_COUNT - 1) + pad * 2;
    W = (int)std::ceil(barW + M * 2);
    H = (int)std::ceil(barH + M * 2);
    g_bar = RectF(M, M, barW, barH);
    float x = M + pad;
    for (int i = 0; i < C_COUNT; i++) {
        g_cell[i] = RectF(x, M + 16 * S, kCellW[i] * S, barH - 32 * S);
        x += kCellW[i] * S + gap;
    }

    // music buttons
    {
        RectF c = g_cell[C_MUSIC];
        float colX = c.X + 116 * S, colW = c.Width - 116 * S;
        float cx = colX + colW / 2, cy = c.Y + 98 * S, bs = 36 * S;
        float off[B_COUNT] = {-46 * S, 0, 46 * S};
        for (int i = 0; i < B_COUNT; i++)
            g_btn[i] = RectF(cx + off[i] - bs / 2, cy - bs / 2, bs, bs);
    }

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(hm, &mi);
    g_mon = mi.rcMonitor;
    int monW = mi.rcMonitor.right - mi.rcMonitor.left;
    g_winX = mi.rcMonitor.left + (monW - W) / 2;
    if (g_set.bottom)
        g_winY = mi.rcWork.bottom - (int)(g_set.margin * S) - (int)barH - (int)M;
    else
        g_winY = mi.rcWork.top + (int)(g_set.margin * S) - (int)M;

    // drawing surface
    if (g_memDC) {
        g_frame.reset();
        SelectObject(g_memDC, g_oldBmp);
        DeleteObject(g_dib);
    } else {
        HDC sdc = GetDC(nullptr);
        g_memDC = CreateCompatibleDC(sdc);
        ReleaseDC(nullptr, sdc);
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_dib = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, &g_bits, nullptr, 0);
    g_oldBmp = (HBITMAP)SelectObject(g_memDC, g_dib);
    g_frame.reset(new Bitmap(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)g_bits));

    fNum.reset(MakeFont({L"Segoe UI Variable Display Light", L"Segoe UI Light"}, 46 * S));
    fTitle.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 15 * S, FontStyleBold));
    fLabel.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 11 * S, FontStyleBold));
    fSmall.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 12 * S));
    fSmallB.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 10 * S, FontStyleBold));
    fTiny.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 9.5f * S));
    fNumeral.reset(MakeFont({L"Segoe UI Variable Display", L"Segoe UI"}, 14 * S, FontStyleBold));
    fSym.reset(MakeFont({L"Segoe UI Symbol", L"Segoe UI"}, 40 * S));

    BuildWeekdayInitials();
    BuildGlass();
}

// ---------------------------------------------------------------- music data
void MediaCommand(int which) {
    wmc::GlobalSystemMediaTransportControlsSession s{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_mediaMx);
        s = g_session;
    }
    if (!s) return;
    try {
        if (which == B_PREV) s.TrySkipPreviousAsync();
        else if (which == B_PLAY) s.TryTogglePlayPauseAsync();
        else if (which == B_NEXT) s.TrySkipNextAsync();
    } catch (...) {
    }
}

// Waits for a WinRT async operation for a bounded time so that the stop event is
// looked at regularly even when the media service or the playing app does not answer.
template <typename Op>
auto AwaitWithTimeout(const Op& op) {
    if (op.wait_for(std::chrono::seconds(3)) ==
        winrt::Windows::Foundation::AsyncStatus::Started) {
        op.Cancel();
        throw winrt::hresult_error(HRESULT_FROM_WIN32(ERROR_TIMEOUT));
    }
    return op.GetResults();
}

DWORD WINAPI MediaThread(LPVOID) {
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
    wmc::GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    std::wstring lastKey;
    std::vector<BYTE> art;
    int artGen = 0, artTries = 0;

    do {
        try {
            if (!mgr)
                mgr = AwaitWithTimeout(
                    wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync());
            auto s = mgr.GetCurrentSession();
            MediaState ns;
            if (s) {
                auto props = AwaitWithTimeout(s.TryGetMediaPropertiesAsync());
                ns.title = props.Title().c_str();
                ns.artist = props.Artist().c_str();
                std::wstring key = ns.title + L"\x1f" + ns.artist + L"\x1f" + props.AlbumTitle().c_str();
                ns.have = !ns.title.empty() || !ns.artist.empty();

                if (key != lastKey) {
                    lastKey = key;
                    art.clear();
                    artTries = 0;
                    artGen++;
                }
                if (art.empty() && artTries < 6) {
                    artTries++;
                    try {
                        auto thumb = props.Thumbnail();
                        if (thumb) {
                            auto st = AwaitWithTimeout(thumb.OpenReadAsync());
                            uint64_t size = st.Size();
                            if (size > 0 && size < 8u * 1024 * 1024) {
                                wss::DataReader dr(st);
                                AwaitWithTimeout(dr.LoadAsync((uint32_t)size));
                                art.resize((size_t)size);
                                dr.ReadBytes(winrt::array_view<uint8_t>(art.data(), art.data() + art.size()));
                                artGen++;
                            }
                        }
                    } catch (...) {
                        art.clear();
                    }
                }

                auto pi = s.GetPlaybackInfo();
                ns.playing = pi.PlaybackStatus() ==
                             wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                auto ctl = pi.Controls();
                ns.canPrev = ctl.IsPreviousEnabled();
                ns.canNext = ctl.IsNextEnabled();
                ns.canToggle = ctl.IsPlayEnabled() || ctl.IsPauseEnabled();

                auto tl = s.GetTimelineProperties();
                double start = std::chrono::duration<double>(tl.StartTime()).count();
                double end = std::chrono::duration<double>(tl.EndTime()).count();
                double pos = std::chrono::duration<double>(tl.Position()).count() - start;
                ns.dur = end - start;
                ns.hasTimeline = ns.dur > 1.0 && ns.dur < 6 * 3600.0;
                if (ns.hasTimeline) {
                    if (ns.playing) {
                        double since = std::chrono::duration<double>(
                            winrt::clock::now() - tl.LastUpdatedTime()).count();
                        if (since > 0 && since < ns.dur) pos += since;
                    }
                    ns.pos = std::clamp(pos, 0.0, ns.dur);
                }
                ns.posTick = GetTickCount64();
            }
            std::lock_guard<std::mutex> lk(g_mediaMx);
            ns.art = art;
            ns.artGen = artGen;
            g_media = std::move(ns);
            g_session = s;
        } catch (...) {
            mgr = nullptr;
            std::lock_guard<std::mutex> lk(g_mediaMx);
            g_media = MediaState();
            g_session = nullptr;
        }
    } while (WaitForSingleObject(g_mediaEvt, 800) == WAIT_TIMEOUT);

    {
        std::lock_guard<std::mutex> lk(g_mediaMx);
        g_session = nullptr;
    }
    mgr = nullptr;
    try { winrt::uninit_apartment(); } catch (...) {}
    return 0;
}

std::unique_ptr<Bitmap> DecodeArt(const std::vector<BYTE>& bytes) {
    if (bytes.empty()) return nullptr;
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!hg) return nullptr;
    void* mem = GlobalLock(hg);
    memcpy(mem, bytes.data(), bytes.size());
    GlobalUnlock(hg);
    IStream* st = nullptr;
    if (FAILED(CreateStreamOnHGlobal(hg, TRUE, &st)) || !st) {
        GlobalFree(hg);
        return nullptr;
    }
    std::unique_ptr<Bitmap> out;
    {
        Bitmap img(st);
        if (img.GetLastStatus() == Ok && img.GetWidth() > 0 && img.GetHeight() > 0) {
            int w = img.GetWidth(), h = img.GetHeight();
            out.reset(new Bitmap(w, h, PixelFormat32bppPARGB));
            Graphics g(out.get());
            g.DrawImage(&img, Rect(0, 0, w, h), 0, 0, w, h, UnitPixel);
        }
    }
    st->Release();
    return out;
}

// UI side: copies the shared state. Returns true if anything changed.
bool MediaTick() {
    MediaState c;
    {
        std::lock_guard<std::mutex> lk(g_mediaMx);
        c = g_media;
    }
    bool changed = c.have != g_m.have || c.playing != g_m.playing || c.title != g_m.title ||
                   c.artist != g_m.artist || c.artGen != g_m.artGen ||
                   c.canPrev != g_m.canPrev || c.canNext != g_m.canNext;
    if (c.artGen != g_artSeen) {
        g_artBmp = DecodeArt(c.art);
        g_artSeen = c.artGen;
    }
    c.art.clear();
    g_m = std::move(c);
    return changed;
}

std::wstring Mmss(double sec) {
    int s = (int)(sec + 0.5);
    wchar_t b[24];
    if (s >= 3600) wsprintfW(b, L"%d:%02d:%02d", s / 3600, (s / 60) % 60, s % 60);
    else wsprintfW(b, L"%d:%02d", s / 60, s % 60);
    return b;
}

// ---------------------------------------------------------------- drawing
void DrawDividers(Graphics& g) {
    // dock separator: light gray on the dark dock, %94 / 255 opacity depending on fill color
    float gap = 36 * S;
    for (int i = 0; i < C_COUNT - 1; i++) {
        float x = g_cell[i].X + g_cell[i].Width + gap / 2;
        float h = g_cell[i].Height * 0.62f;
        float y = g_cell[i].Y + (g_cell[i].Height - h) / 2;
        SolidBrush b(Ca(94, 230, 230, 230));
        g.FillRectangle(&b, x - 0.5f * S, y, 1.0f * S, h);
    }
}

void DrawClock(Graphics& g, const RectF& c, const SYSTEMTIME& lt) {
    float R = (std::min)(c.Width, c.Height) / 2 - 1 * S;
    float cx = c.X + c.Width / 2, cy = c.Y + c.Height / 2;
    const float kPi = 3.14159265f;

    SolidBrush face(Ca(64, 0, 0, 0));
    g.FillEllipse(&face, cx - R, cy - R, R * 2, R * 2);
    Pen ring(Ca(40, 255, 255, 255), 1.0f * S);
    g.DrawEllipse(&ring, cx - R, cy - R, R * 2, R * 2);

    for (int i = 0; i < 60; i++) {
        bool major = i % 5 == 0;
        float a = i * 6 * kPi / 180;
        float r1 = R - 3.5f * S, r2 = R - (major ? 10.0f : 6.5f) * S;
        Pen p(major ? Ca(235, 255, 255, 255) : Ca(85, 255, 255, 255), (major ? 2.0f : 1.0f) * S);
        p.SetStartCap(LineCapRound);
        p.SetEndCap(LineCapRound);
        g.DrawLine(&p, cx + std::sin(a) * r1, cy - std::cos(a) * r1, cx + std::sin(a) * r2,
                   cy - std::cos(a) * r2);
    }
    const wchar_t* nums[4] = {L"12", L"3", L"6", L"9"};
    for (int i = 0; i < 4; i++) {
        float a = i * 90 * kPi / 180, rn = R - 22 * S;
        float nx = cx + std::sin(a) * rn, ny = cy - std::cos(a) * rn;
        Txt(g, nums[i], fNumeral.get(), Ca(245, 255, 255, 255),
            RectF(nx - 16 * S, ny - 10 * S, 32 * S, 20 * S), StringAlignmentCenter,
            StringAlignmentCenter);
    }

    auto hand = [&](float deg, float len, float tail, float w, Color col) {
        float a = deg * kPi / 180;
        float dx = std::sin(a), dy = -std::cos(a);
        Pen p(col, w);
        p.SetStartCap(LineCapRound);
        p.SetEndCap(LineCapRound);
        g.DrawLine(&p, cx - dx * tail, cy - dy * tail, cx + dx * len, cy + dy * len);
    };
    float hourDeg = ((lt.wHour % 12) + lt.wMinute / 60.0f) * 30.0f;
    float minDeg = (lt.wMinute + lt.wSecond / 60.0f) * 6.0f;
    float secDeg = lt.wSecond * 6.0f;
    hand(hourDeg, R * 0.50f, 0, 4.2f * S, Ca(255, 255, 255, 255));
    hand(minDeg, R * 0.76f, 0, 3.0f * S, Ca(255, 255, 255, 255));
    hand(secDeg, R * 0.84f, R * 0.18f, 1.5f * S, Ca(255, ORANGE[0], ORANGE[1], ORANGE[2]));
    SolidBrush dot(Ca(255, ORANGE[0], ORANGE[1], ORANGE[2]));
    g.FillEllipse(&dot, cx - 3.6f * S, cy - 3.6f * S, 7.2f * S, 7.2f * S);
    SolidBrush hole(Ca(255, 30, 30, 30));
    g.FillEllipse(&hole, cx - 1.3f * S, cy - 1.3f * S, 2.6f * S, 2.6f * S);
}

int DaysInMonth(int y, int m) {
    static const int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return d[m - 1];
}

void DrawCalendar(Graphics& g, const RectF& c, const SYSTEMTIME& lt) {
    // left block: day name (red), big number, month and year
    wchar_t wd[48] = L"", my[48] = L"";
    GetDateFormatEx(g_dateLoc, 0, &lt, L"dddd", wd, 48, nullptr);
    std::wstring wdUp = Upper(wd);
    GetDateFormatEx(g_dateLoc, DATE_YEARMONTH, &lt, nullptr, my, 48, nullptr);
    wchar_t day[8];
    wsprintfW(day, L"%d", lt.wDay);

    float lw = 100 * S;
    Txt(g, wdUp, fSmallB.get(), Ca(255, RED[0], RED[1], RED[2]),
        RectF(c.X, c.Y + 4 * S, lw, 14 * S), StringAlignmentNear, StringAlignmentNear, true);
    Txt(g, day, fNum.get(), Ca(255, 255, 255, 255), RectF(c.X - 2 * S, c.Y + 18 * S, lw, 60 * S));
    {
        // shrink long month names (e.g. "Tháng Mười Một 2026") to stay clear of the grid
        float maxW = lw + 8 * S;
        StringFormat mf(StringFormat::GenericTypographic());
        RectF bb;
        g.MeasureString(my, -1, fSmall.get(), PointF(0, 0), &mf, &bb);
        std::unique_ptr<Font> fit;
        Font* mfont = fSmall.get();
        if (bb.Width > maxW) {
            FontFamily fam;
            fSmall->GetFamily(&fam);
            fit.reset(new Font(&fam, fSmall->GetSize() * maxW / bb.Width, fSmall->GetStyle(),
                               UnitPixel));
            mfont = fit.get();
        }
        Txt(g, my, mfont, Ca(160, 255, 255, 255), RectF(c.X, c.Y + 82 * S, maxW + 4 * S, 16 * S),
            StringAlignmentNear, StringAlignmentNear, true);
    }

    // right block: month grid, weeks start on Monday
    float gx = c.X + 112 * S, gw = c.Width - 112 * S, colW = gw / 7, rowH = 15 * S;
    float gy = c.Y + 2 * S;
    for (int i = 0; i < 7; i++) {
        Txt(g, g_wdInit[i], fSmallB.get(), Ca(110, 255, 255, 255),
            RectF(gx + i * colW, gy, colW, rowH), StringAlignmentCenter, StringAlignmentCenter);
    }
    int first = ((lt.wDayOfWeek - (lt.wDay - 1)) % 7 + 7) % 7;  // 0 = Sunday
    int off = (((first + 6) % 7) - g_firstDow + 7) % 7;         // column of the 1st
    int n = DaysInMonth(lt.wYear, lt.wMonth);
    for (int d = 1; d <= n; d++) {
        int idx = off + d - 1;
        int col = idx % 7, row = idx / 7;
        RectF cell(gx + col * colW, gy + (row + 1) * rowH + 3 * S, colW, rowH);
        wchar_t t[8];
        wsprintfW(t, L"%d", d);
        if (d == lt.wDay) {
            SolidBrush b(Ca(255, RED[0], RED[1], RED[2]));
            float rr = 7.6f * S;
            g.FillEllipse(&b, cell.X + colW / 2 - rr, cell.Y + rowH / 2 - rr, rr * 2, rr * 2);
            Txt(g, t, fSmallB.get(), Ca(255, 255, 255, 255), cell, StringAlignmentCenter,
                StringAlignmentCenter);
        } else {
            Txt(g, t, fTiny.get(), Ca(((g_firstDow + col) % 7) >= 5 ? 130 : 215, 255, 255, 255), cell,
                StringAlignmentCenter, StringAlignmentCenter);
        }
    }
}

void FillTri(Graphics& g, Color col, PointF a, PointF b, PointF c2) {
    PointF pts[3] = {a, b, c2};
    SolidBrush br(col);
    g.FillPolygon(&br, pts, 3);
}

void DrawMusic(Graphics& g, const RectF& c) {
    float as = 100 * S;
    RectF art(c.X, c.Y + (c.Height - as) / 2, as, as);
    float rad = 14 * S;

    // album art shadow
    for (int i = 0; i < 6; i++) {
        float k = i * 1.6f * S;
        GraphicsPath sp;
        RRect(sp, art.X - k, art.Y - k + 4 * S, art.Width + 2 * k, art.Height + 2 * k, rad + k);
        SolidBrush sb(Ca(9, 0, 0, 0));
        g.FillPath(&sb, &sp);
    }
    GraphicsPath ap;
    RRect(ap, art.X, art.Y, art.Width, art.Height, rad);
    if (g_artBmp) {
        int iw = g_artBmp->GetWidth(), ih = g_artBmp->GetHeight();
        float side = (float)(std::min)(iw, ih);
        g.SetClip(&ap);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.DrawImage(g_artBmp.get(), art, (iw - side) / 2, (ih - side) / 2, side, side, UnitPixel);
        g.ResetClip();
    } else {
        LinearGradientBrush pb(PointF(art.X, art.Y), PointF(art.X, art.Y + art.Height),
                               Ca(62, 255, 255, 255), Ca(18, 255, 255, 255));
        g.FillPath(&pb, &ap);
        Txt(g, L"♪", fSym.get(), Ca(150, 255, 255, 255), art, StringAlignmentCenter,
            StringAlignmentCenter);
    }
    Pen artEdge(Ca(34, 255, 255, 255), 1.0f * S);
    g.DrawPath(&artEdge, &ap);

    // right column
    float colX = c.X + 116 * S, colW = c.Width - 116 * S;
    std::wstring label = Upper(!g_m.have ? g_lang->music
                                         : (g_m.playing ? g_lang->playing : g_lang->paused));
    Txt(g, label, fSmallB.get(), Ca(125, 255, 255, 255), RectF(colX, c.Y + 8 * S, colW, 13 * S),
        StringAlignmentNear, StringAlignmentNear, true);
    std::wstring title = g_m.have && !g_m.title.empty() ? g_m.title : g_lang->idle;
    Txt(g, title, fTitle.get(), Ca(255, 255, 255, 255), RectF(colX, c.Y + 22 * S, colW, 20 * S),
        StringAlignmentNear, StringAlignmentNear, true);
    if (g_m.have)
        Txt(g, g_m.artist, fSmall.get(), Ca(150, 255, 255, 255),
            RectF(colX, c.Y + 42 * S, colW, 16 * S), StringAlignmentNear, StringAlignmentNear, true);

    // progress
    if (g_m.have && g_m.hasTimeline) {
        double pos = g_m.pos;
        if (g_m.playing) pos += (GetTickCount64() - g_m.posTick) / 1000.0;
        pos = std::clamp(pos, 0.0, g_m.dur);
        float by = c.Y + 66 * S, bh = 4 * S;
        GraphicsPath tp;
        RRect(tp, colX, by, colW, bh, bh / 2);
        SolidBrush tb(Ca(46, 255, 255, 255));
        g.FillPath(&tb, &tp);
        float fw = (std::max)(bh, colW * (float)(pos / g_m.dur));
        GraphicsPath fp;
        RRect(fp, colX, by, fw, bh, bh / 2);
        SolidBrush fb(Ca(225, 255, 255, 255));
        g.FillPath(&fb, &fp);
        Txt(g, Mmss(pos), fTiny.get(), Ca(120, 255, 255, 255), RectF(colX, by + 6 * S, 60 * S, 12 * S));
        Txt(g, L"-" + Mmss(g_m.dur - pos), fTiny.get(), Ca(120, 255, 255, 255),
            RectF(colX + colW - 60 * S, by + 6 * S, 60 * S, 12 * S), StringAlignmentFar);
    }

    // buttons
    for (int i = 0; i < B_COUNT; i++) {
        const RectF& b = g_btn[i];
        bool enabled = !g_m.have ? false : (i == B_PREV ? g_m.canPrev : i == B_NEXT ? g_m.canNext : g_m.canToggle);
        bool hot = g_hoverBtn == i && enabled;
        float cx = b.X + b.Width / 2, cy = b.Y + b.Height / 2;
        if (hot) {
            SolidBrush hb(Ca(34, 255, 255, 255));
            g.FillEllipse(&hb, cx - 16 * S, cy - 16 * S, 32 * S, 32 * S);
        }
        Color col = Ca(enabled ? 255 : 80, 255, 255, 255);
        if (i == B_PREV || i == B_NEXT) {
            float d = i == B_PREV ? -1.f : 1.f, w = 8.5f * S, h = 7.5f * S;
            for (int k = 0; k < 2; k++) {
                float bx = cx + d * (k - 0.5f) * w;
                FillTri(g, col, PointF(bx - d * w / 2, cy - h), PointF(bx - d * w / 2, cy + h),
                        PointF(bx + d * w / 2, cy));
            }
        } else if (g_m.playing) {
            float bw2 = 5.2f * S, bh2 = 16 * S, gp = 3.6f * S;
            GraphicsPath l, r;
            RRect(l, cx - gp - bw2, cy - bh2 / 2, bw2, bh2, 1.6f * S);
            RRect(r, cx + gp, cy - bh2 / 2, bw2, bh2, 1.6f * S);
            SolidBrush pb(col);
            g.FillPath(&pb, &l);
            g.FillPath(&pb, &r);
        } else {
            float h = 10 * S, w = 16 * S;
            FillTri(g, col, PointF(cx - w * 0.38f, cy - h), PointF(cx - w * 0.38f, cy + h),
                    PointF(cx + w * 0.62f, cy));
        }
    }
}

void Render() {
    if (!g_frame || !g_glass || !g_hwnd) return;
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    {
        Graphics g(g_frame.get());
        g.SetCompositingMode(CompositingModeSourceCopy);
        g.DrawImage(g_glass.get(), Rect(0, 0, W, H), 0, 0, W, H, UnitPixel);
        g.SetCompositingMode(CompositingModeSourceOver);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);

        DrawDividers(g);
        DrawClock(g, g_cell[C_CLOCK], lt);
        DrawCalendar(g, g_cell[C_CAL], lt);
        DrawMusic(g, g_cell[C_MUSIC]);
    }
    HDC sdc = GetDC(nullptr);
    POINT pt = {g_winX, g_winY}, src = {0, 0};
    SIZE sz = {W, H};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, sdc, &pt, &sz, g_memDC, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, sdc);
}

// ---------------------------------------------------------------- window
int BtnAt(float x, float y) {
    for (int i = 0; i < B_COUNT; i++)
        if (g_btn[i].Contains(x, y)) return i;
    return -1;
}

void ApplyZOrder() {
    if (!g_hwnd) return;
    bool top = g_set.topmost || g_peek;
    SetWindowPos(g_hwnd, top ? HWND_TOPMOST : HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// (Re)registers the "bring to front" hotkey from the settings; does nothing when it is off.
void ApplyHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, 1);
    if (!g_set.hotkeyOn) {
        g_peek = false;
        return;
    }
    if (!RegisterHotKey(hwnd, 1, g_set.hotkeyMods | MOD_NOREPEAT, g_set.hotkeyVk)) {
        Wh_Log(L"RegisterHotKey failed (error %lu): the key combination is probably used by another app",
               GetLastError());
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER: {
            if (wp == 1) {
                static int ticks = 0;
                bool dirty = g_dirty;
                g_dirty = false;
                SYSTEMTIME lt;
                GetLocalTime(&lt);
                if (lt.wSecond != g_lastSec) {
                    g_lastSec = lt.wSecond;
                    dirty = true;
                }
                if (++ticks % 8 == 0 && MediaTick()) dirty = true;
                if (ticks % 5 == 0 && CheckWallpaper()) dirty = true;  // about every 0.5 s
                if (ticks % 40 == 0 && (IsIconic(hwnd) || !IsWindowVisible(hwnd))) {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    ApplyZOrder();
                }
                if (dirty) Render();
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            int b = BtnAt((float)(short)LOWORD(lp), (float)(short)HIWORD(lp));
            if (!g_tracking) {
                TRACKMOUSEEVENT te = {sizeof(te), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&te);
                g_tracking = true;
            }
            if (b != g_hoverBtn) {
                g_hoverBtn = b;
                Render();
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_tracking = false;
            if (g_hoverBtn != -1) {
                g_hoverBtn = -1;
                Render();
            }
            return 0;
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            SetCursor(LoadCursorW(nullptr, BtnAt((float)pt.x, (float)pt.y) >= 0 ? IDC_HAND : IDC_ARROW));
            return TRUE;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONUP: {
            int b = BtnAt((float)(short)LOWORD(lp), (float)(short)HIWORD(lp));
            if (b >= 0) {
                MediaCommand(b);
                g_dirty = true;
            }
            return 0;
        }
        case WM_KILLFOCUS:
        case WM_ACTIVATE:
            ApplyZOrder();
            return 0;
        case WM_HOTKEY:
            g_peek = !g_peek;
            ApplyZOrder();
            return 0;
        case WM_SETTINGCHANGE:
            // The wallpaper message may arrive before the file is written, so the plate is not
            // rebuilt here; the timer's signature check (CheckWallpaper) refreshes it.
            if (wp == SPI_SETWORKAREA) {
                Layout();
                SetWindowPos(hwnd, nullptr, g_winX, g_winY, W, H, SWP_NOZORDER | SWP_NOACTIVATE);
                Render();
            }
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            Layout();
            Render();
            return 0;
        case WM_APP + 1:  // settings changed
            LoadSettings();
            ApplyHotkey(hwnd);
            Layout();
            ApplyZOrder();
            Render();
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            UnregisterHotKey(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI UiThread(LPVOID) {
    // The scale is read per monitor in Layout(), so this thread must be per-monitor DPI
    // aware; otherwise Windows would virtualize the coordinates and scale the bar.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        using SetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto setContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(
            user32, "SetThreadDpiAwarenessContext");
        if (setContext) setContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    GdiplusStartupInput in;
    if (GdiplusStartup(&g_gdiTok, &in, nullptr) != Ok) return 1;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.lpszClassName = L"GlassWidgetBar";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        GdiplusShutdown(g_gdiTok);
        return 1;
    }

    LoadSettings();
    Layout();
    MediaTick();

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW, wc.lpszClassName,
                                L"Glass Widget Bar", WS_POPUP, g_winX, g_winY, W, H,
                                nullptr, nullptr, wc.hInstance, nullptr);
    if (hwnd) {
        g_hwnd = hwnd;
        if (g_uiReady) SetEvent(g_uiReady);
        Render();
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        ApplyZOrder();
        Render();
        SetTimer(hwnd, 1, 100, nullptr);
        ApplyHotkey(hwnd);

        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
    g_hwnd = nullptr;
    g_frame.reset();
    g_glass.reset();
    g_artBmp.reset();
    fNum.reset(); fTitle.reset(); fLabel.reset(); fSmall.reset(); fSmallB.reset();
    fTiny.reset(); fNumeral.reset(); fSym.reset();
    if (g_memDC) {
        SelectObject(g_memDC, g_oldBmp);
        DeleteObject(g_dib);
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(g_gdiTok);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    g_mediaEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_mediaEvt || !g_uiReady) {
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        if (g_mediaEvt) CloseHandle(g_mediaEvt);
        if (g_uiReady) CloseHandle(g_uiReady);
        g_mediaEvt = g_uiReady = nullptr;
        return FALSE;
    }
    g_mediaThread = CreateThread(nullptr, 0, MediaThread, nullptr, 0, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP + 1, 0, 0);
}

void WhTool_ModUninit() {
    if (g_mediaEvt) SetEvent(g_mediaEvt);
    if (g_thread) {
        // Wait until the window exists, or until the thread has ended on its own
        // (startup failure); only then is it safe to ask it to close.
        HANDLE waits[] = {g_uiReady, g_thread};
        WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (HWND hwnd = g_hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_mediaThread) {
        // The blocking WinRT calls in the thread have a time limit, so this ends.
        WaitForSingleObject(g_mediaThread, INFINITE);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    if (g_mediaEvt) {
        CloseHandle(g_mediaEvt);
        g_mediaEvt = nullptr;
    }
    if (g_uiReady) {
        CloseHandle(g_uiReady);
        g_uiReady = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
