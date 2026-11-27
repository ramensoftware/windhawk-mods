// ==WindhawkMod==
// @id              minimal-pattern-wallpapers
// @name            Minimal Pattern Wallpapers
// @name:tr-TR      Sade Desenli Duvar Kağıtları
// @name:ru-RU      Минималистичные обои с узорами
// @name:it-IT      Sfondi minimalisti con motivi
// @name:de-DE      Minimalistische Musterhintergründe
// @description     Creates minimal pattern wallpapers from lines, shapes and gradients in a dark Mica-styled editor with live preview. After the mod is enabled, it sits in the system tray.
// @description:tr-TR Çizgi, şekil ve renk geçişlerinden sade desenli duvar kağıtları üretir; koyu, Mica görünümlü ve canlı önizlemeli bir düzenleyicisi vardır. Mod aktif edildikten sonra sistem tepsisinde yer alır.
// @description:ru-RU Создаёт минималистичные обои с узорами из линий, фигур и градиентов в тёмном редакторе в стиле Mica с живым предпросмотром. После включения мод находится в системном трее.
// @description:it-IT Crea sfondi minimalisti con motivi di linee, forme e sfumature in un editor scuro in stile Mica con anteprima dal vivo. Dopo l'attivazione, la mod resta nell'area di notifica (system tray).
// @description:de-DE Erstellt minimalistische Musterhintergründe aus Linien, Formen und Farbverläufen in einem dunklen Editor im Mica-Stil mit Live-Vorschau. Nach der Aktivierung befindet sich das Mod im Infobereich der Taskleiste (System Tray).
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lcomctl32 -lcomdlg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minimal Pattern Wallpapers

**After the mod is enabled, it sits in the system tray (notification area).**
**It does not open a window by itself.** Click its tray icon to open the editor.

Creates brand-new wallpapers from simple lines, patterns and gradients, and
applies them as your desktop background. It does not use ready-made images:
every wallpaper is drawn by the mod from the settings you choose, by default in
the style of dark "carbon" stripes with a soft light spot.

All customization is done in a separate dark editor window with a live preview,
which is easier than the Windhawk settings page.

## Screenshots

