// ==WindhawkMod==
// @id              lockscreen-datetime-format
// @name            Lock Screen Date and Time Format
// @description     Changes how the lock screen shows the time and date: vertical or horizontal clock, 12/24-hour, custom separator, and date formats
// @version         1.0
// @author          grn15RenS
// @github          https://github.com/grn15RenS
// @donateUrl       https://ko-fi.com/grn15rens
// @license         MIT
// @include         LockApp.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
 
// ==WindhawkModReadme==
/*
# Lock Screen Date and Time Format

Changes how the Windows lock screen shows the time and the date. The mod only
changes the **text** of the clock and the date. Fonts, sizes, colors and
positions are not touched (see [Styling](#styling) below).

Example (Windows default on the left, this mod on the right):

| Before | After |
|--------|-------|
| ![Before](https://i.imgur.com/vwH10YW.png) | ![After](https://i.imgur.com/2kZgtnm.png) |

The clock keeps updating on its own, and choosing the "Windows default"
options restores the original text.

## Settings

### Clock

- **Clock layout**
  - *Windows default*: the clock is left alone.
  - *Vertical*: hours, minutes (and seconds) each on their own line, no
    separator.
  - *Horizontal with custom separator*: everything on one line, joined by the
    separator below.
- **Line spacing (% of font size)**: distance between the lines of a vertical
  clock, as a percentage of the clock font size. Lower is tighter, higher is
  looser, `0` keeps the font's default. It only has an effect when the clock
  has more than one line. About `80` to `85` is a good starting point.
- **Separator (horizontal layout)**: the text placed between hours, minutes
  and seconds. It is plain text, so a colon, a dot, a dash, a space, or even an
  emoji all work.

  Example with an emoji as the separator:

  ![Clock with an emoji as the separator](https://i.imgur.com/HDPAl63.png)

- **Hour format**: follow the Windows setting, or force 24-hour or 12-hour.
- **Leading zero on hours**: show `09` or `9`.
- **Show seconds**: also show the seconds (`15:19:42`). The clock then updates
  every second instead of once a minute. With the vertical layout this gives
  three lines, so you will probably want a smaller font.
- **AM/PM position (12-hour only)**: beside the last unit shown, on its own
  line below, or hidden.

Digits are always shown as `0-9`, whatever the Windows language is.

**Note:** the line spacing, separator, hour format, leading zero, show seconds
and AM/PM options only take effect when **Clock layout** is *Vertical* or
*Horizontal*. With *Windows default* the clock is left completely alone.

### Date

- **Date format**: choose one of the presets (for example `Fri, 16 Oct`,
  `Fri 16 Oct 26`, `16-10-2026`, `16/10/2026`), keep the Windows default, or
  choose *Custom*.
- **Custom date pattern**: only used when the format is *Custom*. Supported
  letters: `d` / `dd` (day), `ddd` / `dddd` (weekday name), `M` / `MM` (month
  number), `MMM` / `MMMM` (month name), `yy` / `yyyy` (year). Literal text goes
  in single quotes, for example `dddd 'the' d`.

Weekday and month names come from your Windows regional format, so they follow
that language.

## Notes on the 12-hour format

AM/PM is placed after the last unit shown: the minutes, or the seconds when
**Show seconds** is on. The examples below assume seconds are off:

- **Vertical layout, AM/PM beside the minutes:** hours on the first line,
  minutes and AM/PM on the second line (`03` / `24 PM`).
- **Vertical layout, AM/PM below:** three lines (`03` / `24` / `PM`).
- **Horizontal layout, AM/PM beside the minutes:** everything on one line
  (`03:24 PM`). With a very large clock font the line can be wider than the
  space it has, and `PM` may wrap onto a second line. Use a smaller font, or
  set AM/PM to hidden, if that bothers you.
- **AM/PM hidden:** only the hours and minutes are shown.

## Styling

This mod does not change fonts, sizes, colors or positions. For that, use the
**Windows 11 Start Menu Styler** mod with `LockApp.exe` added to its process
inclusion list. For example:

```
- target: StackPanel#TimePanel > TextBlock#Time
  styles:
    - FontSize=350
    - FontWeight=Bold
- target: TextBlock#Date
  styles:
    - FontSize=28
```

Example with this mod and the Styler together:

![Lock screen using this mod together with Windows 11 Start Menu Styler](https://i.imgur.com/FobjRr6.png)

Making the clock font bigger usually moves the clock; adjust a
`TranslateTransform` in the Styler if it ends up off-center.

Lock the screen with **Win+L** to see the result. Changes in the settings
apply right away while the lock screen is showing.

## How it works

The lock screen (`LockApp.exe`) draws the time and the date in XAML
`TextBlock` elements named `Time` and `Date`. This mod finds them, builds the
text itself from the system clock according to your settings, and writes it
back whenever Windows updates them. Unloading the mod puts Windows' own text
back.

The width of the clock is also kept constant, so the date underneath does not
move sideways when the clock text gets wider or narrower (for example when the
seconds change).

## Known limitations

- The mod relies on the internal element names `Time` and `Date` in
  `LockApp.exe`. If a future Windows update renames them, the mod will stop
  finding them.
- Digits are always Latin (`0-9`). Showing the digits of your own language is
  not supported yet.
- The AM/PM text cannot be made smaller than the clock yet.
- Most fonts have digits of different widths, so with **Show seconds** on, the
  digits inside the clock can shift slightly every second. The date and the
  position of the clock stay where they are. Pick a font with equal-width
  digits in the Styler if that bothers you.
- Tested on Windows 11 25H2 (build: 26200.9457). Other versions are untested.

## Troubleshooting

If nothing happens, open the Windhawk log for this mod and lock the screen
with Win+L. You should see `Hook installed on UI thread`, `Clock format
attached` and `Date format attached`.

## Other Screenshots

| | |
|---|---|
| ![otherscreenshot1](https://i.imgur.com/Y6zjDz5.png) | ![otherscreenshot4](https://i.imgur.com/9Dg3G9G.png) |
| ![otherscreenshot2](https://i.imgur.com/mfyVyFZ.png) | ![otherscreenshot5](https://i.imgur.com/K0he91e.png) |
| ![otherscreenshot3](https://i.imgur.com/NN7bGfb.png) | ![otherscreenshot6](https://i.imgur.com/aORwvfu.png) |
*/
// ==/WindhawkModReadme==
 
