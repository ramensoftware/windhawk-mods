// ==WindhawkMod==
// @id              desktop-sticky-notes
// @name            Easy Notes
// @description     Sticky notes that live on the desktop, with checklists, clickable links, auto-numbering and a snapping column grid
// @version         1.0.0
// @author          Torsion
// @github          https://github.com/Torsion1035
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lcomctl32 -lshell32 -lole32 -ldwmapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Easy Notes

Sticky notes for your Windows desktop. The notes sit above the wallpaper and
desktop icons, but **behind every application window**, like part of the
wallpaper you can write on. They never take focus by themselves, and they
never show up in the taskbar or in Alt+Tab.

![Easy Notes: three notes and the Themes panel](https://raw.githubusercontent.com/Torsion1035/easy-notes/main/screenshot.png)

The mod runs in its own process, so it never runs inside Explorer.

## Modes

* **Single**: one note.
* **Multiple**: as many notes as you want, for example one per department or
  client. Each note has its own title, and can have its own background color.

Switching modes never deletes anything. Single mode shows the first note, and
Multiple mode shows all of them. Switch with the *Mode* setting or from a
note's **...** menu > *Mode*; whichever you changed last wins.

## The tool panel

Every note has a two-row tool panel at the top:

* **Row 1, the title bar**: the note's title (drag it to move the note,
  double-click it to rename) and the **...** menu button on the right.
* **Row 2, the tool strip**, left to right:
  * **1.**: auto-numbering on/off
  * **checkbox icon**: turn every line into a checkbox, or back into plain text
  * **B** and **I**: bold and italic for the selected text. They work while
    you're editing and are faded out otherwise.
  * **Color circle** (right end): drawn in the note's current background
    color, so it previews the note. Click it for the color menu.

### "..." menu (title bar)

1. Themes...
2. Mode: Single / Multiple
3. Auto-numbering (checked when on)
4. All lines as checkboxes
5. Height: Small / Medium / Large
6. New note (disabled in Single mode)
7. Delete note
8. About Easy Notes (creator and MIT license)

Right-clicking the title bar or the tool strip opens the same menu.

### Color menu (Color circle in row 2)

1. Themes...
2. Follow theme: clears this note's background override (checked when no
   override is set)
3. Yellow, Green, Blue, Pink, Purple, Orange, Gray, each with a swatch. These
   set this note's background only.
4. Custom (hex from settings): the *Custom color* hex code

### Right-click menu on the note body

Cut, Copy, Paste, Bold, Italic, Themes..., Delete note. Cut, Copy, Paste, Bold
and Italic work while you're editing and are shown disabled otherwise.

## Using the notes

| Action | How |
|---|---|
| Edit text | Click an empty part of a note's body. Press **Esc** or click elsewhere to finish. |
| Rename | Double-click the title. **Enter** saves and **Esc** cancels. |
| Bold / Italic | While editing, select text and click **B** / **I**, press **Ctrl+B** / **Ctrl+I**, or use the right-click menu. With nothing selected, it applies to what you type next. |
| Checkbox item | Start a line with `[ ] ` while editing. `[x] ` marks it as done. In normal view, click the box to toggle it. Done items are struck through. |
| Links | `http://`, `https://` and `www.` addresses are underlined. Click one to open it in your default browser. |
| Auto-numbering | Click **1.** in the tool strip (off by default). Non-empty lines are numbered 1., 2., 3., ... and the numbers update automatically. |
| Checklist | Click the checkbox icon in the tool strip, or "..." > *All lines as checkboxes*. |
| Note color | Click the Color circle: *Follow theme*, a preset color, or *Custom (hex from settings)*. Text turns light automatically on dark colors. |
| Theme | "..." > *Themes...*, the Color circle > *Themes...*, right-click > *Themes...*, or the *Theme* setting. |
| Height | "..." > *Height* > Small / Medium / Large. |
| New / delete | "..." > *New note* / *Delete note* (delete is also in the right-click menu). |
| Move | Drag the title bar. Horizontal movement only switches columns, and vertical movement is free. A blue outline shows where the note will land. |
| Scroll | Use the mouse wheel over a note whose text doesn't fit. |
| Paste | Ctrl+V and *Paste* insert plain text: formatting copied from web pages is dropped. |

## Themes

Pick a theme in the **Themes panel** (open it with *Themes...* from the "..."
menu, the Color circle or the right-click menu), or in the *Theme* setting.
Both set the same thing, and whichever you changed last wins.

The panel shows three sections:

* **Showa Tones** (Wada colors): Hojicha, Yoru Kinari, Sakura Sumi, Kaki,
  Matcha Kuri, Ume
* **Right Now** (2026 trend colors): Kumo, Tsuchi, Moegi, Fuji, Akane
* **Quiet Hours** (minimal neutrals): **Default**, Shiro, Hai, Kinu, Sumi

**Default** is the solid color from the *Solid color* setting, and it's the
theme on a fresh install.

Each tile shows a miniature note in that theme's real colors (title bar, tool
strip with icons, two lines of text). Hover a tile to see what the name means.
A round dot in each tile's corner marks the selection: an empty ring for the
others, a filled dot for the selected theme, which also gets an outline.

Click a tile to apply it to every note immediately; it's saved right away.
Notes with a background override go back to following the theme. You can
also use the arrow keys to move between tiles, **Space** or **Enter** to
select, and **Esc** to close. The panel also closes when you click anywhere
outside it.

## Custom colors

The *Appearance* setting decides where the note look comes from:

* **Theme** (default): the selected theme (see above).
* **Solid color**: every note uses the *Solid color* hex code as its
  background. The title bar, tool strip and text colors are derived from it.
* **Custom (Jiyu builder)**: Jiyu is the free-form theme. Set *Jiyu base
  color* (note background) and *Jiyu strip color* (tool strip). The title bar,
  icons and text are derived from those two. Jiyu is reachable only through
  this setting, not through the panel.

In Solid and Custom mode the Themes panel opens dimmed, with a banner saying
which mode is on. Clicking a tile then only saves your choice. It takes
effect when *Appearance* is set back to *Theme*.

A note's own background (Color circle) applies in every mode. It changes the
background only; the title bar and tool strip keep the current look.

*Option text color* and *Option hover color* change the theme names and the
hover highlight in the Themes panel. Leave them empty for automatic colors.

## Layout (Multiple mode)

Notes are placed on an automatic column grid on the **primary monitor's work
area**. The column count is `floor(available width / (column width + gap))`.
The *Column width* setting ranges from 220 to 600. Every note is exactly one
column wide, and its height is Small, Medium or Large. Notes in a column
stack from top to bottom without overlapping. If you drop a note above (or
onto the upper half of) another note, the notes below are pushed down. When
the resolution or DPI changes, the grid reflows. If there are fewer columns
than before, notes from removed columns move into the last column.

## Storage

Notes are saved shortly after each change, in the mod's own storage folder
(in a subfolder per Windows user). Windhawk removes the folder together with
the mod. The file is written to a temporary file first and then renamed, so a
crash can't leave it half-written. If `notes.json` can't be read, nothing is
overwritten. If it can't be parsed, it's copied to `notes.corrupt.json` before
anything new is written. Mode, column width, colors, font size and opacity
live in the mod settings.

## Known limitations

* Notes only appear on the primary monitor.
* A column that has more notes than fit vertically runs off the bottom of the
  screen. Use smaller heights or more columns.
* Opacity below 100% uses a layered window. In that case the corners are
  rounded with a window region, so they aren't anti-aliased.
* The notes stay visible when you use Show desktop (Win+D). If a wallpaper
  tool or an Explorer restart rearranges the desktop windows, the mod checks
  every 2 seconds and puts the notes back above the desktop, so they may take
  a moment to return.
* Only bold and italic are supported; no underline, colors, sizes or images.
  Text dragged in from another app may show other formatting while you edit,
  but only bold and italic are kept.
* The menus use the standard Windows look. They don't follow the active theme.
* If the lowest window on screen belongs to an elevated program, the notes
  can't be placed relative to it. They then use the nearest window they can,
  and may stay in front of that elevated window.

## Credits

Created by **Torsion**.

Licensed under the MIT License.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: multiple
  $name: Mode
  $description: Single shows only the first note. Multiple shows every note on a column grid. Switching never deletes notes. The "..." menu on a note can also switch the mode; whichever was changed last wins.
  $options:
  - single: Single note
  - multiple: Multiple notes
- columnWidth: 260
  $name: Column width
  $description: Width of a grid column (and of every note) in pixels at 100% scaling. Range 220-600 (the tool panel needs about 220).
- appearanceMode: theme
  $name: Appearance
  $description: Theme uses the Theme below (also selectable from a note via Themes...). Solid uses the solid color below. Custom uses the Jiyu builder (Jiyu base color + Jiyu strip color).
  $options:
  - theme: Theme
  - solid: Solid color
  - custom: Custom (Jiyu builder)
- theme: default
  $name: Theme
  $description: Used when Appearance is Theme. The Themes panel on a note sets the same thing; whichever was changed last wins. Default uses the solid color below.
  $options:
  - default: Default (solid color)
  - hojicha: "Showa Tones: Hojicha (roasted tea and cream)"
  - yoru-kinari: "Showa Tones: Yoru Kinari (night sky and unbleached cloth)"
  - sakura-sumi: "Showa Tones: Sakura Sumi (cherry blossom and ink)"
  - kaki: "Showa Tones: Kaki (persimmon and charcoal)"
  - matcha-kuri: "Showa Tones: Matcha Kuri (green tea and chestnut)"
  - ume: "Showa Tones: Ume (plum blossom and dusk)"
  - kumo: "Right Now: Kumo (cloud white and slate)"
  - tsuchi: "Right Now: Tsuchi (earth clay and loam)"
  - moegi: "Right Now: Moegi (sprout green and moss)"
  - fuji: "Right Now: Fuji (wisteria and violet)"
  - akane: "Right Now: Akane (madder red and rose)"
  - shiro: "Quiet Hours: Shiro (paper white and graphite)"
  - hai: "Quiet Hours: Hai (ash and stone)"
  - kinu: "Quiet Hours: Kinu (raw silk and sand)"
  - sumi: "Quiet Hours: Sumi (ink black and smoke)"
- solidColor: "#FFF1A8"
  $name: Solid color (hex code)
  $description: Note background in Solid mode and for the Default theme. Title bar, tool strip and text colors are derived from it.
- jiyuBaseColor: "#F4ECD8"
  $name: Jiyu base color (hex code)
  $description: Custom mode only. Note background; the title bar and text are derived from it.
- jiyuStrip: "#3B5B7A"
  $name: Jiyu strip color (hex code)
  $description: Custom mode only. Color of the tool strip under the title bar; its icon color is derived from it.
- optionTextColor: ""
  $name: Option text color (hex code)
  $description: Text color of theme names in the Themes panel. Leave empty for automatic.
- optionHoverColor: ""
  $name: Option hover color (hex code)
  $description: Hover highlight behind tiles in the Themes panel. Leave empty for automatic.
- defaultColor: theme
  $name: Default note color
  $description: Color used for newly created notes. "Follow appearance" means no own color, so the note uses the theme / solid / custom look.
  $options:
  - theme: Follow appearance
  - yellow: Yellow
  - green: Green
  - blue: Blue
  - pink: Pink
  - purple: Purple
  - orange: Orange
  - gray: Gray
  - custom: Custom (hex code below)
- customColor: "#FFE8A3"
  $name: Custom color (hex code)
  $description: "Any color as a hex code, for example #FFE8A3, #3A7BD5 or #38F. Used for new notes when the default color is Custom, and available to every note from the Color circle > Custom (hex from settings)."
- fontSize: 10
  $name: Font size
  $description: Note text size in points. Range 7-24.
- opacity: 100
  $name: Opacity
  $description: Note opacity in percent. Range 30-100.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <richedit.h>
#include <richole.h>
#include <sddl.h>
#include <shellapi.h>
#include <tom.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Logging helpers
// ============================================================================

// Evaluates a Win32 call, logs the expression, line and GetLastError() if
// the result is falsy, and passes the result through unchanged.
#define WIN_CHECK(expr)                                                     \
    ([&]() {                                                                \
        auto _r = (expr);                                                   \
        if (!_r) {                                                          \
            DWORD _e = GetLastError();                                      \
            Wh_Log(L"%s failed, error %u", L"" #expr, (unsigned)_e);        \
        }                                                                   \
        return _r;                                                          \
    }())

#define HR_CHECK(expr)                                                      \
    ([&]() {                                                                \
        HRESULT _hr = (expr);                                               \
        if (FAILED(_hr)) {                                                  \
            Wh_Log(L"%s failed, hr=0x%08X", L"" #expr, (unsigned)_hr);      \
        }                                                                   \
        return _hr;                                                         \
    }())

// ============================================================================
// Constants
// ============================================================================

enum : UINT {
    WM_APP_SHUTDOWN = WM_APP + 1,  // Controller: tear everything down.
    WM_APP_SETTINGS,               // Controller: re-read mod settings.
    WM_APP_DELETE_NOTE,            // Controller: wParam = note id.
    WM_APP_RELAYOUT,               // Controller: debounce a relayout.
    WM_APP_ENDEDIT,                // Note: wParam = kind|commit, lParam = edit.
};

constexpr UINT_PTR TIMER_SAVE = 1;
constexpr UINT_PTR TIMER_WATCH = 2;
constexpr UINT_PTR TIMER_RELAYOUT = 3;
constexpr UINT kSaveDelayMs = 800;
constexpr UINT kWatchIntervalMs = 2000;
constexpr UINT kRelayoutDelayMs = 250;

constexpr UINT_PTR ID_BODY_EDIT = 101;
constexpr UINT_PTR ID_TITLE_EDIT = 102;

constexpr WPARAM ENDEDIT_BODY = 0;
constexpr WPARAM ENDEDIT_TITLE = 1;
constexpr WPARAM ENDEDIT_CANCEL = 0x10;

// All sizes below are in DIPs (pixels at 96 DPI) and are scaled to the
// primary monitor's DPI at layout time.
constexpr int kGapDip = 12;
constexpr int kTitleDip = 30;
constexpr int kStripDip = 24;  // Tool strip below the title bar.
constexpr int kPadDip = 10;
constexpr int kIconDip = 20;
constexpr int kLineGapDip = 2;
constexpr int kCornerDip = 10;
constexpr int kScrollbarDip = 6;
constexpr int kHeightDip[3] = {150, 240, 360};
constexpr const wchar_t* kHeightNames[3] = {L"small", L"medium", L"large"};
constexpr const wchar_t* kHeightLabels[3] = {L"Small", L"Medium", L"Large"};

// Every color a note is drawn with. Built by EffectiveLook() from the
// appearance mode (theme / solid / custom) or the note's own color.
struct Look {
    COLORREF bg;     // Note body.
    COLORREF title;  // Title bar.
    COLORREF strip;  // Tool strip under the title bar.
    COLORREF icon;   // Tool strip icons.
    COLORREF text;
    COLORREF muted;  // Numbers, done items, placeholders.
    COLORREF link;
};
Look g_look{};  // Look of the note being drawn (UI thread only).
constexpr COLORREF kIndicatorColor = RGB(0, 120, 215);

// Preset themes shown in the Themes panel. Colors are the mod's own picks
// based on each theme's name; "Showa Tones" lean on Wada Sanzo-style pairs.
struct Theme {
    const wchar_t* id;
    const wchar_t* name;
    const wchar_t* meaning;  // Tooltip.
    int section;             // Index into kThemeSections.
    COLORREF bg, title, strip, icon, text;
};

struct ThemeSection {
    const wchar_t* heading;
    const wchar_t* subtitle;
};

constexpr ThemeSection kThemeSections[] = {
    {L"Showa Tones", L"Wada colors, 6 themes"},
    {L"Right Now", L"2026 trend colors, 5 themes"},
    {L"Quiet Hours", L"Minimal neutrals, 4 themes + default"},
};
constexpr int kThemeSectionCount = 3;

#define HEXRGB(h) RGB(((h) >> 16) & 0xFF, ((h) >> 8) & 0xFF, (h) & 0xFF)
constexpr Theme kThemes[] = {
    // id, name, meaning, section, bg, title bar, strip, icons, text
    {L"hojicha", L"Hojicha", L"roasted tea and cream", 0,
     HEXRGB(0xEFE3D0), HEXRGB(0xDCC7A8), HEXRGB(0x7A5135), HEXRGB(0xF3E6D2), HEXRGB(0x3B2A1E)},
    {L"yoru-kinari", L"Yoru Kinari", L"night sky and unbleached cloth", 0,
     HEXRGB(0xF1EAD8), HEXRGB(0xE2D8BF), HEXRGB(0x1E2433), HEXRGB(0xE8DFC6), HEXRGB(0x22252E)},
    {L"sakura-sumi", L"Sakura Sumi", L"cherry blossom and ink", 0,
     HEXRGB(0xF7DDE2), HEXRGB(0xEDC6CF), HEXRGB(0x2B2A2E), HEXRGB(0xF7DDE2), HEXRGB(0x2B2A2E)},
    {L"kaki", L"Kaki", L"persimmon and charcoal", 0,
     HEXRGB(0xF9D9B8), HEXRGB(0xF2BF8F), HEXRGB(0xC8572B), HEXRGB(0xFFF1E2), HEXRGB(0x3A2418)},
    {L"matcha-kuri", L"Matcha Kuri", L"green tea and chestnut", 0,
     HEXRGB(0xDDE6C3), HEXRGB(0xC9D6A6), HEXRGB(0x6B4632), HEXRGB(0xE8EFD3), HEXRGB(0x2E3320)},
    {L"ume", L"Ume", L"plum blossom and dusk", 0,
     HEXRGB(0xF2D4DC), HEXRGB(0xE3B6C3), HEXRGB(0x7B2F4A), HEXRGB(0xF7E3E9), HEXRGB(0x3A1E28)},
    {L"kumo", L"Kumo", L"cloud white and slate", 1,
     HEXRGB(0xF2F3F1), HEXRGB(0xE3E6E3), HEXRGB(0x8A97A3), HEXRGB(0xF7F8F7), HEXRGB(0x2D3439)},
    {L"tsuchi", L"Tsuchi", L"earth clay and loam", 1,
     HEXRGB(0xE7D3C1), HEXRGB(0xD7BBA4), HEXRGB(0x8C5A3C), HEXRGB(0xF1E3D6), HEXRGB(0x3A271C)},
    {L"moegi", L"Moegi", L"sprout green and moss", 1,
     HEXRGB(0xE3EFC8), HEXRGB(0xD0E3A8), HEXRGB(0x6E8B3D), HEXRGB(0xF1F7E3), HEXRGB(0x26301A)},
    {L"fuji", L"Fuji", L"wisteria and violet", 1,
     HEXRGB(0xE6DDF2), HEXRGB(0xD5C7EA), HEXRGB(0x6C5A9E), HEXRGB(0xF1ECF8), HEXRGB(0x2B2440)},
    {L"akane", L"Akane", L"madder red and rose", 1,
     HEXRGB(0xF6D5CF), HEXRGB(0xEDB9B0), HEXRGB(0xA63A2E), HEXRGB(0xFBE8E4), HEXRGB(0x3B1A16)},
    {L"shiro", L"Shiro", L"paper white and graphite", 2,
     HEXRGB(0xFAFAF7), HEXRGB(0xEFEFEA), HEXRGB(0xDCDCD5), HEXRGB(0x55554F), HEXRGB(0x2A2A27)},
    {L"hai", L"Hai", L"ash and stone", 2,
     HEXRGB(0xE4E4E1), HEXRGB(0xD5D5D1), HEXRGB(0x9C9C97), HEXRGB(0xF2F2EF), HEXRGB(0x2B2B29)},
    {L"kinu", L"Kinu", L"raw silk and sand", 2,
     HEXRGB(0xF3EDE2), HEXRGB(0xE8DFCE), HEXRGB(0xCDBFA6), HEXRGB(0x5E5444), HEXRGB(0x34302A)},
    {L"sumi", L"Sumi", L"ink black and smoke", 2,
     HEXRGB(0x2A2A2C), HEXRGB(0x333336), HEXRGB(0x404044), HEXRGB(0xC9C9CC), HEXRGB(0xE8E8EA)},
};
#undef HEXRGB
constexpr int kThemeCount = (int)(sizeof(kThemes) / sizeof(kThemes[0]));
// The "Default" theme: no preset, the note look is derived from the
// solidColor setting. Also the fallback for unknown theme ids.
constexpr int kDefaultTheme = -1;
constexpr wchar_t kDefaultThemeId[] = L"default";

enum class AppearanceMode { Theme, Solid, Custom };

// DWM values defined locally so the mod builds against any SDK header.
constexpr DWORD kDwmwaWindowCornerPreference = 33;
constexpr DWORD kDwmwcpRound = 2;

constexpr wchar_t kControllerClass[] = L"WhDesktopStickyNotes_Controller";
constexpr wchar_t kNoteClass[] = L"WhDesktopStickyNotes_Note";
constexpr wchar_t kIndicatorClass[] = L"WhDesktopStickyNotes_Indicator";

struct NamedColor {
    const wchar_t* key;
    const wchar_t* label;
    COLORREF color;
};

constexpr NamedColor kPalette[] = {
    {L"yellow", L"Yellow", RGB(255, 241, 150)},
    {L"green", L"Green", RGB(204, 240, 194)},
    {L"blue", L"Blue", RGB(194, 224, 250)},
    {L"pink", L"Pink", RGB(250, 204, 222)},
    {L"purple", L"Purple", RGB(222, 208, 245)},
    {L"orange", L"Orange", RGB(255, 216, 172)},
    {L"gray", L"Gray", RGB(226, 226, 226)},
};
constexpr int kPaletteCount = (int)(sizeof(kPalette) / sizeof(kPalette[0]));

// ============================================================================
// Data model
// ============================================================================

// Inline formatting bits, per character.
constexpr uint8_t kFmtBold = 1;
constexpr uint8_t kFmtItalic = 2;

struct Line {
    std::wstring text;
    bool checkbox = false;
    bool done = false;
    // One entry per character of `text` (kFmt* bits), or empty when the
    // whole line is unformatted. Saved to JSON as [start, length, flags] spans.
    std::vector<uint8_t> fmt;
};

static uint8_t FmtAt(const Line& l, size_t i) {
    return i < l.fmt.size() ? l.fmt[i] : 0;
}

// Drops the per-character array when nothing is formatted.
static void NormalizeFmt(Line& l) {
    l.fmt.resize(l.text.size(), 0);
    if (std::all_of(l.fmt.begin(), l.fmt.end(), [](uint8_t f) { return f == 0; })) {
        l.fmt.clear();
    }
}

enum class HitKind { Checkbox, Link };

struct Hit {
    RECT rc;
    HitKind kind;
    int line;
    std::wstring url;
};

struct Note {
    // Persisted.
    int id = 0;
    std::wstring title;
    // A note either follows the appearance (theme/solid/custom) or has its
    // own color, picked from right-click > Color.
    bool hasColor = false;
    COLORREF color = RGB(255, 241, 150);
    int col = 0;
    int y = 0;  // Top offset inside the work area, in DIPs.
    int height = 1;  // Index into kHeightDip.
    bool numbering = false;
    std::vector<Line> lines;

    // Runtime only.
    HWND hwnd = nullptr;
    HWND bodyEdit = nullptr;
    HWND titleEdit = nullptr;
    std::wstring titleBeforeEdit;
    bool suppressChange = false;
    // The rich edit holds newer text/formatting than `lines`. Synced lazily
    // (on save and when editing ends), because reading formatting back out
    // of a rich edit costs more than plain text.
    bool editDirty = false;
    // Set while the mod itself hides or destroys the window. Any other hide
    // (Show desktop) is cancelled in WM_WINDOWPOSCHANGING.
    bool hiding = false;
    HBRUSH bgBrush = nullptr;
    HBRUSH titleBrush = nullptr;
    COLORREF brushBg = CLR_INVALID, brushTitle = CLR_INVALID;
    int scroll = 0;
    int contentH = 0;
    std::vector<Hit> hits;
    // Last window shape applied, so the corner region is only rebuilt when
    // the size or layered state actually changes.
    int shapeW = -1, shapeH = -1;
    int shapeLayered = -1;
};

struct Settings {
    // Raw values of the two settings that can also be changed from a note.
    // Compared with the last values seen (stored in notes.json) to decide
    // which side changed last; see SyncFromSettings().
    std::wstring modeValue = L"multiple";
    std::wstring themeValue = kDefaultThemeId;
    int columnWidth = 260;
    bool defaultHasColor = false;  // New notes get their own color.
    COLORREF defaultColor = RGB(255, 241, 150);
    bool customValid = false;  // customColor parsed successfully.
    COLORREF customColor = RGB(255, 241, 150);
    int fontSize = 10;
    int opacity = 100;

    AppearanceMode appearance = AppearanceMode::Theme;
    bool solidValid = false;
    COLORREF solidColor = RGB(255, 241, 150);
    bool jiyuValid = false;   // Both Jiyu colors parsed.
    COLORREF jiyuBase = RGB(244, 236, 216);
    COLORREF jiyuStrip = RGB(59, 91, 122);
    bool optionTextValid = false;
    COLORREF optionText = 0;
    bool optionHoverValid = false;
    COLORREF optionHover = 0;
};

// Geometry of the column grid on the primary monitor, in physical pixels.
struct Geo {
    RECT work{0, 0, 1920, 1080};
    UINT dpi = 96;
    int cols = 1;
    int colW = 260;
    int gap = 12;
    int left = 0;  // X of column 0.
    int top = 0;   // Y that corresponds to note.y == 0.
};

struct Fonts {
    // Index bits: 1 = strikethrough, 2 = underline, 4 = bold, 8 = italic.
    HFONT body[16] = {};
    HFONT title = nullptr;
    HFONT icon = nullptr;
    int lineH = 16;
    UINT dpi = 0;
    int pt = 0;
};

struct DragState {
    Note* note = nullptr;
    POINT grab{};   // Cursor offset from the window's top-left corner.
    POINT start{};  // Cursor position at button-down.
    bool moving = false;
    bool committing = false;
    int targetCol = 0;
    int dropY = 0;  // DIPs.
};

// ============================================================================
// Globals (all touched only on the UI thread, except the thread handles and
// g_controller, which the UI thread writes and Windhawk's thread reads).
// ============================================================================

HINSTANCE g_hinst = nullptr;
HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_readyEvent = nullptr;
bool g_startOk = false;
std::atomic<HWND> g_controller{nullptr};

bool g_active = false;    // Notes loaded and windows created.
bool g_notesLoaded = false;  // notes.json was read (or didn't exist): saving is safe.
bool g_quitting = false;
bool g_dirty = false;
bool g_classNote = false, g_classIndicator = false, g_classController = false;

HWND g_host = nullptr;    // Desktop window the notes are stacked just above.
HWND g_indicator = nullptr;
UINT g_taskbarCreatedMsg = 0;

Settings g_settings;
Geo g_geo;
Fonts g_fonts;
DragState g_drag;
std::vector<std::unique_ptr<Note>> g_notes;
int g_nextId = 1;
std::wstring g_dataDir, g_dataFile;

using SetThreadDpiAwarenessContext_t = void*(WINAPI*)(void*);
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
HMODULE g_shcore = nullptr;
GetDpiForMonitor_t g_pGetDpiForMonitor = nullptr;
HMODULE g_msftedit = nullptr;  // Rich edit (RICHEDIT50W) for the note body.

// ============================================================================
// Small utilities
// ============================================================================

static int S(int dip) {
    return MulDiv(dip, (int)g_geo.dpi, 96);
}

static int ToDip(int px) {
    return MulDiv(px, 96, (int)g_geo.dpi);
}

static COLORREF Shade(COLORREF c, int percent) {
    return RGB(GetRValue(c) * percent / 100, GetGValue(c) * percent / 100,
               GetBValue(c) * percent / 100);
}

// Perceived brightness (ITU-R BT.601 weights), 0..255.
static bool IsDark(COLORREF c) {
    return (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000 < 140;
}

// Blends `pctB` percent of b into a.
static COLORREF Mix(COLORREF a, COLORREF b, int pctB) {
    auto m = [&](int x, int y) { return x + (y - x) * pctB / 100; };
    return RGB(m(GetRValue(a), GetRValue(b)), m(GetGValue(a), GetGValue(b)),
               m(GetBValue(a), GetBValue(b)));
}

// Accent variant of a color for the title bar, strip and scrollbar: darker
// on light colors, lighter on dark ones. A plain darken would vanish on
// near-black custom colors. `percent` < 100 sets the strength.
static COLORREF Tone(COLORREF c, int percent) {
    if (!IsDark(c)) return Shade(c, percent);
    return Mix(c, RGB(255, 255, 255), 100 - percent);
}

// Text, muted and link colors that stay readable on `bg`.
static void SetInk(Look& l, COLORREF text) {
    l.text = text;
    l.muted = Mix(text, l.bg, 45);
    l.link = IsDark(l.bg) ? RGB(138, 190, 255) : RGB(10, 88, 202);
}

static COLORREF AutoText(COLORREF bg) {
    return IsDark(bg) ? RGB(240, 240, 240) : RGB(32, 32, 32);
}

// Whole look derived from one base color, plus an optional strip color.
// Used for the solid color, the Default theme and the Jiyu builder.
static Look LookFromBase(COLORREF bg, COLORREF strip) {
    Look l{};
    l.bg = bg;
    l.title = Tone(bg, 92);
    l.strip = strip == CLR_INVALID ? Tone(bg, 84) : strip;
    l.icon = IsDark(l.strip) ? Mix(l.strip, RGB(255, 255, 255), 85)
                             : Mix(l.strip, RGB(0, 0, 0), 65);
    SetInk(l, AutoText(bg));
    return l;
}

static COLORREF SolidColor() {
    return g_settings.solidValid ? g_settings.solidColor : RGB(255, 241, 168);
}

// kDefaultTheme (-1) is the "Default" theme: the look of the solid color.
static Look LookFromTheme(int i) {
    if (i < 0 || i >= kThemeCount) return LookFromBase(SolidColor(), CLR_INVALID);
    const Theme& t = kThemes[i];
    Look l{};
    l.bg = t.bg;
    l.title = t.title;
    l.strip = t.strip;
    l.icon = t.icon;
    SetInk(l, t.text);
    return l;
}

int g_themeIndex = kDefaultTheme;  // Theme in use (panel or settings).
bool g_multiple = true;            // Mode in use (menu or settings).
// Setting values last seen, persisted in notes.json. When the current
// setting differs, the setting was changed after the note-side choice, so
// it wins ("whichever changed last"). Empty = not known yet.
std::wstring g_seenThemeValue, g_seenModeValue;

static Look EffectiveLook(const Note* n) {
    Look l = LookFromTheme(g_themeIndex);
    switch (g_settings.appearance) {
        case AppearanceMode::Solid:
            // The background and everything derived from it come from the
            // solid color, so a theme picked in the panel has no effect
            // until appearanceMode is "theme" again.
            l = LookFromBase(SolidColor(), CLR_INVALID);
            break;
        case AppearanceMode::Custom:
            if (g_settings.jiyuValid) {
                l = LookFromBase(g_settings.jiyuBase, g_settings.jiyuStrip);
            }
            break;
        case AppearanceMode::Theme:
            break;
    }
    // A note's own color replaces its background only. Title bar, strip and
    // icons stay those of the appearance, and the text is re-picked so it
    // stays readable on the new background.
    if (n && n->hasColor) {
        l.bg = n->color;
        SetInk(l, AutoText(l.bg));
    }
    return l;
}

static int ThemeIndexFromId(const std::wstring& id) {
    for (int i = 0; i < kThemeCount; i++) {
        if (id == kThemes[i].id) return i;
    }
    return kDefaultTheme;  // "default" and unknown ids.
}

static const wchar_t* ThemeId(int i) {
    return i >= 0 && i < kThemeCount ? kThemes[i].id : kDefaultThemeId;
}

static std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) {
        return {};
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    if (n <= 0) {
        Wh_Log(L"MultiByteToWideChar (size) failed, error %u", GetLastError());
        return {};
    }
    std::wstring w(n, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &w[0], n) != n) {
        Wh_Log(L"MultiByteToWideChar failed, error %u", GetLastError());
        return {};
    }
    return w;
}

static std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) {
        return {};
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0,
                                nullptr, nullptr);
    if (n <= 0) {
        Wh_Log(L"WideCharToMultiByte (size) failed, error %u", GetLastError());
        return {};
    }
    std::string s(n, '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &s[0], n,
                            nullptr, nullptr) != n) {
        Wh_Log(L"WideCharToMultiByte failed, error %u", GetLastError());
        return {};
    }
    return s;
}

