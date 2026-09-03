// ==WindhawkMod==
// @id              chromium-window-title-format
// @name            Chromium Window Title Format
// @description     Customize how Edge and Chrome compose window titles - drop the browser suffix, restyle the tab count and profile, and rebuild the title from a template.
// @version         1.0
// @author          mazany
// @github          https://github.com/mazany
// @twitter         https://x.com/tomazany
// @donateUrl       https://ko-fi.com/mazany
// @compilerOptions -lshell32 -lole32 -luuid
// @include         msedge.exe
// @include         chrome.exe
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
localized resource file** to learn the exact browser suffix, the private-window
wording and the page-count wording for your language, then matches those
literals right-anchored against each title. The profile is matched against the
names the install actually has, rather than against punctuation - browsers do
not always join the profile with the same character they use elsewhere in the
title.

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
- The italic, bold-italic, serif-italic, script and fraktur styles contain **no
  digits**, so
  digits pass through unstyled.

Style short prefixes like `{count}`, not `{title}`.

## Limits worth knowing

- **If all you want is the browser name gone**, [Remove Taskbar Window Suffixes](https://windhawk.net/mods/file-explorer-remove-suffixes)
  is designed for exactly that and does it for every program at once, with far
  less machinery. Its *Universal* mode drops the last ` - ` or ` — ` segment from
  any title, and its custom regex rules can target `msedge.exe` or `chrome.exe`
  and rewrite with capture groups - which covers dropping the suffix and a fair
  amount of reordering too.

  Two things are genuinely different here, and neither is a criticism of that
  mod. It edits **how a title appears on the taskbar**, by its own description,
  while this one changes the **real window title** - so Alt+Tab, the window's own
  title bar and jump lists follow. And a regex matches text, whereas this mod
  decomposes a title into *named* fields - page title, tab count, profile,
  privacy marker - using the browser's own localized resources, so `{count}` is
  a number you can style or pad and stays right in every language the browser
  ships. If you want the suffix gone, use that mod. Use this one when you want
  the parts.
- **Edge and Chrome only.** The mod targets `msedge.exe` and `chrome.exe`. It
  needs no symbols and no PDB, so it is not pinned to a particular browser
  build, but other Chromium forks are not currently recognized and would each
  need their own entry and browser-name hint.
- **`{profile}` is matched by name, never by position.** A trailing segment is
  removed only when it is exactly one of the profile names this install actually
  has, read from the browser's own `Local State`. One profile is enough. The
  cost of matching by name is a page genuinely titled `Notes - Personal` on an
  install whose profile is `Personal`, whose tail moves into `{profile}`; the
  alternative - stripping whatever follows the last separator - would truncate
  every title containing a dash, so the name match is the floor.
  Where the profile list cannot be read at all, nothing is stripped, and any
  count sitting behind the profile is lost with it. The setting `Guess the
  profile when the profile list cannot be read` trades that back on weaker
  evidence, and is off by default - its description says what it costs.
- **The profile list is read once, at startup.** Add, rename or remove a profile
  and `{profile}` keeps working from the old list until the browser restarts or
  the mod is reloaded. This is deliberate: the list decides whether text is
  removed from a title, so going stale is safer than re-reading it underneath a
  composition in progress.
- **Titles are restored on unload on a best-effort budget.** Each window gets
  250 ms to acknowledge the restoring write, because the alternative is letting
  one unresponsive window hang the uninstall. A window whose thread is busy at
  that moment keeps the rewritten title until it next sets it itself. A window
  that has changed its own title since is left alone rather than being handed a
  now-older one. Both counts are logged.
- **The language is resolved in the browser's own order**: `--lang`, then the
  browser's stored UI language, then Windows' display-language list, then the
  regional-format locale, then `en-US`. Each is matched against the packs the
  browser actually ships, which are mostly bare language codes. Picking the
  wrong one would make the mod appear to work and rewrite nothing, so the pack
  that was read, every page-count form found in it, and the browser suffix that
  actually matched a title are all logged to make that visible.
- **A `UserDataDir` group policy is read; a portable install still is not.** The
  policy is taken from `HKLM` and then `HKCU` under
  `SOFTWARE\Policies\Microsoft\Edge` (`Google\Chrome` for Chrome), which is where
  the browser reads it and it outranks `--user-data-dir`, so a managed machine
  matches profile names normally. A policy path still holding a `${...}` variable
  that this mod cannot expand is treated as unknown rather than guessed at. What
  remains unreadable - a portable or repacked install, a truncated `Local State` -
  leaves `{profile}` empty and takes any count behind it, unless the guess setting
  above is turned on.
- **A profile that has never been renamed may not be matched.** A freshly created
  profile can be shown under a localized default label - `Personal`,
  `Henkilökohtainen` - that appears nowhere in `Local State`, which still records
  the internal name. Once the profile has been used normally the browser writes
  the displayed name and the match works. Nothing is mangled meanwhile; the
  segment simply stays in `{title}`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Format:
    - Preset: keep_profile
      $name: Format preset
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

        A window with a single tab shows no count at all in every count-bearing
        option, rather than "1".

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
    - BrowserSuffix: ""
      $name: Browser suffix override
      $description: >-
        Leave empty for automatic discovery. Set only if the log reports that
        discovery failed for your build. This is the whole literal tail
        including its leading separator, for example " - Google Chrome".
    - GuessProfileWhenUnknown: false
      $name: Guess the profile when the profile list cannot be read
      $description: >-
        Off by default, and it trades away the mod's main guarantee. Normally a
        trailing segment becomes {profile} only when it exactly matches a
        profile name read from the browser's own data, so a title the mod cannot
        account for is left untouched. On the rare install where no profile name
        can be read - a portable or repacked build - turning this on
        lets the mod accept a trailing segment on weaker evidence: that the
        browser's own page-count wording sits behind it. That recovers {count}
        on those installs, at the cost that a page whose title genuinely ends
        that way, such as "E-Mail and 16 more pages - Some Site", is read as a
        17-tab window in a profile called "Some Site". Leave it off unless the
        count matters more to you than the guarantee.
  $name: Title parsing
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Directly used, and previously reaching this file only through
// windhawk_utils.h. A translation unit that compiles because of what its
// dependencies happen to include is one compiler update away from not.
#include <shellapi.h>  // CommandLineToArgvW
#include <shlobj.h>    // SHGetKnownFolderPath, FOLDERID_LocalAppData

#include <climits>   // INT_MAX
#include <cstdint>   // uint32_t
#include <cstdlib>   // _wtoi
#include <cwchar>    // _wcsnicmp, _wcsicmp, wcsncmp
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

// Case-map through the OS, out of place.
//
// Not towupper/towlower: those follow the CRT's LC_CTYPE, and with no setlocale
// call the default "C" locale leaves every non-ASCII letter unchanged.
//
// LOCALE_NAME_INVARIANT, not the user's locale: a title token carries no
// reliable language tag, and the regional locale would make the same template
// produce different text on different machines - the Turkish dotted-I being the
// case everyone eventually hits. Returns the input unchanged if the OS declines.
std::wstring MapCase(const std::wstring& s, bool upper) {
    if (s.empty() || s.size() > static_cast<size_t>(INT_MAX)) return s;
    const DWORD flags = upper ? LCMAP_UPPERCASE : LCMAP_LOWERCASE;
    const int   n     = static_cast<int>(s.size());
    const int   need  = LCMapStringEx(LOCALE_NAME_INVARIANT, flags, s.c_str(), n,
                                      nullptr, 0, nullptr, nullptr, 0);
    if (need <= 0) return s;
    std::wstring out(static_cast<size_t>(need), L'\0');
    const int got = LCMapStringEx(LOCALE_NAME_INVARIANT, flags, s.c_str(), n,
                                  out.data(), need, nullptr, nullptr, 0);
    if (got <= 0) return s;
    out.resize(static_cast<size_t>(got));
    return out;
}

// Bound the result to `maxChars` UTF-16 units, ELLIPSIS INCLUDED, without
// splitting a surrogate pair. 0 means no bound.
std::wstring Clamp(std::wstring s, size_t maxChars) {
    if (maxChars == 0 || s.size() <= maxChars) {
        return s;
    }
    size_t cut = maxChars - 1;  // room for the ellipsis
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
    std::vector<Entry>   entries;
    const uint8_t*       base = nullptr;

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
    out.entries = std::move(entries);
    out.base    = d;
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

// A backstop against a pathological template, not a preference - a template that
// wants a shorter field uses the per-token `max<N>` modifier instead, and that
// modifier is capped here too.
//
// Deliberately high enough that composition alone cannot reach it. It applies to
// the composed result unconditionally, so a lower value would truncate a long
// page title that no template had touched - and this mod's contract elsewhere is
// to leave what it does not change byte-identical.
constexpr size_t kMaxTitleChars = 4096;

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
    // Separators discovered from the privacy-marker resource. The profile match
    // does NOT use these - it anchors on the name - because a browser can join
    // the marker and the profile with different punctuation in the same title.
    // This is the fallback boundary for an install whose names cannot be read.
    std::vector<std::wstring> slot2Seps;   // longest first
    std::vector<CountForm>    countForms;
    // Display names of the profiles this install actually has, read from the
    // browser's own Local State. Empty means "could not be determined", which is
    // treated as "do not guess" - see the profile slot in Decompose.
    std::vector<std::wstring> profileNames;
    // How many profiles the install HAS, which is a different quantity from
    // how many names were collected above: several keys are read per profile,
    // so one profile routinely yields three names. Zero means "unknown".
    int                       profileCount = 0;
};

std::wstring TrimCopy(std::wstring_view s) {
    size_t b = 0, e = s.size();
    // The bidi marks MUST be trimmed here, in step with IsFormatEffector, which
    // already treats U+200E/U+200F as invisible. Out of step, a plural branch in
    // an RTL locale that opens with a bidi mark fails the parser's "must start
    // with {0}" test, the whole message is rejected, no count forms are
    // discovered, and every multi-tab title on that build is left alone.
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

// The characters a browser may put between two parts of a title, as a SHORT
// EXPLICIT list. "Anything that is not a letter or a digit" is the tempting
// version and it is wrong here, because this scan CONSUMES what it walks over:
// by exclusion it also eats the page's own punctuation, and
// "Is this a bug? - Personal" comes back as "Is this a bug".
//
// Spaces and dashes only. A colon, slash, pipe, comma or full stop is ordinary
// title text - the browsers that really join with one are handled by matching
// their discovered separator exactly, not by this shape.
bool IsJoinerSpace(wchar_t c) {
    return c == L' ' || c == L'\t' || c == 0x00A0 || c == kZwsp ||
           c == 0x200E || c == 0x200F || c == 0x3000;
}

bool IsJoinerDash(wchar_t c) {
    switch (c) {
        case L'-':    // hyphen-minus
        case 0x2010:  // hyphen
        case 0x2011:  // non-breaking hyphen
        case 0x2012:  // figure dash
        case 0x2013:  // en dash
        case 0x2014:  // em dash
        case 0x2015:  // horizontal bar
        case 0x2212:  // minus sign
        case 0xFF0D:  // fullwidth hyphen-minus
            return true;
        default:
            return false;
    }
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
    // FILE_SHARE_DELETE as well as read and write. These files belong to the
    // browser, which replaces Local State by rename; without delete sharing a
    // read here can make the browser's own write fail.
    HANDLE h = CreateFileW(
        path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
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
        // REFUSE A FORM THAT MATCHES EVERYTHING. A branch body of exactly "{0}"
        // yields no fixed text, prefix or suffix, and both EndsWith tests then
        // pass trivially - so every title ending in a digit parses as a count,
        // turning "Bug 42" into title "Bug" with a count of 43. No literal means
        // no evidence; drop it at discovery rather than defend at every match.
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
                while (k < tail.size() && !IsCharAlphaNumericW(tail[k])) {
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
    int          extra    = 0;
    bool         hasCount = false;
};

// Right-anchored and validating. Any step that cannot match exactly makes the
// whole parse fail, and a failed parse means the title is left untouched.
// Failure is a valid and preferred outcome - never a reason to guess.
// `guessProfile` is the Parsing.GuessProfileWhenUnknown setting. It defaults to
// false here as well as in the settings, so a caller that does not opt in gets
// the fail-closed behaviour and nothing else has to know about the option.
bool Decompose(const std::wstring& in, const Grammar& g, Fields* out,
               bool guessProfile = false) {
    // 1. browser suffix (the entire literal tail, byte for byte)
    std::wstring r1;
    bool         matched = false;
    for (const std::wstring& s : g.suffixes) {
        if (EndsWith(in, s)) {
            // The suffix that MATCHED, once per load of this DLL. Every Edge
            // channel's .pak carries all four product names, and the list is
            // longest first, so naming a candidate at discovery time reports
            // "Canary" on a stable install - a channel claim this mod cannot
            // make.
            static volatile LONG logged = 0;
            if (InterlockedExchange(&logged, 1) == 0) {
                Wh_Log(L"browser suffix in use: '%s' (of %zu discovered)",
                       s.c_str(), g.suffixes.size());
            }
            r1 = in.substr(0, in.size() - s.size());
            out->browser = TrimCopy(StripEffectors(s));
            // Drop a leading separator run from the display form.
            // IsCharAlphaNumericW, not iswalnum: the CRT's is ASCII-only in the
            // "C" locale and treats a localized product name as separator.
            size_t k = 0;
            while (k < out->browser.size() &&
                   !IsCharAlphaNumericW(out->browser[k])) {
                ++k;
            }
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
            // Private]", so the stored tail is " - [InPrivate]" - and assigning
            // that whole tail to {profile} puts a separator inside the field,
            // which the template then doubles.
            //
            // Do NOT substitute the browser-suffix branch's strip-leading-non-
            // alphanumeric loop: '[' is not alphanumeric, so it eats the bracket
            // and yields "InPrivate]".
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
    // 4. page-count clause, then the generic profile. THE COUNT IS TRIED FIRST,
    // and the profile strip is NESTED in its failure rather than sequenced after
    // it. A flat count-then-profile sequence would strip a profile from behind a
    // count that already matched, taking the tail of the user's own title: with
    // a profile named "Bar", "Foo - Bar and 16 more pages - Microsoft Edge"
    // becomes title "Foo".
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
            // A run longer than the cap is not a count. Without this, the scan
            // keeps the last nine digits, leaves the rest in the title, and a
            // form whose `pre` is empty then accepts the result.
            if (digits == kMaxDigits && d > 0 && DigitValue(in[d - 1]) >= 0) {
                continue;
            }
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
        // MATCH A REAL PROFILE NAME, never just a trailing segment. Position is
        // not evidence: "strip whatever follows the last separator" turns
        // "GitHub - Some Repo - Microsoft Edge" into title "GitHub" with profile
        // "Some Repo" - silent truncation of the user's own text.
        //
        // ONE profile is enough. The evidence is the exact match, and how many
        // OTHER names exist does not make any single match stronger - it only
        // adds strings that can match at all, so a longer list is more
        // false-positive surface, not less. Current Edge shows "Personal" on a
        // single-profile install, and the whole count clause sits behind that
        // segment, so a two-profile floor would cost both.
        //
        // The count test is redundant by construction and kept anyway: a name is
        // recorded only inside a profile object, and entering one is what
        // increments the count. That invariant lives in another function, and
        // the all-or-nothing bail that upholds it reads like over-caution; this
        // conjunct is what keeps a gate that DELETES text from a title closed if
        // someone ever relaxes it to return what it found so far.
        // slot2Seps NON-EMPTY is what says "this browser puts a profile in its
        // titles at all". Chrome discovers no marker resource and so no
        // separator, and a Chrome title never carries a profile - so every match
        // this loop could make there would be a false positive with no true
        // positive to trade against.
        if (out->profile.empty() && g.profileCount >= 1 &&
            !g.profileNames.empty() && !g.slot2Seps.empty()) {
            // ANCHOR ON THE NAME, not on a discovered separator. Measured on
            // Finnish Edge: the privacy-marker resource yields an en dash, so
            // slot2Seps is {" - "} with an EN dash, while the browser joins the
            // profile with a plain hyphen and the suffix with the en dash in the
            // same title - 13 of 84 shipped locale packs disagree this way.
            // Looking for the separator first loses the profile on all of them,
            // and the count sitting behind it with the profile.
            const std::wstring t = TrimCopy(r2);
            for (const std::wstring& name : g.profileNames) {
                // EXACTLY equal, not case-insensitively. The browser renders the
                // stored name verbatim, so folding case buys nothing real and
                // only widens the false-positive window - with a profile named
                // "Work", a case-insensitive test truncates
                // "How to go on vacation - work - Microsoft Edge".
                if (name.empty() || name.size() >= t.size()) continue;
                if (!EndsWith(t, name)) continue;
                const size_t at = t.size() - name.size();
                const std::wstring_view head(t.data(), at);

                // 1. A separator the browser itself declared. Matched whole, so
                //    it can never consume a character the title owns.
                size_t s = std::wstring::npos;
                for (const std::wstring& sep : g.slot2Seps) {
                    if (sep.size() < at && EndsWith(head, sep)) {
                        s = at - sep.size();
                        break;
                    }
                }

                // 2. Failing that, a space then exactly one dash then optional
                //    space - the shape the marker resource cannot describe,
                //    because it is the one those 13 packs join with instead.
                //
                //    The LEADING space carries the whole distinction: without it
                //    "Remote-Work" is a profile named "Work", and "Non-Personal"
                //    a profile named "Personal".
                if (s == std::wstring::npos) {
                    size_t k = at, dashes = 0, spaces = 0;
                    while (k > 0 && at - k < 4) {
                        const wchar_t c = t[k - 1];
                        if (IsJoinerSpace(c))     ++spaces;
                        else if (IsJoinerDash(c)) ++dashes;
                        else break;
                        --k;
                    }
                    if (dashes != 1 || spaces == 0 || k == 0) continue;
                    if (!IsJoinerSpace(t[k])) continue;
                    s = k;
                }
                if (s == 0) continue;  // nothing would be left as a title
                out->profile = name;
                r3 = t.substr(0, s);
                break;
            }
        }

        // NO NAME LIST AT ALL - a UserDataDir group policy, a portable or
        // repacked install, an unparseable Local State. The gate above cannot
        // run, and without this the count behind the segment is unreachable too,
        // so every count-bearing preset collapses to "the title minus the
        // browser name".
        //
        // Strip speculatively then, and accept ONLY where a count form matches
        // behind the segment. That is weaker evidence than an exact name match
        // and it is knowingly weaker: the count clause is ordinary prose, so a
        // title-echoing site reproduces it -
        // "E-Mail and 16 more pages - Google Search" parses here as a 17-tab
        // "E-Mail" in a profile called "Google Search". The trade is a wrong
        // {profile} and count on such a title against no count at all on every
        // title, and it is taken ONLY where nothing better is available.
        //
        // Which is why the condition is "the list is missing", NOT "the match
        // failed". Running it whenever a name did not match would apply that
        // trade to installs whose names ARE readable - where the same title is
        // correctly left alone - and regress them for no gain.
        //
        // The segment goes into {profile}, not away, so nothing is silently
        // lost. No flag separates the two branches: the gate above stores a name
        // it matched, and a stored name is never empty.
        //
        // OFF BY DEFAULT, because it is the one path that can move a page's own
        // text without a name to justify it, and the population it fires on is
        // exactly the one that cannot check the result. The README's promise -
        // what the mod cannot account for is left byte-identical - is therefore
        // true unless the user has explicitly traded it away.
        if (guessProfile && out->profile.empty() && g.profileNames.empty()) {
            for (const std::wstring& sep : g.slot2Seps) {
                const size_t at = r2.rfind(sep);
                if (at == std::wstring::npos || at == 0) continue;
                const std::wstring cand = TrimCopy(r2.substr(at + sep.size()));
                if (cand.empty() || cand.size() > 64) continue;
                Fields       probe = *out;
                std::wstring rest;
                if (!tryStripCount(r2.substr(0, at), &probe, &rest)) continue;
                *out         = probe;
                out->profile = cand;
                r3           = rest;
                break;
            }
        }

        if (TrimCopy(r3).empty()) return false;
        std::wstring r4 = r3;
        if (!out->hasCount && tryStripCount(r3, out, &r4)) r3 = r4;
    }

    out->title = TrimCopy(r3);
    return !out->title.empty();
}

// ---- template rendering -----------------------------------------------------

// Bumped by LoadSettings. The template diagnostics below run on the hook path,
// once per title write per window, so a standing typo would repeat its message
// for the life of the process and bury the lines worth reading. Reporting each
// one once per template - not once per load - is what a typo needs: the user
// fixes it, gets it wrong again, and is told again.
volatile LONG g_templateGeneration = 0;

// True the first time it is called for the current template at this call site.
// A race can let two threads both report; a duplicated diagnostic line is a
// price worth paying over a lock on the title-write path.
bool FirstForThisTemplate(volatile LONG* seen) {
    const LONG gen = InterlockedCompareExchange(&g_templateGeneration, 0, 0);
    if (*seen == gen) return false;
    *seen = gen;
    return true;
}

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
    } else if (name == L"extra") {
        if (f.hasCount && f.extra > 0) { numeric = true; num = f.extra; }
    } else if (name == L"count") {
        if (f.hasCount) { numeric = true; num = f.extra + 1; }
    } else {
        // Resolves empty rather than printing itself, so a typo silently makes
        // the template render less. Logged, or the log has no answer to "my
        // template does nothing".
        static volatile LONG seen = -1;
        if (FirstForThisTemplate(&seen)) {
            Wh_Log(L"template: unknown token '%s'", std::wstring(name).c_str());
        }
        *empty = true;
        return {};
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

    // max<N> is collected here and applied LAST, whatever order it was written
    // in. Applying it in sequence does not bound anything: the styling alphabets
    // are non-BMP, so `{title:max20:bold}` clamps to twenty units and then
    // doubles them. The smallest N wins if several are given.
    size_t bound = 0;
    for (size_t i = 1; i < parts.size(); ++i) {
        const std::wstring_view m = parts[i];
        if (m == L"upper") {
            v = style::MapCase(v, /*upper=*/true);
        } else if (m == L"lower") {
            v = style::MapCase(v, /*upper=*/false);
        } else if (m == L"trim") {
            v = TrimCopy(v);
        } else if (m.rfind(L"max", 0) == 0 && m.size() > 3) {
            // Strictly digits, and the case this buys is a MIXED argument:
            // _wtoi stops at the first non-digit, so "max12abc" would silently
            // apply 12. ("maxbanana" and "max-1" reach the same no-bound outcome
            // either way - _wtoi yields 0, and -1 casts to a size_t no title can
            // exceed.) The rejection is logged, because a bound that quietly
            // does nothing is indistinguishable from one that worked.
            const std::wstring_view digits = m.substr(3);
            size_t n = 0;
            bool   good = true;
            for (const wchar_t c : digits) {
                if (c < L'0' || c > L'9') { good = false; break; }
                n = n * 10 + static_cast<size_t>(c - L'0');
                if (n > kMaxTitleChars) { good = false; break; }
            }
            if (good && n > 0) {
                if (bound == 0 || n < bound) bound = n;
            } else {
                static volatile LONG seen = -1;
                if (FirstForThisTemplate(&seen)) {
                    Wh_Log(L"template: ignoring '%s' - max<N> takes digits "
                           L"only, 1 to %zu",
                           std::wstring(m).c_str(), kMaxTitleChars);
                }
            }
        } else if (const style::Kind k = style::FromName(m);
                   k != style::Kind::kNone) {
            v = style::Apply(v, k);
        } else if (m == L"min2" || m == L"pad2" || m == L"pad3") {
            // Consumed by the numeric pass above - but only for a numeric token.
            // On a text one they are silently inert, which is the same "my
            // template does nothing" the unknown-modifier line exists to answer,
            // so say so rather than letting {title:pad3} fail quietly while the
            // neighbouring typo {title:padd3} is reported.
            if (!numeric) {
                static volatile LONG seen = -1;
                if (FirstForThisTemplate(&seen)) {
                    Wh_Log(L"template: '%s' only applies to {count} and "
                           L"{extra}; ignored here",
                           std::wstring(m).c_str());
                }
            }
        } else {
            // Anything else reaching here is a typo, and skipping it silently is
            // the other half of "my template does nothing".
            static volatile LONG seen = -1;
            if (FirstForThisTemplate(&seen)) {
                Wh_Log(L"template: unknown modifier '%s'",
                       std::wstring(m).c_str());
            }
        }
    }
    if (bound) v = style::Clamp(std::move(v), bound);
    *empty = v.empty();
    return v;
}

// Renders tpl[i..], stopping at ')' when inGroup. Reports whether any token
// inside resolved to something non-empty, which is what drives ?( ) groups.
//
// The depth cap is load-bearing: each `?(` recurses with a std::wstring in the
// frame, and the template is a settings field, so a pasted template of a few
// thousand nested `?(?(?(...` exhausts the browser UI thread's stack.
//
// Past the cap the `?(` stops being a group opener: its two characters go to
// the enclosing frame, and the group's `)` then closes THAT frame, so the
// parentheses shift by one. A capped group whose body resolves to nothing is
// dropped like any other empty group and leaves a stray `)` behind - which is
// enough for the composed result to be non-empty, so a template that is
// nothing but capped groups writes `)` as the title instead of leaving it
// alone. Reachable only by pasting 32 nested `?(`, and noted rather than
// fixed because balancing it changes the parser.
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
    std::wstring chromeOverride;
    std::wstring suffixOverride;
    bool         guessProfile = false;
};

struct WindowState {
    std::wstring source;   // last title the browser composed, pre-transform
    std::wstring applied;  // last title we wrote; the echo detector
    DWORD        tid = 0;  // for HWND-recycling detection
    // Bumped on every reset and every new source. A composition runs without the
    // lock held, so the commit has to prove it is still committing to the state
    // it read - `tid` cannot do that, since every browser frame shares one UI
    // thread and a recycled HWND keeps the same one.
    unsigned     generation = 0;
};

Settings g_settings;

// LoadSettings reassigns the settings strings on whatever thread
// Wh_ModSettingsChanged runs on, while ComposeFor reads them on a browser UI
// thread mid-title-write. std::wstring::operator= frees the old buffer, so a
// settings string must be COPIED under this lock and never held by reference
// across Render - a reference is a use-after-free inside the browser.
//
// Separate from g_lock, which guards the per-window map: merging them would put
// every settings read behind that map on every title write.
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

    // Our own process only. The sweep and the restore loop already required
    // this; folding it in here makes all three paths agree, so the hook cannot
    // cache or rewrite a window belonging to someone else.
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    WCHAR cls[40];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (wcsncmp(cls, L"Chrome_WidgetWin_", 17) != 0) return false;

    // WS_CAPTION is deliberately NOT required. Chromium's fullscreen handler
    // re-applies the frame style as `saved & ~(WS_CAPTION | WS_THICKFRAME)`, so
    // requiring it rejects the real browser frame for as long as it is
    // fullscreen. WS_SYSMENU and WS_MINIMIZEBOX survive fullscreen and still
    // carry the filter, which is why only that one bit is dropped rather than
    // falling back on class and ownership alone.
    const LONG_PTR st = GetWindowLongPtrW(hWnd, GWL_STYLE);
    constexpr LONG_PTR need = WS_SYSMENU | WS_MINIMIZEBOX;
    if ((st & need) != need) return false;

    if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
    return true;
}



std::wstring ComposeFor(const std::wstring& source) {
    if (!InterlockedCompareExchange(&g_ready, 0, 0)) {
        return source;
    }
    // ONE acquisition for both: they are two halves of the same settings
    // generation, and a Save landing between separate reads pairs the parse of
    // the OLD settings with the template of the NEW - a pair that can cut text
    // out of a title neither generation on its own would have touched.
    //
    // A COPY, not a reference: carrying a reference into Render is the
    // use-after-free described at g_settingsLock.
    bool         guessProfile;
    std::wstring tpl;
    {
        AcquireSRWLockShared(&g_settingsLock);
        guessProfile = g_settings.guessProfile;
        tpl          = (g_isChrome && !g_settings.chromeOverride.empty())
                           ? g_settings.chromeOverride
                           : g_settings.normal;
        ReleaseSRWLockShared(&g_settingsLock);
    }

    Fields f;
    if (!Decompose(source, g_grammar, &f, guessProfile)) {
        // Not a title this mod recognises: a window the user has named, a PWA, a
        // dialog, picture-in-picture. From the string alone those are
        // indistinguishable, and telling them apart needs the browser's own
        // objects rather than its text - so the title is left exactly as it is.
        //
        // This is the hard invariant the readme states, and it is what keeps
        // every one of those window kinds safe by construction.
        Wh_Log(L"left as-is (does not match the discovered grammar): %s",
               source.c_str());
        return source;
    }

    if (tpl.empty()) {
        return source;
    }

    std::wstring out;
    bool         any = false;
    Render(tpl, 0, /*inGroup=*/false, f, &out, &any);
    out = TrimCopy(out);
    if (out.empty()) return source;  // never blank a title
    out = style::Clamp(std::move(out), kMaxTitleChars);

    Wh_Log(L"title='%s' count=%d profile='%s' -> '%s'", f.title.c_str(),
           f.hasCount ? f.extra + 1 : 0, f.profile.c_str(), out.c_str());
    return out;
}

// Every write that bypasses the hook must record `applied` itself, or the next
// title the browser composes is mistaken for our own echo.
//
// The timeout is short because a sweep can cover ninety windows on the worker
// thread teardown has to join: a per-window second would stall an uninstall.
// `answered`, when given, reports whether the THREAD replied at all, which is a
// different fact from whether the write took: a window procedure that handles
// WM_SETTEXT and returns FALSE has answered. Only silence is evidence about the
// thread, and only that may mute its remaining windows.
bool WriteTitleFromOtherThread(HWND hWnd, const std::wstring& text,
                               bool* answered = nullptr) {
    // BOTH results matter. The return value says the message was delivered;
    // `result` is what the window procedure returned, and WM_SETTEXT reports
    // TRUE only when the text was set. Counting delivery alone is how a
    // "restored" tally claims a window that rejected the write.
    DWORD_PTR result = 0;
    const LRESULT ok = SendMessageTimeoutW(
        hWnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text.c_str()),
        SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT, 250, &result);
    if (answered) *answered = (ok != 0);
    return ok != 0 && result != 0;
}

// The reading half, bounded for the same reason. GetWindowTextW and
// GetWindowTextLengthW look like local calls and are not: for a window owned by
// THIS process but another thread they are SendMessage(WM_GETTEXT), which has no
// timeout and returns only when the owning thread pumps - fatal on the worker
// teardown joins with INFINITE.
//
// Two sends rather than one fixed buffer: a truncated read loses the browser
// suffix, so Decompose refuses it and the window is never retitled.
//
// False means the TITLE is unusable - a length too large to be one, or a
// handler that overran its buffer, both fail here having ANSWERED. `answered`
// is the separate fact about the thread, as WriteTitleFromOtherThread reports
// it, and only it may mute a thread's remaining windows.
bool ReadTitleFromOtherThread(HWND hWnd, std::wstring* out,
                              bool* answered = nullptr) {
    out->clear();
    if (answered) *answered = false;
    DWORD_PTR len = 0;
    if (!SendMessageTimeoutW(hWnd, WM_GETTEXTLENGTH, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &len)) {
        return false;
    }
    if (answered) *answered = true;
    if (len == 0) return true;
    if (len > 0x10000) return false;  // too large to be a real title

    // WM_GETTEXT's wParam counts the terminator; its result does not.
    std::wstring buf(static_cast<size_t>(len) + 1, L'\0');
    DWORD_PTR copied = 0;
    if (!SendMessageTimeoutW(hWnd, WM_GETTEXT, static_cast<WPARAM>(buf.size()),
                             reinterpret_cast<LPARAM>(buf.data()),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &copied)) {
        // It answered the first send and not the second, so the thread stopped
        // answering DURING this read - which is evidence about the thread.
        if (answered) *answered = false;
        return false;
    }
    // WM_GETTEXTLENGTH is allowed to overestimate, so the copied count is the
    // authoritative one; a conforming handler never exceeds capacity - 1.
    if (copied >= buf.size()) return false;
    buf.resize(static_cast<size_t>(copied));
    *out = std::move(buf);
    return true;
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

// Sleep in interruptible slices when there is no event to wait on.
//
// Teardown joins this thread with an INFINITE wait, so an uninterruptible sleep
// here is an unload that hangs for its full duration - a minute, at the
// maintenance period. The event makes that immediate when it exists; the slices
// bound it to one tick when it does not.
void SleepInSlices(DWORD ms) {
    constexpr DWORD kSlice = 200;
    while (ms) {
        const DWORD slice = (ms < kSlice) ? ms : kSlice;
        Sleep(slice);
        if (StopRequested()) return;
        ms -= slice;
    }
}

// Sleep, unless teardown starts first. Returns true if we should stop.
bool SleepOrStop(DWORD ms) {
    if (StopRequested()) return true;
    if (g_stopEvent) {
        const DWORD r = WaitForSingleObject(g_stopEvent, ms);
        if (r == WAIT_OBJECT_0) return true;
        // A broken handle must not spin, and must not become an unload that
        // waits out the whole period either.
        if (r == WAIT_FAILED) SleepInSlices(ms);
    } else {
        SleepInSlices(ms);
    }
    return StopRequested();
}

// ---- the hook ---------------------------------------------------------------

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (InterlockedCompareExchange(&g_passthrough, 0, 0) || t_inHook ||
        !lpString || !*lpString || !IsBrowserFrame(hWnd)) {
        return SetWindowTextW_Original(hWnd, lpString);
    }

    std::wstring out;
    bool         changed = false;
    {
        // Scoped so it ends BEFORE the single tail call below. The original has
        // always run with the flag clear, and it must keep doing so - a return
        // expression is evaluated before local destructors, so returning from
        // inside this scope would call it with the flag still set and change
        // which nested writes get composed. Scoping it here keeps the behaviour
        // identical and still resets the flag if the string work throws.
        struct Guard {
            Guard() { t_inHook = true; }
            ~Guard() { t_inHook = false; }
            Guard(const Guard&) = delete;
            Guard& operator=(const Guard&) = delete;
        } guard;

        unsigned gen   = 0;
        bool     fresh = false;
        {
            AcquireSRWLockExclusive(&g_lock);
            WindowState& st  = g_states[hWnd];
            const DWORD  tid = GetWindowThreadProcessId(hWnd, nullptr);
            if (st.tid && st.tid != tid) {
                // Reuse of this HWND by a window on a DIFFERENT thread - the
                // only reuse this test can see, since browser frames share one
                // UI thread and same-thread reuse keeps the id.
                const unsigned prevGen = st.generation;
                st = WindowState{};
                st.generation = prevGen + 1;
            }
            st.tid = tid;
            // `applied` matching means our own string coming back around; leave
            // `source` intact so a settings change still recomposes the original.
            if (st.applied != lpString) {
                st.source = lpString;
                gen       = ++st.generation;
                fresh     = true;
            }
            ReleaseSRWLockExclusive(&g_lock);
        }

        if (fresh) {
            // Composed with no lock held - holding one across work that does not
            // need it is how a hook arms a deadlock against itself. The
            // generation check is what makes that safe: it proves this result is
            // committed against the state it was computed from, which `tid`
            // cannot show, since every frame shares one UI thread.
            out     = ComposeFor(lpString);
            changed = (out != lpString);

            AcquireSRWLockExclusive(&g_lock);
            const auto it = g_states.find(hWnd);
            if (it == g_states.end() || it->second.generation != gen) {
                // Reset, pruned or overtaken while composing. The newer owner
                // will write its own title; pass the browser's string through.
                changed = false;
            } else {
                it->second.applied = out;
            }
            ReleaseSRWLockExclusive(&g_lock);
        }
    }

    return SetWindowTextW_Original(hWnd, changed ? out.c_str() : lpString);
}

// ---- discovery --------------------------------------------------------------

// The one seam the test harness replaces, and it exists ONLY there. The suite
// compiles this same translation unit with -DWH_TITLEFMT_TEST and points the
// override at a fixture directory; no Windhawk build defines that macro, so the
// shipped mod contains neither the global nor the branch.
//
// The macro is the harness's own on purpose. Windhawk's editing-mode define is
// undocumented, and a mod should not key any behaviour off it.
#ifdef WH_TITLEFMT_TEST
std::wstring g_localAppDataOverride;
#endif

std::wstring LocalAppDataDir() {
#ifdef WH_TITLEFMT_TEST
    if (!g_localAppDataOverride.empty()) return g_localAppDataOverride;
#endif
    PWSTR        p = nullptr;
    std::wstring out;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &p)) &&
        p) {
        out = p;
    }
    if (p) CoTaskMemFree(p);
    return out;
}

