// ==WindhawkMod==
// @id              chromium-window-title-format
// @name            Chromium Window Title Format
// @description     Customize how Edge and Chrome compose window titles - drop the browser suffix, restyle the tab count and profile, and rebuild the title from a template.
// @version         1.0
// @author          mazany
// @github          https://github.com/mazany
// @twitter         https://x.com/tomazany
// @donateUrl       https://ko-fi.com/mazany
// @include         msedge.exe
// @include         chrome.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

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

A window you named yourself is left alone for the same reason as the rest: its
title is *only* the name you gave it - no tab count, no page title, no profile -
so there is nothing in the text for a template to work from, and nothing that
distinguishes it from a dialog or an app window. Recognising those needs the
browser's own objects rather than its text, which is a separate feature and a
separate discussion.

## Edge and Chrome are not equally capable

Chrome's window title contains **only** the page title and the browser name -
there is no tab count and no profile anywhere in it. That is a fact about
Chrome, not a limitation of the mod, so `{extra}`, `{more}`, `{profile}` and
`{private}` are always empty there and any `?( ... )` group containing only
those tokens disappears. `{title}` and `{browser}` work on both.

On Chrome, `{count}` is therefore empty too: the count is not in the title, and
this mod reads only the title.

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
        - sup_pad: "⁰⁶ Inbox   (superscript, zero-padded to two digits)"
        - sup_pad_profile: "⁰⁶ Inbox — 𝘗𝘦𝘳𝘴𝘰𝘯𝘢𝘭   (padded count, italic profile)"
        - title_only: "Inbox   (page title only)"
        - custom: "Custom - use the template below"
      $description: >-
        Samples use a 17-tab window in the Personal profile, except the two
        zero-padded options, which show a 6-tab one - at 17 tabs the padding
        makes no difference and they would look identical to the plain
        superscript option.
        .
        A window with a single tab shows no count at all in every count-bearing
        option, rather than "1".
        .
        The tab count and profile exist only on Edge, so on Chrome every option
        collapses to the page title. Pick Custom to write your own.
    - Custom: "?({count:min2:pad2:sup} ){title}"
      $name: Custom template
      $description: >-
        Used only when the format above is set to Custom. Tokens: {title} {more}
        {count} {extra} {profile} {private} {browser}. Wrap optional parts in
        ?( ... ) so the whole group vanishes when every token inside is empty.
        Modifiers chain with a colon, e.g. {count:pad2:sup} or {profile:italic}.
        Escapes: \{ \} \? \( \) \\ and \uXXXX.
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

// Directly used, and previously reaching this file only through
// windhawk_utils.h. A translation unit that compiles because of what its
// dependencies happen to include is one compiler update away from not.
#include <cstdint>   // uint32_t, uint64_t, intptr_t
#include <cstdlib>   // _wtoi, _wgetenv
#include <cstring>   // memcpy
#include <cwchar>    // wcsrchr, _wcsnicmp, wcsncmp
#include <cwctype>   // iswalnum, towupper, towlower
#include <utility>   // std::pair, std::move


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
    // Display names of the profiles this install actually has, read from the
    // browser's own Local State. Empty means "could not be determined", which is
    // treated as "do not guess" - see the profile slot in Decompose.
    std::vector<std::wstring> profileNames;
};