// ==WindhawkModSettings==
/*
- clockLayout: horizontal
  $name: Clock layout
  $options:
  - default: Windows default (single line)
  - vertical: Vertical (hours above minutes)
  - horizontal: Horizontal with custom separator
- lineHeightPercent: 85
  $name: Line spacing (% of font size)
  $description: Distance between the lines of a multi-line clock, as a percentage of the clock font size. Only used when the clock has more than one line. Lower is tighter, higher is looser. 0 = font default.
- clockSeparator: ":"
  $name: Separator (horizontal layout)
  $description: Text between hours and minutes in the horizontal layout, for example a colon, a dot, or a single space.
- hourFormat: system
  $name: Hour format
  $options:
  - system: Follow Windows
  - h24: 24-hour
  - h12: 12-hour (with AM/PM)
- leadingZero: true
  $name: Leading zero on hours
  $description: Show 09 instead of 9.
- showSeconds: false
  $name: Show seconds
  $description: Also show the seconds. The clock then updates every second. Only used when Clock layout is Vertical or Horizontal.
- ampmPosition: beside
  $name: AM/PM position (12-hour only)
  $options:
  - beside: Beside the last unit shown
  - below: On its own line below
  - hidden: Hidden
- dateFormat: default
  $name: Date format
  $options:
  - default: Friday, 16 October (Windows default)
  - shortComma: Fri, 16 Oct
  - shortNoComma: Fri 16 Oct
  - longYY: Friday, 16 Oct 26
  - longYYYY: Friday, 16 October 2026
  - shortCommaYY: Fri, 16 Oct 26
  - shortNoCommaYY: Fri 16 Oct 26
  - dashes: 16-10-2026
  - slashes: 16/10/2026
  - custom: Custom (uses the pattern below)
- customDateFormat: "ddd, d MMM"
  $name: Custom date pattern
  $description: "Only used when Date format is set to Custom. Pattern letters: d, dd (day), ddd, dddd (weekday), M, MM (month number), MMM, MMMM (month name), yy, yyyy (year). Put literal text in single quotes."
*/
// ==/WindhawkModSettings==
 
