// ==WindhawkMod==
// @id              hourly-peek-buddy
// @name            Hourly Peek Buddy
// @name:tr-TR      Saat Başı Sürpriz Dostu
// @description     A fun reminder for the top of every hour or at the times you choose: a funny little character pops up from the bottom corner of your screen, on top of all your apps, with a speech bubble and a preview in the settings
// @description:tr-TR Saat başı veya istenilen sürelerde eğlenceli hatırlatıcıdır: ekranın alt köşesinden, tüm uygulamaların üzerine, konuşma balonlu komik bir karakter çıkar; ayarlarda ön izleme de vardır
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hourly Peek Buddy

![Screenshot](https://i.imgur.com/pSUzh1o.gif)

A fun reminder for the top of every hour or at the times you choose.

A funny little character pops up from the bottom corner of your screen, waves
at you with a speech bubble, and goes away again. It shows up at the top of every
hour and at any other times you choose, and it appears on top of all your
apps. There are 14 characters and 6 ways of showing up, and you can see the
result right away with the built-in preview, so you never have to wait for the
next hour.

All characters are drawn by the mod itself, so there are no images to download.

## Preview
Windhawk settings cannot have a button, so the preview works like this:

- **Preview when settings are saved** (on by default): every time you press
  *Save*, the character shows up once with your new settings.
- **Preview now**: flip this switch and save, and the character shows up again
  even if nothing else has changed.

Tip: pick a character or a movement, save, and watch. Repeat until you find your favorite.

## Characters
Ghost, Cat, Alien, Robot, Penguin (with a party hat), Duck, Slime, Grumpy bomb,
Cactus, Pumpkin, Teddy bear, Mushroom, Donut and Pizza slice. Or choose
**Random** to be surprised every time.

## Ways of showing up
- **Peek**: Rises from the bottom edge and wiggles.
- **Springy**: Pops up like a jack-in-the-box and bounces.
- **Slide**: Slides in from the side of the screen with a little overshoot.
- **Hop**: Hops in from the side of the screen.
- **Spin**: Flies in from the corner while spinning.
- **Pop**: Grows out of the corner with a bounce.

## When does it show up?
- At the top of every hour (can be turned off).
- Every N minutes, counted from midnight (for example 15 means :00, :15, :30
  and :45; 0 turns this off).
- At exact times of your choice, for example `09:30, 13:15, 17:45`.
- Quiet hours: it stays away between the two hours you choose (for example from
  23 to 8). Using the same value for both turns quiet hours off.
- Optional: skip it while a full-screen game, video or presentation is active.

## Settings
- Preview when settings are saved / Preview now
- Character, Movement, Corner (bottom right or bottom left), Monitor (primary
  or the one with the mouse)
- Schedule: top of every hour, every N minutes, exact times, quiet hours
- Stay time (seconds), Size (%), Opacity (%)
- Speech bubble: time with a funny message, time only, your own message
  (`{time}` is replaced with the current time), or no bubble
- Message language: automatic (follows Windows), Turkish or English
- Click to dismiss, Play a sound, Skip full-screen apps

## Notes
- The mod runs in its own `windhawk.exe` process and does not inject into
  other programs. It does not change any system or registry setting.
- The character window never takes the keyboard focus. If **Click to dismiss**
  is off, the character is click-through and the mouse goes straight to the
  app below.
- The size follows the DPI of the monitor.
- It appears above the taskbar edge (the work area), as if it peeks out from
  behind it.
- If you start a new show (for example with the preview) while one is on the
  screen, the old one is replaced.

---

# Saat Başı Sürpriz Dostu

Ekranın alt köşesinden komik, küçük bir karakter çıkar, konuşma balonuyla size
göz kırpar ve tekrar gider. Her saat başı ve seçtiğiniz diğer zamanlarda ortaya
çıkar, üstelik tüm uygulamaların üzerinde görünür. 14 karakter ve 6 farklı
çıkış şekli vardır; yerleşik ön izleme sayesinde sonucu hemen görürsünüz, bir
sonraki saat başını beklemenize gerek kalmaz.

Tüm karakterleri modun kendisi çizer, bu yüzden indirilecek görsel yoktur.

## Ön izleme
Windhawk ayarlarında düğme olamadığı için ön izleme şöyle çalışır:

- **Ayarlar kaydedilince ön izle** (varsayılan olarak açık): *Kaydet*'e her
  bastığınızda karakter, yeni ayarlarınızla bir kez ortaya çıkar.
- **Şimdi ön izle**: Bu anahtarı çevirip kaydederseniz, başka hiçbir şey
  değişmemiş olsa bile karakter yeniden çıkar.

İpucu: Bir karakter veya hareket seçin, kaydedin ve izleyin. Favorinizi bulana
kadar tekrarlayın.

## Karakterler
Hayalet, Kedi, Uzaylı, Robot, Penguen (parti şapkalı), Ördek, Jöle, Huysuz
bomba, Kaktüs, Balkabağı, Oyuncak ayı, Mantar, Donut ve Pizza dilimi. Her
seferinde sürpriz için **Rastgele**'yi seçebilirsiniz.

## Çıkış şekilleri
- **Dikizleme**: Alt kenardan yükselir ve sallanır.
- **Yaylı**: Kutudan fırlayan oyuncak gibi çıkar ve zıplar.
- **Kayma**: Ekranın kenarından hafif taşarak kayarak gelir.
- **Hoplama**: Ekranın kenarından hoplaya hoplaya gelir.
- **Dönme**: Köşeden dönerek uçarak gelir.
- **Patlama**: Köşeden zıplayarak büyür.

## Ne zaman çıkar?
- Her saat başı (kapatılabilir).
- Her N dakikada bir, geceyarısından itibaren sayılır (örneğin 15 için :00,
  :15, :30 ve :45; 0 bunu kapatır).
- Seçtiğiniz tam saatlerde, örneğin `09:30, 13:15, 17:45`.
- Sessiz saatler: Seçtiğiniz iki saat arasında çıkmaz (örneğin 23 ile 8
  arası). İkisine de aynı değeri vermek sessiz saatleri kapatır.
- İsteğe bağlı: Tam ekran oyun, video veya sunum varken atlanır.

## Ayarlar
- Ayarlar kaydedilince ön izle / Şimdi ön izle
- Karakter, Hareket, Köşe (sağ alt veya sol alt), Monitör (ana monitör veya
  fare imlecinin olduğu monitör)
- Zamanlama: her saat başı, her N dakikada bir, tam saatler, sessiz saatler
- Kalma süresi (saniye), Boyut (%), Opaklık (%)
- Konuşma balonu: saat ve komik mesaj, sadece saat, kendi mesajınız
  (`{time}` yerine o anki saat yazılır) veya balonsuz
- Mesaj dili: otomatik (Windows'u izler), Türkçe veya İngilizce
- Tıklayınca kapat, Ses çal, Tam ekran uygulamaları atla

## Notlar
- Mod kendi `windhawk.exe` sürecinde çalışır ve başka programlara kod
  enjekte etmez. Hiçbir sistem veya kayıt defteri ayarını değiştirmez.
- Karakter penceresi klavye odağını almaz. **Tıklayınca kapat** kapalıysa
  karakter tıklamayı geçirir; fare doğrudan alttaki uygulamaya gider.
- Boyut, monitörün DPI değerini izler.
- Karakter görev çubuğunun kenarının (çalışma alanının) üstünde, sanki
  arkasından dikiliyormuş gibi görünür.
- Ekranda bir karakter varken yenisi başlatılırsa (örneğin ön izleme ile)
  eskisinin yerini alır.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- previewOnSave: true
  $name: Preview when settings are saved
  $name:tr-TR: Ayarlar kaydedilince ön izle
  $description: Shows the character once every time you save the settings, so you can see the result right away.
  $description:tr-TR: Ayarları her kaydettiğinizde karakteri bir kez gösterir; sonucu hemen görürsünüz.
- previewNow: false
  $name: Preview now
  $name:tr-TR: Şimdi ön izle
  $description: Flip this switch and save to show the character again, even if nothing else changed.
  $description:tr-TR: Bu anahtarı çevirip kaydederseniz, başka bir şey değişmemiş olsa bile karakter yeniden gösterilir.
- character: random
  $name: Character
  $name:tr-TR: Karakter
  $options:
  - random: Random (surprise me)
  - ghost: Ghost
  - cat: Cat
  - alien: Alien
  - robot: Robot
  - penguin: Penguin (with a party hat)
  - duck: Duck
  - slime: Slime
  - bomb: Grumpy bomb
  - cactus: Cactus
  - pumpkin: Pumpkin
  - bear: Teddy bear
  - mushroom: Mushroom
  - donut: Donut
  - pizza: Pizza slice
  $options:tr-TR:
  - random: Rastgele (sürpriz olsun)
  - ghost: Hayalet
  - cat: Kedi
  - alien: Uzaylı
  - robot: Robot
  - penguin: Penguen (parti şapkalı)
  - duck: Ördek
  - slime: Jöle
  - bomb: Huysuz bomba
  - cactus: Kaktüs
  - pumpkin: Balkabağı
  - bear: Oyuncak ayı
  - mushroom: Mantar
  - donut: Donut
  - pizza: Pizza dilimi
- motion: random
  $name: Movement
  $name:tr-TR: Hareket
  $options:
  - random: Random
  - peek: Peek (rises from the bottom edge)
  - springy: Springy (jack-in-the-box)
  - slide: Slide (from the side)
  - hop: Hop (hops in from the side)
  - spin: Spin (flies in spinning)
  - pop: Pop (grows out of the corner)
  $options:tr-TR:
  - random: Rastgele
  - peek: Dikizleme (alt kenardan yükselir)
  - springy: Yaylı (kutudan fırlayan oyuncak)
  - slide: Kayma (yandan kayar)
  - hop: Hoplama (yandan hoplayarak gelir)
  - spin: Dönme (dönerek uçar)
  - pop: Patlama (köşeden büyür)
- corner: right
  $name: Corner
  $name:tr-TR: Köşe
  $options:
  - right: Bottom right
  - left: Bottom left
  $options:tr-TR:
  - right: Sağ alt
  - left: Sol alt
- monitor: primary
  $name: Monitor
  $name:tr-TR: Monitör
  $options:
  - primary: Primary monitor
  - cursor: The monitor with the mouse pointer
  $options:tr-TR:
  - primary: Ana monitör
  - cursor: Fare imlecinin olduğu monitör
- onTheHour: true
  $name: Show at the top of every hour
  $name:tr-TR: Her saat başı göster
- intervalMinutes: 0
  $name: Also show every N minutes
  $name:tr-TR: Ayrıca her N dakikada bir göster
  $description: Counted from midnight, so 15 means :00, :15, :30 and :45. Use 0 to turn this off. Allowed range is 0 to 720.
  $description:tr-TR: Geceyarısından itibaren sayılır; 15 için :00, :15, :30 ve :45 olur. Kapatmak için 0 girin. İzin verilen aralık 0 ile 720 arasıdır.
- customTimes: ""
  $name: Exact times
  $name:tr-TR: Tam saatler
  $description: "Times of the day separated by commas, for example 09:30, 13:15, 17:45 (24-hour format)."
  $description:tr-TR: "Virgülle ayrılmış gün içi saatleri, örneğin 09:30, 13:15, 17:45 (24 saat biçimi)."
- quietFrom: 0
  $name: Quiet hours - from (hour, 0-23)
  $name:tr-TR: Sessiz saatler - başlangıç (saat, 0-23)
  $description: It does not show up between the two hours. Use the same value for both to turn quiet hours off. Example 23 and 8 means from 23:00 to 08:00.
  $description:tr-TR: İki saat arasında ortaya çıkmaz. Kapatmak için ikisine de aynı değeri verin. Örneğin 23 ve 8, 23:00 ile 08:00 arasıdır.
- quietTo: 0
  $name: Quiet hours - to (hour, 0-23)
  $name:tr-TR: Sessiz saatler - bitiş (saat, 0-23)
- stayTime: 6
  $name: Stay time (seconds)
  $name:tr-TR: Kalma süresi (saniye)
  $description: How long the character stays after it has shown up. Allowed range is 2 to 60.
  $description:tr-TR: Karakter ortaya çıktıktan sonra ne kadar kalacağı. İzin verilen aralık 2 ile 60 arasıdır.
- sizePercent: 100
  $name: Size (%)
  $name:tr-TR: Boyut (%)
  $description: 100 is about 200 pixels at a normal DPI. Allowed range is 40 to 300.
  $description:tr-TR: 100, normal DPI'da yaklaşık 200 pikseldir. İzin verilen aralık 40 ile 300 arasıdır.
- opacity: 100
  $name: Opacity (%)
  $name:tr-TR: Opaklık (%)
  $description: Allowed range is 30 to 100.
  $description:tr-TR: İzin verilen aralık 30 ile 100 arasıdır.
- bubble: funny
  $name: Speech bubble
  $name:tr-TR: Konuşma balonu
  $options:
  - funny: Time and a funny message
  - time: Time only
  - custom: My own message
  - none: No bubble
  $options:tr-TR:
  - funny: Saat ve komik bir mesaj
  - time: Sadece saat
  - custom: Kendi mesajım
  - none: Balon yok
- customMessage: "It's {time}!"
  $name: My own message
  $name:tr-TR: Kendi mesajım
  $description: "Used when the speech bubble is set to My own message. {time} is replaced with the current time."
  $description:tr-TR: "Konuşma balonu Kendi mesajım olarak seçiliyse kullanılır. {time} yerine o anki saat yazılır."
- messageLanguage: auto
  $name: Language of the funny messages
  $name:tr-TR: Komik mesajların dili
  $options:
  - auto: Automatic (follows Windows)
  - tr: Turkish
  - en: English
  $options:tr-TR:
  - auto: Otomatik (Windows'u izler)
  - tr: Türkçe
  - en: İngilizce
- clickToDismiss: true
  $name: Click to dismiss
  $name:tr-TR: Tıklayınca kapat
  $description: Clicking the character makes it giggle and leave. When this is off, the character is click-through.
  $description:tr-TR: Karaktere tıklamak onu kıkırdatıp gönderir. Kapalıyken karakter tıklamayı geçirir.
- playSound: false
  $name: Play a sound
  $name:tr-TR: Ses çal
  $description: Plays the default Windows beep when the character shows up.
  $description:tr-TR: Karakter ortaya çıkınca Windows'un varsayılan bip sesini çalar.
- skipFullscreen: false
  $name: Skip full-screen apps
  $name:tr-TR: Tam ekran uygulamaları atla
  $description: Does not show up while a full-screen game, video or presentation is active. The preview ignores this.
  $description:tr-TR: Tam ekran oyun, video veya sunum açıkken ortaya çıkmaz. Ön izleme bunu yok sayar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Tiny software rasterizer (anti-aliased shapes). It does not depend on any
// Windows API. Characters are drawn in a 200x200 design space.
// ---------------------------------------------------------------------------

struct Pt {
    float x, y;
};

struct RGBA {
    float r, g, b, a;  // r, g, b: 0..255, a: 0..1
};

enum ShapeType { SH_CIRCLE, SH_ELLIPSE, SH_CAPSULE, SH_BOX, SH_RING, SH_POLY };

struct Sh {
    int t = SH_CIRCLE;
    float a = 0, b = 0, c = 0, d = 0, e = 0;
    float cs = 1, sn = 0;  // rotation (ellipse, box)
    std::vector<Pt> p;
};

static Sh Circ(float cx, float cy, float r) {
    Sh s;
    s.t = SH_CIRCLE;
    s.a = cx;
    s.b = cy;
    s.c = r;
    return s;
}

static Sh Ell(float cx, float cy, float rx, float ry, float rot = 0) {
    Sh s;
    s.t = SH_ELLIPSE;
    s.a = cx;
    s.b = cy;
    s.c = rx;
    s.d = ry;
    s.cs = cosf(rot);
    s.sn = sinf(rot);
    return s;
}

static Sh Cap(float ax, float ay, float bx, float by, float r) {
    Sh s;
    s.t = SH_CAPSULE;
    s.a = ax;
    s.b = ay;
    s.c = bx;
    s.d = by;
    s.e = r;
    return s;
}

static Sh Box(float cx, float cy, float hw, float hh, float r, float rot = 0) {
    Sh s;
    s.t = SH_BOX;
    s.a = cx;
    s.b = cy;
    s.c = hw;
    s.d = hh;
    s.e = std::min(r, std::min(hw, hh));
    s.cs = cosf(rot);
    s.sn = sinf(rot);
    return s;
}

static Sh Ring(float cx, float cy, float r, float halfWidth) {
    Sh s;
    s.t = SH_RING;
    s.a = cx;
    s.b = cy;
    s.c = r;
    s.d = halfWidth;
    return s;
}

static Sh PolyV(const std::vector<Pt>& pts) {
    Sh s;
    s.t = SH_POLY;
    s.p = pts;
    return s;
}

static Sh Poly(std::initializer_list<Pt> pts) {
    return PolyV(std::vector<Pt>(pts));
}

static float SegDist(float px, float py, float ax, float ay, float bx,
                     float by) {
    float vx = bx - ax, vy = by - ay, wx = px - ax, wy = py - ay;
    float l2 = vx * vx + vy * vy;
    float t = l2 > 0 ? (wx * vx + wy * vy) / l2 : 0;
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    float dx = wx - vx * t, dy = wy - vy * t;
    return sqrtf(dx * dx + dy * dy);
}

// Signed distance: negative inside the shape.
static float ShapeSD(const Sh& s, float x, float y) {
    switch (s.t) {
        case SH_CIRCLE:
            return hypotf(x - s.a, y - s.b) - s.c;
        case SH_ELLIPSE: {
            float dx = x - s.a, dy = y - s.b;
            float lx = dx * s.cs + dy * s.sn;
            float ly = -dx * s.sn + dy * s.cs;
            float qx = lx / s.c, qy = ly / s.d;
            float k0 = sqrtf(qx * qx + qy * qy);
            float gx = lx / (s.c * s.c), gy = ly / (s.d * s.d);
            float k1 = sqrtf(gx * gx + gy * gy);
            if (k1 < 1e-6f) return -std::min(s.c, s.d);
            return k0 * (k0 - 1.0f) / k1;
        }
        case SH_CAPSULE:
            return SegDist(x, y, s.a, s.b, s.c, s.d) - s.e;
        case SH_BOX: {
            float dx = x - s.a, dy = y - s.b;
            float lx = dx * s.cs + dy * s.sn;
            float ly = -dx * s.sn + dy * s.cs;
            float qx = fabsf(lx) - (s.c - s.e);
            float qy = fabsf(ly) - (s.d - s.e);
            float ox = std::max(qx, 0.0f), oy = std::max(qy, 0.0f);
            return sqrtf(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0f) -
                   s.e;
        }
        case SH_RING:
            return fabsf(hypotf(x - s.a, y - s.b) - s.c) - s.d;
        default: {
            size_t n = s.p.size();
            if (n < 3) return 1e9f;
            float dmin = 1e9f;
            bool inside = false;
            for (size_t i = 0, j = n - 1; i < n; j = i++) {
                const Pt& pi = s.p[i];
                const Pt& pj = s.p[j];
                float d = SegDist(x, y, pi.x, pi.y, pj.x, pj.y);
                if (d < dmin) dmin = d;
                if (((pi.y > y) != (pj.y > y)) &&
                    (x < (pj.x - pi.x) * (y - pi.y) / (pj.y - pi.y) + pi.x)) {
                    inside = !inside;
                }
            }
            return inside ? -dmin : dmin;
        }
    }
}

static void ShapeBounds(const Sh& s, float& x0, float& y0, float& x1,
                        float& y1) {
    switch (s.t) {
        case SH_CIRCLE:
            x0 = s.a - s.c, x1 = s.a + s.c, y0 = s.b - s.c, y1 = s.b + s.c;
            break;
        case SH_ELLIPSE: {
            float r = std::max(s.c, s.d);
            x0 = s.a - r, x1 = s.a + r, y0 = s.b - r, y1 = s.b + r;
            break;
        }
        case SH_CAPSULE:
            x0 = std::min(s.a, s.c) - s.e, x1 = std::max(s.a, s.c) + s.e;
            y0 = std::min(s.b, s.d) - s.e, y1 = std::max(s.b, s.d) + s.e;
            break;
        case SH_BOX: {
            float r = hypotf(s.c, s.d);
            x0 = s.a - r, x1 = s.a + r, y0 = s.b - r, y1 = s.b + r;
            break;
        }
        case SH_RING: {
            float r = s.c + s.d;
            x0 = s.a - r, x1 = s.a + r, y0 = s.b - r, y1 = s.b + r;
            break;
        }
        default:
            x0 = y0 = 1e9f;
            x1 = y1 = -1e9f;
            for (const Pt& q : s.p) {
                x0 = std::min(x0, q.x), x1 = std::max(x1, q.x);
                y0 = std::min(y0, q.y), y1 = std::max(y1, q.y);
            }
            break;
    }
}

static inline float Clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

static inline uint32_t Cl8(float v) {
    int i = (int)(v + 0.5f);
    return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
}

static const RGBA kNoColor = {0, 0, 0, -1};

// Pixel buffer with premultiplied alpha (0xAARRGGBB). The design space is
// scaled by k to get pixels.
struct Canvas {
    int w, h;
    float k;
    std::vector<uint32_t> px;

    Canvas(int w_, int h_, float k_)
        : w(w_), h(h_), k(k_), px((size_t)w_ * h_, 0) {}

    void Blend(int x, int y, float r, float g, float b, float a) {
        if (a <= 0.0f) return;
        uint32_t& d = px[(size_t)y * w + x];
        float inv = 1.0f - a;
        float da = (float)((d >> 24) & 255), dr = (float)((d >> 16) & 255);
        float dg = (float)((d >> 8) & 255), db = (float)(d & 255);
        uint32_t oa = Cl8(a * 255.0f + da * inv);
        uint32_t orr = Cl8(r * a + dr * inv);
        uint32_t og = Cl8(g * a + dg * inv);
        uint32_t ob = Cl8(b * a + db * inv);
        d = (oa << 24) | (orr << 16) | (og << 8) | ob;
    }

    // Draws the union of the shapes: outline first, then the fill. fill2
    // (optional) makes a top-to-bottom gradient. soft > 0 blurs the edge.
    void Fill(const std::vector<Sh>& shapes, RGBA fill, RGBA outline,
              float ow, float soft = 0.0f, RGBA fill2 = kNoColor) {
        if (shapes.empty()) return;
        float bx0 = 1e9f, by0 = 1e9f, bx1 = -1e9f, by1 = -1e9f;
        for (const Sh& s : shapes) {
            float x0, y0, x1, y1;
            ShapeBounds(s, x0, y0, x1, y1);
            bx0 = std::min(bx0, x0), by0 = std::min(by0, y0);
            bx1 = std::max(bx1, x1), by1 = std::max(by1, y1);
        }
        float ex = ow + soft + 2.0f / k;
        int ix0 = std::max(0, (int)floorf((bx0 - ex) * k));
        int iy0 = std::max(0, (int)floorf((by0 - ex) * k));
        int ix1 = std::min(w, (int)ceilf((bx1 + ex) * k));
        int iy1 = std::min(h, (int)ceilf((by1 + ex) * k));
        float softPx = std::max(1.0f, soft * k);
        float spanY = std::max(1.0f, by1 - by0);
        bool grad = fill2.a >= 0.0f;

        for (int y = iy0; y < iy1; y++) {
            float dy = (y + 0.5f) / k;
            for (int x = ix0; x < ix1; x++) {
                float dx = (x + 0.5f) / k;
                float d = 1e9f;
                for (const Sh& s : shapes) d = std::min(d, ShapeSD(s, dx, dy));
                if (ow > 0 && outline.a > 0) {
                    float co = Clamp01(0.5f - (d - ow) * k / softPx);
                    if (co > 0)
                        Blend(x, y, outline.r, outline.g, outline.b,
                              outline.a * co);
                }
                float cf = Clamp01(0.5f - d * k / softPx);
                if (cf > 0 && fill.a > 0) {
                    RGBA c = fill;
                    if (grad) {
                        float t = Clamp01((dy - by0) / spanY);
                        c.r = fill.r + (fill2.r - fill.r) * t;
                        c.g = fill.g + (fill2.g - fill.g) * t;
                        c.b = fill.b + (fill2.b - fill.b) * t;
                        c.a = fill.a + (fill2.a - fill.a) * t;
                    }
                    Blend(x, y, c.r, c.g, c.b, c.a * cf);
                }
            }
        }
    }
};

// ---------------------------------------------------------------------------
// Drawing helpers
// ---------------------------------------------------------------------------

static const RGBA INK = {38, 30, 58, 1};
static const RGBA WHITE = {255, 255, 255, 1};
static const float OW = 4.0f;  // default outline width (design units)

static RGBA C3(float r, float g, float b, float a = 1.0f) {
    return RGBA{r, g, b, a};
}

// Outlined part (union of shapes).
static void Part(Canvas& c, const std::vector<Sh>& s, RGBA fill,
                 RGBA fill2 = kNoColor, float ow = OW) {
    c.Fill(s, fill, INK, ow, 0.0f, fill2);
}

// Flat shape without an outline.
static void Flat(Canvas& c, const std::vector<Sh>& s, RGBA fill,
                 float soft = 0.0f) {
    c.Fill(s, fill, INK, 0.0f, soft);
}

static void Shadow(Canvas& c, float cx, float cy, float rx, float ry) {
    Flat(c, {Ell(cx, cy, rx, ry)}, C3(0, 0, 0, 0.22f), 6.0f);
}

static void Blush(Canvas& c, float x, float y, float rx = 10, float ry = 6) {
    Flat(c, {Ell(x, y, rx, ry)}, C3(255, 90, 130, 0.42f), 5.0f);
}

static void ArcLine(Canvas& c, float cx, float cy, float r, float w, float a0,
                float a1, RGBA col) {
    std::vector<Sh> segs;
    const int n = 10;
    for (int i = 0; i < n; i++) {
        float t0 = (a0 + (a1 - a0) * i / n) * 3.14159265f / 180.0f;
        float t1 = (a0 + (a1 - a0) * (i + 1) / n) * 3.14159265f / 180.0f;
        segs.push_back(Cap(cx + r * cosf(t0), cy + r * sinf(t0),
                           cx + r * cosf(t1), cy + r * sinf(t1), w * 0.5f));
    }
    Flat(c, segs, col);
}

static void Smile(Canvas& c, float cx, float cy, float r, float w = 2.6f,
                  float a0 = 15, float a1 = 165, RGBA col = INK) {
    ArcLine(c, cx, cy, r, w, a0, a1, col);
}

static void Eye(Canvas& c, float x, float y, float r) {
    c.Fill({Circ(x, y, r)}, WHITE, INK, 2.2f);
    float px = x + r * 0.12f, py = y + r * 0.14f;
    Flat(c, {Circ(px, py, r * 0.6f)}, INK);
    Flat(c, {Circ(px - r * 0.2f, py - r * 0.24f, r * 0.2f)}, WHITE);
}

static void Dots(Canvas& c, float x, float y, float r) {
    Flat(c, {Circ(x, y, r)}, INK);
    Flat(c, {Circ(x - r * 0.3f, y - r * 0.35f, r * 0.34f)}, WHITE);
}

static std::vector<Pt> StarPts(float cx, float cy, float R, float r, int n,
                               float rot) {
    std::vector<Pt> v;
    for (int i = 0; i < n * 2; i++) {
        float a = rot + 3.14159265f * i / n;
        float rr = (i & 1) ? r : R;
        v.push_back({cx + rr * cosf(a), cy + rr * sinf(a)});
    }
    return v;
}

// ---------------------------------------------------------------------------
// Characters
// ---------------------------------------------------------------------------

static void DrawGhost(Canvas& c) {
    Shadow(c, 100, 186, 42, 6);
    Part(c,
         {Circ(100, 80, 50), Box(100, 118, 50, 38, 0), Circ(66.7f, 156, 16.7f),
          Circ(100, 156, 16.7f), Circ(133.3f, 156, 16.7f),
          Ell(46, 120, 10, 16, 0.5f), Ell(154, 120, 10, 16, -0.5f)},
         C3(252, 252, 255), C3(200, 210, 250));
    Flat(c, {Ell(82, 78, 7, 10), Ell(118, 78, 7, 10)}, INK);
    Flat(c, {Circ(84.5f, 73.5f, 2.6f), Circ(120.5f, 73.5f, 2.6f)}, WHITE);
    Flat(c, {Ell(100, 103, 6, 8)}, INK);
    Blush(c, 66, 96);
    Blush(c, 134, 96);
}

static void DrawCat(Canvas& c) {
    Shadow(c, 100, 187, 56, 7);
    const RGBA o1 = C3(255, 190, 95), o2 = C3(248, 148, 48);
    const RGBA cream = C3(255, 238, 215);
    std::vector<Sh> tail;
    Pt tp[] = {{140, 168}, {162, 164}, {176, 148}, {176, 126}, {167, 112}};
    for (int i = 0; i < 4; i++)
        tail.push_back(Cap(tp[i].x, tp[i].y, tp[i + 1].x, tp[i + 1].y, 8));
    Part(c, tail, o1, o2);
    Flat(c, {Circ(167, 113, 6.5f)}, cream);
    Part(c, {Ell(100, 142, 46, 40)}, o1, o2);
    Flat(c, {Ell(100, 152, 27, 26)}, cream);
    Part(c, {Ell(80, 180, 15, 9), Ell(120, 180, 15, 9)}, cream);
    Part(c,
         {Ell(100, 88, 52, 42), Poly({{52, 72}, {58, 20}, {96, 48}}),
          Poly({{148, 72}, {142, 20}, {104, 48}})},
         o1, o2);
    Flat(c, {Poly({{62, 62}, {64, 34}, {83, 50}}),
             Poly({{138, 62}, {136, 34}, {117, 50}})},
         C3(255, 150, 170));
    Flat(c, {Cap(100, 48, 100, 62, 2.6f), Cap(86, 52, 88, 63, 2.3f),
             Cap(114, 52, 112, 63, 2.3f)},
         C3(205, 100, 25));
    Flat(c, {Ell(100, 106, 24, 15)}, cream);
    for (int s = -1; s <= 1; s += 2) {
        float x = 100 + s * 22;
        c.Fill({Ell(x, 86, 9.5f, 12.5f)}, C3(140, 235, 125), INK, 2.2f);
        Flat(c, {Ell(x, 86, 3.6f, 10)}, INK);
        Flat(c, {Circ(x - 3, 80, 3)}, WHITE);
    }
    c.Fill({Poly({{94, 100}, {106, 100}, {100, 107}})}, C3(255, 120, 150), INK,
           1.4f);
    Smile(c, 94, 107, 6, 2.2f, 10, 170);
    Smile(c, 106, 107, 6, 2.2f, 10, 170);
    Flat(c,
         {Cap(72, 104, 44, 98, 0.9f), Cap(72, 109, 44, 112, 0.9f),
          Cap(128, 104, 156, 98, 0.9f), Cap(128, 109, 156, 112, 0.9f)},
         INK);
    Blush(c, 64, 104);
    Blush(c, 136, 104);
}

static void DrawAlien(Canvas& c) {
    Shadow(c, 100, 187, 40, 6);
    const RGBA s1 = C3(175, 248, 125), s2 = C3(85, 205, 105);
    Part(c, {Cap(68, 138, 50, 160, 6), Circ(48, 162, 7)}, s1, s2);
    Part(c, {Cap(132, 138, 150, 160, 6), Circ(152, 162, 7)}, s1, s2);
    Part(c, {Cap(88, 168, 86, 180, 7), Ell(82, 184, 13, 6)}, s1, s2);
    Part(c, {Cap(112, 168, 114, 180, 7), Ell(118, 184, 13, 6)}, s1, s2);
    Part(c, {Ell(100, 148, 30, 36)}, C3(120, 140, 255), C3(70, 80, 215));
    Flat(c, {Circ(100, 150, 8)}, C3(255, 225, 90));
    Flat(c, {Cap(82, 44, 68, 16, 2.5f), Cap(118, 44, 132, 16, 2.5f)}, INK);
    Part(c, {Circ(66, 12, 7)}, C3(255, 130, 205));
    Part(c, {Circ(134, 12, 7)}, C3(255, 130, 205));
    Part(c, {Ell(100, 84, 58, 52)}, s1, s2);
    Flat(c, {Ell(76, 88, 17, 23, 0.45f), Ell(124, 88, 17, 23, -0.45f)}, INK);
    Flat(c, {Circ(70, 78, 5), Circ(118, 78, 5)}, WHITE);
    Flat(c, {Circ(97, 104, 1.4f), Circ(103, 104, 1.4f)}, INK);
    Smile(c, 100, 114, 9, 2.6f, 20, 160);
}

static void DrawRobot(Canvas& c) {
    Shadow(c, 100, 187, 46, 6);
    const RGBA m1 = C3(225, 232, 245), m2 = C3(160, 172, 200);
    Part(c, {Box(84, 176, 8, 11, 3)}, m2, m2);
    Part(c, {Box(116, 176, 8, 11, 3)}, m2, m2);
    Part(c, {Cap(62, 128, 46, 150, 6), Circ(44, 153, 8)}, m1, m2);
    Part(c, {Cap(138, 128, 154, 150, 6), Circ(156, 153, 8)}, m1, m2);
    Part(c, {Box(100, 142, 36, 28, 9)}, m1, m2);
    c.Fill({Circ(86, 142, 5)}, C3(255, 90, 90), INK, 1.6f);
    c.Fill({Circ(100, 142, 5)}, C3(255, 215, 80), INK, 1.6f);
    c.Fill({Circ(114, 142, 5)}, C3(110, 235, 130), INK, 1.6f);
    Part(c, {Box(100, 112, 10, 6, 2)}, m2, m2);
    Flat(c, {Cap(100, 42, 100, 22, 2.4f)}, INK);
    Part(c, {Circ(100, 17, 7)}, C3(255, 90, 100));
    Part(c, {Box(100, 76, 48, 36, 14), Box(49, 78, 6, 14, 3),
             Box(151, 78, 6, 14, 3)},
         m1, m2);
    Flat(c, {Box(100, 78, 38, 25, 10)}, C3(28, 38, 78));
    Flat(c, {Circ(82, 76, 11), Circ(118, 76, 11)}, C3(120, 240, 255, 0.3f),
         8.0f);
    Flat(c, {Circ(82, 76, 7.5f), Circ(118, 76, 7.5f)}, C3(135, 245, 255));
    Flat(c, {Circ(79.5f, 73.5f, 2.4f), Circ(115.5f, 73.5f, 2.4f)}, WHITE);
    Smile(c, 100, 86, 9, 2.4f, 20, 160, C3(135, 245, 255));
}

static void DrawPenguin(Canvas& c) {
    Shadow(c, 100, 187, 50, 7);
    const RGBA d1 = C3(80, 92, 150), d2 = C3(34, 38, 78);
    Part(c, {Ell(80, 184, 17, 7)}, C3(255, 190, 60), C3(245, 140, 20));
    Part(c, {Ell(120, 184, 17, 7)}, C3(255, 190, 60), C3(245, 140, 20));
    Part(c, {Ell(46, 124, 11, 32, 0.28f)}, d1, d2);
    Part(c, {Ell(154, 124, 11, 32, -0.28f)}, d1, d2);
    Part(c, {Ell(100, 120, 54, 68)}, d1, d2);
    c.Fill({Ell(100, 144, 36, 42)}, C3(255, 255, 255), INK, 0.0f, 0.0f,
           C3(222, 232, 250));
    Eye(c, 84, 82, 10);
    Eye(c, 116, 82, 10);
    Part(c, {Poly({{88, 92}, {112, 92}, {100, 110}})}, C3(255, 185, 50),
         C3(245, 140, 20), 3.0f);
    Blush(c, 68, 98);
    Blush(c, 132, 98);
    Part(c, {Poly({{78, 56}, {122, 56}, {100, 8}})}, C3(255, 110, 175),
         C3(235, 60, 140));
    Flat(c, {Circ(95, 34, 3.6f), Circ(106, 44, 3.6f), Circ(100, 24, 3)},
         C3(255, 235, 90));
    Part(c, {Circ(100, 8, 6)}, C3(255, 235, 90), kNoColor, 3.0f);
}

static void DrawDuck(Canvas& c) {
    Shadow(c, 100, 188, 52, 7);
    const RGBA y1 = C3(255, 240, 110), y2 = C3(255, 200, 40);
    const RGBA or1 = C3(255, 190, 60), or2 = C3(245, 130, 20);
    Part(c, {Ell(80, 185, 16, 6)}, or1, or2);
    Part(c, {Ell(120, 185, 16, 6)}, or1, or2);
    Part(c, {Poly({{140, 124}, {178, 102}, {170, 146}})}, y2, C3(245, 170, 25));
    Part(c, {Ell(100, 134, 54, 46)}, y1, y2);
    Part(c, {Ell(54, 140, 16, 26, 0.3f)}, C3(255, 215, 70), C3(240, 170, 30));
    Part(c, {Ell(146, 140, 16, 26, -0.3f)}, C3(255, 215, 70),
         C3(240, 170, 30));
    Part(c, {Cap(100, 42, 96, 26, 3), Cap(104, 42, 112, 30, 3),
             Cap(100, 42, 100, 24, 3)},
         y1, y2, 3.0f);
    Part(c, {Circ(100, 76, 38)}, y1, y2);
    Part(c, {Ell(100, 99, 17, 7)}, C3(250, 160, 40), or2, 3.0f);
    Part(c, {Ell(100, 90, 23, 10)}, or1, or2, 3.0f);
    Dots(c, 84, 68, 5.6f);
    Dots(c, 116, 68, 5.6f);
    Blush(c, 68, 84);
    Blush(c, 132, 84);
}

static void DrawSlime(Canvas& c) {
    Shadow(c, 100, 188, 62, 7);
    Part(c, {Ell(100, 132, 64, 54), Ell(100, 112, 46, 46)},
         C3(120, 232, 255, 0.95f), C3(60, 165, 245, 0.97f));
    Flat(c, {Ell(70, 100, 12, 7, -0.7f)}, C3(255, 255, 255, 0.7f));
    Flat(c, {Circ(56, 116, 4)}, C3(255, 255, 255, 0.7f));
    Eye(c, 80, 126, 10.5f);
    Eye(c, 120, 126, 10.5f);
    Flat(c, {Ell(100, 150, 15, 11)}, INK);
    Flat(c, {Ell(100, 155, 9, 5)}, C3(255, 120, 150));
    Blush(c, 64, 142);
    Blush(c, 136, 142);
    Flat(c, {Cap(100, 70, 100, 56, 2.5f)}, INK);
    Part(c, {Ell(112, 52, 13, 7, -0.5f)}, C3(130, 230, 110), kNoColor, 2.4f);
    Part(c, {Ell(88, 54, 13, 7, 0.5f)}, C3(130, 230, 110), kNoColor, 2.4f);
}

static void DrawBomb(Canvas& c) {
    Shadow(c, 100, 188, 50, 7);
    Part(c, {Ell(78, 184, 15, 7)}, C3(90, 95, 130));
    Part(c, {Ell(122, 184, 15, 7)}, C3(90, 95, 130));
    Part(c, {Circ(100, 126, 54)}, C3(105, 112, 148), C3(36, 38, 62));
    Flat(c, {Ell(76, 96, 13, 8, -0.7f)}, C3(255, 255, 255, 0.45f), 3.0f);
    Part(c, {Cap(100, 62, 106, 50, 3), Cap(106, 50, 118, 44, 3),
             Cap(118, 44, 124, 36, 3)},
         C3(170, 110, 60), kNoColor, 3.0f);
    Part(c, {Box(100, 72, 17, 10, 3)}, C3(190, 195, 210), C3(130, 135, 155));
    Part(c, {PolyV(StarPts(126, 28, 15, 6.5f, 8, 0.3f))}, C3(255, 225, 70),
         kNoColor, 2.4f);
    Flat(c, {Circ(126, 28, 4)}, WHITE);
    Eye(c, 82, 122, 11);
    Eye(c, 118, 122, 11);
    Flat(c, {Cap(64, 100, 92, 112, 3.4f), Cap(136, 100, 108, 112, 3.4f)}, INK);
    std::vector<Sh> zig;
    Pt zp[] = {{78, 148}, {86, 141}, {94, 148}, {102, 141},
               {110, 148}, {118, 141}, {124, 148}};
    for (int i = 0; i < 6; i++)
        zig.push_back(Cap(zp[i].x, zp[i].y, zp[i + 1].x, zp[i + 1].y, 1.7f));
    Flat(c, zig, WHITE);
    Part(c, {Ell(152, 96, 5, 8)}, C3(140, 215, 255), kNoColor, 2.2f);
}

static void DrawCactus(Canvas& c) {
    Shadow(c, 100, 188, 50, 7);
    const RGBA g1 = C3(120, 225, 130), g2 = C3(60, 165, 95);
    Part(c,
         {Box(100, 114, 28, 62, 26), Cap(72, 130, 54, 130, 9),
          Cap(54, 130, 54, 98, 9), Cap(128, 108, 146, 108, 9),
          Cap(146, 108, 146, 80, 9)},
         g1, g2);
    Flat(c, {Cap(86, 64, 86, 150, 1.8f), Cap(114, 64, 114, 150, 1.8f)},
         C3(40, 140, 80, 0.4f));
    Flat(c,
         {Cap(92, 72, 88, 68, 1), Cap(108, 86, 112, 82, 1), Cap(90, 140, 86,
                                                              136, 1),
          Cap(60, 108, 57, 104, 1), Cap(142, 92, 145, 88, 1)},
         C3(255, 250, 220));
    Part(c, {Poly({{60, 158}, {140, 158}, {131, 190}, {69, 190}})},
         C3(238, 140, 98), C3(190, 92, 60));
    Part(c, {Box(100, 158, 44, 8, 4)}, C3(250, 160, 115), C3(225, 120, 80));
    std::vector<Sh> pet;
    for (int i = 0; i < 5; i++) {
        float a = -1.5708f + i * 1.2566f;
        pet.push_back(Circ(100 + 9 * cosf(a), 44 + 9 * sinf(a), 7));
    }
    Part(c, pet, C3(255, 135, 185), kNoColor, 3.0f);
    Part(c, {Circ(100, 44, 5.5f)}, C3(255, 225, 80), kNoColor, 2.4f);
    Dots(c, 89, 100, 4.4f);
    Dots(c, 111, 100, 4.4f);
    Smile(c, 100, 108, 7, 2.2f, 20, 160);
    Blush(c, 80, 112, 8, 5);
    Blush(c, 120, 112, 8, 5);
}

static void DrawPumpkin(Canvas& c) {
    Shadow(c, 100, 188, 62, 7);
    const RGBA a1 = C3(242, 130, 35), a2 = C3(200, 88, 15);
    Part(c, {Ell(60, 130, 32, 50)}, a1, a2);
    Part(c, {Ell(140, 130, 32, 50)}, a1, a2);
    Part(c, {Ell(100, 130, 34, 54)}, C3(255, 170, 60), C3(240, 125, 25));
    Part(c, {Ell(124, 66, 14, 6, -0.45f)}, C3(125, 210, 90), kNoColor, 2.6f);
    Part(c, {Box(102, 72, 8, 11, 3, 0.25f)}, C3(125, 170, 70), kNoColor, 3.0f);
    Part(c, {Poly({{72, 120}, {94, 120}, {83, 100}})}, C3(255, 238, 120),
         kNoColor, 3.0f);
    Part(c, {Poly({{106, 120}, {128, 120}, {117, 100}})}, C3(255, 238, 120),
         kNoColor, 3.0f);
    Part(c,
         {Poly({{72, 134}, {128, 134}, {123, 150}, {113, 142}, {106, 154},
                {98, 143}, {90, 154}, {84, 142}, {76, 151}})},
         C3(255, 238, 120), kNoColor, 3.0f);
    Blush(c, 60, 130, 9, 5);
    Blush(c, 140, 130, 9, 5);
}

static void DrawBear(Canvas& c) {
    Shadow(c, 100, 188, 52, 7);
    const RGBA b1 = C3(205, 140, 90), b2 = C3(160, 100, 60);
    const RGBA cream = C3(255, 232, 200);
    Part(c, {Circ(62, 54, 19)}, b1, b2);
    Part(c, {Circ(138, 54, 19)}, b1, b2);
    Flat(c, {Circ(62, 56, 10)}, C3(240, 170, 150));
    Flat(c, {Circ(138, 56, 10)}, C3(240, 170, 150));
    Part(c, {Ell(100, 150, 46, 38)}, b1, b2);
    Flat(c, {Ell(100, 156, 28, 24)}, cream);
    Part(c, {Ell(74, 184, 19, 9)}, b1, b2);
    Part(c, {Ell(126, 184, 19, 9)}, b1, b2);
    Flat(c, {Ell(74, 185, 9, 4.5f), Ell(126, 185, 9, 4.5f)}, cream);
    Part(c, {Ell(54, 150, 12, 22, 0.5f)}, b1, b2);
    Part(c, {Ell(146, 150, 12, 22, -0.5f)}, b1, b2);
    Part(c, {Circ(100, 86, 48)}, C3(215, 150, 100), b2);
    Flat(c, {Ell(100, 102, 25, 19)}, cream);
    Flat(c, {Ell(100, 94, 9, 6)}, C3(70, 45, 45));
    Flat(c, {Circ(97, 92, 2)}, WHITE);
    Dots(c, 79, 80, 6);
    Dots(c, 121, 80, 6);
    Flat(c, {Cap(100, 99, 100, 105, 1.4f)}, INK);
    Smile(c, 94, 105, 6, 2.2f, 10, 170);
    Smile(c, 106, 105, 6, 2.2f, 10, 170);
    Blush(c, 64, 100);
    Blush(c, 136, 100);
}

static void DrawMushroom(Canvas& c) {
    Shadow(c, 100, 188, 50, 7);
    Part(c, {Ell(88, 184, 14, 6)}, C3(255, 246, 225));
    Part(c, {Ell(112, 184, 14, 6)}, C3(255, 246, 225));
    Part(c, {Box(100, 140, 27, 42, 13)}, C3(255, 248, 230), C3(235, 212, 180));
    std::vector<Pt> cap;
    for (int i = 0; i <= 24; i++) {
        float a = 3.14159265f + 3.14159265f * i / 24.0f;
        cap.push_back({100 + 68 * cosf(a), 98 + 56 * sinf(a)});
    }
    cap.push_back({158, 105});
    cap.push_back({100, 110});
    cap.push_back({42, 105});
    Part(c, {PolyV(cap)}, C3(250, 95, 100), C3(200, 38, 60));
    Flat(c, {Circ(66, 80, 10), Circ(112, 58, 12), Circ(142, 86, 9),
             Circ(92, 88, 6), Circ(74, 58, 5)},
         C3(255, 255, 255, 0.95f));
    Dots(c, 90, 128, 4.6f);
    Dots(c, 110, 128, 4.6f);
    Smile(c, 100, 134, 6, 2.2f, 20, 160);
    Blush(c, 80, 138, 8, 5);
    Blush(c, 120, 138, 8, 5);
}

static void DrawDonut(Canvas& c) {
    Shadow(c, 100, 188, 58, 7);
    const RGBA d1 = C3(245, 200, 140), d2 = C3(212, 148, 88);
    Part(c, {Ell(82, 186, 13, 6)}, d1, d2);
    Part(c, {Ell(118, 186, 13, 6)}, d1, d2);
    Part(c, {Ring(100, 110, 44, 28)}, d1, d2);
    Part(c, {Ring(100, 106, 44, 22)}, C3(255, 150, 200), C3(235, 90, 155),
         3.0f);
    struct Spr {
        float ang, dir;
        int col;
    };
    const Spr sp[] = {{200, 20, 0}, {225, 80, 1}, {250, 10, 2}, {275, 70, 3},
                      {300, 30, 0}, {325, 90, 1}, {345, 40, 2}, {172, 60, 3}};
    const RGBA cols[] = {C3(255, 255, 255), C3(255, 232, 80), C3(90, 220, 255),
                         C3(150, 255, 150)};
    for (const Spr& s : sp) {
        float a = s.ang * 3.14159265f / 180.0f;
        float d = s.dir * 3.14159265f / 180.0f;
        float x = 100 + 44 * cosf(a), y = 106 + 44 * sinf(a);
        Flat(c, {Cap(x - 4 * cosf(d), y - 4 * sinf(d), x + 4 * cosf(d),
                     y + 4 * sinf(d), 2.3f)},
             cols[s.col]);
    }
    Dots(c, 84, 148, 5);
    Dots(c, 116, 148, 5);
    Smile(c, 100, 152, 7, 2.4f, 20, 160);
    Blush(c, 68, 156, 8, 5);
    Blush(c, 132, 156, 8, 5);
}

static void DrawPizza(Canvas& c) {
    Shadow(c, 100, 188, 38, 6);
    const RGBA ch1 = C3(255, 215, 85), ch2 = C3(255, 168, 40);
    const RGBA skin = C3(255, 228, 185);
    Part(c, {Cap(92, 174, 88, 186, 5.5f), Ell(84, 189, 9, 4.5f)}, skin);
    Part(c, {Cap(108, 174, 112, 186, 5.5f), Ell(116, 189, 9, 4.5f)}, skin);
    Part(c, {Cap(60, 102, 40, 120, 5.5f), Circ(38, 122, 6.5f)}, skin);
    Part(c, {Cap(140, 102, 160, 120, 5.5f), Circ(162, 122, 6.5f)}, skin);
    Part(c, {Poly({{30, 46}, {170, 46}, {100, 182}})}, ch1, ch2);
    Part(c, {Box(100, 44, 72, 11, 10)}, C3(238, 175, 95), C3(200, 125, 55));
    Part(c, {Circ(66, 74, 12)}, C3(232, 70, 70), kNoColor, 3.0f);
    Part(c, {Circ(136, 78, 12)}, C3(232, 70, 70), kNoColor, 3.0f);
    Part(c, {Circ(100, 140, 11)}, C3(232, 70, 70), kNoColor, 3.0f);
    Flat(c, {Circ(62, 70, 3.4f), Circ(132, 74, 3.4f), Circ(97, 136, 3)},
         C3(255, 140, 120));
    Dots(c, 84, 98, 5);
    Dots(c, 116, 98, 5);
    Smile(c, 100, 106, 7, 2.4f, 20, 160);
    Blush(c, 72, 108, 8, 5);
    Blush(c, 128, 108, 8, 5);
}

typedef void (*CharFn)(Canvas&);
struct CharDef {
    const wchar_t* id;
    CharFn fn;
};
static const CharDef kChars[] = {
    {L"ghost", DrawGhost},       {L"cat", DrawCat},
    {L"alien", DrawAlien},       {L"robot", DrawRobot},
    {L"penguin", DrawPenguin},   {L"duck", DrawDuck},
    {L"slime", DrawSlime},       {L"bomb", DrawBomb},
    {L"cactus", DrawCactus},     {L"pumpkin", DrawPumpkin},
    {L"bear", DrawBear},         {L"mushroom", DrawMushroom},
    {L"donut", DrawDonut},       {L"pizza", DrawPizza},
};
static const int kCharCount = (int)(sizeof(kChars) / sizeof(kChars[0]));

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum MotionStyle {
    MS_PEEK = 0,
    MS_SPRINGY,
    MS_SLIDE,
    MS_HOP,
    MS_SPIN,
    MS_POP,
    MS_COUNT,
};
static const wchar_t* kMotionIds[MS_COUNT] = {L"peek", L"springy", L"slide",
                                              L"hop",  L"spin",    L"pop"};

enum BubbleMode { BUB_FUNNY = 0, BUB_TIME, BUB_CUSTOM, BUB_NONE };

struct Cfg {
    int character = -1;  // -1 = random
    int motion = -1;     // -1 = random
    bool rightCorner = true;
    bool cursorMonitor = false;
    bool onTheHour = true;
    int interval = 0;  // minutes, 0 = off
    std::vector<int> times;  // minutes since midnight
    int quietFrom = 0, quietTo = 0;
    int stay = 6;
    int sizePct = 100;
    int opacity = 100;
    int bubble = BUB_FUNNY;
    std::wstring customMsg;
    int lang = 0;  // 0 = auto, 1 = Turkish, 2 = English
    bool clickDismiss = true;
    bool sound = false;
    bool skipFull = false;
    bool previewOnSave = true;
    bool previewNow = false;
};
static Cfg g_cfg;

static std::wstring GetStr(PCWSTR name) {
    std::wstring r;
    PCWSTR s = Wh_GetStringSetting(name);
    if (s) {
        r = s;
        Wh_FreeStringSetting(s);
    }
    return r;
}

// Reads "HH:MM" items separated by commas, spaces or semicolons.
static std::vector<int> ParseTimes(const std::wstring& str) {
    std::vector<int> v;
    const wchar_t* p = str.c_str();
    while (*p) {
        if (*p >= L'0' && *p <= L'9') {
            int h = 0;
            while (*p >= L'0' && *p <= L'9') {
                if (h < 1000) h = h * 10 + (*p - L'0');
                p++;
            }
            if ((*p == L':' || *p == L'.') && p[1] >= L'0' && p[1] <= L'9') {
                p++;
                int m = 0;
                while (*p >= L'0' && *p <= L'9') {
                    if (m < 1000) m = m * 10 + (*p - L'0');
                    p++;
                }
                if (h < 24 && m < 60 && v.size() < 128) v.push_back(h * 60 + m);
            }
        } else {
            p++;
        }
    }
    return v;
}

static void LoadSettings() {
    Cfg c;

    std::wstring s = GetStr(L"character");
    for (int i = 0; i < kCharCount; i++) {
        if (s == kChars[i].id) c.character = i;
    }

    s = GetStr(L"motion");
    for (int i = 0; i < MS_COUNT; i++) {
        if (s == kMotionIds[i]) c.motion = i;
    }

    c.rightCorner = GetStr(L"corner") != L"left";
    c.cursorMonitor = GetStr(L"monitor") == L"cursor";

    c.onTheHour = Wh_GetIntSetting(L"onTheHour") != 0;
    c.interval = std::clamp(Wh_GetIntSetting(L"intervalMinutes"), 0, 720);
    c.times = ParseTimes(GetStr(L"customTimes"));
    c.quietFrom = std::clamp(Wh_GetIntSetting(L"quietFrom"), 0, 23);
    c.quietTo = std::clamp(Wh_GetIntSetting(L"quietTo"), 0, 23);

    c.stay = std::clamp(Wh_GetIntSetting(L"stayTime"), 2, 60);
    c.sizePct = std::clamp(Wh_GetIntSetting(L"sizePercent"), 40, 300);
    c.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 30, 100);

    s = GetStr(L"bubble");
    c.bubble = s == L"time"     ? BUB_TIME
               : s == L"custom" ? BUB_CUSTOM
               : s == L"none"   ? BUB_NONE
                                : BUB_FUNNY;
    c.customMsg = GetStr(L"customMessage");

    s = GetStr(L"messageLanguage");
    c.lang = s == L"tr" ? 1 : (s == L"en" ? 2 : 0);

    c.clickDismiss = Wh_GetIntSetting(L"clickToDismiss") != 0;
    c.sound = Wh_GetIntSetting(L"playSound") != 0;
    c.skipFull = Wh_GetIntSetting(L"skipFullscreen") != 0;
    c.previewOnSave = Wh_GetIntSetting(L"previewOnSave") != 0;
    c.previewNow = Wh_GetIntSetting(L"previewNow") != 0;

    g_cfg = c;
}

// ---------------------------------------------------------------------------
// Messages
// ---------------------------------------------------------------------------

static const wchar_t* kJokesEn[] = {
    L"Drink some water!",
    L"Blink! Your eyes are drying out.",
    L"Stand up and stretch!",
    L"Boo! Did I scare you?",
    L"I'm not a virus. Probably.",
    L"Your posture called. It's upset.",
    L"Time flies, and so do I.",
    L"Snack time? I think so.",
    L"Don't forget to save your work!",
    L"Look at something far away for 20 seconds.",
    L"Psst... the boss isn't looking.",
    L"I was just passing by.",
    L"Another hour gone. One more video?",
    L"Take a deep breath. Better?",
    L"Your shoulders are touching your ears. Relax!",
    L"Long time at the screen. Tea break!",
    L"You're doing great today. Seriously.",
    L"Fix your chair, your back will thank you.",
};

static const wchar_t* kJokesTr[] = {
    L"Su içmeyi unutma!",
    L"Göz kırp! Gözlerin kuruyor.",
    L"Kalk ve biraz esne!",
    L"Hü! Korktun mu?",
    L"Ben virüs değilim. Sanırım.",
    L"Duruşun seni arıyor, biraz kızgın.",
    L"Zaman uçuyor, ben de.",
    L"Atıştırma vakti mi? Bence evet.",
    L"Çalışmanı kaydetmeyi unutma!",
    L"20 saniye uzağa bak. Pencere de olur.",
    L"Pst... Patron bakmıyor.",
    L"Ben sadece yoldan geçiyordum.",
    L"Bir saat daha geçti. Bir video daha?",
    L"Derin bir nefes al. Daha iyi mi?",
    L"Omuzların kulaklarına yapışmış, indir!",
    L"Ekran başında çok kaldın, çay molası!",
    L"Bugün harikasın. Ciddi söylüyorum.",
    L"Sandalyeni düzelt, sırtın sana teşekkür edecek.",
};

static bool UseTurkish() {
    if (g_cfg.lang == 1) return true;
    if (g_cfg.lang == 2) return false;
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_TURKISH;
}

static std::wstring ReplaceAll(std::wstring s, const std::wstring& from,
                               const std::wstring& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::wstring::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

// ---------------------------------------------------------------------------
// Sprites and drawing onto the overlay bitmap
// ---------------------------------------------------------------------------

struct Sprite {
    int w = 0, h = 0;
    std::vector<uint32_t> px;  // premultiplied 0xAARRGGBB
};

static float EaseOutBack(float t) {
    const float c1 = 1.70158f, c3 = c1 + 1.0f;
    float u = t - 1.0f;
    return 1.0f + c3 * u * u * u + c1 * u * u;
}

static float EaseInBack(float t) {
    const float c1 = 1.70158f, c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

static float EaseOutCubic(float t) {
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

static float EaseOutElastic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return powf(2.0f, -10.0f * t) *
               sinf((t * 10.0f - 0.75f) * (2.0f * 3.14159265f / 3.0f)) +
           1.0f;
}

// Draws the sprite with a transform: dest = P + R(th) * (M * (src - pv)).
static void DrawSprite(uint32_t* dst, int dw, int dh, const Sprite& s, float Px,
                       float Py, float pvx, float pvy, float mx, float my,
                       float th, float alpha) {
    if (s.w <= 0 || fabsf(mx) < 1e-3f || fabsf(my) < 1e-3f || alpha <= 0.0f)
        return;
    float c = cosf(th), sn = sinf(th);
    float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
    const float cx[4] = {0, (float)s.w, (float)s.w, 0};
    const float cy[4] = {0, 0, (float)s.h, (float)s.h};
    for (int i = 0; i < 4; i++) {
        float ax = (cx[i] - pvx) * mx, ay = (cy[i] - pvy) * my;
        float x = Px + c * ax - sn * ay, y = Py + sn * ax + c * ay;
        minx = std::min(minx, x), maxx = std::max(maxx, x);
        miny = std::min(miny, y), maxy = std::max(maxy, y);
    }
    int x0 = std::max(0, (int)floorf(minx)), x1 = std::min(dw, (int)ceilf(maxx));
    int y0 = std::max(0, (int)floorf(miny)), y1 = std::min(dh, (int)ceilf(maxy));

    for (int y = y0; y < y1; y++) {
        float vy = y + 0.5f - Py;
        for (int x = x0; x < x1; x++) {
            float vx = x + 0.5f - Px;
            float ux = c * vx + sn * vy, uy = -sn * vx + c * vy;
            float sx = pvx + ux / mx, sy = pvy + uy / my;
            if (sx < 0 || sy < 0 || sx >= s.w || sy >= s.h) continue;
            float fx = sx - 0.5f, fy = sy - 0.5f;
            int ix = (int)floorf(fx), iy = (int)floorf(fy);
            float wx = fx - ix, wy = fy - iy;
            int ax0 = std::clamp(ix, 0, s.w - 1), ax1 = std::clamp(ix + 1, 0, s.w - 1);
            int ay0 = std::clamp(iy, 0, s.h - 1), ay1 = std::clamp(iy + 1, 0, s.h - 1);
            uint32_t p00 = s.px[(size_t)ay0 * s.w + ax0];
            uint32_t p10 = s.px[(size_t)ay0 * s.w + ax1];
            uint32_t p01 = s.px[(size_t)ay1 * s.w + ax0];
            uint32_t p11 = s.px[(size_t)ay1 * s.w + ax1];
            float w00 = (1 - wx) * (1 - wy), w10 = wx * (1 - wy);
            float w01 = (1 - wx) * wy, w11 = wx * wy;
            float sa = 0, sr = 0, sg = 0, sb = 0;
            sa = ((p00 >> 24) & 255) * w00 + ((p10 >> 24) & 255) * w10 +
                 ((p01 >> 24) & 255) * w01 + ((p11 >> 24) & 255) * w11;
            if (sa < 0.5f) continue;
            sr = ((p00 >> 16) & 255) * w00 + ((p10 >> 16) & 255) * w10 +
                 ((p01 >> 16) & 255) * w01 + ((p11 >> 16) & 255) * w11;
            sg = ((p00 >> 8) & 255) * w00 + ((p10 >> 8) & 255) * w10 +
                 ((p01 >> 8) & 255) * w01 + ((p11 >> 8) & 255) * w11;
            sb = (p00 & 255) * w00 + (p10 & 255) * w10 + (p01 & 255) * w01 +
                 (p11 & 255) * w11;
            sa *= alpha, sr *= alpha, sg *= alpha, sb *= alpha;
            uint32_t& d = dst[(size_t)y * dw + x];
            float inv = 1.0f - sa / 255.0f;
            uint32_t oa = Cl8(sa + ((d >> 24) & 255) * inv);
            uint32_t orr = Cl8(sr + ((d >> 16) & 255) * inv);
            uint32_t og = Cl8(sg + ((d >> 8) & 255) * inv);
            uint32_t ob = Cl8(sb + (d & 255) * inv);
            d = (oa << 24) | (orr << 16) | (og << 8) | ob;
        }
    }
}

// Renders the speech bubble. tipX/tipY: tail tip inside the sprite.
static bool BuildBubble(const std::wstring& title, const std::wstring& body,
                        int S, float k, bool right, Sprite& out, float& tipX,
                        float& tipY) {
    if (title.empty() && body.empty()) return false;

    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) return false;
    int fontPx = std::clamp((int)(15.0f * k + 0.5f), 12, 60);
    HFONT fBody = CreateFontW(-fontPx, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT fTitle = CreateFontW(-(int)(fontPx * 1.45f), 0, 0, 0, FW_BOLD, FALSE,
                               FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                               DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    int maxW = std::max(120, (int)(S * 1.05f));
    const UINT calcFlags = DT_CALCRECT | DT_WORDBREAK | DT_CENTER | DT_NOPREFIX;
    RECT rt = {0, 0, maxW, 0}, rb = {0, 0, maxW, 0};
    HGDIOBJ oldFont = SelectObject(hdc, fTitle);
    if (!title.empty()) DrawTextW(hdc, title.c_str(), -1, &rt, calcFlags);
    SelectObject(hdc, fBody);
    if (!body.empty()) DrawTextW(hdc, body.c_str(), -1, &rb, calcFlags);

    int gap = (title.empty() || body.empty()) ? 0 : std::max(2, (int)(2 * k));
    int tw = std::max(1, (int)std::max(rt.right, rb.right));
    int th = std::max(1, (int)(rt.bottom + gap + rb.bottom));

    // Text mask: white text on a black bitmap, the green channel is the alpha.
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = tw;
    bi.bmiHeader.biHeight = -th;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    uint32_t* tbits = nullptr;
    HBITMAP tdib = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)&tbits,
                                    nullptr, 0);
    bool ok = tdib && tbits;
    std::vector<uint32_t> mask;
    if (ok) {
        HGDIOBJ oldBmp = SelectObject(hdc, tdib);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        const UINT drawFlags = DT_WORDBREAK | DT_CENTER | DT_NOPREFIX;
        if (!title.empty()) {
            RECT r = {0, 0, tw, rt.bottom};
            SelectObject(hdc, fTitle);
            DrawTextW(hdc, title.c_str(), -1, &r, drawFlags);
        }
        if (!body.empty()) {
            RECT r = {0, rt.bottom + gap, tw, th};
            SelectObject(hdc, fBody);
            DrawTextW(hdc, body.c_str(), -1, &r, drawFlags);
        }
        GdiFlush();
        mask.assign(tbits, tbits + (size_t)tw * th);
        SelectObject(hdc, oldBmp);
    }
    SelectObject(hdc, oldFont);
    if (tdib) DeleteObject(tdib);
    DeleteObject(fBody);
    DeleteObject(fTitle);
    DeleteDC(hdc);
    if (!ok) return false;

    int pad = std::max(8, (int)(11 * k));
    int m = std::max(4, (int)(4 * k));
    int bw = tw + 2 * pad, bh = th + 2 * pad;
    int tailH = std::max(10, (int)(16 * k));
    int sw = bw + 2 * m, sh = bh + tailH + 2 * m;

    float f = bw > S ? 1.0f - 0.5f * S / bw : 0.5f;
    if (!right) f = 1.0f - f;
    float base = m + f * bw;
    float ow = std::max(2.0f, 2.6f * k);
    tipX = base + (right ? 5.0f : -5.0f) * k;
    tipY = (float)(m + bh + tailH - 1);

    Canvas cv(sw, sh, 1.0f);
    float yb = (float)(m + bh);
    cv.Fill({Box(m + bw * 0.5f, m + bh * 0.5f, bw * 0.5f, bh * 0.5f,
                 std::min(bh * 0.5f, 14.0f * k)),
             Poly({{base - 9 * k, yb - ow}, {base + 9 * k, yb - ow},
                   {tipX, tipY}})},
            C3(255, 255, 255), INK, ow, 0.0f, C3(238, 243, 255));
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            uint32_t v = (mask[(size_t)y * tw + x] >> 8) & 255;
            if (!v) continue;
            cv.Blend(m + pad + x, m + pad + y, INK.r, INK.g, INK.b, v / 255.0f);
        }
    }
    out.w = sw;
    out.h = sh;
    out.px = std::move(cv.px);
    return true;
}

// ---------------------------------------------------------------------------
// The overlay window and the animation
// ---------------------------------------------------------------------------

static const float kEnter = 0.8f;   // seconds
static const float kLeave = 0.45f;  // seconds

struct Show {
    bool active = false;
    bool right = true;
    ULONGLONG t0 = 0;
    ULONGLONG lastTop = 0;
    int style = 0;
    float leaveStart = 0;
    bool clicked = false;
    float clickT = 0;
    bool clickable = true;
    int S = 0;
    float k = 1;
    int CW = 0, CH = 0, charX = 0, charY = 0;
    int winX = 0, winY = 0;
    int opacity = 100;
    Sprite chr, bub;
    bool hasBubble = false;
    float tipX = 0, tipY = 0, bubX = 0, bubY = 0;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    uint32_t* bits = nullptr;
};
static Show g_s;

static HWND g_hwnd = nullptr;
static const UINT WM_APP_SETTINGS = WM_APP + 1;
static const UINT_PTR kTimerSchedule = 1;
static const UINT_PTR kTimerFrame = 2;
static ULONGLONG g_lastMinute = 0;
static int g_lastChar = -1;

static void EndShow() {
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerFrame);
        ShowWindow(g_hwnd, SW_HIDE);
    }
    if (g_s.memDC) {
        if (g_s.oldBmp) SelectObject(g_s.memDC, g_s.oldBmp);
        DeleteDC(g_s.memDC);
    }
    if (g_s.dib) DeleteObject(g_s.dib);
    Show fresh;
    g_s = fresh;
}