// GetModuleFileNameW into a buffer that grows until it fits. MAX_PATH is a
// guess, not a limit, and this API TRUNCATES rather than failing - the call
// still "succeeds". Callers read the result structurally, so a truncated path
// silently costs the Locales directory or the executable's basename, which is
// how Chrome ends up treated as Edge.
std::wstring ModulePath(HMODULE m) {
    std::wstring s(MAX_PATH, L'\0');
    for (;;) {
        const DWORD n = GetModuleFileNameW(m, s.data(),
                                           static_cast<DWORD>(s.size()));
        if (n == 0) return {};
        if (n < s.size()) { s.resize(n); return s; }
        if (s.size() > 32768) return {};  // past any real Windows path
        s.resize(s.size() * 2);
    }
}

std::wstring DirOfModule(const wchar_t* name) {
    HMODULE m = GetModuleHandleW(name);
    if (!m) return {};
    const std::wstring s = ModulePath(m);
    const size_t at = s.rfind(L'\\');
    return (at == std::wstring::npos) ? std::wstring() : s.substr(0, at);
}

// The UserDataDir group policy. Chromium gives it precedence over
// --user-data-dir, so this is read first, and HKLM before HKCU exactly as the
// browser resolves it. Read-only: the mod never writes a policy key.
//
// Reading this is what lets a policy-managed machine match a real profile name
// instead of falling back to guessing at a trailing segment.
//
// `unresolved` is the important half. A policy that EXISTS but cannot be turned
// into a path here is not the same as no policy: falling back would read the
// default directory, which is a Local State the browser is NOT using, and its
// profile names would then be matched against - and cut out of - real titles.
// So that case is reported separately and the caller fails closed.
std::wstring PolicyUserDataDir(bool* unresolved) {
    *unresolved = false;
    const wchar_t* const key = g_isChrome
                                   ? L"SOFTWARE\\Policies\\Google\\Chrome"
                                   : L"SOFTWARE\\Policies\\Microsoft\\Edge";
    for (const HKEY root : {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
        HKEY h = nullptr;
        if (RegOpenKeyExW(root, key, 0, KEY_QUERY_VALUE, &h) != ERROR_SUCCESS) {
            continue;
        }
        wchar_t buf[1024];
        DWORD   cb = sizeof buf, type = 0;
        const LSTATUS st =
            RegQueryValueExW(h, L"UserDataDir", nullptr, &type,
                             reinterpret_cast<LPBYTE>(buf), &cb);
        RegCloseKey(h);
        if (st == ERROR_FILE_NOT_FOUND) continue;  // no policy at this root
        if (st != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ)) {
            // Present and unreadable - too long for the buffer, or not a string.
            *unresolved = true;
            return {};
        }
        // RegQueryValueExW does not promise a terminator. The division rounds
        // down, so an odd byte count cannot produce a partial character.
        std::wstring v(buf, cb / sizeof(wchar_t));
        const size_t nul = v.find(L'\0');
        if (nul != std::wstring::npos) v.resize(nul);

        // TERMINAL, not a miss. The browser reads HKLM and stops; an empty value
        // there means it uses the default, and so must this.
        if (v.empty()) return {};

        // Chromium strips one matching quote pair before expanding, and a GPO
        // written from a .reg file routinely carries them. Without this the
        // quotes reach a file path and every read fails.
        if (v.size() >= 2 && ((v.front() == L'"' && v.back() == L'"') ||
                              (v.front() == L'\'' && v.back() == L'\''))) {
            v = v.substr(1, v.size() - 2);
        }

        if (type == REG_EXPAND_SZ) {
            wchar_t     ex[1024];
            const DWORD n = ExpandEnvironmentStringsW(v.c_str(), ex, 1024);
            if (n == 0 || n > 1024) {
                *unresolved = true;  // needs more room than we gave it
                return {};
            }
            v.assign(ex, n - 1);
            if (v.empty()) return {};
        }
        // Chromium also expands its own ${...} variables here - ${local_app_data}
        // and nine others. This mod does not, and a path still holding one names
        // a directory it cannot find.
        if (v.find(L"${") != std::wstring::npos) {
            *unresolved = true;
            return {};
        }
        return v;
    }
    return {};
}

