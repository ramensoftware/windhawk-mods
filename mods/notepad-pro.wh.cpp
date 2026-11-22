// ==WindhawkMod==
// @id              notepad-pro
// @name            Notepad Pro
// @description     A collection of useful tweaks, tools, and upgrades for Windows 11 Notepad
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         notepad.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32 -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notepad Pro

![Notepad Pro](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/Don't%20Touch/NP.png)

Upgrade the standard Windows Notepad with modern code-editor features while keeping it fast and lightweight.

## Line-Number Gutter

Adds a clean line-number margin to the left side of the editor.

* `Smart wrapping`: Only displays numbers on the first row of wrapped lines.
* `Dynamic sizing`: Automatically adjusts its width as your document grows.
* `Theme-aware`: Seamlessly adapts to light and dark modes in real time.
* `Editor sync`: Matches your current font, size, zoom level, and display scaling, with highlighting for the active line.
* `Markers`: Colors the left edge of the gutter next to every line you changed, added, or removed.

## Minimap

Adds a high-level visual overview of your entire file right alongside the scrollbar.

* `Visual preview`: Represents your text as clean, color-coded blocks for easy scanning.
* `Smooth navigation`: Click anywhere to jump directly to that section, drag the viewport slider, or scroll smoothly with your mouse wheel.
* `Responsive layout`: Automatically scales with the window size and hides itself when the window becomes too narrow.
* `Universal support`: Works across all open documents and file types.

## Syntax Highlighting

Automatically brings color and readability to popular programming and data files based on the open file type.

* `Rich color coding`: Highlights keywords, types, strings, numbers, and directives while keeping plain text and unsaved tabs clean.
* `Non-destructive`: Purely visual, never alters your underlying files.
* `Theme-aware`: Uses tailored color palettes that automatically switch between light and dark themes.

> Note: Very large files may cause slight delays, slower rendering, or higher memory usage when calculating line numbers, drawing the minimap, or running syntax highlighting.

### More Features Soon

This mod is under active development, with additional quality-of-life improvements and editor tools planned for future updates.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lineNumberSettings:
  - HighlightStyle: none
    $name: Current line highlight style
    $description: Extra styling for the current line number.
    $options:
      - none: None
      - underline: Underline
      - bold: Bold
      - both: Underline and bold
  $name: Line-Number Settings
  $description: To show the line numbers on Notepad, right-click the Notepad's tab bar and check Line Numbers.
- minimapSettings:
  - MinimapOnlyWithScrollbar: false
    $name: Only show the minimap when the vertical scrollbar appears
    $description: When enabled, it shows only while the document is long enough to need a vertical scrollbar.
  - ClickBehavior: line
    $name: Minimap click behavior
    $description: What happens when you click (not drag) the minimap. Dragging the slider always scrolls proportionally.
    $options:
      - line: Jump to the clicked line
      - proportional: Jump to the same percentage of the file
  $name: Minimap Settings
  $description: To show the minimap on Notepad, right-click the Notepad's tab bar and check Minimap.
*/
// ==/WindhawkModSettings==

#define _RICHEDIT_VER 0x0800
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include <dwmapi.h>

#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cwctype>  // [windhawk-reviewer] iswalpha / iswalnum / towlower: include their header directly
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <windhawk_utils.h>  // [windhawk-reviewer] WindhawkUtils::SetFunctionHook / StringSetting

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// ---------------------------------------------------------------------------
// Constants / globals
// ---------------------------------------------------------------------------

constexpr WCHAR kGutterClass[] = L"WhNotepadProGutter";
constexpr WCHAR kMinimapClass[] = L"WhNotepadProMinimap";
constexpr WCHAR kPropEditor[] = L"WhNotepadPro.Editor";
constexpr WCHAR kPropTop[] = L"WhNotepadPro.Top";
constexpr UINT_PTR kEditSubclassId = 0x4E505245;
constexpr UINT_PTR kTopSubclassId = 0x4E505254;
constexpr UINT kSysCmdId = 0xA1F0;           // WM_SYSCOMMAND id (low 4 bits must be 0)
constexpr UINT_PTR kTickTimer = 1;           // runs on the gutter window (16 ms while busy, 250 ms idle)
// [windhawk-reviewer] kDiscoverTimer (a 750 ms EnumChildWindows rescan of every Notepad window) was
// removed on purpose: the CreateWindowExW hook and the initial enumeration in AttachTop already
// find every editor. Don't bring the poll back without a case those two miss.
// [windhawk-reviewer] Adaptive tick rate: the per-editor timer used to be a fixed 16 ms (60 Hz) for as
// long as Notepad was open. See SetTickRate() / WakeTick() / SettleTickRate().
constexpr UINT kTickFastMs = 16;          // typing, scrolling, dragging, or something still settling
constexpr UINT kTickSlowMs = 250;         // idle backstop (also background tabs and minimized windows)
constexpr ULONGLONG kTickGraceMs = 300;   // stay fast this long after the last wake-up event
constexpr ULONGLONG kTickStartMs = 1000;  // ... and this long after an editor was attached
constexpr WCHAR kMenuText[] = L"Line Numbers";
constexpr UINT kSysCmdMinimapId = 0xA1E0;    // WM_SYSCOMMAND id for the minimap toggle
constexpr WCHAR kMenuTextMinimap[] = L"Minimap";

// Editor background, only used if copying it from the screen fails (see PaintGutter).
constexpr COLORREF kEditorGuessDark = RGB(0x27, 0x27, 0x27);
constexpr COLORREF kEditorGuessLight = RGB(0xF9, 0xF9, 0xF9);

// Change markers
constexpr size_t kMkMaxChars = 2000000;  // no markers for documents bigger than this
constexpr int kMkBarDip = 3;             // thickness of the colored bar at the gutter's left edge
constexpr int kMkStripDip = 6;           // gutter width when only the markers are shown
constexpr ULONGLONG kMkFollowMs = 1500;  // after a document appears its baseline follows the text
                                         // for this long (loading), unless the user edits first

// Minimap tuning
constexpr double kMmWidthRatio = 0.10;  // width as a fraction of the editor's client width
constexpr int kMmMinDip = 60;           // ... but never narrower than this (DIPs)
constexpr int kMmMaxDip = 160;          // ... or wider than this
constexpr int kMmStepDip = 4;           // width changes in steps of this size (fewer relayouts)
constexpr int kMmMinTextDip = 300;      // hide the minimap if less than this is left for text
constexpr int kMmCols = 100;            // text columns that span the minimap's full width

struct Palette {
    COLORREF bg, text, current, separator;
    COLORREF mkAdd, mkMod, mkDel;  // change markers: added / modified / removed lines
};
constexpr Palette kDark = {RGB(0x2C, 0x2C, 0x2C), RGB(0x85, 0x85, 0x85),
                           RGB(0xFF, 0xFF, 0xFF), RGB(0x3E, 0x3E, 0x3E),
                           RGB(0x60, 0xAA, 0x60), RGB(0xE0, 0x96, 0x3C), RGB(0xE0, 0x5C, 0x5C)};
constexpr Palette kLight = {RGB(0xF8, 0xF8, 0xF8), RGB(0x8A, 0x8A, 0x8A),
                            RGB(0x00, 0x00, 0x00), RGB(0xE2, 0xE2, 0xE2),
                            RGB(0x34, 0xA0, 0x48), RGB(0xDC, 0x82, 0x14), RGB(0xD2, 0x3C, 0x3C)};

std::atomic<bool> g_enabled{true};
std::atomic<bool> g_minimapEnabled{true};
std::atomic<bool> g_mmOnlyScroll{false};  // setting: show the minimap only if a vscroll is needed
std::atomic<bool> g_mmClickLine{true};    // setting: a click jumps to the line under the cursor
std::atomic<int> g_themeEpoch{0};
std::atomic<int> g_highlightStyle{0};  // 0 none, 1 underline, 2 bold, 3 both
std::atomic<bool> g_unloading{false};
UINT g_runMsg = 0;

// [windhawk-reviewer] window classes are registered with the mod's own module (not the exe), and the
// gutter / minimap windows are only created when their class registered (see Wh_ModInit). A stale class
// left by an earlier instance of the mod would otherwise be picked up by CreateWindowExW, and its
// window procedure points into an unloaded image.
HINSTANCE g_hInst = nullptr;
std::atomic<bool> g_gutterClassOk{false};
std::atomic<bool> g_minimapClassOk{false};

std::mutex g_mutex;  // guards g_editors, g_tops

// Cache for "how many paragraph breaks precede character index N".
struct CrCache {
    bool valid = false;
    LONG pos = 0;
    LONG count = 0;
};

// One visual row of the editor, as last measured (client pixels).
struct SnapRow {
    LONG idx;  // first character index
    LONG end;  // one past the last character of the row
    int y;     // row top
    int x;     // x of the first character
};

// What the minimap last painted, so it only repaints when something visible changed.
struct MmSig {
    double top = 0, view = 0;
    LONG nLines = 0;
    unsigned rev = 0;
    int w = 0, h = 0;
    bool dark = false, hot = false;
};

static bool SigSame(const MmSig& a, const MmSig& b) {
    return a.top == b.top && a.view == b.view && a.nLines == b.nLines && a.rev == b.rev &&
           a.w == b.w && a.h == b.h && a.dark == b.dark && a.hot == b.hot;
}

struct EditorCtx {
    HWND edit = nullptr;
    HWND gutter = nullptr;
    bool active = false;  // line numbers on: left margin reserved
    bool addedClipSiblings = false;
    bool captured = false;  // origMargins / clip style captured (shared by gutter + minimap)
    DWORD origMargins = 0;
    bool rectFallback = false;  // EM_SETMARGINS ignored -> use EM_SETRECT
    int insetFailures = 0;
    int gutterW = 0;

    HFONT font = nullptr;
    HFONT fontBold = nullptr;
    WCHAR face[LF_FACESIZE] = {};
    int fontPx = 0;
    bool textColorKnown = false;
    COLORREF textColor = 0;

    bool dark = false;
    int themeEpoch = -1;
    int tickCount = 0;
    bool inTick = false;

    // [windhawk-reviewer] the always-0 `textMode` was removed together with its dead EM_GETTEXTRANGE
    // paths (ReadText / CountBreaksText): line breaks are counted from the row geometry only.

    bool textDirty = true;
    LONG textLen = -1, lineCount = -1;
    ULONGLONG lastChangeTick = 0;
    LONG totalLogical = 1;
    CrCache viewCache, caretCache;

    bool sigValid = false;
    LONG sFirst = 0;
    POINT sScroll = {};
    DWORD sSelS = 0, sSelE = 0;
    int sW = 0, sH = 0;
    RECT lastPlacement = {};

    // Syntax highlighting
    DWORD uiThread = 0;
    bool shown = false;       // visible editor (set by Tick)
    int lang = -2;            // -2 unknown, -1 none, >=0 index into Langs()
    int synTick = 0;
    bool docStale = true;
    std::wstring docText;     // document copy, same character indexes as the editor
    std::vector<uint8_t> cls; // SynClass per character
    std::vector<SnapRow> snap;
    int snapPitch = 18;
    double baseOff = 0;       // baseline offset below a row's top, px
    bool offValid = false;

    // Minimap
    HWND minimap = nullptr;
    bool mmActive = false;  // minimap on: right margin reserved, window shown
    int mmW = 0;
    RECT mmPlacement = {};
    bool mmHot = false, mmDrag = false, mmTracking = false;
    int mmGrab = 0;               // px from the slider's top to the mouse while dragging
    bool mmDragPending = false;   // a drag position waits to be applied by the next timer tick
    int mmDragY = 0;              // ... the latest mouse y (minimap client coordinates)
    double mmDragV = 0;           // viewport size (lines) frozen for the duration of a drag
    LONG mmEndRows = -1, mmEndY = -1;  // editor state after the last "scroll to end" request
    int mmEndH = -1;
    bool docOk = false;           // docText is a valid copy of the document
    unsigned docRev = 0;          // bumped whenever docText is replaced or cleared
    unsigned mmBuiltRev = ~0u;    // docRev that mmStart was built from
    std::vector<uint32_t> mmStart;  // start index (in docText) of every logical line
    bool mmSigValid = false;
    MmSig mmSig;
    bool inMsg = false;  // inside a tick that was triggered by an editor message (not the timer)
    int settle = 0;      // timer ticks left in which the gutter re-checks after a text edit

    // Change markers
    bool numsShown = true;      // line numbers drawn (false: only the marker strip is shown)
    bool mkOn = false;          // markers active for the current document
    bool mkNameOk = false;      // the title names a saved file (not an "Untitled" tab)
    bool mkNameKnown = false;
    std::wstring mkName, mkPendingName;
    bool mkHave = false;        // a baseline exists
    bool mkFrozen = false;      // the baseline no longer follows the text
    bool mkEmptyBase = false;   // the document was empty when the baseline froze: no markers
    bool mkUserEdit = false;    // the user typed / pasted / undid something
    bool mkReload = false;      // the document was replaced (loaded)
    ULONGLONG mkFollowUntil = 0;
    std::wstring mkBase;
    std::vector<uint64_t> mkBaseHash;
    std::vector<uint8_t> mkState;   // per current line: 0 unchanged, 1 added, 2 modified
    std::vector<uint32_t> mkDel;    // sorted: lines were removed right before these line indices
    unsigned mkRev = ~0u;           // docRev the markers were computed for
    bool mkDirty = false;           // the markers changed: the gutter needs a repaint
    bool mkExactFailed = false;     // [windhawk-reviewer] an expensive exact diff gave up for this baseline
    int mkExactSkips = 0;           // [windhawk-reviewer] runs since then (every 32nd retries)

    int synSettle = 0;       // timer ticks left in which the syntax row snapshot is re-checked
    uint64_t snapHash = 0;   // fingerprint of the snapshot, to see whether a re-check changed it
    int docRetry = 0;        // failed document reads retried since the last text change

    // Syntax tracking (independent of the line-number gutter)
    LONG synLen = -1, synLines = -1;
    LONG docSynLen = -2;  // synLen at the time docText was read (staleness check)
    ULONGLONG synChangeTick = 0;
    bool synSigValid = false;
    LONG synFirst = 0;
    POINT synScroll = {};
    DWORD synSelS = 0, synSelE = 0;
    int synW = 0, synH = 0;

    // [windhawk-reviewer] row-geometry calibration: these used to be shared globals (g_cal*) that every
    // thread painting an editor wrote without synchronization; now per editor (keyed by pixel font size).
    int calKey = 0;
    int calPitch = 0;   // distance between visual rows, px
    int calOrigin = 0;  // y of row 0 when not scrolled, px
    bool calPitchValid = false;
    bool calOriginValid = false;

    // [windhawk-reviewer] adaptive tick rate, see SetTickRate()
    UINT tickMs = 0;          // current period of kTickTimer in ms (0 = timer not running)
    ULONGLONG fastUntil = 0;  // GetTickCount64() until which the timer stays fast after a wake-up
};

struct TopCtx {
    HWND hwnd = nullptr;
};

std::vector<EditorCtx*> g_editors;
std::vector<TopCtx*> g_tops;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

struct RunParam {
    void (*proc)(void*);
    void* arg;
};

static LRESULT CALLBACK RunHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runMsg) {
            auto* p = (RunParam*)cwp->lParam;
            p->proc(p->arg);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs proc on the thread that owns hwnd (needed for SetWindowSubclass etc.).
static bool RunFromWindowThread(HWND hwnd, void (*proc)(void*), void* arg) {
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        proc(arg);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunHookProc, nullptr, tid);
    if (!hook) return false;
    RunParam p{proc, arg};
    SendMessageW(hwnd, g_runMsg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static bool IsRichEditName(PCWSTR name) {
    return name && _wcsnicmp(name, L"RichEdit", 8) == 0;  // RichEditD2DPT, RICHEDIT50W, ...
}

static int CountDigits(LONG n) {
    int d = 1;
    while (n >= 10) {
        n /= 10;
        d++;
    }
    return d;
}

static bool SystemAppsUseDark() {
    // ShouldAppsUseDarkMode is uxtheme ordinal 132 (undocumented).
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (ux) {
        using Fn = bool(WINAPI*)();
        auto fn = (Fn)GetProcAddress(ux, MAKEINTRESOURCEA(132));
        if (fn) return fn();
    }
    DWORD v = 1, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v == 0;
}

// Order: window's immersive-dark attribute (Notepad's own theme), editor text
// color, then the system "apps" theme.
static bool IsDarkTheme(EditorCtx* c) {
    HWND top = GetAncestor(c->edit, GA_ROOT);
    BOOL dwmDark = FALSE;
    if (top && SUCCEEDED(DwmGetWindowAttribute(top, DWMWA_USE_IMMERSIVE_DARK_MODE, &dwmDark,
                                               sizeof(dwmDark))) &&
        dwmDark) {
        return true;
    }
    if (c->textColorKnown) {
        int lum = (30 * GetRValue(c->textColor) + 59 * GetGValue(c->textColor) +
                   11 * GetBValue(c->textColor)) / 100;
        return lum >= 128;  // light text => dark editor
    }
    return SystemAppsUseDark();
}

// ---------------------------------------------------------------------------
// Editor queries
// ---------------------------------------------------------------------------

// Caret/char position in editor client coordinates. Handles both RichEdit
// (POINTL via wParam) and Edit-style (packed return value) behavior.
static bool PosFromChar(HWND edit, LONG idx, POINT* out) {
    POINTL pt = {LONG_MIN, LONG_MIN};
    LRESULT r = SendMessageW(edit, EM_POSFROMCHAR, (WPARAM)&pt, idx);
    if (pt.x != LONG_MIN) {
        out->x = pt.x;
        out->y = pt.y;
        return true;
    }
    if (r == -1) return false;
    out->x = (short)LOWORD(r);
    out->y = (short)HIWORD(r);
    return true;
}

// [windhawk-reviewer] ReadText() removed: it was only used by the dead CountBreaksText().

static bool IsBreakChar(WCHAR ch) {
    return ch == L'\r' || ch == L'\n' || ch == 0x2029;
}

// [windhawk-reviewer] CountBreaksText() removed: dead code (textMode was always 0).

// A visual row `idx` (row number v > 0) starts a new paragraph if the previous
// row ended with a paragraph break. Without reading text we infer this from
// geometry: the previous row either (a) is empty (just the break, span == 1; some
// builds report a length of 1 for such rows, so it is special-cased), or (b) its
// text is shorter than the distance to this row (the gap is the break char).
static bool RowStartsParagraphByGap(HWND edit, LONG v, LONG idx) {
    LONG pidx = (LONG)SendMessageW(edit, EM_LINEINDEX, v - 1, 0);
    if (pidx < 0) return true;
    if (idx - pidx <= 1) return true;  // empty line
    LONG plen = (LONG)SendMessageW(edit, EM_LINELENGTH, pidx, 0);
    return idx > pidx + plen;
}

// Paragraph breaks in [from, to) ("\r\n" counts once), derived from the row geometry.
static LONG CountBreaksRows(HWND edit, LONG from, LONG to) {
    LONG lines = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    LONG v = (LONG)SendMessageW(edit, EM_LINEFROMCHAR, from, 0);
    if (v < 0) v = 0;
    LONG total = 0;
    for (v++; v < lines; v++) {
        LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
        if (idx < 0 || idx > to) break;
        if (RowStartsParagraphByGap(edit, v, idx)) total++;
    }
    return total;
}

static LONG CountBreaks(EditorCtx* c, LONG from, LONG to) {
    // [windhawk-reviewer] the dead textMode / CountBreaksText dispatch was removed (textMode was always 0)
    return CountBreaksRows(c->edit, from, to);
}

// [windhawk-reviewer] Line-count shortcut. Counting paragraph breaks from the row geometry costs 2-3
// SendMessage calls per visual row and was redone after every edit (on the order of 100,000 calls for
// a keystroke near the end of a large file). When the document copy is current, the minimap's line
// table (mmStart) already maps a character index to its logical line with one upper_bound, so use
// that. It is built here on demand (no editor messages, works with the minimap off); without a
// current copy (plain file with the minimap and markers off, very large file, copy stale after an edit)
// the row-walking count below is still the fallback.
static LONG MmLineOf(const EditorCtx* c, LONG idx);  // defined with the minimap model
static void BuildMmLines(EditorCtx* c);

static bool LineModelUsable(EditorCtx* c) {
    if (!c->docOk || c->docStale) return false;
    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200 /*CP_UNICODE*/};
    LONG len = (LONG)SendMessageW(c->edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    if (c->docSynLen != len) return false;  // the copy is not of the text that is in the editor now
    if (c->mmBuiltRev != c->docRev) BuildMmLines(c);
    return !c->mmStart.empty();
}

// Number of paragraph breaks in [0, idx), walking from the cached anchor.
static LONG BreaksBefore(EditorCtx* c, CrCache& cache, LONG idx) {
    if (LineModelUsable(c)) {  // [windhawk-reviewer] one lookup instead of walking the rows
        LONG line = MmLineOf(c, idx);
        cache.valid = true;
        cache.pos = idx;
        cache.count = line;
        return line;
    }
    LONG result;
    if (cache.valid) {
        if (idx >= cache.pos)
            result = cache.count + CountBreaks(c, cache.pos, idx);
        else if (cache.pos - idx < idx)
            result = cache.count - CountBreaks(c, idx, cache.pos);
        else
            result = CountBreaks(c, 0, idx);
    } else {
        result = CountBreaks(c, 0, idx);
    }
    cache.valid = true;
    cache.pos = idx;
    cache.count = result;
    return result;
}

// ---------------------------------------------------------------------------
// Font / metrics
// ---------------------------------------------------------------------------

// Mirrors the editor's font (face, size, zoom, DPI). Returns true if changed.
static bool UpdateFont(EditorCtx* c) {
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    SendMessageW(c->edit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    if (!cf.szFaceName[0] || cf.yHeight <= 0) {
        cf = {};
        cf.cbSize = sizeof(cf);
        SendMessageW(c->edit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }
    WCHAR face[LF_FACESIZE];
    if (cf.szFaceName[0]) {
        wcsncpy(face, cf.szFaceName, LF_FACESIZE - 1);
        face[LF_FACESIZE - 1] = 0;
    } else {
        wcscpy(face, L"Consolas");
    }
    LONG twips = cf.yHeight > 0 ? cf.yHeight : 220;
    c->textColorKnown = (cf.dwMask & CFM_COLOR) && !(cf.dwEffects & CFE_AUTOCOLOR);
    c->textColor = cf.crTextColor;

    UINT num = 0, den = 0;
    SendMessageW(c->edit, EM_GETZOOM, (WPARAM)&num, (LPARAM)&den);
    if (!num || !den) num = den = 1;
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;
    int px = (int)((double)twips * dpi * num / (1440.0 * den) + 0.5);
    px = std::max(px, 6);

    if (c->font && px == c->fontPx && wcscmp(face, c->face) == 0) return false;

    if (c->font) DeleteObject(c->font);
    if (c->fontBold) DeleteObject(c->fontBold);
    c->fontBold = CreateFontW(-px, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, face);
    c->font = CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH | FF_DONTCARE, face);
    c->fontPx = px;
    wcscpy(c->face, face);
    return true;
}

static int MeasureGutterWidth(EditorCtx* c, int digits) {
    HWND wnd = c->gutter ? c->gutter : c->edit;
    HDC dc = GetDC(wnd);
    HGDIOBJ old = SelectObject(dc, c->font);
    std::wstring s(digits, L'8');
    SIZE sz{};
    GetTextExtentPoint32W(dc, s.c_str(), digits, &sz);
    SelectObject(dc, old);
    ReleaseDC(wnd, dc);
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;
    return sz.cx + MulDiv(8, dpi, 96) /*left pad*/ + MulDiv(10, dpi, 96) /*right pad + divider*/;
}

// ---------------------------------------------------------------------------
// Layout: reserve space inside the editor and place the gutter on top of it
// ---------------------------------------------------------------------------

static bool InsetHonored(EditorCtx* c, int minX) {
    POINT sp{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    if (sp.x != 0) return true;  // horizontally scrolled: can't tell
    POINT pt{};
    if (!PosFromChar(c->edit, 0, &pt)) return true;
    return pt.x >= minX;
}

static void ApplyRect(EditorCtx* c, int leftInset, int rightInset) {
    RECT r;
    GetClientRect(c->edit, &r);
    r.left += leftInset;
    r.right -= rightInset;
    SendMessageW(c->edit, EM_SETRECT, 0, (LPARAM)&r);
}

// The editor only repaints the dirty region after a margin change, so pixels from the
// previous layout can linger in the new left margin. Repaint everything.
static void ForceEditorRedraw(EditorCtx* c) {
    RedrawWindow(c->edit, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW);
}

// Left inset = line-number gutter (when on), right inset = minimap (when on).
static void ApplyInset(EditorCtx* c) {
    int origLeft = (int)LOWORD(c->origMargins);
    int origRight = (int)HIWORD(c->origMargins);
    int left = c->active ? c->gutterW : 0;
    int right = c->mmActive ? c->mmW : 0;
    if (c->rectFallback) {
        // Some builds ignore EM_SETMARGINS; use the formatting rect instead
        // (it is fixed, so WM_SIZE re-applies it).
        SendMessageW(c->edit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                     MAKELPARAM(origLeft, origRight));
        ApplyRect(c, left, right);
    } else {
        SendMessageW(c->edit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                     MAKELPARAM(origLeft + left, origRight + right));
    }
    ForceEditorRedraw(c);
}

static void Reposition(EditorCtx* c) {
    HWND parent = GetParent(c->edit);
    if (!parent || !c->gutter) return;
    RECT cr;
    GetClientRect(c->edit, &cr);
    POINT p{0, 0};
    MapWindowPoints(c->edit, parent, &p, 1);
    RECT want = {p.x, p.y, p.x + c->gutterW, p.y + cr.bottom};
    bool visible = IsWindowVisible(c->gutter);
    if (visible && EqualRect(&want, &c->lastPlacement)) return;
    c->lastPlacement = want;
    SetWindowPos(c->gutter, HWND_TOP, want.left, want.top, want.right - want.left,
                 want.bottom - want.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    c->sigValid = false;
}

// Remembers the editor's own margins and makes it skip painting over our sibling windows.
// Shared by the gutter and the minimap; undone by ReleaseOrReapply once neither needs it.
static void CaptureOriginals(EditorCtx* c) {
    if (c->captured) return;
    LONG_PTR st = GetWindowLongPtrW(c->edit, GWL_STYLE);
    if (!(st & WS_CLIPSIBLINGS)) {
        // Lets the editor's painting skip the regions covered by the gutter / minimap.
        SetWindowLongPtrW(c->edit, GWL_STYLE, st | WS_CLIPSIBLINGS);
        c->addedClipSiblings = true;
    }
    c->origMargins = (DWORD)SendMessageW(c->edit, EM_GETMARGINS, 0, 0);
    c->captured = true;
}

// Called after one of the two features switched off: if the other one is still on, re-apply
// the margins without it; otherwise give the editor its original layout back.
static void ReleaseOrReapply(EditorCtx* c) {
    if (c->active || c->mmActive) {
        ApplyInset(c);
        return;
    }
    if (!c->captured) return;
    SendMessageW(c->edit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                 MAKELPARAM(LOWORD(c->origMargins), HIWORD(c->origMargins)));
    if (c->rectFallback) ApplyRect(c, 0, 0);
    if (c->addedClipSiblings) {
        LONG_PTR st = GetWindowLongPtrW(c->edit, GWL_STYLE);
        SetWindowLongPtrW(c->edit, GWL_STYLE, st & ~(LONG_PTR)WS_CLIPSIBLINGS);
        c->addedClipSiblings = false;
    }
    ForceEditorRedraw(c);
    c->captured = false;
}

static void Activate(EditorCtx* c) {
    CaptureOriginals(c);
    c->active = true;
    c->textDirty = true;
    c->gutterW = 0;  // forces recompute in Tick
    c->sigValid = false;
}

static void Deactivate(EditorCtx* c) {
    if (c->gutter) ShowWindow(c->gutter, SW_HIDE);
    bool was = c->active;
    c->active = false;
    c->gutterW = 0;
    c->lastPlacement = {};
    if (was && IsWindow(c->edit)) ReleaseOrReapply(c);
}

// ---------------------------------------------------------------------------
// Syntax highlighting (1/2): language detection, lexer, document snapshot
// ---------------------------------------------------------------------------

enum SynClass : uint8_t {
    kcDefault,
    kcKeyword,
    kcType,
    kcString,
    kcComment,
    kcNumber,
    kcPreproc,
    kcConst,
    kcCount
};

constexpr size_t kSyntaxMaxChars = 3000000;  // larger documents are left uncolored

enum LangFlags {
    fLineSlash = 1,    // // comments
    fLineHash = 2,     // # comments
    fBlockC = 4,       // /* */ comments
    fTriple = 8,       // ''' and """ strings
    fBacktick = 16,    // `template` strings
    fPreproc = 32,     // #directive at line start
    fAnnot = 64,       // @Annotation / @decorator
    fJsonKeys = 128,   // "key": colored as a type
    fDigitSep = 256    // 1'000'000
};

struct LangCfg {
    const WCHAR* exts;  // space separated, lower case
    int flags;
    // [windhawk-reviewer] the raw keywords / types / constants pointers were removed: nothing read them
    // after BuildSet() had turned them into the kw / ty / cn sets.
    std::unordered_set<std::wstring_view> kw, ty, cn;
};

static void BuildSet(std::unordered_set<std::wstring_view>& s, const WCHAR* list) {
    const WCHAR* p = list;
    while (*p) {
        while (*p == L' ') p++;
        const WCHAR* q = p;
        while (*q && *q != L' ') q++;
        if (q > p) s.insert(std::wstring_view(p, (size_t)(q - p)));
        p = q;
    }
}

static std::vector<LangCfg> BuildLangs() {
    std::vector<LangCfg> v;
    auto add = [&](const WCHAR* exts, int flags, const WCHAR* kw, const WCHAR* ty,
                   const WCHAR* cn) {
        LangCfg l;
        l.exts = exts;
        l.flags = flags;
        BuildSet(l.kw, kw);
        BuildSet(l.ty, ty);
        BuildSet(l.cn, cn);
        v.push_back(std::move(l));
    };

    // 0: C / C++
    add(L"c h cpp cc cxx c++ hpp hh hxx h++ ino cu cuh",
        fLineSlash | fBlockC | fPreproc | fDigitSep,
        L"alignas alignof and and_eq asm auto bitand bitor break case catch class compl concept "
        L"const consteval constexpr constinit const_cast continue co_await co_return co_yield "
        L"decltype default delete do dynamic_cast else enum explicit export extern final for "
        L"friend goto if inline mutable namespace new noexcept not not_eq operator or or_eq "
        L"override private protected public register reinterpret_cast requires restrict return "
        L"sizeof static static_assert static_cast struct switch template this thread_local "
        L"throw try typedef typeid typename union using virtual volatile while xor xor_eq",
        L"bool char char8_t char16_t char32_t double float int long short signed unsigned void "
        L"wchar_t size_t ssize_t ptrdiff_t intptr_t uintptr_t int8_t int16_t int32_t int64_t "
        L"uint8_t uint16_t uint32_t uint64_t string wstring string_view vector map set "
        L"unordered_map unordered_set list deque array pair tuple optional variant unique_ptr "
        L"shared_ptr weak_ptr function BOOL DWORD WORD HWND LPARAM WPARAM LRESULT HRESULT UINT "
        L"LONG BYTE HANDLE HMODULE HINSTANCE",
        L"true false nullptr NULL");

    // 1: Java
    add(L"java", fLineSlash | fBlockC | fAnnot,
        L"abstract assert break case catch class const continue default do else enum extends "
        L"final finally for goto if implements import instanceof interface native new package "
        L"private protected public return static strictfp super switch synchronized this throw "
        L"throws transient try void volatile while var record sealed permits yield",
        L"boolean byte char double float int long short String Object Integer Long Double Float "
        L"Boolean Character Byte Short List Map Set ArrayList HashMap HashSet Optional Exception "
        L"RuntimeException Math System",
        L"true false null");

    // 2: C#
    add(L"cs csx", fLineSlash | fBlockC | fPreproc,
        L"abstract as base break case catch checked class const continue default delegate do "
        L"else enum event explicit extern finally fixed for foreach goto if implicit in "
        L"interface internal is lock namespace new operator out override params private "
        L"protected public readonly ref return sealed sizeof stackalloc static struct switch "
        L"this throw try typeof unchecked unsafe using virtual volatile while async await var "
        L"record init get set yield when where partial nameof",
        L"bool byte char decimal double float int long object sbyte short string uint ulong "
        L"ushort void dynamic String List Dictionary Task",
        L"true false null");

    // 3: JavaScript / TypeScript
    add(L"js jsx mjs cjs ts tsx mts cts", fLineSlash | fBlockC | fBacktick | fAnnot,
        L"async await break case catch class const continue debugger default delete do else "
        L"export extends finally for function if import in instanceof let new of return static "
        L"super switch this throw try typeof var void while with yield from as get set "
        L"interface type enum implements namespace declare readonly abstract private protected "
        L"public keyof infer is satisfies",
        L"string number boolean any unknown never object symbol bigint Promise Array Map Set "
        L"Date Error Object",
        L"true false null undefined NaN Infinity");

    // 4: Python
    add(L"py pyw pyi", fLineHash | fTriple | fAnnot,
        L"and as assert async await break class continue def del elif else except finally for "
        L"from global if import in is lambda nonlocal not or pass raise return try while with "
        L"yield match case",
        L"int float str bool list dict set tuple bytes object type self cls",
        L"True False None");

    // 5: JSON
    add(L"json jsonc", fLineSlash | fBlockC | fJsonKeys, L"", L"", L"true false null");
    return v;
}

static std::vector<LangCfg>& Langs() {
    static std::vector<LangCfg> v = BuildLangs();
    return v;
}

static bool IsIdStart(WCHAR c) {
    return c == L'_' || c == L'$' ||
           (c < 128 ? ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) : iswalpha(c) != 0);
}
static bool IsIdPart(WCHAR c) {
    return IsIdStart(c) || (c >= L'0' && c <= L'9');
}
static bool IsDigitCh(WCHAR c) {
    return c >= L'0' && c <= L'9';
}

// Classifies every character of the document. Multi-line comments/strings are
// handled here, so the draw hook only has to look classes up.
static void Tokenize(const LangCfg& L, const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > n) b = n;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    size_t i = 0;
    bool lineStart = true;
    while (i < n) {
        WCHAR c = t[i];
        if (c == L'\r' || c == L'\n') {
            i++;
            lineStart = true;
            continue;
        }
        if (c == L' ' || c == L'\t') {
            i++;
            continue;
        }
        bool atStart = lineStart;
        lineStart = false;

        // Comments
        if ((L.flags & fLineSlash) && c == L'/' && i + 1 < n && t[i + 1] == L'/') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if ((L.flags & fBlockC) && c == L'/' && i + 1 < n && t[i + 1] == L'*') {
            size_t e = t.find(L"*/", i + 2);
            e = (e == std::wstring::npos) ? n : e + 2;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if ((L.flags & fLineHash) && c == L'#') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            continue;
        }

        // Preprocessor directive (#include, #define, #region, ...)
        if ((L.flags & fPreproc) && c == L'#' && atStart) {
            size_t e = i + 1;
            while (e < n && (t[e] == L' ' || t[e] == L'\t')) e++;
            size_t ws = e;
            while (e < n && IsIdPart(t[e])) e++;
            fill(i, e, kcPreproc);
            if (t.compare(ws, e - ws, L"include") == 0 || t.compare(ws, e - ws, L"import") == 0) {
                size_t p = e;
                while (p < n && (t[p] == L' ' || t[p] == L'\t')) p++;
                if (p < n && t[p] == L'<') {
                    size_t q = p;
                    while (q < n && t[q] != L'>' && t[q] != L'\r' && t[q] != L'\n') q++;
                    if (q < n && t[q] == L'>') {
                        fill(p, q + 1, kcString);
                        e = q + 1;
                    }
                }
            }
            i = e;
            continue;
        }

        // Strings
        if (c == L'"' || (c == L'\'' && !(L.flags & fJsonKeys)) ||
            (c == L'`' && (L.flags & fBacktick))) {
            if ((L.flags & fTriple) && i + 2 < n && t[i + 1] == c && t[i + 2] == c) {
                WCHAR trip[4] = {c, c, c, 0};
                size_t e = t.find(trip, i + 3);
                e = (e == std::wstring::npos) ? n : e + 3;
                fill(i, e, kcString);
                i = e;
                continue;
            }
            bool multi = (c == L'`');
            size_t e = i + 1;
            while (e < n) {
                WCHAR d = t[e];
                if (d == L'\\' && e + 1 < n) {
                    e += 2;
                    continue;
                }
                if (d == c) {
                    e++;
                    break;
                }
                if (!multi && (d == L'\r' || d == L'\n')) break;
                e++;
            }
            fill(i, e, kcString);
            if (L.flags & fJsonKeys) {
                size_t k = e;
                while (k < n && (t[k] == L' ' || t[k] == L'\t')) k++;
                if (k < n && t[k] == L':') fill(i, e, kcType);
            }
            i = e;
            continue;
        }

        // Numbers
        if (IsDigitCh(c) || (c == L'.' && i + 1 < n && IsDigitCh(t[i + 1]))) {
            size_t e = i + 1;
            bool hex = (c == L'0' && e < n && (t[e] == L'x' || t[e] == L'X'));
            while (e < n) {
                WCHAR d = t[e];
                if ((d < 128 && iswalnum(d)) || d == L'_' || d == L'.') {
                    e++;
                    continue;
                }
                if (d == L'\'' && (L.flags & fDigitSep) && e + 1 < n && iswalnum(t[e + 1])) {
                    e++;
                    continue;
                }
                if ((d == L'+' || d == L'-') && !hex &&
                    (t[e - 1] == L'e' || t[e - 1] == L'E' || t[e - 1] == L'p' ||
                     t[e - 1] == L'P')) {
                    e++;
                    continue;
                }
                break;
            }
            fill(i, e, kcNumber);
            i = e;
            continue;
        }

        // Identifiers / keywords
        if (IsIdStart(c)) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            std::wstring_view w(t.data() + i, e - i);
            uint8_t k = kcDefault;
            if (L.kw.count(w)) k = kcKeyword;
            else if (L.ty.count(w)) k = kcType;
            else if (L.cn.count(w)) k = kcConst;
            if (k != kcDefault) fill(i, e, k);
            i = e;
            continue;
        }

        // Annotations / decorators
        if (c == L'@' && (L.flags & fAnnot) && i + 1 < n && IsIdStart(t[i + 1])) {
            size_t e = i + 1;
            while (e < n && (IsIdPart(t[e]) || t[e] == L'.')) e++;
            fill(i, e, kcPreproc);
            i = e;
            continue;
        }
        i++;
    }
}

// [windhawk-reviewer] g_lastExpected (a global only ReadDocText used) is now a local in ReadDocText.

// The editor numbers characters with a line break counting as ONE character (CRLF is a
// single paragraph mark), while EM_GETTEXTEX hands back CRLF text. Everything in the
// syntax code (row indexes, selection, token classes) must use the editor's numbering.
static void CollapseBreaks(std::wstring& t) {
    size_t w = 0;
    for (size_t i = 0; i < t.size(); i++) {
        WCHAR ch = t[i];
        if (ch == L'\r' && i + 1 < t.size() && t[i + 1] == L'\n') {
            t[w++] = L'\r';
            i++;
        } else if (ch == L'\n') {
            t[w++] = L'\r';
        } else {
            t[w++] = ch;
        }
    }
    t.resize(w);
}

// Length implied by the row geometry: start of the last row + its length.
static LONG ExpectedEditorLength(EditorCtx* c) {
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    if (lines <= 0) return 0;
    LONG idx = (LONG)SendMessageW(c->edit, EM_LINEINDEX, lines - 1, 0);
    if (idx < 0) return 0;
    return idx + (LONG)SendMessageW(c->edit, EM_LINELENGTH, idx, 0);
}

// Reads the whole document in the editor's own index space.
static bool ReadDocText(EditorCtx* c, LONG len, std::wstring& out) {
    // [windhawk-reviewer] was the global g_lastExpected; editor-space length implied by the row geometry
    const LONG lastExpected = ExpectedEditorLength(c);

    // Path 1: whole text in one call, then normalise line breaks.
    out.assign((size_t)len + 1, L'\0');
    GETTEXTEX gt{};
    gt.cb = (DWORD)((len + 1) * sizeof(WCHAR));
    gt.flags = GT_DEFAULT;
    gt.codepage = 1200;
    SendMessageW(c->edit, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)&out[0]);
    out.resize(wcsnlen(out.c_str(), (size_t)len + 1));
    CollapseBreaks(out);
    if (std::abs((long)out.size() - (long)lastExpected) <= 1) {
        return true;
    }

    // Path 2: assemble from rows, placing each row at its EM_LINEINDEX position.
    out.clear();
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    std::vector<WCHAR> buf;
    for (LONG v = 0; v < lines; v++) {
        LONG idx = (LONG)SendMessageW(c->edit, EM_LINEINDEX, v, 0);
        if (idx < 0) break;
        if ((LONG)out.size() < idx) out.append((size_t)(idx - (LONG)out.size()), L'\r');
        LONG ll = (LONG)SendMessageW(c->edit, EM_LINELENGTH, idx, 0);
        if (ll <= 0) continue;
        ll = std::min<LONG>(ll, 65000);
        buf.assign((size_t)ll + 2, 0);
        *(WORD*)buf.data() = (WORD)(ll + 1);
        LRESULT got = SendMessageW(c->edit, EM_GETLINE, v, (LPARAM)buf.data());
        if (got > 0) out.append(buf.data(), (size_t)std::min<LRESULT>(got, ll));
    }
    return std::abs((long)out.size() - (long)lastExpected) <= 1;
}

// Language from the file name in the window title; none for unsaved ("Untitled") tabs.
static std::wstring DocName(HWND edit);  // defined with the change markers below

static int DetectLang(HWND edit) {
    // [windhawk-reviewer] reuse DocName()'s title parsing instead of repeating it here
    std::wstring t = DocName(edit);
    size_t dot = t.rfind(L'.');
    if (dot == std::wstring::npos || dot + 1 >= t.size() || t.size() - dot > 9) return -1;
    std::wstring ext = t.substr(dot + 1);
    for (auto& ch : ext) ch = (WCHAR)towlower(ch);

    auto& langs = Langs();
    for (size_t i = 0; i < langs.size(); i++) {
        const WCHAR* q = langs[i].exts;
        while (*q) {
            while (*q == L' ') q++;
            const WCHAR* e = q;
            while (*e && *e != L' ') e++;
            if ((size_t)(e - q) == ext.size() && wcsncmp(q, ext.c_str(), ext.size()) == 0)
                return (int)i;
            q = e;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Change markers: what changed in the document since it was opened
// ---------------------------------------------------------------------------

// [windhawk-reviewer] removed the leftover DIFFBEGIN / DIFFEND markers around the diff code
// Hash of every line (lines are separated by '\r', the editor's paragraph mark).
static void HashLines(const std::wstring& t, std::vector<uint64_t>& h) {
    h.clear();
    const uint64_t kBasis = 1469598103934665603ULL;
    uint64_t x = kBasis;
    for (wchar_t ch : t) {
        if (ch == L'\r') {
            h.push_back(x);
            x = kBasis;
            continue;
        }
        x ^= (uint64_t)(uint16_t)ch;
        x *= 1099511628211ULL;
    }
    h.push_back(x);  // the last line, possibly empty
}

struct LineDiff {
    std::vector<uint8_t> aDel;  // line of the baseline is gone
    std::vector<uint8_t> bIns;  // line of the current text is new
    // [windhawk-reviewer] don't retry an exact diff that already gave up (see DiffRange)
    bool skipBigExact = false;    // in: an expensive exact diff already gave up for this baseline
    bool bigExactFailed = false;  // out: an expensive exact diff gave up during this run
};

static void MarkAll(LineDiff& out, int a0, int a1, int b0, int b1) {
    for (int i = a0; i < a1; i++) out.aDel[(size_t)i] = 1;
    for (int j = b0; j < b1; j++) out.bIns[(size_t)j] = 1;
}

// Myers' O(ND) diff of A[a0,a1) against B[b0,b1). Gives up (false) beyond maxD edits.
static bool MyersRange(const uint64_t* A, int a0, int a1, const uint64_t* B, int b0, int b1,
                       int maxD, LineDiff& out) {
    const int n = a1 - a0, m = b1 - b0;
    const int D = std::min(maxD, n + m);
    const int off = D + 1;
    std::vector<int> V((size_t)(2 * D + 3), 0);
    std::vector<std::vector<int>> trace;  // window [-(d+1), d+1] of V at the start of round d
    int found = -1;
    for (int d = 0; d <= D && found < 0; d++) {
        std::vector<int> snap((size_t)(2 * d + 3));
        for (int k = -(d + 1); k <= d + 1; k++) snap[(size_t)(k + d + 1)] = V[(size_t)(off + k)];
        trace.push_back(std::move(snap));
        for (int k = -d; k <= d; k += 2) {
            int x;
            if (k == -d || (k != d && V[(size_t)(off + k - 1)] < V[(size_t)(off + k + 1)]))
                x = V[(size_t)(off + k + 1)];
            else
                x = V[(size_t)(off + k - 1)] + 1;
            int y = x - k;
            while (x < n && y < m && A[a0 + x] == B[b0 + y]) {
                x++;
                y++;
            }
            V[(size_t)(off + k)] = x;
            if (x >= n && y >= m) {
                found = d;
                break;
            }
        }
    }
    if (found < 0) return false;

    int x = n, y = m;
    for (int d = found; d >= 0; d--) {
        const std::vector<int>& v = trace[(size_t)d];
        auto at = [&](int k) { return v[(size_t)(k + d + 1)]; };
        int k = x - y;
        int prevk;
        if (k == -d || (k != d && at(k - 1) < at(k + 1)))
            prevk = k + 1;
        else
            prevk = k - 1;
        int prevx = at(prevk);
        int prevy = prevx - prevk;
        while (x > prevx && y > prevy) {
            x--;
            y--;
        }
        if (d > 0) {
            if (x == prevx)
                out.bIns[(size_t)(b0 + prevy)] = 1;  // a line of B was inserted
            else
                out.aDel[(size_t)(a0 + prevx)] = 1;  // a line of A was deleted
        }
        x = prevx;
        y = prevy;
    }
    return true;
}

static void DiffRange(const uint64_t* A, int a0, int a1, const uint64_t* B, int b0, int b1,
                      LineDiff& out, int depth) {
    while (a0 < a1 && b0 < b1 && A[a0] == B[b0]) {
        a0++;
        b0++;
    }
    while (a0 < a1 && b0 < b1 && A[a1 - 1] == B[b1 - 1]) {
        a1--;
        b1--;
    }
    if (a0 == a1 || b0 == b1) {  // pure insertion or deletion
        MarkAll(out, a0, a1, b0, b1);
        return;
    }
    const int n = a1 - a0, m = b1 - b0;
    // Work budget of about 4e7 steps: small documents get exact diffs, huge ones a bounded effort.
    const int cap = (int)std::clamp<int64_t>(40000000LL / (n + m), 64, 3000);
    // [windhawk-reviewer] MyersRange keeps a snapshot of V for every round, i.e. about D*D ints: ~36 MB
    // at the 3,000-round cap, allocated on the UI thread and thrown away when it gives up. That used to
    // repeat on every edit. Once an expensive (D > 1000, > ~4 MB) attempt has failed for this baseline
    // it is skipped (the anchor fallback below takes over); MarkerTick lets one in every 32 runs retry,
    // so the exact diff comes back when the document moves closer to its baseline.
    const bool expensive = std::min(cap, n + m) > 1000;
    if (!(expensive && out.skipBigExact)) {
        if (MyersRange(A, a0, a1, B, b0, b1, cap, out)) return;
        if (expensive) out.bigExactFailed = true;
    }

    // Too many differences for an exact diff (e.g. a replace-all across the file): anchor on the
    // lines that occur exactly once on both sides, keep the longest in-order chain of those, and
    // diff the gaps between them.
    if (depth < 24) {
        std::unordered_map<uint64_t, int> ua, ub;  // line -> position, or -1 if it repeats
        for (int i = a0; i < a1; i++) {
            auto it = ua.find(A[i]);
            if (it == ua.end()) ua[A[i]] = i; else it->second = -1;
        }
        for (int j = b0; j < b1; j++) {
            auto it = ub.find(B[j]);
            if (it == ub.end()) ub[B[j]] = j; else it->second = -1;
        }
        std::vector<std::pair<int, int>> anchors;
        for (int i = a0; i < a1; i++) {
            auto it = ua.find(A[i]);
            if (it->second != i) continue;
            auto jt = ub.find(A[i]);
            if (jt != ub.end() && jt->second >= 0) anchors.push_back({i, jt->second});
        }
        if (!anchors.empty()) {
            // Longest increasing subsequence of the B positions.
            std::vector<int> tailJ, tailIdx, prev(anchors.size(), -1);
            for (size_t t = 0; t < anchors.size(); t++) {
                int j = anchors[t].second;
                size_t p = (size_t)(std::lower_bound(tailJ.begin(), tailJ.end(), j) - tailJ.begin());
                if (p > 0) prev[t] = tailIdx[p - 1];
                if (p == tailJ.size()) {
                    tailJ.push_back(j);
                    tailIdx.push_back((int)t);
                } else {
                    tailJ[p] = j;
                    tailIdx[p] = (int)t;
                }
            }
            std::vector<int> chain;
            for (int t = tailIdx.back(); t >= 0; t = prev[(size_t)t]) chain.push_back(t);
            std::reverse(chain.begin(), chain.end());
            int pa = a0, pb = b0;
            for (int t : chain) {
                DiffRange(A, pa, anchors[(size_t)t].first, B, pb, anchors[(size_t)t].second, out,
                          depth + 1);
                pa = anchors[(size_t)t].first + 1;
                pb = anchors[(size_t)t].second + 1;
            }
            DiffRange(A, pa, a1, B, pb, b1, out, depth + 1);
            return;
        }
    }
    MarkAll(out, a0, a1, b0, b1);
}

// Turns the edit script into per-line states. In a hunk that both removes and adds lines, the
// new lines that replace old ones are "modified", the surplus new lines are "added", and surplus
// removed lines leave a "removed" notch just before the next line.
static void BuildMarkers(const LineDiff& d, int nOld, int nNew, std::vector<uint8_t>& state,
                         std::vector<uint32_t>& del) {
    state.assign((size_t)nNew, 0);
    del.clear();
    int i = 0, j = 0;
    while (i < nOld || j < nNew) {
        bool delA = i < nOld && d.aDel[(size_t)i];
        bool insB = j < nNew && d.bIns[(size_t)j];
        if (!delA && !insB) {
            if (i >= nOld || j >= nNew) break;  // cannot happen for a consistent script
            i++;
            j++;
            continue;
        }
        int dc = 0, ic = 0, j0 = j;
        while (i < nOld && d.aDel[(size_t)i]) {
            i++;
            dc++;
        }
        while (j < nNew && d.bIns[(size_t)j]) {
            j++;
            ic++;
        }
        int mod = std::min(dc, ic);
        for (int t = 0; t < ic; t++) state[(size_t)(j0 + t)] = t < mod ? 2 : 1;
        if (dc > ic && (del.empty() || del.back() != (uint32_t)j)) del.push_back((uint32_t)j);
    }
}

// "Untitled" tabs (and anything without a name) get no markers: typing into a new document
// would turn every line green.
static bool IsSavedName(const std::wstring& n) {
    if (n.empty() || _wcsnicmp(n.c_str(), L"untitled", 8) == 0) return false;
    // A saved document is a file name with an extension ("notes.txt"). Anything else that can
    // show up in the title ("Notepad", an "Untitled" in another language, ...) is a document
    // that was never saved.
    size_t dot = n.rfind(L'.');
    return dot != std::wstring::npos && dot + 1 < n.size();
}

// The document name from the window title ("*name - Notepad" -> "name").
static std::wstring DocName(HWND edit) {
    HWND top = GetAncestor(edit, GA_ROOT);
    if (!top) return L"";
    WCHAR buf[512] = {};
    GetWindowTextW(top, buf, 511);
    std::wstring t = buf;
    size_t p = std::wstring::npos;
    for (const WCHAR* sep : {L" - ", L" \u2013 ", L" \u2014 "}) {
        size_t q = t.rfind(sep);
        if (q != std::wstring::npos && (p == std::wstring::npos || q > p)) p = q;
    }
    if (p != std::wstring::npos) t.resize(p);
    while (!t.empty() && (t[0] == L'*' || t[0] == L' ' || t[0] == 0x25CF)) t.erase(0, 1);
    while (!t.empty() && t.back() == L' ') t.pop_back();
    return t;
}

// Forget the baseline and the markers; a new one is captured from the next document refresh.
static void MarkerResetState(EditorCtx* c) {
    c->mkHave = c->mkFrozen = c->mkUserEdit = c->mkReload = c->mkEmptyBase = false;
    c->mkBase.clear();
    c->mkBaseHash.clear();
    c->mkState.clear();
    c->mkDel.clear();
    c->mkRev = ~0u;
    c->mkDirty = true;
    c->mkExactFailed = false;  // [windhawk-reviewer] a new baseline gets a fresh try at the exact diff
}

// Runs whenever the title is re-read. A different name means a different document, but the
// title is shared by the whole window and can lag behind a tab switch, so a new name must be
// seen twice in a row before it counts.
static void MarkerNameTick(EditorCtx* c) {
    std::wstring nm = DocName(c->edit);
    if (c->mkNameKnown && nm == c->mkName) {
        c->mkPendingName.clear();
        return;
    }
    if (nm != c->mkPendingName) {  // a new name (or the first one): wait for a second read
        c->mkPendingName = nm;
        return;
    }
    c->mkName = nm;
    c->mkNameKnown = true;
    c->mkPendingName.clear();
    c->mkNameOk = IsSavedName(nm);
    MarkerResetState(c);
}

// Called from SyntaxTick after the document copy was refreshed.
static void MarkerTick(EditorCtx* c, ULONGLONG now) {
    if (c->mkReload) {  // the document was replaced (opened / reloaded): start over
        bool keepName = c->mkNameOk;
        MarkerResetState(c);
        c->mkNameOk = keepName;
    }
    if (!c->mkOn) {
        if (!c->mkState.empty() || !c->mkDel.empty()) {
            c->mkState.clear();
            c->mkDel.clear();
            c->mkDirty = true;
        }
        c->mkRev = ~0u;
        return;
    }
    if (!c->docOk || c->mkRev == c->docRev) return;

    if (!c->mkHave) {
        c->mkBase = c->docText;
        HashLines(c->mkBase, c->mkBaseHash);
        c->mkHave = true;
        c->mkFollowUntil = now + kMkFollowMs;
    } else if (!c->mkFrozen) {
        if (c->mkUserEdit || now > c->mkFollowUntil) {
            c->mkFrozen = true;  // from here on every difference is a change
        } else {
            // Still loading: the baseline is whatever the document finally turns out to be.
            c->mkBase = c->docText;
            HashLines(c->mkBase, c->mkBaseHash);
        }
    }
    if (c->mkFrozen && c->mkBase.empty()) {  // started as an empty document: treat like "Untitled"
        c->mkEmptyBase = true;
        c->mkOn = false;
        c->mkState.clear();
        c->mkDel.clear();
        c->mkDirty = true;
        c->mkRev = c->docRev;
        return;
    }

    std::vector<uint64_t> cur;
    HashLines(c->docText, cur);
    const std::vector<uint64_t>& base = c->mkBaseHash;
    LineDiff d;
    d.aDel.assign(base.size(), 0);
    d.bIns.assign(cur.size(), 0);
    // [windhawk-reviewer] remember when the expensive exact diff gave up, and don't redo it on every edit
    d.skipBigExact = c->mkExactFailed && (++c->mkExactSkips % 32) != 0;
    DiffRange(base.data(), 0, (int)base.size(), cur.data(), 0, (int)cur.size(), d, 0);
    if (d.bigExactFailed)
        c->mkExactFailed = true;
    else if (!d.skipBigExact)
        c->mkExactFailed = false;  // the exact diff worked (or was not needed): back to normal
    std::vector<uint8_t> st;
    std::vector<uint32_t> del;
    BuildMarkers(d, (int)base.size(), (int)cur.size(), st, del);
    c->mkRev = c->docRev;
    if (st != c->mkState || del != c->mkDel) {
        c->mkState.swap(st);
        c->mkDel.swap(del);
        c->mkDirty = true;
    }
}

// Called every tick for the visible editor.
static void RefreshSyntax(EditorCtx* c, LONG len, ULONGLONG now) {
    if (c->lang == -2 || ++c->synTick % 15 == 0) {
        MarkerNameTick(c);
        int lg = DetectLang(c->edit);
        if (lg != c->lang) {
            c->lang = lg;
            c->docStale = true;
            c->docSynLen = -2;
            c->docText.clear();
            c->cls.clear();
            c->snap.clear();
            c->docOk = false;
            c->docRev++;
            c->sigValid = false;
            InvalidateRect(c->edit, nullptr, FALSE);  // repaint plain (or newly colored)
        }
    }
    // The document copy is needed for syntax colors (known language) and for the minimap.
    if ((size_t)len > kSyntaxMaxChars || (c->lang < 0 && !c->mmActive && !c->mkOn)) {
        if (!c->docText.empty() || c->docOk) {
            c->docText.clear();
            c->cls.clear();
            c->snap.clear();
            c->docOk = false;
            c->docRev++;
        }
        c->docStale = true;  // read afresh once somebody needs it again
        c->docSynLen = -2;
        return;
    }
    // Plain files (no language) are only read for the minimap; leave that to the timer tick so
    // that no extra editor traffic happens in the middle of a keystroke.
    if (c->docStale && !(c->lang < 0 && c->inMsg) &&
        (len <= 200000 || now - c->synChangeTick > 150)) {
        c->docStale = false;
        if (ReadDocText(c, len, c->docText)) {
            if (c->lang >= 0)
                Tokenize(Langs()[(size_t)c->lang], c->docText, c->cls);
            else
                c->cls.clear();  // no language: the minimap draws everything in the text color
            c->docOk = true;
            c->docRev++;
            c->docSynLen = len;
            c->docRetry = 0;
        } else {
            c->docText.clear();
            c->cls.clear();
            c->docOk = false;
            c->docRev++;
            // A failed read is usually the editor's line table still catching up with an edit (the
            // read is checked against it). Ask for another try on the next tick instead of leaving
            // the document uncolored until some other change comes along.
            if (c->docRetry < 60) {
                c->docRetry++;
                c->docStale = true;
            }
        }
        // Lines below an edit (e.g. a newly opened /* ... */) may have changed color.
        if (c->lang >= 0) InvalidateRect(c->edit, nullptr, FALSE);
    }
}

// Measures the rows around the viewport (one screen above and below, so rows that
// scroll into view between two ticks are already known). Positions are in client
// pixels. EM_POSFROMCHAR is wrong for the first row and for an empty trailing line,
// so those are re-derived from their neighbours.
static void BuildSnapshot(EditorCtx* c) {
    HWND edit = c->edit;
    LONG lineCount = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    LONG first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    RECT cr;
    GetClientRect(edit, &cr);
    int dpi = (int)GetDpiForWindow(edit);
    if (dpi <= 0) dpi = 96;
    if (c->snapPitch <= 0) c->snapPitch = MulDiv(18, dpi, 96);
    LONG perScreen = cr.bottom / std::max(c->snapPitch, 8) + 2;
    LONG v0 = std::max<LONG>(0, first - perScreen);
    LONG v1 = std::min<LONG>(lineCount, first + 2 * perScreen);

    struct R {
        LONG idx, v;
        int y, x;
        bool rel;
    };
    LONG docEnd = c->docText.empty() ? c->synLen : (LONG)c->docText.size();
    std::vector<R> rows;
    for (LONG v = v0; v < v1; v++) {
        LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
        if (idx < 0) break;
        POINT pt{};
        if (!PosFromChar(edit, idx, &pt)) break;
        rows.push_back({idx, v, pt.y, pt.x, idx > 0 && idx < docEnd});
    }
    if (rows.empty()) {
        c->snap.clear();
        c->snapHash = 0;
        return;
    }

    int pitch = 0;
    for (size_t k = 0; k + 1 < rows.size(); k++) {
        if (rows[k].rel && rows[k + 1].rel && rows[k + 1].v == rows[k].v + 1) {
            int d = rows[k + 1].y - rows[k].y;
            if (d > 0 && (pitch == 0 || d < pitch)) pitch = d;
        }
    }
    if (pitch > 0) c->snapPitch = pitch;
    else pitch = c->snapPitch;

    POINT sp{};
    SendMessageW(edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    for (int k = 0; k < (int)rows.size(); k++) {
        if (rows[(size_t)k].rel) continue;
        int ref = -1;
        for (int d = 1; d < (int)rows.size() && ref < 0; d++) {
            if (k - d >= 0 && rows[(size_t)(k - d)].rel) ref = k - d;
            else if (k + d < (int)rows.size() && rows[(size_t)(k + d)].rel) ref = k + d;
        }
        if (ref >= 0)
            rows[(size_t)k].y = rows[(size_t)ref].y + (k - ref) * pitch;
        else
            rows[(size_t)k].y = MulDiv(16, dpi, 96) + (int)rows[(size_t)k].v * pitch - sp.y;
    }

    c->snap.clear();
    for (size_t k = 0; k < rows.size(); k++) {
        LONG end = (k + 1 < rows.size())
                       ? rows[k + 1].idx
                       : rows[k].idx + (LONG)SendMessageW(edit, EM_LINELENGTH, rows[k].idx, 0);
        c->snap.push_back({rows[k].idx, end, rows[k].y, rows[k].x});
    }
    uint64_t hsh = 1469598103934665603ULL;
    for (const SnapRow& r : c->snap)
        for (int64_t f : {(int64_t)r.idx, (int64_t)r.end, (int64_t)r.y, (int64_t)r.x}) {
            hsh ^= (uint64_t)f;
            hsh *= 1099511628211ULL;
        }
    c->snapHash = hsh;
}

// Runs every tick for every editor, whether or not the line-number gutter is on:
// keeps the language, document copy, token classes and row snapshot current.
static void SyntaxTick(EditorCtx* c) {
    if (!IsWindow(c->edit)) return;
    if (!IsWindowVisible(c->edit)) {  // background tab
        c->shown = false;
        return;
    }
    if (!c->shown) {  // became visible: re-detect the language from the title
        c->shown = true;
        c->lang = -2;
        c->synTick = 0;
        c->synSigValid = false;
    }

    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200 /*CP_UNICODE*/};
    LONG len = (LONG)SendMessageW(c->edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    ULONGLONG now = GetTickCount64();
    if (len != c->synLen || lines != c->synLines) {
        c->synLen = len;
        c->synLines = lines;
        c->docStale = true;
        c->synChangeTick = now;
        c->synSigValid = false;
        c->synSettle = 8;  // the editor may still be laying the change out (undo, paste, ...)
        c->docRetry = 0;
    }

    bool wasStale = c->docStale;
    int oldLang = c->lang;
    c->mkOn = c->mkNameOk && !c->mkEmptyBase && (size_t)len <= kMkMaxChars;
    RefreshSyntax(c, len, now);
    MarkerTick(c, now);
    if (c->docStale != wasStale || c->lang != oldLang) c->synSigValid = false;
    if (c->lang < 0) return;

    LONG first = (LONG)SendMessageW(c->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    POINT scroll{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&scroll);
    DWORD s = 0, e = 0;
    SendMessageW(c->edit, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
    RECT cr;
    GetClientRect(c->edit, &cr);
    bool rebuilt = false;
    if (!c->synSigValid || first != c->synFirst || scroll.y != c->synScroll.y ||
        s != c->synSelS || e != c->synSelE || cr.right != c->synW || cr.bottom != c->synH) {
        rebuilt = true;
        // Only the selection moved? Then the rows are where they were and nothing needs a repaint.
        bool viewSame = c->synSigValid && first == c->synFirst && scroll.y == c->synScroll.y &&
                        cr.right == c->synW && cr.bottom == c->synH;
        c->synSigValid = true;
        c->synFirst = first;
        c->synScroll = scroll;
        c->synSelS = s;
        c->synSelE = e;
        c->synW = cr.right;
        c->synH = cr.bottom;
        BuildSnapshot(c);
        // The colors are matched against this snapshot while the editor paints. If the editor
        // already painted for the new view before the snapshot caught up (a single big jump, e.g.
        // from the minimap, has no follow-up scroll events to trigger another paint), the text
        // would stay plain. Ask for one more paint now that the snapshot matches.
        if (!viewSame) InvalidateRect(c->edit, nullptr, FALSE);
    }

    // After a text change (undo of a deleted line, paste, ...) the editor can finish laying the
    // text out after our tick ran, so the row positions we measured may already be stale and the
    // changed lines would not match any row, i.e. stay uncolored until something else (a scroll)
    // forces a new measurement. Keep re-measuring for a moment; repaint only if it changed.
    if (c->synSettle > 0 && !c->inMsg) {
        c->synSettle--;
        if (!rebuilt) {
            uint64_t before = c->snapHash;
            BuildSnapshot(c);
            if (c->snapHash != before) InvalidateRect(c->edit, nullptr, FALSE);
        }
    }
}

// [windhawk-reviewer] removed the empty "Per-editor controller" section header

// ---------------------------------------------------------------------------
// Minimap (1/2): document model, geometry, layout
//
// The minimap shows one thin row per *logical* line (a wrapped line counts once) and
// scrolls along with the editor when the document is taller than the minimap, like the
// one in VS Code. A slider marks the part of the document that is on screen.
// ---------------------------------------------------------------------------

struct MmGeo {
    LONG nLines = 1;           // lines in minimap space
    double top = 0, view = 1;  // viewport: first line, number of lines
    int pitch = 3, bh = 2;     // px per line, px height of the drawn block
    int cap = 1;               // lines that fit into the minimap's height
    int contentPx = 0;         // height of the drawn content
    int sliderPx = 0;          // slider height
    int sliderTop = 0;         // slider top, minimap client y
    double mmTop = 0;          // first line drawn at the top (fractional)
    bool haveModel = false;    // false: no document copy, lines are visual rows
};

// Visual rows: total, first visible, last visible (row whose top is inside the viewport).
// EM_POSFROMCHAR is reliable for every row but the first, which is never probed here.
static void ViewRows(EditorCtx* c, LONG& rows, LONG& first, LONG& last) {
    HWND edit = c->edit;
    rows = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    last = first;
    if (rows <= 0) return;
    first = std::clamp<LONG>(first, 0, rows - 1);
    RECT cr;
    GetClientRect(edit, &cr);
    // [windhawk-reviewer] The binary search used to run from `first` to the last row of the document,
    // so most probes hit rows far below the screen, which can make RichEdit lay out text nobody sees.
    // The last visible row is at most first + cr.bottom / minPitch + 1, so the search starts capped
    // there. minPitch is a deliberately low guess for the row height; if every probe up to the cap is
    // still visible (rows denser than guessed) the cap is widened, so the result never depends on it.
    const LONG minPitch = std::max<LONG>(4, c->fontPx / 2);
    LONG lo = first;
    LONG hi = std::min<LONG>(rows - 1, first + cr.bottom / minPitch + 1);
    for (;;) {
        const LONG bound = hi;
        while (lo < hi) {  // binary search: rows are ordered top to bottom
            LONG mid = lo + (hi - lo + 1) / 2;
            LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, mid, 0);
            POINT pt{};
            if (idx >= 0 && PosFromChar(edit, idx, &pt) && pt.y < cr.bottom)
                lo = mid;
            else
                hi = mid - 1;
        }
        if (lo < bound || bound >= rows - 1) break;  // stopped inside the cap (or it is the end)
        hi = std::min<LONG>(rows - 1, bound + (bound - first) + 1);  // the cap is still visible: widen
    }
    last = lo;
}

// "Scrollbar appears" == the document does not fit: something is above or below the view.
static bool EditorScrollable(EditorCtx* c) {
    LONG rows, first, last;
    ViewRows(c, rows, first, last);
    return rows > 0 && (first > 0 || last < rows - 1);
}

static void BuildMmLines(EditorCtx* c) {
    c->mmStart.clear();
    c->mmBuiltRev = c->docRev;
    if (!c->docOk) return;
    const std::wstring& t = c->docText;
    c->mmStart.push_back(0);
    for (size_t i = 0; i < t.size(); i++)
        if (t[i] == L'\r') c->mmStart.push_back((uint32_t)i + 1);  // editor numbering: CRLF == 1 char
}

static LONG MmLineOf(const EditorCtx* c, LONG idx) {
    auto it = std::upper_bound(c->mmStart.begin(), c->mmStart.end(), (uint32_t)std::max<LONG>(idx, 0));
    return std::clamp<LONG>((LONG)(it - c->mmStart.begin()) - 1, 0, (LONG)c->mmStart.size() - 1);
}

static bool MmCompute(EditorCtx* c, int H, MmGeo& g) {
    LONG rows, first, last;
    ViewRows(c, rows, first, last);
    if (rows <= 0 || H <= 0) return false;
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;

    g.haveModel = c->docOk && c->mmBuiltRev == c->docRev && !c->mmStart.empty();
    if (g.haveModel) {
        LONG ti = (LONG)SendMessageW(c->edit, EM_LINEINDEX, first, 0);
        LONG bi = (LONG)SendMessageW(c->edit, EM_LINEINDEX, last, 0);
        LONG t = MmLineOf(c, ti);
        LONG b = MmLineOf(c, bi);
        g.nLines = (LONG)c->mmStart.size();
        g.top = (double)t;
        g.view = (double)(b - t + 1);
    } else {
        g.nLines = rows;  // very large document: no content, but the slider still works
        g.top = (double)first;
        g.view = (double)(last - first + 1);
    }
    // While dragging, keep the viewport size measured at the start of the drag. The size we
    // measure changes by a line or two with the scroll position (wrapped lines, the end of the
    // document), and it sets the slider's size and travel. If the mapping from mouse position to
    // scroll position depended on it, the editor's own movement would shift the mapping under the
    // mouse, and near the bottom the view could flip back and forth between two positions.
    if (c->mmDrag && c->mmDragV > 0) g.view = c->mmDragV;

    g.pitch = std::max(2, MulDiv(3, dpi, 96));
    g.bh = g.pitch - std::max(1, g.pitch / 3);
    g.cap = std::max(1, H / g.pitch);
    double V = std::min<double>(g.view, (double)g.nLines);
    g.contentPx = (int)std::min<LONG>(g.nLines, g.cap) * g.pitch;
    int minSlider = std::min(g.contentPx, MulDiv(10, dpi, 96));
    g.sliderPx = std::clamp((int)std::lround(V * g.pitch), minSlider, g.contentPx);
    double f = g.nLines > V ? std::clamp(g.top / ((double)g.nLines - V), 0.0, 1.0) : 0.0;
    g.mmTop = g.nLines > g.cap ? f * (double)(g.nLines - g.cap) : 0.0;
    g.sliderTop = (int)std::lround(f * (double)(g.contentPx - g.sliderPx));
    return true;
}

// Scrolls so that logical line `line` (minimap space) becomes the first visible line.
static void MmScrollToTopLine(EditorCtx* c, const MmGeo& g, LONG line) {
    line = std::clamp<LONG>(line, 0, g.nLines - 1);
    LONG idx = g.haveModel ? (LONG)c->mmStart[(size_t)line]
                           : (LONG)SendMessageW(c->edit, EM_LINEINDEX, line, 0);
    if (idx < 0) return;
    LONG v = (LONG)SendMessageW(c->edit, EM_LINEFROMCHAR, idx, 0);  // visual row of that line
    LONG first = (LONG)SendMessageW(c->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    if (v != first) SendMessageW(c->edit, EM_LINESCROLL, 0, (LPARAM)(v - first));
}

// Scrolls to the very end. The viewport size we measure counts a half-visible bottom row, so a
// computed top line can be one row too early and leave the last line just out of view. Ask for
// more than enough and let the editor stop at its own maximum scroll position.
static void MmScrollToEnd(EditorCtx* c) {
    LONG rows = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    if (rows <= 0) return;
    RECT cr;
    GetClientRect(c->edit, &cr);
    POINT sp{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    // Dragging along the bottom edge asks for "the end" over and over; once the editor is there
    // (same scroll position, line count and size as after our last request) there is nothing to do.
    if (c->mmEndRows == rows && c->mmEndH == cr.bottom && c->mmEndY == sp.y) return;
    SendMessageW(c->edit, EM_LINESCROLL, 0, (LPARAM)rows);
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    c->mmEndRows = rows;
    c->mmEndH = cr.bottom;
    c->mmEndY = sp.y;
}

// [windhawk-reviewer] this comment used to sit above MmScrollToTopLine() by mistake.
// Scrolls the editor so that the slider's top edge lands on minimap y = yTop. The slider
// travels over [0, contentPx - sliderPx], which maps linearly onto the whole document, so
// dragging stays stable even while the minimap itself scrolls.
static void MmScrollTo(EditorCtx* c, const MmGeo& g, int yTop) {
    double V = std::min<double>(g.view, (double)g.nLines);
    if ((double)g.nLines <= V) return;
    int travel = g.contentPx - g.sliderPx;
    double f = travel > 0 ? std::clamp((double)yTop / travel, 0.0, 1.0) : 0.0;
    if (f >= 1.0) {  // slider at the very bottom
        MmScrollToEnd(c);
        return;
    }
    MmScrollToTopLine(c, g, (LONG)std::lround(f * ((double)g.nLines - V)));
}

// Mouse moves only record the wanted slider position; this applies the latest one. It runs from
// the ~60 Hz timer tick, so a fast mouse (hundreds of move events a second) can't flood the
// editor with far-away scrolls, each of which has to lay out and repaint a different part of the
// document.
static void MmApplyPendingDrag(EditorCtx* c, int H) {
    if (!c->mmDragPending) return;
    c->mmDragPending = false;
    if (!IsWindow(c->edit)) return;
    MmGeo g;
    if (MmCompute(c, H, g)) MmScrollTo(c, g, c->mmDragY - c->mmGrab);
}

// "Jump to the clicked line": what is drawn under the cursor is where you land, with that line
// centered in the editor. Unlike the slider mapping above this is not proportional to the whole
// file, so on a long document it can only reach lines the minimap is currently showing.
static void MmJumpToLine(EditorCtx* c, const MmGeo& g, LONG line) {
    double V = std::min<double>(g.view, (double)g.nLines);
    if ((double)g.nLines <= V) return;
    double top = (double)line - V / 2.0 + 0.5;  // center the line in the viewport
    if (top >= (double)g.nLines - V) {          // within the last page
        MmScrollToEnd(c);
        return;
    }
    MmScrollToTopLine(c, g, std::max<LONG>(0, (LONG)std::lround(top)));
}

// Width follows the editor's width, in steps so a live resize doesn't re-wrap the text
// on every single pixel.
static int MinimapWidth(int clientW, int dpi) {
    int minW = MulDiv(kMmMinDip, dpi, 96);
    int maxW = MulDiv(kMmMaxDip, dpi, 96);
    int step = std::max(1, MulDiv(kMmStepDip, dpi, 96));
    int w = std::clamp((int)(clientW * kMmWidthRatio + 0.5), minW, maxW);
    return std::max(minW, w / step * step);
}

static void MinimapReposition(EditorCtx* c, const RECT& cr) {
    HWND parent = GetParent(c->edit);
    if (!parent || !c->minimap) return;
    POINT p{0, 0};
    MapWindowPoints(c->edit, parent, &p, 1);
    // Right edge of the editor's client area, i.e. just left of the native scrollbar.
    RECT want = {p.x + cr.right - c->mmW, p.y, p.x + cr.right, p.y + cr.bottom};
    bool visible = IsWindowVisible(c->minimap);
    if (visible && EqualRect(&want, &c->mmPlacement)) return;
    c->mmPlacement = want;
    SetWindowPos(c->minimap, HWND_TOP, want.left, want.top, want.right - want.left,
                 want.bottom - want.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    c->mmSigValid = false;
}

static void ActivateMinimap(EditorCtx* c) {
    CaptureOriginals(c);
    c->mmActive = true;
    c->mmW = 0;  // forces the width to be applied by MinimapTick
    c->mmSigValid = false;
    c->mmBuiltRev = ~0u;
    if (!c->docOk) c->docStale = true;  // make sure a document copy gets read
}

static void DeactivateMinimap(EditorCtx* c) {
    if (c->minimap) {
        if (GetCapture() == c->minimap) ReleaseCapture();
        ShowWindow(c->minimap, SW_HIDE);
    }
    bool was = c->mmActive;
    c->mmActive = false;
    c->mmW = 0;
    c->mmPlacement = {};
    c->mmDrag = c->mmHot = false;
    c->mmDragPending = false;
    c->mmDragV = 0;
    if (was && IsWindow(c->edit)) ReleaseOrReapply(c);
}

// Minimap half of the tick. Decides whether the minimap is shown right now, keeps its width,
// position and model current, and invalidates it when something visible changed.
static void MinimapTick(EditorCtx* c) {
    RECT cr;
    GetClientRect(c->edit, &cr);
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;

    int mw = MinimapWidth(cr.right, dpi);
    // Not enough room left for the text -> hide (a little hysteresis against flicker).
    int need = MulDiv(kMmMinTextDip + (c->mmActive ? 0 : 20), dpi, 96);
    bool show = cr.right - (c->active ? c->gutterW : 0) - mw >= need;
    if (show && g_mmOnlyScroll.load()) show = EditorScrollable(c);

    if (show && !c->mmActive) {
        ActivateMinimap(c);
    } else if (!show && c->mmActive) {
        DeactivateMinimap(c);
        return;
    }
    if (!c->mmActive) return;

    if (mw != c->mmW) {
        c->mmW = mw;
        ApplyInset(c);
        c->mmPlacement = {};
        c->mmSigValid = false;
    }
    MinimapReposition(c, cr);

    if (c->mmBuiltRev != c->docRev) BuildMmLines(c);

    MmApplyPendingDrag(c, cr.bottom);

    MmGeo g;
    MmSig s;
    if (MmCompute(c, cr.bottom, g)) {
        s.top = g.top;
        s.view = g.view;
        s.nLines = g.nLines;
    }
    s.rev = c->docRev;
    s.w = c->mmW;
    s.h = cr.bottom;
    s.dark = c->dark;
    s.hot = c->mmHot || c->mmDrag;
    if (!c->mmSigValid || !SigSame(s, c->mmSig)) {
        c->mmSigValid = true;
        c->mmSig = s;
        InvalidateRect(c->minimap, nullptr, FALSE);
    }
}

// Gutter half of the tick; only runs while line numbers are on.
static void GutterTick(EditorCtx* c, bool fontChanged) {
    // Line numbers shown or hidden since the last tick? (The marker strip stays either way.)
    bool nums = g_enabled.load();
    if (nums != c->numsShown) {
        c->numsShown = nums;
        c->textDirty = true;  // recount the lines when the numbers come back
        c->gutterW = 0;       // forces the new width to be applied below
        c->sigValid = false;
    }
    if (c->mkDirty) {  // the change markers moved
        c->mkDirty = false;
        c->sigValid = false;
    }

    // Detect text changes (length / visual line count)
    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200 /*CP_UNICODE*/};
    LONG len = (LONG)SendMessageW(c->edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    ULONGLONG now = GetTickCount64();
    if (len != c->textLen || lines != c->lineCount) {
        c->textLen = len;
        c->lineCount = lines;
        c->textDirty = true;
        c->lastChangeTick = now;
    }
    if (c->textDirty) {
        c->viewCache.valid = false;
        c->caretCache.valid = false;
        // For very large files debounce the full recount while typing.
        if (!c->numsShown) {
            c->textDirty = false;  // no numbers to size, nothing to count
        } else if ((len <= 4000000 && lines <= 50000) || now - c->lastChangeTick > 200) {
            // [windhawk-reviewer] with a current document copy the line count is just the size of the
            // line table; the row-walking recount is only the fallback (see LineModelUsable)
            c->totalLogical = LineModelUsable(c) ? (LONG)c->mmStart.size() : CountBreaks(c, 0, len) + 1;
            c->textDirty = false;
        }
        c->sigValid = false;
    }

    // Dynamic width: at least 2 digits, grows at 100 / 1000 / ...
    int w;
    if (c->numsShown) {
        int digits = std::max(2, CountDigits(c->totalLogical));
        w = MeasureGutterWidth(c, digits);
    } else {
        int dpi = (int)GetDpiForWindow(c->edit);
        w = MulDiv(kMkStripDip, dpi > 0 ? dpi : 96, 96);  // just the marker strip
    }
    if (w != c->gutterW || fontChanged) {
        c->gutterW = w;
        ApplyInset(c);
        c->lastPlacement = {};
        c->sigValid = false;
    }

    Reposition(c);

    // Repaint only when something visible changed.
    LONG first = (LONG)SendMessageW(c->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    POINT scroll{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&scroll);
    DWORD s = 0, e = 0;
    SendMessageW(c->edit, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
    RECT cr;
    GetClientRect(c->edit, &cr);
    if (!c->sigValid || first != c->sFirst || scroll.y != c->sScroll.y || e != c->sSelE ||
        s != c->sSelS || cr.right != c->sW || cr.bottom != c->sH) {
        c->sigValid = true;
        c->sFirst = first;
        c->sScroll = scroll;
        c->sSelS = s;
        c->sSelE = e;
        c->sW = cr.right;
        c->sH = cr.bottom;
        InvalidateRect(c->gutter, nullptr, FALSE);
    }
}

// ---------------------------------------------------------------------------
// [windhawk-reviewer] Adaptive tick rate
//
// The tick used to run from a fixed 16 ms timer for as long as Notepad was open (one per tab,
// including hidden tabs and minimized windows), which keeps the CPU out of its low-power states.
// Now the timer only runs fast while something can change or is still settling and otherwise
// falls back to a slow backstop. Editor messages (typing, scrolling, size, show, selection) and the
// minimap's mouse input call WakeTick(); every Tick() re-picks the rate when it ends.
// ---------------------------------------------------------------------------

// ms == 0 stops the timer. SetTimer with the same id replaces the running timer.
static void SetTickRate(EditorCtx* c, UINT ms) {
    if (!c->gutter || c->tickMs == ms) return;
    c->tickMs = ms;
    if (ms) {
        if (!SetTimer(c->gutter, kTickTimer, ms, nullptr)) c->tickMs = 0;  // retry on the next call
    } else {
        KillTimer(c->gutter, kTickTimer);
    }
}

// Something happened that the next ticks must pick up quickly: run fast for a short while.
static void WakeTick(EditorCtx* c) {
    c->fastUntil = GetTickCount64() + kTickGraceMs;
    SetTickRate(c, kTickFastMs);
}

// Is there still work that only the timer tick does? Every condition here drains by itself (or
// ends with the user's input), so the timer cannot get stuck on the fast rate.
static bool TickBusy(EditorCtx* c, ULONGLONG now) {
    if (c->mmDrag || c->mmDragPending) return true;  // a minimap drag is applied by the tick
    if (GetCapture() == c->edit) return true;        // mouse selection in progress
    // The settle counters only count down in ticks that actually reach them.
    if (c->settle > 0 && (c->active || c->mmActive)) return true;
    if (c->synSettle > 0 && c->lang >= 0) return true;
    // Marker baseline still follows a document that is loading.
    if (c->mkOn && c->mkHave && !c->mkFrozen && now < c->mkFollowUntil) return true;
    // Document copy still to be read (docStale / docRetry). Mirrors the conditions under which
    // RefreshSyntax really reads it, so an unneeded copy doesn't keep the timer fast forever.
    if (c->docStale && (c->lang >= 0 || c->mmActive || c->mkOn) &&
        (size_t)c->synLen <= kSyntaxMaxChars)
        return true;
    return false;
}

// Picks the timer rate once a tick is over: fast while busy (or shortly after a wake-up), slow
// otherwise. Hidden editors (background tabs) and minimized windows only need the slow backstop.
// NOTE: hidden editors keep a slow visibility poll instead of stopping the timer completely,
// because Notepad may hide a whole tab host without sending WM_SHOWWINDOW to the editor itself.
static void SettleTickRate(EditorCtx* c) {
    if (!c->gutter) return;
    ULONGLONG now = GetTickCount64();
    bool fast = now < c->fastUntil;
    if (!fast) {
        HWND top = GetAncestor(c->edit, GA_ROOT);
        bool dormant = !IsWindowVisible(c->edit) || (top && IsIconic(top));
        fast = !dormant && TickBusy(c, now);
    }
    SetTickRate(c, fast ? kTickFastMs : kTickSlowMs);
}

static void Tick(EditorCtx* c) {
    if (c->inTick || !IsWindow(c->edit)) return;
    c->inTick = true;
    struct Guard {
        EditorCtx* c;
        ~Guard() { c->inTick = false; }
    } guard{c};
    // [windhawk-reviewer] re-pick the timer rate on every way out of this tick
    struct RateGuard {
        EditorCtx* c;
        ~RateGuard() { SettleTickRate(c); }
    } rateGuard{c};

    // [windhawk-reviewer] minimized windows: IsWindowVisible stays true for them, so skip the tick.
    {
        HWND topWnd = GetAncestor(c->edit, GA_ROOT);
        if (topWnd && IsIconic(topWnd)) return;
    }

    // [windhawk-reviewer] At the slow rate one tick stands in for ~15 fast ones: make the periodic
    // checks (title / language every 15 ticks, theme and inset self-test every 30) run on every slow
    // tick, so they keep their ~0.25-0.5 s latency instead of stretching to several seconds.
    if (c->tickMs >= kTickSlowMs && !c->inMsg) {
        c->synTick = 14;
        c->tickCount = 29;
    }

    SyntaxTick(c);

    bool wantGutter = g_enabled.load() || c->mkOn;  // markers keep the gutter alive without numbers
    if (wantGutter != c->active) {
        if (wantGutter)
            Activate(c);
        else
            Deactivate(c);
    }
    bool wantMinimap = g_minimapEnabled.load() && c->minimap;
    if (!wantMinimap && c->mmActive) DeactivateMinimap(c);
    if (!c->gutter || (!c->active && !wantMinimap)) return;

    if (!IsWindowVisible(c->edit)) {  // e.g. background tab
        if (IsWindowVisible(c->gutter)) ShowWindow(c->gutter, SW_HIDE);
        if (c->minimap && IsWindowVisible(c->minimap)) ShowWindow(c->minimap, SW_HIDE);
        c->lastPlacement = {};
        c->mmPlacement = {};
        return;
    }

    bool fontChanged = UpdateFont(c);

    // Theme (cheap, but not every tick)
    int epoch = g_themeEpoch.load();
    if (epoch != c->themeEpoch || ++c->tickCount % 30 == 0) {
        bool epochChanged = epoch != c->themeEpoch;
        c->themeEpoch = epoch;
        bool d = IsDarkTheme(c);
        if (d != c->dark || fontChanged || epochChanged) {
            c->dark = d;
            c->sigValid = false;
            c->mmSigValid = false;
            c->settle = std::max(c->settle, 40);  // the editor repaints in the new theme a bit later
        }
        // Self-test: is the editor really honoring our left inset?
        if (!c->rectFallback && c->gutterW > 0 && !InsetHonored(c, c->gutterW)) {
            if (++c->insetFailures >= 3) {
                c->rectFallback = true;
                ApplyInset(c);
            }
        } else {
            c->insetFailures = 0;
        }
    }

    // The editor may still be laying out a freshly edited line when our message-driven tick runs
    // (and the gutter sits above the editor, so it can repaint before the editor does). Keep
    // re-checking for a moment after each edit so the gutter never stays on stale row positions.
    if (c->settle > 0 && !c->inMsg) {
        c->settle--;
        c->sigValid = false;
    }

    if (c->active) GutterTick(c, fontChanged);
    if (wantMinimap && !c->inMsg) MinimapTick(c);
}

// ---------------------------------------------------------------------------
// Painting (double-buffered, so no flicker/tearing)
// ---------------------------------------------------------------------------

// Row geometry learned from rows whose position the editor reports correctly. Kept per editor
// (EditorCtx::cal*), keyed by the pixel font size (so zoom/DPI changes reset it).
// [windhawk-reviewer] these used to be shared g_cal* globals written by every painting thread; two
// windows (or two zoom levels) overwrote each other's values.

static void DrawNumbers(EditorCtx* c, HDC dc, int w, const Palette& pal) {
    HWND edit = c->edit;
    LONG lineCount = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    LONG first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    LONG v = first > 0 ? first - 1 : 0;  // one extra row for pixel-smooth scrolling
    RECT cr;
    GetClientRect(edit, &cr);

    LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
    if (idx < 0) return;
    LONG breaks = BreaksBefore(c, c->viewCache, idx);  // paragraph breaks before row v

    DWORD cs = 0, ce = 0;
    SendMessageW(edit, EM_GETSEL, (WPARAM)&cs, (LPARAM)&ce);
    LONG countedCaretLine = BreaksBefore(c, c->caretCache, (LONG)ce) + 1;  // by counting breaks

    int padR = MulDiv(10, (int)GetDpiForWindow(edit), 96);
    // Pass 1: collect visible rows (char index, y, label; label 0 = wrapped continuation).
    //
    // EM_POSFROMCHAR is unreliable in this editor for:
    //   * the whole first row (y comes back about one row too high; probing the
    //     second character doesn't help), and
    //   * the end-of-text position on a trailing empty line (y of the previous row).
    // Every other row is accurate. Rows flagged !rel are repaired in pass 1b.
    struct Row {
        LONG idx;
        LONG v;
        int y;
        LONG label;
        bool rel;
        int x;
        LONG line;  // logical line of this row (also for wrapped continuation rows)
    };
    // Length in the editor's own character numbering (a CRLF counts as one character).
    LONG docLen = ExpectedEditorLength(c);
    // Second opinion on where the text ends: the editor's own character count. A trailing empty
    // line starts exactly at the end of the text, whatever EM_LINELENGTH says about it.
    GETTEXTLENGTHEX gtlLive = {GTL_NUMCHARS, 1200 /*CP_UNICODE*/};
    LONG liveLen = (LONG)SendMessageW(edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtlLive, 0);
    if (liveLen > 0) docLen = std::min(docLen, liveLen);
    std::vector<Row> rowList;
    bool firstRow = true;
    for (; v < lineCount && rowList.size() < 500; v++) {
        LONG i = firstRow ? idx : (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
        if (i < 0) break;
        POINT pt{};
        if (!PosFromChar(edit, i, &pt)) break;
        if (pt.y > cr.bottom) break;

        // A row starts a logical line when the previous char is a paragraph
        // break; wrapped continuation rows don't, so they stay blank.
        bool isStart = (i == 0);
        if (!isStart) isStart = RowStartsParagraphByGap(edit, v, i);
        if (!firstRow && isStart) breaks++;
        firstRow = false;

        bool rel = (i > 0 && i < docLen);
        // Two different rows can never share a y: a row that does not sit below the previous one
        // got the previous row's position back (the empty line created by pressing Enter at the
        // end of the text). Treat it as unreliable so it is re-derived from its neighbours.
        if (rel && !rowList.empty() && rowList.back().v == v - 1 && pt.y <= rowList.back().y)
            rel = false;
        rowList.push_back({i, v, pt.y, isStart ? breaks + 1 : 0, rel, (int)pt.x, breaks + 1});
    }

    // Pass 1b: repair unreliable rows from reliable neighbours + a uniform row pitch.
    int rowPitch = 0;
    if (!rowList.empty()) {
        if (c->calKey != c->fontPx) {
            c->calKey = c->fontPx;
            c->calPitchValid = c->calOriginValid = false;
        }
        POINT sp{};
        SendMessageW(edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);

        int pitch = 0;
        for (size_t k = 0; k + 1 < rowList.size(); k++) {
            if (rowList[k].rel && rowList[k + 1].rel && rowList[k + 1].v == rowList[k].v + 1) {
                int d = rowList[k + 1].y - rowList[k].y;
                if (d > 0 && (pitch == 0 || d < pitch)) pitch = d;
            }
        }
        if (pitch > 0) {
            c->calPitch = pitch;
            c->calPitchValid = true;
        } else if (c->calPitchValid) {
            pitch = c->calPitch;
        } else {
            TEXTMETRICW t{};
            GetTextMetricsW(dc, &t);
            pitch = t.tmHeight + t.tmExternalLeading;  // estimate until we can measure
        }

        rowPitch = pitch;

        // Learn where row 0 sits (unscrolled) from any reliable row.
        if (c->calPitchValid) {
            for (const Row& r : rowList) {
                if (r.rel && r.idx > 0 && r.idx < docLen) {
                    c->calOrigin = r.y - (int)r.v * pitch + sp.y;
                    c->calOriginValid = true;
                    break;
                }
            }
        }

        for (int k = 0; k < (int)rowList.size(); k++) {
            if (rowList[k].rel) continue;
            int ref = -1;
            for (int d = 1; d < (int)rowList.size() && ref < 0; d++) {
                if (k - d >= 0 && rowList[k - d].rel) ref = k - d;
                else if (k + d < (int)rowList.size() && rowList[k + d].rel) ref = k + d;
            }
            if (ref >= 0) {
                rowList[k].y = rowList[ref].y + (k - ref) * pitch;
            } else if (c->calOriginValid) {
                rowList[k].y = c->calOrigin + (int)rowList[k].v * pitch - sp.y;
            } else {
                // Nothing learned yet: guess the top inset (16 DIP unless the editor
                // reports a non-zero formatting-rect top).
                RECT fr{};
                SendMessageW(edit, EM_GETRECT, 0, (LPARAM)&fr);
                int dpi = (int)GetDpiForWindow(edit);
                int top = fr.top > 0 ? fr.top : MulDiv(16, dpi > 0 ? dpi : 96, 96);
                rowList[k].y = top + (int)rowList[k].v * pitch - sp.y;
            }
        }
    }

    // Highlighted line: take it from the rows we are about to draw, so the highlight
    // is consistent with the labels. Find the row containing the caret, then walk
    // back to the row that starts its logical line. If the caret is scrolled out of
    // view (or its line starts above the view), use the break-count value instead.
    int caretRow = -1;
    for (int k = 0; k < (int)rowList.size(); k++) {
        if (rowList[k].idx <= (LONG)ce) caretRow = k;
        else break;
    }
    if (caretRow >= 0 && caretRow == (int)rowList.size() - 1) {
        // Last listed row: only valid if the caret is actually inside that row.
        LONG li = rowList[caretRow].idx;
        LONG ll = (LONG)SendMessageW(edit, EM_LINELENGTH, li, 0);
        if ((LONG)ce > li + ll) caretRow = -1;
    }
    LONG caretLine = countedCaretLine;
    if (caretRow >= 0) {
        for (int k = caretRow; k >= 0; k--) {
            if (rowList[k].label > 0) {
                caretLine = rowList[k].label;
                break;
            }
        }
    }

    // Change markers: a thin bar along the gutter's left edge for added / modified lines (all rows
    // of a wrapped line share its bar) and a small notch where lines were removed.
    if (c->mkOn && !c->mkState.empty() && rowPitch > 0) {
        int mdpi = (int)GetDpiForWindow(edit);
        if (mdpi <= 0) mdpi = 96;
        const int barW = std::max(2, MulDiv(kMkBarDip, mdpi, 96));
        const COLORREF cols[3] = {0, pal.mkAdd, pal.mkMod};
        for (const Row& r : rowList) {
            size_t li = (size_t)std::max<LONG>(r.line, 1) - 1;
            if (li >= c->mkState.size()) continue;
            uint8_t s = c->mkState[li];
            if (s == 0 || s > 2) continue;
            RECT br = {0, r.y, barW, r.y + rowPitch};
            HBRUSH mb = CreateSolidBrush(cols[s]);
            FillRect(dc, &br, mb);
            DeleteObject(mb);
        }
        if (!c->mkDel.empty()) {
            HBRUSH nb = CreateSolidBrush(pal.mkDel);
            HGDIOBJ oldBrush = SelectObject(dc, nb);
            HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
            const int hh = std::max(3, MulDiv(4, mdpi, 96));
            const int hw = std::max(barW + 2, MulDiv(5, mdpi, 96));
            auto notch = [&](int y) {
                POINT pts[3] = {{0, y - hh}, {hw, y}, {0, y + hh}};
                Polygon(dc, pts, 3);
            };
            for (const Row& r : rowList) {
                if (r.label <= 0) continue;  // only rows that start a logical line
                if (std::binary_search(c->mkDel.begin(), c->mkDel.end(),
                                       (uint32_t)std::max<LONG>(r.line - 1, 0)))
                    notch(r.y);
            }
            // Lines removed at the very end of the text: just below the last row.
            if (std::binary_search(c->mkDel.begin(), c->mkDel.end(), (uint32_t)c->mkState.size())) {
                for (const Row& r : rowList)
                    if (r.v == lineCount - 1) {
                        notch(r.y + rowPitch);
                        break;
                    }
            }
            SelectObject(dc, oldPen);
            SelectObject(dc, oldBrush);
            DeleteObject(nb);
        }
    }

    // Pass 2: draw. The current line always uses the highlight color; the setting
    // optionally adds an underline and/or bold.
    int style = g_highlightStyle.load();
    bool wantUnderline = (style == 1 || style == 3);
    bool wantBold = (style == 2 || style == 3) && c->fontBold;
    HGDIOBJ regularFont = GetCurrentObject(dc, OBJ_FONT);
    TEXTMETRICW tm{};
    GetTextMetricsW(dc, &tm);
    int ulThick = std::max(1, MulDiv(1, (int)GetDpiForWindow(edit), 96));
    for (const Row& r : rowList) {
        if (r.label <= 0 || !c->numsShown) continue;
        WCHAR b[16];
        int n = wsprintfW(b, L"%ld", r.label);
        bool cur = (r.label == caretLine);
        SetTextColor(dc, cur ? pal.current : pal.text);
        if (cur && wantBold) SelectObject(dc, c->fontBold);
        TextOutW(dc, w - padR, r.y, b, n);
        if (cur && wantUnderline) {
            SIZE sz{};
            GetTextExtentPoint32W(dc, b, n, &sz);
            HBRUSH ub = CreateSolidBrush(pal.current);
            RECT ur = {w - padR - sz.cx, r.y + tm.tmAscent + 1, w - padR,
                       r.y + tm.tmAscent + 1 + ulThick};
            FillRect(dc, &ur, ub);
            DeleteObject(ub);
        }
        if (cur && wantBold) SelectObject(dc, regularFont);
    }
}

static void PaintGutter(EditorCtx* c, HDC hdc, const RECT& rc) {
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;
    const Palette& pal = c->dark ? kDark : kLight;

    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    // With line numbers on the gutter is a panel of its own: background and separator. With only
    // the marker strip showing it must not look like a panel at all, so it takes the editor's own
    // background: the strip sits over the editor's left margin, and the blank margin right next
    // to it is copied in (that keeps whatever tint the window background has). The marker bars
    // are drawn on top.
    bool copied = false;
    if (!c->numsShown && c->gutter) {
        HDC scr = GetDC(nullptr);
        if (scr) {
            POINT o = {0, 0};
            ClientToScreen(c->gutter, &o);
            int gdpi = (int)GetDpiForWindow(c->gutter);
            int gap = MulDiv(2, gdpi > 0 ? gdpi : 96, 96);
            copied = BitBlt(mem, 0, 0, w, h, scr, o.x + w + gap, o.y, SRCCOPY) != 0;
            if (copied && GetPixel(mem, 0, h / 2) == RGB(0, 0, 0)) copied = false;  // got nothing
            ReleaseDC(nullptr, scr);
        }
    }
    if (!copied) {
        HBRUSH bg = CreateSolidBrush(c->numsShown ? pal.bg
                                                  : (c->dark ? kEditorGuessDark : kEditorGuessLight));
        FillRect(mem, &rc, bg);
        DeleteObject(bg);
        if (c->numsShown) {
            HBRUSH sep = CreateSolidBrush(pal.separator);
            RECT sr = {w - 1, 0, w, h};
            FillRect(mem, &sr, sep);
            DeleteObject(sep);
        }
    }

    if (c->font && IsWindow(c->edit)) {
        HGDIOBJ oldFont = SelectObject(mem, c->font);
        SetBkMode(mem, TRANSPARENT);
        SetTextAlign(mem, TA_RIGHT | TA_TOP | TA_NOUPDATECP);
        DrawNumbers(c, mem, w, pal);
        SelectObject(mem, oldFont);
    }

    BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

static LRESULT CALLBACK GutterProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(h, msg, wParam, lParam);
    }
    auto* c = (EditorCtx*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return HTTRANSPARENT;  // clicks / wheel go to the editor underneath
        case WM_TIMER:
            if (wParam == kTickTimer && c && !g_unloading.load()) Tick(c);  // inMsg is false here
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            RECT rc;
            GetClientRect(h, &rc);
            if (c) PaintGutter(c, dc, rc);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_NCDESTROY:
            KillTimer(h, kTickTimer);
            if (c) c->gutter = nullptr;
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Minimap (2/2): painting and mouse handling
// ---------------------------------------------------------------------------

// [windhawk-reviewer] EffClass() was an identity function and was removed.
static D2D1_COLOR_F SynColor(bool dark, int k);

struct MmCanvas {
    uint32_t* bits;
    int w, h;
};

static inline uint32_t PxOf(COLORREF c) {
    return ((uint32_t)GetRValue(c) << 16) | ((uint32_t)GetGValue(c) << 8) | (uint32_t)GetBValue(c);
}

static COLORREF MixRgb(COLORREF bg, COLORREF fg, int fgPct) {
    auto m = [&](int a, int b) { return (a * (100 - fgPct) + b * fgPct) / 100; };
    return RGB(m(GetRValue(bg), GetRValue(fg)), m(GetGValue(bg), GetGValue(fg)),
               m(GetBValue(bg), GetBValue(fg)));
}

static void MmFill(MmCanvas& cv, int x0, int y0, int x1, int y1, uint32_t col) {
    x0 = std::max(x0, 0);
    y0 = std::max(y0, 0);
    x1 = std::min(x1, cv.w);
    y1 = std::min(y1, cv.h);
    if (x0 >= x1) return;
    for (int y = y0; y < y1; y++)
        std::fill(cv.bits + (size_t)y * cv.w + x0, cv.bits + (size_t)y * cv.w + x1, col);
}

// Blends `col` over the existing pixels; alpha is 0..255.
static void MmBlend(MmCanvas& cv, int x0, int y0, int x1, int y1, uint32_t col, int alpha) {
    x0 = std::max(x0, 0);
    y0 = std::max(y0, 0);
    x1 = std::min(x1, cv.w);
    y1 = std::min(y1, cv.h);
    for (int y = y0; y < y1; y++) {
        uint32_t* row = cv.bits + (size_t)y * cv.w;
        for (int x = x0; x < x1; x++) {
            uint32_t d = row[x];
            uint32_t r = (((d >> 16) & 255) * (255 - alpha) + ((col >> 16) & 255) * alpha) / 255;
            uint32_t g = (((d >> 8) & 255) * (255 - alpha) + ((col >> 8) & 255) * alpha) / 255;
            uint32_t b = ((d & 255) * (255 - alpha) + (col & 255) * alpha) / 255;
            row[x] = (r << 16) | (g << 8) | b;
        }
    }
}

// Draws one logical line [s, e) of the document as runs of same-colored blocks. Spaces leave
// gaps, tabs count as 4 columns, and columns map onto the minimap's inner width.
static void MmDrawLine(EditorCtx* c, MmCanvas& cv, uint32_t s, uint32_t e, int y, int bh, int x0,
                       int inner, const uint32_t* clsPx) {
    const std::wstring& t = c->docText;
    const bool haveCls = c->cls.size() == t.size();
    int col = 0;
    int runStart = -1;
    uint8_t runCls = kcDefault;
    auto flush = [&](int endCol) {
        if (runStart < 0) return;
        int xa = x0 + (int)((int64_t)runStart * inner / kMmCols);
        int xb = x0 + (int)((int64_t)endCol * inner / kMmCols);
        if (xb <= xa) xb = xa + 1;
        MmFill(cv, xa, y, xb, y + bh, clsPx[runCls]);
        runStart = -1;
    };
    for (uint32_t i = s; i < e && col < kMmCols; i++) {
        WCHAR ch = t[i];
        if (ch == L' ') {
            flush(col);
            col++;
            continue;
        }
        if (ch == L'\t') {
            flush(col);
            col += 4 - (col % 4);
            continue;
        }
        uint8_t k = haveCls ? c->cls[i] : (uint8_t)kcDefault;
        if (runStart >= 0 && k != runCls) flush(col);
        if (runStart < 0) {
            runStart = col;
            runCls = k;
        }
        col++;
    }
    flush(std::min(col, kMmCols));
}

static void PaintMinimap(EditorCtx* c, HDC hdc, const RECT& rc) {
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;
    const Palette& pal = c->dark ? kDark : kLight;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    uint32_t* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        return;
    }
    HDC mem = CreateCompatibleDC(hdc);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    MmCanvas cv{bits, w, h};
    MmFill(cv, 0, 0, w, h, PxOf(pal.bg));
    MmFill(cv, 0, 0, 1, h, PxOf(pal.separator));  // divider on the side facing the text

    MmGeo g;
    if (IsWindow(c->edit) && MmCompute(c, h, g)) {
        int dpi = (int)GetDpiForWindow(c->edit);
        if (dpi <= 0) dpi = 96;
        COLORREF text = c->textColorKnown ? c->textColor
                                          : (c->dark ? RGB(0xD4, 0xD4, 0xD4) : RGB(0x33, 0x33, 0x33));

        // One color per token class (the same colors as the editor), softened toward the background.
        uint32_t clsPx[kcCount];
        for (int k = 0; k < kcCount; k++) {
            COLORREF fg = text;
            if (k != kcDefault) {
                D2D1_COLOR_F col = SynColor(c->dark, k);
                fg = RGB((int)(col.r * 255 + 0.5f), (int)(col.g * 255 + 0.5f),
                         (int)(col.b * 255 + 0.5f));
            }
            clsPx[k] = PxOf(MixRgb(pal.bg, fg, 72));
        }

        int padL = 1 + MulDiv(4, dpi, 96), padR = MulDiv(4, dpi, 96);
        int inner = std::max(1, w - padL - padR);

        if (g.haveModel) {
            const LONG k0 = (LONG)std::floor(g.mmTop);
            const int yOff = -(int)std::lround((g.mmTop - (double)k0) * g.pitch);
            const uint32_t docLen = (uint32_t)c->docText.size();
            for (LONG k = k0; k < g.nLines; k++) {
                int y = yOff + (int)(k - k0) * g.pitch;
                if (y >= h) break;
                uint32_t s = c->mmStart[(size_t)k];
                uint32_t e = (k + 1 < g.nLines) ? c->mmStart[(size_t)k + 1] - 1 : docLen;  // minus the '\r'
                e = std::min(e, docLen);
                if (e > s) MmDrawLine(c, cv, s, e, y, g.bh, padL, inner, clsPx);
            }
        }

        // The slider: a translucent band over the part of the document that is on screen.
        int alpha = (c->mmDrag || c->mmHot) ? 64 : 40;
        MmBlend(cv, 1, g.sliderTop, w, g.sliderTop + g.sliderPx, PxOf(text), alpha);
    }

    BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

static LRESULT CALLBACK MinimapProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(h, msg, wParam, lParam);
    }
    auto* c = (EditorCtx*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            RECT rc;
            GetClientRect(h, &rc);
            if (c) PaintMinimap(c, dc, rc);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!c || !IsWindow(c->edit)) break;
            WakeTick(c);  // [windhawk-reviewer] tick-rate fix: a click / drag needs the fast timer
            SetFocus(c->edit);  // keep typing in the editor
            RECT rc;
            GetClientRect(h, &rc);
            MmGeo g;
            if (MmCompute(c, rc.bottom, g)) {
                int y = GET_Y_LPARAM(lParam);
                bool onSlider = y >= g.sliderTop && y < g.sliderTop + g.sliderPx;
                if (!onSlider && g_mmClickLine.load()) {
                    // Click-to-line: the inverse of how PaintMinimap places lines (same whole-pixel
                    // scroll offset), so you land on exactly the line drawn under the cursor.
                    const LONG k0 = (LONG)std::floor(g.mmTop);
                    const int yOff = -(int)std::lround((g.mmTop - (double)k0) * g.pitch);
                    LONG line = k0 + (LONG)(std::max(0, y - yOff) / g.pitch);
                    MmJumpToLine(c, g, std::clamp<LONG>(line, 0, g.nLines - 1));
                    InvalidateRect(h, nullptr, FALSE);
                    return 0;  // a plain jump; dragging the slider itself still works
                }
                // Grab the slider where it was hit; otherwise (proportional mode) center it on
                // the click and keep dragging from there.
                c->mmGrab = onSlider ? y - g.sliderTop : g.sliderPx / 2;
                c->mmDrag = true;
                c->mmEndRows = -1;  // forget earlier "scroll to end" bookkeeping
                c->mmDragV = g.view;
                SetCapture(h);
                MmScrollTo(c, g, y - c->mmGrab);  // the first position is applied right away
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!c) break;
            if (!c->mmTracking) {
                TRACKMOUSEEVENT te{sizeof(te), TME_LEAVE, h, 0};
                if (TrackMouseEvent(&te)) c->mmTracking = true;
            }
            if (!c->mmHot) {
                c->mmHot = true;
                InvalidateRect(h, nullptr, FALSE);
            }
            if (c->mmDrag && (wParam & MK_LBUTTON) && IsWindow(c->edit)) {
                c->mmDragY = GET_Y_LPARAM(lParam);  // applied by the next timer tick
                c->mmDragPending = true;
                WakeTick(c);  // [windhawk-reviewer] tick-rate fix: apply the drag on the fast timer
            }
            return 0;
        }
        case WM_LBUTTONUP:
            if (c && c->mmDrag) {
                RECT rc;
                GetClientRect(h, &rc);
                MmApplyPendingDrag(c, rc.bottom);  // land exactly where the button was released
                c->mmDrag = false;
                c->mmDragV = 0;
                if (GetCapture() == h) ReleaseCapture();
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_CAPTURECHANGED:
            if (c && c->mmDrag) {
                c->mmDrag = false;
                c->mmDragV = 0;
                c->mmDragPending = false;
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSELEAVE:
            if (c) {
                c->mmTracking = false;
                c->mmHot = false;
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEWHEEL:  // scrolling over the minimap scrolls the document
            if (c && IsWindow(c->edit)) return SendMessageW(c->edit, WM_MOUSEWHEEL, wParam, lParam);
            break;
        case WM_NCDESTROY:
            if (c) c->minimap = nullptr;
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// System menu toggles + top-level window subclass
// ---------------------------------------------------------------------------

static void ToggleEnabled() {
    bool n = !g_enabled.load();
    g_enabled = n;
    Wh_SetIntValue(L"Enabled", n ? 1 : 0);  // persisted; each editor's tick applies it
}

static void ToggleMinimap() {
    bool n = !g_minimapEnabled.load();
    g_minimapEnabled = n;
    Wh_SetIntValue(L"MinimapEnabled", n ? 1 : 0);
}

static void RemoveSysItem(HMENU m) {
    if (!m) return;
    DeleteMenu(m, kSysCmdMinimapId, MF_BYCOMMAND);
    if (GetMenuState(m, kSysCmdId, MF_BYCOMMAND) == (UINT)-1) return;
    DeleteMenu(m, kSysCmdId, MF_BYCOMMAND);
    int cnt = GetMenuItemCount(m);
    if (cnt > 0) {
        MENUITEMINFOW mi{sizeof(mi)};
        mi.fMask = MIIM_FTYPE;
        if (GetMenuItemInfoW(m, cnt - 1, TRUE, &mi) && (mi.fType & MFT_SEPARATOR))
            DeleteMenu(m, cnt - 1, MF_BYPOSITION);
    }
}

void AttachEditor(HWND edit);

static BOOL CALLBACK DiscoverProc(HWND h, LPARAM) {
    WCHAR cls[64];
    if (GetClassNameW(h, cls, 64) && IsRichEditName(cls) &&
        GetWindowThreadProcessId(h, nullptr) == GetCurrentThreadId()) {
        AttachEditor(h);
    }
    return TRUE;
}

static LRESULT CALLBACK TopSubclassProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR, DWORD_PTR ref) {
    auto* t = (TopCtx*)ref;
    switch (msg) {
        case WM_INITMENUPOPUP: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            if (HIWORD(lParam)) {  // window (system) menu: refresh the checkmark
                CheckMenuItem(GetSystemMenu(h, FALSE), kSysCmdId,
                              MF_BYCOMMAND | (g_enabled.load() ? MF_CHECKED : MF_UNCHECKED));
                CheckMenuItem(GetSystemMenu(h, FALSE), kSysCmdMinimapId,
                              MF_BYCOMMAND | (g_minimapEnabled.load() ? MF_CHECKED : MF_UNCHECKED));
            }
            return r;
        }
        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == kSysCmdId) {
                ToggleEnabled();
                return 0;
            }
            if ((wParam & 0xFFF0) == kSysCmdMinimapId) {
                ToggleMinimap();
                return 0;
            }
            break;
        case WM_ACTIVATE:  // pick up a toggle made in another Notepad process
            if (LOWORD(wParam) != WA_INACTIVE) {
                g_enabled = Wh_GetIntValue(L"Enabled", 1) != 0;
                g_minimapEnabled = Wh_GetIntValue(L"MinimapEnabled", 1) != 0;
            }
            break;
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            g_themeEpoch++;  // live theme updates
            break;
        // [windhawk-reviewer] the WM_TIMER / kDiscoverTimer editor rescan was removed (see the constants).
        case WM_NCDESTROY: {
            RemoveWindowSubclass(h, TopSubclassProc, kTopSubclassId);
            RemovePropW(h, kPropTop);
            {
                std::lock_guard<std::mutex> lk(g_mutex);
                g_tops.erase(std::remove(g_tops.begin(), g_tops.end(), t), g_tops.end());
            }
            delete t;
            return DefSubclassProc(h, msg, wParam, lParam);
        }
    }
    return DefSubclassProc(h, msg, wParam, lParam);
}

static void AttachTop(HWND top) {
    if (!top || GetPropW(top, kPropTop)) return;
    auto* t = new TopCtx{top};
    if (!SetWindowSubclass(top, TopSubclassProc, kTopSubclassId, (DWORD_PTR)t)) {
        delete t;
        return;
    }
    SetPropW(top, kPropTop, (HANDLE)t);
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_tops.push_back(t);
    }
    if (HMENU sm = GetSystemMenu(top, FALSE)) {
        if (GetMenuState(sm, kSysCmdId, MF_BYCOMMAND) == (UINT)-1) {
            AppendMenuW(sm, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(sm, MF_STRING | (g_enabled.load() ? MF_CHECKED : 0), kSysCmdId,
                        kMenuText);
        }
        if (GetMenuState(sm, kSysCmdMinimapId, MF_BYCOMMAND) == (UINT)-1) {
            // Right below "Line Numbers".
            AppendMenuW(sm, MF_STRING | (g_minimapEnabled.load() ? MF_CHECKED : 0),
                        kSysCmdMinimapId, kMenuTextMinimap);
        }
    }
    // [windhawk-reviewer] no 750 ms discovery timer anymore: the CreateWindowExW hook plus this
    // one-time enumeration of existing child windows find every editor.
    EnumChildWindows(top, DiscoverProc, 0);
}

static void DetachTop(TopCtx* t) {
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find(g_tops.begin(), g_tops.end(), t);
        if (it == g_tops.end()) return;  // already gone
        g_tops.erase(it);
    }
    HWND top = t->hwnd;
    if (IsWindow(top)) {
        // [windhawk-reviewer] (there is no kDiscoverTimer to kill anymore)
        RemoveSysItem(GetSystemMenu(top, FALSE));
        RemoveWindowSubclass(top, TopSubclassProc, kTopSubclassId);
        RemovePropW(top, kPropTop);
    }
    delete t;
}

// ---------------------------------------------------------------------------
// Editor attach / subclass
// ---------------------------------------------------------------------------

static LRESULT CALLBACK EditSubclassProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam,
                                         UINT_PTR, DWORD_PTR ref);

static void DetachEditor(EditorCtx* c, bool windowDestroyed) {
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find(g_editors.begin(), g_editors.end(), c);
        if (it == g_editors.end()) return;
        g_editors.erase(it);
    }
    if (c->gutter) {
        KillTimer(c->gutter, kTickTimer);
        SetWindowLongPtrW(c->gutter, GWLP_USERDATA, 0);  // no dangling ctx
        DestroyWindow(c->gutter);
        c->gutter = nullptr;
    }
    if (c->minimap) {
        if (GetCapture() == c->minimap) ReleaseCapture();
        SetWindowLongPtrW(c->minimap, GWLP_USERDATA, 0);
        DestroyWindow(c->minimap);
        c->minimap = nullptr;
    }
    if (!windowDestroyed && IsWindow(c->edit)) {
        Deactivate(c);          // restores the left margin ...
        DeactivateMinimap(c);   // ... and the right one; the last of the two restores the style
        RemoveWindowSubclass(c->edit, EditSubclassProc, kEditSubclassId);
        RemovePropW(c->edit, kPropEditor);
    }
    if (c->font) DeleteObject(c->font);
    if (c->fontBold) DeleteObject(c->fontBold);
    delete c;
}

// Called from editor messages. The gutter's Activate() (which captures the editor's
// original margins) must only run from the timer, once Notepad has finished setting
// the control up; here we only advance the gutter if it is already active.
static void TickNow(EditorCtx* c) {
    // [windhawk-reviewer] every message-driven tick (edit, scroll, size, show) speeds the timer up
    // again; this is how EditSubclassProc calls SetTickRate(c, 16).
    WakeTick(c);
    bool prev = c->inMsg;
    c->inMsg = true;  // minimap work and plain-file document reads wait for the timer tick
    if (c->active || c->mmActive)
        Tick(c);
    else
        SyntaxTick(c);
    c->inMsg = prev;
}

static LRESULT CALLBACK EditSubclassProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam,
                                         UINT_PTR, DWORD_PTR ref) {
    auto* c = (EditorCtx*)ref;
    switch (msg) {
        case WM_NCDESTROY: {
            RemoveWindowSubclass(h, EditSubclassProc, kEditSubclassId);
            RemovePropW(h, kPropEditor);
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            DetachEditor(c, true);
            return r;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_DELETE || GetKeyState(VK_CONTROL) < 0) {
                c->textDirty = c->docStale = true;
                c->settle = 8;
                c->synSettle = 8;  // Ctrl+Z / Ctrl+Y / Ctrl+V ...
            }
            if (wParam == VK_RETURN || wParam == VK_BACK || wParam == VK_DELETE) {
                c->settle = 8;
                c->mkUserEdit = true;
            }
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            TickNow(c);
            return r;
        }
        case WM_CHAR:
        case WM_PASTE:
        case WM_CUT:
        case WM_CLEAR:
        case WM_UNDO:
        case EM_UNDO:
        case EM_REDO:
        case WM_SETTEXT:
        case EM_REPLACESEL:
        case EM_SETTEXTEX:
        case EM_STREAMIN:
        case WM_IME_COMPOSITION: {
            c->textDirty = c->docStale = true;
            c->settle = 8;  // ~130 ms of follow-up checks, see Tick
            c->synSettle = 8;
            // Loading a document replaces the text: the change markers start over. Anything else
            // in this group is the user editing, which freezes the baseline.
            if (msg == WM_SETTEXT || msg == EM_SETTEXTEX || msg == EM_STREAMIN)
                c->mkReload = true;
            else
                c->mkUserEdit = true;
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            TickNow(c);  // resize gutter / refresh colors in the same message, no visible lag
            return r;
        }
        case WM_VSCROLL:
        case WM_MOUSEWHEEL:
        case EM_LINESCROLL:
        case EM_SCROLL:
        case EM_SETSCROLLPOS:
        case EM_SETTARGETDEVICE:  // Notepad's word-wrap toggle
        case EM_SETZOOM: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            c->sigValid = false;
            c->synSigValid = false;
            TickNow(c);  // fresh row positions for the gutter and the syntax hook right away
            return r;
        }
        case WM_SIZE:
        case WM_MOVE:
        case WM_WINDOWPOSCHANGED:
        case WM_SHOWWINDOW: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            if ((c->active || c->mmActive) && c->rectFallback && msg == WM_SIZE)
                ApplyRect(c, c->active ? c->gutterW : 0, c->mmActive ? c->mmW : 0);
            TickNow(c);
            return r;
        }
        // [windhawk-reviewer] tick-rate fix: these move the caret / selection without any of the
        // messages above, so wake the timer for the current-line highlight and the syntax snapshot
        // (otherwise a mouse click would show up only on the next slow tick).
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case EM_SETSEL:
        case EM_EXSETSEL: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            WakeTick(c);
            return r;
        }
        case WM_MOUSEMOVE: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            if (wParam & MK_LBUTTON) WakeTick(c);  // drag-selecting; plain hovering needs no tick
            return r;
        }
    }
    return DefSubclassProc(h, msg, wParam, lParam);
}

void AttachEditor(HWND edit) {
    if (g_unloading.load() || GetPropW(edit, kPropEditor)) return;
    if (!(GetWindowLongPtrW(edit, GWL_STYLE) & ES_MULTILINE)) return;
    HWND parent = GetParent(edit);
    if (!parent) return;

    auto* c = new EditorCtx();
    c->edit = edit;
    c->uiThread = GetCurrentThreadId();
    if (!SetWindowSubclass(edit, EditSubclassProc, kEditSubclassId, (DWORD_PTR)c)) {
        delete c;
        return;
    }
    SetPropW(edit, kPropEditor, (HANDLE)c);
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_editors.push_back(c);
    }

    // Hidden sibling window stacked above the editor; it also hosts the poll timer.
    // [windhawk-reviewer] only created if the class registered; uses the mod's own module handle.
    if (g_gutterClassOk.load())
        c->gutter = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kGutterClass, L"",
                                    WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, parent, nullptr,
                                    g_hInst, c);
    // [windhawk-reviewer] start fast for a short warm-up (Activate() needs a few early ticks);
    // Tick() then picks the rate itself, see SetTickRate().
    c->fastUntil = GetTickCount64() + kTickStartMs;
    SetTickRate(c, kTickFastMs);

    // Second hidden sibling, on the right-hand side; only shown while the minimap is on.
    // [windhawk-reviewer] same here: skipped when the minimap class did not register.
    if (g_minimapClassOk.load())
        c->minimap = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kMinimapClass, L"",
                                     WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, parent, nullptr,
                                     g_hInst, c);

    HWND top = GetAncestor(edit, GA_ROOT);
    if (top && top != edit) {
        WCHAR cls[32];
        if (GetClassNameW(top, cls, 32) && _wcsicmp(cls, L"Notepad") == 0) AttachTop(top);
    }
}

