// ==WindhawkMod==
// @id              taskbar-hover-animations
// @name            Taskbar Icon Animation
// @name:tr-TR      Görev Çubuğu İkon Animasyonu
// @description     Heartbeat, flash, ripple, neon glow, sparkle, bounce, wobble, confetti and more animations when hovering taskbar icons
// @description:tr-TR Görev çubuğu ikonlarının üzerine gelince kalp atışı, flaş, dalga, neon parıltı, yıldız parıltısı, zıplama, sallanma, konfeti ve daha fazla animasyon oynatır
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Icon Animation
![Preview](https://i.imgur.com/nHvCbY5.gif)
![Preview](https://i.imgur.com/YyZFtgA.gif)

When the mouse hovers over a taskbar icon, an animation plays on a transparent
overlay above the icon (the overlay does not block clicks).

## Effects
- **Heartbeat**: Grows and shrinks twice in a "lub-dub" rhythm, with a red/pink halo flashing behind it.
- **Flash**: White glow plus a lens flare, double flash.
- **Ripple**: 3 rings spreading out from around the icon.
- **Neon glow**: A rainbow halo that changes color.
- **Sparkle**: Glints appearing randomly around the icon.
- **Bounce**: 3 damped bounces.
- **Wobble**: A damped side-to-side wobble.
- **Spin**: A full rotation while growing slightly, with a ring appearing around it.
- **Jelly**: A jelly-like squash and stretch that gradually settles.
- **Flip**: Flips around the horizontal axis like a coin.
- **Shake**: A fast horizontal shake.
- **Confetti**: A burst of colorful confetti.
- **Bubbles**: Soap bubbles rising from below the icon.
- **Love**: A heartbeat plus hearts floating upward.
- **Random / Cycle**: A different effect on every hover.

## Notes
- Works on both the Windows 10 and Windows 11 taskbar (button positions are found with UI Automation).
- The mod runs in its own dedicated process, not inside Explorer, so a problem in the mod cannot crash the shell.
- Animations are not played over fullscreen apps.
- The animation is played on the overlay using a screenshot of the icon, so if the
  icon is very dark or the same color as the background, its edges may look soft.
- When "App buttons only" is enabled, Start, Search, tray icons, etc. are not animated.

---

# Görev Çubuğu İkon Animasyonu

Fare görev çubuğundaki bir ikonun üzerine geldiğinde, ikonun üstünde (tıklamayı
engellemeyen) şeffaf bir katman üzerinde animasyon oynatır.

## Efektler
- **Kalp atışı**: "lub-dub" şeklinde iki kez büyüyüp küçülür, arkasında kırmızı/pembe halo yanıp söner.
- **Flaş**: Beyaz parlama + mercek yıldızı (lens flare), çift flaş.
- **Dalga**: İkonun etrafından yayılan 3 halka.
- **Neon parıltı**: Renk değiştiren gökkuşağı halo.
- **Yıldız parıltısı**: İkonun etrafında rastgele beliren ışıltılar.
- **Zıplama**: Sönümlenen 3 zıplama.
- **Sallanma**: Sağa sola sönümlenen sallanma.
- **Dönüş**: Tam tur dönerken hafifçe büyür, etrafında halka belirir.
- **Jöle**: Sıkışıp gerilerek sönümlenen jöle gibi esneme.
- **Çevirme**: Madeni para gibi yatay eksende çevrilir.
- **Titreme**: Hızlı yatay titreme.
- **Konfeti**: Renkli konfeti patlaması.
- **Baloncuklar**: İkonun altından yükselen sabun köpüğü baloncukları.
- **Aşk**: Kalp atışı ve yukarı süzülen kalpler.
- **Rastgele / Sırayla**: Her hover'da farklı efekt.

## Notlar
- Windows 10 ve Windows 11 görev çubuğunda çalışır (UI Automation ile düğme konumu bulunur).
- Mod, Explorer'ın içinde değil kendi ayrı sürecinde çalışır. Bu yüzden modda bir sorun olsa bile kabuk çökmez.
- Tam ekran uygulamaların üstünde animasyon oynatılmaz.
- Animasyon, ikonun ekran görüntüsü alınarak katmanda oynatılır; bu yüzden
  ikon çok koyu/arka planla aynı renkteyse kenarlar yumuşak görünebilir.
- "Sadece uygulama düğmeleri" açıksa Başlat, Arama, tepsi ikonları vb. animasyon almaz.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- effect: random
  $name: Effect
  $name:tr-TR: Efekt
  $options:
  - random: Random
  - cycle: Cycle
  - heartbeat: Heartbeat
  - flash: Flash
  - ripple: Ripple
  - glow: Neon glow
  - sparkle: Sparkle
  - bounce: Bounce
  - wobble: Wobble
  - spin: Spin
  - jelly: Jelly (squash and stretch)
  - flip: Flip
  - shake: Shake
  - confetti: Confetti
  - bubbles: Bubbles
  - love: Love (rising hearts)
  $options:tr-TR:
  - random: Rastgele
  - cycle: Sırayla
  - heartbeat: Kalp atışı
  - flash: Flaş
  - ripple: Dalga
  - glow: Neon parıltı
  - sparkle: Yıldız parıltısı
  - bounce: Zıplama
  - wobble: Sallanma
  - spin: Dönüş
  - jelly: Jöle (sıkış-gerin)
  - flip: Çevirme
  - shake: Titreme
  - confetti: Konfeti
  - bubbles: Baloncuklar
  - love: Aşk (yükselen kalpler)
- duration: 800
  $name: Animation duration (ms)
  $name:tr-TR: Animasyon süresi (ms)
  $description: 300 to 3000 is recommended.
  $description:tr-TR: 300 ile 3000 arası önerilir.
- intensity: 100
  $name: Intensity (%)
  $name:tr-TR: Şiddet (%)
  $description: 100 is the default. A higher value makes the animation more pronounced.
  $description:tr-TR: 100 varsayılan. Daha büyük değer daha belirgin animasyon verir.
- onlyAppButtons: false
  $name: App buttons only
  $name:tr-TR: Sadece uygulama düğmeleri
  $description: When enabled, Start, Search, tray buttons, etc. are not animated.
  $description:tr-TR: Açıksa Başlat, Arama, tepsi vb. düğmeler animasyon almaz.
- accent: "#4cc2ff"
  $name: Accent color (Ripple and Spin effects)
  $name:tr-TR: Vurgu rengi (Dalga ve Dönüş efektleri)
  $description: "Use the #RRGGBB format."
  $description:tr-TR: "#RRGGBB biçiminde."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <stdio.h>
#include <wchar.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// ---------------------------------------------------------------------------
// Ayarlar
// ---------------------------------------------------------------------------

enum Effect {
    E_HEART = 0,
    E_FLASH,
    E_RIPPLE,
    E_GLOW,
    E_SPARKLE,
    E_BOUNCE,
    E_WOBBLE,
    E_SPIN,
    E_JELLY,
    E_FLIP,
    E_SHAKE,
    E_CONFETTI,
    E_BUBBLES,
    E_LOVE,
    E_COUNT,
    E_RANDOM = 100,
    E_CYCLE = 101,
};

struct Color {
    float r, g, b;
};

struct Settings {
    int effect = E_RANDOM;
    int duration = 800;
    float intensity = 1.0f;
    int repeat = 2;  // Sabit: fare ikon üzerinde kaldıkça toplam 2 kez oynatılır
    bool onlyApp = false;
    Color accent = {76, 194, 255};
};

static Settings g_s;
static std::atomic<bool> g_stop{false};
static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_initOk = false;

static void LoadSettings() {
    Settings s;

    PCWSTR eff = Wh_GetStringSetting(L"effect");
    if (eff) {
        if (!wcscmp(eff, L"heartbeat")) s.effect = E_HEART;
        else if (!wcscmp(eff, L"flash")) s.effect = E_FLASH;
        else if (!wcscmp(eff, L"ripple")) s.effect = E_RIPPLE;
        else if (!wcscmp(eff, L"glow")) s.effect = E_GLOW;
        else if (!wcscmp(eff, L"sparkle")) s.effect = E_SPARKLE;
        else if (!wcscmp(eff, L"bounce")) s.effect = E_BOUNCE;
        else if (!wcscmp(eff, L"wobble")) s.effect = E_WOBBLE;
        else if (!wcscmp(eff, L"spin")) s.effect = E_SPIN;
        else if (!wcscmp(eff, L"jelly")) s.effect = E_JELLY;
        else if (!wcscmp(eff, L"flip")) s.effect = E_FLIP;
        else if (!wcscmp(eff, L"shake")) s.effect = E_SHAKE;
        else if (!wcscmp(eff, L"confetti")) s.effect = E_CONFETTI;
        else if (!wcscmp(eff, L"bubbles")) s.effect = E_BUBBLES;
        else if (!wcscmp(eff, L"love")) s.effect = E_LOVE;
        else if (!wcscmp(eff, L"cycle")) s.effect = E_CYCLE;
        else s.effect = E_RANDOM;
        Wh_FreeStringSetting(eff);
    }

    s.duration = std::clamp(Wh_GetIntSetting(L"duration"), 200, 5000);
    s.intensity = std::clamp(Wh_GetIntSetting(L"intensity"), 20, 300) / 100.0f;
    s.onlyApp = Wh_GetIntSetting(L"onlyAppButtons") != 0;

    PCWSTR acc = Wh_GetStringSetting(L"accent");
    if (acc) {
        const wchar_t* p = acc;
        if (*p == L'#') p++;
        if (wcslen(p) == 6) {
            unsigned long v = wcstoul(p, nullptr, 16);
            s.accent = {(float)((v >> 16) & 255), (float)((v >> 8) & 255),
                        (float)(v & 255)};
        }
        Wh_FreeStringSetting(acc);
    }

    g_s = s;
}

// ---------------------------------------------------------------------------
// Piksel yardımcıları (premultiplied BGRA)
// ---------------------------------------------------------------------------

static inline float Clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

static inline uint32_t Cl8(float v) {
    int i = (int)(v + 0.5f);
    return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
}

// Düz (straight) renk r,g,b (0..255), alfa a (0..1) ile üstüne bindir.
static inline void Over(uint32_t* buf, int C, int x, int y, float r, float g,
                        float b, float a) {
    if (a <= 0.003f) return;
    if (a > 1) a = 1;
    uint32_t& d = buf[y * C + x];
    float inv = 1 - a;
    float da = (float)(d >> 24), dr = (float)((d >> 16) & 255),
          dg = (float)((d >> 8) & 255), db = (float)(d & 255);
    float oa = a * 255 + da * inv;
    float orr = r * a + dr * inv;
    float og = g * a + dg * inv;
    float ob = b * a + db * inv;
    d = (Cl8(oa) << 24) | (Cl8(orr) << 16) | (Cl8(og) << 8) | Cl8(ob);
}

// Premultiplied kaynak (0..255) bindir.
static inline void OverPre(uint32_t* buf, int C, int x, int y, float pr,
                           float pg, float pb, float pa) {
    if (pa <= 0.5f) return;
    uint32_t& d = buf[y * C + x];
    float inv = 1 - pa / 255.0f;
    float da = (float)(d >> 24), dr = (float)((d >> 16) & 255),
          dg = (float)((d >> 8) & 255), db = (float)(d & 255);
    d = (Cl8(pa + da * inv) << 24) | (Cl8(pr + dr * inv) << 16) |
        (Cl8(pg + dg * inv) << 8) | Cl8(pb + db * inv);
}

static Color Hue(float h) {
    h = h - floorf(h);
    float r = Clamp01(fabsf(h * 6 - 3) - 1);
    float g = Clamp01(2 - fabsf(h * 6 - 2));
    float b = Clamp01(2 - fabsf(h * 6 - 4));
    return {r * 255, g * 255, b * 255};
}

// Halka / disk parlaması. peak=0 ise dolu disk (merkezden dışa sönümlenir).
static void Ring(uint32_t* buf, int C, float cx, float cy, float peak,
                 float width, Color c, float alpha) {
    if (alpha <= 0.003f || width <= 0.5f) return;
    float R = peak + width;
    int x0 = std::max(0, (int)floorf(cx - R)),
        x1 = std::min(C - 1, (int)ceilf(cx + R));
    int y0 = std::max(0, (int)floorf(cy - R)),
        y1 = std::min(C - 1, (int)ceilf(cy + R));
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - cx, dy = y + 0.5f - cy;
            float d = sqrtf(dx * dx + dy * dy);
            float k = 1 - fabsf(d - peak) / width;
            if (k <= 0) continue;
            Over(buf, C, x, y, c.r, c.g, c.b, alpha * k * k);
        }
    }
}

