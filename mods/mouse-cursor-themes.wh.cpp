// ==WindhawkMod==
// @id              mouse-cursor-themes
// @name            Mouse Cursor Themes
// @name:tr-TR      Fare İmleci Temaları
// @description     Replaces the Windows mouse cursors with built-in themes (neon, classic, sunset, ocean, gold, rainbow and more) that are drawn by the mod itself, including animated busy cursors and several pointer shapes (arrow, triangle, diamond, dot and more)
// @description:tr-TR Windows fare imleçlerini modun kendi çizdiği hazır temalarla (neon, klasik, gün batımı, okyanus, altın, gökkuşağı ve daha fazlası) değiştirir; animasyonlu meşgul imleçleri ve birkaç farklı işaretçi şekli (ok, üçgen, elmas, nokta ve daha fazlası) de dahildir
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mouse Cursor Themes

Replaces the Windows mouse cursors with a theme of your choice. All cursors are
drawn by the mod itself, so every theme is available right after you enable the
mod and you can switch themes from the settings at any time.

The mod draws cursor files for the theme and replaces the system cursors of
your current Windows session with them. It does not change your saved pointer
scheme or any registry settings. Because of this, the theme works in every
program that uses the standard Windows cursors.

## Preview
![Preview 1](https://i.imgur.com/YBKMSqy.gif)

![Preview 2](https://i.imgur.com/oIhV764.gif)

![Preview 3](https://i.imgur.com/lO9i7j0.gif)

## Themes
- **Classic White**: White arrow with a black outline.
- **Classic Black**: Black arrow with a white outline.
- **Neon Blue / Neon Pink / Neon Green**: Bright outline with a glowing halo.
- **Sunset**: Orange to pink gradient with a soft warm glow.
- **Ocean**: Aqua to deep blue gradient.
- **Gold**: Golden gradient with a dark brown outline.
- **Rainbow**: A rainbow-colored cursor.
- **Crimson**: Red cursor with a white outline and a red glow.
- **Silver**: Metallic silver gradient with a dark outline.
- **Forest**: Light green to deep green gradient with a dark green outline.
- **Lava**: Yellow to red-orange gradient with a hot orange glow.
- **Lavender**: Soft lilac to purple gradient.
- **Aurora**: Mint to violet gradient with a cool glow.
- **Ice**: White to icy blue gradient with a blue outline.
- **Candy**: Pink to sky blue gradient with a white outline.
- **Midnight**: Dark navy cursor with a golden outline and glow.
- **Ghost**: Only an outline, the inside is transparent.
- **Custom**: Pick your own fill, outline and glow colors.

## Pointer shapes
The shape of the main pointer can be changed independently from the theme. The
same shape is also used by the busy (working in background) and help cursors.
Shapes marked "tip" click with their tip, the others click at their center.

- **Classic arrow**: The standard Windows arrow (tip).
- **Arrowhead**: The classic arrow without its stem (tip).
- **Triangle**: A plain triangle (tip).
- **Wedge**: A slim triangle with a straight left edge (tip).
- **Kite**: A kite shape (tip).
- **Navigator**: A sharp navigation-style arrowhead (tip).
- **Chevron**: Two strokes forming an open corner (tip).
- **Diamond / Square / Dot / Ring / Target**: Simple geometric shapes.
- **Star / Sparkle / Heart**: Decorative shapes.

## Cursors that are replaced
Normal arrow, text (I-beam), link (hand), busy (animated), working in
background (animated), precision cross, up arrow, all resize cursors, move,
unavailable (forbidden) and help.

## Settings
- Theme
- Pointer shape: Shape of the main pointer (see above).
- Cursor size (%): 100 matches the original Windows cursor size.
- Glow (%): Makes the glow of the themes weaker or stronger (0 turns it off).
- Outline thickness (%): Makes the outline of the cursor thinner or thicker. A thicker outline is easier to see.
- Crisp edges: Draws sharper, less blurry cursor edges.
- Contrast edge: Adds a thin dark or light line around the outline, so the cursor stays clearly visible on any background.
- Animated busy cursors: Turns the spinning busy cursors on or off.
- Custom colors: Used only when the "Custom" theme is selected (format: #RRGGBB).

## Notes
- The cursor size follows the Windows pointer size setting (Settings >
  Accessibility > Mouse pointer and touch). The cursors are drawn in several sizes, so they stay sharp.
- The mod never changes your saved pointer scheme. When you disable the mod,
  Windows reloads your own pointer scheme. The replaced cursors only live in
  memory, so after you sign out they return to your pointer scheme until the
  mod is started again.
- Programs that use their own cursors (for example some games and drawing
  programs) keep their own cursors.
- If you change the pointer scheme from the Windows settings while the mod is
  running, the mod notices it and applies the theme again.
- The generated cursor files are kept in Windhawk's storage folder for this mod
  and are removed when the mod is disabled.

---

# Fare İmleci Temaları

Windows fare imleçlerini seçtiğiniz bir temayla değiştirir. Tüm imleçler modun
kendisi tarafından çizilir; bu yüzden modu açtığınız anda bütün temalar hazırdır
ve ayarlardan istediğiniz zaman tema değiştirebilirsiniz.

Mod, tema için imleç dosyaları çizer ve geçerli Windows oturumunuzun sistem
imleçlerini bu dosyalarla değiştirir. Kayıtlı işaretçi şemanızı veya kayıt defteri
ayarlarını değiştirmez. Bu sayede tema, standart Windows imleçlerini kullanan her
programda çalışır.

## Temalar
- **Klasik Beyaz**: Siyah çerçeveli beyaz ok.
- **Klasik Siyah**: Beyaz çerçeveli siyah ok.
- **Neon Mavi / Neon Pembe / Neon Yeşil**: Parlak çerçeve ve ışıldayan halo.
- **Gün Batımı**: Turuncudan pembeye geçişli, yumuşak sıcak parıltılı.
- **Okyanus**: Açık turkuazdan koyu maviye geçişli.
- **Altın**: Koyu kahverengi çerçeveli altın renkli geçiş.
- **Gökkuşağı**: Gökkuşağı renklerinde imleç.
- **Kızıl**: Beyaz çerçeveli, kırmızı parıltılı kırmızı imleç.
- **Gümüş**: Koyu çerçeveli, metalik gümüş geçişli.
- **Orman**: Açık yeşilden koyu yeşile geçişli, koyu yeşil çerçeveli.
- **Lav**: Sarıdan kırmızı-turuncuya geçişli, sıcak turuncu parıltılı.
- **Lavanta**: Yumuşak leylaktan mora geçişli.
- **Aurora**: Naneden menekşeye geçişli, serin parıltılı.
- **Buz**: Beyazdan buz mavisine geçişli, mavi çerçeveli.
- **Şeker**: Pembeden gök mavisine geçişli, beyaz çerçeveli.
- **Gece Yarısı**: Koyu lacivert, altın sarısı çerçeveli ve parıltılı.
- **Hayalet**: Sadece çerçeve, içi şeffaf.
- **Özel**: Dolgu, çerçeve ve parıltı renklerini kendiniz seçersiniz.

## İşaretçi şekilleri
Ana işaretçinin şekli temadan bağımsız olarak değiştirilebilir. Aynı şekil,
meşgul (arka planda çalışıyor) ve yardım imleçlerinde de kullanılır.
"uç" yazan şekiller uç noktalarıyla, diğerleri merkezleriyle tıklar.

- **Klasik ok**: Standart Windows oku (uç).
- **Ok başı**: Klasik okun çubuksuz hâli (uç).
- **Üçgen**: Sade bir üçgen (uç).
- **Kama**: Sol kenarı düz, ince bir üçgen (uç).
- **Uçurtma**: Uçurtma biçimi (uç).
- **Pusula oku**: Keskin, navigasyon tarzı ok başı (uç).
- **Köşe**: Açık bir köşe oluşturan iki çizgi (uç).
- **Elmas / Kare / Nokta / Halka / Hedef**: Sade geometrik şekiller.
- **Yıldız / Parıltı / Kalp**: Süslü şekiller.

## Değiştirilen imleçler
Normal ok, metin (I imleci), bağlantı (el), meşgul (animasyonlu), arka planda
çalışıyor (animasyonlu), hassas artı, yukarı ok, tüm boyutlandırma imleçleri,
taşıma, kullanılamaz (yasak) ve yardım.

## Ayarlar
- Tema
- İşaretçi şekli: Ana işaretçinin şekli (yukarıya bakın).
- İmleç boyutu (%): 100, orijinal Windows imleç boyutuna denk gelir.
- Parıltı (%): Temaların parıltısını azaltır veya artırır (0 kapatır).
- Çerçeve kalınlığı (%): İmlecin çerçevesini inceltir veya kalınlaştırır. Kalın çerçeve imleci daha kolay görünür yapar.
- Keskin kenarlar: İmleç kenarlarını daha keskin, daha az bulanık çizer.
- Kontrast kenarı: Çerçevenin etrafına ince bir koyu veya açık çizgi ekler; böylece imleç her arka planda net görünür.
- Animasyonlu meşgul imleçleri: Dönen meşgul imleçlerini açar veya kapatır.
- Özel renkler: Sadece "Özel" tema seçiliyse kullanılır (biçim: #RRGGBB).

## Notlar
- İmleç boyutu Windows'un işaretçi boyutu ayarını izler (Ayarlar > Erişilebilirlik >
  Fare işaretçisi ve dokunma). İmleçler birkaç boyutta çizildiği için net kalır.
- Mod kayıtlı işaretçi şemanızı hiçbir zaman değiştirmez. Modu kapattığınızda Windows
  kendi işaretçi şemanızı yeniden yükler. Değiştirilen imleçler yalnızca bellekte
  bulunur; oturumu kapattığınızda mod yeniden başlayana kadar işaretçi şemanıza döner.
- Kendi imleçlerini kullanan programlar (örneğin bazı oyunlar ve çizim programları)
  kendi imleçlerini korur.
- Mod çalışırken Windows ayarlarından işaretçi şemasını değiştirirseniz mod bunu
  fark eder ve temayı yeniden uygular.
- Oluşturulan imleç dosyaları, Windhawk'ın bu mod için ayırdığı depolama klasöründe
  tutulur ve mod kapatılınca silinir.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: neon_blue
  $name: Theme
  $name:tr-TR: Tema
  $options:
  - classic_white: Classic White
  - classic_black: Classic Black
  - neon_blue: Neon Blue
  - neon_pink: Neon Pink
  - neon_green: Neon Green
  - sunset: Sunset
  - ocean: Ocean
  - gold: Gold
  - rainbow: Rainbow
  - crimson: Crimson
  - silver: Silver
  - forest: Forest
  - lava: Lava
  - lavender: Lavender
  - aurora: Aurora
  - ice: Ice
  - candy: Candy
  - midnight: Midnight
  - ghost: Ghost (outline only)
  - custom: Custom (use the colors below)
  $options:tr-TR:
  - classic_white: Klasik Beyaz
  - classic_black: Klasik Siyah
  - neon_blue: Neon Mavi
  - neon_pink: Neon Pembe
  - neon_green: Neon Yeşil
  - sunset: Gün Batımı
  - ocean: Okyanus
  - gold: Altın
  - rainbow: Gökkuşağı
  - crimson: Kızıl
  - silver: Gümüş
  - forest: Orman
  - lava: Lav
  - lavender: Lavanta
  - aurora: Aurora
  - ice: Buz
  - candy: Şeker
  - midnight: Gece Yarısı
  - ghost: Hayalet (sadece çerçeve)
  - custom: Özel (aşağıdaki renkleri kullanır)
- arrowStyle: classic
  $name: Pointer shape
  $name:tr-TR: İşaretçi şekli
  $options:
  - classic: Classic arrow
  - head: Arrowhead (no stem)
  - triangle: Triangle
  - wedge: Wedge (slim triangle)
  - kite: Kite
  - navigator: Navigator arrowhead
  - chevron: Chevron (open corner)
  - diamond: Diamond
  - square: Square
  - dot: Dot
  - ring: Ring
  - target: Target (ring with dot)
  - star: Star
  - sparkle: Sparkle (4-point star)
  - heart: Heart
  $options:tr-TR:
  - classic: Klasik ok
  - head: Ok başı (çubuksuz)
  - triangle: Üçgen
  - wedge: Kama (ince üçgen)
  - kite: Uçurtma
  - navigator: Pusula oku
  - chevron: Köşe (açık ok)
  - diamond: Elmas
  - square: Kare
  - dot: Nokta
  - ring: Halka
  - target: Hedef (içinde nokta olan halka)
  - star: Yıldız
  - sparkle: Parıltı (4 köşeli yıldız)
  - heart: Kalp
- cursorScale: 100
  $name: Cursor size (%)
  $name:tr-TR: İmleç boyutu (%)
  $description: 100 matches the size of the original Windows cursors. The Windows pointer size setting is applied on top of this.
  $description:tr-TR: 100, orijinal Windows imleçlerinin boyutuna denk gelir. Windows'un işaretçi boyutu ayarı bunun üzerine uygulanır.
- glow: 100
  $name: Glow (%)
  $name:tr-TR: Parıltı (%)
  $description: 100 is the default. Use 0 to turn the glow off, a higher value for a stronger glow.
  $description:tr-TR: 100 varsayılan. Parıltıyı kapatmak için 0, daha güçlü parıltı için daha büyük bir değer girin.
- outlineThickness: 100
  $name: Outline thickness (%)
  $name:tr-TR: Çerçeve kalınlığı (%)
  $description: 100 is the default. A thicker outline makes the cursor easier to see. Allowed range is 50 to 250.
  $description:tr-TR: 100 varsayılan. Daha kalın çerçeve imleci daha kolay görünür yapar. İzin verilen aralık 50 ile 250 arasıdır.
- crispEdges: true
  $name: Crisp edges
  $name:tr-TR: Keskin kenarlar
  $description: Draws sharper, less blurry cursor edges.
  $description:tr-TR: İmleç kenarlarını daha keskin, daha az bulanık çizer.
- contrastEdge: true
  $name: Contrast edge
  $name:tr-TR: Kontrast kenarı
  $description: Adds a thin dark or light line around the outline, so the cursor stays clearly visible on any background.
  $description:tr-TR: Çerçevenin etrafına ince bir koyu veya açık çizgi ekler; böylece imleç her arka planda net görünür.
- animateBusy: true
  $name: Animated busy cursors
  $name:tr-TR: Animasyonlu meşgul imleçleri
  $description: Turns the spinning busy and working-in-background cursors on or off.
  $description:tr-TR: Dönen meşgul ve arka planda çalışıyor imleçlerini açar veya kapatır.
- customFill: "#ffffff"
  $name: Custom theme - fill color
  $name:tr-TR: Özel tema - dolgu rengi
  $description: "Use the #RRGGBB format."
  $description:tr-TR: "#RRGGBB biçiminde."
- customOutline: "#000000"
  $name: Custom theme - outline color
  $name:tr-TR: Özel tema - çerçeve rengi
  $description: "Use the #RRGGBB format."
  $description:tr-TR: "#RRGGBB biçiminde."
- customGlow: "#00aaff"
  $name: Custom theme - glow color
  $name:tr-TR: Özel tema - parıltı rengi
  $description: "Use the #RRGGBB format."
  $description:tr-TR: "#RRGGBB biçiminde."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <wchar.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Temalar ve ayarlar
// ---------------------------------------------------------------------------

struct Col {
    float r, g, b;
};

struct Theme {
    const wchar_t* id;
    Col fill1, fill2;  // dolgu (yukarıdan aşağı geçiş)
    Col outline;       // çerçeve rengi
    Col glow;          // parıltı rengi
    float outlineW;    // çerçeve kalınlığı (32 birimlik tasarım alanında)
    float glowStrength;
    float glowR;
    float fillAlpha;
    bool rainbow;
};

// Not: İlk elemanların sırası (özellikle neon_blue = 2) değiştirilmemelidir;
// yeni temalar "custom" girdisinden önce eklenir.
static const Theme kThemes[] = {
    {L"classic_white", {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0},
     1.4f, 0.0f, 2.0f, 1.0f, false},
    {L"classic_black", {20, 20, 20}, {20, 20, 20}, {255, 255, 255}, {0, 0, 0},
     1.4f, 0.0f, 2.0f, 1.0f, false},
    {L"neon_blue", {225, 250, 255}, {225, 250, 255}, {0, 200, 255},
     {0, 160, 255}, 1.1f, 0.85f, 2.8f, 1.0f, false},
    {L"neon_pink", {255, 232, 248}, {255, 232, 248}, {255, 50, 170},
     {255, 0, 140}, 1.1f, 0.85f, 2.8f, 1.0f, false},
    {L"neon_green", {12, 30, 12}, {12, 30, 12}, {57, 255, 20}, {57, 255, 20},
     1.1f, 0.80f, 2.8f, 1.0f, false},
    {L"sunset", {255, 200, 60}, {255, 60, 120}, {255, 255, 255}, {255, 90, 80},
     1.1f, 0.50f, 2.4f, 1.0f, false},
    {L"ocean", {130, 255, 225}, {20, 110, 255}, {0, 35, 90}, {0, 150, 255},
     1.3f, 0.30f, 2.4f, 1.0f, false},
    {L"gold", {255, 238, 130}, {205, 140, 20}, {70, 40, 0}, {255, 200, 0},
     1.3f, 0.35f, 2.4f, 1.0f, false},
    {L"rainbow", {255, 0, 0}, {255, 0, 0}, {25, 25, 25}, {255, 255, 255}, 1.3f,
     0.25f, 2.2f, 1.0f, true},
    {L"crimson", {225, 25, 45}, {150, 0, 20}, {255, 255, 255}, {255, 30, 50},
     1.2f, 0.60f, 2.6f, 1.0f, false},
    {L"silver", {250, 250, 252}, {150, 158, 170}, {35, 40, 50}, {160, 170, 190},
     1.3f, 0.20f, 2.2f, 1.0f, false},
    {L"forest", {150, 235, 120}, {20, 110, 50}, {10, 50, 25}, {60, 200, 90},
     1.3f, 0.30f, 2.4f, 1.0f, false},
    {L"lava", {255, 235, 70}, {235, 40, 10}, {60, 10, 0}, {255, 110, 0}, 1.3f,
     0.55f, 2.6f, 1.0f, false},
    {L"lavender", {240, 225, 255}, {150, 100, 235}, {60, 30, 120},
     {170, 120, 255}, 1.3f, 0.35f, 2.4f, 1.0f, false},
    {L"aurora", {90, 255, 200}, {150, 80, 255}, {10, 20, 60}, {80, 200, 255},
     1.2f, 0.50f, 2.6f, 1.0f, false},
    {L"ice", {255, 255, 255}, {150, 215, 255}, {20, 90, 170}, {120, 200, 255},
     1.3f, 0.40f, 2.4f, 1.0f, false},
    {L"candy", {255, 150, 210}, {120, 210, 255}, {255, 255, 255},
     {255, 120, 200}, 1.2f, 0.45f, 2.5f, 1.0f, false},
    {L"midnight", {30, 40, 90}, {10, 10, 35}, {255, 215, 100}, {255, 190, 60},
     1.2f, 0.45f, 2.5f, 1.0f, false},
    {L"ghost", {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {0, 0, 0},
     1.5f, 0.70f, 2.0f, 0.0f, false},
    {L"custom", {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 170, 255},
     1.3f, 0.60f, 2.6f, 1.0f, false},
};

static Theme g_theme = kThemes[2];
static int g_glowPct = 100;
static bool g_animBusy = true;
static int g_outlinePct = 100;       // çerçeve kalınlığı (%)
static bool g_crisp = true;          // keskin kenarlar
static bool g_contrastEdge = true;   // çerçevenin dışında ince kontrast çizgisi

// İşaretçi şekilleri (ayar kimlikleri sırayla).
enum ArrowStyle {
    AS_CLASSIC = 0,
    AS_HEAD,
    AS_TRIANGLE,
    AS_WEDGE,
    AS_KITE,
    AS_NAVIGATOR,
    AS_CHEVRON,
    AS_DIAMOND,
    AS_SQUARE,
    AS_DOT,
    AS_RING,
    AS_TARGET,
    AS_STAR,
    AS_SPARKLE,
    AS_HEART,
    AS_COUNT,
};
static const wchar_t* kArrowStyles[AS_COUNT] = {
    L"classic", L"head",    L"triangle", L"wedge",   L"kite",
    L"navigator", L"chevron", L"diamond", L"square", L"dot",
    L"ring",    L"target",  L"star",     L"sparkle", L"heart"};
static int g_arrowStyle = AS_CLASSIC;
static float g_scale = 1.0f;  // 1.0 = orijinal Windows imleç boyutu

static Col ParseHex(PCWSTR name, Col def) {
    Col c = def;
    PCWSTR s = Wh_GetStringSetting(name);
    if (s) {
        const wchar_t* p = s;
        if (*p == L'#') p++;
        if (wcslen(p) == 6) {
            unsigned long v = wcstoul(p, nullptr, 16);
            c = {(float)((v >> 16) & 255), (float)((v >> 8) & 255),
                 (float)(v & 255)};
        }
        Wh_FreeStringSetting(s);
    }
    return c;
}

static void LoadSettings() {
    const Theme* sel = &kThemes[2];
    PCWSTR t = Wh_GetStringSetting(L"theme");
    if (t) {
        for (const Theme& th : kThemes) {
            if (!wcscmp(t, th.id)) sel = &th;
        }
        Wh_FreeStringSetting(t);
    }

    Theme T = *sel;
    if (!wcscmp(T.id, L"custom")) {
        Col fill = ParseHex(L"customFill", {255, 255, 255});
        T.fill1 = fill;
        T.fill2 = fill;
        T.outline = ParseHex(L"customOutline", {0, 0, 0});
        T.glow = ParseHex(L"customGlow", {0, 170, 255});
    }
    g_theme = T;

    g_glowPct = std::clamp(Wh_GetIntSetting(L"glow"), 0, 200);
    g_animBusy = Wh_GetIntSetting(L"animateBusy") != 0;
    g_scale = std::clamp(Wh_GetIntSetting(L"cursorScale"), 50, 200) / 100.0f;
    g_outlinePct = std::clamp(Wh_GetIntSetting(L"outlineThickness"), 50, 250);
    g_crisp = Wh_GetIntSetting(L"crispEdges") != 0;
    g_contrastEdge = Wh_GetIntSetting(L"contrastEdge") != 0;

    g_arrowStyle = AS_CLASSIC;
    PCWSTR as = Wh_GetStringSetting(L"arrowStyle");
    if (as) {
        for (int i = 0; i < AS_COUNT; i++) {
            if (!wcscmp(as, kArrowStyles[i])) g_arrowStyle = i;
        }
        Wh_FreeStringSetting(as);
    }
}

// ---------------------------------------------------------------------------
// Yardımcılar
// ---------------------------------------------------------------------------

static inline float Clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

static inline uint32_t Cl8(float v) {
    int i = (int)(v + 0.5f);
    return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
}

static Col Hue(float h) {
    h = h - floorf(h);
    float r = Clamp01(fabsf(h * 6 - 3) - 1);
    float g = Clamp01(2 - fabsf(h * 6 - 2));
    float b = Clamp01(2 - fabsf(h * 6 - 4));
    return {r * 255, g * 255, b * 255};
}

// ---------------------------------------------------------------------------
// Şekiller (32x32 birimlik tasarım alanında, uzaklık alanları ile)
// ---------------------------------------------------------------------------

struct Pt {
    float x, y;
};

struct Prim {
    int t = 0;  // 0: kapsül, 1: dolu daire, 2: halka, 3: çokgen
    float x1 = 0, y1 = 0, x2 = 0, y2 = 0, r = 0;
    std::vector<Pt> pts;
};

typedef std::vector<Prim> Shape;

static void AddCap(Shape& s, float x1, float y1, float x2, float y2, float r) {
    Prim p;
    p.t = 0;
    p.x1 = x1;
    p.y1 = y1;
    p.x2 = x2;
    p.y2 = y2;
    p.r = r;
    s.push_back(p);
}

static void AddCircle(Shape& s, float cx, float cy, float r) {
    Prim p;
    p.t = 1;
    p.x1 = cx;
    p.y1 = cy;
    p.r = r;
    s.push_back(p);
}

static void AddRing(Shape& s, float cx, float cy, float r, float hw) {
    Prim p;
    p.t = 2;
    p.x1 = cx;
    p.y1 = cy;
    p.r = r;
    p.x2 = hw;
    s.push_back(p);
}

static void AddPoly(Shape& s, std::initializer_list<Pt> pts) {
    Prim p;
    p.t = 3;
    p.pts.assign(pts.begin(), pts.end());
    s.push_back(p);
}

static void AddArc(Shape& s, float cx, float cy, float r, float a0, float a1,
                   float hw, int seg) {
    for (int i = 0; i < seg; i++) {
        float t0 = a0 + (a1 - a0) * i / seg;
        float t1 = a0 + (a1 - a0) * (i + 1) / seg;
        AddCap(s, cx + r * cosf(t0), cy + r * sinf(t0), cx + r * cosf(t1),
               cy + r * sinf(t1), hw);
    }
}

static inline float SegDist(float px, float py, float ax, float ay, float bx,
                            float by) {
    float dx = bx - ax, dy = by - ay;
    float l2 = dx * dx + dy * dy;
    float t = l2 > 0 ? ((px - ax) * dx + (py - ay) * dy) / l2 : 0;
    t = Clamp01(t);
    float qx = ax + dx * t - px, qy = ay + dy * t - py;
    return sqrtf(qx * qx + qy * qy);
}

// İşaretli uzaklık: içeride negatif, dışarıda pozitif.
static float PrimSD(const Prim& p, float x, float y) {
    switch (p.t) {
        case 0:
            return SegDist(x, y, p.x1, p.y1, p.x2, p.y2) - p.r;
        case 1: {
            float dx = x - p.x1, dy = y - p.y1;
            return sqrtf(dx * dx + dy * dy) - p.r;
        }
        case 2: {
            float dx = x - p.x1, dy = y - p.y1;
            return fabsf(sqrtf(dx * dx + dy * dy) - p.r) - p.x2;
        }
        default: {
            size_t n = p.pts.size();
            float d = 1e9f;
            bool inside = false;
            for (size_t i = 0, j = n - 1; i < n; j = i++) {
                const Pt& a = p.pts[i];
                const Pt& b = p.pts[j];
                d = fminf(d, SegDist(x, y, a.x, a.y, b.x, b.y));
                if ((a.y > y) != (b.y > y) &&
                    x < (b.x - a.x) * (y - a.y) / (b.y - a.y) + a.x) {
                    inside = !inside;
                }
            }
            return inside ? -d : d;
        }
    }
}

enum Kind {
    K_ARROW = 0,
    K_IBEAM,
    K_WAIT,
    K_CROSS,
    K_UP,
    K_SIZENWSE,
    K_SIZENESW,
    K_SIZEWE,
    K_SIZENS,
    K_SIZEALL,
    K_NO,
    K_HAND,
    K_APPSTART,
    K_HELP,
};

// Cursor names (used for file names) and the OCR_* ids of the system cursors.
struct CurDef {
    const wchar_t* valueName;
    int kind;
    bool animated;
    UINT ocr;  // OCR_* kimliği (yedek yöntem için)
};

static const CurDef kDefs[] = {
    {L"Arrow", K_ARROW, false, 32512},
    {L"IBeam", K_IBEAM, false, 32513},
    {L"Wait", K_WAIT, true, 32514},
    {L"Crosshair", K_CROSS, false, 32515},
    {L"UpArrow", K_UP, false, 32516},
    {L"SizeNWSE", K_SIZENWSE, false, 32642},
    {L"SizeNESW", K_SIZENESW, false, 32643},
    {L"SizeWE", K_SIZEWE, false, 32644},
    {L"SizeNS", K_SIZENS, false, 32645},
    {L"SizeAll", K_SIZEALL, false, 32646},
    {L"No", K_NO, false, 32648},
    {L"Hand", K_HAND, false, 32649},
    {L"AppStarting", K_APPSTART, true, 32650},
    {L"Help", K_HELP, false, 32651},
};

static void AddClassicArrow(Shape& s, float ox, float oy) {
    AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                {ox + 0.0f, oy + 17.0f},
                {ox + 4.2f, oy + 13.2f},
                {ox + 7.2f, oy + 20.0f},
                {ox + 10.0f, oy + 18.8f},
                {ox + 7.1f, oy + 12.2f},
                {ox + 12.4f, oy + 12.2f}});
}

static void AddPolyV(Shape& s, const std::vector<Pt>& pts) {
    Prim p;
    p.t = 3;
    p.pts = pts;
    s.push_back(p);
}

// n köşeli yıldız (R: dış yarıçap, r: iç yarıçap).
static void AddStar(Shape& s, float cx, float cy, float R, float r, int n,
                    float a0) {
    const float PI = 3.14159265f;
    std::vector<Pt> pts;
    for (int i = 0; i < n * 2; i++) {
        float rad = (i % 2 == 0) ? R : r;
        float a = a0 + PI * i / n;
        pts.push_back({cx + rad * cosf(a), cy + rad * sinf(a)});
    }
    AddPolyV(s, pts);
}

// Seçilen işaretçi şeklini (ox, oy) başlangıcına çizer; tıklama noktasını
// (hx, hy) olarak döndürür (32 birimlik tasarım alanında).
static void AddArrow(Shape& s, float ox, float oy, float& hx, float& hy) {
    const float PI = 3.14159265f;
    hx = ox;
    hy = oy;
    // Ortalı şekillerin merkezi.
    const float cx = ox + 7.0f, cy = oy + 7.0f;
    switch (g_arrowStyle) {
        case AS_HEAD:  // klasik okun çubuksuz hâli
            AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                        {ox + 0.0f, oy + 17.0f},
                        {ox + 4.6f, oy + 13.4f},
                        {ox + 12.4f, oy + 12.2f}});
            break;
        case AS_TRIANGLE:
            AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                        {ox + 5.0f, oy + 16.0f},
                        {ox + 16.0f, oy + 5.0f}});
            break;
        case AS_WEDGE:
            AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                        {ox + 0.0f, oy + 18.0f},
                        {ox + 11.5f, oy + 12.5f}});
            break;
        case AS_KITE:
            AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                        {ox + 8.5f, oy + 1.4f},
                        {ox + 11.3f, oy + 11.3f},
                        {ox + 1.4f, oy + 8.5f}});
            break;
        case AS_NAVIGATOR:
            AddPoly(s, {{ox + 0.0f, oy + 0.0f},
                        {ox + 14.0f, oy + 5.0f},
                        {ox + 8.0f, oy + 8.0f},
                        {ox + 5.0f, oy + 14.0f}});
            break;
        case AS_CHEVRON:
            AddCap(s, ox, oy, ox + 13.0f, oy + 5.5f, 1.7f);
            AddCap(s, ox, oy, ox + 5.5f, oy + 13.0f, 1.7f);
            break;
        case AS_DIAMOND:
            hx = cx;
            hy = cy;
            AddPoly(s, {{cx, cy - 7.5f},
                        {cx + 6.0f, cy},
                        {cx, cy + 7.5f},
                        {cx - 6.0f, cy}});
            break;
        case AS_SQUARE:
            hx = cx;
            hy = cy;
            AddPoly(s, {{cx - 5.5f, cy - 5.5f},
                        {cx + 5.5f, cy - 5.5f},
                        {cx + 5.5f, cy + 5.5f},
                        {cx - 5.5f, cy + 5.5f}});
            break;
        case AS_DOT:
            hx = cx;
            hy = cy;
            AddCircle(s, cx, cy, 5.5f);
            break;
        case AS_RING:
            hx = cx;
            hy = cy;
            AddRing(s, cx, cy, 5.2f, 1.4f);
            break;
        case AS_TARGET:
            hx = cx;
            hy = cy;
            AddRing(s, cx, cy, 6.0f, 1.1f);
            AddCircle(s, cx, cy, 1.7f);
            break;
        case AS_STAR:
            hx = cx;
            hy = cy;
            AddStar(s, cx, cy, 8.0f, 3.4f, 5, -PI / 2);
            break;
        case AS_SPARKLE:
            hx = cx;
            hy = cy;
            AddStar(s, cx, cy, 8.5f, 2.2f, 4, -PI / 2);
            break;
        case AS_HEART:
            hx = cx;
            hy = cy;
            AddCircle(s, cx - 3.3f, cy - 2.0f, 3.9f);
            AddCircle(s, cx + 3.3f, cy - 2.0f, 3.9f);
            AddPoly(s, {{cx - 7.0f, cy - 0.8f},
                        {cx + 7.0f, cy - 0.8f},
                        {cx, cy + 7.6f}});
            break;
        default:
            AddClassicArrow(s, ox, oy);
            break;
    }
}