static void RenderFrame() {
    Show& s = g_s;
    if (!s.active || !g_hwnd) return;

    float e = (float)(GetTickCount64() - s.t0) / 1000.0f;
    if (e >= s.leaveStart + kLeave) {
        EndShow();
        return;
    }

    // How far the character is out: 0 = hidden, 1 = fully visible.
    float vis;
    if (e < s.leaveStart) {
        float u = Clamp01(e / kEnter);
        if (s.style == MS_SPRINGY || s.style == MS_POP)
            vis = EaseOutElastic(u);
        else if (s.style == MS_HOP)
            vis = EaseOutCubic(u);
        else
            vis = EaseOutBack(u);
    } else {
        vis = 1.0f - EaseInBack(Clamp01((e - s.leaveStart) / kLeave));
    }

    const float PI = 3.14159265f;
    float S = (float)s.S;
    float dirX = s.right ? 1.0f : -1.0f;
    float inv = 1.0f - vis;
    bool spin = s.style == MS_SPIN;
    float pvx = S * 0.5f, pvy = spin ? S * 0.5f : S * 0.94f;
    float dx = 0, dy = 0, th = 0, sc = 1, alphaMul = 1;

    switch (s.style) {
        case MS_PEEK:
            dy = inv * S * 1.08f;
            th = -dirX * inv * 0.12f;
            break;
        case MS_SPRINGY:
            dy = inv * S * 1.08f;
            break;
        case MS_SLIDE:
            dx = dirX * inv * S * 1.15f;
            break;
        case MS_HOP: {
            float amp = std::max(0.0f, inv);
            float sw = sinf(vis * PI * 4.0f);
            dx = dirX * inv * S * 1.5f;
            dy = -fabsf(sw) * S * 0.17f * amp;
            th = sw * 0.14f * amp * dirX;
            break;
        }
        case MS_SPIN:
            dx = dirX * inv * S * 0.95f;
            dy = inv * S * 0.95f;
            th = -dirX * inv * 6.5f;
            sc = std::clamp(vis, 0.15f, 1.15f);
            alphaMul = Clamp01(vis * 3.0f);
            break;
        default:  // MS_POP
            sc = std::max(vis, 0.0f);
            alphaMul = Clamp01(vis * 4.0f);
            break;
    }

    // Idle wiggle while the character is waiting.
    float idleW = Clamp01((e - kEnter * 0.8f) / 0.3f) *
                  (1.0f - Clamp01((e - s.leaveStart) / 0.15f));
    float squash = 0;
    if (idleW > 0) {
        th += idleW * 0.05f * sinf(e * 8.0f);
        dy -= idleW * fabsf(sinf(e * 4.5f)) * S * 0.025f;
        squash = idleW * 0.02f * sinf(e * 9.0f);
    }
    if (s.clicked) {
        float ct = e - s.clickT;
        if (ct >= 0 && ct < 0.6f)
            th += 0.2f * sinf(ct * 60.0f) * (1.0f - ct / 0.6f);
    }

    memset(s.bits, 0, (size_t)s.CW * s.CH * 4);
    float Px = s.charX + pvx + dx, Py = s.charY + pvy + dy;
    DrawSprite(s.bits, s.CW, s.CH, s.chr, Px, Py, pvx, pvy,
               dirX * sc * (1.0f - squash), sc * (1.0f + squash), th, alphaMul);

    if (s.hasBubble) {
        float bt = Clamp01((e - kEnter * 0.7f) / 0.35f);
        float bs = EaseOutBack(bt);
        if (e >= s.leaveStart) bs *= 1.0f - Clamp01((e - s.leaveStart) / 0.2f);
        bs = std::max(bs, 0.0f);
        float bob = sinf(e * 3.0f) * 1.5f * s.k;
        DrawSprite(s.bits, s.CW, s.CH, s.bub, s.bubX, s.bubY + bob, s.tipX,
                   s.tipY, bs, bs, 0.0f, Clamp01(bs * 2.0f));
    }

    // Stay above the taskbar and other topmost windows.
    ULONGLONG now = GetTickCount64();
    if (now - s.lastTop > 400) {
        s.lastTop = now;
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    HDC scr = GetDC(nullptr);
    POINT ptSrc = {0, 0}, ptDst = {s.winX, s.winY};
    SIZE sz = {s.CW, s.CH};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)(s.opacity * 255 / 100),
                        AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, scr, &ptDst, &sz, s.memDC, &ptSrc, 0, &bf,
                        ULW_ALPHA);
    ReleaseDC(nullptr, scr);
}

