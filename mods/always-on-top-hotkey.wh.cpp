// ==WindhawkMod==
// @id              always-on-top-hotkey
// @name            Always On Top Hotkey
// @name:tr-TR      Her Zaman Üstte Kısayolu
// @name:fr-FR      Raccourci Toujours au premier plan
// @description     Press Ctrl+Alt+T to keep the active window above all others, with a themed badge and optional sound alerts
// @description:tr-TR Ctrl+Alt+T ile aktif pencereyi her şeyin üstünde tutun; temalı rozet ve isteğe bağlı sesli ikaz içerir
// @description:fr-FR Appuyez sur Ctrl+Alt+T pour garder la fenêtre active au premier plan, avec un badge à thème et des alertes sonores facultatives
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always On Top Hotkey

Press **Ctrl+Alt+T** to keep the active window above all other windows. While
a window is pinned, a badge stays on it until the window is closed or you press
the hotkey again to unpin it.

## Features

- Global hotkey: **Ctrl+Alt+T** toggles "always on top" for the active window.
- A badge that follows the pinned window, hides while it is minimized and is
  removed when the window closes. Several windows can be pinned at once.
- 13 badge themes: Classic red, Minimal dark, Light, Carbon amber, Gold, Neon
  green, Cyberpunk, Ocean, Aurora, Sunset, Emerald, Icon only and Custom colors
  (with optional gradient).
- Badge position (title bar right / center, bottom right / left), opacity and
  custom badge text.
- Optional sound alerts when pinning and unpinning: a two-tone chime, the
  Windows information or warning sound, or your own `.wav` files.
- Available in English, Turkish and French. The language follows the Windows
  display language automatically and can be overridden in the settings.
- When the mod is disabled, every pinned window is restored to normal.

## Notes

- Windows running as administrator (for example Task Manager) cannot be pinned
  by a non-elevated `explorer.exe`.
- If another program already uses Ctrl+Alt+T, the hotkey cannot be registered;
  the mod retries every couple of seconds and logs the failure once.
- Badge colors for the "Custom colors" theme are entered as `RRGGBB`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $name:tr-TR: Dil
  $name:fr-FR: Langue
  $description: "Language of the badge text. Automatic follows the Windows display language."
  $description:tr-TR: "Rozet yazısının dili. Otomatik seçenek Windows görüntü dilini izler."
  $description:fr-FR: "Langue du texte du badge. Automatique suit la langue d'affichage de Windows."
  $options:
    - auto: Automatic (Windows language)
    - en: English
    - tr: Türkçe
    - fr: Français
  $options:tr-TR:
    - auto: Otomatik (Windows dili)
    - en: English
    - tr: Türkçe
    - fr: Français
  $options:fr-FR:
    - auto: Automatique (langue de Windows)
    - en: English
    - tr: Türkçe
    - fr: Français
- sound:
    - enabled: true
      $name: Sound alert
      $name:tr-TR: Sesli ikaz
      $name:fr-FR: Alerte sonore
      $description: Play a sound when a window is pinned or unpinned.
      $description:tr-TR: Bir pencere sabitlenirken veya sabitleme kaldırılırken ses çal.
      $description:fr-FR: "Jouer un son lors de l'épinglage ou du désépinglage d'une fenêtre."
    - onPin: true
      $name: Play when pinning
      $name:tr-TR: Sabitlerken çal
      $name:fr-FR: "Jouer à l'épinglage"
    - onUnpin: true
      $name: Play when unpinning
      $name:tr-TR: Sabitleme kaldırılırken çal
      $name:fr-FR: Jouer au désépinglage
    - type: chime
      $name: Sound type
      $name:tr-TR: Ses türü
      $name:fr-FR: Type de son
      $options:
        - chime: Two-tone chime
        - asterisk: Windows information sound
        - exclamation: Windows warning sound
        - custom: Custom .wav file
      $options:tr-TR:
        - chime: Çift tonlu çınlama
        - asterisk: Windows bilgi sesi
        - exclamation: Windows uyarı sesi
        - custom: Özel .wav dosyası
      $options:fr-FR:
        - chime: Carillon à deux tons
        - asterisk: Son d'information Windows
        - exclamation: Son d'avertissement Windows
        - custom: Fichier .wav personnalisé
    - pinFile: ""
      $name: Custom sound file (pin)
      $name:tr-TR: Özel ses dosyası (sabitleme)
      $name:fr-FR: Fichier son personnalisé (épinglage)
      $description: Used when the sound type is Custom. Full path to a .wav file.
      $description:tr-TR: Ses türü Özel iken kullanılır. Bir .wav dosyasının tam yolu.
      $description:fr-FR: Utilisé lorsque le type de son est Personnalisé. Chemin complet d'un fichier .wav.
    - unpinFile: ""
      $name: Custom sound file (unpin)
      $name:tr-TR: Özel ses dosyası (kaldırma)
      $name:fr-FR: Fichier son personnalisé (désépinglage)
      $description: Used when the sound type is Custom. Full path to a .wav file.
      $description:tr-TR: Ses türü Özel iken kullanılır. Bir .wav dosyasının tam yolu.
      $description:fr-FR: Utilisé lorsque le type de son est Personnalisé. Chemin complet d'un fichier .wav.
  $name: Sound
  $name:tr-TR: Ses
  $name:fr-FR: Son