std::wstring UserDataDir() {
    bool               policyUnresolved = false;
    const std::wstring fromPolicy       = PolicyUserDataDir(&policyUnresolved);
    if (policyUnresolved) {
        // A policy is in force and points somewhere this mod cannot follow.
        // Every fallback below would read a directory the browser is not using,
        // so report unknown and let the callers skip their slots.
        Wh_Log(L"a UserDataDir policy is set but could not be resolved; "
               L"profile names will not be read");
        return {};
    }
    if (!fromPolicy.empty()) return fromPolicy;

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

    const std::wstring laStr = LocalAppDataDir();
    if (laStr.empty()) return {};
    const wchar_t* la = laStr.c_str();

    // The CHANNEL, from the running executable's own path. The default user-data
    // directory is one per channel - "Edge Beta", "Edge Dev", "Edge SxS" and the
    // Chrome equivalents - and all of them run as msedge.exe / chrome.exe, so
    // the process name cannot tell them apart.
    //
    // The install path encodes it as "<Vendor>\<Product>\Application\<exe>", and
    // the same pair names the user-data directory under LOCALAPPDATA: it is the
    // same install-mode constant on both sides. Only the two names are used -
    // the install itself can live anywhere.
    const std::wstring exe = ModulePath(nullptr);
    const size_t appAt = exe.rfind(L"\\Application\\");
    if (appAt != std::wstring::npos) {
        const std::wstring head = exe.substr(0, appAt);   // ...\Vendor\Product
        const size_t prodAt = head.rfind(L'\\');
        if (prodAt != std::wstring::npos && prodAt > 0) {
            const std::wstring product = head.substr(prodAt + 1);
            const size_t vendAt = head.rfind(L'\\', prodAt - 1);
            if (vendAt != std::wstring::npos) {
                const std::wstring vendor =
                    head.substr(vendAt + 1, prodAt - vendAt - 1);
                // Validated against the names the browser actually uses, so an
                // unexpected layout falls through to the stable default rather
                // than inventing a directory.
                const bool sane =
                    g_isChrome ? (vendor == L"Google" &&
                                  product.rfind(L"Chrome", 0) == 0)
                               : (vendor == L"Microsoft" &&
                                  product.rfind(L"Edge", 0) == 0);
                if (sane) {
                    return std::wstring(la) + L"\\" + vendor + L"\\" + product +
                           L"\\User Data";
                }
            }
        }
    }

    // Stable, and the fallback for anything unrecognised - a portable repack, a
    // dev build run straight out of its output directory, or the test harness,
    // none of which have an \Application\ component to read.
    return std::wstring(la) +
           (g_isChrome ? L"\\Google\\Chrome\\User Data"
                       : L"\\Microsoft\\Edge\\User Data");
}

