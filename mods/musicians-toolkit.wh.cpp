// ==WindhawkMod==
// @id              musicians-toolkit
// @name            Musician's Toolkit
// @description     An all-in-one toolkit flyout for musicians, containing a chord library, a metronome, a tuner, a tap tempo calculator, a strumming pattern builder, an interactive circle of fifths and a timed note trainer for guitar, ukulele and piano.
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lole32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Musician's Toolkit

![Sample Screenshot](https://raw.githubusercontent.com/DeepakBalaraman/musicians-toolkit/master/assets/screenshot.png)

A chord library, a tuner, a metronome, a strumming trainer, a circle of
fifths, a tap tempo and a note drill, for guitar, ukulele and piano. All of
it lives in one small panel that stays on top of your other windows, so you
can keep it beside your DAW, tab app or video lesson.

Press **Win+Y** to open it and pick a tool from the menu in the top left.
**Ctrl+Tab** cycles through them. Close it with the **x**, with **Esc**, or
with the shortcut again (the shortcut itself is under Settings > Shortcut).
If you would rather it close whenever you click elsewhere, turn that on under
Settings > Placement.

## Chords

Start typing and the chord appears as you go: the list narrows with every
letter, and the one you settle on is drawn underneath.

- **Guitar and ukulele** get a fretboard, with barre positions, open strings
  and muted strings all marked.
- **Piano** lights up the keys you would hold.
- **Fingerings** - the arrow keys, or the slider under the diagram, step
  through the other ways to play the same chord.
- **Hear it** - the speaker on the card plays the chord, strummed on an
  acoustic guitar or played on a piano.

## Tap Tempo

Tap the big circle with the mouse, a touchpad or **Space**, and the panel
works the tempo out from the timing. The bars beneath it show how steady you
are being, so you can watch yourself settle.

- **Half** and **Double** jump to half time or double time.
- **Reset** clears the taps and starts a fresh count.
- The scroll wheel nudges the BPM a step at a time.

## Metronome

Set a meter and a tempo, and you have a click that keeps time while you play.

- **Meters** - the chips cover 4/4, 3/4, 2/4, 5/4, 6/8, 7/8, 9/8 and 12/8;
  the steppers reach the rest, such as 5/8, 11/8 or 7/16.
- **Sound** - a bright click marks the downbeat, a lower one each beat and a
  soft tick the subdivisions, so the bar reads as a pulse.
- **Tempo** - the arrows or the wheel step the BPM, or click the number (or
  just start typing) and enter an exact one. **Enter** sets it, **Esc**
  leaves it as it was.
- **Background** - the metronome keeps playing after you close the panel,
  which is what you want mid-piece. Switch it off under Settings > Metronome.

## Tuner

Play a note and the needle tells you where you are. The display names the
note and the arc shows how far off it is, so you can watch it settle in.

- **Tunings** - standard, drop and open guitar tunings, several basses,
  ukuleles, mandolins, banjos, the violin family, lap steel, dobro and
  bouzouki.
- **Anything else** - the tuner falls back to a chromatic scale, so a
  restrung or homemade instrument tunes just as well.
- **Pitch** - concert pitch is adjustable from 415 to 466 Hz (A4 = 440 by
  default) under Settings > Tuner.

## Strumming Pattern

Lay out a bar of strums on a one-measure grid and hear it back in the same
acoustic voice as the chord library, in time with the tempo.

- **Pattern** - the list is grouped by the grid each pattern is written in,
  and holds the ones people actually play: downstrokes, steady eighths,
  D DU D DU, the folk D DU UDU, D D D U D U, the reggae skank, a muted chug,
  a shuffle and straight sixteenths, plus the 2+2+3 and 3+2+2 groupings that
  odd meters are counted in. Picking a pattern moves the grid chips to the
  grid it was notated in, so it arrives ready to play.
- **Grid** - click a cell to cycle rest, down, up, muted down and muted up,
  or walk the bar with the arrow keys and type one letter: **D** down, **U**
  up, **X** muted downstroke, **M** muted upstroke, **R** rest.
- **Marks** - **A** accents a stroke, **G** turns it into a ghost note and
  **T** ties it into the next cell so it rings on past the beat. **C** clears
  the bar. None of the keys need a modifier, and every one of them is hinted
  along the foot of the card.
- **Strings** - pick guitar or ukulele, then fret a chord from the list or
  leave the strings open, so the pattern rings with a voicing you would really
  use.
- **Sound** - open strums ring out, palm mute damps them into a rhythm and
  muted turns every stroke into a scratch. A stroke written as a mute stays a
  scratch whatever the sound is set to.
- **Meter** - 4/4, 3/4, 2/4, 5/4, 6/8, 7/8, 9/8 or 12/8. The grid chips read
  against the meter's own unit, so 6/8 counts in eighths and its chips offer
  1/8, 1/16, triplet and 1/32.
- **Practice** - count-in, loop, and practice mode, which lifts the tempo a
  little at the start of every bar until you reach your target.
- **Tempo** - the minus and plus buttons nudge the BPM, or click the number
  (or start typing) to enter one. **Enter** sets it, **Esc** cancels.

## Circle of Fifths

All twelve keys around a ring, each a fifth on from the last. Click either
ring to select a key and hear it, and the rest of the wheel reads back from
that selection.

- **Wheel** - the majors on the outer ring, their relative minors inside on
  the same spoke. Scroll or use the arrow keys to walk the circle, and press
  **Up / Down** (or **M**) to hop between the rings without leaving the
  fifth.
- **Chords of the key** - I, IV and V carry their numeral outside, ii, iii
  and vi inside, so the six chords you would normally play are always in
  view. A minor key lights them the other way round.
- **Centre** - the tonic and its seven notes. A minor key uses the natural
  minor, so A minor reads A B C D E F G rather than the major scale of A.
- **Theory** - the key signature, the relative and parallel keys, and the
  mode built on each degree.
- **Instrument** - pick guitar, piano or ukulele (or press **Tab**) to hear
  the scales and chords in that voice.
- **Hear it** - click one of the seven diatonic chips, or press **1**–**7**.

## Note Trainer

A drill for knowing your instrument. A note name appears, the microphone
listens and you play it before the clock runs out.

- **Round** - **Space** starts and stops, and each prompt gets its own timer
  of 2–20 seconds per note, set under Settings > Note trainer.
- **Listening** - a note counts the moment it settles in tune, and the
  readout under the prompt shows what the microphone hears right now.
- **Feedback** - a correct note flashes green with the points it earned; a
  miss shows the note it was looking for before the next prompt appears.
- **Score** - a quick answer is worth up to twice a slow one, and the panel
  keeps your streak, best streak and accuracy. **R** resets it.
- **Voices** - **Tab** cycles guitar, ukulele and piano, and **N** (or the
  Naturals chip) keeps the drill to the white notes.

## Keyboard Shortcuts

- **Win+Y** - open or close the panel.
- **Ctrl+Tab** - switch tools.
- **Tab** - switch instruments.
- **Up / Down** - move through search results, step the metronome, hop
  between the rings of the circle.
- **Left / Right** - step through fingerings, halve or double a tap tempo,
  change the metronome's meter.
- **D U X M R, A G T, C** - lay strokes in the strumming grid, add accents,
  ghosts and ties, clear the bar.
- **Esc** - clear the search, or close the panel when the search is empty.

## Settings

- **Startup** - the tool and instrument the panel opens with.
- **Appearance** - follow Windows or fix light or dark, pick an accent
  colour, set the opacity.
- **Placement** - corner, monitor, fine offsets, and whether the panel closes
  when it loses focus.
- **Shortcut** - the key and modifiers that open the panel.
- **Metronome** - volume, background playback, default meter and subdivision.
- **Chords** - hide the finger numbers.
- **Tap tempo** - how long a pause resets the taps, and whether Space or
  Enter count as taps.
- **Tuner** - reference pitch.
- **Strumming** - default grid, count-in, loop, and the practice ramp.
- **Circle of fifths** - which ring opens, and whether selecting a key plays
  its scale.
- **Note trainer** - seconds per note, chromatic or naturals.

The chord library comes from [chords-db](https://github.com/tombatossals/chords-db)
by David Rubert (MIT). It is packed into the mod, so there is nothing to
download and no internet connection needed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Startup:
  - DefaultTool: chords
    $name: Default tool on open
    $description: "The tool the panel opens on when you press Win+Y."
    $options:
      - chords: Chords
      - tempo: Tap tempo
      - metronome: Metronome
      - tuner: Tuner
      - strum: Strumming
      - circle: Circle of fifths
      - trainer: Note trainer
  - DefaultInstrument: guitar
    $name: Default instrument
    $description: "The starting instrument for chords and tuning."
    $options:
      - guitar: Guitar
      - ukulele: Ukulele
      - piano: Piano
- Appearance:
  - Theme: auto
    $name: Theme
    $description: "Choose whether the panel matches Windows or uses a fixed theme."
    $options:
      - auto: Follow Windows
      - light: Light
      - dark: Dark
  - AccentColor: accent
    $name: Accent color
    $description: "Color used for chord dots, selected items, and sliders."
    $options:
      - accent: Windows Accent Color
      - custom: Custom (Pick A Color Below)
      - blue: Blue
      - green: Green
      - teal: Teal
      - purple: Purple
      - pink: Pink
      - orange: Orange
      - red: Red
      - gold: Gold
      - mint: Mint
  - CustomAccentColor: "#4285F4"
    $name: Custom accent color
    $description: "Custom color value when Accent color is set to Custom (e.g. #4285F4)."
    #! $format: colorRgb
    #! $showIf: {AccentColor: custom}
  - Opacity: 100
    $name: Opacity (%)
    $description: "Transparency level of the panel background."
    #! $min: 70
    #! $max: 100
- Placement:
  - Position: right
    $name: Screen position
    $description: "Where the panel pops up on your screen."
    $options:
      - topleft: Top Left
      - top: Top
      - topright: Top Right
      - left: Left
      - center: Center
      - right: Right
      - bottomleft: Bottom Left
      - bottom: Bottom
      - bottomright: Bottom Right
  - Monitor: pointer
    $name: Target monitor
    $description: "Open on the monitor under your mouse cursor or on the primary display."
    $options:
      - pointer: Monitor With The Pointer
      - primary: Primary Monitor
  - HorizontalOffset: 0
    $name: Horizontal offset (pixels)
    $description: "Fine-tune horizontal position."
    #! $min: -2000
    #! $max: 2000
  - VerticalOffset: 0
    $name: Vertical offset (pixels)
    $description: "Fine-tune vertical position."
    #! $min: -2000
    #! $max: 2000
  - CloseOnFocusLoss: false
    $name: Close when the panel loses focus
    $description: "Close the panel when you click somewhere else. Off by default, so the panel stays on top while you play; the x button or the shortcut closes it."
- Shortcut:
  - HotkeyKey: Y
    $name: Shortcut key
    $description: "The key that opens and closes the panel. Enter a letter (A-Z), a number (0-9), a function key (F1-F24), or a name such as Space, Tab, Enter, Esc, Home, End, PgUp, PgDn, Left, Right, Up, Down."
  - HotkeyWin: true
    $name: Win key
    $description: "Include the Windows key in the shortcut."
  - HotkeyCtrl: false
    $name: Ctrl key
    $description: "Include the Ctrl key in the shortcut."
  - HotkeyAlt: false
    $name: Alt key
    $description: "Include the Alt key in the shortcut."
  - HotkeyShift: false
    $name: Shift key
    $description: "Include the Shift key in the shortcut."
- Metronome:
  - Volume: 70
    $name: Volume (%)
    $description: "Loudness of the metronome clicks."
    #! $min: 0
    #! $max: 100
  - KeepPlaying: true
    $name: Keep playing when panel closes
    $description: "Keep the metronome running in the background while you play."
  - Signature: four-four
    $name: Default time signature
    $description: "Starting meter for the metronome."
    $options:
      - four-four: 4/4
      - three-four: 3/4
      - two-four: 2/4
      - five-four: 5/4
      - six-eight: 6/8
      - seven-eight: 7/8
      - nine-eight: 9/8
      - twelve-eight: 12/8
  - Subdivision: off
    $name: Subdivision
    $description: "Extra background subdivisions between main beats."
    $options:
      - off: Off
      - eighth: Eighth Notes
      - triplet: Triplets
      - sixteenth: Sixteenth Notes
- Chords:
  - HideFingerings: false
    $name: Hide finger numbers
    $description: "Hides the suggested finger numbers inside dots and barre markers."
- TapTempo:
  - ResetAfter: 3
    $name: Reset after (seconds)
    $description: "Time of inactivity before a tap sequence resets."
    #! $min: 1
    #! $max: 10
  - TapKeys: both
    $name: Tap keys
    $description: "Keyboard keys accepted as taps."
    $options:
      - space: Space
      - enter: Enter
      - both: Space Or Enter
- Tuner:
  - ReferencePitch: 440
    $name: Reference pitch (Hz)
    $description: "Frequency of A4 concert pitch."
    #! $min: 415
    #! $max: 466
- Strumming:
  - Subdivision: 2
    $name: Default subdivision
    $description: "Cells per beat the grid opens with: 1 quarters, 2 eighths, 3 triplets, 4 sixteenths."
    #! $min: 1
    #! $max: 4
  - CountIn: true
    $name: Count-in
    $description: "Play one bar of clicks before the pattern starts."
  - Loop: true
    $name: Loop
    $description: "Repeat the measure until you press stop."
  - RampStep: 2
    $name: Practice ramp (BPM)
    $description: "Tempo added at the start of every measure while practice mode is on."
    #! $min: 1
    #! $max: 10
  - TargetOffset: 20
    $name: Practice target (BPM above start)
    $description: "How far above the starting tempo the practice ramp aims."
    #! $min: 0
    #! $max: 80
- Circle:
  - Mode: major
    $name: Default ring
    $description: "Which ring the wheel opens on: the majors outside, or their relative minors inside."
    $options:
      - major: Major
      - minor: Minor
  - PlayOnSelect: true
    $name: Play the scale on select
    $description: "Hear the key's scale play when you pick it."
- NoteTrainer:
  - Seconds: 5
    $name: Seconds per note
    $description: "How long you have to find each note before it counts as a miss."
    #! $min: 2
    #! $max: 20
  - NoteSet: chromatic
    $name: Notes to drill
    $description: "Chromatic includes the sharps and flats; naturals keeps to the white notes."
    $options:
      - chromatic: Chromatic
      - naturals: Naturals only
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>

#include <windhawk_utils.h>

#include <mmreg.h>
#include <mmsystem.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Small helpers

inline float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

inline double NowMs() {
    static const ULONGLONG start = GetTickCount64();
    return static_cast<double>(GetTickCount64() - start);
}

inline double EaseOutCubic(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

// Arrives past its size and settles back, which is what makes a mark worth
// looking at.
inline float EaseOutBack(float t) {
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;
    const float u = Clamp01(t) - 1.0f;
    return 1.0f + c3 * u * u * u + c1 * u * u;
}

template <typename T>
T GetProc(PCWSTR module, const char* name) {
    HMODULE handle = GetModuleHandleW(module);
    if (!handle) {
        handle = LoadLibraryExW(module, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!handle) {
        return nullptr;
    }
    return reinterpret_cast<T>(GetProcAddress(handle, name));
}

std::wstring Trim(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

std::wstring Lower(std::wstring text) {
    for (wchar_t& c : text) {
        c = static_cast<wchar_t>(towlower(c));
    }
    return text;
}

struct OsVersion {
    DWORD major;
    DWORD minor;
    DWORD build;
};

struct OsVersionInfo {
    ULONG size;
    ULONG major;
    ULONG minor;
    ULONG build;
    ULONG platform;
    WCHAR servicePack[128];
};

const OsVersion& GetOsVersion() {
    static const OsVersion version = [] {
        OsVersion v{10, 0, 0};
        using RtlGetVersion_t = LONG(WINAPI*)(OsVersionInfo*);
        auto rtlGetVersion = GetProc<RtlGetVersion_t>(L"ntdll.dll", "RtlGetVersion");
        if (rtlGetVersion) {
            OsVersionInfo info{};
            info.size = sizeof(info);
            if (rtlGetVersion(&info) == 0) {
                v.major = info.major;
                v.minor = info.minor;
                v.build = info.build;
            }
        }
        return v;
    }();
    return version;
}

bool IsWindows10Build(DWORD build) {
    const OsVersion& v = GetOsVersion();
    return v.major > 10 || (v.major == 10 && v.build >= build);
}

////////////////////////////////////////////////////////////////////////////////
// Colors

struct Color {
    float r, g, b, a;
};

constexpr Color Rgb(int r, int g, int b, float a = 1.0f) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, a};
}

inline Color WithAlpha(Color c, float a) {
    c.a = a;
    return c;
}

inline Color Mix(const Color& x, const Color& y, float t) {
    return {x.r + (y.r - x.r) * t, x.g + (y.g - x.g) * t,
            x.b + (y.b - x.b) * t, x.a + (y.a - x.a) * t};
}

constexpr Color kWhite = Rgb(255, 255, 255);
constexpr Color kBlack = Rgb(0, 0, 0);
constexpr Color kFallbackAccent = Rgb(66, 133, 244);

inline uint32_t PackPremultiplied(const Color& c, float coverage) {
    const float a = Clamp01(c.a * coverage);
    if (a <= 0.0f) {
        return 0;
    }
    const uint32_t alpha = static_cast<uint32_t>(a * 255.0f + 0.5f);
    auto channel = [&](float v) {
        return std::min(alpha,
                        static_cast<uint32_t>(Clamp01(v) * a * 255.0f + 0.5f));
    };
    return (alpha << 24) | (channel(c.r) << 16) | (channel(c.g) << 8) |
           channel(c.b);
}

inline void BlendOver(uint32_t& dst, uint32_t src) {
    const uint32_t sa = src >> 24;
    if (sa == 0) {
        return;
    }
    if (sa == 255) {
        dst = src;
        return;
    }
    const uint32_t inv = 255 - sa;
    const uint32_t d = dst;
    uint32_t rb = (d & 0x00FF00FF) * inv + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((d >> 8) & 0x00FF00FF) * inv + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    dst = src + (rb | ag);
}

inline void BlendColor(uint32_t& dst, const Color& c, float coverage) {
    BlendOver(dst, PackPremultiplied(c, coverage));
}

// ClearType reports a coverage value per channel, so text is blended channel by
// channel: the destination is un-premultiplied, mixed and premultiplied again.
inline void BlendText(uint32_t& dst,
                      const Color& c,
                      uint32_t covR,
                      uint32_t covG,
                      uint32_t covB) {
    const float alpha = static_cast<float>(dst >> 24) / 255.0f;
    if (alpha <= 0.0f || (covR | covG | covB) == 0) {
        return;
    }
    auto channel = [&](int shift, float color, uint32_t coverage) {
        const uint32_t value = (dst >> shift) & 0xFF;
        if (coverage == 0) {
            return value;
        }
        const float straight =
            Clamp01(static_cast<float>(value) / 255.0f / alpha);
        const float t = static_cast<float>(coverage) / 255.0f;
        const float out = color * t + straight * (1.0f - t);
        return static_cast<uint32_t>(Clamp01(out * alpha) * 255.0f + 0.5f);
    };
    dst = (dst & 0xFF000000u) | (channel(16, c.r, covR) << 16) |
          (channel(8, c.g, covG) << 8) | channel(0, c.b, covB);
}

////////////////////////////////////////////////////////////////////////////////
// Surfaces

class Surface {
  public:
    Surface() = default;
    ~Surface() { Reset(); }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

    bool Resize(int width, int height) {
        width = std::max(width, 1);
        height = std::max(height, 1);
        if (!bits_ || width > stride_ || height > capacityHeight_) {
            Reset();
            const int capacityWidth = (width + 63) & ~63;
            const int capacityHeight = (height + 63) & ~63;

            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = capacityWidth;
            info.bmiHeader.biHeight = -capacityHeight;  // Top-down.
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            dc_ = CreateCompatibleDC(nullptr);
            if (!dc_) {
                return false;
            }
            void* bits = nullptr;
            bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits,
                                       nullptr, 0);
            if (!bitmap_ || !bits) {
                Reset();
                return false;
            }
            oldBitmap_ = SelectObject(dc_, bitmap_);
            bits_ = static_cast<uint32_t*>(bits);
            stride_ = capacityWidth;
            capacityHeight_ = capacityHeight;
        }
        width_ = width;
        height_ = height;
        Clear();
        return true;
    }

    void Reset() {
        if (dc_) {
            if (oldBitmap_) {
                SelectObject(dc_, oldBitmap_);
            }
            DeleteDC(dc_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        dc_ = nullptr;
        bitmap_ = nullptr;
        oldBitmap_ = nullptr;
        bits_ = nullptr;
        width_ = height_ = stride_ = capacityHeight_ = 0;
    }

    void Clear() {
        for (int y = 0; y < height_; y++) {
            memset(Row(y), 0, static_cast<size_t>(width_) * 4);
        }
    }

    bool Valid() const { return bits_ != nullptr; }
    int Width() const { return width_; }
    int Height() const { return height_; }
    HDC Dc() const { return dc_; }
    uint32_t* Row(int y) { return bits_ + static_cast<size_t>(y) * stride_; }
    const uint32_t* Row(int y) const {
        return bits_ + static_cast<size_t>(y) * stride_;
    }

  private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    uint32_t* bits_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int stride_ = 0;
    int capacityHeight_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Signed distance shapes

struct Vec2 {
    float x, y;
};

inline float SdRoundRect(float px,
                         float py,
                         float left,
                         float top,
                         float right,
                         float bottom,
                         float radius) {
    const float hw = (right - left) * 0.5f;
    const float hh = (bottom - top) * 0.5f;
    const float r = std::max(0.0f, std::min(radius, std::min(hw, hh)));
    const float qx = fabsf(px - (left + hw)) - hw + r;
    const float qy = fabsf(py - (top + hh)) - hh + r;
    const float ox = std::max(qx, 0.0f);
    const float oy = std::max(qy, 0.0f);
    return sqrtf(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0f) - r;
}

inline float SdSegment(Vec2 p, Vec2 a, Vec2 b) {
    const Vec2 pa{p.x - a.x, p.y - a.y};
    const Vec2 ba{b.x - a.x, b.y - a.y};
    const float lengthSq = ba.x * ba.x + ba.y * ba.y;
    const float h =
        lengthSq > 0 ? Clamp01((pa.x * ba.x + pa.y * ba.y) / lengthSq) : 0.0f;
    const float dx = pa.x - ba.x * h;
    const float dy = pa.y - ba.y * h;
    return sqrtf(dx * dx + dy * dy);
}

inline float Erfc(float x) {
    const float z = fabsf(x);
    const float t = 1.0f / (1.0f + 0.3275911f * z);
    const float y =
        t *
        (0.254829592f +
         t * (-0.284496736f +
              t * (1.421413741f + t * (-1.453152027f + t * 1.061405429f)))) *
        expf(-z * z);
    return x >= 0 ? y : 2.0f - y;
}

inline float SoftCoverage(float distance, float sigma) {
    return 0.5f * Erfc(distance / (sigma * 1.41421356f));
}

template <typename DistanceFn, typename ColorFn>
void FillShape(Surface& s,
               float x0,
               float y0,
               float x1,
               float y1,
               DistanceFn distance,
               ColorFn color) {
    const int ix0 = std::max(0, static_cast<int>(floorf(x0)));
    const int iy0 = std::max(0, static_cast<int>(floorf(y0)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(x1)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(y1)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        const float py = y + 0.5f;
        for (int x = ix0; x < ix1; x++) {
            const float px = x + 0.5f;
            const float coverage = Clamp01(0.5f - distance(px, py));
            if (coverage > 0.0f) {
                BlendColor(row[x], color(px, py), coverage);
            }
        }
    }
}

void FillRoundRect(Surface& s,
                   float l,
                   float t,
                   float r,
                   float b,
                   float radius,
                   Color c) {
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float) { return c; });
}

void FillRoundRectGradient(Surface& s,
                           float l,
                           float t,
                           float r,
                           float b,
                           float radius,
                           Color top,
                           Color bottom) {
    const float height = std::max(1.0f, b - t);
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float y) { return Mix(top, bottom, Clamp01((y - t) / height)); });
}

void StrokeRoundRect(Surface& s,
                     float l,
                     float t,
                     float r,
                     float b,
                     float radius,
                     float width,
                     Color c) {
    FillShape(
        s, l - width, t - width, r + width, b + width,
        [&](float x, float y) {
            return fabsf(SdRoundRect(x, y, l, t, r, b, radius)) - width * 0.5f;
        },
        [&](float, float) { return c; });
}

void FillCircle(Surface& s, float cx, float cy, float radius, Color c) {
    FillShape(
        s, cx - radius - 1, cy - radius - 1, cx + radius + 1, cy + radius + 1,
        [&](float x, float y) {
            return sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) - radius;
        },
        [&](float, float) { return c; });
}

void StrokeCircle(Surface& s,
                  float cx,
                  float cy,
                  float radius,
                  float width,
                  Color c) {
    FillShape(
        s, cx - radius - width, cy - radius - width, cx + radius + width,
        cy + radius + width,
        [&](float x, float y) {
            return fabsf(sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) -
                         radius) -
                   width * 0.5f;
        },
        [&](float, float) { return c; });
}

void StrokeLine(Surface& s, Vec2 a, Vec2 b, float width, Color c) {
    const float half = width * 0.5f;
    FillShape(
        s, std::min(a.x, b.x) - width, std::min(a.y, b.y) - width,
        std::max(a.x, b.x) + width, std::max(a.y, b.y) + width,
        [&](float x, float y) { return SdSegment({x, y}, a, b) - half; },
        [&](float, float) { return c; });
}

void DrawShadow(Surface& s,
                float l,
                float t,
                float r,
                float b,
                float radius,
                float sigma,
                Color c) {
    const float reach = sigma * 3.0f;
    const int ix0 = std::max(0, static_cast<int>(floorf(l - reach)));
    const int iy0 = std::max(0, static_cast<int>(floorf(t - reach)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(r + reach)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(b + reach)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        for (int x = ix0; x < ix1; x++) {
            const float d = SdRoundRect(x + 0.5f, y + 0.5f, l, t, r, b, radius);
            const float coverage = SoftCoverage(d, sigma);
            if (coverage > 0.002f) {
                BlendColor(row[x], c, coverage);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

class FontCache {
  public:
    void Init() {
        static const PCWSTR kUiFaces[] = {L"Segoe UI Variable Text",
                                          L"Segoe UI", L"Tahoma"};
        face_ = PickFace(kUiFaces, ARRAYSIZE(kUiFaces));
        // The shell's symbol font. Windows 11 ships the Fluent set, Windows 10
        // the older MDL2 one, and both carry the same glyph points.
        static const PCWSTR kIconFaces[] = {L"Segoe Fluent Icons",
                                            L"Segoe MDL2 Assets"};
        iconFace_ = PickFace(kIconFaces, ARRAYSIZE(kIconFaces));
    }

    HFONT Get(int px, int weight) { return Find(entries_, face_, px, weight); }

    // A glyph font for the icons, so a symbol is drawn by the system set rather
    // than by hand. The size is the glyph's em square, roughly its cap height.
    HFONT GetIcon(int px) {
        return Find(iconEntries_, iconFace_, px, FW_NORMAL);
    }

    void Clear() {
        for (const Entry& entry : entries_) {
            DeleteObject(entry.font);
        }
        entries_.clear();
        for (const Entry& entry : iconEntries_) {
            DeleteObject(entry.font);
        }
        iconEntries_.clear();
    }

  private:
    struct Entry {
        int px;
        int weight;
        HFONT font;
    };

    HFONT Find(std::vector<Entry>& entries,
               const std::wstring& face,
               int px,
               int weight) {
        px = std::max(px, 6);
        for (const Entry& entry : entries) {
            if (entry.px == px && entry.weight == weight) {
                return entry.font;
            }
        }
        HFONT font = CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_TT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, face.c_str());
        if (font) {
            entries.push_back({px, weight, font});
        }
        return font ? font : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }

    static int CALLBACK EnumFontProc(const LOGFONTW*,
                                     const TEXTMETRICW*,
                                     DWORD,
                                     LPARAM param) {
        *reinterpret_cast<bool*>(param) = true;
        return 0;
    }

    static std::wstring PickFace(const PCWSTR* faces, size_t count) {
        HDC dc = GetDC(nullptr);
        for (size_t i = 0; i < count; i++) {
            LOGFONTW lf{};
            lf.lfCharSet = DEFAULT_CHARSET;
            wcsncpy_s(lf.lfFaceName, faces[i], _TRUNCATE);
            bool found = false;
            EnumFontFamiliesExW(dc, &lf, EnumFontProc,
                                reinterpret_cast<LPARAM>(&found), 0);
            if (found) {
                ReleaseDC(nullptr, dc);
                return faces[i];
            }
        }
        ReleaseDC(nullptr, dc);
        return faces[count - 1];
    }

    std::vector<Entry> entries_;
    std::vector<Entry> iconEntries_;
    std::wstring face_ = L"Segoe UI";
    std::wstring iconFace_ = L"Segoe MDL2 Assets";
};

FontCache g_fonts;

class TextPainter {
  public:
    SIZE Measure(HFONT font, const std::wstring& text,
                 UINT flags = DT_SINGLELINE) {
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        RECT rc{0, 0, 0, 0};
        DrawTextW(measureDc_, text.empty() ? L" " : text.c_str(),
                  text.empty() ? 1 : static_cast<int>(text.size()), &rc,
                  flags | DT_CALCRECT | DT_NOPREFIX);
        SelectObject(measureDc_, old);
        return {rc.right - rc.left, rc.bottom - rc.top};
    }

    int Width(HFONT font, const wchar_t* text, int length) {
        if (length <= 0) {
            return 0;
        }
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        SIZE size{};
        GetTextExtentPoint32W(measureDc_, text, length, &size);
        SelectObject(measureDc_, old);
        return size.cx;
    }

    // Shortens the text until it fits, ending it with an ellipsis.
    std::wstring Ellipsize(HFONT font, std::wstring text, int width) {
        if (width <= 0) {
            return std::wstring();
        }
        if (Width(font, text.c_str(), static_cast<int>(text.size())) <= width) {
            return text;
        }
        while (!text.empty()) {
            text.pop_back();
            std::wstring candidate = text + L"\x2026";
            if (Width(font, candidate.c_str(), static_cast<int>(candidate.size())) <=
                width) {
                return candidate;
            }
        }
        return std::wstring();
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              RECT clip,
              UINT flags,
              Color color) {
        RECT bounds{0, 0, dst.Width(), dst.Height()};
        if (!IntersectRect(&clip, &clip, &bounds)) {
            return;
        }
        const int w = clip.right - clip.left;
        const int h = clip.bottom - clip.top;
        if (text.empty() || w <= 0 || h <= 0 || !mask_.Resize(w, h)) {
            return;
        }
        HDC dc = mask_.Dc();
        HGDIOBJ old = SelectObject(dc, font);
        SetTextColor(dc, RGB(255, 255, 255));
        SetBkMode(dc, TRANSPARENT);
        OffsetRect(&layout, -clip.left, -clip.top);
        DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &layout,
                  flags | DT_NOPREFIX);
        SelectObject(dc, old);
        GdiFlush();  // Finish GDI text before reading the mask.

        const float luminance = color.r * 0.3f + color.g * 0.59f + color.b * 0.11f;
        const float gamma = luminance < 0.5f ? 0.8f : 0.9f;
        uint32_t lut[256];
        for (int i = 0; i < 256; i++) {
            lut[i] = static_cast<uint32_t>(powf(i / 255.0f, gamma) * 255.0f + 0.5f);
        }
        for (int y = 0; y < h; y++) {
            const uint32_t* src = mask_.Row(y);
            uint32_t* row = dst.Row(clip.top + y) + clip.left;
            for (int x = 0; x < w; x++) {
                const uint32_t m = src[x];
                if (m & 0x00FFFFFFu) {
                    BlendText(row[x], color, lut[(m >> 16) & 0xFF],
                              lut[(m >> 8) & 0xFF], lut[m & 0xFF]);
                }
            }
        }
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              UINT flags,
              Color color) {
        Draw(dst, font, text, layout, layout, flags, color);
    }

    ~TextPainter() {
        if (measureDc_) {
            DeleteDC(measureDc_);
        }
    }

  private:
    void EnsureMeasureDc() {
        if (!measureDc_) {
            measureDc_ = CreateCompatibleDC(nullptr);
        }
    }

    Surface mask_;
    HDC measureDc_ = nullptr;
};

TextPainter g_text;

////////////////////////////////////////////////////////////////////////////////
// Layered windows

HINSTANCE g_instance;

constexpr WCHAR kPanelClass[] = L"MusiciansToolkitPanel_" WH_MOD_ID;
constexpr WCHAR kShadowClass[] = L"MusiciansToolkitShadow_" WH_MOD_ID;
constexpr WCHAR kMessageClass[] = L"MusiciansToolkitMessages_" WH_MOD_ID;

bool TransparencyEnabled() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

void SetDwmAttribute(HWND hwnd, DWORD attribute, const void* value, DWORD size) {
    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    static auto setAttribute =
        GetProc<DwmSetWindowAttribute_t>(L"dwmapi.dll", "DwmSetWindowAttribute");
    if (setAttribute) {
        setAttribute(hwnd, attribute, value, size);
    }
}

struct AccentPolicy {
    int state;
    int flags;
    DWORD gradientColor;  // 0xAABBGGRR.
    int animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    PVOID data;
    SIZE_T size;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    static auto function = GetProc<SetWindowCompositionAttribute_t>(
        L"user32.dll", "SetWindowCompositionAttribute");
    return function;
}

// Keeps the panel free of non-client chrome, rounds it like a shell flyout and
// gives it the acrylic blur Windows uses for its own.
void ConfigureGlassWindow(HWND hwnd, const Color* tint) {
    const DWORD nonClient = 1;  // DWMNCRP_DISABLED.
    SetDwmAttribute(hwnd, 2 /* DWMWA_NCRENDERING_POLICY */, &nonClient,
                    sizeof(nonClient));
    const BOOL noTransitions = TRUE;
    SetDwmAttribute(hwnd, 3 /* DWMWA_TRANSITIONS_FORCEDISABLED */,
                    &noTransitions, sizeof(noTransitions));
    if (IsWindows10Build(22621)) {
        const DWORD backdrop = 1;  // DWMSBT_NONE.
        SetDwmAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop,
                        sizeof(backdrop));
    }
    if (auto setAttribute = GetSetWindowCompositionAttribute()) {
        auto channel = [](float v) { return static_cast<DWORD>(v * 255.0f + 0.5f); };
        AccentPolicy policy{
            tint ? 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */ : 0, 0,
            tint ? 0x01000000 | channel(tint->b) << 16 |
                       channel(tint->g) << 8 | channel(tint->r)
                 : 0,
            0};
        WindowCompositionAttributeData data{19 /* WCA_ACCENT_POLICY */, &policy,
                                            sizeof(policy)};
        setAttribute(hwnd, &data);
    }
    if (IsWindows10Build(22000)) {
        const DWORD corners = 2;  // DWMWCP_ROUND.
        SetDwmAttribute(hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corners,
                        sizeof(corners));
        const COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE.
        SetDwmAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &border,
                        sizeof(border));
    }
}

class LayeredWindow {
  public:
    bool Create(DWORD exStyle, PCWSTR className, PCWSTR title, void* param) {
        hwnd_ = CreateWindowExW(exStyle | WS_EX_LAYERED | WS_EX_TOPMOST |
                                    WS_EX_TOOLWINDOW,
                                className, title, WS_POPUP, 0, 0, 1, 1, nullptr,
                                nullptr, g_instance, param);
        return hwnd_ != nullptr;
    }

    void Destroy() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
    }

    bool Present(Surface& surface, int x, int y, BYTE alpha) {
        if (!hwnd_ || !surface.Valid()) {
            return false;
        }
        POINT dst{x, y};
        SIZE size{surface.Width(), surface.Height()};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        GdiFlush();  // Finish GDI drawing before uploading the pixels.
        if (!UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, surface.Dc(), &src,
                                 0, &blend, ULW_ALPHA)) {
            return false;
        }
        x_ = x;
        y_ = y;
        width_ = size.cx;
        height_ = size.cy;
        alpha_ = alpha;
        return true;
    }

    void SetAlpha(BYTE alpha) {
        if (hwnd_ && alpha != alpha_) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
            UpdateLayeredWindow(hwnd_, nullptr, nullptr, nullptr, nullptr,
                                nullptr, 0, &blend, ULW_ALPHA);
            alpha_ = alpha;
        }
    }

    void Move(int x, int y) {
        if (hwnd_ && (x != x_ || y != y_)) {
            SetWindowPos(hwnd_, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                             SWP_NOOWNERZORDER);
            x_ = x;
            y_ = y;
        }
    }

    void Show(bool activate) {
        if (hwnd_ && !visible_) {
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                             (activate ? 0 : SWP_NOACTIVATE));
            visible_ = true;
        }
    }

    void Hide() {
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            visible_ = false;
        }
    }

    HWND Hwnd() const { return hwnd_; }
    bool Visible() const { return visible_; }
    int X() const { return x_; }
    int Y() const { return y_; }
    BYTE Alpha() const { return alpha_; }
    void SetPosition(int x, int y) {
        x_ = x;
        y_ = y;
    }

  private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    BYTE alpha_ = 255;
};

////////////////////////////////////////////////////////////////////////////////
// Accent color

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return Trim(result);
}

Color WindowsAccentColor() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        // Stored as 0xAABBGGRR.
        return Rgb(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF);
    }
    return kFallbackAccent;
}

Color ParseColorText(std::wstring text, Color fallback) {
    text = Lower(Trim(text));
    if (text.empty()) {
        return fallback;
    }
    if (text[0] == L'#') {
        text.erase(0, 1);
    } else if (text.rfind(L"0x", 0) == 0) {
        text.erase(0, 2);
    }
    const bool isHex =
        (text.size() == 3 || text.size() == 6) &&
        text.find_first_not_of(L"0123456789abcdef") == std::wstring::npos;
    if (isHex) {
        const unsigned long v = wcstoul(text.c_str(), nullptr, 16);
        if (text.size() == 3) {
            return Rgb(static_cast<int>(((v >> 8) & 0xF) * 17),
                       static_cast<int>(((v >> 4) & 0xF) * 17),
                       static_cast<int>((v & 0xF) * 17));
        }
        return Rgb(static_cast<int>((v >> 16) & 0xFF),
                   static_cast<int>((v >> 8) & 0xFF),
                   static_cast<int>(v & 0xFF));
    }

    int values[3] = {0, 0, 0};
    int count = 0;
    for (size_t i = 0; i < text.size() && count < 3;) {
        if (iswdigit(text[i])) {
            values[count++] = std::min(255, _wtoi(text.c_str() + i));
            while (i < text.size() && iswdigit(text[i])) {
                i++;
            }
        } else {
            i++;
        }
    }
    if (count == 3) {
        return Rgb(values[0], values[1], values[2]);
    }
    return fallback;
}

Color ResolveAccent(const std::wstring& choice, const std::wstring& custom) {
    static const struct {
        PCWSTR name;
        Color color;
    } kPresets[] = {
        {L"blue", Rgb(66, 133, 244)},   {L"red", Rgb(234, 67, 53)},
        {L"green", Rgb(52, 168, 83)},   {L"teal", Rgb(0, 168, 158)},
        {L"purple", Rgb(150, 92, 255)}, {L"pink", Rgb(255, 92, 170)},
        {L"orange", Rgb(255, 138, 32)}, {L"gold", Rgb(240, 180, 40)},
        {L"mint", Rgb(64, 220, 160)},
    };
    const std::wstring key = Lower(Trim(choice));
    if (key == L"custom") {
        return ParseColorText(custom, WindowsAccentColor());
    }
    for (const auto& preset : kPresets) {
        if (key == preset.name) {
            return preset.color;
        }
    }
    return WindowsAccentColor();
}

bool SystemUsesDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return true;
    }
    return value == 0;
}

// End of the shared toolkit. The chord library follows.
////////////////////////////////////////////////////////////////////////////////
// Chord data
//
// The library is packed into one blob per instrument and decoded once, the
// first time the instrument is used. Records are byte streams, so nothing is
// parsed from text at runtime:
//
//   key, suffix, voicingCount, then the voicings:
//     fretted: baseFret, frets[strings], fingers[strings],
//              barreCount, then per barre: fret, fromString, toString
//     piano:   noteCount, then midi[noteCount]
//
// tools/build-chord-data.js produces the tables at the end of this file from the
// chords-db JSON files.

namespace chorddata {

enum Kind {
    kFretted,
    kPiano,
};

struct InstrumentData {
    PCWSTR name;
    Kind kind;
    int strings;
    const int* tuning;          // Midi note of every open string.
    const PCWSTR* keys;
    int keyCount;
    const PCWSTR* suffixes;     // Raw suffixes, for searching.
    const PCWSTR* suffixNames;  // How a suffix is written on screen.
    int suffixCount;
    const char* const* chunks;  // Base64 pieces of the packed voicings.
    int chunkCount;
};

extern const InstrumentData kInstruments[];
extern const int kInstrumentsSize;

}  // namespace chorddata

enum Instrument {
    kInstrumentGuitar,
    kInstrumentUkulele,
    kInstrumentPiano,
    kInstrumentCount,
};

constexpr int kMutedFret = 0xFF;
constexpr int kMaxStrings = 8;
constexpr int kMaxBarres = 8;
constexpr int kMaxNotes = 16;

struct ChordEntry {
    uint8_t key = 0;
    uint8_t suffix = 0;
    uint32_t offset = 0;  // Byte offset of the record in the blob.
    std::wstring name;    // "C#m7".
    std::wstring search;  // Lowercase text that searches match against.
};

struct Library {
    bool loaded = false;
    std::vector<uint8_t> data;
    std::vector<ChordEntry> chords;
};

Library g_libraries[kInstrumentCount];

struct Barre {
    int fret = 0;  // Relative to baseFret.
    int from = 0;
    int to = 0;
};

struct Voicing {
    bool valid = false;
    int baseFret = 1;
    int stringCount = 0;
    int frets[kMaxStrings] = {};
    int fingers[kMaxStrings] = {};
    Barre barres[kMaxBarres];
    int barreCount = 0;
    int notes[kMaxNotes] = {};  // Midi notes, held keys for the piano.
    int noteCount = 0;
};

int Base64Value(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+' || c == '-') {
        return 62;
    }
    if (c == '/' || c == '_') {
        return 63;
    }
    return -1;
}

void AppendBase64(const char* text, std::vector<uint8_t>& out) {
    uint32_t buffer = 0;
    int bits = 0;
    for (const char* p = text; *p; p++) {
        const int value = Base64Value(*p);
        if (value < 0) {
            continue;
        }
        buffer = (buffer << 6) | static_cast<uint32_t>(value);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8_t>((buffer >> bits) & 0xFF));
        }
    }
}

size_t VoicingSize(const chorddata::InstrumentData& info,
                   const uint8_t* p,
                   size_t available) {
    if (available < 1) {
        return available;
    }
    if (info.kind == chorddata::kFretted) {
        const size_t body = 1 + static_cast<size_t>(info.strings) * 2;
        if (available < body + 1) {
            return available;
        }
        return body + 1 + static_cast<size_t>(p[body]) * 3;
    }
    return 1 + static_cast<size_t>(p[0]);
}

bool EnsureLibrary(int instrument) {
    if (instrument < 0 || instrument >= kInstrumentCount ||
        instrument >= chorddata::kInstrumentsSize) {
        return false;
    }
    Library& library = g_libraries[instrument];
    if (library.loaded) {
        return true;
    }
    if (!library.chords.empty()) {
        library.loaded = true;
        return true;
    }

    const chorddata::InstrumentData& info = chorddata::kInstruments[instrument];
    library.data.clear();
    for (int i = 0; i < info.chunkCount; i++) {
        AppendBase64(info.chunks[i], library.data);
    }
    if (library.data.empty()) {
        Wh_Log(L"No chord data for instrument %d", instrument);
        return false;
    }

    const uint8_t* bytes = library.data.data();
    const size_t size = library.data.size();
    size_t offset = 0;
    while (offset + 3 <= size) {
        ChordEntry entry;
        entry.key = bytes[offset];
        entry.suffix = bytes[offset + 1];
        entry.offset = static_cast<uint32_t>(offset);
        const int voicings = bytes[offset + 2];
        if (entry.key < info.keyCount && entry.suffix < info.suffixCount) {
            entry.name = std::wstring(info.keys[entry.key]) +
                         info.suffixNames[entry.suffix];
            // Searched without spaces, so "c m7" and "c# maj7" both land.
            entry.search = Lower(entry.name + info.keys[entry.key] +
                                 info.suffixes[entry.suffix]);
            library.chords.push_back(std::move(entry));
        }
        size_t next = offset + 3;
        for (int i = 0; i < voicings && next < size; i++) {
            next += VoicingSize(info, bytes + next, size - next);
        }
        if (next <= offset) {
            break;  // Corrupt data: stop rather than spin.
        }
        offset = next;
    }
    library.loaded = !library.chords.empty();
    return library.loaded;
}

int VoicingCount(int instrument, const ChordEntry& entry) {
    if (instrument < 0 || instrument >= kInstrumentCount) {
        return 0;
    }
    const Library& library = g_libraries[instrument];
    if (entry.offset + 3 > library.data.size()) {
        return 0;
    }
    return library.data[entry.offset + 2];
}

bool ReadVoicing(int instrument, const ChordEntry& entry, int index, Voicing& out) {
    out = Voicing();
    if (instrument < 0 || instrument >= kInstrumentCount) {
        return false;
    }
    const chorddata::InstrumentData& info = chorddata::kInstruments[instrument];
    const Library& library = g_libraries[instrument];
    if (entry.offset + 3 > library.data.size()) {
        return false;
    }
    const uint8_t* bytes = library.data.data();
    const size_t size = library.data.size();
    const int voicings = bytes[entry.offset + 2];
    if (index < 0 || index >= voicings) {
        return false;
    }
    size_t offset = entry.offset + 3;
    for (int i = 0; i < index; i++) {
        offset += VoicingSize(info, bytes + offset, size - offset);
        if (offset >= size) {
            return false;
        }
    }

    out.valid = true;
    if (info.kind == chorddata::kFretted) {
        if (offset + 1 + static_cast<size_t>(info.strings) * 2 + 1 > size) {
            return false;
        }
        out.stringCount = info.strings;
        out.baseFret = bytes[offset++];
        for (int i = 0; i < info.strings; i++) {
            out.frets[i] = bytes[offset + i];
        }
        offset += info.strings;
        for (int i = 0; i < info.strings; i++) {
            out.fingers[i] = bytes[offset + i];
        }
        offset += info.strings;
        const int barres = bytes[offset++];
        out.barreCount = std::min(barres, kMaxBarres);
        for (int i = 0; i < out.barreCount; i++) {
            if (offset + 3 > size) {
                out.barreCount = i;
                break;
            }
            out.barres[i].fret = bytes[offset];
            out.barres[i].from = bytes[offset + 1];
            out.barres[i].to = bytes[offset + 2];
            offset += 3;
        }
        for (int i = 0; i < info.strings && i < kMaxNotes; i++) {
            const int fret = out.frets[i];
            if (fret == kMutedFret) {
                continue;
            }
            out.notes[out.noteCount++] =
                info.tuning[i] + (fret == 0 ? 0 : out.baseFret + fret - 1);
        }
    } else {
        const int notes = bytes[offset++];
        out.noteCount = std::min(notes, kMaxNotes);
        for (int i = 0; i < out.noteCount; i++) {
            if (offset + i >= size) {
                out.noteCount = i;
                break;
            }
            out.notes[i] = bytes[offset + i];
        }
    }
    return true;
}

// Absolute fret of a string in the drawn window, or a negative value for an
// open or muted string.
int AbsoluteFret(const Voicing& voicing, int string) {
    const int fret = voicing.frets[string];
    if (fret == kMutedFret || fret == 0) {
        return -1;
    }
    return voicing.baseFret + fret - 1;
}

int KeyPitchClass(PCWSTR key) {
    if (!key || !*key) {
        return 0;
    }
    int pitch = 0;
    switch (key[0]) {
        case L'C':
            pitch = 0;
            break;
        case L'D':
            pitch = 2;
            break;
        case L'E':
            pitch = 4;
            break;
        case L'F':
            pitch = 5;
            break;
        case L'G':
            pitch = 7;
            break;
        case L'A':
            pitch = 9;
            break;
        case L'B':
            pitch = 11;
            break;
        default:
            return 0;
    }
    for (PCWSTR p = key + 1; *p; p++) {
        if (*p == L'#') {
            pitch++;
        } else if (*p == L'b') {
            pitch--;
        }
    }
    return ((pitch % 12) + 12) % 12;
}

PCWSTR PitchClassName(int pitchClass) {
    static const PCWSTR kNames[12] = {L"C", L"C#", L"D", L"D#", L"E", L"F",
                                      L"F#", L"G", L"G#", L"A", L"A#", L"B"};
    return kNames[((pitchClass % 12) + 12) % 12];
}

// The notes of the chord, starting at its root, without repeated pitch classes.
std::wstring ChordTones(int instrument, const ChordEntry& entry,
                        const Voicing& voicing) {
    bool used[12] = {};
    for (int i = 0; i < voicing.noteCount; i++) {
        used[((voicing.notes[i] % 12) + 12) % 12] = true;
    }
    int root = 0;
    if (instrument >= 0 && instrument < chorddata::kInstrumentsSize) {
        const auto& info = chorddata::kInstruments[instrument];
        if (entry.key < info.keyCount) {
            root = KeyPitchClass(info.keys[entry.key]);
        }
    }
    std::wstring tones;
    for (int step = 0; step < 12; step++) {
        const int pitch = (root + step) % 12;
        if (!used[pitch]) {
            continue;
        }
        if (!tones.empty()) {
            tones += L" \x00B7 ";
        }
        tones += PitchClassName(pitch);
    }
    return tones;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum ThemeMode {
    kThemeAuto,
    kThemeLight,
    kThemeDark,
};

////////////////////////////////////////////////////////////////////////////////
// Time signatures
//
// The panel offers the eight below as one click presets. The beat count and the
// note value beside them reach the rest of the meters, from 1/2 to 16/16.

constexpr int kSignatureCount = 8;

struct Signature {
    int beats;
    int unit;
};

constexpr Signature kSignatures[kSignatureCount] = {
    {4, 4}, {3, 4}, {2, 4}, {5, 4}, {6, 8}, {7, 8}, {9, 8}, {12, 8},
};

PCWSTR SignatureId(int index) {
    switch (index) {
        case 1:
            return L"three-four";
        case 2:
            return L"two-four";
        case 3:
            return L"five-four";
        case 4:
            return L"six-eight";
        case 5:
            return L"seven-eight";
        case 6:
            return L"nine-eight";
        case 7:
            return L"twelve-eight";
        default:
            break;
    }
    return L"four-four";
}

// Compound meters are felt in threes, so 6/8, 9/8 and 12/8 take a second accent
// on the first beat of every group; the plain meters only accent the downbeat.
int DefaultGrouping(int beats, int unit) {
    return unit >= 8 && beats > 3 && beats % 3 == 0 ? 3 : 1;
}

std::wstring MeterLabel(int beats, int unit) {
    return std::to_wstring(beats) + L"/" + std::to_wstring(unit);
}

// The note values the meter row offers, from half notes down to sixteenths.
constexpr int kBeatUnits[4] = {2, 4, 8, 16};

PCWSTR SubdivisionLabel(int subdivision) {
    switch (subdivision) {
        case 2:
            return L"8ths";
        case 3:
            return L"Triplet";
        case 4:
            return L"16ths";
        default:
            break;
    }
    return L"Off";
}

// A strum grid cell always stands for a note value, so its chips name that
// value: leaving them on the metronome's "Off" would read as a broken grid.
// The name is read against the meter's own unit, so the eighths of a six-eight
// bar are its plain grid while they are the second grid of a four-four bar.
PCWSTR StrumGridLabel(int subdivision, int unit) {
    if (unit >= 8) {
        switch (subdivision) {
            case 2:
                return L"1/16";
            case 3:
                return L"Triplet";
            case 4:
                return L"1/32";
            default:
                break;
        }
        return L"1/8";
    }
    switch (subdivision) {
        case 2:
            return L"1/8";
        case 3:
            return L"Triplet";
        case 4:
            return L"1/16";
        default:
            break;
    }
    return L"1/4";
}

// What a grid is in words, for the headings of the pattern list.
PCWSTR StrumGridNote(int subdivision) {
    switch (subdivision) {
        case 2:
            return L"two to a beat";
        case 3:
            return L"three to a beat";
        case 4:
            return L"four to a beat";
        default:
            break;
    }
    return L"one to a beat";
}

////////////////////////////////////////////////////////////////////////////////
// Tunings
//
// The tuner works from the note each string is meant to sound, so a tuning is a
// name and a list of MIDI notes, low string first. The five string banjo keeps
// its drone at the end of the list, which is where a player reads it.

constexpr int kTunerStringMax = 8;

struct Tuning {
    int instrument;
    PCWSTR name;
    int strings;
    int notes[kTunerStringMax];
};

constexpr PCWSTR kTunerInstruments[] = {
    L"Guitar",  L"Bass",   L"Ukulele", L"Mandolin",  L"Banjo",
    L"Violin",  L"Viola",   L"Cello",   L"Double bass", L"Bouzouki",
    L"Lap steel", L"Dobro",
};
constexpr int kTunerInstrumentCount =
    static_cast<int>(sizeof(kTunerInstruments) / sizeof(kTunerInstruments[0]));

constexpr Tuning kTunings[] = {
    // Guitar, from the book tunings out to the extended range ones.
    {0, L"Standard", 6, {40, 45, 50, 55, 59, 64}},
    {0, L"Drop D", 6, {38, 45, 50, 55, 59, 64}},
    {0, L"Half step down", 6, {39, 44, 49, 54, 58, 63}},
    {0, L"Whole step down", 6, {38, 43, 48, 53, 57, 62}},
    {0, L"Drop C", 6, {36, 43, 48, 53, 57, 62}},
    {0, L"DADGAD", 6, {38, 45, 50, 55, 57, 62}},
    {0, L"Open D", 6, {38, 45, 50, 54, 57, 62}},
    {0, L"Open G", 6, {38, 43, 50, 55, 59, 62}},
    {0, L"Open E", 6, {40, 47, 52, 56, 59, 64}},
    {0, L"Open C", 6, {36, 43, 48, 52, 55, 60}},
    {0, L"All fourths", 6, {40, 45, 50, 55, 60, 65}},
    {0, L"Baritone", 6, {35, 40, 45, 50, 54, 59}},
    {0, L"7 string", 7, {35, 40, 45, 50, 55, 59, 64}},
    {0, L"8 string", 8, {30, 35, 40, 45, 50, 55, 59, 64}},
    // Bass.
    {1, L"4 string", 4, {28, 33, 38, 43}},
    {1, L"Drop D", 4, {26, 33, 38, 43}},
    {1, L"Half step down", 4, {27, 32, 37, 42}},
    {1, L"5 string", 5, {23, 28, 33, 38, 43}},
    {1, L"6 string", 6, {23, 28, 33, 38, 43, 48}},
    // Ukulele.
    {2, L"Standard (high G)", 4, {67, 60, 64, 69}},
    {2, L"Low G", 4, {55, 60, 64, 69}},
    {2, L"Baritone", 4, {50, 55, 59, 64}},
    {2, L"D tuning", 4, {69, 62, 66, 71}},
    // Mandolin.
    {3, L"Standard", 4, {55, 62, 69, 76}},
    {3, L"Cross AEAE", 4, {57, 64, 69, 76}},
    {3, L"GDAD (Irish)", 4, {55, 62, 69, 74}},
    // Five string banjo, drone last.
    {4, L"Open G", 5, {50, 55, 59, 62, 67}},
    {4, L"Double C", 5, {48, 55, 60, 62, 67}},
    {4, L"G modal", 5, {50, 55, 60, 62, 67}},
    // The violin family, low to high.
    {5, L"Standard", 4, {55, 62, 69, 76}},
    {6, L"Standard", 4, {48, 55, 62, 69}},
    {7, L"Standard", 4, {36, 43, 50, 57}},
    {8, L"Standard", 4, {28, 33, 38, 43}},
    // Bouzouki.
    {9, L"GDAD", 4, {43, 50, 57, 62}},
    {9, L"GDAE", 4, {43, 50, 57, 64}},
    // Lap steel and dobro.
    {10, L"C6", 6, {48, 52, 55, 57, 60, 64}},
    {10, L"Open G", 6, {43, 47, 50, 55, 59, 62}},
    {11, L"Open G", 6, {43, 47, 50, 55, 59, 62}},
};

constexpr int kTuningCount =
    static_cast<int>(sizeof(kTunings) / sizeof(kTunings[0]));

PCWSTR TunerInstrumentName(int instrument) {
    return instrument >= 0 && instrument < kTunerInstrumentCount
               ? kTunerInstruments[instrument]
               : L"Guitar";
}

int TunerStringCount(int tuning) {
    return tuning >= 0 && tuning < kTuningCount ? kTunings[tuning].strings : 0;
}

PCWSTR TunerTuningName(int tuning) {
    return tuning >= 0 && tuning < kTuningCount ? kTunings[tuning].name
                                                : L"Standard";
}

// The first tuning of an instrument, or -1 when it has none.
int TunerInstrumentFirst(int instrument) {
    for (int i = 0; i < kTuningCount; i++) {
        if (kTunings[i].instrument == instrument) {
            return i;
        }
    }
    return -1;
}

// How many tunings an instrument offers, and the entry at a list position.
// The select lists work in these positions, so they stay short no matter how
// many tunings the whole table holds.
int TunerTuningCount(int instrument) {
    const int first = TunerInstrumentFirst(instrument);
    int count = 0;
    if (first >= 0) {
        while (first + count < kTuningCount &&
               kTunings[first + count].instrument == instrument) {
            count++;
        }
    }
    return count;
}

int TunerTuningAt(int instrument, int index) {
    const int first = TunerInstrumentFirst(instrument);
    if (first < 0 || index < 0 || index >= TunerTuningCount(instrument)) {
        return -1;
    }
    return first + index;
}

// Where the current tuning sits in its instrument's list, so the select can
// open with its cursor on the row already chosen.
int TunerTuningPosition(int instrument, int tuning) {
    for (int i = 0; i < TunerTuningCount(instrument); i++) {
        if (TunerTuningAt(instrument, i) == tuning) {
            return i;
        }
    }
    return 0;
}

// The tallest list the tuner can show: guitar's fourteen tunings.
constexpr int kTunerMenuMax = 14;

constexpr PCWSTR kNoteNames[12] = {L"C", L"C#", L"D", L"D#", L"E", L"F",
                                   L"F#", L"G", L"G#", L"A", L"A#", L"B"};

std::wstring NoteLabel(int midi) {
    if (midi < 0) {
        return L"\u2014";
    }
    return std::wstring(kNoteNames[midi % 12]) +
           std::to_wstring(midi / 12 - 1);
}

// The nearest note to a frequency, and how far off it is in cents. The panel
// works this out from the reading, so the needle and the note always agree.
int NoteFromFrequency(float frequency, int reference, int& cents) {
    cents = 0;
    if (frequency <= 0.0f || reference <= 0) {
        return -1;
    }
    const double exact =
        69.0 + 12.0 * std::log2(static_cast<double>(frequency) /
                                 static_cast<double>(reference));
    const int nearest = static_cast<int>(lround(exact));
    cents = static_cast<int>(
        lround((exact - static_cast<double>(nearest)) * 100.0));
    return nearest;
}

// Tuner state. The audio thread owns the microphone; everything here is the
// panel's own view of the last reading it drew.
struct TunerState {
    int instrument = 0;
    int tuning = 0;
    int reference = 440;  // A4 in Hz, from the settings.
    bool listening = false;
    bool micFailed = false;
    float frequency = 0.0f;  // Smoothed, 0 when nothing is sounding.
    float level = 0.0f;
    double heardAt = 0.0;
    unsigned reading = 0;    // Counts the analyses the panel has drawn.
    int lock = -1;           // A string picked by hand, -1 to read them all.
    double lockAt = 0.0;     // When a string was picked by hand.
    bool tuned[kTunerStringMax] = {};  // Strings that have settled in tune.
    double stringAt[kTunerStringMax] = {};  // When each mark landed.
    float needle = 0.0f;    // Display cents, eased toward the reading.
    bool pulsing = false;   // The ring the needle plays when it lands in tune.
    double pulseAt = 0.0;
    int shownNote = -1;      // The note the panel is drawing.
    int fadingNote = -1;     // The note sliding out behind it.
    int pendingNote = -1;    // A note seen once, not yet believed.
    double noteAt = 0.0;     // When the shown note last changed.
};

TunerState g_tuner;

// The reading as the panel shows it: the note heard, and the cents the needle
// is drawn against. A locked string reads against its own target note, so
// tuning one string on demand works even next to a chord.
int TunerDisplayReading(int& cents) {
    int nearestCents = 0;
    const int midi =
        NoteFromFrequency(g_tuner.frequency, g_tuner.reference, nearestCents);
    cents = nearestCents;
    if (midi < 0) {
        return midi;
    }
    const int strings = TunerStringCount(g_tuner.tuning);
    if (g_tuner.lock >= 0 && g_tuner.lock < strings) {
        const int target = kTunings[g_tuner.tuning].notes[g_tuner.lock];
        const double expected =
            g_tuner.reference * std::pow(2.0, (target - 69) / 12.0);
        cents = static_cast<int>(
            lround(1200.0 * std::log2(g_tuner.frequency / expected)));
    }
    return midi;
}

// The string of the tuning the reading is closest to, or -1 when the note is
// more than a semitone from every one of them.
int TunerNearestString(int midi) {
    const int strings = TunerStringCount(g_tuner.tuning);
    int nearest = -1;
    int best = 100;
    for (int i = 0; i < strings; i++) {
        const int distance = midi - kTunings[g_tuner.tuning].notes[i];
        const int spread = distance < 0 ? -distance : distance;
        if (spread < best) {
            best = spread;
            nearest = i;
        }
    }
    return best > 1 ? -1 : nearest;
}

struct Settings {
    int theme = kThemeAuto;
    std::wstring accentChoice = L"accent";
    std::wstring customAccent;
    int opacity = 100;
    std::wstring position = L"right";
    int offsetX = 0;
    int offsetY = 0;
    bool primaryMonitorOnly = false;
    // Off by default, so the panel stays up while you play. The x button and
    // the shortcut close it; only turn this on for the old click-away habit.
    bool closeOnFocusLoss = false;
    // The order matches the Tool enum: chords, tempo, metronome, tuner,
    // strumming, circle of fifths.
    int defaultTool = 0;
    bool showFingerings = false;
    int defaultInstrument = 0;
    int tapResetMs = 3000;
    bool tapKeySpace = true;
    bool tapKeyEnter = true;
    int metronomeBeats = 4;
    int metronomeUnit = 4;
    int metronomeSubdivision = 1;
    int metronomeVolume = 70;
    bool metronomeKeepPlaying = true;
    int tunerReference = 440;  // A4 in Hz.
    int strumSubdivision = 2;
    bool strumCountIn = true;
    bool strumLoop = true;
    int strumRampStep = 2;
    int strumTargetOffset = 20;
    bool circleMinor = false;
    bool circlePlayOnSelect = true;
    int trainerSeconds = 5;  // Time allowed per note.
    bool trainerNaturals = false;
    // The toggle shortcut. Win+Y is the default, but any key with any mix of
    // modifiers can be set in the options.
    WORD hotkeyVk = 'Y';
    UINT hotkeyMods = MOD_WIN | MOD_NOREPEAT;
    std::wstring hotkeyText = L"Win+Y";
};

// Turns the key the shortcut settings name into a virtual-key code, so Win+Y
// can become any letter, digit, function key or common navigation key. Returns
// false for anything it cannot read, and gives back a tidy label for the
// panel's own hint text.
bool ParseHotkeyKey(const std::wstring& raw, WORD& vk, std::wstring& label) {
    std::wstring key;
    for (wchar_t c : raw) {
        if (!iswspace(c)) {
            key += static_cast<wchar_t>(towupper(c));
        }
    }
    if (key.empty()) {
        return false;
    }
    label = key;
    if (key == L"SPACE") {
        vk = VK_SPACE;
        return true;
    }
    if (key == L"TAB") {
        vk = VK_TAB;
        return true;
    }
    if (key == L"ENTER" || key == L"RETURN") {
        vk = VK_RETURN;
        label = L"Enter";
        return true;
    }
    if (key == L"ESC" || key == L"ESCAPE") {
        vk = VK_ESCAPE;
        label = L"Esc";
        return true;
    }
    if (key == L"BACKSPACE") {
        vk = VK_BACK;
        label = L"Backspace";
        return true;
    }
    if (key == L"INSERT" || key == L"INS") {
        vk = VK_INSERT;
        label = L"Insert";
        return true;
    }
    if (key == L"DELETE" || key == L"DEL") {
        vk = VK_DELETE;
        label = L"Delete";
        return true;
    }
    if (key == L"HOME") {
        vk = VK_HOME;
        return true;
    }
    if (key == L"END") {
        vk = VK_END;
        return true;
    }
    if (key == L"PAGEUP" || key == L"PGUP") {
        vk = VK_PRIOR;
        label = L"PgUp";
        return true;
    }
    if (key == L"PAGEDOWN" || key == L"PGDN") {
        vk = VK_NEXT;
        label = L"PgDn";
        return true;
    }
    if (key == L"LEFT" || key == L"LEFTARROW") {
        vk = VK_LEFT;
        label = L"Left";
        return true;
    }
    if (key == L"RIGHT" || key == L"RIGHTARROW") {
        vk = VK_RIGHT;
        label = L"Right";
        return true;
    }
    if (key == L"UP" || key == L"UPARROW") {
        vk = VK_UP;
        label = L"Up";
        return true;
    }
    if (key == L"DOWN" || key == L"DOWNARROW") {
        vk = VK_DOWN;
        label = L"Down";
        return true;
    }
    if (key.size() > 1 && key[0] == L'F') {
        const int number = _wtoi(key.c_str() + 1);
        if (number >= 1 && number <= 24) {
            vk = static_cast<WORD>(VK_F1 + number - 1);
            return true;
        }
        return false;
    }
    if (key.size() == 1) {
        const wchar_t c = key[0];
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            vk = static_cast<WORD>(c);
            return true;
        }
    }
    return false;
}

Settings LoadSettings() {
    Settings s;
    const std::wstring startTool = GetStringSetting(L"Startup.DefaultTool");
    s.defaultTool = startTool == L"tempo"   ? 1
                    : startTool == L"metronome" ? 2
                    : startTool == L"tuner"     ? 3
                    : startTool == L"strum"     ? 4
                    : startTool == L"circle"    ? 5
                    : startTool == L"trainer"   ? 6
                                                : 0;
    const std::wstring theme = GetStringSetting(L"Appearance.Theme");
    s.theme = theme == L"light" ? kThemeLight
              : theme == L"dark" ? kThemeDark
                                 : kThemeAuto;
    s.accentChoice = GetStringSetting(L"Appearance.AccentColor");
    if (s.accentChoice.empty()) {
        s.accentChoice = L"accent";
    }
    s.customAccent = GetStringSetting(L"Appearance.CustomAccentColor");
    s.opacity = std::clamp(Wh_GetIntSetting(L"Appearance.Opacity"), 40, 100);

    s.position = GetStringSetting(L"Placement.Position");
    if (s.position.empty()) {
        s.position = L"right";
    }
    s.offsetX = std::clamp(Wh_GetIntSetting(L"Placement.HorizontalOffset"),
                           -4000, 4000);
    s.offsetY = std::clamp(Wh_GetIntSetting(L"Placement.VerticalOffset"),
                           -4000, 4000);
    s.primaryMonitorOnly =
        GetStringSetting(L"Placement.Monitor") == L"primary";
    // Opt in, so a settings file written before this option - and the shipped
    // default - both leave the panel up when it loses focus.
    const std::wstring closeOnFocusLoss =
        GetStringSetting(L"Placement.CloseOnFocusLoss");
    s.closeOnFocusLoss =
        !closeOnFocusLoss.empty() &&
        Wh_GetIntSetting(L"Placement.CloseOnFocusLoss") != 0;
    // Opt out, so numbers stay on even where an older settings file is kept.
    s.showFingerings = Wh_GetIntSetting(L"Chords.HideFingerings") == 0;
    // The setting lives in the Startup group, among the other defaults.
    const std::wstring instrument =
        GetStringSetting(L"Startup.DefaultInstrument");
    s.defaultInstrument = 0;
    for (int i = 0; i < chorddata::kInstrumentsSize; i++) {
        std::wstring name = chorddata::kInstruments[i].name;
        for (wchar_t& letter : name) {
            letter = static_cast<wchar_t>(towlower(letter));
        }
        if (name == instrument) {
            s.defaultInstrument = i;
            break;
        }
    }

    // A settings file written before the tap tempo tool has no value here, and
    // a missing one reads back as zero, so fall back to the documented default.
    int resetAfter = Wh_GetIntSetting(L"TapTempo.ResetAfter");
    if (resetAfter <= 0) {
        resetAfter = 3;
    }
    s.tapResetMs = std::clamp(resetAfter, 1, 10) * 1000;
    const std::wstring tapKeys = GetStringSetting(L"TapTempo.TapKeys");
    s.tapKeySpace = tapKeys != L"enter";
    s.tapKeyEnter = tapKeys != L"space";

    const std::wstring signature = GetStringSetting(L"Metronome.Signature");
    s.metronomeBeats = 4;
    s.metronomeUnit = 4;
    for (int i = 0; i < kSignatureCount; i++) {
        if (signature == SignatureId(i)) {
            s.metronomeBeats = kSignatures[i].beats;
            s.metronomeUnit = kSignatures[i].unit;
            break;
        }
    }
    const std::wstring subdivision = GetStringSetting(L"Metronome.Subdivision");
    s.metronomeSubdivision = subdivision == L"eighth" ? 2
                             : subdivision == L"triplet" ? 3
                             : subdivision == L"sixteenth" ? 4
                                                           : 1;
    // A settings file written before the metronome exists has neither value, so
    // fall back to the documented defaults rather than to silence.
    const std::wstring volume = GetStringSetting(L"Metronome.Volume");
    s.metronomeVolume =
        volume.empty()
            ? 70
            : std::clamp(static_cast<int>(wcstol(volume.c_str(), nullptr, 10)),
                         0, 100);
    const std::wstring keepPlaying = GetStringSetting(L"Metronome.KeepPlaying");
    s.metronomeKeepPlaying =
        keepPlaying.empty() ? true
                            : Wh_GetIntSetting(L"Metronome.KeepPlaying") != 0;
    // A settings file written before the tuner exists reads back as zero, so
    // fall back to the concert standard rather than to nonsense.
    const int reference = Wh_GetIntSetting(L"Tuner.ReferencePitch");
    s.tunerReference = reference == 0 ? 440 : std::clamp(reference, 415, 466);

    // The strumming defaults. A settings file written before this tool reads
    // back as zero, so every value falls through to the documented default;
    // the empty-string guard is what lets the booleans stay on for such a file.
    const int strumSubdivision = Wh_GetIntSetting(L"Strumming.Subdivision");
    s.strumSubdivision = std::clamp(strumSubdivision == 0 ? 2 : strumSubdivision,
                                    1, 4);
    const std::wstring strumCountIn = GetStringSetting(L"Strumming.CountIn");
    s.strumCountIn =
        strumCountIn.empty() ? true : Wh_GetIntSetting(L"Strumming.CountIn") != 0;
    const std::wstring strumLoop = GetStringSetting(L"Strumming.Loop");
    s.strumLoop =
        strumLoop.empty() ? true : Wh_GetIntSetting(L"Strumming.Loop") != 0;
    const int strumRampStep = Wh_GetIntSetting(L"Strumming.RampStep");
    s.strumRampStep = std::clamp(strumRampStep == 0 ? 2 : strumRampStep, 1, 10);
    const int strumTargetOffset = Wh_GetIntSetting(L"Strumming.TargetOffset");
    s.strumTargetOffset =
        std::clamp(strumTargetOffset == 0 ? 20 : strumTargetOffset, 0, 80);

    // The circle of fifths defaults. A settings file written before this tool
    // has no Circle.Mode, so it reads back empty and stays major; PlayOnSelect
    // uses the same empty-string guard to stay on for such a file.
    s.circleMinor = GetStringSetting(L"Circle.Mode") == L"minor";
    const std::wstring circlePlayOnSelect =
        GetStringSetting(L"Circle.PlayOnSelect");
    s.circlePlayOnSelect = circlePlayOnSelect.empty()
                               ? true
                               : Wh_GetIntSetting(L"Circle.PlayOnSelect") != 0;

    // The note trainer defaults. A file written before this tool has no
    // NoteTrainer.Seconds, so it reads back as zero and keeps five seconds.
    const int trainerSeconds = Wh_GetIntSetting(L"NoteTrainer.Seconds");
    s.trainerSeconds =
        std::clamp(trainerSeconds == 0 ? 5 : trainerSeconds, 2, 20);
    const std::wstring trainerNoteSet = GetStringSetting(L"NoteTrainer.NoteSet");
    s.trainerNaturals = trainerNoteSet == L"naturals";

    // The toggle shortcut. The key defaults to Y and Win is the default
    // modifier, so an older settings file keeps opening on Win+Y. Reading the
    // booleans through the empty-string guard is what lets Win stay on for a
    // file written before these options existed.
    WORD hotkeyVk = 'Y';
    std::wstring keyLabel = L"Y";
    const std::wstring keyText = GetStringSetting(L"Shortcut.HotkeyKey");
    if (!keyText.empty()) {
        WORD parsed = 0;
        std::wstring parsedLabel;
        if (ParseHotkeyKey(keyText, parsed, parsedLabel)) {
            hotkeyVk = parsed;
            keyLabel = parsedLabel;
        }
    }
    const std::wstring winText = GetStringSetting(L"Shortcut.HotkeyWin");
    const bool useWin =
        winText.empty() ? true : Wh_GetIntSetting(L"Shortcut.HotkeyWin") != 0;
    const bool useCtrl = Wh_GetIntSetting(L"Shortcut.HotkeyCtrl") != 0;
    const bool useAlt = Wh_GetIntSetting(L"Shortcut.HotkeyAlt") != 0;
    const bool useShift = Wh_GetIntSetting(L"Shortcut.HotkeyShift") != 0;
    UINT mods = MOD_NOREPEAT;
    if (useWin) {
        mods |= MOD_WIN;
    }
    if (useCtrl) {
        mods |= MOD_CONTROL;
    }
    if (useAlt) {
        mods |= MOD_ALT;
    }
    if (useShift) {
        mods |= MOD_SHIFT;
    }
    if (!(mods & (MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_SHIFT))) {
        // RegisterHotKey wants a real modifier, so keep the Windows key rather
        // than register a shortcut that could never fire.
        mods |= MOD_WIN;
    }
    s.hotkeyVk = hotkeyVk;
    s.hotkeyMods = mods;
    std::wstring display;
    if (mods & MOD_CONTROL) {
        display += L"Ctrl+";
    }
    if (mods & MOD_ALT) {
        display += L"Alt+";
    }
    if (mods & MOD_SHIFT) {
        display += L"Shift+";
    }
    if (mods & MOD_WIN) {
        display += L"Win+";
    }
    display += keyLabel;
    s.hotkeyText = display;
    return s;
}

Settings g_settings;
Color g_accent = kFallbackAccent;

bool UseDarkTheme() {
    switch (g_settings.theme) {
        case kThemeLight:
            return false;
        case kThemeDark:
            return true;
        default:
            break;
    }
    return SystemUsesDarkTheme();
}

////////////////////////////////////////////////////////////////////////////////
// Theme

struct Theme {
    bool dark = true;
    Color text;
    Color textMuted;
    Color textFaint;
    Color card;
    Color control;
    Color controlHover;
    Color selection;
    Color stroke;
    Color menu;  // Opaque, so an open list hides the panel behind it.
    Color accent;
    Color onAccent;
    Color panelTop;
    Color panelBottom;
    Color grid;
    Color shadow;
};

float Luminance(const Color& c) {
    return c.r * 0.299f + c.g * 0.587f + c.b * 0.114f;
}

Theme MakeTheme(bool dark, Color accent) {
    Theme t;
    t.dark = dark;
    t.accent = accent;
    t.onAccent = Luminance(accent) > 0.62f ? kBlack : kWhite;
    if (dark) {
        t.text = WithAlpha(kWhite, 0.96f);
        t.textMuted = WithAlpha(kWhite, 0.62f);
        t.textFaint = WithAlpha(kWhite, 0.40f);
        t.card = WithAlpha(kWhite, 0.045f);
        t.control = WithAlpha(kWhite, 0.065f);
        t.controlHover = WithAlpha(kWhite, 0.11f);
        t.selection = WithAlpha(accent, 0.22f);
        t.stroke = WithAlpha(kWhite, 0.09f);
        t.menu = Rgb(42, 44, 51, 1.0f);
        t.grid = WithAlpha(kWhite, 0.30f);
        t.panelTop = Rgb(30, 31, 37, 0.88f);
        t.panelBottom = Rgb(22, 23, 28, 0.90f);
        t.shadow = WithAlpha(kBlack, 0.40f);
    } else {
        t.text = Rgb(16, 18, 22, 0.96f);
        t.textMuted = Rgb(32, 36, 44, 0.62f);
        t.textFaint = Rgb(32, 36, 44, 0.42f);
        t.card = Rgb(0, 0, 0, 0.035f);
        t.control = Rgb(0, 0, 0, 0.05f);
        t.controlHover = Rgb(0, 0, 0, 0.09f);
        t.selection = WithAlpha(accent, 0.18f);
        t.stroke = Rgb(0, 0, 0, 0.08f);
        t.menu = Rgb(252, 252, 255, 1.0f);
        t.grid = Rgb(0, 0, 0, 0.28f);
        t.panelTop = Rgb(250, 250, 253, 0.90f);
        t.panelBottom = Rgb(243, 244, 248, 0.92f);
        t.shadow = WithAlpha(kBlack, 0.22f);
    }
    return t;
}

////////////////////////////////////////////////////////////////////////////////
// Flyout geometry

// Everything below is laid out in device independent pixels and scaled by the
// DPI of the monitor the panel opens on.
constexpr int kPanelWidth = 452;
constexpr int kPanelHeight = 520;
constexpr int kPanelPad = 14;
constexpr float kPanelRadius = 12.0f;
constexpr int kHeaderTop = 12;
constexpr int kHeaderHeight = 26;
constexpr int kSearchTop = 44;
constexpr int kSearchHeight = 34;
constexpr float kControlRadius = 8.0f;
constexpr int kListTop = 86;
constexpr int kRowHeight = 30;
constexpr int kVisibleRows = 5;
constexpr int kCardTop = 242;
constexpr int kCardHeight = 240;
constexpr float kCardRadius = 10.0f;
constexpr int kVoicingStripHeight = 26;
constexpr int kFootTop = 486;
constexpr int kFootHeight = 20;

constexpr int kFontTab = 11;
constexpr int kFontSearch = 13;
constexpr int kFontRow = 13;
constexpr int kFontChord = 22;
constexpr int kFontTone = 11;
constexpr int kFontFoot = 10;
constexpr int kFontFret = 9;
constexpr int kFontBpm = 46;
constexpr int kFontBpmUnit = 11;
constexpr int kWeightSemiBold = 600;

float g_scale = 1.0f;

// Strumming vocabulary, kept here because the layout reserves compile-time
// sized arrays for the grid, the presets, the strings and the switches. A
// pattern is one measure laid over the beat grid the metronome already walks:
// beats * subdivision cells, each holding a stroke. A stroke has a direction
// and a state (open or muted scratch); the per-cell modifiers carry the rest
// of the vocabulary of real strum notation: an accent (>), a ghost or partial
// strum, and a tie that lets the previous stroke ring instead of striking
// again. The instrument, the chord and the sound are whole-pattern switches.
constexpr int kStrumSlotsMax = 32;

enum StrumStroke {
    kStrokeRest,
    kStrokeDown,
    kStrokeUp,
    kStrokeDownMute,
    kStrokeUpMute,
    kStrokeCount,
};

enum StrumChip {
    kChipLoop,
    kChipCountIn,
    kChipPractice,
    kStrumChipCount,
};

// What a stroke rings like. The three sit in the order of damping, so a stroke
// written as a mute can simply take the heavier of the two without a second
// table: max(sound, stroke).
enum StrumSound {
    kStrumOpen,
    kStrumPalm,
    kStrumMuted,
    kStrumSoundCount,
};

// The chords the strum picker offers: the instrument's own open strings first,
// then the open position chords most practice runs lean on. A chord is matched
// to the library by pitch class and mode rather than by name, so the spelling
// an instrument uses (F# on a guitar, Gb on a ukulele) never has to be written
// down twice.
struct StrumChord {
    PCWSTR name;
    int pitch;  // Pitch class, or -1 for the open strings.
    bool minor;
};

constexpr StrumChord kStrumChords[] = {
    {L"Open strings", -1, false},
    {L"C", 0, false},  {L"Am", 9, true},
    {L"G", 7, false},  {L"Em", 4, true},
    {L"D", 2, false},  {L"Dm", 2, true},
    {L"A", 9, false},  {L"E", 4, false},
    {L"F", 5, false},
};
constexpr int kStrumChordCount =
    static_cast<int>(sizeof(kStrumChords) / sizeof(kStrumChords[0]));

struct StrumPreset {
    PCWSTR name;
    int beats;
    int unit;
    int subdivision;
    PCWSTR cells;
};

// One row of cells per beat * subdivision. Lowercase is a plain stroke,
// uppercase an accent; d/u are arrows, x/c a muted scratch down or up.
//
// The table is grouped by meter, in the order kSignatures lists them, and
// inside a meter by grid, thinnest first: a run of the table is a meter's whole
// vocabulary, and the pattern list puts a heading in front of every change of
// grid. Picking a row also switches the grid chips to the grid that row is
// written in, so a pattern always arrives with the cells it was notated for.
//
// The patterns are the ones players actually reach for: the count itself, the
// steady eighths, the two strums every method book opens with - D DU D DU and
// the folk D DU UDU - then the pop D D D U D U, the backbeat, the reggae skank
// and the muted funk chug; the triplet grid holds the shuffle, and straight
// sixteenths close each meter. A stroke lands down on the beat and up on the
// off-beat, so every row alternates the way a hand really moves. A boom-chick
// is left out on purpose: it is one hand alternating a bass note with a chord,
// which a single voice cannot state honestly.
//
// The odd and compound meters list the groupings their bars are counted in -
// 5/4 as 3+2 or 2+3, 7/8 as 2+2+3 or 3+2+2, and 9/8 and 12/8 as three and four
// groups of three - and keep to the two grids that belong to them, because an
// eighth of 6/8 is already the triplet those meters are felt in and six to an
// eighth is not a strum anyone plays.
constexpr StrumPreset kStrumPresets[] = {
    // 4/4 - common time, where most of the vocabulary lives: the count, the
    // eighth-note engine, the two strums every method book opens with, the pop
    // strum, the backbeat, the off-beat skank and the muted chug.
    {L"Downstrokes", 4, 4, 1, L"Dddd"},
    {L"Eighths", 4, 4, 2, L"dudududu"},
    {L"Down-up", 4, 4, 2, L"d-dud-du"},
    {L"Folk", 4, 4, 2, L"d-du-udu"},
    {L"Pop", 4, 4, 2, L"d-d-dudu"},
    {L"Backbeat", 4, 4, 2, L"d-D-d-D-"},
    {L"Skank", 4, 4, 2, L"-u-u-u-u"},
    {L"Chug", 4, 4, 2, L"xcxcxcxc"},
    {L"Shuffle", 4, 4, 3, L"d-ud-ud-ud-u"},
    {L"Sixteenths", 4, 4, 4, L"dudududududududu"},
    // 3/4 - the waltz: the bass of the count on one, two lighter strokes after
    // it, said once a beat and then again in eighths with upstrokes between.
    {L"Downstrokes", 3, 4, 1, L"Ddd"},
    {L"Waltz", 3, 4, 2, L"d-dudu"},
    {L"Eighths", 3, 4, 2, L"dududu"},
    {L"Shuffle", 3, 4, 3, L"d-ud-ud-u"},
    {L"Sixteenths", 3, 4, 4, L"dudududududu"},
    // 2/4 - march and polka.
    {L"Downstrokes", 2, 4, 1, L"Dd"},
    {L"Eighths", 2, 4, 2, L"dudu"},
    {L"Off-beats", 2, 4, 2, L"-u-u"},
    {L"Shuffle", 2, 4, 3, L"d-ud-u"},
    {L"Sixteenths", 2, 4, 4, L"dudududu"},
    // 5/4 - counted three then two, or two then three.
    {L"Three-two", 5, 4, 1, L"DddDd"},
    {L"Two-three", 5, 4, 1, L"DdDdd"},
    {L"Eighths", 5, 4, 2, L"dududududu"},
    {L"Shuffle", 5, 4, 3, L"d-ud-ud-ud-ud-u"},
    {L"Sixteenths", 5, 4, 4, L"dudududududududududu"},
    // 6/8 - two beats of three; the ballad leaves the middle eighth out.
    {L"Downstrokes", 6, 8, 1, L"D--d--"},
    {L"Ballad", 6, 8, 1, L"d-Ud-U"},
    {L"Eighths", 6, 8, 1, L"dududu"},
    {L"Sixteenths", 6, 8, 2, L"dudududududu"},
    // 7/8 - the two groupings the bar is normally counted in.
    {L"Two-two-three", 7, 8, 1, L"DdDdDdd"},
    {L"Three-two-two", 7, 8, 1, L"DddDdDd"},
    {L"Sixteenths", 7, 8, 2, L"dududududududu"},
    // 9/8 - three beats of three, so it rolls like a slow three-four.
    {L"Downstrokes", 9, 8, 1, L"D--d--d--"},
    {L"Ballad", 9, 8, 1, L"d-Ud-Ud-U"},
    {L"Sixteenths", 9, 8, 2, L"dududududududududu"},
    // 12/8 - four beats of three: the slow blues and the triplet shuffle.
    {L"Downstrokes", 12, 8, 1, L"D--d--d--d--"},
    {L"Shuffle", 12, 8, 1, L"d-ud-ud-ud-u"},
    {L"Sixteenths", 12, 8, 2, L"dudududududududududududu"},
};

constexpr int kStrumPresetCount =
    static_cast<int>(sizeof(kStrumPresets) / sizeof(kStrumPresets[0]));

// The run of the table that belongs to one meter. -1 when the meter has no
// patterns, which is what leaves the list empty rather than lying about it.
int StrumPatternBase(int beats, int unit) {
    for (int i = 0; i < kStrumPresetCount; i++) {
        if (kStrumPresets[i].beats == beats && kStrumPresets[i].unit == unit) {
            return i;
        }
    }
    return -1;
}

int StrumPatternCount(int beats, int unit) {
    const int base = StrumPatternBase(beats, unit);
    if (base < 0) {
        return 0;
    }
    int count = 0;
    while (base + count < kStrumPresetCount &&
           kStrumPresets[base + count].beats == beats &&
           kStrumPresets[base + count].unit == unit) {
        count++;
    }
    return count;
}

// The longest run in the table. The card is laid out before the strum state is
// read, so the list is given the room its busiest meter needs and each meter's
// list is cut down to its own rows when it is painted.
constexpr int StrumPatternRowsMax() {
    int best = 1;
    int run = 1;
    for (int i = 1; i < kStrumPresetCount; i++) {
        if (kStrumPresets[i].beats == kStrumPresets[i - 1].beats &&
            kStrumPresets[i].unit == kStrumPresets[i - 1].unit) {
            run++;
            if (run > best) {
                best = run;
            }
        } else {
            run = 1;
        }
    }
    return best;
}

// The four grids a pattern can be written in, in the order the list reads them.
constexpr int kStrumGridCount = 4;

// The roomiest list: the longest run of patterns plus one heading per grid.
// The card is laid out before the strum state is read, so the list is given
// that room and each meter's list is cut to its own lines when it is painted.
constexpr int kStrumPatternEntriesMax =
    StrumPatternRowsMax() + kStrumGridCount;

// One line of the pattern list: either a heading that names the grid the rows
// under it are written in, or a pattern. Because the table is ordered grid by
// grid inside a meter, the list is the meter's run with a heading in front of
// every change of grid.
enum StrumEntryKind {
    kStrumEntryHeading,
    kStrumEntryPattern,
};

struct StrumEntry {
    int kind;
    int preset;  // A table index for a pattern, the grid value for a heading.
};

int StrumPatternEntries(StrumEntry* out, int capacity, int beats, int unit) {
    const int base = StrumPatternBase(beats, unit);
    const int count = StrumPatternCount(beats, unit);
    if (base < 0 || count == 0 || capacity <= 0) {
        return 0;
    }
    int used = 0;
    int grid = 0;
    for (int i = 0; i < count; i++) {
        const StrumPreset& p = kStrumPresets[base + i];
        if (p.subdivision != grid) {
            grid = p.subdivision;
            // A heading without its first pattern is not a list.
            if (used + 2 > capacity) {
                break;
            }
            out[used].kind = kStrumEntryHeading;
            out[used].preset = grid;
            used++;
        }
        if (used >= capacity) {
            break;
        }
        out[used].kind = kStrumEntryPattern;
        out[used].preset = base + i;
        used++;
    }
    return used;
}

// How a meter reads in the meter list: the name players know it by, or the
// grouping an odd bar is counted in.
PCWSTR MeterNote(int beats, int unit) {
    if (unit >= 8) {
        switch (beats) {
            case 6:
                return L"compound, two beats";
            case 7:
                return L"odd, 2+2+3";
            case 9:
                return L"compound, three beats";
            case 12:
                return L"compound, four beats";
            default:
                break;
        }
        return L"compound";
    }
    switch (beats) {
        case 2:
            return L"march";
        case 3:
            return L"waltz";
        case 4:
            return L"common time";
        case 5:
            return L"odd, 3+2";
        default:
            break;
    }
    return L"";
}

////////////////////////////////////////////////////////////////////////////////
// Circle of fifths
//
// Twelve positions clockwise from twelve o'clock, each a major key one fifth
// from the last. Every fact the tool shows - the scale degrees, the diatonic
// chords, the key signature, the relative and parallel minor and the mode
// names - is spelled from these few numbers, so nothing is hard-coded beyond
// the table. letter is the tonic's note name (0 C, 1 D, 2 E, 3 F, 4 G, 5 A,
// 6 B), which is what fixes whether a degree reads as a sharp or a flat.

struct CircleKey {
    int pc;
    int letter;
    int fifths;
    PCWSTR name;
};

constexpr CircleKey kCircleKeys[12] = {
    {0, 0, 0, L"C"},    {7, 4, 1, L"G"},    {2, 1, 2, L"D"},
    {9, 5, 3, L"A"},    {4, 2, 4, L"E"},    {11, 6, 5, L"B"},
    {6, 3, 6, L"F#"},   {1, 1, -5, L"Db"},  {8, 5, -4, L"Ab"},
    {3, 2, -3, L"Eb"},  {10, 6, -2, L"Bb"}, {5, 3, -1, L"F"},
};

// The major scale's semitone offsets from the tonic, degree by degree.
constexpr int kCircleMajorSteps[7] = {0, 2, 4, 5, 7, 9, 11};
// The natural minor scale, which is the same seven notes read from the sixth
// degree. The wheel carries both at once, so both tables are needed.
constexpr int kCircleMinorSteps[7] = {0, 2, 3, 5, 7, 8, 10};
// The triad each degree builds in a major key: 0 major, 1 minor, 2 diminished.
constexpr int kCircleChordQuality[7] = {0, 1, 1, 0, 0, 1, 2};
// The natural minor builds i ii III iv v VI VII, so its qualities differ.
constexpr int kCircleMinorQuality[7] = {1, 2, 0, 1, 1, 0, 0};

// A numeral the wheel can print on a wedge, and the distance in semitones from
// the tonic that earns it. Only the chords of the selected key carry one.
struct CircleNumeral {
    int semitones;
    PCWSTR mark;
};

// A major key lights up I, IV and V on the outer ring and ii, iii and vi on the
// inner one; a minor key turns that inside out, because its tonic is the inner
// wedge and its mediant, submediant and subtonic are majors outside.
constexpr CircleNumeral kCircleMajorOuter[3] = {
    {0, L"I"}, {5, L"IV"}, {7, L"V"}};
constexpr CircleNumeral kCircleMajorInner[3] = {
    {2, L"ii"}, {4, L"iii"}, {9, L"vi"}};
constexpr CircleNumeral kCircleMinorOuter[3] = {
    {3, L"III"}, {8, L"VI"}, {10, L"VII"}};
constexpr CircleNumeral kCircleMinorInner[3] = {
    {0, L"i"}, {5, L"iv"}, {7, L"v"}};

// Base pitch class of a note letter, before any accidental.
int CircleLetterPc(int letter) {
    static const int kBase[7] = {0, 2, 4, 5, 7, 9, 11};  // C D E F G A B
    return kBase[((letter % 7) + 7) % 7];
}

// A single or double sharp or flat, or natural, from a -2..+2 offset.
PCWSTR CircleAccidental(int delta) {
    switch (delta) {
        case -2:
            return L"\x266D\x266D";  // double flat
        case -1:
            return L"\x266D";  // flat
        case 1:
            return L"#";
        case 2:
            return L"x";  // double sharp
        default:
            return L"";
    }
}

// The name of one degree of the key's major scale, spelled correctly. Walking
// the letters a step at a time and choosing the accidental that lands on the
// scale's pitch class is what keeps G major's seventh as F#, not Gb.
std::wstring CircleDegreeName(int index, int degree) {
    const CircleKey& key = kCircleKeys[index];
    const int step = ((degree % 7) + 7) % 7;
    const int letter = (key.letter + step) % 7;
    const int target = (key.pc + kCircleMajorSteps[step]) % 12;
    int delta = (target - CircleLetterPc(letter) + 12) % 12;
    if (delta > 6) {
        delta -= 12;
    }
    static const PCWSTR kLetters[7] = {L"C", L"D", L"E", L"F",
                                       L"G", L"A", L"B"};
    return std::wstring(kLetters[letter]) + CircleAccidental(delta);
}

// The label on a wedge of the inner ring: the relative minor of that key, which
// is its sixth degree and needs no spelling of its own.
std::wstring CircleMinorRingName(int index) {
    return CircleDegreeName(index, 5) + L"m";
}

// The accidentals a signature carries, in the standard order, as a spaced
// string. A positive count is sharps, a negative one flats.
std::wstring CircleSignatureList(int fifths) {
    static const PCWSTR kSharps[7] = {L"F#", L"C#", L"G#", L"D#",
                                      L"A#", L"E#", L"B#"};
    static const PCWSTR kFlats[7] = {L"Bb", L"Eb", L"Ab", L"Db",
                                     L"Gb", L"Cb", L"Fb"};
    std::wstring out;
    const int count = fifths > 0 ? fifths : -fifths;
    const PCWSTR* names = fifths > 0 ? kSharps : kFlats;
    for (int i = 0; i < count && i < 7; i++) {
        if (i) {
            out += L" ";
        }
        out += names[i];
    }
    return out;
}

PCWSTR CircleModeName(int degree) {
    static const PCWSTR kModes[7] = {
        L"Ionian",    L"Dorian",   L"Phrygian", L"Lydian",
        L"Mixolydian", L"Aeolian", L"Locrian"};
    return kModes[degree];
}

////////////////////////////////////////////////////////////////////////////////
// Note trainer
//
// A target note appears at random; the player finds it on their instrument and
// the microphone (the tuner's, reused here) recognises it before the live timer
// runs out. Each instrument carries a range to sample from, naturals mode keeps
// the drill to the white notes, and the score rewards reading the geography
// quickly with a speed bonus on top of a plain hit.

struct TrainerRange {
    int minMidi;  // Lowest note in the drill.
    int span;     // Semitones from there up to the highest.
};

// Indexed by the Instrument enum: guitar, ukulele, piano. The ranges sit where
// each instrument is normally first read, so a beginner never reaches past the
// board on a random prompt.
constexpr TrainerRange kTrainerRanges[kInstrumentCount] = {
    {40, 26},  // Guitar:  low E2 across the first position and beyond.
    {60, 18},  // Ukulele: middle C over the fingerboard.
    {48, 25},  // Piano:   C3 up to C5.
};
constexpr PCWSTR kInstrumentChipLabels[kInstrumentCount] = {
    L"Guitar", L"Ukulele", L"Piano"};
// The three instruments as chips, in the order the note trainer and the circle
// of fifths draw them: guitar, piano, ukulele.
constexpr int kInstrumentChipOrder[3] = {kInstrumentGuitar, kInstrumentPiano,
                                         kInstrumentUkulele};
constexpr int kInstrumentChipCount = 3;

// A reading within this many cents of the target counts as the right note.
constexpr int kTrainerCents = 40;
// Matching ticks in a row needed before a note is scored, so a single doubtful
// frame cannot bank a hit.
constexpr int kTrainerStable = 2;
constexpr double kTrainerResultFlashMs = 750.0;

// Pitch classes with no accidental: C D E F G A B.
bool TrainerIsNatural(int midi) {
    static const bool kNatural[12] = {true,  false, true,  false, true, true,
                                      false, true,  false, true,  false, true};
    return kNatural[((midi % 12) + 12) % 12];
}

// The trainer's live state. The microphone thread only ever publishes a
// frequency; everything here is the panel's running view of the round, touched
// from the timer tick and the paint.
struct NoteTrainerState {
    bool running = false;       // A round is being played.
    bool listening = false;     // The trainer is holding the microphone open.
    bool micFailed = false;     // No input device, so detection cannot work.
    bool naturals = false;      // Restrict prompts to the white notes.
    int seconds = 5;            // Per-note time limit, from the settings.
    int instrument = kInstrumentGuitar;  // Which range to drill.
    int target = -1;            // The MIDI note to find now.
    double noteEndsAt = 0.0;    // When this prompt's timer runs out.
    double roundMs = 5000.0;    // noteEndsAt - start, kept for the bar.
    int stable = 0;             // Consecutive ticks that matched the target.
    int heard = -1;             // The last clear reading, for the live display.
    int correct = 0;            // Hits this session.
    int total = 0;              // Prompts resolved this session.
    int score = 0;              // Speed-weighted points.
    int streak = 0;             // Hits in a row.
    int bestStreak = 0;         // Longest run so far.
    int result = 0;             // 0 idle, 1 just hit, 2 just missed.
    double resultAt = -1000.0;  // When the last result landed, for the flash.
    int lastPoints = 0;         // Points the last hit scored.
    bool seeded = false;        // rand() has been given a start value.
    bool defaultsApplied = false;
};

NoteTrainerState g_trainer;

enum Tool {
    kToolChords,
    kToolTempo,
    kToolMetronome,
    kToolTuner,
    kToolStrum,
    kToolCircle,
    kToolTrainer,
    kToolCount,
};

PCWSTR ToolLabel(int tool) {
    switch (tool) {
        case kToolTempo:
            return L"Tap tempo";
        case kToolMetronome:
            return L"Metronome";
        case kToolTuner:
            return L"Tuner";
        case kToolStrum:
            return L"Strumming";
        case kToolCircle:
            return L"Circle of fifths";
        case kToolTrainer:
            return L"Note trainer";
        default:
            break;
    }
    return L"Chords";
}

enum Hit {
    kHitNone,
    kHitHeader,
    kHitClose,
    kHitTab,
    kHitToolSelect,
    kHitToolMenuItem,
    kHitSearch,
    kHitRow,
    kHitPrevious,
    kHitNext,
    kHitDot,
    kHitDiagram,
    kHitSpeaker,
    kHitTapPad,
    kHitTapReset,
    kHitTapHalf,
    kHitTapDouble,
    kHitMetroStart,
    kHitMetroTap,
    kHitMetroTempo,
    kHitMetroPreset,
    kHitMetroBeatsDown,
    kHitMetroBeatsUp,
    kHitMetroUnit,
    kHitMetroSubdivision,
    kHitMetroGroup,
    kHitTunerInst,
    kHitTunerTune,
    kHitTunerMenuRow,
    kHitTunerString,
    kHitStrumSlot,
    kHitStrumPattern,
    kHitStrumPatternRow,
    kHitStrumMeter,
    kHitStrumMeterRow,
    kHitStrumSub,
    kHitStrumPlay,
    kHitStrumChip,
    kHitStrumInst,
    kHitStrumSound,
    kHitStrumChord,
    kHitStrumChordRow,
    kHitStrumTempo,
    kHitStrumTempoDown,
    kHitStrumTempoUp,
    kHitCircleKey,
    kHitCircleInst,
    kHitCircleChord,
    kHitTrainerStart,
    kHitTrainerInst,
    kHitTrainerNaturals,
};

struct HitResult {
    int id = kHitNone;
    int index = -1;
};

struct Layout {
    int width = 0;
    int height = 0;
    RECT header{};
    RECT close{};
    RECT tabs[kInstrumentCount]{};
    RECT search{};
    RECT searchText{};
    RECT list{};
    RECT rows{};
    RECT card{};
    RECT chordName{};
    RECT speaker{};
    RECT tones{};
    RECT diagram{};
    RECT previous{};
    RECT next{};
    RECT dots[8]{};
    int dotCount = 0;
    RECT foot{};
    int rowHeight = 0;

    // The tool picker in the header: one button, and the list it opens.
    RECT toolSelect{};
    RECT toolMenu{};
    RECT toolMenuRows[kToolCount]{};

    // Tap tempo tool. The pad covers the space the search, list and card take
    // in the chord tool, so the panel keeps one size.
    RECT tapPad{};
    RECT tapValue{};
    RECT tapName{};
    RECT tapBars{};
    RECT tapHint{};
    RECT tapHalf{};
    RECT tapReset{};
    RECT tapDouble{};

    // Metronome tool, laid out inside the same card rectangle.
    RECT metroPad{};
    RECT metroValue{};
    RECT metroMeta{};
    RECT metroDots{};
    RECT metroPreset[kSignatureCount]{};
    RECT metroBeatsDown{};
    RECT metroBeatsLabel{};
    RECT metroBeatsUp{};
    RECT metroUnit[4]{};
    RECT metroSub[4]{};
    RECT metroGroup{};
    RECT metroTap{};
    RECT metroStart{};
    RECT metroHint{};

    // Tuner tool, in the same rectangle once more. The needle sweeps the arc
    // in the middle, the note and the cents sit inside it, and the instrument
    // and tuning are two labelled listbox selects whose lists drop over the
    // arc.
    RECT tunerPad{};
    RECT tunerArc{};
    RECT tunerNote{};
    RECT tunerCents{};
    RECT tunerMeta{};
    RECT tunerStrings{};
    RECT tunerStringChip[kTunerStringMax]{};
    RECT tunerInstCaption{};
    RECT tunerTuneCaption{};
    RECT tunerInstSelect{};
    RECT tunerTuneSelect{};
    RECT tunerInstMenu{};
    RECT tunerInstMenuRow[kTunerMenuMax]{};
    RECT tunerTuneMenu{};
    RECT tunerTuneMenuRow[kTunerMenuMax]{};
    RECT tunerHint{};

    // Strumming tool, in the same rectangle once more. A pattern select on top
    // that unrolls the list of common patterns, the pattern grid, a meter and
    // subdivision row, a transport row (play plus a tempo stepper), a row
    // picking the strings (the instrument and the chord), a row picking what
    // they ring like, the practice switches and a hint line.
    RECT strumPad{};
    RECT strumPattern{};
    RECT strumPatternMenu{};
    RECT strumPatternRow[kStrumPatternEntriesMax]{};
    RECT strumGrid{};
    RECT strumSlot[kStrumSlotsMax]{};
    RECT strumMeterSelect{};
    RECT strumMeterMenu{};
    RECT strumMeterRow[kSignatureCount]{};
    RECT strumSub[4]{};
    RECT strumPlay{};
    RECT strumInst[2]{};
    RECT strumChord{};
    RECT strumChordMenu{};
    RECT strumChordRow[kStrumChordCount]{};
    RECT strumSound[kStrumSoundCount]{};
    RECT strumChip[kStrumChipCount]{};
    RECT strumTempo{};
    RECT strumTempoDown{};
    RECT strumTempoUp{};
    RECT strumHint{};

    // Circle of fifths tool, in the same rectangle once more. A wheel of two
    // rings - the twelve majors outside, their relative minors inside - round a
    // centre readout, the instrument chips above it, then a row of the seven
    // diatonic chords and a block of theory lines below.
    RECT circlePad{};
    RECT circleInst[3]{};
    RECT circleChord[7]{};
    RECT circleCaption{};
    RECT circleCenter{};
    RECT circleSignature{};
    RECT circleMinor{};
    RECT circleDetail{};
    RECT circleHint{};
    int circleCx = 0;
    int circleCy = 0;
    int circleRingR = 0;   // Outer edge of the ring of majors.
    int circleMidR = 0;    // Edge between the two rings.
    int circleCoreR = 0;   // The hole the readout sits in.

    // Note trainer, in the same rectangle once more. A row of instrument chips
    // and a Start button on top, the big target note in the middle, the note
    // heard and a countdown bar under it, then a line of score and streak.
    RECT trainerPad{};
    RECT trainerInst[3]{};
    RECT trainerStart{};
    RECT trainerNaturals{};
    RECT trainerPrompt{};
    RECT trainerPlayed{};
    RECT trainerBar{};
    RECT trainerFeedback{};
    RECT trainerScore{};
    RECT trainerStreak{};
    RECT trainerHint{};
};

int S(float dip) {
    return static_cast<int>(lroundf(dip * g_scale));
}

Layout g_layout;

// Defined with the strumming state further down, but the layout needs it.
int StrumSlotCount();

// The centre of a wedge at a given radius, in physical pixels. Paint and
// hit-test both work from the same polar reading, so a click always lands on
// the wedge that is drawn under it.
void CircleRingPoint(int i, float radius, float& x, float& y);

void ComputeLayout() {
    Layout& l = g_layout;
    l.width = S(static_cast<float>(kPanelWidth));
    l.height = S(static_cast<float>(kPanelHeight));
    const int pad = S(static_cast<float>(kPanelPad));

    l.header = {pad, S(static_cast<float>(kHeaderTop)), l.width - pad,
                S(static_cast<float>(kHeaderTop + kHeaderHeight))};
    l.search = {pad, S(static_cast<float>(kSearchTop)), l.width - pad,
                S(static_cast<float>(kSearchTop + kSearchHeight))};
    l.searchText = {l.search.left + S(38), l.search.top,
                    l.search.right - S(14), l.search.bottom};

    l.list = {pad, S(static_cast<float>(kListTop)), l.width - pad,
              S(static_cast<float>(kListTop + kRowHeight * kVisibleRows))};
    l.rows = l.list;
    l.rowHeight = S(static_cast<float>(kRowHeight));

    l.card = {pad, S(static_cast<float>(kCardTop)), l.width - pad,
              S(static_cast<float>(kCardTop + kCardHeight))};
    l.chordName = {l.card.left + S(16), l.card.top + S(6),
                   l.card.right - S(16), l.card.top + S(34)};
    // The speaker sits at the end of the name row, so the name is shortened to
    // leave it room on a long chord.
    const int speakerSize = S(22);
    l.speaker = {l.card.right - S(14) - speakerSize,
                 l.chordName.top + S(2), l.card.right - S(14),
                 l.chordName.top + S(2) + speakerSize};
    l.chordName.right = l.speaker.left - S(8);
    l.tones = {l.card.left + S(16), l.card.top + S(34), l.card.right - S(16),
               l.card.top + S(52)};

    const int stripTop = l.card.bottom - S(10) - S(kVoicingStripHeight);
    const int diagramTop = l.tones.bottom + S(4);
    const int diagramBottom = stripTop - S(4);
    const int chevronWidth = S(28);
    l.diagram = {l.card.left + S(16) + chevronWidth, diagramTop,
                 l.card.right - S(16) - chevronWidth, diagramBottom};
    l.previous = {l.card.left + S(10), diagramTop,
                  l.card.left + S(10) + chevronWidth, diagramBottom};
    l.next = {l.card.right - S(10) - chevronWidth, diagramTop, l.card.right - S(10),
              diagramBottom};

    l.foot = {pad, S(static_cast<float>(kFootTop)), l.width - pad,
              S(static_cast<float>(kFootTop + kFootHeight))};
    l.dotCount = 0;

    // The close button sits at the far right of the header, tabs beside it.
    const int tabHeight = S(static_cast<float>(kHeaderHeight));
    const int closeSize = S(24.0f);
    const int closeTop = l.header.top + (tabHeight - closeSize) / 2;
    l.close = {l.header.right - closeSize, closeTop, l.header.right,
               closeTop + closeSize};

    // Tabs are measured, then right aligned in the header strip.
    HFONT font = g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    const int instruments =
        std::min(static_cast<int>(kInstrumentCount), chorddata::kInstrumentsSize);
    int x = l.close.left - S(6);
    for (int i = instruments - 1; i >= 0; i--) {
        const std::wstring label = chorddata::kInstruments[i].name;
        const int textWidth = g_text.Measure(font, label).cx;
        const int width = textWidth + S(16);
        l.tabs[i] = {x - width, l.header.top, x, l.header.top + tabHeight};
        x -= width + S(2);
    }

    // The tool picker takes the left of the header, where three chips used to
    // crowd the instrument chips on the right. It is measured for the longest
    // label, so it never changes width as the tool changes.
    int toolWidth = 0;
    for (int i = 0; i < kToolCount; i++) {
        toolWidth = std::max(
            toolWidth,
            static_cast<int>(g_text.Measure(font, ToolLabel(i)).cx)) + 4;
    }
    l.toolSelect = {l.header.left, l.header.top,
                    l.header.left + toolWidth + S(30), l.header.top + tabHeight};
    const int menuRowHeight = S(26);
    const int menuTop = l.toolSelect.bottom + S(4);
    int menuWidth = static_cast<int>(l.toolSelect.right - l.toolSelect.left);
    for (int i = 0; i < kToolCount; i++) {
        menuWidth = std::max(
            menuWidth,
            static_cast<int>(g_text.Measure(font, ToolLabel(i)).cx) + S(46));
    }
    l.toolMenu = {l.toolSelect.left, menuTop, l.toolSelect.left + menuWidth,
                  menuTop + menuRowHeight * kToolCount + S(8)};
    for (int i = 0; i < kToolCount; i++) {
        const int top = menuTop + S(4) + i * menuRowHeight;
        l.toolMenuRows[i] = {l.toolMenu.left + S(4), top,
                             l.toolMenu.right - S(4), top + menuRowHeight};
    }

    // The tap tempo pad covers the search, list and card at once, so the panel
    // keeps a single size for all three tools.
    l.tapPad = {pad, l.search.top, l.width - pad, l.card.bottom};
    l.tapValue = {l.tapPad.left, l.tapPad.top + S(84), l.tapPad.right,
                  l.tapPad.top + S(140)};
    l.tapName = {l.tapPad.left, l.tapPad.top + S(210), l.tapPad.right,
                 l.tapPad.top + S(232)};
    l.tapBars = {l.tapPad.left + S(80), l.tapPad.top + S(240),
                 l.tapPad.right - S(80), l.tapPad.top + S(276)};
    l.tapHint = {l.tapPad.left, l.tapPad.top + S(288), l.tapPad.right,
                 l.tapPad.top + S(306)};

    const int buttonHeight = S(32);
    const int buttonTop = l.tapPad.bottom - S(20) - buttonHeight;
    const int buttonGap = S(8);
    auto buttonWidth = [&](PCWSTR label) {
        return g_text.Measure(font, label).cx + S(30);
    };
    const int halfWidth = buttonWidth(L"Half");
    const int resetWidth = buttonWidth(L"Reset");
    const int doubleWidth = buttonWidth(L"Double");
    int buttonX =
        (l.width - (halfWidth + resetWidth + doubleWidth + buttonGap * 2)) / 2;
    l.tapHalf = {buttonX, buttonTop, buttonX + halfWidth,
                 buttonTop + buttonHeight};
    buttonX += halfWidth + buttonGap;
    l.tapReset = {buttonX, buttonTop, buttonX + resetWidth,
                  buttonTop + buttonHeight};
    buttonX += resetWidth + buttonGap;
    l.tapDouble = {buttonX, buttonTop, buttonX + doubleWidth,
                   buttonTop + buttonHeight};

    // The metronome uses the same rectangle as the tap tempo pad, so all three
    // tools keep one panel size. Everything below is measured from its top.
    l.metroPad = l.tapPad;
    const int metroTop = l.metroPad.top;
    l.metroValue = {l.metroPad.left, metroTop + S(20), l.metroPad.right,
                    metroTop + S(72)};
    l.metroMeta = {l.metroPad.left, metroTop + S(72), l.metroPad.right,
                   metroTop + S(90)};
    l.metroDots = {l.metroPad.left + S(20), metroTop + S(104),
                   l.metroPad.right - S(20), metroTop + S(152)};

    const int presetGap = S(6);
    const int presetWidth =
        (l.metroPad.right - l.metroPad.left - S(32) - presetGap * 3) / 4;
    for (int i = 0; i < kSignatureCount; i++) {
        const int column = i % 4;
        const int row = i / 4;
        const int x =
            l.metroPad.left + S(16) + column * (presetWidth + presetGap);
        const int y = metroTop + S(168) + row * S(28);
        l.metroPreset[i] = {x, y, x + presetWidth, y + S(24)};
    }

    const int meterRow = metroTop + S(240);
    int stepX = l.metroPad.left + S(16);
    l.metroBeatsDown = {stepX, meterRow, stepX + S(28), meterRow + S(24)};
    stepX += S(28) + S(4);
    l.metroBeatsLabel = {stepX, meterRow, stepX + S(96), meterRow + S(24)};
    stepX += S(96) + S(4);
    l.metroBeatsUp = {stepX, meterRow, stepX + S(28), meterRow + S(24)};
    stepX += S(28) + S(10);
    const int unitWidth = (l.metroPad.right - S(16) - stepX - S(6) * 3) / 4;
    for (int i = 0; i < 4; i++) {
        const int x = stepX + i * (unitWidth + S(6));
        l.metroUnit[i] = {x, meterRow, x + unitWidth, meterRow + S(24)};
    }

    const int subRow = metroTop + S(276);
    const int subGap = S(6);
    const int subWidth =
        (l.metroPad.right - l.metroPad.left - S(32) - subGap * 4) / 5;
    int subX = l.metroPad.left + S(16);
    for (int i = 0; i < 5; i++) {
        const RECT chip{subX, subRow, subX + subWidth, subRow + S(24)};
        if (i < 4) {
            l.metroSub[i] = chip;
        } else {
            l.metroGroup = chip;
        }
        subX += subWidth + subGap;
    }

    const int transportY = metroTop + S(340);
    l.metroTap = {l.metroPad.left + S(16), transportY,
                  l.metroPad.left + S(104), transportY + S(38)};
    l.metroStart = {l.metroTap.right + S(8), transportY,
                    l.metroPad.right - S(16), transportY + S(38)};
    l.metroHint = {l.metroPad.left, metroTop + S(396), l.metroPad.right,
                   metroTop + S(414)};

    // The tuner takes the same rectangle a third time. Two labelled selects
    // fill the first row, each opening a listbox over the arc, which is the
    // bulk of the card with the note inside it.
    l.tunerPad = l.metroPad;
    const int tunerTop = l.tunerPad.top;
    const int tunerLeft = l.tunerPad.left + S(16);
    const int tunerRight = l.tunerPad.right - S(16);
    const int tunerGap = S(8);
    const int instWidth = (tunerRight - tunerLeft - tunerGap) * 45 / 100;
    const int instRight = tunerLeft + instWidth;
    const int tuneLeft = instRight + tunerGap;
    l.tunerInstCaption = {tunerLeft + S(2), tunerTop + S(5), instRight,
                          tunerTop + S(17)};
    l.tunerInstSelect = {tunerLeft, tunerTop + S(19), instRight,
                         tunerTop + S(47)};
    l.tunerTuneCaption = {tuneLeft + S(2), tunerTop + S(5), tunerRight,
                          tunerTop + S(17)};
    l.tunerTuneSelect = {tuneLeft, tunerTop + S(19), tunerRight,
                         tunerTop + S(47)};
    l.tunerArc = {tunerLeft, tunerTop + S(56), tunerRight, tunerTop + S(236)};
    l.tunerNote = {l.tunerArc.left, tunerTop + S(80), l.tunerArc.right,
                   tunerTop + S(146)};
    l.tunerCents = {l.tunerArc.left, tunerTop + S(148), l.tunerArc.right,
                    tunerTop + S(166)};
    l.tunerMeta = {l.tunerArc.left, tunerTop + S(170), l.tunerArc.right,
                   tunerTop + S(188)};
    l.tunerStrings = {tunerLeft, tunerTop + S(246), tunerRight,
                      tunerTop + S(278)};
    const int stringCount =
        std::clamp(TunerStringCount(g_tuner.tuning), 1, kTunerStringMax);
    const int stringGap = S(6);
    const int stringWidth =
        (l.tunerStrings.right - l.tunerStrings.left -
         stringGap * (stringCount - 1)) /
        stringCount;
    int stringX = l.tunerStrings.left;
    for (int i = 0; i < stringCount; i++) {
        l.tunerStringChip[i] = {stringX, l.tunerStrings.top,
                                stringX + stringWidth, l.tunerStrings.bottom};
        stringX += stringWidth + stringGap;
    }
    // The lists drop over whatever is underneath, opaque, so they read as
    // popups. Both span the row they belong to.
    const int tunerRowHeight = S(26);
    const int instMenuTop = l.tunerInstSelect.bottom + S(4);
    l.tunerInstMenu = {tunerLeft, instMenuTop, tunerRight,
                       instMenuTop + S(8) +
                           kTunerInstrumentCount * tunerRowHeight};
    for (int i = 0; i < kTunerInstrumentCount; i++) {
        const int rowTop = instMenuTop + S(4) + i * tunerRowHeight;
        l.tunerInstMenuRow[i] = {l.tunerInstMenu.left + S(4), rowTop,
                                 l.tunerInstMenu.right - S(4),
                                 rowTop + tunerRowHeight};
    }
    const int tuneCount = std::clamp(TunerTuningCount(g_tuner.instrument), 0,
                                     kTunerMenuMax);
    const int tuneMenuTop = l.tunerTuneSelect.bottom + S(4);
    l.tunerTuneMenu = {tunerLeft, tuneMenuTop, tunerRight,
                       tuneMenuTop + S(8) + tuneCount * tunerRowHeight};
    for (int i = 0; i < tuneCount; i++) {
        const int rowTop = tuneMenuTop + S(4) + i * tunerRowHeight;
        l.tunerTuneMenuRow[i] = {l.tunerTuneMenu.left + S(4), rowTop,
                                 l.tunerTuneMenu.right - S(4),
                                 rowTop + tunerRowHeight};
    }
    l.tunerHint = {l.tunerPad.left, tunerTop + S(396), l.tunerPad.right,
                   tunerTop + S(414)};

    // Strumming tool, laid out in the same rectangle once more. A pattern
    // select that unrolls the list of common patterns, the grid it writes, a
    // meter and grid-density row, the transport, then the three rows that pick
    // the voice: the strings, the sound and the practice switches.
    l.strumPad = l.metroPad;
    const int strumTop = l.strumPad.top;
    const int strumLeft = l.strumPad.left + S(16);
    const int strumRight = l.strumPad.right - S(16);

    l.strumPattern = {strumLeft, strumTop + S(6), strumRight, strumTop + S(34)};
    // The pattern list unrolls downward and is given the room the busiest meter
    // needs; each meter's list is cut to its own lines when it is painted. The
    // grid under it is the one thing it writes over, and it comes back the
    // moment the list closes.
    // The list has a heading per grid on top of the patterns, so its lines are
    // a little shorter than the meter list's: the tallest of them - all four
    // grids of 4/4 - still ends above the hint row at the foot of the card.
    const int patternRowHeight = S(24);
    const int patternTop = l.strumPattern.bottom + S(4);
    const int patternRows = kStrumPatternEntriesMax;
    l.strumPatternMenu = {strumLeft, patternTop, strumRight,
                          patternTop + S(8) + patternRows * patternRowHeight};
    for (int i = 0; i < patternRows; i++) {
        const int rowTop = patternTop + S(4) + i * patternRowHeight;
        l.strumPatternRow[i] = {l.strumPatternMenu.left + S(4), rowTop,
                                l.strumPatternMenu.right - S(4),
                                rowTop + patternRowHeight};
    }

    // The pattern grid: one cell per subdivision slot with a small gap at each
    // beat, so the bar still reads in beats at sixteenth-note density.
    l.strumGrid = {strumLeft, strumTop + S(42), strumRight, strumTop + S(170)};
    const int cells = std::clamp(StrumSlotCount(), 1, kStrumSlotsMax);
    const int beatGap = S(4);
    const int breaks = cells > 1 ? cells - 1 : 0;
    const int gridW =
        (l.strumGrid.right - l.strumGrid.left) - breaks * beatGap;
    const int slotW = std::max(1, gridW / cells);
    int slotX = l.strumGrid.left;
    for (int i = 0; i < cells; i++) {
        l.strumSlot[i] = {slotX, l.strumGrid.top, slotX + slotW,
                          l.strumGrid.bottom};
        slotX += slotW + beatGap;
    }

    // Meter and grid density share one row: the meter select on the left, the
    // four note-value chips filling the rest of it.
    const int strumRowY = strumTop + S(178);
    l.strumMeterSelect = {strumLeft, strumRowY, strumLeft + S(128),
                          strumRowY + S(28)};
    const int strumSubLeft = l.strumMeterSelect.right + S(8);
    const int strumSubWidth = (strumRight - strumSubLeft - S(6) * 3) / 4;
    for (int i = 0; i < 4; i++) {
        const int x = strumSubLeft + i * (strumSubWidth + S(6));
        l.strumSub[i] = {x, strumRowY, x + strumSubWidth, strumRowY + S(28)};
    }
    // The meter list unrolls downward from its select; eight meters of 26 is
    // the tallest list in the card, and it is gone again as soon as one lands.
    const int meterRowHeight = S(26);
    const int meterTop = l.strumMeterSelect.bottom + S(4);
    l.strumMeterMenu = {strumLeft, meterTop, strumRight,
                        meterTop + S(8) + kSignatureCount * meterRowHeight};
    for (int i = 0; i < kSignatureCount; i++) {
        const int rowTop = meterTop + S(4) + i * meterRowHeight;
        l.strumMeterRow[i] = {l.strumMeterMenu.left + S(4), rowTop,
                              l.strumMeterMenu.right - S(4),
                              rowTop + meterRowHeight};
    }

    // The transport is one run: a wide bar that starts and stops the pattern,
    // carrying the play glyph on its centre line, then the tempo stepper. The
    // bar takes the room a word would have needed, so the row reads as one
    // control instead of a small chip beside a hole.
    const int strumTransportY = strumTop + S(212);
    l.strumPlay = {strumLeft, strumTransportY, strumRight - S(128),
                   strumTransportY + S(38)};
    l.strumTempoDown = {strumRight - S(120), strumTransportY,
                        strumRight - S(88), strumTransportY + S(38)};
    l.strumTempo = {strumRight - S(88), strumTransportY, strumRight - S(32),
                    strumTransportY + S(38)};
    l.strumTempoUp = {
        strumRight - S(32), strumTransportY, strumRight,
        strumTransportY + S(38)};

    // The voice: which instrument's strings, which chord, what they ring like.
    // The instrument chips take the left of the first row and the chord select
    // the rest of it; the sound and the practice switches each own a row, so
    // every group reads as one idea.
    const int strumRowOne = strumTop + S(262);
    const int strumRowTwo = strumTop + S(300);
    const int strumRowThree = strumTop + S(338);
    const int chipGap = S(6);
    const int strumInstWidth = S(92);
    l.strumInst[0] = {strumLeft, strumRowOne, strumLeft + strumInstWidth,
                      strumRowOne + S(28)};
    l.strumInst[1] = {l.strumInst[0].right + chipGap, strumRowOne,
                      l.strumInst[0].right + chipGap + strumInstWidth,
                      strumRowOne + S(28)};
    l.strumChord = {l.strumInst[1].right + chipGap, strumRowOne, strumRight,
                    strumRowOne + S(28)};

    const int soundWidth = (strumRight - strumLeft -
                            chipGap * (kStrumSoundCount - 1)) /
                           kStrumSoundCount;
    for (int i = 0; i < kStrumSoundCount; i++) {
        const int x = strumLeft + i * (soundWidth + chipGap);
        l.strumSound[i] = {x, strumRowTwo, x + soundWidth, strumRowTwo + S(28)};
    }

    const int chipWidth = (strumRight - strumLeft -
                           chipGap * (kStrumChipCount - 1)) /
                          kStrumChipCount;
    for (int i = 0; i < kStrumChipCount; i++) {
        const int x = strumLeft + i * (chipWidth + chipGap);
        l.strumChip[i] = {x, strumRowThree, x + chipWidth,
                          strumRowThree + S(28)};
    }

    // The chord list unrolls upward from the select, since there is no room
    // below it: two columns of five rows over the pattern.
    const int chordRowHeight = S(26);
    const int chordRows = (kStrumChordCount + 1) / 2;
    const int chordBottom = strumRowOne - S(4);
    const int chordTop = chordBottom - chordRows * chordRowHeight - S(8);
    l.strumChordMenu = {strumLeft, chordTop, strumRight, chordBottom};
    const int chordColumn = (strumRight - strumLeft - S(8)) / 2;
    for (int i = 0; i < kStrumChordCount; i++) {
        const int x = strumLeft + S(4) + (i % 2) * chordColumn;
        const int top = chordTop + S(4) + (i / 2) * chordRowHeight;
        l.strumChordRow[i] = {x, top, x + chordColumn, top + chordRowHeight};
    }

    l.strumHint = {l.strumPad.left, strumTop + S(408), l.strumPad.right,
                   strumTop + S(426)};

    // Circle of fifths tool, laid out in the same rectangle once more. Two rings
    // of twelve - the majors outside, their relative minors inside - with the
    // instrument chips above the wheel, the seven diatonic chords under it and
    // three lines of theory at the foot.
    l.circlePad = l.metroPad;
    const int circleTop = l.circlePad.top;
    const int circleLeft = l.circlePad.left + S(16);
    const int circleRight = l.circlePad.right - S(16);
    // The instrument chips only change the sound, so they take the top row on
    // the left and leave the rest of the card to the wheel.
    const int circleChipW = S(56);
    const int circleChipGap = S(4);
    for (int i = 0; i < kInstrumentChipCount; i++) {
        const int x = circleLeft + i * (circleChipW + circleChipGap);
        l.circleInst[i] = {x, circleTop + S(4), x + circleChipW,
                           circleTop + S(28)};
    }

    // The wheel is as wide as the card allows but no taller than the room
    // between the chips and the chord row, so it stays round at every scale.
    // The bands are fixed widths: 32 for the majors, a 3 gap, 30 for the minors,
    // which leaves a hole wide enough for the tonic and its scale.
    int ringR = (circleRight - circleLeft) / 2;
    if (ringR > S(126)) {
        ringR = S(126);
    }
    l.circleRingR = ringR;
    l.circleMidR = ringR - S(32);
    l.circleCoreR = l.circleMidR - S(33);
    l.circleCx = (circleLeft + circleRight) / 2;
    l.circleCy = circleTop + S(36) + ringR;
    l.circleCenter = {l.circleCx - S(64), l.circleCy - S(38),
                      l.circleCx + S(64), l.circleCy + S(8)};
    l.circleCaption = {l.circleCx - S(58), l.circleCy + S(10),
                       l.circleCx + S(58), l.circleCy + S(28)};

    const int chordY = circleTop + S(300);
    const int chordGap = S(4);
    const int chordWidth = (circleRight - circleLeft - chordGap * 6) / 7;
    for (int i = 0; i < 7; i++) {
        const int x = circleLeft + i * (chordWidth + chordGap);
        l.circleChord[i] = {x, chordY, x + chordWidth, chordY + S(26)};
    }

    l.circleSignature = {circleLeft, circleTop + S(332), circleRight,
                         circleTop + S(350)};
    l.circleMinor = {circleLeft, circleTop + S(352), circleRight,
                     circleTop + S(370)};
    l.circleDetail = {circleLeft, circleTop + S(372), circleRight,
                      circleTop + S(390)};
    l.circleHint = {l.circlePad.left, circleTop + S(396), l.circlePad.right,
                    circleTop + S(414)};

    // Note trainer, laid out in the same rectangle once more. A row of three
    // instrument chips with a naturals toggle and a Start button across the
    // top, the big target note in the middle, the note heard and a countdown
    // bar beneath it, then the feedback and score lines.
    l.trainerPad = l.metroPad;
    const int trainerTop = l.trainerPad.top;
    const int trainerLeft = l.trainerPad.left + S(16);
    const int trainerRight = l.trainerPad.right - S(16);
    const int trainerChipW = S(56);
    const int trainerGap = S(4);
    for (int i = 0; i < 3; i++) {
        const int x = trainerLeft + i * (trainerChipW + trainerGap);
        l.trainerInst[i] = {x, trainerTop + S(4), x + trainerChipW,
                            trainerTop + S(28)};
    }
    const int trainerStartW = S(72);
    l.trainerStart = {trainerRight - trainerStartW, trainerTop + S(4),
                      trainerRight, trainerTop + S(28)};
    const int trainerNatW = S(78);
    l.trainerNaturals = {l.trainerStart.left - S(6) - trainerNatW,
                         trainerTop + S(4), l.trainerStart.left - S(6),
                         trainerTop + S(28)};
    l.trainerPrompt = {trainerLeft, trainerTop + S(64), trainerRight,
                       trainerTop + S(150)};
    l.trainerPlayed = {trainerLeft, trainerTop + S(158), trainerRight,
                       trainerTop + S(180)};
    l.trainerBar = {trainerLeft, trainerTop + S(192), trainerRight,
                    trainerTop + S(204)};
    l.trainerFeedback = {trainerLeft, trainerTop + S(216), trainerRight,
                         trainerTop + S(240)};
    const int trainerMid = (trainerLeft + trainerRight) / 2;
    l.trainerScore = {trainerLeft, trainerTop + S(270), trainerMid,
                      trainerTop + S(294)};
    l.trainerStreak = {trainerMid, trainerTop + S(270), trainerRight,
                       trainerTop + S(294)};
    l.trainerHint = {l.trainerPad.left, trainerTop + S(396), l.trainerPad.right,
                     trainerTop + S(414)};
}

////////////////////////////////////////////////////////////////////////////////
// Application state

enum Phase {
    kPhaseClosed,
    kPhaseOpening,
    kPhaseOpen,
    kPhaseClosing,
};

struct AppState {
    Tool tool = kToolChords;
    Instrument instrument = kInstrumentGuitar;
    std::wstring query;
    std::vector<int> results;  // Indices into the instrument's chord list.
    int selected = 0;          // Index into results.
    int voicing = 0;
    int scroll = 0;            // First visible result.
};

AppState g_app;

// Tap tempo. The intervals are kept rather than just the last one, so a couple
// of steady taps settle the number and the bars can show how even they were.
constexpr int kMaxTapIntervals = 7;
constexpr double kMinBpm = 20.0;
constexpr double kMaxBpm = 400.0;
constexpr double kMinTapIntervalMs = 120.0;

struct TapState {
    double intervals[kMaxTapIntervals] = {};
    int intervalCount = 0;
    double lastTap = 0.0;   // 0 until the first tap of a measurement.
    double bpm = 0.0;
    double flashAt = -1000.0;
    bool hasValue = false;
};

TapState g_tap;

double TapAverageInterval() {
    double sum = 0.0;
    for (int i = 0; i < g_tap.intervalCount; i++) {
        sum += g_tap.intervals[i];
    }
    return g_tap.intervalCount > 0 ? sum / g_tap.intervalCount : 0.0;
}

PCWSTR TempoName(double bpm) {
    if (bpm < 40.0) {
        return L"Grave";
    }
    if (bpm < 60.0) {
        return L"Largo";
    }
    if (bpm < 66.0) {
        return L"Adagio";
    }
    if (bpm < 76.0) {
        return L"Andante";
    }
    if (bpm < 108.0) {
        return L"Moderato";
    }
    if (bpm < 120.0) {
        return L"Allegro";
    }
    if (bpm < 168.0) {
        return L"Vivace";
    }
    return L"Presto";
}

// "Space", "Enter" or "Space or Enter", for the hint lines.
PCWSTR TapKeyLabel() {
    if (g_settings.tapKeySpace && g_settings.tapKeyEnter) {
        return L"Space or Enter";
    }
    return g_settings.tapKeyEnter ? L"Enter" : L"Space";
}

// The metronome shares the tap tempo value, so the two tools always agree on
// the tempo: tapping sets it, and nudging it here shows up over there.
struct MetronomeState {
    bool running = false;
    int beats = 4;
    int unit = 4;
    int subdivision = 1;  // Clicks per beat: 1 off, 2 eighths, 3 triplets, 4.
    int group = 1;        // An accent on every Nth beat, 1 for the downbeat.
    bool defaultsApplied = false;
    double pulseAt = -1000.0;
    unsigned paintedPulse = 0;
};

MetronomeState g_metro;

// Strumming tool. The vocabulary constants and the preset table live above
// the tool enum (the layout needs their sizes at compile time). The state
// itself sits here, next to the metronome it borrows its tempo and grid from.
struct StrumState {
    bool running = false;
    int beats = 4;
    int unit = 4;
    int subdivision = 2;  // Cells per beat: 1 quarters, 2 eighths, 3 triplet, 4.
    int preset = 0;       // -1 once the grid is hand edited.
    // The voice of every stroke: which instrument's strings ring, which chord
    // they are fretted to, and how hard the hand chokes them. The chord is an
    // index into kStrumChords, where 0 is the open strings.
    int instrument = kInstrumentGuitar;
    int sound = kStrumOpen;
    int chord = 0;
    bool countIn = true;
    bool loop = true;
    bool practice = false;
    int rampStepBpm = 2;
    int targetBpm = 160;
    int targetOffsetBpm = 20;  // Practice ramp aims this far above the start.
    int slotCursor = 0;
    bool defaultsApplied = false;
    unsigned paintedSlot = 0;
    unsigned paintedMeasure = 0;
    double paintedSlotAt = -1000.0;
    int stroke[kStrumSlotsMax] = {};
    bool accent[kStrumSlotsMax] = {};
    bool ghost[kStrumSlotsMax] = {};
    bool tie[kStrumSlotsMax] = {};
};

StrumState g_strum;

// Typed tempo entry. The metronome and the strumming transport play the same
// tempo, so clicking its readout in either card - or simply typing a digit -
// opens a small field over it: the digits replace the tempo while the field is
// up, Enter commits, Escape puts it back, and a nudge commits it first.
constexpr int kTempoDigitsMax = 3;  // 400 is the fastest tempo there is.

struct TempoEntry {
    bool active = false;
    int tool = kToolMetronome;  // The card that owns the field.
    wchar_t digits[kTempoDigitsMax] = {};
    int count = 0;
    double typedAt = 0.0;  // When the last digit landed, for the caret blink.
};

TempoEntry g_tempoEntry;

// Whether the field is open and belongs to the card on screen.
bool TempoEntryActive() {
    return g_tempoEntry.active &&
           g_tempoEntry.tool == static_cast<int>(g_app.tool);
}

// Circle of fifths tool. The whole view is just which of the twelve spokes is
// selected and which of the wheel's two rings is being read; every note, chord
// and signature the panel shows is spelled from that pair.
struct CircleState {
    int key = 0;         // Index into kCircleKeys, i.e. the spoke.
    bool minor = false;  // Read the inner ring, the relative minor.
    // Which instrument the scales and chords are played on. It only colours
    // the sound, the way the chord library's tabs do.
    int instrument = kInstrumentGuitar;
    bool playOnSelect = true;  // Sound the scale when a key is picked.
    bool defaultsApplied = false;
    double flashAt = -1000.0;  // When a key was last selected, for the pulse.
};

CircleState g_circle;

// A key badge reports which ring it sits on in the index it hands back: the
// twelve spokes are the majors outside, and the twelve past them are the
// relative minors inside the same spokes.
constexpr int kCircleRingMinor = 12;

// The tonic the wheel is speaking about. The selection is a spoke plus a ring,
// so a minor key is the sixth degree of the major one on the same spoke, read
// as a key in its own right.
int CircleTonicPc() {
    return (kCircleKeys[g_circle.key].pc + (g_circle.minor ? 9 : 0)) % 12;
}

int CircleTonicLetter() {
    return (kCircleKeys[g_circle.key].letter + (g_circle.minor ? 5 : 0)) % 7;
}

const int* CircleSteps() {
    return g_circle.minor ? kCircleMinorSteps : kCircleMajorSteps;
}

const int* CircleQualities() {
    return g_circle.minor ? kCircleMinorQuality : kCircleChordQuality;
}

// Spell a pitch class on a given letter, choosing the smallest accidental that
// lands on it. This is what keeps G major's seventh as F# and A minor's third
// as C rather than B#.
std::wstring CircleSpell(int letter, int pc) {
    static const PCWSTR kLetters[7] = {L"C", L"D", L"E", L"F",
                                       L"G", L"A", L"B"};
    int delta = (pc - CircleLetterPc(letter) + 12) % 12;
    if (delta > 6) {
        delta -= 12;
    }
    return std::wstring(kLetters[((letter % 7) + 7) % 7]) +
           CircleAccidental(delta);
}

// The tonic as a bare note name, and as the key it is called (a minor tonic
// carries the m).
std::wstring CircleTonicNote() {
    return CircleSpell(CircleTonicLetter(), CircleTonicPc());
}

std::wstring CircleTonicName() {
    if (g_circle.minor) {
        return CircleTonicNote() + L"m";
    }
    return std::wstring(kCircleKeys[g_circle.key].name);
}

// One degree of the selected key's scale, spelled on its own letter.
std::wstring CircleScaleName(int degree) {
    const int step = ((degree % 7) + 7) % 7;
    return CircleSpell(CircleTonicLetter() + step,
                       (CircleTonicPc() + CircleSteps()[step]) % 12);
}

// The diatonic triad on a degree, as a chord name.
std::wstring CircleScaleChordName(int degree) {
    std::wstring name = CircleScaleName(degree);
    switch (CircleQualities()[degree]) {
        case 1:
            name += L"m";
            break;
        case 2:
            name += L"\x00B0";  // degree sign marks the diminished chord
            break;
        default:
            break;
    }
    return name;
}

// The numeral a chord on the wheel earns, if it belongs to the selected key.
// Majors sit on the outer ring and minors on the inner one, and the two modes
// swap which thirds of the wheel they light up.
PCWSTR CircleWheelNumeral(int semitones, bool minorRing) {
    const CircleNumeral* row;
    if (g_circle.minor) {
        row = minorRing ? kCircleMinorInner : kCircleMinorOuter;
    } else {
        row = minorRing ? kCircleMajorInner : kCircleMajorOuter;
    }
    for (int i = 0; i < 3; i++) {
        if (row[i].semitones == semitones) {
            return row[i].mark;
        }
    }
    return nullptr;
}

int StrumSlotCount() {
    return std::clamp(g_strum.beats * g_strum.subdivision, 1, kStrumSlotsMax);
}

// The finest grid a bar can be written on before it runs past the slots the
// engine holds: the sixteenths of a twelve-eight bar would need forty-eight, so
// that meter stops at its sixteen and a nine-eight bar at its triplet.
int StrumGridLimit(int beats) {
    const int fit = kStrumSlotsMax / std::max(1, beats);
    return std::clamp(fit, 1, 4);
}

// One character of a preset's cell string, in the values the grid draws from.
// The pattern list paints straight from these, so a row previews exactly the
// bar the preset will write.
struct StrumCellLook {
    int stroke;
    bool accent;
};

StrumCellLook StrumDecodeCell(wchar_t c) {
    switch (c) {
        case L'd': return {kStrokeDown, false};
        case L'D': return {kStrokeDown, true};
        case L'u': return {kStrokeUp, false};
        case L'U': return {kStrokeUp, true};
        case L'x': return {kStrokeDownMute, false};
        case L'X': return {kStrokeDownMute, true};
        case L'c': return {kStrokeUpMute, false};
        case L'C': return {kStrokeUpMute, true};
        default: break;
    }
    return {kStrokeRest, false};
}

void StrumLoadPresetInto(StrumState& s, int preset) {
    if (preset < 0 || preset >= kStrumPresetCount) {
        return;
    }
    const StrumPreset& p = kStrumPresets[preset];
    s.preset = preset;
    s.beats = p.beats;
    s.unit = p.unit;
    s.subdivision = p.subdivision;
    s.slotCursor = 0;
    const int cells = std::clamp(p.beats * p.subdivision, 1, kStrumSlotsMax);
    for (int i = 0; i < kStrumSlotsMax; i++) {
        s.stroke[i] = kStrokeRest;
        s.accent[i] = false;
        s.ghost[i] = false;
        s.tie[i] = false;
    }
    for (int i = 0; i < cells; i++) {
        const StrumCellLook look = StrumDecodeCell(p.cells[i]);
        s.stroke[i] = look.stroke;
        s.accent[i] = look.accent;
    }
}

double ToolkitBpm() {
    return g_tap.hasValue ? g_tap.bpm : 120.0;
}

LayeredWindow g_panel;
LayeredWindow g_shadow;
Surface g_surface;
Surface g_shadowSurface;
Theme g_theme;
HWND g_messageWindow = nullptr;

int g_x = 0;
int g_y = 0;
int g_slideOffset = 0;
int g_alpha = 255;
int g_shadowMargin = 0;
Phase g_phase = kPhaseClosed;
double g_phaseStart = 0;
double g_caretStart = 0;
bool g_searchFocused = true;
// The panel stays up when another window takes the focus, so it has to know
// whether it is still the active window: the caret only blinks while it is.
bool g_windowActive = true;
bool g_trackingMouse = false;
bool g_dragging = false;
POINT g_dragCursor{};
POINT g_dragOrigin{};
bool g_sliding = false;
int g_slideAnchor = 0;
HitResult g_hover;
HitResult g_pressed;

// The search box keeps a caret and a selection anchor, both counted in
// characters from the start of the query.
int g_caret = 0;
int g_selAnchor = 0;
bool g_selecting = false;

int SelectionStart() {
    return std::min(g_caret, g_selAnchor);
}

int SelectionEnd() {
    return std::max(g_caret, g_selAnchor);
}

bool HasSelection() {
    return g_caret != g_selAnchor;
}

void MoveCaret(int index, bool extend) {
    g_caret = std::clamp(index, 0, static_cast<int>(g_app.query.size()));
    if (!extend) {
        g_selAnchor = g_caret;
    }
}

constexpr UINT_PTR kAnimationTimer = 1;
constexpr UINT_PTR kCaretTimer = 2;
constexpr UINT_PTR kFlashTimer = 3;
constexpr UINT_PTR kPulseTimer = 4;
constexpr UINT_PTR kTunerTimer = 5;
constexpr UINT_PTR kMenuAnimTimer = 6;
constexpr UINT_PTR kTrainerTimer = 7;
constexpr double kOpenDurationMs = 190.0;
constexpr double kCloseDurationMs = 140.0;
constexpr float kSlideDistance = 14.0f;
constexpr double kMenuAnimMs = 130.0;
constexpr double kTunedBadgeMs = 340.0;
constexpr double kNoteSwapMs = 220.0;

void MetronomeHalt();
void MetronomeStop();
void MetronomeToggle();
void MetronomeSync();
void StrumSync();
void StrumStart();
void StrumStop();
void StrumToggle();
void StrumHalt();
void StrumEditSlot(int slot);
void TempoEntryBegin(int tool);
void TempoEntryCancel();
void TempoEntryCommit();
void TempoEntryErase();
bool TempoEntryType(wchar_t c);
void TunerStart();
void TunerHalt();
void TunerPoll();
void TunerAnimate();
void TunerSetInstrument(int instrument);
void TunerSetTuning(int tuning);
void TunerSetString(int string);
void TunerToggleString(int string);
void TunerStepInstrument(int delta);
void TunerStepTuning(int delta);
void CircleSetInstrument(int instrument);
void TrainerEnter();
void TrainerHalt();
void TrainerToggle();
void TrainerTick();
void TrainerResetScore();
void TrainerSetInstrument(int instrument);
void SetTool(int tool);
void Repaint();

////////////////////////////////////////////////////////////////////////////////
// Popup lists
//
// The tool picker in the header, the tuner's instrument and tuning selects and
// the strumming tool's chord list are the same control. The target is the list
// asked for (0 closed, 1 instrument, 2 tuning, 3 tools, 4 strum chord), shown is
// the list actually drawn so it survives the fade out, progress runs from 0 to
// 1, and cursor is the row the arrows rest on. Open, switch and close all roll
// and fade rather than blink, and a list is only committed when a row is picked.

struct MenuState {
    int target = 0;
    int shown = 0;
    float progress = 0.0f;
    int cursor = -1;
};

MenuState g_toolMenu;
MenuState g_tunerMenu;
MenuState g_strumMenu;     // The chord the strings are fretted to.
MenuState g_patternMenu;   // The pattern the grid is set to.
MenuState g_meterMenu;     // The meter the pattern list is filtered by.

enum MenuKind {
    kMenuNone = 0,
    kMenuInstrument = 1,
    kMenuTuning = 2,
    kMenuTools = 3,
    kMenuStrumChord = 4,
    kMenuStrumPattern = 5,
    kMenuStrumMeter = 6,
};

bool MenuHit(const MenuState& m) {
    return m.target != 0;  // Input belongs to the list once it is asked to open.
}

bool MenuVisual(const MenuState& m) {
    return m.target != 0 || m.progress > 0.0f;  // Anything left to draw.
}

bool MenuAnimating(const MenuState& m) {
    return m.target != 0 ? m.progress < 1.0f : m.progress > 0.0f;
}

bool AnyMenuAnimating() {
    return MenuAnimating(g_toolMenu) || MenuAnimating(g_tunerMenu) ||
           MenuAnimating(g_strumMenu) || MenuAnimating(g_patternMenu) ||
           MenuAnimating(g_meterMenu);
}

void StartMenuAnimTimer() {
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kMenuAnimTimer, 16, nullptr);
    }
}

// One 16ms tick of a list animation. Returns whether anything moved.
bool MenuStep(MenuState& m, bool toward) {
    const float step =
        16.0f / static_cast<float>(kMenuAnimMs);
    float next = m.progress + (toward ? step : -step);
    if (next < 0.0f) {
        next = 0.0f;
    } else if (next > 1.0f) {
        next = 1.0f;
    }
    if (next == m.progress) {
        return false;
    }
    m.progress = next;
    if (next == 0.0f && m.target == 0) {
        m.shown = kMenuNone;  // Nothing of it left on screen.
        m.cursor = -1;
    }
    return true;
}

// Opens a list, or swaps which list is showing while one is already up. The
// cursor lands on the row that matches the current choice.
void MenuShow(MenuState& m, int kind, int cursor) {
    m.target = kind;
    m.shown = kind;
    m.cursor = cursor;
    // A list takes the keyboard for its rows, so a tempo field still open
    // behind it is put away instead of being typed at from under the list.
    g_tempoEntry.active = false;
    g_tempoEntry.count = 0;
    StartMenuAnimTimer();
}

void MenuHide(MenuState& m) {
    if (m.target == 0) {
        return;
    }
    m.target = 0;  // shown keeps the list that is fading away.
    StartMenuAnimTimer();
}

// Closes a list at once, for the panel close and the tool switch.
void MenuReset(MenuState& m) {
    m.target = 0;
    m.shown = 0;
    m.progress = 0.0f;
    m.cursor = -1;
}

// Rows the open list is showing, which is what the arrows wrap around.
int MenuRowCount(const MenuState& m) {
    switch (m.target) {
        case kMenuInstrument:
            return kTunerInstrumentCount;
        case kMenuTuning:
            return TunerTuningCount(g_tuner.instrument);
        case kMenuTools:
            return kToolCount;
        case kMenuStrumChord:
            return kStrumChordCount;
        case kMenuStrumPattern: {
            // The list carries a heading per grid, so the cursor counts lines
            // rather than patterns; the keys step over the headings.
            StrumEntry entries[kStrumPatternEntriesMax];
            return StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                       g_strum.beats, g_strum.unit);
        }
        case kMenuStrumMeter:
            return kSignatureCount;
        default:
            break;
    }
    return 0;
}

// The row a list points at, in the values the commit path wants: the
// instrument index, the tuning table entry, or the tool.
int MenuRowValue(const MenuState& m, int row) {
    if (m.target == kMenuTuning) {
        return TunerTuningAt(g_tuner.instrument, row);
    }
    return row;
}

// The arrows rest on a row instead of applying as they go, which is what makes
// the popup read as a listbox. The cursor starts on the current choice.
void MenuMoveCursor(MenuState& m, int delta) {
    const int count = MenuRowCount(m);
    if (count == 0) {
        return;
    }
    int row = m.cursor;
    if (row < 0) {
        row = delta > 0 ? 0 : count - 1;
    } else {
        row = (row + delta + count) % count;
    }
    m.cursor = row;
}

void UpdateMenuAnimations() {
    bool moved = false;
    if (MenuAnimating(g_toolMenu)) {
        moved = MenuStep(g_toolMenu, g_toolMenu.target != 0) || moved;
    }
    if (MenuAnimating(g_tunerMenu)) {
        moved = MenuStep(g_tunerMenu, g_tunerMenu.target != 0) || moved;
    }
    if (MenuAnimating(g_strumMenu)) {
        moved = MenuStep(g_strumMenu, g_strumMenu.target != 0) || moved;
    }
    if (MenuAnimating(g_patternMenu)) {
        moved = MenuStep(g_patternMenu, g_patternMenu.target != 0) || moved;
    }
    if (MenuAnimating(g_meterMenu)) {
        moved = MenuStep(g_meterMenu, g_meterMenu.target != 0) || moved;
    }
    if (moved) {
        Repaint();
    }
}

void Render();
void PresentPanel(bool updateAlpha);
void CloseFlyout();
void OpenFlyout();
void Refilter();
void SelectResult(int index, bool reveal);
void StepVoicing(int delta);
const ChordEntry* SelectedChord();
const Voicing* SelectedVoicing();

const ChordEntry* SelectedChord() {
    if (g_app.results.empty()) {
        return nullptr;
    }
    const Library& library = g_libraries[g_app.instrument];
    const int index = g_app.results[std::clamp(g_app.selected, 0,
                                               static_cast<int>(g_app.results.size()) - 1)];
    if (index < 0 || index >= static_cast<int>(library.chords.size())) {
        return nullptr;
    }
    return &library.chords[index];
}

const Voicing* SelectedVoicing() {
    static Voicing voicing;
    const ChordEntry* entry = SelectedChord();
    if (!entry) {
        return nullptr;
    }
    const int count = VoicingCount(g_app.instrument, *entry);
    if (count <= 0) {
        return nullptr;
    }
    g_app.voicing = std::clamp(g_app.voicing, 0, count - 1);
    if (!ReadVoicing(g_app.instrument, *entry, g_app.voicing, voicing)) {
        return nullptr;
    }
    return &voicing;
}

void EnsureSelectionVisible() {
    const int count = static_cast<int>(g_app.results.size());
    const int visible = kVisibleRows;
    if (g_app.selected < g_app.scroll) {
        g_app.scroll = g_app.selected;
    }
    if (g_app.selected >= g_app.scroll + visible) {
        g_app.scroll = g_app.selected - visible + 1;
    }
    g_app.scroll = std::clamp(g_app.scroll, 0, std::max(0, count - visible));
}

void Refilter() {
    g_app.results.clear();
    if (!EnsureLibrary(g_app.instrument)) {
        return;
    }
    const std::vector<ChordEntry>& chords = g_libraries[g_app.instrument].chords;
    std::wstring query = Lower(g_app.query);
    // Spaces are dropped so a search such as "c m7" still finds Cm7.
    query.erase(std::remove_if(query.begin(), query.end(),
                               [](wchar_t c) { return iswspace(c) != 0; }),
                query.end());
    if (query.empty()) {
        g_app.results.reserve(chords.size());
        for (int i = 0; i < static_cast<int>(chords.size()); i++) {
            g_app.results.push_back(i);
        }
        return;
    }
    // Names that start with the query come first, then the ones that merely
    // contain it, so typing "maj" does not bury Cmaj7 behind Bbmaj9.
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < static_cast<int>(chords.size()); i++) {
            const std::wstring& search = chords[i].search;
            const bool found = pass == 0 ? search.rfind(query, 0) == 0
                                         : search.find(query) != std::wstring::npos;
            if (!found) {
                continue;
            }
            if (pass == 1 && search.rfind(query, 0) == 0) {
                continue;  // Already listed by the first pass.
            }
            g_app.results.push_back(i);
        }
    }
}

void SelectResult(int index, bool reveal) {
    const int count = static_cast<int>(g_app.results.size());
    if (count == 0) {
        g_app.selected = 0;
        return;
    }
    g_app.selected = std::clamp(index, 0, count - 1);
    if (reveal) {
        EnsureSelectionVisible();
    }
}

void SetInstrument(int instrument, bool keepChord) {
    instrument = std::clamp(instrument, 0, kInstrumentCount - 1);
    if (instrument == static_cast<int>(g_app.instrument)) {
        return;
    }
    std::wstring previousName;
    if (keepChord) {
        if (const ChordEntry* current = SelectedChord()) {
            previousName = current->name;
        }
    }
    g_app.instrument = static_cast<Instrument>(instrument);
    g_app.voicing = 0;
    Refilter();
    if (!previousName.empty()) {
        const Library& library = g_libraries[g_app.instrument];
        for (int i = 0; i < static_cast<int>(g_app.results.size()); i++) {
            if (library.chords[g_app.results[i]].name == previousName) {
                SelectResult(i, true);
                break;
            }
        }
    }
}

void StepVoicing(int delta) {
    const ChordEntry* entry = SelectedChord();
    if (!entry) {
        return;
    }
    const int count = VoicingCount(g_app.instrument, *entry);
    if (count <= 0) {
        return;
    }
    // Wrapping makes the arrows feel like a loop of fingerings.
    g_app.voicing = ((g_app.voicing + delta) % count + count) % count;
}

////////////////////////////////////////////////////////////////////////////////
// Panel and shadow windows

void DrawOutsideShadow(Surface& s,
                       float l,
                       float t,
                       float r,
                       float b,
                       float radius,
                       float sigma,
                       Color c) {
    const float reach = sigma * 3.0f;
    const int ix0 = std::max(0, static_cast<int>(floorf(l - reach)));
    const int iy0 = std::max(0, static_cast<int>(floorf(t - reach)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(r + reach)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(b + reach)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        for (int x = ix0; x < ix1; x++) {
            const float d = SdRoundRect(x + 0.5f, y + 0.5f, l, t, r, b, radius);
            if (d < 0.0f) {
                continue;  // The panel covers this part of the shadow.
            }
            const float coverage = SoftCoverage(d, sigma) * Clamp01(d + 1.0f);
            if (coverage > 0.002f) {
                BlendColor(row[x], c, coverage);
            }
        }
    }
}

bool BuildShadow() {
    g_shadowMargin = S(28.0f);
    const int width = g_layout.width;
    const int height = g_layout.height;
    if (!g_shadowSurface.Resize(width + g_shadowMargin * 2,
                                height + g_shadowMargin * 2)) {
        return false;
    }
    const float margin = static_cast<float>(g_shadowMargin);
    DrawOutsideShadow(g_shadowSurface, margin, margin + S(3.0f),
                      margin + width, margin + height + S(3.0f),
                      S(kPanelRadius), S(11.0f), g_theme.shadow);
    return true;
}

RECT MonitorWorkArea() {
    POINT point{0, 0};
    if (!g_settings.primaryMonitorOnly) {
        GetCursorPos(&point);
    }
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) {
        info.rcWork = RECT{0, 0, GetSystemMetrics(SM_CXSCREEN),
                           GetSystemMetrics(SM_CYSCREEN)};
    }
    return info.rcWork;
}

void ComputePosition(int width, int height, int& x, int& y) {
    const RECT work = MonitorWorkArea();
    const int margin = S(12.0f);
    const std::wstring& position = g_settings.position;
    const int left = work.left + margin;
    const int right = work.right - margin - width;
    const int top = work.top + margin;
    const int bottom = work.bottom - margin - height;
    const int centerX = work.left + ((work.right - work.left) - width) / 2;
    const int centerY = work.top + ((work.bottom - work.top) - height) / 2;

    if (position == L"topleft" || position == L"left" ||
        position == L"bottomleft") {
        x = left;
    } else if (position == L"topright" || position == L"right" ||
               position == L"bottomright") {
        x = right;
    } else {
        x = centerX;
    }
    if (position.rfind(L"top", 0) == 0) {
        y = top;
    } else if (position.rfind(L"bottom", 0) == 0) {
        y = bottom;
    } else {
        y = centerY;
    }

    x += g_settings.offsetX;
    y += g_settings.offsetY;
    x = std::clamp(x, static_cast<int>(work.left),
                   std::max(static_cast<int>(work.left),
                            static_cast<int>(work.right) - width));
    y = std::clamp(y, static_cast<int>(work.top),
                   std::max(static_cast<int>(work.top),
                            static_cast<int>(work.bottom) - height));
}

int BaseAlpha() {
    return static_cast<int>(lroundf(255.0f * g_settings.opacity / 100.0f));
}

void PresentPanel(bool updateAlpha) {
    if (!g_panel.Hwnd() || !g_surface.Valid()) {
        return;
    }
    if (updateAlpha) {
        g_alpha = BaseAlpha();
    }
    g_alpha = std::clamp(g_alpha, 0, 255);
    g_panel.Present(g_surface, g_x, g_y + g_slideOffset,
                    static_cast<BYTE>(g_alpha));
    if (g_shadow.Hwnd() && g_shadowSurface.Valid()) {
        g_shadow.Present(g_shadowSurface, g_x - g_shadowMargin,
                         g_y - g_shadowMargin + g_slideOffset,
                         static_cast<BYTE>(g_alpha));
    }
}

// Drives the fade and the slide. Progress runs from 0 (hidden) to 1 (shown).
void ApplyAnimation(double progress) {
    const float eased = Clamp01(static_cast<float>(progress));
    g_alpha = static_cast<int>(lroundf(BaseAlpha() * eased));
    g_slideOffset = static_cast<int>(lroundf((1.0f - eased) * S(kSlideDistance)));
    PresentPanel(false);
}

void FinishClose() {
    g_phase = kPhaseClosed;
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kCaretTimer);
        KillTimer(g_panel.Hwnd(), kAnimationTimer);
        KillTimer(g_panel.Hwnd(), kFlashTimer);
        KillTimer(g_panel.Hwnd(), kMenuAnimTimer);
    }
    // The pattern never plays with the panel closed, so take the strum layer
    // down first and let the metronome decide about the shared stream below.
    StrumHalt();
    // The clicks can outlive the panel, but then nothing polls their pulse.
    if (g_settings.metronomeKeepPlaying) {
        if (g_panel.Hwnd()) {
            KillTimer(g_panel.Hwnd(), kPulseTimer);
        }
    } else {
        MetronomeHalt();
    }
    // The microphone is only wanted while the tuner is on screen.
    TunerHalt();
    // The trainer holds the same microphone and drives its own round timer.
    TrainerHalt();
    if (g_dragging) {
        ReleaseCapture();
    }
    g_panel.Hide();
    g_shadow.Hide();
    g_dragging = false;
    g_sliding = false;
    g_hover = HitResult();
    g_pressed = HitResult();
    MenuReset(g_toolMenu);
    MenuReset(g_tunerMenu);
    MenuReset(g_strumMenu);
    MenuReset(g_patternMenu);
    MenuReset(g_meterMenu);
}

void UpdateAnimation() {
    if (g_phase != kPhaseOpening && g_phase != kPhaseClosing) {
        return;
    }
    const double now = NowMs();
    const double duration = g_phase == kPhaseOpening ? kOpenDurationMs
                                                     : kCloseDurationMs;
    double t = duration > 0.0 ? (now - g_phaseStart) / duration : 1.0;
    if (t >= 1.0) {
        if (g_phase == kPhaseOpening) {
            g_phase = kPhaseOpen;
            ApplyAnimation(1.0);
        } else {
            FinishClose();
        }
        return;
    }
    const double eased = EaseOutCubic(t);
    ApplyAnimation(g_phase == kPhaseOpening ? eased : 1.0 - eased);
}

void OpenFlyout() {
    if (g_phase == kPhaseOpening || g_phase == kPhaseOpen) {
        return;
    }
    // Measuring depends on the DPI of the panel monitor, so move the (still
    // hidden) window over first.
    if (g_panel.Hwnd()) {
        const RECT work = MonitorWorkArea();
        SetWindowPos(g_panel.Hwnd(), nullptr, work.left, work.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    // The default instrument only decides the tab the mod starts on.
    static bool instrumentApplied = false;
    if (!instrumentApplied) {
        instrumentApplied = true;
        SetInstrument(g_settings.defaultInstrument, false);
    }
    // The default tool works the same way: it only decides the first panel.
    static bool toolApplied = false;
    if (!toolApplied) {
        toolApplied = true;
        SetTool(g_settings.defaultTool);
    }
    if (g_app.results.empty()) {
        Refilter();  // Nothing was picked yet, or the last search came up dry.
    }
    Render();
    if (g_panel.Hwnd()) {
        ConfigureGlassWindow(g_panel.Hwnd(),
                             TransparencyEnabled() && IsWindows10Build(17134)
                                 ? &g_theme.panelTop
                                 : nullptr);
    }
    if (!BuildShadow()) {
        Wh_Log(L"Couldn't build the shadow surface");
    }
    ComputePosition(g_layout.width, g_layout.height, g_x, g_y);

    g_slideOffset = S(kSlideDistance);
    g_alpha = 0;
    PresentPanel(false);
    g_shadow.Show(false);
    g_panel.Show(true);
    if (g_shadow.Hwnd()) {
        // Keep the shadow directly under the panel.
        SetWindowPos(g_shadow.Hwnd(), g_panel.Hwnd(), 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER);
    }
    SetForegroundWindow(g_panel.Hwnd());
    SetFocus(g_panel.Hwnd());

    g_phase = kPhaseOpening;
    g_phaseStart = NowMs();
    g_caretStart = g_phaseStart;
    // Only the chord tool has a search box to type into.
    g_searchFocused = g_app.tool == kToolChords;
    // The caret sits at the end of whatever was typed last time.
    g_caret = static_cast<int>(g_app.query.size());
    g_selAnchor = g_caret;
    SetTimer(g_panel.Hwnd(), kAnimationTimer, 8, nullptr);
    SetTimer(g_panel.Hwnd(), kCaretTimer, 530, nullptr);
    // Pick the beat dots back up if the metronome kept playing.
    if (g_metro.running) {
        SetTimer(g_panel.Hwnd(), kPulseTimer, 33, nullptr);
    }
    // The tuner only listens while its panel is up.
    if (g_app.tool == kToolTuner) {
        TunerStart();
    }
    // The trainer holds the same microphone, so it has to wake it again after
    // the panel was closed; the round itself still waits for Start.
    if (g_app.tool == kToolTrainer) {
        TrainerEnter();
    }
}

void CloseFlyout() {
    if (g_phase == kPhaseClosed || g_phase == kPhaseClosing) {
        return;
    }
    if (g_dragging || g_selecting) {
        g_dragging = false;
        g_selecting = false;
        ReleaseCapture();
    }
    g_sliding = false;
    g_phase = kPhaseClosing;
    g_phaseStart = NowMs();
    // Nothing half-typed survives the close, so the next open starts clean.
    g_tempoEntry.active = false;
    g_tempoEntry.count = 0;
    SetTimer(g_panel.Hwnd(), kAnimationTimer, 8, nullptr);
}

void ToggleFlyout() {
    if (g_phase == kPhaseOpen || g_phase == kPhaseOpening) {
        CloseFlyout();
    } else {
        OpenFlyout();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Chord playback
//
// Parked until the freeze it caused is understood. Bringing it back needs the
// #include <mmreg.h> above as well.
//
// The mod bundles no audio libraries, so the handful of WASAPI interfaces it
// needs are declared here. A render thread owns the device, synthesises the
// requested chord at the device's own sample rate and feeds it in shared mode:
// a plucked string model for guitar and ukulele, a struck one for the piano.

#if 0

struct AudioUnknown {
    virtual HRESULT QueryInterface(const GUID& iid, void** out) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
};

struct AudioDevice {
    virtual HRESULT Activate(const GUID& iid, DWORD context, void* params,
                             void** out) = 0;
};

struct AudioDeviceEnumerator {
    virtual HRESULT EnumAudioEndpoints(int flow, DWORD mask, void** out) = 0;
    virtual HRESULT GetDefaultAudioEndpoint(int flow, int role, AudioDevice** out) = 0;
};

// Every method has to be listed, in order, for the virtual table to line up.
struct AudioClient {
    virtual HRESULT Initialize(int mode, DWORD flags, LONGLONG duration,
                               LONGLONG periodicity, const WAVEFORMATEX* format,
                               const GUID* session) = 0;
    virtual HRESULT GetBufferSize(UINT32* frames) = 0;
    virtual HRESULT GetStreamLatency(LONGLONG* latency) = 0;
    virtual HRESULT GetCurrentPadding(UINT32* padding) = 0;
    virtual HRESULT IsFormatSupported(int mode, const WAVEFORMATEX* format,
                                      WAVEFORMATEX** closest) = 0;
    virtual HRESULT GetMixFormat(WAVEFORMATEX** format) = 0;
    virtual HRESULT GetDevicePeriod(LONGLONG* minimum, LONGLONG* alignment) = 0;
    virtual HRESULT Start() = 0;
    virtual HRESULT Stop() = 0;
    virtual HRESULT Reset() = 0;
    virtual HRESULT SetEventHandle(HANDLE event) = 0;
    virtual HRESULT GetService(const GUID& iid, void** out) = 0;
};

struct AudioRenderClient {
    virtual HRESULT GetBuffer(UINT32 frames, BYTE** data) = 0;
    virtual HRESULT ReleaseBuffer(UINT32 frames, DWORD flags) = 0;
};

const GUID kMmDeviceEnumeratorClsid = {0xBCDE0395, 0xE52F, 0x467C,
                                       {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID kMmDeviceEnumeratorIid = {0xA95664D2, 0x9614, 0x4F35,
                                     {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID kAudioClientIid = {0x1CB9AD4C, 0xDBFA, 0x4C32,
                              {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2}};
const GUID kAudioRenderClientIid = {0xF294ACFC, 0x3146, 0x4483,
                                    {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2}};

struct AudioRequest {
    int generation = 0;
    bool plucked = true;
    float damping = 0.9975f;  // How long a plucked string rings.
    float arpeggio = 0.0f;    // Seconds between notes; 0 sounds them together.
    int noteCount = 0;
    int notes[kMaxNotes] = {};
};

struct AudioOutput {
    AudioClient* client = nullptr;
    AudioRenderClient* render = nullptr;
    WAVEFORMATEX* format = nullptr;
    UINT32 frames = 0;
    int channels = 0;
    int bits = 0;
    bool floating = false;
};

CRITICAL_SECTION g_audioLock;
bool g_audioLockReady = false;
AudioRequest g_audioRequest;
HANDLE g_audioWake = nullptr;
HANDLE g_audioStop = nullptr;
HANDLE g_audioThread = nullptr;
double g_audioBusyUntil = 0.0;
// Asking for the device ahead of time, and handing it back once the panel is
// gone, keeps an idle stream from holding the audio engine awake all day.
bool g_audioWarm = false;
bool g_audioRelease = false;

inline void ReleaseUnknown(void* object) {
    if (object) {
        static_cast<AudioUnknown*>(object)->Release();
    }
}

inline float NoteFrequency(int midi) {
    return 440.0f * powf(2.0f, (static_cast<float>(midi) - 69.0f) / 12.0f);
}

inline float AudioNoise(uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return static_cast<float>(state) / 2147483648.0f - 1.0f;
}

// Karplus-Strong: noise recirculated through a two tap average, which is close
// enough to a plucked string for a handful of lines.
void MixPluck(std::vector<float>& out,
              int channels,
              int rate,
              float frequency,
              double start,
              float damping,
              float gain) {
    const int delay =
        std::max(2, static_cast<int>(static_cast<float>(rate) / frequency));
    std::vector<float> line(static_cast<size_t>(delay));
    uint32_t seed = 0x9E3779B9u ^ static_cast<uint32_t>(frequency * 11.0f);
    float previous = 0.0f;
    for (int i = 0; i < delay; i++) {
        previous = 0.5f * (previous + AudioNoise(seed));
        line[static_cast<size_t>(i)] = previous;
    }

    const size_t total = out.size() / static_cast<size_t>(channels);
    const size_t first = static_cast<size_t>(start * rate);
    if (first >= total) {
        return;
    }
    size_t index = 0;
    for (size_t n = first; n < total; n++) {
        const size_t next = (index + 1) % static_cast<size_t>(delay);
        const float value = 0.5f * (line[index] + line[next]) * damping;
        line[index] = value;
        index = next;
        float* frame = &out[n * static_cast<size_t>(channels)];
        frame[0] += value * gain;
        if (channels > 1) {
            frame[1] += value * gain * 0.94f;
        }
    }
}

// A struck string is a stack of slightly inharmonic partials that fade at their
// own pace, which reads as a piano rather than a plain tone.
void MixStrike(std::vector<float>& out,
               int channels,
               int rate,
               float frequency,
               double start,
               float gain) {
    const size_t total = out.size() / static_cast<size_t>(channels);
    const size_t first = static_cast<size_t>(start * rate);
    if (first >= total) {
        return;
    }
    for (int partial = 1; partial <= 8; partial++) {
        const float order = static_cast<float>(partial);
        const float partialFrequency =
            frequency * order * sqrtf(1.0f + 0.0004f * order * order);
        if (partialFrequency > static_cast<float>(rate) * 0.45f) {
            break;
        }
        const float amplitude = gain / powf(order, 1.35f);
        const float step = 6.2831853f * partialFrequency / static_cast<float>(rate);
        const float fall = 1.7f / (0.55f + order * 0.45f);
        const float decay = expf(-1.0f / (fall * static_cast<float>(rate)));
        float envelope = amplitude;
        float phase = order * 0.9f;
        for (size_t n = first; n < total; n++) {
            phase += step;
            const float attack = (n - first) < 128 ? static_cast<float>(n - first) / 128.0f : 1.0f;
            const float value = sinf(phase) * envelope * attack;
            envelope *= decay;
            float* frame = &out[n * static_cast<size_t>(channels)];
            frame[0] += value;
            if (channels > 1) {
                frame[1] += value * 0.96f;
            }
        }
    }
}

void RenderChord(std::vector<float>& out,
                 int channels,
                 int rate,
                 const AudioRequest& request) {
    const double baseLength = request.plucked ? 3.2 : 2.8;
    // A scale asks for a real arpeggio: each note starts well after the last,
    // so it rises where a chord's two-centimetre spread would blur together.
    const bool spaced = request.arpeggio > 0.0f && request.noteCount > 1;
    const double spread =
        spaced ? (request.noteCount - 1) * static_cast<double>(request.arpeggio)
               : 0.0;
    const double length = baseLength + spread;
    out.assign(static_cast<size_t>(length * rate) *
                   static_cast<size_t>(channels),
               0.0f);
    for (int i = 0; i < request.noteCount; i++) {
        const float frequency = NoteFrequency(request.notes[i]);
        const double start =
            spaced ? i * static_cast<double>(request.arpeggio)
                   : i * (request.plucked ? 0.022 : 0.007);
        if (request.plucked) {
            MixPluck(out, channels, rate, frequency, start, request.damping,
                     0.20f);
        } else {
            MixStrike(out, channels, rate, frequency, start, 0.09f);
        }
    }
    // Soft clipping keeps a dense chord from crackling and leaves headroom.
    for (size_t i = 0; i < out.size(); i++) {
        out[i] = tanhf(out[i] * 1.35f) * 0.82f;
    }
    // The tail fades out so the buffer never ends on a step.
    const size_t totalFrames = out.size() / static_cast<size_t>(channels);
    const size_t fade =
        std::min(totalFrames, static_cast<size_t>(0.18 * static_cast<double>(rate)));
    for (size_t i = 0; i < fade; i++) {
        const float tail = static_cast<float>(i) / static_cast<float>(fade);
        float* frame = &out[(totalFrames - fade + i) * static_cast<size_t>(channels)];
        for (int c = 0; c < channels; c++) {
            frame[c] *= tail;
        }
    }
}

void FillAudio(BYTE* data,
               UINT32 frames,
               const std::vector<float>& samples,
               size_t& playhead,
               const AudioOutput& output) {
    const size_t channels = static_cast<size_t>(output.channels);
    const size_t total = samples.size() / channels;
    for (UINT32 i = 0; i < frames; i++, playhead++) {
        float left = 0.0f;
        float right = 0.0f;
        if (playhead < total) {
            const float* source = &samples[playhead * channels];
            left = source[0];
            right = channels > 1 ? source[1] : left;
        }
        const size_t frame = static_cast<size_t>(i) * channels;
        if (output.bits == 16) {
            auto* target = reinterpret_cast<int16_t*>(data) + frame;
            target[0] = static_cast<int16_t>(left * 32767.0f);
            for (size_t c = 1; c < channels; c++) {
                target[c] = static_cast<int16_t>(right * 32767.0f);
            }
        } else if (output.floating) {
            auto* target = reinterpret_cast<float*>(data) + frame;
            target[0] = left;
            for (size_t c = 1; c < channels; c++) {
                target[c] = right;
            }
        } else {
            auto* target = reinterpret_cast<int32_t*>(data) + frame;
            target[0] = static_cast<int32_t>(left * 2147483000.0f);
            for (size_t c = 1; c < channels; c++) {
                target[c] = static_cast<int32_t>(right * 2147483000.0f);
            }
        }
    }
}

void CloseAudio(AudioOutput& output) {
    ReleaseUnknown(output.render);
    ReleaseUnknown(output.client);
    if (output.format) {
        CoTaskMemFree(output.format);
    }
    output = AudioOutput();
}

bool OpenAudio(AudioOutput& output) {
    AudioDeviceEnumerator* enumerator = nullptr;
    if (FAILED(CoCreateInstance(kMmDeviceEnumeratorClsid, nullptr, CLSCTX_ALL,
                                kMmDeviceEnumeratorIid,
                                reinterpret_cast<void**>(&enumerator))) ||
        !enumerator) {
        Wh_Log(L"Audio: no device enumerator");
        return false;
    }
    AudioDevice* device = nullptr;
    const HRESULT deviceResult = enumerator->GetDefaultAudioEndpoint(
        0 /* eRender */, 0 /* eConsole */, &device);
    ReleaseUnknown(enumerator);
    if (FAILED(deviceResult) || !device) {
        Wh_Log(L"Audio: no default output device");
        return false;
    }
    AudioClient* client = nullptr;
    const HRESULT activateResult = device->Activate(
        kAudioClientIid, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client));
    ReleaseUnknown(device);
    if (FAILED(activateResult) || !client) {
        Wh_Log(L"Audio: no audio client");
        return false;
    }

    WAVEFORMATEX* format = nullptr;
    if (FAILED(client->GetMixFormat(&format)) || !format) {
        Wh_Log(L"Audio: no mix format");
        ReleaseUnknown(client);
        return false;
    }
    constexpr int kSharedMode = 0;
    if (FAILED(client->Initialize(kSharedMode, 0, 0, 0, format, nullptr))) {
        Wh_Log(L"Audio: shared mode init failed");
        CoTaskMemFree(format);
        ReleaseUnknown(client);
        return false;
    }
    AudioRenderClient* render = nullptr;
    if (FAILED(client->GetService(kAudioRenderClientIid,
                                  reinterpret_cast<void**>(&render))) ||
        !render) {
        Wh_Log(L"Audio: no render client");
        CoTaskMemFree(format);
        ReleaseUnknown(client);
        return false;
    }

    UINT32 frames = 0;
    client->GetBufferSize(&frames);
    if (frames == 0) {
        Wh_Log(L"Audio: empty buffer");
        ReleaseUnknown(render);
        CoTaskMemFree(format);
        ReleaseUnknown(client);
        return false;
    }

    bool floating = format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        const auto* extensible =
            reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(format);
        floating = extensible->SubFormat.Data1 == 3;  // 3 is IEEE float.
    }

    output.client = client;
    output.render = render;
    output.format = format;
    output.frames = frames;
    output.channels = format->nChannels > 0 ? format->nChannels : 2;
    output.bits = format->wBitsPerSample;
    output.floating = floating;
    Wh_Log(L"Audio: %u Hz, %d channels, %d bits, float %d",
           format->nSamplesPerSec, output.channels, output.bits,
           floating ? 1 : 0);
    return true;
}

DWORD WINAPI AudioThreadProc(void*) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    AudioOutput output;
    bool opened = OpenAudio(output);
    std::vector<float> samples;
    size_t playhead = 0;
    int rendered = 0;
    bool running = false;

    while (WaitForSingleObject(g_audioStop, 0) != WAIT_OBJECT_0) {
        if (!opened) {
            bool wanted = g_audioWarm;
            g_audioWarm = false;
            EnterCriticalSection(&g_audioLock);
            if (g_audioRequest.generation != rendered) {
                wanted = true;
            }
            LeaveCriticalSection(&g_audioLock);
            if (wanted) {
                opened = OpenAudio(output);
                if (opened) {
                    continue;
                }
            }
            WaitForSingleObject(g_audioWake, 500);
            continue;
        }
        g_audioWarm = false;  // The device is up, so nothing is pending.

        AudioRequest request;
        bool pending = false;
        EnterCriticalSection(&g_audioLock);
        if (g_audioRequest.generation != rendered) {
            request = g_audioRequest;
            pending = true;
        }
        LeaveCriticalSection(&g_audioLock);

        if (pending) {
            if (running) {
                output.client->Stop();
                running = false;
            }
            RenderChord(samples, output.channels,
                        static_cast<int>(output.format->nSamplesPerSec), request);
            playhead = 0;
            rendered = request.generation;
        }

        if (playhead >= samples.size() / static_cast<size_t>(output.channels)) {
            if (running) {
                output.client->Stop();
                running = false;
            }
            if (g_audioRelease) {
                g_audioRelease = false;
                CloseAudio(output);
                opened = false;
                samples.clear();
                continue;
            }
            WaitForSingleObject(g_audioWake, 200);
            continue;
        }
        if (!running) {
            if (FAILED(output.client->Start())) {
                Wh_Log(L"Audio: start failed");
                CloseAudio(output);
                opened = false;
                continue;
            }
            running = true;
        }

        UINT32 padding = 0;
        if (FAILED(output.client->GetCurrentPadding(&padding))) {
            Sleep(5);
            continue;
        }
        const UINT32 available =
            output.frames > padding ? output.frames - padding : 0;
        if (available == 0) {
            Sleep(2);
            continue;
        }
        BYTE* data = nullptr;
        if (FAILED(output.render->GetBuffer(available, &data))) {
            Sleep(2);
            continue;
        }
        FillAudio(data, available, samples, playhead, output);
        output.render->ReleaseBuffer(available, 0);
    }

    if (running) {
        output.client->Stop();
    }
    CloseAudio(output);
    CoUninitialize();
    return 0;
}

void AudioPrepare() {
    if (g_audioThread) {
        g_audioRelease = false;
        g_audioWarm = true;
        if (g_audioWake) {
            SetEvent(g_audioWake);
        }
        return;
    }
    InitializeCriticalSection(&g_audioLock);
    g_audioLockReady = true;
    g_audioWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_audioStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0, nullptr);
}

void AudioRelease() {
    if (!g_audioThread) {
        return;
    }
    g_audioRelease = true;
    if (g_audioWake) {
        SetEvent(g_audioWake);
    }
}

void AudioPlay(const AudioRequest& request) {
    AudioPrepare();
    if (!g_audioLockReady || !g_audioWake) {
        return;
    }
    EnterCriticalSection(&g_audioLock);
    g_audioRequest = request;
    g_audioRequest.generation++;
    LeaveCriticalSection(&g_audioLock);
    SetEvent(g_audioWake);
    g_audioBusyUntil = NowMs() + 2600.0;
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kAudioTimer, 250, nullptr);
    }
}

void AudioShutdown() {
    if (g_audioStop) {
        SetEvent(g_audioStop);
    }
    if (g_audioThread) {
        WaitForSingleObject(g_audioThread, 2000);
        CloseHandle(g_audioThread);
        g_audioThread = nullptr;
    }
    if (g_audioWake) {
        CloseHandle(g_audioWake);
        g_audioWake = nullptr;
    }
    if (g_audioStop) {
        CloseHandle(g_audioStop);
        g_audioStop = nullptr;
    }
    if (g_audioLockReady) {
        DeleteCriticalSection(&g_audioLock);
        g_audioLockReady = false;
    }
}

#endif  // Chord playback

////////////////////////////////////////////////////////////////////////////////
// Metronome audio
//
// winmm keeps this small: one shared mode stream, refilled block by block on
// its own thread, with every click mixed in at an exact sample offset from the
// beat grid. The message loop is never part of the timing, so a busy desktop
// cannot make the tempo wobble the way a WM_TIMER click would.

constexpr int kMetroRate = 44100;
constexpr int kMetroChannels = 2;
constexpr int kMetroBlockFrames = 512;  // About 11.6 ms per block.
// Eight blocks put about 93 ms of audio in the queue. Three is enough for the
// mixing but not for Windows: a single repaint that held this thread for more
// than 35 ms emptied the queue, and every gap the device then played is heard
// as a crackle that drifts against the beat.
constexpr int kMetroBlockCount = 8;
constexpr int kMetroTail = 66150;      // 1.5 s, long enough for a strum to ring.
constexpr int kMetroLevels = 4;

struct MetroConfig {
    double bpm = 120.0;
    int beats = 4;
    int subdivision = 1;
    int group = 1;
    int volume = 70;
};

// Written by the panel, read by the audio thread once per block.
struct MetroShared {
    std::atomic<int> bpmMilli{120000};
    std::atomic<int> beats{4};
    std::atomic<int> subdivision{1};
    std::atomic<int> group{1};
    std::atomic<int> volume{70};
    std::atomic<int> beat{0};
    std::atomic<unsigned> pulse{0};
    std::atomic<bool> running{false};
    // Strum layer. The panel publishes the grid once per edit as packed
    // cells (stroke in the low 3 bits, then accent / ghost / tie flags); the
    // engine reads them on the same subdivision steps it already walks, so
    // count-ins and strums land on exactly the same sample grid. strumSound is
    // the picked sound, which a cell's own mute can only deepen. slot feeds the
    // playhead back to the panel, strumCounting says whether that bar is still
    // the count-in, measureTick drives the practice ramp, and strumRestart is
    // bumped when a new pattern starts, so the engine can put its first stroke
    // at the top of a bar itself.
    std::atomic<bool> strum{false};
    std::atomic<int> strumSound{0};
    std::atomic<bool> countIn{false};
    std::atomic<unsigned> strumRestart{0};
    std::atomic<int> slot{0};
    std::atomic<unsigned> measureTick{0};
    // The engine's own count-in flag, carried back on the same delayed cue as
    // the slot, so the playhead can tell a count-in bar from a played one from
    // its very first stroke.
    std::atomic<bool> strumCounting{false};
    std::atomic<int> grid[kStrumSlotsMax]{};
};

MetroShared g_metroShared;

MetroConfig ReadMetroConfig() {
    MetroConfig c;
    c.bpm = std::clamp(g_metroShared.bpmMilli.load() / 1000.0, 20.0, 400.0);
    c.beats = std::clamp(g_metroShared.beats.load(), 1, 16);
    c.subdivision = std::clamp(g_metroShared.subdivision.load(), 1, 4);
    c.group = std::clamp(g_metroShared.group.load(), 1, 4);
    c.volume = std::clamp(g_metroShared.volume.load(), 0, 100);
    return c;
}

// The beat tones: the downbeat, the start of each group, the plain beat and
// the subdivision. They are four notes of one A chord - the high octave, a
// fifth between it and the beat, the beat itself, then the same note two
// octaves down - so every click belongs to the same family. That is what
// makes a bar read as a pulse instead of a tune, and it keeps the clicks from
// competing with the music in the other tools.
struct MetroBeepSpec {
    float frequency;
    float level;    // Peak, before the volume setting.
    float seconds;  // Length of the beep, tail included.
};

constexpr MetroBeepSpec kMetroBeeps[kMetroLevels] = {
    {880.0f, 0.62f, 0.075f},
    {660.0f, 0.44f, 0.065f},
    {440.0f, 0.38f, 0.055f},
    {220.0f, 0.22f, 0.045f},
};

// What one buffered block has to tell the panel. The queue runs several blocks
// ahead of the speaker, so a cue is held back until the block it belongs to is
// the one playing; otherwise the beat dot and the strum playhead would lead the
// click by the whole length of the queue.
struct MetroBlockCue {
    bool hasBeat = false;
    int beat = 0;
    bool hasSlot = false;
    int slot = 0;
    // The count-in bar is clicks only, and the panel has to know it is still
    // running: the measure count only ticks once a played bar has ended, so it
    // cannot tell the first played bar from the bar before it.
    bool counting = false;
    unsigned pulses = 0;
    unsigned measures = 0;
};

struct MetroAudio {
    HWAVEOUT device = nullptr;
    HANDLE audioEvent = nullptr;
    HANDLE stopEvent = nullptr;
    HANDLE readyEvent = nullptr;
    HANDLE thread = nullptr;
    WAVEHDR headers[kMetroBlockCount]{};
    std::vector<short> pcm[kMetroBlockCount];
    // The four beat tones, rendered once when the stream opens.
    std::vector<float> clicks[kMetroLevels];
    // Holds the block about to be played plus the tails of the clicks that run
    // past its end, so a click is never cut off at a block boundary.
    std::vector<float> canvas;
    bool clicksBuilt = false;
    bool prepared = false;
    long long written = 0;
    double nextEvent = 0.0;
    MetroBlockCue cue[kMetroBlockCount];
    int beat = 0;
    int sub = 0;
    // Strum voices, rendered when a pattern starts and whenever the strings
    // change: one per stroke direction and sound, so a stroke picks its voice
    // by index and nothing is ever synthesised on the audio thread.
    std::vector<float> strumVoice[2][kStrumSoundCount];
    int strumSignature = -1;  // The instrument and chord the voices hold.
    bool strumBuilt = false;
    bool countingIn = false;
    unsigned seenRestart = 0;  // The strumRestart this grid last started from.
};

MetroAudio g_metroAudio;

// Renders the four beat tones. A beep is a short sine with a breath of third
// harmonic, so it reads as a clean tone rather than a test signal, and it is
// shaped by a fast attack and an exponential fall so its edges never click.
void BuildMetroClicks(MetroAudio& a) {
    const float attack = 0.0012f * kMetroRate;
    const float release = 0.0050f * kMetroRate;
    for (int level = 0; level < kMetroLevels; level++) {
        const MetroBeepSpec& spec = kMetroBeeps[level];
        const int frames = static_cast<int>(spec.seconds * kMetroRate);
        std::vector<float>& out = a.clicks[level];
        out.assign(static_cast<size_t>(frames), 0.0f);
        // The tone is down by about 26 dB by the time the buffer ends, which
        // leaves the release ramp nothing abrupt to cut.
        const float decay = 3.0f / spec.seconds;
        float phase = 0.0f;
        for (int i = 0; i < frames; i++) {
            const float time = static_cast<float>(i) / kMetroRate;
            phase += 6.2831853f * spec.frequency / kMetroRate;
            const float value = sinf(phase) + 0.06f * sinf(phase * 3.0f);
            float gain = expf(-decay * time);
            if (i < attack) {
                gain *= static_cast<float>(i) / attack;
            }
            if (frames - i < release) {
                gain *= static_cast<float>(frames - i) / release;
            }
            out[static_cast<size_t>(i)] = value * gain * spec.level;
        }
    }
    a.clicksBuilt = true;
}

// Adds one click at a sample offset inside the canvas.
void MixMetroClick(MetroAudio& a, int level, int offset) {
    const std::vector<float>& click =
        a.clicks[std::clamp(level, 0, kMetroLevels - 1)];
    const int room = static_cast<int>(a.canvas.size()) - offset;
    const int count =
        std::min(static_cast<int>(click.size()), std::min(room, kMetroTail));
    float* target = a.canvas.data() + offset;
    for (int i = 0; i < count; i++) {
        target[i] += click[static_cast<size_t>(i)];
    }
}

// Defined by the acoustic sampler further down. The pattern builder does not
// carry its own tone bank: a strum is the very strings the chord card and the
// circle of fifths play, so the whole toolkit speaks with one voice.
void ClipRenderPluck(std::vector<float>& out, int midi, uint32_t salt);

// The three sounds, in order of damping: an open stroke rings the whole tail, a
// palm mute chokes in half a second, and a mute is a scratch that stops almost
// at once. The rate is the exponent of the choke laid over the plucks, so a
// heavier sound simply decays faster.
constexpr float kStrumRing[kStrumSoundCount] = {1.5f, 0.55f, 0.12f};
constexpr float kStrumChoke[kStrumSoundCount] = {2.0f, 26.0f, 70.0f};

// The strings a stroke strikes: the instrument's open tuning, or the notes of
// the picked chord's first voicing, which is the same voicing the chord card
// shows. A chord the library cannot spell falls back to the open strings, so a
// stroke always sounds.
int StrumStrings(int* notes, int maxNotes) {
    const int instrument =
        std::clamp(g_strum.instrument, 0, kInstrumentCount - 1);
    const chorddata::InstrumentData& info =
        chorddata::kInstruments[instrument];
    const int choice = std::clamp(g_strum.chord, 0, kStrumChordCount - 1);
    const StrumChord& picked = kStrumChords[choice];
    if (picked.pitch < 0 || !EnsureLibrary(instrument)) {
        int count = 0;
        for (int i = 0; i < info.strings && count < maxNotes; i++) {
            notes[count++] = info.tuning[i];
        }
        return count;
    }
    // Open position chords only need the first voicing, which is where the
    // library keeps the one a player would grab.
    const Library& library = g_libraries[instrument];
    for (const ChordEntry& entry : library.chords) {
        if (entry.suffix != (picked.minor ? 1 : 0) ||
            entry.key >= info.keyCount ||
            KeyPitchClass(info.keys[entry.key]) != picked.pitch) {
            continue;
        }
        Voicing voicing;
        if (!ReadVoicing(instrument, entry, 0, voicing) ||
            voicing.noteCount <= 0) {
            continue;
        }
        int count = 0;
        for (int i = 0; i < voicing.noteCount && count < maxNotes; i++) {
            notes[count++] = voicing.notes[i];
        }
        return count;
    }
    int count = 0;
    for (int i = 0; i < info.strings && count < maxNotes; i++) {
        notes[count++] = info.tuning[i];
    }
    return count;
}

// One strum is the instrument's strings struck a few milliseconds apart. A down
// stroke rolls bass to treble, an up stroke comes back the way a hand does; the
// sound decides how hard the hand chokes the ring, from a full tail down to the
// scratch of a palm laid across the strings.
void RenderStrumVoice(std::vector<float>& out, const int* notes, int count,
                      bool up, int sound, uint32_t seedSalt) {
    sound = std::clamp(sound, 0, kStrumSoundCount - 1);
    const float spread = up ? 0.0045f : 0.0065f;  // Seconds between strings.
    const int frames = static_cast<int>(kStrumRing[sound] * kMetroRate);
    out.assign(static_cast<size_t>(frames), 0.0f);

    std::vector<float> pluck;
    for (int s = 0; s < count; s++) {
        // A down stroke starts on the lowest string, an up stroke on the
        // highest, which is the order the strings sit in on the instrument.
        const int note = up ? notes[count - 1 - s] : notes[s];
        const int offset = static_cast<int>(spread * kMetroRate) * s;
        if (offset >= frames) {
            continue;
        }
        ClipRenderPluck(pluck, note, seedSalt + static_cast<uint32_t>(s));
        // Each string leans in at its own weight, so the stroke is not a wall.
        const float weight = 0.86f + 0.04f * static_cast<float>(s % 3);
        const int span =
            std::min(static_cast<int>(pluck.size()), frames - offset);
        for (int i = 0; i < span; i++) {
            out[static_cast<size_t>(offset + i)] +=
                pluck[static_cast<size_t>(i)] * weight;
        }
    }

    // The choke rides on the strings rather than replacing them: a plain strum
    // is damped by the hand a beat later, which is also what keeps a fast
    // pattern from piling ringing chords on top of each other.
    float peak = 0.0f;
    for (int i = 0; i < frames; i++) {
        float value = out[static_cast<size_t>(i)];
        if (kStrumChoke[sound] > 0.0f) {
            value *= expf(-kStrumChoke[sound] * static_cast<float>(i) /
                          kMetroRate);
        }
        out[static_cast<size_t>(i)] = value;
        peak = std::max(peak, fabsf(value));
    }
    // Every voice is levelled alike, so accent and ghost stay the only
    // dynamics the pattern needs.
    const float norm = peak > 1e-6f ? 0.55f / peak : 0.0f;
    for (int i = 0; i < frames; i++) {
        out[static_cast<size_t>(i)] *= norm;
    }
}

// Renders the whole voice set for the strings currently picked. The signature
// is what lets the panel skip the work while the instrument and the chord stay
// put; it is not read by the audio thread.
void BuildStrumVoices(MetroAudio& a, int signature) {
    int notes[kMaxNotes];
    const int count = StrumStrings(notes, kMaxNotes);
    uint32_t salt = 0x1234567u;
    for (int sound = 0; sound < kStrumSoundCount; sound++) {
        for (int dir = 0; dir < 2; dir++) {
            RenderStrumVoice(a.strumVoice[dir][sound], notes, count, dir != 0,
                             sound, salt++);
        }
    }
    a.strumSignature = signature;
    a.strumBuilt = true;
}

// Adds one strum voice at a sample offset, with the accent and ghost gains
// folded in so a single render serves every dynamic.
void MixStrumVoice(MetroAudio& a, bool up, int sound, bool accent, bool ghost,
                   int offset) {
    const std::vector<float>& voice =
        a.strumVoice[up ? 1 : 0][std::clamp(sound, 0, kStrumSoundCount - 1)];
    const float gain = (accent ? 1.35f : 1.0f) * (ghost ? 0.5f : 1.0f);
    const int room = static_cast<int>(a.canvas.size()) - offset;
    const int count =
        std::min(static_cast<int>(voice.size()), std::min(room, kMetroTail));
    if (count <= 0) {
        return;
    }
    float* target = a.canvas.data() + offset;
    for (int i = 0; i < count; i++) {
        target[i] += voice[static_cast<size_t>(i)] * gain;
    }
}

void FillMetroBlock(MetroAudio& a, const MetroConfig& cfg, short* out) {
    if (!a.clicksBuilt) {
        BuildMetroClicks(a);
    }
    // A new pattern asks for the bar to start at the top: the panel bumps
    // strumRestart, and the next block arms the count-in and puts the first
    // stroke on its own first sample. The beat counter belongs to this thread,
    // so the panel never has to write it.
    const unsigned restart = g_metroShared.strumRestart.load();
    if (restart != a.seenRestart) {
        a.seenRestart = restart;
        a.beat = 0;
        a.sub = 0;
        a.countingIn = g_metroShared.strum.load() && g_metroShared.countIn.load();
        a.nextEvent = static_cast<double>(a.written);
    }
    // The strum voices are rendered by the panel before it publishes a pattern,
    // so there is nothing to synthesise here; a stroke that arrives before its
    // voice exists is simply silent rather than late.
    std::fill(a.canvas.begin() + kMetroTail, a.canvas.end(), 0.0f);

    const double stepFrames =
        60.0 * kMetroRate / cfg.bpm / static_cast<double>(cfg.subdivision);
    if (a.sub >= cfg.subdivision) {
        a.sub = 0;  // The subdivision changed under the click pattern.
    }
    if (a.beat >= cfg.beats) {
        a.beat = 0;  // So did the meter.
    }
    const bool strum = g_metroShared.strum.load();
    MetroBlockCue cue;  // What this block tells the panel, published later.
    // A block wholly inside the count-in says so; the block that carries the
    // first played step clears it again below, since the playhead has a slot to
    // follow from then on.
    cue.counting = strum && a.countingIn;
    while (a.nextEvent < static_cast<double>(a.written + kMetroBlockFrames)) {
        // The stream runs for hours, so the offset is kept wide rather than
        // truncated to an int that would wrap after a while.
        const long long offset = static_cast<long long>(a.nextEvent) - a.written;
        if (offset >= 0 && offset < kMetroBlockFrames) {
            const bool onBeat = a.sub == 0;
            int level = 3;
            if (onBeat) {
                level = a.beat == 0 ? 0
                        : (cfg.group > 1 && a.beat % cfg.group == 0) ? 1
                                                                    : 2;
            }
            // The strumming tool keeps no click of its own: the only one it
            // ever mixes is the count-in bar, which is what arms the tempo.
            if (!strum || a.countingIn) {
                MixMetroClick(a, level, static_cast<int>(offset));
            }
            if (strum) {
                if (!a.countingIn) {
                    // The same subdivision step the click grid uses is exactly
                    // one pattern slot, so the strum lands with the count.
                    const int slot = std::min(
                        a.beat * cfg.subdivision + a.sub, kStrumSlotsMax - 1);
                    const int packed =
                        g_metroShared.grid[static_cast<size_t>(slot)].load();
                    cue.slot = slot;
                    cue.hasSlot = true;
                    cue.counting = false;  // The played bars begin here.
                    const int stroke = packed & 7;
                    const bool accent = (packed & 8) != 0;
                    const bool ghost = (packed & 16) != 0;
                    const bool tie = (packed & 32) != 0;
                    if (stroke != kStrokeRest && !tie) {
                        const bool up =
                            stroke == kStrokeUp || stroke == kStrokeUpMute;
                        const bool strokeMute = stroke == kStrokeDownMute ||
                                                stroke == kStrokeUpMute;
                        // A written mute is heavier than the picked sound, so
                        // the scratch survives whatever the voice is set to.
                        const int sound = std::max(
                            g_metroShared.strumSound.load(),
                            strokeMute ? static_cast<int>(kStrumMuted)
                                       : static_cast<int>(kStrumOpen));
                        MixStrumVoice(a, up, sound, accent, ghost,
                                      static_cast<int>(offset));
                    }
                    cue.pulses++;
                }
            } else if (onBeat) {
                cue.beat = a.beat;
                cue.hasBeat = true;
                cue.pulses++;
            }
        }
        a.sub++;
        if (a.sub >= cfg.subdivision) {
            a.sub = 0;
            a.beat = (a.beat + 1) % cfg.beats;
            if (a.beat == 0) {
                // A full measure just ended. The count-in bar is only clicks,
                // so it retires here; a played measure ticks the practice ramp.
                if (a.countingIn) {
                    a.countingIn = false;
                } else if (strum) {
                    cue.measures++;
                }
            }
        }
        a.nextEvent += stepFrames;
    }
    // Park this block's cue, then hand the panel the cue of the block that is
    // about to be heard: the queue holds kMetroBlockCount blocks, so this fill
    // pushes the oldest of them out to the speaker.
    const long long blockIndex = a.written / kMetroBlockFrames;
    a.cue[static_cast<size_t>(blockIndex % kMetroBlockCount)] = cue;
    const long long due = blockIndex - (kMetroBlockCount - 1);
    if (due >= 0) {
        const MetroBlockCue& heard =
            a.cue[static_cast<size_t>(due % kMetroBlockCount)];
        if (heard.pulses > 0) {
            g_metroShared.pulse.fetch_add(heard.pulses);
        }
        if (heard.measures > 0) {
            g_metroShared.measureTick.fetch_add(heard.measures);
        }
        if (heard.hasBeat) {
            g_metroShared.beat.store(heard.beat);
        }
        if (heard.hasSlot) {
            g_metroShared.slot.store(heard.slot);
        }
        g_metroShared.strumCounting.store(heard.counting);
    }
    a.written += kMetroBlockFrames;

    const float gain = static_cast<float>(cfg.volume) / 100.0f;
    for (int i = 0; i < kMetroBlockFrames; i++) {
        const float value = std::clamp(a.canvas[static_cast<size_t>(i)] * gain,
                                       -1.0f, 1.0f);
        const short sample = static_cast<short>(lroundf(value * 32000.0f));
        out[i * kMetroChannels] = sample;
        out[i * kMetroChannels + 1] = sample;
    }
    // Slide the tails down to the front of the next block.
    for (int i = 0; i < kMetroTail; i++) {
        a.canvas[static_cast<size_t>(i)] =
            a.canvas[static_cast<size_t>(i + kMetroBlockFrames)];
    }
}

void CloseMetroAudio(MetroAudio& a) {
    if (a.device) {
        waveOutReset(a.device);
        if (a.prepared) {
            for (int i = 0; i < kMetroBlockCount; i++) {
                waveOutUnprepareHeader(a.device, &a.headers[i],
                                       sizeof(WAVEHDR));
            }
        }
        waveOutClose(a.device);
        a.device = nullptr;
    }
    a.prepared = false;
}

void StopMetroAudio();

DWORD WINAPI MetroThreadProc(void*) {
    MetroAudio& a = g_metroAudio;
    // The queue is refilled a block at a time, so this thread has to outrank
    // the panel's painting: a refill that waits behind a frame is already too
    // late by the time it runs. It sleeps between blocks, so raising it costs
    // the rest of the process nothing.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    a.audioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    a.stopEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    bool opened = false;
    if (a.audioEvent && a.stopEvent) {
        WAVEFORMATEX format{};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = kMetroChannels;
        format.nSamplesPerSec = kMetroRate;
        format.wBitsPerSample = 16;
        format.nBlockAlign = static_cast<WORD>(kMetroChannels * 2);
        format.nAvgBytesPerSec = kMetroRate * format.nBlockAlign;
        opened = waveOutOpen(&a.device, WAVE_MAPPER, &format,
                             reinterpret_cast<DWORD_PTR>(a.audioEvent), 0,
                             CALLBACK_EVENT) == MMSYSERR_NOERROR;
        if (!opened) {
            a.device = nullptr;
            Wh_Log(L"Metronome: no audio output device");
        }
    }

    if (opened) {
        a.canvas.assign(static_cast<size_t>(kMetroTail + kMetroBlockFrames),
                        0.0f);
        for (int i = 0; i < kMetroBlockCount; i++) {
            a.pcm[i].assign(
                static_cast<size_t>(kMetroBlockFrames) * kMetroChannels, 0);
            a.headers[i] = WAVEHDR{};
            a.headers[i].lpData = reinterpret_cast<LPSTR>(a.pcm[i].data());
            a.headers[i].dwBufferLength =
                static_cast<DWORD>(a.pcm[i].size() * sizeof(short));
            waveOutPrepareHeader(a.device, &a.headers[i], sizeof(WAVEHDR));
        }
        a.prepared = true;

        // Prime the queue before the first beat, then keep refilling whatever
        // the device has finished with.
        MetroConfig cfg = ReadMetroConfig();
        bool primed = true;
        for (int i = 0; i < kMetroBlockCount; i++) {
            FillMetroBlock(a, cfg, a.pcm[i].data());
            if (waveOutWrite(a.device, &a.headers[i], sizeof(WAVEHDR)) !=
                MMSYSERR_NOERROR) {
                primed = false;
                break;
            }
        }
        if (primed) {
            g_metroShared.running.store(true);
        }
        if (a.readyEvent) {
            // Only now can a caller tell a running stream from a dead one.
            SetEvent(a.readyEvent);
        }

        const HANDLE waits[2] = {a.audioEvent, a.stopEvent};
        for (; primed;) {
            // The device signals this event as each block finishes, so the
            // wait normally returns at once; the short timeout only catches a
            // lost signal before it costs more than a block of audio.
            const DWORD result = WaitForMultipleObjects(2, waits, FALSE, 25);
            if (result == WAIT_OBJECT_0 + 1) {
                break;
            }
            cfg = ReadMetroConfig();
            bool failed = false;
            for (int i = 0; i < kMetroBlockCount; i++) {
                if (!(a.headers[i].dwFlags & WHDR_DONE)) {
                    continue;
                }
                FillMetroBlock(a, cfg, a.pcm[i].data());
                if (waveOutWrite(a.device, &a.headers[i], sizeof(WAVEHDR)) !=
                    MMSYSERR_NOERROR) {
                    failed = true;
                    break;
                }
            }
            if (failed) {
                Wh_Log(L"Metronome: the output device went away");
                break;
            }
        }
    }

    g_metroShared.running.store(false);
    CloseMetroAudio(a);
    if (a.readyEvent) {
        SetEvent(a.readyEvent);  // Releases a start that is still waiting.
    }

    // A late wake must not outlive the loop.
    if (a.stopEvent) {
        SetEvent(a.stopEvent);
    }
    return 0;
}

bool StartMetroAudio() {
    MetroAudio& a = g_metroAudio;
    if (a.thread && WaitForSingleObject(a.thread, 0) == WAIT_OBJECT_0) {
        // The device was lost while the panel was away, so clear the remains
        // of that thread before opening a new one.
        StopMetroAudio();
    }
    if (a.thread) {
        return true;
    }
    a.written = 0;
    a.nextEvent = 0.0;
    a.beat = 0;
    a.sub = 0;
    a.countingIn = g_metroShared.strum.load() && g_metroShared.countIn.load();
    a.readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!a.readyEvent) {
        return false;
    }
    a.thread = CreateThread(nullptr, 0, MetroThreadProc, nullptr, 0, nullptr);
    if (!a.thread) {
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
        return false;
    }
    WaitForSingleObject(a.readyEvent, 2000);
    if (!g_metroShared.running.load()) {
        WaitForSingleObject(a.thread, 1000);
        CloseHandle(a.thread);
        a.thread = nullptr;
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
        if (a.audioEvent) {
            CloseHandle(a.audioEvent);
            a.audioEvent = nullptr;
        }
        if (a.stopEvent) {
            CloseHandle(a.stopEvent);
            a.stopEvent = nullptr;
        }
        return false;
    }
    return true;
}

void StopMetroAudio() {
    MetroAudio& a = g_metroAudio;
    if (a.stopEvent) {
        SetEvent(a.stopEvent);
    }
    if (a.thread) {
        WaitForSingleObject(a.thread, 2000);
        CloseHandle(a.thread);
        a.thread = nullptr;
    }
    if (a.readyEvent) {
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
    }
    if (a.audioEvent) {
        CloseHandle(a.audioEvent);
        a.audioEvent = nullptr;
    }
    if (a.stopEvent) {
        CloseHandle(a.stopEvent);
        a.stopEvent = nullptr;
    }
    g_metroShared.running.store(false);
    // The thread has been joined, so its buffers can go too.
    a = MetroAudio();
}

////////////////////////////////////////////////////////////////////////////////
// Tuner input
//
// A microphone hands over 16 bit mono in short blocks; a thread on the other
// side folds them down to a quarter of the rate, runs an autocorrelation over
// the last fifth of a second and publishes the frequency it found. Only the
// result crosses back to the panel, so drawing never waits for the microphone.

constexpr int kTunerRate = 44100;
constexpr int kTunerBufferCount = 3;
constexpr int kTunerBufferFrames = 512;   // About 11.6 ms of input per block.
constexpr int kTunerDecimate = 4;         // A quarter of the rate is enough.
constexpr int kTunerWindow = 2048;        // About 186 ms at 11 kHz.
constexpr int kTunerHop = 1024;            // A fresh reading every 93 ms.
constexpr float kTunerMinHz = 25.0f;      // The low B of a five string bass.
constexpr float kTunerMaxHz = 1200.0f;    // Well past a violin's E string.
constexpr float kTunerSilence = 0.0015f;  // Below this the room is just noise.
constexpr float kTunerClean = 0.55f;      // Less than this is not a note.

// Written by the audio thread, read by the panel once per reading.
struct TunerShared {
    std::atomic<float> frequency{0.0f};
    std::atomic<float> level{0.0f};
    std::atomic<float> confidence{0.0f};
    std::atomic<unsigned> reading{0};
    std::atomic<bool> running{false};
};

TunerShared g_tunerShared;

struct TunerAudio {
    HWAVEIN device = nullptr;
    HANDLE audioEvent = nullptr;
    HANDLE stopEvent = nullptr;
    HANDLE readyEvent = nullptr;
    HANDLE thread = nullptr;
    WAVEHDR headers[kTunerBufferCount]{};
    std::vector<short> pcm[kTunerBufferCount];
    std::vector<float> history;    // Decimated samples, oldest at position.
    std::vector<float> frame;       // The same history, straight through.
    std::vector<float> difference;  // Scratch for the autocorrelation.
    std::vector<float> normalized;
    int position = 0;
    int sinceAnalysis = 0;
    float box = 0.0f;
    int boxCount = 0;
    int rate = kTunerRate;
    bool prepared = false;
};

TunerAudio g_tunerAudio;

// YIN's first two stages. The squared difference says how unlike the signal is
// with itself at every lag, and the running mean divides that by the average so
// far, which is what keeps a strong harmonic from winning over the fundamental.
float DetectPitch(TunerAudio& a, int rate, float& confidence) {
    confidence = 0.0f;
    const int minLag = std::max(
        2, static_cast<int>(static_cast<float>(rate) / kTunerMaxHz));
    const int maxLag = std::min(
        kTunerWindow / 2,
        static_cast<int>(static_cast<float>(rate) / kTunerMinHz));
    if (maxLag <= minLag + 2) {
        return 0.0f;
    }
    const int width = kTunerWindow - maxLag;
    const float* x = a.frame.data();

    a.difference.assign(static_cast<size_t>(maxLag + 2), 0.0f);
    for (int lag = std::max(1, minLag - 1); lag <= maxLag + 1; lag++) {
        float sum = 0.0f;
        for (int i = 0; i < width; i++) {
            const float delta = x[i] - x[i + lag];
            sum += delta * delta;
        }
        a.difference[static_cast<size_t>(lag)] = sum;
    }

    a.normalized.assign(static_cast<size_t>(maxLag + 2), 1.0f);
    float running = 0.0f;
    for (int lag = 1; lag <= maxLag; lag++) {
        running += a.difference[static_cast<size_t>(lag)];
        a.normalized[static_cast<size_t>(lag)] =
            running > 0.0f
                ? a.difference[static_cast<size_t>(lag)] *
                      static_cast<float>(lag) / running
                : 1.0f;
    }

    // The first dip that is low enough is the period; everything after it is a
    // multiple of that period, which is how an octave error creeps in.
    int best = -1;
    for (int lag = minLag; lag <= maxLag; lag++) {
        if (a.normalized[static_cast<size_t>(lag)] < 0.16f) {
            while (lag + 1 < maxLag &&
                   a.normalized[static_cast<size_t>(lag + 1)] <
                       a.normalized[static_cast<size_t>(lag)]) {
                lag++;
            }
            best = lag;
            break;
        }
    }
    if (best < 0) {
        // Nothing dipped low enough, so the least unlike lag is the only
        // candidate left, and only when it is still a fair match.
        float lowest = 1.0f;
        for (int lag = minLag; lag <= maxLag; lag++) {
            if (a.normalized[static_cast<size_t>(lag)] < lowest) {
                lowest = a.normalized[static_cast<size_t>(lag)];
                best = lag;
            }
        }
        if (best < 0 || lowest > 0.42f) {
            return 0.0f;
        }
    }

    // A whole lag only ever lands between samples; the parabola finds the rest.
    float shift = 0.0f;
    if (best >= 2 && best + 1 <= maxLag) {
        const float before = a.normalized[static_cast<size_t>(best - 1)];
        const float here = a.normalized[static_cast<size_t>(best)];
        const float after = a.normalized[static_cast<size_t>(best + 1)];
        const float curvature = before - 2.0f * here + after;
        if (fabsf(curvature) > 1e-7f) {
            shift = std::clamp(0.5f * (before - after) / curvature, -0.5f, 0.5f);
        }
    }
    confidence = Clamp01(1.0f - a.normalized[static_cast<size_t>(best)]);
    return static_cast<float>(rate) / (static_cast<float>(best) + shift);
}

// Folds a block into the decimated history. The four sample average doubles as
// the anti alias filter, which is enough once the upper harmonics of a string
// are an octave of level down.
void FeedTunerBlock(TunerAudio& a, const short* samples, int count) {
    for (int i = 0; i < count; i++) {
        a.box += static_cast<float>(samples[i]) * (1.0f / 32768.0f);
        if (++a.boxCount < kTunerDecimate) {
            continue;
        }
        const float value = a.box * (1.0f / static_cast<float>(kTunerDecimate));
        a.box = 0.0f;
        a.boxCount = 0;
        a.history[static_cast<size_t>(a.position)] = value;
        a.position = (a.position + 1) % kTunerWindow;
        a.sinceAnalysis++;
    }
}

// A fresh reading from whatever the history holds right now.
void AnalyseTunerWindow(TunerAudio& a) {
    a.sinceAnalysis = 0;
    for (int i = 0; i < kTunerWindow; i++) {
        a.frame[static_cast<size_t>(i)] =
            a.history[static_cast<size_t>((a.position + i) % kTunerWindow)];
    }
    // A microphone carries an offset, which is not a note; take it out before
    // measuring, or a quiet room would read as steady as a plucked string.
    double total = 0.0;
    for (int i = 0; i < kTunerWindow; i++) {
        total += a.frame[static_cast<size_t>(i)];
    }
    const float mean = static_cast<float>(total / kTunerWindow);
    float energy = 0.0f;
    for (int i = 0; i < kTunerWindow; i++) {
        const float value = a.frame[static_cast<size_t>(i)] - mean;
        a.frame[static_cast<size_t>(i)] = value;
        energy += value * value;
    }
    const float level = sqrtf(energy / static_cast<float>(kTunerWindow));
    float confidence = 0.0f;
    const float frequency =
        level < kTunerSilence
            ? 0.0f
            : DetectPitch(a, a.rate / kTunerDecimate, confidence);
    g_tunerShared.level.store(level);
    g_tunerShared.confidence.store(confidence);
    g_tunerShared.frequency.store(
        confidence >= kTunerClean ? frequency : 0.0f);
    g_tunerShared.reading.fetch_add(1);
}

void CloseTunerAudio(TunerAudio& a) {
    if (a.device) {
        waveInReset(a.device);
        if (a.prepared) {
            for (int i = 0; i < kTunerBufferCount; i++) {
                waveInUnprepareHeader(a.device, &a.headers[i],
                                      sizeof(WAVEHDR));
            }
        }
        waveInClose(a.device);
        a.device = nullptr;
    }
    a.prepared = false;
}

void StopTunerAudio();

DWORD WINAPI TunerThreadProc(void*) {
    TunerAudio& a = g_tunerAudio;
    a.audioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    a.stopEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    bool opened = false;
    if (a.audioEvent && a.stopEvent) {
        // The mapper takes whatever the default microphone offers, so try the
        // rates a device is likely to expose rather than insisting on one.
        const int rates[] = {44100, 48000, 32000, 22050};
        for (int rate : rates) {
            WAVEFORMATEX format{};
            format.wFormatTag = WAVE_FORMAT_PCM;
            format.nChannels = 1;
            format.nSamplesPerSec = static_cast<DWORD>(rate);
            format.wBitsPerSample = 16;
            format.nBlockAlign = 2;
            format.nAvgBytesPerSec = static_cast<DWORD>(rate) * 2;
            if (waveInOpen(&a.device, WAVE_MAPPER, &format,
                           reinterpret_cast<DWORD_PTR>(a.audioEvent), 0,
                           CALLBACK_EVENT) == MMSYSERR_NOERROR) {
                a.rate = rate;
                opened = true;
                break;
            }
            a.device = nullptr;
        }
        if (!opened) {
            Wh_Log(L"Tuner: no microphone for this process");
        }
    }

    if (opened) {
        a.history.assign(static_cast<size_t>(kTunerWindow), 0.0f);
        a.frame.assign(static_cast<size_t>(kTunerWindow), 0.0f);
        bool queued = true;
        for (int i = 0; i < kTunerBufferCount; i++) {
            a.pcm[i].assign(static_cast<size_t>(kTunerBufferFrames), 0);
            a.headers[i] = WAVEHDR{};
            a.headers[i].lpData = reinterpret_cast<LPSTR>(a.pcm[i].data());
            a.headers[i].dwBufferLength =
                static_cast<DWORD>(a.pcm[i].size() * sizeof(short));
            if (waveInPrepareHeader(a.device, &a.headers[i], sizeof(WAVEHDR)) !=
                    MMSYSERR_NOERROR ||
                waveInAddBuffer(a.device, &a.headers[i], sizeof(WAVEHDR)) !=
                    MMSYSERR_NOERROR) {
                queued = false;
                break;
            }
        }
        a.prepared = true;
        if (queued && waveInStart(a.device) == MMSYSERR_NOERROR) {
            g_tunerShared.running.store(true);
        }
        if (a.readyEvent) {
            // Only now can a caller tell a live stream from a dead one.
            SetEvent(a.readyEvent);
        }

        if (g_tunerShared.running.load()) {
            const HANDLE waits[2] = {a.audioEvent, a.stopEvent};
            for (;;) {
                const DWORD result = WaitForMultipleObjects(2, waits, FALSE, 200);
                if (result == WAIT_OBJECT_0 + 1) {
                    break;
                }
                if (result != WAIT_OBJECT_0) {
                    continue;  // A timeout is just a quiet moment.
                }
                bool failed = false;
                for (int i = 0; i < kTunerBufferCount; i++) {
                    if (!(a.headers[i].dwFlags & WHDR_DONE)) {
                        continue;
                    }
                    if (a.headers[i].dwBytesRecorded > 0) {
                        FeedTunerBlock(
                            a,
                            reinterpret_cast<const short*>(a.headers[i].lpData),
                            static_cast<int>(a.headers[i].dwBytesRecorded / 2));
                    }
                    a.headers[i].dwFlags &= ~WHDR_DONE;
                    if (waveInAddBuffer(a.device, &a.headers[i],
                                        sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                        failed = true;
                        break;
                    }
                    if (a.sinceAnalysis >= kTunerHop) {
                        AnalyseTunerWindow(a);
                    }
                }
                if (failed) {
                    Wh_Log(L"Tuner: the microphone went away");
                    break;
                }
            }
        }
    }

    g_tunerShared.running.store(false);
    g_tunerShared.frequency.store(0.0f);
    g_tunerShared.confidence.store(0.0f);
    CloseTunerAudio(a);
    if (a.readyEvent) {
        SetEvent(a.readyEvent);  // Releases a start that is still waiting.
    }
    if (a.stopEvent) {
        SetEvent(a.stopEvent);  // A late wake must not outlive the loop.
    }
    return 0;
}

bool StartTunerAudio() {
    TunerAudio& a = g_tunerAudio;
    if (a.thread && WaitForSingleObject(a.thread, 0) == WAIT_OBJECT_0) {
        // The microphone was lost while the panel was away, so clear the
        // remains of that thread before opening a new one.
        StopTunerAudio();
    }
    if (a.thread) {
        return true;
    }
    a.position = 0;
    a.sinceAnalysis = 0;
    a.box = 0.0f;
    a.boxCount = 0;
    a.rate = kTunerRate;
    a.readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!a.readyEvent) {
        return false;
    }
    a.thread = CreateThread(nullptr, 0, TunerThreadProc, nullptr, 0, nullptr);
    if (!a.thread) {
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
        return false;
    }
    WaitForSingleObject(a.readyEvent, 2000);
    if (!g_tunerShared.running.load()) {
        WaitForSingleObject(a.thread, 1000);
        CloseHandle(a.thread);
        a.thread = nullptr;
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
        if (a.audioEvent) {
            CloseHandle(a.audioEvent);
            a.audioEvent = nullptr;
        }
        if (a.stopEvent) {
            CloseHandle(a.stopEvent);
            a.stopEvent = nullptr;
        }
        return false;
    }
    return true;
}

void StopTunerAudio() {
    TunerAudio& a = g_tunerAudio;
    if (a.stopEvent) {
        SetEvent(a.stopEvent);
    }
    if (a.thread) {
        WaitForSingleObject(a.thread, 2000);
        CloseHandle(a.thread);
        a.thread = nullptr;
    }
    if (a.readyEvent) {
        CloseHandle(a.readyEvent);
        a.readyEvent = nullptr;
    }
    if (a.audioEvent) {
        CloseHandle(a.audioEvent);
        a.audioEvent = nullptr;
    }
    if (a.stopEvent) {
        CloseHandle(a.stopEvent);
        a.stopEvent = nullptr;
    }
    g_tunerShared.running.store(false);
    // The thread has been joined, so its buffers can go too.
    a = TunerAudio();
}

////////////////////////////////////////////////////////////////////////////////
// Tuner

// A different instrument or tuning means the old string marks no longer
// apply, so they all clear together.
void TunerResetProgress() {
    for (int i = 0; i < kTunerStringMax; i++) {
        g_tuner.tuned[i] = false;
        g_tuner.stringAt[i] = 0.0;
    }
    g_tuner.lock = -1;
    g_tuner.lockAt = 0.0;
    g_tuner.needle = 0.0f;
    g_tuner.pulsing = false;
    g_tuner.shownNote = -1;
    g_tuner.fadingNote = -1;
    g_tuner.pendingNote = -1;
    g_tuner.noteAt = 0.0;
}

void TunerSetInstrument(int instrument) {
    const int tuning = TunerInstrumentFirst(instrument);
    if (tuning < 0 || instrument == g_tuner.instrument) {
        return;
    }
    g_tuner.instrument = instrument;
    g_tuner.tuning = tuning;
    TunerResetProgress();
    Repaint();
}

void TunerSetTuning(int tuning) {
    if (tuning < 0 || tuning >= kTuningCount ||
        kTunings[tuning].instrument != g_tuner.instrument ||
        tuning == g_tuner.tuning) {
        return;
    }
    g_tuner.tuning = tuning;
    TunerResetProgress();
    Repaint();
}

// Clicking a string chip tunes just that string; clicking it again hands the
// choice back to the reading. Number keys pick a string the same way, and 0
// stands for every string.
constexpr int kTunerStringAll = kTunerStringMax;  // The 0 key, meaning no lock.

void TunerSetString(int string) {
    const int count = TunerStringCount(g_tuner.tuning);
    if (string == kTunerStringAll) {
        string = -1;
    }
    if (string != -1 && (string < 0 || string >= count)) {
        return;
    }
    g_tuner.lock = string;
    g_tuner.lockAt = NowMs();
    // A string picked out again has to earn its mark from scratch.
    if (string >= 0) {
        g_tuner.tuned[string] = false;
        g_tuner.stringAt[string] = 0.0;
    }
    Repaint();
}

// The pointer toggle: the chip that is already held lets go.
void TunerToggleString(int string) {
    TunerSetString(g_tuner.lock == string ? -1 : string);
}

// Walks the instruments, landing on the first tuning each one offers.
void TunerStepInstrument(int delta) {
    const int count = kTunerInstrumentCount;
    int next = g_tuner.instrument;
    for (int step = 0; step < count; step++) {
        next = (next + (delta > 0 ? 1 : count - 1)) % count;
        if (TunerInstrumentFirst(next) >= 0) {
            break;
        }
    }
    TunerSetInstrument(next);
}

// Walks the tunings of the instrument that is showing.
void TunerStepTuning(int delta) {
    const int count = TunerTuningCount(g_tuner.instrument);
    if (count <= 0) {
        return;
    }
    const int first = TunerInstrumentFirst(g_tuner.instrument);
    const int offset = std::clamp(g_tuner.tuning, first, first + count - 1) -
                       first;
    TunerSetTuning(TunerTuningAt(g_tuner.instrument,
                                 (offset + delta + count) % count));
}

void TunerStart() {
    if (g_tuner.listening) {
        return;
    }
    g_tunerShared.frequency.store(0.0f);
    g_tunerShared.confidence.store(0.0f);
    g_tunerShared.level.store(0.0f);
    g_tuner.frequency = 0.0f;
    g_tuner.level = 0.0f;
    g_tuner.heardAt = 0.0;
    g_tuner.reading = g_tunerShared.reading.load();
    g_tuner.listening = StartTunerAudio();
    g_tuner.micFailed = !g_tuner.listening;
    if (!g_tuner.listening) {
        Wh_Log(L"Tuner: couldn't open the microphone");
    } else if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kTunerTimer, 50, nullptr);
    }
    Repaint();
}

// Releases the microphone, for the close, the tool switch and the unload paths.
void TunerHalt() {
    if (g_tuner.listening) {
        g_tuner.listening = false;
        StopTunerAudio();
    }
    g_tuner.micFailed = false;
    g_tuner.frequency = 0.0f;
    g_tuner.level = 0.0f;
    TunerResetProgress();
    MenuReset(g_tunerMenu);
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kTunerTimer);
    }
}

// Pulls the latest reading into the panel's own view of it and redraws.
void TunerPoll() {
    const unsigned reading = g_tunerShared.reading.load();
    if (reading == g_tuner.reading) {
        return;
    }
    g_tuner.reading = reading;
    const float raw = g_tunerShared.frequency.load();
    g_tuner.level = g_tunerShared.level.load();
    const double now = NowMs();
    if (raw > 0.0f) {
        // Smooth the needle inside a note, but jump the moment the player
        // moves to a different string.
        const float ratio =
            g_tuner.frequency > 0.0f ? raw / g_tuner.frequency : 0.0f;
        g_tuner.frequency = ratio > 0.958f && ratio < 1.044f
                                ? g_tuner.frequency +
                                      (raw - g_tuner.frequency) * 0.45f
                                : raw;
        g_tuner.heardAt = now;
    } else if (now - g_tuner.heardAt > 400.0) {
        // The string has died away, so let the note go rather than freezing.
        g_tuner.frequency = 0.0f;
    }
    // A string that settles within three cents earns its mark, which stays
    // until the tuning changes, so the player can see what is left to do.
    int cents = 0;
    const int midi = TunerDisplayReading(cents);
    if (midi >= 0 && cents >= -3 && cents <= 3) {
        const int string =
            g_tuner.lock >= 0 ? g_tuner.lock : TunerNearestString(midi);
        if (string >= 0 && !g_tuner.tuned[string]) {
            g_tuner.tuned[string] = true;
            g_tuner.stringAt[string] = now;
        }
    }
    // The big note only changes once the reading has repeated itself, so a
    // single doubtful frame cannot set the display rolling.
    if (midi != g_tuner.shownNote) {
        if (midi == g_tuner.pendingNote) {
            g_tuner.fadingNote = g_tuner.shownNote;
            g_tuner.shownNote = midi;
            g_tuner.pendingNote = midi;
            g_tuner.noteAt = now;
        } else {
            g_tuner.pendingNote = midi;
        }
    } else {
        g_tuner.pendingNote = midi;
    }
    Repaint();
}

// Eases the needle toward the reading and plays out the in-tune pulse, the
// fresh check marks and the roll of the note, repainting only while
// something is still moving.
void TunerAnimate() {
    int cents = 0;
    const int midi = TunerDisplayReading(cents);
    const float target =
        midi >= 0 ? static_cast<float>(std::clamp(cents, -50, 50)) : 0.0f;
    g_tuner.needle += (target - g_tuner.needle) * 0.35f;
    bool moving = fabsf(target - g_tuner.needle) > 0.05f;

    const bool inTune = midi >= 0 && cents >= -3 && cents <= 3;
    const double now = NowMs();
    if (inTune && !g_tuner.pulsing) {
        g_tuner.pulsing = true;
        g_tuner.pulseAt = now;
    }
    if (g_tuner.pulsing) {
        if (now - g_tuner.pulseAt < 450.0) {
            moving = true;
        } else {
            g_tuner.pulsing = false;
        }
    }
    for (int i = 0; i < kTunerStringMax; i++) {
        if (g_tuner.tuned[i] && g_tuner.stringAt[i] > 0.0 &&
            now - g_tuner.stringAt[i] < kTunedBadgeMs) {
            moving = true;  // A badge still arriving.
        }
    }
    if (g_tuner.lock >= 0 && now - g_tuner.lockAt < kTunedBadgeMs) {
        moving = true;  // A ring still closing on the chip.
    }
    if (now - g_tuner.noteAt < kNoteSwapMs) {
        moving = true;  // The note display still rolling to the next one.
    }
    if (moving) {
        Repaint();
    }
}


////////////////////////////////////////////////////////////////////////////////
// Painting

inline float L(long value) {
    return static_cast<float>(value);
}

// One Fluent glyph centred on a point. The shell's own symbol font draws the
// icon, so it stays crisp at every scale instead of being rebuilt from strokes.
// The box is twice the em square because DrawText clips to it and sits the
// glyphs a little off centre on their own line.
void PaintGlyph(Surface& s,
                float cx,
                float cy,
                wchar_t glyph,
                int px,
                Color color) {
    const LONG x = static_cast<LONG>(cx + 0.5f);
    const LONG y = static_cast<LONG>(cy + 0.5f);
    const RECT box{x - px, y - px, x + px, y + px};
    g_text.Draw(s, g_fonts.GetIcon(px), std::wstring(1, glyph), box,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, color);
}

// The stepper chevrons either side of the chord diagram.
void PaintChevron(Surface& s, float cx, float cy, bool right, Color color) {
    PaintGlyph(s, cx, cy, right ? L'\xE76C' : L'\xE76B', S(18.0f), color);
}

// The down chevron on the tool picker, saying the label opens a list.
void PaintDownChevron(Surface& s, float cx, float cy, Color color) {
    PaintGlyph(s, cx, cy, L'\xE70D', S(10.0f), color);
}

void PaintCloseButton(Surface& s, const Theme& t) {
    const RECT& box = g_layout.close;
    const bool hovered = g_hover.id == kHitClose;
    const bool pressed = hovered && g_pressed.id == kHitClose;
    const float cx = L(box.left + box.right) * 0.5f;
    const float cy = L(box.top + box.bottom) * 0.5f;
    if (hovered) {
        FillCircle(s, cx, cy, S(10.0f),
                   pressed ? WithAlpha(t.text, 0.22f) : t.controlHover);
    }
    // The Fluent close cross, the same one the shell puts on its title bars.
    PaintGlyph(s, cx, cy, L'\xE8BB', S(10.0f), hovered ? t.text : t.textMuted);
}

void PaintPlayButton(Surface& s, const Theme& t) {
    const RECT& box = g_layout.speaker;
    const Voicing* voicing = SelectedVoicing();
    const bool available = voicing && voicing->noteCount > 0;
    const bool hovered = available && g_hover.id == kHitSpeaker;
    const bool pressed = hovered && g_pressed.id == kHitSpeaker;
    const float cx = L(box.left + box.right) * 0.5f;
    const float cy = L(box.top + box.bottom) * 0.5f;

    if (hovered) {
        FillCircle(s, cx, cy, S(11.0f),
                   pressed ? WithAlpha(t.text, 0.22f) : t.controlHover);
    }
    const Color color = available ? (hovered ? t.text : t.textMuted)
                                  : WithAlpha(t.textMuted, 0.35f);

    PaintGlyph(s, cx, cy, L'\xE768', S(15.0f), color);
}

// One pill in the header: accent when it is the current choice, a soft hover
// wash otherwise. A pill the bar cannot be drawn on - the sixteenths of a
// twelve-eight bar, which would need more slots than the engine holds - is
// faded and refuses the pointer instead of writing half a measure.
void PaintChip(Surface& s,
               const Theme& t,
               const RECT& chip,
               PCWSTR label,
               HFONT font,
               bool active,
               bool hovered,
               bool enabled = true) {
    const Color fill = active ? WithAlpha(t.accent, 0.95f)
                        : (hovered && enabled) ? t.controlHover
                                               : t.control;
    FillRoundRect(s, L(chip.left), L(chip.top), L(chip.right), L(chip.bottom),
                  S(9.0f), fill);
    RECT text = chip;
    const Color ink = active ? t.onAccent
                      : enabled ? t.textMuted
                                : WithAlpha(t.textMuted, 0.45f);
    g_text.Draw(s, font, label, text, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                ink);
}

void PaintTabs(Surface& s, const Theme& t) {
    HFONT font = g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    const int instruments =
        std::min(static_cast<int>(kInstrumentCount), chorddata::kInstrumentsSize);
    for (int i = 0; i < instruments; i++) {
        PaintChip(s, t, g_layout.tabs[i], chorddata::kInstruments[i].name, font,
                  g_app.instrument == i,
                  g_hover.id == kHitTab && g_hover.index == i);
    }
}

// The tool picker: one button naming the open tool, plus the list it opens.
// The list is drawn last, over the panel, so it reads as a popup.
void PaintToolSelect(Surface& s, const Theme& t) {
    const RECT& select = g_layout.toolSelect;
    const bool open = MenuHit(g_toolMenu);
    const bool hovered = g_hover.id == kHitToolSelect;
    FillRoundRect(s, L(select.left), L(select.top), L(select.right),
                  L(select.bottom), S(8.0f),
                  open || hovered ? t.controlHover : t.control);
    StrokeRoundRect(s, L(select.left) + 0.5f, L(select.top) + 0.5f,
                    L(select.right) - 0.5f, L(select.bottom) - 0.5f, S(8.0f),
                    S(1.0f), open ? WithAlpha(t.accent, 0.55f) : t.stroke);

    HFONT font = g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    RECT label = select;
    label.left += S(10);
    label.right -= S(24);
    g_text.Draw(s, font, ToolLabel(g_app.tool), label,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE, t.text);

    const float cy = L(select.top + select.bottom) * 0.5f;
    PaintDownChevron(s, L(select.right) - S(12), cy,
                     open ? t.accent : t.textMuted);
}

// The body of an animated popup list. Progress runs from 0 to 1: the plate
// fades in and unrolls from its anchor, so rows clipped against it arrive with
// the front edge. A list with room below it unrolls downward; one that opens
// upward from a control near the bottom keeps its bottom edge and grows up. The
// result is what content may draw inside, and MenuAlpha is what the rows
// multiply their colours by, so the text comes in with the plate rather than
// sitting on bare glass.
float MenuAlpha(float progress) {
    const float eased = Clamp01(progress);
    return eased < 0.7f ? eased / 0.7f : 1.0f;
}

RECT PaintListShell(Surface& s,
                    const Theme& t,
                    const RECT& full,
                    float progress,
                    bool upward = false) {
    const float alpha = MenuAlpha(progress);
    const float eased = Clamp01(progress);
    const float top = L(full.top);
    const float bottom = L(full.bottom);
    // The front is the edge that travels; the anchor edge stays put.
    const float front = upward ? bottom - (bottom - top) * eased
                               : top + (bottom - top) * eased;
    const float plateTop = upward ? front : top;
    const float plateBottom = upward ? bottom : front;
    const float left = L(full.left);
    const float right = L(full.right);
    const float radius = S(9.0f);
    DrawShadow(s, left, plateBottom - S(1.0f), right, plateBottom + S(3.0f),
               radius, S(5.0f),
               WithAlpha(kBlack, (t.dark ? 0.45f : 0.18f) * alpha));
    FillRoundRect(s, left, plateTop, right, plateBottom, radius,
                  WithAlpha(t.menu, alpha));
    // The edge keeps its own weight and only fades with the plate, so its alpha
    // is scaled rather than replaced.
    Color edge = t.stroke;
    edge.a *= alpha;
    StrokeRoundRect(s, left + 0.5f, plateTop + 0.5f, right - 0.5f,
                    plateBottom - 0.5f, radius, S(1.0f), edge);
    return {full.left, static_cast<long>(front), full.right, full.bottom};
}

// One row of a popup list. Both lists share this so a row entering, a row
// under the pointer and the current choice all look and move the same way.
struct ListItem {
    PCWSTR label = L"";
    PCWSTR detail = L"";  // Right-aligned second column, muted.
    bool active = false;
    bool hovered = false;
};

void PaintListRows(Surface& s,
                   const Theme& t,
                   const ListItem* items,
                   int count,
                   const RECT* rows,
                   const RECT& reveal,
                   bool upward = false) {
    HFONT font = g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    const float front = L(reveal.top);
    const float travel = std::max(1.0f, static_cast<float>(S(20.0f)));
    for (int i = 0; i < count; i++) {
        const RECT& row = rows[i];
        // A row is drawn as the front edge of the unroll reaches it, easing its
        // last few pixels into place as it goes. A list that grows upward hands
        // its rows down from the top, so its front is measured from below.
        const float appear =
            upward ? Clamp01((L(row.bottom) - front) / travel)
                   : Clamp01((front - L(row.top)) / travel);
        if (appear <= 0.0f) {
            continue;
        }
        const float alpha = MenuAlpha(appear);
        const float dy = (1.0f - appear) * (upward ? -S(6.0f) : S(6.0f));
        const float top = L(row.top) + dy;
        const float bottom = L(row.bottom) + dy;
        const bool active = items[i].active;
        const bool hovered = items[i].hovered;
        // The unroll fades the wash and the ink, so their alphas are scaled.
        // WithAlpha sets the alpha outright, and replacing a 16% accent or an
        // 11% hover wash with 1.0 painted a solid block over its own label.
        if (active || hovered) {
            Color wash = active ? t.selection : t.controlHover;
            wash.a *= alpha;
            FillRoundRect(s, L(row.left), top, L(row.right), bottom, S(6.0f),
                          wash);
        }
        RECT label = {row.left + S(12), static_cast<long>(top),
                      row.right - S(26), static_cast<long>(bottom)};
        Color ink = t.text;
        ink.a *= alpha;
        if (items[i].detail && *items[i].detail) {
            // A row can carry a second, quieter column on the right; the label
            // keeps the rest of the width. The smaller font keeps two lines of
            // information inside one row without the row growing.
            HFONT detailFont =
                g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
            const int width = g_text.Measure(detailFont, items[i].detail).cx;
            RECT detail = label;
            detail.left = detail.right - width;
            Color faint = t.textMuted;
            faint.a *= alpha;
            g_text.Draw(s, detailFont, items[i].detail, detail,
                        DT_RIGHT | DT_VCENTER | DT_SINGLELINE, faint);
            label.right = detail.left - S(12);
        }
        g_text.Draw(s, font, items[i].label, label,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS, ink);
        if (active) {
            // A tick on the right, so the current choice still reads plainly.
            const float cx = L(row.right) - S(14);
            const float cy = (top + bottom) * 0.5f;
            Color mark = t.accent;
            mark.a *= alpha;
            PaintGlyph(s, cx, cy, L'\xE73E', S(11.0f), mark);
        }
    }
}

void PaintToolMenu(Surface& s, const Theme& t) {
    // The returned top edge is the front of the unroll.
    const RECT reveal =
        PaintListShell(s, t, g_layout.toolMenu, g_toolMenu.progress);

    ListItem items[kToolCount];
    for (int i = 0; i < kToolCount; i++) {
        items[i].label = ToolLabel(i);
        items[i].active = g_app.tool == i;
        items[i].hovered =
            (g_hover.id == kHitToolMenuItem && g_hover.index == i) ||
            g_toolMenu.cursor == i;  // The keyboard cursor rides with the list.
    }
    PaintListRows(s, t, items, kToolCount, g_layout.toolMenuRows, reveal);
}

// The x coordinate of a caret position inside the search box, in pixels.
int QueryEdge(int index) {
    const int left = static_cast<int>(g_layout.searchText.left);
    index = std::clamp(index, 0, static_cast<int>(g_app.query.size()));
    if (index == 0) {
        return left;
    }
    HFONT font = g_fonts.Get(S(static_cast<float>(kFontSearch)), FW_NORMAL);
    const std::wstring prefix = g_app.query.substr(0, static_cast<size_t>(index));
    return left + g_text.Measure(font, prefix).cx;
}

// The caret position nearest to a click.
int QueryIndexFromX(int x) {
    const int count = static_cast<int>(g_app.query.size());
    int best = 0;
    int bestDistance = std::abs(x - QueryEdge(0));
    for (int i = 1; i <= count; i++) {
        const int distance = std::abs(x - QueryEdge(i));
        if (distance < bestDistance) {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

void PaintSearch(Surface& s, const Theme& t) {
    const RECT& box = g_layout.search;
    const bool focused = g_searchFocused;
    FillRoundRect(s, L(box.left), L(box.top), L(box.right), L(box.bottom),
                  S(kControlRadius), focused ? t.controlHover : t.control);
    StrokeRoundRect(s, L(box.left) + 0.5f, L(box.top) + 0.5f,
                    L(box.right) - 0.5f, L(box.bottom) - 0.5f, S(kControlRadius),
                    S(1.0f), focused ? WithAlpha(t.accent, 0.55f) : t.stroke);

    // The Fluent magnifier, on the same baseline as the placeholder text.
    const float cx = L(box.left) + S(21.0f);
    const float cy = (L(box.top) + L(box.bottom)) * 0.5f;
    PaintGlyph(s, cx, cy, L'\xE721', S(15.0f), t.textMuted);

    HFONT font = g_fonts.Get(S(static_cast<float>(kFontSearch)), FW_NORMAL);
    RECT text = g_layout.searchText;
    if (g_app.query.empty()) {
        g_text.Draw(s, font, L"Search chords", text, DT_LEFT | DT_VCENTER |
                                                      DT_SINGLELINE,
                    t.textFaint);
    } else {
        if (HasSelection()) {
            const float selLeft = L(QueryEdge(SelectionStart()));
            const float selRight = std::min(L(QueryEdge(SelectionEnd())),
                                            L(g_layout.searchText.right));
            FillRoundRect(s, selLeft - S(1.0f), cy - S(9.0f),
                          selRight + S(1.0f), cy + S(9.0f), S(2.0f),
                          t.selection);
        }
        g_text.Draw(s, font, g_app.query, text,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE, t.text);
    }

    if (focused && g_windowActive && !HasSelection()) {
        const double phase = fmod(NowMs() - g_caretStart, 1060.0);
        if (phase < 530.0) {
            const float x = std::min(L(QueryEdge(g_caret)),
                                     L(g_layout.searchText.right) - S(2.0f));
            const float height = S(18.0f);
            FillRoundRect(s, x, cy - height * 0.5f, x + S(1.6f),
                          cy + height * 0.5f, S(0.8f), t.accent);
        }
    }
}

void PaintResults(Surface& s, const Theme& t) {
    const RECT& list = g_layout.list;
    const int rowHeight = g_layout.rowHeight;
    const int count = static_cast<int>(g_app.results.size());
    if (count == 0) {
        const RECT empty{list.left, list.top, list.right, list.top + rowHeight};
        g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontRow)), FW_NORMAL),
                    L"No chord matches that", empty,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE, t.textFaint);
        return;
    }

    HFONT font = g_fonts.Get(S(static_cast<float>(kFontRow)), FW_NORMAL);
    HFONT countFont = g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
    const Library& library = g_libraries[g_app.instrument];
    const RECT clip = list;

    for (int row = 0; row < kVisibleRows; row++) {
        const int index = g_app.scroll + row;
        if (index >= count) {
            break;
        }
        RECT rect{list.left, list.top + row * rowHeight, list.right,
                  list.top + (row + 1) * rowHeight - S(2.0f)};
        const bool selected = index == g_app.selected;
        const bool hovered = g_hover.id == kHitRow && g_hover.index == index;
        if (selected) {
            FillRoundRect(s, L(rect.left), L(rect.top), L(rect.right),
                          L(rect.bottom), S(kControlRadius), t.selection);
        } else if (hovered) {
            FillRoundRect(s, L(rect.left), L(rect.top), L(rect.right),
                          L(rect.bottom), S(kControlRadius), t.controlHover);
        }

        const ChordEntry& entry = library.chords[g_app.results[index]];
        const int voicings = VoicingCount(g_app.instrument, entry);

        RECT text = rect;
        text.left += S(12.0f);
        text.right -= S(40.0f);
        g_text.Draw(s, font, entry.name, text, clip,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    selected ? t.text : t.textMuted);

        if (voicings > 1) {
            RECT badge = rect;
            badge.left = rect.right - S(38.0f);
            badge.right -= S(12.0f);
            g_text.Draw(s, countFont, std::to_wstring(voicings), badge, clip,
                        DT_RIGHT | DT_VCENTER | DT_SINGLELINE, t.textFaint);
        }
    }

    if (count > kVisibleRows) {
        const int trackHeight = list.bottom - list.top;
        const float thumbHeight =
            std::max(static_cast<float>(S(24.0f)),
                     static_cast<float>(trackHeight) *
                         static_cast<float>(kVisibleRows) / count);
        const float span = static_cast<float>(trackHeight) - thumbHeight;
        const float t0 = static_cast<float>(g_app.scroll) /
                         static_cast<float>(count - kVisibleRows);
        const float y = L(list.top) + span * Clamp01(t0);
        const float x = L(list.right) - S(3.0f);
        FillRoundRect(s, x, L(list.top), x + S(2.5f), L(list.bottom),
                      S(1.2f), WithAlpha(t.textFaint, 0.20f));
        FillRoundRect(s, x, y, x + S(2.5f), y + thumbHeight, S(1.2f),
                      WithAlpha(t.textMuted, 0.75f));
    }
}

void PaintFrettedChord(Surface& s, const Theme& t, const Voicing& v, RECT area) {
    const int strings = v.stringCount;
    if (strings < 2) {
        return;
    }
    const float width = L(area.right - area.left);
    const float height = L(area.bottom - area.top);
    const float labelRoom = S(16.0f);
    const float fretRoom = S(4.0f);
    constexpr int kFrets = 5;
    const float usable = height - labelRoom - fretRoom;
    const float cell = std::min(usable / kFrets, width / (strings + 0.6f));
    if (cell <= 2.0f) {
        return;
    }
    // Four string necks can afford a little extra string spacing to fill the
    // card; a six string neck reads better as a tall, narrow window, with the
    // frets further apart than the strings.
    const float widen = strings >= 6 ? 0.8f : 1.25f;
    const float cellX = std::min(width / (strings + 0.6f), cell * widen);
    const float gridWidth = cellX * (strings - 1);
    const float gridTop = L(area.top) + labelRoom + (usable - cell * kFrets) * 0.5f;
    const float gridBottom = gridTop + cell * kFrets;
    const float x0 = L(area.left) + (width - gridWidth) * 0.5f;
    // The neck is a board, not a set of hairlines: a soft shadow, a vertical
    // sheen and a thin rim give the wires and strings something to sit on.
    const float plateL = x0 - cellX * 0.5f;
    const float plateR = x0 + gridWidth + cellX * 0.5f;
    const float plateT = gridTop - S(2.0f);
    const float plateB = gridBottom + S(2.0f);
    const float plateRadius = S(4.0f);
    DrawShadow(s, plateL, plateT, plateR, plateB, plateRadius, S(4.5f),
               WithAlpha(kBlack, t.dark ? 0.34f : 0.14f));
    FillRoundRectGradient(s, plateL, plateT, plateR, plateB, plateRadius,
                          t.dark ? Rgb(60, 62, 70) : Rgb(251, 249, 245),
                          t.dark ? Rgb(37, 38, 44) : Rgb(232, 228, 221));
    StrokeRoundRect(s, plateL, plateT, plateR, plateB, plateRadius, S(1.0f),
                    t.dark ? WithAlpha(kWhite, 0.10f) : Rgb(0, 0, 0, 0.09f));

    // Position markers, like the inlays of a real neck.
    const float inlayRadius = std::min(cell * 0.13f, static_cast<float>(S(3.0f)));
    if (inlayRadius >= 1.5f) {
        const Color inlay =
            t.dark ? WithAlpha(kWhite, 0.11f) : WithAlpha(kBlack, 0.07f);
        for (int fret = 1; fret <= kFrets; fret++) {
            const int absolute = v.baseFret + fret - 1;
            const int mark = absolute % 12;
            if (absolute <= 0 ||
                (mark != 0 && mark != 3 && mark != 5 && mark != 7 && mark != 9)) {
                continue;
            }
            const float y = gridTop + cell * (fret - 0.5f);
            FillCircle(s, x0 + gridWidth * 0.5f, y, inlayRadius, inlay);
        }
    }

    // Wires stop short of the rounded board edge; the nut stands in for the
    // first one whenever the window starts at fret one.
    const Color wireColor =
        t.dark ? WithAlpha(kWhite, 0.20f) : Rgb(102, 94, 84, 0.42f);
    const float wireInset = S(3.0f);
    for (int fret = (v.baseFret <= 1) ? 1 : 0; fret <= kFrets; fret++) {
        const float y = gridTop + cell * fret;
        StrokeLine(s, Vec2{plateL + wireInset, y}, Vec2{plateR - wireInset, y},
                   S(1.4f), wireColor);
    }

    if (v.baseFret <= 1) {
        // The nut is a bone band across the top of the board.
        FillRoundRectGradient(s, plateL + S(1.0f), plateT + S(1.0f),
                              plateR - S(1.0f), plateT + S(5.5f), S(1.5f),
                              t.dark ? Rgb(216, 218, 226) : Rgb(202, 196, 187),
                              t.dark ? Rgb(156, 159, 168) : Rgb(158, 151, 141));
    } else {
        HFONT font = g_fonts.Get(S(static_cast<float>(kFontFret)), FW_NORMAL);
        RECT label{area.left, static_cast<LONG>(gridTop),
                   static_cast<LONG>(plateL - S(3.0f)),
                   static_cast<LONG>(gridTop + cell)};
        g_text.Draw(s, font, std::to_wstring(v.baseFret) + L"fr", label,
                    DT_RIGHT | DT_VCENTER | DT_SINGLELINE, t.textMuted);
    }

    // Strings thicken towards the bass side, with a faint shadow on the light
    // board so they read as round wire instead of lines.
    const Color stringColor =
        t.dark ? WithAlpha(kWhite, 0.62f) : Rgb(78, 73, 67, 0.60f);
    const float stringTop = plateT + S(2.0f);
    const float stringBottom = plateB - S(2.0f);
    for (int i = 0; i < strings; i++) {
        const float stringWidth =
            std::max(1.0f, static_cast<float>(S(1.5f)) *
                               (1.0f - 0.45f * static_cast<float>(i) /
                                           static_cast<float>(strings - 1)));
        const float x = x0 + cellX * i;
        if (!t.dark) {
            StrokeLine(s, Vec2{x + S(1.0f), stringTop},
                       Vec2{x + S(1.0f), stringBottom}, stringWidth,
                       WithAlpha(kBlack, 0.10f));
        }
        StrokeLine(s, Vec2{x, stringTop}, Vec2{x, stringBottom}, stringWidth,
                   stringColor);
    }

    const float dotRadius = std::clamp(cell * 0.32f,
                                       static_cast<float>(S(5.0f)),
                                       static_cast<float>(S(12.0f)));
    const Color dotShadow = WithAlpha(kBlack, t.dark ? 0.40f : 0.16f);
    auto DotY = [&](int absoluteFret) {
        return gridTop + cell * (absoluteFret - v.baseFret) + cell * 0.5f;
    };
    auto DotX = [&](int string) { return x0 + cellX * string; };

    // A lit top and a shadowed bottom, so a finger reads as a pressed button
    // rather than a flat sticker.
    auto PaintDot = [&](float cx, float cy, float radius) {
        FillCircle(s, cx, cy + S(0.8f), radius, dotShadow);
        FillShape(
            s, cx - radius - 1, cy - radius - 1, cx + radius + 1, cy + radius + 1,
            [&](float x, float y) {
                return sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) - radius;
            },
            [&](float, float y) {
                return Mix(Mix(t.accent, kWhite, 0.16f),
                           Mix(t.accent, kBlack, 0.16f),
                           Clamp01((y - (cy - radius)) / (radius * 2.0f)));
            });
        StrokeCircle(s, cx, cy - S(0.4f), radius - S(0.5f), S(0.9f),
                     WithAlpha(kWhite, 0.14f));
    };

    // The open string circles belong above a plain nut; sitting over a barre
    // they read as a contradiction, and the data marks barres loosely enough
    // that such combos are common.
    bool hasBarre = false;
    for (int i = 0; i < v.barreCount; i++) {
        const Barre& barre = v.barres[i];
        if (barre.from >= 0 && barre.to < strings && barre.to > barre.from) {
            hasBarre = true;
            break;
        }
    }

    // A digit drawn inside a box its own size stays optically centred, and its
    // font follows the dot so it never crowds the rim.
    const int minFingerFont = S(7.0f);
    const int maxFingerFont = S(12.0f);
    auto PaintFinger = [&](float cx, float cy, float radius, int finger) {
        const int size = std::clamp(static_cast<int>(radius * 1.2f), minFingerFont,
                                    maxFingerFont);
        HFONT font = g_fonts.Get(size, kWeightSemiBold);
        RECT label{static_cast<LONG>(cx - radius), static_cast<LONG>(cy - radius),
                   static_cast<LONG>(cx + radius), static_cast<LONG>(cy + radius)};
        g_text.Draw(s, font, std::to_wstring(finger), label,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.onAccent);
    };

    // Barres first, so a bar reads as one finger rather than a row of dots.
    auto OnBarre = [&](int string, int absolute) {
        for (int i = 0; i < v.barreCount; i++) {
            const Barre& barre = v.barres[i];
            if (barre.from < 0 || barre.to >= strings || barre.to <= barre.from) {
                continue;
            }
            if (string >= barre.from && string <= barre.to &&
                v.baseFret + barre.fret - 1 == absolute) {
                return true;
            }
        }
        return false;
    };
    for (int i = 0; i < v.barreCount; i++) {
        const Barre& barre = v.barres[i];
        if (barre.from < 0 || barre.to >= strings || barre.to <= barre.from) {
            continue;
        }
        const float y = DotY(v.baseFret + barre.fret - 1);
        // The ends reach past the outer strings, so the rounded caps wrap them
        // whole instead of clipping them at the bar's edge.
        const float pad = dotRadius * 0.55f;
        const float left = std::max(plateL + S(1.0f), DotX(barre.from) - pad);
        const float right = std::min(plateR - S(1.0f), DotX(barre.to) + pad);
        FillRoundRect(s, left, y - dotRadius + S(0.9f), right,
                      y + dotRadius + S(0.9f), dotRadius, dotShadow);
        FillRoundRectGradient(s, left, y - dotRadius, right, y + dotRadius,
                              dotRadius, Mix(t.accent, kWhite, 0.14f),
                              Mix(t.accent, kBlack, 0.14f));
        // The barre is one finger, so its number sits in both cap ends.
        if (g_settings.showFingerings && v.fingers[barre.from] > 0 &&
            dotRadius >= S(5.0f)) {
            if (right - left >= dotRadius * 3.0f) {
                PaintFinger(left + dotRadius, y, dotRadius, v.fingers[barre.from]);
                PaintFinger(right - dotRadius, y, dotRadius,
                            v.fingers[barre.from]);
            } else {
                PaintFinger((left + right) * 0.5f, y, dotRadius,
                            v.fingers[barre.from]);
            }
        }
    }

    for (int i = 0; i < strings; i++) {
        const int fret = v.frets[i];
        if (fret == kMutedFret) {
            const float x = DotX(i);
            // Markers hug the board now that the neck has a top edge.
            const float y = gridTop - S(7.0f);
            const float arm = S(3.2f);
            StrokeLine(s, Vec2{x - arm, y - arm}, Vec2{x + arm, y + arm}, S(1.3f),
                       t.textFaint);
            StrokeLine(s, Vec2{x - arm, y + arm}, Vec2{x + arm, y - arm}, S(1.3f),
                       t.textFaint);
            continue;
        }
        if (fret == 0) {
            if (!hasBarre) {
                StrokeCircle(s, DotX(i), gridTop - S(7.0f), S(3.4f), S(1.3f),
                             t.textFaint);
            }
            continue;
        }
        const int absolute = AbsoluteFret(v, i);
        if (OnBarre(i, absolute)) {
            continue;
        }
        const float y = DotY(absolute);
        PaintDot(DotX(i), y, dotRadius);
        if (g_settings.showFingerings && v.fingers[i] > 0 &&
            dotRadius >= S(5.0f)) {
            PaintFinger(DotX(i), y, dotRadius, v.fingers[i]);
        }
    }
}

void PaintPianoChord(Surface& s, const Theme& t, const Voicing& v, RECT area) {
    if (v.noteCount <= 0) {
        return;
    }
    int low = 127;
    int high = 0;
    for (int i = 0; i < v.noteCount; i++) {
        low = std::min(low, v.notes[i]);
        high = std::max(high, v.notes[i]);
    }
    int start = ((low - 3) / 12) * 12;
    if (start < 0) {
        start = 0;
    }
    // Two octaves starting at the C below the chord, widened only if the chord
    // reaches past them.
    int end = start + 24;
    while (end <= high) {
        start += 12;
        end += 12;
    }

    auto isBlack = [](int note) {
        const int pitch = ((note % 12) + 12) % 12;
        return pitch == 1 || pitch == 3 || pitch == 6 || pitch == 8 ||
               pitch == 10;
    };
    auto held = [&](int note) {
        for (int i = 0; i < v.noteCount; i++) {
            if (v.notes[i] == note) {
                return true;
            }
        }
        return false;
    };

    int whiteCount = 0;
    for (int note = start; note < end; note++) {
        if (!isBlack(note)) {
            whiteCount++;
        }
    }
    if (whiteCount == 0) {
        return;
    }

    const float width = L(area.right - area.left);
    const float height = L(area.bottom - area.top);
    const float keyWidth = width / static_cast<float>(whiteCount);
    // Keys are capped by their own width, so they stay key shaped instead of
    // stretching into long slivers.
    const float whiteHeight = std::min(height, keyWidth * 4.4f);
    const float top = L(area.top) + (height - whiteHeight) * 0.5f;
    const float bottom = top + whiteHeight;
    const Color whiteKey = t.dark ? Rgb(238, 240, 246) : Rgb(255, 255, 255);
    const Color whiteEdge = t.dark ? Rgb(0, 0, 0, 0.35f) : Rgb(0, 0, 0, 0.18f);
    const Color blackKey = t.dark ? Rgb(26, 27, 32) : Rgb(38, 41, 48);

    // A slightly darker deck behind the keys gives them some depth.
    FillRoundRect(s, L(area.left) - S(2.0f), top - S(2.0f),
                  L(area.left) + width + S(2.0f), bottom + S(2.0f), S(5.0f),
                  WithAlpha(kBlack, t.dark ? 0.30f : 0.10f));

    int whiteIndex = 0;
    for (int note = start; note < end; note++) {
        if (isBlack(note)) {
            continue;
        }
        const float x = L(area.left) + keyWidth * whiteIndex;
        const bool pressed = held(note);
        FillRoundRect(s, x + 0.5f, top, x + keyWidth - 0.5f, bottom, S(2.5f),
                      pressed ? t.accent : whiteKey);
        StrokeRoundRect(s, x + 0.5f, top, x + keyWidth - 0.5f, bottom, S(2.5f),
                        S(1.0f), whiteEdge);
        whiteIndex++;
    }

    HFONT font = g_fonts.Get(S(static_cast<float>(kFontFret)), FW_NORMAL);
    whiteIndex = 0;
    for (int note = start; note < end; note++) {
        if (isBlack(note)) {
            continue;
        }
        if (note % 12 == 0) {
            const float keyLeft = L(area.left) + keyWidth * whiteIndex;
            RECT label{static_cast<LONG>(keyLeft),
                       static_cast<LONG>(bottom - S(16.0f)),
                       static_cast<LONG>(keyLeft + keyWidth),
                       static_cast<LONG>(bottom - S(2.0f))};
            const std::wstring name =
                std::wstring(PitchClassName(0)) + std::to_wstring(note / 12 - 1);
            g_text.Draw(s, font, name, label,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                        held(note) ? t.onAccent : t.textFaint);
        }
        whiteIndex++;
    }

    const float blackWidth = keyWidth * 0.60f;
    const float blackHeight = whiteHeight * 0.60f;
    whiteIndex = 0;
    for (int note = start; note < end; note++) {
        if (isBlack(note)) {
            const float x =
                L(area.left) + keyWidth * whiteIndex - blackWidth * 0.5f;
            const bool pressed = held(note);
            FillRoundRect(s, x, top, x + blackWidth, top + blackHeight, S(2.0f),
                          pressed ? t.accent : blackKey);
            continue;
        }
        whiteIndex++;
    }
}

void PaintVoicingSlider(Surface& s, const Theme& t) {
    const ChordEntry* entry = SelectedChord();
    if (!entry) {
        return;
    }
    const int count = VoicingCount(g_app.instrument, *entry);
    const bool hoveredPrevious =
        g_hover.id == kHitPrevious || g_hover.id == kHitDiagram;
    const bool hoveredNext = g_hover.id == kHitNext || g_hover.id == kHitDiagram;
    if (count > 1) {
        const RECT& backward = g_layout.previous;
        const RECT& forward = g_layout.next;
        PaintChevron(s, L(backward.left + backward.right) * 0.5f,
                     L(backward.top + backward.bottom) * 0.5f, false,
                     hoveredPrevious ? t.text : t.textMuted);
        PaintChevron(s, L(forward.left + forward.right) * 0.5f,
                     L(forward.top + forward.bottom) * 0.5f, true,
                     hoveredNext ? t.text : t.textMuted);
    }

    const int dots = std::min(count, 8);
    const float gap = S(14.0f);
    const float total = gap * static_cast<float>(dots - 1);
    const float cx = L(g_layout.card.left + g_layout.card.right) * 0.5f;
    const float cy = L(g_layout.card.bottom) - S(10.0f) - S(kVoicingStripHeight) * 0.5f;
    float x = cx - total * 0.5f;
    for (int i = 0; i < dots; i++) {
        const bool active = i == g_app.voicing;
        const bool hovered = g_hover.id == kHitDot && g_hover.index == i;
        if (active) {
            FillRoundRect(s, x - S(9.0f), cy - S(3.0f), x + S(9.0f),
                          cy + S(3.0f), S(3.0f), t.accent);
            g_layout.dots[i] = RECT{static_cast<LONG>(x - S(9.0f)),
                                    static_cast<LONG>(cy - S(7.0f)),
                                    static_cast<LONG>(x + S(9.0f)),
                                    static_cast<LONG>(cy + S(7.0f))};
        } else {
            FillCircle(s, x, cy, S(3.0f),
                       hovered ? t.textMuted : WithAlpha(t.textMuted, 0.45f));
            g_layout.dots[i] = RECT{static_cast<LONG>(x - S(6.0f)),
                                    static_cast<LONG>(cy - S(6.0f)),
                                    static_cast<LONG>(x + S(6.0f)),
                                    static_cast<LONG>(cy + S(6.0f))};
        }
        x += gap;
    }
    g_layout.dotCount = dots;
}

void PaintCard(Surface& s, const Theme& t) {
    const RECT& card = g_layout.card;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f,
                    S(kCardRadius), S(1.0f), t.stroke);

    const ChordEntry* entry = SelectedChord();
    const Voicing* voicing = SelectedVoicing();
    if (!entry || !voicing) {
        RECT empty = g_layout.card;
        g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontRow)), FW_NORMAL),
                    L"Pick a chord", empty, DT_CENTER | DT_VCENTER |
                                                DT_SINGLELINE,
                    t.textFaint);
        return;
    }

    HFONT nameFont = g_fonts.Get(S(static_cast<float>(kFontChord)),
                                 kWeightSemiBold);
    g_text.Draw(s, nameFont, entry->name, g_layout.chordName,
                DT_LEFT | DT_TOP | DT_SINGLELINE, t.text);

    HFONT toneFont = g_fonts.Get(S(static_cast<float>(kFontTone)), FW_NORMAL);
    g_text.Draw(s, toneFont,
                ChordTones(g_app.instrument, *entry, *voicing), g_layout.tones,
                DT_LEFT | DT_TOP | DT_SINGLELINE, t.textMuted);

    PaintPlayButton(s, t);

    if (chorddata::kInstruments[g_app.instrument].kind == chorddata::kFretted) {
        PaintFrettedChord(s, t, *voicing, g_layout.diagram);
    } else {
        PaintPianoChord(s, t, *voicing, g_layout.diagram);
    }
    PaintVoicingSlider(s, t);
}

void PaintTempo(Surface& s, const Theme& t) {
    const RECT& pad = g_layout.tapPad;
    const bool hovered = g_hover.id == kHitTapPad;
    FillRoundRect(s, L(pad.left), L(pad.top), L(pad.right), L(pad.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(pad.left) + 0.5f, L(pad.top) + 0.5f,
                    L(pad.right) - 0.5f, L(pad.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    // The dial doubles as the tap target: each tap lights it and sends a ring
    // outwards, so the beat is something you can see.
    const float cx = L(pad.left + pad.right) * 0.5f;
    const float cy =
        L(g_layout.tapValue.top + g_layout.tapValue.bottom) * 0.5f;
    const float radius = S(82.0f);
    const double since = NowMs() - g_tap.flashAt;
    const float flash = static_cast<float>(Clamp01(1.0 - since / 320.0));
    FillCircle(s, cx, cy, radius, hovered ? t.controlHover : t.control);
    if (flash > 0.0f) {
        FillCircle(s, cx, cy, radius, WithAlpha(t.accent, 0.14f * flash));
        StrokeCircle(s, cx, cy, radius + S(6.0f) + (1.0f - flash) * S(12.0f),
                     S(2.0f), WithAlpha(t.accent, 0.55f * flash));
    }
    StrokeCircle(s, cx, cy, radius, S(1.2f),
                 flash > 0.0f ? WithAlpha(t.accent, 0.25f + 0.45f * flash)
                              : t.stroke);

    HFONT valueFont =
        g_fonts.Get(S(static_cast<float>(kFontBpm)), kWeightSemiBold);
    const std::wstring value =
        g_tap.hasValue
            ? std::to_wstring(static_cast<int>(lround(g_tap.bpm)))
            : std::wstring(L"\u2014");
    g_text.Draw(s, valueFont, value, g_layout.tapValue,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                g_tap.hasValue ? t.text : t.textFaint);

    HFONT unitFont = g_fonts.Get(S(static_cast<float>(kFontBpmUnit)), FW_NORMAL);
    RECT unit{g_layout.tapValue.left, g_layout.tapValue.bottom - S(4.0f),
              g_layout.tapValue.right, g_layout.tapValue.bottom + S(16.0f)};
    g_text.Draw(s, unitFont, g_tap.hasValue ? L"BPM" : L"tap to start", unit,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textMuted);

    HFONT nameFont = g_fonts.Get(S(static_cast<float>(kFontTone)),
                                 kWeightSemiBold);
    if (g_tap.hasValue) {
        g_text.Draw(s, nameFont, TempoName(g_tap.bpm), g_layout.tapName,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.accent);
    }

    // One bar per stored interval, taller when that tap lagged the average, so
    // an unsettled tempo is visible at a glance.
    const float barsLeft = L(g_layout.tapBars.left);
    const float barsRight = L(g_layout.tapBars.right);
    const float gap = S(6.0f);
    const float barWidth =
        (barsRight - barsLeft - gap * static_cast<float>(kMaxTapIntervals - 1)) /
        static_cast<float>(kMaxTapIntervals);
    const float bottom = L(g_layout.tapBars.bottom);
    const float full = L(g_layout.tapBars.bottom - g_layout.tapBars.top);
    const double average = TapAverageInterval();
    for (int i = 0; i < kMaxTapIntervals; i++) {
        const bool filled = i < g_tap.intervalCount;
        float height = S(3.0f);
        if (filled && average > 0.0) {
            const double ratio = g_tap.intervals[i] / average;
            const float even = static_cast<float>(Clamp01((ratio - 0.6) / 0.8));
            height = full * (0.25f + 0.75f * even);
        }
        const float x = barsLeft + static_cast<float>(i) * (barWidth + gap);
        FillRoundRect(s, x, bottom - height, x + barWidth, bottom, S(3.0f),
                      filled ? WithAlpha(t.accent, 0.85f)
                             : WithAlpha(t.textMuted, 0.22f));
    }

    HFONT hintFont = g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
    std::wstring hint = L"Tap the dial, or press ";
    hint += TapKeyLabel();
    g_text.Draw(s, hintFont, hint, g_layout.tapHint,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);

    // Half / Reset / Double, in the order a player reaches for them.
    HFONT buttonFont = g_fonts.Get(S(static_cast<float>(kFontTab)),
                                   kWeightSemiBold);
    auto Button = [&](const RECT& rect, PCWSTR label, int id) {
        const bool hot = g_hover.id == id;
        FillRoundRect(s, L(rect.left), L(rect.top), L(rect.right), L(rect.bottom),
                      S(8.0f), hot ? t.controlHover : t.control);
        StrokeRoundRect(s, L(rect.left) + 0.5f, L(rect.top) + 0.5f,
                        L(rect.right) - 0.5f, L(rect.bottom) - 0.5f, S(8.0f),
                        S(1.0f), hot ? WithAlpha(t.accent, 0.35f) : t.stroke);
        RECT text = rect;
        g_text.Draw(s, buttonFont, label, text,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    hot ? t.text : t.textMuted);
    };
    Button(g_layout.tapHalf, L"Half", kHitTapHalf);
    Button(g_layout.tapReset, L"Reset", kHitTapReset);
    Button(g_layout.tapDouble, L"Double", kHitTapDouble);
}

// The tempo readout while its field is open: the digits typed so far in place
// of the playing tempo, with a caret under the last of them. The caret blinks on
// the same rhythm as the search box, so typing reads the same everywhere.
void PaintTempoEntry(Surface& s,
                     const Theme& t,
                     const RECT& box,
                     HFONT font) {
    if (!TempoEntryActive()) {
        return;
    }
    std::wstring text;
    for (int i = 0; i < g_tempoEntry.count; i++) {
        text += g_tempoEntry.digits[i];
    }
    g_text.Draw(s, font, text, box, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                t.text);
    if (fmod(NowMs() - g_tempoEntry.typedAt, 1060.0) >= 530.0) {
        return;
    }
    const SIZE size = g_text.Measure(font, text);
    const float x = (L(box.left) + L(box.right)) * 0.5f +
                    static_cast<float>(size.cx) * 0.5f + S(2.0f);
    StrokeLine(s, {x, L(box.top) + S(11.0f)}, {x, L(box.bottom) - S(11.0f)},
               S(1.4f), t.accent);
}

// The metronome card: tempo, the beat row, the meter chips, the subdivision
// row and the transport, stacked in the same rectangle the tap tempo pad uses.
void PaintMetronome(Surface& s, const Theme& t) {
    const RECT& card = g_layout.metroPad;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    const double bpm = ToolkitBpm();
    HFONT valueFont =
        g_fonts.Get(S(static_cast<float>(kFontBpm)), kWeightSemiBold);
    if (TempoEntryActive()) {
        PaintTempoEntry(s, t, g_layout.metroValue, valueFont);
    } else {
        g_text.Draw(s, valueFont, std::to_wstring(lround(bpm)),
                    g_layout.metroValue,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    g_hover.id == kHitMetroTempo ? t.accent : t.text);
    }

    HFONT metaFont =
        g_fonts.Get(S(static_cast<float>(kFontTone)), kWeightSemiBold);
    std::wstring meta = TempoName(bpm);
    meta += L" \x00B7 ";
    meta += MeterLabel(g_metro.beats, g_metro.unit);
    g_text.Draw(s, metaFont, meta, g_layout.metroMeta,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.accent);

    // One dot per beat, with a wider gap at every group boundary so a 7/8 or a
    // 12/8 bar reads at a glance. The downbeat keeps a ring of its own.
    const bool running = g_metro.running;
    const int beats = std::clamp(g_metro.beats, 1, 16);
    const int group = std::clamp(g_metro.group, 1, 4);
    const int current = running ? g_metroShared.beat.load() % beats : 0;
    const float radius = S(beats > 8 ? 5.5f : 8.0f);
    const float gap = S(beats > 8 ? 7.0f : 14.0f);
    const float groupGap = group > 1 ? S(7.0f) : 0.0f;
    const int breaks = group > 1 ? (beats - 1) / group : 0;
    const float total =
        beats * radius * 2 + (beats - 1) * gap + breaks * groupGap;
    const float cy =
        L(g_layout.metroDots.top + g_layout.metroDots.bottom) * 0.5f;
    float x = L(g_layout.metroDots.left + g_layout.metroDots.right) * 0.5f -
              total * 0.5f + radius;
    const float pulse =
        static_cast<float>(Clamp01(1.0 - (NowMs() - g_metro.pulseAt) / 220.0));
    for (int i = 0; i < beats; i++) {
        if (i > 0) {
            x += radius * 2 + gap + (group > 1 && i % group == 0 ? groupGap : 0);
        }
        const bool active = i == current;
        FillCircle(s, x, cy, radius,
                   active ? t.accent
                          : WithAlpha(t.textMuted, running ? 0.26f : 0.20f));
        if (i == 0) {
            StrokeCircle(s, x, cy, radius + S(3.0f), S(1.4f),
                         WithAlpha(t.accent, active ? 0.55f : 0.30f));
        }
        if (active && running && pulse > 0.0f) {
            StrokeCircle(s, x, cy, radius + S(2.0f) + (1.0f - pulse) * S(9.0f),
                         S(2.0f), WithAlpha(t.accent, 0.45f * pulse));
        }
    }

    HFONT chipFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    for (int i = 0; i < kSignatureCount; i++) {
        const std::wstring label =
            MeterLabel(kSignatures[i].beats, kSignatures[i].unit);
        PaintChip(s, t, g_layout.metroPreset[i], label.c_str(), chipFont,
                  g_metro.beats == kSignatures[i].beats &&
                      g_metro.unit == kSignatures[i].unit,
                  g_hover.id == kHitMetroPreset && g_hover.index == i);
    }

    PaintChip(s, t, g_layout.metroBeatsDown, L"\x2212", chipFont, false,
              g_hover.id == kHitMetroBeatsDown);
    PaintChip(s, t, g_layout.metroBeatsUp, L"+", chipFont, false,
              g_hover.id == kHitMetroBeatsUp);
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontTab)), FW_NORMAL),
                std::to_wstring(g_metro.beats) + L" beats / bar",
                g_layout.metroBeatsLabel,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textMuted);
    for (int i = 0; i < 4; i++) {
        const std::wstring label = L"1/" + std::to_wstring(kBeatUnits[i]);
        PaintChip(s, t, g_layout.metroUnit[i], label.c_str(), chipFont,
                  g_metro.unit == kBeatUnits[i],
                  g_hover.id == kHitMetroUnit && g_hover.index == i);
    }

    for (int i = 0; i < 4; i++) {
        PaintChip(s, t, g_layout.metroSub[i], SubdivisionLabel(i + 1), chipFont,
                  g_metro.subdivision == i + 1,
                  g_hover.id == kHitMetroSubdivision && g_hover.index == i);
    }
    const std::wstring groupLabel =
        g_metro.group > 1 ? L"Group " + std::to_wstring(g_metro.group)
                          : std::wstring(L"Group off");
    PaintChip(s, t, g_layout.metroGroup, groupLabel.c_str(), chipFont,
              g_metro.group > 1, g_hover.id == kHitMetroGroup);

    HFONT buttonFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    const bool tapHot = g_hover.id == kHitMetroTap;
    FillRoundRect(s, L(g_layout.metroTap.left), L(g_layout.metroTap.top),
                  L(g_layout.metroTap.right), L(g_layout.metroTap.bottom),
                  S(9.0f), tapHot ? t.controlHover : t.control);
    StrokeRoundRect(s, L(g_layout.metroTap.left) + 0.5f,
                    L(g_layout.metroTap.top) + 0.5f,
                    L(g_layout.metroTap.right) - 0.5f,
                    L(g_layout.metroTap.bottom) - 0.5f, S(9.0f), S(1.0f),
                    t.stroke);
    RECT tapText = g_layout.metroTap;
    g_text.Draw(s, buttonFont, L"Tap", tapText,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                tapHot ? t.text : t.textMuted);

    const bool startHot = g_hover.id == kHitMetroStart;
    const RECT& start = g_layout.metroStart;
    if (running) {
        FillRoundRect(s, L(start.left), L(start.top), L(start.right),
                      L(start.bottom), S(9.0f), startHot ? t.controlHover
                                                         : t.control);
        StrokeRoundRect(s, L(start.left) + 0.5f, L(start.top) + 0.5f,
                        L(start.right) - 0.5f, L(start.bottom) - 0.5f, S(9.0f),
                        S(1.0f), WithAlpha(t.accent, 0.45f));
    } else {
        FillRoundRect(s, L(start.left), L(start.top), L(start.right),
                      L(start.bottom), S(9.0f),
                      WithAlpha(t.accent, startHot ? 1.0f : 0.93f));
    }
    RECT startText = start;
    g_text.Draw(s, buttonFont, running ? L"Stop" : L"Start", startText,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                running ? t.text : t.onAccent);

    std::wstring hint;
    if (TempoEntryActive()) {
        hint = L"Type a tempo \x00B7 Enter sets it \x00B7 Esc cancels";
    } else if (running) {
        hint = L"Space stops \x00B7 \x2191\x2193 tempo \x00B7 \x2190\x2192 meter";
    } else {
        hint = L"Space starts \x00B7 \x2191\x2193 tempo \x00B7 \x2190\x2192 meter";
    }
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                hint, g_layout.metroHint,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);
}

// The three selects in the strumming card - the pattern, the meter and the
// chord - share this chrome, so a list that is up reads the same whichever one
// opened it. The caption names what the value is, which is what keeps a bare
// "Folk" from looking like a label rather than a menu.
void PaintStrumSelect(Surface& s,
                      const Theme& t,
                      const RECT& box,
                      PCWSTR caption,
                      PCWSTR label,
                      HFONT font,
                      bool open,
                      bool hot) {
    FillRoundRect(s, L(box.left), L(box.top), L(box.right), L(box.bottom),
                  S(9.0f), open || hot ? t.controlHover : t.control);
    StrokeRoundRect(s, L(box.left) + 0.5f, L(box.top) + 0.5f,
                    L(box.right) - 0.5f, L(box.bottom) - 0.5f, S(9.0f),
                    S(1.0f), open ? WithAlpha(t.accent, 0.55f) : t.stroke);
    RECT text = box;
    text.left += S(12);
    text.right -= S(28);
    if (caption && *caption) {
        const std::wstring prefix = std::wstring(caption) + L"   ";
        const long width = std::min(g_text.Measure(font, prefix).cx,
                                    text.right - text.left - S(24));
        RECT cap = text;
        cap.right = cap.left + width;
        g_text.Draw(s, font, prefix.c_str(), cap,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS,
                    t.textMuted);
        text.left = cap.right;
    }
    g_text.Draw(s, font, label, text,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS,
                t.text);
    // The chevron flips while its list is up, the way a Fluent combo does.
    PaintGlyph(s, L(box.right) - S(14),
               (L(box.top) + L(box.bottom)) * 0.5f,
               open ? L'\xE70E' : L'\xE70D', S(10.0f),
               open ? t.accent : t.textMuted);
}

// The keycaps live with the footer further down; the card's hint line paints
// its keys with that same chrome, so a shortcut reads the same wherever it is
// written.
void PaintKeycap(Surface& s,
                 const Theme& t,
                 HFONT font,
                 const std::wstring& key,
                 const RECT& box);

// One piece of a hint line: a key in its cap, a run of plain words, or both -
// the key then what it does. A token with no key is just words, and a token
// with no text is a bare cap.
struct HintToken {
    PCWSTR key;
    PCWSTR text;
};

// A centred hint made of keycaps and words. The caps are measured with the
// words beside them and the whole line is laid out from its own width, so the
// dot separators always sit square between the pairs.
void PaintHintRow(Surface& s,
                  const Theme& t,
                  const RECT& row,
                  const HintToken* tokens,
                  int count) {
    HFONT keyFont =
        g_fonts.Get(S(static_cast<float>(kFontFoot)), kWeightSemiBold);
    HFONT textFont = g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
    const int capPad = S(5);
    const int capGap = S(4);
    const int dotPad = S(7);
    const int dotWidth = g_text.Measure(textFont, L"\x00B7").cx;

    // The keys are measured first, so the line can be centred as one run.
    int total = 0;
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            total += dotWidth + dotPad * 2;
        }
        if (tokens[i].key) {
            total += g_text.Measure(keyFont, tokens[i].key).cx + capPad * 2;
            if (tokens[i].text) {
                total += capGap;
            }
        }
        if (tokens[i].text) {
            total += g_text.Measure(textFont, tokens[i].text).cx;
        }
    }

    const int capHeight = S(15);
    const int centerY = (row.top + row.bottom) / 2;
    int x = row.left + (static_cast<int>(row.right - row.left) - total) / 2;
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            const RECT dot{x, row.top, x + dotWidth + dotPad * 2, row.bottom};
            g_text.Draw(s, textFont, L"\x00B7", dot,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);
            x += dotWidth + dotPad * 2;
        }
        if (tokens[i].key) {
            const int width =
                g_text.Measure(keyFont, tokens[i].key).cx + capPad * 2;
            const int capTop = centerY - capHeight / 2;
            const RECT cap{x, capTop, x + width, capTop + capHeight};
            PaintKeycap(s, t, keyFont, std::wstring(tokens[i].key), cap);
            x += width;
            if (tokens[i].text) {
                x += capGap;
            }
        }
        if (tokens[i].text) {
            const int width = g_text.Measure(textFont, tokens[i].text).cx;
            const RECT label{x, row.top, x + width + S(2), row.bottom};
            g_text.Draw(s, textFont, tokens[i].text, label,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE, t.textFaint);
            x += width;
        }
    }
}

// The strumming card: the pattern select, the pattern grid with a moving
// playhead, a meter and density row, the transport with a tempo stepper, the
// rows that pick the voice and the practice switches, then a hint line.
void PaintStrum(Surface& s, const Theme& t) {
    const RECT& card = g_layout.strumPad;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    HFONT chipFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);

    // The grid answers to a pattern the moment one is picked, and to nothing
    // once a cell is edited by hand, which is what "Custom" says.
    const int preset = g_strum.preset;
    PaintStrumSelect(s, t, g_layout.strumPattern, L"Pattern",
                     preset >= 0 && preset < kStrumPresetCount
                         ? kStrumPresets[preset].name
                         : L"Custom",
                     chipFont,
                     MenuHit(g_patternMenu) &&
                         g_patternMenu.shown == kMenuStrumPattern,
                     g_hover.id == kHitStrumPattern);

    // The pattern grid.
    const int cells = std::clamp(StrumSlotCount(), 1, kStrumSlotsMax);
    const int subdivision = std::clamp(g_strum.subdivision, 1, 4);
    const bool running = g_strum.running;
    // The engine's count-in flag rather than the measure count: no played bar
    // has ticked a measure yet, so the count alone hid the playhead for the
    // whole first bar.
    const bool countingIn = running && g_metroShared.strumCounting.load();
    const int current =
        running && !countingIn ? std::clamp(g_metroShared.slot.load(), 0, cells - 1)
                               : -1;
    const float pulse = static_cast<float>(Clamp01(
        1.0 - (NowMs() - g_strum.paintedSlotAt) / 220.0));

    // Faint barlines split the bar into beats; the downbeat is heavier.
    for (int i = 0; i < cells; i++) {
        const RECT& cell = g_layout.strumSlot[i];
        if (i > 0 && i % subdivision == 0) {
            StrokeLine(s, {L(cell.left), L(cell.top) + S(4)},
                       {L(cell.left), L(cell.bottom) - S(4)}, S(1.0f), t.grid);
        }
    }
    const RECT& firstCell = g_layout.strumSlot[0];
    const RECT& lastCell = g_layout.strumSlot[cells - 1];
    StrokeLine(s, {L(firstCell.left), L(firstCell.top) + S(2)},
               {L(firstCell.left), L(firstCell.bottom) - S(2)}, S(1.6f), t.grid);
    StrokeLine(s, {L(lastCell.right), L(lastCell.top) + S(2)},
               {L(lastCell.right), L(lastCell.bottom) - S(2)}, S(1.6f), t.grid);

    const int glyphPx = std::max(
        12, std::min(22, static_cast<int>(g_layout.strumSlot[0].right -
                                         g_layout.strumSlot[0].left) -
                             S(4)));
    HFONT glyphFont =
        g_fonts.Get(S(static_cast<float>(glyphPx)), kWeightSemiBold);
    HFONT markFont = g_fonts.Get(S(static_cast<float>(glyphPx)) * 0.6f,
                                 kWeightSemiBold);
    // Brackets either side of a point, the way a ghost note is written on a
    // staff. The arms are short, so the mark stays inside its cell however
    // tight the grid gets.
    const auto paintGhost = [&](float gx, float gy, float arm, float half,
                                float elbow, Color ink) {
        StrokeLine(s, {gx - arm, gy - half}, {gx - arm - elbow, gy}, S(1.2f),
                   ink);
        StrokeLine(s, {gx - arm - elbow, gy}, {gx - arm, gy + half}, S(1.2f),
                   ink);
        StrokeLine(s, {gx + arm, gy - half}, {gx + arm + elbow, gy}, S(1.2f),
                   ink);
        StrokeLine(s, {gx + arm + elbow, gy}, {gx + arm, gy + half}, S(1.2f),
                   ink);
    };
    float previousCenter = 0.0f;
    bool havePrevious = false;
    for (int i = 0; i < cells; i++) {
        const RECT& cell = g_layout.strumSlot[i];
        const float cx = (L(cell.left) + L(cell.right)) * 0.5f;
        const float cy = (L(cell.top) + L(cell.bottom)) * 0.5f;
        const int stroke = g_strum.stroke[i];
        const bool sounded = stroke != kStrokeRest;

        if (current == i) {
            FillRoundRect(s, L(cell.left) + S(1.0f), L(cell.top) + S(3.0f),
                          L(cell.right) - S(1.0f), L(cell.bottom) - S(3.0f),
                          S(7.0f), t.selection);
        } else if (!running && g_strum.slotCursor == i) {
            StrokeRoundRect(s, L(cell.left) + 0.5f, L(cell.top) + S(3.0f),
                            L(cell.right) - 0.5f, L(cell.bottom) - S(3.0f),
                            S(7.0f), S(1.4f), WithAlpha(t.accent, 0.55f));
        }

        // A tie lets the previous stroke ring, so no new arrow is drawn and a
        // slur is bent from the last struck cell into this one. The chain keeps
        // its start, so a run of tied cells reads as one bend, and a tie with
        // nothing behind it starts inside its own cell rather than vanishing.
        if (g_strum.tie[i]) {
            const float y = L(cell.bottom) - S(9.0f);
            const float apex = y - S(11.0f);
            const float span = L(cell.right - cell.left);
            const float from =
                havePrevious ? previousCenter
                             : cx - std::min(L(S(7)), span * 0.3f);
            Vec2 a{from, y};
            Vec2 b{cx, y};
            Vec2 prev = a;
            for (int k = 1; k <= 8; k++) {
                const float u = k / 8.0f;
                const float mx =
                    (1 - u) * (1 - u) * a.x + 2 * (1 - u) * u *
                        ((a.x + b.x) * 0.5f) +
                    u * u * b.x;
                const float my =
                    (1 - u) * (1 - u) * a.y + 2 * (1 - u) * u * apex +
                    u * u * b.y;
                StrokeLine(s, prev, {mx, my}, S(1.4f),
                           WithAlpha(t.text, 0.55f));
                prev = {mx, my};
            }
            previousCenter = cx;
            havePrevious = true;
        } else if (sounded) {
            const bool up = stroke == kStrokeUp || stroke == kStrokeUpMute;
            const bool mute =
                stroke == kStrokeDownMute || stroke == kStrokeUpMute;
            Color color =
                g_strum.accent[i] ? t.accent : (mute ? t.textMuted : t.text);
            if (g_strum.ghost[i]) {
                // A ghost is barely played, so it is drawn faint and inside
                // brackets: the mark is on the cell, not only in the ink.
                color = WithAlpha(color, 0.45f);
            }
            const std::wstring glyph =
                up ? std::wstring(L"\x2191") : std::wstring(L"\x2193");
            g_text.Draw(s, glyphFont, glyph, cell,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE, color);
            if (mute) {
                // A scratch is shown as a bar struck across the arrow.
                StrokeLine(s, {cx - S(6.0f), cy + S(3.0f)},
                           {cx + S(6.0f), cy + S(3.0f)}, S(1.6f), color);
            }
            if (g_strum.accent[i]) {
                RECT caret{cell.left, cell.top, cell.right,
                           cell.top + S(14)};
                g_text.Draw(s, markFont, L">", caret,
                            DT_CENTER | DT_TOP | DT_SINGLELINE, t.accent);
            }
            if (g_strum.ghost[i]) {
                const float span = L(cell.right - cell.left);
                const float arm = std::clamp(span * 0.34f, L(S(5)), L(S(9)));
                paintGhost(cx, cy, arm, S(7.0f), S(2.0f), color);
            }
            previousCenter = cx;
            havePrevious = true;
        } else {
            // An empty cell is a rest, marked with the same faint dot the
            // pattern rows use so the grid and the list read alike.
            FillCircle(s, cx, cy, S(1.6f), t.textFaint);
            // A rest still answers the ghost key: the brackets are drawn on an
            // empty cell too, so pressing G always shows something.
            if (g_strum.ghost[i]) {
                const float span = L(cell.right - cell.left);
                const float arm = std::clamp(span * 0.24f, L(S(4)), L(S(8)));
                paintGhost(cx, cy, arm, S(5.0f), S(1.6f), t.textFaint);
            }
            havePrevious = false;
        }

        if (current == i && running && pulse > 0.0f) {
            StrokeCircle(s, cx, cy, S(9.0f) + (1.0f - pulse) * S(8.0f), S(1.6f),
                         WithAlpha(t.accent, 0.4f * pulse));
        }
    }

    // Meter and grid density: the meter is a list of the meters the programme
    // knows, and the chips name the note value one cell stands for. Changing
    // either one leaves the bar as it is and calls it custom.
    PaintStrumSelect(s, t, g_layout.strumMeterSelect, L"Meter",
                     MeterLabel(g_strum.beats, g_strum.unit).c_str(), chipFont,
                     MenuHit(g_meterMenu) &&
                         g_meterMenu.shown == kMenuStrumMeter,
                     g_hover.id == kHitStrumMeter);
    const int gridLimit = StrumGridLimit(g_strum.beats);
    for (int i = 0; i < 4; i++) {
        const bool fits = i + 1 <= gridLimit;
        PaintChip(s, t, g_layout.strumSub[i],
                  StrumGridLabel(i + 1, g_strum.unit), chipFont,
                  g_strum.subdivision == i + 1,
                  fits && g_hover.id == kHitStrumSub && g_hover.index == i,
                  fits);
    }

    // Transport: one wide bar that starts and stops the bar, then a tempo
    // stepper. The bar is filled while idle and outlined while it runs, so the
    // glyph is the only thing that has to change when the state does.
    const bool playHot = g_hover.id == kHitStrumPlay;
    const RECT& play = g_layout.strumPlay;
    if (running) {
        FillRoundRect(s, L(play.left), L(play.top), L(play.right),
                      L(play.bottom), S(9.0f),
                      playHot ? t.controlHover : t.control);
        StrokeRoundRect(s, L(play.left) + 0.5f, L(play.top) + 0.5f,
                        L(play.right) - 0.5f, L(play.bottom) - 0.5f, S(9.0f),
                        S(1.0f), WithAlpha(t.accent, 0.45f));
    } else {
        FillRoundRect(s, L(play.left), L(play.top), L(play.right),
                      L(play.bottom), S(9.0f),
                      WithAlpha(t.accent, playHot ? 1.0f : 0.93f));
    }
    // A Fluent transport glyph rather than a word: the triangle while the
    // pattern is waiting, the filled square while it runs.
    PaintGlyph(s, L(play.left + play.right) * 0.5f,
               L(play.top + play.bottom) * 0.5f,
               running ? L'\xE71A' : L'\xE768', S(18.0f),
               running ? t.text : t.onAccent);

    PaintChip(s, t, g_layout.strumTempoDown, L"\x2212", chipFont, false,
              g_hover.id == kHitStrumTempoDown);
    PaintChip(s, t, g_layout.strumTempoUp, L"+", chipFont, false,
              g_hover.id == kHitStrumTempoUp);
    HFONT tempoFont = g_fonts.Get(S(20.0f), kWeightSemiBold);
    if (TempoEntryActive()) {
        // The field borrows the stepper's frame while it is open, so the
        // digits read as a place text goes rather than as a tempo.
        const RECT& box = g_layout.strumTempo;
        FillRoundRect(s, L(box.left), L(box.top), L(box.right), L(box.bottom),
                      S(9.0f), t.control);
        StrokeRoundRect(s, L(box.left) + 0.5f, L(box.top) + 0.5f,
                        L(box.right) - 0.5f, L(box.bottom) - 0.5f, S(9.0f),
                        S(1.0f), WithAlpha(t.accent, 0.55f));
        PaintTempoEntry(s, t, box, tempoFont);
    } else {
        g_text.Draw(s, tempoFont, std::to_wstring(lround(ToolkitBpm())),
                    g_layout.strumTempo,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    g_hover.id == kHitStrumTempo ? t.accent : t.text);
    }

    // The strings: which instrument, and which chord they are fretted to. The
    // chord is a select, since a list of ten would not fit a chip.
    for (int i = 0; i < 2; i++) {
        const int instrument = i == 0 ? kInstrumentGuitar : kInstrumentUkulele;
        PaintChip(s, t, g_layout.strumInst[i],
                  kInstrumentChipLabels[instrument], chipFont,
                  g_strum.instrument == instrument,
                  g_hover.id == kHitStrumInst && g_hover.index == i);
    }
    PaintStrumSelect(s, t, g_layout.strumChord, L"Chord",
                     kStrumChords[g_strum.chord].name, chipFont,
                     MenuHit(g_strumMenu) && g_strumMenu.shown == kMenuStrumChord,
                     g_hover.id == kHitStrumChord);

    // What the strings ring like.
    static const PCWSTR kSoundLabels[kStrumSoundCount] = {L"Open", L"Palm",
                                                          L"Muted"};
    for (int i = 0; i < kStrumSoundCount; i++) {
        PaintChip(s, t, g_layout.strumSound[i], kSoundLabels[i], chipFont,
                  g_strum.sound == i,
                  g_hover.id == kHitStrumSound && g_hover.index == i);
    }

    // Practice switches.
    static const PCWSTR kChipLabels[kStrumChipCount] = {L"Loop", L"Count-in",
                                                        L"Practice"};
    const bool chipActive[kStrumChipCount] = {g_strum.loop, g_strum.countIn,
                                              g_strum.practice};
    for (int i = 0; i < kStrumChipCount; i++) {
        PaintChip(s, t, g_layout.strumChip[i], kChipLabels[i], chipFont,
                  chipActive[i],
                  g_hover.id == kHitStrumChip && g_hover.index == i);
    }

    if (TempoEntryActive() || countingIn || running) {
        std::wstring hint;
        if (TempoEntryActive()) {
            // The field says what it wants, so a typed number is not a guess.
            hint = L"Type a tempo \x00B7 Enter sets it \x00B7 Esc cancels";
        } else if (countingIn) {
            hint = L"Count in \x00B7 get ready";
        } else {
            hint = L"Space stops";
            if (g_strum.practice) {
                hint += L" \x00B7 ramp to ";
                hint += std::to_wstring(g_strum.targetBpm);
                hint += L" bpm";
            }
        }
        g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)),
                                   FW_NORMAL),
                    hint, g_layout.strumHint,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);
    } else {
        // Idle: the marks that ride on a stroke, each on the key that sets it,
        // drawn as caps so the letters read as keys and not as prose.
        static const HintToken kIdleHint[] = {
            {nullptr, L"Click a cell"},
            {L"A", L"accent"},
            {L"G", L"ghost"},
            {L"T", L"tie"},
        };
        PaintHintRow(s, t, g_layout.strumHint, kIdleHint,
                     static_cast<int>(sizeof(kIdleHint) /
                                      sizeof(kIdleHint[0])));
    }
}

// The chord list the strum select opens, two columns of five drawn last so it
// sits over the pattern. It shares the shell, the rows and the animation with
// the other lists, so every popup in the panel reads as the same control.
void PaintStrumMenu(Surface& s, const Theme& t) {
    const RECT reveal = PaintListShell(s, t, g_layout.strumChordMenu,
                                       g_strumMenu.progress, true);
    ListItem items[kStrumChordCount];
    for (int i = 0; i < kStrumChordCount; i++) {
        items[i].label = kStrumChords[i].name;
        items[i].active = g_strum.chord == i;
        items[i].hovered =
            (MenuHit(g_strumMenu) && g_hover.id == kHitStrumChordRow &&
             g_hover.index == i) ||
            g_strumMenu.cursor == i;  // The keyboard cursor rides with the list.
    }
    PaintListRows(s, t, items, kStrumChordCount, g_layout.strumChordRow,
                  reveal, true);
}

// One bar of a preset, drawn in the row's own terms: an arrow per grid cell, a
// bar through the scratched ones, the accent colour on the accented ones and a
// faint tick where the hand rests. Beat boundaries get the same faint rule the
// grid uses, so a grouping like 2+2+3 reads at a glance.
void PaintStrumStrip(Surface& s,
                     const Theme& t,
                     const StrumPreset& p,
                     const RECT& row,
                     float alpha) {
    const int cells = std::clamp(p.beats * p.subdivision, 1, kStrumSlotsMax);
    const float area = L(row.right - row.left);
    const float advance = std::min(area / cells, static_cast<float>(S(30.0f)));
    const float origin = L(row.left) + (area - advance * cells) * 0.5f;
    const float cy = (L(row.top) + L(row.bottom)) * 0.5f;
    const int glyphPx = std::clamp(static_cast<int>(advance) - S(8), 10, 16);
    HFONT font = g_fonts.Get(S(static_cast<float>(glyphPx)), kWeightSemiBold);
    Color rule = t.grid;
    rule.a *= alpha;
    for (int i = 1; i < cells; i++) {
        if (p.subdivision < 2 || i % p.subdivision != 0) {
            continue;
        }
        const float x = origin + advance * static_cast<float>(i);
        StrokeLine(s, {x, L(row.top) + S(4.0f)}, {x, L(row.bottom) - S(4.0f)},
                   S(1.0f), rule);
    }
    for (int i = 0; i < cells; i++) {
        const float cx = origin + advance * (static_cast<float>(i) + 0.5f);
        const StrumCellLook look = StrumDecodeCell(p.cells[i]);
        if (look.stroke == kStrokeRest) {
            Color tick = t.textFaint;
            tick.a *= alpha;
            FillCircle(s, cx, cy, S(1.2f), tick);
            continue;
        }
        const bool up = look.stroke == kStrokeUp || look.stroke == kStrokeUpMute;
        const bool mute =
            look.stroke == kStrokeDownMute || look.stroke == kStrokeUpMute;
        Color ink = look.accent ? t.accent : (mute ? t.textMuted : t.text);
        ink.a *= alpha;
        RECT box{static_cast<long>(cx - advance * 0.5f), row.top,
                 static_cast<long>(cx + advance * 0.5f), row.bottom};
        g_text.Draw(s, font, up ? L"\x2191" : L"\x2193", box,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE, ink);
        if (mute) {
            StrokeLine(s, {cx - S(5.0f), cy + S(3.0f)},
                       {cx + S(5.0f), cy + S(3.0f)}, S(1.3f), ink);
        }
    }
}

// The pattern list: the meter's patterns, grouped under the grid each one is
// written in and drawn as the bar it writes rather than as a name, with the
// name kept down the left so a row can be matched to the select above it.
void PaintStrumPatternMenu(Surface& s, const Theme& t) {
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    if (count == 0) {
        return;
    }
    // The card gives the list room for the busiest meter; a meter with fewer
    // patterns folds the extra lines away instead of leaving an empty plate.
    const int rowHeight =
        g_layout.strumPatternRow[0].bottom - g_layout.strumPatternRow[0].top;
    RECT full = g_layout.strumPatternMenu;
    full.bottom = full.top + S(8) + count * rowHeight;
    const RECT reveal = PaintListShell(s, t, full, g_patternMenu.progress);
    HFONT nameFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    HFONT headFont =
        g_fonts.Get(S(static_cast<float>(kFontFoot)), kWeightSemiBold);
    const float front = L(reveal.top);
    const float travel = std::max(1.0f, static_cast<float>(S(20.0f)));
    for (int i = 0; i < count; i++) {
        const RECT& row = g_layout.strumPatternRow[i];
        // A line is drawn as the front edge of the unroll reaches it, exactly
        // like the lists that go through PaintListRows.
        const float appear = Clamp01((front - L(row.top)) / travel);
        if (appear <= 0.0f) {
            continue;
        }
        const float alpha = MenuAlpha(appear);
        const float top = L(row.top) + (1.0f - appear) * S(6.0f);
        const float bottom = L(row.bottom) + (1.0f - appear) * S(6.0f);
        const StrumEntry& entry = entries[i];
        if (entry.kind == kStrumEntryHeading) {
            // A heading names the grid the lines under it are written in. It
            // is a label rather than a choice, so it takes no wash, and a
            // hairline under it ties the rows below to it.
            std::wstring text = StrumGridLabel(entry.preset, g_strum.unit);
            text += L"  \x00B7  ";
            text += StrumGridNote(entry.preset);
            Color ink = t.textMuted;
            ink.a *= alpha * 0.9f;
            RECT label = {row.left + S(10), static_cast<long>(top),
                          row.right - S(26), static_cast<long>(bottom)};
            g_text.Draw(s, headFont, text.c_str(), label,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE |
                            DT_END_ELLIPSIS,
                        ink);
            Color rule = t.grid;
            rule.a *= alpha * 0.7f;
            StrokeLine(s, {L(row.left) + S(10), bottom - S(1)},
                       {L(row.right) - S(26), bottom - S(1)}, S(1.0f), rule);
            continue;
        }
        const bool active = g_strum.preset == entry.preset;
        const bool hovered =
            (MenuHit(g_patternMenu) && g_hover.id == kHitStrumPatternRow &&
             g_hover.index == i) ||
            g_patternMenu.cursor == i;  // The keyboard cursor rides with it.
        if (active || hovered) {
            Color wash = active ? t.selection : t.controlHover;
            wash.a *= alpha;
            FillRoundRect(s, L(row.left), top, L(row.right), bottom, S(6.0f),
                          wash);
        }
        const StrumPreset& p = kStrumPresets[entry.preset];
        // The bar keeps the bulk of the row; the name and the tick that marks
        // the loaded pattern take the two ends of it.
        RECT strip = {row.left + S(110), static_cast<long>(top),
                      row.right - S(26), static_cast<long>(bottom)};
        PaintStrumStrip(s, t, p, strip, alpha);
        RECT name = {row.left + S(10), static_cast<long>(top),
                     row.left + S(106), static_cast<long>(bottom)};
        Color ink = active ? t.text : t.textMuted;
        ink.a *= alpha;
        g_text.Draw(s, nameFont, p.name, name,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS,
                    ink);
        if (active) {
            Color mark = t.accent;
            mark.a *= alpha;
            PaintGlyph(s, L(row.right) - S(13), (top + bottom) * 0.5f,
                       L'\xE73E', S(11.0f), mark);
        }
    }
}

// The meter list: every meter the toolkit counts, what players call it, and how
// many patterns it will offer once it is picked.
void PaintStrumMeterMenu(Surface& s, const Theme& t) {
    const RECT reveal = PaintListShell(s, t, g_layout.strumMeterMenu,
                                       g_meterMenu.progress);
    std::wstring label[kSignatureCount];
    std::wstring detail[kSignatureCount];
    ListItem items[kSignatureCount];
    for (int i = 0; i < kSignatureCount; i++) {
        const int beats = kSignatures[i].beats;
        const int unit = kSignatures[i].unit;
        const int patterns = StrumPatternCount(beats, unit);
        label[i] = MeterLabel(beats, unit);
        detail[i] = MeterNote(beats, unit);
        detail[i] += L" \x00B7 ";
        detail[i] += std::to_wstring(patterns);
        detail[i] += patterns == 1 ? L" pattern" : L" patterns";
        items[i].label = label[i].c_str();
        items[i].detail = detail[i].c_str();
        items[i].active = g_strum.beats == beats && g_strum.unit == unit;
        items[i].hovered =
            (MenuHit(g_meterMenu) && g_hover.id == kHitStrumMeterRow &&
             g_hover.index == i) ||
            g_meterMenu.cursor == i;
    }
    PaintListRows(s, t, items, kSignatureCount, g_layout.strumMeterRow, reveal);
}

// The tuner card: a wide needle sweep with the note inside it, the strings of
// the tuning under it, and a row each for the instrument and the tuning.
void PaintTuner(Surface& s, const Theme& t) {
    const RECT& card = g_layout.tunerPad;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    // Fifty cents is a quarter tone either way, which is as far as the needle
    // needs to go before the player hears that it is another note.
    const RECT& arc = g_layout.tunerArc;
    const float cx = L(arc.left + arc.right) * 0.5f;
    const float pivot = L(arc.bottom);
    const float radius =
        std::min(L(arc.right - arc.left) * 0.5f - S(6.0f),
                 L(arc.bottom - arc.top) - S(10.0f));
    const float sweep = 1.0471976f;  // 60 degrees either side of upright.
    auto Point = [&](float fraction, float distance) {
        const float angle = fraction * sweep;
        return Vec2{cx + distance * sinf(angle),
                    pivot - distance * cosf(angle)};
    };

    int cents = 0;
    const int midi = TunerDisplayReading(cents);
    const bool heard = midi >= 0;
    // The needle eases toward the reading, so it swings rather than snaps.
    const float off = g_tuner.needle;
    const bool inTune = heard && cents >= -3 && cents <= 3;

    for (int step = -50; step <= 50; step += 5) {
        const bool major = step % 25 == 0;
        const Vec2 inner = Point(static_cast<float>(step) / 50.0f,
                                 radius * (major ? 0.86f : 0.915f));
        const Vec2 outer =
            Point(static_cast<float>(step) / 50.0f, radius);
        StrokeLine(s, inner, outer, S(major ? 1.6f : 1.0f),
                   major ? t.textMuted : WithAlpha(t.textMuted, 0.45f));
    }

    // The band the needle has to land in, drawn once so in tune is a place
    // rather than a guess.
    {
        Vec2 last = Point(-0.06f, radius);
        for (int step = -3; step <= 3; step++) {
            const Vec2 next = Point(static_cast<float>(step) / 50.0f, radius);
            StrokeLine(s, last, next, S(2.6f), WithAlpha(t.accent, 0.55f));
            last = next;
        }
    }

    const Color needle = !heard ? WithAlpha(t.textMuted, 0.35f)
                         : inTune ? t.accent
                                  : Mix(t.accent, t.textMuted, 0.45f);
    const Vec2 tail =
        Point(off / 50.0f, radius * 0.80f);
    const Vec2 tip = Point(off / 50.0f, radius * 0.99f);
    if (inTune) {
        FillCircle(s, tip.x, tip.y, S(9.0f), WithAlpha(t.accent, 0.20f));
    }
    if (g_tuner.pulsing) {
        // A ring plays out from the tip the moment the note lands in tune.
        const float progress = static_cast<float>(EaseOutCubic(
            (NowMs() - g_tuner.pulseAt) / 450.0));
        StrokeCircle(s, tip.x, tip.y, S(9.0f) + S(12.0f) * progress, S(1.8f),
                     WithAlpha(t.accent, 0.60f * (1.0f - progress)));
    }
    StrokeLine(s, tail, tip, S(2.2f), needle);
    FillCircle(s, tip.x, tip.y, S(3.4f), needle);

    // The note display rolls: the reading it is leaving slides up and away
    // while the new one rises into place.
    HFONT valueFont =
        g_fonts.Get(S(static_cast<float>(kFontBpm)), kWeightSemiBold);
    const Color noteColor =
        inTune ? t.accent : heard ? t.text : t.textFaint;
    const float roll = static_cast<float>(Clamp01(
        (NowMs() - g_tuner.noteAt) / kNoteSwapMs));
    const float rise = EaseOutCubic(roll);
    RECT noteRect = g_layout.tunerNote;
    if (roll < 1.0f && g_tuner.fadingNote != g_tuner.shownNote) {
        RECT outRect = noteRect;
        outRect.top -= S(14.0f) * rise;
        outRect.bottom -= S(14.0f) * rise;
        g_text.Draw(s, valueFont, NoteLabel(g_tuner.fadingNote), outRect,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                    WithAlpha(noteColor, 1.0f - rise));
    }
    RECT inRect = noteRect;
    inRect.top += S(14.0f) * (1.0f - rise);
    inRect.bottom += S(14.0f) * (1.0f - rise);
    g_text.Draw(s, valueFont, NoteLabel(g_tuner.shownNote), inRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                WithAlpha(noteColor, 0.25f + 0.75f * rise));

    HFONT centsFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    std::wstring centsText;
    Color centsColor = t.textMuted;
    if (g_tuner.micFailed) {
        centsText = L"no microphone \x00B7 check Settings \x203A Privacy";
        centsColor = t.textFaint;
    } else if (!g_tuner.listening) {
        centsText = L"listening is paused";
        centsColor = t.textFaint;
    } else if (!heard) {
        centsText = L"play a string";
    } else if (inTune) {
        centsText = L"in tune";
        centsColor = t.accent;
    } else {
        centsText = cents > 0 ? L"+" : L"\u2212";
        centsText += std::to_wstring(cents > 0 ? cents : -cents);
        centsText += L" cents";
    }
    g_text.Draw(s, centsFont, centsText, g_layout.tunerCents,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, centsColor);

    HFONT metaFont = g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
    const int strings =
        std::clamp(TunerStringCount(g_tuner.tuning), 0, kTunerStringMax);
    // A running count of the marks below, so progress reads without having
    // to scan the chips one by one.
    int settled = 0;
    for (int i = 0; i < strings; i++) {
        if (g_tuner.tuned[i]) {
            settled++;
        }
    }
    std::wstring meta = TunerInstrumentName(g_tuner.instrument);
    meta += L" \x00B7 ";
    meta += TunerTuningName(g_tuner.tuning);
    meta += L" \x00B7 ";
    if (heard) {
        // One decimal is as fine as the reading deserves.
        const int tenths = static_cast<int>(lround(g_tuner.frequency * 10.0f));
        meta += std::to_wstring(tenths / 10);
        meta += L".";
        meta += std::to_wstring(tenths % 10);
        meta += L" Hz";
    } else {
        meta += L"A4 = ";
        meta += std::to_wstring(g_tuner.reference);
        meta += L" Hz";
    }
    if (strings > 0 && settled < strings) {
        meta += L" \x00B7 ";
        meta += std::to_wstring(settled);
        meta += L" of ";
        meta += std::to_wstring(strings);
        meta += L" in tune";
    }
    g_text.Draw(s, metaFont, meta, g_layout.tunerMeta,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);

    // Each string of the tuning, and every one of them can be picked by hand
    // to tune on demand. The one being heard lights up, the picked one wears
    // an accent ring and a target marker, and one that has settled in tune
    // keeps a check badge over its corner.
    HFONT chipFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    const int nearest = heard && g_tuner.lock < 0 ? TunerNearestString(midi) : -1;
    const double now = NowMs();
    for (int i = 0; i < strings; i++) {
        const RECT& chip = g_layout.tunerStringChip[i];
        const bool locked = i == g_tuner.lock;
        const bool active = locked || i == nearest;
        const bool hovered =
            g_hover.id == kHitTunerString && g_hover.index == i;
        const bool tuned = g_tuner.tuned[i];
        // Both marks know the moment they arrived, so they can play out
        // instead of simply appearing.
        const float born =
            g_tuner.stringAt[i] > 0.0
                ? Clamp01(static_cast<float>(now - g_tuner.stringAt[i]) /
                          static_cast<float>(kTunedBadgeMs))
                : 1.0f;
        const float held =
            locked ? Clamp01(static_cast<float>(now - g_tuner.lockAt) /
                             static_cast<float>(kTunedBadgeMs))
                   : 0.0f;

        PaintChip(s, t, chip,
                  NoteLabel(kTunings[g_tuner.tuning].notes[i]).c_str(),
                  chipFont, active, hovered);
        if (tuned && !active) {
            // A settled string keeps a wash of the accent, so at a glance the
            // player can see which strings are still to do.
            FillRoundRect(s, L(chip.left), L(chip.top), L(chip.right),
                          L(chip.bottom), S(9.0f), WithAlpha(t.accent, 0.13f));
        }
        if (locked) {
            // A ring closes on the chip that was picked by hand.
            StrokeRoundRect(s, L(chip.left) + 0.5f, L(chip.top) + 0.5f,
                            L(chip.right) - 0.5f, L(chip.bottom) - 0.5f,
                            S(9.0f), S(1.5f),
                            WithAlpha(t.accent, 0.55f + 0.45f * held));
            const float dot = S(2.2f) * (0.6f + 0.4f * held);
            FillCircle(s, L(chip.left) + S(7.0f), L(chip.top) + S(7.0f), dot,
                       WithAlpha(active ? t.onAccent : t.accent, held));
        }
        if (tuned) {
            // The badge overshoots a touch and settles, which pulls the eye
            // to the string that just came into tune.
            const float pop = EaseOutBack(born);
            const float badgeX = L(chip.right) - S(7.5f);
            const float badgeY = L(chip.top) + S(7.0f);
            const float badgeR = S(5.6f) * pop;
            if (badgeR > 0.4f) {
                const Color fill = active ? kWhite : t.accent;
                FillCircle(s, badgeX, badgeY, badgeR + S(1.4f),
                           WithAlpha(fill, 0.22f * born));
                FillCircle(s, badgeX, badgeY, badgeR,
                           WithAlpha(fill, 0.95f * born));
                const Color mark =
                    WithAlpha(active ? t.accent : t.onAccent, born);
                const float reach = badgeR * 0.42f;
                StrokeLine(s, {badgeX - reach, badgeY + reach * 0.1f},
                           {badgeX - reach * 0.15f, badgeY + reach * 0.85f},
                           S(1.3f), mark);
                StrokeLine(s, {badgeX - reach * 0.15f, badgeY + reach * 0.85f},
                           {badgeX + reach, badgeY - reach * 0.75f}, S(1.3f),
                           mark);
            }
        }
    }

    // The instrument and the tuning, each a labelled select that opens a
    // listbox over the card.
    HFONT rowFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    HFONT captionFont = g_fonts.Get(S(9.5f), FW_NORMAL);
    auto Select = [&](const RECT& row,
                      const RECT& caption,
                      PCWSTR captionText,
                      PCWSTR label,
                      bool open,
                      int id) {
        g_text.Draw(s, captionFont, captionText, caption,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE,
                    open ? t.accent : t.textFaint);
        const bool hovered = g_hover.id == id;
        FillRoundRect(s, L(row.left), L(row.top), L(row.right), L(row.bottom),
                      S(8.0f), open || hovered ? t.controlHover : t.control);
        StrokeRoundRect(s, L(row.left) + 0.5f, L(row.top) + 0.5f,
                        L(row.right) - 0.5f, L(row.bottom) - 0.5f, S(8.0f),
                        S(1.0f), open ? WithAlpha(t.accent, 0.55f) : t.stroke);
        RECT text = row;
        text.left += S(10);
        text.right -= S(24);
        g_text.Draw(s, rowFont, label, text,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS,
                    t.text);
        // The chevron turns when the list is up.
        const float flip = open ? -1.0f : 1.0f;
        const float cy = L(row.top + row.bottom) * 0.5f + S(0.6f) * flip;
        const float size = S(3.4f);
        const Vec2 a{L(row.right) - S(12) - size, cy + size * 0.5f * flip};
        const Vec2 b{L(row.right) - S(12), cy - size * 0.55f * flip};
        const Vec2 c{L(row.right) - S(12) + size, cy + size * 0.5f * flip};
        StrokeLine(s, a, b, S(1.6f), open ? t.accent : t.textMuted);
        StrokeLine(s, b, c, S(1.6f), open ? t.accent : t.textMuted);
    };
    const bool instOpen =
        MenuHit(g_tunerMenu) && g_tunerMenu.shown == kMenuInstrument;
    const bool tuneOpen =
        MenuHit(g_tunerMenu) && g_tunerMenu.shown == kMenuTuning;
    Select(g_layout.tunerInstSelect, g_layout.tunerInstCaption, L"Instrument",
           TunerInstrumentName(g_tuner.instrument), instOpen, kHitTunerInst);
    Select(g_layout.tunerTuneSelect, g_layout.tunerTuneCaption, L"Tuning",
           TunerTuningName(g_tuner.tuning), tuneOpen, kHitTunerTune);

    std::wstring hint;
    if (g_tuner.lock >= 0) {
        hint = L"Tuning ";
        hint += NoteLabel(kTunings[g_tuner.tuning].notes[g_tuner.lock]);
        hint += L" \x00B7 click the chip again to release";
    } else {
        hint = L"Play a string \x00B7 click a chip or press Enter to hold just that one";
    }
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                hint, g_layout.tunerHint,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);
}

// The list an instrument or tuning select opens, drawn last so it sits over
// the panel. It shares the animation and the rows of the tool list, so all
// three read as the same control.
void PaintTunerMenu(Surface& s, const Theme& t) {
    const bool inst = g_tunerMenu.shown == kMenuInstrument;
    const RECT& full = inst ? g_layout.tunerInstMenu : g_layout.tunerTuneMenu;
    const RECT reveal = PaintListShell(s, t, full, g_tunerMenu.progress);

    const int count =
        inst ? kTunerInstrumentCount : TunerTuningCount(g_tuner.instrument);
    ListItem items[kTunerMenuMax];
    const RECT* rows =
        inst ? g_layout.tunerInstMenuRow : g_layout.tunerTuneMenuRow;
    for (int i = 0; i < count; i++) {
        const int value = inst ? i : TunerTuningAt(g_tuner.instrument, i);
        items[i].label = inst ? kTunerInstruments[i] : kTunings[value].name;
        items[i].active =
            inst ? g_tuner.instrument == i : g_tuner.tuning == value;
        items[i].hovered =
            (MenuHit(g_tunerMenu) && g_hover.id == kHitTunerMenuRow &&
             g_hover.index == i) ||
            g_tunerMenu.cursor == i;  // The keyboard cursor rides with the list.
    }
    PaintListRows(s, t, items, count, rows, reveal);
}

struct FooterItem {
    std::wstring key;
    std::wstring action;
};

// A point on the wheel: the middle of a wedge at a given radius.
void CircleRingPoint(int i, float radius, float& x, float& y) {
    // Twelve o'clock, then clockwise a fifth at a time (30 degrees a step).
    const float angle = -1.5707963f + static_cast<float>(i) * 0.5235988f;
    x = L(g_layout.circleCx) + radius * cosf(angle);
    y = L(g_layout.circleCy) + radius * sinf(angle);
}

// One wedge of the wheel: the band between two radii, cut by two rays. Each ray
// is carried in as its inward normal, so a pixel's distance to the cut is a
// dot product and the fill needs no angle of its own.
void FillCircleSector(Surface& s,
                      float cx,
                      float cy,
                      float rInner,
                      float rOuter,
                      float a0,
                      float a1,
                      Color c) {
    // The box is walked round the wedge - six steps along each radius - so the
    // fill only visits the pixels the wedge can reach. A whole-circle box would
    // cost twenty-four times more for the same picture.
    float minX = cx;
    float maxX = cx;
    float minY = cy;
    float maxY = cy;
    for (int k = 0; k <= 6; k++) {
        const float a = a0 + (a1 - a0) * static_cast<float>(k) / 6.0f;
        for (int edge = 0; edge < 2; edge++) {
            const float r = edge == 0 ? rInner : rOuter;
            const float px = cx + r * cosf(a);
            const float py = cy + r * sinf(a);
            minX = std::min(minX, px);
            maxX = std::max(maxX, px);
            minY = std::min(minY, py);
            maxY = std::max(maxY, py);
        }
    }
    const float n0x = -sinf(a0);
    const float n0y = cosf(a0);
    const float n1x = sinf(a1);
    const float n1y = -cosf(a1);
    FillShape(
        s, minX - 1, minY - 1, maxX + 1, maxY + 1,
        [&](float x, float y) {
            const float dx = x - cx;
            const float dy = y - cy;
            const float r = sqrtf(dx * dx + dy * dy);
            const float cut =
                std::min(dx * n0x + dy * n0y, dx * n1x + dy * n1y);
            return -std::min(std::min(r - rInner, rOuter - r), cut);
        },
        [&](float, float) { return c; });
}

// The circle of fifths card: a wheel of two rings - the twelve majors outside,
// their relative minors inside - round a centre readout, the instrument chips
// above it, and the seven diatonic chords of the selected key with the theory
// lines that spell it out below. The instrument only picks the voice its scales
// and chords are played in.
void PaintCircle(Surface& s, const Theme& t) {
    const RECT& card = g_layout.circlePad;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    const int key = g_circle.key;
    const float cx = L(g_layout.circleCx);
    const float cy = L(g_layout.circleCy);
    const float outerR = L(g_layout.circleRingR);
    const float midR = L(g_layout.circleMidR);
    const float coreR = L(g_layout.circleCoreR);
    // Every wedge is pulled in from its own slice of the wheel, so the card
    // shows between the twelve rooms instead of one unbroken ring.
    const float bandGap = S(3.0f);
    const float wedgeHalf = 0.2617994f - 0.011f;  // 15 degrees, less a hair

    HFONT chipFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    for (int i = 0; i < kInstrumentChipCount; i++) {
        const int inst = kInstrumentChipOrder[i];
        PaintChip(s, t, g_layout.circleInst[i], kInstrumentChipLabels[inst],
                  chipFont, g_circle.instrument == inst,
                  g_hover.id == kHitCircleInst && g_hover.index == i);
    }

    // Twenty-four wedges: the majors outside, and inside them the minor key
    // that shares their key signature. The selected one fills with the accent
    // and eases in from the last click.
    const double since = NowMs() - g_circle.flashAt;
    const float pulse = static_cast<float>(Clamp01(1.0 - since / 260.0));
    HFONT nameFont = g_fonts.Get(S(12.0f), kWeightSemiBold);
    HFONT innerFont = g_fonts.Get(S(11.0f), kWeightSemiBold);
    HFONT markFont = g_fonts.Get(S(9.0f), FW_NORMAL);
    for (int i = 0; i < 12; i++) {
        for (int ring = 0; ring < 2; ring++) {
            const bool minorRing = ring == 1;
            const float rIn = minorRing ? coreR : midR + bandGap;
            const float rOut = minorRing ? midR - bandGap : outerR;
            const bool selected = i == key && g_circle.minor == minorRing;
            const bool hovered =
                g_hover.id == kHitCircleKey &&
                g_hover.index == i + (minorRing ? kCircleRingMinor : 0);
            Color fill = hovered ? t.controlHover : t.control;
            if (selected) {
                fill = WithAlpha(t.accent, 0.76f + 0.2f * pulse);
            }
            const float mid = -1.5707963f + static_cast<float>(i) * 0.5235988f;
            FillCircleSector(s, cx, cy, rIn, rOut, mid - wedgeHalf,
                             mid + wedgeHalf, fill);

            // The name sits above the middle of the band with the numeral it
            // earns below it; a chord outside the key leaves that line empty.
            const float bandMid = (rIn + rOut) * 0.5f;
            float px;
            float py;
            CircleRingPoint(i, bandMid, px, py);
            const float halfW = bandMid * sinf(wedgeHalf) * 0.94f;
            RECT nameBox{static_cast<LONG>(px - halfW),
                         static_cast<LONG>(py - S(16.0f)),
                         static_cast<LONG>(px + halfW),
                         static_cast<LONG>(py + S(2.0f))};
            RECT markBox{nameBox.left, static_cast<LONG>(py + S(2.0f)),
                         nameBox.right, static_cast<LONG>(py + S(16.0f))};
            const std::wstring label =
                minorRing ? CircleMinorRingName(i)
                          : std::wstring(kCircleKeys[i].name);
            g_text.Draw(s, minorRing ? innerFont : nameFont, label, nameBox,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                        selected ? t.onAccent : t.text);
            // A wedge stands for the major triad on its key, or the minor one
            // on that key's relative minor; the numeral follows from how far
            // that root is from the tonic of the key being read.
            const int rootPc =
                minorRing ? (kCircleKeys[i].pc + 9) % 12 : kCircleKeys[i].pc;
            const int semitones = (rootPc - CircleTonicPc() + 12) % 12;
            const PCWSTR mark = CircleWheelNumeral(semitones, minorRing);
            if (mark) {
                g_text.Draw(s, markFont, mark, markBox,
                            DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                            selected ? t.onAccent : t.accent);
            }
        }
    }

    // The tonic in the middle of the hole, with its scale spelled beneath it.
    // A minor key reads on the natural minor here, not on the major scale of
    // the same letter.
    HFONT bigFont = g_fonts.Get(S(28.0f), kWeightSemiBold);
    HFONT smallFont = g_fonts.Get(S(static_cast<float>(kFontTone)), FW_NORMAL);
    g_text.Draw(s, bigFont, CircleTonicName(), g_layout.circleCenter,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.text);
    std::wstring scale;
    for (int d = 0; d < 7; d++) {
        if (d) {
            scale += L" ";
        }
        scale += CircleScaleName(d);
    }
    g_text.Draw(s, g_fonts.Get(S(10.0f), kWeightSemiBold), scale,
                g_layout.circleCaption,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.accent);

    // The seven diatonic chords, each clickable to hear that triad.
    for (int d = 0; d < 7; d++) {
        const std::wstring label = CircleScaleChordName(d);
        PaintChip(s, t, g_layout.circleChord[d], label.c_str(), chipFont,
                  d == 0, g_hover.id == kHitCircleChord && g_hover.index == d);
    }

    // Key signature line. A relative minor carries the signature of the major
    // on its spoke, so the fifths count is the same either way.
    const int fifths = kCircleKeys[key].fifths;
    std::wstring signature;
    if (fifths == 0) {
        signature = L"No sharps or flats";
    } else {
        signature = fifths > 0 ? L"Sharps: " : L"Flats: ";
        signature += CircleSignatureList(fifths);
    }
    g_text.Draw(s, smallFont, signature, g_layout.circleSignature,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.text);

    // The key this one is bound to: a major reads on its relative minor, a
    // minor on its relative major, and each with the parallel key after it.
    std::wstring relatives = g_circle.minor ? L"Relative major "
                                            : L"Relative minor ";
    relatives += g_circle.minor ? std::wstring(kCircleKeys[key].name)
                                : CircleScaleName(5) + L"m";
    relatives += g_circle.minor ? L"  \x00B7  Parallel major "
                                : L"  \x00B7  Parallel minor ";
    if (g_circle.minor) {
        relatives += CircleTonicNote();
    } else {
        relatives += kCircleKeys[key].name;
        relatives += L"m";
    }
    g_text.Draw(s, smallFont, relatives, g_layout.circleMinor,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textMuted);

    // The mode built on each degree of the selected scale.
    std::wstring modes;
    for (int d = 0; d < 7; d++) {
        if (d) {
            modes += L" ";
        }
        modes += CircleModeName((d + (g_circle.minor ? 5 : 0)) % 7);
    }
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                modes, g_layout.circleDetail,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textFaint);

    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                L"Click a key to hear its scale \x00B7 the numerals mark the "
                L"chords of this key",
                g_layout.circleHint, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                t.textFaint);
}

// The note trainer card: three instrument chips, a naturals toggle and a Start
// button across the top, the big target note in the middle, the note the
// microphone hears and a draining countdown bar, then the running score.
void PaintTrainer(Surface& s, const Theme& t) {
    const RECT& card = g_layout.trainerPad;
    FillRoundRect(s, L(card.left), L(card.top), L(card.right), L(card.bottom),
                  S(kCardRadius), t.card);
    StrokeRoundRect(s, L(card.left) + 0.5f, L(card.top) + 0.5f,
                    L(card.right) - 0.5f, L(card.bottom) - 0.5f, S(kCardRadius),
                    S(1.0f), t.stroke);

    HFONT chipFont =
        g_fonts.Get(S(static_cast<float>(kFontTab)), kWeightSemiBold);
    for (int i = 0; i < kInstrumentChipCount; i++) {
        const int inst = kInstrumentChipOrder[i];
        PaintChip(s, t, g_layout.trainerInst[i], kInstrumentChipLabels[inst],
                  chipFont, g_trainer.instrument == inst,
                  g_hover.id == kHitTrainerInst && g_hover.index == i);
    }
    PaintChip(s, t, g_layout.trainerNaturals, L"Naturals", chipFont,
              g_trainer.naturals, g_hover.id == kHitTrainerNaturals);
    const bool running = g_trainer.running;
    PaintChip(s, t, g_layout.trainerStart, running ? L"Stop" : L"Start",
              chipFont, running, g_hover.id == kHitTrainerStart);

    // The big target note. Idle waits, a live round shows the note to find.
    HFONT bigFont =
        g_fonts.Get(S(static_cast<float>(kFontBpm)), kWeightSemiBold);
    std::wstring prompt;
    Color promptColor = t.textFaint;
    if (running && g_trainer.target >= 0) {
        prompt = NoteLabel(g_trainer.target);
        promptColor = t.text;
    } else if (running) {
        prompt = L"...";
    } else {
        prompt = L"Ready";
    }
    g_text.Draw(s, bigFont, prompt, g_layout.trainerPrompt,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, promptColor);

    // The note the microphone is hearing right now.
    std::wstring played;
    Color playedColor = t.textMuted;
    if (g_trainer.micFailed) {
        played = L"no microphone \x00B7 check Settings \x203A Privacy";
        playedColor = t.textFaint;
    } else if (!running) {
        played = L"press Start, then play the note";
    } else {
        played = L"you played: ";
        played += NoteLabel(g_trainer.heard);
        playedColor = g_trainer.heard == g_trainer.target ? t.accent : t.text;
    }
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                played, g_layout.trainerPlayed,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, playedColor);

    // The live countdown, draining right to left over the seconds limit.
    const RECT& bar = g_layout.trainerBar;
    FillRoundRect(s, L(bar.left), L(bar.top), L(bar.right), L(bar.bottom),
                  S(6.0f), t.control);
    if (running && g_trainer.roundMs > 0.0) {
        const float remain = Clamp01(static_cast<float>(
            (g_trainer.noteEndsAt - NowMs()) / g_trainer.roundMs));
        const float fill =
            static_cast<float>(L(bar.right) - L(bar.left)) * remain;
        if (fill > S(2.0f)) {
            FillRoundRect(s, L(bar.left), L(bar.top), L(bar.left) + fill,
                          L(bar.bottom), S(6.0f),
                          remain > 0.34f ? t.accent : t.textMuted);
        }
    }

    // The result flash, easing out over the note just hit or missed.
    const double since = NowMs() - g_trainer.resultAt;
    if (g_trainer.result != 0 && since < kTrainerResultFlashMs) {
        const float fade = Clamp01(static_cast<float>(
            1.0 - since / kTrainerResultFlashMs));
        std::wstring feedback;
        Color color;
        if (g_trainer.result == 1) {
            feedback = L"Correct  +";
            feedback += std::to_wstring(g_trainer.lastPoints);
            color = t.accent;
        } else {
            feedback = L"Miss \x00B7 it was ";
            feedback += NoteLabel(g_trainer.target);
            color = t.textMuted;
        }
        g_text.Draw(
            s, g_fonts.Get(S(static_cast<float>(kFontTone)), kWeightSemiBold),
            feedback, g_layout.trainerFeedback,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE,
            WithAlpha(color, 0.35f + 0.65f * fade));
    }

    // Score on the left, streak and accuracy on the right.
    std::wstring score = L"Score ";
    score += std::to_wstring(g_trainer.score);
    g_text.Draw(s,
                g_fonts.Get(S(static_cast<float>(kFontTone)), kWeightSemiBold),
                score, g_layout.trainerScore,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.text);
    const int accuracy =
        g_trainer.total > 0
            ? static_cast<int>(lround(100.0 * g_trainer.correct /
                                      static_cast<double>(g_trainer.total)))
            : 0;
    std::wstring streak = L"Streak ";
    streak += std::to_wstring(g_trainer.streak);
    streak += L"  \x00B7  Best ";
    streak += std::to_wstring(g_trainer.bestStreak);
    streak += L"  \x00B7  ";
    streak += std::to_wstring(accuracy);
    streak += L"%";
    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                streak, g_layout.trainerStreak,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, t.textMuted);

    g_text.Draw(s, g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL),
                L"Play the shown note \x00B7 the mic listens \x00B7 faster reads score more",
                g_layout.trainerHint, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                t.textFaint);
}

// One Fluent keycap: the shortcut key in a small rounded box, the way the shell
// draws shortcut hints in its own tips.
void PaintKeycap(Surface& s,
                 const Theme& t,
                 HFONT font,
                 const std::wstring& key,
                 const RECT& box) {
    FillRoundRect(s, L(box.left), L(box.top), L(box.right), L(box.bottom),
                  S(4.0f), t.control);
    StrokeRoundRect(s, L(box.left) + 0.5f, L(box.top) + 0.5f,
                    L(box.right) - 0.5f, L(box.bottom) - 0.5f, S(4.0f),
                    S(1.0f), t.stroke);
    g_text.Draw(s, font, key, box, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                t.textMuted);
}

void PaintFooter(Surface& s, const Theme& t) {
    RECT rcFooter = g_layout.foot;

    RECT rcDivider = rcFooter;
    rcDivider.bottom = rcDivider.top + 1;
    FillRoundRectGradient(s, 
        static_cast<float>(rcDivider.left), static_cast<float>(rcDivider.top), 
        static_cast<float>(rcDivider.right), static_cast<float>(rcDivider.bottom), 
        0.0f, t.stroke, t.stroke);

    rcFooter.top += 1;

    std::vector<FooterItem> items;
    if (g_app.tool == kToolChords) {
        items = {
            {L"\x2191\x2193", L"browse"},
            {L"Tab", L"instrument"},
            {L"\x2190\x2192", L"fingering"},
            {L"Esc", L"close"}
        };
    } else if (g_app.tool == kToolTempo) {
        items = {
            {TapKeyLabel(), L"tap"},
            {L"\x2190\x2192", L"half / double"},
            {L"R", L"reset"},
            {L"Esc", L"close"}
        };
    } else if (g_app.tool == kToolMetronome) {
        std::wstring spaceAction = g_metro.running ? L"stop" : L"start";
        items = {
            {L"Space", spaceAction},
            {L"\x2191\x2193", L"tempo"},
            {L"\x2190\x2192", L"meter"},
            {L"Esc", L"close"}
        };
    } else if (g_app.tool == kToolTuner) {
        items = {
            {L"\x2191\x2193", L"instrument"},
            {L"\x2190\x2192", L"tuning"},
            {L"1\x2013\x38", L"one string"},
            {L"Esc", L"close"}
        };
    } else if (g_app.tool == kToolStrum) {
        if (MenuHit(g_meterMenu)) {
            // The arrows browse the meter list while it is up.
            items = {
                {L"\x2191\x2193", L"meter"},
                {L"Enter", L"pick"},
                {L"Esc", L"close"}
            };
        } else if (MenuHit(g_patternMenu)) {
            // The arrows browse the pattern list while it is up.
            items = {
                {L"\x2191\x2193", L"pattern"},
                {L"Enter", L"pick"},
                {L"Esc", L"close"}
            };
        } else if (MenuHit(g_strumMenu)) {
            // The arrows browse the chord list while it is up.
            items = {
                {L"\x2191\x2193", L"chord"},
                {L"Enter", L"pick"},
                {L"Esc", L"close"}
            };
        } else {
            std::wstring spaceAction = g_strum.running ? L"stop" : L"play";
            items = {
                {L"Space", spaceAction},
                {L"\x2190\x2192", L"select cell"},
                {L"D U X M R", L"stroke"},
                {L"Esc", L"close"}
            };
        }
    } else if (g_app.tool == kToolCircle) {
        items = {
            {L"Space", L"play scale"},
            {L"\x2190\x2192", L"step keys"},
            {L"1\x2013\x37", L"chord"},
            {L"Esc", L"close"}
        };
    } else {
        std::wstring spaceAction = g_trainer.running ? L"stop" : L"start";
        items = {
            {L"Space", spaceAction},
            {L"R", L"reset score"},
            {L"Tab", L"instrument"},
            {L"Esc", L"close"}
        };
    }

    HFONT keyFont =
        g_fonts.Get(S(static_cast<float>(kFontFoot)), kWeightSemiBold);
    HFONT actionFont =
        g_fonts.Get(S(static_cast<float>(kFontFoot)), FW_NORMAL);
    const int capPadX = S(5);
    const int capGap = S(5);

    // The keys are measured first, so the caps can be spread across the footer
    // with equal air between the pairs instead of running off the edge.
    const int count = static_cast<int>(items.size());
    std::vector<int> caps(static_cast<size_t>(count));
    std::vector<int> actions(static_cast<size_t>(count));
    int total = 0;
    for (int i = 0; i < count; i++) {
        caps[i] = g_text.Measure(keyFont, items[i].key).cx + capPadX * 2;
        actions[i] = g_text.Measure(actionFont, items[i].action).cx;
        total += caps[i] + capGap + actions[i];
    }
    const int available = static_cast<int>(rcFooter.right - rcFooter.left);
    int space = S(12);
    if (count > 1) {
        space = std::clamp((available - total) / (count - 1), S(6), S(14));
        total += space * (count - 1);
    }

    const int rowHeight = static_cast<int>(rcFooter.bottom - rcFooter.top);
    const int capHeight = std::min(S(15.0f), rowHeight);
    const int capTop = rcFooter.top + (rowHeight - capHeight) / 2;
    int x = rcFooter.left + std::max(0, (available - total) / 2);
    for (int i = 0; i < count; i++) {
        const RECT cap{x, capTop, x + caps[i], capTop + capHeight};
        PaintKeycap(s, t, keyFont, items[i].key, cap);
        // The label gets a couple of pixels of slack so hinting never clips the
        // last column of the word.
        const RECT label{x + caps[i] + capGap, rcFooter.top,
                         x + caps[i] + capGap + actions[i] + S(4),
                         rcFooter.bottom};
        g_text.Draw(s, actionFont, items[i].action, label,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE, t.textFaint);
        x += caps[i] + capGap + actions[i] + space;
    }
}

void PaintPanel(Surface& s) {
    const Theme& t = g_theme;
    const float width = L(g_layout.width);
    const float height = L(g_layout.height);
    const float radius = S(kPanelRadius);
    const bool acrylic = TransparencyEnabled() && IsWindows10Build(17134);
    if (acrylic) {
        FillRoundRectGradient(s, 0, 0, width, height, radius, t.panelTop,
                              t.panelBottom);
    } else {
        FillRoundRectGradient(s, 0, 0, width, height, radius,
                              WithAlpha(t.panelTop, 0.985f),
                              WithAlpha(t.panelBottom, 0.985f));
    }
    StrokeRoundRect(s, 0.5f, 0.5f, width - 0.5f, height - 0.5f, radius - 0.5f,
                    S(1.0f), t.stroke);

    PaintToolSelect(s, t);
    PaintCloseButton(s, t);
    if (g_app.tool == kToolChords) {
        PaintTabs(s, t);
        PaintSearch(s, t);
        PaintResults(s, t);
        PaintCard(s, t);
    } else if (g_app.tool == kToolTempo) {
        PaintTempo(s, t);
    } else if (g_app.tool == kToolMetronome) {
        PaintMetronome(s, t);
    } else if (g_app.tool == kToolTuner) {
        PaintTuner(s, t);
    } else if (g_app.tool == kToolStrum) {
        PaintStrum(s, t);
    } else if (g_app.tool == kToolCircle) {
        PaintCircle(s, t);
    } else {
        PaintTrainer(s, t);
    }
    PaintFooter(s, t);
    if (MenuVisual(g_toolMenu)) {
        PaintToolMenu(s, t);  // Last, so the list sits over the panel.
    }
    if (g_app.tool == kToolTuner && MenuVisual(g_tunerMenu)) {
        PaintTunerMenu(s, t);  // Over the panel, same as the tool list.
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_strumMenu)) {
        PaintStrumMenu(s, t);
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_patternMenu)) {
        PaintStrumPatternMenu(s, t);
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_meterMenu)) {
        PaintStrumMeterMenu(s, t);  // Last, so it sits over the meter select.
    }
}

float CurrentScale() {
    if (g_panel.Hwnd()) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        static auto getDpi =
            GetProc<GetDpiForWindow_t>(L"user32.dll", "GetDpiForWindow");
        if (getDpi) {
            const UINT dpi = getDpi(g_panel.Hwnd());
            if (dpi >= 96) {
                return static_cast<float>(dpi) / 96.0f;
            }
        }
    }
    using GetDpiForSystem_t = UINT(WINAPI*)();
    static auto getSystemDpi =
        GetProc<GetDpiForSystem_t>(L"user32.dll", "GetDpiForSystem");
    if (getSystemDpi) {
        const UINT dpi = getSystemDpi();
        if (dpi >= 96) {
            return static_cast<float>(dpi) / 96.0f;
        }
    }
    return 1.0f;
}

void Render() {
    g_scale = std::clamp(CurrentScale(), 1.0f, 3.0f);
    g_theme = MakeTheme(UseDarkTheme(), g_accent);
    ComputeLayout();
    EnsureSelectionVisible();
    if (!g_surface.Resize(g_layout.width, g_layout.height)) {
        return;
    }
    PaintPanel(g_surface);
}

////////////////////////////////////////////////////////////////////////////////
// Hit testing

HitResult HitTest(POINT pt) {
    HitResult hit;
    if (MenuVisual(g_toolMenu)) {
        // The open list owns the pointer until it closes.
        for (int i = 0; i < kToolCount; i++) {
            if (PtInRect(&g_layout.toolMenuRows[i], pt)) {
                hit.id = kHitToolMenuItem;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.toolSelect, pt)) {
            hit.id = kHitToolSelect;
        }
        return hit;
    }
    if (PtInRect(&g_layout.close, pt)) {
        hit.id = kHitClose;
        return hit;
    }
    if (PtInRect(&g_layout.toolSelect, pt)) {
        hit.id = kHitToolSelect;
        return hit;
    }
    if (g_app.tool == kToolTempo) {
        if (PtInRect(&g_layout.tapHalf, pt)) {
            hit.id = kHitTapHalf;
            return hit;
        }
        if (PtInRect(&g_layout.tapReset, pt)) {
            hit.id = kHitTapReset;
            return hit;
        }
        if (PtInRect(&g_layout.tapDouble, pt)) {
            hit.id = kHitTapDouble;
            return hit;
        }
        if (PtInRect(&g_layout.tapPad, pt)) {
            hit.id = kHitTapPad;
            return hit;
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    if (g_app.tool == kToolMetronome) {
        if (PtInRect(&g_layout.metroStart, pt)) {
            hit.id = kHitMetroStart;
            return hit;
        }
        if (PtInRect(&g_layout.metroTap, pt)) {
            hit.id = kHitMetroTap;
            return hit;
        }
        if (PtInRect(&g_layout.metroValue, pt)) {
            // The number itself is the tempo field; the band it sits in is
            // just the top of the card, so only its middle answers.
            RECT number = g_layout.metroValue;
            const int inset = (number.right - number.left) / 3;
            number.left += inset;
            number.right -= inset;
            if (PtInRect(&number, pt)) {
                hit.id = kHitMetroTempo;
                return hit;
            }
        }
        if (PtInRect(&g_layout.metroBeatsDown, pt)) {
            hit.id = kHitMetroBeatsDown;
            return hit;
        }
        if (PtInRect(&g_layout.metroBeatsUp, pt)) {
            hit.id = kHitMetroBeatsUp;
            return hit;
        }
        if (PtInRect(&g_layout.metroGroup, pt)) {
            hit.id = kHitMetroGroup;
            return hit;
        }
        for (int i = 0; i < kSignatureCount; i++) {
            if (PtInRect(&g_layout.metroPreset[i], pt)) {
                hit.id = kHitMetroPreset;
                hit.index = i;
                return hit;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (PtInRect(&g_layout.metroUnit[i], pt)) {
                hit.id = kHitMetroUnit;
                hit.index = i;
                return hit;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (PtInRect(&g_layout.metroSub[i], pt)) {
                hit.id = kHitMetroSubdivision;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    if (g_app.tool == kToolTuner) {
        // An open list owns the pointer until it closes.
        if (MenuVisual(g_tunerMenu)) {
            const bool instrumentList =
                g_tunerMenu.shown == kMenuInstrument;
            const int count = instrumentList
                                  ? kTunerInstrumentCount
                                  : TunerTuningCount(g_tuner.instrument);
            for (int i = 0; i < count; i++) {
                const RECT& row = instrumentList
                    ? g_layout.tunerInstMenuRow[i]
                    : g_layout.tunerTuneMenuRow[i];
                if (PtInRect(&row, pt)) {
                    hit.id = kHitTunerMenuRow;
                    hit.index = i;
                    return hit;
                }
            }
            if (PtInRect(&g_layout.tunerInstSelect, pt)) {
                hit.id = kHitTunerInst;
            } else if (PtInRect(&g_layout.tunerTuneSelect, pt)) {
                hit.id = kHitTunerTune;
            }
            return hit;
        }
        for (int i = 0; i < kTunerStringMax; i++) {
            if (PtInRect(&g_layout.tunerStringChip[i], pt)) {
                hit.id = kHitTunerString;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.tunerInstSelect, pt)) {
            hit.id = kHitTunerInst;
            return hit;
        }
        if (PtInRect(&g_layout.tunerTuneSelect, pt)) {
            hit.id = kHitTunerTune;
            return hit;
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    if (g_app.tool == kToolStrum) {
        // An open list owns the pointer until it closes, and the pattern list
        // is the one drawn on top, so it answers first.
        if (MenuVisual(g_patternMenu)) {
            // Every line answers, headings included: a click on one has to be
            // swallowed by the list rather than fall through and close it.
            StrumEntry entries[kStrumPatternEntriesMax];
            const int rows = StrumPatternEntries(
                entries, kStrumPatternEntriesMax, g_strum.beats, g_strum.unit);
            for (int i = 0; i < rows; i++) {
                if (PtInRect(&g_layout.strumPatternRow[i], pt)) {
                    hit.id = kHitStrumPatternRow;
                    hit.index = i;
                    return hit;
                }
            }
            if (PtInRect(&g_layout.strumPattern, pt)) {
                hit.id = kHitStrumPattern;
            }
            return hit;
        }
        if (MenuVisual(g_strumMenu)) {
            for (int i = 0; i < kStrumChordCount; i++) {
                if (PtInRect(&g_layout.strumChordRow[i], pt)) {
                    hit.id = kHitStrumChordRow;
                    hit.index = i;
                    return hit;
                }
            }
            if (PtInRect(&g_layout.strumChord, pt)) {
                hit.id = kHitStrumChord;
            }
            return hit;
        }
        if (MenuVisual(g_meterMenu)) {
            for (int i = 0; i < kSignatureCount; i++) {
                if (PtInRect(&g_layout.strumMeterRow[i], pt)) {
                    hit.id = kHitStrumMeterRow;
                    hit.index = i;
                    return hit;
                }
            }
            if (PtInRect(&g_layout.strumMeterSelect, pt)) {
                hit.id = kHitStrumMeter;
            }
            return hit;
        }
        if (PtInRect(&g_layout.strumPattern, pt)) {
            hit.id = kHitStrumPattern;
            return hit;
        }
        const int cells = std::clamp(StrumSlotCount(), 1, kStrumSlotsMax);
        for (int i = 0; i < cells; i++) {
            if (PtInRect(&g_layout.strumSlot[i], pt)) {
                hit.id = kHitStrumSlot;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.strumMeterSelect, pt)) {
            hit.id = kHitStrumMeter;
            return hit;
        }
        const int gridLimit = StrumGridLimit(g_strum.beats);
        for (int i = 0; i < gridLimit; i++) {
            if (PtInRect(&g_layout.strumSub[i], pt)) {
                hit.id = kHitStrumSub;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.strumTempo, pt)) {
            hit.id = kHitStrumTempo;
            return hit;
        }
        if (PtInRect(&g_layout.strumTempoDown, pt)) {
            hit.id = kHitStrumTempoDown;
            return hit;
        }
        if (PtInRect(&g_layout.strumTempoUp, pt)) {
            hit.id = kHitStrumTempoUp;
            return hit;
        }
        if (PtInRect(&g_layout.strumPlay, pt)) {
            hit.id = kHitStrumPlay;
            return hit;
        }
        for (int i = 0; i < kStrumChipCount; i++) {
            if (PtInRect(&g_layout.strumChip[i], pt)) {
                hit.id = kHitStrumChip;
                hit.index = i;
                return hit;
            }
        }
        for (int i = 0; i < kStrumSoundCount; i++) {
            if (PtInRect(&g_layout.strumSound[i], pt)) {
                hit.id = kHitStrumSound;
                hit.index = i;
                return hit;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (PtInRect(&g_layout.strumInst[i], pt)) {
                hit.id = kHitStrumInst;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.strumChord, pt)) {
            hit.id = kHitStrumChord;
            return hit;
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    if (g_app.tool == kToolCircle) {
        for (int i = 0; i < kInstrumentChipCount; i++) {
            if (PtInRect(&g_layout.circleInst[i], pt)) {
                hit.id = kHitCircleInst;
                hit.index = i;
                return hit;
            }
        }
        for (int i = 0; i < 7; i++) {
            if (PtInRect(&g_layout.circleChord[i], pt)) {
                hit.id = kHitCircleChord;
                hit.index = i;
                return hit;
            }
        }
        // The wheel answers anywhere in its two bands: the angle picks the
        // spoke and the radius picks the ring, so the index carries both.
        const float dx = static_cast<float>(pt.x) - L(g_layout.circleCx);
        const float dy = static_cast<float>(pt.y) - L(g_layout.circleCy);
        const float r = sqrtf(dx * dx + dy * dy);
        if (r <= L(g_layout.circleRingR) && r >= L(g_layout.circleCoreR)) {
            float angle = atan2f(dy, dx) + 1.5707963f;  // 0 at twelve o'clock
            if (angle < 0.0f) {
                angle += 6.2831853f;
            }
            const int spoke =
                static_cast<int>(floorf(angle / 0.5235988f + 0.5f)) % 12;
            hit.id = kHitCircleKey;
            hit.index = spoke + (r < L(g_layout.circleMidR) ? kCircleRingMinor
                                                            : 0);
            return hit;
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    if (g_app.tool == kToolTrainer) {
        for (int i = 0; i < kInstrumentChipCount; i++) {
            if (PtInRect(&g_layout.trainerInst[i], pt)) {
                hit.id = kHitTrainerInst;
                hit.index = i;
                return hit;
            }
        }
        if (PtInRect(&g_layout.trainerNaturals, pt)) {
            hit.id = kHitTrainerNaturals;
            return hit;
        }
        if (PtInRect(&g_layout.trainerStart, pt)) {
            hit.id = kHitTrainerStart;
            return hit;
        }
        if (PtInRect(&g_layout.header, pt)) {
            hit.id = kHitHeader;
        }
        return hit;
    }
    for (int i = 0; i < kInstrumentCount; i++) {
        if (PtInRect(&g_layout.tabs[i], pt)) {
            hit.id = kHitTab;
            hit.index = i;
            return hit;
        }
    }
    if (PtInRect(&g_layout.search, pt)) {
        hit.id = kHitSearch;
        return hit;
    }
    if (PtInRect(&g_layout.list, pt)) {
        const int rowHeight = std::max(1, g_layout.rowHeight);
        const int row = (pt.y - g_layout.list.top) / rowHeight;
        const int index = g_app.scroll + row;
        if (row >= 0 && row < kVisibleRows &&
            index < static_cast<int>(g_app.results.size())) {
            hit.id = kHitRow;
            hit.index = index;
        }
        return hit;
    }
    if (PtInRect(&g_layout.previous, pt)) {
        hit.id = kHitPrevious;
        return hit;
    }
    if (PtInRect(&g_layout.next, pt)) {
        hit.id = kHitNext;
        return hit;
    }
    for (int i = 0; i < g_layout.dotCount; i++) {
        if (PtInRect(&g_layout.dots[i], pt)) {
            hit.id = kHitDot;
            hit.index = i;
            return hit;
        }
    }
    if (PtInRect(&g_layout.speaker, pt)) {
        hit.id = kHitSpeaker;
        return hit;
    }
    if (PtInRect(&g_layout.diagram, pt)) {
        hit.id = kHitDiagram;
        return hit;
    }
    if (PtInRect(&g_layout.header, pt)) {
        hit.id = kHitHeader;
    }
    return hit;
}

////////////////////////////////////////////////////////////////////////////////
// Input

constexpr UINT kMessageSettings = WM_APP + 1;
constexpr int kHotkeyToggle = 1;
constexpr size_t kMaxQueryLength = 48;

HANDLE g_workerThread = nullptr;
HANDLE g_workerReadyEvent = nullptr;

void Repaint() {
    Render();
    PresentPanel(false);
}

////////////////////////////////////////////////////////////////////////////////
// Tap tempo

void TapTempoReset() {
    g_tap.intervalCount = 0;
    g_tap.lastTap = 0.0;
    g_tap.bpm = 0.0;
    g_tap.hasValue = false;
    Repaint();
}

void TapTempoSetBpm(double bpm) {
    g_tap.bpm = std::clamp(bpm, kMinBpm, kMaxBpm);
    g_tap.hasValue = true;
    // A manual change ends the measurement, so the next taps start from it.
    g_tap.intervalCount = 0;
    g_tap.lastTap = 0.0;
    MetronomeSync();
    Repaint();
}

void TapTempoTap() {
    const double now = NowMs();
    if (g_tap.lastTap > 0.0 && now - g_tap.lastTap <= g_settings.tapResetMs) {
        const double interval = now - g_tap.lastTap;
        // Repeats and double clicks would otherwise read as a wild tempo.
        if (interval >= kMinTapIntervalMs) {
            if (g_tap.intervalCount == kMaxTapIntervals) {
                for (int i = 1; i < kMaxTapIntervals; i++) {
                    g_tap.intervals[i - 1] = g_tap.intervals[i];
                }
                g_tap.intervalCount = kMaxTapIntervals - 1;
            }
            g_tap.intervals[g_tap.intervalCount++] = interval;
            g_tap.bpm =
                std::clamp(60000.0 / TapAverageInterval(), kMinBpm, kMaxBpm);
            g_tap.hasValue = true;
        }
    } else {
        g_tap.intervalCount = 0;  // First tap of a new measurement.
    }
    g_tap.lastTap = now;
    g_tap.flashAt = now;
    MetronomeSync();
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kFlashTimer, 16, nullptr);
    }
    Repaint();
}

void SetTool(int tool) {
    tool = std::clamp(tool, 0, kToolCount - 1);
    if (tool == static_cast<int>(g_app.tool)) {
        return;
    }
    MenuReset(g_toolMenu);
    MenuReset(g_tunerMenu);
    MenuReset(g_strumMenu);
    MenuReset(g_patternMenu);
    MenuReset(g_meterMenu);
    // The microphone only lives while the tuner is on screen.
    if (g_app.tool == kToolTuner) {
        TunerHalt();
    }
    // The trainer borrows that same microphone and runs its own round timer.
    if (g_app.tool == kToolTrainer) {
        TrainerHalt();
    }
    // And the pattern only plays while the strumming card is up.
    if (g_app.tool == kToolStrum) {
        StrumHalt();
    }
    // A tempo field belongs to the card it was opened on, so it goes with it.
    g_tempoEntry.active = false;
    g_tempoEntry.count = 0;
    g_app.tool = static_cast<Tool>(tool);
    if (g_selecting) {
        g_selecting = false;
        ReleaseCapture();
    }
    g_searchFocused = g_app.tool == kToolChords;
    g_hover = HitResult();
    g_pressed = HitResult();
    if (g_app.tool == kToolChords) {
        g_caret = static_cast<int>(g_app.query.size());
        g_selAnchor = g_caret;
        g_caretStart = NowMs();
    }
    if (g_app.tool == kToolTuner && g_phase == kPhaseOpen) {
        TunerStart();
    }
    if (g_app.tool == kToolTrainer && g_phase == kPhaseOpen) {
        TrainerEnter();
    }
    Repaint();
}

////////////////////////////////////////////////////////////////////////////////
// Metronome

void MetronomeSync() {
    g_metroShared.bpmMilli.store(
        static_cast<int>(lround(ToolkitBpm() * 1000.0)));
    g_metroShared.beats.store(std::clamp(g_metro.beats, 1, 16));
    g_metroShared.subdivision.store(std::clamp(g_metro.subdivision, 1, 4));
    g_metroShared.group.store(std::clamp(g_metro.group, 1, 4));
    g_metroShared.volume.store(std::clamp(g_settings.metronomeVolume, 0, 100));
}

// Silences the clicks without touching the panel, for the close and unload
// paths where drawing would be wasted or wrong.
void MetronomeHalt() {
    if (!g_metro.running) {
        return;
    }
    g_metro.running = false;
    StopMetroAudio();
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kPulseTimer);
    }
}

void MetronomeStop() {
    MetronomeHalt();
    Repaint();
}

void MetronomeStart() {
    if (g_metro.running) {
        return;
    }
    MetronomeSync();
    if (!StartMetroAudio()) {
        Wh_Log(L"Metronome: couldn't open the click stream");
        return;
    }
    g_metro.running = true;
    g_metro.paintedPulse = g_metroShared.pulse.load();
    g_metro.pulseAt = -1000.0;
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kPulseTimer, 33, nullptr);
    }
    Repaint();
}

void MetronomeToggle() {
    if (g_metro.running) {
        MetronomeStop();
    } else {
        MetronomeStart();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Strumming

// Publishes the whole pattern to the audio engine: the grid as packed cells
// plus the meter, tempo and technique switches. Called after any edit, and it
// is what lets a running pattern change under the player's hands.
void StrumSync() {
    const int subdivision = std::clamp(g_strum.subdivision, 1, 4);
    g_metroShared.bpmMilli.store(
        static_cast<int>(lround(ToolkitBpm() * 1000.0)));
    g_metroShared.beats.store(std::clamp(g_strum.beats, 1, 16));
    g_metroShared.subdivision.store(subdivision);
    g_metroShared.group.store(1);
    g_metroShared.volume.store(std::clamp(g_settings.metronomeVolume, 0, 100));
    g_metroShared.strumSound.store(std::clamp(g_strum.sound, 0,
                                              kStrumSoundCount - 1));
    g_metroShared.countIn.store(g_strum.countIn);
    const int cells = StrumSlotCount();
    for (int i = 0; i < kStrumSlotsMax; i++) {
        int packed = 0;
        if (i < cells) {
            packed = (g_strum.stroke[i] & 7) | (g_strum.accent[i] ? 8 : 0) |
                     (g_strum.ghost[i] ? 16 : 0) | (g_strum.tie[i] ? 32 : 0);
        }
        g_metroShared.grid[static_cast<size_t>(i)].store(packed);
    }
}

void StrumHalt() {
    if (!g_strum.running) {
        return;
    }
    g_strum.running = false;
    g_metroShared.strum.store(false);
    // Leave the shared stream up if the metronome still wants it, handing the
    // configuration back to that tool; otherwise take the thread down.
    if (g_metro.running) {
        MetronomeSync();
    } else {
        StopMetroAudio();
    }
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kPulseTimer);
    }
}

void StrumStop() {
    StrumHalt();
    Repaint();
}

void StrumStart() {
    if (g_strum.running) {
        return;
    }
    // A practice ramp climbs from wherever you press play toward a target a
    // little faster, so it is armed against the tempo at this moment.
    g_strum.targetBpm = std::clamp(
        static_cast<int>(lround(ToolkitBpm() + g_strum.targetOffsetBpm)), 20,
        400);
    StrumSync();
    // Render the voices before the pattern is handed over. Building them on
    // the audio thread would hold up the first stroke while the Karplus-Strong
    // strings are synthesised for every direction and sound, and doing it ahead
    // of the flag keeps the audio thread off a half-built voice set. The
    // signature is the strings the voices hold, so picking a different chord
    // re-renders them exactly once.
    const int signature = g_strum.instrument * 32 + g_strum.chord;
    if (!g_metroAudio.strumBuilt || g_metroAudio.strumSignature != signature) {
        BuildStrumVoices(g_metroAudio, signature);
    }
    g_metroShared.strum.store(true);
    g_metroShared.measureTick.store(0);
    g_metroShared.slot.store(0);
    // The panel's own guess until the engine's cue arrives, so the playhead is
    // hidden through the count-in from the very first frame.
    g_metroShared.strumCounting.store(g_strum.countIn);
    // Ask the engine to open the bar at its top, which is also what arms the
    // count-in when the stream is already up for the metronome.
    g_metroShared.strumRestart.fetch_add(1);
    if (!StartMetroAudio()) {
        g_metroShared.strum.store(false);
        Wh_Log(L"Strumming: couldn't open the audio stream");
        return;
    }
    g_strum.running = true;
    g_strum.paintedSlot = g_metroShared.pulse.load();
    g_strum.paintedMeasure = g_metroShared.measureTick.load();
    g_strum.paintedSlotAt = -1000.0;
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kPulseTimer, 33, nullptr);
    }
    Repaint();
}

void StrumToggle() {
    if (g_strum.running) {
        StrumStop();
    } else {
        StrumStart();
    }
}

void StrumSetBpm(double bpm) {
    g_tap.bpm = std::clamp(bpm, kMinBpm, kMaxBpm);
    g_tap.hasValue = true;
    g_tap.intervalCount = 0;
    g_tap.lastTap = 0.0;
    StrumSync();
    Repaint();
}

void StrumNudgeTempo(double delta) {
    StrumSetBpm(ToolkitBpm() + delta);
}

// Opens the tempo field on a card. It always starts empty, so the number typed
// replaces the tempo instead of appending to it.
void TempoEntryBegin(int tool) {
    if (tool != kToolMetronome && tool != kToolStrum) {
        return;
    }
    g_tempoEntry.active = true;
    g_tempoEntry.tool = tool;
    g_tempoEntry.count = 0;
    g_tempoEntry.typedAt = NowMs();
    Repaint();
}

void TempoEntryCancel() {
    if (!g_tempoEntry.active) {
        return;
    }
    g_tempoEntry.active = false;
    g_tempoEntry.count = 0;
    Repaint();
}

void TempoEntryErase() {
    if (!g_tempoEntry.active || g_tempoEntry.count == 0) {
        return;
    }
    g_tempoEntry.count--;
    g_tempoEntry.typedAt = NowMs();
    Repaint();
}

// Hands the typed number to whichever tool owns the field. An empty field just
// closes, so clicking the readout and changing your mind costs nothing.
void TempoEntryCommit() {
    if (!g_tempoEntry.active) {
        return;
    }
    int value = 0;
    for (int i = 0; i < g_tempoEntry.count; i++) {
        value = value * 10 + (g_tempoEntry.digits[i] - L'0');
    }
    const bool typed = g_tempoEntry.count > 0;
    const int tool = g_tempoEntry.tool;
    g_tempoEntry.active = false;
    g_tempoEntry.count = 0;
    if (!typed) {
        Repaint();
        return;
    }
    const double bpm =
        std::clamp(static_cast<double>(value), kMinBpm, kMaxBpm);
    if (tool == kToolStrum) {
        StrumSetBpm(bpm);
    } else {
        TapTempoSetBpm(bpm);
    }
}

// Takes one printed digit. The first one opens the field, which is what lets a
// tempo be typed without clicking the readout first; every other key is left
// to the tool's own shortcuts.
bool TempoEntryType(wchar_t c) {
    if (c < L'0' || c > L'9') {
        return false;
    }
    if (g_app.tool != kToolMetronome && g_app.tool != kToolStrum) {
        return false;
    }
    // A list that is up owns the keyboard, so its rows are not typed over.
    if (MenuHit(g_meterMenu) || MenuHit(g_patternMenu) ||
        MenuHit(g_strumMenu) || MenuHit(g_toolMenu)) {
        return false;
    }
    if (!TempoEntryActive()) {
        TempoEntryBegin(g_app.tool);
    }
    if (g_tempoEntry.count < kTempoDigitsMax) {
        g_tempoEntry.digits[g_tempoEntry.count++] = c;
    }
    g_tempoEntry.typedAt = NowMs();
    Repaint();
    return true;
}

// Advances one measure in the practice engine: a ramp lifts the tempo toward
// the target, and a pattern set to play once stops when its first bar ends.
void StrumPracticeStep() {
    const unsigned measure = g_metroShared.measureTick.load();
    if (measure == g_strum.paintedMeasure) {
        return;
    }
    g_strum.paintedMeasure = measure;
    if (!g_strum.loop && measure >= 1) {
        StrumStop();
        return;
    }
    if (g_strum.practice) {
        const double bpm = ToolkitBpm();
        if (bpm < static_cast<double>(g_strum.targetBpm)) {
            StrumSetBpm(std::min(bpm + g_strum.rampStepBpm,
                                 static_cast<double>(g_strum.targetBpm)));
        }
    }
}

void StrumCycleSlot(int slot) {
    if (slot < 0 || slot >= StrumSlotCount()) {
        return;
    }
    g_strum.stroke[slot] = (g_strum.stroke[slot] + 1) % kStrokeCount;
    g_strum.ghost[slot] = false;
    g_strum.tie[slot] = false;
    g_strum.preset = -1;
    StrumSync();
}

void StrumEditSlot(int slot) {
    StrumCycleSlot(slot);
}

void StrumLoadPreset(int index) {
    StrumLoadPresetInto(g_strum, index);
    StrumSync();
    Repaint();
}

// The pattern list is the meter's run of the table with a heading in front of
// every grid, so a line number, a table index and the pattern that is loaded
// are three different numbers and every click or key has to cross over.
int StrumPatternEntryIndex() {  // line of the loaded pattern, or -1
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    for (int i = 0; i < count; i++) {
        if (entries[i].kind == kStrumEntryPattern &&
            entries[i].preset == g_strum.preset) {
            return i;
        }
    }
    return -1;
}

// Where the list opens its cursor: the loaded pattern, or the first pattern of
// the list when the grid is custom. A heading is never a landing place.
int StrumPatternOpenLine() {
    const int line = StrumPatternEntryIndex();
    if (line >= 0) {
        return line;
    }
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    for (int i = 0; i < count; i++) {
        if (entries[i].kind == kStrumEntryPattern) {
            return i;
        }
    }
    return 0;
}

// Loads the pattern on a line, which also moves the grid chips to the grid the
// pattern is written in. A heading carries no pattern, so it loads nothing and
// the list is left open.
bool StrumLoadPatternEntry(int entry) {
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    if (entry < 0 || entry >= count) {
        return false;
    }
    if (entries[entry].kind != kStrumEntryPattern) {
        return false;
    }
    StrumLoadPreset(entries[entry].preset);
    return true;
}

// The headings take no hover, so the arrows walk from pattern to pattern and
// step over them, wrapping at either end of the list.
void StrumMovePatternCursor(int delta) {
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    if (count == 0) {
        return;
    }
    int line = g_patternMenu.cursor;
    if (line < 0 || line >= count) {
        line = delta > 0 ? count - 1 : 0;
    }
    for (int step = 0; step < count; step++) {
        line = (line + delta + count) % count;
        if (entries[line].kind == kStrumEntryPattern) {
            g_patternMenu.cursor = line;
            return;
        }
    }
}

void StrumSetMeter(int beats, int unit) {
    g_strum.beats = std::clamp(beats, 1, 16);
    g_strum.unit = unit;
    // A grid the new bar cannot hold is pulled back to the finest one it can,
    // so a change of meter never leaves half a measure unwritten.
    g_strum.subdivision =
        std::clamp(g_strum.subdivision, 1, StrumGridLimit(g_strum.beats));
    g_strum.preset = -1;
    g_strum.slotCursor = 0;
    StrumSync();
    Repaint();
}

// The bar's meter in the table the meter list is built from, or -1 when it is
// holding one the list does not offer.
int StrumMeterIndex() {
    for (int i = 0; i < kSignatureCount; i++) {
        if (kSignatures[i].beats == g_strum.beats &&
            kSignatures[i].unit == g_strum.unit) {
            return i;
        }
    }
    return -1;
}

void StrumSetSubdivision(int subdivision) {
    g_strum.subdivision =
        std::clamp(subdivision, 1, StrumGridLimit(g_strum.beats));
    g_strum.preset = -1;
    g_strum.slotCursor = 0;
    StrumSync();
    Repaint();
}

void StrumToggleChip(int chip) {
    switch (chip) {
        case kChipLoop:
            g_strum.loop = !g_strum.loop;
            break;
        case kChipCountIn:
            g_strum.countIn = !g_strum.countIn;
            break;
        case kChipPractice:
            g_strum.practice = !g_strum.practice;
            break;
        default:
            return;
    }
    StrumSync();
    Repaint();
}

// Re-renders the voice under the player's hands. The audio thread only mixes
// from the rendered set, so a new instrument or chord cannot be heard without
// building it again; a running pattern restarts on the new strings rather than
// falling silent mid-bar.
void StrumReshape() {
    const bool wasRunning = g_strum.running;
    if (wasRunning) {
        StrumHalt();
    }
    StrumSync();
    if (wasRunning) {
        StrumStart();
    }
    Repaint();
}

void StrumSetInstrument(int instrument) {
    if (instrument == g_strum.instrument) {
        return;
    }
    g_strum.instrument = std::clamp(instrument, 0, kInstrumentCount - 1);
    StrumReshape();
}

// Picks the chord the strings are fretted to, 0 being the open strings. The
// list is shared by both instruments, so switching instrument keeps the choice.
void StrumSetChord(int chord) {
    if (chord == g_strum.chord) {
        return;
    }
    g_strum.chord = std::clamp(chord, 0, kStrumChordCount - 1);
    StrumReshape();
}

// The sound only picks which rendered voice a stroke mixes, so nothing has to
// be built again and the pattern can keep running.
void StrumSetSound(int sound) {
    if (sound == g_strum.sound) {
        return;
    }
    g_strum.sound = std::clamp(sound, 0, kStrumSoundCount - 1);
    StrumSync();
    Repaint();
}

void StrumClearBar() {
    for (int i = 0; i < kStrumSlotsMax; i++) {
        g_strum.stroke[i] = kStrokeRest;
        g_strum.accent[i] = false;
        g_strum.ghost[i] = false;
        g_strum.tie[i] = false;
    }
    g_strum.preset = -1;
    g_strum.slotCursor = 0;
    StrumSync();
    Repaint();
}

// Keyboard editing: a letter drops the focused cell onto one stroke outright,
// clearing the marks that only ever live beside a stroke; the accent, ghost
// and tie keys reach the marks that ride on top of whatever stroke is there.
void StrumSetStroke(int slot, int stroke) {
    if (slot < 0 || slot >= StrumSlotCount()) {
        return;
    }
    g_strum.stroke[slot] = stroke;
    g_strum.ghost[slot] = false;
    g_strum.tie[slot] = false;
    g_strum.preset = -1;
    StrumSync();
    Repaint();
}

void StrumToggleAccent(int slot) {
    if (slot < 0 || slot >= StrumSlotCount()) {
        return;
    }
    g_strum.accent[slot] = !g_strum.accent[slot];
    g_strum.preset = -1;
    StrumSync();
    Repaint();
}

void StrumToggleGhost(int slot) {
    if (slot < 0 || slot >= StrumSlotCount()) {
        return;
    }
    g_strum.ghost[slot] = !g_strum.ghost[slot];
    g_strum.preset = -1;
    StrumSync();
    Repaint();
}

void StrumToggleTie(int slot) {
    if (slot < 0 || slot >= StrumSlotCount()) {
        return;
    }
    g_strum.tie[slot] = !g_strum.tie[slot];
    g_strum.preset = -1;
    StrumSync();
    Repaint();
}

void StrumMoveCursor(int delta) {
    const int cells = StrumSlotCount();
    if (cells <= 0) {
        return;
    }
    g_strum.slotCursor = (g_strum.slotCursor + delta + cells) % cells;
    Repaint();
}

// Tab walks the list the card is showing, so it never leaves the meter: the
// run is the meter's own vocabulary and the cursor wraps inside it, stepping
// over the headings on the way.
void StrumStepPreset(int delta) {
    StrumEntry entries[kStrumPatternEntriesMax];
    const int count = StrumPatternEntries(entries, kStrumPatternEntriesMax,
                                          g_strum.beats, g_strum.unit);
    if (count == 0) {
        return;
    }
    int line = StrumPatternEntryIndex();
    if (line < 0) {
        line = delta > 0 ? count - 1 : 0;
    }
    for (int step = 0; step < count; step++) {
        line = (line + delta + count) % count;
        if (entries[line].kind == kStrumEntryPattern) {
            StrumLoadPreset(entries[line].preset);
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Acoustic clip engine
//
// The toolkit's WASAPI chord engine is parked, so anything that needs a short
// sound renders its own take here and hands it to a brief waveOut stream on a
// detached thread: the circle's scales and chords and the chord card's speaker
// all play through this one sampler. The string model is the plain
// Karplus-Strong pluck - a period of noise recirculated through a two tap
// average, damped per pass, then narrowed by a band pass at the note - rendered
// at the metronome's sample rate and strummed a string at a time.

constexpr float kClipRate = static_cast<float>(kMetroRate);
constexpr float kClipPi = 3.14159265f;

struct ClipRequest {
    int notes[kMaxNotes];
    int noteCount = 0;
    float arpeggio = 0.0f;  // Seconds between notes; 0 strums them together.
    bool piano = false;     // Keys instead of plucked strings.
};

float ClipNoteFrequency(int midi) {
    return 440.0f * powf(2.0f, (static_cast<float>(midi) - 69.0f) / 12.0f);
}

float ClipNoise(uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return static_cast<float>(state) / 2147483648.0f - 1.0f;
}

// One second order section, in the cookbook's normalised form, with its own
// state so a stereo pair can hold two of them.
struct ClipBiquad {
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float x1 = 0.0f;
    float x2 = 0.0f;
    float y1 = 0.0f;
    float y2 = 0.0f;

    float Process(float x) {
        const float y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
        return y;
    }
};

// Band pass with a flat peak: the filter the web-audio pluck put after its
// string to take the aliasing out of the low notes.
ClipBiquad ClipBandpass(float frequency, float q) {
    const float w = 2.0f * kClipPi * frequency / kClipRate;
    const float alpha = sinf(w) / (2.0f * q);
    const float a0 = 1.0f + alpha;
    ClipBiquad filter;
    filter.b0 = alpha / a0;
    filter.b1 = 0.0f;
    filter.b2 = -alpha / a0;
    filter.a1 = -2.0f * cosf(w) / a0;
    filter.a2 = (1.0f - alpha) / a0;
    return filter;
}

ClipBiquad ClipLowPass(float frequency, float q) {
    const float w = 2.0f * kClipPi * frequency / kClipRate;
    const float alpha = sinf(w) / (2.0f * q);
    const float a0 = 1.0f + alpha;
    ClipBiquad filter;
    filter.b0 = (1.0f - cosf(w)) * 0.5f / a0;
    filter.b1 = (1.0f - cosf(w)) / a0;
    filter.b2 = (1.0f - cosf(w)) * 0.5f / a0;
    filter.a1 = -2.0f * cosf(w) / a0;
    filter.a2 = (1.0f - alpha) / a0;
    return filter;
}

// One string, rendered on its own into a mono buffer that starts at sample
// zero. This is the plain Karplus-Strong the toolkit grew out of, kept because
// it is what the plucked chords are meant to sound like: noise across one
// period, averaged with its neighbour so the top of the spectrum goes first,
// damped a hair on every pass, then narrowed by a band pass at the note so the
// aliasing of the low strings reads as a pluck. The sampler and the strumming
// tool both come through here, so they cannot drift apart.
void ClipRenderPluck(std::vector<float>& out, int midi, uint32_t salt) {
    const float frequency = ClipNoteFrequency(midi);
    const int length =
        std::max(2, static_cast<int>(lround(kMetroRate /
                                            static_cast<double>(frequency))));

    std::vector<float> line(static_cast<size_t>(length), 0.0f);
    uint32_t seed =
        (0x9E3779B9u ^ salt) + static_cast<uint32_t>(midi) * 2654435761u;
    for (int i = 0; i < length; i++) {
        line[static_cast<size_t>(i)] = ClipNoise(seed);
    }

    ClipBiquad band = ClipBandpass(frequency, 1.0f);
    const float damping = 0.99f;
    out.assign(static_cast<size_t>(1.5 * kMetroRate), 0.0f);
    size_t index = 0;
    for (size_t n = 0; n < out.size(); n++) {
        const size_t next = (index + 1) % static_cast<size_t>(length);
        const float tap = 0.5f * (line[index] + line[next]);
        out[n] = band.Process(tap);
        line[index] = tap * damping;
        index = next;
    }
}

// One string, added into the stereo mix. The caller keeps the voice's level
// and its place across the stereo field.
void ClipPluckString(std::vector<float>& out,
                     double start,
                     int midi,
                     float velocity,
                     float pan) {
    const size_t total = out.size() / kMetroChannels;
    const size_t first = static_cast<size_t>(start * kMetroRate);
    if (first >= total) {
        return;
    }

    std::vector<float> mono;
    ClipRenderPluck(mono, midi, 0u);
    const float gainLeft = sqrtf(0.5f * (1.0f - pan)) * velocity;
    const float gainRight = sqrtf(0.5f * (1.0f + pan)) * velocity;

    const size_t count = std::min(mono.size(), total - first);
    for (size_t i = 0; i < count; i++) {
        float* frame = &out[(first + i) * kMetroChannels];
        frame[0] += mono[i] * gainLeft;
        frame[1] += mono[i] * gainRight;
    }
}

// One piano key: two strings a couple of hertz apart, one on each channel, so
// the note beats and sits wide, each string carrying the quicker loss and the
// tighter band pass of the reference piano, then the key's own low pass.
void ClipPianoKey(std::vector<float>& out,
                  double start,
                  int midi,
                  float velocity) {
    const float frequency = ClipNoteFrequency(midi);
    const float cutoff = std::min(frequency * 3.2f, 5500.0f);
    const size_t total = out.size() / kMetroChannels;
    const size_t first = static_cast<size_t>(start * kMetroRate);
    if (first >= total) {
        return;
    }
    for (int side = 0; side < kMetroChannels; side++) {
        const float stringFrequency = frequency + (side == 0 ? -1.2f : 1.2f);
        const int length = std::max(
            2, static_cast<int>(lround(kMetroRate /
                                       static_cast<double>(stringFrequency))));
        // The hammer: noise that fades across the string, so the strike starts
        // bright and rolls off.
        std::vector<float> line(static_cast<size_t>(length), 0.0f);
        uint32_t seed = 0x9E3779B9u +
                        static_cast<uint32_t>(midi * 2 + side) * 2654435761u;
        for (int i = 0; i < length; i++) {
            line[static_cast<size_t>(i)] =
                ClipNoise(seed) *
                expf(-static_cast<float>(i) /
                     (static_cast<float>(length) * 0.45f));
        }
        ClipBiquad band = ClipBandpass(stringFrequency, 1.2f);
        ClipBiquad tone = ClipLowPass(cutoff, 0.4f);
        // The top strings give their energy up a shade faster.
        const float damping = 0.989f + 0.007f * Clamp01(stringFrequency / 2000.0f);
        size_t index = 0;
        for (size_t n = first; n < total; n++) {
            const size_t next = (index + 1) % static_cast<size_t>(length);
            const float tap = 0.498f * (line[index] + line[next]);
            const float shaped = tone.Process(band.Process(tap));
            line[index] = tap * damping;
            index = next;
            out[n * kMetroChannels + side] += shaped * velocity;
        }
    }
}

// A comb and an allpass, enough of a room for a note that rings a few seconds.
struct ClipComb {
    std::vector<float> line;
    size_t index = 0;
    float store = 0.0f;

    float Process(float x, float feedback, float damping) {
        const float y = line[index];
        store = y * (1.0f - damping) + store * damping;
        line[index] = x + store * feedback;
        index = (index + 1) % line.size();
        return y;
    }
};

struct ClipAllpass {
    std::vector<float> line;
    size_t index = 0;

    float Process(float x) {
        const float stored = line[index];
        line[index] = x + stored * 0.5f;
        index = (index + 1) % line.size();
        return stored - x;
    }
};

// The piano sits in a room: four combs into two allpasses, mixed back under the
// dry notes at the weights the reference player used.
void ClipPianoRoom(std::vector<float>& out) {
    const size_t frames = out.size() / kMetroChannels;
    std::vector<float> wet(out.size(), 0.0f);
    static const int kCombs[4] = {1557, 1617, 1491, 1422};
    for (int c = 0; c < kMetroChannels; c++) {
        ClipComb combs[4];
        for (int i = 0; i < 4; i++) {
            combs[i].line.assign(static_cast<size_t>(kCombs[i] + c * 23), 0.0f);
        }
        ClipAllpass passes[2];
        passes[0].line.assign(static_cast<size_t>(225 + c * 23), 0.0f);
        passes[1].line.assign(static_cast<size_t>(556 + c * 23), 0.0f);
        for (size_t n = 0; n < frames; n++) {
            const float dry = out[n * kMetroChannels + c];
            float sum = 0.0f;
            for (int i = 0; i < 4; i++) {
                sum += combs[i].Process(dry, 0.87f, 0.25f);
            }
            sum *= 0.25f;
            sum = passes[0].Process(sum);
            sum = passes[1].Process(sum);
            wet[n * kMetroChannels + c] = sum;
        }
    }
    for (size_t i = 0; i < out.size(); i++) {
        out[i] = out[i] * 0.8f + wet[i] * 0.3f;
    }
}

DWORD WINAPI ClipAudioThreadProc(void* param) {
    ClipRequest* request = static_cast<ClipRequest*>(param);
    // A scale asks for a real arpeggio: each note starts well after the last,
    // so it rises where a chord's near-simultaneous spread would blur together.
    const bool spaced = request->arpeggio > 0.0f && request->noteCount > 1;
    const bool piano = request->piano;
    const double spread =
        spaced ? (request->noteCount - 1) * static_cast<double>(request->arpeggio)
               : 0.0;
    // A plucked chord is spent by two seconds; a key keeps ringing for three
    // or more, and its room rings on after that.
    const double length = (piano ? 4.2 : 2.6) + spread;
    std::vector<float> out(static_cast<size_t>(length * kMetroRate) *
                               static_cast<size_t>(kMetroChannels),
                           0.0f);
    for (int i = 0; i < request->noteCount; i++) {
        const int midi = request->notes[i];
        // A strum rolls through the strings a beat apart in the old player's
        // terms - 25 ms, or 30 for keys - and an arpeggio spaces its notes out
        // much further.
        const double start =
            i * (spaced ? static_cast<double>(request->arpeggio)
                        : (piano ? 0.030 : 0.025));
        const float velocity =
            spaced ? 0.58f : 0.50f + 0.04f * static_cast<float>(i % 3);
        const float pan =
            piano || request->noteCount <= 1
                ? 0.0f
                : -0.22f + 0.44f * static_cast<float>(i) /
                                static_cast<float>(request->noteCount - 1);
        if (piano) {
            ClipPianoKey(out, start, midi, velocity);
        } else {
            ClipPluckString(out, start, midi, velocity, pan);
        }
    }
    if (piano) {
        ClipPianoRoom(out);
    }

    const size_t frames = out.size() / kMetroChannels;
    float loudest = 0.0f;
    for (size_t i = 0; i < out.size(); i++) {
        loudest = std::max(loudest, fabsf(out[i]));
    }
    // One scale for the whole clip, so a dense chord is no louder than a single
    // note and nothing has to be clipped, then a tail fade so it never ends on
    // a step.
    const float scale = loudest > 0.0001f ? 0.86f / loudest : 0.0f;
    for (size_t i = 0; i < out.size(); i++) {
        out[i] *= scale;
    }
    const size_t fade =
        std::min(frames, static_cast<size_t>(0.25 * kMetroRate));
    for (size_t i = 0; i < fade; i++) {
        const float tail = static_cast<float>(i) / static_cast<float>(fade);
        float* frame = &out[(frames - fade + i) * kMetroChannels];
        for (int c = 0; c < kMetroChannels; c++) {
            frame[c] *= tail;
        }
    }
    std::vector<short> pcm(out.size());
    for (size_t i = 0; i < out.size(); i++) {
        const float value = std::clamp(out[i], -1.0f, 1.0f);
        pcm[i] = static_cast<short>(lroundf(value * 32767.0f));
    }

    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = kMetroChannels;
    format.nSamplesPerSec = kMetroRate;
    format.wBitsPerSample = 16;
    format.nBlockAlign = static_cast<WORD>(kMetroChannels * 2);
    format.nAvgBytesPerSec = kMetroRate * format.nBlockAlign;
    HWAVEOUT device = nullptr;
    if (waveOutOpen(&device, WAVE_MAPPER, &format, 0, 0,
                    CALLBACK_NULL) == MMSYSERR_NOERROR) {
        WAVEHDR header{};
        header.lpData = reinterpret_cast<LPSTR>(pcm.data());
        header.dwBufferLength =
            static_cast<DWORD>(pcm.size() * sizeof(short));
        if (waveOutPrepareHeader(device, &header, sizeof(WAVEHDR)) ==
            MMSYSERR_NOERROR) {
            if (waveOutWrite(device, &header, sizeof(WAVEHDR)) ==
                MMSYSERR_NOERROR) {
                const ULONGLONG deadline = GetTickCount64() + 8000;
                while (!(header.dwFlags & WHDR_DONE) &&
                       GetTickCount64() < deadline) {
                    Sleep(25);
                }
            }
            waveOutUnprepareHeader(device, &header, sizeof(WAVEHDR));
        }
        waveOutClose(device);
    }
    delete request;
    return 0;
}

// Fire a one-shot clip on its own thread. The worker takes ownership of the
// request; if the thread cannot start the buffer is freed here and nothing
// sounds, which is the right failure mode for a background nicety.
void PlayAcousticClip(const int* notes, int count, float arpeggio, bool piano) {
    ClipRequest* request = new ClipRequest();
    request->noteCount = std::min(count, kMaxNotes);
    for (int i = 0; i < request->noteCount; i++) {
        request->notes[i] = notes[i];
    }
    request->arpeggio = arpeggio;
    request->piano = piano;
    HANDLE thread = CreateThread(nullptr, 0, ClipAudioThreadProc, request, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        delete request;
    }
}

// The selected key's scale, up to the octave above, as a rising arpeggio so it
// reads as a scale rather than a chord. The table is the mode's own: a minor
// key rises on the natural minor, so A minor sounds A B C D E F G and not the
// major scale of A.
void CirclePlayScale() {
    const int* steps = CircleSteps();
    const int tonic = CircleTonicPc();
    int notes[kMaxNotes];
    int count = 0;
    for (int d = 0; d < 7; d++) {
        notes[count++] = 60 + tonic + steps[d];
    }
    notes[count++] = 60 + tonic + 12;
    PlayAcousticClip(notes, count, 0.16f,
                     g_circle.instrument == kInstrumentPiano);
}

// A single diatonic triad, stacked in root position around middle C. The third
// and fifth lift an octave once the walk passes the tonic again.
void CirclePlayChord(int degree) {
    if (degree < 0 || degree > 6) {
        return;
    }
    const int* steps = CircleSteps();
    const int tonic = CircleTonicPc();
    const int span[3] = {0, 2, 4};
    int notes[kMaxNotes];
    int count = 0;
    for (int i = 0; i < 3; i++) {
        int d = degree + span[i];
        const int octave = d >= 7 ? 12 : 0;
        d %= 7;
        notes[count++] = 60 + tonic + steps[d] + octave;
    }
    PlayAcousticClip(notes, count, 0.0f,
                     g_circle.instrument == kInstrumentPiano);
}

// The chord card's speaker. The voicing already carries its notes as MIDI, for
// the piano's held keys as much as for the fretboard, so a preview is the same
// one-shot clip the circle plays: an arpeggio of zero lets the clip stagger the
// notes itself, which reads as a strum on a fretted chord and a roll on a piano
// one.
void ChordPlaySelected() {
    const Voicing* voicing = SelectedVoicing();
    if (!voicing || voicing->noteCount <= 0) {
        return;
    }
    PlayAcousticClip(voicing->notes, voicing->noteCount, 0.0f,
                     g_app.instrument == kInstrumentPiano);
}

// Pick a spoke and the ring it is read on. A key outside the twelve wraps, so
// the wheel can be walked in either direction without bounds checks.
void CircleSelectRing(int key, bool minor, bool play) {
    g_circle.key = ((key % 12) + 12) % 12;
    g_circle.minor = minor;
    g_circle.flashAt = NowMs();
    if (play) {
        CirclePlayScale();
    }
    Repaint();
}

// Walking the wheel keeps the ring it is being read on: the arrow keys and the
// wheel move a fifth at a time, majors with majors and minors with minors.
void CircleSelect(int key, bool play) {
    CircleSelectRing(key, g_circle.minor, play);
}

// Step in or out of the wheel without moving along it, which is how the outer
// key and the relative minor under it are compared.
void CircleSetRing(bool minor, bool play) {
    if (minor != g_circle.minor) {
        CircleSelectRing(g_circle.key, minor, play);
    }
}

void CircleToggleMode(bool play) {
    CircleSelectRing(g_circle.key, !g_circle.minor, play);
}

// The instrument chips only recolour the sound, so picking one auditions the
// new voice with the selected key's scale when the panel is set to play on
// select.
void CircleSetInstrument(int instrument) {
    if (instrument < 0 || instrument >= kInstrumentCount ||
        g_circle.instrument == instrument) {
        return;
    }
    g_circle.instrument = instrument;
    if (g_circle.playOnSelect) {
        CirclePlayScale();
    }
    Repaint();
}

////////////////////////////////////////////////////////////////////////////////
// Note trainer controls
//
// The trainer borrows the tuner's microphone: one capture thread publishes the
// frequency it hears into g_tunerShared, and a panel timer reads it each tick
// to decide whether the note just played is the one shown. A match held for a
// couple of ticks scores; a bare hit plus a bonus for the time left on the
// clock, so reading the geography fast pays best.

int TrainerRandomNote() {
    const TrainerRange& range = kTrainerRanges[g_trainer.instrument];
    const int lo = range.minMidi;
    const int hi = lo + range.span;
    int midi = lo;
    // Retry off an immediate repeat or a black note in naturals mode, but give
    // up rather than spin if the range is too tight to satisfy both.
    for (int guard = 0; guard < 200; guard++) {
        midi = lo + rand() % (hi - lo + 1);
        if (midi == g_trainer.target) {
            continue;
        }
        if (!g_trainer.naturals || TrainerIsNatural(midi)) {
            break;
        }
    }
    return midi;
}

void TrainerNextNote() {
    g_trainer.target = TrainerRandomNote();
    g_trainer.roundMs = g_trainer.seconds * 1000.0;
    g_trainer.noteEndsAt = NowMs() + g_trainer.roundMs;
    g_trainer.stable = 0;
    g_trainer.heard = -1;
}

// Opening the tool wakes the microphone; the round itself only starts on
// demand so the drill never begins before the player is ready.
void TrainerEnter() {
    if (!g_trainer.seeded) {
        srand(static_cast<unsigned>(GetTickCount64()));
        g_trainer.seeded = true;
    }
    if (!g_trainer.listening) {
        g_tunerShared.frequency.store(0.0f);
        g_tunerShared.confidence.store(0.0f);
        g_trainer.listening = StartTunerAudio();
        g_trainer.micFailed = !g_trainer.listening;
        if (!g_trainer.listening) {
            Wh_Log(L"Note trainer: couldn't open the microphone");
        }
    }
    Repaint();
}

// Leaving the tool or closing the panel: stop the round, release the mic and
// take the tick down. Safe to call when nothing is running.
void TrainerHalt() {
    g_trainer.running = false;
    if (g_trainer.listening) {
        g_trainer.listening = false;
        StopTunerAudio();
    }
    g_trainer.micFailed = false;
    g_trainer.result = 0;
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kTrainerTimer);
    }
    Repaint();
}

void TrainerResetScore() {
    g_trainer.correct = 0;
    g_trainer.total = 0;
    g_trainer.score = 0;
    g_trainer.streak = 0;
    g_trainer.bestStreak = 0;
    g_trainer.result = 0;
    Repaint();
}

void TrainerStartRound() {
    TrainerEnter();  // Make sure the mic is open before the first prompt.
    if (g_trainer.micFailed) {
        Repaint();  // Without input every prompt would time out, so stay idle.
        return;
    }
    g_trainer.running = true;
    g_trainer.result = 0;
    TrainerNextNote();
    if (g_panel.Hwnd()) {
        SetTimer(g_panel.Hwnd(), kTrainerTimer, 60, nullptr);
    }
    Repaint();
}

void TrainerStopRound() {
    g_trainer.running = false;
    g_trainer.result = 0;
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kTrainerTimer);
    }
    Repaint();
}

void TrainerToggle() {
    if (g_trainer.running) {
        TrainerStopRound();
    } else {
        TrainerStartRound();
    }
}

void TrainerSetInstrument(int instrument) {
    instrument = std::clamp(instrument, 0, kInstrumentCount - 1);
    if (instrument == g_trainer.instrument) {
        return;
    }
    g_trainer.instrument = instrument;
    if (g_trainer.running) {
        TrainerNextNote();  // The new range takes over on the next prompt.
    }
    Repaint();
}

// One tick of a live round: read the microphone, decide the note, and move on
// when it is matched or the timer runs out.
void TrainerTick() {
    if (!g_trainer.running || g_app.tool != kToolTrainer ||
        g_phase == kPhaseClosed) {
        return;
    }
    const double now = NowMs();
    const float raw = g_tunerShared.frequency.load();
    int cents = 0;
    const int heard =
        raw > 0.0f ? NoteFromFrequency(raw, g_tuner.reference, cents) : -1;
    g_trainer.heard = heard;

    if (heard == g_trainer.target && cents >= -kTrainerCents &&
        cents <= kTrainerCents) {
        if (++g_trainer.stable >= kTrainerStable) {
            const double remain = static_cast<double>(Clamp01(
                static_cast<float>((g_trainer.noteEndsAt - now) /
                                   g_trainer.roundMs)));
            const int points = 100 + static_cast<int>(lround(remain * 100.0));
            g_trainer.score += points;
            g_trainer.lastPoints = points;
            g_trainer.correct++;
            g_trainer.total++;
            g_trainer.streak++;
            g_trainer.bestStreak =
                std::max(g_trainer.bestStreak, g_trainer.streak);
            g_trainer.result = 1;
            g_trainer.resultAt = now;
            TrainerNextNote();
        }
    } else {
        g_trainer.stable = 0;
    }

    // A fresh prompt has just armed its own timer, so a hit cannot also read as
    // this tick's timeout; only a genuinely expired clock misses here.
    if (now >= g_trainer.noteEndsAt) {
        g_trainer.total++;
        g_trainer.streak = 0;
        g_trainer.result = 2;
        g_trainer.resultAt = now;
        TrainerNextNote();
    }
    Repaint();
}

// refit is set when the whole meter changes, so a preset or a signature step
// picks up the grouping that meter is normally felt in; the steppers leave a
// group the player chose alone.
void MetronomeSetMeter(int beats, int unit, bool refit) {
    g_metro.beats = std::clamp(beats, 1, 16);
    g_metro.unit = unit;
    if (refit) {
        g_metro.group = DefaultGrouping(g_metro.beats, g_metro.unit);
    }
    MetronomeSync();
    Repaint();
}

// Walks the preset signatures, and joins them from whichever meter is closest
// when the steppers have left the presets behind.
void MetronomeStepMeter(int delta) {
    int current = -1;
    for (int i = 0; i < kSignatureCount; i++) {
        if (kSignatures[i].beats == g_metro.beats &&
            kSignatures[i].unit == g_metro.unit) {
            current = i;
            break;
        }
    }
    if (current < 0) {
        // Off the presets: the next one along is the closest in that direction.
        int candidate = -1;
        if (delta > 0) {
            for (int i = 0; i < kSignatureCount; i++) {
                if (kSignatures[i].beats > g_metro.beats) {
                    candidate = i;
                    break;
                }
            }
        } else {
            for (int i = kSignatureCount - 1; i >= 0; i--) {
                if (kSignatures[i].beats < g_metro.beats) {
                    candidate = i;
                    break;
                }
            }
        }
        if (candidate < 0) {
            candidate = delta > 0 ? 0 : kSignatureCount - 1;
        }
        MetronomeSetMeter(kSignatures[candidate].beats,
                          kSignatures[candidate].unit, true);
        return;
    }
    const int next =
        (current + delta + kSignatureCount) % kSignatureCount;
    MetronomeSetMeter(kSignatures[next].beats, kSignatures[next].unit, true);
}

void MetronomeNudgeTempo(double delta) {
    TapTempoSetBpm(ToolkitBpm() + delta);
}

// Registers the toggle shortcut from the settings, swapping out whatever was
// registered before. The message window has to exist first, so this is a
// no-op until CreateWindows has made it; from then on every settings change
// re-hooks it.
void ApplyToggleHotkey() {
    if (!g_messageWindow) {
        return;
    }
    UnregisterHotKey(g_messageWindow, kHotkeyToggle);
    if (!RegisterHotKey(g_messageWindow, kHotkeyToggle, g_settings.hotkeyMods,
                        g_settings.hotkeyVk)) {
        Wh_Log(L"%s is taken by another app (error %u)",
               g_settings.hotkeyText.c_str(), GetLastError());
    }
}

void ApplySettings() {
    g_settings = LoadSettings();
    g_accent = ResolveAccent(g_settings.accentChoice, g_settings.customAccent);
    if (!g_metro.defaultsApplied) {
        // The settings only seed the meter, so a later change under Settings
        // never pulls a meter out from under the panel.
        g_metro.defaultsApplied = true;
        g_metro.beats = g_settings.metronomeBeats;
        g_metro.unit = g_settings.metronomeUnit;
        g_metro.subdivision = g_settings.metronomeSubdivision;
        g_metro.group = DefaultGrouping(g_metro.beats, g_metro.unit);
    }
    if (!g_strum.defaultsApplied) {
        // Same one-shot seeding as the metronome: the strumming settings only
        // shape the tool the first time, so a later trip through Settings never
        // rewrites a pattern the player hand-edited.
        g_strum.defaultsApplied = true;
        g_strum.beats = 4;
        g_strum.unit = 4;
        g_strum.subdivision = g_settings.strumSubdivision;
        g_strum.countIn = g_settings.strumCountIn;
        g_strum.loop = g_settings.strumLoop;
        g_strum.rampStepBpm = g_settings.strumRampStep;
        g_strum.targetOffsetBpm = g_settings.strumTargetOffset;
        // Open on a plain downbeat pattern: one downstroke on every beat,
        // accented on the one, laid out across the default subdivision.
        const int cells = StrumSlotCount();
        for (int i = 0; i < kStrumSlotsMax; i++) {
            g_strum.stroke[i] = kStrokeRest;
            g_strum.accent[i] = false;
            g_strum.ghost[i] = false;
            g_strum.tie[i] = false;
        }
        for (int beat = 0; beat * g_strum.subdivision < cells; beat++) {
            const int slot = beat * g_strum.subdivision;
            g_strum.stroke[slot] = kStrokeDown;
            g_strum.accent[slot] = beat == 0;
        }
        g_strum.preset = 0;
        g_strum.slotCursor = 0;
        StrumSync();
    }
    if (!g_circle.defaultsApplied) {
        // One-shot seeding like the other tools: the circle settings only
        // shape the panel the first time, so a later trip through Settings
        // never overrides the key the player is exploring.
        g_circle.defaultsApplied = true;
        g_circle.minor = g_settings.circleMinor;
        g_circle.playOnSelect = g_settings.circlePlayOnSelect;
    }
    if (!g_trainer.defaultsApplied) {
        // The trainer's time limit and note set come from the settings once,
        // so a later pass never resets a round in progress.
        g_trainer.defaultsApplied = true;
        g_trainer.seconds = g_settings.trainerSeconds;
        g_trainer.naturals = g_settings.trainerNaturals;
    }
    MetronomeSync();
    // The reference pitch follows the settings; the panel never edits it.
    g_tuner.reference = g_settings.tunerReference;
    // Re-hook the shortcut, unless this is the first load before the message
    // window exists, in which case CreateWindows registers it a moment later.
    ApplyToggleHotkey();
}

void SearchChanged() {
    g_app.voicing = 0;
    g_app.selected = 0;
    g_app.scroll = 0;
    Refilter();
    SelectResult(0, true);
    g_caretStart = NowMs();
    Repaint();
}

void DeleteQuerySelection() {
    if (!HasSelection()) {
        return;
    }
    const int start = SelectionStart();
    g_app.query.erase(static_cast<size_t>(start),
                      static_cast<size_t>(SelectionEnd() - start));
    g_caret = start;
    g_selAnchor = start;
}

void AppendQueryChar(wchar_t c) {
    DeleteQuerySelection();
    if (g_app.query.size() >= kMaxQueryLength) {
        return;
    }
    g_app.query.insert(g_app.query.begin() + g_caret, c);
    g_caret++;
    g_selAnchor = g_caret;
    SearchChanged();
}

void EraseQueryChar() {
    if (HasSelection()) {
        DeleteQuerySelection();
        SearchChanged();
        return;
    }
    if (g_caret <= 0) {
        return;
    }
    g_app.query.erase(static_cast<size_t>(g_caret) - 1, 1);
    g_caret--;
    g_selAnchor = g_caret;
    SearchChanged();
}

void DeleteQueryChar() {
    if (HasSelection()) {
        DeleteQuerySelection();
        SearchChanged();
        return;
    }
    if (g_caret >= static_cast<int>(g_app.query.size())) {
        return;
    }
    g_app.query.erase(static_cast<size_t>(g_caret), 1);
    SearchChanged();
}

void SelectAllQuery() {
    if (g_app.query.empty()) {
        return;
    }
    g_selAnchor = 0;
    g_caret = static_cast<int>(g_app.query.size());
    g_caretStart = NowMs();
    Repaint();
}

void ClearQuery() {
    g_app.query.clear();
    g_caret = 0;
    g_selAnchor = 0;
    SearchChanged();
}

void OnMouseMove(HWND hwnd, POINT pt) {
    if (!g_trackingMouse) {
        TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
        TrackMouseEvent(&track);
        g_trackingMouse = true;
    }

    if (g_dragging) {
        POINT cursor;
        GetCursorPos(&cursor);
        const RECT work = MonitorWorkArea();
        const int x = g_dragOrigin.x + (cursor.x - g_dragCursor.x);
        const int y = g_dragOrigin.y + (cursor.y - g_dragCursor.y);
        g_x = std::clamp(x, static_cast<int>(work.left),
                         std::max(static_cast<int>(work.left),
                                  static_cast<int>(work.right) - g_layout.width));
        g_y = std::clamp(y, static_cast<int>(work.top),
                         std::max(static_cast<int>(work.top),
                                  static_cast<int>(work.bottom) - g_layout.height));
        PresentPanel(false);
        return;
    }

    if (g_selecting) {
        // Dragging inside the search box extends the selection.
        const int x = std::clamp(static_cast<int>(pt.x),
                                 static_cast<int>(g_layout.searchText.left),
                                 static_cast<int>(g_layout.searchText.right));
        MoveCaret(QueryIndexFromX(x), true);
        Repaint();
        return;
    }

    if (g_sliding) {
        // Dragging across the diagram walks the fingerings, one step per slice.
        const ChordEntry* entry = SelectedChord();
        if (entry && VoicingCount(g_app.instrument, *entry) > 1) {
            const int step = std::max(S(26.0f), 12);
            bool changed = false;
            while (pt.x - g_slideAnchor >= step) {
                StepVoicing(-1);
                g_slideAnchor += step;
                changed = true;
            }
            while (g_slideAnchor - pt.x >= step) {
                StepVoicing(1);
                g_slideAnchor -= step;
                changed = true;
            }
            if (changed) {
                Repaint();
            }
        }
        return;
    }

    const HitResult hit = HitTest(pt);
    if (hit.id != g_hover.id || hit.index != g_hover.index) {
        g_hover = hit;
        Repaint();
    }
}

#if 0  // Chord playback, parked with the engine above.
void PlaySelectedChord() {
    const Voicing* voicing = SelectedVoicing();
    if (!voicing) {
        return;
    }
    const auto& info = chorddata::kInstruments[g_app.instrument];
    AudioRequest request;
    request.plucked = info.kind == chorddata::kFretted;
    // A four string neck reads as a ukulele: shorter and brighter than a guitar.
    request.damping = info.strings > 0 && info.strings <= 4 ? 0.9962f : 0.9975f;
    if (request.plucked && info.tuning) {
        // Low string first, so the render thread strums upwards.
        for (int i = 0; i < voicing->stringCount && request.noteCount < kMaxNotes;
             i++) {
            const int fret = voicing->frets[i];
            if (fret == kMutedFret) {
                continue;
            }
            // An open string is fret zero at the nut, not the base fret.
            const int absolute = fret == 0 ? 0 : voicing->baseFret + fret - 1;
            request.notes[request.noteCount++] =
                info.tuning[i] + absolute;
        }
    } else {
        const int count = std::min(voicing->noteCount, kMaxNotes);
        for (int i = 0; i < count; i++) {
            request.notes[i] = voicing->notes[i];
        }
        std::sort(request.notes, request.notes + count);
        request.noteCount = count;
    }
    if (request.noteCount == 0) {
        return;
    }
    AudioPlay(request);
}
#endif  // PlaySelectedChord

void OnMouseDown(HWND hwnd, POINT pt) {
    const HitResult hit = HitTest(pt);
    if (MenuVisual(g_toolMenu) && hit.id == kHitNone) {
        // The click that dismisses an open list does nothing else.
        MenuHide(g_toolMenu);
        Repaint();
        return;
    }
    if (g_app.tool == kToolTuner && MenuVisual(g_tunerMenu) &&
        (hit.id == kHitNone || hit.id == kHitToolSelect)) {
        // Same for the tuner lists, which also yield to the tool picker. A
        // click inside the list's own rows belongs to the list, so it is not
        // one of these.
        MenuHide(g_tunerMenu);
        Repaint();
        if (hit.id == kHitToolSelect) {
            g_pressed = hit;
            MenuShow(g_toolMenu, kMenuTools, g_app.tool);
            g_hover = HitResult();
            Repaint();
        }
        return;
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_meterMenu) &&
        (hit.id == kHitNone || hit.id == kHitToolSelect)) {
        // The meter list hangs over the lower half of the card, so a click
        // outside it only puts it away; it yields to the tool picker too.
        MenuHide(g_meterMenu);
        Repaint();
        if (hit.id == kHitToolSelect) {
            g_pressed = hit;
            MenuShow(g_toolMenu, kMenuTools, g_app.tool);
            g_hover = HitResult();
            Repaint();
        }
        return;
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_patternMenu) &&
        (hit.id == kHitNone || hit.id == kHitToolSelect)) {
        // The pattern list sits over the card, so it is the one a stray click
        // dismisses; like the chord list, it yields to the tool picker.
        MenuHide(g_patternMenu);
        Repaint();
        if (hit.id == kHitToolSelect) {
            g_pressed = hit;
            MenuShow(g_toolMenu, kMenuTools, g_app.tool);
            g_hover = HitResult();
            Repaint();
        }
        return;
    }
    if (g_app.tool == kToolStrum && MenuVisual(g_strumMenu) &&
        (hit.id == kHitNone || hit.id == kHitToolSelect)) {
        // Same for the chord list, which also yields to the tool picker. A
        // click on one of its own rows belongs to the list.
        MenuHide(g_strumMenu);
        Repaint();
        if (hit.id == kHitToolSelect) {
            g_pressed = hit;
            MenuShow(g_toolMenu, kMenuTools, g_app.tool);
            g_hover = HitResult();
            Repaint();
        }
        return;
    }
    g_pressed = hit;
    switch (hit.id) {
        case kHitClose:
            CloseFlyout();
            break;
        case kHitToolSelect:
            if (MenuHit(g_toolMenu)) {
                MenuHide(g_toolMenu);
            } else {
                MenuShow(g_toolMenu, kMenuTools, g_app.tool);
            }
            g_hover = HitResult();
            Repaint();
            break;
        case kHitToolMenuItem:
            MenuHide(g_toolMenu);
            SetTool(hit.index);
            break;
        case kHitTapPad: {
            // Clicking also grabs focus, so the keyboard is ready to tap.
            SetFocus(hwnd);
            TapTempoTap();
            break;
        }
        case kHitTapReset:
            TapTempoReset();
            break;
        case kHitTapHalf: {
            const double base = g_tap.hasValue ? g_tap.bpm : 120.0;
            TapTempoSetBpm(base * 0.5);
            break;
        }
        case kHitTapDouble: {
            const double base = g_tap.hasValue ? g_tap.bpm : 120.0;
            TapTempoSetBpm(base * 2.0);
            break;
        }
        case kHitMetroStart:
            MetronomeToggle();
            break;
        case kHitMetroTap:
            // The tap button feeds the same tempo the metronome plays.
            SetFocus(hwnd);
            TapTempoTap();
            break;
        case kHitMetroTempo:
            // The readout is the field: a first click opens it, a second one
            // takes what is typed.
            SetFocus(hwnd);
            if (TempoEntryActive()) {
                TempoEntryCommit();
            } else {
                TempoEntryBegin(kToolMetronome);
            }
            break;
        case kHitMetroPreset:
            if (hit.index >= 0 && hit.index < kSignatureCount) {
                MetronomeSetMeter(kSignatures[hit.index].beats,
                                  kSignatures[hit.index].unit, true);
            }
            break;
        case kHitMetroBeatsDown:
        case kHitMetroBeatsUp: {
            const int next =
                g_metro.beats + (hit.id == kHitMetroBeatsUp ? 1 : -1);
            MetronomeSetMeter(std::clamp(next, 1, 16), g_metro.unit, false);
            break;
        }
        case kHitMetroUnit:
            if (hit.index >= 0 && hit.index < 4) {
                MetronomeSetMeter(g_metro.beats, kBeatUnits[hit.index], false);
            }
            break;
        case kHitMetroSubdivision:
            g_metro.subdivision = hit.index + 1;
            MetronomeSync();
            Repaint();
            break;
        case kHitMetroGroup:
            g_metro.group = g_metro.group >= 4 ? 1 : g_metro.group + 1;
            MetronomeSync();
            Repaint();
            break;
        case kHitTunerInst:
            if (MenuHit(g_tunerMenu) && g_tunerMenu.target == kMenuInstrument) {
                MenuHide(g_tunerMenu);
            } else {
                MenuShow(g_tunerMenu, kMenuInstrument, g_tuner.instrument);
                g_hover = HitResult();
            }
            Repaint();
            break;
        case kHitTunerTune:
            if (MenuHit(g_tunerMenu) && g_tunerMenu.target == kMenuTuning) {
                MenuHide(g_tunerMenu);
            } else {
                MenuShow(g_tunerMenu, kMenuTuning,
                         TunerTuningPosition(g_tuner.instrument,
                                             g_tuner.tuning));
                g_hover = HitResult();
            }
            Repaint();
            break;
        case kHitTunerMenuRow: {
            const bool instrumentList = g_tunerMenu.shown == kMenuInstrument;
            if (instrumentList) {
                TunerSetInstrument(hit.index);
            } else {
                TunerSetTuning(TunerTuningAt(g_tuner.instrument, hit.index));
            }
            MenuHide(g_tunerMenu);
            Repaint();
            break;
        }
        case kHitTunerString:
            // Clicking a chip holds just that string; clicking it again lets
            // go and reads every string again.
            TunerToggleString(hit.index);
            break;
        case kHitStrumPattern:
            if (MenuHit(g_patternMenu)) {
                MenuHide(g_patternMenu);
            } else {
                MenuShow(g_patternMenu, kMenuStrumPattern,
                         StrumPatternOpenLine());
                g_hover = HitResult();
            }
            Repaint();
            break;
        case kHitStrumPatternRow:
            // A heading answers the click but carries no pattern, so it leaves
            // both the grid and the list as they are.
            if (StrumLoadPatternEntry(hit.index)) {
                MenuHide(g_patternMenu);
            }
            Repaint();
            break;
        case kHitStrumSlot: {
            // A click picks the cell and steps it along the five strokes. The
            // marks that ride on a stroke are keyboard work, on single letters
            // that need no modifier: a accent, g ghost, t tie.
            const int slot = hit.index;
            if (slot >= 0 && slot < StrumSlotCount()) {
                g_strum.slotCursor = slot;
                StrumEditSlot(slot);
                Repaint();
            }
            break;
        }
        case kHitStrumMeter:
            if (MenuHit(g_meterMenu)) {
                MenuHide(g_meterMenu);
            } else {
                MenuShow(g_meterMenu, kMenuStrumMeter,
                         std::max(0, StrumMeterIndex()));
                g_hover = HitResult();
            }
            Repaint();
            break;
        case kHitStrumMeterRow:
            if (hit.index >= 0 && hit.index < kSignatureCount) {
                StrumSetMeter(kSignatures[hit.index].beats,
                              kSignatures[hit.index].unit);
            }
            MenuHide(g_meterMenu);
            Repaint();
            break;
        case kHitStrumSub:
            if (hit.index >= 0 && hit.index < 4) {
                StrumSetSubdivision(hit.index + 1);
            }
            break;
        case kHitStrumPlay:
            StrumToggle();
            break;
        case kHitStrumChip:
            StrumToggleChip(hit.index);
            break;
        case kHitStrumInst:
            if (hit.index == 0 || hit.index == 1) {
                StrumSetInstrument(hit.index == 0 ? kInstrumentGuitar
                                                  : kInstrumentUkulele);
            }
            break;
        case kHitStrumSound:
            StrumSetSound(hit.index);
            break;
        case kHitStrumChord:
            if (MenuHit(g_strumMenu)) {
                MenuHide(g_strumMenu);
            } else {
                MenuShow(g_strumMenu, kMenuStrumChord, g_strum.chord);
                g_hover = HitResult();
            }
            Repaint();
            break;
        case kHitStrumChordRow:
            StrumSetChord(hit.index);
            MenuHide(g_strumMenu);
            Repaint();
            break;
        case kHitStrumTempo:
            SetFocus(hwnd);
            if (TempoEntryActive()) {
                TempoEntryCommit();
            } else {
                TempoEntryBegin(kToolStrum);
            }
            break;
        case kHitStrumTempoDown:
            // A nudge takes the typed number first, so "+" after typing 140
            // lands on 141 rather than on the tempo that was playing.
            TempoEntryCommit();
            StrumNudgeTempo(-1.0);
            break;
        case kHitStrumTempoUp:
            TempoEntryCommit();
            StrumNudgeTempo(1.0);
            break;
        case kHitCircleKey:
            // The index carries the ring too: 0-11 are the majors outside and
            // 12-23 the relative minors inside the same spokes.
            CircleSelectRing(hit.index % kCircleRingMinor,
                             hit.index >= kCircleRingMinor,
                             g_circle.playOnSelect);
            break;
        case kHitCircleInst:
            CircleSetInstrument(kInstrumentChipOrder[hit.index]);
            break;
        case kHitCircleChord:
            CirclePlayChord(hit.index);
            break;
        case kHitTrainerInst:
            TrainerSetInstrument(kInstrumentChipOrder[hit.index]);
            break;
        case kHitTrainerNaturals:
            g_trainer.naturals = !g_trainer.naturals;
            if (g_trainer.running) {
                TrainerNextNote();
            }
            Repaint();
            break;
        case kHitTrainerStart:
            TrainerToggle();
            break;
        case kHitHeader:
            g_dragging = true;
            GetCursorPos(&g_dragCursor);
            g_dragOrigin.x = g_x;
            g_dragOrigin.y = g_y;
            SetCapture(hwnd);
            break;
        case kHitTab:
            g_searchFocused = true;
            SetInstrument(hit.index, true);
            Repaint();
            break;
        case kHitSearch: {
            g_searchFocused = true;
            MoveCaret(QueryIndexFromX(pt.x), false);
            g_selecting = true;
            g_caretStart = NowMs();
            SetCapture(hwnd);
            Repaint();
            break;
        }
        case kHitRow:
            SelectResult(hit.index, false);
            g_app.voicing = 0;
            Repaint();
            break;
        case kHitPrevious:
            StepVoicing(-1);
            Repaint();
            break;
        case kHitNext:
            StepVoicing(1);
            Repaint();
            break;
        case kHitDot:
            g_app.voicing = hit.index;
            Repaint();
            break;
        case kHitDiagram:
            g_sliding = true;
            g_slideAnchor = pt.x;
            SetCapture(hwnd);
            break;
        case kHitSpeaker:
            ChordPlaySelected();
            break;
        default:
            break;
    }
}

void OnMouseUp() {
    g_pressed = HitResult();
    if (g_dragging || g_sliding || g_selecting) {
        g_dragging = false;
        g_sliding = false;
        g_selecting = false;
        ReleaseCapture();
        Repaint();  // The hover state went stale while the mouse was captured.
    }
}

void OnWheel(HWND hwnd, WPARAM wParam, POINT pt) {
    ScreenToClient(hwnd, &pt);
    const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (delta == 0) {
        return;
    }
    const int direction = delta > 0 ? -1 : 1;
    if (g_app.tool == kToolTempo) {
        // The wheel fine tunes the tempo one BPM at a time, ten with Shift.
        const bool shift = GetKeyState(VK_SHIFT) < 0;
        const double current = g_tap.hasValue ? g_tap.bpm : 120.0;
        TapTempoSetBpm(current - direction * (shift ? 10.0 : 1.0));
        return;
    }
    if (g_app.tool == kToolMetronome) {
        const bool shift = GetKeyState(VK_SHIFT) < 0;
        TempoEntryCommit();  // A nudge takes what was typed first.
        MetronomeNudgeTempo(-direction * (shift ? 10.0 : 1.0));
        return;
    }
    if (g_app.tool == kToolStrum) {
        const bool shift = GetKeyState(VK_SHIFT) < 0;
        TempoEntryCommit();
        StrumNudgeTempo(-direction * (shift ? 10.0 : 1.0));
        return;
    }
    if (g_app.tool == kToolCircle) {
        // The wheel walks the ring a fifth at a time, silent until you press
        // Space or click.
        CircleSelect(g_circle.key + direction, false);
        return;
    }
    if (g_app.tool == kToolTrainer) {
        // The wheel steps the instrument chips, matching the click order.
        int step = 0;
        for (int i = 0; i < kInstrumentChipCount; i++) {
            if (kInstrumentChipOrder[i] == g_trainer.instrument) {
                step = i;
                break;
            }
        }
        step = (step + direction + kInstrumentChipCount) % kInstrumentChipCount;
        TrainerSetInstrument(kInstrumentChipOrder[step]);
        return;
    }
    if (g_app.tool == kToolTuner) {
        // The wheel walks the tunings, so a drop tuning is one flick away.
        TunerStepTuning(direction);
        return;
    }
    if (PtInRect(&g_layout.card, pt)) {
        StepVoicing(direction);
    } else {
        SelectResult(g_app.selected + direction * 3, true);
        g_app.voicing = 0;
    }
    Repaint();
}

void OnKeyDown(WPARAM key) {
    const bool shift = GetKeyState(VK_SHIFT) < 0;
    const bool ctrl = GetKeyState(VK_CONTROL) < 0;
    if (ctrl && key == VK_TAB) {
        SetTool((g_app.tool + 1) % kToolCount);
        return;
    }
    if (TempoEntryActive()) {
        // The tempo field owns the keys that edit it; everything else - Space
        // above all - still reaches the transport, so the bar can be started
        // with the number that was just typed.
        if (key == VK_ESCAPE) {
            TempoEntryCancel();
            return;
        }
        if (key == VK_RETURN) {
            TempoEntryCommit();
            return;
        }
        if (key == VK_BACK) {
            TempoEntryErase();
            return;
        }
        if (key == VK_UP || key == VK_DOWN) {
            // The arrows commit first, so they nudge what is on screen.
            TempoEntryCommit();
        }
    }
    if (MenuHit(g_toolMenu)) {
        if (key == VK_ESCAPE) {
            MenuHide(g_toolMenu);
            Repaint();
            return;
        }
        if (key == VK_RETURN) {
            // The listbox commits the row the cursor rests on.
            if (g_toolMenu.cursor >= 0) {
                SetTool(g_toolMenu.cursor);
            } else {
                MenuHide(g_toolMenu);
                Repaint();
            }
            return;
        }
        if (key == VK_UP || key == VK_DOWN) {
            // Browsing the list leaves it open, with the cursor riding on it.
            MenuMoveCursor(g_toolMenu, key == VK_DOWN ? 1 : -1);
            Repaint();
            return;
        }
    }
    if (g_app.tool == kToolTempo) {
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        const bool tapKey = (key == VK_SPACE && g_settings.tapKeySpace) ||
                            (key == VK_RETURN && g_settings.tapKeyEnter);
        if (tapKey) {
            TapTempoTap();
            return;
        }
        if (key == 'R') {
            TapTempoReset();
            return;
        }
        if (key == VK_LEFT || key == VK_RIGHT) {
            const double base = g_tap.hasValue ? g_tap.bpm : 120.0;
            TapTempoSetBpm(key == VK_LEFT ? base * 0.5 : base * 2.0);
        }
        return;
    }
    if (g_app.tool == kToolMetronome) {
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        if (key == VK_SPACE) {
            MetronomeToggle();
            return;
        }
        if (key == VK_UP || key == VK_DOWN) {
            MetronomeNudgeTempo((key == VK_UP ? 1.0 : -1.0) *
                                (shift ? 10.0 : 1.0));
            return;
        }
        if (key == VK_LEFT || key == VK_RIGHT) {
            MetronomeStepMeter(key == VK_LEFT ? -1 : 1);
        }
        return;
    }
    if (g_app.tool == kToolStrum) {
        if (MenuHit(g_meterMenu)) {
            if (key == VK_ESCAPE) {
                MenuHide(g_meterMenu);
                Repaint();
                return;
            }
            if (key == VK_RETURN) {
                // Enter picks the meter the cursor rests on; with no row
                // picked yet it just folds the list away.
                if (g_meterMenu.cursor >= 0 &&
                    g_meterMenu.cursor < kSignatureCount) {
                    StrumSetMeter(kSignatures[g_meterMenu.cursor].beats,
                                  kSignatures[g_meterMenu.cursor].unit);
                }
                MenuHide(g_meterMenu);
                Repaint();
                return;
            }
            if (key == VK_UP || key == VK_DOWN) {
                MenuMoveCursor(g_meterMenu, key == VK_DOWN ? 1 : -1);
                Repaint();
                return;
            }
            return;
        }
        if (MenuHit(g_patternMenu)) {
            if (key == VK_ESCAPE) {
                MenuHide(g_patternMenu);
                Repaint();
                return;
            }
            if (key == VK_RETURN) {
                // Enter loads the pattern the cursor rests on; with no line
                // picked yet it just folds the list away.
                if (g_patternMenu.cursor >= 0) {
                    StrumLoadPatternEntry(g_patternMenu.cursor);
                }
                MenuHide(g_patternMenu);
                Repaint();
                return;
            }
            if (key == VK_UP || key == VK_DOWN) {
                StrumMovePatternCursor(key == VK_DOWN ? 1 : -1);
                Repaint();
                return;
            }
            return;
        }
        if (MenuHit(g_strumMenu)) {
            if (key == VK_ESCAPE) {
                MenuHide(g_strumMenu);
                Repaint();
                return;
            }
            if (key == VK_RETURN) {
                // Enter commits the row the cursor rests on; with no row
                // picked yet it just folds the list away.
                if (g_strumMenu.cursor >= 0) {
                    StrumSetChord(g_strumMenu.cursor);
                }
                MenuHide(g_strumMenu);
                Repaint();
                return;
            }
            // The list is two columns, so up and down step a whole column and
            // left and right walk across it.
            if (key == VK_UP || key == VK_DOWN) {
                MenuMoveCursor(g_strumMenu, key == VK_DOWN ? 2 : -2);
                Repaint();
                return;
            }
            if (key == VK_LEFT || key == VK_RIGHT) {
                MenuMoveCursor(g_strumMenu, key == VK_RIGHT ? 1 : -1);
                Repaint();
                return;
            }
            return;
        }
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        if (key == VK_SPACE) {
            StrumToggle();
            return;
        }
        if (key == VK_RETURN) {
            StrumStart();
            return;
        }
        if (key == VK_LEFT || key == VK_RIGHT) {
            StrumMoveCursor(key == VK_LEFT ? -1 : 1);
            return;
        }
        if (key == VK_UP || key == VK_DOWN) {
            StrumNudgeTempo((key == VK_UP ? 1.0 : -1.0) * (shift ? 10.0 : 1.0));
            return;
        }
        if (key == VK_TAB) {
            StrumStepPreset(shift ? -1 : 1);
            return;
        }
        // The letters lay a stroke on the focused cell; a g t reach the marks
        // that ride beside it, and c clears the whole bar back to rests.
        switch (key) {
            case 'D':
                StrumSetStroke(g_strum.slotCursor, kStrokeDown);
                return;
            case 'U':
                StrumSetStroke(g_strum.slotCursor, kStrokeUp);
                return;
            case 'X':
                StrumSetStroke(g_strum.slotCursor, kStrokeDownMute);
                return;
            case 'M':
                StrumSetStroke(g_strum.slotCursor, kStrokeUpMute);
                return;
            case 'R':
                StrumSetStroke(g_strum.slotCursor, kStrokeRest);
                return;
            case 'A':
                StrumToggleAccent(g_strum.slotCursor);
                return;
            case 'G':
                StrumToggleGhost(g_strum.slotCursor);
                return;
            case 'T':
                StrumToggleTie(g_strum.slotCursor);
                return;
            case 'C':
                StrumClearBar();
                return;
            default:
                return;
        }
    }
    if (g_app.tool == kToolCircle) {
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        if (key == VK_SPACE || key == VK_RETURN) {
            CirclePlayScale();
            return;
        }
        if (key == VK_LEFT || key == VK_RIGHT) {
            // Stepping around the ring stays silent so a run of arrow presses
            // does not spam the synth; Space plays whatever lands selected.
            CircleSelect(g_circle.key + (key == VK_RIGHT ? 1 : -1), false);
            return;
        }
        if (key == VK_UP || key == VK_DOWN || key == 'M') {
            // Up and down step between the two rings of the wheel, keeping the
            // fifth under the hand; M flips to the same spoke's other mode.
            if (key == VK_UP) {
                CircleSetRing(false, false);
            } else if (key == VK_DOWN) {
                CircleSetRing(true, false);
            } else {
                CircleToggleMode(false);
            }
            return;
        }
        if (key == VK_TAB) {
            // Tab walks the instrument chips in the order they are drawn.
            int step = 0;
            for (int i = 0; i < kInstrumentChipCount; i++) {
                if (kInstrumentChipOrder[i] == g_circle.instrument) {
                    step = i;
                    break;
                }
            }
            CircleSetInstrument(
                kInstrumentChipOrder[(step + 1) % kInstrumentChipCount]);
            return;
        }
        // The number keys play the diatonic chord built on that degree.
        if (key >= '1' && key <= '7') {
            CirclePlayChord(static_cast<int>(key - '1'));
            return;
        }
        return;
    }
    if (g_app.tool == kToolTrainer) {
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        if (key == VK_SPACE || key == VK_RETURN) {
            TrainerToggle();
            return;
        }
        if (key == 'R') {
            TrainerResetScore();
            return;
        }
        if (key == VK_TAB) {
            // Tab walks the instrument chips in the order they are drawn.
            int step = 0;
            for (int i = 0; i < kInstrumentChipCount; i++) {
                if (kInstrumentChipOrder[i] == g_trainer.instrument) {
                    step = i;
                    break;
                }
            }
            TrainerSetInstrument(
                kInstrumentChipOrder[(step + 1) % kInstrumentChipCount]);
            return;
        }
        if (key == 'N') {
            g_trainer.naturals = !g_trainer.naturals;
            if (g_trainer.running) {
                TrainerNextNote();
            }
            Repaint();
            return;
        }
        return;
    }
    if (g_app.tool == kToolTuner) {
        if (MenuHit(g_tunerMenu)) {
            if (key == VK_ESCAPE) {
                MenuHide(g_tunerMenu);
                Repaint();
                return;
            }
            if (key == VK_RETURN) {
                // Enter commits the row the cursor rests on, which is what
                // makes the popup behave as a listbox.
                if (g_tunerMenu.cursor >= 0) {
                    const int value =
                        MenuRowValue(g_tunerMenu, g_tunerMenu.cursor);
                    if (g_tunerMenu.target == kMenuInstrument) {
                        TunerSetInstrument(value);
                    } else {
                        TunerSetTuning(value);
                    }
                }
                MenuHide(g_tunerMenu);
                Repaint();
                return;
            }
            if (key == VK_UP || key == VK_DOWN) {
                MenuMoveCursor(g_tunerMenu, key == VK_DOWN ? 1 : -1);
                Repaint();
                return;
            }
            return;
        }
        if (key == VK_ESCAPE) {
            CloseFlyout();
            return;
        }
        if (key == VK_RETURN) {
            // Enter holds the string that is sounding, so the note you just
            // played is the one you go on tuning.
            int cents = 0;
            const int midi = TunerDisplayReading(cents);
            const int string =
                g_tuner.lock >= 0
                    ? g_tuner.lock
                    : (midi >= 0 ? TunerNearestString(midi) : -1);
            if (string >= 0) {
                TunerSetString(g_tuner.lock == string ? -1 : string);
            }
            return;
        }
        if (key == VK_UP || key == VK_DOWN) {
            TunerStepInstrument(key == VK_UP ? -1 : 1);
            return;
        }
        if (key == VK_LEFT || key == VK_RIGHT) {
            TunerStepTuning(key == VK_LEFT ? -1 : 1);
            return;
        }
        // The number keys hold a single string, so only that one is tuned.
        // 0 lets go and reads every string again.
        int stringKey = -1;
        if (key >= '0' && key <= '9') {
            stringKey = key == '0' ? kTunerStringAll
                                   : static_cast<int>(key - '1');
        } else if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
            stringKey = key == VK_NUMPAD0 ? kTunerStringAll
                                          : static_cast<int>(key - VK_NUMPAD1);
        }
        if (stringKey >= 0) {
            if (stringKey < TunerStringCount(g_tuner.tuning) ||
                stringKey == kTunerStringAll) {
                TunerSetString(stringKey);
                Repaint();
            }
        }
        return;
    }
    switch (key) {
        case VK_ESCAPE:
            if (g_app.query.empty()) {
                CloseFlyout();
            } else {
                ClearQuery();
            }
            return;
        case VK_RETURN:
            if (!g_app.query.empty()) {
                ClearQuery();
            }
            return;
        case VK_DELETE:
            DeleteQueryChar();
            return;
        case 'A':
            if (ctrl) {
                SelectAllQuery();
            }
            return;
        case VK_UP:
            SelectResult(g_app.selected - 1, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_DOWN:
            SelectResult(g_app.selected + 1, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_PRIOR:
            SelectResult(g_app.selected - kVisibleRows, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_NEXT:
            SelectResult(g_app.selected + kVisibleRows, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_HOME:
            SelectResult(0, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_END:
            SelectResult(static_cast<int>(g_app.results.size()) - 1, true);
            g_app.voicing = 0;
            Repaint();
            return;
        case VK_LEFT:
            StepVoicing(-1);
            Repaint();
            return;
        case VK_RIGHT:
            StepVoicing(1);
            Repaint();
            return;
        case VK_TAB: {
            const int next = (g_app.instrument + (shift ? kInstrumentCount - 1 : 1)) %
                             kInstrumentCount;
            g_searchFocused = true;
            SetInstrument(next, true);
            Repaint();
            return;
        }
        case VK_BACK:
            EraseQueryChar();
            return;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window procedures

LRESULT CALLBACK PanelWndProc(HWND hwnd,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (message) {
        case WM_MOUSEMOVE:
            OnMouseMove(hwnd, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            return 0;
        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_hover.id != kHitNone) {
                g_hover = HitResult();
                Repaint();
            }
            return 0;
        case WM_LBUTTONDOWN:
            OnMouseDown(hwnd, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            return 0;
        case WM_LBUTTONUP:
            OnMouseUp();
            return 0;
        case WM_MOUSEWHEEL:
            OnWheel(hwnd, wParam,
                    POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            return 0;
        case WM_KEYDOWN:
            // Alt combinations belong to the shell, not to the flyout.
            if (!(lParam & (1 << 29))) {
                // A held tap or start key auto repeats, which would read as a
                // very fast tempo or flicker the transport, so only the first
                // press counts.
                const bool repeat = (lParam & (1 << 30)) != 0;
                const bool singleShot =
                    (g_app.tool == kToolTempo &&
                     (wParam == VK_SPACE || wParam == VK_RETURN)) ||
                    (g_app.tool == kToolMetronome && wParam == VK_SPACE);
                if (repeat && singleShot) {
                    return 0;
                }
                OnKeyDown(wParam);
            }
            return 0;
        case WM_CHAR: {
            const wchar_t c = static_cast<wchar_t>(wParam);
            if (c >= 32 && c != 127) {
                if (TempoEntryType(c)) {
                    return 0;  // A digit went into the tempo field.
                }
                if (g_app.tool == kToolChords && !MenuHit(g_toolMenu)) {
                    AppendQueryChar(c);
                }
            }
            return 0;
        }
        case WM_TIMER:
            if (wParam == kAnimationTimer) {
                UpdateAnimation();
            } else if (wParam == kCaretTimer && g_phase != kPhaseClosed &&
                       (g_searchFocused || TempoEntryActive()) &&
                       g_windowActive) {
                Repaint();  // Moves the blinking caret along.
            } else if (wParam == kFlashTimer) {
                if (NowMs() - g_tap.flashAt > 320.0) {
                    KillTimer(hwnd, kFlashTimer);
                }
                if (g_phase != kPhaseClosed) {
                    Repaint();  // Runs the tap flash out.
                }
            } else if (wParam == kPulseTimer) {
                if ((!g_metro.running && !g_strum.running) ||
                    g_phase == kPhaseClosed) {
                    KillTimer(hwnd, kPulseTimer);
                    return 0;
                }
                if (!g_metroShared.running.load()) {
                    // The output device went away mid-bar, so fall back to
                    // stopped rather than leaving a transport that lies.
                    if (g_strum.running) {
                        StrumStop();
                    }
                    if (g_metro.running) {
                        MetronomeStop();
                    }
                    return 0;
                }
                // Only draw on a fresh pulse, plus a short run of frames for
                // the ring to shrink, so a fast tempo cannot turn into a paint
                // storm. The strumming playhead advances per cell, so it also
                // drives the practice ramp and the play-once stop.
                const unsigned pulse = g_metroShared.pulse.load();
                if (g_strum.running && g_app.tool == kToolStrum) {
                    StrumPracticeStep();
                    if (pulse != g_strum.paintedSlot) {
                        g_strum.paintedSlot = pulse;
                        g_strum.paintedSlotAt = NowMs();
                        Repaint();
                    } else if (NowMs() - g_strum.paintedSlotAt < 140.0) {
                        Repaint();
                    }
                } else if (g_metro.running && g_app.tool == kToolMetronome) {
                    if (pulse != g_metro.paintedPulse) {
                        g_metro.paintedPulse = pulse;
                        g_metro.pulseAt = NowMs();
                        Repaint();
                    } else if (NowMs() - g_metro.pulseAt < 120.0) {
                        Repaint();
                    }
                }
            } else if (wParam == kTunerTimer) {
                if (g_phase == kPhaseClosed || g_app.tool != kToolTuner ||
                    !g_tuner.listening) {
                    KillTimer(hwnd, kTunerTimer);
                } else {
                    TunerPoll();
                    TunerAnimate();
                }
            } else if (wParam == kTrainerTimer) {
                if (g_phase == kPhaseClosed || g_app.tool != kToolTrainer ||
                    !g_trainer.running) {
                    KillTimer(hwnd, kTrainerTimer);
                } else {
                    TrainerTick();
                }
            } else if (wParam == kMenuAnimTimer) {
                if (g_phase == kPhaseClosed || !AnyMenuAnimating()) {
                    // Nothing left to roll, so stop the ticks.
                    KillTimer(hwnd, kMenuAnimTimer);
                    return 0;
                }
                UpdateMenuAnimations();
            }
            return 0;
        case WM_ACTIVATE: {
            // The panel stays on top when you click elsewhere - a metronome or
            // a strumming pattern is no use if it folds away while you play.
            // The x button and the shortcut are the way out, and the option to
            // close on focus loss brings back the old click-away dismissal.
            const bool inactive = LOWORD(wParam) == WA_INACTIVE;
            g_windowActive = !inactive;
            if (inactive) {
                if (g_settings.closeOnFocusLoss && g_phase == kPhaseOpen) {
                    CloseFlyout();
                }
            } else if (g_phase != kPhaseClosed) {
                Repaint();  // Bring the caret back with the focus.
            }
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                LPCWSTR cursor = IDC_ARROW;
                if (g_dragging || g_hover.id == kHitHeader) {
                    cursor = IDC_SIZEALL;
                } else if (g_selecting || g_hover.id == kHitSearch) {
                    cursor = IDC_IBEAM;
                } else if (g_hover.id == kHitTab || g_hover.id == kHitRow ||
                           g_hover.id == kHitClose ||
                           g_hover.id == kHitToolSelect ||
                           g_hover.id == kHitToolMenuItem ||
                           g_hover.id == kHitTapPad ||
                           g_hover.id == kHitTapReset ||
                           g_hover.id == kHitTapHalf ||
                           g_hover.id == kHitTapDouble ||
                           g_hover.id == kHitMetroStart ||
                           g_hover.id == kHitMetroTap ||
                           g_hover.id == kHitMetroTempo ||
                           g_hover.id == kHitMetroPreset ||
                           g_hover.id == kHitMetroBeatsDown ||
                           g_hover.id == kHitMetroBeatsUp ||
                           g_hover.id == kHitMetroUnit ||
                           g_hover.id == kHitMetroSubdivision ||
                           g_hover.id == kHitMetroGroup ||
                           g_hover.id == kHitTunerInst ||
                           g_hover.id == kHitTunerTune ||
                           g_hover.id == kHitTunerMenuRow ||
                           g_hover.id == kHitTunerString ||
                           g_hover.id == kHitStrumTempo ||
                           g_hover.id == kHitPrevious ||
                           g_hover.id == kHitNext || g_hover.id == kHitDot) {
                    cursor = IDC_HAND;
                }
                SetCursor(LoadCursorW(nullptr, cursor));
                return TRUE;
            }
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_NCHITTEST:
            return HTCLIENT;
        case WM_DPICHANGED:
            return 0;
        case WM_CLOSE:
            CloseFlyout();
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ShadowWndProc(HWND hwnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;  // Clicks fall through to the desktop.
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void RepositionPanel() {
    if (g_phase == kPhaseClosed) {
        return;
    }
    if (g_panel.Hwnd()) {
        const RECT work = MonitorWorkArea();
        SetWindowPos(g_panel.Hwnd(), nullptr, work.left, work.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    Render();
    BuildShadow();
    ComputePosition(g_layout.width, g_layout.height, g_x, g_y);
    PresentPanel(true);
}

LRESULT CALLBACK MessageWndProc(HWND hwnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (message) {
        case WM_HOTKEY:
            if (wParam == kHotkeyToggle) {
                ToggleFlyout();
            }
            return 0;
        case kMessageSettings:
            ApplySettings();
            RepositionPanel();
            return 0;
        case WM_SETTINGCHANGE:
            // The app theme and the accent color live in the registry, so a
            // theme switch redraws the panel with the new colors.
            if (g_phase != kPhaseClosed) {
                ApplySettings();
                Render();
                PresentPanel(false);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterWindowClasses() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = g_instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    wc.lpfnWndProc = MessageWndProc;
    wc.lpszClassName = kMessageClass;
    if (!RegisterClassExW(&wc)) {
        return false;
    }

    wc.lpfnWndProc = PanelWndProc;
    wc.lpszClassName = kPanelClass;
    if (!RegisterClassExW(&wc)) {
        return false;
    }

    wc.lpfnWndProc = ShadowWndProc;
    wc.lpszClassName = kShadowClass;
    return RegisterClassExW(&wc) != 0;
}

void UnregisterWindowClasses() {
    UnregisterClassW(kPanelClass, g_instance);
    UnregisterClassW(kShadowClass, g_instance);
    UnregisterClassW(kMessageClass, g_instance);
}

bool CreateWindows() {
    if (!RegisterWindowClasses()) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return false;
    }
    g_messageWindow = CreateWindowExW(0, kMessageClass, L"", 0, 0, 0, 0, 0,
                                      HWND_MESSAGE, nullptr, g_instance, nullptr);
    if (!g_messageWindow) {
        Wh_Log(L"Couldn't create the message window: %u", GetLastError());
        return false;
    }
    if (!g_panel.Create(0, kPanelClass, L"Musician\'s Toolkit", nullptr)) {
        Wh_Log(L"Couldn't create the panel: %u", GetLastError());
        return false;
    }
    if (!g_shadow.Create(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, kShadowClass, L"",
                         nullptr)) {
        Wh_Log(L"Couldn't create the shadow: %u", GetLastError());
    }
    // The shortcut comes from the settings, which ApplySettings has already
    // loaded; now that the message window exists it can actually be hooked.
    ApplyToggleHotkey();
    return true;
}

void DestroyWindows() {
    if (g_messageWindow) {
        UnregisterHotKey(g_messageWindow, kHotkeyToggle);
    }
    g_panel.Destroy();
    g_shadow.Destroy();
    if (g_messageWindow && IsWindow(g_messageWindow)) {
        DestroyWindow(g_messageWindow);  // WM_DESTROY posts the quit.
    }
    g_messageWindow = nullptr;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_fonts.Init();
    ApplySettings();

    if (!CreateWindows()) {
        DestroyWindows();
        UnregisterWindowClasses();
        if (SUCCEEDED(coInit)) {
            CoUninitialize();
        }
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        return 1;
    }
    if (g_workerReadyEvent) {
        SetEvent(g_workerReadyEvent);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Silence the clicks before the windows they report to go away.
    MetronomeHalt();
    // And hand the microphone back on the way out.
    TunerHalt();
    TrainerHalt();
    StrumHalt();
    DestroyWindows();
    UnregisterWindowClasses();
    g_fonts.Clear();
    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points
//
// The mod runs inside a windhawk.exe child process started with -tool-mod, the
// same way the other flyout mods do. The launcher process only spawns it.

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_instance = GetModuleHandleW(nullptr);
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    HANDLE handles[] = {g_workerReadyEvent, g_workerThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    return g_messageWindow != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_messageWindow) {
        PostMessageW(g_messageWindow, kMessageSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_messageWindow) {
        PostMessageW(g_messageWindow, WM_CLOSE, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
}

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

    WCHAR commandLine[MAX_PATH + 2 +
                     (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                     1];
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
    auto createProcessInternal = reinterpret_cast<CreateProcessInternalW_t>(
        GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr,
                               nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr,
                               nullptr, &si, &pi, nullptr)) {
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

// ==WindhawkModData:begin==
// Chord data converted from chords-db (https://github.com/tombatossals/chords-db, MIT).
// Generated by tools/build-chord-data.js. Do not edit by hand.

namespace chorddata {

// Guitar: 828 chords, 3283 voicings, 54692 bytes.
const int kGuitarTuning[] = {40, 45, 50, 55, 59, 64};

const PCWSTR kGuitarKeys[] = {
    L"C", L"C#", L"D", L"Eb", L"E", L"F", L"F#", L"G", L"Ab", L"A", L"Bb", L"B",
};

const PCWSTR kGuitarSuffixes[] = {
    L"major", L"minor", L"dim", L"dim7", L"sus", L"sus2",
    L"sus4", L"sus2sus4", L"7sus4", L"7/G", L"alt", L"aug",
    L"5", L"6", L"69", L"7", L"7b5", L"aug7",
    L"9", L"9b5", L"aug9", L"7b9", L"7#9", L"11",
    L"9#11", L"13", L"maj7", L"maj7b5", L"maj7#5", L"maj7sus2",
    L"maj9", L"maj11", L"maj13", L"m6", L"m69", L"m7",
    L"m7b5", L"m9", L"m11", L"mmaj7", L"mmaj7b5", L"mmaj9",
    L"mmaj11", L"add9", L"madd9", L"add11", L"/E", L"/F",
    L"/F#", L"/G", L"/G#", L"/A", L"/Bb", L"/B",
    L"/C", L"/C#", L"m/B", L"m/C", L"m/C#", L"/D",
    L"m/D", L"/D#", L"m/D#", L"m/E", L"m/F", L"m/F#",
    L"m/G", L"m/G#", L"m9/Bb", L"m9/B", L"m9/C", L"m9/C#",
    L"m9/D", L"m9/Eb", L"m9/E", L"m9/F", L"m9/F#", L"m9/G",
    L"m9/Ab", L"m9/A", L"/Eb", L"/Ab", L"m/A", L"m/Bb",
    L"m/Eb", L"m/Ab",
};

const PCWSTR kGuitarSuffixNames[] = {
    L"", L"m", L"dim", L"dim7", L"sus", L"sus2", L"sus4", L"sus2sus4",
    L"7sus4", L"7/G", L"alt", L"aug", L"5", L"6", L"69", L"7",
    L"7b5", L"aug7", L"9", L"9b5", L"aug9", L"7b9", L"7#9", L"11",
    L"9#11", L"13", L"maj7", L"maj7b5", L"maj7#5", L"maj7sus2", L"maj9", L"maj11",
    L"maj13", L"m6", L"m69", L"m7", L"m7b5", L"m9", L"m11", L"mmaj7",
    L"mmaj7b5", L"mmaj9", L"mmaj11", L"add9", L"madd9", L"add11", L"/E", L"/F",
    L"/F#", L"/G", L"/G#", L"/A", L"/Bb", L"/B", L"/C", L"/C#",
    L"m/B", L"m/C", L"m/C#", L"/D", L"m/D", L"/D#", L"m/D#", L"m/E",
    L"m/F", L"m/F#", L"m/G", L"m/G#", L"m9/Bb", L"m9/B", L"m9/C", L"m9/C#",
    L"m9/D", L"m9/Eb", L"m9/E", L"m9/F", L"m9/F#", L"m9/G", L"m9/Ab", L"m9/A",
    L"/Eb", L"/Ab", L"m/A", L"m/Bb", L"m/Eb", L"m/Ab",
};

const char* const kGuitarChunks[] = {
    "AAAEAf8DAgABAAADAgABAAAD/wEDAwMBAAECAwQBAQEBBQX//wEBAQQAAAEBAQQBAQIECAEDAwIBAQEDBAIBAQEBAAUAAQQB/wMBAAEDAAMCAAEEAAMBAQMDAgEBAQMEAgEBAQAFBQQCAQH//wQCAQEAAAEBAgMIAQMDAQEBAQMEAQEBAQEABQACBAH/AwH/AQIABAEAAgMAA/8BAgMC/wABAgQDAAAGAwH/AwL/AwEABAIAAAr//wEC/wIAAAECAAMAAAMEAf//AQIBAgAAAQMCBAAB/wMEAgQCAAIDAQQBAQIDBQcC/wECAf8CAAEDAQABAQIECv//AQIBAgAAAQMBBAEBAgQABAQB/wMDAAEBAAMEAAEBAQEEBQH/AwMAAQMAAgMAAQQAA/8BAQMEAQABAQMEAQEBAQUD/wEDAwQBAAECAwQBAQEBBQAFBAH/AwAAAQMAAwAAAQQAAf8DAAADAwABAAACAwADAQEDAwEBAQEDBAEBAQEABQcC/wABAgICAAABAwQAAAYEAf8DAwABAQADBAABAQEBBAUDAQEDAwQBAQECAwQBAQEABQYDA/8AAQMCAwAAAQQACAEDAwMBAQECAwQBAQEBAAUABwMB/wMDAAMDAAECAAMEAAH/AwAAAQEAAwAAAQEBAQQFA/8BAQMBAQABAQMBAQEBAQUACAQB/wMDAwEBAAIDBAEBAQEEBQMBAQMBBAEBAQMBBAEBAQAFBf//AQECAgAAAQECAwEBAgMIAQMBAwEBAQMBBAEBAQEABQAKBAL/AgEEBAEAAgEDBAEBAQIFA/8BAgMDAAABAgMEAAAH//8EAwECAAAEAwECAAr//wECBAMAAAECBAMAAAsEAf8DAgEB/wADAgEBAAEBAwQD/wEEAwP/AAEEAgMAAAUEAwIBAf8EAwIBAQABAQMECP//AwICAQAABAIDAQAADAMIAQP/////AQMAAAAAAAP/AQP///8AAQMAAAAACAEDA////wEDBAAAAAAADQQB/wMCAgEAAAQCAwEAAAP/AQMDAwMAAQMDAwQBAwIFBwL/AQMC/wIAAQQDAAAIAf8DAgMBAQADAgQBAQEABQAOBAH/AwICAwMAAwEBAwQBAgIDA/8BAAADAwABAAADBAAHAgEBAQICAgEBAQMEAQEBAwn/AgIBAgIAAgIBAwQBAgEFAA8EAf8DAgMBAAADAgQBAAAD/wEDAQMBAAEDAQQBAQEBBQX//wEBAQIAAAEBAQIBAQIECAEDAQIBAQEDAQIBAQEBAAUAEAQB//8CAwECAAACBAEDAAP/AQIBA/8AAQIBAwABAQEDBwL/AgMBAAIAAwQBAAAK//8BAgIDAAABAgMEAAAJAwEDAwID//8CAwEEAAAAAQMBAgABAAQCAwABAAADAQEDAQMBAQEDAQQBAQEABQARBAH/AwID/wQAAgEDAAQAA/8BBAEDAgABBAEDAgEBAQMIAf8BAgIAAQACAwQAAAr//wEEAgMAAAEEAgMAABIFAQADAgADAAACAwAEAAABAwMCAwMDAgIBAwMEAQMABQcCAQIBAgICAQMBBAQBAQEDBwIEAgECBAEDAQIBBAECAAQJ//8CAQMCAAACAQQDAAATBAH/AwIDAwIAAgEDBAEBAgIFAf8DBAMDAAABBAIDAAAHAgECAQECAgEDAQEEAQEBBAgBAgEC/wMBAgEDAAQBAQACABQEAf8DAgMDBAACAQMDBAEDAQQD/wEAAQMCAAEAAgQDAAUCAQIBAQICAQMBAQQBAQEEBwIBAgED/wIBAwEEAAEBAQMAFQQB/wMCAwIDAAIBAwEEAQICBAYDAgMB//8DAgQBAAAACAH/AQIBAgEAAQIBAwEBAAQJ//8CAQMBAAADAQQCAAAWBAH/AwIDBP8AAgEDBAAAA/8BAwECAAABAwECAAEBAQMIAQMBAgEEAQMBAgEEAQEABAn//wIBAwMAAAIBAwQAABcEAf8DAgMBAQADAgQBAQEBBAUD/wEBAQMBAAEBAQMBAQEBBQYDAgAAAQEDAgAAAQEBAQQFCAEBAQIBAQEBAQIBAQEBAAUAGAQB/wMCAwMCAAIBAwQBAQICBQP/AQIBAwEAAQIBAwEBAQEFBwIBAgEBAgIBAwEBBAEBAQQK//8BAgIDAAABAgMEAAAZBAL/AgECBAQAAgEDBAQBBAQFAwEBAQEDAwEBAQEDBAEBAAMGAwICAgMBAwICAgQBAQIBAwgBAwECAwEBAwECBAEBAQAFABoEAQMDAgAAAAIDAQAAAAADAQEDAgMBAQEDAgQBAQEABQX//wEBAQMAAAEBAQQBAQIECv//AQMDAwAAAQMDAwEDAwUAGwQB/wMCBAACAAMBBAACAAP/AQICA/8AAQIDBAAABwIBAwMBAQIBAwQBAQEBAQUK//8BAgMDAAABAgMEAAAcBAH/AwIBAAAAAwIBAAAAA/8BBAIDAAABBAIDAAAFBAMCAQAABAMCAQAAAAgBBAMCAAABBAMCAAAAAB0EAf8DAAQBAwACAAQBAwAB/wMAAAADAAEAAAACAAH/AwAEAAMAAQADAAIAA/8BAwIBAQABAwIBAQEBAQUAHgQB/wMAAAAAAAMAAAAAAAEDAwIEA/8CAgEEAwABAwAEBQABAQEBAwABAQEBAwEBAQQIAf8CAgEDAQACAwEEAQEABAAfBAH/AwIAAAEAAwIAAAEAAf8DAwAAAAABAQAAAAEDAQIGAwIEAAEAAwIEAAEAAAgBAQICAQEBAQIDAQEBAQAFACAEAf8DAgIAAQAEAgMAAQAD/wEBAgMDAAEBAgMEAQEBAgcCAQEBAgECAQEBAwEBAQEFCAEBAgIDAQEBAgMEAQEBAAUAIQQB/wMBAgEDAAMBAgEEAQECBAP/AQP/AgMAAQMAAgQABwL/AQICAgIAAQMDBAECAAUIAQMDAQMBAQIDAQQBAQEABQAjBQgB/wEBAf8CAAMDAwAAAf8DAQME/wACAQMEAAADAQEDAQIBAQEDAQIBAQEABQT//wICAQMAAAIDAQQACAEDAQEBAQEDAQEBAQEBAAUAJAQB/wMEAwT/AAEDAgQAAAT//wECAQMAAAECAQQBAQIECAECAwEEAQECAwEEAQEBAAUK//8BAgICAAABAwMDAQIDBQAlBAH/AwEDAwMAAgEDBAQBAwEFAf8DAAMEAwABAAIEAwAG/wEDAgMDAAEDAgQEAQMCBQgBAwEBAQMBAwEBAQQBAQAEACIEAf8DAQIDAwADAQIEBAEDAQUD/wEAAAIDAAEAAAIEAAYDAQIC//8EAQIDAAAACP8DAwEDAwACAgEDBAEDAQUAJgQB/wMBAwMBAAIBAwQBAQECBQH/AwMDBAMAAQEBAgEBAwEFBgMBAwIBAQMBBAIBAQEBAQUIAQEBAQEDAQEBAQEEAQEABAAnBAH/AwEAAP8AAwEAAAAAAwEBAwICAQEBBAIDAQEBAAUIAQMCAQEBAQMCAQEBAQEABQr//wEDAwIAAAEDBAIAACgEAf8D/wQEAgACAAMEAQAB/wMEBAT/AAECAwQAAAgBAgIB/wEBAgMBAAEBAQAFCv//AQIDAgAAAQIEAwAAKQQB/wMBBAP/AAIBBAMAAAH/AwAEBAMAAQADBAIABgMB/wIAAwMBAAIABAAIAQMCAQEDAQMCAQEEAQEABAAqBAH/AwEAAAEAAwEAAAIAAQMDAwQEAwEBAQIDAQEDAAUIAQECAQEDAQECAQEEAQEABAr//wEBAwIAAAEBAwIBAQIDACsEAf8DAgADAAACAQADAAAB/wMAAAMAAAEAAAMAAAcCAQAAAgACAQAAAwAACP//AwIBAwAAAwIBBAAALQMB/wMCAAEBAAMCAAEBAQEEBQUEAwEBAv8EAwEBAgABAQIDBwIBBAT//wIBBAQAAAEEAgMALAQB/wMBAAMDAAIBAAMEAAP/AQADAgEAAQAEAwIABgMBAAIDAwMBAAIEBAEDAAUI//8DAQEDAAADAQEEAQEDBABEBAMEAQMBAQEEAQMBAQEBAQEFAwQBAwMBAQQBAgMBAQEBAQUK/wQBAwIBAAQBAwIBAQECBQMEAQMBAf8EAQMBAQABAQEEAEkCCgIBAQMCAQIBAQQDAQEBAQUKAv8BAwIBAgABBAMBAQECBQBSBAH/AAEAAQMAAAEAAgQAA/8AAwMCAQAAAwQCAQAF/wABBAQEAAABBAQEAQQDBQUBAgEBBAQBAgEBBAQBAQADAFUEA///BAMCAQAABAMCAQAG//8BAwMDAAABAwMDAQMDBQj/BAMBAQEABAMBAQEBAQMFC/8BAwIDAQABAwIEAQEBAQUAOAQB/wIBAAEDAAMBAAIEAAUDAgEBBAQDAgEBBAQBAQIDBQMCAQQEBAMCAQQEBAEEAwUI//8CAQEBAAACAQEBAQEDBQBTBAj//wEBAQEAAAEBAQEBAQIFAf8BAQAB/wABAgADAAAFAgIBAf//AgMBAQAAAQECAwUC/wEEBAQCAAEEBAQBBAMFADoECAIDAwEBAQIDBAEBAQEBAwUI//8EAQEBAAAEAQEBAQEDBQv//wECAwEAAAECAwEBAQIFAf8EAQAB/wAEAQACAAAAPAQD//8AAwIBAAAAAwIBAAX/AQEEBAQAAQEEBAQBAQECCP//AAEBAQAAAAEBAQEBAwUIAwMDAQEBAgMEAQEBAQEDBQA/BAL//wEEAwIAAAEEAwIAAwABAwMCAQABAwQCAQEBAQUFAAIBAQQEAAIBAQQEAQECAwUAAgEEBAQAAgEEBAQBBAMFAFQEAf//AQABAwAAAQACBAAF/wIBAQQEAAIBAQQEAQECAwX/AgEEBAQAAgEEBAQBBAMFCAQDAwEBAQQCAwEBAQEBAwUAQAQD//8BAwIBAAABAwIBAQECBQj/AQMBAQEAAQMBAQEBAQEFCP8BAwEBBAABAwEBBAEBAQQI/wEDAQH/AAEDAQEAAQEBBABBBAP//wIDAgEAAAIEAwEACP8CAwEBAQACAwEBAQEBAwUI/wIDAQEEAAIDAQEEAQEDBAECAwEAAf8DBAEAAgAAAEIEAwEBAwMCAQEBAwQCAQEBAAUD//8DAwIBAAADBAIBAAX//wEEBAQAAAEEBAQBBAMFCP8DAwEBAQADBAEBAQEBAwUAMwQB/wACAAEAAAACAAEAAAH/AAIAAQMAAAIAAQMAA/8AAwMDAQAAAgMEAQAF/wABAQEEAAABAQEEAQECBABRBAEEAwIAAQAEAwIAAQAAA///BAMDAQAABAIDAQAG//8BBAMDAAABBAIDAAj/BAMCAQEABAMCAQEBAQQFADUEAf8CAgABAAACAwABAAAB/wICAAEDAAIDAAEEAAUDAwEBAQQCAwEBAQQBAQIEBQMDAQEEBAIDAQEEBAEBAgMANAQB/wECAAEAAAEDAAIAAAH/AQIAAQMAAQMAAgQABQIDAQEBBAIDAQEBBAEBAgQFAgMBAQQEAgMBAQQEAQECAwA3BAH/BAIAAQAABAIAAQAAAf8EAgABAwAEAgABAwAI//8EAgEBAAAEAgEBAQEEBQv//wECAwIAAAECBAMAADsEAf//AAABAAAAAAABAAAD//8AAwMBAAAAAwQBAAX/AQEBAQQAAQEBAQQBAQEECP//AAIBAQAAAAIBAQEBBAUALgQBAAMCAAEAAAMCAAEAAAEAAwIAAQMAAwIAAQQAAf//AgABAAAAAgABAAAB//8CAAEDAAACAAEDAABQBAH//wEAAQAAAAEAAgAABf8CAQEBBAACAQEBBAEBAgQF/wIBAQH/AAIBAQEAAQECBAkDAgIB//8EAgMBAAAAAC8EAf//AwABAAAAAwABAAAD//8BAwMBAAABAwQBAQECBQj/AQMCAQEAAQMCAQEBAQEFBf8EAQEB/wAEAQEBAAEBAgQAMAQBAgMCAAEAAgQDAAEAAAH//wQAAQAAAAQAAQAAA///AgMDAQAAAgMEAQAI/wIDAgEBAAIEAwEBAQEEBQAxBAEDAwIAAQADBAIAAQAAAwEBAwMDAQEBAgMEAQEBAAUD//8DAwMBAAACAwQBAAX//wEBAQQAAAEBAQQBAQIEAQAEAf8EAwECAQAEAwECAQEBAwUEAQEDAwMBAQECAwQBAQEABQYEAwEBAQQDAgEBAQQBAQIECQEDAwIBAQEDBAIBAQEBAAUBAQQB/wQCAQL/AAQCAQMAAAQBAQMDAgEBAQMEAgEBAQAFBgQCAQH/BAMCAQEABAEBAgMJAQMDAQEBAQMEAQEBAQEABQECBAH/BAL/AgMABAEAAgMABP8BAgMC/wABAgQDAAAHAwH/AwL/AwEABAIAAAv//wEC/wIAAAECAAMAAQMEAf//AgMCAwAAAQMCBAAD/wIDAQMBAAIDAQQBAQEDBQgC/wECAf8CAAEDAQABAQIEC///AQIBAgAAAQMCBAABBAQE/wEBAwQBAAEBAwQBAQEBBQT/AQMDBAEAAQIDBAEBAQEFCQEBAwMBAQEBAwQBAQEBAAUJAQMDAwEBAQIDBAEBAQEABQEFBAQBAQMDAQEBAQMEAQEBAQAFBgQBAQME/wABAAACAwEBAQIJAQMD/wEDAQIDAAEEAQEABAsBAQEDBAEBAQEDBAEBAQAFAQYEAf8EBAEC/wADBAECAAAEAQEDAwQBAQECAwQBAQEABQYE/wEBAgQCAwAAAQQACQEDAwMBAQECAwQBAQEBAAUBBwIE/wEBAwEBAAEBAwEBAQEBBQkBAQMDAQMBAQIDAQQBAQAEAQgEAf8EBAQCAgACAwQBAQECBAUEAQEDAQQBAQEDAQQBAQEABQb///8BAgIAAAABAgMACQEDAQMBAQEDAQQBAQEBAAUBCgQB/wQDAAIBAAQDAAIBAAP/AgMABAEAAgMABAEABP8BAgMD/wABAgMEAAAI//8EAwECAAAEAwECAAELBAH/BAQEAgIAAgMEAQEBAgQFBAEBAwEEAQEBAwEEAQEBAAUGBAMCAQH/BAMCAQEAAQEDBAkB/wMCAgEBAAQCAwEBAQAFAQwDCQED/////wEDAAAAAAAE/wED////AAEDAAAAAAkBAwP///8BAwQAAAAAAQ0EAf8EAwMC/wAEAgMBAAAE/wEDAwMDAAEDAwMDAQMCBQYEAwMBAQEEAgMBAQEBAQMFCQH/AwIDAQEAAwIEAQEBAAUBDgQB/wQBAwIBAAQBAwIBAQECBQH/BAMDBAQAAgEBAwQBAwIDCAIBAQECAgIBAQEDBAEBAQMK//8CAQICAAACAQMEAAEPBAH/BAMEAv8AAwIEAQAABP8BAwEDAQABAwEEAQEBAQUGBAMBAQECBAMBAQECAQECBAkBAwECAQEBAwECAQEBAQAFARAEAf8EAwAAAQAEAwAAAQAE/wECAQP/AAECAQMAAQEBAwgC/wIDAf8CAAMEAQAADP//AQICAwAAAQIDBAABEQQB/wQDAgABAAQDAgABAAT/AQQBAwIAAQQBAwIBAQEDBwMCAf8AAQQDAQAAAgAJAf8BAgL/AQACAwQAAAESBAEEBAMEBAQCAgEDAwQBBAAFCAIBAgH//wMBBAIAAAAJAQMBAgEDAQMBAgEEAQEABAr//wIBAwIAAAIBBAMAARMDAf8EAwQEAwACAQMEAQEDAgUIAgECAQECAgEDAQEEAQEBBAkBAv8CAAMBAgADAAQAARQEA/8CAQICAwACAQMDBAECAQQHAwIBAgD/BAIBAwAAAAgCAQIBA/8CAQMBBAABAQEDCQEEAQIAAwEEAQIAAwEBAAIBFQQB/wQDBAMEAAIBAwEEAQMCBAT/AQABAwQAAQACAwQABwMCAwEA/wMCBAEAAAAJ//8BAgECAAABAwIEAAEWBAH/BAMEAgAAAwIEAQAAA/8CAQID/wACAQMEAAAI/wECAgICAAECAgMEAQICBQkBAwECAQQBAwECAQQBAQAEARcEAf8EAwAABAACAQAAAwAE/wECAQMBAAECAQMBAQEBBQgCAQIBAQICAQMBAQQBAQEECQECAQIBAQECAQMBAQEBAAUBGAQB/wMCAAADAAIBAAADAAT/AQIBAwEAAQIBAwEBAQEFCAIBAgEBAgIBAwEBBAEBAQQJAQIBAgEBAQIBAwEBAQEABQEZBAH/BAMDAAIABAIDAAEABAEBAQEDAwEBAQEDBAEBAAMHAwICAgMBAwICAgQBAQIBAwkBAQECAwMBAQECAwQBAQACARoEAf8EAwEBAQAEAwEBAQEBAwUEAQEDAgMBAQEDAgQBAQEABQb///8BAQMAAAABAQMBAQMECQH/AgIB/wEAAwQCAAABGwQD/wIBAwQBAAIBAwQBAQECBQT/AQICA/8AAQIDBAAACAIBAwMBAQIBAwQBAQEBAQUL//8BAgMDAAABAgMEAAEcBAEBBAMCAQEBBAMCAQEBAQAFBP8BBAID/wABBAIDAAAI//8EAwMBAAAEAgMBAAv//wEEAwMAAAEEAgMAAR0EBP8BAwIBAQABAwIBAQEBAQUI//8EAQIBAAAEAQIBAQEDBQv//wEDAwEAAAEDBAEBAQIFAf8EAQEB/wAEAQEBAAEBAgQBHgQB/wQBAQEBAAQBAQEBAQECBQP/AgEDAv8AAgEEAwAACAIBAwECAQIBBAEDAQEBAQUK//8CAQQCAAACAQQDAAEfBAL/AwIEAQEAAwIEAQEBAQQFBAEBAQIDAQEBAQIDAQEBAAUHAwL//wECBAIAAAEDAAkBAQICAQEBAQIDAQEBAQAFASAEAf8EAQMBAQAEAgMAAQAE/wEBAgMDAAEBAgMEAQEBAggCAQEBAgECAQEBAwEBAQEFCQEBAgIDAQEBAgMEAQEBAAUBIQQB/wQCAwIEAAMBAgEEAQICBAX//wICAQIAAAIDAQQACAL/AQIC/wIAAQMEAAAJAQMDAQMBAQIDAQQBAQEABQEjBQkB/wEBAf8CAAMDAwAABP8BAwECAQABAwECAQEBAQUF//8CAgEDAAACAwEEAAkBAwEBAQEBBAEBAQEBAQAFC///AQMCAgAAAQQCAwABJAQE/wECAQL/AAEDAgQAAAX//wECAQMAAAECAQQBAQIECAL/AgIB/wIAAwQBAAAL//8BAgICAAABAgICAQIDBQElBAH/BAIEBAQAAgEDBAQBBAEFBf8CAgIBAwACAgMBBAECAQMH/wEDAgMDAAEDAgQEAQMCBQkBAwEBAQMBAwEBAQQBAQAEASIEAf8EAQMCAAAEAQMCAAAB/wQCAwT/AAMBAgQAAAcDAQIC/wAEAQIDAAAACf8DAwEDAwACAgEDBAEDAQUBJgQB/wQCBAICAAIBAwEBAQICBQcDAQMCAQEDAQQCAQEBAQEFCQEBAQEBAwEBAQEBBAEBAAQL//8BAQICAAABAQIDAQECAwEnBAH/BAIBAf8ABAIBAQABAQMEBAEBAwICAQEBBAIDAQEBAAUJAQMCAQEBAQMCAQEBAQEABQv/AQEDAwIAAQEDBAIBAQECASgEAf8EAgABAAAEAgABAAAE/wECAgL/AAECAwQAAAkBAgIB/wEBAgIBAAEBAQAFC///AQIDAgAAAQIEAwABKQQB/wQBAQEAAAQBAQEAAQECBAL/AwEEAwAAAgEEAwAABAEBAwIBAAEBBAMCAAEBAAQJAQMCAQEDAQMCAQEEAQEABAEqBAL/AwEEAwEAAwEEAwEBAQIFBAEBAQICAQEBAQIDAQEBAAUJAQECAQEDAQECAQEEAQEABAv//wEBAwIAAAEBAwIBAQIDASsEAf8EAwEEAQADAgEEAQEBAwUB/wQD/wQEAAIBAAMEAAgCAf8BAv8DAQACBAAACf//AwIBAwAAAwIBBAABLQMB/wQDAQICAAQDAQICAQIEBQYEAwEBAv8EAwEBAgABAQIDCAIBBAT//wIBBAQAAAEEAgMBLAQB/wQCAQT/AAMCAQQAAAT/AQMDAQAAAQMEAgAABwMB/wIDAAMBAAIEAAAJ//8DAQEDAAADAQEEAQEDBAFFBAQEAQMBAQEEAQMBAQEBAQEFBAQBAwMBAQQBAgMBAQEBAQUL/wQBAwIBAAQBAwIBAQECBQQEAQMBAf8EAQMBAQABAQEEAUoCCwIBAQMCAQIBAQQDAQEBAQULAv8BAwIBAgABBAMBAQECBQFSBAH/AAIBAgAAAAIBAwAABP8AAwMCAQAAAwQCAQAE//8EAwIBAAAEAwIBAAb/AAEEBAQAAAEEBAQBBAMFAVUEBAEBAwMCAQEBAwQCAQEBAAUE//8DAwIBAAADBAIBAAb//wEEBAQAAAEEBAQBBAMFCf8DAwEBAQADBAEBAQEBAwUBOAQB/wICAQIAAAIDAQQAAAn//wEBAQEAAAEBAQEBAQIFAf8CAgEC/wACAwEEAAAGAgIBAf//AgMBAQAAAQECAwFTBAH/AQIBAgAAAQMCBAAABgECAQEEBAECAQEEBAEBAAMGAQIBBAQEAQIBBAQEAQEAAgj//wECAgIAAAECAgIBAgMFATkEAf8DAgECAAAEAgEDAAAGAwIBAQQEAwIBAQQEAQECAwYDAgEEBAQDAgEEBAQBBAMFCf//AgEBAQAAAgEBAQEBAwUBPAQB//8AAQIAAAAAAQIAAAT//wADAgEAAAADAgEACf//AAEBAQAAAAEBAQEBAwUJAgMDAQEBAgMEAQEBAQEDBQE/BAH//wIBAgAAAAIBAwAABAABAwMCAQABAwQCAQEBAQUGAAIBAQQEAAIBAQQEAQECAwYAAgEEBAQAAgEEBAQBBAMFAVQEAf//AQECAAAAAQIDAAAG/wEBBAQEAAEBBAQEAQEBAgkDAwMBAQECAwQBAQEBAQMFCwEBAQMEAgEBAQMEAgEBAAIBQAQB//8DAQIAAAADAQIAAAP//wEEAwIAAAEEAwIABv8DAQQEBAADAQQEBAEEAwUBAQQCAf//AQQCAQAAAQEAAwFBBAH//wQBAgAAAAQBAgAABP//AQMCAQAAAQMCAQEBAgUJ/wEDAQEBAAEDAQEBAQEBBQn/AQMBAQQAAQMBAQQBAQEEAUIEBP//AgMCAQAAAgQDAQAJ/wIDAQEBAAIDAQEBAQEDBQn/AgMBAQQAAgMBAQQBAQMECf8CAwEB/wACAwEBAAEBAwQBMwQB/wADAQIBAAADAQIBAQEDBQH/AAMBAgQAAAMBAgQABP8AAwMDAQAAAgMEAQAE//8EAwMBAAAEAgMBAAFRBAQBAQMDAwEBAQIDBAEBAQAFBP//AwMDAQAAAgMEAQAG//8BAQEEAAABAQEEAQECBAn/AwMCAQEAAwQCAQEBAQQFATUEAf8CAwECAQACBAEDAQEBAwUGAgMBAQEEAgMBAQEEAQECBAYCAwEBBAQCAwEBBAQBAQIDCf//AQIBAQAAAQIBAQEBAgUBNAQB/wEDAQIBAAEDAQIBAQEBBQH/AQMBAgQAAQMBAgQBAQEDBgEDAQEBBAEDAQEBBAEBAAQGAQMBAQQEAQMBAQQEAQEAAwE2BAH/AwMBAgEAAwQBAgEBAQMFBgMDAQEBBAIDAQEBBAEBAgQGAwMBAQQEAgMBAQQEAQECAwn//wICAQEAAAIDAQEBAQQFATsEAf//AAECAQAAAAECAQEBAwUE//8AAwMBAAAAAwQBAAn//wACAQEAAAACAQEBAQQFCf//BAIBAQAABAIBAQEBBAUBLgQBAAQDAQIBAAQDAQIBAQEDBQH//wIBAgEAAAIBAwEBAQMFBAABAwMDAQABAgMEAQEBAQUGAAMBAQEEAAMBAQEEAQECBAFQBAH//wEBAgEAAAEBAgEBAQIFBv8BAQEBBAABAQEBBAEBAQQLAQEBAwQDAQEBAgQDAQEAAgb/AQEBAf8AAQEBAQABAQEEAS8EAQEEAwECAQEEAwECAQEBAAUBAf//AQIBAQEBAQIBAQEABQH//wMBAgEAAAMBAgEBAQMFAf//AwECBAAAAwECBAABMAQB//8EAQIBAAAEAQIBAQEDBQT//wEDAwEAAAEDBAEBAQIFCf8BAwIBAQABAwIBAQEBAQUG/wQBAQH/AAQBAQEAAQECBAExBAT//wIDAwEAAAIDBAEACf8CAwIBAQACBAMBAQEBBAUDAQIEBAT/AQIEBAQAAQQCBAn/AgMCAf8AAgQDAQAAAgAEAf//AAIDAgAAAAEDAgAC/wQDAQIBAAQDAQIBAQEDBQX/AQMDAwEAAQIDBAEBAQEFCgEDAwIBAQEDBAIBAQEBAAUCAQQB//8AAgMBAAAAAgMBAAUBAQMDAgEBAQMEAgEBAQAFBv8DAgIB/wAEAgMBAAAKAQMDAQEBAQMEAQEBAQEABQICBAH//wAB/wEAAAABAAIAA/8DAf8BAgAEAQACAwAF/wECAwL/AAECBAMAAAgDAf8DAv8DAQAEAgAAAgMEAf//AAEAAQAAAAIAAwAE/wIDAQMBAAIDAQQBAQEDBQf/AgAEAwEAAgAEAwEACgECAwEDAQECAwEEAQEBAAUCBAQB//8AAgMDAAAAAQICAQMEBQX//wADBAEAAAADBAEABf8BAQMEAQABAQMEAQEBAQUF/wEDAwQBAAECAwQBAQEBBQIFBAH//wACAwAAAAACAwAAAv//AQECBAAAAQECBAEBAgMFAQEDAwEBAQEDBAEBAQEABQf/AQEBBAQAAQEBBAQCAQEDBAQFAgYEAf//AAIDAwAAAAECAwAD/wMAAAEDAAMAAAEEAAUBAQMDBAEBAQIDBAEBAQAFCgEDAwMBAQECAwQBAQEBAAUCBwMF/wEBAwEBAAEBAwEBAQEBBQL/BAEBAgIABAEBAgICAQIDAgQFCgEBAwMBAwEBAgMBBAEBAAQCCAQB//8AAgEDAAAAAgEEAAP/AwMDAf8AAgMEAQAABQEBAwEEAQEBAwEEAQEBAAUKAQMBAwEBAQMBBAEBAQEABQIKBAH//wABAwIAAAABAwIAA/8DAv8BAgAEAgABAwAF/wECAwP/AAECAwQAAAn/AwADAQIAAwAEAQIAAgsEAf//AAMDAgAAAAIDAQAD/wMCAQH/AAMCAQEAAQEDBAcEAwIBAf8EAwIBAQABAQMECgH/AwIC/wEABAIDAAACDAMKAQP/////AQMAAAAAAAX/AQP///8AAQMAAAAACgEDA////wEDBAAAAAACDQQB//8AAgACAAAAAgADAAP/AwICAf8ABAIDAQAABf8BAwMDAwABAwMDBAEDAgUHAQMBAQQBAQMBAQQBAQEABQIOBAL/BAMBAAAABAMBAAAABP8CAQECAgACAQEDBAEBAgMJAgEBAQICAgEBAQMEAQEBAwv/AgIBAgIAAgIBAwQBAgEFAg8EAf//AAIBAgAAAAIBAwAD/wMCAwH/AAMCBAEAAAUBAQMBAwEBAQMBBAEBAQAFCgEDAQIBAQEDAQIBAQEBAAUCEAQB//8AAQECAAAAAQECAQEDBAP//wIDAQIAAAIEAQMABf8BAgED/wABAgEDAAEBAQMJAv8CAwH/AgADBAEAAAIRBAH//wADAQIAAAAEAQIABP8CAQL/AwACAQMABAAF/wEEAQMCAAEEAQMCAQEBAwoB/wECAv8BAAIDBAAAAhIEBAICAQICAgICAQMDBAECAAUH/wEAAQECAAEAAgMEAAkCAQIBAv8CAQMBBAABAQEDCgEDAQIBAwEDAQIBBAEBAAQCEwQE/wIBAgIBAAIBAwQBAQECBQX/AQIBAwAAAQMCBAAACQIBAgEBAgIBAwEBBAEBAQQKAQIBAv8DAQIBAwAEAQEAAgIUBAT/AgECAgMAAgEDAwQBAgEEBf8DAAEDAgADAAEEAgAJAgECAQP/AgEDAQQAAQEBAwoB/wECAgABAAIDBAAAAhUEAv//AAQDAQAAAAQDAQAE/wIBAgECAAIBAwEEAQECBAX/AgABAwEAAgABBAEBAQMFCAMCAwH//wMCBAEAAAACFgQE/wIBAgP/AAIBAwQAAAcAAAAEAQIAAAAEAQIAB/8CAQEBAgACAQEBAwEBAgQKAQMBAgEEAQMBAgEEAQEABAIXBAH//wAAAQIAAAAAAQIAA/8DAgMBAQADAgQBAQEBBAUF/wEBAQMBAAEBAQMBAQEBBQf/AwEBAgIBAQECAQEBAgQFAhgEAf//AAEBAgAAAAECAwAE/wIBAgIBAAIBAwQBAQECBQf/AwABAwIAAwABBAIACQIBAgEBAgIBAwEBBAEBAQQCGQQB//8ABAECAAAABAECAAUBAQEBAwMBAQEBAwQBAQADCAMCAAIAAQQCAAMAAQAKAf8BAgP/AQACAwQAAAIaBAH//wACAgIAAAABAQEBAgMFAv8EAwEBAQAEAwEBAQEBAwUFAQEDAgMBAQEDAgQBAQEABQf//wEBAQMAAAEBAQQBAQIEAhsEAf//AAECAgAAAAECAwAF/wECAgP/AAECAwQAAAkCAQMDAQECAQMEAQEBAQEFCgECAgL/AQECAwQAAQEBAAUCHAQB//8AAwICAAAABAIDAAIBBAMCAQEBBAMCAQEBAQAFBf8BAAIDAgABAAIEAwAJ//8AAwMBAAAAAgMBAAIdBAH//wACAgAAAAABAgAABf//AAIBAQAAAAIBAQEBBAUF/wEDAgEBAAEDAgEBAQEBBQn//wABAgEAAAABAgEBAQMFAh4EAv8EAQEBAQAEAQEBAQEBAgUE/wIBAwH/AAIBBAMAAAf/AwADAQMAAgADAQQACgH/AgIBAwEAAgMBBAEBAAQCHwQB//8AAAICAAAAAAEBAQIEBQUBAQECAwEBAQECAwEBAQAFB/8DAAECAwADAAECBAAKAQECAgEBAQECAwEBAQEABQIgBAH//wAEAgIAAAADAQEBAgQFAv8EAwMBAAAEAgMBAAAF/wEBAgMDAAEBAgMEAQEBAgkCAQEBAgECAQEBAwEBAQEFAiEEAf//AAIAAQAAAAIAAQAD/wMBAgEDAAMBAgEEAQECBAX/AQP/AgMAAQMAAgQACQL/AQICAgIAAQMDAwECAAUCIwUKAf8BAQH/AgADAwMAAAH//wACAQEAAAACAQEBAQQFBf8BAwECAQABAwECAQEBAQUG//8CAgEDAAACAwEEAAoBAwEBAQEBAwEBAQEBAQAFAiQEAf//AAEBAQAAAAEBAQEBAwUD/wMBAwECAAMBBAECAQECBAX/AQIBAv8AAQMCBAAACP8BAAMCAQABAAQDAgACJQQBAQAAAgEAAQAAAwIAAAP/AwEDAwMAAgEDBAQBAwEFBf8BAwECAAABBAIDAAAKAQMBAQEDAQMBAQEEAQEABAIiBAL/BAIBAAAABAIBAAAAA/8DAQIDAAADAQIEAAAG/wIAAgECAAIAAwEEAAr/AwMBAwMAAgIBAwQBAwEFAiYEAf//AAABAQAAAAABAQEBBAUD/wMBAwMBAAIBAwQBAQECBQgDAQMCAQEDAQQCAQEBAQEFCgEBAQEBAwEBAQEBBAEBAAQCJwQB//8AAgIBAAAAAgMBAAL/BAIBAQAABAIBAQABAQMEBQEBAwICAQEBBAIDAQEBAAUKAQMCAQEBAQMCAQEBAQEABQIoBAH//wABAgEAAAABAwIABAECAwMD/wECAwMDAAEDAgQF/wECAgL/AAECAwQAAAoBAgIB/wEBAgIBAAEBAQAFAikEA/8DAQQDAAACAQQDAAAF/wMAAgIBAAQAAgMBAAj/AQACAwIAAQACBAMACgEDAgEBAwEDAgEBBAEBAAQCKgQB//8AAAIBAAAAAAIBAAP/AwEEAwEAAgEEAwEBAQIFBQEBAQMDAQEBAQIDAQEBAAUKAQECAQEDAQECAQEEAQEABAIrBAL/BAMBBAEAAwIBBAEBAQMFBf8BAwMDAAABAgMEAAAH//8AAwEEAAAAAwEEAAr//wMCAQMAAAMCAQQAAi0EAv8EAwECAgAEAwECAgECBAUI//8ABAEDAAAABAEDAAcEAwEBAv8EAwEBAgABAQIDCQIBBAT//wIBBAQAAAEEAgMCLAQC/wQCAQIAAAQCAQMAAAL/BAIBBP8AAwIBBAAABf8BAwMCAAABAwQCAAAK//8DAQEDAAADAQEEAQEDBAJGBAH/AwACAQAAAwACAQAAAf8DAAIDAAACAAEDAAAFBAEDAQEBBAEDAQEBAQEBBQUEAQMDAQEEAQIDAQEBAQEFAksDAQEAAAIBAAEAAAMCAAABAQMAAgMAAQMAAgQAAAEB/wACAQABAAADAgAAAlIEAf8AAAIDAQAAAAIDAQAB/wADAgMBAAADAgQBAAX/AAMDAgEAAAMEAgEABQEBAwMCAQEBAwQCAQEBAAUCVQQBBAAAAgMBBAAAAgMBAAX//wIDAgEAAAIEAwEACv8CAwEBAQACAwEBAQEBAwUK/wIDAQEEAAIDAQEEAQEDBAI4BAH/AgACAwEAAgADBAEABwECAQEEBAECAQEEBAEBAAMHAQIBBAQEAQIBBAQEAQEAAgn//wECAgIAAAECAgIBAgMFAlMEAf8BAwIDAQABAwIEAQEBAQUF//8EAwIBAAAEAwIBAAj//wEDAwMAAAEDAwMBAwMFCv8EAwEBAQAEAwEBAQEBAwUCOQQB/wMAAgMBAAMAAgQBAAr//wEBAQEAAAEBAQEBAQIFAf8DAwID/wACAwEEAAAHAgIBAf//AgMBAQAAAQECAwI6BAH/BAACAwEABAACAwEABwMCAQEEBAMCAQEEBAEBAgMHAwIBBAQEAwIBBAQEAQQDBQr//wIBAQEAAAIBAQEBAQMFAj8EAQAAAAIDAQAAAAIDAQABAAADAgMBAAADAgQBAAH//wICAwEAAAIDBAEABQABAwMCAQABAwQCAQEBAQUCVAQB//8BAgMBAAABAgMBAQECBQoCAwMBAQECAwQBAQEBAQMFCv//BAEBAQAABAEBAQEBAwUG/wECAgH/AAECAwEAAQEBBAJABAH//wMCAwEAAAMCBAEAB/8CAQEEBAACAQEEBAEBAgMH/wIBBAQEAAIBBAQEAQQDBQoEAwMBAQEEAgMBAQEBAQMFAkEEAQIAAAIDAQIAAAMEAQAB//8EAgMBAAAEAgMBAAT//wEEAwIAAAEEAwIAB/8DAQQEBAADAQQEBAEEAwUCQgQBAwAAAgMBAwAAAgQBAAX//wEDAgEAAAEDAgEBAQIFCv8BAwEBAQABAwEBAQEBAQUK/wEDAQEEAAEDAQEEAQEBBAIzBAH/AAACAwIAAAABAgEBAgMFAf8ABAIDAgAAAwECAQECAwUC/wADAQIEAAADAQIEAAX/AAMDAwEAAAIDBAEAAlEEAQQAAAIDAgMAAAECAQECAwUBBAAEAgMCAwAEAQIBAQIDBQX//wIDAwEAAAIDBAEACv8CAwIBAQACBAMBAQEBBAUCNQQB/wIEAgMCAAEDAQIBAQIBBQL/AQMBAgQAAQMBAgQBAQEDBwEDAQEBBAEDAQEBBAEBAAQHAQMBAQQEAQMBAQQEAQEAAwI0BAH/AQACAwIAAQACBAMABf//BAMDAQAABAIDAQAI//8BBAMDAAABBAIDAAr/BAMCAQEABAMCAQEBAQQFAjYEAf8DAAIDAgACAAEDAQECAwUB/wMEAgMCAAIEAQMBAQIDBQcCAwEBAQQCAwEBAQQBAQIEBwIDAQEEBAIDAQEEBAEBAgMCNwQB/wQAAgMCAAMAAQIBAQIDBQH/BAQCAwIAAwQBAgEBAgMFBwMDAQEBBAIDAQEBBAEBAgQHAwMBAQQEAgMBAQQEAQECAwIuBAEAAAACAwIAAAABAgEBAgMFAQAABAIDAgAAAwECAQECAwUCAAQDAQIBAAQDAQIBAQEDBQH//wICAwIAAAEBAgEBAgIFAlAEAf//AQIDAgAAAQIEAwAK//8EAgEBAAAEAgEBAQEEBQb/AQICAv8AAQICAgABAgIECwECAgH//wECAwEAAAEBAAMCLwQBAQAAAgMCAQAAAgQDAAH//wMCAwIAAAIBAwEBAgMFB/8CAQEBBAACAQEBBAEBAgQBAQAEAgP/AQAEAgMAAAIwBAIBBAMBAgEBBAMBAgEBAQAFAQL//wIDAgEBAQECAQECAAUB//8EAgMCAAADAQIBAQIDBQL//wMBAgQAAAMBAgQAAjEEAQMAAAIDAgIAAAEDAQECAwUBAwAEAgMCAgAEAQMBAQIDBQL//wQBAgEAAAQBAgEBAQMFBf//AQMDAQAAAQMEAQEBAgUDAAQB//8BAwQDAAABAgQDAAP/BAMBAgEABAMBAgEBAQMFBv8BAwMDAQABAgMEAQEBAQUI//8BAQEEAAABAQEEAQECBAMBBAH//wEDBAIAAAEDBAIAAf//BAMEAgAAAwIEAQAGAQEDAwIBAQEDBAIBAQEABQsBAwMBAQEBAwQBAQEBAQAFAwIEAf//AQL/AgAAAQIAAwAE/wMB/wECAAQBAAIDAAb/AQIDAv8AAQIEAwAACQMB/wMC/wMBAAQCAAADAwQB//8BAgECAAABAwIEAAX/AgMBAwEAAgMBBAEBAQMFB///AQIBAgAAAQMCBAAKAv8BAgH/AgABAwEAAQECBAMEBAH//wEDBAQAAAEDBAQBBAQFBv8BAQMEAQABAQMEAQEBAQUG/wEDAwQBAAECAwQBAQEBBQsBAQMDAQEBAQMEAQEBAQAFAwUEAQEBAQMEAQEBAQMEAQEBAAUGAQEDAwEBAQEDBAEBAQEABQgEAQEDBP8DAQECBAABAQECCwEDA/8BAwECAwABBAEBAAQDBgQB//8BAwQEAAABAgMEAAYBAQMDBAEBAQIDBAEBAQAFCP//AQEC/wAAAQECAAEBAgMLAQMDAwEBAQIDBAEBAQEABQMHAwb/AQEDAQEAAQEDAQEBAQEFA/8EAQECAgAEAQECAgIBAgMCBAULAQEDAwEDAQECAwEEAQEABAMIBAH//wEDAgQAAAEDAgQABP8DAwMBAQACAwQBAQEBBAUGAQEDAQQBAQEDAQQBAQEABQsBAwEDAQEBAwEEAQEBAQAFAwoEAf//AQIEAwAAAQIEAwAE/wMCAAECAAQCAAEDAAb/AQIAA/8AAQIAAwAACgIB/wMBAgIBAAQBAwEBAQQDCwQD//8DAgIBAAAEAgMBAAT/AwIBAf8AAwIBAQABAQMECAQDAgEB/wQDAgEBAAEBAwQLAf8DAgL/AQAEAgMAAAMMAwsBA/////8BAwAAAAAABv8BA////wABAwAAAAALAQMD////AQMEAAAAAAMNBAH//wEDAQMAAAEDAQQBAQIEBP8DAgIB/wAEAgMBAAAG/wEDAwMDAAEDAwMDAQMCBQoC/wEDAv8CAAEEAwAAAw4EAf//AQABAQAAAgADBAAB/wMDAwQDAAEBAQIBAQMBBQX/AgEBAgIAAgEBAwQBAQIDCgIBAQECAgIBAQEDBAEBAQMDDwQB//8BAwIDAAABAwIEAAb/AQMBAwEAAQMBBAEBAQEFCP//AQEBAgAAAQEBAgEBAgQLAQMBAgEBAQMBAgEBAQEABQMQBAH//wECAgMAAAECAwQABP//AgMBAgAAAgQBAwAG/wECAQMBAAECAQMAAQEBBQoC/wIDAf8CAAMEAQAAAxEEAf//AQQCAwAAAQQCAwAG/wEEAQMCAAEEAQMCAQEBAwkDAgMAAAEDAgQAAAEACwH/AQIC/wEAAgMEAAADEgQB//8BAAIBAAABAAMCAAX/AgECAgIAAgEDAwQBAgEFCgIBAgH//wMBBAIAAAALAQMBAgEDAQMBAgEEAQEABAMTBAX/AgECAgEAAgEDBAEBAQIFBv8BAgABBAABAwACBAAKAgECAQECAQIAAwAEAQEBBAsBAgEC/wMBAgEDAAQBAQACAxQEAQMEAwQEAwECAQMEAQEDAAUF/wIBAgIDAAIBAwMEAQIBBAkDAv8CAAEEAgADAAEACgIBAgED/wIBAwEEAAEBAQMDFQQB//8BAAIAAAACAAQAAAX/AgECAQIAAgEDAQQBAQIEBv8BAwEDAAABAwEEAAEBAQMJAwIDAf//AwIEAQAAAAMWBAH//wEAAgIAAAIAAwQABf8CAQID/wACAQMEAAAI/wIBAQECAAIBAQEDAQECBAoCAQIC//8CAQMEAAAAAxcEAQEBAQECAwEBAQECAwEBAAME/wMCAwEBAAMCBAEBAQEEBQYAAQEBAwEAAQEBAwEBAQEFCwEBAQIBAQEBAQIBAQEBAAUDGAQB//8BAgIDAAABAgMEAAX/AgECAgEAAgEDBAEBAQIFBv8BAgED/wABAgEDAAEBAQMKAgECAQECAgEDAQEEAQEBBAMZBAX/AgECBAQAAgEDBAQBBAQFBgEBAQEDAwEBAQEDBAEBAAMJAwICAAEBBAIDAAEBAQEEBQsBAQECAwMBAQECAwQBAQACAxoEAf8BAQMDAwABAQMDAwIBAQIDAwUD/wQDAQEBAAQDAQEBAQEDBQYBAQMCAwEBAQMCBAEBAQAFCP//AQEBAwAAAQEBBAEBAgQDGwQB//8BAgMDAAABAgMEAAb/AQICAwEAAQICBAABAQEFCgIBAwMBAQIBAwQBAQEBAQULAQICAv8BAQICAgABAgEABQIBAwMcBAMBBAMCAQEBBAMCAQEBAQAFBf8CAQP/AwACAQMABAAG/wEEAgP/AAEEAgMAAAsB/wICAgEBAAIDBAEBAQAFAx0EAf//AQMDAQAAAQMEAQEBAgUG/wEDAgEBAAEDAgEBAQEBBQr//wQBAgEAAAQBAgEBAQMFA/8EAQEB/wAEAQEBAAEBAgQDHgQD/wQBAQEBAAQBAQEBAQECBQX/AgEDAv8AAgEEAwAACP8BAQEBAwABAQEBBAEBAQQL//8CAgEDAAACAwEEAAMfBAH//wEBAwMAAAEBAwQBAQIDBP8DAgQBAQADAgQBAQEBBAUGAQEBAgMBAQEBAgMBAQEABQsBAQICAQEBAQIDAQEBAQAFAyAEAf8DAQAD/wADAQAEAAAD/wQDAwEBAAQCAwEBAQEEBQb/AQECAwMAAQECAwQBAQECCgIBAQECAQIBAQEDAQEBAQUDIQQB/wEBAwECAAEBAwECAQEBBAT/AwECAQMAAwECAQQBAQIEB///AgIBAgAAAgMBBAALAQMDAQMBAQIDAQQBAQEABQMjBQsB/wEBAf8CAAMDAwAAAf//AQMCAgAAAQQCAwAGAQEDAQIBAQEDAQIBAQEABQf//wICAQMAAAIDAQQACwEDAQEBAQEEAQEBAQEBAAUDJAQB//8BAgICAAABAgICAQIDBQb/AQIBAv8AAQMCBAAAB///AQIBAwAAAQIBBAEBAgQKAv8CAgH/AgADBAEAAAMlBAT/AwEDAwMAAgEDBAQBAwEFB///AgQBAwAAAgQBAwAJ/wEDAgMDAAEDAgQEAQMCBQsBAwEBAQMBAwEBAQQBAQAEAyIEAQL/AQMBAQIAAQMBAQEBAgUE/wMBAgP/AAMBAgQAAAkDAQICAQEEAQIDAAABAQEFC///AwEDAwAAAgEDBAADJgQB//8BAQICAAABAQMEAQECAwT/AwEDAwEAAgEDBAEBAQIFCQMBAwIBAQMBBAIBAQEBAQULAQEBAQEDAQEBAQEEAQEABAMnBAH//wEDAwIAAAEDBAIAA/8EAgEB/wAEAwECAAAG/wEDAgIBAAEEAgMBAQEBBQsBAwIBAQEBAwIBAQEBAQAFAygEAf//AQIDAgAAAQIEAwAFAQIDAwP/AQIDAwMAAQMCBAb/AQICAv8AAQIDBAAACwECAgH/AQECAgEAAQIBAAUCAQIDKQQE/wMBBAP/AAIBBAMAAAYBBAMCAQEBBAMCAQEBAQAFCQMBBAL//wMBBAIAAAALAQMCAQEDAQMCAQEEAQEABAMqBAH/AQEBAwIAAQEBAwIBAQEDBP8DAQQDAQADAQQDAQEBAgUGAQEBAgIBAQEBAgMBAQEABQsBAQIBAQMBAQIBAQQBAQAEAysEA/8EAwEEAQADAgEEAQEBAwUF/wIBAAICAAIBAAMEAAoCAf8BAv8DAQACBAAAC///AwIBAwAAAwIBBAADLQQB//8BAAQEAAABAAQEAQQEBQP/BAMBAgIABAMBAgIBAgQFCAQDAQEC/wQDAQECAAEBAgMKAgEEBP//AgEEBAAAAQQCAwMsBAH//wQDBAEAAAMCBAEAA/8EAgEE/wADAgEEAAAE/wMB/wMDAAIBAAMEAAv//wMBAQMAAAMBAQQBAQMEA0cEAf8EAQMCAQAEAQMCAQEBAgUGBAEDAQEBBAEDAQEBAQEBBQYEAQMDAQEEAQIDAQEBAQEFBgQBAwEB/wQBAwEBAAEBAQQDTAIBAgEBAwIBAgEBBAMBAQEBBQEC/wEDAgECAAEEAwEBAQIFA1IEAf8AAQMEAgAAAQMEAgAB/wAEAwQCAAADAgQBAAb/AAMDAgEAAAMEAgEABv//AgMCAQAAAgQDAQADVQQG//8BAwIBAAABAwIBAQECBQv/AQMBAQEAAQMBAQEBAQEFC/8BAwEBBAABAwEBBAEBAQQL/wEDAQH/AAEDAQEAAQEBBAM4BAH/AgQDBAIAAQMCBAEBAgEFBv//BAMCAQAABAMCAQAJ//8BAwMDAAABAwMDAQMDBQv/BAMBAQEABAMBAQEBAQMFA1MEAf8BAQMEAgABAQMEAgEBAQIGAQEDAwIBAQEDBAIBAQEABQb//wMDAgEAAAMEAgEACP//AQQEBAAAAQQEBAEEAwUDOQQIAQIBAQQEAQIBAQQEAQEAAwgBAgEEBAQBAgEEBAQBAQACCv//AQICAgAAAQICAgECAwUB/wMEAwT/AAECAQMAAQMBAwM6BAv//wEBAQEAAAEBAQEBAQIFAf8EBAME/wACAwEEAAAIAgIBAf//AgMBAQAAAQECAwH/BP8DBAIAAwACBAEAAzwEAf//AAMEAgAAAAIDAQAG//8AAwIBAAAAAwIBAAgDAgEBBAQDAgEBBAQBAQIDCAMCAQQEBAMCAQQEBAEEAwUDPwQBAAEBAwQCAAEBAwQCAQEBAgH//wIDBAIAAAECAwEBAgIFBgABAwMCAQABAwQCAQEBAQUIAAIBAQQEAAIBAQQEAQECAwNABAEBAQEDBAIBAQEDBAIBAQACAf//AwMEAgAAAgMEAQAI/wEBBAQEAAEBBAQEAQEBAgsDAwMBAQECAwQBAQEBAQMFA0EEAf//BAMEAgAAAwIEAQAI/wIBAQQEAAIBAQQEAQECAwj/AgEEBAQAAgEEBAQBBAMFCwQDAwEBAQQCAwEBAQEBAwUDQgQC//8EAgMBAAAEAgMBAAX//wEEAwIAAAEEAwIACP8DAQQEBAADAQQEBAEEAwUDAQQCAf//AQQCAQAAAQEAAwMzBAH/AAEDBAMAAAECBAMAA/8AAwECAQAAAwECAQEBAwUD/wADAQIEAAADAQIEAAb/AAMDAwEAAAIDBAEAA1EEA///BAECAQAABAECAQEBAwUG//8BAwMBAAABAwQBAQECBQv/AQMCAQEAAQMCAQEBAQEFCP8EAQEB/wAEAQEBAAEBAgQDNQQG//8EAwMBAAAEAgMBAAn//wEEAwMAAAEEAgMAC/8EAwIBAQAEAwIBAQEBBAUC/wEEAgP/AAEEAgMAAAM0BAH/AQEDBAMAAQECBAMBAQECBgEBAwMDAQEBAgMEAQEBAAUG//8DAwMBAAACAwQBAAj//wEBAQQAAAEBAQQBAQIEAzYEA/8BAwECAQABAwECAQEBAQUD/wEDAQIEAAEDAQIEAQEBAwgBAwEBAQQBAwEBAQQBAQAECAEDAQEEBAEDAQEEBAEBAAMDNwQD/wIDAQIBAAIEAQMBAQEDBQgCAwEBAQQCAwEBAQQBAQIECAIDAQEEBAIDAQEEBAEBAgML//8BAgEBAAABAgEBAQECBQM7BAH//wADBAMAAAABAgEBAwMFA/8DAwECAQADBAECAQEBAwUG//8AAwMBAAAAAwQBAAgDAwEBAQQCAwEBAQQBAQIEAy4EAQABAQMEAwABAQIEAwEBAQIB//8CAwQDAAABAgQDAAMABAMBAgEABAMBAgEBAQMFBgABAwMDAQABAgMEAQEBAQUDLwQBAQEBAwQDAQEBAgQDAQEAAgH//wMDBAMAAAEBAgEBAwIFCP8BAQEBBAABAQEBBAEBAQQI/wEBAQH/AAEBAQEAAQEBBAMwBAH//wQDBAMAAAIBAwEBAwMFCP8CAQEBBAACAQEBBAEBAgQI/wIBAQH/AAIBAQEAAQECBAECAQEA//8DAQIAAAAAAzEEAwEEAwECAQEEAwECAQEBAAUBA///AwQDAQEBAQIBAQMABQP//wMBAgEAAAMBAgEBAQMFA///AwECBAAAAwECBAAEAAQBAAICAQAAAAIDAQAAAAL//wEDBAMAAAECBAMABP8EAwECAQAEAwECAQEBAwUH/wEDAwMBAAECAwQBAQEBBQQBBAEAAgIAAAAAAgMAAAAAAgABAQMEAgABAQMEAgEBAQIH/wEDAwIBAAEDBAIBAQEBBQkEAgEB//8EAwECAAAABAIEAf//AgP/AwAAAQIAAwAF/wMB/wECAAQBAAIDAAf/AQIDAv8AAQIEAwAACgMB/wMC/wMBAAQCAAAEAwQBAAECAAIAAAECAAMAAAH//wIDAgMAAAEDAgQABv8CAwED/wACAwEEAAALAv8BAgH/AgABAwEAAQECBAQEBAEAAAICAAAAAAECAAAAAQAAAgQAAAAAAQMAAAABAAICAgAAAAECAwAAAAH//wICAAAAAAECAAAABAUEAgEBAQMEAQEBAQMEAQEBAAUHAAEDAwEBAAEDBAEBAQEBBQkAAQEBAAAAAQIDAAAADAEDA/8BAwEDBAABBAEBAAQEBgQBAAICAgAAAAIDBAAAAAIAAQEDBAQAAQECAwQBAQECBwEBAwMEAQEBAgMEAQEBAAUJ//8BAQIAAAABAQIAAQECAwQHBAEAAAICAAIAAAECAAMAAQACAgIAAgABAgMABAAH/wEBAwEBAAEBAwEBAQEBBQT/BAEBAgIABAEBAgICAQIDAgQFBAgEAQACAAIAAAACAAMAAAAF/wMDAwEBAAIDBAEBAQEEBQcBAQMBBAEBAQMBBAEBAQAFCf//AQECAgAAAQECAwEBAgMECgQBAAECAf//AAEDAgAAAAL//wECBAMAAAECBAMABQADAv8BAgAEAgABAwAHAAECAwMAAAECAwQAAAQLBAEAAwIBAQAABAMBAgAABf8DAgEB/wADAgEBAAEBAwQH/wEEAwP/BAMCAQEAAAkEAwIBAQAEAwIBAQABAQMEBAwEDAED/////wEDAAAAAAAH/wED////AAEDAAAAAAwBAwP///8BAwQAAAAAAQACAv///wACAwAAAAAEDQQBAAICAQIAAAIDAQQAAAEAAgIEAgQAAQEDAQQBAgEEBf8DAgIB/wAEAgMBAAAH/wEDAwMDAAEDAwMDAQMCBQQOBAEAAgIBAgIAAgIBAwQBAgEFBv8CAQECAgACAQEDBAEBAgMJ/wEBAQEBAAEBAQEBAQEBBQsAAQEBAgIAAQEBAwQBAQEDBA8EAQACAAEAAAACAAEAAAAF/wMCAwH/AAMCBAEAAAcBAQMBAwEBAQMBBAEBAQAFCf//AQEBAgAAAQEBAgEBAgQEEAQBAAEAAQMAAAEAAgQAAAH//wIDAwQAAAECAwQABf//AgMBAgAAAgQBAwAH/wECAQP/AAECAQMAAQEBAwQRBAEAAwABAQAABAABAgAAAv//AQQCAwAAAQQCAwAH/wEEAQMCAAEEAQMCAQEBAwwB/wECAv8BAAIDBAAABBIEAQACAAEAAgACAAEAAwABBP8CBAMCAwABBAIBAQICBQYCAgECAgICAgEDAwQBAgAFCQABAQEBAgABAQEBAgEBAQQEEwQBAAECAQMCAAECAQQDAQEBAwMAAwIBAQIABAIBAQMBAQMEBv8CAQICAQACAQMEAQEBAgUHAAMCAQMAAAMCAQQAAAQUBAEAAwABAwIAAwABBAIABAACAQICAQACAQMEAQEBAgUG/wIBAgIDAAIBAwMEAQIBBAkAAQIBAQIAAQIBAQMBAQEEBBUEAQACAAEAAQADAAEAAgAEAAIAAQMBAAMAAQQBAQEDBQb/AgECAQIAAgEDAQQBAQIECgMCAwH//wMCBAEAAAAEFgQBAAIAAQADAAIAAQAEAAMAAwAAAQIAAwAAAQIABv8CAQID/wACAQMEAAAJAAIBAQECAAIBAQEDAQECBAQXBAEAAAABAAAAAAABAAAAAQAABAQDBAAAAgMBBAAF/wMCAwEBAAMCBAEBAQEEBQf/AQEBAwEAAQEBAwEBAQEFBBgEAQABAAEAAAABAAIAAAADAAMAAQMCAAMAAQQCAAb/AgECAgEAAgEDBAEBAQIFCwIBAgEBAgIBAwEBBAEBAQQEGQQBAAIAAQIAAAIAAQMAAAEAAAABAgIAAAABAgMABQABAgIBAAABAwQCAAAHAQEBAQMDAQEBAQMEAQEAAwQaBAEAAgEBAAAAAwECAAAAAf//AgQEBAAAAQMDAwEEAwUE/wQDAQEBAAQDAQEBAQEDBQcBAQMCAwEBAQMCBAEBAQAFBBsEAQABAQEEAAABAQEEAAEBAQMB//8CAwQEAAABAgMEAAf/AQICA/8AAQIDBAAACwABAwMBAQABAwQBAQEBAQUEHAQBAAMCAQQEAAMCAQQEAQQEBQQBBAMCAQEBBAMCAQEBAQAFB/8BBAID/wABBAIDAAAJAAMCAQEDAAMCAQEEAQEDBAQdBAEAAgIEBAIAAQEDBAEBAgEFAQACBAQEAgABAgMEAQECAQUB//8CBAQCAAABAwQBAQICBQQAAwEBAQQAAwEBAQQBAQIEBB4EAQACAQEAAgADAQIABAABBAICBAQCAgEBAwQBAQIBBQb/AgEDAv8AAgEEAwAACf//AQMBAwAAAQMBBAEBAgQEHwQBAAABAQAAAAABAgAAAAQAAAMBAQIAAAMBAQIBAQMEBwEBAQIDAQEBAQIDAQEBAAUJAAMBAQIDAAMBAQIEAQECAwQgBAEAAgEBAgIAAgEBAwQBAQIDBP8EAwMBAQAEAgMBAQEBBAUH/wEBAgMDAAEBAgMEAQEBAgsCAQEBAgECAQEBAwEBAQEFBCEEAQACAgACAAABAgADAAABAAICBAIDAAEBAwECAQIBBAX/AwECAQMAAwECAQQBAQIECP//AgIBAgAAAgMBBAAEIwcBAP8AAAD/AgADAwMAAAwB/wEBAf8CAAMDAwAAAQACAgADAAACAwAEAAABAAIAAAAAAAIAAAAAAAEAAgIEAwMAAQEEAgMBAgECBwEBAwECAQEBAwECAQEBAAUI//8CAgEDAAACAwEEAAQkBAEAAQIDAwMAAQIDAwMBAwMFB/8BAgEC/wABAwIEAAAI//8BAgEDAAABAgEEAQECBAsC/wICAf8CAAMEAQAABCUEAQACAAAAAgACAAAABAABAAICAAMCAAECAAQDAAX/AwEDAwMAAgEDBAQBAwEFCgABAwIDAQABAwIEAQEBAQUEIgQBAAICAAICAAEBAAIDAQIBBQED/wIEAgICAAEDAQEBAgIFBf8DAQIDAwADAQIEBAEDAQUKAAECAgMAAAECAwQAAAQmBAEAAAAAAAIAAAAAAAEAAwAAAwIBAAAAAwIBAAAF/wMBAwMBAAIBAwQBAQECBQoDAQMCAQEDAQQCAQEBAQEFBCcEAQACAQAAAAACAQAAAAABAAICBAQDAAEBAwQCAQIBAgT/BAIBAf8ABAIBAQABAQMEBwEBAwICAQEBBAIDAQEBAAUEKAQBAAEBAP8AAAECAAAAAAH//wIDBAMAAAECBAMABgECAwMD/wECAwMDAAEDAgQH/wECAgL/AAECAwQAAAQpBAEAAgEAAAIAAgEAAAMAAQD/BAQEAwAAAgIEAQEEAgQHAAQDAgEAAAQDAgEAAAwBAwIBAQMBAwIBAQQBAQAEBCoEAQAAAQAAAgAAAQAAAwAB/wICAgQDAAEBAQMCAQIBAwX/AwEEAwEAAgEEAwEBAQIFBwEBAQICAQEBAQIDAQEBAAUEKwQBAAICAQACAAIDAQAEAAT/BAMBBAEAAwIBBAEBAQMFBv8CAf8CAgACAQADBAAM//8DAgEDAAADAgEEAAQtBAIAAQEBBAMAAQEBBAMBAQEDBAAEAwECAgAEAwECAgECBAUE/wQDAQICAAQDAQICAQIEBQcAAQEDAwEAAQEDBAEBAQEFBCwEAf//AwEBAwAAAwEBBAEBAwQF/wQCAQT/AAMCAQQAAAb/AwEAAQMAAwEAAgQACP8BAwACAQABBAADAgAESAQC/wQBAwIBAAQBAwIBAQECBQcEAQMBAQEEAQMBAQEBAQEFBwQBAwMBAQQBAgMBAQEBAQUHBAEDAQH/BAEDAQEAAQEBBARNAwEDAgIEAwICAQEEAwEBAgEFAQP/AgADAgMAAQAEAgABA/8CBAMCAgABBAMBAQICBQRSBAH/AAIAAAAAAAEAAAAAAf8AAgAAAwAAAQAAAgAB/wACBAADAAABAwACAAL/AAEDBAIAAAEDBAIABFUEAQQCAgAAAAQBAgAAAAABBAICAAADBAECAAADAAP//wQCAwEAAAQCAwEABv//AQQDAgAAAQQDAgAEOAQB/wICAAAAAAECAAAAAAH/AgIAAAMAAQIAAAMAAf8CAgQAAwABAgQAAwAC/wEBAwQCAAEBAwQCAQEBAgRTBAH/AQIAAAAAAQIAAAAAAf8BAgAAAwABAgAAAwAB/wECBAADAAECBAADAAf//wIDAgEAAAIEAwEABDkEAf8DAgAAAAACAQAAAAAB/wMCAAADAAIBAAADAAH/AwIEAAMAAgEEAAMAA/8BAwIDAQABAwIEAQEBAQUEOgQB/wQCAAAAAAMBAAAAAAH/BAIAAAMAAwEAAAIAAf8EAgQAAwADAQQAAgAJAQIBAQQEAQIBAQQEAQEAAwQ8BAH//wAAAAAAAAAAAAAAA///AAIDAQAAAAIDAQAH//8AAwIBAAAAAwIBAAT/AgIBAv8AAgMBBAAABFQEAf//AQAAAAAAAQAAAAAJAwIBAQQEAwIBAQQEAQECAwkDAgEEBAQDAgEEBAQBBAMFBP8DAgEC/wAEAgEDAAAEQAQBAQICAAAAAQIDAAAAAAEBAgIAAAMBAgMAAAQAAf//AwAAAAAAAQAAAAAD//8BAgMBAAABAgMBAQECBQRBBAECAgIAAAABAgMAAAAAAQICAgAAAwECAwAABAACAQEBAwQCAQEBAwQCAQEAAgP//wICAwEAAAIDBAEABEIEAQMCAgAAAAMBAgAAAAABAwICAAADAwECAAAEAAEDAgIEAAADAQIEAAAAA///AwIDAQAAAwIEAQAEMwQB/wACAQAAAAACAQAAAAH/AAIBAAQAAAIBAAQAAf8AAgQABAAAAQMABAAC/wABAwQDAAABAgQDAARRBAQBBAMBAgEBBAMBAgEBAQAFBAH//wECAQEBAQECAQEBAAUE//8DAQIBAAADAQIBAQEDBQT//wMBAgQAAAMBAgQABDUEAf8CAgEAAAACAwEAAAAC/wEBAwQDAAEBAgQDAQEBAgcBAQMDAwEBAQIDBAEBAQAFB///AwMDAQAAAgMEAQAENAQB/wECAQAAAAEDAgAAAAf//wIDAwEAAAIDBAEAAf8BAgEA/wABAwIAAAAGAQIEBAT/AQIEBAQAAQQCBAQ2BAH/AwIBAAAAAwIBAAAAAf8DAgEABAADAgEABAAB/wMCBAAEAAIBAwAEAAf//wQDAwEAAAQCAwEABDcEAf8EAgEAAAAEAgEAAAAE/wEDAQIBAAEDAQIBAQEBBQT/AQMBAgQAAQMBAgQBAQEDCQEDAQEBBAEDAQEBBAEBAAQEOwQB//8AAQAAAAAAAQAAAAT//wABAgEAAAABAgEBAQMFBP8CAwECAQACBAEDAQEBAwUH//8AAwMBAAAAAwQBAARQBAH//wEBAAAAAAECAAAABP8DAwECAQADBAECAQEBAwUJAwMBAQEEAgMBAQEEAQECBAkDAwEBBAQCAwEBBAQBAQIDBC8EAQECAgEAAAEDBAIAAAAB//8DAQAAAAADAQAAAAP//wECAwIAAAECBAMAAQECAgEA/wEDBAIAAAAEMAQBAgICAQAAAgMEAQAAAAH//wQBAAAAAAQBAAAAAgEBAQMEAwEBAQIEAwEBAAIE//8BAQIBAAABAQIBAQECBQQxBAEDAgIBAAAEAgMBAAAABP//AgECAQAAAgEDAQEBAwUJ/wIBAQEEAAIBAQEEAQECBAEDAgIBAP8EAgMBAAAABQAFAQEDAwIBAQEDBAIBAQEBAAUB//8DAgEBAAADAgEBAQEEBQP//wEDBAMAAAECBAMABf8EAwECAQAEAwECAQEBAwUI/wEDAwMBAAECAwQBAQEBBQUBBAEBAwMBAQEBAwQBAQEBAQAFA///AQMEAgAAAQMEAgAI/wEDAwIBAAEDBAIBAQEBBQoEAgEB//8EAgEBAAABAQIDBQIEAf//AwT/BAAAAQIAAwAG/wMB/wECAAQBAAIDAAj/AQIDAv8AAQIEAwAACwMB/wMC/wMBAAQCAAAFAwQBAf8AAQABAQAAAgADAAH//wMEAwQAAAEDAQQBAwIEBwECAwEDAQECAwEEAQEBAAUMAv8BAgH/AwABBAIAAAUEBAEBAQMDAQEBAQMEAQEBAQAFAQEDAwMBAQECAwQBAQEBAAUB//8DAwEBAAADBAEBAQEEBQP//wEDBAQAAAEDBAQBBAQFBQUEAQEDA/8BAwECAwABBAEBAAQDAQEBAwQBAQEBAwQBAQEABQgBAQMDAQEBAQMEAQEBAQAFCgQBAQME/wMBAQIEAAEBAQIFBgQBAQMDAwEBAQIDBAEBAQEABQP//wEDBAQAAAECAwQACAEBAwMEAQEBAgMEAQEBAAUK//8BAQL/AAABAQIAAQECAwUHAwEBAQMDAQMBAQIDAQQBAQAECP8BAQMBAQABAQMBAQEBAQUF/wQBAQICAAQBAQICAgECAwIEBQUIBAEBAwEDAQEBAwEEAQEBAQAFBgEDAwMBAQECAwQBAQEBAAUIAQEDAQQBAQEDAQQBAQEABQr//wEBAgIAAAEBAwQBAQIDBQoEAQECAwIA/wECBAMAAAAD//8BAgADAAABAgADAAj/AQIDA/8AAQIDBAAADAIB/wMBAgIBAAQBAwEBAQQFCwQB//8DAgIBAAAEAgMBAAb/AwIBAf8AAwIBAQABAQMECP8B/wMDAgABAAMEAgAKBAMCAQH/BAMCAQEAAQEDBAUMAwEBA/////8BAwAAAAAACP8BA////wABAwAAAAABAQMD////AQMEAAAAAAUNBAEB/wMCAwEBAAMCBAEBAQAFA/8BAQMBAwABAQMBBAEBAQQG/wMCAgH/AAQCAwEAAAgBAQMDAwMBAQMDAwMCAQABAwIFBQ4EAQEAAAABAQEAAAACAwAB//8DAgMDAAACAQMEAAX/AQEBAgEAAQEBAgEBAQEFB/8CAQECAgACAQEDBAEBAgMFDwQBAQMBAgEBAQMBAgEBAQEABQP/AQEDAgMAAQEDAgQBAQECCAEBAwEDAQEBAwEEAQEBAAUK//8BAQECAAABAQECAQECBAUQBAEBAAECAAEBAAIEAAMAA///AQICAwAAAQIDBAAG//8CAwECAAACBAEDAAj/AQIBA/8AAQIBAwABAQEDBREEAQEAAQIC/wEAAgMEAAAD//8BBAIDAAABBAIDAAj/AQQBAwIAAQQBAwIBAQEDDAIBAv8D/wIBAwAEAAAFEgQBAQMBAgEDAQMBAgEEAQEABAH//wMCBAMAAAIBBAMABwICAQICAgICAQMDBAECAAUK/wEBAQECAAEBAQECAQEBBAUTBAEBAAEAAAEBAAIAAAMAB/8CAQICAQACAQMEAQEBAgUI/wECAAMEAAECAAMEAAwCAQIAAAICAQIAAAMBAgAFBRQEAQEAAQACAQEAAgAEAwAFAQIBAgIBAQIBAwQBAQEABQf/AgECAgMAAgEDAwQBAgEEDAIBAgED/wIBAwEEAAEBAQMFFQQBAQMBAgECAQQBAgEDAQEABAH//wMCBAIAAAIBAwEBAgMFB/8CAQIBAgACAQMBBAEBAgQLAwIDAf//AwIEAQAAAAUWBAEBAwECAQQBAwECAQQBAQAEAf//AwIEBAAAAgEDBAAH/wIBAgP/AAIBAwQAAAr/AgEBAQIAAgEBAQMBAQIEBRcEAQEBAQIBAQEBAQIBAQEBAAUDAQEBAQIDAQEBAQIDAQEAAwb/AwIDAQEAAwIEAQEBAQQFCP8BAQEDAQABAQEDAQEBAQUFGAQBAQABAAABAQACAAADAAP//wECAgMAAAECAwQAB/8CAQICAQACAQMEAQEBAgUI/wECAQMBAAECAQMBAQEBBQUZBAEBAwECAwEBAwECBAEBAQAFAQEBAQIDAwEBAQIDBAEBAAIH/wIBAgQEAAIBAwQEAQQEBQgBAQEBAwMBAQEBAwQBAQADBRoEAf//AwIBAAAAAwIBAAABAQMCAgEBAQQCAwEBAQEABQP/AQEDAwMAAQEDAwMBAQECCAEBAwIDAQEBAwIEAQEBAAUFGwQBAQACAgAAAQACAwAAAAP//wECAwMAAAECAwQACP8BAgID/wABAgMEAAAMAgEDAwEBAgEDBAEBAQEBBQUcBAEBAAICAgABAAIDBAAABQEEAwIBAQEEAwIBAQEBAAUI/wEEAgP/AAEEAgMAAAz//wQDAwEAAAQCAwEABR0EAf//AwABAAAAAwABAAAD//8BAwMBAAABAwQBAQECBQj/AQMCAQEAAQMCAQEBAQEFBf8EAQEB/wAEAQEBAAEBAgQFHgQBAQACAAEAAQADAAIAAAH//wICAQMAAAIDAQQAB/8CAQMC/wACAQQDAAAK//8BAwEDAAABAwEEAQECBAUfBAEBAQICAQEBAQIDAQEBAQAFA///AQEDAwAAAQEDBAEBAgMIAQEBAgMBAQEBAgMBAQEABQsDAgQAAQADAgQAAQAABSAEAQEAAAABAAEAAAACAAAB//8DAgMAAAACAQMAAAX/BAMDAQEABAIDAQEBAQQFCP8BAQIDAwABAQIDBAEBAQIFIQQBAf8AAQEBAQAAAgMEAAP//wEDAQIAAAEDAQIBAQIEBv8DAQIBAwADAQIBBAEBAgQJ//8CAgECAAACAwEEAAUjBgEB/wEBAf8CAAMDAwAADQH/AQEB/wIAAwMDAAABAQMBAQEBAQMBAQEBAQEABQP//wEDAgIAAAEEAgMACAEBAwECAQEBAwECAQEBAAUJ//8CAgEDAAACAwEEAAUkBAEB/wEBAP8BAAIDAAAAAf//AwQEBAAAAQICAgEEAwUI/wECAQL/AAEDAgQAAAsDAQMDAP8CAQMEAAAABSUEAQEDAQEBAwEDAQEBBAEBAAQB/wMDAAQEAAECAAMEAAb/AwEDAwMAAgEDBAQBAwEFCwMBAwADAwIBAwAEBAEDAAUFIgQBAQMDAQMDAQICAQMEAgEAAwMBBQMC/wEDAQECAAEDAQEBAQIFBv8DAQID/wADAQIEAAALAwECAAP/AwECAAQAAAUmBAEBAQEBAQMBAQEBAQQBAQAEAf//AwMEBAAAAQECAwEDAgMG/wMBAwMBAAIBAwQBAQECBQsDAQMCAQEDAQQCAQEBAQEFBScEAQEDAgEBAQEDAgEBAQEBAAUD//8BAwMCAAABAwQCAAgBAQMCAgEBAQQCAwEBAQAFDP//BAICAQAABAIDAQAFKAQBAQICAQAAAQIDAQAAAQEAAwP//wECAwIAAAECBAMABwECAwMD/wECAwMDAAEDAgQI/wECAgL/AAECAwQAAAUpBAEBAwIBAQMBAwIBAQQBAQAEA///AQADAgAAAQAEAgAG/wMBBAP/AAIBBAMAAAsDAf8CAwADAQACBAAABSoEAQEBAgEBAwEBAgEBBAEBAAQD/wEBAQMCAAEBAQMCAQEBAwb/AwEEAwEAAgEEAwEBAQIFCAEBAQICAQEBAQIDAQEBAAUFKwQB//8DAgEDAAADAgEEAAP//wEABAMAAAEABAMABf8EAwEEAQADAgEEAQEBAwUH/wIBAAICAAIBAAMEAAUtAgX/BAMBAgIABAMBAgIBAgQFCgQDAQEC/wQDAQECAAEBAgMFLAQB//8DAQEDAAADAQEEAQEDBAX/BAIBBP8AAwIBBAAABv8DAQABAwADAQACBAAI/wEDAAIBAAEEAAMCAAVJBAP/BAEDAgEABAEDAgEBAQIFCAQBAwEBAQQBAwEBAQEBAQUIBAEDAwEBBAECAwEBAQEBBQgEAQMBAf8EAQMBAQABAQEEBU4CAwIBAQMCAQIBAQQDAQEBAQUDAv8BAwIBAgABBAMBAQECBQVSBAH/AAMBAQEAAAMBAQEBAQMFAf8AAwEBBAAAAwEBBAEBAwQD/wABAwQCAAABAwQCAAT/AAMCAwEAAAMCBAEABVUEAQQDAwEBAQQCAwEBAQEBAwUE//8DAgMBAAADAgQBAAr/AgEBBAQAAgEBBAQBAQIDCv8CAQQEBAACAQQEBAEEAwUFOAQB/wIDAQEBAAIDAQEBAQEDBQH/AgMBAQQAAgMBAQQBAQMECP//AgMCAQAAAgQDAQAB/wIDAQH/AAIDAQEAAQEDBAVTBAH/AQMBAQEAAQMBAQEBAQEFAf8BAwEBBAABAwEBBAEBAQQI//8BAwIBAAABAwIBAQECBQH/AQMBAf8AAQMBAQABAQEEBTkEAf8DAwEBAQADBAEBAQEBAwUB/wMDAQEEAAIDAQEEAQEDBAP/AQEDBAIAAQEDBAIBAQECCAEBAwMCAQEBAwQCAQEBAAUFOgQB/wQDAQEBAAQDAQEBAQEDBQT/AQMCAwEAAQMCBAEBAQEFCP//BAMCAQAABAMCAQAL//8BAwMDAAABAwMDAQMDBQU8BAH//wABAQEAAAABAQEBAQMFBP//AAIDAQAAAAIDAQAI//8AAwIBAAAAAwIBAAoBAgEBBAQBAgEBBAQBAQADBT8EAQADAwEBAQADBAEBAQEBAwUBAAMDAQEEAAIDAQEEAQEDBAH//wIBAQEAAAIBAQEBAQMFAwABAQMEAgABAQMEAgEBAQIFVAQB//8BAQEBAAABAQEBAQECBQX/AgIBAv8AAgMBBAAACgICAQH//wIDAQEAAAEBAgME/wP/AgMBAAMAAgQBAAVBBAECAwMBAQECAwQBAQEBAQMFAf//BAEBAQAABAEBAQEBAwUE//8BAgMBAAABAgMBAQECBQECAwMBAf8CAwQBAQABAQMEBUIEAQMDAwEBAQIDBAEBAQEBAwUDAQEBAwQCAQEBAwQCAQEAAgT//wICAwEAAAIDBAEACv8BAQQEBAABAQQEBAEBAQIFMwQB/wADAgEBAAADAgEBAQEEBQP/AAEDBAMAAAECBAMABf8AAwECAQAAAwECAQEBAwUF/wADAQIEAAADAQIEAAVRBAEEAAMCAQEEAAMCAQEBAQQFBf//AgECAQAAAgEDAQEBAwUK/wIBAQEEAAIBAQEEAQECBAEEAAMCAf8EAAMCAQAABTUEAf8CAwIBAQACBAMBAQEBBAUI//8CAwMBAAACAwQBAAH/AgMCAf8AAgQDAQAABwECBAQE/wECBAQEAAEEAgQFNAQB/wEDAgEBAAEDAgEBAQEBBQX//wQBAgEAAAQBAgEBAQMFCP//AQMDAQAAAQMEAQEBAgUB/wEDAgH/AAEDAgEAAQEBBAU2BAH/AwMCAQEAAwQCAQEBAQQFA/8BAQMEAwABAQIEAwEBAQIIAQEDAwMBAQECAwQBAQEABQj//wMDAwEAAAIDBAEABTcEAf8EAwIBAQAEAwIBAQEBBAUI//8EAwMBAAAEAgMBAAv//wEEAwMAAAEEAgMAAf8EAwIB/wAEAwIBAAAFOwQB//8AAgEBAAAAAgEBAQEEBQX//wABAgEAAAABAgEBAQMFBf8BAwECAQABAwECAQEBAQUF/wEDAQIEAAEDAQIEAQEBAwUuBAEAAAMCAQEAAAMCAQEBAQQFAQADAwIBAQADBAIBAQEBBAUB//8CAgEBAAACAwEBAQEEBQMAAQEDBAMAAQECBAMBAQECBVAEAf//AQIBAQAAAQIBAQEBAgUF/wIDAQIBAAIEAQMBAQEDBQoCAwEBAQQCAwEBAQQBAQIECgIDAQEEBAIDAQEEBAEBAgMFMAQBAgADAgEBAgAEAwEBAQEEBQH//wQCAQEAAAQCAQEBAQQFBP//AQIDAgAAAQIEAwABAgADAgH/AgAEAwEAAAUxBAEDAAMCAQEDAAQCAQEBAQQFAwEBAQMEAwEBAQIEAwEBAAIF//8BAQIBAAABAQIBAQECBQr/AQEBAQQAAQEBAQQBAQEEBgAEAQIEBAMCAgEDBAIBAQECAAUE/wEBAwQDAAEBAgQDAQEBAgYBBAMBAgEBBAMBAgEBAQAFCQEBAwMDAQEBAgMEAQEBAAUGAQQBAgQEAgICAQMEAQEBAQIABQT/AQEDBAIAAQEDBAIBAQECBf//AwIDAQAAAwIEAQAJAQEDAwIBAQEDBAIBAQEABQYCBAECAP8CAf8CAAADAQAABP//AQL/AgAAAQIAAwAH/wMB/wECAAQBAAIDAAn/AQIDAv8AAQIEAwAABgMEAQL/AQIB/wIAAQMBAAEBAgQBAgMEAgQCAQIDAQQBAQIABQT//wECAQIAAAEDAgQACAECAwEDAQECAwEEAQEBAAUGBAQBAgIEBAICAQEDBAEBAQIABQECBAQEAgIBAgMEAQEBAgAFAf//BAQCAgAAAwQBAQECBAUE//8BAwQEAAABAwQEAQQEBQYFBAEC//8BAgICAAABAwQABAEBAQMEAQEBAQMEAQEBAAUJAQEDAwEBAQEDBAEBAQEABQsEAQEDBP8DAQECBAABAQECBgYEAQIEBAQCAgECAwQBAQECAAUE/wEBAwQEAAEBAgMEAQEBAgkBAQMDBAEBAQIDBAEBAQAFC///AQECBAAAAQECBAEBAgMGBwMBAgIEBAIEAQECAwEEAQIABAn/AQEDAQEAAQEDAQEBAQEFBv8EAQECAgAEAQECAgIBAgMCBAUGCAQBAgQCBAICAQMBBAEBAQIABQT//wEDAgQAAAEDAgQAB/8DAwMBAQACAwQBAQEBBAUJAQEDAQQBAQEDAQQBAQEABQYKBAH//wQDAQIAAAQDAQIAAQIDBAP/AgECBAMAAQECAAUE//8BAgQDAAABAgQDAAn/AQIDAwEAAQIDBAABAQEFBgsEAf//BAMDAgAABAIDAQAB//8EAwP/AAACAQEAAQMDBAf/AwIBAf8AAwIBAQABAQMECwQDAgEB/wQDAgEBAAEBAwQGDAMBAgT/////AQMAAAAAAAn/AQP///8AAQMAAAAAAQIEBP///wEDBAAAAAAGDQQBAv8BAwL/AgABBAMAAAEC/wQDBAIBAAMCBAEBAgAFBP8BAQMBAwABAQMBBAEBAQQJ/wEDAwMDAAEDAwMDAQMCBQYOBAECAQEBAgICAQEBAwQBAQEDAf//BAMEBAAAAgEDBAAG/wEBAQIBAAEBAQIBAQEBBQj/AgEBAgIAAgEBAwQBAQIDBg8EAQIEAgMCAgEDAQIBAQECAAUE/wEBAwIDAAEBAwIEAQEBAgf/AwIDAf8AAwIEAQAACQEBAwEDAQEBAwEEAQEBAAUGEAQBAv8CAwH/AgADBAEAAAT//wECAgMAAAECAwQAB/8BAgMBAgABAgQBAwEBAQQJ/wECAQMAAAECAQMAAQEBAwYRBAEC/wIDA/8BAAIDBAAABP//AQQCAwAAAQQCAwAH/wMCAQEAAAQDAQIAAAn/AQQBAwIAAQQBAwIBAQEDBhIEAQIEAgMCBAEDAQIBBAECAAQD//8CAQMCAAACAQQDAAgCAgECAgICAgEDAwQBAgAFC/8BAQEBAgABAQEBAgEBAQQGEwQBAgECAQECAgEDAQEEAQEBBAECAQIBAQACAQMBAQABAQEEA/8BAgEDAgABAgEEAwEBAQMI/wIBAgIBAAIBAwQBAQECBQYUBAECAQIBAwACAQMBBAABAQEDB/8DAgEDAAADAgEEAAAI/wIBAgIDAAIBAwMEAQIBBAsE/wIDAQAEAAIDAQAABhUEAQIBAgACAAIBAwAEAAAD//8CAQMBAAACAQMBAQEDBQj/AgECAQIAAgEDAQQBAQIEDAMCAwH//wMCBAEAAAAGFgQBAgECAgICAgEDAwMEAQIABQIBAwECAQQBAwECAQQBAQAEA///AgEDAwAAAgEDBAAI/wIBAgP/AAIBAwQAAAYXBAECAQIBAAADAQQCAAAABAEBAQECAwEBAQECAwEBAAMH/wMCAwEBAAMCBAEBAQEEBQn/AQEBAwEAAQEBAwEBAQEFBhgEAQIBAgEBAgIBAwEBBAEBAQQE//8BAgIDAAABAgMEAAj/AgECAgEAAgEDBAEBAQIFCf8BAgEDAQABAgEDAQEBAQUGGQQBAgIBAwAAAgMBBAAAAAECAgIDBAQBAQECAwQBAgACB/8DAgIBAAAEAgMBAAAJAQEBAQMDAQEBAQMEAQEAAwYaBAECBAMDAgIBBAIDAQEBAgAFBP8BAQMDAwABAQMDAwEBAQIG/wQDAQEBAAQDAQEBAQEDBQkBAQMCAwEBAQMCBAEBAQAFBhsEAQIBAwMBAQIBAwQBAQEBAQUE//8BAgMDAAABAgMEAAj/AgEDBAEAAgEDBAEBAQIFCf8BAgID/wABAgMEAAAGHAQBAv8DAwP/AQACAwQAAAYBBAMCAQEBBAMCAQEBAQAFCf8BBAID/wABBAIDAAAL//8CAQEDAAACAQEEAQEDBAYdBAH//wQBAgEAAAQBAgEBAQMFBP//AQMDAQAAAQMEAQEBAgUJ/wEDAgEBAAEDAgEBAQEBBQb/BAEBAf8ABAEBAQABAQIEBh4EAQIBAwECAQIBBAEDAQEBAQUBAv8DAwIEAQACAwEEAQIABAP//wIBBAIAAAIBBAMACAICAQMC/wICAQQDAAECAAQGHwQBAgIDAwICAQECAwEBAQIABQT//wEBAwMAAAEBAwQBAQIDCP8CAQMAAgACAQQAAwAJAQEBAgMBAQEBAgQBAQEABQYgBAECAQEBAgECAQEBAwEBAQEFBP8DAQEDAwACAQEDBAEBAgMG/wQDAwEBAAQCAwEBAQEEBQn/AQECAwMAAQECAwQBAQECBiEEAQL/AQICAgIAAQMDBAECAAUE/wEBAwECAAEBAwECAQEBBAf/AwECAQMAAwECAQQBAQIECP8CBAED/wACBAEDAAAGIwYBAv8CAgL/AgADAwMAAA4B/wEBAf8CAAMDAwAAAQIEAgICAgEDAQEBAQECAAUE//8BAwICAAABBAIDAAkBAQMBAgEBAQMBAgEBAQAFCv//AgIBAwAAAgMBBAAGJAQBAgACAgEAAgADBAEAAAT//wECAgIAAAECAgIBAgMFCf8BAgEC/wABAwIEAAAK//8BAgEDAAABAgEEAQECBAYlBAECAAIBAgACAAMBBAAAAQIEAgICBAECAQEBAwECAAQH/wMBAwMDAAIBAwQEAQMBBQr//wIEAQMAAAIEAQMABiIEAQIAAQECAgIAAQEDBAEBAgMBAgQEAgQEAQICAQMEAQQBBQQCAQEDAQECAQEDAQEBAQEFB/8DAQID/wADAQIEAAAGJgQBAgACAQAAAgADAQAAAAECAgICAgQBAQEBAQQBAgAEBP//AQECAgAAAQECAwEBAgMH/wMBAwMBAAIBAwQBAQECBQYnBAECBAMCAgIBAwIBAQEBAgAFBP8BAQMDAgABAQMEAgEBAQIG/wQCAQH/AAQCAQEAAQEDBAkBAQMCAgEBAQQCAwEBAQAFBigEAQIDAwL/AgECAwEAAQECAAUE//8BAgMCAAABAgQDAAgBAgMDA/8BAgMDAwABAwIECf8BAgIC/wABAgMEAAAGKQQBAgADAQIBAgAEAQMBAQEDBQECAAMBAv8CAAQBAwAAAQIEAwICBAEDAgEBBAECAAQH/wMBBAP/AAIBBAMAAAYqBAECAgMCAgQBAQIBAQQBAgAEBP8B",
    "AQEDAgABAQEDAgEBAQMH/wMBBAMBAAIBBAMBAQECBQkBAQECAgEBAQECAwEBAQAFBisEAQIB/wECAgMBAAIEBAECAAUB//8EAwIEAAADAgEEAAb/BAMBBAEAAwIBBAEBAQMFCP8CAf8CAgACAQADBAAGLQQG/wQDAQICAAQDAQICAQIEBQECAQQDAP8CAQQDAAAAAQIEBAMA/wEDBAIAAAALBAMBAQL/BAMBAQIAAQECAwYsBAH//wQCAgQAAAMBAQQBAgMEAQIEBAICBAECAwEBBAECAAQG/wQCAQT/AAMCAQQAAAf/AwH/AwMAAgEAAwQABkoEAQAABAICBAAAAwEBBAECAwQBAAQEAgIEAAIDAQEEAQIDBAT/BAEDAgEABAEDAgEBAQIFCQQBAwEBAQQBAwEBAQEBAQUGTwIEAgEBAwIBAgEBBAMBAQEBBQQC/wEDAgECAAEEAwEBAQIFBlIEAf8ABAICAgAAAwEBAQECAwUC/wADAQEEAAADAQEEAQEDBAIEAwMBAQEEAgMBAQEBAQMFBP8AAQMEAgAAAQMEAgAGVQQBBAAEAgICAwAEAQEBAQIDBQEEBAQCAgICAwQBAQEBAgMFBAEBAQMEAgEBAQMEAgEBAAIF//8CAgMBAAACAwQBAAY4BAH/AgQCAgIAAQMBAQEBAgEFAv8BAwEBBAABAwEBBAEBAQQJ//8BAwIBAAABAwIBAQECBQH/AgQCAv8AAQMBAQABAgEEBlMEBf//BAIDAQAABAIDAQAI//8BBAMCAAABBAMCAAv/AwEEBAQAAwEEBAQBBAMFBgEEAgH//wEEAgEAAAEBAAMGOQQB/wMEAgICAAIDAQEBAQIDBQL/AgMBAQQAAgMBAQQBAQMECf//AgMCAQAAAgQDAQAB/wMEAgL/AAIDAQEAAQIDBAY6BAH/BAQCAgIAAwQBAQEBAgMFAv8DAwEBBAACAwEBBAEBAwQE/wEBAwQCAAEBAwQCAQEBAgkBAQMDAgEBAQMEAgEBAQAFBjwEAf//AAICAgAAAAEBAQECAwUC/wQDAQEBAAQDAQEBAQEDBQX//wACAwEAAAACAwEABf8BAwIDAQABAwIEAQEBAQUGPwQBAAAEAgICAAADAQEBAQIDBQEABAQCAgIAAwQBAQEBAgMFAgADAwEBBAACAwEBBAEBAwQB//8CAgICAAABAQEBAQICBQZUBAH//wECAgIAAAECAgIBAgMFCwECAQEEBAECAQEEBAEBAAMLAQIBBAQEAQIBBAQEAQEAAgb/AQIBAv8AAQIBAwABAQEDBkAEAf//AwICAgAAAgEBAQECAwULAwIBAQQEAwIBAQQEAQECAwsDAgEEBAQDAgEEBAQBBAMFBv8DAgEC/wAEAgEDAAAGQgQBAwAEAgICAgADAQEBAQIDBQEDBAQCAgICAwQBAQEBAgMFAv//BAEBAQAABAEBAQEBAwUF//8BAgMBAAABAgMBAQECBQYzBAH/AAQDAgIAAAMCAQEBAgQFBP8AAQMEAwAAAQIEAwAG/wADAQIBAAADAQIBAQEDBQb/AAMBAgQAAAMBAgQABlEEBAEBAQMEAwEBAQIEAwEBAAIG//8BAQIBAAABAQIBAQECBQv/AQEBAQQAAQEBAQQBAQEEC/8BAQEB/wABAQEBAAEBAQQGNQQB/wIEAwICAAEDAgEBAQIBBQb//wQBAgEAAAQBAgEBAQMFCf//AQMDAQAAAQMEAQEBAgUB/wIEAwL/AAEDAgEAAQIBBAY0BAYBBAMBAgEBBAMBAgEBAQAFBgH//wECAQEBAQECAQEBAAUG//8DAQIBAAADAQIBAQEDBQb//wMBAgQAAAMBAgQABjYEAf8DBAMCAgACBAMBAQECBAUJ//8CAwMBAAACAwQBAAH/AwQDAv8AAgQDAQAACAECBAQE/wECBAQEAAEEAgQGNwQB/wQEAwICAAMEAgEBAQIEBQT/AQEDBAMAAQECBAMBAQECCQEBAwMDAQEBAgMEAQEBAAUJ//8DAwMBAAACAwQBAAY7BAH//wADAgIAAAACAQEBAgQFAv8EAwIBAQAEAwIBAQEBBAUG//8AAQIBAAAAAQIBAQEDBQn//wADAwEAAAADBAEABi4EAQAEBAMCAgADBAIBAQECBAUB//8CAwICAAABAgEBAQICBQQAAQEDBAMAAQECBAMBAQECBgAEAwECAQAEAwECAQEBAwUGUAQB//8BAwICAAABBAIDAAb/AQMBAgEAAQMBAgEBAQEFBv8BAwECBAABAwECBAEBAQMLAQMBAQEEAQMBAQEEAQEABAYvBAH//wMDAgIAAAIDAQEBAgQFBv8DAwECAQADBAECAQEBAwULAwMBAQEEAgMBAQEEAQECBAsDAwEBBAQCAwEBBAQBAQIDBjEEAv//BAIBAQAABAIBAQEBBAUF//8BAgMCAAABAgQDAAr/AQICAv8AAQICAgABAgIEAQMEBAP//wECAwEAAAEDAAMHAAUBAwIAAAADAgEAAAADAAEDAgAAAAMDAgAAAAQAAwEDAwIBAQEDBAIBAQEBAAUF//8BAwQDAAABAgQDAAcBBAMBAgEBBAMBAgEBAQAFBwEEAQMBAAADAwIBAAADBAADAQMDAQEBAQMEAQEBAQEABQX//wEDBAIAAAEDBAIACgEBAwMCAQEBAwQCAQEBAAUHAgQBAwH/AwL/AwEABAIAAAX//wEC/wIAAAECAAMACP8DAf8BAgABAgQDAAAK/wECAwL/AAECBAMAAAcDBAEDAf8DAgADAQAEAgAAAQP/AgMCAAMAAQQCAAADAQIDAQMBAQIDAQQBAQEABQX//wECAQIAAAEDAgQABwQEAQMDAAABAwIDAAABBAADAQEDAwEBAQEDBAEBAQEABQMBAwMDAQEBAgMEAQEBAQAFA///AwMBAQAAAwQBAQEBBAUHBQQBAwAAAAMDAQAAAAIDAAUBAQEDBAEBAQEDBAEBAQAFB/8EAQECBAADAQECBAEBAgMKAQEDAwEBAQEDBAEBAQEABQcGBAEDAwAAAQMCAwAAAQQAAwEDAwMBAQECAwQBAQEBAAUF/wEBAwQEAAEBAwQEAgEBAgQEBQoBAQMDBAEBAQIDBAEBAQAFBwcFAQMDAAIB/wMEAAIBAAABAwMAAgP/AgMAAQQAAAEDAAACAQMDAAACAQQAAwEBAwMBAwEBAgMBBAEBAAQK/wEBAwEBAAEBAwEBAQEBBQcIBAEDAwAAAQECAwAAAQEBAQQFAwEDAQMBAQEDAQQBAQEBAAUI/wMDAwEBAAIDBAEBAQEEBQoBAQMBBAEBAQMBBAEBAQAFBwoEAQMC/wACAwMBAAACBAAF//8BAgADAAABAgADAAn/AgEAAAEAAwEAAAIACv8BAgMD/wABAgMEAAAHCwQBAwIBAAD/AwIBAAAAAAMB/wMCAv8BAAQCAwAABP//AgEB/wAAAgEBAAEBAwQI/wMCAQH/AAMCAQEAAQEDBAcMAwMBA/////8BAwAAAAAACv8BA////wABAwAAAAADAQMD////AQMEAAAAAAcNBAEDAgAAAAACAQAAAAAAAQP/AgQD/wIAAQQDAAAF/wEBAwEDAAEBAwEEAQEBBAr/AQMDAwMAAQMDAwQBAwIFBw4EAQMAAAAAAAEAAAAAAAABAwICAgMDAgEBAQMEAQIBAwT/AgIBAgIAAgIBAwQBAgEFCf8CAQECAgACAQEDBAEBAgMHDwQBAwIAAAABAwIAAAABAAMBAwECAQEBAwECAQEBAQAFBf8BAQMCAwABAQMCBAEBAQIKAQEDAQMBAQEDAQQBAQEABQcQBAED/wMEAv8CAAMEAQAABf//AQICAwAAAQIDBAAJ/wIBAgABAAMBBAACAAr/AQIBA/8AAQIBAwABAQEDBxEEAQMCAQAAAQQDAQAAAgABA/8DBAT/AQACAwQAAAX//wEEAgMAAAEEAgMACv8BBAEDAgABBAEDAgEBAQMHEgQBAwAAAAABAwAAAAABAAEDAgMCA/8CAQMBBAABAgEDAwEDAQIBAwEDAQIBBAEBAAQJAgIBAgICAgIBAwMEAQIABQcTBAEDAgMCAgMCAQMBAQQBAgEEAwECAQIAAwECAQMABAEBAAIJ/wIBAgIBAAIBAwQBAQECBQv/AgEAAAMAAgEAAAMABxQEAQMAAQAAAQMAAgAAAQABAwIDAgT/AgEDAQQAAAn/AgECAgMAAgEDAwQBAgEEDP8BAgABAgABAwACBAAHFQQBAwIAAQMBAwIAAQQBAQEDBQED/wMEAwQBAAECAQMBAwAEBP//AgEDAQAAAgEDAQEBAwUJ/wIBAgECAAIBAwEEAQECBAcWBAEDAgADAAEDAgAEAAEAAwEDAQIEBAEDAQIEBAIBAAIEBAUE/wICAQMDAAIDAQQEAQMEBQn/AgECA/8AAgEDBAAABxcEAQMCAAABAQMCAAABAQEBBAUFAQEBAQIDAQEBAQIDAQEAAwj/AwIDAQEAAwIEAQEBAQQFCv8BAQEDAQABAQEDAQEBAQUHGAQBAwIDAgIDAgEDAQEEAQIBBAX//wECAgMAAAECAwQACf8CAQICAQACAQMEAQEBAgUK/wECAQMBAAECAQMBAQEBBQcZBAEDAAIAAAEDAAIAAAEAAwH/AQIDAwEAAQIDBAEBAAIDAQMBAgMBAQMBAgQBAQEABQn/AgECAgQAAgEDAwQBAgEEBxoEAQMCAAAAAgMCAAAAAQADAQMCAgEBAQQCAwEBAQEABQX/AQEDAwMAAQEDAwMCAQECAwMFCv8BAwIDAQABAwIEAQEBAQUHGwQBAwIEBAICAgEDBAEBAQIBBQEDBAQE//8BAgMEAAAABf//AQIDAwAAAQIDBAAK/wECAgP/AAECAwQAAAccBAH//wEAAAIAAAEAAAMAAQP/BAQE/wEAAgMEAAAHAQQDAgEBAQQDAgEBAQEABQr/AQQCA/8AAQQCAwAABx0EAQMAAAADAgIAAAADAQABAwAAAgMCAgAAAQMBAQIDBQEDAAQAAwICAAQAAwEAAQMABAIDAgIABAEDAQECAwUHHgQBAwAAAAACAgAAAAABAAEDAgQCAwICAQQBAwEBAgEFA///AgIBAwAAAgMBBAAJ/wIBAwL/AAIBBAMAAAcfBAEDAgAAAQIEAgAAAQMAAQMDBAQDAwEBAgMBAQEDAAUH//8DAAECAAADAAECAAoBAQECAwEBAQECAwEBAQAFByAEAQMCAgIDAgMBAQEDAQICAQUDAAQDAQECAgMBAQECAwQBAQEABQf/BAMDAQEABAIDAQEBAQQFCv8BAQIDAwABAQIDBAEBAQIHIQQBA/8CAwMDAgABAwQEAQMABQMBAwMBAwEBAgMBBAEBAQAFBf8BAQMBAgABAQMBAgEBAQQI/wMBAgEDAAMBAgEEAQECBAcjBQED/wMDA/8CAAMDAwAAAwEDAQEBAQEDAQEBAQEBAAUF/wEBAwICAAEBBAIDAQEBAgj/AwEDAQMAAgEDAQQBAQIECgEBAwECAQEBAwECAQEBAAUHJAQBA///AwIBAwAABAIBAAED/wMDAv8CAAMEAQAABf//AQICAgAAAQICAgECAwUK/wECAQL/AAEDAgQAAAclBAEDAAADAwECAAADBAEAAwEDAQEBAwEDAQEBBAEBAAQG//8CAAEBAAACAAEBAQEEBQj/AwEDAwMAAgEDBAQBAwEFByIEAQMBAAIDAAMBAAIEAAAD/wMDAQMDAAICAQMEAQMBBQUCAQEDAQECAQEDAQEBAQEFCP8DAQIDAwADAQIEBAEDAQUHJgQBA/8DAwH/AgADBAEAAAMBAQEBAQMBAQEBAQQBAQAEBf//AQEDAwAAAQECAwEBAgMI/wMBAwMBAAIBAwQBAQECBQcnBAEDAQAAAwIDAQAABAIAAwEDAgEBAQEDAgEBAQEBAAUF/wEBAwMCAAEBAwQCAQEBAgr/AQMCAgEAAQQCAwEBAQEFBygEAQMEBAP/AwECAwEAAQEDAAUF//8BAgMCAAABAgQDAAkBAgMDA/8BAgMDAwABAwIECv8BAgIC/wABAgMEAAAHKQQBAwAAAwMCAgAAAwQBAAMBAwIBAQMBAwIBAQQBAQAEBv//AgACAQAAAgADAQAI/wMBBAP/AAIBBAMAAAcqBAMBAQIBAQMBAQIBAQQBAQAEBf8BAQEDAgABAQEDAgEBAQMI/wMBBAMBAAIBBAMBAQECBQoBAQECAgEBAQECAwEBAQAFBysEAQMAAAIAAwIAAAEAAwAB//8DAgEEAAADAgEEAAf/BAMBBAEAAwIBBAEBAQMFCf8CAQACAgACAQADBAAHLQQBAwIAAAEDAwIAAAEEAAf/BAMBAgIABAMBAgIBAgQFAQMCAAAB/wMCAAABAAABAwIABAH/AwIABAEAAAcsBAEDAQACAwMDAQACBAQBAwAFA///AwEBAwAAAwEBBAEBAwQG//8CAAMBAAACAAMBAAf/BAIBBP8AAwIBBAAAB0sEBf8EAQMCAQAEAQMCAQEBAgUKBAEDAQEBBAEDAQEBAQEBBQoEAQMDAQEEAQIDAQEBAQEFCgQBAwEB/wQBAwEBAAEBAQQHRAIFAgEBAwIBAgEBBAMBAQEBBQUC/wEDAgECAAEEAwEBAQIFB1IEAf8AAAMDAwAAAAEBAQEDAwUD/wADAQEBAAADAQEBAQEDBQP/AAMBAQQAAAMBAQQBAQMEAwMDAwEBAQIDBAEBAQEBAwUHVQQDAgMDAQEBAgMEAQEBAQEDBQP//wQBAQEAAAQBAQEBAQMFBv//AQIDAQAAAQIDAQEBAgUBBAEAAAP/BAEAAAMAAAc4BAH/AgADAwMAAQACAgIBAwMFBv//BAIDAQAABAIDAQAJ//8BBAMCAAABBAMCAAcBBAIB//8BBAIBAAABAQADB1MEAf8BAAAD/wABAAADAAADBAMDAQEBBAIDAQEBAQEDBQb//wMCAwEAAAMCBAEAAwQDAwEB/wQCAwEBAAEBAwQHOQQD/wEDAQEBAAEDAQEBAQEBBQP/AQMBAQQAAQMBAQQBAQEECv//AQMCAQAAAQMCAQEBAgUD/wEDAQH/AAEDAQEAAQEBBAc6BAH/BAADAwMAAgABAQEBAwMFA/8CAwEBAQACAwEBAQEBAwUD/wIDAQEEAAIDAQEEAQEDBAr//wIDAgEAAAIEAwEABzwEAf//AAMDAwAAAAEBAQEDAwUD/wMDAQEBAAMEAQEBAQEDBQP/AwMBAQQAAgMBAQQBAQMEBf8BAQMEAgABAQMEAgEBAQIHPwQB//8CAwMDAAABAgICAQMDBQMAAwMBAQEAAwQBAQEBAQMFAwADAwEBBAACAwEBBAEBAwQFAAEBAwQCAAEBAwQCAQEBAgdUBAH//wEDAwMAAAEDAwMBAwMFA/8EAwEBAQAEAwEBAQEBAwUG/wEDAgMBAAEDAgQBAQEBBQr//wQDAgEAAAQDAgEAB0AEAQEBAAADAwECAAAEBAEDBAUBAQEAAwMDAQIABAQEAQMDBQH//wMDAwMAAAEBAQEBAwIFAQEBAAAD/wECAAAEAAAHQQQB//8EAwMDAAACAQEBAQMDBQECAQAAA/8CAQAAAwAAB/8DAgEC/wAEAgEDAAABAgEAAP//AgEAAAAAAAczBAH/AAAAAAMAAAAAAAEAAf8AAAQAAwAAAAIAAQAB/wAABAMDAAAAAgEBAQMEBQP/AAMCAQEAAAMCAQEBAQQFB1EEAQQCAAAAAwMBAAAAAgABBAIAAAMDAwEAAAICAQMEBQP//wQCAQEAAAQCAQEBAQQFBv//AQIDAgAAAQIEAwAHNQQB/wIAAAADAAEAAAACAAH/AgAAAwMAAQAAAgIBAwQFAf8CAAQAAwABAAMAAgAB/wIABAMDAAEABAIDAAc0BAH/AQAAAAMAAQAAAAMAAf8BAAQAAwABAAQAAwAH//8CAQIBAAACAQMBAQEDBQH/AQAAAP8AAQAAAAAABzYEAf8DAAAAAwABAAAAAgAB/wMABAADAAEAAwACAAP/AQMCAQEAAQMCAQEBAQEFB///BAECAQAABAECAQEBAwUHNwQB/wQAAAADAAIAAAABAAH/BAAEAAMAAgADAAEAAf8EAAQDAwACAAMBAQEDBAUD/wIDAgEBAAIEAwEBAQEEBQc7BAH//wAAAAMAAAAAAAEAAf//AAQAAwAAAAIAAQAB//8ABAMDAAAAAgEBAQMEBQP/AwMCAQEAAwQCAQEBAQQFBy4EAQACAAAAAwABAAAAAgABAAIAAAMDAAEAAAICAQMEBQEAAgAEAAMAAQADAAIAAQACAAQDAwABAAQCAwAHUAQB//8BBAMDAAABBAIDAAP/BAMCAQEABAMCAQEBAQQFCv//BAMDAQAABAIDAQAD/wQDAgH/AAQDAgEAAAcvBAEBAgAAAAMBAgAAAAMAAQECAAQAAwECAAQAAwAB//8DBAMDAAABAgEBAQMCBQf/AgMBAgEAAgQBAwEBAQMFBzAEAQICAAAAAwECAAAAAwABAgIAAAMDAQIAAAMDAQMEBQECAgAEAAMBAgAEAAMAAf//BAQDAwAAAgMBAQEDBAUIAAQBBAMBAQH/AwIBAQEAAQECBAQBAwMCAQEBAwQCAQEBAQAFBv8BAQMEAwABAQIEAwEBAQIIAQQDAQIBAQQDAQIBAQEABQgBBAQBAwMBAQEBAwQBAQEBAQAFBv//AQMEAgAAAQMEAgAH//8DAgMBAAADAgQBAAsBAQMDAgEBAQMEAgEBAQAFCAIEAQQC/wQD/wMBAAQCAAAG//8BAv8CAAABAgADAAn/AwH/AQIABAEAAgMAC/8BAgMC/wABAgQDAAAIAwQB//8AAQABAAAAAQACAAEE/wMEAwQCAAEDAQQBAwIEBv//AQIBAgAAAQMCBAAKAQIDAQMBAQIDAQQBAQEABQgEBAQBAQMDAQEBAQMEAQEBAQAFBAEDAwMBAQECAwQBAQEBAAUE//8DAwEBAAADBAEBAQEEBQb//wEDBAQAAAEDBAQBBAQFCAUEAQT//wMEBAIAAAEDBAAEAQMD/wEDAQIDAAEEAQEABAYBAQEDBAEBAQEDBAEBAQAFCwEBAwMBAQEBAwQBAQEBAAUIBgQB//8BAQIEAAABAQIEAQECAwQBAwMDAQEBAgMEAQEBAQAFBv8BAQMEBAABAQIDBAEBAQILAQEDAwQBAQECAwQBAQEABQgIBAH//wEBAgIAAAEBAgICAQIDAgQFBAEDAQMBAQEDAQQBAQEBAAUJ/wMDAwEBAAIDBAEBAQEEBQsBAQMBBAEBAQMBBAEBAQAFCAoEA///BAMBAgAABAMBAgAG//8BAgQDAAABAgQDAAr/AgEEBAEAAgEEBAECAQIFBAMEC/8BAgMD/wABAgMEAAAICwQBBAMCAQH/BAMCAQEAAQEDBAQB/wMCAv8BAAQCAwAABf//AgEB/wAAAgEBAAEBAwQJ/wMCAQH/AAMCAQEAAQEDBAgMAwQBA/////8BAwAAAAAAC/8BA////wABAwAAAAAEAQMD////AQMEAAAAAAgNBAH/AwEBAQEAAwEBAQEBAQIFAwL/AQMC/wIAAQQDAAAG/wEBAwEDAAEBAwEEAQEBBAn/AwICAf8ABAIDAQAACA4EAf8BAQEBAQABAQEBAQEBAQUBBAMDAwQEAgEBAQMEAQMBAwX//wIBAgIAAAIBAwQACv8CAQECAgACAQEDBAEBAgMIDwQB//8BAQECAAABAQECAQECBAQBAwECAQEBAwECAQEBAQAFBv8BAQMCAwABAQMCBAEBAQILAQEDAQMBAQEDAQQBAQEABQgQBAMC/wIDAf8CAAMEAQAABv//AQICAwAAAQIDBAAJ/wECAwECAAECBAEDAQEBBAv/AQIBA/8AAQIBAwABAQEDCBEEBAH/AQICAAEAAgMEAAAG//8BBAIDAAABBAIDAAn/AwIDAQAAAwIEAQAAC/8BBAEDAgABBAEDAgEBAQMIEgQBBAMEAwT/AgEDAQQAAQMBAwQBAwECAQMBAwECAQQBAQAEBf//AgEDAgAAAgEEAwAKAgIBAgICAgIBAwMEAQIABQgTBAEEAwAD/wIEAgADAAEAAQQDBAMDBAIBAwEBBAEDAQQEAQIBAv8DAQIBAwAEAQEAAgr/AgECAgEAAgEDBAEBAQIFCBQEAQIBAgEBAgIBAwEBBAEBAQQDAgECAQMAAgEDAQQAAQEBAwT//wECAgMAAAECAwQACv8CAQICAAACAQMEAAAIFQQB/wABAQECAAABAQECAQECBAT//wECAQIBAAECAQMBAQIEBf//AgEDAQAAAgEDAQEBAwUK/wIBAgECAAIBAwEEAQECBAgWBAEEAwQEBAQCAQMDAwQBBAAFBAEDAQIEBAEDAQIEBAIBAAIEBAUF//8CAQMDAAADAQQEAQMEBQr/AgECA/8AAgEDBAAACBcEBAEBAQIBAQEBAQIBAQEBAAUGAQEBAQIDAQEBAQIDAQEAAwn/AwIDAQEAAwIEAQEBAQQFC/8BAQEDAQABAQEDAQEBAQUIGAQDAv8CAwH/AgADBAEAAAb//wECAgMAAAECAwQACv8CAQICAQACAQMEAQEBAgUL/wECAQMBAAECAQMBAQEBBQgZBAEEAQMBAQIEAQMBAQIBAQEEBAEDAQIDAQEDAQIEAQEBAAUEAQEBAgMDAQEBAgMEAQEAAgr/AgECBAQAAgEDBAQBBAQFCBoEBAEDAgIBAQEEAgMBAQEBAAUG/wEBAwMDAAEBAwMDAQEBAgn/AwIEAf8AAwIEAQAACwEBAwIDAQEBAwIEAQEBAAUIGwQDAgEDAwEBAgEDBAEBAQEBBQQBAgIC//8BAgMEAAAABgAAAQIDAwAAAQIDBAEDBAUL/wECAgP/AAECAgQAAQICAwgcBAEEAwIAAQAEAwIAAQAAAwIBAwADAAIBAwAEAAAIAQQDAgEBAQQDAgEBAQEABQv/AQQCA/8AAQQCAwAACB0EA///BAECAQAABAECAQEBAwUG//8BAwMBAAABAwQBAQECBQv/AQMCAQEAAQMCAQEBAQEFCP8EAQEB/wAEAQEBAAEBAgQIHgQB/wEBAQEDAAEBAQEEAQEBBAMCAQMBAgECAQQBAwEBAQEFBAEDAgIBAwEDAgIBBAEBAAQKAgIBAwL/AgIBBAMAAQIABAgfBAEEAwEAAv8EAwEAAgAABAEBAgIBAQEBAgMBAQEBAAUG//8BAQMDAAABAQMEAQECAwsBAQECAwEBAQECAwEBAQAFCCAEAQQDAwMEAwIBAQEDAQEDAQUEAQECAgMBAQECAwQBAQEABQj/BAMDAQEABAIDAQEBAQQFC/8BAQIDAwABAQIDBAEBAQIIIQQBBP8DBAT/AgABAwQAAAQBAwMBAwEBAgMBBAEBAQAFBv8BAQMBAgABAQMBAgEBAQQJ/wMBAgEDAAMBAgEEAQECBAgjBQEE/wQEBP8CAAMDAwAABAEDAQEBAQEDAQEBAQEBAAUG/wEBAwICAAEBBAIDAQEBAgn/AwEDAf8AAgEDAQABAQIECwEBAwECAQEBAwECAQEBAAUIJAQB//8AAQACAAAAAQADAAEE/wQEA/8CAAMEAQAABv//AQICAgAAAQICAgECAwUL/wECAQL/AAEDAgQAAAglBAEEAQEBAAIEAQECAAMBAQEDBAEDAQEBAwEDAQEBBAEBAAQHAQMCAgMBAQMCAgQBAQICAwn/AwEDAwMAAgEDBAQBAwEFCCIEAwL/AQICBAIAAQMDBAECAAQEAQMDAQMDAQICAQMEAgEAAwMBBQYCAQEDAQECAQEDAQEBAQEFCf8DAQIDAwADAQIEBAEDAQUIJgQBBAIEAwICAwEEAgEBAQIBBQQBAQEBAQMBAQEBAQQBAQAEBv//AQECAgAAAQECAwEBAgMJ/wMBAwMBAAMBAwQBAQECBQgnBAH/AgEBBAMAAgEBBAMBAQIDBAEDAgEBAQEDAgEBAQEBAAUG/wEBAwMCAAEBAwQCAQEBAgv/AQMCAgEAAQQCAwEBAQEFCCgEBAECAgH/AQECAwEAAQEBAAUG//8BAgMCAAABAgQDAAoBAgMDA/8BAgMDAwABAwIEC/8BAgIC/wABAgMEAAAIKQQDAv8DAQACAgAEAQADAAQBAwIBAQMBAwIBAQQBAQAEBgEBAQMDAgEBAQMEAgEBAAIJ/wMBBAP/AAIBBAMAAAgqBAQBAQIBAQMBAQIBAQQBAQAEBv8BAQEDAgABAQEDAgEBAQMJ/wMBBAMBAAIBBAMBAQECBQsBAQECAgEBAQECAwEBAQAFCCsEAQQD/wME/wMBAAIEAAAE//8DAgEDAAADAgEEAAj/BAMBBAEAAwIBBAEBAQMFCv8CAf8CAgACAQADBAAILQMI/wQDAQICAAQDAQICAQIEBQEEAwEBAv8EAwEBAgABAQIDAwIBBAT//wIBBAQAAAEEAgMILAQBBAL/AwT/AwEAAgQAAAT//wMBAQMAAAMBAQQBAQMEBAEDAwEBAwECAwEBBAEBAAQI/wQCAQT/AAMCAQQAAAhMBAb/BAEDAgEABAEDAgEBAQIFCwQBAwEBAQQBAwEBAQEBAQULBAEDAwEBBAECAwEBAQEBBQsEAQMBAf8EAQMBAQABAQEECEUCBgIBAQMCAQIBAQQDAQEBAQUGAv8BAwIBAgABBAMBAQECBQhSBAH/AAEBAAQAAAECAAQAAf8AAQQEBAAAAQQEBAEEAwUE/wADAQEBAAADAQEBAQEDBQT/AAMBAQQAAAMBAQQBAQMECDgEAf8CAQEEBAACAQEEBAEBAgMB/wIBBAQEAAIBBAQEAQQDBQQEAwMBAQEEAgMBAQEBAQMFB///AwIDAQAAAwIEAQAIUwQB/wEBBAQEAAEBBAQEAQEBAgQDAwMBAQECAwQBAQEBAQMFBgEBAQMEAgEBAQMEAgEBAAIH//8CAgMBAAACAwQBAAg5BAH/AwEEBAQAAwEEBAQBBAMFB///BAIDAQAABAIDAQAK//8BBAMCAAABBAMCAAH/AwEBAP8ABAECAAAACDoEBP8BAwEBAQABAwEBAQEBAQUE/wEDAQEEAAEDAQEEAQEBBAv//wEDAgEAAAEDAgEBAQIFAf8EAQEA/wAEAQIAAAAIPAQB//8ABAQEAAAAAQEBAQQDBQT/AgMBAQEAAgMBAQEBAQMFBP8CAwEBBAACAwEBBAEBAwQH//8AAgMBAAAAAgMBAAg/BAEAAgEBBAQAAgEBBAQBAQIDAQACAQQEBAACAQQEBAEEAwUB//8CBAQEAAABAwMDAQQDBQQAAwMBAQEAAwQBAQEBAQMFCFQEAf//AQEABAAAAQIABAAB//8BBAQEAAABBAQEAQQDBQT/AwMBAQEAAwQBAQEBAQMFBP8DAwEBBAACAwEBBAEBAwQIQAQBAQIBAQQEAQIBAQQEAQEAAwEBAgEEBAQBAgEEBAQBAQACAf//AwQEBAAAAQICAgEEAwUBAQIBAQD/AQQCAwAAAAhBBAH//wQEBAQAAAEBAQEBBAIFAQICAQEA/wMEAQIAAAAI/wICAQL/AAIDAQQAAAECAgEB//8CAwEBAAABAQIDCEIEAQMCAQEEBAMCAQEEBAEBAgMBAwIBBAQEAwIBBAQEAQQDBQT//wIBAQEAAAIBAQEBAQMFAQMCAQEA/wQDAQIAAAAIMwQB/wABAQEEAAABAQEEAQECBAT/AAMCAQEAAAMCAQEBAQQFBP//BAIBAQAABAIBAQEBBAUG/wABAwQDAAABAgQDAAg1BAH/AgEBAQQAAgEBAQQBAQIECP//AgECAQAAAgEDAQEBAwUB/wIBAQH/AAIBAQEAAQECBAUDAgIB//8EAgMBAAAACDQEAf8BAQEBBAABAQEBBAEBAQQGAQEBAwQDAQEBAgQDAQEAAgj//wEBAgEAAAEBAgEBAQIFAf8BAQEB/wABAQEBAAEBAQQINgQB/wMBAQEEAAMBAQEEAQECBAH/AwEBBAQAAwEBBAQBAQIDCAEEAwECAQEEAwECAQEBAAUIAf//AQIBAQEBAQIBAQEABQg3BAT/AQMCAQEAAQMCAQEBAQEFCP//BAECAQAABAECAQEBAwUL//8BAwMBAAABAwQBAQECBQH/BAEBAf8ABAEBAQABAQIECDsEBP//AAIBAQAAAAIBAQEBBAUE/wIDAgEBAAIEAwEBAQEEBQj//wABAgEAAAABAgEBAQMFC///AAMDAQAAAAMEAQAILgQBAAMBAQEEAAMBAQEEAQECBAEAAwEBBAQAAwEBBAQBAQIDAv//AQQDAwAAAQQCAwAEAAMDAgEBAAMEAgEBAQEEBQhQBAH//wEBAQQAAAEBAQQBAQIEBP8DAwIBAQADBAIBAQEBBAUG/wEBAwQDAAEBAgQDAQEBAgsBAQMDAwEBAQIDBAEBAQAFCC8EAQEDAQEBBAEDAQEBBAEBAAQBAQMBAQQEAQMBAQQEAQEAAwP//wEDAgIAAAEEAgMACP8BAwECAQABAwECAQEBAQUIMAQBAgMBAQEEAgMBAQEEAQECBAECAwEBBAQCAwEBBAQBAQIDBP//AQIBAQAAAQIBAQEBAgUI/wIDAQIBAAIEAQMBAQEDBQgxBAEDAwEBAQQCAwEBAQQBAQIEAQMDAQEEBAIDAQEEBAEBAgME//8CAgEBAAACAwEBAQEEBQj/AwMBAgEAAwQBAgEBAQMFCQAEAf8AAgICAAAAAQIDAAAC/wABAQEEAAABAQEEAQECBAUBAwMCAQEBAwQCAQEBAQAFB/8AAQMEAwAAAQIEAwAJAQQB/wACAgEAAAACAwEAAAL/AAEEBAQAAAEEBAQBBAMFBQEDAwEBAQEDBAEBAQEBAAUH/wABAwQCAAABAwQCAAkCBAH/AAECAf8AAAEDAgAAAwMB/wIB/wMBAAQCAAAH//8BAv8CAAABAgADAAr/AwH/AQIABAEAAgMACQMEAf8AAQIBAgAAAQMCBAAEAv8BAgH/AgABAwEAAQECBAUBAgMBAwEBAgMBBAEBAQAFB/8AAQIBAgAAAQMCBAEBAgQJBAQB/wAAAgMAAAAAAQIAAAH/AAICAwAAAAECAwAAAv8AAQECBAAAAQECBAEBAgMF/wADAwEBAAADBAEBAQEEBQkFBAH/AAICAAAAAAIDAAAAAf8AAgQAAAAAAQQAAAAHAQEBAwQBAQEBAwQBAQEABQn/AAEBAAAAAAECAAAACQYEAf8AAgIDAAAAAQIDAAAB/wAA/wMAAAAAAAEAAAUBAwMDAQEBAwMEAQECAQAFAwEDB/8BAQMEBAABAQIDBAEBAQIJBwMB/wAAAgAAAAAAAQAAAAUBAQMDAQMBAQIDAQQBAQAEAgQEAQP//wMEAQIAAAAJCAQB/wACAAMAAAACAAMAAAUBAwEDAQEBAwEEAQEBAQAFB/8AAQMCBAAAAQMCBAAK/wMDAwEBAAIDBAEBAQEEBQkKBAH/AAECAv8AAAECAwAABP8ABAMBAgAABAMBAgAI/wAEAQMCAAAEAQMCAAz/AQIDA/8AAQIDBAAACQsEAf8AAwICAQAABAIDAQACBAMCAQH/BAMCAQEAAQEDBAUB/wMCAgEBAAQCAwEBAQAFCv8DAgEB/wADAgEBAAEBAwQJDAMFAQP/////AQMAAAAAAAH/AAL///8AAAEAAAAABQEDA////wEDBAAAAAAJDQQB/wACAgICAAABAQEBAQICBQQC/wEDAv8CAAEEAwAABQED/wIDAQEDAAIEAQEBAAUK/wMCAgH/AAQCAwEAAAkOBAH/AAQEAgIAAAMEAQEBAgQFBAIBAQECAgIBAQEDBAEBAQMG/wICAQICAAICAQMEAQIBBQv/AgEBAgIAAgEBAwQBAQIDCQ8EAf8AAgACAAAAAgADAAAB/wACAgIDAAABAQECAQICBAUBAwECAQEBAwECAQEBAQAFB/8AAQMCAwAAAQMCBAAJEAQB/wABAgIDAAABAgMEAAT/AAIDAQIAAAIEAQMAB///AQICAwAAAQIDBAAK/wECAwECAAECBAEDAQEBBAkRBAH/AAMAAgEAAAMAAgEAAf8AAwICAwAAAgEBAwECAwQF/wABAgIBAAABAgMBAQECBQf//wEEAgMAAAEEAgMACRIEAgQDAQAAAAQDAQAAAAAB/wACBAIDAAABAwECAQICBAUBAwECAQMBAwECAQQBAQAEC/8CAQICAgACAQMDAwECAQUJEwQB/wABBAIDAAABBAIDAAQCAQIBAQICAQMBAQQBAQEECP8AAgEBAgAAAgEBAwEBAwQL/wIBAgIBAAIBAwQBAQECBQkUBAH/AAMEAgMAAAIEAQMAAwMCAQAAAwMCAQAABAAF/wABAgIDAAABAgMEAAv/AgECAAMAAgEDAAQACRUEAf8AAgMCAwAAAQIBAwECAgQFAQMBAgECAQQBAgEDAQEABAb//wIBAwEAAAIBAwEBAQMFC/8CAQIBAgACAQMBBAEBAgQJFgQFAQMBAgQEAQMBAgQEAgEAAgQEBQb//wIBAwMAAAIBAwQACP8AAwIBAgAABAIBAwAL/wIBAgP/AAIBAwQAAAkXBAH/AAAAAgAAAAAAAgAAAwMCAAAAAQMCAAAAAQAFAQEBAgEBAQEBAgEBAQEABQcBAQEBAgMBAQEBAgMBAQADCRgEAf8AAQACAAAAAQADAAAEAgECAQECAgEDAQEEAQEBBAf//wECAgMAAAECAwQAC/8CAQICAQACAQMEAQEBAgUJGQQB/wACAAICAAABAAIDAAMDAgIAAQAEAgMAAQAABQEDAQIDAQEDAQIEAQEBAAUL/wIBAgQEAAIBAwQEAQQEBQkaBAH/AAIBAgAAAAIBAwAAAf8AAgICBAAAAQEBBAECAgQFAQMCAgEBAQQCAwEBAQEABQf/AAEDAwMAAAEDAwMBAwMFCRsEAf8AAQECBAAAAQECBAEBAgMB/wABAgIEAAABAgIEAQIDBAUBAgIC//8BAgMEAAAAB/8AAQIDAwAAAQIDBAAJHAQB/wADAQIBAAADAQIBAQEDBQH/AAMCAgQAAAIBAQMBAgMEBf8AAgICAQAAAgMEAQAJAQQDAgEBAAQDAgEBAQEABQkdBAH/AAIBAAAAAAIBAAAAAf8AAgEABAAAAgEABAAB/wACAgAEAAABAgAEAAH/AAIEAAQAAAEDAAQACR4EAf8AAgQCBAAAAQMBBAECAgQEAgEDAQIAAgEEAQMAAQEBAwX/AAICAQMAAAIDAQQACwICAQMC/wICAQQDAAECAAQJHwQB/wAAAQIAAAAAAQIAAAH/AAACAgQAAAABAQQBAgMEBQEBAgIBAwEBAgMBBAEBAAQJAgABAQEBAgABAQEBAQECBQkgBAH/AAIBAgIAAAIBAwQABAIBAQECAQIBAQEDAQEBAQUG/wABAQICAAABAQMEAQECAwn/BAMDAQEABAIDAQEBAQQFCSEEAf8AAgIBAgAAAgMBBAAEAv8BAgICAgABAwMEAQIABQUBAwMBAwEBAgMBBAEBAQAFB/8BAQMBAgABAQMBAgEBAQQJIwYB/wACAAEAAAACAAEAAAH/AAICAQMAAAIDAQQABQH/AQEB/wIAAwMDAAAF/wABAQEBAAABAQEBAQECBQUBAwEBAQEBAwEBAQEBAQAFB/8BAQMCAgABAQQCAwEBAQIJJAQB/wABAAH/AAACAAMAAAQC/wICAf8CAAMEAQAABQECAwEEAQECAwEEAQEBAAUH//8BAgICAAABAgICAQIDBQklBAH/AAIEAQMAAAIEAQMABQEDAQEBAwEDAQEBBAEBAAQIAQACAAEAAQADAAIAAAr/AwEDAwMAAgEDBAQBAwEFCSIEBP8AAQIAAAAAAgQAAAAFAQMDAQMDAQICAQMEAgEAAwMBBQcCAAEDAQECAAEDAQEBAQIFCv8DAQIDAwADAQIEBAEDAQUJJgQB/wAAAAEAAAAAAAEAAAMDAQMCAQEDAQQCAQEBAQEFBQEBAQEBAwEBAQEBBAEBAAQK/wMBAwMBAAIBAwQBAQECBQknBAH/AAIBAQAAAAMBAgAABQEDAgEBAQEDAgEBAQEBAAUH/wEBAwMCAAEBAwQCAQEBAgn/BAIBAf8ABAIBAQABAQMECSgEAf8AAQEBBAAAAQEBBAEBAgQFAQICAf8BAQIDAQABAQEABQf//wECAwIAAAECBAMACwECAwMD/wECAwMDAAEDAgQJKQQDAwEEAgAAAwEEAgAAAAT/AAMCAAEAAAMCAAEABQEDAgEBAwEDAgEBBAEBAAQI/wACAgIBAAACAwQBAAkqBAH/AAABAQAAAAABAgAABQEBAgEBAwEBAgEBBAEBAAQH/wABAQMCAAABAQMCAQECAwr/AwEEAwEAAgEEAwEBAQIFCSsEAf8AAgQCAAAAAQMCAAAF//8DAgEDAAADAgEEAAn/BAMBBAEAAwIBBAEBAQMFC/8CAf8CAgACAQADBAAJLQQH/wABAQQDAAABAQQDAQECAwn/AAMBAgIAAAMBAgIBAgQFCf8ABAECAQAABAECAQEBAwUJ/wQDAQICAAQDAQICAQIEBQksBAH/AAIEAQAAAAIEAQAABf//AwEBAwAAAwEBBAEBAwQH//8BAwACAAABAwACAAn/BAIBBP8AAwIBBAAACU0EAQMAAgAAAAIAAQAAAAABAwACAAADAgABAAADAAEDAAICAAADAAECAAAAAQMAAgQAAAIAAQMAAAAJRgIHAgEBAwIBAgEBBAMBAQEBBQcC/wEDAgECAAEEAwEBAQIFCVUEAgMCAQEEBAMCAQEEBAEBAgMCAwIBBAQEAwIBBAQEAQQDBQX//wIBAQEAAAIBAQEBAQMFAgMCAQEE/wMCAQEEAAEBAgMJOAQB/wICAgEAAAIDBAEAAAL/AQEEBAQAAQEEBAQBAQECBQMDAwEBAQIDBAEBAQEBAwUHAQEBAwQCAQEBAwQCAQEAAglTBAH/AQICAQAAAQMEAgAABQIDAwEBAQIDBAEBAQEBAwUF//8EAQEBAAAEAQEBAQEDBQj//wECAwEAAAECAwEBAQIFCTkEAf8DAgIBAAAEAgMBAAAC/wIBAQQEAAIBAQQEAQECAwL/AgEEBAQAAgEEBAQBBAMFBQQDAwEBAQQCAwEBAQEBAwUJOgQC/wMBBAQEAAMBBAQEAQQDBQj//wQCAwEAAAQCAwEAC///AQQDAgAAAQQDAgAJAQQCAf//AQQCAQAAAQEAAwk8BAH//wACAQAAAAACAQAABf//AAEBAQAAAAEBAQEBAwUF/wEDAQEBAAEDAQEBAQEBBQX/AQMBAQQAAQMBAQQBAQEECT8EAQAAAgIBAAAAAgMBAAABAAMCAgEAAAQCAwEAAAIAAgEBBAQAAgEBBAQBAQIDAgACAQQEBAACAQQEBAEEAwUJVAQB//8BAgEAAAABAwIAAAX/AgMBAQEAAgMBAQEBAQMFBf8CAwEBBAACAwEBBAEBAwQF/wIDAQH/AAIDAQEAAQEDBAlABAH//wMCAQAAAAMCAQAAA///AQMDAwAAAQMDAwEDAwUF/wQDAQEBAAQDAQEBAQEDBQj/AQMCAwEAAQMCBAEBAQEFCUEEAQIAAgIBAAIAAwQBAAAB//8EAgEAAAAEAgEAAAIBAgEBBAQBAgEBBAQBAQADAgECAQQEBAECAQQEBAEBAAIJQgQBAwACAgEABAACAwEAAAX//wEBAQEAAAEBAQEBAQIFAQMAAgIB/wQAAgMBAAAJ/wICAQL/AAIDAQQAAAlRBAIDAwEBAQQCAwEBAQQBAQIEAgMDAQEEBAIDAQEEBAEBAgMF//8CAgEBAAACAwEBAQEEBQn/AwMBAgEAAwQBAgEBAQMFCTUEAf8CAgICAAABAgMEAAAC/wEBAQEEAAEBAQEEAQEBBAcBAQEDBAMBAQECBAMBAQACCf//AQECAQAAAQECAQEBAgUJNAQB/wECAgIAAAECAwQAAAX//wQCAQEAAAQCAQEBAQQFCP//AQIDAgAAAQIEAwAB/wECAgL/AAECAgIAAQICBAk2BAH/AwICAgAABAECAwAAAv8CAQEBBAACAQEBBAEBAgQJ//8CAQIBAAACAQMBAQEDBQH/AwICAv8AAgEBAQABAgIECTcEAv8DAQEBBAADAQEBBAEBAgQC/wMBAQQEAAMBAQQEAQECAwkBBAMBAgEBBAMBAgEBAQAFCQH//wECAQEBAQECAQEBAAUJOwQB//8AAgIAAAAAAQIAAAX//wACAQEAAAACAQEBAQQFBf8BAwIBAQABAwIBAQEBAQUJ//8AAQIBAAAAAQIBAQEDBQkuBAEAAAICAgAAAAECAwAAAgADAQEBBAADAQEBBAEBAgQCAAMBAQQEAAMBAQQEAQECAwH//wICAgAAAAECAwAACVAEAf//AQICAAAAAQIDAAAF/wIDAgEBAAIEAwEBAQEEBQX/AgMCAf8AAgQDAQAACwECBAQE/wECBAQEAAEEAgQJLwQBAQACAgIAAQACAwQAAAH//wMCAgAAAAMBAgAAA///AQQDAwAAAQQCAwAF/wQDAgEBAAQDAgEBAQEEBQkwBAECAAICAgABAAIDBAAAAf//BAICAAAABAECAAACAQMBAQEEAQMBAQEEAQEABAIBAwEBBAQBAwEBBAQBAQADCTEEAQMAAgICAAQAAQIDAAACAgMBAQEEAgMBAQEEAQECBAICAwEBBAQCAwEBBAQBAQIDBf//AQIBAQAAAQIBAQEBAgUKAAQB/wEDAwMBAAECAwQBAQEBBQMEAwEBAf8EAwEBAQABAQIEBgEDAwIBAQEDBAIBAQEBAAUI/wEBAwQDAAEBAgQDAQEBAgoBBAH/AQMDAgEAAQMEAgEBAQEFBgEDAwEBAQEDBAEBAQEBAAUG//8DAQEBAAADAQEBAQEDBQj//wEDBAIAAAEDBAIACgIEAf8BAgMC/wABAgQDAAAEAwH/AwL/AwEABAIAAAj//wEC/wIAAAECAAMAC/8DAf8BAgAEAQACAwAKAwQB/wECAAIAAAECAAMAAAH//wIDAgMAAAEDAgQABgECAwEDAQECAwEEAQEBAAUI//8BAgECAAABAwIEAAoEBAH/AQEDBAEAAQEDBAEBAQEFAf8BAwMEAQABAgMEAQEBAQUGAQEDAwEBAQEDBAEBAQEABQYBAwMDAQEBAgMEAQEBAQAFCgUEAQEBAwMBAQEBAwQBAQEBAAUDBAEBAwT/AwEBAgQAAQEBAggBAQEDBAEBAQEDBAEBAQAFCv8EAQECBAADAQECBAEBAgMKBgQB/wEDAwQBAAECAwQBAQEBBQP//wEBAgQAAAEBAgQBAQIDBgEDAwMBAQEDAwMBAQEBAAUI/wEBAwQEAAEBAgMEAQEBAgoHAwH/AQEDAQEAAQEDAQEBAQEFBgEBAwMBAwEBAgMBBAEBAAQDBAQBA///AwQBAgAAAAoIBAH/AQMBBAEAAQMBBAEBAQEFAf//AwMEBAAAAQECAgIDAgMEBAUGAQMBAwEBAQMBBAEBAQEABQj//wEDAgQAAAEDAgQACgoEAf8BAgMDAAABAgMEAAAFAgMAAwEAAgMABAEAAAYBAgAC/wEBAwAEAAIACP//AQIEAwAAAQIEAwAKCwQB/wEEAwP/AAEEAgMAAAMEAwIBAf8EAwIBAQABAQMEBv//AwICAQAABAIDAQAH//8CAQH/AAACAQEAAQEDBAoMAwYBA/////8BAwAAAAAAAf8BA////wABAwAAAAAGAQMD////AQMEAAAAAAoNBAH/AQMDAwMAAQMDAwMBAwIFBQIBAAACAgIBAAADBAAGAQP/AgMBAQMAAgQAAQEABQr/BAICAf8ABAIDAQAACg4EAf8BAAABAQABAAACAwAFAgEBAQICAgEBAQMEAQEBAwf/AgIBAgIAAgIBAwQBAgEFDP8CAQECAgACAQEDBAEBAgMKDwQB/wEDAQMBAAEDAQQBAQEBBQYBAwECAQEBAwECAQEBAQAFCP8BAQMCAwABAQMCBAEBAQIL/wMCAwH/AAMCBAEAAAoQBAH/AQIBA/8AAQIBAwABAQEDBQL/AgMBAAIAAwQBAAAI//8BAgIDAAABAgMEAAv/AwIDAQAAAwIEAQAAChEEAf8BBAEDAgABBAEDAgEBAQMGAf8BAgL/AQACAwQAAAj//wEEAgMAAAEEAgMADP8CAQL/AwACAQMABAAKEgQB/wEAAQEBAAEAAgMEAAUCAQIBAv8CAQMBBAABAQEDBgEDAQIBAwEDAQIBBAEBAAQM/wIBAgICAAIBAwMDAQIBBQoTBAH/AQABAQAAAQACAwAABAP/AAICAQQAAAIDAQAFAgECAQECAgEDAQEEAQEBBAz/AgECAgAAAgEDBAAAChQEAf8BAAEBAgABAAIDBAABBAMEAwMEAgEDAQEEAQMBBAUCAQIBA/8CAQMBBAABAQEDBgH/AQICAwEAAQIDBAEBAAIKFQQB/wEAAQABAAEAAgADAAQDAgMB//8DAgQBAAAABgH/AQIBAgEAAQIBAwEBAAQM/wIBAgECAAIBAwEEAQECBAoWBAH/AQABAv8AAQACAwAABgEDAQIEBAEDAQIEBAIBAAIEBAUH//8CAQMDAAACAQMEAAz/AgECA/8AAgEDBAAAChcEAf8BAQEDAQABAQEDAQEBAQUEAwIAAgEBBAIAAwEBAQEEBQgBAQEBAgMBAQEBAgMBAQADC/8DAgMBAQADAgQBAQEBBAUKGAQB/wEAAQEAAAEAAgMAAAUCAQIBAf8CAQMBAQABAQEECP//AQICAwAAAQIDBAAM/wIBAgIBAAIBAwQBAQECBQoZBAH/AQABAwMAAQACBAQBAwQFBAMCAAABAQMCAAABAQEBBAUGAQEBAgMDAQEBAgMEAQEAAgz/AgECBAQAAgEDBAQBBAQFChoEAf8BAwIDAQABAwIEAQEBAQUD//8BAQEDAAABAQEEAQECBAYBAwICAQEBBAIDAQEBAQAFCP8BAQMDAwABAQMDAwIBAQIDAwUKGwQB/wECAgP/AAECAwQAAAUCAQMDAQECAQMEAQEBAQEFBgECAgL/AAECAwQAAAAI//8BAgMDAAABAgMEAAocBAH/AQACAwIAAQACBAMAA///AgEBAwAAAgEBBAEBAwQGAf8CAgL/AQACAwQAAAoBBAMCAQEBBAMCAQEBAQAFCh0EAf8BAwIBAQABAwIBAQEBAQUF//8EAQIBAAAEAQIBAQEDBQj//wEDAwEAAAEDBAEBAQIFAf8BAwIB/wABAwIBAAEBAQQKHgQBAQEAAgH/AQEAAwIAAQEABAMEAQEBAQMEAQEBAQMBAQEEBQIBAwEC/wIBBAEDAAEBAQMGAQMCAgEDAQMCAgEEAgEABAICAwofBAH/AQECAwEAAQECAwEBAQEFBAP/AAIBAgQAAAIBAwAGAQECAgEBAQECAwEBAQEABQj//wEBAwMAAAEBAwQBAQIDCiAEAf8BAQIDAwABAQIDBAEBAQIFAgEBAQIBAgEBAQMBAQEBBQYBAQICAwEBAQIDBAEBAQAFCv8EAwMBAQAEAgMBAQEBBAUKIQQB/wED/wIDAAEDAAIEAAP/AgMBBP8AAgMBBAAABQL/AQICAgIAAQMDBAECAAUGAQMDAQMBAQIDAQQBAQEABQojBQUB/wEBAf8CAAMDAwAAAf8BAwECAQABAwECAQEBAQUB//8DAwIEAAACAwEEAAYBAwEBAQEBAwEBAQEBAQAFCP8BAQMCAgABAQQCAwEBAQIKJAQB/wECAQL/AAEDAgQAAAH//wIDAgQAAAECAQQBAgIEBQL/AgIB/wIAAwQBAAAI//8BAgICAAABAgICAQIDBQolBAL//wIEAQMAAAIEAQMABAMB/wIDAQMBAAIEAQEBAQUGAQMBAQEDAQMBAQEEAQEABAv/AwEDA/8AAgEDBAAACiIEBQL/AQICBAIAAQMDBAECAAQGAf8DAQMDAQACAQMEAQEAAwgC/wEDAQECAAEEAQEBAQIFC/8DAQID/wADAQIEAAAKJgQEAwEDAgEBAwEEAgEBAQEBBQYBAQEBAQMBAQEBAQQBAQAECP//AQECAgAAAQECAwEBAgML/wMBAwMBAAIBAwQBAQECBQonBAH/AQMCAgEAAQQCAwEBAQEFBgEDAgEBAQEDAgEBAQEBAAUI/wEBAwMCAAEBAwQCAQEBAgr/BAIBAf8ABAIBAQABAQMECigEAf8BAgICAAABAgMEAAAFAgP/AgEBAgQAAwEBAQEEBQYBAgIB/wEBAgMBAAEBAQAFCP//AQIDAgAAAQIEAwAKKQQEAwH/AgMCAwEAAgQCAQIDBQQDAQQC//8DAQQCAAAABgEDAgEBAwEDAgEBBAEBAAQIAQEBAwMCAQEBAwQCAQEAAgoqBAH/AQECAgEAAQECAwEBAQEFBgEBAgEBAwEBAgEBBAEBAAQI/wEBAQMCAAEBAQMCAQEBAwv/AwEEAwEAAgEEAwEBAQIFCisEAf8BAAMBAQABAAQCAwAG//8DAgEDAAADAgEEAAf//wIB/wIAAAIBAAMACv8EAwEEAQADAgEEAQEBAwUKLQQK/wQDAQICAAQDAQICAQIEBQH/AQADBP8AAQADBAAAAwQDAQEC/wQDAQECAAEBAgMFAgEEBP//AgEEBAAAAQQCAwosBAH/BAMDAf8ABAIDAQAABAMB/wID/wMBAAIEAAAG//8DAQEDAAADAQEEAQEDBAr/BAIBBP8AAwIBBAAACk4EAQQBAwEBAQQBAwEBAQEBAQUBBAEDAwEBBAECAwEBAQEBBQj/BAEDAgEABAEDAgEBAQIFAQQBAwEB/wQBAwEBAAEBAQQKRwIIAgEBAwIBAgEBBAMBAQEBBQgC/wEDAgECAAEEAwEBAQIFClIEAf8AAwMCAQAAAwQCAQAD/wABBAQEAAABBAQEAQQDBQMDAgEBBAQDAgEBBAQBAQIDAwMCAQQEBAMCAQQEBAEEAwUKVQQG//8BAQEBAAABAQEBAQECBQr/AgIBAv8AAgMBBAAAAQQEAwP//wIDAQEAAAEDAgMDAv8BBAQEAgABBAQEAQQDBQo4BAYCAwMBAQECAwQBAQEBAQMFBv//BAEBAQAABAEBAQEBAwUJ//8BAgMBAAABAgMBAQECBQH/AgMDAv8AAQIDAQABAgEECjkEA/8BAQQEBAABAQQEBAEBAQIGAwMDAQEBAgMEAQEBAQEDBQgBAQEDBAIBAQEDBAIBAQACCf//AgIDAQAAAgMEAQAKOgQD/wIBAQQEAAIBAQQEAQECAwP/AgEEBAQAAgEEBAQBBAMFBgQDAwEBAQQCAwEBAQEBAwUJ//8DAgMBAAADAgQBAAo8BAH//wADAgEAAAADAgEAA/8DAQQEBAADAQQEBAEEAwUG//8AAQEBAAAAAQEBAQEDBQn//wACAwEAAAACAwEACj8EAQABAwMCAQABAwQCAQEBAQUB//8CAwIBAAACBAMBAAMAAgEBBAQAAgEBBAQBAQIDAwACAQQEBAACAQQEBAEEAwUKVAQB//8BAwIBAAABAwIBAQECBQb/AQMBAQEAAQMBAQEBAQEFBv8BAwEBBAABAwEBBAEBAQQG/wEDAQH/AAEDAQEAAQEBBApABAEBAQMDAgEBAQMEAgEBAQAFAf//AwMCAQAAAwQCAQAD//8BBAQEAAABBAQEAQQDBQb/AwMBAQEAAwQBAQEBAQMFCkEEAf//BAMCAQAABAMCAQAE//8BAwMDAAABAwMDAQMDBQb/BAMBAQEABAMBAQEBAQMFCf8BAwIDAQABAwIEAQEBAQUKQgQDAQIBAQQEAQIBAQQEAQEAAwMBAgEEBAQBAgEEBAQBAQACBf//AQICAgAAAQICAgECAwUDAQIBAQT/AQIBAQQAAQEAAwozBAH/AAADAwEAAAADBAEAAf8AAwMDAQAAAgMEAQAD/wABAQEEAAABAQEEAQECBAMDAwEBAQQCAwEBAQQBAQIEClEEAwIDAQEBBAIDAQEBBAEBAgQDAgMBAQQEAgMBAQQEAQECAwb//wECAQEAAAECAQEBAQIFCv8CAwECAQACBAEDAQEBAwUKNQQB/wIAAwMBAAIAAwQBAAb//wQCAQEAAAQCAQEBAQQFCf//AQIDAgAAAQIEAwAB/wIDAwP/AAECAgIAAQMCBAo2BAH/AwADAwEAAgADBAEAA/8BAQEBBAABAQEBBAEBAQQIAQEBAwQDAQEBAgQDAQEAAgr//wEBAgEAAAEBAgEBAQIFCjcEAf8EAAMDAQAEAAIDAQAD/wIBAQEEAAIBAQEEAQECBAr//wIBAgEAAAIBAwEBAQMFAf8EAwMD/wACAQEBAAEDAgQKOwQB//8AAwMBAAAAAwQBAAP//wABBAQAAAABBAQBBAQFA/8DAQEBBAADAQEBBAEBAgQD/wMBAQQEAAMBAQQEAQECAwouBAEAAQMDAwEAAQIDBAEBAQEFAf//AgMDAQAAAgMEAQADAAMBAQEEAAMBAQEEAQECBAMAAwEBBAQAAwEBBAQBAQIDClAEAf//AQMDAQAAAQMEAQEBAgUG/wEDAgEBAAEDAgEBAQEBBQr//wQBAgEAAAQBAgEBAQMFA/8EAQEB/wAEAQEBAAEBAgQKLwQBAQEDAwMBAQECAwQBAQEABQH//wMDAwEAAAIDBAEAA///AQEBBAAAAQEBBAEBAgQG/wMDAgEBAAMEAgEBAQEEBQowBAH//wQDAwEAAAQCAwEABP//AQQDAwAAAQQCAwAG/wQDAgEBAAQDAgEBAQEEBQECAQMDA/8CAQMDAwABAwIECjEEAwEDAQEBBAEDAQEBBAEBAAQDAQMBAQQEAQMBAQQEAQEAAwX//wEDAgIAAAEEAgMACv8BAwECAQABAwECAQEBAQULAAQBAgIEBAQCAQECAwQBAQIABQT//wEBAQQAAAEBAQQBAQIEBwEDAwIBAQEDBAIBAQEBAAUJ/wEBAwQDAAEBAgQDAQEBAgsBBAECAgQEAwIBAQMEAgEBAgAFBwEDAwEBAQEDBAEBAQEBAAUJ//8BAwQCAAABAwQCAAr//wMCAwEAAAMCBAEACwIEAf8CAP8AAQADAAAAAgAB/wIDBAP/AAECBAMAAAUDAf8DAv8DAQAEAgAACf//AQL/AgAAAQIAAwALAwQB/wIDAQMBAAIDAQQBAQEDBQH//wMEAwQAAAEDAgQABgL/AQIB/wMAAQQCAAAHAQIDAQMBAQIDAQQBAQEABQsEBAH/AgQEAAAAAQMEAAAAAv8BAQMEAQABAQMEAQEBAQUC/wEDAwQBAAECAwQBAQEBBQcBAQMDAQEBAQMEAQEBAQAFCwUEAQICBAQCAgEBAwQBAQECAAUGAv//AQICAgAAAQMEAAkBAQEDBAEBAQEDBAEBAQAFC/8EAQECBAADAQECBAEBAgMLBgQCAQEDAwQBAQECAwQBAQEABQT//wEBAgQAAAEBAgQBAQIDBwEDAwMBAQECAwQBAQEBAAUJ/wEBAwQEAAEBAwQEAgEBAgQEBQsHAwH/AgIEAgIAAQEDAQEBAgEFBwEBAwMBAwEBAgMBBAEBAAQEBAQBA///AwQBAgAAAAsIBAH/AgICAAAAAQIDAAAAAgEBAwEEAQEBAwEEAQEBAAUE//8BAQICAAABAQICAgECAwIEBQcBAwEDAQEBAwEEAQEBAQAFCwoEAf8CAwQE/wABAgMEAAAG//8EAwECAAAEAwECAAcBAgMCAP8BAgQDAAAACf//AQIAAwAAAQIAAwALCwQB/wIBAAD/AAIBAAAAAAP//wMCAgEAAAQCAwEABwH/AwIC/wEABAIDAAAM/wMCAQEAAAMCAQEAAQEDBAsMAwcBA/////8BAwAAAAAAAf8CBP///wABAwAAAAAHAQMD////AQMEAAAAAAsNBAH/AgEBAP8AAwECAAAAAf8CBAQEBAABAwMDAwEEAgUHAf8DAgP/AQADAgQAAAn//wEDAQMAAAEDAQQBAQIECw4EAf8CAQECAgACAQEDBAEBAgMGAgEBAQICAgEBAQMEAQEBAwj/AgIBAgIAAgIBAwQBAgEFC/8EAQMCAQAEAQMCAQEBAgULDwQB/wIBAgACAAIBAwAEAAECAgQCBAIBAQMBBAEBAgAFBP//AQEBAgAAAQEBAgEBAgQHAQMBAgEBAQMBAgEBAQEABQsQBAH/AgECAAEAAwEEAAIAAf8CAwIE/wABAgEDAAAGAv8CAwH/AgADBAEAAAn//wECAgMAAAECAwQACxEEAf8CAQIAAwACAQMABAAC/wEEAQMCAAACAQEDAQEBAwcB/wECAv8BAAIDBAAACf//AQQCAwAAAQQCAwALEgQB/wIBAgICAAIBAwMEAQIBBQQEAQEDAQIEAQEDAQIBAQEEBwEDAQIBAwEDAQIBBAEBAAQI//8CAQMCAAACAQQDAAsTBAH/AgECAgEAAgEDBAEBAQIFBgIBAgEBAgIBAwEBBAEBAQQHAQIBAv8DAQIBAwAEAQEAAg3/AgECAgEAAgEDBAEBAQIFCxQEAf8CAQICAwACAQMDBAECAQQEAgECAQECAgEDAQEEAQEBBAYCAQIBA/8CAQMBBAABAQEDCwECAQICAQECAQMEAQEBAAULFQQB/wIBAgECAAIBAwEEAQECBAUDAgMB//8DAgQBAAAABwH/AQIBAgEAAQIBAwEBAAQI//8CAQMBAAACAQMBAQEDBQsWBAH/AgECA/8AAgEDBAAABQMCAAMAAQMCAAQAAQAHAQMBAgQEAQMBAgQEAgEAAgQEBQj//wIBAwMAAAIBAwQACxcEAf8CAQIAAAACAQMAAAABAgICAgQCAQEBAQMBAQIABQcBAQECAQEBAQECAQEBAQAFCQEBAQECAwEBAQECAwEBAAMLGAQB/wIBAgIBAAIBAwQBAQECBQH/AgMCBAIAAQIBAwEBAgEFBgL/AgMB/wIAAwQBAAAJ//8BAgIDAAABAgMEAAsZBAH/AgECBAQAAgEDBAQBBAQFAQICAgIEBAEBAQEDBAECAAMEBAEDAQECBAEDAQECAQEBBAcBAQECAwMBAQECAwQBAQACCxoEAQICBAMEAgEBAwIEAQECAAUE//8BAQEDAAABAQEEAQECBAcBAwICAQEBBAIDAQEBAQAFCf8BAQMDAwABAQMDAwIBAQIDAwULGwQB/wIDAwT/AAECAwQAAAYCAQMDAQECAQMEAQEBAQEFBwECAgIA/wECAwQAAAAJ//8BAgMDAAABAgMEAAscBAH/AgEDAAMAAgEDAAQAAv8BBAID/wABBAIDAAAGAgEDAAP/AgEDAAMAAAsBBAMCAQEBBAMCAQEBAQAFCx0EAf8CBAMCAgABAwIBAQECAQUG//8EAQIBAAAEAQIBAQEDBQn//wEDAwEAAAEDBAEBAQIFAf8CBAMC/wABAwIBAAECAQQLHgQBAgIBAwL/AgIBBAMAAQIABAT//wEDAQMAAAEDAQQBAQIEBgIBAwEC/wIBBAEDAAEBAQMI//8CAQQCAAACAQQDAAsfBAH/AgEDAAAAAgEDAAAAAQICAgMEAgEBAQIDAQECAAUHAQECAgEDAQECAgEDAQEABAn//wEBAwMAAAEBAwQBAQIDCyAEAf8CAgMEBAABAQIDBAECAQIGAgEBAQIBAgEBAQMBAQEBBQcBAQICAwEBAQIDBAEBAQAFC/8EAwMBAQAEAgMBAQEBBAULIQQBAgIAAQACAgMAAQAEAAH//wQEAwQAAAIDAQQABgL/AQIC/wIAAQMEAAAHAQMDAQMBAQIDAQQBAQEABQsjBQcB/wEBAf8CAAMDAwAAAQICBAIDAgEBAwECAQECAAUD//8CAgEDAAACAwEEAAcBAwEBAQEBAwEBAQEBAQAFCf8BAQMCAgABAQQCAwEBAQILJAQB/wIDAgP/AAEDAgQAAAYC/wICAf8CAAMEAQAABwECAwEEAQECAwEEAQEBAAUJ//8BAgICAAABAgICAQIDBQslBAH/AgACAgIAAQACAwQAA/8CAgIBAwACAwMBBAECAQMHAQMBAQEDAQMBAQEEAQEABAoBAwICAwEBAwICBAECAQAFAgIDCyIEAf8CAAECAgACAAEDBAAF/wECAgMDAAECAgMDAgICAwMEBQcBAwMBAwMBAgIBAwQCAQADAwEFCQL/AQMBAQIAAQMBAQEBAgULJgQB/wIAAgIAAAEAAgMAAAUDAQMCAQEDAQQCAQEBAQEFBwEBAQEBAwEBAQEBBAEBAAQJ//8BAQICAAABAQIDAgECAwIEBQsnBAH/AgADAAIAAQADAAIAAQICBAMDAgEBBAIDAQECAAUHAQMCAQEBAQMCAQEBAQEABQn/AQEDAwIAAQEDBAIBAQECCygEAf8CAwMD/wABAgMEAAAGAgP/AgEBAgQAAwEBAQEEBQcBAgIB/wEBAgMBAAEBAQAFCf//AQIDAgAAAQIEAwALKQQB/wIAAwICAAEABAIDAAUDAQQCAP8DAQQCAAAABwEDAgEBAwEDAgEBBAEBAAQM/wMBBAP/AAMBBAMAAAsqBAH/AgADAgAAAQADAgAAAQICAgMDAgEBAQIDAQECAAUHAQECAQEDAQECAQEEAQEABAn/AQEBAwIAAQEBAwIBAQEDCysEAf8CAf8CAgACAQADBAAGAgH/AQICAwEAAgQEAQIABQf//wMCAQMAAAMCAQQAC/8EAwEEAQADAgEEAQEBAwULLQQB/wIBBAAAAAIBBAAAAAv/BAMBAgIABAMBAgIBAgQFBAQDAQEC/wQDAQECAAEBAgMGAgEEBP//AgEEBAAAAQQCAwssBAL/BAMDAf8ABAIDAQAABQMBAAID/wMBAAIEAAAH//8DAQEDAAADAQEEAQEDBAv/BAIBBP8AAwIBBAAAC08EAgQBAwEBAQQBAwEBAQEBAQUCBAEDAwEBBAECAwEBAQEBBQn/BAEDAgEABAEDAgEBAQIFAgQBAwEB/wQBAwEBAAEBAQQLSAIJAgEBAwIBAgEBBAMBAQEBBQkC/wEDAgECAAEEAwEBAQIFC1IEAf8ABAQDAgAAAwQCAQAB/wAABAACAAAAAwABAAH/AAAEAwIAAAADAgEABP8AAQQEBAAAAQQEBAEEAwULVQQBBP8EBAP/AgADBAEAAAQBAgEBBAQBAgEBBAQBAQADBAECAQQEBAECAQQEBAEBAAIG//8BAgICAAABAgICAQIDBQtTBAH/AQAEAAIAAQAEAAIABAMCAQEEBAMCAQEEBAEBAgMEAwIBBAQEAwIBBAQEAQQDBQf//wIBAQEAAAIBAQEBAQMFCzkEAf8DAAQAAgACAAMAAQAB/wMABAMCAAIABAMBAAcCAwMBAQECAwQBAQEBAQMFB///BAEBAQAABAEBAQEBAwULOgQB/wQABAACAAMABAABAAH/BAAEAwIAAwAEAgEABP8BAQQEBAABAQQEBAEBAQIHAwMDAQEBAgMEAQEBAQEDBQs8BAH//wAEAAIAAAADAAEAAf//AAQDAgAAAAMCAQAE//8AAQQEAAAAAQQEAQQEBQT/AgEBBAQAAgEBBAQBAQIDCz8EAQACAAQAAgABAAQAAgABAAIEBAMCAAEDBAIBAQIBBQH//wIEAwIAAAEDAgEBAgIFBAACAQEEBAACAQEEBAEBAgMLVAQB//8BBAMCAAABBAMCAAT/AwEEBAQAAwEEBAQBBAMFCv//BAIDAQAABAIDAQALAQQCAf//AQQCAQAAAQEAAwtABAH//wMEAwIAAAIEAwEAB/8CAwEBAQACAwEBAQEBAwUH/wIDAQEEAAIDAQEEAQEDBAf/AgMBAf8AAgMBAQABAQMEC0EEAQICBAQDAgEBAwQCAQECAAUB//8EBAMCAAADBAIBAAT//wEEBAQAAAEEBAQBBAMFB/8DAwEBAQADBAEBAQEBAwULQgQC//8EAwIBAAAEAwIBAAX//wEDAwMAAAEDAwMBAwMFB/8EAwEBAQAEAwEBAQEBAwUK/wEDAgMBAAEDAgQBAQEBBQszBAH/AAEEAAIAAAEEAAIAAf8ABAQEAgAAAgMEAQAE/wABAQEEAAABAQEEAQECBAQCAwEBAQQCAwEBAQQBAQIEC1EEBAEDAQEBBAEDAQEBBAEBAAQEAQMBAQQEAQMBAQQEAQEAAwb//wEDAgIAAAEEAgMAC/8BAwECAQABAwECAQEBAQULNAQEAwMBAQEEAgMBAQEEAQECBAQDAwEBBAQCAwEBBAQBAQIDB///AgIBAQAAAgMBAQEBBAUL/wMDAQIBAAMEAQIBAQEDBQs2BAH/AwEEAAIAAwEEAAIAB///BAIBAQAABAIBAQEBBAUK//8BAgMCAAABAgQDAAH/AwQEBP8AAQICAgABBAIECzcEBP8BAQEBBAABAQEBBAEBAQQJAQEBAwQDAQEBAgQDAQEAAgv//wEBAgEAAAEBAgEBAQIFAf8EBAQE/wABAQEBAAEEAQQLOwQB//8ABAQCAAAAAwQBAAT/AgEBAQQAAgEBAQQBAQIEB///AAIBAQAAAAIBAQEBBAUL//8AAQIBAAAAAQIBAQEDBQsuBAEAAgQEBAIAAQIDBAEBAgEFAf//AgQEAgAAAQMEAQECAgUEAAMBAQEEAAMBAQEEAQECBAQAAwEBBAQAAwEBBAQBAQIDC1AEAf//AQQAAgAAAQQAAgAE/wMBAQEEAAMBAQEEAQECBAT/AwEBBAQAAwEBBAQBAQIDCwEEAwECAQEEAwECAQEBAAULLwQB//8DBAQCAAACAwQBAAf/AgMCAQEAAgQDAQEBAQQFAQECBAQE/wECBAQEAAEEAgQH/wIDAgH/AAIEAwEAAAswBAECAgQEBAIBAQIDBAEBAgAFAf//BAQEAgAAAgMEAQAE//8BAQEEAAABAQEEAQECBAf/AwMCAQEAAwQCAQEBAQQFCzEEAv//BAMDAQAABAIDAQAF//8BBAMDAAABBAIDAAf/BAMCAQEABAMCAQEBAQQFAQMCBAQE/wIBAwMDAAEEAgQ=",
};


// Ukulele: 552 chords, 2114 voicings, 25637 bytes.
const int kUkuleleTuning[] = {67, 60, 64, 69};

const PCWSTR kUkuleleKeys[] = {
    L"C", L"Db", L"D", L"Eb", L"E", L"F", L"Gb", L"G", L"Ab", L"A", L"Bb", L"B",
};

const PCWSTR kUkuleleSuffixes[] = {
    L"major", L"minor", L"dim", L"dim7", L"sus2", L"sus4",
    L"7sus4", L"alt", L"aug", L"6", L"69", L"7",
    L"7b5", L"aug7", L"9", L"9b5", L"aug9", L"7b9",
    L"7b9#5", L"7#9", L"11", L"9#11", L"13", L"13b9",
    L"13b5b9", L"b13b9", L"b13#9", L"maj7", L"maj7b5", L"maj7#5",
    L"maj9", L"maj11", L"maj13", L"m6", L"m7", L"m7b5",
    L"m9", L"m69", L"m9b5", L"m11", L"mmaj7", L"mmaj7b5",
    L"mmaj9", L"mmaj11", L"add9", L"madd9",
};

const PCWSTR kUkuleleSuffixNames[] = {
    L"", L"m", L"dim", L"dim7", L"sus2", L"sus4", L"7sus4", L"alt",
    L"aug", L"6", L"69", L"7", L"7b5", L"aug7", L"9", L"9b5",
    L"aug9", L"7b9", L"7b9#5", L"7#9", L"11", L"9#11", L"13", L"13b9",
    L"13b5b9", L"b13b9", L"b13#9", L"maj7", L"maj7b5", L"maj7#5", L"maj9", L"maj11",
    L"maj13", L"m6", L"m7", L"m7b5", L"m9", L"m69", L"m9b5", L"m11",
    L"mmaj7", L"mmaj7b5", L"mmaj9", L"mmaj11", L"add9", L"madd9",
};

const char* const kUkuleleChunks[] = {
    "CQAEAQIBAAACAQAAAAIBAwQDAQIEAwAEAwECAQQBAgEBAQEDCQEBAQQBAQEEAQEAAgkBBAECAAAAAgAAAAABAgAAAwIAAAMAAgEDBAIBAwQCAAMDAgMBAwIEAQAJAgQCAQIEAgECBAMAAwMBAwEDAQQBAQEBAwYDBAMBAgQDAQALBAIBAgQCAQMACQMEAQIDAgMBAwIEAAUBAgECAQMCBAAIAQIBAgEDAgQACwECAQIBAwIEAAkEBAIBAwQBAQMEAQEBAAMEAQECBAEBAgQBAQABBwMDAQEDBAEBAQECAwkBAwQEAQIDAwEEAgMJBQQBAgIAAAIDAAAAAgEDBAQBAgMEAAQEAQICBAECAgECAgMEBAECBAMBAgQACQYEAQACAAAAAgAAAAIBAwIEAQMCBAAFAwMBAwIDAQQACQEBAgIBAQICAgEAAQICAwkHAwIBAgQDAQIEAwADBAEDAgQBAwIABgEEBAEBAwQBAQEAAwkIBAECAQEAAwECAAABAgEBBAIBAQQBAQECAgEEBAMBAwQCAAUCAQEEAgEBBAEBAQIJCQQBAgQCBAEDAgQABQICAQMCAwEEAAkBAQEBAQEBAQEBAAMLAQMCAgEEAgMACQoEAQQEAgQCAwEEAAYBAQICAQECAgIBAAECAgMJAQMBAQEDAQEBAQADCwEDAgQBAwIEAAkLBAEAAQAAAAEAAAABAgQDBAEDAgQABQIDAQMCAwEEAAkBAQECAQEBAgEBAAIJDAQBAgMDBAECAwQABQIDAQICBAEDAAgBAgIDAQIDBAALAgMBAgIEAQMACQ0EAQABAQAAAQIAAAIBBAIDAQQCAwAFAgMBBAIDAQQACQIBAQICAQEDAQEBAgkOBAEAAQACAAEAAgABBAQDBAIDAQQABgECAgIBAgMEAAkBAwECAQMBAgEBAAIJDwQBBAMDBAIBAQMBAwECBgECAgEBAgIBAgEAAwIBAggBBAIDAQQCAwALAgMBBAIDAQQACRAEAQABAQIAAQECAQEBAgMCAwECAgQBAwAGAQICAwECAgMBAgECCQIDAQICBAEDAAkRBAEAAQABAAEAAgABAwQDBAEDAgQABgECAQIBAwIEAAkBAgECAQMCBAAJEgQBAAEBAQABAgMAAwEDAQIBAwECAQEAAgYBAgEDAQIBAwEBAAIJAgIBAgIDAQQACRMEAQABAAMAAQADAAMDAgECBAIBAwADBAIBAQQCAQEBAQIDBgECAwIBAgQDAAkUAwEEAgMEAwECBAAEBAQEAQIDBAEABQIDAwECAwQBAAkVBAEEAwMEAgEBAwEDAQIGAQICAQECAgECAQADAgECCAEEAgMBBAIDAAsCAwEEAgMBBAAJFgQBAAECAgABAgMAAwIEAQICBAEDAAYBAgIEAQIDBAAJAwMBAgMEAQIACRcEAQABAgEAAQMCAAMBBAECAQQBAgEBAAIGAQIBBAECAQQBAQACCQQCAQEEAgEBAQECAwkYBAMBBAEEAQMBBAEBAAIGAwIBBAMCAQQACQACAwEAAgMBAAoCAQIBAwEEAgAJGQQBAAEBAQABAgMAAwEDAQIBAwECAQEAAgYBAgEDAQIBAwEBAAIJAgIBAgIDAQQACRoEAQABAQMAAQIDAAMDAwECAwQBAgADBAMBAQQDAQEBAQIDBgECAwMBAgMEAAkbBAEBAQAAAQIAAAABAgQEBAECAwQABQIEAQMCBAEDAAkBAQEDAQEBAwEBAAIJHAQBAgMEBAECAwQABQIEAQICBAEDAAgBAgIEAQIDBAALBAMBAQQDAQEBAQIDCR0EAQEBAQABAgMAAAIBBAMDAQQCAwAFAgQBBAIDAQQACQIBAQMCAQEDAQEBAgkeBAEBAQACAQIAAwABBAQEBAEBAQEBBAADBgEDAgIBAwICAQICAwkBAwEDAQMCBAAJHwIBBAIEBAIBAwQABQIEAwECBAMBAAkgBAEBAQICAQECAgIBAAECAgMEAQMBAQEDAQEBAQADBgEDAgQBAwIEAAkDAwEDAgMBBAAJIQQBAgQCAwEDAQIBAgACBQECAQMBAgEDAQEAAggCAgECAgMBBAALAQICAgECAwQACSIEAQAAAAAAAAAAAAECBAMDAQMCAgEDAgMFAQMBAwEDAgQACAICAQMCAgEDAQIAAQkjBAECAwMDAQIDBAAFAQMBAgEDAQIBAQACCAECAQMBAgEDAQEAAgsCAgECAgMBBAAJJAQBAAAAAgAAAAIAAgQDAgEEAwIBAAEEBAMDAgIBAQIDAgMEAAEFAQMDAwECAwQACSUEAQQEAgMDBAECAAIEAwEBBAMBAQEBAgMFAQIDAwECAwQACAIEAQICBAEDAAkmBAIEAgIBBAICAQECAQIBBAMDAwIBAQEBAwEDBQEDAwIBAwQCAAgBBAEDAQQBAwEBAAIJJwMBBAIDAwMBAgIBAwIDAgQBAgEEAQIBAQEBAwUBAwMBAQMEAQEBAAMJKAQBAQAAAAEAAAAAAQIEBAMBAwQCAAUBBAEDAQQBAwEBAAIIAgIBBAICAQQBAgABCSkEAQIDBAMBAgQDAAUBBAECAQQBAgEBAAIIAQIBBAECAQQBAQACCwMCAQIEAgEDAAkqBAEBAAACAQAAAgACBAMDAQQCAwEAAQQEBAMCAwQBAAUBBAMDAQQCAwAJKwMBBAIEAwMBBAIAAgQBAwEEAQMBAQEBAwUBBAMBAQQDAQEBAAMJLAQBAgEAAgIBAAMABAEBAgEBAQIBAQEAAwYBBAICAQQCAgECAgMJAQMBBAEDAQQBAQACCS0DAQIAAAICAAADAAIEAwQBAwIEAQADAgIDAQICAwEBAgABCAAEAQEDBAMBAgQDAAMDAQIBAwECAQEBAQMDAwECBAMBAgQACAEBAQQBAQEEAQEAAggBBAEBAwQCAQMEAgABBAMEAgMCBAEAAwIBAgQCAQMEAAYDAwIBAwQCAQAIAgQBAQIEAgECBAMAAQQCBAIDAQQBAQIBAwUDBAMBAgQDAQAKBAIBAgQCAQMACAMEAQECAQIBAwIEAAQBAgECAQMCBAAHAQIBAgEDAgQACgECAQIBAwIEAAgEBAEBAwQBAQMEAQEBAAMDAQECBAEBAgQBAQABBgMDAQEDBAEBAQECAwgBAwQEAQIDAwEEAgMIBQQBAQMEBAECAwQAAwQBAgIEAQICAQICAwMEAQIEAwECBAAGAQMEAQEDBAEBAQADCAYEAQEDAgQBAwIEAAQDAwEDAgMBBAAIAQECAgEBAgICAQABAgIDCwEDAQEBAwEBAQEAAwgHAgIEAQMCBAEDAgAFAQQEAQEDBAEBAQADCAgEAQEAAAMBAAADAAEBBAQDAQMEAgADAwICAQMCAgEBAgECBAIBAQQCAQEEAQEBAggJBAEBAwEDAQMCBAAEAgIBAwIDAQQACAEBAQEBAQEBAQEAAwoBAwICAQQCAwAICgQBAwMBAwIDAQQABQEBAgIBAQICAgEAAQICAwgBAwEBAQMBAQEBAAMKAQMCBAEDAgQACAsEAQEDAgMBAwIEAAQCAwEDAgMBBAAIAQEBAgEBAQIBAQACCwECAQEBAgEBAQEAAwgMBAEBAgIDAQIDBAAEAgMBAgIEAQMABwECAgMBAgMEAAoCAwECAgQBAwAIDQQBAQQCAwEEAgMABAIDAQQCAwEEAAgCAQECAgEBAwEBAQILAQICAQECAgECAQADAgECCA4EAQMDAgMCAwEEAAUBAgICAQIDBAAIAQMBAgEDAQIBAQACCwECAQMBAgEDAQEAAggPBAEDAgIDAgEBAwECAQIFAQICAQECAgECAQADAgECBwEEAgMBBAIDAAoCAwEEAgMBBAAIEAQBAwQCAwIEAQMABQECAgMBAgIDAQIBAggCAwECAgQBAwALAQICAwECAwQACBEEAQIDAgMBAwIEAAUBAgECAQMCBAAIAQIBAgEDAgQACwECAQIBAwIEAAgSBAECBAIDAQMBAgECAAIFAQIBAwECAQMBAQACCAICAQICAwEEAAsBAgICAQIDBAAIEwQBBAMCAwQCAQMAAgQCAQEEAgEBAQECAwUBAgMCAQIEAwAIAQQBAgEEAQIBAQACCBQDAQMBAgMDAQIEAAMEBAQBAgMEAQAEAgMDAQIDBAEACBUEAQMCAgMCAQEDAQIBAgUBAgIBAQICAQIBAAMCAQIHAQQCAwEEAgMACgIDAQQCAwEEAAgWBAICBAECAgQBAwAFAQICBAECAwQACAMDAQIDBAECAAgEAwEBBAMBAQEBAgMIFwQCAQQBAgEEAQIBAQACBQECAQQBAgEEAQEAAggEAgEBBAIBAQEBAgMLAQIDAgECBAMACBgEAgEEAQQBAwEEAQEAAgUDAgEEAwIBBAAIBAIDAQQCAwEACQIBAgEDAQQCAAgZBAECBAIDAQMBAgECAAIFAQIBAwECAQMBAQACCAICAQICAwEEAAsBAgICAQIDBAAIGgQCBAMBAQQDAQEBAQIDAQQEAgMDBAECAAUBAgMDAQIDBAAIAgQBAgIEAQMACBsEAQEDAwMBAgMEAAQCBAEDAgQBAwAIAQEBAwEBAQMBAQACCgQDAgEEAwIBAAgcBAEBAgMDAQIDBAAEAgQBAgIEAQMABwECAgQBAgMEAAoEAwEBBAMBAQEBAgMIHQQBAQQDAwEEAgMABAIEAQQCAwEEAAgCAQEDAgEBAwEBAQIKBAMDAQQCAwEACB4EAQMDAwMBAQEBAQMAAwUBAwICAQMCAgECAgMIAQMBAwEDAgQACwICAQMCAgEDAQIAAQgfAgEDAQMDAgEDBAAEAgQDAQIBAwQACCAEAQAAAQEAAAECAAMBAwEBAQMBAQEBAAMFAQMCBAEDAgQACAMDAQMCAwEEAAghBAEBAwECAQMBAgEBAAIEAQIBAwECAQMBAQACBwICAQICAwEEAAoBAgICAQIDBAAIIgQBAQMCAgEDAgIBAgIDBAEDAQMBAwIEAAcCAgEDAgIBAwECAAELAQEBAQEBAQEBAQADCCMEAQECAgIBAgMEAAQBAwECAQMBAgEBAAIHAQIBAwECAQMBAQACCgICAQICAwEEAAgkBAEEAwIBBAMCAQABAwMCAgICAQECAgIDAwABBAEDAwMBAgMEAAcCBAEDAgQBAwAIJQQBAwMBAgMEAQIAAQQDAQEEAwEBAQECAwQBAgMDAQIDBAAHAgQBAgIEAQMACCYEAQQCAgEEAgIBAQIBAgEDAgICAgEBAQECAQMEAQMDAgEDBAIABwEEAQMBBAEDAQEAAggnAwEDAQICAwECAgECAgMBBAECAQQBAgEBAQEDBAEDAwEBAwQBAQEAAwgoBAEBAwMCAQMEAgAEAQQBAwEEAQMBAQACBwICAQQCAgEEAQIAAQoEAgIBBAICAQECAQIIKQQBAQIDAgECBAMABAEEAQIBBAECAQEAAgcBAgEEAQIBBAEBAAIKAwIBAgQCAQMACCoEAQQDAwEEAgMBAAEDAwMCAgMEAQAEAQQDAwEEAgMABwIEAQQCAwEEAAgrAwEDAQMCAwEEAgABBAEDAQQBAwEBAQEDBAEEAwEBBAMBAQEAAwgsBAEDAwQDAQECAQEDAAMFAQQCAgEEAgIBAgIDCAEDAQQBAwEEAQEAAgsDAgEDAwIBBAAILQMBBAMEAQMCBAEAAQMDBAICAgMBAQMAAQsDAQEDAwEBBAEBAQILAAQBBAMCAgMCAQEBAgIDBAEDBAMBAgQDAAYDAQIBAwECAQEBAQMLAQEBBAEBAQQBAQACCwEEAQQCAgIDAQEBAQIBAwIDAQEEAwEBBAEBAQIEAQMEAgEDBAIABQMCAwEDAgQBAAsCBAEEAgECBAIBAwAEAQIEAgECBAMABQMBAwEDAQQBAQEBAwgDBAMBAgQDAQALAwQBAQIBAgEDAgQABAECAQIBAwIEAAcBAgECAQMCBAAKAQIBAgEDAgQACwQEBAEDBAEBAwQBAQEAAwYBAQIEAQECBAEBAAEJAwMBAQMEAQEBAQIDCwEDBAQBAgMDAQQCAwsFBAEEBAICAwQBAQECAgMEAQMEBAECAwQABgQBAgIEAQICAQICAwYEAQIEAwECBAALBgQBAgQCAgEDAQEBAgADBAEDAgQBAwIEAAcDAwEDAgMBBAALAQECAgEBAgICAQABAgIDCwcDAQQDAQIEAwECAAQBAgQDAQIEAwAFBAEDAgQBAwIACwgEAQADAwIAAgMBAAEEAwMCAwICAQEDAQIDAgEBBAIBAQQBAQECBAEEBAMBAwQCAAsJBAEBAwICAQQCAwAEAQMBAwEDAgQABwICAQMCAwEEAAsBAQEBAQEBAQEBAAMLCgQBAQMCBAEDAgQABAMDAQMCAwEEAAgBAQICAQECAgIBAAECAgMLAQMBAQEDAQEBAQADCwsEAQIDAgIBAgEBAQIAAwQBAwIDAQMCBAAHAgMBAwIDAQQACwEBAQIBAQECAQEAAgsMBAECAwECAgQBAwAEAQICAwECAwQABwIDAQICBAEDAAoBAgIDAQIDBAALDQQBAgMDAgECAgECAgADAwECBAEEAgMBBAIDAAcCAwEEAgMBBAALAgEBAgIBAQMBAQECCw4EAQIDAgQBAgEDAQIAAgUCAgECAgMBBAAIAQICAgECAwQACwEDAQIBAwECAQEAAgsPBAECAwEEAgMBBAAFAgEBAgIBAQMBAQECCAECAgEBAgIBAgEAAwIBAgoBBAIDAQQCAwALEAQBAgMDBAECAgMBAwECBQIDAQICBAEDAAgBAgIDAQICAwECAQILAgMBAgIEAQMACxEEAQIDAgMBAwIEAAUBAgECAQMCBAAIAQIBAgEDAgQACwECAQIBAwIEAAsSBAECAwMDAQIDBAAFAQMBAgEDAQIBAQACCAECAQMBAgEDAQEAAgsCAgECAgMBBAALEwQCAQIBBAECAQQBAQACBQMCAQIEAgEDAAUEAgEBBAIBAQEBAgMIAQIDAgECBAMACxQEAQIDAAQBAgADAAQDAQIDAwECBAAGBAQEAQIDBAEABwIDAwECAwQBAAsVBAECAwEEAgMBBAAFAgEBAgIBAQMBAQECCAECAgEBAgIBAgEAAwIBAgoBBAIDAQQCAwALFgQBAgMEBAECAwQABQIEAQICBAEDAAgBAgIEAQIDBAALAwMBAgMEAQIACxcEAQIDBAMBAgQDAAUBBAECAQQBAgEBAAIIAQIBBAECAQQBAQACCwQCAQEEAgEBAQECAwsYBAEBAAEAAQACAAACAQQDAgEEAwIABQEEAQQBAwEEAQEAAggDAgEEAwIBBAALGQQBAgMDAwECAwQABQEDAQIBAwECAQEAAggBAgEDAQIBAwEBAAILAgIBAgIDAQQACxoEAgECAgQBAgMEAAUEAwEBBAMBAQEBAgMFAwMBAgQDAQIACAECAwMBAgMEAAsbBAEEAwIBBAMCAQABAwMCAgIDAQEBAgIDBAEDAwMBAgMEAAcCBAEDAgQBAwALHAQBBAMBAQQDAQEBAQIDAQMDAQIDBAECAAQBAgMDAQIDBAAHAgQBAgIEAQMACx0EAQQDAwEEAgMBAAEDAwMCAgMEAQAEAQQDAwEEAgMABwIEAQQCAwEEAAseBAEDAwIEAgIBAwEDAAEGAQEBAQEBAQEBAQADCAEDAgIBAwICAQICAwsBAwEDAQMCBAALHwMBAwMABAECAAMABAMBAwMCAQMEAAcCBAMBAgQDAQALIAQBAwMEBAEBAgICAwABBAIDBgEDAQEBAwEBAQEAAwgBAwIEAQMCBAALAwMBAwIDAQQACyEEAQECAgIBAgMEAAQBAwECAQMBAgEBAAIHAQIBAwECAQMBAQACCgICAQICAwEEAAsiBAECAgICAQEBAQECAAMEAQMCAgEDAgIBAgIDBwEDAQMBAwIEAAoCAgEDAgIBAwECAAELIwQBAgIBAgIDAQQABAECAgIBAgMEAAcBAwECAQMBAgEBAAIKAQIBAwECAQMBAQACCyQEAQICAgQBAQEDAQIAAgQEAwIBBAMCAQAFAgIBAQICAQECAQIDAgABBwEDAwMBAgMEAAslBAEBAgIEAQIDBAAEAwMBAgMEAQIABAQDAQEEAwEBAQECAwcBAgMDAQIDBAALJgQBAgIBBAICAQQBAgABBAQCAgEEAgIBAQIBAgUCAQEBAgEBAQEBAQMHAQMDAgEDBAIACycDBAMBAgIDAQICAQICAwQEAQIBBAECAQEBAQMHAQMDAQEDBAEBAQADCygEAQQCAgEEAgIBAQIBAgEDAgICAgEBAQECAQMEAQMDAgEDBAIABwEEAQMBBAEDAQEAAgspBAEDAgECBAIBAwABBAIBAQQCAQEBAQIDBAECAwIBAgQDAAcBBAECAQQBAgEBAAILKgQBAwICBAIBAQMBAgECBAQDAwEEAgMBAAUCAgIBAgMEAQAHAQQDAwEEAgMACysDBAMBAwIDAQQCAAQEAQMBBAEDAQEBAQMHAQQDAQEEAwEBAQADCywEAQQDAgQDAgEEAAYBAQIBAQECAQEBAAMIAQQCAgEEAgIBAgIDCwEDAQQBAwEEAQEAAgstAwECAAACAgAAAwACBAMEAQMCBAEAAwICAwECAgMBAQIAAQoABAEDAgEBAwIBAQEBAgMDAQMEAwECBAMABQMBAgEDAQIBAQEBAwoBAQEEAQEBBAEBAAIKAQQBAwEBAQMBAQEBAQEDAQMBAQQDAQEEAQEBAgMBAwQCAQMEAgAEAwIDAQMCBAEACgIEAQMBAAEDAQACAAMBAgQCAQIEAwAEAwEDAQMBBAEBAQEDBwMEAwECBAMBAAoDBAEAAQABAAEAAgABAwQDBAEDAgQABgECAQIBAwIEAAkBAgECAQMCBAAKBAQDAQMEAQEDBAEBAQADBQEBAgQBAQIEAQEAAQgDAwEBAwQBAQEBAgMKAQMEBAECAwMBBAIDCgUEAQMDAQEDBAEBAQECAwMBAwQEAQIDBAAFBAECAgQBAgIBAgIDBQQBAgQDAQIEAAoGBAEBAwEBAQMBAQEBAAMDAQMCBAEDAgQABgMDAQMCAwEEAAoBAQICAQECAgIBAAECAgMKBwQBAwIAAQMCAAEAAwECBAMBAgQDAAQEAQMCBAEDAgAHAQQEAQEDBAEBAQADCggEAQMCAgEDAgIBAQIBAgICAQEEAgEBBAEBAQIDAQQEAwEDBAIABgIBAQQCAQEEAQEBAgoJBAEAAgEBAAIBAQEBAgMDAQMBAwEDAgQABgICAQMCAwEEAAoBAQEBAQEBAQEBAAMKCgQBAAIBAwACAQMAAwMDAQMCAwEEAAcBAQICAQECAgIBAAECAgMKAQMBAQEDAQEBAQADCgsEAQECAQEBAgEBAQEAAwMBAwIDAQMCBAAGAgMBAwIDAQQACgEBAQIBAQECAQEAAgoMBAEBAgABAQMAAgADAQICAwECAwQABgIDAQICBAEDAAkBAgIDAQIDBAAKDQQBAQICAQECAgECAQADAgECAwEEAgMBBAIDAAYCAwEEAgMBBAAKAgEBAgIBAQMBAQECCg4EAQECAQMBAgEDAQEAAgQCAgECAgMBBAAHAQICAgECAwQACgEDAQIBAwECAQEAAgoPBAEBAgADAQIAAwAEAgEBAgIBAQMBAQECBwECAgEBAgIBAgEAAwIBAgkBBAIDAQQCAwAKEAQBAQICAwECAgMBAgECBAIDAQICBAEDAAcBAgIDAQICAwECAQIKAgMBAgIEAQMAChEEAQECAQIBAwIEAAQBAgECAQMCBAAHAQIBAgEDAgQACgECAQIBAwIEAAoSBAEBAgICAQIDBAAEAQMBAgEDAQIBAQACBwECAQMBAgEDAQEAAgoCAgECAgMBBAAKEwQBAQIBBAECAQQBAQACBAMCAQIEAgEDAAQEAgEBBAIBAQEBAgMHAQIDAgECBAMAChQDAwMBAgMDAQIEAAUEBAQBAgMEAQAGAgMDAQIDBAEAChUEAQECAAMBAgADAAQCAQECAgEBAwEBAQIHAQICAQECAgECAQADAgECCQEEAgMBBAIDAAoWBAEBAgMDAQIDBAAEAgQBAgIEAQMABwECAgQBAgMEAAoDAwECAwQBAgAKFwQBAQIDAgECBAMABAEEAQIBBAECAQEAAgcBAgEEAQIBBAEBAAIKBAIBAQQCAQEBAQIDChgEAQEEAwIBBAMCAAQBBAEEAQMBBAEBAAIHAwIBBAMCAQQACgQCAwEEAgMBAAoZBAEBAgICAQIDBAAEAQMBAgEDAQIBAQACBwECAQMBAgEDAQEAAgoCAgECAgMBBAAKGgQBAQICBAECAwQABAQDAQEEAwEBAQECAwQDAwECAwQBAgAHAQIDAwECAwQAChsEAQMCAQADAgEAAAECAgEBAgMBAQEBAgMDAQMDAwECAwQABgIEAQMCBAEDAAocBAEDAgAAAwIAAAABAgIAAQIDAAEAAwECAwMBAgMEAAYCBAECAgQBAwAKHQQBAwICAAMBAgAAAQICAgECAwQBAAMBBAMDAQQCAwAGAgQBBAIDAQQACh4EAQICAQMCAgEDAQIAAQUBAQEBAQEBAQEBAAMHAQMCAgEDAgIBAgIDCgEDAQMBAwIEAAofAgMDAQMDAgEDBAAGAgQDAQIEAwEACiAEAQICAwMBAQICAgIAAQMCAwUBAwEBAQMBAQEBAAMHAQMCBAEDAgQACgMDAQMCAwEEAAohBAEAAQEBAAECAwADAQMBAgEDAQIBAQACBgECAQMBAgEDAQEAAgkCAgECAgMBBAAKIgQBAQEBAQEBAQEBAQADAwEDAgIBAwICAQICAwYBAwEDAQMCBAAJAgIBAwICAQMBAgABCiMEAQEBAAEBAgADAAEDBAQEAQIDBAAGAQMBAgEDAQIBAQACCQECAQMBAgEDAQEAAgokBAEBAQEDAQEBAwEBAAIEAgIBAQICAQECAQIDAgABAwQDAgEEAwIBAAYBAwMDAQIDBAAKJQQBAAEBAwACAQQAAwMDAQIDBAECAAMEAwEBBAMBAQEBAgMGAQIDAwECAwQACiYEAQEBAAMBAgAEAAQCAQEBAgEBAQEBAQMDBAICAQQCAgEBAgECBgEDAwIBAwQCAAonAwMDAQICAwECAgECAgMDBAECAQQBAgEBAQEDBgEDAwEBAwQBAQEAAwooBAEDAQEAAwECAAABAgEBAQIBAQEBAQEDAwEDAwIBAwQCAAYBBAEDAQQBAwEBAAIKKQQBAgEAAQMBAAIAAQMBAAADAQAAAAMBAgMCAQIEAwAGAQQBAgEEAQIBAQACCioEAQIBAQMCAQEDAQEBAgQCAgIBAgMEAQADBAMDAQQCAwEABgEEAwMBBAIDAAorAwMDAQMCAwEEAgADBAEDAQQBAwEBAQEDBgEEAwEBBAMBAQEAAwosBAEDAgEDAwIBBAAFAQECAQEBAgEBAQADBwEEAgIBBAICAQICAwoBAwEEAQMBBAEBAAIKLQMBAwEBAwMBAQQBAQECAwQDBAEDAgQBAAQCAgMBAgIDAQECAAEAAAQBAAAAAwAAAAMAAQAEAwMAAgEBAQMCAwMDAgEBAwIBAQEBAgMFAQMEAwECBAMAAAEEAQADAwMAAQIDAAMDAQEBAwEBAQEBAQMDAwEBBAMBAQQBAQECBQEDBAIBAwQCAAACBAIEAgECBAIBAwAFAQIEAgECBAMABgMBAwEDAQQBAQEBAwkDBAMBAgQDAQAAAwQBAgMCAwEDAgQABQECAQIBAwIEAAgBAgECAQMCBAALAQIBAgEDAgQAAAQEAQACAwMAAQICAQMCAwUBAwQBAQMEAQEBAAMHAQECBAEBAgQBAQABCgMDAQEDBAEBAQECAwAFBAEAAAEDAAABAwADAwMBAQIDAQEBAQIDBQEDBAQBAgMEAAcEAQIEAwECBAAABgQBAAABAQAAAQIAAwEDAQEBAwEBAQEAAwUBAwIEAQMCBAAIAwMBAwIDAQQAAAcDAgQDAQIEAwECAAUBAgQDAQIEAwAGBAEDAgQBAwIAAAgEAQEAAAMBAAAEAAEBBAQDAQMEAgADAwICAQQCAwEABAIBAQQCAQEEAQEBAgAJBAEAAAAAAAAAAAABAgQDAwEEAgMABQEDAQMBAwIEAAgCAgEDAgIBAwECAAEACgQBAAIAAAACAAAAAgEDAgQBAwIEAAUDAwEDAgMBBAAJAQECAgEBAgICAQABAgIDAAsEAQAAAAEAAAABAAEDBAMDAQIBAQEDAAMFAQMCAwEDAgQACAIDAQMCAwEEAAAMBAEDBAIDAgQBAwAFAQICAwECAgMBAgECCAIDAQICBAEDAAsBAgIDAQIDBAAADQQBAQAAAQEAAAIAAQMEBAMBBAMCAAUBBAIDAQQCAwAIAgMBBAIDAQQAAA4EAQACAAEAAgABAAMBAgEDAQIBAwEBAAIGAgIBAgIDAQQACQECAgIBAgMEAAAPBAICAwEEAgMBBAAGAgEBAgIBAQMBAQECCQECAgEBAgIBAgEAAwIBAgsBBAIDAQQCAwAAEAQBAQIAAQEDAAIAAwECAgMBAgMEAAYCAwECAgQBAwAJAQICAwECAwQAABEEAQABAAEAAQACAAEDBAMEAQMCBAAGAQIBAgEDAgQACQECAQIBAwIEAAASBAEBAQABAQIAAwABAwQEBAECAwQABgEDAQIBAwECAQEAAgkBAgEDAQIBAwEBAAIAEwQBAAMAAQADAAEAAwECAQQBAgEEAQEAAgYDAgECBAIBAwAGBAIBAQQCAQEBAQIDABQBBQMBAgMDAQIEAAAVBAICAwEEAgMBBAAGAgEBAgIBAQMBAQECCQECAgEBAgIBAgEAAwIBAgsBBAIDAQQCAwAAFgQBAgIAAQIDAAEAAQMCAAADAgAAAAMBAgMDAQIDBAAGAgQBAgIEAQMAABcEAQIBAAEDAQACAAEDAQAAAwEAAAADAQIDAgECBAMABgEEAQIBBAECAQEAAgAYAwECAQIBAwEEAgABAwECAAMBAgAAAwEEAwIBBAMCAAAZBAEBAQABAQIAAwABAwQEBAECAwQABgEDAQIBAwECAQEAAgkBAgEDAQIBAwEBAAIAGgQBAQMAAQEDAAIAAwECAgQBAgMEAAYEAwEBBAMBAQEBAgMGAwMBAgMEAQIAABsEAQAAAAIAAAACAAIEAwIBBAMCAQABBAQDAwIDAQEBAwIDBQEDAwMBAgMEAAAcBAIEAwEBBAMBAQEBAgMBBAQCAwMEAQIABQECAwMBAgMEAAgCBAECAgQBAwAAHQQBAQAAAgEAAAIAAgQDAwEEAgMBAAEEBAQDAgMEAQAFAQQDAwEEAgMAAB4EAQACAAIAAQACAAMCAgEDAgIBAwECAAEHAQEBAQEBAQEBAQADCQEDAgIBAwICAQICAwAfAgUDAQMDAgEDBAAIAgQDAQIEAwEAACAEAQICAAIBAgADAAQBAQICAQECAgIBAAECAgMHAQMBAQEDAQEBAQADCQEDAgQBAwIEAAAhBAECAwMDAQIDBAAFAQMBAgEDAQIBAQACCAECAQMBAgEDAQEAAgsCAgECAgMBBAAAIgQBAwMDAwEBAQEBAwADBQEDAgIBAwICAQICAwgBAwEDAQMCBAALAgIBAwICAQMBAgABACMEAQMDAgMCAwEEAAUBAgICAQIDBAAIAQMBAgEDAQIBAQACCwECAQMBAgEDAQEAAgAkBAMBAQEDAQEBAwEBAAIFBAMCAQQDAgEABgICAQECAgEBAgECAwIAAQgBAwMDAQIDBAAAJQQCAQICBAECAwQABQMDAQIDBAECAAUEAwEBBAMBAQEBAgMIAQIDAwECAwQAACYEAgICAQQCAgEEAQIAAQUEAgIBBAICAQECAQIGAgEBAQIBAQEBAQEDCAEDAwIBAwQCAAAnAwUDAQICAwECAgECAgMFBAECAQQBAgEBAQEDCAEDAwEBAwQBAQEAAwAoBAIEAgIBBAICAQECAQIBBAMDAwIBAQEBAwEDBQEDAwIBAwQCAAgBBAEDAQMBAgEBAAIAKQMBBAMCAwQCAQMAAgQCAQEEAgEBAQECAwUBAgMCAQIEAwAAKgQDAgEBAwIBAQMBAQECBQQDAwEEAgMBAAYCAgIBAgMEAQAIAQQDAwEEAgMAACsDBQMBAwIDAQQCAAUEAQMBBAEDAQEBAQMIAQQDAQEEAwEBAQADACwEAQACAAMAAgADAAMDAgEDAwIBBAAHAQECAQEBAgEBAQADCQEEAgIBBAICAQICAwAtAwMDAQEDAwEBBAEBAQIFBAMEAQMCBAEABgICAwECAgMBAQIAAQIABAECAgIAAQIDAAACAQEBBAEBAQQBAQACBQMCAQEDAgEBAQECAwcBAwQDAQIEAwACAQQBAgIBAAIDAQAAAgEEBAQBAgMEAAUDAQEBAwEBAQEBAQMFAwEBBAMBAQQBAQECAgIEBAQCAQIEAgEDAAcBAgQCAQIEAwAIAwEDAQMBBAEBAQEDCwMEAwECBAMBAAIDBAEBAgECAQMCBAAEAQIBAgEDAgQABwECAQIBAwIEAAoBAgECAQMCBAACBAQCAQMEBAECAwMBBAIDBwEDBAEBAwQBAQEAAwkBAQIEAQECBAEBAAEMAwMBAQMEAQEBAQIDAgUEAQACAwAAAgMAAAECAgMAAQIDAAACAQECBAEBAgQBAQABBQMDAQEDBAEBAQECAwIGBAECAgMDAQECAgICAAEDAgMFAQMBAQEDAQEBAQADBwEDAgQBAwIEAAoDAwEDAgMBBAACBwMEBAMBAgQDAQIABwECBAMBAgQDAAgEAQMCBAEDAgACCAQBAwICAQMCAgEBAgECAgIBAQQCAQEEAQEBAgMBBAQDAQMEAgAGAgEBBAIBAQQBAQECAgkEAQICAgIBAQEBAQIAAwQBAwICAQQCAwAHAQMBAwEDAgQACgICAQMCAgEDAQIAAQIKBAECBAICAQMBAQECAAMEAQMCBAEDAgQABwMDAQMCAwEEAAsBAQICAQECAgIBAAECAgMCCwQBAgICAwEBAQIBAgACBQECAQEBAgEBAQEAAwcBAwIDAQMCBAAKAgMBAwIDAQQAAgwEAQECAgMBAgMEAAQCAwECAgQBAwAHAQICAwECAwQACgIDAQICBAEDAAINBAEDAgIDAgEBAwECAQIFAQICAQECAgECAQADAgECBwEEAgMBBAIDAAoCAwEEAgMBBAACDgQBAgQCAwEDAQIBAgACBQECAQMBAgEDAQEAAggCAgECAgMBBAALAQICAgECAwQAAg8EAQEEAgMBBAIDAAQCAwEEAgMBBAAIAgEBAgIBAQMBAQECCwECAgEBAgIBAgEAAwIBAgIQAwEDBAIDAgQBAwAFAQICAwECAwQACAIDAQICBAEDAAIRBAECAwIDAQMCBAAFAQIBAgEDAgQACAECAQIBAwIEAAsBAgECAQMCBAACEgQBAwMCAwIDAQQABQECAgIBAgMEAAgBAwECAQMBAgEBAAILAQIBAwECAQMBAQACAhMEAgEEAQIBBAECAQEAAgUBAgEEAQIBBAEBAAIIAwIBAgQCAQMACAQCAQEEAgEBAQECAwIUAgEABAIDAAMBAgAHAwECAwMBAgQAAhUEAQEEAgMBBAIDAAQCAwEEAgMBBAAIAgEBAgIBAQMBAQECCwECAgEBAgIBAgEAAwIBAgIWBAEEBAIDAwQBAgACBAMBAQQDAQEBAQIDBQECAwMBAgMEAAgCBAECAgQBAwACFwQCBAIBAQQCAQEBAQIDAQQDAgMEAgEDAAUBAgMCAQIEAwAIAQQBAgEEAQIBAQACAhgDAgQCAwEEAgMBAAEEAwQDAwEEAgAFAQQDAgEEAwIAAhkEAQMDAgMCAwEEAAUBAgICAQIDBAAIAQMBAgEDAQIBAQACCwECAQMBAgEDAQEAAgIaBAICBAECAgQBAwAFAQICBAECAwQACAQDAQEEAwEBAQECAwgDAwECAwQBAgACGwQBAgICBAEBAQMBAgACBAQDAgEEAwIBAAUCAgEBAgMBAQEBAgMHAQMDAwECAwQAAhwEAQECAgQBAgMEAAQEAwEBBAMBAQEBAgMEAwMBAgMEAQIABwECAwMBAgMEAAIdBAEDAgIEAgEBAwECAQIEBAMDAQQCAwEABQICAgECAwQBAAcBBAMDAQQCAwACHgQBAgQCBAEDAgQABQICAQMCAgEDAQIAAQkBAQEBAQEBAQEBAAMLAQMCAgEDAgIBAgIDAh8DAQAEAgQAAgEDAAcDAQMDAgEDBAAKAgQDAQIEAwEAAiAEAQQEAgQCAwEEAAYBAQICAQECAgIBAAECAgMJAQMBAQEDAQEBAQADCwEDAgQBAwIEAAIhBAECAgECAgMBBAAEAQICAgECAwQABwEDAQIBAwECAQEAAgoBAgEDAQIBAwEBAAICIgQBAgIBAwICAQMBAgABBQEBAQEBAQEBAQEAAwcBAwICAQMCAgECAgMKAQMBAwEDAgQAAiMEAQECAQMBAgEDAQEAAgQCAgECAgMBBAAHAQICAgECAwQACgEDAQIBAwECAQEAAgIkBAECBAEDAgQBAwAFAQEBAwEBAQMBAQACBwQDAgEEAwIBAAgCAgEBAgIBAQIBAgMCAAECJQQBAgQBAgIEAQMABAECAgQBAgMEAAcDAwECAwQBAgAHBAMBAQQDAQEBAQIDAiYEAQEEAQMBBAEDAQEAAgQCAgEEAgIBBAECAAEHBAICAQQCAgEBAgECCAIBAQECAQEBAQEBAwInAwcDAQICAwECAgECAgMHBAECAQQBAgEBAQEDCgEDAwEBAwQBAQEAAwIoBAECAgEEAgIBBAECAAEEBAICAQQCAgEBAgECBQIBAQECAQEBAQEBAwcBAwMCAQMEAgACKQQBAQIBBAECAQQBAQACBAMCAQIEAgEDAAQEAgEBBAIBAQEBAgMHAQIDAgECBAMAAioEAQIEAQQCAwEEAAUCAQEDAgEBAwEBAQIHBAMDAQQCAwEACAICAgECAwQBAAIrAwcDAQMCAwEEAgAHBAEDAQQBAwEBAQEDCgEEAwEBBAMBAQEAAwIsBAIBAwEEAQMBBAEBAAIFAwIBAwMCAQQACQEBAgEBAQIBAQEAAwsBBAICAQQCAgECAgMCLQMFAwEBAwMBAQQBAQECBwQDBAEDAgQBAAgCAgMBAgIDAQECAAEBAAQBAQEBBAEBAQQBAQACBAMCAQEDAgEBAQECAwYBAwQDAQIEAwAIAwECAQMBAgEBAQEDAQEEAQEEBAQBAgMEAAQDAQEBAgEBAQEBAQMEAwEBBAMBAQQBAQECBgEDBAIBAwQCAAECBAMEAgECBAIBAwAGAQIEAgECBAMABwMBAwEDAQQBAQEBAwoDBAMBAgQDAQABAwQBAAEAAQABAAIAAQMEAwQBAwIEAAYBAgECAQMCBAAJAQIBAgEDAgQAAQQEAQEDBAQBAgMDAQQCAwYBAwQBAQMEAQEBAAMIAQECBAEBAgQBAQABCwMDAQEDBAEBAQECAwEFBAEBAQIEAQECBAEBAAEEAwMBAQMEAQEBAQIDBgEDBAQBAgMEAAgEAQIEAwECBAABBgQBAQECAgEBAgICAQABAgIDBAEDAQEBAwEBAQEAAwYBAwIEAQMCBAAJAwMBAwIDAQQAAQcEAQABAQQAAQEEAQEBAgMEAwECBAMBAgAGAQIEAwECBAMABwQBAwIEAQMCAAEIBAECAQEAAwECAAABAgEBBAIBAQQBAQECAgEEBAMBAwQCAAQDAgIBBAIDAQABCQQBAQEBAQEBAQEBAQADAwEDAgIBBAIDAAYBAwEDAQMCBAAJAgIBAwICAQMBAgABAQoEAQEDAQEBAwEBAQEAAwMBAwIEAQMCBAAGAwMBAwIDAQQACgEBAgIBAQICAgEAAQICAwELBAEBAQECAQEBAgEBAAIEAQIBAQECAQEBAQADBgEDAgMBAwIEAAkCAwEDAgMBBAABDAQBAAEBAgABAgMAAwIDAQICBAEDAAYBAgIDAQIDBAAJAgMBAgIEAQMAAQ0EAQIBAQICAQEDAQEBAgQBAgIBAQICAQIBAAMCAQIGAQQCAwEEAgMACQIDAQQCAwEEAAEOBAEBAwECAQMBAgEBAAIEAQIBAwECAQMBAQACBwICAQICAwEEAAoBAgICAQIDBAABDwQBAAMBAgADAQIAAwIDAQQCAwEEAAcCAQECAgEBAwEBAQIKAQICAQECAgECAQADAgECARAEAQIDAQICBAEDAAEEAwEAAwIBAAAEAQICAwECAwQABwIDAQICBAEDAAERBAEBAgECAQMCBAAEAQIBAgEDAgQABwECAQIBAwIEAAoBAgECAQMCBAABEgQBAgIBAgIDAQQABAECAgIBAgMEAAcBAwECAQMBAgEBAAIKAQIBAwECAQMBAQACARMEAQEEAQIBBAECAQEAAgQBAgEEAQIBBAEBAAIHAwIBAgQCAQMABwQCAQEEAgEBAQECAwEUAQYDAQIDAwECBAABFQQBAAMBAgADAQIAAwIDAQQCAwEEAAcCAQECAgEBAwEBAQIKAQICAQECAgECAQADAgECARYEAQMDAQIDBAECAAEEAwEBBAMBAQEBAgMEAQIDAwECAwQABwIEAQICBAEDAAEXBAEEAgEBBAIBAQEBAgMBAwIBAgQCAQMABAECAwIBAgQDAAcBBAECAQQBAgEBAAIBGAMBBAIDAQQCAwEAAQMCAwIDAQQCAAQBBAMCAQQDAgABGQQBAgIBAgIDAQQABAECAgIBAgMEAAcBAwECAQMBAgEBAAIKAQIBAwECAQMBAQACARoEAQIEAQICBAEDAAQBAgIEAQIDBAAHBAMBAQQDAQEBAQIDBwMDAQIDBAECAAEbBAEBAQEDAQEBAwEBAAIEAgIBAQIDAQEBAQIDAwQDAgEEAwIBAAYBAwMDAQIDBAABHAQBAAEBAwABAgMAAwMDAQIDBAECAAMEAwEBBAMBAQEBAgMGAQIDAwECAwQAAR0EAQIBAQMCAQEDAQEBAgQCAgIBAgMEAQADBAMDAQQCAwEABgEEAwMBBAIDAAEeBAEBAwEDAQMCBAAEAgIBAwICAQMBAgABCAEBAQEBAQEBAQEAAwoBAwICAQMCAgECAgMBHwIGAwEDAwIBAwQACQIEAwECBAMBAAEgBAEDAwEDAgMBBAAFAQECAgEBAgICAQABAgIDCAEDAQEBAwEBAQEAAwoBAwIEAQMCBAABIQQBAQEAAQECAAMAAQMEBAQBAgMEAAYBAwECAQMBAgEBAAIJAQIBAwECAQMBAQACASIEAQEBAAIBAgADAAEEBAQEAQEBAQEEAAMGAQMCAgEDAgIBAgIDCQEDAQMBAwIEAAEjBAEAAQACAAEAAgABBAQDBAIDAQQABgECAgIBAgMEAAkBAwECAQMBAgEBAAIBJAQBAQMAAgEDAAIABAEBAQMBAQEDAQEAAgYEAwIBBAMCAQAHAgIBAQICAQECAQIDAgABASUEAQEDAAEBAwACAAMBAgIEAQIDBAAGAwMBAgMEAQIABgQDAQEEAwEBAQECAwEmBAEAAwACAAIAAQADAgIBBAICAQQBAgABBgQCAgEEAgIBAQIBAgcCAQEBAgEBAQEBAQMBJwMGAwECAgMBAgIBAgIDBgQBAgEEAQIBAQEBAwkBAwMBAQMEAQEBAAMBKAQBAQEAAwECAAQABAIBAQECAQEBAQEBAwMEAgIBBAICAQECAQIGAQMDAgEDBAIAASkEAQABAAMAAQADAAMDAgECBAIBAwADBAIBAQQCAQEBAQIDBgECAwIBAgQDAAEqBAEBAwADAQIAAwAEAgEBAwIBAQMBAQECBgQDAwEEAgMBAAcCAgIBAgMEAQABKwMGAwEDAgMBBAIABgQBAwEEAQMBAQEBAwkBBAMBAQQDAQEBAAMBLAQBAQMBBAEDAQQBAQACBAMCAQMDAgEEAAgBAQIBAQECAQEBAAMKAQQCAgEEAgIBAgIDAS0DBAMBAQMDAQEEAQEBAgYEAwQBAwIEAQAHAgIDAQICAwEBAgABBAAEAQEEAAIBBAACAAEEBAQCAgMEAQAEAQEBBAEBAQQBAQACBwMCAQEEAwEBAQECAwQBBAEABAMCAAMCAQABBAQDAgMEAgEABAEEBAQBAgMEAAcDAQEBAwEBAQEBAQMEAgQBAwQDAQIEAwEABgQCAQIEAgEDAAkBAgQCAQIEAwAKAwEDAQMBBAEBAQEDBAMEAQABAAEAAQACAAEDBAMEAQMCBAAGAQIBAgEDAgQACQECAQIBAwIEAAQEBAEEBAICAwQBAQECAgMEAQMEBAECAwMBBAIDCQEDBAEBAwQBAQEAAwsBAQIEAQECBAEBAAEEBQQCAQMEAQECAwEBAQADAgMDBAECAwQBAAQBAQIEAQECBAEBAAEHAwMBAQMEAQEBAQIDBAYEAQICAAIBAgADAAQBAQICAQECAgIBAAECAgMHAQMBAQEDAQEBAQADCQEDAgQBAwIEAAQHAwYEAwECBAMBAgAJAQIEAwECBAMACgQBAwIEAQMCAAQIBAEBAAADAQAAAwABAQQEAwEDBAIAAwMCAgEDAgIBAQIBAgQCAQEEAgEBBAEBAQIECQQBAQEAAgECAAMAAQQEBAQBAQEBAQQAAwYBAwICAQQCAwAJAQMBAwEDAgQABAoEAQEBAgIBAQICAgEAAQICAwQBAwEBAQMBAQEBAAMGAQMCBAEDAgQACQMDAQMCAwEEAAQLBAEBAgACAQIAAwAEAQEBAgEBAQIBAQACBwECAQEBAgEBAQEAAwkBAwIDAQMCBAAEDAQBAQIAAQEDAAIAAwECAgMBAgMEAAYCAwECAgQBAwAJAQICAwECAwQABA0EAQECAAMBAgADAAQCAQECAgEBAwEBAQIHAQICAQECAgECAQADAgECCQEEAgMBBAIDAAQOBAEBAgICAQIDBAAEAQMBAgEDAQIBAQACBwECAQMBAgEDAQEAAgoCAgECAgMBBAAEDwQBAQICAQECAgECAQADAgECAwEEAgMBBAIDAAYCAwEEAgMBBAAKAgEBAgIBAQMBAQECBBAEAQECAgMBAgIDAQIBAgQCAwECAgQBAwAHAQICAwECAwQACgIDAQICBAEDAAQRBAEBAgECAQMCBAAEAQIBAgEDAgQABwECAQIBAwIEAAoBAgECAQMCBAAEEgQBAQIBAwECAQMBAQACBAICAQICAwEEAAcBAgICAQIDBAAKAQMBAgEDAQIBAQACBBMEAQECAwIBAgQDAAQBBAECAQQBAgEBAAIHAQIBBAECAQQBAQACCgMCAQIEAgEDAAQUAQEBAgIAAQIDAAAEFQQBAQICAQECAgECAQADAgECAwEEAgMBBAIDAAYCAwEEAgMBBAAKAgEBAgIBAQMBAQECBBYEAQECAgQBAgMEAAQDAwECAwQBAgAEBAMBAQQDAQEBAQIDBwECAwMBAgMEAAQXBAEBAgEEAQIBBAEBAAIEBAIBAQQCAQEBAQIDBAMCAQIEAgEDAAoBBAECAQQBAgEBAAIEGAQBAwIBBAMCAQQABAQCAwEEAgMBAAUCAQIBAwEEAgAHAQQDAgEEAwIABBkEAQECAQMBAgEDAQEAAgQCAgECAgMBBAAHAQICAgECAwQACgEDAQIBAwECAQEAAgQaBAEBAgMDAQIDBAAEAgQBAgIEAQMABwECAgQBAgMEAAoEAwEBBAMBAQEBAgMEGwQBAQMAAgEDAAIABAEBAQMBAQEDAQEAAgYEAwIBBAMCAQAHAgIBAQIDAQEBAQIDBBwEAQEDAAEBAwACAAMBAgIEAQIDBAAGBAMBAQQDAQEBAQIDBgMDAQIDBAECAAQdBAEBAwADAQIAAwAEAgEBAwIBAQMBAQECBgQDAwEEAgMBAAcCAgIBAgMEAQAEHgQBAQMCAgEDAgIBAgIDBAEDAQMBAwIEAAcCAgEDAgIBAwECAAELAQEBAQEBAQEBAQADBB8CAQEDAgABAwIAAAkDAQMDAgEDBAAEIAQBAQMCBAEDAgQABAMDAQMCAwEEAAgBAQICAQECAgIBAAECAgMLAQMBAQEDAQEBAQADBCEEAQABAAIAAQACAAEEBAMEAgMBBAAGAQICAgECAwQACQEDAQIBAwECAQEAAgQiBAEAAgACAAEAAgADAgIBAwICAQMBAgABBwEBAQEBAQEBAQEAAwkBAwICAQMCAgECAgMEIwQBAAIAAQACAAEAAwECAQMBAgEDAQEAAgYCAgECAgMBBAAJAQICAgECAwQABCQEAQACAgIAAQIDAAMCBAEDAgQBAwAHAQEBAwEBAQMBAQACCQQDAgEEAwIBAAQlBAEAAQICAAECAwADAgQBAgIEAQMABgECAgQBAgMEAAkDAwECAwQBAgAEJgQBAAICAQACAwEAAwEEAQMBBAEDAQEAAgYCAgEEAgIBBAECAAEJBAICAQQCAgEBAgECBCcDAQACAgAAAgMAAAkDAQICAwECAgECAgMJBAECAQQBAgEBAQEDBCgEAQADAAIAAgABAAMCAgEEAgIBBAECAAEGBAICAQQCAgEBAgECBwIBAQECAQEBAQEBAwQpBAEAAwABAAMAAQADAQIBBAECAQQBAQACBgMCAQIEAgEDAAYEAgEBBAIBAQEBAgMEKgQBAAMCAgADAQIAAwIEAQQCAwEEAAcCAQEDAgEBAwEBAQIJBAMDAQQCAwEABCsDAQADAgAAAwIAAAkDAQMCAwEEAgAJBAEDAQQBAwEBAQEDBCwEAQEEAgIBBAICAQICAwQBAwEEAQMBBAEBAAIHAwIBAwMCAQQACwEBAgEBAQIBAQEAAwQtAwcDAQEDAwEBBAEBAQIJBAMEAQMCBAEACgICAwECAgMBAQIAAQMABAEAAwMBAAMEAQABAwMDAQIDBAEAAwEBAQQBAQEEAQEAAgYDAgEBAwIBAQEBAgMDAQQBAwMCAQMEAgEAAwEEBAQBAwMDAQQBAwYDAQEBAwEBAQEBAQMGAwEBBAMBAQQBAQECAwIEAQIDAgABAwIAAAUEAgECBAIBAwAIAQIEAgECBAMACQMBAwEDAQQBAQEBAwMDBAECAwIDAQMCBAAFAQIBAgEDAgQACAECAQIBAwIEAAsBAgECAQMCBAADBAQBAwMBAQMEAQEBAQIDAwEDBAQBAgMDAQQCAwgBAwQBAQMEAQEBAAMKAQECBAEBAgQBAQABAwUEAQEDBAEBAgMBAQEAAwEDAwQBAgMEAQADAQECBAEBAgQBAQABBgMDAQEDBAEBAQECAwMGBAEDAwQEAQECAgIDAAEDAAEGAQMBAQEDAQEBAQADCAEDAgQBAwIEAAsDAwEDAgMBBAADBwMFBAMBAgMEAQIACAECBAMBAgQDAAkEAQMCBAEDAgADCAQBAAMDAgADAgEAAQQDAwIDAgIBAQMBAgMCAQEEAgEBBAEBAQIEAQQEAwEDBAIAAwkEAQMDAwMBAQEBAQMAAwUBAwICAQQCAwAIAQMBAwEDAgQACwICAQMCAgEDAQIAAQMKBAEAAAEBAAABAgADAQMBAQEDAQEBAQADBQEDAgQBAwIEAAgDAwEDAgMBBAADCwQBAwMDBAEBAQIBAwACBgECAQEBAgEBAQEAAwgBAwIDAQMCBAALAgMBAwIDAQQAAwwEAQIDAwQBAgMEAAUCAwECAgQBAwAIAQICAwECAwQACwIDAQICBAEDAAMNBAEEAwMEAgEBAwEDAQIGAQICAQECAgECAQADAgECCAEEAgMBBAIDAAsCAwEEAgMBBAADDgQBAAEBAQABAgMAAwEDAQIBAwECAQEAAgYBAgEDAQIBAwEBAAIJAgIBAgIDAQQAAw8EAQABAQAAAQIAAAIBBAIDAQQCAwAFAgMBBAIDAQQACQIBAQICAQEDAQEBAgMQBAEAAQECAAEBAgEBAQIDAgMBAgIEAQMABgECAgMBAgMEAAkCAwECAgQBAwADEQQBAAEAAQABAAIAAQMEAwQBAwIEAAYBAgECAQMCBAAJAQIBAgEDAgQAAxIEAQABAAIAAQACAAEEBAMEAgMBBAAGAQICAgECAwQACQEDAQIBAwECAQEAAgMTBAEAAQIBAAEDAgADAQQBAgEEAQIBAQACBgECAQQBAgEEAQEAAgkDAgECBAIBAwADFAEIAwECAwMBAgQAAxUEAQABAQAAAQIAAAIBBAIDAQQCAwAFAgMBBAIDAQQACQIBAQICAQEDAQEBAgMWBAEAAQEDAAECBAADAwMBAgMEAQIAAwQDAQEEAwEBAQECAwYBAgMDAQIDBAADFwQBAAEAAwABAAMAAwMCAQIEAgEDAAMEAgEBBAIBAQEBAgMJAQQBAgEEAQIBAQACAxgEAQIBAAMCAQADAAQCAQIBAwEEAgADBAIDAQQCAwEABgEEAwIBBAMCAAMZBAEAAQACAAEAAgABBAQDBAIDAQQABgECAgIBAgMEAAkBAwECAQMBAgEBAAIDGgQBAAECAgABAgMAAwIEAQICBAEDAAYBAgIEAQIDBAAJBAMBAQQDAQEBAQIDAxsEAwEBAQMBAQEDAQEAAgUEAwIBBAMCAQAGAgIBAQIDAQEBAQIDCAEDAwMBAgMEAAMcBAIBAgIEAQIDBAAFBAMBAQQDAQEBAQIDBQMDAQIDBAECAAgBAgMDAQIDBAADHQQDAgEBAwIBAQMBAQECBQQDAwEEAgMBAAYCAgIBAgMEAQAIAQQDAwEEAgMAAx4EAQACAQEAAgEBAQECAwMBAwEDAQMCBAAGAgIBAwICAQMBAgABCgEBAQEBAQEBAQEAAwMfAggDAQMDAgEDBAALAgQDAQIEAwEAAyAEAQACAQMAAgEDAAMDAwEDAgMBBAAHAQECAgEBAgICAQABAgIDCgEDAQEBAwEBAQEAAwMhBAEDAwIDAgMBBAAFAQICAgECAwQACAEDAQIBAwECAQEAAgsBAgEDAQIBAwEBAAIDIgQBAwMCBAICAQMBAwABBgEBAQEBAQEBAQEAAwgBAwICAQMCAgECAgMLAQMBAwEDAgQAAyMEAQIDAgQBAgEDAQIAAgUCAgECAgMBBAAIAQICAgECAwQACwEDAQIBAwECAQEAAgMkBAICBAEDAgQBAwAGAQEBAwEBAQMBAQACCAQDAgEEAwIBAAkCAgEBAgIBAQIBAgMCAAEDJQQCAgQBAgIEAQMABQECAgQBAgMEAAgDAwECAwQBAgAIBAMBAQQDAQEBAQIDAyYEAgEEAQMBBAEDAQEAAgUCAgEEAgIBBAECAAEIBAICAQQCAgEBAgECCQIBAQECAQEBAQEBAwMnAwgDAQICAwECAgECAgMIBAECAQQBAgEBAQEDCwEDAwEBAwQBAQEAAwMoBAICAgEEAgIBBAECAAEFBAICAQQCAgEBAgECBgIBAQECAQEBAQEBAwgBAwMCAQMEAgADKQQCAQIBBAECAQQBAQACBQMCAQIEAgEDAAUEAgEBBAIBAQEBAgMIAQIDAgECBAMAAyoEAgIEAQQCAwEEAAYCAQEDAgEBAwEBAQIIBAMDAQQCAwEACQICAgECAwQBAAMrAwgDAQMCAwEEAgAIBAEDAQQBAwEBAQEDCwEEAwEBBAMBAQEAAwMsBAEAAwEBAAMBAQEBAgMDAQMBBAEDAQQBAQACBgMCAQMDAgEEAAoBAQIBAQECAQEBAAMDLQMGAwEBAwMBAQQBAQECCAQDBAEDAgQBAAkCAgMBAgIDAQECAAEFAAQBAgABAAIAAQAAAQIAAQMCAAEDAAUBAQEEAQEBBAEBAAIIAwIBAQMCAQEBAQIDBQEEAQEAAQMBAAIEAAMDAwIBAwQCAQAFAQQEBAECAwQACAMBAQEDAQEBAQEBAwUCBAIDBAMBAgQDAQAHBAIBAgQCAQMACgECBAIBAgQDAAsDAQMBAwEEAQEBAQMFAwQBAQIBAgEDAgQABAECAQIBAwIEAAcBAgECAQMCBAAKAQIBAgEDAgQABQQEAQAAAQMAAAEDAAMDAwEBAwQBAQEBAgMFAQMEBAECAwMBBAIDCgEDBAEBAwQBAQEAAwUFBAEDAAEBAwABAQEBAgMBAwABAwIAAQMAAwEDBAEBAgMBAQEAAwMDAwQBAgMEAQAFBgQBAwMBAwIDAQQABQEBAgIBAQICAgEAAQICAwgBAwEBAQMBAQEBAAMKAQMCBAEDAgQABQcDAgEEBAEBAwQBAQEAAwcEAwECBAMBAgAKAQIEAwECBAMABQgEAQIBAQADAQIAAAECAQEEAgEBBAEBAQICAQQEAwEDBAIABQIBAQQCAQEEAQEBAgUJBAECAgEDAgMBBAAFAQEBAQEBAQEBAQADBwEDAgIBBAIDAAoBAwEDAQMCBAAFCgQBAgIDAwEBAgICAgABAwIDBQEDAQEBAwEBAQEAAwcBAwIEAQMCBAAKAwMBAwIDAQQABQsEAQIDAQMCAwEEAAUBAQECAQEBAgEBAAIIAQIBAQECAQEBAQADCgEDAgMBAwIEAAUMBAECAwECAgQBAwAEAQICAwECAwQABwIDAQICBAEDAAoBAgIDAQIDBAAFDQQBAgMBBAIDAQQABQIBAQICAQEDAQEBAggBAgIBAQICAQIBAAMCAQIKAQQCAwEEAgMABQ4EAQIDAwMBAgMEAAUBAwECAQMBAgEBAAIIAQIBAwECAQMBAQACCwICAQICAwEEAAUPBAECAwMCAQICAQICAAMDAQIEAQQCAwEEAgMABwIDAQQCAwEEAAsCAQECAgEBAwEBAQIFEAQBAgMDBAECAgMBAwECBQIDAQICBAEDAAgBAgIDAQIDBAALAgMBAgIEAQMABREEAQIDAgMBAwIEAAUBAgECAQMCBAAIAQIBAgEDAgQACwECAQIBAwIEAAUSBAECAwIEAQIBAwECAAIFAgIBAgIDAQQACAECAgIBAgMEAAsBAwECAQMBAgEBAAIFEwQBAgMEAwECBAMABQEEAQIBBAECAQEAAggBAgEEAQIBBAEBAAILAwIBAgQCAQMABRQBAQECAgABAgMAAAUVBAECAwMCAQICAQICAAMDAQIEAQQCAwEEAgMABwIDAQQCAwEEAAsCAQECAgEBAwEBAQIFFgQCAQICBAECAwQABQMDAQIDBAECAAUEAwEBBAMBAQEBAgMIAQIDAwECAwQABRcEAgECAQQBAgEEAQEAAgUEAgEBBAIBAQEBAgMIAQIDAgECBAMACwEEAQIBBAECAQEAAgUYBAIDAgEEAwIBBAAFBAIDAQQCAwEABgIBAgEDAQQCAAgBBAMCAQQDAgAFGQQBAgMCBAECAQMBAgACBQICAQICAwEEAAgBAgICAQIDBAALAQMBAgEDAQIBAQACBRoEAQIDBAQBAgMEAAUCBAECAgQBAwAIAQICBAECAwQACwQDAQEEAwEBAQECAwUbBAECBAEDAgQBAwAFAQEBAwEBAQMBAQACBwQDAgEEAwIBAAgCAgEBAgMBAQEBAgMFHAQBAgQBAgIEAQMABAECAgQBAgMEAAcEAwEBBAMBAQEBAgMHAwMBAgMEAQIABR0EAQIEAQQCAwEEAAUCAQEDAgEBAwEBAQIHBAMDAQQCAwEACAICAgECAwQBAAUeBAEAAAAAAAAAAAABAgQDAwEDAgIBAwIDBQEDAQMBAwIEAAgCAgEDAgIBAwECAAEFHwIBAgQDAQIEAwEACgMBAwMCAQMEAAUgBAEAAgAAAAIAAAACAQMCBAEDAgQABQMDAQMCAwEEAAkBAQICAQECAgIBAAECAgMFIQQBAQIBAwECAQMBAQACBAICAQICAwEEAAcBAgICAQIDBAAKAQMBAgEDAQIBAQACBSIEAQEDAQMBAwIEAAQCAgEDAgIBAwECAAEIAQEBAQEBAQEBAQADCgEDAgIBAwICAQICAwUjBAEBAwECAQMBAgEBAAIEAQIBAwECAQMBAQACBwICAQICAwEEAAoBAgICAQIDBAAFJAQBAQMDAwECAwQABAIEAQMCBAEDAAgBAQEDAQEBAwEBAAIKBAMCAQQDAgEABSUEAQECAwMBAgMEAAQCBAECAgQBAwAHAQICBAECAwQACgMDAQIDBAECAAUmBAEBAwMCAQMEAgAEAQQBAwEEAQMBAQACBwICAQQCAgEEAQIAAQoEAgIBBAICAQECAQIFJwMBAQMDAQEDBAEBAQADCgMBAgIDAQICAQICAwoEAQIBBAECAQEBAQMFKAQBAQQBAwEEAQMBAQACBAICAQQCAgEEAQIAAQcEAgIBBAICAQECAQIIAgEBAQIBAQEBAQEDBSkEAQEEAQIBBAECAQEAAgQBAgEEAQIBBAEBAAIHAwIBAgQCAQMABwQCAQEEAgEBAQECAwUqBAEBBAMDAQQCAwAEAgQBBAIDAQQACAIBAQMCAQEDAQEBAgoEAwMBBAIDAQAFKwMBAQQDAQEEAwEBAQADCgMBAwIDAQQCAAoEAQMBBAEDAQEBAQMFLAQBAAABAAAAAQAAAgEEAgIBBAICAQICAwUBAwEEAQMBBAEBAAIIAwIBAwMCAQQABS0EAwADAgEAAwIBAAgDAQEDAwEBBAEBAQIKBAMEAQMCBAEACwICAwECAgMBAQIAAQcABAEAAgMCAAEDAgABBAIDAgMBAgEBAgEDAgMBAgQDAQIEAAcBAQEEAQEBBAEBAAIHAQQBAAIDAQACAwEAAQMCAwEDAgQBAAICAQIEAgEDBAAFAwMCAQMEAgEABwIEAQABAwEAAQMCAAEDAQMBAwEEAQEBAQMEAwQDAQIEAwEACQQCAQIEAgEDAAcDBAEAAQABAAEAAgABAwQDBAEDAgQABgECAQIBAwIEAAkBAgECAQMCBAAHBAQBAAIDAAACAwAAAgEBAgQBAQIEAQEAAQUDAwEBAwQBAQEBAgMHAQMEBAECAwMBBAIDBwUEAQACAwMAAQIDAAIEAQICBAECAgECAgMCBAECBAMBAgQABQEDBAEBAwQBAQEAAwcGBAEAAgEDAAIBAwADAwMBAwIDAQQABwEBAgIBAQICAgEAAQEAAQoBAwEBAQMBAQEBAAMHBwMBBAEDAgQBAwIABAEEBAEBAwQBAQEAAwkEAwECBAMBAgAHCAQBAAMDAgACAwEAAQQDAwIDAgIBAQMBAgMCAQEEAgEBBAEBAQIEAQQEAwEDBAIABwkEAQACAAIAAQACAAMCAgEDAgMBBAAHAQEBAQEBAQEBAQADCQEDAgIBBAIDAAcKBAECAgACAQIAAwAEAQECAgEBAgICAQABAgIDBwEDAQEBAwEBAQEAAwkBAwIEAQMCBAAHCwQBAAIBAgACAQMAAwIDAQMCAwEEAAcBAQECAQEBAgEBAAIKAQIBAQECAQEBAQADBwwEAQABAQIAAQIDAAMCAwECAgQBAwAGAQICAwECAwQACQIDAQICBAEDAAcNBAEAAwECAAMBAgADAgMBBAIDAQQABwIBAQICAQEDAQEBAgoBAgIBAQICAQIBAAMCAQIHDgQBAgIBAgIDAQQABAECAgIBAgMEAAcBAwECAQMBAgEBAAIKAQIBAwECAQMBAQACBw8EAQIBAQICAQEDAQEBAgQBAgIBAQICAQIBAAMCAQIGAQQCAwEEAgMACQIDAQQCAwEEAAcQBAECAwECAgQBAwAEAQICAwECAgMBAgECBwIDAQICBAEDAAoBAgIDAQIDBAAHEQQBAQIBAgEDAgQABAECAQIBAwIEAAcBAgECAQMCBAAKAQIBAgEDAgQABxIEAQEDAQIBAwECAQEAAgQBAgEDAQIBAwEBAAIHAgIBAgIDAQQACgECAgIBAgMEAAcTBAEDAgECBAIBAwABBAIBAQQCAQEBAQIDBAECAwIBAgQDAAcBBAECAQQBAgEBAAIHFAMBAgABAgIAAQMAAgQEBAECAwQBAAMCAwMBAgMEAQAHFQQBAgEBAgIBAQMBAQECBAECAgEBAgIBAgEAAwIBAgYBBAIDAQQCAwAJAgMBBAIDAQQABxYEAQIEAQICBAEDAAQBAgIEAQIDBAAHAwMBAgMEAQIABwQDAQEEAwEBAQECAwcXBAEBBAECAQQBAgEBAAIEAQIBBAECAQQBAQACBwQCAQEEAgEBAQECAwoBAgMCAQIEAwAHGAQBAQQBBAEDAQQBAQACBAMCAQQDAgEEAAcEAgMBBAIDAQAIAgECAQMBBAIABxkEAQEDAQIBAwECAQEAAgQBAgEDAQIBAwEBAAIHAgIBAgIDAQQACgECAgIBAgMEAAcaBAEEAwEBBAMBAQEBAgMBAwMBAgMEAQIABAECAwMBAgMEAAcCBAECAgQBAwAHGwQBAAICAgABAgMAAwIEAQMCBAEDAAcBAQEDAQEBAwEBAAIJBAMCAQQDAgEABxwEAQABAgIAAQIDAAMCBAECAgQBAwAGAQICBAECAwQACQQDAQEEAwEBAQECAwcdBAEAAwICAAIBAQECAgMDAgQBBAIDAQQABwIBAQMCAQEDAQEBAgkEAwMBBAIDAQAHHgQBAgICAgEBAQEBAgADBAEDAgIBAwICAQICAwcBAwEDAQMCBAAKAgIBAwICAQMBAgABBx8CAQIAAgIBAAIDAAMCBAMBAgQDAQAHIAQBAgQCAgEDAQEBAgADBAEDAgQBAwIEAAcDAwEDAgMBBAALAQECAgEBAgICAQABAgIDByEEAQACAAEAAgABAAMBAgEDAQIBAwEBAAIGAgIBAgIDAQQACQECAgIBAgMEAAciBAEAAgEBAAIBAQEBAgMDAQMBAwEDAgQABgICAQMCAgEDAQIAAQoBAQEBAQEBAQEBAAMHIwQBAAEBAQABAgMAAwEDAQIBAwECAQEAAgYBAgEDAQIBAwEBAAIJAgIBAgIDAQQAByQEAQMCAQADAgEAAAECAgEBAgIBAQIBAgMCAAEDAQMDAwECAwQABgIEAQMCBAEDAAclBAECAgABAgMAAQABAwIAAAMCAAAAAwECAwMBAgMEAAYCBAECAgQBAwAHJgQBAwEBAAMBAgAAAQIBAQECAQEBAQEBAwMBAwMCAQMEAgAGAQQBAwEEAQMBAQACBycEAQIAAQEDAAEBAQECAwEDAAEAAwABAAADAQMDAQEDBAEBAQADBQEBAgABAgMAAAcoBAEAAgIBAAIDAQADAQQBAwEEAQMBAQACBgICAQQCAgEEAQIAAQkEAgIBBAICAQECAQIHKQQBAAECAQABAwIAAwEEAQIBBAECAQEAAgYBAgEEAQIBBAEBAAIJAwIBAgQCAQMAByoEAQMCAgADAQIAAAECAgIBAgMEAQADAQQDAwEEAgMABgIEAQQCAwEEAAcrBAECAAIBAgADAQABAwACAAMAAgAAAwEEAwEBBAMBAQEAAwUBAgIAAQIDAAAHLAQBAgIDAgEBAgEBAgADBAEEAgIBBAICAQICAwcBAwEEAQMBBAEBAAIKAwIBAwMCAQQABy0DAQMCAwACAQMAAAECAgMBAgIDAQECAAEKAwEBAwMBAQQBAQECBgAEAQMBAgEDAQIBAQEBAwEDAQIEAwECBAAGAQEBBAEBAQQBAQACCQMCAQEDAgEBAQECAwYBBAECAQIAAgEDAAABAgECBAIBAwQABAMDAgEDBAIBAAkDAQEBAwEBAQEBAQMGAgQBAgACAAIAAwAAAwMEAwECBAMBAAgEAgECBAIBAwALAQIEAgECBAMABgMEAQIDAgMBAwIEAAUBAgECAQMCBAAIAQIBAgEDAgQACwECAQIBAwIEAAYEBAEBAQIEAQECBAEBAAEEAwMBAQMEAQEBAQIDBgEDBAQBAgMDAQQCAwsBAwQBAQMEAQEBAAMGBQQBBAECBAMBAgQABAEDBAEBAgMBAQEAAwQDAwQBAgMEAQAGAQECBAEBAgQBAQABBgYEAQQEAgQCAwEEAAYBAQICAQECAgIBAAECAgMJAQMBAQEDAQEBAQADCwEDAgQBAwIEAAYHAwEDAAIBAwACAQADAQQEAQEDBAEBAQADCAQDAQIEAwECAAYIBAEDAgIBAwICAQECAQICAgEBBAIBAQQBAQECAwEEBAMBAwQCAAYCAQEEAgEBBAEBAQIGCQQBAwMCBAIDAQQABgEBAQEBAQEBAQEAAwgBAwICAQQCAwALAQMBAwEDAgQABgoEAQMDBAQBAQICAgMAAQQCAwYBAwEBAQMBAQEBAAMIAQMCBAEDAgQACwMDAQMCAwEEAAYLBAEDBAIEAgMBBAAGAQEBAgEBAQIBAQACCQECAQEBAgEBAQEAAwsBAwIDAQMCBAAGDAQBAwQCAwIEAQMABQECAgMBAgMEAAgCAwECAgQBAwALAQICAwECAwQABg0EAgIDAQQCAwEEAAYCAQECAgEBAwEBAQIJAQICAQECAgECAQADAgECCwEEAgMBBAIDAAYOBAEBAQABAQIAAwABAwQEBAECAwQABgEDAQIBAwECAQEAAgkBAgEDAQIBAwEBAAIGDwQBAQAAAQEAAAIAAQMEBAMBAgIBAgMAAwQBAgUBBAIDAQQCAwAIAgMBBAIDAQQABhAEAQECAAEBAwACAAMBAgIDAQICAwECAQIGAgMBAgIEAQMACQECAgMBAgMEAAYRBAEAAQABAAEAAgABAwQDBAEDAgQABgECAQIBAwIEAAkBAgECAQMCBAAGEgQBAAIAAQACAAEAAwECAQMBAgEDAQEAAgYCAgECAgMBBAAJAQICAgECAwQABhMEAQIBAAEDAQACAAEDAQAAAwEAAAADAQIDAgECBAMABgEEAQIBBAECAQEAAgYUAgEDBAQCAgMEAQABBAQEAQIDBAEABhUEAQEAAAEBAAACAAEDBAQDAQICAQIDAAMEAQIFAQQCAwEEAgMACAIDAQQCAwEEAAYWBAEBAwABAQMAAgADAQICBAECAwQABgMDAQIDBAECAAYEAwEBBAMBAQEBAgMGFwQBAAMAAQADAAEAAwECAQQBAgEEAQEAAgYEAgEBBAIBAQEBAgMJAQIDAgECBAMABhgEAQADAAMAAQACAAMDAgEEAwIBBAAGBAIDAQQCAwEABwIBAgEDAQQCAAYZBAEAAgABAAIAAQADAQIBAwECAQMBAQACBgICAQICAwEEAAkBAgICAQIDBAAGGgQBAwIAAAMCAAAAAQICAAECAwABAAMBAgMDAQIDBAAGAgQBAgIEAQMABhsEAgIEAQMCBAEDAAYBAQEDAQEBAwEBAAIIBAMCAQQDAgEACQICAQECAwEBAQECAwYcBAICBAECAgQBAwAFAQICBAECAwQACAQDAQEEAwEBAQECAwgDAwECAwQBAgAGHQQCAgQBBAIDAQQABgIBAQMCAQEDAQEBAggEAwMBBAIDAQAJAgICAQIDBAEABh4EAQEBAQEBAQEBAQEAAwMBAwICAQMCAgECAgMGAQMBAwEDAgQACQICAQMCAgEDAQIAAQYfAgICBAMBAgQDAQALAwEDAwIBAwQABiAEAQEDAQEBAwEBAQEAAwMBAwIEAQMCBAAGAwMBAwIDAQQACgEBAgIBAQICAgEAAQICAwYhBAECAwIEAQIBAwECAAIFAgIBAgIDAQQACAECAgIBAgMEAAsBAwECAQMBAgEBAAIGIgQBAgQCBAEDAgQABQICAQMCAgEDAQIAAQkBAQEBAQEBAQEBAAMLAQMCAgEDAgIBAgIDBiMEAQIEAgMBAwECAQIAAgUBAgEDAQIBAwEBAAIIAgIBAgIDAQQACwECAgIBAgMEAAYkBAEBAQAAAQIAAAABAgQEBAECAwQABQIEAQMCBAEDAAkBAQEDAQEBAwEBAAIGJQQBAgMEBAECAwQABQIEAQICBAEDAAgBAgIEAQIDBAALAwMBAgMEAQIABiYEAQEAAAABAAAAAAECBAQDAQMEAgAFAQQBAwEEAQMBAQACCAICAQQCAgEEAQIAAQYnAwECBAQCAQMEAQECAAMLAwECAgMBAgIBAgIDCwQBAgEEAQIBAQEBAwYoBAIBBAEDAQQBAwEBAAIFAgIBBAICAQQBAgABCAQCAgEEAgIBAQIBAgkCAQEBAgEBAQEBAQMGKQQCAQQBAgEEAQIBAQACBQECAQQBAgEEAQEAAggDAgECBAIBAwAIBAIBAQQCAQEBAQIDBioEAQEBAQABAgMAAAIBBAMDAQQCAwAFAgQBBAIDAQQACQIBAQMCAQEDAQEBAgYrAwIBBAMBAQQDAQEBAAMLAwEDAgMBBAIACwQBAwEEAQMBAQEBAwYsBAEBAQIBAQECAQEBAAMDAQQCAgEEAgIBAgIDBgEDAQQBAwEEAQEAAgkDAgEDAwIBBAAGLQMBAQECAAECAwAACQMBAQMDAQEEAQEBAgsEAwQBAwIEAQA=",
};


// Piano: 528 chords, 528 voicings, 4331 bytes.
const PCWSTR kPianoKeys[] = {
    L"C", L"C#", L"D", L"Eb", L"E", L"F", L"F#", L"G", L"Ab", L"A", L"Bb", L"B",
};

const PCWSTR kPianoSuffixes[] = {
    L"major", L"m", L"dim", L"dim7", L"sus2", L"sus4",
    L"7sus4", L"aug", L"aug7", L"aug9", L"5", L"6",
    L"69", L"7", L"7b5", L"7b9", L"7sharp9", L"7b5b9",
    L"7sharp5b9", L"7b5sharp9", L"7sharp5sharp9", L"9", L"9b5", L"9sharp11",
    L"11", L"13", L"add9", L"maj7", L"maj7b5", L"maj7sharp5",
    L"maj9", L"maj11", L"maj13", L"m6", L"m69", L"m7",
    L"m7b5", L"m9", L"m11", L"mmaj7", L"mmaj7b5", L"mmaj9",
    L"mmaj11", L"madd9",
};

const PCWSTR kPianoSuffixNames[] = {
    L"", L"m", L"dim", L"dim7", L"sus2", L"sus4", L"7sus4", L"aug",
    L"aug7", L"aug9", L"5", L"6", L"69", L"7", L"7b5", L"7b9",
    L"7#9", L"7b5b9", L"7#5b9", L"7b5#9", L"7#5#9", L"9", L"9b5", L"9#11",
    L"11", L"13", L"add9", L"maj7", L"maj7b5", L"maj7#5", L"maj9", L"maj11",
    L"maj13", L"m6", L"m69", L"m7", L"m7b5", L"m9", L"m11", L"mmaj7",
    L"mmaj7b5", L"mmaj9", L"mmaj11", L"madd9",
};

const char* const kPianoChunks[] = {
    "AAABAzxAQwABAQI8QwACAQE8AAMBAjxFAAQBAzw+QwAFAQM8QUMABgEDPEFDAAcBAjxAAAgBAjxAAAkBAzxAPgAKAQI8QwALAQQ8QENFAAwBBTxAQ0U+AA0BAzxAQwAOAQI8QAAPAQM8QEMAEAEDPEBDABEBAjxAABIBAjxAABMBAjxAABQBAjxAABUBBDxAQz4AFgEDPEA+ABcBBDxAQz4AGAEFPEBDPkEAGQEGPEBDPkFFABoBBDxAQz4AGwEEPEBDRwAcAQM8QEcAHQEDPEBHAB4BBTxAQ0c+AB8BBTxDRz5BACABBjxAQ0c+RQAhAQM8Q0UAIgEEPENFPgAjAQI8QwAkAQE8ACUBAzxDPgAmAQQ8Qz5BACcBAzxDRwAoAQI8RwApAQQ8Q0c+ACoBBTxDRz5BACsBAzxDPgEAAQM9QUQBAQEDPUBEAQIBAz1AQwEDAQQ9QENGAQQBAz0/RAEFAQM9QkQBBgEEPUJERwEHAQM9QUUBCAEEPUFFRwEJAQU9QUVHPwEKAQI9RAELAQQ9QURGAQwBBT1BREY/AQ0BBD1BREcBDgEEPUFDRwEPAQU9QURHPgEQAQU9QURHQAERAQU9QUNHPgESAQU9QUVHPgETAQU9QUNHQAEUAQU9QUVHQAEVAQU9QURHPwEWAQU9QUNHPwEXAQY9QURHP0MBGAEGPUFERz9CARkBBz1BREc/QkYBGgEEPUFEPwEbAQQ9QUQ8ARwBBD1BQzwBHQEEPUFFPAEeAQU9QUQ8PwEfAQU9RDw/QgEgAQY9QUQ8P0YBIQEEPUBERgEiAQU9QERGPwEjAQQ9QERHASQBBD1AQ0cBJQEFPUBERz8BJgEGPUBERz9CAScBBD1ARDwBKAEEPUBDPAEpAQU9QEQ8PwEqAQY9QEQ8P0IBKwEEPUBEPwIAAQM+QkUCAQEDPkFFAgIBAz5BRAIDAQQ+QURHAgQBAz5ARQIFAQM+Q0UCBgEEPkNFPAIHAQM+QkYCCAEEPkJGPAIJAQU+QkY8QAIKAQI+RQILAQQ+QkVHAgwBBT5CRUdAAg0BBD5CRTwCDgEEPkJEPAIPAQU+QkU8PwIQAQU+QkU8QQIRAQU+QkQ8PwISAQU+QkY8PwITAQU+QkQ8QQIUAQU+QkY8QQIVAQU+QkU8QAIWAQU+QkQ8QAIXAQY+QkU8QEQCGAEGPkJFPEBDAhkBBz5CRTxAQ0cCGgEEPkJFQAIbAQQ+QkU9AhwBBD5CRD0CHQEEPkJGPQIeAQU+QkU9QAIfAQU+RT1AQwIgAQY+QkU9QEcCIQEEPkFFRwIiAQU+QUVHQAIjAQQ+QUU8AiQBAz5BPAIlAQU+QUU8QAImAQY+QUU8QEMCJwEDPkFFAigBAj5BAikBBD5BRUACKgEFPkFFQEMCKwEEPkFFQAMAAQM/Q0YDAQEDP0JGAwIBAz9CRQMDAQQ/QkU8AwQBAz9BRgMFAQM/REYDBgEEP0RGPQMHAQM/Q0cDCAEEP0NHPQMJAQU/Q0c9QQMKAQI/RgMLAQQ/Q0Y8AwwBBT9DRjxBAw0BBD9DRj0DDgEEP0NFPQMPAQU/Q0Y9QAMQAQU/Q0Y9QgMRAQU/Q0U9QAMSAQU/Q0c9QAMTAQU/Q0U9QgMUAQU/Q0c9QgMVAQU/Q0Y9QQMWAQU/Q0U9QQMXAQY/Q0Y9QUUDGAEGP0NGPUFEAxkBBz9DRj1BRDwDGgEEP0NGQQMbAQQ/Q0Y+AxwBBD9DRT4DHQEEP0NHPgMeAQU/Q0Y+QQMfAQU/Rj5BRAMgAQY/Q0Y+QTwDIQEEP0JGPAMiAQU/QkY8QQMjAQQ/QkY9AyQBBD9CRT0DJQEFP0JGPUEDJgEGP0JGPUFEAycBBD9CRj4DKAEEP0JFPgMpAQU/QkY+QQMqAQY/QkY+QUQDKwEEP0JGQQQAAQNAREcEAQEDQENHBAIBA0BDRgQDAQRAQ0Y9BAQBA0BCRwQFAQNARUcEBgEEQEVHPgQHAQNARDwECAEEQEQ8PgQJAQVARDw+QgQKAQJARwQLAQRAREc9BAwBBUBERz1CBA0BBEBERz4EDgEEQERGPgQPAQVAREc+QQQQAQVAREc+QwQRAQVAREY+QQQSAQVARDw+QQQTAQVAREY+QwQUAQVARDw+QwQVAQVAREc+QgQWAQVAREY+QgQXAQZAREc+QkYEGAEGQERHPkJFBBkBB0BERz5CRT0EGgEEQERHQgQbAQRAREc/BBwBBEBERj8EHQEEQEQ8PwQeAQVAREc/QgQfAQVARz9CRQQgAQZAREc/Qj0EIQEEQENHPQQiAQVAQ0c9QgQjAQRAQ0c+BCQBBEBDRj4EJQEFQENHPkIEJgEGQENHPkJFBCcBBEBDRz8EKAEEQENGPwQpAQVAQ0c/QgQqAQZAQ0c/QkUEKwEEQENHQgUAAQNBRTwFAQECQTwFAgECQUcFAwEDQUc+BQQBA0FDPAUFAQJBPAUGAQJBPAUHAQJBRQUIAQJBRQUJAQNBRUMFCgECQTwFCwEEQUU8PgUMAQVBRTw+QwUNAQNBRTwFDgEDQUVHBQ8BA0FFPAUQAQNBRTwFEQEDQUVHBRIBAkFFBRMBA0FFRwUUAQJBRQUVAQRBRTxDBRYBBEFFR0MFFwEFQUU8Q0cFGAEEQUU8QwUZAQVBRTxDPgUaAQRBRTxDBRsBBEFFPEAFHAEEQUVHQAUdAQNBRUAFHgEFQUU8QEMFHwEEQTxAQwUgAQZBRTxAQz4FIQEDQTw+BSIBBEE8PkMFIwECQTwFJAECQUcFJQEDQTxDBSYBA0E8QwUnAQNBPEAFKAEDQUdABSkBBEE8QEMFKgEEQTxAQwUrAQNBPEMGAAEDQkY9BgEBA0JFPQYCAQNCRTwGAwEEQkU8PwYEAQNCRD0GBQEDQkc9BgYBBEJHPUAGBwEDQkY+BggBBEJGPkAGCQEFQkY+QEQGCgECQj0GCwEEQkY9PwYMAQVCRj0/RAYNAQRCRj1ABg4BBEJGPEAGDwEFQkY9QEMGEAEFQkY9QEUGEQEFQkY8QEMGEgEFQkY+QEMGEwEFQkY8QEUGFAEFQkY+QEUGFQEFQkY9QEQGFgEFQkY8QEQGFwEGQkY9QEQ8BhgBBkJGPUBERwYZAQdCRj1AREc/BhoBBEJGPUQGGwEEQkY9QQYcAQRCRjxBBh0BBEJGPkEGHgEFQkY9QUQGHwEFQj1BREcGIAEGQkY9QUQ/BiEBBEJFPT8GIgEFQkU9P0QGIwEEQkU9QAYkAQRCRTxABiUBBUJFPUBEBiYBBkJFPUBERwYnAQRCRT1BBigBBEJFPEEGKQEFQkU9QUQGKgEGQkU9QURHBisBBEJFPUQHAAEDQ0c+BwEBAkM+BwIBA0NGPQcDAQRDRj1ABwQBA0NFPgcFAQNDPD4HBgEEQzw+QQcHAQNDRz8HCAEEQ0c/QQcJAQVDRz9BRQcKAQJDPgcLAQRDRz5ABwwBBUNHPkBFBw0BBENHPkEHDgEEQ0c9QQcPAQVDRz5BRAcQAQVDRz5BRgcRAQVDRz1BRAcSAQVDRz9BRAcTAQVDRz1BRgcUAQVDRz9BRgcVAQVDRz5BRQcWAQVDRz1BRQcXAQZDRz5BRT0HGAEGQ0c+QUU8BxkBB0NHPkFFPEAHGgEEQ0c+RQcbAQRDRz5CBxwBBENHPUIHHQEEQ0c/QgceAQVDRz5CRQcfAQVDPkJFPAcgAQZDRz5CRUAHIQEDQz5AByIBBEM+QEUHIwEDQz5BByQBAkNBByUBBEM+QUUHJgEFQz5BRTwHJwECQz4HKAEBQwcpAQNDPkUHKgEEQz5FPAcrAQNDPkUIAAEDRDw/CAEBA0RHPwgCAQNERz4IAwEEREc+QQgEAQNERj8IBQEDRD0/CAYBBEQ9P0IIBwEDRDxACAgBBEQ8QEIICQEFRDxAQkYICgECRD8ICwEERDw/QQgMAQVEPD9BRggNAQREPD9CCA4BBEQ8PkIIDwEFRDw/QkUIEAEFRDw/QkcIEQEFRDw+QkUIEgEFRDxAQkUIEwEFRDw+QkcIFAEFRDxAQkcIFQEFRDw/QkYIFgEFRDw+QkYIFwEGRDw/QkY+CBgBBkQ8P0JGPQgZAQdEPD9CRj1BCBoBBEQ8P0YIGwEERDw/QwgcAQREPD5DCB0BBEQ8QEMIHgEFRDw/Q0YIHwEFRD9DRj0IIAEGRDw/Q0ZBCCEBBERHP0EIIgEFREc/QUYIIwEEREc/QggkAQRERz5CCCUBBURHP0JGCCYBBkRHP0JGPQgnAQRERz9DCCgBBERHPkMIKQEFREc/Q0YIKgEGREc/Q0Y9CCsBBERHP0YJAAEDRT1ACQEBA0U8QAkCAQNFPD8JAwEERTw/QgkEAQNFR0AJBQEDRT5ACQYBBEU+QEMJBwEDRT1BCQgBBEU9QUMJCQEFRT1BQ0cJCgECRUAJCwEERT1AQgkMAQVFPUBCRwkNAQRFPUBDCQ4BBEU9P0MJDwEFRT1AQ0YJEAEFRT1AQzwJEQEFRT0/Q0YJEgEFRT1BQ0YJEwEFRT0/QzwJFAEFRT1BQzwJFQEFRT1AQ0cJFgEFRT0/Q0cJFwEGRT1AQ0c/CRgBBkU9QENHPgkZAQdFPUBDRz5CCRoBBEU9QEcJGwEERT1ARAkcAQRFPT9ECR0BBEU9QUQJHgEFRT1AREcJHwEFRUBERz4JIAEGRT1AREdCCSEBBEU8QEIJIgEFRTxAQkcJIwEERTxAQwkkAQRFPD9DCSUBBUU8QENHCSYBBkU8QENHPgknAQRFPEBECSgBBEU8P0QJKQEFRTxAREcJKgEGRTxAREc+CSsBBEU8QEcKAAEDRj5BCgEBA0Y9QQoCAQNGPUAKAwEERj1AQwoEAQNGPEEKBQEDRj9BCgYBBEY/QUQKBwEDRj5CCggBBEY+QkQKCQEFRj5CRDwKCgECRkEKCwEERj5BQwoMAQVGPkFDPAoNAQRGPkFECg4BBEY+QEQKDwEFRj5BREcKEAEFRj5BRD0KEQEFRj5AREcKEgEFRj5CREcKEwEFRj5ARD0KFAEFRj5CRD0KFQEFRj5BRDwKFgEFRj5ARDwKFwEGRj5BRDxAChgBBkY+QUQ8PwoZAQdGPkFEPD9DChoBBEY+QTwKGwEERj5BRQocAQRGPkBFCh0BBEY+QkUKHgEFRj5BRTwKHwEFRkFFPD8KIAEGRj5BRTxDCiEBBEY9QUMKIgEFRj1BQzwKIwEERj1BRAokAQRGPUBECiUBBUY9QUQ8CiYBBkY9QUQ8PwonAQRGPUFFCigBBEY9QEUKKQEFRj1BRTwKKgEGRj1BRTw/CisBBEY9QTwLAAEDRz9CCwEBA0c+QgsCAQNHPkELAwEERz5BRAsEAQNHPUILBQEDR0BCCwYBBEdAQkULBwEDRz9DCwgBBEc/Q0ULCQEFRz9DRT0LCgECR0ILCwEERz9CRAsMAQVHP0JEPQsNAQRHP0JFCw4BBEc/QUULDwEFRz9CRTwLEAEFRz9CRT4LEQEFRz9BRTwLEgEFRz9DRTwLEwEFRz9BRT4LFAEFRz9DRT4LFQEFRz9CRT0LFgEFRz9BRT0LFwEGRz9CRT1BCxgBBkc/QkU9QAsZAQdHP0JFPUBECxoBBEc/Qj0LGwEERz9CRgscAQRHP0FGCx0BBEc/Q0YLHgEFRz9CRj0LHwEFR0JGPUALIAEGRz9CRj1ECyEBBEc+QkQLIgEFRz5CRD0LIwEERz5CRQskAQRHPkFFCyUBBUc+QkU9CyYBBkc+QkU9QAsnAQRHPkJGCygBBEc+QUYLKQEFRz5CRj0LKgEGRz5CRj1ACysBBEc+Qj0=",
};


const InstrumentData kInstruments[] = {
    {L"Guitar", kFretted, 6, kGuitarTuning, kGuitarKeys, 12, kGuitarSuffixes, kGuitarSuffixNames, 86, kGuitarChunks, 2},
    {L"Ukulele", kFretted, 4, kUkuleleTuning, kUkuleleKeys, 12, kUkuleleSuffixes, kUkuleleSuffixNames, 46, kUkuleleChunks, 1},
    {L"Piano", kPiano, 0, nullptr, kPianoKeys, 12, kPianoSuffixes, kPianoSuffixNames, 44, kPianoChunks, 1},
};

const int kInstrumentsSize = 3;

}  // namespace chorddata
// ==WindhawkModData:end==