![Minimal Pattern Wallpapers - editor](https://i.imgur.com/dgs0DRV.png)

![Minimal Pattern Wallpapers - presets and patterns](https://i.imgur.com/eNAopAE.png)

![Minimal Pattern Wallpapers - result](https://i.imgur.com/zhyztYP.jpeg)

## How to open the editor

- When Windows or Windhawk starts, the mod only adds its icon to the system tray
  and shows a small notification that it is running (this can be turned off in
  the mod settings). If the icon is hidden, look in the "hidden icons" arrow of
  the taskbar.
- Click the tray icon to open the editor.
- Right-click the tray icon for more options, including "Randomize and apply"
  for a quick new wallpaper.
- If you prefer the editor to open on start, turn on "Open the editor window
  when the mod starts" in the mod settings.

## What you can change

- **Preset**: Ready-made starting points (Carbon Dark, Midnight Blue, Crimson,
  Emerald, Violet, Graphite Light, Carbon Weave, Slate Lines, Crosshatch,
  Honeycomb, Ember Dots, Teal Ripples, Dusk Waves, Soft Checker, Facets,
  Brick Wall, Plus Grid, Gold Chevron, Spiral, Sunburst, Bubbles, Cubes,
  Brushed Steel).
- **Pattern**: Diagonal lines, crosshatch, horizontal lines, vertical lines,
  triangles, honeycomb, carbon fiber weave, dot grid, checkerboard, waves,
  concentric rings, brick wall, plus signs, chevron, spiral, sunburst, bubbles,
  cubes, brushed metal, or no lines (gradient only). Rings, spiral and sunburst
  are centered on the light spot.
- **Angle, spacing, thickness, line opacity and line color**: The look of the
  pattern.
- **Center color and edge color**: The background gradient.
- **Light X / Y / size**: Position and size of the light spot.
- **Vignette**: Darkens the corners of the picture.
- **Grain**: Adds fine noise.
- **Resolution**: Your screen size (automatic) or a fixed size.
- **Randomize**: Makes a new random wallpaper.
- **Apply as wallpaper**: Draws the full-size picture and sets it as your wallpaper.
- **Save as BMP...**: Saves the full-size picture to a file.
- **Restore original**: Brings back the wallpaper you had before the first time
  you applied one.

## Preview

Scroll the mouse wheel over the preview to zoom in and out (the zoom follows the
mouse pointer), drag with the left mouse button to move around, and
double-click to jump between "fit" and real pixels (1:1). Fine lines are hard to
see in a small preview, so zoom in to see the real detail.

## Appearance

The editor always uses a dark theme. On Windows 11 (22H2 or newer) it has a
Mica background, cards, and your Windows accent color. On other systems it falls
back to a solid dark background. The backdrop material (Mica, Mica Alt, Acrylic
or none) can be chosen in the mod settings.

## Notes

- Your settings in the editor are remembered between sessions.
- Generated pictures are kept in %LOCALAPPDATA%\MinimalPatternWallpapers. Only the
  latest one is kept. The wallpaper stays in place if you disable the mod.
- The wallpaper is applied with the "Fill" fit style.
- The editor is available in English, Turkish, Russian, Italian and German. By
  default the language follows your Windows display language ("Automatic");
  you can also choose it in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openOnStart: false
  $name: Open the editor window when the mod starts
  $name:tr-TR: Mod başlarken düzenleyici pencereyi aç
  $name:ru-RU: Открывать окно редактора при запуске мода
  $name:it-IT: Apri la finestra dell'editor all'avvio della mod
  $name:de-DE: Editorfenster beim Start des Mods öffnen
  $description: Off by default. The mod starts quietly in the notification area; click its icon to open the editor.
  $description:tr-TR: Varsayılan olarak kapalıdır. Mod bildirim alanında sessizce başlar; düzenleyiciyi açmak için simgesine tıklayın.
  $description:ru-RU: По умолчанию выключено. Мод тихо запускается в области уведомлений; щёлкните его значок, чтобы открыть редактор.
  $description:it-IT: Disattivato per impostazione predefinita. La mod si avvia silenziosamente nell'area di notifica; fai clic sulla sua icona per aprire l'editor.
  $description:de-DE: Standardmäßig aus. Das Mod startet leise im Infobereich; klicken Sie auf sein Symbol, um den Editor zu öffnen.
- showBalloon: true
  $name: Show a notification when the mod starts
  $name:tr-TR: Mod başlarken bildirim göster
  $name:ru-RU: Показывать уведомление при запуске мода
  $name:it-IT: Mostra una notifica all'avvio della mod
  $name:de-DE: Beim Start des Mods eine Benachrichtigung anzeigen
  $description: Shows a small balloon that tells you the mod is running in the notification area.
  $description:tr-TR: Modun bildirim alanında çalıştığını belirten küçük bir balon gösterir.
  $description:ru-RU: Показывает небольшое сообщение о том, что мод работает в области уведомлений.
  $description:it-IT: Mostra un piccolo fumetto che indica che la mod è in esecuzione nell'area di notifica.
  $description:de-DE: Zeigt eine kleine Sprechblase an, die mitteilt, dass das Mod im Infobereich läuft.
- backdrop: mica
  $name: Window material
  $name:tr-TR: Pencere malzemesi
  $name:ru-RU: Материал окна
  $name:it-IT: Materiale della finestra
  $name:de-DE: Fenstermaterial
  $description: Needs Windows 11 22H2 or newer. Older systems use a solid background.
  $description:tr-TR: Windows 11 22H2 veya daha yenisi gerekir. Eski sistemlerde düz arka plan kullanılır.
  $description:ru-RU: Требуется Windows 11 22H2 или новее. В старых системах используется сплошной фон.
  $description:it-IT: Richiede Windows 11 22H2 o successivo. I sistemi più vecchi usano uno sfondo a tinta unita.
  $description:de-DE: Erfordert Windows 11 22H2 oder neuer. Ältere Systeme verwenden einen einfarbigen Hintergrund.
  $options:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Acrylic
  - none: None (solid)
  $options:tr-TR:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Akrilik
  - none: Yok (düz)
  $options:ru-RU:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Акрил
  - none: Нет (сплошной)
  $options:it-IT:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Acrilico
  - none: Nessuno (tinta unita)
  $options:de-DE:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Acryl
  - none: Keins (einfarbig)
- language: auto
  $name: Editor language
  $name:tr-TR: Düzenleyici dili
  $name:ru-RU: Язык редактора
  $name:it-IT: Lingua dell'editor
  $name:de-DE: Sprache des Editors
  $options:
  - auto: Automatic (system language)
  - en: English
  - tr: Türkçe
  - ru: Русский
  - it: Italiano
  - de: Deutsch
  $options:tr-TR:
  - auto: Otomatik (sistem dili)
  - en: English
  - tr: Türkçe
  - ru: Русский
  - it: Italiano
  - de: Deutsch
  $options:ru-RU:
  - auto: Автоматически (язык системы)
  - en: English
  - tr: Türkçe
  - ru: Русский
  - it: Italiano
  - de: Deutsch
  $options:it-IT:
  - auto: Automatica (lingua di sistema)
  - en: English
  - tr: Türkçe
  - ru: Русский
  - it: Italiano
  - de: Deutsch
  $options:de-DE:
  - auto: Automatisch (Systemsprache)
  - en: English
  - tr: Türkçe
  - ru: Русский
  - it: Italiano
  - de: Deutsch
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// BEGIN-RENDERER (no Windows dependencies)
// ---------------------------------------------------------------------------

static const float kPi = 3.14159265f;

constexpr uint32_t Rgb(int r, int g, int b) {
    return (uint32_t)(r | (g << 8) | (b << 16));
}
static inline int ColR(uint32_t c) { return (int)(c & 255); }
static inline int ColG(uint32_t c) { return (int)((c >> 8) & 255); }
static inline int ColB(uint32_t c) { return (int)((c >> 16) & 255); }

// The numeric values are stored in the settings, so existing ones never change.
enum Pattern {
    PAT_DIAG = 0,
    PAT_CROSS,
    PAT_HORIZ,
    PAT_VERT,
    PAT_NONE,
    PAT_WEAVE,
    PAT_TRI,
    PAT_HEX,
    PAT_DOTS,
    PAT_CHECK,
    PAT_WAVES,
    PAT_RINGS,
    PAT_BRICK,
    PAT_CROSSES,
    PAT_CHEVRON,
    PAT_SPIRAL,
    PAT_SUNBURST,
    PAT_BUBBLES,
    PAT_CUBES,
    PAT_BRUSH,
    PAT_COUNT,
};

struct PatternInfo {
    int id;
    const wchar_t* en;
    const wchar_t* tr;
};

// Order shown in the editor.
static const PatternInfo kPatterns[] = {
    {PAT_DIAG, L"Diagonal lines", L"Çapraz çizgiler"},
    {PAT_CROSS, L"Crosshatch", L"Çapraz ızgara"},
    {PAT_HORIZ, L"Horizontal lines", L"Yatay çizgiler"},
    {PAT_VERT, L"Vertical lines", L"Dikey çizgiler"},
    {PAT_TRI, L"Triangles", L"Üçgenler"},
    {PAT_HEX, L"Honeycomb", L"Petek"},
    {PAT_WEAVE, L"Carbon fiber weave", L"Karbon fiber dokuma"},
    {PAT_DOTS, L"Dot grid", L"Nokta ızgarası"},
    {PAT_CHECK, L"Checkerboard", L"Dama tahtası"},
    {PAT_WAVES, L"Waves", L"Dalgalar"},
    {PAT_RINGS, L"Concentric rings", L"Eş merkezli halkalar"},
    {PAT_BRICK, L"Brick wall", L"Tuğla duvar"},
    {PAT_CROSSES, L"Plus signs", L"Artı işaretleri"},
    {PAT_CHEVRON, L"Chevron", L"Şevron"},
    {PAT_SPIRAL, L"Spiral", L"Spiral"},
    {PAT_SUNBURST, L"Sunburst", L"Güneş ışınları"},
    {PAT_BUBBLES, L"Bubbles", L"Kabarcıklar"},
    {PAT_CUBES, L"Cubes", L"Küpler"},
    {PAT_BRUSH, L"Brushed metal", L"Fırçalanmış metal"},
    {PAT_NONE, L"No lines (gradient only)", L"Çizgisiz (sadece renk geçişi)"},
};
static const int kPatternCount = (int)(sizeof(kPatterns) / sizeof(kPatterns[0]));

// Colors use the COLORREF layout (0x00BBGGRR).
struct Params {
    int pattern = PAT_DIAG;
    int angle = 45;         // degrees; 45 = lines running like "/"
    int spacing = 5;        // px, measured on a 1080-pixel-high picture
    int thick10 = 20;       // line thickness in tenths of a pixel
    int lineOpacity = 72;   // percent
    uint32_t lineCol = Rgb(0, 0, 0);
    uint32_t centerCol = Rgb(75, 75, 75);
    uint32_t edgeCol = Rgb(14, 14, 14);
    int lightX = 55;        // percent of the width
    int lightY = 0;         // percent of the height
    int lightSize = 100;    // percent
    int vignette = 5;       // percent
    int grain = 0;          // percent
    int resMode = 0;        // 0 = screen size, 1 = 1080p, 2 = 1440p, 3 = 4K
    int seed = 1;
};

struct Preset {
    const wchar_t* en;
    const wchar_t* tr;
    int pattern, angle, spacing, thick10, opacity;
    uint32_t line, center, edge;
    int lx, ly, ls, vig, grain;
};

static const Preset kPresets[] = {
    {L"Carbon Dark", L"Karbon Koyu", PAT_DIAG, 45, 5, 20, 72, Rgb(0, 0, 0),
     Rgb(75, 75, 75), Rgb(14, 14, 14), 55, 0, 100, 5, 0},
    {L"Midnight Blue", L"Gece Mavisi", PAT_DIAG, 45, 5, 20, 62, Rgb(0, 0, 0),
     Rgb(60, 100, 186), Rgb(7, 13, 33), 55, 0, 100, 5, 0},
    {L"Crimson", L"Kızıl", PAT_DIAG, 45, 5, 20, 62, Rgb(0, 0, 0),
     Rgb(165, 42, 53), Rgb(20, 4, 7), 55, 0, 100, 5, 0},
    {L"Emerald", L"Zümrüt", PAT_DIAG, 45, 5, 20, 62, Rgb(0, 0, 0),
     Rgb(40, 154, 114), Rgb(4, 20, 15), 55, 0, 100, 5, 0},
    {L"Violet", L"Menekşe", PAT_DIAG, 45, 5, 20, 62, Rgb(0, 0, 0),
     Rgb(132, 77, 209), Rgb(11, 6, 24), 55, 0, 100, 5, 0},
    {L"Graphite Light", L"Açık Grafit", PAT_DIAG, 45, 5, 10, 14,
     Rgb(0, 0, 0), Rgb(240, 240, 244), Rgb(170, 172, 180), 50, 0, 120, 15, 2},
    {L"Carbon Weave", L"Karbon Dokuma", PAT_WEAVE, 45, 8, 24, 70,
     Rgb(0, 0, 0), Rgb(86, 86, 92), Rgb(10, 10, 12), 50, 0, 100, 30, 0},
    {L"Slate Lines", L"Arduvaz Çizgileri", PAT_HORIZ, 0, 4, 15, 60,
     Rgb(0, 0, 0), Rgb(84, 98, 112), Rgb(10, 12, 16), 50, 0, 100, 30, 0},
    {L"Crosshatch", L"Çapraz Ağ", PAT_CROSS, 45, 7, 24, 55,
     Rgb(0, 0, 0), Rgb(78, 78, 84), Rgb(10, 10, 12), 50, 0, 100, 30, 0},
    {L"Honeycomb", L"Petek", PAT_HEX, 0, 16, 20, 70,
     Rgb(0, 0, 0), Rgb(72, 96, 128), Rgb(8, 11, 18), 50, 10, 100, 30, 0},
    {L"Ember Dots", L"Kor Noktalar", PAT_DOTS, 0, 8, 28, 70,
     Rgb(0, 0, 0), Rgb(170, 96, 58), Rgb(22, 9, 6), 50, 10, 100, 30, 0},
    {L"Teal Ripples", L"Turkuaz Halkalar", PAT_RINGS, 0, 9, 20, 55,
     Rgb(0, 0, 0), Rgb(44, 130, 156), Rgb(4, 15, 21), 50, 50, 90, 30, 0},
    {L"Dusk Waves", L"Alacakaranlık Dalgaları", PAT_WAVES, 0, 7, 18, 60,
     Rgb(0, 0, 0), Rgb(128, 72, 140), Rgb(12, 6, 18), 50, 20, 100, 30, 0},
    {L"Soft Checker", L"Yumuşak Dama", PAT_CHECK, 0, 24, 20, 14,
     Rgb(0, 0, 0), Rgb(206, 206, 212), Rgb(140, 142, 150), 50, 30, 120, 15, 2},
    {L"Facets", L"Yüzeyler", PAT_TRI, 0, 16, 12, 55,
     Rgb(0, 0, 0), Rgb(70, 118, 92), Rgb(6, 15, 11), 50, 0, 100, 30, 0},
    {L"Brick Wall", L"Tuğla Duvar", PAT_BRICK, 0, 14, 14, 65,
     Rgb(0, 0, 0), Rgb(150, 84, 66), Rgb(22, 10, 8), 50, 30, 110, 35, 3},
    {L"Plus Grid", L"Artı Izgarası", PAT_CROSSES, 0, 16, 14, 55,
     Rgb(0, 0, 0), Rgb(74, 84, 118), Rgb(8, 10, 18), 50, 20, 100, 30, 0},
    {L"Gold Chevron", L"Altın Şevron", PAT_CHEVRON, 0, 8, 16, 60,
     Rgb(0, 0, 0), Rgb(176, 142, 66), Rgb(24, 16, 6), 50, 0, 100, 30, 0},
    {L"Spiral", L"Spiral", PAT_SPIRAL, 0, 8, 14, 55,
     Rgb(0, 0, 0), Rgb(150, 60, 110), Rgb(16, 4, 12), 50, 50, 100, 30, 0},
    {L"Sunburst", L"Güneş Işınları", PAT_SUNBURST, 0, 10, 14, 55,
     Rgb(0, 0, 0), Rgb(190, 140, 50), Rgb(20, 12, 4), 50, 50, 100, 30, 0},
    {L"Bubbles", L"Kabarcıklar", PAT_BUBBLES, 0, 4, 12, 60,
     Rgb(0, 0, 0), Rgb(60, 110, 150), Rgb(6, 12, 20), 50, 20, 100, 30, 0},
    {L"Cubes", L"Küpler", PAT_CUBES, 0, 20, 14, 60,
     Rgb(0, 0, 0), Rgb(112, 112, 124), Rgb(10, 10, 12), 50, 0, 100, 30, 0},
    {L"Brushed Steel", L"Fırçalanmış Çelik", PAT_BRUSH, 0, 2, 10, 30,
     Rgb(0, 0, 0), Rgb(150, 152, 158), Rgb(50, 52, 56), 50, 20, 100, 25, 2},
};
static const int kPresetCount = (int)(sizeof(kPresets) / sizeof(kPresets[0]));

// Applies a preset but keeps the resolution and seed.
static void ApplyPreset(Params& p, int i) {
    if (i < 0 || i >= kPresetCount) return;
    const Preset& s = kPresets[i];
    p.pattern = s.pattern;
    p.angle = s.angle;
    p.spacing = s.spacing;
    p.thick10 = s.thick10;
    p.lineOpacity = s.opacity;
    p.lineCol = s.line;
    p.centerCol = s.center;
    p.edgeCol = s.edge;
    p.lightX = s.lx;
    p.lightY = s.ly;
    p.lightSize = s.ls;
    p.vignette = s.vig;
    p.grain = s.grain;
}

static uint32_t HsvToCol(float h, float s, float v) {
    h = h - floorf(h);
    float r = fminf(1.0f, fmaxf(0.0f, fabsf(h * 6 - 3) - 1));
    float g = fminf(1.0f, fmaxf(0.0f, 2 - fabsf(h * 6 - 2)));
    float b = fminf(1.0f, fmaxf(0.0f, 2 - fabsf(h * 6 - 4)));
    r = 1 + (r - 1) * s;
    g = 1 + (g - 1) * s;
    b = 1 + (b - 1) * s;
    return Rgb((int)(r * v * 255 + 0.5f), (int)(g * v * 255 + 0.5f),
               (int)(b * v * 255 + 0.5f));
}

static float RandF() { return (float)rand() / (float)RAND_MAX; }
static int RandI(int a, int b) { return a + rand() % (b - a + 1); }

// Makes a new random wallpaper. Keeps the resolution.
static void Randomize(Params& p) {
    static const struct {
        int pat;
        int weight;
    } kWeights[] = {
        {PAT_DIAG, 17},  {PAT_CROSS, 6},   {PAT_HORIZ, 5},  {PAT_VERT, 5},
        {PAT_TRI, 5},    {PAT_HEX, 7},     {PAT_WEAVE, 10}, {PAT_DOTS, 6},
        {PAT_CHECK, 2},  {PAT_WAVES, 6},   {PAT_RINGS, 4},  {PAT_BRICK, 4},
        {PAT_CROSSES, 3}, {PAT_CHEVRON, 4}, {PAT_SPIRAL, 3}, {PAT_SUNBURST, 3},
        {PAT_BUBBLES, 3}, {PAT_CUBES, 4},  {PAT_BRUSH, 3},
    };
    int roll = RandI(0, 99), acc = 0;
    p.pattern = PAT_DIAG;
    for (const auto& w : kWeights) {
        acc += w.weight;
        if (roll < acc) {
            p.pattern = w.pat;
            break;
        }
    }

    p.angle = RandI(0, 179);
    switch (p.pattern) {
        case PAT_HEX:
            p.spacing = RandI(8, 22);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_WEAVE:
            p.spacing = RandI(5, 14);
            p.thick10 = RandI(16, 40);
            break;
        case PAT_DOTS:
            p.spacing = RandI(4, 12);
            p.thick10 = RandI(15, 40);
            break;
        case PAT_CHECK:
            p.spacing = RandI(10, 36);
            p.thick10 = 20;
            break;
        case PAT_WAVES:
            p.spacing = RandI(4, 10);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_RINGS:
            p.spacing = RandI(5, 14);
            p.thick10 = RandI(10, 28);
            break;
        case PAT_TRI:
            p.spacing = RandI(8, 20);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_BRICK:
            p.spacing = RandI(8, 20);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_CROSSES:
            p.spacing = RandI(10, 24);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_CHEVRON:
        case PAT_SPIRAL:
            p.spacing = RandI(5, 12);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_SUNBURST:
            p.spacing = RandI(6, 20);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_BUBBLES:
            p.spacing = RandI(3, 6);
            p.thick10 = RandI(8, 16);
            break;
        case PAT_CUBES:
            p.spacing = RandI(12, 30);
            p.thick10 = RandI(10, 24);
            break;
        case PAT_BRUSH:
            p.spacing = RandI(2, 4);
            p.thick10 = RandI(8, 14);
            break;
        default:
            p.spacing = RandI(3, 12);
            p.thick10 = RandI(8, 30);
            break;
    }

    float hue = RandF();
    bool tinted = RandI(0, 2) != 0;
    if (tinted) {
        p.centerCol = HsvToCol(hue, 0.35f + RandF() * 0.5f, 0.30f + RandF() * 0.45f);
        p.edgeCol = HsvToCol(hue, 0.35f + RandF() * 0.5f, 0.02f + RandF() * 0.08f);
    } else {
        int c = RandI(45, 95);
        p.centerCol = Rgb(c, c, c + RandI(0, 8));
        int e = RandI(4, 16);
        p.edgeCol = Rgb(e, e, e);
    }
    if (RandI(0, 4) == 0) {
        p.lineCol = Rgb(255, 255, 255);
        p.lineOpacity = RandI(4, 14);
    } else {
        p.lineCol = Rgb(0, 0, 0);
        p.lineOpacity = RandI(35, 80);
        if (p.pattern == PAT_CHECK) p.lineOpacity = RandI(12, 40);
        if (p.pattern == PAT_BRUSH) p.lineOpacity = RandI(15, 45);
    }
    p.lightX = RandI(25, 75);
    p.lightY = RandI(0, 35);
    if (p.pattern == PAT_RINGS || p.pattern == PAT_SPIRAL ||
        p.pattern == PAT_SUNBURST) {
        p.lightX = RandI(30, 70);
        p.lightY = RandI(30, 70);
    }
    p.lightSize = RandI(70, 140);
    p.vignette = RandI(15, 60);
    p.grain = RandI(0, 8);
    p.seed = rand();
}

static inline float Clamp01f(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

// Coverage (0..1) of a repeating line along the coordinate u.
static inline float LineCov(float u, float period, float thick) {
    float m = fmodf(u, period);
    if (m < 0) m += period;
    float d = fabsf(m - period * 0.5f);
    return Clamp01f(thick * 0.5f - d + 0.5f);
}

// Position inside a repeating cell, measured from the cell center.
static inline float Centered(float u, float period) {
    float m = fmodf(u, period);
    if (m < 0) m += period;
    return m - period * 0.5f;
}

// Smooth square wave: 1 and 0 blocks of the given length each (period 2 * len).
static inline float SqWave(float u, float len) {
    float m = fmodf(u, 2 * len);
    if (m < 0) m += 2 * len;
    float t = fabsf(m - len);
    return Clamp01f(t - len * 0.5f + 0.5f);
}

// Honeycomb cell lookup for (u, v). Cells are `size` pixels wide (flat to flat).
// edge: distance in pixels to the nearest cell edge (positive inside).
// lx, ly: offset in pixels from the cell center.
struct HexHit {
    float edge, lx, ly;
};

static inline HexHit HexCell(float u, float v, float size) {
    const float sy = 1.7320508f;
    const float x = u / size, y = v / size;
    const float ax = floorf(x) + 0.5f, ay = floorf(y / sy) + 0.5f;
    const float bx = floorf(x - 0.5f) + 0.5f, by = floorf((y - 1.0f) / sy) + 0.5f;
    const float h1x = x - ax, h1y = y - ay * sy;
    const float h2x = x - (bx + 0.5f), h2y = y - (by + 0.5f) * sy;
    float px, py;
    if (h1x * h1x + h1y * h1y < h2x * h2x + h2y * h2y) {
        px = h1x;
        py = h1y;
    } else {
        px = h2x;
        py = h2y;
    }
    const float qx = fabsf(px), qy = fabsf(py);
    const float c = fmaxf(qx * 0.5f + qy * 0.8660254f, qx);
    HexHit r;
    r.edge = (0.5f - c) * size;
    r.lx = px * size;
    r.ly = py * size;
    return r;
}

// Values that stay the same for the whole picture.
struct RenderCtx {
    int pattern, W, H;
    float period, thick, lop;
    float cr, cg, cb, er, eg, eb, lr, lg, lb;
    float n1x, n1y, n2x, n2y, n3x, n3y, n4x, n4y;
    float lx, ly, rad, vig, grainAmp, a1, rayStep;
    uint32_t seed;
};

static RenderCtx MakeCtx(const Params& p, int W, int H) {
    RenderCtx c;
    const float scale = (float)H / 1080.0f;
    c.pattern = p.pattern;
    c.W = W;
    c.H = H;
    c.period = fmaxf(1.5f, (float)p.spacing * scale);
    c.thick = fmaxf(0.3f, (float)p.thick10 / 10.0f * scale);
    c.lop = Clamp01f((float)p.lineOpacity / 100.0f);

    c.cr = (float)ColR(p.centerCol);
    c.cg = (float)ColG(p.centerCol);
    c.cb = (float)ColB(p.centerCol);
    c.er = (float)ColR(p.edgeCol);
    c.eg = (float)ColG(p.edgeCol);
    c.eb = (float)ColB(p.edgeCol);
    c.lr = (float)ColR(p.lineCol);
    c.lg = (float)ColG(p.lineCol);
    c.lb = (float)ColB(p.lineCol);

    const float a1 = (float)p.angle * kPi / 180.0f;
    c.a1 = a1;
    c.n1x = sinf(a1);
    c.n1y = cosf(a1);
    c.n2x = sinf(a1 + kPi / 2);
    c.n2y = cosf(a1 + kPi / 2);
    c.n3x = sinf(a1 + kPi / 3);
    c.n3y = cosf(a1 + kPi / 3);
    c.n4x = sinf(a1 + 2 * kPi / 3);
    c.n4y = cosf(a1 + 2 * kPi / 3);

    c.lx = (float)p.lightX / 100.0f * (float)W;
    c.ly = (float)p.lightY / 100.0f * (float)H;
    const float diag = sqrtf((float)W * (float)W + (float)H * (float)H);
    c.rad = fmaxf(1.0f, (float)p.lightSize / 100.0f * diag * 0.6f);
    c.vig = (float)p.vignette / 100.0f;
    c.grainAmp = (float)p.grain * 0.5f;
    c.seed = (uint32_t)p.seed;
    int rays = 1200 / (p.spacing < 1 ? 1 : p.spacing);
    if (rays < 6) rays = 6;
    c.rayStep = 2 * kPi / (float)rays;
    return c;
}


static inline float Hash01(uint32_t x, uint32_t y, uint32_t seed) {
    uint32_t h = x * 374761393u + y * 668265263u + seed * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (float)(h & 0xFFFF) / 65535.0f;
}

// Coverage (0..1) of the pattern at the picture position (fx, fy).
static inline float PatternCov(const RenderCtx& c, float fx, float fy) {
    // Rotated coordinates: u runs along the lines, v across them. At an angle
    // of 0 they are simply x and y.
    const float u = fx * c.n2x + fy * c.n2y;
    const float v = fx * c.n1x + fy * c.n1y;
    switch (c.pattern) {
        case PAT_DIAG:
            return LineCov(v, c.period, c.thick);
        case PAT_CROSS:
            return fmaxf(LineCov(v, c.period, c.thick),
                         LineCov(u, c.period, c.thick));
        case PAT_HORIZ:
            return LineCov(fy, c.period, c.thick);
        case PAT_VERT:
            return LineCov(fx, c.period, c.thick);
        case PAT_TRI:
            return fmaxf(
                LineCov(v, c.period, c.thick),
                fmaxf(LineCov(fx * c.n3x + fy * c.n3y, c.period, c.thick),
                      LineCov(fx * c.n4x + fy * c.n4y, c.period, c.thick)));
        case PAT_HEX:
            return Clamp01f(c.thick * 0.5f - HexCell(u, v, c.period).edge + 0.5f);
        case PAT_WEAVE: {
            // 2/2 twill: strands run one way over two cells, then the other.
            const float cu = u / c.period, cv = v / c.period;
            const float iu = floorf(cu), iv = floorf(cv);
            const int cell = ((int)iu + (int)iv) & 3;
            const float fu = cu - iu, fv = cv - iv;
            const float across = cell < 2 ? fu : fv;
            const float along = cell < 2 ? fv : fu;
            float d = 1.0f - sinf(kPi * across);          // dark at strand edges
            d += 0.22f * (1.0f - sinf(kPi * along));       // dark where it dives
            const float w = Clamp01f(c.thick / c.period);
            return Clamp01f(d * (0.55f + 1.6f * w));
        }
        case PAT_DOTS: {
            const float du = Centered(u, c.period), dv = Centered(v, c.period);
            return Clamp01f(c.thick * 0.5f - sqrtf(du * du + dv * dv) + 0.5f);
        }
        case PAT_CHECK: {
            const float a = SqWave(u, c.period), b = SqWave(v, c.period);
            return a + b - 2 * a * b;
        }
        case PAT_WAVES: {
            const float amp = c.period * 1.6f;
            const float wl = c.period * 14.0f;
            return LineCov(v + amp * sinf(2 * kPi * u / wl), c.period, c.thick);
        }
        case PAT_RINGS: {
            const float dx = fx - c.lx, dy = fy - c.ly;
            return LineCov(sqrtf(dx * dx + dy * dy), c.period, c.thick);
        }
        case PAT_BRICK: {
            const float P = c.period;
            const int row = (int)floorf((v - P * 0.5f) / P);
            const float shift = (row & 1) ? P : 0.0f;
            return fmaxf(LineCov(v, P, c.thick), LineCov(u + shift, 2 * P, c.thick));
        }
        case PAT_CROSSES: {
            const float du = fabsf(Centered(u, c.period));
            const float dv = fabsf(Centered(v, c.period));
            const float arm = c.period * 0.32f;
            const float a = Clamp01f(c.thick * 0.5f - dv + 0.5f) * Clamp01f(arm - du + 0.5f);
            const float b = Clamp01f(c.thick * 0.5f - du + 0.5f) * Clamp01f(arm - dv + 0.5f);
            return fmaxf(a, b);
        }
        case PAT_CHEVRON:
            return LineCov(v + fabsf(Centered(u, c.period * 8.0f)), c.period, c.thick);
        case PAT_SPIRAL: {
            const float dx = fx - c.lx, dy = fy - c.ly;
            const float r = sqrtf(dx * dx + dy * dy);
            return LineCov(r + c.period * atan2f(dy, dx) / (2 * kPi), c.period, c.thick);
        }
        case PAT_SUNBURST: {
            const float dx = fx - c.lx, dy = fy - c.ly;
            const float r = sqrtf(dx * dx + dy * dy);
            const float off = Centered(atan2f(dy, dx) - c.a1, c.rayStep);
            return Clamp01f(c.thick * 0.5f - r * fabsf(off) + 0.5f);
        }
        case PAT_BUBBLES: {
            const HexHit h = HexCell(u, v, c.period * 8.0f);
            return LineCov(sqrtf(h.lx * h.lx + h.ly * h.ly), c.period, c.thick);
        }
        case PAT_CUBES: {
            // Tumbling blocks: every honeycomb cell is split into three faces.
            const HexHit h = HexCell(u, v, c.period);
            const float r = sqrtf(h.lx * h.lx + h.ly * h.ly);
            float t = fmodf(atan2f(h.ly, h.lx) - kPi * 0.5f + 4 * kPi, 2 * kPi);
            const float seg = 2 * kPi / 3;
            int face = (int)(t / seg);
            if (face > 2) face = 2;
            const float m = t - (float)face * seg;
            const float off = fminf(m, seg - m);
            const float spoke = Clamp01f(c.thick * 0.5f - r * sinf(off) + 0.5f);
            const float edge = Clamp01f(c.thick * 0.5f - h.edge + 0.5f);
            const float shade = face == 0 ? 0.0f : (face == 1 ? 0.30f : 0.60f);
            return fmaxf(fmaxf(spoke, edge), shade);
        }
        case PAT_BRUSH: {
            const uint32_t row = (uint32_t)(int)floorf(v / c.period);
            return LineCov(v, c.period, c.thick) * (0.2f + 0.8f * Hash01(row, 17u, c.seed));
        }
        default:
            return 0.0f;
    }
}

// Color (0..255 floats, not clamped) of the picture at (fx, fy).
// (ix, iy) is the pixel the grain noise belongs to.
static inline void ShadeAt(const RenderCtx& c, float fx, float fy, uint32_t ix,
                           uint32_t iy, float& oR, float& oG, float& oB) {
    // Background: light spot gradient.
    const float dx = fx - c.lx, dy = fy - c.ly;
    float t = Clamp01f(sqrtf(dx * dx + dy * dy) / c.rad);
    t = t * t * (3 - 2 * t);
    float r = c.cr + (c.er - c.cr) * t;
    float g = c.cg + (c.eg - c.cg) * t;
    float b = c.cb + (c.eb - c.cb) * t;

    // Vignette.
    if (c.vig > 0.0f) {
        const float vx = (fx / (float)c.W - 0.5f) * 2;
        const float vy = (fy / (float)c.H - 0.5f) * 2;
        const float vr = (vx * vx + vy * vy) * 0.5f;
        const float k = 1.0f - c.vig * 0.85f * vr;
        r *= k;
        g *= k;
        b *= k;
    }

    // Pattern.
    const float a = PatternCov(c, fx, fy) * c.lop;
    if (a > 0.0f) {
        r += (c.lr - r) * a;
        g += (c.lg - g) * a;
        b += (c.lb - b) * a;
    }

    // Grain.
    if (c.grainAmp > 0.0f) {
        const float n = (Hash01(ix, iy, c.seed) - 0.5f) * c.grainAmp;
        r += n;
        g += n;
        b += n;
    }
    oR = r;
    oG = g;
    oB = b;
}

static inline uint32_t PackRgb(float r, float g, float b) {
    int ir = (int)(r + 0.5f), ig = (int)(g + 0.5f), ib = (int)(b + 0.5f);
    ir = ir < 0 ? 0 : (ir > 255 ? 255 : ir);
    ig = ig < 0 ? 0 : (ig > 255 ? 255 : ig);
    ib = ib < 0 ? 0 : (ib > 255 ? 255 : ib);
    return ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
}

// Renders a w x h view of a W x H picture into out (0x00RRGGBB).
// Output pixel (x, y) shows the picture position (ox + x * step, oy + y * step).
// step = 1 is real pixels, a larger step zooms out. ss x ss samples are
// averaged per output pixel. Positions outside the picture get `outside`.
// Line sizes scale with the picture height, so a picture looks the same at
// every resolution.
static void RenderView(const Params& p, int W, int H, float ox, float oy,
                       float step, int ss, uint32_t outside, int w, int h,
                       uint32_t* out) {
    const RenderCtx c = MakeCtx(p, W, H);
    if (ss < 1) ss = 1;
    const float sub = 1.0f / (float)ss;
    const float inv = 1.0f / (float)(ss * ss);
    const float outR = (float)((outside >> 16) & 255);
    const float outG = (float)((outside >> 8) & 255);
    const float outB = (float)(outside & 255);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float ar = 0, ag = 0, ab = 0;
            for (int sy = 0; sy < ss; sy++) {
                const float fy = oy + ((float)y + ((float)sy + 0.5f) * sub) * step;
                for (int sx = 0; sx < ss; sx++) {
                    const float fx = ox + ((float)x + ((float)sx + 0.5f) * sub) * step;
                    if (fx < 0 || fy < 0 || fx >= (float)W || fy >= (float)H) {
                        ar += outR;
                        ag += outG;
                        ab += outB;
                        continue;
                    }
                    float r, g, b;
                    ShadeAt(c, fx, fy, (uint32_t)fx, (uint32_t)fy, r, g, b);
                    ar += r;
                    ag += g;
                    ab += b;
                }
            }
            out[(size_t)y * w + x] = PackRgb(ar * inv, ag * inv, ab * inv);
        }
    }
}

// Renders the rectangle (x0, y0, w, h) of a W x H picture at real pixel size.
static void RenderRegion(const Params& p, int W, int H, int x0, int y0, int w,
                         int h, uint32_t* out) {
    RenderView(p, W, H, (float)x0, (float)y0, 1.0f, 1, 0, w, h, out);
}

// ---------------------------------------------------------------------------
// END-RENDERER
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Language and settings
// ---------------------------------------------------------------------------

enum UiLang { UL_EN = 0, UL_TR, UL_RU, UL_IT, UL_DE };

static bool g_openOnStart = false;
static bool g_showBalloon = true;
static int g_backdrop = 1;  // 0 = none, 1 = Mica, 2 = Mica Alt, 3 = Acrylic
static int g_lang = UL_EN;

// Translations, looked up by the English text. Turkish is passed to T() directly.
struct Tr3 {
    const wchar_t* en;
    const wchar_t* ru;
    const wchar_t* it;
    const wchar_t* de;
};

static const Tr3 kTr[] = {
    {L"Zoom %d%%   |   Wheel: zoom   Drag: pan", L"Масштаб %d%%   |   Колесо: масштаб   Перетаскивание: сдвиг", L"Zoom %d%%   |   Rotella: zoom   Trascina: sposta", L"Zoom %d%%   |   Mausrad: Zoom   Ziehen: Verschieben"},
    {L"Drawing the wallpaper...", L"Рисование обоев...", L"Creazione dello sfondo...", L"Hintergrund wird erstellt..."},
    {L"Wallpaper applied (%d x %d).", L"Обои установлены (%d x %d).", L"Sfondo applicato (%d x %d).", L"Hintergrund festgelegt (%d x %d)."},
    {L"Could not apply the wallpaper.", L"Не удалось установить обои.", L"Impossibile applicare lo sfondo.", L"Der Hintergrund konnte nicht festgelegt werden."},
    {L"Saved.", L"Сохранено.", L"Salvato.", L"Gespeichert."},
    {L"Could not save the file.", L"Не удалось сохранить файл.", L"Impossibile salvare il file.", L"Die Datei konnte nicht gespeichert werden."},
    {L"Fit", L"Вписать", L"Adatta", L"Einpassen"},
    {L"Resolution", L"Разрешение", L"Risoluzione", L"Auflösung"},
    {L"Screen size (automatic)", L"Размер экрана (авто)", L"Dimensione schermo (auto)", L"Bildschirmgröße (automatisch)"},
    {L"Randomize", L"Случайно", L"Casuale", L"Zufall"},
    {L"Reset", L"Сбросить", L"Ripristina", L"Zurücksetzen"},
    {L"Save as BMP...", L"Сохранить как BMP...", L"Salva come BMP...", L"Als BMP speichern..."},
    {L"Apply as wallpaper", L"Установить как обои", L"Imposta come sfondo", L"Als Hintergrund festlegen"},
    {L"Restore original", L"Вернуть прежние обои", L"Ripristina originale", L"Original wiederherstellen"},
    {L"Preset", L"Пресет", L"Preset", L"Vorlage"},
    {L"Pattern", L"Узор", L"Motivo", L"Muster"},
    {L"Original wallpaper restored.", L"Прежние обои восстановлены.", L"Sfondo originale ripristinato.", L"Originalhintergrund wiederhergestellt."},
    {L"No original wallpaper was saved yet.", L"Прежние обои ещё не сохранялись.", L"Nessuno sfondo originale è stato ancora salvato.", L"Es wurde noch kein Originalhintergrund gespeichert."},
    {L"Minimal Pattern Wallpapers", L"Минималистичные обои с узорами", L"Sfondi minimalisti con motivi", L"Minimalistische Musterhintergründe"},
    {L"Running in the notification area. Click the icon to open the editor.", L"Работает в области уведомлений. Щёлкните значок, чтобы открыть редактор.", L"In esecuzione nell'area di notifica. Fai clic sull'icona per aprire l'editor.", L"Läuft im Infobereich. Klicken Sie auf das Symbol, um den Editor zu öffnen."},
    {L"Open editor", L"Открыть редактор", L"Apri editor", L"Editor öffnen"},
    {L"Randomize and apply", L"Случайные обои и установить", L"Casuale e applica", L"Zufällig erstellen und anwenden"},
    {L"Restore original wallpaper", L"Вернуть прежние обои", L"Ripristina sfondo originale", L"Originalhintergrund wiederherstellen"},
    {L"Diagonal lines", L"Диагональные линии", L"Linee diagonali", L"Diagonale Linien"},
    {L"Crosshatch", L"Штриховка", L"Tratteggio incrociato", L"Kreuzschraffur"},
    {L"Horizontal lines", L"Горизонтальные линии", L"Linee orizzontali", L"Horizontale Linien"},
    {L"Vertical lines", L"Вертикальные линии", L"Linee verticali", L"Vertikale Linien"},
    {L"Triangles", L"Треугольники", L"Triangoli", L"Dreiecke"},
    {L"Honeycomb", L"Соты", L"Nido d'ape", L"Waben"},
    {L"Carbon fiber weave", L"Плетение карбона", L"Trama in fibra di carbonio", L"Carbon-Geflecht"},
    {L"Dot grid", L"Сетка точек", L"Griglia di punti", L"Punktraster"},
    {L"Checkerboard", L"Шахматная доска", L"Scacchiera", L"Schachbrett"},
    {L"Waves", L"Волны", L"Onde", L"Wellen"},
    {L"Concentric rings", L"Концентрические кольца", L"Anelli concentrici", L"Konzentrische Ringe"},
    {L"Brick wall", L"Кирпичная кладка", L"Muro di mattoni", L"Ziegelmauer"},
    {L"Plus signs", L"Знаки плюс", L"Segni più", L"Pluszeichen"},
    {L"Chevron", L"Шевроны", L"Chevron", L"Chevron"},
    {L"Spiral", L"Спираль", L"Spirale", L"Spirale"},
    {L"Sunburst", L"Солнечные лучи", L"Raggi di sole", L"Sonnenstrahlen"},
    {L"Bubbles", L"Пузыри", L"Bolle", L"Blasen"},
    {L"Cubes", L"Кубы", L"Cubi", L"Würfel"},
    {L"Brushed metal", L"Шлифованный металл", L"Metallo spazzolato", L"Gebürstetes Metall"},
    {L"No lines (gradient only)", L"Без линий (только градиент)", L"Nessuna linea (solo sfumatura)", L"Keine Linien (nur Farbverlauf)"},
    {L"Carbon Dark", L"Тёмный карбон", L"Carbonio scuro", L"Dunkles Carbon"},
    {L"Midnight Blue", L"Полуночный синий", L"Blu notte", L"Mitternachtsblau"},
    {L"Crimson", L"Малиновый", L"Cremisi", L"Karmesin"},
    {L"Emerald", L"Изумруд", L"Smeraldo", L"Smaragd"},
    {L"Violet", L"Фиолетовый", L"Viola", L"Violett"},
    {L"Graphite Light", L"Светлый графит", L"Grafite chiaro", L"Helles Graphit"},
    {L"Carbon Weave", L"Карбоновое плетение", L"Trama carbonio", L"Carbon-Gewebe"},
    {L"Slate Lines", L"Сланцевые линии", L"Linee ardesia", L"Schieferlinien"},
    {L"Ember Dots", L"Тлеющие точки", L"Punti di brace", L"Glutpunkte"},
    {L"Teal Ripples", L"Бирюзовая рябь", L"Onde turchesi", L"Türkise Wellen"},
    {L"Dusk Waves", L"Сумеречные волны", L"Onde del tramonto", L"Dämmerungswellen"},
    {L"Soft Checker", L"Мягкая шахматка", L"Scacchiera soft", L"Sanftes Schachbrett"},
    {L"Facets", L"Грани", L"Sfaccettature", L"Facetten"},
    {L"Brick Wall", L"Кирпичная стена", L"Muro di mattoni", L"Ziegelwand"},
    {L"Plus Grid", L"Сетка плюсов", L"Griglia di più", L"Plusraster"},
    {L"Gold Chevron", L"Золотые шевроны", L"Chevron dorato", L"Goldenes Chevron"},
    {L"Brushed Steel", L"Шлифованная сталь", L"Acciaio spazzolato", L"Gebürsteter Stahl"},
    {L"Angle", L"Угол", L"Angolo", L"Winkel"},
    {L"Spacing", L"Шаг", L"Spaziatura", L"Abstand"},
    {L"Thickness", L"Толщина", L"Spessore", L"Dicke"},
    {L"Line opacity", L"Плотность", L"Opacità", L"Deckkraft"},
    {L"Light X", L"Свет X", L"Luce X", L"Licht X"},
    {L"Light Y", L"Свет Y", L"Luce Y", L"Licht Y"},
    {L"Light size", L"Размер света", L"Dim. luce", L"Lichtgröße"},
    {L"Vignette", L"Виньетка", L"Vignettatura", L"Vignette"},
    {L"Grain", L"Зерно", L"Grana", L"Körnung"},
    {L"Line color", L"Цвет линий", L"Colore linee", L"Linienfarbe"},
    {L"Center color", L"Цвет центра", L"Colore centro", L"Mittelfarbe"},
    {L"Edge color", L"Цвет краёв", L"Colore bordi", L"Randfarbe"},
};

// Returns the text in the editor language (English, Turkish, Russian, Italian
// or German). Falls back to English when a translation is missing.
static const wchar_t* T(const wchar_t* en, const wchar_t* tr) {
    if (g_lang == UL_TR) return tr;
    if (g_lang == UL_EN) return en;
    for (const Tr3& t : kTr) {
        if (wcscmp(t.en, en) == 0) {
            return g_lang == UL_RU ? t.ru : (g_lang == UL_IT ? t.it : t.de);
        }
    }
    return en;
}

// Reads the mod settings. Called on the worker thread (and once before it starts).
static void LoadModSettings() {
    g_openOnStart = Wh_GetIntSetting(L"openOnStart") != 0;
    g_showBalloon = Wh_GetIntSetting(L"showBalloon") != 0;

    g_backdrop = 1;
    PCWSTR bd = Wh_GetStringSetting(L"backdrop");
    if (bd) {
        if (!wcscmp(bd, L"none")) g_backdrop = 0;
        else if (!wcscmp(bd, L"micaAlt")) g_backdrop = 2;
        else if (!wcscmp(bd, L"acrylic")) g_backdrop = 3;
        Wh_FreeStringSetting(bd);
    }

    // "auto" follows the Windows display language.
    int lang = UL_EN;
    switch (GetUserDefaultUILanguage() & 0x3ff) {
        case LANG_TURKISH: lang = UL_TR; break;
        case LANG_RUSSIAN: lang = UL_RU; break;
        case LANG_ITALIAN: lang = UL_IT; break;
        case LANG_GERMAN: lang = UL_DE; break;
        default: break;
    }
    PCWSTR set = Wh_GetStringSetting(L"language");
    if (set) {
        if (!wcscmp(set, L"en")) lang = UL_EN;
        else if (!wcscmp(set, L"tr")) lang = UL_TR;
        else if (!wcscmp(set, L"ru")) lang = UL_RU;
        else if (!wcscmp(set, L"it")) lang = UL_IT;
        else if (!wcscmp(set, L"de")) lang = UL_DE;
        Wh_FreeStringSetting(set);
    }
    g_lang = lang;
}

// ---------------------------------------------------------------------------
// Persistent parameters
// ---------------------------------------------------------------------------

static Params g_p;

template <class F>
static void ForEachParam(Params& p, F f) {
    f(L"pattern", (int*)&p.pattern);
    f(L"angle", (int*)&p.angle);
    f(L"spacing", (int*)&p.spacing);
    f(L"thick10", (int*)&p.thick10);
    f(L"lineOpacity", (int*)&p.lineOpacity);
    f(L"lineCol", (int*)&p.lineCol);
    f(L"centerCol", (int*)&p.centerCol);
    f(L"edgeCol", (int*)&p.edgeCol);
    f(L"lightX", (int*)&p.lightX);
    f(L"lightY", (int*)&p.lightY);
    f(L"lightSize", (int*)&p.lightSize);
    f(L"vignette", (int*)&p.vignette);
    f(L"grain", (int*)&p.grain);
    f(L"resMode", (int*)&p.resMode);
    f(L"seed", (int*)&p.seed);
}

static void LoadParams(Params& p) {
    ForEachParam(p, [](const wchar_t* name, int* v) {
        *v = Wh_GetIntValue(name, *v);
    });
    p.pattern = std::clamp(p.pattern, 0, (int)PAT_COUNT - 1);
    p.angle = std::clamp(p.angle, 0, 179);
    p.spacing = std::clamp(p.spacing, 2, 40);
    p.thick10 = std::clamp(p.thick10, 3, 120);
    p.lineOpacity = std::clamp(p.lineOpacity, 0, 100);
    p.lightX = std::clamp(p.lightX, 0, 100);
    p.lightY = std::clamp(p.lightY, 0, 100);
    p.lightSize = std::clamp(p.lightSize, 10, 200);
    p.vignette = std::clamp(p.vignette, 0, 100);
    p.grain = std::clamp(p.grain, 0, 100);
    p.resMode = std::clamp(p.resMode, 0, 3);
}

static void SaveParams(Params& p) {
    ForEachParam(p, [](const wchar_t* name, int* v) {
        Wh_SetIntValue(name, *v);
    });
}

// ---------------------------------------------------------------------------
// Files, wallpaper and registry helpers
// ---------------------------------------------------------------------------

static void GetTargetSize(int& W, int& H) {
    switch (g_p.resMode) {
        case 1: W = 1920; H = 1080; return;
        case 2: W = 2560; H = 1440; return;
        case 3: W = 3840; H = 2160; return;
        default: break;
    }
    W = GetSystemMetrics(SM_CXSCREEN);
    H = GetSystemMetrics(SM_CYSCREEN);
    if (W < 320 || H < 200) {
        W = 1920;
        H = 1080;
    }
}

// Pixel buffer that does not rely on exceptions when memory runs out.
struct PixBuf {
    uint32_t* p;
    explicit PixBuf(size_t n) : p((uint32_t*)malloc(n * sizeof(uint32_t))) {}
    ~PixBuf() { free(p); }
    PixBuf(const PixBuf&) = delete;
    PixBuf& operator=(const PixBuf&) = delete;
};

static bool WriteBmpFile(const std::wstring& path, const uint32_t* px, int W,
                         int H) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;

    const DWORD rowBytes = ((DWORD)W * 3 + 3) & ~3u;
    const DWORD imgSize = rowBytes * (DWORD)H;

    BITMAPFILEHEADER bf = {};
    bf.bfType = 0x4D42;
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bf.bfSize = bf.bfOffBits + imgSize;
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(bi);
    bi.biWidth = W;
    bi.biHeight = H;  // bottom-up
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = imgSize;

    DWORD wr = 0;
    bool ok = WriteFile(f, &bf, sizeof(bf), &wr, nullptr) && wr == sizeof(bf);
    ok = ok && WriteFile(f, &bi, sizeof(bi), &wr, nullptr) && wr == sizeof(bi);

    std::vector<uint8_t> row(rowBytes, 0);
    for (int y = H - 1; y >= 0 && ok; y--) {
        const uint32_t* s = px + (size_t)y * W;
        for (int x = 0; x < W; x++) {
            row[(size_t)x * 3 + 0] = (uint8_t)(s[x] & 255);
            row[(size_t)x * 3 + 1] = (uint8_t)((s[x] >> 8) & 255);
            row[(size_t)x * 3 + 2] = (uint8_t)((s[x] >> 16) & 255);
        }
        ok = WriteFile(f, row.data(), rowBytes, &wr, nullptr) && wr == rowBytes;
    }
    CloseHandle(f);
    if (!ok) DeleteFileW(path.c_str());
    return ok;
}

static bool EnsureDir(std::wstring& dir) {
    WCHAR base[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, ARRAYSIZE(base));
    if (!n || n >= ARRAYSIZE(base)) return false;
    dir = std::wstring(base) + L"\\MinimalPatternWallpapers";
    CreateDirectoryW(dir.c_str(), nullptr);
    return GetFileAttributesW(dir.c_str()) != INVALID_FILE_ATTRIBUTES;
}

static void CleanOldWallpapers(const std::wstring& dir,
                               const std::wstring& keepName) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\wallpaper_*.bmp").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (keepName != fd.cFileName) {
            DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static const wchar_t* kDesktopKey = L"Control Panel\\Desktop";

static void SetWallpaperStyle(const wchar_t* style, const wchar_t* tile) {
    HKEY k = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kDesktopKey, 0, KEY_SET_VALUE, &k) !=
        ERROR_SUCCESS) {
        return;
    }
    RegSetValueExW(k, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)style,
                   (DWORD)((wcslen(style) + 1) * sizeof(WCHAR)));
    RegSetValueExW(k, L"TileWallpaper", 0, REG_SZ, (const BYTE*)tile,
                   (DWORD)((wcslen(tile) + 1) * sizeof(WCHAR)));
    RegCloseKey(k);
}