static std::wstring ColorToHex(COLORREF c) {
    static const wchar_t hex[] = L"0123456789ABCDEF";
    std::wstring s = L"#";
    for (int v : {(int)GetRValue(c), (int)GetGValue(c), (int)GetBValue(c)}) {
        s += hex[(v >> 4) & 0xF];
        s += hex[v & 0xF];
    }
    return s;
}

static bool HexToColor(const std::wstring& s, COLORREF* out) {
    if (s.size() != 7 || s[0] != L'#') {
        return false;
    }
    int v[3];
    for (int i = 0; i < 3; i++) {
        int byte = 0;
        for (int j = 0; j < 2; j++) {
            wchar_t h = s[1 + i * 2 + j];
            byte <<= 4;
            if (h >= L'0' && h <= L'9') byte |= h - L'0';
            else if (h >= L'a' && h <= L'f') byte |= h - L'a' + 10;
            else if (h >= L'A' && h <= L'F') byte |= h - L'A' + 10;
            else return false;
        }
        v[i] = byte;
    }
    *out = RGB(v[0], v[1], v[2]);
    return true;
}

// Forgiving parser for the user-typed custom color: surrounding spaces and
// the leading '#' are optional, and both RRGGBB and the short RGB form
// (expanded like CSS, "38F" -> "3388FF") are accepted.
static bool ParseUserHex(PCWSTR raw, COLORREF* out) {
    if (!raw) return false;
    std::wstring s = raw;
    size_t b = s.find_first_not_of(L" \t"), e = s.find_last_not_of(L" \t");
    if (b == std::wstring::npos) return false;
    s = s.substr(b, e - b + 1);
    if (!s.empty() && s[0] == L'#') s.erase(0, 1);
    if (s.size() == 3) {
        s = std::wstring{s[0], s[0], s[1], s[1], s[2], s[2]};
    }
    return s.size() == 6 && HexToColor(L"#" + s, out);
}

static COLORREF PaletteColor(PCWSTR key) {
    for (const auto& c : kPalette) {
        if (key && wcscmp(key, c.key) == 0) {
            return c.color;
        }
    }
    return kPalette[0].color;
}

// ============================================================================
// Minimal JSON reader/writer (only what notes.json needs)
// ============================================================================

struct JValue {
    enum Type { Null, Bool, Number, String, Array, Object } type = Null;
    bool b = false;
    double num = 0;
    std::wstring str;
    std::vector<JValue> arr;          // Array items.
    std::vector<std::wstring> keys;   // Object keys...
    std::vector<JValue> vals;         // ...and their values.

    const JValue* Find(const wchar_t* key) const {
        if (type != Object) return nullptr;
        for (size_t i = 0; i < keys.size(); i++) {
            if (keys[i] == key) return &vals[i];
        }
        return nullptr;
    }
};

class JsonParser {
   public:
    explicit JsonParser(const std::wstring& s)
        : p_(s.c_str()), end_(s.c_str() + s.size()) {}

    bool Parse(JValue& out) {
        SkipWs();
        if (!Value(out, 0)) return false;
        SkipWs();
        return p_ == end_;
    }

   private:
    const wchar_t* p_;
    const wchar_t* end_;

    void SkipWs() {
        while (p_ < end_ && (*p_ == L' ' || *p_ == L'\t' || *p_ == L'\r' ||
                             *p_ == L'\n')) {
            ++p_;
        }
    }

    bool Lit(const wchar_t* lit) {
        size_t n = wcslen(lit);
        if ((size_t)(end_ - p_) < n || wcsncmp(p_, lit, n) != 0) return false;
        p_ += n;
        return true;
    }

    bool Value(JValue& v, int depth) {
        if (depth > 64 || p_ >= end_) return false;
        switch (*p_) {
            case L'{': return Obj(v, depth);
            case L'[': return Arr(v, depth);
            case L'"': v.type = JValue::String; return Str(v.str);
            case L't': v.type = JValue::Bool; v.b = true; return Lit(L"true");
            case L'f': v.type = JValue::Bool; v.b = false; return Lit(L"false");
            case L'n': v.type = JValue::Null; return Lit(L"null");
            default: return Num(v);
        }
    }

    bool Num(JValue& v) {
        const wchar_t* start = p_;
        while (p_ < end_ && ((*p_ >= L'0' && *p_ <= L'9') || *p_ == L'-' ||
                             *p_ == L'+' || *p_ == L'.' || *p_ == L'e' ||
                             *p_ == L'E')) {
            ++p_;
        }
        if (p_ == start) return false;
        std::wstring t(start, p_);
        wchar_t* e = nullptr;
        v.num = wcstod(t.c_str(), &e);
        if (!e || *e) return false;
        v.type = JValue::Number;
        return true;
    }

    static int HexVal(wchar_t h) {
        if (h >= L'0' && h <= L'9') return h - L'0';
        if (h >= L'a' && h <= L'f') return h - L'a' + 10;
        if (h >= L'A' && h <= L'F') return h - L'A' + 10;
        return -1;
    }

    bool Str(std::wstring& out) {
        ++p_;  // Opening quote.
        while (p_ < end_) {
            wchar_t c = *p_++;
            if (c == L'"') return true;
            if (c != L'\\') {
                out += c;
                continue;
            }
            if (p_ >= end_) return false;
            wchar_t e = *p_++;
            switch (e) {
                case L'"': out += L'"'; break;
                case L'\\': out += L'\\'; break;
                case L'/': out += L'/'; break;
                case L'b': out += L'\b'; break;
                case L'f': out += L'\f'; break;
                case L'n': out += L'\n'; break;
                case L'r': out += L'\r'; break;
                case L't': out += L'\t'; break;
                case L'u': {
                    if (end_ - p_ < 4) return false;
                    unsigned cp = 0;
                    for (int i = 0; i < 4; i++) {
                        int h = HexVal(*p_++);
                        if (h < 0) return false;
                        cp = (cp << 4) | (unsigned)h;
                    }
                    // UTF-16 code units map 1:1 to wchar_t, surrogates included.
                    out += (wchar_t)cp;
                    break;
                }
                default: return false;
            }
        }
        return false;
    }

    bool Arr(JValue& v, int depth) {
        v.type = JValue::Array;
        ++p_;
        SkipWs();
        if (p_ < end_ && *p_ == L']') { ++p_; return true; }
        for (;;) {
            JValue item;
            SkipWs();
            if (!Value(item, depth + 1)) return false;
            v.arr.push_back(std::move(item));
            SkipWs();
            if (p_ >= end_) return false;
            if (*p_ == L',') { ++p_; continue; }
            if (*p_ == L']') { ++p_; return true; }
            return false;
        }
    }

    bool Obj(JValue& v, int depth) {
        v.type = JValue::Object;
        ++p_;
        SkipWs();
        if (p_ < end_ && *p_ == L'}') { ++p_; return true; }
        for (;;) {
            SkipWs();
            if (p_ >= end_ || *p_ != L'"') return false;
            std::wstring key;
            if (!Str(key)) return false;
            SkipWs();
            if (p_ >= end_ || *p_ != L':') return false;
            ++p_;
            SkipWs();
            JValue val;
            if (!Value(val, depth + 1)) return false;
            v.keys.push_back(std::move(key));
            v.vals.push_back(std::move(val));
            SkipWs();
            if (p_ >= end_) return false;
            if (*p_ == L',') { ++p_; continue; }
            if (*p_ == L'}') { ++p_; return true; }
            return false;
        }
    }
};

static void JsonString(std::wstring& out, const std::wstring& s) {
    static const wchar_t hex[] = L"0123456789ABCDEF";
    out += L'"';
    for (wchar_t c : s) {
        switch (c) {
            case L'"': out += L"\\\""; break;
            case L'\\': out += L"\\\\"; break;
            case L'\n': out += L"\\n"; break;
            case L'\r': out += L"\\r"; break;
            case L'\t': out += L"\\t"; break;
            default:
                if (c < 0x20) {
                    out += L"\\u00";
                    out += hex[(c >> 4) & 0xF];
                    out += hex[c & 0xF];
                } else {
                    out += c;
                }
        }
    }
    out += L'"';
}

// ============================================================================
// Settings
// ============================================================================

static void LoadSettings() {
    Settings s;

    // Wh_GetStringSetting never returns NULL (an unset value is L"").
    s.modeValue = wcscmp(WindhawkUtils::StringSetting::make(L"mode"), L"single") == 0
                      ? L"single"
                      : L"multiple";

    {
        auto theme = WindhawkUtils::StringSetting::make(L"theme");
        s.themeValue = *theme.get() ? theme.get() : kDefaultThemeId;
    }

    // Row 2 of the tool panel needs about 220 DIP.
    int cw = Wh_GetIntSetting(L"columnWidth");
    s.columnWidth = cw <= 0 ? 260 : std::clamp(cw, 220, 600);

    {
        auto custom = WindhawkUtils::StringSetting::make(L"customColor");
        s.customValid = ParseUserHex(custom, &s.customColor);
        if (!s.customValid && *custom.get()) {
            Wh_Log(L"Custom color \"%s\" is not a valid hex code (use #RRGGBB)", custom.get());
        }
    }

    {
        auto color = WindhawkUtils::StringSetting::make(L"defaultColor");
        s.defaultHasColor = *color.get() && wcscmp(color, L"theme") != 0;
        if (wcscmp(color, L"custom") == 0) {
            // Invalid or empty hex: new notes follow the appearance instead.
            s.defaultHasColor = s.customValid;
            s.defaultColor = s.customColor;
        } else if (s.defaultHasColor) {
            s.defaultColor = PaletteColor(color);
        }
    }

    int fs = Wh_GetIntSetting(L"fontSize");
    s.fontSize = fs <= 0 ? 10 : std::clamp(fs, 7, 24);

    int op = Wh_GetIntSetting(L"opacity");
    s.opacity = op <= 0 ? 100 : std::clamp(op, 30, 100);

    {
        auto appearance = WindhawkUtils::StringSetting::make(L"appearanceMode");
        if (wcscmp(appearance, L"solid") == 0) {
            s.appearance = AppearanceMode::Solid;
        } else if (wcscmp(appearance, L"custom") == 0) {
            s.appearance = AppearanceMode::Custom;
        }
    }

    // Reads a hex color setting. Empty is allowed (means "automatic"); any
    // other unparsable value is logged.
    auto readHex = [](PCWSTR name, COLORREF* out) {
        auto v = WindhawkUtils::StringSetting::make(name);
        bool ok = ParseUserHex(v, out);
        if (!ok && *v.get()) {
            Wh_Log(L"Setting %s = \"%s\" is not a valid hex code (use #RRGGBB)", name, v.get());
        }
        return ok;
    };
    s.solidValid = readHex(L"solidColor", &s.solidColor);
    bool baseOk = readHex(L"jiyuBaseColor", &s.jiyuBase);
    bool stripOk = readHex(L"jiyuStrip", &s.jiyuStrip);
    s.jiyuValid = baseOk && stripOk;
    s.optionTextValid = readHex(L"optionTextColor", &s.optionText);
    s.optionHoverValid = readHex(L"optionHoverColor", &s.optionHover);
    if (s.appearance == AppearanceMode::Solid && !s.solidValid) {
        Wh_Log(L"Solid mode without a valid solidColor; using the theme");
    }
    if (s.appearance == AppearanceMode::Custom && !s.jiyuValid) {
        Wh_Log(L"Custom (Jiyu) mode needs valid jiyuBaseColor and jiyuStrip; using the theme");
    }

    g_settings = s;
    Wh_Log(L"Settings: mode=%s theme=%s columnWidth=%d fontSize=%d opacity=%d appearance=%d",
           s.modeValue.c_str(), s.themeValue.c_str(), s.columnWidth, s.fontSize, s.opacity,
           (int)s.appearance);
}

// "Whichever changed last wins" for mode and theme. The note side (the "..."
// menu and the Themes panel) writes g_multiple / g_themeIndex directly and
// they are saved in notes.json. The settings side wins only when its value
// differs from the one seen last time, i.e. when it was changed since.
// Returns true if anything was taken over from the settings.
static bool SyncFromSettings() {
    bool changed = false;
    if (g_seenModeValue.empty()) {
        g_seenModeValue = g_settings.modeValue;  // First run: nothing to compare.
        g_multiple = g_settings.modeValue != L"single";
        changed = true;
    } else if (g_seenModeValue != g_settings.modeValue) {
        g_seenModeValue = g_settings.modeValue;
        g_multiple = g_settings.modeValue != L"single";
        changed = true;
    }
    if (g_seenThemeValue.empty()) {
        // No record of the setting yet: keep whatever theme was loaded and
        // just remember the setting.
        g_seenThemeValue = g_settings.themeValue;
        changed = true;
    } else if (g_seenThemeValue != g_settings.themeValue) {
        g_seenThemeValue = g_settings.themeValue;
        g_themeIndex = ThemeIndexFromId(g_settings.themeValue);
        changed = true;
    }
    return changed;
}

// ============================================================================
// Line text helpers (checkbox syntax, URL detection)
// ============================================================================

// `prefixLen` receives how many leading characters of `raw` were the
// checkbox marker, so formatting read from the editor can be aligned with
// `text`.
static Line ParseLine(const std::wstring& raw, size_t* prefixLen = nullptr) {
    Line l;
    size_t prefix = 0;
    // "[ ] item" is an open checkbox, "[x] item" a done one.
    if (raw.size() >= 3 && raw[0] == L'[' && raw[2] == L']' &&
        (raw[1] == L' ' || raw[1] == L'x' || raw[1] == L'X')) {
        l.checkbox = true;
        l.done = raw[1] != L' ';
        prefix = (raw.size() > 3 && raw[3] == L' ') ? 4 : 3;
    }
    l.text = raw.substr(prefix);
    if (prefixLen) *prefixLen = prefix;
    return l;
}

// Splits on "\r\n", "\r" or "\n" (rich edit uses a bare "\r").
static std::vector<std::wstring> SplitRawLines(const std::wstring& text) {
    std::vector<std::wstring> out;
    size_t start = 0;
    for (;;) {
        size_t nl = text.find_first_of(L"\r\n", start);
        out.push_back(text.substr(start, nl == std::wstring::npos ? std::wstring::npos
                                                                   : nl - start));
        if (nl == std::wstring::npos) break;
        start = nl + 1;
        if (text[nl] == L'\r' && start < text.size() && text[start] == L'\n') ++start;
    }
    return out;
}

static std::vector<Line> ParseLines(const std::wstring& text) {
    std::vector<Line> out;
    for (const std::wstring& raw : SplitRawLines(text)) out.push_back(ParseLine(raw));
    return out;
}

static std::wstring LinePrefix(const Line& l) {
    return l.checkbox ? (l.done ? L"[x] " : L"[ ] ") : L"";
}