#undef GetCurrentTime
 
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
 
#include <atomic>
#include <mutex>
#include <string>
#include <limits>
 
using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
 
// The clock TextBlock and the property-changed callback token.
// Only touched from the UI thread.
static Controls::TextBlock g_orig{nullptr};
static int64_t g_cbToken = 0;
 
static std::atomic<bool> g_stop{false};
static HANDLE g_thread = nullptr;
static int g_lineHeight = 0;  // line spacing, in percent of the font size
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_detached{false};
static HHOOK g_hook = nullptr;
static HWND g_uiWnd = nullptr;
static ULONGLONG g_lastTick = 0;
static std::atomic<bool> g_dirty{false};  // settings changed, re-apply on UI thread
static std::atomic<bool> g_showSeconds{false};
static bool g_updating = false;           // guards against re-entering our own Text change
// Date TextBlock (x:Name="Date") state. Only touched from the UI thread.
static Controls::TextBlock g_date{nullptr};
static int64_t g_dateToken = 0;
static std::wstring g_dateOrig;  // last date text written by Windows
static std::wstring g_dateOut;   // last date text written by this mod
static bool g_dateUpdating = false;

// Date settings. Written from the settings thread, read from the UI thread.
static std::mutex g_settingsMutex;
static std::wstring g_dateFormat = L"default";
static std::wstring g_customDate;
// Clock state. Only touched from the UI thread.
static std::wstring g_clockOrig;  // last clock text written by Windows
static std::wstring g_clockOut;   // last clock text written by this mod

// Clock settings. Written from the settings thread, read from the UI thread
// (guarded by g_settingsMutex).
static std::wstring g_clockLayout = L"vertical";
static std::wstring g_hourFormat = L"system";
static std::wstring g_clockSeparator = L":";
static std::wstring g_ampmPosition = L"beside";
static bool g_leadingZero = true;
 
static std::wstring FormatNumber(int v, bool pad) {
    std::wstring s = std::to_wstring(v);
    if (pad && s.size() < 2) s = L"0" + s;
    return s;
}

// Builds the clock text from the system clock, so it doesn't depend on how
// Windows formats it for the current locale. Empty = keep Windows' own text.
static std::wstring BuildClock() {
    std::wstring layout, hourFmt, sep, ampmPos;
    bool leadingZero;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        layout = g_clockLayout;
        hourFmt = g_hourFormat;
        sep = g_clockSeparator;
        ampmPos = g_ampmPosition;
        leadingZero = g_leadingZero;
    }
    if (layout != L"vertical" && layout != L"horizontal") return L"";

    SYSTEMTIME st;
    GetLocalTime(&st);

    bool use12 = false;
    if (hourFmt == L"h12") {
        use12 = true;
    } else if (hourFmt != L"h24") {
        // Follow Windows: LOCALE_ITIME is 0 for 12-hour and 1 for 24-hour
        DWORD v = 0;
        int n = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                                LOCALE_ITIME | LOCALE_RETURN_NUMBER,
                                reinterpret_cast<LPWSTR>(&v),
                                sizeof(v) / sizeof(wchar_t));
        use12 = (n > 0 && v == 0);
    }

    int hour = st.wHour;
    std::wstring suffix;
    if (use12) {
        bool pm = hour >= 12;
        hour %= 12;
        if (hour == 0) hour = 12;
        if (ampmPos != L"hidden") {
            wchar_t buf[32] = L"";
            int n = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                                    pm ? LOCALE_SPM : LOCALE_SAM, buf, 32);
            if (n > 0) suffix = buf;
            else suffix = pm ? L"PM" : L"AM";
        }
    }

    // Digits are always Latin (0-9) regardless of the locale
    std::wstring text = FormatNumber(hour, leadingZero);
    if (layout == L"vertical") text += L"\n";
    else text += sep;
    text += FormatNumber(st.wMinute, true);

    if (g_showSeconds) {
        if (layout == L"vertical") text += L"\n";
        else text += sep;
        text += FormatNumber(st.wSecond, true);
    }

    if (!suffix.empty()) {
        if (ampmPos == L"below") text += L"\n";
        else text += L" ";
        text += suffix;
    }
    return text;
}

