// ==WindhawkMod==
// @id              notepad-pro
// @name            Notepad Pro
// @description     A collection of useful tweaks, tools, and upgrades for Windows 11 Notepad
// @version         1.1.0
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
* `Supported languages`: C/C++, C#, Java, JavaScript/TypeScript, Python, JSON. More soon.
* `Non-destructive`: Purely visual, never alters your underlying files.
* `Theme-aware`: Uses tailored color palettes that automatically switch between light and dark themes.
* `Indent guides`: Draws faint vertical lines inside the indentation, one per level, and works out the indent size (tabs, 2, 4 or 8 spaces) from each file.

## Preview

Shows a live, read-only preview beside the editor for common document and data files. Right-click the tab bar and turn on Preview.

* `Markdown`: Renders headings, emphasis, lists, quotes, code, tables, and clickable links.
* `Data files`: Shows CSV and TSV as a table, and JSON, XML, and INI as an expandable tree.
* `RTF`: Displays the formatted document instead of the raw markup.
* `Live updates`: Refreshes shortly after you stop typing and follows light and dark themes.

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
#include <functional>
#include <cwctype>  // [keep] include <cwctype> directly
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <windhawk_utils.h>  // [keep] typed hook / string-setting helpers

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// Constants / globals

constexpr WCHAR kGutterClass[] = L"WhNotepadProGutter";
constexpr WCHAR kMinimapClass[] = L"WhNotepadProMinimap";
constexpr WCHAR kPropEditor[] = L"WhNotepadPro.Editor";
constexpr WCHAR kPropTop[] = L"WhNotepadPro.Top";
constexpr UINT_PTR kEditSubclassId = 0x4E505245;
constexpr UINT_PTR kTopSubclassId = 0x4E505254;
constexpr UINT kSysCmdId = 0xA1F0;
constexpr UINT_PTR kTickTimer = 1;
// [keep] adaptive tick rate instead of a fixed 16 ms timer (see SetTickRate)
constexpr UINT kTickFastMs = 16;
constexpr UINT kTickSlowMs = 250;
constexpr ULONGLONG kTickGraceMs = 300;
constexpr ULONGLONG kTickStartMs = 1000;
constexpr WCHAR kMenuText[] = L"Line Numbers";
constexpr UINT kSysCmdMinimapId = 0xA1E0;
constexpr WCHAR kMenuTextMinimap[] = L"Minimap";
constexpr WCHAR kPreviewClass[] = L"WhNotepadProPreview";
constexpr UINT kSysCmdPreviewId = 0xA1D0;
constexpr WCHAR kMenuTextPreview[] = L"Preview";

// Fallback editor background, used when sampling it from the screen fails.
constexpr COLORREF kEditorGuessDark = RGB(0x27, 0x27, 0x27);
constexpr COLORREF kEditorGuessLight = RGB(0xF9, 0xF9, 0xF9);

// Change markers
constexpr size_t kMkMaxChars = 2000000;
constexpr int kMkBarDip = 3;
constexpr int kMkStripDip = 6;
// The marker baseline follows the document for this long while it loads, unless the user edits.
constexpr ULONGLONG kMkFollowMs = 1500;

// Minimap tuning
constexpr double kMmWidthRatio = 0.10;
constexpr int kMmMinDip = 60;
constexpr int kMmMaxDip = 160;
constexpr int kMmStepDip = 4;
constexpr int kMmMinTextDip = 300;
constexpr int kMmCols = 100;

struct Palette {
    COLORREF bg, text, current, separator;
    COLORREF mkAdd, mkMod, mkDel;
};
constexpr Palette kDark = {RGB(0x2C, 0x2C, 0x2C), RGB(0x85, 0x85, 0x85),
                           RGB(0xFF, 0xFF, 0xFF), RGB(0x3E, 0x3E, 0x3E),
                           RGB(0x60, 0xAA, 0x60), RGB(0xE0, 0x96, 0x3C), RGB(0xE0, 0x5C, 0x5C)};
constexpr Palette kLight = {RGB(0xF8, 0xF8, 0xF8), RGB(0x8A, 0x8A, 0x8A),
                            RGB(0x00, 0x00, 0x00), RGB(0xE2, 0xE2, 0xE2),
                            RGB(0x34, 0xA0, 0x48), RGB(0xDC, 0x82, 0x14), RGB(0xD2, 0x3C, 0x3C)};

std::atomic<bool> g_enabled{true};
std::atomic<bool> g_minimapEnabled{true};
std::atomic<bool> g_previewEnabled{false};
std::atomic<bool> g_mmOnlyScroll{false};
std::atomic<bool> g_mmClickLine{true};
std::atomic<int> g_themeEpoch{0};
std::atomic<int> g_highlightStyle{0};
std::atomic<bool> g_unloading{false};
UINT g_runMsg = 0;

// [keep] classes use the mod's own module; windows are skipped if registration failed
HINSTANCE g_hInst = nullptr;
std::atomic<bool> g_gutterClassOk{false};
std::atomic<bool> g_minimapClassOk{false};
std::atomic<bool> g_previewClassOk{false};

std::mutex g_mutex;

// Cache of paragraph breaks before a character index.
struct CrCache {
    bool valid = false;
    LONG pos = 0;
    LONG count = 0;
};

// One measured visual row (client pixels).
struct SnapRow {
    LONG idx;
    LONG end;
    int y;
    int x;
};

// Last painted minimap state, so it repaints only on visible changes.
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

// Indent guides: what to draw for the rows around the viewport (client pixels).
struct GuideRow {
    int y;
    int h;
    int levels;
};

struct GuideModel {
    bool valid = false;
    bool dark = false;
    int x0 = 0;
    int unitPx = 0;
    int unitPitch = 0;
    int pitch = 0;
    int clientW = 0, clientH = 0;
    POINT scroll = {};
    std::vector<GuideRow> rows;
    uint64_t hash = 0;

    LONG sig[16] = {};
    bool sigValid = false;
    LONG sigLen = -1, sigLines = -1;
    int settle = 0;
    int themeTick = 0;
    int themeEpoch = -1;
};

// Preview pane content: tree nodes (JSON, XML, INI), table rows (CSV) and link ranges (Markdown).
struct PvNode {
    std::wstring text;
    std::vector<int> kids;
    bool done = false;
};

struct PvLink {
    LONG a, b;
    std::wstring url;
};

// A clickable <details> summary line: character range [a, b) and the block id.
struct PvToggle {
    LONG a, b;
    int id;
};

struct PvData {
    HWND rich = nullptr, list = nullptr, tree = nullptr;
    std::vector<PvNode> nodes;
    std::vector<int> roots;
    std::vector<std::vector<std::wstring>> rows;
    std::vector<PvLink> links;
    std::unordered_set<int> flip;
    std::vector<PvToggle> toggles;
};

struct EditorCtx {
    HWND edit = nullptr;
    HWND gutter = nullptr;
    bool active = false;
    bool addedClipSiblings = false;
    bool captured = false;
    DWORD origMargins = 0;
    bool rectFallback = false;
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
    bool shown = false;
    int lang = -2;
    int synTick = 0;
    bool docStale = true;
    std::wstring docText;
    std::vector<uint8_t> cls;
    std::vector<SnapRow> snap;
    int snapPitch = 18;
    double baseOff = 0;
    bool offValid = false;

    // Minimap
    HWND minimap = nullptr;
    bool mmActive = false;
    int mmW = 0;
    RECT mmPlacement = {};
    bool mmHot = false, mmDrag = false, mmTracking = false;
    int mmGrab = 0;
    bool mmDragPending = false;
    int mmDragY = 0;
    double mmDragV = 0;
    LONG mmEndRows = -1, mmEndY = -1;
    int mmEndH = -1;
    bool docOk = false;
    unsigned docRev = 0;
    unsigned mmBuiltRev = ~0u;
    std::vector<uint32_t> mmStart;
    bool mmSigValid = false;
    MmSig mmSig;
    bool inMsg = false;
    int settle = 0;

    // Change markers
    bool numsShown = true;
    bool mkOn = false;
    bool mkNameOk = false;
    bool mkNameKnown = false;
    std::wstring mkName, mkPendingName;
    bool mkHave = false;
    bool mkFrozen = false;
    bool mkEmptyBase = false;
    bool mkUserEdit = false;
    bool mkReload = false;
    ULONGLONG mkFollowUntil = 0;
    std::wstring mkBase;
    std::vector<uint64_t> mkBaseHash;
    std::vector<uint8_t> mkState;
    std::vector<uint32_t> mkDel;
    unsigned mkRev = ~0u;
    bool mkDirty = false;
    bool mkExactFailed = false;  // [keep] exact diff gave up for this baseline
    int mkExactSkips = 0;  // [keep] runs since then (every 32nd retries)

    int synSettle = 0;
    uint64_t snapHash = 0;
    int docRetry = 0;

    // Syntax tracking, independent of the gutter
    LONG synLen = -1, synLines = -1;
    LONG docSynLen = -2;
    ULONGLONG synChangeTick = 0;
    bool synSigValid = false;
    LONG synFirst = 0;
    POINT synScroll = {};
    DWORD synSelS = 0, synSelE = 0;
    int synW = 0, synH = 0;

    // [keep] row calibration moved from shared globals into the editor
    int calKey = 0;
    int calPitch = 0;
    int calOrigin = 0;
    bool calPitchValid = false;
    bool calOriginValid = false;

    // [keep] adaptive tick rate state
    UINT tickMs = 0;
    ULONGLONG fastUntil = 0;

    GuideModel guide;
    bool docLenient = false;
    COLORREF edBg = 0;
    int edBgKey = -1;
    int mkOff = 0;

    // Preview pane
    HWND preview = nullptr;
    bool pvActive = false;
    bool pvDark = false;
    bool pvKeep = false;
    int pvW = 0, pvKind = 0;
    unsigned pvRev = ~0u;
    ULONGLONG pvDue = 0;
    RECT pvPlacement = {};
    std::wstring pvName, pvText;
    HFONT pvFont = nullptr;
    int pvFontPx = 0;
    WCHAR pvFace[LF_FACESIZE] = {};
    PvData pv;
};

struct TopCtx {
    HWND hwnd = nullptr;
};

std::vector<EditorCtx*> g_editors;
std::vector<TopCtx*> g_tops;

// Helpers

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

// Runs proc on the window's owning thread.
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
    return name && _wcsnicmp(name, L"RichEdit", 8) == 0;
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
    // uxtheme ordinal 132 = ShouldAppsUseDarkMode (undocumented).
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

// Dark-mode order: window's dark attribute, editor text color, then the system apps theme.
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
        return lum >= 128;
    }
    return SystemAppsUseDark();
}

// Editor queries

// Caret/char position in client coordinates, for both RichEdit and Edit return styles.
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

static bool IsBreakChar(WCHAR ch) {
    return ch == L'\r' || ch == L'\n' || ch == 0x2029;
}

// A row starts a new paragraph if the previous row ended with a break, inferred from geometry.
static bool RowStartsParagraphByGap(HWND edit, LONG v, LONG idx) {
    LONG pidx = (LONG)SendMessageW(edit, EM_LINEINDEX, v - 1, 0);
    if (pidx < 0) return true;
    if (idx - pidx <= 1) return true;
    LONG plen = (LONG)SendMessageW(edit, EM_LINELENGTH, pidx, 0);
    return idx > pidx + plen;
}

// Paragraph breaks in [from, to), counted from row geometry.
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
    return CountBreaksRows(c->edit, from, to);
}

// [keep] line-count shortcut: use the minimap line table when the document copy is current, else walk rows
static LONG MmLineOf(const EditorCtx* c, LONG idx);
static void BuildMmLines(EditorCtx* c);

static bool LineModelUsable(EditorCtx* c) {
    if (!c->docOk || c->docStale) return false;
    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200};
    LONG len = (LONG)SendMessageW(c->edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    if (c->docSynLen != len) return false;
    if (c->mmBuiltRev != c->docRev) BuildMmLines(c);
    return !c->mmStart.empty();
}