// ---------------------------------------------------------------------------
// Syntax highlighting (2/2): Direct2D DrawGlyphRun hook
//
// Notepad's editor (riched20.dll) draws text with ID2D1RenderTarget::DrawGlyphRun.
// We intercept those calls, work out which document characters the glyphs are, and
// redraw the run in pieces with per-token brushes. The document is never modified.
// ---------------------------------------------------------------------------

using GlyphFn = void(STDMETHODCALLTYPE*)(void*, D2D1_POINT_2F, const DWRITE_GLYPH_RUN*, void*,
                                         DWRITE_MEASURING_MODE);
// [windhawk-reviewer] The raw vtable helpers (ComAddRef / ComRelease, CreateBrushFn / GetDpiFn /
// GetTransformFn and the vt[8] / vt[31] / vt[52] / font-face slot 11 calls) were replaced by typed COM
// calls below. Only the hooked DrawGlyphRun slot (29) is still addressed by index (GlyphHook::Install).

struct ModRange {
    uintptr_t base, end;
    bool riched;
};
struct FaceMap {
    IDWriteFontFace* face;  // [windhawk-reviewer] typed (was void*)
    std::unordered_map<UINT16, WCHAR> rev;  // glyph index -> character
};
static std::mutex g_synMutex;  // guards g_ranges, g_faces
static std::vector<ModRange> g_ranges;
static std::vector<FaceMap*> g_faces;