// The display names of the profiles this install has, from the browser's own
// Local State.
//
// A narrow scan rather than a JSON parser, but syntax-aware: a key is recognised
// only by the ':' that follows it AND a value opening with a quote. Position
// alone lets a field whose VALUE is "name" be taken as a key, and a non-string
// value leaves the key armed so the NEXT key is recorded as a name. A wrong name
// here decides whether part of the user's title is discarded.
//
// The COUNT is collected in the same pass and is a different quantity: profile
// objects are the depth 1 -> 2 transitions, while the name list is several keys
// per profile flattened together. It is what the discovery log reports beside
// the name total, which is how a one-profile install is told from an unreadable
// one.
//
// Malformed input returns nothing rather than a partial answer; empty means
// "unknown", which the caller treats as "do not strip a profile".
struct ProfileInfo {
    std::vector<std::wstring> names;
    int                       count = 0;
};

ProfileInfo DiscoverProfiles() {
    ProfileInfo info;
    std::vector<std::wstring>& names = info.names;
    const std::wstring root = UserDataDir();
    if (root.empty()) return info;

    const std::vector<uint8_t> buf = ReadWholeFile(root + L"\\Local State");
    if (buf.empty()) return info;

    const std::string_view sv(reinterpret_cast<const char*>(buf.data()),
                              buf.size());
    size_t at = sv.find("\"info_cache\"");
    if (at == std::string_view::npos) return info;
    at = sv.find('{', at);
    if (at == std::string_view::npos) return info;

    // The two LOCAL name keys, deliberately not the GAIA ones. This list decides
    // what may be deleted from a title, so it holds only what has been observed
    // in one: Edge renders `shortcut_name`, including on signed-in profiles, so
    // gaia_name buys nothing - and gaia_given_name is a bare first name, the
    // entry most likely to appear innocently at the end of a page title.
    auto wanted = [](std::string_view k) {
        return k == "name" || k == "shortcut_name";
    };

    int    depth  = 0;
    bool   closed = false;
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
                    // The VALUE must then be a string too, and checking that is
                    // not pedantry. Without it, "name": null followed by
                    // "shortcut_name": "Work" left `key` armed across the null,
                    // so the next string token seen - the literal KEY
                    // "shortcut_name" - was recorded as a profile name, and the
                    // real "Work" was then skipped because `key` had just been
                    // cleared. That both invents a name and loses a true one, in
                    // the list that decides what may be cut from a title.
                    if (j < sv.size() && sv[j] == ':') {
                        ++j;
                        while (j < sv.size() && (sv[j] == ' ' || sv[j] == '\t' ||
                                                 sv[j] == '\r' || sv[j] == '\n')) {
                            ++j;
                        }
                        if (j < sv.size() && sv[j] == '"') key.assign(tok);
                    }
                }
            }
            continue;
        }
        if (c == '"')  { inStr = true; strAt = i + 1; continue; }
        // Depth 1 is info_cache itself - the loop starts on its opening brace -
        // so each 1 -> 2 transition is one profile object. Dictionaries nested
        // inside a profile go to 3 and are not counted, and a brace inside a
        // string never reaches here because inStr is tested first.
        if (c == '{')  { if (++depth == 2) ++info.count; continue; }
        if (c == '}')  { if (--depth == 0) { closed = true; break; } key.clear(); continue; }
    }

    // ALL OR NOTHING. Returning what was found so far is wrong for this
    // particular result: a truncated or malformed Local State - a torn write, a
    // half-flushed file, an unterminated string - would otherwise yield a short
    // name list and a low profile count, and BOTH of those feed the gate that
    // decides whether text is deleted from a user's title. A partial count is
    // the worse half: fewer profiles than the install really has silently turns
    // the profile slot off, and more would turn it on. Unknown is a state this
    // parser already handles correctly, so say unknown.
    if (!closed || inStr) return {};
    return info;
}

