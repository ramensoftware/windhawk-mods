// ==WindhawkMod==
// @id              confetti-spawner
// @name            Confetti Spawner
// @description     Hold a hotkey to fire confetti or emoji across your screen
// @version         1.4.0
// @author          JudeCJz
// @github          https://github.com/JudeCJz
// @homepage        https://github.com/JudeCJz/confetti-spawner
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lgdi32 -luser32 -lole32 -lshell32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Confetti Spawner

Press a hotkey and a burst of confetti (or color emoji) flies across the monitor
under your mouse pointer. Hold the hotkey to keep it going. The effect is drawn on a click-through overlay that
never takes focus, so you can keep typing and clicking while it plays.

![Confetti Spawner demo](https://raw.githubusercontent.com/JudeCJz/confetti-spawner/main/assets/windhawk-demo.gif)

The overlay only exists while particles are on screen. When the last particle is
gone the window is destroyed and the frame timer stops, so the mod uses no CPU
while idle.

The mod runs in its own small Windhawk process and does not inject into any
other program.

## Hotkeys

| Default | Action |
| --- | --- |
| `Ctrl+Shift+C` | Fire a burst. Hold the keys to keep firing |
| `Ctrl+Shift+E` | Cycle the origin (sides, top, bottom, cursor, random) and fire |
| `Ctrl+Shift+M` | Cycle the mode (confetti, emoji, mixed) and fire |

Hotkeys are written as modifiers plus a key, for example `Ctrl+Shift+C`,
`Alt+F9`, `Win+X` or `Ctrl+Alt+Space`. Modifiers are `Ctrl`, `Shift`, `Alt` and
`Win`. Keys can be a letter, a digit, `F1` to `F24`, `Space`, `Enter`, `Tab`,
`Esc`, `Insert`, `Delete`, `Home`, `End`, `PageUp`, `PageDown`, the arrow keys
(`Up`, `Down`, `Left`, `Right`), `Num0` to `Num9`, `Pause` or `PrintScreen`.
Type `none` (or leave the field empty) to turn a hotkey off. If another program
already owns a combination, it can't be registered; the mod log says so.

**Hold to fire**: while the fire hotkey (its main key and modifiers) stays held,
the mod keeps emitting at the normal rate (particle count per emission time) and
stops when you let go. A quick tap still plays one full burst.

Cycling the origin or the mode only lasts until the settings are saved again or
the mod restarts. The saved settings are not changed.

## Modes

- **Confetti**: paper pieces in the shapes and colors you pick. With `Flip`
  enabled they tumble in 3D as they fall.
- **Emoji**: the emoji from the `Emoji` list, drawn in full color.
- **Mixed**: both at once. `Emoji share` sets how many of the particles are emoji.

## Origins

- **Sides**: two cannons in the bottom corners shooting up and inward (the
  classic look). `Side cannon angle` sets how steeply they fire: 70 is classic,
  90 shoots straight up, lower values shoot flatter toward the middle.
- **Top**: confetti rain from the top edge.
- **Bottom**: a fountain from the bottom center.
- **Cursor**: an explosion from the mouse pointer. Every particle spawns at the
  pointer position at the moment it is emitted, so moving the mouse while holding
  the hotkey drags the stream along, even onto another monitor.
- **Random**: one of the above, picked per burst.

## Settings

The defaults reproduce the original classic confetti: sides origin, 600
rectangular pieces in six colors, no 3D flip, long lifetime.

Windhawk number settings are whole numbers, so decimal values are stored scaled.
Each setting's description says how: for example `Gravity` is in hundredths, so
`50` means 0.50, and `Drag` is in thousandths, so `990` means 0.990.

- **Particle count, Emission time, Speed, Gravity, Spread, Source width**: shape
  of the burst. Emission time is how long a burst keeps spawning particles.
- **Wobble, Size, Spin, Drag**: how particles move and look while falling. Drag
  is the share of velocity kept per 60 Hz tick.
- **Lifetime, Fade, Opacity**: how long particles live, whether they fade out
  over the last quarter of their life, and the overall overlay opacity.
- **Frame rate**: animation frame rate. Physics is time based, so the motion
  looks the same at any frame rate; higher is only smoother.
- **Emoji, Emoji size, Emoji spin**: the emoji list is separated by spaces.
  Joined emoji sequences (such as family or flag emoji) are kept intact.
- **Shapes, Colors**: confetti shapes (`rect`, `square`, `circle`, `triangle`,
  `star`, `heart`) and colors (`#RRGGBB`). Each list item can also
  hold several values separated by commas.
- **Preview on save**: fire one burst whenever you save the settings, handy
  while tuning.

## Tips

- Bursts land on the monitor under the pointer and on the virtual desktop you
  are on.
- Firing again while a burst is still playing on the same monitor adds to it.
- Old overlays keep animating until empty after the pointer moves to another
  monitor.
- For a snow look, use origin `Top`, shape `circle`, color `#FFFFFF`, gravity
  `5` and wobble `20`.
- Very high particle counts on 4K monitors cost more GPU and CPU time per frame.
  A single overlay is capped at 8000 live particles.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyFire: Ctrl+Shift+C
  $name: Fire hotkey
  $description: Fires a burst, and keeps firing while the keys are held. Examples - Ctrl+Shift+C, Alt+F9, Win+X. Use none to disable.
- hotkeyCycleOrigin: Ctrl+Shift+E
  $name: Cycle origin hotkey
  $description: Switches to the next origin and fires a burst. Not saved to the settings. Use none to disable.
- hotkeyCycleMode: Ctrl+Shift+M
  $name: Cycle mode hotkey
  $description: Switches between confetti, emoji and mixed and fires a burst. Not saved to the settings. Use none to disable.
- mode: confetti
  $name: Mode
  $options:
  - confetti: Confetti
  - emoji: Emoji
  - mixed: Mixed
- origin: sides
  $name: Origin
  $description: Where the burst comes from
  $options:
  - sides: Sides (bottom corners)
  - top: Top (rain)
  - bottom: Bottom (fountain)
  - cursor: Mouse pointer
  - random: Random per burst
- sideAngle: 70
  $name: Side cannon angle (degrees)
  $description: Launch angle of the left and right cannons above horizontal, 10 to 90. 70 is the classic look, 90 shoots straight up, lower values shoot flatter toward the middle.
- count: 600
  $name: Particle count
  $description: Particles per burst, 10 to 2000
- duration: 1200
  $name: Emission time (ms)
  $description: How long a burst keeps spawning particles, 100 to 6000 ms
- speed: 25
  $name: Speed
  $description: Launch speed, 5 to 40
- gravity: 50
  $name: Gravity (hundredths)
  $description: 0 to 200, where 50 means 0.50
- spread: 100
  $name: Spread (hundredths)
  $description: Sideways spread of the launch, 10 to 500, where 100 means 1.00. Higher covers more of the screen.
- density: 300
  $name: Source width (hundredths)
  $description: How wide the source and launch fan are, 10 to 300, where 300 means 3.00. Higher covers more of the screen.
- wobble: 50
  $name: Wobble (tenths)
  $description: Side to side flutter, 0 to 50, where 50 means 5.0
- size: 100
  $name: Confetti size (hundredths)
  $description: Size multiplier, 10 to 300, where 100 means 1.00
- spin: 100
  $name: Spin (hundredths)
  $description: Rotation and tumble speed multiplier, 0 to 300, where 100 means 1.00
- drag: 990
  $name: Drag (thousandths)
  $description: Share of velocity kept per 60 Hz tick, 900 to 1000, where 990 means 0.990
- lifetime: 20
  $name: Lifetime (seconds)
  $description: Maximum life of a particle, 1 to 20 seconds
- fade: true
  $name: Fade out
  $description: Fade particles over the last quarter of their life
- opacity: 100
  $name: Opacity (%)
  $description: Overall opacity of the effect, 10 to 100
- fps: 60
  $name: Frame rate
  $description: Animation frames per second, 30 to 240. Physics is time based, so this only changes smoothness.
- emojiRatio: 50
  $name: Emoji share in mixed mode (%)
  $description: 0 to 100
- emojis: "🎉 🎊 ✨ ⭐ ❤️ 🎈 🥳 🌟"
  $name: Emoji
  $description: Emoji used in emoji and mixed modes, separated by spaces. Joined sequences are kept intact. Leave empty for the built-in set.
- emojiSize: 100
  $name: Emoji size (hundredths)
  $description: 30 to 400, where 100 means about 32 pixels at 100% display scaling
- emojiSpin: true
  $name: Emoji spin
  $description: Let emoji rotate while they fly
- shapes: [rect]
  $name: Confetti shapes
  $description: Any of rect, square, circle, triangle, star, heart. One per item, or several separated by commas.
- colors: ["#A78BFA", "#F472B6", "#FB923C", "#34D399", "#60A5FA", "#FFFFFF"]
  $name: Confetti colors
  $description: Colors as #RRGGBB. One per item, or several separated by commas.
- flip: false
  $name: Flip
  $description: Confetti pieces tumble in 3D as they fall
- previewOnSave: true
  $name: Preview on save
  $description: Fire one burst whenever the settings are saved
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <d2d1.h>
#include <dwrite.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <wrl/client.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <random>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Constants and types

constexpr float kPi = 3.14159265f;
constexpr UINT kMsgSettingsChanged = WM_APP + 1;
constexpr int kHotkeyFire = 1;
constexpr int kHotkeyCycleOrigin = 2;
constexpr int kHotkeyCycleMode = 3;
constexpr size_t kMaxParticlesPerOverlay = 8000;
constexpr float kBaseEmojiPx = 32.0f;
// Emoji sprites are rasterised slightly larger than the nominal size so the
// per-particle size variation only ever scales them down.
constexpr float kEmojiSpriteOversize = 1.2f;

// CLSID_VirtualDesktopManager, defined here so no extra import library is
// needed.
constexpr GUID kClsidVirtualDesktopManager = {
    0xaa509086, 0x5ca9, 0x4c25, {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};

constexpr WCHAR kMsgWindowClass[] = L"ConfettiSpawnerMsg_" WH_MOD_ID;
constexpr WCHAR kOverlayWindowClass[] = L"ConfettiSpawnerOverlay_" WH_MOD_ID;

enum class Origin { Sides, Top, Bottom, Cursor, Random };
enum class Mode { Confetti, Emoji, Mixed };
enum class Shape : uint8_t {
    Rect,
    Square,
    Circle,
    Triangle,
    Star,
    Heart
};

struct Settings {
    std::wstring hotkeyFire;
    std::wstring hotkeyCycleOrigin;
    std::wstring hotkeyCycleMode;
    Mode mode = Mode::Confetti;
    Origin origin = Origin::Sides;
    int count = 600;
    float duration = 1.2f;  // seconds
    float speed = 25.0f;
    float gravity = 0.5f;
    float spread = 1.0f;
    float density = 3.0f;
    float sideAngle = 70.0f;
    float wobble = 5.0f;
    float size = 1.0f;
    float spin = 1.0f;
    float drag = 0.99f;
    float lifetime = 20.0f;  // seconds
    bool fade = true;
    float opacity = 1.0f;
    int fps = 60;
    float emojiRatio = 0.5f;
    std::vector<std::wstring> emojis;
    float emojiSize = 1.0f;
    bool emojiSpin = true;
    std::vector<Shape> shapes;
    std::vector<D2D1_COLOR_F> colors;
    bool flip = false;
    bool previewOnSave = true;
};

struct Particle {
    float x, y;    // position in overlay pixels
    float vx, vy;  // pixels per 60 Hz tick
    float angle;   // degrees
    float spin;    // degrees per tick
    float flipPhase, flipSpeed;
    float wobblePhase, wobbleSpeed;
    float w, h;      // confetti size in pixels
    float scale;     // emoji sprite scale
    float age, life;  // seconds
    uint16_t color;
    uint16_t emoji;
    Shape shape;
    bool isEmoji;
};

// Parameters for spawning one particle.
struct Emitter {
    Origin origin;
    Mode mode;
    float cursorX, cursorY;
};

// A running emission. Streams started by a hotkey press stay alive while the
// key is held (vk != 0); otherwise they end once `total` particles are out.
struct Stream {
    Origin origin;
    Mode mode;
    HMONITOR monitor;  // fixed target, unused for the cursor origin
    UINT mods;
    UINT vk;  // 0 when not tied to a held key
    int total;
    int emitted;
    float rate;  // particles per second
    float carry;
};

struct Overlay {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    POINT pos{};
    int width = 0;
    int height = 0;
    float scale = 1.0f;  // DPI scale of the monitor

    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;

    ComPtr<ID2D1DCRenderTarget> rt;
    std::vector<ComPtr<ID2D1SolidColorBrush>> brushes;
    std::vector<ComPtr<ID2D1Bitmap>> sprites;
    float spritePx = 0.0f;
    bool spritesReady = false;
    bool usesEmoji = false;
    unsigned resourceGen = 0;
    bool resourcesValid = false;

    std::vector<Particle> particles;

    RECT prevDirty{};
    bool fullClear = true;
    bool shown = false;
};

////////////////////////////////////////////////////////////////////////////////
// Globals (all touched only on the worker thread, except the thread handles)

HANDLE g_thread;
DWORD g_threadId;
HANDLE g_readyEvent;
bool g_initOk;
HWND g_msgWnd;
HINSTANCE g_instance;

Settings g_settings;
unsigned g_settingsGen = 1;
Origin g_activeOrigin = Origin::Sides;
Mode g_activeMode = Mode::Confetti;

ComPtr<ID2D1Factory> g_d2d;
ComPtr<IDWriteFactory> g_dwrite;
ComPtr<ID2D1PathGeometry> g_geoTriangle;
ComPtr<ID2D1PathGeometry> g_geoStar;
ComPtr<ID2D1PathGeometry> g_geoHeart;
ComPtr<IVirtualDesktopManager> g_desktopManager;

std::vector<Overlay*> g_overlays;
std::vector<Stream> g_streams;

HANDLE g_frameTimer;
bool g_highResTimer;
bool g_timePeriodActive;
bool g_animating;
LARGE_INTEGER g_qpcFreq;
LONGLONG g_lastTick;
LONGLONG g_nextFrame;

std::mt19937 g_rng;

using SetThreadDpiAwarenessContext_t =
    DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
GetDpiForWindow_t g_getDpiForWindow;

////////////////////////////////////////////////////////////////////////////////
// Small helpers

float Rand() {
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(g_rng);
}

int RandIndex(size_t n) {
    if (n <= 1) {
        return 0;
    }
    return (int)std::uniform_int_distribution<size_t>(0, n - 1)(g_rng);
}

template <typename T>
T Clamp(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

std::wstring Trim(const std::wstring& s) {
    size_t a = 0;
    size_t b = s.size();
    while (a < b && iswspace(s[a])) {
        a++;
    }
    while (b > a && iswspace(s[b - 1])) {
        b--;
    }
    return s.substr(a, b - a);
}

std::wstring Lower(std::wstring s) {
    for (auto& c : s) {
        c = (WCHAR)towlower(c);
    }
    return s;
}

std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep) {
    std::vector<std::wstring> out;
    size_t start = 0;
    while (start <= s.size()) {
        size_t end = s.find(sep, start);
        if (end == std::wstring::npos) {
            end = s.size();
        }
        std::wstring part = Trim(s.substr(start, end - start));
        if (!part.empty()) {
            out.push_back(part);
        }
        start = end + 1;
    }
    return out;
}

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

// Reads a Windhawk string array setting. Each item may itself hold several
// comma separated values.
std::vector<std::wstring> GetStringArraySetting(PCWSTR name) {
    std::vector<std::wstring> out;
    WCHAR key[64];
    for (int i = 0; i < 64; i++) {
        swprintf_s(key, L"%s[%d]", name, i);
        PCWSTR value = Wh_GetStringSetting(key);
        std::wstring item = value ? value : L"";
        Wh_FreeStringSetting(value);
        if (item.empty()) {
            break;
        }
        for (auto& part : Split(item, L',')) {
            out.push_back(part);
        }
    }
    return out;
}

bool ParseColor(const std::wstring& text, D2D1_COLOR_F* color) {
    std::wstring s = Trim(text);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() == 3) {
        s = {s[0], s[0], s[1], s[1], s[2], s[2]};
    }
    if (s.size() != 6) {
        return false;
    }
    unsigned value = 0;
    for (WCHAR c : s) {
        value <<= 4;
        if (c >= L'0' && c <= L'9') {
            value |= c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            value |= c - L'a' + 10;
        } else if (c >= L'A' && c <= L'F') {
            value |= c - L'A' + 10;
        } else {
            return false;
        }
    }
    *color = D2D1::ColorF(value, 1.0f);
    return true;
}

bool ParseShape(const std::wstring& text, Shape* shape) {
    static const struct {
        PCWSTR name;
        Shape shape;
    } kShapes[] = {
        {L"rect", Shape::Rect},         {L"rectangle", Shape::Rect},
        {L"square", Shape::Square},     {L"circle", Shape::Circle},
        {L"triangle", Shape::Triangle}, {L"star", Shape::Star},
        {L"heart", Shape::Heart},
    };
    std::wstring s = Lower(Trim(text));
    for (const auto& entry : kShapes) {
        if (s == entry.name) {
            *shape = entry.shape;
            return true;
        }
    }
    return false;
}

Origin ParseOrigin(const std::wstring& text) {
    std::wstring s = Lower(Trim(text));
    if (s == L"top") {
        return Origin::Top;
    }
    if (s == L"bottom") {
        return Origin::Bottom;
    }
    if (s == L"center") {
        return Origin::Cursor;  // center was removed; cursor replaces it
    }
    if (s == L"cursor") {
        return Origin::Cursor;
    }
    if (s == L"random") {
        return Origin::Random;
    }
    return Origin::Sides;
}

Mode ParseMode(const std::wstring& text) {
    std::wstring s = Lower(Trim(text));
    if (s == L"emoji") {
        return Mode::Emoji;
    }
    if (s == L"mixed") {
        return Mode::Mixed;
    }
    return Mode::Confetti;
}

PCWSTR OriginName(Origin origin) {
    switch (origin) {
        case Origin::Sides:
            return L"sides";
        case Origin::Top:
            return L"top";
        case Origin::Bottom:
            return L"bottom";
        case Origin::Cursor:
            return L"cursor";
        case Origin::Random:
            return L"random";
    }
    return L"sides";
}

PCWSTR ModeName(Mode mode) {
    switch (mode) {
        case Mode::Confetti:
            return L"confetti";
        case Mode::Emoji:
            return L"emoji";
        case Mode::Mixed:
            return L"mixed";
    }
    return L"confetti";
}

////////////////////////////////////////////////////////////////////////////////
// Hotkeys

// Parses strings such as "Ctrl+Shift+C", "Alt+F9" or "Win+X". Returns false
// for "none", an empty string or anything that can't be parsed.
bool ParseHotkey(const std::wstring& text, UINT* modifiers, UINT* vk) {
    static const struct {
        PCWSTR name;
        UINT vk;
    } kKeys[] = {
        {L"space", VK_SPACE},      {L"enter", VK_RETURN},
        {L"return", VK_RETURN},    {L"tab", VK_TAB},
        {L"esc", VK_ESCAPE},       {L"escape", VK_ESCAPE},
        {L"backspace", VK_BACK},   {L"insert", VK_INSERT},
        {L"ins", VK_INSERT},       {L"delete", VK_DELETE},
        {L"del", VK_DELETE},       {L"home", VK_HOME},
        {L"end", VK_END},          {L"pageup", VK_PRIOR},
        {L"pgup", VK_PRIOR},       {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT},        {L"up", VK_UP},
        {L"down", VK_DOWN},        {L"left", VK_LEFT},
        {L"right", VK_RIGHT},      {L"pause", VK_PAUSE},
        {L"printscreen", VK_SNAPSHOT}, {L"prtsc", VK_SNAPSHOT},
        {L"plus", VK_OEM_PLUS},    {L"minus", VK_OEM_MINUS},
        {L"comma", VK_OEM_COMMA},  {L"period", VK_OEM_PERIOD},
    };

    *modifiers = 0;
    *vk = 0;

    std::wstring s = Lower(Trim(text));
    if (s.empty() || s == L"none" || s == L"off" || s == L"disabled") {
        return false;
    }

    for (const auto& rawToken : Split(s, L'+')) {
        std::wstring token = Trim(rawToken);
        if (token == L"ctrl" || token == L"control") {
            *modifiers |= MOD_CONTROL;
        } else if (token == L"shift") {
            *modifiers |= MOD_SHIFT;
        } else if (token == L"alt") {
            *modifiers |= MOD_ALT;
        } else if (token == L"win" || token == L"windows") {
            *modifiers |= MOD_WIN;
        } else if (*vk) {
            return false;  // More than one non-modifier key.
        } else if (token.size() == 1 &&
                   ((token[0] >= L'a' && token[0] <= L'z') ||
                    (token[0] >= L'0' && token[0] <= L'9'))) {
            *vk = (UINT)towupper(token[0]);
        } else if (token.size() >= 2 && token[0] == L'f' &&
                   iswdigit(token[1])) {
            int n = _wtoi(token.c_str() + 1);
            if (n < 1 || n > 24) {
                return false;
            }
            *vk = VK_F1 + (UINT)(n - 1);
        } else if (token.size() == 4 && token.compare(0, 3, L"num") == 0 &&
                   iswdigit(token[3])) {
            *vk = VK_NUMPAD0 + (UINT)(token[3] - L'0');
        } else {
            bool found = false;
            for (const auto& key : kKeys) {
                if (token == key.name) {
                    *vk = key.vk;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
    }

    return *vk != 0;
}

void RegisterOneHotkey(int id, const std::wstring& text, PCWSTR label) {
    UINT modifiers;
    UINT vk;
    if (!ParseHotkey(text, &modifiers, &vk)) {
        std::wstring lower = Lower(Trim(text));
        if (!lower.empty() && lower != L"none" && lower != L"off" &&
            lower != L"disabled") {
            Wh_Log(L"Could not parse the %s hotkey: %s", label, text.c_str());
        }
        return;
    }
    if (!RegisterHotKey(g_msgWnd, id, modifiers | MOD_NOREPEAT, vk)) {
        Wh_Log(L"Could not register the %s hotkey %s (error %u), it may be in "
               L"use by another program",
               label, text.c_str(), GetLastError());
    }
}

void RegisterHotkeys() {
    UnregisterHotKey(g_msgWnd, kHotkeyFire);
    UnregisterHotKey(g_msgWnd, kHotkeyCycleOrigin);
    UnregisterHotKey(g_msgWnd, kHotkeyCycleMode);
    RegisterOneHotkey(kHotkeyFire, g_settings.hotkeyFire, L"fire");
    RegisterOneHotkey(kHotkeyCycleOrigin, g_settings.hotkeyCycleOrigin,
                      L"cycle origin");
    RegisterOneHotkey(kHotkeyCycleMode, g_settings.hotkeyCycleMode,
                      L"cycle mode");
}

////////////////////////////////////////////////////////////////////////////////
// Settings

void LoadSettings() {
    Settings s;

    s.hotkeyFire = GetStringSetting(L"hotkeyFire");
    s.hotkeyCycleOrigin = GetStringSetting(L"hotkeyCycleOrigin");
    s.hotkeyCycleMode = GetStringSetting(L"hotkeyCycleMode");
    s.mode = ParseMode(GetStringSetting(L"mode"));
    s.origin = ParseOrigin(GetStringSetting(L"origin"));

    s.count = Clamp(Wh_GetIntSetting(L"count"), 10, 2000);
    s.duration = Clamp(Wh_GetIntSetting(L"duration"), 100, 6000) / 1000.0f;
    s.speed = (float)Clamp(Wh_GetIntSetting(L"speed"), 5, 40);
    s.gravity = Clamp(Wh_GetIntSetting(L"gravity"), 0, 200) / 100.0f;
    s.spread = Clamp(Wh_GetIntSetting(L"spread"), 10, 500) / 100.0f;
    s.density = Clamp(Wh_GetIntSetting(L"density"), 10, 300) / 100.0f;
    s.sideAngle = (float)Clamp(Wh_GetIntSetting(L"sideAngle"), 10, 90);
    s.wobble = Clamp(Wh_GetIntSetting(L"wobble"), 0, 50) / 10.0f;
    s.size = Clamp(Wh_GetIntSetting(L"size"), 10, 300) / 100.0f;
    s.spin = Clamp(Wh_GetIntSetting(L"spin"), 0, 300) / 100.0f;
    s.drag = Clamp(Wh_GetIntSetting(L"drag"), 900, 1000) / 1000.0f;
    s.lifetime = (float)Clamp(Wh_GetIntSetting(L"lifetime"), 1, 20);
    s.fade = Wh_GetIntSetting(L"fade") != 0;
    s.opacity = Clamp(Wh_GetIntSetting(L"opacity"), 10, 100) / 100.0f;
    s.fps = Clamp(Wh_GetIntSetting(L"fps"), 30, 240);
    s.emojiRatio = Clamp(Wh_GetIntSetting(L"emojiRatio"), 0, 100) / 100.0f;
    s.emojiSize = Clamp(Wh_GetIntSetting(L"emojiSize"), 30, 400) / 100.0f;
    s.emojiSpin = Wh_GetIntSetting(L"emojiSpin") != 0;
    s.flip = Wh_GetIntSetting(L"flip") != 0;
    s.previewOnSave = Wh_GetIntSetting(L"previewOnSave") != 0;

    // Emoji: split on whitespace only, so ZWJ sequences, skin tone modifiers
    // and variation selectors stay attached to their base character.
    std::wstring emojiText = GetStringSetting(L"emojis");
    std::wstring current;
    for (WCHAR c : emojiText) {
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' ||
            c == 0x3000) {
            if (!current.empty()) {
                s.emojis.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        s.emojis.push_back(current);
    }
    if (s.emojis.size() > 256) {
        s.emojis.resize(256);
    }
    if (s.emojis.empty()) {
        s.emojis = {
            L"\U0001F389",  // party popper
            L"\U0001F38A",  // confetti ball
            L"\x2728",        // sparkles
            L"\x2B50",        // star
            L"\x2764\xFE0F",  // red heart
            L"\U0001F388",  // balloon
            L"\U0001F973",  // partying face
            L"\U0001F31F",  // glowing star
        };
    }

    for (const auto& item : GetStringArraySetting(L"shapes")) {
        Shape shape;
        if (ParseShape(item, &shape)) {
            s.shapes.push_back(shape);
        } else {
            Wh_Log(L"Unknown shape: %s", item.c_str());
        }
    }
    if (s.shapes.empty()) {
        s.shapes = {Shape::Rect};
    }

    for (const auto& item : GetStringArraySetting(L"colors")) {
        D2D1_COLOR_F color;
        if (ParseColor(item, &color)) {
            s.colors.push_back(color);
        } else {
            Wh_Log(L"Unknown color: %s", item.c_str());
        }
    }
    if (s.colors.size() > 256) {
        s.colors.resize(256);
    }
    if (s.colors.empty()) {
        for (unsigned rgb : {0xA78BFAu, 0xF472B6u, 0xFB923Cu, 0x34D399u,
                             0x60A5FAu, 0xFFFFFFu}) {
            s.colors.push_back(D2D1::ColorF(rgb, 1.0f));
        }
    }

    g_settings = std::move(s);
    g_activeOrigin = g_settings.origin;
    g_activeMode = g_settings.mode;
    g_settingsGen++;
}

////////////////////////////////////////////////////////////////////////////////
// Shared Direct2D resources (device independent, created once)

HRESULT CreateFilledGeometry(const D2D1_POINT_2F* points,
                             UINT count,
                             ComPtr<ID2D1PathGeometry>* out) {
    ComPtr<ID2D1PathGeometry> geo;
    HRESULT hr = g_d2d->CreatePathGeometry(&geo);
    if (FAILED(hr)) {
        return hr;
    }
    ComPtr<ID2D1GeometrySink> sink;
    hr = geo->Open(&sink);
    if (FAILED(hr)) {
        return hr;
    }
    sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLines(points + 1, count - 1);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *out = geo;
    }
    return hr;
}

// All shapes are defined in a unit box centred on the origin and scaled by
// the particle transform.
bool CreateGeometries() {
    const D2D1_POINT_2F triangle[] = {
        {0.0f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};
    if (FAILED(CreateFilledGeometry(triangle, ARRAYSIZE(triangle),
                                    &g_geoTriangle))) {
        return false;
    }

    D2D1_POINT_2F star[10];
    for (int i = 0; i < 10; i++) {
        float r = (i % 2 == 0) ? 0.5f : 0.2f;
        float a = -kPi / 2 + i * kPi / 5;
        star[i] = {r * cosf(a), r * sinf(a)};
    }
    if (FAILED(CreateFilledGeometry(star, ARRAYSIZE(star), &g_geoStar))) {
        return false;
    }

    ComPtr<ID2D1PathGeometry> heart;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreatePathGeometry(&heart)) ||
        FAILED(heart->Open(&sink))) {
        return false;
    }
    sink->BeginFigure({0.0f, 0.5f}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddBezier({{-0.5f, 0.1f}, {-0.6f, -0.35f}, {-0.25f, -0.45f}});
    sink->AddBezier({{-0.1f, -0.5f}, {0.0f, -0.38f}, {0.0f, -0.28f}});
    sink->AddBezier({{0.0f, -0.38f}, {0.1f, -0.5f}, {0.25f, -0.45f}});
    sink->AddBezier({{0.6f, -0.35f}, {0.5f, 0.1f}, {0.0f, 0.5f}});
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    if (FAILED(sink->Close())) {
        return false;
    }
    g_geoHeart = heart;
    return true;
}

bool IsOnCurrentDesktop(HWND hwnd) {
    if (!g_desktopManager) {
        return true;
    }
    BOOL onCurrent = TRUE;
    if (FAILED(g_desktopManager->IsWindowOnCurrentVirtualDesktop(
            hwnd, &onCurrent))) {
        return true;
    }
    return onCurrent != FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Frame timer

void ScheduleNextFrame() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    LONGLONG frameTicks = g_qpcFreq.QuadPart / g_settings.fps;
    g_nextFrame += frameTicks;
    if (g_nextFrame <= now.QuadPart) {
        // Fell behind, don't try to catch up.
        g_nextFrame = now.QuadPart + frameTicks;
    }
    LONGLONG wait100ns =
        (g_nextFrame - now.QuadPart) * 10000000 / g_qpcFreq.QuadPart;
    LARGE_INTEGER due;
    due.QuadPart = -std::max<LONGLONG>(wait100ns, 1);
    SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE);
}

void StartAnimation() {
    if (g_animating) {
        return;
    }
    g_animating = true;
    if (!g_highResTimer && !g_timePeriodActive) {
        timeBeginPeriod(1);
        g_timePeriodActive = true;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    g_lastTick = now.QuadPart;
    g_nextFrame = now.QuadPart;
    LARGE_INTEGER due;
    due.QuadPart = -1;
    SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE);
}

void StopAnimation() {
    if (!g_animating) {
        return;
    }
    g_animating = false;
    CancelWaitableTimer(g_frameTimer);
    if (g_timePeriodActive) {
        timeEndPeriod(1);
        g_timePeriodActive = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Overlay windows

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void ReleaseDeviceResources(Overlay* o) {
    o->sprites.clear();
    o->brushes.clear();
    o->rt.Reset();
    o->spritesReady = false;
    o->resourcesValid = false;
}

void DestroyOverlay(Overlay* o) {
    ReleaseDeviceResources(o);
    if (o->memDC) {
        if (o->oldBitmap) {
            SelectObject(o->memDC, o->oldBitmap);
        }
        DeleteDC(o->memDC);
    }
    if (o->dib) {
        DeleteObject(o->dib);
    }
    if (o->hwnd) {
        DestroyWindow(o->hwnd);
    }
    delete o;
}

Overlay* CreateOverlay(HMONITOR monitor) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(monitor, &mi)) {
        return nullptr;
    }

    auto* o = new Overlay();
    o->monitor = monitor;
    o->pos = {mi.rcMonitor.left, mi.rcMonitor.top};
    o->width = mi.rcMonitor.right - mi.rcMonitor.left;
    o->height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    o->hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                 WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                 WS_EX_TOPMOST,
                             kOverlayWindowClass, L"Confetti Spawner",
                             WS_POPUP, o->pos.x, o->pos.y, o->width, o->height,
                             nullptr, nullptr, g_instance, nullptr);
    if (!o->hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        delete o;
        return nullptr;
    }

    UINT dpi = g_getDpiForWindow ? g_getDpiForWindow(o->hwnd) : 0;
    if (!dpi) {
        HDC screen = GetDC(nullptr);
        dpi = (UINT)GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(nullptr, screen);
    }
    o->scale = Clamp(dpi / 96.0f, 0.5f, 6.0f);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = o->width;
    bmi.bmiHeader.biHeight = -o->height;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    o->memDC = CreateCompatibleDC(nullptr);
    o->dib = o->memDC ? CreateDIBSection(o->memDC, &bmi, DIB_RGB_COLORS,
                                         &bits, nullptr, 0)
                      : nullptr;
    if (!o->dib) {
        Wh_Log(L"Could not create the overlay bitmap");
        DestroyOverlay(o);
        return nullptr;
    }
    o->oldBitmap = SelectObject(o->memDC, o->dib);
    return o;
}

bool BuildSprites(Overlay* o) {
    o->sprites.clear();
    o->spritesReady = true;

    float fontPx =
        kBaseEmojiPx * g_settings.emojiSize * o->scale * kEmojiSpriteOversize;
    o->spritePx = ceilf(fontPx * 1.3f);

    ComPtr<IDWriteTextFormat> format;
    HRESULT hr = g_dwrite->CreateTextFormat(
        L"Segoe UI Emoji", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontPx, L"",
        &format);
    if (FAILED(hr)) {
        Wh_Log(L"CreateTextFormat failed: 0x%08X", (unsigned)hr);
        return true;
    }
    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    D2D1_SIZE_F size = D2D1::SizeF(o->spritePx, o->spritePx);
    for (const auto& emoji : g_settings.emojis) {
        ComPtr<ID2D1Bitmap> bitmap;
        ComPtr<ID2D1BitmapRenderTarget> brt;
        if (SUCCEEDED(o->rt->CreateCompatibleRenderTarget(size, &brt))) {
            ComPtr<ID2D1SolidColorBrush> brush;
            brt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
                                       &brush);
            brt->BeginDraw();
            brt->Clear(D2D1::ColorF(0, 0.0f));
            if (brush) {
                brt->DrawText(emoji.c_str(), (UINT32)emoji.size(),
                              format.Get(),
                              D2D1::RectF(0, 0, size.width, size.height),
                              brush.Get(),
                              D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
            hr = brt->EndDraw();
            if (hr == D2DERR_RECREATE_TARGET) {
                return false;
            }
            if (SUCCEEDED(hr)) {
                brt->GetBitmap(&bitmap);
            }
        }
        o->sprites.push_back(bitmap);
    }
    return true;
}

// Returns false if the render target couldn't be (re)created.
bool EnsureDeviceResources(Overlay* o) {
    if (!o->rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);
        HRESULT hr = g_d2d->CreateDCRenderTarget(&props, &o->rt);
        if (FAILED(hr)) {
            Wh_Log(L"CreateDCRenderTarget failed: 0x%08X", (unsigned)hr);
            return false;
        }
        RECT full{0, 0, o->width, o->height};
        hr = o->rt->BindDC(o->memDC, &full);
        if (FAILED(hr)) {
            Wh_Log(L"BindDC failed: 0x%08X", (unsigned)hr);
            o->rt.Reset();
            return false;
        }
        o->rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        o->resourcesValid = false;
        o->fullClear = true;
    }

    if (!o->resourcesValid || o->resourceGen != g_settingsGen) {
        o->brushes.clear();
        for (const auto& color : g_settings.colors) {
            ComPtr<ID2D1SolidColorBrush> brush;
            o->rt->CreateSolidColorBrush(color, &brush);
            o->brushes.push_back(brush);
        }
        o->sprites.clear();
        o->spritesReady = false;
        o->resourceGen = g_settingsGen;
        o->resourcesValid = true;
    }

    if (o->usesEmoji && !o->spritesReady) {
        if (!BuildSprites(o)) {
            ReleaseDeviceResources(o);
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Particles

void SpawnParticle(Overlay* o, const Emitter& e) {
    const Settings& st = g_settings;
    const float s = o->scale;
    const float sw = (float)o->width;
    const float sh = (float)o->height;

    Particle p{};
    p.isEmoji = e.mode == Mode::Emoji ||
                (e.mode == Mode::Mixed && Rand() < st.emojiRatio);
    if (st.emojis.empty()) {
        p.isEmoji = false;
    }

    float angle;
    float magnitude;
    switch (e.origin) {
        case Origin::Top:
            p.x = sw / 2 + (Rand() - 0.5f) * sw * st.density;
            p.y = -50 * s;
            p.vx = (Rand() * 10 - 5) * st.spread * s;
            p.vy = (Rand() * 5 + 3) * s;
            break;

        case Origin::Bottom:
            p.x = Clamp(sw / 2 + (Rand() - 0.5f) * sw * st.density,
                        -0.1f * sw, 1.1f * sw);
            p.y = sh + 20 * s;
            angle = -kPi / 2 +
                    (Rand() * 2 - 1) * (0.35f + 0.30f * st.density);
            magnitude = st.speed * (0.8f + Rand() * 0.7f) * s;
            p.vx = cosf(angle) * magnitude;
            p.vy = sinf(angle) * magnitude;
            break;

        case Origin::Cursor: {
            float jitter = 20 * s * st.density;
            p.x = e.cursorX + (Rand() - 0.5f) * jitter;
            p.y = e.cursorY + (Rand() - 0.5f) * jitter;
            angle = Rand() * 2 * kPi;
            magnitude = st.speed * (0.3f + Rand() * 1.1f) * s;
            p.vx = cosf(angle) * magnitude * st.spread;
            p.vy = sinf(angle) * magnitude - st.speed * 0.2f * s;
            break;
        }

        case Origin::Sides:
        case Origin::Random: {
            bool left = Rand() < 0.5f;
            p.x = left ? -20 * s : sw + 20 * s;
            p.y = sh + 20 * s;
            angle = (left ? -st.sideAngle : -(180.0f - st.sideAngle)) * kPi / 180 +
                    (Rand() - 0.5f) * 0.4f * st.density;
            magnitude = (Rand() * st.speed * 0.7f + st.speed * 0.9f) * s;
            p.vx = cosf(angle) * magnitude * st.spread;
            p.vy = sinf(angle) * magnitude;
            break;
        }
    }

    p.life = st.lifetime * (0.7f + 0.3f * Rand());
    p.wobblePhase = Rand() * 2 * kPi;
    p.wobbleSpeed = 0.1f + 0.1f * Rand();

    if (p.isEmoji) {
        p.emoji = (uint16_t)RandIndex(st.emojis.size());
        p.scale = (1.0f / kEmojiSpriteOversize) * (0.8f + 0.4f * Rand());
        if (st.emojiSpin) {
            p.angle = Rand() * 40 - 20;
            p.spin = (Rand() * 8 - 4) * st.spin;
        }
        o->usesEmoji = true;
    } else {
        p.shape = st.shapes[RandIndex(st.shapes.size())];
        p.color = (uint16_t)RandIndex(st.colors.size());
        float base = (8 + Rand() * 12) * st.size * s;
        switch (p.shape) {
            case Shape::Rect:
                p.w = base;
                p.h = base * 0.6f;
                break;
            case Shape::Square:
                p.w = p.h = base * 0.75f;
                break;
            case Shape::Circle:
                p.w = p.h = base * 0.7f;
                break;
            case Shape::Triangle:
                p.w = p.h = base * 0.85f;
                break;
            case Shape::Star:
                p.w = p.h = base;
                break;
            case Shape::Heart:
                p.w = p.h = base * 0.9f;
                break;
        }
        p.angle = Rand() * 360;
        p.spin = (Rand() * 30 - 15) * st.spin;
        p.flipPhase = Rand() * 2 * kPi;
        p.flipSpeed = (0.08f + Rand() * 0.22f) * st.spin;
    }

    o->particles.push_back(p);
}

// Advances one overlay by dt seconds. Returns the bounding box of all live
// particles in overlay coordinates (empty if none).
RECT StepOverlay(Overlay* o, float dt) {
    const Settings& st = g_settings;
    const float s = o->scale;
    const float k = dt * 60.0f;  // 60 Hz ticks elapsed
    const float dragK = powf(st.drag, k);
    const float sw = (float)o->width;
    const float sh = (float)o->height;

    float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
    std::erase_if(o->particles, [&](Particle& p) {
        p.age += dt;
        if (p.age >= p.life) {
            return true;
        }
        p.vy += st.gravity * s * k;
        p.vx *= dragK;
        p.vy *= dragK;
        p.wobblePhase += p.wobbleSpeed * k;
        p.x += (p.vx + sinf(p.wobblePhase) * st.wobble * s) * k;
        p.y += p.vy * k;
        p.angle = fmodf(p.angle + p.spin * k, 360.0f);
        p.flipPhase = fmodf(p.flipPhase + p.flipSpeed * k, 2 * kPi);

        if (p.y >= sh + 100 * s || (p.y < -500 * s && p.vy < 0) ||
            p.x < -sw * 2.0f || p.x > sw * 3.0f) {
            return true;
        }

        float r = p.isEmoji ? o->spritePx * p.scale * 0.75f
                            : std::max(p.w, p.h) * 0.75f;
        if (p.isEmoji && r <= 0) {
            // Sprites not built yet, use the expected size.
            r = kBaseEmojiPx * st.emojiSize * s * 1.3f;
        }
        r += 2;
        minX = std::min(minX, p.x - r);
        minY = std::min(minY, p.y - r);
        maxX = std::max(maxX, p.x + r);
        maxY = std::max(maxY, p.y + r);
        return false;
    });

    RECT bounds{};
    if (maxX > minX) {
        bounds.left = Clamp((LONG)floorf(minX), 0L, (LONG)o->width);
        bounds.top = Clamp((LONG)floorf(minY), 0L, (LONG)o->height);
        bounds.right = Clamp((LONG)ceilf(maxX), 0L, (LONG)o->width);
        bounds.bottom = Clamp((LONG)ceilf(maxY), 0L, (LONG)o->height);
    }
    return bounds;
}

void DrawParticles(Overlay* o) {
    const Settings& st = g_settings;
    ID2D1DCRenderTarget* rt = o->rt.Get();
    const D2D1_RECT_F unitRect = D2D1::RectF(-0.5f, -0.5f, 0.5f, 0.5f);
    const D2D1_ELLIPSE unitEllipse =
        D2D1::Ellipse(D2D1::Point2F(0, 0), 0.5f, 0.5f);

    for (const auto& p : o->particles) {
        float alpha = 1.0f;
        if (st.fade) {
            alpha = Clamp((p.life - p.age) / (0.25f * p.life), 0.0f, 1.0f);
        }
        if (alpha <= 0.0f) {
            continue;
        }

        if (p.isEmoji) {
            if (o->sprites.empty()) {
                continue;
            }
            ID2D1Bitmap* bitmap = o->sprites[p.emoji % o->sprites.size()].Get();
            if (!bitmap) {
                continue;
            }
            float half = o->spritePx * p.scale / 2;
            rt->SetTransform(D2D1::Matrix3x2F::Rotation(p.angle) *
                             D2D1::Matrix3x2F::Translation(p.x, p.y));
            rt->DrawBitmap(bitmap, D2D1::RectF(-half, -half, half, half),
                           alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            continue;
        }

        if (o->brushes.empty()) {
            continue;
        }
        ID2D1SolidColorBrush* brush =
            o->brushes[p.color % o->brushes.size()].Get();
        if (!brush) {
            continue;
        }
        float sx = p.w;
        if (st.flip) {
            sx *= cosf(p.flipPhase);
            if (fabsf(sx) < 0.05f) {
                continue;  // Edge-on, nothing visible.
            }
        }
        brush->SetOpacity(alpha);
        rt->SetTransform(D2D1::Matrix3x2F::Scale(sx, p.h) *
                         D2D1::Matrix3x2F::Rotation(p.angle) *
                         D2D1::Matrix3x2F::Translation(p.x, p.y));
        switch (p.shape) {
            case Shape::Rect:
            case Shape::Square:
                rt->FillRectangle(unitRect, brush);
                break;
            case Shape::Circle:
                rt->FillEllipse(unitEllipse, brush);
                break;
            case Shape::Triangle:
                rt->FillGeometry(g_geoTriangle.Get(), brush);
                break;
            case Shape::Star:
                rt->FillGeometry(g_geoStar.Get(), brush);
                break;
            case Shape::Heart:
                rt->FillGeometry(g_geoHeart.Get(), brush);
                break;
        }
    }
}

void RenderOverlay(Overlay* o, RECT current) {
    if (!EnsureDeviceResources(o)) {
        return;
    }

    RECT dirty;
    if (o->fullClear) {
        dirty = {0, 0, o->width, o->height};
    } else if (!UnionRect(&dirty, &o->prevDirty, &current)) {
        return;  // Nothing drawn before and nothing to draw now.
    }

    ID2D1DCRenderTarget* rt = o->rt.Get();
    rt->BeginDraw();
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->PushAxisAlignedClip(
        D2D1::RectF((float)dirty.left, (float)dirty.top, (float)dirty.right,
                    (float)dirty.bottom),
        D2D1_ANTIALIAS_MODE_ALIASED);
    rt->Clear(D2D1::ColorF(0, 0.0f));
    DrawParticles(o);
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->PopAxisAlignedClip();
    HRESULT hr = rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        Wh_Log(L"Render target lost, recreating");
        ReleaseDeviceResources(o);
        o->fullClear = true;
        return;
    }
    if (FAILED(hr)) {
        Wh_Log(L"EndDraw failed: 0x%08X", (unsigned)hr);
        return;
    }

    POINT srcPos{0, 0};
    SIZE size{o->width, o->height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0,
                        (BYTE)Clamp((int)(g_settings.opacity * 255 + 0.5f), 0,
                                    255),
                        AC_SRC_ALPHA};
    UPDATELAYEREDWINDOWINFO info{};
    info.cbSize = sizeof(info);
    info.pptDst = &o->pos;
    info.psize = &size;
    info.hdcSrc = o->memDC;
    info.pptSrc = &srcPos;
    info.pblend = &blend;
    info.dwFlags = ULW_ALPHA;
    // The first push must cover the whole window.
    info.prcDirty = o->shown ? &dirty : nullptr;
    if (!UpdateLayeredWindowIndirect(o->hwnd, &info)) {
        Wh_Log(L"UpdateLayeredWindowIndirect failed: %u", GetLastError());
    }

    if (!o->shown) {
        ShowWindow(o->hwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(o->hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        o->shown = true;
    }

    o->prevDirty = current;
    o->fullClear = false;
}

// Finds (or creates) the overlay for a monitor on the current desktop.
Overlay* GetOverlay(HMONITOR monitor) {
    for (Overlay* o : g_overlays) {
        if (o->monitor == monitor && IsOnCurrentDesktop(o->hwnd)) {
            return o;
        }
    }
    Overlay* overlay = CreateOverlay(monitor);
    if (overlay) {
        g_overlays.push_back(overlay);
    }
    return overlay;
}

bool IsKeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool IsStreamHeld(const Stream& s) {
    if (!s.vk || !IsKeyDown((int)s.vk)) {
        return false;
    }
    if ((s.mods & MOD_CONTROL) && !IsKeyDown(VK_CONTROL)) {
        return false;
    }
    if ((s.mods & MOD_SHIFT) && !IsKeyDown(VK_SHIFT)) {
        return false;
    }
    if ((s.mods & MOD_ALT) && !IsKeyDown(VK_MENU)) {
        return false;
    }
    if ((s.mods & MOD_WIN) && !IsKeyDown(VK_LWIN) && !IsKeyDown(VK_RWIN)) {
        return false;
    }
    return true;
}

// Emits this frame's share of every stream.
void UpdateStreams(float dt) {
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        cursor = {0, 0};
    }
    for (size_t i = 0; i < g_streams.size();) {
        Stream& s = g_streams[i];
        if (s.vk && !IsStreamHeld(s)) {
            s.vk = 0;
        }

        HMONITOR monitor =
            s.origin == Origin::Cursor
                ? MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY)
                : s.monitor;
        Overlay* o = GetOverlay(monitor);
        if (!o) {
            g_streams.erase(g_streams.begin() + i);
            continue;
        }

        float n = s.carry + s.rate * dt;
        int spawn = (int)n;
        s.carry = n - spawn;
        if (!s.vk) {
            spawn = std::min(spawn, s.total - s.emitted);
        }

        Emitter e{};
        e.origin = s.origin;
        e.mode = s.mode;
        e.cursorX = (float)(cursor.x - o->pos.x);
        e.cursorY = (float)(cursor.y - o->pos.y);
        for (int k = 0; k < spawn; k++) {
            if (o->particles.size() >= kMaxParticlesPerOverlay) {
                break;
            }
            SpawnParticle(o, e);
        }
        s.emitted += spawn;

        if (!s.vk && s.emitted >= s.total) {
            g_streams.erase(g_streams.begin() + i);
            continue;
        }
        i++;
    }
}

void Tick() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = (float)(now.QuadPart - g_lastTick) / (float)g_qpcFreq.QuadPart;
    g_lastTick = now.QuadPart;
    dt = Clamp(dt, 0.0f, 0.1f);

    UpdateStreams(dt);

    for (size_t i = 0; i < g_overlays.size();) {
        Overlay* o = g_overlays[i];
        RECT bounds = StepOverlay(o, dt);
        if (o->particles.empty() && g_streams.empty()) {
            DestroyOverlay(o);
            g_overlays.erase(g_overlays.begin() + i);
            continue;
        }
        RenderOverlay(o, bounds);
        i++;
    }

    if (g_overlays.empty()) {
        StopAnimation();
    } else {
        ScheduleNextFrame();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Bursts

// Starts an emission. With a key (vk != 0) it continues while that key is held
// and always plays at least one full burst.
void StartStream(UINT mods, UINT vk) {
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        cursor = {0, 0};
    }
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);
    if (!GetOverlay(monitor)) {
        return;
    }

    Origin origin = g_activeOrigin;
    if (origin == Origin::Random) {
        static const Origin kChoices[] = {Origin::Sides, Origin::Top,
                                          Origin::Bottom,
                                          Origin::Cursor};
        origin = kChoices[RandIndex(ARRAYSIZE(kChoices))];
    }

    Stream s{};
    s.origin = origin;
    s.mode = g_activeMode;
    s.monitor = monitor;
    s.mods = mods;
    s.vk = vk;
    s.total = g_settings.count;
    s.rate = g_settings.count / g_settings.duration;
    s.carry = 1.0f;  // Start the burst on the first frame.
    g_streams.push_back(s);

    StartAnimation();
}

void Fire() {
    StartStream(0, 0);
}

// The fire hotkey: keeps emitting while its keys stay held.
void FireHotkey() {
    UINT mods = 0;
    UINT vk = 0;
    if (!ParseHotkey(g_settings.hotkeyFire, &mods, &vk)) {
        vk = 0;
    }
    for (Stream& s : g_streams) {
        if (s.vk) {
            if (IsStreamHeld(s)) {
                return;  // Still held, ignore.
            }
            s.vk = 0;
        }
    }
    StartStream(mods, vk);
}

void CycleOrigin() {
    static const Origin kOrder[] = {Origin::Sides,  Origin::Top,
                                    Origin::Bottom,
                                    Origin::Cursor, Origin::Random};
    size_t index = 0;
    for (size_t i = 0; i < ARRAYSIZE(kOrder); i++) {
        if (kOrder[i] == g_activeOrigin) {
            index = i;
        }
    }
    g_activeOrigin = kOrder[(index + 1) % ARRAYSIZE(kOrder)];
    Wh_Log(L"Origin: %s", OriginName(g_activeOrigin));
    Fire();
}

void CycleMode() {
    switch (g_activeMode) {
        case Mode::Confetti:
            g_activeMode = Mode::Emoji;
            break;
        case Mode::Emoji:
            g_activeMode = Mode::Mixed;
            break;
        case Mode::Mixed:
            g_activeMode = Mode::Confetti;
            break;
    }
    Wh_Log(L"Mode: %s", ModeName(g_activeMode));
    Fire();
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_HOTKEY:
            switch ((int)wp) {
                case kHotkeyFire:
                    FireHotkey();
                    break;
                case kHotkeyCycleOrigin:
                    CycleOrigin();
                    break;
                case kHotkeyCycleMode:
                    CycleMode();
                    break;
            }
            return 0;

        case kMsgSettingsChanged:
            LoadSettings();
            RegisterHotkeys();
            if (g_settings.previewOnSave) {
                Fire();
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

bool WorkerInit() {
    auto setThreadDpiAwarenessContext =
        (SetThreadDpiAwarenessContext_t)GetProcAddress(
            GetModuleHandle(L"user32.dll"), "SetThreadDpiAwarenessContext");
    if (setThreadDpiAwarenessContext) {
        setThreadDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    g_getDpiForWindow = (GetDpiForWindow_t)GetProcAddress(
        GetModuleHandle(L"user32.dll"), "GetDpiForWindow");

    QueryPerformanceFrequency(&g_qpcFreq);
    g_rng.seed((unsigned)GetTickCount64() ^ GetCurrentThreadId());

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                   g_d2d.GetAddressOf());
    if (FAILED(hr)) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08X", (unsigned)hr);
        return false;
    }
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(g_dwrite.GetAddressOf()));
    if (FAILED(hr)) {
        Wh_Log(L"DWriteCreateFactory failed: 0x%08X", (unsigned)hr);
        return false;
    }
    if (!CreateGeometries()) {
        Wh_Log(L"Could not create the confetti shapes");
        return false;
    }

    // Optional: lets a reused overlay be skipped after a virtual desktop
    // switch. Without it every burst simply reuses the overlay.
    CoCreateInstance(kClsidVirtualDesktopManager, nullptr, CLSCTX_ALL,
                     IID_PPV_ARGS(&g_desktopManager));

    g_frameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_ALL_ACCESS);
    g_highResTimer = g_frameTimer != nullptr;
    if (!g_frameTimer) {
        g_frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }
    if (!g_frameTimer) {
        Wh_Log(L"Could not create the frame timer: %u", GetLastError());
        return false;
    }

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = g_instance;
    wc.lpszClassName = kMsgWindowClass;
    RegisterClassEx(&wc);

    WNDCLASSEX overlayClass{};
    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.lpfnWndProc = OverlayWndProc;
    overlayClass.hInstance = g_instance;
    overlayClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    overlayClass.lpszClassName = kOverlayWindowClass;
    RegisterClassEx(&overlayClass);

    g_msgWnd = CreateWindowEx(0, kMsgWindowClass, L"", 0, 0, 0, 0, 0,
                              HWND_MESSAGE, nullptr, g_instance, nullptr);
    if (!g_msgWnd) {
        Wh_Log(L"Could not create the message window: %u", GetLastError());
        return false;
    }

    LoadSettings();
    RegisterHotkeys();
    return true;
}

void WorkerCleanup() {
    StopAnimation();
    g_streams.clear();
    for (Overlay* o : g_overlays) {
        DestroyOverlay(o);
    }
    g_overlays.clear();

    if (g_msgWnd) {
        UnregisterHotKey(g_msgWnd, kHotkeyFire);
        UnregisterHotKey(g_msgWnd, kHotkeyCycleOrigin);
        UnregisterHotKey(g_msgWnd, kHotkeyCycleMode);
        DestroyWindow(g_msgWnd);
        g_msgWnd = nullptr;
    }
    UnregisterClass(kMsgWindowClass, g_instance);
    UnregisterClass(kOverlayWindowClass, g_instance);

    if (g_frameTimer) {
        CloseHandle(g_frameTimer);
        g_frameTimer = nullptr;
    }

    g_desktopManager.Reset();
    g_geoTriangle.Reset();
    g_geoStar.Reset();
    g_geoHeart.Reset();
    g_dwrite.Reset();
    g_d2d.Reset();
}

DWORD WINAPI WorkerThread(LPVOID) {
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    g_initOk = WorkerInit();
    SetEvent(g_readyEvent);

    if (g_initOk) {
        bool quit = false;
        while (!quit) {
            HANDLE handles[] = {g_frameTimer};
            DWORD handleCount = g_animating ? 1 : 0;
            DWORD result = MsgWaitForMultipleObjectsEx(
                handleCount, handles, INFINITE, QS_ALLINPUT,
                MWMO_INPUTAVAILABLE);
            if (handleCount && result == WAIT_OBJECT_0) {
                Tick();
            }

            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    WorkerCleanup();
    if (SUCCEEDED(coHr)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&WorkerThread), &g_instance);

    g_readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (!g_initOk) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Settings are reloaded on the worker thread, which owns all state.
    if (g_msgWnd) {
        PostMessage(g_msgWnd, kMsgSettingsChanged, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_thread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_thread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"The worker thread did not exit in time");
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
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