// Is the draw call coming from the editor control (riched20.dll)?
static bool CallerIsRiched(void* ra) {
    uintptr_t a = (uintptr_t)ra;
    std::lock_guard<std::mutex> lk(g_synMutex);
    for (const auto& r : g_ranges)
        if (a >= r.base && a < r.end) return r.riched;
    HMODULE hm = nullptr;
    bool riched = false;
    uintptr_t base = a, end = a + 1;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)ra, &hm) &&
        hm) {
        auto dos = (PIMAGE_DOS_HEADER)hm;
        auto nt = (PIMAGE_NT_HEADERS)((BYTE*)hm + dos->e_lfanew);
        base = (uintptr_t)hm;
        end = base + nt->OptionalHeader.SizeOfImage;
        WCHAR path[MAX_PATH] = {};
        GetModuleFileNameW(hm, path, MAX_PATH);
        const WCHAR* b = wcsrchr(path, L'\\');
        riched = _wcsicmp(b ? b + 1 : path, L"riched20.dll") == 0;
    }
    g_ranges.push_back({base, end, riched});
    return riched;
}

// Turns glyph indices back into characters using the font's cmap (IDWriteFontFace::
// GetGlyphIndices). Unknown glyphs become 0xFFFF.
// [windhawk-reviewer] typed COM calls instead of raw vtable slots (AddRef / Release / slot 11).
static bool DecodeRun(const DWRITE_GLYPH_RUN* run, std::wstring& S) {
    std::lock_guard<std::mutex> lk(g_synMutex);
    IDWriteFontFace* face = run->fontFace;
    FaceMap* fm = nullptr;
    for (FaceMap* f : g_faces)
        if (f->face == face) {
            fm = f;
            break;
        }
    if (!fm) {
        if (g_faces.size() >= 16) {
            g_faces.front()->face->Release();
            delete g_faces.front();
            g_faces.erase(g_faces.begin());
        }
        fm = new FaceMap();
        fm->face = face;
        face->AddRef();  // keep the pointer valid / unique while cached
        std::vector<UINT32> cps;
        for (UINT32 cp = 0x20; cp <= 0x7E; cp++) cps.push_back(cp);
        for (UINT32 cp = 0xA0; cp <= 0x17F; cp++) cps.push_back(cp);
        std::vector<UINT16> gl(cps.size(), 0);
        if (SUCCEEDED(face->GetGlyphIndices(cps.data(), (UINT32)cps.size(), gl.data()))) {
            for (size_t i = 0; i < cps.size(); i++)
                if (gl[i]) fm->rev.emplace(gl[i], (WCHAR)cps[i]);  // first mapping wins
        }
        g_faces.push_back(fm);
    }
    S.assign(run->glyphCount, L'\0');
    UINT32 unknown = 0;
    for (UINT32 i = 0; i < run->glyphCount; i++) {
        auto it = fm->rev.find(run->glyphIndices[i]);
        if (it == fm->rev.end()) {
            S[i] = 0xFFFF;
            unknown++;
        } else {
            S[i] = it->second;
        }
    }
    return unknown * 2 <= run->glyphCount;
}