// Rich edit paragraphs end with a single '\r'; character positions count it
// as one character, which the formatting offsets rely on.
static std::wstring SerializeLines(const std::vector<Line>& lines) {
    std::wstring s;
    for (size_t i = 0; i < lines.size(); i++) {
        if (i) s += L"\r";
        s += LinePrefix(lines[i]);
        s += lines[i].text;
    }
    return s;
}

static bool AllLinesAreCheckboxes(const std::vector<Line>& lines) {
    return !lines.empty() && std::all_of(lines.begin(), lines.end(), [](const Line& l) {
        return l.checkbox || l.text.empty();
    });
}

static void TrimTrailingEmptyLines(std::vector<Line>& lines) {
    while (!lines.empty() && lines.back().text.empty() &&
           !lines.back().checkbox) {
        lines.pop_back();
    }
}

struct Run {
    std::wstring text;
    bool link = false;
    std::wstring url;
    size_t start = 0;   // Offset of `text` inside the line.
    uint8_t fmt = 0;    // kFmt* bits, uniform over the run.
};

static bool StartsWithI(const std::wstring& s, size_t i, const wchar_t* p) {
    size_t n = wcslen(p);
    return s.size() - i >= n && _wcsnicmp(s.c_str() + i, p, n) == 0;
}

static bool IsCharIn(wchar_t c, const wchar_t* set) {
    return c && wcschr(set, c);
}

// Splits a line into plain and link runs. A URL starts with http://,
// https:// or www. at a word boundary and runs until whitespace. Trailing
// punctuation is not part of the URL.
static std::vector<Run> SplitUrls(const std::wstring& s) {
    std::vector<Run> runs;
    size_t i = 0, plainStart = 0;
    while (i < s.size()) {
        bool boundary = i == 0 || iswspace(s[i - 1]) ||
                        IsCharIn(s[i - 1], L"([<\"'");
        size_t prefix = 0;
        if (boundary) {
            if (StartsWithI(s, i, L"https://")) prefix = 8;
            else if (StartsWithI(s, i, L"http://")) prefix = 7;
            else if (StartsWithI(s, i, L"www.")) prefix = 4;
        }
        if (prefix) {
            size_t j = i;
            while (j < s.size() && !iswspace(s[j]) && !IsCharIn(s[j], L"<>\"")) {
                ++j;
            }
            while (j > i + prefix && IsCharIn(s[j - 1], L".,;:!?)]}'")) {
                --j;
            }
            if (j > i + prefix) {
                if (i > plainStart) {
                    runs.push_back({s.substr(plainStart, i - plainStart), false, {}, plainStart});
                }
                std::wstring shown = s.substr(i, j - i);
                std::wstring url = prefix == 4 ? L"http://" + shown : shown;
                runs.push_back({shown, true, url, i});
                i = plainStart = j;
                continue;
            }
        }
        ++i;
    }
    if (plainStart < s.size()) {
        runs.push_back({s.substr(plainStart), false, {}, plainStart});
    }
    return runs;
}

// URL runs further split wherever bold/italic changes, so every run can be
// drawn with a single font.
static std::vector<Run> StyledRuns(const Line& line) {
    std::vector<Run> out;
    for (const Run& r : SplitUrls(line.text)) {
        size_t a = 0;
        while (a < r.text.size()) {
            uint8_t f = FmtAt(line, r.start + a);
            size_t b = a + 1;
            while (b < r.text.size() && FmtAt(line, r.start + b) == f) ++b;
            out.push_back({r.text.substr(a, b - a), r.link, r.url, r.start + a, f});
            a = b;
        }
    }
    return out;
}

static void OpenUrl(const std::wstring& url) {
    // Only http/https URLs ever reach here (see SplitUrls).
    HINSTANCE r = ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr,
                                SW_SHOWNORMAL);
    if ((INT_PTR)r <= 32) {
        Wh_Log(L"ShellExecuteW failed for %s, code %d", url.c_str(), (int)(INT_PTR)r);
    }
}

// ============================================================================
// Persistence
// ============================================================================

static std::wstring CurrentUserSid() {
    std::wstring result;
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"OpenProcessToken failed, error %u", GetLastError());
        return result;
    }
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buf(size);
    if (size && GetTokenInformation(token, TokenUser, buf.data(), size, &size)) {
        LPWSTR sid = nullptr;
        if (ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &sid)) {
            result = sid;
            LocalFree(sid);
        } else {
            Wh_Log(L"ConvertSidToStringSidW failed, error %u", GetLastError());
        }
    } else {
        Wh_Log(L"GetTokenInformation failed, error %u", GetLastError());
    }
    CloseHandle(token);
    return result;
}

// Creates `dir` if it doesn't exist. With `sddl`, a new folder gets that
// security descriptor instead of inheriting the parent's permissions. The
// permissions of a folder that already exists are never changed.
static bool EnsureDirectory(const std::wstring& dir, PCWSTR sddl = nullptr) {
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, FALSE};
    PSECURITY_DESCRIPTOR sd = nullptr;
    if (sddl && ConvertStringSecurityDescriptorToSecurityDescriptorW(
                    sddl, SDDL_REVISION_1, &sd, nullptr)) {
        sa.lpSecurityDescriptor = sd;
    } else if (sddl) {
        Wh_Log(L"Couldn't build the folder permissions, error %u; using the default",
               GetLastError());
    }
    bool ok = CreateDirectoryW(dir.c_str(), sd ? &sa : nullptr) ||
              GetLastError() == ERROR_ALREADY_EXISTS;
    if (!ok) {
        Wh_Log(L"CreateDirectoryW(%s) failed, error %u", dir.c_str(), GetLastError());
    }
    if (sd) LocalFree(sd);
    return ok;
}

// The notes live in the mod's own storage folder, which Windhawk removes
// together with the mod. That folder is shared by all Windows users, so the
// notes go into a subfolder named after the current user's SID, created so
// that only that user, SYSTEM and administrators can open it. The paths are
// only set once the folder exists, so nothing is ever saved to a location
// that wasn't verified.
static bool InitPaths() {
    g_dataDir.clear();
    g_dataFile.clear();
    wchar_t buf[MAX_PATH];
    if (!Wh_GetModStoragePath(buf, ARRAYSIZE(buf))) {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return false;
    }
    std::wstring dir = buf;
    if (!EnsureDirectory(dir)) return false;
    // No fallback to the shared folder: notes saved there would seem to
    // vanish once the SID can be read again. The watchdog retries instead.
    std::wstring sid = CurrentUserSid();
    if (sid.empty()) return false;
    dir += L"\\" + sid;
    // Protected DACL (no inheritance from the shared folder): full access for
    // this user, SYSTEM and administrators (so Windhawk can still remove the
    // folder together with the mod).
    std::wstring sddl = L"D:P(A;OICI;FA;;;" + sid + L")(A;OICI;FA;;;SY)(A;OICI;FA;;;BA)";
    if (!EnsureDirectory(dir, sddl.c_str())) return false;
    g_dataDir = dir;
    g_dataFile = dir + L"\\notes.json";
    return true;
}

static bool ReadWholeFile(const std::wstring& path, std::string& out,
                          bool* notFound) {
    *notFound = false;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        DWORD e = GetLastError();
        if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) {
            *notFound = true;
        } else {
            Wh_Log(L"CreateFileW(%s) for reading failed, error %u", path.c_str(), e);
        }
        return false;
    }
    bool ok = false;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(h, &size)) {
        Wh_Log(L"GetFileSizeEx failed, error %u", GetLastError());
    } else if (size.QuadPart > 16 * 1024 * 1024) {
        Wh_Log(L"notes.json is unreasonably large (%lld bytes), ignoring",
               size.QuadPart);
    } else {
        out.resize((size_t)size.QuadPart);
        DWORD read = 0;
        if (out.empty()) {
            ok = true;
        } else if (!ReadFile(h, &out[0], (DWORD)out.size(), &read, nullptr) ||
                   read != out.size()) {
            Wh_Log(L"ReadFile failed, error %u", GetLastError());
        } else {
            ok = true;
        }
    }
    WIN_CHECK(CloseHandle(h));
    return ok;
}

static std::unique_ptr<Note> MakeNote() {
    auto n = std::make_unique<Note>();
    n->id = g_nextId++;
    n->hasColor = g_settings.defaultHasColor;
    n->color = g_settings.defaultColor;
    n->title = L"Note " + std::to_wstring(n->id);
    n->height = 1;
    return n;
}

// Converts a JSON number to int, clamped to [lo, hi]. A plain (int) cast is
// undefined for NaN and out-of-range values, which a damaged or hand-edited
// file could contain.
static int ToIntClamped(double d, int lo, int hi) {
    if (!(d >= lo)) return lo;  // Also catches NaN.
    if (d > hi) return hi;
    return (int)d;
}

static std::unique_ptr<Note> NoteFromJson(const JValue& j, int* order) {
    if (j.type != JValue::Object) return nullptr;
    auto n = std::make_unique<Note>();

    if (auto v = j.Find(L"id"); v && v->type == JValue::Number) {
        n->id = ToIntClamped(v->num, 0, 1000000000);
    }
    if (auto v = j.Find(L"title"); v && v->type == JValue::String) n->title = v->str;
    // A missing or null color means the note follows the appearance.
    if (auto v = j.Find(L"color"); v && v->type == JValue::String) {
        n->hasColor = HexToColor(v->str, &n->color);
    }
    if (auto v = j.Find(L"column"); v && v->type == JValue::Number) {
        n->col = ToIntClamped(v->num, 0, 1000);
    }
    if (auto v = j.Find(L"y"); v && v->type == JValue::Number) {
        n->y = ToIntClamped(v->num, 0, 100000);
    }
    if (auto v = j.Find(L"order"); v && v->type == JValue::Number) {
        *order = ToIntClamped(v->num, 0, 1000000);
    }
    if (auto v = j.Find(L"height")) {
        if (v->type == JValue::String) {
            for (int i = 0; i < 3; i++) {
                if (v->str == kHeightNames[i]) n->height = i;
            }
        } else if (v->type == JValue::Number) {
            n->height = ToIntClamped(v->num, 0, 2);
        }
    }
    if (auto v = j.Find(L"numbering"); v && v->type == JValue::Bool) {
        n->numbering = v->b;
    }
    if (auto v = j.Find(L"lines"); v && v->type == JValue::Array) {
        for (const auto& lj : v->arr) {
            Line l;
            if (lj.type == JValue::String) {
                l.text = lj.str;
            } else if (lj.type == JValue::Object) {
                if (auto t = lj.Find(L"text"); t && t->type == JValue::String) l.text = t->str;
                if (auto c = lj.Find(L"checkbox"); c && c->type == JValue::Bool) l.checkbox = c->b;
                if (auto d = lj.Find(L"done"); d && d->type == JValue::Bool) l.done = d->b;
            } else {
                continue;
            }
            // Embedded newlines would break the one-item-per-line model.
            std::replace(l.text.begin(), l.text.end(), L'\n', L' ');
            std::replace(l.text.begin(), l.text.end(), L'\r', L' ');
            // Bold/italic spans: [[start, length, flags], ...].
            if (const JValue* sp = lj.Find(L"spans"); sp && sp->type == JValue::Array) {
                l.fmt.assign(l.text.size(), 0);
                for (const JValue& s : sp->arr) {
                    if (s.type != JValue::Array || s.arr.size() != 3 ||
                        s.arr[0].type != JValue::Number || s.arr[1].type != JValue::Number ||
                        s.arr[2].type != JValue::Number) {
                        continue;
                    }
                    int textLen = (int)l.text.size();
                    int a = ToIntClamped(s.arr[0].num, 0, textLen);
                    int len = ToIntClamped(s.arr[1].num, 0, textLen);
                    uint8_t f = (uint8_t)(ToIntClamped(s.arr[2].num, 0, 255) &
                                          (kFmtBold | kFmtItalic));
                    for (int k = a; k < a + len && k < textLen; k++) {
                        l.fmt[(size_t)k] = f;
                    }
                }
                NormalizeFmt(l);
            }
            n->lines.push_back(std::move(l));
        }
    }
    return n;
}

// Returns false if notes.json exists but couldn't be read or backed up. The
// caller must then not activate (and nothing is ever saved), because carrying
// on with an empty set of notes would overwrite the real file at the next
// autosave. The watchdog retries a moment later.
static bool LoadNotes() {
    g_notes.clear();
    g_nextId = 1;

    std::string bytes;
    bool notFound = false;
    std::vector<std::pair<int, std::unique_ptr<Note>>> loaded;

    bool haveFile = ReadWholeFile(g_dataFile, bytes, &notFound);
    if (!haveFile && !notFound) {
        Wh_Log(L"notes.json exists but couldn't be read; retrying later");
        return false;
    }

    if (haveFile) {
        if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF &&
            (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF) {
            bytes.erase(0, 3);
        }
        std::wstring text = Utf8ToWide(bytes);
        JValue root;
        JsonParser parser(text);
        const JValue* arr = nullptr;
        if (parser.Parse(root) && (arr = root.Find(L"notes")) &&
            arr->type == JValue::Array) {
            g_themeIndex = kDefaultTheme;
            if (auto ap = root.Find(L"appearance")) {
                if (auto t = ap->Find(L"theme"); t && t->type == JValue::String) {
                    g_themeIndex = ThemeIndexFromId(t->str);
                    if (t->str != ThemeId(g_themeIndex)) {
                        Wh_Log(L"Unknown or removed theme id \"%s\"; using %s",
                               t->str.c_str(), ThemeId(g_themeIndex));
                    }
                }
                if (auto t = ap->Find(L"seenSettingTheme"); t && t->type == JValue::String) {
                    g_seenThemeValue = t->str;
                }
            }
            if (auto m = root.Find(L"mode"); m && m->type == JValue::String) {
                g_multiple = m->str != L"single";
            }
            if (auto m = root.Find(L"seenSettingMode"); m && m->type == JValue::String) {
                g_seenModeValue = m->str;
            }
            int idx = 0;
            for (const auto& nj : arr->arr) {
                int order = idx++;
                if (auto n = NoteFromJson(nj, &order)) {
                    loaded.emplace_back(order, std::move(n));
                }
            }
        } else {
            // Keep the user's data: back up the broken file before our next
            // save replaces it. If the backup fails, don't continue.
            std::wstring bak = g_dataDir + L"\\notes.corrupt.json";
            Wh_Log(L"notes.json could not be parsed; backing it up to %s",
                   bak.c_str());
            if (!CopyFileW(g_dataFile.c_str(), bak.c_str(), FALSE)) {
                Wh_Log(L"Backup failed, error %u; retrying later", GetLastError());
                g_notes.clear();
                return false;
            }
            g_notes.clear();
            g_nextId = 1;
        }
    }

    std::stable_sort(loaded.begin(), loaded.end(),
                     [](const auto& a, const auto& b) { return a.first < b.first; });

    // Fix up missing or duplicate ids.
    for (auto& [order, n] : loaded) {
        g_nextId = std::max(g_nextId, n->id + 1);
    }
    std::vector<int> seen;
    for (auto& [order, n] : loaded) {
        if (n->id <= 0 || std::find(seen.begin(), seen.end(), n->id) != seen.end()) {
            n->id = g_nextId++;
        }
        seen.push_back(n->id);
        g_notes.push_back(std::move(n));
    }

    if (!haveFile) {
        // No notes.json yet: start from the settings for both theme and mode.
        g_themeIndex = ThemeIndexFromId(g_settings.themeValue);
        g_seenThemeValue = g_settings.themeValue;
        g_multiple = g_settings.modeValue != L"single";
        g_seenModeValue = g_settings.modeValue;
    }

    if (g_notes.empty()) {
        auto n = MakeNote();
        if (!haveFile) {
            // First run: a short self-explaining note.
            n->title = L"Sticky notes";
            n->lines = ParseLines(
                L"Click here to edit this note\r\n"
                L"[ ] Start a line with [ ] to make a checkbox\r\n"
                L"[x] Click a box to tick it off\r\n"
                L"Links open in your browser: https://windhawk.net\r\n"
                L"Click ... > Themes... to restyle every note");
        }
        g_notes.push_back(std::move(n));
    }
    g_notesLoaded = true;  // From here on, saving is allowed.
    Wh_Log(L"Loaded %d note(s)", (int)g_notes.size());
    return true;
}

static std::wstring SerializeNotes() {
    std::wstring o = L"{\n  \"version\": 1,\n  \"appearance\": {\"theme\": \"";
    o += ThemeId(g_themeIndex);
    o += L"\", \"seenSettingTheme\": ";
    JsonString(o, g_seenThemeValue);
    o += L"},\n  \"mode\": \"";
    o += g_multiple ? L"multiple" : L"single";
    o += L"\", \"seenSettingMode\": ";
    JsonString(o, g_seenModeValue);
    o += L",\n  \"notes\": [";
    for (size_t i = 0; i < g_notes.size(); i++) {
        const Note* n = g_notes[i].get();
        o += i ? L",\n    {" : L"\n    {";
        o += L"\"id\": " + std::to_wstring(n->id);
        o += L", \"title\": ";
        JsonString(o, n->title);
        o += n->hasColor ? L", \"color\": \"" + ColorToHex(n->color) + L"\""
                         : std::wstring(L", \"color\": null");
        o += L", \"column\": " + std::to_wstring(n->col);
        o += L", \"y\": " + std::to_wstring(n->y);
        o += L", \"order\": " + std::to_wstring(i);
        o += L", \"height\": \"" + std::wstring(kHeightNames[n->height]) + L"\"";
        o += L", \"numbering\": ";
        o += n->numbering ? L"true" : L"false";
        o += L",\n      \"lines\": [";
        for (size_t j = 0; j < n->lines.size(); j++) {
            const Line& l = n->lines[j];
            o += j ? L",\n        {\"text\": " : L"\n        {\"text\": ";
            JsonString(o, l.text);
            o += L", \"checkbox\": ";
            o += l.checkbox ? L"true" : L"false";
            o += L", \"done\": ";
            o += l.done ? L"true" : L"false";
            if (!l.fmt.empty()) {
                // Run-length encode the per-character flags.
                o += L", \"spans\": [";
                bool first = true;
                for (size_t a = 0; a < l.fmt.size();) {
                    size_t b = a + 1;
                    while (b < l.fmt.size() && l.fmt[b] == l.fmt[a]) ++b;
                    if (l.fmt[a]) {
                        o += first ? L"[" : L", [";
                        o += std::to_wstring(a) + L", " + std::to_wstring(b - a) + L", " +
                             std::to_wstring(l.fmt[a]) + L"]";
                        first = false;
                    }
                    a = b;
                }
                o += L"]";
            }
            o += L"}";
        }
        o += n->lines.empty() ? L"]}" : L"\n      ]}";
    }
    o += L"\n  ]\n}\n";
    return o;
}

// Writes notes.json atomically: write a temp file, flush it, then rename it
// over the real file. A crash at any point leaves either the old or the new
// file, never a truncated one.
static void SyncEditorsToModel();
static void ScheduleSave();

static bool SaveNow() {
    if (g_controller) {
        KillTimer(g_controller, TIMER_SAVE);  // Fails harmlessly if not set.
    }
    // Never write before notes.json has been read (or confirmed missing), or
    // an unread file would be replaced by an empty set of notes.
    if (g_dataFile.empty() || !g_notesLoaded) return false;
    SyncEditorsToModel();  // Pull text and formatting out of open editors.

    std::string data = WideToUtf8(SerializeNotes());
    if (data.empty()) {
        Wh_Log(L"Serialization produced no data; not saving");
        return false;
    }
    std::wstring tmp = g_dataFile + L".tmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateFileW(%s) failed, error %u", tmp.c_str(), GetLastError());
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(h, data.data(), (DWORD)data.size(), &written, nullptr) &&
              written == data.size();
    if (!ok) {
        Wh_Log(L"WriteFile failed, error %u", GetLastError());
    } else {
        WIN_CHECK(FlushFileBuffers(h));
    }
    WIN_CHECK(CloseHandle(h));
    if (!ok) {
        WIN_CHECK(DeleteFileW(tmp.c_str()));
        return false;
    }
    if (!MoveFileExW(tmp.c_str(), g_dataFile.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"MoveFileExW failed, error %u", GetLastError());
        WIN_CHECK(DeleteFileW(tmp.c_str()));
        return false;
    }
    g_dirty = false;
    return true;
}

// Autosave: coalesces bursts of edits into one write shortly afterwards.
static void ScheduleSave() {
    g_dirty = true;
    if (!g_controller || !SetTimer(g_controller, TIMER_SAVE, kSaveDelayMs, nullptr)) {
        Wh_Log(L"SetTimer(save) failed, error %u; saving immediately", GetLastError());
        SaveNow();
    }
}

// ============================================================================
// Geometry, DPI and fonts
// ============================================================================

static UINT GetMonitorDpi(HMONITOR mon) {
    if (g_pGetDpiForMonitor && mon) {
        UINT x = 0, y = 0;
        // 0 == MDT_EFFECTIVE_DPI.
        HRESULT hr = g_pGetDpiForMonitor(mon, 0, &x, &y);
        if (SUCCEEDED(hr) && x) return x;
        Wh_Log(L"GetDpiForMonitor failed, hr=0x%08X", (unsigned)hr);
    }
    HDC dc = WIN_CHECK(GetDC(nullptr));
    if (!dc) return 96;
    int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    WIN_CHECK(ReleaseDC(nullptr, dc));
    return dpi > 0 ? (UINT)dpi : 96;
}

static Geo ComputeGeo() {
    Geo g;
    HMONITOR mon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        g.work = mi.rcWork;
    } else {
        Wh_Log(L"GetMonitorInfoW failed, error %u", GetLastError());
        if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &g.work, 0)) {
            Wh_Log(L"SPI_GETWORKAREA failed, error %u", GetLastError());
            g.work = RECT{0, 0, 1920, 1080};
        }
    }
    g.dpi = GetMonitorDpi(mon);
    g.colW = MulDiv(g_settings.columnWidth, (int)g.dpi, 96);
    g.gap = MulDiv(kGapDip, (int)g.dpi, 96);
    // floor(available width / column stride), at least one column. The gap
    // on the left edge is not part of the available width.
    int avail = (g.work.right - g.work.left) - g.gap;
    g.cols = std::max(1, avail / (g.colW + g.gap));
    g.left = g.work.left + g.gap;
    g.top = g.work.top + g.gap;
    return g;
}

static bool SameGeo(const Geo& a, const Geo& b) {
    return EqualRect(&a.work, &b.work) && a.dpi == b.dpi && a.cols == b.cols &&
           a.colW == b.colW;
}

static int ColumnX(int col) {
    return g_geo.left + col * (g_geo.colW + g_geo.gap);
}

// Snap: the column whose left edge is nearest to x. A note can never sit
// between columns because its x is always derived from this.
static int NearestColumn(int x) {
    int stride = g_geo.colW + g_geo.gap;
    int c = (int)std::lround((double)(x - g_geo.left) / stride);
    return std::clamp(c, 0, g_geo.cols - 1);
}

static void DeleteFonts(Fonts& f) {
    for (HFONT& h : f.body) {
        if (h) WIN_CHECK(DeleteObject(h));
        h = nullptr;
    }
    if (f.title) WIN_CHECK(DeleteObject(f.title));
    if (f.icon) WIN_CHECK(DeleteObject(f.icon));
    f.title = f.icon = nullptr;
}

static HFONT MakeFont(int heightPx, int weight, bool underline, bool strike,
                      bool italic = false) {
    HFONT f = CreateFontW(-heightPx, 0, 0, 0, weight, italic, underline, strike,
                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                          L"Segoe UI");
    if (!f) {
        Wh_Log(L"CreateFontW failed, error %u", GetLastError());
    }
    return f;
}