// Paragraph breaks in [0, idx), walking from the cached anchor.
static LONG BreaksBefore(EditorCtx* c, CrCache& cache, LONG idx) {
    if (LineModelUsable(c)) {  // [keep] one lookup instead of walking rows
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

// Font / metrics

// Mirrors the editor's font (face, size, zoom, DPI); returns true if changed.
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
    return sz.cx + MulDiv(8, dpi, 96) + MulDiv(10, dpi, 96);
}

// Layout: reserve space inside the editor and place the gutter on top

static bool InsetHonored(EditorCtx* c, int minX) {
    POINT sp{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    if (sp.x != 0) return true;
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

// Repaint everything: stale pixels can linger in the new margin.
static void ForceEditorRedraw(EditorCtx* c) {
    RedrawWindow(c->edit, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW);
}

// Right inset = minimap + preview pane (when on).
static int RightInset(EditorCtx* c) {
    return (c->mmActive ? c->mmW : 0) + (c->pvActive ? c->pvW : 0);
}

// Left inset = gutter, right inset = RightInset.
static void ApplyInset(EditorCtx* c) {
    int origLeft = (int)LOWORD(c->origMargins);
    int origRight = (int)HIWORD(c->origMargins);
    int left = c->active ? c->gutterW : 0;
    int right = RightInset(c);
    if (c->rectFallback) {
        // Fallback for builds that ignore EM_SETMARGINS: use the formatting rect.
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

// Remembers the editor's margins and skips painting under our windows; shared by gutter and minimap.
static void CaptureOriginals(EditorCtx* c) {
    if (c->captured) return;
    LONG_PTR st = GetWindowLongPtrW(c->edit, GWL_STYLE);
    if (!(st & WS_CLIPSIBLINGS)) {
        // Lets the editor skip painting under the gutter / minimap.
        SetWindowLongPtrW(c->edit, GWL_STYLE, st | WS_CLIPSIBLINGS);
        c->addedClipSiblings = true;
    }
    c->origMargins = (DWORD)SendMessageW(c->edit, EM_GETMARGINS, 0, 0);
    c->captured = true;
}

// Re-applies margins for the remaining feature, or restores the editor's original layout.
static void ReleaseOrReapply(EditorCtx* c) {
    if (c->active || c->mmActive || c->pvActive) {
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
    c->gutterW = 0;
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

// Syntax highlighting (1/2): language detection, lexer, document snapshot

enum SynClass : uint8_t {
    kcDefault,
    kcKeyword,
    kcType,
    kcString,
    kcComment,
    kcNumber,
    kcPreproc,
    kcConst,
    // Java-only classes, one per scope group in the Java grammar.
    kcDeclKeyword,
    kcControl,
    kcModifier,
    kcPrimitive,
    kcClassName,
    kcInherit,
    kcMethodDecl,
    kcMethodCall,
    kcAnnotAt,
    kcAnnotName,
    kcThisSuper,
    kcEscape,
    kcTextBlock,
    kcCharLit,
    kcStringQuote,
    kcParam,
    kcDocTag,
    kcDocParam,
    kcDocRef,
    kcOperator,
    kcSeparator,
    kcKey,
    kcCount
};

constexpr size_t kSyntaxMaxChars = 3000000;

enum LangFlags {
    fLineSlash = 1,
    fLineHash = 2,
    fBlockC = 4,
    fTriple = 8,
    fBacktick = 16,
    fPreproc = 32,
    fAnnot = 64,
    fJsonKeys = 128,
    fDigitSep = 256,
    fJava = 512,
    fCpp = 1024,
    fCSharp = 2048,
    fJson = 4096,
    fJs = 8192,
    fTs = 16384,
    fPython = 32768
};

struct LangCfg {
    const WCHAR* exts;
    int flags;
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

    // 0: C / C++ (its word tables live in TokenizeCpp, which scans it)
    add(L"c h cpp cc cxx c++ hpp hh hxx h++ ino cu cuh",
        fLineSlash | fBlockC | fPreproc | fDigitSep | fCpp, L"", L"", L"");

    // 1: Java (its word tables live in TokenizeJava, which scans it)
    add(L"java", fJava, L"", L"", L"");

    // 2: C# (its word tables live in TokenizeCsharp, which scans it)
    add(L"cs csx", fLineSlash | fBlockC | fPreproc | fDigitSep | fCSharp, L"", L"", L"");

    // 3: JavaScript
    add(L"js jsx mjs cjs", fLineSlash | fBlockC | fBacktick | fAnnot | fJs, L"", L"", L"");

    // 4: TypeScript
    add(L"ts tsx mts cts", fLineSlash | fBlockC | fBacktick | fAnnot | fTs, L"", L"", L"");

    // 5: Python
    add(L"py pyw pyi", fLineHash | fTriple | fAnnot | fPython, L"", L"", L"");

    // 6: JSON
    add(L"json jsonc", fLineSlash | fBlockC | fJson, L"", L"", L"");
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

// Java

struct JavaWords {
    std::unordered_set<std::wstring_view> decl, control, modifiers, primitive, literal, other;
};

static JavaWords BuildJavaWords() {
    JavaWords w;
    BuildSet(w.decl, L"class interface enum import package");
    BuildSet(w.control, L"assert break case catch continue default do else finally for if return "
                        L"switch synchronized throw try while");
    BuildSet(w.modifiers, L"public protected private abstract final native static strictfp transient "
                          L"volatile extends implements throws");
    BuildSet(w.primitive, L"boolean byte char short int float long double void");
    BuildSet(w.literal, L"true false null");
    BuildSet(w.other, L"new instanceof const goto");
    return w;
}

static const JavaWords& JavaWordList() {
    static JavaWords w = BuildJavaWords();
    return w;
}

static bool JwIs(const std::wstring_view& w, const WCHAR* s) {
    const size_t n = wcslen(s);
    return w.size() == n && wcsncmp(w.data(), s, n) == 0;
}

static bool JavaUpper(WCHAR c) {
    return c < 128 ? (c >= L'A' && c <= L'Z') : iswupper(c) != 0;
}

static void JavaDocTags(const std::wstring& t, size_t a, size_t b, std::vector<uint8_t>& cls) {
    auto paint = [&](size_t p, size_t q, uint8_t k) {
        if (q > b) q = b;
        for (; p < q; p++) cls[p] = k;
    };
    auto blank = [&](size_t p, size_t e) {
        while (p < e && (t[p] == L' ' || t[p] == L'\t')) p++;
        return p;
    };
    auto wordEnd = [&](size_t p, size_t e) {
        while (p < e && t[p] != L' ' && t[p] != L'\t' && t[p] != L'}') p++;
        return p;
    };
    for (size_t line = a; line < b;) {
        size_t e = line;
        while (e < b && t[e] != L'\r' && t[e] != L'\n') e++;
        size_t p = blank(line, e);
        if (p < e && t[p] == L'*') p = blank(p + 1, e);
        if (p < e && t[p] == L'@') {
            size_t q = p + 1;
            while (q < e && IsIdPart(t[q])) q++;
            const std::wstring_view name(t.data() + p + 1, q - p - 1);
            if (!name.empty()) {
                paint(p, q, kcDocTag);
                const size_t arg = blank(q, e);
                if (arg < e && t[arg] != L'@') {
                    const size_t end = wordEnd(arg, e);
                    if (JwIs(name, L"param")) {
                        paint(arg, end, kcDocParam);
                    } else if (JwIs(name, L"see") || JwIs(name, L"throws") ||
                               JwIs(name, L"exception") || JwIs(name, L"uses") ||
                               JwIs(name, L"provides")) {
                        paint(arg, end, kcDocRef);
                    }
                }
            }
        }
        for (size_t k = line; k + 2 < e; k++) {
            if (t[k] != L'{' || t[k + 1] != L'@') continue;
            size_t q = k + 2;
            while (q < e && IsIdPart(t[q])) q++;
            const std::wstring_view name(t.data() + k + 2, q - k - 2);
            if (name.empty()) continue;
            paint(k, q, kcDocTag);
            size_t next = q;
            if (JwIs(name, L"link") || JwIs(name, L"linkplain") || JwIs(name, L"value") ||
                JwIs(name, L"inheritDoc")) {
                size_t r = q;
                while (r < e && (t[r] == L' ' || t[r] == L'\t')) r++;
                const size_t end = wordEnd(r, e);
                if (end > r) paint(r, end, kcDocRef);
                next = end;
            }
            k = next;
        }
        line = e + 1;
    }
}

static void TokenizeJava(const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    const JavaWords& W = JavaWordList();

    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > n) b = n;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    auto skipWs = [&](size_t p) {
        while (p < n && (t[p] == L' ' || t[p] == L'\t' || t[p] == L'\r' || t[p] == L'\n')) p++;
        return p;
    };
    auto nextIs = [&](size_t p, WCHAR ch) {
        p = skipWs(p);
        return p < n && t[p] == ch;
    };
    auto nextWord = [&](size_t p, std::wstring_view& out) {
        p = skipWs(p);
        const size_t a = p;
        if (p < n && IsIdStart(t[p])) {
            size_t b = p + 1;
            while (b < n && IsIdPart(t[b])) b++;
            out = std::wstring_view(t.data() + a, b - a);
            return b;
        }
        return p;
    };
    auto escapeEnd = [&](size_t q, size_t lim) {
        size_t e = q + 1;
        if (e >= lim) return e;
        if (t[e] == L'u') {
            while (e < lim && t[e] == L'u') e++;
            for (int d = 0; d < 4 && e < lim && iswxdigit((wint_t)t[e]); d++) e++;
            return e;
        }
        if (t[e] >= L'0' && t[e] <= L'7') {
            for (int d = 0; d < 3 && e < lim && t[e] >= L'0' && t[e] <= L'7'; d++) e++;
            return e;
        }
        return e + 1;
    };

    size_t i = 0;
    WCHAR prevPunct = 0;
    bool prevTypeLike = false;
    bool prevModifier = false;
    bool prevDeclKw = false;
    bool prevNew = false;
    bool prevDot = false;
    bool pendingParams = false;
    bool pendingAnnot = false;
    int parenDepth = 0;
    int braceDepth = 0;
    int paramDepth = 0;
    int annotDepth = 0;
    int moduleDepth = 0;
    bool modulePending = false;
    bool inheritMode = false;
    bool importMode = false;
    bool recordDecl = false;

    auto clear = [&] {
        prevPunct = 0;
        prevTypeLike = prevModifier = prevDeclKw = prevNew = prevDot = false;
    };
    auto opEdge = [](WCHAR ch) {
        return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' ||
               ch == L'(' || ch == L')' || ch == L';' || ch == L',';
    };
    auto spacedOp = [&](size_t p) {
        return (p == 0 || opEdge(t[p - 1])) && (p + 1 >= n || opEdge(t[p + 1]));
    };

    while (i < n) {
        const WCHAR c = t[i];
        if (c == L'\r' || c == L'\n' || c == L' ' || c == L'\t') {
            i++;
            continue;
        }

        if (c == L'/' && i + 1 < n && t[i + 1] == L'/') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            clear();
            continue;
        }
        if (c == L'/' && i + 1 < n && t[i + 1] == L'*') {
            const bool doc = i + 2 < n && t[i + 2] == L'*' && !(i + 3 < n && t[i + 3] == L'/');
            const size_t close = t.find(L"*/", i + 2);
            const size_t e = close == std::wstring::npos ? n : close + 2;
            fill(i, e, kcComment);
            if (doc) JavaDocTags(t, i + 3, close == std::wstring::npos ? n : close, cls);
            i = e;
            clear();
            continue;
        }

        if (c == L'@' && i + 1 < n && IsIdStart(t[i + 1])) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            if (JwIs(std::wstring_view(t.data() + i + 1, e - i - 1), L"interface")) {
                fill(i, e, kcDeclKeyword);
                clear();
                prevDeclKw = true;
                i = e;
                continue;
            }
            fill(i, i + 1, kcAnnotAt);
            fill(i + 1, e, kcAnnotName);
            size_t end = e;
            for (;;) {
                const size_t dot = skipWs(end);
                if (dot >= n || t[dot] != L'.') break;
                const size_t r = skipWs(dot + 1);
                if (r >= n || !IsIdStart(t[r])) break;
                size_t s = r + 1;
                while (s < n && IsIdPart(t[s])) s++;
                fill(r, s, kcAnnotName);
                end = s;
            }
            i = end;
            clear();
            pendingAnnot = nextIs(end, L'(');
            continue;
        }

        if (c == L'"' && i + 2 < n && t[i + 1] == L'"' && t[i + 2] == L'"') {
            size_t e = i + 3;
            bool closed = false;
            while (e < n) {
                if (t[e] == L'\\' && e + 1 < n) {
                    e += 2;
                    continue;
                }
                if (t[e] == L'"' && e + 2 < n && t[e + 1] == L'"' && t[e + 2] == L'"') {
                    e += 3;
                    closed = true;
                    break;
                }
                e++;
            }
            fill(i, e, kcTextBlock);
            fill(i, i + 3, kcStringQuote);
            if (closed) fill(e - 3, e, kcStringQuote);
            for (size_t q = i + 3; q + 1 < e;) {
                if (t[q] == L'\\') {
                    const size_t r = escapeEnd(q, e);
                    fill(q, r, kcEscape);
                    q = r;
                } else {
                    q++;
                }
            }
            i = e;
            clear();
            continue;
        }

        if (c == L'"') {
            size_t e = i + 1;
            while (e < n) {
                const WCHAR d = t[e];
                if (d == L'\\' && e + 1 < n && t[e + 1] != L'\r' && t[e + 1] != L'\n') {
                    e += 2;
                    continue;
                }
                if (d == L'"') {
                    e++;
                    break;
                }
                if (d == L'\r' || d == L'\n') break;
                e++;
            }
            fill(i, e, kcString);
            fill(i, i + 1, kcStringQuote);
            if (e > i + 1 && t[e - 1] == L'"') fill(e - 1, e, kcStringQuote);
            for (size_t q = i + 1; q + 1 < e;) {
                if (t[q] == L'\\') {
                    const size_t r = escapeEnd(q, e);
                    fill(q, r, kcEscape);
                    q = r;
                } else {
                    q++;
                }
            }
            i = e;
            clear();
            continue;
        }

        if (c == L'\'') {
            size_t e = i + 1;
            while (e < n) {
                const WCHAR d = t[e];
                if (d == L'\\' && e + 1 < n && t[e + 1] != L'\r' && t[e + 1] != L'\n') {
                    e += 2;
                    continue;
                }
                if (d == L'\'') {
                    e++;
                    break;
                }
                if (d == L'\r' || d == L'\n') break;
                e++;
            }
            fill(i, e, kcCharLit);
            fill(i, i + 1, kcStringQuote);
            if (e > i + 1 && t[e - 1] == L'\'') fill(e - 1, e, kcStringQuote);
            for (size_t q = i + 1; q + 1 < e;) {
                if (t[q] == L'\\') {
                    const size_t r = escapeEnd(q, e);
                    fill(q, r, kcEscape);
                    q = r;
                } else {
                    q++;
                }
            }
            i = e;
            clear();
            continue;
        }

        if (IsDigitCh(c) || (c == L'.' && i + 1 < n && IsDigitCh(t[i + 1]))) {
            const bool hex = c == L'0' && i + 1 < n && (t[i + 1] == L'x' || t[i + 1] == L'X');
            size_t e = i + 1;
            while (e < n) {
                const WCHAR d = t[e];
                if ((d < 128 && iswalnum(d)) || d == L'_' || (d >= 128 && iswalpha(d))) {
                    e++;
                    continue;
                }
                if (d == L'.' && !(e + 1 < n && t[e + 1] == L'.')) {
                    e++;
                    continue;
                }
                if (d == L'+' || d == L'-') {
                    const WCHAR p = t[e - 1];
                    if (p == L'e' || p == L'E' || (hex && (p == L'p' || p == L'P'))) {
                        e++;
                        continue;
                    }
                }
                break;
            }
            fill(i, e, kcNumber);
            i = e;
            clear();
            continue;
        }

        if (IsIdStart(c)) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            const std::wstring_view w(t.data() + i, e - i);
            const bool afterDot = prevDot;
            const WCHAR mark = prevPunct;
            const bool wasType = prevTypeLike;
            const bool wasMod = prevModifier;
            const bool wasDecl = prevDeclKw;
            const bool wasNew = prevNew;
            const bool wasRecord = recordDecl;
            clear();
            recordDecl = false;
            pendingParams = false;
            pendingAnnot = false;

            uint8_t k = kcDefault;
            bool typeLike = false;
            bool mod = false;
            bool declKw = false;
            bool isNew = false;

            if (!afterDot && JwIs(w, L"non") && e + 7 <= n && t[e] == L'-' &&
                wcsncmp(t.data() + e + 1, L"sealed", 6) == 0 &&
                (e + 7 == n || !IsIdPart(t[e + 7]))) {
                fill(i, e + 7, kcModifier);
                mod = true;
                i = e + 7;
                prevModifier = mod;
                continue;
            }

            if (JwIs(w, L"class") && afterDot) {
                k = kcThisSuper;
            } else if (W.decl.count(w)) {
                k = kcDeclKeyword;
                declKw = JwIs(w, L"class") || JwIs(w, L"interface") || JwIs(w, L"enum");
                if (JwIs(w, L"import") || JwIs(w, L"package")) importMode = true;
            } else if (W.control.count(w)) {
                k = kcControl;
            } else if (W.modifiers.count(w)) {
                k = kcModifier;
                mod = true;
                if (JwIs(w, L"extends") || JwIs(w, L"implements")) inheritMode = true;
            } else if (W.primitive.count(w)) {
                k = kcPrimitive;
                typeLike = true;
            } else if (W.literal.count(w)) {
                k = kcConst;
            } else if (W.other.count(w)) {
                k = kcKeyword;
                isNew = JwIs(w, L"new");
            } else if (JwIs(w, L"this") || JwIs(w, L"super")) {
                k = kcThisSuper;
            } else if (JwIs(w, L"record")) {
                std::wstring_view nw;
                nextWord(e, nw);
                if (!afterDot && !nw.empty() && JavaUpper(nw[0])) {
                    k = kcDeclKeyword;
                    declKw = true;
                }
            } else if (JwIs(w, L"sealed")) {
                std::wstring_view nw;
                nextWord(e, nw);
                if (!afterDot && (JwIs(nw, L"class") || JwIs(nw, L"interface") ||
                                  JwIs(nw, L"enum") || JwIs(nw, L"record") ||
                                  JwIs(nw, L"abstract") || JwIs(nw, L"final") ||
                                  JwIs(nw, L"static") || JwIs(nw, L"public") ||
                                  JwIs(nw, L"protected") || JwIs(nw, L"private") ||
                                  JwIs(nw, L"strictfp"))) {
                    k = kcModifier;
                    mod = true;
                }
            } else if (JwIs(w, L"permits")) {
                std::wstring_view nw;
                nextWord(e, nw);
                if (!afterDot && !nw.empty() && JavaUpper(nw[0])) {
                    k = kcModifier;
                    mod = true;
                    inheritMode = true;
                }
            } else if (JwIs(w, L"yield")) {
                const size_t q = skipWs(e);
                const bool plain = q < n && (t[q] == L'(' || t[q] == L'.' || t[q] == L'=' ||
                                             t[q] == L';' || t[q] == L')' || t[q] == L',' ||
                                             t[q] == L':');
                if (!afterDot && !plain) k = kcControl;
            } else if (JwIs(w, L"var")) {
                std::wstring_view nw;
                const size_t nb = nextWord(e, nw);
                const size_t q = skipWs(nb);
                if (!afterDot && !nw.empty() && q < n &&
                    (t[q] == L'=' || t[q] == L';' || t[q] == L',' || t[q] == L')' ||
                     t[q] == L':' || t[q] == L'[')) {
                    k = kcPrimitive;
                    typeLike = true;
                }
            } else if (JwIs(w, L"module")) {
                std::wstring_view nw;
                nextWord(e, nw);
                if (!afterDot && !nw.empty()) {
                    k = kcDeclKeyword;
                    modulePending = true;
                }
            } else if (JwIs(w, L"open")) {
                std::wstring_view nw;
                nextWord(e, nw);
                if (!afterDot && JwIs(nw, L"module")) {
                    k = kcModifier;
                    mod = true;
                }
            } else if (moduleDepth > 0 &&
                       (JwIs(w, L"requires") || JwIs(w, L"exports") || JwIs(w, L"opens") ||
                        JwIs(w, L"uses") || JwIs(w, L"provides") || JwIs(w, L"to") ||
                        JwIs(w, L"with") || JwIs(w, L"transitive"))) {
                k = kcDeclKeyword;
            } else if (JwIs(w, L"when")) {
                const size_t q = skipWs(e);
                const bool plain = q < n && (t[q] == L'(' || t[q] == L'.' || t[q] == L'=' ||
                                             t[q] == L';' || t[q] == L')' || t[q] == L',');
                if (!afterDot && !plain) k = kcControl;
            }

            if (k == kcDefault) {
                const size_t q = skipWs(e);
                const bool nextParen = q < n && t[q] == L'(';
                const bool paramEnd = q < n && (t[q] == L')' || t[q] == L',' || t[q] == L'[');
                if (wasDecl) {
                    k = kcClassName;
                    if (wasRecord && nextIs(e, L'(')) pendingParams = true;
                } else if (paramDepth > 0 && paramEnd) {
                    k = kcParam;
                } else if (annotDepth > 0 && q < n && t[q] == L'=') {
                    k = kcParam;
                } else if (inheritMode) {
                    k = kcInherit;
                    typeLike = true;
                } else if (nextParen) {
                    if (afterDot) {
                        k = kcMethodCall;
                    } else if (wasType || wasMod) {
                        k = kcMethodDecl;
                        pendingParams = true;
                    } else if (wasNew) {
                        k = kcType;
                        typeLike = true;
                    } else if (JavaUpper(w[0]) &&
                               (mark == 0 || mark == L'{' || mark == L'}' || mark == L';' ||
                                mark == L'(' || mark == L')')) {
                        k = kcMethodDecl;
                        pendingParams = true;
                    } else {
                        k = kcMethodCall;
                    }
                } else if (importMode && q < n && t[q] == L';') {
                    k = kcClassName;
                } else if (JavaUpper(w[0])) {
                    k = kcType;
                    typeLike = true;
                }
            }

            if (k != kcDefault) fill(i, e, k);
            i = e;
            prevTypeLike = typeLike;
            prevModifier = mod;
            prevDeclKw = declKw;
            prevNew = isNew;
            recordDecl = k == kcDeclKeyword && JwIs(w, L"record");
            if ((k == kcDeclKeyword || k == kcControl || k == kcModifier ||
                 k == kcPrimitive || k == kcConst || k == kcThisSuper || k == kcKeyword) &&
                !(JwIs(w, L"extends") || JwIs(w, L"implements") || JwIs(w, L"permits")))
                inheritMode = false;
            continue;
        }

        if (c == L'(') {
            inheritMode = false;
            parenDepth++;
            if (pendingParams) {
                paramDepth = parenDepth;
                pendingParams = false;
            }
            if (pendingAnnot) {
                annotDepth = parenDepth;
                pendingAnnot = false;
            }
            clear();
            prevPunct = L'(';
            i++;
            continue;
        }
        if (c == L')') {
            if (paramDepth == parenDepth) paramDepth = 0;
            if (annotDepth == parenDepth) annotDepth = 0;
            if (parenDepth > 0) parenDepth--;
            clear();
            prevPunct = L')';
            i++;
            continue;
        }
        if (c == L'{') {
            braceDepth++;
            if (modulePending) {
                moduleDepth = braceDepth;
                modulePending = false;
            }
            inheritMode = false;
            clear();
            prevPunct = L'{';
            i++;
            continue;
        }
        if (c == L'}') {
            if (moduleDepth == braceDepth) moduleDepth = 0;
            if (braceDepth > 0) braceDepth--;
            inheritMode = false;
            clear();
            prevPunct = L'}';
            i++;
            continue;
        }
        if (c == L';') {
            inheritMode = false;
            importMode = false;
            modulePending = false;
            clear();
            prevPunct = L';';
            i++;
            continue;
        }
        if (c == L'.') {
            clear();
            prevDot = true;
            i++;
            continue;
        }
        {
            const WCHAR d1 = i + 1 < n ? t[i + 1] : 0;
            size_t len = 0, skip = 0;
            switch (c) {
            case L'=': len = d1 == L'=' ? 2 : 1; break;
            case L'!': len = d1 == L'=' ? 2 : 1; break;
            case L'<':
                if (d1 == L'=') len = 2;
                else if (d1 == L'<') {
                    skip = 2;
                    while (i + skip < n && t[i + skip] == L'<') skip++;
                    if (i + skip < n && t[i + skip] == L'=') skip++;
                }
                break;
            case L'>':
                if (d1 == L'=') len = 2;
                else if (d1 == L'>') {
                    skip = 2;
                    while (i + skip < n && t[i + skip] == L'>') skip++;
                    if (i + skip < n && t[i + skip] == L'=') skip++;
                }
                break;
            case L'+': len = (d1 == L'+' || d1 == L'=') ? 2 : 1; break;
            case L'-': len = (d1 == L'-' || d1 == L'=' || d1 == L'>') ? 2 : 1; break;
            case L'*': case L'/': case L'%': case L'&': case L'|': case L'^':
                len = (d1 == L'=' || ((c == L'&' || c == L'|') && d1 == c)) ? 2 : 1;
                break;
            case L'~': len = 1; break;
            default: break;
            }
            if (skip > 0) {
                clear();
                prevPunct = c;
                i += skip;
                continue;
            }
            if (len == 0 && (c == L'<' || c == L'>') && spacedOp(i)) len = 1;
            if (len > 0) {
                fill(i, i + len, kcOperator);
                clear();
                prevPunct = c;
                i += len;
                continue;
            }
        }
        if (c == L'>' || c == L']') {
            if (c == L'>' && !nextIs(i + 1, L',')) inheritMode = false;
            clear();
            prevTypeLike = true;
            prevPunct = c;
            i++;
            continue;
        }
        clear();
        prevPunct = c;
        i++;
    }
}

// C / C++

struct CppWords {
    std::unordered_set<std::wstring_view> control, decl, modifier, primitive, type, literal,
        opword, misc;
};

static CppWords BuildCppWords() {
    CppWords w;
    BuildSet(w.control, L"break case catch continue default do else for goto if return switch "
                        L"throw try while co_await co_return co_yield new delete using operator");
    BuildSet(w.decl, L"class struct enum union concept");
    BuildSet(w.modifier, L"static extern register thread_local _Thread_local constexpr consteval "
                         L"constinit inline restrict __restrict __restrict__ _Noreturn noreturn "
                         L"const volatile _Atomic export friend explicit virtual mutable typename "
                         L"final override noexcept public private protected");
    BuildSet(w.primitive, L"asm __asm__ auto bool _Bool char _Complex double float _Imaginary int "
                          L"long short signed unsigned void");
    BuildSet(w.type, L"wchar_t char8_t char16_t char32_t size_t ssize_t ptrdiff_t intptr_t "
                     L"uintptr_t int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
                     L"string wstring string_view vector map set unordered_map unordered_set list "
                     L"deque array pair tuple optional variant unique_ptr shared_ptr weak_ptr "
                     L"function __int8 __int16 __int32 __int64 BOOL BOOLEAN BYTE CHAR DWORD DWORD32 "
                     L"DWORD64 DWORD_PTR FLOAT HANDLE HINSTANCE HKEY HKL HMODULE HRESULT HWND INT "
                     L"INT8 INT16 INT32 INT64 LANGID LCID LONG LONGLONG LONG_PTR LPARAM LPCSTR "
                     L"LPCWSTR LPSTR LPVOID LPWSTR LRESULT PSTR PVOID QWORD SHORT SIZE_T SSIZE_T "
                     L"TCHAR UCHAR UINT UINT8 UINT16 UINT32 UINT64 ULONG ULONGLONG ULONG_PTR USHORT "
                     L"VOID WCHAR WINAPI WORD WPARAM");
    BuildSet(w.literal, L"true false TRUE FALSE NULL nullptr __func__ __FILE__ __FUNCTION__ "
                        L"__LINE__");
    BuildSet(w.opword, L"sizeof alignof _Alignof _Countof _Generic static_assert _Static_assert "
                       L"const_cast dynamic_cast static_cast reinterpret_cast and and_eq bitand "
                       L"bitor compl not not_eq or or_eq xor xor_eq");
    BuildSet(w.misc, L"namespace template typedef typeid alignas _Alignas __declspec __attribute__");
    return w;
}

static const CppWords& CppWordList() {
    static CppWords w = BuildCppWords();
    return w;
}

static bool SameWord(const std::wstring_view& a, const std::wstring_view& b) {
    return a.size() == b.size() && (a.empty() || wcsncmp(a.data(), b.data(), a.size()) == 0);
}

static void TokenizeCpp(const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    const CppWords& W = CppWordList();

    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > n) b = n;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    auto skipWs = [&](size_t p) {
        while (p < n && (t[p] == L' ' || t[p] == L'\t' || t[p] == L'\r' || t[p] == L'\n')) p++;
        return p;
    };
    auto escapeEnd = [&](size_t q, size_t lim) {
        size_t e = q + 1;
        if (e >= lim) return e;
        if (t[e] == L'x' || t[e] == L'X') {
            e++;
            while (e < lim && iswxdigit((wint_t)t[e])) e++;
            return e;
        }
        if (t[e] == L'u' || t[e] == L'U') {
            const int max = t[e] == L'U' ? 8 : 4;
            e++;
            for (int d = 0; d < max && e < lim && iswxdigit((wint_t)t[e]); d++) e++;
            return e;
        }
        if (t[e] >= L'0' && t[e] <= L'7') {
            for (int d = 0; d < 3 && e < lim && t[e] >= L'0' && t[e] <= L'7'; d++) e++;
            return e;
        }
        return e + 1;
    };

    size_t i = 0;
    bool lineStart = true;
    bool prevTypeLike = false;
    bool prevModifier = false;
    bool prevDeclKw = false;
    bool prevOperatorKw = false;
    bool prevTemplateKw = false;
    bool prevDot = false;
    bool prevColonColon = false;
    bool pendingParams = false;
    int parenDepth = 0;
    int paramDepth = 0;
    std::wstring_view qualifier;

    auto reset = [&] {
        prevTypeLike = prevModifier = prevDeclKw = false;
        prevOperatorKw = prevTemplateKw = false;
        prevDot = prevColonColon = false;
    };
    auto opEdge = [](WCHAR ch) {
        return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' ||
               ch == L'(' || ch == L')' || ch == L';' || ch == L',';
    };
    auto spacedOp = [&](size_t p, size_t len) {
        return (p == 0 || opEdge(t[p - 1])) && (p + len >= n || opEdge(t[p + len]));
    };

    while (i < n) {
        const WCHAR c = t[i];
        if (c == L'\r' || c == L'\n') {
            i++;
            lineStart = true;
            continue;
        }
        if (c == L' ' || c == L'\t') {
            i++;
            continue;
        }
        const bool atStart = lineStart;
        lineStart = false;

        if (c == L'/' && i + 1 < n && t[i + 1] == L'/') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            reset();
            continue;
        }
        if (c == L'/' && i + 1 < n && t[i + 1] == L'*') {
            size_t e = t.find(L"*/", i + 2);
            e = (e == std::wstring::npos) ? n : e + 2;
            fill(i, e, kcComment);
            i = e;
            reset();
            continue;
        }

        if (c == L'#' && atStart) {
            size_t e = i + 1;
            while (e < n && (t[e] == L' ' || t[e] == L'\t')) e++;
            const size_t ws = e;
            while (e < n && IsIdPart(t[e])) e++;
            fill(i, e, kcPreproc);
            if (t.compare(ws, e - ws, L"include") == 0) {
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
            reset();
            continue;
        }

        {
            size_t sp = i;
            if ((c == L'L' || c == L'u' || c == L'U' || c == L'R') && i + 1 < n) {
                size_t q = i + 1;
                if (c == L'u' && q < n && t[q] == L'8') q++;
                if (q < n && (t[q] == L'"' || t[q] == L'\'')) sp = q;
            }
            if (t[sp] == L'"' || t[sp] == L'\'') {
                const WCHAR quote = t[sp];
                size_t e;
                bool raw = false;
                if (quote == L'"' && sp > i && t[sp - 1] == L'R' && sp + 1 < n &&
                    t[sp + 1] == L'(') {
                    const size_t close = t.find(L")\"", sp + 2);
                    e = close == std::wstring::npos ? n : close + 2;
                    raw = true;
                } else {
                    e = sp + 1;
                    while (e < n) {
                        const WCHAR d = t[e];
                        if (d == L'\\' && e + 1 < n && t[e + 1] != L'\r' && t[e + 1] != L'\n') {
                            e += 2;
                            continue;
                        }
                        if (d == quote) {
                            e++;
                            break;
                        }
                        if (d == L'\r' || d == L'\n') break;
                        e++;
                    }
                }
                fill(i, e, quote == L'\'' ? kcCharLit : kcString);
                if (sp > i) fill(i, sp, kcPrimitive);
                fill(sp, sp + 1, kcStringQuote);
                if (e > sp + 1 && t[e - 1] == quote) fill(e - 1, e, kcStringQuote);
                if (!raw) {
                    for (size_t q = sp + 1; q + 1 < e;) {
                        if (t[q] == L'\\') {
                            const size_t r = escapeEnd(q, e);
                            fill(q, r, kcEscape);
                            q = r;
                        } else {
                            q++;
                        }
                    }
                }
                i = e;
                reset();
                continue;
            }
        }

        if (IsDigitCh(c) || (c == L'.' && i + 1 < n && IsDigitCh(t[i + 1]))) {
            const bool hex = c == L'0' && i + 1 < n && (t[i + 1] == L'x' || t[i + 1] == L'X');
            size_t e = i + 1;
            while (e < n) {
                const WCHAR d = t[e];
                if ((d < 128 && iswalnum(d)) || d == L'_' || (d >= 128 && iswalpha(d))) {
                    e++;
                    continue;
                }
                if (d == L'.' && !(e + 1 < n && t[e + 1] == L'.')) {
                    e++;
                    continue;
                }
                if (d == L'\'' && e + 1 < n && IsIdPart(t[e + 1])) {
                    e++;
                    continue;
                }
                if (d == L'+' || d == L'-') {
                    const WCHAR p = t[e - 1];
                    if (p == L'e' || p == L'E' || (hex && (p == L'p' || p == L'P'))) {
                        e++;
                        continue;
                    }
                }
                break;
            }
            fill(i, e, kcNumber);
            i = e;
            reset();
            continue;
        }

        if (IsIdStart(c)) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            const std::wstring_view w(t.data() + i, e - i);
            const bool afterDot = prevDot;
            const bool afterColon = prevColonColon;
            const bool wasType = prevTypeLike;
            const bool wasMod = prevModifier;
            const bool wasDecl = prevDeclKw;
            reset();

            uint8_t k = kcDefault;
            bool typeLike = false;
            bool mod = false;
            bool declKw = false;

            if (afterDot) {
            } else if (W.control.count(w)) {
                k = kcControl;
            } else if (W.decl.count(w)) {
                k = kcDeclKeyword;
                declKw = true;
            } else if (W.modifier.count(w)) {
                k = kcModifier;
                mod = true;
            } else if (W.primitive.count(w)) {
                k = kcPrimitive;
                typeLike = true;
            } else if (W.type.count(w)) {
                k = kcType;
                typeLike = true;
            } else if (W.literal.count(w)) {
                k = kcConst;
            } else if (W.opword.count(w)) {
                k = kcOperator;
            } else if (W.misc.count(w)) {
                k = kcKeyword;
            } else if (JwIs(w, L"this")) {
                k = kcThisSuper;
            }

            if (k == kcDefault) {
                const size_t q = skipWs(e);
                const bool nextParen = q < n && t[q] == L'(';
                const bool paramEnd = q < n && (t[q] == L')' || t[q] == L',' ||
                                                t[q] == L'[' || t[q] == L'=');
                if (wasDecl) {
                    k = kcClassName;
                } else if (paramDepth > 0 && parenDepth == paramDepth && paramEnd) {
                    k = kcParam;
                } else if (nextParen) {
                    if (afterDot) {
                        k = kcMethodCall;
                    } else if (afterColon) {
                        if (wasType || SameWord(w, qualifier)) {
                            k = kcMethodDecl;
                            pendingParams = true;
                        } else {
                            k = kcMethodCall;
                        }
                    } else if (wasType || wasMod) {
                        k = kcMethodDecl;
                        pendingParams = true;
                    } else {
                        k = kcMethodCall;
                    }
                } else if (wasType) {
                    typeLike = true;
                }
            }

            if (k != kcDefault) fill(i, e, k);
            i = e;
            prevTypeLike = typeLike;
            prevModifier = mod;
            prevDeclKw = declKw;
            prevOperatorKw = JwIs(w, L"operator");
            prevTemplateKw = JwIs(w, L"template");
            qualifier = w;
            continue;
        }

        if (c == L'(') {
            parenDepth++;
            if (pendingParams) {
                paramDepth = parenDepth;
                pendingParams = false;
            }
            reset();
            i++;
            continue;
        }
        if (c == L')') {
            if (paramDepth == parenDepth) paramDepth = 0;
            if (parenDepth > 0) parenDepth--;
            reset();
            i++;
            continue;
        }
        if (c == L'{' || c == L'}' || c == L';') {
            paramDepth = 0;
            pendingParams = false;
            reset();
            i++;
            continue;
        }
        if (c == L'.') {
            reset();
            prevDot = true;
            i++;
            continue;
        }
        if (c == L'-' && i + 1 < n && t[i + 1] == L'>') {
            reset();
            prevDot = true;
            i += 2;
            continue;
        }
        if (c == L':' && i + 1 < n && t[i + 1] == L':') {
            const bool wasType = prevTypeLike || prevModifier;
            reset();
            prevColonColon = true;
            prevTypeLike = wasType;
            i += 2;
            continue;
        }

        {
            const WCHAR d1 = i + 1 < n ? t[i + 1] : 0;
            const WCHAR d2 = i + 2 < n ? t[i + 2] : 0;
            size_t len = 0;
            bool skip = false;
            switch (c) {
            case L'=': len = d1 == L'=' ? 2 : 1; break;
            case L'!': len = d1 == L'=' ? 2 : 1; break;
            case L'<':
                if (d1 == L'=' && d2 == L'>') len = 3;
                else if (d1 == L'=') len = 2;
                else if (d1 == L'<' && d2 == L'=') len = 3;
                else if (d1 == L'<' && !prevTemplateKw) len = 2;
                break;
            case L'>':
                if (d1 == L'=') len = 2;
                else if (d1 == L'>' && d2 == L'=') len = 3;
                else if (d1 == L'>') {
                    if (spacedOp(i, 2)) len = 2;
                    else skip = true;
                }
                break;
            case L'+': len = (d1 == L'+' || d1 == L'=') ? 2 : 1; break;
            case L'-': len = (d1 == L'-' || d1 == L'=') ? 2 : 1; break;
            case L'*': case L'/': case L'%': case L'&': case L'|': case L'^':
                len = (d1 == L'=' || ((c == L'&' || c == L'|') && d1 == c)) ? 2 : 1;
                break;
            case L'~': len = 1; break;
            default: break;
            }
            if (skip) {
                reset();
                i += 2;
                continue;
            }
            if (len == 0 && (c == L'<' || c == L'>') && !prevOperatorKw && !prevTemplateKw &&
                spacedOp(i, 1))
                len = 1;
            if (len > 0) {
                fill(i, i + len, kcOperator);
                const bool keepType = prevTypeLike || prevModifier;
                const bool pointer = c == L'*' || c == L'&';
                reset();
                if (pointer) prevTypeLike = keepType;
                i += len;
                continue;
            }
        }
        reset();
        i++;
    }
}

// C#

struct CSharpWords {
    std::unordered_set<std::wstring_view> control, decl, accessor, modifier, primitive, type,
        literal, other;
};

static CSharpWords BuildCSharpWords() {
    CSharpWords w;
    BuildSet(w.control, L"break case catch continue default do else finally for foreach goto if "
                        L"lock return switch throw try while await yield");
    BuildSet(w.decl, L"class struct interface namespace using alias enum record union delegate event fixed");
    BuildSet(w.accessor, L"get set init add remove");
    BuildSet(w.modifier, L"abstract async const explicit extern implicit override partial readonly "
                         L"required scoped sealed static unsafe virtual volatile in out params ref "
                         L"where file allows unmanaged notnull");
    BuildSet(w.modifier, L"internal private protected public");
    BuildSet(w.primitive, L"bool byte sbyte char decimal double float int uint nint nuint long "
                          L"ulong short ushort dynamic object string void var");
    BuildSet(w.type, L"String Object Boolean Byte SByte Char Decimal Double Single Int16 Int32 "
                     L"Int64 UInt16 UInt32 UInt64 IntPtr UIntPtr Guid DateTime TimeSpan Exception "
                     L"Task ValueTask List Dictionary HashSet SortedSet Queue Stack IEnumerable "
                     L"IList IDictionary ICollection IDisposable IReadOnlyList Span Memory Array "
                     L"Console Math Regex StringBuilder Stream Path File Directory Encoding "
                     L"Convert Tuple Nullable Attribute Action Func Predicate Comparison Converter "
                     L"Marshal Environment Random Uri Version Type Activator Thread Monitor "
                     L"Interlocked CancellationToken CancellationTokenSource Lazy");
    BuildSet(w.literal, L"true false null");
    BuildSet(w.other, L"checked unchecked nameof sizeof typeof stackalloc");
    return w;
}

static const CSharpWords& CSharpWordList() {
    static CSharpWords w = BuildCSharpWords();
    return w;
}

static void TokenizeCsharp(const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    const CSharpWords& W = CSharpWordList();

    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > n) b = n;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    auto skipWs = [&](size_t p) {
        while (p < n && (t[p] == L' ' || t[p] == L'\t' || t[p] == L'\r' || t[p] == L'\n')) p++;
        return p;
    };
    auto escapeEnd = [&](size_t q, size_t lim) {
        size_t e = q + 1;
        if (e >= lim) return e;
        if (t[e] == L'x' || t[e] == L'X') {
            e++;
            while (e < lim && iswxdigit((wint_t)t[e])) e++;
            return e;
        }
        if (t[e] == L'u' || t[e] == L'U') {
            const int max = t[e] == L'U' ? 8 : 4;
            e++;
            for (int d = 0; d < max && e < lim && iswxdigit((wint_t)t[e]); d++) e++;
            return e;
        }
        return e + 1;
    };
    auto numberEnd = [&](size_t p, size_t lim) {
        size_t e = p + 1;
        while (e < lim) {
            const WCHAR d = t[e];
            if ((d < 128 && iswalnum(d)) || d == L'_' || (d >= 128 && iswalpha(d))) {
                e++;
                continue;
            }
            if (d == L'.' && !(e + 1 < lim && t[e + 1] == L'.')) {
                e++;
                continue;
            }
            if ((d == L'+' || d == L'-') && (t[e - 1] == L'e' || t[e - 1] == L'E')) {
                e++;
                continue;
            }
            break;
        }
        return e;
    };
    auto colourHole = [&](size_t a, size_t b) {
        size_t p = a;
        while (p < b) {
            const WCHAR d = t[p];
            if (d == L' ' || d == L'\t' || d == L'\r' || d == L'\n') {
                p++;
                continue;
            }
            if (d == L'"') {
                size_t e = p + 1;
                while (e < b && t[e] != L'"' && t[e] != L'\r' && t[e] != L'\n') {
                    if (t[e] == L'\\' && e + 1 < b) e++;
                    e++;
                }
                if (e < b && t[e] == L'"') e++;
                fill(p, e, kcString);
                fill(p, p + 1, kcStringQuote);
                if (e > p + 1 && t[e - 1] == L'"') fill(e - 1, e, kcStringQuote);
                p = e;
                continue;
            }
            if (IsDigitCh(d)) {
                const size_t e = numberEnd(p, b);
                fill(p, e, kcNumber);
                p = e;
                continue;
            }
            if (IsIdStart(d)) {
                size_t e = p + 1;
                while (e < b && IsIdPart(t[e])) e++;
                const std::wstring_view w2(t.data() + p, e - p);
                uint8_t k = kcDefault;
                if (W.control.count(w2)) k = kcControl;
                else if (W.literal.count(w2)) k = kcConst;
                else if (W.primitive.count(w2)) k = kcPrimitive;
                else if (W.type.count(w2)) k = kcType;
                else if (W.modifier.count(w2)) k = kcModifier;
                if (k != kcDefault) fill(p, e, k);
                p = e;
                continue;
            }
            size_t len = 0;
            switch (d) {
            case L'=': len = (p + 1 < b && t[p + 1] == L'=') ? 2 : 1; break;
            case L'!': len = (p + 1 < b && t[p + 1] == L'=') ? 2 : 1; break;
            case L'<': len = (p + 1 < b && t[p + 1] == L'=') ? 2 : 1; break;
            case L'>': len = (p + 1 < b && (t[p + 1] == L'=' || t[p + 1] == L'>')) ? 2 : 1; break;
            case L'+': case L'-': case L'*': case L'/': case L'%':
            case L'&': case L'|': case L'^': case L'?':
                len = (p + 1 < b && (t[p + 1] == L'=' || t[p + 1] == d)) ? 2 : 1;
                break;
            default: break;
            }
            if (len > 0) {
                fill(p, p + len, kcOperator);
                p += len;
                continue;
            }
            p++;
        }
    };

    size_t i = 0;
    bool lineStart = true;
    bool prevTypeLike = false;
    bool prevModifier = false;
    bool prevDeclKw = false;
    bool prevDot = false;
    bool prevNew = false;
    bool typeHeader = false;
    bool inBaseList = false;
    bool inNamespace = false;
    bool inAttr = false;
    bool inConstraint = false;
    int attrParen = 0;
    int genericDepth = 0;
    int ternDepth = 0;
    int parenDepth = 0;
    int paramDepth = 0;
    bool pendingParams = false;
    std::wstring_view declName;

    auto reset = [&] {
        prevTypeLike = prevModifier = prevDeclKw = false;
        prevDot = prevNew = false;
    };

    while (i < n) {
        const WCHAR c = t[i];
        if (c == L'\r' || c == L'\n') {
            i++;
            lineStart = true;
            continue;
        }
        if (c == L' ' || c == L'\t') {
            i++;
            continue;
        }
        const bool atStart = lineStart;
        lineStart = false;

        if (c == L'/' && i + 1 < n && t[i + 1] == L'/') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            reset();
            continue;
        }
        if (c == L'/' && i + 1 < n && t[i + 1] == L'*') {
            size_t e = t.find(L"*/", i + 2);
            e = (e == std::wstring::npos) ? n : e + 2;
            fill(i, e, kcComment);
            i = e;
            reset();
            continue;
        }

        if (c == L'#' && atStart) {
            size_t e = i + 1;
            while (e < n && (t[e] == L' ' || t[e] == L'\t')) e++;
            while (e < n && IsIdPart(t[e])) e++;
            fill(i, e, kcPreproc);
            i = e;
            reset();
            continue;
        }

        {
            size_t sp = i;
            bool dollar = false;
            bool at = false;
            size_t q = i;
            while (q < n && (t[q] == L'$' || t[q] == L'@') && q - i < 4) {
                if (t[q] == L'$') dollar = true;
                else at = true;
                q++;
            }
            if (q < n && (t[q] == L'"' || t[q] == L'\'')) sp = q;
            if (t[sp] == L'"' || t[sp] == L'\'') {
                const WCHAR quote = t[sp];
                size_t qlen = 1;
                if (quote == L'"')
                    while (sp + qlen < n && t[sp + qlen] == L'"') qlen++;
                const bool raw = qlen >= 3;
                const bool verbatim = at && !raw;
                size_t e = sp + qlen;
                if (raw) {
                    while (e < n) {
                        if (t[e] != L'"') {
                            e++;
                            continue;
                        }
                        size_t r = e;
                        while (r < n && t[r] == L'"') r++;
                        const bool closes = r - e >= qlen;
                        e = r;
                        if (closes) break;
                    }
                } else {
                    while (e < n) {
                        const WCHAR d = t[e];
                        if (verbatim) {
                            if (d == L'"') {
                                if (e + 1 < n && t[e + 1] == L'"') {
                                    e += 2;
                                    continue;
                                }
                                e++;
                                break;
                            }
                        } else {
                            if (d == L'\\' && e + 1 < n && t[e + 1] != L'\r' &&
                                t[e + 1] != L'\n') {
                                e += 2;
                                continue;
                            }
                            if (d == quote) {
                                e++;
                                break;
                            }
                            if (d == L'\r' || d == L'\n') break;
                        }
                        e++;
                    }
                }
                fill(i, e, quote == L'\'' ? kcCharLit : kcString);
                if (sp > i) fill(i, sp, kcStringQuote);
                fill(sp, sp + qlen, kcStringQuote);
                if (raw) {
                    if (e >= qlen && e - qlen >= sp) fill(e - qlen, e, kcStringQuote);
                } else if (e > sp + 1 && t[e - 1] == quote) {
                    fill(e - 1, e, kcStringQuote);
                }
                if (!raw && !verbatim) {
                    for (size_t p = sp + 1; p + 1 < e;) {
                        if (t[p] == L'\\') {
                            const size_t r = escapeEnd(p, e);
                            fill(p, r, kcEscape);
                            p = r;
                        } else {
                            p++;
                        }
                    }
                }
                if (dollar) {
                    const size_t lim = raw ? (e >= qlen ? e - qlen : e)
                                           : (e > sp ? e - 1 : e);
                    size_t p = sp + qlen;
                    while (p < lim) {
                        if (t[p] != L'{') {
                            p++;
                            continue;
                        }
                        if (p + 1 < lim && t[p + 1] == L'{') {
                            p += 2;
                            continue;
                        }
                        size_t h = p + 1;
                        int depth = 1;
                        while (h < lim) {
                            if (t[h] == L'{') depth++;
                            else if (t[h] == L'}' && --depth == 0) break;
                            h++;
                        }
                        colourHole(p + 1, h < lim ? h : lim);
                        p = h < lim ? h + 1 : lim;
                    }
                }
                i = e;
                reset();
                continue;
            }
        }

        if (IsDigitCh(c) || (c == L'.' && i + 1 < n && IsDigitCh(t[i + 1]))) {
            const size_t e = numberEnd(i, n);
            fill(i, e, kcNumber);
            i = e;
            reset();
            continue;
        }

        if (c == L'@' && i + 1 < n && IsIdStart(t[i + 1])) {
            size_t e = i + 2;
            while (e < n && IsIdPart(t[e])) e++;
            reset();
            i = e;
            continue;
        }

        if (IsIdStart(c)) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            const std::wstring_view w(t.data() + i, e - i);
            const bool afterDot = prevDot;
            const bool wasType = prevTypeLike;
            const bool wasMod = prevModifier;
            const bool wasDecl = prevDeclKw;
            const bool wasNew = prevNew;
            const bool wasNamespaceKw = JwIs(w, L"namespace");
            const bool upper = JavaUpper(w[0]);
            reset();

            uint8_t k = kcDefault;
            bool typeLike = false;
            bool mod = false;
            bool declKw = false;

            if (inNamespace) {
                k = kcClassName;
                typeLike = true;
            } else if (afterDot) {
            } else if (inAttr && !(attrParen > 0 && parenDepth >= attrParen)) {
                if (JwIs(w, L"assembly") || JwIs(w, L"module") || JwIs(w, L"event") ||
                    JwIs(w, L"field") || JwIs(w, L"method") || JwIs(w, L"param") ||
                    JwIs(w, L"property") || JwIs(w, L"return") || JwIs(w, L"type")) {
                    k = kcModifier;
                } else {
                    k = kcAnnotName;
                    typeLike = true;
                }
            } else if (inConstraint && (JwIs(w, L"class") || JwIs(w, L"struct"))) {
                k = kcPrimitive;
                typeLike = true;
            } else if (W.decl.count(w)) {
                k = kcDeclKeyword;
                declKw = JwIs(w, L"class") || JwIs(w, L"struct") || JwIs(w, L"interface") ||
                         JwIs(w, L"enum") || JwIs(w, L"record") || JwIs(w, L"union");
                if (declKw) typeHeader = true;
                if (wasNamespaceKw) inNamespace = true;
            } else if (W.control.count(w)) {
                k = kcControl;
            } else if (JwIs(w, L"operator")) {
                k = kcDeclKeyword;
                pendingParams = true;
            } else if (W.accessor.count(w)) {
                const size_t q2 = skipWs(e);
                if (q2 < n && (t[q2] == L'{' || t[q2] == L';'))
                    k = kcDeclKeyword;
                else if (q2 + 1 < n && t[q2] == L'=' && t[q2 + 1] == L'>')
                    k = kcDeclKeyword;
            } else if (JwIs(w, L"new")) {
                k = kcOperator;
            } else if (JwIs(w, L"as") || JwIs(w, L"is") || JwIs(w, L"and") || JwIs(w, L"or") ||
                       JwIs(w, L"not")) {
                k = kcOperator;
            } else if (JwIs(w, L"this") || JwIs(w, L"base")) {
                k = kcThisSuper;
            } else if (JwIs(w, L"in")) {
                k = genericDepth > 0 ? kcModifier : kcControl;
            } else if (W.modifier.count(w)) {
                k = kcModifier;
                mod = true;
                if (JwIs(w, L"where")) {
                    inBaseList = false;
                    typeHeader = false;
                    inConstraint = true;
                }
            } else if (W.primitive.count(w)) {
                k = kcPrimitive;
                typeLike = true;
            } else if (W.type.count(w)) {
                k = kcType;
                typeLike = true;
            } else if (W.literal.count(w)) {
                k = kcConst;
            } else if (W.other.count(w)) {
                k = kcKeyword;
            }

            if (k == kcDefault) {
                const size_t q2 = skipWs(e);
                const bool nextParen = q2 < n && t[q2] == L'(';
                bool genericThenParen = false;
                if (q2 < n && t[q2] == L'<') {
                    size_t r = q2 + 1;
                    int d = 1;
                    while (r < n && d > 0 && r - q2 < 512) {
                        if (t[r] == L'<') d++;
                        else if (t[r] == L'>') d--;
                        else if (t[r] == L';' || t[r] == L'{' || t[r] == L'}') break;
                        r++;
                    }
                    if (d == 0) {
                        const size_t r2 = skipWs(r);
                        genericThenParen = r2 < n && t[r2] == L'(';
                    }
                }
                const bool memberEnd = q2 < n && (t[q2] == L')' || t[q2] == L',' ||
                                                   t[q2] == L']' || t[q2] == L'=' ||
                                                   t[q2] == L';' || t[q2] == L'{');
                if (wasDecl) {
                    k = kcClassName;
                    declName = w;
                    typeLike = true;
                    if (nextParen) pendingParams = true;
                } else if (wasNew) {
                    k = kcType;
                    typeLike = true;
                } else if (inBaseList && upper && !nextParen) {
                    k = kcInherit;
                } else if (paramDepth > 0 && parenDepth == paramDepth && memberEnd) {
                    k = kcParam;
                } else if (nextParen || genericThenParen) {
                    if (afterDot) {
                        k = kcMethodCall;
                    } else if (wasType || wasMod ||
                               (declName.size() && SameWord(w, declName))) {
                        k = kcMethodDecl;
                        pendingParams = true;
                        typeLike = genericThenParen;
                    } else {
                        k = kcMethodCall;
                        typeLike = genericThenParen;
                    }
                } else if (wasType) {
                    typeLike = true;
                } else if (upper) {
                    k = kcType;
                    typeLike = true;
                }
            }

            if (k != kcDefault) fill(i, e, k);
            if (inNamespace && !wasNamespaceKw) {
                const size_t q2 = skipWs(e);
                inNamespace = q2 < n && t[q2] == L'.';
            }
            i = e;
            prevTypeLike = typeLike;
            prevModifier = mod;
            prevDeclKw = declKw;
            prevNew = JwIs(w, L"new");
            continue;
        }

        if (c == L'[') {
            if ((atStart || prevModifier) && i + 1 < n && JavaUpper(t[i + 1])) {
                inAttr = true;
                attrParen = 0;
                fill(i, i + 1, kcAnnotAt);
                reset();
                i++;
                continue;
            }
        }
        if (c == L']' && inAttr && attrParen == 0) {
            inAttr = false;
            fill(i, i + 1, kcAnnotAt);
            reset();
            i++;
            continue;
        }

        if (c == L'(') {
            parenDepth++;
            if (pendingParams) {
                paramDepth = parenDepth;
                pendingParams = false;
            }
            if (inAttr) attrParen = parenDepth;
            reset();
            i++;
            continue;
        }
        if (c == L')') {
            if (paramDepth == parenDepth) paramDepth = 0;
            if (inAttr && attrParen == parenDepth) attrParen = 0;
            if (parenDepth > 0) parenDepth--;
            reset();
            i++;
            continue;
        }
        if (c == L'{' || c == L'}' || c == L';') {
            paramDepth = 0;
            pendingParams = false;
            genericDepth = 0;
            typeHeader = false;
            inBaseList = false;
            inConstraint = false;
            ternDepth = 0;
            reset();
            i++;
            continue;
        }

        if (c == L'~' && i + 1 < n && IsIdStart(t[i + 1])) {
            size_t e = i + 2;
            while (e < n && IsIdPart(t[e])) e++;
            const size_t q2 = skipWs(e);
            if (q2 < n && t[q2] == L'(') {
                fill(i, e, kcMethodDecl);
                i = e;
                pendingParams = true;
                reset();
                continue;
            }
        }

        if (c == L'?') {
            const WCHAR d1 = i + 1 < n ? t[i + 1] : 0;
            if (d1 == L'?') {
                const size_t len = (i + 2 < n && t[i + 2] == L'=') ? 3 : 2;
                fill(i, i + len, kcOperator);
                reset();
                i += len;
                continue;
            }
            if (d1 == L'.' || d1 == L'[') {
                fill(i, i + 2, kcOperator);
                reset();
                prevDot = d1 == L'.';
                i += 2;
                continue;
            }
            if (prevTypeLike) {
                fill(i, i + 1, kcModifier);
                reset();
                prevTypeLike = true;
                i++;
                continue;
            }
            fill(i, i + 1, kcOperator);
            ternDepth++;
            reset();
            i++;
            continue;
        }

        if (c == L':') {
            if (i + 1 < n && t[i + 1] == L':') {
                reset();
                i += 2;
                continue;
            }
            if (ternDepth > 0) {
                fill(i, i + 1, kcOperator);
                ternDepth--;
                reset();
                i++;
                continue;
            }
            if (typeHeader && !inBaseList) inBaseList = true;
            reset();
            i++;
            continue;
        }

        if (c == L'.' && !(i + 1 < n && t[i + 1] == L'.')) {
            reset();
            prevDot = true;
            i++;
            continue;
        }

        if (c == L'<' && prevTypeLike) {
            const WCHAR d1 = i + 1 < n ? t[i + 1] : 0;
            if (d1 != L'=' && d1 != L'<') {
                genericDepth++;
                reset();
                i++;
                continue;
            }
        }
        if (c == L'>' && genericDepth > 0) {
            size_t k2 = 0;
            while (i + k2 < n && t[i + k2] == L'>' && (int)k2 < genericDepth) k2++;
            genericDepth -= (int)k2;
            reset();
            prevTypeLike = true;
            i += k2;
            continue;
        }

        {
            const WCHAR d1 = i + 1 < n ? t[i + 1] : 0;
            const WCHAR d2 = i + 2 < n ? t[i + 2] : 0;
            size_t len = 0;
            switch (c) {
            case L'=': len = (d1 == L'=' || d1 == L'>') ? 2 : 1; break;
            case L'!': len = d1 == L'=' ? 2 : 1; break;
            case L'<': len = d1 == L'=' ? 2 : 1; break;
            case L'>':
                if (d1 == L'>') len = (d2 == L'>' || d2 == L'=') ? 3 : 2;
                else len = d1 == L'=' ? 2 : 1;
                break;
            case L'+': len = (d1 == L'+' || d1 == L'=') ? 2 : 1; break;
            case L'-':
                if (d1 == L'>') len = 2;
                else len = (d1 == L'-' || d1 == L'=') ? 2 : 1;
                break;
            case L'*': case L'/': case L'%': case L'&': case L'|': case L'^':
                len = (d1 == L'=' || d1 == c) ? 2 : 1;
                break;
            case L'~': len = 1; break;
            case L'.': len = d1 == L'.' ? 2 : 0; break;
            default: break;
            }
            if (len > 0) {
                fill(i, i + len, kcOperator);
                const bool member = c == L'-' && d1 == L'>';
                reset();
                if (member) prevDot = true;
                i += len;
                continue;
            }
        }
        reset();
        i++;
    }
}