static D2D1_COLOR_F SynColor(bool dark, int k) {
    static const int kD[kcCount][3] = {{0, 0, 0},       {0x56, 0x9C, 0xD6}, {0x4E, 0xC9, 0xB0},
                                       {0xCE, 0x91, 0x78}, {0x6A, 0x99, 0x55}, {0xB5, 0xCE, 0xA8},
                                       {0xC5, 0x86, 0xC0}, {0x4F, 0xC1, 0xFF}};
    static const int kL[kcCount][3] = {{0, 0, 0},       {0x00, 0x00, 0xFF}, {0x26, 0x7F, 0x99},
                                       {0xA3, 0x15, 0x15}, {0x00, 0x80, 0x00}, {0x09, 0x86, 0x58},
                                       {0xAF, 0x00, 0xDB}, {0x00, 0x70, 0xC1}};
    const int* c = dark ? kD[k] : kL[k];
    int rgb[3] = {c[0], c[1], c[2]};
    if (k == kcComment) {
        // Same muted gray as the line numbers (the ones that are not the current line).
        COLORREF g = dark ? kDark.text : kLight.text;
        rgb[0] = GetRValue(g);
        rgb[1] = GetGValue(g);
        rgb[2] = GetBValue(g);
    }
    D2D1_COLOR_F col;
    col.r = rgb[0] / 255.0f;
    col.g = rgb[1] / 255.0f;
    col.b = rgb[2] / 255.0f;
    col.a = 1.0f;
    return col;
}