bool SameLocale(std::wstring_view a, std::wstring_view b) {
    return a.size() == b.size() && _wcsnicmp(a.data(), b.data(), a.size()) == 0;
}

// One locale tag -> every pak name that could serve it, most specific first.
//
// Windows hands out full tags; the Locales folder is mostly BARE language codes,
// so "cs-CZ" alone finds nothing and discovery falls through to en-US - which
// DOES yield a suffix, so it reports success while matching no real title.
//
// Three rules, and none of them is "try the language part":
//
//   * Bare `zh`, `pt` and `en` DO NOT EXIST as packs. Chinese is reachable only
//     as zh-CN / zh-TW, so a parent chain alone strands it on English - hence
//     the aliases.
//   * Parents drop subtags from the END. Truncating to the last subtag corrupts
//     script and variant tags: sr-Latn-BA is not sr-BA, ca-ES-valencia is not
//     ca-valencia.
//   * es-MX must reach es-419 BEFORE bare es, which is Spain's. Both packs
//     exist, so getting this wrong matches the WRONG one rather than failing.
void ExpandLocale(std::wstring_view tag, std::vector<std::wstring>* out) {
    auto add = [out](std::wstring v) {
        if (v.empty()) return;
        for (const std::wstring& e : *out) {
            if (SameLocale(e, v)) return;
        }
        out->push_back(std::move(v));
    };
    if (tag.empty() || tag.size() > 64) return;

    add(std::wstring(tag));

    // Language and region, for the alias rules. Subtags are ASCII by definition.
    const size_t dash = tag.find(L'-');
    const std::wstring_view lang = tag.substr(0, dash);
    std::wstring_view last;
    if (dash != std::wstring_view::npos) {
        const size_t at = tag.rfind(L'-');
        last = tag.substr(at + 1);
    }

    // Chinese is script-based and ships only as two regional packs.
    if (SameLocale(lang, L"zh")) {
        const bool hant = tag.find(L"Hant") != std::wstring_view::npos ||
                          SameLocale(last, L"TW") || SameLocale(last, L"HK") ||
                          SameLocale(last, L"MO");
        add(hant ? L"zh-TW" : L"zh-CN");
    }
    // Spanish outside Spain is served by the Latin-American pack, and bare `es`
    // is Spain's - so it must not be reached first.
    if (SameLocale(lang, L"es") && !last.empty() && !SameLocale(last, L"ES")) {
        add(L"es-419");
    }
    // Portuguese ships only as the two regional packs; there is no bare `pt`.
    if (SameLocale(lang, L"pt") && !SameLocale(last, L"BR") &&
        !SameLocale(last, L"PT")) {
        add(L"pt-PT");
    }

    // Then the proper parents, dropping one subtag at a time from the end:
    // zh-Hans-CN -> zh-Hans -> zh, ca-ES-valencia -> ca-ES -> ca.
    std::wstring parent(tag);
    for (;;) {
        const size_t at = parent.rfind(L'-');
        if (at == std::wstring::npos) break;
        parent.resize(at);
        add(parent);
    }
}

