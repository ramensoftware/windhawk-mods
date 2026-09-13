// ==WindhawkMod==
// @id              chromium-cursor-remap
// @name            Chromium Cursor Remap
// @description     Replaces the low-resolution cursors that Chromium browsers and Electron apps draw for CSS cursors like grab, zoom-in and col-resize with your Windows cursor theme, or with your own .cur/.ani files.
// @version         1.0
// @author          mazany
// @github          https://github.com/mazany
// @twitter         https://x.com/tomazany
// @donateUrl       https://ko-fi.com/mazany
// @include         *
// @exclude         explorer.exe
// @exclude         SearchHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         RuntimeBroker.exe
// @exclude         taskhostw.exe
// @exclude         sihost.exe
// @exclude         ctfmon.exe
// @exclude         conhost.exe
// @exclude         cmd.exe
// @exclude         powershell.exe
// @exclude         pwsh.exe
// @exclude         WindowsTerminal.exe
// @exclude         mstsc.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chromium Cursor Remap

Chromium browsers and Electron apps draw ten CSS cursors from bitmaps of their
own instead of asking Windows: `grab`, `grabbing`, `zoom-in`, `zoom-out`,
`cell`, `vertical-text`, `alias`, `copy`, `col-resize` and `row-resize`. Those
bitmaps don't follow your Windows cursor scheme. This mod replaces them with
cursors from that scheme, or with `.cur` / `.ani` files of your choice.

![Chromium's own cursors (top) and the Windows cursors that replace them (bottom), in Microsoft Edge at 150% display scale](https://raw.githubusercontent.com/mazany/windhawk-images/5d2c26f93a023738d3597a6a7c8805a9124dc9b2/chromium-cursor-remap/before-after.png)

Cursors that Chromium already takes from Windows (`default`, `pointer`, `text`,
`wait`, `not-allowed`, the directional resize cursors and so on) are not
touched, and neither are cursor images that a web page sets itself with
`cursor: url(...)`.

## Default replacements

| CSS cursor              | Replaced with                        |
|-------------------------|--------------------------------------|
| grab                    | Link select (`IDC_HAND`)             |
| grabbing                | Move (`IDC_SIZEALL`)                 |
| zoom-in, zoom-out, cell | Precision select (`IDC_CROSS`)       |
| vertical-text           | Text select (`IDC_IBEAM`)            |
| alias, copy             | Normal select (`IDC_ARROW`)          |
| col-resize              | Horizontal resize (`IDC_SIZEWE`)     |
| row-resize              | Vertical resize (`IDC_SIZENS`)       |

## Settings

Each cursor type has two settings:

- **Keep Chromium's own cursor** leaves that type alone.
- **Custom cursor source** takes either an `IDC_*` name - `IDC_ARROW`,
  `IDC_IBEAM`, `IDC_WAIT`, `IDC_CROSS`, `IDC_UPARROW`, `IDC_SIZENWSE`,
  `IDC_SIZENESW`, `IDC_SIZEWE`, `IDC_SIZENS`, `IDC_SIZEALL`, `IDC_NO`,
  `IDC_HAND`, `IDC_APPSTARTING`, `IDC_HELP`, `IDC_PIN`, `IDC_PERSON` - or the
  full path of a `.cur` or `.ani` file, such as `C:\Cursors\grab.ani`. Leave it
  empty for the default above; a source that cannot be loaded also falls back
  to the default.

**Test mode** puts one cursor (by default the busy cursor, `IDC_WAIT`) on every
cursor the mod recognises, whatever the per-type settings say, so you can see at
a glance which ones it catches. If the test cursor cannot be loaded, the
per-type settings stay in force.

**Guess cursors that are not Chromium's own** is off by default; see below.

Settings take effect without restarting the app.

## Tested with

- Microsoft Edge 152 and 153, and Google Chrome 152: all ten cursor types replaced.
- MarkText 0.19.1, an Electron app.
- The cursor images bundled with VS Code, Obsidian, Vivaldi and Steam's built-in
  browser (CEF) were checked to be recognised.

Other Chromium-based browsers and Electron or CEF apps should work the same way.

## How it works

The mod is set to load into every program, because Electron and CEF apps can
have any file name and a list of names misses most of them. It stays only in
the main process of a Chromium-based app, which it recognises from the program
file itself before installing any hook; anywhere else, including Chromium's own
helper processes, it unloads again at once. A few shell and console programs
(Explorer, Windows Terminal, PowerShell and similar) are excluded outright, so
the check does not even run there.

In the processes where it stays, it watches `SetCursor`. The first time a cursor
appears, the mod asks Windows which module and resource it was loaded from,
loads that resource again at 32x32 and compares it with Chromium's ten bundled
images. The answer does not depend on the size the app loaded the cursor at
(checked from 32 to 72 px, which covers display scaling from 100% to 225%). A
web page's own cursor image has no resource behind it and is left alone.

**Guess cursors that are not Chromium's own** also judges cursors with no
resource behind them, by their shape. It is a fallback for an app that creates
Chromium's cursors from images at run time instead of loading them from its
resources; none of the apps above needs it. It can misread a web page's own
cursor that merely looks like a hand, a magnifier or a resize bar, and replace
it, so leave it off unless you need it.

## Known limitations

- 64-bit apps only. On ARM64 Windows the mod is also built for ARM64 apps;
  that has not been tested.
- On screen, only 150% display scaling has been checked; other sizes were
  checked by loading the cursors at those sizes.
- Apps that host WebView2 in visual (composition) mode set the cursor from
  their own process, so their web content keeps Chromium's cursors.
- A CEF app that loads `libcef.dll` only after it has started is not
  recognised.
- Chromium's middle-click autoscroll cursors are not replaced.
- A `.cur` / `.ani` file is read once per app session. After editing a file in
  place, restart the app, or turn the mod off and on again.
- Custom files are loaded at the cursor size set in Windows.

## Compatibility

**Icon Resource Redirect** can also theme cursors, by redirecting the resources
apps load them from; this mod was tested with it enabled. A Chromium cursor that
Icon Resource Redirect has already redirected no longer matches Chromium's own
images, so this mod leaves it as Icon Resource Redirect drew it.

## Troubleshooting

Turn on logging for this mod in Windhawk. Each new cursor gets one log line
naming the module and resource it came from, a hash of its image and the
verdict. If a Chromium cursor stops being replaced after a browser update,
Chromium has probably redrawn it: please report that log line together with the
CSS cursor type in the Windhawk mods repository on GitHub
(ramensoftware/windhawk-mods).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Grab:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant (e.g. IDC_HAND) or path to .cur/.ani file. Leave empty for default (IDC_HAND).
  $name: grab (open hand)
- Grabbing:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_SIZEALL).
  $name: grabbing (closed fist)
