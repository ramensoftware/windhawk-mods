// ==WindhawkMod==
// @id              wallpaper-changer
// @name            Wallpaper Changer
// @description     Change your wallpaper from a visual selector opened with a hotkey. Three styles, collections from your folders.
// @version         1.0.0
// @author          David Ravelo
// @github          https://github.com/DavidRaveloU
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -luser32 -lgdi32 -luuid -ld2d1 -ldwrite -lwindowscodecs
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- root: ""
  $name: Wallpapers folder
  $description: >-
    Folder that holds your wallpapers. Every subfolder becomes a collection.
    Leave empty to use your Pictures folder, but then everything inside it is scanned (up to 5000
    images), personal photos and screenshots included: a folder just for wallpapers works better.
    Example: C:\Users\David\Pictures\Wallpapers\
- style: carousel
  $name: Style
  $description: >-
    How the selector looks. Takes effect the next time it opens.
  $options:
  - carousel: Card carousel
  - strip: Folder strip
  - showcase: Showcase
- transition: expand
  $name: Transition
  $description: >-
    The effect that plays when the wallpaper changes. It plays on every monitor the wallpaper is
    applied to.
  $options:
  - expand: Card expand
  - blinds: Blinds
  - pixelate: Pixelate
  - tiles: Tile wave
  - random: Random
  - none: None (instant)
- currentMonitorOnly: false
  $name: Only change the current monitor
  $description: >-
    Off: the wallpaper and the effect go to every monitor. On: only the monitor the selector opened
    on changes, and the effect plays only there.
- monitorSource: mouse
  $name: Open the selector on
  $description: >-
    Which monitor gets the selector (and, with the option above, the new wallpaper).
  $options:
  - mouse: The monitor with the mouse
  - focus: The monitor with the focused window
- hotkey: Ctrl+Alt+P
  $name: Hotkey
  $description: >-
    Opens and closes the selector. At least one modifier (Ctrl, Alt, Shift,
    Win) plus a letter, a digit or F1-F24, for example Ctrl+Alt+P.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Wallpaper Changer

Press a hotkey, pick a wallpaper, done.

Choose a folder of wallpapers, turn its subfolders into collections and change your wallpaper in a
couple of seconds without opening Settings. Three looks are available, picked with the **Style**
setting.

## How it differs from Simple Wallpaper Switcher

[Simple Wallpaper Switcher](https://windhawk.net/mods/simple-wallpaper-switcher) is a compact utility
picker: a paged thumbnail grid with folder navigation and search, themes and a tray icon. This mod is
a gallery instead. You browse one wallpaper at a time, large, with the keyboard or by dragging, and
the selector and the effect are one motion: the card you pick grows from where it was on screen until
it is the wallpaper, and the overlay stays until the desktop really shows it. That costs more (a
Direct2D scene and a monitor-size decode on every change, about 33 MB at 4K), which a compact picker
should not carry. If you want search, themes or a tray icon, Simple Wallpaper Switcher is the better
fit.

## Styles

### Card carousel

A fan of overlapping cards, the centre one in front, with your collections as tabs at the bottom.

![Card carousel](https://raw.githubusercontent.com/DavidRaveloU/Wallpaper-Changer-Windhawk/main/assets/Card%20carousel.png)

### Folder strip

Slanted cards side by side. You browse your folders first, press **Enter** to open one, and then pick
the wallpaper. **Esc** or **Backspace** goes back to the folders.

![Folder strip](https://raw.githubusercontent.com/DavidRaveloU/Wallpaper-Changer-Windhawk/main/assets/Folder%20strip.png)

### Showcase

A big preview of the selected wallpaper over a dimmed copy of itself, with a rail of thumbnails
underneath. Moving between wallpapers crossfades them. Collections are chips along the top.

![Showcase](https://raw.githubusercontent.com/DavidRaveloU/Wallpaper-Changer-Windhawk/main/assets/Showcase.png)

## Transitions

When you confirm a wallpaper, an effect plays over the desktop while Windows changes the wallpaper
underneath it. Pick it with the **Transition** setting:

- **Card expand** (default): the card you picked grows from where it was until it covers the
  monitor, its corners straightening as it goes.
- **Blinds**: horizontal slats open from their middle line, from the top down.
- **Pixelate**: the new wallpaper fades in as a coarse mosaic that sharpens into the picture.
- **Tile wave**: tiles grow in a wave that starts at the card.
- **Random** picks one each time, and **None** changes the wallpaper instantly.

The effect plays on every monitor the wallpaper is applied to (see **Several monitors** below). It
stays on screen until Windows has repainted the desktop (a couple of seconds at most), so the old
wallpaper never flashes back. The desktop icons are covered meanwhile. If the picture position in
Windows is Tile or Span, the wallpaper changes instantly.

## Several monitors

The selector opens on the monitor with the mouse, or on the one with the focused window if you set
**Open the selector on** to that. By default the wallpaper and the effect go to every monitor. Turn on
**Only change the current monitor** and just the monitor the selector opened on changes, with the
effect playing only there. If Windows cannot match that monitor, every monitor changes and the mod
log says so.

## How to use

1. Set **Wallpapers folder** in the mod settings. It defaults to your Pictures folder, which is
   scanned whole (up to 5000 images, personal photos included), so a folder just for wallpapers works
   better.
2. Press the hotkey (**Ctrl+Alt+P** by default) to open the selector.
3. Move with the **arrow keys**, the **mouse wheel** or by **dragging**. `Home` and `End` jump to the
   first and last card.
4. Press **Enter**, or click the centred card, to set it as your wallpaper. **Esc**, the close
   button or clicking another window closes the selector without changing anything.

The wallpaper only changes when you confirm, never while you browse. It goes through the Windows
desktop wallpaper setting, so Windows keeps it after the mod is disabled. Applying an image also
replaces a Slideshow or Windows Spotlight background. With **Only change the current monitor** the
wallpaper is saved for that monitor only.

## Collections

Every subfolder of your wallpapers folder is a collection. **All** lists every image, including the
ones inside subfolders and the ones sitting directly in the folder. In the card carousel each
collection has a tab at the bottom: use **Tab** and **Shift+Tab**, or click a tab, to switch. In the
folder strip each one is a card with a cover image. **F5**, or the refresh button of the selector,
rescans the folder.

## Settings

| Setting | Default | Notes |
| --- | --- | --- |
| Wallpapers folder | your Pictures folder | Every subfolder becomes a collection. Quotes around the path and `%VARIABLES%` are accepted. Empty scans the whole Pictures folder (up to 5000 images), so a folder just for wallpapers is better. |
| Style | Card carousel | Card carousel, Folder strip or Showcase. Applies the next time the selector opens. |
| Transition | Card expand | Card expand, Blinds, Pixelate, Tile wave, Random or None. |
| Only change the current monitor | off | Off: every monitor changes and plays the effect. On: only the monitor the selector opened on. |
| Open the selector on | The monitor with the mouse | Or the monitor with the focused window. |
| Hotkey | `Ctrl+Alt+P` | At least one modifier (Ctrl, Alt, Shift, Win) plus a letter, a digit or F1-F24. |

## Notes

- Supported images: JPG, JPEG, PNG, BMP and WebP (WebP needs the Windows WebP codec). The folder is
  scanned each time the selector opens, up to 5000 images.
- The mod runs as a Windhawk tool mod, in its own `windhawk.exe` process, so nothing it does can
  affect Explorer, the taskbar or the desktop.
- Previews are decoded with the Windows imaging components in that process, on a background
  thread. Images above roughly 150 megapixels show "No preview" instead of being decoded.
- Nothing runs in the background while the selector is closed, apart from the hotkey itself and, for
  a couple of seconds after you pick a wallpaper, the transition.

## Contributing

Bug reports, fixes, new features, translations and documentation corrections are welcome. Open an
[issue](https://github.com/DavidRaveloU/Wallpaper-Changer-Windhawk/issues) with your Windows build,
the mod version, the steps to reproduce it and, when it is visual, a screenshot or a short clip.

For a pull request: fork the
[repository](https://github.com/DavidRaveloU/Wallpaper-Changer-Windhawk), branch from `main`, change
`wallpaper-changer.wh.cpp`, confirm it compiles and open the PR describing **what** changed and
**why**. One bug or feature per PR is much easier to review.
*/
// ==/WindhawkModReadme==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <windowsx.h>
#include <shlobj.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Pure logic: no Windows state.
// ---------------------------------------------------------------------------

static wchar_t AsciiLower(wchar_t c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c + 32) : c;
}

static std::wstring TrimAscii(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && (s[b] == L' ' || s[b] == L'\t')) ++b;
    while (e > b && (s[e - 1] == L' ' || s[e - 1] == L'\t')) --e;
    return s.substr(b, e - b);
}

// Cleans up a folder typed or pasted into the settings: surrounding spaces, the quotes Explorer's
// "Copy as path" adds, and %VARIABLES%. An unknown variable stays as typed.
static std::wstring NormalizeRoot(const std::wstring& text) {
    std::wstring s = TrimAscii(text);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = TrimAscii(s.substr(1, s.size() - 2));
    }
    if (s.find(L'%') == std::wstring::npos) return s;

    const DWORD needed = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);  // includes the null
    if (!needed) return s;
    std::wstring out(needed, L'\0');
    const DWORD written = ExpandEnvironmentStringsW(s.c_str(), out.data(), needed);
    if (!written || written > needed) return s;
    out.resize(written - 1);
    return out;
}

// Parses "Ctrl+Alt+P" into RegisterHotKey arguments. Strict: one or more
// distinct modifiers plus exactly one key (ASCII letter, digit or F1-F24).
static bool ParseHotkey(const std::wstring& text, UINT& mods, UINT& vk) {
    UINT m = 0, key = 0;
    size_t pos = 0;
    for (;;) {
        size_t plus = text.find(L'+', pos);
        std::wstring token = TrimAscii(
            text.substr(pos, plus == std::wstring::npos ? plus : plus - pos));
        if (token.empty()) return false;

        std::wstring low;
        for (wchar_t c : token) low += AsciiLower(c);

        UINT mod = 0;
        if (low == L"ctrl" || low == L"control") mod = MOD_CONTROL;
        else if (low == L"alt") mod = MOD_ALT;
        else if (low == L"shift") mod = MOD_SHIFT;
        else if (low == L"win") mod = MOD_WIN;

        if (mod) {
            if (m & mod) return false;
            m |= mod;
        } else {
            if (key) return false;
            wchar_t c = low[0];
            if (low.size() == 1 && ((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9'))) {
                key = (c >= L'a') ? static_cast<UINT>(c - 32) : static_cast<UINT>(c);
            } else if (c == L'f' && low.size() >= 2 && low.size() <= 3 &&
                       std::all_of(low.begin() + 1, low.end(),
                                   [](wchar_t d) { return d >= L'0' && d <= L'9'; })) {
                int n = std::stoi(low.substr(1));
                if (n < 1 || n > 24) return false;
                key = static_cast<UINT>(VK_F1 + n - 1);
            } else {
                return false;
            }
        }

        if (plus == std::wstring::npos) break;
        pos = plus + 1;
    }
    if (!m || !key) return false;
    mods = m | MOD_NOREPEAT;
    vk = key;
    return true;
}

struct Image {
    std::wstring path;
    std::wstring name;  // file name without extension
};

struct Collection {
    std::wstring name;
    std::vector<Image> images;
};

// shortcut: images beyond this are ignored, raise it if someone has more wallpapers.
static constexpr size_t kMaxImages = 5000;

static bool IsImageFile(const std::filesystem::path& p) {
    std::wstring ext;
    for (wchar_t c : p.extension().wstring()) ext += AsciiLower(c);
    return ext == L".jpg" || ext == L".jpeg" || ext == L".png" || ext == L".bmp" ||
           ext == L".webp";
}

static std::wstring LowerKey(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) out += AsciiLower(c);
    return out;
}

static void SortImages(std::vector<Image>& images) {
    std::sort(images.begin(), images.end(), [](const Image& a, const Image& b) {
        return LowerKey(a.path) < LowerKey(b.path);
    });
}

// Scans `root` once. Result[0] is "All" (every image, recursive); the rest are
// the direct subfolders that hold at least one image, sorted by name. Images
// sitting directly in `root` belong to "All" only. A missing or unreadable
// root gives an empty "All"; a cancelled scan gives an empty result.
static std::vector<Collection> ScanLibrary(const std::wstring& root,
                                           const std::atomic<bool>& cancel) {
    namespace fs = std::filesystem;

    Collection all;
    all.name = L"All";
    std::map<std::wstring, Collection> byFolder;  // keyed by lowercase name

    std::error_code ec;
    fs::recursive_directory_iterator it(fs::path(root), fs::directory_options::skip_permission_denied,
                                        ec);
    // The walk is depth first, so every entry below a direct subfolder follows that subfolder.
    // Tracking it is what keeps this independent of how the root is spelled (\\?\, // and so on).
    std::wstring topFolder;
    for (; !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (cancel) return {};
        if (all.images.size() >= kMaxImages) break;

        std::error_code fileEc;
        if (it.depth() == 0 && it->is_directory(fileEc)) {
            topFolder = it->path().filename().wstring();
            continue;
        }
        if (!it->is_regular_file(fileEc) || !IsImageFile(it->path())) continue;

        Image img{it->path().wstring(), it->path().stem().wstring()};
        if (it.depth() > 0) {
            Collection& c = byFolder[LowerKey(topFolder)];
            c.name = topFolder;
            c.images.push_back(img);
        }
        all.images.push_back(std::move(img));
    }
    if (cancel) return {};

    SortImages(all.images);
    std::vector<Collection> result;
    result.push_back(std::move(all));
    for (auto& entry : byFolder) {
        SortImages(entry.second.images);
        result.push_back(std::move(entry.second));
    }
    return result;
}

// Where one card sits in the fan. `offset` is the signed distance, in cards,
// from the centre one (fractional while the selector is animating).
struct CardPose {
    float x, y;  // card centre, in stage pixels
    float scale;
    float angleDeg;  // positive tilts clockwise
    float alpha;
    int z;  // higher is drawn later (on top)
};

// Card size relative to the stage height; the renderer draws cards with these.
static constexpr float kCardWPerStageH = 0.56f;
static constexpr float kCardHPerStageH = 0.86f;
// Distance between neighbouring card centres, in card widths (they overlap a little).
static constexpr float kCardSpacing = 0.62f;

static CardPose ComputePose(float offset, float stageW, float stageH) {
    const float d = std::fabs(offset);
    const float sign = offset < 0 ? -1.f : 1.f;
    const float cardW = stageH * kCardWPerStageH;

    CardPose p;
    p.x = stageW * 0.5f + offset * cardW * kCardSpacing;
    p.y = stageH * 0.5f + d * d * stageH * 0.012f;  // side cards sag slightly: a fan, not a row
    p.scale = std::max(0.5f, 1.f - 0.1f * d);
    p.angleDeg = sign * std::min(9.f, 3.f * d);
    p.alpha = d >= 3.f ? 0.f : 1.f - std::pow(d / 3.f, 2.5f);
    p.z = 1000 - static_cast<int>(std::lround(d * 100.f));
    return p;
}

// Whatever can be clicked is a convex quad, so every style hit-tests the same way.
struct Pt {
    float x, y;
};
using Quad = std::array<Pt, 4>;  // corners in drawing order, either winding

static bool PointInQuad(const Quad& q, float px, float py) {
    bool positive = false, negative = false;
    for (size_t i = 0; i < q.size(); ++i) {
        const Pt& a = q[i];
        const Pt& b = q[(i + 1) % q.size()];
        const float cross = (b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x);
        positive |= cross > 0.f;
        negative |= cross < 0.f;
    }
    return positive != negative;  // same side of every edge; a collapsed quad has neither
}

// The corners of a card drawn with `pose`: translate, rotate and scale applied to its rectangle.
static Quad CardQuad(const CardPose& pose, float cardW, float cardH, float stageTop) {
    const float rad = pose.angleDeg * 3.14159265f / 180.f;
    const float c = std::cos(rad), s = std::sin(rad);
    const float hw = cardW * 0.5f * pose.scale, hh = cardH * 0.5f * pose.scale;
    auto corner = [&](float lx, float ly) {
        return Pt{pose.x + lx * c - ly * s, stageTop + pose.y + lx * s + ly * c};
    };
    return {corner(-hw, -hh), corner(hw, -hh), corner(hw, hh), corner(-hw, hh)};
}

// Position while dragging: moving the pointer `pixelsPerItem` to the left brings the next item in.
static float DragPosition(float startPosition, float dx, float pixelsPerItem) {
    return startPosition - dx / pixelsPerItem;
}

// The showcase style: a rail of small thumbnails under the big preview. The selected one is a
// little larger and the far ones fade out.
static constexpr float kRailZoom = 0.2f;  // extra size of the selected thumbnail

struct RailSlot {
    float cx;  // centre, in stage pixels
    float scale;
    float alpha;
};

static RailSlot ComputeRailSlot(float offset, float stageW, float thumbW, float gap) {
    const float d = std::fabs(offset);
    return {stageW * 0.5f + offset * (thumbW + gap), 1.f + kRailZoom * std::max(0.f, 1.f - d),
            std::clamp(1.f - (d - 3.2f) / 1.2f, 0.f, 1.f)};
}

// The strip style: slanted cards side by side. The centre one is wide, the rest narrow, and the
// gap between neighbours is constant, also while the strip is animating.
static constexpr float kStripWideRatio = 2.f;    // centre card width / card height
static constexpr float kStripNarrowRatio = 1.f;  // other cards
static constexpr float kStripGap = 0.04f;        // of the card height
static constexpr float kStripSkew = 0.15f;       // top edge shift over the bottom one, of the card height

struct StripSlot {
    float cx, w;  // centre and width, in stage pixels
    float alpha;
};

static StripSlot ComputeStripSlot(float offset, float stageW, float cardH) {
    const float d = std::fabs(offset);
    const float sign = offset < 0 ? -1.f : 1.f;
    const float wide = cardH * kStripWideRatio, narrow = cardH * kStripNarrowRatio;
    const float gap = cardH * kStripGap;
    // Linear between slots, which is what keeps the gap constant while animating.
    const float toFirst = wide / 2 + gap + narrow / 2;
    const float dist = d <= 1.f ? d * toFirst : toFirst + (d - 1.f) * (narrow + gap);
    return {stageW * 0.5f + sign * dist, wide + (narrow - wide) * std::min(d, 1.f),
            std::clamp(3.f - d, 0.f, 1.f)};
}

static Quad StripQuad(const StripSlot& slot, float top, float cardH) {
    const float skew = cardH * kStripSkew * 0.5f;
    const float l = slot.cx - slot.w / 2, r = slot.cx + slot.w / 2, bottom = top + cardH;
    return {Pt{l + skew, top}, Pt{r + skew, top}, Pt{r - skew, bottom}, Pt{l - skew, bottom}};
}

// The horizontal slice of a quad between t0 and t1 (0 = top edge, 1 = bottom edge).
static Quad QuadBand(const Quad& q, float t0, float t1) {
    auto at = [](const Pt& a, const Pt& b, float t) {
        return Pt{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    };
    return {at(q[0], q[3], t0), at(q[1], q[2], t0), at(q[1], q[2], t1), at(q[0], q[3], t1)};
}

static int WrapIndex(int i, int count) {
    if (count <= 0) return 0;
    int m = i % count;
    return m < 0 ? m + count : m;
}

using Microsoft::WRL::ComPtr;

struct Thumb {
    UINT w = 0, h = 0;
    std::vector<BYTE> bgra;  // premultiplied BGRA, w * h * 4 bytes
};

// shortcut: sources above this many pixels get a placeholder instead of being
// decoded (keeps a 16k x 16k image from eating gigabytes), raise it if needed.
static constexpr UINT64 kMaxSourcePixels = 150'000'000;

// Decodes `path` scaled so its short side is `shortSide` (never upscaled), or, when `exactW` and
// `exactH` are given, scaled to exactly that size. The caller's thread must have COM initialised.
static bool DecodeThumbnail(const std::wstring& path, UINT shortSide, Thumb& out, UINT exactW = 0,
                            UINT exactH = 0) {
    ComPtr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory)))) {
        return false;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    if (FAILED(factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand, &decoder))) {
        return false;
    }
    ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, &frame))) return false;

    UINT w = 0, h = 0;
    if (FAILED(frame->GetSize(&w, &h)) || !w || !h) return false;
    if (static_cast<UINT64>(w) * h > kMaxSourcePixels) return false;

    UINT nw = w, nh = h;
    const UINT shortest = std::min(w, h);
    if (exactW && exactH) {
        nw = exactW;
        nh = exactH;
    } else if (shortest > shortSide) {
        const double s = static_cast<double>(shortSide) / shortest;
        nw = std::max<UINT>(1, static_cast<UINT>(std::lround(w * s)));
        nh = std::max<UINT>(1, static_cast<UINT>(std::lround(h * s)));
    }

    ComPtr<IWICBitmapSource> source = frame;
    if (nw != w || nh != h) {
        ComPtr<IWICBitmapScaler> scaler;
        if (FAILED(factory->CreateBitmapScaler(&scaler)) ||
            FAILED(scaler->Initialize(frame.Get(), nw, nh, WICBitmapInterpolationModeFant))) {
            return false;
        }
        source = scaler;
    }

    ComPtr<IWICFormatConverter> converter;
    if (FAILED(factory->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(source.Get(), GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0.0,
                                     WICBitmapPaletteTypeCustom))) {
        return false;
    }

    out.w = nw;
    out.h = nh;
    out.bgra.resize(static_cast<size_t>(nw) * nh * 4);
    return SUCCEEDED(converter->CopyPixels(nullptr, nw * 4, static_cast<UINT>(out.bgra.size()),
                                           out.bgra.data()));
}