// The visible, syntax-enabled editor on the calling thread (if any).
static EditorCtx* CurrentEditor() {
    DWORD tid = GetCurrentThreadId();
    std::lock_guard<std::mutex> lk(g_mutex);
    for (EditorCtx* c : g_editors)
        if (c->uiThread == tid && c->shown && c->lang >= 0 && !c->snap.empty() &&
            c->docSynLen == c->synLen && c->cls.size() == c->docText.size())
            return c;
    return nullptr;
}

// Finds S (0xFFFF = wildcard) inside one row of the document, preferring the
// occurrence closest to the estimated column.
static LONG FindInRow(const EditorCtx* c, const SnapRow& r, const std::wstring& S, double estCol) {
    LONG n = (LONG)S.size();
    LONG end = std::min<LONG>(r.end, (LONG)c->docText.size());
    LONG best = -1;
    double bestD = 1e18;
    LONG start = std::max<LONG>(r.idx, 0);
    // [windhawk-reviewer] With word wrap off a row is a whole logical line, and every glyph run used to
    // scan all of it (a minified one-line JSON file = the entire file per run, on every paint). Rows up
    // to kFindFullRowMax characters are still scanned completely, exactly as before; only longer
    // rows are searched in a window of +-kFindWindow characters around the estimated column.
    constexpr LONG kFindFullRowMax = 4096;
    constexpr LONG kFindWindow = 1024;
    if (end - start > kFindFullRowMax) {
        LONG center = (LONG)std::clamp<double>((double)r.idx + estCol, (double)start, (double)end);
        start = std::max<LONG>(start, center - kFindWindow);
        end = std::min<LONG>(end, center + kFindWindow + n);
    }
    for (LONG p = start; p + n <= end; p++) {
        bool ok = true;
        for (LONG k = 0; k < n; k++) {
            WCHAR s = S[(size_t)k];
            if (s != 0xFFFF && s != c->docText[(size_t)(p + k)]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            double d = std::fabs((double)(p - r.idx) - estCol);
            if (d < bestD) {
                bestD = d;
                best = p;
            }
        }
    }
    return best;
}

// [windhawk-reviewer] EffClass() removed (it just returned its argument). Comments get their own
// muted gray in SynColor.

// Returns true if the run was drawn (in colored pieces) by us.
static bool TryColorRun(void* self, D2D1_POINT_2F o, const DWRITE_GLYPH_RUN* run, void* brush,
                        DWRITE_MEASURING_MODE mode, GlyphFn orig) {
    EditorCtx* c = CurrentEditor();
    if (!c) return false;

    std::wstring S;
    if (!DecodeRun(run, S)) return false;
    const LONG n = (LONG)S.size();

    // Device-pixel position of the run's baseline origin.
    // [windhawk-reviewer] typed COM calls instead of raw vtable slots 52 (GetDpi) / 31 (GetTransform).
    auto* rt = static_cast<ID2D1RenderTarget*>(self);
    float dpiX = 96.f, dpiY = 96.f;
    rt->GetDpi(&dpiX, &dpiY);
    D2D1_MATRIX_3X2_F mt = {1, 0, 0, 1, 0, 0};
    rt->GetTransform(&mt);
    double scale = dpiX > 0 ? dpiX / 96.0 : 1.0;
    double xPx = (o.x * mt._11 + o.y * mt._21 + mt._31) * scale;
    double yPx = (o.x * mt._12 + o.y * mt._22 + mt._32) * scale;
    double advPx = run->glyphAdvances[0] * scale;

    if (!c->offValid) {
        c->baseOff = 0.85 * run->fontEmSize * scale;  // baseline below row top; refined below
        c->offValid = true;
    }

    // 1) Pick the row whose top best matches this baseline and verify the text,
    //    tolerating a row or two of error.
    const std::vector<SnapRow>& rows = c->snap;
    double target = yPx - c->baseOff;
    int k = -1;
    double bestDy = 1e18;
    for (int i = 0; i < (int)rows.size(); i++) {
        double d = std::fabs(rows[(size_t)i].y - target);
        if (d < bestDy) {
            bestDy = d;
            k = i;
        }
    }
    LONG found = -1;
    int foundRow = -1;
    bool trusted = false;
    if (k >= 0 && bestDy <= c->snapPitch * 1.5) {
        static const int kOrder[5] = {0, -1, 1, -2, 2};
        int matches = 0;
        for (int oi = 0; oi < 5; oi++) {
            int r = k + kOrder[oi];
            if (r < 0 || r >= (int)rows.size()) continue;
            double estCol = advPx > 0 ? (xPx - rows[(size_t)r].x) / advPx : 0.0;
            LONG p = FindInRow(c, rows[(size_t)r], S, estCol);
            if (p >= 0) {
                matches++;
                if (found < 0) {
                    found = p;
                    foundRow = r;
                }
                if (oi == 0) break;  // matched in the predicted row
            }
        }
        trusted = (foundRow == k) || (matches == 1 && n >= 4);
    }

    // 2) Fallback: search every measured row (this also recovers from a bad offset).
    if (found < 0 && n >= 2) {
        int matches = 0;
        double bestD = 1e18;
        for (int r = 0; r < (int)rows.size(); r++) {
            double estCol = advPx > 0 ? (xPx - rows[(size_t)r].x) / advPx : 0.0;
            LONG p = FindInRow(c, rows[(size_t)r], S, estCol);
            if (p >= 0) {
                matches++;
                double d = std::fabs(rows[(size_t)r].y - target);
                if (d < bestD) {
                    bestD = d;
                    found = p;
                    foundRow = r;
                }
            }
        }
        trusted = (matches == 1 && n >= 4);
    }
    if (found < 0) return false;
    if (trusted) c->baseOff = yPx - rows[(size_t)foundRow].y;

    // Leave selected text to the editor (its own selection colors). The selection is read live:
    // the editor repaints the lines whose selection just changed before our tick has noticed
    // it, so a cached copy still describes the old selection, and lines that were just
    // unselected would be left uncolored until something else forced another repaint. A caret
    // (empty selection) is not a selection.
    DWORD selS = 0, selE = 0;
    SendMessageW(c->edit, EM_GETSEL, (WPARAM)&selS, (LPARAM)&selE);
    if (selS != selE && found < (LONG)selE && found + n > (LONG)selS) return false;
    if ((size_t)(found + n) > c->cls.size()) {
        return false;
    }

    // Anything to color in this run?
    bool any = false;
    for (LONG i = 0; i < n; i++)
        if (c->cls[(size_t)(found + i)] != kcDefault) {
            any = true;
            break;
        }
    if (!any) return false;

    // Draw the run in segments of equal token class.
    ID2D1SolidColorBrush* brushes[kcCount] = {};  // [windhawk-reviewer] typed (was void* + vtable slot 8)
    bool dark = c->dark;
    float x = o.x;
    LONG i = 0;
    while (i < n) {
        uint8_t kc = c->cls[(size_t)(found + i)];
        LONG j = i + 1;
        while (j < n && c->cls[(size_t)(found + j)] == kc) j++;

        void* useBrush = brush;
        if (kc != kcDefault) {
            if (!brushes[kc]) {
                D2D1_COLOR_F col = SynColor(dark, kc);
                ID2D1SolidColorBrush* b = nullptr;
                if (SUCCEEDED(rt->CreateSolidColorBrush(col, &b)) && b) brushes[kc] = b;
            }
            if (brushes[kc]) useBrush = brushes[kc];
        }

        DWRITE_GLYPH_RUN sub = *run;
        sub.glyphCount = (UINT32)(j - i);
        sub.glyphIndices = run->glyphIndices + i;
        sub.glyphAdvances = run->glyphAdvances + i;
        sub.glyphOffsets = run->glyphOffsets ? run->glyphOffsets + i : nullptr;
        D2D1_POINT_2F so = {x, o.y};
        orig(self, so, &sub, useBrush, mode);

        for (LONG q = i; q < j; q++) x += run->glyphAdvances[q];
        i = j;
    }
    for (ID2D1SolidColorBrush* b : brushes)
        if (b) b->Release();
    return true;
}

static thread_local bool tl_inHook = false;

static void HandleGlyphRun(void* self, D2D1_POINT_2F o, const DWRITE_GLYPH_RUN* run, void* brush,
                           DWRITE_MEASURING_MODE mode, GlyphFn orig, void* ra) {
    if (!tl_inHook && run && run->fontFace && run->glyphIndices && run->glyphAdvances &&
        run->glyphCount > 0 && run->glyphCount <= 4096 && !run->isSideways &&
        !(run->bidiLevel & 1) && !g_unloading.load() && CallerIsRiched(ra)) {
        tl_inHook = true;
        bool handled = TryColorRun(self, o, run, brush, mode, orig);
        tl_inHook = false;
        if (handled) return;
    }
    orig(self, o, run, brush, mode);
}

static std::vector<void*> g_hooked;

static void HookSlot(void** vtbl, int slot, void* hook, void** orig) {
    void* target = vtbl[slot];
    for (void* h : g_hooked)
        if (h == target) return;  // implementation shared with an already-hooked class
    if (Wh_SetFunctionHook(target, hook, orig)) g_hooked.push_back(target);
}

// One instantiation per concrete render-target class (each has its own original).
template <int V>
struct GlyphHook {
    static inline GlyphFn orig = nullptr;
    static void STDMETHODCALLTYPE hook(void* self, D2D1_POINT_2F o, const DWRITE_GLYPH_RUN* run,
                                       void* brush, DWRITE_MEASURING_MODE m) {
        HandleGlyphRun(self, o, run, brush, m, orig, __builtin_return_address(0));
    }
    static void Install(void* comObject) {
        void** vtbl = *(void***)comObject;
        HookSlot(vtbl, 29 /* ID2D1RenderTarget::DrawGlyphRun */, (void*)hook, (void**)&orig);
    }
};

// Create throw-away instances of each concrete render-target class just to read
// their vtables; the code they point at is shared by every real instance.
static void InstallD2DHooks() {
    // [windhawk-reviewer] System32 only: a bare LoadLibraryW name uses the default search order, and
    // neither d2d1.dll nor d3d11.dll is a KnownDLL, so a copy next to the exe could be loaded instead.
    HMODULE d2d = LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!d2d) {
        return;
    }
    using CreateFactory_t = HRESULT(WINAPI*)(D2D1_FACTORY_TYPE, REFIID, const D2D1_FACTORY_OPTIONS*,
                                             void**);
    auto createFactory = (CreateFactory_t)GetProcAddress(d2d, "D2D1CreateFactory");
    if (!createFactory) return;

    ID2D1Factory1* f1 = nullptr;
    if (FAILED(createFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), nullptr,
                             (void**)&f1)) ||
        !f1) {
        return;
    }

    D2D1_RENDER_TARGET_PROPERTIES rp{};
    rp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    rp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    rp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    rp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    // 0: ID2D1DeviceContext (most likely for a modern, GPU-composed control)
    HMODULE d3d = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);  // [windhawk-reviewer] see above
    using D3D11CreateDevice_t = HRESULT(WINAPI*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
                                                 const D3D_FEATURE_LEVEL*, UINT, UINT,
                                                 ID3D11Device**, D3D_FEATURE_LEVEL*,
                                                 ID3D11DeviceContext**);
    auto createDev = d3d ? (D3D11CreateDevice_t)GetProcAddress(d3d, "D3D11CreateDevice") : nullptr;
    ID3D11Device* dev = nullptr;
    if (createDev) {
        if (FAILED(createDev(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                             D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, &dev,
                             nullptr, nullptr)) ||
            !dev) {
            dev = nullptr;
            createDev(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                      nullptr, 0, D3D11_SDK_VERSION, &dev, nullptr, nullptr);
        }
    }
    if (dev) {
        IDXGIDevice* dxgi = nullptr;
        if (SUCCEEDED(dev->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi)) && dxgi) {
            ID2D1Device* d2dDev = nullptr;
            if (SUCCEEDED(f1->CreateDevice(dxgi, &d2dDev)) && d2dDev) {
                ID2D1DeviceContext* ctx = nullptr;
                if (SUCCEEDED(d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ctx)) &&
                    ctx) {
                    GlyphHook<0>::Install(ctx);
                    ctx->Release();
                }
                d2dDev->Release();
            }
            dxgi->Release();
        }

        // 3: DXGI-surface render target
        D3D11_TEXTURE2D_DESC td{};
        td.Width = td.Height = 16;
        td.MipLevels = td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_RENDER_TARGET;
        ID3D11Texture2D* tex = nullptr;
        if (SUCCEEDED(dev->CreateTexture2D(&td, nullptr, &tex)) && tex) {
            IDXGISurface* surf = nullptr;
            if (SUCCEEDED(tex->QueryInterface(__uuidof(IDXGISurface), (void**)&surf)) && surf) {
                ID2D1RenderTarget* srt = nullptr;
                if (SUCCEEDED(f1->CreateDxgiSurfaceRenderTarget(surf, &rp, &srt)) && srt) {
                    GlyphHook<3>::Install(srt);
                    srt->Release();
                }
                surf->Release();
            }
            tex->Release();
        }
        dev->Release();
    }

    // 1: DC render target
    ID2D1DCRenderTarget* dc = nullptr;
    if (SUCCEEDED(f1->CreateDCRenderTarget(&rp, &dc)) && dc) {
        GlyphHook<1>::Install(dc);
        dc->Release();
    }

    // 2: HWND render target (needs a throw-away window)
    HWND tmp = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 16, 16, HWND_MESSAGE, nullptr, nullptr,
                               nullptr);
    if (tmp) {
        D2D1_HWND_RENDER_TARGET_PROPERTIES hp{};
        hp.hwnd = tmp;
        hp.pixelSize = {16, 16};
        ID2D1HwndRenderTarget* hrt = nullptr;
        if (SUCCEEDED(f1->CreateHwndRenderTarget(&rp, &hp, &hrt)) && hrt) {
            GlyphHook<2>::Install(hrt);
            hrt->Release();
        }
        DestroyWindow(tmp);
    }

    f1->Release();
}