// 4 uçlu parıltı yıldızı.
static void Star(uint32_t* buf, int C, float cx, float cy, float len,
                 float thick, Color c, float alpha) {
    if (alpha <= 0.003f || len <= 1 || thick <= 0.2f) return;
    int x0 = std::max(0, (int)floorf(cx - len)),
        x1 = std::min(C - 1, (int)ceilf(cx + len));
    int y0 = std::max(0, (int)floorf(cy - len)),
        y1 = std::min(C - 1, (int)ceilf(cy + len));
    float t2 = thick * thick;
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - cx, dy = y + 0.5f - cy;
            float h = fmaxf(0, 1 - fabsf(dx) / len) * expf(-(dy * dy) / t2);
            float v = fmaxf(0, 1 - fabsf(dy) / len) * expf(-(dx * dx) / t2);
            float core = 0.8f * expf(-(dx * dx + dy * dy) / (t2 * 4));
            float k = fminf(1.0f, h + v + core);
            Over(buf, C, x, y, c.r, c.g, c.b, alpha * k);
        }
    }
}

// ---------------------------------------------------------------------------
// Animasyon durumu
// ---------------------------------------------------------------------------

struct Sparkle {
    float ang, dist, phase, size;
    bool gold;
};

struct Part {
    float a, sp, sz, ph, hu;
};