static std::wstring ReadDesktopString(const wchar_t* name,
                                      const wchar_t* def) {
    WCHAR buf[64];
    DWORD sz = sizeof(buf);
    if (RegGetValueW(HKEY_CURRENT_USER, kDesktopKey, name, RRF_RT_REG_SZ,
                     nullptr, buf, &sz) == ERROR_SUCCESS) {
        return buf;
    }
    return def;
}

// Remembers the wallpaper from before the first time this mod applies one.
static void SaveOriginalOnce() {
    if (Wh_GetIntValue(L"origSaved", 0)) return;
    WCHAR path[MAX_PATH] = L"";
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    Wh_SetStringValue(L"origPath", path);
    Wh_SetStringValue(L"origStyle", ReadDesktopString(L"WallpaperStyle", L"10").c_str());
    Wh_SetStringValue(L"origTile", ReadDesktopString(L"TileWallpaper", L"0").c_str());
    Wh_SetIntValue(L"origSaved", 1);
}

static bool SetWallpaperFile(const wchar_t* path) {
    return SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)path,
                                 SPIF_UPDATEINIFILE | SPIF_SENDCHANGE) != 0;
}

// Draws the full-size picture and applies it. Returns false on failure.
static bool ApplyWallpaper(int& outW, int& outH) {
    int W = 0, H = 0;
    GetTargetSize(W, H);
    outW = W;
    outH = H;

    std::wstring dir;
    if (!EnsureDir(dir)) return false;

    PixBuf buf((size_t)W * H);
    if (!buf.p) return false;
    RenderRegion(g_p, W, H, 0, 0, W, H, buf.p);

    WCHAR name[64];
    swprintf_s(name, L"wallpaper_%llu.bmp", (unsigned long long)GetTickCount64());
    std::wstring path = dir + L"\\" + name;
    if (!WriteBmpFile(path, buf.p, W, H)) return false;

    SaveOriginalOnce();
    SetWallpaperStyle(L"10", L"0");  // Fill
    if (!SetWallpaperFile(path.c_str())) {
        Wh_Log(L"SPI_SETDESKWALLPAPER failed: %lu", GetLastError());
        return false;
    }
    CleanOldWallpapers(dir, name);
    return true;
}