static UINT GetMonitorDpi(HMONITOR mon) {
    typedef HRESULT(WINAPI * GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE m = LoadLibraryW(L"shcore.dll");
        if (m) fn = (GetDpiForMonitor_t)GetProcAddress(m, "GetDpiForMonitor");
    }
    UINT x = 96, y = 96;
    if (fn && SUCCEEDED(fn(mon, 0, &x, &y)) && x) return x;
    return 96;
}

static void StartShow(bool preview) {
    EndShow();
    if (!g_hwnd) return;
    const Cfg& cf = g_cfg;

    // Monitor and size.
    POINT pt = {0, 0};
    HMONITOR mon;
    if (cf.cursorMonitor && GetCursorPos(&pt)) {
        mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    } else {
        mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        Wh_Log(L"GetMonitorInfo failed: %lu", GetLastError());
        return;
    }
    UINT dpi = GetMonitorDpi(mon);
    int S = (int)(200.0f * cf.sizePct / 100.0f * dpi / 96.0f + 0.5f);
    S = std::clamp(S, 64, 1400);
    float k = S / 200.0f;

    // Pick the character and the movement.
    int chr = cf.character;
    if (chr < 0) {
        chr = rand() % kCharCount;
        if (chr == g_lastChar) chr = (chr + 1 + rand() % (kCharCount - 1)) % kCharCount;
    }
    g_lastChar = chr;
    int style = cf.motion >= 0 ? cf.motion : rand() % MS_COUNT;
    Wh_Log(L"Showing %s (movement %d, size %d)%s", kChars[chr].id, style, S,
           preview ? L" [preview]" : L"");

    // Texts.
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR tbuf[16];
    swprintf_s(tbuf, L"%02d:%02d", st.wHour, st.wMinute);
    std::wstring title, body;
    bool tr = UseTurkish();
    switch (cf.bubble) {
        case BUB_FUNNY: {
            title = tbuf;
            if (tr)
                body = kJokesTr[rand() % (int)ARRAYSIZE(kJokesTr)];
            else
                body = kJokesEn[rand() % (int)ARRAYSIZE(kJokesEn)];
            break;
        }
        case BUB_TIME:
            title = tbuf;
            break;
        case BUB_CUSTOM:
            body = ReplaceAll(cf.customMsg, L"{time}", tbuf);
            break;
        default:
            break;
    }

    Show& s = g_s;
    s.right = cf.rightCorner;
    s.style = style;
    s.S = S;
    s.k = k;
    s.opacity = cf.opacity;
    s.clickable = cf.clickDismiss;

    // Character sprite.
    {
        Canvas cv(S, S, k);
        kChars[chr].fn(cv);
        s.chr.w = S;
        s.chr.h = S;
        s.chr.px = std::move(cv.px);
    }
    int headTop = S / 6;
    for (int y = 0; y < S; y++) {
        bool found = false;
        for (int x = 0; x < S; x++) {
            if ((s.chr.px[(size_t)y * S + x] >> 24) > 130) {
                found = true;
                break;
            }
        }
        if (found) {
            headTop = y;
            break;
        }
    }

    // Speech bubble and the layout of the window.
    s.hasBubble = BuildBubble(title, body, S, k, s.right, s.bub, s.tipX, s.tipY);
    s.charX = 0;
    s.charY = 0;
    s.CW = S;
    int charH = (int)(S * 0.965f);
    s.CH = charH;
    if (s.hasBubble) {
        float tipRel = std::max(0.0f, headTop - 2.0f * k);
        float leftOv = std::max(0.0f, s.tipX - S * 0.5f);
        float rightOv = std::max(0.0f, (s.bub.w - s.tipX) - S * 0.5f);
        s.charX = (int)ceilf(leftOv);
        s.charY = (int)ceilf(std::max(0.0f, s.tipY - tipRel));
        s.CW = s.charX + S + (int)ceilf(rightOv);
        s.CH = s.charY + charH;
        s.bubX = s.charX + S * 0.5f;
        s.bubY = s.charY + tipRel;
    }

    int margin = (int)(8 * k);
    s.winX = s.right ? mi.rcWork.right - s.CW - margin : mi.rcWork.left + margin;
    s.winY = mi.rcWork.bottom - s.CH;

    // Bitmap that is shown through UpdateLayeredWindow.
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = s.CW;
    bi.bmiHeader.biHeight = -s.CH;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC scr = GetDC(nullptr);
    s.memDC = CreateCompatibleDC(scr);
    s.dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void**)&s.bits, nullptr, 0);
    ReleaseDC(nullptr, scr);
    if (!s.memDC || !s.dib || !s.bits) {
        Wh_Log(L"Failed to create the bitmap: %lu", GetLastError());
        EndShow();
        return;
    }
    s.oldBmp = SelectObject(s.memDC, s.dib);

    // Click-through unless the character can be clicked away.
    LONG_PTR ex = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
    if (s.clickable)
        ex &= ~(LONG_PTR)WS_EX_TRANSPARENT;
    else
        ex |= WS_EX_TRANSPARENT;
    SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, ex);

    s.leaveStart = kEnter + (float)cf.stay;
    s.t0 = GetTickCount64();
    s.active = true;
    RenderFrame();
    if (!s.active) return;
    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetTimer(g_hwnd, kTimerFrame, 15, nullptr);
    if (cf.sound) MessageBeep(MB_OK);
}