// ---------------------------------------------------------------------------
// Hooks / entry points
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x,
                                 int y, int w, int h, HWND parent, HMENU menu,
                                 HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst,
                                         param);
    if (hwnd && cls && !IS_INTRESOURCE(cls) && !g_unloading.load()) {
        if (!(style & WS_CHILD) && _wcsicmp(cls, L"Notepad") == 0)
            AttachTop(hwnd);
        else if ((style & WS_CHILD) && IsRichEditName(cls))
            AttachEditor(hwnd);
    }
    return hwnd;
}

static void LoadSettings() {
    // [windhawk-reviewer] WindhawkUtils::StringSetting (RAII) instead of Wh_GetStringSetting /
    // Wh_FreeStringSetting. Wh_GetStringSetting never returns NULL, so the NULL checks are gone.
    int v = 0;
    {
        auto st = WindhawkUtils::StringSetting::make(L"lineNumberSettings.HighlightStyle");
        if (wcscmp(st.get(), L"underline") == 0) v = 1;
        else if (wcscmp(st.get(), L"bold") == 0) v = 2;
        else if (wcscmp(st.get(), L"both") == 0) v = 3;
    }
    g_highlightStyle = v;
    g_mmOnlyScroll = Wh_GetIntSetting(L"minimapSettings.MinimapOnlyWithScrollbar") != 0;

    auto cb = WindhawkUtils::StringSetting::make(L"minimapSettings.ClickBehavior");
    g_mmClickLine = wcscmp(cb.get(), L"proportional") != 0;  // "line" is the default
}