struct Anim {
    bool active = false;
    int effect = 0;
    ULONGLONG start = 0;
    int duration = 800;
    float e = 1.0f;
    Color accent = {76, 194, 255};
    int side = 0;
    int C = 0;
    POINT center = {0, 0};
    std::vector<uint32_t> icon;  // side*side, premultiplied BGRA
    Color bg = {0, 0, 0};
    Sparkle sp[7];
    Part pt[16];
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    uint32_t* bits = nullptr;
};

static Anim g_a;
static HWND g_hwnd = nullptr;
static HDC g_memdc = nullptr;
static int g_cycle = 0;

// İkonu (dönüş/ölçek/öteleme/tint ile) çiz.
static void DrawIcon(uint32_t* buf, int C, const Anim& A, float cx, float cy,
                     float s, float theta, Color tintC, float tint,
                     float alphaMul, float sy = 0) {
    const int S = A.side;
    const float half = S * 0.5f;
    if (sy == 0) sy = s;
    // s / sy negatif olabilir (ayna çevirme); sıfıra bölmeyi önle.
    float ss = fabsf(s) < 0.03f ? (s < 0 ? -0.03f : 0.03f) : s;
    float sy2 = fabsf(sy) < 0.03f ? (sy < 0 ? -0.03f : 0.03f) : sy;
    float R = half * fmaxf(fabsf(s), fabsf(sy)) * 1.5f + 2;
    int x0 = std::max(0, (int)floorf(cx - R)),
        x1 = std::min(C - 1, (int)ceilf(cx + R));
    int y0 = std::max(0, (int)floorf(cy - R)),
        y1 = std::min(C - 1, (int)ceilf(cy + R));
    float cs = cosf(theta), sn = sinf(theta);
    const uint32_t* ic = A.icon.data();

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float px = x + 0.5f - cx, py = y + 0.5f - cy;
            float u = (cs * px + sn * py) / ss + half - 0.5f;
            float v = (-sn * px + cs * py) / sy2 + half - 0.5f;
            int iu = (int)floorf(u), iv = (int)floorf(v);
            float fu = u - iu, fv = v - iv;
            float pr = 0, pg = 0, pb = 0, pa = 0;
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 2; i++) {
                    int sx = iu + i, sy = iv + j;
                    if (sx < 0 || sy < 0 || sx >= S || sy >= S) continue;
                    float w = (i ? fu : 1 - fu) * (j ? fv : 1 - fv);
                    uint32_t p = ic[sy * S + sx];
                    pa += w * (float)(p >> 24);
                    pr += w * (float)((p >> 16) & 255);
                    pg += w * (float)((p >> 8) & 255);
                    pb += w * (float)(p & 255);
                }
            }
            if (pa <= 0.5f) continue;
            if (tint > 0) {
                float it = 1 - tint;
                pr = pr * it + pa * tint * (tintC.r / 255.0f);
                pg = pg * it + pa * tint * (tintC.g / 255.0f);
                pb = pb * it + pa * tint * (tintC.b / 255.0f);
            }
            pr *= alphaMul;
            pg *= alphaMul;
            pb *= alphaMul;
            pa *= alphaMul;
            OverPre(buf, C, x, y, pr, pg, pb, pa);
        }
    }
}

// Orijinal ikonu gizlemek için arka plan renginde yumuşak yama.
static void Patch(uint32_t* buf, int C, float cx, float cy, const Anim& A,
                  float alpha) {
    if (alpha <= 0.01f) return;
    float half = A.side * 0.55f;
    float feather = 4.0f;
    int x0 = std::max(0, (int)floorf(cx - half - 1)),
        x1 = std::min(C - 1, (int)ceilf(cx + half + 1));
    int y0 = std::max(0, (int)floorf(cy - half - 1)),
        y1 = std::min(C - 1, (int)ceilf(cy + half + 1));
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float ax = Clamp01((half - fabsf(x + 0.5f - cx)) / feather);
            float ay = Clamp01((half - fabsf(y + 0.5f - cy)) / feather);
            Over(buf, C, x, y, A.bg.r, A.bg.g, A.bg.b, ax * ay * alpha);
        }
    }
}

// Kalp şekli (2x2 süper örnekleme ile yumuşatılmış). size ≈ kalbin yarı genişliği/1.14
static void Heart(uint32_t* buf, int C, float cx, float cy, float size,
                  Color c, float alpha) {
    if (alpha <= 0.003f || size < 1) return;
    int R = (int)ceilf(size * 1.5f);
    int x0 = std::max(0, (int)floorf(cx - R)), x1 = std::min(C - 1, (int)ceilf(cx + R));
    int y0 = std::max(0, (int)floorf(cy - R)), y1 = std::min(C - 1, (int)ceilf(cy + R));
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            int hit = 0;
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 2; i++) {
                    float fx = (x + (i + 0.5f) * 0.5f - cx) / size;
                    float fy = -(y + (j + 0.5f) * 0.5f - cy) / size + 0.12f;
                    float q = fx * fx + fy * fy - 1;
                    if (q * q * q - fx * fx * fy * fy * fy <= 0) hit++;
                }
            }
            if (hit) Over(buf, C, x, y, c.r, c.g, c.b, alpha * hit * 0.25f);
        }
    }
}

