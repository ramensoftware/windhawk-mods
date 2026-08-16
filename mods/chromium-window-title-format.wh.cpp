// ==WindhawkMod==
// @id              chromium-window-title-format
// @name            Chromium Window Title Format
// @description     Customize how Edge and Chrome compose window titles - drop the browser suffix, restyle the tab count and profile, rebuild the title from a template, and optionally use the active tab's favicon as the window icon.
// @version         1.0
// @author          mazany
// @github          https://github.com/mazany
// @twitter         https://x.com/tomazany
// @donateUrl       https://ko-fi.com/mazany
// @include         msedge.exe
// @include         chrome.exe
// @architecture    x86-64
// @license         MIT
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// chrome.exe is back: the probe in poc/ finished and identified
// TabStripModel::AddTab as the hook that seeds Chrome's structural proof. Keep
// that probe DISABLED whenever this mod includes chrome.exe - two mods symbol-
// hooking the same chrome.dll functions would confuse any measurement and, if
// something went wrong, the blame.
//
// Note the metadata block above is entirely `//` lines, so an @include cannot
// be "commented out" - it has to be deleted. Leaving it prefixed does nothing,
// and prose inside that block fails windhawk-cli with exit 2.

// ==WindhawkModReadme==
/*
# Chromium Window Title Format

Rewrites Microsoft Edge and Google Chrome window titles using a template you
control, so a window with many tabs is identifiable at a glance in Alt+Tab, the
taskbar and jump lists.

![Before and after, on Edge and Chrome](https://raw.githubusercontent.com/mazany/windhawk-images/7a382537a7e63a8645df1d3599f30cda286128d3/chromium-window-title-format/before-after.png)

Taskbar previews before and after, on both browsers. The bottom row is the same
window list under two different templates - bold digits, and a superscript count
with the profile in italics.

Default title:

    E-Mail and 16 more pages - Personal - Microsoft Edge

Some things you can ask for instead:

    E-Mail and 16 more pages - Personal      {title}?( {more})?( - {profile})
    E-Mail and 16 more pages                 {title}?( {more})
    [17] E-Mail                              ?([{count}] ){title}
    17 E-Mail (bold digits)                  ?({count:bold} ){title}
    03 E-Mail - Personal (superscript+italic) ?({count:pad2:sup} ){title}?( — {profile:italic})

## How it works, and what it will not touch

The mod never guesses at English text. On startup it reads the browser's **own
localized resource file** to learn the exact browser suffix, the profile
separator and the page-count wording for your language, then matches those
literals right-anchored against each title.

If a title does not match the grammar it discovered, the title is left **exactly
as it is**. That is a hard rule, not a setting, and it is what keeps the
following safe:

- app and installed-web-app (PWA) windows
- picture-in-picture, DevTools, print preview, task manager
- dialogs
- **windows you named yourself** with the browser's own "Name window" command

Windows you named yourself are the one exception, and only when the optional
symbol layer below is switched on. A named window's title is *only* the name you
gave it - no tab count, no page title, no profile - so there is nothing in the
text for a template to work from, and without that layer the mod leaves it
alone. With it, the name keeps its own `{name}` token and gains a live tab
count.

## Edge and Chrome are not equally capable

Chrome's window title contains **only** the page title and the browser name -
there is no tab count and no profile anywhere in it. That is a fact about
Chrome, not a limitation of the mod, so `{extra}`, `{more}`, `{profile}` and
`{private}` are always empty there and any `?( ... )` group containing only
those tokens disappears. `{title}` and `{browser}` work on both.

`{count}` is the exception. It is a property of the window rather than of the
text, so with the optional symbol layer on it works on Chrome too - the count
comes from the browser's own tab strip instead of being parsed out of the title.
With that layer off, Chrome has no count to give and `{count}` is empty.

## Template syntax

| Token | Meaning |
| --- | --- |
| `{title}` | Active tab title |
| `{more}` | The browser's own count phrase, e.g. `and 16 more pages` (Edge) |
| `{count}` | Total tabs, including the active one (Edge) |
| `{extra}` | Just the number from the count phrase (Edge) |
| `{profile}` | Profile name, or the privacy marker (Edge) |
| `{private}` | `InPrivate` / `Guest` without brackets, else empty (Edge) |
| `{browser}` | Browser name as it appears in the suffix |

Modifiers chain with `:` - `{count:pad2:sup}`, `{profile:italic}`.

- Numeric: `pad2`, `pad3` (zero-pad), `min2` (empty unless 2 or more)
- Style: `sup`, `sub` (digits only), `bold`, `italic`, `bolditalic`, `sans`,
  `serifbold`, `serifitalic`, `mono`, `script`, `fraktur`, `dbl`
- Text: `upper`, `lower`, `trim`, `max<N>`

`?( ... )` drops the whole group, literal text included, when every token inside
is empty. Escapes: `\{ \} \? \( \) \\` and `\uXXXX`.

## A caution about the fancy letter styles

`bold`, `italic` and friends use Unicode mathematical alphanumerics. They look
great in the taskbar but they are not plain letters, which has real costs:

- Screen readers announce them character by character. The window's
  accessibility name is the title you display, so styling `{title}` degrades it.
- Alt+Tab type-to-search stops matching what you type.
- They need a font that covers the range. Superscript and subscript digits live
  in Segoe UI itself and are safe; the mathematical letters come from Segoe UI
  Symbol, so on a trimmed Windows image they can render as boxes.
- The italic, bold-italic, script and fraktur styles contain **no digits**, so
  digits pass through unstyled.

Style short prefixes like `{count}`, not `{title}`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Format:
    - Preset: keep_profile
      $name: Title format
      $options:
        - keep_profile: "Inbox and 16 more pages - Personal   (drop only the browser name)"
        - with_count: "Inbox and 16 more pages"
        - bracket: "[17] Inbox"
        - bold: "𝟭𝟳 Inbox   (bold digits)"
        - sup: "¹⁷ Inbox   (superscript digits)"
        - sup_pad: "⁰⁶ Inbox   (superscript, always two digits)"
        - sup_pad_profile: "⁰⁶ Inbox — 𝘗𝘦𝘳𝘴𝘰𝘯𝘢𝘭   (superscript count, italic profile)"
        - title_only: "Inbox   (page title only)"
        - custom: "Custom - use the template below"
      $description: >-
        Each option shows how a 17-tab window in the Personal profile would look.
        The tab count and profile exist only on Edge, so on Chrome every option
        collapses to the page title. Pick Custom to write your own.
    - Custom: "?({count:pad2:sup} ){title}"
      $name: Custom template
      $description: >-
        Used only when the format above is set to Custom. Tokens: {title} {more}
        {count} {extra} {profile} {private} {browser}. Wrap optional parts in
        ?( ... ) so the whole group vanishes when every token inside is empty.
        Modifiers chain with a colon, e.g. {count:pad2:sup} or {profile:italic}.
        Escapes: \{ \} \? \( \) \\ and \uXXXX.
    - Named: "?({count:pad2:sup} ){name}"
      $name: Windows you have named
      $description: >-
        Used only for windows named with the browser's own naming command, and
        only when the optional symbol layer below is on and has verified itself.
        {name} is the name you gave the window; {count} is its live tab count.
        With the layer off this is ignored and named windows are left untouched.
    - ChromeOverride: ""
      $name: Chrome template override
      $description: >-
        Optional, and always a raw template rather than a preset. Chrome titles
        carry no tab count and no profile, so a simpler template often reads
        better there. Leave empty to use the format chosen above.
  $name: Title format
  $description: >-
    A title the mod cannot recognize as a browser title is always left exactly
    as it is - app windows, picture in picture, dialogs, and windows named with
    the browser's own naming command. That is deliberate and not configurable.
- Parsing:
    - Enabled: true
      $name: Rewrite titles
      $description: >-
        Turn off to leave every title untouched while keeping the mod loaded.
    - BrowserSuffix: ""
      $name: Browser suffix override
      $description: >-
        Leave empty for automatic discovery. Set only if the log reports that
        discovery failed for your build. This is the whole literal tail
        including its leading separator, for example " - Google Chrome".
  $name: Title parsing
- Symbols:
    - Enabled: false
      $name: Read tab counts from the browser (experimental)
      $description: >-
        Off by default, and everything above works without it. Turning it on
        lets the mod reformat windows you have named - which is otherwise
        impossible, because a named window's title is only the name, with no tab
        count, no page title and no profile in it anywhere.
        .
        It works by resolving a few of the browser's internal function names
        from its debug symbols, which are downloaded once per browser build (the
        first run after an update can take a few minutes and needs the network).
        If any of them cannot be resolved, this feature switches itself off and
        the mod carries on normally.
        .
        The mod never simply trusts what it reads. It first compares the tab
        count it gets from the browser against the count parsed out of ordinary
        window titles, where the answer is already known, and only starts using
        it after several agreements in a row. One disagreement disables it for
        good until the browser restarts. Check the log to see which happened.
    - Favicon: false
      $name: Use the active tab's icon as the window icon
      $description: >-
        Replaces the browser's own icon in the taskbar and Alt+Tab with the
        favicon of the window's active tab. Needs the symbol layer above to be
        on, since the icon comes from the browser rather than from the title.
        The original icon is restored when the mod is turned off.
  $name: Advanced (experimental)
- Advanced:
    - MaxTitleChars: 512
      $name: Maximum title length
      $description: >-
        Safety clamp. Truncation never splits a character and appends an
        ellipsis.
    - VerboseLogging: false
      $name: Verbose logging
      $description: >-
        Log every parse and compose decision. Discovery results and failures are
        logged regardless of this setting.
  $name: Advanced
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// ---------------------------------------------------------------------------
// Unicode styling
//
// Every table below was derived empirically from the actual characters, not
// from memory, because two traps make recall unreliable:
//
//   1. Superscript digits are NOT contiguous. 0 and 4-9 live at U+2070..U+2079,
//      but 1, 2 and 3 are legacy Latin-1 characters (U+00B9, U+00B2, U+00B3).
//      A base+offset mapping silently emits the wrong glyphs.
//   2. Four of the mathematical alphabets have HOLES where a letter was already
//      encoded in the Letterlike Symbols block. base + (c - 'a') lands on an
//      unassigned codepoint there and renders as .notdef.
//
// Verified hole sets: Italic {h}; Script {B,E,F,H,I,L,M,R,e,g,o}; Fraktur
// {C,H,I,R,Z}; DoubleStruck {C,H,N,P,Q,R,Z}. Every other shipped style is
// hole-free and safe for base+offset.
// ---------------------------------------------------------------------------

namespace style {

enum class Kind {
    kNone,
    kSuper,
    kSub,
    kBold,            // serif bold
    kItalic,          // sans-serif italic  (what "italic" means here)
    kBoldItalic,      // sans-serif bold italic
    kSans,
    kSansBold,        // what "bold" digits resolve to; see kBold note below
    kSerifItalic,
    kMono,
    kScript,
    kFraktur,
    kDouble,
};

// Superscript / subscript digit tables. Subscripts ARE contiguous
// (U+2080..U+2089); superscripts are not.
constexpr wchar_t kSuperDigits[10] = {0x2070, 0x00B9, 0x00B2, 0x00B3, 0x2074,
                                      0x2075, 0x2076, 0x2077, 0x2078, 0x2079};
constexpr wchar_t kSubDigits[10] = {0x2080, 0x2081, 0x2082, 0x2083, 0x2084,
                                    0x2085, 0x2086, 0x2087, 0x2088, 0x2089};

struct Plane {
    uint32_t upper;   // base for 'A'
    uint32_t lower;   // base for 'a'
    uint32_t digits;  // base for '0', or 0 when the alphabet has no digits
};

// digits == 0 means "this alphabet has no digits, pass them through as ASCII".
// That is not an omission: the italic, bold-italic, script and fraktur ranges
// genuinely contain no digits. It is also why the canonical example pairs
// superscript digits with italic letters.
constexpr Plane PlaneFor(Kind k) {
    switch (k) {
        case Kind::kBold:        return {0x1D400, 0x1D41A, 0x1D7CE};
        case Kind::kSerifItalic: return {0x1D434, 0x1D44E, 0};
        case Kind::kBoldItalic:  return {0x1D63C, 0x1D656, 0};
        case Kind::kSans:        return {0x1D5A0, 0x1D5BA, 0x1D7E2};
        case Kind::kSansBold:    return {0x1D5D4, 0x1D5EE, 0x1D7EC};
        case Kind::kItalic:      return {0x1D608, 0x1D622, 0};
        case Kind::kMono:        return {0x1D670, 0x1D68A, 0x1D7F6};
        case Kind::kScript:      return {0x1D49C, 0x1D4B6, 0};
        case Kind::kFraktur:     return {0x1D504, 0x1D51E, 0};
        case Kind::kDouble:      return {0x1D538, 0x1D552, 0x1D7D8};
        default:                 return {0, 0, 0};
    }
}

// The 24 verified holes and their correct substitutes.
uint32_t HoleSubstitute(Kind k, wchar_t c) {
    switch (k) {
        case Kind::kSerifItalic:
            return (c == L'h') ? 0x210E : 0;  // PLANCK CONSTANT
        case Kind::kScript:
            switch (c) {
                case L'B': return 0x212C; case L'E': return 0x2130;
                case L'F': return 0x2131; case L'H': return 0x210B;
                case L'I': return 0x2110; case L'L': return 0x2112;
                case L'M': return 0x2133; case L'R': return 0x211B;
                case L'e': return 0x212F; case L'g': return 0x210A;
                case L'o': return 0x2134; default: return 0;
            }
        case Kind::kFraktur:
            switch (c) {
                case L'C': return 0x212D; case L'H': return 0x210C;
                case L'I': return 0x2111; case L'R': return 0x211C;
                case L'Z': return 0x2128; default: return 0;
            }
        case Kind::kDouble:
            switch (c) {
                case L'C': return 0x2102; case L'H': return 0x210D;
                case L'N': return 0x2115; case L'P': return 0x2119;
                case L'Q': return 0x211A; case L'R': return 0x211D;
                case L'Z': return 0x2124; default: return 0;
            }
        default:
            return 0;
    }
}

void AppendCodepoint(std::wstring& out, uint32_t cp) {
    if (cp < 0x10000) {
        out.push_back(static_cast<wchar_t>(cp));
        return;
    }
    // Non-BMP: emit a surrogate pair. Every mathematical alphanumeric is here,
    // which is why styled text is twice the UTF-16 length of its input and why
    // truncation must be surrogate-aware.
    cp -= 0x10000;
    out.push_back(static_cast<wchar_t>(0xD800 + (cp >> 10)));
    out.push_back(static_cast<wchar_t>(0xDC00 + (cp & 0x3FF)));
}

// Identity for every character outside [A-Za-z0-9]. Accented letters, spaces,
// dashes and CJK all pass through byte-exact - styling must never alter the
// user's own text, and there is no correct way to "style" a letter the target
// alphabet does not contain.
std::wstring Apply(std::wstring_view in, Kind k) {
    if (k == Kind::kNone) {
        return std::wstring(in);
    }
    std::wstring out;
    out.reserve(in.size() * 2);

    if (k == Kind::kSuper || k == Kind::kSub) {
        const wchar_t* table = (k == Kind::kSuper) ? kSuperDigits : kSubDigits;
        for (wchar_t c : in) {
            // Digits only. Superscript letters are an incomplete set (no
            // capital S/X/Y/Z at all), so letters pass through unchanged rather
            // than producing a half-styled word.
            out.push_back((c >= L'0' && c <= L'9') ? table[c - L'0'] : c);
        }
        return out;
    }

    const Plane p = PlaneFor(k);
    for (wchar_t c : in) {
        if (const uint32_t sub = HoleSubstitute(k, c); sub) {
            AppendCodepoint(out, sub);
        } else if (c >= L'A' && c <= L'Z') {
            AppendCodepoint(out, p.upper + (c - L'A'));
        } else if (c >= L'a' && c <= L'z') {
            AppendCodepoint(out, p.lower + (c - L'a'));
        } else if (c >= L'0' && c <= L'9' && p.digits) {
            AppendCodepoint(out, p.digits + (c - L'0'));
        } else {
            out.push_back(c);
        }
    }
    return out;
}

Kind FromName(std::wstring_view n) {
    if (n == L"sup")         return Kind::kSuper;
    if (n == L"sub")         return Kind::kSub;
    if (n == L"bold")        return Kind::kSansBold;
    if (n == L"italic")      return Kind::kItalic;
    if (n == L"bolditalic")  return Kind::kBoldItalic;
    if (n == L"sans")        return Kind::kSans;
    if (n == L"serifbold")   return Kind::kBold;
    if (n == L"serifitalic") return Kind::kSerifItalic;
    if (n == L"mono")        return Kind::kMono;
    if (n == L"script")      return Kind::kScript;
    if (n == L"fraktur")     return Kind::kFraktur;
    if (n == L"dbl")         return Kind::kDouble;
    return Kind::kNone;
}

// Truncate to `maxChars` UTF-16 units without splitting a surrogate pair, then
// append an ellipsis.
std::wstring Clamp(std::wstring s, size_t maxChars) {
    if (maxChars == 0 || s.size() <= maxChars) {
        return s;
    }
    size_t cut = maxChars;
    if (cut > 0 && s[cut - 1] >= 0xD800 && s[cut - 1] <= 0xDBFF) {
        --cut;  // do not leave a dangling high surrogate
    }
    s.resize(cut);
    s.push_back(0x2026);  // HORIZONTAL ELLIPSIS
    return s;
}

}  // namespace style

// ---------------------------------------------------------------------------
// .pak (DataPack) reader
//
// Two layouts exist and the version field says 5 for both, so they cannot be
// told apart by header value. Guessing wrong does not crash - it yields
// plausible garbage, which is the worst possible failure mode. Each candidate
// layout is therefore structurally validated before its contents are trusted,
// and if neither validates the mod runs in pass-through rather than falling
// back to English guesses.
//
//   Chrome: 12-byte header, 6-byte entries, 16-bit resource ids.
//   Edge:   16-byte header, 8-byte entries, 32-bit resource ids.
// ---------------------------------------------------------------------------

namespace pak {

struct Entry {
    uint32_t id;
    uint32_t begin;
    uint32_t end;
};

struct File {
    bool                 ok = false;
    std::vector<Entry>   entries;
    const uint8_t*       base = nullptr;
    size_t               size = 0;

    std::string_view At(size_t i) const {
        const Entry& e = entries[i];
        return std::string_view(reinterpret_cast<const char*>(base + e.begin),
                                e.end - e.begin);
    }
};

uint16_t RdU16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}
uint32_t RdU32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

bool Validate(const std::vector<Entry>& e, size_t tableEnd, size_t fileSize) {
    if (e.empty()) {
        return false;
    }
    for (size_t i = 0; i < e.size(); ++i) {
        if (i > 0 && e[i].id <= e[i - 1].id) return false;   // strictly sorted
        if (e[i].begin < tableEnd || e[i].end > fileSize) return false;
        if (e[i].begin > e[i].end) return false;
    }
    return e.back().end == fileSize;
}

bool TryLayout(const uint8_t* d, size_t n, bool edge, File& out) {
    const size_t hdr = edge ? 16 : 12;
    if (n < hdr || RdU32(d) != 5) {
        return false;
    }
    const uint32_t count = edge ? RdU32(d + 8) : RdU16(d + 8);
    const uint32_t alias = edge ? RdU32(d + 12) : RdU16(d + 10);
    if (count == 0 || count > 1000000u || alias > 1000000u) {
        return false;
    }
    const size_t entSz   = edge ? 8u : 6u;
    const size_t aliasSz = edge ? 8u : 4u;
    const size_t tableEnd = hdr + (static_cast<size_t>(count) + 1) * entSz +
                            static_cast<size_t>(alias) * aliasSz;
    if (tableEnd > n) {
        return false;
    }

    std::vector<uint32_t> ids(count), offs(count + 1);
    for (uint32_t i = 0; i <= count; ++i) {
        const uint8_t* p = d + hdr + static_cast<size_t>(i) * entSz;
        const uint32_t id  = edge ? RdU32(p) : RdU16(p);
        const uint32_t off = edge ? RdU32(p + 4) : RdU32(p + 2);
        if (i < count) ids[i] = id;
        offs[i] = off;
    }
    std::vector<Entry> entries;
    entries.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        entries.push_back({ids[i], offs[i], offs[i + 1]});
    }
    if (!Validate(entries, tableEnd, n)) {
        return false;
    }
    out.ok      = true;
    out.entries = std::move(entries);
    out.base    = d;
    out.size    = n;
    return true;
}

bool Parse(const uint8_t* d, size_t n, File& out) {
    out = File{};
    if (!d || n < 12) return false;
    if (TryLayout(d, n, /*edge=*/false, out)) return true;
    out = File{};
    if (TryLayout(d, n, /*edge=*/true, out)) return true;
    out = File{};
    return false;
}

