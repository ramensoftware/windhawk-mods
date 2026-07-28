// ==WindhawkMod==
// @id              mond-lockscreen-clock
// @name            Mond Lock Screen Clock
// @description     Mond-style lock screen clock with a large day of the week.
// @version         7.2
// @author          Siva
// @github          https://github.com/siva-ratnakar
// @license         MIT
// @include         LockApp.exe
// @compilerOptions -lruntimeobject -lole32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mond Lock Screen Clock

Restyles the Windows lock screen into a Mond-like arrangement: the day of the
week as a large first line, with the date and time smaller underneath.

The stock day/date line is not left in place - it is reused to carry the second
line, so nothing is duplicated and the two lines can have different sizes.

## After changing a setting, wait about a minute
LockApp.exe is suspended while it is not in use, so changes cannot be applied
immediately. The clock updates its text once a minute, and that is when new
settings take effect.

  1. Change the setting.
  2. Press Win+L.
  3. Wait up to ~60 seconds, until the minute ticks over.

Or lock, unlock, and lock again - the second lock is styled straight away.

## Using a custom font
The font must be installed for ALL USERS. A plain right-click "Install" puts it
in your user profile, which the lock screen cannot read; it will silently fall
back to the default font with no error.

  1. Download the font. Mond uses Anurati:
     https://www.behance.net/gallery/33704618/ANURATI-Free-Font
  2. Right-click the .otf and choose "Install for all users", so it lands in
     C:\Windows\Fonts.
  3. Run: taskkill /f /im LockApp.exe
     The font list is cached per process, so the lock screen must restart.
  4. Enter the FAMILY name in the settings ("Anurati"), not the file name
     ("Anurati-Regular 400.otf").

The mod checks whether the font is visible to the lock screen and, if the name
does not match but exactly one installed font is close, corrects it on its own.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- highlight: day
  $name: Main line
  $description: Which item is shown large on the first line. The remaining two appear smaller underneath.
  $options:
  - day: Day of week
  - time: Time
  - date: Date

- heroSize: 52
  $name: Main line size

- secondSize: 34
  $name: Second line size

- useCustomFont: true
  $name: Use a custom font
  $description: >-
    The font must be installed for ALL USERS. A normal right-click "Install"
    puts it in your user profile, where the lock screen cannot read it, and it
    will silently fall back to the default font.

    Download Anurati (free): https://www.behance.net/gallery/33704618/ANURATI-Free-Font
    Then right-click the .otf and choose "Install for all users", so the file
    ends up in C:\Windows\Fonts. Afterwards run
    "taskkill /f /im LockApp.exe" so the lock screen reloads its font list.

    Enter the FAMILY name below, not the file name: "Anurati", not
    "Anurati-Regular 400.otf". If the name is wrong but exactly one installed
    font matches, the mod corrects it automatically.

- fontFamily: Anurati
  $name: Font name

- fallbackFont: Segoe UI
  $name: Fallback font
  $description: Used when the font above is not installed or cannot be applied.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winstring.h>
#include <roapi.h>
#include <string>

// ---------------------------------------------------------------------------
// Hooked / resolved functions
//
// Only put_Text is genuinely intercepted. The setters are hooked purely as
// passthroughs so that Windhawk resolves their addresses for us to call.
// Nothing hot (get_Children, get_Parent, get_Text) is hooked any more - doing
// so in 2.3 stopped hooks from being applied at all.
// ---------------------------------------------------------------------------

typedef HRESULT(__cdecl* put_Text_t)(void*, HSTRING);
typedef HRESULT(__cdecl* put_FontSize_t)(void*, double);
struct XamlFontWeight { UINT16 Weight; };
typedef HRESULT(__cdecl* put_FontWeight_t)(void*, XamlFontWeight);
typedef HRESULT(__cdecl* put_CharacterSpacing_t)(void*, INT32);
typedef HRESULT(__cdecl* put_TextAlignment_t)(void*, int);
typedef HRESULT(__cdecl* put_FontFamily_t)(void*, void*);

// Both names below come from a real symbol dump.
// get_Parent is an instance method on DirectUI::FrameworkElement (NOT
// FrameworkElementGenerated, which was my earlier wrong guess).
typedef HRESULT(__cdecl* get_Parent_t)(void*, void**);
// GetChildStatic is a STATIC, so there is no this-pointer and no ambiguity
// about which interface pointer to pass - the reason the earlier
// Panel::get_Children approach was doomed.
typedef HRESULT(__cdecl* GetChildStatic_t)(void*, int, void**);
typedef HRESULT(__cdecl* GetChildrenCountStatic_t)(void*, int*);
typedef HRESULT(__cdecl* get_Text_t)(void*, HSTRING*);
typedef HRESULT(__cdecl* put_Opacity_t)(void*, double);

// ctl::do_query_interface template instantiations. Each has the target IID
// compiled in, so these are safe pointer converters requiring no IID from us.
// This is what fixes the 5.0 crash: our pointer is an ITextBlock slice while
// get_Parent is virtual on a different slice. Convert first, then call.
typedef HRESULT(__cdecl* qi_t)(void** out, void* in);

// DirectUI::VisualTreeHelper::GetParentStaticPrivate(IDependencyObject*,
//     unsigned char, IDependencyObject**, unsigned char*, unsigned char*)
// A STATIC: the object is a parameter, so there is no this-pointer and no
// virtual dispatch. The instance method FrameworkElement::get_Parent crashed
// LockApp even when handed a correctly converted FrameworkElement pointer,
// while the sibling statics GetChildStatic / GetChildrenCountStatic are the
// same shape as this one.
typedef HRESULT(__cdecl* GetParentStatic_t)(void*, unsigned char, void**,
                                            unsigned char*, unsigned char*);

put_Text_t pOriginal_put_Text = nullptr;
put_FontSize_t pOriginal_put_FontSize = nullptr;
put_FontWeight_t pOriginal_put_FontWeight = nullptr;
put_CharacterSpacing_t pOriginal_put_CharacterSpacing = nullptr;
put_TextAlignment_t pOriginal_put_TextAlignment = nullptr;
put_FontFamily_t pOriginal_put_FontFamily = nullptr;
get_Parent_t pOriginal_get_Parent = nullptr;
GetChildStatic_t pOriginal_GetChildStatic = nullptr;
GetChildrenCountStatic_t pOriginal_GetChildrenCount = nullptr;
get_Text_t pOriginal_get_Text = nullptr;
put_Opacity_t pOriginal_put_Opacity = nullptr;
qi_t pQI_FrameworkElement = nullptr;   // IUnknown*     -> DirectUI::FrameworkElement*
qi_t pQI_TextBlock = nullptr;          // IUnknown*     -> ITextBlock*
qi_t pQI_DependencyObject = nullptr;   // IInspectable* -> IDependencyObject*
GetParentStatic_t pGetParentStatic = nullptr;

