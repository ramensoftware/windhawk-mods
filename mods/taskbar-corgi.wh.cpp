// ==WindhawkMod==
// @id              taskbar-corgi
// @name            Taskbar Corgi
// @description     A little pixel corgi lives on top of your taskbar: it walks, naps, does zoomies, dances to music, plays fetch and eats from its bowl
// @version         1.1
// @author          Dimitris Katsimpris
// @github          https://github.com/dimitriskatsimpris
// @include         explorer.exe
// @compilerOptions -lgdi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Corgi

A small pixel-art corgi that lives on top of your taskbar.

- **Walks** back and forth, **sits** for a while and takes **naps** (with
  little floating z's).
- **Zoomies**: now and then it sprints back and forth at full speed, then
  flops down panting.
- **Petting**: hover the mouse over it to get hearts and a butt wiggle. Keep
  petting for a few seconds and it rolls over for a **belly rub**. Petting a
  sleeping corgi wakes it up.
- **Dances to music**: when audio is playing it hops along to the beat.
- **Day and night**: lively in the morning, sleepy in the late evening, and
  always asleep between 2 and 6 am.
- **Seasonal hats**: a pumpkin at Halloween, a Santa hat in December and a
  party hat on your birthday.
- **Fetch**: now and then a ball rolls onto the taskbar and the corgi brings
  it back.
- **Food bowl**: a bowl near the left end of the taskbar is filled at 8:00,
  13:00 and 19:00. Even a sleeping corgi wakes up for food.
- **Friends**: up to 3 corgis with different coats. They join each other's
  zoomies and fetch runs and nap in a pile.

Coats: red and white, tricolour, sable and fluffy cream.

The corgis are click-through, so they never block anything, and they hide
while a fullscreen app or game is in front. Optionally they chase the mouse
cursor (off by default).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 3
  $name: Size
  $description: Pixel size of the corgis (1-8), multiplied by your display scaling
- corgis: 1
  $name: Number of corgis
  $description: 1 to 3. Extra corgis get different coats and play together
- coat: red
  $name: Coat
  $options:
  - red: Red and white
  - tricolor: Tricolour
  - sable: Sable
  - cream: Fluffy cream
- danceToMusic: true
  $name: Dance to music
  $description: Hop along to the beat when audio is playing
- dayNight: true
  $name: Day and night rhythm
  $description: Lively in the morning, sleepy late in the evening, always asleep between 2 and 6 am
- hats: true
  $name: Seasonal hats
  $description: Pumpkin at Halloween, Santa hat in December, party hat on your birthday
- birthday: ""
  $name: Your birthday
  $description: As MM-DD, for example 07-14. Leave empty for no party hat
- fetch: true
  $name: Play fetch
  $description: Now and then a ball rolls onto the taskbar
- foodBowl: true
  $name: Food bowl
  $description: A bowl near the left end of the taskbar, filled at 8:00, 13:00 and 19:00
- chaseMouse: false
  $name: Chase the mouse
  $description: Run toward the cursor when it comes close to the taskbar
- hideInFullscreen: true
  $name: Hide in fullscreen apps
- speed: 100
  $name: Walking speed (%)
  $description: 25 to 300
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <mmdeviceapi.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <initializer_list>

namespace {

// Hat anchor: (hatX, hatY) is the centre column and bottom row of a hat in
// sprite coordinates. hatY < 0 means the frame can't wear a hat.
struct Sprite {
    int w;
    int h;
    const char* const* rows;
    int hatX;
    int hatY;
};

const char* const kWalk1Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWKKKKKKKKKKWWKWK....",
    "..KWK.........KWKKWK....",
    "..KWK.........KWK.KWK...",
    "..KK..........KK...KK...",
};
const Sprite kWalk1 = {24, 16, kWalk1Rows, 19, 2};

const char* const kWalk2Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWKKKKKKKKKKKWWKK....",
    "...KWK.........KWWK.....",
    "...KWK.........KWWK.....",
    "...KK..........KKK......",
};
const Sprite kWalk2 = {24, 16, kWalk2Rows, 19, 2};

const char* const kRun1Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWKKKKKKKKKKWWWWK....",
    ".KWWK..........KWWWWK...",
    "KWWK.............KWWWK..",
    "KK.................KKK..",
};
const Sprite kRun1 = {24, 16, kRun1Rows, 19, 2};

const char* const kRun2Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWWKKKKKKKKWWWWK.....",
    "...KWWK.....KWWWK.......",
    "....KWWK...KWWK.........",
    ".....KK....KK...........",
};
const Sprite kRun2 = {24, 16, kRun2Rows, 19, 2};

const char* const kSitRows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    ".........KKKKKKOOOWWWWK.",
    ".......KKOOOOOOOOOWWKK..",
    "......KDOOOOOOOOOWWWK...",
    ".....KDOOOOOOOOOWWWWK...",
    "....KDOOOOOOOOOOWWWWK...",
    "....KDOOOOOOOOOWWWWK....",
    "...KWDDOOOOOOOKWWKWK....",
    "...KWWWDDDOOOOKWKKWK....",
    "...KWWWWWWKKKKKWK.KWK...",
    "....KKKKKKK...KK...KK...",
};
const Sprite kSit = {24, 16, kSitRows, 19, 2};

const char* const kSleep1Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "...............KPK.KPK..",
    "..KKKKKKKKKKKKKKPOKKPOK.",
    ".KOOOOOOOOOOOOOKOOOOOOK.",
    "KDOOOOOOOOOOOOOOOOOOKKOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWK.",
    "KWWDDOOOOOOOOOOWWWWWWK..",
    ".KWWWWKKKKKKKKKWWWWWK...",
    "..KKKK.........KKKKK....",
};
const Sprite kSleep1 = {24, 16, kSleep1Rows, 19, 8};

const char* const kSleep2Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "..KKKKKKKKKKKKK.KPK.KPK.",
    ".KOOOOOOOOOOOOOKPOKKPOK.",
    "KDOOOOOOOOOOOOOOOOOOKKOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWK.",
    "KWWDDOOOOOOOOOOWWWWWWK..",
    ".KWWWWKKKKKKKKKWWWWWK...",
    "..KKKK.........KKKKK....",
};
const Sprite kSleep2 = {24, 16, kSleep2Rows, 20, 9};