// ---------------------------------------------------------------------------
// Transitions: the geometry of the effects that cover the desktop while the wallpaper changes.
// `t` is the progress of the whole effect, from 0 to 1.
// ---------------------------------------------------------------------------

enum class Transition { None, Expand, Blinds, Pixelate, Tiles, Random };

struct TransitionInfo {
    const wchar_t* id;  // the value of the `transition` setting
    Transition kind;
    int durationMs;
};

static constexpr TransitionInfo kTransitions[] = {
    {L"none", Transition::None, 0},         {L"expand", Transition::Expand, 650},
    {L"blinds", Transition::Blinds, 800},   {L"pixelate", Transition::Pixelate, 700},
    {L"tiles", Transition::Tiles, 900},     {L"random", Transition::Random, 0},
};

// Unknown values fall back to Expand, the default.
static Transition ParseTransition(const std::wstring& id) {
    const std::wstring key = LowerKey(TrimAscii(id));
    for (const TransitionInfo& info : kTransitions) {
        if (key == info.id) return info.kind;
    }
    return Transition::Expand;
}

// Random becomes one of the four effects, chosen by `pick`. Anything else is returned as it is.
static Transition ResolveTransition(Transition t, unsigned pick) {
    static constexpr Transition effects[] = {Transition::Expand, Transition::Blinds,
                                             Transition::Pixelate, Transition::Tiles};
    return t == Transition::Random ? effects[pick % std::size(effects)] : t;
}

static int TransitionDuration(Transition t) {
    for (const TransitionInfo& info : kTransitions) {
        if (info.kind == t) return info.durationMs;
    }
    return 0;
}

struct RectF {
    float l, t, r, b;
    float w() const { return r - l; }
    float h() const { return b - t; }
};

// How Windows lays the wallpaper out on a monitor. Tile and Span have no counterpart here: the
// effects need one rectangle for the image, so those two change the wallpaper without an effect.
enum class Fit { Fill, Fit, Stretch, Center };

// Where an image of iw x ih lands on a monitor of mw x mh, in monitor pixels.
static RectF PlaceImage(float iw, float ih, float mw, float mh, Fit fit) {
    float w = mw, h = mh;
    switch (fit) {
        case Fit::Fill: {
            const float s = std::max(mw / iw, mh / ih);
            w = iw * s;
            h = ih * s;
            break;
        }
        case Fit::Fit: {
            const float s = std::min(mw / iw, mh / ih);
            w = iw * s;
            h = ih * s;
            break;
        }
        case Fit::Center: w = iw; h = ih; break;
        case Fit::Stretch: break;
    }
    const float l = (mw - w) / 2, t = (mh - h) / 2;
    return {l, t, l + w, t + h};
}

static float EaseInOut(float t) {
    t = std::clamp(t, 0.f, 1.f);
    return t < 0.5f ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3.f) / 2;
}

static RectF LerpRect(const RectF& a, const RectF& b, float t) {
    return {a.l + (b.l - a.l) * t, a.t + (b.t - a.t) * t, a.r + (b.r - a.r) * t,
            a.b + (b.b - a.b) * t};
}

// Slats and tiles do not open together: each one starts a little after the previous one and takes
// this share of the whole effect, so the last one finishes exactly at t = 1.
static constexpr float kPartSpan = 0.5f;

static float PartProgress(float t, float delay) {
    return EaseInOut(std::clamp((t - delay) / kPartSpan, 0.f, 1.f));
}

// How open slat `i` of `n` is, from the top one down.
static float BlindOpen(int i, int n, float t) {
    return PartProgress(t, n > 1 ? static_cast<float>(i) / (n - 1) * (1 - kPartSpan) : 0.f);
}

// How big a tile is, given its distance from where the effect starts (0 = there, 1 = farthest).
static float TileOpen(float distance, float t) {
    return PartProgress(t, std::clamp(distance, 0.f, 1.f) * (1 - kPartSpan));
}

// The mosaic goes through blocks of 64, 32, 16, 8, 4, 2 and 1 pixels. Powers of two, so each level
// is the previous one averaged 2 x 2 and no level is a noisy resample of the full image.
static constexpr int kPixelLevels = 7;

// The edge of the mosaic blocks at `t`, in pixels: chunky first, one pixel at the end.
static int PixelBlock(float t) {
    const int level = std::min(kPixelLevels - 1, static_cast<int>(std::clamp(t, 0.f, 1.f) * kPixelLevels));
    return 1 << (kPixelLevels - 1 - level);
}

// ---------------------------------------------------------------------------
// Settling. Windows changes the wallpaper some time after being asked, so the overlay stays until
// the desktop under it shows the new one. A few small patches of the desktop are compared with
// the same patches of the decoded image.
// ---------------------------------------------------------------------------

// The mean colour of the (2 * radius + 1) square around (cx, cy), clamped to the image.
static void AverageColor(const Thumb& img, int cx, int cy, int radius, int& r, int& g, int& b) {
    cx = std::clamp(cx, 0, static_cast<int>(img.w) - 1);
    cy = std::clamp(cy, 0, static_cast<int>(img.h) - 1);
    const int x0 = std::max(0, cx - radius), x1 = std::min(static_cast<int>(img.w) - 1, cx + radius);
    const int y0 = std::max(0, cy - radius), y1 = std::min(static_cast<int>(img.h) - 1, cy + radius);
    long sum[3] = {0, 0, 0};
    long n = 0;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const BYTE* px = &img.bgra[(static_cast<size_t>(y) * img.w + x) * 4];
            sum[0] += px[2];  // the image is BGRA
            sum[1] += px[1];
            sum[2] += px[0];
            ++n;
        }
    }
    r = static_cast<int>((sum[0] + n / 2) / n);
    g = static_cast<int>((sum[1] + n / 2) / n);
    b = static_cast<int>((sum[2] + n / 2) / n);
}

static bool ColorsClose(int r1, int g1, int b1, int r2, int g2, int b2, int tolerance) {
    return std::abs(r1 - r2) <= tolerance && std::abs(g1 - g2) <= tolerance &&
           std::abs(b1 - b2) <= tolerance;
}

// Whether two readings of the same patches are the same picture: a wallpaper that Windows is still
// fading in changes between readings, a settled one does not.
static bool SamplesStable(const std::vector<int>& a, const std::vector<int>& b, int tolerance) {
    if (a.empty() || a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > tolerance) return false;
    }
    return true;
}

// Where the desktop is compared: a 4 x 3 grid, row by row, in monitor pixels.
static std::vector<Pt> ProbePoints(int mw, int mh) {
    std::vector<Pt> points;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            points.push_back(Pt{(col + 0.5f) * mw / 4.f, (row + 0.5f) * mh / 3.f});
        }
    }
    return points;
}

// The monitor that `target` overlaps the most, or -1 when it is on none of them. Used to find the
// monitor Windows knows by its own id from the rectangle the selector sits on.
static int PickMonitor(const std::vector<RECT>& monitors, const RECT& target) {
    int best = -1;
    long long bestArea = 0;
    for (size_t i = 0; i < monitors.size(); ++i) {
        const long l = std::max(monitors[i].left, target.left), r = std::min(monitors[i].right, target.right);
        const long t = std::max(monitors[i].top, target.top), b = std::min(monitors[i].bottom, target.bottom);
        const long long area = r > l && b > t ? static_cast<long long>(r - l) * (b - t) : 0;
        if (area > bestArea) {
            bestArea = area;
            best = static_cast<int>(i);
        }
    }
    return best;
}

// Which monitor the selector opens on. Unknown values keep the first behaviour, the mouse.
enum class MonitorSource { Mouse, Focus };

static MonitorSource ParseMonitorSource(const std::wstring& id) {
    return LowerKey(TrimAscii(id)) == L"focus" ? MonitorSource::Focus : MonitorSource::Mouse;
}

// ---------------------------------------------------------------------------
// Windows side: hotkey, UI thread and window.
// ---------------------------------------------------------------------------

#include <windhawk_utils.h>

#include <d2d1.h>
#include <dwrite.h>
#include <shobjidl.h>

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <set>
#include <thread>

extern "C" IMAGE_DOS_HEADER __ImageBase;

static constexpr wchar_t kWindowClass[] = L"WallpaperChangerWnd";
static constexpr int kHotkeyId = 0x5743;
static constexpr UINT WM_WC_SETTINGS = WM_APP + 1;

struct Settings {
    std::wstring root;
    std::wstring style;       // an id from kStyles; unknown values fall back to the first one
    std::wstring transition;  // an id from kTransitions; unknown values fall back to Expand
    std::wstring monitorSource;      // "mouse" or "focus": the monitor the selector opens on
    bool currentMonitorOnly = false;  // false: every monitor gets the wallpaper and the effect
    UINT hotkeyMods = 0;
    UINT hotkeyVk = 0;  // 0 when the hotkey setting is invalid
};

// Decodes thumbnails on one worker thread and turns them into Direct2D bitmaps
// on the UI thread. Request/TryGet/Drain/Retain/Clear are UI-thread only.
class ThumbCache {
public:
    static constexpr UINT kShortSide = 480;

    void Start(HWND notify, UINT notifyMsg) {
        notify_ = notify;
        msg_ = notifyMsg;
        stop_ = false;
        thread_ = std::thread(&ThumbCache::Worker, this);
    }

    // Stops the worker and waits for it. Safe to call twice.
    void Join() {
        {
            std::lock_guard<std::mutex> lock(m_);
            stop_ = true;
        }
        cv_.notify_all();
        if (thread_.joinable()) thread_.join();
    }

    // Newest request first, so the cards on screen load before the prefetched ones.
    void Request(const std::wstring& path) {
        if (bitmaps_.count(path)) return;
        {
            std::lock_guard<std::mutex> lock(m_);
            if (!requested_.insert(path).second) return;
            queue_.push_front(path);
        }
        cv_.notify_one();
    }

    // True when the result is known; `out` is null if the image could not be decoded.
    bool TryGet(const std::wstring& path, ComPtr<ID2D1Bitmap>& out) const {
        auto it = bitmaps_.find(path);
        if (it == bitmaps_.end()) return false;
        out = it->second;
        return true;
    }