// NOTE: this hook is a plain passthrough again. Version 3.5 used it to try to
// discover the stock date TextBlock, on the assumption that XAML would apply a
// style through it. It never fired once - XAML sets properties through the core
// property system and bypasses these projected DirectUI setters entirely, which
// is also why the date's text never reaches put_Text. Reaching that element
// requires the visual/logical tree instead.
HRESULT __cdecl Hook_put_FontSize(void* t, double v) {
    return pOriginal_put_FontSize(t, v);
}
HRESULT __cdecl Hook_put_FontWeight(void* t, XamlFontWeight v) {
    return pOriginal_put_FontWeight(t, v);
}
HRESULT __cdecl Hook_put_CharacterSpacing(void* t, INT32 v) {
    return pOriginal_put_CharacterSpacing(t, v);
}
HRESULT __cdecl Hook_put_TextAlignment(void* t, int v) {
    return pOriginal_put_TextAlignment(t, v);
}
HRESULT __cdecl Hook_put_FontFamily(void* t, void* v) {
    return pOriginal_put_FontFamily(t, v);
}
HRESULT __cdecl Hook_get_Parent(void* t, void** p) {
    return pOriginal_get_Parent(t, p);
}
HRESULT __cdecl Hook_GetChildStatic(void* d, int i, void** c) {
    return pOriginal_GetChildStatic(d, i, c);
}
HRESULT __cdecl Hook_GetChildrenCount(void* d, int* n) {
    return pOriginal_GetChildrenCount(d, n);
}
HRESULT __cdecl Hook_get_Text(void* t, HSTRING* v) {
    return pOriginal_get_Text(t, v);
}
HRESULT __cdecl Hook_put_Opacity(void* t, double v) {
    return pOriginal_put_Opacity(t, v);
}
HRESULT __cdecl Hook_QI_FE(void** o, void* i) { return pQI_FrameworkElement(o, i); }
HRESULT __cdecl Hook_QI_TB(void** o, void* i) { return pQI_TextBlock(o, i); }
HRESULT __cdecl Hook_QI_DO(void** o, void* i) { return pQI_DependencyObject(o, i); }
HRESULT __cdecl Hook_GetParentStatic(void* d, unsigned char f, void** o,
                                     unsigned char* a, unsigned char* b) {
    return pGetParentStatic(d, f, o, a, b);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class Highlight { Day, Time, Date };

struct Settings {
    Highlight highlight;
    bool showSecondary;
    double fontSize;
    int fontWeight;
    int letterSpacing;
    int textAlignment;
    bool uppercase;
    bool boldNumbers;
    std::wstring separator;
    bool applyCustomFont;
    std::wstring fontName;
    std::wstring fallbackFont;
    bool verboseLog;
    bool fontFactoryFourArgs;
    std::wstring logFontsMatching;
    bool walkTree;
    bool writeToDateElement;
    bool hideDateElement;
    int searchDepth;
    int parentLevels;
    int dateElementIndex;
    double secondarySize;
};
Settings g_s;

HANDLE g_setupThread = nullptr;
HANDLE g_stopEvent = nullptr;

void* g_clockElement = nullptr;
void* g_fontFamily = nullptr;      // cached IFontFamily*, created once
bool g_fontAttempted = false;

// ---------------------------------------------------------------------------
// Pointer safety. Windhawk's clang build has no MSVC SEH (__try), so instead
// of catching faults we validate before dereferencing.
// ---------------------------------------------------------------------------

static bool IsReadableAddress(const void* p, size_t bytes) {
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi = {};
    if (VirtualQuery(p, &mbi, sizeof(mbi)) == 0) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    const BYTE* end = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
    return ((const BYTE*)p + bytes) <= end;
}

static bool IsLiveElement(void* p) {
    if (!IsReadableAddress(p, sizeof(void*))) return false;
    return IsReadableAddress(*(void**)p, sizeof(void*));
}

// ---------------------------------------------------------------------------
// Clock text detection
// ---------------------------------------------------------------------------

static bool IsInvisibleFormatChar(wchar_t c) {
    return c == 0x200E || c == 0x200F || c == 0x061C ||
           (c >= 0x2066 && c <= 0x2069) ||
           (c >= 0x202A && c <= 0x202E) || c == 0xFEFF;
}

// The lock screen uses U+2236 RATIO, not an ASCII colon.
static bool IsColonLike(wchar_t c) {
    return c == 0x003A || c == 0x2236 || c == 0x02D0 ||
           c == 0xA789 || c == 0xFE55 || c == 0xFF1A;
}

static bool LooksLikeClock(PCWSTR s, UINT32 len) {
    if (len == 0 || len > 24) return false;
    bool hasColon = false;
    int digits = 0;
    for (UINT32 i = 0; i < len; i++) {
        wchar_t c = s[i];
        if (IsColonLike(c)) hasColon = true;
        else if (c >= L'0' && c <= L'9') digits++;
        else if (c == L' ' || c == 0x00A0 || c == 0x202F) continue;
        else if (IsInvisibleFormatChar(c)) continue;
        else if (c >= 0xE000 && c <= 0xF8FF) return false;   // MDL2 icon
        else if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) continue;
        else return false;
    }
    return hasColon && digits >= 2;
}

// ---------------------------------------------------------------------------
// Building our text
// ---------------------------------------------------------------------------

static std::wstring FormatDatePart(PCWSTR pattern) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR buf[128] = {0};
    int n = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pattern, buf,
                            ARRAYSIZE(buf), nullptr);
    return n > 0 ? std::wstring(buf) : std::wstring();
}

static std::wstring GetTimeText() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR buf[128] = {0};
    // nullptr format = respect the user's 12h/24h preference.
    int n = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st,
                            nullptr, buf, ARRAYSIZE(buf));
    return n > 0 ? std::wstring(buf) : std::wstring();
}