// Locale candidates, best first. A wrong guess is safe only in the sense that a
// missing pak is skipped; a wrong pak that EXISTS is accepted, which is why the
// expansion above is ordered rather than merely generous.
std::vector<std::wstring> LocaleCandidates() {
    std::vector<std::wstring> out;
    auto add = [&out](std::wstring_view v) { ExpandLocale(v, &out); };

    // LAST --lang wins, which is what the browser does with a repeated switch -
    // and what UserDataDir already does with a repeated --user-data-dir. Adding
    // each occurrence in turn made the FIRST one win here, so the two functions
    // disagreed about the same command line.
    int          argc = 0;
    LPWSTR*      argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::wstring fromLang;
    if (argv) {
        for (int i = 1; i < argc; ++i) {
            if (_wcsnicmp(argv[i], L"--lang=", 7) == 0) fromLang = argv[i] + 7;
        }
        LocalFree(argv);
    }
    if (!fromLang.empty()) add(fromLang);

    // The browser's own UI language, which need not match the OS.
    //
    // Through UserDataDir(), so --user-data-dir is honoured here exactly as it is
    // for the profile names. Rebuilding the per-channel default by hand read the
    // wrong install's app_locale, and the resulting failure was quiet rather than
    // loud: discovery stops at the first candidate whose .pak yields a suffix, so
    // a wrong-language pak is accepted, the log prints a healthy-looking line,
    // and then no real title ever matches what was discovered.
    if (const std::wstring root = UserDataDir(); !root.empty()) {
        const std::vector<uint8_t> buf = ReadWholeFile(root + L"\\Local State");
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

    // The preferred UI LANGUAGES, in the user's own order - not
    // GetUserDefaultLocaleName, which is the regional-format locale and answers
    // a different question. Someone running an English Windows with Czech dates
    // has cs-CZ as their format locale and en-US as their UI language, and it is
    // the UI language the browser follows.
    ULONG  num = 0, chars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num, nullptr, &chars) &&
        chars > 0 && chars < 4096) {
        std::wstring buf(chars, L'\0');
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num, buf.data(),
                                        &chars)) {
            // Double-null-terminated list of null-separated names.
            for (size_t at = 0; at < buf.size() && buf[at];) {
                const std::wstring_view one(buf.data() + at);
                add(one);
                at += one.size() + 1;
            }
        }
    }

    // Still consulted, after the UI languages: on a machine with no UI language
    // pack installed the two agree, and it costs nothing when they do not.
    WCHAR loc[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(loc, ARRAYSIZE(loc))) add(loc);

    // Last, and deliberately unexpanded - it is the pack that always exists.
    if (std::find(out.begin(), out.end(), L"en-US") == out.end()) {
        out.push_back(L"en-US");
    }
    return out;
}



// Drop state for windows that no longer exist, which bounds the map: an entry is
// created per frame swept and per title written.
//
// NOT an identity check, and nothing here is - the hook's tid comparison catches
// only reuse on a DIFFERENT thread, and browser frames share one UI thread. What
// limits the damage is that a reused HWND's first title write does not match the
// remembered `applied`, so the entry is overwritten rather than trusted.
void PruneDeadWindows() {
    // Dead entries are MOVED out rather than counted so their strings are freed
    // after g_lock is released, not during erase() while a browser UI thread may
    // be waiting on it. A counter would put every deallocation back under the
    // exclusive section.
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

    if (!dead.empty()) {
        Wh_Log(L"pruned %zu dead window(s)", dead.size());
    }
}