    // Turns finished decodes into bitmaps; returns true if anything new arrived.
    bool Drain(ID2D1RenderTarget* rt) {
        std::vector<Done> batch;
        {
            std::lock_guard<std::mutex> lock(m_);
            for (Done& d : done_) {
                if (requested_.count(d.path)) batch.push_back(std::move(d));
            }
            done_.clear();
        }
        for (Done& d : batch) {
            ComPtr<ID2D1Bitmap> bitmap;
            if (d.ok) {
                D2D1_BITMAP_PROPERTIES props{{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
                                             96.f, 96.f};
                rt->CreateBitmap(D2D1::SizeU(d.thumb.w, d.thumb.h), d.thumb.bgra.data(),
                                 d.thumb.w * 4, props, &bitmap);
            }
            bitmaps_[d.path] = bitmap;  // stays null for a failed decode: drawn as a placeholder
        }
        return !batch.empty();
    }

    // Drops everything that is not in `keep`, queued, finished or decoded.
    void Retain(const std::set<std::wstring>& keep) {
        for (auto it = bitmaps_.begin(); it != bitmaps_.end();) {
            it = keep.count(it->first) ? std::next(it) : bitmaps_.erase(it);
        }
        std::lock_guard<std::mutex> lock(m_);
        std::erase_if(queue_, [&](const std::wstring& p) { return !keep.count(p); });
        std::erase_if(requested_, [&](const std::wstring& p) { return !keep.count(p); });
    }

    void Clear() { Retain({}); }

private:
    struct Done {
        std::wstring path;
        bool ok = false;
        Thumb thumb;
    };

    void Worker() {
        // Background mode lowers CPU and I/O priority so decoding never competes with the shell.
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
        const bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
        for (;;) {
            std::wstring path;
            {
                std::unique_lock<std::mutex> lock(m_);
                cv_.wait(lock, [&] { return stop_ || !queue_.empty(); });
                if (stop_) break;
                path = std::move(queue_.front());
                queue_.pop_front();
            }
            Done d;
            d.path = path;
            try {
                d.ok = DecodeThumbnail(path, kShortSide, d.thumb);
            } catch (...) {  // e.g. out of memory on a huge image: a placeholder, never a dead shell
                d.ok = false;
            }
            {
                std::lock_guard<std::mutex> lock(m_);
                if (stop_) break;
                done_.push_back(std::move(d));
            }
            PostMessageW(notify_, msg_, 0, 0);
        }
        if (com) CoUninitialize();
    }

    HWND notify_ = nullptr;
    UINT msg_ = 0;
    std::thread thread_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;
    std::deque<std::wstring> queue_;
    std::set<std::wstring> requested_;
    std::vector<Done> done_;
    std::map<std::wstring, ComPtr<ID2D1Bitmap>> bitmaps_;  // UI thread only
};

static constexpr UINT WM_WC_THUMB = WM_APP + 2;
static constexpr UINT WM_WC_SCAN = WM_APP + 3;
static constexpr UINT WM_WC_HIDE = WM_APP + 4;  // the transition overlay is up: hide the selector
static constexpr UINT_PTR kAnimTimer = 1;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static std::atomic<HWND> g_hwnd{nullptr};

static HINSTANCE ModuleInstance() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

static std::wstring ReadStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

static std::wstring PicturesFolder() {
    PWSTR path = nullptr;
    std::wstring out;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, nullptr, &path))) out = path;
    CoTaskMemFree(path);
    return out;
}

static Settings LoadSettings() {
    Settings s;
    s.root = NormalizeRoot(ReadStringSetting(L"root"));
    if (s.root.empty()) s.root = PicturesFolder();
    s.style = TrimAscii(ReadStringSetting(L"style"));
    s.transition = TrimAscii(ReadStringSetting(L"transition"));
    s.monitorSource = TrimAscii(ReadStringSetting(L"monitorSource"));
    s.currentMonitorOnly = Wh_GetIntSetting(L"currentMonitorOnly") != 0;
    UINT mods = 0, vk = 0;
    if (ParseHotkey(ReadStringSetting(L"hotkey"), mods, vk)) {
        s.hotkeyMods = mods;
        s.hotkeyVk = vk;
    }
    return s;
}

static void RegisterHotkeyFromSettings(HWND hwnd) {
    UnregisterHotKey(hwnd, kHotkeyId);
    Settings s = LoadSettings();
    if (!s.hotkeyVk) {
        Wh_Log(L"Invalid hotkey setting, the selector has no hotkey");
        return;
    }
    if (!RegisterHotKey(hwnd, kHotkeyId, s.hotkeyMods, s.hotkeyVk)) {
        Wh_Log(L"RegisterHotKey failed: %lu", GetLastError());
    }
}

// ---------------------------------------------------------------------------
// Folder scan worker. The result is parked in a slot instead of being posted
// by pointer, so a window destroyed before it reads it cannot leak it.
// ---------------------------------------------------------------------------

// A joinable std::thread destroyed at process exit calls std::terminate, and the unload code does
// not run then, so these are never destroyed (see Windhawk's "Global objects and process shutdown").
[[clang::no_destroy]] static std::optional<std::thread> g_scanThread;
static std::atomic<bool> g_scanCancel{false};
static std::mutex g_scanMutex;
static std::optional<std::vector<Collection>> g_scanResult;

static void StopScan() {
    g_scanCancel = true;
    if (g_scanThread && g_scanThread->joinable()) g_scanThread->join();
    g_scanThread.reset();
}

static void StartScan(HWND hwnd, std::wstring root) {
    StopScan();
    g_scanCancel = false;
    g_scanThread.emplace([hwnd, root = std::move(root)] {
        std::vector<Collection> library;
        try {
            library = ScanLibrary(root, g_scanCancel);
        } catch (...) {  // an escaping exception would end the host process: show an empty library instead
            Wh_Log(L"Scanning the wallpapers folder failed");
            library.push_back(Collection{L"All", {}});
        }
        if (g_scanCancel) return;
        {
            std::lock_guard<std::mutex> lock(g_scanMutex);
            g_scanResult = std::move(library);
        }
        PostMessageW(hwnd, WM_WC_SCAN, 0, 0);
    });
}

// ---------------------------------------------------------------------------
// Applying a wallpaper. Runs on its own short-lived thread so the UI thread
// never waits on the shell's COM server.
// ---------------------------------------------------------------------------

[[clang::no_destroy]] static std::optional<std::thread> g_applyThread;  // see g_scanThread

// The id Windows gives the monitor that sits at `target`, or empty when none matches.
static std::wstring FindMonitorId(IDesktopWallpaper* wallpaper, const RECT& target) {
    UINT count = 0;
    if (FAILED(wallpaper->GetMonitorDevicePathCount(&count))) return L"";
    std::vector<std::wstring> ids;
    std::vector<RECT> rects;
    for (UINT i = 0; i < count; ++i) {
        PWSTR id = nullptr;
        RECT rect{};  // stays empty for a monitor that is not attached: it matches nothing
        if (SUCCEEDED(wallpaper->GetMonitorDevicePathAt(i, &id)) && id) {
            wallpaper->GetMonitorRECT(id, &rect);
            ids.push_back(id);
        } else {
            ids.push_back(L"");
        }
        CoTaskMemFree(id);
        rects.push_back(rect);
    }
    const int best = PickMonitor(rects, target);
    return best < 0 ? L"" : ids[best];
}

// `only`: the monitor to change; without it every monitor changes.
static bool ApplyWallpaper(const std::wstring& path, const std::optional<RECT>& only = std::nullopt) {
    // The timings below are kept on purpose: they tell a slow shell apart from a slow mod.
    const ULONGLONG t0 = GetTickCount64();
    Wh_Log(L"apply: start (%zu chars)", path.size());
    ComPtr<IDesktopWallpaper> wallpaper;
    HRESULT hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_LOCAL_SERVER,
                                  IID_PPV_ARGS(&wallpaper));
    const ULONGLONG t1 = GetTickCount64();
    Wh_Log(L"apply: DesktopWallpaper created in %lu ms (0x%08X)", static_cast<unsigned long>(t1 - t0),
           static_cast<unsigned>(hr));
    if (SUCCEEDED(hr)) {
        std::wstring monitorId;
        if (only) {
            monitorId = FindMonitorId(wallpaper.Get(), *only);
            // shortcut: a monitor Windows cannot match to the selector's changes them all, and says so.
            if (monitorId.empty()) Wh_Log(L"apply: no monitor matches the selector's, changing all of them");
        }
        hr = wallpaper->SetWallpaper(monitorId.empty() ? nullptr : monitorId.c_str(), path.c_str());
    }
    Wh_Log(L"apply: SetWallpaper returned in %lu ms (0x%08X)",
           static_cast<unsigned long>(GetTickCount64() - t1), static_cast<unsigned>(hr));
    return SUCCEEDED(hr);
}

// ---------------------------------------------------------------------------
// Transition overlay. A click-through window just above the desktop plays the effect with the new
// wallpaper while the real one is set underneath it, then fades away. It all runs on the apply
// thread; the UI thread only learns when to hide the selector.
// ---------------------------------------------------------------------------

struct TransitionJob {
    Transition kind = Transition::None;
    RECT monitor{};  // the monitor the selector was on, in physical pixels
    RectF from{};    // the card the wallpaper grows out of, relative to that monitor
};

// The wallpaper as Windows will draw it on the monitor: what the overlay shows once an effect is over.
struct Scene {
    ComPtr<ID2D1Bitmap> image;
    RectF place;  // where the image lands on the monitor
    D2D1_COLOR_F background;
    float w = 0.f, h = 0.f;  // the monitor
    std::vector<ComPtr<ID2D1Bitmap>> mips;  // Pixelate only: the scene halved level by level
};

static void DrawScene(ID2D1RenderTarget* rt, const Scene& s) {
    ComPtr<ID2D1SolidColorBrush> brush;
    if (SUCCEEDED(rt->CreateSolidColorBrush(s.background, &brush))) {
        rt->FillRectangle(D2D1::RectF(0.f, 0.f, s.w, s.h), brush.Get());
    }
    rt->DrawBitmap(s.image.Get(), D2D1::RectF(s.place.l, s.place.t, s.place.r, s.place.b));
}

static bool BuildScene(ID2D1RenderTarget* rt, const Thumb& img, const RectF& place,
                       D2D1_COLOR_F background, float w, float h, bool withMips, Scene& out) {
    const D2D1_BITMAP_PROPERTIES props{{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
                                       96.f, 96.f};
    if (FAILED(rt->CreateBitmap(D2D1::SizeU(img.w, img.h), img.bgra.data(), img.w * 4, props,
                                &out.image))) {
        return false;
    }
    out.place = place;
    out.background = background;
    out.w = w;
    out.h = h;
    if (!withMips) return true;

    // Level k is the scene averaged in blocks of 2^k pixels: level 0 is the scene itself and each
    // next one is the previous halved.
    auto makeTarget = [&](UINT pw, UINT ph, ComPtr<ID2D1BitmapRenderTarget>& target) {
        const D2D1_SIZE_U px = D2D1::SizeU(pw, ph);
        return SUCCEEDED(rt->CreateCompatibleRenderTarget(
            nullptr, &px, nullptr, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &target));
    };
    UINT lw = static_cast<UINT>(std::lround(w)), lh = static_cast<UINT>(std::lround(h));
    for (int k = 0; k < kPixelLevels; ++k) {
        ComPtr<ID2D1BitmapRenderTarget> target;
        ComPtr<ID2D1Bitmap> level;
        if (!makeTarget(lw, lh, target)) return false;
        target->BeginDraw();
        target->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
        if (k == 0) DrawScene(target.Get(), out);
        else target->DrawBitmap(out.mips[k - 1].Get(), D2D1::RectF(0.f, 0.f, static_cast<float>(lw), static_cast<float>(lh)));
        if (FAILED(target->EndDraw()) || FAILED(target->GetBitmap(&level))) return false;
        out.mips.push_back(level);
        lw = std::max<UINT>(1, (lw + 1) / 2);
        lh = std::max<UINT>(1, (lh + 1) / 2);
    }
    return true;
}

// A path made of rectangles that do not overlap, used to mask the scene.
static ComPtr<ID2D1PathGeometry> RectsGeometry(ID2D1Factory* factory,
                                               const std::vector<D2D1_RECT_F>& rects) {
    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(factory->CreatePathGeometry(&path)) || FAILED(path->Open(&sink))) return nullptr;
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    for (const D2D1_RECT_F& r : rects) {
        sink->BeginFigure(D2D1::Point2F(r.left, r.top), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(r.right, r.top));
        sink->AddLine(D2D1::Point2F(r.right, r.bottom));
        sink->AddLine(D2D1::Point2F(r.left, r.bottom));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }
    return SUCCEEDED(sink->Close()) ? path : nullptr;
}

// Draws the scene only where `mask` is. `sceneTransform` moves the scene but not the mask.
static void DrawMasked(ID2D1RenderTarget* rt, const Scene& s, ID2D1Geometry* mask,
                       const D2D1_MATRIX_3X2_F& sceneTransform) {
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), mask), nullptr);
    rt->SetTransform(sceneTransform);
    DrawScene(rt, s);
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->PopLayer();
}

// One frame of the effect at progress `t`, drawn over a transparent target the size of the monitor.
static void DrawTransitionFrame(ID2D1Factory* factory, ID2D1RenderTarget* rt, const Scene& s,
                                const TransitionJob& job, float t) {
    if (t >= 1.f) return DrawScene(rt, s);  // exact, with no mask seams
    const float W = s.w, H = s.h;
    const D2D1_MATRIX_3X2_F identity = D2D1::Matrix3x2F::Identity();

    switch (job.kind) {
        case Transition::Expand: {
            // The card grows to the monitor and its corners straighten. The wallpaper inside is
            // scaled to cover the card, so it never stretches.
            const float e = EaseInOut(t);
            const RectF r = LerpRect(job.from, RectF{0.f, 0.f, W, H}, e);
            const float radius = 28.f * (1.f - e);
            const float k = std::max(r.w() / W, r.h() / H);
            ComPtr<ID2D1RoundedRectangleGeometry> mask;
            if (SUCCEEDED(factory->CreateRoundedRectangleGeometry(
                    D2D1::RoundedRect(D2D1::RectF(r.l, r.t, r.r, r.b), radius, radius), &mask))) {
                DrawMasked(rt, s, mask.Get(),
                           D2D1::Matrix3x2F::Scale(k, k) *
                               D2D1::Matrix3x2F::Translation((r.l + r.r) / 2 - W * k / 2,
                                                             (r.t + r.b) / 2 - H * k / 2));
            }
            break;
        }
        case Transition::Blinds: {
            // Horizontal slats open from their middle line, the top one first.
            const int n = std::max(6, static_cast<int>(std::lround(H / 90.f)));
            const float slat = H / n;
            std::vector<D2D1_RECT_F> bands;
            for (int i = 0; i < n; ++i) {
                const float open = BlindOpen(i, n, t) * slat, mid = (i + 0.5f) * slat;
                if (open > 0.5f) bands.push_back(D2D1::RectF(0.f, mid - open / 2, W, mid + open / 2));
            }
            if (auto mask = RectsGeometry(factory, bands)) DrawMasked(rt, s, mask.Get(), identity);
            break;
        }
        case Transition::Tiles: {
            // Tiles grow from their centres in a wave that starts at the card.
            const int cols = std::max(4, static_cast<int>(std::lround(W / 160.f)));
            const int rows = std::max(3, static_cast<int>(std::lround(H / 160.f)));
            const float tw = W / cols, th = H / rows;
            const float ox = (job.from.l + job.from.r) / 2, oy = (job.from.t + job.from.b) / 2;
            const float farthest = std::max({std::hypot(ox, oy), std::hypot(W - ox, oy),
                                             std::hypot(ox, H - oy), std::hypot(W - ox, H - oy)});
            std::vector<D2D1_RECT_F> tiles;
            for (int row = 0; row < rows; ++row) {
                for (int col = 0; col < cols; ++col) {
                    const float cx = (col + 0.5f) * tw, cy = (row + 0.5f) * th;
                    const float d = farthest > 0.f ? std::hypot(cx - ox, cy - oy) / farthest : 0.f;
                    const float grow = TileOpen(d, t);
                    if (grow > 0.01f) {
                        tiles.push_back(D2D1::RectF(cx - tw * grow / 2, cy - th * grow / 2,
                                                    cx + tw * grow / 2, cy + th * grow / 2));
                    }
                }
            }
            if (auto mask = RectsGeometry(factory, tiles)) DrawMasked(rt, s, mask.Get(), identity);
            break;
        }
        case Transition::Pixelate: {
            // The mosaic fades in coarse and sharpens until it is the wallpaper itself.
            const int block = PixelBlock(t);
            int level = 0;
            while ((1 << level) < block) ++level;
            if (level >= static_cast<int>(s.mips.size())) break;
            ID2D1Bitmap* mip = s.mips[level].Get();
            const D2D1_SIZE_F size = mip->GetSize();
            rt->DrawBitmap(mip, D2D1::RectF(0.f, 0.f, size.width * block, size.height * block),
                           std::min(1.f, t * 3.f), D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
            break;
        }
        default: break;
    }
}

// A transparent DIB the size of the monitor, drawn with Direct2D and pushed to a layered window.
class OverlayCanvas {
public:
    OverlayCanvas() = default;
    OverlayCanvas(const OverlayCanvas&) = delete;
    OverlayCanvas& operator=(const OverlayCanvas&) = delete;
    ~OverlayCanvas() {
        rt.Reset();
        if (dc_) {
            SelectObject(dc_, old_);
            DeleteDC(dc_);
        }
        if (dib_) DeleteObject(dib_);
    }

    bool Create(ID2D1Factory* factory, SIZE size) {
        size_ = size;
        const auto props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
        if (FAILED(factory->CreateDCRenderTarget(&props, &rt))) return false;
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = size.cx;
        bi.bmiHeader.biHeight = -size.cy;  // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        dc_ = CreateCompatibleDC(nullptr);
        void* bits = nullptr;  // required by the API, the pixels are only reached through the DC
        dib_ = dc_ ? CreateDIBSection(dc_, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
        if (!dib_) return false;
        old_ = SelectObject(dc_, dib_);
        return true;
    }

    template <class Fn>
    bool Draw(Fn&& fn) {
        const RECT bounds{0, 0, size_.cx, size_.cy};
        if (FAILED(rt->BindDC(dc_, &bounds))) return false;
        rt->BeginDraw();
        rt->SetTransform(D2D1::Matrix3x2F::Identity());
        rt->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
        fn();
        return SUCCEEDED(rt->EndDraw());
    }

    // Shows what was last drawn, with `alpha` over all of it.
    bool Present(HWND window, POINT position, BYTE alpha = 255) {
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        return UpdateLayeredWindow(window, nullptr, &position, &size_, dc_, &source, 0, &blend,
                                   ULW_ALPHA) != FALSE;
    }

    ComPtr<ID2D1DCRenderTarget> rt;

private:
    HDC dc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HGDIOBJ old_ = nullptr;
    SIZE size_{};
};

struct OverlayWindow {
    HWND hwnd = nullptr;
    ~OverlayWindow() {
        if (hwnd) DestroyWindow(hwnd);
    }
};

// Just above the desktop and under every other window, so the effect never covers an app. The
// desktop icons are under it for the moment it plays.
static void PlaceAboveDesktop(HWND window) {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND above = progman ? GetWindow(progman, GW_HWNDPREV) : nullptr;
    for (int i = 0; above && i < 1000 && (above == window || !IsWindowVisible(above)); ++i) {
        above = GetWindow(above, GW_HWNDPREV);
    }
    const bool topmost = above && (GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOPMOST);
    const HWND insertAfter = !progman ? HWND_BOTTOM : (above && !topmost ? above : HWND_TOP);
    wchar_t cls[64] = L"none";
    if (above) GetClassNameW(above, cls, 64);
    Wh_Log(L"transition: overlay goes under a '%ls' window (topmost=%d)", cls, topmost ? 1 : 0);
    SetWindowPos(window, insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static std::atomic<bool> g_applyCancel{false};  // an effect that is playing jumps to its end
static std::atomic<bool> g_applySuperseded{false};  // with it, a newer wallpaper follows: this one is not written

static void PumpMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

static void SleepUnlessCancelled(int ms) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (!g_applyCancel && std::chrono::steady_clock::now() < end) {
        PumpMessages();
        Sleep(8);
    }
}

// The pixel size of an image, read from its header without decoding it.
static bool ReadImageSize(const std::wstring& path, UINT& w, UINT& h) {
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    return SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(&factory))) &&
           SUCCEEDED(factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                        WICDecodeMetadataCacheOnDemand, &decoder)) &&
           SUCCEEDED(decoder->GetFrame(0, &frame)) && SUCCEEDED(frame->GetSize(&w, &h)) && w && h;
}