// JSON

static void TokenizeJson(const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);

    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > n) b = n;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };

    size_t i = 0;
    while (i < n) {
        const WCHAR c = t[i];
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }
        if (c == L'/' && i + 1 < n && t[i + 1] == L'/') {
            size_t e = i;
            while (e < n && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if (c == L'/' && i + 1 < n && t[i + 1] == L'*') {
            size_t e = t.find(L"*/", i + 2);
            e = (e == std::wstring::npos) ? n : e + 2;
            fill(i, e, kcComment);
            i = e;
            continue;
        }

        if (c == L'"') {
            size_t e = i + 1;
            while (e < n) {
                const WCHAR d = t[e];
                if (d == L'\\' && e + 1 < n) {
                    e += 2;
                    continue;
                }
                if (d == L'"') {
                    e++;
                    break;
                }
                if (d == L'\r' || d == L'\n') break;
                e++;
            }
            size_t k = e;
            while (k < n && (t[k] == L' ' || t[k] == L'\t')) k++;
            if (k < n && t[k] == L':') {
                fill(i, e, kcKey);
            } else {
                fill(i, e, kcString);
                fill(i, i + 1, kcStringQuote);
                if (e > i + 1 && t[e - 1] == L'"') fill(e - 1, e, kcStringQuote);
                for (size_t q = i + 1; q + 1 < e;) {
                    if (t[q] != L'\\') {
                        q++;
                        continue;
                    }
                    size_t r = q + 1;
                    if (r + 4 < e && t[r] == L'u') r += 5;
                    else if (r < e) r++;
                    fill(q, r, kcEscape);
                    q = r;
                }
            }
            i = e;
            continue;
        }

        if (c == L',' || c == L':') {
            fill(i, i + 1, kcSeparator);
            i++;
            continue;
        }
        if (c == L'-') {
            fill(i, i + 1, kcOperator);
            i++;
            continue;
        }
        if (IsDigitCh(c)) {
            size_t e = i;
            while (e < n && IsDigitCh(t[e])) e++;
            fill(i, e, kcNumber);
            if (e + 1 < n && t[e] == L'.' && IsDigitCh(t[e + 1])) {
                fill(e, e + 1, kcSeparator);
                const size_t f = e + 1;
                e = f;
                while (e < n && IsDigitCh(t[e])) e++;
                fill(f, e, kcNumber);
            }
            if (e < n && (t[e] == L'e' || t[e] == L'E')) {
                size_t g = e + 1;
                if (g < n && (t[g] == L'+' || t[g] == L'-')) g++;
                while (g < n && IsDigitCh(t[g])) g++;
                fill(e, g, kcNumber);
                e = g;
            }
            i = e;
            continue;
        }

        if (IsIdStart(c)) {
            size_t e = i + 1;
            while (e < n && IsIdPart(t[e])) e++;
            const std::wstring_view w(t.data() + i, e - i);
            if (JwIs(w, L"true") || JwIs(w, L"false") || JwIs(w, L"null"))
                fill(i, e, kcConst);
            i = e;
            continue;
        }
        i++;
    }
}

// JavaScript / TypeScript

struct JsWords {
    std::unordered_set<std::wstring_view> control, decl, modifier, accessor, literal, language,
        builtinClass, builtinConst, builtinFunc, tsType, tsDecl, tsModifier;
};

static JsWords BuildJsWords() {
    JsWords w;
    BuildSet(w.control, L"break case catch continue debugger default do else finally for if "
                        L"return switch throw try while with import export from await async yield");
    BuildSet(w.decl, L"var let const function class using");
    BuildSet(w.modifier, L"static extends");
    BuildSet(w.accessor, L"get set");
    BuildSet(w.literal, L"true false null undefined NaN Infinity");
    BuildSet(w.language, L"this super arguments globalThis");
    BuildSet(w.builtinClass, L"Array ArrayBuffer BigInt BigInt64Array BigUint64Array Boolean DataView "
                             L"Date Error EvalError FinalizationRegistry Float32Array Float64Array "
                             L"Function Int8Array Int16Array Int32Array Intl JSON Map Math Number "
                             L"Object Promise Proxy RangeError ReferenceError Reflect RegExp Set "
                             L"SharedArrayBuffer String Symbol SyntaxError TypeError URIError "
                             L"Uint8Array Uint8ClampedArray Uint16Array Uint32Array WeakMap WeakRef "
                             L"WeakSet console document navigator window");
    BuildSet(w.builtinConst, L"Atomics");
    BuildSet(w.builtinFunc, L"decodeURI decodeURIComponent encodeURI encodeURIComponent eval "
                            L"isFinite isNaN parseFloat parseInt");
    BuildSet(w.tsType, L"any unknown never void string number boolean object symbol bigint");
    BuildSet(w.tsDecl, L"interface type enum namespace module declare abstract");
    BuildSet(w.tsModifier, L"public private protected readonly override accessor implements");
    return w;
}

static const JsWords& JsWordList() {
    static JsWords w = BuildJsWords();
    return w;
}

static size_t JsMatchParen(const std::wstring& t, size_t open, size_t lim) {
    size_t d = 0;
    for (size_t p = open; p < lim; p++) {
        if (t[p] == L'(') d++;
        else if (t[p] == L')' && --d == 0) return p;
    }
    return lim;
}

static size_t JsCode(const std::wstring& t, size_t start, size_t lim, std::vector<uint8_t>& cls,
                     bool ts, int depth) {
    const JsWords& W = JsWordList();
    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > lim) b = lim;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    auto skipWs = [&](size_t p) {
        while (p < lim && (t[p] == L' ' || t[p] == L'\t' || t[p] == L'\r' || t[p] == L'\n')) p++;
        return p;
    };
    auto escapes = [&](size_t a, size_t b) {
        for (size_t q = a; q < b; q++) {
            if (t[q] != L'\\' || q + 1 >= b) continue;
            size_t e = q + 2;
            if (t[q + 1] == L'u' && q + 2 < b && t[q + 2] == L'{') {
                e = q + 3;
                while (e < b && t[e] != L'}') e++;
                if (e < b) e++;
            } else if (t[q + 1] == L'x' || t[q + 1] == L'u') {
                const int max = t[q + 1] == L'x' ? 2 : 4;
                e = q + 2;
                for (int d = 0; d < max && e < b && iswxdigit((wint_t)t[e]); d++) e++;
            }
            fill(q, e, kcEscape);
            q = e - 1;
        }
    };
    auto opLen = [&](size_t p) -> size_t {
        const WCHAR a = t[p];
        const WCHAR b = p + 1 < lim ? t[p + 1] : 0;
        const WCHAR d = p + 2 < lim ? t[p + 2] : 0;
        const WCHAR f = p + 3 < lim ? t[p + 3] : 0;
        if (a == L'=' && b == L'=' && d == L'=') return 3;
        if (a == L'!' && b == L'=' && d == L'=') return 3;
        if (a == L'>' && b == L'>' && d == L'>') return f == L'=' ? 4 : 3;
        if ((a == L'<' || a == L'>') && b == a && d == L'=') return 3;
        if ((a == L'&' || a == L'|' || a == L'?') && b == a && d == L'=') return 3;
        if (a == L'*' && b == L'*' && d == L'=') return 3;
        if ((a == L'+' || a == L'-') && b == a) return 2;
        if (a == L'=' && b == L'>') return 2;
        if (a == L'?' && b == L'.') return 2;
        if (a == L'?' && b == L'?') return 2;
        if (a == L'*' && b == L'*') return 2;
        if ((a == L'&' || a == L'|') && b == a) return 2;
        if ((a == L'!' || a == L'<' || a == L'>' || a == L'%' || a == L'^' || a == L'/' || a == L'*' ||
             a == L'&' || a == L'|' || a == L'+' || a == L'-') && b == L'=') return 2;
        if (a == L'=' || a == L'!' || a == L'<' || a == L'>' || a == L'~' || a == L'?' || a == L'+' ||
            a == L'-' || a == L'*' || a == L'/' || a == L'%' || a == L'&' || a == L'|' || a == L'^')
            return 1;
        return 0;
    };

    size_t i = start;
    size_t braces = 0;
    int pending = 0;
    int paramDepth = 0;
    bool canEnd = false;
    bool inParams = false;
    bool inAnnotation = false;
    bool awaitingParen = false;

    while (i < lim) {
        const WCHAR c = t[i];
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }
        if (c == L'/' && i + 1 < lim && t[i + 1] == L'/') {
            size_t e = i + 2;
            while (e < lim && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if (c == L'/' && i + 1 < lim && t[i + 1] == L'*') {
            size_t e = i + 2;
            while (e + 1 < lim && !(t[e] == L'*' && t[e + 1] == L'/')) e++;
            e = e + 1 < lim ? e + 2 : lim;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if (c == L'/' && !canEnd) {
            size_t e = i + 1;
            bool inClass = false;
            bool closed = false;
            while (e < lim) {
                const WCHAR d = t[e];
                if (d == L'\\') {
                    e += 2;
                    continue;
                }
                if (d == L'\r' || d == L'\n') break;
                if (d == L'[') inClass = true;
                else if (d == L']') inClass = false;
                else if (d == L'/' && !inClass) {
                    closed = true;
                    break;
                }
                e++;
            }
            if (closed) {
                fill(i, i + 1, kcStringQuote);
                fill(i + 1, e, kcString);
                fill(e, e + 1, kcStringQuote);
                size_t f = e + 1;
                while (f < lim && ((t[f] >= L'a' && t[f] <= L'z') || (t[f] >= L'A' && t[f] <= L'Z'))) f++;
                if (f > e + 1) fill(e + 1, f, kcKeyword);
                i = f;
                canEnd = true;
                continue;
            }
        }
        if (c == L'"' || c == L'\'') {
            fill(i, i + 1, kcStringQuote);
            size_t e = i + 1;
            while (e < lim) {
                if (t[e] == L'\\') {
                    e += 2;
                    continue;
                }
                if (t[e] == c || t[e] == L'\r' || t[e] == L'\n') break;
                e++;
            }
            fill(i + 1, e, kcString);
            escapes(i + 1, e);
            if (e < lim && t[e] == c) {
                fill(e, e + 1, kcStringQuote);
                const size_t nx = skipWs(e + 1);
                size_t b = i;
                while (b > start && (t[b - 1] == L' ' || t[b - 1] == L'\t' || t[b - 1] == L'\r' ||
                                     t[b - 1] == L'\n'))
                    b--;
                if (nx < lim && t[nx] == L':' &&
                    (b <= start || t[b - 1] == L'{' || t[b - 1] == L','))
                    fill(i, e + 1, kcKey);
                i = e + 1;
            } else {
                i = e;
            }
            canEnd = true;
            continue;
        }
        if (c == L'`') {
            fill(i, i + 1, kcStringQuote);
            size_t seg = i + 1;
            size_t p = i + 1;
            while (p < lim) {
                const WCHAR d = t[p];
                if (d == L'\\') {
                    p += 2;
                    continue;
                }
                if (d == L'`') {
                    fill(seg, p, kcString);
                    escapes(seg, p);
                    fill(p, p + 1, kcStringQuote);
                    p++;
                    break;
                }
                if (d == L'$' && p + 1 < lim && t[p + 1] == L'{') {
                    fill(seg, p, kcString);
                    escapes(seg, p);
                    const size_t close = depth < 16 ? JsCode(t, p + 2, lim, cls, ts, depth + 1) : lim;
                    if (close >= lim) {
                        p = lim;
                        break;
                    }
                    p = close + 1;
                    seg = p;
                    continue;
                }
                p++;
            }
            i = p;
            canEnd = true;
            continue;
        }
        if ((c >= L'0' && c <= L'9') ||
            (c == L'.' && i + 1 < lim && t[i + 1] >= L'0' && t[i + 1] <= L'9')) {
            size_t e = i + 1;
            while (e < lim) {
                const WCHAR d = t[e];
                if ((d >= L'0' && d <= L'9') || (d >= L'a' && d <= L'z') || (d >= L'A' && d <= L'Z') ||
                    d == L'_') {
                    e++;
                    continue;
                }
                if (d == L'.' && !(e + 1 < lim && t[e + 1] == L'.')) {
                    e++;
                    continue;
                }
                if ((d == L'+' || d == L'-') && (t[e - 1] == L'e' || t[e - 1] == L'E')) {
                    e++;
                    continue;
                }
                break;
            }
            for (size_t q = i; q < e; q++) cls[q] = t[q] == L'.' ? kcSeparator : kcNumber;
            i = e;
            canEnd = true;
            continue;
        }
        if (c == L'@' && i + 1 < lim && IsIdStart(t[i + 1])) {
            size_t e = i + 1;
            while (e < lim && IsIdPart(t[e])) e++;
            fill(i, i + 1, kcAnnotAt);
            fill(i + 1, e, kcAnnotName);
            i = e;
            canEnd = true;
            continue;
        }
        if (IsIdStart(c)) {
            size_t e = i;
            while (e < lim && IsIdPart(t[e])) e++;
            const std::wstring_view w(t.data() + i, e - i);
            const size_t nx = skipWs(e);
            uint8_t k = kcDefault;
            if (W.literal.count(w)) {
                k = kcConst;
            } else if (W.language.count(w)) {
                k = kcThisSuper;
            } else if (ts && W.tsType.count(w)) {
                k = kcType;
            } else if (JwIs(w, L"new") || JwIs(w, L"typeof") || JwIs(w, L"instanceof") ||
                       JwIs(w, L"delete") || JwIs(w, L"in") || JwIs(w, L"of") || JwIs(w, L"void")) {
                k = kcOperator;
                if (JwIs(w, L"new")) pending = 4;
            } else if (ts && (JwIs(w, L"as") || JwIs(w, L"satisfies") || JwIs(w, L"keyof") ||
                              JwIs(w, L"infer") || JwIs(w, L"is") || JwIs(w, L"asserts") ||
                              JwIs(w, L"unique"))) {
                k = kcOperator;
            } else if (ts && W.tsDecl.count(w)) {
                k = kcDeclKeyword;
                pending = 3;
            } else if (JwIs(w, L"extends") || JwIs(w, L"implements")) {
                k = kcModifier;
                pending = 5;
            } else if (ts && W.tsModifier.count(w)) {
                k = kcModifier;
            } else if (JwIs(w, L"function")) {
                k = kcDeclKeyword;
                pending = 1;
            } else if (JwIs(w, L"class")) {
                k = kcDeclKeyword;
                pending = 2;
            } else if (W.control.count(w)) {
                k = kcKeyword;
            } else if (W.decl.count(w)) {
                k = kcDeclKeyword;
            } else if (W.modifier.count(w)) {
                k = kcModifier;
            } else if (W.accessor.count(w)) {
                k = kcPrimitive;
            } else if (W.builtinClass.count(w)) {
                k = kcType;
            } else if (W.builtinConst.count(w)) {
                k = kcKeyword;
            } else if (W.builtinFunc.count(w)) {
                k = nx < lim && t[nx] == L'(' ? kcMethodCall : kcDefault;
            }
            if (k == kcDefault) {
                if (pending == 1) {
                    k = kcMethodDecl;
                    pending = 0;
                    if (nx < lim && t[nx] == L'(') inParams = true;
                    else if (nx < lim && t[nx] == L'<') awaitingParen = true;
                } else if (pending == 2 || pending == 3 || pending == 4) {
                    k = kcClassName;
                    pending = 0;
                } else if (pending == 5) {
                    k = kcInherit;
                    pending = 0;
                } else if (inParams && paramDepth == 1 && !inAnnotation) {
                    k = kcParam;
                } else if (nx < lim && (t[nx] == L'(' || t[nx] == L'<')) {
                    size_t open = nx;
                    if (t[nx] == L'<') {
                        size_t d = 0;
                        const size_t cap = nx + 512 < lim ? nx + 512 : lim;
                        while (open < cap) {
                            if (t[open] == L'<') d++;
                            else if (t[open] == L'>' && d > 0 && --d == 0) {
                                open++;
                                break;
                            }
                            open++;
                        }
                        open = skipWs(open);
                    }
                    if (open < lim && t[open] == L'(') {
                        const size_t close = JsMatchParen(t, open, lim);
                        const size_t after = close < lim ? skipWs(close + 1) : lim;
                        if (after < lim && (t[after] == L'{' || (ts && t[after] == L':'))) {
                            k = kcMethodDecl;
                            inParams = true;
                        } else {
                            k = kcMethodCall;
                        }
                    }
                } else if (nx < lim && t[nx] == L':') {
                    size_t b = i;
                    while (b > start && (t[b - 1] == L' ' || t[b - 1] == L'\t' || t[b - 1] == L'\r' ||
                                         t[b - 1] == L'\n'))
                        b--;
                    if (b <= start || t[b - 1] == L'{' || t[b - 1] == L',') k = kcKey;
                }
            }
            if (k != kcDefault) fill(i, e, k);
            if (k == kcParam && nx < lim && t[nx] == L':') inAnnotation = true;
            canEnd = !(k == kcKeyword || k == kcDeclKeyword || k == kcModifier || k == kcPrimitive ||
                       k == kcOperator);
            i = e;
            continue;
        }
        if (c == L'(') {
            if (awaitingParen) {
                awaitingParen = false;
                inParams = true;
                paramDepth = 1;
            } else if (inParams) {
                paramDepth++;
            }
            pending = 0;
            canEnd = false;
            i++;
            continue;
        }
        if (c == L')') {
            if (inParams) {
                if (paramDepth > 0) paramDepth--;
                if (paramDepth == 0) {
                    inParams = false;
                    inAnnotation = false;
                }
            }
            canEnd = true;
            i++;
            continue;
        }
        if (c == L'{') {
            braces++;
            if (paramDepth == 0) inParams = false;
            inAnnotation = false;
            pending = 0;
            canEnd = false;
            i++;
            continue;
        }
        if (c == L'}') {
            if (braces == 0) return i;
            braces--;
            pending = 0;
            canEnd = true;
            i++;
            continue;
        }
        if (c == L'[' || c == L']') {
            canEnd = c == L']';
            i++;
            continue;
        }
        if (c == L',' || c == L';') {
            fill(i, i + 1, kcSeparator);
            pending = 0;
            inAnnotation = false;
            canEnd = false;
            i++;
            continue;
        }
        if (c == L'.') {
            if (i + 2 < lim && t[i + 1] == L'.' && t[i + 2] == L'.') {
                fill(i, i + 3, kcOperator);
                i += 3;
                canEnd = false;
                continue;
            }
            canEnd = false;
            i++;
            continue;
        }
        const size_t len = opLen(i);
        if (len) {
            fill(i, i + len, kcOperator);
            i += len;
            canEnd = false;
            continue;
        }
        i++;
    }
    return lim;
}

static void TokenizeJs(const std::wstring& t, std::vector<uint8_t>& cls, bool ts) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    size_t i = 0;
    while (i < n) {
        const size_t r = JsCode(t, i, n, cls, ts, 0);
        if (r >= n) break;
        i = r + 1;
    }
}

// Python

struct PyWords {
    std::unordered_set<std::wstring_view> keyword, operatorWord, builtinFunc, builtinType,
        builtinError, constant, language;
};

static PyWords BuildPyWords() {
    PyWords w;
    BuildSet(w.keyword, L"as assert async await break case class continue def del elif else except "
                        L"finally for from global if import lambda match nonlocal pass raise return "
                        L"try while with yield");
    BuildSet(w.operatorWord, L"and in is not or");
    BuildSet(w.constant, L"True False None NotImplemented Ellipsis __debug__");
    BuildSet(w.language, L"self cls");
    BuildSet(w.builtinType, L"bool bytearray bytes complex dict float frozenset frozendict int list "
                            L"memoryview object sentinel set slice str tuple basestring long unicode");
    BuildSet(w.builtinError,
             L"ArithmeticError AssertionError AttributeError BaseException BlockingIOError "
             L"BrokenPipeError BufferError BytesWarning ChildProcessError ConnectionAbortedError "
             L"ConnectionRefusedError ConnectionResetError DeprecationWarning EnvironmentError "
             L"EOFError Exception FileExistsError FileNotFoundError FloatingPointError "
             L"FutureWarning GeneratorExit ImportError ImportWarning IndentationError IndexError "
             L"InterruptedError IOError IsADirectoryError KeyboardInterrupt KeyError LookupError "
             L"MemoryError ModuleNotFoundError NameError NotADirectoryError NotImplementedError "
             L"OSError OverflowError PendingDeprecationWarning PermissionError ProcessLookupError "
             L"RecursionError ReferenceError ResourceWarning RuntimeError RuntimeWarning StopIteration "
             L"StopAsyncIteration SyntaxError SyntaxWarning SystemError SystemExit TabError "
             L"TimeoutError TypeError UnboundLocalError UnicodeDecodeError UnicodeEncodeError "
             L"UnicodeTranslateError UnicodeWarning UserWarning ValueError Warning "
             L"ZeroDivisionError");
    BuildSet(w.builtinFunc, L"all abs any ascii bin callable chr classmethod compile delattr dir "
                            L"divmod enumerate eval filter format getattr globals hasattr hash help "
                            L"hex id input isinstance issubclass iter len locals map max min next oct "
                            L"open ord pow property range repr reversed round setattr sorted "
                            L"staticmethod sum super type vars zip aiter anext breakpoint compiile "
                            L"exec print apply cmp coerce execfile intern raw_input reduce reload "
                            L"unichr xrange __import__");
    return w;
}

static const PyWords& PyWordList() {
    static PyWords w = BuildPyWords();
    return w;
}

static size_t PyCode(const std::wstring& t, size_t start, size_t lim, std::vector<uint8_t>& cls,
                     int depth, bool inF) {
    const PyWords& W = PyWordList();
    auto fill = [&](size_t a, size_t b, uint8_t k) {
        if (b > lim) b = lim;
        for (size_t q = a; q < b; q++) cls[q] = k;
    };
    auto skipWs = [&](size_t p) {
        while (p < lim && (t[p] == L' ' || t[p] == L'\t' || t[p] == L'\r' || t[p] == L'\n')) p++;
        return p;
    };
    auto escapes = [&](size_t a, size_t b) {
        for (size_t q = a; q < b; q++) {
            if (t[q] != L'\\' || q + 1 >= b) continue;
            size_t e = q + 2;
            if ((t[q + 1] == L'x' || t[q + 1] == L'u' || t[q + 1] == L'U') && q + 2 < b) {
                const int max = t[q + 1] == L'x' ? 2 : (t[q + 1] == L'u' ? 4 : 8);
                e = q + 2;
                for (int d = 0; d < max && e < b && iswxdigit((wint_t)t[e]); d++) e++;
            } else if (t[q + 1] >= L'0' && t[q + 1] <= L'7') {
                e = q + 2;
                for (int d = 0; d < 2 && e < b && t[e] >= L'0' && t[e] <= L'7'; d++) e++;
            }
            fill(q, e, kcEscape);
            q = e - 1;
        }
    };

    size_t i = start;
    size_t braces = 0;
    int pending = 0;
    int paramDepth = 0;
    bool inParams = false;
    bool inAnnotation = false;
    bool pendingF = false;
    bool pendingRaw = false;

    while (i < lim) {
        const WCHAR c = t[i];
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }
        if (c == L'#') {
            size_t e = i + 1;
            while (e < lim && t[e] != L'\r' && t[e] != L'\n') e++;
            fill(i, e, kcComment);
            i = e;
            continue;
        }
        if (c == L'"' || c == L'\'') {
            const bool triple = i + 2 < lim && t[i + 1] == c && t[i + 2] == c;
            size_t b = i;
            while (b > start && (t[b - 1] == L' ' || t[b - 1] == L'\t' || t[b - 1] == L'\r')) b--;
            const bool doc = triple && (b <= start || t[b - 1] == L'\r' || t[b - 1] == L'\n');
            const bool fstr = pendingF;
            const bool raw = pendingRaw;
            pendingF = false;
            pendingRaw = false;
            if (doc) {
                size_t p = i + 3;
                while (p < lim && !(t[p] == c && p + 2 < lim && t[p + 1] == c && t[p + 2] == c)) p++;
                fill(i, p < lim ? p + 3 : lim, kcComment);
                i = p < lim ? p + 3 : lim;
                continue;
            }
            fill(i, i + (triple ? 3 : 1), kcStringQuote);
            size_t p = i + (triple ? 3 : 1);
            size_t seg = p;
            bool closed = false;
            while (p < lim) {
                if (!raw && t[p] == L'\\') {
                    p += 2;
                    continue;
                }
                if (t[p] == c) {
                    if (triple) {
                        if (p + 2 < lim && t[p + 1] == c && t[p + 2] == c) {
                            fill(seg, p, kcString);
                            if (!raw) escapes(seg, p);
                            fill(p, p + 3, kcStringQuote);
                            p += 3;
                            closed = true;
                            break;
                        }
                        p++;
                        continue;
                    }
                    fill(seg, p, kcString);
                    if (!raw) escapes(seg, p);
                    fill(p, p + 1, kcStringQuote);
                    p++;
                    closed = true;
                    break;
                }
                if (fstr && t[p] == L'{') {
                    if (p + 1 < lim && t[p + 1] == L'{') {
                        p += 2;
                        continue;
                    }
                    fill(seg, p, kcString);
                    if (!raw) escapes(seg, p);
                    const size_t close =
                        depth < 16 ? PyCode(t, p + 1, lim, cls, depth + 1, true) : lim;
                    if (close >= lim) {
                        p = lim;
                        break;
                    }
                    p = close + 1;
                    seg = p;
                    continue;
                }
                if (fstr && t[p] == L'}' && p + 1 < lim && t[p + 1] == L'}') {
                    p += 2;
                    continue;
                }
                p++;
            }
            if (!closed) {
                fill(seg, p, kcString);
                if (!raw) escapes(seg, p);
            }
            i = p;
            continue;
        }
        if ((c >= L'0' && c <= L'9') ||
            (c == L'.' && i + 1 < lim && t[i + 1] >= L'0' && t[i + 1] <= L'9')) {
            size_t e = i + 1;
            while (e < lim) {
                const WCHAR d = t[e];
                if (iswalnum((wint_t)d) || d == L'_') {
                    e++;
                    continue;
                }
                if (d == L'.' && !(e + 1 < lim && t[e + 1] == L'.')) {
                    e++;
                    continue;
                }
                if ((d == L'+' || d == L'-') && (t[e - 1] == L'e' || t[e - 1] == L'E')) {
                    e++;
                    continue;
                }
                break;
            }
            for (size_t q = i; q < e; q++) cls[q] = t[q] == L'.' ? kcSeparator : kcNumber;
            i = e;
            continue;
        }
        if (c == L'@') {
            size_t b = i;
            while (b > start && (t[b - 1] == L' ' || t[b - 1] == L'\t' || t[b - 1] == L'\r')) b--;
            if (b <= start || t[b - 1] == L'\r' || t[b - 1] == L'\n') {
                fill(i, i + 1, kcAnnotAt);
                size_t p = i + 1;
                while (p < lim) {
                    if (t[p] == L'_' || iswalnum((wint_t)t[p])) {
                        size_t e = p;
                        while (e < lim && (t[e] == L'_' || iswalnum((wint_t)t[e]))) e++;
                        fill(p, e, kcAnnotName);
                        p = e;
                        continue;
                    }
                    if (t[p] == L'.') {
                        p++;
                        continue;
                    }
                    break;
                }
                i = p;
                continue;
            }
        }
        if (c == L'_' || iswalpha((wint_t)c)) {
            size_t e = i;
            while (e < lim && (t[e] == L'_' || iswalnum((wint_t)t[e]))) e++;
            const std::wstring_view w(t.data() + i, e - i);
            const size_t nx = skipWs(e);
            if (w.size() <= 2 && e < lim && (t[e] == L'"' || t[e] == L'\'')) {
                bool ok = true, f = false, r = false;
                for (size_t k = 0; k < w.size(); k++) {
                    const WCHAR d = (WCHAR)towlower((wint_t)w[k]);
                    if (d == L'f') f = true;
                    else if (d == L'r') r = true;
                    else if (d != L'b' && d != L'u') ok = false;
                }
                if (ok && (w.size() == 1 || w[0] != w[1])) {
                    fill(i, e, kcPrimitive);
                    pendingF = f;
                    pendingRaw = r;
                    i = e;
                    continue;
                }
            }
            uint8_t k = kcDefault;
            if (W.constant.count(w)) {
                k = kcConst;
            } else if (W.language.count(w)) {
                k = kcThisSuper;
            } else if (W.operatorWord.count(w)) {
                k = kcOperator;
            } else if (W.keyword.count(w)) {
                k = kcKeyword;
            } else if (W.builtinType.count(w) || W.builtinError.count(w)) {
                k = kcType;
            } else if (W.builtinFunc.count(w) && nx < lim && t[nx] == L'(') {
                k = kcMethodCall;
            }
            if (k == kcDefault) {
                if (pending == 1) {
                    k = kcMethodDecl;
                    pending = 0;
                    if (nx < lim && t[nx] == L'(') inParams = true;
                } else if (pending == 2) {
                    k = kcClassName;
                    pending = 0;
                } else if (inParams && paramDepth == 1 && !inAnnotation) {
                    k = kcParam;
                } else if (nx < lim && t[nx] == L'(') {
                    k = kcMethodCall;
                }
            }
            if (k == kcKeyword) {
                if (JwIs(w, L"def")) pending = 1;
                else if (JwIs(w, L"class")) pending = 2;
            }
            if (k != kcDefault) fill(i, e, k);
            if (k == kcParam && nx < lim && t[nx] == L':') inAnnotation = true;
            i = e;
            continue;
        }
        if (c == L'(') {
            if (inParams) paramDepth++;
            pending = 0;
            i++;
            continue;
        }
        if (c == L')') {
            if (inParams) {
                if (paramDepth > 0) paramDepth--;
                if (paramDepth == 0) {
                    inParams = false;
                    inAnnotation = false;
                }
            }
            i++;
            continue;
        }
        if (c == L'{') {
            braces++;
            pending = 0;
            i++;
            continue;
        }
        if (c == L'}') {
            if (braces == 0) return i;
            braces--;
            i++;
            continue;
        }
        if (inF && c == L':' && braces == 0) {
            size_t p = i + 1;
            size_t d = 0;
            while (p < lim) {
                if (t[p] == L'{') d++;
                else if (t[p] == L'}') {
                    if (d == 0) break;
                    d--;
                }
                p++;
            }
            fill(i, p, kcString);
            return p;
        }
        if (c == L'[' || c == L']') {
            i++;
            continue;
        }
        if (c == L',' || c == L';') {
            fill(i, i + 1, kcSeparator);
            pending = 0;
            inAnnotation = false;
            i++;
            continue;
        }
        if (c == L'.') {
            if (i + 2 < lim && t[i + 1] == L'.' && t[i + 2] == L'.') {
                fill(i, i + 3, kcConst);
                i += 3;
                continue;
            }
            i++;
            continue;
        }
        {
            const WCHAR d1 = i + 1 < lim ? t[i + 1] : 0;
            const WCHAR d2 = i + 2 < lim ? t[i + 2] : 0;
            size_t len = 0;
            if ((c == L'*' && d1 == L'*') || (c == L'/' && d1 == L'/') || (c == L'<' && d1 == L'<') ||
                (c == L'>' && d1 == L'>') || (c == L'<' && d1 == L'=') || (c == L'>' && d1 == L'=') ||
                (c == L'=' && d1 == L'=') || (c == L'!' && d1 == L'=') || (c == L':' && d1 == L'=') ||
                (c == L'-' && d1 == L'>'))
                len = 2;
            if (len == 2 && d2 == L'=' && d1 == c && c != L'=') len = 3;
            if (!len && (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'%' ||
                         c == L'&' || c == L'|' || c == L'^' || c == L'~' || c == L'<' || c == L'>' ||
                         c == L'=' || c == L'@'))
                len = 1;
            if (len) {
                fill(i, i + len, kcOperator);
                i += len;
                continue;
            }
        }
        i++;
    }
    return lim;
}