// A named __stdcall callback rather than a lambda, and that is an architecture
// requirement rather than a style choice. A captureless lambda converts to a
// function pointer of the DEFAULT calling convention; on x86-64 and ARM64 there
// is only one convention so it converts to WNDENUMPROC fine, but on 32-bit x86
// the default is __cdecl and WNDENUMPROC is __stdcall, and the lambda form
// simply does not compile. CALLBACK is __stdcall there and nothing everywhere
// else, so this builds identically on all three.
BOOL CALLBACK CollectBrowserFrames(HWND h, LPARAM lp) {
    if (IsBrowserFrame(h)) {
        reinterpret_cast<std::vector<HWND>*>(lp)->push_back(h);
    }
    return TRUE;
}

void SweepAllWindows() {
    PruneDeadWindows();

    std::vector<HWND> frames;
    EnumWindows(CollectBrowserFrames, reinterpret_cast<LPARAM>(&frames));

    // Threads that already failed to answer within this sweep.
    //
    // The per-window timeout below bounds one send, not the whole sweep, and
    // every browser frame shares a UI thread - so on a session of a hundred
    // windows whose thread is wedged, a bounded sweep still costs a hundred
    // timeouts back to back. Wh_ModSettingsChanged runs this on the Windhawk
    // engine thread, which would mean a settings change taking half a minute.
    // One failure per thread is all the evidence needed to skip the rest.
    std::vector<DWORD> mute;

    int changed = 0;
    int muted   = 0;
    for (HWND h : frames) {
        // Teardown must be able to cut a sweep short. Without this the worker
        // can still be walking dozens of windows when Wh_ModBeforeUninit tries
        // to join it, and the DLL would then be unloaded out from under a
        // thread that is still executing this function.
        if (StopRequested()) {
            Wh_Log(L"sweep aborted: shutting down");
            return;
        }
        // RE-CHECKED, not trusted from collection time. CollectBrowserFrames
        // filtered by process when it enumerated, but this loop can run for
        // several hundred milliseconds of blocking sends, and an HWND is unique
        // only while its window lives. A frame closed mid-sweep can have its
        // handle recycled by any process on the desktop, and the one kind of
        // stranger whose title would survive ComposeFor unchanged is another
        // browser's frame - a second --user-data-dir instance. The restore loop
        // has always guarded this; the sweep only checked at collection.
        if (!IsBrowserFrame(h)) continue;

        const DWORD owner = GetWindowThreadProcessId(h, nullptr);
        if (std::find(mute.begin(), mute.end(), owner) != mute.end()) {
            ++muted;
            continue;
        }
        // Bounded, and shared with the restore path - see the helper for why
        // these reads cannot be the plain GetWindowTextW form.
        std::wstring cur;
        bool         answered = false;
        if (!ReadTitleFromOtherThread(h, &cur, &answered)) {
            // Only silence mutes, and only from a window still ours to have
            // been silent - the restore loop's rule, for its reasons. The
            // IsBrowserFrame check above does NOT survive the send: a frame
            // closing during WM_GETTEXTLENGTH fails it for want of a window,
            // not because its thread went quiet. Not counted either - `muted`
            // is the log line's "skipped after a thread stopped answering".
            DWORD       pid2 = 0;
            const DWORD tid2 = GetWindowThreadProcessId(h, &pid2);
            if (!answered && pid2 == GetCurrentProcessId() && tid2 == owner) {
                mute.push_back(owner);
                ++muted;
            }
            continue;
        }
        if (cur.empty()) continue;

        // Capture the generation with the source, validate it on commit, or a
        // stale pair lands on top of state a concurrent title write just
        // refreshed - leaving `source` belonging to the PREVIOUS page, which the
        // next settings change would then recompose and display.
        //
        // This makes the STATE MAP coherent and nothing more. The final write is
        // still outside the lock, so a title write landing between commit and
        // send is overwritten and restored by that window's next write. Closing
        // it means recomposing on the owning UI thread - a different design.
        unsigned gen = 0;
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
            gen = (it != g_states.end()) ? it->second.generation : 0;
            ReleaseSRWLockShared(&g_lock);
        }
        const std::wstring out = ComposeFor(src);
        {
            AcquireSRWLockExclusive(&g_lock);
            WindowState& st = g_states[h];
            if (st.generation != gen) {
                // A title write overtook this window while we composed. Its
                // result is newer than ours; leave it alone.
                ReleaseSRWLockExclusive(&g_lock);
                continue;
            }
            st.source     = src;
            st.applied    = out;
            st.generation = gen + 1;
            st.tid        = owner;
            ReleaseSRWLockExclusive(&g_lock);
        }
        if (out != cur) {
            if (WriteTitleFromOtherThread(h, out)) {
                ++changed;
            } else {
                // THE MAP MUST RECORD WHAT THE WINDOW SHOWS, not what we meant
                // it to show. `applied` was committed above, before the send;
                // when the send does not land, the window still shows `cur`, and
                // leaving the map claiming otherwise makes the restore loop read
                // `cur != st.applied` and classify the window as "moved on" -
                // so a title we did write elsewhere would never be restored.
                //
                // Guarded by the generation this pass installed: if a real title
                // write has landed since, its state is newer and stays.
                AcquireSRWLockExclusive(&g_lock);
                const auto it2 = g_states.find(h);
                if (it2 != g_states.end() && it2->second.generation == gen + 1) {
                    it2->second.applied = cur;
                }
                ReleaseSRWLockExclusive(&g_lock);
            }
        }
    }
    // `muted` is the one that answers "did the one-strike rule cost anything
    // here". It prints unconditionally, because a line emitted only when the
    // rule fires is invisible to a capture attached afterwards - and this
    // number has never been observed to be anything but zero.
    Wh_Log(L"sweep: %zu frame(s), %d retitled, %d skipped after a thread "
           L"stopped answering",
           frames.size(), changed, muted);
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
    // Falls through to the maintenance loop rather than returning, for the same
    // reason the discovery-failure path does: the title hook records a state per
    // frame regardless, so an early return leaves nothing pruning that map for
    // the life of the process.
    bool haveBrowser = chromium != nullptr;
    if (!haveBrowser) {
        Wh_Log(L"%s never loaded; nothing to do in this process", browserDll);
    }

    // From the browser MODULE, with no fallback to the executable's directory.
    // Locales\ sits beside the DLL under Application\<version>\, while the
    // executable lives in Application\ - so that fallback could only ever look
    // in a directory with no .pak in it.
    std::wstring dir;
    if (haveBrowser) {
        dir = DirOfModule(browserDll);
    }
    const std::wstring hint = g_isChrome ? L"Chrome" : L"Edge";

    std::vector<std::wstring> locales;
    if (haveBrowser) locales = LocaleCandidates();

    Grammar g;
    bool    ok = false;
    for (const std::wstring& loc : locales) {
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
            // The install's real profiles, so the profile slot can require both
            // "this install shows a profile at all" and "this is one of its
            // names" rather than taking whatever follows a separator.
            {
                ProfileInfo pi = DiscoverProfiles();
                g.profileNames = std::move(pi.names);
                g.profileCount = pi.count;
            }
            Wh_Log(L"grammar from %s: %zu suffix, %zu marker, %zu sep, %zu count, "
                   L"%d profile(s) / %zu name(s)",
                   path.c_str(), g.suffixes.size(), g.markerTails.size(),
                   g.slot2Seps.size(), g.countForms.size(), g.profileCount,
                   g.profileNames.size());
            // The literals themselves, not just how many. Counts alone cannot
            // distinguish "found the right language" from "found English on a
            // German install" - both print a healthy-looking line - and that is
            // the one discovery failure that produces no other symptom.
            //
            // EVERY branch, in the shape it has: a "=N" branch keeps its whole
            // tail in `fixed` and has no pre/post at all, so printing one form's
            // pre and post reports a successful discovery as "'' # ''".
            for (const CountForm& cf : g.countForms) {
                if (cf.fixed.empty()) {
                    Wh_Log(L"  count:  '%s' <number> '%s'", cf.pre.c_str(),
                           cf.post.c_str());
                } else {
                    Wh_Log(L"  count:  =%d '%s'", cf.fixedValue,
                           cf.fixed.c_str());
                }
            }
            if (g.profileNames.empty()) {
                // BOTH STATES. This line is read on exactly the installs the
                // setting exists for, so one that describes only the default
                // tells half of them something untrue about their own titles.
                bool guess;
                {
                    AcquireSRWLockShared(&g_settingsLock);
                    guess = g_settings.guessProfile;
                    ReleaseSRWLockShared(&g_settingsLock);
                }
                if (guess) {
                    Wh_Log(L"could not read this install's profile names; "
                           L"guessing is ON, so a trailing segment becomes "
                           L"{profile} where a page-count form matches behind "
                           L"it - a page whose own title ends that way is "
                           L"mis-split");
                } else {
                    Wh_Log(L"could not read this install's profile names, so "
                           L"{profile} stays empty and any count behind it is "
                           L"left alone. Turn on 'Guess the profile when the "
                           L"profile list cannot be read' to recover the count "
                           L"on weaker evidence");
                }
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
    if (haveBrowser && !suffixOverride.empty()) {
        g.suffixes.insert(g.suffixes.begin(), suffixOverride);
        ok = true;
        Wh_Log(L"using suffix override");
    }
    if (!ok) {
        if (haveBrowser) {
            Wh_Log(L"DISCOVERY FAILED - no titles will be changed. Set the "
                   L"browser suffix override in settings if this persists.");
        }
        // NOT a return. Falling through to the maintenance loop below is the
        // whole point - see the comment on it.
    } else {
        // Published WITHOUT g_lock: readers reach the grammar through ComposeFor
        // and take no lock, so one here would imply a discipline they do not
        // share. Safety comes from g_ready - the grammar is written once before
        // it is set and never touched again, and the Interlocked pair on both
        // sides is a full fence, so observing g_ready == 1 means observing the
        // completed assignment.
        g_grammar = std::move(g);
        InterlockedExchange(&g_ready, 1);

        // Only now is a sweep meaningful. Doing it at AfterInit would retitle
        // nothing, and on a session with many open windows that reads as "the
        // mod does not work" until each window happens to change its own title.
        SweepAllWindows();
    }

    // ---- maintenance loop ---------------------------------------------------
    //
    // Bounds memory, and nothing else - everything the mod does is driven by the
    // browser writing a title. Reached even when discovery failed or the browser
    // module never appeared, deliberately: the hook records a WindowState per
    // frame regardless, so any early return here leaves that map unpruned for
    // the life of the process.
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
    // `min2` throughout. The count can only come from the title, and Chromium
    // omits it on a one-tab window, so the token is already empty there and the
    // modifier is belt and braces - but it is what keeps these templates correct
    // if a count of 1 ever does reach them, and it costs nothing.
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
    s.chromeOverride =
        WindhawkUtils::StringSetting::make(L"Format.ChromeOverride").get();
    s.suffixOverride =
        WindhawkUtils::StringSetting::make(L"Parsing.BrowserSuffix").get();
    s.guessProfile = Wh_GetIntSetting(L"Parsing.GuessProfileWhenUnknown") != 0;

    if (s.normal.empty()) {
        s.normal = L"{title}?( {more})?( - {profile})";
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);

    // After the swap, so the new template is in place for whatever the bump
    // re-arms. This narrows the window, it does not close it: the counter is
    // read where a diagnostic fires, not where the render began, so a render
    // still holding the old template can consume the new generation.
    InterlockedIncrement(&g_templateGeneration);
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    const std::wstring exe = ModulePath(nullptr);
    const size_t baseAt = exe.rfind(L'\\');
    const std::wstring base =
        (baseAt == std::wstring::npos) ? exe : exe.substr(baseAt + 1);
    g_isChrome = (_wcsicmp(base.c_str(), L"chrome.exe") == 0);

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
            // A WebView2 host owns no browser frames either. Not the ordinary
            // runtime - that is msedgewebview2.exe, which @include never names -
            // but an app pointed at a full Edge install through
            // browserExecutableFolder, which launches msedge.exe with this
            // switch and no --type=. The '=' is optional; the switch is also
            // accepted bare.
            if (_wcsnicmp(argv[i], L"--embedded-browser-webview", 26) == 0 &&
                (argv[i][26] == L'\0' || argv[i][26] == L'=')) {
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

    if (!WindhawkUtils::SetFunctionHook(SetWindowTextW, SetWindowTextW_Hook,
                                        &SetWindowTextW_Original)) {
        Wh_Log(L"failed to hook SetWindowTextW");
        return FALSE;
    }

    // AFTER the hook: Windhawk does not call Wh_ModUninit for a mod whose init
    // returned FALSE, so anything created before a failing step leaks once per
    // load attempt. Manual-reset, so that once teardown starts every wait in the
    // worker returns rather than one of them consuming the signal.
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"could not create the stop event; teardown will be up to a "
               L"slice slower");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    g_worker = CreateThread(nullptr, 0, DiscoveryThread, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"failed to start discovery thread");
    }
}