struct Probe {
    int x, y;     // the centre of a patch of the monitor
    int r, g, b;  // the colour the wallpaper has there
};

// Patches that sit fully inside the image or fully outside it (the background colour), so the
// colour expected on the desktop is certain.
static std::vector<Probe> MakeProbes(const Thumb& image, const RectF& place, COLORREF background,
                                     int mw, int mh) {
    constexpr float kReach = 3.f;  // the patch plus a margin for the scaling at the image edge
    std::vector<Probe> probes;
    for (const Pt& p : ProbePoints(mw, mh)) {
        const float l = p.x - kReach, r = p.x + kReach, t = p.y - kReach, b = p.y + kReach;
        const bool inside = l >= place.l && r <= place.r && t >= place.t && b <= place.b;
        const bool outside = r <= place.l || l >= place.r || b <= place.t || t >= place.b;
        Probe probe{static_cast<int>(p.x), static_cast<int>(p.y), GetRValue(background),
                    GetGValue(background), GetBValue(background)};
        if (inside) {
            AverageColor(image, static_cast<int>(p.x - place.l), static_cast<int>(p.y - place.t), 2,
                         probe.r, probe.g, probe.b);
        } else if (!outside) {
            continue;
        }
        probes.push_back(probe);
    }
    return probes;
}

// Whether the point is on the desktop: nothing but the desktop (or the effect's own overlay, which
// is under every app) is there. An app window or the taskbar over it shows its own pixels instead.
static bool OnDesktop(POINT p, HWND overlay) {
    HWND window = WindowFromPoint(p);
    HWND root = window ? GetAncestor(window, GA_ROOT) : nullptr;
    if (!root) return false;
    if (root == overlay) return true;
    wchar_t cls[32] = L"";
    GetClassNameW(root, cls, 32);
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

// Whether the desktop shows the wallpaper at the probes. The overlay must be excluded from
// capture, or this reads the overlay itself. Probes under another window are left out, and with
// fewer than four left `covered` is set: there is no desktop to wait for.
// `seen` gets what was read (r, g, b per patch, -1 for a left out one) and `close` how many match.
static bool DesktopShowsWallpaper(const std::vector<Probe>& probes, POINT origin, HWND overlay,
                                  std::vector<int>& seen, int& close, bool& covered) {
    constexpr int kSide = 5, kTolerance = 16;  // patch size, and the largest accepted channel difference
    const int n = static_cast<int>(probes.size());
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = n * kSide;
    bi.bmiHeader.biHeight = -kSide;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = mem ? CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    bool result = false;
    covered = false;
    if (dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        for (int i = 0; i < n; ++i) {
            BitBlt(mem, i * kSide, 0, kSide, kSide, screen, origin.x + probes[i].x - kSide / 2,
                   origin.y + probes[i].y - kSide / 2, SRCCOPY);
        }
        GdiFlush();
        const BYTE* px = static_cast<const BYTE*>(bits);
        close = 0;
        seen.clear();
        int shown = 0;  // the patches on the desktop
        for (int i = 0; i < n; ++i) {
            if (!OnDesktop(POINT{origin.x + probes[i].x, origin.y + probes[i].y}, overlay)) {
                seen.insert(seen.end(), {-1, -1, -1});
                continue;
            }
            ++shown;
            long sum[3] = {0, 0, 0};
            for (int y = 0; y < kSide; ++y) {
                for (int x = 0; x < kSide; ++x) {
                    const BYTE* p = &px[(static_cast<size_t>(y) * n * kSide + i * kSide + x) * 4];
                    sum[0] += p[2];
                    sum[1] += p[1];
                    sum[2] += p[0];
                }
            }
            const long cells = kSide * kSide;
            const int r = static_cast<int>(sum[0] / cells), g = static_cast<int>(sum[1] / cells),
                      b = static_cast<int>(sum[2] / cells);
            seen.insert(seen.end(), {r, g, b});
            if (ColorsClose(r, g, b, probes[i].r, probes[i].g, probes[i].b, kTolerance)) ++close;
        }
        covered = shown < 4;
        result = !covered && close * 4 >= shown * 3;  // three out of four patches
        SelectObject(mem, old);
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return result;
}

// One overlay: the monitor it covers, what it draws, and what the desktop must show under it.
struct Stage {
    TransitionJob job;
    int w = 0, h = 0;
    Scene scene;
    OverlayCanvas canvas;
    OverlayWindow overlay;
    std::vector<Probe> probes;
    bool shown = false, excluded = false, settled = false;
    int streak = 0, matching = 0;
    std::vector<int> last;  // the previous reading of the desktop
    POINT origin() const { return POINT{job.monitor.left, job.monitor.top}; }
};

// Decodes the wallpaper as `fit` places it on the stage's monitor and creates the hidden overlay.
static bool PrepareStage(Stage& st, ID2D1Factory* factory, const std::wstring& path, UINT iw, UINT ih,
                         Fit fit, COLORREF color) {
    st.w = st.job.monitor.right - st.job.monitor.left;
    st.h = st.job.monitor.bottom - st.job.monitor.top;
    if (st.w <= 0 || st.h <= 0) return false;
    const RectF place = PlaceImage(static_cast<float>(iw), static_cast<float>(ih), static_cast<float>(st.w),
                                   static_cast<float>(st.h), fit);
    const UINT pw = static_cast<UINT>(std::lround(place.w())), ph = static_cast<UINT>(std::lround(place.h()));
    // shortcut: a centred image far bigger than the monitor gets no effect instead of a huge decode.
    if (!pw || !ph || static_cast<UINT64>(pw) * ph > 4ull * st.w * st.h) {
        Wh_Log(L"transition: no effect on a monitor (image too large to place)");
        return false;
    }
    Thumb image;
    const ULONGLONG t0 = GetTickCount64();
    if (!DecodeThumbnail(path, 0, image, pw, ph)) {
        Wh_Log(L"transition: no effect on a monitor (decode failed)");
        return false;
    }
    Wh_Log(L"transition: decoded %ux%u in %lu ms", pw, ph, static_cast<unsigned long>(GetTickCount64() - t0));
    if (!st.canvas.Create(factory, SIZE{st.w, st.h})) {
        Wh_Log(L"transition: no effect on a monitor (Direct2D canvas failed)");
        return false;
    }
    const D2D1_COLOR_F background =
        D2D1::ColorF(GetRValue(color) / 255.f, GetGValue(color) / 255.f, GetBValue(color) / 255.f);
    if (!BuildScene(st.canvas.rt.Get(), image, place, background, static_cast<float>(st.w),
                    static_cast<float>(st.h), st.job.kind == Transition::Pixelate, st.scene)) {
        Wh_Log(L"transition: no effect on a monitor (scene build failed)");
        return false;
    }
    st.probes = MakeProbes(image, place, color, st.w, st.h);
    st.overlay.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                      L"STATIC", nullptr, WS_POPUP, st.job.monitor.left, st.job.monitor.top,
                                      st.w, st.h, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!st.overlay.hwnd) Wh_Log(L"transition: no effect on a monitor (overlay window failed)");
    return st.overlay.hwnd != nullptr;
}

// Plays the effect on every job's monitor at once and sets the wallpaper under it (`only`: just on
// that monitor). Returns false, having changed nothing, when there is no effect to play (a layout
// the effects cannot draw, an image it cannot decode, ...): the caller then sets the wallpaper
// plainly. A monitor that cannot play it is left out and just changes instantly.
// `notify` gets WM_WC_HIDE once the first overlay is up.
static bool PlayTransition(const std::vector<TransitionJob>& jobs, const std::wstring& path,
                           const std::optional<RECT>& only, HWND notify) {
    struct SelectorHider {
        HWND hwnd;
        bool sent = false;
        void Send() {
            if (!sent && hwnd) PostMessageW(hwnd, WM_WC_HIDE, 0, 0);
            sent = true;
        }
        ~SelectorHider() { Send(); }  // also when giving up, so the selector never stays behind
    } hide{notify};

    // The reason goes to the log: a skipped effect is otherwise silent.
    auto skip = [](const wchar_t* why) {
        Wh_Log(L"transition: no effect (%ls)", why);
        return false;
    };
    if (jobs.empty()) return skip(L"no monitor");

    ComPtr<IDesktopWallpaper> wallpaper;
    DESKTOP_WALLPAPER_POSITION position = DWPOS_FILL;
    COLORREF color = 0;
    if (FAILED(CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_LOCAL_SERVER,
                                IID_PPV_ARGS(&wallpaper))) ||
        FAILED(wallpaper->GetPosition(&position))) {
        return skip(L"IDesktopWallpaper unavailable");
    }
    wallpaper->GetBackgroundColor(&color);
    Fit fit;
    switch (position) {
        case DWPOS_FILL: fit = Fit::Fill; break;
        case DWPOS_FIT: fit = Fit::Fit; break;
        case DWPOS_STRETCH: fit = Fit::Stretch; break;
        case DWPOS_CENTER: fit = Fit::Center; break;
        default: return skip(L"tile or span layout");
    }

    UINT iw = 0, ih = 0;
    if (!ReadImageSize(path, iw, ih)) return skip(L"image header unreadable");

    ComPtr<ID2D1Factory> factory;
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory.GetAddressOf()))) {
        return skip(L"Direct2D factory failed");
    }
    std::vector<std::unique_ptr<Stage>> stages;
    for (const TransitionJob& job : jobs) {
        auto st = std::make_unique<Stage>();
        st->job = job;
        if (PrepareStage(*st, factory.Get(), path, iw, ih, fit, color)) stages.push_back(std::move(st));
    }
    if (stages.empty()) return skip(L"no overlay could be prepared");

    using Clock = std::chrono::steady_clock;
    const Clock::time_point start = Clock::now();
    const float duration = static_cast<float>(TransitionDuration(stages[0]->job.kind));
    bool intact = true;
    for (;;) {
        const float t = std::min(1.f, std::chrono::duration<float, std::milli>(Clock::now() - start).count() / duration);
        for (auto& st : stages) {
            if (!st->canvas.Draw([&] { DrawTransitionFrame(factory.Get(), st->canvas.rt.Get(), st->scene, st->job, t); }) ||
                !st->canvas.Present(st->overlay.hwnd, st->origin())) {
                intact = false;
                Wh_Log(L"transition: a frame failed (t=%.2f, error %lu)", t, GetLastError());
                break;
            }
            if (!st->shown) {
                ShowWindow(st->overlay.hwnd, SW_SHOWNOACTIVATE);
                PlaceAboveDesktop(st->overlay.hwnd);
                st->shown = true;
                hide.Send();
            }
        }
        if (!intact || t >= 1.f || g_applyCancel) break;
        PumpMessages();
        Sleep(10);
    }
    const bool anyShown = std::any_of(stages.begin(), stages.end(), [](const auto& st) { return st->shown; });
    if (!anyShown) return skip(L"first frame failed");
    Wh_Log(L"transition: played on %zu monitor(s) in %lld ms", stages.size(),
           static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count()));

    // Windows repaints the desktop some time after it is asked to, so the overlays stay until the
    // desktop under each shows the new wallpaper. To read that desktop an overlay is left out of
    // captures (it stays on screen), and it goes back in once the wallpaper is seen.
    constexpr DWORD kExcludeFromCapture = 0x11;  // WDA_EXCLUDEFROMCAPTURE, Windows 10 2004 and later
    for (auto& st : stages) st->excluded = SetWindowDisplayAffinity(st->overlay.hwnd, kExcludeFromCapture) != FALSE;
    if (!g_applySuperseded) ApplyWallpaper(path, only);
    if (intact && !g_applyCancel) {
        const Clock::time_point applied = Clock::now();
        auto since = [&] {
            return static_cast<long long>(
                std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - applied).count());
        };
        SleepUnlessCancelled(150);  // the exclusion takes a frame to reach the compositor
        bool seen = false;
        while (!g_applyCancel && since() < 2500) {
            bool all = true;
            for (auto& st : stages) {
                if (st->settled) continue;
                // shortcut: without the exclusion, or with too few patches to compare, a monitor waits
                // a fixed time instead. Raise it if a slow machine still shows the old wallpaper.
                bool ready = since() >= 1500;
                if (st->excluded && st->probes.size() >= 4) {
                    std::vector<int> now;
                    bool covered = false;
                    const bool shows = DesktopShowsWallpaper(st->probes, st->origin(), st->overlay.hwnd, now,
                                                             st->matching, covered);
                    // a monitor under an app has no desktop to wait for; otherwise not still fading in
                    ready = covered || (shows && SamplesStable(st->last, now, 3));
                    st->last = std::move(now);
                }
                st->streak = ready ? st->streak + 1 : 0;
                st->settled = st->streak >= 3;  // three readings in a row, so a half-painted frame does not count
                all = all && st->settled;
            }
            if (all) {
                seen = true;
                break;
            }
            SleepUnlessCancelled(50);
        }
        Wh_Log(L"transition: the desktop %ls after %lld ms (%zu monitor(s), %d of %zu patches match on the first)",
               seen ? L"shows the wallpaper" : L"did not show it", since(), stages.size(), stages[0]->matching,
               stages[0]->probes.size());
        for (auto& st : stages) {
            if (st->excluded) SetWindowDisplayAffinity(st->overlay.hwnd, WDA_NONE);
        }
        SleepUnlessCancelled(250);  // margin: the desktop is fully painted before the overlay goes
        for (int alpha = 255; alpha > 0 && !g_applyCancel; alpha -= 24) {
            for (auto& st : stages) st->canvas.Present(st->overlay.hwnd, st->origin(), static_cast<BYTE>(alpha));
            Sleep(16);
        }
    }
    return true;
}

