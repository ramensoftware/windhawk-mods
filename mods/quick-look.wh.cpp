// ==WindhawkMod==
// @id              quick-look
// @name            Quick Look
// @description     Preview the file or folder selected in File Explorer with Space, in a panel docked inside the window.
// @version         1.0.0
// @author          David Ravelo
// @github          https://github.com/DavidRaveloU
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luser32 -lgdi32 -lcomctl32 -luuid -luxtheme -ldwmapi -lwindowscodecs -lmsimg32 -ladvapi32 -lshlwapi -lmfplat -lmfreadwrite -lmfuuid -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- reflowColumns: true
  $name: Shrink columns to fit the panel
  $description: >-
    Resize the file view and its Details columns to reserve space for the
    panel, like File Pilot. When disabled, the panel overlays the columns.
- textRememberOptions: true
  $name: Remember the text preview options
  $description: >-
    Keep word wrap, line numbers, whitespace dots and alternating rows as you
    left them when another window or a new session opens. When disabled, every
    panel starts with the defaults below.
- textLineNumbers: false
  $name: Line numbers by default
  $description: >-
    Show the line number gutter when a text file is previewed.
- textZebra: false
  $name: Alternating rows by default
  $description: >-
    Tint every other source line when a text file is previewed.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Quick Look

A native Quick Look preview for Windows File Explorer, packed as a [Windhawk](https://windhawk.net) mod.

Select a file or a folder and press **Space**. That is the whole idea.

![Quick Look panel docked in File Explorer](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/preview.png)

Quick Look docks a real panel **inside the File Explorer window**, next to the file list, and previews
whatever is selected. It is not a floating window: it is a child of the Explorer tab, so it moves with
it, is clipped to it and, if you want, shrinks the Details columns to reserve its own space, the way
File Pilot's inspector does.

Folders open with **Explorer's own file list**, not a copy: the same icons, columns, sorting, view modes
and scrollbars. Text, images, video, audio, PDF, Word, Excel, PowerPoint, archives and everything else
have their own preview or a proper fallback. Nothing runs on a timer in the background, and nothing that
touches the disk ever blocks Explorer.

## Highlights

- **A panel, not a window.** Space opens it, Space or Esc closes it, the header has a close button for the
  mouse. It docks on the right, or at the bottom when the window is too narrow; drag its edge to change
  its width and the width is remembered.
- **Folders, with Explorer's own list.** The preview hosts a real Explorer browser, so icons, columns,
  sorting, grouping and the Details / List / Icons view modes are Explorer's. A view dropdown in the
  header switches between Details, List, Small, Medium, Large and Extra Large icons.
- **A real filter, at the same level.** Typing in the filter box shows only the matching direct children
  of the previewed folder, with their original columns, and clearing it brings the folder back. No
  subfolder is ever searched.
- **Text and code, detected by content.** No extension list: a file is text when its bytes say so, so
  `.ahk`, `.psd1`, `.config`, extensionless files and anything else just work. UTF-8, UTF-16 LE/BE and the
  ANSI code page are decoded, and the footer reports which one was used, the size and the line count.
- **An editor-style toolbar.** Word wrap, line numbers (a wrapped continuation keeps its number once), a
  whitespace inspector that draws spaces as dim dots, and alternating row colors. The four toggles are
  remembered if you want them remembered, and the defaults are settings.
- **Search inside the file.** The same filter box keeps only the matching lines, each with its original
  line number, and highlights every match with your Windows accent color.
- **Documents without Office.** Word, Excel, PowerPoint, RTF and OpenDocument files are opened as a zip
  of XML parts and reduced to their text, so they preview instantly even when Office is not installed.
  Old binary `.doc`, `.xls` and `.ppt` files use the Windows preview handler when one is registered.
- **Images, video and audio.** Images are decoded at full resolution, videos show a frame and audio shows
  its album art, both through the same still-image providers Windows uses.
- **PDF.** Rendered by the Windows PDF preview handler, the same component Explorer's preview pane uses,
  hosted inside the panel. Zoom and scroll belong to the handler.
- **Archives.** A `.zip` (and any format the shell exposes as a folder) lists its contents in the native
  list. Formats with no shell namespace fall back to the properties view.
- **A fallback that still helps.** Anything else shows the real file icon plus its type, size, modified
  date and full path.
- **Colors that follow Windows.** The panel samples Explorer's actual pixels for the background and the
  text, and takes the accent from Windows itself, so light, dark and custom accents look native.
- **Explorer never freezes.** Listing a folder, reading a file, decoding an image, building a thumbnail or
  checking a cloud placeholder all run on worker threads. The panel opens at once with a loading state,
  a new selection cancels the previous load, and a file that is only a cloud placeholder is never
  downloaded just to be previewed.

A few of the things it previews:

![Quick Look archive preview](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/archive.png)

![Quick Look text preview](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/text.png)

![Quick Look image preview](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/images.png)

![Quick Look PDF preview](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/pdf.png)

![Quick Look document preview](https://raw.githubusercontent.com/DavidRaveloU/Quick-Look-Windhawk/main/assets/office.png)

## Install

1. Install [Windhawk](https://windhawk.net) (Windows 10 or 11).
2. Open [Quick Look on windhawk.net](https://windhawk.net/mods/quick-look) and press **Install**.
3. Select a file or folder in File Explorer and press **Space**.

Quick Look is an **injected** mod: it runs inside `explorer.exe`, which is what lets the panel be a real
child of the window instead of a window that follows it.

## Keys

| Key | Action |
| --- | --- |
| **Space** | Open or close the preview |
| **Esc** | Close the preview |
| **Click the close button** | Close the preview with the mouse |
| **Shift + wheel** | Scroll a text preview sideways when word wrap is off |
| **Mouse wheel / drag the bars** | Scroll the preview |

## Settings

| Setting | Default | What it does |
| --- | --- | --- |
| Shrink columns to fit the panel | On | Shrinks the file view and scales the Details columns to the space left by the panel. Turn it off to overlay the panel instead. |
| Remember the text preview options | On | Keeps wrap, line numbers, whitespace dots and alternating rows as you left them for the next window or session. |
| Line numbers by default | Off | The gutter used when the options are not remembered. |
| Alternating rows by default | Off | The alternating row color used when the options are not remembered. |

## What each file shows

| Selection | Preview |
| --- | --- |
| Folder | Explorer's native list, with a view dropdown, a name filter and file/folder counts |
| Text, code, config, logs | Text with wrap, line numbers, whitespace dots, alternating rows and in-file search |
| Word, Excel, PowerPoint, RTF, OpenDocument | The document text, read directly without Office |
| Old `.doc`, `.xls`, `.ppt` | The Windows preview handler when one is installed, otherwise the properties view |
| Images (`png`, `jpg`, `gif`, `bmp`, `tif`, `ico`, `webp`…) | The decoded image, fitted to the panel |
| Video and audio | A frame or the album art, as Windows produces it |
| PDF | The Windows PDF preview handler, hosted in the panel |
| Archives (`.zip`, and formats with a shell namespace) | The archive contents in the native list |
| Anything else | The real file icon with type, size, modified date and path |

## How it behaves

- **The panel never navigates.** Double-clicking a folder inside the preview opens its **Properties**;
  it does not enter it. Quick Look is a preview, not a file manager.
- **Single click selects.** A double click on empty space does nothing.
- **Cloud files are left alone.** A file that is only a placeholder is not downloaded to be previewed.
- **Text is capped.** A text preview reads the first 200 KB and says so in the footer.

## Troubleshooting

- **Space does nothing.** The panel only opens when the file list itself has the focus, so a pending
  rename, an address bar edit or the search box is never hijacked. Click the file list and try again.
- **Something looks wrong.** Turn on the Windhawk log for this mod (Advanced tab), reproduce it and
  attach the log to the issue. It is off by default because it adds a line for every step.
- **A document shows only text.** That is by design: modern Office and OpenDocument files are read as
  text, so layout, images and colors are not part of the preview.

## Contributing

Bug reports, fixes and new features are welcome. Open an
[issue](https://github.com/DavidRaveloU/Quick-Look-Windhawk/issues) with your Windows build, the mod
version, the steps to reproduce it and, when it is visual, a screenshot or a short clip.

For a pull request: fork the
[repository](https://github.com/DavidRaveloU/Quick-Look-Windhawk), branch from `main`, change
`quick-look.wh.cpp`, confirm it compiles and open the PR describing **what** changed and **why**. One bug
or feature per PR is much easier to review.

## License

Released under the **MIT License**. See the
[license file](https://github.com/DavidRaveloU/Quick-Look-Windhawk/blob/main/LICENSE).
The inflate code that reads Word and OpenDocument files is derived from zlib's `puff.c` and keeps its
zlib license notice in the source.

Copyright (c) 2026-present David Ravelo.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <exdisp.h>
#include <ocidl.h>
#include <servprov.h>
#include <oleidl.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <winrt/Windows.UI.ViewManagement.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <vector>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::wstring Fmt(const wchar_t* format, ...) {
    wchar_t buffer[2048];
    va_list args;
    va_start(args, format);
    _vsnwprintf(buffer, ARRAYSIZE(buffer) - 1, format, args);
    buffer[ARRAYSIZE(buffer) - 1] = L'\0';
    va_end(args);
    return buffer;
}

// GetDpiForWindow exists since Windows 10 1607, which is the oldest version
// Windhawk supports, so there is nothing to look up at runtime.
static UINT GetDpiForWindowSafe(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    if (dpi) {
        return dpi;
    }
    HDC dc = GetDC(nullptr);
    int fallback = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) {
        ReleaseDC(nullptr, dc);
    }
    return fallback ? fallback : 96;
}

static int Scale(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

static std::wstring ClassNameOf(HWND hwnd) {
    wchar_t name[128] = {};
    if (hwnd) {
        GetClassNameW(hwnd, name, ARRAYSIZE(name));
    }
    return name;
}

static bool IsTextInputClass(const wchar_t* name) {
    return _wcsicmp(name, L"Edit") == 0 ||
           _wcsicmp(name, L"RichEdit") == 0 ||
           _wcsicmp(name, L"RICHEDIT50W") == 0 ||
           _wcsicmp(name, L"ComboBox") == 0 ||
           _wcsicmp(name, L"ComboBoxEx32") == 0;
}

static bool IsExplorerClass(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    std::wstring name = ClassNameOf(hwnd);
    return _wcsicmp(name.c_str(), L"CabinetWClass") == 0 ||
           _wcsicmp(name.c_str(), L"ExploreWClass") == 0;
}

template <typename T>
static void ReleaseCom(T*& pointer) {
    if (pointer) {
        pointer->Release();
        pointer = nullptr;
    }
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// Fixed names: the classes are registered once in Wh_ModInit and removed in
// Wh_ModUninit. A name still taken at init means a copy of the mod did not
// unload, and creating a window with it would call into unmapped code, so
// registration failure is fatal instead of tolerated.
static const wchar_t* g_panelClass = L"QuickLookPanel";
static const wchar_t* g_menuClass = L"QuickLookViewMenu";
static const UINT_PTR QL_TIMER_SELECTION = 1;
static const UINT_PTR QL_TIMER_BROWSER_COLUMNS = 2;
static const UINT_PTR QL_TIMER_LIVE_LAYOUT = 4;
static const UINT_PTR QL_TIMER_FILTER = 5;
// One-shot pass that themes and repaints a freshly filled results view.
static const UINT_PTR QL_TIMER_BROWSER_THEME = 6;
// Reveals the embedded browser once its first frame is ready.
static const UINT_PTR QL_TIMER_COVER = 7;
// Starts the probe once the selection has settled.
static const UINT_PTR QL_TIMER_PROBE = 10;
// Animates the loading indicator.
static const UINT_PTR QL_TIMER_SPINNER = 11;
// Feeds filtered items to the results view in small batches.
static const UINT_PTR QL_TIMER_RESULTS = 12;
static const int kFilterEditId = 1001;
// Posted to the panel when a click or a blocked navigation asks to open an item,
// so the shell dialog is not created inside a browser callback.
static const UINT QL_WM_OPEN_PATH = WM_APP + 20;
// A worker thread finished reading and decoding a text file.
static const UINT QL_WM_TEXT_LOADED = WM_APP + 21;
// A worker thread finished decoding an image.
static const UINT QL_WM_IMAGE_LOADED = WM_APP + 22;
// A worker thread finished probing the selected item.
static const UINT QL_WM_PROBE_DONE = WM_APP + 23;
// Messages sent to Explorer's own top-level windows. They are registered at
// init (RegisterWindowMessageW), so their numbers can never collide with a
// message Explorer, another mod or a future Windows build already uses.
static UINT QL_WM_TOGGLE = 0;
static UINT QL_WM_SETTINGS = 0;
static UINT QL_WM_CLEANUP = 0;

// Wh_ModUninit asks every Explorer window to clean up and waits here until the
// last one is done, so nothing can call into this module after it returns.
static HANDLE g_cleanupEvent = nullptr;
static std::mutex g_cleanupMutex;
static std::set<HWND> g_cleanupPending;

enum class QlView {
    Details,
    List,
    Icons,
};

struct FolderEntry {
    std::wstring name;
    std::wstring path;
    std::wstring type;
    // Cached from the folder enumeration, so the icon helpers below can ask the
    // shell for an icon without touching the file system again.
    DWORD attributes = 0;
    bool isFolder = false;
    ULONGLONG size = 0;
    FILETIME modified = {};
    HICON smallIcon = nullptr;
    HICON largeIcon = nullptr;
};

class NavigationEvents;

struct PanelState {
    HWND top = nullptr;
    HWND tab = nullptr;
    HWND view = nullptr;
    HWND parent = nullptr;
    HWND panel = nullptr;
    RECT viewFull = {};
    RECT parentClient = {};
    bool open = false;
    std::wstring path;
    std::wstring debug;

    IColumnManager* columnManager = nullptr;
    IShellView* shellView = nullptr;
    std::vector<PROPERTYKEY> columnKeys;
    std::vector<UINT> columnWidths;
    bool columnsSaved = false;

    // The Explorer tab's browser object, and the connection that tells the
    // panel the moment that tab starts to navigate (item: columns must be given
    // back before Explorer saves the folder's view settings).
    IWebBrowser2* webBrowser = nullptr;
    IConnectionPoint* navPoint = nullptr;
    DWORD navCookie = 0;
    NavigationEvents* navEvents = nullptr;

    COLORREF background = CLR_INVALID;
    COLORREF text = CLR_INVALID;
    COLORREF accent = CLR_INVALID;

    QlView viewMode = QlView::Details;
    bool folderPreview = false;
    std::wstring folderPath;
    std::vector<FolderEntry> entries;
    std::vector<int> filtered;
    std::wstring filter;
    int fileCount = 0;
    int folderCount = 0;
    int totalFileCount = 0;
    int totalFolderCount = 0;
    int scrollRow = 0;
    int hoverIndex = -1;
    int visibleRows = 0;
    RECT listRect = {};
    RECT searchRect = {};
    RECT searchEditRect = {};
    RECT searchClearRect = {};
    RECT scrollbarRect = {};
    RECT scrollThumbRect = {};

    // Plain-text preview of a file.
    bool textPreview = false;
    bool textLoading = false;
    unsigned loadGeneration = 0;
    std::wstring textRaw;
    std::vector<std::wstring> textLines;
    std::vector<int> textLineNumbers;
    std::vector<int> textLineSources;
    std::vector<std::vector<std::pair<int, int>>> textLineMatches;
    int textMatchCount = 0;
    int textScroll = 0;
    int textVisibleLines = 0;
    int textWrapWidth = 0;
    int textCharWidth = 0;
    int textGutterWidth = 0;
    // Horizontal scrolling, used when word wrap is off. Measured in characters
    // because the text font is monospaced.
    int textScrollX = 0;
    int textMaxChars = 0;
    int textVisibleChars = 0;
    RECT textHScrollRect = {};
    RECT textHThumbRect = {};
    bool draggingHScroll = false;
    int textSourceLines = 0;
    RECT textToolbar[4] = {};
    int textToolbarHover = -1;
    RECT closeRect = {};
    bool closeHover = false;
    ULONGLONG textBytes = 0;
    bool textTruncated = false;
    std::wstring textEncoding;

    // Image, video frame, PDF page or audio art.
    bool mediaPreview = false;
    HBITMAP mediaBitmap = nullptr;
    int mediaWidth = 0;
    int mediaHeight = 0;
    // Pixel size of the original image; the bitmap may be a downscaled copy.
    int mediaSourceWidth = 0;
    int mediaSourceHeight = 0;
    ULONGLONG mediaBytes = 0;
    bool mediaHasAlpha = false;
    std::wstring mediaKind;
    // PDFs use the Windows preview handler, which paints into its own child
    // window instead of a bitmap.
    IPreviewHandler* previewHandler = nullptr;
    bool mediaHandler = false;
    bool mediaPending = false;
    // Archives are previewed as a native folder view; when the shell has no
    // namespace for the format the panel falls back to the properties view.
    bool archivePreview = false;
    HICON propertyIcon = nullptr;
    // The panel draws into this back buffer and blits once, so a scroll repaint
    // never shows the background before the text.
    HDC backDc = nullptr;
    HBITMAP backBitmap = nullptr;
    HGDIOBJ backOldBitmap = nullptr;
    int backWidth = 0;
    int backHeight = 0;
    RECT viewDropdown = {};
    int rowHeight = 0;
    int headerRowHeight = 0;
    int iconColumns = 1;
    int iconItemWidth = 0;
    int iconItemHeight = 0;
    int iconSize = 48;
    bool draggingScroll = false;
    HWND filterEdit = nullptr;
    HFONT filterFont = nullptr;
    HBRUSH filterBrush = nullptr;

    IExplorerBrowser* explorerBrowser = nullptr;
    IExplorerBrowserEvents* browserEvents = nullptr;
    DWORD browserCookie = 0;
    IShellView* browserView = nullptr;
    HWND browserViewWnd = nullptr;
    HWND browserListWnd = nullptr;
    std::wstring browserPath;
    IColumnManager* browserColumnManager = nullptr;
    std::vector<PROPERTYKEY> browserColumnKeys;
    std::vector<UINT> browserColumnWidths;
    bool browserColumnsSaved = false;
    int browserSettleTicks = 0;
    int borderWatchTick = 0;
    int browserExtraReserve = 0;
    int browserFitAttempts = 0;
    bool liveResize = false;
    ULONGLONG lastLiveLayout = 0;
    HWND viewMenuWnd = nullptr;
    RECT searchBoxRect = {};
    bool resultsMode = false;
    // True only while FillFromObject navigates to its own results folder: that
    // navigation is ours, not the user entering a folder from the preview.
    bool fillingResults = false;
    // A fresh browser first paints a light, empty list. A cover window of the
    // panel's own background sits above it until the view exists, is themed and
    // has its first items, so that unfinished frame is never visible.
    HWND coverWnd = nullptr;
    HBRUSH coverBrush = nullptr;
    COLORREF coverColor = CLR_INVALID;
    bool coverActive = false;
    int coverTicks = 0;
    bool browserThemed = false;
    std::wstring resultsFilter;
    std::wstring pendingOpenPath;
    std::wstring lastOpenPath;
    DWORD lastOpenTick = 0;

    // Loading. The selected item is probed on a worker thread; the panel keeps
    // showing the previous preview until the result arrives.
    bool loadPending = false;
    bool probeStarted = false;
    bool loadSlowCleared = false;
    ULONGLONG loadingSince = 0;
    int spinnerFrame = 0;
    RECT loadingRect = {};
    std::shared_ptr<std::atomic<unsigned>> loadToken;
    // Properties view data, gathered by the probe.
    std::wstring propType;
    std::wstring propSize;
    std::wstring propModified;
    std::wstring propNote;
    // Filtered items still to be added to the results view.
    std::vector<int> resultsPending;
    size_t resultsPendingPos = 0;

    bool dockRight = true;
    bool draggingSplitter = false;
    int splitDragStartX = 0;
    int splitDragStartWidth = 0;
};

static std::map<HWND, PanelState*> g_states;
static std::mutex g_stateLock;
// Media Foundation is started once; the video frame fallback needs it.
static std::atomic<bool> g_mediaFoundation{false};
static std::atomic<bool> g_unloading{false};
// Set by Wh_ModUninit: workers stop what they are doing as soon as they see it.
static std::atomic<bool> g_stopWorkers{false};
static std::atomic<bool> g_swallowSpaceUp{false};
static std::atomic<bool> g_reflowColumns{true};
// Text preview toggles. They live for the session, like the panel width.
static std::atomic<bool> g_textWrap{true};
static std::atomic<bool> g_textLineNumbers{false};
static std::atomic<bool> g_textWhitespace{false};
static std::atomic<bool> g_textZebra{false};
// Whether the toggles survive a new window or session, and the defaults used
// when the user asked them to reset.
static std::atomic<bool> g_textRememberOptions{true};
static bool g_defaultTextLineNumbers = false;
static bool g_defaultTextZebra = false;

static void ResetTextOptionsToDefaults() {
    g_textWrap = true;
    g_textLineNumbers = g_defaultTextLineNumbers;
    g_textWhitespace = false;
    g_textZebra = g_defaultTextZebra;
}

static void SaveTextOptions() {
    if (!g_textRememberOptions.load()) {
        return;
    }
    Wh_SetIntValue(L"textWrapState", g_textWrap.load() ? 1 : 0);
    Wh_SetIntValue(L"textNumbersState", g_textLineNumbers.load() ? 1 : 0);
    Wh_SetIntValue(L"textWhitespaceState", g_textWhitespace.load() ? 1 : 0);
    Wh_SetIntValue(L"textZebraState", g_textZebra.load() ? 1 : 0);
}
// Panel width chosen by dragging its edge, in 96-DPI units. 0 = automatic.
static std::atomic<int> g_panelWidthDip{0};
static HINSTANCE g_instance = nullptr;

static void LayoutPanelContent(PanelState* state);
static void CloseState(PanelState* state);
static void DestroyState(PanelState* state, bool removeFromMap);
static LRESULT CALLBACK TopSubclassProc(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam, DWORD_PTR referenceData);
static void ConnectNavigationEvents(PanelState* state);
static void DisconnectNavigationEvents(PanelState* state);
static void ClampScroll(PanelState* state);
static void SyncExplorerBrowser(PanelState* state);
static void DestroyExplorerBrowser(PanelState* state);
static bool EnsureExplorerBrowser(PanelState* state);
static void EnsureBrowserListSubclass(PanelState* state);
static void ApplyBrowserViewMode(PanelState* state);
static void SetExplorerBrowserRect(PanelState* state);
static void ScaleBrowserColumns(PanelState* state);
static void RestoreBrowserColumns(PanelState* state);
static void ReleaseBrowserColumns(PanelState* state);
static void CollectBorderWindows(PanelState* state, std::vector<HWND>& out);
static bool SameFolderPath(const std::wstring& left,
                           const std::wstring& right);
static void UnloadPdfPreview(PanelState* state);
static void CloseViewMenu(PanelState* state);
static COLORREF BlendColor(COLORREF from, COLORREF to, int percent);
static int ColorLuma(COLORREF color);
static COLORREF GetUserAccentColor();
static void UpdateFilterEditVisibility(PanelState* state);
static bool IsWindows11OrLater();
static void DrawViewMenuLineGlyph(HDC dc, const RECT& rect, QlView mode,
                                  int iconSize, COLORREF color, UINT dpi);

// Whether a top-level window has a panel open. It lives on the window itself,
// so the low-level keyboard hook reads it without taking a lock: a hook that
// waits for the UI thread would stall the input of the whole system.
static const wchar_t kPanelOpenProp[] = L"QuickLookPanelOpen";

static bool IsTopWithOpenPanel(HWND top) {
    return top && GetPropW(top, kPanelOpenProp) != nullptr;
}

static void UpdateTopOpenFlag(HWND top) {
    if (!top || !IsWindow(top)) {
        return;
    }
    bool anyOpen = false;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (const auto& entry : g_states) {
            if (entry.second && entry.second->top == top && entry.second->open) {
                anyOpen = true;
                break;
            }
        }
    }
    if (anyOpen) {
        SetPropW(top, kPanelOpenProp, reinterpret_cast<HANDLE>(1));
    } else {
        RemovePropW(top, kPanelOpenProp);
    }
}

// ---------------------------------------------------------------------------
// Folder preview data
// ---------------------------------------------------------------------------

static void ClearEntries(PanelState* state) {
    if (!state) {
        return;
    }
    for (FolderEntry& entry : state->entries) {
        if (entry.smallIcon) {
            DestroyIcon(entry.smallIcon);
        }
        if (entry.largeIcon) {
            DestroyIcon(entry.largeIcon);
        }
    }
    state->entries.clear();
    state->filtered.clear();
    state->fileCount = 0;
    state->folderCount = 0;
    state->totalFileCount = 0;
    state->totalFolderCount = 0;
    state->scrollRow = 0;
    state->hoverIndex = -1;
}

static std::wstring LowerText(std::wstring text) {
    if (!text.empty()) {
        CharLowerBuffW(text.data(), static_cast<DWORD>(text.size()));
    }
    return text;
}

static std::wstring FormatEntryDate(const FILETIME& fileTime) {
    FILETIME localTime = {};
    SYSTEMTIME systemTime = {};
    if (!FileTimeToLocalFileTime(&fileTime, &localTime) ||
        !FileTimeToSystemTime(&localTime, &systemTime)) {
        return L"";
    }
    wchar_t buffer[64] = {};
    GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &systemTime, nullptr,
                   buffer, ARRAYSIZE(buffer));
    return buffer;
}

static std::wstring FormatEntrySize(ULONGLONG size) {
    if (size < 1024) {
        return Fmt(L"%llu B", size);
    }
    if (size < 1024ull * 1024) {
        return Fmt(L"%.1f KB", static_cast<double>(size) / 1024.0);
    }
    if (size < 1024ull * 1024 * 1024) {
        return Fmt(L"%.1f MB",
                   static_cast<double>(size) / (1024.0 * 1024.0));
    }
    return Fmt(L"%.1f GB",
               static_cast<double>(size) / (1024.0 * 1024.0 * 1024.0));
}

// Lists a folder without touching any panel state, so it can run on a worker
// thread. `token` carries the newest selection number: when it moves on, the
// listing stops early instead of finishing work nobody will look at.
static void EnumerateFolderEntries(
    const std::wstring& folderPath, std::vector<FolderEntry>& entries,
    const std::shared_ptr<std::atomic<unsigned>>& token, unsigned generation) {
    std::wstring base = folderPath;
    if (!base.empty() && base.back() != L'\\') {
        base += L'\\';
    }
    std::wstring pattern = base + L'*';

    WIN32_FIND_DATAW data = {};
    HANDLE find = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &data,
                                   FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    unsigned counter = 0;
    do {
        if (wcscmp(data.cFileName, L".") == 0 ||
            wcscmp(data.cFileName, L"..") == 0) {
            continue;
        }
        if (entries.size() >= 5000) {
            break;
        }
        if ((++counter & 63) == 0 &&
            (g_stopWorkers.load() || (token && token->load() != generation))) {
            break;
        }

        FolderEntry entry;
        entry.name = data.cFileName;
        entry.path = base + entry.name;
        entry.attributes = data.dwFileAttributes;
        entry.isFolder =
            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry.size = (static_cast<ULONGLONG>(data.nFileSizeHigh) << 32) |
                     data.nFileSizeLow;
        entry.modified = data.ftLastWriteTime;
        entry.type = entry.isFolder ? L"File folder" : L"File";
        entries.push_back(std::move(entry));
    } while (FindNextFileW(find, &data));
    FindClose(find);

    std::sort(entries.begin(), entries.end(),
              [](const FolderEntry& a, const FolderEntry& b) {
                  if (a.isFolder != b.isFolder) {
                      return a.isFolder > b.isFolder;
                  }
                  return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
              });
}

static void ApplyFilter(PanelState* state) {
    if (!state) {
        return;
    }

    state->filtered.clear();
    std::wstring needle = LowerText(state->filter);
    state->fileCount = 0;
    state->folderCount = 0;

    for (size_t i = 0; i < state->entries.size(); i++) {
        const FolderEntry& entry = state->entries[i];
        if (!needle.empty() &&
            LowerText(entry.name).find(needle) == std::wstring::npos) {
            continue;
        }
        state->filtered.push_back(static_cast<int>(i));
        if (entry.isFolder) {
            state->folderCount++;
        } else {
            state->fileCount++;
        }
    }

    state->scrollRow = 0;
    state->hoverIndex = -1;
}

// Shows the name filter text field whenever a preview can be filtered.
static void UpdateFilterEditVisibility(PanelState* state) {
    if (!state || !state->filterEdit || !IsWindow(state->filterEdit)) {
        return;
    }
    const RECT& edit = state->searchEditRect;
    if ((state->folderPreview || state->textPreview) &&
        !state->archivePreview) {
        if (edit.right > edit.left) {
            SetWindowPos(state->filterEdit, nullptr, edit.left, edit.top,
                         edit.right - edit.left, edit.bottom - edit.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    } else {
        ShowWindow(state->filterEdit, SW_HIDE);
    }
}

// ---------------------------------------------------------------------------
// Text file preview
// ---------------------------------------------------------------------------

static HFONT CreateTextFont(UINT dpi) {
    return CreateFontW(-Scale(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN,
                       L"Consolas");
}

// Windows decides what is text by looking at the bytes, not by trusting the
// extension, and so does this: a file is text when it carries no NUL bytes and
// almost no control characters. That covers .ahk, .psd1, .config, files with no
// extension and every other text format without keeping a list.
static bool LooksLikeText(const char* data, size_t size) {
    if (size == 0) {
        return true;
    }
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data);

    if (size >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        return true;
    }
    if (size >= 2 &&
        ((bytes[0] == 0xFF && bytes[1] == 0xFE) ||
         (bytes[0] == 0xFE && bytes[1] == 0xFF))) {
        return true;
    }

    // A NUL byte is the usual signature of a binary file, with one exception:
    // UTF-16 text without a BOM puts a NUL in every high byte.
    size_t nulEven = 0;
    size_t nulOdd = 0;
    for (size_t i = 0; i < size; i++) {
        if (bytes[i] == 0) {
            if (i & 1) {
                nulOdd++;
            } else {
                nulEven++;
            }
        }
    }
    if (nulEven + nulOdd > 0) {
        size_t high = std::max(nulEven, nulOdd);
        size_t low = std::min(nulEven, nulOdd);
        return high * 4 >= size && low * 16 <= size;
    }

    size_t control = 0;
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = bytes[i];
        if (byte == 0x09 || byte == 0x0A || byte == 0x0D || byte == 0x0C ||
            byte == 0x08) {
            continue;
        }
        if (byte < 0x20 || byte == 0x7F) {
            control++;
        }
    }
    // Up to one control character in fifty: still text.
    return control * 50 <= size;
}

static std::wstring DecodeMultiByte(UINT codePage, DWORD flags,
                                    const char* data, int length) {
    if (length <= 0) {
        return std::wstring();
    }
    int wideLength =
        MultiByteToWideChar(codePage, flags, data, length, nullptr, 0);
    if (wideLength <= 0) {
        return std::wstring();
    }
    std::wstring wide(static_cast<size_t>(wideLength), L'\0');
    MultiByteToWideChar(codePage, flags, data, length, wide.data(), wideLength);
    return wide;
}

// UTF-8 and UTF-16 are recognised by their BOM, plain UTF-8 is validated, and
// anything else falls back to the ANSI code page. A NUL byte in the first
// kilobytes means binary, unless the file declared itself as UTF-16.
static bool DecodeText(const std::vector<char>& data, std::wstring& out,
                       std::wstring& encoding) {
    const size_t size = data.size();
    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(data.data());

    if (size >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        out = DecodeMultiByte(CP_UTF8, 0, data.data() + 3,
                              static_cast<int>(size - 3));
        encoding = L"UTF-8 BOM";
        return true;
    }
    if (size >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        size_t characters = (size - 2) / 2;
        out.resize(characters);
        const unsigned char* source = bytes + 2;
        for (size_t i = 0; i < characters; i++) {
            out[i] = static_cast<wchar_t>(source[i * 2] |
                                          (source[i * 2 + 1] << 8));
        }
        encoding = L"UTF-16 LE";
        return true;
    }
    if (size >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) {
        size_t characters = (size - 2) / 2;
        out.resize(characters);
        const unsigned char* source = bytes + 2;
        for (size_t i = 0; i < characters; i++) {
            out[i] = static_cast<wchar_t>((source[i * 2] << 8) |
                                          source[i * 2 + 1]);
        }
        encoding = L"UTF-16 BE";
        return true;
    }
    if (memchr(data.data(), 0, std::min<size_t>(size, 4096)) != nullptr) {
        // No BOM, but UTF-16 text without a BOM still has a NUL in every high
        // byte. Anything else with NUL bytes is binary.
        size_t nulEven = 0;
        size_t nulOdd = 0;
        for (size_t i = 0; i < size; i++) {
            if (bytes[i] == 0) {
                if (i & 1) {
                    nulOdd++;
                } else {
                    nulEven++;
                }
            }
        }
        bool littleEndian = nulOdd * 4 >= size && nulEven * 16 <= size;
        bool bigEndian = nulEven * 4 >= size && nulOdd * 16 <= size;
        if (!littleEndian && !bigEndian) {
            return false;
        }
        size_t characters = size / 2;
        out.resize(characters);
        for (size_t i = 0; i < characters; i++) {
            out[i] = littleEndian
                         ? static_cast<wchar_t>(bytes[i * 2] |
                                                (bytes[i * 2 + 1] << 8))
                         : static_cast<wchar_t>((bytes[i * 2] << 8) |
                                                bytes[i * 2 + 1]);
        }
        encoding = littleEndian ? L"UTF-16 LE, no BOM" : L"UTF-16 BE, no BOM";
        return true;
    }

    std::wstring utf8 =
        DecodeMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, data.data(),
                        static_cast<int>(size));
    if (!utf8.empty() || size == 0) {
        out = std::move(utf8);
        encoding = L"UTF-8";
        return true;
    }

    std::wstring ansi =
        DecodeMultiByte(CP_ACP, 0, data.data(), static_cast<int>(size));
    if (ansi.empty()) {
        return false;
    }
    out = std::move(ansi);
    encoding = L"ANSI";
    return true;
}

static void NormalizeText(std::wstring& text) {
    std::wstring normalized;
    normalized.reserve(text.size() + text.size() / 8);
    for (size_t i = 0; i < text.size(); i++) {
        wchar_t ch = text[i];
        if (ch == L'\r') {
            if (i + 1 < text.size() && text[i + 1] == L'\n') {
                i++;
            }
            normalized.push_back(L'\n');
        } else if (ch == L'\t') {
            normalized.append(L"    ");
        } else if (ch != L'\0') {
            normalized.push_back(ch);
        }
    }
    text = std::move(normalized);
}

static void ClearTextPreview(PanelState* state) {
    if (!state) {
        return;
    }
    state->textPreview = false;
    state->textRaw.clear();
    state->textLines.clear();
    state->textLineNumbers.clear();
    state->textScroll = 0;
    state->textScrollX = 0;
    state->textMaxChars = 0;
    state->textVisibleChars = 0;
    state->textHScrollRect = {};
    state->textHThumbRect = {};
    state->textVisibleLines = 0;
    state->textWrapWidth = 0;
    state->textGutterWidth = 0;
    state->textCharWidth = 0;
    state->textSourceLines = 0;
    state->textLineMatches.clear();
    state->textMatchCount = 0;
    state->textToolbarHover = -1;
    for (RECT& rect : state->textToolbar) {
        rect = {};
    }
    state->textBytes = 0;
    state->textTruncated = false;
    state->textEncoding.clear();
}

// Wraps the raw text into visual lines using the real advance width of the
// monospace font. Each visual line remembers the source line it belongs to, and
// a wrapped remainder carries 0 so the gutter only prints one number per source
// line. The gutter width itself depends on the source line count, not on the
// wrapping, so it is computed before the lines are split.
static void RebuildTextLines(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    UINT dpi = GetDpiForWindowSafe(state->panel);
    RECT client = {};
    GetClientRect(state->panel, &client);
    int width = (client.right - Scale(16, dpi) * 2) - Scale(12, dpi);
    if (width < Scale(40, dpi)) {
        width = Scale(40, dpi);
    }

    HFONT font = CreateTextFont(dpi);
    HDC dc = GetDC(state->panel);
    HGDIOBJ previous = SelectObject(dc, font);
    TEXTMETRICW metrics = {};
    GetTextMetricsW(dc, &metrics);
    SelectObject(dc, previous);
    DeleteObject(font);
    ReleaseDC(state->panel, dc);

    int charWidth = std::max(1, static_cast<int>(metrics.tmAveCharWidth));
    state->textCharWidth = charWidth;

    size_t sourceLines = 1;
    for (wchar_t ch : state->textRaw) {
        if (ch == L'\n') {
            sourceLines++;
        }
    }
    state->textSourceLines = static_cast<int>(sourceLines);

    int gutter = 0;
    int digits = 1;
    for (size_t value = sourceLines; value >= 10; value /= 10) {
        digits++;
    }
    state->textGutterWidth = digits * charWidth + Scale(10, dpi);
    if (g_textLineNumbers.load()) {
        gutter = state->textGutterWidth;
    }

    bool wrap = g_textWrap.load();
    bool dots = g_textWhitespace.load();
    int characters = std::max(8, (width - gutter) / charWidth);

    state->textWrapWidth = width;
    state->textLines.clear();
    state->textLineNumbers.clear();
    state->textLineSources.clear();
    state->textLineMatches.clear();
    state->textMatchCount = 0;
    state->textMaxChars = 0;

    std::wstring needle = LowerText(state->filter);

    auto push = [&](const std::wstring& piece, int number, int source) {
        std::vector<std::pair<int, int>> matches;
        if (!needle.empty()) {
            std::wstring lower = LowerText(piece);
            size_t found = lower.find(needle);
            while (found != std::wstring::npos) {
                matches.emplace_back(static_cast<int>(found),
                                     static_cast<int>(found + needle.size()));
                found = lower.find(needle, found + 1);
            }
        }

        state->textMaxChars =
            std::max(state->textMaxChars, static_cast<int>(piece.size()));
        if (dots) {
            std::wstring shown = piece;
            for (wchar_t& ch : shown) {
                if (ch == L' ') {
                    ch = L'\u00B7';
                }
            }
            state->textLines.push_back(std::move(shown));
        } else {
            state->textLines.push_back(piece);
        }
        state->textLineNumbers.push_back(number);
        state->textLineSources.push_back(source);
        state->textLineMatches.push_back(std::move(matches));
    };

    const std::wstring& raw = state->textRaw;
    size_t start = 0;
    int lineNumber = 1;
    while (true) {
        size_t end = raw.find(L'\n', start);
        std::wstring line = end == std::wstring::npos
                                ? raw.substr(start)
                                : raw.substr(start, end - start);
        if (!needle.empty()) {
            if (LowerText(line).find(needle) == std::wstring::npos) {
                lineNumber++;
                if (end == std::wstring::npos) {
                    break;
                }
                start = end + 1;
                continue;
            }
            state->textMatchCount++;
        }
        bool first = true;
        if (wrap) {
            // A cursor instead of erasing the front of the string: erasing made
            // a single huge line quadratic, which is what froze Explorer on a
            // minified HTML file.
            size_t cursor = 0;
            while (cursor < line.size() &&
                   static_cast<int>(line.size() - cursor) > characters) {
                size_t limit = cursor + static_cast<size_t>(characters);
                size_t cut = std::wstring::npos;
                size_t found = line.rfind(L' ', limit - 1);
                if (found != std::wstring::npos && found >= cursor &&
                    found >= cursor + static_cast<size_t>(characters) / 2 &&
                    found < limit) {
                    cut = found;
                }
                if (cut == std::wstring::npos || cut <= cursor) {
                    cut = limit;
                }
                push(line.substr(cursor, cut - cursor),
                     first ? lineNumber : 0, lineNumber);
                first = false;
                cursor = cut;
                while (cursor < line.size() && line[cursor] == L' ') {
                    cursor++;
                }
            }
            push(line.substr(cursor), first ? lineNumber : 0, lineNumber);
        } else {
            push(line, lineNumber, lineNumber);
        }
        lineNumber++;

        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }

    int maximum = std::max(0, static_cast<int>(state->textLines.size()) - 1);
    state->textScroll = std::max(0, std::min(state->textScroll, maximum));
    if (wrap) {
        state->textScrollX = 0;
    }
}

// Reads and decodes a text file. It touches no panel state, so it can run on a
// worker thread while the panel keeps painting.
static bool ReadTextContent(const std::wstring& path, std::wstring& raw,
                            std::wstring& encoding, ULONGLONG& bytes,
                            bool& truncated) {
    raw.clear();
    encoding.clear();
    bytes = 0;
    truncated = false;

    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER fileSize = {};
    GetFileSizeEx(file, &fileSize);
    const ULONGLONG kLimit = 200ull * 1024;
    bytes = static_cast<ULONGLONG>(fileSize.QuadPart);
    DWORD toRead = static_cast<DWORD>(std::min<ULONGLONG>(bytes, kLimit));
    std::vector<char> data(toRead);
    DWORD read = 0;
    BOOL ok = toRead == 0 ||
              ReadFile(file, data.data(), toRead, &read, nullptr);
    CloseHandle(file);
    if (!ok) {
        return false;
    }
    data.resize(read);

    if (!LooksLikeText(data.data(), data.size())) {
        return false;
    }

    std::wstring text;
    std::wstring detected;
    if (!DecodeText(data, text, detected)) {
        return false;
    }
    NormalizeText(text);
    if (bytes > kLimit) {
        text += L"\n\n[... truncated at 200 KB]";
        truncated = true;
    }

    raw = std::move(text);
    encoding = std::move(detected);
    return true;
}

// ---------------------------------------------------------------------------
// Document text (Office Open XML, OpenDocument, RTF)
// ---------------------------------------------------------------------------
//
// The Office preview handler lives in a surrogate process and Explorer cannot
// drive it from outside (only a few of its interfaces cross the process
// boundary). So the text of modern documents is read here directly: a .docx,
// .xlsx, .pptx or OpenDocument file is a zip of XML parts, and its text is
// shown in the same text preview used for plain files. Nothing here touches
// panel state, so it runs on a worker thread.

static bool IsNativeDocExtension(const std::wstring& extension) {
    static const wchar_t* const kNative[] = {
        L"docx", L"docm", L"dotx", L"dotm", L"xlsx", L"xlsm", L"xltx",
        L"xltm", L"pptx", L"pptm", L"ppsx", L"ppsm", L"potx", L"potm",
        L"odt",  L"ods",  L"odp",  L"rtf"};
    for (const wchar_t* name : kNative) {
        if (extension == name) {
            return true;
        }
    }
    return false;
}

static void FallbackDocumentPreview(PanelState* state, ULONGLONG size);

static std::wstring LowerExtension(const std::wstring& path);

// Tells a document worker whether the preview that asked for its work is still
// the current one. A new selection and Wh_ModUninit both stop the parse.
struct CancelCheck {
    const std::shared_ptr<std::atomic<unsigned>>* token = nullptr;
    unsigned generation = 0;

    bool operator()() const {
        return g_stopWorkers.load() ||
               (token && *token && (*token)->load() != generation);
    }
};

// Set for the duration of one extraction on the worker thread that runs it, so
// the deep parsing loops can stop without a parameter through every function.
static thread_local const CancelCheck* t_docCancel = nullptr;

static bool DocCancelled() {
    return g_stopWorkers.load() || (t_docCancel && (*t_docCancel)());
}

// Restores the previous check on every return path.
struct CancelScope {
    const CancelCheck* previous = nullptr;
    explicit CancelScope(const CancelCheck* current) : previous(t_docCancel) {
        t_docCancel = current;
    }
    ~CancelScope() { t_docCancel = previous; }
};

namespace qldoc {

// --- Numbers ---------------------------------------------------------------
//
// Parses a decimal run without undefined behaviour: it stops at the first
// non-digit and at a sane magnitude, so a crafted document cannot overflow an
// int the way atoi() would.
static int ParseDecimal(const char* text, size_t length) {
    int value = 0;
    for (size_t i = 0; i < length; i++) {
        char ch = text[i];
        if (ch < '0' || ch > '9') {
            break;
        }
        if (value > 100000000) {
            break;
        }
        value = value * 10 + (ch - '0');
    }
    return value;
}

// --- Inflate (RFC 1951), derived from zlib's puff --------------------------
//
// This code is a C++ port of puff.c:
//
//   Copyright (C) 2002-2013 Mark Adler, all rights reserved
//   version 2.3, 21 Jan 2013
//
//   This software is provided 'as-is', without any express or implied
//   warranty.  In no event will the author be held liable for any damages
//   arising from the use of this software.
//
//   Permission is granted to anyone to use this software for any purpose,
//   including commercial applications, and to alter it and redistribute it
//   freely, subject to the following restrictions:
//
//   1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//   2. Altered source versions must be plainly marked as such, and must not be
//      misrepresented as being the original software.
//   3. This notice may not be removed or altered from any source distribution.
//
//   Mark Adler
//   madler@alumni.caltech.edu
//
// Altered source version: rewritten in C++ for this mod, with an output byte
// limit and no checksum or preset-dictionary support.

struct BitReader {
    const unsigned char* data;
    size_t size;
    size_t pos;
    uint32_t buffer;
    int count;
};

static int GetBits(BitReader& s, int need) {
    uint32_t value = s.buffer;
    while (s.count < need) {
        if (s.pos >= s.size) {
            return -1;
        }
        value |= static_cast<uint32_t>(s.data[s.pos++]) << s.count;
        s.count += 8;
    }
    s.buffer = value >> need;
    s.count -= need;
    return static_cast<int>(value & ((1u << need) - 1));
}

struct Huffman {
    short count[16];
    short symbol[320];
};

static int BuildHuffman(Huffman& h, const short* length, int n) {
    for (int i = 0; i < 16; i++) {
        h.count[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        h.count[length[i]]++;
    }
    if (h.count[0] == n) {
        return 0;
    }
    int left = 1;
    for (int len = 1; len <= 15; len++) {
        left <<= 1;
        left -= h.count[len];
        if (left < 0) {
            return left;
        }
    }
    short offsets[16];
    offsets[1] = 0;
    for (int len = 1; len < 15; len++) {
        offsets[len + 1] = static_cast<short>(offsets[len] + h.count[len]);
    }
    for (int i = 0; i < n; i++) {
        if (length[i] != 0) {
            h.symbol[offsets[length[i]]++] = static_cast<short>(i);
        }
    }
    return left;
}

static int DecodeSymbol(BitReader& s, const Huffman& h) {
    int code = 0;
    int first = 0;
    int index = 0;
    for (int len = 1; len <= 15; len++) {
        int bit = GetBits(s, 1);
        if (bit < 0) {
            return -1;
        }
        code |= bit;
        int count = h.count[len];
        if (code - count < first) {
            return h.symbol[index + (code - first)];
        }
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -2;
}

// Stops quietly (and successfully) when `limit` bytes were produced.
static bool InflateCodes(BitReader& s, std::vector<unsigned char>& out,
                         size_t limit, const Huffman& lencode,
                         const Huffman& distcode) {
    static const short kLens[29] = {3,  4,  5,  6,  7,  8,  9,  10, 11, 13,
                                    15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
                                    67, 83, 99, 115, 131, 163, 195, 227, 258};
    static const short kLenExtra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                        1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
                                        4, 4, 4, 4, 5, 5, 5, 5, 0};
    static const short kDists[30] = {1,   2,   3,   4,   5,   7,    9,    13,
                                     17,  25,  33,  49,  65,  97,   129,  193,
                                     257, 385, 513, 769, 1025, 1537, 2049, 3073,
                                     4097, 6145, 8193, 12289, 16385, 24577};
    static const short kDistExtra[30] = {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,
                                         4, 4, 5, 5, 6, 6, 7, 7,  8,  8,
                                         9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
    for (;;) {
        if (out.size() >= limit) {
            return true;
        }
        // Decompressing a document is CPU-bound and can run for a long time.
        // Wh_ModUninit waits for every worker, so it has to be able to stop.
        if ((out.size() & 0x3FFF) == 0 && DocCancelled()) {
            return false;
        }
        int symbol = DecodeSymbol(s, lencode);
        if (symbol < 0) {
            return false;
        }
        if (symbol < 256) {
            out.push_back(static_cast<unsigned char>(symbol));
        } else if (symbol == 256) {
            return true;
        } else {
            symbol -= 257;
            if (symbol >= 29) {
                return false;
            }
            int extra = GetBits(s, kLenExtra[symbol]);
            if (extra < 0) {
                return false;
            }
            int length = kLens[symbol] + extra;
            symbol = DecodeSymbol(s, distcode);
            if (symbol < 0 || symbol >= 30) {
                return false;
            }
            extra = GetBits(s, kDistExtra[symbol]);
            if (extra < 0) {
                return false;
            }
            size_t distance = static_cast<size_t>(kDists[symbol] + extra);
            if (distance > out.size()) {
                return false;
            }
            size_t from = out.size() - distance;
            for (int i = 0; i < length && out.size() < limit; i++) {
                unsigned char byte = out[from + i];
                out.push_back(byte);
            }
        }
    }
}

static bool Inflate(const unsigned char* data, size_t size,
                    std::vector<unsigned char>& out, size_t limit) {
    BitReader s = {data, size, 0, 0, 0};
    int last = 0;
    do {
        last = GetBits(s, 1);
        int type = GetBits(s, 2);
        if (last < 0 || type < 0) {
            return false;
        }
        if (type == 0) {
            s.buffer = 0;
            s.count = 0;
            if (s.pos + 4 > s.size) {
                return false;
            }
            unsigned len = s.data[s.pos] | (s.data[s.pos + 1] << 8);
            unsigned nlen = s.data[s.pos + 2] | (s.data[s.pos + 3] << 8);
            if (len != (~nlen & 0xFFFF)) {
                return false;
            }
            s.pos += 4;
            if (s.pos + len > s.size) {
                return false;
            }
            size_t room = limit > out.size() ? limit - out.size() : 0;
            size_t take = std::min<size_t>(len, room);
            out.insert(out.end(), s.data + s.pos, s.data + s.pos + take);
            s.pos += len;
            if (take < len) {
                return true;
            }
        } else if (type == 1) {
            short lengths[320];
            int symbol = 0;
            for (; symbol < 144; symbol++) lengths[symbol] = 8;
            for (; symbol < 256; symbol++) lengths[symbol] = 9;
            for (; symbol < 280; symbol++) lengths[symbol] = 7;
            for (; symbol < 288; symbol++) lengths[symbol] = 8;
            Huffman lit;
            BuildHuffman(lit, lengths, 288);
            short distLengths[30];
            for (int i = 0; i < 30; i++) distLengths[i] = 5;
            Huffman dist;
            BuildHuffman(dist, distLengths, 30);
            if (!InflateCodes(s, out, limit, lit, dist)) {
                return false;
            }
        } else if (type == 2) {
            static const short kOrder[19] = {16, 17, 18, 0, 8,  7, 9,  6, 10, 5,
                                             11, 4,  12, 3, 13, 2, 14, 1, 15};
            int nlen = GetBits(s, 5);
            int ndist = GetBits(s, 5);
            int ncode = GetBits(s, 4);
            if (nlen < 0 || ndist < 0 || ncode < 0) {
                return false;
            }
            nlen += 257;
            ndist += 1;
            ncode += 4;
            if (nlen > 286 || ndist > 30) {
                return false;
            }
            short lengths[320];
            int index = 0;
            for (; index < ncode; index++) {
                int value = GetBits(s, 3);
                if (value < 0) {
                    return false;
                }
                lengths[kOrder[index]] = static_cast<short>(value);
            }
            for (; index < 19; index++) {
                lengths[kOrder[index]] = 0;
            }
            Huffman codeCode;
            if (BuildHuffman(codeCode, lengths, 19) != 0) {
                return false;
            }
            index = 0;
            while (index < nlen + ndist) {
                int symbol = DecodeSymbol(s, codeCode);
                if (symbol < 0) {
                    return false;
                }
                if (symbol < 16) {
                    lengths[index++] = static_cast<short>(symbol);
                } else {
                    int repeatValue = 0;
                    int repeat = 0;
                    int extra = 0;
                    if (symbol == 16) {
                        if (index == 0) {
                            return false;
                        }
                        repeatValue = lengths[index - 1];
                        extra = GetBits(s, 2);
                        repeat = 3;
                    } else if (symbol == 17) {
                        extra = GetBits(s, 3);
                        repeat = 3;
                    } else {
                        extra = GetBits(s, 7);
                        repeat = 11;
                    }
                    if (extra < 0) {
                        return false;
                    }
                    repeat += extra;
                    if (index + repeat > nlen + ndist) {
                        return false;
                    }
                    while (repeat--) {
                        lengths[index++] = static_cast<short>(repeatValue);
                    }
                }
            }
            if (lengths[256] == 0) {
                return false;
            }
            Huffman lit;
            int err = BuildHuffman(lit, lengths, nlen);
            if (err < 0 || (err > 0 && nlen - lit.count[0] != 1)) {
                return false;
            }
            Huffman dist;
            err = BuildHuffman(dist, lengths + nlen, ndist);
            if (err < 0 || (err > 0 && ndist - dist.count[0] != 1)) {
                return false;
            }
            if (!InflateCodes(s, out, limit, lit, dist)) {
                return false;
            }
        } else {
            return false;
        }
        if (out.size() >= limit) {
            return true;
        }
        if (DocCancelled()) {
            return false;
        }
    } while (!last);
    return true;
}

// --- Zip ---------------------------------------------------------------------

struct ZipEntry {
    uint32_t method = 0;
    uint32_t compSize = 0;
    uint32_t uncompSize = 0;
    uint32_t offset = 0;
};

struct ZipArchive {
    std::vector<unsigned char> data;
    std::map<std::string, ZipEntry> entries;
};

static uint32_t Rd16(const unsigned char* p) {
    return static_cast<uint32_t>(p[0] | (p[1] << 8));
}

static uint32_t Rd32(const unsigned char* p) {
    return Rd16(p) | (Rd16(p + 2) << 16);
}

static bool OpenZip(ZipArchive& zip) {
    const std::vector<unsigned char>& d = zip.data;
    size_t n = d.size();
    if (n < 22) {
        return false;
    }
    size_t minimum = n > 65557 ? n - 65557 : 0;
    size_t eocd = std::string::npos;
    for (size_t i = n - 22;; i--) {
        if (Rd32(&d[i]) == 0x06054b50) {
            eocd = i;
            break;
        }
        if (i == minimum) {
            break;
        }
    }
    if (eocd == std::string::npos) {
        return false;
    }
    uint32_t count = Rd16(&d[eocd + 10]);
    size_t pos = Rd32(&d[eocd + 16]);
    for (uint32_t i = 0; i < count; i++) {
        if (pos + 46 > n || Rd32(&d[pos]) != 0x02014b50) {
            break;
        }
        ZipEntry entry;
        entry.method = Rd16(&d[pos + 10]);
        entry.compSize = Rd32(&d[pos + 20]);
        entry.uncompSize = Rd32(&d[pos + 24]);
        size_t nameLen = Rd16(&d[pos + 28]);
        size_t extraLen = Rd16(&d[pos + 30]);
        size_t commentLen = Rd16(&d[pos + 32]);
        entry.offset = Rd32(&d[pos + 42]);
        if (pos + 46 + nameLen > n) {
            break;
        }
        std::string name(reinterpret_cast<const char*>(&d[pos + 46]), nameLen);
        for (char& ch : name) {
            if (ch == '\\') {
                ch = '/';
            }
        }
        zip.entries[name] = entry;
        pos += 46 + nameLen + extraLen + commentLen;
    }
    return !zip.entries.empty();
}

static const size_t kMaxPartBytes = 48u * 1024 * 1024;

static bool ReadZipEntry(const ZipArchive& zip, const std::string& name,
                         std::string& out) {
    out.clear();
    auto found = zip.entries.find(name);
    if (found == zip.entries.end()) {
        return false;
    }
    const ZipEntry& e = found->second;
    const std::vector<unsigned char>& d = zip.data;
    size_t n = d.size();
    size_t offset = e.offset;
    if (offset + 30 > n || Rd32(&d[offset]) != 0x04034b50) {
        return false;
    }
    size_t dataPos = offset + 30 + Rd16(&d[offset + 26]) +
                     Rd16(&d[offset + 28]);
    if (dataPos >= n) {
        return e.uncompSize == 0;
    }
    size_t available = n - dataPos;
    size_t compressed = std::min<size_t>(e.compSize, available);
    if (e.method == 0) {
        out.assign(reinterpret_cast<const char*>(&d[dataPos]),
                   std::min(compressed, kMaxPartBytes));
        return true;
    }
    if (e.method == 8) {
        std::vector<unsigned char> buffer;
        buffer.reserve(std::min<size_t>(e.uncompSize, kMaxPartBytes));
        if (!Inflate(&d[dataPos], compressed, buffer, kMaxPartBytes)) {
            return false;
        }
        out.assign(reinterpret_cast<const char*>(buffer.data()),
                   buffer.size());
        return true;
    }
    return false;
}

// --- XML ---------------------------------------------------------------------

static void AppendUtf8(std::string& out, uint32_t cp) {
    if (cp < 0x80) {
        out.push_back(static_cast<char>(cp));
    } else if (cp < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x110000) {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

static void AppendXmlText(std::string& out, std::string_view text) {
    for (size_t i = 0; i < text.size(); i++) {
        char ch = text[i];
        if (ch != '&') {
            out.push_back(ch);
            continue;
        }
        size_t end = text.find(';', i);
        if (end == std::string_view::npos || end - i > 10) {
            out.push_back(ch);
            continue;
        }
        std::string_view entity = text.substr(i + 1, end - i - 1);
        if (entity == "lt") {
            out.push_back('<');
        } else if (entity == "gt") {
            out.push_back('>');
        } else if (entity == "amp") {
            out.push_back('&');
        } else if (entity == "quot") {
            out.push_back('"');
        } else if (entity == "apos") {
            out.push_back('\'');
        } else if (!entity.empty() && entity[0] == '#') {
            std::string number(entity.substr(1));
            bool hex = !number.empty() && (number[0] == 'x' || number[0] == 'X');
            uint32_t cp = static_cast<uint32_t>(
                strtoul(number.c_str() + (hex ? 1 : 0), nullptr, hex ? 16 : 10));
            AppendUtf8(out, cp);
        } else {
            out.push_back(ch);
            continue;
        }
        i = end;
    }
}

// kind: 0 start tag, 1 end tag, 2 empty tag, 3 text, 4 CDATA text.
template <typename F>
static void ScanXml(const std::string& xml, F&& callback) {
    size_t i = 0;
    const size_t n = xml.size();
    size_t nextCancelCheck = 0;
    std::string_view view(xml);
    while (i < n) {
        if (i >= nextCancelCheck) {
            if (DocCancelled()) {
                break;
            }
            nextCancelCheck = i + 4096;
        }
        if (xml[i] != '<') {
            size_t end = xml.find('<', i);
            if (end == std::string::npos) {
                end = n;
            }
            callback(3, view.substr(i, end - i));
            i = end;
            continue;
        }
        if (xml.compare(i, 4, "<!--") == 0) {
            size_t end = xml.find("-->", i + 4);
            if (end == std::string::npos) {
                break;
            }
            i = end + 3;
            continue;
        }
        if (xml.compare(i, 9, "<![CDATA[") == 0) {
            size_t end = xml.find("]]>", i + 9);
            size_t stop = end == std::string::npos ? n : end;
            callback(4, view.substr(i + 9, stop - i - 9));
            i = end == std::string::npos ? n : end + 3;
            continue;
        }
        if (i + 1 < n && (xml[i + 1] == '?' || xml[i + 1] == '!')) {
            size_t end = xml.find('>', i);
            if (end == std::string::npos) {
                break;
            }
            i = end + 1;
            continue;
        }
        size_t end = xml.find('>', i);
        if (end == std::string::npos) {
            break;
        }
        std::string_view tag = view.substr(i + 1, end - i - 1);
        if (!tag.empty() && tag[0] == '/') {
            callback(1, tag.substr(1));
        } else if (!tag.empty() && tag.back() == '/') {
            callback(2, tag.substr(0, tag.size() - 1));
        } else {
            callback(0, tag);
        }
        i = end + 1;
    }
}

static std::string_view XmlName(std::string_view tag) {
    size_t end = 0;
    while (end < tag.size() && tag[end] != ' ' && tag[end] != '\t' &&
           tag[end] != '\r' && tag[end] != '\n' && tag[end] != '/') {
        end++;
    }
    return tag.substr(0, end);
}

static std::string XmlAttr(std::string_view tag, std::string_view attribute) {
    size_t pos = 0;
    while ((pos = tag.find(attribute, pos)) != std::string_view::npos) {
        size_t after = pos + attribute.size();
        bool startOk = pos > 0 && (tag[pos - 1] == ' ' || tag[pos - 1] == '\t' ||
                                   tag[pos - 1] == '\r' || tag[pos - 1] == '\n');
        if (startOk && after + 1 < tag.size() && tag[after] == '=') {
            char quote = tag[after + 1];
            size_t end = tag.find(quote, after + 2);
            if (end == std::string_view::npos) {
                return std::string();
            }
            std::string value;
            AppendXmlText(value, tag.substr(after + 2, end - after - 2));
            return value;
        }
        pos = after;
    }
    return std::string();
}

static void AddText(std::string& out, int kind, std::string_view text) {
    if (kind == 4) {
        out.append(text);
    } else {
        AppendXmlText(out, text);
    }
}

static std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return std::wstring();
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(),
                                     static_cast<int>(text.size()), nullptr, 0);
    if (length <= 0) {
        return std::wstring();
    }
    std::wstring wide(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                        wide.data(), length);
    return wide;
}

static const size_t kMaxDocText = 4u * 1024 * 1024;

// --- Word --------------------------------------------------------------------

static std::wstring ExtractDocx(const ZipArchive& zip) {
    std::string xml;
    if (!ReadZipEntry(zip, "word/document.xml", xml)) {
        return std::wstring();
    }
    std::string out;
    bool inText = false;
    int cell = 0;
    ScanXml(xml, [&](int kind, std::string_view tag) {
        if (out.size() > kMaxDocText) {
            return;
        }
        if (kind >= 3) {
            if (inText) {
                AddText(out, kind, tag);
            }
            return;
        }
        std::string_view name = XmlName(tag);
        if (kind == 0) {
            if (name == "w:t") {
                inText = true;
            } else if (name == "w:tc") {
                cell++;
            }
        } else if (kind == 1) {
            if (name == "w:t") {
                inText = false;
            } else if (name == "w:p") {
                out += cell > 0 ? " " : "\n";
            } else if (name == "w:tc") {
                if (cell > 0) {
                    cell--;
                }
                out += " | ";
            } else if (name == "w:tr") {
                out += "\n";
            }
        } else {
            if (name == "w:tab") {
                out += "    ";
            } else if (name == "w:br" || name == "w:cr") {
                out += "\n";
            }
        }
    });
    return Utf8ToWide(out);
}

// --- PowerPoint --------------------------------------------------------------

static std::wstring ExtractPptx(const ZipArchive& zip) {
    static const char kPrefix[] = "ppt/slides/slide";
    std::vector<std::pair<int, std::string>> slides;
    for (const auto& entry : zip.entries) {
        const std::string& name = entry.first;
        if (name.rfind(kPrefix, 0) != 0) {
            continue;
        }
        size_t start = sizeof(kPrefix) - 1;
        size_t end = start;
        while (end < name.size() && name[end] >= '0' && name[end] <= '9') {
            end++;
        }
        if (end == start || name.substr(end) != ".xml") {
            continue;
        }
        slides.emplace_back(
            ParseDecimal(name.data() + start, end - start), name);
    }
    std::sort(slides.begin(), slides.end());

    std::string out;
    int number = 0;
    for (const auto& slide : slides) {
        if (DocCancelled()) {
            break;
        }
        std::string xml;
        if (!ReadZipEntry(zip, slide.second, xml)) {
            continue;
        }
        number++;
        out += "--- Slide " + std::to_string(number) + " ---\n";
        bool inText = false;
        ScanXml(xml, [&](int kind, std::string_view tag) {
            if (out.size() > kMaxDocText) {
                return;
            }
            if (kind >= 3) {
                if (inText) {
                    AddText(out, kind, tag);
                }
                return;
            }
            std::string_view name = XmlName(tag);
            if (kind == 0 && name == "a:t") {
                inText = true;
            } else if (kind == 1 && name == "a:t") {
                inText = false;
            } else if (kind == 1 && name == "a:p") {
                out += "\n";
            } else if (kind == 2 && name == "a:br") {
                out += "\n";
            }
        });
        out += "\n";
    }
    return Utf8ToWide(out);
}

// --- Excel -------------------------------------------------------------------

static int ColumnIndex(const std::string& reference) {
    // Excel references stop at XFD (three letters). Parsing more is both
    // meaningless and an integer overflow on a crafted file.
    int column = 0;
    int letters = 0;
    for (char ch : reference) {
        int digit = 0;
        if (ch >= 'A' && ch <= 'Z') {
            digit = ch - 'A' + 1;
        } else if (ch >= 'a' && ch <= 'z') {
            digit = ch - 'a' + 1;
        } else {
            break;
        }
        if (++letters > 3) {
            break;
        }
        column = column * 26 + digit;
    }
    return column - 1;
}

static std::wstring ExtractXlsx(const ZipArchive& zip) {
    std::string xml;

    std::vector<std::string> shared;
    if (ReadZipEntry(zip, "xl/sharedStrings.xml", xml)) {
        std::string current;
        bool inText = false;
        int phonetic = 0;
        ScanXml(xml, [&](int kind, std::string_view tag) {
            if (kind >= 3) {
                if (inText && phonetic == 0) {
                    AddText(current, kind, tag);
                }
                return;
            }
            std::string_view name = XmlName(tag);
            if (kind == 0) {
                if (name == "si") {
                    current.clear();
                } else if (name == "t") {
                    inText = true;
                } else if (name == "rPh") {
                    phonetic++;
                }
            } else if (kind == 1) {
                if (name == "si") {
                    shared.push_back(current);
                } else if (name == "t") {
                    inText = false;
                } else if (name == "rPh" && phonetic > 0) {
                    phonetic--;
                }
            } else if (name == "si") {
                shared.push_back(std::string());
            }
        });
    }

    struct Sheet {
        std::string name;
        std::string path;
    };
    std::vector<Sheet> sheets;
    std::map<std::string, std::string> relations;
    if (ReadZipEntry(zip, "xl/_rels/workbook.xml.rels", xml)) {
        ScanXml(xml, [&](int kind, std::string_view tag) {
            if (kind >= 3 || kind == 1) {
                return;
            }
            if (XmlName(tag) == "Relationship") {
                relations[XmlAttr(tag, "Id")] = XmlAttr(tag, "Target");
            }
        });
    }
    if (ReadZipEntry(zip, "xl/workbook.xml", xml)) {
        ScanXml(xml, [&](int kind, std::string_view tag) {
            if (kind >= 3 || kind == 1 || XmlName(tag) != "sheet") {
                return;
            }
            Sheet sheet;
            sheet.name = XmlAttr(tag, "name");
            std::string target = relations[XmlAttr(tag, "r:id")];
            if (target.empty()) {
                return;
            }
            sheet.path = target[0] == '/' ? target.substr(1) : "xl/" + target;
            sheets.push_back(sheet);
        });
    }
    if (sheets.empty()) {
        for (const auto& entry : zip.entries) {
            if (entry.first.rfind("xl/worksheets/sheet", 0) == 0 &&
                entry.first.size() > 4 &&
                entry.first.substr(entry.first.size() - 4) == ".xml") {
                sheets.push_back({entry.first.substr(13), entry.first});
            }
        }
    }

    const size_t kMaxRows = 3000;
    const size_t kMaxColumns = 80;
    const size_t kMaxCellWidth = 40;
    std::wstring out;

    for (size_t sheetIndex = 0; sheetIndex < sheets.size() && sheetIndex < 30;
         sheetIndex++) {
        if (DocCancelled()) {
            break;
        }
        const Sheet& sheet = sheets[sheetIndex];
        if (!ReadZipEntry(zip, sheet.path, xml)) {
            continue;
        }

        std::vector<std::vector<std::wstring>> rows;
        std::vector<std::wstring> row;
        std::string cellType;
        std::string cellText;
        int column = 0;
        bool inValue = false;
        bool inInline = false;

        auto commit = [&]() {
            std::string value;
            if (cellType == "s") {
                int index = ParseDecimal(cellText.data(), cellText.size());
                if (index >= 0 && static_cast<size_t>(index) < shared.size()) {
                    value = shared[static_cast<size_t>(index)];
                }
            } else if (cellType == "b") {
                value = cellText == "1" ? "TRUE" : "FALSE";
            } else {
                value = cellText;
            }
            if (column >= 0 && static_cast<size_t>(column) < kMaxColumns &&
                !value.empty()) {
                if (row.size() <= static_cast<size_t>(column)) {
                    row.resize(static_cast<size_t>(column) + 1);
                }
                std::wstring wide = Utf8ToWide(value);
                for (wchar_t& ch : wide) {
                    if (ch == L'\n' || ch == L'\r' || ch == L'\t') {
                        ch = L' ';
                    }
                }
                if (wide.size() > kMaxCellWidth) {
                    wide.resize(kMaxCellWidth - 1);
                    wide.push_back(L'…');
                }
                row[static_cast<size_t>(column)] = std::move(wide);
            }
            column++;
        };

        ScanXml(xml, [&](int kind, std::string_view tag) {
            if (rows.size() >= kMaxRows) {
                return;
            }
            if (kind >= 3) {
                if (inValue || inInline) {
                    AddText(cellText, kind, tag);
                }
                return;
            }
            std::string_view name = XmlName(tag);
            if (kind == 0) {
                if (name == "row") {
                    row.clear();
                    column = 0;
                } else if (name == "c") {
                    cellType = XmlAttr(tag, "t");
                    std::string reference = XmlAttr(tag, "r");
                    if (!reference.empty()) {
                        int parsed = ColumnIndex(reference);
                        if (parsed >= 0) {
                            column = parsed;
                        }
                    }
                    cellText.clear();
                } else if (name == "v") {
                    inValue = true;
                } else if (name == "t") {
                    inInline = true;
                }
            } else if (kind == 1) {
                if (name == "v") {
                    inValue = false;
                } else if (name == "t") {
                    inInline = false;
                } else if (name == "c") {
                    commit();
                } else if (name == "row") {
                    bool any = false;
                    for (const std::wstring& cell : row) {
                        if (!cell.empty()) {
                            any = true;
                            break;
                        }
                    }
                    if (any) {
                        rows.push_back(row);
                    }
                }
            } else if (name == "c") {
                column++;
            }
        });

        if (rows.empty() && sheets.size() > 1) {
            out += L"=== " + Utf8ToWide(sheet.name) + L" ===\n(empty)\n\n";
            continue;
        }

        std::vector<size_t> widths;
        for (const auto& cells : rows) {
            if (widths.size() < cells.size()) {
                widths.resize(cells.size(), 0);
            }
            for (size_t c = 0; c < cells.size(); c++) {
                widths[c] = std::max(widths[c], cells[c].size());
            }
        }

        if (sheets.size() > 1 || !sheet.name.empty()) {
            out += L"=== " + Utf8ToWide(sheet.name) + L" ===\n";
        }
        for (const auto& cells : rows) {
            std::wstring line;
            for (size_t c = 0; c < cells.size(); c++) {
                if (c > 0) {
                    line += L" | ";
                }
                line += cells[c];
                if (c + 1 < cells.size() && cells[c].size() < widths[c]) {
                    line.append(widths[c] - cells[c].size(), L' ');
                }
            }
            while (!line.empty() && (line.back() == L' ' || line.back() == L'|')) {
                line.pop_back();
            }
            out += line;
            out += L'\n';
            if (out.size() > kMaxDocText) {
                break;
            }
        }
        out += L'\n';
        if (out.size() > kMaxDocText) {
            break;
        }
    }
    return out;
}

// --- OpenDocument ------------------------------------------------------------

// mode: 0 text, 1 spreadsheet, 2 presentation.
static std::wstring ExtractOdf(const ZipArchive& zip, int mode) {
    std::string xml;
    if (!ReadZipEntry(zip, "content.xml", xml)) {
        return std::wstring();
    }
    std::string out;
    std::string rowText;
    bool inRow = false;
    int paragraph = 0;
    int cell = 0;
    int slide = 0;
    auto sink = [&]() -> std::string& { return inRow ? rowText : out; };

    ScanXml(xml, [&](int kind, std::string_view tag) {
        if (out.size() > kMaxDocText) {
            return;
        }
        if (kind >= 3) {
            if (paragraph > 0) {
                AddText(sink(), kind, tag);
            }
            return;
        }
        std::string_view name = XmlName(tag);
        if (kind == 0) {
            if (name == "text:p" || name == "text:h") {
                paragraph++;
            } else if (name == "table:table-row") {
                inRow = true;
                rowText.clear();
            } else if (name == "table:table-cell") {
                cell++;
            } else if (name == "draw:page" && mode == 2) {
                slide++;
                out += "--- Slide " + std::to_string(slide) + " ---\n";
            } else if (name == "table:table" && mode == 1) {
                out += "=== " + XmlAttr(tag, "table:name") + " ===\n";
            }
        } else if (kind == 1) {
            if (name == "text:p" || name == "text:h") {
                if (paragraph > 0) {
                    paragraph--;
                }
                if (paragraph == 0) {
                    sink() += cell > 0 ? " " : "\n";
                }
            } else if (name == "table:table-cell") {
                if (cell > 0) {
                    cell--;
                }
                sink() += " | ";
            } else if (name == "table:table-row") {
                inRow = false;
                bool meaningful = false;
                for (char ch : rowText) {
                    if (ch != ' ' && ch != '|' && ch != '\n') {
                        meaningful = true;
                        break;
                    }
                }
                if (meaningful) {
                    while (!rowText.empty() &&
                           (rowText.back() == ' ' || rowText.back() == '|' ||
                            rowText.back() == '\n')) {
                        rowText.pop_back();
                    }
                    out += rowText;
                    out += "\n";
                }
                rowText.clear();
            } else if (name == "draw:page" && mode == 2) {
                out += "\n";
            } else if (name == "table:table" && mode == 1) {
                out += "\n";
            }
        } else {
            if (name == "text:tab") {
                sink() += "    ";
            } else if (name == "text:line-break") {
                sink() += "\n";
            } else if (name == "text:s") {
                std::string count = XmlAttr(tag, "text:c");
                int spaces = count.empty()
                                 ? 1
                                 : ParseDecimal(count.data(), count.size());
                sink().append(static_cast<size_t>(std::clamp(spaces, 1, 200)),
                              ' ');
            }
        }
    });
    return Utf8ToWide(out);
}

// --- RTF ---------------------------------------------------------------------

static std::wstring ExtractRtf(const std::string& rtf) {
    std::wstring out;
    std::vector<bool> stack;
    bool skip = false;
    const size_t n = rtf.size();

    auto appendByte = [&](unsigned char byte) {
        if (skip) {
            return;
        }
        wchar_t wide = 0;
        char single = static_cast<char>(byte);
        if (MultiByteToWideChar(CP_ACP, 0, &single, 1, &wide, 1) == 1) {
            out.push_back(wide);
        }
    };
    auto hexValue = [](char ch) -> int {
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
        if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
        return -1;
    };

    for (size_t i = 0; i < n && out.size() < kMaxDocText; i++) {
        if ((i & 0xFFFF) == 0 && DocCancelled()) {
            return std::wstring();
        }
        char ch = rtf[i];
        if (ch == '{') {
            stack.push_back(skip);
            if (i + 2 < n && rtf[i + 1] == '\\' && rtf[i + 2] == '*') {
                skip = true;
            }
        } else if (ch == '}') {
            if (!stack.empty()) {
                skip = stack.back();
                stack.pop_back();
            }
        } else if (ch == '\\') {
            if (i + 1 >= n) {
                break;
            }
            char next = rtf[i + 1];
            if (next == '\\' || next == '{' || next == '}') {
                if (!skip) {
                    out.push_back(static_cast<wchar_t>(next));
                }
                i++;
            } else if (next == '\'') {
                int high = i + 2 < n ? hexValue(rtf[i + 2]) : -1;
                int low = i + 3 < n ? hexValue(rtf[i + 3]) : -1;
                if (high >= 0 && low >= 0) {
                    appendByte(static_cast<unsigned char>(high * 16 + low));
                }
                i += 3;
            } else if (next == '~') {
                if (!skip) out.push_back(L'\u00A0');
                i++;
            } else if ((next >= 'a' && next <= 'z') ||
                       (next >= 'A' && next <= 'Z')) {
                size_t j = i + 1;
                std::string word;
                while (j < n && ((rtf[j] >= 'a' && rtf[j] <= 'z') ||
                                 (rtf[j] >= 'A' && rtf[j] <= 'Z'))) {
                    word.push_back(rtf[j++]);
                }
                bool hasNumber = false;
                long number = 0;
                bool negative = false;
                if (j < n && rtf[j] == '-') {
                    negative = true;
                    j++;
                }
                while (j < n && rtf[j] >= '0' && rtf[j] <= '9') {
                    hasNumber = true;
                    // Only \u and the small word codes use the value. Stop
                    // accumulating before a crafted file can overflow a long.
                    if (number < 1000000) {
                        number = number * 10 + (rtf[j] - '0');
                    }
                    j++;
                }
                if (negative) {
                    number = -number;
                }
                if (j < n && rtf[j] == ' ') {
                    j++;
                }
                i = j - 1;

                if (word == "par" || word == "line" || word == "sect" ||
                    word == "page") {
                    if (!skip) out.push_back(L'\n');
                } else if (word == "tab") {
                    if (!skip) out.append(L"    ");
                } else if (word == "emdash") {
                    if (!skip) out.push_back(L'—');
                } else if (word == "endash") {
                    if (!skip) out.push_back(L'–');
                } else if (word == "bullet") {
                    if (!skip) out.push_back(L'•');
                } else if (word == "fonttbl" || word == "colortbl" ||
                           word == "stylesheet" || word == "info" ||
                           word == "pict" || word == "header" ||
                           word == "footer" || word == "footnote" ||
                           word == "themedata" || word == "datastore") {
                    skip = true;
                } else if (word == "u" && hasNumber) {
                    long code = number < 0 ? number + 65536 : number;
                    if (!skip) out.push_back(static_cast<wchar_t>(code));
                    // The ANSI fallback character that follows is not shown.
                    if (i + 1 < n && rtf[i + 1] == '\\' && i + 2 < n &&
                        rtf[i + 2] == '\'') {
                        i += 4;
                    } else if (i + 1 < n) {
                        i += 1;
                    }
                }
            } else {
                i++;
            }
        } else if (ch != '\r' && ch != '\n') {
            appendByte(static_cast<unsigned char>(ch));
        }
    }
    return out;
}

}  // namespace qldoc

// Reads a document and extracts its text. Returns false when the file is not a
// document this code understands (an encrypted or legacy binary file, say); the
// caller then falls back to the Windows preview handler.
static bool ReadDocumentContent(const std::wstring& path, std::wstring& raw,
                                std::wstring& encoding, ULONGLONG& bytes,
                                const CancelCheck& cancel) {
    raw.clear();
    encoding.clear();
    bytes = 0;
    if (cancel()) {
        return false;
    }
    CancelScope cancelScope(&cancel);

    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER fileSize = {};
    GetFileSizeEx(file, &fileSize);
    bytes = static_cast<ULONGLONG>(fileSize.QuadPart);
    const ULONGLONG kMaxDocBytes = 150ull * 1024 * 1024;
    if (bytes < 4 || bytes > kMaxDocBytes) {
        CloseHandle(file);
        return false;
    }

    std::vector<unsigned char> data(static_cast<size_t>(bytes));
    size_t total = 0;
    while (total < data.size()) {
        if (cancel()) {
            break;
        }
        DWORD read = 0;
        DWORD chunk = static_cast<DWORD>(
            std::min<size_t>(data.size() - total, 16u * 1024 * 1024));
        if (!ReadFile(file, data.data() + total, chunk, &read, nullptr) ||
            read == 0) {
            break;
        }
        total += read;
    }
    CloseHandle(file);
    if (total != data.size()) {
        return false;
    }

    std::wstring extension = LowerExtension(path);
    std::wstring text;

    if (extension == L"rtf") {
        text = qldoc::ExtractRtf(std::string(data.begin(), data.end()));
        encoding = L"Rich Text (text only)";
    } else {
        qldoc::ZipArchive zip;
        zip.data = std::move(data);
        if (!qldoc::OpenZip(zip)) {
            return false;
        }
        auto starts = [&](const wchar_t* prefix) {
            return extension.rfind(prefix, 0) == 0;
        };
        if (starts(L"doc") || starts(L"dot")) {
            text = qldoc::ExtractDocx(zip);
            encoding = L"Word (text only)";
        } else if (starts(L"xls") || starts(L"xlt")) {
            text = qldoc::ExtractXlsx(zip);
            encoding = L"Excel (text only)";
        } else if (starts(L"ppt") || starts(L"pps") || starts(L"pot")) {
            text = qldoc::ExtractPptx(zip);
            encoding = L"PowerPoint (text only)";
        } else if (extension == L"odt") {
            text = qldoc::ExtractOdf(zip, 0);
            encoding = L"OpenDocument (text only)";
        } else if (extension == L"ods") {
            text = qldoc::ExtractOdf(zip, 1);
            encoding = L"OpenDocument (text only)";
        } else if (extension == L"odp") {
            text = qldoc::ExtractOdf(zip, 2);
            encoding = L"OpenDocument (text only)";
        } else {
            return false;
        }
    }

    bool blank = true;
    for (wchar_t ch : text) {
        if (!iswspace(ch)) {
            blank = false;
            break;
        }
    }
    if (cancel()) {
        return false;
    }
    if (blank) {
        text = L"This document has no text content to show.";
    } else if (text.size() > 1000000) {
        text.resize(1000000);
        text += L"\n\n[... truncated]";
    }
    raw = std::move(text);
    return true;
}

// ---------------------------------------------------------------------------
// Worker threads
// ---------------------------------------------------------------------------
//
// Explorer's UI thread must never wait for a disk, a network share, a cloud
// placeholder or a shell extension, so every such call runs on a worker. The
// handle of every worker is kept in a list: Wh_ModUninit asks the workers to
// stop and waits for each one before it returns, so no worker can ever run code
// of a mod image that Windhawk has already unloaded.

static std::atomic<int> g_activeWorkers{0};
static const int kMaxActiveWorkers = 12;

static std::mutex g_workersMutex;
static std::vector<HANDLE> g_workerHandles;

struct WorkerStart {
    LPTHREAD_START_ROUTINE proc;
    void* parameter;
};

static DWORD WINAPI WorkerTrampoline(void* parameter) {
    WorkerStart start = *static_cast<WorkerStart*>(parameter);
    delete static_cast<WorkerStart*>(parameter);
    start.proc(start.parameter);
    g_activeWorkers--;
    return 0;
}

// Closes the handles of the workers that already finished. Caller holds
// g_workersMutex.
static void PruneFinishedWorkersLocked() {
    for (auto it = g_workerHandles.begin(); it != g_workerHandles.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_workerHandles.erase(it);
        } else {
            ++it;
        }
    }
}

static bool StartWorker(LPTHREAD_START_ROUTINE proc, void* parameter) {
    std::lock_guard<std::mutex> lock(g_workersMutex);
    if (g_stopWorkers.load()) {
        return false;
    }
    // One cap for every load, not only the probe: a burst of selections must
    // not pile up threads each parsing or decoding a large file.
    if (g_activeWorkers.load() >= kMaxActiveWorkers) {
        return false;
    }
    PruneFinishedWorkersLocked();

    WorkerStart* start = new WorkerStart{proc, parameter};
    g_activeWorkers++;
    HANDLE thread = CreateThread(nullptr, 0, WorkerTrampoline, start, 0, nullptr);
    if (!thread) {
        g_activeWorkers--;
        delete start;
        return false;
    }
    g_workerHandles.push_back(thread);
    return true;
}

// Called by Wh_ModUninit: no new worker starts, and every running one is
// cancelled (a blocking file call is interrupted) and waited for.
static void StopAllWorkers() {
    std::vector<HANDLE> handles;
    {
        std::lock_guard<std::mutex> lock(g_workersMutex);
        g_stopWorkers = true;
        handles.swap(g_workerHandles);
    }
    for (HANDLE thread : handles) {
        // The wait has no timeout: returning while a worker still runs would
        // unmap code it is executing. The cancel is repeated because a worker
        // can enter a new blocking call after the first one.
        while (WaitForSingleObject(thread, 50) == WAIT_TIMEOUT) {
            CancelSynchronousIo(thread);
        }
        CloseHandle(thread);
    }
}

struct TextLoadResult {
    HWND panel = nullptr;
    unsigned generation = 0;
    std::shared_ptr<std::atomic<unsigned>> token;
    std::wstring path;
    std::wstring raw;
    std::wstring encoding;
    ULONGLONG bytes = 0;
    bool truncated = false;
    bool isText = false;
    // A Word / Excel / PowerPoint / OpenDocument / RTF file whose text is
    // extracted instead of read as plain text.
    bool document = false;
};

static DWORD WINAPI TextLoadThreadProc(void* parameter) {
    TextLoadResult* result = static_cast<TextLoadResult*>(parameter);
    CancelCheck cancel{&result->token, result->generation};
    if (cancel()) {
        delete result;
        return 0;
    }
    if (result->document) {
        result->isText = ReadDocumentContent(result->path, result->raw,
                                             result->encoding, result->bytes,
                                             cancel);
    } else {
        result->isText = ReadTextContent(result->path, result->raw,
                                         result->encoding, result->bytes,
                                         result->truncated);
    }
    if (cancel()) {
        delete result;
        return 0;
    }
    if (!result->panel ||
        !PostMessageW(result->panel, QL_WM_TEXT_LOADED, 0,
                      reinterpret_cast<LPARAM>(result))) {
        delete result;
    }
    return 0;
}

// Opens the preview immediately and leaves the file work to a worker thread, so
// a big file never blocks Explorer. A stale result is dropped by generation.
static void StartTextLoad(PanelState* state, bool document = false) {
    if (!state || state->path.empty() || !state->panel ||
        !IsWindow(state->panel)) {
        return;
    }

    ClearTextPreview(state);
    state->textLoading = true;

    TextLoadResult* result = new TextLoadResult();
    result->panel = state->panel;
    result->generation = state->loadGeneration;
    result->token = state->loadToken;
    result->path = state->path;
    result->document = document;

    if (!StartWorker(TextLoadThreadProc, result)) {
        state->textLoading = false;
        delete result;
    }
    SetTimer(state->panel, QL_TIMER_SPINNER, 70, nullptr);
    InvalidateRect(state->panel, nullptr, TRUE);
}

static void ApplyTextLoadResult(PanelState* state, TextLoadResult* result) {
    if (!state || !result) {
        return;
    }
    if (result->generation != state->loadGeneration) {
        return;
    }
    state->textLoading = false;

    if (!result->isText) {
        Wh_Log(L"text: no text preview for %s", result->path.c_str());
        if (result->document) {
            // Not a document the built-in reader understands: try the Windows
            // preview handler, then the properties view.
            FallbackDocumentPreview(state, result->bytes);
            if (state->panel && IsWindow(state->panel)) {
                LayoutPanelContent(state);
            }
        }
        if (state->panel && IsWindow(state->panel)) {
            InvalidateRect(state->panel, nullptr, TRUE);
        }
        return;
    }

    state->textRaw = std::move(result->raw);
    state->textEncoding = result->encoding;
    state->textBytes = result->bytes;
    state->textTruncated = result->truncated;
    state->textPreview = true;
    state->textScroll = 0;
    LayoutPanelContent(state);
    if (state->panel && IsWindow(state->panel)) {
        InvalidateRect(state->panel, nullptr, TRUE);
    }
    Wh_Log(L"text: %s bytes=%llu lines=%u encoding=%s",
          result->path.c_str(), (unsigned long long)result->bytes,
          (unsigned)state->textLines.size(), result->encoding.c_str());
}

// The content needs real contrast in both modes: the sampled Explorer text can
// be a dim gray that disappears on a dark panel, so the content color is derived
// from the sampled background instead of copied from it.
static COLORREF ContentTextColor(COLORREF background) {
    bool dark = ColorLuma(background) < 140;
    return dark ? BlendColor(background, RGB(255, 255, 255), 88)
                : BlendColor(background, RGB(0, 0, 0), 84);
}

static void DrawTextToolbar(HDC dc, PanelState* state, UINT dpi,
                            COLORREF background, COLORREF text,
                            COLORREF accent) {
    bool active[4] = {g_textWrap.load(), g_textLineNumbers.load(),
                      g_textWhitespace.load(), g_textZebra.load()};

    HFONT font = CreateFontW(
        -Scale(12, dpi), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HGDIOBJ previous = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);

    for (int i = 0; i < 4; i++) {
        RECT rect = state->textToolbar[i];
        if (rect.right <= rect.left) {
            continue;
        }
        bool hover = state->textToolbarHover == i;
        COLORREF fill = active[i] ? BlendColor(background, accent, 20)
                                  : (hover ? BlendColor(background, text, 10)
                                           : background);
        COLORREF border = active[i] ? BlendColor(background, accent, 70)
                                    : BlendColor(background, text, 16);
        HBRUSH brush = CreateSolidBrush(fill);
        HPEN pen = CreatePen(PS_SOLID, 1, border);
        HGDIOBJ oldBrush = SelectObject(dc, brush);
        HGDIOBJ oldPen = SelectObject(dc, pen);
        RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom,
                  Scale(6, dpi), Scale(6, dpi));
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(pen);
        DeleteObject(brush);

        COLORREF glyphColor = active[i] ? BlendColor(background, accent, 90)
                                        : BlendColor(background, text, 70);
        if (i == 0) {
            // A line that bends down at the right edge: wrapping.
            HPEN glyphPen = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)),
                                      glyphColor);
            HGDIOBJ oldGlyphPen = SelectObject(dc, glyphPen);
            int left = rect.left + Scale(7, dpi);
            int right = rect.right - Scale(7, dpi);
            int middle = (rect.top + rect.bottom) / 2;
            MoveToEx(dc, left, middle - Scale(3, dpi), nullptr);
            LineTo(dc, right, middle - Scale(3, dpi));
            LineTo(dc, right, middle + Scale(3, dpi));
            LineTo(dc, left + Scale(3, dpi), middle + Scale(3, dpi));
            LineTo(dc, left + Scale(6, dpi), middle + Scale(1, dpi));
            MoveToEx(dc, left + Scale(3, dpi), middle + Scale(3, dpi),
                     nullptr);
            LineTo(dc, left + Scale(6, dpi), middle + Scale(5, dpi));
            SelectObject(dc, oldGlyphPen);
            DeleteObject(glyphPen);
        } else if (i == 3) {
            // Two stacked bands, the first one filled: alternating row colors.
            int left = rect.left + Scale(7, dpi);
            int right = rect.right - Scale(7, dpi);
            int top = rect.top + Scale(8, dpi);
            int band = Scale(3, dpi);
            HBRUSH filled = CreateSolidBrush(glyphColor);
            RECT first = {left, top, right, top + band};
            FillRect(dc, &first, filled);
            DeleteObject(filled);
            HPEN outline = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)),
                                     glyphColor);
            HGDIOBJ oldOutline = SelectObject(dc, outline);
            HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
            Rectangle(dc, left, top + band + Scale(2, dpi), right,
                      top + band * 2 + Scale(2, dpi));
            SelectObject(dc, oldBrush);
            SelectObject(dc, oldOutline);
            DeleteObject(outline);
        } else {
            const wchar_t* label = i == 1 ? L"#" : L"\u00B7";
            SetTextColor(dc, glyphColor);
            RECT labelRect = rect;
            DrawTextW(dc, label, -1, &labelRect,
                      DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
        }
    }

    SelectObject(dc, previous);
    DeleteObject(font);
}

static void DrawCloseButton(HDC dc, PanelState* state, UINT dpi,
                            COLORREF background, COLORREF text) {
    RECT rect = state->closeRect;
    if (rect.right <= rect.left) {
        return;
    }

    if (state->closeHover) {
        HBRUSH fill = CreateSolidBrush(BlendColor(background, text, 10));
        HPEN noPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HGDIOBJ oldBrush = SelectObject(dc, fill);
        HGDIOBJ oldPen = SelectObject(dc, noPen);
        RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom,
                  Scale(6, dpi), Scale(6, dpi));
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(noPen);
        DeleteObject(fill);
    }

    COLORREF glyph = BlendColor(background, text, state->closeHover ? 90 : 65);
    HPEN pen = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)), glyph);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    int arm = Scale(4, dpi);
    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    MoveToEx(dc, cx - arm, cy - arm, nullptr);
    LineTo(dc, cx + arm + 1, cy + arm + 1);
    MoveToEx(dc, cx + arm, cy - arm, nullptr);
    LineTo(dc, cx - arm - 1, cy + arm + 1);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

static void DrawTextPreview(HDC dc, PanelState* state, UINT dpi,
                            const RECT& body) {
    HFONT font = CreateTextFont(dpi);
    HGDIOBJ previous = SelectObject(dc, font);
    TEXTMETRICW metrics = {};
    GetTextMetricsW(dc, &metrics);
    int lineHeight = static_cast<int>(metrics.tmHeight * 1.28);
    if (lineHeight < 1) {
        lineHeight = Scale(16, dpi);
    }

    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF content = ContentTextColor(background);

    if (state->textLines.empty()) {
        state->textVisibleLines = 0;
        state->scrollbarRect = {};
        state->scrollThumbRect = {};
        state->textHScrollRect = {};
        state->textHThumbRect = {};
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, BlendColor(background, content, 45));
        RECT message = body;
        DrawTextW(dc, state->filter.empty() ? L"Empty file" : L"No matches",
                  -1, &message,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, previous);
        DeleteObject(font);
        return;
    }

    int gutter = g_textLineNumbers.load() ? state->textGutterWidth : 0;
    int textLeft = body.left + gutter;

    // Without word wrap a line can be wider than the panel. The text area then
    // scrolls sideways, and a horizontal bar takes the bottom strip of the body.
    int textRight = body.right - Scale(12, dpi);
    int charWidth = std::max(1, state->textCharWidth);
    int visibleChars = std::max(1, (textRight - textLeft) / charWidth);
    state->textVisibleChars = visibleChars;
    bool needHorizontal =
        !g_textWrap.load() && state->textMaxChars > visibleChars;
    int horizontalReserve = needHorizontal ? Scale(12, dpi) : 0;
    int maxScrollX = needHorizontal ? state->textMaxChars - visibleChars : 0;
    state->textScrollX = std::max(0, std::min(state->textScrollX, maxScrollX));

    int visible = std::max(1, (static_cast<int>(body.bottom - body.top) -
                               horizontalReserve) /
                                  lineHeight);
    state->textVisibleLines = visible;

    int total = static_cast<int>(state->textLines.size());
    int maximum = std::max(0, total - visible);
    state->textScroll = std::max(0, std::min(state->textScroll, maximum));

    SetBkMode(dc, TRANSPARENT);
    int y = body.top;
    for (int row = 0; row < visible; row++) {
        int index = state->textScroll + row;
        if (index >= total) {
            break;
        }

        // Alternating row color: one source line tinted, the next one not, and
        // a wrapped continuation keeps the tint of the line it belongs to.
        if (g_textZebra.load() &&
            index < static_cast<int>(state->textLineSources.size()) &&
            (state->textLineSources[index] & 1)) {
            RECT band = {body.left, y, body.right, y + lineHeight};
            HBRUSH brush = CreateSolidBrush(BlendColor(background, content, 7));
            FillRect(dc, &band, brush);
            DeleteObject(brush);
        }

        if (gutter > 0 && index < static_cast<int>(state->textLineNumbers.size())) {
            int number = state->textLineNumbers[index];
            if (number > 0) {
                std::wstring label = Fmt(L"%d", number);
                SetTextColor(dc, BlendColor(background, content, 42));
                int labelWidth = static_cast<int>(label.size()) *
                                 state->textCharWidth;
                TextOutW(dc, body.left + gutter - Scale(6, dpi) - labelWidth,
                         y, label.c_str(), static_cast<int>(label.size()));
            }
        }

        // Text runs are grouped by state: a search match paints the accent
        // behind it, a whitespace dot is dimmer than the real text, and the
        // rest uses the content color.
        const std::wstring& line = state->textLines[index];
        const std::vector<std::pair<int, int>>* matches =
            index < static_cast<int>(state->textLineMatches.size())
                ? &state->textLineMatches[index]
                : nullptr;
        auto matched = [&](int position) {
            if (!matches) {
                return false;
            }
            for (const auto& range : *matches) {
                if (position >= range.first && position < range.second) {
                    return true;
                }
            }
            return false;
        };

        // Only the characters that can be seen are looked at, so a huge
        // single-line file costs the same to paint as a short one.
        COLORREF dotColor = BlendColor(background, content, 34);
        size_t firstChar = static_cast<size_t>(state->textScrollX);
        size_t lastChar = std::min(
            line.size(), firstChar + static_cast<size_t>(visibleChars) + 2);
        int rowClip = SaveDC(dc);
        IntersectClipRect(dc, textLeft, y, textRight, y + lineHeight);
        size_t pos = firstChar;
        while (pos < lastChar) {
            bool isMatch = matched(static_cast<int>(pos));
            bool isDot = !isMatch && g_textWhitespace.load() &&
                         line[pos] == L'\u00B7';
            size_t end = pos + 1;
            while (end < lastChar) {
                bool nextMatch = matched(static_cast<int>(end));
                bool nextDot = !nextMatch && g_textWhitespace.load() &&
                               line[end] == L'\u00B7';
                if (nextMatch != isMatch || nextDot != isDot) {
                    break;
                }
                end++;
            }

            int x = textLeft + (static_cast<int>(pos) - state->textScrollX) *
                                   state->textCharWidth;
            if (isMatch) {
                COLORREF accent = state->accent != CLR_INVALID
                                      ? state->accent
                                      : GetUserAccentColor();
                RECT highlight = {
                    x, y, x + static_cast<int>(end - pos) * state->textCharWidth,
                    y + lineHeight};
                HBRUSH brush = CreateSolidBrush(accent);
                FillRect(dc, &highlight, brush);
                DeleteObject(brush);
                SetTextColor(dc, ColorLuma(accent) > 140 ? RGB(0, 0, 0)
                                                         : RGB(255, 255, 255));
            } else {
                SetTextColor(dc, isDot ? dotColor : content);
            }
            TextOutW(dc, x, y, line.c_str() + pos,
                     static_cast<int>(end - pos));
            pos = end;
        }
        RestoreDC(dc, rowClip);
        y += lineHeight;
    }

    state->scrollbarRect = {};
    state->scrollThumbRect = {};
    state->textHScrollRect = {};
    state->textHThumbRect = {};
    if (total > visible) {
        int trackWidth = Scale(6, dpi);
        RECT track = {body.right - trackWidth, body.top, body.right,
                      body.bottom - horizontalReserve};
        int trackHeight = track.bottom - track.top;
        int thumbHeight = std::max(Scale(20, dpi),
                                   trackHeight * visible / std::max(1, total));
        thumbHeight = std::min(thumbHeight, trackHeight);
        int span = trackHeight - thumbHeight;
        int offset = maximum > 0 ? span * state->textScroll / maximum : 0;
        state->scrollbarRect = track;
        state->scrollThumbRect = {track.left, track.top + offset, track.right,
                                  track.top + offset + thumbHeight};

        HBRUSH thumb = CreateSolidBrush(BlendColor(background, content, 32));
        HPEN noPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HGDIOBJ oldBrush = SelectObject(dc, thumb);
        HGDIOBJ oldPen = SelectObject(dc, noPen);
        RoundRect(dc, state->scrollThumbRect.left,
                  state->scrollThumbRect.top, state->scrollThumbRect.right,
                  state->scrollThumbRect.bottom, trackWidth, trackWidth);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(noPen);
        DeleteObject(thumb);
    }

    if (needHorizontal) {
        int trackHeight = Scale(6, dpi);
        RECT track = {textLeft, body.bottom - trackHeight, textRight,
                      body.bottom};
        int trackWidth = track.right - track.left;
        int thumbWidth = std::max(
            Scale(20, dpi),
            trackWidth * visibleChars / std::max(1, state->textMaxChars));
        thumbWidth = std::min(thumbWidth, trackWidth);
        int span = trackWidth - thumbWidth;
        int offset = maxScrollX > 0 ? span * state->textScrollX / maxScrollX : 0;
        state->textHScrollRect = track;
        state->textHThumbRect = {track.left + offset, track.top,
                                 track.left + offset + thumbWidth,
                                 track.bottom};

        HBRUSH thumb = CreateSolidBrush(BlendColor(background, content, 32));
        HPEN noPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HGDIOBJ oldBrush = SelectObject(dc, thumb);
        HGDIOBJ oldPen = SelectObject(dc, noPen);
        RoundRect(dc, state->textHThumbRect.left, state->textHThumbRect.top,
                  state->textHThumbRect.right, state->textHThumbRect.bottom,
                  trackHeight, trackHeight);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(noPen);
        DeleteObject(thumb);
    }

    SelectObject(dc, previous);
    DeleteObject(font);
}

// ---------------------------------------------------------------------------
// Image, video, PDF and audio previews
// ---------------------------------------------------------------------------

static std::wstring LowerExtension(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos ||
        (slash != std::wstring::npos && dot < slash)) {
        return std::wstring();
    }
    std::wstring extension = path.substr(dot + 1);
    for (wchar_t& ch : extension) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return extension;
}

static bool MatchesExtension(const std::wstring& extension,
                             const wchar_t* const* list, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (extension == list[i]) {
            return true;
        }
    }
    return false;
}

#define QL_EXT_LIST(name, ...)                                            \
    static const wchar_t* const name[] = {__VA_ARGS__};                   \
    static const size_t name##Count = sizeof(name) / sizeof(name[0])

QL_EXT_LIST(kImageExtensions, L"png", L"jpg", L"jpeg", L"jpe", L"jfif",
            L"bmp", L"dib", L"gif", L"tif", L"tiff", L"ico", L"webp",
            L"heic", L"avif");
QL_EXT_LIST(kVideoExtensions, L"mp4", L"mpg4", L"mpeg4", L"m4v", L"mkv",
            L"webm", L"avi", L"mov", L"wmv", L"mpg", L"mpeg", L"mpe", L"mts",
            L"m2ts", L"ts", L"flv", L"f4v", L"3gp", L"3g2", L"ogv", L"vob",
            L"asf");
QL_EXT_LIST(kAudioExtensions, L"mp3", L"flac", L"wav", L"m4a", L"aac",
            L"ogg", L"opus", L"wma", L"mid", L"midi");
QL_EXT_LIST(kPdfExtensions, L"pdf");
// Formats shown through a Windows preview handler (registered by Office or
// another program). Whether a handler exists is checked when the file is opened.
QL_EXT_LIST(kOfficeExtensions, L"doc", L"docx", L"docm", L"dot", L"dotx",
            L"dotm", L"rtf", L"odt", L"xls", L"xlsx", L"xlsm", L"xlsb",
            L"xlt", L"xltx", L"xltm", L"ods", L"ppt", L"pptx", L"pptm",
            L"pps", L"ppsx", L"ppsm", L"pot", L"potx", L"potm", L"odp",
            L"vsd", L"vsdx", L"vsdm", L"msg", L"eml", L"xps", L"oxps");

#undef QL_EXT_LIST

static bool IsImageExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kImageExtensions, kImageExtensionsCount);
}

static bool IsVideoExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kVideoExtensions, kVideoExtensionsCount);
}

static bool IsAudioExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kAudioExtensions, kAudioExtensionsCount);
}

static bool IsPdfExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kPdfExtensions, kPdfExtensionsCount);
}

static bool IsOfficeExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kOfficeExtensions,
                            kOfficeExtensionsCount);
}

// Footer label for a document preview. `extension` has no leading dot.
static const wchar_t* OfficeKindLabel(const std::wstring& extension) {
    auto starts = [&](const wchar_t* prefix) {
        return extension.rfind(prefix, 0) == 0;
    };
    if (starts(L"doc") || starts(L"dot") || extension == L"rtf" ||
        extension == L"odt") {
        return L"Word";
    }
    if (starts(L"xls") || starts(L"xlt") || extension == L"ods") {
        return L"Excel";
    }
    if (starts(L"ppt") || starts(L"pps") || starts(L"pot") ||
        extension == L"odp") {
        return L"PowerPoint";
    }
    if (starts(L"vsd")) {
        return L"Visio";
    }
    return L"Document";
}

static const wchar_t* const kArchiveExtensions[] = {
    L"zip", L"rar", L"7z", L"tar", L"gz", L"tgz", L"bz2", L"xz", L"cab",
    L"iso"};
static const size_t kArchiveExtensionsCount =
    sizeof(kArchiveExtensions) / sizeof(kArchiveExtensions[0]);

static bool IsArchiveExtension(const std::wstring& extension) {
    return MatchesExtension(extension, kArchiveExtensions,
                            kArchiveExtensionsCount);
}

static void ClearPropertyIcon(PanelState* state) {
    if (!state) {
        return;
    }
    if (state->propertyIcon) {
        DestroyIcon(state->propertyIcon);
        state->propertyIcon = nullptr;
    }
}

static void ClearMediaPreview(PanelState* state) {
    if (!state) {
        return;
    }
    UnloadPdfPreview(state);
    state->mediaPreview = false;
    state->mediaPending = false;
    if (state->mediaBitmap) {
        DeleteObject(state->mediaBitmap);
        state->mediaBitmap = nullptr;
    }
    state->mediaWidth = 0;
    state->mediaHeight = 0;
    state->mediaSourceWidth = 0;
    state->mediaSourceHeight = 0;
    state->mediaBytes = 0;
    state->mediaHasAlpha = false;
    state->mediaKind.clear();
}

// Videos, PDFs and audio only need a still image, and Windows already knows how
// to produce one through the thumbnail providers.
static HBITMAP LoadShellThumbnail(const std::wstring& path, int size) {
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_IShellItem, (void**)&item)) ||
        !item) {
        return nullptr;
    }

    HBITMAP bitmap = nullptr;
    IShellItemImageFactory* factory = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_IShellItemImageFactory,
                                       (void**)&factory)) &&
        factory) {
        SIZE requested = {size, size};
        if (FAILED(factory->GetImage(
                requested,
                static_cast<SIIGBF>(SIIGBF_RESIZETOFIT | SIIGBF_BIGGERSIZEOK |
                                    SIIGBF_THUMBNAILONLY),
                &bitmap))) {
            bitmap = nullptr;
        }
        factory->Release();
    }
    item->Release();
    return bitmap;
}

// Windows only produces a thumbnail for a video when the shell has a thumbnail
// provider registered for that extension and a codec is installed. A file named
// .mpg4, for instance, gets nothing. This decodes the first frame with Media
// Foundation instead, so any container it can open previews: the extension only
// decides that the file is a video, not that Windows knows what to do with it.
static HBITMAP LoadVideoFrameMediaFoundation(const std::wstring& path,
                                             int& width, int& height) {
    width = 0;
    height = 0;
    if (!g_mediaFoundation.load()) {
        return nullptr;
    }

    IMFAttributes* attributes = nullptr;
    IMFSourceReader* reader = nullptr;
    IMFMediaType* outputType = nullptr;
    IMFMediaType* currentType = nullptr;
    IMFSample* sample = nullptr;
    IMFMediaBuffer* buffer = nullptr;
    IMF2DBuffer* buffer2d = nullptr;
    HBITMAP result = nullptr;

    if (FAILED(MFCreateAttributes(&attributes, 1)) || !attributes) {
        return nullptr;
    }
    attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), attributes, &reader);
    attributes->Release();
    if (FAILED(hr) || !reader) {
        return nullptr;
    }

    reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    reader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);

    if (SUCCEEDED(MFCreateMediaType(&outputType)) && outputType &&
        SUCCEEDED(outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video)) &&
        SUCCEEDED(outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32))) {
        hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                         nullptr, outputType);
    }
    if (outputType) {
        outputType->Release();
    }
    if (FAILED(hr)) {
        reader->Release();
        return nullptr;
    }

    // A second in, so the frame is not the black or title first frame.
    PROPVARIANT position;
    PropVariantInit(&position);
    position.vt = VT_I8;
    position.hVal.QuadPart = 10000000;
    reader->SetCurrentPosition(GUID_NULL, position);
    PropVariantClear(&position);

    for (int attempt = 0; attempt < 40 && !result; attempt++) {
        if (g_stopWorkers.load()) {
            break;
        }
        DWORD flags = 0;
        LONGLONG timestamp = 0;
        hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr,
                                &flags, &timestamp, &sample);
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
            break;
        }
        if (!sample) {
            continue;
        }
        if (FAILED(sample->ConvertToContiguousBuffer(&buffer)) || !buffer) {
            sample->Release();
            sample = nullptr;
            continue;
        }
        if (FAILED(reader->GetCurrentMediaType(
                MF_SOURCE_READER_FIRST_VIDEO_STREAM, &currentType)) ||
            !currentType) {
            buffer->Release();
            sample->Release();
            buffer = nullptr;
            sample = nullptr;
            continue;
        }

        UINT32 frameWidth = 0;
        UINT32 frameHeight = 0;
        MFGetAttributeSize(currentType, MF_MT_FRAME_SIZE, &frameWidth,
                           &frameHeight);
        if (frameWidth == 0 || frameHeight == 0 || frameWidth > 8192 ||
            frameHeight > 8192) {
            currentType->Release();
            buffer->Release();
            sample->Release();
            currentType = nullptr;
            buffer = nullptr;
            sample = nullptr;
            continue;
        }

        // The decoder pads to a multiple of 16; the aperture says what is
        // actually visible.
        int cropLeft = 0;
        int cropTop = 0;
        int cropWidth = static_cast<int>(frameWidth);
        int cropHeight = static_cast<int>(frameHeight);
        MFVideoArea aperture = {};
        UINT32 apertureSize = 0;
        if (SUCCEEDED(currentType->GetBlob(
                MF_MT_MINIMUM_DISPLAY_APERTURE,
                reinterpret_cast<UINT8*>(&aperture), sizeof(aperture),
                &apertureSize)) &&
            apertureSize >= sizeof(MFVideoArea) && aperture.Area.cx > 0 &&
            aperture.Area.cy > 0) {
            // MFOffset splits the value into a signed integer and a fraction.
            int offsetX = aperture.OffsetX.value;
            int offsetY = aperture.OffsetY.value;
            if (offsetX >= 0 && offsetY >= 0 &&
                offsetX + aperture.Area.cx <= static_cast<LONG>(frameWidth) &&
                offsetY + aperture.Area.cy <= static_cast<LONG>(frameHeight)) {
                cropLeft = offsetX;
                cropTop = offsetY;
                cropWidth = static_cast<int>(aperture.Area.cx);
                cropHeight = static_cast<int>(aperture.Area.cy);
            }
        }

        BYTE* scanline = nullptr;
        LONG pitch = 0;
        bool locked2d = false;
        if (SUCCEEDED(buffer->QueryInterface(IID_IMF2DBuffer,
                                             (void**)&buffer2d)) &&
            buffer2d) {
            if (SUCCEEDED(buffer2d->Lock2D(&scanline, &pitch))) {
                locked2d = true;
            }
        }
        if (!scanline) {
            DWORD length = 0;
            if (SUCCEEDED(buffer->Lock(&scanline, nullptr, &length))) {
                MFGetStrideForBitmapInfoHeader(MFVideoFormat_RGB32.Data1,
                                               frameWidth, &pitch);
            }
        }

        if (scanline && pitch != 0) {
            BITMAPINFO info = {};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = cropWidth;
            info.bmiHeader.biHeight = -cropHeight;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            void* bits = nullptr;
            HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                                              &bits, nullptr, 0);
            if (bitmap && bits) {
                LONG sourceStride = pitch < 0 ? -pitch : pitch;
                for (int y = 0; y < cropHeight; y++) {
                    int visibleRow = cropTop + y;
                    int sourceRow = pitch < 0
                                        ? static_cast<int>(frameHeight) - 1 -
                                              visibleRow
                                        : visibleRow;
                    const BYTE* source =
                        scanline + static_cast<size_t>(sourceRow) *
                                       static_cast<size_t>(sourceStride) +
                        static_cast<size_t>(cropLeft) * 4;
                    BYTE* target = reinterpret_cast<BYTE*>(bits) +
                                   static_cast<size_t>(y) * cropWidth * 4;
                    memcpy(target, source,
                           static_cast<size_t>(cropWidth) * 4);
                    // The video processor leaves a zero alpha channel, which
                    // would make the frame invisible when it is blended.
                    for (int x = 0; x < cropWidth; x++) {
                        target[x * 4 + 3] = 0xFF;
                    }
                }
                width = cropWidth;
                height = cropHeight;
                result = bitmap;
            }
        }

        if (locked2d && buffer2d) {
            buffer2d->Unlock2D();
        } else if (scanline) {
            buffer->Unlock();
        }
        if (buffer2d) {
            buffer2d->Release();
            buffer2d = nullptr;
        }
        currentType->Release();
        currentType = nullptr;
        buffer->Release();
        buffer = nullptr;
        sample->Release();
        sample = nullptr;
    }

    reader->Release();
    return result;
}

// Images are decoded at full resolution through WIC and converted to a
// premultiplied 32bpp DIB, which AlphaBlend can draw with its corners intact.
// Images are decoded through WIC into a premultiplied 32bpp DIB, which
// AlphaBlend can draw with its corners intact. The decoded copy is capped at
// kMaxSide pixels on its longest edge: a 100 megapixel photo would otherwise need
// 400 MB of memory inside Explorer, and the panel is never that large anyway.
// The original size is reported separately for the footer.
static HBITMAP LoadWicBitmap(const std::wstring& path, int& width, int& height,
                             int& sourceWidth, int& sourceHeight) {
    width = 0;
    height = 0;
    sourceWidth = 0;
    sourceHeight = 0;
    const UINT kMaxSide = 2560;
    const ULONGLONG kMaxDecodePixels = 300ull * 1000 * 1000;

    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                (void**)&factory)) ||
        !factory) {
        return nullptr;
    }

    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    HBITMAP result = nullptr;

    if (SUCCEEDED(factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, &decoder)) &&
        decoder && SUCCEEDED(decoder->GetFrame(0, &frame)) && frame &&
        SUCCEEDED(factory->CreateFormatConverter(&converter)) && converter &&
        SUCCEEDED(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
                                        WICBitmapDitherTypeNone, nullptr, 0.0,
                                        WICBitmapPaletteTypeCustom))) {
        UINT wicWidth = 0;
        UINT wicHeight = 0;
        if (SUCCEEDED(frame->GetSize(&wicWidth, &wicHeight)) && wicWidth > 0 &&
            wicHeight > 0 &&
            static_cast<ULONGLONG>(wicWidth) * wicHeight <= kMaxDecodePixels) {
            sourceWidth = static_cast<int>(wicWidth);
            sourceHeight = static_cast<int>(wicHeight);

            UINT outWidth = wicWidth;
            UINT outHeight = wicHeight;
            IWICBitmapSource* source = converter;
            UINT longest = std::max(wicWidth, wicHeight);
            if (longest > kMaxSide) {
                double factorDown = static_cast<double>(kMaxSide) / longest;
                UINT scaledWidth = std::max<UINT>(
                    1, static_cast<UINT>(wicWidth * factorDown));
                UINT scaledHeight = std::max<UINT>(
                    1, static_cast<UINT>(wicHeight * factorDown));
                if (SUCCEEDED(factory->CreateBitmapScaler(&scaler)) && scaler &&
                    SUCCEEDED(scaler->Initialize(
                        converter, scaledWidth, scaledHeight,
                        WICBitmapInterpolationModeFant))) {
                    source = scaler;
                    outWidth = scaledWidth;
                    outHeight = scaledHeight;
                }
            }

            BITMAPINFO info = {};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = static_cast<LONG>(outWidth);
            info.bmiHeader.biHeight = -static_cast<LONG>(outHeight);
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            void* bits = nullptr;
            result = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits,
                                      nullptr, 0);
            if (result && bits) {
                UINT stride = outWidth * 4;
                if (FAILED(source->CopyPixels(
                        nullptr, stride, stride * outHeight,
                        reinterpret_cast<BYTE*>(bits)))) {
                    DeleteObject(result);
                    result = nullptr;
                } else {
                    width = static_cast<int>(outWidth);
                    height = static_cast<int>(outHeight);
                }
            } else if (result) {
                DeleteObject(result);
                result = nullptr;
            }
        }
    }

    if (scaler) {
        scaler->Release();
    }
    if (converter) {
        converter->Release();
    }
    if (frame) {
        frame->Release();
    }
    if (decoder) {
        decoder->Release();
    }
    factory->Release();
    return result;
}

static bool ReadPreviewHandlerClsid(const std::wstring& extension,
                                    CLSID& clsid) {
    static const wchar_t* kHandlerKey =
        L"\\ShellEx\\{8895b1c6-b41f-4c1c-a562-0d564250836f}";
    std::wstring key = extension + kHandlerKey;
    wchar_t value[128] = {};
    DWORD size = sizeof(value);
    LONG result = RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr,
                               RRF_RT_REG_SZ, nullptr, value, &size);
    if (result != ERROR_SUCCESS || value[0] == L'\0') {
        // The extension may delegate its shell verbs to a ProgID.
        wchar_t progId[128] = {};
        DWORD progSize = sizeof(progId);
        if (RegGetValueW(HKEY_CLASSES_ROOT, extension.c_str(), nullptr,
                         RRF_RT_REG_SZ, nullptr, progId, &progSize) !=
                ERROR_SUCCESS ||
            progId[0] == L'\0') {
            return false;
        }
        key = std::wstring(progId) + kHandlerKey;
        size = sizeof(value);
        if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr,
                         RRF_RT_REG_SZ, nullptr, value, &size) !=
            ERROR_SUCCESS) {
            return false;
        }
    }
    return SUCCEEDED(CLSIDFromString(value, &clsid));
}

static void UnloadPdfPreview(PanelState* state) {
    if (!state || !state->previewHandler) {
        return;
    }
    state->previewHandler->Unload();
    state->previewHandler->Release();
    state->previewHandler = nullptr;
    state->mediaHandler = false;
}

static void PositionPdfPreview(PanelState* state) {
    if (!state || !state->previewHandler || !state->panel ||
        !IsWindow(state->panel)) {
        return;
    }
    UINT dpi = GetDpiForWindowSafe(state->panel);
    RECT client = {};
    GetClientRect(state->panel, &client);
    RECT body = client;
    body.left += Scale(16, dpi);
    body.right -= Scale(16, dpi);
    body.top = Scale(46, dpi) + Scale(12, dpi);
    body.bottom = client.bottom - Scale(42, dpi);
    if (body.bottom < body.top) {
        body.bottom = body.top;
    }
    state->previewHandler->SetRect(&body);
}

#ifndef CLSCTX_ACTIVATE_32_BIT_SERVER
#define CLSCTX_ACTIVATE_32_BIT_SERVER 0x40000
#endif

// Also looks under SystemFileAssociations, where some programs register their
// preview handler instead of on the extension itself.
static bool FindPreviewHandlerClsid(const std::wstring& extension,
                                    CLSID& clsid) {
    // First ask the shell, which is how Explorer itself resolves the handler:
    // it follows the ProgID and the per-user association chain.
    wchar_t assoc[128] = {};
    DWORD assocLength = ARRAYSIZE(assoc);
    if (SUCCEEDED(AssocQueryStringW(
            ASSOCF_INIT_DEFAULTTOSTAR, ASSOCSTR_SHELLEXTENSION,
            extension.c_str(), L"{8895b1c6-b41f-4c1c-a562-0d564250836f}",
            assoc, &assocLength)) &&
        assoc[0] != L'\0' && SUCCEEDED(CLSIDFromString(assoc, &clsid))) {
        return true;
    }
    if (ReadPreviewHandlerClsid(extension, clsid)) {
        return true;
    }
    std::wstring key = L"SystemFileAssociations\\" + extension +
                       L"\\ShellEx\\{8895b1c6-b41f-4c1c-a562-0d564250836f}";
    wchar_t value[128] = {};
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, RRF_RT_REG_SZ,
                     nullptr, value, &size) != ERROR_SUCCESS ||
        value[0] == L'\0') {
        return false;
    }
    return SUCCEEDED(CLSIDFromString(value, &clsid));
}

// Hands the file to a freshly created handler. A handler may expose an
// interface and still answer E_NOTIMPL from it, so every way is tried in turn:
// by path, by stream (what Office really uses) and by shell item. What each one
// answered is appended to `trace`.
static HRESULT InitializePreviewHandler(IUnknown* unknown,
                                        const std::wstring& path,
                                        std::wstring& trace) {
    HRESULT result = E_NOINTERFACE;

    IInitializeWithStream* initializeStream = nullptr;
    HRESULT qi = unknown->QueryInterface(IID_IInitializeWithStream,
                                         (void**)&initializeStream);
    if (SUCCEEDED(qi) && initializeStream) {
        IStream* stream = nullptr;
        HRESULT hr = SHCreateStreamOnFileEx(
            path.c_str(), STGM_READ | STGM_SHARE_DENY_NONE,
            FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &stream);
        if (SUCCEEDED(hr) && stream) {
            hr = initializeStream->Initialize(stream, STGM_READ);
            stream->Release();
        }
        trace += Fmt(L"stream=0x%08lX ", (unsigned long)hr);
        initializeStream->Release();
        if (SUCCEEDED(hr)) {
            return hr;
        }
        result = hr;
    } else {
        trace += Fmt(L"stream=no(0x%08lX) ", (unsigned long)qi);
    }

    IInitializeWithFile* initializeFile = nullptr;
    qi = unknown->QueryInterface(IID_IInitializeWithFile,
                                 (void**)&initializeFile);
    if (SUCCEEDED(qi) && initializeFile) {
        HRESULT hr = initializeFile->Initialize(path.c_str(), STGM_READ);
        trace += Fmt(L"file=0x%08lX ", (unsigned long)hr);
        initializeFile->Release();
        if (SUCCEEDED(hr)) {
            return hr;
        }
        result = hr;
    } else {
        trace += Fmt(L"file=no(0x%08lX) ", (unsigned long)qi);
    }

    IInitializeWithItem* initializeItem = nullptr;
    qi = unknown->QueryInterface(IID_IInitializeWithItem,
                                 (void**)&initializeItem);
    if (SUCCEEDED(qi) && initializeItem) {
        IShellItem* item = nullptr;
        HRESULT hr = SHCreateItemFromParsingName(
            path.c_str(), nullptr, IID_IShellItem, (void**)&item);
        if (SUCCEEDED(hr) && item) {
            hr = initializeItem->Initialize(item, STGM_READ);
            item->Release();
        }
        trace += Fmt(L"item=0x%08lX; ", (unsigned long)hr);
        initializeItem->Release();
        if (SUCCEEDED(hr)) {
            return hr;
        }
        result = hr;
    } else {
        trace += Fmt(L"item=no(0x%08lX); ", (unsigned long)qi);
    }
    return result;
}

// The preview handler comes from Windows itself (the same one Explorer's
// preview pane uses), so the page is rendered by the system and not by this mod.
// It serves PDFs and every document format a program has registered a handler
// for (Office, OpenDocument, ...). `extension` includes the leading dot.
// `diag` (optional) receives what went wrong when the handler fails; it belongs
// to the caller, so two Explorer windows loading a preview at the same time
// never share it.
static bool LoadPdfPreview(PanelState* state,
                           const std::wstring& extension = L".pdf",
                           std::wstring* diag = nullptr) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return false;
    }
    UnloadPdfPreview(state);

    std::wstring localDiag;
    std::wstring& diagOut = diag ? *diag : localDiag;
    diagOut.clear();
    CLSID clsid = {};
    if (!FindPreviewHandlerClsid(extension, clsid)) {
        diagOut = L"no preview handler is registered for " + extension;
        Wh_Log(L"preview: no handler registered for %s", extension.c_str());
        return false;
    }
    wchar_t clsidText[64] = {};
    StringFromGUID2(clsid, clsidText, ARRAYSIZE(clsidText));

    // A 32-bit Office has its handler in the 32-bit registry view, which a
    // 64-bit Explorer does not see by default: retry asking COM for a 32-bit
    // (then 64-bit) server explicitly.
    // The handler is created out of process only (the prevhost.exe surrogate
    // Explorer's own preview pane uses), then as a 32-bit server. It is never
    // loaded into explorer.exe: a handler that crashes or hangs must not take
    // the shell down with it. Each way gets the full initialization cascade and
    // the first one that accepts the file wins.
    struct CreateAttempt {
        DWORD context;
        const wchar_t* name;
    };
    const CreateAttempt attempts[] = {
        {CLSCTX_LOCAL_SERVER, L"out-of-process"},
        {CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER, L"32-bit"},
    };
    IUnknown* unknown = nullptr;
    HRESULT hr = E_FAIL;
    std::wstring trace;
    bool created = false;
    HRESULT lastCreate = E_FAIL;
    for (const CreateAttempt& attempt : attempts) {
        IUnknown* candidate = nullptr;
        HRESULT createHr = CoCreateInstance(clsid, nullptr, attempt.context,
                                            IID_IUnknown,
                                            (void**)&candidate);
        Wh_Log(L"preview: %s create hr=0x%08lX", attempt.name,
              (unsigned long)createHr);
        trace += Fmt(L"%s: ", attempt.name);
        if (FAILED(createHr) || !candidate) {
            lastCreate = createHr;
            trace += Fmt(L"create=0x%08lX; ", (unsigned long)createHr);
            continue;
        }
        created = true;
        HRESULT initHr = InitializePreviewHandler(candidate, state->path, trace);
        if (SUCCEEDED(initHr)) {
            unknown = candidate;
            hr = initHr;
            break;
        }
        hr = initHr;
        candidate->Release();
    }
    if (!unknown) {
        // The panel shows a short reason; the CLSID and the HRESULT trace are
        // only useful in the Windhawk log.
        diagOut = created ? L"the preview handler could not open this file"
                          : L"the preview handler could not be started";
        Wh_Log(L"preview: handler %s failed (lastCreate=0x%08lX): %s",
               clsidText, (unsigned long)lastCreate, trace.c_str());
        return false;
    }

    IPreviewHandler* handler = nullptr;
    if (SUCCEEDED(hr) &&
        SUCCEEDED(unknown->QueryInterface(IID_IPreviewHandler,
                                          (void**)&handler)) &&
        handler) {
        state->previewHandler = handler;
        RECT client = {};
        GetClientRect(state->panel, &client);
        UINT dpi = GetDpiForWindowSafe(state->panel);
        RECT body = client;
        body.left += Scale(16, dpi);
        body.right -= Scale(16, dpi);
        body.top = Scale(46, dpi) + Scale(12, dpi);
        body.bottom = client.bottom - Scale(42, dpi);

        hr = handler->SetWindow(state->panel, &body);
        if (SUCCEEDED(hr)) {
            handler->SetRect(&body);
            hr = handler->DoPreview();
        }
        if (SUCCEEDED(hr)) {
            state->mediaHandler = true;
        } else {
            diagOut = L"the preview handler could not render this file";
            Wh_Log(L"preview: handler %s render failed (0x%08lX)", clsidText,
                   (unsigned long)hr);
            UnloadPdfPreview(state);
        }
    } else {
        UnloadPdfPreview(state);
    }

    unknown->Release();
    Wh_Log(L"pdf: handler=%p hr=0x%08lX", (void*)state->previewHandler,
          (unsigned long)hr);
    return state->mediaHandler;
}

struct ImageLoadResult {
    HWND panel = nullptr;
    unsigned generation = 0;
    std::shared_ptr<std::atomic<unsigned>> token;
    std::wstring path;
    ULONGLONG bytes = 0;
    HBITMAP bitmap = nullptr;
    int width = 0;
    int height = 0;
    int sourceWidth = 0;
    int sourceHeight = 0;
    bool fromWic = false;
    bool ok = false;
};

static HBITMAP LoadShellThumbnail(const std::wstring& path, int size);

static DWORD WINAPI ImageLoadThreadProc(void* parameter) {
    ImageLoadResult* result = static_cast<ImageLoadResult*>(parameter);
    CancelCheck cancel{&result->token, result->generation};
    if (cancel()) {
        delete result;
        return 0;
    }
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (!cancel()) {
        result->bitmap =
            LoadWicBitmap(result->path, result->width, result->height,
                          result->sourceWidth, result->sourceHeight);
        result->fromWic = result->bitmap != nullptr;
        if (!result->bitmap && !cancel()) {
            // No WIC codec (or an image too large to decode): the shell may
            // still have a thumbnail. This runs here, never on the UI thread.
            result->bitmap = LoadShellThumbnail(result->path, 1024);
            if (result->bitmap) {
                BITMAP info = {};
                if (GetObjectW(result->bitmap, sizeof(info), &info)) {
                    result->width = info.bmWidth;
                    result->height = info.bmHeight;
                }
                result->sourceWidth = 0;
                result->sourceHeight = 0;
            }
        }
    }
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    result->ok = result->bitmap != nullptr;
    if (cancel()) {
        if (result->bitmap) {
            DeleteObject(result->bitmap);
            result->bitmap = nullptr;
        }
        delete result;
        return 0;
    }
    if (!result->panel ||
        !PostMessageW(result->panel, QL_WM_IMAGE_LOADED, 0,
                      reinterpret_cast<LPARAM>(result))) {
        if (result->bitmap) {
            DeleteObject(result->bitmap);
        }
        delete result;
    }
    return 0;
}

// A large photo takes long enough to decode that doing it inline would freeze
// Explorer, so the panel opens and paints at once and the bitmap arrives later.
// The file size comes from the probe, so nothing here touches the disk.
static void StartImageLoad(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    state->mediaKind = L"Image";
    state->mediaPreview = true;
    state->mediaPending = true;

    ImageLoadResult* result = new ImageLoadResult();
    result->panel = state->panel;
    result->generation = state->loadGeneration;
    result->token = state->loadToken;
    result->path = state->path;
    result->bytes = state->mediaBytes;

    if (!StartWorker(ImageLoadThreadProc, result)) {
        state->mediaPending = false;
        delete result;
    }
    SetTimer(state->panel, QL_TIMER_SPINNER, 70, nullptr);
    InvalidateRect(state->panel, nullptr, TRUE);
}

// Takes ownership of the bitmap when it is used; whatever is left over is freed
// by the caller.
static void ApplyImageLoadResult(PanelState* state, ImageLoadResult* result) {
    if (!state || !result || result->generation != state->loadGeneration) {
        return;
    }
    state->mediaPending = false;
    if (result->ok && result->bitmap) {
        if (state->mediaBitmap) {
            DeleteObject(state->mediaBitmap);
        }
        state->mediaBitmap = result->bitmap;
        result->bitmap = nullptr;
        state->mediaHasAlpha = result->fromWic;
        state->mediaWidth = result->width;
        state->mediaHeight = result->height;
        state->mediaSourceWidth = result->sourceWidth;
        state->mediaSourceHeight = result->sourceHeight;
        state->mediaBytes = result->bytes;
    }
    Wh_Log(L"image: %s bitmap=%p size=%dx%d source=%dx%d", result->path.c_str(),
          (void*)state->mediaBitmap, state->mediaWidth, state->mediaHeight,
          state->mediaSourceWidth, state->mediaSourceHeight);
    if (state->panel && IsWindow(state->panel)) {
        InvalidateRect(state->panel, nullptr, TRUE);
    }
}

// Paints the loading state before a synchronous thumbnail or handler is built,
// so the panel never looks frozen while the shell works.
static void ShowMediaLoading(PanelState* state) {
    if (!state) {
        return;
    }
    state->mediaPreview = true;
    state->mediaPending = true;
    if (state->panel && IsWindow(state->panel)) {
        SetTimer(state->panel, QL_TIMER_SPINNER, 70, nullptr);
        InvalidateRect(state->panel, nullptr, TRUE);
        UpdateWindow(state->panel);
    }
}

// A spinner with a label, for any load that is taking more than a moment. It
// draws nothing during the first quarter second, so a fast load never flashes
// a spinner, and it says so when a load turns out to be slow.
static void DrawLoadingIndicator(HDC dc, PanelState* state, UINT dpi,
                                 const RECT& body) {
    if (!state) {
        return;
    }
    ULONGLONG elapsed = GetTickCount64() - state->loadingSince;
    if (elapsed < 250) {
        return;
    }
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF content = ContentTextColor(background);

    int cx = (body.left + body.right) / 2;
    int cy = (body.top + body.bottom) / 2 - Scale(12, dpi);
    int radius = Scale(14, dpi);
    int dot = std::max(2, Scale(3, dpi));
    const int count = 10;
    const double kTwoPi = 6.283185307179586;
    int head = state->spinnerFrame % count;

    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    for (int i = 0; i < count; i++) {
        double angle = kTwoPi * i / count - kTwoPi / 4;
        int px = cx + static_cast<int>(std::lround(std::cos(angle) * radius));
        int py = cy + static_cast<int>(std::lround(std::sin(angle) * radius));
        int age = (head - i + count) % count;
        int strength = 85 - age * 70 / (count - 1);
        HBRUSH brush = CreateSolidBrush(BlendColor(background, content, strength));
        HGDIOBJ oldBrush = SelectObject(dc, brush);
        Ellipse(dc, px - dot, py - dot, px + dot + 1, py + dot + 1);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }
    SelectObject(dc, oldPen);

    const wchar_t* label =
        elapsed > 3000 ? L"Still loading\u2026 this one is taking a while"
                       : L"Loading\u2026";
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, BlendColor(background, content, 55));
    RECT labelRect = {body.left, cy + radius + dot + Scale(10, dpi), body.right,
                      cy + radius + dot + Scale(34, dpi)};
    DrawTextW(dc, label, -1, &labelRect,
              DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOPREFIX |
                  DT_END_ELLIPSIS);
    state->loadingRect = {body.left, cy - radius - dot - Scale(4, dpi),
                          body.right, labelRect.bottom + Scale(4, dpi)};
}

static void DrawMediaPreview(HDC dc, PanelState* state, UINT dpi,
                             const RECT& body) {
    if (state->mediaHandler) {
        // The PDF preview handler paints into its own child window.
        return;
    }
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF content = ContentTextColor(background);
    SetBkMode(dc, TRANSPARENT);

    if (state->mediaPending) {
        DrawLoadingIndicator(dc, state, dpi, body);
        return;
    }

    if (!state->mediaBitmap || state->mediaWidth <= 0 ||
        state->mediaHeight <= 0) {
        SetTextColor(dc, BlendColor(background, content, 45));
        RECT message = body;
        DrawTextW(dc, L"No preview for this file type yet.", -1, &message,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        return;
    }

    int availableWidth = static_cast<int>(body.right - body.left);
    int availableHeight = static_cast<int>(body.bottom - body.top);
    if (availableWidth <= 0 || availableHeight <= 0) {
        return;
    }

    double scale = std::min(
        static_cast<double>(availableWidth) / state->mediaWidth,
        static_cast<double>(availableHeight) / state->mediaHeight);
    int width = std::max(1, static_cast<int>(state->mediaWidth * scale));
    int height = std::max(1, static_cast<int>(state->mediaHeight * scale));
    int x = body.left + (availableWidth - width) / 2;
    int y = body.top + (availableHeight - height) / 2;

    HDC memory = CreateCompatibleDC(dc);
    if (!memory) {
        return;
    }
    HGDIOBJ previous = SelectObject(memory, state->mediaBitmap);

    BITMAP info = {};
    GetObjectW(state->mediaBitmap, sizeof(info), &info);
    if (state->mediaHasAlpha && info.bmBitsPixel == 32) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, width, height, memory, 0, 0, state->mediaWidth,
                   state->mediaHeight, blend);
    } else {
        // Shell thumbnails often carry a zero alpha channel, so they are drawn
        // without it; their pixels are opaque anyway.
        SetStretchBltMode(dc, HALFTONE);
        SetBrushOrgEx(dc, 0, 0, nullptr);
        StretchBlt(dc, x, y, width, height, memory, 0, 0, state->mediaWidth,
                   state->mediaHeight, SRCCOPY);
    }

    SelectObject(memory, previous);
    DeleteDC(memory);
}

// Windows exposes a zip (and any archive with a namespace extension) as a
// shell folder, which is exactly what the folder preview already knows how to
// render. This asks the shell before committing to that path.
static bool ShellItemIsFolder(const std::wstring& path) {
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_IShellItem, (void**)&item)) ||
        !item) {
        return false;
    }
    SFGAOF flags = SFGAO_FOLDER;
    bool folder = SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &flags)) &&
                  (flags & SFGAO_FOLDER) != 0;
    item->Release();
    return folder;
}

// `attributesOnly` asks the shell for the icon by file type alone, so it does not
// have to open the file (used for cloud placeholders).
static HICON LoadPropertyIcon(const std::wstring& path,
                              bool attributesOnly = false) {
    SHFILEINFOW info = {};
    UINT flags = SHGFI_SYSICONINDEX | (attributesOnly ? SHGFI_USEFILEATTRIBUTES : 0);
    if (!SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &info,
                        sizeof(info), flags)) {
        return nullptr;
    }

    HICON icon = nullptr;
    IImageList* imageList = nullptr;
    // IID_IImageList is not exported by every toolchain, so it is spelled out.
    static const GUID kImageListIid = {
        0x46EB5926, 0x582E, 0x4017,
        {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50}};
    if (SUCCEEDED(SHGetImageList(SHIL_JUMBO, kImageListIid,
                                 (void**)&imageList)) &&
        imageList) {
        if (FAILED(imageList->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon))) {
            icon = nullptr;
        }
        imageList->Release();
    }
    if (!icon) {
        SHFILEINFOW fallback = {};
        UINT fallbackFlags = SHGFI_ICON | SHGFI_LARGEICON |
                             (attributesOnly ? SHGFI_USEFILEATTRIBUTES : 0);
        if (SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &fallback,
                           sizeof(fallback), fallbackFlags)) {
            icon = fallback.hIcon;
        }
    }
    return icon;
}

static std::wstring FormatPropertyDate(const FILETIME& fileTime) {
    SYSTEMTIME time = {};
    FILETIME local = {};
    if (!FileTimeToLocalFileTime(&fileTime, &local) ||
        !FileTimeToSystemTime(&local, &time)) {
        return L"";
    }
    return Fmt(L"%04u-%02u-%02u %02u:%02u", (unsigned)time.wYear,
               (unsigned)time.wMonth, (unsigned)time.wDay,
               (unsigned)time.wHour, (unsigned)time.wMinute);
}

// The view used when no renderer exists: the real file icon plus the properties
// Explorer would show for it. Everything it draws was gathered by the probe, so
// painting never calls into the shell or the disk.
static void DrawPropertiesPreview(HDC dc, PanelState* state, UINT dpi,
                                  const RECT& body) {
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF content = ContentTextColor(background);
    COLORREF dim = BlendColor(background, content, 55);
    SetBkMode(dc, TRANSPARENT);

    if (state->path.empty()) {
        SetTextColor(dc, dim);
        RECT message = body;
        DrawTextW(dc, L"Select a file or folder in Explorer and press Space.",
                  -1, &message,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        return;
    }

    int iconSize = Scale(96, dpi);
    int left = body.left;
    int top = body.top + Scale(6, dpi);
    if (state->propertyIcon) {
        DrawIconEx(dc, left, top, state->propertyIcon, iconSize, iconSize, 0,
                   nullptr, DI_NORMAL);
    }

    int textLeft = left + iconSize + Scale(18, dpi);
    RECT textRect = {textLeft, top, body.right, body.bottom};

    HFONT font = CreateFontW(
        -Scale(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HGDIOBJ previous = SelectObject(dc, font);

    int rowHeight = Scale(22, dpi);
    int y = textRect.top;
    auto row = [&](const std::wstring& value) {
        if (y + rowHeight > textRect.bottom) {
            return;
        }
        SetTextColor(dc, content);
        RECT line = {textRect.left, y, textRect.right, y + rowHeight};
        DrawTextW(dc, value.c_str(), -1, &line,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
        y += rowHeight;
    };

    if (!state->propType.empty()) {
        row(state->propType);
    }
    if (!state->propSize.empty()) {
        row(state->propSize);
    }
    if (!state->propModified.empty()) {
        row(state->propModified);
    }

    SetTextColor(dc, dim);
    if (!state->propNote.empty()) {
        RECT noteRect = {textRect.left, y + Scale(2, dpi), textRect.right,
                         textRect.bottom};
        RECT measure = noteRect;
        DrawTextW(dc, state->propNote.c_str(), -1, &measure,
                  DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
        DrawTextW(dc, state->propNote.c_str(), -1, &noteRect,
                  DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);
        y += (measure.bottom - measure.top) + Scale(8, dpi);
    }

    RECT pathRect = {textRect.left, y + Scale(2, dpi), textRect.right,
                     textRect.bottom};
    DrawTextW(dc, state->path.c_str(), -1, &pathRect,
              DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);

    SelectObject(dc, previous);
    DeleteObject(font);
}

// ---------------------------------------------------------------------------
// Loading the selected item
// ---------------------------------------------------------------------------
//
// Selecting an item never touches the disk on the UI thread. LoadSelectedItem
// only starts a probe on a worker; the probe stats the item, lists a folder,
// classifies a file, builds a video/audio thumbnail and reads the icon and type
// the properties view needs. The panel keeps showing the previous preview until
// the result arrives, and shows a spinner when that takes a while.

#ifndef FILE_ATTRIBUTE_RECALL_ON_OPEN
#define FILE_ATTRIBUTE_RECALL_ON_OPEN 0x00040000
#endif
#ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#endif

enum class ProbeKind {
    Missing,
    Folder,
    Archive,
    Image,
    Video,
    Audio,
    Pdf,
    Office,
    Other,
};

struct ProbeResult {
    HWND panel = nullptr;
    unsigned generation = 0;
    std::shared_ptr<std::atomic<unsigned>> token;
    std::wstring path;
    ProbeKind kind = ProbeKind::Missing;
    ULONGLONG size = 0;
    FILETIME modified = {};
    // A cloud placeholder: reading it would download it, so it is not opened.
    bool cloudOnly = false;
    std::vector<FolderEntry> entries;
    HBITMAP thumbnail = nullptr;
    int thumbnailWidth = 0;
    int thumbnailHeight = 0;
    std::wstring typeName;
    HICON icon = nullptr;
};

static void FreeProbeResult(ProbeResult* result) {
    if (!result) {
        return;
    }
    if (result->thumbnail) {
        DeleteObject(result->thumbnail);
        result->thumbnail = nullptr;
    }
    if (result->icon) {
        DestroyIcon(result->icon);
        result->icon = nullptr;
    }
}

static void RunProbe(ProbeResult* result) {
    auto stale = [&]() {
        return g_stopWorkers.load() ||
               (result->token && result->token->load() != result->generation);
    };

    WIN32_FILE_ATTRIBUTE_DATA data = {};
    if (!GetFileAttributesExW(result->path.c_str(), GetFileExInfoStandard,
                              &data)) {
        result->kind = ProbeKind::Missing;
        return;
    }
    result->size = (static_cast<ULONGLONG>(data.nFileSizeHigh) << 32) |
                   data.nFileSizeLow;
    result->modified = data.ftLastWriteTime;

    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        result->kind = ProbeKind::Folder;
        EnumerateFolderEntries(result->path, result->entries, result->token,
                               result->generation);
        return;
    }

    const DWORD kCloudAttributes = FILE_ATTRIBUTE_OFFLINE |
                                   FILE_ATTRIBUTE_RECALL_ON_OPEN |
                                   FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS;
    result->cloudOnly = (data.dwFileAttributes & kCloudAttributes) != 0;

    std::wstring extension = LowerExtension(result->path);
    if (!result->cloudOnly && IsArchiveExtension(extension) &&
        ShellItemIsFolder(result->path)) {
        result->kind = ProbeKind::Archive;
    } else if (IsImageExtension(extension)) {
        result->kind = ProbeKind::Image;
    } else if (IsVideoExtension(extension)) {
        result->kind = ProbeKind::Video;
    } else if (IsAudioExtension(extension)) {
        result->kind = ProbeKind::Audio;
    } else if (IsPdfExtension(extension)) {
        result->kind = ProbeKind::Pdf;
    } else if (IsOfficeExtension(extension)) {
        result->kind = ProbeKind::Office;
    } else {
        result->kind = ProbeKind::Other;
    }
    if (stale()) {
        return;
    }

    if (!result->cloudOnly && (result->kind == ProbeKind::Video ||
                               result->kind == ProbeKind::Audio)) {
        result->thumbnail = LoadShellThumbnail(
            result->path, result->kind == ProbeKind::Video ? 1280 : 512);
        if (!result->thumbnail && result->kind == ProbeKind::Video) {
            // No thumbnail provider for this extension or codec: decode the
            // first frame ourselves.
            int frameWidth = 0;
            int frameHeight = 0;
            result->thumbnail = LoadVideoFrameMediaFoundation(
                result->path, frameWidth, frameHeight);
            if (result->thumbnail) {
                result->thumbnailWidth = frameWidth;
                result->thumbnailHeight = frameHeight;
                Wh_Log(L"video frame: media foundation %s %dx%d",
                      result->path.c_str(), frameWidth, frameHeight);
            } else {
                Wh_Log(L"video frame: no thumbnail and no decoder for %s",
                      result->path.c_str());
            }
        }
        if (result->thumbnail) {
            BITMAP info = {};
            if (GetObjectW(result->thumbnail, sizeof(info), &info)) {
                result->thumbnailWidth = info.bmWidth;
                result->thumbnailHeight = info.bmHeight;
            }
        }
    }
    if (stale()) {
        return;
    }

    SHFILEINFOW fileInfo = {};
    UINT typeFlags =
        SHGFI_TYPENAME | (result->cloudOnly ? SHGFI_USEFILEATTRIBUTES : 0);
    if (SHGetFileInfoW(result->path.c_str(), FILE_ATTRIBUTE_NORMAL, &fileInfo,
                       sizeof(fileInfo), typeFlags)) {
        result->typeName = fileInfo.szTypeName;
    }
    result->icon = LoadPropertyIcon(result->path, result->cloudOnly);
}

static DWORD WINAPI ProbeThreadProc(void* parameter) {
    ProbeResult* result = static_cast<ProbeResult*>(parameter);
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    RunProbe(result);
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    bool stale = result->token && result->token->load() != result->generation;
    if (stale || !result->panel ||
        !PostMessageW(result->panel, QL_WM_PROBE_DONE, 0,
                      reinterpret_cast<LPARAM>(result))) {
        FreeProbeResult(result);
        delete result;
    }
    return 0;
}

static void StartProbe(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    if (g_activeWorkers.load() >= kMaxActiveWorkers) {
        // Many loads are still running (a slow share, say): try again shortly
        // instead of piling up more threads.
        SetTimer(state->panel, QL_TIMER_PROBE, 150, nullptr);
        return;
    }
    state->probeStarted = true;

    ProbeResult* result = new ProbeResult();
    result->panel = state->panel;
    result->generation = state->loadGeneration;
    result->token = state->loadToken;
    result->path = state->path;
    if (!StartWorker(ProbeThreadProc, result)) {
        delete result;
        state->probeStarted = false;
        state->loadPending = false;
    }
}

// Clears whatever the previous selection was showing.
static void ResetPreviewState(PanelState* state) {
    ClearEntries(state);
    ClearTextPreview(state);
    ClearMediaPreview(state);
    ClearPropertyIcon(state);
    state->propType.clear();
    state->propSize.clear();
    state->propModified.clear();
    state->propNote.clear();
    state->resultsPending.clear();
    state->resultsPendingPos = 0;
    state->archivePreview = false;
    state->textLoading = false;
    state->folderPreview = false;
    state->folderPath.clear();
    state->filter.clear();
    if (state->filterEdit) {
        SetWindowTextW(state->filterEdit, L"");
    }
}

static const ULONGLONG kMaxPdfPreviewBytes = 200ull * 1024 * 1024;

// Last resort for a document: the Windows preview handler registered for its
// extension (it only works for some formats), then the properties view.
static void FallbackDocumentPreview(PanelState* state, ULONGLONG size) {
    if (!state) {
        return;
    }
    std::wstring extension = LowerExtension(state->path);
    state->mediaKind = OfficeKindLabel(extension);
    state->mediaBytes = size;
    ShowMediaLoading(state);
    state->mediaPreview = true;
    state->mediaPending = false;
    std::wstring diag;
    if (!LoadPdfPreview(state, L"." + extension, &diag)) {
        ClearMediaPreview(state);
        state->propNote =
            L"No preview is available for this document: " + diag + L".";
    }
}

// Runs on the UI thread when the probe is done: swaps the previous preview for
// the new one. Takes ownership of the icon and thumbnail it uses.
static void ApplyProbeResult(PanelState* state, ProbeResult* result) {
    if (!state || !result || result->generation != state->loadGeneration) {
        return;
    }
    state->loadPending = false;
    state->probeStarted = false;
    ResetPreviewState(state);

    if (result->kind != ProbeKind::Missing &&
        result->kind != ProbeKind::Folder) {
        state->propType = result->typeName;
        state->propSize = FormatEntrySize(result->size);
        state->propModified = FormatPropertyDate(result->modified);
    }
    state->propertyIcon = result->icon;
    result->icon = nullptr;

    const wchar_t* cloudNote =
        L"This file is only in the cloud. It is not previewed so that it is "
        L"not downloaded just to look at it.";

    switch (result->kind) {
        case ProbeKind::Missing:
            state->propNote = L"This item is no longer available.";
            break;
        case ProbeKind::Folder:
            state->folderPreview = true;
            state->folderPath = state->path;
            state->entries = std::move(result->entries);
            for (const FolderEntry& entry : state->entries) {
                if (entry.isFolder) {
                    state->totalFolderCount++;
                } else {
                    state->totalFileCount++;
                }
            }
            break;
        case ProbeKind::Archive:
            // The shell opens the archive as a folder: reuse the folder view.
            state->archivePreview = true;
            state->folderPreview = true;
            state->folderPath = state->path;
            break;
        case ProbeKind::Image:
            if (result->cloudOnly) {
                state->propNote = cloudNote;
            } else {
                state->mediaBytes = result->size;
                StartImageLoad(state);
            }
            break;
        case ProbeKind::Video:
        case ProbeKind::Audio:
            if (result->cloudOnly) {
                state->propNote = cloudNote;
            } else {
                state->mediaKind =
                    result->kind == ProbeKind::Video ? L"Video" : L"Audio";
                state->mediaBytes = result->size;
                state->mediaPreview = true;
                state->mediaPending = false;
                if (result->thumbnail) {
                    state->mediaBitmap = result->thumbnail;
                    result->thumbnail = nullptr;
                    state->mediaWidth = result->thumbnailWidth;
                    state->mediaHeight = result->thumbnailHeight;
                }
            }
            break;
        case ProbeKind::Pdf:
            if (result->cloudOnly) {
                state->propNote = cloudNote;
            } else if (result->size > kMaxPdfPreviewBytes) {
                state->propNote =
                    L"This PDF is too large to preview here.";
            } else {
                // The preview handler has to be created on this thread. The
                // spinner is painted first so the panel never looks stuck.
                state->mediaKind = L"PDF";
                state->mediaBytes = result->size;
                ShowMediaLoading(state);
                state->mediaPreview = true;
                state->mediaPending = false;
                LoadPdfPreview(state);
            }
            break;
        case ProbeKind::Office:
            if (result->cloudOnly) {
                state->propNote = cloudNote;
            } else if (result->size > kMaxPdfPreviewBytes) {
                state->propNote =
                    L"This document is too large to preview here.";
            } else {
                std::wstring extension = LowerExtension(state->path);
                if (IsNativeDocExtension(extension)) {
                    // Modern documents: the text is read straight from the
                    // file on a worker thread and shown as a text preview.
                    StartTextLoad(state, true);
                } else {
                    // Legacy binary formats: only a registered preview
                    // handler can show them.
                    FallbackDocumentPreview(state, result->size);
                }
            }
            break;
        case ProbeKind::Other:
            if (result->cloudOnly) {
                state->propNote = cloudNote;
            } else {
                StartTextLoad(state);
            }
            break;
    }

    ApplyFilter(state);
    SyncExplorerBrowser(state);
    if (state->panel && IsWindow(state->panel)) {
        // Lays out the text toolbar and wraps the preview for the new content,
        // so a click on a toggle works before the next resize.
        LayoutPanelContent(state);
        InvalidateRect(state->panel, nullptr, TRUE);
    }
}

// `immediate` is true when the panel opens; a selection change waits a moment
// so that holding an arrow key does not start a load for every row it passes.
static void LoadSelectedItem(PanelState* state, bool immediate = true) {
    if (!state) {
        return;
    }
    // Any load still in flight is now stale.
    state->loadGeneration++;
    if (!state->loadToken) {
        state->loadToken = std::make_shared<std::atomic<unsigned>>(0);
    }
    state->loadToken->store(state->loadGeneration);
    state->textLoading = false;
    state->mediaPending = false;
    state->probeStarted = false;
    state->loadSlowCleared = false;
    state->loadingSince = GetTickCount64();
    const bool hasPanel = state->panel && IsWindow(state->panel);
    if (hasPanel) {
        KillTimer(state->panel, QL_TIMER_PROBE);
    }

    if (state->path.empty()) {
        state->loadPending = false;
        ResetPreviewState(state);
        ApplyFilter(state);
        SyncExplorerBrowser(state);
        if (hasPanel) {
            InvalidateRect(state->panel, nullptr, TRUE);
        }
        return;
    }

    state->loadPending = true;
    if (!hasPanel) {
        return;
    }
    SetTimer(state->panel, QL_TIMER_SPINNER, 70, nullptr);
    if (immediate) {
        StartProbe(state);
    } else {
        SetTimer(state->panel, QL_TIMER_PROBE, 140, nullptr);
    }
    InvalidateRect(state->panel, nullptr, TRUE);
}

// Drives the loading indicator. If the probe is slow (an unreachable share, say)
// the previous preview is dropped so the spinner is what the user sees.
static void TickSpinner(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    bool busy = state->open && (state->loadPending || state->textLoading ||
                                state->mediaPending);
    if (!busy) {
        KillTimer(state->panel, QL_TIMER_SPINNER);
        state->spinnerFrame = 0;
        state->loadingRect = {};
        return;
    }
    state->spinnerFrame++;
    ULONGLONG elapsed = GetTickCount64() - state->loadingSince;
    if (state->loadPending && !state->loadSlowCleared && elapsed > 600) {
        state->loadSlowCleared = true;
        ResetPreviewState(state);
        ApplyFilter(state);
        SyncExplorerBrowser(state);
        LayoutPanelContent(state);
        InvalidateRect(state->panel, nullptr, TRUE);
        return;
    }
    if (elapsed >= 250) {
        if (IsRectEmpty(&state->loadingRect)) {
            InvalidateRect(state->panel, nullptr, FALSE);
        } else {
            InvalidateRect(state->panel, &state->loadingRect, FALSE);
        }
    }
}

// ---------------------------------------------------------------------------
// Shell view and selection
// ---------------------------------------------------------------------------

static IShellView* GetActiveShellView(HWND top, HWND* outTab,
                                      IWebBrowser2** outBrowser = nullptr) {
    if (outTab) {
        *outTab = nullptr;
    }
    if (outBrowser) {
        *outBrowser = nullptr;
    }
    if (!top) {
        return nullptr;
    }

    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_IShellWindows, (void**)&shellWindows);
    if (FAILED(hr) || !shellWindows) {
        Wh_Log(L"CoCreateInstance(ShellWindows) failed: 0x%08X", (unsigned)hr);
        return nullptr;
    }

    HWND activeTab = FindWindowExW(top, nullptr, L"ShellTabWindowClass", nullptr);
    long count = 0;
    shellWindows->get_Count(&count);
    Wh_Log(L"view: top=%p activeTab=%p count=%ld", (void*)top,
          (void*)activeTab, count);

    IShellView* result = nullptr;

    for (long i = 0; i < count && !result; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* dispatch = nullptr;
        if (FAILED(shellWindows->Item(index, &dispatch)) || !dispatch) {
            VariantClear(&index);
            continue;
        }
        VariantClear(&index);

        IWebBrowser2* browser = nullptr;
        if (FAILED(dispatch->QueryInterface(IID_IWebBrowser2,
                                            (void**)&browser)) ||
            !browser) {
            ReleaseCom(dispatch);
            continue;
        }

        SHANDLE_PTR browserHandle = 0;
        browser->get_HWND(&browserHandle);
        if ((HWND)browserHandle != top) {
            ReleaseCom(browser);
            ReleaseCom(dispatch);
            continue;
        }

        IServiceProvider* serviceProvider = nullptr;
        IShellBrowser* shellBrowser = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_IServiceProvider,
                                               (void**)&serviceProvider)) &&
            serviceProvider &&
            SUCCEEDED(serviceProvider->QueryService(
                SID_STopLevelBrowser, IID_IShellBrowser,
                (void**)&shellBrowser)) &&
            shellBrowser) {
            HWND tabWindow = nullptr;
            shellBrowser->GetWindow(&tabWindow);

            bool match = activeTab ? tabWindow == activeTab
                                   : (tabWindow && IsWindowVisible(tabWindow));
            if (match) {
                IShellView* view = nullptr;
                if (SUCCEEDED(shellBrowser->QueryActiveShellView(&view)) &&
                    view) {
                    result = view;
                    if (outTab) {
                        *outTab = tabWindow ? tabWindow
                                            : (activeTab ? activeTab : top);
                    }
                    if (outBrowser) {
                        browser->AddRef();
                        *outBrowser = browser;
                    }
                }
            }
        }

        ReleaseCom(shellBrowser);
        ReleaseCom(serviceProvider);
        ReleaseCom(browser);
        ReleaseCom(dispatch);
    }

    ReleaseCom(shellWindows);
    Wh_Log(L"view result: top=%p result=%p tab=%p", (void*)top,
          (void*)result, (void*)(outTab ? *outTab : nullptr));
    return result;
}

static std::wstring ItemPath(IShellItem* item) {
    if (!item) {
        return L"";
    }
    PWSTR value = nullptr;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &value)) && value) {
        std::wstring path = value;
        CoTaskMemFree(value);
        return path;
    }
    if (SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &value)) &&
        value) {
        std::wstring path = value;
        CoTaskMemFree(value);
        return path;
    }
    return L"";
}

static std::wstring GetSelectedPath(IShellView* shellView,
                                    std::wstring* debug) {
    std::wstring path;
    if (debug) {
        debug->clear();
    }
    if (!shellView) {
        return path;
    }

    IFolderView2* folderView = nullptr;
    if (FAILED(shellView->QueryInterface(IID_IFolderView2,
                                         (void**)&folderView)) ||
        !folderView) {
        if (debug) {
            *debug = L"no IFolderView2";
        }
        return path;
    }

    DWORD itemCount = 0;
    int focusedIndex = -1;
    IShellItemArray* selection = nullptr;
    if (SUCCEEDED(folderView->GetSelection(FALSE, &selection)) && selection) {
        selection->GetCount(&itemCount);
        if (itemCount > 0) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(selection->GetItemAt(0, &item)) && item) {
                path = ItemPath(item);
                ReleaseCom(item);
            }
        }
        ReleaseCom(selection);
    }

    if (path.empty() && SUCCEEDED(folderView->GetFocusedItem(&focusedIndex)) &&
        focusedIndex >= 0) {
        IFolderView* baseView = nullptr;
        if (SUCCEEDED(folderView->QueryInterface(IID_IFolderView,
                                                 (void**)&baseView)) &&
            baseView) {
            PITEMID_CHILD child = nullptr;
            if (SUCCEEDED(baseView->Item(focusedIndex, &child)) && child) {
                IShellFolder* folder = nullptr;
                if (SUCCEEDED(baseView->GetFolder(IID_IShellFolder,
                                                  (void**)&folder)) &&
                    folder) {
                    IShellItem* item = nullptr;
                    if (SUCCEEDED(SHCreateItemWithParent(
                            nullptr, folder, child, IID_IShellItem,
                            (void**)&item)) &&
                        item) {
                        path = ItemPath(item);
                        ReleaseCom(item);
                    }
                    ReleaseCom(folder);
                }
                CoTaskMemFree(child);
            }
            ReleaseCom(baseView);
        }
    }

    if (debug) {
        *debug = Fmt(L"items=%lu focused=%d", itemCount, focusedIndex);
    }

    ReleaseCom(folderView);
    return path;
}

// ---------------------------------------------------------------------------
// Native Explorer browser
// ---------------------------------------------------------------------------

static void OnBrowserViewCreated(PanelState* state, IShellView* view);

struct BrowserListContext {
    HWND best;
    LONGLONG area;
};

static BOOL CALLBACK FindBrowserListProc(HWND child, LPARAM parameter) {
    BrowserListContext* context =
        reinterpret_cast<BrowserListContext*>(parameter);

    if (!IsWindowVisible(child)) {
        return TRUE;
    }
    std::wstring className = ClassNameOf(child);
    if (_wcsicmp(className.c_str(), L"DirectUIHWND") != 0 &&
        _wcsicmp(className.c_str(), L"SysListView32") != 0 &&
        _wcsicmp(className.c_str(), L"SHELLDLL_DefView") != 0) {
        return TRUE;
    }
    RECT rect = {};
    if (!GetWindowRect(child, &rect)) {
        return TRUE;
    }
    LONGLONG area =
        (LONGLONG)(rect.right - rect.left) * (rect.bottom - rect.top);
    if (area > context->area) {
        context->area = area;
        context->best = child;
    }
    return TRUE;
}

static void OpenPathExternally(PanelState* state, const std::wstring& path) {
    if (!state || path.empty()) {
        return;
    }

    DWORD now = GetTickCount();
    if (!state->lastOpenPath.empty() &&
        SameFolderPath(state->lastOpenPath, path) &&
        now - state->lastOpenTick < 700) {
        return;
    }
    state->lastOpenPath = path;
    state->lastOpenTick = now;

    // The panel only shows the direct children of the previewed folder, so the
    // entry is already known. An unknown path (an archive item, say) is treated
    // as a file: a file system call here could block the UI thread on an
    // unreachable share.
    bool isFolder = false;
    for (const FolderEntry& entry : state->entries) {
        if (SameFolderPath(entry.path, path)) {
            isFolder = entry.isFolder;
            break;
        }
    }
    if (isFolder) {
        Wh_Log(L"open folder properties: %s", path.c_str());
        SHObjectProperties(state->panel, SHOP_FILEPATH, path.c_str(), nullptr);
    } else {
        Wh_Log(L"open file: %s", path.c_str());
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
    }
}

// The panel never navigates, so the intent behind any open request is carried
// to the panel message loop and resolved there. Opening a shell dialog from
// inside a browser event callback would re-enter the view that is still
// finishing the click.
static void RequestOpenPath(PanelState* state, const std::wstring& path) {
    if (!state || path.empty() || !state->open || !state->panel ||
        !IsWindow(state->panel)) {
        return;
    }
    // Items inside an archive cannot be opened from here, and a double click on
    // a folder inside it must not enter it either.
    if (state->archivePreview) {
        return;
    }
    state->pendingOpenPath = path;
    PostMessageW(state->panel, QL_WM_OPEN_PATH, 0, 0);
}

static LRESULT CALLBACK BrowserListSubclassProc(HWND hwnd, UINT message,
                                                WPARAM wParam, LPARAM lParam,
                                                UINT_PTR subclassId,
                                                DWORD_PTR referenceData) {
    PanelState* state = reinterpret_cast<PanelState*>(referenceData);

    if (message == WM_LBUTTONDBLCLK) {
        // A double click only asks to open the selected item. The request is
        // posted because the shell view is still inside its own click handling,
        // and the message is swallowed so the view never starts a navigation.
        if (state && state->browserView) {
            RequestOpenPath(state,
                            GetSelectedPath(state->browserView, nullptr));
        }
        return 0;
    }

    if (message == WM_SIZE) {
        // Explorer applies its own layout after a resize and restores the
        // column widths it remembers, so the fit has to run again afterwards.
        if (state && state->panel && IsWindow(state->panel)) {
            SetTimer(state->panel, QL_TIMER_BROWSER_COLUMNS, 150, nullptr);
        }
    }

    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, BrowserListSubclassProc, subclassId);
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

static void EnsureBrowserListSubclass(PanelState* state) {
    if (!state || !state->browserViewWnd ||
        !IsWindow(state->browserViewWnd)) {
        return;
    }

    HWND best = nullptr;
    std::wstring viewClass = ClassNameOf(state->browserViewWnd);
    if (_wcsicmp(viewClass.c_str(), L"DirectUIHWND") == 0 ||
        _wcsicmp(viewClass.c_str(), L"SysListView32") == 0 ||
        _wcsicmp(viewClass.c_str(), L"SHELLDLL_DefView") == 0) {
        best = state->browserViewWnd;
    }

    BrowserListContext context = {nullptr, 0};
    EnumChildWindows(state->browserViewWnd, FindBrowserListProc,
                     reinterpret_cast<LPARAM>(&context));
    if (context.best) {
        best = context.best;
    }

    // The shell view itself receives the double click when the pointer lands
    // outside the list rows, so it watches for it too.
    if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
        SetWindowSubclass(state->browserViewWnd, BrowserListSubclassProc, 5,
                          reinterpret_cast<DWORD_PTR>(state));
    }

    if (!best || best == state->browserListWnd) {
        return;
    }

    if (state->browserListWnd && IsWindow(state->browserListWnd)) {
        RemoveWindowSubclass(state->browserListWnd, BrowserListSubclassProc,
                             4);
    }
    state->browserListWnd = best;
    SetWindowSubclass(best, BrowserListSubclassProc, 4,
                      reinterpret_cast<DWORD_PTR>(state));
    Wh_Log(L"browser list subclass: hwnd=%p class=%s", (void*)best,
          ClassNameOf(best).c_str());
}

static void ApplyBrowserViewMode(PanelState* state) {
    if (!state || !state->browserView) {
        return;
    }
    IFolderView2* folderView = nullptr;
    if (FAILED(state->browserView->QueryInterface(IID_IFolderView2,
                                                  (void**)&folderView)) ||
        !folderView) {
        return;
    }

    FOLDERVIEWMODE mode = FVM_DETAILS;
    int iconSize = 16;
    if (state->viewMode == QlView::List) {
        mode = FVM_LIST;
    } else if (state->viewMode == QlView::Icons) {
        mode = FVM_ICON;
        iconSize = state->iconSize;
    }

    folderView->SetViewModeAndIconSize(mode, iconSize);
    ReleaseCom(folderView);
    Wh_Log(L"browser view mode: %d icon=%d", (int)mode, iconSize);
}

static void ReleaseBrowserColumns(PanelState* state) {
    if (!state) {
        return;
    }
    ReleaseCom(state->browserColumnManager);
    state->browserColumnKeys.clear();
    state->browserColumnWidths.clear();
    state->browserColumnsSaved = false;
}

static bool SaveBrowserColumns(PanelState* state) {
    if (!state || !state->browserView) {
        return false;
    }

    IFolderView2* folderView = nullptr;
    if (FAILED(state->browserView->QueryInterface(IID_IFolderView2,
                                                  (void**)&folderView)) ||
        !folderView) {
        return false;
    }
    FOLDERVIEWMODE mode = FVM_AUTO;
    int iconSize = 0;
    bool details = SUCCEEDED(folderView->GetViewModeAndIconSize(
                        &mode, &iconSize)) &&
                   mode == FVM_DETAILS;
    ReleaseCom(folderView);
    if (!details) {
        return false;
    }

    IColumnManager* columnManager = nullptr;
    if (FAILED(state->browserView->QueryInterface(
            IID_IColumnManager, (void**)&columnManager)) ||
        !columnManager) {
        return false;
    }

    UINT count = 0;
    if (FAILED(columnManager->GetColumnCount(CM_ENUM_VISIBLE, &count)) ||
        count == 0) {
        ReleaseCom(columnManager);
        return false;
    }

    std::vector<PROPERTYKEY> keys(count);
    if (FAILED(columnManager->GetColumns(CM_ENUM_VISIBLE, keys.data(),
                                        count))) {
        ReleaseCom(columnManager);
        return false;
    }

    UINT dpi = GetDpiForWindowSafe(state->browserViewWnd);
    std::vector<UINT> widths(count, 0);
    UINT valid = 0;
    for (UINT i = 0; i < count; i++) {
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        if (SUCCEEDED(columnManager->GetColumnInfo(keys[i], &info))) {
            UINT width = info.uWidth;
            if (width == static_cast<UINT>(CM_WIDTH_AUTOSIZE) || width == 0) {
                if (info.uIdealWidth > 0) {
                    width = info.uIdealWidth;
                } else if (info.uDefaultWidth > 0) {
                    width = info.uDefaultWidth;
                } else {
                    width = static_cast<UINT>(Scale(120, dpi));
                }
            }
            widths[i] = width;
            valid++;
        }
    }

    if (!valid) {
        ReleaseCom(columnManager);
        return false;
    }

    ReleaseBrowserColumns(state);
    state->browserColumnManager = columnManager;
    state->browserColumnKeys = std::move(keys);
    state->browserColumnWidths = std::move(widths);
    state->browserColumnsSaved = true;

    Wh_Log(L"browser columns saved: count=%u valid=%u", count, valid);
    return true;
}

static const GUID kStorageFmtid = {0xB725F130,
                                   0x47EF,
                                   0x101A,
                                   {0xA5, 0xF3, 0x08, 0x00, 0x2B, 0x30, 0x30,
                                    0x9D}};
static const DWORD kPidName = 10;
static const DWORD kPidType = 4;
static const DWORD kPidSize = 12;
static const DWORD kPidDateModified = 14;

static bool IsNameColumn(const PROPERTYKEY& key) {
    return key.pid == kPidName && IsEqualGUID(key.fmtid, kStorageFmtid);
}

// Comfortable width of each secondary column at 96 DPI. Name is not listed:
// it takes whatever is left.
static int PreferredColumnWidth(const PROPERTYKEY& key, UINT dpi) {
    int dip = 90;
    if (IsEqualGUID(key.fmtid, kStorageFmtid)) {
        if (key.pid == kPidDateModified) {
            dip = 130;
        } else if (key.pid == kPidType) {
            dip = 100;
        } else if (key.pid == kPidSize) {
            dip = 72;
        }
    }
    return Scale(dip, dpi);
}

// Fits every visible Details column inside the browser so no horizontal
// scrollbar is needed. Secondary columns keep a compact preferred width and
// shrink only when space runs out; Name always gets the largest share.
static void ScaleBrowserColumns(PanelState* state) {
    if (!state || !state->browserColumnManager ||
        !state->browserColumnsSaved || state->browserColumnKeys.empty()) {
        return;
    }

    IFolderView2* folderView = nullptr;
    if (state->browserView &&
        SUCCEEDED(state->browserView->QueryInterface(
            IID_IFolderView2, (void**)&folderView)) &&
        folderView) {
        FOLDERVIEWMODE mode = FVM_AUTO;
        int iconSize = 0;
        bool details = SUCCEEDED(folderView->GetViewModeAndIconSize(
                            &mode, &iconSize)) &&
                       mode == FVM_DETAILS;
        ReleaseCom(folderView);
        if (!details) {
            return;
        }
    }

    UINT dpi = GetDpiForWindowSafe(state->browserViewWnd ? state->browserViewWnd
                                                         : state->panel);
    HWND targetWindow = state->browserListWnd && IsWindow(state->browserListWnd)
                            ? state->browserListWnd
                            : state->browserViewWnd;
    RECT client = {};
    int clientWidth = state->listRect.right - state->listRect.left;
    if (targetWindow && GetClientRect(targetWindow, &client) &&
        client.right > client.left) {
        clientWidth = client.right - client.left;
    }

    // Leave room for the vertical scrollbar plus a little slack, so the sum of
    // the columns can never exceed the visible width.
    // Explorer's Details view keeps a gutter to the left of the first column
    // and may show a vertical scrollbar, so the usable width is smaller than
    // the client width. FitBrowserOverflow() learns any remaining difference.
    int available = clientWidth - Scale(40, dpi) - state->browserExtraReserve;
    if (available < Scale(120, dpi)) {
        return;
    }

    const size_t count = state->browserColumnKeys.size();
    size_t nameIndex = 0;
    for (size_t i = 0; i < count; i++) {
        if (IsNameColumn(state->browserColumnKeys[i])) {
            nameIndex = i;
            break;
        }
    }

    const int minOther = Scale(40, dpi);
    std::vector<int> widths(count, 0);
    long long othersTotal = 0;
    for (size_t i = 0; i < count; i++) {
        if (i == nameIndex) {
            continue;
        }
        int preferred = PreferredColumnWidth(state->browserColumnKeys[i], dpi);
        int saved = static_cast<int>(state->browserColumnWidths[i]);
        int width = saved > 0 ? std::min(saved, preferred) : preferred;
        width = std::max(width, minOther);
        widths[i] = width;
        othersTotal += width;
    }

    // Name never drops below ~45% of the width (and never below 140 dip),
    // as long as the other columns can still keep their minimum.
    int nameMinimum = std::max(Scale(140, dpi), available * 45 / 100);
    nameMinimum = std::min(
        nameMinimum, available - static_cast<int>(count - 1) * minOther);
    nameMinimum = std::max(nameMinimum, Scale(60, dpi));

    int nameWidth = available - static_cast<int>(othersTotal);
    if (nameWidth < nameMinimum && othersTotal > 0) {
        double shrink = static_cast<double>(available - nameMinimum) /
                        static_cast<double>(othersTotal);
        othersTotal = 0;
        for (size_t i = 0; i < count; i++) {
            if (i == nameIndex) {
                continue;
            }
            widths[i] = std::max(minOther,
                                 static_cast<int>(widths[i] * shrink));
            othersTotal += widths[i];
        }
        nameWidth = available - static_cast<int>(othersTotal);
    }
    widths[nameIndex] = std::max(nameWidth, Scale(60, dpi));

    for (size_t i = 0; i < count; i++) {
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        info.uWidth = static_cast<UINT>(widths[i]);
        state->browserColumnManager->SetColumnInfo(
            state->browserColumnKeys[i], &info);
    }

    Wh_Log(L"browser columns fitted: client=%d available=%d name=%d others=%lld count=%u window=%p class=%s",
          clientWidth, available, widths[nameIndex], othersTotal,
          (unsigned)count, (void*)targetWindow,
          ClassNameOf(targetWindow).c_str());
    if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
        ShowScrollBar(state->browserViewWnd, SB_HORZ, FALSE);
    }
    if (targetWindow && IsWindow(targetWindow)) {
        ShowScrollBar(targetWindow, SB_HORZ, FALSE);
    }
}

static void RestoreBrowserColumns(PanelState* state) {
    if (!state || !state->browserColumnManager ||
        !state->browserColumnsSaved) {
        return;
    }

    for (size_t i = 0; i < state->browserColumnKeys.size(); i++) {
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        info.uWidth = state->browserColumnWidths[i];
        state->browserColumnManager->SetColumnInfo(
            state->browserColumnKeys[i], &info);
    }
    Wh_Log(L"browser columns restored");
    ReleaseBrowserColumns(state);
}

static const UINT_PTR kBorderGuardId = 5;
static const LONG kEdgeStyleBits = WS_BORDER | WS_DLGFRAME;
static const LONG kEdgeExStyleBits =
    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE;

// Explorer re-applies the 3D client edge to the embedded browser windows at
// unpredictable moments (a slow navigation, a theme refresh, DirectUI
// finishing its setup). On a dark view the edge shows up as a white line on
// the right and bottom. This subclass refuses the style change outright.
static LRESULT CALLBACK BorderGuardProc(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam, UINT_PTR subclassId,
                                        DWORD_PTR) {
    if (message == WM_STYLECHANGING && lParam) {
        STYLESTRUCT* style = reinterpret_cast<STYLESTRUCT*>(lParam);
        if (wParam == static_cast<WPARAM>(GWL_EXSTYLE)) {
            style->styleNew &= ~static_cast<DWORD>(kEdgeExStyleBits);
        } else if (wParam == static_cast<WPARAM>(GWL_STYLE)) {
            style->styleNew &= ~static_cast<DWORD>(kEdgeStyleBits);
        }
    }
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, BorderGuardProc, subclassId);
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Returns true when a border style was actually removed.
static bool RemoveBorderStyles(HWND hwnd, bool force) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    SetWindowSubclass(hwnd, BorderGuardProc, kBorderGuardId, 0);

    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    LONG newStyle = style & ~kEdgeStyleBits;
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    LONG newExStyle = exStyle & ~kEdgeExStyleBits;
    bool changed = newStyle != style || newExStyle != exStyle;
    if (newStyle != style) {
        SetWindowLongW(hwnd, GWL_STYLE, newStyle);
    }
    if (newExStyle != exStyle) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, newExStyle);
    }
    if (changed) {
        Wh_Log(L"border removed: hwnd=%p class=%s style=%08lX->%08lX ex=%08lX->%08lX",
              (void*)hwnd, ClassNameOf(hwnd).c_str(), (unsigned long)style,
              (unsigned long)newStyle, (unsigned long)exStyle,
              (unsigned long)newExStyle);
    }
    if (changed || force) {
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
    }
    if (changed) {
        RedrawWindow(hwnd, nullptr, nullptr,
                     RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
    return changed;
}

static HWND GetExplorerBrowserWindow(PanelState* state) {
    if (!state || !state->explorerBrowser) {
        return nullptr;
    }
    HWND window = nullptr;
    IOleWindow* oleWindow = nullptr;
    if (SUCCEEDED(state->explorerBrowser->QueryInterface(
            IID_IOleWindow, (void**)&oleWindow)) &&
        oleWindow) {
        oleWindow->GetWindow(&window);
        oleWindow->Release();
    }
    return window;
}

static BOOL CALLBACK CollectBorderChildProc(HWND child, LPARAM parameter) {
    std::vector<HWND>* windows = reinterpret_cast<std::vector<HWND>*>(parameter);
    std::wstring className = ClassNameOf(child);
    // Never touch the rename edit box or similar real controls.
    if (!IsTextInputClass(className.c_str()) &&
        _wcsicmp(className.c_str(), L"ScrollBar") != 0) {
        windows->push_back(child);
    }
    return TRUE;
}

// Every window of the embedded browser that can carry a border: the browser
// control itself, the shell view and the list chains up to the panel, and all
// descendants.
static void CollectBorderWindows(PanelState* state, std::vector<HWND>& out) {
    if (!state || !state->panel) {
        return;
    }
    auto addChain = [&](HWND start) {
        HWND current = start;
        while (current && IsWindow(current) && current != state->panel) {
            out.push_back(current);
            current = GetParent(current);
        }
    };
    HWND browserWindow = GetExplorerBrowserWindow(state);
    addChain(browserWindow);
    addChain(state->browserViewWnd);
    addChain(state->browserListWnd);
    if (browserWindow && IsWindow(browserWindow)) {
        EnumChildWindows(browserWindow, CollectBorderChildProc,
                         reinterpret_cast<LPARAM>(&out));
    }
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
}

static void ReleaseBorderGuards(PanelState* state) {
    std::vector<HWND> windows;
    CollectBorderWindows(state, windows);
    for (HWND window : windows) {
        RemoveWindowSubclass(window, BorderGuardProc, kBorderGuardId);
    }
}

static void RemoveBrowserBorders(PanelState* state, bool force = true) {
    if (!state || !state->panel) {
        return;
    }
    std::vector<HWND> windows;
    CollectBorderWindows(state, windows);
    for (HWND window : windows) {
        RemoveBorderStyles(window, force);
        if (GetWindowLongW(window, GWL_STYLE) & WS_HSCROLL) {
            ShowScrollBar(window, SB_HORZ, FALSE);
        }
    }
    if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
        ShowScrollBar(state->browserViewWnd, SB_HORZ, FALSE);
    }
    if (state->browserListWnd && IsWindow(state->browserListWnd)) {
        ShowScrollBar(state->browserListWnd, SB_HORZ, FALSE);
    }
}

// Measures how far the columns overflow the visible width (the range of the
// horizontal scrollbar minus its page) and, if they do, reserves that much more
// room and fits again. Bounded, so a stale scroll range can never run away.
static void FitBrowserOverflow(PanelState* state) {
    if (!state || !state->browserColumnsSaved ||
        state->viewMode != QlView::Details || state->liveResize ||
        state->browserFitAttempts >= 4) {
        return;
    }
    std::vector<HWND> windows;
    CollectBorderWindows(state, windows);
    int overflow = 0;
    for (HWND window : windows) {
        if (!IsWindowVisible(window)) {
            continue;
        }
        SCROLLINFO info = {};
        info.cbSize = sizeof(info);
        info.fMask = SIF_RANGE | SIF_PAGE;
        if (GetScrollInfo(window, SB_HORZ, &info) && info.nPage > 0) {
            int range = info.nMax - info.nMin + 1;
            if (range > static_cast<int>(info.nPage)) {
                overflow = std::max(overflow,
                                    range - static_cast<int>(info.nPage));
            }
        }
    }
    if (overflow <= 0) {
        return;
    }
    UINT dpi = GetDpiForWindowSafe(state->panel);
    state->browserFitAttempts++;
    state->browserExtraReserve =
        std::min(Scale(160, dpi),
                 state->browserExtraReserve + overflow + Scale(2, dpi));
    Wh_Log(L"browser overflow: %d px, extra reserve now %d (attempt %d)",
          overflow, state->browserExtraReserve, state->browserFitAttempts);
    ScaleBrowserColumns(state);
}

static void ArmBrowserSettle(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    state->browserSettleTicks = 6;
    SetTimer(state->panel, QL_TIMER_BROWSER_COLUMNS, 200, nullptr);
}

// The embedded browser can show an arbitrary set of items, but only through
// IExplorerBrowser::FillFromObject, and only on a browser that has not navigated
// to a folder yet: on a browsed browser it answers E_FAIL, and RemoveAll would
// leave the view empty forever. So the filtered view is built on a fresh browser:
// the matching direct children of the previewed folder go into an item array,
// FillFromObject turns it into an IResultsFolder that the real DefView renders,
// and later keystrokes only add or remove items from that folder. Clearing the
// text rebuilds the browser and browses the folder again.
static IResultsFolder* GetResultsFolder(PanelState* state) {
    if (!state || !state->browserView) {
        return nullptr;
    }
    IFolderView* view = nullptr;
    if (FAILED(state->browserView->QueryInterface(IID_IFolderView,
                                                  (void**)&view)) ||
        !view) {
        return nullptr;
    }
    IResultsFolder* folder = nullptr;
    view->GetFolder(IID_IResultsFolder, (void**)&folder);
    ReleaseCom(view);
    return folder;
}

// Collects the absolute PIDLs of the first `limit` filtered entries. Only direct
// children of the previewed folder are in state->entries, so the result never
// reaches into a subfolder.
static void CollectFilteredPidls(PanelState* state,
                                 std::vector<PIDLIST_ABSOLUTE>& out,
                                 size_t limit) {
    size_t take = std::min(limit, state->filtered.size());
    out.reserve(take);
    for (size_t i = 0; i < take; i++) {
        int index = state->filtered[i];
        if (index < 0 || index >= static_cast<int>(state->entries.size())) {
            continue;
        }
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(state->entries[index].path.c_str(),
                                         nullptr, &pidl, 0, nullptr)) &&
            pidl) {
            out.push_back(pidl);
        }
    }
}

// FillFromObject walks the object it is given (it is built for data objects and
// namespace walks), so a folder in that list is expanded instead of listed, and
// a matching folder goes missing from the results. One leaf item is enough to
// make the browser create its results folder; every product item is added
// afterwards with IResultsFolder::AddItem, which takes folders as they are.
static IShellItemArray* SeedItemArray(PanelState* state) {
    std::vector<PIDLIST_ABSOLUTE> owned;

    // The mod's own file is a local leaf that always exists and never costs a
    // walk.
    wchar_t modulePath[MAX_PATH] = {};
    if (GetModuleFileNameW(g_instance, modulePath, ARRAYSIZE(modulePath))) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(modulePath, nullptr, &pidl, 0,
                                         nullptr)) &&
            pidl) {
            owned.push_back(pidl);
        }
    }
    if (owned.empty()) {
        CollectFilteredPidls(state, owned, 1);
    }
    if (owned.empty() && !state->entries.empty()) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(state->entries[0].path.c_str(),
                                         nullptr, &pidl, 0, nullptr)) &&
            pidl) {
            owned.push_back(pidl);
        }
    }

    std::vector<PCIDLIST_ABSOLUTE> list;
    list.reserve(owned.size());
    for (PIDLIST_ABSOLUTE pidl : owned) {
        list.push_back(pidl);
    }

    IShellItemArray* items = nullptr;
    if (!list.empty()) {
        SHCreateShellItemArrayFromIDLists(static_cast<UINT>(list.size()),
                                          list.data(), &items);
    }
    for (PIDLIST_ABSOLUTE pidl : owned) {
        ILFree(pidl);
    }
    return items;
}

// A results folder is a brand new shell view: Explorer does not hand it the
// theme it gives the folder view, so its first cells and its infotips come out
// in the light palette. The pass runs from our own timer, never from inside a
// shell callback, and only invalidates: painting synchronously there re-entered
// the view and could keep re-arming the settle loop. The theme is applied only
// to the view and to the list, which is where the shell reads it for its items
// and infotips; walking every descendant is not worth the risk.
// uxtheme ordinal 133: lets a window use the dark palette (scrollbars, header,
// infotips). Explorer does this for its own views, but not for a results view
// created by us, which is why its scrollbars stayed white.
static void AllowDarkModeForWindowSafe(HWND hwnd, bool allow) {
    typedef BOOL(WINAPI * AllowDarkModeForWindowFn)(HWND, BOOL);
    static AllowDarkModeForWindowFn fn = []() -> AllowDarkModeForWindowFn {
        // uxtheme is linked (-luxtheme), so it is always loaded: no
        // LoadLibrary by bare name.
        HMODULE module = GetModuleHandleW(L"uxtheme.dll");
        if (!module) {
            return nullptr;
        }
        return reinterpret_cast<AllowDarkModeForWindowFn>(
            reinterpret_cast<void*>(
                GetProcAddress(module, MAKEINTRESOURCEA(133))));
    }();
    if (fn && hwnd && IsWindow(hwnd)) {
        fn(hwnd, allow ? TRUE : FALSE);
    }
}

static void ThemeOneWindow(HWND hwnd, const wchar_t* theme, bool dark) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    AllowDarkModeForWindowSafe(hwnd, dark);
    SetWindowTheme(hwnd, theme, nullptr);
    SendMessageW(hwnd, WM_THEMECHANGED, 0, 0);
    // Non-client area: this is where the scrollbars live.
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_FRAMECHANGED);
}

static BOOL CALLBACK ThemeChildProc(HWND child, LPARAM parameter) {
    const wchar_t* theme = reinterpret_cast<const wchar_t*>(parameter);
    std::wstring className = ClassNameOf(child);
    // Skip the rename edit box; everything else (list, header, tooltips,
    // scrollbar controls) takes the theme.
    if (_wcsicmp(className.c_str(), L"Edit") != 0) {
        bool dark = _wcsicmp(theme, L"DarkMode_Explorer") == 0;
        AllowDarkModeForWindowSafe(child, dark);
        SetWindowTheme(child, theme, nullptr);
    }
    return TRUE;
}

static void ThemeBrowserView(PanelState* state) {
    if (!state || !state->resultsMode || !state->browserViewWnd ||
        !IsWindow(state->browserViewWnd)) {
        return;
    }

    bool dark = false;
    if (state->background != CLR_INVALID) {
        int luminance = (GetRValue(state->background) * 3 +
                         GetGValue(state->background) * 4 +
                         GetBValue(state->background)) /
                        8;
        dark = luminance < 128;
    }
    const wchar_t* theme = dark ? L"DarkMode_Explorer" : L"Explorer";

    EnsureBrowserListSubclass(state);

    ThemeOneWindow(state->browserViewWnd, theme, dark);
    EnumChildWindows(state->browserViewWnd, ThemeChildProc,
                     reinterpret_cast<LPARAM>(theme));
    if (state->browserListWnd && IsWindow(state->browserListWnd)) {
        ThemeOneWindow(state->browserListWnd, theme, dark);
        ShowScrollBar(state->browserListWnd, SB_HORZ, FALSE);
    }
    ShowScrollBar(state->browserViewWnd, SB_HORZ, FALSE);

    // RDW_FRAME is what repaints the scrollbars.
    RedrawWindow(state->browserViewWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);

    state->browserThemed = true;
}

static void ScheduleThemeBrowserView(PanelState* state) {
    if (!state || !state->resultsMode || !state->panel ||
        !IsWindow(state->panel)) {
        return;
    }
    SetTimer(state->panel, QL_TIMER_BROWSER_THEME, 40, nullptr);
}

// Destroys and rebuilds the browser so a filtered results folder can be filled
// on a browser that has not browsed anything yet.
static void RecreateBrowser(PanelState* state, bool keepPath) {
    std::wstring folder = state->folderPath;
    DestroyExplorerBrowser(state);
    if (!EnsureExplorerBrowser(state)) {
        return;
    }
    SetExplorerBrowserRect(state);
    // Pretending the browser is on the folder keeps the navigation veto
    // working: a double click inside the results view is still not navigation.
    state->browserPath = keepPath ? folder : L"";
}

// Adds the queued filtered items to the results folder for at most ~12 ms, then
// hands the thread back and continues from a timer. Typing "a" in a big folder
// matches thousands of items, and resolving all of them in one go would freeze
// Explorer for seconds.
static void PumpResultsItems(PanelState* state) {
    if (!state || state->resultsPendingPos >= state->resultsPending.size()) {
        if (state) {
            state->resultsPending.clear();
            state->resultsPendingPos = 0;
        }
        return;
    }
    IResultsFolder* folder = GetResultsFolder(state);
    if (!folder) {
        state->resultsPending.clear();
        state->resultsPendingPos = 0;
        return;
    }

    ULONGLONG start = GetTickCount64();
    size_t pos = state->resultsPendingPos;
    while (pos < state->resultsPending.size()) {
        int index = state->resultsPending[pos++];
        if (index >= 0 && index < static_cast<int>(state->entries.size())) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(SHCreateItemFromParsingName(
                    state->entries[index].path.c_str(), nullptr,
                    IID_IShellItem, (void**)&item)) &&
                item) {
                folder->AddItem(item);
                ReleaseCom(item);
            }
        }
        if (GetTickCount64() - start >= 12) {
            break;
        }
    }
    state->resultsPendingPos = pos;
    ReleaseCom(folder);

    if (pos < state->resultsPending.size()) {
        if (state->panel && IsWindow(state->panel)) {
            SetTimer(state->panel, QL_TIMER_RESULTS, 20, nullptr);
        }
    } else {
        state->resultsPending.clear();
        state->resultsPendingPos = 0;
        Wh_Log(L"results folder: items=%u", (unsigned)state->filtered.size());
        ScheduleThemeBrowserView(state);
        ArmBrowserSettle(state);
    }
}

static void FillResults(PanelState* state) {
    if (!state || !state->explorerBrowser) {
        return;
    }
    state->resultsPending.clear();
    state->resultsPendingPos = 0;

    IShellItemArray* seed = SeedItemArray(state);
    if (!seed) {
        return;
    }

    // Marked before the call: FillFromObject can complete the navigation
    // synchronously, and the events must not rebuild the browser underneath it.
    state->resultsMode = true;
    state->fillingResults = true;
    HRESULT hr = state->explorerBrowser->FillFromObject(seed, EBF_NODROPTARGET);
    state->fillingResults = false;
    ReleaseCom(seed);
    state->resultsMode = SUCCEEDED(hr);

    IResultsFolder* folder = nullptr;
    if (SUCCEEDED(hr)) {
        folder = GetResultsFolder(state);
        if (folder) {
            folder->RemoveAll();  // drop the seed item
        }
    }
    Wh_Log(L"results folder: fill hr=0x%08lX filtered=%u", (unsigned long)hr,
          (unsigned)state->filtered.size());

    if (FAILED(hr) || !folder) {
        ReleaseCom(folder);
        // Never leave the panel without a list: fall back to the folder view.
        // resultsFilter keeps this text, so the fallback does not retry the
        // same fill (that would recreate the browser in a loop).
        state->resultsMode = false;
        RecreateBrowser(state, false);
        SyncExplorerBrowser(state);
        ArmBrowserSettle(state);
        return;
    }
    ReleaseCom(folder);

    // Every matching item, folders included, is added directly: FillFromObject
    // would walk a folder instead of listing it, which is how it went missing.
    state->resultsPending = state->filtered;
    state->resultsPendingPos = 0;
    if (state->resultsPending.empty()) {
        ScheduleThemeBrowserView(state);
        ArmBrowserSettle(state);
    } else {
        PumpResultsItems(state);
    }
}

static void UpdateResultsItems(PanelState* state) {
    IResultsFolder* folder = GetResultsFolder(state);
    if (!folder) {
        // The results folder is not there yet; rebuild it from scratch.
        state->resultsMode = false;
        RecreateBrowser(state, true);
        FillResults(state);
        return;
    }

    folder->RemoveAll();
    ReleaseCom(folder);
    state->resultsPending = state->filtered;
    state->resultsPendingPos = 0;
    PumpResultsItems(state);
}

static void ApplyBrowserFilter(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel) ||
        !state->folderPreview || state->archivePreview) {
        return;
    }

    bool wantResults = !state->filter.empty();

    if (!wantResults) {
        state->resultsFilter.clear();
        if (!state->resultsMode) {
            return;
        }
        state->resultsMode = false;
        RecreateBrowser(state, false);
        SyncExplorerBrowser(state);
        ArmBrowserSettle(state);
        Wh_Log(L"results folder: back to the folder view");
        return;
    }

    if (state->resultsMode) {
        if (state->resultsFilter == state->filter) {
            return;
        }
        state->resultsFilter = state->filter;
        UpdateResultsItems(state);
        return;
    }

    // One attempt per filter text. If the shell refuses the results folder, the
    // folder view stays up instead of the fill being retried in a loop.
    if (state->resultsFilter == state->filter) {
        return;
    }
    RecreateBrowser(state, true);
    state->resultsFilter = state->filter;
    FillResults(state);
}

static void OnBrowserViewCreated(PanelState* state, IShellView* view) {
    if (!state || !view) {
        return;
    }
    if (state->browserListWnd && IsWindow(state->browserListWnd)) {
        RemoveWindowSubclass(state->browserListWnd, BrowserListSubclassProc,
                             4);
    }
    state->browserListWnd = nullptr;
    if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
        RemoveWindowSubclass(state->browserViewWnd, BrowserListSubclassProc,
                             5);
    }
    ReleaseBrowserColumns(state);

    ReleaseCom(state->browserView);
    state->browserView = view;
    view->AddRef();

    HWND viewWindow = nullptr;
    view->GetWindow(&viewWindow);
    state->browserViewWnd = viewWindow;
    state->browserListWnd = nullptr;

    Wh_Log(L"browser view created: view=%p window=%p class=%s", (void*)view,
          (void*)viewWindow, ClassNameOf(viewWindow).c_str());

    state->browserThemed = false;
    state->browserFitAttempts = 0;
    ApplyBrowserViewMode(state);
    // A results folder has its own columns: capturing them would later be
    // restored onto the real folder view.
    if (!state->resultsMode && state->viewMode == QlView::Details) {
        SaveBrowserColumns(state);
        ScaleBrowserColumns(state);
    }
    EnsureBrowserListSubclass(state);
    RemoveBrowserBorders(state);
    if (state->resultsMode) {
        ScheduleThemeBrowserView(state);
    }
    ArmBrowserSettle(state);
}

// True when `path` is `root` or a location inside it. Archive parsing names use
// the archive file as their root, so one text comparison covers both.
static bool IsSameOrInside(const std::wstring& path, const std::wstring& root) {
    if (SameFolderPath(path, root)) {
        return true;
    }
    if (root.empty() || path.size() <= root.size()) {
        return false;
    }
    if (CompareStringOrdinal(path.c_str(), static_cast<int>(root.size()),
                             root.c_str(), static_cast<int>(root.size()),
                             TRUE) != CSTR_EQUAL) {
        return false;
    }
    wchar_t last = root.back();
    wchar_t next = path[root.size()];
    return last == L'\\' || last == L'/' || next == L'\\' || next == L'/';
}

static bool SameFolderPath(const std::wstring& left,
                           const std::wstring& right) {
    if (left.empty() || right.empty()) {
        return false;
    }
    std::wstring a = left;
    std::wstring b = right;
    while (a.size() > 3 && (a.back() == L'\\' || a.back() == L'/')) {
        a.pop_back();
    }
    while (b.size() > 3 && (b.back() == L'\\' || b.back() == L'/')) {
        b.pop_back();
    }
    return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) ==
           CSTR_EQUAL;
}

class ExplorerBrowserEvents final : public IExplorerBrowserEvents {
   public:
    explicit ExplorerBrowserEvents(PanelState* state) : state_(state) {}
    ~ExplorerBrowserEvents() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IExplorerBrowserEvents) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&ref_));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG value = InterlockedDecrement(&ref_);
        if (!value) {
            delete this;
        }
        return static_cast<ULONG>(value);
    }

    HRESULT STDMETHODCALLTYPE OnNavigationPending(
        PCIDLIST_ABSOLUTE pidlFolder) override {
        if (!state_ || !pidlFolder || state_->browserPath.empty()) {
            return S_OK;
        }
        // FillFromObject navigates the browser to its own results folder. That
        // is our fill, not the user leaving the preview: let it through, or the
        // fill fails and the retry recreates the browser in a loop.
        if (state_->fillingResults) {
            return S_OK;
        }
        std::wstring target;
        bool fileSystemPath = false;
        wchar_t buffer[MAX_PATH] = {};
        if (SHGetPathFromIDListW(pidlFolder, buffer)) {
            target = buffer;
            fileSystemPath = true;
        } else {
            // A location inside an archive is not a file system path, but its
            // parsing name still says where it is ("C:\a.zip\folder").
            PWSTR name = nullptr;
            if (SUCCEEDED(SHGetNameFromIDList(
                    pidlFolder, SIGDN_DESKTOPABSOLUTEPARSING, &name)) &&
                name) {
                target = name;
                CoTaskMemFree(name);
            }
        }
        if (target.empty()) {
            // The archive root always resolves, so an unknown place inside an
            // archive is somewhere deeper and is refused.
            return state_->archivePreview ? E_ABORT : S_OK;
        }
        if (SameFolderPath(target, state_->browserPath)) {
            return S_OK;
        }
        // This panel is a preview, never a navigator: the navigation is always
        // cancelled. Only an item of the previewed folder becomes an open
        // request (a folder shows its Properties); the parent folder, a results
        // folder or any other namespace target is refused silently, never
        // opened as an Explorer window.
        if (!IsSameOrInside(target, state_->browserPath)) {
            Wh_Log(L"navigation blocked outside the preview: %s", target.c_str());
            return E_ABORT;
        }
        Wh_Log(L"navigation blocked: %s (root=%s)", target.c_str(),
              state_->browserPath.c_str());
        if (!state_->archivePreview && fileSystemPath) {
            RequestOpenPath(state_, target);
        }
        return E_ABORT;
    }

    HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView* view) override {
        if (state_) {
            OnBrowserViewCreated(state_, view);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnNavigationComplete(
        PCIDLIST_ABSOLUTE) override {
        if (state_) {
            EnsureBrowserListSubclass(state_);
            ArmBrowserSettle(state_);
            ApplyBrowserFilter(state_);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnNavigationFailed(
        PCIDLIST_ABSOLUTE) override {
        Wh_Log(L"browser navigation failed");
        return S_OK;
    }

   private:
    LONG ref_ = 1;
    PanelState* state_ = nullptr;
};

// ---------------------------------------------------------------------------
// Browser cover: hides the first, unfinished frame of a new browser
// ---------------------------------------------------------------------------

static void RepositionBrowserCover(PanelState* state) {
    if (!state || !state->coverActive || !state->coverWnd ||
        !IsWindow(state->coverWnd)) {
        return;
    }
    const RECT& r = state->listRect;
    SetWindowPos(state->coverWnd, HWND_TOP, r.left, r.top, r.right - r.left,
                 r.bottom - r.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static void BeginBrowserCover(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }
    COLORREF color = state->background != CLR_INVALID
                         ? state->background
                         : GetSysColor(COLOR_WINDOW);
    if (!state->coverBrush || state->coverColor != color) {
        if (state->coverBrush) {
            DeleteObject(state->coverBrush);
        }
        state->coverBrush = CreateSolidBrush(color);
        state->coverColor = color;
    }
    if (!state->coverWnd || !IsWindow(state->coverWnd)) {
        state->coverWnd = CreateWindowExW(0, L"STATIC", L"", WS_CHILD, 0, 0, 0,
                                          0, state->panel, nullptr,
                                          g_instance, nullptr);
    }
    if (!state->coverWnd) {
        return;
    }
    state->coverActive = true;
    state->coverTicks = 0;
    state->browserThemed = false;
    RepositionBrowserCover(state);
    // Painted right now, before the browser gets a chance to paint anything.
    RedrawWindow(state->coverWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
    SetTimer(state->panel, QL_TIMER_COVER, 30, nullptr);
}

static void EndBrowserCover(PanelState* state) {
    if (!state || !state->coverActive) {
        return;
    }
    state->coverActive = false;
    if (state->panel && IsWindow(state->panel)) {
        KillTimer(state->panel, QL_TIMER_COVER);
    }
    if (state->coverWnd && IsWindow(state->coverWnd)) {
        ShowWindow(state->coverWnd, SW_HIDE);
    }
    HWND browserWindow = GetExplorerBrowserWindow(state);
    if (browserWindow && IsWindow(browserWindow)) {
        RedrawWindow(browserWindow, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }
}

// Runs every 30 ms while the cover is up. The browser is revealed when its view
// exists, a results view has its dark theme, and the list has its first items
// (or enough time passed for an empty folder / no matches).
static void TickBrowserCover(PanelState* state) {
    if (!state || !state->coverActive) {
        return;
    }
    state->coverTicks++;

    if (!state->explorerBrowser || !state->folderPreview) {
        EndBrowserCover(state);
        return;
    }

    // Keep the cover above the browser, whatever it does to its own z-order.
    if (state->coverWnd && IsWindow(state->coverWnd)) {
        SetWindowPos(state->coverWnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    bool ready = false;
    if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
        EnsureBrowserListSubclass(state);
        bool themed = !state->resultsMode || state->browserThemed;
        if (themed && state->coverTicks >= 3) {
            if (state->browserListWnd && IsWindow(state->browserListWnd) &&
                _wcsicmp(ClassNameOf(state->browserListWnd).c_str(),
                         L"SysListView32") == 0) {
                int items = static_cast<int>(SendMessageW(
                    state->browserListWnd, LVM_GETITEMCOUNT, 0, 0));
                ready = items > 0 || state->coverTicks >= 10;
            } else {
                ready = state->coverTicks >= 8;
            }
        }
    }
    // Safety net: never keep the list covered for long.
    if (!ready && state->coverTicks >= 30) {
        ready = true;
    }
    if (ready) {
        EndBrowserCover(state);
    }
}

static bool EnsureExplorerBrowser(PanelState* state) {
    if (!state || !state->panel) {
        return false;
    }

    if (!state->explorerBrowser) {
        // Before the browser exists, so its first paint is already covered.
        BeginBrowserCover(state);
        IExplorerBrowser* browser = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IExplorerBrowser, (void**)&browser);
        if (FAILED(hr) || !browser) {
            Wh_Log(L"browser create failed: 0x%08X", (unsigned)hr);
            return false;
        }

        RECT rect = state->listRect;
        FOLDERSETTINGS settings = {};
        settings.ViewMode = FVM_DETAILS;
        settings.fFlags = 0;

        hr = browser->Initialize(state->panel, &rect, &settings);
        if (FAILED(hr)) {
            Wh_Log(L"browser initialize failed: 0x%08X", (unsigned)hr);
            browser->Release();
            return false;
        }

        browser->SetOptions((EXPLORER_BROWSER_OPTIONS)(
            EBO_NOBORDER | EBO_NOPERSISTVIEWSTATE));

        ExplorerBrowserEvents* events = new ExplorerBrowserEvents(state);
        if (SUCCEEDED(browser->Advise(events, &state->browserCookie))) {
            state->browserEvents = events;
        } else {
            events->Release();
        }

        state->explorerBrowser = browser;
        RemoveBrowserBorders(state, true);
        Wh_Log(L"browser created: browser=%p", (void*)browser);
    }

    return true;
}

static void SetExplorerBrowserRect(PanelState* state) {
    if (!state || !state->explorerBrowser) {
        return;
    }
    state->explorerBrowser->SetRect(nullptr, state->listRect);
    RepositionBrowserCover(state);
}

static void DestroyExplorerBrowser(PanelState* state) {
    if (!state) {
        return;
    }

    if (state->explorerBrowser) {
        if (state->browserCookie) {
            state->explorerBrowser->Unadvise(state->browserCookie);
            state->browserCookie = 0;
        }
        if (state->browserEvents) {
            state->browserEvents->Release();
            state->browserEvents = nullptr;
        }
        if (state->browserListWnd && IsWindow(state->browserListWnd)) {
            RemoveWindowSubclass(state->browserListWnd,
                                 BrowserListSubclassProc, 4);
        }
        ReleaseBorderGuards(state);
        state->browserListWnd = nullptr;
        if (state->browserViewWnd && IsWindow(state->browserViewWnd)) {
            RemoveWindowSubclass(state->browserViewWnd,
                                 BrowserListSubclassProc, 5);
        }
        RestoreBrowserColumns(state);
        ReleaseCom(state->browserView);
        state->browserViewWnd = nullptr;
        state->explorerBrowser->Destroy();
        ReleaseCom(state->explorerBrowser);
    }
    state->browserPath.clear();
    state->browserThemed = false;
    state->resultsMode = false;
    state->resultsPending.clear();
    state->resultsPendingPos = 0;
}

static void SyncExplorerBrowser(PanelState* state) {
    if (!state) {
        return;
    }
    if (!state->folderPreview || state->folderPath.empty()) {
        DestroyExplorerBrowser(state);
        UpdateFilterEditVisibility(state);
        return;
    }
    if (!EnsureExplorerBrowser(state)) {
        return;
    }

    // Leaving a filtered results folder because the text went away, the panel
    // moved to another item or the panel was reopened.
    if (state->resultsMode && state->filter.empty()) {
        state->resultsMode = false;
        state->resultsFilter.clear();
        RecreateBrowser(state, false);
        if (!state->explorerBrowser) {
            return;
        }
    }

    UpdateFilterEditVisibility(state);

    SetExplorerBrowserRect(state);

    // A results folder is filled, not browsed; browsing here would fail and
    // would also throw away the filtered item set.
    if (state->resultsMode) {
        return;
    }

    if (state->browserPath != state->folderPath) {
        state->browserPath = state->folderPath;
        IShellItem* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(
                state->folderPath.c_str(), nullptr, IID_IShellItem,
                (void**)&item)) &&
            item) {
            state->explorerBrowser->BrowseToObject(item, 0);
            ReleaseCom(item);
        } else {
            Wh_Log(L"browser browse failed: %s", state->folderPath.c_str());
        }
    }
    ApplyBrowserFilter(state);
}

// Updates the panel when the user changes the selected item with the keyboard
// or the mouse. The timer lives on the panel; it is created when the panel
// opens and killed when it closes.
static void RefreshSelection(PanelState* state) {
    if (!state || !state->open || !state->panel || !IsWindow(state->panel) ||
        !state->view || !IsWindow(state->view) || !state->shellView) {
        return;
    }

    HWND viewWindow = nullptr;
    if (FAILED(state->shellView->GetWindow(&viewWindow)) ||
        viewWindow != state->view) {
        // The folder view was replaced: the user navigated somewhere else.
        // The panel belongs to the old view, so it is closed.
        Wh_Log(L"selection: view changed, closing the panel");
        CloseState(state);
        return;
    }

    std::wstring debug;
    std::wstring path = GetSelectedPath(state->shellView, &debug);
    bool pathChanged = path != state->path;
    if (pathChanged || debug != state->debug) {
        state->path = path;
        state->debug = debug;
        if (pathChanged) {
            LoadSelectedItem(state, false);
            LayoutPanelContent(state);
        }
        InvalidateRect(state->panel, nullptr, TRUE);
        Wh_Log(L"selection: path=%s debug=%s", path.c_str(), debug.c_str());
    }

    if (state->explorerBrowser) {
        EnsureBrowserListSubclass(state);
        // Cheap watchdog: removes a border that Explorer re-applied late.
        if ((++state->borderWatchTick % 3) == 0 && !state->liveResize) {
            RemoveBrowserBorders(state, false);
            FitBrowserOverflow(state);
        }
    }
}

// ---------------------------------------------------------------------------
// Theme colors
// ---------------------------------------------------------------------------

static int ColorLuma(COLORREF color) {
    return (GetRValue(color) * 299 + GetGValue(color) * 587 +
            GetBValue(color) * 114) /
           1000;
}

static COLORREF BlendColor(COLORREF from, COLORREF to, int percent) {
    percent = std::clamp(percent, 0, 100);
    int r = GetRValue(from) + (GetRValue(to) - GetRValue(from)) * percent / 100;
    int g = GetGValue(from) + (GetGValue(to) - GetGValue(from)) * percent / 100;
    int b = GetBValue(from) + (GetBValue(to) - GetBValue(from)) * percent / 100;
    return RGB(r, g, b);
}

static int ColorDistance(COLORREF a, COLORREF b) {
    return std::abs(ColorLuma(a) - ColorLuma(b));
}

static bool IsNeutralColor(COLORREF color) {
    int maximum = std::max({GetRValue(color), GetGValue(color),
                            GetBValue(color)});
    int minimum = std::min({GetRValue(color), GetGValue(color),
                            GetBValue(color)});
    return maximum - minimum <= 30;
}

static COLORREF FilterBoxFill(const PanelState* state) {
    COLORREF background = state && state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state && state->text != CLR_INVALID
                        ? state->text
                        : GetSysColor(COLOR_WINDOWTEXT);
    return BlendColor(background, text, 6);
}

static void RefreshFilterBrush(PanelState* state) {
    if (!state) {
        return;
    }
    if (state->filterBrush) {
        DeleteObject(state->filterBrush);
        state->filterBrush = nullptr;
    }
    state->filterBrush = CreateSolidBrush(FilterBoxFill(state));
    if (state->filterEdit && IsWindow(state->filterEdit)) {
        InvalidateRect(state->filterEdit, nullptr, TRUE);
    }
}

// The user's Windows accent, read once from the same source Windows apps use:
// UISettings (Windows.UI.ViewManagement). The registry values and
// COLOR_HIGHLIGHT are only a fallback, because they are not always the color
// Windows really shows. The same call gives the accent shades Windows itself
// uses: Accent is the selected-text background, and the focus underline of a
// text box is AccentLight2 in dark mode and AccentDark1 in light mode.
struct UserAccent {
    COLORREF accent = CLR_INVALID;
    COLORREF light2 = CLR_INVALID;
    COLORREF dark1 = CLR_INVALID;
    bool fromUiSettings = false;
};

static bool ReadAccentDword(const wchar_t* subKey, const wchar_t* value,
                            DWORD& out) {
    DWORD size = sizeof(out);
    return RegGetValueW(HKEY_CURRENT_USER, subKey, value, RRF_RT_REG_DWORD,
                        nullptr, &out, &size) == ERROR_SUCCESS;
}

static bool ReadAccentFromUISettings(UserAccent& out) {
    HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool initialized = SUCCEEDED(initHr);

    bool ok = false;
    try {
        winrt::Windows::UI::ViewManagement::UISettings settings;
        using winrt::Windows::UI::ViewManagement::UIColorType;
        const auto accent = settings.GetColorValue(UIColorType::Accent);
        const auto light2 = settings.GetColorValue(UIColorType::AccentLight2);
        const auto dark1 = settings.GetColorValue(UIColorType::AccentDark1);
        out.accent = RGB(accent.R, accent.G, accent.B);
        out.light2 = RGB(light2.R, light2.G, light2.B);
        out.dark1 = RGB(dark1.R, dark1.G, dark1.B);
        out.fromUiSettings = true;
        ok = true;
    } catch (...) {
        // No UISettings here: the caller falls back to the registry value.
        // Catch everything: an exception escaping into explorer.exe would
        // take the shell down.
    }

    if (initialized) {
        CoUninitialize();
    }
    return ok;
}

// ---------------------------------------------------------------------------
// The user's Windows accent color, as plain global variables.
//
// They never hold a color of their own: they only hold whatever Windows says
// the accent is. RefreshWindowsAccent() asks Windows and stores the answer; it
// runs at start-up, every time the panel opens, and whenever Windows reports a
// color or setting change, so changing the accent in Settings is picked up by
// itself. Every drawing routine just reads these.
//   g_windowsAccentColor  - the accent (selected-text background)
//   g_windowsAccentLight2 - focus underline of a text box in dark mode
//   g_windowsAccentDark1  - focus underline of a text box in light mode
// ---------------------------------------------------------------------------
static std::atomic<COLORREF> g_windowsAccentColor{CLR_INVALID};
static std::atomic<COLORREF> g_windowsAccentLight2{CLR_INVALID};
static std::atomic<COLORREF> g_windowsAccentDark1{CLR_INVALID};

// Returns true when the accent is different from the one stored before.
static bool RefreshWindowsAccent() {
    UserAccent accent;
    if (!ReadAccentFromUISettings(accent)) {
        // Fallback only: the registry value Windows writes for the accent.
        DWORD value = 0;
        if (ReadAccentDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\"
                            L"Explorer\\Accent",
                            L"AccentColorMenu", value) &&
            (value & 0x00FFFFFF)) {
            accent.accent = value & 0x00FFFFFF;  // 0xAABBGGRR
        } else {
            accent.accent = GetSysColor(COLOR_HIGHLIGHT);
        }
        accent.light2 = accent.accent;
        accent.dark1 = accent.accent;
    }

    bool changed = g_windowsAccentColor.load() != accent.accent ||
                   g_windowsAccentLight2.load() != accent.light2 ||
                   g_windowsAccentDark1.load() != accent.dark1;
    g_windowsAccentColor = accent.accent;
    g_windowsAccentLight2 = accent.light2;
    g_windowsAccentDark1 = accent.dark1;
    if (changed) {
        Wh_Log(L"windows accent (%s): accent=%06X light2=%06X dark1=%06X",
               accent.fromUiSettings ? L"UISettings" : L"fallback",
               (unsigned)accent.accent, (unsigned)accent.light2,
               (unsigned)accent.dark1);
    }
    return changed;
}

static COLORREF GetUserAccentColor() {
    if (g_windowsAccentColor.load() == CLR_INVALID) {
        RefreshWindowsAccent();
    }
    return g_windowsAccentColor.load();
}

// The color Windows gives the focus underline of a text box.
static COLORREF GetUserAccentFocusColor(bool darkTheme) {
    if (g_windowsAccentColor.load() == CLR_INVALID) {
        RefreshWindowsAccent();
    }
    return darkTheme ? g_windowsAccentLight2.load()
                     : g_windowsAccentDark1.load();
}

// The filter text field paints itself: the stock EDIT draws its selection with
// the system highlight (classic blue), which ignores the user's accent. This
// subclass renders the text, the "Filter by name" hint and the selection with
// the accent color, double-buffered, like the native Windows search box. The
// control still owns editing, scrolling and the caret.
static LRESULT CALLBACK FilterEditSubclassProc(HWND hwnd, UINT message,
                                               WPARAM wParam, LPARAM lParam,
                                               UINT_PTR subclassId,
                                               DWORD_PTR refData) {
    PanelState* state = reinterpret_cast<PanelState*>(refData);

    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE && state) {
                // The filter belongs to the preview, so Esc closes it here as
                // it does everywhere else in the panel.
                CloseState(state);
                return 0;
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC dc = BeginPaint(hwnd, &ps);
            RECT client = {};
            GetClientRect(hwnd, &client);
            int width = client.right - client.left;
            int height = client.bottom - client.top;
            if (width > 0 && height > 0) {
                HDC memory = CreateCompatibleDC(dc);
                HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);
                HGDIOBJ oldBitmap = SelectObject(memory, bitmap);

                COLORREF fill = FilterBoxFill(state);
                COLORREF background = state && state->background != CLR_INVALID
                                          ? state->background
                                          : GetSysColor(COLOR_WINDOW);
                COLORREF text = state && state->text != CLR_INVALID
                                    ? state->text
                                    : GetSysColor(COLOR_WINDOWTEXT);
                HBRUSH fillBrush = CreateSolidBrush(fill);
                FillRect(memory, &client, fillBrush);
                DeleteObject(fillBrush);

                HFONT font = reinterpret_cast<HFONT>(
                    SendMessageW(hwnd, WM_GETFONT, 0, 0));
                HGDIOBJ oldFont =
                    SelectObject(memory, font ? font : GetStockObject(
                                                           DEFAULT_GUI_FONT));
                SetBkMode(memory, TRANSPARENT);

                TEXTMETRICW metrics = {};
                GetTextMetricsW(memory, &metrics);
                int textTop = (height - metrics.tmHeight) / 2;

                RECT format = client;
                SendMessageW(hwnd, EM_GETRECT, 0,
                             reinterpret_cast<LPARAM>(&format));

                int length = GetWindowTextLengthW(hwnd);
                if (length <= 0) {
                    const wchar_t* hint = L"Filter by name";
                    SetTextColor(memory, BlendColor(background, text, 55));
                    TextOutW(memory, format.left, textTop, hint,
                             static_cast<int>(wcslen(hint)));
                } else {
                    std::wstring value(static_cast<size_t>(length) + 1, L'\0');
                    GetWindowTextW(hwnd, &value[0], length + 1);
                    value.resize(static_cast<size_t>(length));

                    // Where the first character is, scroll offset included.
                    LRESULT origin =
                        SendMessageW(hwnd, EM_POSFROMCHAR, 0, 0);
                    int originX = static_cast<short>(LOWORD(origin));

                    auto xAt = [&](size_t index) {
                        SIZE size = {};
                        GetTextExtentPoint32W(memory, value.c_str(),
                                              static_cast<int>(index), &size);
                        return originX + static_cast<int>(size.cx);
                    };

                    DWORD selStart = 0;
                    DWORD selEnd = 0;
                    SendMessageW(hwnd, EM_GETSEL,
                                 reinterpret_cast<WPARAM>(&selStart),
                                 reinterpret_cast<LPARAM>(&selEnd));
                    bool focused = GetFocus() == hwnd;

                    HRGN textClip = CreateRectRgn(format.left, 0,
                                                  format.right, height);
                    SelectClipRgn(memory, textClip);

                    SetTextColor(memory, text);
                    TextOutW(memory, originX, textTop, value.c_str(),
                             static_cast<int>(value.size()));

                    if (focused && selEnd > selStart &&
                        selEnd <= static_cast<DWORD>(value.size())) {
                        COLORREF accent = GetUserAccentColor();
                        if (accent == CLR_INVALID) {
                            accent = GetSysColor(COLOR_HIGHLIGHT);
                        }
                        RECT selection = {xAt(selStart), textTop,
                                          xAt(selEnd),
                                          textTop + metrics.tmHeight};
                        HBRUSH accentBrush = CreateSolidBrush(accent);
                        FillRect(memory, &selection, accentBrush);
                        DeleteObject(accentBrush);

                        // The selected letters again, in a color that reads
                        // on the accent.
                        HRGN selectionClip = CreateRectRgn(
                            std::max(selection.left,
                                     static_cast<LONG>(format.left)),
                            0,
                            std::min(selection.right,
                                     static_cast<LONG>(format.right)),
                            height);
                        SelectClipRgn(memory, selectionClip);
                        SetTextColor(memory, ColorLuma(accent) > 190
                                                 ? RGB(0, 0, 0)
                                                 : RGB(255, 255, 255));
                        TextOutW(memory, originX, textTop, value.c_str(),
                                 static_cast<int>(value.size()));
                        DeleteObject(selectionClip);
                    }
                    SelectClipRgn(memory, nullptr);
                    DeleteObject(textClip);
                }

                SelectObject(memory, oldFont);
                BitBlt(dc, 0, 0, width, height, memory, 0, 0, SRCCOPY);
                SelectObject(memory, oldBitmap);
                DeleteObject(bitmap);
                DeleteDC(memory);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, FilterEditSubclassProc, subclassId);
            break;
    }

    LRESULT result = DefSubclassProc(hwnd, message, wParam, lParam);

    // Anything that can move the selection or the focus repaints the field, so
    // the accent selection never lags behind.
    switch (message) {
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MOUSEMOVE:
        case WM_CUT:
        case WM_PASTE:
        case WM_CLEAR:
        case WM_UNDO:
        case WM_SETTEXT:
        case EM_SETSEL:
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
    }
    return result;
}

// Samples the visible file view and derives the panel palette from what
// Explorer is actually drawing. Nothing here is a hardcoded theme color: the
// background and text come from the real pixels, and the accent is the user's
// Windows accent color.
static void UpdatePanelColors(PanelState* state) {
    // Ask Windows for the accent again: this runs on every open and on every
    // color / setting change.
    RefreshWindowsAccent();
    if (!state || !state->view || !IsWindow(state->view)) {
        return;
    }

    RECT rect = {};
    if (!GetWindowRect(state->view, &rect) || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return;
    }

    // When the panel overlays the view, do not sample the pixels the panel
    // itself covers. The visible part of the view is the real Explorer surface.
    if (state->open && !g_reflowColumns.load() && state->panel &&
        IsWindowVisible(state->panel)) {
        RECT panelRect = {};
        if (GetWindowRect(state->panel, &panelRect)) {
            if (panelRect.left > rect.left && panelRect.left < rect.right) {
                rect.right = panelRect.left - 2;
            } else if (panelRect.top > rect.top &&
                       panelRect.top < rect.bottom) {
                rect.bottom = panelRect.top - 2;
            }
        }
    }

    ULONGLONG start = GetTickCount64();
    std::map<COLORREF, int> counts;
    UINT dpi = GetDpiForWindowSafe(state->view);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (width > 0 && height > 0) {
        HDC screen = GetDC(nullptr);
        if (screen) {
            HDC memory = CreateCompatibleDC(screen);
            HBITMAP bitmap =
                CreateCompatibleBitmap(screen, width, height);
            if (memory && bitmap) {
                HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
                BOOL copied =
                    BitBlt(memory, 0, 0, width, height, screen, rect.left,
                           rect.top, SRCCOPY | CAPTUREBLT);
                SelectObject(memory, oldBitmap);

                if (copied) {
                    BITMAPINFO info = {};
                    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    info.bmiHeader.biWidth = width;
                    info.bmiHeader.biHeight = -height;
                    info.bmiHeader.biPlanes = 1;
                    info.bmiHeader.biBitCount = 32;
                    info.bmiHeader.biCompression = BI_RGB;

                    std::vector<DWORD> pixels(
                        static_cast<size_t>(width) * height);
                    int lines = GetDIBits(memory, bitmap, 0, height,
                                          pixels.data(), &info,
                                          DIB_RGB_COLORS);
                    if (lines == height) {
                        int step = std::max(2, Scale(4, dpi));
                        for (int y = 0; y < height; y += step) {
                            for (int x = 0; x < width; x += step) {
                                DWORD pixel =
                                    pixels[static_cast<size_t>(y) * width + x];
                                COLORREF color =
                                    RGB((pixel >> 16) & 0xFF,
                                        (pixel >> 8) & 0xFF, pixel & 0xFF);
                                counts[color]++;
                            }
                        }
                    }
                }
            }
            if (bitmap) {
                DeleteObject(bitmap);
            }
            if (memory) {
                DeleteDC(memory);
            }
            ReleaseDC(nullptr, screen);
        }
    }

    if (counts.empty()) {
        Wh_Log(L"colors: no pixels sampled, falling back to system colors");
        state->background = GetSysColor(COLOR_WINDOW);
        state->text = GetSysColor(COLOR_WINDOWTEXT);
        state->accent = GetUserAccentColor();
        RefreshFilterBrush(state);
        return;
    }

    COLORREF background = counts.begin()->first;
    int backgroundCount = 0;
    for (const auto& entry : counts) {
        if (entry.second > backgroundCount) {
            backgroundCount = entry.second;
            background = entry.first;
        }
    }

    COLORREF text = CLR_INVALID;
    int textCount = 0;
    for (const auto& entry : counts) {
        if (ColorDistance(entry.first, background) < 60 ||
            !IsNeutralColor(entry.first)) {
            continue;
        }
        if (entry.second > textCount) {
            textCount = entry.second;
            text = entry.first;
        }
    }
    if (textCount < 6 || text == CLR_INVALID ||
        ColorDistance(text, background) < 50) {
        text = ColorLuma(background) > 140 ? RGB(0, 0, 0) : RGB(255, 255, 255);
    }

    COLORREF accent = GetUserAccentColor();
    if (accent == CLR_INVALID ||
        ColorDistance(accent, background) < 20) {
        accent = BlendColor(background, text, 35);
    }

    state->background = background;
    state->text = text;
    state->accent = accent;
    RefreshFilterBrush(state);

    Wh_Log(L"colors: bg=%06X text=%06X accent=%06X sysBg=%06X sysText=%06X points=%u bgCount=%d textCount=%d took=%lu",
          (unsigned)background, (unsigned)text, (unsigned)accent,
          (unsigned)GetSysColor(COLOR_WINDOW),
          (unsigned)GetSysColor(COLOR_WINDOWTEXT),
          (unsigned)counts.size(), backgroundCount, textCount,
          (unsigned long)(GetTickCount64() - start));
}

// ---------------------------------------------------------------------------
// Folder preview layout and drawing
// ---------------------------------------------------------------------------

static int TotalRows(PanelState* state) {
    if (!state) {
        return 0;
    }
    if (state->viewMode == QlView::Icons) {
        if (state->iconColumns < 1) {
            return 0;
        }
        return (int)((state->filtered.size() + state->iconColumns - 1) /
                     state->iconColumns);
    }
    return (int)state->filtered.size();
}

static int VisibleRows(PanelState* state) {
    if (!state) {
        return 0;
    }
    int height = state->listRect.bottom - state->listRect.top;
    if (state->viewMode == QlView::Details) {
        height -= state->headerRowHeight;
    }
    if (state->viewMode == QlView::Icons) {
        return state->iconItemHeight > 0
                   ? std::max(0, height / state->iconItemHeight)
                   : 0;
    }
    return state->rowHeight > 0 ? std::max(0, height / state->rowHeight) : 0;
}

static void ClampScroll(PanelState* state) {
    if (!state) {
        return;
    }
    int total = TotalRows(state);
    int visible = VisibleRows(state);
    int maximum = std::max(0, total - visible);
    state->scrollRow = std::max(0, std::min(state->scrollRow, maximum));
    state->visibleRows = visible;
}

static void LayoutPanelContent(PanelState* state) {
    if (!state || !state->panel || !IsWindow(state->panel)) {
        return;
    }

    RECT client = {};
    GetClientRect(state->panel, &client);
    UINT dpi = GetDpiForWindowSafe(state->panel);

    int margin = Scale(12, dpi);
    int headerHeight = Scale(46, dpi);
    int footerHeight = Scale(40, dpi);
    state->rowHeight = Scale(24, dpi);
    state->headerRowHeight = Scale(22, dpi);
    state->iconItemWidth = Scale(112, dpi);
    state->iconItemHeight = Scale(104, dpi);

    state->listRect = {margin, headerHeight + Scale(2, dpi),
                       client.right - margin, client.bottom - footerHeight};
    if (state->listRect.bottom < state->listRect.top + Scale(40, dpi)) {
        state->listRect.bottom = state->listRect.top + Scale(40, dpi);
    }

    state->searchRect = {margin, client.bottom - footerHeight + Scale(4, dpi),
                         client.right - margin,
                         client.bottom - Scale(6, dpi)};

    int dropdownWidth = Scale(150, dpi);
    state->viewDropdown = {margin, Scale(9, dpi), margin + dropdownWidth,
                           Scale(9, dpi) + Scale(28, dpi)};

    // Close button: always at the far right of the header.
    int closeSize = Scale(26, dpi);
    state->closeRect = {client.right - Scale(16, dpi) - closeSize,
                        Scale(9, dpi), client.right - Scale(16, dpi),
                        Scale(9, dpi) + closeSize};

    // Text preview toolbar: wrap, line numbers, whitespace and alternating
    // rows, docked to the left of the close button.
    for (RECT& rect : state->textToolbar) {
        rect = {};
    }
    if (state->textPreview) {
        int buttonSize = Scale(26, dpi);
        int buttonGap = Scale(4, dpi);
        int right = state->closeRect.left - Scale(6, dpi);
        for (int i = 3; i >= 0; i--) {
            state->textToolbar[i] = {right - buttonSize, Scale(9, dpi), right,
                                     Scale(9, dpi) + buttonSize};
            right -= buttonSize + buttonGap;
        }
    }

    // The filter box takes the left of the footer; the file / folder counts
    // sit at the right.
    int countsWidth = Scale(172, dpi);
    RECT box = state->searchRect;
    box.right = std::max(box.left + Scale(100, dpi),
                         state->searchRect.right - countsWidth);
    box.right = std::min(box.right, state->searchRect.right);
    state->searchBoxRect = box;
    int boxHeight = box.bottom - box.top;
    int editHeight = Scale(18, dpi);
    int editTop = box.top + (boxHeight - editHeight) / 2;
    // Like the native search box: the text on the left, then a clear button
    // (only drawn while there is text) and the magnifier on the right.
    int glyphArea = Scale(34, dpi);
    int clearWidth = Scale(26, dpi);
    state->searchClearRect = {box.right - glyphArea - clearWidth,
                              box.top + Scale(4, dpi), box.right - glyphArea,
                              box.bottom - Scale(4, dpi)};
    state->searchEditRect = {box.left + Scale(12, dpi), editTop,
                             state->searchClearRect.left - Scale(2, dpi),
                             editTop + editHeight};

    int listWidth = state->listRect.right - state->listRect.left -
                    Scale(8, dpi);
    state->iconColumns =
        std::max(1, listWidth / std::max(1, state->iconItemWidth));

    // A results folder has its own columns; do not capture or scale them.
    UpdateFilterEditVisibility(state);
    if (state->explorerBrowser) {
        SetExplorerBrowserRect(state);
        if (!state->liveResize) {
            RemoveBrowserBorders(state, false);
        }
        if (!state->resultsMode && state->viewMode == QlView::Details &&
            !state->liveResize) {
            if (!state->browserColumnsSaved) {
                SaveBrowserColumns(state);
            }
            ScaleBrowserColumns(state);
        }
    }

    if (state->textPreview) {
        // The wrap depends on the panel width, so it is recomputed here.
        RebuildTextLines(state);
    }
    if (state->mediaHandler) {
        PositionPdfPreview(state);
    }

    ClampScroll(state);
}

// The icon is resolved from the name and the attributes cached by the folder
// enumeration, never from the file itself: this runs while painting, and a
// shell call for a path on an unreachable share would freeze the panel.
static HICON GetSmallIcon(FolderEntry& entry) {
    if (!entry.smallIcon) {
        SHFILEINFOW info = {};
        if (SHGetFileInfoW(entry.path.c_str(), entry.attributes, &info,
                           sizeof(info), SHGFI_ICON | SHGFI_SMALLICON |
                                               SHGFI_USEFILEATTRIBUTES)) {
            entry.smallIcon = info.hIcon;
        }
    }
    return entry.smallIcon;
}

static HICON GetLargeIcon(FolderEntry& entry) {
    if (!entry.largeIcon) {
        SHFILEINFOW info = {};
        if (SHGetFileInfoW(entry.path.c_str(), entry.attributes, &info,
                           sizeof(info), SHGFI_ICON | SHGFI_LARGEICON |
                                               SHGFI_USEFILEATTRIBUTES)) {
            entry.largeIcon = info.hIcon;
        }
    }
    return entry.largeIcon;
}

static std::wstring ViewLabel(PanelState* state) {
    if (!state) {
        return L"Details";
    }
    if (state->viewMode == QlView::Details) {
        return L"Details";
    }
    if (state->viewMode == QlView::List) {
        return L"List";
    }
    if (state->iconSize >= 256) {
        return L"Extra Large Icons";
    }
    if (state->iconSize >= 96) {
        return L"Large Icons";
    }
    if (state->iconSize >= 48) {
        return L"Medium Icons";
    }
    return L"Small Icons";
}

static void DrawViewDropdown(HDC dc, PanelState* state, UINT dpi) {
    if (!state) {
        return;
    }
    const bool win11 = IsWindows11OrLater();
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                                : GetSysColor(COLOR_WINDOWTEXT);

    // Windows 11: soft rounded button. Windows 10: flat square button.
    // Slightly stronger while its menu is open.
    bool pressed = state->viewMenuWnd != nullptr;
    RECT rect = state->viewDropdown;
    HBRUSH fill = CreateSolidBrush(
        BlendColor(background, text, pressed ? 13 : (win11 ? 7 : 8)));
    HPEN border = CreatePen(PS_SOLID, 1,
                            BlendColor(background, text, win11 ? 14 : 30));
    HGDIOBJ oldBrush = SelectObject(dc, fill);
    HGDIOBJ oldPen = SelectObject(dc, border);
    if (win11) {
        RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom,
                  Scale(8, dpi), Scale(8, dpi));
    } else {
        Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    }
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(border);
    DeleteObject(fill);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, text);

    int glyphSize = Scale(16, dpi);
    RECT glyphRect = {rect.left + Scale(10, dpi),
                      (rect.top + rect.bottom - glyphSize) / 2, 0, 0};
    glyphRect.right = glyphRect.left + glyphSize;
    glyphRect.bottom = glyphRect.top + glyphSize;
    DrawViewMenuLineGlyph(dc, glyphRect, state->viewMode, state->iconSize,
                          text, dpi);

    HFONT font = CreateFontW(
        -Scale(win11 ? 13 : 12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, font);

    RECT textRect = rect;
    textRect.left += Scale(34, dpi);
    textRect.right -= Scale(28, dpi);
    DrawTextW(dc, ViewLabel(state).c_str(), -1, &textRect,
              DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

    int cx = rect.right - Scale(15, dpi);
    int cy = (rect.top + rect.bottom) / 2;
    if (win11) {
        // Chevron
        HPEN chevronPen = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)), text);
        HGDIOBJ previousPen = SelectObject(dc, chevronPen);
        MoveToEx(dc, cx - Scale(4, dpi), cy - Scale(2, dpi), nullptr);
        LineTo(dc, cx, cy + Scale(2, dpi));
        LineTo(dc, cx + Scale(5, dpi), cy - Scale(3, dpi));
        SelectObject(dc, previousPen);
        DeleteObject(chevronPen);
    } else {
        // Small solid triangle
        POINT caret[3] = {{cx - Scale(4, dpi), cy - Scale(2, dpi)},
                          {cx + Scale(4, dpi), cy - Scale(2, dpi)},
                          {cx, cy + Scale(3, dpi)}};
        HBRUSH caretBrush = CreateSolidBrush(text);
        HGDIOBJ previousBrush = SelectObject(dc, caretBrush);
        HPEN caretPen = CreatePen(PS_SOLID, 1, text);
        HGDIOBJ previousPen = SelectObject(dc, caretPen);
        Polygon(dc, caret, 3);
        SelectObject(dc, previousPen);
        SelectObject(dc, previousBrush);
        DeleteObject(caretPen);
        DeleteObject(caretBrush);
    }

    SelectObject(dc, oldFont);
    DeleteObject(font);
}

static void DrawScrollbar(HDC dc, PanelState* state, UINT dpi) {
    state->scrollbarRect = {};
    state->scrollThumbRect = {};
    if (!state) {
        return;
    }

    int total = TotalRows(state);
    int visible = VisibleRows(state);
    if (total <= visible || state->listRect.right <= state->listRect.left) {
        return;
    }

    int width = Scale(8, dpi);
    RECT track = {state->listRect.right - width, state->listRect.top,
                  state->listRect.right, state->listRect.bottom};
    int trackHeight = track.bottom - track.top;
    int thumbHeight = std::max(Scale(20, dpi),
                               trackHeight * visible / std::max(1, total));
    int maximum = std::max(1, total - visible);
    int thumbTop =
        track.top + (trackHeight - thumbHeight) * state->scrollRow / maximum;

    RECT thumb = {track.left, thumbTop, track.right, thumbTop + thumbHeight};
    state->scrollbarRect = track;
    state->scrollThumbRect = thumb;

    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                                : GetSysColor(COLOR_WINDOWTEXT);
    HBRUSH trackBrush =
        CreateSolidBrush(BlendColor(background, text, 10));
    FillRect(dc, &track, trackBrush);
    DeleteObject(trackBrush);

    HBRUSH thumbBrush =
        CreateSolidBrush(BlendColor(background, text, 35));
    FillRect(dc, &thumb, thumbBrush);
    DeleteObject(thumbBrush);
}

static void DrawCountsGlyph(HDC dc, const RECT& rect, bool isFolder,
                            COLORREF color) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int size = std::min(width, height);
    if (size < 6) {
        return;
    }

    int thickness = std::max(1, size / 14);
    HPEN pen = CreatePen(PS_SOLID, thickness, color);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));

    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    int left = cx - size / 2 + thickness;
    int top = cy - size / 2 + thickness;
    int right = cx + size / 2 - thickness;
    int bottom = cy + size / 2 - thickness;

    if (isFolder) {
        int tabWidth = (right - left) * 5 / 12;
        int tabDrop = std::max(2, size / 5);
        POINT points[6] = {
            {left, top + tabDrop},
            {left + tabWidth, top + tabDrop},
            {left + tabWidth + tabDrop, top},
            {right, top},
            {right, bottom},
            {left, bottom},
        };
        Polygon(dc, points, 6);
    } else {
        int fold = std::max(3, size / 4);
        POINT points[5] = {
            {left, top},
            {right - fold, top},
            {right, top + fold},
            {right, bottom},
            {left, bottom},
        };
        Polygon(dc, points, 5);
        MoveToEx(dc, right - fold, top, nullptr);
        LineTo(dc, right - fold, top + fold);
        LineTo(dc, right, top + fold);
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

// The name filter box: rounded with an accent underline when focused on
// Windows 11, flat with an accent border on Windows 10.
static void DrawFilterBox(HDC dc, PanelState* state, UINT dpi) {
    if (!state || !state->filterEdit ||
        state->searchBoxRect.right <= state->searchBoxRect.left) {
        return;
    }
    const bool win11 = IsWindows11OrLater();
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                                : GetSysColor(COLOR_WINDOWTEXT);
    COLORREF accent = state->accent != CLR_INVALID
                          ? state->accent
                          : BlendColor(background, text, 75);
    bool focused = GetFocus() == state->filterEdit;
    RECT box = state->searchBoxRect;

    HBRUSH fill = CreateSolidBrush(FilterBoxFill(state));
    COLORREF borderColor =
        win11 ? BlendColor(background, text, 14)
              : (focused ? accent : BlendColor(background, text, 30));
    HPEN border = CreatePen(PS_SOLID, 1, borderColor);
    HGDIOBJ oldBrush = SelectObject(dc, fill);
    HGDIOBJ oldPen = SelectObject(dc, border);
    if (win11) {
        RoundRect(dc, box.left, box.top, box.right, box.bottom, Scale(8, dpi),
                  Scale(8, dpi));
    } else {
        Rectangle(dc, box.left, box.top, box.right, box.bottom);
    }
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(border);
    DeleteObject(fill);

    if (win11 && focused) {
        HBRUSH line = CreateSolidBrush(
            GetUserAccentFocusColor(ColorLuma(background) < 128));
        RECT underline = {box.left + Scale(4, dpi),
                          box.bottom - Scale(2, dpi),
                          box.right - Scale(4, dpi), box.bottom - 1};
        FillRect(dc, &underline, line);
        DeleteObject(line);
    }

    // Clear button, only while the filter has text.
    if (!state->filter.empty()) {
        const RECT& clear = state->searchClearRect;
        int ccx = (clear.left + clear.right) / 2;
        int ccy = (clear.top + clear.bottom) / 2;
        int arm = Scale(4, dpi);
        HPEN crossPen = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)),
                                  BlendColor(background, text, 80));
        HGDIOBJ prevPen = SelectObject(dc, crossPen);
        MoveToEx(dc, ccx - arm, ccy - arm, nullptr);
        LineTo(dc, ccx + arm + 1, ccy + arm + 1);
        MoveToEx(dc, ccx + arm, ccy - arm, nullptr);
        LineTo(dc, ccx - arm - 1, ccy + arm + 1);
        SelectObject(dc, prevPen);
        DeleteObject(crossPen);
    }

    // Magnifier, at the right edge like the native search box.
    int cx = box.right - Scale(19, dpi);
    int cy = (box.top + box.bottom) / 2;
    HPEN glyphPen = CreatePen(PS_SOLID, std::max(1, Scale(1, dpi)),
                              BlendColor(background, text, 80));
    oldPen = SelectObject(dc, glyphPen);
    oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Ellipse(dc, cx - Scale(5, dpi), cy - Scale(6, dpi), cx + Scale(3, dpi),
            cy + Scale(2, dpi));
    MoveToEx(dc, cx + Scale(2, dpi), cy + Scale(1, dpi), nullptr);
    LineTo(dc, cx + Scale(6, dpi), cy + Scale(5, dpi));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(glyphPen);
}

// Footer: the name filter text field and the file / folder counts.
static void DrawFooter(HDC dc, PanelState* state, UINT dpi) {
    if (!state) {
        return;
    }
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                                : GetSysColor(COLOR_WINDOWTEXT);
    COLORREF dim = BlendColor(background, text, 55);
    (void)background;
    DrawFilterBox(dc, state, dpi);
    RECT footer = state->searchRect;
    std::wstring filesText = Fmt(L"%d files", state->fileCount);
    std::wstring foldersText = Fmt(L"%d folders", state->folderCount);

    HFONT font = CreateFontW(
        -Scale(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, dim);

    SIZE filesSize = {};
    SIZE foldersSize = {};
    GetTextExtentPoint32W(dc, filesText.c_str(),
                          static_cast<int>(filesText.size()),
                          &filesSize);
    GetTextExtentPoint32W(dc, foldersText.c_str(),
                          static_cast<int>(foldersText.size()),
                          &foldersSize);

    int iconSize = Scale(16, dpi);
    int gap = Scale(4, dpi);
    int groupGap = Scale(12, dpi);
    int totalWidth = iconSize + gap + filesSize.cx + groupGap +
                     iconSize + gap + foldersSize.cx;
    int x = std::max(footer.left, footer.right - totalWidth);
    if (state->filterEdit) {
        x = std::max(x, static_cast<int>(state->searchBoxRect.right) +
                            Scale(8, dpi));
    }
    int cy = (footer.top + footer.bottom) / 2;

    RECT fileGlyph = {x, cy - iconSize / 2, x + iconSize,
                      cy + iconSize / 2};
    DrawCountsGlyph(dc, fileGlyph, false, dim);
    x += iconSize + gap;
    RECT filesRect = {x, footer.top, x + filesSize.cx, footer.bottom};
    DrawTextW(dc, filesText.c_str(), -1, &filesRect,
              DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    x += filesSize.cx + groupGap;

    RECT folderGlyph = {x, cy - iconSize / 2, x + iconSize,
                        cy + iconSize / 2};
    DrawCountsGlyph(dc, folderGlyph, true, dim);
    x += iconSize + gap;
    RECT foldersRect = {x, footer.top, x + foldersSize.cx, footer.bottom};
    DrawTextW(dc, foldersText.c_str(), -1, &foldersRect,
              DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

    SelectObject(dc, oldFont);
    DeleteObject(font);
}

static void DrawFolderContent(HDC dc, PanelState* state, UINT dpi) {
    if (!state) {
        return;
    }

    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                                : GetSysColor(COLOR_WINDOWTEXT);
    COLORREF dim = BlendColor(background, text, 55);

    if (state->explorerBrowser) {
        // An archive has no counts to show: its entries are not filesystem
        // children of the path.
        if (!state->archivePreview) {
            DrawFooter(dc, state, dpi);
        }
        return;
    }

    DrawScrollbar(dc, state, dpi);
    int contentRight =
        state->scrollbarRect.right ? state->scrollbarRect.left
                                   : state->listRect.right;

    int saved = SaveDC(dc);
    IntersectClipRect(dc, state->listRect.left, state->listRect.top,
                      state->listRect.right, state->listRect.bottom);
    SetBkMode(dc, TRANSPARENT);

    int headerHeight =
        state->viewMode == QlView::Details ? state->headerRowHeight : 0;
    int rowHeight = state->rowHeight;
    int listHeight =
        state->listRect.bottom - state->listRect.top - headerHeight;

    if (state->filtered.empty()) {
        SetTextColor(dc, dim);
        RECT empty = state->listRect;
        DrawTextW(dc,
                  state->filter.empty() ? L"This folder is empty"
                                        : L"No matching items",
                  -1, &empty,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    } else if (state->viewMode == QlView::Details) {
        int available = contentRight - state->listRect.left;
        int nameWidth = available * 48 / 100;
        int dateWidth = available * 22 / 100;
        int typeWidth = available * 18 / 100;
        int sizeWidth = available - nameWidth - dateWidth - typeWidth;
        if (available < Scale(300, dpi)) {
            typeWidth = 0;
            sizeWidth = 0;
            nameWidth = available - dateWidth;
        }
        if (available < Scale(220, dpi)) {
            dateWidth = 0;
            nameWidth = available;
        }

        RECT header = {state->listRect.left, state->listRect.top,
                       contentRight, state->listRect.top + headerHeight};
        SetTextColor(dc, dim);
        RECT hName = {header.left + Scale(24, dpi), header.top,
                      header.left + nameWidth, header.bottom};
        DrawTextW(dc, L"Name", -1, &hName,
                  DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        if (dateWidth > 0) {
            RECT hDate = {header.left + nameWidth, header.top,
                          header.left + nameWidth + dateWidth, header.bottom};
            DrawTextW(dc, L"Date modified", -1, &hDate,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                          DT_NOPREFIX);
        }
        if (typeWidth > 0) {
            RECT hType = {header.left + nameWidth + dateWidth, header.top,
                          header.left + nameWidth + dateWidth + typeWidth,
                          header.bottom};
            DrawTextW(dc, L"Type", -1, &hType,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                          DT_NOPREFIX);
        }
        if (sizeWidth > 0) {
            RECT hSize = {header.left + nameWidth + dateWidth + typeWidth,
                          header.top, contentRight, header.bottom};
            DrawTextW(dc, L"Size", -1, &hSize,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_RIGHT |
                          DT_NOPREFIX);
        }

        HPEN separator = CreatePen(PS_SOLID, 1, BlendColor(background, text, 18));
        HGDIOBJ oldPen = SelectObject(dc, separator);
        MoveToEx(dc, header.left, header.bottom - 1, nullptr);
        LineTo(dc, contentRight, header.bottom - 1);
        SelectObject(dc, oldPen);
        DeleteObject(separator);

        int visible = std::max(0, listHeight / rowHeight);
        for (int row = 0; row < visible; row++) {
            int index = state->scrollRow + row;
            if (index >= (int)state->filtered.size()) {
                break;
            }
            int entryIndex = state->filtered[index];
            FolderEntry& entry = state->entries[entryIndex];
            int top = state->listRect.top + headerHeight + row * rowHeight;
            RECT rowRect = {state->listRect.left, top, contentRight,
                            top + rowHeight};

            if (index == state->hoverIndex) {
                HBRUSH hover = CreateSolidBrush(BlendColor(background, text, 8));
                FillRect(dc, &rowRect, hover);
                DeleteObject(hover);
            }

            if (HICON rowIcon = GetSmallIcon(entry)) {
                DrawIconEx(dc, rowRect.left + Scale(4, dpi),
                           top + (rowHeight - Scale(16, dpi)) / 2,
                           rowIcon, Scale(16, dpi), Scale(16, dpi), 0,
                           nullptr, DI_NORMAL);
            }

            SetTextColor(dc, text);
            RECT nameRect = {rowRect.left + Scale(24, dpi), top,
                             rowRect.left + nameWidth, top + rowHeight};
            DrawTextW(dc, entry.name.c_str(), -1, &nameRect,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                          DT_NOPREFIX);

            SetTextColor(dc, dim);
            if (dateWidth > 0) {
                RECT dateRect = {rowRect.left + nameWidth, top,
                                 rowRect.left + nameWidth + dateWidth,
                                 top + rowHeight};
                std::wstring date = FormatEntryDate(entry.modified);
                DrawTextW(dc, date.c_str(), -1, &dateRect,
                          DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                              DT_NOPREFIX);
            }
            if (typeWidth > 0) {
                RECT typeRect = {rowRect.left + nameWidth + dateWidth, top,
                                 rowRect.left + nameWidth + dateWidth +
                                     typeWidth,
                                 top + rowHeight};
                DrawTextW(dc, entry.type.c_str(), -1, &typeRect,
                          DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                              DT_NOPREFIX);
            }
            if (sizeWidth > 0) {
                RECT sizeRect = {rowRect.left + nameWidth + dateWidth +
                                     typeWidth,
                                 top, contentRight, top + rowHeight};
                std::wstring size =
                    entry.isFolder ? L"" : FormatEntrySize(entry.size);
                DrawTextW(dc, size.c_str(), -1, &sizeRect,
                          DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                              DT_RIGHT | DT_NOPREFIX);
            }
        }
    } else if (state->viewMode == QlView::List) {
        int visible = std::max(0, listHeight / rowHeight);
        for (int row = 0; row < visible; row++) {
            int index = state->scrollRow + row;
            if (index >= (int)state->filtered.size()) {
                break;
            }
            FolderEntry& entry = state->entries[state->filtered[index]];
            int top = state->listRect.top + row * rowHeight;
            RECT rowRect = {state->listRect.left, top, contentRight,
                            top + rowHeight};
            if (index == state->hoverIndex) {
                HBRUSH hover = CreateSolidBrush(BlendColor(background, text, 8));
                FillRect(dc, &rowRect, hover);
                DeleteObject(hover);
            }
            if (HICON rowIcon = GetSmallIcon(entry)) {
                DrawIconEx(dc, rowRect.left + Scale(4, dpi),
                           top + (rowHeight - Scale(16, dpi)) / 2,
                           rowIcon, Scale(16, dpi), Scale(16, dpi), 0,
                           nullptr, DI_NORMAL);
            }
            SetTextColor(dc, text);
            RECT nameRect = {rowRect.left + Scale(24, dpi), top, contentRight,
                             top + rowHeight};
            DrawTextW(dc, entry.name.c_str(), -1, &nameRect,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                          DT_NOPREFIX);
        }
    } else {
        int itemWidth = state->iconItemWidth;
        int itemHeight = state->iconItemHeight;
        int columns = state->iconColumns;
        int visible = std::max(0, listHeight / itemHeight);
        for (int row = 0; row < visible; row++) {
            for (int col = 0; col < columns; col++) {
                int index = (state->scrollRow + row) * columns + col;
                if (index >= (int)state->filtered.size()) {
                    break;
                }
                FolderEntry& entry = state->entries[state->filtered[index]];
                int left = state->listRect.left + col * itemWidth;
                int top = state->listRect.top + row * itemHeight;
                RECT itemRect = {left, top, left + itemWidth, top + itemHeight};
                if (index == state->hoverIndex) {
                    HBRUSH hover =
                        CreateSolidBrush(BlendColor(background, text, 8));
                    FillRect(dc, &itemRect, hover);
                    DeleteObject(hover);
                }

                HICON icon = GetLargeIcon(entry);
                if (icon) {
                    DrawIconEx(dc, left + (itemWidth - Scale(48, dpi)) / 2,
                               top + Scale(8, dpi), icon, Scale(48, dpi),
                               Scale(48, dpi), 0, nullptr, DI_NORMAL);
                }

                SetTextColor(dc, text);
                RECT nameRect = {left + Scale(4, dpi), top + Scale(60, dpi),
                                 left + itemWidth - Scale(4, dpi),
                                 top + itemHeight - Scale(4, dpi)};
                DrawTextW(dc, entry.name.c_str(), -1, &nameRect,
                          DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS |
                              DT_NOPREFIX);
            }
        }
    }

    RestoreDC(dc, saved);

    DrawFooter(dc, state, dpi);
}

// ---------------------------------------------------------------------------
// Panel rendering
// ---------------------------------------------------------------------------

static void DrawPanel(HDC dc, const RECT& client, PanelState* state) {
    UINT dpi = GetDpiForWindowSafe(state && state->panel ? state->panel
                                                         : nullptr);

    COLORREF background =
        state && state->background != CLR_INVALID ? state->background
                                                   : GetSysColor(COLOR_WINDOW);
    COLORREF text = state && state->text != CLR_INVALID
                        ? state->text
                        : GetSysColor(COLOR_WINDOWTEXT);

    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(dc, &client, backgroundBrush);
    DeleteObject(backgroundBrush);

    RECT header = client;
    header.bottom = client.top + Scale(46, dpi);
    HBRUSH headerBrush = CreateSolidBrush(background);
    FillRect(dc, &header, headerBrush);
    DeleteObject(headerBrush);

    RECT separator = header;
    separator.top = separator.bottom - 1;
    HBRUSH separatorBrush =
        CreateSolidBrush(BlendColor(background, text, 18));
    FillRect(dc, &separator, separatorBrush);
    DeleteObject(separatorBrush);

    SetBkMode(dc, TRANSPARENT);

    HFONT titleFont = CreateFontW(
        -Scale(15, dpi), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT bodyFont = CreateFontW(
        -Scale(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT footerFont = CreateFontW(
        -Scale(12, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    SetTextColor(dc, text);
    HGDIOBJ previousFont = SelectObject(dc, titleFont);
    if (state && state->folderPreview) {
        DrawViewDropdown(dc, state, dpi);
    } else {
        RECT titleRect = header;
        titleRect.left += Scale(16, dpi);
        titleRect.right -= Scale(16, dpi);
        DrawTextW(dc, L"Quick Look", -1, &titleRect,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    }
    if (state && state->textPreview) {
        COLORREF accent = state->accent != CLR_INVALID
                              ? state->accent
                              : BlendColor(background, text, 75);
        DrawTextToolbar(dc, state, dpi, background, text, accent);
    }
    if (state) {
        DrawCloseButton(dc, state, dpi, background, text);
    }

    std::wstring fileName = state ? state->path : L"";
    size_t slash = fileName.find_last_of(L'\\');
    if (slash != std::wstring::npos) {
        fileName = fileName.substr(slash + 1);
    }
    if (fileName.empty()) {
        fileName = L"No selection";
    }
    SetTextColor(dc, BlendColor(background, text, 65));
    SelectObject(dc, bodyFont);
    RECT nameRect = header;
    nameRect.left += Scale((state && state->folderPreview) ? 170 : 120, dpi);
    if (state && state->closeRect.right > state->closeRect.left) {
        nameRect.right = state->closeRect.left - Scale(8, dpi);
    } else {
        nameRect.right -= Scale(16, dpi);
    }
    if (state && state->textPreview &&
        state->textToolbar[0].right > state->textToolbar[0].left) {
        nameRect.right = state->textToolbar[0].left - Scale(8, dpi);
    }
    DrawTextW(dc, fileName.c_str(), -1, &nameRect,
              DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS |
                  DT_RIGHT);

    if (state && state->folderPreview) {
        DrawFolderContent(dc, state, dpi);
        SelectObject(dc, previousFont);
        DeleteObject(titleFont);
        DeleteObject(bodyFont);
        DeleteObject(footerFont);
        return;
    }

    RECT body = client;
    body.left += Scale(16, dpi);
    body.right -= Scale(16, dpi);
    body.top = header.bottom + Scale(12, dpi);
    body.bottom = client.bottom - Scale(42, dpi);

    SetTextColor(dc, text);
    SelectObject(dc, bodyFont);

    if (state && (state->textLoading ||
                  (state->loadPending && state->loadSlowCleared))) {
        SelectObject(dc, bodyFont);
        DrawLoadingIndicator(dc, state, dpi, body);
    } else if (state->textPreview) {
        DrawTextPreview(dc, state, dpi, body);
    } else if (state->mediaPreview) {
        DrawMediaPreview(dc, state, dpi, body);
    } else {
        DrawPropertiesPreview(dc, state, dpi, body);
    }

    RECT footer = client;
    footer.top = client.bottom - Scale(40, dpi);
    footer.left += Scale(16, dpi);
    footer.right -= Scale(16, dpi);
    SetTextColor(dc, BlendColor(background, text, 60));
    SelectObject(dc, footerFont);
    if (state && state->textPreview) {
        DrawFilterBox(dc, state, dpi);
        std::wstring info;
        if (state->filter.empty()) {
            info = Fmt(L"%s   %u lines   %s%s",
                       FormatEntrySize(state->textBytes).c_str(),
                       static_cast<unsigned>(state->textSourceLines),
                       state->textEncoding.c_str(),
                       state->textTruncated ? L"   truncated at 200 KB" : L"");
        } else {
            info = Fmt(L"%u matches   %u lines   %s",
                       static_cast<unsigned>(state->textMatchCount),
                       static_cast<unsigned>(state->textSourceLines),
                       state->textEncoding.c_str());
        }
        RECT infoRect = footer;
        if (state->filterEdit) {
            infoRect.left = std::max<LONG>(
                infoRect.left,
                static_cast<LONG>(state->searchBoxRect.right) +
                    Scale(8, dpi));
        }
        DrawTextW(dc, info.c_str(), -1, &infoRect,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS |
                      DT_RIGHT);
    } else if (state && state->mediaPreview) {
        std::wstring info = state->mediaKind;
        int shownWidth = state->mediaSourceWidth > 0 ? state->mediaSourceWidth
                                                      : state->mediaWidth;
        int shownHeight = state->mediaSourceHeight > 0 ? state->mediaSourceHeight
                                                       : state->mediaHeight;
        if (shownWidth > 0 && shownHeight > 0) {
            info += Fmt(L"   %d x %d", shownWidth, shownHeight);
        }
        info += L"   " + FormatEntrySize(state->mediaBytes);
        DrawTextW(dc, info.c_str(), -1, &footer,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS |
                      DT_RIGHT);
    }

    SelectObject(dc, previousFont);
    DeleteObject(titleFont);
    DeleteObject(bodyFont);
    DeleteObject(footerFont);
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------

static void CloseState(PanelState* state);

// ---------------------------------------------------------------------------
// Details columns
// ---------------------------------------------------------------------------

static void ReleaseColumns(PanelState* state) {
    if (!state) {
        return;
    }
    ReleaseCom(state->columnManager);
    state->columnKeys.clear();
    state->columnWidths.clear();
    state->columnsSaved = false;
}

static bool SaveColumns(PanelState* state, IShellView* shellView) {
    if (!state || !shellView) {
        return false;
    }
    ULONGLONG start = GetTickCount64();

    IFolderView2* folderView = nullptr;
    if (FAILED(shellView->QueryInterface(IID_IFolderView2,
                                         (void**)&folderView)) ||
        !folderView) {
        Wh_Log(L"columns: no IFolderView2");
        return false;
    }

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    bool details = SUCCEEDED(folderView->GetViewModeAndIconSize(
                        &viewMode, &iconSize)) &&
                   viewMode == FVM_DETAILS;
    ReleaseCom(folderView);

    if (!details) {
        Wh_Log(L"columns: view is not Details");
        return false;
    }

    IColumnManager* columnManager = nullptr;
    if (FAILED(shellView->QueryInterface(IID_IColumnManager,
                                         (void**)&columnManager)) ||
        !columnManager) {
        Wh_Log(L"columns: no IColumnManager");
        return false;
    }

    UINT count = 0;
    if (FAILED(columnManager->GetColumnCount(CM_ENUM_VISIBLE, &count)) ||
        count == 0) {
        Wh_Log(L"columns: no visible columns");
        ReleaseCom(columnManager);
        return false;
    }

    std::vector<PROPERTYKEY> keys(count);
    if (FAILED(columnManager->GetColumns(CM_ENUM_VISIBLE, keys.data(),
                                        count))) {
        Wh_Log(L"columns: GetColumns failed");
        ReleaseCom(columnManager);
        return false;
    }

    std::vector<UINT> widths(count, 0);
    UINT validWidths = 0;
    UINT dpi = GetDpiForWindowSafe(state->view);
    for (UINT i = 0; i < count; i++) {
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        if (SUCCEEDED(columnManager->GetColumnInfo(keys[i], &info))) {
            UINT width = info.uWidth;
            if (width == static_cast<UINT>(CM_WIDTH_AUTOSIZE) || width == 0) {
                if (info.uIdealWidth > 0) {
                    width = info.uIdealWidth;
                } else if (info.uDefaultWidth > 0) {
                    width = info.uDefaultWidth;
                } else {
                    width = static_cast<UINT>(Scale(120, dpi));
                }
            }
            widths[i] = width;
            validWidths++;
        }
    }

    if (!validWidths) {
        Wh_Log(L"columns: no usable widths");
        ReleaseCom(columnManager);
        return false;
    }

    ReleaseColumns(state);
    state->columnManager = columnManager;
    state->columnKeys = std::move(keys);
    state->columnWidths = std::move(widths);
    state->columnsSaved = true;

    Wh_Log(L"columns saved: count=%u valid=%u took=%lu", count, validWidths,
          (unsigned long)(GetTickCount64() - start));
    return true;
}

static void ScaleColumns(PanelState* state, int targetWidth, UINT dpi) {
    if (!state || !state->columnsSaved || !state->columnManager ||
        state->columnKeys.empty() || targetWidth <= 0) {
        return;
    }
    ULONGLONG start = GetTickCount64();

    UINT count = static_cast<UINT>(state->columnKeys.size());
    long long total = 0;
    for (UINT width : state->columnWidths) {
        total += width;
    }
    if (total <= 0) {
        return;
    }

    int margin = Scale(8, dpi);
    int usable = targetWidth - margin;
    if (usable < Scale(120, dpi)) {
        return;
    }

    double scale = static_cast<double>(usable) / static_cast<double>(total);
    if (scale > 1.0) {
        scale = 1.0;
    }

    int minWidth = Scale(40, dpi);
    for (UINT i = 0; i < count; i++) {
        int width = static_cast<int>(state->columnWidths[i] * scale);
        width = std::max(width, minWidth);
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        info.uWidth = static_cast<UINT>(width);
        state->columnManager->SetColumnInfo(state->columnKeys[i], &info);
    }

    Wh_Log(L"columns scaled: count=%u total=%lld target=%d usable=%d scale=%.3f took=%lu",
          count, total, targetWidth, usable, scale,
          (unsigned long)(GetTickCount64() - start));
}

static void RestoreColumns(PanelState* state) {
    if (!state || !state->columnsSaved || !state->columnManager) {
        return;
    }
    ULONGLONG start = GetTickCount64();

    UINT count = static_cast<UINT>(state->columnKeys.size());
    for (UINT i = 0; i < count; i++) {
        CM_COLUMNINFO info = {};
        info.cbSize = sizeof(info);
        info.dwMask = CM_MASK_WIDTH;
        info.uWidth = state->columnWidths[i];
        state->columnManager->SetColumnInfo(state->columnKeys[i], &info);
    }

    Wh_Log(L"columns restored: count=%u took=%lu", count,
          (unsigned long)(GetTickCount64() - start));
}

static void LayoutState(PanelState* state) {
    if (!state || !state->open || !state->panel || !state->view ||
        !state->parent) {
        return;
    }
    if (!IsWindow(state->panel) || !IsWindow(state->view) ||
        !IsWindow(state->parent)) {
        return;
    }

    UINT dpi = GetDpiForWindowSafe(state->panel);
    RECT client = {};
    GetClientRect(state->parent, &client);

    int deltaWidth = client.right - state->parentClient.right;
    int deltaHeight = client.bottom - state->parentClient.bottom;
    if (deltaWidth || deltaHeight) {
        state->viewFull.right += deltaWidth;
        state->viewFull.bottom += deltaHeight;
        state->parentClient = client;
    }

    state->viewFull.left = std::max(0L, state->viewFull.left);
    state->viewFull.top = std::max(0L, state->viewFull.top);
    state->viewFull.right = std::min(client.right, state->viewFull.right);
    state->viewFull.bottom = std::min(client.bottom, state->viewFull.bottom);

    int viewWidth = state->viewFull.right - state->viewFull.left;
    int viewHeight = state->viewFull.bottom - state->viewFull.top;
    if (viewWidth < 1 || viewHeight < 1) {
        return;
    }

    RECT panel = {};
    int newViewWidth = viewWidth;
    int newViewHeight = viewHeight;
    bool reflow = g_reflowColumns.load();

    if (viewWidth < Scale(760, dpi)) {
        int panelHeight = std::min(Scale(380, dpi), viewHeight / 2);
        panelHeight = std::max(panelHeight, Scale(180, dpi));
        if (reflow) {
            newViewHeight = std::max(1, viewHeight - panelHeight);
        }
        panel = {state->viewFull.left, state->viewFull.top + viewHeight -
                                           panelHeight,
                 state->viewFull.right, state->viewFull.bottom};
        state->dockRight = false;
    } else {
        int panelWidth =
            std::min(Scale(520, dpi), std::max(Scale(320, dpi), viewWidth / 2));
        int userWidthDip = g_panelWidthDip.load();
        if (userWidthDip > 0) {
            panelWidth = Scale(userWidthDip, dpi);
        }
        // Keep the panel usable, and leave room for the file list itself.
        int minimumPanel = Scale(300, dpi);
        int maximumPanel = std::max(minimumPanel, viewWidth - Scale(260, dpi));
        panelWidth = std::min(std::max(panelWidth, minimumPanel), maximumPanel);
        state->dockRight = true;
        if (reflow) {
            newViewWidth = std::max(1, viewWidth - panelWidth);
        }
        panel = {state->viewFull.left + viewWidth - panelWidth,
                 state->viewFull.top, state->viewFull.right,
                 state->viewFull.bottom};
    }

    // While the panel edge is being dragged, the expensive relayouts (Explorer's
    // own file list and the embedded browser) run at most every ~40 ms. In
    // between, only the panel itself moves, which is cheap, and a short timer
    // guarantees a final up-to-date layout if the mouse pauses.
    if (state->liveResize) {
        ULONGLONG now = GetTickCount64();
        if (now - state->lastLiveLayout < 40) {
            SetWindowPos(state->panel, nullptr, panel.left, panel.top,
                         panel.right - panel.left, panel.bottom - panel.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            SetTimer(state->panel, QL_TIMER_LIVE_LAYOUT, 45, nullptr);
            return;
        }
        state->lastLiveLayout = now;
    }

    if (reflow) {
        SetWindowPos(state->view, nullptr, state->viewFull.left,
                     state->viewFull.top, newViewWidth, newViewHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        if (!state->liveResize) {
            ScaleColumns(state, newViewWidth, dpi);
        }
    } else {
        if (state->columnsSaved) {
            RestoreColumns(state);
            ReleaseColumns(state);
        }
        SetWindowPos(state->view, nullptr, state->viewFull.left,
                     state->viewFull.top, viewWidth, viewHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    SetWindowPos(state->panel, HWND_TOP, panel.left, panel.top,
                 panel.right - panel.left, panel.bottom - panel.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    LayoutPanelContent(state);
    if (!state->liveResize)
        Wh_Log(L"layout: view=%p parent=%p client=%ld,%ld viewFull=%ld,%ld,%ld,%ld panel=%ld,%ld,%ld,%ld view=%dx%d",
          (void*)state->view, (void*)state->parent, client.right, client.bottom,
          state->viewFull.left, state->viewFull.top, state->viewFull.right,
          state->viewFull.bottom, panel.left, panel.top, panel.right,
          panel.bottom, newViewWidth, newViewHeight);
    InvalidateRect(state->panel, nullptr, TRUE);
}

static void ReleaseBackBuffer(PanelState* state) {
    if (!state) {
        return;
    }
    if (state->backDc) {
        if (state->backOldBitmap) {
            SelectObject(state->backDc, state->backOldBitmap);
        }
        if (state->backBitmap) {
            DeleteObject(state->backBitmap);
        }
        DeleteDC(state->backDc);
    }
    state->backDc = nullptr;
    state->backBitmap = nullptr;
    state->backOldBitmap = nullptr;
    state->backWidth = 0;
    state->backHeight = 0;
}

// Grows or reuses the back buffer. Drawing the whole panel off-screen and
// blitting it in one BitBlt is what removes the flicker while scrolling: the
// window never shows the background fill before the rows.
static bool EnsureBackBuffer(PanelState* state, HDC reference, int width,
                             int height) {
    if (!state || width <= 0 || height <= 0) {
        return false;
    }
    if (state->backDc && state->backBitmap && state->backWidth == width &&
        state->backHeight == height) {
        return true;
    }

    ReleaseBackBuffer(state);
    state->backDc = CreateCompatibleDC(reference);
    if (!state->backDc) {
        return false;
    }
    state->backBitmap = CreateCompatibleBitmap(reference, width, height);
    if (!state->backBitmap) {
        ReleaseBackBuffer(state);
        return false;
    }
    state->backOldBitmap = SelectObject(state->backDc, state->backBitmap);
    state->backWidth = width;
    state->backHeight = height;
    return true;
}

static void CloseState(PanelState* state) {
    if (!state) {
        return;
    }
    state->open = false;
    DisconnectNavigationEvents(state);
    state->pendingOpenPath.clear();
    state->loadGeneration++;
    if (state->loadToken) {
        state->loadToken->store(state->loadGeneration);
    }
    state->loadPending = false;
    state->probeStarted = false;
    state->textLoading = false;
    ClearMediaPreview(state);
    ReleaseBackBuffer(state);
    CloseViewMenu(state);
    if (state->view && IsWindow(state->view)) {
        SetWindowPos(state->view, nullptr, state->viewFull.left,
                     state->viewFull.top,
                     state->viewFull.right - state->viewFull.left,
                     state->viewFull.bottom - state->viewFull.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        RestoreColumns(state);
    }
    RestoreBrowserColumns(state);
    ReleaseColumns(state);
    if (state->panel && IsWindow(state->panel)) {
        KillTimer(state->panel, QL_TIMER_SELECTION);
        KillTimer(state->panel, QL_TIMER_BROWSER_COLUMNS);
        KillTimer(state->panel, QL_TIMER_FILTER);
        KillTimer(state->panel, QL_TIMER_BROWSER_THEME);
        KillTimer(state->panel, QL_TIMER_COVER);
        KillTimer(state->panel, QL_TIMER_PROBE);
        KillTimer(state->panel, QL_TIMER_SPINNER);
        KillTimer(state->panel, QL_TIMER_RESULTS);
        state->coverActive = false;
        ShowWindow(state->panel, SW_HIDE);
    }
    if (state->view && IsWindow(state->view)) {
        SetFocus(state->view);
    }
    UpdateTopOpenFlag(state->top);
    Wh_Log(L"panel closed for tab=%p view=%p", (void*)state->tab,
           (void*)state->view);
}

// ---------------------------------------------------------------------------
// Navigation events of the Explorer tab
// ---------------------------------------------------------------------------
//
// With "Shrink columns to fit the panel" the real folder view carries the
// shrunk column widths while the panel is open, and Explorer saves a folder's
// widths when it leaves the folder. So the panel listens to the tab's
// DWebBrowserEvents2::BeforeNavigate2 and closes (which restores the columns)
// before the navigation starts.

static const IID kDiidWebBrowserEvents2 = {
    0x34A715A0, 0x6587, 0x11D0, {0x92, 0x4A, 0x00, 0x20, 0xAF, 0xC7, 0xAC, 0x4D}};
static const DISPID kDispBeforeNavigate2 = 250;
// Fired when the tab's browser object quits or its window is about to close:
// closing a single tab does not go through the top-level window's WM_CLOSE.
static const DISPID kDispOnQuit = 253;
static const DISPID kDispWindowClosing = 263;

class NavigationEvents final : public IDispatch {
   public:
    explicit NavigationEvents(PanelState* state) : state_(state) {}

    void Detach() { state_ = nullptr; }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDispatch ||
            riid == kDiidWebBrowserEvents2) {
            *ppvObject = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&ref_));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG value = InterlockedDecrement(&ref_);
        if (!value) {
            delete this;
        }
        return static_cast<ULONG>(value);
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (count) {
            *count = 0;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID,
                                            DISPID*) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID, WORD,
                                     DISPPARAMS*, VARIANT*, EXCEPINFO*,
                                     UINT*) override {
        if ((dispIdMember == kDispBeforeNavigate2 || dispIdMember == kDispOnQuit ||
             dispIdMember == kDispWindowClosing) &&
            state_ && state_->open) {
            PanelState* state = state_;
            // CloseState disconnects this sink; keep it alive until we return.
            AddRef();
            Wh_Log(L"navigation or close starting (dispid %ld): closing the panel",
                   (long)dispIdMember);
            CloseState(state);
            Release();
        }
        return S_OK;
    }

   private:
    LONG ref_ = 1;
    PanelState* state_ = nullptr;
};

static void ConnectNavigationEvents(PanelState* state) {
    if (!state || !state->webBrowser || state->navPoint) {
        return;
    }
    IConnectionPointContainer* container = nullptr;
    if (FAILED(state->webBrowser->QueryInterface(
            IID_IConnectionPointContainer, (void**)&container)) ||
        !container) {
        Wh_Log(L"navigation events: no connection point container");
        return;
    }
    IConnectionPoint* point = nullptr;
    HRESULT hr = container->FindConnectionPoint(kDiidWebBrowserEvents2, &point);
    container->Release();
    if (FAILED(hr) || !point) {
        Wh_Log(L"navigation events: no connection point (0x%08X)",
               (unsigned)hr);
        return;
    }
    NavigationEvents* sink = new NavigationEvents(state);
    DWORD cookie = 0;
    hr = point->Advise(static_cast<IDispatch*>(sink), &cookie);
    if (FAILED(hr)) {
        Wh_Log(L"navigation events: advise failed (0x%08X)", (unsigned)hr);
        sink->Release();
        point->Release();
        return;
    }
    state->navPoint = point;
    state->navCookie = cookie;
    state->navEvents = sink;
}

static void DisconnectNavigationEvents(PanelState* state) {
    if (!state) {
        return;
    }
    if (state->navEvents) {
        state->navEvents->Detach();
    }
    if (state->navPoint) {
        if (state->navCookie) {
            state->navPoint->Unadvise(state->navCookie);
        }
        state->navPoint->Release();
        state->navPoint = nullptr;
    }
    state->navCookie = 0;
    if (state->navEvents) {
        state->navEvents->Release();
        state->navEvents = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Parent subclass
// ---------------------------------------------------------------------------

static LRESULT CALLBACK ParentSubclassProc(HWND hwnd, UINT message,
                                           WPARAM wParam, LPARAM lParam,
                                           UINT_PTR subclassId,
                                           DWORD_PTR referenceData) {
    PanelState* state = reinterpret_cast<PanelState*>(referenceData);

    if (message == WM_SIZE) {
        LRESULT result = DefSubclassProc(hwnd, message, wParam, lParam);
        Wh_Log(L"WM_SIZE: hwnd=%p %ux%u open=%d", (void*)hwnd,
              (unsigned)LOWORD(lParam), (unsigned)HIWORD(lParam),
              state && state->open ? 1 : 0);
        if (state && state->open) {
            LayoutState(state);
        }
        return result;
    }

    // A tab (or window) asked to close: give the columns back now, before
    // Explorer tears the view down and saves its settings.
    if (message == WM_CLOSE && state && hwnd == state->tab && state->open) {
        CloseState(state);
    }

    // The window or tab this state is keyed on is going away: give the columns
    // back and free everything (browser, preview handler, GDI objects) now,
    // while the view still exists, instead of keeping it for the whole session.
    if (message == WM_DESTROY && state && hwnd == state->tab) {
        Wh_Log(L"window destroyed: freeing the panel state for %p",
               (void*)hwnd);
        DestroyState(state, true);
        return DefSubclassProc(hwnd, message, wParam, lParam);
    }

    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, ParentSubclassProc, subclassId);
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Panel window
// ---------------------------------------------------------------------------

static int HitTestList(PanelState* state, int x, int y) {
    if (!state || !state->folderPreview) {
        return -1;
    }
    if (x < state->listRect.left || x >= state->listRect.right ||
        y < state->listRect.top || y >= state->listRect.bottom) {
        return -1;
    }
    if (state->scrollbarRect.right && x >= state->scrollbarRect.left) {
        return -1;
    }

    if (state->viewMode == QlView::Icons) {
        int itemWidth = std::max(1, state->iconItemWidth);
        int itemHeight = std::max(1, state->iconItemHeight);
        int col = (x - state->listRect.left) / itemWidth;
        int row =
            state->scrollRow + (y - state->listRect.top) / itemHeight;
        if (col >= state->iconColumns) {
            return -1;
        }
        int index = row * state->iconColumns + col;
        return index < (int)state->filtered.size() ? index : -1;
    }

    int top = state->listRect.top +
              (state->viewMode == QlView::Details ? state->headerRowHeight : 0);
    if (y < top) {
        return -1;
    }
    int row = state->scrollRow +
              (y - top) / std::max(1, state->rowHeight);
    return row < (int)state->filtered.size() ? row : -1;
}

static void ScrollFromY(PanelState* state, int y) {
    if (!state || !state->scrollbarRect.right) {
        return;
    }
    int total = state->textPreview ? static_cast<int>(state->textLines.size())
                                   : TotalRows(state);
    int visible = state->textPreview ? state->textVisibleLines
                                     : VisibleRows(state);
    if (total <= visible) {
        return;
    }
    int trackHeight = state->scrollbarRect.bottom - state->scrollbarRect.top;
    int thumbHeight =
        state->scrollThumbRect.bottom - state->scrollThumbRect.top;
    int maximum = std::max(1, total - visible);
    int span = trackHeight - thumbHeight;
    if (span <= 0) {
        return;
    }
    int position = y - state->scrollbarRect.top - thumbHeight / 2;
    int row = position * maximum / span;
    row = std::max(0, std::min(row, maximum));
    if (state->textPreview) {
        state->textScroll = row;
    } else {
        state->scrollRow = row;
    }
    InvalidateRect(state->panel, nullptr, TRUE);
}

static void ScrollFromX(PanelState* state, int x) {
    if (!state || state->textHScrollRect.right <= state->textHScrollRect.left) {
        return;
    }
    int trackWidth = state->textHScrollRect.right - state->textHScrollRect.left;
    int thumbWidth = state->textHThumbRect.right - state->textHThumbRect.left;
    int maximum = std::max(1, state->textMaxChars - state->textVisibleChars);
    int span = trackWidth - thumbWidth;
    if (span <= 0) {
        return;
    }
    int position = x - state->textHScrollRect.left - thumbWidth / 2;
    int value = position * maximum / span;
    state->textScrollX = std::max(0, std::min(value, maximum));
    InvalidateRect(state->panel, nullptr, TRUE);
}

struct ViewMenuItemData {
    const wchar_t* label;
    QlView mode;
    int iconSize;
    bool current = false;
};

static bool IsWindows11OrLater() {
    static int cached = -1;
    if (cached < 0) {
        cached = 0;
        if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
            using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
            auto getVersion = reinterpret_cast<RtlGetVersion_t>(
                reinterpret_cast<void*>(GetProcAddress(ntdll, "RtlGetVersion")));
            OSVERSIONINFOW info = {};
            info.dwOSVersionInfoSize = sizeof(info);
            if (getVersion && getVersion(&info) == 0 &&
                info.dwBuildNumber >= 22000) {
                cached = 1;
            }
        }
    }
    return cached == 1;
}

// Line-art icon of one entry of the view menu, in the same spirit as the
// Windows menus: outlined squares for the icon sizes, rows for List / Details.
static void DrawViewMenuLineGlyph(HDC dc, const RECT& rect, QlView mode,
                                  int iconSize, COLORREF color, UINT dpi) {
    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    int stroke = std::max(1, Scale(1, dpi));
    HBRUSH solid = CreateSolidBrush(color);

    auto line = [&](int x1, int y, int x2) {
        RECT r = {x1, y - stroke / 2, x2, y - stroke / 2 + stroke};
        FillRect(dc, &r, solid);
    };

    if (mode == QlView::Icons) {
        HPEN pen = CreatePen(PS_SOLID, stroke, color);
        HGDIOBJ oldPen = SelectObject(dc, pen);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        int radius = Scale(4, dpi);
        auto box = [&](int centerX, int centerY, int size) {
            int left = centerX - size / 2;
            int top = centerY - size / 2;
            RoundRect(dc, left, top, left + size, top + size, radius, radius);
        };
        if (iconSize >= 256) {
            box(cx, cy, Scale(14, dpi));
        } else if (iconSize >= 96) {
            box(cx, cy, Scale(12, dpi));
        } else if (iconSize >= 48) {
            box(cx, cy, Scale(10, dpi));
        } else {
            int size = Scale(6, dpi);
            int offset = Scale(4, dpi);
            for (int row = -1; row <= 1; row += 2) {
                for (int col = -1; col <= 1; col += 2) {
                    box(cx + col * offset, cy + row * offset, size);
                }
            }
        }
        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(pen);
    } else if (mode == QlView::List) {
        int left = cx - Scale(7, dpi);
        int right = cx + Scale(7, dpi);
        for (int row = -1; row <= 1; row++) {
            int y = cy + row * Scale(5, dpi);
            RECT dot = {left, y - Scale(1, dpi), left + Scale(2, dpi),
                        y - Scale(1, dpi) + Scale(2, dpi)};
            FillRect(dc, &dot, solid);
            line(left + Scale(4, dpi), y, right);
        }
    } else {
        int left = cx - Scale(7, dpi);
        int right = cx + Scale(7, dpi);
        for (int row = 0; row < 4; row++) {
            line(left, cy - Scale(6, dpi) + row * Scale(4, dpi), right);
        }
    }
    DeleteObject(solid);
}

static HFONT CreateViewMenuFont(int pixelHeight, UINT dpi) {
    return CreateFontW(-Scale(pixelHeight, dpi), 0, 0, 0, FW_NORMAL, FALSE,
                       FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static void ApplyViewSelection(PanelState* state, QlView mode, int iconSize,
                               const wchar_t* label) {
    if (!state) {
        return;
    }
    state->viewMode = mode;
    state->iconSize = iconSize;
    ApplyBrowserViewMode(state);
    if (state->viewMode == QlView::Details) {
        if (!state->browserColumnsSaved) {
            SaveBrowserColumns(state);
        }
        ScaleBrowserColumns(state);
    } else {
        RestoreBrowserColumns(state);
    }
    LayoutPanelContent(state);
    if (state->panel && IsWindow(state->panel)) {
        InvalidateRect(state->panel, nullptr, TRUE);
    }
    ArmBrowserSettle(state);
    Wh_Log(L"view menu: %s", label ? label : L"");
}

// The view menu is a small popup window drawn entirely by the mod, so it looks
// the same everywhere and does not depend on how Explorer themes native menus.
struct ViewPopup {
    PanelState* state = nullptr;
    ViewMenuItemData items[6];
    int hover = -1;
    UINT dpi = 96;

    // Style: Windows 11 (rounded, soft) or Windows 10 (flat, square).
    bool win11 = true;
    int itemHeight = 0;
    int padding = 0;
    int rowInsetX = 0;
    int rowInsetY = 0;
    int iconLeft = 0;
    int textLeft = 0;
    int rightPadding = 0;
    int fontPixels = 14;
    COLORREF background = 0;
    COLORREF border = 0;
    COLORREF text = 0;
    COLORREF hoverFill = 0;
    COLORREF currentFill = 0;
    COLORREF currentBorder = 0;
};

static int ViewPopupHit(const ViewPopup* popup, int x, int y, int width) {
    if (!popup || x < 0 || x >= width) {
        return -1;
    }
    int row = (y - popup->padding) / std::max(1, popup->itemHeight);
    if (y < popup->padding || row < 0 || row >= 6) {
        return -1;
    }
    return row;
}

static void PaintViewPopup(HDC target, const RECT& client,
                           const ViewPopup* popup) {
    int width = client.right - client.left;
    int height = client.bottom - client.top;
    HDC dc = CreateCompatibleDC(target);
    HBITMAP bitmap = CreateCompatibleBitmap(target, width, height);
    if (!dc || !bitmap) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (dc) {
            DeleteDC(dc);
        }
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    UINT dpi = popup->dpi;

    HBRUSH fillBrush = CreateSolidBrush(popup->background);
    FillRect(dc, &client, fillBrush);
    DeleteObject(fillBrush);
    HBRUSH borderBrush = CreateSolidBrush(popup->border);
    FrameRect(dc, &client, borderBrush);
    DeleteObject(borderBrush);

    HFONT font = CreateViewMenuFont(popup->fontPixels, dpi);
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, popup->text);

    for (int i = 0; i < 6; i++) {
        const ViewMenuItemData& item = popup->items[i];
        RECT row = {client.left + popup->rowInsetX,
                    client.top + popup->padding + i * popup->itemHeight,
                    client.right - popup->rowInsetX, 0};
        row.bottom = row.top + popup->itemHeight;
        bool hovered = i == popup->hover;

        if (popup->win11) {
            // Rounded highlight; a fainter one marks the active view.
            if (hovered || item.current) {
                HBRUSH tint = CreateSolidBrush(hovered ? popup->hoverFill
                                                       : popup->currentFill);
                HGDIOBJ oldBrush = SelectObject(dc, tint);
                HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
                int radius = Scale(8, dpi);
                RoundRect(dc, row.left, row.top + popup->rowInsetY,
                          row.right + 1,
                          row.bottom - popup->rowInsetY + 1, radius, radius);
                SelectObject(dc, oldPen);
                SelectObject(dc, oldBrush);
                DeleteObject(tint);
            }
        } else {
            // Flat full-width highlight; the active view gets a framed icon
            // box, as in the classic Windows 10 menus.
            if (hovered) {
                HBRUSH tint = CreateSolidBrush(popup->hoverFill);
                FillRect(dc, &row, tint);
                DeleteObject(tint);
            }
            if (item.current) {
                RECT box = {row.left + Scale(2, dpi), row.top + Scale(2, dpi),
                            0, row.bottom - Scale(2, dpi)};
                box.right = box.left + (box.bottom - box.top);
                HBRUSH boxFill = CreateSolidBrush(popup->currentFill);
                FillRect(dc, &box, boxFill);
                DeleteObject(boxFill);
                HBRUSH boxBorder = CreateSolidBrush(popup->currentBorder);
                FrameRect(dc, &box, boxBorder);
                DeleteObject(boxBorder);
            }
        }

        int iconSize = Scale(16, dpi);
        RECT iconRect = {client.left + popup->iconLeft,
                         (row.top + row.bottom - iconSize) / 2, 0, 0};
        iconRect.right = iconRect.left + iconSize;
        iconRect.bottom = iconRect.top + iconSize;
        DrawViewMenuLineGlyph(dc, iconRect, item.mode, item.iconSize,
                              popup->text, dpi);

        RECT textRect = row;
        textRect.left = client.left + popup->textLeft;
        textRect.right = client.right - popup->rightPadding;
        DrawTextW(dc, item.label, -1, &textRect,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
    }

    SelectObject(dc, oldFont);
    DeleteObject(font);
    BitBlt(target, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

static void CloseViewMenu(PanelState* state) {
    if (!state || !state->viewMenuWnd) {
        return;
    }
    HWND window = state->viewMenuWnd;
    state->viewMenuWnd = nullptr;
    if (state->panel && IsWindow(state->panel)) {
        InvalidateRect(state->panel, &state->viewDropdown, FALSE);
    }
    if (IsWindow(window)) {
        if (GetCapture() == window) {
            ReleaseCapture();
        }
        DestroyWindow(window);
    }
}

static LRESULT CALLBACK ViewMenuWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam) {
    ViewPopup* popup =
        reinterpret_cast<ViewPopup*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE:
            SetWindowLongPtrW(
                hwnd, GWLP_USERDATA,
                (LONG_PTR)reinterpret_cast<CREATESTRUCTW*>(lParam)
                    ->lpCreateParams);
            return TRUE;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE:
            if (popup) {
                RECT client = {};
                GetClientRect(hwnd, &client);
                int hover = ViewPopupHit(
                    popup, static_cast<short>(LOWORD(lParam)),
                    static_cast<short>(HIWORD(lParam)), client.right);
                if (hover != popup->hover) {
                    popup->hover = hover;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (popup) {
                RECT client = {};
                GetClientRect(hwnd, &client);
                POINT point = {static_cast<short>(LOWORD(lParam)),
                               static_cast<short>(HIWORD(lParam))};
                if (!PtInRect(&client, point)) {
                    CloseViewMenu(popup->state);
                }
            }
            return 0;
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // Any mouse button dismisses the menu, as native menus do.
            if (popup) {
                CloseViewMenu(popup->state);
            }
            return 0;
        case WM_LBUTTONUP:
            if (popup) {
                RECT client = {};
                GetClientRect(hwnd, &client);
                int index = ViewPopupHit(
                    popup, static_cast<short>(LOWORD(lParam)),
                    static_cast<short>(HIWORD(lParam)), client.right);
                if (index >= 0) {
                    PanelState* state = popup->state;
                    ViewMenuItemData chosen = popup->items[index];
                    CloseViewMenu(state);
                    ApplyViewSelection(state, chosen.mode, chosen.iconSize,
                                       chosen.label);
                }
            }
            return 0;
        case WM_CAPTURECHANGED:
            if (popup && popup->state &&
                popup->state->viewMenuWnd == hwnd) {
                CloseViewMenu(popup->state);
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            HDC dc = BeginPaint(hwnd, &paint);
            RECT client = {};
            GetClientRect(hwnd, &client);
            if (popup) {
                PaintViewPopup(dc, client, popup);
            }
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_NCDESTROY:
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            delete popup;
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool RegisterMenuClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DROPSHADOW;
    windowClass.lpfnWndProc = ViewMenuWndProc;
    windowClass.hInstance = g_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_menuClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW (menu) failed: %lu", GetLastError());
        return false;
    }
    return true;
}

static void ShowViewMenu(HWND hwnd, PanelState* state) {
    if (!state) {
        return;
    }
    if (state->viewMenuWnd) {
        CloseViewMenu(state);
        return;
    }
    ViewPopup* popup = new ViewPopup();
    popup->state = state;
    const bool win11 = IsWindows11OrLater();
    popup->win11 = win11;
    popup->items[0] = {L"Extra large icons", QlView::Icons, 256};
    popup->items[1] = {L"Large icons", QlView::Icons, 96};
    popup->items[2] = {win11 ? L"Medium-sized icons" : L"Medium icons",
                       QlView::Icons, 48};
    popup->items[3] = {L"Small icons", QlView::Icons, 16};
    popup->items[4] = {L"List", QlView::List, 16};
    popup->items[5] = {L"Details", QlView::Details, 16};

    int current = 5;
    if (state->viewMode == QlView::List) {
        current = 4;
    } else if (state->viewMode == QlView::Icons) {
        current = state->iconSize >= 256  ? 0
                  : state->iconSize >= 96 ? 1
                  : state->iconSize >= 48 ? 2
                                          : 3;
    }
    popup->items[current].current = true;

    UINT dpi = GetDpiForWindowSafe(hwnd);
    popup->dpi = dpi;

    // Dark or light follows the panel, which follows Explorer's theme.
    COLORREF panelBackground = state->background != CLR_INVALID
                                   ? state->background
                                   : GetSysColor(COLOR_WINDOW);
    bool dark = (GetRValue(panelBackground) * 299 +
                 GetGValue(panelBackground) * 587 +
                 GetBValue(panelBackground) * 114) /
                    1000 <
                128;
    COLORREF white = RGB(255, 255, 255);
    COLORREF black = RGB(0, 0, 0);
    if (win11) {
        popup->background = dark ? RGB(44, 44, 44) : RGB(249, 249, 249);
        popup->border = dark ? RGB(66, 66, 66) : RGB(224, 224, 224);
        popup->text = dark ? white : RGB(26, 26, 26);
        popup->hoverFill = BlendColor(popup->background, dark ? white : black, 9);
        popup->currentFill =
            BlendColor(popup->background, dark ? white : black, 5);
        popup->itemHeight = Scale(32, dpi);
        popup->padding = Scale(4, dpi);
        popup->rowInsetX = Scale(5, dpi);
        popup->rowInsetY = Scale(2, dpi);
        popup->iconLeft = Scale(16, dpi);
        popup->textLeft = Scale(44, dpi);
        popup->rightPadding = Scale(16, dpi);
        popup->fontPixels = 14;
    } else {
        popup->background = dark ? RGB(43, 43, 43) : RGB(242, 242, 242);
        popup->border = dark ? RGB(80, 80, 80) : RGB(204, 204, 204);
        popup->text = dark ? white : black;
        COLORREF menuAccent = GetUserAccentColor();
        popup->hoverFill = dark ? RGB(77, 77, 77)
                                : BlendColor(popup->background, menuAccent, 40);
        popup->currentFill = dark ? RGB(64, 64, 64)
                                  : BlendColor(popup->background, menuAccent, 18);
        popup->currentBorder =
            dark ? RGB(110, 110, 110)
                 : BlendColor(popup->background, menuAccent, 50);
        popup->itemHeight = Scale(28, dpi);
        popup->padding = Scale(2, dpi);
        popup->rowInsetX = 1;
        popup->rowInsetY = 0;
        popup->iconLeft = Scale(7, dpi);
        popup->textLeft = Scale(36, dpi);
        popup->rightPadding = Scale(20, dpi);
        popup->fontPixels = 12;
    }

    int textWidth = 0;
    if (HDC dc = GetDC(hwnd)) {
        HFONT font = CreateViewMenuFont(popup->fontPixels, dpi);
        HGDIOBJ oldFont = SelectObject(dc, font);
        for (const ViewMenuItemData& item : popup->items) {
            SIZE size = {};
            GetTextExtentPoint32W(dc, item.label,
                                  static_cast<int>(wcslen(item.label)), &size);
            textWidth = std::max(textWidth, static_cast<int>(size.cx));
        }
        SelectObject(dc, oldFont);
        DeleteObject(font);
        ReleaseDC(hwnd, dc);
    }
    int width = std::max(popup->textLeft + textWidth + popup->rightPadding,
                         Scale(win11 ? 190 : 170, dpi));
    int height = popup->itemHeight * 6 + popup->padding * 2;

    POINT point = {state->viewDropdown.left,
                   state->viewDropdown.bottom + Scale(2, dpi)};
    ClientToScreen(hwnd, &point);
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        if (point.y + height > monitorInfo.rcWork.bottom) {
            point.y = std::max(monitorInfo.rcWork.top,
                               monitorInfo.rcWork.bottom - height);
        }
        if (point.x + width > monitorInfo.rcWork.right) {
            point.x = std::max(monitorInfo.rcWork.left,
                               monitorInfo.rcWork.right - width);
        }
    }

    HWND owner = GetAncestor(hwnd, GA_ROOT);
    HWND window = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_menuClass, L"",
        WS_POPUP, point.x, point.y, width, height, owner, nullptr, g_instance,
        popup);
    if (!window) {
        Wh_Log(L"view menu create failed: err=%lu", GetLastError());
        delete popup;
        return;
    }

    // Rounded corners, as native Windows 11 menus have. Windows 10 menus are
    // square. dwmapi is linked (-ldwmapi), so no LoadLibrary is needed.
    if (win11) {
        DWORD corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                              sizeof(corner));
    }

    state->viewMenuWnd = window;
    InvalidateRect(hwnd, &state->viewDropdown, FALSE);
    ShowWindow(window, SW_SHOWNOACTIVATE);
    SetCapture(window);
}

// The draggable strip along the left edge of a right-docked panel.
static bool InSplitter(PanelState* state, int x) {
    if (!state || !state->dockRight || !state->panel) {
        return false;
    }
    UINT dpi = GetDpiForWindowSafe(state->panel);
    return x >= 0 && x < Scale(7, dpi);
}

static void DrawSplitter(HDC dc, const RECT& client, PanelState* state) {
    if (!state || !state->dockRight) {
        return;
    }
    COLORREF background = state->background != CLR_INVALID
                              ? state->background
                              : GetSysColor(COLOR_WINDOW);
    COLORREF text = state->text != CLR_INVALID ? state->text
                                               : GetSysColor(COLOR_WINDOWTEXT);
    // A constant, neutral hairline: no hover or drag highlight.
    RECT line = {client.left, client.top, client.left + 1, client.bottom};
    HBRUSH lineBrush = CreateSolidBrush(BlendColor(background, text, 18));
    FillRect(dc, &line, lineBrush);
    DeleteObject(lineBrush);
}

static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                     LPARAM lParam) {
    PanelState* state =
        reinterpret_cast<PanelState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE: {
            CREATESTRUCTW* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              (LONG_PTR)create->lpCreateParams);
            return TRUE;
        }
        case WM_COMMAND:
            if (state && LOWORD(wParam) == kFilterEditId &&
                HIWORD(wParam) == EN_CHANGE) {
                wchar_t buffer[256] = {};
                if (state->filterEdit) {
                    GetWindowTextW(state->filterEdit, buffer,
                                   ARRAYSIZE(buffer));
                }
                state->filter = buffer;
                // The clear button appears and disappears with the text.
                InvalidateRect(hwnd, &state->searchBoxRect, FALSE);
                if (state->textPreview) {
                    // In a text preview the query keeps only the matching lines.
                    RebuildTextLines(state);
                    InvalidateRect(hwnd, nullptr, TRUE);
                    return 0;
                }
                ApplyFilter(state);
                ClampScroll(state);
                // The list itself is updated right after the last keystroke.
                SetTimer(hwnd, QL_TIMER_FILTER, 160, nullptr);
                return 0;
            }
            if (state && LOWORD(wParam) == kFilterEditId &&
                (HIWORD(wParam) == EN_SETFOCUS ||
                 HIWORD(wParam) == EN_KILLFOCUS)) {
                InvalidateRect(hwnd, &state->searchBoxRect, FALSE);
                return 0;
            }
            break;
        case WM_CTLCOLORSTATIC:
            if (state && state->coverWnd && state->coverBrush &&
                reinterpret_cast<HWND>(lParam) == state->coverWnd) {
                SetBkColor(reinterpret_cast<HDC>(wParam), state->coverColor);
                return reinterpret_cast<LRESULT>(state->coverBrush);
            }
            break;
        case WM_CTLCOLOREDIT: {
            HDC editDc = reinterpret_cast<HDC>(wParam);
            if (state) {
                SetTextColor(editDc,
                             state->text != CLR_INVALID
                                 ? state->text
                                 : GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(editDc, FilterBoxFill(state));
                return reinterpret_cast<LRESULT>(
                    state->filterBrush
                        ? state->filterBrush
                        : GetSysColorBrush(COLOR_WINDOW));
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            if (!state) {
                break;
            }
            int x = static_cast<short>(LOWORD(lParam));
            int y = static_cast<short>(HIWORD(lParam));
            POINT point = {x, y};

            if (state->closeRect.right > state->closeRect.left &&
                PtInRect(&state->closeRect, point)) {
                CloseState(state);
                return 0;
            }

            if (state->textPreview) {
                for (int i = 0; i < 4; i++) {
                    if (state->textToolbar[i].right >
                            state->textToolbar[i].left &&
                        PtInRect(&state->textToolbar[i], point)) {
                        if (i == 0) {
                            g_textWrap = !g_textWrap.load();
                        } else if (i == 1) {
                            g_textLineNumbers = !g_textLineNumbers.load();
                        } else if (i == 2) {
                            g_textWhitespace = !g_textWhitespace.load();
                        } else {
                            g_textZebra = !g_textZebra.load();
                        }
                        // Wrap, the gutter and the whitespace dots change the
                        // visual lines, so the text is re-split; the zebra only
                        // changes what is painted.
                        if (i != 3) {
                            RebuildTextLines(state);
                        }
                        SaveTextOptions();
                        InvalidateRect(hwnd, nullptr, TRUE);
                        return 0;
                    }
                }
            }

            if (InSplitter(state, x)) {
                POINT cursor = {};
                GetCursorPos(&cursor);
                RECT panelRect = {};
                GetWindowRect(hwnd, &panelRect);
                state->draggingSplitter = true;
                state->liveResize = true;
                state->splitDragStartX = cursor.x;
                state->splitDragStartWidth = panelRect.right - panelRect.left;
                SetCapture(hwnd);
                return 0;
            }

            if (state->folderPreview &&
                PtInRect(&state->viewDropdown, point)) {
                ShowViewMenu(hwnd, state);
                return 0;
            }

            if (state->textPreview &&
                state->textHScrollRect.right > state->textHScrollRect.left) {
                RECT hit = state->textHScrollRect;
                hit.top -= Scale(6, GetDpiForWindowSafe(hwnd));
                if (PtInRect(&hit, point)) {
                    state->draggingHScroll = true;
                    SetCapture(hwnd);
                    ScrollFromX(state, x);
                    return 0;
                }
            }

            if (state->scrollbarRect.right &&
                PtInRect(&state->scrollbarRect, point)) {
                state->draggingScroll = true;
                SetCapture(hwnd);
                ScrollFromY(state, y);
                return 0;
            }

            if (state->folderPreview && state->filterEdit &&
                !state->filter.empty() &&
                PtInRect(&state->searchClearRect, point)) {
                SetWindowTextW(state->filterEdit, L"");
                SetFocus(state->filterEdit);
                return 0;
            }

            if ((state->folderPreview || state->textPreview) &&
                PtInRect(&state->searchRect, point)) {
                if (state->filterEdit) {
                    SetFocus(state->filterEdit);
                }
                return 0;
            }

            break;
        }
        case WM_LBUTTONUP:
            if (state && state->draggingSplitter) {
                state->draggingSplitter = false;
                ReleaseCapture();
                state->liveResize = false;
                KillTimer(hwnd, QL_TIMER_LIVE_LAYOUT);
                RECT panelRect = {};
                GetWindowRect(hwnd, &panelRect);
                int widthDip = MulDiv(panelRect.right - panelRect.left, 96,
                                      static_cast<int>(GetDpiForWindowSafe(hwnd)));
                g_panelWidthDip = widthDip;
                Wh_SetIntValue(L"panelWidthDip", widthDip);
                LayoutState(state);
                ArmBrowserSettle(state);
                return 0;
            }
            if (state && state->draggingHScroll) {
                state->draggingHScroll = false;
                ReleaseCapture();
                return 0;
            }
            if (state && state->draggingScroll) {
                state->draggingScroll = false;
                ReleaseCapture();
                return 0;
            }
            break;
        case WM_CAPTURECHANGED:
            if (state && state->draggingSplitter &&
                reinterpret_cast<HWND>(lParam) != hwnd) {
                state->draggingSplitter = false;
                state->liveResize = false;
                LayoutState(state);
                ArmBrowserSettle(state);
            }
            break;
        case WM_SETCURSOR:
            if (state && LOWORD(lParam) == HTCLIENT) {
                POINT cursor = {};
                GetCursorPos(&cursor);
                ScreenToClient(hwnd, &cursor);
                if (state->draggingSplitter || InSplitter(state, cursor.x)) {
                    SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                    return TRUE;
                }
                bool overButton =
                    (state->closeRect.right > state->closeRect.left &&
                     PtInRect(&state->closeRect, cursor)) ||
                    (state->textPreview &&
                     state->textToolbarHover >= 0);
                if (overButton) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;
        case WM_MOUSEMOVE: {
            if (!state) {
                break;
            }
            int x = static_cast<short>(LOWORD(lParam));
            int y = static_cast<short>(HIWORD(lParam));
            if (state->closeRect.right > state->closeRect.left) {
                POINT closePoint = {x, y};
                bool hover = PtInRect(&state->closeRect, closePoint) != FALSE;
                if (hover != state->closeHover) {
                    state->closeHover = hover;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            if (state->textPreview) {
                POINT hoverPoint = {x, y};
                int hover = -1;
                for (int i = 0; i < 4; i++) {
                    if (state->textToolbar[i].right > state->textToolbar[i].left &&
                        PtInRect(&state->textToolbar[i], hoverPoint)) {
                        hover = i;
                        break;
                    }
                }
                if (hover != state->textToolbarHover) {
                    state->textToolbarHover = hover;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            if (state->draggingSplitter) {
                POINT cursor = {};
                GetCursorPos(&cursor);
                int widthPx = state->splitDragStartWidth +
                              (state->splitDragStartX - cursor.x);
                int widthDip = MulDiv(widthPx, 96,
                                      static_cast<int>(GetDpiForWindowSafe(hwnd)));
                if (widthDip != g_panelWidthDip.load()) {
                    g_panelWidthDip = widthDip;
                    LayoutState(state);
                }
                return 0;
            }
            if (state->draggingHScroll) {
                ScrollFromX(state, x);
                return 0;
            }
            if (state->draggingScroll) {
                ScrollFromY(state, y);
                return 0;
            }
            int index = HitTestList(state, x, y);
            if (index != state->hoverIndex) {
                state->hoverIndex = index;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }
        case WM_MOUSELEAVE:
            if (state && state->textToolbarHover != -1) {
                state->textToolbarHover = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            if (state && state->hoverIndex != -1) {
                state->hoverIndex = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (state && state->folderPreview) {
                int delta = static_cast<short>(HIWORD(wParam));
                state->scrollRow -= (delta / 120) * 3;
                ClampScroll(state);
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            if (state && state->textPreview) {
                int delta = static_cast<short>(HIWORD(wParam));
                if ((GetKeyState(VK_SHIFT) & 0x8000) && !g_textWrap.load()) {
                    int maxX = std::max(
                        0, state->textMaxChars - state->textVisibleChars);
                    state->textScrollX = std::max(
                        0, std::min(state->textScrollX - (delta / 120) * 8,
                                    maxX));
                    InvalidateRect(hwnd, nullptr, TRUE);
                    return 0;
                }
                state->textScroll -= (delta / 120) * 3;
                int maximum = std::max(
                    0, static_cast<int>(state->textLines.size()) -
                           std::max(1, state->textVisibleLines));
                state->textScroll =
                    std::max(0, std::min(state->textScroll, maximum));
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            break;
        case WM_MOUSEHWHEEL:
            if (state && state->textPreview && !g_textWrap.load()) {
                int delta = static_cast<short>(HIWORD(wParam));
                int maxX =
                    std::max(0, state->textMaxChars - state->textVisibleChars);
                state->textScrollX = std::max(
                    0, std::min(state->textScrollX + (delta / 120) * 8, maxX));
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            HDC dc = BeginPaint(hwnd, &paint);
            RECT client = {};
            GetClientRect(hwnd, &client);
            int width = client.right - client.left;
            int height = client.bottom - client.top;

            if (state && EnsureBackBuffer(state, dc, width, height)) {
                DrawPanel(state->backDc, client, state);
                DrawSplitter(state->backDc, client, state);
                // One blit for the dirty rectangle: the window never shows the
                // intermediate background-then-content of a direct paint.
                BitBlt(dc, paint.rcPaint.left, paint.rcPaint.top,
                       paint.rcPaint.right - paint.rcPaint.left,
                       paint.rcPaint.bottom - paint.rcPaint.top,
                       state->backDc, paint.rcPaint.left, paint.rcPaint.top,
                       SRCCOPY);
            } else {
                DrawPanel(dc, client, state);
                DrawSplitter(dc, client, state);
            }

            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case QL_WM_OPEN_PATH:
            if (state && !state->pendingOpenPath.empty()) {
                std::wstring path = state->pendingOpenPath;
                state->pendingOpenPath.clear();
                OpenPathExternally(state, path);
            }
            return 0;
        case QL_WM_PROBE_DONE: {
            ProbeResult* result = reinterpret_cast<ProbeResult*>(lParam);
            if (state && result) {
                ApplyProbeResult(state, result);
            }
            FreeProbeResult(result);
            delete result;
            return 0;
        }
        case QL_WM_TEXT_LOADED: {
            TextLoadResult* result = reinterpret_cast<TextLoadResult*>(lParam);
            if (state) {
                ApplyTextLoadResult(state, result);
            }
            delete result;
            return 0;
        }
        case QL_WM_IMAGE_LOADED: {
            ImageLoadResult* result =
                reinterpret_cast<ImageLoadResult*>(lParam);
            if (state) {
                ApplyImageLoadResult(state, result);
            }
            if (result && result->bitmap) {
                DeleteObject(result->bitmap);
            }
            delete result;
            return 0;
        }
        case WM_TIMER:
            if (wParam == QL_TIMER_SELECTION) {
                RefreshSelection(state);
                return 0;
            }
            if (wParam == QL_TIMER_PROBE) {
                KillTimer(hwnd, QL_TIMER_PROBE);
                if (state && state->loadPending && !state->probeStarted) {
                    StartProbe(state);
                }
                return 0;
            }
            if (wParam == QL_TIMER_SPINNER) {
                if (state) {
                    TickSpinner(state);
                } else {
                    KillTimer(hwnd, QL_TIMER_SPINNER);
                }
                return 0;
            }
            if (wParam == QL_TIMER_RESULTS) {
                KillTimer(hwnd, QL_TIMER_RESULTS);
                PumpResultsItems(state);
                return 0;
            }
            if (wParam == QL_TIMER_FILTER) {
                KillTimer(hwnd, QL_TIMER_FILTER);
                ApplyBrowserFilter(state);
                return 0;
            }
            if (wParam == QL_TIMER_LIVE_LAYOUT) {
                KillTimer(hwnd, QL_TIMER_LIVE_LAYOUT);
                if (state && state->liveResize) {
                    LayoutState(state);
                }
                return 0;
            }
            if (wParam == QL_TIMER_BROWSER_COLUMNS) {
                KillTimer(hwnd, QL_TIMER_BROWSER_COLUMNS);
                if (state) {
                    SetExplorerBrowserRect(state);
                    RemoveBrowserBorders(state);
                    if (!state->resultsMode &&
                        state->viewMode == QlView::Details) {
                        ScaleBrowserColumns(state);
                        FitBrowserOverflow(state);
                    }
                    if (state->browserSettleTicks > 0) {
                        state->browserSettleTicks--;
                        SetTimer(hwnd, QL_TIMER_BROWSER_COLUMNS, 200, nullptr);
                    }
                }
                return 0;
            }
            if (wParam == QL_TIMER_COVER) {
                TickBrowserCover(state);
                return 0;
            }
            if (wParam == QL_TIMER_BROWSER_THEME) {
                KillTimer(hwnd, QL_TIMER_BROWSER_THEME);
                ThemeBrowserView(state);
                return 0;
            }
            break;
        case WM_KEYDOWN:
            if (state && (wParam == VK_ESCAPE || wParam == VK_SPACE)) {
                CloseState(state);
                return 0;
            }
            break;
        case WM_NCDESTROY:
            if (state) {
                state->panel = nullptr;
            }
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Registered once, in Wh_ModInit, so there is no lazily initialized state shared
// between Explorer's window threads.
static bool RegisterPanelClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = PanelWndProc;
    windowClass.hInstance = g_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_panelClass;

    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Per-window setup
// ---------------------------------------------------------------------------

// Results that a worker posted to the panel and the panel never got to process
// would be dropped with the window and leak (a bitmap, a file listing...).
static void DrainPanelResults(HWND panel) {
    if (!panel) {
        return;
    }
    MSG message = {};
    while (PeekMessageW(&message, panel, QL_WM_TEXT_LOADED, QL_WM_PROBE_DONE,
                        PM_REMOVE | PM_NOYIELD)) {
        if (message.message == QL_WM_PROBE_DONE) {
            ProbeResult* result = reinterpret_cast<ProbeResult*>(message.lParam);
            if (result) {
                FreeProbeResult(result);
                delete result;
            }
        } else if (message.message == QL_WM_TEXT_LOADED) {
            delete reinterpret_cast<TextLoadResult*>(message.lParam);
        } else if (message.message == QL_WM_IMAGE_LOADED) {
            ImageLoadResult* result =
                reinterpret_cast<ImageLoadResult*>(message.lParam);
            if (result) {
                if (result->bitmap) {
                    DeleteObject(result->bitmap);
                }
                delete result;
            }
        }
    }
}

static void DestroyState(PanelState* state, bool removeFromMap) {
    if (!state) {
        return;
    }

    if (state->open) {
        CloseState(state);
    }
    CloseViewMenu(state);
    DisconnectNavigationEvents(state);
    ReleaseCom(state->webBrowser);
    DestroyExplorerBrowser(state);
    if (state->columnsSaved) {
        RestoreColumns(state);
    }
    ReleaseColumns(state);
    ClearEntries(state);
    if (state->filterEdit && IsWindow(state->filterEdit)) {
        DestroyWindow(state->filterEdit);
        state->filterEdit = nullptr;
    }
    if (state->filterFont) {
        DeleteObject(state->filterFont);
        state->filterFont = nullptr;
    }
    if (state->coverWnd && IsWindow(state->coverWnd)) {
        DestroyWindow(state->coverWnd);
    }
    state->coverWnd = nullptr;
    if (state->coverBrush) {
        DeleteObject(state->coverBrush);
        state->coverBrush = nullptr;
    }
    if (state->filterBrush) {
        DeleteObject(state->filterBrush);
        state->filterBrush = nullptr;
    }
    ClearMediaPreview(state);
    ClearPropertyIcon(state);
    ReleaseBackBuffer(state);
    ReleaseCom(state->shellView);
    if (state->panel && IsWindow(state->panel)) {
        DrainPanelResults(state->panel);
        DestroyWindow(state->panel);
    }
    if (state->parent && IsWindow(state->parent)) {
        RemoveWindowSubclass(state->parent, ParentSubclassProc, 1);
    }
    if (state->tab && IsWindow(state->tab)) {
        RemoveWindowSubclass(state->tab, ParentSubclassProc, 2);
    }

    if (removeFromMap) {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (auto it = g_states.begin(); it != g_states.end();) {
            if (it->second == state) {
                it = g_states.erase(it);
            } else {
                ++it;
            }
        }
    }

    delete state;
}

static PanelState* EnsureState(HWND top, IShellView** outView = nullptr) {
    if (outView) {
        *outView = nullptr;
    }
    if (!IsWindow(top)) {
        return nullptr;
    }

    HWND tab = nullptr;
    IWebBrowser2* webBrowser = nullptr;
    IShellView* shellView = GetActiveShellView(top, &tab, &webBrowser);
    if (!shellView) {
        Wh_Log(L"ensure: no active shell view for top=%p", (void*)top);
        return nullptr;
    }

    HWND viewWindow = nullptr;
    shellView->GetWindow(&viewWindow);
    if (!viewWindow) {
        Wh_Log(L"ensure: shell view has no window for top=%p", (void*)top);
        ReleaseCom(webBrowser);
        shellView->Release();
        return nullptr;
    }

    HWND parent = GetParent(viewWindow);
    if (!parent) {
        parent = tab ? tab : top;
    }
    Wh_Log(L"ensure: top=%p tab=%p view=%p class=%s parent=%p class=%s",
          (void*)top, (void*)tab, (void*)viewWindow,
          ClassNameOf(viewWindow).c_str(), (void*)parent,
          ClassNameOf(parent).c_str());

    HWND key = tab ? tab : top;

    PanelState* state = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        auto it = g_states.find(key);
        if (it != g_states.end()) {
            state = it->second;
        }
    }

    if (state && (state->view != viewWindow || state->parent != parent)) {
        DestroyState(state, true);
        state = nullptr;
    }

    if (!state) {
        state = new PanelState();
        state->top = top;
        state->tab = key;
        state->view = viewWindow;
        state->parent = parent;

        GetWindowRect(viewWindow, &state->viewFull);
        MapWindowPoints(HWND_DESKTOP, parent,
                        reinterpret_cast<POINT*>(&state->viewFull), 2);
        GetClientRect(parent, &state->parentClient);

        state->panel = CreateWindowExW(
            0, g_panelClass, L"",
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 10, 10, parent,
            nullptr, g_instance, state);
        if (!state->panel) {
            Wh_Log(L"panel create failed: parent=%p class=%s err=%lu",
                  (void*)parent, ClassNameOf(parent).c_str(), GetLastError());
            delete state;
            ReleaseCom(webBrowser);
            shellView->Release();
            return nullptr;
        }
        Wh_Log(L"panel created: panel=%p parent=%p", (void*)state->panel,
              (void*)parent);

        UINT dpi = GetDpiForWindowSafe(state->panel);
        state->filterEdit = CreateWindowExW(
            0, L"EDIT", L"", WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0,
            state->panel, (HMENU)(INT_PTR)kFilterEditId, g_instance, nullptr);
        if (state->filterEdit) {
            SetWindowSubclass(state->filterEdit, FilterEditSubclassProc, 10,
                              (DWORD_PTR)state);
            state->filterFont = CreateFontW(
                -Scale(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            SendMessageW(state->filterEdit, WM_SETFONT,
                         (WPARAM)state->filterFont, TRUE);
            ShowWindow(state->filterEdit, SW_HIDE);
        } else {
            Wh_Log(L"filter edit create failed: err=%lu", GetLastError());
        }

        SetWindowSubclass(parent, ParentSubclassProc, 1, (DWORD_PTR)state);
        if (key != parent && IsWindow(key)) {
            SetWindowSubclass(key, ParentSubclassProc, 2, (DWORD_PTR)state);
        }

        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            g_states[key] = state;
        }

        Wh_Log(L"state created top=%p tab=%p view=%p parent=%p panel=%p",
               (void*)top, (void*)key, (void*)viewWindow, (void*)parent,
               (void*)state->panel);
    }

    state->path = GetSelectedPath(shellView, &state->debug);
    ReleaseCom(state->webBrowser);
    state->webBrowser = webBrowser;
    ReleaseCom(state->shellView);
    state->shellView = shellView;
    shellView->AddRef();
    if (outView) {
        shellView->AddRef();
        *outView = shellView;
    }
    shellView->Release();
    return state;
}

// ---------------------------------------------------------------------------
// Actions on the Explorer UI thread
// ---------------------------------------------------------------------------

struct ToggleParam {
    HWND top;
    int action;  // 0 = toggle, 1 = close
    bool handled;
    bool open;
};

static void ToggleOnExplorerThread(ToggleParam* param) {
    Wh_Log(L"toggle: action=%d top=%p", param->action, (void*)param->top);
    if (g_unloading.load()) {
        param->handled = false;
        return;
    }

    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool uninitialize = (comResult == S_OK);

    IShellView* activeView = nullptr;
    PanelState* state = EnsureState(param->top, &activeView);
    if (!state) {
        Wh_Log(L"toggle: no state, handled=false");
        param->handled = false;
        if (activeView) {
            activeView->Release();
        }
        if (uninitialize) {
            CoUninitialize();
        }
        return;
    }
    Wh_Log(L"toggle: state=%p open=%d tab=%p view=%p", (void*)state,
          state->open ? 1 : 0, (void*)state->tab, (void*)state->view);

    if (param->action == 0) {
        if (state->open) {
            CloseState(state);
        } else {
            UpdatePanelColors(state);
            if (g_reflowColumns.load() && activeView) {
                SaveColumns(state, activeView);
            }
            if (!g_textRememberOptions.load()) {
                ResetTextOptionsToDefaults();
            }
            state->open = true;
            ConnectNavigationEvents(state);
            LayoutState(state);
            LoadSelectedItem(state);
            UpdateTopOpenFlag(state->top);
            SetTimer(state->panel, QL_TIMER_SELECTION, 120, nullptr);
            Wh_Log(L"panel opened for tab=%p view=%p path=%s",
                   (void*)state->tab, (void*)state->view,
                   state->path.c_str());
        }
        param->handled = true;
        param->open = state->open;
    } else if (param->action == 1) {
        if (state->open) {
            CloseState(state);
            param->handled = true;
        }
    }

    if (activeView) {
        activeView->Release();
    }
    if (uninitialize) {
        CoUninitialize();
    }
    Wh_Log(L"toggle done: handled=%d open=%d", param->handled ? 1 : 0,
          param->open ? 1 : 0);
}

// The keyboard hook cannot call COM directly: a WH_CALLWNDPROC callback runs
// inside an input-synchronous call and CoCreateInstance(ShellWindows) fails
// with RPC_E_CANTCALLOUT_ININPUTSYNCCALL (0x8001010D). The hook therefore posts
// this message to the Explorer top-level window, and this subclass runs the
// real work from the normal message loop.

// The panel that is already open in this window, if any. A settings change must
// not create a panel (and its browser, filter edit and subclasses) in a window
// that never showed one.
static PanelState* FindOpenStateForTop(HWND top) {
    PanelState* found = nullptr;
    std::lock_guard<std::mutex> lock(g_stateLock);
    for (const auto& entry : g_states) {
        if (entry.second && entry.second->top == top && entry.second->open) {
            found = entry.second;
            break;
        }
    }
    return found;
}

static void RefreshSettingsOnExplorerThread(HWND top) {
    PanelState* state = FindOpenStateForTop(top);
    if (!state) {
        return;
    }

    CloseState(state);
    UpdatePanelColors(state);
    if (g_reflowColumns.load() && state->shellView) {
        SaveColumns(state, state->shellView);
    }
    if (!g_textRememberOptions.load()) {
        ResetTextOptionsToDefaults();
    }
    state->open = true;
    ConnectNavigationEvents(state);
    LayoutState(state);
    LoadSelectedItem(state);
    UpdateTopOpenFlag(top);
    SetTimer(state->panel, QL_TIMER_SELECTION, 120, nullptr);
    Wh_Log(L"settings applied: top=%p reflow=%d open=%d", (void*)top,
          g_reflowColumns.load() ? 1 : 0, state->open ? 1 : 0);
}

static void RefreshColorsOnExplorerThread(HWND top) {
    std::vector<PanelState*> states;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (const auto& entry : g_states) {
            if (entry.second && entry.second->top == top &&
                entry.second->open) {
                states.push_back(entry.second);
            }
        }
    }

    for (PanelState* state : states) {
        UpdatePanelColors(state);
        if (state->panel && IsWindow(state->panel)) {
            InvalidateRect(state->panel, nullptr, TRUE);
        }
    }
    Wh_Log(L"colors refreshed: top=%p states=%u", (void*)top,
          (unsigned)states.size());
}

// Closes every open panel of a window, which gives the Details columns back.
static void CloseOpenPanelsOnThread(HWND top) {
    std::vector<PanelState*> states;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (const auto& entry : g_states) {
            if (entry.second && entry.second->top == top &&
                entry.second->open) {
                states.push_back(entry.second);
            }
        }
    }

    for (PanelState* state : states) {
        CloseState(state);
    }
}

// Frees every panel state of a window. Runs on the window's own thread.
static void DestroyStatesForTop(HWND top) {
    std::vector<PanelState*> states;

    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (auto it = g_states.begin(); it != g_states.end();) {
            PanelState* state = it->second;
            if (state && state->top == top) {
                it = g_states.erase(it);
                states.push_back(state);
            } else {
                ++it;
            }
        }
    }

    for (PanelState* state : states) {
        DestroyState(state, false);
    }
}

// Undoes everything this mod installed in one Explorer window. Must run on the
// thread that owns the window.
static void CleanupTopWindow(HWND top) {
    DestroyStatesForTop(top);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(top, TopSubclassProc);
}

// Marks one window as cleaned up; the last one releases Wh_ModUninit.
static void FinishWindowCleanup(HWND top) {
    std::lock_guard<std::mutex> lock(g_cleanupMutex);
    g_cleanupPending.erase(top);
    if (g_cleanupPending.empty() && g_cleanupEvent) {
        SetEvent(g_cleanupEvent);
    }
}

static LRESULT CALLBACK TopSubclassProc(HWND hwnd, UINT message,
                                        WPARAM wParam, LPARAM lParam,
                                        DWORD_PTR) {
    if (message == QL_WM_TOGGLE) {
        Wh_Log(L"top message: hwnd=%p action=%d", (void*)hwnd, (int)wParam);
        ToggleParam param = {};
        param.top = hwnd;
        param.action = (int)wParam;
        ToggleOnExplorerThread(&param);
        return 0;
    }

    if (message == QL_WM_SETTINGS) {
        Wh_Log(L"top settings: hwnd=%p", (void*)hwnd);
        RefreshSettingsOnExplorerThread(hwnd);
        return 0;
    }

    // Wh_ModUninit asks for the cleanup of this window. wParam 0 comes from a
    // blocking SendMessage, so this thread is inside an input-synchronous call
    // and COM refuses the outgoing calls that releasing a preview handler
    // needs; re-post it so the real cleanup runs from the normal message loop.
    // wParam 1 is that posted pass (or the direct call when there is no queue
    // to wait on).
    if (message == QL_WM_CLEANUP) {
        if (wParam == 0) {
            PostMessageW(hwnd, QL_WM_CLEANUP, 1, 0);
        } else {
            CleanupTopWindow(hwnd);
            FinishWindowCleanup(hwnd);
        }
        return 0;
    }

    // 0x0320: the accent / colorization color changed.
    if (message == WM_SETTINGCHANGE || message == WM_THEMECHANGED ||
        message == WM_SYSCOLORCHANGE || message == 0x0320) {
        RefreshColorsOnExplorerThread(hwnd);
    }

    // Closing the window: the Details columns go back to normal before
    // Explorer saves the folder's view settings.
    if (message == WM_CLOSE) {
        CloseOpenPanelsOnThread(hwnd);
    }

    if (message == WM_DESTROY) {
        DestroyStatesForTop(hwnd);
    }

    if (message == WM_NCDESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TopSubclassProc);
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

static BOOL CALLBACK EnumExplorerWindowProc(HWND hwnd, LPARAM parameter) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }
    if (IsExplorerClass(hwnd)) {
        reinterpret_cast<std::vector<HWND>*>(parameter)->push_back(hwnd);
    }
    return TRUE;
}

static std::vector<HWND> GetExplorerWindows() {
    std::vector<HWND> windows;
    EnumWindows(EnumExplorerWindowProc, reinterpret_cast<LPARAM>(&windows));
    return windows;
}

// ---------------------------------------------------------------------------
// Keyboard hook
// ---------------------------------------------------------------------------

static bool IsPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

static bool IsFileViewFocus(HWND top) {
    if (!top) {
        return false;
    }
    DWORD threadId = GetWindowThreadProcessId(top, nullptr);
    GUITHREADINFO info = {};
    info.cbSize = sizeof(info);
    if (!GetGUIThreadInfo(threadId, &info)) {
        return false;
    }

    HWND focus = info.hwndFocus ? info.hwndFocus : top;
    std::wstring focusClass = ClassNameOf(focus);
    if (IsTextInputClass(focusClass.c_str()) ||
        _wcsicmp(focusClass.c_str(), L"SysTreeView32") == 0) {
        return false;
    }

    if (_wcsicmp(focusClass.c_str(), L"DirectUIHWND") == 0 ||
        _wcsicmp(focusClass.c_str(), L"DUIViewWndClassName") == 0 ||
        _wcsicmp(focusClass.c_str(), L"SysListView32") == 0 ||
        _wcsicmp(focusClass.c_str(), L"SHELLDLL_DefView") == 0 ||
        _wcsicmp(focusClass.c_str(), L"ShellTabWindowClass") == 0 ||
        _wcsicmp(focusClass.c_str(), L"CtrlNotifySink") == 0 ||
        _wcsicmp(focusClass.c_str(), g_panelClass) == 0 ||
        focus == top) {
        return true;
    }

    return false;
}

// A low-level keyboard hook runs inside the system's input pipeline: while it
// runs, the keyboard of the whole machine waits. So this callback only reads
// cheap state and posts messages. It never waits for an Explorer thread, never
// touches the disk and never logs; if the Explorer window is busy or hung the
// key is simply handed on. It only ever acts on Space and Esc, and only when
// the foreground window is one of this process's Explorer windows.
static LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam,
                                             LPARAM lParam) {
    if (code != HC_ACTION || g_unloading.load()) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (!key) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    if (key->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    static DWORD lastKey = 0;
    static DWORD lastTick = 0;

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        if (key->vkCode == VK_SPACE && g_swallowSpaceUp.load()) {
            g_swallowSpaceUp = false;
            lastKey = 0;
            lastTick = 0;
            return 1;
        }
        lastKey = 0;
        lastTick = 0;
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    DWORD now = GetTickCount();
    bool repeat = key->vkCode == lastKey && now - lastTick < 250;
    lastKey = key->vkCode;
    lastTick = now;

    if (key->vkCode != VK_SPACE && key->vkCode != VK_ESCAPE &&
        key->vkCode != 'W') {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    HWND foreground = GetForegroundWindow();
    if (!IsExplorerClass(foreground)) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foreground, &foregroundProcessId);
    if (foregroundProcessId != GetCurrentProcessId()) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // Ctrl+W closes the tab: the panel is closed first (the key is not taken),
    // so the Details columns are back before Explorer closes the tab.
    if (key->vkCode == 'W') {
        if (IsPressed(VK_CONTROL) && IsTopWithOpenPanel(foreground)) {
            PostMessageW(foreground, QL_WM_TOGGLE, 1, 0);
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    if (key->vkCode == VK_SPACE) {
        const bool control = IsPressed(VK_CONTROL);
        const bool alt = IsPressed(VK_MENU);
        const bool shift = IsPressed(VK_SHIFT);
        if (control || alt || shift) {
            g_swallowSpaceUp = false;
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        // Only when the file list (or the panel) of this window has the focus:
        // in the address bar, the search box or a rename edit, Space is typing.
        if (!IsFileViewFocus(foreground)) {
            g_swallowSpaceUp = false;
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        // A Space still held from a press we already acted on is an
        // auto-repeat, whatever the system repeat delay is: swallow it so the
        // panel opens once and stays open.
        if (g_swallowSpaceUp.load() || repeat) {
            g_swallowSpaceUp = true;
            return 1;
        }

        if (PostMessageW(foreground, QL_WM_TOGGLE, 0, 0)) {
            g_swallowSpaceUp = true;
            return 1;
        }

        g_swallowSpaceUp = false;
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // Esc: only for the window whose own panel is open, and only when the
    // focus is on the file list or the panel. A rename, the address bar and the
    // search box keep their Esc.
    if (IsTopWithOpenPanel(foreground) && IsFileViewFocus(foreground)) {
        if (PostMessageW(foreground, QL_WM_TOGGLE, 1, 0)) {
            return 1;
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hook thread
// ---------------------------------------------------------------------------

static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HHOOK g_keyboardHook = nullptr;

static DWORD WINAPI HookThreadProc(LPVOID) {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)LowLevelKeyboardProc, &module);
    if (!module) {
        module = g_instance;
    }

    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, module, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"hook install failed: err=%lu", GetLastError());
    } else {
        Wh_Log(L"hook installed: hook=%p module=%p", (void*)g_keyboardHook,
               (void*)module);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

// Quits the hook thread and waits for it, however long that takes: the hook
// callback and the message loop live in this mod's image, so the image must not
// be unmapped while the thread can still run.
static void StopHookThread() {
    if (!g_hookThread) {
        return;
    }
    // WM_QUIT is posted again until the thread is gone: a post made before the
    // thread created its message queue is lost.
    while (WaitForSingleObject(g_hookThread, 50) == WAIT_TIMEOUT) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    g_hookThreadId = 0;
}

// ---------------------------------------------------------------------------
// Windhawk callbacks
// ---------------------------------------------------------------------------

static void LoadSettings() {
    g_reflowColumns = Wh_GetIntSetting(L"reflowColumns") != 0;
    g_textRememberOptions = Wh_GetIntSetting(L"textRememberOptions") != 0;
    g_defaultTextLineNumbers = Wh_GetIntSetting(L"textLineNumbers") != 0;
    g_defaultTextZebra = Wh_GetIntSetting(L"textZebra") != 0;
    if (g_textRememberOptions.load()) {
        g_textWrap = Wh_GetIntValue(L"textWrapState", 1) != 0;
        g_textLineNumbers =
            Wh_GetIntValue(L"textNumbersState",
                           g_defaultTextLineNumbers ? 1 : 0) != 0;
        g_textWhitespace = Wh_GetIntValue(L"textWhitespaceState", 0) != 0;
        g_textZebra = Wh_GetIntValue(L"textZebraState",
                                     g_defaultTextZebra ? 1 : 0) != 0;
    } else {
        ResetTextOptionsToDefaults();
    }
    Wh_Log(L"settings: reflowColumns=%d rememberTextOptions=%d numbers=%d zebra=%d",
          g_reflowColumns.load() ? 1 : 0,
          g_textRememberOptions.load() ? 1 : 0,
          g_textLineNumbers.load() ? 1 : 0, g_textZebra.load() ? 1 : 0);
}

// Explorer windows are subclassed when they are created, so nothing has to
// look for them afterwards.
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName, DWORD dwStyle,
                                        int X, int Y, int nWidth, int nHeight,
                                        HWND hWndParent, HMENU hMenu,
                                        HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hwnd || g_unloading.load()) {
        return hwnd;
    }

    if (!IS_INTRESOURCE(lpClassName) && (_wcsicmp(lpClassName, L"CabinetWClass") == 0 ||
                             _wcsicmp(lpClassName, L"ExploreWClass") == 0)) {
        Wh_Log(L"Explorer window created: %p", (void*)hwnd);
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TopSubclassProc, 0);
    }
    return hwnd;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Quick Look initializing");
    LoadSettings();
    g_panelWidthDip = std::max(0, Wh_GetIntValue(L"panelWidthDip", 0));
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)PanelWndProc, &g_instance);
    if (!g_instance) {
        g_instance = GetModuleHandleW(nullptr);
    }

    QL_WM_TOGGLE = RegisterWindowMessageW(L"QuickLook_Toggle_" WH_MOD_ID);
    QL_WM_SETTINGS = RegisterWindowMessageW(L"QuickLook_Settings_" WH_MOD_ID);
    QL_WM_CLEANUP = RegisterWindowMessageW(L"QuickLook_Cleanup_" WH_MOD_ID);
    if (!QL_WM_TOGGLE || !QL_WM_SETTINGS || !QL_WM_CLEANUP) {
        Wh_Log(L"RegisterWindowMessageW failed: %lu", GetLastError());
        return FALSE;
    }

    // Both window classes are registered here, once, before any Explorer
    // window thread can need them.
    if (!RegisterPanelClass()) {
        return FALSE;
    }
    if (!RegisterMenuClass()) {
        UnregisterClassW(g_panelClass, g_instance);
        return FALSE;
    }

    RefreshWindowsAccent();
    // Media Foundation is started once, for the video frame fallback.
    g_mediaFoundation = SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET));
    Wh_Log(L"init: pid=%u instance=%p", GetCurrentProcessId(),
           (void*)g_instance);

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        if (g_mediaFoundation.exchange(false)) {
            MFShutdown();
        }
        UnregisterClassW(g_panelClass, g_instance);
        UnregisterClassW(g_menuClass, g_instance);
        return FALSE;
    }

    g_cleanupEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    return TRUE;
}

void Wh_ModAfterInit() {
    // The windows that already exist; the ones created from now on are caught by
    // the CreateWindowExW hook.
    auto windows = GetExplorerWindows();
    Wh_Log(L"after init: pid=%u explorer windows=%u", GetCurrentProcessId(),
           (unsigned)windows.size());
    for (HWND window : windows) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(window, TopSubclassProc,
                                                      0);
        Wh_Log(L"Explorer window: %p", (void*)window);
    }

    // Started last, so every window that exists can already answer a key.
    DWORD threadId = 0;
    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &threadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return;
    }
    g_hookThreadId = threadId;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    // A registered message: a window without the mod's subclass ignores it.
    for (HWND window : GetExplorerWindows()) {
        PostMessageW(window, QL_WM_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Quick Look unloading");
    g_unloading = true;

    // 1. Nothing can start new work any more: the key hook goes first, then
    //    every worker is cancelled and waited for.
    StopHookThread();
    StopAllWorkers();

    // 2. Every Explorer window undoes what the mod did to it, on its own
    //    thread. Waiting for a busy window is better than leaving a subclass
    //    behind that would crash Explorer on its next message.
    std::vector<HWND> windows = GetExplorerWindows();
    {
        std::lock_guard<std::mutex> lock(g_cleanupMutex);
        g_cleanupPending.clear();
        for (HWND window : windows) {
            g_cleanupPending.insert(window);
        }
        if (g_cleanupEvent) {
            ResetEvent(g_cleanupEvent);
        }
    }

    if (!g_cleanupEvent) {
        // No event to wait on: clean inline. Still correct; only the release of
        // an open preview handler can fail inside a sent call.
        for (HWND window : windows) {
            SendMessageW(window, QL_WM_CLEANUP, 1, 0);
        }
    } else {
        for (HWND window : windows) {
            if (GetWindowThreadProcessId(window, nullptr) ==
                GetCurrentThreadId()) {
                // Uninit is running on this window's own thread: waiting would
                // block the queue that runs the posted cleanup, so do it here.
                CleanupTopWindow(window);
                FinishWindowCleanup(window);
            } else {
                SendMessageW(window, QL_WM_CLEANUP, 0, 0);
            }
        }

        for (;;) {
            {
                std::lock_guard<std::mutex> lock(g_cleanupMutex);
                if (g_cleanupPending.empty()) {
                    break;
                }
                // A window destroyed before its posted cleanup ran dropped
                // that message; its WM_DESTROY already destroyed the state.
                for (auto it = g_cleanupPending.begin();
                     it != g_cleanupPending.end();) {
                    if (!IsWindow(*it)) {
                        it = g_cleanupPending.erase(it);
                    } else {
                        ++it;
                    }
                }
                if (g_cleanupPending.empty()) {
                    break;
                }
            }
            WaitForSingleObject(g_cleanupEvent, 200);
        }

        CloseHandle(g_cleanupEvent);
        g_cleanupEvent = nullptr;
    }

    if (g_mediaFoundation.exchange(false)) {
        MFShutdown();
    }

    if (g_instance) {
        UnregisterClassW(g_panelClass, g_instance);
        UnregisterClassW(g_menuClass, g_instance);
    }
}