// UTF-8 -> UTF-16. Returns false on malformed input rather than substituting,
// because a malformed resource means we misparsed and must not trust it.
bool Utf8ToWide(std::string_view s, std::wstring& out) {
    out.clear();
    if (s.empty()) return true;
    const int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(),
                                      static_cast<int>(s.size()), nullptr, 0);
    if (n <= 0) return false;
    out.resize(static_cast<size_t>(n));
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(),
                        static_cast<int>(s.size()), out.data(), n);
    return true;
}

}  // namespace pak

// ---------------------------------------------------------------------------
// Discovered grammar
// ---------------------------------------------------------------------------

namespace {

constexpr wchar_t kZwsp = 0x200B;

// One branch of the localized page-count message.
struct CountForm {
    std::wstring pre;    // literal between the title and the number
    std::wstring post;   // literal after the number
    std::wstring fixed;  // whole literal tail for "=N" branches (no # present)
    int          fixedValue = 0;
};

struct Grammar {
    std::vector<std::wstring> suffixes;    // full literal tails, longest first
    std::vector<std::wstring> markerTails; // e.g. " - [InPrivate]"
    std::vector<std::wstring> slot2Seps;   // profile separators, longest first
    std::vector<CountForm>    countForms;
    std::wstring              browserName;
};

std::wstring TrimCopy(std::wstring_view s) {
    size_t b = 0, e = s.size();
    auto sp = [](wchar_t c) {
        return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r' ||
               c == 0x00A0 || c == kZwsp;
    };
    while (b < e && sp(s[b])) ++b;
    while (e > b && sp(s[e - 1])) --e;
    return std::wstring(s.substr(b, e - b));
}

bool EndsWith(std::wstring_view s, std::wstring_view t) {
    return s.size() >= t.size() && s.compare(s.size() - t.size(), t.size(), t) == 0;
}

// Decimal value of a Unicode Nd digit, or -1. Counts render in the locale's
// numbering system (Persian uses U+06F0..U+06F9), so an ASCII-only scan fails
// silently on those locales.
int DigitValue(wchar_t c) {
    static constexpr wchar_t kBlocks[] = {
        0x0030, 0x0660, 0x06F0, 0x0966, 0x09E6, 0x0A66, 0x0AE6, 0x0B66,
        0x0BE6, 0x0C66, 0x0CE6, 0x0D66, 0x0E50, 0x0ED0, 0x0F20, 0x1040,
        0x17E0, 0x1810, 0xFF10,
    };
    for (wchar_t b : kBlocks) {
        if (c >= b && c <= b + 9) return c - b;
    }
    return -1;
}

bool IsFormatEffector(wchar_t c) {
    return c == kZwsp || c == 0x200E || c == 0x200F || c == 0x00AD || c == 0xFEFF;
}

std::wstring StripEffectors(std::wstring_view s) {
    std::wstring o;
    o.reserve(s.size());
    for (wchar_t c : s) {
        if (!IsFormatEffector(c)) o.push_back(c);
    }
    return o;
}

std::vector<uint8_t> ReadWholeFile(const std::wstring& path) {
    std::vector<uint8_t> buf;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return buf;
    LARGE_INTEGER sz{};
    if (GetFileSizeEx(h, &sz) && sz.QuadPart > 0 && sz.QuadPart < (64 << 20)) {
        buf.resize(static_cast<size_t>(sz.QuadPart));
        DWORD got = 0;
        if (!ReadFile(h, buf.data(), static_cast<DWORD>(buf.size()), &got,
                      nullptr) ||
            got != buf.size()) {
            buf.clear();
        }
    }
    CloseHandle(h);
    return buf;
}

// ---- ICU plural message tokenizing -----------------------------------------
//
// The count clause is stored as a multi-line ICU message, e.g.
//     {1, plural,
//         =1 {{0} and 1 more page}
//         other {{0} and # more pages}}
// Branch counts vary by language: en-US has 2, Czech and Russian have 5.

bool ReadBalancedBraces(const std::wstring& s, size_t open, size_t* endOut,
                        std::wstring* body) {
    int depth = 0;
    for (size_t i = open; i < s.size(); ++i) {
        if (s[i] == L'{') {
            ++depth;
        } else if (s[i] == L'}') {
            if (--depth == 0) {
                *body   = s.substr(open + 1, i - open - 1);
                *endOut = i + 1;
                return true;
            }
        }
    }
    return false;
}

bool ParsePluralForms(const std::wstring& msg, std::vector<CountForm>* out) {
    const size_t p = msg.find(L"plural,");
    if (p == std::wstring::npos) return false;

    size_t i = p + 7;
    bool any = false;
    while (i < msg.size()) {
        while (i < msg.size() && (msg[i] == L' ' || msg[i] == L'\n' ||
                                  msg[i] == L'\r' || msg[i] == L'\t')) {
            ++i;
        }
        if (i >= msg.size() || msg[i] == L'}') break;

        const size_t selStart = i;
        while (i < msg.size() && msg[i] != L'{' && msg[i] != L' ' &&
               msg[i] != L'\n' && msg[i] != L'\r' && msg[i] != L'\t') {
            ++i;
        }
        const std::wstring sel = msg.substr(selStart, i - selStart);
        while (i < msg.size() && msg[i] != L'{') ++i;
        if (i >= msg.size()) break;

        std::wstring body;
        size_t       end = 0;
        if (!ReadBalancedBraces(msg, i, &end, &body)) break;
        i = end;

        // Discriminator: the title's count message always starts its branch
        // body with the {0} placeholder. Unrelated plural resources (a password
        // lock timeout, for example) embed {0} mid-sentence, and this single
        // test is what separates them without relying on resource ids.
        const std::wstring trimmed = TrimCopy(body);
        if (trimmed.rfind(L"{0}", 0) != 0) return false;

        const std::wstring tail = trimmed.substr(3);
        CountForm form;
        if (const size_t hash = tail.find(L'#'); hash != std::wstring::npos) {
            form.pre  = tail.substr(0, hash);
            form.post = tail.substr(hash + 1);
        } else {
            form.fixed = tail;
            form.fixedValue = 1;
            if (sel.size() > 1 && sel[0] == L'=') {
                form.fixedValue = _wtoi(sel.c_str() + 1);
            }
            if (form.fixedValue <= 0) form.fixedValue = 1;
        }
        out->push_back(std::move(form));
        any = true;
    }
    return any;
}

// ---- grammar discovery ------------------------------------------------------

bool DiscoverGrammar(const pak::File& f, const std::wstring& hint, Grammar* g) {
    std::wstring w;
    for (size_t i = 0; i < f.entries.size(); ++i) {
        const std::string_view raw = f.At(i);
        if (raw.empty() || raw.size() > 1200) continue;
        if (!pak::Utf8ToWide(raw, w)) continue;

        // -- browser suffix: "$1" + separator punctuation + product name
        if (w.rfind(L"$1", 0) == 0 && w.size() <= 50) {
            const std::wstring tail = w.substr(2);
            if (!tail.empty() && tail.find(L'$') == std::wstring::npos) {
                // Leading run of non-alphanumerics must contain at least one
                // punctuation character. This is what rejects prose tails like
                // " manages Microsoft Edge" while accepting " - ", " – ",
                // ": " and Lithuanian's opening quote.
                size_t k = 0;
                bool   punct = false;
                while (k < tail.size() && !iswalnum(tail[k])) {
                    if (tail[k] != L' ' && !IsFormatEffector(tail[k])) punct = true;
                    ++k;
                }
                const std::wstring flat = StripEffectors(tail);
                if (punct && k < tail.size() &&
                    flat.find(hint) != std::wstring::npos) {
                    g->suffixes.push_back(tail);
                }
            }
        }

        // -- privacy marker: "$1<sep>[word]"
        if (w.rfind(L"$1", 0) == 0 && !w.empty() && w.back() == L']') {
            const size_t lb = w.find(L'[', 2);
            if (lb != std::wstring::npos && lb > 2 && lb - 2 <= 6 &&
                lb + 1 < w.size() - 1) {
                g->markerTails.push_back(w.substr(2));
                std::wstring sep = w.substr(2, lb - 2);
                if (!sep.empty() &&
                    std::find(g->slot2Seps.begin(), g->slot2Seps.end(), sep) ==
                        g->slot2Seps.end()) {
                    g->slot2Seps.push_back(std::move(sep));
                }
            }
        }

        // -- page-count clause
        if (g->countForms.empty() && w.find(L"plural,") != std::wstring::npos &&
            w.find(L"{0}") != std::wstring::npos &&
            w.find(L'#') != std::wstring::npos) {
            std::vector<CountForm> forms;
            if (ParsePluralForms(w, &forms)) {
                g->countForms = std::move(forms);
            }
        }
    }

    // Longest first everywhere: a longer literal is the more specific match,
    // and trying it first removes any ordering ambiguity.
    auto byLenDesc = [](const std::wstring& a, const std::wstring& b) {
        return a.size() > b.size();
    };
    std::sort(g->suffixes.begin(), g->suffixes.end(), byLenDesc);
    std::sort(g->markerTails.begin(), g->markerTails.end(), byLenDesc);
    std::sort(g->slot2Seps.begin(), g->slot2Seps.end(), byLenDesc);
    return !g->suffixes.empty();
}

// ---- decomposition ----------------------------------------------------------

struct Fields {
    std::wstring title;
    std::wstring more;
    std::wstring profile;
    std::wstring priv;
    std::wstring browser;
    std::wstring name;      // user-set window name; only ever non-empty when the
                            // symbol layer identified the window as named
    int          extra    = 0;
    bool         hasCount = false;
};

// Right-anchored and validating. Any step that cannot match exactly makes the
// whole parse fail, and a failed parse means the title is left untouched.
// Failure is a valid and preferred outcome - never a reason to guess.
bool Decompose(const std::wstring& in, const Grammar& g, Fields* out) {
    // 1. browser suffix (the entire literal tail, byte for byte)
    std::wstring r1;
    bool         matched = false;
    for (const std::wstring& s : g.suffixes) {
        if (EndsWith(in, s)) {
            r1 = in.substr(0, in.size() - s.size());
            out->browser = TrimCopy(StripEffectors(s));
            // Drop a leading separator run from the display form.
            size_t k = 0;
            while (k < out->browser.size() && !iswalnum(out->browser[k])) ++k;
            if (k < out->browser.size()) out->browser = out->browser.substr(k);
            matched = true;
            break;
        }
    }
    if (!matched) return false;

    // 2. never produce an empty title
    if (TrimCopy(r1).empty()) return false;

    // 3. privacy marker, else profile in slot 2 (Edge only - Chrome discovers
    //    no markers and no separators, so both loops are simply empty there)
    std::wstring r2 = r1;
    for (const std::wstring& m : g.markerTails) {
        if (EndsWith(r1, m)) {
            out->profile = TrimCopy(m);
            size_t lb = m.find(L'[');
            size_t rb = m.rfind(L']');
            if (lb != std::wstring::npos && rb != std::wstring::npos && rb > lb) {
                out->priv = m.substr(lb + 1, rb - lb - 1);
            }
            r2 = r1.substr(0, r1.size() - m.size());
            break;
        }
    }
    // 4. page-count clause, and the generic profile - in the order that resolves
    //    the ambiguity between them.
    //
    // These cannot be stripped in a fixed order. The generic profile rule is
    // "whatever follows the last separator", and a page title containing that
    // same separator is indistinguishable from a real profile. Edge only puts a
    // profile in the title when more than one profile exists, so on a
    // single-profile install
    //
    //     "Foo - Bar and 16 more pages - Microsoft Edge"
    //
    // was read as profile="Bar and 16 more pages", title="Foo", hasCount=false -
    // losing the count and half the title on a string the mod believed it had
    // parsed.
    //
    // Resolved in favour of the reading that finds a count: try the count clause
    // against the whole remainder first, and strip a profile only on the branch
    // where that failed. A plain swap would NOT be enough - after removing the
    // count from "Foo - Bar and 16 more pages" the tail "Bar" would still be
    // taken as a profile.
    //
    // Nothing is written to `out` until an attempt succeeds, so a failed attempt
    // leaves no partial state behind.
    const auto tryStripCount = [&g](const std::wstring& in, Fields* f,
                                    std::wstring* rest) -> bool {
        for (const CountForm& cf : g.countForms) {
            if (!cf.fixed.empty()) {
                if (!EndsWith(in, cf.fixed)) continue;
                f->extra    = cf.fixedValue;
                f->hasCount = true;
                f->more     = TrimCopy(cf.fixed);
                *rest       = in.substr(0, in.size() - cf.fixed.size());
                return true;
            }
            if (!EndsWith(in, cf.post)) continue;
            const size_t e = in.size() - cf.post.size();
            size_t d = e;
            int    value = 0, scale = 1;
            // Bounded: without a cap, a title ending in a long digit run
            // overflows `value` and `scale`, which is undefined behaviour. No
            // real tab count needs more than nine digits.
            constexpr int kMaxDigits = 9;
            int digits = 0;
            while (d > 0 && digits < kMaxDigits) {
                const int dv = DigitValue(in[d - 1]);
                if (dv < 0) break;
                value += dv * scale;
                scale *= 10;
                --d;
                ++digits;
            }
            if (d == e) continue;  // no digits present
            const std::wstring head = in.substr(0, d);
            if (!EndsWith(head, cf.pre)) continue;
            f->extra    = value;
            f->hasCount = true;
            f->more     = TrimCopy(in.substr(head.size() - cf.pre.size()));
            *rest       = head.substr(0, head.size() - cf.pre.size());
            return true;
        }
        return false;
    };

    std::wstring r3 = r2;
    if (!tryStripCount(r2, out, &r3)) {
        // No count at this level. A trailing segment may therefore be a profile;
        // strip one and try the count again behind it.
        if (out->profile.empty()) {
            for (const std::wstring& sep : g.slot2Seps) {
                const size_t at = r2.rfind(sep);
                if (at == std::wstring::npos || at == 0) continue;
                const std::wstring cand = r2.substr(at + sep.size());
                if (cand.empty() || cand.size() > 64) continue;
                out->profile = cand;
                r3 = r2.substr(0, at);
                break;
            }
        }
        if (TrimCopy(r3).empty()) return false;
        std::wstring r4 = r3;
        if (tryStripCount(r3, out, &r4)) r3 = r4;
    }

    out->title = TrimCopy(r3);
    return !out->title.empty();
}

// ---- template rendering -----------------------------------------------------

std::wstring ResolveToken(std::wstring_view spec, const Fields& f, bool* empty) {
    // name[:mod[:mod...]]
    std::vector<std::wstring_view> parts;
    size_t start = 0;
    for (size_t i = 0; i <= spec.size(); ++i) {
        if (i == spec.size() || spec[i] == L':') {
            parts.push_back(spec.substr(start, i - start));
            start = i + 1;
        }
    }
    if (parts.empty()) {
        *empty = true;
        return {};
    }
    const std::wstring_view name = parts[0];

    std::wstring v;
    bool numeric = false;
    int  num = 0;
    if (name == L"title") {
        v = f.title;
    } else if (name == L"more") {
        v = f.more;
    } else if (name == L"profile") {
        v = f.profile;
    } else if (name == L"private") {
        v = f.priv;
    } else if (name == L"browser") {
        v = f.browser;
    } else if (name == L"name") {
        v = f.name;
    } else if (name == L"extra") {
        if (f.hasCount && f.extra > 0) { numeric = true; num = f.extra; }
    } else if (name == L"count") {
        if (f.hasCount) { numeric = true; num = f.extra + 1; }
    } else {
        *empty = true;
        return {};  // unknown token resolves empty rather than printing itself
    }

    // Numeric modifiers first, then styling.
    if (numeric) {
        int pad = 0;
        for (size_t i = 1; i < parts.size(); ++i) {
            if (parts[i] == L"min2" && num < 2) { *empty = true; return {}; }
            if (parts[i] == L"pad2") pad = 2;
            if (parts[i] == L"pad3") pad = 3;
        }
        v = std::to_wstring(num);
        while (static_cast<int>(v.size()) < pad) v.insert(v.begin(), L'0');
    }

    if (v.empty()) {
        *empty = true;
        return {};
    }

    for (size_t i = 1; i < parts.size(); ++i) {
        const std::wstring_view m = parts[i];
        if (m == L"upper") {
            for (auto& c : v) c = towupper(c);
        } else if (m == L"lower") {
            for (auto& c : v) c = towlower(c);
        } else if (m == L"trim") {
            v = TrimCopy(v);
        } else if (m.rfind(L"max", 0) == 0 && m.size() > 3) {
            v = style::Clamp(v, static_cast<size_t>(
                                    _wtoi(std::wstring(m.substr(3)).c_str())));
        } else if (const style::Kind k = style::FromName(m);
                   k != style::Kind::kNone) {
            v = style::Apply(v, k);
        }
    }
    *empty = v.empty();
    return v;
}

// Renders tpl[i..], stopping at ')' when inGroup. Reports whether any token
// inside resolved to something non-empty, which is what drives ?( ) groups.
size_t Render(const std::wstring& tpl, size_t i, bool inGroup, const Fields& f,
              std::wstring* out, bool* anyNonEmpty) {
    *anyNonEmpty = false;
    while (i < tpl.size()) {
        const wchar_t c = tpl[i];
        if (inGroup && c == L')') {
            return i + 1;
        }
        if (c == L'\\' && i + 1 < tpl.size()) {
            const wchar_t n = tpl[i + 1];
            if (n == L'u' && i + 5 < tpl.size()) {
                wchar_t cp = 0;
                bool    okHex = true;
                for (int k = 0; k < 4; ++k) {
                    const wchar_t h = tpl[i + 2 + k];
                    int val;
                    if (h >= L'0' && h <= L'9')      val = h - L'0';
                    else if (h >= L'a' && h <= L'f') val = h - L'a' + 10;
                    else if (h >= L'A' && h <= L'F') val = h - L'A' + 10;
                    else { okHex = false; break; }
                    cp = static_cast<wchar_t>(cp * 16 + val);
                }
                if (okHex) {
                    out->push_back(cp);
                    i += 6;
                    continue;
                }
            }
            out->push_back(n);
            i += 2;
            continue;
        }
        if (c == L'?' && i + 1 < tpl.size() && tpl[i + 1] == L'(') {
            std::wstring inner;
            bool         innerAny = false;
            i = Render(tpl, i + 2, /*inGroup=*/true, f, &inner, &innerAny);
            if (innerAny) {
                *out += inner;
                *anyNonEmpty = true;
            }
            continue;
        }
        if (c == L'{') {
            const size_t close = tpl.find(L'}', i + 1);
            if (close == std::wstring::npos) {
                out->push_back(c);
                ++i;
                continue;
            }
            bool empty = false;
            *out += ResolveToken(
                std::wstring_view(tpl).substr(i + 1, close - i - 1), f, &empty);
            if (!empty) *anyNonEmpty = true;
            i = close + 1;
            continue;
        }
        out->push_back(c);
        ++i;
    }
    return i;
}

// ---------------------------------------------------------------------------
// Runtime state
// ---------------------------------------------------------------------------

struct Settings {
    std::wstring normal;
    std::wstring named;
    std::wstring chromeOverride;
    std::wstring suffixOverride;
    bool         enabled = true;
    bool         useSymbols = false;
    bool         useFavicon = false;
    bool         verbose = false;
    size_t       maxChars = 512;
};

struct WindowState {
    std::wstring source;   // last title the browser composed, pre-transform
    std::wstring applied;  // last title we wrote; the echo detector
    DWORD        tid = 0;  // for HWND-recycling detection
    // Remembered from the first title update in which Chromium handed us one.
    // It has to persist: once a window is named, its composed title stops
    // changing, so there may never be another update carrying the correlation.
    void*        controller = nullptr;
    // Favicon-as-window-icon bookkeeping. `ours` is destroyed when replaced or
    // on uninstall; `originalSmall`/`originalBig` are whatever the window had
    // before we touched it, so the window can be handed back unchanged.
    HICON        oursSmall     = nullptr;
    HICON        oursBig       = nullptr;
    HICON        originalSmall = nullptr;
    HICON        originalBig   = nullptr;
    bool         iconSaved     = false;
    // How many times this window has been through a full title transform.
    // The favicon path refuses to run until this is >= 2 - see ApplyFavicon.
    unsigned     titleWrites   = 0;
    bool         subclassed    = false;
    // Independently established as a fully-constructed frame, by having been
    // found through EnumWindows as a VISIBLE top-level browser window with a
    // real title. That is evidence the `titleWrites >= 2` gate exists to obtain,
    // and it is the only evidence available for a window that never writes its
    // title - which on Chrome is most of them, since a Chrome title does not
    // change when tabs do.
    bool         constructed   = false;
    // Identity of the gfx::Image the last applied icon was built from. Chromium
    // hands out a refcounted storage pointer, so an unchanged page icon yields
    // the same value - which makes "has the favicon changed?" a single compare
    // instead of a bitmap conversion. That is what makes polling for icon
    // changes affordable; see ApplyFavicon.
    uint64_t     iconToken     = 0;
};

Settings g_settings;
Grammar  g_grammar;
SRWLOCK  g_lock = SRWLOCK_INIT;
std::unordered_map<HWND, WindowState> g_states;

volatile LONG g_ready       = 0;  // discovery finished
volatile LONG g_passthrough = 0;  // set in BeforeUninit
HANDLE        g_worker      = nullptr;
bool          g_isChrome    = false;

// A single thread-local guard. Costs nothing, and it is the only thing between
// a future helper that calls SetWindowTextW and unbounded recursion inside a
// UI-thread message dispatch.
thread_local bool t_inHook = false;

using SetWindowTextW_t = BOOL(WINAPI*)(HWND, LPCWSTR);
SetWindowTextW_t SetWindowTextW_Original;

// Browser frame predicate. Recomputed per call rather than cached: style bits
// change over a window's life, and the check is a handful of cheap user32
// calls. WS_VISIBLE is deliberately NOT tested - Chromium's ScopedRedrawLock
// strips it for the duration of a WM_SETTEXT, so a visibility test would reject
// exactly the calls we care about.
bool IsBrowserFrame(HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    if (GetWindow(hWnd, GW_OWNER)) return false;

    WCHAR cls[40];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (wcsncmp(cls, L"Chrome_WidgetWin_", 17) != 0) return false;

    const LONG_PTR st = GetWindowLongPtrW(hWnd, GWL_STYLE);
    constexpr LONG_PTR need = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    if ((st & need) != need) return false;

    if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
    return true;
}

// Forward declarations: the optional symbol layer is defined further down (it
// needs the hook plumbing above it), but composition is where it is consumed.
namespace syms {
// Defined here rather than beside its implementation because composition, which
// consumes it, is written above the symbol layer.
enum class Named { kNo, kYes, kUnknown };

void  CrossCheck(void* controller, int parsed);
int   TabCountForNamed(void* controller);
bool  Proved();
int   RawTabCount(void* controller);
Named IsNamedWindow(void* controller);
void  CheckPredicateAgainstParsed(void* controller);
void  RefreshNameVerdict(void* controller, const std::wstring& delegateTitle,
                         bool settled);
HWND  HwndForController(void* controller);
void  ForgetWindow(HWND hWnd);
}  // namespace syms

// Defined below the symbol layer, called from the title hook above it.
void ApplyFavicon(HWND hWnd, void* controller);

// Called once, on the browser UI thread, the first time a Chromium controller is
// matched to a window. Defined with the subclass plumbing further down, because
// applying a title has to be posted to the window rather than done from inside
// Chromium's own title composition.
void OnControllerCorrelated(HWND hWnd, void* controller,
                            const std::wstring& delegateTitle);
void EnsureSubclassed(HWND hWnd);

// Ask a window to recompose its own title on its own UI thread. Safe to call
// from anywhere, including from inside a tab-strip mutation.
void PostTitleRefresh(HWND hWnd);

std::wstring ComposeFor(const std::wstring& source, void* controller) {
    if (!g_settings.enabled || !InterlockedCompareExchange(&g_ready, 0, 0)) {
        return source;
    }
    Fields f;
    const bool parsed = Decompose(source, g_grammar, &f);

    if (parsed) {
        // A window whose title parses is the only place the symbol chain can be
        // checked against a known-good answer, so feed every one of them in.
        if (controller && f.hasCount) {
            syms::CrossCheck(controller, f.extra + 1);
        }
        // It is also the only place the NAME predicate can be checked, and for
        // the same reason: a composed title is proof the window is not named.
        if (controller) {
            syms::CheckPredicateAgainstParsed(controller);
        }
        // Fill in a tab count the TITLE could not supply.
        //
        // Chrome's window title contains no count at all, so {count} was always
        // empty there - which produced the odd result the author noticed: on
        // Chrome a NAMED window showed a count while every ordinary window
        // showed none, because only the named path consulted the browser. The
        // count is a property of the window, not of the string the browser
        // happened to compose, so it is filled in wherever it is knowable.
        //
        // Ordering matters: the cross-check above must run FIRST and only on a
        // count the title really carried, or the layer would end up validating
        // itself against its own answer.
        //
        // This also covers single-tab windows on Edge, whose titles carry no
        // count clause either. That is why the count-bearing presets use `min2`
        // - the count is now known to be 1 rather than unknown, and rendering
        // "01" on every one-tab window would be noise.
        if (controller && !f.hasCount) {
            const int total = syms::TabCountForNamed(controller);
            if (total > 0) {
                f.extra    = total - 1;
                f.hasCount = true;
            } else if (g_settings.verbose) {
                // Only the failure speaks, and it reports the value already
                // computed rather than asking again.
                //
                // What used to be here logged on EVERY composition with a
                // controller, and called TabCountForNamed a second time to get
                // the number - so on Edge, whose titles already carry a count
                // and never reach the branch above, every single title write
                // paid for a full symbol-chain resolution that existed only to
                // produce a log line. A successful count is visible in the
                // composed title on the next line anyway.
                Wh_Log(L"symbol tab count unavailable for this window (%d)",
                       total);
            }
        }
    } else {
        // Not a composed browser title. It is a named window, a PWA, a dialog or
        // picture-in-picture - and from the string alone those are identical.
        // Only the symbol layer can tell them apart: a window Chromium is
        // composing a title for has a metadata controller, and once the object
        // chain has proved itself it also yields the tab count the title lacks.
        // THE GATE. Having a controller and a resolvable tab count says only
        // that this is some Browser window - DevTools, an extension window and
        // a PWA all satisfy that, and all three were observed being retitled
        // before this check existed. Only an affirmative "yes, this window
        // carries a user title" is allowed through; "no" and "unknown" both
        // leave the title exactly as it is.
        const syms::Named isNamed = syms::IsNamedWindow(controller);
        if (isNamed != syms::Named::kYes) {
            if (g_settings.verbose) {
                Wh_Log(L"left as-is (%s): %s",
                       isNamed == syms::Named::kNo ? L"not a named window"
                                                   : L"cannot tell yet",
                       source.c_str());
            }
            return source;
        }

        const int total = controller ? syms::TabCountForNamed(controller) : -1;
        if (total < 0) {
            if (g_settings.verbose) {
                // Distinguishes "no controller correlated to this window yet"
                // from "correlated, but the chain is not proved" - the two look
                // identical from outside and need different fixes.
                Wh_Log(L"named window, but no tab count available yet "
                       L"(correlated=%d, proved=%d): %s",
                       controller ? 1 : 0, syms::Proved() ? 1 : 0,
                       source.c_str());
            }
            return source;  // hard invariant: unrecognized titles are untouched
        }
        if (g_settings.verbose) {
            Wh_Log(L"named window '%s' has %d tabs", source.c_str(), total);
        }
        f.name     = source;
        f.title    = source;
        f.extra    = total - 1;
        f.hasCount = true;
    }

    const std::wstring& tpl =
        !parsed                       ? g_settings.named
        : (g_isChrome && !g_settings.chromeOverride.empty())
                                      ? g_settings.chromeOverride
                                      : g_settings.normal;
    if (tpl.empty()) {
        return source;
    }

    std::wstring out;
    bool         any = false;
    Render(tpl, 0, /*inGroup=*/false, f, &out, &any);
    out = TrimCopy(out);
    if (out.empty()) return source;  // never blank a title
    out = style::Clamp(std::move(out), g_settings.maxChars);

    if (g_settings.verbose) {
        Wh_Log(L"title='%s' count=%d profile='%s' -> '%s'", f.title.c_str(),
               f.hasCount ? f.extra + 1 : 0, f.profile.c_str(), out.c_str());
    }
    return out;
}

// Every write that bypasses the hook must record `applied` itself, or the next
// title the browser composes is mistaken for our own echo.
//
// The timeout is deliberately short. A sweep can cover ninety windows, and this
// runs on the worker thread that teardown has to join before the DLL unloads -
// so a per-window budget of one second would let a single unresponsive window
// stall an uninstall past any reasonable join.
void WriteTitleFromOtherThread(HWND hWnd, const std::wstring& text) {
    DWORD_PTR unused = 0;
    SendMessageTimeoutW(hWnd, WM_SETTEXT, 0,
                        reinterpret_cast<LPARAM>(text.c_str()),
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &unused);
}

bool StopRequested() {
    return InterlockedCompareExchange(&g_passthrough, 0, 0) != 0;
}

// ---------------------------------------------------------------------------
// Optional symbol layer
//
// Everything the parser cannot recover lives here: whether a window carries a
// user-set name at all, and the tab count of a window whose title is only that
// name. Both are absent from the window text by construction, so no amount of
// string work reaches them.
//
// The whole layer is opt-in, every hook is `optional`, and the mod is fully
// functional with all of it unresolved. Symbol names moved once already
// (Browser:: -> WindowMetadataController:: between browser 150 and 151), so
// each entry carries a candidate list, newest first.
//
// HOW THE TAB COUNT IS REACHED
//
//     controller  --search--> BrowserWindowInterface*
//                 --GetBrowserForMigrationOnly--> Browser*
//                 --GetTabStripModel-----------> TabStripModel*
//                 --count----------------------> int
//
// No object offsets are read anywhere. In particular
// GetBrowserForMigrationOnly is NOT an offset-0 assumption: disassembling it in
// both installed builds shows it is the compiler-generated
// interface-to-complete-object adjustment thunk, and the adjustment is nonzero
// in both (Edge `lea rax,[rcx-0x90]`, Chrome `lea rax,[rcx-0x50]`). Calling it
// is exactly how the adjustment is obtained rather than guessed.
//
// The chain is nonetheless made to earn trust, because "we called only exported
// functions" does not by itself prove the answer is the right window's:
//   * Structural, and the only proof available on Chrome: a derived
//     TabStripModel* is used only if the browser itself has called
//     TabStripModel::count() on that pointer (see g_seenStrips).
//   * Numeric, where the title supplies one: on Edge the derived count is
//     compared against the count parsed out of ordinary titles, and a single
//     disagreement disables the layer for the rest of the process's life.
// ---------------------------------------------------------------------------

namespace syms {

constexpr int kProofsRequired = 3;

// Member functions returning a large object use a hidden second parameter
// pointing at the caller's return buffer, so the observed signature is
// (this, sret, args...). We only ever want `this`, and the buffer is the
// caller's problem.
using GetTitle_t   = void*(*)(void* pThis, void* sret, bool includeAppName);
using From_t       = void*(*)(void* browserWindowInterface);
using ToBrowser_t  = void*(*)(void* browserWindowInterface);
using GetTabStrip_t = void*(*)(void* browser);
using Count_t      = int(*)(void* tabStripModel);

// The two functions that make the structural proof work on Chrome.
//
// g_seenStrips originally learned real TabStripModel pointers only from
// count(), which is an Edge-shaped assumption: measured, count() fired 245 times
// in an Edge session and ZERO times in a Chrome one, because Chrome's core tab
// paths read the size inline. On Chrome the proof could therefore never be
// satisfied and no derived tab count was ever trusted.
//
// AddTab is the function BOTH insertion routes actually reach - startup tabs and
// a plain new tab alike - so it seeds a window at creation. CloseWebContentsAt
// covers removal. Both are public members, so both carry mangled names and
// resolve on the fast path; both receive the model as `this` in RCX.
//
// AddTab's full arity is declared deliberately. Win64 is caller-cleanup, so
// declaring too few parameters would not corrupt the caller's stack - but it
// would fail to forward the two STACK arguments when calling the original.
// RCX this, RDX unique_ptr, R8D index, R9D transition, then int and
// std::optional<TabGroupId> on the stack; the optional exceeds eight bytes, so
// Win64 passes it by address - one pointer slot.
using AddTab_t  = void(*)(void* tabStripModel, void* tabModel, int index,
                          int transition, int addTypes, void* groupOptional);
using CloseAt_t = void(*)(void* tabStripModel, int index, unsigned int closeTypes);

// base::FunctionRef<bool(BrowserWindowInterface*)>, as the window iterator takes
// it. base::FunctionRef holds exactly one member - an absl::FunctionRef - whose
// layout is two pointers: an erased callable and a trampoline taking that
// callable as its first argument. The browser's own trampoline symbol confirms
// the shape: absl::functional_internal::InvokeObject<...>(VoidPtr,
// BrowserWindowInterface*) returning bool.
//
// This is an INTERNAL ABI, not a published one. It is declared here because
// there is no alternative, and the static assertions below plus "refuse if any
// symbol is missing" are what keep a mismatch from becoming a crash.
struct FunctionRefAbi {
    void* target;
    bool (*invoke)(void* target, void* browserWindowInterface);
};
static_assert(sizeof(FunctionRefAbi) == 2 * sizeof(void*),
              "base::FunctionRef is two pointers wide");
static_assert(alignof(FunctionRefAbi) == alignof(void*),
              "base::FunctionRef aligns as a pointer");

// Passed indirectly: the Win64 convention passes a 16-byte aggregate by
// address, and both installed iterator bodies were observed doing so. Taking a
// pointer here states that explicitly rather than relying on the compiler to
// reproduce it.
using ForEachBwi_t = void(*)(FunctionRefAbi* onBrowserWindow);

// BrowserView::GetWindowTitle is the WidgetDelegate override that
// Widget::UpdateWindowTitle calls BEFORE comparing the result with the current
// title. That timing is the whole point of hooking it: it fires on every update
// attempt, including the ones that change nothing - which is exactly the case
// for a window whose title is a fixed user-given name. SetWindowTextW, by
// contrast, is only reached when the title actually changed, so a window named
// before the mod loaded would otherwise never be seen at all.
// ---- favicon-as-window-icon -------------------------------------------------
//
// Chromium hands out the active tab's icon as a gfx::Image and will convert an
// SkBitmap to an HICON itself, so the mod never touches pixels - which matters,
// because SkBitmap's pixel accessors are not exported anyway.
//
//     controller --GetCurrentPageIcon--> gfx::Image
//                --gfx::Image::AsBitmap--> SkBitmap
//                --IconUtil::CreateHICONFromSkBitmapSizedTo--> HICON
//
// Two objects are returned BY VALUE, which on Win64 means the caller supplies
// the storage and is responsible for destroying it. Both destructors were
// disassembled and both null-check before dereferencing (~Image reads [this];
// ~SkBitmap reads [this] and [this+0x18]), so destroying a zeroed buffer is
// harmless. That is what makes a half-failed sequence safe to unwind.
//
// Buffers are deliberately far larger than the objects: an sret callee writes
// only sizeof(T) bytes, so over-allocating cannot hurt, while under-allocating
// would corrupt the stack.
using GetPageIcon_t  = void*(*)(void* controller, void* sretImage);
using AsBitmap_t     = void*(*)(void* image, void* sretBitmap);
using CreateHicon_t  = void*(*)(void* sretScopedHicon, const void* bitmap,
                                int width, int height);
using DtorImage_t    = void(*)(void* image);
using DtorBitmap_t   = void(*)(void* bitmap);

// Higher-resolution path. The favicon Chromium hands out is 16x16 at scale 1,
// so asking IconUtil for a 32x32 icon upscales it and looks soft. An ImageSkia
// carries one representation per scale, and on a HiDPI display a 2x (32x32)
// representation usually exists - so ask for the scale that matches the icon
// size wanted, instead of stretching the 1x one.
//
// GetRepresentation and GetBitmap both return REFERENCES, i.e. plain pointers
// in RAX, so only ImageSkia itself needs by-value handling.
using AsImageSkia_t     = void*(*)(void* image, void* sretImageSkia);
using DtorImageSkia_t   = void(*)(void* imageSkia);
using GetRepresentation_t = const void*(*)(void* imageSkia, float scale);
using RepGetBitmap_t    = const void*(*)(const void* rep);
using RepGetWidth_t     = int(*)(const void* rep);

AsImageSkia_t       g_asImageSkia      = nullptr;
DtorImageSkia_t     g_dtorImageSkia    = nullptr;
GetRepresentation_t g_getRepresentation = nullptr;
RepGetBitmap_t      g_repGetBitmap     = nullptr;
RepGetWidth_t       g_repGetWidth      = nullptr;

GetPageIcon_t g_getPageIcon  = nullptr;
AsBitmap_t    g_asBitmap     = nullptr;
CreateHicon_t g_createHicon  = nullptr;
DtorImage_t   g_dtorImage    = nullptr;
DtorBitmap_t  g_dtorBitmap   = nullptr;

GetTitle_t    g_getTitle_orig   = nullptr;
From_t        g_from            = nullptr;  // address only - we CALL it
ToBrowser_t   g_toBrowser       = nullptr;  // address only
GetTabStrip_t g_getTabStrip     = nullptr;  // address only
Count_t       g_count_orig      = nullptr;  // hooked, and called through
AddTab_t      g_addTab_orig     = nullptr;  // hooked, and called through
CloseAt_t     g_closeAt_orig    = nullptr;  // hooked, and called through

// The named-window predicate's chain. Declared here rather than beside its
// implementation because the strip validation above it needs two of them.
using GetUserTitle_t = void*(*)(void* liveTabContext, void* sretString);
using FindContext_t  = void*(*)(const void* webContents);
using GetActiveWc_t  = void*(*)(void* tabStripModel);
using DtorString_t   = void(*)(void* str);
// The active tab's page title. Returns a const REFERENCE - a plain pointer in
// RAX - so unlike the window-title getter there is no allocation and nothing to
// destroy. That is what makes it usable both for correlating windows the mod may
// never see a title write for, and for proving a strip belongs to a window.
using WcGetTitle_t   = const void*(*)(void* webContents);

GetUserTitle_t g_getUserTitle = nullptr;
FindContext_t  g_findContext  = nullptr;
GetActiveWc_t  g_getActiveWc  = nullptr;
DtorString_t   g_dtorString   = nullptr;
WcGetTitle_t   g_wcGetTitle   = nullptr;

// Defined further down in this namespace, beside the title hook.
bool ReadU16String(const void* p, std::wstring* out);
ForEachBwi_t  g_forEachBwi      = nullptr;  // address only

volatile LONG g_enabled  = 0;  // user turned the layer on
volatile LONG g_poisoned = 0;  // a cross-check failed; never use the chain again
volatile LONG g_proofs   = 0;  // numeric agreements (Edge only - see below)
volatile LONG g_numericPossible = 0;  // grammar yields a tab count to check against


// Every TabStripModel the BROWSER ITSELF has called count() on.
//
// This is the structural proof, and it is the only one available on Chrome:
// Chrome's window title carries no tab count, so there is no independently
// known number to compare against and the numeric cross-check can never run
// there. Requiring that our resolved pointer is one Chromium has already used
// as a TabStripModel validates the object chain without needing any number.
SRWLOCK g_stripLock = SRWLOCK_INIT;
std::unordered_set<void*> g_seenStrips;

// ---- diagnostic event census (dev build only) ------------------------------
// Answers by OBSERVATION, not by reading upstream source: for each user action,
// which of the three observation points actually fires -
//   (a) the delegate getter, (b) TabStripModel::count() returning a new value,
//   (c) the native SetWindowTextW write.
// The gap between (a)/(b) and (c) is the whole named-window problem:
// HWNDMessageHandler::SetTitle compares against its OWN cached title, so once
// the mod has rewritten the native text the browser has no reason to write
// again and a "<count> <name>" title would freeze.
// Last tab count seen per strip, so a change can be detected and the owning
// window asked to recompose. Bounded; a browser has tens of these.
SRWLOCK g_countLock = SRWLOCK_INIT;
std::unordered_map<void*, int> g_lastCount;

// Defined with the correlation code further down in this namespace; needed here
// so a tab-count change can find the window that owns the strip.
HWND HwndForStrip(void* strip);
HWND HwndForController(void* controller);


// Set by the title hook, consumed by the SetWindowTextW hook further down the
// same synchronous call stack on the same thread. This is what maps an HWND to
// a Chromium object without reading a single struct offset.
thread_local void* t_controller = nullptr;

// ...and the title the getter produced for it, which is what makes the pairing
// trustworthy.
//
// The controller alone was not enough. Chromium suppresses the native write when
// the composed title has not changed, so a getter call often has no matching
// SetWindowTextW - and every browser frame shares one UI thread, so the value
// survived to be consumed by the NEXT window's write, which then stored another
// window's controller permanently. That window would show a foreign tab count
// and favicon: exactly the mispairing the correlation code works so hard to
// avoid everywhere else.
//
// Requiring the incoming string to equal what the getter returned closes it. It
// is not identity proof - two windows can carry the same title - so the consumer
// also refuses when the controller is already mapped to a different window.
thread_local std::wstring t_delegateTitle;
thread_local bool         t_delegateValid = false;

bool Usable() {
    return InterlockedCompareExchange(&g_enabled, 0, 0) != 0 &&
           InterlockedCompareExchange(&g_poisoned, 0, 0) == 0;
}

// Is [p, p+bytes) readable user-space memory?
//
// THE WHOLE RANGE, not just the first byte. Checking only the base is a real
// bug and it bit this mod in three places: a pointer can sit one byte inside a
// committed region while the member being read, or the string being copied out
// of it, runs off the end. VirtualQuery reports the region containing `p`, so
// the range is accepted only if it fits inside that region.
//
// This still does not make a wrong pointer right - it only turns the likeliest
// garbage into a refusal instead of a fault.
bool ReadableRange(const void* p, size_t bytes) {
    if (!p || bytes == 0) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    if (addr < 0x10000) return false;
    if (addr + bytes < addr) return false;  // overflow
    const uintptr_t end = addr + bytes;

    constexpr DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
                               PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                               PAGE_EXECUTE_WRITECOPY;

    // Walk every region the span touches. A first version of this stopped at the
    // first region and demanded the whole span fit inside it, which is not the
    // same question - a perfectly valid buffer that happens to start near a
    // region boundary was rejected. That over-conservatism broke reading the
    // title getter's return buffer, which silently disabled correlation and,
    // with it, the favicons. Being too strict here fails as visibly as being too
    // loose, just less dangerously.
    while (addr < end) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(reinterpret_cast<void*>(addr), &mbi, sizeof mbi)) {
            return false;
        }
        if (mbi.State != MEM_COMMIT) return false;
        if (!(mbi.Protect & readable) || (mbi.Protect & PAGE_GUARD)) return false;