const char* const kFlop1Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "...............KPK.KPK..",
    "..KKKKKKKKKKKKKKPOKKPOK.",
    ".KOOOOOOOOOOOOOKOOOOOOK.",
    "KDOOOOOOOOOOOOOOOOOOEOOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWPK",
    "KWWDDOOOOOOOOOOWWWWWWKPK",
    ".KWWWWKKKKKKKKKWWWWWK.K.",
    "..KKKK.........KKKKK....",
};
const Sprite kFlop1 = {24, 16, kFlop1Rows, 19, 8};

const char* const kFlop2Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "...............KPK.KPK..",
    "..KKKKKKKKKKKKKKPOKKPOK.",
    ".KOOOOOOOOOOOOOKOOOOOOK.",
    "KDOOOOOOOOOOOOOOOOOOEOOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWPK",
    "KWWDDOOOOOOOOOOWWWWWWKK.",
    ".KWWWWKKKKKKKKKWWWWWK...",
    "..KKKK.........KKKKK....",
};
const Sprite kFlop2 = {24, 16, kFlop2Rows, 19, 8};

const char* const kBelly1Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "..K.K......K.K..........",
    ".KWKWK....KWKWK.........",
    ".KWKWK....KWKWK.........",
    "KKWWWKKKKKKWWWKKKKKK....",
    "KWWWWWWWWWWWWWWWWWWOKK..",
    "KDWWWWWWWWWWWWWWWWOOOOK.",
    "KDDOOOOOOOOOOOOOOOOOEOOK",
    ".KDDOOOOOOOOOOOOOOOOWWEK",
    "..KKKKKKKKKKKKKKKKKPWWK.",
    "..................KPK...",
};
const Sprite kBelly1 = {24, 16, kBelly1Rows, 0, -1};

const char* const kBelly2Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "...K.K....K.K...........",
    "..KWKWK..KWKWK..........",
    "..KWKWK..KWKWK..........",
    "KKKWWWKKKKWWWKKKKKKK....",
    "KWWWWWWWWWWWWWWWWWWOKK..",
    "KDWWWWWWWWWWWWWWWWOOOOK.",
    "KDDOOOOOOOOOOOOOOOOOEOOK",
    ".KDDOOOOOOOOOOOOOOOOWWEK",
    "..KKKKKKKKKKKKKKKKKPWWK.",
    "..................KPK...",
};
const Sprite kBelly2 = {24, 16, kBelly2Rows, 0, -1};

const char* const kEat1Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    ".................K...K..",
    "................KPK.KPK.",
    "..KKKKKKKKKKKKKKKPOKKPOK",
    ".KOOOOOOOOOOOOOOKOOOOOOK",
    "KDOOOOOOOOOOOOOOOOOOOOK.",
    "KDOOOOOOOOOOOOOOOOOEOOK.",
    "KWDOOOOOOOOOOOOOOOOOOOK.",
    ".KWWDDOOOOOOOOOWWWOWWWK.",
    "..KWWKKKKKKKKKKWWKWWWEK.",
    "..KWK.........KWK.KWWK..",
    "..KWK.........KWK..KK...",
    "..KK..........KK........",
};
const Sprite kEat1 = {24, 16, kEat1Rows, 20, 6};

const char* const kEat2Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    ".................K...K..",
    "..KKKKKKKKKKKKKKKKPK.KPK",
    ".KOOOOOOOOOOOOOOKPOKKPOK",
    "KDOOOOOOOOOOOOOOKOOOOOOK",
    "KDOOOOOOOOOOOOOOOOOOOOK.",
    "KWDOOOOOOOOOOOOOOOOEOOK.",
    ".KWWDDOOOOOOOOOOOOOOOOK.",
    "..KWWKKKKKKKKKKWWKWWWWK.",
    "..KWK.........KWK.KWWEK.",
    "..KWK.........KWK..KK...",
    "..KK..........KK........",
};
const Sprite kEat2 = {24, 16, kEat2Rows, 20, 7};

const char* const kZRows[] = {
    "LLLL",
    "..L.",
    ".L..",
    "LLLL",
};
const Sprite kZ = {4, 4, kZRows, 0, -1};

const char* const kHeartRows[] = {
    ".KK.KK.",
    "KPPKPPK",
    "KPPPPPK",
    ".KPPPK.",
    "..KPK..",
    "...K...",
};
const Sprite kHeart = {7, 6, kHeartRows, 0, -1};

const char* const kSantaRows[] = {
    "......KK",
    "....KKWK",
    "..KKRRK.",
    ".KRRRRK.",
    "KWWWWWWK",
    "KKKKKKKK",
};
const Sprite kSanta = {8, 6, kSantaRows, 0, -1};

const char* const kPumpkinRows[] = {
    "...G...",
    "..KGK..",
    ".KQKQK.",
    "KQQKQQK",
    "KQQKQQK",
    ".KKKKK.",
};
const Sprite kPumpkin = {7, 6, kPumpkinRows, 0, -1};

const char* const kPartyRows[] = {
    "...Y...",
    "...K...",
    "..KBK..",
    "..KMK..",
    ".KBBBK.",
    ".KMMMK.",
    "KKKKKKK",
};
const Sprite kParty = {7, 7, kPartyRows, 0, -1};

const char* const kBallRows[] = {
    ".KKK.",
    "KTTTK",
    "KTWTK",
    "KTTTK",
    ".KKK.",
};
const Sprite kBall = {5, 5, kBallRows, 0, -1};

const char* const kBowlFullRows[] = {
    "..NYNNYN..",
    ".KYNNYNYK.",
    "KBBBBBBBBK",
    ".KBBBBBBK.",
    "..KKKKKK..",
};
const Sprite kBowlFull = {10, 5, kBowlFullRows, 0, -1};

const char* const kBowlEmptyRows[] = {
    "..........",
    ".K......K.",
    "KBBBBBBBBK",
    ".KBBBBBBK.",
    "..KKKKKK..",
};
const Sprite kBowlEmpty = {10, 5, kBowlEmptyRows, 0, -1};

// Colours that do not depend on the coat.
uint32_t BaseColor(char c) {
    switch (c) {
        case 'K': return 0xFF3B2418;
        case 'W': return 0xFFF8F4EE;
        case 'P': return 0xFFF5A6B8;
        case 'E': return 0xFF11111B;
        case 'L': return 0xFFB4BEFE;
        case 'R': return 0xFFE03B3B;
        case 'Q': return 0xFFFE8019;
        case 'G': return 0xFF40A02B;
        case 'Y': return 0xFFF9E2AF;
        case 'B': return 0xFF89B4FA;
        case 'M': return 0xFFCBA6F7;
        case 'T': return 0xFFC6E04A;
        case 'N': return 0xFF8B5A2B;
    }
    return 0;
}