BOOL Wh_ModInit() {
    InstallD2DHooks();
    LoadSettings();
    g_enabled = Wh_GetIntValue(L"Enabled", 1) != 0;
    g_minimapEnabled = Wh_GetIntValue(L"MinimapEnabled", 1) != 0;
    g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    // [windhawk-reviewer] register the window classes with this mod's own module (not the exe's),
    // and remember whether that worked: AttachEditor skips creating the gutter / minimap windows
    // when a class failed to register. Falls back to the exe's handle if the lookup fails.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)&Wh_ModInit, &g_hInst) ||
        !g_hInst) {
        g_hInst = GetModuleHandleW(nullptr);
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = GutterProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kGutterClass;
    g_gutterClassOk = RegisterClassW(&wc) != 0;

    WNDCLASSW mc{};
    mc.lpfnWndProc = MinimapProc;
    mc.hInstance = g_hInst;
    mc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    mc.lpszClassName = kMinimapClass;
    g_minimapClassOk = RegisterClassW(&mc) != 0;

    // [windhawk-reviewer] WindhawkUtils::SetFunctionHook (typed) instead of void* casts.
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    return TRUE;
}

static BOOL CALLBACK EnumTopProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    WCHAR cls[32];
    if (pid == GetCurrentProcessId() && GetClassNameW(h, cls, 32) &&
        _wcsicmp(cls, L"Notepad") == 0) {
        RunFromWindowThread(h, [](void* p) { AttachTop((HWND)p); }, h);
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_themeEpoch++;  // forces a repaint of every gutter
}