        const uintptr_t regionEnd =
            reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (regionEnd <= addr) return false;  // no forward progress; refuse
        addr = regionEnd;
    }
    return true;
}

// Convenience for "this pointer is at least a readable object header". Callers
// that go on to read at a known offset must use ReadableRange with that offset
// instead - see StripForController.
bool PlausiblePointer(const void* p) {
    return ReadableRange(p, sizeof(void*));
}

struct ControllerSearch {
    void* wanted = nullptr;  // the controller we already correlated to an HWND
    void* found  = nullptr;  // its BrowserWindowInterface, once identified
};

// Called by the browser, once per live window, from inside the iterator.
// Returning true continues the walk, false stops it - verified in
// browser_window_interface_iterator.h.
bool SearchTrampoline(void* target, void* browserWindowInterface) {
    auto* search = static_cast<ControllerSearch*>(target);
    if (!search || !browserWindowInterface || !g_from) {
        return false;
    }
    if (g_from(browserWindowInterface) == search->wanted) {
        search->found = browserWindowInterface;
        return false;  // identity matched; nothing further to look at
    }
    return true;
}

// Find the BrowserWindowInterface whose metadata controller is `controller`.
//
// This replaces hooking WindowMetadataController::From, which resolves but
// never fires - the call site exists on the title path but is inlined away, so
// the out-of-line body never executes. Calling it ourselves works precisely
// because that body is still present and resolvable.
//
// The iterator's header warns against "first-match" consumers, because it walks
// in activation order and picking the first window matching a loose predicate
// gives results that depend on focus history. That warning does not apply here:
// the predicate is pointer identity against a controller we already hold, so at
// most one window can match and the answer is order-independent.
//
// MUST be called on the browser UI thread. Every caller reaches this from the
// SetWindowTextW hook; the discovery worker never does, because it composes
// with a null controller.
void* InterfaceForController(void* controller) {
    if (!controller || !g_forEachBwi || !g_from) {
        return nullptr;
    }
    ControllerSearch search{controller, nullptr};
    FunctionRefAbi   ref{&search, &SearchTrampoline};
    g_forEachBwi(&ref);
    return search.found;
}