// Dönen küçük dikdörtgen (konfeti parçası). w,h yarı boyutlar.
static void Piece(uint32_t* buf, int C, float px, float py, float w, float h,
                  float rot, Color c, float alpha) {
    if (alpha <= 0.003f) return;
    float R = fmaxf(w, h) * 1.5f + 1;
    int x0 = std::max(0, (int)floorf(px - R)), x1 = std::min(C - 1, (int)ceilf(px + R));
    int y0 = std::max(0, (int)floorf(py - R)), y1 = std::min(C - 1, (int)ceilf(py + R));
    float cs = cosf(rot), sn = sinf(rot);
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x + 0.5f - px, dy = y + 0.5f - py;
            float lx = cs * dx + sn * dy, ly = -sn * dx + cs * dy;
            float cov = Clamp01(w - fabsf(lx) + 0.5f) * Clamp01(h - fabsf(ly) + 0.5f);
            Over(buf, C, x, y, c.r, c.g, c.b, alpha * cov);
        }
    }
}

static inline float Bump(float t, float c, float w) {
    float d = (t - c) / w;
    return expf(-d * d);
}

static void Render(float t) {
    Anim& A = g_a;
    const int C = A.C;
    uint32_t* buf = A.bits;
    memset(buf, 0, (size_t)C * C * 4);

    const float cx = C * 0.5f, cy = C * 0.5f;
    const float S = (float)A.side;
    const float e = A.e;
    const float ec = fminf(e, 1.5f);
    const float PI = 3.14159265f;
    const Color white = {255, 255, 255};

    switch (A.effect) {
        case E_HEART: {
            float k = fminf(1.0f, Bump(t, 0.14f, 0.08f) + 0.8f * Bump(t, 0.40f, 0.09f));
            float s = 1 + 0.30f * e * k;
            Patch(buf, C, cx, cy, A, Clamp01(k * 4));
            Ring(buf, C, cx, cy, 0, S * 0.95f * s, {255, 50, 90}, 0.55f * ec * k);
            DrawIcon(buf, C, A, cx, cy, s, 0, {255, 60, 90}, 0.25f * k, 1.0f);
            break;
        }
        case E_FLASH: {
            float f = fminf(1.0f, Bump(t, 0.13f, 0.10f) + 0.55f * Bump(t, 0.40f, 0.09f));
            Ring(buf, C, cx, cy, 0, S * (0.8f + 0.5f * f), white, 0.6f * f * ec);
            DrawIcon(buf, C, A, cx, cy, 1.0f, 0, white, fminf(1.0f, 0.95f * f * e), 1.0f);
            Star(buf, C, cx, cy, S * (0.5f + 0.9f * f * e), S * 0.045f, white, f);
            break;
        }
        case E_RIPPLE: {
            for (int i = 0; i < 3; i++) {
                float lt = t * 1.5f - i * 0.22f;
                if (lt <= 0 || lt >= 1) continue;
                float ease = 1 - (1 - lt) * (1 - lt);
                float radius = S * (0.45f + 1.0f * ease * e);
                float al = powf(1 - lt, 1.5f) * 0.85f * ec;
                Ring(buf, C, cx, cy, radius, S * 0.10f + 2, A.accent, al);
            }
            break;
        }
        case E_GLOW: {
            float env = sinf(PI * t);
            float pulse = 1 + 0.08f * sinf(2 * PI * 2 * t);
            Color c1 = Hue(t * 1.2f), c2 = Hue(t * 1.2f + 0.33f);
            Ring(buf, C, cx, cy, S * 0.66f * pulse, S * 0.38f, c1, 0.80f * env * ec);
            Ring(buf, C, cx, cy, S * 0.80f * pulse, S * 0.30f, c2, 0.45f * env * ec);
            break;
        }
        case E_SPARKLE: {
            for (const Sparkle& sp : A.sp) {
                float lt = (t - sp.phase) / 0.5f;
                if (lt <= 0 || lt >= 1) continue;
                float env = sinf(PI * lt);
                float px = cx + cosf(sp.ang) * sp.dist * S;
                float py = cy + sinf(sp.ang) * sp.dist * S;
                Color c = sp.gold ? Color{255, 225, 120} : white;
                Star(buf, C, px, py, sp.size * S * env * (0.6f + 0.4f * e), S * 0.03f + 0.6f, c, env);
            }
            float tw = sinf(PI * t);
            Ring(buf, C, cx, cy, 0, S * 0.9f, {255, 230, 150}, 0.12f * tw * ec);
            break;
        }
        case E_BOUNCE: {
            float damp = powf(1 - t, 1.5f);
            float h = fabsf(sinf(3 * PI * t)) * damp;
            float dy = -S * 0.45f * e * h;
            Patch(buf, C, cx, cy, A, Clamp01(h * 4));
            DrawIcon(buf, C, A, cx, cy + dy, 1 + 0.06f * e * h, 0, white, 0, 1.0f);
            break;
        }
        case E_WOBBLE: {
            float theta = 0.42f * e * sinf(2 * PI * 3 * t) * (1 - t);
            Patch(buf, C, cx, cy, A, Clamp01(fabsf(theta) * 10));
            DrawIcon(buf, C, A, cx, cy, 1 + 0.05f * e * (1 - t), theta, white, 0, 1.0f);
            break;
        }
        case E_SPIN: {
            float ease = t * t * (3 - 2 * t);
            float theta = 2 * PI * ease;
            float env = sinf(PI * t);
            Patch(buf, C, cx, cy, A, Clamp01(env * 6));
            Ring(buf, C, cx, cy, S * 0.62f, S * 0.30f, A.accent, 0.35f * env * ec);
            DrawIcon(buf, C, A, cx, cy, 1 + 0.14f * e * env, theta, white, 0, 1.0f);
            break;
        }
        case E_JELLY: {
            float w = sinf(2 * PI * 3.2f * t) * powf(1 - t, 1.5f) * e;
            Patch(buf, C, cx, cy, A, Clamp01(fabsf(w) * 5));
            DrawIcon(buf, C, A, cx, cy, 1 + 0.28f * w, 0, white, 0, 1.0f, 1 - 0.28f * w);
            break;
        }
        case E_FLIP: {
            float ease = t * t * (3 - 2 * t);
            float sx = cosf(2 * PI * ease);
            float env = sinf(PI * t);
            Patch(buf, C, cx, cy, A, Clamp01(env * 8));
            Ring(buf, C, cx, cy, 0, S * 0.9f, white, 0.18f * env * ec);
            DrawIcon(buf, C, A, cx, cy, sx, 0, white, 0.15f * (1 - fabsf(sx)), 1.0f,
                     1 + 0.08f * e * env);
            break;
        }
        case E_SHAKE: {
            float dx = S * 0.14f * e * sinf(2 * PI * 9 * t) * (1 - t);
            Patch(buf, C, cx, cy, A, Clamp01(fabsf(dx) / (S * 0.03f)));
            DrawIcon(buf, C, A, cx + dx, cy, 1.0f, 0, white, 0, 1.0f);
            break;
        }
        case E_CONFETTI: {
            float burst = Bump(t, 0.05f, 0.08f);
            Ring(buf, C, cx, cy, 0, S * 0.8f, {255, 240, 180}, 0.45f * burst * ec);
            for (int i = 0; i < 16; i++) {
                const Part& p = A.pt[i];
                float tt = Clamp01((t - p.ph * 0.1f) / 0.9f);
                if (tt <= 0 || tt >= 1) continue;
                float ease = 1 - (1 - tt) * (1 - tt);
                float dist = S * (0.30f + p.sp * 0.7f) * (0.6f + 0.4f * ec) * ease;
                float px = cx + cosf(p.a) * dist;
                float py = cy + sinf(p.a) * dist + S * 0.35f * tt * tt;
                float al = 1 - tt * tt * tt;
                Piece(buf, C, px, py, S * 0.05f + 1.2f + p.sz * S * 0.04f,
                      S * 0.025f + 0.8f, p.ph * 10 + tt * 14, Hue(p.hu), al);
            }
            break;
        }
        case E_BUBBLES: {
            for (int i = 0; i < 8; i++) {
                const Part& p = A.pt[i];
                float lt = (t - p.ph * 0.35f) / 0.65f;
                if (lt <= 0 || lt >= 1) continue;
                float env = sinf(PI * lt);
                float r = S * (0.07f + 0.10f * p.sp);
                float px = cx + (p.sz - 0.5f) * S * 0.9f + sinf(lt * 6 + p.ph * 6) * S * 0.08f;
                float py = cy + S * 0.25f - S * 1.35f * lt;
                float al = env * 0.9f * ec;
                Ring(buf, C, px, py, 0, r, {150, 210, 255}, al * 0.25f);
                Ring(buf, C, px, py, r, 1.6f, {210, 238, 255}, al);
                Ring(buf, C, px - r * 0.35f, py - r * 0.35f, 0, r * 0.3f, white, al * 0.9f);
            }
            break;
        }
        case E_LOVE: {
            float k = fminf(1.0f, Bump(t, 0.15f, 0.08f) + 0.7f * Bump(t, 0.38f, 0.09f));
            Patch(buf, C, cx, cy, A, Clamp01(k * 4));
            DrawIcon(buf, C, A, cx, cy, 1 + 0.12f * e * k, 0, white, 0, 1.0f);
            static const Color pinks[3] = {{255, 70, 110}, {255, 130, 170}, {255, 50, 80}};
            for (int i = 0; i < 6; i++) {
                const Part& p = A.pt[i];
                float lt = (t - p.ph * 0.4f) / 0.6f;
                if (lt <= 0 || lt >= 1) continue;
                float env = sinf(PI * lt);
                float px = cx + (p.sz - 0.5f) * S * 1.0f + sinf(lt * 5 + p.ph * 6) * S * 0.07f;
                float py = cy - S * 0.1f - S * 1.1f * lt;
                float size = S * (0.08f + 0.07f * p.sp) * (0.5f + 0.5f * env) * (0.7f + 0.3f * ec);
                Heart(buf, C, px, py, size, pinks[i % 3], env);
            }
            break;
        }
    }

    POINT dst = {A.center.x - C / 2, A.center.y - C / 2};
    SIZE sz = {C, C};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &sz, g_memdc, &src, 0, &bf,
                        ULW_ALPHA);
}