std::wstring TrimCopy(std::wstring_view s) {
    size_t b = 0, e = s.size();
    // The bidi marks belong here, and leaving them out had teeth. IsFormatEffector
    // already treats U+200E/U+200F as invisible, but this did not - so a plural
    // branch in an RTL locale that opens with a bidi mark failed the parser's
    // "must start with {0}" test, the whole message was rejected, no count forms
    // were discovered, and the symbol layer then had no numeric cross-check to
    // validate itself against on exactly the build whose grammar it had just
    // failed to read.
    //
    // Ends only. Interior marks are deliberately kept: Edge's composed titles
    // carry them too, and the right-anchored matching compares against the
    // literals discovered here, so stripping them throughout would stop those
    // comparisons matching at all.
    auto sp = [](wchar_t c) {
        return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r' ||
               c == 0x00A0 || c == kZwsp || c == 0x200E || c == 0x200F;
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
        // REFUSE A FORM THAT MATCHES EVERYTHING.
        //
        // A branch whose body is exactly "{0}" - a natural way to write "add
        // nothing when there is nothing to add" - produces a form with no fixed
        // text, no prefix and no suffix. tryStripCount skips the fixed path
        // because fixed is empty, then tests EndsWith(in, post) and
        // EndsWith(head, pre) against empty strings, both of which are trivially
        // true. Every title ending in a digit would then parse as carrying a tab
        // count: "Bug 42" becomes title "Bug" with a count of 43.
        //
        // That is the opposite of this parser's contract, which is to leave a
        // title alone when it cannot recognise it. A form carrying no literal
        // carries no evidence, so it is dropped at discovery rather than
        // defended against at every match.
        if (form.fixed.empty() && form.pre.empty() && form.post.empty()) {
            continue;
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
            // FROM THE BRACKET, not the whole tail. The discovered marker
            // carries its own leading separator - the resource is "$1 - [In
            // Private]", so the tail stored is " - [InPrivate]" - and assigning
            // that straight to {profile} put the separator INSIDE the field.
            // The default preset supplies its own, so every InPrivate and Guest
            // window rendered "Page and 3 more pages - - [InPrivate]".
            //
            // Do NOT reuse the browser-suffix branch's strip-leading-non-
            // alphanumeric loop instead: '[' is not alphanumeric, so that eats
            // the bracket too and yields "InPrivate]".
            //
            // Ordinary named profiles are unaffected - they are matched by the
            // separator loop further down, which already stores only what
            // follows the separator.
            const size_t lb = m.find(L'[');
            const size_t rb = m.rfind(L']');
            out->profile =
                TrimCopy(lb != std::wstring::npos ? m.substr(lb) : m);
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
        //
        // MATCH A REAL PROFILE NAME, never just a trailing segment.
        //
        // Reached when no count clause matched at this level - typically a
        // one-tab window, but also a multi-profile title whose count sits BEHIND
        // the profile, which is why the count is retried below after stripping.
        // It used to strip whatever followed the last separator, and
        // slot2Seps is always {" - "} on Edge, discovered from the InPrivate
        // resource, even on an install with a single profile that never puts a
        // profile in a title at all. So a one-tab window titled
        //
        //     "GitHub - Some Repo - Microsoft Edge"
        //
        // parsed as title "GitHub" with profile "Some Repo", and every preset
        // that does not render {profile} - which is all of them except
        // keep_profile - displayed just "GitHub". Silent truncation of the
        // user's own text, in the default configuration, on the exact windows
        // the presets are tuned for.
        //
        // TWO conditions, and the count is the important one. Chromium only
        // writes a profile into the title when the install has MORE THAN ONE
        // profile, so a single-profile install never does - and matching by name
        // alone still truncated "GitHub - Personal - Microsoft Edge" on an
        // install whose one profile happened to be called Personal. Requiring
        // both "more than one profile exists" and "this is one of their names"
        // makes the rule identity, not position.
        //
        // When the names cannot be read the slot is skipped entirely: losing
        // {profile} is a missing field, while guessing is a mangled title, and
        // this parser's contract is to leave what it does not recognise alone.
        // Note the degradation is slightly wider than the field itself - a count
        // that sits behind a profile cannot be reached either, so such a title
        // keeps its count wording inside {title}.
        if (out->profile.empty() && g.profileNames.size() >= 2) {
            for (const std::wstring& sep : g.slot2Seps) {
                const size_t at = r2.rfind(sep);
                if (at == std::wstring::npos || at == 0) continue;
                const std::wstring cand = r2.substr(at + sep.size());
                if (cand.empty() || cand.size() > 64) continue;
                // EXACTLY equal, not case-insensitively. The browser renders
                // the stored name verbatim, so folding case buys nothing real and
                // only widens the false-positive window - with a profile named
                // "Work", a case-insensitive test truncates
                // "How to go on vacation - work - Microsoft Edge".
                const std::wstring trimmed = TrimCopy(cand);
                bool known = false;
                for (const std::wstring& name : g.profileNames) {
                    if (trimmed == name) {
                        known = true;
                        break;
                    }
                }
                if (!known) continue;
                out->profile = trimmed;
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
// `depth` exists to bound the recursion below, not to do anything useful.
//
// Each `?(` recurses with a std::wstring local in the frame, and the template is
// a settings field - so a pasted template of a few thousand nested `?(?(?(...`
// exhausts the browser UI thread's stack and takes the browser down. Self-
// inflicted, but "self" here means whoever the user copied the template from,
// and a crash in someone's browser is not an acceptable answer to a malformed
// setting. Past the cap the group is treated as literal text rather than
// silently dropped, so the template still renders something recognisable.
constexpr int kMaxGroupDepth = 32;

size_t Render(const std::wstring& tpl, size_t i, bool inGroup, const Fields& f,
              std::wstring* out, bool* anyNonEmpty, int depth = 0) {
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
            if (depth >= kMaxGroupDepth) {
                // Emit the delimiter literally and carry on iteratively. The
                // stack is the resource being protected, so the answer cannot
                // itself be another frame.
                out->push_back(c);
                ++i;
                continue;
            }
            std::wstring inner;
            bool         innerAny = false;
            i = Render(tpl, i + 2, /*inGroup=*/true, f, &inner, &innerAny,
                       depth + 1);
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
    bool         verbose = false;
    size_t       maxChars = 512;
};

struct WindowState {
    std::wstring source;   // last title the browser composed, pre-transform
    std::wstring applied;  // last title we wrote; the echo detector
    DWORD        tid = 0;  // for HWND-recycling detection
    // Bumped on every reset and every new source. A composition runs without the
    // lock held, so the commit has to prove it is still committing to the state
    // it read - `tid` cannot do that, since every browser frame shares one UI
    // thread and a recycled HWND keeps the same one.
    unsigned     generation    = 0;
    // Independently established as a fully-constructed frame, by having been
    // found through EnumWindows as a VISIBLE top-level browser window with a
    // real title. That is evidence the `titleWrites >= 2` gate exists to obtain,
    // and it is the only evidence available for a window that never writes its
    // title - which on Chrome is most of them, since a Chrome title does not
    // change when tabs do.
    bool         constructed   = false;
};

Settings g_settings;

// Guards ONLY the std::wstring members of g_settings, and it has to be its own
// lock rather than g_lock.
//
// The bug it closes: LoadSettings runs on whatever thread Wh_ModSettingsChanged
// is called on and reassigns those strings, while ComposeFor - on a browser UI
// thread, mid-title-write - used to bind a const reference to one and carry it
// through Render. std::wstring::operator= frees the old buffer, so a settings
// change concurrent with any title write is a use-after-free inside the
// browser. Every read of a settings STRING now copies it under this lock.
//
// It cannot be g_lock: SetWindowTextW_Hook calls ComposeFor while already
// holding g_lock exclusively, and SRW locks are not reentrant, so reusing it
// here would deadlock the browser's UI thread on the first title write.
//
// The scalar members (bool/size_t) are deliberately still read without it. They
// are independently meaningful, a stale value costs at most one composition
// rendered with the previous setting, and threading a lock through the dozens of
// g_settings.verbose reads would cost far more than it buys.
SRWLOCK g_settingsLock = SRWLOCK_INIT;
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



std::wstring ComposeFor(const std::wstring& source) {
    if (!g_settings.enabled || !InterlockedCompareExchange(&g_ready, 0, 0)) {
        return source;
    }
    Fields f;
    if (!Decompose(source, g_grammar, &f)) {
        // Not a title this mod recognises: a window the user has named, a PWA, a
        // dialog, picture-in-picture. From the string alone those are
        // indistinguishable, and telling them apart needs the browser's own
        // objects rather than its text - so the title is left exactly as it is.
        //
        // This is the hard invariant the readme states, and it is what keeps
        // every one of those window kinds safe by construction.
        if (g_settings.verbose) {
            Wh_Log(L"left as-is (does not match the discovered grammar): %s",
                   source.c_str());
        }
        return source;
    }

    // A COPY, not a reference. Binding a reference here and carrying it into
    // Render is the use-after-free described at g_settingsLock: the settings
    // thread can reassign the very string being rendered.
    std::wstring tpl;
    {
        AcquireSRWLockShared(&g_settingsLock);
        tpl = (g_isChrome && !g_settings.chromeOverride.empty())
                  ? g_settings.chromeOverride
                  : g_settings.normal;
        ReleaseSRWLockShared(&g_settingsLock);
    }
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
    // DELIBERATELY still the flag, not the event.
    //
    // Reading the event here instead looks tidier and is a trap: Wh_ModBeforeUninit
    // joins this worker with an INFINITE wait, so if CreateEventW had failed and
    // the handle were null, WaitForSingleObject would return WAIT_FAILED, this
    // would never report a stop, the worker would never exit, and Windhawk would
    // hang on uninstall with no way out. The flag cannot fail.
    return InterlockedCompareExchange(&g_passthrough, 0, 0) != 0;
}

// Manual-reset, signalled once at teardown. Purely a waker for the sleeps below:
// it exists so the worker stops sleeping in 100 ms slices just to notice a flag.
// A null handle is survivable - the waits fall back to plain Sleep.
HANDLE g_stopEvent = nullptr;

// Sleep, unless teardown starts first. Returns true if we should stop.
bool SleepOrStop(DWORD ms) {
    if (StopRequested()) return true;
    if (g_stopEvent) {
        const DWORD r = WaitForSingleObject(g_stopEvent, ms);
        if (r == WAIT_OBJECT_0) return true;
        // WAIT_FAILED must not spin: fall back to sleeping so a broken handle
        // costs latency rather than a busy loop on a browser's worker thread.
        if (r == WAIT_FAILED) Sleep(ms);
    } else {
        Sleep(ms);
    }
    return StopRequested();
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










BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (InterlockedCompareExchange(&g_passthrough, 0, 0) || t_inHook ||
        !lpString || !*lpString || !IsBrowserFrame(hWnd)) {
        return SetWindowTextW_Original(hWnd, lpString);
    }

    t_inHook = true;
    std::wstring out;
    bool         changed = false;
    unsigned     gen = 0;
    {
        AcquireSRWLockExclusive(&g_lock);
        WindowState& st = g_states[hWnd];
        const DWORD  tid = GetWindowThreadProcessId(hWnd, nullptr);
        if (st.tid && st.tid != tid) {
            // HWND was recycled; discard stale state.
            const unsigned prevGen = st.generation;
            st = WindowState{};
            st.generation = prevGen + 1;
        }
        st.tid = tid;

        if (st.applied == lpString) {
            // Our own string coming back around. Keep `source` intact so a
            // settings change can still recompose from the original.
            ReleaseSRWLockExclusive(&g_lock);
            t_inHook = false;
            return SetWindowTextW_Original(hWnd, lpString);
        }
        st.source = lpString;
        gen       = ++st.generation;
        ReleaseSRWLockExclusive(&g_lock);
    }

    // Compose with no lock held.
    //
    // Composition is pure string work, but it is deliberately kept outside the
    // lock anyway: the write below was ALREADY outside it, so "applied recorded"
    // and "text on screen" were never atomic, and holding a lock across work
    // that does not need it is how the mod previously armed a deadlock against
    // its own hooks.
    //
    // The generation check is what makes that safe. It proves this result is
    // still being committed against the state it was computed from - a check
    // `tid` cannot provide, because every browser frame shares one UI thread and
    // a recycled HWND keeps the same one.
    out     = ComposeFor(lpString);
    changed = (out != lpString);

    {
        AcquireSRWLockExclusive(&g_lock);
        const auto it = g_states.find(hWnd);
        if (it == g_states.end() || it->second.generation != gen) {
            // The entry was reset, pruned, or overtaken while composing. Its
            // newer owner will write its own title; ours is stale, so pass the
            // browser's own string through untouched rather than fight it.
            ReleaseSRWLockExclusive(&g_lock);
            t_inHook = false;
            return SetWindowTextW_Original(hWnd, lpString);
        }
        it->second.applied = out;
        ReleaseSRWLockExclusive(&g_lock);
    }
    t_inHook = false;

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

// The user-data directory this browser is actually running with.
//
// --user-data-dir wins, exactly as it does for the browser: the command line is
// already parsed a few lines below for --lang=, and reading the default location
// while the browser runs from somewhere else means answering about the wrong
// install. Falls back to the per-channel default.
std::wstring UserDataDir() {
    int     argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::wstring fromArgs;
    if (argv) {
        for (int i = 1; i < argc; ++i) {
            if (_wcsnicmp(argv[i], L"--user-data-dir=", 16) == 0) {
                fromArgs = argv[i] + 16;
                // Chromium accepts it quoted; the shell usually strips them.
                if (fromArgs.size() >= 2 && fromArgs.front() == L'"' &&
                    fromArgs.back() == L'"') {
                    fromArgs = fromArgs.substr(1, fromArgs.size() - 2);
                }
            }
        }
        LocalFree(argv);
    }
    if (!fromArgs.empty()) return fromArgs;

    const wchar_t* la = _wgetenv(L"LOCALAPPDATA");
    if (!la) return {};
    return std::wstring(la) +
           (g_isChrome ? L"\\Google\\Chrome\\User Data"
                       : L"\\Microsoft\\Edge\\User Data");
}

// The display names of the profiles this install has, from the browser's own
// Local State.
//
// A narrow scan rather than a JSON parser, but a syntax-aware one: keys are
// distinguished from values by requiring the ':' that follows a key, because
// inferring it positionally let a field whose VALUE was the word "name" be taken
// as a key - which both invented a profile called "name" and swallowed the real
// one after it. A wrong name here is not cosmetic: it is what decides whether a
// piece of the user's title gets discarded.
//
// Several keys are collected, not just "name". Chromium composes the displayed
// profile through GetNameToDisplay(), which prefers the GAIA name over the local
// one, so on a signed-in profile the segment in the title may never equal "name"
// - and the slot would then quietly stop working for exactly the multi-profile
// users it exists for. Collecting the alternatives keeps the test anchored to
// identity while covering the shapes the browser can actually display.
//
// Anything unexpected returns what was found so far, and an empty result means
// "unknown", which the caller treats as "do not strip a profile".
std::vector<std::wstring> DiscoverProfileNames() {
    std::vector<std::wstring> names;
    const std::wstring root = UserDataDir();
    if (root.empty()) return names;

    const std::vector<uint8_t> buf = ReadWholeFile(root + L"\\Local State");
    if (buf.empty()) return names;

    const std::string_view sv(reinterpret_cast<const char*>(buf.data()),
                              buf.size());
    size_t at = sv.find("\"info_cache\"");
    if (at == std::string_view::npos) return names;
    at = sv.find('{', at);
    if (at == std::string_view::npos) return names;

    // Any of these can be what the browser puts in the title.
    auto wanted = [](std::string_view k) {
        return k == "name" || k == "gaia_name" || k == "gaia_given_name" ||
               k == "shortcut_name";
    };

    int    depth  = 0;
    bool   inStr  = false;
    bool   esc    = false;
    size_t strAt  = 0;
    std::string key;
    for (size_t i = at; i < sv.size(); ++i) {
        const char c = sv[i];
        if (inStr) {
            if (esc)            { esc = false; continue; }
            if (c == '\\')      { esc = true;  continue; }
            if (c != '"')       continue;
            inStr = false;
            const std::string_view tok = sv.substr(strAt, i - strAt);
            if (depth == 2) {
                if (!key.empty()) {
                    // This string is the VALUE of a key we were waiting on.
                    std::wstring w;
                    if (!tok.empty() && tok.size() < 128 &&
                        pak::Utf8ToWide(tok, w)) {
                        // Escapes are not decoded, so a name containing one
                        // simply never matches a title - which fails closed.
                        if (std::find(names.begin(), names.end(), w) ==
                            names.end()) {
                            names.push_back(std::move(w));
                        }
                    }
                    key.clear();
                } else if (wanted(tok)) {
                    // Only a real KEY is followed by ':'. Without this test a
                    // value that happens to read "name" was mistaken for one.
                    size_t j = i + 1;
                    while (j < sv.size() && (sv[j] == ' ' || sv[j] == '\t' ||
                                             sv[j] == '\r' || sv[j] == '\n')) {
                        ++j;
                    }
                    if (j < sv.size() && sv[j] == ':') key.assign(tok);
                }
            }
            continue;
        }
        if (c == '"')  { inStr = true; strAt = i + 1; continue; }
        if (c == '{')  { ++depth; continue; }
        if (c == '}')  { if (--depth == 0) break; key.clear(); continue; }
    }
    return names;
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



// Drop state for windows that no longer exist.
//
// The WM_NCDESTROY handler cannot carry this on its own: it only runs for
// windows the mod SUBCLASSED, and subclassing is only ever reached from the
// symbol layer's correlation paths. In the default configuration - symbols off,
// which is what most users run - no window is ever subclassed, so nothing was
// ever pruned, while an entry with two title strings was created for every frame
// swept and every title written. A long session accumulates one per window ever
// opened, and a recycled HWND inherits the dead window's remembered original.
//
// Pruning here instead makes it independent of any optional feature. Icons owned
// by a dead window are destroyed rather than restored - there is no window left
// to restore them to.
void PruneDeadWindows() {
    std::vector<std::pair<HWND, WindowState>> dead;
    {
        AcquireSRWLockExclusive(&g_lock);
        for (auto it = g_states.begin(); it != g_states.end();) {
            if (!IsWindow(it->first)) {
                dead.emplace_back(it->first, std::move(it->second));
                it = g_states.erase(it);
            } else {
                ++it;
            }
        }
        ReleaseSRWLockExclusive(&g_lock);
    }

    if (g_settings.verbose && !dead.empty()) {
        Wh_Log(L"pruned %zu dead window(s)", dead.size());
    }
}

void SweepAllWindows() {
    PruneDeadWindows();

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
        const std::wstring out = ComposeFor(src);
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
    }
    Wh_Log(L"sweep: %zu frame(s), %d retitled", ctx.frames.size(), changed);
}

DWORD WINAPI DiscoveryThread(LPVOID) {
    const wchar_t* const browserDll = g_isChrome ? L"chrome.dll" : L"msedge.dll";

    // The browser module is not loaded at injection time, so wait for it rather
    // than concluding it is absent. Bounded: a process that never loads it is
    // not a browser process we care about.
    HMODULE chromium = nullptr;
    for (int i = 0; i < 60; ++i) {
        chromium = GetModuleHandleW(browserDll);
        if (chromium) break;
        if (SleepOrStop(500)) break;
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
            // The install's real profile names, so the profile slot can require
            // a match rather than taking whatever follows a separator.
            g.profileNames = DiscoverProfileNames();
            Wh_Log(L"grammar from %s: %zu suffix, %zu marker, %zu sep, %zu count, "
                   L"%zu profile name(s)",
                   path.c_str(), g.suffixes.size(), g.markerTails.size(),
                   g.slot2Seps.size(), g.countForms.size(),
                   g.profileNames.size());
            if (g.profileNames.empty()) {
                Wh_Log(L"could not read this install's profile names, so "
                       L"{profile} will stay empty rather than guess at a "
                       L"trailing segment of the page title");
            }
            break;
        }
    }

    std::wstring suffixOverride;
    {
        AcquireSRWLockShared(&g_settingsLock);
        suffixOverride = g_settings.suffixOverride;
        ReleaseSRWLockShared(&g_settingsLock);
    }
    if (!suffixOverride.empty()) {
        g.suffixes.insert(g.suffixes.begin(), suffixOverride);
        ok = true;
        Wh_Log(L"using suffix override");
    }
    if (!ok) {
        Wh_Log(L"DISCOVERY FAILED - no titles will be changed. Set the browser "
               L"suffix override in settings if this persists.");
        return 0;
    }

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
    // NOT OFFERED ON CHROME, deliberately, and this is a measurement rather
    // than a preference.
    //
    // The layer resolves against the browser's PDB. Edge's is published on the
    // Microsoft public symbol server, which is what a NULL symbolServer asks
    // for, and it resolves in minutes. Chrome's is not there at all, and the
    // file itself is enormous: the copy this was developed against measured
    // 5.18 GB on disk. Another catalog mod documents an attempt that pegged a
    // core for over four hours with noUndecoratedSymbols set. Chrome also ships
    // every few days, so that cost repeats per build.
    //
    // Every Chrome measurement in this mod's own notes was taken against a warm
    // ---- maintenance loop ---------------------------------------------------
    //
    // Its only job is to bound memory. Everything else the mod does is driven by
    // the browser writing a title, so there is nothing here to poll for.
    //
    // Reached even when discovery FAILED, deliberately. The title hook keeps
    // recording a WindowState for every frame that writes a title regardless of
    // whether a grammar was found - the cached original is what a later sweep
    // would recompose from - so returning early on failure left nothing to prune
    // and the map grew for the life of the process, in the one configuration
    // where the mod is not doing anything useful anyway.
    while (!StopRequested()) {
        // One wake a minute, on an event rather than a poll, so teardown is
        // still immediate.
        if (SleepOrStop(60000)) break;
        PruneDeadWindows();
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

// Builds the new settings into a local first, then publishes them under
// g_settingsLock. Reading the settings themselves involves calls into the
// engine, which must not happen with a lock held that a browser UI thread wants
// on every title write; and assigning field by field would leave readers seeing
// a half-updated set.
void LoadSettings() {
    Settings s;

    const std::wstring preset =
        WindhawkUtils::StringSetting::make(L"Format.Preset").get();
    if (const wchar_t* tpl = TemplateForPreset(preset); tpl) {
        s.normal = tpl;
    } else {
        s.normal = WindhawkUtils::StringSetting::make(L"Format.Custom").get();
    }
    s.named  = WindhawkUtils::StringSetting::make(L"Format.Named").get();
    s.chromeOverride =
        WindhawkUtils::StringSetting::make(L"Format.ChromeOverride").get();
    s.suffixOverride =
        WindhawkUtils::StringSetting::make(L"Parsing.BrowserSuffix").get();
    s.enabled    = Wh_GetIntSetting(L"Parsing.Enabled") != 0;
    s.verbose    = Wh_GetIntSetting(L"Advanced.VerboseLogging") != 0;

    const int mx = Wh_GetIntSetting(L"Advanced.MaxTitleChars");
    s.maxChars = (mx > 16) ? static_cast<size_t>(mx) : 512;
    if (mx <= 16) {
        // Say so. Silently substituting 512 for a deliberate 10 looks like the
        // setting being ignored, which is exactly what it is - just not
        // arbitrarily.
        Wh_Log(L"maximum title length %d is below the minimum of 17; using 512",
               mx);
    }

    if (s.normal.empty()) {
        s.normal = L"{title}?( {more})?( - {profile})";
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
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

    // Manual-reset: once teardown starts it must stay signalled, so every wait
    // in the worker returns immediately rather than one of them consuming it.
    // Failure is not fatal - SleepOrStop falls back to a plain Sleep.
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"could not create the stop event; teardown will be up to two "
               L"seconds slower");
    }

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

// Handles a settings change in place, EXCEPT for the one setting that cannot be
// honoured in place.
//
// The browser-suffix override is consumed ONCE, by the worker, during grammar
// discovery - and by the time a user reaches for it the worker has already
// returned. Its own description tells them to set it after reading a discovery
// failure in the log, so it was inert in precisely the situation it exists for,
// and looked like a setting that does nothing at all.
//
// A reload is clean here: Wh_ModUninit restores every original title, and the
// fresh instance re-runs discovery with the override in hand. Everything else is
// still applied in place, because a reload drops the per-window state and with
// it the remembered originals.
//
// Not gated on the browser: unlike anything symbol-related, this is a parse-path
// setting and applies to Chrome exactly as it does to Edge.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    std::wstring wasSuffix;
    {
        AcquireSRWLockShared(&g_settingsLock);
        wasSuffix = g_settings.suffixOverride;
        ReleaseSRWLockShared(&g_settingsLock);
    }

    LoadSettings();

    std::wstring nowSuffix;
    {
        AcquireSRWLockShared(&g_settingsLock);
        nowSuffix = g_settings.suffixOverride;
        ReleaseSRWLockShared(&g_settingsLock);
    }

    if (nowSuffix != wasSuffix) {
        Wh_Log(L"the browser suffix override changed - reloading so discovery "
               L"runs again with it");
        *bReload = TRUE;
        return TRUE;
    }
    *bReload = FALSE;

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
    // Flag first, THEN wake. The flag is what the worker believes; the event
    // only stops it sleeping. Signalling first would let it wake, re-read a flag
    // that was not yet set, and go back to sleep for a whole period.
    if (g_stopEvent) SetEvent(g_stopEvent);

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
    // Hooks are removed by now, so these restoring writes are not intercepted.
    // Leaving rewritten titles behind after an uninstall would be unacceptable.
    std::unordered_map<HWND, WindowState> snapshot;
    AcquireSRWLockExclusive(&g_lock);
    snapshot.swap(g_states);
    ReleaseSRWLockExclusive(&g_lock);

    int restored = 0;
    for (const auto& [hWnd, st] : snapshot) {
        if (st.source.empty() || st.source == st.applied) continue;
        if (!IsWindow(hWnd)) continue;
        WriteTitleFromOtherThread(hWnd, st.source);
        ++restored;
    }
    Wh_Log(L"restored %d title(s)", restored);

    // Last, and only here. Wh_ModBeforeUninit has already joined the worker, so
    // nothing can still be waiting on this handle - closing it while a wait was
    // outstanding is undefined.
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}