- badge:
    - theme: classic
      $name: Badge theme
      $name:tr-TR: Rozet teması
      $name:fr-FR: Thème du badge
      $options:
        - classic: Classic red
        - minimal: Minimal dark
        - light: Light
        - carbon: Carbon amber
        - gold: Gold
        - neon: Neon green
        - cyber: Cyberpunk
        - ocean: Ocean
        - aurora: Aurora
        - sunset: Sunset
        - emerald: Emerald
        - icon: Icon only
        - custom: Custom colors
      $options:tr-TR:
        - classic: Klasik kırmızı
        - minimal: Minimal koyu
        - light: Açık
        - carbon: Karbon amber
        - gold: Altın
        - neon: Neon yeşil
        - cyber: Cyberpunk
        - ocean: Okyanus
        - aurora: Aurora
        - sunset: Gün batımı
        - emerald: Zümrüt
        - icon: Sadece simge
        - custom: Özel renkler
      $options:fr-FR:
        - classic: Rouge classique
        - minimal: Sombre minimaliste
        - light: Clair
        - carbon: Carbone ambré
        - gold: Or
        - neon: Vert néon
        - cyber: Cyberpunk
        - ocean: Océan
        - aurora: Aurore
        - sunset: Coucher de soleil
        - emerald: Émeraude
        - icon: Icône seule
        - custom: Couleurs personnalisées
    - position: titleRight
      $name: Badge position
      $name:tr-TR: Rozet konumu
      $name:fr-FR: Position du badge
      $options:
        - titleRight: Title bar, right
        - titleCenter: Title bar, center
        - bottomRight: Bottom right corner
        - bottomLeft: Bottom left corner
      $options:tr-TR:
        - titleRight: Başlık çubuğu sağı
        - titleCenter: Başlık çubuğu ortası
        - bottomRight: Sağ alt köşe
        - bottomLeft: Sol alt köşe
      $options:fr-FR:
        - titleRight: Barre de titre, à droite
        - titleCenter: Barre de titre, au centre
        - bottomRight: Coin inférieur droit
        - bottomLeft: Coin inférieur gauche
    - opacity: 92
      $name: Opacity (40-100)
      $name:tr-TR: Saydamlık (40-100)
      $name:fr-FR: Opacité (40-100)
    - customText: ""
      $name: Custom badge text
      $name:tr-TR: Özel rozet yazısı
      $name:fr-FR: Texte du badge personnalisé
      $description: Leave empty to use the theme text in the selected language.
      $description:tr-TR: Boş bırakılırsa seçili dildeki tema yazısı kullanılır.
      $description:fr-FR: Laisser vide pour utiliser le texte du thème dans la langue choisie.
    - customBg: "C62828"
      $name: Custom theme - start color (RRGGBB)
      $name:tr-TR: Özel tema - başlangıç rengi (RRGGBB)
      $name:fr-FR: Thème personnalisé - couleur de départ (RRGGBB)
    - customBg2: ""
      $name: Custom theme - gradient end color (RRGGBB)
      $name:tr-TR: Özel tema - gradyan bitiş rengi (RRGGBB)
      $name:fr-FR: Thème personnalisé - couleur de fin du dégradé (RRGGBB)
      $description: Leave empty for a solid color.
      $description:tr-TR: Düz renk için boş bırakın.
      $description:fr-FR: Laisser vide pour une couleur unie.
    - customFg: "FFFFFF"
      $name: Custom theme - text color (RRGGBB)
      $name:tr-TR: Özel tema - yazı rengi (RRGGBB)
      $name:fr-FR: Thème personnalisé - couleur du texte (RRGGBB)
  $name: Badge
  $name:tr-TR: Rozet
  $name:fr-FR: Badge
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <cstdlib>
#include <cwchar>
#include <map>
#include <string>