- ZoomIn:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_CROSS).
  $name: zoom-in (magnifier +)
- ZoomOut:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_CROSS).
  $name: zoom-out (magnifier -)
- Cell:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_CROSS).
  $name: cell (cross/plus)
- VerticalText:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_IBEAM).
  $name: vertical-text
- Alias:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_ARROW).
  $name: alias (shortcut arrow)
- Copy:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_ARROW).
  $name: copy (arrow + plus)
- ColResize:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_SIZEWE).
  $name: col-resize (horizontal double arrow)
- RowResize:
    - keepOriginal: false
      $name: Keep Chromium's own cursor
    - curFile: ""
      $name: Custom cursor source
      $description: IDC_* constant or .cur/.ani path. Leave empty for default (IDC_SIZENS).
  $name: row-resize (vertical double arrow)
- GuessUnknownCursors: false
  $name: Guess cursors that are not Chromium's own
  $description: "Off: only cursors loaded from a program's resources that match Chromium's bundled cursor images are replaced, and web pages' own cursor images are left alone. On: cursors with no resource behind them are judged by their shape too, which can replace a page's cursor that only looks like a hand or a magnifier."
- TestMode:
    - enabled: false
      $name: Enable test mode
      $description: Replaces every cursor the mod recognises with the test cursor below, whatever the settings above say, so you can see which cursors it catches.
    - testCursor: IDC_WAIT
      $name: Test cursor
      $description: IDC_* constant or path to a .cur/.ani file.
  $name: Test mode
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <map>
#include <set>
#include <string>
#include <cmath>
#include <cstring>

// The CSS cursors Chromium draws from its own resources on Windows.
enum class CssCursorType : int {
    Unknown = 0,
    Grab,
    Grabbing,
    ZoomIn,
    ZoomOut,
    Cell,
    VerticalText,
    Alias,
    Copy,
    ColResize,
    RowResize,
    Blank,     // Fully transparent; only the heuristic says so. Never replaced.
    Count
};

static const wchar_t* CssCursorName(CssCursorType t) {
    switch (t) {
        case CssCursorType::Grab:         return L"grab";
        case CssCursorType::Grabbing:     return L"grabbing";
        case CssCursorType::ZoomIn:       return L"zoom-in";
        case CssCursorType::ZoomOut:      return L"zoom-out";
        case CssCursorType::Cell:         return L"cell";
        case CssCursorType::VerticalText: return L"vertical-text";
        case CssCursorType::Alias:        return L"alias";
        case CssCursorType::Copy:         return L"copy";
        case CssCursorType::ColResize:    return L"col-resize";
        case CssCursorType::RowResize:    return L"row-resize";
        case CssCursorType::Blank:        return L"blank";
        default:                          return L"unknown";
    }
}

// The settings, resolved to cursor handles when they are loaded so that a hook
// never loads anything.
struct ModSettings {
    // NULL keeps Chromium's own cursor.
    HCURSOR replacement[static_cast<int>(CssCursorType::Count)] = {};
    bool guessUnknown = false;
};

// Guards the globals below up to g_settings. Nothing that can call into Win32
// runs while it is held - not an original function, a cursor load or Wh_Log:
// other mods' hooks run inside those calls and may take locks of their own, so
// a call under this lock could deadlock against them.
static SRWLOCK g_lock = SRWLOCK_INIT;

struct SharedLock {
    explicit SharedLock(SRWLOCK& lock) : lock_(lock) { AcquireSRWLockShared(&lock_); }
    ~SharedLock() { ReleaseSRWLockShared(&lock_); }
    SRWLOCK& lock_;
};