// Main ('O') and shade ('D') colour of each coat, in Coat order.
const uint32_t kCoatColors[][2] = {
    {0xFFF0913A, 0xFFC96A24},  // red
    {0xFF2E2A33, 0xFFB8743A},  // tricolor
    {0xFFB9773E, 0xFF6E4122},  // sable
    {0xFFF2DDB4, 0xFFD6B88A},  // cream
};

enum Coat { kRed, kTricolor, kSable, kCream, kCoatCount };
enum Hat { kNoHat, kSantaHat, kPumpkinHat, kPartyHat };

constexpr int kCanvasW = 30;
constexpr int kCanvasH = 26;
constexpr int kDogX = 3;  // where the 24x16 dog sits inside the canvas
constexpr int kDogY = 10;
constexpr int kDogW = 24;
constexpr int kDogH = 16;
constexpr int kMouthX = 22;  // snout column in standing frames
constexpr int kMaxCorgis = 3;
constexpr int kMealHours[] = {8, 13, 19};

constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr PCWSTR kClassName = L"WindhawkTaskbarCorgi";

const GUID kClsidMMDeviceEnumerator = {
    0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID kIidIMMDeviceEnumerator = {
    0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID kIidIAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// IAudioMeterInformation is only forward-declared in the bundled headers.
struct AudioMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count,
                                                            float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};

uint32_t PaletteColor(char c, int coat) {
    if (c == 'O') {
        return kCoatColors[coat][0];
    }
    if (c == 'D') {
        return kCoatColors[coat][1];
    }
    return BaseColor(c);
}

// ---------------------------------------------------------------------------
// Drawing

struct Canvas {
    int w = 0;
    int h = 0;
    uint32_t px[kCanvasW * kCanvasH];

    void Reset(int width, int height) {
        w = width;
        h = height;
        memset(px, 0, sizeof(px));
    }

    // rearShift moves the back half of the body (not the legs) down, for the
    // butt wiggle.
    void Draw(const Sprite& s, int ox, int oy, int coat = kRed,
              int rearShift = 0) {
        for (int y = 0; y < s.h; y++) {
            for (int x = 0; x < s.w; x++) {
                uint32_t color = PaletteColor(s.rows[y][x], coat);
                if (!color) {
                    continue;
                }
                int cx = ox + x;
                int cy = oy + y + (x < 9 && y < 12 ? rearShift : 0);
                if (cx >= 0 && cx < w && cy >= 0 && cy < h) {
                    px[cy * w + cx] = color;
                }
            }
        }
    }
};

// A click-through, always-on-top layered window showing one canvas.
struct Layer {
    HWND hwnd = nullptr;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    uint32_t* bits = nullptr;
    int bitmapW = 0;
    int bitmapH = 0;
    bool shown = false;
};

HINSTANCE g_hInstance = nullptr;

bool CreateLayer(Layer& l) {
    l.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                 WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                 WS_EX_NOACTIVATE,
                             kClassName, L"Taskbar Corgi", WS_POPUP, 0, 0, 1, 1,
                             nullptr, nullptr, g_hInstance, nullptr);
    if (!l.hwnd) {
        return false;
    }
    l.dc = CreateCompatibleDC(nullptr);
    return true;
}

void DestroyLayer(Layer& l) {
    if (l.bitmap) {
        SelectObject(l.dc, l.oldBitmap);
        DeleteObject(l.bitmap);
    }
    if (l.dc) {
        DeleteDC(l.dc);
    }
    if (l.hwnd) {
        DestroyWindow(l.hwnd);
    }
    l = Layer{};
}

void HideLayer(Layer& l) {
    if (l.shown) {
        ShowWindow(l.hwnd, SW_HIDE);
        l.shown = false;
    }
}

