// ==WindhawkMod==
// @id              start-everything
// @name            Everything & Power Tools in the Start Menu
// @description     Search files, apps and settings from the Start menu with voidtools Everything, in place of Windows Search.
// @version         1.1
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lcomctl32 -ldwmapi -luser32 -liphlpapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Everything & Power Tools in the Start Menu

A native replacement for Windows 11 Start Menu search, powered by voidtools Everything. Type in the Start Menu to search files, apps, and settings instantly. Windows Search stays out of the way: its window is never shown, and it cannot start the Edge WebView2 process behind its Bing-backed search panel. Win+S and the taskbar search icon open this search too.

![Everything & Power Tools in the Start Menu](https://raw.githubusercontent.com/bardelyne/start-everything/main/screenshot.png)

## Key Features

- Instant Everything Search: Queries voidtools Everything directly through its IPC interface for fast results across millions of files. The mod keeps no index of its own.
- Smart Apps and Settings Search: Fuzzy matching across Desktop applications, Microsoft Store / UWP packages, Control Panel applets, and Windows Settings URIs (ms-settings:) with sharp shell icons, made at the exact pixel size of your display.
- Learns your favorites: apps you open from here more often move up among results that match equally well. A clearly better match always stays on top. Can be turned off in the settings, which also forgets what was learned.
- On-Demand Animated Palette: The Start Menu stays completely clean and uncluttered when idle. The search palette slides in with a short ease-out animation the moment you type or click the search box, and collapses when emptied or on Escape.
- Windows Search Out of the Way: SearchHost keeps running for the shell, but its window is never shown and it cannot launch Edge WebView2, the web view behind its Bing-backed search panel.
- Inline Calculator: Type a sum (e.g. 100 * 5, sqrt(144), 15% of 200, 2^10) and the result shows above the apps; large and small results in scientific notation too. Press Enter to copy it.
- Unit Converter: Type 100 km to see it in the usual units of its kind, or 100 km to mi (also in, ->) for just one; 255 hex or 0xFF for other bases. Knows length, mass, temperature, speed, area, volume, data, data rate, time, pressure, energy, power, angle and frequency, with . or , as the decimal point; each kind has its color. Add units it does not know in the settings (Reciprocal converts them back).
- Network Interface Inspector: Type /ip to display all active Wi-Fi, Ethernet, and VPN network interfaces with their IP addresses, subnet masks, gateways, and hardware descriptions. Press Enter to copy the IP.
- Full Right-Click Context Menu: Right-click any file, folder, or application to Open, Run as Administrator, Open in terminal (folders), Properties, Create desktop shortcut, Cut/Copy (files), Copy path, Open file location, or Uninstall (apps).
- Native Properties Dialogs: Properties opens through explorer.exe, the same dialog as in File Explorer.
- Drag and Drop: drag a file or folder from the results into File Explorer, the desktop or another app - Photoshop, a code editor, a chat - as you would from File Explorer. It is always a copy, never a move. Start closes once it has landed; Escape, or letting go over Start, cancels.
- Uninstall: right-click an app and choose Uninstall. After a confirmation that names what goes, a Store app is removed as Start removes it, and a program runs its own uninstaller, the one Installed apps in Settings runs. Apps that are part of Windows offer none. A Store app is removed in the background, with no progress bar: it is gone from the list after a few seconds.
- File Preview: the selected file gets a preview card beside the Start menu - a large thumbnail (the one File Explorer shows, so pictures, a frame of a video and documents; PDFs show their first page, drawn by Windows' own PDF renderer) with its size, date and, for pictures and videos, dimensions and length. Videos and animated GIF and WebP images play in it, muted, and a PDF turns through its first pages. SVG images are drawn by Windows' own renderer. A file with no picture shows the start of its text instead: a text or code file its first lines, and a Word, Excel, PowerPoint, RTF or OpenDocument file its first paragraphs, read by Windows' own document readers (no Office needed). Below it, the details: type, size, dates, and what the kind of file has of its own - dimensions, length, artist and album, camera, a program's description and version, a document's title and author. A folder shows how many items it holds and, when Everything keeps folder sizes, its size. A click on the path copies it.
- Opens in Front: Programs are started by Explorer, the way the stock Start menu starts them, so they come to the front even when they take a while to start or you move the mouse meanwhile.
- Explicit Web Search: Trigger web searches on demand using the '?' prefix (e.g. '?query'). Includes customizable keyword shortcuts such as '?yt' (YouTube), '?gh' (GitHub), '?w' (Wikipedia), and '?r' (Reddit).
- Start Menu Styler Compatibility: Automatically syncs background styles (Tinted Glass, Acrylic, custom theme colors) in real time without restarting the mod.
- Type Anywhere: Typing anywhere in the open Start Menu goes to the search box.
- Win+S and the Search Icon: Win+S and the taskbar search icon open the Start Menu with this search instead of the Windows search panel.

## Requirements

1. Windows 11 (version 22H2+ x86-64).
2. voidtools Everything (version 1.4 or 1.5, alpha or beta) running in the background.

## Taskbar Search

The Windows key, the Start button, Win+S, and the taskbar search icon all open this search. The full taskbar search box is not supported, so set Search to "Search icon only" or "Hide":
1. Right-click the Taskbar and select Taskbar settings (or Settings > Personalization > Taskbar).
2. Under Taskbar items, set Search to "Search icon only" or "Hide".

## Keyboard Shortcuts

- Type any key: Automatically reveals the search palette, focuses the search box, and queries apps and files.
- Up / Down: Navigate through application, calculation, conversion, and file results.
- Tab / Shift + Tab: Move to the next / previous result, through the apps and on into the files.
- Left / Right: Switch between the Apps and Files columns. Right switches only with the cursor at the end of the query, so the arrows still move the cursor while you edit.
- Enter: Launch the selected application, copy calculation/conversion/IP result, or open item. Pressed before the results for what you typed are in, it opens the first one as soon as they arrive.
- Ctrl + Enter: Run the selected application or file as Administrator (triggers UAC).
- Shift + Enter: Open the selected result's context menu, the same one a right-click opens; navigate it with the arrow keys and Enter.
- Escape: Clear the current query and smoothly collapse the search palette back to pinned apps.
- Right-Click: Context menu with Open, Run as Administrator, Open in terminal, Properties, Create desktop shortcut, Cut/Copy (files), Copy path, Open file location, and Uninstall (apps).

Note on Pinning: Windows 11 blocks programmatic pinning to the Taskbar or Start Menu. Use 'Create desktop shortcut' first, then right-click the shortcut on your desktop and select 'Pin to Taskbar' or 'Pin to Start'.

## Command Reference

- <sum>: Calculate (e.g. 100 * 5, sqrt(144), 15% of 200, 2^10).
- <number> hex, 0xFF, 0b1010: The number in other bases (Hex, Bin, Oct).
- <number> <unit>: The number in the usual units of its kind (e.g. 100 km, 32 f, 1,5 kg).
- <number> <unit> to <unit>: Just that conversion (e.g. 100 km to mi; also in, ->, =).
- /ip: List all active network interfaces and IP addresses.
- ? <term>: Web search using default search engine.
- ?<shortcut> <term>: Targeted web search (e.g. ?yt lo-fi, ?gh windhawk, ?w physics, ?r windows).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxAppResults: 6
  $name: Max App Results
  $description: Number of application matches to display in the Apps column (default 6).
- maxFileResults: 12
  $name: Max File Results
  $description: Number of file matches to display in the Files column (default 12).
- panelMargin: 0
  $name: Search Panel Margin
  $description: Space in pixels between the search panel and the edges of the Start menu. 0 (the default) fills the Start menu edge to edge; 14 gives the inset look of earlier versions.
- textScale: 100
  $name: Text Size (%)
  $description: Size of the search panel's text, as a percentage of the usual (80 to 150, default 100).
- searchDebounceMs: 0
  $name: Search Debounce Delay (ms)
  $description: Extra delay in milliseconds before searching, to let typing settle (default 0, instant). Rarely needed - while a search runs, new keystrokes already wait and only the latest text is searched.
- learnFavorites: true
  $name: Learn Favorite Apps
  $description: Apps you open from here more often move up among results that match equally well. Turning this off stops counting and forgets the apps learned so far; turn it on again to start over.
- filePreview: true
  $name: File Preview
  $description: Show a preview of the selected file beside the Start menu - its thumbnail (pictures, video frames, PDF pages, SVG), or the start of a text file or document - with its details (type, size, dates, and per kind dimensions, length, artist, version or author) and its path, which a click copies.
- animatePreview: true
  $name: Play Videos and Animations
  $description: In the file preview, play videos and animated GIF and WebP images - muted, on a loop - and turn through a PDF's first pages.
- showKeyHints: true
  $name: Show Keyboard Shortcuts Bar
  $description: Display the keyboard shortcut hints ([Up/Down] Select, [Tab] Next, [Enter] Open, [Ctrl+Enter] Admin, [Shift+Enter] Menu, [Esc] Close) in the bottom bar.
- filterNoisyPaths: true
  $name: Filter Noisy Paths
  $description: Filter out deep build caches, version control internals, and temporary directories from file search results unless no other matches exist.
- excludedPaths:
    - "\\node_modules\\"
    - "\\.git\\"
    - "\\.gradle\\"
    - "\\appdata\\local\\temp\\"
    - "\\appdata\\local\\packages\\"
    - "\\__pycache__\\"
    - "\\.venv\\"
    - "\\site-packages\\"
    - "\\.cache\\"
    - "\\build\\intermediates\\"
    - "\\obj\\debug\\"
    - "\\obj\\release\\"
    - "\\windows\\winsxs\\"
    - "\\windows\\servicing\\"
  $name: Excluded Path Patterns
  $description: >-
    Paths matching any of these substrings will be filtered out from file search results so build caches, dependencies, and internal system folders don't clutter matches.
- defaultSearchUrl: "https://duckduckgo.com/?q={q}"
  $name: Default Search Engine URL
  $description: >-
    The URL template for standard web searches. Use {q} for the query placeholder.
    Defaults to DuckDuckGo.
- webShortcuts:
    - - prefix: "yt"
        $name: Shortcut Keyword
        $description: Keyword to trigger this search (e.g. "?yt music")
      - name: "YouTube"
        $name: Service Name
      - url: "https://www.youtube.com/results?search_query={q}"
        $name: Search URL
        $description: URL template with {q} placeholder
    - - prefix: "gh"
        $name: Shortcut Keyword
      - name: "GitHub"
        $name: Service Name
      - url: "https://github.com/search?q={q}"
        $name: Search URL
    - - prefix: "w"
        $name: Shortcut Keyword
      - name: "Wikipedia"
        $name: Service Name
      - url: "https://en.wikipedia.org/wiki/Special:Search?search={q}"
        $name: Search URL
    - - prefix: "r"
        $name: Shortcut Keyword
      - name: "Reddit"
        $name: Service Name
      - url: "https://www.reddit.com/search/?q={q}"
        $name: Search URL
  $name: Web Search Shortcuts
  $description: >-
    Keywords that send a query to a specific site instead of the default search engine, typed as "?keyword query" (for example "?yt music").
- unitConversions:
    - - fromUnit: "px"
        $name: Source Unit
        $description: The unit typed after the number (e.g. px)
      - toUnit: "em"
        $name: Target Unit
      - formula: "x / 16"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "CSS"
        $name: Category
      - reciprocal: true
        $name: Reciprocal
        $description: Also convert back, from the target unit to the source unit. For a linear formula (a * x + b), as a unit conversion is.
  $name: Custom Unit Conversions
  $description: >-
    Units the converter does not know already - it knows length, mass, temperature, speed, area, volume, data, data rate, time, pressure, energy, power, angle and frequency.
    Formulas use 'x' as the input number. Add, edit or remove items at any time.
*/
// ==/WindhawkModSettings==

// Defines the GUIDs the headers below declare, so only the used ones are
// linked instead of whole objects from libuuid.
#include <initguid.h>

#include <inspectable.h>

// winbase.h defines GetCurrentTime as a macro, which collides with
// Windows.UI.Xaml.Media.Animation's method of the same name.
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
// Not just the .0.h forward declarations: Append/Size have deduced return
// types and must be defined before use.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <appmodel.h>
#include <filter.h>
#include <filterr.h>
#include <string_view>
#include <limits>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <map>
#include <optional>
#include <thread>

inline HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module);
    return module;
}

// ===========================================================================
// Component: Everything IPC Client
// ===========================================================================
// Everything IPC client.
//
// Everything exposes a hidden window that answers WM_COPYDATA queries. This
// is the only transport worth shipping: the HTTP server is off by default and
// the SDK DLL would have to be redistributed, whereas the IPC window is there
// whenever Everything is running.
//
// Two query messages exist. QUERY2 is preferred: it carries a sort order and
// returns size, dates, attributes and run count, which the ranker needs.
// QUERYW is the original and is kept as a fallback for older builds -- it
// returns names and paths only, in whatever order the Everything window
// happens to be sorted by.
//
// The struct layouts were taken from the official everything_ipc.h shipped
// with the "es" client, then confirmed against a live reply, because that
// header documents the QUERY2 field order in a sequence that does not match
// the flag bit order. They are ABI: a wrong offset reads arbitrary bytes out
// of a buffer another process filled in, so every read here is bounds-checked
// rather than trusted.


#include <windows.h>

#include <string>
#include <vector>

namespace everything {

// Window class of Everything's IPC listener.
inline constexpr wchar_t kIpcWindowClass[] = L"EVERYTHING_TASKBAR_NOTIFICATION";

// WM_COPYDATA dwData values for the two query messages.
inline constexpr DWORD kCopyDataQueryW = 2;
inline constexpr DWORD kCopyDataQuery2W = 18;

// Search flags. Worth spelling out rather than trusting memory: match-path is
// 0x4, while 0x2 is match-whole-word. Confusing the two does not fail, it
// silently changes what every query means.
inline constexpr DWORD kMatchCase = 0x00000001;
inline constexpr DWORD kMatchWholeWord = 0x00000002;
inline constexpr DWORD kMatchPath = 0x00000004;
inline constexpr DWORD kRegex = 0x00000008;

// max_results sentinel.
inline constexpr DWORD kAllResults = 0xFFFFFFFF;

// Item flags in a reply.
inline constexpr DWORD kItemFolder = 0x00000001;
inline constexpr DWORD kItemDrive = 0x00000002;

// QUERY2 request flags. Only the subset below is supported, and that is
// deliberate: the official header lists the fields inside the data blob in an
// order that disagrees with their bit order for EXTENSION, TYPE_NAME and the
// highlighted variants. For every flag named here the two orders agree, so
// the blob can be walked in ascending bit order without guessing. Adding one
// of the others means re-establishing the order against a live reply first.
inline constexpr DWORD kReqName = 0x00000001;
inline constexpr DWORD kReqPath = 0x00000002;
inline constexpr DWORD kReqFullPath = 0x00000004;
inline constexpr DWORD kReqSize = 0x00000010;
inline constexpr DWORD kReqDateCreated = 0x00000020;
inline constexpr DWORD kReqDateModified = 0x00000040;
inline constexpr DWORD kReqDateAccessed = 0x00000080;
inline constexpr DWORD kReqAttributes = 0x00000100;
inline constexpr DWORD kReqRunCount = 0x00000400;
inline constexpr DWORD kReqDateRun = 0x00000800;

inline constexpr DWORD kReqSupported =
    kReqName | kReqPath | kReqFullPath | kReqSize | kReqDateCreated |
    kReqDateModified | kReqDateAccessed | kReqAttributes | kReqRunCount |
    kReqDateRun;

// What the panel shows, plus what the ranker sorts on.
inline constexpr DWORD kDefaultRequestFlags = kReqName | kReqPath | kReqSize |
                                              kReqDateModified |
                                              kReqAttributes | kReqRunCount;

// Sort orders. Name-ascending is the only one guaranteed instant; the others
// are fast only when the matching fast-sort is enabled in Everything's
// options, which cannot be assumed on a user machine. So the client asks for
// name-ascending and the ranking happens here.
inline constexpr DWORD kSortNameAscending = 1;
inline constexpr DWORD kSortDateModifiedDescending = 14;
inline constexpr DWORD kSortRunCountDescending = 20;

#pragma pack(push, 1)
struct QueryHeaderW {
    DWORD reply_hwnd;              // truncated HWND; 32 bits suffice on x64
    DWORD reply_copydata_message;  // dwData Everything uses on the reply
    DWORD search_flags;
    DWORD offset;
    DWORD max_results;
    // followed by the null-terminated search string
};

struct ListHeaderW {
    DWORD totfolders;
    DWORD totfiles;
    DWORD totitems;
    DWORD numfolders;
    DWORD numfiles;
    DWORD numitems;
    DWORD offset;
    // followed by numitems ItemW, then the string pool
};

struct ItemW {
    DWORD flags;
    DWORD filename_offset;  // byte offset from the start of ListHeaderW
    DWORD path_offset;
};

struct Query2HeaderW {
    DWORD reply_hwnd;
    DWORD reply_copydata_message;
    DWORD search_flags;
    DWORD offset;
    DWORD max_results;
    DWORD request_flags;
    DWORD sort_type;
    // followed by the null-terminated search string
};

struct List2HeaderW {
    DWORD totitems;
    DWORD numitems;
    DWORD offset;
    DWORD request_flags;  // what Everything actually honoured
    DWORD sort_type;      // may differ from what was asked for
    // followed by numitems Item2, then the per-item data blobs
};

struct Item2 {
    DWORD flags;
    DWORD data_offset;  // byte offset from the start of List2HeaderW
};
#pragma pack(pop)

struct Result {
    std::wstring name;
    std::wstring path;
    bool isFolder = false;
    ULONGLONG size = 0;
    FILETIME dateModified{};
    DWORD attributes = 0;
    DWORD runCount = 0;
};

// Is Everything running and listening? A named instance listens under the
// class name with its name appended in brackets, as voidtools' es.exe looks
// for it with -instance -- and Everything 1.5 alpha runs as the instance
// "1.5a" unless told otherwise, so for most 1.5a users the plain name finds
// nothing. The plain name first, then 1.5a, then any other instance.
inline HWND FindIpcWindow() {
    if (HWND hwnd = FindWindowW(kIpcWindowClass, nullptr)) {
        return hwnd;
    }
    if (HWND hwnd = FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)", nullptr)) {
        return hwnd;
    }
    static const wchar_t kInstancePrefix[] = L"EVERYTHING_TASKBAR_NOTIFICATION_(";
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, nullptr, nullptr); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) {
        wchar_t name[128] = {};
        if (GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            _wcsnicmp(name, kInstancePrefix, ARRAYSIZE(kInstancePrefix) - 1) == 0) {
            return hwnd;
        }
    }
    return nullptr;
}

namespace detail {

// A bounds-checked walk over a reply buffer. Once a read runs past the end
// the cursor latches failed, so a truncated or mismatched reply yields no
// results instead of reading whatever happens to follow it in memory.
class Cursor {
   public:
    Cursor(const BYTE* base, size_t size, size_t pos)
        : base_(base), size_(size), pos_(pos), ok_(pos <= size) {}

    bool ok() const { return ok_; }

    template <typename T>
    bool Read(T* out) {
        if (!ok_ || pos_ + sizeof(T) > size_) {
            ok_ = false;
            return false;
        }
        memcpy(out, base_ + pos_, sizeof(T));
        pos_ += sizeof(T);
        return true;
    }

    // A length-prefixed, null-terminated wide string: a DWORD character count
    // excluding the terminator, then count+1 characters.
    bool ReadString(std::wstring* out) {
        DWORD chars = 0;
        if (!Read(&chars)) {
            return false;
        }
        // Cap the count before it is multiplied: a corrupt length could
        // otherwise wrap the size computation and slip past the bounds check.
        if (chars > (1u << 20)) {
            ok_ = false;
            return false;
        }
        size_t bytes = (static_cast<size_t>(chars) + 1) * sizeof(wchar_t);
        if (pos_ + bytes > size_) {
            ok_ = false;
            return false;
        }
        out->assign(reinterpret_cast<const wchar_t*>(base_ + pos_), chars);
        pos_ += bytes;
        return true;
    }

    bool Skip(size_t bytes) {
        if (!ok_ || pos_ + bytes > size_) {
            ok_ = false;
            return false;
        }
        pos_ += bytes;
        return true;
    }

   private:
    const BYTE* base_;
    size_t size_;
    size_t pos_;
    bool ok_;
};

}  // namespace detail

// Parses a QUERY2 reply.
inline bool ParseReply2(const void* data, DWORD size, std::vector<Result>* out,
                        DWORD* totalMatches) {
    if (!data || size < sizeof(List2HeaderW)) {
        return false;
    }
    const auto* base = static_cast<const BYTE*>(data);
    List2HeaderW list{};
    memcpy(&list, base, sizeof(list));

    if (totalMatches) {
        *totalMatches = list.totitems;
    }
    // Anything Everything honoured that this parser does not know the
    // position of means the blob cannot be walked safely.
    if (list.request_flags & ~kReqSupported) {
        return false;
    }
    size_t itemsEnd = sizeof(List2HeaderW) +
                      static_cast<size_t>(list.numitems) * sizeof(Item2);
    if (list.numitems > 100000 || itemsEnd > size) {
        return false;
    }

    for (DWORD i = 0; i < list.numitems; i++) {
        Item2 item{};
        memcpy(&item, base + sizeof(List2HeaderW) + i * sizeof(Item2),
               sizeof(item));

        detail::Cursor c(base, size, item.data_offset);
        Result r;
        r.isFolder = (item.flags & kItemFolder) != 0;

        // Ascending bit order, which for this flag subset is also the order
        // the fields appear in.
        std::wstring scratch;
        if ((list.request_flags & kReqName) && !c.ReadString(&r.name)) break;
        if ((list.request_flags & kReqPath) && !c.ReadString(&r.path)) break;
        if ((list.request_flags & kReqFullPath) && !c.ReadString(&scratch)) break;
        if ((list.request_flags & kReqSize) && !c.Read(&r.size)) break;
        if ((list.request_flags & kReqDateCreated) && !c.Skip(sizeof(FILETIME))) break;
        if ((list.request_flags & kReqDateModified) && !c.Read(&r.dateModified)) break;
        if ((list.request_flags & kReqDateAccessed) && !c.Skip(sizeof(FILETIME))) break;
        if ((list.request_flags & kReqAttributes) && !c.Read(&r.attributes)) break;
        if ((list.request_flags & kReqRunCount) && !c.Read(&r.runCount)) break;
        if ((list.request_flags & kReqDateRun) && !c.Skip(sizeof(FILETIME))) break;

        if (!c.ok()) {
            break;
        }
        out->push_back(std::move(r));
    }
    return true;
}

// Parses a legacy QUERYW reply.
inline bool ParseReply(const void* data, DWORD size, std::vector<Result>* out,
                       DWORD* totalMatches) {
    if (!data || size < sizeof(ListHeaderW)) {
        return false;
    }
    const auto* base = static_cast<const BYTE*>(data);
    ListHeaderW list{};
    memcpy(&list, base, sizeof(list));

    size_t itemsEnd = sizeof(ListHeaderW) +
                      static_cast<size_t>(list.numitems) * sizeof(ItemW);
    if (list.numitems > 100000 || itemsEnd > size) {
        return false;
    }
    if (totalMatches) {
        *totalMatches = list.totitems;
    }

    for (DWORD i = 0; i < list.numitems; i++) {
        ItemW it{};
        memcpy(&it, base + sizeof(ListHeaderW) + i * sizeof(ItemW), sizeof(it));
        if (it.filename_offset >= size || it.path_offset >= size) {
            return false;  // layout mismatch; refuse the whole reply
        }
        auto readString = [&](DWORD offset) -> std::wstring {
            const auto* p = reinterpret_cast<const wchar_t*>(base + offset);
            DWORD maxChars = (size - offset) / sizeof(wchar_t);
            DWORD n = 0;
            while (n < maxChars && p[n]) {
                n++;
            }
            return std::wstring(p, n);
        };
        Result r;
        r.name = readString(it.filename_offset);
        r.path = readString(it.path_offset);
        r.isFolder = (it.flags & kItemFolder) != 0;
        out->push_back(std::move(r));
    }
    return true;
}

// Synchronous query. Creates a hidden window, sends the request, and pumps
// until the reply arrives or the timeout expires.
class Client {
   public:
    // Each thread that queries has a client of its own, under a class name of
    // its own: the search thread's and the preview thread's (FolderFacts).
    explicit Client(const wchar_t* className = kReplyClass) : className_(className) {}
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    ~Client() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        if (atom_) {
            UnregisterClassW(className_, GetCurrentModuleHandle());
        }
    }

    bool Init() {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &Client::WndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = className_;
        atom_ = RegisterClassExW(&wc);
        if (!atom_) {
            return false;
        }
        hwnd_ = CreateWindowExW(0, className_, L"", WS_POPUP, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, wc.hInstance, this);
        if (!hwnd_) {
            return false;
        }

        // No message filter: Everything replies from the same integrity level
        // as Start or a higher one, which UIPI lets through, and allowing
        // lower ones would only let a sandboxed process hand us results.
        return true;
    }

    bool Query(const std::wstring& text, DWORD maxResults,
               std::vector<Result>* out, DWORD* totalMatches,
               DWORD timeoutMs = 500,
               DWORD requestFlags = kDefaultRequestFlags,
               DWORD sortType = kSortNameAscending) {
        HWND everything = FindIpcWindow();
        if (!everything || !hwnd_) {
            return false;
        }

        if (SendQuery2(everything, text, maxResults, requestFlags, sortType) &&
            Await(timeoutMs)) {
            *out = std::move(results_);
            if (totalMatches) {
                *totalMatches = total_;
            }
            return true;
        }

        // Older Everything builds answer FALSE to QUERY2. Names and paths
        // still come back through the original message; the ranker then has
        // only the name to score on.
        if (!SendQueryLegacy(everything, text, maxResults) ||
            !Await(timeoutMs)) {
            return false;
        }
        *out = std::move(results_);
        if (totalMatches) {
            *totalMatches = total_;
        }
        return true;
    }

   private:
    static constexpr wchar_t kReplyClass[] = L"WindhawkEverythingBrokerReply";
    const wchar_t* className_ = kReplyClass;
    static constexpr DWORD kReplyIdBase = 0x45560000;  // EV, then a serial

    // Everything echoes the id as the reply's dwData. A new one per query means
    // a late reply to a query that timed out can't pass for the next one's.
    void Reset(bool query2) {
        results_.clear();
        total_ = 0;
        replied_ = false;
        expecting_ = kReplyIdBase | (++serial_ & 0xFFFF);
        expectingQuery2_ = query2;
    }

    bool SendQuery2(HWND everything, const std::wstring& text,
                    DWORD maxResults, DWORD requestFlags, DWORD sortType) {
        Reset(true);
        std::vector<BYTE> buffer(sizeof(Query2HeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<Query2HeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = expecting_;
        q->search_flags = 0;
        q->offset = 0;
        q->max_results = maxResults;
        q->request_flags = requestFlags & kReqSupported;
        q->sort_type = sortType;
        memcpy(buffer.data() + sizeof(Query2HeaderW), text.c_str(),
               (text.size() + 1) * sizeof(wchar_t));
        return Send(everything, kCopyDataQuery2W, buffer);
    }

    bool SendQueryLegacy(HWND everything, const std::wstring& text,
                         DWORD maxResults) {
        Reset(false);
        std::vector<BYTE> buffer(sizeof(QueryHeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<QueryHeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = expecting_;
        q->search_flags = 0;
        q->offset = 0;
        q->max_results = maxResults;
        memcpy(buffer.data() + sizeof(QueryHeaderW), text.c_str(),
               (text.size() + 1) * sizeof(wchar_t));
        return Send(everything, kCopyDataQueryW, buffer);
    }

    bool Send(HWND everything, DWORD message, std::vector<BYTE>& buffer) {
        COPYDATASTRUCT cds{};
        cds.dwData = message;
        cds.cbData = static_cast<DWORD>(buffer.size());
        cds.lpData = buffer.data();
        DWORD_PTR result = 0;
        return SendMessageTimeoutW(everything, WM_COPYDATA,
                                   reinterpret_cast<WPARAM>(hwnd_),
                                   reinterpret_cast<LPARAM>(&cds),
                                   SMTO_ABORTIFHUNG, 500,
                                   &result) != 0;
    }

    // Everything answers with a sent (not posted) message, so this thread has
    // to be inside a message-retrieval call for the reply to be delivered.
    //
    // This used to spin on Sleep(1). That looked harmless and was not: the
    // default system timer tick is about 15.6 ms, so Sleep(1) sleeps for a
    // tick, and every query appeared to cost 15, 30 or 45 ms. A search with
    // zero matches measured 30 ms -- all of it this loop. Blocking on the
    // message queue instead removes that floor entirely.
    bool Await(DWORD timeoutMs) {
        const DWORD start = GetTickCount();
        MSG msg;
        for (;;) {
            // Drain first. The reply may already be waiting, and blocking
            // before draining would wait for the message after it.
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (replied_) {
                return true;
            }
            const DWORD elapsed = GetTickCount() - start;
            if (elapsed >= timeoutMs) {
                return false;
            }
            MsgWaitForMultipleObjectsEx(0, nullptr, timeoutMs - elapsed,
                                        QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
        if (m == WM_CREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(l);
            SetWindowLongPtrW(h, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return 0;
        }
        if (m == WM_COPYDATA) {
            auto* self =
                reinterpret_cast<Client*>(GetWindowLongPtrW(h, GWLP_USERDATA));
            auto* cds = reinterpret_cast<COPYDATASTRUCT*>(l);
            if (self && cds && cds->dwData == self->expecting_) {
                if (self->expectingQuery2_) {
                    ParseReply2(cds->lpData, cds->cbData, &self->results_,
                                &self->total_);
                } else {
                    ParseReply(cds->lpData, cds->cbData, &self->results_,
                               &self->total_);
                }
                self->replied_ = true;
                return TRUE;
            }
        }
        return DefWindowProcW(h, m, w, l);
    }

    ATOM atom_ = 0;
    HWND hwnd_ = nullptr;
    std::vector<Result> results_;
    DWORD total_ = 0;
    DWORD expecting_ = 0;
    DWORD serial_ = 0;
    bool expectingQuery2_ = false;
    bool replied_ = false;
};

}  // namespace everything
// ===========================================================================
// Component: Everything Relevance Ranker
// ===========================================================================
// Relevance ordering for Everything results.
//
// Everything is asked for results sorted by name ascending, because that is
// the only sort it guarantees is instant -- every other sort is fast only if
// the user happens to have the matching fast-sort enabled in Tools ->
// Options -> Indexes, which a mod cannot assume. Name-ascending is useless as
// a presentation order, though: a search for "code" leads with
// "-abstract-code-quality-task" out of a Gradle docs folder.
//
// So the client over-fetches a pool and this reorders it.
//
// The ordering is a lexicographic key rather than a sum of weights. A sum
// reads naturally but behaves badly here: the first attempt added small
// bonuses for recency and run count on top of a match score, and because
// thousands of results tie on the match score and most have a run count of
// zero, the top eight for "code" came back as eight different folders all
// literally named "Code", ordered by nothing meaningful. A key makes the
// tie-breaks explicit and total: how well the name matched, then how often
// the user has opened it, then how recently it changed.
//
// Known limitation: the pool is the alphabetically first N matches, so
// ranking can only reorder what that window happened to catch. A very
// recently edited file whose name sorts late loses to worse matches that sort
// early, and for a query with thousands of hits the window is a small
// fraction of them. Fixing it properly needs a sort Everything can do
// server-side, which means date-modified-descending -- fast only when the
// user has that fast-sort enabled, and silently slow when they have not. The
// honest options are to keep this, or to probe the cost of the date sort once
// at startup and use it when it turns out to be cheap. Not decided yet.


#include <windows.h>

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>


namespace ranker {

// How many results to ask Everything for before ranking. The pool has to be
// much larger than what is displayed or ranking has nothing to work with: at
// a pool of 10, a name-ascending query for "code" only ever sees names
// starting with punctuation.
//
// Measured on this machine (best of 3, ms, blocking wait):
//
//   query          total     8    50   200   300   500  1000
//   c             589945  11.3  11.7  16.6  48.6  65.7 145.6
//   code            8365  30.1  28.4  32.8  39.3  44.3  51.4
//   readme          2434  29.6  28.0  32.8  37.6  39.2  50.3
//   (no matches)       0  22.6  23.0  23.5  27.1  24.6  25.2
//
// So about 23 ms is a fixed cost inside Everything that no pool size avoids,
// and 200 is the largest pool that stays close to it for every query
// including a single letter. 300 is already 48 ms on "c" and 1000 is far too
// slow to run per keystroke.
inline constexpr DWORD kDefaultPool = 200;

inline std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

namespace detail {

inline std::wstring TrimSlashes(std::wstring_view s) {
    while (!s.empty() && (s.front() == L'\\' || s.front() == L'/')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == L'\\' || s.back() == L'/')) s.remove_suffix(1);
    return std::wstring(s);
}

// Match quality, coarse buckets, lower is better. This is the primary key and
// the only signal available when Everything is too old for QUERY2.
enum MatchClass : int {
    kExactName = 0,
    kNamePrefix = 1,
    kNameWordStart = 2,
    kNameSubstring = 3,
    kPathOnly = 4,
};

inline int Classify(const std::wstring& nameLower, const std::wstring& q) {
    size_t pos = nameLower.find(q);
    if (pos == std::wstring::npos) {
        return kPathOnly;
    }
    if (pos == 0) {
        return nameLower.size() == q.size() ? kExactName : kNamePrefix;
    }
    wchar_t prev = nameLower[pos - 1];
    bool wordStart = prev == L' ' || prev == L'-' || prev == L'_' ||
                     prev == L'.' || prev == L'(' || prev == L'[';
    return wordStart ? kNameWordStart : kNameSubstring;
}

// Checks if an item belongs to any excluded noisy path pattern.
// Handles directory paths with or without trailing slashes, full item paths,
// and skips penalization if the user explicitly typed the keyword in their query.
inline bool IsNoise(const std::wstring& pathLower, const std::wstring& nameLower, bool isFolder,
                    const std::wstring& qLower,
                    const std::vector<std::wstring>* customNoise = nullptr) {
    if (!customNoise || customNoise->empty()) return false;

    // Normalised directory path with trailing backslash
    std::wstring dir = pathLower;
    if (dir.empty() || dir.back() != L'\\') {
        dir.push_back(L'\\');
    }

    // Normalised full item path with trailing backslash for directories
    std::wstring full = dir + nameLower;
    if (isFolder) {
        full.push_back(L'\\');
    }

    for (const auto& n : *customNoise) {
        if (n.empty()) continue;

        // If the query explicitly targets this noise pattern, don't penalize it
        std::wstring raw = TrimSlashes(n);
        if (!raw.empty() && qLower.find(raw) != std::wstring::npos) {
            continue;
        }

        // Direct substring check on dir or full item path
        if (dir.find(n) != std::wstring::npos || full.find(n) != std::wstring::npos) {
            return true;
        }
        // Match with path delimiters around raw pattern
        if (!raw.empty()) {
            std::wstring bounded = L"\\" + raw + L"\\";
            if (dir.find(bounded) != std::wstring::npos || full.find(bounded) != std::wstring::npos) {
                return true;
            }
        }
    }
    return false;
}

inline ULONGLONG AsTicks(const FILETIME& ft) {
    ULARGE_INTEGER v{};
    v.LowPart = ft.dwLowDateTime;
    v.HighPart = ft.dwHighDateTime;
    return v.QuadPart;
}

// Sort key. Descending fields are stored pre-negated so the comparison is a
// plain ascending tuple compare -- writing the descending fields by swapping
// this and other inside std::tie does work, but it is the kind of expression
// nobody can check at a glance.
struct Key {
    int matchClass;         // ascending: better match first
    ULONGLONG runsDesc;     // negated run count: more opens first
    ULONGLONG modifiedDesc; // negated timestamp: more recent first
    size_t index;           // keeps the order total and reproducible

    bool operator<(const Key& o) const {
        return std::tie(matchClass, runsDesc, modifiedDesc, index) <
               std::tie(o.matchClass, o.runsDesc, o.modifiedDesc, o.index);
    }
};

// Turns a "higher is better" value into a sort key.
inline ULONGLONG Descending(ULONGLONG v) {
    return ~0ull - v;
}

}  // namespace detail

// Reorders a pool in place and truncates it to limit. Returns how many of the
// results kept are clean (not under an excluded path); when that is zero, all
// that was left was noise and the pool holds it.
inline size_t Rank(std::vector<everything::Result>* pool,
                   const std::wstring& query, size_t limit,
                   const std::vector<std::wstring>* excludedPaths = nullptr) {
    if (!pool || pool->empty()) {
        return 0;
    }
    const std::wstring q = ToLower(query);

    std::vector<detail::Key> keys;
    keys.reserve(pool->size());
    for (size_t i = 0; i < pool->size(); i++) {
        const everything::Result& r = (*pool)[i];
        int cls = detail::Classify(ToLower(r.name), q);
        // Demote noisy locations by 100 so all non-noise matches rank above them.
        if (detail::IsNoise(ToLower(r.path), ToLower(r.name), r.isFolder, q, excludedPaths)) {
            cls += 100;
        }
        // Everything only counts opens that went through Everything itself,
        // so this is sparse -- but where it is set it is the strongest
        // evidence available that the user wants this particular file. Capped
        // so one heavily used file cannot dominate a whole class.
        ULONGLONG runs = r.runCount > 50 ? 50 : r.runCount;
        keys.push_back({cls, detail::Descending(runs),
                        detail::Descending(detail::AsTicks(r.dateModified)), i});
    }

    std::sort(keys.begin(), keys.end());

    // If we have clean (non-noisy) matches, do not pollute remaining slots with noisy items!
    bool hasCleanMatches = !keys.empty() && keys.front().matchClass < 100;

    std::vector<everything::Result> ranked;
    ranked.reserve(keys.size() < limit ? keys.size() : limit);
    for (size_t i = 0; i < keys.size() && ranked.size() < limit; i++) {
        if (hasCleanMatches && keys[i].matchClass >= 100) {
            break;
        }
        ranked.push_back(std::move((*pool)[keys[i].index]));
    }
    *pool = std::move(ranked);
    return hasCleanMatches ? pool->size() : 0;
}

// The excluded paths as Everything search terms, appended to the query, for
// when Rank alone cannot help: it can only demote what the query returned,
// the first kDefaultPool matches by name, and for a broad query those can all
// be noise (a one-letter search starts with "-x-..." folders under .gradle).
//
// `!path:"<pattern>"` rules out the pattern anywhere in the full path, as
// IsNoise does -- but only single-folder patterns are sent. Measured on this
// machine for "c" (700k matches, 19 ms as typed), one like \.gradle\ adds
// about 10 ms, while one spanning folders like \appdata\local\temp\ adds 60-80
// ms, all six of the defaults together about 380 ms. Those stay with IsNoise,
// which applies every pattern exactly to whatever comes back. A pattern the
// query itself names is left out, as IsNoise leaves it undemoted.
inline std::wstring WithExclusions(const std::wstring& query,
                                   const std::vector<std::wstring>* excludedPaths) {
    if (!excludedPaths || excludedPaths->empty()) {
        return query;
    }
    const std::wstring q = ToLower(query);
    std::wstring out = query;
    for (const auto& n : *excludedPaths) {
        std::wstring raw = detail::TrimSlashes(n);
        if (raw.empty() || raw.find(L'\\') != std::wstring::npos ||
            q.find(raw) != std::wstring::npos) {
            continue;
        }
        std::wstring term;
        for (wchar_t c : n) {
            if (c != L'"') term.push_back(c);
        }
        out += L" !path:\"" + term + L"\"";
    }
    return out;
}

}  // namespace ranker
// ===========================================================================
// Component: Shell Icon Utilities
// ===========================================================================
// Turning shell icons into the pixel format the panel can draw.
//
// The panel lives in an AppContainer with no shell namespace and no file
// access, so it cannot fetch an icon for anything. Icons have to arrive as
// raw pixels over the same message that carries the rows: BGRA, top-down,
// premultiplied, which is what a XAML WriteableBitmap expects.

#include <commoncontrols.h>
#include <shellapi.h>
#include <shlobj.h>

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace icons {

// Icons are made at exactly the size they are drawn, in physical pixels, so
// XAML puts them on screen pixel for pixel. Left to XAML, a 48px icon shown at
// 24 was resampled on the GPU, and the shell's own resizing is a plain GDI
// stretch: both left jagged edges. Instead the shell is asked for its nearest
// frame at or above the size, and Resample shrinks that by averaging, which
// keeps the edges smooth.

// The sources of one output pixel along one axis, and how much of it each covers.
struct Tap {
    int index;
    float weight;
};

inline std::vector<std::vector<Tap>> AxisTaps(int from, int to) {
    std::vector<std::vector<Tap>> taps(to);
    const double scale = static_cast<double>(from) / to;
    for (int o = 0; o < to; ++o) {
        if (to <= from) {
            // Shrinking: the average of the source pixels the output pixel
            // covers, the ones at its edges weighted by how much of them.
            const double begin = o * scale, end = begin + scale;
            for (int i = static_cast<int>(begin); i < from && i < end; ++i) {
                const double cover = std::min<double>(i + 1, end) - std::max<double>(i, begin);
                if (cover > 1e-6) {
                    taps[o].push_back({i, static_cast<float>(cover / scale)});
                }
            }
        } else {
            // Growing (an icon with no frame that large): linear between the
            // two nearest source pixels.
            const double x = (o + 0.5) * scale - 0.5;
            const int left = static_cast<int>(std::floor(x));
            const float t = static_cast<float>(x - left);
            taps[o].push_back({std::clamp(left, 0, from - 1), 1.0f - t});
            taps[o].push_back({std::clamp(left + 1, 0, from - 1), t});
        }
    }
    return taps;
}

// Resizes a premultiplied BGRA image, one axis at a time.
inline std::vector<BYTE> Resample(const std::vector<BYTE>& src, int fromW, int fromH, int toW, int toH) {
    if (fromW == toW && fromH == toH) {
        return src;
    }
    const auto tapsX = AxisTaps(fromW, toW);
    const auto tapsY = AxisTaps(fromH, toH);
    std::vector<float> rows(static_cast<size_t>(fromH) * toW * 4);
    for (int y = 0; y < fromH; ++y) {
        for (int x = 0; x < toW; ++x) {
            float* d = &rows[(static_cast<size_t>(y) * toW + x) * 4];
            for (const Tap& t : tapsX[x]) {
                const BYTE* s = &src[(static_cast<size_t>(y) * fromW + t.index) * 4];
                for (int c = 0; c < 4; ++c) {
                    d[c] += s[c] * t.weight;
                }
            }
        }
    }
    std::vector<BYTE> out(static_cast<size_t>(toW) * toH * 4);
    for (int y = 0; y < toH; ++y) {
        for (int x = 0; x < toW; ++x) {
            float acc[4] = {};
            for (const Tap& t : tapsY[y]) {
                const float* s = &rows[(static_cast<size_t>(t.index) * toW + x) * 4];
                for (int c = 0; c < 4; ++c) {
                    acc[c] += s[c] * t.weight;
                }
            }
            BYTE* d = &out[(static_cast<size_t>(y) * toW + x) * 4];
            const BYTE alpha = static_cast<BYTE>(std::clamp(std::lround(acc[3]), 0L, 255L));
            for (int c = 0; c < 3; ++c) {
                // A colour above its alpha is not a premultiplied pixel.
                d[c] = static_cast<BYTE>(std::clamp(std::lround(acc[c]), 0L, static_cast<long>(alpha)));
            }
            d[3] = alpha;
        }
    }
    return out;
}

// The same for a square image.
inline std::vector<BYTE> Resample(const std::vector<BYTE>& src, int from, int to) {
    return Resample(src, from, from, to, to);
}

// Premultiplies an image whose colours are not, which XAML would otherwise
// draw with bright fringes wherever an edge is partly transparent. The shell
// hands icons over either way; a colour above its pixel's alpha is the tell.
// A bitmap with no alpha at all is opaque when the caller says so: the shell's
// are, while DrawIconEx leaves an old mask-only icon at zero alpha throughout.
inline void Premultiply(std::vector<BYTE>* pixels, bool opaqueIfNoAlpha) {
    bool straight = false, anyAlpha = false;
    for (size_t i = 0; i < pixels->size(); i += 4) {
        const BYTE* p = &(*pixels)[i];
        anyAlpha |= p[3] != 0;
        straight |= p[0] > p[3] || p[1] > p[3] || p[2] > p[3];
    }
    for (size_t i = 0; i < pixels->size(); i += 4) {
        BYTE* p = &(*pixels)[i];
        if (!anyAlpha) {
            if (opaqueIfNoAlpha) {
                p[3] = 255;
            }
        } else if (straight) {
            for (int c = 0; c < 3; ++c) {
                p[c] = static_cast<BYTE>((p[c] * p[3] + 127) / 255);
            }
        }
    }
}

// Copies a square bitmap into a top-down 32bpp premultiplied BGRA buffer of
// size x size pixels.
inline bool BitmapToBgra(HBITMAP bitmap, int size, std::vector<BYTE>* out) {
    if (!bitmap) {
        return false;
    }
    BITMAP info{};
    if (!GetObjectW(bitmap, sizeof(info), &info)) {
        return false;
    }
    const int side = info.bmWidth;
    if (side <= 0 || side > 1024 || std::abs(info.bmHeight) != side) {
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = side;
    bi.bmiHeader.biHeight = -side;  // negative: top-down, matching XAML
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(static_cast<size_t>(side) * side * 4, 0);
    HDC screen = GetDC(nullptr);
    int scanned = GetDIBits(screen, bitmap, 0, side, pixels.data(), &bi,
                            DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (scanned != side) {
        return false;
    }
    Premultiply(&pixels, true);
    *out = Resample(pixels, side, size);
    return true;
}

// Copies a bitmap of any shape into a top-down 32bpp premultiplied BGRA
// buffer, at its own size: thumbnails are seldom square.
inline bool BitmapToPixels(HBITMAP bitmap, std::vector<BYTE>* out, int* width, int* height) {
    BITMAP info{};
    if (!bitmap || !GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0 || info.bmWidth > 4096 ||
        info.bmHeight == 0 || std::abs(info.bmHeight) > 4096) {
        return false;
    }
    const int w = info.bmWidth, h = std::abs(info.bmHeight);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    out->assign(static_cast<size_t>(w) * h * 4, 0);
    HDC screen = GetDC(nullptr);
    const int scanned = GetDIBits(screen, bitmap, 0, h, out->data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (scanned != h) {
        return false;
    }
    Premultiply(out, true);
    *width = w;
    *height = h;
    return true;
}

// The size an icon was made at, which DrawIconEx draws it at when given no
// size of its own.
inline int IconSideOf(HICON icon) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return 0;
    }
    BITMAP bitmap{};
    int side = 0;
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap)) {
        side = bitmap.bmWidth;
    } else if (info.hbmMask && GetObjectW(info.hbmMask, sizeof(bitmap), &bitmap)) {
        side = bitmap.bmWidth;  // a monochrome icon: the mask is twice as high
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    return side;
}

// Draws an icon at its own size into a 32bpp surface, copies it out and
// resizes it to size x size. DrawIconEx is used rather than reading the
// icon's own bitmaps because it handles both modern 32bpp icons and the old
// mask-plus-colour pairs.
inline bool IconToBgra(HICON icon, int size, std::vector<BYTE>* out) {
    if (!icon) {
        return false;
    }
    int side = IconSideOf(icon);
    if (side <= 0 || side > 1024) {
        side = size;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = side;
    bi.bmiHeader.biHeight = -side;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) {
        return false;
    }
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        DeleteDC(dc);
        return false;
    }
    HGDIOBJ previous = SelectObject(dc, dib);
    memset(bits, 0, static_cast<size_t>(side) * side * 4);
    BOOL drawn = DrawIconEx(dc, 0, 0, icon, side, side, 0, nullptr, DI_NORMAL);
    if (drawn) {
        std::vector<BYTE> pixels(static_cast<BYTE*>(bits),
                                 static_cast<BYTE*>(bits) + static_cast<size_t>(side) * side * 4);
        Premultiply(&pixels, false);
        *out = Resample(pixels, side, size);
    }
    SelectObject(dc, previous);
    DeleteObject(dib);
    DeleteDC(dc);
    return drawn != FALSE;
}

// The side of a square BGRA icon buffer, or 0 if it is not one.
inline int IconSide(const std::vector<BYTE>& pixels) {
    const int side = static_cast<int>(std::lround(std::sqrt(pixels.size() / 4.0)));
    return side > 0 && static_cast<size_t>(side) * side * 4 == pixels.size() ? side : 0;
}

// Icons for files.
//
// Fetching a real icon per result is far too slow to do per keystroke -- a
// single IShellItemImageFactory::GetImage measured 38-220 ms. Almost every
// file of the same type has the same icon, though, so the shell is asked once
// per extension using SHGFI_USEFILEATTRIBUTES, which answers from the
// registered file type without touching the disk at all.
//
// The exception is a type whose files carry their own icon: programs,
// shortcuts, icon files. Those are fetched per file and kept per path. A new
// one measured 3-40 ms and about 0.6 ms once the shell has seen it, so each
// set of results gets a time budget for them, top rows first; the rest show
// their type's icon until they are fetched while idle (TakeLate).
class FileIconCache {
   public:
    explicit FileIconCache(int size) : size_(size) {}

    // A new icon size -- Start is on a display with another scale. Every icon
    // kept was made for the old one.
    void SetSize(int size) {
        if (size == size_) {
            return;
        }
        size_ = size;
        types_.clear();
        files_.clear();
        pending_.clear();
        late_.clear();
    }

    // Starts a new set of results: files still queued from the last one are
    // no longer on screen.
    void NewResults(std::chrono::milliseconds budget) {
        pending_.clear();
        late_.clear();
        deadline_ = std::chrono::steady_clock::now() + budget;
    }

    // Returns BGRA pixels, or nullptr when the shell had nothing.
    const std::vector<BYTE>* Get(const std::wstring& path, bool isFolder) {
        if (!isFolder && HasOwnIcon(ExtensionOf(path))) {
            auto it = files_.find(path);
            if (it == files_.end() && std::chrono::steady_clock::now() < deadline_) {
                it = StoreFile(path);
            }
            if (it == files_.end()) {
                pending_.push_back(path);
            } else if (!it->second.empty()) {
                return &it->second;
            }
        }
        return ForType(path, isFolder);
    }

    // Fetches one file left over from the budget. Returns whether there was one.
    bool FetchPending() {
        while (!pending_.empty()) {
            std::wstring path = std::move(pending_.front());  // top rows first
            pending_.erase(pending_.begin());
            if (!files_.count(path)) {
                auto it = StoreFile(path);
                if (!it->second.empty()) {
                    late_.emplace_back(path, it->second);
                }
                return true;
            }
        }
        return false;
    }

    bool HasPending() const { return !pending_.empty(); }

    // The icons FetchPending found since the last call, for rows already drawn.
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> TakeLate() {
        return std::exchange(late_, {});
    }

   private:
    using Map = std::unordered_map<std::wstring, std::vector<BYTE>>;

    const std::vector<BYTE>* ForType(const std::wstring& nameOrPath, bool isFolder) {
        std::wstring key = isFolder ? L"<dir>" : ExtensionOf(nameOrPath);
        auto it = types_.find(key);
        if (it == types_.end()) {
            // A name that does not exist is fine and is the point: with
            // SHGFI_USEFILEATTRIBUTES the shell answers from the extension alone.
            std::wstring probe = isFolder ? L"folder" : (L"file" + key);
            it = types_.emplace(key, Load(probe.c_str(),
                                          isFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
                                          SHGFI_USEFILEATTRIBUTES)).first;
        }
        return it->second.empty() ? nullptr : &it->second;
    }

    // Empty when the icon would come from somewhere slow; the type's icon
    // stands in for it then.
    Map::iterator StoreFile(const std::wstring& path) {
        if (files_.size() >= 4096) {
            files_.clear();
        }
        return files_.emplace(path, IconIsLocal(path) ? Load(path.c_str(), 0, 0) : std::vector<BYTE>{}).first;
    }

    static bool HasOwnIcon(const std::wstring& ext) {
        static const wchar_t* const kTypes[] = {L".exe", L".lnk", L".ico", L".url",
                                                L".scr", L".cpl", L".cur", L".ani"};
        for (const wchar_t* type : kTypes) {
            if (ext == type) {
                return true;
            }
        }
        return false;
    }

    // Whether the shell would read the icon from a local fixed drive: a
    // network or removable path could stall the search thread until it times
    // out. For a shortcut that is its icon location, or else its target.
    static bool IconIsLocal(const std::wstring& path) {
        if (!OnFixedDrive(path)) {
            return false;
        }
        std::wstring ext = ExtensionOf(path);
        wchar_t where[MAX_PATH] = {};
        if (ext == L".url") {
            GetPrivateProfileStringW(L"InternetShortcut", L"IconFile", L"", where, MAX_PATH, path.c_str());
        } else if (ext == L".lnk") {
            IShellLinkW* link = nullptr;
            if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) {
                return false;
            }
            IPersistFile* file = nullptr;
            bool loaded = SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file))) &&
                          SUCCEEDED(file->Load(path.c_str(), STGM_READ));
            int index = 0;
            if (loaded && (FAILED(link->GetIconLocation(where, MAX_PATH, &index)) || !where[0]) &&
                FAILED(link->GetPath(where, MAX_PATH, nullptr, SLGP_RAWPATH))) {
                where[0] = 0;
            }
            if (file) {
                file->Release();
            }
            link->Release();
            if (!loaded) {
                return false;
            }
        }
        // Nothing named: the file's own icon, a URL's browser, or the shell
        // item a link points to.
        if (!where[0]) {
            return true;
        }
        wchar_t expanded[MAX_PATH] = {};
        return ExpandEnvironmentStringsW(where, expanded, MAX_PATH) && OnFixedDrive(expanded);
    }

    static bool OnFixedDrive(const std::wstring& path) {
        if (path.size() < 3 || path[1] != L':' || path[2] != L'\\') {
            return false;
        }
        const wchar_t root[] = {path[0], L':', L'\\', 0};
        return GetDriveTypeW(root) == DRIVE_FIXED;
    }

    std::vector<BYTE> Load(const wchar_t* probe, DWORD attributes, UINT flags) {
        std::vector<BYTE> pixels;
        SHFILEINFOW info{};

        // Through the system image lists: the smaller of SHIL_LARGE (normally
        // 32px) and SHIL_EXTRALARGE (48) that is at least the size wanted,
        // resized to it by IconToBgra; past 48, the 48 enlarged. Not
        // SHIL_JUMBO: a type with no 256px image has its 48 in a corner of
        // it. The shell already has these lists, so the right one costs no
        // more than any other.
        bool got = false;
        if (SHGetFileInfoW(probe, attributes, &info, sizeof(info),
                           flags | SHGFI_SYSICONINDEX)) {
            for (int which : {SHIL_LARGE, SHIL_EXTRALARGE}) {
                IImageList* list = nullptr;
                if (FAILED(SHGetImageList(which, IID_PPV_ARGS(&list))) || !list) {
                    continue;
                }
                int cx = 0, cy = 0;
                list->GetIconSize(&cx, &cy);
                const bool fits = cx >= size_ || which == SHIL_EXTRALARGE;
                if (fits) {
                    HICON icon = nullptr;
                    if (SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon)) && icon) {
                        got = IconToBgra(icon, size_, &pixels);
                        DestroyIcon(icon);
                    }
                }
                list->Release();
                if (fits) {
                    break;
                }
            }
        }

        if (!got &&
            SHGetFileInfoW(probe, attributes, &info, sizeof(info),
                           flags | SHGFI_ICON | SHGFI_LARGEICON)) {
            IconToBgra(info.hIcon, size_, &pixels);
            DestroyIcon(info.hIcon);
        }
        return pixels;
    }

    static std::wstring ExtensionOf(const std::wstring& name) {
        size_t dot = name.rfind(L'.');
        if (dot == std::wstring::npos || dot + 1 >= name.size() ||
            name.find_first_of(L"\\/", dot) != std::wstring::npos) {
            return L"";
        }
        std::wstring ext = name.substr(dot);
        for (wchar_t& c : ext) {
            c = static_cast<wchar_t>(towlower(c));
        }
        return ext;
    }

    int size_;
    Map types_;
    Map files_;
    std::vector<std::wstring> pending_;
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> late_;
    std::chrono::steady_clock::time_point deadline_{};
};

}  // namespace icons
// ===========================================================================
// Component: Windows Settings Database
// ===========================================================================

namespace settings {

struct SettingItem {
    const wchar_t* name;
    const wchar_t* command;
    const wchar_t* area;
    const wchar_t* altNames;
};

inline const std::vector<SettingItem>& GetSettingsList() {
    static const std::vector<SettingItem> kSettings = {
        { L"Settings (Application homepage)", L"ms-settings:", L"Settings", L"Settings app;System settings" },
        { L"Control Panel (Application homepage)", L"control.exe", L"Control Panel", L"" },
        { L"Access work or school", L"ms-settings:workplace", L"Accounts", L"Workplace" },
        { L"Email and app accounts", L"ms-settings:emailandaccounts", L"Accounts", L"" },
        { L"Family and other people", L"ms-settings:otherusers", L"Accounts", L"Other users" },
        { L"Set up a kiosk", L"ms-settings:assignedaccess", L"Accounts", L"Assigned access" },
        { L"Sign-in options", L"ms-settings:signinoptions", L"Accounts", L"sign in;sign-in;password;pin;windows hello;fingerprint;face" },
        { L"Sign-in options - Dynamic lock", L"ms-settings:signinoptions-dynamiclock", L"Accounts", L"" },
        { L"Sync your settings", L"ms-settings:sync", L"Accounts", L"" },
        { L"Windows Hello setup - Face", L"ms-settings:signinoptions-launchfaceenrollment", L"Accounts", L"" },
        { L"Windows Hello setup - Fingerprint", L"ms-settings:signinoptions-launchfingerprintenrollment", L"Accounts", L"" },
        { L"Your info", L"ms-settings:yourinfo", L"Accounts", L"" },
        { L"Apps & Features", L"ms-settings:appsfeatures", L"Apps", L"Uninstall programs;Remove programs;Change programs;Repair programs;apps;apps & features;installed apps;uninstall;remove programs;applications" },
        { L"App features", L"ms-settings:appsfeatures-app", L"Apps", L"" },
        { L"Apps for websites", L"ms-settings:appsforwebsites", L"Apps", L"" },
        { L"Default apps", L"ms-settings:defaultapps", L"Apps", L"default apps;default browser;default programs;defaults" },
        { L"Manage optional features", L"ms-settings:optionalfeatures", L"Apps", L"" },
        { L"Offline Maps", L"ms-settings:maps", L"Apps", L"" },
        { L"Offline Maps - Download maps", L"ms-settings:maps-downloadmaps", L"Apps", L"" },
        { L"Startup apps", L"ms-settings:startupapps", L"Apps", L"startup;startup apps;autostart;run on startup" },
        { L"Video playback", L"ms-settings:videoplayback", L"Apps", L"" },
        { L"Notifications", L"ms-settings:cortana-notifications", L"Cortana", L"" },
        { L"More details", L"ms-settings:cortana-moredetails", L"Cortana", L"" },
        { L"Permissions and history", L"ms-settings:cortana-permissions", L"Cortana", L"" },
        { L"Windows search", L"ms-settings:cortana-windowssearch", L"Cortana", L"Windows search settings" },
        { L"Cortana - Language", L"ms-settings:cortana-language", L"Cortana", L"Talk" },
        { L"Cortana", L"ms-settings:cortana", L"Cortana", L"" },
        { L"Talk to Cortana", L"ms-settings:cortana-talktocortana", L"Cortana", L"" },
        { L"AutoPlay", L"ms-settings:autoplay", L"Devices", L"" },
        { L"Bluetooth and other devices", L"ms-settings:bluetooth", L"Devices", L"bluetooth;bt;wireless;pair" },
        { L"Devices", L"ms-settings:bluetooth", L"Bluetooth & devices", L"Manage devices;Add devices;Bluetooth devices;bluetooth;bt;wireless;pair" },
        { L"Bluetooth & devices", L"ms-settings:devices", L"Settings", L"Devices;Bluetooth devices;Printers and scanners;Phone Link;Camera;Mouse;Touchpad;Pen and Windows Ink;AutoPlay;USB" },
        { L"Connected Devices", L"ms-settings:connecteddevices", L"Devices", L"" },
        { L"Default camera", L"ms-settings:camera", L"Devices", L"camera;webcam" },
        { L"Mouse and touchpad", L"ms-settings:mousetouchpad", L"Devices", L"mouse;touchpad;cursor;pointer;scroll;touch" },
        { L"Pen and Windows Ink", L"ms-settings:pen", L"Devices", L"" },
        { L"Printers and scanners", L"ms-settings:printers", L"Devices", L"printers;printer;scanners;scanner;print;fax" },
        { L"Touchpad", L"ms-settings:devices-touchpad", L"Devices", L"" },
        { L"Typing", L"ms-settings:typing", L"Devices", L"typing;keyboard;autocorrect;touch keyboard" },
        { L"USB", L"ms-settings:usb", L"Devices", L"" },
        { L"Wheel", L"ms-settings:wheel", L"Devices", L"" },
        { L"Phone", L"ms-settings:mobile-devices", L"Phone", L"Mobile devices" },
        { L"Audio", L"ms-settings:easeofaccess-audio", L"Ease of access", L"Mono;Volume;Audio alerts" },
        { L"Closed captions", L"ms-settings:easeofaccess-closedcaptioning", L"Ease of access", L"" },
        { L"Color filters", L"ms-settings:easeofaccess-colorfilter", L"Ease of access", L"Inverted colors;Grayscale;Red-green;Blue-yellow;Green week;Red week;deuteranopia;protanopia;tritanopia" },
        { L"Mouse pointer", L"ms-settings:easeofaccess-mousepointer", L"Ease of access", L"Touch feedback" },
        { L"Display", L"ms-settings:easeofaccess-display", L"Ease of access", L"Transparency;Animations;Scroll bars;Size" },
        { L"Eye control", L"ms-settings:easeofaccess-eyecontrol", L"Ease of access", L"" },
        { L"Fonts", L"ms-settings:fonts", L"Ease of access", L"" },
        { L"High contrast", L"ms-settings:easeofaccess-highcontrast", L"Ease of access", L"" },
        { L"Keyboard", L"ms-settings:easeofaccess-keyboard", L"Ease of access", L"Print screen;Shortcuts;On-Screen;Keys;Scroll Lock;Caps Lock;Num Lock" },
        { L"Magnifier", L"ms-settings:easeofaccess-magnifier", L"Ease of access", L"Zoom" },
        { L"Mouse", L"ms-settings:easeofaccess-mouse", L"Ease of access", L"Keypad;Touch" },
        { L"Narrator", L"ms-settings:easeofaccess-narrator", L"Ease of access", L"" },
        { L"Other options", L"ms-settings:easeofaccess-otheroptions", L"Ease of access", L"" },
        { L"Speech", L"ms-settings:easeofaccess-speechrecognition", L"Ease of access", L"Recognition;Talk" },
        { L"Extras", L"ms-settings:extras", L"Extras", L"" },
        { L"Broadcasting", L"ms-settings:gaming-broadcasting", L"Gaming", L"" },
        { L"Game bar", L"ms-settings:gaming-gamebar", L"Gaming", L"" },
        { L"Game DVR", L"ms-settings:gaming-gamedvr", L"Gaming", L"" },
        { L"Game Mode", L"ms-settings:gaming-gamemode", L"Gaming", L"" },
        { L"Playing a game full screen", L"ms-settings:quietmomentsgame", L"Gaming", L"Quiet moments game" },
        { L"TruePlay", L"ms-settings:gaming-trueplay", L"Gaming", L"" },
        { L"Xbox Networking", L"ms-settings:gaming-xboxnetworking", L"Gaming", L"" },
        { L"Audio and speech", L"ms-settings:holographic-audio", L"Mixed reality", L"Holographic audio" },
        { L"Environment", L"ms-settings:privacy-holographic-environment", L"Mixed reality", L"Holographic Environment" },
        { L"Headset display", L"ms-settings:holographic-headset", L"Mixed reality", L"Holographic Headset" },
        { L"Uninstall", L"ms-settings:holographic-management", L"Mixed reality", L"Holographic Management" },
        { L"Airplane mode", L"ms-settings:network-airplanemode", L"Network and Internet", L"" },
        { L"Proximity", L"ms-settings:proximity", L"Network and Internet", L"" },
        { L"Cellular and SIM", L"ms-settings:network-cellular", L"Network and Internet", L"" },
        { L"Data usage", L"ms-settings:datausage", L"Network and Internet", L"" },
        { L"Dial-up", L"ms-settings:network-dialup", L"Network and Internet", L"" },
        { L"Direct access", L"ms-settings:network-directaccess", L"Network and Internet", L"" },
        { L"Ethernet", L"ms-settings:network-ethernet", L"Network and Internet", L"DNS;SDNS;SecureDNS;Gateway;DHCP;IP" },
        { L"Manage known networks", L"ms-settings:network-wifisettings", L"Network and Internet", L"Wi-Fi settings;wifi" },
        { L"Mobile hotspot", L"ms-settings:network-mobilehotspot", L"Network and Internet", L"hotspot;mobile hotspot;share internet" },
        { L"NFC", L"ms-settings:nfctransactions", L"Network and Internet", L"NFC Transactions" },
        { L"Proxy", L"ms-settings:network-proxy", L"Network and Internet", L"proxy;manual proxy;pac" },
        { L"Network status", L"ms-settings:network-status", L"Network and Internet", L"" },
        { L"Network", L"ms-settings:network", L"Network and Internet", L"DNS;SDNS;SecureDNS;Gateway;DHCP;IP;network;internet;ethernet;connection" },
        { L"VPN", L"ms-settings:network-vpn", L"Network and Internet", L"vpn;virtual private network" },
        { L"Wi-Fi", L"ms-settings:network-wifi", L"Network and Internet", L"Wireless;Metered connection;wifi;wi-fi;wlan;wireless;internet" },
        { L"Wi-Fi Calling", L"ms-settings:network-wificalling", L"Network and Internet", L"wifi" },
        { L"Background", L"ms-settings:personalization-background", L"Personalization", L"Wallpaper;Picture;Image;wallpaper;background;desktop image;picture" },
        { L"Choose which folders appear on Start", L"ms-settings:personalization-start-places", L"Personalization", L"Start places" },
        { L"Colors", L"ms-settings:colors", L"Personalization", L"Dark mode;Light mode;Dark color;Light color;App color;Taskbar color;Window border" },
        { L"Glance", L"ms-settings:personalization-glance", L"Personalization", L"" },
        { L"Lock screen", L"ms-settings:lockscreen", L"Personalization", L"Image;Picture;Screen saver;lock screen;lockscreen" },
        { L"Navigation bar", L"ms-settings:personalization-navbar", L"Personalization", L"" },
        { L"Personalization (category)", L"ms-settings:personalization", L"Settings", L"personalization;personalize;wallpaper;theme;colors" },
        { L"Start", L"ms-settings:personalization-start", L"Personalization", L"" },
        { L"Taskbar", L"ms-settings:taskbar", L"Personalization", L"taskbar;task bar;taskbar behaviors;unhide taskbar" },
        { L"Themes", L"ms-settings:themes", L"Personalization", L"themes;theme;desktop theme" },
        { L"Add your phone", L"ms-settings:mobile-devices-addphone", L"Phone", L"" },
        { L"Direct open your phone", L"ms-settings:mobile-devices-addphone-direct", L"Phone", L"" },
        { L"Accessory apps", L"ms-settings:privacy-accessoryapps", L"Privacy", L"" },
        { L"Account info", L"ms-settings:privacy-accountinfo", L"Privacy", L"" },
        { L"Activity history", L"ms-settings:privacy-activityhistory", L"Privacy", L"" },
        { L"Advertising ID", L"ms-settings:privacy-advertisingid", L"Privacy", L"" },
        { L"App diagnostics", L"ms-settings:privacy-appdiagnostics", L"Privacy", L"" },
        { L"Automatic file downloads", L"ms-settings:privacy-automaticfiledownloads", L"Privacy", L"" },
        { L"Background Apps", L"ms-settings:privacy-backgroundapps", L"Privacy", L"" },
        { L"Calendar", L"ms-settings:privacy-calendar", L"Privacy", L"" },
        { L"Call history", L"ms-settings:privacy-callhistory", L"Privacy", L"" },
        { L"Camera", L"ms-settings:privacy-webcam", L"Privacy", L"" },
        { L"Contacts", L"ms-settings:privacy-contacts", L"Privacy", L"" },
        { L"Documents", L"ms-settings:privacy-documents", L"Privacy", L"" },
        { L"Email", L"ms-settings:privacy-email", L"Privacy", L"" },
        { L"Eye tracker", L"ms-settings:privacy-eyetracker", L"Privacy", L"" },
        { L"Feedback and diagnostics", L"ms-settings:privacy-feedback", L"Privacy", L"" },
        { L"File system", L"ms-settings:privacy-broadfilesystemaccess", L"Privacy", L"" },
        { L"General", L"ms-settings:privacy-general", L"Privacy", L"" },
        { L"Inking and typing", L"ms-settings:privacy-speechtyping", L"Privacy", L"Speech typing" },
        { L"Location", L"ms-settings:privacy-location", L"Privacy", L"" },
        { L"Messaging", L"ms-settings:privacy-messaging", L"Privacy", L"" },
        { L"Microphone", L"ms-settings:privacy-microphone", L"Privacy", L"microphone;mic;record audio" },
        { L"Motion", L"ms-settings:privacy-motion", L"Privacy", L"" },
        { L"Notifications", L"ms-settings:privacy-notifications", L"Privacy", L"" },
        { L"Other devices", L"ms-settings:privacy-customdevices", L"Privacy", L"Custom devices" },
        { L"Phone calls", L"ms-settings:privacy-phonecalls", L"Privacy", L"" },
        { L"Pictures", L"ms-settings:privacy-pictures", L"Privacy", L"" },
        { L"Radios", L"ms-settings:privacy-radios", L"Privacy", L"" },
        { L"Speech", L"ms-settings:privacy-speech", L"Privacy", L"" },
        { L"Tasks", L"ms-settings:privacy-tasks", L"Privacy", L"" },
        { L"Videos", L"ms-settings:privacy-videos", L"Privacy", L"" },
        { L"Voice activation", L"ms-settings:privacy-voiceactivation", L"Privacy", L"" },
        { L"Accounts", L"ms-settings:surfacehub-accounts", L"SurfaceHub", L"" },
        { L"Session cleanup", L"ms-settings:surfacehub-sessioncleanup", L"SurfaceHub", L"" },
        { L"Team Conferencing", L"ms-settings:surfacehub-calling", L"SurfaceHub", L"calling" },
        { L"Team device management", L"ms-settings:surfacehub-devicemanagenent", L"SurfaceHub", L"" },
        { L"Welcome screen", L"ms-settings:surfacehub-welcome", L"SurfaceHub", L"" },
        { L"About", L"ms-settings:about", L"System", L"RAM;Processor;OS;ID;Edition;Version;about;system info;specs;pc specs;ram;processor;device name" },
        { L"Advanced display settings", L"ms-settings:display-advanced", L"System", L"refresh rate;hz;144hz;165hz;240hz;60hz;display refresh rate;display information;bit depth;color format;dynamic refresh rate" },
        { L"App volume and device preferences", L"ms-settings:apps-volume", L"System", L"volume mixer;app volume;individual volume;application volume;audio output per app;spatial audio;device preferences" },
        { L"Battery Saver", L"ms-settings:batterysaver", L"System", L"battery;battery saver;power saving" },
        { L"Battery Saver settings", L"ms-settings:batterysaver-settings", L"System", L"" },
        { L"Battery use", L"ms-settings:batterysaver-usagedetails", L"System", L"Battery saver usage details" },
        { L"Clipboard", L"ms-settings:clipboard", L"System", L"clipboard;clipboard history;win+v;sync clipboard;clear clipboard data" },
        { L"Display", L"ms-settings:display", L"System", L"Night light;Blue light;Warmer color;Red eye;display;screen;resolution;monitor;monitors;scale;hdr;refresh rate" },
        { L"Default Save Locations", L"ms-settings:savelocations", L"System", L"" },
        { L"Screen rotation", L"ms-settings:screenrotation", L"System", L"" },
        { L"Duplicating my display", L"ms-settings:quietmomentspresentation", L"System", L"Presentation" },
        { L"During these hours", L"ms-settings:quietmomentsscheduled", L"System", L"Scheduled" },
        { L"Encryption", L"ms-settings:deviceencryption", L"System", L"" },
        { L"Focus assist - Quiet hours", L"ms-settings:quiethours", L"System", L"" },
        { L"Focus assist - Quiet moments", L"ms-settings:quietmomentshome", L"System", L"" },
        { L"Graphics settings", L"ms-settings:display-advancedgraphics", L"System", L"graphics;graphics settings;hags;hardware-accelerated gpu scheduling;gpu preference;variable refresh rate;vrr;optimizations for windowed games;auto hdr;high performance gpu" },
        { L"Messaging", L"ms-settings:messaging", L"System", L"" },
        { L"Multitasking", L"ms-settings:multitasking", L"System", L"multitasking;snap windows;snap layouts;snap assist;title bar window shake;virtual desktops;alt tab;timeline" },
        { L"Night light settings", L"ms-settings:nightlight", L"System", L"night light;blue light filter;warm colors;color temperature;schedule night light" },
        { L"Phone - Default apps", L"ms-settings:phone-defaultapps", L"System", L"" },
        { L"Projecting to this PC", L"ms-settings:project", L"System", L"" },
        { L"Shared experience settings", L"ms-settings:crossdevice", L"System", L"Crossdevice;Nearby sharing settings;Share across devices" },
        { L"Nearby sharing settings", L"ms-settings:crossdevice", L"System", L"Crossdevice;Share across devices;Shared experience settings" },
        { L"Share across devices", L"ms-settings:crossdevice", L"System", L"Crossdevice;Nearby sharing settings;Shared experience settings" },
        { L"Tablet mode", L"ms-settings:tabletmode", L"System", L"" },
        { L"Notifications and actions", L"ms-settings:notifications", L"System", L"notifications;alerts;do not disturb;focus;focus assist;dnd;priority notifications" },
        { L"Remote Desktop", L"ms-settings:remotedesktop", L"System", L"remote desktop;enable rdp;allow remote desktop;remote desktop port" },
        { L"Phone", L"ms-settings:phone", L"System", L"" },
        { L"Power and sleep", L"ms-settings:powersleep", L"System", L"power;sleep;battery;power & sleep;energy;screen off;standby;power mode;power plan" },
        { L"Sound", L"ms-settings:sound", L"System", L"sound;audio;volume;speaker;speakers;headphones;mic;microphone;sound settings" },
        { L"Storage Sense", L"ms-settings:storagesense", L"System", L"storage;disk space;free up space;clean drive;storage sense;cleanup recommendations;temporary files" },
        { L"Storage policies", L"ms-settings:storagepolicies", L"System", L"" },
        { L"Date and time", L"ms-settings:dateandtime", L"Time and language", L"date;time;clock;timezone;time zone;set time" },
        { L"Japan IME settings", L"ms-settings:regionlanguage-jpnime", L"Time and language", L"jpnime" },
        { L"Region", L"ms-settings:regionformatting", L"Time and language", L"Region formatting" },
        { L"Keyboard", L"ms-settings:keyboard", L"Time and language", L"" },
        { L"Regional language", L"ms-settings:regionlanguage", L"Time and language", L"language;region;locale;keyboard language;input language" },
        { L"Bopomofo IME", L"ms-settings:regionlanguage-bpmfime", L"Time and language", L"bpmf" },
        { L"Cangjie IME", L"ms-settings:regionlanguage-cangjieime", L"Time and language", L"" },
        { L"Pinyin IME settings - domain lexicon", L"ms-settings:regionlanguage-chsime-pinyin-domainlexicon", L"Time and language", L"" },
        { L"Pinyin IME settings - Key configuration", L"ms-settings:regionlanguage-chsime-pinyin-keyconfig", L"Time and language", L"" },
        { L"Pinyin IME settings - UDP", L"ms-settings:regionlanguage-chsime-pinyin-udp", L"Time and language", L"" },
        { L"Wubi IME settings - UDP", L"ms-settings:regionlanguage-chsime-wubi-udp", L"Time and language", L"" },
        { L"Quickime", L"ms-settings:regionlanguage-quickime", L"Time and language", L"" },
        { L"Pinyin IME settings", L"ms-settings:regionlanguage-chsime-pinyin", L"Time and language", L"" },
        { L"Speech", L"ms-settings:speech", L"Time and language", L"" },
        { L"Wubi IME settings", L"ms-settings:regionlanguage-chsime-wubi", L"Time and language", L"" },
        { L"Activation", L"ms-settings:activation", L"Update and security", L"activation;product key;change product key;windows license;digital license;upgrade windows" },
        { L"Backup", L"ms-settings:backup", L"Update and security", L"" },
        { L"Delivery Optimization", L"ms-settings:delivery-optimization", L"Update and security", L"delivery optimization;allow downloads from other pcs;bandwidth limits;peer to peer update" },
        { L"Find My Device", L"ms-settings:findmydevice", L"Update and security", L"" },
        { L"For developers", L"ms-settings:developers", L"Update and security", L"developer mode;end task;taskbar end task;sideload apps;remote tools;developer options" },
        { L"Recovery", L"ms-settings:recovery", L"Update and security", L"recovery;reset this pc;advanced startup;uefi firmware settings;boot to bios;go back;reinstall windows;troubleshoot" },
        { L"Troubleshoot", L"ms-settings:troubleshoot", L"Update and security", L"" },
        { L"Windows Security", L"ms-settings:windowsdefender", L"Update and security", L"Windows Defender;Firewall;Virus;Core Isolation;Security Processor;Isolated Browsing;Exploit Protection;windows security;antivirus;defender;virus;threat" },
        { L"Windows Insider Program", L"ms-settings:windowsinsider", L"Update and security", L"" },
        { L"Windows Update", L"ms-settings:windowsupdate", L"Update and security", L"windows update;update;updates;patch;check for updates" },
        { L"Windows Update - Check for updates", L"ms-settings:windowsupdate-action", L"Update and security", L"" },
        { L"Windows Update - Check for updates", L"ms-settings:windowsupdate", L"Update and security", L"windows update;update;updates;patch;check for updates" },
        { L"Windows Update - Advanced options", L"ms-settings:windowsupdate-options", L"Update and security", L"windows update options;optional updates;delivery optimization;active hours;metered network update;pause updates" },
        { L"Windows Update - Restart options", L"ms-settings:windowsupdate-restartoptions", L"Update and security", L"" },
        { L"Windows Update - View update history", L"ms-settings:windowsupdate-history", L"Update and security", L"update history;view installed updates;uninstall updates;quality updates;driver updates" },
        { L"Windows Update - View optional updates", L"ms-settings:windowsupdate-optionalupdates", L"Update and security", L"" },
        { L"Workplace provisioning", L"ms-settings:workplace-provisioning", L"User accounts", L"" },
        { L"Provisioning", L"ms-settings:provisioning", L"User accounts", L"" },
        { L"Windows Anywhere", L"ms-settings:windowsanywhere", L"User accounts", L"" },
        { L"Accessibility Options", L"control access.cpl", L"Ease of access", L"access.cpl" },
        { L"Action Center", L"control /name Microsoft.ActionCenter", L"System and Security", L"wscui.cpl" },
        { L"Add Hardware", L"control /name Microsoft.AddHardware", L"Hardware and Sound", L"" },
        { L"Add or remove programs", L"control appwiz.cpl", L"Hardware and Sound", L"appwiz.cpl;Uninstall programs;Change programs;Repair programs" },
        { L"Administrative Tools", L"control /name Microsoft.AdministrativeTools", L"System and Security", L"" },
        { L"AutoPlay", L"control /name Microsoft.AutoPlay", L"Programs", L"" },
        { L"Backup and Restore", L"control /name Microsoft.BackupAndRestore", L"System and Security", L"" },
        { L"Biometric Devices", L"control /name Microsoft.BiometricDevices", L"Hardware and Sound", L"" },
        { L"BitLocker Drive Encryption", L"control /name Microsoft.BitLockerDriveEncryption", L"System and Security", L"" },
        { L"Bluetooth devices", L"control /bthprops.cpl", L"Hardware and Sound", L"" },
        { L"Color management", L"control /name Microsoft.ColorManagement", L"Appearance and Personalization", L"colorcpl.exe;color management;icc profile;icm;monitor color;color profiles;display profile" },
        { L"Credential manager", L"control /name Microsoft.CredentialManager", L"User accounts", L"Password" },
        { L"Client service for NetWare", L"control nwc.cpl", L"Programs", L"nwc.cpl" },
        { L"Date and time", L"control /name Microsoft.DateAndTime", L"Clock and Region", L"timedate.cpl;date and time;internet time;clock" },
        { L"Default location", L"control /name Microsoft.DefaultLocation", L"Clock and Region", L"" },
        { L"Default programs", L"control /name Microsoft.DefaultPrograms", L"Programs", L"" },
        { L"Device manager", L"control /name Microsoft.DeviceManager", L"Hardware and Sound", L"hdwwiz.cpl;devmgmt.msc;device manager;devices;hardware" },
        { L"Devices and printers", L"control /name Microsoft.DevicesAndPrinters", L"Hardware and Sound", L"printers;devices and printers;control printers" },
        { L"Devices and printers", L"explorer.exe shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", L"Hardware and Sound", L"printers;devices and printers" },
        { L"Ease of access center", L"control /name Microsoft.EaseOfAccessCenter", L"Ease of access", L"" },
        { L"Folder options", L"control /name Microsoft.FolderOptions", L"Appearance and Personalization", L"folder options;file explorer options;show hidden files" },
        { L"Fonts", L"control /name Microsoft.Fonts", L"Appearance and Personalization", L"" },
        { L"Game controllers", L"control /name Microsoft.GameControllers", L"Hardware and Sound", L"joy.cpl;game controllers;gamepad;joystick;calibrate controller" },
        { L"Get programs", L"control /name Microsoft.GetPrograms", L"Programs", L"" },
        { L"Getting started", L"control /name Microsoft.GettingStarted", L"Control Panel", L"" },
        { L"Home group", L"control /name Microsoft.HomeGroup", L"Network and Internet", L"" },
        { L"Indexing options", L"control /name Microsoft.IndexingOptions", L"System and Security", L"indexing options;search index;rebuild index" },
        { L"Infrared", L"control /name Microsoft.Infrared", L"Hardware and Sound", L"irprops.cpl" },
        { L"Internet options", L"control /name Microsoft.InternetOptions", L"Network and Internet", L"inetcpl.cpl;internet properties;proxy settings;trusted sites;certificates;tls;ssl;connections;lan settings" },
        { L"Mail - Microsoft Exchange or Windows Messaging", L"control mlcfg32.cpl", L"Network and Internet", L"mlcfg32.cpl" },
        { L"Mouse", L"control /name Microsoft.Mouse", L"Hardware and Sound", L"mouse;mouse properties;pointer speed;cursor" },
        { L"Network and sharing center", L"control /name Microsoft.NetworkAndSharingCenter", L"Network and Internet", L"network and sharing;adapter settings;network center" },
        { L"Network Connections (ncpa.cpl)", L"ncpa.cpl", L"Network and Internet", L"ncpa.cpl;network connections;change adapter settings;network adapters;network adapter properties;ethernet properties;wifi properties;ip configuration;dns properties;lan settings;network cards;network control panel" },
        { L"Network Setup Wizard", L"control netsetup.cpl", L"Network and Internet", L"netsetup.cpl" },
        { L"ODBC Data Source Administrator (32-bit)", L"%windir%/syswow64/odbcad32.exe", L"System and Security", L"odbccp32.cpl" },
        { L"ODBC Data Source Administrator (64-bit)", L"%windir%/system32/odbcad32.exe", L"System and Security", L"" },
        { L"Offline files", L"control /name Microsoft.OfflineFiles", L"Network and Internet", L"" },
        { L"Parental controls", L"control /name Microsoft.ParentalControls", L"User accounts", L"" },
        { L"Pen and input devices", L"control /name Microsoft.PenAndInputDevices", L"Hardware and Sound", L"" },
        { L"Pen and touch", L"control /name Microsoft.PenAndTouch", L"Hardware and Sound", L"" },
        { L"People Near Me", L"control /name Microsoft.PeopleNearMe", L"User accounts", L"" },
        { L"Performance information and tools", L"control /name Microsoft.PerformanceInformationAndTools", L"System and Security", L"" },
        { L"Phone and modem - Options", L"control /name Microsoft.PhoneAndModemOptions", L"Network and Internet", L"modem.cpl;telephon.cpl" },
        { L"Phone and modem", L"control /name Microsoft.PhoneAndModem", L"Network and Internet", L"modem.cpl;telephon.cpl" },
        { L"Power options", L"control /name Microsoft.PowerOptions", L"System and Security", L"powercfg.cpl" },
        { L"Printers", L"control /name Microsoft.Printers", L"Hardware and Sound", L"" },
        { L"Problem reports and solutions", L"control /name Microsoft.ProblemReportsAndSolutions", L"System and Security", L"" },
        { L"Programs and features", L"control /name Microsoft.ProgramsAndFeatures", L"Programs", L"" },
        { L"Recovery", L"control /name Microsoft.Recovery", L"System and Security", L"" },
        { L"Region and language", L"control /name Microsoft.RegionAndLanguage", L"Clock and Region", L"" },
        { L"RemoteApp and desktop connections", L"control /name Microsoft.RemoteAppAndDesktopConnections", L"Network and Internet", L"" },
        { L"Scanners and cameras", L"control /name Microsoft.ScannersAndCameras", L"Hardware and Sound", L"sticpl.cpl" },
        { L"Scheduled tasks", L"control schedtasks", L"System and Security", L"schedtasks" },
        { L"Security Center", L"control /name Microsoft.SecurityCenter", L"System and Security", L"" },
        { L"Sound", L"control /name Microsoft.Sound", L"Hardware and Sound", L"" },
        { L"Speech recognition", L"control /name Microsoft.SpeechRecognition", L"Ease of access", L"" },
        { L"Sync center", L"control /name Microsoft.SyncCenter", L"System and Security", L"" },
        { L"System", L"control sysdm.cpl", L"System and Security", L"sysdm.cpl" },
        { L"Tablet PC settings", L"control /name Microsoft.TabletPCSettings", L"System and Security", L"TabletPC.cpl" },
        { L"Text to speech", L"control /name Microsoft.TextToSpeech", L"Ease of access", L"" },
        { L"User accounts", L"control /name Microsoft.UserAccounts", L"User accounts", L"" },
        { L"Welcome center", L"control /name Microsoft.WelcomeCenter", L"System and Security", L"" },
        { L"Windows Anytime Upgrade", L"control /name Microsoft.WindowsAnytimeUpgrade", L"System and Security", L"" },
        { L"Windows CardSpace", L"control /name Microsoft.CardSpace", L"Hardware and Sound", L"" },
        { L"Windows Defender", L"control /name Microsoft.WindowsDefender", L"System and Security", L"" },
        { L"Windows Firewall", L"control /name Microsoft.WindowsFirewall", L"System and Security", L"" },
        { L"Windows Mobility Center", L"control /name Microsoft.MobilityCenter", L"Network and Internet", L"" },
        { L"Display properties", L"control Desk.cpl", L"Hardware and Sound", L"desk.cpl" },
        { L"FindFast", L"control FindFast.cpl", L"System and Security", L"findfast.cpl" },
        { L"Regional settings properties", L"control Intl.cpl", L"Ease of access", L"intl.cpl" },
        { L"Joystick properties", L"control Joy.cpl", L"Hardware and Sound", L"joy.cpl" },
        { L"Mouse, Fonts, Keyboard, and Printers properties", L"control Main.cpl", L"Hardware and Sound", L"main.cpl" },
        { L"Multimedia properties", L"control Mmsys.cpl", L"Hardware and Sound", L"mmsys.cpl" },
        { L"Network properties", L"control Netcpl.cpl", L"Network and Internet", L"netcpl.cpl" },
        { L"Password properties", L"control Password.cpl", L"User accounts", L"password.cpl" },
        { L"System properties and Add New Hardware wizard", L"control Sysdm.cpl", L"Hardware and Sound", L"sysdm.cpl" },
        { L"Desktop themes", L"control Themes.cpl", L"Appearance and Personalization", L"themes.cpl" },
        { L"Microsoft Mail Post Office", L"control Wgpocpl.cpl", L"Programs", L"wgpocpl.cpl" },
        { L"Change User Account Control settings", L"UserAccountControlSettings.exe", L"System and Security", L"UserAccountControlSettings.exe;User accounts;UAC;uac;user account control;slider;admin prompt" },
        { L"Edit the system environment variables", L"SystemPropertiesAdvanced.exe", L"System and Security", L"Edit environment variables;System variables;Env vars;System env vars;sysdm.cpl;environment variables;env;path;system variables;advanced system settings" },
        { L"Edit environment variables for your account", L"rundll32.exe sysdm.cpl,EditEnvironmentVariables", L"System and Security", L"User environment variables;User variables;Env vars;User env vars;sysdm.cpl" },
        { L"Change screen saver", L"control desk.cpl,,@screensaver", L"Control Panel", L"Turn screen saver on or off;Timeout;desk.cpl" },
        { L"Connect to a wireless display", L"ms-settings-connectabledevices:devicediscovery", L"System", L"Connect panel;Connectable devices;Connect to a wireless audio device;Device discovery" },
        { L"Microsoft Management Console", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Authorization Manager", L"azman.msc", L"Administrative Tools", L"mmc.exe;azman.msc" },
        { L"Certificates - Current User", L"certmgr.msc", L"Administrative Tools", L"mmc.exe;certmgr.msc" },
        { L"Certificates - Local Computer", L"certlm.msc", L"Administrative Tools", L"mmc.exe;certlm.msc" },
        { L"Component Services", L"comexp.msc", L"Administrative Tools", L"mmc.exe;comexp.msc;COM-Objects" },
        { L"Computer Management", L"compmgmt.msc", L"Administrative Tools", L"mmc.exe;compmgmt.msc;System Tools;Task Scheduler;Event Viewer;Shared Folders;Network sessions;SMB;Local Users and Groups;Performance Monitor;Device manager;PNP Device;Storage;Disk Management;Create and format hard disk partitions;GPT;MBR;Services;WMI Control;Windows Management Instrumentation" },
        { L"Device manager", L"devmgmt.msc", L"Administrative Tools", L"mmc.exe;devmgmt.msc;PNP Device" },
        { L"Disk Management", L"diskmgmt.msc", L"Administrative Tools", L"mmc.exe;diskmgmt.msc;Storage;Create and format hard disk partitions;GPT;MBR" },
        { L"Event Viewer", L"eventvwr.msc", L"Administrative Tools", L"mmc.exe;eventvwr.msc" },
        { L"Local Computer Policy", L"gpedit.msc", L"Administrative Tools", L"mmc.exe;gpedit.msc;Group Policy" },
        { L"IP Security Monitor", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"IP Security Policies on Local Computer", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Local Users and Groups", L"lusrmgr.msc", L"Administrative Tools", L"mmc.exe;lusrmgr.msc" },
        { L"Performance Monitor", L"perfmon.msc", L"Administrative Tools", L"mmc.exe;perfmon.msc" },
        { L"Print Management", L"printmanagement.msc", L"Administrative Tools", L"mmc.exe;printmanagement.msc;Printer Spooler" },
        { L"Resultant Set of Policy", L"rsop.msc", L"Administrative Tools", L"mmc.exe;rsop.msc" },
        { L"Security Configuration and Analysis", L"secpol.msc", L"Administrative Tools", L"mmc.exe;secpol.msc" },
        { L"Security Templates", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Services", L"services.msc", L"Administrative Tools", L"mmc.exe;services.msc" },
        { L"Shared Folders", L"fsmgmt.msc", L"Administrative Tools", L"mmc.exe;fsmgmt.msc;Network sessions" },
        { L"Task Scheduler", L"taskschd.msc", L"Administrative Tools", L"mmc.exe;taskschd.msc" },
        { L"TPM Management", L"tpm.msc", L"Administrative Tools", L"mmc.exe;tpm.msc" },
        { L"Windows Defender Firewall with Advanced Security", L"wf.msc", L"Administrative Tools", L"mmc.exe;WF.msc" },
        { L"WMI Control", L"wmimgmt.msc", L"Administrative Tools", L"mmc.exe;WmiMgmt.msc;Windows Management Instrumentation" },

        // Deep Administrative Tools & Control Applets
        { L"Turn Windows features on or off", L"OptionalFeatures.exe", L"System and Security", L"OptionalFeatures.exe;windows features;optional features;hyper-v;wsl;windows subsystem for linux;iis;sandbox;windows sandbox;telnet;smb1;.net framework;tftp;virtual machine platform;features" },
        { L"System Configuration", L"msconfig.exe", L"Administrative Tools", L"msconfig.exe;msconfig;system configuration;boot options;safe mode;clean boot;services configuration;diagnostic startup;bootloader" },
        { L"Disk Cleanup", L"cleanmgr.exe", L"Administrative Tools", L"cleanmgr.exe;cleanmgr;disk cleanup;free up disk space;clean c drive;delete temp files;recycle bin cleanup;windows update cleanup" },
        { L"DirectX Diagnostic Tool", L"dxdiag.exe", L"Administrative Tools", L"dxdiag.exe;dxdiag;directx;directx diagnostic;gpu info;vram;system specifications;sound diagnostics;display diagnostics;graphics card info" },
        { L"User Accounts (Advanced)", L"netplwiz.exe", L"Administrative Tools", L"netplwiz.exe;netplwiz;control userpasswords2;userpasswords2;auto login;autologin;manage user accounts;local accounts;passwords;require sign in" },
        { L"Resource Monitor", L"resmon.exe", L"Administrative Tools", L"resmon.exe;resmon;resource monitor;cpu monitor;memory monitor;disk activity;network activity;handles;modules;processes" },
        { L"Registry Editor", L"regedit.exe", L"Administrative Tools", L"regedit.exe;regedit;registry;reg;hkey_local_machine;hkey_current_user;regedit32" },
        { L"Display Color Calibration", L"dccw.exe", L"Appearance and Personalization", L"dccw.exe;dccw;color calibration;calibrate display;gamma;color balance;brightness;contrast;calibrate monitor" },
        { L"Color Management", L"colorcpl.exe", L"Appearance and Personalization", L"colorcpl.exe;color management;icc profile;icm;monitor color;color profiles;display profile" },
        { L"Windows Memory Diagnostic", L"mdsched.exe", L"Administrative Tools", L"mdsched.exe;mdsched;memory test;ram test;check ram;memory diagnostic;ram errors" },
        { L"Sound Control Panel (Legacy)", L"mmsys.cpl", L"Hardware and Sound", L"mmsys.cpl;sound control panel;playback devices;recording devices;stereo mix;audio settings legacy;communications;sounds;exclusive mode;bitrate;sample rate" },
        { L"Internet Properties", L"inetcpl.cpl", L"Network and Internet", L"inetcpl.cpl;internet options;proxy settings;trusted sites;certificates;tls;ssl;connections;lan settings" },
        { L"Game Controllers", L"joy.cpl", L"Hardware and Sound", L"joy.cpl;game controllers;gamepad;joystick;calibrate controller;controller test;usb gamepad" },
        { L"Date and Time (Legacy)", L"timedate.cpl", L"Clock and Region", L"timedate.cpl;date and time;internet time;ntp;sync time;additional clocks;time zone" },
        { L"Region (Legacy)", L"intl.cpl", L"Clock and Region", L"intl.cpl;region;formats;short date;long date;administrative region;system locale;non-unicode" },
        { L"Power Options (Legacy)", L"powercfg.cpl", L"System and Security", L"powercfg.cpl;power options;power plan;high performance;ultimate performance;balanced;sleep timer;turn off display;choose what the power button does;fast startup" },
        { L"Programs and Features (Legacy)", L"appwiz.cpl", L"Programs", L"appwiz.cpl;uninstall a program;programs and features;classic uninstall;installed updates;view installed updates" },
        { L"System Properties (Legacy)", L"sysdm.cpl", L"System and Security", L"sysdm.cpl;system properties;computer name;hardware;device manager;advanced system settings;system protection;system restore;pagefile;virtual memory;remote" },
        { L"Windows Defender Firewall (Legacy)", L"firewall.cpl", L"System and Security", L"firewall.cpl;firewall;allow an app through firewall;inbound rules;outbound rules;turn firewall on or off;network firewall" },
        { L"Devices and Printers (Classic)", L"control printers", L"Hardware and Sound", L"control printers;printers;classic printers;devices and printers;add printer;printer properties;print queue" },
        { L"Remote Desktop Connection", L"mstsc.exe", L"System and Security", L"mstsc.exe;mstsc;remote desktop;rdp;remote connect;terminal services" },
        { L"About Windows (Winver)", L"winver.exe", L"System", L"winver.exe;winver;windows version;os build;build number;windows 11 edition" },
        { L"Performance Options", L"SystemPropertiesPerformance.exe", L"System and Security", L"SystemPropertiesPerformance.exe;performance options;visual effects;adjust for best performance;smooth edges of screen fonts;virtual memory;page file;paging file size" },
        { L"System Protection (System Restore)", L"SystemPropertiesProtection.exe", L"System and Security", L"SystemPropertiesProtection.exe;system protection;system restore;create restore point;configure restore;restore points" },
        { L"System Properties - Computer Name", L"SystemPropertiesComputerName.exe", L"System and Security", L"SystemPropertiesComputerName.exe;rename pc;workgroup;domain;full computer name" },
        { L"System Properties - Hardware", L"SystemPropertiesHardware.exe", L"Hardware and Sound", L"SystemPropertiesHardware.exe;device installation settings;hardware wizard" },
        { L"System Properties - Remote", L"SystemPropertiesRemote.exe", L"System and Security", L"SystemPropertiesRemote.exe;remote desktop;remote assistance;allow remote connections" },
        { L"Data Execution Prevention (DEP)", L"SystemPropertiesDataExecutionPrevention.exe", L"System and Security", L"SystemPropertiesDataExecutionPrevention.exe;dep;data execution prevention;turn on dep" },
        { L"God Mode (All Tasks)", L"explorer.exe shell:::{ED7BA470-8E54-465E-825C-99712043E01C}", L"Administrative Tools", L"god mode;godmode;all tasks;all settings;master control panel;every setting" },

        // Modern Windows 11 Deep Settings URIs
        { L"Core Isolation (Memory Integrity)", L"ms-settings:coreisolation", L"Update and security", L"ms-settings:coreisolation;core isolation;memory integrity;hvci;hypervisor;vbs;virtualization-based security;driver blacklist;vulnerable driver blocklist;security processor" },
        { L"Exploit Protection", L"ms-settings:exploitprotection", L"Update and security", L"ms-settings:exploitprotection;exploit protection;dep;cfg;aslr;system exploit protection;mitigations" },
        { L"Sound Devices & Properties", L"ms-settings:sound-devices", L"System", L"ms-settings:sound-devices;sound devices;output devices;input devices;default playback device;sample rate;bitrate;audio enhancement;audio properties" },
        { L"Encrypted DNS & IP Settings", L"ms-settings:network-ethernet", L"Network and Internet", L"ms-settings:network-ethernet;dns;encrypted dns;dns over https;doh;static ip;dhcp;ipv4;ipv6;gateway;subnet mask;ip address" },
        { L"Installed Apps (Windows 11)", L"ms-settings:installed-apps", L"Apps", L"ms-settings:installed-apps;installed apps;uninstall apps;remove apps;installed programs;manage apps" },
    };
    return kSettings;
}

} // namespace settings
// ===========================================================================
// Apps & Settings Indexer
// ===========================================================================
// An index of installed applications, from the shell's AppsFolder.
//
// AppsFolder is the virtual folder that unions Win32 shortcuts and Store
// apps, so one enumeration covers both. Measured on a real machine: 187 apps,
// ~200 ms to enumerate names but ~9.7 ms per icon -- which is why names are
// enumerated once up front and icons are fetched lazily, for visible rows
// only.
//
// Enumeration runs in-process on the background search thread in
// StartMenuExperienceHost at Medium integrity.

#include <cwctype>
#include <memory>
#include <mutex>
#include <sstream>

namespace apps {

// PKEY_Link_TargetParsingPath. Not from propkey.h: after initguid.h, that
// header would define every key it declares.
inline constexpr PROPERTYKEY kLinkTargetParsingPath = {
    {0xb9b4b3fc, 0x2b51, 0x4a42, {0xb5, 0xd8, 0x32, 0x41, 0x46, 0xaf, 0xcf, 0x25}}, 2};

namespace detail {

struct PidlDeleter {
    void operator()(ITEMIDLIST* p) const noexcept {
        if (p) {
            CoTaskMemFree(p);
        }
    }
};

}  // namespace detail

using UniquePidl = std::unique_ptr<ITEMIDLIST, detail::PidlDeleter>;

struct App {
    std::wstring name;
    std::wstring nameLower;        // precomputed, so filtering never allocates
    std::wstring targetPath;       // file path, AUMID, or ms-settings: URI
    std::wstring targetPathLower;  // precomputed lowercase target path
    std::wstring linkTarget;       // the program an app-ID entry's shortcut runs, if the shell knows it
    std::wstring exeNameLower;     // executable / command name (e.g. "cmd", "wt", "calc")
    std::wstring acronym;          // acronym from name words (e.g. "cp" for Command Prompt)
    std::vector<std::wstring> words;   // individual words in name
    std::vector<std::wstring> aliases; // smart aliases / keywords
    UniquePidl pidl;
    bool isSetting = false;        // true if Windows Setting or Control Panel page
    std::wstring area;             // Category/Area (e.g. "System", "Devices", "Personalization")
};

struct Match {
    const App* app;
    int score;  // lower is better
    int rank;   // the score less the head start for use (UsageBonus): the order shown
};

// How often each app was opened from Start, by lowercase name -- names are
// what identify an entry across index rebuilds. Owned by the search thread,
// kept in the mod's storage.
using UsageCounts = std::unordered_map<std::wstring, int>;

constexpr wchar_t kUsageValueName[] = L"appLaunchCounts";
constexpr size_t kUsageKept = 300;   // most used first; keeps the value small
constexpr int kUsageCap = 1000;

// A head start, in score points, for apps opened often, applied only among
// matches of the same kind (MatchTier): an often opened name prefix moves up
// past other prefixes, never past an exact match. 3 points after one launch,
// 9 at most, from about seven.
inline int UsageBonus(const UsageCounts* usage, const std::wstring& nameLower) {
    if (!usage) {
        return 0;
    }
    auto it = usage->find(nameLower);
    if (it == usage->end() || it->second <= 0) {
        return 0;
    }
    return std::min(9, static_cast<int>(std::lround(3.0 * std::log2(1.0 + it->second))));
}

// The kind of match a ScoreApp score stands for: exact (name, program, alias),
// prefix, acronym or substring, fuzzy, typo.
inline int MatchTier(int score) {
    return score < 10 ? 0 : score < 30 ? 1 : score < 80 ? 2 : score < 120 ? 3 : 4;
}

// One line per app: count, a tab, the lowercase name.
inline UsageCounts LoadUsage() {
    UsageCounts usage;
    std::vector<wchar_t> buffer(64 * 1024);
    size_t length = Wh_GetStringValue(kUsageValueName, buffer.data(), buffer.size());
    std::wstring text(buffer.data(), length);
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        size_t tab = text.find(L'\t', pos);
        if (tab != std::wstring::npos && tab < end) {
            int count = _wtoi(text.substr(pos, tab - pos).c_str());
            std::wstring name = text.substr(tab + 1, end - tab - 1);
            if (count > 0 && !name.empty()) {
                usage[name] = std::min(count, kUsageCap);
            }
        }
        pos = end + 1;
    }
    return usage;
}

inline void SaveUsage(const UsageCounts& usage) {
    std::vector<std::pair<int, std::wstring>> sorted;
    sorted.reserve(usage.size());
    for (const auto& [name, count] : usage) {
        sorted.emplace_back(count, name);
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    if (sorted.size() > kUsageKept) {
        sorted.resize(kUsageKept);
    }
    std::wstring text;
    for (const auto& [count, name] : sorted) {
        text += std::to_wstring(count) + L'\t' + name + L'\n';
    }
    Wh_SetStringValue(kUsageValueName, text.c_str());
}

inline std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

inline std::wstring ResolveAppParsingPath(const std::wstring& parse) {
    if (parse.empty()) return L"";
    if (parse.front() == L'{') {
        size_t closing = parse.find(L'}');
        if (closing != std::wstring::npos) {
            std::wstring guidStr = parse.substr(0, closing + 1);
            GUID guid{};
            if (SUCCEEDED(IIDFromString(guidStr.c_str(), &guid))) {
                PWSTR kfPath = nullptr;
                if (SUCCEEDED(SHGetKnownFolderPath(guid, 0, NULL, &kfPath)) && kfPath) {
                    std::wstring resolved = kfPath;
                    CoTaskMemFree(kfPath);
                    if (closing + 1 < parse.size()) {
                        resolved += parse.substr(closing + 1);
                    }
                    return resolved;
                }
            }
        }
    }
    return parse;
}

inline std::wstring ResolveLnkTarget(const std::wstring& lnkPath) {
    if (lnkPath.size() < 4) return L"";
    std::wstring ext = lnkPath.substr(lnkPath.size() - 4);
    for (auto& c : ext) c = static_cast<wchar_t>(towlower(c));
    if (ext != L".lnk") return L"";

    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl)))) {
        return L"";
    }
    IPersistFile* ppf = nullptr;
    std::wstring result;
    if (SUCCEEDED(psl->QueryInterface(IID_PPV_ARGS(&ppf)))) {
        if (SUCCEEDED(ppf->Load(lnkPath.c_str(), STGM_READ))) {
            wchar_t szTarget[MAX_PATH] = {};
            if (SUCCEEDED(psl->GetPath(szTarget, MAX_PATH, nullptr, 0)) && szTarget[0]) {
                result = szTarget;
            }
        }
        ppf->Release();
    }
    psl->Release();
    return result;
}

inline std::wstring ExtractExeName(const std::wstring& path) {
    if (path.empty()) return L"";
    if (path.starts_with(L"ms-settings:")) {
        std::wstring sub = path.substr(12);
        size_t q = sub.find_first_of(L"?#");
        if (q != std::wstring::npos) sub = sub.substr(0, q);
        return sub;
    }
    if (path.starts_with(L"control ")) {
        return L"control";
    }
    std::wstring lower = ToLower(path);
    size_t exePos = lower.rfind(L".exe");
    if (exePos != std::wstring::npos && exePos > 0) {
        size_t start = lower.find_last_of(L"\\/._", exePos - 1);
        if (start == std::wstring::npos) {
            return lower.substr(0, exePos);
        } else {
            return lower.substr(start + 1, exePos - (start + 1));
        }
    }
    size_t lastSlash = path.find_last_of(L"\\/");
    std::wstring filename = (lastSlash != std::wstring::npos) ? path.substr(lastSlash + 1) : path;
    // A packaged app's ID (Microsoft.WindowsNotepad_8wekyb3d8bbwe!App): the
    // name is the package's last dotted part, not what precedes the first dot.
    size_t bang = filename.find_last_of(L'!');
    if (bang != std::wstring::npos) {
        std::wstring pkg = filename.substr(0, std::min(bang, filename.find_first_of(L'_')));
        size_t dotInPkg = pkg.find_last_of(L'.');
        return dotInPkg != std::wstring::npos ? pkg.substr(dotInPkg + 1) : pkg;
    }
    size_t dot = filename.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        return filename.substr(0, dot);
    }
    return filename;
}

inline void PopulateAppAliases(App& a) {
    if (a.exeNameLower.empty()) {
        if (a.nameLower == L"command prompt") a.exeNameLower = L"cmd";
        else if (a.nameLower.find(L"powershell") != std::wstring::npos) a.exeNameLower = L"powershell";
        else if (a.nameLower == L"terminal" || a.nameLower == L"windows terminal") a.exeNameLower = L"wt";
        else if (a.nameLower == L"task manager") a.exeNameLower = L"taskmgr";
        else if (a.nameLower == L"registry editor") a.exeNameLower = L"regedit";
        else if (a.nameLower == L"calculator") a.exeNameLower = L"calc";
        else if (a.nameLower == L"control panel") a.exeNameLower = L"control";
        else if (a.nameLower == L"file explorer") a.exeNameLower = L"explorer";
        else if (a.nameLower == L"notepad") a.exeNameLower = L"notepad";
        else if (a.nameLower == L"paint") a.exeNameLower = L"mspaint";
        else if (a.nameLower.find(L"snipping") != std::wstring::npos) a.exeNameLower = L"snippingtool";
        else if (a.nameLower.find(L"remote desktop") != std::wstring::npos) a.exeNameLower = L"mstsc";
        else if (a.nameLower.find(L"visual studio code") != std::wstring::npos) a.exeNameLower = L"code";
    }

    if (a.targetPath.empty()) {
        if (a.exeNameLower == L"cmd") a.targetPath = L"C:\\Windows\\System32\\cmd.exe";
        else if (a.exeNameLower == L"powershell") a.targetPath = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
        else if (a.exeNameLower == L"taskmgr") a.targetPath = L"C:\\Windows\\System32\\Taskmgr.exe";
        else if (a.exeNameLower == L"regedit") a.targetPath = L"C:\\Windows\\regedit.exe";
        else if (a.exeNameLower == L"control") a.targetPath = L"C:\\Windows\\System32\\control.exe";
        else if (a.exeNameLower == L"explorer") a.targetPath = L"C:\\Windows\\explorer.exe";
        else if (a.exeNameLower == L"notepad") a.targetPath = L"C:\\Windows\\System32\\notepad.exe";
        else if (a.exeNameLower == L"mspaint") a.targetPath = L"C:\\Windows\\System32\\mspaint.exe";
    }

    if (!a.exeNameLower.empty() && a.exeNameLower != a.nameLower) {
        a.aliases.push_back(a.exeNameLower);
    }

    if (a.exeNameLower == L"cmd" || a.nameLower == L"command prompt") {
        a.aliases.insert(a.aliases.end(), {L"cmd", L"cmd.exe", L"command", L"prompt", L"terminal", L"console", L"cli", L"shell", L"dos"});
    } else if (a.exeNameLower == L"powershell" || a.nameLower.find(L"powershell") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"powershell", L"pwsh", L"ps", L"posh", L"shell", L"terminal", L"console"});
    } else if (a.nameLower == L"terminal" || a.targetPathLower.find(L"windowsterminal") != std::wstring::npos || a.exeNameLower == L"wt") {
        a.aliases.insert(a.aliases.end(), {L"wt", L"wt.exe", L"terminal", L"windows terminal", L"bash", L"cmd", L"powershell", L"console"});
    } else if (a.exeNameLower == L"taskmgr" || a.nameLower == L"task manager") {
        a.aliases.insert(a.aliases.end(), {L"taskmgr", L"taskmgr.exe", L"task", L"tasks", L"process", L"processes", L"kill", L"performance"});
    } else if (a.exeNameLower == L"regedit" || a.nameLower == L"registry editor") {
        a.aliases.insert(a.aliases.end(), {L"regedit", L"regedit.exe", L"reg", L"registry"});
    } else if (a.exeNameLower == L"calc" || a.nameLower == L"calculator") {
        a.aliases.insert(a.aliases.end(), {L"calc", L"calc.exe", L"calculator", L"math"});
    } else if (a.exeNameLower == L"control" || a.nameLower == L"control panel") {
        a.aliases.insert(a.aliases.end(), {L"control", L"control.exe", L"control panel", L"cpl", L"settings"});
    } else if (a.exeNameLower == L"explorer" || a.nameLower == L"file explorer") {
        a.aliases.insert(a.aliases.end(), {L"explorer", L"explorer.exe", L"files", L"my pc", L"this pc"});
    } else if (a.exeNameLower == L"notepad" || a.nameLower == L"notepad") {
        a.aliases.insert(a.aliases.end(), {L"notepad", L"notepad.exe", L"text", L"editor", L"notes", L"txt"});
    } else if (a.exeNameLower == L"mspaint" || a.nameLower == L"paint") {
        a.aliases.insert(a.aliases.end(), {L"mspaint", L"paint", L"pbrush", L"draw", L"sketch"});
    } else if (a.nameLower.find(L"snipping") != std::wstring::npos || a.targetPathLower.find(L"snippingtool") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"snippingtool", L"snip", L"screenshot", L"capture", L"screen"});
    } else if (a.exeNameLower == L"mstsc" || a.nameLower.find(L"remote desktop") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"mstsc", L"mstsc.exe", L"rdp", L"remote"});
    } else if (a.exeNameLower == L"code" || a.nameLower.find(L"visual studio code") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"code", L"code.exe", L"vscode", L"vs code", L"ide", L"editor"});
    } else if (a.nameLower.find(L"device manager") != std::wstring::npos || a.targetPathLower.find(L"devmgmt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"devmgmt", L"devmgmt.msc", L"device manager", L"devices"});
    } else if (a.nameLower.find(L"disk management") != std::wstring::npos || a.targetPathLower.find(L"diskmgmt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"diskmgmt", L"diskmgmt.msc", L"partition", L"disk"});
    } else if (a.nameLower == L"services") {
        a.aliases.insert(a.aliases.end(), {L"services", L"services.msc"});
    } else if (a.nameLower.find(L"event viewer") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"eventvwr", L"eventvwr.msc", L"event viewer", L"logs"});
    } else if (a.nameLower.find(L"system configuration") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msconfig", L"msconfig.exe"});
    } else if (a.nameLower.find(L"system information") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msinfo32", L"msinfo"});
    } else if (a.nameLower == L"settings") {
        a.aliases.insert(a.aliases.end(), {L"settings", L"control", L"preferences", L"config"});
    } else if (a.nameLower == L"word" || a.exeNameLower == L"winword" || a.targetPathLower.find(L"winword") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"winword", L"winword.exe", L"word", L"doc", L"docx", L"document", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"winword";
    } else if (a.nameLower == L"excel" || a.exeNameLower == L"excel" || a.targetPathLower.find(L"excel") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"excel", L"excel.exe", L"xls", L"xlsx", L"sheet", L"spreadsheet", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"excel";
    } else if (a.nameLower == L"powerpoint" || a.exeNameLower == L"powerpnt" || a.targetPathLower.find(L"powerpnt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"powerpnt", L"powerpnt.exe", L"powerpoint", L"ppt", L"pptx", L"slides", L"presentation", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"powerpnt";
    } else if (a.nameLower == L"access" || a.exeNameLower == L"msaccess" || a.targetPathLower.find(L"msaccess") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msaccess", L"msaccess.exe", L"access", L"database", L"accdb", L"mdb", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"msaccess";
    } else if (a.nameLower == L"outlook" || a.exeNameLower == L"outlook" || a.targetPathLower.find(L"outlook") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"outlook", L"outlook.exe", L"mail", L"email", L"calendar", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"outlook";
    } else if (a.nameLower == L"onenote" || a.exeNameLower == L"onenote" || a.targetPathLower.find(L"onenote") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"onenote", L"onenote.exe", L"notes", L"notebook", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"onenote";
    }
}

inline int DamerauLevenshteinDistance(const std::wstring& s1, const std::wstring& s2, int maxDist = 2) {
    int len1 = static_cast<int>(s1.size());
    int len2 = static_cast<int>(s2.size());
    if (std::abs(len1 - len2) > maxDist) return maxDist + 1;
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    constexpr int kMaxDim = 40;
    if (len1 >= kMaxDim || len2 >= kMaxDim) return maxDist + 1;

    int d[kMaxDim + 1][kMaxDim + 1];
    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        int rowMin = 999;
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({
                d[i - 1][j] + 1,       // deletion
                d[i][j - 1] + 1,       // insertion
                d[i - 1][j - 1] + cost // substitution
            });
            // Transposition
            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + 1);
            }
            if (d[i][j] < rowMin) rowMin = d[i][j];
        }
        if (rowMin > maxDist) return maxDist + 1;
    }
    return d[len1][len2];
}

inline int ScoreApp(const App& app, const std::wstring& q) {
    if (q.empty()) return -1;
    int s = -1;

    // 1. Exact Name match
    if (app.nameLower == q) {
        s = 0;
    }
    // 2. Exact Exe Name match (e.g. q == "cmd")
    else if (!app.exeNameLower.empty() && app.exeNameLower == q) {
        s = 2;
    }
    // 3. Exact Alias match (e.g. q == "cmd", "wifi", "calc")
    else {
        for (const auto& al : app.aliases) {
            if (al == q) {
                s = 4;
                break;
            }
        }
    }

    // 4. Name starts with query
    if (s < 0) {
        if (app.nameLower.starts_with(q)) {
            s = 10;
        }
        // 5. Exe Name starts with query
        else if (!app.exeNameLower.empty() && app.exeNameLower.starts_with(q)) {
            s = 15;
        }
        // 6. Word in Name starts with query
        else {
            for (const auto& w : app.words) {
                if (w.starts_with(q)) {
                    s = 20;
                    break;
                }
            }
            // 7. Alias starts with query
            if (s < 0) {
                for (const auto& al : app.aliases) {
                    if (al.starts_with(q)) {
                        s = 25;
                        break;
                    }
                }
            }
        }
    }

    // 8. Exact Acronym match (e.g. "cp" -> "Command Prompt")
    if (s < 0) {
        if (!app.acronym.empty() && app.acronym == q) {
            s = 30;
        }
        // 9. Substring in Name
        else {
            size_t namePos = app.nameLower.find(q);
            if (namePos != std::wstring::npos) {
                s = 40 + static_cast<int>(std::min<size_t>(namePos, 20));
            }
            // 10. Substring in Exe / Target Path
            else if (!app.targetPathLower.empty() && app.targetPathLower.find(q) != std::wstring::npos) {
                s = 60;
            }
            // 11. Substring in any alias
            else {
                for (const auto& al : app.aliases) {
                    if (al.find(q) != std::wstring::npos) {
                        s = 70;
                        break;
                    }
                }
            }
        }
    }

    // 12. Fuzzy subsequence match on Name or Exe Name
    if (s < 0) {
        auto isSubsequence = [](const std::wstring& pattern, const std::wstring& text, int& gaps) -> bool {
            if (pattern.size() > text.size()) return false;
            size_t p = 0;
            size_t lastMatch = 0;
            gaps = 0;
            for (size_t t = 0; t < text.size() && p < pattern.size(); ++t) {
                if (pattern[p] == text[t]) {
                    if (p > 0) gaps += static_cast<int>(t - lastMatch - 1);
                    lastMatch = t;
                    ++p;
                }
            }
            return p == pattern.size();
        };

        // Letters the query leaves unmatched count against it too, so a typo
        // of a short name beats the same letters strewn through a long one:
        // "setings" is Settings, not File Converter Settings.
        auto spare = [&q](const std::wstring& text) {
            return static_cast<int>((text.size() - q.size()) / 4);
        };
        int gaps = 0;
        if (q.size() >= 2 && isSubsequence(q, app.nameLower, gaps)) {
            s = 80 + std::min(gaps + spare(app.nameLower), 30);
        } else if (q.size() >= 2 && !app.exeNameLower.empty() && isSubsequence(q, app.exeNameLower, gaps)) {
            s = 85 + std::min(gaps + spare(app.exeNameLower), 30);
        }
    }

    // 13. Typo-tolerant edit distance matching
    if (s < 0 && q.size() >= 3) {
        int maxDist = (q.size() <= 4) ? 1 : 2;
        int bestDist = 999;

        auto checkCandidate = [&](const std::wstring& cand) {
            if (cand.empty()) return;
            // Whole string distance
            int d = DamerauLevenshteinDistance(q, cand, maxDist);
            if (d <= maxDist && d < bestDist) {
                bestDist = d;
            }
            // Prefix distance if candidate is longer
            if (cand.size() > q.size()) {
                std::wstring candPfx = cand.substr(0, q.size());
                int pfxD = DamerauLevenshteinDistance(q, candPfx, maxDist);
                if (pfxD <= maxDist && pfxD < bestDist) {
                    bestDist = pfxD;
                }
                if (cand.size() > q.size() + 1) {
                    std::wstring candPfx1 = cand.substr(0, q.size() + 1);
                    int pfxD1 = DamerauLevenshteinDistance(q, candPfx1, maxDist);
                    if (pfxD1 <= maxDist && pfxD1 < bestDist) {
                        bestDist = pfxD1;
                    }
                }
            }
        };

        checkCandidate(app.nameLower);
        if (!app.exeNameLower.empty()) {
            checkCandidate(app.exeNameLower);
        }
        for (const auto& w : app.words) {
            checkCandidate(w);
        }
        for (const auto& al : app.aliases) {
            checkCandidate(al);
        }

        if (bestDist <= maxDist) {
            s = 120 + bestDist * 15;
        }
    }

    if (s >= 0 && app.isSetting) {
        s += 1; // tie-breaker: slightly prefer real apps on exact ties
    }
    return s;
}

class Index {
   public:
    Index() = default;
    Index(const Index&) = delete;
    Index& operator=(const Index&) = delete;

    bool Rebuild() {
        std::vector<App> fresh;

        IShellItem* folder = nullptr;
        HRESULT hr = SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr,
                                                 IID_PPV_ARGS(&folder));
        if (FAILED(hr) || !folder) {
            return false;
        }

        IEnumShellItems* e = nullptr;
        hr = folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&e));
        folder->Release();
        if (FAILED(hr) || !e) {
            return false;
        }

        IShellItem* item = nullptr;
        while (e->Next(1, &item, nullptr) == S_OK && item) {
            App a;
            LPWSTR display = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &display)) &&
                display) {
                a.name = display;
                CoTaskMemFree(display);
            }
            LPWSTR fsPath = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
                std::wstring resolved = ResolveLnkTarget(fsPath);
                if (!resolved.empty()) {
                    a.targetPath = resolved;
                } else {
                    a.targetPath = fsPath;
                }
                CoTaskMemFree(fsPath);
            }
            
            LPWSTR parse = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &parse)) && parse) {
                std::wstring resolvedParse = ResolveAppParsingPath(parse);
                if (a.targetPath.empty()) {
                    a.targetPath = resolvedParse;
                } else if (a.targetPath.size() >= 4 && 
                           ToLower(a.targetPath.substr(a.targetPath.size() - 4)) == L".lnk" &&
                           !resolvedParse.empty() && resolvedParse.front() != L'{') {
                    a.targetPath = resolvedParse;
                }
                CoTaskMemFree(parse);
            }

            // An app registered under an app ID (Windhawk's is
            // RamenSoftware.Windhawk) has the ID as its parsing name. For a
            // desktop app the shell still knows the program its shortcut
            // runs; a packaged app has none.
            if (a.targetPath.find(L":\\") == std::wstring::npos) {
                IShellItem2* item2 = nullptr;
                if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&item2))) && item2) {
                    LPWSTR target = nullptr;
                    if (SUCCEEDED(item2->GetString(kLinkTargetParsingPath, &target)) && target) {
                        a.linkTarget = target;
                        CoTaskMemFree(target);
                    }
                    item2->Release();
                }
            }

            ITEMIDLIST* pidl = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
                a.pidl.reset(pidl);
            }
            if (!a.name.empty() && a.pidl) {
                a.nameLower = ToLower(a.name);
                a.targetPathLower = ToLower(a.targetPath);
                a.exeNameLower = ToLower(ExtractExeName(a.linkTarget.empty() ? a.targetPath : a.linkTarget));

                std::wistringstream ss(a.nameLower);
                std::wstring word;
                while (ss >> word) {
                    while (!word.empty() && iswpunct(word.front())) word.erase(word.begin());
                    while (!word.empty() && iswpunct(word.back())) word.pop_back();
                    if (!word.empty()) {
                        a.words.push_back(word);
                        a.acronym.push_back(word[0]);
                    }
                }

                PopulateAppAliases(a);
                if (a.nameLower == L"settings" || a.nameLower == L"windows settings" ||
                    a.targetPathLower.find(L"immersivecontrolpanel") != std::wstring::npos ||
                    a.exeNameLower == L"systemsettings") {
                    a.isSetting = true;
                }
                fresh.push_back(std::move(a));
            }
            item->Release();
            item = nullptr;
        }
        e->Release();

        // Append Windows Settings
        for (const auto& s : settings::GetSettingsList()) {
            App a;
            a.name = s.name;
            a.targetPath = s.command;
            a.area = s.area;
            a.isSetting = true;
            a.nameLower = ToLower(a.name);
            a.targetPathLower = ToLower(a.targetPath);
            a.exeNameLower = ToLower(ExtractExeName(a.targetPath));

            std::wistringstream ss(a.nameLower);
            std::wstring word;
            while (ss >> word) {
                while (!word.empty() && iswpunct(word.front())) word.erase(word.begin());
                while (!word.empty() && iswpunct(word.back())) word.pop_back();
                if (!word.empty()) {
                    a.words.push_back(word);
                    a.acronym.push_back(word[0]);
                }
            }

            if (s.altNames && s.altNames[0]) {
                std::wistringstream alts(s.altNames);
                std::wstring alt;
                while (std::getline(alts, alt, L';')) {
                    if (!alt.empty()) {
                        a.aliases.push_back(ToLower(alt));
                    }
                }
            }
            if (!a.area.empty()) {
                a.aliases.push_back(ToLower(a.area));
            }

            fresh.push_back(std::move(a));
        }

        std::lock_guard<std::mutex> lock(mutex_);
        apps_ = std::move(fresh);
        return true;
    }

    size_t Count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return apps_.size();
    }

    std::vector<Match> Search(const std::wstring& query, size_t limit, const UsageCounts* usage) const {
        std::vector<Match> hits;
        if (query.empty()) {
            return hits;
        }
        const std::wstring q = ToLower(query);

        std::lock_guard<std::mutex> lock(mutex_);
        for (const App& a : apps_) {
            int score = ScoreApp(a, q);
            if (score >= 0) {
                hits.push_back({&a, score, score - UsageBonus(usage, a.nameLower)});
            }
        }

        std::stable_sort(hits.begin(), hits.end(),
                         [](const Match& x, const Match& y) {
                             if (MatchTier(x.score) != MatchTier(y.score)) {
                                 return x.score < y.score;
                             }
                             if (x.rank != y.rank) {
                                 return x.rank < y.rank;
                             }
                             if (x.score != y.score) {
                                 return x.score < y.score;
                             }
                             return x.app->name.size() < y.app->name.size();
                         });

        // What is shown:
        //  - one entry per name: many things are indexed twice, typically a
        //    Settings page and the Control Panel applet or app behind it
        //    (Display, Sound, Device Manager), and a second row with the same
        //    name only takes a slot from something else;
        //  - with a real match (below the fuzzy tiers at 80), no fuzzy ones:
        //    "calc" is Calculator, not Local Computer Policy;
        //  - with only fuzzy or typo matches, just those close to the best.
        // By the score itself: use reorders what is shown, never what is.
        const int kFuzzy = 80;
        int best = hits.empty() ? 0 : hits.front().score;
        for (const Match& m : hits) {
            best = std::min(best, m.score);
        }
        const int cutoff = best < kFuzzy ? kFuzzy : best + 12;
        std::vector<Match> shown;
        shown.reserve(std::min(limit, hits.size()));
        for (const Match& m : hits) {
            if (shown.size() >= limit) {
                break;
            }
            if (m.score >= cutoff) {
                continue;
            }
            bool seen = false;
            for (const Match& kept : shown) {
                if (kept.app->nameLower == m.app->nameLower) {
                    seen = true;
                    break;
                }
            }
            if (!seen) {
                shown.push_back(m);
            }
        }
        return shown;
    }

    // The app at position i, for work that walks the whole index (the icon
    // prefetch). Valid until the next Rebuild, which only the search thread
    // calls -- the same thread that walks it.
    const App* At(size_t i) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return i < apps_.size() ? &apps_[i] : nullptr;
    }

   private:
    mutable std::mutex mutex_;
    std::vector<App> apps_;
};

}  // namespace apps
// ===========================================================================
// Calculator & Power Tools
// ===========================================================================

#include <iphlpapi.h>
#include <cmath>
#include <iomanip>
#include <cctype>

#ifndef DROPEFFECT_COPY
#define DROPEFFECT_COPY 1
#endif
#ifndef DROPEFFECT_MOVE
#define DROPEFFECT_MOVE 2
#endif

namespace tools {

// ---------------------------------------------------------------------------
// 1. Clipboard Copy & Cut
// ---------------------------------------------------------------------------
inline bool CopyTextToClipboard(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text.c_str(), bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
    }
    CloseClipboard();
    return true;
}

inline bool CopyOrCutFileToClipboard(const std::wstring& filePath, bool isCut) {
    if (filePath.empty()) return false;
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();

    // 1. CF_HDROP (Shell file list)
    size_t pathLen = filePath.size();
    size_t dropFilesSize = sizeof(DROPFILES);
    size_t totalBytes = dropFilesSize + (pathLen + 2) * sizeof(wchar_t);

    HGLOBAL hDrop = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, totalBytes);
    if (hDrop) {
        char* pData = static_cast<char*>(GlobalLock(hDrop));
        if (pData) {
            DROPFILES* pDrop = reinterpret_cast<DROPFILES*>(pData);
            pDrop->pFiles = static_cast<DWORD>(dropFilesSize);
            pDrop->fWide = TRUE;
            wchar_t* pDest = reinterpret_cast<wchar_t*>(pData + dropFilesSize);
            memcpy(pDest, filePath.c_str(), pathLen * sizeof(wchar_t));
            pDest[pathLen] = L'\0';
            pDest[pathLen + 1] = L'\0';
            GlobalUnlock(hDrop);
            SetClipboardData(CF_HDROP, hDrop);
        } else {
            GlobalFree(hDrop);
        }
    }

    // 2. Preferred DropEffect (DROPEFFECT_COPY or DROPEFFECT_MOVE)
    UINT uDropEffect = RegisterClipboardFormatW(L"Preferred DropEffect");
    if (uDropEffect) {
        HGLOBAL hEffect = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hEffect) {
            DWORD* pEffect = static_cast<DWORD*>(GlobalLock(hEffect));
            if (pEffect) {
                *pEffect = isCut ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
                GlobalUnlock(hEffect);
                SetClipboardData(uDropEffect, hEffect);
            } else {
                GlobalFree(hEffect);
            }
        }
    }

    // 3. CF_UNICODETEXT (fallback for text fields & text editors)
    size_t textBytes = (pathLen + 1) * sizeof(wchar_t);
    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, textBytes);
    if (hText) {
        wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hText));
        if (pText) {
            memcpy(pText, filePath.c_str(), textBytes);
            GlobalUnlock(hText);
            SetClipboardData(CF_UNICODETEXT, hText);
        } else {
            GlobalFree(hText);
        }
    }

    CloseClipboard();
    return true;
}

inline bool CreateDesktopShortcut(const std::wstring& targetPath, const std::wstring& preferredName = L"") {
    if (targetPath.empty()) return false;
    if (targetPath.starts_with(L"ms-settings:") || targetPath.find(L"immersivecontrolpanel") != std::wstring::npos) {
        return false;
    }

    PWSTR desktopFolder = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopFolder);
    if (FAILED(hr) || !desktopFolder) return false;

    std::wstring desktop = desktopFolder;
    CoTaskMemFree(desktopFolder);

    std::wstring baseName = preferredName;
    if (baseName.empty()) {
        size_t slash = targetPath.find_last_of(L"\\/");
        baseName = (slash != std::wstring::npos) ? targetPath.substr(slash + 1) : targetPath;
        if (baseName.size() > 4 && _wcsicmp(baseName.c_str() + baseName.size() - 4, L".lnk") == 0) {
            baseName.resize(baseName.size() - 4);
        } else if (baseName.size() > 4 && _wcsicmp(baseName.c_str() + baseName.size() - 4, L".exe") == 0) {
            baseName.resize(baseName.size() - 4);
        }
    }

    for (wchar_t& ch : baseName) {
        if (wcschr(L"\\/:*?\"<>|", ch)) {
            ch = L'_';
        }
    }
    while (!baseName.empty() && (baseName.back() == L' ' || baseName.back() == L'.')) {
        baseName.pop_back();
    }
    size_t first = baseName.find_first_not_of(L' ');
    if (first != std::wstring::npos && first > 0) {
        baseName = baseName.substr(first);
    }
    if (baseName.empty()) baseName = L"Shortcut";

    // A unique name, because neither write path below asks before replacing
    // what is already on the desktop: CopyFileW's third argument is
    // bFailIfExists, and IPersistFile::Save always overwrites. An installer's
    // own "Google Chrome.lnk" may carry a profile argument, a custom icon or a
    // hotkey, and replacing it with a bare link to the exe loses all of that
    // with no prompt and no way back.
    //
    // PathYetAnotherMakeUniqueName yields "Name.lnk", then "Name (2).lnk", and
    // so on -- the same convention Explorer's own "Create shortcut" uses.
    // The trailing backslash is load-bearing. Without one this treats the last
    // component as a file name and resolves against the *parent*, so passing
    // "C:\Users\me\Desktop" quietly writes to "C:\Users\me". SHGetKnownFolderPath
    // returns the path without it, so it has to be added here.
    std::wstring spec = baseName + L".lnk";
    std::wstring desktopDir = desktop + L"\\";
    WCHAR unique[MAX_PATH];
    if (!PathYetAnotherMakeUniqueName(unique, desktopDir.c_str(), nullptr,
                                      spec.c_str())) {
        Wh_Log(L"No unique shortcut name for %s", spec.c_str());
        return false;
    }
    std::wstring shortcutFile = unique;

    if (targetPath.size() > 4 && _wcsicmp(targetPath.c_str() + targetPath.size() - 4, L".lnk") == 0) {
        if (GetFileAttributesW(targetPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            // Fail rather than overwrite. The name was unique a moment ago, so
            // this only fires if something else got there in between.
            if (CopyFileW(targetPath.c_str(), shortcutFile.c_str(), TRUE)) {
                return true;
            }
        }
    }

    IShellLinkW* psl = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void**>(&psl));
    if (FAILED(hr) || !psl) return false;

    // Logged rather than fatal. SetPath is documented for file-system paths,
    // and UWP entries come through here as "shell:AppsFolder\<AUMID>", so a
    // failure here is exactly the case worth seeing in a log -- but refusing
    // outright would stop creating links that may be working today, which is
    // not something to change without testing it.
    hr = psl->SetPath(targetPath.c_str());
    if (FAILED(hr)) {
        Wh_Log(L"SetPath(%s) failed: %08X", targetPath.c_str(),
               static_cast<unsigned>(hr));
    }

    size_t lastSlash = targetPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos && !targetPath.starts_with(L"shell:")) {
        std::wstring dir = targetPath.substr(0, lastSlash);
        psl->SetWorkingDirectory(dir.c_str());
    }

    IPersistFile* ppf = nullptr;
    hr = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&ppf));
    bool success = false;
    if (SUCCEEDED(hr) && ppf) {
        hr = ppf->Save(shortcutFile.c_str(), TRUE);
        success = SUCCEEDED(hr);
        ppf->Release();
    }
    psl->Release();
    return success;
}

// ---------------------------------------------------------------------------
// 2. Local IPv4 Resolver & Network Interfaces
// ---------------------------------------------------------------------------
struct LocalIPv4Info {
    std::wstring ip;
    std::wstring adapterType;
};

inline std::vector<LocalIPv4Info> GetLocalIPv4Addresses() {
    std::vector<LocalIPv4Info> list;
    ULONG size = 0;
    if (GetIpAddrTable(nullptr, &size, FALSE) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        PMIB_IPADDRTABLE pTable = reinterpret_cast<PMIB_IPADDRTABLE>(buf.data());
        if (GetIpAddrTable(pTable, &size, FALSE) == NO_ERROR) {
            for (DWORD i = 0; i < pTable->dwNumEntries; ++i) {
                DWORD addr = pTable->table[i].dwAddr;
                BYTE b1 = static_cast<BYTE>((addr >> 0) & 0xFF);
                BYTE b2 = static_cast<BYTE>((addr >> 8) & 0xFF);
                BYTE b3 = static_cast<BYTE>((addr >> 16) & 0xFF);
                BYTE b4 = static_cast<BYTE>((addr >> 24) & 0xFF);

                // Filter loopback (127.x), zero (0.0.0.0), APIPA (169.254.x)
                if (b1 == 127 || b1 == 0 || (b1 == 169 && b2 == 254)) {
                    continue;
                }

                wchar_t ipBuf[64];
                swprintf_s(ipBuf, L"%u.%u.%u.%u", b1, b2, b3, b4);

                std::wstring type = L"Local IPv4";
                if (b1 == 192 && b2 == 168) {
                    type = L"Wi-Fi / LAN (192.168.x.x)";
                } else if (b1 == 10) {
                    type = L"Private Network (10.x.x.x)";
                } else if (b1 == 172 && (b2 >= 16 && b2 <= 31)) {
                    type = L"Private Network (172.16.x.x)";
                } else if (b1 == 26) {
                    type = L"Virtual Network / VPN";
                }

                list.push_back({ipBuf, type});
            }
        }
    }
    return list;
}

struct NetworkInterfaceInfo {
    std::wstring ip;
    std::wstring mask;
    std::wstring gateway;
    std::wstring adapterName;
    std::wstring typeLabel;
    std::wstring glyph;
};

inline std::vector<NetworkInterfaceInfo> GetNetworkInterfaces() {
    std::vector<NetworkInterfaceInfo> list;
    ULONG size = 0;
    DWORD ret = GetAdaptersInfo(nullptr, &size);
    if (ret == ERROR_BUFFER_OVERFLOW && size > 0) {
        std::vector<BYTE> buf(size);
        PIP_ADAPTER_INFO pInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buf.data());
        if (GetAdaptersInfo(pInfo, &size) == NO_ERROR) {
            for (PIP_ADAPTER_INFO pCurr = pInfo; pCurr != nullptr; pCurr = pCurr->Next) {
                std::string desc = pCurr->Description;
                std::string descLower = desc;
                for (char& c : descLower) c = static_cast<char>(tolower(c));

                std::wstring typeLabel = L"Network";
                std::wstring glyph = L"\uE701";

                if (pCurr->Type == 71 || descLower.find("wi-fi") != std::string::npos || descLower.find("wireless") != std::string::npos) {
                    typeLabel = L"Wi-Fi";
                    glyph = L"\uE704";
                } else if (descLower.find("vpn") != std::string::npos || descLower.find("radmin") != std::string::npos || descLower.find("wireguard") != std::string::npos || descLower.find("tailscale") != std::string::npos || descLower.find("tap") != std::string::npos) {
                    typeLabel = L"VPN";
                    glyph = L"\uE701";
                } else if (pCurr->Type == 6) {
                    typeLabel = L"Ethernet";
                    glyph = L"\uE839";
                }

                int wlen = MultiByteToWideChar(CP_ACP, 0, desc.c_str(), -1, nullptr, 0);
                std::wstring wDesc(wlen > 1 ? wlen - 1 : 0, L'\0');
                if (wlen > 1) {
                    MultiByteToWideChar(CP_ACP, 0, desc.c_str(), -1, &wDesc[0], wlen);
                }

                std::string gw = (pCurr->GatewayList.IpAddress.String[0] != '\0' && strcmp(pCurr->GatewayList.IpAddress.String, "0.0.0.0") != 0) ? pCurr->GatewayList.IpAddress.String : "";
                std::wstring wGw(gw.begin(), gw.end());

                for (IP_ADDR_STRING* pIp = &(pCurr->IpAddressList); pIp != nullptr; pIp = pIp->Next) {
                    std::string ipStr = pIp->IpAddress.String;
                    if (ipStr.empty() || ipStr == "0.0.0.0" || ipStr.starts_with("127.")) {
                        continue;
                    }
                    std::string maskStr = pIp->IpMask.String;
                    std::wstring wIp(ipStr.begin(), ipStr.end());
                    std::wstring wMask(maskStr.begin(), maskStr.end());

                    list.push_back({
                        wIp,
                        wMask,
                        wGw,
                        wDesc,
                        typeLabel,
                        glyph
                    });
                }
            }
        }
    }

    if (list.empty()) {
        auto fallback = GetLocalIPv4Addresses();
        for (const auto& fb : fallback) {
            list.push_back({
                fb.ip,
                L"",
                L"",
                fb.adapterType,
                L"Local IPv4",
                L"\uE701"
            });
        }
    }
    return list;
}

// ---------------------------------------------------------------------------
// 3. String & Number Formatting Helpers
// ---------------------------------------------------------------------------
inline std::wstring FormatCleanNumber(double val) {
    if (std::isnan(val) || std::isinf(val)) {
        return L"Error";
    }
    // Check if effectively integer
    double rounded = std::round(val);
    if (std::abs(val - rounded) < 1e-9 && std::abs(val) < 1e15) {
        return std::to_wstring(static_cast<long long>(rounded));
    }
    // Decimal formatting
    wchar_t buf[64];
    swprintf_s(buf, L"%.6f", val);
    std::wstring s = buf;
    while (!s.empty() && s.back() == L'0') s.pop_back();
    if (!s.empty() && s.back() == L'.') s.pop_back();
    return s;
}

inline std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = static_cast<wchar_t>(towlower(c));
    return res;
}

inline std::wstring Trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// ---------------------------------------------------------------------------
// 4. Math Expression Evaluator
// ---------------------------------------------------------------------------
namespace detail {

struct Token {
    enum Type { Number, Plus, Minus, Mul, Div, Mod, Pow, LParen, RParen, End, Ident } type;
    double value = 0.0;
    std::wstring text;
};

inline bool IsIdentChar(wchar_t c) {
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c == L'_');
}

inline std::vector<Token> Tokenize(const std::wstring& expr) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t n = expr.size();

    while (i < n) {
        wchar_t c = expr[i];
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }

        // Check for Hex prefix: 0x or 0X
        if (c == L'0' && i + 1 < n && (expr[i + 1] == L'x' || expr[i + 1] == L'X')) {
            size_t start = i + 2;
            size_t end = start;
            while (end < n && iswxdigit(expr[end])) end++;
            if (end > start) {
                std::wstring hexStr = expr.substr(start, end - start);
                wchar_t* pEnd = nullptr;
                unsigned long long val = wcstoull(hexStr.c_str(), &pEnd, 16);
                tokens.push_back({Token::Number, static_cast<double>(val), L""});
                i = end;
                continue;
            }
        }

        // Numbers: digits or '.' followed by digit
        if (iswdigit(c) || (c == L'.' && i + 1 < n && iswdigit(expr[i + 1]))) {
            size_t start = i;
            while (i < n && (iswdigit(expr[i]) || expr[i] == L'.')) i++;
            // Scientific notation: e+10, e-5
            if (i < n && (expr[i] == L'e' || expr[i] == L'E')) {
                i++;
                if (i < n && (expr[i] == L'+' || expr[i] == L'-')) i++;
                while (i < n && iswdigit(expr[i])) i++;
            }
            std::wstring numStr = expr.substr(start, i - start);
            wchar_t* pEnd = nullptr;
            double val = wcstod(numStr.c_str(), &pEnd);
            tokens.push_back({Token::Number, val, L""});
            continue;
        }

        // Operators & Parens
        if (c == L'+') { tokens.push_back({Token::Plus}); i++; continue; }
        if (c == L'-') { tokens.push_back({Token::Minus}); i++; continue; }
        if (c == L'*') { tokens.push_back({Token::Mul}); i++; continue; }
        if (c == L'x' || c == L'X') {
            // Can be multiply if preceded by a number or closing paren, and followed by whitespace or number
            if (!tokens.empty() && (tokens.back().type == Token::Number || tokens.back().type == Token::RParen)) {
                tokens.push_back({Token::Mul});
                i++;
                continue;
            }
        }
        if (c == L'/') { tokens.push_back({Token::Div}); i++; continue; }
        if (c == L'%') { tokens.push_back({Token::Mod}); i++; continue; }
        if (c == L'^') { tokens.push_back({Token::Pow}); i++; continue; }
        if (c == L'(') { tokens.push_back({Token::LParen}); i++; continue; }
        if (c == L')') { tokens.push_back({Token::RParen}); i++; continue; }

        // Identifiers / function names / constants
        if (IsIdentChar(c)) {
            size_t start = i;
            while (i < n && IsIdentChar(expr[i])) i++;
            std::wstring word = ToLower(expr.substr(start, i - start));
            if (word == L"pi") {
                tokens.push_back({Token::Number, 3.14159265358979323846, L""});
            } else if (word == L"e") {
                tokens.push_back({Token::Number, 2.71828182845904523536, L""});
            } else {
                tokens.push_back({Token::Ident, 0.0, word});
            }
            continue;
        }

        // Unrecognized character -> cannot parse
        return {};
    }

    tokens.push_back({Token::End});
    return tokens;
}

class Parser {
   public:
    explicit Parser(std::vector<Token> t) : tokens(std::move(t)), pos(0) {}

    bool Parse(double& result) {
        if (tokens.empty() || tokens.front().type == Token::End) return false;
        try {
            result = ParseExpression();
            return Current().type == Token::End && !std::isnan(result) && !std::isinf(result);
        } catch (...) {
            return false;
        }
    }

   private:
    const std::vector<Token> tokens;
    size_t pos;

    const Token& Current() const {
        return (pos < tokens.size()) ? tokens[pos] : tokens.back();
    }

    void Consume() {
        if (pos < tokens.size()) pos++;
    }

    // Expression = Term (('+' | '-') Term)*
    double ParseExpression() {
        double left = ParseTerm();
        while (true) {
            if (Current().type == Token::Plus) {
                Consume();
                left += ParseTerm();
            } else if (Current().type == Token::Minus) {
                Consume();
                left -= ParseTerm();
            } else {
                break;
            }
        }
        return left;
    }

    // Term = Power (('*' | '/' | '%') Power)*
    double ParseTerm() {
        double left = ParsePower();
        while (true) {
            if (Current().type == Token::Mul) {
                Consume();
                left *= ParsePower();
            } else if (Current().type == Token::Div) {
                Consume();
                double right = ParsePower();
                if (std::abs(right) < 1e-15) throw false;
                left /= right;
            } else if (Current().type == Token::Mod) {
                Consume();
                double right = ParsePower();
                if (std::abs(right) < 1e-15) throw false;
                left = std::fmod(left, right);
            } else {
                break;
            }
        }
        return left;
    }

    // Power = Factor ('^' Factor)*
    double ParsePower() {
        double left = ParseFactor();
        if (Current().type == Token::Pow) {
            Consume();
            double right = ParsePower(); // right associative
            left = std::pow(left, right);
        }
        return left;
    }

    // Factor = ('+' | '-')? Primary
    double ParseFactor() {
        if (Current().type == Token::Plus) {
            Consume();
            return ParseFactor();
        }
        if (Current().type == Token::Minus) {
            Consume();
            return -ParseFactor();
        }
        return ParsePrimary();
    }

    // Primary = Number | '(' Expression ')' | Ident '(' Expression ')'
    double ParsePrimary() {
        const Token& cur = Current();
        if (cur.type == Token::Number) {
            double v = cur.value;
            Consume();
            return v;
        }
        if (cur.type == Token::LParen) {
            Consume();
            double v = ParseExpression();
            if (Current().type != Token::RParen) throw false;
            Consume();
            return v;
        }
        if (cur.type == Token::Ident) {
            std::wstring fn = cur.text;
            Consume();
            if (Current().type != Token::LParen) throw false;
            Consume();
            double arg = ParseExpression();
            if (Current().type != Token::RParen) throw false;
            Consume();

            if (fn == L"sqrt") {
                if (arg < 0.0) throw false;
                return std::sqrt(arg);
            } else if (fn == L"cbrt") {
                return std::cbrt(arg);
            } else if (fn == L"abs") {
                return std::abs(arg);
            } else if (fn == L"sin") {
                return std::sin(arg);
            } else if (fn == L"cos") {
                return std::cos(arg);
            } else if (fn == L"tan") {
                return std::tan(arg);
            } else if (fn == L"log" || fn == L"log10") {
                if (arg <= 0.0) throw false;
                return std::log10(arg);
            } else if (fn == L"ln") {
                if (arg <= 0.0) throw false;
                return std::log(arg);
            } else if (fn == L"round") {
                return std::round(arg);
            } else if (fn == L"floor") {
                return std::floor(arg);
            } else if (fn == L"ceil") {
                return std::ceil(arg);
            }
            throw false;
        }
        throw false;
    }
};

} // namespace detail

inline bool IsStandaloneNumber(const std::wstring& input, double& outNum) {
    std::wstring t = Trim(input);
    if (t.empty()) return false;

    wchar_t* pEnd = nullptr;
    double val = wcstod(t.c_str(), &pEnd);
    if (pEnd && *pEnd == L'\0' && pEnd != t.c_str()) {
        outNum = val;
        return true;
    }
    return false;
}

inline bool EvaluateMath(const std::wstring& input, double& outResult) {
    std::wstring trimmed = Trim(input);
    if (trimmed.empty()) return false;

    // Handle "X% of Y" pattern
    std::wstring lower = ToLower(trimmed);
    size_t ofPos = lower.find(L"% of ");
    if (ofPos != std::wstring::npos) {
        std::wstring part1 = Trim(trimmed.substr(0, ofPos));
        std::wstring part2 = Trim(trimmed.substr(ofPos + 5));
        double pct = 0, base = 0;
        auto evalVal = [](const std::wstring& s, double& val) -> bool {
            if (IsStandaloneNumber(s, val)) return true;
            return EvaluateMath(s, val);
        };
        if (evalVal(part1, pct) && evalVal(part2, base)) {
            outResult = (pct / 100.0) * base;
            return true;
        }
    }

    auto tokens = detail::Tokenize(trimmed);
    if (tokens.size() <= 1) return false;

    // Ensure it contains at least one operator or function or paren, not just a standalone number
    bool hasOperator = false;
    for (const auto& t : tokens) {
        if (t.type != detail::Token::Number && t.type != detail::Token::End) {
            hasOperator = true;
            break;
        }
    }
    if (!hasOperator) return false;

    detail::Parser parser(std::move(tokens));
    return parser.Parse(outResult);
}

inline bool EvaluateConversionFormula(const std::wstring& formula, double n, double& outResult) {
    std::wstring expr = Trim(formula);
    if (expr.empty()) return false;

    // Check if formula is just a numeric multiplier (e.g. "0.621371")
    wchar_t* pEnd = nullptr;
    double mult = wcstod(expr.c_str(), &pEnd);
    if (pEnd && *pEnd == L'\0' && pEnd != expr.c_str()) {
        outResult = n * mult;
        return true;
    }

    std::wstring nStr = FormatCleanNumber(n);
    if (n < 0) nStr = L"(" + nStr + L")";

    auto replaceAll = [](std::wstring& s, const std::wstring& from, const std::wstring& to) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::wstring::npos) {
            s.replace(pos, from.length(), to);
            pos += to.length();
        }
    };

    replaceAll(expr, L"{n}", nStr);
    replaceAll(expr, L"{N}", nStr);
    replaceAll(expr, L"{x}", nStr);
    replaceAll(expr, L"{X}", nStr);

    for (size_t i = 0; i < expr.size(); ++i) {
        if (expr[i] == L'x' || expr[i] == L'X') {
            bool leftOk = (i == 0 || (!iswalnum(expr[i - 1]) && expr[i - 1] != L'_'));
            bool rightOk = (i + 1 == expr.size() || (!iswalnum(expr[i + 1]) && expr[i + 1] != L'_'));
            if (leftOk && rightOk) {
                expr.replace(i, 1, nStr);
                i += nStr.size() - 1;
            }
        }
    }

    return EvaluateMath(expr, outResult);
}

// ---------------------------------------------------------------------------
// 5. Standalone Number & Multi-Unit Conversions (/c)
// ---------------------------------------------------------------------------
inline std::wstring NormalizeUnit(const std::wstring& unitRaw) {
    std::wstring u = ToLower(Trim(unitRaw));
    if (u == L"c" || u == L"\u00B0c" || u == L"celsius") return L"c";
    if (u == L"f" || u == L"\u00B0f" || u == L"fahrenheit") return L"f";
    if (u == L"k" || u == L"kelvin") return L"k";
    if (u == L"km" || u == L"kilometer" || u == L"kilometers") return L"km";
    if (u == L"mi" || u == L"mile" || u == L"miles") return L"miles";
    if (u == L"m" || u == L"meter" || u == L"meters") return L"m";
    if (u == L"cm" || u == L"centimeter" || u == L"centimeters") return L"cm";
    if (u == L"mm" || u == L"millimeter" || u == L"millimeters") return L"mm";
    if (u == L"in" || u == L"inch" || u == L"inches" || u == L"\"") return L"in";
    if (u == L"ft" || u == L"foot" || u == L"feet" || u == L"'") return L"feet";
    if (u == L"yd" || u == L"yard" || u == L"yards") return L"yd";
    if (u == L"kg" || u == L"kilo" || u == L"kilogram" || u == L"kilograms") return L"kg";
    if (u == L"lb" || u == L"lbs" || u == L"pound" || u == L"pounds") return L"lbs";
    if (u == L"g" || u == L"gram" || u == L"grams") return L"g";
    if (u == L"oz" || u == L"ounce" || u == L"ounces") return L"oz";
    if (u == L"kmh" || u == L"km/h" || u == L"kph") return L"km/h";
    if (u == L"mph") return L"mph";
    if (u == L"b" || u == L"bytes" || u == L"byte") return L"bytes";
    if (u == L"kb" || u == L"kilobyte" || u == L"kilobytes") return L"kb";
    if (u == L"mb" || u == L"megabyte" || u == L"megabytes") return L"mb";
    if (u == L"gb" || u == L"gigabyte" || u == L"gigabytes") return L"gb";
    if (u == L"tb" || u == L"terabyte" || u == L"terabytes") return L"tb";
    if (u == L"s" || u == L"sec" || u == L"second" || u == L"seconds") return L"s";
    if (u == L"min" || u == L"minute" || u == L"minutes") return L"min";
    if (u == L"h" || u == L"hr" || u == L"hrs" || u == L"hour" || u == L"hours") return L"hours";
    if (u == L"d" || u == L"day" || u == L"days") return L"days";
    if (u == L"l" || u == L"liter" || u == L"liters") return L"l";
    if (u == L"gal" || u == L"gallon" || u == L"gallons") return L"gal";
    if (u == L"bar") return L"bar";
    if (u == L"psi") return L"psi";
    if (u == L"kw") return L"kw";
    if (u == L"hp") return L"hp";
    return u;
}

} // namespace tools

#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.Data.Pdf.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>

#pragma pop_macro("GetCurrentTime")

#include <robuffer.h>
#ifndef WH_MOD_ID
#define WH_MOD_ID L"start-everything"
#endif
#include <windhawk_utils.h>

namespace wf = winrt::Windows::Foundation;
namespace wut = winrt::Windows::UI::Text;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wui = winrt::Windows::UI::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wuxma = winrt::Windows::UI::Xaml::Media::Animation;

static std::atomic<bool> g_quit{false};

// ===========================================================================
// Domain: Process Identification
// ===========================================================================

enum class TargetProcess {
    Unknown,
    StartMenu,
    SearchHost,
    Explorer
};

static TargetProcess g_targetProcess = TargetProcess::Unknown;

TargetProcess IdentifyCurrentProcess() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? (name + 1) : path;
    if (_wcsicmp(name, L"StartMenuExperienceHost.exe") == 0) return TargetProcess::StartMenu;
    if (_wcsicmp(name, L"SearchHost.exe") == 0) return TargetProcess::SearchHost;
    if (_wcsicmp(name, L"explorer.exe") == 0) return TargetProcess::Explorer;
    return TargetProcess::Unknown;
}

// ===========================================================================
// Domain: explorer.exe (Start and Search window lookup)
// ===========================================================================
//
// Explorer's foreground calls are not hooked. Explorer only holds the right to
// change the foreground because it received the click or keypress, and
// AllowSetForegroundWindow hands that right over: a grant made before
// Explorer's own SetForegroundWindow left Explorer unable to make the switch,
// so with another app in front, clicking Start did nothing. Explorer switches
// to SearchHost exactly as it does without this mod, and SearchHost -- then
// the foreground process -- is what lets Start take over.

// Start publishes its CoreWindow on the taskbar when it attaches. Reading it
// back is a property lookup, and the PID comes from GetWindowThreadProcessId,
// which opens no process handle -- cheap enough to call on any activation.
static HWND CachedStartWindow() {
    static std::atomic<HWND> s_start{nullptr};
    HWND h = s_start.load(std::memory_order_relaxed);
    if (h && IsWindow(h)) {
        return h;
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    h = tray ? static_cast<HWND>(GetPropW(tray, L"WindhawkStartMenuHwnd")) : nullptr;
    if (h && !IsWindow(h)) {
        h = nullptr;
    }
    if (!h) {
        HWND hStart = FindWindowW(L"Windows.UI.Core.CoreWindow", L"Start");
        if (hStart && IsWindow(hStart)) {
            h = hStart;
            if (tray) {
                SetPropW(tray, L"WindhawkStartMenuHwnd", hStart);
            }
        }
    }
    if (!h) {
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            if (GetPropW(hwnd, L"WindhawkStartMenuWindow")) {
                *reinterpret_cast<HWND*>(lParam) = hwnd;
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&h));
        if (h && IsWindow(h) && tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", h);
        }
    }
    s_start.store(h, std::memory_order_relaxed);
    return h;
}

static inline HWND FindStartMenuCoreWindow() {
    return CachedStartWindow();
}

// Whether `hwnd` is SearchHost's CoreWindow. Called only on this mod's helper
// thread in Explorer and on Start's UI thread, never inside Explorer's own
// foreground path, so identifying a new SearchHost process by name once is
// fine.
static bool IsSearchHostWindow(HWND hwnd) {
    static std::atomic<DWORD> s_searchPid{0};
    static std::atomic<DWORD> s_otherPid{0};
    if (!hwnd) {
        return false;
    }
    wchar_t cls[32];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == s_otherPid.load(std::memory_order_relaxed)) {
        return false;
    }
    if (pid == s_searchPid.load(std::memory_order_relaxed)) {
        return true;
    }
    bool match = false;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        wchar_t path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            const wchar_t* name = wcsrchr(path, L'\\');
            match = _wcsicmp(name ? name + 1 : path, L"SearchHost.exe") == 0;
        }
        CloseHandle(process);
    }
    (match ? s_searchPid : s_otherPid).store(pid, std::memory_order_relaxed);
    return match;
}

// Explorer's helper sends this to Start's CoreWindow to ask whether a request
// to remove a Store app is really Start's (AcceptPackageRemoval): lParam is
// the family name's hash; Start answers 1 while it is asking for just that
// (RequestPackageRemoval).
static UINT RemovalCheckMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_RemovalCheck");
    return message;
}

// Explorer's helper posts this to Start's CoreWindow when a drag of a result
// it ran (StartFileDrag) is over: wParam 1 if it was dropped.
static UINT DragDoneMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_DragDone");
    return message;
}

// Start sends this to SearchHost's CoreWindow when SearchHost holds the
// foreground and Start cannot take it. See SearchHostSubclassProc.
static UINT StartForegroundRequestMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_StartForegroundRequest");
    return message;
}


// Tracked launch threads for clean unload synchronization across processes
static std::mutex g_launchHandlesMutex;
// Bumped by every launch once its shell call has returned, started or not
// (see DismissStartMenuForLaunch).
static std::atomic<unsigned> g_launchesDone{0};
// Closed while DismissStartMenuForLaunch makes sure Start holds the foreground,
// so that is when the program is started; open otherwise. Launches wait at it.
static HANDLE g_launchGate;
static void WaitForLaunchGate() {
    if (g_launchGate) {
        WaitForSingleObject(g_launchGate, 1000);
    }
}
static std::vector<HANDLE> g_launchHandles;

template <typename F>
static void SpawnTrackedLaunch(F&& f) {
    auto fnCopy = new std::decay_t<F>(std::forward<F>(f));
    HANDLE h = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
        auto pFn = reinterpret_cast<std::decay_t<F>*>(param);
        try {
            (*pFn)();
        } catch (...) {}
        delete pFn;
        return 0;
    }, fnCopy, 0, nullptr);

    if (h) {
        std::lock_guard<std::mutex> lock(g_launchHandlesMutex);
        g_launchHandles.erase(
            std::remove_if(g_launchHandles.begin(), g_launchHandles.end(),
                [](HANDLE handle) {
                    if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0) {
                        CloseHandle(handle);
                        return true;
                    }
                    return false;
                }),
            g_launchHandles.end()
        );
        g_launchHandles.push_back(h);
    } else {
        delete fnCopy;
    }
}

static void WaitForTrackedLaunches() {
    std::vector<HANDLE> handlesToJoin;
    {
        std::lock_guard<std::mutex> lock(g_launchHandlesMutex);
        handlesToJoin = std::move(g_launchHandles);
    }
    for (HANDLE h : handlesToJoin) {
        if (h) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
}

// ===========================================================================
// Domain: Explorer Shell Property Relay
// ===========================================================================

static const wchar_t kExplorerHelperClassName[] = L"StartEverything_ExplorerHostClass";
static const wchar_t kExplorerHelperWindowName[] = L"StartEverything_ExplorerHost";
static const ULONG_PTR kExplorerCopyDataMagic = 0x53455052; // 'SEPR'
static const ULONG_PTR kExplorerDragMagic = 0x53454447;      // 'SEDG': drag a result (StartFileDrag)
static const ULONG_PTR kExplorerUninstallMagic = 0x5345554E; // 'SEUN': remove a packaged app (RequestPackageRemoval)
static const UINT kExplorerStartDrag = WM_APP + 0x31;
static std::wstring g_explorerDragPath;  // helper thread only

static HANDLE g_hExplorerHelperThread = nullptr;
static DWORD g_explorerHelperThreadId = 0;
static HWND g_hExplorerHelperWnd = nullptr;
static HANDLE g_hExplorerHelperReadyEvent = nullptr;

// ---------------------------------------------------------------------------
// Search is never left showing without Start
//
// SearchHost is invisible under this mod, so whenever the shell shows Search
// on its own, the user sees nothing at all. Two things make it do that:
//
//   - Win+S and the taskbar search button, which ask for Search alone;
//   - a key typed in the few milliseconds after the Win key, before Start
//     holds the keyboard: SearchHost takes it as the start of a native search,
//     and the shell swaps Start out for Search -- Start vanishes mid-typing.
//
// Either way, once Search is showing, Start is closed and the user is still on
// one of the two, Start is opened -- and its search box is this mod's. The
// shell's hotkey, button and type-ahead handling are internal, so this watches
// what is public instead: the foreground moving to Search, and Start being
// cloaked (closed). A normal close never qualifies: by the time Start is
// cloaked the foreground is back on another app, and Search was cloaked first.
//
// Opening Start is a toggle, so it is sent only once Start is known to be
// closed, not on its way up or down. The Win key and the Start button open
// Start alongside Search, uncloaking it within ~30ms, so normally Start has to
// stay closed across several checks. Win+S, though, opens Search while the
// Win key is still held, and the Win key on its own opens Start only once it
// is released: under a held Win key one check is enough -- none at all if
// Start was already closed. If Start was open (Win+S over Start), this waits
// for it to finish closing first. When Start has just been cloaked, it is
// already closed, and one check is enough too.
//
// Start is opened with WM_SYSCOMMAND/SC_TASKLIST, the documented "activate the
// Start menu" command, from this helper thread: no injected keystrokes, and
// nothing runs in Explorer's own foreground path.
// ---------------------------------------------------------------------------
static const UINT_PTR kSearchAloneTimerId = 1;
static const UINT kSearchAloneTickMs = 80;
static const int kSearchAloneTicks = 3;
static const int kSearchAloneMaxTicks = 15;
static HWINEVENTHOOK g_searchForegroundHook = nullptr;
static HWINEVENTHOOK g_startCloakedHook = nullptr;
static bool g_searchAloneArmed = false;
static int g_searchAloneTicksNeeded = 0;
static int g_searchAloneClosedTicks = 0;  // consecutive checks with Start closed
static int g_searchAloneTotalTicks = 0;

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return hwnd && SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

static bool IsStartMenuClosed() {
    return IsCloaked(CachedStartWindow());
}

// SearchHost's CoreWindow, found by process: its title is localized.
static HWND FindSearchHostCoreWindow() {
    static HWND s_search = nullptr;  // helper thread only
    if (s_search && IsSearchHostWindow(s_search)) {
        return s_search;
    }
    s_search = nullptr;
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", nullptr); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) {
        if (IsSearchHostWindow(hwnd)) {
            s_search = hwnd;
            break;
        }
    }
    return s_search;
}

// Opening Start puts Search in front first, as every opening does, and Start
// is still cloaked at that moment -- which looks exactly like what this
// watches for. So for a moment after asking, nothing is asked again: a second
// SC_TASKLIST would toggle Start closed.
static const ULONGLONG kOpenStartSettleMs = 600;
static ULONGLONG g_openStartTick = 0;  // helper thread only

static bool OpeningStartJustAsked() {
    return g_openStartTick && GetTickCount64() - g_openStartTick < kOpenStartSettleMs;
}

static void OpenStartInsteadOfSearch() {
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        Wh_Log(L"[Explorer] Search showing without Start -> opening Start");
        g_openStartTick = GetTickCount64();
        PostMessageW(tray, WM_SYSCOMMAND, SC_TASKLIST, 0);
    }
}

// There can be several explorer.exe processes, each running this helper. Only
// the one running the taskbar acts, or Start would be toggled once per process.
static bool IsTaskbarProcess() {
    DWORD pid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &pid);
    }
    return pid == GetCurrentProcessId();
}

static void ArmSearchAloneCheck(int ticksNeeded) {
    g_searchAloneArmed = true;
    g_searchAloneTicksNeeded = ticksNeeded;
    g_searchAloneClosedTicks = 0;
    g_searchAloneTotalTicks = 0;
    SetTimer(g_hExplorerHelperWnd, kSearchAloneTimerId, kSearchAloneTickMs, nullptr);
}

static void CALLBACK ShellWindowEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                          LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !g_hExplorerHelperWnd ||
        OpeningStartJustAsked()) {
        return;
    }
    if (event == EVENT_OBJECT_CLOAKED) {
        // Start's cloak is the last step of a close, never a transition to
        // wait out: one confirming check is enough.
        if (hwnd == CachedStartWindow() && IsTaskbarProcess()) {
            ArmSearchAloneCheck(1);
        }
        return;
    }
    if (event != EVENT_SYSTEM_FOREGROUND || !IsSearchHostWindow(hwnd) || !IsTaskbarProcess()) {
        return;
    }
    bool winKeyDown = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
    if (winKeyDown && IsStartMenuClosed()) {
        KillTimer(g_hExplorerHelperWnd, kSearchAloneTimerId);
        g_searchAloneArmed = false;
        OpenStartInsteadOfSearch();
        return;
    }
    ArmSearchAloneCheck(winKeyDown ? 1 : kSearchAloneTicks);
}

// Returns true once there is nothing left to wait for.
static bool CheckSearchShownWithoutStart() {
    HWND search = FindSearchHostCoreWindow();
    HWND start = CachedStartWindow();
    HWND fg = GetForegroundWindow();
    if (!g_searchAloneArmed || !search || !start || (fg != search && fg != start) || IsCloaked(search) ||
        OpeningStartJustAsked()) {
        return true;  // the user moved on, Search is not showing, or Start is on its way
    }
    if (fg == start && !IsCloaked(start)) {
        return true;  // Start is open and has the keyboard
    }
    if (++g_searchAloneTotalTicks > kSearchAloneMaxTicks) {
        // What is left is an invisible Search holding the keyboard. Worth a
        // trace: the checks above assume timings that may not hold everywhere.
        Wh_Log(L"[Explorer] Search still in front after %d checks, Start %ls; giving up",
               kSearchAloneMaxTicks, IsCloaked(start) ? L"closed" : L"open");
        return true;
    }
    if (!IsCloaked(start)) {
        g_searchAloneClosedTicks = 0;  // Start is open, opening, or still closing
        return false;
    }
    if (++g_searchAloneClosedTicks < g_searchAloneTicksNeeded) {
        return false;
    }
    OpenStartInsteadOfSearch();
    return true;
}

static int PrimaryButton() {
    return GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
}

// Feeds a drag of a result (RunExplorerDrag) the mouse. The button was
// pressed in Start, and until it is released Windows sends the mouse to the
// window it was pressed on -- Start's -- whichever window holds the capture:
// the drag loop here would never hear of the moves or the release. So a
// thread watches the pointer and tells the loop: a move whenever it has
// moved, and the release once the button is up, each with where the pointer
// is -- the loop takes it from the message, as from a real one -- and the
// loop asks ExplorerDragSource whether to go on.
struct ExplorerDragFeed {
    DWORD loopThread = 0;
    std::atomic<bool> released{false};
    std::atomic<bool> done{false};
};

static DWORD WINAPI ExplorerDragFeedThread(LPVOID param) {
    auto* feed = static_cast<ExplorerDragFeed*>(param);
    POINT last{LONG_MIN, LONG_MIN};
    while (!feed->done.load()) {
        if (!(GetAsyncKeyState(PrimaryButton()) & 0x8000)) {
            feed->released.store(true);
        }
        GUITHREADINFO info{sizeof(info)};
        HWND loop = GetGUIThreadInfo(feed->loopThread, &info) ? info.hwndCapture : nullptr;
        POINT pt;
        if (loop && GetCursorPos(&pt)) {
            POINT client = pt;
            ScreenToClient(loop, &client);
            const LPARAM at = MAKELPARAM(client.x, client.y);
            if (feed->released.load()) {
                PostMessageW(loop, WM_LBUTTONUP, 0, at);
            } else if (pt.x != last.x || pt.y != last.y) {
                PostMessageW(loop, WM_MOUSEMOVE, MK_LBUTTON, at);
                last = pt;
            }
        }
        Sleep(feed->released.load() ? 50 : 10);
    }
    return 0;
}

// Ends a drag of a result when the button is up or Escape is pressed. Lives
// on RunExplorerDrag's stack.
struct ExplorerDragSource : IDropSource {
    ExplorerDragFeed* feed;
    explicit ExplorerDragSource(ExplorerDragFeed* f) : feed(f) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *ppv = static_cast<IDropSource*>(this);
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD) override {
        if (escape) {
            return DRAGDROP_S_CANCEL;
        }
        if (feed->released.load() || !(GetAsyncKeyState(PrimaryButton()) & 0x8000)) {
            return DRAGDROP_S_DROP;
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

// Runs a drag of a result for Start (StartFileDrag) with the shell's own data
// object and drag loop -- the same as dragging the file in File Explorer: the
// same formats for whatever it is dropped on, the same picture under the
// pointer -- but only ever as a copy, so a drag from search can never move a
// file. Then tells Start how it ended -- dropped, or cancelled with Escape
// or by letting go over Start itself -- and on a cancel hands it the
// keyboard back.
static void RunExplorerDrag(HWND hWnd) {
    std::wstring path = std::exchange(g_explorerDragPath, std::wstring());
    if (path.empty()) {
        return;
    }
    HRESULT ole = OleInitialize(nullptr);
    IShellItem* item = nullptr;
    IDataObject* data = nullptr;
    DWORD effect = DROPEFFECT_NONE;
    HRESULT hr = E_FAIL;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) && item &&
        SUCCEEDED(item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data))) && data) {
        ExplorerDragFeed feed;
        feed.loopThread = GetCurrentThreadId();
        ExplorerDragSource source(&feed);
        if (HANDLE feeder = CreateThread(nullptr, 0, ExplorerDragFeedThread, &feed, 0, nullptr)) {
            hr = SHDoDragDrop(hWnd, data, &source, DROPEFFECT_COPY, &effect);
            feed.done.store(true);
            WaitForSingleObject(feeder, INFINITE);
            CloseHandle(feeder);
        }
        Wh_Log(L"[Explorer] drag: %ls -> %08X, effect %lu", path.c_str(), static_cast<unsigned>(hr), effect);
    } else {
        Wh_Log(L"[Explorer] drag: could not open %ls", path.c_str());
    }
    if (data) {
        data->Release();
    }
    if (item) {
        item->Release();
    }
    if (SUCCEEDED(ole)) {
        OleUninitialize();
    }
    // Not the effect: a drop into a folder copies in the background and
    // reports none.
    HWND start = CachedStartWindow();
    POINT pt{};
    GetCursorPos(&pt);
    HWND under = WindowFromPoint(pt);
    const bool overStart = start && under && GetAncestor(under, GA_ROOT) == start;
    const bool dropped = hr == DRAGDROP_S_DROP && !overStart;
    if (start) {
        if (!dropped && !IsCloaked(start) && GetForegroundWindow() == hWnd) {
            SetForegroundWindow(start);
        }
        PostMessageW(start, DragDoneMessage(), dropped ? 1 : 0, 0);
    }
}

// The thread showing an uninstall error, while it does (AcceptPackageRemoval):
// StopExplorerHelperHost closes the box rather than wait for a click.
static std::atomic<DWORD> g_removalBoxThread{0};

// Removes a packaged app for this user, for Uninstall in an app's menu in
// Start (RequestPackageRemoval): here, in a plain desktop process, as package
// management is not open to Start's. Nothing the request says is taken on
// trust -- any process can send it: it is acted on only while Start is open
// and in front, where the user has just confirmed it, when Start itself says
// it is asking for this package (RemovalCheckMessage), and only for a
// package that is not part of Windows, checked again here. Says so when it
// fails, under the package's own name.
static bool AcceptPackageRemoval(std::wstring family) {
    while (!family.empty() && family.back() == L'\0') {
        family.pop_back();
    }
    HWND start = CachedStartWindow();
    DWORD startPid = 0, foregroundPid = 0;
    if (start) {
        GetWindowThreadProcessId(start, &startPid);
    }
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
    const size_t underscore = family.rfind(L'_');
    if (!start || IsCloaked(start) || !startPid || foregroundPid != startPid || family.size() > 128 ||
        underscore == std::wstring::npos || family.size() - underscore - 1 != 13 ||
        family.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") !=
            std::wstring::npos) {
        Wh_Log(L"[Explorer] uninstall: refused %ls", family.c_str());
        return false;
    }
    // Start sent this from inside a SendMessage, so it answers this one meanwhile.
    DWORD_PTR confirmed = 0;
    if (!SendMessageTimeoutW(start, RemovalCheckMessage(), 0, static_cast<LPARAM>(std::hash<std::wstring>{}(family)),
                             SMTO_ABORTIFHUNG, 1000, &confirmed) ||
        !confirmed) {
        Wh_Log(L"[Explorer] uninstall: refused %ls, not asked for by Start", family.c_str());
        return false;
    }
    SpawnTrackedLaunch([family] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        std::wstring name = family;
        std::wstring error;
        try {
            namespace appmodel = winrt::Windows::ApplicationModel;
            winrt::Windows::Management::Deployment::PackageManager manager;
            bool found = false;
            for (auto const& package : manager.FindPackagesForUser(L"", family)) {
                if (package.IsFramework() || package.SignatureKind() == appmodel::PackageSignatureKind::System) {
                    continue;  // part of Windows: never removed from here
                }
                found = true;
                try {
                    name = package.DisplayName().c_str();
                } catch (...) {
                }
                auto result = manager.RemovePackageAsync(package.Id().FullName()).get();
                const int32_t code = result.ExtendedErrorCode();
                if (code < 0) {
                    error = result.ErrorText().c_str();
                    if (error.empty()) {
                        error = winrt::hresult_error(code).message().c_str();
                    }
                }
            }
            if (!found) {
                error = L"It is not installed for this account, or it is part of Windows.";
            }
        } catch (winrt::hresult_error const& e) {
            error = e.message().c_str();
        }
        Wh_Log(L"[Explorer] uninstall %ls: %ls", family.c_str(), error.empty() ? L"removed" : error.c_str());
        if (!error.empty() && !g_quit.load()) {
            g_removalBoxThread.store(GetCurrentThreadId());
            MessageBoxW(nullptr, (L"Couldn't uninstall " + name + L".\n\n" + error).c_str(), name.c_str(),
                        MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
            g_removalBoxThread.store(0);
        }
        winrt::uninit_apartment();
    });
    return true;
}

static LRESULT CALLBACK ExplorerHelperWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case kExplorerStartDrag:
        RunExplorerDrag(hWnd);
        return 0;
    case WM_COPYDATA: {
        auto pcds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (pcds && pcds->dwData == kExplorerUninstallMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            return AcceptPackageRemoval(
                       std::wstring(reinterpret_cast<const wchar_t*>(pcds->lpData), pcds->cbData / sizeof(wchar_t)))
                       ? 1
                       : 0;
        }
        if (pcds && pcds->dwData == kExplorerDragMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            std::wstring path(reinterpret_cast<const wchar_t*>(pcds->lpData), pcds->cbData / sizeof(wchar_t));
            while (!path.empty() && path.back() == L'\0') {
                path.pop_back();
            }
            // Only a drag the user is making: this window was given the
            // foreground, which only the foreground process can do, and the
            // primary button is down. And only a local file or folder that
            // exists, as for Properties.
            const int button = PrimaryButton();
            bool local = path.size() >= 3 && path[1] == L':' && !PathIsUNCW(path.c_str());
            if (local) {
                const wchar_t root[] = {path[0], L':', L'\\', 0};
                local = GetDriveTypeW(root) != DRIVE_REMOTE;
            }
            if (GetForegroundWindow() != hWnd || !(GetAsyncKeyState(button) & 0x8000) || !local ||
                GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"[Explorer] drag: refused for %ls", path.c_str());
                if (HWND start = CachedStartWindow(); start && !IsCloaked(start) && GetForegroundWindow() == hWnd) {
                    SetForegroundWindow(start);  // it gave this window the foreground for the drag
                }
                return 0;
            }
            // Started from the message loop: the drag loop must not run
            // inside Start's SendMessage, which would wait for it.
            g_explorerDragPath = std::move(path);
            PostMessageW(hWnd, kExplorerStartDrag, 0, 0);
            return 1;
        }
        if (pcds && pcds->dwData == kExplorerCopyDataMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            size_t charCount = pcds->cbData / sizeof(wchar_t);
            const wchar_t* pStr = reinterpret_cast<const wchar_t*>(pcds->lpData);
            std::wstring targetPath(pStr, charCount);
            while (!targetPath.empty() && targetPath.back() == L'\0') {
                targetPath.pop_back();
            }
            while (!targetPath.empty() && (targetPath.front() == L' ' || targetPath.front() == L'\t' || targetPath.front() == L'"')) {
                targetPath.erase(targetPath.begin());
            }
            while (!targetPath.empty() && (targetPath.back() == L' ' || targetPath.back() == L'\t' || targetPath.back() == L'"')) {
                targetPath.pop_back();
            }

            if (targetPath.empty()) return 0;

            // Security validation: reject UNC paths and remote network drives
            if (PathIsUNCW(targetPath.c_str()) || targetPath.starts_with(L"\\\\")) {
                Wh_Log(L"[Explorer] Rejected UNC path: %ls", targetPath.c_str());
                return 0;
            }
            if (targetPath.size() >= 2 && targetPath[1] == L':') {
                wchar_t root[4] = { targetPath[0], L':', L'\\', 0 };
                if (GetDriveTypeW(root) == DRIVE_REMOTE) {
                    Wh_Log(L"[Explorer] Rejected remote drive path: %ls", targetPath.c_str());
                    return 0;
                }
            }

            // Security validation: path must exist
            DWORD attr = GetFileAttributesW(targetPath.c_str());
            if (attr == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"[Explorer] Rejected non-existent path: %ls", targetPath.c_str());
                return 0;
            }

            Wh_Log(L"[Explorer] Received valid SEPR WM_COPYDATA for: %ls", targetPath.c_str());

            SpawnTrackedLaunch([path = std::move(targetPath)]() {
                HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

                // Attempt 1: SHObjectProperties (Dedicated Win32 Shell Properties API)
                BOOL ok = SHObjectProperties(nullptr, 0x00000002 /* SHOP_FILEPATH */, path.c_str(), nullptr);
                Wh_Log(L"[Explorer] SHObjectProperties returned %d, err=%lu for %ls", ok, GetLastError(), path.c_str());

                // Attempt 2: ShellExecuteExW with PIDL
                if (!ok) {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    SFGAOF sfgao = 0;
                    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, &sfgao)) && pidl) {
                        SHELLEXECUTEINFOW sei{};
                        sei.cbSize = sizeof(sei);
                        sei.fMask = SEE_MASK_INVOKEIDLIST;
                        sei.hwnd = nullptr;
                        sei.lpIDList = pidl;
                        sei.lpVerb = L"properties";
                        sei.nShow = SW_SHOWNORMAL;
                        ok = ShellExecuteExW(&sei);
                        Wh_Log(L"[Explorer] ShellExecuteExW PIDL returned %d, err=%lu", ok, GetLastError());
                        CoTaskMemFree(pidl);
                    }
                }

                // Attempt 3: ShellExecuteExW with file path
                if (!ok) {
                    SHELLEXECUTEINFOW sei{};
                    sei.cbSize = sizeof(sei);
                    sei.fMask = SEE_MASK_INVOKEIDLIST;
                    sei.hwnd = nullptr;
                    sei.lpFile = path.c_str();
                    sei.lpVerb = L"properties";
                    sei.nShow = SW_SHOWNORMAL;
                    ok = ShellExecuteExW(&sei);
                    Wh_Log(L"[Explorer] ShellExecuteExW string returned %d, err=%lu", ok, GetLastError());
                }

                if (SUCCEEDED(hr)) {
                    CoUninitialize();
                }
            });

            return 1;
        }
        break;
    }
    case WM_TIMER:
        if (wParam == kSearchAloneTimerId) {
            if (CheckSearchShownWithoutStart()) {
                KillTimer(hWnd, kSearchAloneTimerId);
                g_searchAloneArmed = false;
            }
            return 0;
        }
        break;
    case WM_SYSCOMMAND:
        // It holds the foreground while a program opened from Start starts
        // (DismissStartMenuForLaunch), so Alt+F4 can reach it.
        if ((wParam & 0xFFF0) == SC_CLOSE) {
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI ExplorerHelperThreadProc(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = ExplorerHelperWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = kExplorerHelperClassName;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,
        kExplorerHelperClassName,
        kExplorerHelperWindowName,
        WS_POPUP,
        0, 0, 0, 0,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (hWnd) {
        ChangeWindowMessageFilterEx(hWnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
        g_hExplorerHelperWnd = hWnd;
        Wh_Log(L"[Explorer] Helper host window created: %p", hWnd);
        // Out of context: delivered to this thread's message loop, never run
        // inside the thread that changed the foreground.
        g_searchForegroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            ShellWindowEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        g_startCloakedHook = SetWinEventHook(
            EVENT_OBJECT_CLOAKED, EVENT_OBJECT_CLOAKED, nullptr,
            ShellWindowEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    } else {
        Wh_Log(L"[Explorer] Failed to create helper host window, err=%lu", GetLastError());
    }

    if (g_hExplorerHelperReadyEvent) {
        SetEvent(g_hExplorerHelperReadyEvent);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_searchForegroundHook) {
        UnhookWinEvent(g_searchForegroundHook);
        g_searchForegroundHook = nullptr;
    }
    if (g_startCloakedHook) {
        UnhookWinEvent(g_startCloakedHook);
        g_startCloakedHook = nullptr;
    }
    g_searchAloneArmed = false;

    if (hWnd && IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }
    g_hExplorerHelperWnd = nullptr;
    UnregisterClassW(kExplorerHelperClassName, wc.hInstance);

    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    return 0;
}

// Every explorer.exe hosts one: File Explorer windows can run in processes of
// their own, and which process loads this mod first is not fixed, so "only the
// first" could leave the taskbar's process without one. Extra hosts are
// harmless: the properties relay sends to whichever FindWindow returns, and
// only the taskbar's process acts on Start and Search (IsTaskbarProcess).
static void StartExplorerHelperHost() {
    if (g_hExplorerHelperThread) return;

    g_hExplorerHelperReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hExplorerHelperThread = CreateThread(nullptr, 0, ExplorerHelperThreadProc, nullptr, 0, &g_explorerHelperThreadId);
    if (g_hExplorerHelperThread && g_hExplorerHelperReadyEvent) {
        WaitForSingleObject(g_hExplorerHelperReadyEvent, 3000);
    }
    if (g_hExplorerHelperReadyEvent) {
        CloseHandle(g_hExplorerHelperReadyEvent);
        g_hExplorerHelperReadyEvent = nullptr;
    }
}

static void StopExplorerHelperHost() {
    if (g_hExplorerHelperWnd && IsWindow(g_hExplorerHelperWnd)) {
        PostMessageW(g_hExplorerHelperWnd, WM_CLOSE, 0, 0);
    }
    if (g_explorerHelperThreadId) {
        PostThreadMessageW(g_explorerHelperThreadId, WM_QUIT, 0, 0);
    }
    if (g_hExplorerHelperThread) {
        WaitForSingleObject(g_hExplorerHelperThread, INFINITE);
        CloseHandle(g_hExplorerHelperThread);
        g_hExplorerHelperThread = nullptr;
        g_explorerHelperThreadId = 0;
    }
    // An uninstall error still up would hold the unload until clicked.
    for (int i = 0; i < 50 && g_removalBoxThread.load(); ++i) {
        EnumThreadWindows(g_removalBoxThread.load(), [](HWND hwnd, LPARAM) -> BOOL {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            return TRUE;
        }, 0);
        Sleep(100);
    }
    WaitForTrackedLaunches();
}

void InitExplorer() {
    Wh_Log(L"=== start-everything: starting explorer.exe helper ===");
    StartExplorerHelperHost();
}

// ===========================================================================
// Domain: SearchHost.exe (kept invisible, no WebView2)
// ===========================================================================
//
// Windows hands the foreground to SearchHost whenever Start or Search opens:
// Explorer activates SearchHost's CoreWindow first (~16ms after the Win key or
// a Start-button click) and Start after that. Earlier versions fought this --
// every SearchHost window subclassed, its activation and focus swallowed,
// every show forced to a hide, every foreground call redirected -- and the
// fight is what made the focus workarounds elsewhere necessary.
//
// Now SearchHost does what the shell asks of it, with three differences:
//
//   1. Its CoreWindow is drawn at zero alpha (SetNeutralized). The shell
//      still shows, hides and activates it exactly as before; it just never
//      appears, and clicks pass through it to Start.
//   2. When Start cannot take the foreground from it, Start asks, and it
//      grants it (AllowStartToTakeForeground). It is the foreground process
//      at that moment, the only process in the chain entitled to, and Start
//      then takes the foreground itself.
//   3. WebView2 and the indexer are not launched: that is the disconnect.

struct HookReentryGuard {
    bool& flag;
    explicit HookReentryGuard(bool& f) : flag(f) { flag = true; }
    ~HookReentryGuard() { flag = false; }
};

using CreateProcessW_t = decltype(&CreateProcessW);
static CreateProcessW_t pOriginalCreateProcessW = nullptr;

static bool IsWebViewProcess(LPCWSTR text) {
    if (!text) return false;
    std::wstring lower(text);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return (lower.find(L"msedgewebview2.exe") != std::wstring::npos) ||
           (lower.find(L"embeddedbrowserwebview") != std::wstring::npos) ||
           (lower.find(L"searchindexer.exe") != std::wstring::npos);
}

static BOOL WINAPI Hook_SearchHost_CreateProcessW(
    LPCWSTR applicationName, LPWSTR commandLine,
    LPSECURITY_ATTRIBUTES processAttributes,
    LPSECURITY_ATTRIBUTES threadAttributes,
    BOOL inheritHandles, DWORD creationFlags,
    LPVOID environment, LPCWSTR currentDirectory,
    LPSTARTUPINFOW startupInfo,
    LPPROCESS_INFORMATION processInformation) {
    thread_local bool inHook = false;
    if (inHook) {
        return pOriginalCreateProcessW(applicationName, commandLine,
                                      processAttributes, threadAttributes,
                                      inheritHandles, creationFlags, environment,
                                      currentDirectory, startupInfo, processInformation);
    }
    HookReentryGuard guard(inHook);

    if (IsWebViewProcess(applicationName) || IsWebViewProcess(commandLine)) {
        Wh_Log(L"[SearchHost] Blocked WebView2 launch: app=%ls, cmd=%ls",
            applicationName ? applicationName : L"(null)",
            commandLine ? commandLine : L"(null)");
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    return pOriginalCreateProcessW(applicationName, commandLine,
                                  processAttributes, threadAttributes,
                                  inheritHandles, creationFlags, environment,
                                  currentDirectory, startupInfo, processInformation);
}

// Neutralizes SearchHost's CoreWindow (wParam TRUE) or restores it (FALSE).
// Sent by the watchdog and by uninit; applied on the window's own thread.
static UINT SearchNeutralizeMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_SearchNeutralize");
    return message;
}

// Start's CoreWindow while it is open, else null.
static HWND OpenStartWindow() {
    HWND start = CachedStartWindow();
    DWORD cloaked = 0;
    if (start && SUCCEEDED(DwmGetWindowAttribute(start, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        !cloaked) {
        return start;
    }
    return nullptr;
}

// Lets Start take the foreground from SearchHost, while Start is open.
static BOOL AllowStartToTakeForeground() {
    DWORD pid = 0;
    if (HWND start = OpenStartWindow()) {
        GetWindowThreadProcessId(start, &pid);
    }
    BOOL ok = pid && AllowSetForegroundWindow(pid);
    Wh_Log(L"[SearchHost] foreground requested by Start -> %d", ok);
    return ok;
}

// What SetNeutralized changed, kept on each window so every one is restored
// to exactly what it had: whether it was layered already and, if so, its
// layered attributes (color key, alpha, LWA_ flags), packed into the value.
constexpr wchar_t kOriginalLayeringProp[] = L"StartEverything_OriginalLayering";
constexpr ULONG_PTR kLayeringRecorded = ULONG_PTR{1} << 41;  // never 0, so "no prop" is unambiguous
constexpr ULONG_PTR kWasLayered = ULONG_PTR{1} << 40;

// Takes a SearchHost CoreWindow out of the picture (on) or puts it back (off).
// Runs on the window's own thread.
static void SetNeutralized(HWND hWnd, bool on) {
    // DWM refuses an app cloak (DWMWA_CLOAK) on a CoreWindow, and a window
    // region does not clip its composition content. Layered alpha 0 does:
    // nothing is drawn, and clicks pass through. The shell still shows, hides
    // and activates the window through its own cloak exactly as before.
    LONG ex = GetWindowLongW(hWnd, GWL_EXSTYLE);
    auto saved = reinterpret_cast<ULONG_PTR>(GetPropW(hWnd, kOriginalLayeringProp));
    if (on) {
        if (saved) {
            return;  // already done
        }
        ULONG_PTR record = kLayeringRecorded;
        if (ex & WS_EX_LAYERED) {
            COLORREF key = 0;
            BYTE alpha = 0;
            DWORD flags = 0;
            if (!GetLayeredWindowAttributes(hWnd, &key, &alpha, &flags)) {
                // Layered through UpdateLayeredWindow: there would be nothing
                // to restore it to, so it is left alone.
                Wh_Log(L"[SearchHost] CoreWindow %p uses UpdateLayeredWindow; left visible", hWnd);
                return;
            }
            record |= kWasLayered | (static_cast<ULONG_PTR>(key & 0xFFFFFF) << 16) |
                      (static_cast<ULONG_PTR>(flags & 0xFF) << 8) | alpha;
        } else {
            SetWindowLongW(hWnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        }
        SetPropW(hWnd, kOriginalLayeringProp, reinterpret_cast<HANDLE>(record));
        SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    } else {
        if (!saved) {
            return;  // never changed
        }
        RemovePropW(hWnd, kOriginalLayeringProp);
        if (saved & kWasLayered) {
            SetLayeredWindowAttributes(hWnd, static_cast<COLORREF>((saved >> 16) & 0xFFFFFF),
                                       static_cast<BYTE>(saved & 0xFF), static_cast<DWORD>((saved >> 8) & 0xFF));
        } else {
            SetWindowLongW(hWnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        }
    }
    Wh_Log(L"[SearchHost] %ls CoreWindow %p", on ? L"hid" : L"restored", hWnd);
}

static LRESULT CALLBACK SearchHostSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    UINT requestMessage = StartForegroundRequestMessage();
    UINT neutralizeMessage = SearchNeutralizeMessage();
    if (uMsg == requestMessage && requestMessage) {
        return AllowStartToTakeForeground();
    }
    if (uMsg == neutralizeMessage && neutralizeMessage) {
        SetNeutralized(hWnd, wParam != 0);
        return TRUE;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SetSearchWindowNeutralized(HWND hwnd, BOOL on) {
    DWORD_PTR done = 0;
    SendMessageTimeoutW(hwnd, SearchNeutralizeMessage(), on, 0, SMTO_ABORTIFHUNG, 1000, &done);
}

void InitSearchHost() {
    Wh_Log(L"=== start-everything: initializing SearchHost disconnect ===");
    // kernel32's CreateProcessW only forwards to kernelbase's, and a caller
    // importing through an API set goes to kernelbase directly.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto target = kernelBase
        ? reinterpret_cast<CreateProcessW_t>(GetProcAddress(kernelBase, "CreateProcessW"))
        : nullptr;
    WindhawkUtils::SetFunctionHook(target ? target : CreateProcessW, Hook_SearchHost_CreateProcessW,
                                   &pOriginalCreateProcessW);
}

// SearchHost's CoreWindow can appear after this mod loads, and the shell can
// recreate it, so it is looked for for as long as the mod runs: every 500ms
// for the first minute, then every 5s. Only CoreWindows are touched; the
// process's other top-level windows (IME windows, a zero-size IE host) never
// show.
[[clang::no_destroy]] static std::optional<std::thread> g_searchHostWatchdog;
static std::vector<HWND> g_searchWindows;  // watchdog thread, then uninit after the join

static void StartSearchHostWatchdog() {
    g_searchHostWatchdog.emplace([] {
        for (int i = 0; !g_quit.load(); ++i) {
            // A destroyed window's handle can be reused by a new one.
            std::erase_if(g_searchWindows, [](HWND hwnd) { return !IsWindow(hwnd); });
            for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", nullptr); hwnd;
                 hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) {
                DWORD pid = 0;
                GetWindowThreadProcessId(hwnd, &pid);
                if (pid != GetCurrentProcessId() ||
                    std::find(g_searchWindows.begin(), g_searchWindows.end(), hwnd) != g_searchWindows.end()) {
                    continue;
                }
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc, 0)) {
                    SetSearchWindowNeutralized(hwnd, TRUE);
                    g_searchWindows.push_back(hwnd);
                }
            }
            int slices = i < 120 ? 10 : 100;  // 50ms each, so unload never waits long
            for (int s = 0; s < slices && !g_quit.load(); ++s) {
                Sleep(50);
            }
        }
    });
}

// ===========================================================================
// Domain: StartMenuExperienceHost.exe
// ===========================================================================

namespace {

struct WebShortcut {
    std::wstring prefix;
    std::wstring name;
    std::wstring url;
};

struct CustomConversion {
    std::wstring fromUnit;
    std::wstring toUnit;
    std::wstring formula;
    std::wstring category;
    bool reciprocal = false;  // also back, from toUnit (calc::Invert)
};

// The text size setting (textScale), for Fs.
std::atomic<int> g_textScale{100};

struct Settings {
    std::wstring defaultSearchUrl = L"https://duckduckgo.com/?q={q}";
    std::vector<WebShortcut> webShortcuts;
    std::vector<CustomConversion> unitConversions;
    std::vector<std::wstring> excludedPaths;
    int maxAppResults = 6;
    int maxFileResults = 12;
    int searchDebounceMs = 0;
    int panelMargin = 0;
    int textScale = 100;  // percent
    bool showKeyHints = true;
    bool filterNoisyPaths = true;
    bool learnFavorites = true;
    bool filePreview = true;
    bool animatePreview = true;
};

Settings g_settings;
std::mutex g_settingsMutex;
// Set when Learn Favorite Apps is turned off: the search thread drops its counts.
std::atomic<bool> g_forgetUsage{false};
std::atomic<DWORD> g_xamlThreadId{0};

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    auto defUrl = WindhawkUtils::StringSetting::make(L"defaultSearchUrl");
    if (defUrl.get() && *defUrl.get()) {
        g_settings.defaultSearchUrl = defUrl.get();
    } else {
        g_settings.defaultSearchUrl = L"https://duckduckgo.com/?q={q}";
    }

    g_settings.webShortcuts.clear();
    for (int i = 0;; ++i) {
        auto pfx = WindhawkUtils::StringSetting::make(L"webShortcuts[%d].prefix", i);
        if (!pfx.get() || !*pfx.get()) break;
        auto name = WindhawkUtils::StringSetting::make(L"webShortcuts[%d].name", i);
        auto url = WindhawkUtils::StringSetting::make(L"webShortcuts[%d].url", i);

        WebShortcut s;
        s.prefix = pfx.get();
        for (auto& c : s.prefix) c = static_cast<wchar_t>(towlower(c));
        s.name = (name.get() && *name.get()) ? name.get() : L"Web";
        s.url = (url.get() && *url.get()) ? url.get() : L"";
        if (!s.prefix.empty() && !s.url.empty()) {
            g_settings.webShortcuts.push_back(std::move(s));
        }
    }
    if (g_settings.webShortcuts.empty()) {
        g_settings.webShortcuts.push_back({L"yt", L"YouTube", L"https://www.youtube.com/results?search_query={q}"});
        g_settings.webShortcuts.push_back({L"gh", L"GitHub", L"https://github.com/search?q={q}"});
        g_settings.webShortcuts.push_back({L"w", L"Wikipedia", L"https://en.wikipedia.org/wiki/Special:Search?search={q}"});
        g_settings.webShortcuts.push_back({L"r", L"Reddit", L"https://www.reddit.com/search/?q={q}"});
    }

    g_settings.unitConversions.clear();
    for (int i = 0;; ++i) {
        auto fromU = WindhawkUtils::StringSetting::make(L"unitConversions[%d].fromUnit", i);
        if (!fromU.get() || !*fromU.get()) break;
        auto toU = WindhawkUtils::StringSetting::make(L"unitConversions[%d].toUnit", i);
        auto formula = WindhawkUtils::StringSetting::make(L"unitConversions[%d].formula", i);
        auto cat = WindhawkUtils::StringSetting::make(L"unitConversions[%d].category", i);

        CustomConversion c;
        c.fromUnit = tools::Trim(fromU.get());  // shown as written; matched by NormalizeUnit
        c.toUnit = (toU.get() && *toU.get()) ? tools::Trim(toU.get()) : L"";
        c.formula = (formula.get() && *formula.get()) ? tools::Trim(formula.get()) : L"";
        c.category = (cat.get() && *cat.get()) ? tools::Trim(cat.get()) : L"Conversion";
        c.reciprocal = Wh_GetIntSetting(L"unitConversions[%d].reciprocal", i) != 0;

        if (!c.fromUnit.empty() && !c.toUnit.empty() && !c.formula.empty()) {
            g_settings.unitConversions.push_back(std::move(c));
        }
    }

    g_settings.filterNoisyPaths = Wh_GetIntSetting(L"filterNoisyPaths") != 0;

    g_settings.excludedPaths.clear();
    for (int i = 0;; ++i) {
        auto val = WindhawkUtils::StringSetting::make(L"excludedPaths[%d]", i);
        if (!val.get() || !*val.get()) break;
        std::wstring s = tools::ToLower(tools::Trim(val.get()));
        for (auto& ch : s) {
            if (ch == L'/') ch = L'\\';
        }
        if (!s.empty()) {
            g_settings.excludedPaths.push_back(std::move(s));
        }
    }
    if (g_settings.excludedPaths.empty() && g_settings.filterNoisyPaths) {
        static const wchar_t* kDefaults[] = {
            L"\\node_modules\\",
            L"\\.git\\",
            L"\\.gradle\\",
            L"\\appdata\\local\\temp\\",
            L"\\appdata\\local\\packages\\",
            L"\\__pycache__\\",
            L"\\.venv\\",
            L"\\site-packages\\",
            L"\\.cache\\",
            L"\\build\\intermediates\\",
            L"\\obj\\debug\\",
            L"\\obj\\release\\",
            L"\\windows\\winsxs\\",
            L"\\windows\\servicing\\",
        };
        for (const wchar_t* d : kDefaults) {
            g_settings.excludedPaths.push_back(d);
        }
    }

    int maxApps = Wh_GetIntSetting(L"maxAppResults");
    g_settings.maxAppResults = (maxApps > 0) ? std::clamp(maxApps, 1, 30) : 6;

    int maxFiles = Wh_GetIntSetting(L"maxFileResults");
    g_settings.maxFileResults = (maxFiles > 0) ? std::clamp(maxFiles, 1, 50) : 12;

    g_settings.searchDebounceMs = std::clamp(Wh_GetIntSetting(L"searchDebounceMs"), 0, 1000);

    g_settings.panelMargin = std::clamp(Wh_GetIntSetting(L"panelMargin"), 0, 60);
    const int textScale = Wh_GetIntSetting(L"textScale");
    g_settings.textScale = textScale ? std::clamp(textScale, 80, 150) : 100;  // 0: not set yet
    g_textScale.store(g_settings.textScale);

    g_settings.showKeyHints = Wh_GetIntSetting(L"showKeyHints") != 0;

    // Off forgets: the stored counts now, the search thread's copy on its next
    // turn (g_forgetUsage).
    g_settings.learnFavorites = Wh_GetIntSetting(L"learnFavorites") != 0;
    g_settings.filePreview = Wh_GetIntSetting(L"filePreview") != 0;
    g_settings.animatePreview = Wh_GetIntSetting(L"animatePreview") != 0;
    if (!g_settings.learnFavorites) {
        Wh_DeleteValue(apps::kUsageValueName);
        g_forgetUsage.store(true);
    }

    Wh_Log(L"=== settings: defSearch=%ls shortcuts=%zu maxApps=%d maxFiles=%d debounce=%d hints=%d filterNoise=%d excluded=%zu ===",
        g_settings.defaultSearchUrl.c_str(), g_settings.webShortcuts.size(),
        g_settings.maxAppResults, g_settings.maxFileResults,
        g_settings.searchDebounceMs,
        g_settings.showKeyHints ? 1 : 0,
        g_settings.filterNoisyPaths ? 1 : 0,
        g_settings.excludedPaths.size());
}

wux::DependencyObject FindDescendantByName(wux::DependencyObject const& root,
                                           std::wstring_view name,
                                           int maxDepth) {
    if (maxDepth < 0) {
        return nullptr;
    }
    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        if (std::wstring_view{fe.Name()} == name) {
            return root;
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindDescendantByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// Type plus x:Name, for log lines that have to be matched against what a
// tree inspector shows.

std::wstring ElementLabel(wux::DependencyObject const& obj) {
    std::wstring label;
    try {
        label = winrt::get_class_name(obj);
    } catch (...) {
        label = L"<unknown>";
    }
    if (auto fe = obj.try_as<wux::FrameworkElement>()) {
        std::wstring name{fe.Name()};
        if (!name.empty()) {
            label += L"#" + name;
        }
    }
    return label;
}


// ---------------------------------------------------------------------------
// A search box that is actually a search box
//
// The stock one is a Button: a placeholder TextBlock, two icons and a
// Rectangle called TextCaret drawn to look like a cursor. Typing into it is
// not possible because there is nothing there to type into -- activating it
// hands off to SearchHost, which owns the only real text box in the whole
// arrangement.
//
// So this collapses the decoy and puts a TextBox in the same grid cell, at the
// same size, to see whether the Start menu will host one at all.
// ---------------------------------------------------------------------------

// What currently has keyboard focus, for the log.
std::wstring FocusedElementLabel() {
    try {
        auto focused = wux::Input::FocusManager::GetFocusedElement();
        if (!focused) {
            return L"(nothing)";
        }
        if (auto dobj = focused.try_as<wux::DependencyObject>()) {
            return ElementLabel(dobj);
        }
        return std::wstring{winrt::get_class_name(focused)};
    } catch (...) {
        return L"(threw)";
    }
}

[[clang::no_destroy]] wuxc::TextBox g_ourBox{nullptr};
[[clang::no_destroy]] wux::FrameworkElement g_stockButton{nullptr};
[[clang::no_destroy]] wuxc::Grid g_resultsHost{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_resultsList{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_appsList{nullptr};
struct AppCardUI {
    int appIndex = -1;
    std::wstring title;
    std::wstring openPath;
    wuxc::Button button{nullptr};
    bool canRunAsAdmin = true;
    bool isSetting = false;
    bool isFile = false;
};

[[clang::no_destroy]] static std::optional<std::vector<wuxc::Button>> g_appButtonsOpt;
[[clang::no_destroy]] static std::optional<std::vector<AppCardUI>> g_activeAppsOpt;
// The Files column's rows, in order, for keyboard selection there.
[[clang::no_destroy]] static std::optional<std::vector<wuxc::Button>> g_fileButtonsOpt;

// Every handler the mod puts on Start's XAML objects, so the teardown can take
// them all off while the mod is still loaded. Taking an element out of the tree
// doesn't free it: XAML can hold a dropped row until its next frame, which a
// closed Start doesn't draw until it opens again, and freeing a handler whose
// code went away with the mod crashes Start.
struct XamlHandler {
    winrt::weak_ref<wf::IInspectable> source;
    std::shared_ptr<void> revoker;  // a C++/WinRT revoker; revokes when destroyed
};
[[clang::no_destroy]] std::optional<std::vector<XamlHandler>> g_xamlHandlers{std::in_place};
// Rows are rebuilt on every keystroke; handlers on objects XAML has since
// freed are forgotten whenever the list doubles.
size_t g_xamlHandlersPruneAt = 512;

template <typename Revoker>
void KeepHandler(wf::IInspectable const& source, Revoker revoker) {
    if (!g_xamlHandlers) {
        return;  // torn down: the revoker revokes as it goes out of scope
    }
    if (g_xamlHandlers->size() >= g_xamlHandlersPruneAt) {
        std::erase_if(*g_xamlHandlers, [](XamlHandler const& h) { return !h.source.get(); });
        g_xamlHandlersPruneAt = std::max<size_t>(512, g_xamlHandlers->size() * 2);
    }
    g_xamlHandlers->push_back({winrt::make_weak(source), std::make_shared<Revoker>(std::move(revoker))});
}

[[clang::no_destroy]] wuxc::Border g_appsHeaderHolder{nullptr};
[[clang::no_destroy]] wuxc::Border g_filesHeaderHolder{nullptr};
[[clang::no_destroy]] wuxc::Border g_searchBarBorder{nullptr};
[[clang::no_destroy]] wuxc::Border g_divider{nullptr};
[[clang::no_destroy]] wuxc::Border g_footerBorder{nullptr};
[[clang::no_destroy]] wuxm::TranslateTransform g_resultsTranslate{nullptr};
[[clang::no_destroy]] wuxma::Storyboard g_revealAnim{nullptr};
[[clang::no_destroy]] wuxma::Storyboard g_hideAnim{nullptr};
std::atomic<bool> g_isOverlayVisible{false};
std::atomic<bool> g_isHiding{false};

void HideStockPlaceholder(wux::FrameworkElement const& stock);
void RevealOverlayAnimated();
void HideOverlayAnimated();
void HidePreview();
void HideAllOtherSearchBoxes(wux::DependencyObject const& root, int depth = 15);
void SyncOverlayBackground();

// A font size of the mod's own UI at the text size setting (textScale).
double Fs(double size) {
    return size * g_textScale.load(std::memory_order_relaxed) / 100.0;
}

// Sets a font size of the mod's UI, keeping the size it is at 100% in the
// element's Tag, for RescaleFonts.
template <typename Element>
void ScaleFont(Element const& element, double size) {
    element.FontSize(Fs(size));
    element.Tag(winrt::box_value(size));
}

// Puts a new text size on what is already built (ScaleFont).
void RescaleFonts(wux::DependencyObject const& root) {
    if (!root) {
        return;
    }
    if (auto element = root.try_as<wux::FrameworkElement>()) {
        if (auto base = element.Tag().try_as<wf::IReference<double>>()) {
            const double size = Fs(base.Value());
            if (auto text = root.try_as<wuxc::TextBlock>()) {
                text.FontSize(size);
            } else if (auto icon = root.try_as<wuxc::FontIcon>()) {
                icon.FontSize(size);
            } else if (auto control = root.try_as<wuxc::Control>()) {
                control.FontSize(size);
            }
        }
    }
    const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int32_t i = 0; i < count; ++i) {
        RescaleFonts(wuxm::VisualTreeHelper::GetChild(root, i));
    }
}

// The space between the search panel and Start's edges (panelMargin).
int PanelMargin() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings.panelMargin;
}

void RequestRender();
void TeardownStartMenuUi();
inline UINT GetTeardownMessage() {
    static UINT s_msg = RegisterWindowMessageW(L"Windhawk_StartMenuTeardown_start-everything");
    return s_msg;
}
// The subclass's answer to the teardown message. A window that isn't
// subclassed answers 0 through DefWindowProc, which must not count as done.
constexpr LRESULT kTeardownDone = 0x5445;

[[clang::no_destroy]] wuxc::TextBox::TextChanged_revoker g_ourBoxChanged;
[[clang::no_destroy]] wux::UIElement::LostFocus_revoker g_ourBoxLost;
[[clang::no_destroy]] wux::DispatcherTimer g_openFocus{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_shownFocus{nullptr};

inline wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush{winrt::Windows::UI::ColorHelper::FromArgb(a, r, g, b)};
}

static std::atomic<int> g_cachedLightTheme{-1};

inline void RefreshThemeCache() {
    DWORD val = 0;
    DWORD sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS) {
        g_cachedLightTheme.store(val != 0 ? 1 : 0);
        return;
    }
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS) {
        g_cachedLightTheme.store(val != 0 ? 1 : 0);
        return;
    }
    try {
        wux::FrameworkElement target = g_resultsHost ? g_resultsHost.try_as<wux::FrameworkElement>() : g_stockButton;
        if (target) {
            auto theme = target.ActualTheme();
            if (theme == wux::ElementTheme::Light) {
                g_cachedLightTheme.store(1);
                return;
            }
            if (theme == wux::ElementTheme::Dark) {
                g_cachedLightTheme.store(0);
                return;
            }
        }
    } catch (...) {}
    g_cachedLightTheme.store(0);
}

inline bool IsLightTheme() {
    int cached = g_cachedLightTheme.load();
    if (cached >= 0) {
        return cached != 0;
    }
    RefreshThemeCache();
    return g_cachedLightTheme.load() != 0;
}

wuxc::Border FindMenuAcrylicBorder() {
    try {
        wux::DependencyObject start = g_resultsHost ? g_resultsHost : g_stockButton;
        if (!start) return nullptr;

        wux::DependencyObject root = start;
        wux::DependencyObject node = start;
        for (int up = 0; up < 12; ++up) {
            auto p = wuxm::VisualTreeHelper::GetParent(node);
            if (!p) break;
            root = p;
            node = p;
        }

        if (auto found = FindDescendantByName(root, L"AcrylicBorder", 10)) {
            if (auto b = found.try_as<wuxc::Border>()) {
                return b;
            }
        }
    } catch (...) {}
    return nullptr;
}

void SyncOverlayBackground() {
    if (!g_resultsHost) return;
    const int margin = PanelMargin();
    try {
        g_resultsHost.Margin(wux::ThicknessHelper::FromUniformLength(margin));
    } catch (...) {}
    try {
        if (auto border = FindMenuAcrylicBorder()) {
            if (auto brush = border.Background()) {
                if (g_resultsHost.Background() != brush) {
                    g_resultsHost.Background(brush);
                    Wh_Log(L"SyncOverlayBackground: synchronized background from AcrylicBorder (%ls)",
                        winrt::get_class_name(brush).c_str());
                }
                try {
                    g_resultsHost.CornerRadius(border.CornerRadius());
                    // Covering Start to its edges, the results cover its
                    // outline too: it is drawn again on top. Inset, the
                    // outline still shows around them.
                    g_resultsHost.BorderBrush(border.BorderBrush());
                    g_resultsHost.BorderThickness(margin == 0 ? border.BorderThickness()
                                                              : wux::ThicknessHelper::FromUniformLength(0));
                } catch (...) {}
            }
        }
    } catch (...) {}

    // Synchronize search bar, divider, and footer theme colors in real time
    try {
        bool isLight = IsLightTheme();
        if (g_resultsHost) {
            g_resultsHost.RequestedTheme(isLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
        }
        if (g_searchBarBorder) {
            g_searchBarBorder.Background(isLight ? MakeBrush(0xD0, 0xFF, 0xFF, 0xFF) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
            g_searchBarBorder.BorderBrush(isLight ? MakeBrush(0x30, 0x00, 0x00, 0x00) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
        }
        if (g_divider) {
            g_divider.Background(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x14, 0xFF, 0xFF, 0xFF));
        }
        if (g_footerBorder) {
            g_footerBorder.BorderBrush(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x12, 0xFF, 0xFF, 0xFF));
        }
    } catch (...) {}
}

void RevealOverlayAnimated() {
    if (!g_resultsHost) return;
    try {
        // If already visible and not in the process of hiding, nothing to animate!
        if (g_isOverlayVisible.load() && !g_isHiding.load()) {
            return;
        }

        g_isOverlayVisible.store(true);
        g_isHiding.store(false);

        if (g_hideAnim) {
            g_hideAnim.Stop();
            g_hideAnim = nullptr;
        }

        SyncOverlayBackground();
        g_resultsHost.Visibility(wux::Visibility::Visible);
        g_resultsHost.IsHitTestVisible(true);

        if (g_resultsHost.Opacity() >= 0.98 && g_resultsTranslate && std::abs(g_resultsTranslate.Y()) < 0.1) {
            g_resultsHost.Opacity(1.0);
            g_resultsHost.IsHitTestVisible(true);
            if (g_resultsTranslate) g_resultsTranslate.Y(0.0);
            return;
        }

        if (g_revealAnim) {
            g_revealAnim.Stop();
            g_revealAnim = nullptr;
        }

        wuxma::Storyboard sb;
        wuxma::CubicEase ease;
        ease.EasingMode(wuxma::EasingMode::EaseOut);

        wuxma::DoubleAnimation animOpacity;
        double currentOpacity = g_resultsHost.Opacity();
        animOpacity.From(currentOpacity < 0.95 ? currentOpacity : 0.0);
        animOpacity.To(1.0);
        animOpacity.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(120)));
        animOpacity.EasingFunction(ease);
        wuxma::Storyboard::SetTarget(animOpacity, g_resultsHost);
        wuxma::Storyboard::SetTargetProperty(animOpacity, L"Opacity");
        sb.Children().Append(animOpacity);

        if (g_resultsTranslate) {
            wuxma::DoubleAnimation animY;
            double currentY = g_resultsTranslate.Y();
            animY.From(currentY < -0.5 ? currentY : -8.0);
            animY.To(0.0);
            animY.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(120)));
            animY.EasingFunction(ease);
            wuxma::Storyboard::SetTarget(animY, g_resultsTranslate);
            wuxma::Storyboard::SetTargetProperty(animY, L"Y");
            sb.Children().Append(animY);
        }

        KeepHandler(sb, sb.Completed(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
            if (g_isOverlayVisible.load() && g_resultsHost) {
                g_resultsHost.Opacity(1.0);
                g_resultsHost.IsHitTestVisible(true);
                if (g_resultsTranslate) g_resultsTranslate.Y(0.0);
            }
        }));

        g_revealAnim = sb;
        sb.Begin();
    } catch (...) {}
}

void HideOverlayAnimated() {
    HidePreview();
    if (!g_resultsHost) return;
    try {
        if (g_isHiding.load()) {
            return;
        }
        if (!g_isOverlayVisible.load()) {
            return;
        }
        g_isOverlayVisible.store(false);
        g_isHiding.store(true);

        if (g_revealAnim) {
            g_revealAnim.Stop();
            g_revealAnim = nullptr;
        }

        wuxma::Storyboard sb;
        wuxma::DoubleAnimation animOpacity;
        double currentOpacity = g_resultsHost.Opacity();
        animOpacity.From(currentOpacity > 0.05 ? currentOpacity : 1.0);
        animOpacity.To(0.0);
        animOpacity.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(180)));
        wuxma::Storyboard::SetTarget(animOpacity, g_resultsHost);
        wuxma::Storyboard::SetTargetProperty(animOpacity, L"Opacity");

        wuxma::CubicEase ease;
        ease.EasingMode(wuxma::EasingMode::EaseOut);
        animOpacity.EasingFunction(ease);
        sb.Children().Append(animOpacity);

        KeepHandler(sb, sb.Completed(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
            g_isHiding.store(false);
            if (!g_isOverlayVisible.load() && g_resultsHost) {
                g_resultsHost.Opacity(0.0);
                g_resultsHost.IsHitTestVisible(false);
                if (g_resultsTranslate) {
                    g_resultsTranslate.Y(-8.0);
                }
                if (g_resultsList) g_resultsList.Children().Clear();
                if (g_appsList) g_appsList.Children().Clear();
                if (g_activeAppsOpt) g_activeAppsOpt->clear();
                if (g_appButtonsOpt) g_appButtonsOpt->clear();
                if (g_fileButtonsOpt) g_fileButtonsOpt->clear();
            }
        }));

        g_hideAnim = sb;
        sb.Begin();
    } catch (...) {
        g_isHiding.store(false);
        if (g_resultsHost) {
            g_resultsHost.Opacity(0.0);
            g_resultsHost.IsHitTestVisible(false);
        }
    }
}

struct SuppressedElement {
    winrt::weak_ref<wux::FrameworkElement> element;
    wux::Visibility visibility = wux::Visibility::Visible;
    double opacity = 1.0;
    bool hitTestVisible = true;
    double width = std::numeric_limits<double>::quiet_NaN();
    double maxWidth = std::numeric_limits<double>::quiet_NaN();
    double height = std::numeric_limits<double>::quiet_NaN();
    double maxHeight = std::numeric_limits<double>::quiet_NaN();
    wux::Thickness margin{};
    bool isControl = false;
    bool tabStop = true;
    bool tabStopOnly = false;
};

static std::vector<SuppressedElement> g_suppressed;

void SuppressShellElement(wux::FrameworkElement const& fe, bool collapse = true,
                          bool zeroSize = false, bool tabStopOnly = false) {
    if (!fe) {
        return;
    }
    try {
        SuppressedElement saved;
        saved.element = winrt::make_weak(fe);
        if (auto ctl = fe.try_as<wuxc::Control>()) {
            saved.isControl = true;
            saved.tabStop = ctl.IsTabStop();
        }
        saved.tabStopOnly = tabStopOnly;

        if (tabStopOnly) {
            g_suppressed.push_back(std::move(saved));
            if (auto ctl = fe.try_as<wuxc::Control>()) {
                ctl.IsTabStop(false);
            }
            return;
        }

        saved.visibility = fe.Visibility();
        saved.opacity = fe.Opacity();
        saved.hitTestVisible = fe.IsHitTestVisible();
        saved.width = fe.Width();
        saved.maxWidth = fe.MaxWidth();
        saved.height = fe.Height();
        saved.maxHeight = fe.MaxHeight();
        saved.margin = fe.Margin();
        g_suppressed.push_back(std::move(saved));

        fe.Opacity(0.0);
        fe.IsHitTestVisible(false);
        if (collapse) {
            fe.Visibility(wux::Visibility::Collapsed);
        }
        if (zeroSize) {
            fe.MaxWidth(0.0);
            fe.Width(0.0);
            fe.MaxHeight(0.0);
            fe.Height(0.0);
            fe.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 0));
        }
    } catch (...) {
    }
}

void RestoreShellElements() {
    size_t restored = 0;
    for (auto it = g_suppressed.rbegin(); it != g_suppressed.rend(); ++it) {
        auto fe = it->element.get();
        if (!fe) {
            continue;
        }
        try {
            if (it->tabStopOnly) {
                if (it->isControl) {
                    if (auto ctl = fe.try_as<wuxc::Control>()) {
                        ctl.IsTabStop(it->tabStop);
                    }
                }
                ++restored;
                continue;
            }

            fe.Visibility(it->visibility);
            fe.Opacity(it->opacity);
            fe.IsHitTestVisible(it->hitTestVisible);
            fe.Width(it->width);
            fe.MaxWidth(it->maxWidth);
            fe.Height(it->height);
            fe.MaxHeight(it->maxHeight);
            fe.Margin(it->margin);
            if (it->isControl) {
                if (auto ctl = fe.try_as<wuxc::Control>()) {
                    ctl.IsTabStop(it->tabStop);
                }
            }
            ++restored;
        } catch (...) {
        }
    }
    Wh_Log(L"teardown: restored %zu shell element(s) of %zu", restored,
        g_suppressed.size());
    g_suppressed.clear();
}

void HideAllOtherSearchBoxes(wux::DependencyObject const& root, int depth) {
    if (!root || depth < 0) return;
    try {
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            std::wstring name{fe.Name()};
            std::wstring cls;
            try { cls = winrt::get_class_name(root); } catch (...) {}

            if (name.find(L"Windhawk") == std::wstring::npos) {
                if (cls.find(L"SearchBox") != std::wstring::npos ||
                    cls.find(L"RichSearch") != std::wstring::npos ||
                    cls.find(L"SearchControl") != std::wstring::npos ||
                    name.find(L"SearchBox") != std::wstring::npos ||
                    name.find(L"SearchBlock") != std::wstring::npos) {
                    SuppressShellElement(fe, true, false);
                    if (auto ctl = fe.try_as<wuxc::Control>()) {
                        ctl.IsTabStop(false);
                    }
                    Wh_Log(L"HideAllOtherSearchBoxes: suppressed %ls#%ls", cls.c_str(), name.c_str());
                }
            }
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < count; ++i) {
            HideAllOtherSearchBoxes(wuxm::VisualTreeHelper::GetChild(root, i), depth - 1);
        }
    } catch (...) {}
}

// Read and written by the XAML, search and launch threads.
static std::atomic<HWND> g_hCoreWindow{nullptr};

HWND GetOurCoreWindow() {
    if (HWND cached = g_hCoreWindow.load(); cached && IsWindow(cached)) {
        return cached;
    }
    HWND ours = nullptr;
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid != GetCurrentProcessId()) {
                return TRUE;
            }
            wchar_t cls[128] = {};
            GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                *reinterpret_cast<HWND*>(param) = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ours));
    if (ours) {
        g_hCoreWindow = ours;
    }
    return ours;
}

bool IsOurWindowCloaked() {
    HWND ours = GetOurCoreWindow();
    if (!ours) return true;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(ours, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

std::atomic<bool> g_suppressRefocus{false};
std::atomic<bool> g_appIndexNeedsRefresh{false};
std::atomic<ULONGLONG> g_lastAppIndexRebuildTick{0};

void RequestAppIndexRefresh();

// SearchHost answers the foreground request on its UI thread. If it does not
// answer in time it is busy -- typically just woken -- and asking again on
// every retry would stall Start's UI thread 200ms at a time while it opens.
// So after an unanswered request, requests pause for a moment.
std::atomic<ULONGLONG> g_foregroundRequestsPausedUntil{0};
constexpr ULONGLONG kForegroundRequestPauseMs = 1000;

void TakeForeground(bool force = false) {
    try {
        if (!force && g_suppressRefocus.load()) {
            return;
        }

        HWND ours = GetOurCoreWindow();
        if (!ours || !IsWindow(ours)) {
            return;
        }

        HWND current = GetForegroundWindow();
        if (current == ours || IsOurWindowCloaked()) {
            return;  // already there, or Start is closed: nothing to show
        }

        BOOL ok = SetForegroundWindow(ours);
        if (!ok && IsSearchHostWindow(current) && GetTickCount64() >= g_foregroundRequestsPausedUntil.load()) {
            // The shell gives SearchHost the foreground when Start opens, and
            // only the foreground process can pass it on. Ask SearchHost to.
            DWORD_PTR granted = 0;
            if (!SendMessageTimeoutW(current, StartForegroundRequestMessage(), 0, 0, SMTO_ABORTIFHUNG, 200,
                                     &granted)) {
                g_foregroundRequestsPausedUntil.store(GetTickCount64() + kForegroundRequestPauseMs);
                Wh_Log(L"focus: SearchHost did not answer the foreground request; pausing requests");
            } else if (granted) {
                ok = SetForegroundWindow(ours);
            }
        }
        BringWindowToTop(ours);
        Wh_Log(L"focus: TakeForeground -> SetForegroundWindow result=%d, prev=0x%p, ours=0x%p",
               ok ? 1 : 0, current, ours);
    } catch (...) {
    }
}

void FocusOurBoxNow() {
    if (g_suppressRefocus.load()) return;
    if (!g_ourBox) return;

    try {
        auto now = wux::Input::FocusManager::GetFocusedElement();
        if (now && now == g_ourBox) {
            return; // Already focused! Do not re-focus or interrupt typing!
        }
        if (g_isOverlayVisible.load() && now && (now.try_as<wuxc::Button>() || 
                                                now.try_as<wuxc::GridViewItem>() || 
                                                now.try_as<wuxc::ListViewItem>())) {
            return;
        }
    } catch (...) {}

    TakeForeground();
    try {
        bool ok = g_ourBox.Focus(wux::FocusState::Programmatic);
        g_ourBox.SelectionStart(static_cast<int32_t>(g_ourBox.Text().size()));
        g_ourBox.SelectionLength(0);
        Wh_Log(L"focus: FocusOurBoxNow -> Focus result=%d, focused=%ls",
            ok ? 1 : 0, FocusedElementLabel().c_str());
    } catch (...) {}
}

void TriggerMenuOpenFocus() {
    try {
        g_suppressRefocus.store(false);
        TakeForeground(true);
        FocusOurBoxNow();
        RequestAppIndexRefresh();
        if (g_openFocus) {
            g_openFocus.Stop();
            g_openFocus = nullptr;
        }
        auto t = wux::DispatcherTimer();
        t.Interval(std::chrono::milliseconds(50));
        auto ticks = std::make_shared<int>(0);
        KeepHandler(t, t.Tick(winrt::auto_revoke, [ticks](wf::IInspectable const& sender, wf::IInspectable const&) {
            auto timer = sender.try_as<wux::DispatcherTimer>();
            if (g_suppressRefocus.load()) {
                if (timer) timer.Stop();
                return;
            }
            HWND ours = GetOurCoreWindow();
            HWND fg = GetForegroundWindow();
            if (fg != ours) {
                TakeForeground();
            }
            FocusOurBoxNow();
            auto now = wux::Input::FocusManager::GetFocusedElement();
            if ((now && now == g_ourBox && fg == ours) || ++(*ticks) >= 10) {
                if (timer) timer.Stop();
            }
        }));
        t.Start();
        g_openFocus = t;
    } catch (...) {
    }
}

// Takes the foreground as soon as the shell has shown Start, and keeps it
// from SearchHost while Start settles.
//
// The shell shows Start ~16-50ms after the Win key or a Start-button click
// but activates it only ~200ms later, and until Start holds the foreground,
// keys go to SearchHost -- which takes them as a native search, grabs the
// foreground back, and has the shell swap Start for Search (Explorer then
// reopens Start; see "Search is never left showing without Start").
// VisibilityChanged arrives before the shell lifts its cloak, so for ~300ms
// this checks every 10ms and, whenever Start is open and SearchHost has the
// foreground, takes it (TakeForeground asks SearchHost for it). Only
// SearchHost is taken from: once any other window is in front -- a quick
// Escape, a click elsewhere -- the check stops.
void TakeForegroundWhenShown() {
    try {
        if (g_shownFocus) {
            g_shownFocus.Stop();
            g_shownFocus = nullptr;
        }
        auto t = wux::DispatcherTimer();
        t.Interval(std::chrono::milliseconds(10));
        auto ticks = std::make_shared<int>(0);
        KeepHandler(t, t.Tick(winrt::auto_revoke, [ticks](wf::IInspectable const& sender, wf::IInspectable const&) {
            HWND ours = GetOurCoreWindow();
            HWND fg = GetForegroundWindow();  // null while the foreground changes hands
            bool done = !ours || ++(*ticks) > 30 || (fg && fg != ours && !IsSearchHostWindow(fg));
            if (!done && fg != ours && !IsOurWindowCloaked()) {
                TakeForeground(true);
            }
            if (done) {
                if (auto timer = sender.try_as<wux::DispatcherTimer>()) {
                    timer.Stop();
                }
            }
        }));
        t.Start();
        g_shownFocus = t;
    } catch (...) {
    }
}

// Closes Start the way pressing the Start button a second time does.
//
// SC_TASKLIST to the taskbar is the documented "activate the Start menu"
// command. Sent while Start is open it closes it, and the shell hands the
// foreground back to whatever had it before -- what the injected Escape used
// to achieve, with no synthesized input. Measured on 26100: foreground back to
// the previous app ~16ms after the message, Start cloaked ~200ms later.
//
// It toggles, so it is sent only while Start is open and active: the
// foreground must still be in this process or in SearchHost. Once it has
// moved elsewhere Start is already closing, and because the cloak lags the
// foreground change by ~200ms, a toggle in that gap would reopen it. The one
// exception is Explorer's helper, given the foreground for a launch
// (DismissStartMenuForLaunch) or a drag (StartFileDrag): a hidden window,
// which Start does not close for.
HWND FindExplorerLaunchHolder();

void CloseStartMenu() {
    HWND ours = GetOurCoreWindow();
    if (!ours || !IsWindow(ours) || IsOurWindowCloaked()) {
        return;
    }
    HWND fg = GetForegroundWindow();
    DWORD fgPid = 0;
    GetWindowThreadProcessId(fg, &fgPid);
    if (fgPid != GetCurrentProcessId() && !IsSearchHostWindow(fg) && (!fg || fg != FindExplorerLaunchHolder())) {
        Wh_Log(L"CloseStartMenu: foreground already moved to %p, leaving Start to close", fg);
        return;
    }
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        PostMessageW(tray, WM_SYSCOMMAND, SC_TASKLIST, 0);
        Wh_Log(L"CloseStartMenu: asked the shell to close Start");
    }
}

// SearchHost taking the foreground while Start stays open.
//
// A click on an empty part of Start makes the shell activate Search -- on
// stock Windows so that typing goes into its search -- and a key typed before
// Start holds the keyboard does the same. Under this mod that must not count
// as leaving Start: it would hide the results and clear what was typed, and
// the next key would reach SearchHost and swap Start out. Start takes the
// foreground back instead (TakeForeground asks SearchHost for it). Losing it
// to anything else -- Escape, another app, a launch -- is left alone; closing
// Start hands the foreground to the previous app, never to SearchHost.
// When something was last opened from Start (DismissStartMenuForLaunch). A click
// in Start hands SearchHost the foreground too, and taking it back right after
// a launch took it from the program instead: that cancelled its right to come
// to the front, and it opened behind.
std::atomic<ULONGLONG> g_launchedAtTick{0};

// The Store app Start is asking Explorer to remove, while it asks
// (RequestPackageRemoval): what it answers RemovalCheckMessage by. XAML thread.
static std::wstring g_removalPending;

// A result is being dragged, by Explorer's helper (StartFileDrag), which holds
// the foreground until the drop: Start losing it then is not Start closing.
static bool g_dragging = false;
constexpr ULONGLONG kLaunchHandoffMs = 2000;

bool SearchHostTookForegroundFromOpenStart() {
    if (GetTickCount64() - g_launchedAtTick.load() < kLaunchHandoffMs) {
        return false;  // a launch is taking over; let SearchHost hold it until then
    }
    return !IsOurWindowCloaked() && IsSearchHostWindow(GetForegroundWindow());
}

void ReclaimForegroundFromSearchHost() {
    try {
        if (!g_ourBox) {
            return;
        }
        g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal, [] {
            if (SearchHostTookForegroundFromOpenStart()) {
                TakeForeground(true);
            }
        });
    } catch (...) {
    }
}

// What was typed, kept across Start being swapped out and reopened.
//
// A key that reaches SearchHost before Start holds the keyboard makes the
// shell swap Start out for Search, and Explorer then opens Start again (see
// "Search is never left showing without Start"). The keys typed meanwhile
// still arrive in this box, but reopening briefly activates SearchHost, and
// losing activation clears the box. Start is already cloaked by then -- in a
// normal close it is still visible when it loses activation -- so the text is
// set aside and put back if Start is activated again straight away.
static std::wstring g_swappedOutText;
static ULONGLONG g_swappedOutTick = 0;
static const ULONGLONG kSwappedOutTextLifetimeMs = 1500;

void StashTextIfSwappedOut() {
    try {
        if (!g_ourBox || !IsOurWindowCloaked()) {
            return;
        }
        std::wstring text{g_ourBox.Text()};
        if (text.empty()) {
            return;
        }
        g_swappedOutText = std::move(text);
        g_swappedOutTick = GetTickCount64();
        Wh_Log(L"focus: Start was swapped out; keeping '%ls' for its reopen", g_swappedOutText.c_str());
    } catch (...) {
    }
}

void RestoreSwappedOutText() {
    if (g_swappedOutText.empty()) {
        return;
    }
    std::wstring text = std::move(g_swappedOutText);
    g_swappedOutText.clear();
    if (!g_ourBox || GetTickCount64() - g_swappedOutTick > kSwappedOutTextLifetimeMs) {
        return;
    }
    try {
        g_ourBox.Text(text);
        RevealOverlayAnimated();
        g_ourBox.Focus(wux::FocusState::Programmatic);
        g_ourBox.SelectionStart(static_cast<int32_t>(text.size()));
        g_ourBox.SelectionLength(0);
    } catch (...) {
    }
}

// An Enter pressed before the results for the text were in, run when they
// arrive (RenderResults); dropped once Start closes, or if they take longer
// than this.
static bool g_enterWaiting = false;
static bool g_enterWaitingCtrl = false;
static ULONGLONG g_enterWaitingTick = 0;
constexpr ULONGLONG kEnterWaitMs = 3000;

void DismissStartMenu() {
    try {
        HidePreview();
        g_enterWaiting = false;
        g_suppressRefocus.store(true);
        if (g_openFocus) {
            g_openFocus.Stop();
            g_openFocus = nullptr;
        }
        if (g_shownFocus) {
            g_shownFocus.Stop();
            g_shownFocus = nullptr;
        }
        if (g_ourBox) {
            g_ourBox.Text(L"");
        }
        if (g_resultsHost) {
            g_resultsHost.Visibility(wux::Visibility::Visible);
            g_resultsHost.Opacity(0.0);
            g_resultsHost.IsHitTestVisible(false);
            if (g_resultsTranslate) g_resultsTranslate.Y(-8.0);
        }
        g_isOverlayVisible.store(false);
        g_isHiding.store(false);
        if (g_revealAnim) g_revealAnim.Stop();
        if (g_hideAnim) g_hideAnim.Stop();

        CloseStartMenu();
    } catch (...) {
    }
}

// Programs opened from Start are started by Explorer, as stock Start's are.
//
// ShellExecute in Start is carried out for it by sihost.exe, so a program
// opened here was never started by the foreground process. All it had towards
// coming to the front was a grant (AllowSetForegroundWindow), and Windows
// cancels grants on the next input: the Enter key coming back up, the mouse
// moving. A program slower to show its window than that -- Office, or one
// started for the first time -- opened behind the app Start was opened over.
// Some did not start at all that way: an app entry run through sihost lost
// what its shortcut passes (SteelSeries GG).
//
// A program started by the foreground process keeps its right through any
// input. Stock Start hands the foreground to Explorer and has Explorer start
// the program; DismissStartMenuForLaunch does the same with this mod's helper
// window in Explorer, and the launches run ShellExecute inside Explorer
// through the desktop's IShellDispatch2, the documented way to have Explorer
// open something.

// The helper window (ExplorerHelperThreadProc) of the explorer.exe running the
// taskbar and desktop: the process ShellExecuteInExplorer reaches.
HWND FindExplorerLaunchHolder() {
    DWORD shellPid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &shellPid);
    }
    for (HWND w = FindWindowExW(nullptr, nullptr, kExplorerHelperClassName, kExplorerHelperWindowName); w;
         w = FindWindowExW(nullptr, w, kExplorerHelperClassName, kExplorerHelperWindowName)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (shellPid && pid == shellPid) {
            return w;
        }
    }
    return nullptr;
}

// Runs ShellExecute in Explorer, with the default verb. Returns false when
// Explorer could not be asked; the caller then opens it here instead. Needs
// COM on the calling thread.
bool ShellExecuteInExplorer(const std::wstring& file, const std::wstring& params, const std::wstring& dir) {
    IShellWindows* windows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&windows))) ||
        !windows) {
        return false;
    }
    VARIANT empty{};
    long hwnd = 0;
    IDispatch* desktop = nullptr;
    HRESULT hr = windows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &desktop);
    windows->Release();
    if (hr != S_OK || !desktop) {
        return false;
    }
    IServiceProvider* provider = nullptr;
    IShellBrowser* browser = nullptr;
    IShellView* view = nullptr;
    IDispatch* background = nullptr;
    IShellFolderViewDual* folderView = nullptr;
    IDispatch* application = nullptr;
    IShellDispatch2* shell = nullptr;
    hr = desktop->QueryInterface(IID_PPV_ARGS(&provider));
    if (SUCCEEDED(hr)) hr = provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    if (SUCCEEDED(hr)) hr = browser->QueryActiveShellView(&view);
    if (SUCCEEDED(hr)) hr = view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&background));
    if (SUCCEEDED(hr)) hr = background->QueryInterface(IID_PPV_ARGS(&folderView));
    if (SUCCEEDED(hr)) hr = folderView->get_Application(&application);
    if (SUCCEEDED(hr)) hr = application->QueryInterface(IID_PPV_ARGS(&shell));
    if (SUCCEEDED(hr)) {
        BSTR fileArg = SysAllocString(file.c_str());
        VARIANT paramsArg{}, dirArg{}, verbArg{}, showArg{};
        if (!params.empty()) {
            paramsArg.vt = VT_BSTR;
            paramsArg.bstrVal = SysAllocString(params.c_str());
        }
        if (!dir.empty()) {
            dirArg.vt = VT_BSTR;
            dirArg.bstrVal = SysAllocString(dir.c_str());
        }
        showArg.vt = VT_I4;
        showArg.lVal = SW_SHOWNORMAL;
        hr = shell->ShellExecute(fileArg, paramsArg, dirArg, verbArg, showArg);
        SysFreeString(fileArg);
        VariantClear(&paramsArg);
        VariantClear(&dirArg);
    }
    for (IUnknown* object : std::initializer_list<IUnknown*>{shell, application, folderView, background, view, browser, provider, desktop}) {
        if (object) {
            object->Release();
        }
    }
    Wh_Log(L"launch: Explorer opened %ls (%08X)", file.c_str(), static_cast<unsigned>(hr));
    return SUCCEEDED(hr);
}

// An app entry as Explorer can open it: shell:AppsFolder and its name there,
// which keeps what its shortcut passes. Empty if that does not lead back to
// the same entry.
std::wstring AppsFolderPath(PCIDLIST_ABSOLUTE pidl) {
    std::wstring path;
    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) && item) {
        LPWSTR name = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &name)) && name) {
            path = std::wstring(L"shell:AppsFolder\\") + name;
            CoTaskMemFree(name);
        }
        item->Release();
    }
    PIDLIST_ABSOLUTE parsed = nullptr;
    bool same = !path.empty() && SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &parsed, 0, nullptr)) &&
                parsed && ILIsEqual(parsed, pidl);
    if (parsed) {
        ILFree(parsed);
    }
    return same ? path : std::wstring();
}

// Opens something from Start and closes Start, in the order Windows needs for
// the program to come to the front. Closing Start hands the foreground back to
// the app that had it before Start opened, and a program whose window appears
// after that may take the foreground only if it was started while Start held
// it (packaged apps get the right through their activation instead). That
// failed twice over: Start was closed before the launch, and on a click the
// shell hands SearchHost the foreground a few milliseconds later, before the
// launch thread got to it.
//
// So the launch is requested at once -- the search thread looks the app up
// while the results are still there -- but waits at g_launchGate: a moment for
// that hand-over, the foreground taken back from SearchHost if it happened,
// then handed on to Explorer, which starts the program (ShellExecuteInExplorer
// explains why), and once it has, Start is closed as usual, animation and all.
// A program that hands off to an instance already running passes its own
// right on, as single-instance programs do.
void DismissStartMenuForLaunch(std::function<void()> launch) {
    g_launchedAtTick.store(GetTickCount64());
    g_suppressRefocus.store(true);
    if (g_openFocus) {
        g_openFocus.Stop();
        g_openFocus = nullptr;
    }
    if (g_shownFocus) {
        g_shownFocus.Stop();
        g_shownFocus = nullptr;
    }
    wuc::CoreDispatcher dispatcher{nullptr};
    try {
        if (g_ourBox) {
            dispatcher = g_ourBox.Dispatcher();
        }
    } catch (...) {}
    if (!g_launchGate) {
        g_launchGate = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    }
    if (!dispatcher || !g_launchGate) {
        launch();
        DismissStartMenu();
        return;
    }
    ResetEvent(g_launchGate);
    const unsigned before = g_launchesDone.load();
    launch();
    SpawnTrackedLaunch([dispatcher, before] {
        Sleep(30);
        HWND ours = GetOurCoreWindow();
        HWND fg = GetForegroundWindow();
        if (ours && fg != ours && IsSearchHostWindow(fg)) {
            DWORD_PTR granted = 0;
            if (SendMessageTimeoutW(fg, StartForegroundRequestMessage(), 0, 0, SMTO_ABORTIFHUNG, 200, &granted) &&
                granted && SetForegroundWindow(ours)) {
                for (int i = 0; i < 20 && GetForegroundWindow() != ours; ++i) {
                    Sleep(10);
                }
            }
        }
        HWND holder = FindExplorerLaunchHolder();
        const bool handedOver = holder && ours && GetForegroundWindow() == ours && SetForegroundWindow(holder);
        if (!handedOver && holder) {
            DWORD explorerPid = 0;
            GetWindowThreadProcessId(holder, &explorerPid);
            AllowSetForegroundWindow(explorerPid);
        }
        Wh_Log(L"launch: foreground %ls Explorer", handedOver ? L"handed to" : L"NOT handed to");
        SetEvent(g_launchGate);
        for (int i = 0; i < 200 && g_launchesDone.load() == before && !g_quit.load(); ++i) {
            Sleep(10);
        }
        Wh_Log(L"launch: started with Start in front (%ls); closing Start",
               GetForegroundWindow() == ours ? L"yes" : L"no");
        if (!g_quit.load()) {
            dispatcher.RunAsync(wuc::CoreDispatcherPriority::Normal, wuc::DispatchedHandler{[] {
                DismissStartMenu();
                // Over: a Start opened again from here on is a new one.
                g_launchedAtTick.store(0);
            }});
        }
    });
}

void DisarmScrollTabStops(wux::DependencyObject const& root, int depth = 15) {
    if (!root || depth < 0) return;
    if (auto scroller = root.try_as<wuxc::ScrollViewer>()) {
        if (scroller.IsTabStop()) {
            SuppressShellElement(scroller.as<wux::FrameworkElement>(), false, false, /*tabStopOnly=*/true);
            Wh_Log(L"focus: proactively disabled IsTabStop on %ls", ElementLabel(root).c_str());
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        DisarmScrollTabStops(wuxm::VisualTreeHelper::GetChild(root, i), depth - 1);
    }
}

bool IsNavigationKey(winrt::Windows::System::VirtualKey key);
bool HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl);
bool ContextMenuHasFocus();
bool EscapeBelongsToContextMenu();
void NoteContextMenuOpened();
void NoteContextMenuClosed();

static HHOOK g_hGetMsgHook = nullptr;
static winrt::event_token g_charReceivedToken{};
static winrt::event_token g_keyDownToken{};
static winrt::event_token g_activatedToken{};
static winrt::event_token g_visibilityToken{};
static bool g_coreEventsHooked = false;

static DWORD s_lastCharTick = 0;
static wchar_t s_lastChar = 0;

bool ProcessKeyChar(wchar_t ch) {
    if (ch < 0x20 || ch == 0x7F) return false;
    if (!g_ourBox || !g_resultsHost) return false;

    // If our search box is already focused, let standard XAML TextBox handle typing natively.
    // Its TextChanged handler will automatically reveal the overlay and query results without double characters.
    try {
        auto focused = wux::Input::FocusManager::GetFocusedElement();
        if (focused && focused == g_ourBox) {
            if (!g_isOverlayVisible.load()) {
                RevealOverlayAnimated();
            }
            return false;
        }
    } catch (...) {}

    // Deduplicate rapid duplicate events (e.g. from WM_CHAR and CharacterReceived) within 60ms
    DWORD now = GetTickCount();
    if ((now - s_lastCharTick < 60) && s_lastChar == ch) {
        return true; // Consume duplicate event so XAML doesn't double-type it
    }
    s_lastCharTick = now;
    s_lastChar = ch;

    Wh_Log(L"key listener: intercepted char '%c' (0x%X) [overlayVisible=%d]",
        ch, static_cast<unsigned>(ch), g_isOverlayVisible.load() ? 1 : 0);
    try {
        g_suppressRefocus.store(false);
        RevealOverlayAnimated();
        std::wstring text{g_ourBox.Text()};
        text.push_back(ch);
        g_ourBox.Text(text);
        g_ourBox.Focus(wux::FocusState::Programmatic);
        g_ourBox.SelectionStart(static_cast<int32_t>(text.size()));
        g_ourBox.SelectionLength(0);
        return true;
    } catch (...) {
        return false;
    }
}

bool ProcessKeyCommand(WPARAM vk) {
    if (!g_ourBox || !g_resultsHost) return false;

    if (vk == VK_ESCAPE) {
        // With a result's context menu open, Escape closes the menu and
        // leaves the query alone.
        if (EscapeBelongsToContextMenu()) {
            return false;
        }
        if (g_isOverlayVisible.load() || g_isHiding.load()) {
            Wh_Log(L"key listener: intercepted Escape -> hiding overlay");
            try {
                if (g_ourBox) g_ourBox.Text(L"");
                HideOverlayAnimated();
            } catch (...) {}
            return true;
        }
        return false;
    }

    auto navKey = static_cast<winrt::Windows::System::VirtualKey>(vk);
    if (IsNavigationKey(navKey)) {
        if (g_isOverlayVisible.load()) {
            bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
            bool used = true;
            try {
                used = HandleNavigationKey(navKey, ctrl);
            } catch (...) {}
            return used;
        }
        return false;
    }

    if (vk == VK_BACK) {
        bool alreadyFocused = false;
        try {
            auto focused = wux::Input::FocusManager::GetFocusedElement();
            if (focused && focused == g_ourBox) alreadyFocused = true;
        } catch (...) {}

        if (!alreadyFocused) {
            std::wstring text{g_ourBox.Text()};
            if (!text.empty()) {
                text.pop_back();
                try {
                    g_ourBox.Text(text);
                    if (text.empty()) {
                        HideOverlayAnimated();
                    } else {
                        RevealOverlayAnimated();
                        g_ourBox.Focus(wux::FocusState::Programmatic);
                        g_ourBox.SelectionStart(static_cast<int32_t>(text.size()));
                    }
                } catch (...) {}
                return true;
            }
        }
    }

    return false;
}

LRESULT CALLBACK StartMenuGetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0 && lParam) {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg) {
            if (msg->message == WM_CHAR) {
                if (ProcessKeyChar(static_cast<wchar_t>(msg->wParam))) {
                    msg->message = WM_NULL;
                }
            } else if (msg->message == WM_KEYDOWN) {
                if (ProcessKeyCommand(msg->wParam)) {
                    msg->message = WM_NULL;
                }
            }
        }
    }
    return CallNextHookEx(g_hGetMsgHook, code, wParam, lParam);
}

void InstallMessageHook() {
    if (g_hGetMsgHook) return;
    DWORD tid = g_xamlThreadId.load();
    if (!tid) tid = GetCurrentThreadId();
    g_hGetMsgHook = SetWindowsHookExW(WH_GETMESSAGE, StartMenuGetMsgProc, NULL, tid);
    if (g_hGetMsgHook) {
        Wh_Log(L"key listener: installed WH_GETMESSAGE hook on thread %lu", tid);
    }
}

static bool g_subclassed = false;
static LRESULT CALLBACK StartMenuSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg && uMsg == GetTeardownMessage()) {
        Wh_Log(L"subclass: teardown message received");
        TeardownStartMenuUi();
        return kTeardownDone;
    }

    if (uMsg && uMsg == RemovalCheckMessage()) {
        return !g_removalPending.empty() &&
               std::hash<std::wstring>{}(g_removalPending) == static_cast<size_t>(lParam);
    }

    if (uMsg && uMsg == DragDoneMessage()) {
        g_dragging = false;
        if (IsOurWindowCloaked()) {
            return 0;  // closed meanwhile
        }
        if (wParam) {
            DismissStartMenu();  // dropped: Start's job is done
        } else if (g_ourBox) {
            try {
                g_ourBox.Focus(wux::FocusState::Programmatic);  // cancelled: back to typing
            } catch (...) {
            }
        }
        return 0;
    }

    static UINT s_uMsgTaskbarCreated = 0;
    if (s_uMsgTaskbarCreated == 0) {
        s_uMsgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    }
    if (uMsg && uMsg == s_uMsgTaskbarCreated) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray && hWnd) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", hWnd);
            Wh_Log(L"subclass: TaskbarCreated -> re-published WindhawkStartMenuHwnd %p", hWnd);
        }
        // Observed only: Start itself, and anything subclassed before this
        // mod, needs the broadcast too.
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_ACTIVATE) {
        HWND otherHwnd = reinterpret_cast<HWND>(lParam);
        DWORD otherPid = 0;
        if (otherHwnd) GetWindowThreadProcessId(otherHwnd, &otherPid);

        if (LOWORD(wParam) != WA_INACTIVE) {
            if (GetTickCount64() - g_launchedAtTick.load() < kLaunchHandoffMs) {
                // Taken back from SearchHost to launch (DismissStartMenuForLaunch).
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            g_suppressRefocus.store(false);
            g_dragging = false;  // in case Explorer never said the drag was over
            Wh_Log(L"subclass: WM_ACTIVATE (active, prev=%p) -> ready for search", otherHwnd);
            if (g_ourBox && !g_isOverlayVisible.load()) {
                g_ourBox.Text(L"");
            }
            if (g_resultsHost && !g_isOverlayVisible.load()) {
                g_resultsHost.Visibility(wux::Visibility::Visible);
                g_resultsHost.Opacity(0.0);
                g_resultsHost.IsHitTestVisible(false);
                if (g_resultsTranslate) g_resultsTranslate.Y(-8.0);
                SyncOverlayBackground();
            }
            RestoreSwappedOutText();
            TriggerMenuOpenFocus();
        } else {
            // If the other window is in our own process (e.g. context menu, flyout, tooltip), don't suppress refocus!
            if (otherPid == GetCurrentProcessId()) {
                Wh_Log(L"subclass: WM_ACTIVATE (inactive, internal other=%p) -> ignoring", otherHwnd);
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            if (g_dragging) {
                // Explorer's helper running a drag of a result (StartFileDrag).
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }

            if (SearchHostTookForegroundFromOpenStart()) {
                Wh_Log(L"subclass: WM_ACTIVATE (inactive) to SearchHost while open -> taking it back");
                ReclaimForegroundFromSearchHost();
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            StashTextIfSwappedOut();
            g_suppressRefocus.store(true);
            g_enterWaiting = false;
            HidePreview();
            Wh_Log(L"subclass: WM_ACTIVATE (inactive, other=%p pid=%lu) -> suppressing refocus", otherHwnd, otherPid);
            if (g_resultsHost) {
                g_resultsHost.Visibility(wux::Visibility::Visible);
                g_resultsHost.Opacity(0.0);
                g_resultsHost.IsHitTestVisible(false);
                if (g_resultsTranslate) g_resultsTranslate.Y(-8.0);
            }
            g_isOverlayVisible.store(false);
            g_isHiding.store(false);
            if (g_hideAnim) g_hideAnim.Stop();
            if (g_revealAnim) g_revealAnim.Stop();
            if (g_openFocus) {
                g_openFocus.Stop();
                g_openFocus = nullptr;
            }
        }
    } else if (uMsg == WM_SETFOCUS) {
        g_suppressRefocus.store(false);
        TriggerMenuOpenFocus();
    } else if (uMsg == WM_CHAR) {
        if (ProcessKeyChar(static_cast<wchar_t>(wParam))) {
            return 0;
        }
    } else if (uMsg == WM_KEYDOWN) {
        if (ProcessKeyCommand(wParam)) {
            return 0;
        }
    } else if (uMsg == WM_SETTINGCHANGE || uMsg == WM_THEMECHANGED) {
        Wh_Log(L"subclass: setting or theme changed -> refreshing theme and synchronizing overlay colors");
        RefreshThemeCache();
        SyncOverlayBackground();
        if (g_isOverlayVisible.load()) {
            RequestRender();
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void PlaceOurSearchBox(wux::FrameworkElement const& stockButton);

// The search box, found by walking rather than by being told.
//
// The stock SearchBoxToggleButton is what we replace, and anything else
// search-shaped gets suppressed so two boxes are never live at once.
wux::FrameworkElement FindStockSearchToggle(wux::DependencyObject const& root,
                                            int depth = 24) {
    if (!root || depth < 0) {
        return nullptr;
    }
    try {
        std::wstring_view type{winrt::get_class_name(root)};
        if (type == L"StartMenu.SearchBoxToggleButton") {
            if (auto fe = root.try_as<wux::FrameworkElement>()) {
                std::wstring name{fe.Name()};
                if (name.find(L"Windhawk") == std::wstring::npos) {
                    return fe;
                }
            }
        }
    } catch (...) {
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindStockSearchToggle(
                wuxm::VisualTreeHelper::GetChild(root, i), depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

std::atomic<bool> g_attaching{false};

// Runs on the XAML thread. Idempotent: it stops as soon as our box is in the
// tree, so being called on every shown window costs one failed lookup.
void TryAttachFromWindowRoot() {
    bool expected = false;
    if (!g_attaching.compare_exchange_strong(expected, true)) {
        return;
    }
    struct Guard {
        ~Guard() { g_attaching.store(false); }
    } guard;

    try {
        if (g_ourBox) {
            return;  // already placed
        }
        auto window = wux::Window::Current();
        if (!window) {
            return;  // not a XAML thread; most shown windows are not
        }
        auto content = window.Content();
        if (!content) {
            return;
        }
        auto toggle = FindStockSearchToggle(content);
        if (!toggle) {
            return;  // tree not built yet; the next shown window retries
        }
        g_xamlThreadId.store(GetCurrentThreadId());
        Wh_Log(L"attach: found SearchBoxToggleButton (%.0fx%.0f) on thread %lu",
            toggle.ActualWidth(), toggle.ActualHeight(), GetCurrentThreadId());
        PlaceOurSearchBox(toggle);
    } catch (...) {
        Wh_Log(L"attach: threw %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
}

// The hook lives on a thread of its own: UnhookWinEvent only works on the
// thread that called SetWinEventHook, and Wh_ModAfterInit and Wh_ModUninit
// don't always run on the same thread. A hook left behind keeps the DLL loaded
// and its callback running in Start after the mod is unloaded.
HANDLE g_attachWatchThread = nullptr;
HANDLE g_attachWatchStop = nullptr;
std::atomic<int> g_attachWatchCalls{0};

void CALLBACK AttachWatchProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                              LONG idObject, LONG idChild, DWORD, DWORD) {
    g_attachWatchCalls.fetch_add(1);
    struct Leave {
        ~Leave() { g_attachWatchCalls.fetch_sub(1); }
    } leave;
    if (g_quit.load()) {
        return;  // unloading: once the teardown clears g_ourBox, attaching would put the box back
    }
    if ((event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_UNCLOAKED) ||
        !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if ((event == EVENT_OBJECT_UNCLOAKED || event == EVENT_OBJECT_SHOW) && hwnd == GetOurCoreWindow()) {
        g_suppressRefocus.store(false);
        TriggerMenuOpenFocus();
    }
    // In-context, so this is the thread that raised the event. The filter is
    // Window::Current() returning something rather than a class name, because
    // a name that changes across builds breaks quietly.
    TryAttachFromWindowRoot();
}

DWORD WINAPI AttachWatchThreadProc(LPVOID) {
    // In-context, so the callback runs on whichever thread shows the window;
    // this thread only owns the hooks, and needs no message loop for them.
    // One hook per event: the range between them covers focus, location and
    // name changes too, each of which would be a call into the mod.
    const DWORD events[] = {EVENT_OBJECT_SHOW, EVENT_OBJECT_UNCLOAKED};
    HWINEVENTHOOK hooks[ARRAYSIZE(events)] = {};
    for (size_t i = 0; i < ARRAYSIZE(events); ++i) {
        hooks[i] = SetWinEventHook(events[i], events[i], GetCurrentModuleHandle(),
                                   AttachWatchProc, GetCurrentProcessId(), 0,
                                   WINEVENT_INCONTEXT);
        if (hooks[i]) {
            Wh_Log(L"attach watch: event 0x%X hooked on thread %lu", events[i], GetCurrentThreadId());
        } else {
            Wh_Log(L"attach watch: event 0x%X FAILED, err=%lu", events[i], GetLastError());
        }
    }
    WaitForSingleObject(g_attachWatchStop, INFINITE);
    for (HWINEVENTHOOK hook : hooks) {
        if (hook && !UnhookWinEvent(hook)) {
            Wh_Log(L"attach watch: unhook failed, err=%lu", GetLastError());
        }
    }
    return 0;
}

void StartAttachWatch() {
    if (g_attachWatchThread) {
        return;
    }
    g_attachWatchStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_attachWatchStop) {
        Wh_Log(L"attach watch: CreateEvent failed, err=%lu", GetLastError());
        return;
    }
    g_attachWatchThread = CreateThread(nullptr, 0, AttachWatchThreadProc, nullptr, 0, nullptr);
    if (!g_attachWatchThread) {
        Wh_Log(L"attach watch: CreateThread failed, err=%lu", GetLastError());
        CloseHandle(g_attachWatchStop);
        g_attachWatchStop = nullptr;
    }
}

// Must not run on Start's XAML thread: it waits for callbacks, which run there.
void StopAttachWatch() {
    if (!g_attachWatchThread) {
        return;
    }
    SetEvent(g_attachWatchStop);
    WaitForSingleObject(g_attachWatchThread, INFINITE);
    CloseHandle(g_attachWatchThread);
    g_attachWatchThread = nullptr;
    CloseHandle(g_attachWatchStop);
    g_attachWatchStop = nullptr;
    // A call that got in before the unhook may still be running elsewhere.
    for (int i = 0; g_attachWatchCalls.load() > 0 && i < 200; ++i) {
        Sleep(10);
    }
}

void SubclassStartMenuWindow() {
    HWND ours = nullptr;
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid != GetCurrentProcessId()) return TRUE;
            wchar_t cls[128] = {};
            GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                *reinterpret_cast<HWND*>(param) = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ours));
    if (ours) {
        g_hCoreWindow = ours;
        SetPropW(ours, L"WindhawkStartMenuWindow", (HANDLE)1);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", ours);
        }
        if (!g_subclassed) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(ours, StartMenuSubclassProc, 0)) {
                g_subclassed = true;
                Wh_Log(L"subclass: hooked CoreWindow HWND %p", ours);
            }
        }
    }
    InstallMessageHook();
}

// Results, handed from the search thread to the XAML thread.
std::mutex g_resultsMutex;
std::atomic<DWORD> g_totalMatches{0};

[[clang::no_destroy]] std::optional<std::thread> g_searchThread;
std::mutex g_queryMutex;
std::condition_variable g_queryWake;
std::wstring g_pendingQuery;
std::atomic<bool> g_searchQuit{false};
std::atomic<bool> g_queryDirty{false};

void RequestAppIndexRefresh() {
    ULONGLONG now = GetTickCount64();
    if (now - g_lastAppIndexRebuildTick.load() >= 5000) {
        {
            std::lock_guard<std::mutex> lock(g_queryMutex);
            g_appIndexNeedsRefresh.store(true);
        }
        g_queryWake.notify_all();
    }
}

// A row as the XAML thread needs it: text, an optional icon as raw BGRA, and
// what to do when it is clicked.
//
// Icons travel as pixels rather than as a path to fetch later, because the
// fetch is the slow part and it has already happened on the search thread.
struct Row {
    std::wstring title;
    std::wstring subtitle;
    std::wstring openPath;   // files: what ShellExecute opens
    int appIndex = -1;       // apps: which entry of the index to launch
    std::vector<BYTE> icon;  // premultiplied BGRA, IconPixels() square, or empty
    bool canRunAsAdmin = true;
    bool isSetting = false;
    bool isFolder = false;
    bool isFile = false;
    std::wstring programPath; // apps launched by app ID: their program on disk, for its file actions
    std::wstring copyText;   // text to copy to clipboard on activation
    std::wstring customGlyph; // Segoe Fluent glyph override (e.g. \uE1D0, \uE701, \uE88E)
    uint32_t glyphColor = 0;  // ARGB of the glyph -- a conversion's kind (calc::KindColor) -- or 0
};

// Icons take 24 logical pixels in a row, which on a display at 150% is 36 real
// ones. They are made at that real size (IconPixels), so XAML has nothing to
// scale.
inline constexpr int kIconDisplay = 24;  // what an icon occupies in the row, in DIPs

// The same in physical pixels on the display Start is on: the size icons are
// made at (see icons::Resample), so XAML draws them without scaling.
int IconPixels() {
    HWND start = GetOurCoreWindow();
    UINT dpi = start ? GetDpiForWindow(start) : 0;
    if (!dpi) {
        dpi = GetDpiForSystem();
    }
    return std::clamp(MulDiv(kIconDisplay, static_cast<int>(dpi), 96), 16, 256);
}

// A row's icon as a bitmap, or null when it has none.
wuxmi::WriteableBitmap IconBitmap(const std::vector<BYTE>& pixels) {
    const int side = icons::IconSide(pixels);
    if (!side) {
        return nullptr;
    }
    wuxmi::WriteableBitmap bitmap{side, side};
    auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* dest = nullptr;
    if (FAILED(access->Buffer(&dest)) || !dest) {
        return nullptr;
    }
    memcpy(dest, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

std::vector<Row> g_appRows;
std::vector<Row> g_fileRows;
std::wstring g_rowsQuery;  // the text g_appRows and g_fileRows are the results for

// Launch requests, posted from the XAML thread back to the search thread,
// which owns the app index and therefore the PIDLs.
//
// A PIDL cannot simply be captured in a click handler: the index is rebuilt
// and the pointer would dangle. Sending an index back to the owning thread
// keeps the lifetime where the data lives.
std::atomic<int> g_launchRequest{-1};
std::atomic<bool> g_launchAsAdmin{false};



// Opens what was clicked.
//
// ShellExecute rather than CreateProcess: these are paths of any kind, and
// the shell decides what opening one means. This process runs at the user's
// own integrity, so what opens is what the user would have opened -- which is
// exactly what the broker could not promise when it ran elevated.
void OpenResult(std::wstring path, bool asAdmin = false) {
    SpawnTrackedLaunch([path = std::move(path), asAdmin] {
        WaitForLaunchGate();
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        wchar_t userProfile[MAX_PATH] = {};
        if (!GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH) || !userProfile[0]) {
            PWSTR kf = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &kf)) && kf) {
                wcsncpy_s(userProfile, kf, MAX_PATH - 1);
                CoTaskMemFree(kf);
            }
        }

        std::wstring lowerPath = path;
        for (auto& c : lowerPath) c = static_cast<wchar_t>(towlower(c));
        bool isTerminalOrShell = (lowerPath.find(L"cmd.exe") != std::wstring::npos ||
                                  lowerPath.find(L"powershell") != std::wstring::npos ||
                                  lowerPath.find(L"pwsh") != std::wstring::npos ||
                                  lowerPath.find(L"windowsterminal") != std::wstring::npos ||
                                  lowerPath.ends_with(L"wt.exe"));

        size_t lastSlash = path.find_last_of(L"\\/");
        std::wstring parentDir = (lastSlash != std::wstring::npos) ? path.substr(0, lastSlash) : L"";
        LPCWSTR workDir = isTerminalOrShell ? userProfile : (!parentDir.empty() ? parentDir.c_str() : userProfile);

        std::wstring params;
        if (lowerPath.ends_with(L"cmd.exe")) {
            params = L"/k cd /d \"" + std::wstring(userProfile) + L"\"";
        } else if (lowerPath.find(L"powershell.exe") != std::wstring::npos || lowerPath.ends_with(L"pwsh.exe")) {
            params = L"-NoExit -Command \"Set-Location '" + std::wstring(userProfile) + L"'\"";
        }

        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        if (!params.empty()) {
            info.lpParameters = params.c_str();
        }
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if ((asAdmin || !ShellExecuteInExplorer(path, params, workDir)) && !ShellExecuteExW(&info)) {
            Wh_Log(L"open failed (%lu): %ls (admin=%d)", GetLastError(), path.c_str(), asAdmin ? 1 : 0);
        }
        g_launchesDone.fetch_add(1);
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

// Lets the taskbar's explorer.exe, where the Properties relay runs, bring the
// sheet it opens for us to the front. Only a process that just got user input
// may hand that right on, and Start stops being one once it closes, so this
// is called from the click handler, before DismissStartMenu.
void AllowExplorerForeground() {
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD explorerPid = 0;
        GetWindowThreadProcessId(tray, &explorerPid);
        if (explorerPid) {
            AllowSetForegroundWindow(explorerPid);
        }
    }
}

// Opens a folder window with the file selected, through the shell rather than
// by starting explorer.exe /select. Which explorer.exe gets the window is not
// ours to know -- a started one hands the request on and exits, and the window
// lands in another it never met -- but the shell's own call reaches that
// process over COM and hands it our right to the foreground on the way. Runs
// on a worker (OpenFileLocationThenDismiss), while Start is still in front.
void OpenFileLocation(const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr) && pidl) {
        hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }
    if (FAILED(hr)) {
        Wh_Log(L"open location failed (%08X): %ls", static_cast<unsigned>(hr), path.c_str());
    }
}

// "Open file location" from a menu: the folder opens on a worker, and Start is
// dismissed after, back on its thread. The shell call waits on Explorer and
// pumps messages meanwhile; in the click handler, Start losing the foreground
// then closed the menu and rebuilt the results under the running handler, and
// a slow or disconnected share froze Start. The foreground right belongs to
// the process, so the folder still opens in front.
void OpenFileLocationThenDismiss(std::wstring path) {
    wuc::CoreDispatcher dispatcher{nullptr};
    try {
        if (g_ourBox) {
            dispatcher = g_ourBox.Dispatcher();
        }
    } catch (...) {}
    SpawnTrackedLaunch([path = std::move(path), dispatcher] {
        HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        OpenFileLocation(path);
        if (SUCCEEDED(co)) {
            CoUninitialize();
        }
        if (dispatcher && !g_quit.load()) {
            dispatcher.RunAsync(wuc::CoreDispatcherPriority::Normal,
                                wuc::DispatchedHandler{[] { DismissStartMenu(); }});
        }
    });
}

// Every explorer.exe hosts a relay window (StartExplorerHelperHost). The one in
// the taskbar's process is used when it exists: that is the process the
// Properties click lets take the foreground, so the dialog opens in front.
HWND FindExplorerHelperWindow() {
    DWORD trayPid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &trayPid);
    }
    HWND any = nullptr;
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, kExplorerHelperClassName, kExplorerHelperWindowName); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, kExplorerHelperClassName, kExplorerHelperWindowName)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == trayPid) {
            return hwnd;
        }
        if (!any) {
            any = hwnd;
        }
    }
    return any;
}

// What Uninstall in an app's menu does for an app. A packaged app is removed
// for this user, unless it is part of Windows. A program runs the uninstaller
// it registered for Settings' Installed apps -- found by its program file,
// install folder or name -- and one that registered none, or that is part of
// Windows, gets no Uninstall.
namespace uninstall {

struct Command {
    std::wstring name;    // as Installed apps shows it
    std::wstring file;    // the uninstaller
    std::wstring params;
};

inline bool EqualsI(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() && CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_EQUAL;
}

// Whether path is dir or inside it.
inline bool IsUnder(const std::wstring& path, const std::wstring& dir) {
    if (dir.empty() || path.size() < dir.size() ||
        CompareStringOrdinal(path.c_str(), static_cast<int>(dir.size()), dir.c_str(), static_cast<int>(dir.size()),
                             TRUE) != CSTR_EQUAL) {
        return false;
    }
    return path.size() == dir.size() || path[dir.size()] == L'\\' || dir.back() == L'\\';
}

inline std::wstring Folder(const std::wstring& path) {
    size_t slash = path.find_last_of(L'\\');
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}

// A registry path value as a plain path: environment variables expanded,
// without quotes, an icon index (",0") or a trailing backslash.
inline std::wstring CleanPath(std::wstring s, bool iconIndex) {
    if (s.find(L'%') != std::wstring::npos) {
        wchar_t expanded[MAX_PATH * 2];
        DWORD n = ExpandEnvironmentStringsW(s.c_str(), expanded, ARRAYSIZE(expanded));
        if (n && n <= ARRAYSIZE(expanded)) {
            s = expanded;
        }
    }
    auto trim = [&s] {
        while (!s.empty() && (s.front() == L' ' || s.front() == L'"')) s.erase(s.begin());
        while (!s.empty() && (s.back() == L' ' || s.back() == L'"')) s.pop_back();
    };
    trim();
    if (iconIndex) {
        size_t comma = s.find_last_of(L',');
        if (comma != std::wstring::npos && comma + 1 < s.size() &&
            s.find_first_not_of(L"-0123456789 ", comma + 1) == std::wstring::npos) {
            s.resize(comma);
            trim();
        }
    }
    while (s.size() > 3 && s.back() == L'\\') s.pop_back();
    return s;
}

// Splits a command line into the program and its arguments: the quoted part,
// else up to the first ".exe" (uninstall strings are often unquoted paths
// with spaces), else up to the first space.
inline void SplitCommand(const std::wstring& line, std::wstring& file, std::wstring& params) {
    std::wstring s = line;
    while (!s.empty() && s.front() == L' ') s.erase(s.begin());
    size_t end;
    if (!s.empty() && s.front() == L'"') {
        end = s.find(L'"', 1);
        file = s.substr(1, end == std::wstring::npos ? std::wstring::npos : end - 1);
        end = end == std::wstring::npos ? s.size() : end + 1;
    } else {
        std::wstring lower = s;
        for (auto& c : lower) c = static_cast<wchar_t>(towlower(c));
        size_t exe = lower.find(L".exe");
        end = exe != std::wstring::npos ? exe + 4 : s.find(L' ');
        if (end == std::wstring::npos) end = s.size();
        file = s.substr(0, end);
    }
    params = end < s.size() ? s.substr(end) : std::wstring();
    while (!params.empty() && params.front() == L' ') params.erase(params.begin());
    if (file.find(L'%') != std::wstring::npos) {
        file = CleanPath(file, false);
    }
}

inline std::wstring ReadString(HKEY key, const wchar_t* name) {
    wchar_t buf[2048];
    DWORD size = sizeof(buf) - sizeof(wchar_t);
    DWORD type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<BYTE*>(buf), &size) != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ)) {
        return {};
    }
    buf[size / sizeof(wchar_t)] = 0;
    return buf;
}

inline DWORD ReadDword(HKEY key, const wchar_t* name) {
    DWORD value = 0, size = sizeof(value), type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<BYTE*>(&value), &size) != ERROR_SUCCESS ||
        type != REG_DWORD) {
        return 0;
    }
    return value;
}

// Folders that hold many programs, which an install folder must not be taken
// for (some installers register one as theirs).
inline std::vector<std::wstring> SharedFolders() {
    static const KNOWNFOLDERID* const kShared[] = {
        &FOLDERID_ProgramFiles,       &FOLDERID_ProgramFilesX86, &FOLDERID_ProgramFilesCommon,
        &FOLDERID_ProgramFilesCommonX86, &FOLDERID_LocalAppData,  &FOLDERID_RoamingAppData,
        &FOLDERID_UserProgramFiles,   &FOLDERID_Windows,         &FOLDERID_Profile,
        &FOLDERID_ProgramData,
    };
    std::vector<std::wstring> folders;
    for (const KNOWNFOLDERID* id : kShared) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, KF_FLAG_DONT_VERIFY, nullptr, &path)) && path) {
            folders.push_back(path);
        }
        CoTaskMemFree(path);
    }
    return folders;
}

inline bool IsSharedFolder(const std::wstring& dir, const std::vector<std::wstring>& shared) {
    if (dir.size() <= 3) {
        return true;  // a drive
    }
    for (const std::wstring& folder : shared) {
        if (EqualsI(dir, folder)) {
            return true;
        }
    }
    return false;
}

inline bool IsWindowsPath(const std::wstring& path) {
    wchar_t windir[MAX_PATH];
    UINT n = GetWindowsDirectoryW(windir, MAX_PATH);
    return n && n < MAX_PATH && IsUnder(path, windir);
}

// How well an entry's name fits the app's name in Start: 2 the same, 1 the
// same but for a version ("NVIDIA App 11.0.9" for "NVIDIA App"), or the
// start of it, at least two words long ("File Converter" for "File
// Converter Settings"), else 0.
inline int NameMatch(const std::wstring& name, const std::wstring& title) {
    if (name.empty() || title.empty()) {
        return 0;
    }
    if (EqualsI(name, title)) {
        return 2;
    }
    auto startsWithWord = [](const std::wstring& s, const std::wstring& prefix) {
        return s.size() > prefix.size() + 1 && s[prefix.size()] == L' ' &&
               CompareStringOrdinal(s.c_str(), static_cast<int>(prefix.size()), prefix.c_str(),
                                    static_cast<int>(prefix.size()), TRUE) == CSTR_EQUAL;
    };
    if (startsWithWord(name, title)) {
        const wchar_t next = name[title.size() + 1];
        const wchar_t after = title.size() + 2 < name.size() ? name[title.size() + 2] : 0;
        if (iswdigit(next) || next == L'(' || ((next == L'v' || next == L'V') && iswdigit(after))) {
            return 1;
        }
    }
    if (startsWithWord(title, name) && name.find(L' ') != std::wstring::npos) {
        return 1;
    }
    return 0;
}

// The uninstaller registered for a program (its file, or empty when not
// known) and its name in Start.
inline std::optional<Command> FindProgramUninstaller(const std::wstring& program, const std::wstring& title) {
    if (!program.empty() && IsWindowsPath(program)) {
        return std::nullopt;
    }
    const std::wstring programDir = Folder(program);
    const std::vector<std::wstring> shared = SharedFolders();
    struct Root {
        HKEY hive;
        REGSAM view;
    };
    static const Root kRoots[] = {
        {HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY},
        {HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY},
        {HKEY_CURRENT_USER, 0},
    };
    std::optional<Command> best;
    int bestScore = 0;
    size_t bestDepth = 0;
    for (const Root& root : kRoots) {
        HKEY list = nullptr;
        if (RegOpenKeyExW(root.hive, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0,
                          KEY_READ | root.view, &list) != ERROR_SUCCESS) {
            continue;
        }
        wchar_t sub[256];
        for (DWORD i = 0;; ++i) {
            DWORD subLen = ARRAYSIZE(sub);
            if (RegEnumKeyExW(list, i, sub, &subLen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
                break;
            }
            HKEY entry = nullptr;
            if (RegOpenKeyExW(list, sub, 0, KEY_READ | root.view, &entry) != ERROR_SUCCESS) {
                continue;
            }
            // As Installed apps lists them: named, removable, not an update.
            const std::wstring name = ReadString(entry, L"DisplayName");
            const std::wstring uninstallString = ReadString(entry, L"UninstallString");
            const bool listed = !name.empty() && !uninstallString.empty() && !ReadDword(entry, L"SystemComponent") &&
                                !ReadDword(entry, L"NoRemove") && ReadString(entry, L"ParentKeyName").empty();

            int score = 0;
            size_t depth = 0;
            if (listed) {
                std::wstring file, params;
                SplitCommand(uninstallString, file, params);
                const std::wstring icon = CleanPath(ReadString(entry, L"DisplayIcon"), true);
                const std::wstring location = CleanPath(ReadString(entry, L"InstallLocation"), false);
                const std::wstring fileName = file.substr(file.find_last_of(L'\\') + 1);
                const bool sharedUninstaller = EqualsI(fileName, L"msiexec.exe") || EqualsI(fileName, L"rundll32.exe");
                // A launcher's uninstaller removes something else -- a game:
                // steam.exe steam://uninstall/730.
                const bool forOther = EqualsI(file, program) || params.find(L"://") != std::wstring::npos;
                int rule = 0;
                if (!program.empty() && EqualsI(icon, program)) {
                    rule = 4;
                } else if (!program.empty() && !location.empty() && IsUnder(program, location) &&
                           !IsSharedFolder(location, shared)) {
                    rule = 3;
                    depth = location.size();
                } else if (!program.empty() && !sharedUninstaller && !forOther && !programDir.empty() &&
                           (IsUnder(file, programDir) || IsUnder(icon, programDir)) && !IsSharedFolder(programDir, shared)) {
                    rule = 2;
                }
                const int named = NameMatch(name, title);
                if (!rule && named) {
                    rule = 1;
                }
                score = rule ? rule * 4 + named : 0;
                if (score > bestScore || (score == bestScore && score && depth > bestDepth)) {
                    // MSI's /I opens its repair-or-remove dialog; /X removes.
                    if (EqualsI(fileName, L"msiexec.exe") || EqualsI(file, L"msiexec")) {
                        for (size_t at = 0; at + 1 < params.size(); ++at) {
                            if (params[at] == L'/' && (params[at + 1] == L'I' || params[at + 1] == L'i')) {
                                params[at + 1] = L'X';
                                break;
                            }
                        }
                    }
                    best = Command{name, file, params};
                    bestScore = score;
                    bestDepth = depth;
                }
            }
            RegCloseKey(entry);
        }
        RegCloseKey(list);
    }
    return best;
}

// The package family of a packaged app's app ID ("family!app") when it can be
// uninstalled: installed for this user and not part of Windows (those live
// under the Windows folder, in SystemApps).
inline std::wstring RemovablePackageFamily(const std::wstring& appId) {
    const size_t bang = appId.find(L'!');
    if (bang == std::wstring::npos || bang == 0 || appId.find(L'\\') != std::wstring::npos) {
        return {};
    }
    const std::wstring family = appId.substr(0, bang);
    UINT32 count = 0, length = 0;
    if (GetPackagesByPackageFamily(family.c_str(), &count, nullptr, &length, nullptr) != ERROR_INSUFFICIENT_BUFFER ||
        !count) {
        return {};
    }
    std::vector<PWSTR> names(count);
    std::vector<wchar_t> buffer(length);
    if (GetPackagesByPackageFamily(family.c_str(), &count, names.data(), &length, buffer.data()) != ERROR_SUCCESS ||
        !count) {
        return {};
    }
    wchar_t path[MAX_PATH];
    UINT32 pathLength = MAX_PATH;
    if (GetPackagePathByFullName(names[0], &pathLength, path) != ERROR_SUCCESS || IsWindowsPath(path)) {
        return {};
    }
    return family;
}

}  // namespace uninstall

// Has Explorer remove a packaged app (AcceptPackageRemoval).
bool RequestPackageRemoval(const std::wstring& family) {
    HWND helper = FindExplorerHelperWindow();
    if (!helper) {
        return false;
    }
    std::wstring request = family;
    COPYDATASTRUCT cds{};
    cds.dwData = kExplorerUninstallMagic;
    cds.cbData = static_cast<DWORD>((request.size() + 1) * sizeof(wchar_t));
    cds.lpData = request.data();
    DWORD_PTR accepted = 0;
    g_removalPending = family;  // Explorer asks back (RemovalCheckMessage)
    const bool ok = SendMessageTimeoutW(helper, WM_COPYDATA, reinterpret_cast<WPARAM>(GetOurCoreWindow()),
                                        reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG, 2000, &accepted) &&
                    accepted;
    g_removalPending.clear();
    return ok;
}

// Starts a program's uninstaller, through Explorer like any program opened
// from Start (DismissStartMenuForLaunch), so it comes to the front -- and
// asks for elevation itself when it needs it.
void RunUninstaller(uninstall::Command command) {
    SpawnTrackedLaunch([command = std::move(command)] {
        WaitForLaunchGate();
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        const std::wstring dir = uninstall::Folder(command.file);
        if (!ShellExecuteInExplorer(command.file, command.params, dir)) {
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpFile = command.file.c_str();
            sei.lpParameters = command.params.c_str();
            sei.lpDirectory = dir.empty() ? nullptr : dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        Wh_Log(L"uninstall: ran %ls %ls", command.file.c_str(), command.params.c_str());
        g_launchesDone.fetch_add(1);
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

double VisibleTop();

// Asks before uninstalling, as Start's own Uninstall does: a flyout on the
// app's card naming what goes.
void ConfirmUninstall(wux::FrameworkElement const& anchor, std::wstring const& name, std::function<void()> uninstallNow) {
    wuxc::Flyout flyout;
    wuxc::StackPanel panel;
    panel.MaxWidth(300);
    wuxc::TextBlock text;
    text.Text(winrt::hstring{name + L" and its related info will be uninstalled."});
    text.TextWrapping(wux::TextWrapping::Wrap);
    panel.Children().Append(text);
    wuxc::Button confirm;
    confirm.Content(winrt::box_value(L"Uninstall"));
    confirm.HorizontalAlignment(wux::HorizontalAlignment::Right);
    confirm.Margin(wux::ThicknessHelper::FromLengths(0, 12, 0, 0));
    try {
        auto resources = wux::Application::Current().Resources();
        auto key = winrt::box_value(L"AccentButtonStyle");
        if (resources.HasKey(key)) {
            confirm.Style(resources.Lookup(key).as<wux::Style>());
        }
    } catch (...) {
    }
    KeepHandler(confirm, confirm.Click(winrt::auto_revoke, [weakFlyout = winrt::make_weak(flyout), uninstallNow](
                                                               wf::IInspectable const&, wux::RoutedEventArgs const&) {
        if (auto f = weakFlyout.get()) {
            f.Hide();
        }
        uninstallNow();
    }));
    panel.Children().Append(confirm);
    flyout.Content(panel);
    // Above the card, where XAML puts it, unless it would reach past the top
    // of what Start shows (VisibleTop) -- XAML sees room there that is not
    // drawn, as on the first card -- then below.
    try {
        panel.Measure(wf::Size{300, std::numeric_limits<float>::infinity()});
        constexpr double kChrome = 40;  // the flyout's padding, border and gap to the card
        const double cardTop = anchor.TransformToVisual(nullptr).TransformPoint(wf::Point{0, 0}).Y;
        if (cardTop - panel.DesiredSize().Height - kChrome < VisibleTop()) {
            flyout.Placement(wuxcp::FlyoutPlacementMode::Bottom);
        }
    } catch (...) {
    }
    KeepHandler(flyout, flyout.Opened(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
        NoteContextMenuOpened();
    }));
    KeepHandler(flyout, flyout.Closed(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
        NoteContextMenuClosed();
    }));
    flyout.ShowAt(anchor);
}

void ShowPropertiesDialog(std::wstring path) {
    SpawnTrackedLaunch([path = std::move(path)] {
        // Give Start a moment to close first (DismissStartMenu asks the shell
        // for it), so the dialog does not open underneath it.
        Sleep(150);

        std::wstring cleanPath = path;
        while (!cleanPath.empty() && (cleanPath.front() == L' ' || cleanPath.front() == L'\t' || cleanPath.front() == L'"')) {
            cleanPath.erase(cleanPath.begin());
        }
        while (!cleanPath.empty() && (cleanPath.back() == L' ' || cleanPath.back() == L'\t' || cleanPath.back() == L'"')) {
            cleanPath.pop_back();
        }

        if (cleanPath.empty()) return;

        // 1. Relay to Explorer host window (runs at Medium integrity desktop shell)
        HWND hHost = nullptr;
        for (int retry = 0; retry < 3 && !hHost; ++retry) {
            hHost = FindExplorerHelperWindow();
            if (!hHost) Sleep(50);
        }

        if (hHost && IsWindow(hHost)) {
            COPYDATASTRUCT cds{};
            cds.dwData = kExplorerCopyDataMagic;
            cds.cbData = static_cast<DWORD>((cleanPath.size() + 1) * sizeof(wchar_t));
            cds.lpData = const_cast<wchar_t*>(cleanPath.c_str());

            DWORD_PTR dwResult = 0;
            LRESULT lres = SendMessageTimeoutW(
                hHost,
                WM_COPYDATA,
                0,
                reinterpret_cast<LPARAM>(&cds),
                SMTO_ABORTIFHUNG | SMTO_NORMAL,
                3000,
                &dwResult
            );
            Wh_Log(L"ShowPropertiesDialog: SendMessageTimeoutW to explorer host returned %ld, res=%llu for %ls",
                   lres, (unsigned long long)dwResult, cleanPath.c_str());
            if (lres && dwResult == 1) {
                return; // Successfully handed off to Explorer
            }
        }

        Wh_Log(L"ShowPropertiesDialog: explorer host not reachable, trying direct fallback for %ls", cleanPath.c_str());

        // 2. Direct in-process fallback
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        BOOL ok = SHObjectProperties(nullptr, 0x00000002 /* SHOP_FILEPATH */, cleanPath.c_str(), nullptr);
        if (!ok) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            SFGAOF sfgao = 0;
            if (SUCCEEDED(SHParseDisplayName(cleanPath.c_str(), nullptr, &pidl, 0, &sfgao)) && pidl) {
                SHELLEXECUTEINFOW sei{};
                sei.cbSize = sizeof(sei);
                sei.fMask = SEE_MASK_INVOKEIDLIST;
                sei.hwnd = nullptr;
                sei.lpIDList = pidl;
                sei.lpVerb = L"properties";
                sei.nShow = SW_SHOWNORMAL;
                ok = ShellExecuteExW(&sei);
                CoTaskMemFree(pidl);
            }
        }

        Sleep(1000);

        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

void LaunchTerminal(const std::wstring& dir, bool isPowerShell, bool asAdmin) {
    SpawnTrackedLaunch([dir, isPowerShell, asAdmin] {
        WaitForLaunchGate();
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (isPowerShell) {
            std::wstring psTarget = dir;
            size_t pos = 0;
            while ((pos = psTarget.find(L'\'', pos)) != std::wstring::npos) {
                psTarget.insert(pos, L"'");
                pos += 2;
            }
            std::wstring params = L"-NoExit -Command Set-Location -LiteralPath '" + psTarget + L"'";
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = asAdmin ? L"runas" : L"open";
            sei.lpFile = L"powershell.exe";
            sei.lpParameters = params.c_str();
            sei.lpDirectory = dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (asAdmin || !ShellExecuteInExplorer(sei.lpFile, params, dir)) {
                ShellExecuteExW(&sei);
            }
        } else {
            std::wstring cmdTarget = dir;
            if (cmdTarget.size() > 3 && cmdTarget.back() == L'\\') {
                cmdTarget.pop_back();
            }
            std::wstring params = L"/K cd /d \"" + cmdTarget + (cmdTarget.back() == L'\\' ? L"\\\"" : L"\"");
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = asAdmin ? L"runas" : L"open";
            sei.lpFile = L"cmd.exe";
            sei.lpParameters = params.c_str();
            sei.lpDirectory = dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (asAdmin || !ShellExecuteInExplorer(sei.lpFile, params, dir)) {
                ShellExecuteExW(&sei);
            }
        }
        g_launchesDone.fetch_add(1);
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

inline std::wstring UrlEncode(const std::wstring& str) {
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) return L"";
    std::string utf8(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &utf8[0], utf8Len, nullptr, nullptr);

    std::string encoded;
    const char hex[] = "0123456789ABCDEF";
    for (size_t i = 0; i < utf8.size() - 1; ++i) {
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += static_cast<char>(c);
        } else if (c == ' ') {
            encoded += '+';
        } else {
            encoded += '%';
            encoded += hex[(c >> 4) & 0x0F];
            encoded += hex[c & 0x0F];
        }
    }
    return std::wstring(encoded.begin(), encoded.end());
}

struct ResolvedWebQuery {
    std::wstring serviceName;
    std::wstring queryTerm;
    std::wstring searchUrl;
    std::wstring homeUrl;
    bool isShortcut = false;
};

inline std::wstring DeriveHomeUrl(const std::wstring& urlTemplate) {
    size_t qMark = urlTemplate.find(L"?");
    if (qMark != std::wstring::npos) {
        size_t slash = urlTemplate.find(L"/", 8); // after https://
        if (slash != std::wstring::npos && slash < qMark) {
            return urlTemplate.substr(0, slash + 1);
        }
        return urlTemplate.substr(0, qMark);
    }
    return urlTemplate;
}

inline std::wstring SubstituteQuery(const std::wstring& urlTemplate, const std::wstring& query) {
    if (query.empty()) return DeriveHomeUrl(urlTemplate);
    std::wstring encoded = UrlEncode(query);
    std::wstring res = urlTemplate;
    size_t pos = res.find(L"{q}");
    if (pos != std::wstring::npos) {
        res.replace(pos, 3, encoded);
        return res;
    }
    pos = res.find(L"{searchTerms}");
    if (pos != std::wstring::npos) {
        res.replace(pos, 13, encoded);
        return res;
    }
    if (res.find(L"?") == std::wstring::npos) {
        res += L"?q=" + encoded;
    } else {
        res += L"&q=" + encoded;
    }
    return res;
}

inline std::wstring DeriveEngineName(const std::wstring& url) {
    std::wstring lower = url;
    for (auto& c : lower) c = static_cast<wchar_t>(towlower(c));
    if (lower.find(L"duckduckgo") != std::wstring::npos) return L"DuckDuckGo";
    if (lower.find(L"google") != std::wstring::npos) return L"Google";
    if (lower.find(L"bing") != std::wstring::npos) return L"Bing";
    if (lower.find(L"brave") != std::wstring::npos) return L"Brave";
    if (lower.find(L"yahoo") != std::wstring::npos) return L"Yahoo";
    if (lower.find(L"ecosia") != std::wstring::npos) return L"Ecosia";
    if (lower.find(L"kagi") != std::wstring::npos) return L"Kagi";
    if (lower.find(L"startpage") != std::wstring::npos) return L"Startpage";
    return L"Web";
}

inline ResolvedWebQuery ResolveWebSearch(const std::wstring& input) {
    ResolvedWebQuery res;
    std::wstring text = input;
    while (!text.empty() && text.front() == L' ') text.erase(0, 1);

    std::vector<WebShortcut> shortcuts;
    std::wstring defUrl;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        shortcuts = g_settings.webShortcuts;
        defUrl = g_settings.defaultSearchUrl;
    }
    if (defUrl.empty()) defUrl = L"https://duckduckgo.com/?q={q}";

    // Check if first token matches any shortcut
    size_t delim = text.find_first_of(L" :");
    std::wstring firstWord = (delim != std::wstring::npos) ? text.substr(0, delim) : text;
    std::wstring firstWordLower = firstWord;
    for (auto& c : firstWordLower) c = static_cast<wchar_t>(towlower(c));

    for (const auto& sc : shortcuts) {
        if (!firstWordLower.empty() && firstWordLower == sc.prefix) {
            res.isShortcut = true;
            res.serviceName = sc.name;
            std::wstring remainder = (delim != std::wstring::npos) ? text.substr(delim + 1) : L"";
            while (!remainder.empty() && remainder.front() == L' ') remainder.erase(0, 1);
            res.queryTerm = remainder;
            res.homeUrl = DeriveHomeUrl(sc.url);
            res.searchUrl = SubstituteQuery(sc.url, res.queryTerm);
            return res;
        }
    }

    // Default search engine (DuckDuckGo or user configured in settings)
    res.isShortcut = false;
    res.serviceName = DeriveEngineName(defUrl);
    res.queryTerm = text;
    res.homeUrl = DeriveHomeUrl(defUrl);
    res.searchUrl = SubstituteQuery(defUrl, text);
    return res;
}

[[clang::no_destroy]] wuxc::TextBlock g_footerStatus{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_footerHints{nullptr};

void QueueQuery(std::wstring text);

std::vector<Row> g_currentAppRows;
std::vector<Row> g_currentFileRows;
static int g_selectedApp = -1;
static int g_selectedFile = -1;
// Which column Up, Down and Enter act on.
static bool g_filesColumnActive = false;
static uint64_t g_lastNavTick = 0;

// Highlights the selected button of a column and clears the rest; -1 clears
// them all.
void PaintColumnSelection(const std::optional<std::vector<wuxc::Button>>& buttons, int selected) {
    if (!buttons) {
        return;
    }
    bool isLight = IsLightTheme();
    for (size_t i = 0; i < buttons->size(); ++i) {
        const auto& btn = (*buttons)[i];
        if (!btn) continue;

        if (static_cast<int>(i) == selected) {
            if (isLight) {
                btn.Background(MakeBrush(0x14, 0x00, 0x5F, 0xB8));
                btn.BorderBrush(MakeBrush(0x80, 0x00, 0x5F, 0xB8));
            } else {
                btn.Background(MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
                btn.BorderBrush(MakeBrush(0x55, 0x60, 0xCD, 0xFF));
            }
            try {
                btn.StartBringIntoView();
            } catch (...) {}
        } else {
            btn.Background(MakeBrush(0, 0, 0, 0));
            btn.BorderBrush(MakeBrush(0, 0, 0, 0));
        }
    }
}

void SchedulePreview();

// Selects an app card, which makes Apps the active column.
void SetAppSelection(int index) {
    if (!g_appButtonsOpt || g_appButtonsOpt->empty()) {
        g_selectedApp = -1;
        return;
    }
    if (index < 0) index = 0;
    if (index >= static_cast<int>(g_appButtonsOpt->size())) {
        index = static_cast<int>(g_appButtonsOpt->size()) - 1;
    }
    g_selectedApp = index;
    g_filesColumnActive = false;
    PaintColumnSelection(g_appButtonsOpt, index);
    PaintColumnSelection(g_fileButtonsOpt, -1);
    SchedulePreview();
}

// Selects a file row, which makes Files the active column.
void SetFileSelection(int index) {
    if (!g_fileButtonsOpt || g_fileButtonsOpt->empty()) {
        g_selectedFile = -1;
        return;
    }
    if (index < 0) index = 0;
    if (index >= static_cast<int>(g_fileButtonsOpt->size())) {
        index = static_cast<int>(g_fileButtonsOpt->size()) - 1;
    }
    g_selectedFile = index;
    g_filesColumnActive = true;
    PaintColumnSelection(g_fileButtonsOpt, index);
    PaintColumnSelection(g_appButtonsOpt, -1);
    SchedulePreview();
}

// ---------------------------------------------------------------------------
// File preview: a card beside Start for the selected file
//
// A large thumbnail and a few details -- size, date, and for pictures and
// videos their dimensions and length -- for telling similar names apart. The
// thumbnail is the one File Explorer shows: whatever the shell has for the
// type (pictures, a frame of a video, documents, anything an installed app
// provides one for), else the type's icon. So no type needs code here -- but
// PDFs, which Windows gives no thumbnail of its own: their first page is
// drawn by Windows' own PDF renderer (RenderPdfPage), so the preview does not
// depend on a PDF app being installed.
//
// An SVG with no thumbnail is drawn by XAML's own renderer (OpenSvg).
// A file with no picture shows the start of its text instead, when it has
// some: a text file -- any, by what is in it -- line for line
// (ReadTextStart), or a Word, Excel, PowerPoint, RTF or OpenDocument file
// through Windows' or Office's own document filters (ReadDocumentText),
// which run here, in Start.
//
// Motion plays over the still once it is ready (animatePreview): a video,
// muted and looping, through XAML's own player; a GIF, which XAML animates
// itself; an animated WebP, whose frames Windows' WebP decoder hands over
// whole but without their timing, which is read from the file
// (WebpFrameDurations); a PDF's first pages, a second each. Never sound. All
// of it stops and is let go the
// moment the selection moves or Start closes.
//
// The card is a Popup beside the menu: right of it, or left when that would
// run off the screen. Start's window covers the whole screen, but its window
// region is the menu, and nothing of the window shows outside it -- so while
// the card is up, its rectangle is added to the region (SetPreviewRegion), and
// exactly that part is taken out again when it goes -- so a click on it goes
// to Start's window, which hands it to the card: a click on the path copies
// it. It never takes focus; the keyboard stays in the search box. It follows
// the selection
// after a short pause, so arrowing through results does not flash a card per
// row. Thumbnails are fetched on a thread of their own: one can take a few
// hundred milliseconds, more the first time a video is seen.
// ---------------------------------------------------------------------------

namespace preview {

inline constexpr double kWidth = 300;       // the card, in DIPs
inline constexpr double kThumbWidth = 276;  // the thumbnail's box inside it
inline constexpr double kThumbHeight = 400;  // a portrait photo at the full width

// A file with no thumbnail shows its icon as large as a folder's thumbnail:
// up to 256 pixels, the largest size icons are made at.
inline int TypeIconPixels(double scale) {
    return std::min(256, static_cast<int>(kThumbWidth * scale));
}

// What the thread found for a path.
struct Result {
    std::wstring path;
    std::vector<BYTE> pixels;  // premultiplied BGRA, width x height; empty if none
    int width = 0;
    int height = 0;
    double scale = 1.0;        // physical pixels per DIP when it was made
    int pages = 0;             // a PDF drawn by RenderPdfPage: how many pages
    bool morePages = false;    // and its next pages are to follow (RenderPdfFrames)
    std::wstring name;
    std::vector<std::pair<std::wstring, std::wstring>> facts;  // label and value, in the order shown
    std::wstring text;       // the start of a text file or a document, shown where a picture would be
    bool monospace = false;  // the text is a text file's, line for line
    // Motion over the still (see the module comment). At most one is set.
    winrt::Windows::Storage::Streams::IRandomAccessStream gif{nullptr};
    winrt::Windows::Storage::Streams::IRandomAccessStream svg{nullptr};  // drawn by XAML (OpenSvg)
    double svgAspect = 1.0;                                               // its height over its width
    winrt::Windows::Storage::StorageFile video{nullptr};
    std::vector<std::vector<BYTE>> frames;  // an animated WebP, each like pixels
    std::vector<int> delays;                // milliseconds per frame
};

std::mutex g_mutex;
std::condition_variable g_wake;
std::wstring g_request;  // the path to fetch next, or empty
bool g_quit = false;
[[clang::no_destroy]] std::optional<Result> g_result;  // the last fetched, for the XAML thread
[[clang::no_destroy]] std::optional<Result> g_frames;  // a PDF's pages after its card (ApplyPreviewFrames)
[[clang::no_destroy]] std::optional<std::thread> g_thread;

// Properties, defined here: propkey.h would add every key there is to the
// DLL (see kLinkTargetParsingPath). System.Image.HorizontalSize and
// VerticalSize, System.Video.FrameWidth and FrameHeight, System.Media.Duration,
// then those of the details (Fetch): System.ItemTypeText,
// System.Video.FrameRate, System.Audio.EncodingBitrate, System.Music.Artist
// and AlbumTitle, System.Photo.DateTaken and CameraModel,
// System.FileDescription, FileVersion and Company, System.Title and Author.
inline constexpr PROPERTYKEY kImageWidth{{0x6444048F, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kImageHeight{{0x6444048F, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kVideoWidth{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kVideoHeight{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kDuration{{0x64440490, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kItemTypeText{{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}}, 4};
inline constexpr PROPERTYKEY kFrameRate{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 6};
inline constexpr PROPERTYKEY kBitrate{{0x64440490, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kArtist{{0x56A3372E, 0xCE9C, 0x11D2, {0x9F, 0x0E, 0x00, 0x60, 0x97, 0xC6, 0x86, 0xF6}}, 2};
inline constexpr PROPERTYKEY kAlbum{{0x56A3372E, 0xCE9C, 0x11D2, {0x9F, 0x0E, 0x00, 0x60, 0x97, 0xC6, 0x86, 0xF6}}, 4};
inline constexpr PROPERTYKEY kDateTaken{{0x14B81DA1, 0x0135, 0x4D31, {0x96, 0xD9, 0x6C, 0xBF, 0xC9, 0x67, 0x1A, 0x99}}, 36867};
inline constexpr PROPERTYKEY kCameraModel{{0x14B81DA1, 0x0135, 0x4D31, {0x96, 0xD9, 0x6C, 0xBF, 0xC9, 0x67, 0x1A, 0x99}}, 272};
inline constexpr PROPERTYKEY kFileDescription{{0x0CEF7D53, 0xFA64, 0x11D1, {0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE}}, 3};
inline constexpr PROPERTYKEY kFileVersion{{0x0CEF7D53, 0xFA64, 0x11D1, {0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE}}, 4};
inline constexpr PROPERTYKEY kCompany{{0xD5CDD502, 0x2E9C, 0x101B, {0x93, 0x97, 0x08, 0x00, 0x2B, 0x2C, 0xF9, 0xAE}}, 15};
inline constexpr PROPERTYKEY kTitle{{0xF29F85E0, 0x4FF9, 0x1068, {0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9}}, 2};
inline constexpr PROPERTYKEY kAuthor{{0xF29F85E0, 0x4FF9, 0x1068, {0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9}}, 4};

// A property as text: a string, or a list of them joined (artists, authors).
inline std::wstring PropertyText(IShellItem2* item, REFPROPERTYKEY key) {
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(item->GetProperty(key, &value))) {
        if (value.vt == VT_LPWSTR && value.pwszVal) {
            text = value.pwszVal;
        } else if (value.vt == (VT_VECTOR | VT_LPWSTR)) {
            for (ULONG i = 0; i < value.calpwstr.cElems; ++i) {
                if (value.calpwstr.pElems[i] && *value.calpwstr.pElems[i]) {
                    text += (text.empty() ? L"" : L"; ");
                    text += value.calpwstr.pElems[i];
                }
            }
        }
        PropVariantClear(&value);
    }
    while (!text.empty() && iswspace(text.back())) {
        text.pop_back();
    }
    return text;
}

inline std::wstring ByteSize(ULONGLONG bytes) {
    wchar_t text[32] = {};
    StrFormatByteSizeW(static_cast<LONGLONG>(bytes), text, ARRAYSIZE(text));
    return text;
}

inline std::wstring ItemCount(DWORD count, bool more) {
    return std::to_wstring(count) + (more ? L"+" : L"") + (count == 1 && !more ? L" item" : L" items");
}

// A folder's size and how many items it holds directly, from Everything's
// index: one query each, answered in a few milliseconds. The size only when
// Everything keeps folder sizes (its "Index folder size" option); false when
// Everything does not know the folder.
inline bool FolderFacts(everything::Client* client, const std::wstring& path, ULONGLONG* size, bool* sized,
                        DWORD* items) {
    const size_t slash = path.find_last_of(L'\\');
    if (!client || slash == std::wstring::npos || slash + 1 >= path.size()) {
        return false;  // a drive: not a folder to Everything
    }
    std::wstring parent = path.substr(0, slash);
    if (parent.size() == 2) {
        parent += L'\\';  // "C:" is "C:\"
    }
    std::vector<everything::Result> found;
    DWORD total = 0;
    if (!client->Query(L"folder: parent:\"" + parent + L"\" wfn:\"" + path.substr(slash + 1) + L"\"", 1, &found, &total,
                       300, everything::kReqName | everything::kReqPath | everything::kReqSize) ||
        found.empty()) {
        return false;
    }
    *sized = found[0].size != ~0ULL;  // what Everything answers when it keeps no folder sizes
    *size = found[0].size;
    std::vector<everything::Result> children;
    return client->Query(L"parent:\"" + path + L"\"", 1, &children, items, 300, everything::kReqName);
}

// How many items a folder holds directly, counted here: when Everything
// does not know it. Up to 10000.
inline DWORD CountItems(const std::wstring& path, bool* more) {
    DWORD count = 0;
    *more = false;
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((path + L"\\*").c_str(), FindExInfoBasic, &data, FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return 0;
    }
    do {
        if (wcscmp(data.cFileName, L".") && wcscmp(data.cFileName, L"..")) {
            if (++count >= 10000) {
                *more = true;
                break;
            }
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return count;
}

// A thumbnail or a property can make the shell read the file, which on a
// network or removable drive can stall for seconds. Those get the name and
// folder only.
inline bool OnFixedDrive(const std::wstring& path) {
    if (path.size() < 3 || path[1] != L':') {
        return false;
    }
    const wchar_t root[] = {path[0], L':', L'\\', 0};
    return GetDriveTypeW(root) == DRIVE_FIXED;
}

inline std::wstring FormatDuration(ULONGLONG hundredNs) {
    const ULONGLONG total = hundredNs / 10000000ULL;
    wchar_t text[32];
    if (total >= 3600) {
        swprintf_s(text, L"%llu:%02llu:%02llu", total / 3600, (total / 60) % 60, total % 60);
    } else {
        swprintf_s(text, L"%llu:%02llu", total / 60, total % 60);
    }
    return text;
}

inline std::wstring FormatDate(const FILETIME& ft) {
    SYSTEMTIME utc{}, local{};
    if (!FileTimeToSystemTime(&ft, &utc) || !SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local)) {
        return L"";
    }
    wchar_t date[64] = {}, time[64] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &local, nullptr, date, 64, nullptr);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local, nullptr, time, 64);
    return std::wstring(date) + L" " + time;
}

// Runs f on a thread of its own in the multithreaded apartment and waits for
// it: f waits for WinRT operations, which must not be done in the preview
// thread's single-threaded one.
template <typename F>
inline void RunInMta(F&& f) {
    std::thread worker([&] {
        HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        try {
            f();
        } catch (...) {
        }
        if (SUCCEEDED(co)) {
            CoUninitialize();
        }
    });
    worker.join();
}

// How many of a PDF's pages the preview turns through, and for how long each.
inline constexpr int kPdfPages = 10;
inline constexpr int kPdfPageMs = 1000;

// A page of a PDF, fitted into width x height pixels, by Windows' own
// renderer (Windows.Data.Pdf). In the multithreaded apartment (RunInMta).
inline bool RenderPdfPageOf(winrt::Windows::Data::Pdf::PdfDocument const& document, uint32_t index, int width, int height,
                            std::vector<BYTE>* pixels, int* w, int* h) {
    namespace pdf = winrt::Windows::Data::Pdf;
    namespace imaging = winrt::Windows::Graphics::Imaging;
    auto page = document.GetPage(index);
    const auto size = page.Size();
    const double fit = std::min(width / std::max(1.0, static_cast<double>(size.Width)),
                                height / std::max(1.0, static_cast<double>(size.Height)));
    pdf::PdfPageRenderOptions options;
    options.DestinationWidth(static_cast<uint32_t>(std::max(1.0, size.Width * fit)));
    options.DestinationHeight(static_cast<uint32_t>(std::max(1.0, size.Height * fit)));
    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
    page.RenderToStreamAsync(stream, options).get();
    auto decoder = imaging::BitmapDecoder::CreateAsync(stream).get();
    auto data = decoder.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8, imaging::BitmapAlphaMode::Premultiplied,
                                          imaging::BitmapTransform(), imaging::ExifOrientationMode::IgnoreExifOrientation,
                                          imaging::ColorManagementMode::DoNotColorManage)
                    .get();
    auto bytes = data.DetachPixelData();
    *w = static_cast<int>(decoder.PixelWidth());
    *h = static_cast<int>(decoder.PixelHeight());
    if (*w <= 0 || *h <= 0 || bytes.size() != static_cast<size_t>(*w) * *h * 4) {
        return false;
    }
    pixels->assign(bytes.begin(), bytes.end());
    return true;
}

// A PDF's first page, fitted into width x height pixels, and how many pages it
// has. The card shows it at once; the next pages follow (RenderPdfFrames).
// Fails on an encrypted PDF, for one.
inline bool RenderPdfPage(const std::wstring& path, int width, int height, Result* r) {
    bool ok = false;
    RunInMta([&] {
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto document = winrt::Windows::Data::Pdf::PdfDocument::LoadFromFileAsync(file).get();
        if (document.PageCount() > 0 && RenderPdfPageOf(document, 0, width, height, &r->pixels, &r->width, &r->height)) {
            r->pages = static_cast<int>(document.PageCount());
            ok = true;
        }
    });
    return ok;
}

// A PDF's first pages, up to kPdfPages, as frames the size of its first page
// (still), a page of another shape centered: shown a second each once they
// are ready. Gives up when stop() says the selection has moved.
template <typename Stop>
inline void RenderPdfFrames(const std::wstring& path, const Result& still, Stop&& stop, std::vector<std::vector<BYTE>>* frames,
                            std::vector<int>* delays) {
    RunInMta([&] {
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto document = winrt::Windows::Data::Pdf::PdfDocument::LoadFromFileAsync(file).get();
        const int count = std::min(static_cast<int>(document.PageCount()), kPdfPages);
        frames->push_back(still.pixels);
        delays->push_back(kPdfPageMs);
        for (int i = 1; i < count && !stop(); ++i) {
            std::vector<BYTE> page;
            int w = 0, h = 0;
            if (!RenderPdfPageOf(document, static_cast<uint32_t>(i), still.width, still.height, &page, &w, &h)) {
                break;
            }
            std::vector<BYTE> frame(still.pixels.size(), 0);
            const int cw = std::min(w, still.width), ch = std::min(h, still.height);
            const int left = (still.width - cw) / 2, top = (still.height - ch) / 2;
            for (int y = 0; y < ch; ++y) {
                memcpy(&frame[(static_cast<size_t>(top + y) * still.width + left) * 4], &page[static_cast<size_t>(y) * w * 4],
                       static_cast<size_t>(cw) * 4);
            }
            frames->push_back(std::move(frame));
            delays->push_back(kPdfPageMs);
        }
    });
    if (frames->size() < 2 || stop()) {
        frames->clear();
        delays->clear();
    }
}

// How long each frame of an animated WebP shows, in milliseconds, from the
// file's ANMF chunks: Windows' decoder hands over the frames without it.
inline std::vector<int> WebpFrameDurations(const std::wstring& path) {
    std::vector<int> durations;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return durations;
    }
    BYTE header[12] = {};
    DWORD read = 0;
    if (ReadFile(file, header, sizeof(header), &read, nullptr) && read == sizeof(header) &&
        memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "WEBP", 4) == 0) {
        LARGE_INTEGER pos{};
        pos.QuadPart = 12;
        for (int chunks = 0; chunks < 100000; ++chunks) {
            BYTE chunk[8 + 16] = {};  // the chunk header, and an ANMF's fields
            if (!SetFilePointerEx(file, pos, nullptr, FILE_BEGIN) || !ReadFile(file, chunk, sizeof(chunk), &read, nullptr) ||
                read < 8) {
                break;
            }
            const uint32_t size = chunk[4] | (chunk[5] << 8) | (chunk[6] << 16) | (static_cast<uint32_t>(chunk[7]) << 24);
            if (memcmp(chunk, "ANMF", 4) == 0 && read >= 8 + 15) {
                durations.push_back(chunk[8 + 12] | (chunk[8 + 13] << 8) | (chunk[8 + 14] << 16));
            }
            pos.QuadPart += 8 + static_cast<LONGLONG>(size) + (size & 1);
        }
    }
    CloseHandle(file);
    return durations;
}

// An animated WebP's frames, fitted into width x height pixels -- the first
// becomes the still. Leaves r alone for a still WebP, or one too large to keep
// in memory.
inline void DecodeAnimatedWebp(const std::wstring& path, int width, int height, Result* r) {
    constexpr size_t kMaxFrames = 300;
    constexpr size_t kMaxBytes = 64 << 20;
    RunInMta([&] {
        namespace imaging = winrt::Windows::Graphics::Imaging;
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto decoder = imaging::BitmapDecoder::CreateAsync(file.OpenReadAsync().get()).get();
        const uint32_t count = decoder.FrameCount();
        const double fit = std::min({1.0, width / std::max(1.0, static_cast<double>(decoder.PixelWidth())),
                                     height / std::max(1.0, static_cast<double>(decoder.PixelHeight()))});
        const uint32_t w = std::max(1u, static_cast<uint32_t>(decoder.PixelWidth() * fit));
        const uint32_t h = std::max(1u, static_cast<uint32_t>(decoder.PixelHeight() * fit));
        if (count < 2 || count > kMaxFrames || static_cast<size_t>(count) * w * h * 4 > kMaxBytes) {
            return;
        }
        imaging::BitmapTransform transform;
        transform.ScaledWidth(w);
        transform.ScaledHeight(h);
        transform.InterpolationMode(imaging::BitmapInterpolationMode::Fant);
        std::vector<std::vector<BYTE>> frames;
        for (uint32_t i = 0; i < count; ++i) {
            auto frame = decoder.GetFrameAsync(i).get();
            auto data = frame.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8, imaging::BitmapAlphaMode::Premultiplied,
                                                transform, imaging::ExifOrientationMode::IgnoreExifOrientation,
                                                imaging::ColorManagementMode::DoNotColorManage)
                            .get();
            auto bytes = data.DetachPixelData();
            if (bytes.size() != static_cast<size_t>(w) * h * 4) {
                return;
            }
            frames.emplace_back(bytes.begin(), bytes.end());
        }
        std::vector<int> delays = WebpFrameDurations(path);
        delays.resize(frames.size(), 100);
        for (int& delay : delays) {
            delay = delay < 20 ? 100 : delay;  // as browsers treat 0 and near-0
        }
        r->pixels = frames.front();
        r->width = static_cast<int>(w);
        r->height = static_cast<int>(h);
        r->frames = std::move(frames);
        r->delays = std::move(delays);
    });
}

// Opens what the card plays over the still, if anything: a GIF for XAML to
// animate, a video for its player, an animated WebP's frames.
inline void FetchMotion(const std::wstring& path, int width, int height, Result* r) {
    const size_t dot = r->name.rfind(L'.');
    if (dot == std::wstring::npos) {
        return;
    }
    const std::wstring ext = tools::ToLower(r->name.substr(dot));
    PERCEIVED type = PERCEIVED_TYPE_UNSPECIFIED;
    PERCEIVEDFLAG flags = 0;
    if (ext == L".gif") {
        RunInMta([&] {
            auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
            r->gif = file.OpenReadAsync().get();
        });
    } else if (ext == L".webp") {
        DecodeAnimatedWebp(path, width, height, r);
    } else if (SUCCEEDED(AssocGetPerceivedType(ext.c_str(), &type, &flags, nullptr)) && type == PERCEIVED_TYPE_VIDEO) {
        RunInMta([&] { r->video = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get(); });
    }
}

// An SVG, for XAML's own renderer (SvgImageSource) to draw on the card, and
// its shape: from the viewBox of its <svg> element, else its width and
// height, else square.
inline bool OpenSvg(const std::wstring& path, Result* r) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    char bytes[4096];
    DWORD read = 0;
    ReadFile(file, bytes, sizeof(bytes) - 1, &read, nullptr);
    CloseHandle(file);
    bytes[read] = 0;
    const char* tag = strstr(bytes, "<svg");
    if (!tag) {
        return false;
    }
    const char* end = strchr(tag, '>');
    const std::string svgTag(tag, end ? end : bytes + read);
    auto attribute = [&](const char* name) -> std::string {
        const std::string key = std::string(" ") + name + "=";
        size_t at = svgTag.find(key);
        if (at == std::string::npos) {
            at = svgTag.find(std::string("\n") + name + "=");
        }
        if (at == std::string::npos || at + key.size() >= svgTag.size()) {
            return {};
        }
        const char quote = svgTag[at + key.size()];
        const size_t start = at + key.size() + 1;
        const size_t close = svgTag.find(quote, start);
        return close == std::string::npos ? std::string() : svgTag.substr(start, close - start);
    };
    double aspect = 0;
    const std::string viewBox = attribute("viewBox");
    if (!viewBox.empty()) {
        double box[4] = {};
        const char* cursor = viewBox.c_str();
        for (double& value : box) {
            char* next = nullptr;
            value = strtod(cursor, &next);
            cursor = next;
            while (*cursor == ',' || *cursor == ' ') {
                ++cursor;
            }
        }
        if (box[2] > 0 && box[3] > 0) {
            aspect = box[3] / box[2];
        }
    }
    if (!aspect) {
        const double width = strtod(attribute("width").c_str(), nullptr);
        const double height = strtod(attribute("height").c_str(), nullptr);
        aspect = width > 0 && height > 0 ? height / width : 1;
    }
    r->svgAspect = std::clamp(aspect, 0.2, 5.0);
    RunInMta([&] {
        r->svg = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get().OpenReadAsync().get();
    });
    return r->svg != nullptr;
}

// The start of a text file -- any: by what is in it, not its name -- as up to
// 40 lines: UTF-16 or UTF-8 by its byte order mark, else UTF-8 when it is,
// else the system's code page. Nothing when it is not text: a NUL, or too
// many other control characters, in the first 8 KB.
inline bool ReadTextStart(const std::wstring& path, std::wstring* text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    char bytes[8192];
    DWORD read = 0;
    const bool ok = ReadFile(file, bytes, sizeof(bytes), &read, nullptr) && read > 0;
    CloseHandle(file);
    if (!ok) {
        return false;
    }
    std::wstring decoded;
    auto decode = [&](UINT codePage, const char* from, int length, DWORD flags) {
        const int count = MultiByteToWideChar(codePage, flags, from, length, nullptr, 0);
        if (count <= 0) {
            return false;
        }
        decoded.resize(count);
        MultiByteToWideChar(codePage, flags, from, length, decoded.data(), count);
        return true;
    };
    if (read >= 2 && static_cast<BYTE>(bytes[0]) == 0xFF && static_cast<BYTE>(bytes[1]) == 0xFE) {
        decoded.assign(reinterpret_cast<const wchar_t*>(bytes + 2), (read - 2) / 2);
    } else {
        const int skip = read >= 3 && static_cast<BYTE>(bytes[0]) == 0xEF && static_cast<BYTE>(bytes[1]) == 0xBB &&
                                 static_cast<BYTE>(bytes[2]) == 0xBF
                             ? 3
                             : 0;
        if (memchr(bytes, 0, read)) {
            return false;
        }
        // A character cut off at the end of what was read is not an error.
        int length = static_cast<int>(read) - skip;
        bool utf8 = false;
        for (int cut = 0; cut < 4 && length - cut > 0 && !utf8; ++cut) {
            utf8 = decode(CP_UTF8, bytes + skip, length - cut, MB_ERR_INVALID_CHARS);
        }
        if (!utf8 && !decode(CP_ACP, bytes + skip, length, 0)) {
            return false;
        }
    }
    size_t control = 0;
    for (wchar_t ch : decoded) {
        if (ch == 0) {
            return false;
        }
        control += ch < 0x20 && ch != L'\t' && ch != L'\r' && ch != L'\n' && ch != L'\f';
    }
    if (decoded.empty() || control * 20 > decoded.size()) {
        return false;
    }
    // Line for line: tabs as four spaces, long lines cut.
    std::wstring out;
    int lines = 0;
    size_t column = 0;
    for (size_t i = 0; i < decoded.size() && lines < 40; ++i) {
        const wchar_t ch = decoded[i];
        if (ch == L'\r') {
            continue;
        }
        if (ch == L'\n') {
            out += L'\n';
            ++lines;
            column = 0;
        } else if (column < 200) {
            if (ch == L'\t') {
                out.append(4, L' ');
                column += 4;
            } else if (ch >= 0x20) {
                out += ch;
                ++column;
            }
        }
    }
    while (!out.empty() && (out.back() == L'\n' || out.back() == L' ')) {
        out.pop_back();
    }
    if (out.empty()) {
        return false;
    }
    *text = std::move(out);
    return true;
}

// The document filter (IFilter) registered for a file name's extension, as
// LoadIFilter finds it: its class, if its DLL is Windows' own (System32) or
// Microsoft Office's. Those are what the types in ReadDocumentText have; no
// other is loaded into Start.
inline bool TrustedDocumentFilter(const std::wstring& extension, CLSID* filter) {
    auto read = [](const std::wstring& key) {
        wchar_t value[MAX_PATH * 2];
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, value,
                         &size) != ERROR_SUCCESS) {
            return std::wstring();
        }
        wchar_t expanded[MAX_PATH * 2];
        const DWORD n = ExpandEnvironmentStringsW(value, expanded, ARRAYSIZE(expanded));
        return std::wstring(n && n <= ARRAYSIZE(expanded) ? expanded : value);
    };
    std::wstring handler = read(extension + L"\\PersistentHandler");
    if (handler.empty()) {
        const std::wstring progId = read(extension);
        const std::wstring clsid = progId.empty() ? std::wstring() : read(progId + L"\\CLSID");
        handler = clsid.empty() ? std::wstring() : read(L"CLSID\\" + clsid + L"\\PersistentHandler");
    }
    const std::wstring filterClsid =
        handler.empty()
            ? std::wstring()
            : read(L"CLSID\\" + handler + L"\\PersistentAddinsRegistered\\{89BCB740-6119-101A-BCB7-00DD010655AF}");
    const std::wstring dll = filterClsid.empty() ? std::wstring() : read(L"CLSID\\" + filterClsid + L"\\InprocServer32");
    if (dll.empty() || FAILED(CLSIDFromString(filterClsid.c_str(), filter))) {
        return false;
    }
    wchar_t system[MAX_PATH];
    const UINT n = GetSystemDirectoryW(system, MAX_PATH);
    if (n && n < MAX_PATH && uninstall::IsUnder(dll, system)) {
        return true;
    }
    static const std::pair<const KNOWNFOLDERID*, const wchar_t*> kOffice[] = {
        {&FOLDERID_ProgramFiles, L"\\Microsoft Office"},
        {&FOLDERID_ProgramFilesX86, L"\\Microsoft Office"},
        {&FOLDERID_ProgramFilesCommon, L"\\Microsoft Shared\\Filters"},
        {&FOLDERID_ProgramFilesCommonX86, L"\\Microsoft Shared\\Filters"},
    };
    for (const auto& [id, under] : kOffice) {
        PWSTR folder = nullptr;
        bool inside = false;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, KF_FLAG_DONT_VERIFY, nullptr, &folder)) && folder) {
            inside = uninstall::IsUnder(dll, std::wstring(folder) + under);
        }
        CoTaskMemFree(folder);
        if (inside) {
            return true;
        }
    }
    return false;
}

// The start of a document's text -- Word, Excel, PowerPoint, RTF,
// OpenDocument -- through the document filter Windows' own search reads it
// with (TrustedDocumentFilter): Windows has them for these, Office or not.
// They run here, in Start, so: only Windows' or Office's, up to 30 MB, the
// first 1500 characters.
inline bool ReadDocumentText(const std::wstring& path, const std::wstring& lowerName, std::wstring* text) {
    static const wchar_t* const kDocuments[] = {L".doc", L".docx", L".docm", L".dot", L".dotx", L".rtf",
                                                L".odt", L".xls", L".xlsx", L".xlsm", L".ppt", L".pptx",
                                                L".pptm", L".ods", L".odp"};
    const size_t dot = lowerName.rfind(L'.');
    if (dot == std::wstring::npos ||
        std::none_of(std::begin(kDocuments), std::end(kDocuments),
                     [&](const wchar_t* ext) { return lowerName.compare(dot, std::wstring::npos, ext) == 0; })) {
        return false;
    }
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes) || attributes.nFileSizeHigh ||
        attributes.nFileSizeLow > 30u * 1024 * 1024) {
        return false;
    }
    CLSID filterClass{};
    if (!TrustedDocumentFilter(lowerName.substr(dot), &filterClass)) {
        return false;
    }
    IFilter* filter = nullptr;
    IPersistFile* file = nullptr;
    if (FAILED(CoCreateInstance(filterClass, nullptr, CLSCTX_INPROC_SERVER, IID_IFilter, reinterpret_cast<void**>(&filter))) ||
        !filter) {
        return false;
    }
    bool loaded = SUCCEEDED(filter->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&file))) && file &&
                  SUCCEEDED(file->Load(path.c_str(), STGM_READ | STGM_SHARE_DENY_NONE));
    if (file) {
        file->Release();
    }
    ULONG flags = 0;
    std::wstring out;
    if (loaded &&
        SUCCEEDED(filter->Init(IFILTER_INIT_CANON_PARAGRAPHS | IFILTER_INIT_HARD_LINE_BREAKS, 0, nullptr, &flags))) {
        STAT_CHUNK chunk{};
        for (int chunks = 0; out.size() < 1500 && chunks < 2000 && filter->GetChunk(&chunk) == S_OK; ++chunks) {
            if (!(chunk.flags & CHUNK_TEXT)) {
                continue;
            }
            if (!out.empty()) {
                out += chunk.breakType >= CHUNK_EOP ? L'\n' : L' ';
            }
            wchar_t buffer[512];
            ULONG size = ARRAYSIZE(buffer);
            HRESULT got;
            while (out.size() < 1500 && ((got = filter->GetText(&size, buffer)) == S_OK || got == FILTER_S_LAST_TEXT)) {
                for (ULONG i = 0; i < size; ++i) {
                    const wchar_t ch = buffer[i] == L'\r' || buffer[i] == 0x0B ? L'\n' : buffer[i];
                    // No empty lines or runs of spaces: the text, close.
                    if ((ch == L'\n' && (out.empty() || out.back() == L'\n')) ||
                        (ch == L' ' && (out.empty() || out.back() == L' ' || out.back() == L'\n')) ||
                        (ch < 0x20 && ch != L'\n')) {
                        continue;
                    }
                    out += ch;
                }
                if (got == FILTER_S_LAST_TEXT) {
                    break;
                }
                size = ARRAYSIZE(buffer);
            }
        }
    }
    filter->Release();
    while (!out.empty() && iswspace(out.back())) {
        out.pop_back();
    }
    if (out.empty()) {
        return false;
    }
    *text = std::move(out);
    return true;
}

// The icon of the file's type, from its name alone -- for a file the shell
// could not open (no access, or a drive not read from here). SHIL_JUMBO's,
// unless the type has no 256px image -- then that list has its 48 in a corner
// of the 256 -- and then the 48 itself, at its own size.
inline void TypeIcon(const std::wstring& path, int side, Result* r) {
    SHFILEINFOW info{};
    const DWORD attributes = GetFileAttributesW(path.c_str());
    const bool folder = attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    if (!SHGetFileInfoW(path.c_str(), folder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX)) {
        return;
    }
    for (int which : {SHIL_JUMBO, SHIL_EXTRALARGE}) {
        IImageList* list = nullptr;
        if (FAILED(SHGetImageList(which, IID_PPV_ARGS(&list))) || !list) {
            continue;
        }
        int cx = 0, cy = 0;
        list->GetIconSize(&cx, &cy);
        const int size = which == SHIL_JUMBO ? side : std::min(side, cx);
        HICON icon = nullptr;
        std::vector<BYTE> pixels;
        bool ok = SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon)) && icon &&
                  icons::IconToBgra(icon, size, &pixels);
        if (icon) {
            DestroyIcon(icon);
        }
        list->Release();
        if (ok && which == SHIL_JUMBO) {
            // Nothing drawn past the corner a 48 takes in a 256: that is all there is.
            const int corner = size * 48 / 256 + 1;
            bool beyond = false;
            for (int y = 0; y < size && !beyond; ++y) {
                for (int x = y < corner ? corner : 0; x < size; ++x) {
                    if (pixels[(static_cast<size_t>(y) * size + x) * 4 + 3]) {
                        beyond = true;
                        break;
                    }
                }
            }
            ok = beyond;
        }
        if (ok) {
            r->pixels = std::move(pixels);
            r->width = r->height = size;
            return;
        }
    }
}

// On the preview thread.
inline Result Fetch(const std::wstring& path, double scale, bool animate, everything::Client* everythingClient) {
    Result r;
    r.path = path;
    r.scale = scale;
    const size_t slash = path.find_last_of(L"\\/");
    r.name = slash == std::wstring::npos ? path : path.substr(slash + 1);

    // The details: what File Explorer's details pane shows of it, and what
    // its kind has of its own.
    std::wstring type, size, items, dimensions, length, frameRate, bitrate, artist, album, title, author, description,
        version, company, camera, taken, modified, created;
    if (OnFixedDrive(path)) {
        IShellItem2* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) && item) {
            type = PropertyText(item, kItemTypeText);
            ULONG w = 0, h = 0;
            if ((SUCCEEDED(item->GetUInt32(kImageWidth, &w)) && SUCCEEDED(item->GetUInt32(kImageHeight, &h)) && w && h) ||
                (SUCCEEDED(item->GetUInt32(kVideoWidth, &w)) && SUCCEEDED(item->GetUInt32(kVideoHeight, &h)) && w && h)) {
                dimensions = std::to_wstring(w) + L" \u00D7 " + std::to_wstring(h);
            }
            ULONGLONG duration = 0;
            if (SUCCEEDED(item->GetUInt64(kDuration, &duration)) && duration) {
                length = FormatDuration(duration);
            }
            ULONG number = 0;
            if (SUCCEEDED(item->GetUInt32(kFrameRate, &number)) && number) {  // frames per 1000 seconds
                wchar_t text[32];
                swprintf_s(text, number % 1000 ? L"%.2f fps" : L"%.0f fps", number / 1000.0);
                frameRate = text;
            }
            if (SUCCEEDED(item->GetUInt32(kBitrate, &number)) && number) {  // bits per second
                bitrate = std::to_wstring((number + 500) / 1000) + L" kbps";
            }
            artist = PropertyText(item, kArtist);
            album = PropertyText(item, kAlbum);
            title = PropertyText(item, kTitle);
            author = PropertyText(item, kAuthor);
            description = PropertyText(item, kFileDescription);
            version = PropertyText(item, kFileVersion);
            company = PropertyText(item, kCompany);
            camera = PropertyText(item, kCameraModel);
            FILETIME shot{};
            if (SUCCEEDED(item->GetFileTime(kDateTaken, &shot)) && (shot.dwLowDateTime || shot.dwHighDateTime)) {
                taken = FormatDate(shot);
            }

            // A thumbnail at full size; without one, a PDF's first page, else
            // the type's icon (TypeIconPixels).
            IShellItemImageFactory* factory = nullptr;
            if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
                SIZE want{static_cast<LONG>(kThumbWidth * scale), static_cast<LONG>(kThumbHeight * scale)};
                HBITMAP bitmap = nullptr;
                const std::wstring lower = tools::ToLower(r.name);
                if (SUCCEEDED(factory->GetImage(want, SIIGBF_THUMBNAILONLY, &bitmap)) && bitmap) {
                    // The shell fits a thumbnail to the longer side of the size
                    // asked for, so a wide one comes back wider than the box.
                    if (icons::BitmapToPixels(bitmap, &r.pixels, &r.width, &r.height) &&
                        (r.width > want.cx || r.height > want.cy)) {
                        const double fit = std::min(static_cast<double>(want.cx) / r.width,
                                                    static_cast<double>(want.cy) / r.height);
                        const int w = std::max(1, static_cast<int>(r.width * fit));
                        const int h = std::max(1, static_cast<int>(r.height * fit));
                        r.pixels = icons::Resample(r.pixels, r.width, r.height, w, h);
                        r.width = w;
                        r.height = h;
                    }
                    DeleteObject(bitmap);
                } else if (lower.size() > 4 && lower.ends_with(L".pdf") &&
                           RenderPdfPage(path, want.cx, want.cy, &r)) {
                    r.morePages = animate && r.pages > 1;  // RenderPdfFrames, after the card shows
                } else if (lower.size() > 4 && lower.ends_with(L".svg") && OpenSvg(path, &r)) {
                    // drawn on the card
                } else if (ReadDocumentText(path, lower, &r.text)) {
                    // its text, where a picture would be
                } else if (ReadTextStart(path, &r.text)) {
                    r.monospace = true;
                } else {
                    const int side = TypeIconPixels(scale);
                    if (SUCCEEDED(factory->GetImage(SIZE{side, side}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap)) &&
                        bitmap) {
                        if (icons::BitmapToPixels(bitmap, &r.pixels, &r.width, &r.height) && r.width == r.height &&
                            r.width != side) {
                            r.pixels = icons::Resample(r.pixels, r.width, side);
                            r.width = r.height = side;
                        }
                        DeleteObject(bitmap);
                    }
                }
                factory->Release();
            }
            item->Release();
        }
        if (animate) {
            FetchMotion(path, static_cast<int>(kThumbWidth * scale), static_cast<int>(kThumbHeight * scale), &r);
        }

        WIN32_FILE_ATTRIBUTE_DATA attributes{};
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes)) {
            if (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ULONGLONG bytes = 0;
                bool sized = false, more = false;
                DWORD count = 0;
                if (!FolderFacts(everythingClient, path, &bytes, &sized, &count)) {
                    count = CountItems(path, &more);
                }
                if (sized) {
                    size = ByteSize(bytes);
                }
                items = ItemCount(count, more);
            } else {
                size = ByteSize((static_cast<ULONGLONG>(attributes.nFileSizeHigh) << 32) | attributes.nFileSizeLow);
            }
            modified = FormatDate(attributes.ftLastWriteTime);
            created = FormatDate(attributes.ftCreationTime);
        }
    }
    if (r.pixels.empty() && r.text.empty() && !r.svg) {
        TypeIcon(path, TypeIconPixels(scale), &r);
    }
    auto add = [&r](const wchar_t* label, const std::wstring& value) {
        if (!value.empty()) {
            r.facts.emplace_back(label, value);
        }
    };
    add(L"Type", type);
    add(L"Size", size);
    add(L"Contains", items);
    add(L"Dimensions", dimensions);
    add(L"Length", length);
    add(L"Pages", r.pages ? std::to_wstring(r.pages) : std::wstring());
    add(L"Frame rate", frameRate);
    add(L"Bitrate", bitrate);
    add(L"Artist", artist);
    add(L"Album", album);
    add(L"Title", title);
    add(L"Author", author);
    add(L"Description", description);
    add(L"Version", version);
    add(L"Company", company);
    add(L"Camera", camera);
    add(L"Taken", taken);
    add(L"Modified", modified);
    add(L"Created", created);
    return r;
}

}  // namespace preview

// The card, built on first use. XAML thread only.
[[clang::no_destroy]] wuxcp::Popup g_previewPopup{nullptr};
[[clang::no_destroy]] wuxc::Border g_previewCard{nullptr};
[[clang::no_destroy]] wuxc::Border g_previewThumbBox{nullptr};
[[clang::no_destroy]] wuxc::Image g_previewImage{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewName{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewText{nullptr};
[[clang::no_destroy]] wuxc::Grid g_previewFacts{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewPath{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_previewCopied{nullptr};  // puts the path back after "Copied"
std::wstring g_previewShownPath;
[[clang::no_destroy]] wux::DispatcherTimer g_previewTimer{nullptr};
[[clang::no_destroy]] wuxma::Storyboard g_previewResize{nullptr};  // PreviewResize
[[clang::no_destroy]] wuxma::DoubleAnimation g_previewResizeAnimation{nullptr};
std::wstring g_previewWanted;  // the selected file's path, or empty

// What plays over the still. XAML thread only.
[[clang::no_destroy]] wuxc::MediaPlayerElement g_previewVideo{nullptr};
[[clang::no_destroy]] winrt::Windows::Media::Playback::MediaPlayer g_previewPlayer{nullptr};
[[clang::no_destroy]] wuxmi::BitmapImage g_previewGif{nullptr};
[[clang::no_destroy]] std::optional<wuxmi::BitmapImage::ImageOpened_revoker> g_previewGifOpened;
[[clang::no_destroy]] wux::DispatcherTimer g_previewFrameTimer{nullptr};
[[clang::no_destroy]] wuxmi::WriteableBitmap g_previewFrameBitmap{nullptr};
std::vector<std::vector<BYTE>> g_previewFrames;
std::vector<int> g_previewDelays;
size_t g_previewFrame = 0;
std::atomic<unsigned> g_previewPlayGeneration{0};

void ApplyPreview();
void ApplyPreviewFrames();

void PreviewThreadMain() {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::optional<everything::Client> everythingClient;
    everythingClient.emplace(L"WindhawkEverythingPreviewReply");
    if (!everythingClient->Init()) {
        everythingClient.reset();
    }
    for (;;) {
        std::wstring path;
        {
            std::unique_lock<std::mutex> lock(preview::g_mutex);
            preview::g_wake.wait(lock, [] { return preview::g_quit || !preview::g_request.empty(); });
            if (preview::g_quit) {
                break;
            }
            path = std::exchange(preview::g_request, std::wstring());
        }
        HWND start = GetOurCoreWindow();
        UINT dpi = start ? GetDpiForWindow(start) : 0;
        bool animate = true;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            animate = g_settings.animatePreview;
        }
        preview::Result result =
            preview::Fetch(path, (dpi ? dpi : 96) / 96.0, animate, everythingClient ? &*everythingClient : nullptr);
        // The card shows the first page; a PDF's next pages follow. What they
        // need is taken here, before the result is the XAML thread's.
        std::optional<preview::Result> still;
        {
            std::lock_guard<std::mutex> lock(preview::g_mutex);
            if (preview::g_quit || !preview::g_request.empty()) {
                continue;  // stopping, or already outdated
            }
            if (result.morePages) {
                still.emplace();
                still->path = result.path;
                still->pixels = result.pixels;
                still->width = result.width;
                still->height = result.height;
            }
            preview::g_result = std::move(result);
        }
        try {
            if (g_ourBox) {
                g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                               wuc::DispatchedHandler{[] { ApplyPreview(); }});
            }
        } catch (...) {
        }
        if (still) {
            auto moved = [] {
                std::lock_guard<std::mutex> lock(preview::g_mutex);
                return preview::g_quit || !preview::g_request.empty();
            };
            preview::RenderPdfFrames(still->path, *still, moved, &still->frames, &still->delays);
            if (!still->frames.empty()) {
                {
                    std::lock_guard<std::mutex> lock(preview::g_mutex);
                    if (preview::g_quit || !preview::g_request.empty()) {
                        continue;
                    }
                    preview::g_frames = std::move(still);
                }
                try {
                    if (g_ourBox) {
                        g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                                       wuc::DispatchedHandler{[] { ApplyPreviewFrames(); }});
                    }
                } catch (...) {
                }
            }
        }
    }
    everythingClient.reset();  // its window, on this thread
    if (SUCCEEDED(comHr)) {
        CoUninitialize();
    }
}

void StopPreviewThread() {
    std::optional<std::thread> thread;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        preview::g_quit = true;  // for good: nothing starts the thread again
        thread.swap(preview::g_thread);
    }
    preview::g_wake.notify_all();
    if (thread && thread->joinable()) {
        thread->join();
    }
    std::lock_guard<std::mutex> lock(preview::g_mutex);
    preview::g_result.reset();
    preview::g_frames.reset();
}

bool PreviewEnabled() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings.filePreview;
}

// What SetPreviewRegion added to Start's window region, or null.
static HRGN g_previewAddedRegion = nullptr;

// The top of what Start shows, in XAML's coordinates: its window covers the
// screen but draws only inside its window region, the menu (less what
// SetPreviewRegion added). 0 when it has no region.
double VisibleTop() {
    HWND start = GetOurCoreWindow();
    if (!start) {
        return 0;
    }
    double top = 0;
    HRGN region = CreateRectRgn(0, 0, 0, 0);
    const int kind = GetWindowRgn(start, region);
    if (kind != ERROR && kind != NULLREGION) {
        if (g_previewAddedRegion) {
            CombineRgn(region, region, g_previewAddedRegion, RGN_DIFF);
        }
        RECT box{};
        if (GetRgnBox(region, &box) != NULLREGION) {
            top = box.top * 96.0 / GetDpiForWindow(start);
        }
    }
    DeleteObject(region);
    return top;
}

// Adds a rectangle of Start's window (physical pixels) to its window region,
// in place of what was added before; null only takes that out. Only the part
// the region lacked is added, so taking it out never cuts into the menu.
void SetPreviewRegion(const RECT* rect) {
    HWND start = GetOurCoreWindow();
    if (!start) {
        return;
    }
    HRGN region = CreateRectRgn(0, 0, 0, 0);
    const int kind = GetWindowRgn(start, region);
    if (kind == ERROR || kind == NULLREGION) {
        DeleteObject(region);  // no region: nothing is clipped
        return;
    }
    if (g_previewAddedRegion) {
        CombineRgn(region, region, g_previewAddedRegion, RGN_DIFF);
        DeleteObject(g_previewAddedRegion);
        g_previewAddedRegion = nullptr;
    }
    if (rect) {
        HRGN card = CreateRectRgnIndirect(rect);
        HRGN added = CreateRectRgn(0, 0, 0, 0);
        if (CombineRgn(added, card, region, RGN_DIFF) != NULLREGION) {
            CombineRgn(region, region, added, RGN_OR);
            g_previewAddedRegion = added;
        } else {
            DeleteObject(added);
        }
        DeleteObject(card);
    }
    SetWindowRgn(start, region, TRUE);  // the window owns it from here
}

// Stops whatever plays over the still and lets it go.
void StopPreviewMotion() {
    g_previewPlayGeneration.fetch_add(1);
    try {
        g_previewGifOpened.reset();
        g_previewGif = nullptr;
        if (g_previewFrameTimer) {
            g_previewFrameTimer.Stop();
        }
        g_previewFrames.clear();
        g_previewDelays.clear();
        g_previewFrameBitmap = nullptr;
        if (g_previewVideo) {
            g_previewVideo.Visibility(wux::Visibility::Collapsed);
            g_previewVideo.SetMediaPlayer(nullptr);
        }
        if (g_previewPlayer) {
            g_previewPlayer.Pause();
            g_previewPlayer.Source(nullptr);
            g_previewPlayer.Close();
            g_previewPlayer = nullptr;
        }
    } catch (...) {
    }
}

void HidePreview() {
    StopPreviewMotion();
    g_previewWanted.clear();
    try {
        if (g_previewTimer) {
            g_previewTimer.Stop();
        }
        if (g_previewPopup && g_previewPopup.IsOpen()) {
            g_previewPopup.IsOpen(false);
        }
    } catch (...) {
    }
    if (g_previewAddedRegion) {
        SetPreviewRegion(nullptr);
    }
}

void BuildPreviewCard() {
    if (g_previewPopup) {
        return;
    }
    wuxc::Border card;
    card.Width(preview::kWidth);
    card.Padding(wux::ThicknessHelper::FromUniformLength(12));
    card.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
    card.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));

    wuxc::StackPanel stack;
    wuxc::Border thumbBox;
    thumbBox.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
    wuxc::Grid thumbGrid;
    wuxc::Image image;
    image.Stretch(wuxm::Stretch::Uniform);
    image.HorizontalAlignment(wux::HorizontalAlignment::Center);
    image.VerticalAlignment(wux::VerticalAlignment::Center);
    thumbGrid.Children().Append(image);
    wuxc::MediaPlayerElement video;
    video.AreTransportControlsEnabled(false);
    video.Stretch(wuxm::Stretch::Uniform);
    video.HorizontalAlignment(wux::HorizontalAlignment::Center);
    video.VerticalAlignment(wux::VerticalAlignment::Center);
    video.Visibility(wux::Visibility::Collapsed);
    thumbGrid.Children().Append(video);
    wuxc::TextBlock thumbText;  // a text file's or a document's start (Result::text)
    thumbText.Margin(wux::ThicknessHelper::FromLengths(10, 8, 10, 8));
    ScaleFont(thumbText, 11);
    thumbText.Opacity(0.85);
    thumbText.MaxLines(24);
    thumbText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    thumbText.Visibility(wux::Visibility::Collapsed);
    thumbGrid.Children().Append(thumbText);
    thumbBox.Child(thumbGrid);
    stack.Children().Append(thumbBox);

    wuxc::TextBlock name;
    name.Margin(wux::ThicknessHelper::FromLengths(2, 10, 2, 0));
    ScaleFont(name, 14);
    name.FontWeight(wut::FontWeights::SemiBold());
    name.TextWrapping(wux::TextWrapping::Wrap);
    name.MaxLines(2);
    name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    stack.Children().Append(name);

    // The details, label and value (ApplyPreview fills them in).
    wuxc::Grid facts;
    facts.Margin(wux::ThicknessHelper::FromLengths(2, 8, 2, 0));
    facts.ColumnSpacing(12);
    facts.RowSpacing(3);
    wuxc::ColumnDefinition labelColumn;
    labelColumn.Width(wux::GridLengthHelper::Auto());
    facts.ColumnDefinitions().Append(labelColumn);
    wuxc::ColumnDefinition valueColumn;
    valueColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    facts.ColumnDefinitions().Append(valueColumn);
    stack.Children().Append(facts);

    // The full path, which a click copies -- without taking the keyboard
    // from the search box.
    wuxc::Button pathButton;
    pathButton.Margin(wux::ThicknessHelper::FromLengths(-2, 6, -2, 0));
    pathButton.Padding(wux::ThicknessHelper::FromLengths(4, 4, 4, 4));
    pathButton.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    pathButton.HorizontalContentAlignment(wux::HorizontalAlignment::Left);
    pathButton.Background(MakeBrush(0, 0, 0, 0));
    pathButton.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
    pathButton.IsTabStop(false);
    pathButton.AllowFocusOnInteraction(false);
    wuxc::ToolTipService::SetToolTip(pathButton, winrt::box_value(L"Copy path"));
    wuxc::TextBlock pathText;
    ScaleFont(pathText, 12);
    pathText.Opacity(0.7);
    pathText.TextWrapping(wux::TextWrapping::Wrap);
    pathText.MaxLines(3);
    pathText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    pathButton.Content(pathText);
    KeepHandler(pathButton, pathButton.Click(winrt::auto_revoke, [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        if (g_previewShownPath.empty() || !g_previewPath) {
            return;
        }
        tools::CopyTextToClipboard(g_previewShownPath);
        g_previewPath.Text(L"Copied to clipboard");
        if (!g_previewCopied) {
            g_previewCopied = wux::DispatcherTimer();
            g_previewCopied.Interval(std::chrono::milliseconds(1200));
            KeepHandler(g_previewCopied, g_previewCopied.Tick(winrt::auto_revoke, [](wf::IInspectable const&,
                                                                                    wf::IInspectable const&) {
                g_previewCopied.Stop();
                if (g_previewPath) {
                    g_previewPath.Text(winrt::hstring{g_previewShownPath});
                }
            }));
        }
        g_previewCopied.Stop();
        g_previewCopied.Start();
    }));
    stack.Children().Append(pathButton);
    card.Child(stack);

    wuxcp::Popup popup;
    popup.Child(card);
    popup.IsLightDismissEnabled(false);
    try {
        if (g_resultsHost && g_resultsHost.XamlRoot()) {
            popup.XamlRoot(g_resultsHost.XamlRoot());
        }
    } catch (...) {
    }

    g_previewPopup = popup;
    g_previewCard = card;
    g_previewThumbBox = thumbBox;
    g_previewImage = image;
    g_previewVideo = video;
    g_previewText = thumbText;
    g_previewName = name;
    g_previewFacts = facts;
    g_previewPath = pathText;
}

// Beside the panel: right of it, or left when the right would run off the
// window (Start's window, the whole screen). The window region takes in at
// least atLeast DIPs of height: what the card spans while it changes size.
void PositionPreview(double atLeast = 0) {
    if (!g_previewPopup || !g_resultsHost) {
        return;
    }
    const auto origin = g_resultsHost.TransformToVisual(nullptr).TransformPoint(wf::Point{0, 0});
    const double panelWidth = g_resultsHost.ActualWidth();
    double windowWidth = 0, windowHeight = 0;
    try {
        if (auto root = g_resultsHost.XamlRoot()) {
            windowWidth = root.Size().Width;
            windowHeight = root.Size().Height;
        }
    } catch (...) {
    }
    if (windowWidth <= 0) {
        try {
            windowWidth = wux::Window::Current().Bounds().Width;
            windowHeight = wux::Window::Current().Bounds().Height;
        } catch (...) {
        }
    }
    constexpr double kGap = 12;
    double x = origin.X + panelWidth + kGap;
    if (windowWidth > 0 && x + preview::kWidth > windowWidth && origin.X - kGap - preview::kWidth >= 0) {
        x = origin.X - kGap - preview::kWidth;
    }
    // Level with the top of the panel, unless a tall card would run off the
    // bottom of the screen.
    g_previewCard.Measure(wf::Size{static_cast<float>(preview::kWidth), std::numeric_limits<float>::infinity()});
    const double height = std::max(static_cast<double>(g_previewCard.DesiredSize().Height), atLeast);
    double y = origin.Y;
    if (windowHeight > 0 && y + height > windowHeight) {
        y = std::max(0.0, windowHeight - height);
    }
    g_previewPopup.HorizontalOffset(x);
    g_previewPopup.VerticalOffset(y);

    // Its rectangle in physical pixels, for the window region.
    HWND start = GetOurCoreWindow();
    const double scale = (start ? GetDpiForWindow(start) : 96) / 96.0;
    RECT rect{static_cast<LONG>(std::floor(x * scale)) - 1, static_cast<LONG>(std::floor(y * scale)) - 1,
              static_cast<LONG>(std::ceil((x + preview::kWidth) * scale)) + 1,
              static_cast<LONG>(std::ceil((y + height) * scale)) + 1};
    SetPreviewRegion(&rect);
}

// Plays a result's motion over the still, already shown at its size.
void StartPreviewMotion(preview::Result& result) {
    const unsigned generation = g_previewPlayGeneration.load();
    if (result.gif) {
        // Swapped in once loaded, so the still shows until then.
        wuxmi::BitmapImage gif;
        gif.DecodePixelWidth(result.width);
        g_previewGifOpened = gif.ImageOpened(winrt::auto_revoke, [generation](auto&&, auto&&) {
            if (generation == g_previewPlayGeneration.load() && g_previewGif && g_previewImage) {
                g_previewImage.Source(g_previewGif);
            }
        });
        g_previewGif = gif;
        gif.SetSourceAsync(result.gif);
    } else if (!result.frames.empty()) {
        g_previewFrames = std::move(result.frames);
        g_previewDelays = std::move(result.delays);
        g_previewFrame = 0;
        g_previewFrameBitmap = wuxmi::WriteableBitmap{result.width, result.height};
        g_previewImage.Source(g_previewFrameBitmap);
        if (!g_previewFrameTimer) {
            g_previewFrameTimer = wux::DispatcherTimer();
            KeepHandler(g_previewFrameTimer,
                        g_previewFrameTimer.Tick(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
                            if (g_previewFrames.empty() || !g_previewFrameBitmap) {
                                g_previewFrameTimer.Stop();
                                return;
                            }
                            g_previewFrame = (g_previewFrame + 1) % g_previewFrames.size();
                            const auto& frame = g_previewFrames[g_previewFrame];
                            auto access = g_previewFrameBitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
                            BYTE* dest = nullptr;
                            if (SUCCEEDED(access->Buffer(&dest)) && dest &&
                                g_previewFrameBitmap.PixelBuffer().Capacity() >= frame.size()) {
                                memcpy(dest, frame.data(), frame.size());
                                g_previewFrameBitmap.Invalidate();
                            }
                            g_previewFrameTimer.Interval(std::chrono::milliseconds(g_previewDelays[g_previewFrame]));
                        }));
        }
        auto access = g_previewFrameBitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* dest = nullptr;
        if (SUCCEEDED(access->Buffer(&dest)) && dest) {
            memcpy(dest, g_previewFrames[0].data(), g_previewFrames[0].size());
            g_previewFrameBitmap.Invalidate();
        }
        g_previewFrameTimer.Interval(std::chrono::milliseconds(g_previewDelays[0]));
        g_previewFrameTimer.Start();
    } else if (result.video) {
        // Muted and looping, over the still, which it shows as its poster
        // until the first frame is ready.
        namespace playback = winrt::Windows::Media::Playback;
        playback::MediaPlayer player;
        player.IsMuted(true);
        player.IsLoopingEnabled(true);
        player.AutoPlay(true);
        g_previewVideo.PosterSource(g_previewImage.Source());
        g_previewVideo.Visibility(wux::Visibility::Visible);
        g_previewVideo.Width(g_previewImage.Width());
        g_previewVideo.Height(g_previewImage.Height());
        g_previewVideo.SetMediaPlayer(player);
        g_previewPlayer = player;
        player.Source(winrt::Windows::Media::Core::MediaSource::CreateFromStorageFile(result.video));
    }
}

// Drags a result out of Start, as File Explorer drags a file, by handing the
// drag to this mod's helper window in Explorer (RunExplorerDrag).
//
// Nowhere in this process works. XAML's own drag (CanDrag, StartDragAsync)
// fails inside XAML in Start's window, which then ends the process. A shell
// drag loop on Start's thread gets only the first move of the mouse -- Start's
// input does not come through ordinary window messages -- and then waits for
// good, the whole menu with it. One on another thread gets no mouse at all,
// and Windows will not hand the foreground to it while the button is down.
// It does hand it to Explorer's helper, a plain window in a plain process,
// where the drag works as anywhere else. Start's results stay up meanwhile
// (g_dragging); Explorer tells Start how it ended (DragDoneMessage).
void StartFileDrag(std::wstring const& path) {
    // Only what Explorer's helper takes (a local file or folder that exists),
    // or Start would give it the foreground for nothing.
    bool local = path.size() >= 3 && path[1] == L':' && !PathIsUNCW(path.c_str());
    if (local) {
        const wchar_t root[] = {path[0], L':', L'\\', 0};
        local = GetDriveTypeW(root) != DRIVE_REMOTE && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
    }
    HWND helper = local ? FindExplorerLaunchHolder() : nullptr;
    if (!helper) {
        Wh_Log(L"drag: not for %ls", path.c_str());
        return;
    }
    ReleaseCapture();  // the row's button holds the mouse
    g_dragging = true;
    if (!SetForegroundWindow(helper)) {
        g_dragging = false;
        Wh_Log(L"drag: Explorer's helper did not get the foreground");
        return;
    }
    COPYDATASTRUCT cds{};
    cds.dwData = kExplorerDragMagic;
    cds.cbData = static_cast<DWORD>((path.size() + 1) * sizeof(wchar_t));
    cds.lpData = const_cast<wchar_t*>(path.c_str());
    DWORD_PTR accepted = 0;
    if (!SendMessageTimeoutW(helper, WM_COPYDATA, reinterpret_cast<WPARAM>(GetOurCoreWindow()),
                             reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG, 500, &accepted) ||
        !accepted) {
        g_dragging = false;
        Wh_Log(L"drag: Explorer's helper refused %ls", path.c_str());
    }
}

// Removes, when destroyed, a handler added with AddHandler -- which C++/WinRT
// gives no revoker for -- so KeepHandler can hold it like the others.
struct RoutedHandlerRevoker {
    winrt::weak_ref<wux::UIElement> element;
    wux::RoutedEvent event{nullptr};
    wf::IInspectable handler{nullptr};

    RoutedHandlerRevoker(wux::UIElement const& e, wux::RoutedEvent const& ev, wf::IInspectable const& h)
        : element(winrt::make_weak(e)), event(ev), handler(h) {}
    RoutedHandlerRevoker(RoutedHandlerRevoker&&) = default;
    RoutedHandlerRevoker& operator=(RoutedHandlerRevoker&&) = default;
    ~RoutedHandlerRevoker() {
        if (!handler) {
            return;  // moved from
        }
        try {
            if (auto e = element.get()) {
                e.RemoveHandler(event, handler);
            }
        } catch (...) {
        }
    }
};

// A pointer handler that also hears events a control has already handled --
// a Button handles its own presses.
void AddPointerHandler(wux::UIElement const& element, wux::RoutedEvent const& event,
                       wux::Input::PointerEventHandler const& handler) {
    auto boxed = winrt::box_value(handler);
    element.AddHandler(event, boxed, true);
    KeepHandler(element, RoutedHandlerRevoker{element, event, boxed});
}

// Shows what the preview thread found, if it is still what is selected.
// Eases the card's height from what it was to what its new content needs,
// over a sixth of a second. The content is laid out at once at its own size:
// while the card is shorter, it is cut off at the bottom; while taller, the
// card shows its background below it. The window region covers the larger of
// the two meanwhile.
void PreviewResize(double fromHeight, double toHeight) {
    if (!g_previewResize) {
        g_previewResizeAnimation = wuxma::DoubleAnimation();
        g_previewResizeAnimation.EnableDependentAnimation(true);  // Height is laid out, so on this thread
        g_previewResizeAnimation.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(160)));
        wuxma::CubicEase ease;
        ease.EasingMode(wuxma::EasingMode::EaseOut);
        g_previewResizeAnimation.EasingFunction(ease);
        wuxma::Storyboard::SetTargetProperty(g_previewResizeAnimation, L"Height");
        g_previewResize = wuxma::Storyboard();
        g_previewResize.Children().Append(g_previewResizeAnimation);
        KeepHandler(g_previewResize, g_previewResize.Completed(winrt::auto_revoke, [](wf::IInspectable const&,
                                                                                      wf::IInspectable const&) {
            if (g_previewCard) {
                g_previewCard.ClearValue(wux::FrameworkElement::HeightProperty());
            }
            if (g_previewResize) {
                g_previewResize.Stop();  // let go of the animated value: back to its own height
            }
            PositionPreview();
        }));
    }
    wuxma::Storyboard::SetTarget(g_previewResizeAnimation, g_previewCard);
    g_previewResizeAnimation.From(fromHeight);
    g_previewResizeAnimation.To(toHeight);
    g_previewCard.Height(fromHeight);
    PositionPreview(std::max(fromHeight, toHeight));
    g_previewResize.Begin();
}

void ApplyPreview() try {
    std::optional<preview::Result> result;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        result = std::move(preview::g_result);
        preview::g_result.reset();
    }
    if (!result || result->path != g_previewWanted || !g_isOverlayVisible.load() || !g_resultsHost) {
        Wh_Log(L"preview: dropped (%ls)", result ? L"no longer selected" : L"nothing fetched");
        return;
    }
    BuildPreviewCard();

    const bool isLight = IsLightTheme();
    g_previewCard.RequestedTheme(isLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
    auto background = g_resultsHost.Background();
    g_previewCard.Background(background ? background
                                        : (isLight ? MakeBrush(0xF2, 0xF3, 0xF3, 0xF3) : MakeBrush(0xF2, 0x20, 0x20, 0x20)));
    g_previewCard.BorderBrush(isLight ? MakeBrush(0x24, 0x00, 0x00, 0x00) : MakeBrush(0x30, 0xFF, 0xFF, 0xFF));
    g_previewThumbBox.Background(isLight ? MakeBrush(0x0C, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));

    // Already up: it eases from the old content's height to the new one's
    // (PreviewResize) rather than jump.
    const double fromHeight = g_previewPopup.IsOpen() ? g_previewCard.ActualHeight() : 0;
    if (g_previewResize) {
        g_previewResize.Stop();
    }
    g_previewCard.ClearValue(wux::FrameworkElement::HeightProperty());

    StopPreviewMotion();
    wuxmi::WriteableBitmap bitmap{nullptr};
    if (result->width > 0 && result->height > 0 &&
        result->pixels.size() == static_cast<size_t>(result->width) * result->height * 4) {
        bitmap = wuxmi::WriteableBitmap{result->width, result->height};
        auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* dest = nullptr;
        if (SUCCEEDED(access->Buffer(&dest)) && dest) {
            memcpy(dest, result->pixels.data(), result->pixels.size());
            bitmap.Invalidate();
        } else {
            bitmap = nullptr;
        }
    }
    g_previewImage.Source(bitmap);
    g_previewImage.Margin(wux::ThicknessHelper::FromUniformLength(0));
    if (bitmap) {
        // Pixel for pixel: made at the display's scale. The box is as high as
        // the picture, so a wide one leaves no bands above and below.
        g_previewImage.Width(result->width / result->scale);
        g_previewImage.Height(result->height / result->scale);
        g_previewThumbBox.Height(std::max(result->height / result->scale, 48.0));
    }
    // An SVG, drawn by XAML at the display's scale, in a box of its shape, on
    // a light background: most are dark shapes on nothing.
    const bool showSvg = !bitmap && result->svg;
    if (showSvg) {
        double width = preview::kThumbWidth, height = width * result->svgAspect;
        if (height > preview::kThumbHeight) {
            height = preview::kThumbHeight;
            width = height / result->svgAspect;
        }
        height = std::max(height, 48.0);
        constexpr double kInset = 8;
        wuxmi::SvgImageSource svg;
        svg.RasterizePixelWidth((width - 2 * kInset) * result->scale);
        svg.RasterizePixelHeight((height - 2 * kInset) * result->scale);
        svg.SetSourceAsync(result->svg);
        g_previewImage.Source(svg);
        g_previewImage.Width(width - 2 * kInset);
        g_previewImage.Height(height - 2 * kInset);
        g_previewImage.Margin(wux::ThicknessHelper::FromUniformLength(kInset));
        g_previewThumbBox.Height(height);
        g_previewThumbBox.Background(MakeBrush(0xF2, 0xFF, 0xFF, 0xFF));
    }
    const bool picture = bitmap || showSvg;
    g_previewImage.Visibility(picture ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    // Text in place of a picture: a text file line for line, in a monospaced
    // font; a document's paragraphs, wrapped. The box is as high as the text.
    const bool showText = !picture && !result->text.empty();
    g_previewText.Visibility(showText ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (showText) {
        g_previewText.Text(winrt::hstring{result->text});
        g_previewText.FontFamily(wuxm::FontFamily(result->monospace ? L"Cascadia Mono, Consolas" : L"Segoe UI"));
        g_previewText.TextWrapping(result->monospace ? wux::TextWrapping::NoWrap : wux::TextWrapping::Wrap);
        g_previewThumbBox.ClearValue(wux::FrameworkElement::HeightProperty());
    }
    g_previewThumbBox.Visibility(picture || showText ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (bitmap) {
        StartPreviewMotion(*result);
    }
    g_previewName.Text(winrt::hstring{result->name});
    g_previewFacts.Children().Clear();
    g_previewFacts.RowDefinitions().Clear();
    for (size_t i = 0; i < result->facts.size(); ++i) {
        wuxc::RowDefinition row;
        row.Height(wux::GridLengthHelper::Auto());
        g_previewFacts.RowDefinitions().Append(row);
        wuxc::TextBlock label;
        label.Text(winrt::hstring{result->facts[i].first});
        ScaleFont(label, 12);
        label.Opacity(0.55);
        wuxc::Grid::SetRow(label, static_cast<int32_t>(i));
        g_previewFacts.Children().Append(label);
        wuxc::TextBlock value;
        value.Text(winrt::hstring{result->facts[i].second});
        ScaleFont(value, 12);
        value.TextWrapping(wux::TextWrapping::Wrap);
        value.MaxLines(2);
        value.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        wuxc::Grid::SetRow(value, static_cast<int32_t>(i));
        wuxc::Grid::SetColumn(value, 1);
        g_previewFacts.Children().Append(value);
    }
    g_previewShownPath = result->path;
    if (g_previewCopied) {
        g_previewCopied.Stop();
    }
    g_previewPath.Text(winrt::hstring{result->path});

    g_previewCard.Measure(wf::Size{static_cast<float>(preview::kWidth), std::numeric_limits<float>::infinity()});
    const double toHeight = g_previewCard.DesiredSize().Height;
    if (fromHeight > 0 && std::abs(toHeight - fromHeight) > 1) {
        PreviewResize(fromHeight, toHeight);
    } else {
        PositionPreview();
    }
    if (!g_previewPopup.IsOpen()) {
        g_previewCard.Opacity(0.0);
        g_previewPopup.IsOpen(true);
        wuxma::Storyboard fade;
        wuxma::DoubleAnimation opacity;
        opacity.From(0.0);
        opacity.To(1.0);
        opacity.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(120)));
        wuxma::Storyboard::SetTarget(opacity, g_previewCard);
        wuxma::Storyboard::SetTargetProperty(opacity, L"Opacity");
        fade.Children().Append(opacity);
        fade.Begin();
    }
    Wh_Log(L"preview: showing %ls (%dx%d) at %.0f,%.0f", result->name.c_str(), result->width, result->height,
           g_previewPopup.HorizontalOffset(), g_previewPopup.VerticalOffset());
} catch (...) {
    Wh_Log(L"preview: failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// A PDF's next pages, once ready, for the card still showing its first.
void ApplyPreviewFrames() try {
    std::optional<preview::Result> frames;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        frames = std::move(preview::g_frames);
        preview::g_frames.reset();
    }
    if (!frames || !g_previewPopup || !g_previewPopup.IsOpen() || frames->path != g_previewShownPath ||
        !g_previewFrames.empty()) {
        return;
    }
    StartPreviewMotion(*frames);
} catch (...) {
}

// Follows the selection: the selected file's preview, after a short pause.
void SchedulePreview() try {
    if (g_dragging) {
        return;
    }
    std::wstring path;
    if (g_filesColumnActive && g_selectedFile >= 0 && g_selectedFile < static_cast<int>(g_currentFileRows.size())) {
        path = g_currentFileRows[g_selectedFile].openPath;
    }
    if (path.empty() || !PreviewEnabled()) {
        HidePreview();
        return;
    }
    if (path == g_previewWanted) {
        return;
    }
    g_previewWanted = path;
    if (!g_previewTimer) {
        g_previewTimer = wux::DispatcherTimer();
        KeepHandler(g_previewTimer, g_previewTimer.Tick(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
            g_previewTimer.Stop();
            if (g_previewWanted.empty() || !g_isOverlayVisible.load()) {
                return;
            }
            Wh_Log(L"preview: fetching %ls", g_previewWanted.c_str());
            {
                std::lock_guard<std::mutex> lock(preview::g_mutex);
                preview::g_request = g_previewWanted;
                if (!preview::g_thread && !preview::g_quit) {
                    preview::g_thread.emplace(PreviewThreadMain);
                }
            }
            preview::g_wake.notify_all();
        }));
    }
    // Quicker once a card is up: moving from one file to the next.
    const bool open = g_previewPopup && g_previewPopup.IsOpen();
    g_previewTimer.Interval(std::chrono::milliseconds(open ? 80 : 250));
    g_previewTimer.Stop();
    g_previewTimer.Start();
} catch (...) {
}

// File types "Run as administrator" is offered for.
bool CanElevatePath(const std::wstring& path) {
    std::wstring lower = path;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    return lower.ends_with(L".exe") || lower.ends_with(L".bat") ||
           lower.ends_with(L".cmd") || lower.ends_with(L".ps1") ||
           lower.ends_with(L".msc") || lower.ends_with(L".lnk");
}

void OpenSelectedFile(int index, bool asAdmin) {
    if (index < 0 || index >= static_cast<int>(g_currentFileRows.size())) {
        return;
    }
    const std::wstring& path = g_currentFileRows[index].openPath;
    if (path.empty()) {
        return;
    }
    if (asAdmin && !CanElevatePath(path)) {
        asAdmin = false;
    }
    DismissStartMenuForLaunch([path = path, asAdmin] { OpenResult(path, asAdmin); });
    Wh_Log(L"open file: index %d ('%ls'), asAdmin=%d", index, path.c_str(), asAdmin ? 1 : 0);
}

// Opens the selected result's context menu, as a right-click on it would.
// Shown in standard mode it takes the keyboard, so the arrows and Enter work
// in it (see ContextMenuHasFocus) and Escape hands focus back to the search
// box. Queued rather than shown here: the key usually arrives inside the
// message hook, which is no place to open a popup.
void OpenSelectedContextMenu() {
    wuxc::Button target{nullptr};
    if (g_filesColumnActive && g_fileButtonsOpt && g_selectedFile >= 0 &&
        g_selectedFile < static_cast<int>(g_fileButtonsOpt->size())) {
        target = (*g_fileButtonsOpt)[g_selectedFile];
    } else if (!g_filesColumnActive && g_appButtonsOpt && g_selectedApp >= 0 &&
               g_selectedApp < static_cast<int>(g_appButtonsOpt->size())) {
        target = (*g_appButtonsOpt)[g_selectedApp];
    }
    if (!target || !target.ContextFlyout()) {
        return;
    }
    try {
        target.Dispatcher().RunAsync(
            wuc::CoreDispatcherPriority::Normal,
            [weak = winrt::make_weak(target)] {
                auto button = weak.get();
                if (!button) return;
                auto flyout = button.ContextFlyout();
                if (!flyout) return;
                try {
                    wuxc::Primitives::FlyoutShowOptions options;
                    options.ShowMode(wuxc::Primitives::FlyoutShowMode::Standard);
                    options.Placement(wuxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
                    flyout.ShowAt(button, options);
                } catch (...) {}
            });
    } catch (...) {}
}

// Whether the caret sits at the end of the query, with nothing selected, so
// Right has no text left to move over.
bool CaretAtEndOfQuery() {
    if (!g_ourBox) {
        return true;
    }
    try {
        return g_ourBox.SelectionLength() == 0 &&
               g_ourBox.SelectionStart() >= static_cast<int32_t>(g_ourBox.Text().size());
    } catch (...) {
        return true;
    }
}

void LaunchSelectedApp(int index, bool asAdmin) {
    if (index < 0 || index >= static_cast<int>(g_currentAppRows.size())) {
        return;
    }
    const auto& row = g_currentAppRows[index];
    if (!row.copyText.empty()) {
        DismissStartMenu();
        tools::CopyTextToClipboard(row.copyText);
        Wh_Log(L"copied to clipboard: '%ls'", row.copyText.c_str());
        return;
    }
    if (row.openPath.starts_with(L"http:") || row.openPath.starts_with(L"https:")) {
        DismissStartMenuForLaunch([url = row.openPath] { OpenResult(url, false); });
        Wh_Log(L"launch web: '%ls'", row.openPath.c_str());
        return;
    }
    int which = row.appIndex;
    if (which >= 0) {
        if (asAdmin && !row.canRunAsAdmin) {
            asAdmin = false;
        }
        DismissStartMenuForLaunch([which, asAdmin] {
            {
                std::lock_guard<std::mutex> lock(g_queryMutex);
                g_launchAsAdmin.store(asAdmin);
                g_launchRequest.store(which);
            }
            g_queryWake.notify_all();
        });
        Wh_Log(L"launch app: index %d ('%ls'), asAdmin=%d", index, row.title.c_str(), asAdmin ? 1 : 0);
    }
}

// The text the rows on screen are the results for (RenderResults).
std::wstring g_shownQuery;

// Whether the rows on screen are not yet the results for what is typed.
bool ResultsPending() {
    std::lock_guard<std::mutex> lock(g_queryMutex);
    return g_pendingQuery != g_shownQuery;
}

// What Enter does: opens the selected result.
void ActivateSelection(bool asAdmin) {
    const bool haveFiles = g_fileButtonsOpt && !g_fileButtonsOpt->empty();
    if (g_filesColumnActive && haveFiles) {
        OpenSelectedFile(g_selectedFile < 0 ? 0 : g_selectedFile, asAdmin);
    } else if (!g_currentAppRows.empty()) {
        LaunchSelectedApp(g_selectedApp < 0 ? 0 : g_selectedApp, asAdmin);
    }
}

bool IsNavigationKey(winrt::Windows::System::VirtualKey key) {
    using VK = winrt::Windows::System::VirtualKey;
    return key == VK::Down || key == VK::Up || key == VK::Enter || key == VK::Tab ||
           key == VK::Left || key == VK::Right;
}

// A result's context menu is open and has the keyboard: the arrows, Tab and
// Enter are its own (Right opens its submenus).
bool ContextMenuHasFocus() {
    try {
        auto focused = wux::Input::FocusManager::GetFocusedElement();
        return focused && (focused.try_as<wuxc::MenuFlyoutItemBase>() ||
                           focused.try_as<wuxc::MenuFlyoutPresenter>());
    } catch (...) {
        return false;
    }
}

// Escape reaches the CoreWindow's KeyDown only after XAML has used it to
// close an open menu, by which time the menu no longer has focus. So a
// menu still closing, or closed a moment ago, counts as open, and the
// query is kept.
static bool g_contextMenuOpen = false;
static uint64_t g_contextMenuClosedTick = 0;

void NoteContextMenuOpened() {
    g_contextMenuOpen = true;
}

void NoteContextMenuClosed() {
    g_contextMenuOpen = false;
    g_contextMenuClosedTick = GetTickCount64();
}

bool EscapeBelongsToContextMenu() {
    return g_contextMenuOpen || ContextMenuHasFocus() ||
           GetTickCount64() - g_contextMenuClosedTick < 250;
}

// Keyboard selection. Up and Down move within the active column and Enter
// opens what is selected there; Shift+Enter opens its context menu. Tab and Shift+Tab step through every result
// as one list, the apps and then the files, so Tab past the last app lands on
// the first file. Left and Right switch between the columns, but still edit
// the query while they have text to move over: Right switches only with the
// caret at the end, and Left only from the Files column. Returns whether the
// key was used; a key that was not goes on to the search box.
bool HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl) {
    using VK = winrt::Windows::System::VirtualKey;

    if (ContextMenuHasFocus()) {
        return false;
    }

    // One keypress can arrive by several routes (the message hook, the box,
    // the CoreWindow). Act on it once, and give the other routes the same
    // answer, so that none of them lets a used Left through to the caret.
    static VK s_lastKey = VK::None;
    static bool s_lastUsed = false;
    uint64_t now = GetTickCount64();
    if (key == s_lastKey && now - g_lastNavTick < 60) {
        return s_lastUsed;
    }

    const bool haveApps = g_appButtonsOpt && !g_appButtonsOpt->empty();
    const bool haveFiles = g_fileButtonsOpt && !g_fileButtonsOpt->empty();
    const bool inFiles = g_filesColumnActive && haveFiles;
    auto toApps = [&] { SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp); };
    auto toFiles = [&] { SetFileSelection(g_selectedFile < 0 ? 0 : g_selectedFile); };

    bool used = true;
    switch (key) {
        case VK::Down:
            if (inFiles) {
                SetFileSelection(g_selectedFile < 0 ? 0 : g_selectedFile + 1);
            } else if (haveApps) {
                SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp + 1);
            }
            break;
        case VK::Up:
            if (inFiles) {
                SetFileSelection(g_selectedFile - 1);
            } else if (haveApps) {
                SetAppSelection(g_selectedApp - 1);
            }
            break;
        case VK::Enter:
            if (GetKeyState(VK_SHIFT) < 0) {
                OpenSelectedContextMenu();
            } else if (ResultsPending()) {
                // Typed faster than the search: what is on screen is for
                // an earlier text, or nothing yet. RenderResults opens the
                // first result once the ones for this text are in.
                g_enterWaiting = true;
                g_enterWaitingCtrl = ctrl;
                g_enterWaitingTick = now;
                Wh_Log(L"nav: Enter before the results; waiting for them");
            } else {
                ActivateSelection(ctrl);
            }
            break;
        case VK::Tab: {
            // Used even at either end of the list, so focus never tabs out of
            // the search box.
            const int appCount = haveApps ? static_cast<int>(g_appButtonsOpt->size()) : 0;
            if (GetKeyState(VK_SHIFT) >= 0) {
                if (inFiles) {
                    SetFileSelection(g_selectedFile + 1);
                } else if (haveApps && g_selectedApp < appCount - 1) {
                    SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp + 1);
                } else if (haveFiles) {
                    SetFileSelection(0);
                }
            } else {
                if (inFiles && g_selectedFile > 0) {
                    SetFileSelection(g_selectedFile - 1);
                } else if (inFiles && haveApps) {
                    SetAppSelection(appCount - 1);
                } else if (!inFiles && haveApps) {
                    SetAppSelection(g_selectedApp - 1);
                }
            }
            break;
        }
        case VK::Right:
            used = !inFiles && haveFiles && CaretAtEndOfQuery();
            if (used) {
                toFiles();
            }
            break;
        case VK::Left:
            used = inFiles && haveApps;
            if (used) {
                toApps();
            }
            break;
        default:
            used = false;
            break;
    }

    s_lastKey = key;
    s_lastUsed = used;
    g_lastNavTick = now;
    return used;
}

// Results palette:
// Search box at top (Row 0),
// Dual-column apps and files in middle (Row 1),
// and keyboard navigation footer at bottom (Row 2).
void BuildResultsList(wuxc::Panel const& ownerPanel) try {
    if (g_resultsHost) {
        return;
    }

    if (!g_activeAppsOpt) g_activeAppsOpt.emplace();
    if (!g_appButtonsOpt) g_appButtonsOpt.emplace();
    if (!g_fileButtonsOpt) g_fileButtonsOpt.emplace();

    wuxc::Grid root;
    root.Name(L"WindhawkEverythingResults");
    // Edge to edge by default: with a margin, a band of Start shows around the
    // results like a frame inside the frame. SyncOverlayBackground gives it
    // Start's own surface -- background, corners and, edge to edge, outline --
    // so it reads as Start. The margin is a setting (panelMargin).
    root.Margin(wux::ThicknessHelper::FromUniformLength(PanelMargin()));
    root.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    root.VerticalAlignment(wux::VerticalAlignment::Stretch);
    root.Visibility(wux::Visibility::Visible);
    root.Opacity(0.0);
    root.IsHitTestVisible(false);
    root.Padding(wux::ThicknessHelper::FromLengths(16, 12, 16, 8));
    root.CornerRadius(wux::CornerRadius{8, 8, 8, 8});

    wuxm::TranslateTransform tt;
    tt.Y(-8.0);
    root.RenderTransform(tt);
    g_resultsTranslate = tt;

    // Spanning every row and column of MainContent
    wuxc::Grid::SetRow(root, 0);
    wuxc::Grid::SetRowSpan(root, 12);
    wuxc::Grid::SetColumn(root, 0);
    wuxc::Grid::SetColumnSpan(root, 12);
    wuxc::Canvas::SetZIndex(root, 999);

    // Root layout:
    // Row 0 (Auto): Search bar inside the overlay
    // Row 1 (1*): Dual-column Results (Apps 40* / hairline divider / Files 60*)
    // Row 2 (Auto): Status Footer
    wuxc::RowDefinition searchRowDef, resultsRowDef, footerRowDef;
    searchRowDef.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    resultsRowDef.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    footerRowDef.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    root.RowDefinitions().Append(searchRowDef);
    root.RowDefinitions().Append(resultsRowDef);
    root.RowDefinitions().Append(footerRowDef);

    bool isLight = IsLightTheme();

    // Search bar container at Row 0
    wuxc::Border searchBarBorder;
    searchBarBorder.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 8));
    searchBarBorder.Padding(wux::ThicknessHelper::FromLengths(12, 0, 10, 0));
    searchBarBorder.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
    searchBarBorder.Background(isLight ? MakeBrush(0xD0, 0xFF, 0xFF, 0xFF) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    searchBarBorder.BorderBrush(isLight ? MakeBrush(0x30, 0x00, 0x00, 0x00) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
    searchBarBorder.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
    searchBarBorder.Height(Fs(40));  // the text size's (Wh_ModSettingsChanged)
    wuxc::Grid::SetRow(searchBarBorder, 0);
    g_searchBarBorder = searchBarBorder;

    wuxc::Grid searchBarGrid;
    wuxc::ColumnDefinition sbIconCol, sbBoxCol;
    sbIconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    sbBoxCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    searchBarGrid.ColumnDefinitions().Append(sbIconCol);
    searchBarGrid.ColumnDefinitions().Append(sbBoxCol);

    wuxc::FontIcon searchIcon;
    searchIcon.Glyph(L"\uE721");
    ScaleFont(searchIcon, 14);
    searchIcon.Opacity(0.65);
    searchIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
    searchIcon.VerticalAlignment(wux::VerticalAlignment::Center);
    wuxc::Grid::SetColumn(searchIcon, 0);
    searchBarGrid.Children().Append(searchIcon);

    wuxc::TextBox box;
    box.Name(L"WindhawkStartSearchBox");
    box.PlaceholderText(L"Search apps, settings, and files...");
    box.VerticalAlignment(wux::VerticalAlignment::Center);
    box.VerticalContentAlignment(wux::VerticalAlignment::Center);
    ScaleFont(box, 14);
    // As tall as its text and centered in the bar, at any text size: no
    // minimum height, the same space above the text as below.
    box.MinHeight(0);
    box.Padding(wux::ThicknessHelper::FromLengths(10, 5, 6, 5));
    box.Background(MakeBrush(0, 0, 0, 0));
    box.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
    box.IsTabStop(true);
    box.TabIndex(0);
    box.IsSpellCheckEnabled(false);
    box.IsTextPredictionEnabled(false);

    static const wchar_t* kClearKeys[] = {
        L"TextControlBackground",
        L"TextControlBackgroundPointerOver",
        L"TextControlBackgroundFocused",
        L"TextControlBackgroundDisabled",
        L"TextControlBorderBrush",
        L"TextControlBorderBrushPointerOver",
        L"TextControlBorderBrushFocused",
        L"TextControlBorderBrushDisabled",
        L"TextControlButtonBackground",
        L"TextControlButtonBackgroundPointerOver",
        L"TextControlButtonBackgroundPressed",
    };
    for (const wchar_t* key : kClearKeys) {
        box.Resources().Insert(winrt::box_value(winrt::hstring{key}),
                               wuxm::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    }
    // Start's TextBox style still draws a background of its own when focused,
    // a lighter pill inside the bar: its template's BorderElement, hidden so
    // the bar is one surface. The bar has the border.
    KeepHandler(box, box.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
        std::function<bool(wux::DependencyObject const&)> hide = [&](wux::DependencyObject const& node) {
            const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int32_t i = 0; i < count; ++i) {
                auto child = wuxm::VisualTreeHelper::GetChild(node, i);
                if (auto element = child.try_as<wux::FrameworkElement>(); element && element.Name() == L"BorderElement") {
                    element.Opacity(0);
                    return true;
                }
                if (hide(child)) {
                    return true;
                }
            }
            return false;
        };
        try {
            hide(sender.as<wux::DependencyObject>());
        } catch (...) {
        }
    }));

    g_ourBoxChanged = box.TextChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const& sender, wuxc::TextChangedEventArgs const&) {
            try {
                auto b = sender.as<wuxc::TextBox>();
                std::wstring text{b.Text()};
                Wh_Log(L"own box: '%ls'", text.c_str());
                if (text.empty()) {
                    HideOverlayAnimated();
                } else {
                    RevealOverlayAnimated();
                }
                QueueQuery(std::move(text));
            } catch (...) {}
        });

    KeepHandler(box, box.PreviewKeyDown(winrt::auto_revoke, [](wf::IInspectable const&, wux::Input::KeyRoutedEventArgs const& args) {
        try {
            auto key = args.Key();
            if (key == winrt::Windows::System::VirtualKey::Escape && !EscapeBelongsToContextMenu()) {
                if (g_isOverlayVisible.load() || g_isHiding.load()) {
                    if (g_ourBox) g_ourBox.Text(L"");
                    HideOverlayAnimated();
                    args.Handled(true);
                    return;
                }
            }
            if (IsNavigationKey(key) && g_isOverlayVisible.load()) {
                bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
                if (HandleNavigationKey(key, ctrl)) {
                    args.Handled(true);
                }
            }
        } catch (...) {}
    }));

    g_ourBoxLost = box.LostFocus(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            try {
                if (g_ourBox && g_ourBox.Text().empty() && (g_isOverlayVisible.load() || g_isHiding.load())) {
                    HideOverlayAnimated();
                }
            } catch (...) {}
        });

    wuxc::Grid::SetColumn(box, 1);
    searchBarGrid.Children().Append(box);
    g_ourBox = box;

    searchBarBorder.Child(searchBarGrid);
    root.Children().Append(searchBarBorder);

    // Results container grid: Apps column (40*), 1px hairline divider, Files column (60*)
    wuxc::Grid resultsGrid;
    wuxc::Grid::SetRow(resultsGrid, 1);

    wuxc::ColumnDefinition appsCol, divCol, filesCol;
    appsCol.Width(wux::GridLengthHelper::FromValueAndType(40, wux::GridUnitType::Star));
    divCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    filesCol.Width(wux::GridLengthHelper::FromValueAndType(60, wux::GridUnitType::Star));
    resultsGrid.ColumnDefinitions().Append(appsCol);
    resultsGrid.ColumnDefinitions().Append(divCol);
    resultsGrid.ColumnDefinitions().Append(filesCol);

    wuxc::StackPanel apps;

    wuxc::Grid appsColGrid;
    wuxc::RowDefinition aHeadRow, aListRow;
    aHeadRow.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    aListRow.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    appsColGrid.RowDefinitions().Append(aHeadRow);
    appsColGrid.RowDefinitions().Append(aListRow);
    wuxc::Grid::SetColumn(appsColGrid, 0);

    wuxc::Border appsHeaderHolder;
    wuxc::Grid::SetRow(appsHeaderHolder, 0);
    appsColGrid.Children().Append(appsHeaderHolder);
    g_appsHeaderHolder = appsHeaderHolder;

    wuxc::ScrollViewer appsScroll;
    appsScroll.Content(apps);
    appsScroll.IsTabStop(false);
    appsScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    appsScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    wuxc::Grid::SetRow(appsScroll, 1);
    appsColGrid.Children().Append(appsScroll);

    resultsGrid.Children().Append(appsColGrid);

    wuxc::Border divider;
    divider.Width(1);
    divider.HorizontalAlignment(wux::HorizontalAlignment::Center);
    divider.VerticalAlignment(wux::VerticalAlignment::Stretch);
    divider.Background(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x14, 0xFF, 0xFF, 0xFF));
    divider.Margin(wux::ThicknessHelper::FromLengths(6, 4, 6, 4));
    wuxc::Grid::SetColumn(divider, 1);
    resultsGrid.Children().Append(divider);
    g_divider = divider;

    wuxc::StackPanel files;
    wuxc::Grid filesColGrid;
    wuxc::RowDefinition fHeadRow, fListRow;
    fHeadRow.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    fListRow.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    filesColGrid.RowDefinitions().Append(fHeadRow);
    filesColGrid.RowDefinitions().Append(fListRow);
    wuxc::Grid::SetColumn(filesColGrid, 2);

    wuxc::Border filesHeaderHolder;
    wuxc::Grid::SetRow(filesHeaderHolder, 0);
    filesColGrid.Children().Append(filesHeaderHolder);
    g_filesHeaderHolder = filesHeaderHolder;

    wuxc::ScrollViewer filesScroll;
    filesScroll.Content(files);
    filesScroll.IsTabStop(false);
    filesScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    filesScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    wuxc::Grid::SetRow(filesScroll, 1);
    filesColGrid.Children().Append(filesScroll);

    resultsGrid.Children().Append(filesColGrid);

    root.Children().Append(resultsGrid);

    // Modern Raycast-style bottom status and shortcut bar
    wuxc::Border footerBorder;
    footerBorder.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
    footerBorder.Padding(wux::ThicknessHelper::FromLengths(8, 6, 8, 4));
    footerBorder.BorderBrush(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x12, 0xFF, 0xFF, 0xFF));
    footerBorder.BorderThickness(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
    wuxc::Grid::SetRow(footerBorder, 2);
    g_footerBorder = footerBorder;

    wuxc::Grid footerGrid;
    wuxc::ColumnDefinition fColLeft, fColRight;
    fColLeft.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    fColRight.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    footerGrid.ColumnDefinitions().Append(fColLeft);
    footerGrid.ColumnDefinitions().Append(fColRight);

    wuxc::StackPanel leftStatus;
    leftStatus.Orientation(wuxc::Orientation::Horizontal);
    leftStatus.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::FontIcon boltIcon;
    boltIcon.Glyph(L"\uE946");
    ScaleFont(boltIcon, 11);
    boltIcon.Opacity(0.5);
    boltIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
    boltIcon.VerticalAlignment(wux::VerticalAlignment::Center);
    leftStatus.Children().Append(boltIcon);

    wuxc::TextBlock statusText;
    statusText.Text(L"Everything Search");
    ScaleFont(statusText, 11);
    statusText.Opacity(0.5);
    statusText.VerticalAlignment(wux::VerticalAlignment::Center);
    leftStatus.Children().Append(statusText);
    g_footerStatus = statusText;

    wuxc::Grid::SetColumn(leftStatus, 0);
    footerGrid.Children().Append(leftStatus);

    wuxc::StackPanel rightHints;
    rightHints.Orientation(wuxc::Orientation::Horizontal);
    rightHints.VerticalAlignment(wux::VerticalAlignment::Center);

    auto makeKeyCap = [isLight](const wchar_t* key, const wchar_t* action) {
        wuxc::StackPanel pair;
        pair.Orientation(wuxc::Orientation::Horizontal);
        pair.VerticalAlignment(wux::VerticalAlignment::Center);
        pair.Margin(wux::ThicknessHelper::FromLengths(8, 0, 0, 0));

        wuxc::Border keyBadge;
        keyBadge.CornerRadius(wux::CornerRadius{3, 3, 3, 3});
        keyBadge.Background(isLight ? MakeBrush(0x0C, 0x00, 0x00, 0x00) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
        keyBadge.BorderBrush(isLight ? MakeBrush(0x20, 0x00, 0x00, 0x00) : MakeBrush(0x24, 0xFF, 0xFF, 0xFF));
        keyBadge.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        keyBadge.Padding(wux::ThicknessHelper::FromLengths(4, 1, 4, 1));
        keyBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock keyBlock;
        keyBlock.Text(winrt::hstring{key});
        ScaleFont(keyBlock, 9.5);
        keyBlock.FontWeight(wut::FontWeights::SemiBold());
        keyBlock.Opacity(0.75);
        keyBadge.Child(keyBlock);
        pair.Children().Append(keyBadge);

        wuxc::TextBlock actionBlock;
        actionBlock.Text(winrt::hstring{action});
        ScaleFont(actionBlock, 10.5);
        actionBlock.Opacity(0.45);
        actionBlock.Margin(wux::ThicknessHelper::FromLengths(4, 0, 0, 0));
        actionBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        pair.Children().Append(actionBlock);

        return pair;
    };

    rightHints.Children().Append(makeKeyCap(L"\u2191\u2193", L"Select"));
    rightHints.Children().Append(makeKeyCap(L"Tab", L"Next"));
    rightHints.Children().Append(makeKeyCap(L"\u21B5", L"Open"));
    rightHints.Children().Append(makeKeyCap(L"Ctrl+\u21B5", L"Admin"));
    rightHints.Children().Append(makeKeyCap(L"\u21E7\u21B5", L"Menu"));
    rightHints.Children().Append(makeKeyCap(L"Esc", L"Close"));

    wuxc::Grid::SetColumn(rightHints, 1);
    g_footerHints = rightHints;
    bool showHints = true;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        showHints = g_settings.showKeyHints;
    }
    rightHints.Visibility(showHints ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    footerGrid.Children().Append(rightHints);

    footerBorder.Child(footerGrid);
    root.Children().Append(footerBorder);

    // Spanning every row and column of the menu's grid.
    wuxc::Grid::SetRow(root, 0);
    wuxc::Grid::SetRowSpan(root, 12);
    wuxc::Grid::SetColumn(root, 0);
    wuxc::Grid::SetColumnSpan(root, 12);

    ownerPanel.Children().Append(root);
    g_appsList = apps;
    g_resultsList = files;
    g_resultsHost = root;


    SyncOverlayBackground();
    if (!root.Background()) {
        root.Background(isLight ? MakeBrush(0xF2, 0xF3, 0xF3, 0xF3) : MakeBrush(0xF2, 0x20, 0x20, 0x20));
        Wh_Log(L"results: no AcrylicBorder found; flat background instead");
    }

    Wh_Log(L"results: two columns and footer built");
} catch (...) {
    Wh_Log(L"results list failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// Runs on the XAML thread.
void RenderResults() try {
    if (!g_resultsList || !g_resultsHost || !g_appsList || !g_activeAppsOpt || !g_appButtonsOpt) {
        return;
    }

    bool isLight = IsLightTheme();

    std::vector<Row> files;
    std::vector<Row> appNames;
    DWORD total = 0;
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        files = g_fileRows;
        appNames = g_appRows;
        g_shownQuery = g_rowsQuery;
        total = g_totalMatches.load();
    }

    if (files.empty() && appNames.empty() && g_ourBox && g_ourBox.Text().empty()) {
        g_enterWaiting = false;
        g_currentAppRows.clear();
        g_currentFileRows.clear();
        g_selectedApp = -1;
        g_selectedFile = -1;
        g_filesColumnActive = false;
        HideOverlayAnimated();
        Wh_Log(L"render: nothing to show; menu restored");
        return;
    }

    g_resultsList.Children().Clear();
    g_currentAppRows = appNames;

    // Update bottom status bar
    if (g_footerStatus) {
        if (!appNames.empty() && (appNames[0].customGlyph == L"\uE701" || appNames[0].customGlyph == L"\uE704" || appNames[0].customGlyph == L"\uE839")) {
            std::wstring statusStr = L"Network Interfaces \u2022 " + std::to_wstring(appNames.size()) + L" active \u2022 Press Enter to copy IP";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE88E") {
            std::wstring statusStr = L"Unit Converter \u2022 " + std::to_wstring(appNames.size()) + L" conversions \u2022 Press Enter to copy";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE1D0") {
            std::wstring statusStr = L"Calculator \u2022 Press Enter to copy result";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].appIndex < 0 &&
                   (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:"))) {
            std::wstring statusStr = L"Web Search \u2022 Press Enter to search in default browser";
            if (!files.empty()) {
                statusStr += L" (" + std::to_wstring(files.size()) + L" files matched)";
            }
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else {
            std::wstring statusStr;
            if (everything::FindIpcWindow()) {
                statusStr = L"Everything \u2022 " + std::to_wstring(total) + L" matches";
                if (!appNames.empty() || !files.empty()) {
                    statusStr += L" (" + std::to_wstring(appNames.size()) + L" apps, " + std::to_wstring(files.size()) + L" files shown)";
                }
            } else {
                statusStr = L"Everything (not running)";
                if (!appNames.empty()) {
                    statusStr += L" \u2022 " + std::to_wstring(appNames.size()) + L" apps shown";
                }
            }
            g_footerStatus.Text(winrt::hstring{statusStr});
        }
    }

    if (g_footerHints) {
        bool showHints = true;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            showHints = g_settings.showKeyHints;
        }
        g_footerHints.Visibility(showHints ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }

    auto makeHeader = [isLight](const std::wstring& title, const wchar_t* iconGlyph, int count, const std::wstring& badgeText = L"") {
        wuxc::Grid headerGrid;
        headerGrid.Margin(wux::ThicknessHelper::FromLengths(8, 4, 8, 6));

        wuxc::ColumnDefinition leftCol, rightCol;
        leftCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        rightCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        headerGrid.ColumnDefinitions().Append(leftCol);
        headerGrid.ColumnDefinitions().Append(rightCol);

        wuxc::StackPanel leftStack;
        leftStack.Orientation(wuxc::Orientation::Horizontal);
        leftStack.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::FontIcon icon;
        icon.Glyph(winrt::hstring{iconGlyph});
        ScaleFont(icon, 11.5);
        icon.Opacity(0.6);
        icon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
        icon.VerticalAlignment(wux::VerticalAlignment::Center);
        leftStack.Children().Append(icon);

        wuxc::TextBlock titleBlock;
        titleBlock.Text(winrt::hstring{title});
        ScaleFont(titleBlock, 11);
        titleBlock.FontWeight(wut::FontWeights::SemiBold());
        titleBlock.Opacity(0.65);
        titleBlock.CharacterSpacing(40);
        titleBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        leftStack.Children().Append(titleBlock);

        wuxc::Grid::SetColumn(leftStack, 0);
        headerGrid.Children().Append(leftStack);

        std::wstring badgeStr = badgeText.empty() ? std::to_wstring(count) : badgeText;
        if (!badgeStr.empty() && count >= 0) {
            wuxc::Border badge;
            badge.CornerRadius(wux::CornerRadius{4, 4, 4, 4});
            badge.Background(isLight ? MakeBrush(0x0E, 0x00, 0x00, 0x00) : MakeBrush(0x15, 0xFF, 0xFF, 0xFF));
            badge.BorderBrush(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x20, 0xFF, 0xFF, 0xFF));
            badge.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
            badge.Padding(wux::ThicknessHelper::FromLengths(7, 1, 7, 1));
            badge.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::TextBlock badgeBlock;
            badgeBlock.Text(winrt::hstring{badgeStr});
            ScaleFont(badgeBlock, 9.5);
            badgeBlock.FontWeight(wut::FontWeights::SemiBold());
            badgeBlock.Opacity(0.7);
            badge.Child(badgeBlock);

            wuxc::Grid::SetColumn(badge, 1);
            headerGrid.Children().Append(badge);
        }

        return headerGrid;
    };

    // The web-search rows have no app index; a Start menu internet shortcut is an app.
    bool isWebMode = (!appNames.empty() && appNames[0].appIndex < 0 &&
                      (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:")));
    if (g_appsHeaderHolder) {
        if (!appNames.empty() && (appNames[0].customGlyph == L"\uE701" || appNames[0].customGlyph == L"\uE704" || appNames[0].customGlyph == L"\uE839")) {
            g_appsHeaderHolder.Child(makeHeader(L"NETWORK INTERFACES", L"\uE701", static_cast<int>(appNames.size())));
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE88E") {
            g_appsHeaderHolder.Child(makeHeader(L"UNIT CONVERTER", L"\uE88E", static_cast<int>(appNames.size())));
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE1D0") {
            g_appsHeaderHolder.Child(makeHeader(L"CALCULATOR", L"\uE1D0", static_cast<int>(appNames.size())));
        } else if (isWebMode) {
            g_appsHeaderHolder.Child(makeHeader(L"WEB SEARCH", L"\uE774", static_cast<int>(appNames.size())));
        } else {
            g_appsHeaderHolder.Child(makeHeader(L"APPS", L"\uE71D", static_cast<int>(appNames.size())));
        }
    }
    std::wstring filesBadge = std::to_wstring(files.size()) + L" of " + std::to_wstring(total);
    if (g_filesHeaderHolder) {
        g_filesHeaderHolder.Child(makeHeader(L"FILES", L"\uE8B7", static_cast<int>(files.size()), filesBadge));
    }

    auto makeAppCard = [isLight](const Row& item) -> AppCardUI {
        wuxc::Grid layout;
        wuxc::ColumnDefinition iconCol, textCol;
        iconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        textCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        layout.ColumnDefinitions().Append(iconCol);
        layout.ColumnDefinitions().Append(textCol);

        bool hasBitmap = false;
        if (auto bmp = IconBitmap(item.icon)) {
            wuxc::Image image;
            image.Source(bmp);
            image.Width(kIconDisplay);
            image.Height(kIconDisplay);
            image.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            image.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetColumn(image, 0);
            layout.Children().Append(image);
            hasBitmap = true;
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::FontIcon fallbackIcon;
            ScaleFont(fallbackIcon, 15);
            fallbackIcon.Opacity(0.5);
            fallbackIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
            fallbackIcon.VerticalAlignment(wux::VerticalAlignment::Center);

            if (item.openPath.starts_with(L"ms-settings:") || item.subtitle.starts_with(L"Settings")) {
                fallbackIcon.Glyph(L"\uE713");
                fallbackIcon.Opacity(0.7);
            } else if (item.openPath.starts_with(L"http:") || item.openPath.starts_with(L"https:")) {
                fallbackIcon.Glyph(L"\uE774");
                fallbackIcon.Opacity(0.85);
            } else if (!item.customGlyph.empty()) {
                fallbackIcon.Glyph(winrt::hstring{item.customGlyph});
                fallbackIcon.Opacity(0.85);
                if (const uint32_t c = item.glyphColor) {
                    fallbackIcon.Foreground(MakeBrush(static_cast<uint8_t>(c >> 24), static_cast<uint8_t>(c >> 16),
                                                      static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)));
                    fallbackIcon.Opacity(1.0);
                }
            } else {
                fallbackIcon.Glyph(L"\uE71D");
            }
            iconBox.Child(fallbackIcon);
            wuxc::Grid::SetColumn(iconBox, 0);
            layout.Children().Append(iconBox);
        }

        wuxc::StackPanel text;
        wuxc::Grid::SetColumn(text, 1);
        text.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock name;
        name.Text(winrt::hstring{item.title});
        ScaleFont(name, 12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        // A result to copy -- a sum, a conversion -- is shown whole: a long
        // one wraps instead of being cut off.
        const bool whole = !item.copyText.empty();
        if (whole) {
            name.TextWrapping(wux::TextWrapping::Wrap);
            name.MaxLines(4);
        }
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            wuxc::TextBlock sub;
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            ScaleFont(sub, 10.5);
            sub.Margin(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
            sub.TextTrimming(wux::TextTrimming::CharacterEllipsis);
            sub.TextWrapping(whole ? wux::TextWrapping::Wrap : wux::TextWrapping::NoWrap);
            if (whole) {
                sub.MaxLines(2);
            }
            text.Children().Append(sub);
        }
        layout.Children().Append(text);

        wuxc::Button button;
        button.Content(layout);
        button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        button.Background(MakeBrush(0, 0, 0, 0));
        button.BorderBrush(MakeBrush(0, 0, 0, 0));
        button.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        button.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
        button.Padding(wux::ThicknessHelper::FromLengths(10, 6, 10, 6));
        button.Margin(wux::ThicknessHelper::FromLengths(2, 1, 2, 1));
        button.IsTabStop(false);
        if (whole) {
            wuxc::ToolTipService::SetToolTip(button, winrt::box_value(winrt::hstring{item.title}));
        }

        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPointerOver"),
            isLight ? MakeBrush(0x14, 0x00, 0x5F, 0xB8) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPointerOver"),
            isLight ? MakeBrush(0x80, 0x00, 0x5F, 0xB8) : MakeBrush(0x55, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPressed"),
            isLight ? MakeBrush(0x24, 0x00, 0x5F, 0xB8) : MakeBrush(0x35, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPressed"),
            isLight ? MakeBrush(0xA0, 0x00, 0x5F, 0xB8) : MakeBrush(0x70, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundFocused"),
            MakeBrush(0, 0, 0, 0));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushFocused"),
            MakeBrush(0, 0, 0, 0));

        // Weak: the button holds these handlers, and a strong self-reference
        // would keep every card ever built alive.
        KeepHandler(button, button.PointerEntered(winrt::auto_revoke, [weakBtn = winrt::make_weak(button)](wf::IInspectable const&,
                                                                   wux::Input::PointerRoutedEventArgs const&) {
            auto btn = weakBtn.get();
            if (!btn || !g_activeAppsOpt || g_dragging) return;  // a drag passing over
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    SetAppSelection(static_cast<int>(i));
                    break;
                }
            }
        }));

        KeepHandler(button, button.Click(winrt::auto_revoke, [weakBtn = winrt::make_weak(button)](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            auto btn = weakBtn.get();
            if (!btn || !g_activeAppsOpt) return;
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    LaunchSelectedApp(static_cast<int>(i), false);
                    return;
                }
            }
        }));

        wuxc::MenuFlyout menu;
        KeepHandler(menu, menu.Opened(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) { NoteContextMenuOpened(); }));
        KeepHandler(menu, menu.Closed(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) { NoteContextMenuClosed(); }));
        // Filled in when first opened, not with the card (see the file rows).
        // What it needs of the row is copied, less the icon pixels; the card
        // is held weakly, since it owns this menu.
        Row menuItem = item;
        menuItem.icon.clear();
        KeepHandler(menu, menu.Opening(winrt::auto_revoke, [item = std::move(menuItem), weakBtn = winrt::make_weak(button)](
                         wf::IInspectable const& sender, wf::IInspectable const&) {
            auto flyout = sender.as<wuxc::MenuFlyout>();
            if (flyout.Items().Size() > 0) return;
            if (!item.copyText.empty()) {
                wuxc::MenuFlyoutItem copyItem;
                copyItem.Text(L"Copy to clipboard");
                wuxc::FontIcon copyIcon;
                copyIcon.Glyph(L"\uE8C8");
                copyItem.Icon(copyIcon);
                KeepHandler(copyItem, copyItem.Click(winrt::auto_revoke, [txt = item.copyText](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyTextToClipboard(txt);
                }));
                flyout.Items().Append(copyItem);
            } else {
                bool isWebItem = item.appIndex < 0 && (item.openPath.starts_with(L"http:") || item.openPath.starts_with(L"https:"));
                bool isSettingItem = item.isSetting;

                wuxc::MenuFlyoutItem openItem;
                openItem.Text(isWebItem ? L"Search in browser" : L"Open");
                wuxc::FontIcon openIcon;
                openIcon.Glyph(isWebItem ? L"\uE774" : L"\uE8A7");
                openItem.Icon(openIcon);
                KeepHandler(openItem, openItem.Click(winrt::auto_revoke, [weakBtn](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    auto btn = weakBtn.get();
                    if (!btn || !g_activeAppsOpt) return;
                    for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                        if ((*g_activeAppsOpt)[i].button == btn) {
                            LaunchSelectedApp(static_cast<int>(i), false);
                            return;
                        }
                    }
                }));
                flyout.Items().Append(openItem);

                if (item.canRunAsAdmin && !isWebItem && !isSettingItem) {
                    wuxc::MenuFlyoutItem adminItem;
                    adminItem.Text(L"Run as administrator");
                    wuxc::FontIcon adminIcon;
                    adminIcon.Glyph(L"\uE7EF");
                    adminItem.Icon(adminIcon);
                    KeepHandler(adminItem, adminItem.Click(winrt::auto_revoke, [weakBtn](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        auto btn = weakBtn.get();
                        if (!btn || !g_activeAppsOpt) return;
                        for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                            if ((*g_activeAppsOpt)[i].button == btn) {
                                LaunchSelectedApp(static_cast<int>(i), true);
                                return;
                            }
                        }
                    }));
                    flyout.Items().Append(adminItem);
                }

                if (isWebItem) {
                    std::wstring webUrl = item.openPath;
                    wuxc::MenuFlyoutItem copyUrlItem;
                    copyUrlItem.Text(L"Copy search link");
                    wuxc::FontIcon copyIcon;
                    copyIcon.Glyph(L"\uE8C8");
                    copyUrlItem.Icon(copyIcon);
                    KeepHandler(copyUrlItem, copyUrlItem.Click(winrt::auto_revoke, [webUrl](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        tools::CopyTextToClipboard(webUrl);
                    }));
                    flyout.Items().Append(copyUrlItem);
                } else if (!isSettingItem && !item.openPath.empty()) {
                    std::wstring locTarget = item.openPath;
                    std::wstring appTitle = item.title;
                    bool isFile = item.isFile;
                    bool viaAppId = !isFile && !locTarget.starts_with(L"ms-settings:") &&
                                    !locTarget.starts_with(L"http:") && !locTarget.starts_with(L"https:");
                    // An app launched by its app ID can still have a program
                    // on disk, which the card shows; the file actions use it.
                    std::wstring filePath = isFile ? locTarget : viaAppId ? item.programPath : L"";

                    if (isFile || viaAppId) {
                        wuxc::MenuFlyoutSeparator sep1;
                        flyout.Items().Append(sep1);

                        if (!filePath.empty()) {
                            wuxc::MenuFlyoutItem locItem;
                            locItem.Text(L"Open file location");
                            wuxc::FontIcon locIcon;
                            locIcon.Glyph(L"\uE838");
                            locItem.Icon(locIcon);
                            KeepHandler(locItem, locItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                OpenFileLocationThenDismiss(filePath);
                            }));
                            flyout.Items().Append(locItem);

                            wuxc::MenuFlyoutItem copyPathItem;
                            copyPathItem.Text(L"Copy path");
                            wuxc::FontIcon copyPathIcon;
                            copyPathIcon.Glyph(L"\uE71B");
                            copyPathItem.Icon(copyPathIcon);
                            KeepHandler(copyPathItem, copyPathItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                tools::CopyTextToClipboard(filePath);
                            }));
                            flyout.Items().Append(copyPathItem);
                        }

                        // Through the app ID where there is one, so the
                        // shortcut keeps the original's arguments.
                        std::wstring shortcutTarget = isFile ? locTarget : L"shell:AppsFolder\\" + locTarget;
                        wuxc::MenuFlyoutItem shortcutItem;
                        shortcutItem.Text(L"Create desktop shortcut");
                        wuxc::FontIcon shortcutIcon;
                        shortcutIcon.Glyph(L"\uE7C5");
                        shortcutItem.Icon(shortcutIcon);
                        KeepHandler(shortcutItem, shortcutItem.Click(winrt::auto_revoke, [shortcutTarget, appTitle](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                            tools::CreateDesktopShortcut(shortcutTarget, appTitle);
                        }));
                        flyout.Items().Append(shortcutItem);

                        if (!filePath.empty()) {
                            wuxc::MenuFlyoutSeparator sep2;
                            flyout.Items().Append(sep2);

                            wuxc::MenuFlyoutItem propItem;
                            propItem.Text(L"Properties");
                            wuxc::FontIcon propIcon;
                            propIcon.Glyph(L"\uE946");
                            propItem.Icon(propIcon);
                            KeepHandler(propItem, propItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                AllowExplorerForeground();
                                DismissStartMenu();
                                ShowPropertiesDialog(filePath);
                            }));
                            flyout.Items().Append(propItem);
                        }

                        // Uninstall, as in Start's own menu: a packaged app is
                        // removed, a program runs its uninstaller (uninstall).
                        // Offered only where one of them applies.
                        std::wstring family = viaAppId ? uninstall::RemovablePackageFamily(locTarget) : std::wstring();
                        std::optional<uninstall::Command> uninstaller;
                        if (family.empty()) {
                            std::wstring program = filePath;
                            if (std::wstring target = apps::ResolveLnkTarget(program); !target.empty()) {
                                program = target;
                            }
                            uninstaller = uninstall::FindProgramUninstaller(program, appTitle);
                        }
                        if (!family.empty() || uninstaller) {
                            wuxc::MenuFlyoutSeparator sep3;
                            flyout.Items().Append(sep3);

                            wuxc::MenuFlyoutItem uninstallItem;
                            uninstallItem.Text(L"Uninstall");
                            wuxc::FontIcon uninstallIcon;
                            uninstallIcon.Glyph(L"\uE74D");
                            uninstallItem.Icon(uninstallIcon);
                            KeepHandler(uninstallItem, uninstallItem.Click(winrt::auto_revoke, [weakBtn, family, uninstaller, appTitle](
                                                                               wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                auto btn = weakBtn.get();
                                if (!btn) return;
                                // After the menu has closed: one flyout at a time.
                                btn.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Low, wuc::DispatchedHandler{[weakBtn, family, uninstaller, appTitle] {
                                    auto anchor = weakBtn.get();
                                    if (!anchor) return;
                                    ConfirmUninstall(anchor, uninstaller ? uninstaller->name : appTitle, [family, uninstaller, appTitle] {
                                        if (!family.empty()) {
                                            if (RequestPackageRemoval(family)) {
                                                DismissStartMenu();
                                            }
                                        } else if (uninstaller) {
                                            DismissStartMenuForLaunch([command = *uninstaller] { RunUninstaller(command); });
                                        }
                                    });
                                }});
                            }));
                            flyout.Items().Append(uninstallItem);
                        }
                    }
                }
            }

        }));
        button.ContextFlyout(menu);

        return AppCardUI{item.appIndex, item.title, item.openPath, button, item.canRunAsAdmin, item.isSetting, item.isFile};
    };

    auto makeAppEmptyCard = [isLight]() -> wuxc::Border {
        wuxc::Border emptyCard;
        emptyCard.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
        emptyCard.Background(isLight ? MakeBrush(0x08, 0x00, 0x00, 0x00) : MakeBrush(0x0A, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderBrush(isLight ? MakeBrush(0x15, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        emptyCard.Padding(wux::ThicknessHelper::FromLengths(16, 22, 16, 22));
        emptyCard.Margin(wux::ThicknessHelper::FromLengths(6, 10, 6, 8));
        emptyCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

        wuxc::StackPanel emptyStack;
        emptyStack.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::FontIcon emptyIcon;
        emptyIcon.Glyph(L"\uE71D");
        ScaleFont(emptyIcon, 24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        wuxc::TextBlock emptyTitle;
        emptyTitle.Text(L"No matching applications");
        ScaleFont(emptyTitle, 11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        wuxc::TextBlock emptySubtitle;
        emptySubtitle.Text(L"Check files or refine your query");
        ScaleFont(emptySubtitle, 10.5);
        emptySubtitle.Opacity(0.3);
        emptySubtitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptySubtitle.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        emptyStack.Children().Append(emptySubtitle);

        emptyCard.Child(emptyStack);
        return emptyCard;
    };

    auto isMatch = [](const AppCardUI& card, const Row& row) {
        if (!card.openPath.empty() && !row.openPath.empty()) {
            return card.openPath == row.openPath && card.title == row.title;
        }
        return card.title == row.title;
    };

    if (appNames.empty()) {
        for (int i = static_cast<int>(g_activeAppsOpt->size()) - 1; i >= 0; --i) {
            g_appsList.Children().RemoveAt(i);
        }
        g_activeAppsOpt->clear();
        g_appButtonsOpt->clear();

        if (g_appsList.Children().Size() == 0) {
            g_appsList.Children().Append(makeAppEmptyCard());
        }
        SetAppSelection(-1);
    } else {
        if (g_activeAppsOpt->empty() && g_appsList.Children().Size() > 0) {
            g_appsList.Children().Clear();
        }

        // 1. Remove cards that are no longer in appNames
        for (int i = static_cast<int>(g_activeAppsOpt->size()) - 1; i >= 0; --i) {
            bool found = false;
            for (const auto& newApp : appNames) {
                if (isMatch((*g_activeAppsOpt)[i], newApp)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                g_appsList.Children().RemoveAt(i);
                g_activeAppsOpt->erase(g_activeAppsOpt->begin() + i);
            }
        }

        // 2. Insert new cards or reorder existing ones
        for (size_t targetIdx = 0; targetIdx < appNames.size(); ++targetIdx) {
            const Row& want = appNames[targetIdx];
            int existingIdx = -1;
            for (size_t i = targetIdx; i < g_activeAppsOpt->size(); ++i) {
                if (isMatch((*g_activeAppsOpt)[i], want)) {
                    existingIdx = static_cast<int>(i);
                    break;
                }
            }

            if (existingIdx >= 0) {
                (*g_activeAppsOpt)[existingIdx].appIndex = want.appIndex;
                (*g_activeAppsOpt)[existingIdx].canRunAsAdmin = want.canRunAsAdmin;
                (*g_activeAppsOpt)[existingIdx].isSetting = want.isSetting;
                (*g_activeAppsOpt)[existingIdx].isFile = want.isFile;

                if (static_cast<size_t>(existingIdx) != targetIdx) {
                    auto card = (*g_activeAppsOpt)[existingIdx];
                    g_activeAppsOpt->erase(g_activeAppsOpt->begin() + existingIdx);
                    g_activeAppsOpt->insert(g_activeAppsOpt->begin() + targetIdx, card);

                    g_appsList.Children().RemoveAt(existingIdx);
                    g_appsList.Children().InsertAt(static_cast<uint32_t>(targetIdx), card.button);
                }
            } else {
                AppCardUI newCard = makeAppCard(want);
                g_activeAppsOpt->insert(g_activeAppsOpt->begin() + targetIdx, newCard);
                g_appsList.Children().InsertAt(static_cast<uint32_t>(targetIdx), newCard.button);
            }
        }

        g_appButtonsOpt->clear();
        for (const auto& card : *g_activeAppsOpt) {
            g_appButtonsOpt->push_back(card.button);
        }

        SetAppSelection(0);
    }

    // Files renderer
    auto makeFileRow = [isLight](const Row& item) {
        wuxc::Grid layout;
        wuxc::ColumnDefinition iconCol, textCol;
        iconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        textCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        layout.ColumnDefinitions().Append(iconCol);
        layout.ColumnDefinitions().Append(textCol);

        bool hasBitmap = false;
        if (auto bmp = IconBitmap(item.icon)) {
            wuxc::Image image;
            image.Source(bmp);
            image.Width(kIconDisplay);
            image.Height(kIconDisplay);
            image.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            image.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetColumn(image, 0);
            layout.Children().Append(image);
            hasBitmap = true;
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::FontIcon fallbackIcon;
            ScaleFont(fallbackIcon, 15);
            fallbackIcon.Opacity(0.5);
            fallbackIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
            fallbackIcon.VerticalAlignment(wux::VerticalAlignment::Center);

            if (item.isFolder) {
                fallbackIcon.Glyph(L"\uE8B7");
            } else {
                fallbackIcon.Glyph(L"\uE8A5");
            }
            iconBox.Child(fallbackIcon);
            wuxc::Grid::SetColumn(iconBox, 0);
            layout.Children().Append(iconBox);
        }

        wuxc::StackPanel text;
        wuxc::Grid::SetColumn(text, 1);
        text.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock name;
        name.Text(winrt::hstring{item.title});
        ScaleFont(name, 12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            wuxc::TextBlock sub;
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            ScaleFont(sub, 10.5);
            sub.Margin(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
            sub.TextTrimming(wux::TextTrimming::CharacterEllipsis);
            sub.TextWrapping(wux::TextWrapping::NoWrap);
            text.Children().Append(sub);
        }
        layout.Children().Append(text);

        wuxc::Button button;
        button.Content(layout);
        button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        button.Background(MakeBrush(0, 0, 0, 0));
        button.BorderBrush(MakeBrush(0, 0, 0, 0));
        button.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        button.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
        button.Padding(wux::ThicknessHelper::FromLengths(10, 6, 10, 6));
        button.Margin(wux::ThicknessHelper::FromLengths(2, 1, 2, 1));
        button.IsTabStop(false);

        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPointerOver"),
            isLight ? MakeBrush(0x14, 0x00, 0x5F, 0xB8) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPointerOver"),
            isLight ? MakeBrush(0x80, 0x00, 0x5F, 0xB8) : MakeBrush(0x55, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPressed"),
            isLight ? MakeBrush(0x24, 0x00, 0x5F, 0xB8) : MakeBrush(0x35, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPressed"),
            isLight ? MakeBrush(0xA0, 0x00, 0x5F, 0xB8) : MakeBrush(0x70, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundFocused"),
            MakeBrush(0, 0, 0, 0));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushFocused"),
            MakeBrush(0, 0, 0, 0));

        // Hovering a row selects it, as it does an app card, so the pointer's
        // highlight and the keyboard's are the same one. Weak: the button
        // holds this handler.
        KeepHandler(button, button.PointerEntered(winrt::auto_revoke, [weak = winrt::make_weak(button)](wf::IInspectable const&,
                                                                wux::Input::PointerRoutedEventArgs const&) {
            auto btn = weak.get();
            if (!btn || !g_fileButtonsOpt || g_dragging) return;  // a drag passing over
            for (size_t i = 0; i < g_fileButtonsOpt->size(); ++i) {
                if ((*g_fileButtonsOpt)[i] == btn) {
                    SetFileSelection(static_cast<int>(i));
                    break;
                }
            }
        }));

        if (!item.openPath.empty()) {
            std::wstring target = item.openPath;
            KeepHandler(button, button.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (g_dragging) {
                    return;  // the release that ends a drag (StartFileDrag)
                }
                DismissStartMenuForLaunch([target] { OpenResult(target); });
            }));

            // Dragging the row drags the file (StartFileDrag). A Button takes the
            // mouse the moment it is pressed and handles the events, so the
            // press and the moves are watched here, handled or not, and the
            // drag starts once the pointer has moved a few pixels with the
            // button down. The Button lets the mouse go then, so no click
            // follows.
            auto pressedAt = std::make_shared<std::optional<wf::Point>>();
            const auto weakButton = winrt::make_weak(button);
            AddPointerHandler(button, wux::UIElement::PointerPressedEvent(),
                              [pressedAt, weakButton](wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const& e) {
                                  auto btn = weakButton.get();
                                  pressedAt->reset();
                                  if (btn) {
                                      auto point = e.GetCurrentPoint(btn);
                                      if (point.Properties().IsLeftButtonPressed()) {
                                          *pressedAt = point.Position();
                                      }
                                  }
                              });
            AddPointerHandler(button, wux::UIElement::PointerMovedEvent(),
                              [pressedAt, weakButton, target](wf::IInspectable const&,
                                                              wux::Input::PointerRoutedEventArgs const& e) {
                                  auto btn = weakButton.get();
                                  if (!*pressedAt || !btn) {
                                      return;
                                  }
                                  auto point = e.GetCurrentPoint(btn);
                                  if (!point.Properties().IsLeftButtonPressed()) {
                                      pressedAt->reset();
                                      return;
                                  }
                                  const float dx = point.Position().X - (*pressedAt)->X;
                                  const float dy = point.Position().Y - (*pressedAt)->Y;
                                  if (dx * dx + dy * dy < 36) {
                                      return;
                                  }
                                  pressedAt->reset();
                                  btn.ReleasePointerCaptures();
                                  HidePreview();
                                  StartFileDrag(target);
                              });
            AddPointerHandler(button, wux::UIElement::PointerReleasedEvent(),
                              [pressedAt](wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
                                  pressedAt->reset();
                              });

            wuxc::MenuFlyout menu;
            KeepHandler(menu, menu.Opened(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) { NoteContextMenuOpened(); }));
            KeepHandler(menu, menu.Closed(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) { NoteContextMenuClosed(); }));
            // Filled in when first opened, not with the row: every keystroke
            // builds every row, and hardly any of their menus are ever opened.
            KeepHandler(menu, menu.Opening(winrt::auto_revoke, [target, isFolder = item.isFolder, title = item.title](
                             wf::IInspectable const& sender, wf::IInspectable const&) {
                auto flyout = sender.as<wuxc::MenuFlyout>();
                if (flyout.Items().Size() > 0) return;
                wuxc::MenuFlyoutItem openItem;
                openItem.Text(L"Open");
                wuxc::FontIcon openIcon;
                openIcon.Glyph(L"\uE8A7");
                openItem.Icon(openIcon);
                KeepHandler(openItem, openItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    DismissStartMenuForLaunch([target] { OpenResult(target); });
                }));
                flyout.Items().Append(openItem);

                if (CanElevatePath(target)) {
                    wuxc::MenuFlyoutItem adminItem;
                    adminItem.Text(L"Run as administrator");
                    wuxc::FontIcon adminIcon;
                    adminIcon.Glyph(L"\uE7EF");
                    adminItem.Icon(adminIcon);
                    KeepHandler(adminItem, adminItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenuForLaunch([target] { OpenResult(target, true /* asAdmin */); });
                    }));
                    flyout.Items().Append(adminItem);
                }

                if (isFolder) {
                    wuxc::MenuFlyoutSubItem termSub;
                    termSub.Text(L"Open in terminal");
                    wuxc::FontIcon termIcon;
                    termIcon.Glyph(L"\uE756");
                    termSub.Icon(termIcon);

                    auto addTermItem = [&](const wchar_t* label, bool isPowerShell, bool asAdmin) {
                        wuxc::MenuFlyoutItem termItem;
                        termItem.Text(winrt::hstring{label});
                        wuxc::FontIcon icon;
                        icon.Glyph(asAdmin ? L"\uE7EF" : L"\uE756");
                        termItem.Icon(icon);
                        KeepHandler(termItem, termItem.Click(winrt::auto_revoke, [target, isPowerShell, asAdmin](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                            DismissStartMenuForLaunch(
                                [target, isPowerShell, asAdmin] { LaunchTerminal(target, isPowerShell, asAdmin); });
                        }));
                        termSub.Items().Append(termItem);
                    };

                    addTermItem(L"Command Prompt", false, false);
                    addTermItem(L"Command Prompt (Administrator)", false, true);
                    addTermItem(L"PowerShell", true, false);
                    addTermItem(L"PowerShell (Administrator)", true, true);

                    flyout.Items().Append(termSub);
                }

                wuxc::MenuFlyoutSeparator sep1;
                flyout.Items().Append(sep1);

                wuxc::MenuFlyoutItem cutItem;
                cutItem.Text(L"Cut");
                wuxc::FontIcon cutIcon;
                cutIcon.Glyph(L"\uE8C6");
                cutItem.Icon(cutIcon);
                KeepHandler(cutItem, cutItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyOrCutFileToClipboard(target, true /* isCut */);
                }));
                flyout.Items().Append(cutItem);

                wuxc::MenuFlyoutItem copyItem;
                copyItem.Text(L"Copy");
                wuxc::FontIcon copyIcon;
                copyIcon.Glyph(L"\uE8C8");
                copyItem.Icon(copyIcon);
                KeepHandler(copyItem, copyItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyOrCutFileToClipboard(target, false /* isCut */);
                }));
                flyout.Items().Append(copyItem);

                wuxc::MenuFlyoutItem copyPathItem;
                copyPathItem.Text(L"Copy path");
                wuxc::FontIcon copyPathIcon;
                copyPathIcon.Glyph(L"\uE71B");
                copyPathItem.Icon(copyPathIcon);
                KeepHandler(copyPathItem, copyPathItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyTextToClipboard(target);
                }));
                flyout.Items().Append(copyPathItem);

                wuxc::MenuFlyoutSeparator sep2;
                flyout.Items().Append(sep2);

                wuxc::MenuFlyoutItem locItem;
                locItem.Text(L"Open file location");
                wuxc::FontIcon locIcon;
                locIcon.Glyph(L"\uE838");
                locItem.Icon(locIcon);
                KeepHandler(locItem, locItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    OpenFileLocationThenDismiss(target);
                }));
                flyout.Items().Append(locItem);

                wuxc::MenuFlyoutItem shortcutItem;
                shortcutItem.Text(L"Create desktop shortcut");
                wuxc::FontIcon shortcutIcon;
                shortcutIcon.Glyph(L"\uE7C5");
                shortcutItem.Icon(shortcutIcon);
                KeepHandler(shortcutItem, shortcutItem.Click(winrt::auto_revoke, [target, title](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CreateDesktopShortcut(target, title);
                }));
                flyout.Items().Append(shortcutItem);

                wuxc::MenuFlyoutSeparator sep3;
                flyout.Items().Append(sep3);

                wuxc::MenuFlyoutItem propItem;
                propItem.Text(L"Properties");
                wuxc::FontIcon propIcon;
                propIcon.Glyph(L"\uE946");
                propItem.Icon(propIcon);
                KeepHandler(propItem, propItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    AllowExplorerForeground();
                    DismissStartMenu();
                    ShowPropertiesDialog(target);
                }));
                flyout.Items().Append(propItem);

            }));
            button.ContextFlyout(menu);
        }
        return button;
    };

    // Files on the right.
    g_currentFileRows = files;
    g_selectedFile = -1;
    if (!g_fileButtonsOpt) g_fileButtonsOpt.emplace();
    g_fileButtonsOpt->clear();
    if (files.empty()) {
        wuxc::Border emptyCard;
        emptyCard.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
        emptyCard.Background(isLight ? MakeBrush(0x08, 0x00, 0x00, 0x00) : MakeBrush(0x0A, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderBrush(isLight ? MakeBrush(0x15, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        emptyCard.Padding(wux::ThicknessHelper::FromLengths(16, 22, 16, 22));
        emptyCard.Margin(wux::ThicknessHelper::FromLengths(6, 10, 6, 8));
        emptyCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

        wuxc::StackPanel emptyStack;
        emptyStack.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::FontIcon emptyIcon;
        emptyIcon.Glyph(L"\uE8B7");
        ScaleFont(emptyIcon, 24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        wuxc::TextBlock emptyTitle;
        emptyTitle.Text(L"No matching files");
        ScaleFont(emptyTitle, 11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        wuxc::TextBlock emptySubtitle;
        emptySubtitle.Text(L"Everything index returned 0 items");
        ScaleFont(emptySubtitle, 10.5);
        emptySubtitle.Opacity(0.3);
        emptySubtitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptySubtitle.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        emptyStack.Children().Append(emptySubtitle);

        emptyCard.Child(emptyStack);
        g_resultsList.Children().Append(emptyCard);
    } else {
        for (const Row& file : files) {
            wuxc::Button row = makeFileRow(file);
            g_resultsList.Children().Append(row);
            g_fileButtonsOpt->push_back(row);
        }
    }

    // New results start the keyboard selection over: on the first app, or on
    // the first file when there are no apps.
    if (g_appButtonsOpt->empty() && !g_fileButtonsOpt->empty()) {
        SetFileSelection(0);
    } else {
        g_filesColumnActive = false;
    }

    if (g_ourBox && g_ourBox.Text().empty()) {
        HideOverlayAnimated();
    } else {
        RevealOverlayAnimated();
    }
    Wh_Log(L"render: %zu apps, %zu files, host %.0fx%.0f", appNames.size(),
        files.size(), g_resultsHost.ActualWidth(),
        g_resultsHost.ActualHeight());

    if (g_enterWaiting && !ResultsPending()) {
        g_enterWaiting = false;
        if (GetTickCount64() - g_enterWaitingTick < kEnterWaitMs) {
            Wh_Log(L"render: running the Enter that waited for these results");
            ActivateSelection(g_enterWaitingCtrl);
        }
    }
} catch (...) {
    Wh_Log(L"render failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// File icons fetched after their rows were drawn (see FileIconCache), under
// g_resultsMutex. They are swapped into the rows in place: drawing the rows
// again would start the keyboard selection over.
std::vector<std::pair<std::wstring, std::vector<BYTE>>> g_lateFileIcons;

void ApplyLateFileIcons() try {
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> late;
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        late.swap(g_lateFileIcons);
    }
    if (!g_fileButtonsOpt) {
        return;
    }
    const size_t rows = std::min(g_currentFileRows.size(), g_fileButtonsOpt->size());
    for (const auto& [path, pixels] : late) {
        auto bmp = IconBitmap(pixels);
        if (!bmp) {
            continue;
        }
        for (size_t i = 0; i < rows; ++i) {
            if (g_currentFileRows[i].openPath != path) {
                continue;
            }
            auto layout = (*g_fileButtonsOpt)[i].Content().try_as<wuxc::Grid>();
            auto image = layout && layout.Children().Size() > 0
                             ? layout.Children().GetAt(0).try_as<wuxc::Image>()
                             : nullptr;
            if (!image) {
                continue;
            }
            image.Source(bmp);
        }
    }
} catch (...) {
}

// Called from the search thread.
void PublishLateFileIcons(std::vector<std::pair<std::wstring, std::vector<BYTE>>> late) {
    if (late.empty()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        for (auto& icon : late) {
            g_lateFileIcons.push_back(std::move(icon));
        }
    }
    try {
        if (g_ourBox) {
            g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                           wuc::DispatchedHandler{[] { ApplyLateFileIcons(); }});
        }
    } catch (...) {
    }
}

// Called from the search thread once results are in.
void RequestRender() {
    try {
        if (!g_ourBox) {
            return;
        }
        auto dispatcher = g_ourBox.Dispatcher();
        if (!dispatcher) {
            return;
        }
        dispatcher.RunAsync(wuc::CoreDispatcherPriority::High,
                            wuc::DispatchedHandler{[] { RenderResults(); }});
    } catch (...) {
    }
}

// ---------------------------------------------------------------------------
// Asking Everything, from inside the Start menu
//
// On its own thread, with its own message pump: the IPC is a WM_COPYDATA
// round trip and the reply lands on a window, so it cannot run on the XAML
// thread without blocking the menu while the user types.
//
// Debounced, because a keystroke every ~60ms would otherwise be a query every
// ~60ms. 120ms was measured as comfortable in the broker.
// ---------------------------------------------------------------------------



void QueueQuery(std::wstring text) {
    {
        std::lock_guard<std::mutex> lock(g_queryMutex);
        g_pendingQuery = std::move(text);
        g_queryDirty.store(true);
    }
    g_queryWake.notify_all();
}

// An app's icon, from its shell identity.
//
// Not the extension cache: that answers from a file type, and an app is not a
// file type -- every app has its own icon, so there is nothing to share. It
// goes through the item itself for the same reason the index stores PIDLs
// rather than paths: AUMIDs and known-folder GUIDs cannot be re-parsed back
// into something SHGetFileInfo understands.
//
// Measured at roughly 9.7ms per app in the broker, so this runs on the search
// thread and is cached by name. Six visible rows make it about 60ms once, and
// nothing after that.
bool FetchAppIcon(const apps::App* app, int size, std::vector<BYTE>* out) {
    if (!app || !app->pidl || !out) {
        return false;
    }
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromIDList(app->pidl.get(), IID_PPV_ARGS(&item))) ||
        !item) {
        return false;
    }
    bool ok = false;
    IShellItemImageFactory* factory = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
        SIZE want{size, size};
        HBITMAP bitmap = nullptr;
        if (SUCCEEDED(factory->GetImage(
                want, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap)) &&
            bitmap) {
            ok = icons::BitmapToBgra(bitmap, size, out);
            DeleteObject(bitmap);
        }
        factory->Release();
    }
    item->Release();
    return ok;
}

// Opens an app.
//
// One with no file of its own goes by its shell identity: the parsing names
// come in several shapes, including AUMIDs and known-folder GUIDs, and
// rebuilding a path from them fails outright for some, while the PIDL, or the
// shell:AppsFolder name made from it (AppsFolderPath), works for all of them.
//
// Runs on a launch thread. The index stays with the search thread, so this
// gets copies of what it needs, the PIDL included.
void LaunchAppAsync(std::wstring name, std::wstring path, ITEMIDLIST* pidl, bool asAdmin = false) {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool launched = false;

    wchar_t userProfile[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH) || !userProfile[0]) {
        PWSTR kf = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &kf)) && kf) {
            wcsncpy_s(userProfile, kf, MAX_PATH - 1);
            CoTaskMemFree(kf);
        }
    }

    std::wstring lowerPath = path;
    for (auto& c : lowerPath) c = static_cast<wchar_t>(towlower(c));
    std::wstring lowerName = name;
    for (auto& c : lowerName) c = static_cast<wchar_t>(towlower(c));

    bool isTerminalOrShell = (lowerPath.find(L"cmd.exe") != std::wstring::npos ||
                              lowerPath.find(L"powershell") != std::wstring::npos ||
                              lowerPath.find(L"pwsh") != std::wstring::npos ||
                              lowerPath.find(L"windowsterminal") != std::wstring::npos ||
                              lowerPath.ends_with(L"wt.exe") ||
                              lowerName == L"command prompt" ||
                              lowerName.find(L"powershell") != std::wstring::npos ||
                              lowerName == L"terminal" ||
                              lowerName == L"windows terminal");

    size_t lastSlash = path.find_last_of(L"\\/");
    std::wstring parentDir = (lastSlash != std::wstring::npos) ? path.substr(0, lastSlash) : L"";
    LPCWSTR workDir = isTerminalOrShell ? userProfile : (!parentDir.empty() ? parentDir.c_str() : userProfile);

    // Expand environment strings if any (e.g. %windir%\system32\...)
    if (path.find(L'%') != std::wstring::npos) {
        wchar_t expanded[MAX_PATH] = {};
        if (ExpandEnvironmentStringsW(path.c_str(), expanded, MAX_PATH) > 0) {
            path = expanded;
        }
    }

    // 0. If path is a URI (e.g. ms-settings:, http:, https:) or command:
    if (!path.empty()) {
        if (path.starts_with(L"ms-settings:") || path.starts_with(L"http:") || path.starts_with(L"https:")) {
            if (ShellExecuteInExplorer(path, L"", L"")) {
                launched = true;
            } else if (HINSTANCE hInst = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                       reinterpret_cast<INT_PTR>(hInst) > 32) {
                Wh_Log(L"launched URI setting: %ls", path.c_str());
                launched = true;
            } else {
                Wh_Log(L"URI launch failed (%ld): %ls", reinterpret_cast<INT_PTR>(hInst), path.c_str());
            }
        } else if (path.starts_with(L"control ")) {
            std::wstring params = path.substr(8);
            if (!asAdmin && ShellExecuteInExplorer(L"control.exe", params, L"")) {
                launched = true;
            } else if (HINSTANCE hInst = ShellExecuteW(nullptr, asAdmin ? L"runas" : L"open", L"control.exe", params.c_str(), nullptr, SW_SHOWNORMAL);
                       reinterpret_cast<INT_PTR>(hInst) > 32) {
                Wh_Log(L"launched control applet: %ls", path.c_str());
                launched = true;
            }
        }
    }

    // 1. If path is a real file on disk or executable in PATH (e.g. C:\Windows\System32\cmd.exe, ncpa.cpl, services.msc, cleanmgr.exe):
    bool isFileOnDisk = false;
    std::wstring extraParams;
    if (!launched && !path.empty() && path.find(L"http") != 0 && !path.starts_with(L"ms-settings:")) {
        DWORD attr = GetFileAttributesW(path.c_str());
        if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
            isFileOnDisk = true;
        } else {
            // Try resolving via PATH / System32
            wchar_t resolved[MAX_PATH] = {};
            if (SearchPathW(nullptr, path.c_str(), nullptr, MAX_PATH, resolved, nullptr) > 0) {
                path = resolved;
                isFileOnDisk = true;
            } else {
                // Check if path has arguments (e.g. "rundll32.exe sysdm.cpl,EditEnvironmentVariables" or "explorer.exe shell:::{...}")
                size_t spacePos = path.find(L' ');
                if (spacePos != std::wstring::npos) {
                    std::wstring exePart = path.substr(0, spacePos);
                    std::wstring paramPart = path.substr(spacePos + 1);
                    if (SearchPathW(nullptr, exePart.c_str(), nullptr, MAX_PATH, resolved, nullptr) > 0) {
                        path = resolved;
                        extraParams = paramPart;
                        isFileOnDisk = true;
                    }
                }
            }
        }
    }

    if (isFileOnDisk) {
        std::wstring params;
        if (lowerPath.ends_with(L"cmd.exe")) {
            params = L"/k cd /d \"" + std::wstring(userProfile) + L"\"";
        } else if (lowerPath.find(L"powershell.exe") != std::wstring::npos || lowerPath.ends_with(L"pwsh.exe")) {
            params = L"-NoExit -Command \"Set-Location '" + std::wstring(userProfile) + L"'\"";
        }
        if (!extraParams.empty()) {
            if (!params.empty()) params += L" ";
            params += extraParams;
        }

        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        // If asAdmin, do NOT suppress UI (UAC elevation prompt must show).
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        if (!params.empty()) {
            info.lpParameters = params.c_str();
        }
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if ((!asAdmin && ShellExecuteInExplorer(path, params, workDir ? workDir : L"")) || ShellExecuteExW(&info)) {
            Wh_Log(L"launched app (by file path): %ls (admin=%d, dir=%ls)", name.c_str(), asAdmin ? 1 : 0, workDir ? workDir : L"(null)");
            launched = true;
        } else {
            Wh_Log(L"app file launch failed (%lu): %ls, trying fallback", GetLastError(), path.c_str());
        }
    }

    // 2. If not launched yet and we have a PIDL:
    if (!launched && pidl) {
        if (asAdmin) {
            // Try IContextMenu verb "runas"
            IShellItem* item = nullptr;
            if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) && item) {
                IContextMenu* menu = nullptr;
                if (SUCCEEDED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&menu))) && menu) {
                    CMINVOKECOMMANDINFOEX ici{};
                    ici.cbSize = sizeof(ici);
                    ici.fMask = CMIC_MASK_UNICODE;
                    ici.lpVerb = "runas";
                    ici.lpVerbW = L"runas";
                    ici.lpDirectoryW = workDir;
                    ici.nShow = SW_SHOWNORMAL;
                    HRESULT hr = menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&ici));
                    menu->Release();
                    item->Release();
                    if (SUCCEEDED(hr)) {
                        Wh_Log(L"launched app (by IContextMenu runas): %ls", name.c_str());
                        launched = true;
                    } else {
                        Wh_Log(L"IContextMenu runas failed (%08X) for %ls", static_cast<unsigned>(hr), name.c_str());
                    }
                } else if (item) {
                    item->Release();
                }
            }

            if (!launched) {
                // Fallback: ShellExecuteExW with SEE_MASK_IDLIST and "runas"
                SHELLEXECUTEINFOW info{};
                info.cbSize = sizeof(info);
                info.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC;
                info.lpIDList = pidl;
                info.lpVerb = L"runas";
                info.lpDirectory = workDir;
                info.nShow = SW_SHOWNORMAL;
                if (ShellExecuteExW(&info)) {
                    Wh_Log(L"launched app (by PIDL runas): %ls", name.c_str());
                    launched = true;
                } else {
                    Wh_Log(L"app PIDL runas failed (%lu): %ls", GetLastError(), name.c_str());
                }
            }
        }
        
        // Fallback to normal launch if runas failed or was not requested:
        if (!launched && !asAdmin) {
            std::wstring appPath = AppsFolderPath(pidl);
            if (!appPath.empty() && ShellExecuteInExplorer(appPath, L"", L"")) {
                Wh_Log(L"launched app (by app entry): %ls", name.c_str());
                launched = true;
            }
        }
        if (!launched) {
            SHELLEXECUTEINFOW info{};
            info.cbSize = sizeof(info);
            info.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
            info.lpIDList = pidl;
            info.lpVerb = L"open";
            info.lpDirectory = workDir;
            info.nShow = SW_SHOWNORMAL;
            if (ShellExecuteExW(&info)) {
                Wh_Log(L"launched app (by PIDL open): %ls", name.c_str());
                launched = true;
            } else {
                Wh_Log(L"app PIDL open failed (%lu): %ls", GetLastError(), name.c_str());
            }
        }
    }

    // 3. Fallback direct ShellExecuteExW for any remaining commands without PIDL:
    if (!launched && !path.empty()) {
        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if (ShellExecuteExW(&info)) {
            Wh_Log(L"launched app (by direct fallback): %ls (admin=%d)", path.c_str(), asAdmin ? 1 : 0);
            launched = true;
        } else {
            Wh_Log(L"app direct fallback launch failed (%lu): %ls", GetLastError(), path.c_str());
        }
    }

    if (SUCCEEDED(comHr)) {
        CoUninitialize();
    }
}

// ---------------------------------------------------------------------------
// Calculator and unit converter, with no command: whatever is typed that is a
// sum (2^10), a number with a unit (100 km), a conversion (100 km to mi) or a
// number in another base (0xFF, 255 hex) is worked out above the apps -- and
// only that: a number alone, or one in a file name ("2024 report", "3d"), is
// left to the search.
//
// It knows units itself (kUnits), so nothing has to be set up: any unit
// converts to any other of its kind, through the kind's base unit (base =
// value * factor + offset), both ways. "100 km" shows the kind's usual units,
// and first all of them on one line; "100 km to mi" (in, ->, =) just that
// one. The settings add units it does not know (unitConversions): one formula
// each, and back again when asked (reciprocal) and the formula is linear.
// Numbers are read with "." or "," as the decimal point and shown with the
// user's.
// ---------------------------------------------------------------------------
namespace calc {

struct Unit {
    const wchar_t* symbol;  // as shown
    const wchar_t* kind;
    double factor;  // to the kind's base unit
    double offset;
    bool usual;  // among those shown when no target is asked for
    const wchar_t* names;  // lowercase, '|'-separated; the symbol is one too
};

// KB, MB, GB, TB count in 1024s, as File Explorer does.
inline constexpr Unit kUnits[] = {
    {L"mm", L"Length", 0.001, 0, true, L"millimeter|millimeters|millimetre|millimetres"},
    {L"cm", L"Length", 0.01, 0, true, L"centimeter|centimeters|centimetre|centimetres"},
    {L"m", L"Length", 1, 0, true, L"meter|meters|metre|metres"},
    {L"km", L"Length", 1000, 0, true, L"kilometer|kilometers|kilometre|kilometres"},
    {L"in", L"Length", 0.0254, 0, true, L"inch|inches|\""},
    {L"ft", L"Length", 0.3048, 0, true, L"foot|feet|'"},
    {L"yd", L"Length", 0.9144, 0, false, L"yard|yards"},
    {L"mi", L"Length", 1609.344, 0, true, L"mile|miles"},
    {L"nmi", L"Length", 1852, 0, false, L"nautical mile|nautical miles"},

    {L"mg", L"Mass", 1e-6, 0, false, L"milligram|milligrams"},
    {L"g", L"Mass", 0.001, 0, true, L"gram|grams|gramme|grammes"},
    {L"kg", L"Mass", 1, 0, true, L"kilo|kilos|kilogram|kilograms"},
    {L"t", L"Mass", 1000, 0, false, L"tonne|tonnes|ton|tons"},
    {L"oz", L"Mass", 0.028349523125, 0, true, L"ounce|ounces"},
    {L"lb", L"Mass", 0.45359237, 0, true, L"lbs|pound|pounds"},
    {L"st", L"Mass", 6.35029318, 0, false, L"stone|stones"},

    {L"\u00B0C", L"Temperature", 1, 273.15, true, L"c|\u2103|celsius"},
    {L"\u00B0F", L"Temperature", 5.0 / 9, 459.67 * 5.0 / 9, true, L"f|\u2109|fahrenheit"},
    {L"K", L"Temperature", 1, 0, true, L"kelvin|kelvins"},

    {L"m/s", L"Speed", 1, 0, true, L"mps"},
    {L"km/h", L"Speed", 1 / 3.6, 0, true, L"kmh|kph|kmph"},
    {L"mph", L"Speed", 0.44704, 0, true, L"mi/h"},
    {L"kn", L"Speed", 1852.0 / 3600, 0, true, L"kt|knot|knots"},
    {L"ft/s", L"Speed", 0.3048, 0, false, L""},

    {L"mm\u00B2", L"Area", 1e-6, 0, false, L"mm2|sq mm"},
    {L"cm\u00B2", L"Area", 1e-4, 0, true, L"cm2|sq cm"},
    {L"m\u00B2", L"Area", 1, 0, true, L"m2|sq m|sqm"},
    {L"km\u00B2", L"Area", 1e6, 0, true, L"km2|sq km"},
    {L"a", L"Area", 100, 0, false, L"are|ares"},
    {L"ha", L"Area", 1e4, 0, true, L"hectare|hectares"},
    {L"ac", L"Area", 4046.8564224, 0, true, L"acre|acres"},
    {L"in\u00B2", L"Area", 0.00064516, 0, false, L"in2|sq in"},
    {L"ft\u00B2", L"Area", 0.09290304, 0, true, L"ft2|sq ft|sqft"},
    {L"yd\u00B2", L"Area", 0.83612736, 0, false, L"yd2|sq yd"},
    {L"mi\u00B2", L"Area", 2589988.110336, 0, false, L"mi2|sq mi"},

    {L"ml", L"Volume", 0.001, 0, true, L"milliliter|milliliters|millilitre|millilitres|cc|cm\u00B3|cm3"},
    {L"l", L"Volume", 1, 0, true, L"liter|liters|litre|litres"},
    {L"m\u00B3", L"Volume", 1000, 0, true, L"m3|cubic meter|cubic meters|cubic metre|cubic metres"},
    {L"gal", L"Volume", 3.785411784, 0, true, L"gallon|gallons"},
    {L"qt", L"Volume", 0.946352946, 0, false, L"quart|quarts"},
    {L"pt", L"Volume", 0.473176473, 0, false, L"pint|pints"},
    {L"cup", L"Volume", 0.2365882365, 0, true, L"cups"},
    {L"fl oz", L"Volume", 0.0295735295625, 0, true, L"floz|fluid ounce|fluid ounces"},
    {L"tbsp", L"Volume", 0.01478676478125, 0, false, L"tablespoon|tablespoons"},
    {L"tsp", L"Volume", 0.00492892159375, 0, false, L"teaspoon|teaspoons"},

    {L"bit", L"Data", 0.125, 0, false, L"bits"},
    {L"B", L"Data", 1, 0, true, L"byte|bytes"},
    {L"KB", L"Data", 1024, 0, true, L"kib|kilobyte|kilobytes"},
    {L"MB", L"Data", 1048576, 0, true, L"mib|megabyte|megabytes"},
    {L"GB", L"Data", 1073741824, 0, true, L"gib|gigabyte|gigabytes"},
    {L"TB", L"Data", 1099511627776, 0, true, L"tib|terabyte|terabytes"},
    {L"PB", L"Data", 1125899906842624, 0, false, L"pib|petabyte|petabytes"},
    {L"Kbit", L"Data", 125, 0, false, L"kbits|kilobit|kilobits"},
    {L"Mbit", L"Data", 125000, 0, false, L"mbits|megabit|megabits"},
    {L"Gbit", L"Data", 125000000, 0, false, L"gbits|gigabit|gigabits"},

    {L"KB/s", L"Data rate", 1024, 0, false, L""},
    {L"MB/s", L"Data rate", 1048576, 0, true, L""},
    {L"Mbps", L"Data rate", 125000, 0, true, L"mbit/s"},
    {L"Gbps", L"Data rate", 125000000, 0, true, L"gbit/s"},

    {L"ms", L"Time", 0.001, 0, false, L"millisecond|milliseconds"},
    {L"s", L"Time", 1, 0, true, L"sec|secs|second|seconds"},
    {L"min", L"Time", 60, 0, true, L"mins|minute|minutes"},
    {L"h", L"Time", 3600, 0, true, L"hr|hrs|hour|hours"},
    {L"d", L"Time", 86400, 0, true, L"day|days"},
    {L"wk", L"Time", 604800, 0, true, L"week|weeks"},
    {L"yr", L"Time", 31557600, 0, false, L"year|years"},

    {L"Pa", L"Pressure", 1, 0, false, L"pascal|pascals"},
    {L"kPa", L"Pressure", 1000, 0, true, L""},
    {L"MPa", L"Pressure", 1e6, 0, false, L""},
    {L"bar", L"Pressure", 1e5, 0, true, L"bars"},
    {L"atm", L"Pressure", 101325, 0, true, L"atmosphere|atmospheres"},
    {L"psi", L"Pressure", 6894.757293168, 0, true, L""},
    {L"mmHg", L"Pressure", 133.322387415, 0, true, L"torr"},

    {L"J", L"Energy", 1, 0, true, L"joule|joules"},
    {L"kJ", L"Energy", 1000, 0, true, L""},
    {L"cal", L"Energy", 4.184, 0, false, L"calorie|calories"},
    {L"kcal", L"Energy", 4184, 0, true, L""},
    {L"Wh", L"Energy", 3600, 0, false, L""},
    {L"kWh", L"Energy", 3.6e6, 0, true, L""},

    {L"W", L"Power", 1, 0, true, L"watt|watts"},
    {L"kW", L"Power", 1000, 0, true, L"kilowatt|kilowatts"},
    {L"hp", L"Power", 745.69987158227022, 0, true, L"horsepower"},

    {L"\u00B0", L"Angle", 1, 0, true, L"deg|degree|degrees"},
    {L"rad", L"Angle", 57.29577951308232, 0, true, L"radian|radians"},
    {L"grad", L"Angle", 0.9, 0, false, L"gon|gradian|gradians"},
    {L"turn", L"Angle", 360, 0, false, L"turns|rev"},

    {L"Hz", L"Frequency", 1, 0, true, L"hertz"},
    {L"kHz", L"Frequency", 1e3, 0, true, L""},
    {L"MHz", L"Frequency", 1e6, 0, true, L""},
    {L"GHz", L"Frequency", 1e9, 0, true, L""},
};

// One color per kind, on the row's icon; a kind from the settings gets one
// of the same by its name.
inline uint32_t KindColor(const std::wstring& kind) {
    static const std::pair<const wchar_t*, uint32_t> kColors[] = {
        {L"Length", 0xFF4FA3F7},    {L"Mass", 0xFFF2994A},      {L"Temperature", 0xFFEB5757},
        {L"Speed", 0xFFBB6BD9},     {L"Area", 0xFF27AE60},      {L"Volume", 0xFF2DB7B7},
        {L"Data", 0xFF6C7BF2},      {L"Data rate", 0xFF9B8CF2}, {L"Time", 0xFFE2B93B},
        {L"Pressure", 0xFFB08968},  {L"Energy", 0xFFF2C94C},    {L"Power", 0xFFEF7AB0},
        {L"Angle", 0xFF9AA5B1},     {L"Frequency", 0xFF56CCF2}, {L"Base Radix", 0xFF8FA0B5},
    };
    for (const auto& [name, color] : kColors) {
        if (kind == name) {
            return color;
        }
    }
    uint32_t hash = 2166136261u;
    for (wchar_t ch : kind) {
        hash = (hash ^ static_cast<uint32_t>(towlower(ch))) * 16777619u;
    }
    return kColors[hash % std::size(kColors)].second;
}

// Lowercase, any script.
inline std::wstring Lower(std::wstring text) {
    if (!text.empty()) {
        CharLowerBuffW(text.data(), static_cast<DWORD>(text.size()));
    }
    return text;
}

// A unit by its symbol or any of its names, whatever the case, the spaces or
// a closing period ("min.").
inline const Unit* FindUnit(const std::wstring& name) {
    std::wstring key;
    for (wchar_t ch : Lower(tools::Trim(name))) {
        if (ch == L' ' && (key.empty() || key.back() == L' ')) {
            continue;
        }
        key += ch;
    }
    for (int pass = 0; pass < 2 && !key.empty(); ++pass) {
        for (const Unit& unit : kUnits) {
            if (key == Lower(unit.symbol)) {
                return &unit;
            }
            for (const wchar_t* name = unit.names; *name;) {
                const wchar_t* end = wcschr(name, L'|');
                const size_t length = end ? static_cast<size_t>(end - name) : wcslen(name);
                if (key.size() == length && key.compare(0, length, name, length) == 0) {
                    return &unit;
                }
                if (!end) {
                    break;
                }
                name = end + 1;
            }
        }
        if (key.back() != L'.') {
            break;
        }
        key.pop_back();
    }
    return nullptr;
}

// The user's decimal point.
inline wchar_t DecimalPoint() {
    wchar_t point[4] = {};
    return GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, point, 4) > 0 && point[0] ? point[0] : L'.';
}

// A number at the start of text: "." or "," as the decimal point (in
// "1,000.5" the last one is), thousands grouped by spaces ("1 000"), the
// other mark ("1,000" in English) or several of one ("1.000.000"), an
// exponent ("1e3"). How many characters it took, or 0.
inline size_t ParseNumber(const std::wstring& text, double* value) {
    size_t i = 0;
    std::wstring span;  // digits and marks, spaces left out
    if (i < text.size() && (text[i] == L'-' || text[i] == L'+')) {
        span += text[i++];
    }
    auto threeDigits = [&](size_t at) {
        return at + 3 <= text.size() && iswdigit(text[at]) && iswdigit(text[at + 1]) && iswdigit(text[at + 2]) &&
               (at + 3 == text.size() || !iswdigit(text[at + 3]));
    };
    bool digits = false;
    while (i < text.size()) {
        const wchar_t ch = text[i];
        if (ch >= L'0' && ch <= L'9') {
            span += ch;
            digits = true;
            ++i;
        } else if ((ch == L'.' || ch == L',') && i + 1 < text.size() && iswdigit(text[i + 1])) {
            span += ch;
            ++i;
        } else if ((ch == L' ' || ch == 0xA0 || ch == 0x202F || ch == L'\'') && digits && threeDigits(i + 1)) {
            ++i;  // a thousands group
        } else {
            break;
        }
    }
    if (!digits) {
        return 0;
    }
    // Which mark is the decimal point.
    const size_t lastDot = span.rfind(L'.'), lastComma = span.rfind(L',');
    wchar_t point = 0;
    if (lastDot != std::wstring::npos && lastComma != std::wstring::npos) {
        point = lastDot > lastComma ? L'.' : L',';
    } else if (lastDot != std::wstring::npos || lastComma != std::wstring::npos) {
        const wchar_t mark = lastDot != std::wstring::npos ? L'.' : L',';
        const size_t at = span.rfind(mark);
        const bool several = span.find(mark) != at;
        const bool groupLike = span.size() - at - 1 == 3 && mark != DecimalPoint();
        point = several || groupLike ? 0 : mark;
    }
    std::string ascii;
    for (wchar_t ch : span) {
        if (ch == point) {
            ascii += '.';
        } else if (ch != L'.' && ch != L',') {
            ascii += static_cast<char>(ch);
        }
    }
    // An exponent: e, a sign, digits.
    if (i < text.size() && (text[i] == L'e' || text[i] == L'E')) {
        size_t j = i + 1;
        std::string exponent = "e";
        if (j < text.size() && (text[j] == L'-' || text[j] == L'+')) {
            exponent += static_cast<char>(text[j++]);
        }
        const size_t digitsAt = j;
        while (j < text.size() && iswdigit(text[j])) {
            exponent += static_cast<char>(text[j++]);
        }
        if (j > digitsAt) {
            ascii += exponent;
            i = j;
        }
    }
    *value = strtod(ascii.c_str(), nullptr);
    return i;
}

// Scientific notation, as 9.9e4: up to 6 significant digits.
inline std::wstring Scientific(double value) {
    if (value == 0 || std::isnan(value) || std::isinf(value)) {
        return value == 0 ? L"0" : L"Error";
    }
    int exponent = static_cast<int>(std::floor(std::log10(std::fabs(value))));
    double mantissa = value / std::pow(10.0, exponent);
    wchar_t text[64];
    swprintf_s(text, L"%.6g", mantissa);
    if (std::fabs(wcstod(text, nullptr)) >= 10) {  // rounded up to 10
        mantissa /= 10;
        ++exponent;
        swprintf_s(text, L"%.6g", mantissa);
    }
    std::wstring result = text;
    std::replace(result.begin(), result.end(), L'.', DecimalPoint());
    return result + L"e" + std::to_wstring(exponent);
}

// A result as people write it: whole numbers whole, others to 7 significant
// digits, the user's decimal point; scientific when too large or small for
// that.
inline std::wstring Format(double value) {
    if (std::isnan(value) || std::isinf(value)) {
        return L"Error";
    }
    const double magnitude = std::fabs(value);
    if (magnitude != 0 && (magnitude >= 1e15 || magnitude < 1e-4)) {
        return Scientific(value);
    }
    wchar_t text[64];
    if (value == std::round(value)) {
        swprintf_s(text, L"%.0f", value);
    } else {
        swprintf_s(text, L"%.7g", value);
    }
    std::wstring result = text;
    std::replace(result.begin(), result.end(), L'.', DecimalPoint());
    return result;
}

// Worth showing in scientific notation too: large or small, and not
// already shown that way (Format).
inline bool Large(double value) {
    const double magnitude = std::fabs(value);
    return (magnitude >= 1e4 || (magnitude != 0 && magnitude < 1e-3)) && Format(value) != Scientific(value);
}

inline Row MakeRow(std::wstring title, std::wstring subtitle, std::wstring copy, const wchar_t* glyph,
                   uint32_t color = 0) {
    Row r;
    r.title = std::move(title);
    r.subtitle = std::move(subtitle);
    r.copyText = std::move(copy);
    r.customGlyph = glyph;
    r.glyphColor = color;
    r.canRunAsAdmin = false;
    return r;
}

inline constexpr wchar_t kCalcGlyph[] = L"\uE1D0";
inline constexpr wchar_t kConvertGlyph[] = L"\uE88E";
inline constexpr wchar_t kDot[] = L" \u2022 ";

// Whether a unit typed is a unit from the settings: by its name, by what it
// stands for (NormalizeUnit), or as the same unit of kUnits ("kilometre",
// "km").
inline bool SameUnit(const std::wstring& typed, const std::wstring& configured) {
    if (Lower(tools::Trim(typed)) == Lower(tools::Trim(configured)) ||
        tools::NormalizeUnit(typed) == tools::NormalizeUnit(configured)) {
        return true;
    }
    const Unit* unit = FindUnit(typed);
    return unit && unit == FindUnit(configured);
}

// A conversion from the settings that kUnits does already: one of those the
// earlier default settings had, left as it was in someone's settings. One
// changed -- a decimal MB to GB, say -- is the user's, and stays.
inline bool Known(const CustomConversion& c) {
    static const wchar_t* const kOldDefaults[][3] = {
        {L"km", L"miles", L"x*0.621371"}, {L"c", L"\u00B0f", L"x*9/5+32"}, {L"kg", L"lbs", L"x*2.20462"},
        {L"m", L"feet", L"x*3.28084"},    {L"cm", L"in", L"x/2.54"},      {L"mb", L"gb", L"x/1024"},
    };
    std::wstring formula;
    for (wchar_t ch : Lower(c.formula)) {
        if (ch != L' ') {
            formula += ch;
        }
    }
    const std::wstring from = Lower(tools::Trim(c.fromUnit)), to = Lower(tools::Trim(c.toUnit));
    for (const auto& old : kOldDefaults) {
        if (from == old[0] && to == old[1] && formula == old[2]) {
            return true;
        }
    }
    return false;
}

// Runs a formula from the settings backwards: possible when it is linear,
// as a * x + b, which three points tell.
inline bool Invert(const std::wstring& formula, double y, double* x) {
    double f0 = 0, f1 = 0, f2 = 0;
    if (!tools::EvaluateConversionFormula(formula, 0, f0) || !tools::EvaluateConversionFormula(formula, 1, f1) ||
        !tools::EvaluateConversionFormula(formula, 2, f2)) {
        return false;
    }
    const double a = f1 - f0;
    if (a == 0 || std::fabs(f2 - (f0 + 2 * a)) > 1e-9 * std::max(1.0, std::fabs(f2))) {
        return false;
    }
    *x = (y - f0) / a;
    return true;
}

// The settings' conversions of a number with the unit typed: from it, and
// back to it where reciprocal.
inline void CustomRows(double number, const std::wstring& unit, const std::vector<CustomConversion>& customs,
                       std::vector<Row>* rows) {
    for (const CustomConversion& c : customs) {
        if (Known(c)) {
            continue;
        }
        double out = 0;
        if (SameUnit(unit, c.fromUnit) && tools::EvaluateConversionFormula(c.formula, number, out)) {
            const std::wstring result = Format(out) + L" " + c.toUnit;
            rows->push_back(MakeRow(Format(number) + L" " + c.fromUnit + L" = " + result,
                                    c.category + kDot + L"Press Enter to copy " + result, result, kConvertGlyph,
                                    KindColor(c.category)));
        }
        if (c.reciprocal && SameUnit(unit, c.toUnit) && Invert(c.formula, number, &out)) {
            const std::wstring result = Format(out) + L" " + c.fromUnit;
            rows->push_back(MakeRow(Format(number) + L" " + c.toUnit + L" = " + result,
                                    c.category + kDot + L"Press Enter to copy " + result, result, kConvertGlyph,
                                    KindColor(c.category)));
        }
    }
}

// Groups of `size` digits from the right, as 111 1111 1111.
inline std::wstring Grouped(const std::wstring& digits, size_t size) {
    std::wstring out;
    for (size_t i = 0; i < digits.size(); ++i) {
        if (i && (digits.size() - i) % size == 0) {
            out += L' ';
        }
        out += digits[i];
    }
    return out;
}

// A whole number in other bases: 2047 = 0x7FF = 0b111 1111 1111 = 0o3777.
inline void RadixRows(double number, std::vector<Row>* rows) {
    if (number < 0 || number > 9007199254740992.0 || number != std::floor(number)) {
        return;
    }
    const unsigned long long n = static_cast<unsigned long long>(number);
    wchar_t hex[32], oct[32];
    swprintf_s(hex, L"%llX", n);
    swprintf_s(oct, L"%llo", n);
    std::wstring bin;
    for (unsigned long long v = n; v || bin.empty(); v >>= 1) {
        bin.insert(bin.begin(), (v & 1) ? L'1' : L'0');
    }
    const std::wstring hexText = std::wstring(L"0x") + (wcslen(hex) > 8 ? Grouped(hex, 4) : hex);
    rows->push_back(MakeRow(Format(number) + L" = " + hexText + L" (Hex) = 0b" + Grouped(bin, 4) + L" (Bin) = 0o" + oct +
                                L" (Oct)",
                            std::wstring(L"Base Radix") + kDot + L"Press Enter to copy 0x" + hex,
                            std::wstring(L"0x") + hex, kConvertGlyph, KindColor(L"Base Radix")));
}

// A number written in another base: 0xFF, 0b1010, 0o17.
inline bool ParseBased(const std::wstring& text, double* value) {
    if (text.size() < 3 || text[0] != L'0') {
        return false;
    }
    const wchar_t mark = static_cast<wchar_t>(towlower(text[1]));
    const int radix = mark == L'x' ? 16 : mark == L'b' ? 2 : mark == L'o' ? 8 : 0;
    if (!radix) {
        return false;
    }
    unsigned long long n = 0;
    for (size_t i = 2; i < text.size(); ++i) {
        const wchar_t ch = static_cast<wchar_t>(towlower(text[i]));
        const int digit = iswdigit(ch) ? ch - L'0' : (ch >= L'a' && ch <= L'f') ? ch - L'a' + 10 : 99;
        if (digit >= radix || n > (1ULL << 53)) {
            return false;
        }
        n = n * radix + digit;
    }
    *value = static_cast<double>(n);
    return true;
}

// Asks for other bases: "hex", "to binary", "in oct".
inline bool BaseWords(std::wstring text) {
    text = Lower(tools::Trim(text));
    for (const wchar_t* lead : {L"to ", L"in ", L"as "}) {
        if (text.starts_with(lead)) {
            text = tools::Trim(text.substr(wcslen(lead)));
        }
    }
    for (const wchar_t* word : {L"hex", L"hexadecimal", L"bin", L"binary", L"oct", L"octal", L"base", L"bases", L"radix"}) {
        if (text == word) {
            return true;
        }
    }
    return false;
}

// A number with a unit, and maybe a target: "100 km", "100 km to mi".
inline bool ConversionRows(double number, const std::wstring& text, const std::vector<CustomConversion>& customs,
                           std::vector<Row>* rows) {
    const Unit* from = nullptr;
    const Unit* to = nullptr;
    std::wstring unitText = text;
    const std::wstring lower = Lower(text);
    for (const wchar_t* separator : {L" to ", L" in ", L" into ", L" as ", L"->", L"\u2192", L"="}) {
        for (size_t at = lower.find(separator); at != std::wstring::npos && !to;
             at = lower.find(separator, at + 1)) {
            const Unit* left = FindUnit(text.substr(0, at));
            const Unit* right = FindUnit(text.substr(at + wcslen(separator)));
            if (left && right && wcscmp(left->kind, right->kind) == 0) {
                from = left;
                to = right;
                unitText = tools::Trim(text.substr(0, at));
            }
        }
    }
    if (!from) {
        from = FindUnit(text);
    }
    const size_t before = rows->size();
    if (from) {
        const double base = number * from->factor + from->offset;
        const std::wstring source = Format(number) + L" " + from->symbol;
        const uint32_t color = KindColor(from->kind);
        std::vector<std::wstring> results;
        for (const Unit& unit : kUnits) {
            if (to ? &unit != to : (!unit.usual || &unit == from || wcscmp(unit.kind, from->kind) != 0)) {
                continue;
            }
            results.push_back(Format((base - unit.offset) / unit.factor) + L" " + unit.symbol);
        }
        if (results.size() > 1) {
            // All of them on one line first.
            std::wstring all = source;
            for (const std::wstring& result : results) {
                all += L" = " + result;
            }
            rows->push_back(MakeRow(all, from->kind + std::wstring(kDot) + L"Press Enter to copy this line", all,
                                    kConvertGlyph, color));
        }
        for (const std::wstring& result : results) {
            rows->push_back(MakeRow(source + L" = " + result,
                                    from->kind + std::wstring(kDot) + L"Press Enter to copy " + result, result,
                                    kConvertGlyph, color));
        }
    }
    if (!to) {
        CustomRows(number, unitText, customs, rows);
    }
    return rows->size() > before;
}

// Not a sum, though it would work out as one: a name, as of a file -- a
// resolution (1920x1080), a date (2024-01-01, 01.02.2024), a span of years
// (2024-2025).
inline bool LooksLikeName(const std::wstring& text) {
    for (size_t i = 1; i + 1 < text.size(); ++i) {
        if ((text[i] == L'x' || text[i] == L'X') && iswdigit(text[i - 1]) && iswdigit(text[i + 1])) {
            return true;
        }
    }
    if (text.find_first_not_of(L"0123456789-./") != std::wstring::npos) {
        return false;
    }
    const size_t marks = std::count_if(text.begin(), text.end(), [](wchar_t ch) { return ch == L'-' || ch == L'.' || ch == L'/'; });
    if (marks >= 2) {
        return true;
    }
    const size_t dash = text.find(L'-');
    return marks == 1 && dash == 4 && (text.size() - dash - 1 == 2 || text.size() - dash - 1 == 4);
}

// The rows for what was typed, when it is something to work out; none when
// it is not.
inline std::vector<Row> Rows(const std::wstring& input) {
    std::vector<Row> rows;
    const std::wstring arg = tools::Trim(input);
    if (arg.find_first_of(L"0123456789") == std::wstring::npos) {
        return rows;  // "e", "pi": words, not sums
    }
    double number = 0;
    if (ParseBased(arg, &number)) {
        RadixRows(number, &rows);
        return rows;
    }
    std::vector<CustomConversion> customs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        customs = g_settings.unitConversions;
    }
    const size_t used = ParseNumber(arg, &number);
    if (used && used < arg.size()) {
        const std::wstring rest = tools::Trim(arg.substr(used));
        if (BaseWords(rest)) {
            RadixRows(number, &rows);
            return rows;
        }
        // A one-letter unit right after the number is a name: 3d, 4k, 5g.
        const bool spaced = arg[used] == L' ';
        if ((spaced || rest.size() > 1) && ConversionRows(number, rest, customs, &rows)) {
            return rows;
        }
    }
    double value = 0;
    if (used < arg.size() && !LooksLikeName(arg) && tools::EvaluateMath(arg, value)) {  // a sum, not a number alone
        rows.push_back(MakeRow(L"= " + Format(value), arg + kDot + L"Press Enter to copy result", Format(value),
                               kCalcGlyph));
        if (Large(value)) {
            rows.push_back(MakeRow(L"= " + Scientific(value),
                                   std::wstring(L"Scientific notation") + kDot + L"Press Enter to copy " +
                                       Scientific(value),
                                   Scientific(value), kCalcGlyph));
        }
    }
    return rows;
}

}  // namespace calc

void SearchThreadMain() {
    // COM for the apps index: it enumerates shell:AppsFolder.
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    apps::Index appIndex;
    apps::UsageCounts appUsage = apps::LoadUsage();
    // The counts to rank by, or null while Learn Favorite Apps is off.
    auto learnedUsage = [&appUsage]() -> apps::UsageCounts* {
        if (g_forgetUsage.exchange(false)) {
            appUsage.clear();
        }
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        return g_settings.learnFavorites ? &appUsage : nullptr;
    };
    if (appIndex.Rebuild()) {
        g_lastAppIndexRebuildTick.store(GetTickCount64());
        Wh_Log(L"apps: indexed (%zu apps)", appIndex.Count());
    } else {
        Wh_Log(L"apps: index failed");
    }

    int iconPixels = IconPixels();
    icons::FileIconCache iconCache(iconPixels);

    // The apps behind the rows currently on screen, in the same order.
    std::vector<const apps::App*> lastHits;

    // One fetch per app, ever. Keyed by name because that is what identifies
    // an entry across index rebuilds.
    std::map<std::wstring, std::vector<BYTE>> appIconCache;

    // An app's icon is the slow part of showing it the first time -- about
    // 10 ms each through the shell, 20-50 ms for a keystroke that brings up a
    // few new ones -- so they are fetched ahead, while there is nothing to
    // search, one at a time so a keystroke never waits for more than one.
    // Walks the index once, and again after a rebuild.
    size_t prefetchNext = 0;
    auto prefetchIcon = [&] {
        const apps::App* app = appIndex.At(prefetchNext++);
        if (!app || appIconCache.count(app->name)) {
            return;
        }
        std::vector<BYTE> pixels;
        if (!app->isSetting || app->pidl) {
            FetchAppIcon(app, iconPixels, &pixels);
        }
        appIconCache.emplace(app->name, std::move(pixels));
    };

    everything::Client client;
    if (!client.Init()) {
        Wh_Log(L"search: could not create the reply window");
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
        return;
    }
    Wh_Log(L"search: ready (Everything %ls)",
        everything::FindIpcWindow() ? L"found" : L"NOT running");

    std::wstring last;
    while (!g_searchQuit.load()) {
        std::wstring query;
        {
            std::unique_lock<std::mutex> lock(g_queryMutex);
            auto hasWork = [] {
                return g_queryDirty.load() || g_searchQuit.load() || (g_launchRequest.load() >= 0) || g_appIndexNeedsRefresh.load();
            };
            if (!hasWork() && (iconCache.HasPending() || prefetchNext < appIndex.Count())) {
                lock.unlock();
                // Files first: they are on screen now.
                if (iconCache.FetchPending()) {
                    if (!iconCache.HasPending()) {
                        PublishLateFileIcons(iconCache.TakeLate());
                    }
                } else {
                    prefetchIcon();
                }
                continue;
            }
            g_queryWake.wait(lock, hasWork);
            if (g_searchQuit.load()) {
                if (SUCCEEDED(comHr)) {
                    CoUninitialize();
                }
                return;
            }
            int wanted = g_launchRequest.exchange(-1);
            if (wanted >= 0) {
                bool asAdmin = g_launchAsAdmin.exchange(false);
                if (static_cast<size_t>(wanted) < lastHits.size() && lastHits[wanted]) {
                    const auto* app = lastHits[wanted];
                    apps::UsageCounts* usage = learnedUsage();
                    if (usage) {
                        int& uses = (*usage)[app->nameLower];
                        uses = std::min(uses + 1, apps::kUsageCap);
                    }
                    std::wstring name = app->name;
                    std::wstring targetPath = app->targetPath;
                    ITEMIDLIST* pidlClone = app->pidl ? ILClone(app->pidl.get()) : nullptr;
                    lock.unlock();
                    if (usage) {
                        apps::SaveUsage(*usage);  // this thread's own: no lock needed
                    }

                    SpawnTrackedLaunch([name = std::move(name), targetPath = std::move(targetPath), pidlClone, asAdmin] {
                        WaitForLaunchGate();
                        LaunchAppAsync(name, targetPath, pidlClone, asAdmin);
                        g_launchesDone.fetch_add(1);
                        if (pidlClone) {
                            ILFree(pidlClone);
                        }
                    });
                } else {
                    lock.unlock();
                }
                continue;
            }

            bool shouldRebuild = g_appIndexNeedsRefresh.exchange(false);
            if (shouldRebuild) {
                lock.unlock();
                g_lastAppIndexRebuildTick.store(GetTickCount64());
                bool rebuilt = appIndex.Rebuild();
                lock.lock();
                if (rebuilt) {
                    lastHits.clear();
                    prefetchNext = 0;
                    g_launchRequest.store(-1);
                    last.clear();
                    g_queryDirty.store(true);
                    Wh_Log(L"apps: dynamically refreshed (%zu apps)", appIndex.Count());
                }
            }

            if (!g_queryDirty.exchange(false)) {
                continue;
            }
            query = g_pendingQuery;
        }

        // Optional settle (searchDebounceMs, off by default). Not needed to keep
        // up with typing: this thread takes the latest text each time round,
        // so keystrokes that land during a search are folded into the next
        // one. A delay here would only be added to every keystroke -- 25 ms of
        // the ~110 it took, when that was the default. A fresh query from
        // empty never waits, so results are ready before the overlay reveals.
        int debounceMs = 0;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            debounceMs = g_settings.searchDebounceMs;
        }
        if (!last.empty() && debounceMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(debounceMs));
            std::lock_guard<std::mutex> lock(g_queryMutex);
            if (g_pendingQuery != query) {
                g_queryDirty.store(true);
                continue;
            }
        }

        if (query == last) {
            continue;
        }
        last = query;

        // Start may be on a display with another scale by now.
        if (int pixels = IconPixels(); pixels != iconPixels) {
            iconPixels = pixels;
            iconCache.SetSize(pixels);
            appIconCache.clear();
            prefetchNext = 0;
        }

        if (query.empty()) {
            lastHits.clear();
            g_launchRequest.store(-1);
            {
                std::lock_guard<std::mutex> lock(g_resultsMutex);
                g_appRows.clear();
                g_fileRows.clear();
                g_rowsQuery.clear();
                g_totalMatches.store(0);
            }
            RequestRender();
            continue;
        }


        std::wstring qTrim = tools::Trim(query);
        std::wstring qLower = tools::ToLower(qTrim);

        bool isIpCommand = (qLower == L"/ip" || qLower.starts_with(L"/ip "));
        bool isExplicitWeb = query.starts_with(L"?");
        std::wstring webQuery;
        ResolvedWebQuery explicitWeb;
        if (isExplicitWeb) {
            webQuery = query.substr(1);
            while (!webQuery.empty() && webQuery.front() == L' ') {
                webQuery.erase(0, 1);
            }
            explicitWeb = ResolveWebSearch(webQuery);
        }

        std::vector<everything::Result> pool;
        DWORD total = 0;
        long long ms = 0;

        int maxFiles = 12;
        int maxApps = 6;
        bool filterNoise = true;
        std::vector<std::wstring> excludedPaths;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            maxFiles = g_settings.maxFileResults;
            maxApps = g_settings.maxAppResults;
            filterNoise = g_settings.filterNoisyPaths;
            excludedPaths = g_settings.excludedPaths;
        }
        static const std::vector<std::wstring> s_disabledNoise;
        const std::vector<std::wstring>* noisePtr = filterNoise ? &excludedPaths : &s_disabledNoise;

        // Queries and ranks the files. The query as typed is the fast one and
        // is usually enough: Rank takes the excluded paths out of what it
        // returns. Only when that leaves too few clean results while
        // Everything has more matches than it sent is the query run again
        // with the excluded paths left out at the source (WithExclusions) --
        // it costs more, and most queries never need it. Noise still shows
        // when it is all there is.
        auto queryFiles = [&](const std::wstring& text) {
            const size_t limit = static_cast<size_t>(maxFiles);
            if (!client.Query(text, ranker::kDefaultPool, &pool, &total)) {
                return false;
            }
            size_t clean = ranker::Rank(&pool, text, limit, noisePtr);
            if (clean >= limit || total <= ranker::kDefaultPool) {
                return true;
            }
            const std::wstring filtered = ranker::WithExclusions(text, noisePtr);
            if (filtered == text) {
                return true;
            }
            std::vector<everything::Result> more;
            DWORD moreTotal = 0;
            if (client.Query(filtered, ranker::kDefaultPool, &more, &moreTotal) &&
                ranker::Rank(&more, text, limit, noisePtr) > clean) {
                pool = std::move(more);
                total = moreTotal;
            }
            return true;
        };

        if (isIpCommand) {
            pool.clear();
            total = 0;
            ms = 0;
        } else if (isExplicitWeb) {
            if (!explicitWeb.queryTerm.empty()) {
                auto start = std::chrono::steady_clock::now();
                queryFiles(explicitWeb.queryTerm);
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now() - start)
                         .count();
            }
        } else {
            auto start = std::chrono::steady_clock::now();
            bool ok = queryFiles(query);
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - start)
                          .count();
            if (!ok) {
                Wh_Log(L"search: '%ls' failed (Everything running?)", query.c_str());
                pool.clear();
                total = 0;
            }
        }

        // Apps are a name match over an index built once at startup, so this
        // costs nothing next to the file query.
        std::vector<Row> appRows;
        lastHits.clear();

        if (isIpCommand) {
            auto ifaces = tools::GetNetworkInterfaces();
            if (ifaces.empty()) {
                Row r;
                r.title = L"No Active Network Interfaces";
                r.subtitle = L"Check your Wi-Fi, Ethernet, or VPN connection";
                r.customGlyph = L"\uE701";
                r.canRunAsAdmin = false;
                r.appIndex = static_cast<int>(lastHits.size());
                lastHits.push_back(nullptr);
                appRows.push_back(std::move(r));
            } else {
                for (const auto& iface : ifaces) {
                    Row r;
                    r.title = iface.ip;
                    std::wstring sub = iface.typeLabel + L": " + iface.adapterName;
                    if (!iface.mask.empty()) sub += L" \u2022 Subnet: " + iface.mask;
                    if (!iface.gateway.empty()) sub += L" \u2022 GW: " + iface.gateway;
                    r.subtitle = sub + L" \u2022 Press Enter to copy";
                    r.copyText = iface.ip;
                    r.customGlyph = iface.glyph;
                    r.canRunAsAdmin = false;
                    r.appIndex = static_cast<int>(lastHits.size());
                    lastHits.push_back(nullptr);
                    appRows.push_back(std::move(r));
                }
            }
        } else if (isExplicitWeb) {
            Row webRow;
            if (explicitWeb.queryTerm.empty()) {
                if (explicitWeb.isShortcut) {
                    webRow.title = L"Open " + explicitWeb.serviceName;
                    webRow.subtitle = explicitWeb.serviceName + L" \u2022 " + explicitWeb.homeUrl;
                } else {
                    webRow.title = L"Search the web";
                    webRow.subtitle = explicitWeb.serviceName + L" Search";
                }
                webRow.openPath = explicitWeb.homeUrl;
            } else {
                webRow.title = L"Search " + explicitWeb.serviceName + L" for \"" + explicitWeb.queryTerm + L"\"";
                webRow.subtitle = explicitWeb.serviceName + L" Search \u2022 " + explicitWeb.queryTerm;
                webRow.openPath = explicitWeb.searchUrl;
            }
            webRow.canRunAsAdmin = false;
            webRow.appIndex = -1;
            appRows.push_back(std::move(webRow));
        } else {
            // A sum, a conversion or another base typed: worked out above the
            // apps (calc).
            for (Row& r : calc::Rows(qTrim)) {
                r.appIndex = static_cast<int>(lastHits.size());
                lastHits.push_back(nullptr);
                appRows.push_back(std::move(r));
            }
            for (const apps::Match& m : appIndex.Search(query, static_cast<size_t>(maxApps), learnedUsage())) {
                if (!m.app) {
                    continue;
                }
                Row row;
                row.title = m.app->name;
                bool isSettingItem = m.app->isSetting;
                row.isSetting = isSettingItem;

                bool canAdmin = true;
                bool isFileTarget = false;
                if (isSettingItem) {
                    if (!m.app->area.empty()) {
                        row.subtitle = L"Settings \u2022 " + m.app->area;
                    } else {
                        row.subtitle = L"Settings";
                    }
                    canAdmin = false;
                } else if (!m.app->targetPath.empty() && GetFileAttributesW(m.app->targetPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    row.subtitle = m.app->targetPath;
                    canAdmin = true;
                    isFileTarget = true;
                } else if (!m.app->linkTarget.empty() &&
                           GetFileAttributesW(m.app->linkTarget.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    // Shown, and used by the file actions, but not launched:
                    // the app ID keeps the shortcut's arguments and taskbar
                    // identity.
                    row.subtitle = m.app->linkTarget;
                    row.programPath = m.app->linkTarget;
                } else {
                    row.subtitle = L"Application";
                }
                row.canRunAsAdmin = canAdmin;
                row.isFile = isFileTarget;
                row.openPath = m.app->targetPath;
                row.appIndex = static_cast<int>(lastHits.size());

                auto cached = appIconCache.find(m.app->name);
                if (cached == appIconCache.end()) {
                    std::vector<BYTE> pixels;
                    if (!m.app->isSetting || m.app->pidl) {
                        FetchAppIcon(m.app, iconPixels, &pixels);
                    }
                    cached = appIconCache.emplace(m.app->name, std::move(pixels))
                                 .first;
                }
                row.icon = cached->second;

                appRows.push_back(std::move(row));
                // The App outlives this loop -- the index owns it and lives as
                // long as this thread -- so a pointer is safe here in a way it
                // would not be inside a XAML click handler.
                lastHits.push_back(m.app);
            }

        }

        std::vector<Row> fileRows;
        iconCache.NewResults(std::chrono::milliseconds(25));
        for (const everything::Result& r : pool) {
            Row row;
            row.title = r.name;
            row.subtitle = r.path;
            row.isFolder = r.isFolder;
            row.openPath = r.path;
            if (!row.openPath.empty() && row.openPath.back() != L'\\') {
                row.openPath += L'\\';
            }
            row.openPath += r.name;
            // One shell call per distinct extension, not per row, except for
            // files with an icon of their own (see FileIconCache).
            if (const std::vector<BYTE>* pixels =
                    iconCache.Get(row.openPath, r.isFolder)) {
                row.icon = *pixels;
            }
            fileRows.push_back(std::move(row));
        }

        {
            std::lock_guard<std::mutex> lock(g_resultsMutex);
            g_appRows = appRows;
            g_fileRows = fileRows;
            g_rowsQuery = query;
            g_totalMatches.store(total);
        }
        RequestRender();
        Wh_Log(L"search: '%ls' -> %u files (kept %zu), %zu apps (%lld ms)", query.c_str(),
            total, pool.size(), appRows.size(), static_cast<long long>(ms));
        for (size_t i = 0; i < appRows.size(); ++i) {
            Wh_Log(L"    [app %zu] %ls -> %ls", i, appRows[i].title.c_str(), appRows[i].subtitle.c_str());
        }
        for (size_t i = 0; i < pool.size() && i < 5; ++i) {
            Wh_Log(L"    [file %zu] %ls  %ls", i, pool[i].name.c_str(), pool[i].path.c_str());
        }
    }
}




void RecursivelyHideTextBlocks(wux::DependencyObject const& node, int depth = 0) {
    if (!node || depth > 8) return;
    try {
        if (auto tb = node.try_as<wuxc::TextBlock>()) {
            SuppressShellElement(tb);
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < count; ++i) {
            RecursivelyHideTextBlocks(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1);
        }
    } catch (...) {}
}

void HideStockPlaceholder(wux::FrameworkElement const& stock) {
    if (!stock) return;
    try {
        if (auto ph = FindDescendantByName(stock, L"PlaceholderText", 8)) {
            SuppressShellElement(ph.try_as<wux::FrameworkElement>());
        }
        if (auto caret = FindDescendantByName(stock, L"TextCaret", 8)) {
            SuppressShellElement(caret.try_as<wux::FrameworkElement>());
        }
        if (auto stb = FindDescendantByName(stock, L"SearchTextBox", 8)) {
            SuppressShellElement(stb.try_as<wux::FrameworkElement>());
        }

        // Recursively hide all TextBlocks inside the stock search button
        RecursivelyHideTextBlocks(stock, 0);
    } catch (...) {}
}

void PlaceOurSearchBox(wux::FrameworkElement const& stockButton) try {
    g_stockButton = stockButton;
    SuppressShellElement(stockButton);
    if (auto ctl = stockButton.try_as<wuxc::Control>()) {
        ctl.IsTabStop(false);
    }
    HideStockPlaceholder(stockButton);

    auto parent = wuxm::VisualTreeHelper::GetParent(stockButton);
    auto cell = parent ? parent.try_as<wuxc::Panel>() : nullptr;
    if (!cell) {
        Wh_Log(L"own box: stock button parent is not a Panel; cannot place");
        return;
    }

    auto owner = wuxm::VisualTreeHelper::GetParent(cell);
    auto ownerPanel = owner ? owner.try_as<wuxc::Panel>() : nullptr;
    if (!ownerPanel) {
        Wh_Log(L"own box: cell parent is not a Panel; cannot place");
        return;
    }

    // The shell's own search cell, zeroed so our box can take its place.
    // Recorded, because leaving a container collapsed at zero height after
    // the mod goes away is what the Start menu cannot survive.
    SuppressShellElement(cell.try_as<wux::FrameworkElement>(), /*collapse=*/true,
                         /*zeroSize=*/true);


    // Verify if g_resultsHost is valid and currently attached to this ownerPanel
    bool needsBuild = false;
    if (!g_resultsHost) {
        needsBuild = true;
    } else {
        auto currentParent = wuxm::VisualTreeHelper::GetParent(g_resultsHost);
        if (!currentParent || currentParent != ownerPanel) {
            Wh_Log(L"PlaceOurSearchBox: visual tree changed (Start Menu Styler)! Re-attaching results host.");
            if (currentParent) {
                if (auto oldPanel = currentParent.try_as<wuxc::Panel>()) {
                    uint32_t idx = 0;
                    if (oldPanel.Children().IndexOf(g_resultsHost, idx)) {
                        oldPanel.Children().RemoveAt(idx);
                    }
                }
            }
            needsBuild = true;
        }
    }

    if (needsBuild) {
        if (g_revealAnim) { g_revealAnim.Stop(); g_revealAnim = nullptr; }
        if (g_hideAnim) { g_hideAnim.Stop(); g_hideAnim = nullptr; }
        g_resultsTranslate = nullptr;
        g_resultsHost = nullptr;
        g_ourBox = nullptr;
        g_appsList = nullptr;
        g_resultsList = nullptr;
        if (g_activeAppsOpt) g_activeAppsOpt->clear();
        if (g_appButtonsOpt) g_appButtonsOpt->clear();
        if (g_fileButtonsOpt) g_fileButtonsOpt->clear();
        g_appsHeaderHolder = nullptr;
        g_filesHeaderHolder = nullptr;
        g_searchBarBorder = nullptr;
        g_divider = nullptr;
        g_footerBorder = nullptr;
        BuildResultsList(ownerPanel);
    }

    // Proactively clean any other search boxes across the tree
    try {
        wux::DependencyObject node = cell;
        wux::DependencyObject menuRoot = cell;
        for (int up = 0; up < 12; ++up) {
            auto parentNode = wuxm::VisualTreeHelper::GetParent(node);
            if (!parentNode) break;
            menuRoot = parentNode;
            node = parentNode;
        }
        HideAllOtherSearchBoxes(menuRoot);
        DisarmScrollTabStops(menuRoot);
    } catch (...) {}

    SubclassStartMenuWindow();

    if (!g_coreEventsHooked) {
        try {
            if (auto window = wux::Window::Current()) {
                if (auto core = window.CoreWindow()) {
                    if (!g_charReceivedToken) {
                        g_charReceivedToken = core.CharacterReceived([](wuc::CoreWindow const&, wuc::CharacterReceivedEventArgs const& args) {
                            try {
                                unsigned code = args.KeyCode();
                                if (ProcessKeyChar(static_cast<wchar_t>(code))) {
                                    args.Handled(true);
                                }
                            } catch (...) {}
                        });
                    }

                    if (!g_keyDownToken) {
                        g_keyDownToken = core.KeyDown([](wuc::CoreWindow const&, wuc::KeyEventArgs const& args) {
                            try {
                                if (!g_ourBox) return;
                                auto key = args.VirtualKey();

                                if (key == winrt::Windows::System::VirtualKey::Escape && !EscapeBelongsToContextMenu()) {
                                    if (g_isOverlayVisible.load() || g_isHiding.load()) {
                                        if (g_ourBox) g_ourBox.Text(L"");
                                        HideOverlayAnimated();
                                        args.Handled(true);
                                        return;
                                    }
                                }

                                if (IsNavigationKey(key) && g_isOverlayVisible.load()) {
                                    bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
                                    if (HandleNavigationKey(key, ctrl)) {
                                        args.Handled(true);
                                        return;
                                    }
                                }

                                auto focused = wux::Input::FocusManager::GetFocusedElement();
                                if (focused && focused.try_as<wuxc::TextBox>()) {
                                    return;
                                }

                                std::wstring text{g_ourBox.Text()};
                                if (text.empty()) return;

                                if (key == winrt::Windows::System::VirtualKey::Back) {
                                    text.pop_back();
                                    g_ourBox.Text(text);
                                    if (text.empty()) {
                                        HideOverlayAnimated();
                                    } else {
                                        RevealOverlayAnimated();
                                        g_ourBox.Focus(wux::FocusState::Programmatic);
                                        g_ourBox.SelectionStart(static_cast<int32_t>(text.size()));
                                    }
                                    args.Handled(true);
                                }
                            } catch (...) {}
                        });
                    }

                    if (!g_activatedToken) {
                        g_activatedToken = core.Activated([](wuc::CoreWindow const&, wuc::WindowActivatedEventArgs const& args) {
                            if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated &&
                                GetTickCount64() - g_launchedAtTick.load() < kLaunchHandoffMs) {
                                // Taken back from SearchHost to launch (DismissStartMenuForLaunch).
                            } else if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated) {
                                g_suppressRefocus.store(false);
                                if (g_ourBox && !g_isOverlayVisible.load()) {
                                    g_ourBox.Text(L"");
                                }
                                if (g_resultsHost && !g_isOverlayVisible.load()) {
                                    g_resultsHost.Visibility(wux::Visibility::Visible);
                                    g_resultsHost.Opacity(0.0);
                                    g_resultsHost.IsHitTestVisible(false);
                                    if (g_resultsTranslate) g_resultsTranslate.Y(-8.0);
                                    SyncOverlayBackground();
                                }
                                RestoreSwappedOutText();
                                TriggerMenuOpenFocus();
                            } else if (SearchHostTookForegroundFromOpenStart()) {
                                // Taken back by the subclass (WM_ACTIVATE); nothing to reset.
                            } else if (g_dragging) {
                                // Explorer's helper running a drag (StartFileDrag).
                            } else {
                                StashTextIfSwappedOut();
                                g_suppressRefocus.store(true);
                                if (g_resultsHost) {
                                    g_resultsHost.Visibility(wux::Visibility::Visible);
                                    g_resultsHost.Opacity(0.0);
                                    g_resultsHost.IsHitTestVisible(false);
                                    if (g_resultsTranslate) g_resultsTranslate.Y(-8.0);
                                }
                                if (g_ourBox) {
                                    g_ourBox.Text(L"");
                                }
                                g_isOverlayVisible.store(false);
                                g_isHiding.store(false);
                                if (g_hideAnim) g_hideAnim.Stop();
                                if (g_revealAnim) g_revealAnim.Stop();
                                if (g_openFocus) {
                                    g_openFocus.Stop();
                                    g_openFocus = nullptr;
                                }
                            }
                        });
                    }
                    if (!g_visibilityToken) {
                        g_visibilityToken = core.VisibilityChanged(
                            [](wuc::CoreWindow const&, wuc::VisibilityChangedEventArgs const& args) {
                                Wh_Log(L"focus: VisibilityChanged visible=%d", args.Visible() ? 1 : 0);
                                if (args.Visible()) {
                                    TakeForegroundWhenShown();
                                }
                            });
                    }
                    g_coreEventsHooked = true;
                }
            }
        } catch (...) {}
    }

    TriggerMenuOpenFocus();
} catch (...) {
    Wh_Log(L"PlaceOurSearchBox error: %08X", static_cast<unsigned>(winrt::to_hresult()));
}

void TeardownStartMenuUi() {
    Wh_Log(L"teardown: tearing down Start Menu UI on XAML thread");


    // Here rather than in Wh_ModUninit: on the hooked thread, the hook
    // procedure can't be mid-call when it goes.
    if (g_hGetMsgHook) {
        UnhookWindowsHookEx(g_hGetMsgHook);
        g_hGetMsgHook = nullptr;
    }

    // Before anything is dropped: see g_xamlHandlers.
    try {
        g_xamlHandlers.reset();
    } catch (...) {}

    try {
        HidePreview();
        if (g_previewResize) {
            g_previewResize.Stop();
        }
        g_previewResize = nullptr;
        g_previewResizeAnimation = nullptr;
        g_previewTimer = nullptr;
        g_previewFrameTimer = nullptr;
        g_previewVideo = nullptr;
        g_previewText = nullptr;
        g_previewPopup = nullptr;
        g_previewCard = nullptr;
        g_previewThumbBox = nullptr;
        g_previewImage = nullptr;
        g_previewName = nullptr;
        g_previewFacts = nullptr;
        g_previewPath = nullptr;
        if (g_previewCopied) {
            g_previewCopied.Stop();
        }
        g_previewCopied = nullptr;
    } catch (...) {}

    try {
        if (HWND core = g_hCoreWindow.load(); core && g_subclassed) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(core, StartMenuSubclassProc);
            g_subclassed = false;
        }
    } catch (...) {}

    if (g_openFocus) {
        g_openFocus.Stop();
        g_openFocus = nullptr;
    }
    if (g_shownFocus) {
        g_shownFocus.Stop();
        g_shownFocus = nullptr;
    }
    if (g_revealAnim) {
        g_revealAnim.Stop();
        g_revealAnim = nullptr;
    }
    if (g_hideAnim) {
        g_hideAnim.Stop();
        g_hideAnim = nullptr;
    }

    try {
        auto core = wuc::CoreWindow::GetForCurrentThread();
        if (core) {
            if (g_charReceivedToken) {
                core.CharacterReceived(g_charReceivedToken);
                g_charReceivedToken = {};
            }
            if (g_keyDownToken) {
                core.KeyDown(g_keyDownToken);
                g_keyDownToken = {};
            }
            if (g_activatedToken) {
                core.Activated(g_activatedToken);
                g_activatedToken = {};
            }
            if (g_visibilityToken) {
                core.VisibilityChanged(g_visibilityToken);
                g_visibilityToken = {};
            }
        }
    } catch (...) {}
    g_coreEventsHooked = false;

    try {
        g_ourBoxChanged.revoke();
        g_ourBoxLost.revoke();
    } catch (...) {}

    try {
        if (g_resultsList) {
            g_resultsList.Children().Clear();
        }
        if (g_appsList) {
            g_appsList.Children().Clear();
        }
        if (g_resultsHost) {
            g_resultsHost.Visibility(wux::Visibility::Collapsed);
            g_resultsHost.Opacity(0.0);
            g_resultsHost.IsHitTestVisible(false);
            auto parent = wuxm::VisualTreeHelper::GetParent(g_resultsHost);
            if (parent) {
                if (auto parentPanel = parent.try_as<wuxc::Panel>()) {
                    uint32_t idx = 0;
                    if (parentPanel.Children().IndexOf(g_resultsHost, idx)) {
                        parentPanel.Children().RemoveAt(idx);
                    }
                }
            }
        }
    } catch (...) {}

    g_activeAppsOpt.reset();
    g_appButtonsOpt.reset();
    g_fileButtonsOpt.reset();
    g_resultsTranslate = nullptr;
    g_resultsHost = nullptr;
    g_ourBox = nullptr;
    g_appsList = nullptr;
    g_resultsList = nullptr;
    g_appsHeaderHolder = nullptr;
    g_filesHeaderHolder = nullptr;
    g_searchBarBorder = nullptr;
    g_divider = nullptr;
    g_footerBorder = nullptr;
    g_footerStatus = nullptr;
    g_footerHints = nullptr;
    g_currentAppRows.clear();
    g_currentFileRows.clear();
    g_isOverlayVisible.store(false);
    g_isHiding.store(false);
    g_stockButton = nullptr;

    RestoreShellElements();
    Wh_Log(L"teardown: Start Menu UI teardown complete");
}

}  // namespace

// ===========================================================================
// Mod entry points
// ===========================================================================



BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_targetProcess = IdentifyCurrentProcess();

    if (g_targetProcess == TargetProcess::Explorer) {
        InitExplorer();
        return TRUE;
    }
    if (g_targetProcess == TargetProcess::SearchHost) {
        InitSearchHost();
        return TRUE;
    }
    if (g_targetProcess == TargetProcess::StartMenu) {
        LoadSettings();
        return TRUE;
    }
    return FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"=== attached to pid %lu (%ls) ===", GetCurrentProcessId(),
        g_targetProcess == TargetProcess::StartMenu ? L"StartMenu" :
        g_targetProcess == TargetProcess::SearchHost ? L"SearchHost" :
        g_targetProcess == TargetProcess::Explorer ? L"Explorer" : L"Unknown");

    if (g_targetProcess == TargetProcess::SearchHost) {
        StartSearchHostWatchdog();
        return;
    }

    if (g_targetProcess != TargetProcess::StartMenu) {
        return;
    }

    g_searchQuit.store(false);
    g_searchThread.emplace(SearchThreadMain);

    StartAttachWatch();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_targetProcess != TargetProcess::StartMenu) {
        return;
    }
    LoadSettings();
    RequestAppIndexRefresh();
    if (g_resultsHost) {
        try {
            g_resultsHost.Dispatcher().RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                []() {
                    if (g_footerHints) {
                        bool show = true;
                        {
                            std::lock_guard<std::mutex> lock(g_settingsMutex);
                            show = g_settings.showKeyHints;
                        }
                        g_footerHints.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
                    }
                    SyncOverlayBackground();  // the panel margin
                    // The text size, on all that is built.
                    if (g_ourBox) {
                        ScaleFont(g_ourBox, 14);
                    }
                    if (g_searchBarBorder) {
                        g_searchBarBorder.Height(Fs(40));
                    }
                    RescaleFonts(g_resultsHost);
                    RescaleFonts(g_previewCard);
                });
        } catch (...) {}
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (HWND core = g_hCoreWindow.load()) {
        RemovePropW(core, L"WindhawkStartMenuWindow");
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        RemovePropW(tray, L"WindhawkStartMenuHwnd");
    }

    g_quit.store(true);

    if (g_targetProcess == TargetProcess::SearchHost) {
        Wh_Log(L"uninit: SearchHost cleaning up");
        if (g_searchHostWatchdog && g_searchHostWatchdog->joinable()) {
            g_searchHostWatchdog->join();
            g_searchHostWatchdog.reset();
        }
        // Put SearchHost back as the shell left it: no subclass, and no zero
        // alpha -- left behind, it would keep native search invisible after
        // the mod is disabled.
        for (HWND hwnd : g_searchWindows) {
            if (IsWindow(hwnd)) {
                SetSearchWindowNeutralized(hwnd, FALSE);  // through the subclass: first
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc);
            }
        }
        g_searchWindows.clear();
        Wh_Log(L"uninit: SearchHost cleanup complete");
        return;
    }

    if (g_targetProcess != TargetProcess::StartMenu) {
        if (g_targetProcess == TargetProcess::Explorer) {
            StopExplorerHelperHost();
        }
        Wh_Log(L"uninit: explorer.exe unhook complete");
        return;
    }

    StopAttachWatch();

    {
        std::lock_guard<std::mutex> lock(g_queryMutex);
        g_searchQuit.store(true);
    }
    g_queryWake.notify_all();
    if (g_searchThread && g_searchThread->joinable()) {
        g_searchThread->join();
        g_searchThread.reset();
    }
    // Before the teardown is queued, so whatever it dispatched runs first.
    StopPreviewThread();
    WaitForTrackedLaunches();

    bool tornDown = false;
    // Only the dispatcher: a reference to the box itself would be released
    // here, off the XAML thread, after the teardown dropped the others. The
    // stock button is kept from before the box is placed, so a half-done
    // attach (stock elements hidden, no box) is still undone on its thread.
    wuc::CoreDispatcher dispatcher{nullptr};
    try {
        if (g_ourBox) {
            dispatcher = g_ourBox.Dispatcher();
        } else if (g_stockButton) {
            dispatcher = g_stockButton.Dispatcher();
        }
    } catch (...) {}
    if (dispatcher) {
        try {
            auto op = dispatcher.RunAsync(
                wuc::CoreDispatcherPriority::Low,
                wuc::DispatchedHandler{[] { TeardownStartMenuUi(); }});
            if (op.wait_for(std::chrono::seconds(5)) == wf::AsyncStatus::Completed) {
                tornDown = true;
            } else {
                op.Cancel();
            }
        } catch (...) {}
    }
    if (!tornDown) {
        HWND hCore = g_hCoreWindow;
        if (hCore && IsWindow(hCore)) {
            DWORD_PTR result = 0;
            LRESULT lr = SendMessageTimeoutW(hCore, GetTeardownMessage(), 0, 0,
                                             SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &result);
            if (lr != 0 && static_cast<LRESULT>(result) == kTeardownDone) {
                tornDown = true;
            }
        }
    }
    if (!tornDown) {
        try {
            TeardownStartMenuUi();
        } catch (...) {}
    }
    // A click before the teardown may have started a launch.
    WaitForTrackedLaunches();
    if (g_launchGate) {
        CloseHandle(g_launchGate);
        g_launchGate = nullptr;
    }
    Wh_Log(L"uninit: StartMenuExperienceHost teardown complete");
}