struct ExclusiveLock {
    explicit ExclusiveLock(SRWLOCK& lock) : lock_(lock) { AcquireSRWLockExclusive(&lock_); }
    ~ExclusiveLock() { ReleaseSRWLockExclusive(&lock_); }
    SRWLOCK& lock_;
};

static std::set<HCURSOR> g_systemCursors;

// Verdicts by handle, dropped when the handle is destroyed so that a reused
// handle value is classified afresh.
static std::map<HCURSOR, CssCursorType> g_classifiedCursors;

// Handles being classified outside the lock (with a thread count), and a
// counter bumped when one of them is destroyed or the settings change. A
// verdict is cached only if the counter did not move while it was worked out.
static std::map<HCURSOR, int> g_classifying;
static unsigned g_generation = 0;

// Handles inside DestroyCursor/DestroyIcon (with a count for nested calls). No
// verdict is cached for one: its value may already belong to another cursor.
static std::map<HCURSOR, int> g_destroying;

static ModSettings g_settings;

// A SetCursor made from inside SetCursor_Hook, including from another mod's
// hook on the original, goes straight to the original.
static thread_local bool g_inSetCursorHook = false;

struct SetCursorHookScope {
    SetCursorHookScope() { g_inSetCursorHook = true; }
    ~SetCursorHookScope() { g_inSetCursorHook = false; }
};

// Set in Wh_ModBeforeUninit: a hook call still running while the hooks are
// removed goes straight to the original.
static std::atomic<bool> g_unloading{false};

// .cur/.ani files, loaded once per path and never destroyed: one may be the
// cursor on screen. Used only by LoadSettings, under g_loadLock.
static std::map<std::wstring, HCURSOR> g_fileCursors;
static SRWLOCK g_loadLock = SRWLOCK_INIT;

using DestroyCursor_t = decltype(&DestroyCursor);
static DestroyCursor_t DestroyCursor_Original;

// The standard system cursors, by the IDC_* names the settings accept.
struct SystemCursor { const wchar_t* name; LPCWSTR id; };
static const SystemCursor kSystemCursors[] = {
    { L"IDC_ARROW",       IDC_ARROW },
    { L"IDC_IBEAM",       IDC_IBEAM },
    { L"IDC_WAIT",        IDC_WAIT },
    { L"IDC_CROSS",       IDC_CROSS },
    { L"IDC_UPARROW",     IDC_UPARROW },
    { L"IDC_SIZENWSE",    IDC_SIZENWSE },
    { L"IDC_SIZENESW",    IDC_SIZENESW },
    { L"IDC_SIZEWE",      IDC_SIZEWE },
    { L"IDC_SIZENS",      IDC_SIZENS },
    { L"IDC_SIZEALL",     IDC_SIZEALL },
    { L"IDC_NO",          IDC_NO },
    { L"IDC_HAND",        IDC_HAND },
    { L"IDC_APPSTARTING", IDC_APPSTARTING },
    { L"IDC_HELP",        IDC_HELP },
    { L"IDC_PIN",         IDC_PIN },
    { L"IDC_PERSON",      IDC_PERSON },
};

static LPCWSTR ResolveIdcConstant(const std::wstring& name) {
    for (const auto& e : kSystemCursors) {
        if (_wcsicmp(name.c_str(), e.name) == 0) {
            return e.id;
        }
    }
    return NULL;
}