// A TextBlock has exactly one FontWeight, so digits cannot be bolded through
// the API while letters stay light. Unicode does however define pre-bolded
// digit characters (MATHEMATICAL SANS-SERIF BOLD DIGIT ZERO at U+1D7EC), which
// render bold through normal font fallback. Above U+FFFF, so surrogate pairs.
static void AppendBoldDigit(std::wstring& out, wchar_t digit) {
    const UINT32 base = 0x1D7EC;                  // sans-serif bold zero
    UINT32 cp = base + (UINT32)(digit - L'0');
    UINT32 v = cp - 0x10000;
    out += (wchar_t)(0xD800 + (v >> 10));         // high surrogate
    out += (wchar_t)(0xDC00 + (v & 0x3FF));       // low surrogate
}

static std::wstring BoldenDigits(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size() * 2);
    for (wchar_t c : in) {
        if (c >= L'0' && c <= L'9') AppendBoldDigit(out, c);
        else out += c;
    }
    return out;
}

// ---------------------------------------------------------------------------
// Visual tree walk
//
// The stock date TextBlock is set by XAML binding, so it never appears in
// put_Text and never receives a projected put_FontSize. The visual tree is the
// only way to reach it. GetRuntimeClassName (IInspectable slot 4) identifies
// each child without needing any IID.
// ---------------------------------------------------------------------------

struct InspectableVtbl {
    HRESULT(__stdcall* QueryInterface)(void*, const GUID*, void**);
    ULONG(__stdcall* AddRef)(void*);
    ULONG(__stdcall* Release)(void*);
    HRESULT(__stdcall* GetIids)(void*, ULONG*, GUID**);
    HRESULT(__stdcall* GetRuntimeClassName)(void*, HSTRING*);
    HRESULT(__stdcall* GetTrustLevel)(void*, int*);
};

static std::wstring RuntimeClassOf(void* obj) {
    if (!IsLiveElement(obj)) return L"<unreadable>";
    InspectableVtbl* v = *(InspectableVtbl**)obj;
    if (!IsReadableAddress(v, sizeof(InspectableVtbl))) return L"<no vtable>";

    HSTRING h = nullptr;
    if (FAILED(v->GetRuntimeClassName(obj, &h)) || !h) return L"<no name>";
    UINT32 len = 0;
    PCWSTR raw = WindowsGetStringRawBuffer(h, &len);
    std::wstring out = raw ? std::wstring(raw, len) : L"<empty>";
    WindowsDeleteString(h);
    return out;
}

void* g_dateElement = nullptr;
bool g_walkDone = false;

// Read a TextBlock's current text. This is what makes identification reliable:
// rather than assuming which sibling is the date, we read each one and check.
static bool ReadElementText(void* el, std::wstring& out) {
    if (!pOriginal_get_Text || !IsLiveElement(el)) return false;
    HSTRING h = nullptr;
    if (FAILED(pOriginal_get_Text(el, &h))) return false;
    UINT32 len = 0;
    PCWSTR raw = h ? WindowsGetStringRawBuffer(h, &len) : nullptr;
    out = raw ? std::wstring(raw, len) : L"";
    if (h) WindowsDeleteString(h);
    return true;
}

// Does this text look like the stock date line? Letters plus, usually, digits,
// and definitely not our own rewritten block.
static bool TextLooksLikeDate(const std::wstring& t) {
    if (t.empty() || t.length() > 80) return false;
    int letters = 0;
    for (wchar_t c : t) {
        if (c >= 0xE000 && c <= 0xF8FF) return false;   // icon glyph
        if (iswalpha(c)) letters++;
    }
    return letters >= 3;
}

// Convert our pointer into the interface a call expects. Returns nullptr on
// failure; callers must never fall through to calling a method on an
// unconverted pointer.
static void* AsFrameworkElement(void* any) {
    if (!pQI_FrameworkElement || !IsLiveElement(any)) return nullptr;
    void* out = nullptr;
    // Log BEFORE the call. If the process dies, the missing "returned" line
    // identifies this exact call - that is how the 6.0 crash was located.
    Wh_Log(L"  QI->FrameworkElement: calling %p(out, %p)", pQI_FrameworkElement,
           any);
    HRESULT hr = pQI_FrameworkElement(&out, any);
    Wh_Log(L"  QI->FrameworkElement returned hr=0x%08X out=%p", hr, out);
    if (FAILED(hr) || !out) {
        Wh_Log(L"  QI->FrameworkElement failed hr=0x%08X", hr);
        return nullptr;
    }
    return out;
}

static void* AsDependencyObject(void* any) {
    if (!pQI_DependencyObject || !IsLiveElement(any)) return nullptr;
    void* out = nullptr;
    Wh_Log(L"  QI->DependencyObject: calling on %p", any);
    HRESULT hr = pQI_DependencyObject(&out, any);
    Wh_Log(L"  QI->DependencyObject returned hr=0x%08X out=%p", hr, out);
    if (FAILED(hr) || !out) return nullptr;
    return out;
}

static void* AsTextBlock(void* any) {
    if (!pQI_TextBlock || !IsLiveElement(any)) return nullptr;
    void* out = nullptr;
    Wh_Log(L"  QI->TextBlock: calling on %p", any);
    HRESULT hr = pQI_TextBlock(&out, any);
    Wh_Log(L"  QI->TextBlock returned hr=0x%08X out=%p", hr, out);
    // Failure just means it is not a TextBlock - a useful type test in itself.
    if (FAILED(hr) || !out) return nullptr;
    return out;
}

// Depth-first search for the stock date TextBlock. Every pointer is converted
// to the right interface before any method is called on it.
int g_nodeBudget = 0;