static bool g_shown = false;

// Başlat menüsü açılıp kapanınca görev çubuğu kendini öne alıp katmanı
// altta bırakabilir. Önce NOTOPMOST sonra TOPMOST yapmak katmanı gerçekten
// en üste taşır (zaten topmost olan pencerede tek çağrı sırayı değiştirmez).
static void RaiseOverlay() {
    if (!g_hwnd) return;
    UINT f = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW;
    SetWindowPos(g_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, f);
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, f);
}

// hide=false: katmanı görünür bırak ama içini boşalt (hover sürerken katman
// durumu animasyon sırasındakiyle aynı kalsın).
static void EndAnim(bool hide = true) {
    if (!hide && g_a.bits && g_hwnd && g_memdc) {
        memset(g_a.bits, 0, (size_t)g_a.C * g_a.C * 4);
        POINT dst = {g_a.center.x - g_a.C / 2, g_a.center.y - g_a.C / 2};
        SIZE sz = {g_a.C, g_a.C};
        POINT src = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_hwnd, nullptr, &dst, &sz, g_memdc, &src, 0, &bf,
                            ULW_ALPHA);
    } else if (g_hwnd) {
        ShowWindow(g_hwnd, SW_HIDE);
        g_shown = false;
    }
    if (g_memdc && g_a.oldBmp) {
        SelectObject(g_memdc, g_a.oldBmp);
        g_a.oldBmp = nullptr;
    }
    if (g_a.dib) {
        DeleteObject(g_a.dib);
        g_a.dib = nullptr;
    }
    g_a.bits = nullptr;
    g_a.icon.clear();
    g_a.active = false;
}