// Loads an IDC_* name or a .cur/.ani path; NULL for an empty or bad source.
static HCURSOR LoadCursorFromSource(const std::wstring& source) {
    if (source.empty()) {
        return NULL;
    }

    LPCWSTR idcId = ResolveIdcConstant(source);
    if (idcId) {
        HCURSOR h = LoadCursorW(NULL, idcId);
        if (!h) {
            Wh_Log(L"Cannot load cursor '%s' (err=%lu)", source.c_str(), GetLastError());
        }
        return h;
    }

    // Not LR_SHARED: it does not share file cursors, and every reload would
    // leak a copy.
    auto cached = g_fileCursors.find(source);
    if (cached != g_fileCursors.end()) {
        return cached->second;
    }
    HCURSOR h = static_cast<HCURSOR>(LoadImageW(
        NULL, source.c_str(), IMAGE_CURSOR,
        0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
    if (h) {
        g_fileCursors[source] = h;
        return h;
    }

    Wh_Log(L"Cannot load cursor '%s' (err=%lu)",
           source.c_str(), GetLastError());
    return NULL;
}

// The shape metrics are relative to the bounding box of the non-transparent
// pixels, so they do not depend on the canvas size.
struct CursorFingerprint {
    int canvasW = 0, canvasH = 0;
    int hotspotX = 0, hotspotY = 0;
    bool isBlank = true;
    uint32_t pixelHash = 0;            // FNV-1a over the BGRA pixels
    int opaqueCount = 0;
    int bbW = 0, bbH = 0;
    float hsRelX = 0.0f, hsRelY = 0.0f;
    int cqTL = 0, cqTR = 0, cqBL = 0, cqBR = 0;  // pixels per quadrant
};

// Renders the cursor into a 32-bpp DIB; withShape adds the heuristic's metrics.
static bool ComputeFingerprint(HCURSOR hCursor, CursorFingerprint& fp,
                               bool withShape = true) {
    ICONINFO ii = {};
    if (!GetIconInfo(hCursor, &ii)) {
        return false;
    }
    fp.hotspotX = static_cast<int>(ii.xHotspot);
    fp.hotspotY = static_cast<int>(ii.yHotspot);

    HBITMAP hbm = ii.hbmColor ? ii.hbmColor : ii.hbmMask;
    if (hbm) {
        BITMAP bm = {};
        if (GetObject(hbm, sizeof(bm), &bm)) {
            fp.canvasW = bm.bmWidth;
            fp.canvasH = bm.bmHeight;
            if (!ii.hbmColor) fp.canvasH /= 2;  // Monochrome mask is 2x
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);

    if (fp.canvasW <= 0 || fp.canvasH <= 0) return false;

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return false;
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) { ReleaseDC(NULL, hdcScreen); return false; }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = fp.canvasW;
    bmi.bmiHeader.biHeight = -fp.canvasH;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hDib = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS,
                                    &pBits, NULL, 0);
    if (!hDib || !pBits) {
        DeleteDC(hdcMem); ReleaseDC(NULL, hdcScreen); return false;
    }

    HGDIOBJ hOld = SelectObject(hdcMem, hDib);
    int dataSize = fp.canvasW * fp.canvasH * 4;
    memset(pBits, 0, dataSize);

    if (!DrawIconEx(hdcMem, 0, 0, hCursor, fp.canvasW, fp.canvasH,
                    0, NULL, DI_NORMAL)) {
        SelectObject(hdcMem, hOld);
        DeleteObject(hDib); DeleteDC(hdcMem); ReleaseDC(NULL, hdcScreen);
        return false;
    }
    GdiFlush();

    BYTE* px = static_cast<BYTE*>(pBits);
    uint32_t hash = 0x811c9dc5u;
    int bbL = fp.canvasW, bbT = fp.canvasH, bbR = -1, bbB = -1;

    for (int y = 0; y < fp.canvasH; y++) {
        for (int x = 0; x < fp.canvasW; x++) {
            int i = (y * fp.canvasW + x) * 4;
            for (int k = 0; k < 4; k++) {
                hash ^= px[i + k];
                hash *= 0x01000193u;
            }
            if ((px[i] | px[i+1] | px[i+2] | px[i+3]) != 0) {
                fp.isBlank = false;
                fp.opaqueCount++;
                if (x < bbL) bbL = x;
                if (x > bbR) bbR = x;
                if (y < bbT) bbT = y;
                if (y > bbB) bbB = y;
            }
        }
    }
    fp.pixelHash = hash;

    if (withShape && !fp.isBlank && bbR >= bbL && bbB >= bbT) {
        fp.bbW = bbR - bbL + 1;
        fp.bbH = bbB - bbT + 1;

        if (fp.bbW > 0) fp.hsRelX = (float)(fp.hotspotX - bbL) / fp.bbW;
        if (fp.bbH > 0) fp.hsRelY = (float)(fp.hotspotY - bbT) / fp.bbH;

        float cx = bbL + fp.bbW / 2.0f;
        float cy = bbT + fp.bbH / 2.0f;
        for (int y = 0; y < fp.canvasH; y++) {
            for (int x = 0; x < fp.canvasW; x++) {
                int i = (y * fp.canvasW + x) * 4;
                if ((px[i] | px[i+1] | px[i+2] | px[i+3]) != 0) {
                    if (x < cx) { (y < cy) ? fp.cqTL++ : fp.cqBL++; }
                    else         { (y < cy) ? fp.cqTR++ : fp.cqBR++; }
                }
            }
        }
    }

    SelectObject(hdcMem, hOld);
    DeleteObject(hDib);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    return true;
}

// Chromium's ten bundled cursors as they render when the resource is loaded at
// 32x32, which gives the same pixels whatever size the app loaded it at. The
// same in Chrome, Edge, Electron and CEF builds of Chromium 152.
static const int kCanonicalCursorSize = 32;

struct KnownCursor {
    uint32_t hash;
    int hotspotX, hotspotY;
    CssCursorType type;
};

static const KnownCursor kChromiumCursors[] = {
    {0x860C8213, 13, 13, CssCursorType::Grab},          // IDC_HAND_GRAB
    {0x4362F14D, 13, 13, CssCursorType::Grabbing},      // IDC_HAND_GRABBING
    {0xB04E9BA5, 6, 6, CssCursorType::ZoomIn},          // IDC_ZOOMIN
    {0xB9D3DBA5, 6, 6, CssCursorType::ZoomOut},         // IDC_ZOOMOUT
    {0x728A3F45, 7, 7, CssCursorType::Cell},            // IDC_CELL
    {0x4D9DBAE5, 9, 3, CssCursorType::VerticalText},    // IDC_VERTICALTEXT
    {0x7CA4E2C5, 7, 4, CssCursorType::Alias},           // IDC_ALIAS
    {0x705F6A0C, 7, 4, CssCursorType::Copy},            // IDC_COPYCUR
    {0xAD2C0B45, 10, 8, CssCursorType::ColResize},      // IDC_COLRESIZE
    {0x25B107C5, 9, 10, CssCursorType::RowResize},      // IDC_ROWRESIZE
};

// Shape heuristic for cursors with no resource behind them, used only when
// GuessUnknownCursors is on. The thresholds come from Chromium's own cursors.
static CssCursorType ClassifyByHeuristic(const CursorFingerprint& fp) {
    if (fp.isBlank || fp.opaqueCount == 0) return CssCursorType::Blank;
    if (fp.bbW < 4 || fp.bbH < 4) return CssCursorType::Unknown;

    int total = fp.opaqueCount;
    float cDens = (float)total / (float)(fp.bbW * fp.bbH);
    float asp = (float)fp.bbW / fp.bbH;
    float hx = fp.hsRelX, hy = fp.hsRelY;

    int cL = fp.cqTL + fp.cqBL, cR = fp.cqTR + fp.cqBR;
    int cT = fp.cqTL + fp.cqTR, cB = fp.cqBL + fp.cqBR;
    float symLR = (total > 0) ? 1.0f - std::abs(cL - cR) / (float)total : 0;
    float symTB = (total > 0) ? 1.0f - std::abs(cT - cB) / (float)total : 0;

    // Rule 1: Hotspot at/before left edge of content → arrow-based
    if (hx < 0.10f && hy < 0.25f) {
        return (symLR < 0.57f) ? CssCursorType::Copy : CssCursorType::Alias;
    }
    // Rule 2: Extreme width → vertical-text (asp≈2.70)
    if (asp > 1.8f) return CssCursorType::VerticalText;
    // Rule 3: Low density → resize cursors (cDens≈0.222)
    if (cDens < 0.28f) {
        return (asp > 1.05f) ? CssCursorType::ColResize
                             : CssCursorType::RowResize;
    }
    // Rule 4: Very high symmetry → zoom/cell/grabbing
    if (symLR > 0.93f && symTB > 0.93f) {
        if (cDens > 0.73f) return CssCursorType::Grabbing;
        if (symLR > 0.995f && symTB > 0.995f && cDens < 0.60f)
            return CssCursorType::Cell;
        return (cDens < 0.65f) ? CssCursorType::ZoomIn
                               : CssCursorType::ZoomOut;
    }
    // Rule 5: Moderate symmetry + high density → grab
    if (cDens > 0.45f) return CssCursorType::Grab;

    return CssCursorType::Unknown;
}

// Loads the resource again at 32x32 and looks it up. The copy is never cached,
// so destroying it is safe.
static CssCursorType ClassifyResourceCursor(HMODULE module, LPCWSTR name,
                                            uint32_t& hash) {
    HCURSOR canonical = static_cast<HCURSOR>(LoadImageW(
        module, name, IMAGE_CURSOR, kCanonicalCursorSize, kCanonicalCursorSize, 0));
    if (!canonical) {
        return CssCursorType::Unknown;
    }
    CursorFingerprint fp = {};
    bool fingerprinted = ComputeFingerprint(canonical, fp, false);
    // The original is unset when this runs before the hooks exist.
    (DestroyCursor_Original ? DestroyCursor_Original : DestroyCursor)(canonical);
    if (!fingerprinted) {
        return CssCursorType::Unknown;
    }

    hash = fp.pixelHash;
    if (fp.canvasW != kCanonicalCursorSize || fp.canvasH != kCanonicalCursorSize) {
        return CssCursorType::Unknown;
    }
    for (const auto& known : kChromiumCursors) {
        if (known.hash == fp.pixelHash && known.hotspotX == fp.hotspotX &&
            known.hotspotY == fp.hotspotY) {
            return known.type;
        }
    }
    return CssCursorType::Unknown;
}

// Classifies a cursor by the resource it was loaded from; a cursor with no
// resource behind it is left alone unless guessUnknown is set.
static CssCursorType ClassifyCursor(HCURSOR hCursor, bool guessUnknown) {
    ICONINFOEXW info = {sizeof(info)};
    if (!GetIconInfoExW(hCursor, &info)) {
        Wh_Log(L"cursor %p: GetIconInfoExW failed (err=%lu)", hCursor, GetLastError());
        return CssCursorType::Unknown;
    }
    if (info.hbmColor) DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);

    if (info.szModName[0] && (info.wResID || info.szResName[0])) {
        HMODULE module = GetModuleHandleW(info.szModName);
        CssCursorType type = CssCursorType::Unknown;
        uint32_t hash = 0;
        if (module) {
            type = ClassifyResourceCursor(
                module, info.wResID ? MAKEINTRESOURCEW(info.wResID) : info.szResName, hash);
        }
        std::wstring resource = info.wResID ? L"#" + std::to_wstring(info.wResID)
                                            : std::wstring(info.szResName);
        Wh_Log(L"cursor %p: %s %s hash=0x%08X -> %s", hCursor, info.szModName,
               resource.c_str(), hash, module ? CssCursorName(type) : L"module not loaded");
        return type;
    }

    if (!guessUnknown) {
        Wh_Log(L"cursor %p: no resource, left alone", hCursor);
        return CssCursorType::Unknown;
    }

    CursorFingerprint fp = {};
    if (!ComputeFingerprint(hCursor, fp)) {
        return CssCursorType::Unknown;
    }
    CssCursorType type = ClassifyByHeuristic(fp);
    Wh_Log(L"cursor %p: no resource, %dx%d, guessed -> %s", hCursor, fp.canvasW,
           fp.canvasH, CssCursorName(type));
    return type;
}