static bool RestoreOriginalWallpaper() {
    if (!Wh_GetIntValue(L"origSaved", 0)) return false;
    WCHAR path[MAX_PATH] = L"", style[16] = L"10", tile[16] = L"0";
    Wh_GetStringValue(L"origPath", path, ARRAYSIZE(path));
    Wh_GetStringValue(L"origStyle", style, ARRAYSIZE(style));
    Wh_GetStringValue(L"origTile", tile, ARRAYSIZE(tile));
    SetWallpaperStyle(style, tile);
    return SetWallpaperFile(path);
}

// ---------------------------------------------------------------------------
// Theme, Mica and drawing helpers
// ---------------------------------------------------------------------------

static const COLORREF kNone = 0xFFFFFFFF;

struct Palette {
    COLORREF bg, card, cardBorder, text, subText;
    COLORREF btn, btnHot, btnDown, btnBorder;
    COLORREF track, knob, previewBg;
    COLORREF accent, accentHot, accentDown, accentText;
};

static Palette g_pal;
static bool g_mica = false;  // true while a system backdrop shows through
static DWORD g_build = 0;
static HBRUSH g_brCard = nullptr;
static HBRUSH g_brBtn = nullptr;
static int g_dpi = 96;

#define SC(v) MulDiv((v), g_dpi, 96)