// Keeps the clock's width constant. Digits in most fonts have different
// widths, so the text would get wider and narrower as the time changes and
// pull the date along with it. We measure the widest possible text (every
// digit replaced by the widest digit of the font) and use that as a minimum
// width for the clock.
static std::wstring g_widthKey;  // what the last measurement was based on

static double MeasureText(Controls::TextBlock const& ref, std::wstring const& text) {
    Controls::TextBlock tb;
    tb.FontFamily(ref.FontFamily());
    tb.FontSize(ref.FontSize());
    tb.FontWeight(ref.FontWeight());
    tb.FontStyle(ref.FontStyle());
    tb.CharacterSpacing(ref.CharacterSpacing());
    tb.Text(hstring(text));
    float inf = std::numeric_limits<float>::infinity();
    tb.Measure(winrt::Windows::Foundation::Size{inf, inf});
    return tb.DesiredSize().Width;
}

static void StabilizeClockWidth(std::wstring const& text) {
    if (!g_orig) return;

    // Only measure again when the text shape or the font changed
    std::wstring shape = text;
    for (auto& c : shape) if (c >= L'0' && c <= L'9') c = L'#';
    std::wstring key = shape + L"|" +
                       std::wstring(g_orig.FontFamily().Source().c_str()) + L"|" +
                       std::to_wstring(g_orig.FontSize()) + L"|" +
                       std::to_wstring(g_orig.FontWeight().Weight) + L"|" +
                       std::to_wstring(g_orig.CharacterSpacing());
    if (key == g_widthKey) return;

    wchar_t widest = L'0';
    double best = -1;
    for (wchar_t d = L'0'; d <= L'9'; d++) {
        double w = MeasureText(g_orig, std::wstring(4, d));
        if (w > best) { best = w; widest = d; }
    }

    std::wstring wide = text;
    for (auto& c : wide) if (c >= L'0' && c <= L'9') c = widest;

    double width = MeasureText(g_orig, wide);
    g_orig.MinWidth(width);
    g_widthKey = key;
    Wh_Log(L"Clock minimum width set to %d px", (int)width);
}

static void ResetClockWidth() {
    if (!g_orig || g_widthKey.empty()) return;
    g_orig.MinWidth(0);
    g_widthKey.clear();
}

// Writes the formatted clock into the Time TextBlock. Must run on the UI thread.
static void ApplyClock() {
    if (!g_orig || g_updating) return;
    g_updating = true;
    try {
        std::wstring cur = g_orig.Text().c_str();
        if (cur != g_clockOut) g_clockOrig = cur;  // Windows wrote this text

        std::wstring want = BuildClock();
        bool formatted = !want.empty();
        if (!formatted) want = g_clockOrig;        // "Windows default"

        if (!want.empty() && want != cur) {
            g_orig.Text(hstring(want));
            g_clockOut = want;

            if (!g_showSeconds) {
                std::wstring shown = want;
                for (auto& c : shown) if (c == L'\n') c = L'/';
                Wh_Log(L"Clock set to: %s", shown.c_str());
            }
        }
        if (formatted) StabilizeClockWidth(want);
        else ResetClockWidth();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to set clock: %s", e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to set clock: unknown error");
    }
    g_updating = false;
}
 
// Finds a TextBlock by its x:Name in the visual tree.
static Controls::TextBlock FindTextBlock(DependencyObject node, hstring const& name) {
    if (auto tb = node.try_as<Controls::TextBlock>()) {
        if (tb.Name() == name) return tb;
    }
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto r = FindTextBlock(VisualTreeHelper::GetChild(node, i), name);
        if (r) return r;
    }
    return nullptr;
}
 
// Stops watching the clock and puts Windows' own text back.
static void Detach() {
    try {
        if (g_orig) {
            g_orig.UnregisterPropertyChangedCallback(Controls::TextBlock::TextProperty(), g_cbToken);
            g_orig.LineHeight(0);
            g_orig.LineStackingStrategy(LineStackingStrategy::MaxHeight);
            if (!g_clockOrig.empty()) g_orig.Text(hstring(g_clockOrig));
            g_orig.MinWidth(0);
            g_widthKey.clear();
        }
    } catch (...) {}
    g_orig = nullptr;
    g_clockOut.clear();
}