// Exported by the executable of every Chromium browser, WebView2 host,
// Electron app and sandboxed CEF host. Checked instead of DLL names: the
// executable is mapped before Wh_ModInit, chrome.dll or msedge.dll may not be
// yet, and Edge and Electron rename them.
static const char* kChromiumExeExports[] = {
    "GetHandleVerifier",
    "IsSandboxedProcess",
};

static BOOL CALLBACK StopAtFirstResource(HMODULE, LPCWSTR, LPWSTR, LONG_PTR found) {
    *(bool*)found = true;
    return FALSE;
}

// The exports alone also match programs that only borrowed Chromium's sandbox
// (JetBrains IDE launchers, Acrobat, Firefox-based browsers), so the executable
// must also carry cursor resources (Edge, WebView2, Electron) or import a
// *_elf.dll (Chrome and its forks).
static bool HasChromiumExeMarkers(HMODULE exe) {
    for (const auto* name : kChromiumExeExports) {
        if (!GetProcAddress(exe, name)) return false;
    }

    bool hasCursors = false;
    EnumResourceNamesW(exe, RT_GROUP_CURSOR, StopAtFirstResource,
                       (LONG_PTR)&hasCursors);
    if (hasCursors) return true;

    // Fails closed on anything that is not a well-formed PE32+ image.
    BYTE* base = (BYTE*)exe;
    auto dos = (IMAGE_DOS_HEADER*)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return false;
    auto nt = (IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_IMPORT) {
        return false;
    }
    const DWORD imageSize = nt->OptionalHeader.SizeOfImage;
    const auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress || dir.VirtualAddress >= imageSize) return false;
    for (DWORD at = dir.VirtualAddress;
         at + sizeof(IMAGE_IMPORT_DESCRIPTOR) <= imageSize;
         at += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        auto d = (IMAGE_IMPORT_DESCRIPTOR*)(base + at);
        if (!d->Name) break;
        if (d->Name >= imageSize) return false;
        const char* dll = (const char*)(base + d->Name);
        size_t len = strnlen(dll, imageSize - d->Name);
        if (len == imageSize - d->Name) return false;
        if (len > 8 && _strnicmp(dll + len - 8, "_elf.dll", 8) == 0) return true;
    }
    return false;
}