// ang: 0 = dikey çift ok; döndürülmüş hâlleri çapraz/yatay oklar olur.
static void AddDoubleArrow(Shape& s, float ang) {
    float c = cosf(ang), sn = sinf(ang);
    auto R = [&](float x, float y) -> Pt {
        float dx = x - 16, dy = y - 16;
        return {16 + dx * c - dy * sn, 16 + dx * sn + dy * c};
    };
    Pt a = R(16, 9), b = R(16, 23);
    AddCap(s, a.x, a.y, b.x, b.y, 1.1f);
    AddPoly(s, {R(16, 3.5f), R(11.3f, 10), R(20.7f, 10)});
    AddPoly(s, {R(16, 28.5f), R(11.3f, 22), R(20.7f, 22)});
}

static void BuildShape(int kind, float phase, Shape& s, float& hx, float& hy) {
    const float PI = 3.14159265f;
    hx = 16;
    hy = 16;
    switch (kind) {
        case K_ARROW:
            AddArrow(s, 3, 3, hx, hy);
            break;
        case K_IBEAM:
            AddCap(s, 16, 6, 16, 26, 1.1f);
            AddCap(s, 12.5f, 6, 19.5f, 6, 1.1f);
            AddCap(s, 12.5f, 26, 19.5f, 26, 1.1f);
            break;
        case K_WAIT: {
            float a0 = phase * 2 * PI;
            AddArc(s, 16, 16, 9, a0, a0 + 1.5f * PI, 2.2f, 16);
            break;
        }
        case K_CROSS:
            AddCap(s, 16, 5, 16, 27, 1.0f);
            AddCap(s, 5, 16, 27, 16, 1.0f);
            break;
        case K_UP:
            AddPoly(s, {{16, 3},
                        {24, 13},
                        {18.7f, 13},
                        {18.7f, 28},
                        {13.3f, 28},
                        {13.3f, 13},
                        {8, 13}});
            hx = 16;
            hy = 3;
            break;
        case K_SIZENS:
            AddDoubleArrow(s, 0);
            break;
        case K_SIZEWE:
            AddDoubleArrow(s, PI / 2);
            break;
        case K_SIZENWSE:
            AddDoubleArrow(s, -PI / 4);
            break;
        case K_SIZENESW:
            AddDoubleArrow(s, PI / 4);
            break;
        case K_SIZEALL:
            AddDoubleArrow(s, 0);
            AddDoubleArrow(s, PI / 2);
            break;
        case K_NO:
            AddRing(s, 16, 16, 9.5f, 2.0f);
            AddCap(s, 9.5f, 22.5f, 22.5f, 9.5f, 2.0f);
            break;
        case K_HAND:
            AddCap(s, 12, 6, 12, 17, 2.3f);
            AddCap(s, 16.6f, 14, 16.6f, 18, 2.1f);
            AddCap(s, 20.8f, 15, 20.8f, 18.5f, 2.0f);
            AddCap(s, 24.8f, 16.5f, 24.8f, 19, 1.9f);
            AddPoly(s, {{9.8f, 16},
                        {26.7f, 16},
                        {26.7f, 22},
                        {23, 27.5f},
                        {13.5f, 27.5f},
                        {9.8f, 23.5f}});
            AddCap(s, 10.5f, 21, 6.8f, 17.2f, 2.1f);
            hx = 12;
            hy = 4;
            break;
        case K_APPSTART: {
            AddArrow(s, 3, 3, hx, hy);
            float a0 = phase * 2 * PI;
            AddArc(s, 23, 23, 4.2f, a0, a0 + 1.5f * PI, 1.3f, 12);
            break;
        }
        case K_HELP: {
            AddArrow(s, 3, 3, hx, hy);
            AddArc(s, 23, 15, 3.4f, PI, 2 * PI + 1.0f, 1.0f, 12);
            AddCap(s, 24.84f, 17.86f, 23, 20.2f, 1.0f);
            AddCap(s, 23, 20.2f, 23, 21.4f, 1.0f);
            AddCircle(s, 23, 24.6f, 1.2f);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Çizim (straight BGRA, N x N)
// ---------------------------------------------------------------------------

// Şeklin dikey sınırları (renk geçişi için).
static void ShapeYRange(const Shape& sh, float& y0, float& y1) {
    y0 = 1e9f;
    y1 = -1e9f;
    for (const Prim& p : sh) {
        float a, b;
        switch (p.t) {
            case 0:
                a = fminf(p.y1, p.y2) - p.r;
                b = fmaxf(p.y1, p.y2) + p.r;
                break;
            case 1:
                a = p.y1 - p.r;
                b = p.y1 + p.r;
                break;
            case 2:
                a = p.y1 - p.r - p.x2;
                b = p.y1 + p.r + p.x2;
                break;
            default:
                a = 1e9f;
                b = -1e9f;
                for (const Pt& q : p.pts) {
                    a = fminf(a, q.y);
                    b = fmaxf(b, q.y);
                }
                break;
        }
        y0 = fminf(y0, a);
        y1 = fmaxf(y1, b);
    }
    if (y1 - y0 < 1.0f) y1 = y0 + 1.0f;
}

static void RenderPixels(int kind, float phase, int N,
                         std::vector<uint32_t>& out, int& hotX, int& hotY) {
    Shape shape;
    float hxu = 16, hyu = 16;
    BuildShape(kind, phase, shape, hxu, hyu);

    const float k = N / 32.0f;
    const Theme& T = g_theme;
    // Tasarım alanındaki şekiller, orijinal Windows imleç boyutuna uysun diye
    // tıklama noktası etrafında küçültülür (0.76 * kullanıcı ölçeği).
    const float S = 0.76f * g_scale;
    float gy0, gy1;
    ShapeYRange(shape, gy0, gy1);
    const float ow = T.outlineW * (g_outlinePct / 100.0f) * k;
    const float gr = fmaxf(0.5f, T.glowR * 0.8f * k);
    const float gs = fminf(1.0f, T.glowStrength * g_glowPct / 100.0f);

    // Kenar yumuşatma bandının genişliği (piksel). "Keskin kenarlar" açıkken
    // dar tutulur, böylece kenarlar daha net görünür.
    const float ramp = g_crisp ? 0.7f : 1.0f;
    auto cov = [&](float d) { return Clamp01(0.5f - d / ramp); };

    // Kontrast kenarı: çerçevenin hemen dışında, çerçeve rengine zıt ince çizgi.
    const float ce = g_contrastEdge ? 0.9f * k : 0.0f;
    const float lum =
        0.299f * T.outline.r + 0.587f * T.outline.g + 0.114f * T.outline.b;
    const Col ceCol = lum < 128.0f ? Col{255.0f, 255.0f, 255.0f}
                                   : Col{0.0f, 0.0f, 0.0f};

    hotX = std::clamp((int)floorf(hxu * k + 0.5f), 0, N - 1);
    hotY = std::clamp((int)floorf(hyu * k + 0.5f), 0, N - 1);

    out.assign((size_t)N * N, 0);
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            float qx = (x + 0.5f) / k, qy = (y + 0.5f) / k;
            float ux = hxu + (qx - hxu) / S, uy = hyu + (qy - hyu) / S;
            float sd = 1e9f;
            for (const Prim& p : shape) sd = fminf(sd, PrimSD(p, ux, uy));
            float sdp = sd * S * k;

            float fillCov = cov(sdp);
            float outerCov = cov(sdp - ow);
            float edgeCov = cov(sdp - ow - ce);
            float ringCov = Clamp01(outerCov - fillCov);
            float edgeRing = Clamp01(edgeCov - outerCov);
            float outside = fmaxf(0.0f, sdp - ow - ce);
            float glowA = 0;
            if (gs > 0.001f && outside < gr * 6) {
                glowA = gs * expf(-outside / gr) * (1 - edgeCov);
            }
            if (glowA <= 0.002f && edgeCov <= 0.002f) continue;

            Col fc;
            if (T.rainbow) {
                fc = Hue((ux + uy) / 48.0f + phase);
            } else {
                float tt = Clamp01((uy - gy0) / (gy1 - gy0));
                fc = {T.fill1.r + (T.fill2.r - T.fill1.r) * tt,
                      T.fill1.g + (T.fill2.g - T.fill1.g) * tt,
                      T.fill1.b + (T.fill2.b - T.fill1.b) * tt};
            }

            float R = 0, G = 0, B = 0, A = 0;
            auto over = [&](const Col& c, float a) {
                if (a <= 0) return;
                float na = a + A * (1 - a);
                R = (c.r * a + R * A * (1 - a)) / na;
                G = (c.g * a + G * A * (1 - a)) / na;
                B = (c.b * a + B * A * (1 - a)) / na;
                A = na;
            };
            over(T.glow, glowA);
            over(ceCol, edgeRing * 0.9f);
            over(T.outline, ringCov);
            over(fc, fillCov * T.fillAlpha);

            out[(size_t)y * N + x] = (Cl8(A * 255) << 24) | (Cl8(R) << 16) |
                                     (Cl8(G) << 8) | Cl8(B);
        }
    }
}

// ---------------------------------------------------------------------------
// .cur / .ani dosyası oluşturma
// ---------------------------------------------------------------------------

static void Put16(std::vector<uint8_t>& v, uint32_t x) {
    v.push_back((uint8_t)(x & 255));
    v.push_back((uint8_t)((x >> 8) & 255));
}

static void Put32(std::vector<uint8_t>& v, uint32_t x) {
    for (int i = 0; i < 4; i++) v.push_back((uint8_t)((x >> (8 * i)) & 255));
}

static void PutTag(std::vector<uint8_t>& v, const char* tag) {
    v.insert(v.end(), tag, tag + 4);
}

// Windows imleç boyutuna göre en uygun görüntüyü seçebilsin diye birkaç boyut.
static const int kSizes[] = {32, 48, 64, 96, 128};

struct CurImage {
    int N = 0, hx = 0, hy = 0;
    std::vector<uint32_t> px;
};

static uint32_t ImageBytes(int N) {
    uint32_t rowMask = (uint32_t)((N + 31) / 32) * 4;
    return 40 + (uint32_t)N * N * 4 + rowMask * N;
}

static void AppendImage(std::vector<uint8_t>& d, const CurImage& im) {
    const int N = im.N;
    uint32_t rowMask = (uint32_t)((N + 31) / 32) * 4;
    uint32_t xorSize = (uint32_t)N * N * 4;
    uint32_t andSize = rowMask * N;

    Put32(d, 40);
    Put32(d, (uint32_t)N);
    Put32(d, (uint32_t)N * 2);
    Put16(d, 1);
    Put16(d, 32);
    Put32(d, 0);
    Put32(d, xorSize + andSize);
    Put32(d, 0);
    Put32(d, 0);
    Put32(d, 0);
    Put32(d, 0);

    for (int y = N - 1; y >= 0; y--) {
        for (int x = 0; x < N; x++) Put32(d, im.px[(size_t)y * N + x]);
    }
    d.insert(d.end(), andSize, 0);
}

// Birden çok boyutlu, 32 bit alfa kanallı .cur dosyası içeriği.
static std::vector<uint8_t> BuildCurFile(int kind, float phase) {
    const int n = (int)ARRAYSIZE(kSizes);
    std::vector<CurImage> imgs(n);
    for (int i = 0; i < n; i++) {
        imgs[i].N = kSizes[i];
        RenderPixels(kind, phase, kSizes[i], imgs[i].px, imgs[i].hx,
                     imgs[i].hy);
    }

    std::vector<uint8_t> d;
    Put16(d, 0);
    Put16(d, 2);
    Put16(d, (uint32_t)n);
    uint32_t offset = 6 + 16 * (uint32_t)n;
    for (int i = 0; i < n; i++) {
        d.push_back((uint8_t)imgs[i].N);
        d.push_back((uint8_t)imgs[i].N);
        d.push_back(0);
        d.push_back(0);
        Put16(d, (uint32_t)imgs[i].hx);
        Put16(d, (uint32_t)imgs[i].hy);
        Put32(d, ImageBytes(imgs[i].N));
        Put32(d, offset);
        offset += ImageBytes(imgs[i].N);
    }
    for (int i = 0; i < n; i++) AppendImage(d, imgs[i]);
    return d;
}

// Animasyonlu imleç (.ani) dosyası içeriği.
static std::vector<uint8_t> BuildAniFile(int kind) {
    const int FRAMES = 16;

    std::vector<uint8_t> list;
    PutTag(list, "fram");
    for (int i = 0; i < FRAMES; i++) {
        std::vector<uint8_t> cur = BuildCurFile(kind, (float)i / FRAMES);
        PutTag(list, "icon");
        Put32(list, (uint32_t)cur.size());
        list.insert(list.end(), cur.begin(), cur.end());
        if (cur.size() & 1) list.push_back(0);
    }

    std::vector<uint8_t> body;
    PutTag(body, "ACON");
    PutTag(body, "anih");
    Put32(body, 36);
    Put32(body, 36);      // cbSize
    Put32(body, FRAMES);  // cFrames
    Put32(body, FRAMES);  // cSteps
    Put32(body, 0);       // cx
    Put32(body, 0);       // cy
    Put32(body, 0);       // cBitCount
    Put32(body, 0);       // cPlanes
    Put32(body, 4);       // jifRate (4/60 sn)
    Put32(body, 1);       // AF_ICON
    PutTag(body, "LIST");
    Put32(body, (uint32_t)list.size());
    body.insert(body.end(), list.begin(), list.end());

    std::vector<uint8_t> file;
    PutTag(file, "RIFF");
    Put32(file, (uint32_t)body.size());
    file.insert(file.end(), body.begin(), body.end());
    return file;
}

static bool WriteBytes(const std::wstring& path,
                       const std::vector<uint8_t>& data) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Failed to create the file (%lu): %s", GetLastError(),
               path.c_str());
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(f, data.data(), (DWORD)data.size(), &written, nullptr);
    CloseHandle(f);
    if (!ok || written != data.size()) {
        Wh_Log(L"Failed to write the file (%lu): %s", GetLastError(),
               path.c_str());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Storage folder and cursor files
// ---------------------------------------------------------------------------

// The cursor files live in a per-session subfolder of the mod storage, so
// several signed-in users never delete each other's files.
static std::wstring g_dir;
static std::vector<std::wstring> g_paths;  // current files, empty = missing
static std::wstring g_genPrefix;           // file name prefix of the current files
static ULONGLONG g_lastApply = 0;

static DWORD GetSessionId() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    return sessionId;
}