typedef HRESULT(WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
typedef HRESULT(WINAPI* DwmExtendFrame_t)(HWND, const void*);
typedef HRESULT(WINAPI* SetWindowTheme_t)(HWND, LPCWSTR, LPCWSTR);

struct WlsMargins {
    int left, right, top, bottom;
};

static HMODULE g_dwmLib = nullptr;
static HMODULE g_uxLib = nullptr;
static DwmSetWindowAttribute_t g_pDwmSet = nullptr;
static DwmExtendFrame_t g_pDwmExtend = nullptr;
static SetWindowTheme_t g_pSetTheme = nullptr;

static void InitDynApis() {
    g_dwmLib = LoadLibraryW(L"dwmapi.dll");
    if (g_dwmLib) {
        g_pDwmSet = (DwmSetWindowAttribute_t)GetProcAddress(g_dwmLib, "DwmSetWindowAttribute");
        g_pDwmExtend = (DwmExtendFrame_t)GetProcAddress(g_dwmLib, "DwmExtendFrameIntoClientArea");
    }
    g_uxLib = LoadLibraryW(L"uxtheme.dll");
    if (g_uxLib) {
        g_pSetTheme = (SetWindowTheme_t)GetProcAddress(g_uxLib, "SetWindowTheme");
    }

    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersion_t rtl = nt ? (RtlGetVersion_t)GetProcAddress(nt, "RtlGetVersion") : nullptr;
    OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    g_build = (rtl && rtl(&vi) == 0) ? vi.dwBuildNumber : 0;
}

static void FreeDynApis() {
    if (g_dwmLib) FreeLibrary(g_dwmLib);
    if (g_uxLib) FreeLibrary(g_uxLib);
    g_dwmLib = g_uxLib = nullptr;
    g_pDwmSet = nullptr;
    g_pDwmExtend = nullptr;
    g_pSetTheme = nullptr;
}

static inline uint32_t ToPx(COLORREF c) {
    return ((c & 255u) << 16) | (c & 0xFF00u) | ((c >> 16) & 255u);
}

static COLORREF MixCol(COLORREF a, COLORREF b, float t) {
    auto mix = [t](int x, int y) { return (int)((float)x + (float)(y - x) * t + 0.5f); };
    return RGB(mix(GetRValue(a), GetRValue(b)), mix(GetGValue(a), GetGValue(b)),
               mix(GetBValue(a), GetBValue(b)));
}

static float Luma(COLORREF c) {
    return (0.2126f * GetRValue(c) + 0.7152f * GetGValue(c) + 0.0722f * GetBValue(c)) / 255.0f;
}

static COLORREF ReadAccent() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS) {
        return (COLORREF)(v & 0x00FFFFFFu);  // stored as 0xAABBGGRR
    }
    return RGB(0, 120, 212);
}

// Works out the (dark) palette. Never uses pure black for
// anything that is drawn: on a Mica window pure black means "transparent".
static void ResolveTheme() {
    COLORREF acc = ReadAccent();
    if (Luma(acc) < 0.30f) acc = MixCol(acc, RGB(255, 255, 255), 0.30f);
    g_pal.bg = RGB(32, 32, 32);
    g_pal.card = RGB(43, 43, 43);
    g_pal.cardBorder = RGB(60, 60, 60);
    g_pal.text = RGB(250, 250, 250);
    g_pal.subText = RGB(170, 170, 170);
    g_pal.btn = RGB(55, 55, 55);
    g_pal.btnHot = RGB(64, 64, 64);
    g_pal.btnDown = RGB(48, 48, 48);
    g_pal.btnBorder = RGB(78, 78, 78);
    g_pal.track = RGB(118, 118, 118);
    g_pal.knob = RGB(72, 72, 72);
    g_pal.previewBg = RGB(24, 24, 24);
    g_pal.accent = acc;
    g_pal.accentHot = MixCol(acc, g_pal.card, 0.10f);
    g_pal.accentDown = MixCol(acc, g_pal.card, 0.22f);
    g_pal.accentText = Luma(acc) > 0.6f ? RGB(24, 24, 24) : RGB(255, 255, 255);

    if (g_brCard) DeleteObject(g_brCard);
    if (g_brBtn) DeleteObject(g_brBtn);
    g_brCard = CreateSolidBrush(g_pal.card);
    g_brBtn = CreateSolidBrush(g_pal.btn);
}

// Dark title bar and the Mica / Acrylic backdrop.
static void ApplyWindowFrame(HWND hwnd) {
    g_mica = false;
    if (!g_pDwmSet) return;

    BOOL dark = TRUE;  // the editor is always dark
    if (FAILED(g_pDwmSet(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark)))) {
        g_pDwmSet(hwnd, 19, &dark, sizeof(dark));
    }

    if (g_build >= 22621 && g_backdrop != 0 && g_pDwmExtend) {
        // DWMSBT_MAINWINDOW = 2 (Mica), TRANSIENTWINDOW = 3 (Acrylic), TABBEDWINDOW = 4 (Mica Alt)
        int type = g_backdrop == 1 ? 2 : (g_backdrop == 2 ? 4 : 3);
        if (SUCCEEDED(g_pDwmSet(hwnd, 38 /*DWMWA_SYSTEMBACKDROP_TYPE*/, &type, sizeof(type)))) {
            WlsMargins all = {-1, -1, -1, -1};
            if (SUCCEEDED(g_pDwmExtend(hwnd, &all))) g_mica = true;
        }
    }
    if (!g_mica) {
        if (g_build >= 22621) {
            int none = 1;  // DWMSBT_NONE
            g_pDwmSet(hwnd, 38, &none, sizeof(none));
        }
        if (g_pDwmExtend) {
            WlsMargins zero = {0, 0, 0, 0};
            g_pDwmExtend(hwnd, &zero);
        }
    }
}

// 32-bit top-down bitmap with anti-aliased shape drawing. Pure black pixels
// are never produced by the shape functions (see ResolveTheme).
struct Canvas {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    uint32_t* px = nullptr;
    int w = 0, h = 0;
    COLORREF edgeBase = kNone;  // what a black (transparent) pixel counts as when blending

    ~Canvas() { Destroy(); }

    bool Create(HDC ref, int w_, int h_) {
        Destroy();
        if (w_ < 1 || h_ < 1) return false;
        dc = CreateCompatibleDC(ref);
        if (!dc) return false;
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w_;
        bi.bmiHeader.biHeight = -h_;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        bmp = CreateDIBSection(ref, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp || !bits) {
            Destroy();
            return false;
        }
        px = (uint32_t*)bits;
        old = SelectObject(dc, bmp);
        w = w_;
        h = h_;
        return true;
    }

    void Destroy() {
        if (dc && old) SelectObject(dc, old);
        if (bmp) DeleteObject(bmp);
        if (dc) DeleteDC(dc);
        dc = nullptr;
        bmp = nullptr;
        old = nullptr;
        px = nullptr;
        w = h = 0;
    }

    void Clear(COLORREF c) {
        const uint32_t v = ToPx(c);
        for (int i = 0; i < w * h; i++) px[i] = v;
    }

    void Blend(int x, int y, COLORREF c, float a) {
        if (a <= 0.0f || x < 0 || y < 0 || x >= w || y >= h) return;
        uint32_t& d = px[(size_t)y * w + x];
        uint32_t v;
        if (a >= 1.0f) {
            v = ToPx(c);
        } else {
            float dr, dg, db;
            if (d == 0 && edgeBase != kNone) {
                dr = (float)GetRValue(edgeBase);
                dg = (float)GetGValue(edgeBase);
                db = (float)GetBValue(edgeBase);
            } else {
                dr = (float)((d >> 16) & 255);
                dg = (float)((d >> 8) & 255);
                db = (float)(d & 255);
            }
            const int r = (int)(dr + ((float)GetRValue(c) - dr) * a + 0.5f);
            const int g = (int)(dg + ((float)GetGValue(c) - dg) * a + 0.5f);
            const int b = (int)(db + ((float)GetBValue(c) - db) * a + 0.5f);
            v = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
        if ((v & 0xFFFFFFu) == 0) v = 0x010101;
        d = v;
    }

    // Rounded rectangle with a border of width bw. fill / border may be kNone.
    void FillRR(float x0, float y0, float x1, float y1, float r, COLORREF fill,
                COLORREF border, float bw) {
        const float cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f;
        const float hx = (x1 - x0) * 0.5f, hy = (y1 - y0) * 0.5f;
        r = std::min(r, std::min(hx, hy));
        const int ix0 = std::max(0, (int)floorf(x0) - 1);
        const int iy0 = std::max(0, (int)floorf(y0) - 1);
        const int ix1 = std::min(w - 1, (int)ceilf(x1) + 1);
        const int iy1 = std::min(h - 1, (int)ceilf(y1) + 1);
        for (int y = iy0; y <= iy1; y++) {
            for (int x = ix0; x <= ix1; x++) {
                const float qx = fabsf((float)x + 0.5f - cx) - (hx - r);
                const float qy = fabsf((float)y + 0.5f - cy) - (hy - r);
                const float mx = fmaxf(qx, 0.0f), my = fmaxf(qy, 0.0f);
                const float d = sqrtf(mx * mx + my * my) + fminf(fmaxf(qx, qy), 0.0f) - r;
                const float covOut = Clamp01f(0.5f - d);
                if (covOut <= 0.0f) continue;
                if (border != kNone && bw > 0.0f) {
                    const float covIn = Clamp01f(0.5f - (d + bw));
                    if (fill != kNone) {
                        Blend(x, y, border, covOut);
                        Blend(x, y, fill, covIn);
                    } else {
                        Blend(x, y, border, covOut - covIn);
                    }
                } else if (fill != kNone) {
                    Blend(x, y, fill, covOut);
                }
            }
        }
    }
};

static inline float LineW() { return std::max(1.0f, (float)SC(1)); }

// ---------------------------------------------------------------------------
// Editor window: shared state
// ---------------------------------------------------------------------------

enum {
    IDC_PREVIEW = 100,
    IDC_ZOOMINFO,
    IDC_FIT,
    IDC_ONE,
    IDC_PRESET,
    IDC_PATTERN,
    IDC_RES,
    IDC_RANDOM,
    IDC_RESET,
    IDC_SAVE,
    IDC_APPLY,
    IDC_RESTORE,
    IDC_STATUS,
    IDC_SW_LINE,
    IDC_SW_CENTER,
    IDC_SW_EDGE,
    IDC_HEX_LINE,
    IDC_HEX_CENTER,
    IDC_HEX_EDGE,
    IDC_SL_FIRST = 200,
    IDC_VAL_FIRST = 300,
};

struct SliderDef {
    const wchar_t* en;
    const wchar_t* tr;
    int mn, mx;
    int Params::*field;
    const wchar_t* unit;
    bool tenths;
};

static const SliderDef kSliders[] = {
    {L"Angle", L"Açı", 0, 179, &Params::angle, L"\u00B0", false},
    {L"Spacing", L"Aralık", 2, 40, &Params::spacing, L" px", false},
    {L"Thickness", L"Kalınlık", 3, 120, &Params::thick10, L" px", true},
    {L"Line opacity", L"Çizgi saydamlığı", 0, 100, &Params::lineOpacity, L"%", false},
    {L"Light X", L"Işık X", 0, 100, &Params::lightX, L"%", false},
    {L"Light Y", L"Işık Y", 0, 100, &Params::lightY, L"%", false},
    {L"Light size", L"Işık boyutu", 10, 200, &Params::lightSize, L"%", false},
    {L"Vignette", L"Vinyet", 0, 100, &Params::vignette, L"%", false},
    {L"Grain", L"Gren", 0, 100, &Params::grain, L"%", false},
};
static const int kSliderCount = (int)(sizeof(kSliders) / sizeof(kSliders[0]));

// Cards of the editor (design pixels at 96 dpi, client coordinates).
static const RECT kCards[] = {
    {16, 16, 600, 392},    // preview
    {16, 400, 600, 578},   // actions
    {616, 16, 964, 104},   // preset, pattern
    {616, 112, 964, 420},  // sliders
    {616, 428, 964, 578},  // colors
};
static const int kClientW = 980, kClientH = 594;

static const UINT WM_APP_TRAY = WM_APP + 1;
static const UINT WM_APP_SETTINGS = WM_APP + 2;
static const UINT SLM_SETRANGE = WM_APP + 20;  // wParam = min, lParam = max
static const UINT SLM_SETPOS = WM_APP + 21;    // wParam = position
static const UINT SLM_GETPOS = WM_APP + 22;

#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif

static std::atomic<HWND> g_msgWnd{nullptr};
static HWND g_editor = nullptr;
static HFONT g_font = nullptr;
static HICON g_trayIcon = nullptr;
static UINT g_wmTaskbarCreated = 0;

static Canvas g_bg;        // cards on the window background
static HDC g_prevDC = nullptr;
static HBITMAP g_prevBmp = nullptr;
static HGDIOBJ g_prevOld = nullptr;
static uint32_t* g_prevBits = nullptr;
static int g_pw = 560, g_ph = 315;

static float g_zoom = 1.0f;  // 1 = whole picture visible
static float g_cx = 0, g_cy = 0;  // picture position shown in the preview center
static bool g_dragging = false;
static int g_dragX = 0, g_dragY = 0;
static bool g_refreshPending = false;
static int g_lastPct = -1;
static HWND g_hotBtn = nullptr;

static HWND Mk(const wchar_t* cls, const wchar_t* text, DWORD style, int x,
               int y, int w, int h, HWND parent, int id) {
    return CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style, SC(x),
                           SC(y), SC(w), SC(h), parent, (HMENU)(INT_PTR)id,
                           GetModuleHandleW(nullptr), nullptr);
}

static void SetStatus(const wchar_t* text) {
    if (g_editor) SetDlgItemTextW(g_editor, IDC_STATUS, text);
}

// ---------------------------------------------------------------------------
// Slider control (own window class)
// ---------------------------------------------------------------------------

struct SliderData {
    int mn, mx, pos;
    bool drag, hot;
};

static SliderData* SD(HWND h) {
    return (SliderData*)GetWindowLongPtrW(h, GWLP_USERDATA);
}

static int SliderFromX(HWND h, const SliderData* d, int x) {
    RECT rc;
    GetClientRect(h, &rc);
    const int pad = SC(11);
    const float span = (float)std::max(1, (int)rc.right - 2 * pad);
    const float t = Clamp01f(((float)(x - pad)) / span);
    return d->mn + (int)(t * (float)(d->mx - d->mn) + 0.5f);
}

static void SliderSet(HWND h, SliderData* d, int v, bool notify) {
    v = std::clamp(v, d->mn, d->mx);
    if (v == d->pos) return;
    d->pos = v;
    InvalidateRect(h, nullptr, FALSE);
    if (notify) {
        SendMessageW(GetParent(h), WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, v), (LPARAM)h);
    }
}