// (Re)creates every font for the current DPI and font size. Live edit
// controls are switched to the new fonts before the old ones are deleted.
static void CreateFonts() {
    Fonts f;
    f.dpi = g_geo.dpi;
    f.pt = g_settings.fontSize;
    int px = MulDiv(f.pt, (int)f.dpi, 72);
    for (int i = 0; i < 16; i++) {
        f.body[i] = MakeFont(px, (i & 4) ? FW_BOLD : FW_NORMAL, (i & 2) != 0, (i & 1) != 0,
                             (i & 8) != 0);
    }
    f.title = MakeFont(px, FW_SEMIBOLD, false, false);
    f.icon = MakeFont(px * 85 / 100, FW_BOLD, false, false);

    f.lineH = px + px / 3;
    if (HDC dc = WIN_CHECK(GetDC(nullptr))) {
        HGDIOBJ old = SelectObject(dc, f.body[0]);
        TEXTMETRICW tm;
        if (WIN_CHECK(GetTextMetricsW(dc, &tm))) {
            f.lineH = tm.tmHeight;
        }
        SelectObject(dc, old);
        WIN_CHECK(ReleaseDC(nullptr, dc));
    }

    for (auto& n : g_notes) {
        // The body rich edit keeps face/size in its character formats (see
        // ApplyEditorStyle), so only the plain title edit needs the HFONT.
        if (n->titleEdit) SendMessageW(n->titleEdit, WM_SETFONT, (WPARAM)f.title, TRUE);
    }
    DeleteFonts(g_fonts);
    g_fonts = f;
}

// ============================================================================
// Desktop host discovery
// ============================================================================

static BOOL CALLBACK FindDefViewWorkerProc(HWND hwnd, LPARAM lParam) {
    wchar_t cls[32];
    if (GetClassNameW(hwnd, cls, 32) && wcscmp(cls, L"WorkerW") == 0 &&
        FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
        *(HWND*)lParam = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Returns the top-level window that currently shows the desktop icons. The
// notes are unowned top-level windows that are kept stacked directly above
// this window (see DesktopInsertAfter), so they sit just above the wallpaper
// and icons and below every application window. They are deliberately not
// owned by it: the mod runs in its own process, and an owner relationship
// across processes would attach the input queues of the two.
//
// The hierarchy differs between Windows versions:
//  * Classic, and Windows 11 24H2+: Progman hosts SHELLDLL_DefView directly.
//    On 24H2+ the wallpaper WorkerW is a child of Progman.
//  * Windows 10 / early 11 after Progman receives message 0x052C (used by
//    the slideshow, animated wallpapers and wallpaper engines):
//    SHELLDLL_DefView moves into a separate top-level WorkerW, which then
//    covers Progman. Stacking the notes above Progman would hide them behind
//    that WorkerW, so the WorkerW is the reference window.
// Fallbacks: Progman without DefView, then GetShellWindow(). If none exists
// (the shell is starting), the caller keeps its previous host and retries.
static HWND FindDesktopHost() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman && FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr)) {
        return progman;
    }
    HWND worker = nullptr;
    EnumWindows(FindDefViewWorkerProc, (LPARAM)&worker);  // FALSE = found it.
    if (worker) return worker;
    if (progman) return progman;
    return GetShellWindow();
}

// Integrity level of a token (SECURITY_MANDATORY_*_RID), or 0 on failure.
static DWORD TokenIntegrity(HANDLE token) {
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    if (!size) return 0;
    std::vector<BYTE> buf(size);
    if (!GetTokenInformation(token, TokenIntegrityLevel, buf.data(), size, &size)) return 0;
    PSID sid = ((TOKEN_MANDATORY_LABEL*)buf.data())->Label.Sid;
    UCHAR count = *GetSidSubAuthorityCount(sid);
    return count ? *GetSidSubAuthority(sid, count - 1) : 0;
}

// SetWindowPos fails with "access denied" when hwndInsertAfter belongs to a
// process of higher integrity (UIPI), for example the hidden helper windows of
// elevated or service processes that can sit right above the desktop. This
// tells whether a window of process `pid` can be used as an anchor.
static bool CanAnchorTo(DWORD pid) {
    if (pid == GetCurrentProcessId()) return true;
    static DWORD ownLevel = [] {
        HANDLE token = nullptr;
        DWORD level = 0;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            level = TokenIntegrity(token);
            CloseHandle(token);
        }
        return level;
    }();
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    bool ok = false;
    HANDLE token = nullptr;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        DWORD level = TokenIntegrity(token);
        ok = level != 0 && level <= ownLevel;
        CloseHandle(token);
    }
    CloseHandle(process);
    return ok;
}

// Works out where `hwnd` has to go to be at desktop level. Returns false if
// it already is (or can't be moved); otherwise returns true and stores the
// hwndInsertAfter value to pass to SetWindowPos in `*after`.
//
// Desktop level means just above the desktop window and below every visible
// window of other processes. SetWindowPos inserts a window below
// hwndInsertAfter, so the anchor is the lowest visible window of another
// process: the notes go directly under it. Hidden windows between it and the
// desktop don't matter visually, and anchoring to the window directly above
// the desktop would often fail (see CanAnchorTo). HWND_BOTTOM is avoided
// because it would drop the note behind the desktop.
//
// If the lowest visible window is topmost (or there is none), no normal
// window is above the desktop, and `*after` is HWND_TOP: for a non-topmost
// window that means the top of the normal band, below all topmost windows.
// (HWND_NOTOPMOST would do nothing here, since the notes aren't topmost.)
// Note that HWND_TOP is a null handle, which is why "needs to move" is a
// separate return value instead of a null `*after`.
static bool DesktopInsertAfter(HWND hwnd, HWND* after) {
    if (!g_host || !IsWindow(g_host)) return false;
    const DWORD ownPid = GetCurrentProcessId();
    for (HWND w = GetWindow(g_host, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV)) {
        if (w == hwnd) return false;  // Already below every visible window.
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid == ownPid || !IsWindowVisible(w)) continue;
        // `w` is the lowest visible window of another process.
        if (GetWindowLongPtrW(w, GWL_EXSTYLE) & WS_EX_TOPMOST) {
            *after = HWND_TOP;
            return true;
        }
        if (CanAnchorTo(pid)) {
            *after = w;
            return true;
        }
        // Can't anchor to it: use the nearest usable window below it instead.
        for (HWND d = GetWindow(w, GW_HWNDNEXT); d && d != g_host; d = GetWindow(d, GW_HWNDNEXT)) {
            if (d == hwnd) return false;
            DWORD dpid = 0;
            GetWindowThreadProcessId(d, &dpid);
            if (CanAnchorTo(dpid)) {
                *after = d;
                return true;
            }
        }
        return false;
    }
    *after = HWND_TOP;  // Nothing visible above the desktop.
    return true;
}

static void PlaceAboveHost(HWND hwnd) {
    HWND after = nullptr;
    if (!DesktopInsertAfter(hwnd, &after)) return;
    WIN_CHECK(SetWindowPos(hwnd, after, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
}

// ============================================================================
// Note geometry and hit testing
// ============================================================================

static bool IsVisibleNote(const Note* n) {
    return g_multiple || (!g_notes.empty() && g_notes[0].get() == n);
}

static RECT NoteRect(const Note* n) {
    RECT r;
    r.left = ColumnX(n->col);
    r.top = g_geo.top + S(n->y);
    r.right = r.left + g_geo.colW;
    r.bottom = r.top + S(kHeightDip[n->height]);
    return r;
}

static RECT TitleRect(const RECT& client) {
    return RECT{0, 0, client.right, S(kTitleDip)};
}

// The tool panel has two rows:
//  * Row 1, the title bar: the title (drag handle, double-click to rename)
//    and the "..." menu button on the right.
//  * Row 2, the tool strip: numbering, checklist | Bold, Italic | and the
//    Color circle on the right. Its buttons need about 220 DIP in total,
//    which is why columnWidth can't go below 220.
static RECT StripRect(const RECT& client) {
    return RECT{0, S(kTitleDip), client.right, S(kTitleDip) + S(kStripDip)};
}

enum StripSlot { SLOT_NUMBER, SLOT_CHECKLIST, SLOT_BOLD, SLOT_ITALIC };

static RECT StripIconRect(const RECT& client, int slot) {
    (void)client;
    int sz = S(kIconDip);
    int top = S(kTitleDip) + (S(kStripDip) - sz) / 2;
    // A wider gap separates the list group from the text-format group.
    int left = S(kPadDip) / 2 + slot * (sz + S(6)) + (slot >= SLOT_BOLD ? S(10) : 0);
    return RECT{left, top, left + sz, top + sz};
}

static RECT NumIconRect(const RECT& client) { return StripIconRect(client, SLOT_NUMBER); }
static RECT ChecklistIconRect(const RECT& client) { return StripIconRect(client, SLOT_CHECKLIST); }
static RECT BoldIconRect(const RECT& client) { return StripIconRect(client, SLOT_BOLD); }
static RECT ItalicIconRect(const RECT& client) { return StripIconRect(client, SLOT_ITALIC); }

static RECT ColorIconRect(const RECT& client) {
    int sz = S(kIconDip);
    int top = S(kTitleDip) + (S(kStripDip) - sz) / 2;
    int right = client.right - S(kPadDip) / 2;
    return RECT{right - sz, top, right, top + sz};
}

// "..." in the title bar (row 1).
static RECT MenuIconRect(const RECT& client) {
    int sz = S(kIconDip), top = (S(kTitleDip) - sz) / 2;
    int right = client.right - S(kPadDip) / 2;
    return RECT{right - sz, top, right, top + sz};
}

static RECT TitleTextRect(const RECT& client) {
    RECT m = MenuIconRect(client);
    return RECT{S(kPadDip), 0, m.left - S(4), S(kTitleDip)};
}

static RECT BodyRect(const RECT& client) {
    int top = S(kTitleDip) + S(kStripDip) + S(6);
    return RECT{S(kPadDip), top, client.right - S(kPadDip),
                std::max(top, (int)client.bottom - S(kPadDip))};
}

enum class Zone {
    None, Title, Strip, NumIcon, ChecklistIcon, BoldIcon, ItalicIcon, ColorIcon,
    MenuIcon, Checkbox, Link, Body
};

static Zone HitTestNote(Note* n, POINT pt, const Hit** hitOut) {
    RECT client;
    if (!GetClientRect(n->hwnd, &client)) return Zone::None;
    RECT menu = MenuIconRect(client), num = NumIconRect(client),
         check = ChecklistIconRect(client), bold = BoldIconRect(client),
         italic = ItalicIconRect(client), color = ColorIconRect(client),
         title = TitleRect(client), strip = StripRect(client);
    if (PtInRect(&menu, pt)) return Zone::MenuIcon;
    if (PtInRect(&num, pt)) return Zone::NumIcon;
    if (PtInRect(&check, pt)) return Zone::ChecklistIcon;
    if (PtInRect(&bold, pt)) return Zone::BoldIcon;
    if (PtInRect(&italic, pt)) return Zone::ItalicIcon;
    if (PtInRect(&color, pt)) return Zone::ColorIcon;
    if (PtInRect(&title, pt)) return Zone::Title;
    if (PtInRect(&strip, pt)) return Zone::Strip;
    RECT body = BodyRect(client);
    if (!PtInRect(&body, pt)) {
        return pt.y >= strip.bottom ? Zone::Body : Zone::None;
    }
    for (const Hit& h : n->hits) {
        if (PtInRect(&h.rc, pt)) {
            if (hitOut) *hitOut = &h;
            return h.kind == HitKind::Checkbox ? Zone::Checkbox : Zone::Link;
        }
    }
    return Zone::Body;
}

// ============================================================================
// Painting
// ============================================================================

// Brushes for the background and title bar, used both for painting and for
// the edit controls (WM_CTLCOLOREDIT). Rebuilt when the look changes.
static void EnsureBrushes(Note* n, const Look& look) {
    if (n->brushBg == look.bg && n->brushTitle == look.title && n->bgBrush &&
        n->titleBrush) {
        return;
    }
    if (n->bgBrush) WIN_CHECK(DeleteObject(n->bgBrush));
    if (n->titleBrush) WIN_CHECK(DeleteObject(n->titleBrush));
    n->bgBrush = WIN_CHECK(CreateSolidBrush(look.bg));
    n->titleBrush = WIN_CHECK(CreateSolidBrush(look.title));
    n->brushBg = look.bg;
    n->brushTitle = look.title;
}

static void FillSolid(HDC dc, const RECT& r, COLORREF c) {
    HBRUSH b = WIN_CHECK(CreateSolidBrush(c));
    if (!b) return;
    WIN_CHECK(FillRect(dc, &r, b));
    WIN_CHECK(DeleteObject(b));
}

static void FillRound(HDC dc, const RECT& r, int radius, COLORREF c) {
    HBRUSH b = WIN_CHECK(CreateSolidBrush(c));
    if (!b) return;
    HGDIOBJ ob = SelectObject(dc, b);
    HGDIOBJ op = SelectObject(dc, GetStockObject(NULL_PEN));
    WIN_CHECK(RoundRect(dc, r.left, r.top, r.right + 1, r.bottom + 1, radius, radius));
    SelectObject(dc, op);
    SelectObject(dc, ob);
    WIN_CHECK(DeleteObject(b));
}

static void DrawCheckbox(HDC dc, RECT box, bool done) {
    int w = std::max(1, S(1));
    HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, w, g_look.muted));
    if (!pen) return;
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    int r = S(3);
    WIN_CHECK(RoundRect(dc, box.left, box.top, box.right, box.bottom, r, r));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    WIN_CHECK(DeleteObject(pen));

    if (done) {
        HPEN check = WIN_CHECK(CreatePen(PS_SOLID, std::max(2, S(2)), g_look.text));
        if (!check) return;
        oldPen = SelectObject(dc, check);
        int bw = box.right - box.left, bh = box.bottom - box.top;
        POINT pts[3] = {{box.left + bw * 22 / 100, box.top + bh * 52 / 100},
                        {box.left + bw * 42 / 100, box.top + bh * 72 / 100},
                        {box.left + bw * 78 / 100, box.top + bh * 28 / 100}};
        WIN_CHECK(Polyline(dc, pts, 3));
        SelectObject(dc, oldPen);
        WIN_CHECK(DeleteObject(check));
    }
}

static void DrawFragment(HDC dc, int x, int y, const wchar_t* s, int len,
                         int style, COLORREF color) {
    SelectObject(dc, g_fonts.body[style]);
    SetTextColor(dc, color);
    WIN_CHECK(ExtTextOutW(dc, x, y, 0, nullptr, s, (UINT)len, nullptr));
}

// Lays out (and optionally draws) the body text. Fills n->hits with
// checkbox and link rectangles in client coordinates and returns the
// content height in pixels. Lines wrap at spaces, and a word longer than
// the available width is broken between characters. Wrapped rows are
// indented to the start of the text, after the number and the checkbox.
static int LayoutBody(Note* n, HDC dc, const RECT& body, bool draw) {
    n->hits.clear();
    const int lineH = g_fonts.lineH;
    const int right = body.right - S(kScrollbarDip);
    int y = body.top - n->scroll;

    int numW = 0;
    if (n->numbering) {
        // Gutter wide enough for the largest number: at least two digits,
        // more for notes with 100+ numbered lines.
        int numbered = 0;
        for (const Line& l : n->lines) {
            if (!l.text.empty() || l.checkbox) ++numbered;
        }
        std::wstring widest(std::max<size_t>(2, std::to_wstring(numbered).size()), L'0');
        widest += L'.';
        SelectObject(dc, g_fonts.body[0]);
        SIZE sz;
        if (WIN_CHECK(GetTextExtentPoint32W(dc, widest.c_str(), (int)widest.size(), &sz))) {
            numW = sz.cx;
        }
    }

    if (n->lines.empty() && draw) {
        DrawFragment(dc, body.left, y, L"Click to add text\x2026", 18, 0, g_look.muted);
    }

    int number = 0;
    for (int i = 0; i < (int)n->lines.size(); i++) {
        const Line& line = n->lines[i];
        int x = body.left;
        bool empty = line.text.empty() && !line.checkbox;

        if (n->numbering && !empty) {
            std::wstring s = std::to_wstring(++number) + L".";
            SelectObject(dc, g_fonts.body[0]);
            SIZE sz{};
            WIN_CHECK(GetTextExtentPoint32W(dc, s.c_str(), (int)s.size(), &sz));
            if (draw) {
                DrawFragment(dc, x + numW - sz.cx, y, s.c_str(), (int)s.size(), 0,
                             g_look.muted);
            }
            x += numW + S(4);
        }

        if (line.checkbox) {
            int box = std::max(S(10), lineH * 68 / 100);
            RECT rb{x, y + (lineH - box) / 2, x + box, y + (lineH - box) / 2 + box};
            if (draw) DrawCheckbox(dc, rb, line.done);
            RECT hit = rb;
            InflateRect(&hit, S(3), S(3));
            n->hits.push_back({hit, HitKind::Checkbox, i, {}});
            x += box + S(6);
        }

        const int indent = x;
        for (const Run& run : StyledRuns(line)) {
            int style = (line.done ? 1 : 0) | (run.link ? 2 : 0) |
                        ((run.fmt & kFmtBold) ? 4 : 0) | ((run.fmt & kFmtItalic) ? 8 : 0);
            COLORREF color = line.done ? g_look.muted : run.link ? g_look.link : g_look.text;
            SelectObject(dc, g_fonts.body[style]);
            const std::wstring& t = run.text;
            size_t pos = 0;
            while (pos < t.size()) {
                // Token = a word plus the spaces that follow it.
                size_t wordEnd = pos;
                while (wordEnd < t.size() && t[wordEnd] != L' ') ++wordEnd;
                size_t end = wordEnd;
                while (end < t.size() && t[end] == L' ') ++end;

                SIZE wordSz{}, tokSz{};
                WIN_CHECK(GetTextExtentPoint32W(dc, t.c_str() + pos,
                                                (int)(wordEnd - pos), &wordSz));
                WIN_CHECK(GetTextExtentPoint32W(dc, t.c_str() + pos,
                                                (int)(end - pos), &tokSz));
                if (x + wordSz.cx > right && x > indent) {
                    y += lineH;
                    x = indent;
                }
                if (x + wordSz.cx > right) {
                    // Too long even for an empty row: break between characters.
                    int fit = 0;
                    SIZE dummy;
                    WIN_CHECK(GetTextExtentExPointW(dc, t.c_str() + pos,
                                                    (int)(wordEnd - pos),
                                                    std::max(1, right - x), &fit,
                                                    nullptr, &dummy));
                    fit = std::max(1, fit);
                    SIZE partSz{};
                    WIN_CHECK(GetTextExtentPoint32W(dc, t.c_str() + pos, fit, &partSz));
                    if (draw) DrawFragment(dc, x, y, t.c_str() + pos, fit, style, color);
                    if (run.link) {
                        n->hits.push_back({RECT{x, y, x + partSz.cx, y + lineH},
                                           HitKind::Link, i, run.url});
                    }
                    pos += fit;
                    y += lineH;
                    x = indent;
                    continue;
                }
                if (draw) {
                    DrawFragment(dc, x, y, t.c_str() + pos, (int)(end - pos), style, color);
                }
                if (run.link) {
                    n->hits.push_back({RECT{x, y, x + wordSz.cx, y + lineH},
                                       HitKind::Link, i, run.url});
                }
                x += tokSz.cx;
                pos = end;
            }
        }
        y += lineH + S(kLineGapDip);
    }

    // Hits outside the visible body (scrolled away) must not be clickable.
    std::erase_if(n->hits, [&](const Hit& h) {
        RECT r;
        return !IntersectRect(&r, &h.rc, &body);
    });
    return y + n->scroll - body.top;
}

static uint8_t EditSelectionFmt(Note* n);

static void DrawNote(Note* n, HDC dc, const RECT& client) {
    g_look = EffectiveLook(n);
    EnsureBrushes(n, g_look);
    if (n->bgBrush) WIN_CHECK(FillRect(dc, &client, n->bgBrush));
    RECT title = TitleRect(client);
    if (n->titleBrush) WIN_CHECK(FillRect(dc, &title, n->titleBrush));
    FillSolid(dc, StripRect(client), g_look.strip);
    SetBkMode(dc, TRANSPARENT);

    // Toggles are drawn as a filled chip when on.
    COLORREF chip = Tone(g_look.strip, 80);
    COLORREF iconOff = Mix(g_look.icon, g_look.strip, 35);

    // Numbering toggle ("1.").
    RECT num = NumIconRect(client);
    if (n->numbering) FillRound(dc, num, S(6), chip);
    SelectObject(dc, g_fonts.icon);
    SetTextColor(dc, n->numbering ? g_look.icon : iconOff);
    WIN_CHECK(DrawTextW(dc, L"1.", 2, &num, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX));

    // Checklist toggle: a small box with a tick, "on" when every line is a
    // checkbox.
    RECT chk = ChecklistIconRect(client);
    bool allChecks = AllLinesAreCheckboxes(n->lines);
    if (allChecks) FillRound(dc, chk, S(6), chip);
    {
        COLORREF c = allChecks ? g_look.icon : iconOff;
        int bs = S(10);
        RECT box{(chk.left + chk.right - bs) / 2, (chk.top + chk.bottom - bs) / 2, 0, 0};
        box.right = box.left + bs;
        box.bottom = box.top + bs;
        HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, std::max(1, S(1)), c));
        if (pen) {
            HGDIOBJ op = SelectObject(dc, pen);
            HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
            WIN_CHECK(RoundRect(dc, box.left, box.top, box.right, box.bottom, S(3), S(3)));
            POINT tick[3] = {{box.left + bs * 22 / 100, box.top + bs * 52 / 100},
                             {box.left + bs * 42 / 100, box.top + bs * 72 / 100},
                             {box.left + bs * 78 / 100, box.top + bs * 28 / 100}};
            WIN_CHECK(Polyline(dc, tick, 3));
            SelectObject(dc, ob);
            SelectObject(dc, op);
            WIN_CHECK(DeleteObject(pen));
        }
    }

    // Bold / Italic: only usable while editing. Shown "on" when the
    // selection (or the typing position) has that format, and faded out
    // when not editing.
    uint8_t selFmt = n->bodyEdit ? EditSelectionFmt(n) : 0;
    COLORREF iconDisabled = Mix(g_look.icon, g_look.strip, 65);
    RECT bold = BoldIconRect(client), italic = ItalicIconRect(client);
    if (selFmt & kFmtBold) FillRound(dc, bold, S(6), chip);
    if (selFmt & kFmtItalic) FillRound(dc, italic, S(6), chip);
    SetTextColor(dc, !n->bodyEdit ? iconDisabled : (selFmt & kFmtBold) ? g_look.icon : iconOff);
    SelectObject(dc, g_fonts.body[4]);  // Bold.
    WIN_CHECK(DrawTextW(dc, L"B", 1, &bold, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX));
    SetTextColor(dc, !n->bodyEdit ? iconDisabled : (selFmt & kFmtItalic) ? g_look.icon : iconOff);
    SelectObject(dc, g_fonts.body[8 | 4]);  // Bold italic, so the "I" reads.
    WIN_CHECK(DrawTextW(dc, L"I", 1, &italic, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX));

    // Color button: a circle in the note's current background color with a
    // 1 px outline in the icon color, so it previews the note's look.
    {
        RECT cr = ColorIconRect(client);
        int d = S(14);
        POINT c{(cr.left + cr.right) / 2, (cr.top + cr.bottom) / 2};
        HBRUSH fill = WIN_CHECK(CreateSolidBrush(g_look.bg));
        HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, 1, g_look.icon));
        if (fill && pen) {
            HGDIOBJ ob = SelectObject(dc, fill);
            HGDIOBJ op = SelectObject(dc, pen);
            WIN_CHECK(Ellipse(dc, c.x - d / 2, c.y - d / 2, c.x + d / 2 + 1, c.y + d / 2 + 1));
            SelectObject(dc, op);
            SelectObject(dc, ob);
        }
        if (fill) WIN_CHECK(DeleteObject(fill));
        if (pen) WIN_CHECK(DeleteObject(pen));
    }

    // "..." menu button in the title bar (row 1).
    RECT menu = MenuIconRect(client);
    {
        HBRUSH b = WIN_CHECK(CreateSolidBrush(g_look.muted));
        if (b) {
            HGDIOBJ ob = SelectObject(dc, b);
            HGDIOBJ op = SelectObject(dc, GetStockObject(NULL_PEN));
            int cy = (menu.top + menu.bottom) / 2, cx = (menu.left + menu.right) / 2;
            int r = std::max(2, S(2)), step = S(5);
            for (int k = -1; k <= 1; k++) {
                WIN_CHECK(Ellipse(dc, cx + k * step - r, cy - r, cx + k * step + r + 1, cy + r + 1));
            }
            SelectObject(dc, op);
            SelectObject(dc, ob);
            WIN_CHECK(DeleteObject(b));
        }
    }

    if (!n->titleEdit) {
        RECT tr = TitleTextRect(client);
        SelectObject(dc, g_fonts.title);
        SetTextColor(dc, n->title.empty() ? g_look.muted : g_look.text);
        const std::wstring& t = n->title.empty() ? std::wstring(L"Untitled") : n->title;
        WIN_CHECK(DrawTextW(dc, t.c_str(), (int)t.size(), &tr,
                            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX |
                                DT_END_ELLIPSIS));
    }

    if (n->bodyEdit) return;  // The edit control draws the body.

    RECT body = BodyRect(client);
    int bodyH = body.bottom - body.top;
    // Measure first so the scroll offset can be clamped before drawing.
    n->contentH = LayoutBody(n, dc, body, false);
    int maxScroll = std::max(0, n->contentH - bodyH);
    n->scroll = std::clamp(n->scroll, 0, maxScroll);

    int saved = SaveDC(dc);
    IntersectClipRect(dc, body.left, body.top, body.right, body.bottom);
    LayoutBody(n, dc, body, true);
    if (saved) RestoreDC(dc, saved);

    if (maxScroll > 0) {
        // Thin scroll position indicator on the right edge of the body.
        int trackH = bodyH;
        int thumbH = std::max(S(16), trackH * bodyH / n->contentH);
        int thumbY = body.top + (trackH - thumbH) * n->scroll / maxScroll;
        RECT thumb{body.right - S(4), thumbY, body.right, thumbY + thumbH};
        HBRUSH b = WIN_CHECK(CreateSolidBrush(Tone(g_look.bg, 70)));
        if (b) {
            WIN_CHECK(FillRect(dc, &thumb, b));
            WIN_CHECK(DeleteObject(b));
        }
    }
}