static bool IsChromiumProcess() {
    if (HasChromiumExeMarkers(GetModuleHandleW(NULL))) {
        return true;
    }
    // A CEF host without the sandbox exports; seen only if loaded before init.
    return GetModuleHandleW(L"libcef.dll") != NULL;
}

// A non-empty --type= switch marks a Chromium child process, which never owns
// the windows. Not a substring search: a URL or path on the browser's command
// line can contain "--type=". Follows base::CommandLine::ParseFromString: split
// as CommandLineToArgvW does, trim, stop at "--" or --single-argument, switch
// prefixes "--", "-" and "/", names lowercased, last value wins.
static bool IsChromiumWhitespace(wchar_t c) {
    return (c >= 0x09 && c <= 0x0D) || c == 0x20 || c == 0x85 ||
           c == 0xA0 || c == 0x1680 || (c >= 0x2000 && c <= 0x200A) ||
           c == 0x2028 || c == 0x2029 || c == 0x202F || c == 0x205F ||
           c == 0x3000;
}

static bool HasChromiumTypeSwitch(const wchar_t* commandLine) {
    const wchar_t* p = commandLine;
    while (IsChromiumWhitespace(*p)) p++;

    // argv[0]: quotes delimit the program name and backslashes are literal.
    // Unquoted, it ends at (and consumes) any character up to U+0020, while
    // later arguments split on space and tab only.
    if (*p == L'"') {
        p++;
        while (*p && *p != L'"') p++;
        if (*p) p++;
    } else {
        while (*p > L' ') p++;
        if (*p) p++;
    }

    bool isChild = false;
    std::wstring arg;
    for (;;) {
        while (*p == L' ' || *p == L'\t') p++;
        if (!*p) break;

        arg.clear();
        bool quoted = false;
        while (*p && (quoted || (*p != L' ' && *p != L'\t'))) {
            size_t backslashes = 0;
            while (*p == L'\\') { backslashes++; p++; }
            if (*p == L'"') {
                arg.append(backslashes / 2, L'\\');
                if (backslashes % 2) {
                    arg += L'"';
                } else if (quoted && p[1] == L'"') {
                    // CommandLineToArgvW: "" inside quotes is a literal quote
                    // and also ends the quoted run.
                    arg += L'"';
                    quoted = false;
                    p++;
                } else {
                    quoted = !quoted;
                }
                p++;
            } else {
                arg.append(backslashes, L'\\');
                if (*p && (quoted || (*p != L' ' && *p != L'\t'))) arg += *p++;
            }
        }

        size_t begin = 0, end = arg.size();
        while (begin < end && IsChromiumWhitespace(arg[begin])) begin++;
        while (end > begin && IsChromiumWhitespace(arg[end - 1])) end--;
        arg = arg.substr(begin, end - begin);

        if (arg == L"--") break;

        size_t prefix = (arg.compare(0, 2, L"--") == 0) ? 2
                      : (!arg.empty() && (arg[0] == L'-' || arg[0] == L'/')) ? 1
                      : 0;
        if (prefix == 0 || prefix == arg.size()) continue;

        size_t eq = arg.find(L'=');
        std::wstring name = arg.substr(prefix, eq == std::wstring::npos
                                                   ? std::wstring::npos
                                                   : eq - prefix);
        if (name == L"single-argument") break;

        for (auto& c : name) {
            if (c >= L'A' && c <= L'Z') c = c - L'A' + L'a';
        }
        if (name == L"type") {
            isChild = eq != std::wstring::npos && eq + 1 < arg.size();
        }
    }
    return isChild;
}