// Handled in place, EXCEPT the browser-suffix override: it is consumed once by
// the worker during discovery, so by the time a user reaches for it the worker
// has returned and applying it in place would do nothing in exactly the
// situation its own description tells them to use it for. A reload is clean -
// Wh_ModUninit restores every title and the fresh instance re-runs discovery.
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
    // flag is also what makes the worker abandon an in-progress sweep, so it has
    // to be set first.
    InterlockedExchange(&g_passthrough, 1);
    // Flag first, THEN wake. The flag is what the worker believes; the event
    // only stops it sleeping. Signalling first would let it wake, re-read a flag
    // that was not yet set, and go back to sleep for a whole period.
    if (g_stopEvent) SetEvent(g_stopEvent);

    if (g_worker) {
        // WAIT UNCONDITIONALLY - a timeout does not help. Windhawk unloads this
        // DLL right after Wh_ModUninit returns, and what matters is the worker's
        // instruction pointer, not its handle: a thread still executing mod code
        // when the image unmaps faults the browser. The stop flag is checked at
        // points, which is a race rather than a guarantee, and teardown needs the
        // guarantee.
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
}

void Wh_ModUninit() {
    // Hooks are removed by now, so these restoring writes are not intercepted.
    // Leaving rewritten titles behind after an uninstall would be unacceptable.
    std::unordered_map<HWND, WindowState> snapshot;
    AcquireSRWLockExclusive(&g_lock);
    snapshot.swap(g_states);
    ReleaseSRWLockExclusive(&g_lock);

    int restored = 0;
    int failed   = 0;
    int skipped  = 0;
    int muted    = 0;
    // Threads that have already failed to answer, exactly as the sweep does:
    // every browser frame shares a UI thread, so one timeout is all the evidence
    // needed not to spend another 250 ms per window on an unresponsive one.
    std::vector<DWORD> mute;
    for (const auto& [hWnd, st] : snapshot) {
        if (st.source.empty() || st.source == st.applied) continue;

        // IsWindow ALONE IS NOT ENOUGH, and this is the dangerous case. An HWND
        // is only unique while its window lives; once ours is destroyed the
        // handle can be recycled by ANY process on the desktop. IsWindow would
        // then be true of a stranger's window, and this loop would set that
        // window's title to a browser title it never had. Requiring the window
        // to still belong to this process makes the restore self-limiting.
        DWORD pid = 0;
        const DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
        if (!pid || pid != GetCurrentProcessId()) { ++skipped; continue; }
        // The frame must also still be showing what we wrote. A title that
        // arrived while the mod was already in passthrough - between
        // Wh_ModBeforeUninit and here - is NEWER than our remembered original,
        // and restoring over it would replace a current title with a stale one
        // on a window that may never write its own title again.
        if (std::find(mute.begin(), mute.end(), tid) != mute.end()) {
            ++muted;
            continue;
        }
        std::wstring cur;
        bool         readAnswered = false;
        if (!ReadTitleFromOtherThread(hWnd, &cur, &readAnswered)) {
            // Same rule as the write below, reached sooner: a title too large
            // to be real, and a window destroyed since the check above, both
            // fail this read without the thread having gone quiet.
            DWORD       pid2 = 0;
            const DWORD tid2 = GetWindowThreadProcessId(hWnd, &pid2);
            if (!readAnswered && pid2 == GetCurrentProcessId() && tid2 == tid) {
                mute.push_back(tid);
            }
            ++failed;
            continue;
        }
        if (cur != st.applied) { ++skipped; continue; }

        bool answered = false;
        if (WriteTitleFromOtherThread(hWnd, st.source, &answered)) {
            ++restored;
        } else {
            // Mute ONLY on silence, and only when the window is still there to
            // have been silent. A window that answered and refused says nothing
            // about its siblings, and since every browser frame shares one UI
            // thread, muting on a refusal abandons the restore for all of them -
            // leaving our titles in place after the mod is gone. The sweep
            // already gets this right.
            //
            // SMTO_ERRORONEXIT also fails a send whose window is being
            // destroyed, which is ordinary during teardown and says nothing
            // about the thread either - so re-check ownership rather than trust
            // the failure. Recycling is not a concern here: a handle that now
            // belongs to another process or thread fails this test too, and the
            // only cost of a false negative is one unmuted thread.
            DWORD pid2 = 0;
            const DWORD tid2 = GetWindowThreadProcessId(hWnd, &pid2);
            if (!answered && pid2 == GetCurrentProcessId() && tid2 == tid) {
                mute.push_back(tid);
            }
            ++failed;
        }
    }
    // All four, because the ones that are not "restored" are the interesting
    // ones: a window whose thread did not answer keeps our title after the mod
    // is gone, and this log line is the only place that says so. `failed` says
    // "not restored" and not "not acknowledged" because it also holds a window
    // that answered and refused, and one whose title read back unusable - both
    // acknowledged, neither restored. The last count is kept apart from
    // `skipped` deliberately - a skip is benign (the window moved on, or is not
    // ours), while a mute means the one-strike rule gave up on every remaining
    // window of that thread, and lumping the two together is what made the
    // rule's cost unmeasurable.
    Wh_Log(L"restored %d title(s), %d not restored, %d skipped, %d skipped "
           L"after a thread stopped answering",
           restored, failed, skipped, muted);

    // Last, and only here. Wh_ModBeforeUninit has already joined the worker, so
    // nothing can still be waiting on this handle - closing it while a wait was
    // outstanding is undefined.
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}