static void PaintNote(Note* n) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(n->hwnd, &ps);
    if (!hdc) {
        Wh_Log(L"BeginPaint failed, error %u", GetLastError());
        return;
    }
    RECT client;
    WIN_CHECK(GetClientRect(n->hwnd, &client));
    int w = client.right, h = client.bottom;

    // Double buffering avoids flicker while dragging and typing.
    HDC mem = w > 0 && h > 0 ? CreateCompatibleDC(hdc) : nullptr;
    HBITMAP bmp = mem ? CreateCompatibleBitmap(hdc, w, h) : nullptr;
    if (mem && bmp) {
        HGDIOBJ oldBmp = SelectObject(mem, bmp);
        HGDIOBJ oldFont = SelectObject(mem, g_fonts.body[0]);
        DrawNote(n, mem, client);
        WIN_CHECK(BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY));
        SelectObject(mem, oldFont);
        SelectObject(mem, oldBmp);
    } else {
        if (w > 0 && h > 0) {
            Wh_Log(L"Back buffer creation failed, error %u; drawing directly",
                   GetLastError());
        }
        HGDIOBJ oldFont = SelectObject(hdc, g_fonts.body[0]);
        DrawNote(n, hdc, client);
        SelectObject(hdc, oldFont);
    }
    if (bmp) WIN_CHECK(DeleteObject(bmp));
    if (mem) WIN_CHECK(DeleteDC(mem));
    EndPaint(n->hwnd, &ps);
}

// ============================================================================
// Window shape: rounded corners and opacity
// ============================================================================

// Uses DWM rounded corners on Windows 11. On Windows 10, and for layered
// windows (which DWM does not round), it falls back to a window region.
static void ApplyShape(HWND hwnd, bool layered, int* lastW, int* lastH,
                       int* lastLayered) {
    RECT wr;
    if (!WIN_CHECK(GetWindowRect(hwnd, &wr))) return;
    int w = wr.right - wr.left, h = wr.bottom - wr.top;
    if (w == *lastW && h == *lastH && (int)layered == *lastLayered) return;
    *lastW = w;
    *lastH = h;
    *lastLayered = layered;

    bool dwmRounded = false;
    if (!layered) {
        DWORD pref = kDwmwcpRound;
        // E_INVALIDARG before Windows 11 is expected and handled below.
        dwmRounded = SUCCEEDED(DwmSetWindowAttribute(
            hwnd, kDwmwaWindowCornerPreference, &pref, sizeof(pref)));
    }
    if (dwmRounded) {
        WIN_CHECK(SetWindowRgn(hwnd, nullptr, TRUE));
        return;
    }
    int r = S(kCornerDip);
    HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, r, r);
    if (!rgn) {
        Wh_Log(L"CreateRoundRectRgn failed");
        return;
    }
    if (!SetWindowRgn(hwnd, rgn, TRUE)) {
        Wh_Log(L"SetWindowRgn failed, error %u", GetLastError());
        WIN_CHECK(DeleteObject(rgn));  // Not owned by the system on failure.
    }
}

static void ApplyOpacity(Note* n) {
    bool want = g_settings.opacity < 100;
    LONG_PTR ex = GetWindowLongPtrW(n->hwnd, GWL_EXSTYLE);
    bool has = (ex & WS_EX_LAYERED) != 0;
    if (want != has) {
        SetLastError(0);
        LONG_PTR newEx = want ? (ex | WS_EX_LAYERED) : (ex & ~(LONG_PTR)WS_EX_LAYERED);
        if (!SetWindowLongPtrW(n->hwnd, GWL_EXSTYLE, newEx) && GetLastError()) {
            Wh_Log(L"SetWindowLongPtrW(GWL_EXSTYLE) failed, error %u", GetLastError());
            return;
        }
        WIN_CHECK(RedrawWindow(n->hwnd, nullptr, nullptr,
                               RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN));
    }
    if (want) {
        BYTE alpha = (BYTE)(g_settings.opacity * 255 / 100);
        WIN_CHECK(SetLayeredWindowAttributes(n->hwnd, 0, alpha, LWA_ALPHA));
    }
    ApplyShape(n->hwnd, want, &n->shapeW, &n->shapeH, &n->shapeLayered);
}

// ============================================================================
// Column stacking (push-down) logic
// ============================================================================

struct StackItem {
    Note* note;
    int y;      // DIPs.
    int h;      // DIPs.
    int order;  // Index in g_notes, the tie-breaker.
};

// Resolves one column so no notes overlap, working top to bottom: each note
// keeps its own y unless that would overlap the note above, in which case
// it is pushed down to just below it (plus the gap). Notes only ever move
// down, so free vertical placement is kept wherever it doesn't collide.
//
// `priority` is the note that was just dropped. Its place in the order
// follows from where it was dropped, not from a plain y comparison: it goes
// in front of the first note whose vertical midpoint lies below the drop
// point. Dropping onto the upper half of a note therefore puts the dropped
// note above it and pushes it, and everything after it, down.
static void ResolveStack(std::vector<StackItem>& items, const StackItem* priority) {
    std::stable_sort(items.begin(), items.end(), [](const StackItem& a, const StackItem& b) {
        return a.y != b.y ? a.y < b.y : a.order < b.order;
    });
    if (priority) {
        auto it = std::find_if(items.begin(), items.end(), [&](const StackItem& s) {
            return s.y + s.h / 2 > priority->y;
        });
        items.insert(it, *priority);
    }
    int bottom = 0;
    for (StackItem& s : items) {
        s.y = std::max(s.y, bottom);
        bottom = s.y + s.h + kGapDip;
    }
}

static std::vector<StackItem> ColumnItems(int col, const Note* exclude) {
    std::vector<StackItem> items;
    for (int i = 0; i < (int)g_notes.size(); i++) {
        Note* n = g_notes[i].get();
        if (n != exclude && n->col == col && IsVisibleNote(n)) {
            items.push_back({n, n->y, kHeightDip[n->height], i});
        }
    }
    return items;
}

static int OrderOf(const Note* n) {
    for (int i = 0; i < (int)g_notes.size(); i++) {
        if (g_notes[i].get() == n) return i;
    }
    return 0;
}

// Applies ResolveStack to the real notes of a column. Returns true if any y
// changed.
static bool ResolveColumn(int col, Note* priority) {
    std::vector<StackItem> items = ColumnItems(col, priority);
    StackItem p{};
    if (priority) p = {priority, priority->y, kHeightDip[priority->height], OrderOf(priority)};
    ResolveStack(items, priority ? &p : nullptr);
    bool changed = false;
    for (const StackItem& s : items) {
        if (s.note->y != s.y) {
            s.note->y = s.y;
            changed = true;
        }
    }
    return changed;
}

// Same resolution on a copy: where would `n` land if dropped at (col, y)?
// The drag indicator uses this.
static int SimulateLanding(Note* n, int col, int yDip) {
    std::vector<StackItem> items = ColumnItems(col, n);
    StackItem p{n, yDip, kHeightDip[n->height], OrderOf(n)};
    ResolveStack(items, &p);
    for (const StackItem& s : items) {
        if (s.note == n) return s.y;
    }
    return yDip;
}

// ============================================================================
// Layout
// ============================================================================

static void EndBodyEdit(Note* n);
static void RelayoutPanel();
static void EndTitleEdit(Note* n, bool commit);

static void PositionNote(Note* n) {
    if (!n->hwnd) return;
    if (!IsVisibleNote(n)) {
        if (n->bodyEdit) EndBodyEdit(n);
        if (n->titleEdit) EndTitleEdit(n, true);
        if (IsWindowVisible(n->hwnd)) {
            n->hiding = true;  // Our own hide: don't cancel it.
            ShowWindow(n->hwnd, SW_HIDE);
            n->hiding = false;
        }
        return;
    }
    RECT r = NoteRect(n);
    WIN_CHECK(SetWindowPos(n->hwnd, nullptr, r.left, r.top, r.right - r.left,
                           r.bottom - r.top,
                           SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW));
    // Showing a hidden window makes Windows move it toward the top of the
    // stack even with SWP_NOZORDER, and the Z-order handler skips such calls.
    // Put it back directly above the desktop (not while it's being edited or
    // dragged, when it is deliberately raised).
    if (!n->bodyEdit && !n->titleEdit && g_drag.note != n) PlaceAboveHost(n->hwnd);
    ApplyOpacity(n);
    WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
}

// Full relayout: recompute the grid, clamp columns, resolve overlaps in
// every column and move every window.
static void Layout() {
    if (!g_active || g_quitting) return;
    Geo geo = ComputeGeo();
    bool fontsStale = geo.dpi != g_fonts.dpi || g_settings.fontSize != g_fonts.pt;
    g_geo = geo;
    if (fontsStale) CreateFonts();

    bool changed = false;
    for (auto& n : g_notes) {
        // If the column count shrank, overflowing notes move into the last
        // column. ResolveColumn below stacks them under what is already there.
        int c = std::clamp(n->col, 0, g_geo.cols - 1);
        if (c != n->col) {
            n->col = c;
            changed = true;
        }
    }
    for (int c = 0; c < g_geo.cols; c++) {
        changed |= ResolveColumn(c, nullptr);
    }
    for (auto& n : g_notes) {
        PositionNote(n.get());
    }
    RelayoutPanel();  // DPI, work area or appearance mode may have changed.
    if (changed) ScheduleSave();
    Wh_Log(L"Layout: %d column(s), dpi %u, work %ld,%ld-%ld,%ld", g_geo.cols,
           g_geo.dpi, g_geo.work.left, g_geo.work.top, g_geo.work.right,
           g_geo.work.bottom);
}

static void RequestRelayout() {
    if (g_controller &&
        !SetTimer(g_controller, TIMER_RELAYOUT, kRelayoutDelayMs, nullptr)) {
        Wh_Log(L"SetTimer(relayout) failed, error %u", GetLastError());
        Layout();
    }
}

// ============================================================================
// Drag indicator window
// ============================================================================

static LRESULT CALLBACK IndicatorWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (!dc) {
                Wh_Log(L"BeginPaint(indicator) failed, error %u", GetLastError());
                return 0;
            }
            RECT rc;
            WIN_CHECK(GetClientRect(hwnd, &rc));
            HBRUSH fill = WIN_CHECK(CreateSolidBrush(kIndicatorColor));
            if (fill) {
                WIN_CHECK(FillRect(dc, &rc, fill));
                WIN_CHECK(DeleteObject(fill));
            }
            HBRUSH frame = WIN_CHECK(CreateSolidBrush(RGB(255, 255, 255)));
            if (frame) {
                RECT in = rc;
                for (int i = 0; i < std::max(2, S(2)); i++) {
                    InflateRect(&in, -1, -1);
                    WIN_CHECK(FrameRect(dc, &in, frame));
                }
                WIN_CHECK(DeleteObject(frame));
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCDESTROY:
            if (g_indicator == hwnd) g_indicator = nullptr;
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static int g_indW = -1, g_indH = -1, g_indLayered = -1;

static void EnsureIndicator() {
    if (g_indicator) return;
    g_indicator = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        kIndicatorClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_hinst, nullptr);
    if (!g_indicator) {
        Wh_Log(L"CreateWindowExW(indicator) failed, error %u", GetLastError());
        return;
    }
    g_indW = g_indH = g_indLayered = -1;
    WIN_CHECK(SetLayeredWindowAttributes(g_indicator, 0, 80, LWA_ALPHA));
}

// Shows the landing slot just below the dragged note in the Z-order.
static void ShowIndicator(int col, int yDip, int hDip, HWND below) {
    EnsureIndicator();
    if (!g_indicator) return;
    int x = ColumnX(col), y = g_geo.top + S(yDip);
    WIN_CHECK(SetWindowPos(g_indicator, below, x, y, g_geo.colW, S(hDip),
                           SWP_NOACTIVATE | SWP_SHOWWINDOW));
    ApplyShape(g_indicator, true, &g_indW, &g_indH, &g_indLayered);
}

static void HideIndicator() {
    if (g_indicator && IsWindowVisible(g_indicator)) {
        ShowWindow(g_indicator, SW_HIDE);
    }
}

// ============================================================================
// Editing (body text and title)
// ============================================================================

// The body editor is a rich edit (RICHEDIT50W) so lines can carry bold and
// italic. The model (`lines`) stays plain text plus per-character flags; the
// editor is filled from it when editing starts and read back on save and
// when editing ends.

static void ToggleEditFormat(Note* n, uint8_t flag);

// Pastes clipboard text without its formatting (fonts, colors and sizes
// from web pages would otherwise leak into the note).
static void PastePlain(HWND edit) {
    SendMessageW(edit, EM_PASTESPECIAL, CF_UNICODETEXT, 0);
}

static LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam, UINT_PTR id,
                                         DWORD_PTR ref) {
    Note* n = (Note*)ref;
    WPARAM kind = id == ID_TITLE_EDIT ? ENDEDIT_TITLE : ENDEDIT_BODY;
    bool body = kind == ENDEDIT_BODY;
    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                // Esc saves body text but cancels a rename.
                WPARAM flags = kind | (kind == ENDEDIT_TITLE ? ENDEDIT_CANCEL : 0);
                WIN_CHECK(PostMessageW(n->hwnd, WM_APP_ENDEDIT, flags, (LPARAM)hwnd));
                return 0;
            }
            if (wParam == VK_RETURN && kind == ENDEDIT_TITLE) {
                WIN_CHECK(PostMessageW(n->hwnd, WM_APP_ENDEDIT, kind, (LPARAM)hwnd));
                return 0;
            }
            if (ctrl && !alt && wParam == 'A') {
                SendMessageW(hwnd, EM_SETSEL, 0, -1);
                return 0;
            }
            if (body && ctrl && !alt) {
                switch (wParam) {
                    case 'B': ToggleEditFormat(n, kFmtBold); return 0;
                    case 'I': ToggleEditFormat(n, kFmtItalic); return 0;
                    case 'V': PastePlain(hwnd); return 0;
                    // Rich edit shortcuts for formatting the notes can't keep
                    // (underline, alignment, line spacing) are swallowed.
                    case 'U': case 'E': case 'L': case 'R': case 'J':
                    case '1': case '2': case '5':
                        return 0;
                }
            }
            if (body && wParam == VK_INSERT && (GetKeyState(VK_SHIFT) & 0x8000)) {
                PastePlain(hwnd);
                return 0;
            }
            break;
        case WM_CHAR:
            // Swallow the characters of the keys handled above (no beep, and
            // Ctrl+I would otherwise insert a tab).
            if (wParam == VK_ESCAPE || wParam == 1 /* Ctrl+A */ ||
                (wParam == VK_RETURN && kind == ENDEDIT_TITLE)) {
                return 0;
            }
            if (body && ctrl && !alt && wParam < 0x20 && wParam != VK_RETURN &&
                wParam != VK_BACK) {
                return 0;
            }
            break;
        case WM_PASTE:
            if (body) {
                PastePlain(hwnd);
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            // Finish editing when focus leaves (clicking elsewhere). Posted,
            // so the edit isn't destroyed in the middle of a focus change.
            PostMessageW(n->hwnd, WM_APP_ENDEDIT, kind, (LPARAM)hwnd);
            break;
        case WM_NCDESTROY:
            WIN_CHECK(RemoveWindowSubclass(hwnd, EditSubclassProc, id));
            break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Typing needs keyboard focus, so this is the one place a note takes focus,
// and only in direct response to a user click.
static void FocusEdit(Note* n, HWND edit) {
    if (!SetForegroundWindow(n->hwnd)) {
        Wh_Log(L"SetForegroundWindow failed (foreground lock?)");
    }
    SetFocus(edit);
    if (GetFocus() != edit) {
        Wh_Log(L"SetFocus on edit failed, error %u", GetLastError());
    }
}

// Face, size, text color and background of the rich edit, from the note's
// current look. Bold/italic are left alone (the mask doesn't include them).
static void ApplyEditorStyle(Note* n) {
    if (!n->bodyEdit) return;
    Look look = EffectiveLook(n);
    SendMessageW(n->bodyEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)look.bg);
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR;
    cf.yHeight = g_settings.fontSize * 20;  // Twips.
    cf.crTextColor = look.text;
    wcscpy_s(cf.szFaceName, L"Segoe UI");
    if (!SendMessageW(n->bodyEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf)) {
        Wh_Log(L"EM_SETCHARFORMAT(SCF_ALL) failed");
    }
    if (!SendMessageW(n->bodyEdit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf)) {
        Wh_Log(L"EM_SETCHARFORMAT(SCF_DEFAULT) failed");
    }
}

static void SetRangeFmt(HWND e, LONG a, LONG b, uint8_t f) {
    CHARRANGE cr{a, b};
    SendMessageW(e, EM_EXSETSEL, 0, (LPARAM)&cr);
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD | CFM_ITALIC;
    cf.dwEffects = ((f & kFmtBold) ? CFE_BOLD : 0) | ((f & kFmtItalic) ? CFE_ITALIC : 0);
    SendMessageW(e, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

// Fills the editor from the model. Character positions: every line is
// prefix + text + one '\r'.
static void SetEditorContent(Note* n) {
    HWND e = n->bodyEdit;
    n->suppressChange = true;
    std::wstring text = SerializeLines(n->lines);
    SETTEXTEX st{ST_DEFAULT, 1200};  // 1200 = UTF-16.
    if (!SendMessageW(e, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)text.c_str()) && !text.empty()) {
        Wh_Log(L"EM_SETTEXTEX failed");
    }
    ApplyEditorStyle(n);
    LONG pos = 0;
    for (const Line& l : n->lines) {
        LONG base = pos + (LONG)LinePrefix(l).size();
        for (size_t a = 0; a < l.fmt.size();) {
            size_t b = a + 1;
            while (b < l.fmt.size() && l.fmt[b] == l.fmt[a]) ++b;
            if (l.fmt[a]) SetRangeFmt(e, base + (LONG)a, base + (LONG)b, l.fmt[a]);
            a = b;
        }
        pos = base + (LONG)l.text.size() + 1;
    }
    SendMessageW(e, EM_EMPTYUNDOBUFFER, 0, 0);
    n->suppressChange = false;
}

static std::wstring GetEditorText(HWND e) {
    GETTEXTLENGTHEX gl{GTL_NUMCHARS | GTL_PRECISE, 1200};
    LONG len = (LONG)SendMessageW(e, EM_GETTEXTLENGTHEX, (WPARAM)&gl, 0);
    if (len <= 0) return {};
    std::wstring s((size_t)len + 1, L'\0');
    GETTEXTEX gt{};
    gt.cb = (DWORD)((len + 1) * sizeof(wchar_t));
    gt.flags = GT_DEFAULT;  // Paragraphs end in '\r', matching positions.
    gt.codepage = 1200;
    LONG got = (LONG)SendMessageW(e, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)&s[0]);
    s.resize((size_t)std::max(0L, got));
    return s;
}

// IID of ITextDocument ({8CC497C0-A1DF-11CE-8098-00AA0047BE5D}), defined here
// because the MinGW import libraries don't provide it.
static const IID kIidTextDocument = {
    0x8CC497C0, 0xA1DF, 0x11CE, {0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D}};

// Bold/italic of every character, read through the Text Object Model. Unlike
// probing with EM_EXSETSEL + EM_GETCHARFORMAT, this never touches the user's
// selection (including its direction) or scroll position. A range whose font
// is uniform is answered by one query; only mixed ranges are split.
static bool ReadFormats(HWND e, std::vector<uint8_t>& flags) {
    IRichEditOle* ole = nullptr;
    if (!SendMessageW(e, EM_GETOLEINTERFACE, 0, (LPARAM)&ole) || !ole) {
        Wh_Log(L"EM_GETOLEINTERFACE failed");
        return false;
    }
    ITextDocument* doc = nullptr;
    HRESULT hr = ole->QueryInterface(kIidTextDocument, (void**)&doc);
    ole->Release();  // EM_GETOLEINTERFACE adds a reference.
    if (FAILED(hr) || !doc) {
        Wh_Log(L"ITextDocument not available, hr=0x%08X", (unsigned)hr);
        return false;
    }

    auto query = [&](LONG a, LONG b, LONG* bold, LONG* italic) {
        ITextRange* range = nullptr;
        if (FAILED(doc->Range(a, b, &range)) || !range) return false;
        ITextFont* font = nullptr;
        bool ok = SUCCEEDED(range->GetFont(&font)) && font &&
                  SUCCEEDED(font->GetBold(bold)) && SUCCEEDED(font->GetItalic(italic));
        if (font) font->Release();
        range->Release();
        return ok;
    };

    bool ok = true;
    auto probe = [&](auto& self, LONG a, LONG b) -> void {
        LONG bold = tomUndefined, italic = tomUndefined;
        if (!query(a, b, &bold, &italic)) {
            ok = false;
            return;
        }
        if ((bold != tomUndefined && italic != tomUndefined) || b - a <= 1) {
            uint8_t f = (bold == tomTrue ? kFmtBold : 0) | (italic == tomTrue ? kFmtItalic : 0);
            std::fill(flags.begin() + a, flags.begin() + b, f);
            return;
        }
        LONG mid = a + (b - a) / 2;
        self(self, a, mid);
        self(self, mid, b);
    };
    if (!flags.empty()) probe(probe, 0, (LONG)flags.size());
    doc->Release();
    return ok;
}