static bool IsChromiumChildProcess() {
    return HasChromiumTypeSwitch(GetCommandLineW());
}

// Under g_lock.
static bool IsSystemCursor(HCURSOR h) {
    return g_systemCursors.count(h) > 0;
}

static void CacheSystemCursors() {
    for (const auto& e : kSystemCursors) {
        HCURSOR h = LoadCursorW(NULL, e.id);
        if (h) g_systemCursors.insert(h);
    }
}

static const wchar_t kKeepOriginalKey[] = L"keepOriginal";

static std::wstring ReadStringSetting(const wchar_t* name) {
    PCWSTR val = Wh_GetStringSetting(name);
    std::wstring result = val ? val : L"";
    if (val) Wh_FreeStringSetting(val);
    return result;
}

// Reads the settings and resolves every replacement to a handle. Loads
// cursors, so it must not run under g_lock.
static ModSettings LoadSettings() {
    ExclusiveLock loadLock(g_loadLock);
    ModSettings next;

    next.guessUnknown = (Wh_GetIntSetting(L"GuessUnknownCursors") != 0);

    // Test mode overrides every per-type setting.
    HCURSOR testCursor = NULL;
    if (Wh_GetIntSetting(L"TestMode.enabled") != 0) {
        std::wstring testSource = ReadStringSetting(L"TestMode.testCursor");
        if (testSource.empty()) testSource = L"IDC_WAIT";
        testCursor = LoadCursorFromSource(testSource);
        Wh_Log(L"Test mode: '%s' -> %p", testSource.c_str(), testCursor);
    }

    struct TypeSetting { CssCursorType type; const wchar_t* section; LPCWSTR defaultId; };
    static const TypeSetting kTypes[] = {
        {CssCursorType::Grab,         L"Grab",         IDC_HAND},
        {CssCursorType::Grabbing,     L"Grabbing",     IDC_SIZEALL},
        {CssCursorType::ZoomIn,       L"ZoomIn",       IDC_CROSS},
        {CssCursorType::ZoomOut,      L"ZoomOut",      IDC_CROSS},
        {CssCursorType::Cell,         L"Cell",         IDC_CROSS},
        {CssCursorType::VerticalText, L"VerticalText", IDC_IBEAM},
        {CssCursorType::Alias,        L"Alias",        IDC_ARROW},
        {CssCursorType::Copy,         L"Copy",         IDC_ARROW},
        {CssCursorType::ColResize,    L"ColResize",    IDC_SIZEWE},
        {CssCursorType::RowResize,    L"RowResize",    IDC_SIZENS},
    };
    for (const auto& t : kTypes) {
        HCURSOR& slot = next.replacement[static_cast<int>(t.type)];
        if (testCursor) {
            slot = testCursor;
            continue;
        }

        wchar_t key[128];
        // Wh_GetIntSetting returns 0 for an absent key, which must mean
        // "replace": the default of the key is false.
        wsprintfW(key, L"%s.%s", t.section, kKeepOriginalKey);
        if (Wh_GetIntSetting(key) != 0) {
            continue;
        }

        wsprintfW(key, L"%s.curFile", t.section);
        std::wstring source = ReadStringSetting(key);
        slot = LoadCursorFromSource(source);
        if (!slot) {
            slot = LoadCursorW(NULL, t.defaultId);
        }
        if (!source.empty()) {
            Wh_Log(L"%s: '%s' -> %p", CssCursorName(t.type), source.c_str(), slot);
        }
    }
    return next;
}

using SetCursor_t = decltype(&SetCursor);
static SetCursor_t SetCursor_Original;