static void StopApply() {
    g_applyCancel = true;
    if (g_applyThread && g_applyThread->joinable()) g_applyThread->join();
    g_applyThread.reset();
    g_applyCancel = false;
}

// `only`: the one monitor to change (none: all of them). With `jobs` the effect plays first, one
// overlay per job, and the wallpaper is set under it.
static void ApplyWallpaperAsync(std::wstring path, std::optional<RECT> only,
                                std::vector<TransitionJob> jobs = {}, HWND notify = nullptr) {
    g_applySuperseded = true;  // a previous apply is cut short, and does not write its wallpaper
    StopApply();
    g_applySuperseded = false;
    g_applyThread.emplace([path = std::move(path), only, jobs = std::move(jobs), notify] {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);  // pixels, as the shell reports them
        const bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
        try {
            if (jobs.empty() || !PlayTransition(jobs, path, only, notify)) ApplyWallpaper(path, only);
        } catch (...) {
            Wh_Log(L"apply: failed with an exception");
            try {
                ApplyWallpaper(path, only);
            } catch (...) {
            }
        }
        if (com) CoUninitialize();
    });
}

// ---------------------------------------------------------------------------
// UI state. Owned and touched by the UI thread only.
// ---------------------------------------------------------------------------

class Style;

// What is being browsed: the collections (cards with a cover image) or the images of one
// collection. Only a style with a folder level ever shows Folders.
enum class Level { Wallpapers, Folders };

enum class Target { Close, Rescan, Prev, Next, Tab, Card };

struct HitRegion {
    Target target;
    int index;  // Tab: collection index. Card: item index. Otherwise unused.
    Quad shape;
};

struct HitMap {  // what was drawn last frame and can be clicked, in window DIPs
    std::vector<HitRegion> regions;  // front-most first
    float dragStep = 1.f;            // pixels of horizontal drag that move the selection by one item
};

struct Ui {
    ComPtr<ID2D1Factory> d2d;
    ComPtr<IDWriteFactory> dwrite;
    ComPtr<IDWriteTextFormat> fontUi, fontName, fontIndex, fontSymbol, fontHint, fontTiny, fontNum,
        fontBig, fontTitle;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1SolidColorBrush> brush;
    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    SIZE dibSize{};
    float scale = 1.f;  // DPI / 96

    const Style* style = nullptr;
    std::wstring root;
    std::vector<Collection> library;
    bool scanned = false;  // a scan result has arrived at least once
    Level level = Level::Wallpapers;
    int collection = 0;    // the open collection, or the last one opened while at Folders
    int selected = 0;      // index into the current level's items
    float position = 0.f;  // animated; follows `selected`

    bool pressed = false, dragMoved = false;
    float pressX = 0.f, pressPos = 0.f;
    int pressCard = -1;
    int wheelAcc = 0;
    HitMap hit;
    std::wstring transition;  // the `transition` setting as it was when the selector opened
    bool currentMonitorOnly = false;  // likewise

    ThumbCache thumbs;  // last member: destroyed first, before the render target

    ~Ui() {
        if (memDc) DeleteDC(memDc);
        if (dib) DeleteObject(dib);
    }
};

static Ui* g_ui = nullptr;  // created and destroyed by the UI thread

// The items of the current level. Callers keep `i` within [0, ItemCount).
static bool HasImages(const Ui& ui) {
    return !ui.library.empty() && !ui.library[0].images.empty();  // library[0] is "All"
}

static int ItemCount(const Ui& ui) {
    if (ui.level == Level::Folders) return HasImages(ui) ? static_cast<int>(ui.library.size()) : 0;
    return ui.collection < static_cast<int>(ui.library.size())
               ? static_cast<int>(ui.library[ui.collection].images.size())
               : 0;
}

static const std::wstring& ItemLabel(const Ui& ui, int i) {
    return ui.level == Level::Folders ? ui.library[i].name
                                      : ui.library[ui.collection].images[i].name;
}

// The image a card shows: a folder shows its first image.
static const std::wstring& ItemThumb(const Ui& ui, int i) {
    return ui.level == Level::Folders ? ui.library[i].images.front().path
                                      : ui.library[ui.collection].images[i].path;
}

// Secondary text of a card: how many wallpapers a folder holds. Empty for an image.
static std::wstring ItemDetail(const Ui& ui, int i) {
    return ui.level == Level::Folders ? std::to_wstring(ui.library[i].images.size()) + L" WALLS"
                                      : std::wstring();
}

static const wchar_t* ItemUnit(const Ui& ui) {
    return ui.level == Level::Folders ? L"FOLDERS" : L"WALLS";
}

static std::wstring OpenCollectionName(const Ui& ui) {
    return ui.collection < static_cast<int>(ui.library.size()) ? ui.library[ui.collection].name
                                                               : std::wstring();
}

// Switches what is browsed and puts the selection where it belongs: on the open folder when
// going up, on the first image when going down.
static void SetLevel(Ui& ui, Level level) {
    ui.level = level;
    const int last = std::max(ItemCount(ui) - 1, 0);
    ui.selected = std::clamp(level == Level::Folders ? ui.collection : 0, 0, last);
    ui.position = static_cast<float>(ui.selected);
}

// ---------------------------------------------------------------------------
// Drawing kit shared by every style.
// ---------------------------------------------------------------------------

static D2D1_COLOR_F Col(float r, float g, float b, float a = 1.f) {
    return D2D1::ColorF(r, g, b, a);
}

static D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float a) {
    c.a *= a;
    return c;
}

static bool InitUi(Ui& ui) {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, ui.d2d.GetAddressOf()))) {
        return false;
    }
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(ui.dwrite.GetAddressOf())))) {
        return false;
    }

    struct FontSpec {
        ComPtr<IDWriteTextFormat> Ui::*font;
        PCWSTR family;
        DWRITE_FONT_WEIGHT weight;
        float size;
        DWRITE_TEXT_ALIGNMENT align;
    };
    const FontSpec specs[] = {
        {&Ui::fontUi, L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD, 13.f, DWRITE_TEXT_ALIGNMENT_LEADING},
        {&Ui::fontName, L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD, 12.f, DWRITE_TEXT_ALIGNMENT_LEADING},
        {&Ui::fontIndex, L"Segoe UI", DWRITE_FONT_WEIGHT_NORMAL, 11.f, DWRITE_TEXT_ALIGNMENT_TRAILING},
        {&Ui::fontSymbol, L"Segoe UI Symbol", DWRITE_FONT_WEIGHT_NORMAL, 16.f, DWRITE_TEXT_ALIGNMENT_CENTER},
        {&Ui::fontHint, L"Segoe UI", DWRITE_FONT_WEIGHT_NORMAL, 12.f, DWRITE_TEXT_ALIGNMENT_CENTER},
        {&Ui::fontTiny, L"Segoe UI", DWRITE_FONT_WEIGHT_BOLD, 10.f, DWRITE_TEXT_ALIGNMENT_CENTER},
        {&Ui::fontNum, L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD, 24.f, DWRITE_TEXT_ALIGNMENT_TRAILING},
        {&Ui::fontBig, L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD, 44.f, DWRITE_TEXT_ALIGNMENT_TRAILING},
        {&Ui::fontTitle, L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD, 22.f, DWRITE_TEXT_ALIGNMENT_LEADING},
    };
    for (const FontSpec& spec : specs) {
        ComPtr<IDWriteTextFormat>& out = ui.*spec.font;
        if (FAILED(ui.dwrite->CreateTextFormat(spec.family, nullptr, spec.weight,
                                               DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                               spec.size, L"", &out))) {
            return false;
        }
        out->SetTextAlignment(spec.align);
        out->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        out->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    ComPtr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(ui.dwrite->CreateEllipsisTrimmingSign(ui.fontName.Get(), &ellipsis))) {
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        for (IDWriteTextFormat* font :
             {ui.fontName.Get(), ui.fontUi.Get(), ui.fontHint.Get(), ui.fontTitle.Get()}) {
            font->SetTrimming(&trimming, ellipsis.Get());
        }
    }

    ui.memDc = CreateCompatibleDC(nullptr);
    return ui.memDc != nullptr;
}

static bool EnsureTarget(Ui& ui) {
    if (ui.rt) return true;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.f, 0.f,
        D2D1_RENDER_TARGET_USAGE_NONE);
    if (FAILED(ui.d2d->CreateDCRenderTarget(&props, &ui.rt))) return false;
    // Everything is authored in DIPs and scaled by ui.scale; the default (system) DPI would scale twice.
    ui.rt->SetDpi(96.f, 96.f);
    // Grayscale: ClearType fringes look wrong on a per-pixel alpha surface.
    ui.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    if (FAILED(ui.rt->CreateSolidColorBrush(Col(1, 1, 1), &ui.brush))) {
        ui.rt.Reset();
        return false;
    }
    return true;
}

static bool EnsureSurface(Ui& ui, int w, int h) {
    if (ui.dib && ui.dibSize.cx == w && ui.dibSize.cy == h) return true;
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(ui.memDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp) return false;
    SelectObject(ui.memDc, bmp);
    if (ui.dib) DeleteObject(ui.dib);
    ui.dib = bmp;
    ui.dibSize = {w, h};
    return true;
}

static void DrawTextIn(Ui& ui, const std::wstring& text, IDWriteTextFormat* font,
                       D2D1_RECT_F rect, D2D1_COLOR_F color) {
    ui.brush->SetColor(color);
    ui.rt->DrawText(text.c_str(), static_cast<UINT32>(text.size()), font, rect, ui.brush.Get());
}

static float TextWidth(Ui& ui, const std::wstring& text, IDWriteTextFormat* font) {
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(ui.dwrite->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()), font,
                                           4096.f, 64.f, &layout))) {
        return 0.f;
    }
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    return m.widthIncludingTrailingWhitespace;
}

static void FillRound(Ui& ui, D2D1_RECT_F r, float radius, D2D1_COLOR_F color) {
    ui.brush->SetColor(color);
    ui.rt->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), ui.brush.Get());
}

static void StrokeRound(Ui& ui, D2D1_RECT_F r, float radius, D2D1_COLOR_F color) {
    ui.brush->SetColor(color);
    ui.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, radius, radius), ui.brush.Get(), 1.f);
}

static void Line(Ui& ui, float x0, float y0, float x1, float y1, D2D1_COLOR_F color, float w) {
    ui.brush->SetColor(color);
    ui.rt->DrawLine(D2D1::Point2F(x0, y0), D2D1::Point2F(x1, y1), ui.brush.Get(), w);
}

static const D2D1_COLOR_F kAccent = {0.12f, 0.64f, 0.95f, 1.f};

static std::wstring TwoDigits(int n) {
    wchar_t buf[16];
    swprintf(buf, 16, L"%02d", n);
    return buf;
}

static std::wstring ThreeDigits(int n) {
    wchar_t buf[16];
    swprintf(buf, 16, L"%03d", n);
    return buf;
}

static Quad RectQuad(const D2D1_RECT_F& r) {
    return {Pt{r.left, r.top}, Pt{r.right, r.top}, Pt{r.right, r.bottom}, Pt{r.left, r.bottom}};
}

static void AddRegion(Ui& ui, Target target, int index, const Quad& shape) {
    ui.hit.regions.push_back({target, index, shape});
}

// Null when Direct2D refuses; callers skip what they cannot draw.
static ComPtr<ID2D1PathGeometry> QuadGeometry(Ui& ui, const Quad& q) {
    ComPtr<ID2D1PathGeometry> geometry;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(ui.d2d->CreatePathGeometry(&geometry)) || FAILED(geometry->Open(&sink))) {
        return nullptr;
    }
    sink->BeginFigure(D2D1::Point2F(q[0].x, q[0].y), D2D1_FIGURE_BEGIN_FILLED);
    for (size_t i = 1; i < q.size(); ++i) sink->AddLine(D2D1::Point2F(q[i].x, q[i].y));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    return SUCCEEDED(sink->Close()) ? geometry : nullptr;
}

static ComPtr<ID2D1RoundedRectangleGeometry> RoundedGeometry(Ui& ui, const D2D1_RECT_F& r,
                                                             float radius) {
    ComPtr<ID2D1RoundedRectangleGeometry> geometry;
    ui.d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &geometry);
    return geometry;  // null when Direct2D refuses
}

// Darkens the lower part of `shape` (top to bottom of `bounds`) so text on it stays readable.
static void FillBottomScrim(Ui& ui, ID2D1Geometry* shape, const D2D1_RECT_F& bounds,
                            float strength) {
    // The middle stop eases the start of the fade, a straight ramp shows as a visible edge.
    const D2D1_GRADIENT_STOP stops[] = {{0.f, Col(0, 0, 0, 0.f)},
                                        {0.45f, Col(0, 0, 0, 0.f)},
                                        {0.75f, Col(0, 0, 0, strength * 0.3f)},
                                        {1.f, Col(0, 0, 0, strength)}};
    ComPtr<ID2D1GradientStopCollection> collection;
    ComPtr<ID2D1LinearGradientBrush> brush;
    if (FAILED(ui.rt->CreateGradientStopCollection(stops, 4, &collection)) ||
        FAILED(ui.rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.f, bounds.top),
                                                D2D1::Point2F(0.f, bounds.bottom)),
            collection.Get(), &brush))) {
        return;
    }
    ui.rt->FillGeometry(shape, brush.Get());
}

static void FillQuad(Ui& ui, const Quad& q, D2D1_COLOR_F color) {
    if (ComPtr<ID2D1PathGeometry> geometry = QuadGeometry(ui, q)) {
        ui.brush->SetColor(color);
        ui.rt->FillGeometry(geometry.Get(), ui.brush.Get());
    }
}

// Fills `shape` with the thumbnail of `path`, scaled to cover `bounds`. While the thumbnail is
// still loading the shape gets a plain placeholder; a failed decode says so.
static void PaintThumb(Ui& ui, const std::wstring& path, ID2D1Geometry* shape, D2D1_RECT_F bounds,
                       float alpha) {
    ComPtr<ID2D1Bitmap> bitmap;
    const bool known = ui.thumbs.TryGet(path, bitmap);
    if (!bitmap) {
        ui.brush->SetColor(Col(0.16f, 0.16f, 0.21f, alpha));
        ui.rt->FillGeometry(shape, ui.brush.Get());
        if (known) {  // decode failed
            DrawTextIn(ui, L"No preview", ui.fontUi.Get(), bounds, Col(1, 1, 1, 0.45f * alpha));
        }
        return;
    }
    const D2D1_SIZE_F bs = bitmap->GetSize();
    const float w = bounds.right - bounds.left, h = bounds.bottom - bounds.top;
    const float k = std::max(w / bs.width, h / bs.height);
    const float cx = (bounds.left + bounds.right) / 2, cy = (bounds.top + bounds.bottom) / 2;
    ComPtr<ID2D1BitmapBrush> bb;
    if (SUCCEEDED(ui.rt->CreateBitmapBrush(
            bitmap.Get(), D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP),
            D2D1::BrushProperties(alpha, D2D1::Matrix3x2F::Scale(k, k) *
                                             D2D1::Matrix3x2F::Translation(
                                                 cx - bs.width * k / 2, cy - bs.height * k / 2)),
            &bb))) {
        ui.rt->FillGeometry(shape, bb.Get());
    }
}

static void DrawEmptyState(Ui& ui, float W, float top, float bottom) {
    const std::wstring msg = ui.scanned ? L"No wallpapers found in " + ui.root
                                        : std::wstring(L"Looking for wallpapers…");
    DrawTextIn(ui, msg, ui.fontHint.Get(), D2D1::RectF(0, top, W, bottom), Col(1, 1, 1, 0.55f));
}

// ---------------------------------------------------------------------------
// Styles. A style decides how the items of the current level are laid out and drawn.
//
// To add one: derive from Style, draw with the kit above, register what can be clicked with
// AddRegion (front-most first), set hit.dragStep, and add a line to kStyles plus an option to
// the `style` setting. Selection, navigation, input, thumbnails and applying are shared.
// ---------------------------------------------------------------------------

struct StyleSize {
    float w, h;  // DIPs
};

class Style {
public:
    virtual ~Style() = default;
    virtual StyleSize WindowSize() const = 0;
    // True when the style browses folders as cards first and opens one with Enter; false when
    // it shows the images of a collection and switches collection some other way.
    virtual bool HasFolderLevel() const = 0;
    // Draws one frame in window DIPs (the render target is already scaled) and fills ui.hit.
    virtual void Draw(Ui& ui, float W, float H) const = 0;
};