// Reads text and bold/italic back into n->lines.
static bool ReadEditor(Note* n) {
    HWND e = n->bodyEdit;
    if (!e) return true;

    std::wstring s = GetEditorText(e);
    std::vector<uint8_t> flags(s.size(), 0);
    if (!ReadFormats(e, flags)) {
        // Text is still saved; formatting falls back to plain.
        Wh_Log(L"Couldn't read bold/italic from the editor; saving plain text");
        std::fill(flags.begin(), flags.end(), 0);
    }

    std::vector<Line> lines;
    size_t start = 0;
    for (size_t i = 0; i <= s.size(); i++) {
        if (i < s.size() && s[i] != L'\r' && s[i] != L'\n') continue;
        size_t prefix = 0;
        Line l = ParseLine(s.substr(start, i - start), &prefix);
        if (start + prefix <= i) {
            l.fmt.assign(flags.begin() + (start + prefix), flags.begin() + i);
        }
        NormalizeFmt(l);
        lines.push_back(std::move(l));
        if (i < s.size() && s[i] == L'\r' && i + 1 < s.size() && s[i + 1] == L'\n') ++i;
        start = i + 1;
    }
    n->lines = std::move(lines);
    n->editDirty = false;
    return true;
}

// Called before every save: pulls text and formatting out of open editors.
static void SyncEditorsToModel() {
    for (auto& n : g_notes) {
        if (n->bodyEdit && n->editDirty) ReadEditor(n.get());
    }
}

static uint8_t EditSelectionFmt(Note* n) {
    if (!n->bodyEdit) return 0;
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD | CFM_ITALIC;
    SendMessageW(n->bodyEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    uint8_t f = 0;
    if ((cf.dwMask & CFM_BOLD) && (cf.dwEffects & CFE_BOLD)) f |= kFmtBold;
    if ((cf.dwMask & CFM_ITALIC) && (cf.dwEffects & CFE_ITALIC)) f |= kFmtItalic;
    return f;
}

static void InvalidateStrip(Note* n) {
    RECT client;
    if (n->hwnd && GetClientRect(n->hwnd, &client)) {
        RECT strip = StripRect(client);
        WIN_CHECK(InvalidateRect(n->hwnd, &strip, FALSE));
    }
}

// Bold / Italic on the selection; with no selection it sets the format for
// the text typed next. Only meaningful while editing.
static void ToggleEditFormat(Note* n, uint8_t flag) {
    if (!n->bodyEdit) return;
    bool on = (EditSelectionFmt(n) & flag) != 0;
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = flag == kFmtBold ? CFM_BOLD : CFM_ITALIC;
    cf.dwEffects = on ? 0 : (flag == kFmtBold ? CFE_BOLD : CFE_ITALIC);
    if (!SendMessageW(n->bodyEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf)) {
        Wh_Log(L"EM_SETCHARFORMAT(selection) failed");
    }
    // Formatting changes don't raise EN_CHANGE, so mark the edit dirty here.
    n->editDirty = true;
    ScheduleSave();
    InvalidateStrip(n);
}

static void BeginBodyEdit(Note* n) {
    if (!n->hwnd) return;
    if (n->bodyEdit) {
        FocusEdit(n, n->bodyEdit);
        return;
    }
    if (n->titleEdit) EndTitleEdit(n, true);
    if (!g_msftedit) {
        Wh_Log(L"Msftedit.dll is not loaded; cannot edit");
        return;
    }
    RECT client;
    WIN_CHECK(GetClientRect(n->hwnd, &client));
    RECT b = BodyRect(client);
    HWND e = CreateWindowExW(0, MSFTEDIT_CLASS, L"",
                             WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE |
                                 ES_AUTOVSCROLL | ES_WANTRETURN | ES_NOHIDESEL,
                             b.left, b.top, b.right - b.left, b.bottom - b.top,
                             n->hwnd, (HMENU)ID_BODY_EDIT, g_hinst, nullptr);
    if (!e) {
        Wh_Log(L"CreateWindowExW(RICHEDIT50W) failed, error %u", GetLastError());
        return;
    }
    n->bodyEdit = e;
    SendMessageW(e, EM_EXLIMITTEXT, 0, 1 << 20);
    SendMessageW(e, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 0);
    SetEditorContent(n);
    SendMessageW(e, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE);
    if (!SetWindowSubclass(e, EditSubclassProc, ID_BODY_EDIT, (DWORD_PTR)n)) {
        Wh_Log(L"SetWindowSubclass(body) failed");
    }
    CHARRANGE end{-1, -1};  // Caret at the end.
    SendMessageW(e, EM_EXSETSEL, 0, (LPARAM)&end);
    SendMessageW(e, EM_SCROLLCARET, 0, 0);
    n->editDirty = false;
    FocusEdit(n, e);
    WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
}

static std::wstring GetEditText(HWND e) {
    int len = GetWindowTextLengthW(e);
    std::wstring s(len + 1, L'\0');
    int got = GetWindowTextW(e, &s[0], len + 1);
    if (got == 0 && len > 0) {
        Wh_Log(L"GetWindowTextW failed, error %u", GetLastError());
    }
    s.resize(got);
    return s;
}

static void EndBodyEdit(Note* n) {
    HWND e = n->bodyEdit;
    if (!e) return;
    ReadEditor(n);  // While the editor still exists.
    n->bodyEdit = nullptr;  // Before destroying, so a queued kill-focus is ignored.
    TrimTrailingEmptyLines(n->lines);
    WIN_CHECK(DestroyWindow(e));
    if (n->hwnd) {
        WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
        PlaceAboveHost(n->hwnd);  // Back behind the apps.
    }
    ScheduleSave();
}

static void BeginTitleEdit(Note* n) {
    if (!n->hwnd) return;
    if (n->titleEdit) {
        FocusEdit(n, n->titleEdit);
        return;
    }
    if (n->bodyEdit) EndBodyEdit(n);
    RECT client;
    WIN_CHECK(GetClientRect(n->hwnd, &client));
    RECT t = TitleTextRect(client);
    int h = g_fonts.lineH + S(4);
    int top = (S(kTitleDip) - h) / 2;
    HWND e = CreateWindowExW(0, L"EDIT", n->title.c_str(),
                             WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, t.left, top,
                             t.right - t.left, h, n->hwnd, (HMENU)ID_TITLE_EDIT,
                             g_hinst, nullptr);
    if (!e) {
        Wh_Log(L"CreateWindowExW(title EDIT) failed, error %u", GetLastError());
        return;
    }
    SendMessageW(e, WM_SETFONT, (WPARAM)g_fonts.title, FALSE);
    SendMessageW(e, EM_SETLIMITTEXT, 200, 0);
    if (!SetWindowSubclass(e, EditSubclassProc, ID_TITLE_EDIT, (DWORD_PTR)n)) {
        Wh_Log(L"SetWindowSubclass(title) failed");
    }
    n->titleBeforeEdit = n->title;
    n->titleEdit = e;
    SendMessageW(e, EM_SETSEL, 0, -1);
    FocusEdit(n, e);
    WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
}

static void EndTitleEdit(Note* n, bool commit) {
    HWND e = n->titleEdit;
    if (!e) return;
    n->titleEdit = nullptr;
    if (commit) {
        n->title = GetEditText(e);
    } else {
        n->title = n->titleBeforeEdit;
    }
    WIN_CHECK(DestroyWindow(e));
    if (n->hwnd) {
        WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
        PlaceAboveHost(n->hwnd);
    }
    if (commit) ScheduleSave();
}

// ============================================================================
// Note lifecycle
// ============================================================================

static bool CreateNoteWindow(Note* n) {
    // WS_EX_TOOLWINDOW: no taskbar button and no Alt+Tab entry.
    // WS_EX_NOACTIVATE: clicking the note doesn't take focus.
    // No owner: the window is kept just above the desktop by PlaceAboveHost
    // and the WM_WINDOWPOSCHANGING handler (see FindDesktopHost).
    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kNoteClass,
                                L"Sticky note", WS_POPUP | WS_CLIPCHILDREN, 0, 0,
                                10, 10, nullptr, nullptr, g_hinst, n);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW(note) failed, error %u", GetLastError());
        return false;
    }
    n->shapeW = n->shapeH = n->shapeLayered = -1;
    PlaceAboveHost(hwnd);
    return true;
}

static void DestroyNoteWindow(Note* n) {
    if (n->bodyEdit) EndBodyEdit(n);
    if (n->titleEdit) EndTitleEdit(n, true);
    if (n->hwnd) {
        n->hiding = true;  // Destroying hides the window first.
        WIN_CHECK(DestroyWindow(n->hwnd));  // WM_NCDESTROY clears n->hwnd.
    }
    if (n->bgBrush) WIN_CHECK(DeleteObject(n->bgBrush));
    if (n->titleBrush) WIN_CHECK(DeleteObject(n->titleBrush));
    n->bgBrush = n->titleBrush = nullptr;
    n->brushBg = n->brushTitle = CLR_INVALID;
}

// Puts a new note at the bottom of the column with the most room left.
static void AddNote() {
    auto n = MakeNote();
    int bestCol = 0, bestBottom = INT_MAX;
    for (int c = 0; c < g_geo.cols; c++) {
        int bottom = 0;
        for (const StackItem& s : ColumnItems(c, nullptr)) {
            bottom = std::max(bottom, s.y + s.h + kGapDip);
        }
        if (bottom < bestBottom) {
            bestBottom = bottom;
            bestCol = c;
        }
    }
    n->col = bestCol;
    n->y = bestBottom;
    Note* raw = n.get();
    g_notes.push_back(std::move(n));
    if (!CreateNoteWindow(raw)) {
        g_notes.pop_back();
        return;
    }
    Layout();
    ScheduleSave();
    BeginTitleEdit(raw);
}

static void DeleteNoteById(int id) {
    auto it = std::find_if(g_notes.begin(), g_notes.end(),
                           [&](const auto& n) { return n->id == id; });
    if (it == g_notes.end()) return;
    if (g_drag.note == it->get()) g_drag = {};
    DestroyNoteWindow(it->get());
    g_notes.erase(it);
    if (g_notes.empty()) {
        auto n = MakeNote();
        Note* raw = n.get();
        g_notes.push_back(std::move(n));
        CreateNoteWindow(raw);
    }
    Layout();
    ScheduleSave();
}

// ============================================================================
// Dragging
// ============================================================================

static void BeginDragTracking(Note* n) {
    POINT pt;
    RECT wr;
    if (!WIN_CHECK(GetCursorPos(&pt)) || !WIN_CHECK(GetWindowRect(n->hwnd, &wr))) {
        return;
    }
    g_drag = {};
    g_drag.note = n;
    g_drag.grab = POINT{pt.x - wr.left, pt.y - wr.top};
    g_drag.start = pt;
    SetCapture(n->hwnd);
}

static void UpdateDrag(Note* n) {
    POINT pt;
    if (!WIN_CHECK(GetCursorPos(&pt))) return;
    if (!g_drag.moving) {
        if (abs(pt.x - g_drag.start.x) < GetSystemMetrics(SM_CXDRAG) &&
            abs(pt.y - g_drag.start.y) < GetSystemMetrics(SM_CYDRAG)) {
            return;
        }
        g_drag.moving = true;
        // Lift the dragged note above the other notes (still below apps):
        // put it directly under the anchor, which is the top of the band
        // the notes live in. Passing no window skips the "already in place"
        // shortcut, and the Z-order handler leaves a dragged note alone.
        HWND after = nullptr;
        if (DesktopInsertAfter(nullptr, &after)) {
            WIN_CHECK(SetWindowPos(n->hwnd, after, 0, 0, 0, 0,
                                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
        }
    }

    // Horizontal: snap live to the nearest column, never in between.
    // Vertical: free, kept inside the work area.
    int col = NearestColumn(pt.x - g_drag.grab.x);
    int maxTop = std::max(g_geo.top, (int)g_geo.work.bottom - S(kTitleDip));
    int top = std::clamp((int)(pt.y - g_drag.grab.y), g_geo.top, maxTop);
    WIN_CHECK(SetWindowPos(n->hwnd, nullptr, ColumnX(col), top, 0, 0,
                           SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));

    g_drag.targetCol = col;
    g_drag.dropY = std::max(0, ToDip(top - g_geo.top));
    int landing = SimulateLanding(n, col, g_drag.dropY);
    ShowIndicator(col, landing, kHeightDip[n->height], n->hwnd);
}

static void CommitDrop(Note* n) {
    HideIndicator();
    n->col = g_drag.targetCol;
    n->y = g_drag.dropY;
    ResolveColumn(n->col, n);  // Pushes lower notes down.
    Layout();
    ScheduleSave();
}

// ============================================================================
// Themes panel
//
// Windhawk's settings UI only has dropdowns and text fields, so the preset
// theme picker is a custom-drawn GDI popup owned by the note it was opened
// from. Unlike the notes, the panel is activated (keyboard focus) while it
// is open. It closes on Esc, when it loses activation, or when a note is
// clicked (notes never activate, so that click must close it explicitly).
// ============================================================================

constexpr wchar_t kPanelClass[] = L"WhDesktopStickyNotes_Themes";
constexpr int kPanelWidthDip = 620;
constexpr int kPanelPadDip = 20;
constexpr int kTileWDip = 88;
constexpr int kTileHDip = 104;
constexpr int kTileGapDip = 8;
constexpr int kPreviewWDip = 64;
constexpr int kPreviewHDip = 48;
constexpr int kBannerDip = 30;

constexpr COLORREF kPanelBg = RGB(251, 250, 247);
constexpr COLORREF kPanelBorder = RGB(217, 214, 206);
constexpr COLORREF kPanelText = RGB(34, 33, 31);
constexpr COLORREF kPanelMuted = RGB(119, 117, 111);
constexpr COLORREF kPanelRule = RGB(228, 226, 220);
constexpr COLORREF kPanelHover = RGB(239, 237, 231);
constexpr COLORREF kPanelAccent = RGB(47, 95, 208);
constexpr COLORREF kPanelRing = RGB(155, 152, 143);
constexpr COLORREF kPanelHairline = RGB(60, 58, 54);
constexpr COLORREF kBannerBg = RGB(255, 243, 214);
constexpr COLORREF kBannerText = RGB(107, 78, 0);

struct PanelState {
    HWND hwnd = nullptr;
    HWND tooltip = nullptr;
    HFONT heading = nullptr, subtitle = nullptr, name = nullptr, banner = nullptr;
    UINT fontDpi = 0;
    int width = 0, height = 0;
    RECT bannerRect{};
    RECT sectionRow[kThemeSectionCount]{};  // Heading + subtitle row.
    int ruleY[kThemeSectionCount]{};
    // Tiles in display order. tileTheme[k] is the theme index shown by tile k,
    // or kDefaultTheme for the "Default" (solid color) tile.
    RECT tiles[kThemeCount + 1]{};
    int tileTheme[kThemeCount + 1]{};
    int hover = -1;
    int focus = -1;
    bool showFocus = false;  // Focus ring only after keyboard use.
    bool tracking = false;   // TrackMouseEvent(TME_LEAVE) is armed.
    int shapeW = -1, shapeH = -1, shapeLayered = -1;
};
PanelState g_panel;
bool g_classPanel = false;
constexpr int kTileCount = kThemeCount + 1;

static const wchar_t* TileName(int k) {
    int t = g_panel.tileTheme[k];
    return t == kDefaultTheme ? L"Default" : kThemes[t].name;
}

static const wchar_t* TileMeaning(int k) {
    int t = g_panel.tileTheme[k];
    return t == kDefaultTheme ? L"your solid color from settings" : kThemes[t].meaning;
}

static int TileOfTheme(int theme) {
    for (int k = 0; k < kTileCount; k++) {
        if (g_panel.tileTheme[k] == theme) return k;
    }
    return 0;
}

// While the appearance comes from the settings (solid or Jiyu), the panel
// is shown dimmed and a click only stores the choice.
static bool PanelDimmed() {
    return g_settings.appearance != AppearanceMode::Theme;
}

static COLORREF Dim(COLORREF c) {
    return PanelDimmed() ? Mix(c, kPanelBg, 55) : c;
}

static void DeletePanelFonts() {
    for (HFONT* f : {&g_panel.heading, &g_panel.subtitle, &g_panel.name, &g_panel.banner}) {
        if (*f) WIN_CHECK(DeleteObject(*f));
        *f = nullptr;
    }
    g_panel.fontDpi = 0;
}

static void CreatePanelFonts() {
    DeletePanelFonts();
    // Sizes are pixel heights in DIPs.
    g_panel.heading = MakeFont(S(15), FW_MEDIUM, false, false);
    g_panel.subtitle = MakeFont(S(12), FW_NORMAL, false, false);
    g_panel.name = MakeFont(S(12), FW_MEDIUM, false, false);
    g_panel.banner = MakeFont(S(12), FW_NORMAL, false, false);
    g_panel.fontDpi = g_geo.dpi;
}

// Computes every rectangle of the panel for the current DPI and mode.
static void ComputePanelLayout() {
    int pad = S(kPanelPadDip), w = S(kPanelWidthDip);
    int tileW = S(kTileWDip), tileH = S(kTileHDip), gap = S(kTileGapDip);
    int cols = std::max(1, (w - 2 * pad + gap) / (tileW + gap));
    int y = pad;
    int tile = 0;

    g_panel.bannerRect = RECT{};
    if (PanelDimmed()) {
        g_panel.bannerRect = RECT{pad, y, w - pad, y + S(kBannerDip)};
        y += S(kBannerDip) + S(14);
    }
    for (int s = 0; s < kThemeSectionCount; s++) {
        g_panel.sectionRow[s] = RECT{pad, y, w - pad, y + S(22)};
        y += S(22);
        g_panel.ruleY[s] = y + S(6);
        y = g_panel.ruleY[s] + 1 + S(12);
        // Themes of this section; Quiet Hours starts with the Default tile.
        std::vector<int> themes;
        if (s == kThemeSectionCount - 1) themes.push_back(kDefaultTheme);
        for (int i = 0; i < kThemeCount; i++) {
            if (kThemes[i].section == s) themes.push_back(i);
        }
        int k = 0;
        for (int theme : themes) {
            int c = k % cols, r = k / cols;
            int left = pad + c * (tileW + gap), top = y + r * (tileH + gap);
            g_panel.tiles[tile] = RECT{left, top, left + tileW, top + tileH};
            g_panel.tileTheme[tile] = theme;
            tile++;
            k++;
        }
        int rows = (k + cols - 1) / cols;
        y += rows * tileH + std::max(0, rows - 1) * gap;
        if (s + 1 < kThemeSectionCount) y += S(20);
    }
    g_panel.width = w;
    g_panel.height = y + pad;
}

// One tooltip tool per tile, showing the theme's meaning.
static void RebuildPanelTooltips() {
    if (g_panel.tooltip) {
        WIN_CHECK(DestroyWindow(g_panel.tooltip));
        g_panel.tooltip = nullptr;
    }
    g_panel.tooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                      WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT, g_panel.hwnd, nullptr, g_hinst, nullptr);
    if (!g_panel.tooltip) {
        Wh_Log(L"CreateWindowExW(tooltip) failed, error %u", GetLastError());
        return;
    }
    for (int k = 0; k < kTileCount; k++) {
        TTTOOLINFOW ti{};
        // V2 size works with both comctl32 v5 and v6.
        ti.cbSize = TTTOOLINFOW_V2_SIZE;
        ti.uFlags = TTF_SUBCLASS;
        ti.hwnd = g_panel.hwnd;
        ti.uId = (UINT_PTR)(k + 1);
        ti.rect = g_panel.tiles[k];
        ti.hinst = g_hinst;
        ti.lpszText = const_cast<LPWSTR>(TileMeaning(k));
        if (!SendMessageW(g_panel.tooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti)) {
            Wh_Log(L"TTM_ADDTOOLW failed for %s", TileName(k));
        }
    }
}

// Places the panel beside `anchor` (right side if it fits, else left),
// clamped to the work area.
static void PositionPanel(HWND anchor) {
    RECT ar{};
    if (!anchor || !GetWindowRect(anchor, &ar)) {
        ar = RECT{g_geo.work.left, g_geo.work.top, g_geo.work.left, g_geo.work.top};
    }
    const RECT& wa = g_geo.work;
    int gap = S(kGapDip), w = g_panel.width, h = g_panel.height;
    int x = ar.right + gap;
    if (x + w > wa.right) x = ar.left - gap - w;
    x = std::clamp(x, (int)wa.left, std::max((int)wa.left, (int)wa.right - w));
    int y = std::clamp((int)ar.top, (int)wa.top, std::max((int)wa.top, (int)wa.bottom - h));
    WIN_CHECK(SetWindowPos(g_panel.hwnd, nullptr, x, y, w, h,
                           SWP_NOZORDER | SWP_NOACTIVATE));
    ApplyShape(g_panel.hwnd, false, &g_panel.shapeW, &g_panel.shapeH, &g_panel.shapeLayered);
}

// Re-derives fonts, layout, size and tooltips (DPI or mode changed).
static void RelayoutPanel() {
    if (!g_panel.hwnd) return;
    if (g_panel.fontDpi != g_geo.dpi) CreatePanelFonts();
    ComputePanelLayout();
    HWND owner = GetWindow(g_panel.hwnd, GW_OWNER);
    PositionPanel(owner);
    RebuildPanelTooltips();
    WIN_CHECK(InvalidateRect(g_panel.hwnd, nullptr, FALSE));
}

static void ClosePanel() {
    if (g_panel.hwnd) {
        WIN_CHECK(DestroyWindow(g_panel.hwnd));  // WM_NCDESTROY resets state.
    }
}

static void InvalidateAllNotes() {
    for (auto& n : g_notes) {
        if (!n->hwnd) continue;
        WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
        ApplyEditorStyle(n.get());  // Rich edit colors follow the look.
        if (n->bodyEdit) WIN_CHECK(InvalidateRect(n->bodyEdit, nullptr, TRUE));
        if (n->titleEdit) WIN_CHECK(InvalidateRect(n->titleEdit, nullptr, TRUE));
    }
}

// `i` is a theme index or kDefaultTheme.
static void SelectTheme(int i) {
    if (i < kDefaultTheme || i >= kThemeCount) return;
    g_themeIndex = i;
    if (!PanelDimmed()) {
        // "Apply to every note": notes with their own color follow the
        // theme again.
        for (auto& n : g_notes) n->hasColor = false;
        InvalidateAllNotes();
    }
    // In solid/custom mode the choice is only stored, and it takes effect
    // once appearanceMode is set back to "theme".
    ScheduleSave();
    if (g_panel.hwnd) WIN_CHECK(InvalidateRect(g_panel.hwnd, nullptr, FALSE));
}

static void DrawThemePreview(HDC dc, RECT r, const Look& t) {
    int w = r.right - r.left, rad = S(8);
    int saved = SaveDC(dc);
    HRGN clip = CreateRoundRectRgn(r.left, r.top, r.right + 1, r.bottom + 1, rad, rad);
    if (clip) {
        SelectClipRgn(dc, clip);  // Copies the region.
        WIN_CHECK(DeleteObject(clip));
    }
    FillSolid(dc, r, Dim(t.bg));
    RECT title{r.left, r.top, r.right, r.top + S(8)};
    FillSolid(dc, title, Dim(t.title));
    RECT strip{r.left, title.bottom, r.right, title.bottom + S(10)};
    FillSolid(dc, strip, Dim(t.strip));
    for (int k = 0; k < 3; k++) {  // Tool icons as small dashes.
        int x = r.left + S(6) + k * S(10);
        int y = strip.top + (S(10) - S(3)) / 2;
        FillSolid(dc, RECT{x, y, x + S(7), y + S(3)}, Dim(t.icon));
    }
    int textW = w - S(12);
    int y1 = strip.bottom + S(7);
    FillSolid(dc, RECT{r.left + S(6), y1, r.left + S(6) + textW * 78 / 100, y1 + S(3)},
              Dim(t.text));
    int y2 = y1 + S(8);
    FillSolid(dc, RECT{r.left + S(6), y2, r.left + S(6) + textW * 52 / 100, y2 + S(3)},
              Dim(t.text));
    if (saved) RestoreDC(dc, saved);

    // 1 px dark hairline around the mini note.
    HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, 1, Dim(kPanelHairline)));
    if (pen) {
        HGDIOBJ op = SelectObject(dc, pen);
        HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
        WIN_CHECK(RoundRect(dc, r.left, r.top, r.right, r.bottom, rad, rad));
        SelectObject(dc, ob);
        SelectObject(dc, op);
        WIN_CHECK(DeleteObject(pen));
    }
}