static void SearchSubtree(void* nodeDO, void* clockTB, int depth, int indent) {
    if (!nodeDO || depth < 0 || g_dateElement || g_nodeBudget <= 0) return;

    int count = 0;
    if (!pOriginal_GetChildrenCount ||
        FAILED(pOriginal_GetChildrenCount(nodeDO, &count))) {
        return;
    }
    if (count < 0 || count > 64) return;

    for (int i = 0; i < count && !g_dateElement; i++) {
        if (--g_nodeBudget <= 0) {
            Wh_Log(L"WALK node budget exhausted - raise it or lower the depth");
            return;
        }

        void* childDO = nullptr;
        if (FAILED(pOriginal_GetChildStatic(nodeDO, i, &childDO)) || !childDO) {
            continue;
        }

        std::wstring cls = RuntimeClassOf(childDO);

        // A successful QI to ITextBlock IS the type test - no class-name string
        // matching, and no risk of calling a TextBlock method on a non-TextBlock.
        void* childTB = AsTextBlock(childDO);
        bool isClock = (childTB && childTB == clockTB);

        std::wstring text;
        if (childTB && !isClock) ReadElementText(childTB, text);

        Wh_Log(L"WALK %*s[%d] %s%s%s", indent, L"", i, cls.c_str(),
               childTB ? L" (TextBlock)" : L"",
               isClock ? L"  <== CLOCK" : L"");
        if (!text.empty()) {
            Wh_Log(L"WALK %*s     text=[%s]", indent, L"", text.c_str());
        }

        if (childTB && !isClock && TextLooksLikeDate(text)) {
            g_dateElement = childTB;      // keep the CONVERTED pointer
            Wh_Log(L"WALK %*s     ^^ SELECTED as date element (%p)", indent,
                   L"", childTB);
            return;
        }

        SearchSubtree(childDO, clockTB, depth - 1, indent + 2);
    }
}

static void WalkTreeFromClock(void* clock) {
    if (g_walkDone || !g_s.walkTree) return;
    g_walkDone = true;      // once per load; this is chatty

    if (!pQI_FrameworkElement || !pQI_TextBlock) {
        Wh_Log(L"WALK unavailable: QI converters fe=%p tb=%p",
               pQI_FrameworkElement, pQI_TextBlock);
        return;
    }
    if (!pGetParentStatic || !pOriginal_GetChildStatic ||
        !pOriginal_GetChildrenCount) {
        Wh_Log(L"WALK unavailable: parentStatic=%p child=%p count=%p",
               pGetParentStatic, pOriginal_GetChildStatic,
               pOriginal_GetChildrenCount);
        return;
    }

    Wh_Log(L"WALK clock=%p (%s)", clock, RuntimeClassOf(clock).c_str());

    // 6.0 converted the pointer correctly and STILL crashed inside the virtual
    // FrameworkElement::get_Parent. 6.2 therefore uses the static instead:
    // GetParentStaticPrivate takes the object as a plain argument, so there is
    // no vtable dispatch at all - the same shape as GetChildStatic, which is
    // the only family of calls here that has never crashed.
    void* clockDO = AsDependencyObject(clock);
    if (!clockDO) {
        Wh_Log(L"WALK cannot convert clock to IDependencyObject - stopping");
        return;
    }

    // The immediate parent is a StackPanel holding only the clock and two
    // Grids; the date TextBlock is elsewhere. Climb to a common ancestor.
    void* parentDO = nullptr;
    void* node = clockDO;
    for (int level = 1; level <= g_s.parentLevels; level++) {
        void* up = nullptr;
        unsigned char outA = 0, outB = 0;
        HRESULT hr = pGetParentStatic(node, 0, &up, &outA, &outB);
        if (FAILED(hr) || !up) {
            Wh_Log(L"WALK climb stopped at level %d hr=0x%08X", level, hr);
            break;
        }
        Wh_Log(L"WALK ancestor level %d = %p (%s)", level, up,
               RuntimeClassOf(up).c_str());
        parentDO = up;
        node = up;
    }

    if (!parentDO) {
        Wh_Log(L"WALK no ancestor found");
        return;
    }
    Wh_Log(L"WALK searching from %p (%s), depth %d", parentDO,
           RuntimeClassOf(parentDO).c_str(), g_s.searchDepth);

    g_nodeBudget = 300;
    SearchSubtree(parentDO, clock, g_s.searchDepth, 0);
    Wh_Log(L"WALK done. dateElement=%p", g_dateElement);
}

static void BuildParts(std::wstring& hero, std::wstring& second);

// The secondary line on its own, for writing into the date element.
static std::wstring BuildSecondaryLine() {
    std::wstring hero, second;
    BuildParts(hero, second);
    if (g_s.uppercase) {
        for (auto& c : second) c = towupper(c);
    }
    if (g_s.boldNumbers) second = BoldenDigits(second);
    return second;
}

// Split into hero and secondary according to the highlight setting. Shared by
// BuildBlock and BuildSecondaryLine so the two can never disagree.
static void BuildParts(std::wstring& hero, std::wstring& second) {
    std::wstring day = FormatDatePart(L"dddd");
    std::wstring date = FormatDatePart(L"d MMMM");
    std::wstring time = GetTimeText();

    switch (g_s.highlight) {
        case Highlight::Time:
            hero = time;  second = day + g_s.separator + date;  break;
        case Highlight::Date:
            hero = date;  second = day + g_s.separator + time;  break;
        case Highlight::Day:
        default:
            hero = day;   second = date + g_s.separator + time; break;
    }
}

static std::wstring BuildBlock() {
    std::wstring hero, second;
    BuildParts(hero, second);

    std::wstring out = hero;
    // With the date element in use the second line lives there, so keep this
    // block to the hero alone.
    if (g_s.showSecondary && !(g_s.writeToDateElement && g_dateElement)) {
        out += L"\n";
        out += second;
    }
    if (g_s.uppercase) {
        for (auto& c : out) c = towupper(c);
    }
    if (g_s.boldNumbers) {
        out = BoldenDigits(out);   // must run after uppercasing
    }
    return out;
}

// ---------------------------------------------------------------------------
// Font family
//
// put_FontFamily needs an IFontFamily object, not a string. Rather than
// hard-coding an IID I do not actually know, we ask the activation factory
// which interfaces it implements (IInspectable::GetIids) and QueryInterface
// the one that is not a well-known system interface. That turns a guess into
// a runtime lookup.
// ---------------------------------------------------------------------------