// A fan of overlapping cards, the centre one in front, with the collections as tabs.
class CarouselStyle final : public Style {
public:
    StyleSize WindowSize() const override { return {1100.f, 440.f}; }
    bool HasFolderLevel() const override { return false; }

    void Draw(Ui& ui, float W, float H) const override {
        const int n = ItemCount(ui);
        DrawHint(ui, W, n);
        const float barY = DrawBottomBar(ui, W, H, n);

        const float stageTop = 48.f, stageBottom = barY - 6.f;
        if (n == 0) return DrawEmptyState(ui, W, stageTop, stageBottom);

        const float stageH = stageBottom - stageTop;
        const float cardW = stageH * kCardWPerStageH, cardH = stageH * kCardHPerStageH;
        ui.hit.dragStep = cardW * kCardSpacing;

        struct Visible {
            int index;
            CardPose pose;
        };
        std::vector<Visible> visible;
        const int lo = std::max(0, static_cast<int>(std::floor(ui.position)) - 3);
        const int hi = std::min(n - 1, static_cast<int>(std::ceil(ui.position)) + 3);
        for (int i = lo; i <= hi; ++i) {
            CardPose p = ComputePose(static_cast<float>(i) - ui.position, W, stageH);
            if (p.alpha > 0.f) visible.push_back({i, p});
        }
        std::sort(visible.begin(), visible.end(),
                  [](const Visible& a, const Visible& b) { return a.pose.z < b.pose.z; });
        for (const Visible& v : visible) {
            DrawCard(ui, v.index, v.pose, v.index == ui.selected, stageTop, cardW, cardH);
        }
        for (auto it = visible.rbegin(); it != visible.rend(); ++it) {
            AddRegion(ui, Target::Card, it->index, CardQuad(it->pose, cardW, cardH, stageTop));
        }
        ui.rt->SetTransform(D2D1::Matrix3x2F::Scale(ui.scale, ui.scale));
    }

private:
    static D2D1_COLOR_F Panel() { return Col(0.07f, 0.07f, 0.10f, 0.86f); }
    static D2D1_COLOR_F Border() { return Col(1, 1, 1, 0.10f); }
    static D2D1_COLOR_F Text() { return Col(1, 1, 1, 0.92f); }
    static D2D1_COLOR_F Dim() { return Col(1, 1, 1, 0.55f); }

    static void DrawHint(Ui& ui, float W, int n) {
        const std::wstring hint = L"Drag or Click · Wheel to Spin";
        const std::wstring name = n ? ItemLabel(ui, std::clamp(ui.selected, 0, n - 1)) : L"";
        const std::wstring coll = n ? OpenCollectionName(ui) : L"";
        // Long names are capped so the pill never outgrows the window; the font trims with "...".
        const float wName =
            name.empty() ? 0.f : std::min(TextWidth(ui, name, ui.fontUi.Get()) + 14.f, W * 0.35f);
        const float wColl =
            coll.empty() ? 0.f : std::min(TextWidth(ui, coll, ui.fontUi.Get()) + 14.f, W * 0.35f);
        const float wHint = TextWidth(ui, hint, ui.fontUi.Get());
        const float total = wName + wColl + wHint + 36.f;
        float x = (W - total) / 2;
        FillRound(ui, D2D1::RectF(x, 8, x + total, 40), 16.f, Panel());
        StrokeRound(ui, D2D1::RectF(x, 8, x + total, 40), 16.f, Border());
        x += 18;
        if (wName) {
            DrawTextIn(ui, name, ui.fontUi.Get(), D2D1::RectF(x, 8, x + wName, 40), Text());
            x += wName;
        }
        if (wColl) {
            DrawTextIn(ui, coll, ui.fontUi.Get(), D2D1::RectF(x, 8, x + wColl, 40), Dim());
            x += wColl;
        }
        DrawTextIn(ui, hint, ui.fontUi.Get(), D2D1::RectF(x, 8, x + wHint, 40), Dim());
    }

    // Collection tabs, previous/next, refresh and close. Returns the bar's top edge.
    static float DrawBottomBar(Ui& ui, float W, float H, int n) {
        const float barH = 44.f, barY = H - 8.f - barH;
        const std::wstring counter =
            std::to_wstring(n ? ui.selected + 1 : 0) + L"/" + std::to_wstring(n);
        const float counterW = TextWidth(ui, counter, ui.fontUi.Get()) + 16.f;
        const float fixedW = 14 + 8 + 1 + 12 + 28 + counterW + 28 + 12 + 1 + 8 + 32 + 8 + 32 + 14;

        std::vector<float> tabW(ui.library.size());
        float tabsW = 0;
        size_t fitted = 0;
        for (size_t i = 0; i < ui.library.size(); ++i) {
            tabW[i] = std::min(TextWidth(ui, ui.library[i].name, ui.fontUi.Get()) + 26.f, 180.f);
            // shortcut: tabs that do not fit the bar are not drawn, Tab still cycles them.
            if (fixedW + tabsW + tabW[i] + 4 > W - 24) break;
            tabsW += tabW[i] + 4;
            fitted = i + 1;
        }

        const float barW = fixedW + tabsW;
        float x = (W - barW) / 2;
        const D2D1_RECT_F bar = D2D1::RectF(x, barY, x + barW, barY + barH);
        FillRound(ui, bar, barH / 2, Panel());
        StrokeRound(ui, bar, barH / 2, Border());
        x += 14;

        for (size_t i = 0; i < fitted; ++i) {
            const D2D1_RECT_F r = D2D1::RectF(x, barY + 7, x + tabW[i], barY + barH - 7);
            const bool on = static_cast<int>(i) == ui.collection;
            if (on) FillRound(ui, r, 15.f, kAccent);
            DrawTextIn(ui, ui.library[i].name, ui.fontUi.Get(),
                       D2D1::RectF(r.left + 13, r.top, r.right - 13, r.bottom), on ? Text() : Dim());
            AddRegion(ui, Target::Tab, static_cast<int>(i), RectQuad(r));
            x += tabW[i] + 4;
        }
        x += 4;
        Line(ui, x, barY + 12, x, barY + barH - 12, Border(), 1.f);
        x += 13;

        const D2D1_COLOR_F nav = n ? Text() : Col(1, 1, 1, 0.25f);
        AddRegion(ui, Target::Prev, 0, RectQuad(D2D1::RectF(x, barY + 8, x + 28, barY + barH - 8)));
        {
            const float cx = x + 14, cy = barY + barH / 2;
            Line(ui, cx + 3, cy - 6, cx - 3, cy, nav, 2.f);
            Line(ui, cx - 3, cy, cx + 3, cy + 6, nav, 2.f);
        }
        x += 28;
        DrawTextIn(ui, counter, ui.fontUi.Get(), D2D1::RectF(x + 8, barY, x + counterW, barY + barH),
                   Dim());
        x += counterW;
        AddRegion(ui, Target::Next, 0, RectQuad(D2D1::RectF(x, barY + 8, x + 28, barY + barH - 8)));
        {
            const float cx = x + 14, cy = barY + barH / 2;
            Line(ui, cx - 3, cy - 6, cx + 3, cy, nav, 2.f);
            Line(ui, cx + 3, cy, cx - 3, cy + 6, nav, 2.f);
        }
        x += 28 + 12;
        Line(ui, x, barY + 12, x, barY + barH - 12, Border(), 1.f);
        x += 1 + 8;

        const D2D1_RECT_F refresh = D2D1::RectF(x, barY + 6, x + 32, barY + barH - 6);
        AddRegion(ui, Target::Rescan, 0, RectQuad(refresh));
        DrawTextIn(ui, L"↻", ui.fontSymbol.Get(), refresh, Text());
        x += 32 + 8;

        AddRegion(ui, Target::Close, 0, RectQuad(D2D1::RectF(x, barY + 6, x + 32, barY + barH - 6)));
        ui.brush->SetColor(kAccent);
        ui.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + 16, barY + barH / 2), 15.f, 15.f),
                           ui.brush.Get());
        Line(ui, x + 11, barY + barH / 2 - 5, x + 21, barY + barH / 2 + 5, Col(1, 1, 1), 2.f);
        Line(ui, x + 21, barY + barH / 2 - 5, x + 11, barY + barH / 2 + 5, Col(1, 1, 1), 2.f);
        return barY;
    }

    static void DrawCard(Ui& ui, int index, const CardPose& p, bool selected, float stageTop,
                         float cardW, float cardH) {
        const float s = ui.scale;
        ui.rt->SetTransform(D2D1::Matrix3x2F::Scale(p.scale, p.scale) *
                            D2D1::Matrix3x2F::Rotation(p.angleDeg) *
                            D2D1::Matrix3x2F::Translation(p.x, stageTop + p.y) *
                            D2D1::Matrix3x2F::Scale(s, s));

        const D2D1_RECT_F card = D2D1::RectF(-cardW / 2, -cardH / 2, cardW / 2, cardH / 2);
        const float radius = 14.f;

        FillRound(ui, D2D1::RectF(card.left, card.top + 6, card.right, card.bottom + 6), radius,
                  Col(0, 0, 0, 0.35f * p.alpha));  // cheap drop shadow

        if (auto shape = RoundedGeometry(ui, card, radius)) {
            PaintThumb(ui, ItemThumb(ui, index), shape.Get(), card, p.alpha);
        }
        StrokeRound(ui, card, radius,
                    selected ? WithAlpha(kAccent, p.alpha) : Col(1, 1, 1, 0.14f * p.alpha));

        // Name chip at the bottom of the card.
        const D2D1_RECT_F chip =
            D2D1::RectF(card.left + 8, card.bottom - 34, card.right - 8, card.bottom - 8);
        FillRound(ui, chip, 9.f, Col(0, 0, 0, 0.55f * p.alpha));
        DrawTextIn(ui, ItemLabel(ui, index), ui.fontName.Get(),
                   D2D1::RectF(chip.left + 10, chip.top, chip.right - 36, chip.bottom),
                   Col(1, 1, 1, 0.95f * p.alpha));
        DrawTextIn(ui, TwoDigits(index + 1), ui.fontIndex.Get(),
                   D2D1::RectF(chip.right - 34, chip.top, chip.right - 8, chip.bottom),
                   Col(1, 1, 1, 0.55f * p.alpha));

        if (selected) {  // check badge
            const float cx = card.right - 20, cy = card.top + 20;
            ui.brush->SetColor(WithAlpha(kAccent, p.alpha));
            ui.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 12.f, 12.f), ui.brush.Get());
            const D2D1_COLOR_F white = Col(1, 1, 1, p.alpha);
            Line(ui, cx - 5, cy, cx - 1.5f, cy + 4, white, 2.f);
            Line(ui, cx - 1.5f, cy + 4, cx + 5, cy - 4, white, 2.f);
        }
    }
};

// Slanted cards side by side, the centre one wide. Browses the folders first; Enter opens one.
class StripStyle final : public Style {
public:
    StyleSize WindowSize() const override { return {1400.f, 520.f}; }
    bool HasFolderLevel() const override { return true; }

    void Draw(Ui& ui, float W, float H) const override {
        const int n = ItemCount(ui);
        FillRound(ui, D2D1::RectF(0, 0, W, H), 22.f, Col(0.05f, 0.05f, 0.08f, 0.80f));
        StrokeRound(ui, D2D1::RectF(0.5f, 0.5f, W - 0.5f, H - 0.5f), 22.f, Col(1, 1, 1, 0.08f));
        DrawHeader(ui, W, n);

        const float stageTop = 84.f, stageBottom = H - 70.f;
        if (n == 0) return DrawEmptyState(ui, W, stageTop, stageBottom);
        DrawCards(ui, W, stageTop, stageBottom, n);
        DrawFooter(ui, W, H, n);
    }

private:
    static constexpr float kCaptionH = 30.f;

    static D2D1_COLOR_F Ink(float a = 1.f) { return Col(0.09f, 0.09f, 0.11f, a); }

    // Light panel: level title, the selected item and how many items there are.
    static void DrawHeader(Ui& ui, float W, int n) {
        const D2D1_RECT_F bar = D2D1::RectF(12, 12, W - 12, 68);
        FillRound(ui, bar, 14.f, Col(0.96f, 0.96f, 0.98f, 0.97f));

        const std::wstring title = ui.level == Level::Folders ? std::wstring(L"Categories")
                                                              : OpenCollectionName(ui);
        const float pillW = std::min(TextWidth(ui, title, ui.fontUi.Get()) + 36.f, 240.f);
        const D2D1_RECT_F pill =
            D2D1::RectF(bar.left + 10, bar.top + 8, bar.left + 10 + pillW, bar.bottom - 8);
        FillRound(ui, pill, 10.f, Ink());
        DrawTextIn(ui, title, ui.fontUi.Get(),
                   D2D1::RectF(pill.left + 18, pill.top, pill.right - 18, pill.bottom), Col(1, 1, 1));

        const float numW = 110.f, right = bar.right - 16;
        DrawTextIn(ui, std::to_wstring(n), ui.fontNum.Get(),
                   D2D1::RectF(right - numW, bar.top + 4, right, bar.top + 34), Ink());
        DrawTextIn(ui, ItemUnit(ui), ui.fontIndex.Get(),
                   D2D1::RectF(right - numW, bar.top + 34, right, bar.bottom - 4), Ink(0.55f));
        if (!n) return;

        const int sel = std::clamp(ui.selected, 0, n - 1);
        const std::wstring name = ItemLabel(ui, sel), detail = ItemDetail(ui, sel);
        float x = pill.right + 18;
        const float room = right - numW - 18 - x;
        const float nameW = std::min(TextWidth(ui, name, ui.fontUi.Get()) + 10.f, room);
        DrawTextIn(ui, name, ui.fontUi.Get(), D2D1::RectF(x, bar.top, x + nameW, bar.bottom),
                   Ink(0.85f));
        x += nameW;
        if (!detail.empty() && room - nameW > 60.f) {
            DrawTextIn(ui, L"·  " + detail, ui.fontUi.Get(),
                       D2D1::RectF(x, bar.top, x + room - nameW, bar.bottom), Ink(0.45f));
        }
    }

    static void DrawCards(Ui& ui, float W, float stageTop, float stageBottom, int n) {
        const float stageH = stageBottom - stageTop;
        const float cardH = stageH * 0.8f, top = stageTop + (stageH - cardH) / 2;
        ui.hit.dragStep = cardH * (kStripNarrowRatio + kStripGap);

        const int lo = std::max(0, static_cast<int>(std::floor(ui.position)) - 3);
        const int hi = std::min(n - 1, static_cast<int>(std::ceil(ui.position)) + 3);
        // Cards never overlap, so the order only matters for the selected one's border: last.
        std::vector<std::pair<int, StripSlot>> visible;
        for (int i = lo; i <= hi; ++i) {
            StripSlot slot = ComputeStripSlot(static_cast<float>(i) - ui.position, W, cardH);
            if (slot.alpha > 0.f) visible.push_back({i, slot});
        }
        std::stable_sort(visible.begin(), visible.end(), [&](const auto& a, const auto& b) {
            return (a.first == ui.selected) < (b.first == ui.selected);
        });
        for (const auto& [index, slot] : visible) {
            DrawCard(ui, index, slot, top, cardH);
            AddRegion(ui, Target::Card, index, StripQuad(slot, top, cardH));
        }
    }

    static void DrawCard(Ui& ui, int index, const StripSlot& slot, float top, float cardH) {
        const Quad quad = StripQuad(slot, top, cardH);
        ComPtr<ID2D1PathGeometry> shape = QuadGeometry(ui, quad);
        if (!shape) return;
        const bool selected = index == ui.selected;
        const float a = slot.alpha;

        Quad shadow = quad;  // cheap drop shadow
        for (Pt& p : shadow) p.y += 6.f;
        FillQuad(ui, shadow, Col(0, 0, 0, 0.35f * a));

        const float skew = cardH * kStripSkew * 0.5f;
        PaintThumb(ui, ItemThumb(ui, index), shape.Get(),
                   D2D1::RectF(slot.cx - slot.w / 2 - skew, top, slot.cx + slot.w / 2 + skew,
                               top + cardH),
                   a);
        ui.brush->SetColor(selected ? Col(1, 1, 1, 0.95f * a) : Col(1, 1, 1, 0.14f * a));
        ui.rt->DrawGeometry(shape.Get(), ui.brush.Get(), selected ? 2.5f : 1.f);

        // Caption band along the bottom: light on the selected card, dark on the others.
        const Quad band = QuadBand(quad, 1.f - kCaptionH / cardH, 1.f);
        FillQuad(ui, band, selected ? Col(1, 1, 1, 0.95f * a) : Col(0.05f, 0.05f, 0.08f, 0.80f * a));
        const float left = std::max(band[0].x, band[3].x) + 12.f;
        const float right = std::min(band[1].x, band[2].x) - 12.f;
        if (right > left) {
            DrawTextIn(ui, ItemLabel(ui, index), ui.fontName.Get(),
                       D2D1::RectF(left, band[0].y, right, band[3].y),
                       selected ? Ink(a) : Col(1, 1, 1, 0.92f * a));
        }

        const std::wstring detail = ItemDetail(ui, index);
        if (!detail.empty()) {  // folder badge, top left
            const float w = TextWidth(ui, detail, ui.fontTiny.Get()) + 16.f;
            const float x = std::max(quad[0].x, quad[3].x) + 12.f, y = top + 12.f;
            FillRound(ui, D2D1::RectF(x, y, x + w, y + 20.f), 10.f, Col(1, 1, 1, 0.92f * a));
            DrawTextIn(ui, detail, ui.fontTiny.Get(), D2D1::RectF(x, y, x + w, y + 20.f), Ink(a));
        }
    }