static void DrawRing(HDC dc, POINT c, int radius, int width, COLORREF color) {
    HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, width, color));
    if (!pen) return;
    HGDIOBJ op = SelectObject(dc, pen);
    HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
    WIN_CHECK(Ellipse(dc, c.x - radius, c.y - radius, c.x + radius + 1, c.y + radius + 1));
    SelectObject(dc, ob);
    SelectObject(dc, op);
    WIN_CHECK(DeleteObject(pen));
}

static void DrawDot(HDC dc, POINT c, int radius, COLORREF color) {
    HBRUSH b = WIN_CHECK(CreateSolidBrush(color));
    if (!b) return;
    HGDIOBJ ob = SelectObject(dc, b);
    HGDIOBJ op = SelectObject(dc, GetStockObject(NULL_PEN));
    WIN_CHECK(Ellipse(dc, c.x - radius, c.y - radius, c.x + radius + 1, c.y + radius + 1));
    SelectObject(dc, op);
    SelectObject(dc, ob);
    WIN_CHECK(DeleteObject(b));
}

static void DrawPanel(HDC dc, const RECT& client) {
    FillSolid(dc, client, kPanelBg);
    SetBkMode(dc, TRANSPARENT);
    COLORREF nameColor = g_settings.optionTextValid ? g_settings.optionText : kPanelText;
    COLORREF hoverColor = g_settings.optionHoverValid ? g_settings.optionHover : kPanelHover;

    if (PanelDimmed()) {
        FillRound(dc, g_panel.bannerRect, S(8), kBannerBg);
        RECT tr = g_panel.bannerRect;
        tr.left += S(12);
        SelectObject(dc, g_panel.banner);
        SetTextColor(dc, kBannerText);
        const wchar_t* msg = g_settings.appearance == AppearanceMode::Solid
                                 ? L"Solid color mode is on in settings"
                                 : L"Custom color mode is on in settings";
        WIN_CHECK(DrawTextW(dc, msg, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX));
    }

    // Headings and subtitles share a baseline on one row.
    UINT oldAlign = SetTextAlign(dc, TA_LEFT | TA_BASELINE);
    for (int s = 0; s < kThemeSectionCount; s++) {
        const RECT& row = g_panel.sectionRow[s];
        int baseline = row.bottom - S(5);
        const wchar_t* h = kThemeSections[s].heading;
        SelectObject(dc, g_panel.heading);
        SetTextColor(dc, Dim(kPanelText));
        WIN_CHECK(TextOutW(dc, row.left, baseline, h, (int)wcslen(h)));
        SIZE hs{};
        WIN_CHECK(GetTextExtentPoint32W(dc, h, (int)wcslen(h), &hs));
        const wchar_t* sub = kThemeSections[s].subtitle;
        SelectObject(dc, g_panel.subtitle);
        SetTextColor(dc, Dim(kPanelMuted));
        WIN_CHECK(TextOutW(dc, row.left + hs.cx + S(10), baseline, sub, (int)wcslen(sub)));
        // 1 px rule under the row.
        FillSolid(dc, RECT{row.left, g_panel.ruleY[s], row.right, g_panel.ruleY[s] + 1},
                  kPanelRule);
    }
    if (oldAlign != GDI_ERROR) SetTextAlign(dc, oldAlign);

    for (int i = 0; i < kTileCount; i++) {
        const RECT& t = g_panel.tiles[i];
        bool selected = g_panel.tileTheme[i] == g_themeIndex;
        if (i == g_panel.hover) FillRound(dc, t, S(10), hoverColor);
        if (selected) {
            HPEN pen = WIN_CHECK(CreatePen(PS_SOLID, 2, Dim(kPanelAccent)));
            if (pen) {
                HGDIOBJ op = SelectObject(dc, pen);
                HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
                WIN_CHECK(RoundRect(dc, t.left + 1, t.top + 1, t.right, t.bottom, S(10), S(10)));
                SelectObject(dc, ob);
                SelectObject(dc, op);
                WIN_CHECK(DeleteObject(pen));
            }
        }

        int pw = S(kPreviewWDip), ph = S(kPreviewHDip);
        RECT pr{t.left + (t.right - t.left - pw) / 2, t.top + S(22), 0, 0};
        pr.right = pr.left + pw;
        pr.bottom = pr.top + ph;
        DrawThemePreview(dc, pr, LookFromTheme(g_panel.tileTheme[i]));

        RECT nr{t.left + S(2), pr.bottom + S(6), t.right - S(2), pr.bottom + S(6) + S(18)};
        SelectObject(dc, g_panel.name);
        SetTextColor(dc, Dim(nameColor));
        WIN_CHECK(DrawTextW(dc, TileName(i), -1, &nr,
                            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX |
                                DT_END_ELLIPSIS));

        // 16 DIP selection dot in the top-right corner: a ring, filled in
        // the middle when selected.
        POINT c{t.right - S(4) - S(8), t.top + S(4) + S(8)};
        int ringW = std::max(1, MulDiv(3, (int)g_geo.dpi, 192));  // 1.5 DIP.
        DrawRing(dc, c, S(8) - ringW / 2, ringW, Dim(selected ? kPanelAccent : kPanelRing));
        if (selected) DrawDot(dc, c, S(4), Dim(kPanelAccent));

        if (g_panel.showFocus && i == g_panel.focus) {
            RECT fr = t;
            InflateRect(&fr, -S(3), -S(3));
            SetTextColor(dc, kPanelText);
            SetBkColor(dc, kPanelBg);
            WIN_CHECK(DrawFocusRect(dc, &fr));
        }
    }

    // Thin border; DWM rounds the corners on Windows 11.
    HBRUSH border = WIN_CHECK(CreateSolidBrush(kPanelBorder));
    if (border) {
        WIN_CHECK(FrameRect(dc, &client, border));
        WIN_CHECK(DeleteObject(border));
    }
}

static int PanelTileAt(POINT pt) {
    for (int i = 0; i < kTileCount; i++) {
        if (PtInRect(&g_panel.tiles[i], pt)) return i;
    }
    return -1;
}

// Arrow-key navigation over tile indices. Left/right walk the tiles in
// order (across sections). Up/down go to the nearest row above/below, then
// to the tile whose center is closest horizontally.
static int PanelNeighbor(int from, int dx, int dy) {
    if (from < 0) return TileOfTheme(g_themeIndex);
    if (dx) return std::clamp(from + dx, 0, kTileCount - 1);
    const RECT& r = g_panel.tiles[from];
    int cx = (r.left + r.right) / 2;
    int best = from;
    long long bestScore = LLONG_MAX;
    for (int i = 0; i < kTileCount; i++) {
        const RECT& t = g_panel.tiles[i];
        bool ok = dy > 0 ? t.top > r.top : t.top < r.top;
        if (!ok) continue;
        long long score = (long long)abs(t.top - r.top) * 100000 +
                          abs((t.left + t.right) / 2 - cx);
        if (score < bestScore) {
            bestScore = score;
            best = i;
        }
    }
    return best;
}

static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            // Losing activation (a click anywhere else) closes the panel.
            // Posted, so it isn't destroyed in the middle of the switch.
            if (LOWORD(wParam) == WA_INACTIVE) PostMessageW(hwnd, WM_CLOSE, 0, 0);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (!hdc) {
                Wh_Log(L"BeginPaint(panel) failed, error %u", GetLastError());
                return 0;
            }
            RECT client;
            WIN_CHECK(GetClientRect(hwnd, &client));
            HDC mem = CreateCompatibleDC(hdc);
            HBITMAP bmp = mem ? CreateCompatibleBitmap(hdc, client.right, client.bottom) : nullptr;
            if (mem && bmp) {
                HGDIOBJ ob = SelectObject(mem, bmp);
                DrawPanel(mem, client);
                WIN_CHECK(BitBlt(hdc, 0, 0, client.right, client.bottom, mem, 0, 0, SRCCOPY));
                SelectObject(mem, ob);
            } else {
                Wh_Log(L"Panel back buffer failed, error %u", GetLastError());
                DrawPanel(hdc, client);
            }
            if (bmp) WIN_CHECK(DeleteObject(bmp));
            if (mem) WIN_CHECK(DeleteDC(mem));
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt;
                if (GetCursorPos(&pt) && ScreenToClient(hwnd, &pt)) {
                    LPCWSTR id = PanelTileAt(pt) >= 0 ? IDC_HAND : IDC_ARROW;
                    if (HCURSOR c = WIN_CHECK(LoadCursorW(nullptr, id))) SetCursor(c);
                    return TRUE;
                }
            }
            break;

        case WM_MOUSEMOVE: {
            if (!g_panel.tracking) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                g_panel.tracking = WIN_CHECK(TrackMouseEvent(&tme)) != FALSE;
            }
            int hover = PanelTileAt(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (hover != g_panel.hover) {
                g_panel.hover = hover;
                WIN_CHECK(InvalidateRect(hwnd, nullptr, FALSE));
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_panel.tracking = false;
            if (g_panel.hover != -1) {
                g_panel.hover = -1;
                WIN_CHECK(InvalidateRect(hwnd, nullptr, FALSE));
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int i = PanelTileAt(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (i >= 0) {
                g_panel.focus = i;
                g_panel.showFocus = false;
                SelectTheme(g_panel.tileTheme[i]);
            }
            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                    return 0;
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN: {
                    int dx = wParam == VK_LEFT ? -1 : wParam == VK_RIGHT ? 1 : 0;
                    int dy = wParam == VK_UP ? -1 : wParam == VK_DOWN ? 1 : 0;
                    // The first key press only reveals the focus ring.
                    g_panel.focus = g_panel.showFocus ? PanelNeighbor(g_panel.focus, dx, dy)
                                                      : (g_panel.focus < 0 ? TileOfTheme(g_themeIndex) : g_panel.focus);
                    g_panel.showFocus = true;
                    WIN_CHECK(InvalidateRect(hwnd, nullptr, FALSE));
                    return 0;
                }
                case VK_SPACE:
                case VK_RETURN:
                    if (g_panel.focus >= 0) SelectTheme(g_panel.tileTheme[g_panel.focus]);
                    g_panel.showFocus = true;
                    return 0;
            }
            break;

        case WM_DPICHANGED:
            RequestRelayout();  // Layout() also relayouts the panel.
            return 0;

        case WM_NCDESTROY:
            // The tooltip is owned by the panel and is destroyed with it.
            g_panel.hwnd = nullptr;
            g_panel.tooltip = nullptr;
            g_panel.hover = g_panel.focus = -1;
            g_panel.showFocus = g_panel.tracking = false;
            g_panel.shapeW = g_panel.shapeH = g_panel.shapeLayered = -1;
            DeletePanelFonts();
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void OpenThemePanel(Note* n) {
    if (!n->hwnd) return;
    if (!g_panel.hwnd) {
        // Owned by the note, so it sits above it and dies with it.
        g_panel.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kPanelClass, L"Themes", WS_POPUP,
                                       0, 0, 10, 10, n->hwnd, nullptr, g_hinst, nullptr);
        if (!g_panel.hwnd) {
            Wh_Log(L"CreateWindowExW(themes panel) failed, error %u", GetLastError());
            return;
        }
    } else {
        SetLastError(0);
        if (!SetWindowLongPtrW(g_panel.hwnd, GWLP_HWNDPARENT, (LONG_PTR)n->hwnd) &&
            GetLastError()) {
            Wh_Log(L"Re-owning the panel failed, error %u", GetLastError());
        }
    }
    g_panel.showFocus = false;
    CreatePanelFonts();
    ComputePanelLayout();
    g_panel.focus = TileOfTheme(g_themeIndex);
    PositionPanel(n->hwnd);
    RebuildPanelTooltips();
    ShowWindow(g_panel.hwnd, SW_SHOW);
    // Opened from a click on the note's menu, so taking focus is allowed.
    if (!SetForegroundWindow(g_panel.hwnd)) {
        Wh_Log(L"SetForegroundWindow(panel) failed");
    }
    SetFocus(g_panel.hwnd);
    WIN_CHECK(InvalidateRect(g_panel.hwnd, nullptr, FALSE));
}

// ============================================================================
// Menus: "..." (title bar), Color button (row 2), body right-click
// ============================================================================

enum : UINT {
    IDM_NEW = 1,
    IDM_NUMBERING,
    IDM_CHECKLIST,
    IDM_DELETE,
    IDM_THEMES,
    IDM_MODE_SINGLE,
    IDM_MODE_MULTIPLE,
    IDM_ABOUT,
    IDM_CUT,
    IDM_COPY,
    IDM_PASTE,
    IDM_BOLD,
    IDM_ITALIC,
    IDM_HEIGHT_BASE = 100,
    IDM_COLOR_BASE = 200,
    IDM_COLOR_CUSTOM = 300,
    IDM_COLOR_FOLLOW = 301,
};


// Turns every non-empty line into a checkbox, or, if they all are already,
// back into plain text.
static void ToggleChecklist(Note* n) {
    if (n->bodyEdit) EndBodyEdit(n);
    bool allChecks = AllLinesAreCheckboxes(n->lines);
    for (Line& l : n->lines) {
        if (allChecks) {
            l.checkbox = l.done = false;
        } else if (!l.text.empty()) {
            l.checkbox = true;
        }
    }
    if (n->hwnd) WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
    ScheduleSave();
}

// Mode switch from the "..." menu. The Windhawk setting isn't touched; the
// choice is saved in notes.json and holds until the setting is changed.
static void SetMode(bool multiple) {
    if (g_multiple == multiple) return;
    g_multiple = multiple;
    Wh_Log(L"Mode switched from the menu: %s", multiple ? L"multiple" : L"single");
    Layout();
    ScheduleSave();
}

static void ShowAbout(Note* n) {
    int r = MessageBoxW(n->hwnd,
                        L"Easy Notes\n\nCreated by Torsion\nLicensed under the MIT License",
                        L"About Easy Notes", MB_OK | MB_ICONINFORMATION);
    if (r == 0) Wh_Log(L"MessageBoxW(About) failed, error %u", GetLastError());
}

static void ConfirmDelete(Note* n) {
    int id = n->id;
    int r = MessageBoxW(n->hwnd, L"Delete this note? This can't be undone.", L"Easy Notes",
                        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (r == 0) Wh_Log(L"MessageBoxW failed, error %u", GetLastError());
    if (g_quitting) return;
    // Deleted from the controller, outside this note's window procedure, so
    // the Note isn't freed under our feet.
    if (r == IDYES) {
        WIN_CHECK(PostMessageW(g_controller, WM_APP_DELETE_NOTE, (WPARAM)id, 0));
    }
}

// Runs a menu command picked from any of the three menus.
static void RunCommand(Note* n, UINT cmd) {
    bool customPick = cmd == IDM_COLOR_CUSTOM && g_settings.customValid;
    bool palettePick = cmd >= IDM_COLOR_BASE && cmd < IDM_COLOR_BASE + (UINT)kPaletteCount;
    if (customPick || palettePick || cmd == IDM_COLOR_FOLLOW) {
        // Background override for this note only.
        n->hasColor = cmd != IDM_COLOR_FOLLOW;
        if (customPick) n->color = g_settings.customColor;
        if (palettePick) n->color = kPalette[cmd - IDM_COLOR_BASE].color;
        ApplyEditorStyle(n);
        WIN_CHECK(InvalidateRect(n->hwnd, nullptr, TRUE));
        if (n->bodyEdit) WIN_CHECK(InvalidateRect(n->bodyEdit, nullptr, TRUE));
        ScheduleSave();
        return;
    }
    if (cmd >= IDM_HEIGHT_BASE && cmd < IDM_HEIGHT_BASE + 3) {
        n->height = (int)(cmd - IDM_HEIGHT_BASE);
        n->scroll = 0;
        ResolveColumn(n->col, nullptr);  // Taller note pushes the ones below.
        Layout();
        ScheduleSave();
        return;
    }
    switch (cmd) {
        case IDM_THEMES:
            OpenThemePanel(n);
            break;
        case IDM_MODE_SINGLE:
            SetMode(false);
            break;
        case IDM_MODE_MULTIPLE:
            SetMode(true);
            break;
        case IDM_NUMBERING:
            n->numbering = !n->numbering;
            WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
            ScheduleSave();
            break;
        case IDM_CHECKLIST:
            ToggleChecklist(n);
            break;
        case IDM_NEW:
            if (g_multiple) AddNote();
            break;
        case IDM_DELETE:
            ConfirmDelete(n);
            break;
        case IDM_ABOUT:
            ShowAbout(n);
            break;
        // Edit commands: only enabled while editing.
        case IDM_CUT:
            if (n->bodyEdit) SendMessageW(n->bodyEdit, WM_CUT, 0, 0);
            break;
        case IDM_COPY:
            if (n->bodyEdit) SendMessageW(n->bodyEdit, WM_COPY, 0, 0);
            break;
        case IDM_PASTE:
            if (n->bodyEdit) PastePlain(n->bodyEdit);
            break;
        case IDM_BOLD:
            ToggleEditFormat(n, kFmtBold);
            break;
        case IDM_ITALIC:
            ToggleEditFormat(n, kFmtItalic);
            break;
    }
}

// Shows `menu` for the note and runs the picked command. Menus are standard
// Win32 popups (see Known limitations: they don't follow the theme).
static void TrackNoteMenu(Note* n, HMENU menu, POINT screenPt) {
    // A popup menu only closes on outside clicks if its owner is the
    // foreground window (documented TrackPopupMenu behavior). This is a
    // direct response to the user's click. While editing, the note already
    // is the foreground window and the editor keeps the keyboard focus.
    if (!n->bodyEdit && !SetForegroundWindow(n->hwnd)) {
        Wh_Log(L"SetForegroundWindow for menu failed");
    }
    UINT cmd = (UINT)TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                                    screenPt.x, screenPt.y, 0, n->hwnd, nullptr);
    PostMessageW(n->hwnd, WM_NULL, 0, 0);
    // The mod may have been unloaded, or the window destroyed, while the
    // menu's modal loop ran.
    if (g_quitting || !n->hwnd || !cmd) return;
    RunCommand(n, cmd);
}

// "..." menu in the title bar.
static void ShowDotsMenu(Note* n, POINT screenPt) {
    HMENU menu = CreatePopupMenu();
    HMENU mode = CreatePopupMenu();
    HMENU heights = CreatePopupMenu();
    if (!menu || !mode || !heights) {
        Wh_Log(L"CreatePopupMenu failed, error %u", GetLastError());
        if (menu) DestroyMenu(menu);
        if (mode) DestroyMenu(mode);
        if (heights) DestroyMenu(heights);
        return;
    }
    WIN_CHECK(AppendMenuW(mode, MF_STRING | (g_multiple ? 0 : MF_CHECKED), IDM_MODE_SINGLE,
                          L"Single"));
    WIN_CHECK(AppendMenuW(mode, MF_STRING | (g_multiple ? MF_CHECKED : 0), IDM_MODE_MULTIPLE,
                          L"Multiple"));
    for (int i = 0; i < 3; i++) {
        WIN_CHECK(AppendMenuW(heights, MF_STRING | (n->height == i ? MF_CHECKED : 0),
                              IDM_HEIGHT_BASE + i, kHeightLabels[i]));
    }
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_THEMES, L"Themes..."));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_POPUP, (UINT_PTR)mode, L"Mode"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | (n->numbering ? MF_CHECKED : 0), IDM_NUMBERING,
                          L"Auto-numbering"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | (AllLinesAreCheckboxes(n->lines) ? MF_CHECKED : 0),
                          IDM_CHECKLIST, L"All lines as checkboxes"));
    WIN_CHECK(AppendMenuW(menu, MF_POPUP, (UINT_PTR)heights, L"Height"));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | (g_multiple ? 0 : MF_GRAYED), IDM_NEW,
                          g_multiple ? L"New note" : L"New note (Multiple mode only)"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_DELETE, L"Delete note"));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_ABOUT, L"About Easy Notes"));
    TrackNoteMenu(n, menu, screenPt);
    WIN_CHECK(DestroyMenu(menu));  // Also destroys the submenus.
}

// A small filled square with a hairline, used as a color swatch in menus.
static HBITMAP MakeSwatch(COLORREF c) {
    int sz = S(14);
    HDC screen = GetDC(nullptr);
    if (!screen) return nullptr;
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = mem ? CreateCompatibleBitmap(screen, sz, sz) : nullptr;
    if (bmp) {
        HGDIOBJ old = SelectObject(mem, bmp);
        RECT r{0, 0, sz, sz};
        FillSolid(mem, r, RGB(110, 110, 110));
        InflateRect(&r, -1, -1);
        FillSolid(mem, r, c);
        SelectObject(mem, old);
    } else {
        Wh_Log(L"Swatch bitmap creation failed, error %u", GetLastError());
    }
    if (mem) WIN_CHECK(DeleteDC(mem));
    WIN_CHECK(ReleaseDC(nullptr, screen));
    return bmp;
}

static void AppendSwatchItem(HMENU menu, UINT id, const wchar_t* label, COLORREF c,
                             bool checked, std::vector<HBITMAP>& bitmaps) {
    WIN_CHECK(AppendMenuW(menu, MF_STRING | (checked ? MF_CHECKED : 0), id, label));
    if (HBITMAP bmp = MakeSwatch(c)) {
        MENUITEMINFOW mii{};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_BITMAP;
        mii.hbmpItem = bmp;
        WIN_CHECK(SetMenuItemInfoW(menu, id, FALSE, &mii));
        bitmaps.push_back(bmp);  // Menus don't own item bitmaps.
    }
}

// Color button (row 2): theme picker plus this note's background override.
static void ShowColorMenu(Note* n, POINT screenPt) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        Wh_Log(L"CreatePopupMenu failed, error %u", GetLastError());
        return;
    }
    std::vector<HBITMAP> bitmaps;
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_THEMES, L"Themes..."));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | (n->hasColor ? 0 : MF_CHECKED), IDM_COLOR_FOLLOW,
                          L"Follow theme"));
    for (int i = 0; i < kPaletteCount; i++) {
        AppendSwatchItem(menu, IDM_COLOR_BASE + i, kPalette[i].label, kPalette[i].color,
                         n->hasColor && n->color == kPalette[i].color, bitmaps);
    }
    if (g_settings.customValid) {
        AppendSwatchItem(menu, IDM_COLOR_CUSTOM, L"Custom (hex from settings)",
                         g_settings.customColor,
                         n->hasColor && n->color == g_settings.customColor, bitmaps);
    } else {
        WIN_CHECK(AppendMenuW(menu, MF_STRING | MF_GRAYED, IDM_COLOR_CUSTOM,
                              L"Custom (hex from settings)"));
    }
    TrackNoteMenu(n, menu, screenPt);
    WIN_CHECK(DestroyMenu(menu));
    for (HBITMAP b : bitmaps) WIN_CHECK(DeleteObject(b));
}