static float Rnd() { return (float)rand() / (float)RAND_MAX; }

static bool CaptureIcon(Anim& A) {
    const int side = A.side;
    HDC scr = GetDC(nullptr);
    if (!scr) return false;
    HDC mdc = CreateCompatibleDC(scr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = side;
    bi.bmiHeader.biHeight = -side;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        DeleteDC(mdc);
        ReleaseDC(nullptr, scr);
        return false;
    }
    HGDIOBJ old = SelectObject(mdc, bmp);
    BitBlt(mdc, 0, 0, side, side, scr, A.center.x - side / 2,
           A.center.y - side / 2, SRCCOPY | CAPTUREBLT);
    GdiFlush();

    const uint32_t* raw = (const uint32_t*)bits;
    auto px = [&](int x, int y) { return raw[y * side + x]; };
    uint32_t cs[4] = {px(1, 1), px(side - 2, 1), px(1, side - 2),
                      px(side - 2, side - 2)};
    float br = 0, bg = 0, bb = 0;
    for (uint32_t c : cs) {
        br += (float)((c >> 16) & 255);
        bg += (float)((c >> 8) & 255);
        bb += (float)(c & 255);
    }
    br /= 4;
    bg /= 4;
    bb /= 4;
    A.bg = {br, bg, bb};

    A.icon.assign((size_t)side * side, 0);
    float cxm = side * 0.5f;
    float maskR = side * 0.475f, maskF = side * 0.12f;
    for (int y = 0; y < side; y++) {
        for (int x = 0; x < side; x++) {
            uint32_t p = raw[y * side + x];
            float r = (float)((p >> 16) & 255), g = (float)((p >> 8) & 255),
                  b = (float)(p & 255);
            float d = fmaxf(fabsf(r - br), fmaxf(fabsf(g - bg), fabsf(b - bb)));
            float key = Clamp01((d - 12.0f) / 40.0f);
            float dx = x + 0.5f - cxm, dy = y + 0.5f - cxm;
            float m = Clamp01((maskR - sqrtf(dx * dx + dy * dy)) / maskF);
            float a = fmaxf(key, m);
            A.icon[y * side + x] = (Cl8(a * 255) << 24) | (Cl8(r * a) << 16) |
                                   (Cl8(g * a) << 8) | Cl8(b * a);
        }
    }

    SelectObject(mdc, old);
    DeleteObject(bmp);
    DeleteDC(mdc);
    ReleaseDC(nullptr, scr);
    return true;
}

static void StartAnim(const RECT& r, int forcedEffect) {
    if (g_a.active) {
        // Önceki katman ekran görüntüsüne girmesin.
        EndAnim();
        Sleep(20);
    }

    Settings s = g_s;
    Anim& A = g_a;
    A.center = {(r.left + r.right) / 2, (r.top + r.bottom) / 2};
    int minDim = std::min(r.right - r.left, r.bottom - r.top);
    A.side = std::max(16, (int)(minDim * 0.58f)) & ~1;
    A.C = std::max(64, A.side * 3) & ~1;
    A.duration = s.duration;
    A.e = s.intensity;
    A.accent = s.accent;

    int eff = forcedEffect >= 0 ? forcedEffect : s.effect;
    if (eff == E_RANDOM) eff = rand() % E_COUNT;
    else if (eff == E_CYCLE) eff = (g_cycle++) % E_COUNT;
    A.effect = eff;

    if (!CaptureIcon(A)) return;

    for (Sparkle& sp : A.sp) {
        sp.ang = Rnd() * 6.2831853f;
        sp.dist = 0.50f + Rnd() * 0.35f;
        sp.phase = Rnd() * 0.5f;
        sp.size = 0.18f + Rnd() * 0.17f;
        sp.gold = rand() % 2 == 0;
    }
    for (Part& p : A.pt) {
        p.a = Rnd() * 6.2831853f;
        p.sp = Rnd();
        p.sz = Rnd();
        p.ph = Rnd();
        p.hu = Rnd();
    }

    HDC scr = GetDC(nullptr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = A.C;
    bi.bmiHeader.biHeight = -A.C;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    A.dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, scr);
    if (!A.dib || !bits) {
        A.dib = nullptr;
        return;
    }
    A.bits = (uint32_t*)bits;
    A.oldBmp = SelectObject(g_memdc, A.dib);
    A.start = GetTickCount64();
    A.active = true;

    Render(0.0f);
    RaiseOverlay();
    g_shown = true;
}

// ---------------------------------------------------------------------------
// Görev çubuğu / UI Automation
// ---------------------------------------------------------------------------

// Imlecin altındaki gerçek pencere bir görev çubuğu mu? Tam ekran bir uygulama
// görev çubuğunu kapatıyorsa imlecin altındaki pencere o uygulama olur.
// Animasyon katmanı WS_EX_TRANSPARENT olduğu için WindowFromPoint onu atlar.
static bool InTaskbar(POINT pt) {
    HWND hit = WindowFromPoint(pt);
    HWND root = hit ? GetAncestor(hit, GA_ROOT) : nullptr;
    WCHAR cls[32];
    return root && GetClassNameW(root, cls, ARRAYSIZE(cls)) &&
           (!wcscmp(cls, L"Shell_TrayWnd") ||
            !wcscmp(cls, L"Shell_SecondaryTrayWnd"));
}

static bool ClassContains(IUIAutomationElement* el, const wchar_t* what,
                          bool exact) {
    BSTR cn = nullptr;
    bool res = false;
    if (SUCCEEDED(el->get_CurrentClassName(&cn)) && cn) {
        res = exact ? wcscmp(cn, what) == 0 : wcsstr(cn, what) != nullptr;
        SysFreeString(cn);
    }
    return res;
}

// ClassContains'in önbellekteki değeri okuyan hali (FindAllBuildCache sonrası).
static bool CachedClassContains(IUIAutomationElement* el, const wchar_t* what,
                                bool exact) {
    BSTR cn = nullptr;
    bool res = false;
    if (SUCCEEDED(el->get_CachedClassName(&cn)) && cn) {
        res = exact ? wcscmp(cn, what) == 0 : wcsstr(cn, what) != nullptr;
        SysFreeString(cn);
    }
    return res;
}