static LRESULT CALLBACK SliderProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCCREATE) {
        SliderData* nd = (SliderData*)calloc(1, sizeof(SliderData));
        if (!nd) return FALSE;
        nd->mn = 0;
        nd->mx = 100;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)nd);
        return DefWindowProcW(h, m, w, l);
    }
    SliderData* d = SD(h);
    if (!d) return DefWindowProcW(h, m, w, l);

    switch (m) {
        case WM_NCDESTROY:
            free(d);
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;

        case SLM_SETRANGE:
            d->mn = (int)w;
            d->mx = std::max((int)l, (int)w + 1);
            d->pos = std::clamp(d->pos, d->mn, d->mx);
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        case SLM_SETPOS:
            d->pos = std::clamp((int)w, d->mn, d->mx);
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        case SLM_GETPOS:
            return d->pos;

        case WM_GETDLGCODE:
            return DLGC_WANTARROWS;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(h, &ps);
            RECT rc;
            GetClientRect(h, &rc);
            const int cw = rc.right, ch = rc.bottom;
            Canvas cv;
            if (cv.Create(hdc, cw, ch)) {
                cv.Clear(g_pal.card);
                const float pad = (float)SC(11), cy = (float)ch * 0.5f;
                const float th = (float)SC(4);
                const float x0 = pad, x1 = (float)cw - pad;
                const float t = (float)(d->pos - d->mn) / (float)(d->mx - d->mn);
                const float kx = x0 + (x1 - x0) * t;
                cv.FillRR(x0, cy - th / 2, x1, cy + th / 2, th / 2, g_pal.track, kNone, 0);
                if (kx > x0) {
                    cv.FillRR(x0, cy - th / 2, kx, cy + th / 2, th / 2, g_pal.accent, kNone, 0);
                }
                const bool focus = GetFocus() == h;
                const float R = (float)SC(9);
                cv.FillRR(kx - R, cy - R, kx + R, cy + R, R, g_pal.knob,
                          focus ? g_pal.accent : g_pal.btnBorder,
                          focus ? LineW() * 1.5f : LineW());
                const float r2 = (d->drag ? 4.0f : (d->hot ? 6.0f : 5.0f)) * (float)g_dpi / 96.0f;
                cv.FillRR(kx - r2, cy - r2, kx + r2, cy + r2, r2, g_pal.accent, kNone, 0);
                BitBlt(hdc, 0, 0, cw, ch, cv.dc, 0, 0, SRCCOPY);
            }
            EndPaint(h, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
            SetFocus(h);
            SetCapture(h);
            d->drag = true;
            SliderSet(h, d, SliderFromX(h, d, GET_X_LPARAM(l)), true);
            InvalidateRect(h, nullptr, FALSE);
            return 0;

        case WM_MOUSEMOVE:
            if (!d->hot) {
                d->hot = true;
                TRACKMOUSEEVENT te = {sizeof(te), TME_LEAVE, h, 0};
                TrackMouseEvent(&te);
                InvalidateRect(h, nullptr, FALSE);
            }
            if (d->drag) SliderSet(h, d, SliderFromX(h, d, GET_X_LPARAM(l)), true);
            return 0;

        case WM_MOUSELEAVE:
            d->hot = false;
            InvalidateRect(h, nullptr, FALSE);
            return 0;

        case WM_LBUTTONUP:
            if (GetCapture() == h) ReleaseCapture();
            return 0;

        case WM_CAPTURECHANGED:
            d->drag = false;
            InvalidateRect(h, nullptr, FALSE);
            return 0;

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            InvalidateRect(h, nullptr, FALSE);
            return 0;

        case WM_KEYDOWN: {
            const int big = std::max(1, (d->mx - d->mn) / 10);
            switch (w) {
                case VK_LEFT:
                case VK_DOWN: SliderSet(h, d, d->pos - 1, true); return 0;
                case VK_RIGHT:
                case VK_UP: SliderSet(h, d, d->pos + 1, true); return 0;
                case VK_PRIOR: SliderSet(h, d, d->pos + big, true); return 0;
                case VK_NEXT: SliderSet(h, d, d->pos - big, true); return 0;
                case VK_HOME: SliderSet(h, d, d->mn, true); return 0;
                case VK_END: SliderSet(h, d, d->mx, true); return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            const int steps = GET_WHEEL_DELTA_WPARAM(w) / WHEEL_DELTA;
            const int unit = std::max(1, (d->mx - d->mn) / 40);
            SliderSet(h, d, d->pos + steps * unit, true);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

// ---------------------------------------------------------------------------
// Preview: rendering, zoom and pan
// ---------------------------------------------------------------------------

static void FreePreview() {
    if (g_prevDC && g_prevOld) SelectObject(g_prevDC, g_prevOld);
    if (g_prevBmp) DeleteObject(g_prevBmp);
    if (g_prevDC) DeleteDC(g_prevDC);
    g_prevDC = nullptr;
    g_prevBmp = nullptr;
    g_prevOld = nullptr;
    g_prevBits = nullptr;
}

static void CreatePreviewBitmap() {
    g_pw = SC(560);
    g_ph = SC(315);
    HDC scr = GetDC(nullptr);
    g_prevDC = CreateCompatibleDC(scr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_pw;
    bi.bmiHeader.biHeight = -g_ph;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_prevBmp = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, scr);
    if (!g_prevBmp || !bits) {
        FreePreview();
        return;
    }
    g_prevBits = (uint32_t*)bits;
    g_prevOld = SelectObject(g_prevDC, g_prevBmp);
    memset(g_prevBits, 0x20, (size_t)g_pw * g_ph * 4);
}

// Preview pixels per picture pixel when the whole picture is visible.
static float FitScale(int W, int H) {
    return std::min((float)g_pw / (float)W, (float)g_ph / (float)H);
}

static float MaxZoom(float fit) { return std::max(1.0f, 8.0f / fit); }

// Keeps the view inside the picture.
static void ClampView() {
    int W = 0, H = 0;
    GetTargetSize(W, H);
    const float S = FitScale(W, H) * g_zoom;
    const float vw = (float)g_pw / S, vh = (float)g_ph / S;
    g_cx = vw >= (float)W ? (float)W * 0.5f
                          : std::clamp(g_cx, vw * 0.5f, (float)W - vw * 0.5f);
    g_cy = vh >= (float)H ? (float)H * 0.5f
                          : std::clamp(g_cy, vh * 0.5f, (float)H - vh * 0.5f);
}

// Blends the four corners toward `base` so the preview has rounded corners.
static void RoundPreviewCorners(uint32_t* px, int w, int h, float r, COLORREF base) {
    const int n = (int)ceilf(r);
    const float br = (float)GetRValue(base), bgc = (float)GetGValue(base),
                bb = (float)GetBValue(base);
    for (int cyi = 0; cyi < 2; cyi++) {
        for (int cxi = 0; cxi < 2; cxi++) {
            const int xs = cxi ? w - n : 0, ys = cyi ? h - n : 0;
            const float ccx = cxi ? (float)w - r : r;
            const float ccy = cyi ? (float)h - r : r;
            for (int y = ys; y < ys + n; y++) {
                for (int x = xs; x < xs + n; x++) {
                    const float dx = (float)x + 0.5f - ccx, dy = (float)y + 0.5f - ccy;
                    if (!(cxi ? dx > 0 : dx < 0) || !(cyi ? dy > 0 : dy < 0)) continue;
                    const float cov = Clamp01f(0.5f - (sqrtf(dx * dx + dy * dy) - r));
                    if (cov >= 1.0f) continue;
                    uint32_t& v = px[(size_t)y * w + x];
                    const float pr = (float)((v >> 16) & 255), pg = (float)((v >> 8) & 255),
                                pb = (float)(v & 255);
                    v = PackRgb(br + (pr - br) * cov, bgc + (pg - bgc) * cov,
                                bb + (pb - bb) * cov);
                }
            }
        }
    }
}

static void UpdateZoomLabel(float S) {
    const int pct = (int)(S * 100.0f + 0.5f);
    if (pct == g_lastPct) return;
    g_lastPct = pct;
    WCHAR buf[160];
    swprintf_s(buf,
               T(L"Zoom %d%%   |   Wheel: zoom   Drag: pan",
                 L"Yakınlaştırma %%%d   |   Tekerlek: yakınlaştır   Sürükle: kaydır"),
               pct);
    SetDlgItemTextW(g_editor, IDC_ZOOMINFO, buf);
}

static void UpdatePreview() {
    if (!g_prevBits || !g_editor) return;
    int W = 0, H = 0;
    GetTargetSize(W, H);
    ClampView();
    const float S = FitScale(W, H) * g_zoom;
    const float step = 1.0f / S;
    const float ox = g_cx - (float)g_pw * 0.5f * step;
    const float oy = g_cy - (float)g_ph * 0.5f * step;

    // Average several samples per pixel when zoomed out, within a time budget.
    const long long px = (long long)g_pw * g_ph;
    int ss = 1;
    if (step >= 4.0f && px * 9 <= 2200000) ss = 3;
    else if (step >= 1.5f && px * 4 <= 3200000) ss = 2;

    RenderView(g_p, W, H, ox, oy, step, ss, ToPx(g_pal.previewBg), g_pw, g_ph,
               g_prevBits);
    for (long long i = 0; i < px; i++) {
        if ((g_prevBits[i] & 0xFFFFFFu) == 0) g_prevBits[i] = 0x010101;  // never "transparent"
    }
    RoundPreviewCorners(g_prevBits, g_pw, g_ph, (float)SC(6), g_pal.card);

    HWND pv = GetDlgItem(g_editor, IDC_PREVIEW);
    if (pv) InvalidateRect(pv, nullptr, FALSE);
    UpdateZoomLabel(S);
}

static void ScheduleRefresh() {
    if (g_editor && !g_refreshPending) {
        g_refreshPending = true;
        SetTimer(g_editor, 1, 30, nullptr);
    }
}

// Zooms by `factor`, keeping the picture point under (mx, my) in place.
static void ZoomAt(float factor, float mx, float my) {
    int W = 0, H = 0;
    GetTargetSize(W, H);
    const float fit = FitScale(W, H);
    const float S0 = fit * g_zoom;
    const float pxp = g_cx + (mx - (float)g_pw * 0.5f) / S0;
    const float pyp = g_cy + (my - (float)g_ph * 0.5f) / S0;
    g_zoom = std::clamp(g_zoom * factor, 1.0f, MaxZoom(fit));
    const float S1 = fit * g_zoom;
    g_cx = pxp - (mx - (float)g_pw * 0.5f) / S1;
    g_cy = pyp - (my - (float)g_ph * 0.5f) / S1;
    ClampView();
    ScheduleRefresh();
}

// Jumps to real pixels (1:1) around (mx, my), or back to "fit".
static void ToggleOneToOne(float mx, float my) {
    int W = 0, H = 0;
    GetTargetSize(W, H);
    const float fit = FitScale(W, H);
    const float oneToOne = std::clamp(1.0f / fit, 1.0f, MaxZoom(fit));
    if (g_zoom > 1.01f) {
        g_zoom = 1.0f;
        ClampView();
        ScheduleRefresh();
    } else {
        ZoomAt(oneToOne, mx, my);
    }
}

static LRESULT CALLBACK PreviewProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            if (g_prevDC) BitBlt(dc, 0, 0, g_pw, g_ph, g_prevDC, 0, 0, SRCCOPY);
            EndPaint(h, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
            SetCapture(h);
            g_dragging = true;
            g_dragX = GET_X_LPARAM(l);
            g_dragY = GET_Y_LPARAM(l);
            return 0;

        case WM_MOUSEMOVE:
            if (g_dragging) {
                int W = 0, H = 0;
                GetTargetSize(W, H);
                const float S = FitScale(W, H) * g_zoom;
                const int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
                g_cx -= (float)(x - g_dragX) / S;
                g_cy -= (float)(y - g_dragY) / S;
                g_dragX = x;
                g_dragY = y;
                ClampView();
                ScheduleRefresh();
            }
            return 0;

        case WM_LBUTTONUP:
            if (GetCapture() == h) ReleaseCapture();
            return 0;

        case WM_CAPTURECHANGED:
            g_dragging = false;
            return 0;

        case WM_LBUTTONDBLCLK:
            ToggleOneToOne((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l));
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt = {GET_X_LPARAM(l), GET_Y_LPARAM(l)};  // screen coordinates
            ScreenToClient(h, &pt);
            const float notches = (float)GET_WHEEL_DELTA_WPARAM(w) / (float)WHEEL_DELTA;
            ZoomAt(powf(1.2f, notches), (float)pt.x, (float)pt.y);
            return 0;
        }

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, g_zoom > 1.001f ? IDC_SIZEALL : IDC_ARROW));
            return TRUE;
    }
    return DefWindowProcW(h, m, w, l);
}

// ---------------------------------------------------------------------------
// Editor window: controls and behavior
// ---------------------------------------------------------------------------

static void UpdateSliderLabel(int i) {
    const SliderDef& d = kSliders[i];
    int v = g_p.*(d.field);
    WCHAR buf[32];
    if (d.tenths) swprintf_s(buf, L"%.1f%s", v / 10.0, d.unit);
    else swprintf_s(buf, L"%d%s", v, d.unit);
    SetDlgItemTextW(g_editor, IDC_VAL_FIRST + i, buf);
}

static void UpdateHexLabels() {
    const struct {
        int id;
        uint32_t col;
    } items[] = {{IDC_HEX_LINE, g_p.lineCol}, {IDC_HEX_CENTER, g_p.centerCol},
                 {IDC_HEX_EDGE, g_p.edgeCol}};
    for (const auto& it : items) {
        WCHAR buf[16];
        swprintf_s(buf, L"#%02X%02X%02X", ColR(it.col), ColG(it.col), ColB(it.col));
        SetDlgItemTextW(g_editor, it.id, buf);
    }
}

static void SelectPatternItem(int pattern) {
    HWND cb = GetDlgItem(g_editor, IDC_PATTERN);
    const int n = (int)SendMessageW(cb, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < n; i++) {
        if ((int)SendMessageW(cb, CB_GETITEMDATA, i, 0) == pattern) {
            SendMessageW(cb, CB_SETCURSEL, i, 0);
            return;
        }
    }
    SendMessageW(cb, CB_SETCURSEL, -1, 0);
}

// Copies the current parameters into the controls.
static void SyncControls() {
    if (!g_editor) return;
    for (int i = 0; i < kSliderCount; i++) {
        HWND sl = GetDlgItem(g_editor, IDC_SL_FIRST + i);
        SendMessageW(sl, SLM_SETPOS, (WPARAM)(g_p.*(kSliders[i].field)), 0);
        UpdateSliderLabel(i);
    }
    SelectPatternItem(g_p.pattern);
    SendDlgItemMessageW(g_editor, IDC_RES, CB_SETCURSEL, g_p.resMode, 0);
    InvalidateRect(GetDlgItem(g_editor, IDC_SW_LINE), nullptr, FALSE);
    InvalidateRect(GetDlgItem(g_editor, IDC_SW_CENTER), nullptr, FALSE);
    InvalidateRect(GetDlgItem(g_editor, IDC_SW_EDGE), nullptr, FALSE);
    UpdateHexLabels();
}

static bool PickColor(HWND owner, uint32_t& c) {
    static COLORREF custom[16];
    CHOOSECOLORW cc = {};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = owner;
    cc.rgbResult = c;
    cc.lpCustColors = custom;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (!ChooseColorW(&cc)) return false;
    c = cc.rgbResult;
    return true;
}