void PresentLayer(Layer& l, const Canvas& c, bool flip, int scale, int x,
                  int y) {
    if (!l.hwnd) {
        return;
    }

    int w = c.w * scale;
    int h = c.h * scale;
    if (!l.bits || l.bitmapW != w || l.bitmapH != h) {
        if (l.bitmap) {
            SelectObject(l.dc, l.oldBitmap);
            DeleteObject(l.bitmap);
            l.bitmap = nullptr;
            l.bits = nullptr;
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;  // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        l.bitmap =
            CreateDIBSection(l.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!l.bitmap) {
            return;
        }
        l.oldBitmap = SelectObject(l.dc, l.bitmap);
        l.bits = static_cast<uint32_t*>(bits);
        l.bitmapW = w;
        l.bitmapH = h;
    }

    for (int cy = 0; cy < c.h; cy++) {
        for (int cx = 0; cx < c.w; cx++) {
            uint32_t color = c.px[cy * c.w + (flip ? c.w - 1 - cx : cx)];
            for (int sy = 0; sy < scale; sy++) {
                uint32_t* row = l.bits + (size_t)(cy * scale + sy) * w;
                for (int sx = 0; sx < scale; sx++) {
                    row[cx * scale + sx] = color;
                }
            }
        }
    }

    POINT dst = {x, y};
    SIZE size = {w, h};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(l.hwnd, nullptr, &dst, &size, l.dc, &src, 0, &blend,
                        ULW_ALPHA);
    if (!l.shown) {
        ShowWindow(l.hwnd, SW_SHOWNOACTIVATE);
        l.shown = true;
    }
}

// ---------------------------------------------------------------------------
// State

struct Settings {
    int size = 3;
    int corgis = 1;
    int coat = kRed;
    bool dance = true;
    bool dayNight = true;
    bool hats = true;
    int birthMonth = 0;
    int birthDay = 0;
    bool fetch = true;
    bool bowl = true;
    bool chase = false;
    bool hideInFullscreen = true;
    double speed = 1.0;
};

enum class State {
    Walk,
    Sit,
    Sleep,
    GoNap,   // walking over to nap next to a friend
    Watch,   // being petted, or watching the cursor
    Belly,   // belly rub
    Chase,   // chasing the cursor
    Zoom,    // zoomies
    Follow,  // joining a friend's zoomies or fetch run
    Pant,
    Dance,
    Fetch,  // running to the ball
    Carry,  // bringing the ball back
    GoEat,
    Eat,
};

struct Corgi {
    Layer layer;
    int coat = kRed;
    bool placed = false;
    State state = State::Sit;
    double x = 0;  // screen x of the canvas' left edge
    int dir = 1;   // 1 = facing right, -1 = facing left
    bool moving = false;
    double targetX = 0;
    double stateLeft = 2.0;
    double anim = 0;
    double petTime = 0;
    double heart = 0;
    double hop = 0;  // dance hop height in sprite pixels
    int beats = 0;
    int leader = -1;
    double homeX = 0;
    int mealEaten = -1;
};

struct Ball {
    Layer layer;
    bool active = false;
    double x = 0;  // screen x of the ball's left edge
    double vx = 0;
    int carrier = -1;
    double restLeft = -1;  // < 0: waiting to be fetched
    double nextIn = 0;
};

struct Bowl {
    Layer layer;
    bool full = false;
    int mealKey = -1;
    int lastMealKey = -1;
    double x = 0;
};

// Recomputed every tick.
struct Env {
    double dt = 0;
    int scale = 1;
    int ground = 0;  // screen y the corgis stand on
    int winY = 0;    // screen y of the corgi windows
    double minX = 0;
    double maxX = 0;
    RECT taskbar = {};
    RECT monitor = {};
    POINT cursor = {};
    int hour = 12;
    int hat = kNoHat;
    bool beat = false;
};

// Everything below is only touched on the worker thread.
Settings g_settings;
Corgi g_corgis[kMaxCorgis];
int g_corgiCount = 0;
Ball g_ball;
Bowl g_bowl;
Env g_env;
uint32_t g_rng = 1;
ULONGLONG g_lastTick = 0;
double g_topmostTimer = 0;
UINT_PTR g_timer = 0;
bool g_mealsInitialized = false;

AudioMeter* g_meter = nullptr;
double g_meterAge = 10.0;
double g_audioAvg = 0;
double g_loudTime = 0;
double g_quietTime = 0;
double g_sinceBeat = 0;
bool g_music = false;

HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_stopEvent = nullptr;

double Rand() {
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return (g_rng & 0xFFFFFF) / 16777216.0;
}

double Clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

void LoadSettings() {
    Settings s;
    s.size = (int)Clamp(Wh_GetIntSetting(L"size"), 1, 8);
    s.corgis = (int)Clamp(Wh_GetIntSetting(L"corgis"), 1, kMaxCorgis);

    PCWSTR coat = Wh_GetStringSetting(L"coat");
    s.coat = wcscmp(coat, L"tricolor") == 0 ? kTricolor
             : wcscmp(coat, L"sable") == 0  ? kSable
             : wcscmp(coat, L"cream") == 0  ? kCream
                                            : kRed;
    Wh_FreeStringSetting(coat);

    s.dance = Wh_GetIntSetting(L"danceToMusic") != 0;
    s.dayNight = Wh_GetIntSetting(L"dayNight") != 0;
    s.hats = Wh_GetIntSetting(L"hats") != 0;

    PCWSTR birthday = Wh_GetStringSetting(L"birthday");
    int month = 0;
    int day = 0;
    if (swscanf(birthday, L"%d-%d", &month, &day) == 2 && month >= 1 &&
        month <= 12 && day >= 1 && day <= 31) {
        s.birthMonth = month;
        s.birthDay = day;
    }
    Wh_FreeStringSetting(birthday);

    s.fetch = Wh_GetIntSetting(L"fetch") != 0;
    s.bowl = Wh_GetIntSetting(L"foodBowl") != 0;
    s.chase = Wh_GetIntSetting(L"chaseMouse") != 0;
    s.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    s.speed = Clamp(Wh_GetIntSetting(L"speed"), 25, 300) / 100.0;
    g_settings = s;
}

// Creates or removes windows to match the settings.
void ApplySettings() {
    for (int i = 0; i < kMaxCorgis; i++) {
        Corgi& c = g_corgis[i];
        if (i < g_settings.corgis) {
            if (!c.layer.hwnd) {
                c = Corgi{};
                CreateLayer(c.layer);
            }
            c.coat = (g_settings.coat + i) % kCoatCount;
            if (c.leader >= g_settings.corgis) {
                c.state = State::Sit;
                c.leader = -1;
            }
        } else if (c.layer.hwnd) {
            DestroyLayer(c.layer);
        }
    }
    g_corgiCount = g_settings.corgis;

    if (g_ball.carrier >= g_corgiCount || !g_settings.fetch) {
        g_ball.carrier = -1;
        g_ball.active = false;
    }
    if (g_settings.fetch && !g_ball.layer.hwnd) {
        CreateLayer(g_ball.layer);
        g_ball.nextIn = 60 + Rand() * 120;
    } else if (!g_settings.fetch && g_ball.layer.hwnd) {
        DestroyLayer(g_ball.layer);
    }

    if (g_settings.bowl && !g_bowl.layer.hwnd) {
        CreateLayer(g_bowl.layer);
    } else if (!g_settings.bowl && g_bowl.layer.hwnd) {
        DestroyLayer(g_bowl.layer);
        g_bowl.full = false;
    }
}

void HideAll() {
    for (int i = 0; i < g_corgiCount; i++) {
        HideLayer(g_corgis[i].layer);
    }
    HideLayer(g_ball.layer);
    HideLayer(g_bowl.layer);
}

// ---------------------------------------------------------------------------
// Audio: a peak meter on the default playback device.

void ReleaseMeter() {
    if (g_meter) {
        g_meter->Release();
        g_meter = nullptr;
    }
}

void AcquireMeter() {
    IMMDeviceEnumerator* enumerator = nullptr;
    if (FAILED(CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                kIidIMMDeviceEnumerator,
                                reinterpret_cast<void**>(&enumerator)))) {
        return;
    }
    IMMDevice* device = nullptr;
    if (SUCCEEDED(
            enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
        device->Activate(kIidIAudioMeterInformation, CLSCTX_ALL, nullptr,
                         reinterpret_cast<void**>(&g_meter));
        device->Release();
    }
    enumerator->Release();
}

// Updates g_music and returns true on a beat.
bool UpdateAudio(double dt) {
    g_sinceBeat += dt;
    if (!g_settings.dance) {
        ReleaseMeter();
        g_music = false;
        return false;
    }

    // Re-open the meter every 10 seconds to follow default device changes.
    g_meterAge += dt;
    if (g_meterAge >= 10.0) {
        ReleaseMeter();
        AcquireMeter();
        g_meterAge = 0;
    }

    float peak = 0;
    if (g_meter && FAILED(g_meter->GetPeakValue(&peak))) {
        ReleaseMeter();
        peak = 0;
    }

    g_audioAvg += (peak - g_audioAvg) * Clamp(dt / 1.5, 0, 1);
    if (g_audioAvg > 0.015) {
        g_loudTime += dt;
        g_quietTime = 0;
    } else {
        g_quietTime += dt;
        g_loudTime = 0;
    }
    if (!g_music && g_loudTime > 1.5) {
        g_music = true;
    } else if (g_music && g_quietTime > 2.5) {
        g_music = false;
    }

    bool beat =
        g_music && peak > g_audioAvg * 1.3 + 0.02 && g_sinceBeat > 0.3;
    if (beat) {
        g_sinceBeat = 0;
    }
    return beat;
}

// ---------------------------------------------------------------------------
// Behaviour

bool IsAsleep(const Corgi& c) {
    return c.state == State::Sleep || c.state == State::GoNap;
}

bool IsFree(const Corgi& c) {
    return c.state == State::Sit || c.state == State::Walk ||
           c.state == State::Watch || c.state == State::Dance;
}

bool HasBall(const Corgi& c) {
    return c.state == State::Fetch || c.state == State::Carry;
}

double DogCenter(const Corgi& c) {
    return c.x + (kDogX + kDogW / 2.0) * g_env.scale;
}

// Canvas x that puts sprite column `col` at screen x `screenX`.
double CanvasXFor(double screenX, int col, int dir) {
    int canvasCol = dir > 0 ? kDogX + col : kCanvasW - 1 - (kDogX + col);
    return screenX - (canvasCol + 0.5) * g_env.scale;
}

// Moves toward `target`. Returns true once there.
bool MoveToward(Corgi& c, double target, double speed) {
    double d = target - c.x;
    if (std::fabs(d) <= speed * g_env.dt + 0.5) {
        c.x = target;
        return true;
    }
    c.dir = d > 0 ? 1 : -1;
    c.x += c.dir * speed * g_env.dt;
    c.moving = true;
    return false;
}

void SitFor(Corgi& c, double seconds) {
    c.state = State::Sit;
    c.stateLeft = seconds;
}

void StartWalk(Corgi& c) {
    double target = c.x;
    for (int i = 0; i < 5; i++) {
        target = g_env.minX + Rand() * (g_env.maxX - g_env.minX);
        if (std::fabs(target - c.x) > 40.0 * g_env.scale) {
            break;
        }
    }
    c.targetX = target;
    c.state = State::Walk;
}

void StartSleep(int i, double seconds) {
    Corgi& c = g_corgis[i];
    c.state = State::Sleep;
    c.stateLeft = seconds;

    // Friends may come over and nap in a pile.
    int side = 1;
    for (int j = 0; j < g_corgiCount; j++) {
        Corgi& f = g_corgis[j];
        if (j == i || !IsFree(f) || Rand() > 0.6) {
            continue;
        }
        f.state = State::GoNap;
        f.targetX = Clamp(c.x + side * 17.0 * g_env.scale, g_env.minX,
                          g_env.maxX);
        f.stateLeft = seconds * (0.8 + Rand() * 0.4);
        side = -side;
    }
}

void StartZoomies(int i) {
    Corgi& c = g_corgis[i];
    c.state = State::Zoom;
    c.stateLeft = 4.0 + Rand() * 3.0;
    double mid = (g_env.minX + g_env.maxX) / 2;
    double range = g_env.maxX - g_env.minX;
    c.targetX = c.x < mid ? g_env.maxX - Rand() * range * 0.2
                          : g_env.minX + Rand() * range * 0.2;

    for (int j = 0; j < g_corgiCount; j++) {
        Corgi& f = g_corgis[j];
        if (j != i && IsFree(f) && Rand() < 0.7) {
            f.state = State::Follow;
            f.leader = i;
        }
    }
}

void StartDance(Corgi& c, double seconds) {
    c.state = State::Dance;
    c.stateLeft = seconds;
    c.beats = 0;
}

void PickNextActivity(int i) {
    Corgi& c = g_corgis[i];
    int hour = g_env.hour;
    bool dayNight = g_settings.dayNight;

    if (dayNight && hour >= 2 && hour < 6) {
        StartSleep(i, 60 + Rand() * 120);
        return;
    }

    bool evening = dayNight && (hour >= 22 || hour < 2);
    bool morning = dayNight && hour >= 7 && hour < 11;
    double wSit = 0.35;
    double wWalk = 0.30;
    double wSleep = evening ? 0.45 : 0.15;
    double wZoom = morning ? 0.15 : 0.06;
    double wDance = g_music ? 0.5 : 0.0;

    double r = Rand() * (wSit + wWalk + wSleep + wZoom + wDance);
    if ((r -= wSit) < 0) {
        SitFor(c, 3.0 + Rand() * 5.0);
    } else if ((r -= wWalk) < 0) {
        StartWalk(c);
    } else if ((r -= wSleep) < 0) {
        StartSleep(i, 20 + Rand() * 30);
    } else if ((r -= wZoom) < 0) {
        StartZoomies(i);
    } else {
        StartDance(c, 8 + Rand() * 12);
    }
}

void UpdateCorgi(int i) {
    Corgi& c = g_corgis[i];
    const Env& e = g_env;
    double s = e.scale;
    double walkSpeed = 14.0 * s * g_settings.speed;
    double runSpeed = 48.0 * s * g_settings.speed;
    double zoomSpeed = 85.0 * s * g_settings.speed;

    if (!c.placed) {
        c.placed = true;
        c.x = e.minX + (e.maxX - e.minX) * (0.2 + Rand() * 0.6);
        c.dir = Rand() < 0.5 ? 1 : -1;
        SitFor(c, 1.0 + Rand() * 2.0);
    }

    c.anim += e.dt;
    c.moving = false;
    if (c.heart > 0) {
        c.heart -= e.dt;
    }
    if (c.hop > 0) {
        c.hop = c.hop > e.dt * 14 ? c.hop - e.dt * 14 : 0;
    }

    // Petting.
    RECT dogRect = {(LONG)(c.x + kDogX * s), (LONG)(e.ground - kDogH * s),
                    (LONG)(c.x + (kDogX + kDogW) * s), e.ground};
    bool petted = PtInRect(&dogRect, e.cursor);
    if (petted) {
        c.petTime += e.dt;
        if (c.heart <= 0 && c.petTime > 0.4) {
            c.heart = 1.6;
            if (IsAsleep(c)) {
                c.state = State::Watch;
            }
        }
    } else {
        c.petTime = 0;
    }

    // Food comes first, even for a sleeping corgi.
    if (g_bowl.full && c.mealEaten != g_bowl.mealKey && !HasBall(c) &&
        c.state != State::GoEat && c.state != State::Eat) {
        c.state = State::GoEat;
    }

    bool busy = c.state == State::Zoom || c.state == State::Follow ||
                c.state == State::Pant || HasBall(c) ||
                c.state == State::GoEat || c.state == State::Eat;

    if (petted && !busy && !IsAsleep(c)) {
        c.state = c.petTime > 3.0 ? State::Belly : State::Watch;
        return;
    }
    if (c.state == State::Belly) {
        SitFor(c, 2.0);
    }

    if (g_settings.chase && !busy && !IsAsleep(c)) {
        double dist = e.cursor.x - DogCenter(c);
        bool nearby = e.cursor.y >= e.winY - 30 * s &&
                      e.cursor.y <= e.taskbar.bottom &&
                      std::fabs(dist) < 90.0 * s;
        if (nearby) {
            if (std::fabs(dist) > 6.0 * s) {
                c.state = State::Chase;
                MoveToward(c, c.x + dist, runSpeed);
            } else {
                c.state = State::Watch;
            }
            return;
        }
    }
    if (c.state == State::Chase || c.state == State::Watch) {
        SitFor(c, 1.5 + Rand() * 2.5);
    }

    switch (c.state) {
        case State::Walk:
            if (MoveToward(c, c.targetX, walkSpeed)) {
                PickNextActivity(i);
            }
            break;

        case State::GoNap:
            if (MoveToward(c, c.targetX, walkSpeed)) {
                c.state = State::Sleep;
            }
            break;

        case State::Sit:
        case State::Sleep:
        case State::Pant:
            c.stateLeft -= e.dt;
            if (c.stateLeft <= 0) {
                if (c.state == State::Sit) {
                    PickNextActivity(i);
                } else {
                    SitFor(c, 1.5 + Rand());
                }
            }
            break;

        case State::Dance:
            if (!g_music) {
                SitFor(c, 2.0);
                break;
            }
            if (e.beat) {
                c.hop = 3.0;
                if (++c.beats % 4 == 0) {
                    c.dir = -c.dir;
                }
            }
            c.stateLeft -= e.dt;
            if (c.stateLeft <= 0) {
                PickNextActivity(i);
            }
            break;

        case State::Zoom:
            c.stateLeft -= e.dt;
            if (c.stateLeft <= 0) {
                c.state = State::Pant;
                c.stateLeft = 3.0 + Rand() * 2.0;
            } else if (MoveToward(c, c.targetX, zoomSpeed)) {
                double range = e.maxX - e.minX;
                c.targetX = c.dir > 0 ? e.minX + Rand() * range * 0.3
                                      : e.maxX - Rand() * range * 0.3;
            }
            break;

        case State::Follow: {
            const Corgi& l = g_corgis[c.leader];
            if (l.state == State::Zoom || HasBall(l)) {
                double target = Clamp(l.x - l.dir * 20.0 * s, e.minX, e.maxX);
                double speed =
                    l.state == State::Zoom ? zoomSpeed * 0.9 : runSpeed;
                if (MoveToward(c, target, speed)) {
                    c.dir = l.dir;
                }
            } else if (l.state == State::Pant) {
                c.state = State::Pant;
                c.stateLeft = 3.0 + Rand() * 2.0;
            } else {
                SitFor(c, 2.0);
            }
            break;
        }

        case State::Fetch: {
            if (!g_ball.active || g_ball.carrier >= 0) {
                SitFor(c, 2.0);
                break;
            }
            double ballX = g_ball.x + 2.5 * s;
            int dir = ballX > DogCenter(c) ? 1 : -1;
            if (MoveToward(c, CanvasXFor(ballX, kMouthX, dir), runSpeed)) {
                c.dir = dir;
                g_ball.carrier = i;
                c.state = State::Carry;
                c.targetX = c.homeX;
            }
            break;
        }

        case State::Carry:
            if (MoveToward(c, c.targetX, runSpeed * 0.7)) {
                int mouthCol = c.dir > 0 ? kDogX + kMouthX
                                         : kCanvasW - 1 - (kDogX + kMouthX);
                g_ball.carrier = -1;
                g_ball.vx = 0;
                g_ball.x = c.x + (mouthCol - 2) * s;
                g_ball.restLeft = 3.0;
                SitFor(c, 3.0);
                c.heart = 1.6;
            }
            break;

        case State::GoEat: {
            // Even corgis eat from the left side, odd ones from the right.
            int dir = i % 2 == 0 ? 1 : -1;
            double bowlX = g_bowl.x + 5 * s;
            double target = CanvasXFor(bowlX, 21, dir);
            bool spotTaken = false;
            for (int j = 0; j < g_corgiCount; j++) {
                if (j != i && j % 2 == i % 2 &&
                    g_corgis[j].state == State::Eat) {
                    spotTaken = true;
                }
            }
            if (spotTaken) {
                target -= dir * 26.0 * s;  // wait in line
            }
            if (MoveToward(c, Clamp(target, e.minX, e.maxX), runSpeed)) {
                c.dir = dir;
                if (!spotTaken) {
                    c.state = State::Eat;
                    c.stateLeft = 6.0;
                }
            }
            break;
        }

        case State::Eat:
            c.stateLeft -= e.dt;
            if (c.stateLeft <= 0) {
                c.mealEaten = g_bowl.mealKey;
                SitFor(c, 3.0);
                c.heart = 1.6;
                bool allFed = true;
                for (int j = 0; j < g_corgiCount; j++) {
                    if (g_corgis[j].mealEaten != g_bowl.mealKey) {
                        allFed = false;
                    }
                }
                if (allFed) {
                    g_bowl.full = false;
                }
            }
            break;

        case State::Watch:
        case State::Belly:
        case State::Chase:
            break;
    }

    c.x = Clamp(c.x, e.minX, e.maxX);
}

const Sprite& PickFrame(const Corgi& c, int* lift) {
    auto alt = [&](double period, const Sprite& a, const Sprite& b)
        -> const Sprite& { return std::fmod(c.anim, period) < period / 2 ? a : b; };
    *lift = 0;

    switch (c.state) {
        case State::Walk:
        case State::GoNap:
        case State::Carry:
            return c.moving ? alt(0.36, kWalk1, kWalk2) : kSit;

        case State::GoEat:
        case State::Chase:
        case State::Fetch:
        case State::Follow:
        case State::Zoom: {
            if (!c.moving) {
                return kSit;
            }
            const Sprite& f =
                alt(c.state == State::Zoom ? 0.14 : 0.2, kRun1, kRun2);
            *lift = &f == &kRun2 ? 1 : 0;
            return f;
        }

        case State::Sleep:
            return alt(2.4, kSleep1, kSleep2);
        case State::Pant:
            return alt(0.4, kFlop1, kFlop2);
        case State::Belly:
            return alt(0.5, kBelly1, kBelly2);
        case State::Eat:
            return alt(0.6, kEat1, kEat2);
        case State::Dance:
            *lift = (int)std::lround(c.hop);
            return c.hop > 0.5 ? kWalk1 : kSit;
        case State::Watch:
            return c.petTime > 0 ? kWalk1 : kSit;
        case State::Sit:
            return kSit;
    }
    return kSit;
}

void RenderCorgi(Corgi& c) {
    const Env& e = g_env;
    Canvas canvas;
    canvas.Reset(kCanvasW, kCanvasH);

    int lift = 0;
    const Sprite& frame = PickFrame(c, &lift);
    int dogY = kDogY - lift;
    int wiggle = c.state == State::Watch && c.petTime > 0 &&
                         std::fmod(c.anim, 0.24) < 0.12
                     ? 1
                     : 0;
    canvas.Draw(frame, kDogX, dogY, c.coat, wiggle);

    if (e.hat != kNoHat && frame.hatY >= 0) {
        const Sprite& hat = e.hat == kSantaHat     ? kSanta
                            : e.hat == kPumpkinHat ? kPumpkin
                                                   : kParty;
        canvas.Draw(hat, kDogX + frame.hatX - hat.w / 2,
                    dogY + frame.hatY - hat.h + 1);
    }
    if (c.state == State::Carry) {
        canvas.Draw(kBall, kDogX + kMouthX - 2, dogY + 6);
    }
    if (c.state == State::Sleep) {
        for (int i = 0; i < 2; i++) {
            double p = std::fmod(c.anim + i * 1.2, 2.4) / 2.4;
            if (p < 0.9) {
                canvas.Draw(kZ, kDogX + 20 + (int)(p * 2),
                            kDogY + 4 - (int)(p * 12));
            }
        }
    }
    if (c.heart > 0) {
        canvas.Draw(kHeart, kDogX + 9, 1 + (int)(c.heart / 1.6 * 3));
    }

    PresentLayer(c.layer, canvas, c.dir < 0, e.scale, (int)std::lround(c.x),
                 e.winY);
}

void UpdateBall() {
    const Env& e = g_env;
    double s = e.scale;
    if (!g_settings.fetch) {
        return;
    }

    if (!g_ball.active) {
        HideLayer(g_ball.layer);
        g_ball.nextIn -= e.dt;
        if (g_ball.nextIn > 0) {
            return;
        }

        int fetcher = -1;
        for (int i = 0; i < g_corgiCount; i++) {
            if (IsFree(g_corgis[i]) && g_corgis[i].state != State::Watch) {
                fetcher = i;
                break;
            }
        }
        if (fetcher < 0) {
            g_ball.nextIn = 30;
            return;
        }

        bool fromLeft = Rand() < 0.5;
        g_ball.active = true;
        g_ball.carrier = -1;
        g_ball.restLeft = -1;
        g_ball.x = fromLeft ? e.monitor.left - 5 * s : e.monitor.right;
        g_ball.vx = (fromLeft ? 1 : -1) * (110 + Rand() * 80) * s;

        Corgi& f = g_corgis[fetcher];
        f.state = State::Fetch;
        f.homeX = f.x;
        for (int j = 0; j < g_corgiCount; j++) {
            Corgi& o = g_corgis[j];
            if (j != fetcher && IsFree(o) && Rand() < 0.5) {
                o.state = State::Follow;
                o.leader = fetcher;
            }
        }
        return;
    }

    if (g_ball.carrier >= 0) {
        HideLayer(g_ball.layer);  // drawn in the corgi's mouth
        return;
    }

    g_ball.x += g_ball.vx * e.dt;
    g_ball.vx *= std::exp(-1.0 * e.dt);
    if (std::fabs(g_ball.vx) < 3 * s) {
        g_ball.vx = 0;
    }
    double left = e.monitor.left;
    double right = e.monitor.right - 5 * s;
    if (g_ball.x < left && g_ball.vx < 0) {
        g_ball.vx = -g_ball.vx * 0.5;
    } else if (g_ball.x > right && g_ball.vx > 0) {
        g_ball.vx = -g_ball.vx * 0.5;
    }

    // Nobody is going for it any more: let it disappear.
    bool wanted = false;
    for (int i = 0; i < g_corgiCount; i++) {
        if (g_corgis[i].state == State::Fetch) {
            wanted = true;
        }
    }
    if (!wanted && g_ball.restLeft < 0) {
        g_ball.restLeft = 3.0;
    }
    if (g_ball.restLeft >= 0) {
        g_ball.restLeft -= e.dt;
        if (g_ball.restLeft < 0) {
            g_ball.active = false;
            g_ball.nextIn = 180 + Rand() * 300;
            HideLayer(g_ball.layer);
            return;
        }
    }

    Canvas canvas;
    canvas.Reset(kBall.w, kBall.h);
    canvas.Draw(kBall, 0, 0);
    PresentLayer(g_ball.layer, canvas, false, e.scale, (int)std::lround(g_ball.x),
                 e.ground - kBall.h * e.scale);
}

void UpdateBowl(const SYSTEMTIME& now) {
    const Env& e = g_env;
    if (!g_settings.bowl) {
        return;
    }
    g_bowl.x = e.monitor.left + 50.0 * e.scale;

    int slot = -1;
    for (int i = 0; i < (int)ARRAYSIZE(kMealHours); i++) {
        if (now.wHour >= kMealHours[i]) {
            slot = i;
        }
    }
    if (slot >= 0) {
        int key = ((now.wYear * 13 + now.wMonth) * 32 + now.wDay) * 4 + slot;
        if (key != g_bowl.lastMealKey) {
            g_bowl.lastMealKey = key;
            // Don't serve a meal just because the mod was (re)loaded.
            if (g_mealsInitialized && now.wHour - kMealHours[slot] < 1) {
                g_bowl.full = true;
                g_bowl.mealKey = key;
            }
        }
    }
    g_mealsInitialized = true;

    const Sprite& sprite = g_bowl.full ? kBowlFull : kBowlEmpty;
    Canvas canvas;
    canvas.Reset(sprite.w, sprite.h);
    canvas.Draw(sprite, 0, 0);
    PresentLayer(g_bowl.layer, canvas, false, e.scale, (int)std::lround(g_bowl.x),
                 e.ground - sprite.h * e.scale);
}

bool IsFullscreenAppActive(const RECT& monitor) {
    HWND fg = GetForegroundWindow();
    if (!fg) {
        return false;
    }

    WCHAR cls[64];
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls))) {
        for (PCWSTR skip : {L"Progman", L"WorkerW", L"Shell_TrayWnd"}) {
            if (wcscmp(cls, skip) == 0) {
                return false;
            }
        }
    }

    RECT r;
    if (!GetWindowRect(fg, &r)) {
        return false;
    }
    return r.left <= monitor.left && r.top <= monitor.top &&
           r.right >= monitor.right && r.bottom >= monitor.bottom;
}