static bool ProbeButton(IUIAutomation* uia, IUIAutomationTreeWalker* walker,
                        POINT pt, bool onlyApp, RECT* out) {
    IUIAutomationElement* el = nullptr;
    if (FAILED(uia->ElementFromPoint(pt, &el)) || !el) return false;

    bool ok = false;
    IUIAutomationElement* cur = el;
    cur->AddRef();

    for (int depth = 0; depth < 4 && cur; depth++) {
        CONTROLTYPEID type = 0;
        if (SUCCEEDED(cur->get_CurrentControlType(&type)) &&
            type == UIA_ButtonControlTypeId) {
            RECT r = {};
            if (SUCCEEDED(cur->get_CurrentBoundingRectangle(&r)) &&
                PtInRect(&r, pt)) {
                int w = r.right - r.left, h = r.bottom - r.top;
                if (w >= 16 && h >= 16 && w <= 220 && h <= 220) {
                    bool pass = !onlyApp;
                    if (!pass) pass = ClassContains(cur, L"TaskListButton", false);
                    if (!pass) {
                        IUIAutomationElement* p = nullptr;
                        if (SUCCEEDED(walker->GetParentElement(cur, &p)) && p) {
                            pass = ClassContains(p, L"MSTaskListWClass", true);
                            p->Release();
                        }
                    }
                    if (pass) {
                        *out = r;
                        ok = true;
                    }
                }
            }
            break;
        }
        IUIAutomationElement* p = nullptr;
        if (FAILED(walker->GetParentElement(cur, &p))) p = nullptr;
        cur->Release();
        cur = p;
    }
    if (cur) cur->Release();
    el->Release();
    return ok;
}

// Düğme konumlarını önbelleğe al: ElementFromPoint animasyon katmanına
// takılabildiği için fare konumunu kendimiz eşleştiriyoruz.
static std::vector<RECT> g_btns;
static ULONGLONG g_btnsTime = 0;

static void RefreshButtons(IUIAutomation* uia,
                           IUIAutomationTreeWalker* walker) {
    g_btns.clear();
    g_btnsTime = GetTickCount64();

    std::vector<HWND> bars;
    HWND h = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (h && IsWindowVisible(h)) bars.push_back(h);
    HWND prev = nullptr;
    while ((prev = FindWindowExW(nullptr, prev, L"Shell_SecondaryTrayWnd",
                                 nullptr)) != nullptr) {
        if (IsWindowVisible(prev)) bars.push_back(prev);
    }
    if (bars.empty()) return;

    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* cond = nullptr;
    if (FAILED(uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v,
                                            &cond)) ||
        !cond)
        return;

    // Konum ve sınıf adı tek seferde (ve üst öğeyle birlikte) önbelleğe alınır;
    // böylece her düğme için ayrı bir süreçler arası çağrı yapılmaz.
    IUIAutomationCacheRequest* cache = nullptr;
    if (FAILED(uia->CreateCacheRequest(&cache)) || !cache) {
        cond->Release();
        return;
    }
    cache->AddProperty(UIA_BoundingRectanglePropertyId);
    cache->AddProperty(UIA_ClassNamePropertyId);
    cache->put_TreeScope((TreeScope)(TreeScope_Element | TreeScope_Parent));

    for (HWND bar : bars) {
        IUIAutomationElement* root = nullptr;
        if (FAILED(uia->ElementFromHandle(bar, &root)) || !root) continue;
        IUIAutomationElementArray* arr = nullptr;
        if (SUCCEEDED(root->FindAllBuildCache(TreeScope_Descendants, cond,
                                              cache, &arr)) &&
            arr) {
            int n = 0;
            arr->get_Length(&n);
            for (int i = 0; i < n; i++) {
                IUIAutomationElement* el = nullptr;
                if (FAILED(arr->GetElement(i, &el)) || !el) continue;
                RECT r = {};
                if (SUCCEEDED(el->get_CachedBoundingRectangle(&r))) {
                    int w = r.right - r.left, hh = r.bottom - r.top;
                    if (w >= 16 && hh >= 16 && w <= 220 && hh <= 220) {
                        bool pass = !g_s.onlyApp;
                        if (!pass)
                            pass = CachedClassContains(el, L"TaskListButton",
                                                       false);
                        if (!pass) {
                            IUIAutomationElement* p = nullptr;
                            if (SUCCEEDED(el->GetCachedParent(&p)) && p) {
                                pass = CachedClassContains(
                                    p, L"MSTaskListWClass", true);
                                p->Release();
                            } else if (SUCCEEDED(
                                           walker->GetParentElement(el, &p)) &&
                                       p) {
                                // Önbellekte üst öğe yoksa canlı sorguya dön.
                                pass = ClassContains(p, L"MSTaskListWClass",
                                                     true);
                                p->Release();
                            }
                        }
                        if (pass) g_btns.push_back(r);
                    }
                }
                el->Release();
            }
            arr->Release();
        }
        root->Release();
    }
    cache->Release();
    cond->Release();
}

static bool FindButton(POINT pt, RECT* out) {
    // Kutular birbirine taşabilir: fareyi kapsayanlar arasından merkezi
    // fareye en yakın olanı seç.
    double best = 0;
    bool found = false;
    for (const RECT& r : g_btns) {
        if (!PtInRect(&r, pt)) continue;
        double mx = (r.left + r.right) * 0.5, my = (r.top + r.bottom) * 0.5;
        double dx = pt.x - mx, dy = pt.y - my;
        double d = dx * dx + dy * dy;
        if (!found || d < best) {
            best = d;
            *out = r;
            found = true;
        }
    }
    return found;
}

// Aynı düğme mi? Önbellek yenilenince kutu birkaç piksel kayabilir; bu yüzden
// merkezlerin birbirine yakın olup olmadığına bakıyoruz.
static bool SimilarRect(const RECT& a, const RECT& b) {
    int w = a.right - a.left, h = a.bottom - a.top;
    int dx = abs((a.left + a.right) / 2 - (b.left + b.right) / 2);
    int dy = abs((a.top + a.bottom) / 2 - (b.top + b.bottom) / 2);
    return dx <= w / 3 && dy <= h / 3;
}

// ---------------------------------------------------------------------------
// Katman penceresi ve iş parçacığı
// ---------------------------------------------------------------------------

static LRESULT CALLBACK OverlayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(h, m, w, l);
}