static bool EnsureDir() {
    WCHAR storage[MAX_PATH];
    size_t len = Wh_GetModStoragePath(storage, ARRAYSIZE(storage));
    if (len == 0 || len >= ARRAYSIZE(storage)) {
        Wh_Log(L"The mod storage path is not available");
        return false;
    }
    WCHAR sub[32];
    swprintf_s(sub, L"session-%lu", GetSessionId());
    g_dir = std::wstring(storage) + L"\\" + sub;
    int r = SHCreateDirectoryExW(nullptr, g_dir.c_str(), nullptr);
    if (r != ERROR_SUCCESS && r != ERROR_ALREADY_EXISTS &&
        r != ERROR_FILE_EXISTS) {
        Wh_Log(L"Failed to create the folder (%d): %s", r, g_dir.c_str());
        return false;
    }
    return true;
}

// If keepPrefix is empty, every cursor file in the session folder is deleted.
static void CleanupFiles(const std::wstring& keepPrefix) {
    if (g_dir.empty()) return;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((g_dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring name = fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        size_t len = name.size();
        bool ours = len > 4 && (name.compare(len - 4, 4, L".cur") == 0 ||
                                name.compare(len - 4, 4, L".ani") == 0);
        if (!ours) continue;
        if (!keepPrefix.empty() && name.rfind(keepPrefix, 0) == 0) continue;
        DeleteFileW((g_dir + L"\\" + name).c_str());
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static void RemoveSessionDir() {
    CleanupFiles(L"");
    if (!g_dir.empty()) RemoveDirectoryW(g_dir.c_str());
}

// ---------------------------------------------------------------------------
// Applying and restoring the cursors
//
// The theme is applied only with SetSystemCursor, which changes the system
// cursors of the current session in memory. The user's saved pointer scheme
// (the registry) is never modified.
// ---------------------------------------------------------------------------

// Remembers that the cursors are replaced, so that the next start can undo
// the change if the previous run did not exit cleanly.
static void GetModifiedKey(WCHAR* key, size_t count) {
    swprintf_s(key, count, L"cursorsModified_%lu", GetSessionId());
}

static void SetModifiedFlag(bool on) {
    WCHAR key[64];
    GetModifiedKey(key, ARRAYSIZE(key));
    Wh_SetIntValue(key, on ? 1 : 0);
}

static bool GetModifiedFlag() {
    WCHAR key[64];
    GetModifiedKey(key, ARRAYSIZE(key));
    return Wh_GetIntValue(key, 0) != 0;
}

// Reloads the user's own pointer scheme from the registry.
static void RestoreSystemCursors() {
    if (!SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0)) {
        Wh_Log(L"SPI_SETCURSORS failed: %lu", GetLastError());
    }
}

// Generates the cursor files of the current theme. Returns true if at least
// one file was written.
static bool GenerateFiles() {
    if (!EnsureDir()) return false;

    WCHAR gen[32];
    swprintf_s(gen, L"%llu_", (unsigned long long)GetTickCount64());

    const int total = (int)ARRAYSIZE(kDefs);
    std::vector<std::wstring> paths(total);
    int okCount = 0;
    for (int i = 0; i < total; i++) {
        const CurDef& d = kDefs[i];
        bool anim = d.animated && g_animBusy;
        std::vector<uint8_t> data =
            anim ? BuildAniFile(d.kind) : BuildCurFile(d.kind, 0.0f);
        std::wstring path =
            g_dir + L"\\" + gen + d.valueName + (anim ? L".ani" : L".cur");
        if (!WriteBytes(path, data)) continue;
        paths[i] = path;
        okCount++;
    }
    Wh_Log(L"Cursor files: %d/%d", okCount, total);

    g_paths = paths;
    g_genPrefix = gen;
    return okCount > 0;
}

// Loads the generated files and replaces the system cursors with them.
static void ApplyCursors() {
    const int total = (int)ARRAYSIZE(kDefs);
    if ((int)g_paths.size() != total) return;

    int n = 0;
    for (int i = 0; i < total; i++) {
        if (g_paths[i].empty()) continue;
        HCURSOR h = LoadCursorFromFileW(g_paths[i].c_str());
        if (!h) {
            Wh_Log(L"LoadCursorFromFile failed (%s): %lu", kDefs[i].valueName,
                   GetLastError());
            continue;
        }
        if (SetSystemCursor(h, kDefs[i].ocr)) {
            n++;
        } else {
            Wh_Log(L"SetSystemCursor failed (%s): %lu", kDefs[i].valueName,
                   GetLastError());
            DestroyCursor(h);
        }
    }
    if (n > 0) SetModifiedFlag(true);
    g_lastApply = GetTickCount64();
    Wh_Log(L"Applied %d/%d cursors", n, total);
}

static void ApplyTheme() {
    Wh_Log(L"Applying theme: %s", g_theme.id);
    if (!GenerateFiles()) return;
    ApplyCursors();

    // The cursors are now loaded in memory, so older files can go. Keep the
    // files of this generation, they are used to re-apply the theme.
    CleanupFiles(g_genPrefix);
}

// ---------------------------------------------------------------------------
// Worker thread, message window and tool mod entry points
// ---------------------------------------------------------------------------

static HANDLE g_stopEvent = nullptr;
static HANDLE g_readyEvent = nullptr;
static HANDLE g_thread = nullptr;
static HWND g_hwnd = nullptr;

static const UINT WM_APP_SETTINGS = WM_APP + 1;
static const UINT_PTR kReapplyTimer = 1;

// Hidden top-level window (message-only windows don't receive broadcasts). It
// notices when Windows reloads the pointer scheme, for example after the user
// picks another scheme in the Mouse settings, and applies the theme again.
static LRESULT CALLBACK MsgWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_APP_SETTINGS:
            LoadSettings();
            ApplyTheme();
            return 0;
        case WM_SETTINGCHANGE:
            // Ignore the changes caused by our own SetSystemCursor calls.
            if (w == SPI_SETCURSORS && GetTickCount64() - g_lastApply > 500) {
                SetTimer(h, kReapplyTimer, 300, nullptr);
            }
            return 0;
        case WM_TIMER:
            if (w == kReapplyTimer) {
                KillTimer(h, kReapplyTimer);
                Wh_Log(L"The pointer scheme was reloaded, applying the theme "
                       L"again");
                ApplyCursors();
            }
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

// Keeps the tool mod process alive and runs the message loop until the mod is
// unloaded.
static DWORD WINAPI Worker(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkMouseCursorThemesWindow";
    RegisterClassW(&wc);
    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0,
                             nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"Failed to create the message window: %lu", GetLastError());
    }

    // Undo the changes of a previous run that did not exit cleanly.
    if (GetModifiedFlag()) {
        Wh_Log(L"Restoring the cursors left by a previous run");
        RestoreSystemCursors();
        SetModifiedFlag(false);
    }

    ApplyTheme();
    if (g_readyEvent) SetEvent(g_readyEvent);

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                            QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) break;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_hwnd) DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Mouse Cursor Themes starting");
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Failed to create the worker thread: %lu", GetLastError());
        return FALSE;
    }
    DWORD w = WaitForSingleObject(g_readyEvent, 60000);
    if (w != WAIT_OBJECT_0) Wh_Log(L"The first apply did not finish: %lu", w);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    // Applied on the worker thread, which owns the cursor state.
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
    } else {
        LoadSettings();
        ApplyTheme();
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Mouse Cursor Themes stopping");
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    // Reload the user's own pointer scheme, then delete the generated files.
    RestoreSystemCursors();
    SetModifiedFlag(false);
    RemoveSessionDir();
    if (g_stopEvent) CloseHandle(g_stopEvent);
    if (g_readyEvent) CloseHandle(g_readyEvent);
    g_stopEvent = nullptr;
    g_readyEvent = nullptr;
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