void Tick() {
    ULONGLONG now = GetTickCount64();
    double dt = g_lastTick ? (now - g_lastTick) / 1000.0 : 0.0;
    g_lastTick = now;
    Env& e = g_env;
    e.dt = Clamp(dt, 0, 0.1);

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!taskbar || !GetWindowRect(taskbar, &e.taskbar) ||
        !GetMonitorInfoW(MonitorFromWindow(taskbar, MONITOR_DEFAULTTOPRIMARY),
                         &mi)) {
        HideAll();
        return;
    }
    e.monitor = mi.rcMonitor;

    if (g_settings.hideInFullscreen && IsFullscreenAppActive(e.monitor)) {
        HideAll();
        return;
    }

    UINT dpi = GetDpiForWindow(taskbar);
    e.scale = (int)std::lround(g_settings.size * (dpi ? dpi : 96) / 96.0);
    if (e.scale < 1) {
        e.scale = 1;
    }
    e.ground = e.taskbar.top < e.monitor.bottom ? e.taskbar.top
                                                : e.monitor.bottom;
    e.winY = e.ground - kCanvasH * e.scale;
    e.minX = e.monitor.left - kDogX * e.scale;
    e.maxX = e.monitor.right - (kDogX + kDogW) * e.scale;
    GetCursorPos(&e.cursor);

    SYSTEMTIME st;
    GetLocalTime(&st);
    e.hour = st.wHour;
    e.hat = kNoHat;
    if (g_settings.hats) {
        if (st.wMonth == g_settings.birthMonth &&
            st.wDay == g_settings.birthDay) {
            e.hat = kPartyHat;
        } else if (st.wMonth == 12) {
            e.hat = kSantaHat;
        } else if (st.wMonth == 10 && st.wDay >= 24) {
            e.hat = kPumpkinHat;
        }
    }

    bool hadMusic = g_music;
    e.beat = UpdateAudio(e.dt);
    if (g_music && !hadMusic) {
        for (int i = 0; i < g_corgiCount; i++) {
            if (IsFree(g_corgis[i]) && g_corgis[i].state != State::Watch) {
                StartDance(g_corgis[i], 8 + Rand() * 12);
            }
        }
    }

    UpdateBowl(st);
    for (int i = 0; i < g_corgiCount; i++) {
        UpdateCorgi(i);
    }
    UpdateBall();
    for (int i = 0; i < g_corgiCount; i++) {
        RenderCorgi(g_corgis[i]);
    }

    // Keep the windows above other always-on-top windows, the bowl and the
    // ball in front of the corgis.
    g_topmostTimer -= e.dt;
    if (g_topmostTimer <= 0) {
        g_topmostTimer = 2.0;
        auto raise = [](const Layer& l) {
            if (l.hwnd) {
                SetWindowPos(l.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        };
        for (int i = 0; i < g_corgiCount; i++) {
            raise(g_corgis[i].layer);
        }
        raise(g_bowl.layer);
        raise(g_ball.layer);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // create the queue

    // Only the explorer.exe instance that owns the taskbar gets corgis.
    for (;;) {
        HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbar) {
            DWORD pid = 0;
            GetWindowThreadProcessId(taskbar, &pid);
            if (pid != GetCurrentProcessId()) {
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_rng = (uint32_t)GetTickCount64() | 1;

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    LoadSettings();
    ApplySettings();
    g_timer = SetTimer(nullptr, 0, 33, nullptr);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd) {
            if (msg.message == WM_TIMER && msg.wParam == g_timer) {
                Tick();
                continue;
            }
            if (msg.message == kMsgReloadSettings) {
                LoadSettings();
                ApplySettings();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    KillTimer(nullptr, g_timer);
    for (Corgi& c : g_corgis) {
        DestroyLayer(c.layer);
    }
    DestroyLayer(g_ball.layer);
    DestroyLayer(g_bowl.layer);
    ReleaseMeter();
    UnregisterClassW(kClassName, g_hInstance);
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WndProc), &g_hInstance);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_threadId) {
        PostThreadMessageW(g_threadId, kMsgReloadSettings, 0, 0);
    }
}

void Wh_ModUninit() {
    if (!g_thread) {
        return;
    }
    SetEvent(g_stopEvent);
    while (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
        if (WaitForSingleObject(g_thread, 10) == WAIT_OBJECT_0) {
            break;
        }
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_stopEvent);
    g_thread = nullptr;
    g_stopEvent = nullptr;
    g_threadId = 0;
}