// Mod loaded into an already-running Notepad: attach to existing windows.
void Wh_ModAfterInit() {
    EnumWindows(EnumTopProc, 0);
}

// Full teardown: restore margins, styles, system menu, window procs; free GDI objects.
void Wh_ModUninit() {
    g_unloading = true;
    {
        std::lock_guard<std::mutex> lk(g_synMutex);
        for (FaceMap* f : g_faces) {
            f->face->Release();  // [windhawk-reviewer] typed COM call (was ComRelease)
            delete f;
        }
        g_faces.clear();
    }

    // [windhawk-reviewer] unload race: only the HWNDs are copied while the lock is held. The old code
    // copied the context pointers, released the lock and then read c->edit / t->hwnd; if a tab or
    // window closed in between, its WM_NCDESTROY had already deleted that context. The context is now
    // looked up again on the window's own thread (GetPropW), where it cannot be deleted underneath us.
    std::vector<HWND> editors;
    std::vector<HWND> tops;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        for (EditorCtx* c : g_editors) editors.push_back(c->edit);
        for (TopCtx* t : g_tops) tops.push_back(t->hwnd);
    }
    for (HWND h : editors) {
        if (IsWindow(h))
            RunFromWindowThread(
                h,
                [](void* p) {
                    if (auto* c = (EditorCtx*)GetPropW((HWND)p, kPropEditor)) DetachEditor(c, false);
                },
                h);
    }
    for (HWND h : tops) {
        if (IsWindow(h))
            RunFromWindowThread(
                h,
                [](void* p) {
                    if (auto* t = (TopCtx*)GetPropW((HWND)p, kPropTop)) DetachTop(t);
                },
                h);
    }
    // [windhawk-reviewer] unregister with the same module handle the classes were registered with,
    // and only the ones this instance registered.
    if (g_gutterClassOk.load()) UnregisterClassW(kGutterClass, g_hInst);
    if (g_minimapClassOk.load()) UnregisterClassW(kMinimapClass, g_hInst);
}