// Right-click on the note body. Edit commands work only while editing and
// are shown disabled otherwise.
static void ShowBodyMenu(Note* n, POINT screenPt) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        Wh_Log(L"CreatePopupMenu failed, error %u", GetLastError());
        return;
    }
    UINT edit = n->bodyEdit ? 0 : MF_GRAYED;
    uint8_t fmt = EditSelectionFmt(n);
    WIN_CHECK(AppendMenuW(menu, MF_STRING | edit, IDM_CUT, L"Cut\tCtrl+X"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | edit, IDM_COPY, L"Copy\tCtrl+C"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | edit, IDM_PASTE, L"Paste\tCtrl+V"));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | edit | ((fmt & kFmtBold) ? MF_CHECKED : 0), IDM_BOLD,
                          L"Bold\tCtrl+B"));
    WIN_CHECK(AppendMenuW(menu, MF_STRING | edit | ((fmt & kFmtItalic) ? MF_CHECKED : 0),
                          IDM_ITALIC, L"Italic\tCtrl+I"));
    WIN_CHECK(AppendMenuW(menu, MF_SEPARATOR, 0, nullptr));
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_THEMES, L"Themes..."));
    WIN_CHECK(AppendMenuW(menu, MF_STRING, IDM_DELETE, L"Delete note"));
    TrackNoteMenu(n, menu, screenPt);
    WIN_CHECK(DestroyMenu(menu));
}

// Opens a menu anchored under a tool-panel button.
static POINT BelowButton(Note* n, const RECT& button) {
    POINT p{button.left, button.bottom};
    WIN_CHECK(ClientToScreen(n->hwnd, &p));
    return p;
}

// ============================================================================
// Note window procedure
// ============================================================================

static void OnNoteLeftDown(Note* n, POINT pt, bool dbl) {
    // Notes never activate, so a click on one doesn't deactivate the Themes
    // panel. Treat it as the "click outside" that closes the panel, and
    // consume it.
    if (g_panel.hwnd) {
        ClosePanel();
        return;
    }
    const Hit* hit = nullptr;
    RECT client{};
    WIN_CHECK(GetClientRect(n->hwnd, &client));
    switch (HitTestNote(n, pt, &hit)) {
        case Zone::MenuIcon:
            ShowDotsMenu(n, BelowButton(n, MenuIconRect(client)));
            break;
        case Zone::ColorIcon:
            ShowColorMenu(n, BelowButton(n, ColorIconRect(client)));
            break;
        case Zone::NumIcon:
            n->numbering = !n->numbering;
            WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
            ScheduleSave();
            break;
        case Zone::ChecklistIcon:
            ToggleChecklist(n);
            break;
        // Clicking the note doesn't move keyboard focus (notes never
        // activate), so the editor keeps its selection for these.
        case Zone::BoldIcon:
            ToggleEditFormat(n, kFmtBold);
            break;
        case Zone::ItalicIcon:
            ToggleEditFormat(n, kFmtItalic);
            break;
        case Zone::Strip:
            break;
        case Zone::Title:
            if (dbl) BeginTitleEdit(n);
            else BeginDragTracking(n);
            break;
        case Zone::Checkbox:
            // A double-click arrives as a click plus a double-click message;
            // only the first toggles, otherwise the box would flip back.
            if (hit && !dbl && hit->line < (int)n->lines.size()) {
                n->lines[hit->line].done = !n->lines[hit->line].done;
                WIN_CHECK(InvalidateRect(n->hwnd, nullptr, FALSE));
                ScheduleSave();
            }
            break;
        case Zone::Link:
            if (hit && !dbl) OpenUrl(hit->url);
            break;
        case Zone::Body:
            BeginBodyEdit(n);
            break;
        case Zone::None:
            break;
    }
}

static LRESULT CALLBACK NoteWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
    Note* n = (Note*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (msg == WM_NCCREATE) {
        auto cs = (CREATESTRUCTW*)lParam;
        n = (Note*)cs->lpCreateParams;
        SetLastError(0);
        if (!SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)n) && GetLastError()) {
            Wh_Log(L"SetWindowLongPtrW(GWLP_USERDATA) failed, error %u", GetLastError());
            return FALSE;
        }
        n->hwnd = hwnd;
    }
    if (!n) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;  // Never take focus on a click.

        case WM_WINDOWPOSCHANGING: {
            // Keep notes at desktop level: any Z-order change (for example
            // from activation) becomes "directly above the desktop host".
            // Skipped while editing, so the note being typed in stays visible,
            // and while it's being dragged.
            auto wp = (WINDOWPOS*)lParam;
            // Show desktop (Win+D) hides top-level windows with
            // SWP_HIDEWINDOW. A note stays on the desktop unless the mod
            // hides it itself (single mode, deleting the note, shutdown).
            if ((wp->flags & SWP_HIDEWINDOW) && !n->hiding && !g_quitting) {
                wp->flags &= ~SWP_HIDEWINDOW;
            }
            if (!(wp->flags & SWP_NOZORDER) && !n->bodyEdit && !n->titleEdit &&
                g_drag.note != n) {
                HWND after = nullptr;
                if (DesktopInsertAfter(hwnd, &after)) wp->hwndInsertAfter = after;
                else if (g_host) wp->flags |= SWP_NOZORDER;
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PaintNote(n);
            return 0;

        case WM_SIZE: {
            RECT client{0, 0, LOWORD(lParam), HIWORD(lParam)};
            if (n->bodyEdit) {
                RECT b = BodyRect(client);
                WIN_CHECK(MoveWindow(n->bodyEdit, b.left, b.top, b.right - b.left,
                                     b.bottom - b.top, TRUE));
            }
            if (n->titleEdit) {
                RECT t = TitleTextRect(client);
                int h = g_fonts.lineH + S(4);
                WIN_CHECK(MoveWindow(n->titleEdit, t.left, (S(kTitleDip) - h) / 2,
                                     t.right - t.left, h, TRUE));
            }
            return 0;
        }

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt;
                if (GetCursorPos(&pt) && ScreenToClient(hwnd, &pt)) {
                    LPCWSTR id = IDC_ARROW;
                    switch (HitTestNote(n, pt, nullptr)) {
                        case Zone::Title: id = IDC_SIZEALL; break;
                        case Zone::BoldIcon:
                        case Zone::ItalicIcon:
                            id = n->bodyEdit ? IDC_HAND : IDC_ARROW;
                            break;
                        case Zone::NumIcon:
                        case Zone::ChecklistIcon:
                        case Zone::ColorIcon:
                        case Zone::MenuIcon:
                        case Zone::Checkbox:
                        case Zone::Link: id = IDC_HAND; break;
                        case Zone::Body: id = IDC_IBEAM; break;
                        case Zone::Strip:
                        case Zone::None: break;
                    }
                    if (HCURSOR c = WIN_CHECK(LoadCursorW(nullptr, id))) SetCursor(c);
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            OnNoteLeftDown(n, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)},
                           msg == WM_LBUTTONDBLCLK);
            return 0;

        case WM_MOUSEMOVE:
            if (g_drag.note == n && GetCapture() == hwnd) UpdateDrag(n);
            return 0;

        case WM_LBUTTONUP:
            if (g_drag.note == n) {
                bool moved = g_drag.moving;
                g_drag.committing = true;
                if (GetCapture() == hwnd) WIN_CHECK(ReleaseCapture());
                if (moved) CommitDrop(n);
                g_drag = {};
            }
            return 0;

        case WM_CAPTURECHANGED:
            // Capture lost without a button-up (Esc, Alt+Tab, ...): cancel
            // the drag and snap back.
            if (g_drag.note == n && !g_drag.committing) {
                bool moved = g_drag.moving;
                g_drag = {};
                HideIndicator();
                if (moved) Layout();
            }
            return 0;

        case WM_CONTEXTMENU: {
            // Also arrives from the rich edit (its default handling passes
            // WM_CONTEXTMENU up to the parent).
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            POINT local{};
            if (lParam == -1) {  // Keyboard: open at the top of the body.
                RECT client{};
                WIN_CHECK(GetClientRect(hwnd, &client));
                RECT b = BodyRect(client);
                pt = POINT{b.left, b.top};
                local = pt;
                WIN_CHECK(ClientToScreen(hwnd, &pt));
            } else {
                local = pt;
                WIN_CHECK(ScreenToClient(hwnd, &local));
            }
            ClosePanel();  // A right-click on a note is a click outside it.
            // Title bar and tool strip: the "..." menu. Body: the edit menu.
            if ((HWND)wParam == hwnd && lParam != -1 && local.y < S(kTitleDip) + S(kStripDip)) {
                ShowDotsMenu(n, pt);
            } else {
                ShowBodyMenu(n, pt);
            }
            return 0;
        }

        case WM_MOUSEWHEEL:
            if (!n->bodyEdit) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                n->scroll -= MulDiv(delta, g_fonts.lineH * 3, WHEEL_DELTA);
                n->scroll = std::max(0, n->scroll);  // Upper clamp in DrawNote.
                WIN_CHECK(InvalidateRect(hwnd, nullptr, FALSE));
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BODY_EDIT && HIWORD(wParam) == EN_CHANGE &&
                n->bodyEdit && !n->suppressChange) {
                // The model is synced from the editor when the autosave runs
                // (and when editing ends), so typing stays cheap.
                n->editDirty = true;
                ScheduleSave();
            }
            return 0;

        case WM_NOTIFY: {
            // Selection moved in the rich edit: refresh the B / I buttons.
            auto hdr = (NMHDR*)lParam;
            if (hdr && hdr->idFrom == ID_BODY_EDIT && hdr->code == EN_SELCHANGE &&
                !n->suppressChange) {
                InvalidateStrip(n);
            }
            return 0;
        }

        case WM_CTLCOLOREDIT: {
            // Only the title edit (a plain EDIT) asks for colors; the rich
            // edit is styled through ApplyEditorStyle.
            HDC dc = (HDC)wParam;
            Look look = EffectiveLook(n);
            EnsureBrushes(n, look);
            SetTextColor(dc, look.text);
            SetBkColor(dc, look.title);
            return (LRESULT)n->titleBrush;
        }

        case WM_APP_ENDEDIT: {
            HWND which = (HWND)lParam;
            bool cancel = (wParam & ENDEDIT_CANCEL) != 0;
            if ((wParam & 0xF) == ENDEDIT_TITLE) {
                if (n->titleEdit == which) EndTitleEdit(n, !cancel);
            } else if (n->bodyEdit == which) {
                EndBodyEdit(n);
            }
            return 0;
        }

        case WM_DPICHANGED:
            // All sizes derive from the primary monitor's DPI, so relayout
            // everything instead of using the suggested rectangle.
            RequestRelayout();
            return 0;

        case WM_NCDESTROY:
            // The watchdog recreates the window if it was destroyed by
            // anything other than a shutdown.
            if (g_drag.note == n) g_drag = {};
            n->hwnd = nullptr;
            n->bodyEdit = nullptr;
            n->titleEdit = nullptr;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Activation, re-attaching and the watchdog
// ============================================================================

// Switches to a new desktop window (Explorer restarted, or the icons moved
// to another WorkerW) and stacks every note above it again.
static void RepinToHost(HWND host) {
    g_host = host;
    for (auto& n : g_notes) {
        if (n->hwnd) PlaceAboveHost(n->hwnd);
    }
}

static void RecreateMissingWindows() {
    bool any = false;
    for (auto& n : g_notes) {
        if (!n->hwnd) {
            any |= CreateNoteWindow(n.get());
        }
    }
    if (any) Layout();
}

// Runs once the desktop shell exists. If the notes can't be loaded safely
// (storage folder not ready, notes.json unreadable), it does nothing and the
// watchdog calls it again 2 seconds later.
static void TryActivate() {
    if (g_active) return;
    if (!GetShellWindow()) return;  // Shell not up yet; the watchdog retries.
    if (!InitPaths()) {
        Wh_Log(L"Storage folder not available yet; retrying");
        return;
    }
    g_host = FindDesktopHost();
    Wh_Log(L"Desktop host: %p", g_host);
    g_geo = ComputeGeo();
    CreateFonts();
    if (!LoadNotes()) return;
    // Settings changed while the mod was off win over the stored choice.
    if (SyncFromSettings()) ScheduleSave();
    for (auto& n : g_notes) {
        CreateNoteWindow(n.get());
    }
    g_active = true;
    Layout();
}

static void Watchdog() {
    if (g_quitting) return;
    if (!g_active) {
        TryActivate();
        return;
    }
    // Follow the desktop if Explorer recreated it or DefView moved to a
    // different WorkerW (wallpaper changes, Explorer restarts, Win+D).
    HWND host = FindDesktopHost();
    if (host && host != g_host) {
        Wh_Log(L"Desktop host changed %p -> %p; re-stacking notes", g_host, host);
        RepinToHost(host);
    }
    RecreateMissingWindows();
    // Bring back a note that something hid, and restore its stacking if
    // something raised the desktop above it (Show desktop, wallpaper tools).
    // PlaceAboveHost does nothing for a note that is already in place.
    for (auto& p : g_notes) {
        Note* n = p.get();
        if (!n->hwnd || !IsVisibleNote(n) || n->bodyEdit || n->titleEdit || g_drag.note == n) {
            continue;
        }
        if (!IsWindowVisible(n->hwnd)) ShowWindow(n->hwnd, SW_SHOWNOACTIVATE);
        PlaceAboveHost(n->hwnd);
    }
    // Covers resolution, work area and DPI changes that weren't broadcast.
    Geo geo = ComputeGeo();
    if (!SameGeo(geo, g_geo)) Layout();
}

static void Shutdown() {
    if (g_quitting) return;
    g_quitting = true;
    Wh_Log(L"Shutting down UI thread");
    EndMenu();  // Close an open context menu, if any.
    if (GetCapture()) ReleaseCapture();
    g_drag = {};
    ClosePanel();
    for (auto& n : g_notes) {
        DestroyNoteWindow(n.get());  // Commits any active edit first.
    }
    if (g_dirty) SaveNow();
    if (g_indicator) WIN_CHECK(DestroyWindow(g_indicator));
    KillTimer(g_controller, TIMER_WATCH);
    KillTimer(g_controller, TIMER_RELAYOUT);
    KillTimer(g_controller, TIMER_SAVE);
    WIN_CHECK(DestroyWindow(g_controller));  // WM_DESTROY posts WM_QUIT.
}

static void OnSettingsChanged() {
    LoadSettings();
    if (!g_active) return;
    // Mode or Theme changed in the settings: the settings now win.
    if (SyncFromSettings()) ScheduleSave();
    CreateFonts();
    for (auto& n : g_notes) {
        n->scroll = 0;
        if (n->hwnd) ApplyOpacity(n.get());
        ApplyEditorStyle(n.get());
    }
    Layout();  // Also repaints every note with the new look.
}

// ============================================================================
// Controller window (hidden top-level window that receives broadcasts)
// ============================================================================

static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam) {
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg) {
        // Explorer recreated its shell windows: re-stack right away.
        if (g_active && !g_quitting) {
            Wh_Log(L"TaskbarCreated received; re-stacking notes");
            if (HWND host = FindDesktopHost()) RepinToHost(host);
            RecreateMissingWindows();
            RequestRelayout();
        }
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_SAVE) {
                SaveNow();
            } else if (wParam == TIMER_WATCH) {
                Watchdog();
            } else if (wParam == TIMER_RELAYOUT) {
                KillTimer(hwnd, TIMER_RELAYOUT);
                Layout();
            }
            return 0;
        case WM_APP_SHUTDOWN:
            Shutdown();
            return 0;
        case WM_APP_SETTINGS:
            if (!g_quitting) OnSettingsChanged();
            return 0;
        case WM_APP_DELETE_NOTE:
            if (!g_quitting) DeleteNoteById((int)wParam);
            return 0;
        case WM_APP_RELAYOUT:
            RequestRelayout();
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            RequestRelayout();
            return 0;
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) RequestRelayout();
            break;
        case WM_ENDSESSION:
            // Logoff or shutdown: flush edits made in the last moments.
            if (wParam && g_dirty) SaveNow();
            return 0;
        case WM_DESTROY:
            if (g_controller == hwnd) g_controller = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ============================================================================
// UI thread
// ============================================================================

static bool RegisterClasses() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = g_hinst;

    wc.lpfnWndProc = ControllerWndProc;
    wc.lpszClassName = kControllerClass;
    g_classController = RegisterClassExW(&wc) != 0;
    if (!g_classController) {
        Wh_Log(L"RegisterClassExW(controller) failed, error %u", GetLastError());
        return false;
    }

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = NoteWndProc;
    wc.lpszClassName = kNoteClass;
    g_classNote = RegisterClassExW(&wc) != 0;
    if (!g_classNote) {
        Wh_Log(L"RegisterClassExW(note) failed, error %u", GetLastError());
        return false;
    }

    wc.style = 0;
    wc.lpfnWndProc = IndicatorWndProc;
    wc.lpszClassName = kIndicatorClass;
    g_classIndicator = RegisterClassExW(&wc) != 0;
    if (!g_classIndicator) {
        Wh_Log(L"RegisterClassExW(indicator) failed, error %u", GetLastError());
        return false;
    }

    wc.lpfnWndProc = PanelWndProc;
    wc.lpszClassName = kPanelClass;
    g_classPanel = RegisterClassExW(&wc) != 0;
    if (!g_classPanel) {
        Wh_Log(L"RegisterClassExW(panel) failed, error %u", GetLastError());
        return false;
    }
    return true;
}

static void UnregisterClasses() {
    if (g_classNote) WIN_CHECK(UnregisterClassW(kNoteClass, g_hinst));
    if (g_classIndicator) WIN_CHECK(UnregisterClassW(kIndicatorClass, g_hinst));
    if (g_classPanel) WIN_CHECK(UnregisterClassW(kPanelClass, g_hinst));
    if (g_classController) WIN_CHECK(UnregisterClassW(kControllerClass, g_hinst));
    g_classNote = g_classIndicator = g_classPanel = g_classController = false;
}

static DWORD WINAPI UiThreadProc(void*) {
    HRESULT hrCo = HR_CHECK(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    // Per-monitor DPI awareness (v2) for every window this thread creates.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto setCtx = (SetThreadDpiAwarenessContext_t)GetProcAddress(
            user32, "SetThreadDpiAwarenessContext");
        // (DPI_AWARENESS_CONTEXT)-4 == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2.
        if (!setCtx || !setCtx((void*)(INT_PTR)-4)) {
            Wh_Log(L"SetThreadDpiAwarenessContext(PMv2) unavailable or failed");
        }
    }
    g_shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_shcore) {
        g_pGetDpiForMonitor =
            (GetDpiForMonitor_t)GetProcAddress(g_shcore, "GetDpiForMonitor");
    } else {
        Wh_Log(L"LoadLibraryExW(shcore.dll) failed, error %u", GetLastError());
    }
    // Registers the RICHEDIT50W class used by the body editor.
    g_msftedit = LoadLibraryExW(L"Msftedit.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_msftedit) {
        Wh_Log(L"LoadLibraryExW(Msftedit.dll) failed, error %u; editing disabled",
               GetLastError());
    }

    LoadSettings();
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"RegisterWindowMessageW failed, error %u", GetLastError());
    }

    bool ok = RegisterClasses();
    if (ok) {
        // A hidden top-level window (not message-only), so it receives
        // broadcasts like WM_DISPLAYCHANGE and TaskbarCreated.
        g_controller = CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"",
                                       WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                       g_hinst, nullptr);
        if (!g_controller) {
            Wh_Log(L"CreateWindowExW(controller) failed, error %u", GetLastError());
            ok = false;
        }
    }
    g_startOk = ok;
    WIN_CHECK(SetEvent(g_readyEvent));

    if (ok) {
        TryActivate();
        if (!SetTimer(g_controller, TIMER_WATCH, kWatchIntervalMs, nullptr)) {
            Wh_Log(L"SetTimer(watch) failed, error %u", GetLastError());
        }
        MSG msg;
        BOOL r;
        while ((r = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
            if (r == -1) {
                Wh_Log(L"GetMessageW failed, error %u", GetLastError());
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // Cleanup on the owning thread. Shutdown() normally did most of this
    // already; this also covers an abnormal loop exit.
    g_quitting = true;
    for (auto& n : g_notes) DestroyNoteWindow(n.get());
    if (g_dirty) SaveNow();
    if (g_indicator) WIN_CHECK(DestroyWindow(g_indicator));
    if (g_controller) WIN_CHECK(DestroyWindow(g_controller));
    g_notes.clear();
    DeleteFonts(g_fonts);
    UnregisterClasses();
    if (g_shcore) {
        WIN_CHECK(FreeLibrary(g_shcore));
        g_shcore = nullptr;
        g_pGetDpiForMonitor = nullptr;
    }
    if (g_msftedit) {  // After every rich edit window is gone.
        WIN_CHECK(FreeLibrary(g_msftedit));
        g_msftedit = nullptr;
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();
    Wh_Log(L"UI thread exited");
    return 0;
}

// ============================================================================
// Tool mod entry points
//
// The mod runs in its own windhawk.exe process (see the launcher below), so
// a crash or stall in the notes UI can't take the taskbar or desktop down.
// ============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)&g_hinst, (HMODULE*)&g_hinst)) {
        Wh_Log(L"GetModuleHandleExW failed, error %u", GetLastError());
        return FALSE;
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEventW failed, error %u", GetLastError());
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed, error %u", GetLastError());
        WIN_CHECK(CloseHandle(g_readyEvent));
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Wait until the controller window exists (or startup failed), so that
    // WhTool_ModUninit and WhTool_ModSettingsChanged always have a target.
    HANDLE handles[] = {g_readyEvent, g_thread};
    DWORD w = WaitForMultipleObjects(2, handles, FALSE, 10000);
    if (w != WAIT_OBJECT_0 || !g_startOk) {
        Wh_Log(L"UI thread failed to start (wait result %u)", w);
        if (g_controller) PostMessageW(g_controller, WM_APP_SHUTDOWN, 0, 0);
        else PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        WIN_CHECK(CloseHandle(g_thread));
        g_thread = nullptr;
        WIN_CHECK(CloseHandle(g_readyEvent));
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_thread) {
        // Ask the UI thread to destroy its windows, save and quit its loop.
        // If the controller is gone, post WM_QUIT to the thread directly.
        HWND controller = g_controller;
        if (!controller || !PostMessageW(controller, WM_APP_SHUTDOWN, 0, 0)) {
            if (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
                Wh_Log(L"PostThreadMessageW(WM_QUIT) failed, error %u", GetLastError());
            }
        }
        // Must join: the thread runs code from this DLL, which is unloaded
        // when this function returns.
        if (WaitForSingleObject(g_thread, INFINITE) != WAIT_OBJECT_0) {
            Wh_Log(L"WaitForSingleObject(thread) failed, error %u", GetLastError());
        }
        WIN_CHECK(CloseHandle(g_thread));
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        WIN_CHECK(CloseHandle(g_readyEvent));
        g_readyEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    // Settings are applied on the UI thread, which owns all state.
    if (g_controller && !PostMessageW(g_controller, WM_APP_SETTINGS, 0, 0)) {
        Wh_Log(L"PostMessageW(settings) failed, error %u", GetLastError());
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