// Returns the live tab count, or -1 when it cannot be obtained safely.
// Callable even before the chain is proved - proving it is exactly what the
// first callers are for.
bool StripIsKnown(void* strip) {
    AcquireSRWLockShared(&g_stripLock);
    const bool known = g_seenStrips.count(strip) != 0;
    ReleaseSRWLockShared(&g_stripLock);
    return known;
}

// The verified object chain, ending at a TabStripModel the browser itself has
// used. Factored out of RawTabCount so the name predicate can reuse it rather
// than open a second, unproved route to the same window.

// REMOVED: a second structural proof that asked the candidate strip for its
// active tab's title and compared it with the correlated window's parsed title.
// It CRASHED Edge - c0000409 subcode 0xa, FAST_FAIL_GUARD_ICALL_CHECK_FAILURE.
//
// The reasoning was sound and the implementation was not. g_seenStrips exists
// precisely so that a candidate is never TOUCHED before it is known to be a
// TabStripModel; that check validated the candidate BY DEREFERENCING IT, which
// is the one thing it must not do. `PlausiblePointer` proves memory is committed
// and readable, never that it holds the type expected - so on the wrong
// candidate, GetActiveWebContents read a garbage vtable pointer and Chromium's
// Control Flow Guard killed the process on the indirect call.
//
// This is the v0.5 lesson in a new costume, and worth restating in the form it
// took here: a validator that must dereference the thing it is validating is not
// a validator. Any replacement has to establish the type from OUTSIDE the
// object - which is exactly what the AddTab and CloseWebContentsAt hooks now do:
// the browser hands them a TabStripModel as `this`, so no guess is involved and
// nothing unproved is ever dereferenced.
//
// (SendDetachWebContentsNotifications would also have worked, but it has
// internal linkage - no mangled name - and reaching it means resolving with
// noUndecoratedSymbols off, measured at >14 min and 2.8 GB. See docs/.)

// The tab strip for a BrowserWindowInterface we already hold.
//
// Split out from StripForController because the correlation sweep is handed the
// interface by Chromium's own iterator and then used to throw it away and look
// it back up - re-walking the entire window list, from inside that iterator's
// callback, twice per window. On a seventy-window session that was ~150 full
// walks every three seconds on the browser UI thread, which starved the posted
// icon refreshes badly enough to look like the favicon feature had broken.
// It was also a nested iteration of the browser's own collection, which is an
// assumption about that iterator nobody here can verify.
void* StripForInterface(void* bwi) {
    if (InterlockedCompareExchange(&g_poisoned, 0, 0)) return nullptr;
    if (!g_toBrowser || !g_getTabStrip || !g_count_orig) return nullptr;
    if (!PlausiblePointer(bwi)) return nullptr;

    // BOTH candidates, with the structural gate deciding between them.
    //
    // Review round 1 cut this to the adjusted candidate alone, on the argument
    // that GetBrowserForMigrationOnly is provably the interface-to-complete-
    // object thunk - verified by disassembly in both builds (Chrome
    // `lea rax,[rcx-0x50]`, Edge `lea rax,[rcx-0x90]`) - so its result IS the
    // Browser and a second guess can add nothing.
    //
    // The disassembly is right. The conclusion was wrong, and it cost the
    // favicon feature entirely. Measured on a live session after that change:
    // correlation succeeded for 0 of 76 windows in one process and 0 of 6 in
    // another - a total symbol-layer failure - with the adjusted candidate
    // yielding a plausible pointer that sat 17 MB from the nearest address the
    // browser had ever actually used as a TabStripModel. The adjustment is real;
    // the premise that it applies HERE is not. The pointer this iterator hands
    // out is already the `this` GetTabStripModel wants, so applying the thunk
    // over-adjusts and reads a member off the wrong object.
    //
    // That is precisely the case the gate exists to catch, and it caught it:
    // nothing wrong was ever trusted, the layer just went silent. Which is the
    // argument for probing rather than reasoning - `StripIsKnown` accepts only a
    // pointer the browser ITSELF has used as a TabStripModel, so trying both and
    // keeping what proves out is a proof, not a guess.
    //
    // What was legitimate in the review's concern is kept. GetTabStripModel is a
    // plain member load (Chrome `mov rax,[rcx+0x138]`, Edge `mov rax,[rcx+0x250]`),
    // so a wrong `this` reads hundreds of bytes past an address we only know is
    // readable somewhere. Each candidate's whole span is therefore validated
    // BEFORE the accessor touches it - the check the old two-candidate version
    // genuinely lacked.
    constexpr size_t kMaxMemberOffset = 0x400;
    void* const      candidates[]     = {g_toBrowser(bwi), bwi};

    void* strip       = nullptr;
    void* produced[2] = {nullptr, nullptr};
    for (size_t i = 0; i < ARRAYSIZE(candidates); ++i) {
        if (!ReadableRange(candidates[i], kMaxMemberOffset)) continue;
        void* const s = g_getTabStrip(candidates[i]);
        produced[i]   = s;
        if (PlausiblePointer(s) && StripIsKnown(s)) {
            strip = s;
            break;
        }
    }

    if (!strip) {
        // Rate limited to one line a minute. This is the single most frequent
        // outcome on a session with many windows - every uncorrelated one
        // produces it on every sweep - and unthrottled it wrote seven thousand
        // identical lines in three minutes, which is not diagnostics, it is a
        // denial of service against the log the user is being told to read.
        //
        // A second was the first attempt and is still far too generous: the line
        // is identical every time, so sixty of them a minute carry exactly as
        // much information as one and bury everything else. One a minute is
        // enough to notice a layer that never comes up.
        if (g_settings.verbose) {
            static volatile LONG lastLog = -1;
            const LONG now = static_cast<LONG>(GetTickCount64() / 60000);
            if (InterlockedExchange(&lastLog, now) != now) {
                // The intermediate values, not just the verdict. "It did not
                // match" cannot distinguish a wrong thunk from a wrong accessor
                // from a genuinely unproved strip, and this mod has already lost
                // days to trusting an outcome without looking at what produced
                // it. Both candidates' results are shown, so a future failure
                // says WHICH route was tried and what each returned.
                AcquireSRWLockShared(&g_stripLock);
                const size_t seen = g_seenStrips.size();
                ReleaseSRWLockShared(&g_stripLock);
                Wh_Log(L"  tab strip not recognized yet (%zu known): bwi=%p "
                       L"adjusted=%p -> %p, raw -> %p",
                       seen, bwi, candidates[0], produced[0], produced[1]);
            }
        }
        return nullptr;
    }
    return strip;
}

void* StripForController(void* controller) {
    void* const bwi = InterfaceForController(controller);
    if (!bwi) {
        if (g_settings.verbose) {
            Wh_Log(L"  no browser window matched this controller");
        }
        return nullptr;
    }
    return StripForInterface(bwi);
}

int RawTabCount(void* controller) {
    void* const strip = StripForController(controller);
    if (!strip) return -1;
    const int n = g_count_orig(strip);
    return (n > 0 && n < 10000) ? n : -1;
}

// ---------------------------------------------------------------------------
// The named-window predicate
//
// THE PROBLEM. "The title did not parse" is not a predicate for "the user named
// this window", and shipping as if it were put a fabricated tab count on an
// extension window (`⁰¹ QuicKey`), on every undocked DevTools window, and on an
// installed PWA (`⁰¹ YouTube - (1889) YouTube`). All three are Browser objects
// with a metadata controller and a one-tab strip, so every condition the old
// inference tested was satisfied.
//
// AN APPROACH THAT WAS BUILT, MEASURED AND REMOVED. Chromium's title getter
// takes an `include_app_name` flag that a user title short-circuits ahead of, so
// comparing the two flag values looked like a free semantic test. Measured: on
// the steady-state title path Chromium only ever calls it with the flag SET.
// The other value arrives only during window construction, so for a window that
// is named later the comparison would be between strings captured at different
// times - which is not merely useless but WRONG, since a construction-time
// "Untitled" against a post-naming "MyWindow" reads as "not named". It was
// removed rather than patched with a generation counter, because the evidence it
// needs does not exist on the path that matters.
//
// WHAT IS USED INSTEAD. BrowserLiveTabContext::GetUserTitle() returns the user
// title itself, out of line and callable. Emptiness is the whole answer, and
// libc++'s alternate string layout makes that one byte:
//
//     byte[23] == 0   <=>   the string is empty   (see ReadU16String)
//
// No character decoding, no pointer chasing in the common case.
//
// WHY THIS IS SAFE TO CALL WHEN THE u16string GETTER WAS NOT. This one returns a
// std::string, and `basic_string<char>::~basic_string` IS present out of line
// (Edge 052DD610 / Chrome 00EA15D0) - unlike ~basic_string<char16_t>, which has
// zero matches across 1.5M symbols. So an allocation can actually be handed
// back, which is what made a self-call unacceptable before.
//
// THE OFFSET THIS DOES NOT ASSUME. FindContextForWebContents returns a
// `sessions::LiveTabContext*` while GetUserTitle is a BrowserLiveTabContext
// member, so calling one on the other would assume the base sits at offset 0.
// Rather than assume it, the result is VALIDATED: a returned buffer that is not
// a plausible libc++ string disables the feature, and a buffer that is not
// plausible is also never destroyed, because freeing a bogus pointer is worse
// than leaking 24 bytes once.
// ---------------------------------------------------------------------------

// (The accessors' declarations were hoisted above StripForController, which
// needs GetActiveWebContents and GetTitle to validate a candidate strip.)

// A window whose title PARSES as a composed browser title is definitively NOT
// named - composition is exactly what a user title replaces. That gives both a
// proof and a tripwire: every such window must report "not named", and one that
// reports "named" proves the chain or the string layout is wrong on this build.
constexpr int kNameProofsRequired = 3;
volatile LONG g_nameProofs   = 0;
volatile LONG g_namePoisoned = 0;

void PoisonName(const wchar_t* why) {
    if (InterlockedExchange(&g_namePoisoned, 1) == 0) {
        Wh_Log(L"named-window detection DISABLED (%s). Named windows will be "
               L"left exactly as the browser wrote them.", why);
    }
}

// Ask the browser directly. Returns kUnknown for every gap, which the caller
// treats as "leave the title alone". When `nameOut` is given and the window is
// named, it receives the name - which is also what correlation matches on.
Named QueryUserTitleForStrip(void* strip, std::wstring* nameOut = nullptr) {
    if (InterlockedCompareExchange(&g_namePoisoned, 0, 0)) return Named::kUnknown;
    if (!g_getUserTitle || !g_findContext || !g_getActiveWc || !g_dtorString) {
        return Named::kUnknown;
    }
    if (!strip) return Named::kUnknown;

    void* const wc = g_getActiveWc(strip);
    if (!PlausiblePointer(wc)) return Named::kUnknown;
    void* const ctx = g_findContext(wc);
    if (!PlausiblePointer(ctx)) return Named::kUnknown;

    // Deliberately larger than the 24-byte object: an sret callee writes only
    // sizeof(T), so over-allocating is free while under-allocating corrupts the
    // stack.
    alignas(16) unsigned char buf[64] = {};
    g_getUserTitle(ctx, buf);

    const unsigned char flag = buf[23];
    const bool isLong = (flag & 0x80) != 0;
    bool plausible = false;
    size_t len = 0;

    if (!isLong) {
        len = flag & 0x7F;
        plausible = (len <= 22);  // the short form cannot hold more
    } else {
        uint64_t ptr = 0, size = 0, cap = 0;
        memcpy(&ptr, buf + 0, sizeof ptr);
        memcpy(&size, buf + 8, sizeof size);
        memcpy(&cap, buf + 16, sizeof cap);
        cap &= ~(uint64_t{1} << 63);  // top bit is the is_long flag
        // The whole run of bytes, not the first one: this is a std::string, so
        // `size` is bytes, and a string_view over it is built further down.
        plausible = ptr && size && size < 4096 && size <= cap &&
                    ReadableRange(reinterpret_cast<void*>(ptr),
                                  static_cast<size_t>(size));
        len = static_cast<size_t>(size);
    }

    if (g_settings.verbose && !plausible) {
        // Kept for release: if libc++'s string layout ever changes under us,
        // this line is what says so, and it is the difference between "the
        // feature stopped working" and knowing why.
        //
        // On the FAILURE path only. It used to log on every call, which is
        // several times a second per window on a large session - the single
        // largest source of noise in this mod's log - and every one of those
        // lines said the layout was fine. The layout being fine is not news;
        // the numbers are only worth reading when the assumption has broken,
        // which is exactly when this now fires.
        Wh_Log(L"  user title: flagByte=%02X long=%d len=%zu plausible=%d", flag,
               isLong ? 1 : 0, len, plausible ? 1 : 0);
    }

    if (!plausible) {
        // Do NOT destroy it. If this is not really a libc++ string then the
        // pointer at +0 is not ours to free, and leaking 24 bytes once is
        // strictly better than corrupting the browser's heap.
        PoisonName(L"the user-title accessor returned something that is not a "
                   L"libc++ string on this build");
        return Named::kUnknown;
    }

    if (nameOut && len) {
        // std::string, so UTF-8. Copy BEFORE destroying - the long form's bytes
        // live in an allocation the destructor is about to release.
        const char* data = isLong
                               ? *reinterpret_cast<const char* const*>(buf)
                               : reinterpret_cast<const char*>(buf);
        std::wstring w;
        if (pak::Utf8ToWide(std::string_view(data, len), w)) {
            *nameOut = std::move(w);
        }
    }

    // A short string owns no allocation, so only the long form needs the
    // destructor - but calling it either way is what the type expects.
    g_dtorString(buf);
    return (flag != 0) ? Named::kYes : Named::kNo;
}

// Cached, because this runs on every title update and walks four hops. The
// verdict is invalidated whenever the delegate title changes, since renaming and
// un-naming both change it.
struct NameVerdict {
    std::wstring title;      // delegate title the verdict was computed for
    Named        verdict = Named::kUnknown;
};
SRWLOCK g_verdictLock = SRWLOCK_INIT;
std::unordered_map<void*, NameVerdict> g_verdicts;

Named IsNamedWindow(void* controller) {
    if (!controller || InterlockedCompareExchange(&g_namePoisoned, 0, 0)) {
        return Named::kUnknown;
    }
    AcquireSRWLockShared(&g_verdictLock);
    const auto it = g_verdicts.find(controller);
    const Named r = (it == g_verdicts.end()) ? Named::kUnknown : it->second.verdict;
    ReleaseSRWLockShared(&g_verdictLock);
    // Only an affirmative answer that has also been proved is allowed out.
    if (r == Named::kYes &&
        InterlockedCompareExchange(&g_nameProofs, 0, 0) < kNameProofsRequired) {
        return Named::kUnknown;
    }
    return r;
}