static void DoApply() {
    HCURSOR old = SetCursor(LoadCursorW(nullptr, IDC_WAIT));
    SetStatus(T(L"Drawing the wallpaper...", L"Duvar kağıdı çiziliyor..."));
    int W = 0, H = 0;
    bool ok = ApplyWallpaper(W, H);
    SetCursor(old);
    SaveParams(g_p);
    WCHAR msg[128];
    if (ok) {
        swprintf_s(msg, T(L"Wallpaper applied (%d x %d).", L"Duvar kağıdı uygulandı (%d x %d)."), W, H);
    } else {
        swprintf_s(msg, T(L"Could not apply the wallpaper.", L"Duvar kağıdı uygulanamadı."));
    }
    SetStatus(msg);
}

static void DoSave(HWND owner) {
    WCHAR file[MAX_PATH] = L"wallpaper.bmp";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"BMP\0*.bmp\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrDefExt = L"bmp";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&ofn)) return;

    HCURSOR old = SetCursor(LoadCursorW(nullptr, IDC_WAIT));
    int W = 0, H = 0;
    GetTargetSize(W, H);
    bool ok = false;
    {
        PixBuf buf((size_t)W * H);
        if (buf.p) {
            RenderRegion(g_p, W, H, 0, 0, W, H, buf.p);
            ok = WriteBmpFile(file, buf.p, W, H);
        }
    }
    SetCursor(old);
    SetStatus(ok ? T(L"Saved.", L"Kaydedildi.")
                 : T(L"Could not save the file.", L"Dosya kaydedilemedi."));
}

static BOOL CALLBACK SetFontProc(HWND h, LPARAM) {
    SendMessageW(h, WM_SETFONT, (WPARAM)g_font, TRUE);
    return TRUE;
}

// Gives the combo boxes the dark system look.
static BOOL CALLBACK ThemeChildProc(HWND h, LPARAM) {
    WCHAR cls[32];
    if (!GetClassNameW(h, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"ComboBox") != 0) return TRUE;
    if (g_pSetTheme) {
        g_pSetTheme(h, L"DarkMode_CFD", nullptr);
        COMBOBOXINFO ci = {};
        ci.cbSize = sizeof(ci);
        if (GetComboBoxInfo(h, &ci) && ci.hwndList) {
            g_pSetTheme(ci.hwndList, L"DarkMode_Explorer", nullptr);
        }
    }
    InvalidateRect(h, nullptr, TRUE);
    return TRUE;
}

static void RebuildBackground(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC dc = GetDC(hwnd);
    g_bg.Destroy();
    if (g_bg.Create(dc, rc.right, rc.bottom)) {
        g_bg.Clear(g_mica ? 0 : g_pal.bg);  // black = transparent on a Mica window
        g_bg.edgeBase = g_mica ? g_pal.bg : kNone;
        for (const RECT& c : kCards) {
            g_bg.FillRR((float)SC(c.left), (float)SC(c.top), (float)SC(c.right),
                        (float)SC(c.bottom), (float)SC(8), g_pal.card,
                        g_pal.cardBorder, LineW());
        }
    }
    ReleaseDC(hwnd, dc);
}

// Applies the current theme to the whole editor.
static void ApplyTheme(HWND hwnd) {
    ResolveTheme();
    ApplyWindowFrame(hwnd);
    EnumChildWindows(hwnd, ThemeChildProc, 0);
    RebuildBackground(hwnd);
    UpdatePreview();
    RedrawWindow(hwnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static COLORREF SwatchColor(int id) {
    return id == IDC_SW_LINE ? g_p.lineCol : (id == IDC_SW_CENTER ? g_p.centerCol : g_p.edgeCol);
}

static void DrawOwnerButton(const DRAWITEMSTRUCT* dis) {
    const int id = (int)dis->CtlID;
    const RECT rc = dis->rcItem;
    const int w = rc.right - rc.left, h = rc.bottom - rc.top;
    Canvas cv;
    if (!cv.Create(dis->hDC, w, h)) return;
    cv.Clear(g_pal.card);

    const bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    const bool hot = g_hotBtn == dis->hwndItem;
    const bool focus = (dis->itemState & ODS_FOCUS) != 0;
    const float rad = (float)SC(5);

    if (id == IDC_SW_LINE || id == IDC_SW_CENTER || id == IDC_SW_EDGE) {
        cv.FillRR(0, 0, (float)w, (float)h, rad, SwatchColor(id), g_pal.btnBorder, LineW());
    } else {
        const bool primary = id == IDC_APPLY;
        COLORREF fill, border, txt;
        if (primary) {
            fill = pressed ? g_pal.accentDown : (hot ? g_pal.accentHot : g_pal.accent);
            border = fill;
            txt = g_pal.accentText;
        } else {
            fill = pressed ? g_pal.btnDown : (hot ? g_pal.btnHot : g_pal.btn);
            border = g_pal.btnBorder;
            txt = g_pal.text;
        }
        cv.FillRR(0, 0, (float)w, (float)h, rad, fill, border, LineW());

        WCHAR text[128] = L"";
        GetWindowTextW(dis->hwndItem, text, ARRAYSIZE(text));
        HGDIOBJ oldFont = SelectObject(cv.dc, g_font);
        SetBkMode(cv.dc, TRANSPARENT);
        SetTextColor(cv.dc, txt);
        RECT tr = {SC(6), 0, w - SC(6), h};
        DrawTextW(cv.dc, text, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        SelectObject(cv.dc, oldFont);
    }
    if (focus) {
        const float o = LineW() * 1.5f;
        cv.FillRR(o * 0.5f, o * 0.5f, (float)w - o * 0.5f, (float)h - o * 0.5f,
                  std::max(1.0f, rad - 1.0f), kNone,
                  id == IDC_APPLY ? g_pal.text : g_pal.accent, o);
    }
    BitBlt(dis->hDC, rc.left, rc.top, w, h, cv.dc, 0, 0, SRCCOPY);
}

// Tracks which button the mouse is over so it can be drawn highlighted.
static LRESULT CALLBACK BtnSubclass(HWND h, UINT m, WPARAM w, LPARAM l,
                                    UINT_PTR id, DWORD_PTR) {
    switch (m) {
        case WM_MOUSEMOVE:
            if (g_hotBtn != h) {
                HWND prev = g_hotBtn;
                g_hotBtn = h;
                TRACKMOUSEEVENT te = {sizeof(te), TME_LEAVE, h, 0};
                TrackMouseEvent(&te);
                if (prev) InvalidateRect(prev, nullptr, FALSE);
                InvalidateRect(h, nullptr, FALSE);
            }
            break;
        case WM_MOUSELEAVE:
            if (g_hotBtn == h) g_hotBtn = nullptr;
            InvalidateRect(h, nullptr, FALSE);
            break;
        case WM_NCDESTROY:
            if (g_hotBtn == h) g_hotBtn = nullptr;
            RemoveWindowSubclass(h, BtnSubclass, id);
            break;
    }
    return DefSubclassProc(h, m, w, l);
}

static HWND MkButton(const wchar_t* text, int x, int y, int w, int h, HWND parent, int id) {
    HWND b = Mk(L"BUTTON", text, BS_OWNERDRAW | WS_TABSTOP, x, y, w, h, parent, id);
    if (b) SetWindowSubclass(b, BtnSubclass, (UINT_PTR)id, 0);
    return b;
}

static HWND MkCombo(int x, int y, int w, HWND parent, int id) {
    HWND c = Mk(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, x, y, w, 220, parent, id);
    if (c) SendMessageW(c, CB_SETITEMHEIGHT, (WPARAM)-1, SC(24));
    return c;
}

static void BuildEditor(HWND hwnd) {
    g_editor = hwnd;
    g_lastPct = -1;
    g_zoom = 1.0f;
    g_cx = g_cy = 0;
    ResolveTheme();
    CreatePreviewBitmap();

    g_font = CreateFontW(-MulDiv(9, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE,
                         FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Card 1: preview.
    Mk(L"WLSPreview", L"", 0, 28, 28, 560, 315, hwnd, IDC_PREVIEW);
    Mk(L"STATIC", L"", SS_LEFT, 28, 355, 410, 22, hwnd, IDC_ZOOMINFO);
    MkButton(T(L"Fit", L"Sığdır"), 450, 351, 64, 28, hwnd, IDC_FIT);
    MkButton(L"1:1", 522, 351, 66, 28, hwnd, IDC_ONE);

    // Card 2: actions.
    Mk(L"STATIC", T(L"Resolution", L"Çözünürlük"), SS_LEFT, 28, 416, 110, 22, hwnd, -1);
    HWND res = MkCombo(142, 412, 300, hwnd, IDC_RES);
    SendMessageW(res, CB_ADDSTRING, 0, (LPARAM)T(L"Screen size (automatic)", L"Ekran boyutu (otomatik)"));
    SendMessageW(res, CB_ADDSTRING, 0, (LPARAM)L"1920 x 1080");
    SendMessageW(res, CB_ADDSTRING, 0, (LPARAM)L"2560 x 1440");
    SendMessageW(res, CB_ADDSTRING, 0, (LPARAM)L"3840 x 2160");

    MkButton(T(L"Randomize", L"Rastgele"), 28, 452, 176, 34, hwnd, IDC_RANDOM);
    MkButton(T(L"Reset", L"Sıfırla"), 212, 452, 176, 34, hwnd, IDC_RESET);
    MkButton(T(L"Save as BMP...", L"BMP olarak kaydet..."), 396, 452, 192, 34, hwnd, IDC_SAVE);
    MkButton(T(L"Apply as wallpaper", L"Duvar kağıdı olarak uygula"), 28, 494, 372, 40, hwnd, IDC_APPLY);
    MkButton(T(L"Restore original", L"Özgün olanı geri yükle"), 408, 494, 180, 40, hwnd, IDC_RESTORE);
    Mk(L"STATIC", L"", SS_LEFT, 28, 544, 560, 22, hwnd, IDC_STATUS);

    // Card 3: preset, pattern.
    const int lx = 628, cx = 724, cw = 228;
    Mk(L"STATIC", T(L"Preset", L"Hazır ayar"), SS_LEFT, lx, 32, 92, 22, hwnd, -1);
    HWND pre = MkCombo(cx, 28, cw, hwnd, IDC_PRESET);
    for (int i = 0; i < kPresetCount; i++) {
        SendMessageW(pre, CB_ADDSTRING, 0, (LPARAM)T(kPresets[i].en, kPresets[i].tr));
    }
    SendMessageW(pre, CB_SETCURSEL, (WPARAM)-1, 0);

    Mk(L"STATIC", T(L"Pattern", L"Desen"), SS_LEFT, lx, 68, 92, 22, hwnd, -1);
    HWND pat = MkCombo(cx, 64, cw, hwnd, IDC_PATTERN);
    for (int i = 0; i < kPatternCount; i++) {
        int idx = (int)SendMessageW(pat, CB_ADDSTRING, 0, (LPARAM)T(kPatterns[i].en, kPatterns[i].tr));
        SendMessageW(pat, CB_SETITEMDATA, idx, kPatterns[i].id);
    }


    // Card 4: sliders.
    for (int i = 0; i < kSliderCount; i++) {
        const int y = 122 + i * 32;
        Mk(L"STATIC", T(kSliders[i].en, kSliders[i].tr), SS_LEFT | SS_ENDELLIPSIS, lx, y + 4, 96, 22, hwnd, -1);
        HWND sl = Mk(L"WLSSlider", L"", WS_TABSTOP, 726, y, 160, 28, hwnd, IDC_SL_FIRST + i);
        SendMessageW(sl, SLM_SETRANGE, (WPARAM)kSliders[i].mn, (LPARAM)kSliders[i].mx);
        Mk(L"STATIC", L"", SS_RIGHT, 890, y + 4, 62, 22, hwnd, IDC_VAL_FIRST + i);
    }

    // Card 5: colors.
    const struct {
        const wchar_t* en;
        const wchar_t* tr;
        int sw, hex;
    } colors[] = {
        {L"Line color", L"Çizgi rengi", IDC_SW_LINE, IDC_HEX_LINE},
        {L"Center color", L"Merkez rengi", IDC_SW_CENTER, IDC_HEX_CENTER},
        {L"Edge color", L"Kenar rengi", IDC_SW_EDGE, IDC_HEX_EDGE},
    };
    for (int i = 0; i < 3; i++) {
        const int y = 442 + i * 38;
        Mk(L"STATIC", T(colors[i].en, colors[i].tr), SS_LEFT | SS_ENDELLIPSIS, lx, y + 4, 96, 22, hwnd, -1);
        MkButton(L"", 726, y, 100, 26, hwnd, colors[i].sw);
        Mk(L"STATIC", L"", SS_LEFT, 836, y + 4, 116, 22, hwnd, colors[i].hex);
    }

    EnumChildWindows(hwnd, SetFontProc, 0);

    ApplyWindowFrame(hwnd);
    EnumChildWindows(hwnd, ThemeChildProc, 0);
    RebuildBackground(hwnd);
    SyncControls();
    UpdatePreview();
}

static LRESULT CALLBACK EditorProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            BuildEditor(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (g_bg.dc) BitBlt(dc, 0, 0, g_bg.w, g_bg.h, g_bg.dc, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC dc = (HDC)wp;
            const int id = GetDlgCtrlID((HWND)lp);
            SetBkColor(dc, g_pal.card);
            SetTextColor(dc, (id == IDC_ZOOMINFO || id == IDC_STATUS) ? g_pal.subText : g_pal.text);
            return (LRESULT)g_brCard;
        }

        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLOREDIT: {
            HDC dc = (HDC)wp;
            SetBkColor(dc, g_pal.btn);
            SetTextColor(dc, g_pal.text);
            return (LRESULT)g_brBtn;
        }

        case WM_DRAWITEM: {
            const DRAWITEMSTRUCT* dis = (const DRAWITEMSTRUCT*)lp;
            if (dis->CtlType == ODT_BUTTON) {
                DrawOwnerButton(dis);
                return TRUE;
            }
            break;
        }

        case WM_SETTINGCHANGE:  // accent color changes
            if (lp && wcscmp((LPCWSTR)lp, L"ImmersiveColorSet") == 0) ApplyTheme(hwnd);
            return 0;

        case 0x0320:  // WM_DWMCOLORIZATIONCOLORCHANGED
            ApplyTheme(hwnd);
            return 0;

        case WM_HSCROLL: {
            HWND sl = (HWND)lp;
            int id = GetDlgCtrlID(sl) - IDC_SL_FIRST;
            if (id >= 0 && id < kSliderCount) {
                g_p.*(kSliders[id].field) = (int)SendMessageW(sl, SLM_GETPOS, 0, 0);
                UpdateSliderLabel(id);
                SendDlgItemMessageW(hwnd, IDC_PRESET, CB_SETCURSEL, (WPARAM)-1, 0);
                ScheduleRefresh();
            }
            return 0;
        }

        case WM_COMMAND: {
            const int id = LOWORD(wp), code = HIWORD(wp);
            switch (id) {
                case IDC_PRESET:
                    if (code == CBN_SELCHANGE) {
                        int sel = (int)SendDlgItemMessageW(hwnd, IDC_PRESET, CB_GETCURSEL, 0, 0);
                        ApplyPreset(g_p, sel);
                        SyncControls();
                        ScheduleRefresh();
                    }
                    break;
                case IDC_PATTERN:
                    if (code == CBN_SELCHANGE) {
                        int sel = (int)SendDlgItemMessageW(hwnd, IDC_PATTERN, CB_GETCURSEL, 0, 0);
                        if (sel >= 0) {
                            g_p.pattern = std::clamp(
                                (int)SendDlgItemMessageW(hwnd, IDC_PATTERN, CB_GETITEMDATA, sel, 0),
                                0, (int)PAT_COUNT - 1);
                        }
                        SendDlgItemMessageW(hwnd, IDC_PRESET, CB_SETCURSEL, (WPARAM)-1, 0);
                        ScheduleRefresh();
                    }
                    break;
                case IDC_RES:
                    if (code == CBN_SELCHANGE) {
                        g_p.resMode = std::clamp(
                            (int)SendDlgItemMessageW(hwnd, IDC_RES, CB_GETCURSEL, 0, 0), 0, 3);
                        ScheduleRefresh();
                    }
                    break;
                case IDC_FIT:
                    g_zoom = 1.0f;
                    ClampView();
                    ScheduleRefresh();
                    break;
                case IDC_ONE: {
                    int W = 0, H = 0;
                    GetTargetSize(W, H);
                    const float fit = FitScale(W, H);
                    ZoomAt(std::clamp(1.0f / fit, 1.0f, MaxZoom(fit)) / g_zoom,
                           (float)g_pw * 0.5f, (float)g_ph * 0.5f);
                    break;
                }
                case IDC_RANDOM:
                    Randomize(g_p);
                    SendDlgItemMessageW(hwnd, IDC_PRESET, CB_SETCURSEL, (WPARAM)-1, 0);
                    SyncControls();
                    ScheduleRefresh();
                    break;
                case IDC_RESET:
                    ApplyPreset(g_p, 0);
                    SendDlgItemMessageW(hwnd, IDC_PRESET, CB_SETCURSEL, 0, 0);
                    SyncControls();
                    ScheduleRefresh();
                    break;
                case IDC_SW_LINE:
                case IDC_SW_CENTER:
                case IDC_SW_EDGE: {
                    uint32_t* target = id == IDC_SW_LINE     ? &g_p.lineCol
                                       : id == IDC_SW_CENTER ? &g_p.centerCol
                                                             : &g_p.edgeCol;
                    if (PickColor(hwnd, *target)) {
                        InvalidateRect(GetDlgItem(hwnd, id), nullptr, FALSE);
                        UpdateHexLabels();
                        SendDlgItemMessageW(hwnd, IDC_PRESET, CB_SETCURSEL, (WPARAM)-1, 0);
                        ScheduleRefresh();
                    }
                    break;
                }
                case IDC_APPLY:
                    DoApply();
                    break;
                case IDC_SAVE:
                    DoSave(hwnd);
                    break;
                case IDC_RESTORE:
                    SetStatus(RestoreOriginalWallpaper()
                                  ? T(L"Original wallpaper restored.", L"Özgün duvar kağıdı geri yüklendi.")
                                  : T(L"No original wallpaper was saved yet.", L"Henüz kaydedilmiş bir özgün duvar kağıdı yok."));
                    break;
            }
            return 0;
        }

        case WM_TIMER:
            if (wp == 1) {
                KillTimer(hwnd, 1);
                g_refreshPending = false;
                UpdatePreview();
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            g_refreshPending = false;
            SaveParams(g_p);
            FreePreview();
            g_bg.Destroy();
            if (g_font) {
                DeleteObject(g_font);
                g_font = nullptr;
            }
            g_hotBtn = nullptr;
            g_dragging = false;
            g_editor = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void OpenEditor() {
    if (g_editor) {
        if (IsIconic(g_editor)) ShowWindow(g_editor, SW_RESTORE);
        SetForegroundWindow(g_editor);
        return;
    }

    HDC scr = GetDC(nullptr);
    g_dpi = GetDeviceCaps(scr, LOGPIXELSX);
    ReleaseDC(nullptr, scr);
    if (g_dpi < 96) g_dpi = 96;

    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rc = {0, 0, SC(kClientW), SC(kClientH)};
    AdjustWindowRectEx(&rc, style, FALSE, 0);
    const int w = rc.right - rc.left, h = rc.bottom - rc.top;
    const int x = std::max(0, (GetSystemMetrics(SM_CXSCREEN) - w) / 2);
    const int y = std::max(0, (GetSystemMetrics(SM_CYSCREEN) - h) / 2);

    HWND wnd = CreateWindowExW(0, L"WallpaperLineStudioEditor",
                               T(L"Minimal Pattern Wallpapers", L"Sade Desenli Duvar Kağıtları"),
                               style, x, y, w, h, nullptr, nullptr,
                               GetModuleHandleW(nullptr), nullptr);
    if (!wnd) {
        Wh_Log(L"Editor window creation failed: %lu", GetLastError());
        return;
    }
    ShowWindow(wnd, SW_SHOW);
    SetForegroundWindow(wnd);
}

// ---------------------------------------------------------------------------
// Tray icon and hidden main window
// ---------------------------------------------------------------------------

static bool g_balloonPending = false;
static int g_trayRetries = 0;

static HICON MakeTrayIcon() {
    const int N = 32;
    HDC scr = GetDC(nullptr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = N;
    bi.bmiHeader.biHeight = -N;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, scr);
    if (!color || !bits) {
        if (color) DeleteObject(color);
        return nullptr;
    }
    uint32_t* px = (uint32_t*)bits;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            // Dark rounded square with light diagonal stripes.
            float dx = fabsf(x + 0.5f - N / 2.0f), dy = fabsf(y + 0.5f - N / 2.0f);
            bool inside = dx < 15.0f && dy < 15.0f &&
                          !(dx > 11.0f && dy > 11.0f && (dx - 11.0f) + (dy - 11.0f) > 4.0f);
            uint32_t v = 0;
            if (inside) {
                bool stripe = ((x + y) % 8) < 3;
                uint32_t g = stripe ? 190 : 52;
                v = 0xFF000000u | (g << 16) | (g << 8) | g;
            }
            px[y * N + x] = v;
        }
    }
    static const uint8_t zeros[N * 4] = {0};
    HBITMAP mask = CreateBitmap(N, N, 1, 1, zeros);
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = color;
    ii.hbmMask = mask;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(color);
    DeleteObject(mask);
    return icon;
}

static BOOL AddTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = g_trayIcon ? g_trayIcon : LoadIconW(nullptr, IDI_APPLICATION);
    lstrcpynW(nid.szTip, T(L"Minimal Pattern Wallpapers", L"Sade Desenli Duvar Kağıtları"), ARRAYSIZE(nid.szTip));
    return Shell_NotifyIconW(NIM_ADD, &nid);
}

static void RemoveTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

// Small notification that tells the user where the mod lives.
static void ShowBalloon(HWND hwnd) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
    lstrcpynW(nid.szInfoTitle, T(L"Minimal Pattern Wallpapers", L"Sade Desenli Duvar Kağıtları"), ARRAYSIZE(nid.szInfoTitle));
    lstrcpynW(nid.szInfo,
              T(L"Running in the notification area. Click the icon to open the editor.",
                L"Bildirim alanında çalışıyor. Düzenleyiciyi açmak için simgeye tıklayın."),
              ARRAYSIZE(nid.szInfo));
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void ShowPendingBalloon(HWND hwnd) {
    if (!g_balloonPending) return;
    g_balloonPending = false;
    ShowBalloon(hwnd);
}

static void RandomizeAndApply() {
    Randomize(g_p);
    int W = 0, H = 0;
    ApplyWallpaper(W, H);
    SaveParams(g_p);
    if (g_editor) {
        SyncControls();
        SendDlgItemMessageW(g_editor, IDC_PRESET, CB_SETCURSEL, (WPARAM)-1, 0);
        UpdatePreview();
    }
}

static void ShowTrayMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, T(L"Open editor", L"Düzenleyiciyi aç"));
    AppendMenuW(menu, MF_STRING, 2, T(L"Randomize and apply", L"Rastgele üret ve uygula"));
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 3, T(L"Restore original wallpaper", L"Özgün duvar kağıdını geri yükle"));
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    int cmd = (int)TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    if (cmd == 1) OpenEditor();
    else if (cmd == 2) RandomizeAndApply();
    else if (cmd == 3) RestoreOriginalWallpaper();
}

static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_wmTaskbarCreated && msg == g_wmTaskbarCreated) {
        if (AddTrayIcon(hwnd)) ShowPendingBalloon(hwnd);
        return 0;
    }
    switch (msg) {
        case WM_APP_TRAY:
            if (lp == WM_LBUTTONUP || lp == WM_LBUTTONDBLCLK || lp == NIN_BALLOONUSERCLICK) OpenEditor();
            else if (lp == WM_RBUTTONUP) ShowTrayMenu(hwnd);
            return 0;

        case WM_TIMER:
            // The taskbar may not exist yet right after sign-in: keep trying.
            if (wp == 7) {
                if (AddTrayIcon(hwnd)) {
                    KillTimer(hwnd, 7);
                    ShowPendingBalloon(hwnd);
                } else if (++g_trayRetries > 60) {
                    KillTimer(hwnd, 7);
                }
            }
            return 0;

        case WM_APP_SETTINGS: {
            const int oldLang = g_lang;
            const int oldBackdrop = g_backdrop;
            LoadModSettings();
            RemoveTrayIcon(hwnd);
            AddTrayIcon(hwnd);
            if (g_editor) {
                if (g_lang != oldLang) {
                    DestroyWindow(g_editor);
                    OpenEditor();
                } else if (g_backdrop != oldBackdrop) {
                    ApplyTheme(g_editor);
                }
            }
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 7);
            RemoveTrayIcon(hwnd);
            if (g_editor) DestroyWindow(g_editor);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Worker thread and tool mod entry points
// ---------------------------------------------------------------------------

static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_initOk = false;

static void SignalReady(bool ok) {
    g_initOk = ok;
    if (g_readyEvent) SetEvent(g_readyEvent);
}

static const wchar_t* kClsMain = L"WallpaperLineStudioMain";
static const wchar_t* kClsEditor = L"WallpaperLineStudioEditor";
static const wchar_t* kClsPreview = L"WLSPreview";
static const wchar_t* kClsSlider = L"WLSSlider";

static void UnregisterClasses(HINSTANCE inst) {
    UnregisterClassW(kClsMain, inst);
    UnregisterClassW(kClsEditor, inst);
    UnregisterClassW(kClsPreview, inst);
    UnregisterClassW(kClsSlider, inst);
}

static void FreeThemeObjects() {
    if (g_brCard) DeleteObject(g_brCard);
    if (g_brBtn) DeleteObject(g_brBtn);
    g_brCard = g_brBtn = nullptr;
}

// Sends the mouse wheel to the preview or a slider under the pointer, whichever
// control currently has the keyboard focus.
static bool RouteWheel(const MSG& msg) {
    if (!g_editor) return false;
    POINT pt = {GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)};
    HWND under = WindowFromPoint(pt);
    if (!under || GetParent(under) != g_editor) return false;
    const int id = GetDlgCtrlID(under);
    const bool isSlider = id >= IDC_SL_FIRST && id < IDC_SL_FIRST + kSliderCount;
    if (id != IDC_PREVIEW && !isSlider) return false;
    SendMessageW(under, WM_MOUSEWHEEL, msg.wParam, msg.lParam);
    return true;
}