static HCURSOR WINAPI SetCursor_Hook(HCURSOR hCursor) {
    if (!hCursor || g_inSetCursorHook || g_unloading)
        return SetCursor_Original(hCursor);
    SetCursorHookScope scope;

    // System cursors and our own replacements pass through. The replacement
    // slots of Unknown, Blank and kept types are NULL.
    bool classify = false;
    HCURSOR hRepl = NULL;
    {
        SharedLock lock(g_lock);
        bool passThrough = IsSystemCursor(hCursor);
        for (HCURSOR r : g_settings.replacement) {
            passThrough = passThrough || r == hCursor;
        }
        if (!passThrough) {
            auto it = g_classifiedCursors.find(hCursor);
            if (it != g_classifiedCursors.end()) {
                hRepl = g_settings.replacement[static_cast<int>(it->second)];
            } else {
                classify = true;
            }
        }
    }

    if (classify) {
        // Classifying loads and draws cursors, so it runs outside the lock.
        bool guessUnknown;
        unsigned generation;
        {
            ExclusiveLock lock(g_lock);
            ++g_classifying[hCursor];
            generation = g_generation;
            guessUnknown = g_settings.guessUnknown;
        }
        CssCursorType type = ClassifyCursor(hCursor, guessUnknown);
        {
            ExclusiveLock lock(g_lock);
            auto it = g_classifying.find(hCursor);  // registered above
            if (--it->second == 0) {
                g_classifying.erase(it);
            }
            // A verdict worked out while the handle was destroyed or the
            // settings changed may be stale: it is used neither now nor later.
            if (generation == g_generation && !g_destroying.count(hCursor)) {
                g_classifiedCursors.emplace(hCursor, type);
                hRepl = g_settings.replacement[static_cast<int>(type)];
            }
        }
    }
    return SetCursor_Original(hRepl ? hRepl : hCursor);
}

// A destroyed handle value can come back for a different cursor, such as a
// page's own cursor image. The verdict is dropped before the original runs and
// again after it, and none is cached in between (see g_destroying).
using DestroyIcon_t = decltype(&DestroyIcon);
static DestroyIcon_t DestroyIcon_Original;

static void ForgetCursor(HCURSOR hCursor, bool beforeDestroy) {
    if (g_unloading) {
        return;
    }
    ExclusiveLock lock(g_lock);
    g_classifiedCursors.erase(hCursor);
    if (beforeDestroy) {
        ++g_destroying[hCursor];
    } else {
        auto it = g_destroying.find(hCursor);
        if (it != g_destroying.end() && --it->second == 0) {
            g_destroying.erase(it);
        }
    }
    if (g_classifying.count(hCursor)) {
        ++g_generation;
    }
}

static BOOL WINAPI DestroyCursor_Hook(HCURSOR hCursor) {
    ForgetCursor(hCursor, true);
    BOOL result = DestroyCursor_Original(hCursor);
    ForgetCursor(hCursor, false);
    return result;
}

static BOOL WINAPI DestroyIcon_Hook(HICON hIcon) {
    ForgetCursor(static_cast<HCURSOR>(hIcon), true);
    BOOL result = DestroyIcon_Original(hIcon);
    ForgetCursor(static_cast<HCURSOR>(hIcon), false);
    return result;
}

BOOL Wh_ModInit() {
    if (!IsChromiumProcess() || IsChromiumChildProcess()) {
        return FALSE;
    }

    g_settings = LoadSettings();
    CacheSystemCursors();

    // Every hook is required: without the destroy hooks a verdict could outlive
    // its cursor. No hook is live before Wh_ModInit returns, so giving up here
    // leaves nothing behind.
    //
    // Windows 11 exports DestroyCursor and DestroyIcon at one address, where a
    // second hook fails. Compared through GetProcAddress because &DestroyIcon
    // and &DestroyCursor are distinct import stubs inside the mod.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    void* destroyCursor = user32 ? (void*)GetProcAddress(user32, "DestroyCursor") : nullptr;
    void* destroyIcon = user32 ? (void*)GetProcAddress(user32, "DestroyIcon") : nullptr;
    if (!destroyCursor || !destroyIcon) {
        Wh_Log(L"user32 destroy exports not found, unloading");
        return FALSE;
    }
    bool separateDestroyIcon = destroyIcon != destroyCursor;
    struct { void* target; void* hook; void** original; const wchar_t* name; }
    hooks[] = {
        {(void*)SetCursor, (void*)SetCursor_Hook,
         (void**)&SetCursor_Original, L"SetCursor"},
        {destroyCursor, (void*)DestroyCursor_Hook,
         (void**)&DestroyCursor_Original, L"DestroyCursor"},
        {destroyIcon, (void*)DestroyIcon_Hook,
         (void**)&DestroyIcon_Original, L"DestroyIcon"},
    };
    for (const auto& h : hooks) {
        if (h.hook == (void*)DestroyIcon_Hook && !separateDestroyIcon) {
            continue;
        }
        if (!Wh_SetFunctionHook(h.target, h.hook, h.original)) {
            Wh_Log(L"%s hook failed, unloading", h.name);
            return FALSE;
        }
    }

    Wh_Log(L"Chromium browser process, hooks registered (guess=%d)", g_settings.guessUnknown);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // g_fileCursors is not destroyed: one may still be on screen. Emptied under
    // the lock so that a hook call already holding it finishes first.
    std::map<HCURSOR, CssCursorType> classified;
    std::set<HCURSOR> system;
    AcquireSRWLockExclusive(&g_lock);
    classified.swap(g_classifiedCursors);
    system.swap(g_systemCursors);
    ReleaseSRWLockExclusive(&g_lock);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    // Verdicts are dropped, including any being worked out, because
    // GuessUnknownCursors decides some of them. Freed after the lock is released.
    ModSettings next = LoadSettings();
    std::map<HCURSOR, CssCursorType> dropped;
    AcquireSRWLockExclusive(&g_lock);
    g_settings = next;
    dropped.swap(g_classifiedCursors);
    ++g_generation;
    ReleaseSRWLockExclusive(&g_lock);
}