static const GUID G_IActivationFactory =
    {0x00000035,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID G_IInspectable =
    {0xAF86E2E0,0xB12D,0x4C6A,{0x9C,0x5A,0xD7,0xAA,0x65,0x10,0x1E,0x90}};
static const GUID G_IAgileObject =
    {0x94EA2B94,0xE9CC,0x49E0,{0xC0,0xFF,0xEE,0x64,0xCA,0x8F,0x5B,0x90}};
static const GUID G_IMarshal =
    {0x00000003,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID G_IWeakRefSource =
    {0x00000038,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

struct FactoryVtbl {
    HRESULT(__stdcall* QueryInterface)(void*, const GUID*, void**);
    ULONG(__stdcall* AddRef)(void*);
    ULONG(__stdcall* Release)(void*);
    HRESULT(__stdcall* GetIids)(void*, ULONG*, GUID**);
    HRESULT(__stdcall* GetRuntimeClassName)(void*, HSTRING*);
    HRESULT(__stdcall* GetTrustLevel)(void*, int*);
    // Slot 6 is CreateInstanceWithName, but its argument count depends on
    // whether the class uses the composable pattern - so it is called through
    // an explicit vtable index rather than declared here.
};

static void ComRelease(void* p) {
    if (!IsLiveElement(p)) return;
    FactoryVtbl* v = *(FactoryVtbl**)p;
    if (IsReadableAddress(v, sizeof(FactoryVtbl))) v->Release(p);
}

static bool IsWellKnownIid(const GUID& g) {
    return IsEqualGUID(g, G_IActivationFactory) || IsEqualGUID(g, G_IInspectable) ||
           IsEqualGUID(g, G_IAgileObject) || IsEqualGUID(g, G_IMarshal) ||
           IsEqualGUID(g, G_IWeakRefSource);
}

// Is the font file actually present? Purely informational, but it turns
// "the font did not apply" into an actionable message.
static void ReportFontFile(const std::wstring& name) {
    std::wstring pattern = L"C:\\Windows\\Fonts\\*" + name + L"*";
    WIN32_FIND_DATAW fd = {};
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        Wh_Log(L"font file found: %s", fd.cFileName);
        FindClose(h);
        return;
    }
    Wh_Log(L"font file matching '%s' NOT found in C:\\Windows\\Fonts "
           L"(error %lu). Install the font with 'Install for all users' - a "
           L"per-user install is not readable by LockApp.",
           name.c_str(), GetLastError());
}

// XAML does not report an error for an unknown font family - it silently falls
// back to the default face. So a successful put_FontFamily tells us nothing
// about whether the name resolved. Enumerate what this process can actually
// see, which is the only reliable way to learn the real family name.
struct FontEnumCtx {
    std::wstring* names;      // flat, newline-separated list of family names
    int count;
};

static int CALLBACK FontEnumProc(const LOGFONTW* lf, const TEXTMETRICW*, DWORD,
                                 LPARAM param) {
    FontEnumCtx* ctx = (FontEnumCtx*)param;
    if (!lf || !ctx || !ctx->names) return 1;

    // Skip the @vertical-writing duplicates Windows reports.
    if (lf->lfFaceName[0] == L'@') return 1;

    *ctx->names += lf->lfFaceName;
    *ctx->names += L'\n';
    ctx->count++;
    return 1;
}

static std::wstring ToLower(const std::wstring& in) {
    std::wstring out = in;
    for (auto& c : out) c = towlower(c);
    return out;
}

// Trim spaces and any trailing dot, a common copy/paste artefact.
static std::wstring TrimFamilyName(const std::wstring& in) {
    size_t first = in.find_first_not_of(L" \t");
    if (first == std::wstring::npos) return L"";
    size_t last = in.find_last_not_of(L" \t.");
    return in.substr(first, last - first + 1);
}

// Every family name visible to THIS process, newline separated.
static std::wstring CollectVisibleFonts(int* total) {
    std::wstring names;
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        Wh_Log(L"could not get a DC to enumerate fonts");
        return names;
    }
    LOGFONTW lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;
    FontEnumCtx ctx = {&names, 0};
    EnumFontFamiliesExW(hdc, &lf, FontEnumProc, (LPARAM)&ctx, 0);
    ReleaseDC(nullptr, hdc);
    if (total) *total = ctx.count;
    return names;
}

static bool FamilyIsVisible(const std::wstring& all, const std::wstring& name) {
    std::wstring needle = L"\n" + ToLower(name) + L"\n";
    std::wstring hay = L"\n" + ToLower(all);
    return hay.find(needle) != std::wstring::npos;
}

// Families containing `filter`. Returns how many, and the last one seen.
static int FamiliesMatching(const std::wstring& all, const std::wstring& filter,
                            std::wstring* onlyMatch) {
    if (filter.empty()) return 0;
    std::wstring low = ToLower(filter);
    int count = 0;
    size_t pos = 0;
    while (pos < all.size()) {
        size_t nl = all.find(L'\n', pos);
        if (nl == std::wstring::npos) break;
        std::wstring fam = all.substr(pos, nl - pos);
        pos = nl + 1;
        if (!fam.empty() && ToLower(fam).find(low) != std::wstring::npos) {
            count++;
            Wh_Log(L"  visible font family: [%s]", fam.c_str());
            if (onlyMatch) *onlyMatch = fam;
        }
    }
    return count;
}

// XAML reports success for an unknown family and silently falls back to the
// default face, so put_FontFamily's HRESULT proves nothing. Validate the name
// against the fonts this process can really see, and self-correct when the
// configured value is obviously the filename rather than the family name.
static std::wstring ResolveFamilyName(const std::wstring& configured,
                                      const std::wstring& filter) {
    std::wstring name = TrimFamilyName(configured);
    if (name != configured) {
        Wh_Log(L"font name trimmed to '%s'", name.c_str());
    }

    int total = 0;
    std::wstring all = CollectVisibleFonts(&total);
    Wh_Log(L"%d font families visible to LockApp", total);

    if (FamilyIsVisible(all, name)) {
        Wh_Log(L"font family '%s' is visible - using it", name.c_str());
        return name;
    }

    Wh_Log(L"font family '%s' is NOT visible; XAML would silently fall back",
           name.c_str());

    std::wstring only;
    int matches = FamiliesMatching(all, filter, &only);
    if (matches == 1) {
        Wh_Log(L"auto-correcting to the single match: '%s'", only.c_str());
        return only;
    }
    if (matches == 0) {
        Wh_Log(L"no family matching '%s'. The font must be in C:\\Windows\\"
               L"Fonts and registered under HKLM ... CurrentVersion\\Fonts "
               L"with (OpenType) for .otf files. Restart LockApp after fixing.",
               filter.c_str());
    } else {
        Wh_Log(L"%d families match '%s' - set one of them exactly", matches,
               filter.c_str());
    }
    return name;
}

static void* CreateFontFamily(const std::wstring& name) {
    HSTRING clsName = nullptr;
    PCWSTR runtimeClass = L"Windows.UI.Xaml.Media.FontFamily";
    if (FAILED(WindowsCreateString(runtimeClass,
                                   (UINT32)wcslen(runtimeClass), &clsName))) {
        return nullptr;
    }

    void* factory = nullptr;
    HRESULT hr = RoGetActivationFactory(clsName, G_IActivationFactory, &factory);
    WindowsDeleteString(clsName);
    if (FAILED(hr) || !factory) {
        Wh_Log(L"RoGetActivationFactory failed hr=0x%08X", hr);
        return nullptr;
    }
    if (!IsLiveElement(factory)) {
        Wh_Log(L"factory pointer not usable");
        return nullptr;
    }

    FactoryVtbl* fv = *(FactoryVtbl**)factory;
    if (!IsReadableAddress(fv, sizeof(FactoryVtbl))) {
        ComRelease(factory);
        return nullptr;
    }

    // Ask the object which interfaces it supports, instead of guessing an IID.
    ULONG count = 0;
    GUID* iids = nullptr;
    hr = fv->GetIids(factory, &count, &iids);
    if (FAILED(hr) || !iids) {
        Wh_Log(L"GetIids failed hr=0x%08X", hr);
        ComRelease(factory);
        return nullptr;
    }

    void* ffFactory = nullptr;
    for (ULONG i = 0; i < count; i++) {
        if (IsWellKnownIid(iids[i])) continue;
        void* candidate = nullptr;
        if (SUCCEEDED(fv->QueryInterface(factory, &iids[i], &candidate)) &&
            candidate) {
            Wh_Log(L"using factory interface #%lu {%08lX-%04X-%04X-...}", i,
                   iids[i].Data1, iids[i].Data2, iids[i].Data3);
            ffFactory = candidate;
            break;
        }
    }
    CoTaskMemFree(iids);

    if (!ffFactory) {
        Wh_Log(L"no candidate factory interface found");
        ComRelease(factory);
        return nullptr;
    }

    HSTRING familyName = nullptr;
    if (FAILED(WindowsCreateString(name.c_str(), (UINT32)name.length(),
                                   &familyName))) {
        ComRelease(ffFactory);
        ComRelease(factory);
        return nullptr;
    }

    void* fontFamily = nullptr;
    void** vt = *(void***)ffFactory;

    if (IsReadableAddress(vt, sizeof(void*) * 8)) {
        void* slot6 = vt[6];    // first method after IInspectable

        // Log BEFORE the call. If the process dies, the absence of the
        // matching "returned" line is what tells us this call was fatal -
        // which is exactly how the 3.6 crash was identified.
        Wh_Log(L"calling factory slot6=%p with %s args for '%s'", slot6,
               g_s.fontFactoryFourArgs ? L"FOUR" : L"TWO", name.c_str());

        if (g_s.fontFactoryFourArgs) {
            // Composable form: (this, name, outer, inner, instance)
            typedef HRESULT(__stdcall* Fn4)(void*, HSTRING, void*, void**, void**);
            void* inner = nullptr;
            hr = ((Fn4)slot6)(ffFactory, familyName, nullptr, &inner, &fontFamily);
            if (inner && inner != fontFamily) ComRelease(inner);
        } else {
            // Simple form: (this, name, instance)
            typedef HRESULT(__stdcall* Fn2)(void*, HSTRING, void**);
            hr = ((Fn2)slot6)(ffFactory, familyName, &fontFamily);
        }

        Wh_Log(L"factory returned hr=0x%08X obj=%p", hr, fontFamily);
    }

    WindowsDeleteString(familyName);
    ComRelease(ffFactory);
    ComRelease(factory);
    return fontFamily;
}

static void EnsureFontFamily() {
    if (g_fontAttempted || !g_s.applyCustomFont) return;
    if (g_fontFamily) {          // settings changed: drop the old object
        ComRelease(g_fontFamily);
        g_fontFamily = nullptr;
    }
    if (!pOriginal_put_FontFamily) {
        g_fontAttempted = true;
        Wh_Log(L"custom font unavailable: put_FontFamily is not resolved in "
               L"this build (see readme)");
        return;
    }
    g_fontAttempted = true;

    // Match on the first word so "Anurati Regular" still finds Anurati*.otf.
    std::wstring fileHint = g_s.fontName.substr(0, g_s.fontName.find(L' '));
    ReportFontFile(fileHint.empty() ? g_s.fontName : fileHint);

    std::wstring resolved = ResolveFamilyName(g_s.fontName, g_s.logFontsMatching);
    g_fontFamily = CreateFontFamily(resolved);
    if (!g_fontFamily && !g_s.fallbackFont.empty()) {
        Wh_Log(L"falling back to '%s'", g_s.fallbackFont.c_str());
        g_fontFamily = CreateFontFamily(g_s.fallbackFont);
    }
    if (!g_fontFamily) {
        Wh_Log(L"no font applied; keeping the stock font with letter spacing");
    }
}

// ---------------------------------------------------------------------------
// Applying text + style
// ---------------------------------------------------------------------------

static HRESULT SetElementText(void* el, const std::wstring& text) {
    if (!pOriginal_put_Text || !IsLiveElement(el)) return E_FAIL;
    HSTRING h = nullptr;
    HRESULT hr = WindowsCreateString(text.c_str(), (UINT32)text.length(), &h);
    if (FAILED(hr)) return hr;
    hr = pOriginal_put_Text(el, h);
    WindowsDeleteString(h);
    return hr;
}

static void StyleElement(void* el) {
    if (!IsLiveElement(el)) return;

    if (pOriginal_put_FontSize) pOriginal_put_FontSize(el, g_s.fontSize);
    if (pOriginal_put_FontWeight) {
        XamlFontWeight w{(UINT16)g_s.fontWeight};
        pOriginal_put_FontWeight(el, w);
    }
    if (pOriginal_put_CharacterSpacing) {
        pOriginal_put_CharacterSpacing(el, g_s.letterSpacing);
    }
    if (pOriginal_put_TextAlignment) {
        HRESULT hr = pOriginal_put_TextAlignment(el, g_s.textAlignment);
        if (FAILED(hr)) Wh_Log(L"put_TextAlignment(%d) hr=0x%08X",
                               g_s.textAlignment, hr);
    }
    if (g_fontFamily && pOriginal_put_FontFamily) {
        HRESULT hr = pOriginal_put_FontFamily(el, g_fontFamily);
        if (FAILED(hr)) Wh_Log(L"put_FontFamily failed hr=0x%08X", hr);
    }
}

HRESULT __cdecl Hook_put_Text(void* pThis, HSTRING value) {
    UINT32 len = 0;
    PCWSTR raw = value ? WindowsGetStringRawBuffer(value, &len) : nullptr;
    if (!raw || len == 0) return pOriginal_put_Text(pThis, value);

    if (!LooksLikeClock(raw, len)) {
        return pOriginal_put_Text(pThis, value);   // date, icons, status text
    }

    if (g_clockElement != pThis) {
        g_clockElement = pThis;
        Wh_Log(L"clock element = %p", pThis);
    }

    EnsureFontFamily();

    WalkTreeFromClock(pThis);

    std::wstring block = BuildBlock();
    if (g_s.verboseLog) {
        Wh_Log(L"writing block: [%s]", block.c_str());
    }

    // Two-tier layout: hero stays here, the small line goes to the date
    // element, which is the only way to get two different font sizes.
    if (g_s.hideDateElement && g_dateElement && pOriginal_put_Opacity &&
        IsLiveElement(g_dateElement)) {
        pOriginal_put_Opacity(g_dateElement, 0.0);
        if (g_s.verboseLog) Wh_Log(L"hid date element %p", g_dateElement);
    } else if (g_s.writeToDateElement && g_dateElement && g_dateElement != pThis) {
        if (IsLiveElement(g_dateElement)) {
            HRESULT hr = SetElementText(g_dateElement, BuildSecondaryLine());
            if (pOriginal_put_FontSize) {
                pOriginal_put_FontSize(g_dateElement, g_s.secondarySize);
            }
            if (pOriginal_put_FontWeight) {
                XamlFontWeight w{(UINT16)g_s.fontWeight};
                pOriginal_put_FontWeight(g_dateElement, w);
            }
            if (pOriginal_put_CharacterSpacing) {
                pOriginal_put_CharacterSpacing(g_dateElement, g_s.letterSpacing);
            }
            if (pOriginal_put_TextAlignment) {
                pOriginal_put_TextAlignment(g_dateElement, g_s.textAlignment);
            }
            if (g_fontFamily && pOriginal_put_FontFamily) {
                pOriginal_put_FontFamily(g_dateElement, g_fontFamily);
            }
            if (g_s.verboseLog) {
                Wh_Log(L"wrote secondary line to date element %p hr=0x%08X",
                       g_dateElement, hr);
            }
        }
    }


    // Replace the shell's text with ours, so do not forward `value`.
    SetElementText(pThis, block);
    StyleElement(pThis);
    return S_OK;
}

// ---------------------------------------------------------------------------
// Settings / setup
// ---------------------------------------------------------------------------

static std::wstring ReadStringSetting(PCWSTR key, PCWSTR fallback) {
    PCWSTR v = Wh_GetStringSetting(key);
    std::wstring out = (v && *v) ? v : fallback;
    if (v) Wh_FreeStringSetting(v);
    return out;
}

static void LoadSettings() {
    std::wstring h = ReadStringSetting(L"highlight", L"day");
    g_s.highlight = Highlight::Day;
    if (h == L"time") g_s.highlight = Highlight::Time;
    else if (h == L"date") g_s.highlight = Highlight::Date;

    g_s.fontSize = (double)Wh_GetIntSetting(L"heroSize");
    if (g_s.fontSize < 8) g_s.fontSize = 52;
    g_s.secondarySize = (double)Wh_GetIntSetting(L"secondSize");
    if (g_s.secondarySize < 6) g_s.secondarySize = 34;

    g_s.applyCustomFont = Wh_GetIntSetting(L"useCustomFont") != 0;
    g_s.fontName = ReadStringSetting(L"fontFamily", L"Anurati");
    g_s.fallbackFont = ReadStringSetting(L"fallbackFont", L"Segoe UI");

    // Everything below is fixed. These are the values that were arrived at by
    // testing, and exposing them only invited misconfiguration.
    g_s.showSecondary = true;      // second line lives in the date element
    g_s.fontWeight = 200;          // ExtraLight suits Anurati
    g_s.letterSpacing = 120;       // 1/1000 em; Anurati is wide-tracked
    g_s.textAlignment = 0;         // WinRT XAML: Center=0, Left=1, Right=2
    g_s.uppercase = true;          // Anurati has no lowercase glyphs
    g_s.boldNumbers = true;
    g_s.separator = L" \u00B7 ";
    g_s.fontFactoryFourArgs = true;   // composable factory shape; two crashes

    // Tree walk: locates the stock date TextBlock so it can carry the second
    // line. These are the values that located it successfully - the clock sits
    // in a StackPanel, and the date is found three levels up, three deep.
    g_s.walkTree = true;
    g_s.writeToDateElement = true;
    g_s.hideDateElement = false;
    g_s.parentLevels = 3;
    g_s.searchDepth = 3;
    g_s.dateElementIndex = -1;

    // Match the font file on the first word, so "Anurati Regular" still finds
    // Anurati*.otf when reporting whether the file is present.
    g_s.logFontsMatching = g_s.fontName.substr(0, g_s.fontName.find(L' '));
    g_s.verboseLog = false;

    Wh_Log(L"settings: highlight=%d hero=%.0f second=%.0f font='%s' custom=%d",
           (int)g_s.highlight, g_s.fontSize, g_s.secondarySize,
           g_s.fontName.c_str(), (int)g_s.applyCustomFont);
}

// In a FRESH LockApp process, Wh_ModInit runs before Windows.UI.Xaml.dll is
// loaded, so GetModuleHandleW returns null. Earlier versions gave up here and
// silently did nothing. Wait for the module, and load it ourselves if it still
// has not appeared - LockApp is a XAML app, so it is loaded regardless.
static HMODULE WaitForXamlModule() {
    for (int i = 0; i < 120; i++) {          // up to ~60 seconds
        HMODULE h = GetModuleHandleW(L"Windows.UI.Xaml.dll");
        if (h) {
            if (i > 0) Wh_Log(L"Windows.UI.Xaml.dll appeared after %d ms", i * 500);
            return h;
        }
        // Bail out promptly if the mod is being unloaded.
        if (g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            Wh_Log(L"setup cancelled (mod unloading)");
            return nullptr;
        }
        if (!g_stopEvent) Sleep(500);
    }

    Wh_Log(L"Windows.UI.Xaml.dll still absent; loading it explicitly");
    return LoadLibraryW(L"Windows.UI.Xaml.dll");
}

DWORD WINAPI SetupHookThread(LPVOID) {
    HMODULE hXaml = WaitForXamlModule();
    if (!hXaml) {
        Wh_Log(L"could not obtain Windows.UI.Xaml.dll - no hooks installed");
        return 0;
    }

    // Windows.UI.Xaml.dll
    WindhawkUtils::SYMBOL_HOOK windowsUiXamlDllHooks[] = {
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_Text(struct HSTRING__ *))"},
            &pOriginal_put_Text, Hook_put_Text,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_FontSize(double))"},
            &pOriginal_put_FontSize, Hook_put_FontSize, true,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_FontWeight(struct Windows::UI::Text::FontWeight))"},
            &pOriginal_put_FontWeight, Hook_put_FontWeight, true,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_CharacterSpacing(int))"},
            &pOriginal_put_CharacterSpacing, Hook_put_CharacterSpacing, true,
        },
        // These two names come from an actual symbol dump, not a guess.
        // The critical detail: there is NO "ABI::" prefix. Earlier attempts
        // used ABI::Windows::UI::Xaml::..., which matched nothing and made
        // Windhawk fall back to a full PDB scan - that is what hung the setup
        // thread and produced the black lock screen.
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_TextAlignment(enum Windows::UI::Xaml::TextAlignment))"},
            &pOriginal_put_TextAlignment, Hook_put_TextAlignment, true,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::put_FontFamily(struct Windows::UI::Xaml::Media::IFontFamily *))"},
            &pOriginal_put_FontFamily, Hook_put_FontFamily, true,
        },
        // Both verified by symbol dump - note DirectUI::FrameworkElement, not
        // FrameworkElementGenerated.
        {
            {LR"(public: virtual long __cdecl DirectUI::FrameworkElement::get_Parent(struct Windows::UI::Xaml::IDependencyObject * *))"},
            &pOriginal_get_Parent, Hook_get_Parent, true,
        },
        {
            {LR"(public: static long __cdecl DirectUI::VisualTreeHelper::GetChildStatic(struct Windows::UI::Xaml::IDependencyObject *,int,struct Windows::UI::Xaml::IDependencyObject * *))"},
            &pOriginal_GetChildStatic, Hook_GetChildStatic, true,
        },
        {
            {LR"(public: static long __cdecl DirectUI::VisualTreeHelper::GetChildrenCountStatic(struct Windows::UI::Xaml::IDependencyObject *,int *))"},
            &pOriginal_GetChildrenCount, Hook_GetChildrenCount, true,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::TextBlockGenerated::get_Text(struct HSTRING__ * *))"},
            &pOriginal_get_Text, Hook_get_Text, true,
        },
        {
            {LR"(public: virtual long __cdecl DirectUI::UIElementGenerated::put_Opacity(double))"},
            &pOriginal_put_Opacity, Hook_put_Opacity, true,
        },
        // Pointer converters, verbatim from the symbol dump. Each carries its
        // target IID internally, so no IID is guessed anywhere in this mod.
        {
            {LR"(long __cdecl ctl::do_query_interface<class DirectUI::FrameworkElement,struct IUnknown>(class DirectUI::FrameworkElement * &,struct IUnknown *))"},
            &pQI_FrameworkElement, Hook_QI_FE, true,
        },
        {
            {LR"(long __cdecl ctl::do_query_interface<struct Windows::UI::Xaml::Controls::ITextBlock,struct IUnknown>(struct Windows::UI::Xaml::Controls::ITextBlock * &,struct IUnknown *))"},
            &pQI_TextBlock, Hook_QI_TB, true,
        },
        {
            {LR"(long __cdecl ctl::do_query_interface<struct Windows::UI::Xaml::IDependencyObject,struct IInspectable>(struct Windows::UI::Xaml::IDependencyObject * &,struct IInspectable *))"},
            &pQI_DependencyObject, Hook_QI_DO, true,
        },
        {
            {LR"(private: static long __cdecl DirectUI::VisualTreeHelper::GetParentStaticPrivate(struct Windows::UI::Xaml::IDependencyObject *,unsigned char,struct Windows::UI::Xaml::IDependencyObject * *,unsigned char *,unsigned char *))"},
            &pGetParentStatic, Hook_GetParentStatic, true,
        },
    };

    if (!WindhawkUtils::HookSymbols(hXaml, windowsUiXamlDllHooks,
                                    ARRAYSIZE(windowsUiXamlDllHooks))) {
        Wh_Log(L"HookSymbols FAILED");
        return 0;
    }

    Wh_ApplyHookOperations();
    Wh_Log(L"hooks ACTIVE text=%p size=%p weight=%p", pOriginal_put_Text,
           pOriginal_put_FontSize, pOriginal_put_FontWeight);
    Wh_Log(L"  spacing=%p align=%p fontfamily=%p", pOriginal_put_CharacterSpacing,
           pOriginal_put_TextAlignment, pOriginal_put_FontFamily);
    Wh_Log(L"  tree: parent=%p child=%p count=%p getText=%p opacity=%p",
           pOriginal_get_Parent, pOriginal_GetChildStatic,
           pOriginal_GetChildrenCount, pOriginal_get_Text,
           pOriginal_put_Opacity);
    Wh_Log(L"  QI: fe=%p tb=%p do=%p parentStatic=%p", pQI_FrameworkElement,
           pQI_TextBlock, pQI_DependencyObject, pGetParentStatic);
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"=== INIT pid=%lu === (remember: wait ~1 min after compiling)",
           GetCurrentProcessId());

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_setupThread = CreateThread(nullptr, 0, SetupHookThread, nullptr, 0, nullptr);
    if (!g_setupThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    // Only flag the font for recreation. Do NOT call any XAML method here:
    // Windhawk invokes this on a background thread and XAML objects are
    // thread-affine, which produced RPC_E_WRONG_THREAD (0x8001010E). Both the
    // font and the new settings are picked up on the next clock tick, from
    // inside Hook_put_Text, where we are genuinely on the UI thread.
    g_fontAttempted = false;
    Wh_Log(L"settings updated; applied on the next minute tick");
}

void Wh_ModUninit() {
    // Without this, Windhawk waits on the setup thread and the mod gets stuck
    // showing "Uninitializing...".
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_setupThread) {
        WaitForSingleObject(g_setupThread, 2000);
        CloseHandle(g_setupThread);
        g_setupThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    if (g_fontFamily) {
        ComRelease(g_fontFamily);
        g_fontFamily = nullptr;
    }
    g_clockElement = nullptr;
    Wh_Log(L"=== UNINIT ===");
}