// Başlatma sonucunu WhTool_ModInit'e bildirir.
static void SignalReady(bool ok) {
    g_initOk = ok;
    if (g_readyEvent) SetEvent(g_readyEvent);
}

static DWORD WINAPI Worker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    srand((unsigned)GetTickCount64());

    IUIAutomation* uia = nullptr;
    IUIAutomationTreeWalker* walker = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia));
    if (FAILED(hr) || !uia || FAILED(uia->get_ControlViewWalker(&walker)) ||
        !walker) {
        Wh_Log(L"UI Automation baslatilamadi: 0x%08X", (unsigned)hr);
        if (walker) walker->Release();
        if (uia) uia->Release();
        CoUninitialize();
        SignalReady(false);
        return 1;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkTaskbarHoverAnimOverlay";
    ATOM atom = RegisterClassW(&wc);
    if (!atom) {
        // Sınıf kaydedilemediyse (ör. önceki bir örnekten kalmış), o sınıfın
        // pencere işlevi artık geçersiz olabilir. Pencere oluşturmadan çık.
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        walker->Release();
        uia->Release();
        CoUninitialize();
        SignalReady(false);
        return 1;
    }

    const DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                          WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

    // Görev çubuğu ve Başlat menüsü sıradan "topmost" pencerelerin üstüne
    // çıkabildiği için katmanı UIAccess band'ında (ZBID_UIACCESS = 2)
    // oluşturmayı dene. Olmazsa normal pencereye düş.
    typedef HWND(WINAPI * CreateWindowInBand_t)(
        DWORD, ATOM, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU,
        HINSTANCE, LPVOID, DWORD);
    CreateWindowInBand_t pCreateInBand = (CreateWindowInBand_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "CreateWindowInBand");
    g_hwnd = nullptr;
    if (pCreateInBand) {
        g_hwnd = pCreateInBand(exStyle, atom, L"", WS_POPUP, 0, 0, 1, 1,
                               nullptr, nullptr, wc.hInstance, nullptr, 2);
        Wh_Log(L"CreateWindowInBand: %p (err=%lu)", g_hwnd, GetLastError());
    }
    if (!g_hwnd) {
        g_hwnd = CreateWindowExW(exStyle, wc.lpszClassName, L"", WS_POPUP, 0,
                                 0, 1, 1, nullptr, nullptr, wc.hInstance,
                                 nullptr);
        Wh_Log(L"Normal pencere kullaniliyor: %p", g_hwnd);
    }
    if (!g_hwnd) {
        Wh_Log(L"Overlay window creation failed: %lu", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        walker->Release();
        uia->Release();
        CoUninitialize();
        SignalReady(false);
        return 1;
    }

    HDC scr = GetDC(nullptr);
    g_memdc = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);

    SignalReady(true);

    RECT hoverRect = {};
    bool hoverValid = false;
    bool wasInBar = false;
    int plays = 0;
    ULONGLONG lastProbe = 0;
    ULONGLONG lastRaise = 0;

    while (!g_stop) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        ULONGLONG now = GetTickCount64();

        if (now - lastProbe >= 30) {
            lastProbe = now;
            POINT pt;
            RECT r = {};
            bool on = false;
            bool inBar = GetCursorPos(&pt) && InTaskbar(pt);
            if (!inBar && g_shown && !g_a.active) {
                ShowWindow(g_hwnd, SW_HIDE);
                g_shown = false;
            }
            if (inBar) {
                ULONGLONG age = now - g_btnsTime;
                // Görev çubuğuna girişte ya da önbellek çok eskiyse yenile.
                if (!wasInBar || age >= 5000u) {
                    RefreshButtons(uia, walker);
                }
                on = FindButton(pt, &r);
                // Yeni açılan/kapanan ikonlar için, ama en çok saniyede bir.
                if (!on && GetTickCount64() - g_btnsTime >= 1000) {
                    RefreshButtons(uia, walker);
                    on = FindButton(pt, &r);
                }
                if (!on && g_btns.empty()) {
                    on = ProbeButton(uia, walker, pt, g_s.onlyApp, &r);
                }
            }
            wasInBar = inBar;
            if (on) {
                if (!hoverValid || !SimilarRect(r, hoverRect)) {
                    hoverRect = r;
                    hoverValid = true;
                    Wh_Log(L"Hover: [%ld,%ld,%ld,%ld] buttons=%d", r.left,
                           r.top, r.right, r.bottom, (int)g_btns.size());
                    plays = 1;
                    StartAnim(r, -1);
                    // Başlatılamadıysa bir sonraki kontrolde yeniden dene.
                    if (!g_a.active) hoverValid = false;
                }
            } else {
                hoverValid = false;
            }
        }

        if (g_a.active) {
            float t = (float)(now - g_a.start) / (float)g_a.duration;
            if (t >= 1.0f) {
                if (hoverValid &&
                    (g_s.repeat == 0 || plays < g_s.repeat)) {
                    plays++;
                    StartAnim(hoverRect, -1);
                    if (!g_a.active) hoverValid = false;
                } else {
                    EndAnim(false);
                }
            } else {
                if (now - lastRaise >= 250) {
                    RaiseOverlay();
                    lastRaise = now;
                }
                Render(t);
            }
        }

        // Animasyon sırasında ~kare hızında, görev çubuğundayken orta,
        // görev çubuğu dışındayken seyrek uyan.
        DWORD waitMs = g_a.active ? 16 : (wasInBar ? 30 : 100);
        MsgWaitForMultipleObjects(0, nullptr, FALSE, waitMs, QS_ALLINPUT);
    }

    EndAnim();
    if (g_memdc) DeleteDC(g_memdc);
    g_memdc = nullptr;
    if (g_hwnd) DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    walker->Release();
    uia->Release();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod giriş noktaları
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Taskbar Hover Animations basliyor");
    LoadSettings();
    g_stop = false;
    g_initOk = false;

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // İş parçacığı başlatmayı bitirene kadar bekle (başarısızlıkta da sinyal verir).
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
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Taskbar Hover Animations kapaniyor");
    g_stop = true;
    if (g_thread) {
        // Iş parçacığı bitmeden mod kaldırılırsa kodu bellekten silinir ve
        // Explorer çökebilir; bu yüzden süresiz bekle.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
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