static DWORD WINAPI Worker(LPVOID) {
    srand((unsigned)GetTickCount64());

    // Work in real pixels (the screen size and the editor layout depend on it).
    typedef DPI_AWARENESS_CONTEXT(WINAPI * SetThreadDpiCtx_t)(DPI_AWARENESS_CONTEXT);
    SetThreadDpiCtx_t pSetDpi = (SetThreadDpiCtx_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext");
    if (pSetDpi) pSetDpi((DPI_AWARENESS_CONTEXT)-4);

    InitDynApis();

    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    HINSTANCE inst = GetModuleHandleW(nullptr);

    WNDCLASSW mainWc = {};
    mainWc.lpfnWndProc = MainProc;
    mainWc.hInstance = inst;
    mainWc.lpszClassName = kClsMain;
    WNDCLASSW edWc = {};
    edWc.lpfnWndProc = EditorProc;
    edWc.hInstance = inst;
    edWc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    edWc.lpszClassName = kClsEditor;
    WNDCLASSW pvWc = {};
    pvWc.style = CS_DBLCLKS;
    pvWc.lpfnWndProc = PreviewProc;
    pvWc.hInstance = inst;
    pvWc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    pvWc.lpszClassName = kClsPreview;
    WNDCLASSW slWc = {};
    slWc.lpfnWndProc = SliderProc;
    slWc.hInstance = inst;
    slWc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    slWc.lpszClassName = kClsSlider;

    if (!RegisterClassW(&mainWc) || !RegisterClassW(&edWc) ||
        !RegisterClassW(&pvWc) || !RegisterClassW(&slWc)) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        UnregisterClasses(inst);
        FreeDynApis();
        SignalReady(false);
        return 1;
    }

    g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    g_trayIcon = MakeTrayIcon();

    HWND main = CreateWindowExW(WS_EX_TOOLWINDOW, kClsMain, L"", WS_POPUP, 0, 0,
                                0, 0, nullptr, nullptr, inst, nullptr);
    if (!main) {
        Wh_Log(L"Main window creation failed: %lu", GetLastError());
        if (g_trayIcon) DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
        UnregisterClasses(inst);
        FreeDynApis();
        SignalReady(false);
        return 1;
    }
    g_msgWnd = main;

    // Start quietly in the notification area; the window only opens on request.
    g_balloonPending = g_showBalloon && !g_openOnStart;
    if (AddTrayIcon(main)) {
        ShowPendingBalloon(main);
    } else {
        SetTimer(main, 7, 1500, nullptr);
    }
    if (g_openOnStart) OpenEditor();
    SignalReady(true);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_MOUSEWHEEL && RouteWheel(msg)) continue;
        if (g_editor && IsDialogMessageW(g_editor, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_msgWnd = nullptr;
    if (g_trayIcon) DestroyIcon(g_trayIcon);
    g_trayIcon = nullptr;
    FreeThemeObjects();
    UnregisterClasses(inst);
    FreeDynApis();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Minimal Pattern Wallpapers starting");
    LoadModSettings();
    LoadParams(g_p);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Wait until the worker has finished initializing (it signals on failure too).
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (!g_initOk) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // The settings are re-read on the worker thread.
    HWND w = g_msgWnd.load();
    if (w) PostMessageW(w, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"Minimal Pattern Wallpapers stopping");
    HWND w = g_msgWnd.load();
    if (w) PostMessageW(w, WM_CLOSE, 0, 0);
    if (g_thread) {
        // Wait for the worker: it removes the tray icon and closes the editor
        // before the mod is unloaded.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

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