// ---------------------------------------------------------------------------
// Types and global state
// ---------------------------------------------------------------------------

static const wchar_t kClassName[] = L"WhAlwaysOnTopHotkeyBadge";
static const int kHotkeyId = 0x4A07;
static const UINT WM_APP_RELOAD = WM_APP + 1;

enum class Lang { En = 0, Tr = 1, Fr = 2 };
enum class SoundType { Chime, Asterisk, Exclamation, Custom };
enum class BadgePos { TitleRight, TitleCenter, BottomRight, BottomLeft };

struct LangStrings {
    const wchar_t* onTop;
    const wchar_t* pinned;
};

// Indexed by Lang.
static const LangStrings kLangStrings[] = {
    {L"ON TOP", L"PINNED"},
    {L"\u00DCSTTE", L"SAB\u0130T"},
    {L"AU-DESSUS", L"\u00C9PINGL\u00C9"},
};

struct BadgeStyle {
    COLORREF bg;
    COLORREF bg2;  // gradient end; equal to bg for a solid color
    COLORREF fg;
    COLORREF border;
    int borderW;   // pixels at 96 DPI
    int radius;    // pixels at 96 DPI, -1 = pill / circle
    int fontPx;    // pixels at 96 DPI
    bool bold;
    bool squareIcon;
    bool usePinnedLabel;
    const wchar_t* prefix;
    const wchar_t* font;
};

struct SoundSettings {
    bool enabled = true;
    bool onPin = true;
    bool onUnpin = true;
    SoundType type = SoundType::Chime;
    std::wstring pinFile;
    std::wstring unpinFile;
};

static std::map<HWND, HWND> g_pins;  // pinned window -> badge window
static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static HANDLE g_readyEvent = nullptr;

static bool g_hotkeyRegistered = false;
static bool g_hotkeyFailLogged = false;
static int g_tick = 0;