// Called from the title hook, on the browser UI thread, with the delegate title
// Chromium just produced. `settled` carries the same steady-state gate the
// favicon path uses: this walks out of a Browser object, which is exactly the
// shape that crashed v0.5 on a window still under construction.
void RefreshNameVerdict(void* controller, const std::wstring& delegateTitle,
                        bool settled) {
    if (!controller || !settled) return;
    if (InterlockedCompareExchange(&g_namePoisoned, 0, 0)) return;

    {
        AcquireSRWLockShared(&g_verdictLock);
        const auto it = g_verdicts.find(controller);
        const bool fresh = it != g_verdicts.end() && it->second.title == delegateTitle;
        ReleaseSRWLockShared(&g_verdictLock);
        if (fresh) return;
    }

    const Named v = QueryUserTitleForStrip(StripForController(controller));

    AcquireSRWLockExclusive(&g_verdictLock);
    if (g_verdicts.size() < 512 || g_verdicts.count(controller)) {
        g_verdicts[controller] = NameVerdict{delegateTitle, v};
    }
    ReleaseSRWLockExclusive(&g_verdictLock);
}

// The proof and the tripwire, run on every window whose title parsed.
void CheckPredicateAgainstParsed(void* controller) {
    if (!controller || InterlockedCompareExchange(&g_namePoisoned, 0, 0)) return;
    AcquireSRWLockShared(&g_verdictLock);
    const auto it = g_verdicts.find(controller);
    const Named v = (it == g_verdicts.end()) ? Named::kUnknown : it->second.verdict;
    ReleaseSRWLockShared(&g_verdictLock);

    if (v == Named::kYes) {
        PoisonName(L"a window whose title parsed as a composed browser title "
                   L"also reported a user title");
    } else if (v == Named::kNo) {
        const LONG n = InterlockedIncrement(&g_nameProofs);
        if (n == kNameProofsRequired) {
            Wh_Log(L"named-window detection verified (%d windows with composed "
                   L"titles all correctly reported no user title)",
                   kNameProofsRequired);
        }
    }
}

int Count_hook(void* tabStripModel) {
    if (tabStripModel) {
        AcquireSRWLockExclusive(&g_stripLock);
        // Bounded: a browser has tens of these, not thousands. The cap only
        // stops a pathological case from growing without limit.
        if (g_seenStrips.size() < 4096) {
            g_seenStrips.insert(tabStripModel);
        }
        ReleaseSRWLockExclusive(&g_stripLock);
    }
    const int n = g_count_orig(tabStripModel);

    // A named window's title stops changing the moment it is named, so Chromium
    // has no reason to write it again and its tab count would freeze. This is a
    // signal that does not depend on the title: the strip's own count changed,
    // so whichever window owns it needs to recompose. Posted, never done here -
    // this runs inside a tab-strip mutation, where doing real work would be
    // reckless.
    //
    // NOT gated on verbose logging. It was, while this block was diagnostic,
    // which quietly meant the refresh only happened for users who had turned
    // logging on.
    if (tabStripModel) {
        bool changed = false;
        AcquireSRWLockExclusive(&g_countLock);
        auto it = g_lastCount.find(tabStripModel);
        if (it == g_lastCount.end()) {
            if (g_lastCount.size() < 512) {
                g_lastCount.emplace(tabStripModel, n);
            }
        } else if (it->second != n) {
            it->second = n;
            changed = true;
        }
        ReleaseSRWLockExclusive(&g_countLock);
        if (changed) {
            PostTitleRefresh(HwndForStrip(tabStripModel));
        }
    }
    return n;
}

// Record a pointer the browser has just used as a TabStripModel, and ask the
// window that owns it to recompose.
//
// This is the same evidence Count_hook collects, from functions Chrome actually
// calls. Refresh is POSTED, never done here: both callers run inside a
// tab-strip mutation.
void NoteStripAndRefresh(void* strip, const wchar_t* why) {
    if (!strip) return;
    AcquireSRWLockExclusive(&g_stripLock);
    if (g_seenStrips.size() < 4096) {
        g_seenStrips.insert(strip);
    }
    ReleaseSRWLockExclusive(&g_stripLock);
    if (g_settings.verbose) {
        Wh_Log(L"tab strip changed (%s)", why);
    }
    PostTitleRefresh(HwndForStrip(strip));
}

// Tab insertion. Both routes reach this - startup tabs and a plain new tab -
// so it seeds a window AT CREATION, which is what makes a freshly opened Chrome
// window show its count without the user having to touch it first.
void AddTab_hook(void* strip, void* tabModel, int index, int transition,
                 int addTypes, void* groupOptional) {
    g_addTab_orig(strip, tabModel, index, transition, addTypes, groupOptional);
    NoteStripAndRefresh(strip, L"ADDTAB");
}

// Tab removal. BrowserView::OnTabDetached returns early for a background tab, so
// no title update fires for one - this is the signal that does not depend on the
// title changing.
void CloseAt_hook(void* strip, int index, unsigned int closeTypes) {
    g_closeAt_orig(strip, index, closeTypes);
    NoteStripAndRefresh(strip, L"CLOSETAB");
}

// Called on every window whose title DID parse, where `parsed` is the tab count
// the title itself reported. This is the only place the chain earns trust, and
// the only place it loses it.
void CrossCheck(void* controller, int parsed) {
    if (!Usable() || parsed <= 0) return;
    const int viaSymbols = RawTabCount(controller);
    if (viaSymbols < 0) return;  // could not evaluate; says nothing either way

    if (viaSymbols != parsed) {
        InterlockedExchange(&g_poisoned, 1);
        Wh_Log(L"symbol layer DISABLED: tab count %d from symbols disagrees with "
               L"%d parsed from the title. The object chain is not valid on this "
               L"build; falling back to parse-only.",
               viaSymbols, parsed);
        return;
    }
    const LONG n = InterlockedIncrement(&g_proofs);
    if (n == kProofsRequired) {
        Wh_Log(L"symbol layer verified (%d consecutive agreements); named-window "
               L"tab counts are now available", kProofsRequired);
    }
}

// Two proof regimes, because the evidence available differs by browser:
//
//   Edge   - titles carry a tab count, so the chain is checked against a number
//            that is already known. Strongest available evidence; require it.
//   Chrome - titles carry no count at all, so no such number exists. The
//            structural proof inside RawTabCount (the resolved pointer must be
//            one the browser itself has used as a TabStripModel) carries the
//            whole weight there.
//
// Requiring the numeric proof unconditionally would mean the layer could never
// activate on Chrome, which is where it is most useful - Chrome's titles lack
// the count for named and unnamed windows alike.
bool Proved() {
    if (!Usable()) return false;
    if (!InterlockedCompareExchange(&g_numericPossible, 0, 0)) {
        return true;  // structural proof only; RawTabCount still enforces it
    }
    return InterlockedCompareExchange(&g_proofs, 0, 0) >= kProofsRequired;
}

// Tab count for a window whose title did NOT parse - the named-window case.
// Refuses until the chain has been proved on windows where the answer was
// independently known.
int TabCountForNamed(void* controller) {
    return Proved() ? RawTabCount(controller) : -1;
}

bool FaviconAvailable() {
    return Usable() && g_getPageIcon && g_asBitmap && g_createHicon &&
           g_dtorImage && g_dtorBitmap;
}

// Build an HICON of the active tab's favicon, or nullptr. Ownership transfers
// to the caller.
//
// The ScopedHICON that CreateHICONFromSkBitmapSizedTo returns owns its handle
// and would destroy it on scope exit - so its destructor is deliberately NOT
// run. Reading the handle out and taking ownership is exactly equivalent to
// what .release() would do, and avoids depending on that type's layout beyond
// its first member being the handle.
HICON BuildFaviconIcon(void* controller, int size) {
    if (!FaviconAvailable() || !controller || size <= 0) {
        return nullptr;
    }

    alignas(16) unsigned char image[256]  = {};
    alignas(16) unsigned char bitmap[256] = {};
    alignas(16) unsigned char scoped[64]  = {};

    auto dword = [](const unsigned char* p, size_t off) -> uint32_t {
        uint32_t v = 0;
        memcpy(&v, p + off, sizeof v);
        return v;
    };

    g_getPageIcon(controller, image);

    // Prefer a representation whose scale matches the icon size being asked
    // for. Chromium's favicon is 16px at scale 1, so a 32px icon wants scale 2;
    // taking the 1x bitmap and letting IconUtil stretch it is what made the
    // first working build look soft.
    alignas(16) unsigned char imageSkia[256] = {};
    bool haveSkia = false;
    const void* sourceBitmap = nullptr;
    int nativeWidth = 0;

    if (g_asImageSkia && g_dtorImageSkia && g_getRepresentation &&
        g_repGetBitmap) {
        g_asImageSkia(image, imageSkia);
        haveSkia = true;
        const float scale = static_cast<float>(size) / 16.0f;
        if (const void* rep = g_getRepresentation(imageSkia, scale); rep) {
            if (const void* bmp = g_repGetBitmap(rep); bmp) {
                sourceBitmap = bmp;
                if (g_repGetWidth) {
                    nativeWidth = g_repGetWidth(rep);
                }
            }
        }
    }

    if (!sourceBitmap) {
        // Fallback: the plain 1x bitmap. Correct, just softer when upscaled.
        g_asBitmap(image, bitmap);
        sourceBitmap = bitmap;
        if (g_settings.verbose) {
            // SkBitmap's width/height sit at +0x28/+0x2C - the factory itself
            // reads them there, which is how those offsets were established.
            Wh_Log(L"  fallback bitmap w=%u h=%u", dword(bitmap, 0x28),
                   dword(bitmap, 0x2C));
        }
        nativeWidth = static_cast<int>(dword(bitmap, 0x28));
    }

    // Build at exactly the size Windows will display, and let Skia do the
    // resampling.
    //
    // The source really is 16x16 with no larger representation (measured:
    // GetRepresentation(2.0) still returns 16px), so sharpness is capped. But
    // WHO scales it still matters. An earlier version handed the shell a 16px
    // icon and let it stretch to the 24px/48px the system actually asks for,
    // which is visibly softer than what Chromium's own tab strip shows from
    // the same bitmap. IconUtil resamples through Skia, so producing the
    // target size here replaces the shell's scaler with a better one.
    // Logged only if the source is bigger than the 16px the readme documents as
    // the hard sharpness cap. Every routine build used to narrate its own scale,
    // its source pointer and the resulting handle - four lines per icon, two
    // icons per window, on a poll: hundreds of lines a minute all saying the
    // pipeline worked exactly as designed. The one outcome here that would be
    // genuine news is Chromium starting to supply something larger, so that is
    // the only one that still speaks.
    if (g_settings.verbose && nativeWidth > 16) {
        Wh_Log(L"  source representation is %dpx - larger than the documented "
               L"16px cap, so the readme's sharpness limit may be out of date",
               nativeWidth);
    }
    g_createHicon(scoped, sourceBitmap, size, size);

    // Where the handle sits inside the returned ScopedGeneric is not knowable
    // from outside, and the obvious reading is wrong: the observed layout has a
    // CONSTANT module pointer at +0 and the real handle at +8. So rather than
    // hard-code an offset, probe the candidates and let validation pick - the
    // same probe-and-verify approach used for the tab strip, for the same
    // reason: a guess that turns out wrong must fail closed instead of reaching
    // a window.
    //
    // CopyIcon is the validator rather than GetIconInfo because it and
    // DestroyIcon both live in user32, so this needs no gdi32 link (which would
    // mean another @compilerOptions entry to justify at review).
    HICON icon = nullptr;
    for (size_t off : {size_t{8}, size_t{0}, size_t{16}}) {
        HICON candidate = nullptr;
        memcpy(&candidate, scoped + off, sizeof candidate);
        if (!candidate) {
            continue;
        }
        if (HICON probe = CopyIcon(candidate); probe) {
            DestroyIcon(probe);
            icon = candidate;
            break;
        }
    }
    if (!icon && g_settings.verbose) {
        Wh_Log(L"  REJECTED: no offset held a valid icon handle");
    }

    // Destroy in reverse order of construction. The ScopedGeneric is
    // deliberately NOT destroyed - we have taken its handle, which is exactly
    // what release() would do. The SkBitmap from a representation is owned by
    // that representation and must NOT be destroyed; only the fallback bitmap,
    // which we constructed, is ours to clean up.
    if (sourceBitmap == bitmap) {
        g_dtorBitmap(bitmap);
    }
    if (haveSkia) {
        g_dtorImageSkia(imageSkia);
    }
    g_dtorImage(image);
    return icon;
}

// Identity of the window's current page icon, or 0.
//
// gfx::Image holds a refcounted storage pointer as its first member, so an
// unchanged page icon yields the same value. That turns "has the favicon
// changed?" into a single compare instead of two bitmap conversions and two
// HICON builds - which is what makes it affordable to ASK repeatedly.
//
// Asking repeatedly is the point. A favicon arrives asynchronously, often after
// the title has stopped changing, and on Chrome the title changes far less than
// on Edge - Chrome's title carries no tab count, so opening or closing a
// background tab does not touch it at all. Waiting for a title change therefore
// meant the icon only appeared once the user switched tabs.
uint64_t PageIconToken(void* controller) {
    if (!FaviconAvailable() || !controller) return 0;
    alignas(16) unsigned char image[256] = {};
    g_getPageIcon(controller, image);
    uint64_t token = 0;
    memcpy(&token, image, sizeof token);
    g_dtorImage(image);
    return token;
}

// Read a libc++ std::u16string WITHOUT taking ownership and WITHOUT destroying
// it. The buffer belongs to the CALLER of the hooked getter, and that caller
// will destroy it - doing so ourselves would be a double free.
//
// These builds use libc++'s ALTERNATE string layout, 24 bytes:
//   long  : +0 data pointer, +8 size, +16 capacity (63 bits), byte 23 bit 7 = 1
//   short : +0..21 inline char16_t (max 11), byte 23 bits 0..6 = size, bit 7 = 0
// so "is this string empty" is exactly `byte[23] == 0` - no decoding, no
// pointer chasing. wchar_t is 16-bit on Windows, so char16_t data is directly
// assignable to a std::wstring.
bool ReadU16String(const void* p, std::wstring* out) {
    // The 24-byte object itself has to be readable before byte 23 is touched.
    if (!ReadableRange(p, 24)) return false;
    const auto* b = static_cast<const unsigned char*>(p);
    const unsigned char flag = b[23];
    const wchar_t* data = nullptr;
    size_t len = 0;

    if (flag & 0x80) {
        uint64_t ptr = 0, size = 0;
        memcpy(&ptr, b + 0, sizeof ptr);
        memcpy(&size, b + 8, sizeof size);
        if (!ptr || size == 0 || size > 4096) return false;
        // The WHOLE string, not its first byte: assign() below copies
        // size * sizeof(char16_t) bytes and would happily run off the region.
        if (!ReadableRange(reinterpret_cast<const void*>(ptr),
                           static_cast<size_t>(size) * sizeof(wchar_t))) {
            return false;
        }
        data = reinterpret_cast<const wchar_t*>(ptr);
        len  = static_cast<size_t>(size);
    } else {
        len = flag & 0x7F;
        if (len > 11) return false;  // impossible for the short form; refuse
        data = reinterpret_cast<const wchar_t*>(b);
    }
    if (out) out->assign(data, len);
    return true;
}

// ---------------------------------------------------------------------------
// Correlating a controller to its HWND without waiting for a title write
//
// THE GAP THIS CLOSES. The controller was only ever learned from a
// SetWindowTextW that followed the getter on the same stack. A window that was
// ALREADY NAMED when the mod loaded never writes its title again - the delegate
// keeps returning the same user title, so Chromium suppresses every write - so
// that window was never correlated, never got a tab count, and was left alone
// forever. Measured: all three of the author's named windows arrived from the
// sweep with a null controller.
//
// THE KEY. The getter fires on every update ATTEMPT, including the suppressed
// ones. So the controller is available; only the HWND is missing. The delegate
// title we can now read out of the getter's return buffer is enough to find it:
// for a window we have not rewritten, that string IS the window text.
//
// WHY IT MATCHES THE CACHED ORIGINAL, NOT THE SCREEN. Once the mod has rewritten
// a window, the on-screen text is the mod's own output and would never equal the
// delegate's string again. Every comparison is therefore against the remembered
// pre-transform title where one exists - the same discipline the sweep already
// uses when recomposing.
//
// AMBIGUITY IS REFUSED, NOT GUESSED. Two windows can genuinely share a title.
// A match is accepted only when exactly one window matches; anything else waits
// for a better moment. Correlating the wrong window would put one window's tab
// count on another's title, which is worse than not doing it at all.
// ---------------------------------------------------------------------------

SRWLOCK g_corrLock = SRWLOCK_INIT;
std::unordered_map<void*, HWND>      g_ctrlHwnd;   // controller -> its window
std::unordered_map<void*, ULONGLONG> g_corrTried;  // controller -> last attempt
std::unordered_map<void*, HWND>      g_stripHwnd;  // TabStripModel -> its window

HWND HwndForController(void* controller) {
    AcquireSRWLockShared(&g_corrLock);
    const auto it = g_ctrlHwnd.find(controller);
    HWND h = (it == g_ctrlHwnd.end()) ? nullptr : it->second;
    ReleaseSRWLockShared(&g_corrLock);
    return h;
}

void NoteStripHwnd(void* strip, HWND hWnd) {
    if (!strip || !hWnd) return;
    AcquireSRWLockExclusive(&g_corrLock);
    if (g_stripHwnd.size() < 512) g_stripHwnd[strip] = hWnd;
    ReleaseSRWLockExclusive(&g_corrLock);
}

HWND HwndForStrip(void* strip) {
    AcquireSRWLockShared(&g_corrLock);
    const auto it = g_stripHwnd.find(strip);
    HWND h = (it == g_stripHwnd.end()) ? nullptr : it->second;
    ReleaseSRWLockShared(&g_corrLock);
    return h;
}

// Forget everything learned about a window that has just been destroyed.
//
// Without this the maps only ever grow, and their caps are not harmless: a
// session that opens and closes enough windows stops recording correlations
// entirely, with no log line to say so. Worse, a stale entry is how a recycled
// HWND inherits a dead window's controller - the per-window thread-id check
// cannot catch that, because every browser frame lives on the same UI thread.
//
// Called from WM_NCDESTROY, i.e. while the window is going away for good.
void ForgetWindow(HWND hWnd) {
    if (!hWnd) return;

    std::vector<void*> controllers, strips;
    AcquireSRWLockExclusive(&g_corrLock);
    for (auto it = g_ctrlHwnd.begin(); it != g_ctrlHwnd.end();) {
        if (it->second == hWnd) {
            controllers.push_back(it->first);
            it = g_ctrlHwnd.erase(it);
        } else {
            ++it;
        }
    }
    for (void* c : controllers) g_corrTried.erase(c);
    for (auto it = g_stripHwnd.begin(); it != g_stripHwnd.end();) {
        if (it->second == hWnd) {
            strips.push_back(it->first);
            it = g_stripHwnd.erase(it);
        } else {
            ++it;
        }
    }
    ReleaseSRWLockExclusive(&g_corrLock);

    // Each map under its own lock, and never two at once - taking g_corrLock and
    // then another would introduce a lock order this mod does not otherwise have.
    if (!controllers.empty()) {
        AcquireSRWLockExclusive(&g_verdictLock);
        for (void* c : controllers) g_verdicts.erase(c);
        ReleaseSRWLockExclusive(&g_verdictLock);
    }
    if (!strips.empty()) {
        AcquireSRWLockExclusive(&g_countLock);
        for (void* s : strips) g_lastCount.erase(s);
        ReleaseSRWLockExclusive(&g_countLock);

        // Also drop the type proof for this window's strip. g_seenStrips is the
        // evidence that an address really is a TabStripModel, and it only ever
        // grew - so once the allocator reused a dead strip's address for
        // something else, stale membership could turn a guess into "proved" and
        // hand Chromium the wrong type. Forgetting is safe in the other
        // direction: the worst case is re-earning the proof.
        AcquireSRWLockExclusive(&g_stripLock);
        for (void* s : strips) g_seenStrips.erase(s);
        ReleaseSRWLockExclusive(&g_stripLock);
    }
}

struct CorrSearch {
    const std::wstring* want    = nullptr;
    HWND                found   = nullptr;
    int                 matches = 0;
};