    // Big position counter on the right and the keys for the current level.
    static void DrawFooter(Ui& ui, float W, float H, int n) {
        const float right = W - 24.f, left = right - 220.f;
        DrawTextIn(ui, ThreeDigits(std::clamp(ui.selected, 0, n - 1) + 1), ui.fontBig.Get(),
                   D2D1::RectF(left, H - 80, right, H - 28), Col(1, 1, 1, 0.95f));
        DrawTextIn(ui, L"OF " + std::to_wstring(n), ui.fontIndex.Get(),
                   D2D1::RectF(left, H - 28, right, H - 8), Col(1, 1, 1, 0.5f));

        const std::wstring hint = ui.level == Level::Folders
                                      ? L"← → / wheel  browse     Enter / click  open folder     Esc  close"
                                      : L"← → / wheel  browse     Enter / click  apply     Esc  back";
        // Same margin on both sides keeps the text centred on the window, clear of the counter.
        const float margin = W - left + 20.f;
        DrawTextIn(ui, hint, ui.fontHint.Get(), D2D1::RectF(margin, H - 30, W - margin, H - 8),
                   Col(1, 1, 1, 0.5f));
    }
};

// A big preview of the selected wallpaper over a dimmed copy of itself, with a rail of thumbnails
// underneath. Moving the selection crossfades both. Collections are chips along the top.
class ShowcaseStyle final : public Style {
public:
    StyleSize WindowSize() const override { return {1100.f, 580.f}; }
    bool HasFolderLevel() const override { return false; }

    void Draw(Ui& ui, float W, float H) const override {
        const int n = ItemCount(ui);
        DrawBackdrop(ui, W, H, n);
        DrawChips(ui, W);
        DrawWindowButtons(ui, W);

        const float heroTop = 64.f, heroH = H - heroTop - 170.f, heroW = heroH * 16.f / 9.f;
        const D2D1_RECT_F hero =
            D2D1::RectF((W - heroW) / 2, heroTop, (W + heroW) / 2, heroTop + heroH);
        if (n == 0) return DrawEmptyState(ui, W, heroTop, hero.bottom);

        DrawArrows(ui, hero, n);
        DrawHero(ui, hero, n);
        DrawRail(ui, W, H - 92.f, n);
        DrawKeys(ui, W, H);
    }

private:
    static constexpr float kThumbW = 112.f, kThumbH = 70.f, kRailGap = 12.f;

    static D2D1_COLOR_F Ink() { return Col(0.09f, 0.09f, 0.11f); }

    // The backdrop and the preview both crossfade the two images the position sits between.
    static void Crossfade(Ui& ui, ID2D1Geometry* shape, const D2D1_RECT_F& bounds, int n) {
        const int lo = std::clamp(static_cast<int>(std::floor(ui.position)), 0, n - 1);
        const int hi = std::min(lo + 1, n - 1);
        const float f = std::clamp(ui.position - static_cast<float>(lo), 0.f, 1.f);
        PaintThumb(ui, ItemThumb(ui, lo), shape, bounds, 1.f);
        if (hi != lo && f > 0.002f) PaintThumb(ui, ItemThumb(ui, hi), shape, bounds, f);
    }

    static void DrawBackdrop(Ui& ui, float W, float H, int n) {
        const D2D1_RECT_F window = D2D1::RectF(0, 0, W, H);
        FillRound(ui, window, 24.f, Col(0.04f, 0.04f, 0.07f));
        if (n) {
            if (auto shape = RoundedGeometry(ui, window, 24.f)) Crossfade(ui, shape.Get(), window, n);
        }
        FillRound(ui, window, 24.f, Col(0.03f, 0.03f, 0.06f, 0.70f));  // dims the blended copy
        StrokeRound(ui, D2D1::RectF(0.5f, 0.5f, W - 0.5f, H - 0.5f), 24.f, Col(1, 1, 1, 0.10f));
    }

    static void DrawChips(Ui& ui, float W) {
        const float y = 16.f, h = 30.f, limit = W - 24.f - 2 * 30.f - 8.f - 12.f;
        float x = 24.f;
        for (size_t i = 0; i < ui.library.size(); ++i) {
            const float w = std::min(TextWidth(ui, ui.library[i].name, ui.fontUi.Get()) + 26.f, 160.f);
            // shortcut: chips that do not fit are not drawn, Tab still cycles through them.
            if (x + w > limit) break;
            const D2D1_RECT_F r = D2D1::RectF(x, y, x + w, y + h);
            const bool on = static_cast<int>(i) == ui.collection;
            FillRound(ui, r, h / 2, on ? Col(1, 1, 1, 0.94f) : Col(1, 1, 1, 0.12f));
            DrawTextIn(ui, ui.library[i].name, ui.fontUi.Get(),
                       D2D1::RectF(r.left + 13, r.top, r.right - 13, r.bottom),
                       on ? Ink() : Col(1, 1, 1, 0.85f));
            AddRegion(ui, Target::Tab, static_cast<int>(i), RectQuad(r));
            x += w + 6.f;
        }
    }

    static void DrawWindowButtons(Ui& ui, float W) {
        const float d = 30.f, y = 16.f;
        const D2D1_RECT_F close = D2D1::RectF(W - 24 - d, y, W - 24, y + d);
        const D2D1_RECT_F rescan = D2D1::RectF(close.left - 8 - d, y, close.left - 8, y + d);
        for (const D2D1_RECT_F& r : {close, rescan}) {
            ui.brush->SetColor(Col(1, 1, 1, 0.12f));
            ui.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F((r.left + r.right) / 2, y + d / 2),
                                             d / 2, d / 2),
                               ui.brush.Get());
        }
        const float cx = (close.left + close.right) / 2, cy = y + d / 2;
        Line(ui, cx - 5, cy - 5, cx + 5, cy + 5, Col(1, 1, 1, 0.9f), 2.f);
        Line(ui, cx + 5, cy - 5, cx - 5, cy + 5, Col(1, 1, 1, 0.9f), 2.f);
        DrawTextIn(ui, L"↻", ui.fontSymbol.Get(), rescan, Col(1, 1, 1, 0.9f));
        AddRegion(ui, Target::Close, 0, RectQuad(close));
        AddRegion(ui, Target::Rescan, 0, RectQuad(rescan));
    }

    static void DrawArrows(Ui& ui, const D2D1_RECT_F& hero, int n) {
        const float d = 40.f, cy = (hero.top + hero.bottom) / 2;
        struct Arrow {
            float cx;
            Target target;
            float direction;  // the chevron points away from the centre
            bool enabled;
        };
        const Arrow arrows[] = {{hero.left - 36.f, Target::Prev, -1.f, ui.selected > 0},
                                {hero.right + 36.f, Target::Next, 1.f, ui.selected < n - 1}};
        for (const Arrow& a : arrows) {
            ui.brush->SetColor(Col(1, 1, 1, a.enabled ? 0.16f : 0.07f));
            ui.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(a.cx, cy), d / 2, d / 2), ui.brush.Get());
            const D2D1_COLOR_F c = Col(1, 1, 1, a.enabled ? 0.95f : 0.30f);
            Line(ui, a.cx - a.direction * 3, cy - 8, a.cx + a.direction * 3, cy, c, 2.5f);
            Line(ui, a.cx + a.direction * 3, cy, a.cx - a.direction * 3, cy + 8, c, 2.5f);
            AddRegion(ui, a.target, 0,
                      RectQuad(D2D1::RectF(a.cx - d / 2, cy - d / 2, a.cx + d / 2, cy + d / 2)));
        }
    }

    static void DrawHero(Ui& ui, const D2D1_RECT_F& hero, int n) {
        const int sel = std::clamp(ui.selected, 0, n - 1);
        FillRound(ui, D2D1::RectF(hero.left, hero.top + 10, hero.right, hero.bottom + 10), 18.f,
                  Col(0, 0, 0, 0.45f));  // cheap drop shadow
        if (auto shape = RoundedGeometry(ui, hero, 18.f)) {
            Crossfade(ui, shape.Get(), hero, n);
            FillBottomScrim(ui, shape.Get(), hero, 0.80f);
        }
        StrokeRound(ui, hero, 18.f, Col(1, 1, 1, 0.20f));

        DrawTextIn(ui, ItemLabel(ui, sel), ui.fontTitle.Get(),
                   D2D1::RectF(hero.left + 24, hero.bottom - 66, hero.right - 24, hero.bottom - 34),
                   Col(1, 1, 1, 0.97f));
        DrawTextIn(ui, TwoDigits(sel + 1) + L" / " + std::to_wstring(n) + L"  ·  " + OpenCollectionName(ui),
                   ui.fontUi.Get(),
                   D2D1::RectF(hero.left + 24, hero.bottom - 36, hero.right - 24, hero.bottom - 12),
                   Col(1, 1, 1, 0.65f));
        AddRegion(ui, Target::Card, sel, RectQuad(hero));  // a click on the preview applies it
    }

    static void DrawRail(Ui& ui, float W, float cy, int n) {
        const int lo = std::max(0, static_cast<int>(std::floor(ui.position)) - 5);
        const int hi = std::min(n - 1, static_cast<int>(std::ceil(ui.position)) + 5);
        std::vector<std::pair<int, RailSlot>> visible;
        for (int i = lo; i <= hi; ++i) {
            RailSlot slot = ComputeRailSlot(static_cast<float>(i) - ui.position, W, kThumbW, kRailGap);
            if (slot.alpha > 0.f) visible.push_back({i, slot});
        }
        // Far thumbnails first, so the zoomed one is never covered.
        std::sort(visible.begin(), visible.end(), [&](const auto& a, const auto& b) {
            return std::fabs(static_cast<float>(a.first) - ui.position) >
                   std::fabs(static_cast<float>(b.first) - ui.position);
        });
        auto rectOf = [&](const RailSlot& s) {
            const float w = kThumbW * s.scale, h = kThumbH * s.scale;
            return D2D1::RectF(s.cx - w / 2, cy - h / 2, s.cx + w / 2, cy + h / 2);
        };

        for (const auto& [index, slot] : visible) {
            const D2D1_RECT_F r = rectOf(slot);
            const float focus = std::max(0.f, 1.f - std::fabs(static_cast<float>(index) - ui.position));
            if (auto shape = RoundedGeometry(ui, r, 10.f)) {
                PaintThumb(ui, ItemThumb(ui, index), shape.Get(), r, slot.alpha * (0.6f + 0.4f * focus));
            }
            StrokeRound(ui, r, 10.f,
                        index == ui.selected ? WithAlpha(kAccent, slot.alpha)
                                             : Col(1, 1, 1, 0.14f * slot.alpha));
        }
        for (auto it = visible.rbegin(); it != visible.rend(); ++it) {  // front-most first
            AddRegion(ui, Target::Card, it->first, RectQuad(rectOf(it->second)));
        }
    }

    static void DrawKeys(Ui& ui, float W, float H) {
        DrawTextIn(ui, L"← → / wheel  browse     Enter / click  apply     Tab  collection     Esc  close",
                   ui.fontHint.Get(), D2D1::RectF(24, H - 36, W - 24, H - 12), Col(1, 1, 1, 0.5f));
    }
};

struct StyleEntry {
    const wchar_t* id;  // the value of the `style` setting
    const Style* style;
};

static const CarouselStyle kCarousel{};
static const StripStyle kStrip{};
static const ShowcaseStyle kShowcase{};
static const StyleEntry kStyles[] = {
    {L"carousel", &kCarousel}, {L"strip", &kStrip}, {L"showcase", &kShowcase}};

// Unknown or empty ids fall back to the first style.
static const Style* FindStyle(const std::wstring& id) {
    for (const StyleEntry& entry : kStyles) {
        if (id == entry.id) return entry.style;
    }
    return kStyles[0].style;
}

// Asks for the thumbnails around the current position and frees the rest.
static void PrepareThumbs(Ui& ui) {
    std::set<std::wstring> keep;
    const int n = ItemCount(ui);
    const int center = static_cast<int>(std::lround(ui.position));
    if (n > 0) {
        for (int i = std::max(0, center - 8); i <= std::min(n - 1, center + 8); ++i) {
            keep.insert(ItemThumb(ui, i));
        }
        ui.thumbs.Retain(keep);
        for (int d = 5; d >= 0; --d) {  // far to near: the newest request loads first
            for (int i : {center - d, center + d}) {
                if (i >= 0 && i < n) ui.thumbs.Request(ItemThumb(ui, i));
            }
        }
    } else {
        ui.thumbs.Retain(keep);
    }
}

static void Render(HWND hwnd, Ui& ui) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    const int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0 || !ui.style || !EnsureTarget(ui) || !EnsureSurface(ui, w, h)) return;

    PrepareThumbs(ui);
    ui.thumbs.Drain(ui.rt.Get());

    ui.rt->BindDC(ui.memDc, &rc);
    ui.rt->BeginDraw();
    ui.rt->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
    ui.rt->SetTransform(D2D1::Matrix3x2F::Scale(ui.scale, ui.scale));
    ui.hit = HitMap{};
    ui.style->Draw(ui, w / ui.scale, h / ui.scale);
    if (ui.rt->EndDraw() == D2DERR_RECREATE_TARGET) {
        ui.thumbs.Clear();
        ui.brush.Reset();
        ui.rt.Reset();  // rebuilt on the next frame
        return;
    }

    RECT wr;
    GetWindowRect(hwnd, &wr);
    POINT dst{wr.left, wr.top}, src{0, 0};
    SIZE size{w, h};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, nullptr, &dst, &size, ui.memDc, &src, 0, &blend, ULW_ALPHA);
}

static void StartAnimation(HWND hwnd) {
    SetTimer(hwnd, kAnimTimer, 16, nullptr);
}

static void ApplyScanResult(HWND hwnd, Ui& ui, std::vector<Collection> library) {
    const bool inImages = ui.level == Level::Wallpapers;
    const int n = inImages ? ItemCount(ui) : 0;
    const std::wstring collName = OpenCollectionName(ui);
    const std::wstring imgPath =
        n ? ui.library[ui.collection].images[std::clamp(ui.selected, 0, n - 1)].path : L"";

    ui.library = std::move(library);
    ui.scanned = true;

    // Keep the user where they were when the same collection and image still exist.
    ui.collection = 0;
    for (size_t i = 0; i < ui.library.size(); ++i) {
        if (ui.library[i].name == collName) ui.collection = static_cast<int>(i);
    }
    ui.selected = 0;
    if (!inImages) {
        ui.selected = std::clamp(ui.collection, 0, std::max(ItemCount(ui) - 1, 0));
    } else if (!ui.library.empty() && !imgPath.empty()) {
        const auto& images = ui.library[ui.collection].images;
        for (size_t i = 0; i < images.size(); ++i) {
            if (images[i].path == imgPath) ui.selected = static_cast<int>(i);
        }
    }
    ui.position = static_cast<float>(ui.selected);
    Render(hwnd, ui);
}

static void HideSelector() {
    HWND hwnd = g_hwnd.load();
    if (!hwnd) return;
    KillTimer(hwnd, kAnimTimer);
    if (GetCapture() == hwnd) ReleaseCapture();
    g_scanCancel = true;  // the next show joins the thread and starts a fresh scan
    if (g_ui) {
        g_ui->pressed = false;
        g_ui->thumbs.Clear();  // frees the decoded bitmaps while the selector is closed
    }
    ShowWindow(hwnd, SW_HIDE);
}