static SoundSettings g_snd;
static BadgeStyle g_style;
static std::wstring g_text;
static BadgePos g_pos = BadgePos::TitleRight;
static BYTE g_alpha = 235;
static int g_badgeW = 86;
static int g_badgeH = 24;
static int g_badgeRadius = 3;
static int g_borderW = 0;
static double g_scale = 1.0;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static std::wstring GetStr(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

static COLORREF ParseHex(const std::wstring& text, COLORREF fallback) {
    std::wstring t = text;
    if (!t.empty() && t[0] == L'#') {
        t.erase(0, 1);
    }
    if (t.size() != 6) {
        return fallback;
    }
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(t.c_str(), &end, 16);
    if (!end || *end != L'\0') {
        return fallback;
    }
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static Lang ResolveLanguage(const std::wstring& setting) {
    if (setting == L"en") {
        return Lang::En;
    }
    if (setting == L"tr") {
        return Lang::Tr;
    }
    if (setting == L"fr") {
        return Lang::Fr;
    }
    // "auto": follow the Windows display language.
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_TURKISH:
            return Lang::Tr;
        case LANG_FRENCH:
            return Lang::Fr;
        default:
            return Lang::En;
    }
}

static BadgeStyle MakeTheme(const std::wstring& theme) {
    BadgeStyle s = {};
    s.bg = s.bg2 = RGB(198, 40, 40);
    s.fg = RGB(255, 255, 255);
    s.border = 0;
    s.borderW = 0;
    s.radius = 3;
    s.fontPx = 14;
    s.bold = true;
    s.squareIcon = false;
    s.usePinnedLabel = false;
    s.prefix = L"\u25B2 ";
    s.font = L"Segoe UI";

    if (theme == L"minimal") {
        s.bg = s.bg2 = RGB(45, 45, 48);
        s.fg = RGB(235, 235, 235);
        s.border = RGB(100, 100, 105);
        s.borderW = 1;
        s.bold = false;
        s.fontPx = 13;
        s.prefix = L"";
    } else if (theme == L"light") {
        s.bg = s.bg2 = RGB(250, 250, 250);
        s.fg = RGB(32, 32, 36);
        s.border = RGB(176, 176, 184);
        s.borderW = 1;
        s.fontPx = 13;
    } else if (theme == L"carbon") {
        s.bg = RGB(20, 20, 22);
        s.bg2 = RGB(48, 48, 52);
        s.fg = RGB(255, 193, 7);
        s.border = RGB(255, 193, 7);
        s.borderW = 1;
        s.radius = 4;
        s.prefix = L"\u25C6 ";
        s.usePinnedLabel = true;
    } else if (theme == L"gold") {
        s.bg = RGB(255, 224, 102);
        s.bg2 = RGB(255, 167, 38);
        s.fg = RGB(62, 39, 0);
        s.border = RGB(184, 134, 11);
        s.borderW = 1;
        s.radius = -1;
        s.prefix = L"\u2605 ";
        s.usePinnedLabel = true;
    } else if (theme == L"neon") {
        s.bg = s.bg2 = RGB(8, 8, 8);
        s.fg = RGB(57, 255, 20);
        s.border = RGB(57, 255, 20);
        s.borderW = 2;
        s.radius = 4;
        s.font = L"Consolas";
        s.prefix = L"\u25CF ";
    } else if (theme == L"cyber") {
        s.bg = RGB(255, 0, 128);
        s.bg2 = RGB(124, 58, 237);
        s.border = RGB(0, 229, 255);
        s.borderW = 1;
        s.radius = 2;
        s.font = L"Consolas";
        s.prefix = L"\u00BB ";
    } else if (theme == L"ocean") {
        s.bg = RGB(13, 71, 161);
        s.bg2 = RGB(0, 188, 212);
        s.radius = -1;
    } else if (theme == L"aurora") {
        s.bg = RGB(106, 17, 203);
        s.bg2 = RGB(37, 117, 252);
        s.radius = -1;
        s.prefix = L"\u25CF ";
        s.usePinnedLabel = true;
    } else if (theme == L"sunset") {
        s.bg = RGB(255, 152, 0);
        s.bg2 = RGB(233, 30, 99);
        s.radius = -1;
    } else if (theme == L"emerald") {
        s.bg = RGB(17, 153, 142);
        s.bg2 = RGB(56, 239, 125);
        s.fg = RGB(4, 48, 30);
        s.radius = -1;
    } else if (theme == L"icon") {
        s.radius = -1;
        s.squareIcon = true;
    }
    // "classic" and "custom" keep the defaults above.
    return s;
}

static void LoadSettings() {
    UINT dpi = GetDpiForSystem();
    g_scale = dpi ? dpi / 96.0 : 1.0;

    // Sound
    g_snd.enabled = Wh_GetIntSetting(L"sound.enabled") != 0;
    g_snd.onPin = Wh_GetIntSetting(L"sound.onPin") != 0;
    g_snd.onUnpin = Wh_GetIntSetting(L"sound.onUnpin") != 0;
    std::wstring type = GetStr(L"sound.type");
    if (type == L"asterisk") {
        g_snd.type = SoundType::Asterisk;
    } else if (type == L"exclamation") {
        g_snd.type = SoundType::Exclamation;
    } else if (type == L"custom") {
        g_snd.type = SoundType::Custom;
    } else {
        g_snd.type = SoundType::Chime;
    }
    g_snd.pinFile = GetStr(L"sound.pinFile");
    g_snd.unpinFile = GetStr(L"sound.unpinFile");

    // Badge style
    std::wstring theme = GetStr(L"badge.theme");
    g_style = MakeTheme(theme);
    if (theme == L"custom") {
        g_style.bg = ParseHex(GetStr(L"badge.customBg"), RGB(198, 40, 40));
        g_style.bg2 = ParseHex(GetStr(L"badge.customBg2"), g_style.bg);
        g_style.fg = ParseHex(GetStr(L"badge.customFg"), RGB(255, 255, 255));
    }

    // Badge text (localized)
    Lang lang = ResolveLanguage(GetStr(L"language"));
    const LangStrings& ls = kLangStrings[static_cast<int>(lang)];
    std::wstring customText = GetStr(L"badge.customText");
    if (!customText.empty()) {
        g_text = customText;
        g_style.squareIcon = false;
    } else if (g_style.squareIcon) {
        g_text = L"\u25B2";
    } else {
        g_text = std::wstring(g_style.prefix) +
                 (g_style.usePinnedLabel ? ls.pinned : ls.onTop);
    }

    // Position and opacity
    std::wstring pos = GetStr(L"badge.position");
    if (pos == L"titleCenter") {
        g_pos = BadgePos::TitleCenter;
    } else if (pos == L"bottomRight") {
        g_pos = BadgePos::BottomRight;
    } else if (pos == L"bottomLeft") {
        g_pos = BadgePos::BottomLeft;
    } else {
        g_pos = BadgePos::TitleRight;
    }

    int opacity = Wh_GetIntSetting(L"badge.opacity");
    if (opacity < 40) {
        opacity = 40;
    }
    if (opacity > 100) {
        opacity = 100;
    }
    g_alpha = static_cast<BYTE>(opacity * 255 / 100);

    // Measure the badge
    int fontPx = static_cast<int>(g_style.fontPx * g_scale + 0.5);
    HDC screen = GetDC(nullptr);
    HFONT font = CreateFontW(-fontPx, 0, 0, 0, g_style.bold ? FW_BOLD : FW_NORMAL,
                             FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                             g_style.font);
    HGDIOBJ oldFont = SelectObject(screen, font);
    SIZE size = {0, 0};
    GetTextExtentPoint32W(screen, g_text.c_str(), static_cast<int>(g_text.size()),
                          &size);
    SelectObject(screen, oldFont);
    DeleteObject(font);
    ReleaseDC(nullptr, screen);

    g_badgeH = fontPx + static_cast<int>(10 * g_scale + 0.5);
    g_badgeW = g_style.squareIcon
                   ? g_badgeH
                   : size.cx + static_cast<int>(20 * g_scale + 0.5);
    g_borderW = static_cast<int>(g_style.borderW * g_scale + 0.5);
    g_badgeRadius = (g_style.radius < 0)
                        ? g_badgeH / 2
                        : static_cast<int>(g_style.radius * g_scale + 0.5);
}

// ---------------------------------------------------------------------------
// Sound
// ---------------------------------------------------------------------------

static DWORD WINAPI ToneThread(LPVOID param) {
    bool pin = param != nullptr;
    if (pin) {
        Beep(880, 70);
        Beep(1320, 90);
    } else {
        Beep(1320, 70);
        Beep(880, 90);
    }
    return 0;
}

static void PlayChime(bool pin) {
    HANDLE h = CreateThread(nullptr, 0, ToneThread,
                            pin ? reinterpret_cast<LPVOID>(1) : nullptr, 0,
                            nullptr);
    if (h) {
        CloseHandle(h);
    }
}

static void PlayAlert(bool pin) {
    if (!g_snd.enabled || (pin && !g_snd.onPin) || (!pin && !g_snd.onUnpin)) {
        return;
    }
    switch (g_snd.type) {
        case SoundType::Asterisk:
            MessageBeep(pin ? MB_ICONASTERISK : MB_OK);
            break;
        case SoundType::Exclamation:
            MessageBeep(pin ? MB_ICONEXCLAMATION : MB_ICONASTERISK);
            break;
        case SoundType::Custom: {
            const std::wstring& file = pin ? g_snd.pinFile : g_snd.unpinFile;
            if (!file.empty() &&
                GetFileAttributesW(file.c_str()) != INVALID_FILE_ATTRIBUTES) {
                PlaySoundW(file.c_str(), nullptr,
                           SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            } else {
                PlayChime(pin);
            }
            break;
        }
        case SoundType::Chime:
        default:
            PlayChime(pin);
            break;
    }
}

// ---------------------------------------------------------------------------
// Badge window
// ---------------------------------------------------------------------------

static void FillSolid(HDC dc, const RECT& rc, COLORREF color) {
    SetDCBrushColor(dc, color);
    FillRect(dc, &rc, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

static void FillGradientH(HDC dc, const RECT& rc, COLORREF c1, COLORREF c2) {
    int w = rc.right - rc.left;
    if (c1 == c2 || w <= 1) {
        FillSolid(dc, rc, c1);
        return;
    }
    for (int i = 0; i < w; ++i) {
        int r = GetRValue(c1) + (GetRValue(c2) - GetRValue(c1)) * i / (w - 1);
        int g = GetGValue(c1) + (GetGValue(c2) - GetGValue(c1)) * i / (w - 1);
        int b = GetBValue(c1) + (GetBValue(c2) - GetBValue(c1)) * i / (w - 1);
        RECT line = {rc.left + i, rc.top, rc.left + i + 1, rc.bottom};
        FillSolid(dc, line, RGB(r, g, b));
    }
}

static LRESULT CALLBACK BadgeProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            if (g_borderW > 0) {
                FillSolid(dc, rc, g_style.border);

                RECT inner = rc;
                InflateRect(&inner, -g_borderW, -g_borderW);
                int r = g_badgeRadius * 2 - g_borderW * 2;
                if (r < 0) {
                    r = 0;
                }
                HRGN rgn = CreateRoundRectRgn(inner.left, inner.top,
                                              inner.right + 1, inner.bottom + 1,
                                              r, r);
                SelectClipRgn(dc, rgn);
                FillGradientH(dc, inner, g_style.bg, g_style.bg2);
                SelectClipRgn(dc, nullptr);
                DeleteObject(rgn);
            } else {
                FillGradientH(dc, rc, g_style.bg, g_style.bg2);
            }

            int fontPx = static_cast<int>(g_style.fontPx * g_scale + 0.5);
            HFONT font = CreateFontW(
                -fontPx, 0, 0, 0, g_style.bold ? FW_BOLD : FW_NORMAL, FALSE,
                FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, g_style.font);
            HGDIOBJ oldFont = SelectObject(dc, font);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, g_style.fg);
            DrawTextW(dc, g_text.c_str(), -1, &rc,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(dc, oldFont);
            DeleteObject(font);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static HWND CreateBadge() {
    HWND badge = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kClassName, L"", WS_POPUP, 0, 0, g_badgeW, g_badgeH, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!badge) {
        return nullptr;
    }

    SetLayeredWindowAttributes(badge, 0, g_alpha, LWA_ALPHA);
    if (g_badgeRadius > 0) {
        HRGN rgn = CreateRoundRectRgn(0, 0, g_badgeW + 1, g_badgeH + 1,
                                      g_badgeRadius * 2, g_badgeRadius * 2);
        // On success the system owns the region.
        if (!SetWindowRgn(badge, rgn, FALSE)) {
            DeleteObject(rgn);
        }
    }
    return badge;
}

static void SetTopmost(HWND hwnd, bool topmost) {
    SetWindowPos(hwnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void UpdateBadges() {
    for (auto it = g_pins.begin(); it != g_pins.end();) {
        HWND target = it->first;
        HWND& badge = it->second;

        // The window was closed: remove the badge.
        if (!IsWindow(target)) {
            if (badge) {
                DestroyWindow(badge);
            }
            it = g_pins.erase(it);
            continue;
        }

        if (!badge) {
            badge = CreateBadge();
            if (!badge) {
                ++it;
                continue;
            }
        }

        // Minimized or hidden: hide the badge.
        if (IsIconic(target) || !IsWindowVisible(target)) {
            ShowWindow(badge, SW_HIDE);
            ++it;
            continue;
        }

        // Re-apply "topmost" if the application cleared it.
        if (!(GetWindowLongW(target, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
            SetTopmost(target, true);
        }

        RECT r;
        GetWindowRect(target, &r);
        bool zoomed = IsZoomed(target) != 0;
        int edge = zoomed ? static_cast<int>(8 * g_scale) : 0;
        int captionH = static_cast<int>(31 * g_scale);
        int titleY = r.top + edge + (captionH - g_badgeH) / 2;
        int margin = static_cast<int>(20 * g_scale);

        int x = 0;
        int y = 0;
        switch (g_pos) {
            case BadgePos::TitleCenter:
                x = r.left + ((r.right - r.left) - g_badgeW) / 2;
                y = titleY;
                break;
            case BadgePos::BottomRight:
                x = r.right - g_badgeW - margin - edge;
                y = r.bottom - g_badgeH - margin - edge;
                break;
            case BadgePos::BottomLeft:
                x = r.left + margin + edge;
                y = r.bottom - g_badgeH - margin - edge;
                break;
            case BadgePos::TitleRight:
            default:
                x = r.right - g_badgeW - static_cast<int>(150 * g_scale) - edge;
                y = titleY;
                break;
        }

        // Keep the badge directly above the pinned window.
        UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE;
        HWND insertAfter = HWND_TOPMOST;
        HWND prev = GetWindow(target, GW_HWNDPREV);
        if (prev == badge) {
            flags |= SWP_NOZORDER;
        } else if (prev) {
            insertAfter = prev;
        }
        SetWindowPos(badge, insertAfter, x, y, 0, 0, flags);
        ++it;
    }
}

// ---------------------------------------------------------------------------
// Hotkey handling
// ---------------------------------------------------------------------------

static void TryRegisterHotkey() {
    if (g_hotkeyRegistered) {
        return;
    }
    if (RegisterHotKey(nullptr, kHotkeyId, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT,
                       'T')) {
        g_hotkeyRegistered = true;
        Wh_Log(L"Hotkey registered");
    } else if (!g_hotkeyFailLogged) {
        g_hotkeyFailLogged = true;
        Wh_Log(L"RegisterHotKey failed (Ctrl+Alt+T may be in use): %lu",
               GetLastError());
    }
}

static void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD) {
    UpdateBadges();
    // Retry the hotkey registration every ~2 seconds until it succeeds.
    if (!g_hotkeyRegistered && ++g_tick % 20 == 0) {
        TryRegisterHotkey();
    }
}

static void TogglePin() {
    HWND fg = GetForegroundWindow();
    if (!fg) {
        return;
    }
    HWND root = GetAncestor(fg, GA_ROOT);
    if (!root) {
        root = fg;
    }

    wchar_t cls[64] = {};
    GetClassNameW(root, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Shell_TrayWnd") ||
        !wcscmp(cls, L"Shell_SecondaryTrayWnd") || !wcscmp(cls, L"Progman") ||
        !wcscmp(cls, L"WorkerW") || !wcscmp(cls, kClassName)) {
        return;
    }

    auto it = g_pins.find(root);
    if (it != g_pins.end()) {
        SetTopmost(root, false);
        if (it->second) {
            DestroyWindow(it->second);
        }
        g_pins.erase(it);
        PlayAlert(false);
        Wh_Log(L"Unpinned %p", root);
    } else {
        HWND badge = CreateBadge();
        if (!badge) {
            Wh_Log(L"Badge creation failed: %lu", GetLastError());
            return;
        }
        SetTopmost(root, true);
        g_pins[root] = badge;
        UpdateBadges();
        PlayAlert(true);
        Wh_Log(L"Pinned %p", root);
    }
}

static void ReloadSettingsAndBadges() {
    LoadSettings();
    for (auto& pin : g_pins) {
        if (pin.second) {
            DestroyWindow(pin.second);
        }
        pin.second = CreateBadge();
    }
    UpdateBadges();
}

static DWORD WINAPI HotkeyThread(LPVOID) {
    LoadSettings();

    WNDCLASSW wc = {};
    wc.lpfnWndProc = BadgeProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    // Create the thread's message queue, then signal that it is ready.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    TryRegisterHotkey();
    UINT_PTR timer = SetTimer(nullptr, 0, 100, TimerProc);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
            TogglePin();
        } else if (msg.message == WM_APP_RELOAD) {
            ReloadSettingsAndBadges();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (timer) {
        KillTimer(nullptr, timer);
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }

    // Restore every pinned window and remove the badges.
    for (auto& pin : g_pins) {
        if (IsWindow(pin.first)) {
            SetTopmost(pin.first, false);
        }
        if (pin.second) {
            DestroyWindow(pin.second);
        }
    }
    g_pins.clear();

    UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init %s", WH_MOD_VERSION);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, 3000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    // The badge windows belong to the hotkey thread, so the reload is
    // forwarded to it.
    if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}