BOOL CALLBACK CorrEnumProc(HWND hWnd, LPARAM lp) {
    auto* s = reinterpret_cast<CorrSearch*>(lp);
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId() || !IsBrowserFrame(hWnd)) return TRUE;

    std::wstring src;
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        if (it != g_states.end() && !it->second.source.empty()) {
            src = it->second.source;
        }
        ReleaseSRWLockShared(&g_lock);
    }
    if (src.empty()) {
        WCHAR buf[1024];
        const int n = GetWindowTextW(hWnd, buf, ARRAYSIZE(buf));
        if (n > 0) src.assign(buf, static_cast<size_t>(n));
    }
    if (!src.empty() && src == *s->want) {
        ++s->matches;
        s->found = hWnd;
    }
    return TRUE;
}

// Same as TryCorrelate, but the key is the ACTIVE TAB's title rather than the
// whole window title, so each candidate's remembered original is decomposed
// first and its {title} field compared. Uniqueness is still required: two
// windows showing the same page is entirely possible, and picking either would
// put one window's tab count on the other.
struct PageSearch {
    const std::wstring* want    = nullptr;
    HWND                found   = nullptr;
    int                 matches = 0;
};

BOOL CALLBACK PageEnumProc(HWND hWnd, LPARAM lp) {
    auto* s = reinterpret_cast<PageSearch*>(lp);
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId() || !IsBrowserFrame(hWnd)) return TRUE;

    std::wstring src;
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        if (it != g_states.end() && !it->second.source.empty()) {
            src = it->second.source;
        }
        ReleaseSRWLockShared(&g_lock);
    }
    if (src.empty()) return TRUE;  // never seen; nothing trustworthy to compare

    Fields f;
    if (!Decompose(src, g_grammar, &f)) return TRUE;
    if (!f.title.empty() && f.title == *s->want) {
        ++s->matches;
        s->found = hWnd;
    }
    return TRUE;
}

void TryCorrelateByPageTitle(void* controller, const std::wstring& pageTitle) {
    if (!controller || pageTitle.empty()) return;
    if (HwndForController(controller)) return;
    if (!InterlockedCompareExchange(&g_ready, 0, 0)) return;  // no grammar yet

    PageSearch s;
    s.want = &pageTitle;
    EnumWindows(PageEnumProc, reinterpret_cast<LPARAM>(&s));
    if (s.matches != 1) return;

    AcquireSRWLockExclusive(&g_corrLock);
    if (g_ctrlHwnd.size() < 512) g_ctrlHwnd[controller] = s.found;
    ReleaseSRWLockExclusive(&g_corrLock);

    if (g_settings.verbose) {
        Wh_Log(L"matched a window by its active tab: '%s'", pageTitle.c_str());
    }

    std::wstring source;
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(s.found);
        if (it != g_states.end()) source = it->second.source;
        ReleaseSRWLockShared(&g_lock);
    }
    OnControllerCorrelated(s.found, controller, source);
}

void TryCorrelate(void* controller, const std::wstring& delegateTitle) {
    if (!controller || delegateTitle.empty()) return;
    if (HwndForController(controller)) return;

    // Throttled: this enumerates every top-level window, and the getter can fire
    // several times per event. A retry every half second is far more often than
    // a window's identity can change.
    const ULONGLONG now = GetTickCount64();
    {
        AcquireSRWLockExclusive(&g_corrLock);
        const auto it = g_corrTried.find(controller);
        const bool tooSoon = it != g_corrTried.end() && (now - it->second) < 500;
        if (!tooSoon && g_corrTried.size() < 512) g_corrTried[controller] = now;
        ReleaseSRWLockExclusive(&g_corrLock);
        if (tooSoon) return;
    }

    CorrSearch s;
    s.want = &delegateTitle;
    EnumWindows(CorrEnumProc, reinterpret_cast<LPARAM>(&s));
    if (s.matches != 1) {
        if (g_settings.verbose && s.matches > 1) {
            // Refusing is the correct outcome, but it is indistinguishable from
            // the feature not working unless it says so.
            Wh_Log(L"not matching a window: %d of them share the title '%s'",
                   s.matches, delegateTitle.c_str());
        }
        return;
    }

    AcquireSRWLockExclusive(&g_corrLock);
    if (g_ctrlHwnd.size() < 512) g_ctrlHwnd[controller] = s.found;
    ReleaseSRWLockExclusive(&g_corrLock);

    if (g_settings.verbose) {
        Wh_Log(L"matched a window by its title: '%s'", delegateTitle.c_str());
    }
    OnControllerCorrelated(s.found, controller, delegateTitle);
}

// Correlating the window that is updating is not enough: a window that was
// already named when the mod loaded is IDLE, so its own getter never fires and
// it would wait forever for an event it will not produce. Measured exactly that
// - all three named windows stayed untouched while other windows correlated
// fine.
//
// So whenever we are on the UI thread with any window updating, walk EVERY live
// browser window and correlate the named ones. Their name comes from
// GetUserTitle - the std::string one, which can be read and destroyed safely,
// unlike the u16string the title getter returns - and for a window the mod has
// not yet rewritten, that name IS the window text.
//
// Result: an idle named window is picked up the moment ANY window in the browser
// does anything, instead of never.
ULONGLONG g_lastFullCorrelate = 0;

struct NamedSweep {
    int correlated = 0;
};

// The active tab's page title for a strip we already have, read through a const
// reference. Never allocates and never destroys.
bool ActivePageTitleForStrip(void* strip, std::wstring* out) {
    if (!g_wcGetTitle || !g_getActiveWc || !strip) return false;
    void* const wc = g_getActiveWc(strip);
    if (!PlausiblePointer(wc)) return false;
    const void* const s = g_wcGetTitle(wc);
    if (!PlausiblePointer(s)) return false;
    return ReadU16String(s, out);
}

bool NamedSweepTrampoline(void* target, void* bwi) {
    auto* sweep = static_cast<NamedSweep*>(target);
    if (!sweep || !bwi || !g_from) return false;

    void* const controller = g_from(bwi);
    if (!PlausiblePointer(controller) || HwndForController(controller)) {
        return true;  // unusable, or already known
    }

    // Resolve the strip ONCE, from the interface Chromium just handed us. The
    // previous shape threw `bwi` away and looked it back up through
    // InterfaceForController, which re-walked this very iterator's collection -
    // twice per window, from inside its own callback.
    void* const strip = StripForInterface(bwi);
    if (!strip) return true;

    // A named window matches on its name, which is also its window text.
    std::wstring name;
    if (QueryUserTitleForStrip(strip, &name) == Named::kYes && !name.empty()) {
        TryCorrelate(controller, name);
        if (HwndForController(controller)) ++sweep->correlated;
        return true;
    }

    // An ordinary window has no name to match on, and its composed title cannot
    // be asked for safely - that getter returns a u16string BY VALUE with no
    // destructor to hand it back with. Its active tab's title can: that one
    // returns a reference. So match on the page title, against the {title}
    // field decomposed out of each window's remembered original.
    //
    // Without this, an ordinary window that never writes its title again - the
    // normal state of an idle Chrome window, whose title carries no tab count
    // and so does not change when tabs do - would never be correlated, and would
    // never receive a tab count or a refreshed favicon.
    std::wstring page;
    if (!ActivePageTitleForStrip(strip, &page) || page.empty()) return true;
    TryCorrelateByPageTitle(controller, page);
    if (HwndForController(controller)) ++sweep->correlated;
    return true;
}

void CorrelateAllNamedWindows() {
    if (!g_forEachBwi || !g_from || !g_getUserTitle) return;
    if (InterlockedCompareExchange(&g_namePoisoned, 0, 0)) return;

    // Throttled hard: this walks every window and calls into Chromium for each.
    const ULONGLONG now = GetTickCount64();
    if (g_lastFullCorrelate && (now - g_lastFullCorrelate) < 3000) return;
    g_lastFullCorrelate = now;

    NamedSweep sweep;
    FunctionRefAbi ref{&sweep, &NamedSweepTrampoline};
    g_forEachBwi(&ref);
    if (sweep.correlated && g_settings.verbose) {
        Wh_Log(L"matched %d named window(s) that were already open",
               sweep.correlated);
    }
}

void* GetTitle_hook(void* pThis, void* sret, bool includeAppName) {
    // Drop any pairing left by a previous attempt before doing anything, so a
    // suppressed write can never leave one behind for the next window.
    t_controller    = nullptr;
    t_delegateValid = false;

    void* const r = g_getTitle_orig(pThis, sret, includeAppName);

    std::wstring delegate;
    const bool haveTitle = ReadU16String(sret, &delegate);
    if (pThis && haveTitle) {
        t_controller    = pThis;
        t_delegateTitle = delegate;
        t_delegateValid = true;
    }

    // Correlate here rather than in the SetWindowTextW hook. This runs on every
    // update ATTEMPT, which is the only signal a window that never writes its
    // title again produces.
    if (pThis && haveTitle && !HwndForController(pThis)) {
        TryCorrelate(pThis, delegate);
    }
    // ...and pick up the idle named windows that will never get here themselves.
    CorrelateAllNamedWindows();

    // Deliberately silent. This runs on every title update ATTEMPT, several
    // times per tab switch, so logging here drowns everything else - including
    // the lines that matter when something is actually wrong.
    return r;
}

// REMOVED in 0.5.1 - this crashed Chrome (0xC0000005 in
// BrowserView::GetNativeWindow, chrome.dll+0x3FC6122, four dumps).
//
// The idea was sound and the pointer was genuinely a BrowserView: the fault was
// TIMING. BrowserView::GetWindowTitle is also called while a window is still
// being constructed - during Chrome's startup right after profile selection -
// and GetNativeWindow walks `this + 0x30` into the widget, which does not exist
// yet. `PlausiblePointer(browserView)` passed because the BrowserView itself is
// a real object; what was null lay one dereference further in, where nothing
// outside Chromium can check it.
//
// The lesson generalises: "the object is right by construction" says nothing
// about whether it is INITIALISED, and an accessor that is safe to call at
// steady state is not automatically safe to call during construction. Any
// retry of this needs a way to know the widget exists before asking for it.

// The same symbol set is resolved against whichever browser DLL is loaded, so
// the target cannot be encoded in the array's name - hence the comment form.
bool Install(HMODULE mod) {
    // msedge.dll, chrome.dll
    WindhawkUtils::SYMBOL_HOOK titleHooks[] = {
        {
            {
                LR"(public: class std::__Cr::basic_string<char16_t,struct std::__Cr::char_traits<char16_t>,class std::__Cr::allocator<char16_t> > __cdecl WindowMetadataController::GetWindowTitleForCurrentTab(bool)const )",
                LR"(public: class std::__Cr::basic_string<char16_t,struct std::__Cr::char_traits<char16_t>,class std::__Cr::allocator<char16_t> > __cdecl Browser::GetWindowTitleForCurrentTab(bool)const )",
            },
            &g_getTitle_orig,
            GetTitle_hook,
            true,
        },
        {
            {LR"(public: class gfx::Image __cdecl WindowMetadataController::GetCurrentPageIcon(void)const )"},
            &g_getPageIcon,
            nullptr,
            true,
        },
        {
            {LR"(public: class SkBitmap __cdecl gfx::Image::AsBitmap(void)const )"},
            &g_asBitmap,
            nullptr,
            true,
        },
        {
            {LR"(public: class gfx::ImageSkia __cdecl gfx::Image::AsImageSkia(void)const )"},
            &g_asImageSkia,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl gfx::ImageSkia::~ImageSkia(void))"},
            &g_dtorImageSkia,
            nullptr,
            true,
        },
        {
            {LR"(public: class gfx::ImageSkiaRep const & __cdecl gfx::ImageSkia::GetRepresentation(float)const )"},
            &g_getRepresentation,
            nullptr,
            true,
        },
        {
            {LR"(public: class SkBitmap const & __cdecl gfx::ImageSkiaRep::GetBitmap(void)const )"},
            &g_repGetBitmap,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl gfx::ImageSkiaRep::GetWidth(void)const )"},
            &g_repGetWidth,
            nullptr,
            true,
        },
        {
            {LR"(public: static class base::ScopedGeneric<struct HICON__ *,struct base::win::internal::ScopedGDIObjectTraits<struct HICON__ *> > __cdecl IconUtil::CreateHICONFromSkBitmapSizedTo(class SkBitmap const &,int,int))"},
            &g_createHicon,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl gfx::Image::~Image(void))"},
            &g_dtorImage,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl SkBitmap::~SkBitmap(void))"},
            &g_dtorBitmap,
            nullptr,
            true,
        },
        {
            {LR"(public: static class WindowMetadataController * __cdecl WindowMetadataController::From(class BrowserWindowInterface *))"},
            &g_from,
            nullptr,  // address only - hooking it is useless, it is inlined away
            true,
        },
        {
            {LR"(void __cdecl ForEachCurrentBrowserWindowInterfaceOrderedByActivation(class base::FunctionRef<bool __cdecl(class BrowserWindowInterface *)>))"},
            &g_forEachBwi,
            nullptr,  // address only
            true,
        },
        {
            // Not an offset-0 assumption: disassembling this function in both
            // installed builds shows it IS the interface-to-complete-object
            // adjustment thunk (Edge `lea rax,[rcx-0x90]`, Chrome
            // `lea rax,[rcx-0x50]`). Calling it is what avoids guessing.
            {LR"(public: virtual class Browser * __cdecl Browser::GetBrowserForMigrationOnly(void))"},
            &g_toBrowser,
            nullptr,  // address only - we call it, we do not intercept it
            true,
        },
        {
            {LR"(public: virtual class TabStripModel * __cdecl Browser::GetTabStripModel(void))"},
            &g_getTabStrip,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl TabStripModel::count(void)const )"},
            &g_count_orig,
            Count_hook,  // hooked to learn which pointers are real TabStripModels
            true,
        },
        {
            // Same purpose as count() above, but this one Chrome actually calls.
            // The two builds record it differently - Chrome's PDB carries the
            // parameter type, Edge's does not - so both forms are offered.
            // Identical undecorated string in both builds, so one candidate
            // covers Chrome (RVA 01161890) and Edge (02C1DA76).
            {LR"(public: void __cdecl TabStripModel::AddTab(class std::__Cr::unique_ptr<class tabs::TabModel,struct std::__Cr::default_delete<class tabs::TabModel> >,int,enum ui::PageTransition,int,class std::__Cr::optional<class tab_groups::TabGroupId>))"},
            &g_addTab_orig,
            AddTab_hook,
            true,
        },
        {
            // Chrome 09762DE0 / Edge 030E9954.
            {LR"(public: void __cdecl TabStripModel::CloseWebContentsAt(int,unsigned int))"},
            &g_closeAt_orig,
            CloseAt_hook,
            true,
        },
        // ---- the named-window predicate ------------------------------------
        // Two extra hops off the TabStripModel we already trust, then the user
        // title itself. All address-only: we call these, we do not intercept.
        {
            {LR"(public: class content::WebContents * __cdecl TabStripModel::GetActiveWebContents(void)const )"},
            &g_getActiveWc,
            nullptr,
            true,
        },
        {
            {LR"(public: static class sessions::LiveTabContext * __cdecl BrowserLiveTabContext::FindContextForWebContents(class content::WebContents const *))"},
            &g_findContext,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::__Cr::basic_string<char,struct std::__Cr::char_traits<char>,class std::__Cr::allocator<char> > __cdecl BrowserLiveTabContext::GetUserTitle(void)const )"},
            &g_getUserTitle,
            nullptr,
            true,
        },
        {
            // Required, not optional-within-the-group: GetUserTitle returns a
            // std::string BY VALUE and a long one allocates from the browser's
            // allocator. Without this there is nothing to hand it back with, so
            // the predicate refuses to run at all rather than leak per call.
            {LR"(public: __cdecl std::__Cr::basic_string<char,struct std::__Cr::char_traits<char>,class std::__Cr::allocator<char> >::~basic_string<char,struct std::__Cr::char_traits<char>,class std::__Cr::allocator<char> >(void))"},
            &g_dtorString,
            nullptr,
            true,
        },
        {
            // Returns a const REFERENCE, so nothing is allocated and nothing
            // needs destroying - the one string accessor here that is free to
            // call as often as we like, which is what lets a window be
            // correlated without waiting for it to write its title.
            {LR"(public: virtual class std::__Cr::basic_string<char16_t,struct std::__Cr::char_traits<char16_t>,class std::__Cr::allocator<char16_t> > const & __cdecl content::WebContentsImpl::GetTitle(void))"},
            &g_wcGetTitle,
            nullptr,
            true,
        },
    };

    // noUndecoratedSymbols matters enormously here: these modules carry ~1.5-1.9
    // million symbols, and skipping the undecorated forms roughly halves the work.
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.noUndecoratedSymbols = TRUE;

    if (!WindhawkUtils::HookSymbols(mod, titleHooks,
                                    ARRAYSIZE(titleHooks), &options)) {
        Wh_Log(L"symbol resolution failed; staying in parse-only mode");
        return false;
    }
    // Log RESOLVED OFFSETS, not just "did it resolve". A symbol that resolves
    // to the wrong address is indistinguishable from a working one until it
    // returns nonsense, which is exactly what happened with the favicon chain.
    // Expected (from the dumps): Chrome pageIcon=0943C9E0 asBitmap=01927440
    // createHicon=0AC9F450 dtorImage=05277AD0 dtorBitmap=052CA9E0.
    {
        const auto rva = [mod](void* p) -> uintptr_t {
            return p ? reinterpret_cast<uintptr_t>(p) -
                           reinterpret_cast<uintptr_t>(mod)
                     : 0;
        };
        Wh_Log(L"favicon RVAs: pageIcon=%08zX asBitmap=%08zX createHicon=%08zX "
               L"dtorImage=%08zX dtorBitmap=%08zX",
               rva((void*)g_getPageIcon), rva((void*)g_asBitmap),
               rva((void*)g_createHicon), rva((void*)g_dtorImage),
               rva((void*)g_dtorBitmap));
    }
    Wh_Log(L"symbols: title=%d from=%d forEach=%d toBrowser=%d tabStrip=%d "
           L"count=%d addTab=%d closeAt=%d",
           g_getTitle_orig ? 1 : 0, g_from ? 1 : 0, g_forEachBwi ? 1 : 0,
           g_toBrowser ? 1 : 0, g_getTabStrip ? 1 : 0, g_count_orig ? 1 : 0,
           g_addTab_orig ? 1 : 0, g_closeAt_orig ? 1 : 0);

    // The name predicate degrades on its own: if any of its four symbols is
    // missing it simply never answers, IsNamedWindow stays "unknown", and named
    // windows are left alone. That is the pre-predicate behaviour, so a gap here
    // costs a feature rather than correctness - which is why it does not join
    // the all-or-nothing check below.
    const bool nameOk = g_getActiveWc && g_findContext && g_getUserTitle &&
                        g_dtorString;
    Wh_Log(L"name predicate: activeWc=%d findCtx=%d getUserTitle=%d strDtor=%d -> %s",
           g_getActiveWc ? 1 : 0, g_findContext ? 1 : 0, g_getUserTitle ? 1 : 0,
           g_dtorString ? 1 : 0,
           nameOk ? L"available" : L"UNAVAILABLE, named windows stay untouched");
    if (!nameOk) {
        // Refuse partially. Calling GetUserTitle without the destructor would
        // leak on every long name, so all four or none.
        g_getActiveWc = nullptr;
        g_findContext = nullptr;
        g_getUserTitle = nullptr;
        g_dtorString = nullptr;
    }

    // All or nothing. The correlation alone buys nothing without the tab count,
    // and a partially resolved chain is the state most likely to produce a
    // confident wrong answer, so treat any gap as "stay in parse-only mode".
    const bool complete = g_getTitle_orig && g_from && g_forEachBwi &&
                          g_toBrowser && g_getTabStrip && g_count_orig;
    if (!complete) {
        Wh_Log(L"symbol chain incomplete; named-window support stays off");
        return false;
    }

    // The engine only applies hooks automatically once, after Wh_ModInit
    // returns. These are registered later - they have to be, because the
    // browser module is not loaded at init time - so without this they resolve,
    // report success, and then silently never fire. That failure mode looks
    // exactly like the symbols being wrong, which cost a full debugging cycle
    // to tell apart.
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Wh_ApplyHookOperations failed; symbol hooks are not live");
        return false;
    }
    return true;
}

}  // namespace syms