// Finds the clock and starts watching it. Must run on the UI thread.
static void Attach() {
    auto content = Window::Current().Content();
    if (!content) return;

    // Already attached and the element is still in the tree
    if (g_orig) {
        if (VisualTreeHelper::GetParent(g_orig)) return;
        Detach();
    }

    auto orig = FindTextBlock(content, L"Time");
    if (!orig) return;

    g_orig = orig;
    g_clockOrig = orig.Text().c_str();
    g_clockOut.clear();

    // Re-apply the format every time Windows rewrites the clock text
    g_cbToken = orig.RegisterPropertyChangedCallback(
        Controls::TextBlock::TextProperty(),
        [](DependencyObject const&, DependencyProperty const&) { ApplyClock(); });

    orig.TextAlignment(TextAlignment::Center);
    g_dirty = true;  // apply line spacing from the hook
    ApplyClock();
    Wh_Log(L"Clock format attached");
}
 
// Applies the line spacing setting. Must run on the UI thread.
static void ApplyLineHeight() {
    if (!g_orig) return;
    try {
        bool multiLine = g_clockOut.find(L'\n') != std::wstring::npos;
        if (g_lineHeight > 0 && multiLine) {
            double h = g_orig.FontSize() * g_lineHeight / 100.0;
            g_orig.LineStackingStrategy(LineStackingStrategy::BlockLineHeight);
            g_orig.LineHeight(h);
            Wh_Log(L"Line spacing applied: %d%% (%d px)", g_lineHeight, (int)h);
        } else {
            g_orig.LineHeight(0);
            g_orig.LineStackingStrategy(LineStackingStrategy::MaxHeight);
            Wh_Log(L"Line spacing reset to font default");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to set line spacing: %s", e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to set line spacing: unknown error");
    }
}
 
static std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring r = s ? s : L"";
    if (s) Wh_FreeStringSetting(s);
    return r;
}

static void LoadSettings() {
    g_lineHeight = Wh_GetIntSetting(L"lineHeightPercent");

    std::wstring dateFmt = GetStringSetting(L"dateFormat");
    std::wstring customDate = GetStringSetting(L"customDateFormat");
    std::wstring layout = GetStringSetting(L"clockLayout");
    std::wstring hourFmt = GetStringSetting(L"hourFormat");
    std::wstring sep = GetStringSetting(L"clockSeparator");
    std::wstring ampmPos = GetStringSetting(L"ampmPosition");
    bool leadingZero = Wh_GetIntSetting(L"leadingZero") != 0;
    g_showSeconds = Wh_GetIntSetting(L"showSeconds") != 0;    

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_dateFormat = dateFmt;
    g_customDate = customDate;
    g_clockLayout = layout;
    g_hourFormat = hourFmt;
    g_clockSeparator = sep;
    g_ampmPosition = ampmPos;
    g_leadingZero = leadingZero;
}

// Returns the GetDateFormatEx pattern for the selected preset,
// or an empty string for "Windows default".
static std::wstring GetDatePattern() {
    std::wstring key, custom;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        key = g_dateFormat;
        custom = g_customDate;
    }
    if (key == L"shortComma")     return L"ddd, d MMM";
    if (key == L"shortNoComma")   return L"ddd d MMM";
    if (key == L"longYY")         return L"dddd, d MMM yy";
    if (key == L"longYYYY")       return L"dddd, d MMMM yyyy";
    if (key == L"shortCommaYY")   return L"ddd, d MMM yy";
    if (key == L"shortNoCommaYY") return L"ddd d MMM yy";
    if (key == L"dashes")         return L"dd-MM-yyyy";
    if (key == L"slashes")        return L"dd/MM/yyyy";
    if (key == L"custom")         return custom;
    return L"";
}

// Builds today's date text from the system clock. Empty = keep Windows' text.
static std::wstring BuildDate() {
    std::wstring pattern = GetDatePattern();
    if (pattern.empty()) return L"";

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t buf[128];
    int n = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pattern.c_str(),
                            buf, 128, nullptr);
    if (n <= 0) return L"";
    return buf;
}