static void OnClicked() {
    Show& s = g_s;
    if (!s.active || s.clicked || !s.clickable) return;
    float e = (float)(GetTickCount64() - s.t0) / 1000.0f;
    s.clicked = true;
    s.clickT = e;
    s.leaveStart = std::max(kEnter, std::min(s.leaveStart, e + 0.55f));
}

// ---------------------------------------------------------------------------
// Schedule
// ---------------------------------------------------------------------------

static bool IsQuietHour(int h) {
    int a = g_cfg.quietFrom, b = g_cfg.quietTo;
    if (a == b) return false;
    return a < b ? (h >= a && h < b) : (h >= a || h < b);
}

static bool IsBusyFullscreen() {
    QUERY_USER_NOTIFICATION_STATE qs;
    if (FAILED(SHQueryUserNotificationState(&qs))) return false;
    return qs == QUNS_BUSY || qs == QUNS_RUNNING_D3D_FULL_SCREEN ||
           qs == QUNS_PRESENTATION_MODE;
}

static ULONGLONG CurrentMinuteId(SYSTEMTIME* out) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (out) *out = st;
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return u.QuadPart / 600000000ULL;
}

static void CheckSchedule() {
    SYSTEMTIME st;
    ULONGLONG id = CurrentMinuteId(&st);
    if (id == g_lastMinute) return;
    g_lastMinute = id;

    int mod = st.wHour * 60 + st.wMinute;
    bool fire = false;
    if (g_cfg.onTheHour && st.wMinute == 0) fire = true;
    if (g_cfg.interval > 0 && mod % g_cfg.interval == 0) fire = true;
    for (int t : g_cfg.times) {
        if (t == mod) fire = true;
    }
    if (!fire) return;

    if (IsQuietHour(st.wHour)) {
        Wh_Log(L"Quiet hours, skipping %02d:%02d", st.wHour, st.wMinute);
        return;
    }
    if (g_cfg.skipFull && IsBusyFullscreen()) {
        Wh_Log(L"A fullscreen app is active, skipping");
        return;
    }
    StartShow(false);
}