// Apply the active tab's favicon as the window icon.
//
// Runs on the browser UI thread from the title hook, where the HWND is an
// ARGUMENT rather than something derived from a Chromium object. That is
// deliberately unlike v0.5, whose crash came from walking out of a window still
// under construction: nothing here dereferences a Chromium pointer we were not
// just handed.
void ApplyFavicon(HWND hWnd, void* controller) {
    if (!g_settings.useFavicon || !controller ||
        InterlockedCompareExchange(&g_passthrough, 0, 0)) {
        return;
    }

    // STEADY-STATE GATE. Disassembly shows GetCurrentPageIcon walks
    // [this] -> [+0x188] -> [+0x90] on Chrome (and [this] -> [+0x2E0] on Edge)
    // and only null-checks the LAST link. On a window still being constructed
    // an earlier link can be null, and that is precisely the fault that took
    // down v0.5 - a pointer that was right, at a moment when it was not yet
    // initialised.
    //
    // A window that has already completed two full title transforms is past
    // construction. This costs one skipped icon update on a brand new window,
    // which the next title change corrects.
    //
    // `constructed` is the second way to satisfy this, and it is what makes the
    // icon work on Chrome at all. Title writes are the wrong evidence there: a
    // Chrome title carries no tab count, so it does not change when tabs do, and
    // a window correlated through the sweep rather than through a write has
    // titleWrites == 0 forever. It would then never pass this gate and never get
    // an icon - which is exactly the symptom that was reported.
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        const bool settled = it != g_states.end() &&
                             (it->second.titleWrites >= 2 || it->second.constructed);
        ReleaseSRWLockShared(&g_lock);
        if (!settled) {
            if (g_settings.verbose) {
                Wh_Log(L"favicon: window not settled yet (titleWrites<2)");
            }
            return;
        }
    }

    if (!syms::FaviconAvailable()) {
        if (g_settings.verbose) {
            Wh_Log(L"favicon: chain unavailable (symbols missing or layer off)");
        }
        return;
    }

    // Nothing to do if the page icon is the same object we last built from.
    // This is what lets the refresh be driven by a timer rather than only by a
    // title change: the common case costs one call and one compare.
    const uint64_t token = syms::PageIconToken(controller);
    if (token) {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        const bool unchanged = it != g_states.end() &&
                               it->second.iconToken == token &&
                               (it->second.oursSmall || it->second.oursBig);
        ReleaseSRWLockShared(&g_lock);
        if (unchanged) return;
    }

    // Ask the system what it will actually display rather than assuming 16/32.
    // These are physical pixels for a per-monitor-DPI-aware process, so at 150%
    // they come back 24 and 48 - handing over 16px for both is precisely what
    // forced the shell to upscale.
    int smallPx = GetSystemMetrics(SM_CXSMICON);
    int bigPx   = GetSystemMetrics(SM_CXICON);
    if (smallPx <= 0) smallPx = 16;
    if (bigPx   <= 0) bigPx   = 32;

    const HICON small = syms::BuildFaviconIcon(controller, smallPx);
    const HICON big   = syms::BuildFaviconIcon(controller, bigPx);
    // Only a build that produced nothing is worth a line. The successful case
    // fires once per icon per refreshed window on a poll, which on a large
    // session is most of the log and says only that the expected thing happened.
    if (g_settings.verbose && (!small || !big)) {
        Wh_Log(L"favicon: build produced no icon (small=%p big=%p)", (void*)small,
               (void*)big);
    }
    if (!small && !big) {
        return;  // no favicon yet; leave whatever the window already shows
    }

    // Read the browser's own icons BEFORE taking the lock. These are synchronous
    // window messages, and sending one while holding a non-recursive SRW lock
    // means any path that ever re-enters a hook needing that lock deadlocks the
    // thread permanently. It works today only because the send stays on this
    // thread; that is not a property worth depending on.
    HICON origSmall = nullptr;
    HICON origBig   = nullptr;
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        const bool needOriginals = it == g_states.end() || !it->second.iconSaved;
        ReleaseSRWLockShared(&g_lock);
        if (needOriginals) {
            origSmall = reinterpret_cast<HICON>(
                SendMessageW(hWnd, WM_GETICON, ICON_SMALL, 0));
            origBig = reinterpret_cast<HICON>(
                SendMessageW(hWnd, WM_GETICON, ICON_BIG, 0));
        }
    }

    HICON oldSmall = nullptr;
    HICON oldBig   = nullptr;
    {
        AcquireSRWLockExclusive(&g_lock);
        WindowState& st = g_states[hWnd];
        if (!st.iconSaved) {
            st.originalSmall = origSmall;
            st.originalBig   = origBig;
            st.iconSaved     = true;
        }
        // Retire each size ONLY if it was actually replaced. This used to
        // capture both old handles unconditionally and destroy both, so when
        // one size failed to build - which the handle probe in BuildFaviconIcon
        // can cause - its old handle was destroyed while still installed on the
        // window and still recorded in state, and RestoreIcon then destroyed it
        // a second time at uninstall.
        if (small) {
            oldSmall     = st.oursSmall;
            st.oursSmall = small;
        }
        if (big) {
            oldBig     = st.oursBig;
            st.oursBig = big;
        }
        // Only claim this page icon as done when BOTH sizes are in place;
        // otherwise the identity guard above would suppress every retry of the
        // size that failed.
        st.iconToken = (st.oursSmall && st.oursBig) ? token : 0;
        ReleaseSRWLockExclusive(&g_lock);
    }

    if (small) {
        SendMessageW(hWnd, WM_SETICON, ICON_SMALL,
                     reinterpret_cast<LPARAM>(small));
    }
    if (big) {
        SendMessageW(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(big));
    }

    // Only now is the previous icon no longer referenced by the window.
    if (oldSmall) DestroyIcon(oldSmall);
    if (oldBig)   DestroyIcon(oldBig);
}

// Refreshing a window's icon has to happen on that window's UI thread, because
// GetCurrentPageIcon walks Chromium objects. The sweep runs on the discovery
// worker, so it cannot do the work itself - it posts this message instead, and
// the subclass below picks it up on the right thread.
//
// Without this, an icon only ever appeared when the TITLE changed, so a window
// sitting on one tab kept the browser's own icon until the user switched tabs.
UINT g_applyIconMsg = 0;

// Recomposing a named window's title has to happen on the window's own UI
// thread and OUTSIDE Chromium's title composition, so it is posted rather than
// done inline. Same shape as the icon message, and for the same reason.
UINT g_applyTitleMsg = 0;

// Asks a window's UI thread to run the correlation sweep on everyone's behalf.
//
// Without this the whole symbol layer is DORMANT after a reload: correlation
// only ever ran from the title getter, so a browser nobody is touching produces
// no getter calls, correlates nothing, and therefore shows no tab counts and no
// favicons until the user happens to interact with it. Observed exactly that -
// fifteen minutes of a loaded, enabled, symbol-resolved mod doing nothing at all
// because both browsers were idle.
//
// The sweep itself is UI-thread-only, so the worker cannot call it; it posts
// this instead. Any managed frame will do - the sweep covers every window.
UINT g_correlateMsg = 0;

void PostTitleRefresh(HWND hWnd) {
    if (hWnd && g_applyTitleMsg &&
        !InterlockedCompareExchange(&g_passthrough, 0, 0)) {
        PostMessageW(hWnd, g_applyTitleMsg, 0, 0);
    }
}

// Write a title the browser is never going to write itself.
//
// For an ordinary window the mod only ever TRANSFORMS a write Chromium makes.
// A named window makes none: its delegate title stops changing the moment it is
// named, so Chromium suppresses every update. The count therefore has to be put
// on by the mod, from the remembered pre-transform title.
void ApplyTitleNow(HWND hWnd) {
    if (InterlockedCompareExchange(&g_passthrough, 0, 0) || t_inHook) return;

    std::wstring source, out;
    void* controller = nullptr;
    {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        if (it != g_states.end()) {
            source     = it->second.source;
            controller = it->second.controller;
        }
        ReleaseSRWLockShared(&g_lock);
    }
    if (source.empty() || !controller) return;

    t_inHook = true;
    out = ComposeFor(source, controller);
    t_inHook = false;
    if (out.empty() || out == source) return;

    WCHAR cur[1024];
    const int n = GetWindowTextW(hWnd, cur, ARRAYSIZE(cur));
    if (n > 0 && out == std::wstring(cur, static_cast<size_t>(n))) {
        return;  // already showing it
    }

    {
        AcquireSRWLockExclusive(&g_lock);
        g_states[hWnd].applied = out;
        ReleaseSRWLockExclusive(&g_lock);
    }
    // Bypass our own hook: the string is already composed, and going back
    // through it would re-enter composition on our own output.
    SetWindowTextW_Original(hWnd, out.c_str());
}

LRESULT CALLBACK FrameSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam,
                                   DWORD_PTR dwRefData) {
    // A window going away for good. Let it finish first, then forget it -
    // without this, per-window state accumulated for the life of the browser
    // process, the fixed caps eventually stopped recording anything (silently,
    // with no log line), and a recycled HWND could inherit a dead window's
    // controller. The thread-id check elsewhere cannot catch that, because every
    // browser frame lives on the same UI thread.
    if (uMsg == WM_NCDESTROY) {
        const LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        HICON ours[2] = {nullptr, nullptr};
        {
            AcquireSRWLockExclusive(&g_lock);
            if (const auto it = g_states.find(hWnd); it != g_states.end()) {
                // The window is being destroyed, so do NOT restore its icons -
                // just release the ones we created for it.
                ours[0] = it->second.oursSmall;
                ours[1] = it->second.oursBig;
                g_states.erase(it);
            }
            ReleaseSRWLockExclusive(&g_lock);
        }
        for (HICON h : ours) {
            if (h) DestroyIcon(h);
        }
        syms::ForgetWindow(hWnd);
        return r;
    }
    if (g_correlateMsg && uMsg == g_correlateMsg) {
        syms::CorrelateAllNamedWindows();
        return 0;
    }
    if (g_applyTitleMsg && uMsg == g_applyTitleMsg) {
        ApplyTitleNow(hWnd);
        return 0;
    }
    if (g_applyIconMsg && uMsg == g_applyIconMsg) {
        void* controller = nullptr;
        AcquireSRWLockShared(&g_lock);
        if (const auto it = g_states.find(hWnd); it != g_states.end()) {
            controller = it->second.controller;
        }
        ReleaseSRWLockShared(&g_lock);
        if (controller) {
            ApplyFavicon(hWnd, controller);
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Record what a newly correlated window is, and give it its title.
//
// The window state gets the controller it never had, and `source` is seeded from
// the delegate title so composition has an original to work from - a window that
// never wrote its title has nothing cached otherwise. The strip is recorded too,
// so a later tab-count change can find its way back to this window.
void OnControllerCorrelated(HWND hWnd, void* controller,
                            const std::wstring& delegateTitle) {
    if (!hWnd || !controller) return;
    {
        AcquireSRWLockExclusive(&g_lock);
        WindowState& st = g_states[hWnd];
        st.controller  = controller;
        st.constructed = true;
        if (st.source.empty()) st.source = delegateTitle;
        if (!st.tid) st.tid = GetWindowThreadProcessId(hWnd, nullptr);
        ReleaseSRWLockExclusive(&g_lock);
    }
    EnsureSubclassed(hWnd);

    if (void* strip = syms::StripForController(controller); strip) {
        syms::NoteStripHwnd(strip, hWnd);
    }

    // The verdict is normally computed in the SetWindowTextW hook, which for
    // these windows never fires - that is the whole reason they were invisible.
    // So compute it here.
    //
    // The steady-state gate is satisfied differently, and deliberately: this
    // window was just found by EnumWindows as a VISIBLE top-level browser frame
    // carrying a non-empty title that matched the delegate's own string. A
    // window under construction is none of those things, so this is at least as
    // strong as the "two completed title transforms" evidence the write path
    // uses - and unlike that one, it is obtainable here.
    syms::RefreshNameVerdict(controller, delegateTitle, /*settled=*/true);

    // Feed the proof counter from here too.
    //
    // The predicate refuses to answer "named" until three windows whose titles
    // PARSE have correctly reported "not named". That evidence normally arrives
    // through the write path - but a write only happens when a title changes,
    // and on an idle browser it may not come at all. Chrome makes this acute:
    // its title carries no tab count, so tab activity does not touch it, and the
    // predicate could sit unproved indefinitely while a named Chrome window
    // waited for a verdict it would never get.
    //
    // A correlated window whose original title parses is exactly the same
    // evidence, available without waiting for anything.
    if (InterlockedCompareExchange(&g_ready, 0, 0)) {
        Fields f;
        if (Decompose(delegateTitle, g_grammar, &f)) {
            syms::CheckPredicateAgainstParsed(controller);
        }
    }

    // Posted, not applied inline: this runs inside Chromium's title getter, and
    // writing the window text from there would re-enter composition.
    if (g_applyTitleMsg) PostMessageW(hWnd, g_applyTitleMsg, 0, 0);
}

void EnsureSubclassed(HWND hWnd) {
    if (!g_applyIconMsg && !g_applyTitleMsg) {
        return;
    }
    // Refuse once teardown has begun. Hooks are still live during
    // Wh_ModBeforeUninit, so without this a window can be subclassed again
    // AFTER teardown removed its subclass - and a window still pointing at this
    // image when it unmaps takes the browser down on its next message.
    if (InterlockedCompareExchange(&g_passthrough, 0, 0)) return;

    AcquireSRWLockExclusive(&g_lock);
    WindowState& st = g_states[hWnd];
    const bool already = st.subclassed;
    ReleaseSRWLockExclusive(&g_lock);
    if (already) return;

    // Record success, not intent. This used to set `subclassed = true` before
    // calling, and ignore the result - so a single failure both suppressed
    // every retry and left teardown believing there was a subclass to remove.
    if (InterlockedCompareExchange(&g_passthrough, 0, 0)) return;
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, FrameSubclassProc,
                                                       0)) {
        Wh_Log(L"could not subclass a browser window; its icon and named-window "
               L"title will not refresh");
        return;
    }
    AcquireSRWLockExclusive(&g_lock);
    g_states[hWnd].subclassed = true;
    ReleaseSRWLockExclusive(&g_lock);
}

// Put a window's own icons back and release ours. Leaving a destroyed icon on a
// live window would leave it blank until the browser next set one.
void RestoreIcon(HWND hWnd, WindowState& st) {
    if (!st.iconSaved) {
        return;
    }
    if (IsWindow(hWnd)) {
        // Bounded, like every other cross-thread send here. This runs from
        // Wh_ModUninit on an arbitrary thread and reaches each browser UI
        // thread; an unbounded SendMessageW means one busy window hangs the
        // whole uninstall while the user watches a stuck disable.
        DWORD_PTR unused = 0;
        SendMessageTimeoutW(hWnd, WM_SETICON, ICON_SMALL,
                            reinterpret_cast<LPARAM>(st.originalSmall),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &unused);
        SendMessageTimeoutW(hWnd, WM_SETICON, ICON_BIG,
                            reinterpret_cast<LPARAM>(st.originalBig),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &unused);
    }
    if (st.oursSmall) DestroyIcon(st.oursSmall);
    if (st.oursBig)   DestroyIcon(st.oursBig);
    st.oursSmall = st.oursBig = nullptr;
}


BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    // Consume the pairing left by the title getter earlier in this same
    // synchronous call stack, and clear it immediately either way.
    void* const      pendingController = syms::t_controller;
    const bool       pendingValid      = syms::t_delegateValid;
    const std::wstring pendingTitle    = pendingValid ? syms::t_delegateTitle
                                                      : std::wstring();
    syms::t_controller    = nullptr;
    syms::t_delegateValid = false;

    if (InterlockedCompareExchange(&g_passthrough, 0, 0) || t_inHook ||
        !lpString || !*lpString || !IsBrowserFrame(hWnd)) {
        return SetWindowTextW_Original(hWnd, lpString);
    }

    // Accept the pairing only if this really is the write that getter produced.
    //
    // Chromium suppresses the write when the composed title has not changed, so
    // a getter call frequently has no matching SetWindowTextW - and all browser
    // frames share one UI thread, so the value would otherwise be picked up by
    // the NEXT window's write and stored permanently. Requiring the string to
    // match is the first half of the check; the second is refusing a controller
    // already known to belong to a different window, since two windows can
    // legitimately carry the same title.
    void* controller = nullptr;
    if (pendingValid && pendingController && pendingTitle == lpString) {
        const HWND known = syms::HwndForController(pendingController);
        if (!known || known == hWnd) {
            controller = pendingController;
        } else if (g_settings.verbose) {
            Wh_Log(L"pairing refused: that controller already belongs to "
                   L"another window");
        }
    } else if (pendingController && g_settings.verbose) {
        Wh_Log(L"pairing refused: valid=%d titleMatch=%d written='%s' getter='%s'",
               pendingValid ? 1 : 0, (pendingTitle == lpString) ? 1 : 0,
               lpString, pendingTitle.c_str());
    }

    // Decide whether this window carries a user title BEFORE composing, and do
    // it outside our own lock: the query walks four hops into Chromium objects.
    //
    // The steady-state gate is not optional. This follows a pointer out of a
    // Browser, which is the exact shape that crashed v0.5 on a window still
    // under construction - "the object is right" says nothing about whether it
    // is initialised. Two completed title transforms is the same evidence of
    // completed construction the favicon path uses.
    if (controller) {
        AcquireSRWLockShared(&g_lock);
        const auto it = g_states.find(hWnd);
        const bool settled = it != g_states.end() && it->second.titleWrites >= 2;
        ReleaseSRWLockShared(&g_lock);
        // lpString IS the delegate's title - Chromium passes what the getter
        // returned straight through - so it is the right cache key: renaming and
        // un-naming both change it.
        syms::RefreshNameVerdict(controller, lpString, settled);
    }

    t_inHook = true;
    std::wstring out;
    bool         changed = false;
    {
        AcquireSRWLockExclusive(&g_lock);
        WindowState& st = g_states[hWnd];
        const DWORD  tid = GetWindowThreadProcessId(hWnd, nullptr);
        if (st.tid && st.tid != tid) {
            st = WindowState{};  // HWND was recycled; discard stale state
        }
        st.tid = tid;

        if (st.applied == lpString) {
            // Our own string coming back around. Keep `source` intact so a
            // settings change can still recompose from the original.
            ReleaseSRWLockExclusive(&g_lock);
            t_inHook = false;
            return SetWindowTextW_Original(hWnd, lpString);
        }
        if (controller) {
            st.controller = controller;
        }
        ++st.titleWrites;
        st.source = lpString;
        out = ComposeFor(st.source, st.controller);
        st.applied = out;
        changed = (out != lpString);
        ReleaseSRWLockExclusive(&g_lock);
    }
    t_inHook = false;

    // After the title, and only ever with the HWND we were handed.
    if (controller) {
        EnsureSubclassed(hWnd);
        ApplyFavicon(hWnd, controller);
    }

    return SetWindowTextW_Original(hWnd, changed ? out.c_str() : lpString);
}

// ---- discovery --------------------------------------------------------------

std::wstring DirOfModule(const wchar_t* name) {
    HMODULE m = GetModuleHandleW(name);
    if (!m) return {};
    WCHAR p[MAX_PATH];
    if (!GetModuleFileNameW(m, p, ARRAYSIZE(p))) return {};
    std::wstring s(p);
    const size_t at = s.rfind(L'\\');
    return (at == std::wstring::npos) ? std::wstring() : s.substr(0, at);
}