// A hotkey normally grants foreground rights; if Windows still refuses, an empty input event
// from this thread makes it accept the call (no input queue is shared with the other app).
static bool TakeForeground(HWND hwnd) {
    SetForegroundWindow(hwnd);
    if (GetForegroundWindow() == hwnd) return true;
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    return GetForegroundWindow() == hwnd;
}

// The monitor with the mouse, or with the window that has the focus. The desktop and the taskbar
// are no real focus (they are always "focused" with nothing open), so they fall back to the mouse.
static HMONITOR MonitorForSelector(MonitorSource source) {
    if (source == MonitorSource::Focus) {
        HWND focus = GetForegroundWindow();
        wchar_t cls[32] = L"";
        if (focus) GetClassNameW(focus, cls, 32);
        const bool shell = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
                           !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd");
        if (focus && !shell && !IsIconic(focus)) return MonitorFromWindow(focus, MONITOR_DEFAULTTONEAREST);
    }
    POINT pt;
    GetCursorPos(&pt);
    return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
}

static void ShowSelector() {
    HWND hwnd = g_hwnd.load();
    Ui* ui = g_ui;
    if (!hwnd || !ui) return;

    const Settings settings = LoadSettings();
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorForSelector(ParseMonitorSource(settings.monitorSource)), &mi)) return;

    const Style* style = FindStyle(settings.style);
    const StyleSize size = style->WindowSize();

    // Move first so GetDpiForWindow reports the DPI of the target monitor.
    SetWindowPos(hwnd, HWND_TOPMOST, mi.rcWork.left, mi.rcWork.top, 1, 1, SWP_NOACTIVATE);
    const UINT dpi = GetDpiForWindow(hwnd);
    ui->scale = dpi / 96.f;
    const int workW = mi.rcWork.right - mi.rcWork.left;
    const int w = std::min<int>(static_cast<int>(size.w * ui->scale), workW - MulDiv(32, dpi, 96));
    const int h = static_cast<int>(size.h * ui->scale);
    const int x = mi.rcWork.left + (workW - w) / 2;
    const int y = mi.rcWork.bottom - h - MulDiv(24, dpi, 96);
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);

    // A style that browses folders always opens on them, even if the last session ended inside one.
    const Level start = style->HasFolderLevel() ? Level::Folders : Level::Wallpapers;
    if (ui->style != style || ui->level != start) {
        ui->style = style;
        SetLevel(*ui, start);
    }

    ui->root = settings.root;
    ui->transition = settings.transition;
    ui->currentMonitorOnly = settings.currentMonitorOnly;
    ui->pressed = false;
    StartScan(hwnd, ui->root);
    Render(hwnd, *ui);
    // Without the foreground the selector gets no keys and never sees WA_INACTIVE, so it would stay stuck.
    if (!TakeForeground(hwnd)) {
        Wh_Log(L"Could not take the foreground, closing the selector");
        HideSelector();
    }
}

static void SelectIndex(HWND hwnd, Ui& ui, int index) {
    const int n = ItemCount(ui);
    if (!n) return;
    ui.selected = std::clamp(index, 0, n - 1);  // clamped, not wrapped: 1-2 images must not repeat
    StartAnimation(hwnd);
}

static void SwitchCollection(HWND hwnd, Ui& ui, int index) {
    if (ui.library.empty()) return;
    ui.collection = WrapIndex(index, static_cast<int>(ui.library.size()));
    ui.selected = 0;
    ui.position = 0.f;
    Render(hwnd, ui);
}

static void Rescan(HWND hwnd, Ui& ui) {
    ui.thumbs.Clear();  // files may have changed on disk
    ui.root = LoadSettings().root;
    StartScan(hwnd, ui.root);
}

static BOOL CALLBACK CollectMonitor(HMONITOR, HDC, LPRECT rect, LPARAM list) {
    reinterpret_cast<std::vector<RECT>*>(list)->push_back(*rect);
    return TRUE;
}

// The middle of a monitor: where the effect starts when no card is on that monitor.
static RectF CentreOf(const RECT& monitor) {
    const float mw = static_cast<float>(monitor.right - monitor.left);
    const float mh = static_cast<float>(monitor.bottom - monitor.top);
    return RectF{mw * 0.375f, mh * 0.375f, mw * 0.625f, mh * 0.625f};
}

static bool SelectorMonitor(HWND hwnd, RECT& monitor) {
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) return false;
    monitor = mi.rcMonitor;
    return true;
}

// The effect on the selector's own monitor. It starts at the biggest thing on screen that stands
// for the selected wallpaper (the centre card, the strip card or the preview).
static TransitionJob MakeSelectorJob(HWND hwnd, const Ui& ui, Transition kind) {
    TransitionJob job;
    job.kind = kind;
    if (!SelectorMonitor(hwnd, job.monitor)) return job;  // an empty monitor: no effect
    job.from = CentreOf(job.monitor);                      // when nothing is on screen

    RECT window;
    GetWindowRect(hwnd, &window);
    float bestArea = 0.f;
    for (const HitRegion& r : ui.hit.regions) {
        if (r.target != Target::Card || r.index != ui.selected) continue;
        float l = r.shape[0].x, t = r.shape[0].y, rr = l, b = t;
        for (const Pt& p : r.shape) {
            l = std::min(l, p.x);
            rr = std::max(rr, p.x);
            t = std::min(t, p.y);
            b = std::max(b, p.y);
        }
        const float area = (rr - l) * (b - t);
        if (area <= bestArea) continue;
        bestArea = area;
        const float dx = static_cast<float>(window.left - job.monitor.left);
        const float dy = static_cast<float>(window.top - job.monitor.top);
        job.from = RectF{l * ui.scale + dx, t * ui.scale + dy, rr * ui.scale + dx, b * ui.scale + dy};
    }
    return job;
}

// One job per monitor that plays the effect: just the selector's, or every monitor with the
// selector's first.
static std::vector<TransitionJob> MakeTransitionJobs(HWND hwnd, const Ui& ui, Transition kind) {
    const TransitionJob job = MakeSelectorJob(hwnd, ui, kind);
    std::vector<TransitionJob> jobs;
    if (job.monitor.right <= job.monitor.left) return jobs;  // the monitor is unknown: no effect
    jobs.push_back(job);
    if (ui.currentMonitorOnly) return jobs;
    std::vector<RECT> all;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitor, reinterpret_cast<LPARAM>(&all));
    for (const RECT& rect : all) {
        if (EqualRect(&rect, &job.monitor)) continue;
        TransitionJob other;
        other.kind = kind;
        other.monitor = rect;
        other.from = CentreOf(rect);
        jobs.push_back(other);
    }
    return jobs;
}

static void ConfirmSelection(HWND hwnd, Ui& ui) {
    const int n = ItemCount(ui);
    if (!n || ui.level != Level::Wallpapers) return;
    std::wstring path = ui.library[ui.collection].images[std::clamp(ui.selected, 0, n - 1)].path;
    const Transition kind = ResolveTransition(ParseTransition(ui.transition), GetTickCount());
    Wh_Log(L"confirm: setting '%ls' gives effect %d, %ls", ui.transition.c_str(), static_cast<int>(kind),
           ui.currentMonitorOnly ? L"current monitor only" : L"every monitor");
    // The monitor the selector is on is the only one that changes when the setting says so.
    std::optional<RECT> only;
    RECT own{};
    if (ui.currentMonitorOnly && SelectorMonitor(hwnd, own)) only = own;
    if (kind == Transition::None) {
        HideSelector();
        ApplyWallpaperAsync(std::move(path), only);
        return;
    }
    // The selector stays until the overlay is up (it asks to be hidden), so the card never blinks.
    ApplyWallpaperAsync(std::move(path), only, MakeTransitionJobs(hwnd, ui, kind), hwnd);
}

static void OpenFolder(HWND hwnd, Ui& ui, int collection) {
    ui.collection = std::clamp(collection, 0, std::max(static_cast<int>(ui.library.size()) - 1, 0));
    SetLevel(ui, Level::Wallpapers);
    Render(hwnd, ui);
}

static void BackToFolders(HWND hwnd, Ui& ui) {
    SetLevel(ui, Level::Folders);
    Render(hwnd, ui);
}

// Enter, or a click on the selected card: open the folder, or apply the wallpaper.
static void Activate(HWND hwnd, Ui& ui) {
    if (!ItemCount(ui)) return;
    if (ui.level == Level::Folders) OpenFolder(hwnd, ui, ui.selected);
    else ConfirmSelection(hwnd, ui);
}

static bool InsideFolder(const Ui& ui) {
    return ui.style->HasFolderLevel() && ui.level == Level::Wallpapers;
}

static void HandleKey(HWND hwnd, Ui& ui, WPARAM key) {
    switch (key) {
        case VK_ESCAPE:
            if (InsideFolder(ui)) BackToFolders(hwnd, ui);
            else HideSelector();
            break;
        case VK_BACK:
            if (InsideFolder(ui)) BackToFolders(hwnd, ui);
            break;
        case VK_RETURN: Activate(hwnd, ui); break;
        case VK_F5: Rescan(hwnd, ui); break;
        case VK_LEFT: SelectIndex(hwnd, ui, ui.selected - 1); break;
        case VK_RIGHT: SelectIndex(hwnd, ui, ui.selected + 1); break;
        case VK_HOME: SelectIndex(hwnd, ui, 0); break;
        case VK_END: SelectIndex(hwnd, ui, ItemCount(ui) - 1); break;
        case VK_TAB:
            if (!ui.style->HasFolderLevel()) {
                SwitchCollection(hwnd, ui,
                                 ui.collection + ((GetKeyState(VK_SHIFT) & 0x8000) ? -1 : 1));
            }
            break;
    }
}

static void HandleMouseDown(HWND hwnd, Ui& ui, float x, float y) {
    ui.pressCard = -1;
    for (const HitRegion& r : ui.hit.regions) {
        if (!PointInQuad(r.shape, x, y)) continue;
        switch (r.target) {
            case Target::Close: return HideSelector();
            case Target::Rescan: return Rescan(hwnd, ui);
            case Target::Prev: return SelectIndex(hwnd, ui, ui.selected - 1);
            case Target::Next: return SelectIndex(hwnd, ui, ui.selected + 1);
            case Target::Tab: return SwitchCollection(hwnd, ui, r.index);
            case Target::Card: ui.pressCard = r.index; break;
        }
        break;  // the front-most region under the pointer wins
    }

    // Press on the stage: a click selects or activates a card, a drag spins the strip.
    ui.pressed = true;
    ui.dragMoved = false;
    ui.pressX = x;
    ui.pressPos = ui.position;
    SetCapture(hwnd);
}

static void HandleMouseMove(HWND hwnd, Ui& ui, float x) {
    if (!ui.pressed) return;
    const float dx = x - ui.pressX;
    if (!ui.dragMoved && std::fabs(dx) > 4.f) ui.dragMoved = true;
    if (!ui.dragMoved) return;
    KillTimer(hwnd, kAnimTimer);  // the pointer drives the position while dragging
    const float last = static_cast<float>(std::max(ItemCount(ui) - 1, 0));
    ui.position = std::clamp(DragPosition(ui.pressPos, dx, ui.hit.dragStep), 0.f, last);
    Render(hwnd, ui);
}

static void HandleMouseUp(HWND hwnd, Ui& ui) {
    if (!ui.pressed) return;
    ui.pressed = false;
    ReleaseCapture();
    if (ui.dragMoved) {
        SelectIndex(hwnd, ui, static_cast<int>(std::lround(ui.position)));
    } else if (ui.pressCard >= 0) {
        if (ui.pressCard == ui.selected) Activate(hwnd, ui);
        else SelectIndex(hwnd, ui, ui.pressCard);
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Ui* ui = g_ui;
    switch (msg) {
        case WM_HOTKEY:
            if (wp == kHotkeyId) {
                if (IsWindowVisible(hwnd)) HideSelector();
                else ShowSelector();
            }
            return 0;
        case WM_WC_SETTINGS:
            RegisterHotkeyFromSettings(hwnd);
            return 0;
        case WM_WC_HIDE:
            HideSelector();
            return 0;
        case WM_WC_THUMB:
            if (ui && IsWindowVisible(hwnd)) Render(hwnd, *ui);
            return 0;
        case WM_WC_SCAN: {
            std::optional<std::vector<Collection>> result;
            {
                std::lock_guard<std::mutex> lock(g_scanMutex);
                result.swap(g_scanResult);
            }
            if (ui && result && IsWindowVisible(hwnd)) ApplyScanResult(hwnd, *ui, std::move(*result));
            return 0;
        }
        case WM_TIMER:
            if (wp == kAnimTimer && ui) {
                const float target = static_cast<float>(ui->selected);
                ui->position += (target - ui->position) * 0.28f;
                if (std::fabs(target - ui->position) < 0.003f) {
                    ui->position = target;
                    KillTimer(hwnd, kAnimTimer);
                }
                Render(hwnd, *ui);
            }
            return 0;
        case WM_KEYDOWN:
            if (ui) HandleKey(hwnd, *ui, wp);
            return 0;
        case WM_MOUSEWHEEL:
            if (ui) {
                ui->wheelAcc += GET_WHEEL_DELTA_WPARAM(wp);
                const int steps = ui->wheelAcc / WHEEL_DELTA;
                ui->wheelAcc -= steps * WHEEL_DELTA;
                if (steps) SelectIndex(hwnd, *ui, ui->selected - steps);  // wheel up = previous
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (ui) HandleMouseDown(hwnd, *ui, GET_X_LPARAM(lp) / ui->scale, GET_Y_LPARAM(lp) / ui->scale);
            return 0;
        case WM_MOUSEMOVE:
            if (ui) HandleMouseMove(hwnd, *ui, GET_X_LPARAM(lp) / ui->scale);
            return 0;
        case WM_LBUTTONUP:
            if (ui) HandleMouseUp(hwnd, *ui);
            return 0;
        case WM_CAPTURECHANGED:
            if (ui) ui->pressed = false;  // e.g. Alt+Tab in the middle of a drag
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) HideSelector();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            UnregisterHotKey(hwnd, kHotkeyId);
            KillTimer(hwnd, kAnimTimer);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

struct UiStart {
    HANDLE ready = nullptr;
    bool ok = false;
};

static DWORD WINAPI UiThreadProc(LPVOID param) {
    auto* start = static_cast<UiStart*>(param);
    // The code works in real pixels (monitor rectangles, per-monitor DPI), whatever the host process is.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kWindowClass,
                                L"Wallpaper Changer", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                                ModuleInstance(), nullptr);
    Ui* ui = nullptr;
    if (hwnd) {
        ui = new (std::nothrow) Ui;
        if (ui && InitUi(*ui)) {
            ui->style = FindStyle(L"");  // the real one is picked from the settings on every show
            g_ui = ui;
            ui->thumbs.Start(hwnd, WM_WC_THUMB);
            g_hwnd = hwnd;
            RegisterHotkeyFromSettings(hwnd);
            start->ok = true;
        } else {
            delete ui;
            ui = nullptr;
            DestroyWindow(hwnd);
            hwnd = nullptr;
        }
    }
    SetEvent(start->ready);  // `start` lives on the caller's stack: do not touch it again
    if (!hwnd) {
        if (com) CoUninitialize();
        return 0;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hwnd = nullptr;
    ShowWindow(hwnd, SW_HIDE);
    StopScan();
    StopApply();
    ui->thumbs.Join();
    {
        std::lock_guard<std::mutex> lock(g_scanMutex);
        g_scanResult.reset();
    }
    g_ui = nullptr;
    delete ui;
    DestroyWindow(hwnd);
    if (com) CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ModuleInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed: %lu", GetLastError());
        return FALSE;
    }

    UiStart start;
    start.ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (start.ready) {
        g_uiThread = CreateThread(nullptr, 0, UiThreadProc, &start, 0, &g_uiThreadId);
        if (g_uiThread) WaitForSingleObject(start.ready, INFINITE);
        CloseHandle(start.ready);
    }
    if (g_uiThread && start.ok) return TRUE;

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    UnregisterClassW(kWindowClass, ModuleInstance());
    return FALSE;
}

void WhTool_ModSettingsChanged() {
    if (HWND hwnd = g_hwnd.load()) PostMessageW(hwnd, WM_WC_SETTINGS, 0, 0);
}

// The process exits right after this returns, so everything is stopped and joined here.
void WhTool_ModUninit() {
    if (!g_uiThread) return;
    // WhTool_ModInit returned only after the thread made its window, so its message queue exists.
    PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_uiThread, INFINITE);
    CloseHandle(g_uiThread);
    g_uiThread = nullptr;
    UnregisterClassW(kWindowClass, ModuleInstance());
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