// ---------------------------------------------------------------------------
// Worker thread and tool mod entry points
// ---------------------------------------------------------------------------

static HANDLE g_stopEvent = nullptr;
static HANDLE g_readyEvent = nullptr;
static HANDLE g_thread = nullptr;

static LRESULT CALLBACK OverlayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_APP_SETTINGS: {
            bool prevNow = g_cfg.previewNow;
            LoadSettings();
            if (g_cfg.previewOnSave || g_cfg.previewNow != prevNow) {
                StartShow(true);
            }
            return 0;
        }
        case WM_TIMER:
            if (w == kTimerFrame) {
                RenderFrame();
            } else if (w == kTimerSchedule) {
                CheckSchedule();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            OnClicked();
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static DWORD WINAPI Worker(LPVOID) {
    // Use real pixels on every monitor.
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContext_t)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        auto fn = (SetThreadDpiAwarenessContext_t)GetProcAddress(
            user32, "SetThreadDpiAwarenessContext");
        if (fn) fn((HANDLE)-4);  // PER_MONITOR_AWARE_V2
    }
    srand((unsigned)GetTickCount());

    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wc.lpszClassName = L"WindhawkHourlyPeekBuddyWindow";
    RegisterClassW(&wc);
    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"Hourly Peek Buddy", WS_POPUP, 0, 0, 1, 1, nullptr,
        nullptr, wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"Failed to create the window: %lu", GetLastError());
    } else {
        g_lastMinute = CurrentMinuteId(nullptr);
        SetTimer(g_hwnd, kTimerSchedule, 500, nullptr);
    }
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

    EndShow();
    if (g_hwnd) DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Hourly Peek Buddy starting");
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Failed to create the worker thread: %lu", GetLastError());
        return FALSE;
    }
    WaitForSingleObject(g_readyEvent, 10000);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    // Applied on the worker thread, which owns the window and the animation.
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"Hourly Peek Buddy stopping");
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
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