// Locale candidates, best first. A wrong guess is safe: the discovered suffix
// then fails to match real titles and nothing is rewritten.
std::vector<std::wstring> LocaleCandidates() {
    std::vector<std::wstring> out;
    auto add = [&out](std::wstring v) {
        if (!v.empty() && std::find(out.begin(), out.end(), v) == out.end()) {
            out.push_back(std::move(v));
        }
    };

    int     argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 1; i < argc; ++i) {
            if (_wcsnicmp(argv[i], L"--lang=", 7) == 0) add(argv[i] + 7);
        }
        LocalFree(argv);
    }

    // The browser's own UI language, which need not match the OS.
    if (const wchar_t* la = _wgetenv(L"LOCALAPPDATA"); la) {
        const std::wstring ls =
            std::wstring(la) +
            (g_isChrome ? L"\\Google\\Chrome\\User Data\\Local State"
                        : L"\\Microsoft\\Edge\\User Data\\Local State");
        const std::vector<uint8_t> buf = ReadWholeFile(ls);
        if (!buf.empty()) {
            const std::string_view sv(reinterpret_cast<const char*>(buf.data()),
                                      buf.size());
            if (const size_t at = sv.find("\"app_locale\":\"");
                at != std::string_view::npos) {
                const size_t b = at + 14;
                const size_t e = sv.find('"', b);
                if (e != std::string_view::npos && e - b < 32) {
                    std::wstring w;
                    if (pak::Utf8ToWide(sv.substr(b, e - b), w)) add(w);
                }
            }
        }
    }

    WCHAR loc[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(loc, ARRAYSIZE(loc))) add(loc);
    add(L"en-US");
    return out;
}

void SweepAllWindows() {
    struct Ctx {
        std::vector<HWND> frames;
    } ctx;
    EnumWindows(
        [](HWND h, LPARAM lp) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid == GetCurrentProcessId() && IsBrowserFrame(h)) {
                reinterpret_cast<Ctx*>(lp)->frames.push_back(h);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    int changed = 0;
    for (HWND h : ctx.frames) {
        // Teardown must be able to cut a sweep short. Without this the worker
        // can still be walking dozens of windows when Wh_ModBeforeUninit tries
        // to join it, and the DLL would then be unloaded out from under a
        // thread that is still executing this function.
        if (StopRequested()) {
            Wh_Log(L"sweep aborted: shutting down");
            return;
        }
        WCHAR buf[1024];
        const int n = GetWindowTextW(h, buf, ARRAYSIZE(buf));
        if (n <= 0) continue;

        std::wstring cur(buf, static_cast<size_t>(n));
        std::wstring src;
        {
            AcquireSRWLockShared(&g_lock);
            const auto it = g_states.find(h);
            // If we already rewrote this window, recompose from the ORIGINAL,
            // never from what is on screen.
            src = (it != g_states.end() && !it->second.source.empty() &&
                   it->second.applied == cur)
                      ? it->second.source
                      : cur;
            ReleaseSRWLockShared(&g_lock);
        }
        // No controller here: a sweep runs outside Chromium's title-composition
        // call stack, so named windows are simply left alone until Chromium
        // next updates them itself.
        const std::wstring out = ComposeFor(src, nullptr);
        {
            AcquireSRWLockExclusive(&g_lock);
            WindowState& st = g_states[h];
            st.source  = src;
            st.applied = out;
            st.tid     = GetWindowThreadProcessId(h, nullptr);
            ReleaseSRWLockExclusive(&g_lock);
        }
        if (out != cur) {
            WriteTitleFromOtherThread(h, out);
            ++changed;
        }
        // Icons are refreshed by the window itself; posting is safe from here,
        // calling into Chromium from this thread would not be.
        if (g_settings.useFavicon && g_applyIconMsg) {
            bool known = false;
            AcquireSRWLockShared(&g_lock);
            if (const auto it = g_states.find(h); it != g_states.end()) {
                known = it->second.controller != nullptr;
            }
            ReleaseSRWLockShared(&g_lock);
            if (known) {
                PostMessageW(h, g_applyIconMsg, 0, 0);
            }
        }
    }
    Wh_Log(L"sweep: %zu frame(s), %d retitled", ctx.frames.size(), changed);
}

DWORD WINAPI DiscoveryThread(LPVOID) {
    const wchar_t* const browserDll = g_isChrome ? L"chrome.dll" : L"msedge.dll";

    // The browser module is not loaded at injection time, so wait for it rather
    // than concluding it is absent. Bounded: a process that never loads it is
    // not a browser process we care about.
    HMODULE chromium = nullptr;
    for (int i = 0; i < 60 && !StopRequested(); ++i) {
        chromium = GetModuleHandleW(browserDll);
        if (chromium) break;
        Sleep(500);
    }
    if (!chromium) {
        Wh_Log(L"%s never loaded; nothing to do in this process", browserDll);
        return 0;
    }

    std::wstring dir = DirOfModule(browserDll);
    if (dir.empty()) {
        WCHAR p[MAX_PATH];
        if (GetModuleFileNameW(nullptr, p, ARRAYSIZE(p))) {
            std::wstring s(p);
            const size_t at = s.rfind(L'\\');
            if (at != std::wstring::npos) dir = s.substr(0, at);
        }
    }
    const std::wstring hint = g_isChrome ? L"Chrome" : L"Edge";

    Grammar g;
    bool    ok = false;
    for (const std::wstring& loc : LocaleCandidates()) {
        if (StopRequested()) return 0;
        const std::wstring path = dir + L"\\Locales\\" + loc + L".pak";
        const std::vector<uint8_t> buf = ReadWholeFile(path);
        if (buf.empty()) continue;

        pak::File f;
        if (!pak::Parse(buf.data(), buf.size(), f)) {
            Wh_Log(L"pak layout not recognized: %s", path.c_str());
            continue;
        }
        Grammar cand;
        if (DiscoverGrammar(f, hint, &cand)) {
            g  = std::move(cand);
            ok = true;
            Wh_Log(L"grammar from %s: %zu suffix, %zu marker, %zu sep, %zu count",
                   path.c_str(), g.suffixes.size(), g.markerTails.size(),
                   g.slot2Seps.size(), g.countForms.size());
            break;
        }
    }

    if (!g_settings.suffixOverride.empty()) {
        g.suffixes.insert(g.suffixes.begin(), g_settings.suffixOverride);
        ok = true;
        Wh_Log(L"using suffix override");
    }
    if (!ok) {
        Wh_Log(L"DISCOVERY FAILED - no titles will be changed. Set the browser "
               L"suffix override in settings if this persists.");
        return 0;
    }

    // Whether titles here carry a tab count decides which proof regime the
    // symbol layer can use: Edge discovers count forms, Chrome discovers none.
    InterlockedExchange(&syms::g_numericPossible, g.countForms.empty() ? 0 : 1);

    AcquireSRWLockExclusive(&g_lock);
    g_grammar = std::move(g);
    ReleaseSRWLockExclusive(&g_lock);
    InterlockedExchange(&g_ready, 1);

    // Only now is a sweep meaningful. Doing it at AfterInit would retitle
    // nothing, and on a session with many open windows that reads as "the mod
    // does not work" until each window happens to change its own title.
    SweepAllWindows();

    // Symbols LAST, deliberately.
    //
    // Resolving them walks ~1.5-1.9 million symbols and can take a long time on
    // first run for a build. Composition is gated on g_ready, so doing this any
    // earlier leaves every title untransformed for the whole duration - which
    // looks exactly like the mod being broken. The core feature must never wait
    // on an optional one.
    if (g_settings.useSymbols && !StopRequested()) {
        Wh_Log(L"resolving symbols (this can take a while on first run)...");
        if (syms::Install(chromium)) {
            InterlockedExchange(&syms::g_enabled, 1);
        } else {
            Wh_Log(L"symbol layer unavailable - continuing without it");
        }
    }

    // ---- maintenance loop ---------------------------------------------------
    //
    // ONE thing the browser never announces: a favicon arriving. It loads
    // asynchronously, usually after the title has settled, and Chrome makes the
    // consequence obvious - a Chrome title carries no tab count, so far fewer
    // events touch it than on Edge, and the icon only appeared once the user
    // switched tabs.
    //
    // Deliberately icon-only. The TITLE needs no polling and must not get any:
    // a rename changes the delegate string, so Chromium writes and the hook
    // sees it; a tab-count change is caught by the count() hook. Polling it
    // instead would be quadratic - each recompose walks every browser window to
    // resolve its controller, so refreshing n windows costs n^2 hops, which on a
    // seventy-window session is thousands of walks every couple of seconds.
    //
    // The icon poll is affordable only because of the identity guard in
    // ApplyFavicon: the unchanged case is one call and one compare, with no
    // bitmap conversion and no HICON built.
    //
    // Posted to each window's own UI thread, never done here - this is the
    // discovery worker, and every Chromium object involved is UI-thread-only.
    // One line, once, under verbose. It answers the first question a "the icons
    // stopped working" report raises - did the worker reach this loop at all,
    // and with which settings - and not being able to answer that is what sent
    // an entire favicon investigation at the wrong subsystem three times over.
    if (g_settings.verbose) {
        Wh_Log(L"maintenance loop entered: useFavicon=%d iconMsg=%u symbols=%d",
               g_settings.useFavicon ? 1 : 0, g_applyIconMsg,
               static_cast<int>(
                   InterlockedCompareExchange(&syms::g_enabled, 0, 0)));
    }

    while (!StopRequested()) {
        // Responsive to teardown: many short waits rather than one long one, so
        // an uninstall never waits two seconds for this loop to notice.
        for (int i = 0; i < 20 && !StopRequested(); ++i) {
            Sleep(100);
        }
        if (StopRequested()) break;

        // Kick the correlation sweep. Correlation is UI-thread-only and used to
        // run only from the title getter, so an IDLE browser correlated nothing
        // and the whole symbol layer sat dormant - no counts, no icons - until
        // the user happened to touch it. Subclassing needs no controller, so any
        // frame can be asked to run the sweep for everyone.
        if (InterlockedCompareExchange(&syms::g_enabled, 0, 0) && g_correlateMsg) {
            struct Ctx { std::vector<HWND> frames; } ctx;
            EnumWindows(
                [](HWND h, LPARAM lp) -> BOOL {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(h, &pid);
                    if (pid == GetCurrentProcessId() && IsBrowserFrame(h)) {
                        reinterpret_cast<Ctx*>(lp)->frames.push_back(h);
                    }
                    return TRUE;
                },
                reinterpret_cast<LPARAM>(&ctx));
            if (!ctx.frames.empty()) {
                EnsureSubclassed(ctx.frames.front());
                PostMessageW(ctx.frames.front(), g_correlateMsg, 0, 0);
            }
        }

        if (!g_settings.useFavicon || !g_applyIconMsg) continue;

        std::vector<HWND> managed;
        size_t nStates = 0, nSubclassed = 0, nController = 0;
        {
            AcquireSRWLockShared(&g_lock);
            managed.reserve(g_states.size());
            nStates = g_states.size();
            for (const auto& [hWnd, st] : g_states) {
                if (st.subclassed) ++nSubclassed;
                if (st.controller) ++nController;
                if (st.subclassed && st.controller) managed.push_back(hWnd);
            }
            ReleaseSRWLockShared(&g_lock);
        }
        // DIAGNOSTIC: separates the three ways a window can fail to receive an
        // icon refresh - no state at all, no subclass, or no controller.
        //
        // On every change, and otherwise at most once a minute. Both halves are
        // deliberate, and each alone was tried and was wrong. Change-triggered
        // alone is what hid the favicon fault: seeded with a sentinel, it fires
        // once seconds after load and never again, so every capture opened later
        // saw nothing and read as "this code never runs". Time-triggered alone
        // is a line every ten seconds forever on an idle session - exactly the
        // log noise this mod was fairly pulled up on. Transitions are the signal;
        // the periodic line exists only so a late capture still learns the
        // steady state.
        //
        // Worker-thread-only, so plain statics rather than interlocked.
        if (g_settings.verbose) {
            static size_t lastManaged = static_cast<size_t>(-1);
            static LONG   lastMinute  = -1;
            const LONG    minute = static_cast<LONG>(GetTickCount64() / 60000);
            if (managed.size() != lastManaged || minute != lastMinute) {
                lastManaged = managed.size();
                lastMinute  = minute;
                Wh_Log(L"icon refresh targets: %zu (of %zu known windows; "
                       L"%zu subclassed, %zu with a controller)",
                       managed.size(), nStates, nSubclassed, nController);
            }
        }
        for (HWND h : managed) {
            if (StopRequested()) break;
            if (IsWindow(h)) PostMessageW(h, g_applyIconMsg, 0, 0);
        }
    }
    return 0;
}

// Preset id -> template. The labels in the settings dropdown are rendered
// samples of these, so the two must be kept in step: a label that no longer
// matches its template is worse than no preview at all.
const wchar_t* TemplateForPreset(std::wstring_view id) {
    if (id == L"keep_profile")    return L"{title}?( {more})?( - {profile})";
    if (id == L"with_count")      return L"{title}?( {more})";
    // `min2` throughout: now that the tab count comes from the browser rather
    // than from the title, it is known even for a one-tab window - and a bare
    // "01" on every single-tab window is noise. min2 renders nothing below two,
    // which is exactly the behaviour these presets had when the count could only
    // come from a title that omitted it.
    if (id == L"bracket")         return L"?([{count:min2}] ){title}";
    if (id == L"bold")            return L"?({count:min2:bold} ){title}";
    if (id == L"sup")             return L"?({count:min2:sup} ){title}";
    if (id == L"sup_pad")         return L"?({count:min2:pad2:sup} ){title}";
    if (id == L"sup_pad_profile") return L"?({count:min2:pad2:sup} ){title}?( \\u2014 {profile:italic})";
    if (id == L"title_only")      return L"{title}";
    return nullptr;  // "custom", or an id from a newer version of the mod
}

void LoadSettings() {
    const std::wstring preset =
        WindhawkUtils::StringSetting::make(L"Format.Preset").get();
    if (const wchar_t* tpl = TemplateForPreset(preset); tpl) {
        g_settings.normal = tpl;
    } else {
        g_settings.normal =
            WindhawkUtils::StringSetting::make(L"Format.Custom").get();
    }
    g_settings.named =
        WindhawkUtils::StringSetting::make(L"Format.Named").get();
    g_settings.chromeOverride =
        WindhawkUtils::StringSetting::make(L"Format.ChromeOverride").get();
    g_settings.suffixOverride =
        WindhawkUtils::StringSetting::make(L"Parsing.BrowserSuffix").get();
    g_settings.enabled = Wh_GetIntSetting(L"Parsing.Enabled") != 0;
    g_settings.useSymbols = Wh_GetIntSetting(L"Symbols.Enabled") != 0;
    g_settings.useFavicon = Wh_GetIntSetting(L"Symbols.Favicon") != 0;
    g_settings.verbose = Wh_GetIntSetting(L"Advanced.VerboseLogging") != 0;

    const int mx = Wh_GetIntSetting(L"Advanced.MaxTitleChars");
    g_settings.maxChars = (mx > 16) ? static_cast<size_t>(mx) : 512;

    if (g_settings.normal.empty()) {
        g_settings.normal = L"{title}?( {more})?( - {profile})";
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    WCHAR exe[MAX_PATH] = L"";
    GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe));
    const WCHAR* base = wcsrchr(exe, L'\\');
    base = base ? base + 1 : exe;
    g_isChrome = (_wcsicmp(base, L"chrome.exe") == 0);

    // The mod is injected into every browser process - renderers, GPU, network,
    // utility - and only the browser process owns frame windows. Match the
    // "--type=" prefix on argv entries rather than searching the raw command
    // line: a URL containing that text would otherwise make the browser process
    // look like a child, and the mod would silently never load anywhere.
    int     argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    bool    isChild = false;
    if (argv) {
        for (int i = 1; i < argc; ++i) {
            if (_wcsnicmp(argv[i], L"--type=", 7) == 0) {
                isChild = true;
                break;
            }
        }
        LocalFree(argv);
    }
    if (isChild) {
        return FALSE;
    }

    LoadSettings();
    g_applyIconMsg =
        RegisterWindowMessageW(L"WindhawkChromiumWindowTitleFormat.ApplyIcon");
    g_applyTitleMsg =
        RegisterWindowMessageW(L"WindhawkChromiumWindowTitleFormat.ApplyTitle");
    g_correlateMsg =
        RegisterWindowMessageW(L"WindhawkChromiumWindowTitleFormat.Correlate");

    if (!WindhawkUtils::SetFunctionHook(SetWindowTextW, SetWindowTextW_Hook,
                                        &SetWindowTextW_Original)) {
        Wh_Log(L"failed to hook SetWindowTextW");
        return FALSE;
    }

    // NOTE: the symbol layer is deliberately NOT set up here. Windhawk injects
    // early enough that msedge.dll / chrome.dll is not loaded yet, so
    // GetModuleHandleW returns null and resolution can never succeed. It is
    // installed from the discovery thread instead, which already runs late
    // enough to find the browser module. Resolution is also slow (these modules
    // carry ~1.5 million symbols), and Wh_ModInit runs on the browser's startup
    // path where blocking is not acceptable.
    return TRUE;
}

void Wh_ModAfterInit() {
    g_worker = CreateThread(nullptr, 0, DiscoveryThread, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"failed to start discovery thread");
    }
}

// The two-argument form must return BOOL. Returning FALSE (or declaring this
// void) makes Windhawk fall back to reloading the whole mod on every settings
// change, which would drop all per-window state and lose the original titles.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
    LoadSettings();
    if (InterlockedCompareExchange(&g_ready, 0, 0)) {
        SweepAllWindows();  // recomposes from cached `source`, not from screen
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    // Hooks are still live here, so stop transforming before unwinding. This
    // flag is also what makes the worker abandon an in-progress sweep, and what
    // makes EnsureSubclassed refuse, so it has to be set first.
    InterlockedExchange(&g_passthrough, 1);

    if (g_worker) {
        // WAIT UNCONDITIONALLY. This used to give up after 10 s and leak the
        // handle, which does not help: Windhawk unloads this DLL right after
        // Wh_ModUninit returns, and what matters is the worker's instruction
        // pointer, not its handle. A thread still executing mod code when the
        // image unmaps faults the browser.
        //
        // The timeout was not hypothetical either. The worker's one long
        // operation is HookSymbols over ~1.5-1.9M symbols, which takes no
        // cancellation callback and cannot be interrupted, so disabling the mod
        // mid-resolution blew straight past 10 s. The stop flag is checked
        // before resolution starts, but that is a race, not a guarantee - so
        // the join has to be the guarantee.
        //
        // The visible cost is that disabling the mod during a first-run symbol
        // resolution waits for it. That is the correct trade against a
        // use-after-unmap.
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }

    // Subclasses are NOT removed here. They are removed in Wh_ModUninit, after
    // Windhawk has torn the hooks down - otherwise the still-live title getter
    // can put one straight back through
    // GetTitle_hook -> CorrelateAllNamedWindows -> OnControllerCorrelated ->
    // EnsureSubclassed, and a window left pointing at this image when it
    // unmaps crashes the browser on its next message.
}

void Wh_ModUninit() {
    // FIRST, before anything else: drop the subclasses. Hooks and the worker are
    // both gone by now, so nothing can reinstall one behind us.
    //
    // Done by hand rather than with WindhawkUtils::RemoveAllWindowSubclasses(),
    // which exists only on Windhawk 2.x - the catalog also compiles against
    // 1.6.1 and 1.7.3, and that call was the one non-portable API in this mod.
    //
    // The list is copied out under the lock and the removals performed outside
    // it: RemoveWindowSubclassFromAnyThread marshals to each window's own
    // thread, and holding a lock across that is how a teardown deadlocks.
    {
        std::vector<HWND> subclassed;
        AcquireSRWLockShared(&g_lock);
        subclassed.reserve(g_states.size());
        for (const auto& [hWnd, st] : g_states) {
            if (st.subclassed) subclassed.push_back(hWnd);
        }
        ReleaseSRWLockShared(&g_lock);
        for (HWND hWnd : subclassed) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                             FrameSubclassProc);
        }
    }

    // Hooks are removed by now, so these restoring writes are not intercepted.
    // Leaving rewritten titles behind after an uninstall would be unacceptable.
    std::unordered_map<HWND, WindowState> snapshot;
    AcquireSRWLockExclusive(&g_lock);
    snapshot.swap(g_states);
    ReleaseSRWLockExclusive(&g_lock);

    int restored = 0;
    for (auto& [hWnd, st] : snapshot) {
        RestoreIcon(hWnd, st);
    }
    for (const auto& [hWnd, st] : snapshot) {
        if (st.source.empty() || st.source == st.applied) continue;
        if (!IsWindow(hWnd)) continue;
        WriteTitleFromOtherThread(hWnd, st.source);
        ++restored;
    }
    Wh_Log(L"restored %d title(s)", restored);
}