// Writes the formatted date into the Date TextBlock. Must run on the UI thread.
static void ApplyDate() {
    if (!g_date || g_dateUpdating) return;
    g_dateUpdating = true;
    try {
        std::wstring cur = g_date.Text().c_str();
        if (cur != g_dateOut) g_dateOrig = cur;  // Windows wrote this text

        std::wstring want = BuildDate();
        if (want.empty()) want = g_dateOrig;     // "Windows default"

        if (!want.empty() && want != cur) {
            g_date.Text(hstring(want));
            g_dateOut = want;
            Wh_Log(L"Date set to: %s", want.c_str());
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to set date: %s", e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to set date: unknown error");
    }
    g_dateUpdating = false;
}

// Stops watching the date and puts Windows' own text back.
static void DetachDate() {
    try {
        if (g_date) {
            g_date.UnregisterPropertyChangedCallback(Controls::TextBlock::TextProperty(), g_dateToken);
            if (!g_dateOrig.empty()) g_date.Text(hstring(g_dateOrig));
        }
    } catch (...) {}
    g_date = nullptr;
    g_dateOut.clear();
}

// Finds the date element and starts watching it. Must run on the UI thread.
static void AttachDate() {
    auto content = Window::Current().Content();
    if (!content) return;

    if (g_date) {
        if (VisualTreeHelper::GetParent(g_date)) return;
        DetachDate();
    }

    auto d = FindTextBlock(content, L"Date");
    if (!d) return;

    g_date = d;
    g_dateOrig = d.Text().c_str();
    g_dateOut.clear();
    g_dateToken = d.RegisterPropertyChangedCallback(
        Controls::TextBlock::TextProperty(),
        [](DependencyObject const&, DependencyProperty const&) { ApplyDate(); });

    ApplyDate();
    Wh_Log(L"Date format attached");
}

// WH_GETMESSAGE hook procedure. It runs on the lock screen's UI thread, which is
// the only place where it is safe to touch XAML objects.
static LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        if (g_unloading) {
            if (!g_detached) {
                try { Detach(); } catch (...) {}
                try { DetachDate(); } catch (...) {}
                g_detached = true;
            }
        } else {
            bool dirty = g_dirty.exchange(false);
            ULONGLONG now = GetTickCount64();
            ULONGLONG interval = g_showSeconds ? 200 : 1000;
            if (dirty || now - g_lastTick > interval) {
                g_lastTick = now;
                try {
                    Attach();
                    AttachDate();
                    ApplyClock();
                    ApplyDate();
                    if (dirty) ApplyLineHeight();
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"Hook error: %s", e.message().c_str());
                } catch (...) {
                    Wh_Log(L"Hook error: unknown");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
 
// Looks for the lock screen's CoreWindow in this process.
static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
 
    wchar_t cls[256] = L"";
    GetClassNameW(hwnd, cls, 256);
 
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}
 
static DWORD WINAPI WorkerThread(void*) {
    // Wait for the UI thread to show up, then install the hook on it
    while (!g_stop && !g_hook) {
        HWND found = nullptr;
        EnumWindows(EnumWndProc, reinterpret_cast<LPARAM>(&found));
        if (found) {
            DWORD tid = GetWindowThreadProcessId(found, nullptr);
            g_hook = SetWindowsHookExW(WH_GETMESSAGE, GetMsgProc, nullptr, tid);
            if (g_hook) {
                g_uiWnd = found;
                Wh_Log(L"Hook installed on UI thread %u", tid);
            } else {
                Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
            }
        }
        if (!g_hook) Sleep(2000);
    }
    // Periodically wake the UI thread so the hook is guaranteed to run
    while (!g_stop) {
        if (g_uiWnd) PostMessageW(g_uiWnd, WM_NULL, 0, 0);
        Sleep(g_showSeconds ? 250 : 1000);
    }
    return 0;
}
 
BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = false;
    g_unloading = false;
    g_detached = false;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return TRUE;
}
 
void Wh_ModUninit() {
    g_stop = true;
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_hook) {
        // Ask the UI thread to restore the original clock, then remove the hook
        g_unloading = true;
        if (g_uiWnd) PostMessageW(g_uiWnd, WM_NULL, 0, 0);
        for (int i = 0; i < 20 && !g_detached; i++) Sleep(50);
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
}
 
void Wh_ModSettingsChanged() {
    LoadSettings();
    g_dirty = true;
}