static void TokenizePython(const std::wstring& t, std::vector<uint8_t>& cls) {
    const size_t n = t.size();
    cls.assign(n, kcDefault);
    size_t i = 0;
    while (i < n) {
        const size_t r = PyCode(t, i, n, cls, 0, false);
        if (r >= n) break;
        i = r + 1;
    }
}

// Classifies every character (multi-line comments/strings too) so the draw hook only looks classes up.
static void Tokenize(const LangCfg& L, const std::wstring& t, std::vector<uint8_t>& cls) {
    if (L.flags & fCpp) {  // [keep] C/C++ has its own scope-rich pass
        TokenizeCpp(t, cls);
        return;
    }
    if (L.flags & fCSharp) {  // [keep] C# has its own scope-rich pass
        TokenizeCsharp(t, cls);
        return;
    }
    if (L.flags & fJson) {  // [keep] JSON tints its keys and separators
        TokenizeJson(t, cls);
        return;
    }
    if (L.flags & (fJs | fTs)) {
        TokenizeJs(t, cls, (L.flags & fTs) != 0);
        return;
    }
    if (L.flags & fPython) {
        TokenizePython(t, cls);
        return;
    }
    if (L.flags & fJava) {  // [keep] Java has its own scope-rich pass
        TokenizeJava(t, cls);
        return;
    }
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

        // Preprocessor directive
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

// The editor counts a line break as ONE character (CRLF = one paragraph mark); syntax code uses that numbering.
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

// Length implied by row geometry: start of the last row + its length.
static LONG ExpectedEditorLength(EditorCtx* c) {
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    if (lines <= 0) return 0;
    LONG idx = (LONG)SendMessageW(c->edit, EM_LINEINDEX, lines - 1, 0);
    if (idx < 0) return 0;
    return idx + (LONG)SendMessageW(c->edit, EM_LINELENGTH, idx, 0);
}

// Reads the whole document in the editor's index space.
static bool ReadDocText(EditorCtx* c, LONG len, std::wstring& out, bool lenient = false) {
    // [keep] was the global g_lastExpected
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
    if (std::abs((long)out.size() - (long)lastExpected) <= 1 || (lenient && !out.empty())) {
        return true;
    }

    // Path 2: assemble from rows at their EM_LINEINDEX positions.
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

// Language from the file name in the title; none for "Untitled" tabs.
static std::wstring DocName(HWND edit);

static int DetectLang(HWND edit) {
    // [keep] reuse DocName() for the title parsing
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

// Change markers: what changed since the document was opened

// Hash of every line (lines are separated by the editor paragraph mark).
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
    h.push_back(x);
}

struct LineDiff {
    std::vector<uint8_t> aDel;
    std::vector<uint8_t> bIns;
    // [keep] skip exact-diff retries after a failure (see DiffRange)
    bool skipBigExact = false;
    bool bigExactFailed = false;
};

static void MarkAll(LineDiff& out, int a0, int a1, int b0, int b1) {
    for (int i = a0; i < a1; i++) out.aDel[(size_t)i] = 1;
    for (int j = b0; j < b1; j++) out.bIns[(size_t)j] = 1;
}

// Myers' O(ND) diff of A against B; returns false beyond maxD edits.
static bool MyersRange(const uint64_t* A, int a0, int a1, const uint64_t* B, int b0, int b1,
                       int maxD, LineDiff& out) {
    const int n = a1 - a0, m = b1 - b0;
    const int D = std::min(maxD, n + m);
    const int off = D + 1;
    std::vector<int> V((size_t)(2 * D + 3), 0);
    std::vector<std::vector<int>> trace;
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
                out.bIns[(size_t)(b0 + prevy)] = 1;
            else
                out.aDel[(size_t)(a0 + prevx)] = 1;
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
    if (a0 == a1 || b0 == b1) {
        MarkAll(out, a0, a1, b0, b1);
        return;
    }
    const int n = a1 - a0, m = b1 - b0;
    // Work budget ~4e7 steps: exact diffs for small documents, bounded effort for huge ones.
    const int cap = (int)std::clamp<int64_t>(40000000LL / (n + m), 64, 3000);
    // [keep] skip the expensive exact diff after it failed (MarkerTick retries every 32nd run)
    const bool expensive = std::min(cap, n + m) > 1000;
    if (!(expensive && out.skipBigExact)) {
        if (MyersRange(A, a0, a1, B, b0, b1, cap, out)) return;
        if (expensive) out.bigExactFailed = true;
    }

    // Too many differences: anchor on lines unique to both sides, keep the longest in-order chain, diff the gaps.
    if (depth < 24) {
        std::unordered_map<uint64_t, int> ua, ub;
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

// Turns the edit script into line states: "modified", "added", or a "removed" notch.
static void BuildMarkers(const LineDiff& d, int nOld, int nNew, std::vector<uint8_t>& state,
                         std::vector<uint32_t>& del) {
    state.assign((size_t)nNew, 0);
    del.clear();
    int i = 0, j = 0;
    while (i < nOld || j < nNew) {
        bool delA = i < nOld && d.aDel[(size_t)i];
        bool insB = j < nNew && d.bIns[(size_t)j];
        if (!delA && !insB) {
            if (i >= nOld || j >= nNew) break;
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

// Untitled tabs get no markers (typing would turn every line green).
static bool IsSavedName(const std::wstring& n) {
    if (n.empty() || _wcsnicmp(n.c_str(), L"untitled", 8) == 0) return false;
    // A saved document is a file name with an extension.
    size_t dot = n.rfind(L'.');
    return dot != std::wstring::npos && dot + 1 < n.size();
}

// Document name from the window title ("*name - Notepad" -> "name").
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

// Forgets the baseline and markers; a new baseline is captured on the next refresh.
static void MarkerResetState(EditorCtx* c) {
    c->mkHave = c->mkFrozen = c->mkUserEdit = c->mkReload = c->mkEmptyBase = false;
    c->mkBase.clear();
    c->mkBaseHash.clear();
    c->mkState.clear();
    c->mkDel.clear();
    c->mkRev = ~0u;
    c->mkDirty = true;
    c->mkExactFailed = false;  // [keep] fresh exact-diff try for a new baseline
}

// Title re-read: a new name must be seen twice in a row, since the title can lag behind a tab switch.
static void MarkerNameTick(EditorCtx* c) {
    std::wstring nm = DocName(c->edit);
    if (c->mkNameKnown && nm == c->mkName) {
        c->mkPendingName.clear();
        return;
    }
    if (nm != c->mkPendingName) {
        c->mkPendingName = nm;
        return;
    }
    c->mkName = nm;
    c->mkNameKnown = true;
    c->mkPendingName.clear();
    c->mkNameOk = IsSavedName(nm);
    MarkerResetState(c);
}

// Called from SyntaxTick after the document copy refreshes.
static void MarkerTick(EditorCtx* c, ULONGLONG now) {
    if (c->mkReload) {
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
            c->mkFrozen = true;
        } else {
            // Still loading: the baseline becomes whatever the document ends up as.
            c->mkBase = c->docText;
            HashLines(c->mkBase, c->mkBaseHash);
        }
    }
    if (c->mkFrozen && c->mkBase.empty()) {
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
    // [keep] remember a failed expensive exact diff instead of redoing it on every edit
    d.skipBigExact = c->mkExactFailed && (++c->mkExactSkips % 32) != 0;
    DiffRange(base.data(), 0, (int)base.size(), cur.data(), 0, (int)cur.size(), d, 0);
    if (d.bigExactFailed)
        c->mkExactFailed = true;
    else if (!d.skipBigExact)
        c->mkExactFailed = false;
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
            InvalidateRect(c->edit, nullptr, FALSE);
        }
    }
    // The document copy is needed for syntax colors and the minimap.
    if ((size_t)len > kSyntaxMaxChars || (c->lang < 0 && !c->mmActive && !c->mkOn && !c->pvActive)) {
        if (!c->docText.empty() || c->docOk) {
            c->docText.clear();
            c->cls.clear();
            c->snap.clear();
            c->docOk = false;
            c->docRev++;
        }
        c->docStale = true;
        c->docSynLen = -2;
        return;
    }
    // [keep] read plain files <= 200000 chars during the message too, so the line table is current
    if (c->docStale && !(c->lang < 0 && c->inMsg && len > 200000) &&
        (len <= 200000 || now - c->synChangeTick > 150)) {
        c->docStale = false;
        // After 20 failed checks the editor's own text is accepted: its line table can stay behind.
        std::wstring fresh;
        if (ReadDocText(c, len, fresh, c->docLenient || c->docRetry >= 20)) {
            if (c->docRetry >= 20) c->docLenient = true;
            // Same text again (holding Ctrl marks the copy stale): keep the revision so nothing rebuilds or repaints.
            if (c->docOk && fresh == c->docText) {
                c->docSynLen = len;
                c->docRetry = 0;
                return;
            }
            c->docText.swap(fresh);
            if (c->lang >= 0)
                Tokenize(Langs()[(size_t)c->lang], c->docText, c->cls);
            else
                c->cls.clear();
            c->docOk = true;
            c->docRev++;
            c->docSynLen = len;
            c->docRetry = 0;
        } else {
            if (!c->docOk) {
                c->docText.clear();
                c->cls.clear();
                c->docRev++;
            }
            // A failed read is usually the line table lagging behind an edit; retry on the next tick.
            if (c->docRetry < 60) {
                c->docRetry++;
                c->docStale = true;
            }
        }
        // Lines below an edit may have changed color.
        if (c->lang >= 0) InvalidateRect(c->edit, nullptr, FALSE);
    }
}

// Measures rows around the viewport; re-derives the first and empty trailing rows, where EM_POSFROMCHAR is wrong.
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

// Indent guides (1/2): measure the rows around the viewport.

constexpr LONG kGuideCtxRows = 48;
constexpr int kGuideMaxLevels = 64;
constexpr int kGuideReadChars = 256;

static bool GuideDark(EditorCtx* c) {
    HWND top = GetAncestor(c->edit, GA_ROOT);
    BOOL dwmDark = FALSE;
    if (top && SUCCEEDED(DwmGetWindowAttribute(top, DWMWA_USE_IMMERSIVE_DARK_MODE, &dwmDark,
                                               sizeof(dwmDark))) &&
        dwmDark) {
        return true;
    }
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    SendMessageW(c->edit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    if ((cf.dwMask & CFM_COLOR) && !(cf.dwEffects & CFE_AUTOCOLOR)) {
        int lum = (30 * GetRValue(cf.crTextColor) + 59 * GetGValue(cf.crTextColor) +
                   11 * GetBValue(cf.crTextColor)) / 100;
        return lum >= 128;
    }
    return SystemAppsUseDark();
}

static void GuideBuild(EditorCtx* c) {
    GuideModel& g = c->guide;
    HWND edit = c->edit;
    g.rows.clear();
    g.valid = false;
    g.hash = 0;

    LONG lineCount = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200};
    LONG textLen = (LONG)SendMessageW(edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    if (lineCount <= 0 || textLen <= 0) return;
    LONG first = std::clamp<LONG>((LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0), 0,
                                  lineCount - 1);

    RECT cr;
    GetClientRect(edit, &cr);
    int dpi = (int)GetDpiForWindow(edit);
    if (dpi <= 0) dpi = 96;
    if (g.pitch <= 0) g.pitch = MulDiv(18, dpi, 96);
    POINT sp{};
    SendMessageW(edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);

    struct R {
        LONG idx, v;
        int y, x;
        int lead, xText;
        bool rel, para, blank;
        int indent, levels;
    };
    std::vector<R> rows;

    LONG v0 = std::max<LONG>(0, first - kGuideCtxRows);
    LONG prevIdx = -1, prevLen = 0;
    if (v0 > 0) {
        prevIdx = (LONG)SendMessageW(edit, EM_LINEINDEX, v0 - 1, 0);
        if (prevIdx >= 0) prevLen = (LONG)SendMessageW(edit, EM_LINELENGTH, prevIdx, 0);
    }
    bool pastBottom = false, seenAfter = false;
    LONG extra = 0;
    for (LONG v = v0; v < lineCount; v++) {
        if (pastBottom && (seenAfter || extra >= kGuideCtxRows)) break;
        LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
        if (idx < 0) break;
        POINT pt{};
        if (!PosFromChar(edit, idx, &pt)) break;
        LONG len = (LONG)SendMessageW(edit, EM_LINELENGTH, idx, 0);

        R r{};
        r.idx = idx;
        r.v = v;
        r.y = pt.y;
        r.x = pt.x;
        r.rel = idx > 0 && idx < textLen;
        r.para = idx == 0 ||
                 (prevIdx >= 0 && (idx - prevIdx <= 1 || idx > prevIdx + prevLen));
        r.blank = true;
        if (r.para && len > 0) {
            WCHAR buf[kGuideReadChars + 2];
            LONG want = std::min<LONG>(len, kGuideReadChars);
            *(WORD*)buf = (WORD)(want + 1);
            LRESULT got = SendMessageW(edit, EM_GETLINE, v, (LPARAM)buf);
            if (got > want) got = want;
            int lead = 0;
            while (lead < got && (buf[lead] == L' ' || buf[lead] == L'\t')) lead++;
            r.lead = lead;
            r.blank = lead >= got;
            if (!r.blank && lead > 0) {
                POINT p2{};
                if (PosFromChar(edit, idx + lead, &p2))
                    r.xText = p2.x;
                else
                    r.blank = true;
            }
        }
        rows.push_back(r);
        prevIdx = idx;
        prevLen = len;

        if (r.rel && pt.y >= cr.bottom) {
            pastBottom = true;
            extra++;
            if (r.para && !r.blank) seenAfter = true;
        } else if (pastBottom) {
            extra++;
            if (r.para && !r.blank) seenAfter = true;
        }
    }
    if (rows.empty()) return;

    // Row pitch and y of unreliable rows, as in BuildSnapshot.
    int pitch = 0;
    for (size_t k = 0; k + 1 < rows.size(); k++) {
        if (rows[k].rel && rows[k + 1].rel && rows[k + 1].v == rows[k].v + 1) {
            int d = rows[k + 1].y - rows[k].y;
            if (d > 0 && (pitch == 0 || d < pitch)) pitch = d;
        }
    }
    if (pitch > 0)
        g.pitch = pitch;
    else
        pitch = g.pitch;
    for (int k = 0; k < (int)rows.size(); k++) {
        if (rows[(size_t)k].rel) continue;
        int ref = -1;
        for (int d = 1; d < (int)rows.size() && ref < 0; d++) {
            if (k - d >= 0 && rows[(size_t)(k - d)].rel)
                ref = k - d;
            else if (k + d < (int)rows.size() && rows[(size_t)(k + d)].rel)
                ref = k + d;
        }
        if (ref >= 0)
            rows[(size_t)k].y = rows[(size_t)ref].y + (k - ref) * pitch;
        else
            rows[(size_t)k].y = MulDiv(16, dpi, 96) + (int)rows[(size_t)k].v * pitch - sp.y;
    }

    int x0 = rows[0].x;
    for (const R& r : rows)
        if (r.rel) {
            x0 = r.x;
            break;
        }
    for (R& r : rows) {
        r.indent = (r.lead > 0 && !r.blank) ? std::max(0, r.xText - x0) : 0;
    }

    // Indent size = most common pixel step between consecutive text lines (tabs, 2/4/8 spaces).
    const int tol = std::max(2, pitch / 10);
    const int minUnit = std::max(4, pitch * 6 / 10);
    std::vector<int> diffs;
    int prevIndent = -1;
    for (const R& r : rows) {
        if (!r.para || r.blank) continue;
        if (prevIndent >= 0 && r.indent != prevIndent) {
            int d = std::abs(r.indent - prevIndent);
            if (d >= minUnit) diffs.push_back(d);
        }
        prevIndent = r.indent;
    }
    std::sort(diffs.begin(), diffs.end());
    int best = 0, bestCount = 0;
    for (size_t i = 0; i < diffs.size();) {
        size_t j = i + 1;
        while (j < diffs.size() && diffs[j] - diffs[j - 1] <= tol) j++;
        int count = (int)(j - i);
        if (count > bestCount) {
            bestCount = count;
            best = diffs[i + (j - i) / 2];
        }
        i = j;
    }
    if (best >= minUnit && (bestCount >= 2 || g.unitPx <= 0)) {
        g.unitPx = best;
        g.unitPitch = pitch;
    } else if (g.unitPx > 0 && g.unitPitch > 0 && g.unitPitch != pitch) {
        g.unitPx = std::max(1, MulDiv(g.unitPx, pitch, g.unitPitch));
        g.unitPitch = pitch;
    }
    const int unit = g.unitPx;
    if (unit <= 0) return;

    for (R& r : rows)
        if (r.para && !r.blank)
            r.levels = std::min(kGuideMaxLevels, std::max(0, r.indent - tol + unit - 1) / unit);

    // Blank lines keep the guides of the block they sit in.
    const size_t n = rows.size();
    std::vector<int> prevL(n, -1), nextL(n, -1);
    int cur = -1;
    for (size_t i = 0; i < n; i++) {
        if (rows[i].para && !rows[i].blank) cur = rows[i].levels;
        prevL[i] = cur;
    }
    cur = -1;
    for (size_t i = n; i-- > 0;) {
        if (rows[i].para && !rows[i].blank) cur = rows[i].levels;
        nextL[i] = cur;
    }
    for (size_t i = 0; i < n; i++) {
        if (!rows[i].para || !rows[i].blank) continue;
        int pl = prevL[i], nl = nextL[i];
        int lv = 0;
        if (pl >= 0 && nl >= 0)
            lv = std::min(nl, pl + 1);
        else if (pl >= 0)
            lv = pl;
        else if (nl >= 0)
            lv = nl;
        rows[i].levels = lv;
    }

    uint64_t hsh = 1469598103934665603ULL;
    auto mix = [&](int64_t f) {
        hsh ^= (uint64_t)f;
        hsh *= 1099511628211ULL;
    };
    mix(x0);
    mix(unit);
    for (size_t k = 0; k < n; k++) {
        int h = (k + 1 < n && rows[k + 1].y > rows[k].y) ? rows[k + 1].y - rows[k].y : pitch;
        g.rows.push_back({rows[k].y, h, rows[k].levels});
        mix(rows[k].y);
        mix(h);
        mix(rows[k].levels);
    }
    g.x0 = x0;
    g.clientW = cr.right;
    g.clientH = cr.bottom;
    g.scroll = sp;
    g.hash = hsh;
    g.valid = true;
}

// Called from SyntaxTick: re-measures when something affecting the guides changed.
static void GuideTick(EditorCtx* c, LONG len, LONG lines) {
    GuideModel& g = c->guide;
    HWND edit = c->edit;
    if (len != g.sigLen || lines != g.sigLines) {
        g.sigLen = len;
        g.sigLines = lines;
        g.sigValid = false;
        g.settle = 8;
    }

    LONG sig[16] = {};
    int n = 0;
    sig[n++] = len;
    sig[n++] = lines;
    LONG first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    sig[n++] = first;
    POINT sp{};
    SendMessageW(edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    sig[n++] = sp.x;
    sig[n++] = sp.y;
    RECT cr;
    GetClientRect(edit, &cr);
    sig[n++] = cr.right;
    sig[n++] = cr.bottom;
    sig[n++] = (LONG)GetDpiForWindow(edit);
    // Moves with font, zoom and DPI changes.
    LONG i0 = (LONG)SendMessageW(edit, EM_LINEINDEX, first, 0);
    LONG i1 = (LONG)SendMessageW(edit, EM_LINEINDEX, first + 1, 0);
    POINT p{};
    if (i0 >= 0 && PosFromChar(edit, i0 + 1, &p)) {
        sig[n++] = p.x;
        sig[n++] = p.y;
    } else {
        n += 2;
    }
    p = {};
    if (i1 >= 0 && PosFromChar(edit, i1, &p)) sig[n++] = p.y;

    bool changed = !g.sigValid || !std::equal(sig, sig + 16, g.sig);
    bool rebuild = changed || g.settle > 0;
    // Drains on timer ticks only, like synSettle.
    if (g.settle > 0 && !c->inMsg) g.settle--;
    if (changed) {
        std::copy(sig, sig + 16, g.sig);
        g.sigValid = true;
    }

    bool needPaint = false;
    if (rebuild) {
        uint64_t before = g.hash;
        GuideBuild(c);
        if (g.hash != before) needPaint = true;
    }
    int epoch = g_themeEpoch.load();
    if (rebuild || epoch != g.themeEpoch || c->tickMs >= kTickSlowMs || ++g.themeTick % 30 == 0) {
        g.themeEpoch = epoch;
        bool d = GuideDark(c);
        if (d != g.dark) {
            g.dark = d;
            needPaint = true;
        }
    }
    if (needPaint) InvalidateRect(edit, nullptr, FALSE);
}

static void PreviewTick(EditorCtx* c, ULONGLONG now);
static void PreviewHide(EditorCtx* c);

// Runs every tick for every editor: keeps language, document copy, token classes and row snapshot current.
static void SyntaxTick(EditorCtx* c) {
    if (!IsWindow(c->edit)) return;
    if (!IsWindowVisible(c->edit)) {
        c->shown = false;
        PreviewHide(c);
        return;
    }
    if (!c->shown) {
        c->shown = true;
        c->lang = -2;
        c->synTick = 0;
        c->synSigValid = false;
    }

    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200};
    LONG len = (LONG)SendMessageW(c->edit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
    LONG lines = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    ULONGLONG now = GetTickCount64();
    if (len != c->synLen || lines != c->synLines) {
        c->synLen = len;
        c->synLines = lines;
        c->docStale = true;
        c->synChangeTick = now;
        c->synSigValid = false;
        c->synSettle = 8;
        c->docRetry = 0;
    }

    GuideTick(c, len, lines);

    bool wasStale = c->docStale;
    int oldLang = c->lang;
    c->mkOn = c->mkNameOk && !c->mkEmptyBase && (size_t)len <= kMkMaxChars;
    RefreshSyntax(c, len, now);
    MarkerTick(c, now);
    PreviewTick(c, now);
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
        // Only the selection moved: rows are unchanged, no repaint needed.
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
        // Repaint once more so the colors match the new snapshot after a big jump.
        if (!viewSame) InvalidateRect(c->edit, nullptr, FALSE);
    }

    // After a text change the editor may finish layout late: keep re-measuring briefly, repaint only if it changed.
    if (c->synSettle > 0 && !c->inMsg) {
        c->synSettle--;
        if (!rebuilt) {
            uint64_t before = c->snapHash;
            BuildSnapshot(c);
            if (c->snapHash != before) InvalidateRect(c->edit, nullptr, FALSE);
        }
    }
}

// Minimap (1/2): document model, geometry, layout (one row per logical line, slider marks the visible part)

struct MmGeo {
    LONG nLines = 1;
    double top = 0, view = 1;
    int pitch = 3, bh = 2;
    int cap = 1;
    int contentPx = 0;
    int sliderPx = 0;
    int sliderTop = 0;
    double mmTop = 0;
    bool haveModel = false;
};

// Visual rows: total, first visible, last visible (EM_POSFROMCHAR is reliable except for the first row).
static void ViewRows(EditorCtx* c, LONG& rows, LONG& first, LONG& last) {
    HWND edit = c->edit;
    rows = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    last = first;
    if (rows <= 0) return;
    first = std::clamp<LONG>(first, 0, rows - 1);
    RECT cr;
    GetClientRect(edit, &cr);
    // [keep] cap the search near the screen; widen the cap if every probe is still visible
    const LONG minPitch = std::max<LONG>(4, c->fontPx / 2);
    LONG lo = first;
    LONG hi = std::min<LONG>(rows - 1, first + cr.bottom / minPitch + 1);
    for (;;) {
        const LONG bound = hi;
        while (lo < hi) {
            LONG mid = lo + (hi - lo + 1) / 2;
            LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, mid, 0);
            POINT pt{};
            if (idx >= 0 && PosFromChar(edit, idx, &pt) && pt.y < cr.bottom)
                lo = mid;
            else
                hi = mid - 1;
        }
        if (lo < bound || bound >= rows - 1) break;
        hi = std::min<LONG>(rows - 1, bound + (bound - first) + 1);
    }
    last = lo;
}

// "Scrollbar appears" == the document does not fit.
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
        if (t[i] == L'\r') c->mmStart.push_back((uint32_t)i + 1);
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
        g.nLines = rows;
        g.top = (double)first;
        g.view = (double)(last - first + 1);
    }
    // While dragging, keep the viewport size from the drag start so the mouse-to-scroll mapping stays stable.
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

// Scrolls so that a logical line (minimap space) becomes the first visible line.
static void MmScrollToTopLine(EditorCtx* c, const MmGeo& g, LONG line) {
    line = std::clamp<LONG>(line, 0, g.nLines - 1);
    LONG idx = g.haveModel ? (LONG)c->mmStart[(size_t)line]
                           : (LONG)SendMessageW(c->edit, EM_LINEINDEX, line, 0);
    if (idx < 0) return;
    LONG v = (LONG)SendMessageW(c->edit, EM_LINEFROMCHAR, idx, 0);
    LONG first = (LONG)SendMessageW(c->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    if (v != first) SendMessageW(c->edit, EM_LINESCROLL, 0, (LPARAM)(v - first));
}

// Scrolls to the very end: asks for more than enough and lets the editor stop at its maximum.
static void MmScrollToEnd(EditorCtx* c) {
    LONG rows = (LONG)SendMessageW(c->edit, EM_GETLINECOUNT, 0, 0);
    if (rows <= 0) return;
    RECT cr;
    GetClientRect(c->edit, &cr);
    POINT sp{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    // Skips repeated "scroll to end" requests once the editor is already there.
    if (c->mmEndRows == rows && c->mmEndH == cr.bottom && c->mmEndY == sp.y) return;
    SendMessageW(c->edit, EM_LINESCROLL, 0, (LPARAM)rows);
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    c->mmEndRows = rows;
    c->mmEndH = cr.bottom;
    c->mmEndY = sp.y;
}

// [keep] comment moved here from MmScrollToTopLine
// Scrolls so the slider's top edge lands on minimap y = yTop; slider travel maps linearly onto the whole document.
static void MmScrollTo(EditorCtx* c, const MmGeo& g, int yTop) {
    double V = std::min<double>(g.view, (double)g.nLines);
    if ((double)g.nLines <= V) return;
    int travel = g.contentPx - g.sliderPx;
    double f = travel > 0 ? std::clamp((double)yTop / travel, 0.0, 1.0) : 0.0;
    if (f >= 1.0) {
        MmScrollToEnd(c);
        return;
    }
    MmScrollToTopLine(c, g, (LONG)std::lround(f * ((double)g.nLines - V)));
}

// Mouse moves only record the slider position; the ~60 Hz tick applies the latest one.
static void MmApplyPendingDrag(EditorCtx* c, int H) {
    if (!c->mmDragPending) return;
    c->mmDragPending = false;
    if (!IsWindow(c->edit)) return;
    MmGeo g;
    if (MmCompute(c, H, g)) MmScrollTo(c, g, c->mmDragY - c->mmGrab);
}

// Jump to the clicked line, centered; only reaches lines the minimap currently shows.
static void MmJumpToLine(EditorCtx* c, const MmGeo& g, LONG line) {
    double V = std::min<double>(g.view, (double)g.nLines);
    if ((double)g.nLines <= V) return;
    double top = (double)line - V / 2.0 + 0.5;
    if (top >= (double)g.nLines - V) {
        MmScrollToEnd(c);
        return;
    }
    MmScrollToTopLine(c, g, std::max<LONG>(0, (LONG)std::lround(top)));
}

// Width follows the editor's width in steps, so live resizes don't re-wrap on every pixel.
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
    // Right edge of the editor's client area, just left of the scrollbar.
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
    c->mmW = 0;
    c->mmSigValid = false;
    c->mmBuiltRev = ~0u;
    if (!c->docOk) c->docStale = true;
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

// Minimap half of the tick: visibility, width, position and model; invalidates on visible changes.
static void MinimapTick(EditorCtx* c) {
    RECT cr;
    GetClientRect(c->edit, &cr);
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;

    int mw = MinimapWidth(cr.right, dpi);
    // Hide when too little room is left for the text (with hysteresis).
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

// Gutter half of the tick; runs only while line numbers are on.
// Pixels of the editor's left edge that lie past the monitor edge (a maximized window's hidden frame).
static int HiddenLeft(HWND w) {
    RECT r;
    MONITORINFO mi = {sizeof(mi)};
    if (!GetWindowRect(w, &r) || !GetMonitorInfoW(MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST), &mi)) return 0;
    return (int)std::max<LONG>(0, mi.rcMonitor.left - r.left);
}

static void GutterTick(EditorCtx* c, bool fontChanged) {
    // Were line numbers shown or hidden since the last tick? (The marker strip stays.)
    bool nums = g_enabled.load();
    if (nums != c->numsShown) {
        c->numsShown = nums;
        c->textDirty = true;
        c->gutterW = 0;
        c->sigValid = false;
    }
    if (c->mkDirty) {
        c->mkDirty = false;
        c->sigValid = false;
    }

    // Detect text changes (length / visual line count)
    GETTEXTLENGTHEX gtl = {GTL_NUMCHARS, 1200};
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
        // [keep] very large file whose copy is not re-read yet: stay dirty and recount from the copy, not by walking rows
        const bool waitCopy = c->numsShown && len > 200000 && c->docStale && now - c->lastChangeTick <= 1000;
        if (!waitCopy) {  // [keep] the anchors stay valid meanwhile, so repaints do not walk from row 0
            c->viewCache.valid = false;
            c->caretCache.valid = false;
        }
        // Debounce the full recount while typing in very large files.
        if (!c->numsShown) {
            c->textDirty = false;
        } else if (!waitCopy && ((len <= 4000000 && lines <= 50000) || now - c->lastChangeTick > 200)) {
            // [keep] line count = line table size when the document copy is current (see LineModelUsable)
            c->totalLogical = LineModelUsable(c) ? (LONG)c->mmStart.size() : CountBreaks(c, 0, len) + 1;
            c->textDirty = false;
        }
        c->sigValid = false;
    }

    c->mkOff = HiddenLeft(c->edit);

    // Dynamic width: at least 2 digits, grows at 100 / 1000 / ...
    int w;
    if (c->numsShown) {
        int digits = std::max(2, CountDigits(c->totalLogical));
        w = MeasureGutterWidth(c, digits);
    } else {
        int dpi = (int)GetDpiForWindow(c->edit);
        w = MulDiv(kMkStripDip, dpi > 0 ? dpi : 96, 96) + c->mkOff;
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

// [keep] adaptive tick rate: fast only while busy, slow backstop otherwise; WakeTick() speeds it up

// ms == 0 stops the timer; SetTimer with the same id replaces it.
static void SetTickRate(EditorCtx* c, UINT ms) {
    if (!c->gutter || c->tickMs == ms) return;
    c->tickMs = ms;
    if (ms) {
        if (!SetTimer(c->gutter, kTickTimer, ms, nullptr)) c->tickMs = 0;
    } else {
        KillTimer(c->gutter, kTickTimer);
    }
}

// Something happened: run fast for a short while.
static void WakeTick(EditorCtx* c) {
    c->fastUntil = GetTickCount64() + kTickGraceMs;
    SetTickRate(c, kTickFastMs);
}

// Work only the timer tick does; every condition drains by itself, so the timer cannot get stuck fast.
static bool TickBusy(EditorCtx* c, ULONGLONG now) {
    if (c->mmDrag || c->mmDragPending) return true;
    if (GetCapture() == c->edit) return true;
    // Settle counters only drain in ticks that reach them.
    if (c->settle > 0 && (c->active || c->mmActive)) return true;
    if (c->synSettle > 0 && c->lang >= 0) return true;
    // Marker baseline still follows a loading document.
    if (c->mkOn && c->mkHave && !c->mkFrozen && now < c->mkFollowUntil) return true;
    // Document copy still to be read; mirrors RefreshSyntax so an unneeded copy doesn't keep the timer fast.
    if (c->docStale && (c->lang >= 0 || c->mmActive || c->mkOn) &&
        (size_t)c->synLen <= kSyntaxMaxChars)
        return true;
    return false;
}

// Picks the rate after a tick: fast while busy, slow otherwise (hidden editors keep a slow poll; WM_SHOWWINDOW isn't reliable).
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
    // [keep] re-pick the timer rate on every exit of this tick
    struct RateGuard {
        EditorCtx* c;
        ~RateGuard() { SettleTickRate(c); }
    } rateGuard{c};

    // [keep] skip the tick while minimized (IsWindowVisible stays true)
    {
        HWND topWnd = GetAncestor(c->edit, GA_ROOT);
        if (topWnd && IsIconic(topWnd)) return;
    }

    // [keep] at the slow rate run the periodic title/theme checks on every tick
    if (c->tickMs >= kTickSlowMs && !c->inMsg) {
        c->synTick = 14;
        c->tickCount = 29;
    }

    SyntaxTick(c);

    bool wantGutter = g_enabled.load() || c->mkOn;
    if (wantGutter != c->active) {
        if (wantGutter)
            Activate(c);
        else
            Deactivate(c);
    }
    bool wantMinimap = g_minimapEnabled.load() && c->minimap;
    if (!wantMinimap && c->mmActive) DeactivateMinimap(c);
    if (!c->gutter || (!c->active && !wantMinimap)) return;

    if (!IsWindowVisible(c->edit)) {
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
            c->settle = std::max(c->settle, 40);
        }
        // Self-test: does the editor honor our left inset?
        if (!c->rectFallback && c->gutterW > 0 && !InsetHonored(c, c->gutterW)) {
            if (++c->insetFailures >= 3) {
                c->rectFallback = true;
                ApplyInset(c);
            }
        } else {
            c->insetFailures = 0;
        }
    }

    // Keep re-checking briefly after an edit so the gutter never keeps stale row positions.
    if (c->settle > 0 && !c->inMsg) {
        c->settle--;
        c->sigValid = false;
    }

    if (c->active) GutterTick(c, fontChanged);
    if (wantMinimap && !c->inMsg) MinimapTick(c);
}

// Painting (double-buffered)

// Row geometry learned from reliable rows; per editor, keyed by pixel font size so zoom/DPI changes reset it.
// [keep] calibration is per editor now (was shared g_cal* globals)

static void DrawNumbers(EditorCtx* c, HDC dc, int w, const Palette& pal) {
    HWND edit = c->edit;
    LONG lineCount = (LONG)SendMessageW(edit, EM_GETLINECOUNT, 0, 0);
    LONG first = (LONG)SendMessageW(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
    LONG v = first > 0 ? first - 1 : 0;
    RECT cr;
    GetClientRect(edit, &cr);

    LONG idx = (LONG)SendMessageW(edit, EM_LINEINDEX, v, 0);
    if (idx < 0) return;
    LONG breaks = BreaksBefore(c, c->viewCache, idx);

    DWORD cs = 0, ce = 0;
    SendMessageW(edit, EM_GETSEL, (WPARAM)&cs, (LPARAM)&ce);
    LONG countedCaretLine = BreaksBefore(c, c->caretCache, (LONG)ce) + 1;

    int padR = MulDiv(10, (int)GetDpiForWindow(edit), 96);
    // Pass 1: collect visible rows; EM_POSFROMCHAR is wrong for the first and a trailing empty row (!rel, fixed in 1b).
    struct Row {
        LONG idx;
        LONG v;
        int y;
        LONG label;
        bool rel;
        int x;
        LONG line;
    };
    // Length in the editor's numbering (CRLF = one character).
    LONG docLen = ExpectedEditorLength(c);
    // Second opinion on where the text ends: the editor's character count.
    GETTEXTLENGTHEX gtlLive = {GTL_NUMCHARS, 1200};
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

        // A row starts a logical line when the previous char is a break; wrapped rows stay blank.
        bool isStart = (i == 0);
        if (!isStart) isStart = RowStartsParagraphByGap(edit, v, i);
        if (!firstRow && isStart) breaks++;
        firstRow = false;

        bool rel = (i > 0 && i < docLen);
        // A row not below the previous one got its position back (Enter at the end): unreliable.
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
            pitch = t.tmHeight + t.tmExternalLeading;
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
                // Nothing learned yet: guess the top inset (16 DIP unless the formatting rect says otherwise).
                RECT fr{};
                SendMessageW(edit, EM_GETRECT, 0, (LPARAM)&fr);
                int dpi = (int)GetDpiForWindow(edit);
                int top = fr.top > 0 ? fr.top : MulDiv(16, dpi > 0 ? dpi : 96, 96);
                rowList[k].y = top + (int)rowList[k].v * pitch - sp.y;
            }
        }
    }

    // Highlighted line: taken from the rows being drawn; if the caret is out of view, use the break-count value.
    int caretRow = -1;
    for (int k = 0; k < (int)rowList.size(); k++) {
        if (rowList[k].idx <= (LONG)ce) caretRow = k;
        else break;
    }
    if (caretRow >= 0 && caretRow == (int)rowList.size() - 1) {
        // Last listed row: valid only if the caret is inside it.
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

    // Change markers: a bar on the gutter's left edge for added/modified lines, a notch where lines were removed.
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
            RECT br = {c->mkOff, r.y, c->mkOff + barW, r.y + rowPitch};
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
                POINT pts[3] = {{c->mkOff, y - hh}, {c->mkOff + hw, y}, {c->mkOff, y + hh}};
                Polygon(dc, pts, 3);
            };
            for (const Row& r : rowList) {
                if (r.label <= 0) continue;
                if (std::binary_search(c->mkDel.begin(), c->mkDel.end(),
                                       (uint32_t)std::max<LONG>(r.line - 1, 0)))
                    notch(r.y);
            }
            // Lines removed at the very end: just below the last row.
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

    // Pass 2: draw; the current line uses the highlight color, optionally underlined and/or bold.
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

    // Numbers on: the gutter is its own panel; markers only: the editor's background, sampled once per theme.
    if (!c->numsShown && c->gutter) {
        int key = (c->dark ? 1 : 0) | (g_themeEpoch.load() << 1);
        if (key != c->edBgKey) {
            c->edBgKey = key;
            c->edBg = c->dark ? kEditorGuessDark : kEditorGuessLight;
            if (HDC scr = GetDC(nullptr)) {
                POINT o = {0, 0};
                ClientToScreen(c->gutter, &o);
                int gdpi = (int)GetDpiForWindow(c->gutter);
                int gap = MulDiv(2, gdpi > 0 ? gdpi : 96, 96);
                COLORREF px = GetPixel(scr, o.x + w + gap, o.y + h / 2);
                if (px != CLR_INVALID && px != RGB(0, 0, 0)) c->edBg = px;
                ReleaseDC(nullptr, scr);
            }
        }
    }
    {
        HBRUSH bg = CreateSolidBrush(c->numsShown ? pal.bg : c->edBg);
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
            return HTTRANSPARENT;
        case WM_TIMER:
            if (wParam == kTickTimer && c && !g_unloading.load()) Tick(c);
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

// Minimap (2/2): painting and mouse handling

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

// Blends col over the existing pixels; alpha is 0..255.
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

// Draws one logical line as runs of same-colored blocks (spaces leave gaps, tabs = 4 columns).
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
    bmi.bmiHeader.biHeight = -h;
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
    MmFill(cv, 0, 0, 1, h, PxOf(pal.separator));

    MmGeo g;
    if (IsWindow(c->edit) && MmCompute(c, h, g)) {
        int dpi = (int)GetDpiForWindow(c->edit);
        if (dpi <= 0) dpi = 96;
        COLORREF text = c->textColorKnown ? c->textColor
                                          : (c->dark ? RGB(0xD4, 0xD4, 0xD4) : RGB(0x33, 0x33, 0x33));

        // One color per token class, softened toward the background.
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
                uint32_t e = (k + 1 < g.nLines) ? c->mmStart[(size_t)k + 1] - 1 : docLen;
                e = std::min(e, docLen);
                if (e > s) MmDrawLine(c, cv, s, e, y, g.bh, padL, inner, clsPx);
            }
        }

        // The slider: a translucent band over the visible part.
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
            WakeTick(c);  // [keep] click/drag needs the fast timer
            SetFocus(c->edit);
            RECT rc;
            GetClientRect(h, &rc);
            MmGeo g;
            if (MmCompute(c, rc.bottom, g)) {
                int y = GET_Y_LPARAM(lParam);
                bool onSlider = y >= g.sliderTop && y < g.sliderTop + g.sliderPx;
                if (!onSlider && g_mmClickLine.load()) {
                    // Click-to-line: inverse of PaintMinimap's placement, so you land on the line under the cursor.
                    const LONG k0 = (LONG)std::floor(g.mmTop);
                    const int yOff = -(int)std::lround((g.mmTop - (double)k0) * g.pitch);
                    LONG line = k0 + (LONG)(std::max(0, y - yOff) / g.pitch);
                    MmJumpToLine(c, g, std::clamp<LONG>(line, 0, g.nLines - 1));
                    InvalidateRect(h, nullptr, FALSE);
                    return 0;
                }
                // Grab the slider where hit; otherwise (proportional mode) center it on the click and keep dragging.
                c->mmGrab = onSlider ? y - g.sliderTop : g.sliderPx / 2;
                c->mmDrag = true;
                c->mmEndRows = -1;
                c->mmDragV = g.view;
                SetCapture(h);
                MmScrollTo(c, g, y - c->mmGrab);
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
                c->mmDragY = GET_Y_LPARAM(lParam);
                c->mmDragPending = true;
                WakeTick(c);  // [keep] apply the drag on the fast timer
            }
            return 0;
        }
        case WM_LBUTTONUP:
            if (c && c->mmDrag) {
                RECT rc;
                GetClientRect(h, &rc);
                MmApplyPendingDrag(c, rc.bottom);
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
        case WM_MOUSEWHEEL:
            if (c && IsWindow(c->edit)) return SendMessageW(c->edit, WM_MOUSEWHEEL, wParam, lParam);
            break;
        case WM_NCDESTROY:
            if (c) c->minimap = nullptr;
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

// Preview pane (1/2): converters. Markdown becomes RTF, CSV becomes rows, JSON/XML/INI become tree nodes.

enum PvKind { pvNone, pvMarkdown, pvRtf, pvCsv, pvTsv, pvJson, pvXml, pvIni };

struct PvPal {
    COLORREF bg, text, muted, link, code, codeBg;
};
constexpr PvPal kPvDark = {RGB(0x27, 0x27, 0x27), RGB(0xE0, 0xE0, 0xE0), RGB(0x9A, 0x9A, 0x9A),
                           RGB(0x6C, 0xB6, 0xFF), RGB(0xF0, 0xA7, 0x7A), RGB(0x3A, 0x3A, 0x3A)};
constexpr PvPal kPvLight = {RGB(0xF9, 0xF9, 0xF9), RGB(0x20, 0x20, 0x20), RGB(0x6A, 0x6A, 0x6A),
                            RGB(0x0B, 0x63, 0xCE), RGB(0xB0, 0x3A, 0x2E), RGB(0xEB, 0xEB, 0xEB)};
constexpr int kPvMinDip = 640;
constexpr ULONGLONG kPvDebounceMs = 300;
constexpr size_t kPvMaxChars = 300000;
constexpr size_t kNpos = std::wstring::npos;

static std::wstring PvTrim(const std::wstring& s) {
    size_t a = 0, b = s.size();
    while (a < b && (s[a] == L' ' || s[a] == L'\t')) a++;
    while (b > a && (s[b - 1] == L' ' || s[b - 1] == L'\t')) b--;
    return s.substr(a, b - a);
}

static int PvLine(const std::wstring& s, size_t pos) {
    int n = 1;
    for (size_t i = 0; i < pos && i < s.size(); i++)
        if (s[i] == L'\n' || (s[i] == L'\r' && !(i + 1 < s.size() && s[i + 1] == L'\n'))) n++;
    return n;
}

static void RtfText(std::string& o, const std::wstring& s) {
    for (WCHAR ch : s) {
        if (ch == L'\\' || ch == L'{' || ch == L'}') {
            o += '\\';
            o += (char)ch;
        } else if (ch == L'\t') {
            o += "\\tab ";
        } else if (ch == L'\n') {
            o += "\\line ";
        } else if (ch >= 0x20 && ch < 0x80) {
            o += (char)ch;
        } else if (ch >= 0x80) {
            o += "\\u" + std::to_string((int)(short)ch) + "?";
        }
    }
}

static void RtfColor(std::string& o, COLORREF c) {
    o += "\\red" + std::to_string(GetRValue(c)) + "\\green" + std::to_string(GetGValue(c)) +
         "\\blue" + std::to_string(GetBValue(c)) + ";";
}

static void MdInline(std::string& o, const std::wstring& s);

// A link is its text plus the URL in hidden text between private-use marks; PvLinks() finds them again.
static void MdLink(std::string& o, const std::wstring& text, const std::wstring& url) {
    o += "{\\v\\u57344?}{\\cf2\\ul ";
    if (text == url)
        RtfText(o, text);
    else
        MdInline(o, text);
    o += "}{\\v\\u57345?";
    RtfText(o, url);
    o += "\\u57346?}";
}

static size_t MdFindClose(const std::wstring& s, size_t from, const WCHAR* d) {
    size_t n = wcslen(d);
    for (size_t i = from; i + n <= s.size(); i++) {
        if (s[i] == L'\\') {
            i++;
            continue;
        }
        if (i > from && s.compare(i, n, d) == 0) return i;
    }
    return kNpos;
}

static bool MdUrlAt(const std::wstring& s, size_t i) {
    return s.compare(i, 7, L"http://") == 0 || s.compare(i, 8, L"https://") == 0;
}

static bool MdPunct(WCHAR c) { return c > 32 && c < 127 && !iswalnum(c); }

// Decodes &#N; &#xH; and the common named entities at s[i] == '&'; unknown names stay literal text.
static bool MdEntity(const std::wstring& s, size_t i, std::wstring& out, size_t& len) {
    size_t e = s.find(L';', i + 1);
    if (e == kNpos || e - i > 12) return false;
    std::wstring b = s.substr(i + 1, e - i - 1);
    uint32_t cp = 0;
    if (b.size() >= 2 && b[0] == L'#') {
        bool hex = b[1] == L'x' || b[1] == L'X';
        size_t k = hex ? 2 : 1;
        if (b.size() == k || b.size() - k > (hex ? 6u : 7u)) return false;
        for (size_t j = k; j < b.size(); j++) {
            WCHAR c = b[j];
            int d;
            if (c >= L'0' && c <= L'9')
                d = c - L'0';
            else if (hex && c >= L'a' && c <= L'f')
                d = c - L'a' + 10;
            else if (hex && c >= L'A' && c <= L'F')
                d = c - L'A' + 10;
            else
                return false;
            cp = cp * (hex ? 16 : 10) + d;
        }
        if (cp == 0 || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) cp = 0xFFFD;
    } else {
        static const struct {
            const WCHAR* n;
            WCHAR c;
        } T[] = {{L"amp", L'&'}, {L"lt", L'<'}, {L"gt", L'>'}, {L"quot", L'"'}, {L"apos", L'\''},
                 {L"nbsp", 0xA0}, {L"copy", 0xA9}, {L"reg", 0xAE}, {L"trade", 0x2122}, {L"hellip", 0x2026},
                 {L"mdash", 0x2014}, {L"ndash", 0x2013}, {L"lsquo", 0x2018}, {L"rsquo", 0x2019}, {L"ldquo", 0x201C},
                 {L"rdquo", 0x201D}, {L"laquo", 0xAB}, {L"raquo", 0xBB}, {L"bull", 0x2022}, {L"middot", 0xB7},
                 {L"times", 0xD7}, {L"divide", 0xF7}, {L"deg", 0xB0}, {L"plusmn", 0xB1}, {L"micro", 0xB5},
                 {L"para", 0xB6}, {L"sect", 0xA7}, {L"euro", 0x20AC}, {L"pound", 0xA3}, {L"yen", 0xA5},
                 {L"cent", 0xA2}, {L"larr", 0x2190}, {L"rarr", 0x2192}, {L"uarr", 0x2191}, {L"darr", 0x2193},
                 {L"harr", 0x2194}, {L"hearts", 0x2665}, {L"infin", 0x221E}, {L"ne", 0x2260}, {L"le", 0x2264},
                 {L"ge", 0x2265}, {L"frac12", 0xBD}, {L"frac14", 0xBC}, {L"frac34", 0xBE}, {L"AElig", 0xC6},
                 {L"auml", 0xE4}, {L"ouml", 0xF6}, {L"uuml", 0xFC}, {L"Auml", 0xC4}, {L"Ouml", 0xD6},
                 {L"Uuml", 0xDC}, {L"szlig", 0xDF}, {L"eacute", 0xE9}, {L"egrave", 0xE8}, {L"aacute", 0xE1},
                 {L"agrave", 0xE0}, {L"ccedil", 0xE7}, {L"ntilde", 0xF1}};
        for (const auto& t : T)
            if (b == t.n) cp = t.c;
        if (!cp) return false;
    }
    out.clear();
    if (cp > 0xFFFF && sizeof(WCHAR) == 2) {
        cp -= 0x10000;
        out += (WCHAR)(0xD800 + (cp >> 10));
        out += (WCHAR)(0xDC00 + (cp & 0x3FF));
    } else {
        out += (WCHAR)cp;
    }
    len = e - i + 1;
    return true;
}

// A tag or comment at s[i] == '<': returns the index after it (0 if it is not one) and fills name (lowercase) and close.
static size_t MdTagEnd(const std::wstring& s, size_t i, std::wstring& name, bool& close) {
    name.clear();
    close = false;
    if (s.compare(i, 4, L"<!--") == 0) {
        size_t e = s.find(L"-->", i + 4);
        if (e == kNpos) return 0;
        name = L"!--";
        return e + 3;
    }
    size_t k = i + 1;
    if (k < s.size() && s[k] == L'/') {
        close = true;
        k++;
    }
    if (k >= s.size() || s[k] >= 128 || !iswalpha(s[k])) return 0;
    while (k < s.size() && s[k] < 128 && (iswalnum(s[k]) || s[k] == L'-')) name += (WCHAR)towlower(s[k++]);
    if (k < s.size() && !(iswspace(s[k]) || s[k] == L'>' || s[k] == L'/')) return 0;
    WCHAR q = 0;
    for (; k < s.size(); k++) {
        WCHAR c = s[k];
        if (q) {
            if (c == q) q = 0;
        } else if (c == L'"' || c == L'\'') {
            q = c;
        } else if (c == L'>') {
            return k + 1;
        } else if (c == L'<') {
            return 0;
        }
    }
    return 0;
}

static std::wstring MdAttr(const std::wstring& tag, const std::wstring& key) {
    std::wstring lo = tag;
    for (WCHAR& c : lo) c = (WCHAR)towlower(c);
    size_t p = 0;
    while ((p = lo.find(key + L"=", p)) != kNpos) {
        if (p > 0 && iswspace(lo[p - 1])) break;
        p += key.size();
    }
    if (p == kNpos) return L"";
    p += key.size() + 1;
    if (p >= tag.size()) return L"";
    WCHAR q = tag[p];
    size_t e;
    if (q == L'"' || q == L'\'') {
        e = tag.find(q, p + 1);
        return e == kNpos ? std::wstring() : tag.substr(p + 1, e - p - 1);
    }
    e = p;
    while (e < tag.size() && !iswspace(tag[e]) && tag[e] != L'>') e++;
    return tag.substr(p, e - p);
}

// Finds </name> at or after `from`; returns its start and sets end to the index after it.
static size_t MdCloseTag(const std::wstring& s, size_t from, const std::wstring& nm, size_t& end) {
    for (size_t p = from; (p = s.find(L"</", p)) != kNpos; p += 2) {
        size_t k = p + 2, j = 0;
        while (j < nm.size() && k < s.size() && (WCHAR)towlower(s[k]) == nm[j]) {
            k++;
            j++;
        }
        if (j != nm.size()) continue;
        while (k < s.size() && iswspace(s[k])) k++;
        if (k < s.size() && s[k] == L'>') {
            end = k + 1;
            return p;
        }
    }
    return kNpos;
}

static const char* MdTagFmt(const std::wstring& n) {
    if (n == L"b" || n == L"strong" || n == L"summary") return "\\b ";
    if (n == L"i" || n == L"em") return "\\i ";
    if (n == L"u" || n == L"ins") return "\\ul ";
    if (n == L"del" || n == L"s" || n == L"strike") return "\\strike ";
    if (n == L"code" || n == L"kbd") return "\\f1\\cf4 ";
    if (n == L"sub") return "\\sub ";
    if (n == L"sup") return "\\super ";
    return nullptr;
}

static size_t MdBracketEnd(const std::wstring& s, size_t from) {
    int d = 0;
    for (size_t i = from; i < s.size(); i++) {
        if (s[i] == L'\\') {
            i++;
        } else if (s[i] == L'[') {
            d++;
        } else if (s[i] == L']') {
            if (!d) return i;
            d--;
        }
    }
    return kNpos;
}

static std::wstring MdLabel(const std::wstring& s) {
    std::wstring r;
    bool sp = false;
    for (WCHAR c : s) {
        if (iswspace(c)) {
            sp = !r.empty();
        } else {
            if (sp) r += L' ';
            sp = false;
            r += (WCHAR)towlower(c);
        }
    }
    return r;
}

static void MdPutCp(std::wstring& out, uint32_t cp) {
    if (cp > 0xFFFF && sizeof(WCHAR) == 2) {
        cp -= 0x10000;
        out += (WCHAR)(0xD800 + (cp >> 10));
        out += (WCHAR)(0xDC00 + (cp & 0x3FF));
    } else {
        out += (WCHAR)cp;
    }
}

// Common GitHub emoji shortcodes (name without colons) -> code point; 0 if unknown.
static uint32_t MdEmoji(const std::wstring& name) {
    static const struct {
        const WCHAR* n;
        uint32_t c;
    } T[] = {{L"smile", 0x1F604}, {L"smiley", 0x1F603}, {L"grinning", 0x1F600}, {L"laughing", 0x1F606}, {L"joy", 0x1F602},
             {L"wink", 0x1F609}, {L"blush", 0x1F60A}, {L"heart_eyes", 0x1F60D}, {L"sunglasses", 0x1F60E}, {L"thinking", 0x1F914},
             {L"neutral_face", 0x1F610}, {L"confused", 0x1F615}, {L"cry", 0x1F622}, {L"sob", 0x1F62D}, {L"scream", 0x1F631},
             {L"angry", 0x1F620}, {L"sweat_smile", 0x1F605}, {L"innocent", 0x1F607}, {L"sleeping", 0x1F634}, {L"clap", 0x1F44F},
             {L"wave", 0x1F44B}, {L"ok_hand", 0x1F44C}, {L"pray", 0x1F64F}, {L"muscle", 0x1F4AA}, {L"+1", 0x1F44D},
             {L"-1", 0x1F44E}, {L"thumbsup", 0x1F44D}, {L"thumbsdown", 0x1F44E}, {L"raised_hands", 0x1F64C}, {L"eyes", 0x1F440},
             {L"point_right", 0x1F449}, {L"point_left", 0x1F448}, {L"point_up", 0x261D}, {L"point_down", 0x1F447},
             {L"heart", 0x2764}, {L"broken_heart", 0x1F494}, {L"green_heart", 0x1F49A}, {L"blue_heart", 0x1F499},
             {L"yellow_heart", 0x1F49B}, {L"purple_heart", 0x1F49C}, {L"star", 0x2B50}, {L"star2", 0x1F31F}, {L"sparkles", 0x2728},
             {L"fire", 0x1F525}, {L"zap", 0x26A1}, {L"boom", 0x1F4A5}, {L"tada", 0x1F389}, {L"balloon", 0x1F388},
             {L"gift", 0x1F381}, {L"trophy", 0x1F3C6}, {L"medal", 0x1F3C5}, {L"rocket", 0x1F680}, {L"airplane", 0x2708},
             {L"car", 0x1F697}, {L"bug", 0x1F41B}, {L"bulb", 0x1F4A1}, {L"memo", 0x1F4DD}, {L"pencil2", 0x270F},
             {L"book", 0x1F4D6}, {L"books", 0x1F4DA}, {L"bookmark", 0x1F516}, {L"link", 0x1F517}, {L"paperclip", 0x1F4CE},
             {L"pushpin", 0x1F4CC}, {L"lock", 0x1F512}, {L"unlock", 0x1F513}, {L"key", 0x1F511}, {L"mag", 0x1F50D},
             {L"bell", 0x1F514}, {L"mega", 0x1F4E3}, {L"warning", 0x26A0}, {L"no_entry", 0x26D4}, {L"no_entry_sign", 0x1F6AB},
             {L"x", 0x274C}, {L"heavy_check_mark", 0x2714}, {L"white_check_mark", 0x2705}, {L"ballot_box_with_check", 0x2611},
             {L"heavy_plus_sign", 0x2795}, {L"heavy_minus_sign", 0x2796}, {L"question", 0x2753}, {L"exclamation", 0x2757},
             {L"bangbang", 0x203C}, {L"construction", 0x1F6A7}, {L"hammer", 0x1F528}, {L"wrench", 0x1F527}, {L"gear", 0x2699},
             {L"package", 0x1F4E6}, {L"computer", 0x1F4BB}, {L"email", 0x1F4E7}, {L"calendar", 0x1F4C5}, {L"hourglass", 0x231B},
             {L"chart_with_upwards_trend", 0x1F4C8}, {L"bar_chart", 0x1F4CA}, {L"clipboard", 0x1F4CB}, {L"file_folder", 0x1F4C1},
             {L"art", 0x1F3A8}, {L"musical_note", 0x1F3B5}, {L"sunny", 0x2600}, {L"cloud", 0x2601}, {L"umbrella", 0x2614},
             {L"snowflake", 0x2744}, {L"rainbow", 0x1F308}, {L"coffee", 0x2615}, {L"beer", 0x1F37A}, {L"pizza", 0x1F355},
             {L"cake", 0x1F370}, {L"dog", 0x1F436}, {L"cat", 0x1F431}, {L"penguin", 0x1F427}, {L"whale", 0x1F433},
             {L"seedling", 0x1F331}, {L"skull", 0x1F480}, {L"ghost", 0x1F47B}, {L"robot", 0x1F916}, {L"100", 0x1F4AF},
             {L"recycle", 0x267B}, {L"arrow_right", 0x27A1}, {L"arrow_left", 0x2B05}, {L"arrow_up", 0x2B06},
             {L"arrow_down", 0x2B07}, {L"information_source", 0x2139}, {L"red_circle", 0x1F534}};
    std::wstring l = name;
    for (WCHAR& c : l) c = (WCHAR)towlower(c);
    for (const auto& t : T)
        if (l == t.n) return t.c;
    return 0;
}

// Reference definitions of the document being converted (label -> url); set by MarkdownToRtf.
static const std::unordered_map<std::wstring, std::wstring>* g_mdRefs = nullptr;

// Footnote definitions (label -> text) and the labels in order of first reference; set by MarkdownToRtf.
static const std::unordered_map<std::wstring, std::wstring>* g_mdFoot = nullptr;
static std::vector<std::wstring>* g_mdFootOrder = nullptr;

// Indices of <details> blocks whose open/closed state the user flipped by clicking; set by the preview.
static const std::unordered_set<int>* g_mdOpen = nullptr;

static void MdInline(std::string& o, const std::wstring& s) {
    std::wstring plain;
    auto flush = [&] {
        RtfText(o, plain);
        plain.clear();
    };
    for (size_t i = 0; i < s.size();) {
        WCHAR ch = s[i];
        if (ch == L'\\' && i + 1 < s.size() && MdPunct(s[i + 1])) {
            plain += s[i + 1];
            i += 2;
            continue;
        }
        if (ch == L'`') {
            size_t n = 1;
            while (i + n < s.size() && s[i + n] == L'`') n++;
            size_t e = i + n;
            while ((e = s.find(L'`', e)) != kNpos) {
                size_t m = 1;
                while (e + m < s.size() && s[e + m] == L'`') m++;
                if (m == n) break;
                e += m;
            }
            if (e != kNpos) {
                flush();
                std::wstring c = s.substr(i + n, e - i - n);
                for (WCHAR& x : c)
                    if (x == L'\n') x = L' ';
                if (c.size() >= 2 && c.front() == L' ' && c.back() == L' ' && c.find_first_not_of(L' ') != kNpos) c = c.substr(1, c.size() - 2);
                o += "{\\f1\\cf4 ";
                RtfText(o, c);
                o += "}";
                i = e + n;
                continue;
            }
            plain.append(n, L'`');
            i += n;
            continue;
        }
        if ((ch == L'*' || ch == L'_' || ch == L'~') && i + 1 < s.size() && s[i + 1] == ch) {
            WCHAR d[3] = {ch, ch, 0};
            size_t e = MdFindClose(s, i + 2, d);
            if (e != kNpos) {
                flush();
                o += ch == L'~' ? "{\\strike " : "{\\b ";
                MdInline(o, s.substr(i + 2, e - i - 2));
                o += "}";
                i = e + 2;
                continue;
            }
        }
        if ((ch == L'*' || ch == L'_') && i + 1 < s.size() && s[i + 1] != L' ' &&
            (ch == L'*' || i == 0 || !iswalnum(s[i - 1]))) {
            WCHAR d[2] = {ch, 0};
            size_t e = MdFindClose(s, i + 1, d);
            if (e != kNpos && s[e - 1] != L' ' &&
                (ch == L'*' || e + 1 >= s.size() || !iswalnum(s[e + 1]))) {
                flush();
                o += "{\\i ";
                MdInline(o, s.substr(i + 1, e - i - 1));
                o += "}";
                i = e + 1;
                continue;
            }
        }
        bool img = ch == L'!' && i + 1 < s.size() && s[i + 1] == L'[';
        if (ch == L'[' || img) {
            size_t b = i + (img ? 2 : 1);
            size_t e = s.find(L']', b);
            if (e != kNpos && e + 1 < s.size() && s[e + 1] == L'(') {
                size_t u = s.find(L')', e + 2);
                if (u != kNpos) {
                    flush();
                    if (img) {
                        o += "{\\i\\cf3 [image: ";
                        RtfText(o, s.substr(b, e - b));
                        o += "]}";
                    } else {
                        std::wstring url = PvTrim(s.substr(e + 2, u - e - 2));
                        if (size_t sp = url.find(L' '); sp != kNpos) url.resize(sp);
                        MdLink(o, s.substr(b, e - b), url);
                    }
                    i = u + 1;
                    continue;
                }
            }
        }
        if (ch == L'[' && i + 2 < s.size() && s[i + 1] == L'^' && g_mdFoot && g_mdFootOrder) {
            size_t e = s.find(L']', i + 2);
            if (e != kNpos && e > i + 2) {
                std::wstring lab = MdLabel(s.substr(i + 2, e - i - 2));
                if (g_mdFoot->count(lab)) {
                    size_t idx = 0;
                    while (idx < g_mdFootOrder->size() && (*g_mdFootOrder)[idx] != lab) idx++;
                    if (idx == g_mdFootOrder->size()) g_mdFootOrder->push_back(lab);
                    flush();
                    o += "{\\super\\cf2 " + std::to_string(idx + 1) + "}";
                    i = e + 1;
                    continue;
                }
            }
        }
        if ((ch == L'[' || img) && g_mdRefs) {
            size_t b = i + (img ? 2 : 1);
            size_t e = MdBracketEnd(s, b);
            if (e != kNpos) {
                std::wstring text = s.substr(b, e - b), label = text;
                size_t fin = e + 1;
                if (e + 1 < s.size() && s[e + 1] == L'[') {
                    size_t e2 = s.find(L']', e + 2);
                    if (e2 != kNpos) {
                        if (e2 > e + 2) label = s.substr(e + 2, e2 - e - 2);
                        fin = e2 + 1;
                    }
                }
                auto it = g_mdRefs->find(MdLabel(label));
                if (it != g_mdRefs->end()) {
                    flush();
                    if (img) {
                        o += "{\\i\\cf3 [image: ";
                        RtfText(o, text);
                        o += "]}";
                    } else {
                        MdLink(o, text, it->second);
                    }
                    i = fin;
                    continue;
                }
            }
        }
        if (ch == L'<') {
            size_t e = s.find(L'>', i + 1);
            if (e != kNpos && MdUrlAt(s, i + 1)) {
                flush();
                std::wstring u = s.substr(i + 1, e - i - 1);
                MdLink(o, u, u);
                i = e + 1;
                continue;
            }
        }
        if (ch == L'<') {
            std::wstring nm;
            bool cl = false;
            size_t e = MdTagEnd(s, i, nm, cl);
            if (e) {
                flush();
                size_t ce = 0;
                if (nm == L"!--") {
                } else if (nm == L"br") {
                    o += "\\line ";
                } else if (!cl && (nm == L"script" || nm == L"style")) {
                    if (MdCloseTag(s, e, nm, ce) != kNpos) e = ce;
                } else if (!cl && nm == L"img") {
                    std::wstring alt = MdAttr(s.substr(i, e - i), L"alt");
                    if (!alt.empty()) {
                        o += "{\\i\\cf3 [image: ";
                        RtfText(o, alt);
                        o += "]}";
                    }
                } else if (!cl && nm == L"a") {
                    std::wstring href = MdAttr(s.substr(i, e - i), L"href");
                    size_t cp = MdCloseTag(s, e, nm, ce);
                    if (!href.empty() && cp != kNpos) {
                        MdLink(o, s.substr(e, cp - e), href);
                        e = ce;
                    }
                } else if (!cl && MdTagFmt(nm)) {
                    size_t cp = MdCloseTag(s, e, nm, ce);
                    if (cp != kNpos) {
                        o += std::string("{") + MdTagFmt(nm);
                        MdInline(o, s.substr(e, cp - e));
                        o += "}";
                        e = ce;
                    }
                }
                i = e;
                continue;
            }
        }
        if (ch == L'h' && MdUrlAt(s, i) && (i == 0 || !iswalnum(s[i - 1]))) {
            size_t e = i;
            while (e < s.size() && !iswspace(s[e]) && s[e] != L'<' && s[e] != L'>' && s[e] != L')') e++;
            while (e > i && wcschr(L".,;:!?'\"", s[e - 1])) e--;
            flush();
            std::wstring u = s.substr(i, e - i);
            MdLink(o, u, u);
            i = e;
            continue;
        }
        if (ch == L':' && i + 2 < s.size()) {
            size_t e = i + 1;
            while (e < s.size() && ((s[e] < 128 && iswalnum(s[e])) || s[e] == L'_' || s[e] == L'+' || s[e] == L'-')) e++;
            if (e > i + 1 && e < s.size() && s[e] == L':') {
                if (uint32_t cp = MdEmoji(s.substr(i + 1, e - i - 1))) {
                    std::wstring eg;
                    MdPutCp(eg, cp);
                    flush();
                    o += "{\\f2 ";
                    RtfText(o, eg);
                    o += "}";
                    i = e + 1;
                    continue;
                }
            }
        }
        if (ch == L'&') {
            std::wstring r;
            size_t len = 0;
            if (MdEntity(s, i, r, len)) {
                plain += r;
                i += len;
                continue;
            }
        }
        plain += ch;
        i++;
    }
    flush();
}

static bool MdIsHr(const std::wstring& t) {
    if (t.size() < 3 || (t[0] != L'-' && t[0] != L'*' && t[0] != L'_')) return false;
    int cnt = 0;
    for (WCHAR ch : t) {
        if (ch == t[0])
            cnt++;
        else if (ch != L' ')
            return false;
    }
    return cnt >= 3;
}

static bool MdIsSep(const std::wstring& line) {
    std::wstring t = PvTrim(line);
    if (t.find(L'-') == kNpos || t.find(L'|') == kNpos) return false;
    for (WCHAR ch : t)
        if (ch != L'|' && ch != L'-' && ch != L':' && ch != L' ') return false;
    return true;
}

static std::vector<std::wstring> MdCells(const std::wstring& line) {
    std::wstring t = PvTrim(line);
    if (!t.empty() && t[0] == L'|') t.erase(0, 1);
    if (!t.empty() && t.back() == L'|' && (t.size() < 2 || t[t.size() - 2] != L'\\')) t.pop_back();
    std::vector<std::wstring> c;
    std::wstring cur;
    for (size_t i = 0; i < t.size(); i++) {
        if (t[i] == L'\\' && i + 1 < t.size() && t[i + 1] == L'|') {
            cur += L'|';
            i++;
        } else if (t[i] == L'|') {
            c.push_back(PvTrim(cur));
            cur.clear();
        } else {
            cur += t[i];
        }
    }
    c.push_back(PvTrim(cur));
    return c;
}

// Length of a list marker at `at` (0 if none); ordered markers also return their number.
static size_t MdListMarker(const std::wstring& s, size_t at, bool& ordered, std::wstring& num) {
    if (at + 1 < s.size() && (s[at] == L'-' || s[at] == L'*' || s[at] == L'+') && s[at + 1] == L' ') {
        ordered = false;
        return 2;
    }
    size_t i = at;
    while (i < s.size() && iswdigit(s[i]) && i - at < 9) i++;
    if (i > at && i + 1 < s.size() && (s[i] == L'.' || s[i] == L')') && s[i + 1] == L' ') {
        ordered = true;
        num = s.substr(at, i - at) + L".";
        return i + 2 - at;
    }
    return 0;
}

static std::wstring MdLower(std::wstring s) {
    for (WCHAR& c : s) c = (WCHAR)towlower(c);
    return s;
}

// Number of <details ...> (close = false) or </details> (close = true) tags in a line.
static int MdCountDetails(const std::wstring& line, bool close) {
    std::wstring l = MdLower(line), pat = close ? L"</details" : L"<details";
    int n = 0;
    for (size_t p = 0; (p = l.find(pat, p)) != kNpos; p += pat.size()) {
        size_t k = p + pat.size();
        if (k >= l.size() || l[k] == L' ' || l[k] == L'>' || l[k] == L'\t' || l[k] == L'/') n++;
    }
    return n;
}

static int MdSetext(const std::wstring& t) {
    if (t.empty() || (t[0] != L'=' && t[0] != L'-')) return 0;
    for (WCHAR c : t)
        if (c != t[0]) return 0;
    return t[0] == L'=' ? 1 : 2;
}

// Delimiter row to per-column alignment (0 none/left, 1 center, 2 right); false if it is not a valid row.
static bool MdAligns(const std::wstring& line, std::vector<char>& al) {
    al.clear();
    if (line.find(L'-') == kNpos) return false;
    for (const std::wstring& c : MdCells(line)) {
        size_t a = 0, b = c.size();
        bool l = a < b && c[a] == L':';
        if (l) a++;
        bool r = b > a && c[b - 1] == L':';
        if (r) b--;
        if (b <= a) return false;
        for (size_t k = a; k < b; k++)
            if (c[k] != L'-') return false;
        al.push_back(l && r ? 1 : r ? 2 : 0);
    }
    return !al.empty();
}

static bool MdBs(const std::wstring& s) {
    size_t k = 0;
    while (k < s.size() && s[s.size() - 1 - k] == L'\\') k++;
    return k % 2 == 1;
}

static size_t MdSkipWs(const std::wstring& s, size_t p, bool oneNl) {
    while (p < s.size()) {
        if (s[p] == L' ' || s[p] == L'\t') {
            p++;
        } else if (oneNl && s[p] == L'\n') {
            oneNl = false;
            p++;
        } else {
            break;
        }
    }
    return p;
}

static bool MdTitle(const std::wstring& s, size_t p, size_t& e) {
    WCHAR o = s[p], c = o == L'(' ? L')' : o;
    for (size_t k = p + 1; k < s.size(); k++) {
        if (s[k] == L'\\') {
            k++;
        } else if (s[k] == c) {
            e = k + 1;
            return true;
        } else if (o == L'(' && s[k] == L'(') {
            return false;
        }
    }
    return false;
}

// A link reference definition at line n: returns the number of lines it spans (0 if none).
static size_t MdParseDef(const std::vector<std::wstring>& L, size_t n, std::wstring& label, std::wstring& url) {
    std::wstring s;
    size_t cnt = 0;
    for (size_t k = n; k < L.size() && cnt < 6 && !PvTrim(L[k]).empty(); k++, cnt++) {
        s += L[k];
        s += L'\n';
    }
    size_t i = 0;
    while (i < s.size() && i < 3 && s[i] == L' ') i++;
    if (i >= s.size() || s[i] != L'[') return 0;
    size_t e = MdBracketEnd(s, i + 1);
    if (e == kNpos || e + 1 >= s.size() || s[e + 1] != L':') return 0;
    label = s.substr(i + 1, e - i - 1);
    if (PvTrim(label).empty() || label[0] == L'^') return 0;
    i = MdSkipWs(s, e + 2, true);
    if (i >= s.size()) return 0;
    size_t d = i;
    std::wstring raw;
    if (s[i] == L'<') {
        size_t g = s.find(L'>', i + 1);
        if (g == kNpos || s.find(L'\n', i) < g) return 0;
        raw = s.substr(i + 1, g - i - 1);
        i = g + 1;
    } else {
        int depth = 0;
        while (i < s.size() && s[i] > L' ') {
            if (s[i] == L'\\' && i + 1 < s.size() && s[i + 1] > L' ') {
                i++;
            } else if (s[i] == L'(') {
                depth++;
            } else if (s[i] == L')' && depth-- == 0) {
                return 0;
            }
            i++;
        }
        if (i == d) return 0;
        raw = s.substr(d, i - d);
    }
    url.clear();
    for (size_t k = 0; k < raw.size(); k++) {
        if (raw[k] == L'\\' && k + 1 < raw.size() && MdPunct(raw[k + 1])) k++;
        url += raw[k];
    }
    size_t afterDest = i, endPos = i;
    size_t p0 = MdSkipWs(s, afterDest, false);
    bool sameLine = p0 < s.size() && s[p0] != L'\n';
    size_t t = MdSkipWs(s, afterDest, true);
    if (t < s.size() && t > afterDest && (s[t] == L'"' || s[t] == L'\'' || s[t] == L'(')) {
        size_t te = 0;
        if (MdTitle(s, t, te)) {
            size_t r = MdSkipWs(s, te, false);
            if (r >= s.size() || s[r] == L'\n')
                endPos = te;
            else if (sameLine)
                return 0;
        } else if (sameLine) {
            return 0;
        }
    } else if (sameLine) {
        return 0;
    }
    return (size_t)std::count(s.begin(), s.begin() + endPos, L'\n') + 1;
}

static std::wstring MdUnquote(const std::wstring& s) {
    size_t i = 0;
    for (;;) {
        size_t j = i;
        while (j < s.size() && j - i < 3 && s[j] == L' ') j++;
        if (j < s.size() && s[j] == L'>') {
            j++;
            if (j < s.size() && s[j] == L' ') j++;
            i = j;
        } else {
            break;
        }
    }
    return s.substr(i);
}

static bool MdInterrupts(const std::wstring& raw);

// Language index (into Langs()) for a fence info string, or -1; understands common aliases.
static int MdLang(const std::wstring& info) {
    std::wstring w;
    for (WCHAR c : info) {
        if (iswspace(c) || c == L'{') break;
        w += (WCHAR)towlower(c);
    }
    static const struct {
        const WCHAR* a;
        const WCHAR* ext;
    } A[] = {{L"c++", L"cpp"}, {L"c#", L"cs"}, {L"csharp", L"cs"}, {L"javascript", L"js"}, {L"node", L"js"},
             {L"nodejs", L"js"}, {L"typescript", L"ts"}, {L"python", L"py"}, {L"python3", L"py"}, {L"py3", L"py"},
             {L"json5", L"json"}, {L"hxx", L"hpp"}};
    for (const auto& a : A)
        if (w == a.a) w = a.ext;
    if (w.empty()) return -1;
    auto& langs = Langs();
    for (size_t i = 0; i < langs.size(); i++) {
        const WCHAR* q = langs[i].exts;
        while (*q) {
            while (*q == L' ') q++;
            const WCHAR* e = q;
            while (*e && *e != L' ') e++;
            if ((size_t)(e - q) == w.size() && wcsncmp(q, w.c_str(), w.size()) == 0) return (int)i;
            q = e;
        }
    }
    return -1;
}

// First pass: collects reference and footnote definitions (outside code) and marks the lines they use.
static void MdCollectRefs(const std::vector<std::wstring>& L, std::unordered_map<std::wstring, std::wstring>& refs,
                          std::unordered_map<std::wstring, std::wstring>& foots, std::vector<char>& skip) {
    std::vector<std::wstring> S;
    for (const std::wstring& l : L) S.push_back(MdUnquote(l));
    bool fence = false, para = false;
    WCHAR fc = 0;
    size_t fl = 0;
    for (size_t n = 0; n < S.size(); n++) {
        std::wstring t = PvTrim(S[n]);
        size_t ind = 0;
        int w = 0;
        while (ind < S[n].size() && (S[n][ind] == L' ' || S[n][ind] == L'\t')) w += S[n][ind++] == L'\t' ? 4 : 1;
        if (fence) {
            size_t k = 0;
            while (k < t.size() && t[k] == fc) k++;
            if (k >= fl && k == t.size()) fence = false;
            continue;
        }
        if (t.empty()) {
            para = false;
            continue;
        }
        if (w < 4 && (t[0] == L'`' || t[0] == L'~')) {
            size_t k = 0;
            while (k < t.size() && t[k] == t[0]) k++;
            if (k >= 3 && (t[0] == L'~' || t.find(L'`', k) == kNpos)) {
                fence = true;
                fc = t[0];
                fl = k;
                para = false;
                continue;
            }
        }
        if (!para && w < 4 && t.compare(0, 2, L"[^") == 0) {
            size_t e = t.find(L"]:");
            if (e != kNpos && e > 2 && t.substr(2, e - 2).find_first_of(L" \t[]") == kNpos) {
                std::wstring txt = PvTrim(t.substr(e + 2));
                size_t k = n + 1;
                for (; k < S.size() && !PvTrim(S[k]).empty() && !MdInterrupts(S[k]) && PvTrim(S[k]).compare(0, 2, L"[^") != 0; k++) {
                    txt += L' ';
                    txt += PvTrim(S[k]);
                }
                foots.emplace(MdLabel(t.substr(2, e - 2)), txt);
                for (size_t m = n; m < k; m++) skip[m] = 1;
                n = k - 1;
                continue;
            }
        }
        if (!para && w < 4) {
            std::wstring lab, url;
            if (size_t c = MdParseDef(S, n, lab, url)) {
                refs.emplace(MdLabel(lab), url);
                for (size_t k = 0; k < c; k++) skip[n + k] = 1;
                n += c - 1;
                continue;
            }
        }
        if (w >= 4 && !para) continue;
        para = t[0] != L'#' && !MdIsHr(t);
    }
}

// HTML block start kind (1-7, 0 = none) for a trimmed line; kind 7 cannot interrupt a paragraph.
static int MdHtmlKind(const std::wstring& t, bool allow7) {
    if (t.empty() || t[0] != L'<') return 0;
    std::wstring l = t.substr(0, 12);
    for (WCHAR& c : l) c = (WCHAR)towlower(c);
    auto word = [&](const WCHAR* w) {
        size_t n = wcslen(w);
        return l.compare(0, n, w) == 0 && (l.size() == n || l[n] == L' ' || l[n] == L'\t' || l[n] == L'>');
    };
    if (word(L"<script") || word(L"<pre") || word(L"<style")) return 1;
    if (l.compare(0, 4, L"<!--") == 0) return 2;
    if (l.compare(0, 2, L"<?") == 0) return 3;
    if (l.compare(0, 9, L"<![cdata[") == 0) return 5;
    if (t.size() > 2 && t[1] == L'!' && t[2] >= L'A' && t[2] <= L'Z') return 4;
    static const std::wstring kBlk =
        L" address article aside base basefont blockquote body caption center col colgroup dd details dialog dir div "
        L"dl dt fieldset figcaption figure footer form frame frameset h1 h2 h3 h4 h5 h6 head header hr html iframe "
        L"legend li link main menu menuitem nav noframes ol optgroup option p param section source summary table "
        L"tbody td tfoot th thead title tr track ul ";
    size_t k = t.size() > 1 && t[1] == L'/' ? 2 : 1;
    std::wstring nm;
    while (k < t.size() && t[k] < 128 && iswalnum(t[k])) nm += (WCHAR)towlower(t[k++]);
    if (!nm.empty() && kBlk.find(L" " + nm + L" ") != kNpos &&
        (k >= t.size() || t[k] == L' ' || t[k] == L'\t' || t[k] == L'>' || t.compare(k, 2, L"/>") == 0))
        return 6;
    if (allow7) {
        std::wstring n2;
        bool cl;
        size_t e = MdTagEnd(t, 0, n2, cl);
        if (e && n2 != L"script" && n2 != L"style" && n2 != L"pre" && n2 != L"!--" && PvTrim(t.substr(e)).empty())
            return 7;
    }
    return 0;
}

static bool MdHtmlEnds(int k, const std::wstring& line, size_t from) {
    std::wstring l = line;
    if (k == 1)
        for (WCHAR& c : l) c = (WCHAR)towlower(c);
    if (from > l.size()) from = l.size();
    switch (k) {
        case 1:
            return l.find(L"</script>", from) != kNpos || l.find(L"</pre>", from) != kNpos ||
                   l.find(L"</style>", from) != kNpos;
        case 2:
            return l.find(L"-->", from) != kNpos;
        case 3:
            return l.find(L"?>", from) != kNpos;
        case 4:
            return l.find(L'>', from) != kNpos;
        default:
            return l.find(L"]]>", from) != kNpos;
    }
}

// Text of s without tags and comments; with vis, an <img alt> counts as visible text.
static std::wstring MdStripTags(const std::wstring& s, bool vis) {
    std::wstring r;
    for (size_t i = 0; i < s.size();) {
        std::wstring nm;
        bool cl = false;
        size_t e = s[i] == L'<' ? MdTagEnd(s, i, nm, cl) : 0;
        if (e) {
            if (vis && nm == L"img" && !cl && !MdAttr(s.substr(i, e - i), L"alt").empty()) r += L'x';
            i = e;
        } else {
            r += s[i++];
        }
    }
    return r;
}

// Escapes Markdown specials outside tags so raw HTML block text is shown literally.
static std::wstring MdLiteral(const std::wstring& s) {
    std::wstring r;
    for (size_t i = 0; i < s.size();) {
        std::wstring nm;
        bool cl = false;
        size_t e = s[i] == L'<' ? MdTagEnd(s, i, nm, cl) : 0;
        if (!e && s[i] == L'h' && MdUrlAt(s, i) && (i == 0 || !iswalnum(s[i - 1]))) {
            e = i;
            while (e < s.size() && !iswspace(s[e]) && s[e] != L'<' && s[e] != L'>') e++;
        }
        if (e) {
            r.append(s, i, e - i);
            i = e;
            continue;
        }
        if (s[i] && wcschr(L"\\`*_~[]!|", s[i])) r += L'\\';
        r += s[i++];
    }
    return r;
}

// A line that ends a table or a paragraph: blank, quote, heading, fence, rule, list item or HTML block.
static bool MdInterrupts(const std::wstring& raw) {
    std::wstring t = PvTrim(raw);
    if (t.empty()) return true;
    size_t ind = 0;
    while (ind < raw.size() && raw[ind] == L' ') ind++;
    if (ind >= 4) return false;
    if (t[0] == L'>') return true;
    if (t[0] == L'#') {
        size_t k = 0;
        while (k < t.size() && t[k] == L'#') k++;
        if (k <= 6 && (k == t.size() || t[k] == L' ')) return true;
    }
    if (t.compare(0, 3, L"```") == 0 || t.compare(0, 3, L"~~~") == 0 || MdIsHr(t)) return true;
    bool ord = false;
    std::wstring num;
    if (MdListMarker(raw, ind, ord, num)) return true;
    int hk = MdHtmlKind(t, false);
    return hk >= 1 && hk <= 6;
}

// tw = width of the preview in twips (table column widths), fs = body size in half-points.
static std::string MarkdownToRtf(const std::wstring& doc, const PvPal& pal, int tw, int fs) {
    auto fsz = [&](int pct) { return "\\fs" + std::to_string(std::max(8, fs * pct / 100)); };
    std::vector<std::wstring> Lines;
    {
        std::wstring cur;
        for (size_t i = 0; i < doc.size(); i++) {
            WCHAR ch = doc[i];
            if (ch == L'\r' || ch == L'\n') {
                if (ch == L'\n' && i > 0 && doc[i - 1] == L'\r') continue;
                Lines.push_back(cur);
                cur.clear();
            } else {
                cur += ch;
            }
        }
        Lines.push_back(cur);
    }
    std::unordered_map<std::wstring, std::wstring> refs, foots;
    std::vector<char> Skip(Lines.size(), 0);
    MdCollectRefs(Lines, refs, foots, Skip);
    std::vector<std::wstring> footOrder;
    g_mdRefs = &refs;
    g_mdFoot = &foots;
    g_mdFootOrder = &footOrder;
    std::string o = "{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0 Segoe UI;}{\\f1 Consolas;}{\\f2 Segoe UI Emoji;}{\\f3 Segoe UI Symbol;}}{\\colortbl;";
    for (COLORREF c : {pal.text, pal.link, pal.muted, pal.code, pal.codeBg}) RtfColor(o, c);
    const bool dk = &pal == &kPvDark;
    for (COLORREF c : {dk ? RGB(0x4A, 0x4A, 0x4A) : RGB(0xD0, 0xD7, 0xDE), dk ? RGB(0x44, 0x93, 0xF8) : RGB(0x09, 0x69, 0xDA),
                       dk ? RGB(0x3F, 0xB9, 0x50) : RGB(0x1A, 0x7F, 0x37), dk ? RGB(0xAB, 0x7D, 0xF8) : RGB(0x82, 0x50, 0xDF),
                       dk ? RGB(0xD2, 0x99, 0x22) : RGB(0x9A, 0x67, 0x00), dk ? RGB(0xF8, 0x51, 0x49) : RGB(0xD1, 0x24, 0x2F),
                       dk ? RGB(0x2F, 0x2F, 0x2F) : RGB(0xF0, 0xF0, 0xF0)})
        RtfColor(o, c);
    for (int k = 1; k < kcCount; k++) {
        D2D1_COLOR_F c = SynColor(dk, k);
        RtfColor(o, RGB((int)(c.r * 255 + 0.5f), (int)(c.g * 255 + 0.5f), (int)(c.b * 255 + 0.5f)));
    }
    RtfColor(o, pal.bg);
    o += "}\\f0" + fsz(100) + "\\cf1 ";

    const int em = fs * 10;
    const std::string bsl = "\\sl360\\slmult1", lsl = "\\sl312\\slmult1", csl = "\\sl300\\slmult1";
    auto ns = [](int v) { return std::to_string(v); };
    const std::string nb = "\\brdrs\\brdrw15\\brdrcf" + std::to_string(12 + (int)kcCount);
    auto inl = [&](const std::wstring& s) {
        std::string r;
        MdInline(r, s);
        return r;
    };
    bool cellMode = false;
    int detailsN = 0;
    int baseLi = 0;
    std::string curCf = "\\cf1";
    auto pd = [&] { return std::string(cellMode ? "\\pard\\intbl\\li" : "\\pard\\li") + ns(baseLi); };
    auto sp = [&](int twips) {
        o += pd() + "\\sa0\\sl0\\f0\\b0" + curCf + "\\highlight0\\fs" + ns(std::max(2, twips / 12)) + "\\par" + fsz(100) + "\n";
    };
    auto oneCell = [&](char side, const std::string& spec, int gaph, bool shade, const std::vector<std::string>& ps) {
        o += "\\trowd\\trgaph" + ns(gaph) + "\\trleft0";
        for (char sd : {'t', 'l', 'b', 'r'}) o += std::string("\\clbrdr") + sd + (sd == side ? spec : nb);
        o += std::string(shade ? "\\clcbpat5" : "") + "\\cellx" + ns(tw) + "\n";
        for (size_t k = 0; k < ps.size(); k++) o += ps[k] + (k + 1 < ps.size() ? "\\par\n" : "\\cell");
        o += "\\row\n";
        sp(40);
    };
    auto rule = [&] {
        if (cellMode) {
            o += pd() + bsl + "\\sa" + ns(em / 2) + " {\\f1\\cf6 ";
            for (int k = 0; k < 16; k++) o += "\\u9472?";
            o += "}\\par\n";
            return;
        }
        oneCell('b', "\\brdrs\\brdrw15\\brdrcf6", 0, false, {"\\pard\\intbl\\sa0\\sb0\\sl-15\\slmult0\\fs2 "});
    };
    static const int kHsz[] = {200, 150, 125, 100, 88, 85};
    auto heading = [&](size_t lvl, const std::wstring& h) {
        o += pd() + "\\sb" + ns(em * 14 / 10) + "\\sa" + ns(lvl <= 2 ? em / 6 : em / 2) + "\\sl300\\slmult1\\keepn\\b" + fsz(kHsz[lvl - 1]) + (lvl == 6 ? "\\cf3 " : " ") + inl(h) + "\\b0" + fsz(100) + (lvl == 6 ? curCf : std::string()) + "\\par\n";
        if (lvl <= 2 && !cellMode) {
            rule();
            sp(em * 3 / 10);
        }
    };
    // Per-line RTF for code text, colored with the mod's own tokenizer when the language is known.
    auto codeText = [&](const std::vector<std::wstring>& ls, int lang, bool& hl) {
        std::vector<std::string> out;
        std::vector<uint8_t> cls;
        size_t total = 0;
        for (const auto& l : ls) total += l.size() + 1;
        hl = lang >= 0 && total <= 20000;
        if (hl) {
            std::wstring all;
            for (const auto& l : ls) {
                all += l;
                all += L'\n';
            }
            Tokenize(Langs()[lang], all, cls);
        }
        size_t pos = 0;
        for (const auto& l : ls) {
            std::string r;
            if (!hl) {
                RtfText(r, l);
            } else {
                for (size_t i = 0; i < l.size();) {
                    uint8_t k = cls[pos + i];
                    size_t j = i + 1;
                    while (j < l.size() && cls[pos + j] == k) j++;
                    if (k == kcDefault) {
                        RtfText(r, l.substr(i, j - i));
                    } else {
                        r += "{\\cf" + ns(12 + k) + " ";
                        RtfText(r, l.substr(i, j - i));
                        r += "}";
                    }
                    i = j;
                }
            }
            pos += l.size() + 1;
            out.push_back(r);
        }
        return out;
    };
    auto codeBlock = [&](const std::vector<std::wstring>& ls, int lang) {
        bool hl = false;
        std::vector<std::string> txt = codeText(ls, lang, hl);
        const std::string cf = hl ? "\\cf1" : "\\cf4";
        if (cellMode) {
            for (const auto& r : txt) o += pd() + csl + "\\sa0 {\\f1" + fsz(90) + cf + "\\highlight5 " + r + "}\\par\n";
            sp(em * 4 / 10);
            return;
        }
        const int pad = em / 2;
        std::vector<std::string> ps;
        for (size_t k = 0; k < txt.size() || k == 0; k++) {
            std::string p = "\\pard\\intbl" + csl + "\\sb" + ns(k == 0 ? pad : 0) + "\\sa" + ns(k + 1 >= txt.size() ? pad : 0) + "\\f1" + fsz(90) + cf + " ";
            if (k < txt.size()) p += txt[k];
            ps.push_back(p);
        }
        oneCell(0, "", 160, true, ps);
        sp(em * 4 / 10);
    };
    auto emitTable = [&](const std::vector<std::vector<std::wstring>>& rows, const std::vector<char>& al, bool vert) {
        size_t cols = al.size();
        if (cellMode) {
            for (size_t r = 0; r < rows.size(); r++) {
                std::string line;
                for (size_t k = 0; k < cols; k++) {
                    if (k) line += "  \\u9474?  ";
                    if (k < rows[r].size()) line += (r == 0 && !vert) || (vert && k == 0) ? "{\\b " + inl(rows[r][k]) + "}" : inl(rows[r][k]);
                }
                o += pd() + lsl + "\\sa" + ns(em / 5) + " " + line + "\\par\n";
            }
            sp(em * 4 / 10);
            return;
        }
        auto vlen = [](const std::wstring& c) {
            int n = 0;
            for (size_t i = 0; i < c.size(); i++) {
                if (c[i] == L']' && i + 1 < c.size() && c[i + 1] == L'(') {
                    size_t e = c.find(L')', i);
                    if (e == kNpos) break;
                    i = e;
                    continue;
                }
                if (c[i] != L'*' && c[i] != L'`' && c[i] != L'[') n++;
            }
            return n;
        };
        std::vector<int> wt(cols, 6);
        int sum = 0;
        for (const auto& r : rows)
            for (size_t k = 0; k < cols && k < r.size(); k++) wt[k] = std::max(wt[k], std::min(vlen(r[k]), 40));
        for (int v : wt) sum += v;
        std::vector<int> xs(cols);
        int x = 0;
        for (size_t k = 0; k < cols; k++) {
            x += std::max(tw / 12, (int)((long long)tw * wt[k] / sum));
            xs[k] = k + 1 == cols ? tw : std::min(x, tw);
        }
        for (size_t r = 0; r < rows.size(); r++) {
            bool head = r == 0 && !vert;
            o += "\\trowd\\trgaph80\\trleft0";
            for (size_t k = 0; k < cols; k++) {
                for (const char* side : {"t", "l", "b", "r"})
                    o += std::string("\\clbrdr") + side + "\\brdrs\\brdrw10\\brdrcf3";
                if (head || (vert && k == 0))
                    o += "\\clcbpat5";
                else if (!vert && r % 2 == 0)
                    o += "\\clcbpat12";
                o += "\\cellx" + ns(xs[k]);
            }
            for (size_t k = 0; k < cols; k++) {
                o += "\\pard\\intbl" + lsl + "\\sb40\\sa40" + (al[k] == 1 ? "\\qc" : al[k] == 2 ? "\\qr" : "\\ql") + (head || (vert && k == 0) ? "\\b " : " ");
                if (k < rows[r].size()) MdInline(o, rows[r][k]);
                o += head || (vert && k == 0) ? "\\b0\\cell" : "\\cell";
            }
            o += "\\row\n";
        }
        sp(em * 6 / 10);
    };
    auto strip4 = [](const std::wstring& r) {
        size_t k = 0;
        int cols = 0;
        while (k < r.size() && cols < 4 && (r[k] == L' ' || r[k] == L'\t')) cols += r[k++] == L'\t' ? 4 : 1;
        return r.substr(k);
    };

    std::function<void(const std::vector<std::wstring>&, const std::vector<char>&)> run;
    // A block quote: each run of lines at one depth becomes a row with one 2-pixel bar cell per level and a text cell.
    auto quoteBlock = [&](std::vector<std::wstring> ql, std::vector<char> qs, std::vector<int> qd) {
        int al = -1;
        if (!ql.empty() && qd[0] == 1) {
            std::wstring t0 = PvTrim(ql[0]);
            size_t e = t0.find(L']');
            if (t0.compare(0, 2, L"[!") == 0 && e != kNpos && (e + 1 == t0.size() || iswspace(t0[e + 1]))) {
                std::wstring ty = t0.substr(2, e - 2);
                for (WCHAR& c : ty) c = (WCHAR)towupper(c);
                static const WCHAR* kTy[] = {L"NOTE", L"TIP", L"IMPORTANT", L"WARNING", L"CAUTION"};
                for (int k = 0; k < 5; k++)
                    if (ty == kTy[k]) al = k;
                if (al >= 0) {
                    std::wstring rest = PvTrim(t0.substr(e + 1));
                    if (rest.empty()) {
                        ql.erase(ql.begin());
                        qs.erase(qs.begin());
                        qd.erase(qd.begin());
                    } else {
                        ql[0] = rest;
                    }
                }
            }
        }
        bool any = false;
        for (const auto& l : ql)
            if (!PvTrim(l).empty()) any = true;
        if (!any && al < 0) return;
        if (al >= 0 && ql.empty()) {
            ql.push_back(L"");
            qs.push_back(0);
            qd.push_back(1);
        }
        const bool wasCell = cellMode;
        const int oldLi = baseLi;
        const std::string oldCf = curCf;
        std::vector<std::pair<int, std::string>> rows;
        for (size_t i = 0; i < ql.size();) {
            size_t j = i;
            while (j < ql.size() && qd[j] == qd[i]) j++;
            std::vector<std::wstring> sl(ql.begin() + i, ql.begin() + j);
            std::vector<char> ss(qs.begin() + i, qs.begin() + j);
            std::string saved;
            saved.swap(o);
            cellMode = true;
            baseLi = wasCell ? oldLi + 360 : 0;
            curCf = al >= 0 ? "\\cf1" : "\\cf3";
            if (rows.empty() && al >= 0) {
                static const char* kLbl[] = {"Note", "Tip", "Important", "Warning", "Caution"};
                o += pd() + bsl + "\\sa" + ns(em / 4) + " {\\b\\cf" + ns(7 + al) + " " + kLbl[al] + "}\\par\n";
            }
            run(sl, ss);
            std::string body;
            body.swap(o);
            o.swap(saved);
            cellMode = wasCell;
            baseLi = oldLi;
            curCf = oldCf;
            if (!body.empty()) rows.push_back({qd[i], body});
            i = j;
        }
        if (wasCell) {
            for (const auto& r : rows) o += r.second;
            return;
        }
        for (size_t ri = 0; ri < rows.size(); ri++) {
            std::string body = rows[ri].second;
            if (ri + 1 == rows.size()) {
                size_t lp = body.rfind("\\pard");
                size_t sa = lp == std::string::npos ? std::string::npos : body.find("\\sa", lp);
                if (sa != std::string::npos) {
                    size_t e = sa + 3;
                    while (e < body.size() && isdigit((unsigned char)body[e])) e++;
                    body.replace(sa, e - sa, "\\sa0");
                }
            }
            size_t last = body.rfind("\\par");
            if (last != std::string::npos && body.compare(last, 5, "\\pard") != 0) body.replace(last, 4, "\\cell");
            const int bd = std::min(rows[ri].first, 6);
            o += "\\trowd\\trgaph0\\trleft0";
            for (int k = 0; k < bd; k++)
                o += "\\clbrdrt" + nb + "\\clbrdrl\\brdrs\\brdrw30\\brdrcf" + ns(k == 0 && al >= 0 ? 7 + al : 3) + "\\clbrdrb" + nb + "\\clbrdrr" + nb + "\\cellx" + ns(190 * (k + 1));
            o += "\\clbrdrt" + nb + "\\clbrdrl" + nb + "\\clbrdrb" + nb + "\\clbrdrr" + nb + "\\cellx" + ns(std::max(tw, 190 * bd + 600)) + "\n";
            for (int k = 0; k < bd; k++) o += "\\pard\\intbl\\sa0\\cell";
            o += (al >= 0 ? std::string("\\cf1") : std::string("\\cf3")) + " " + body + "\\row\n";
        }
        sp(em * 8 / 10);
    };

    run = [&](const std::vector<std::wstring>& L, const std::vector<char>& skip) {
        std::wstring para, bullet;
        int pk = 0, plvl = 0, hb = 0;
        std::vector<int> stack;
        auto flush = [&] {
            if (!para.empty()) {
                if (pk == 1) {
                    o += pd() + "\\li" + ns(baseLi + 360 + (plvl + 1) * 360) + "\\fi-300" + lsl + "\\sa" + ns(em / 5) + " ";
                    RtfText(o, bullet);
                    o += "\\tab ";
                } else {
                    o += pd() + bsl + "\\sa" + ns(em * 8 / 10) + " ";
                }
                MdInline(o, para);
                o += "\\par\n";
            }
            para.clear();
            pk = 0;
            hb = 0;
        };
        bool fence = false, icode = false;
        WCHAR fch = 0;
        size_t flen = 0, fin = 0, pend = 0;
        int lang = -1;
        std::vector<std::wstring> cb;
        for (size_t n = 0; n < L.size(); n++) {
            if (skip[n]) continue;
            const std::wstring& raw = L[n];
            std::wstring t = PvTrim(raw);
            size_t ind = 0;
            int w = 0;
            while (ind < raw.size() && (raw[ind] == L' ' || raw[ind] == L'\t')) w += raw[ind++] == L'\t' ? 4 : 1;
            if (fence) {
                size_t k = 0;
                while (k < t.size() && t[k] == fch) k++;
                if (k >= flen && k == t.size()) {
                    fence = false;
                    codeBlock(cb, lang);
                    cb.clear();
                } else {
                    size_t s0 = 0;
                    while (s0 < fin && s0 < raw.size() && raw[s0] == L' ') s0++;
                    cb.push_back(raw.substr(s0));
                }
                continue;
            }
            if (icode) {
                if (t.empty()) {
                    pend++;
                    continue;
                }
                if (w >= 4) {
                    for (; pend; pend--) cb.push_back(L"");
                    cb.push_back(strip4(raw));
                    continue;
                }
                icode = false;
                pend = 0;
                codeBlock(cb, -1);
                cb.clear();
            }
            if (!t.empty() && (w < 4 || !stack.empty()) && (t[0] == L'`' || t[0] == L'~')) {
                size_t k = 0;
                while (k < t.size() && t[k] == t[0]) k++;
                if (k >= 3 && (t[0] == L'~' || t.find(L'`', k) == kNpos)) {
                    flush();
                    fence = true;
                    fch = t[0];
                    flen = k;
                    fin = ind;
                    lang = MdLang(PvTrim(t.substr(k)));
                    cb.clear();
                    continue;
                }
            }
            if (t.empty()) {
                bool wasList = pk == 1 && !para.empty();
                flush();
                if (wasList) sp(em * 7 / 10);
                continue;
            }
            if ((w < 4 || !stack.empty()) && t.size() >= 8 && t[0] == L'<' && _wcsnicmp(t.c_str(), L"<details", 8) == 0 &&
                (t.size() == 8 || t[8] == L' ' || t[8] == L'>' || t[8] == L'\t')) {
                flush();
                if (w == 0) stack.clear();
                int depth = 0;
                size_t e = n;
                for (; e < L.size(); e++) {
                    depth += MdCountDetails(L[e], false) - MdCountDetails(L[e], true);
                    if (depth <= 0) break;
                }
                if (e >= L.size()) e = L.size() - 1;
                std::vector<std::wstring> B(L.begin() + n, L.begin() + e + 1);
                std::vector<char> BS(skip.begin() + n, skip.begin() + e + 1);
                bool attrOpen = false;
                {
                    std::wstring lo = MdLower(B[0]);
                    size_t p = lo.find(L"<details"), q = lo.find(L'>', p);
                    if (q != kNpos) {
                        std::wstring tag = lo.substr(p, q - p);
                        size_t po = tag.find(L" open");
                        attrOpen = po != kNpos && (po + 5 >= tag.size() || tag[po + 5] == L' ' || tag[po + 5] == L'=' || tag[po + 5] == L'/');
                        B[0].erase(p, q - p + 1);
                    }
                }
                {
                    std::wstring lo = MdLower(B.back());
                    size_t p = lo.rfind(L"</details"), q = p == kNpos ? kNpos : lo.find(L'>', p);
                    if (q != kNpos) B.back().erase(p, q - p + 1);
                }
                std::wstring sum = L"Details";
                for (auto& ln : B) {
                    std::wstring lo = MdLower(ln);
                    size_t p = lo.find(L"<summary"), q = p == kNpos ? kNpos : lo.find(L'>', p);
                    size_t c = q == kNpos ? kNpos : lo.find(L"</summary>", q);
                    if (c != kNpos) {
                        sum = PvTrim(ln.substr(q + 1, c - q - 1));
                        ln.erase(p, c + 10 - p);
                        break;
                    }
                }
                const int idx = detailsN++;
                const bool open = attrOpen != (g_mdOpen && g_mdOpen->count(idx) > 0);
                o += pd() + bsl + "\\sa" + ns(open ? em / 4 : em * 8 / 10) + " {\\v\\u57347?}{\\f3" + fsz(125) + "\\cf3 " + (open ? "\\u9660?" : "\\u9654?") +
                     "\\u65038?} " + inl(sum) + "{\\v\\u57348?" + ns(idx) + "\\u57349?}\\par\n";
                if (open) run(B, BS);
                n = e;
                continue;
            }
            if ((w < 4 || !stack.empty()) && t[0] == L'<') {
                int hk = MdHtmlKind(t, para.empty());
                if (hk) {
                    flush();
                    if (w == 0) stack.clear();
                    size_t e = n, lt = raw.find(L'<');
                    if (hk <= 5) {
                        while (e < L.size() && !MdHtmlEnds(hk, L[e], e == n ? lt + 2 : 0)) e++;
                        if (e >= L.size()) e = L.size() - 1;
                    } else {
                        while (e + 1 < L.size() && !PvTrim(L[e + 1]).empty()) e++;
                    }
                    if (hk == 1 && t.compare(0, 4, L"<pre") == 0) {
                        std::vector<std::wstring> cl;
                        for (size_t k = n; k <= e; k++) cl.push_back(MdStripTags(L[k], false));
                        codeBlock(cl, -1);
                    } else if (hk >= 6) {
                        std::vector<int> as;
                        for (size_t k = n; k <= e; k++) {
                            const std::wstring& ln = L[k];
                            int lineAl = as.empty() ? 0 : as.back(), hN = 0;
                            for (size_t i = 0; i < ln.size();) {
                                std::wstring nm;
                                bool cl = false;
                                size_t te = ln[i] == L'<' ? MdTagEnd(ln, i, nm, cl) : 0;
                                if (!te) {
                                    i++;
                                    continue;
                                }
                                static const std::wstring kCont =
                                    L" div p center section article header footer main nav aside blockquote h1 h2 h3 h4 h5 h6 ";
                                if (kCont.find(L" " + nm + L" ") != kNpos) {
                                    if (cl) {
                                        if (!as.empty()) as.pop_back();
                                    } else {
                                        std::wstring a = MdLower(MdAttr(ln.substr(i, te - i), L"align"));
                                        int v = nm == L"center" || a == L"center" ? 1 : a == L"right" ? 2 : a == L"left" ? 0 : (as.empty() ? 0 : as.back());
                                        as.push_back(v);
                                        lineAl = v;
                                        if (nm.size() == 2 && nm[0] == L'h' && hN == 0) hN = nm[1] - L'0';
                                    }
                                }
                                i = te;
                            }
                            if (PvTrim(MdStripTags(ln, true)).empty()) continue;
                            const std::string qa = lineAl == 1 ? "\\qc" : lineAl == 2 ? "\\qr" : "";
                            if (hN >= 1 && hN <= 6)
                                o += pd() + qa + "\\sb" + ns(em * 14 / 10) + "\\sa" + ns(em / 2) + "\\sl300\\slmult1\\b" + fsz(kHsz[hN - 1]) + " " +
                                     inl(MdLiteral(PvTrim(ln))) + "\\b0" + fsz(100) + "\\par\n";
                            else
                                o += pd() + qa + bsl + "\\sa" + ns(em / 2) + " " + inl(MdLiteral(PvTrim(ln))) + "\\par\n";
                        }
                    }
                    n = e;
                    continue;
                }
            }
            if (w < 4 && pk == 0 && !para.empty()) {
                if (int lv = MdSetext(t)) {
                    stack.clear();
                    heading((size_t)lv, PvTrim(para));
                    para.clear();
                    hb = 0;
                    continue;
                }
            }

            if (t[0] == L'#') {
                size_t k = 0;
                while (k < t.size() && t[k] == L'#') k++;
                if (k <= 6 && (k == t.size() || t[k] == L' ')) {
                    flush();
                    stack.clear();
                    std::wstring h = PvTrim(t.substr(k));
                    while (!h.empty() && h.back() == L'#') h.pop_back();
                    heading(k, PvTrim(h));
                    continue;
                }
            }
            if (MdIsHr(t)) {
                flush();
                stack.clear();
                sp(em * 4 / 10);
                rule();
                sp(em * 4 / 10);
                continue;
            }
            std::vector<char> al;
            if (t.find(L'|') != kNpos && n + 1 < L.size() && MdIsSep(L[n + 1]) && MdAligns(L[n + 1], al) && MdCells(raw).size() == al.size()) {
                flush();
                stack.clear();
                std::vector<std::vector<std::wstring>> rows{MdCells(raw)};
                n += 2;
                while (n < L.size() && !MdInterrupts(L[n])) rows.push_back(MdCells(L[n++]));
                n--;
                emitTable(rows, al, false);
                continue;
            }
            if (t[0] == L'>') {
                flush();
                stack.clear();
                std::vector<std::wstring> ql;
                std::vector<char> qs;
                std::vector<int> qd;
                bool lastPara = false;
                int ld = 1;
                size_t k = n;
                for (; k < L.size(); k++) {
                    std::wstring cur = L[k];
                    int d = 0;
                    bool firstMark = true;
                    for (;;) {
                        size_t j = 0;
                        while (j < cur.size() && (firstMark ? (cur[j] == L' ' || cur[j] == L'\t') : (j < 3 && cur[j] == L' '))) j++;
                        if (j < cur.size() && cur[j] == L'>') {
                            j++;
                            if (j < cur.size() && cur[j] == L' ') j++;
                            cur = cur.substr(j);
                            d++;
                            firstMark = false;
                        } else {
                            break;
                        }
                    }
                    if (d > 0) {
                        ql.push_back(cur);
                        qs.push_back(skip[k]);
                        qd.push_back(std::min(d, 6));
                        ld = std::min(d, 6);
                        std::wstring c = PvTrim(cur);
                        lastPara = !c.empty() && c[0] != L'#' && c[0] != L'`' && c[0] != L'~' && !MdIsHr(c);
                    } else {
                        std::wstring tt = PvTrim(L[k]);
                        if (!tt.empty() && lastPara && !MdInterrupts(L[k])) {
                            ql.push_back(tt);
                            qs.push_back(skip[k]);
                            qd.push_back(ld);
                        } else {
                            break;
                        }
                    }
                }
                n = k - 1;
                quoteBlock(ql, qs, qd);
                continue;
            }
            bool ord = false;
            std::wstring num;
            if (size_t mk = MdListMarker(raw, ind, ord, num)) {
                flush();
                while (!stack.empty() && w < stack.back()) stack.pop_back();
                if (stack.empty() || w > stack.back()) stack.push_back(w);
                plvl = (int)stack.size() - 1;
                bullet = ord ? num : (plvl % 2 ? L"\u25E6" : L"\u2022");
                para = PvTrim(raw.substr(ind + mk));
                if (para.size() >= 4 && para[0] == L'[' && (para[1] == L' ' || para[1] == L'x' || para[1] == L'X') && para[2] == L']' &&
                    (para[3] == L' ' || para[3] == L'\t')) {
                    bullet = para[1] == L' ' ? L"\u2610" : L"\u2611";
                    para = PvTrim(para.substr(4));
                }
                pk = 1;
                hb = raw.size() >= 2 && raw.compare(raw.size() - 2, 2, L"  ") == 0;
                if (!hb && MdBs(raw)) hb = 2;
                continue;
            }
            if (para.empty() && stack.empty() && w >= 4) {
                icode = true;
                pend = 0;
                cb.clear();
                cb.push_back(strip4(raw));
                continue;
            }
            if (!para.empty()) {
                if (hb == 2 && para.back() == L'\\') para.pop_back();
                para += hb ? L"\n" : L" ";
                para += t;
            } else {
                if (w == 0) stack.clear();
                flush();
                para = t;
            }
            hb = raw.size() >= 2 && raw.compare(raw.size() - 2, 2, L"  ") == 0;
            if (!hb && MdBs(raw)) hb = 2;
        }
        if (fence || icode) codeBlock(cb, fence ? lang : -1);
        flush();
    };

    size_t n0 = 0;
    if (!Lines.empty() && PvTrim(Lines[0]) == L"---") {
        size_t e = 1;
        while (e < Lines.size() && e < 80 && PvTrim(Lines[e]) != L"---") e++;
        if (e < Lines.size() && e < 80) {
            std::vector<std::vector<std::wstring>> kv;
            bool ok = e > 1 && e - 1 <= 12;
            for (size_t k = 1; k < e && ok; k++) {
                const std::wstring& ln = Lines[k];
                if (PvTrim(ln).empty()) continue;
                size_t c = ln.find(L':');
                if (c == 0 || c == kNpos) {
                    ok = false;
                    break;
                }
                std::wstring key = ln.substr(0, c), val = PvTrim(ln.substr(c + 1));
                for (WCHAR ch : key)
                    if (!iswalnum(ch) && ch != L'_' && ch != L'-' && ch != L'.') ok = false;
                if (c + 1 < ln.size() && ln[c + 1] != L' ') ok = false;
                if (val == L"|" || val == L">" || val == L"|-" || val == L">-" || (!val.empty() && wcschr(L"&*!%@`", val[0]))) ok = false;
                if (val.size() >= 2 && (val[0] == L'"' || val[0] == L'\'') && val.back() == val[0]) val = val.substr(1, val.size() - 2);
                kv.push_back({key, val});
            }
            if (ok && !kv.empty()) {
                emitTable(kv, std::vector<char>(2, 0), true);
            } else {
                std::vector<std::wstring> cl(Lines.begin() + 1, Lines.begin() + e);
                codeBlock(cl, -1);
            }
            n0 = e + 1;
        }
    }
    {
        std::vector<std::wstring> rest(Lines.begin() + n0, Lines.end());
        std::vector<char> rs(Skip.begin() + n0, Skip.end());
        run(rest, rs);
    }
    if (!footOrder.empty()) {
        rule();
        sp(em * 3 / 10);
        for (size_t k = 0; k < footOrder.size() && k < 200; k++) {
            auto it = foots.find(footOrder[k]);
            if (it == foots.end()) continue;
            o += pd() + "\\li360\\fi-360" + lsl + "\\sa" + ns(em / 5) + fsz(90) + " {\\cf2 " + ns((int)k + 1) + ".}\\tab ";
            MdInline(o, it->second);
            o += "\\par" + fsz(100) + "\n";
        }
    }
    g_mdRefs = nullptr;
    g_mdFoot = nullptr;
    g_mdFootOrder = nullptr;
    o += "}";
    return o;
}

static WCHAR DetectDelim(const std::wstring& s) {
    size_t cm = 0, sc = 0, tb = 0;
    bool q = false;
    for (WCHAR ch : s) {
        if (ch == L'\r' || ch == L'\n') break;
        if (ch == L'"')
            q = !q;
        else if (!q) {
            if (ch == L',')
                cm++;
            else if (ch == L';')
                sc++;
            else if (ch == L'\t')
                tb++;
        }
    }
    return tb > cm && tb > sc ? L'\t' : (sc > cm ? L';' : L',');
}

static void ParseCsv(const std::wstring& s, WCHAR delim, std::vector<std::vector<std::wstring>>& rows) {
    std::vector<std::wstring> row;
    std::wstring f;
    bool q = false;
    auto endRow = [&] {
        row.push_back(f);
        f.clear();
        if (!(row.size() == 1 && row[0].empty())) rows.push_back(row);
        row.clear();
    };
    for (size_t i = 0; i < s.size(); i++) {
        WCHAR ch = s[i];
        if (q) {
            if (ch == L'"') {
                if (i + 1 < s.size() && s[i + 1] == L'"') {
                    f += L'"';
                    i++;
                } else {
                    q = false;
                }
            } else {
                f += (ch == L'\r' || ch == L'\n') ? L' ' : ch;
            }
        } else if (ch == L'"' && f.empty()) {
            q = true;
        } else if (ch == delim) {
            row.push_back(f);
            f.clear();
        } else if (ch == L'\r' || ch == L'\n') {
            if (ch == L'\n' && i > 0 && s[i - 1] == L'\r') continue;
            endRow();
        } else {
            f += ch;
        }
    }
    if (!f.empty() || !row.empty()) endRow();
}

struct JP {
    const std::wstring& s;
    std::vector<PvNode>& n;
    size_t i = 0;
    int depth = 0;
    bool err = false;
};

static void JWs(JP& p) {
    while (p.i < p.s.size() && (iswspace(p.s[p.i]) || p.s[p.i] == 0xFEFF)) p.i++;
}

static bool JString(JP& p, std::wstring& o) {
    p.i++;
    while (p.i < p.s.size()) {
        WCHAR ch = p.s[p.i++];
        if (ch == L'"') return true;
        if (ch == L'\\' && p.i < p.s.size()) {
            WCHAR e = p.s[p.i++];
            if (e == L'n' || e == L'r' || e == L't') {
                o += L' ';
            } else if (e == L'u') {
                if (p.i + 4 <= p.s.size()) {
                    o += (WCHAR)wcstoul(p.s.substr(p.i, 4).c_str(), nullptr, 16);
                    p.i += 4;
                }
            } else if (e != L'b' && e != L'f') {
                o += e;
            }
        } else {
            o += (ch == L'\r' || ch == L'\n') ? L' ' : ch;
        }
    }
    return false;
}

static int JValue(JP& p, const std::wstring& label) {
    JWs(p);
    if (p.i >= p.s.size() || p.depth > 100) {
        p.err = true;
        return -1;
    }
    int me = (int)p.n.size();
    p.n.emplace_back();
    std::wstring pre = label.empty() ? L"" : label + L": ";
    WCHAR ch = p.s[p.i];
    if (ch == L'{' || ch == L'[') {
        bool obj = ch == L'{';
        WCHAR close = obj ? L'}' : L']';
        p.i++;
        p.depth++;
        JWs(p);
        if (p.i < p.s.size() && p.s[p.i] == close) {
            p.i++;
        } else {
            for (;;) {
                JWs(p);
                std::wstring key;
                if (obj) {
                    if (p.i >= p.s.size() || p.s[p.i] != L'"' || !JString(p, key)) {
                        p.err = true;
                        return -1;
                    }
                    JWs(p);
                    if (p.i >= p.s.size() || p.s[p.i] != L':') {
                        p.err = true;
                        return -1;
                    }
                    p.i++;
                } else {
                    key = L"[" + std::to_wstring(p.n[(size_t)me].kids.size()) + L"]";
                }
                int k = JValue(p, key);
                if (k < 0) return -1;
                p.n[(size_t)me].kids.push_back(k);
                JWs(p);
                if (p.i < p.s.size() && p.s[p.i] == L',') {
                    p.i++;
                    continue;
                }
                if (p.i < p.s.size() && p.s[p.i] == close) {
                    p.i++;
                    break;
                }
                p.err = true;
                return -1;
            }
        }
        p.depth--;
        p.n[(size_t)me].text = label + (label.empty() ? L"" : L"  ") + (obj ? L"{" : L"[") +
                               std::to_wstring(p.n[(size_t)me].kids.size()) + (obj ? L"}" : L"]");
    } else if (ch == L'"') {
        std::wstring v;
        if (!JString(p, v)) {
            p.err = true;
            return -1;
        }
        if (v.size() > 400) v.resize(400), v += L"\u2026";
        p.n[(size_t)me].text = pre + L"\"" + v + L"\"";
    } else {
        size_t j = p.i;
        while (j < p.s.size() && !iswspace(p.s[j]) && p.s[j] != L',' && p.s[j] != L'}' && p.s[j] != L']') j++;
        std::wstring tok = p.s.substr(p.i, j - p.i);
        if (tok.empty() || !(tok == L"true" || tok == L"false" || tok == L"null" || tok[0] == L'-' || iswdigit(tok[0]))) {
            p.err = true;
            return -1;
        }
        p.i = j;
        p.n[(size_t)me].text = pre + tok;
    }
    return me;
}

static void ParseJson(const std::wstring& s, std::vector<PvNode>& n, std::vector<int>& roots) {
    JP p{s, n};
    int r = JValue(p, L"(root)");
    if (r >= 0) {
        JWs(p);
        if (p.i < s.size()) p.err = true;
    }
    if (p.err || r < 0) {
        bool blank = true;
        for (WCHAR ch : s)
            if (!iswspace(ch) && ch != 0xFEFF) blank = false;
        n.clear();
        if (blank) return;
        n.emplace_back();
        n[0].text = L"JSON error near line " + std::to_wstring(PvLine(s, p.i));
        roots.push_back(0);
        return;
    }
    roots.push_back(r);
}

static std::wstring XmlDecode(const std::wstring& s) {
    std::wstring o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == L'&') {
            size_t e = s.find(L';', i);
            if (e != kNpos && e - i <= 10) {
                std::wstring en = s.substr(i + 1, e - i - 1);
                if (en == L"lt") {
                    o += L'<';
                } else if (en == L"gt") {
                    o += L'>';
                } else if (en == L"amp") {
                    o += L'&';
                } else if (en == L"quot") {
                    o += L'"';
                } else if (en == L"apos") {
                    o += L'\'';
                } else if (en.size() > 1 && en[0] == L'#') {
                    unsigned long v = en[1] == L'x' ? wcstoul(en.c_str() + 2, nullptr, 16)
                                                    : wcstoul(en.c_str() + 1, nullptr, 10);
                    if (v > 0 && v < 0x10000) {
                        o += (WCHAR)v;
                    } else if (v >= 0x10000 && v < 0x110000) {
                        v -= 0x10000;
                        o += (WCHAR)(0xD800 + (v >> 10));
                        o += (WCHAR)(0xDC00 + (v & 0x3FF));
                    }
                } else {
                    o += s.substr(i, e - i + 1);
                }
                i = e;
                continue;
            }
        }
        o += s[i];
    }
    return o;
}

static std::wstring XmlCollapse(const std::wstring& s) {
    std::wstring o;
    bool sp = false;
    for (WCHAR ch : s) {
        if (iswspace(ch)) {
            sp = !o.empty();
        } else {
            if (sp) o += L' ';
            sp = false;
            o += ch;
        }
    }
    if (o.size() > 400) o.resize(400), o += L"\u2026";
    return o;
}

static int XmlElement(const std::wstring& s, size_t& i, std::vector<PvNode>& n, int depth, bool& err) {
    if (depth > 200) {
        err = true;
        return -1;
    }
    size_t st = ++i;
    while (i < s.size() && !iswspace(s[i]) && s[i] != L'/' && s[i] != L'>') i++;
    std::wstring name = s.substr(st, i - st);
    std::wstring text = L"<" + name;
    bool self = false;
    for (;;) {
        while (i < s.size() && iswspace(s[i])) i++;
        if (i >= s.size()) {
            err = true;
            return -1;
        }
        if (s[i] == L'>') {
            i++;
            break;
        }
        if (s[i] == L'/') {
            if (i + 1 < s.size() && s[i + 1] == L'>') {
                i += 2;
                self = true;
                break;
            }
            err = true;
            return -1;
        }
        size_t a = i;
        while (i < s.size() && s[i] != L'=' && !iswspace(s[i]) && s[i] != L'>' && s[i] != L'/') i++;
        std::wstring an = s.substr(a, i - a), av;
        while (i < s.size() && iswspace(s[i])) i++;
        if (i < s.size() && s[i] == L'=') {
            i++;
            while (i < s.size() && iswspace(s[i])) i++;
            if (i >= s.size() || (s[i] != L'"' && s[i] != L'\'')) {
                err = true;
                return -1;
            }
            WCHAR q = s[i++];
            size_t e = s.find(q, i);
            if (e == kNpos) {
                err = true;
                return -1;
            }
            av = XmlDecode(s.substr(i, e - i));
            i = e + 1;
        }
        text += L" " + an + L"=\"" + av + L"\"";
    }
    text += self ? L" />" : L">";
    int me = (int)n.size();
    n.emplace_back();
    n[(size_t)me].text = text;
    if (self) return me;
    auto leaf = [&](const std::wstring& t) {
        n.emplace_back();
        n.back().text = t;
        n[(size_t)me].kids.push_back((int)n.size() - 1);
    };
    bool lastText = false;
    for (;;) {
        if (i >= s.size()) {
            err = true;
            return -1;
        }
        if (s[i] == L'<') {
            if (s.compare(i, 4, L"<!--") == 0) {
                size_t e = s.find(L"-->", i + 4);
                if (e == kNpos) {
                    err = true;
                    return -1;
                }
                leaf(L"<!-- " + XmlCollapse(s.substr(i + 4, e - i - 4)) + L" -->");
                i = e + 3;
                lastText = false;
            } else if (s.compare(i, 9, L"<![CDATA[") == 0) {
                size_t e = s.find(L"]]>", i + 9);
                if (e == kNpos) {
                    err = true;
                    return -1;
                }
                std::wstring tx = XmlCollapse(s.substr(i + 9, e - i - 9));
                if (!tx.empty()) {
                    leaf(tx);
                    lastText = true;
                }
                i = e + 3;
            } else if (s.compare(i, 2, L"<?") == 0) {
                size_t e = s.find(L"?>", i);
                if (e == kNpos) {
                    err = true;
                    return -1;
                }
                i = e + 2;
            } else if (s.compare(i, 2, L"</") == 0) {
                size_t e = s.find(L'>', i);
                if (e == kNpos || PvTrim(s.substr(i + 2, e - i - 2)) != name) {
                    err = true;
                    return -1;
                }
                i = e + 1;
                break;
            } else {
                int k = XmlElement(s, i, n, depth + 1, err);
                if (k < 0) return -1;
                n[(size_t)me].kids.push_back(k);
                lastText = false;
            }
            continue;
        }
        size_t e = s.find(L'<', i);
        if (e == kNpos) {
            err = true;
            return -1;
        }
        std::wstring tx = XmlCollapse(XmlDecode(s.substr(i, e - i)));
        i = e;
        if (!tx.empty()) {
            leaf(tx);
            lastText = true;
        }
    }
    if (n[(size_t)me].kids.size() == 1 && lastText) {
        n[(size_t)me].text += L"  " + n.back().text;
        n[(size_t)me].kids.clear();
        n.pop_back();
    }
    return me;
}

static void ParseXml(const std::wstring& s, std::vector<PvNode>& n, std::vector<int>& roots) {
    size_t i = 0;
    bool err = false;
    while (!err) {
        while (i < s.size() && (iswspace(s[i]) || s[i] == 0xFEFF)) i++;
        if (i >= s.size()) break;
        if (s[i] != L'<') {
            err = true;
        } else if (s.compare(i, 4, L"<!--") == 0 || s.compare(i, 2, L"<?") == 0) {
            size_t e = s.find(s[i + 1] == L'!' ? L"-->" : L"?>", i);
            if (e == kNpos)
                err = true;
            else
                i = e + (s[i + 1] == L'!' ? 3 : 2);
        } else if (s.compare(i, 2, L"<!") == 0) {
            int dpt = 0;
            size_t j = i;
            for (; j < s.size(); j++) {
                if (s[j] == L'[')
                    dpt++;
                else if (s[j] == L']')
                    dpt--;
                else if (s[j] == L'>' && dpt <= 0)
                    break;
            }
            if (j >= s.size())
                err = true;
            else
                i = j + 1;
        } else {
            int k = XmlElement(s, i, n, 0, err);
            if (k >= 0) roots.push_back(k);
        }
    }
    if (err) {
        n.clear();
        roots.clear();
        n.emplace_back();
        n[0].text = L"XML error near line " + std::to_wstring(PvLine(s, i));
        roots.push_back(0);
    }
}

static void ParseIni(const std::wstring& s, std::vector<PvNode>& n, std::vector<int>& roots) {
    int cur = -1;
    auto node = [&](const std::wstring& t) {
        n.emplace_back();
        n.back().text = t;
        return (int)n.size() - 1;
    };
    for (size_t i = 0; i <= s.size();) {
        size_t e = i;
        while (e < s.size() && s[e] != L'\r' && s[e] != L'\n') e++;
        std::wstring ln = PvTrim(s.substr(i, e - i));
        i = e + 1;
        if (ln.empty() || ln[0] == L';' || ln[0] == L'#') continue;
        if (ln[0] == L'[') {
            size_t r = ln.find(L']');
            cur = node(r == kNpos ? ln : ln.substr(0, r + 1));
            roots.push_back(cur);
            continue;
        }
        if (cur < 0) {
            cur = node(L"(no section)");
            roots.push_back(cur);
        }
        size_t eq = ln.find(L'=');
        if (eq == kNpos) eq = ln.find(L':');
        int k = node(eq == kNpos ? ln : PvTrim(ln.substr(0, eq)) + L" = " + PvTrim(ln.substr(eq + 1)));
        n[(size_t)cur].kids.push_back(k);
    }
}

// Preview pane (2/2): the window, its views, and the tick that keeps it in step with the editor.

static bool ExtIn(const std::wstring& e, const WCHAR* list) {
    const WCHAR* q = list;
    while (*q) {
        while (*q == L' ') q++;
        const WCHAR* x = q;
        while (*x && *x != L' ') x++;
        if ((size_t)(x - q) == e.size() && wcsncmp(q, e.c_str(), e.size()) == 0) return true;
        q = x;
    }
    return false;
}

static int PreviewKind(const std::wstring& nm) {
    size_t dot = nm.rfind(L'.');
    if (dot == kNpos || dot + 1 >= nm.size() || nm.size() - dot > 12) return pvNone;
    std::wstring e = nm.substr(dot + 1);
    for (auto& ch : e) ch = (WCHAR)towlower(ch);
    static const struct {
        const WCHAR* exts;
        int kind;
    } kinds[] = {{L"md markdown mdown mkd", pvMarkdown},
                 {L"rtf", pvRtf},
                 {L"csv", pvCsv},
                 {L"tsv tab", pvTsv},
                 {L"json geojson webmanifest", pvJson},
                 {L"xml xsd xsl xslt csproj vcxproj props targets config plist", pvXml},
                 {L"ini cfg inf desktop", pvIni}};
    for (const auto& k : kinds)
        if (ExtIn(e, k.exts)) return k.kind;
    return pvNone;
}

// The preview's own rich edit must not be taken for a Notepad editor.
static bool IsPreviewHost(HWND w) {
    WCHAR cls[40];
    return GetClassNameW(w, cls, 40) && wcscmp(cls, kPreviewClass) == 0;
}

struct PvStream {
    const char* p;
    size_t n;
};

static DWORD CALLBACK PvStreamCb(DWORD_PTR ck, LPBYTE buf, LONG cb, LONG* pcb) {
    auto* st = (PvStream*)ck;
    size_t k = std::min<size_t>((size_t)cb, st->n);
    memcpy(buf, st->p, k);
    st->p += k;
    st->n -= k;
    *pcb = (LONG)k;
    return 0;
}

static void PvSetTheme(HWND h, bool dark) {
    using Fn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
    static Fn fn = (Fn)GetProcAddress(LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
                                      "SetWindowTheme");
    if (fn) fn(h, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
}

static void PvOpen(const std::wstring& url) {
    if (url.compare(0, 7, L"http://") && url.compare(0, 8, L"https://") && url.compare(0, 7, L"mailto:")) return;
    using Fn = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
    static Fn fn = (Fn)GetProcAddress(LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
                                      "ShellExecuteW");
    if (fn) fn(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// True if h is a real rich-text control: streams in a bold RTF run and reads the bold flag back.
static bool PvRichProbe(HWND h) {
    static const char kProbe[] = "{\\rtf1\\ansi{\\b x}}";
    PvStream st{kProbe, sizeof(kProbe) - 1};
    EDITSTREAM es{};
    es.dwCookie = (DWORD_PTR)&st;
    es.pfnCallback = PvStreamCb;
    SendMessageW(h, EM_STREAMIN, SF_RTF, (LPARAM)&es);
    CHARRANGE cr = {0, 1};
    SendMessageW(h, EM_EXSETSEL, 0, (LPARAM)&cr);
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    SendMessageW(h, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    bool ok = (cf.dwMask & CFM_BOLD) && (cf.dwEffects & CFE_BOLD);
    SendMessageW(h, WM_SETTEXT, 0, (LPARAM)L"");
    return ok;
}

static void PvCreateChildren(EditorCtx* c) {
    PvData& d = c->pv;
    if (d.rich || !c->preview) return;
    LoadLibraryExW(L"Msftedit.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // Notepad's own DirectWrite control draws color emoji; keep it only if it really takes RTF, else fall back below.
    d.rich = CreateWindowExW(0, L"RichEditD2DPT", L"",
                             WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_NOHIDESEL,
                             0, 0, 0, 0, c->preview, nullptr, g_hInst, nullptr);
    if (d.rich) {
        SendMessageW(d.rich, EM_SETTEXTMODE, TM_RICHTEXT, 0);
        if (!PvRichProbe(d.rich)) {
            DestroyWindow(d.rich);
            d.rich = nullptr;
        }
    }
    if (!d.rich) d.rich = CreateWindowExW(0, L"RICHEDIT50W", L"",
                             WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_NOHIDESEL,
                             0, 0, 0, 0, c->preview, nullptr, g_hInst, nullptr);
    d.list = CreateWindowExW(0, L"SysListView32", L"", WS_CHILD | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS,
                             0, 0, 0, 0, c->preview, nullptr, g_hInst, nullptr);
    d.tree = CreateWindowExW(0, L"SysTreeView32", L"",
                             WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                             0, 0, 0, 0, c->preview, nullptr, g_hInst, nullptr);
    if (d.rich) {
        SendMessageW(d.rich, EM_SETEVENTMASK, 0, ENM_LINK | ENM_MOUSEEVENTS);
        SendMessageW(d.rich, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(14, 14));
    }
    if (d.list)
        ListView_SetExtendedListViewStyle(d.list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    RECT rc;
    GetClientRect(c->preview, &rc);
    SendMessageW(c->preview, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
}

static void PvTheme(EditorCtx* c) {
    PvData& d = c->pv;
    const PvPal& p = c->pvDark ? kPvDark : kPvLight;
    if (d.list) {
        ListView_SetBkColor(d.list, p.bg);
        ListView_SetTextBkColor(d.list, p.bg);
        ListView_SetTextColor(d.list, p.text);
        PvSetTheme(d.list, c->pvDark);
    }
    if (d.tree) {
        TreeView_SetBkColor(d.tree, p.bg);
        TreeView_SetTextColor(d.tree, p.text);
        TreeView_SetLineColor(d.tree, p.muted);
        PvSetTheme(d.tree, c->pvDark);
    }
    if (d.rich) PvSetTheme(d.rich, c->pvDark);
}

static void PvShow(PvData& d, HWND which) {
    for (HWND h : {d.rich, d.list, d.tree})
        if (h) ShowWindow(h, h == which ? SW_SHOWNA : SW_HIDE);
}

// Notepad's editor font (face and size, zoom included) for the tree and the table; true if it changed.
static bool PvFontSync(EditorCtx* c) {
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    SendMessageW(c->edit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    UINT num = 0, den = 0;
    SendMessageW(c->edit, EM_GETZOOM, (WPARAM)&num, (LPARAM)&den);
    if (!num || !den) num = den = 1;
    int dpi = (int)GetDpiForWindow(c->edit);
    if (dpi <= 0) dpi = 96;
    int px = std::max(6, (int)((double)(cf.yHeight > 0 ? cf.yHeight : 220) * dpi * num / (1440.0 * den) + 0.5));
    const WCHAR* face = cf.szFaceName[0] ? cf.szFaceName : L"Consolas";
    if (c->pvFont && px == c->pvFontPx && wcscmp(face, c->pvFace) == 0) return false;
    HFONT f = CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, face);
    for (HWND h : {c->pv.tree, c->pv.list})
        if (h) SendMessageW(h, WM_SETFONT, (WPARAM)f, TRUE);
    if (c->pv.tree) TreeView_SetItemHeight(c->pv.tree, (SHORT)-1);
    if (c->pvFont) DeleteObject(c->pvFont);
    c->pvFont = f;
    c->pvFontPx = px;
    wcsncpy(c->pvFace, face, LF_FACESIZE - 1);
    return true;
}

// Finds the link marks in the streamed text and turns the text between them into clickable links.
static void PvLinksByText(PvData& d) {
    d.links.clear();
    GETTEXTLENGTHEX gl = {GTL_NUMCHARS, 1200};
    LONG len = (LONG)SendMessageW(d.rich, EM_GETTEXTLENGTHEX, (WPARAM)&gl, 0);
    if (len <= 0) return;
    std::wstring t((size_t)len + 1, L'\0');
    GETTEXTEX gt{};
    gt.cb = (DWORD)((len + 1) * sizeof(WCHAR));
    gt.flags = GT_DEFAULT;
    gt.codepage = 1200;
    LRESULT got = SendMessageW(d.rich, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)&t[0]);
    t.resize((size_t)std::max<LRESULT>(0, got));
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_LINK;
    cf.dwEffects = CFE_LINK;
    for (size_t i = 0; i < t.size();) {
        size_t a = t.find((WCHAR)0xE000, i);
        if (a == kNpos) break;
        size_t b = t.find((WCHAR)0xE001, a + 1);
        size_t e = b == kNpos ? kNpos : t.find((WCHAR)0xE002, b + 1);
        if (e == kNpos) break;
        LONG s = (LONG)a + 1, f = (LONG)b;
        d.links.push_back({s, f, t.substr(b + 1, e - b - 1)});
        CHARRANGE cr = {s, f};
        SendMessageW(d.rich, EM_EXSETSEL, 0, (LPARAM)&cr);
        SendMessageW(d.rich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        i = e + 1;
    }
}

static LONG PvFindCh(HWND h, WCHAR ch, LONG from) {
    WCHAR pat[2] = {ch, 0};
    FINDTEXTEXW ft{};
    ft.chrg.cpMin = from;
    ft.chrg.cpMax = -1;
    ft.lpstrText = pat;
    return (LONG)SendMessageW(h, EM_FINDTEXTEXW, 1, (LPARAM)&ft);
}

// Same as PvLinksByText but locates the marks with EM_FINDTEXTEXW, so positions stay right after tables.
static void PvLinks(PvData& d) {
    d.links.clear();
    CHARFORMAT2W cf{};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_LINK;
    cf.dwEffects = CFE_LINK;
    LONG from = 0;
    for (;;) {
        LONG a = PvFindCh(d.rich, (WCHAR)0xE000, from);
        if (a < 0) break;
        LONG b = PvFindCh(d.rich, (WCHAR)0xE001, a + 1);
        if (b < 0) break;
        LONG e = PvFindCh(d.rich, (WCHAR)0xE002, b + 1);
        if (e < 0) break;
        std::wstring url((size_t)(e - b), L'\0');
        TEXTRANGEW tr{{b + 1, e}, &url[0]};
        LONG got = (LONG)SendMessageW(d.rich, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
        url.resize((size_t)std::max<LONG>(0, got));
        d.links.push_back({a + 1, b, url});
        CHARRANGE cr = {a + 1, b};
        SendMessageW(d.rich, EM_EXSETSEL, 0, (LPARAM)&cr);
        SendMessageW(d.rich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        from = e + 1;
    }
    d.toggles.clear();
    for (LONG from2 = 0;;) {
        LONG a = PvFindCh(d.rich, (WCHAR)0xE003, from2);
        if (a < 0) break;
        LONG b = PvFindCh(d.rich, (WCHAR)0xE004, a + 1);
        if (b < 0) break;
        LONG e = PvFindCh(d.rich, (WCHAR)0xE005, b + 1);
        if (e < 0) break;
        std::wstring num((size_t)(e - b), L'\0');
        TEXTRANGEW tr{{b + 1, e}, &num[0]};
        SendMessageW(d.rich, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
        d.toggles.push_back({a + 1, b, _wtoi(num.c_str())});
        from2 = e + 1;
    }
    if (d.links.empty()) PvLinksByText(d);
}

static void PvLoadRtf(PvData& d, const std::string& rtf, COLORREF bg, bool keep) {
    POINT sp{};
    if (keep) SendMessageW(d.rich, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    SendMessageW(d.rich, WM_SETREDRAW, FALSE, 0);
    SendMessageW(d.rich, EM_SETBKGNDCOLOR, 0, (LPARAM)bg);
    PvStream st{rtf.data(), rtf.size()};
    EDITSTREAM es{};
    es.dwCookie = (DWORD_PTR)&st;
    es.pfnCallback = PvStreamCb;
    SendMessageW(d.rich, EM_STREAMIN, SF_RTF, (LPARAM)&es);
    PvLinks(d);
    SendMessageW(d.rich, EM_SETSEL, 0, 0);
    SendMessageW(d.rich, EM_SETSCROLLPOS, 0, (LPARAM)&sp);
    SendMessageW(d.rich, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(d.rich, nullptr, TRUE);
}

static void PvLoadList(PvData& d) {
    ListView_SetItemCount(d.list, 0);
    while (ListView_DeleteColumn(d.list, 0)) {
    }
    size_t cols = 0;
    for (size_t r = 0; r < d.rows.size() && r < 200; r++) cols = std::max(cols, d.rows[r].size());
    cols = std::min<size_t>(cols, 64);
    int maxW = MulDiv(360, std::max(96, (int)GetDpiForWindow(d.list)), 96);
    for (size_t k = 0; k < cols; k++) {
        std::wstring name = d.rows.empty() || k >= d.rows[0].size() ? L"" : d.rows[0][k];
        LVCOLUMNW col{};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 100;
        col.pszText = (LPWSTR)name.c_str();
        ListView_InsertColumn(d.list, (int)k, &col);
    }
    ListView_SetItemCount(d.list, d.rows.empty() ? 0 : (int)d.rows.size() - 1);
    for (size_t k = 0; k < cols; k++) {
        ListView_SetColumnWidth(d.list, (int)k, LVSCW_AUTOSIZE_USEHEADER);
        if (ListView_GetColumnWidth(d.list, (int)k) > maxW) ListView_SetColumnWidth(d.list, (int)k, maxW);
    }
    InvalidateRect(d.list, nullptr, TRUE);
}

static void PvAddKids(PvData& d, HTREEITEM parent, const std::vector<int>& ids) {
    for (int id : ids) {
        TVINSERTSTRUCTW is{};
        is.hParent = parent;
        is.hInsertAfter = TVI_LAST;
        is.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
        is.item.pszText = (LPWSTR)d.nodes[(size_t)id].text.c_str();
        is.item.cChildren = d.nodes[(size_t)id].kids.empty() ? 0 : 1;
        is.item.lParam = id;
        TreeView_InsertItem(d.tree, &is);
    }
}

// Children are inserted when a node is first expanded (TVN_ITEMEXPANDING), so big files load fast.
static void PvLoadTree(PvData& d) {
    SendMessageW(d.tree, WM_SETREDRAW, FALSE, 0);
    PvAddKids(d, TVI_ROOT, d.roots);
    int n = 0;
    for (HTREEITEM h = TreeView_GetRoot(d.tree); h && n < 30; h = TreeView_GetNextSibling(d.tree, h), n++)
        TreeView_Expand(d.tree, h, TVE_EXPAND);
    SendMessageW(d.tree, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(d.tree, nullptr, TRUE);
}

// The document text: the verified copy when it is current, else read straight from the editor (it needs no line table).
static const std::wstring& PvSource(EditorCtx* c) {
    if (c->docOk && c->docSynLen == c->synLen) return c->docText;
    LONG len = c->synLen;
    if (len <= 0 || (size_t)len > kSyntaxMaxChars) {
        c->pvText.clear();
        return c->pvText;
    }
    c->pvText.assign((size_t)len + 1, L'\0');
    GETTEXTEX gt{};
    gt.cb = (DWORD)((len + 1) * sizeof(WCHAR));
    gt.flags = GT_DEFAULT;
    gt.codepage = 1200;
    SendMessageW(c->edit, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)&c->pvText[0]);
    c->pvText.resize(wcsnlen(c->pvText.c_str(), (size_t)len + 1));
    CollapseBreaks(c->pvText);
    return c->pvText;
}

static void PreviewBuild(EditorCtx* c, int kind) {
    PvData& d = c->pv;
    PvCreateChildren(c);
    if (!d.rich || !d.list || !d.tree) return;
    PvFontSync(c);
    PvTheme(c);
    const std::wstring& t = PvSource(c);
    const PvPal& pal = c->pvDark ? kPvDark : kPvLight;
    if (kind == pvMarkdown) {
        int dpi = (int)GetDpiForWindow(c->edit);
        int tw = std::max(1500, MulDiv(c->pvW - 70, 1440, dpi > 0 ? dpi : 96));
        int fs = std::max(12, (int)(c->pvFontPx * 144.0 / (dpi > 0 ? dpi : 96) + 0.5));
        g_mdOpen = &d.flip;
        PvLoadRtf(d, MarkdownToRtf(t, pal, tw, fs), pal.bg, c->pvKeep);
        PvShow(d, d.rich);
    } else if (kind == pvRtf) {
        std::string r;
        for (WCHAR ch : t) {
            if (ch < 0x80)
                r += (char)ch;
            else
                r += "\\u" + std::to_string((int)(short)ch) + "?";
        }
        size_t at = r.find_first_not_of(" \t\r\n");
        bool ok = at != std::string::npos && r.compare(at, 5, "{\\rtf") == 0;
        PvLoadRtf(d, ok ? r : std::string("{\\rtf1 This file is not a valid RTF document.}"), kPvLight.bg,
                  c->pvKeep);
        PvShow(d, d.rich);
    } else if (kind == pvCsv || kind == pvTsv) {
        d.rows.clear();
        ParseCsv(t, kind == pvTsv ? L'\t' : DetectDelim(t), d.rows);
        PvLoadList(d);
        PvShow(d, d.list);
    } else {
        TreeView_DeleteAllItems(d.tree);
        d.nodes.clear();
        d.roots.clear();
        if (kind == pvJson)
            ParseJson(t, d.nodes, d.roots);
        else if (kind == pvXml)
            ParseXml(t, d.nodes, d.roots);
        else
            ParseIni(t, d.nodes, d.roots);
        PvLoadTree(d);
        PvShow(d, d.tree);
    }
}

// Posted by a click on a <details> triangle: flips that block and redraws, keeping the scroll position.
constexpr UINT kPvToggle = WM_APP + 0x1D1;

static LRESULT CALLBACK PreviewProc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam) {
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
            bool dark = c && c->pvDark;
            HBRUSH b = CreateSolidBrush((dark ? kPvDark : kPvLight).bg);
            FillRect(dc, &rc, b);
            DeleteObject(b);
            RECT line = {0, 0, 1, rc.bottom};
            b = CreateSolidBrush((dark ? kDark : kLight).separator);
            FillRect(dc, &line, b);
            DeleteObject(b);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_SIZE: {
            if (!c) break;
            int w = LOWORD(lParam), ht = HIWORD(lParam);
            for (HWND k : {c->pv.rich, c->pv.list, c->pv.tree})
                if (k) MoveWindow(k, 1, 0, std::max(0, w - 1), ht, TRUE);
            return 0;
        }
        case kPvToggle: {
            if (!c) break;
            if (!c->pv.flip.erase((int)wParam)) c->pv.flip.insert((int)wParam);
            bool keep = c->pvKeep;
            c->pvKeep = true;
            PreviewBuild(c, pvMarkdown);
            c->pvKeep = keep;
            return 0;
        }
        case WM_NOTIFY: {
            if (!c) break;
            auto* nh = (NMHDR*)lParam;
            PvData& d = c->pv;
            if (nh->hwndFrom == d.rich && nh->code == EN_LINK) {
                auto* el = (ENLINK*)lParam;
                if (el->msg == WM_LBUTTONUP)
                    for (const PvLink& l : d.links)
                        if (el->chrg.cpMin >= l.a && el->chrg.cpMin < l.b) {
                            PvOpen(l.url);
                            break;
                        }
            } else if (nh->hwndFrom == d.rich && nh->code == EN_MSGFILTER) {
                auto* mf = (MSGFILTER*)lParam;
                if (mf->msg == WM_LBUTTONUP && !d.toggles.empty()) {
                    POINTL pt = {(LONG)(short)LOWORD(mf->lParam), (LONG)(short)HIWORD(mf->lParam)};
                    LONG ci = (LONG)SendMessageW(d.rich, EM_CHARFROMPOS, 0, (LPARAM)&pt);
                    for (const PvToggle& tg : d.toggles)
                        if (ci >= tg.a && ci < tg.b) {
                            PostMessageW(h, kPvToggle, (WPARAM)tg.id, 0);
                            break;
                        }
                }
            } else if (nh->hwndFrom == d.list && nh->code == LVN_GETDISPINFOW) {
                auto* di = (NMLVDISPINFOW*)lParam;
                size_t r = (size_t)di->item.iItem + 1, col = (size_t)di->item.iSubItem;
                if (di->item.mask & LVIF_TEXT)
                    di->item.pszText = (LPWSTR)(r < d.rows.size() && col < d.rows[r].size()
                                                    ? d.rows[r][col].c_str()
                                                    : L"");
            } else if (nh->hwndFrom == d.tree && nh->code == TVN_ITEMEXPANDINGW) {
                auto* nt = (NMTREEVIEWW*)lParam;
                int id = (int)nt->itemNew.lParam;
                if (nt->action == TVE_EXPAND && id >= 0 && (size_t)id < d.nodes.size() &&
                    !d.nodes[(size_t)id].done) {
                    d.nodes[(size_t)id].done = true;
                    PvAddKids(d, nt->itemNew.hItem, d.nodes[(size_t)id].kids);
                }
            }
            return 0;
        }
        case WM_NCDESTROY:
            if (c) c->preview = nullptr;
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

static void PreviewHide(EditorCtx* c) {
    if (c->preview && IsWindowVisible(c->preview)) ShowWindow(c->preview, SW_HIDE);
    c->pvPlacement = {};
}

static void PreviewDeactivate(EditorCtx* c) {
    if (c->preview) ShowWindow(c->preview, SW_HIDE);
    bool was = c->pvActive;
    c->pvActive = false;
    c->pvW = 0;
    c->pvKind = 0;
    c->pvRev = ~0u;
    c->pvPlacement = {};
    c->pv.rows.clear();
    c->pv.nodes.clear();
    c->pv.roots.clear();
    std::wstring().swap(c->pvText);
    if (was && IsWindow(c->edit)) ReleaseOrReapply(c);
}

// Sits just left of the minimap (or the editor's right edge) and fills the editor's height.
static void PreviewReposition(EditorCtx* c, const RECT& cr) {
    HWND parent = GetParent(c->edit);
    if (!parent) return;
    POINT p{0, 0};
    MapWindowPoints(c->edit, parent, &p, 1);
    int right = cr.right - (c->mmActive ? c->mmW : 0);
    RECT want = {p.x + right - c->pvW, p.y, p.x + right, p.y + cr.bottom};
    if (IsWindowVisible(c->preview) && EqualRect(&want, &c->pvPlacement)) return;
    c->pvPlacement = want;
    SetWindowPos(c->preview, HWND_TOP, want.left, want.top, want.right - want.left, want.bottom - want.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static void PreviewTick(EditorCtx* c, ULONGLONG now) {
    int kind = pvNone, dpi = 96;
    RECT cr = {};
    std::wstring nm;
    if (g_previewEnabled.load() && c->preview && !g_unloading.load()) {
        GetClientRect(c->edit, &cr);
        dpi = (int)GetDpiForWindow(c->edit);
        if (dpi <= 0) dpi = 96;
        if (cr.right >= MulDiv(kPvMinDip, dpi, 96) && (size_t)std::max<LONG>(c->synLen, 0) <= kPvMaxChars) {
            nm = DocName(c->edit);
            kind = PreviewKind(nm);
        }
    }
    if (kind == pvNone) {
        if (c->pvActive) PreviewDeactivate(c);
        return;
    }
    int step = std::max(1, MulDiv(16, dpi, 96));
    int w = (cr.right - (c->mmActive ? c->mmW : 0)) / 2 / step * step;
    if (!c->pvActive) {
        CaptureOriginals(c);
        c->pvActive = true;
        c->pvW = w;
        c->pvRev = ~0u;
        if (!c->docOk) c->docStale = true;
        ApplyInset(c);
    } else if (w != c->pvW) {
        c->pvW = w;
        if (kind == pvMarkdown) c->pvRev = ~0u;
        ApplyInset(c);
    }
    PreviewReposition(c, cr);
    if (c->pv.tree && PvFontSync(c)) c->pvRev = ~0u;

    bool dark = GuideDark(c);
    if (kind != c->pvKind || nm != c->pvName) {
        c->pvKind = kind;
        c->pvName = nm;
        c->pvKeep = false;
        c->pvRev = ~0u;
    }
    if (dark != c->pvDark) {
        c->pvDark = dark;
        c->pvRev = ~0u;
        InvalidateRect(c->preview, nullptr, FALSE);
    }
    if (c->docRev == c->pvRev) return;
    // Rebuild right away the first time, then at most every kPvDebounceMs while the text keeps changing.
    if (c->pvDue == 0)
        c->pvDue = c->pvRev == ~0u ? now : now + kPvDebounceMs + (ULONGLONG)std::max<LONG>(c->synLen, 0) / 500;
    if (now < c->pvDue) return;
    PreviewBuild(c, kind);
    c->pvKeep = true;
    c->pvRev = c->docRev;
    c->pvDue = 0;
}

// System menu toggles + top-level window subclass

static void ToggleEnabled() {
    bool n = !g_enabled.load();
    g_enabled = n;
    Wh_SetIntValue(L"Enabled", n ? 1 : 0);
}

static void ToggleMinimap() {
    bool n = !g_minimapEnabled.load();
    g_minimapEnabled = n;
    Wh_SetIntValue(L"MinimapEnabled", n ? 1 : 0);
}

static void TogglePreview() {
    bool n = !g_previewEnabled.load();
    g_previewEnabled = n;
    Wh_SetIntValue(L"PreviewEnabled", n ? 1 : 0);
}

static void RemoveSysItem(HMENU m) {
    if (!m) return;
    DeleteMenu(m, kSysCmdPreviewId, MF_BYCOMMAND);
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
            if (HIWORD(lParam)) {
                CheckMenuItem(GetSystemMenu(h, FALSE), kSysCmdId,
                              MF_BYCOMMAND | (g_enabled.load() ? MF_CHECKED : MF_UNCHECKED));
                CheckMenuItem(GetSystemMenu(h, FALSE), kSysCmdMinimapId,
                              MF_BYCOMMAND | (g_minimapEnabled.load() ? MF_CHECKED : MF_UNCHECKED));
                CheckMenuItem(GetSystemMenu(h, FALSE), kSysCmdPreviewId,
                              MF_BYCOMMAND | (g_previewEnabled.load() ? MF_CHECKED : MF_UNCHECKED));
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
            if ((wParam & 0xFFF0) == kSysCmdPreviewId) {
                TogglePreview();
                return 0;
            }
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                g_enabled = Wh_GetIntValue(L"Enabled", 1) != 0;
                g_minimapEnabled = Wh_GetIntValue(L"MinimapEnabled", 1) != 0;
                g_previewEnabled = Wh_GetIntValue(L"PreviewEnabled", 0) != 0;
            }
            break;
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            g_themeEpoch++;
            break;
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
        if (GetMenuState(sm, kSysCmdPreviewId, MF_BYCOMMAND) == (UINT)-1) {
            AppendMenuW(sm, MF_STRING | (g_previewEnabled.load() ? MF_CHECKED : 0),
                        kSysCmdPreviewId, kMenuTextPreview);
        }
    }
    EnumChildWindows(top, DiscoverProc, 0);
}

static void DetachTop(TopCtx* t) {
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find(g_tops.begin(), g_tops.end(), t);
        if (it == g_tops.end()) return;
        g_tops.erase(it);
    }
    HWND top = t->hwnd;
    if (IsWindow(top)) {
        RemoveSysItem(GetSystemMenu(top, FALSE));
        RemoveWindowSubclass(top, TopSubclassProc, kTopSubclassId);
        RemovePropW(top, kPropTop);
    }
    delete t;
}

// Editor attach / subclass

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
        SetWindowLongPtrW(c->gutter, GWLP_USERDATA, 0);
        DestroyWindow(c->gutter);
        c->gutter = nullptr;
    }
    if (c->minimap) {
        if (GetCapture() == c->minimap) ReleaseCapture();
        SetWindowLongPtrW(c->minimap, GWLP_USERDATA, 0);
        DestroyWindow(c->minimap);
        c->minimap = nullptr;
    }
    if (c->preview) {
        SetWindowLongPtrW(c->preview, GWLP_USERDATA, 0);
        DestroyWindow(c->preview);
        c->preview = nullptr;
    }
    if (!windowDestroyed && IsWindow(c->edit)) {
        Deactivate(c);
        DeactivateMinimap(c);
        PreviewDeactivate(c);
        RemoveWindowSubclass(c->edit, EditSubclassProc, kEditSubclassId);
        RemovePropW(c->edit, kPropEditor);
    }
    if (c->font) DeleteObject(c->font);
    if (c->fontBold) DeleteObject(c->fontBold);
    if (c->pvFont) DeleteObject(c->pvFont);
    delete c;
}

// Called from editor messages; Activate() must only run from the timer, so here we only advance an active gutter.
static void TickNow(EditorCtx* c) {
    // [keep] message-driven ticks also speed the timer up
    WakeTick(c);
    bool prev = c->inMsg;
    c->inMsg = true;
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
                c->synSettle = 8;
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
            c->settle = 8;
            c->synSettle = 8;
            // Loading replaces the text, so markers start over; anything else is a user edit that freezes the baseline.
            if (msg == WM_SETTEXT || msg == EM_SETTEXTEX || msg == EM_STREAMIN)
                c->mkReload = true;
            else
                c->mkUserEdit = true;
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            TickNow(c);
            return r;
        }
        case WM_VSCROLL:
        case WM_MOUSEWHEEL:
        case EM_LINESCROLL:
        case EM_SCROLL:
        case EM_SETSCROLLPOS:
        case EM_SETTARGETDEVICE:
        case EM_SETZOOM: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            c->sigValid = false;
            c->synSigValid = false;
            TickNow(c);
            return r;
        }
        case WM_SIZE:
        case WM_MOVE:
        case WM_WINDOWPOSCHANGED:
        case WM_SHOWWINDOW: {
            LRESULT r = DefSubclassProc(h, msg, wParam, lParam);
            if ((c->active || c->mmActive || c->pvActive) && c->rectFallback && msg == WM_SIZE)
                ApplyRect(c, c->active ? c->gutterW : 0, RightInset(c));
            TickNow(c);
            return r;
        }
        // [keep] selection moves also wake the timer (else a click shows up only on the next slow tick)
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
            if (wParam & MK_LBUTTON) WakeTick(c);
            return r;
        }
    }
    return DefSubclassProc(h, msg, wParam, lParam);
}

void AttachEditor(HWND edit) {
    if (g_unloading.load() || GetPropW(edit, kPropEditor)) return;
    if (!(GetWindowLongPtrW(edit, GWL_STYLE) & ES_MULTILINE)) return;
    HWND parent = GetParent(edit);
    if (!parent || IsPreviewHost(parent)) return;

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

    // Hidden sibling window above the editor; it also hosts the poll timer.
    // [keep] only created if the class registered; uses the mod's module handle
    if (g_gutterClassOk.load())
        c->gutter = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kGutterClass, L"",
                                    WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, parent, nullptr,
                                    g_hInst, c);
    // [keep] start fast for a warm-up; Tick() then picks the rate
    c->fastUntil = GetTickCount64() + kTickStartMs;
    SetTickRate(c, kTickFastMs);

    // Second hidden sibling on the right; shown only while the minimap is on.
    // [keep] skipped if the minimap class did not register
    if (g_minimapClassOk.load())
        c->minimap = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kMinimapClass, L"",
                                     WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, parent, nullptr,
                                     g_hInst, c);
    if (g_previewClassOk.load())
        c->preview = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kPreviewClass, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                     0, 0, 0, 0, parent, nullptr, g_hInst, c);

    HWND top = GetAncestor(edit, GA_ROOT);
    if (top && top != edit) {
        WCHAR cls[32];
        if (GetClassNameW(top, cls, 32) && _wcsicmp(cls, L"Notepad") == 0) AttachTop(top);
    }
}

// Syntax highlighting (2/2): Direct2D DrawGlyphRun hook that recolors riched20 text runs without touching the document.

using GlyphFn = void(STDMETHODCALLTYPE*)(void*, D2D1_POINT_2F, const DWRITE_GLYPH_RUN*, void*,
                                         DWRITE_MEASURING_MODE);

struct ModRange {
    uintptr_t base, end;
    bool riched;
};
struct FaceMap {
    IDWriteFontFace* face;  // [keep] typed (was void*)
    std::unordered_map<UINT16, WCHAR> rev;
};
static std::mutex g_synMutex;
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

// Maps glyph indices back to characters via the font's cmap; unknown glyphs become 0xFFFF.
// [keep] typed COM calls instead of raw vtable slots
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
        face->AddRef();
        std::vector<UINT32> cps;
        for (UINT32 cp = 0x20; cp <= 0x7E; cp++) cps.push_back(cp);
        for (UINT32 cp = 0xA0; cp <= 0x17F; cp++) cps.push_back(cp);
        std::vector<UINT16> gl(cps.size(), 0);
        if (SUCCEEDED(face->GetGlyphIndices(cps.data(), (UINT32)cps.size(), gl.data()))) {
            for (size_t i = 0; i < cps.size(); i++)
                if (gl[i]) fm->rev.emplace(gl[i], (WCHAR)cps[i]);
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
    // Palette: Mariana (dark) and Breakers (light). Hues and saturations are
    // unchanged; brightness is nudged 1 point up in dark mode and down in light
    // mode. String, comment and separator are what the schemes tint apart.
    static const int kD[][3] = {
        {0x00, 0x00, 0x00},
        {0xC9, 0x97, 0xC9},
        {0x67, 0x9B, 0xCF},
        {0x9B, 0xCA, 0x96},
        {0xA8, 0xAE, 0xBC},
        {0xFC, 0xB0, 0x59},
        {0xC9, 0x97, 0xC9},
        {0xEF, 0x60, 0x67},
        {0xC9, 0x97, 0xC9},
        {0xC9, 0x97, 0xC9},
        {0xEF, 0x60, 0x67},
        {0xC9, 0x97, 0xC9},
        {0xFC, 0xB0, 0x59},
        {0x60, 0xB7, 0xB7},
        {0x60, 0xB7, 0xB7},
        {0x67, 0x9B, 0xCF},
        {0x60, 0xB7, 0xB7},
        {0x67, 0x9B, 0xCF},
        {0xEF, 0x60, 0x67},
        {0xC9, 0x97, 0xC9},
        {0x9B, 0xCA, 0x96},
        {0xC9, 0x97, 0xC9},
        {0x60, 0xB7, 0xB7},
        {0xFC, 0xB0, 0x59},
        {0xEF, 0x60, 0x67},
        {0xFC, 0xB0, 0x59},
        {0x67, 0x9B, 0xCF},
        {0xFC, 0x7C, 0x59},
        {0xA8, 0xAE, 0xBC},
        {0x60, 0xB7, 0xB7},
    };
    static const int kL[][3] = {
        {0x00, 0x00, 0x00},
        {0xC3, 0x93, 0xC3},
        {0x65, 0x97, 0xC9},
        {0x7E, 0xB6, 0x77},
        {0x96, 0x96, 0x96},
        {0xF6, 0xAC, 0x57},
        {0xC3, 0x93, 0xC3},
        {0xE9, 0x5E, 0x65},
        {0xC3, 0x93, 0xC3},
        {0xC3, 0x93, 0xC3},
        {0xE9, 0x5E, 0x65},
        {0xC3, 0x93, 0xC3},
        {0xF6, 0xAC, 0x57},
        {0x5E, 0xB1, 0xB1},
        {0x5E, 0xB1, 0xB1},
        {0x65, 0x97, 0xC9},
        {0x5E, 0xB1, 0xB1},
        {0x65, 0x97, 0xC9},
        {0xE9, 0x5E, 0x65},
        {0xC3, 0x93, 0xC3},
        {0x7E, 0xB6, 0x77},
        {0xC3, 0x93, 0xC3},
        {0x5E, 0xB1, 0xB1},
        {0xF6, 0xAC, 0x57},
        {0xE9, 0x5E, 0x65},
        {0xF6, 0xAC, 0x57},
        {0x65, 0x97, 0xC9},
        {0xF6, 0x7A, 0x57},
        {0xAA, 0x79, 0x67},
        {0x5E, 0xB1, 0xB1},
    };
    static_assert(sizeof(kD) / sizeof(kD[0]) == kcCount, "dark palette size");
    static_assert(sizeof(kL) / sizeof(kL[0]) == kcCount, "light palette size");
    const int* c = (dark ? kD : kL)[k];
    D2D1_COLOR_F col;
    col.r = c[0] / 255.0f;
    col.g = c[1] / 255.0f;
    col.b = c[2] / 255.0f;
    col.a = 1.0f;
    return col;
}

// The visible, syntax-enabled editor on the calling thread, if any.
static EditorCtx* CurrentEditor() {
    DWORD tid = GetCurrentThreadId();
    std::lock_guard<std::mutex> lk(g_mutex);
    for (EditorCtx* c : g_editors)
        if (c->uiThread == tid && c->shown && c->lang >= 0 && !c->snap.empty() &&
            c->docSynLen == c->synLen && c->cls.size() == c->docText.size())
            return c;
    return nullptr;
}

// Finds S (0xFFFF = wildcard) in a row, preferring the match closest to the estimated column.
static LONG FindInRow(const EditorCtx* c, const SnapRow& r, const std::wstring& S, double estCol) {
    LONG n = (LONG)S.size();
    LONG end = std::min<LONG>(r.end, (LONG)c->docText.size());
    LONG best = -1;
    double bestD = 1e18;
    LONG start = std::max<LONG>(r.idx, 0);
    // [keep] rows over 4096 chars search only a +-1024 window around the estimated column
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

// Returns true if the run was drawn (in colored pieces) by us.
static bool TryColorRun(void* self, D2D1_POINT_2F o, const DWRITE_GLYPH_RUN* run, void* brush,
                        DWRITE_MEASURING_MODE mode, GlyphFn orig) {
    EditorCtx* c = CurrentEditor();
    if (!c) return false;

    std::wstring S;
    if (!DecodeRun(run, S)) return false;
    const LONG n = (LONG)S.size();

    // Device-pixel position of the run's baseline origin.
    // [keep] typed COM calls instead of vtable slots 52 / 31
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
        c->baseOff = 0.85 * run->fontEmSize * scale;
        c->offValid = true;
    }

    // 1) Pick the row whose top best matches this baseline and verify the text.
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
                if (oi == 0) break;
            }
        }
        trusted = (foundRow == k) || (matches == 1 && n >= 4);
    }

    // 2) Fallback: search every measured row (also recovers from a bad offset).
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

    // Leave selected text to the editor; the selection is read live (the editor repaints before our tick).
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
    ID2D1SolidColorBrush* brushes[kcCount] = {};  // [keep] typed (was void*)
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

// Indent guides (2/2): drawn into the render target right before EndDraw.

using EndDrawFn = HRESULT(STDMETHODCALLTYPE*)(void*, D2D1_TAG*, D2D1_TAG*);

constexpr COLORREF kGuideColorDark = RGB(0x46, 0x46, 0x46);
constexpr COLORREF kGuideColorLight = RGB(0xD8, 0xD8, 0xD8);

// The visible editor on this thread, if it is the only one.
static EditorCtx* GuideEditor() {
    DWORD tid = GetCurrentThreadId();
    EditorCtx* found = nullptr;
    std::lock_guard<std::mutex> lk(g_mutex);
    for (EditorCtx* c : g_editors) {
        if (c->uiThread != tid || !c->shown || !IsWindow(c->edit) || !IsWindowVisible(c->edit))
            continue;
        if (found) return nullptr;
        found = c;
    }
    if (!found || !found->guide.valid || found->guide.rows.empty()) return nullptr;
    return found;
}

static thread_local bool tl_inGuide = false;

static void DrawIndentGuides(void* self) {
    if (tl_inGuide) return;
    EditorCtx* c = GuideEditor();
    if (!c) return;
    tl_inGuide = true;
    struct Reset {
        ~Reset() { tl_inGuide = false; }
    } reset;

    const GuideModel& g = c->guide;
    if (g.unitPx <= 0) return;
    int maxLevels = 0;
    for (const GuideRow& r : g.rows) maxLevels = std::max(maxLevels, r.levels);
    if (maxLevels <= 0) return;

    auto* rt = static_cast<ID2D1RenderTarget*>(self);
    float dpiX = 96.f, dpiY = 96.f;
    rt->GetDpi(&dpiX, &dpiY);
    if (dpiX <= 0.f || dpiY <= 0.f) return;
    const double sx = 96.0 / dpiX, sy = 96.0 / dpiY;
    const int lw = std::max(1, (int)(dpiX / 96.0 + 0.5));

    POINT sp{};
    SendMessageW(c->edit, EM_GETSCROLLPOS, 0, (LPARAM)&sp);
    const int dx = g.scroll.x - sp.x;
    const int dy = g.scroll.y - sp.y;

    COLORREF cref = g.dark ? kGuideColorDark : kGuideColorLight;
    D2D1_COLOR_F col;
    col.r = GetRValue(cref) / 255.0f;
    col.g = GetGValue(cref) / 255.0f;
    col.b = GetBValue(cref) / 255.0f;
    col.a = 1.0f;
    ID2D1SolidColorBrush* br = nullptr;
    if (FAILED(rt->CreateSolidColorBrush(col, &br)) || !br) return;

    // Device pixels, identity transform, no anti-aliasing; state restored below.
    D2D1_MATRIX_3X2_F oldT = {1, 0, 0, 1, 0, 0};
    rt->GetTransform(&oldT);
    D2D1_ANTIALIAS_MODE oldAA = rt->GetAntialiasMode();
    const D2D1_MATRIX_3X2_F ident = {1, 0, 0, 1, 0, 0};
    rt->SetTransform(&ident);
    rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

    auto fill = [&](int x, int top, int bottom) {
        D2D1_RECT_F r = {(float)(x * sx), (float)(top * sy), (float)((x + lw) * sx),
                         (float)(bottom * sy)};
        rt->FillRectangle(&r, br);
    };
    for (int k = 0; k < maxLevels; k++) {
        const int x = g.x0 + dx + k * g.unitPx;
        if (x < 0 || x + lw > g.clientW) continue;
        bool open = false;
        int segTop = 0, segBottom = 0;
        for (const GuideRow& r : g.rows) {
            const int top = r.y + dy, bottom = top + r.h;
            const bool has = r.levels > k && bottom > 0 && top < g.clientH;
            if (has && open && top <= segBottom + 1) {
                segBottom = std::max(segBottom, bottom);
                continue;
            }
            if (open) {
                fill(x, segTop, segBottom);
                open = false;
            }
            if (has) {
                segTop = top;
                segBottom = bottom;
                open = true;
            }
        }
        if (open) fill(x, segTop, segBottom);
    }

    rt->SetAntialiasMode(oldAA);
    rt->SetTransform(&oldT);
    br->Release();
}

static std::vector<void*> g_hooked;

static void HookSlot(void** vtbl, int slot, void* hook, void** orig) {
    void* target = vtbl[slot];
    for (void* h : g_hooked)
        if (h == target) return;
    if (Wh_SetFunctionHook(target, hook, orig)) g_hooked.push_back(target);
}

// One instantiation per render-target class (each has its own original).
template <int V>
struct GlyphHook {
    static inline GlyphFn orig = nullptr;
    static inline EndDrawFn origEnd = nullptr;
    static void STDMETHODCALLTYPE hook(void* self, D2D1_POINT_2F o, const DWRITE_GLYPH_RUN* run,
                                       void* brush, DWRITE_MEASURING_MODE m) {
        HandleGlyphRun(self, o, run, brush, m, orig, __builtin_return_address(0));
    }
    static HRESULT STDMETHODCALLTYPE endHook(void* self, D2D1_TAG* t1, D2D1_TAG* t2) {
        void* ra = __builtin_return_address(0);
        if (self && !g_unloading.load() && CallerIsRiched(ra)) DrawIndentGuides(self);
        return origEnd(self, t1, t2);
    }
    static void Install(void* comObject) {
        void** vtbl = *(void***)comObject;
        HookSlot(vtbl, 29, (void*)hook, (void**)&orig);
        HookSlot(vtbl, 49, (void*)endHook, (void**)&origEnd);
    }
};

// Create throw-away render targets just to read their vtables; the code is shared by every real instance.
static void InstallD2DHooks() {
    // [keep] load from System32 only (not KnownDLLs, so a local copy could win)
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

    // 0: ID2D1DeviceContext (most likely for a GPU-composed control)
    HMODULE d3d = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);  // [keep] System32 only
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

// Hooks / entry points

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
    // [keep] StringSetting RAII helper; no NULL checks needed
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
    g_mmClickLine = wcscmp(cb.get(), L"proportional") != 0;
}

BOOL Wh_ModInit() {
    InstallD2DHooks();
    LoadSettings();
    g_enabled = Wh_GetIntValue(L"Enabled", 1) != 0;
    g_minimapEnabled = Wh_GetIntValue(L"MinimapEnabled", 1) != 0;
    g_previewEnabled = Wh_GetIntValue(L"PreviewEnabled", 0) != 0;
    g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    // [keep] register classes with the mod's module and remember success; falls back to the exe's handle
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

    WNDCLASSW pc{};
    pc.lpfnWndProc = PreviewProc;
    pc.hInstance = g_hInst;
    pc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    pc.lpszClassName = kPreviewClass;
    g_previewClassOk = RegisterClassW(&pc) != 0;

    // [keep] typed SetFunctionHook instead of void* casts
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
    g_themeEpoch++;
}

// Mod loaded into an already-running Notepad: attach to existing windows.
void Wh_ModAfterInit() {
    EnumWindows(EnumTopProc, 0);
}

// Full teardown: restore margins, styles, menu, window procs; free GDI objects.
void Wh_ModUninit() {
    g_unloading = true;
    {
        std::lock_guard<std::mutex> lk(g_synMutex);
        for (FaceMap* f : g_faces) {
            f->face->Release();  // [keep] typed COM call
            delete f;
        }
        g_faces.clear();
    }

    // [keep] copy HWNDs under the lock, look contexts up via GetPropW on the window's thread (unload race)
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
    // [keep] unregister only the classes this instance registered, with the same module handle
    if (g_gutterClassOk.load()) UnregisterClassW(kGutterClass, g_hInst);
    if (g_minimapClassOk.load()) UnregisterClassW(kMinimapClass, g_hInst);
    if (g_previewClassOk.load()) UnregisterClassW(kPreviewClass, g_hInst);
}
