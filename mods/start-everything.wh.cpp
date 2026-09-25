// ==WindhawkMod==
// @id              start-everything
// @name            Everything & Power Tools in the Start Menu
// @description     Native Everything search inside the Start menu, complete SearchHost disconnection, and seamless focus management.
// @version         1.0
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

A high-performance, native replacement for Windows 11 Start Menu search powered directly by voidtools Everything. Completely severs SearchHost background telemetry, Bing web queries, and Edge WebView2 processes, replacing them with instantaneous sub-millisecond local file, application, and settings search directly inside the Start Menu.

![Everything & Power Tools in the Start Menu](https://raw.githubusercontent.com/bardelyne/start-everything/main/screenshot.png)

## Key Features

- Instant Everything Search: Sub-millisecond file querying directly through the voidtools Everything Win32 IPC interface. Instant results across millions of files without background indexing lag or disk thrashing.
- Smart Apps and Settings Search: Fuzzy matching across Desktop applications, Microsoft Store / UWP packages, Control Panel applets, and Windows Settings URIs (ms-settings:) with high-resolution shell icons.
- On-Demand Animated Palette: The Start Menu stays completely clean and uncluttered when idle. The search palette smoothly reveals with a 140ms ease-out animation the moment you type or click the top search trigger, and collapses on empty or Escape.
- Complete SearchHost Disconnection: Prevents background Bing web queries, Edge WebView2 child processes, and indexing CPU spikes via process, database file, and COM interception.
- Inline Calculator: Type /c <expression> (e.g. /c 100 * 5, /c sqrt(144), /c 15% of 200, /c 2^10) to evaluate math expressions instantly. Press Enter to copy the result.
- Configurable Unit Conversions: Type /c <number> [unit] to convert units using formulas configured in Mod Settings. Users can add, edit, or delete conversion items individually from the settings UI.
- Network Interface Inspector: Type /ip to display all active Wi-Fi, Ethernet, and VPN network interfaces with their IP addresses, subnet masks, gateways, and hardware descriptions. Press Enter to copy the IP.
- Full Right-Click Context Menu: Right-click any file, folder, or application to Open, Run as Administrator, Open in terminal (folders), Properties, Create desktop shortcut, Cut/Copy (files), Copy path, or Open file location.
- Explorer Shell Property Relay: Seamlessly bridges the AppContainer isolation boundary to display native Win32 properties dialogs hosted directly by explorer.exe.
- Explicit Web Search: Trigger web searches on demand using the '?' prefix (e.g. '?query'). Includes customizable keyword shortcuts such as '?yt' (YouTube), '?gh' (GitHub), '?w' (Wikipedia), and '?r' (Reddit).
- Start Menu Styler Compatibility: Automatically syncs background styles (Tinted Glass, Acrylic, custom theme colors) in real time without restarting the mod.
- Robust Win32 Key Listener: Combines a WH_GETMESSAGE UI thread hook, HWND subclassing, and XAML CoreWindow handling to ensure zero dropped keystrokes.
- Shell Focus Protection: Intercepts explorer.exe foreground redirection to prevent SearchHost from stealing focus away from the Start Menu.

## Requirements

1. Windows 11 (version 22H2+ x86-64).
2. voidtools Everything (version 1.4 or 1.5a) running in the background.

## Recommended Setup: Hide Taskbar Search

In Windows 11, clicking or typing into the taskbar search box opens the standalone SearchHost flyout rather than the Start Menu. Because this mod replaces Start Menu search and disconnects SearchHost background queries, it is strongly recommended to hide the search icon/box from your taskbar:
1. Right-click the Taskbar and select Taskbar settings (or Settings > Personalization > Taskbar).
2. Under Taskbar items, set Search to Hide.
All searches will now seamlessly route through the native Start Menu (Windows Key or Start button).

## Keyboard Shortcuts

- Type any key: Automatically reveals the search palette, focuses the search box, and queries apps and files.
- Up / Down: Navigate through application, calculation, conversion, and file results.
- Enter: Launch the selected application, copy calculation/conversion/IP result, or open item.
- Ctrl + Enter: Run the selected application or file as Administrator (triggers UAC).
- Escape: Clear the current query and smoothly collapse the search palette back to pinned apps.
- Right-Click: Context menu with Open, Run as Administrator, Open in terminal, Properties, Create desktop shortcut, Cut/Copy (files), Copy path, and Open file location.

Note on Pinning: Windows 11 blocks programmatic pinning to the Taskbar or Start Menu. Use 'Create desktop shortcut' first, then right-click the shortcut on your desktop and select 'Pin to Taskbar' or 'Pin to Start'.

## Command Reference

- /c <expression>: Calculate math expression (e.g. /c 100 * 5, /c sqrt(144), /c 15% of 200).
- /c <number>: Display all configured unit conversions and programmer radix (Hex, Bin, Oct).
- /c <number> <unit>: Targeted unit conversion (e.g. /c 100 km, /c 32 c, /c 50 lbs).
- /ip: List all active network interfaces and IP addresses.
- ?<term>: Web search using default search engine.
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
- showKeyHints: true
  $name: Show Keyboard Shortcuts Bar
  $description: Display the keyboard shortcut hints ([Up/Down] Select, [Enter] Open, [Ctrl+Enter] Admin, [Esc] Close) in the bottom bar.
- filterNoisyPaths: true
  $name: Demote Noisy Paths
  $description: Automatically demote deep build caches, version control internals, and temporary directories to the bottom of file search results.
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
    Paths matching any of these substrings will be demoted in file search results so build caches, dependencies, and internal system folders don't clutter the top matches.
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
- unitConversions:
    - - fromUnit: "km"
        $name: Source Unit
        $description: Trigger unit (e.g. km)
      - toUnit: "miles"
        $name: Target Unit
      - formula: "x * 0.621371"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Distance"
        $name: Category
    - - fromUnit: "c"
        $name: Source Unit
        $description: Trigger unit (e.g. c)
      - toUnit: "°F"
        $name: Target Unit
      - formula: "x * 9 / 5 + 32"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Temperature"
        $name: Category
    - - fromUnit: "kg"
        $name: Source Unit
        $description: Trigger unit (e.g. kg)
      - toUnit: "lbs"
        $name: Target Unit
      - formula: "x * 2.20462"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Weight"
        $name: Category
    - - fromUnit: "m"
        $name: Source Unit
        $description: Trigger unit (e.g. m)
      - toUnit: "feet"
        $name: Target Unit
      - formula: "x * 3.28084"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Length"
        $name: Category
    - - fromUnit: "cm"
        $name: Source Unit
        $description: Trigger unit (e.g. cm)
      - toUnit: "in"
        $name: Target Unit
      - formula: "x / 2.54"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Length"
        $name: Category
    - - fromUnit: "mb"
        $name: Source Unit
        $description: Trigger unit (e.g. mb)
      - toUnit: "GB"
        $name: Target Unit
      - formula: "x / 1024"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "Storage"
        $name: Category
  $name: Custom Unit Conversions
  $description: >-
    Configurable unit conversions for /c <number> [unit].
    Formulas evaluate using 'x' as input. You can add, edit, or remove items individually at any time.
*/
// ==/WindhawkModSettings==

#include <initguid.h>  // must precede xamlom.h

#include <inspectable.h>
#include <xamlom.h>

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
#include <shobjidl.h>
#include <shlobj.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <limits>

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

// Is Everything running and listening?
inline HWND FindIpcWindow() {
    return FindWindowW(kIpcWindowClass, nullptr);
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
    Client() = default;
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    ~Client() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        if (atom_) {
            UnregisterClassW(kReplyClass, GetCurrentModuleHandle());
        }
    }

    bool Init() {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &Client::WndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = kReplyClass;
        atom_ = RegisterClassExW(&wc);
        if (!atom_) {
            return false;
        }
        hwnd_ = CreateWindowExW(0, kReplyClass, L"", WS_POPUP, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, wc.hInstance, this);
        if (!hwnd_) {
            return false;
        }

        // Everything answers by sending WM_COPYDATA back to this window, and
        // UIPI silently drops messages sent from a lower integrity level to a
        // higher one. Everything runs at medium, so this only bites when the
        // broker is elevated -- but when it does, the failure is invisible:
        // the query is accepted, Everything runs the search, and the reply is
        // discarded, which is indistinguishable from a timeout. Measured on
        // this machine: elevated without this call, zero replies in 2 s; with
        // it, the same query answers in about 30 ms.
        ChangeWindowMessageFilterEx(hwnd_, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
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
    static constexpr DWORD kReplyId = 0x45565251;   // EVRQ
    static constexpr DWORD kReplyId2 = 0x45565232;  // EVR2

    void Reset() {
        results_.clear();
        total_ = 0;
        replied_ = false;
    }

    bool SendQuery2(HWND everything, const std::wstring& text,
                    DWORD maxResults, DWORD requestFlags, DWORD sortType) {
        Reset();
        expecting_ = kReplyId2;
        std::vector<BYTE> buffer(sizeof(Query2HeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<Query2HeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = kReplyId2;
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
        Reset();
        expecting_ = kReplyId;
        std::vector<BYTE> buffer(sizeof(QueryHeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<QueryHeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = kReplyId;
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
                if (cds->dwData == kReplyId2) {
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

// Results out of package caches, build outputs and version-control internals
// swamp everything else on a developer machine: the first unranked page for
// "code" was eight Gradle doc stubs. Demoted a whole match class rather than
// hidden, so they still show up once the better matches run out.
inline bool IsNoise(const std::wstring& pathLower, const std::vector<std::wstring>* customNoise = nullptr) {
    if (!customNoise) return false;
    for (const auto& n : *customNoise) {
        if (!n.empty() && pathLower.find(n) != std::wstring::npos) {
            return true;
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

// Reorders a pool in place and truncates it to limit.
inline void Rank(std::vector<everything::Result>* pool,
                 const std::wstring& query, size_t limit,
                 const std::vector<std::wstring>* excludedPaths = nullptr) {
    if (!pool || pool->empty()) {
        return;
    }
    const std::wstring q = ToLower(query);

    std::vector<detail::Key> keys;
    keys.reserve(pool->size());
    for (size_t i = 0; i < pool->size(); i++) {
        const everything::Result& r = (*pool)[i];
        int cls = detail::Classify(ToLower(r.name), q);
        // A noisy location costs a whole class, so an exact name match buried
        // in node_modules still loses to a plain prefix match somewhere real.
        if (detail::IsNoise(ToLower(r.path), excludedPaths)) {
            cls += detail::kPathOnly + 1;
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

    std::vector<everything::Result> ranked;
    ranked.reserve(keys.size() < limit ? keys.size() : limit);
    for (size_t i = 0; i < keys.size() && ranked.size() < limit; i++) {
        ranked.push_back(std::move((*pool)[keys[i].index]));
    }
    *pool = std::move(ranked);
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

#include <string>
#include <unordered_map>
#include <vector>

namespace icons {

// Copies a bitmap into a top-down 32bpp BGRA buffer.
inline bool BitmapToBgra(HBITMAP bitmap, int size, std::vector<BYTE>* out) {
    if (!bitmap) {
        return false;
    }
    BITMAP info{};
    if (!GetObjectW(bitmap, sizeof(info), &info)) {
        return false;
    }
    // Rescaling is not this function's job. A mismatch means the caller asked
    // the shell for one size and got another, and sending a buffer whose
    // dimensions disagree with its byte count would just be rejected at the
    // other end.
    if (info.bmWidth != size || info.bmHeight != size) {
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // negative: top-down, matching XAML
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    out->assign(static_cast<size_t>(size) * size * 4, 0);
    HDC screen = GetDC(nullptr);
    int scanned = GetDIBits(screen, bitmap, 0, size, out->data(), &bi,
                            DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    return scanned == size;
}

// Draws an icon into a 32bpp surface and copies it out. DrawIconEx is used
// rather than reading the icon's own bitmaps because it handles both modern
// 32bpp icons and the old mask-plus-colour pairs, and produces straight
// alpha either way.
inline bool IconToBgra(HICON icon, int size, std::vector<BYTE>* out) {
    if (!icon) {
        return false;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
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
    memset(bits, 0, static_cast<size_t>(size) * size * 4);
    BOOL drawn = DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
    if (drawn) {
        out->assign(static_cast<BYTE*>(bits),
                    static_cast<BYTE*>(bits) + static_cast<size_t>(size) * size * 4);
    }
    SelectObject(dc, previous);
    DeleteObject(dib);
    DeleteDC(dc);
    return drawn != FALSE;
}

// Icons for files, keyed by extension.
//
// Fetching a real icon per result is far too slow to do per keystroke -- a
// single IShellItemImageFactory::GetImage measured 38-220 ms. Almost every
// file of the same type has the same icon, though, so the shell is asked once
// per extension using SHGFI_USEFILEATTRIBUTES, which answers from the
// registered file type without touching the disk at all.
class ExtensionCache {
   public:
    explicit ExtensionCache(int size) : size_(size) {}

    // Returns BGRA pixels, or nullptr when the shell had nothing.
    const std::vector<BYTE>* Get(const std::wstring& nameOrPath,
                                 bool isFolder) {
        std::wstring key = isFolder ? L"<dir>" : ExtensionOf(nameOrPath);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second.empty() ? nullptr : &it->second;
        }

        std::vector<BYTE> pixels;
        // A name that does not exist is fine and is the point: with
        // SHGFI_USEFILEATTRIBUTES the shell answers from the extension alone.
        std::wstring probe = isFolder ? L"folder" : (L"file" + key);
        SHFILEINFOW info{};
        DWORD attributes =
            isFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;

        // Above 32px, go through the system image list rather than
        // SHGFI_LARGEICON.
        //
        // SHGFI_LARGEICON is 32 and nothing else, so asking it for a 48px
        // icon gets a 32px one stretched -- visibly soft once the row is
        // drawn on a scaled display. SHIL_EXTRALARGE is 48 and SHIL_JUMBO is
        // 256; the shell already has both, so the sharper one costs no more
        // than the blurry one did.
        bool got = false;
        if (size_ > 32) {
            if (SHGetFileInfoW(probe.c_str(), attributes, &info, sizeof(info),
                               SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX)) {
                IImageList* list = nullptr;
                int which = (size_ > 48) ? SHIL_JUMBO : SHIL_EXTRALARGE;
                if (SUCCEEDED(SHGetImageList(which, IID_PPV_ARGS(&list))) &&
                    list) {
                    HICON icon = nullptr;
                    if (SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT,
                                                &icon)) &&
                        icon) {
                        got = IconToBgra(icon, size_, &pixels);
                        DestroyIcon(icon);
                    }
                    list->Release();
                }
            }
        }

        if (!got &&
            SHGetFileInfoW(probe.c_str(), attributes, &info, sizeof(info),
                           SHGFI_USEFILEATTRIBUTES | SHGFI_ICON |
                               SHGFI_LARGEICON)) {
            IconToBgra(info.hIcon, size_, &pixels);
            DestroyIcon(info.hIcon);
        }
        auto inserted = cache_.emplace(key, std::move(pixels));
        return inserted.first->second.empty() ? nullptr
                                              : &inserted.first->second;
    }

    size_t size() const { return cache_.size(); }

   private:
    static std::wstring ExtensionOf(const std::wstring& name) {
        size_t dot = name.rfind(L'.');
        if (dot == std::wstring::npos || dot + 1 >= name.size()) {
            return L"";
        }
        std::wstring ext = name.substr(dot);
        for (wchar_t& c : ext) {
            c = static_cast<wchar_t>(towlower(c));
        }
        return ext;
    }

    int size_;
    std::unordered_map<std::wstring, std::vector<BYTE>> cache_;
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
};

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
    size_t dot = filename.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        return filename.substr(0, dot);
    }
    size_t bang = filename.find_last_of(L'!');
    if (bang != std::wstring::npos) {
        size_t underscore = filename.find_first_of(L'_');
        if (underscore != std::wstring::npos) {
            std::wstring pkg = filename.substr(0, underscore);
            size_t dotInPkg = pkg.find_last_of(L'.');
            if (dotInPkg != std::wstring::npos) {
                return pkg.substr(dotInPkg + 1);
            }
            return pkg;
        }
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

        int gaps = 0;
        if (q.size() >= 2 && isSubsequence(q, app.nameLower, gaps)) {
            s = 80 + std::min(gaps, 30);
        } else if (q.size() >= 2 && !app.exeNameLower.empty() && isSubsequence(q, app.exeNameLower, gaps)) {
            s = 85 + std::min(gaps, 30);
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

            ITEMIDLIST* pidl = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
                a.pidl.reset(pidl);
            }
            if (!a.name.empty() && a.pidl) {
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

    std::vector<Match> Search(const std::wstring& query, size_t limit) const {
        std::vector<Match> hits;
        if (query.empty()) {
            return hits;
        }
        const std::wstring q = ToLower(query);

        std::lock_guard<std::mutex> lock(mutex_);
        for (const App& a : apps_) {
            int score = ScoreApp(a, q);
            if (score >= 0) {
                hits.push_back({&a, score});
            }
        }

        std::stable_sort(hits.begin(), hits.end(),
                         [](const Match& x, const Match& y) {
                             if (x.score != y.score) {
                                 return x.score < y.score;
                             }
                             return x.app->name.size() < y.app->name.size();
                         });
        if (hits.size() > limit) {
            hits.resize(limit);
        }
        return hits;
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
inline bool ParseConversionQuery(const std::wstring& input, double& outNum, std::wstring& outUnit, bool& isHelp) {
    std::wstring t = Trim(input);
    if (t.starts_with(L"/c") || t.starts_with(L"/C")) {
        t = Trim(t.substr(2));
    } else {
        return false;
    }

    if (t.empty()) {
        isHelp = true;
        return true;
    }
    isHelp = false;

    size_t i = 0;
    if (i < t.size() && (t[i] == L'+' || t[i] == L'-')) i++;
    bool hasDigits = false;
    bool hasDot = false;
    while (i < t.size()) {
        if (iswdigit(t[i])) {
            hasDigits = true;
            i++;
        } else if (t[i] == L'.' && !hasDot) {
            hasDot = true;
            i++;
        } else {
            break;
        }
    }

    if (!hasDigits) return false;

    std::wstring numPart = t.substr(0, i);
    wchar_t* pEnd = nullptr;
    outNum = wcstod(numPart.c_str(), &pEnd);

    std::wstring rest = Trim(t.substr(i));
    outUnit = ToLower(rest);
    return true;
}

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

#pragma pop_macro("GetCurrentTime")

#include <robuffer.h>
#ifndef WH_MOD_ID
#define WH_MOD_ID L"start-everything"
#endif
#ifndef WH_MOD_VERSION
#define WH_MOD_VERSION L"1.0"
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
// Domain: explorer.exe (Shell Focus Redirection)
// ===========================================================================

using Explorer_SetForegroundWindow_t = BOOL(WINAPI*)(HWND);
static Explorer_SetForegroundWindow_t pOriginalExplorerSetForegroundWindow = nullptr;

static bool IsProcessNamed(DWORD pid, const wchar_t* name) {
    if (!pid || pid == GetCurrentProcessId()) return false;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) return false;
    wchar_t path[MAX_PATH] = {};
    DWORD size = MAX_PATH;
    bool match = false;
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
        const wchar_t* exeName = wcsrchr(path, L'\\');
        exeName = exeName ? (exeName + 1) : path;
        match = (_wcsicmp(exeName, name) == 0);
    }
    CloseHandle(hProcess);
    return match;
}

static HWND FindStartMenuCoreWindow() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        HWND h = reinterpret_cast<HWND>(GetPropW(tray, L"WindhawkStartMenuHwnd"));
        if (h && IsWindow(h)) return h;
    }

    HWND hStart = FindWindowW(L"Windows.UI.Core.CoreWindow", L"Start");
    if (hStart && IsWindow(hStart)) {
        if (tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", hStart);
        }
        return hStart;
    }

    HWND found = nullptr;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        if (GetPropW(hwnd, L"WindhawkStartMenuWindow")) {
            *reinterpret_cast<HWND*>(lParam) = hwnd;
            return FALSE;
        }
        wchar_t cls[64] = {};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (IsProcessNamed(pid, L"StartMenuExperienceHost.exe")) {
                *reinterpret_cast<HWND*>(lParam) = hwnd;
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&found));

    if (found && IsWindow(found)) {
        if (tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", found);
        }
        return found;
    }

    return nullptr;
}

static BOOL WINAPI Hook_Explorer_SetForegroundWindow(HWND hWnd) {
    if (!hWnd) {
        return pOriginalExplorerSetForegroundWindow(hWnd);
    }

    DWORD targetPid = 0;
    GetWindowThreadProcessId(hWnd, &targetPid);

    if (IsProcessNamed(targetPid, L"SearchHost.exe")) {
        Wh_Log(L"[Explorer] Blocked SetForegroundWindow to SearchHost.exe (0x%p)", hWnd);

        // Redirect shell foreground activation directly to Start Menu CoreWindow
        HWND hStart = FindStartMenuCoreWindow();
        if (hStart && IsWindow(hStart)) {
            Wh_Log(L"[Explorer] Preserving foreground on StartMenu window 0x%p", hStart);
            pOriginalExplorerSetForegroundWindow(hStart);
        }
        return TRUE; // Pretend success
    }

    return pOriginalExplorerSetForegroundWindow(hWnd);
}

using Explorer_BringWindowToTop_t = BOOL(WINAPI*)(HWND);
static Explorer_BringWindowToTop_t pOriginalExplorerBringWindowToTop = nullptr;

static BOOL WINAPI Hook_Explorer_BringWindowToTop(HWND hWnd) {
    if (!hWnd) return pOriginalExplorerBringWindowToTop(hWnd);
    DWORD targetPid = 0;
    GetWindowThreadProcessId(hWnd, &targetPid);
    if (IsProcessNamed(targetPid, L"SearchHost.exe")) {
        Wh_Log(L"[Explorer] Blocked BringWindowToTop to SearchHost.exe (0x%p)", hWnd);
        HWND hStart = FindStartMenuCoreWindow();
        if (hStart && IsWindow(hStart)) {
            pOriginalExplorerBringWindowToTop(hStart);
        }
        return TRUE;
    }
    return pOriginalExplorerBringWindowToTop(hWnd);
}

using Explorer_SwitchToThisWindow_t = void(WINAPI*)(HWND, BOOL);
static Explorer_SwitchToThisWindow_t pOriginalExplorerSwitchToThisWindow = nullptr;

static void WINAPI Hook_Explorer_SwitchToThisWindow(HWND hWnd, BOOL fAltTab) {
    if (!hWnd) return;
    DWORD targetPid = 0;
    GetWindowThreadProcessId(hWnd, &targetPid);
    if (IsProcessNamed(targetPid, L"SearchHost.exe")) {
        Wh_Log(L"[Explorer] Blocked SwitchToThisWindow to SearchHost.exe (0x%p)", hWnd);
        HWND hStart = FindStartMenuCoreWindow();
        if (hStart && IsWindow(hStart)) {
            if (pOriginalExplorerSwitchToThisWindow) {
                pOriginalExplorerSwitchToThisWindow(hStart, fAltTab);
            } else {
                SetForegroundWindow(hStart);
            }
        }
        return;
    }
    if (pOriginalExplorerSwitchToThisWindow) {
        pOriginalExplorerSwitchToThisWindow(hWnd, fAltTab);
    }
}

// Tracked launch threads for clean unload synchronization across processes
static std::mutex g_launchHandlesMutex;
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

static HANDLE g_hExplorerHelperThread = nullptr;
static DWORD g_explorerHelperThreadId = 0;
static HWND g_hExplorerHelperWnd = nullptr;
static HANDLE g_hExplorerHelperReadyEvent = nullptr;

static LRESULT CALLBACK ExplorerHelperWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COPYDATA: {
        auto pcds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
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

                AllowSetForegroundWindow(ASFW_ANY);

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

static void StartExplorerHelperHost() {
    if (g_hExplorerHelperThread) return;

    if (FindWindowW(kExplorerHelperClassName, kExplorerHelperWindowName)) {
        Wh_Log(L"[Explorer] Helper host window already active in another explorer instance");
        return;
    }

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
    WaitForTrackedLaunches();
}

void InitExplorer() {
    Wh_Log(L"=== start-everything: initializing explorer.exe shell hooks ===");
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, Hook_Explorer_SetForegroundWindow,
                                   &pOriginalExplorerSetForegroundWindow);
    WindhawkUtils::SetFunctionHook(BringWindowToTop, Hook_Explorer_BringWindowToTop,
                                   &pOriginalExplorerBringWindowToTop);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pSwitch = (Explorer_SwitchToThisWindow_t)GetProcAddress(hUser32, "SwitchToThisWindow");
        if (pSwitch) {
            WindhawkUtils::SetFunctionHook(pSwitch, Hook_Explorer_SwitchToThisWindow,
                                           &pOriginalExplorerSwitchToThisWindow);
        }
    }

    StartExplorerHelperHost();
}

// ===========================================================================
// Domain: SearchHost.exe (SearchHost Disconnect & Suppression)
// ===========================================================================

struct DisconnectRecursionGuard {
    bool& flag;
    explicit DisconnectRecursionGuard(bool& f) : flag(f) { flag = true; }
    ~DisconnectRecursionGuard() { flag = false; }
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
    DisconnectRecursionGuard guard(inHook);

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

using CreateFileW_t = decltype(&CreateFileW);
static CreateFileW_t pOriginalCreateFileW = nullptr;

static bool IsRestrictedDatabaseFile(LPCWSTR lpFileName) {
    if (!lpFileName) return false;
    std::wstring lower(lpFileName);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });

    return (lower.find(L"appsindex.db") != std::wstring::npos ||
            lower.find(L"settings.db") != std::wstring::npos ||
            lower.find(L"windows.edb") != std::wstring::npos);
}

static HANDLE WINAPI Hook_SearchHost_CreateFileW(
    LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    thread_local bool inHook = false;
    if (inHook) {
        return pOriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                                    lpSecurityAttributes, dwCreationDisposition,
                                    dwFlagsAndAttributes, hTemplateFile);
    }
    DisconnectRecursionGuard guard(inHook);

    if (IsRestrictedDatabaseFile(lpFileName)) {
        Wh_Log(L"[SearchHost] Blocked access to search database: %ls", lpFileName);
        SetLastError(ERROR_FILE_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }

    return pOriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                                lpSecurityAttributes, dwCreationDisposition,
                                dwFlagsAndAttributes, hTemplateFile);
}

using CoCreateInstance_t = decltype(&CoCreateInstance);
static CoCreateInstance_t pOriginalCoCreateInstance = nullptr;

static const GUID kSearchManager = {0x7D096C5F, 0xAC08, 0x4F1F, {0xBE, 0xB7, 0x5C, 0x22, 0xC5, 0x17, 0xCE, 0x39}};
static const GUID kCollatorUtilities = {0x9E175B8B, 0xF52A, 0x11D8, {0xB9, 0xA5, 0x50, 0x50, 0x54, 0x50, 0x30, 0x30}};
static const GUID kSearchQuery = {0x0B63E349, 0x9CCC, 0x11D0, {0xBC, 0xDB, 0x00, 0x80, 0x5F, 0xCC, 0xCE, 0x04}};
static const GUID kSearchFolder = {0x323CA680, 0xC24D, 0x4099, {0xB9, 0xD4, 0x44, 0x6D, 0xD2, 0xD7, 0x24, 0x9E}};

static HRESULT WINAPI Hook_SearchHost_CoCreateInstance(
    REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext,
    REFIID riid, LPVOID* ppv) {
    thread_local bool inHook = false;
    if (inHook) {
        return pOriginalCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    }
    DisconnectRecursionGuard guard(inHook);

    if (IsEqualGUID(rclsid, kSearchManager) ||
        IsEqualGUID(rclsid, kCollatorUtilities) ||
        IsEqualGUID(rclsid, kSearchQuery) ||
        IsEqualGUID(rclsid, kSearchFolder)) {
        Wh_Log(L"[SearchHost] Blocked Windows Search COM activation");
        if (ppv) *ppv = nullptr;
        return REGDB_E_CLASSNOTREG;
    }

    return pOriginalCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

static LRESULT CALLBACK SearchHostSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_ACTIVATE) {
        if (LOWORD(wParam) != WA_INACTIVE) {
            Wh_Log(L"[SearchHost] WM_ACTIVATE (active) on 0x%p -> redirecting foreground to StartMenu", hWnd);
            HWND hStart = FindStartMenuCoreWindow();
            if (hStart && IsWindow(hStart)) {
                SetForegroundWindow(hStart);
                BringWindowToTop(hStart);
            }
            return 0;
        }
    } else if (uMsg == WM_SETFOCUS) {
        Wh_Log(L"[SearchHost] WM_SETFOCUS on 0x%p -> redirecting foreground to StartMenu", hWnd);
        HWND hStart = FindStartMenuCoreWindow();
        if (hStart && IsWindow(hStart)) {
            SetForegroundWindow(hStart);
            BringWindowToTop(hStart);
        }
        return 0;
    } else if (uMsg == WM_WINDOWPOSCHANGING) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (wp) {
            wp->flags |= SWP_HIDEWINDOW;
            wp->flags &= ~SWP_SHOWWINDOW;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using SetForegroundWindow_t = BOOL(WINAPI*)(HWND);
static SetForegroundWindow_t pOrigSearchHostSetForegroundWindow = nullptr;
static BOOL WINAPI Hook_SearchHost_SetForegroundWindow(HWND hWnd) {
    Wh_Log(L"[SearchHost] SetForegroundWindow called for 0x%p", hWnd);
    HWND hStart = FindStartMenuCoreWindow();
    if (hStart && IsWindow(hStart)) {
        Wh_Log(L"[SearchHost] Redirecting SetForegroundWindow to StartMenu 0x%p", hStart);
        pOrigSearchHostSetForegroundWindow(hStart);
        BringWindowToTop(hStart);
    }
    return TRUE;
}

using BringWindowToTop_t = BOOL(WINAPI*)(HWND);
static BringWindowToTop_t pOrigSearchHostBringWindowToTop = nullptr;
static BOOL WINAPI Hook_SearchHost_BringWindowToTop(HWND hWnd) {
    Wh_Log(L"[SearchHost] BringWindowToTop called for 0x%p", hWnd);
    HWND hStart = FindStartMenuCoreWindow();
    if (hStart && IsWindow(hStart)) {
        pOrigSearchHostBringWindowToTop(hStart);
    }
    return TRUE;
}

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t pOrigSearchHostShowWindow = nullptr;
static BOOL WINAPI Hook_SearchHost_ShowWindow(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWDEFAULT) {
        Wh_Log(L"[SearchHost] Redirected SearchHost ShowWindow to SW_HIDE");
        nCmdShow = SW_HIDE;
    }
    return pOrigSearchHostShowWindow(hWnd, nCmdShow);
}

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t pOrigSearchHostSetWindowPos = nullptr;
static BOOL WINAPI Hook_SearchHost_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_SHOWWINDOW) {
        uFlags &= ~SWP_SHOWWINDOW;
        uFlags |= SWP_HIDEWINDOW;
    }
    return pOrigSearchHostSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using SwitchToThisWindow_t = void(WINAPI*)(HWND, BOOL);
static SwitchToThisWindow_t pOrigSearchHostSwitchToThisWindow = nullptr;
static void WINAPI Hook_SearchHost_SwitchToThisWindow(HWND hWnd, BOOL fAltTab) {
    Wh_Log(L"[SearchHost] SwitchToThisWindow called for 0x%p", hWnd);
    HWND hStart = FindStartMenuCoreWindow();
    if (hStart && IsWindow(hStart)) {
        SetForegroundWindow(hStart);
    }
}

void InitSearchHost() {
    Wh_Log(L"=== start-everything: initializing SearchHost disconnect & suppression hooks ===");
    WindhawkUtils::SetFunctionHook(CreateProcessW, Hook_SearchHost_CreateProcessW, &pOriginalCreateProcessW);
    WindhawkUtils::SetFunctionHook(CreateFileW, Hook_SearchHost_CreateFileW, &pOriginalCreateFileW);
    WindhawkUtils::SetFunctionHook(CoCreateInstance, Hook_SearchHost_CoCreateInstance, &pOriginalCoCreateInstance);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, Hook_SearchHost_SetForegroundWindow, &pOrigSearchHostSetForegroundWindow);
    WindhawkUtils::SetFunctionHook(BringWindowToTop, Hook_SearchHost_BringWindowToTop, &pOrigSearchHostBringWindowToTop);
    WindhawkUtils::SetFunctionHook(ShowWindow, Hook_SearchHost_ShowWindow, &pOrigSearchHostShowWindow);
    WindhawkUtils::SetFunctionHook(SetWindowPos, Hook_SearchHost_SetWindowPos, &pOrigSearchHostSetWindowPos);

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pSwitch = (SwitchToThisWindow_t)GetProcAddress(hUser32, "SwitchToThisWindow");
        if (pSwitch) {
            WindhawkUtils::SetFunctionHook(pSwitch, Hook_SearchHost_SwitchToThisWindow, &pOrigSearchHostSwitchToThisWindow);
        }
    }
}

[[clang::no_destroy]] static std::optional<std::thread> g_searchHostWatchdog;
static void StartSearchHostWatchdog() {
    g_searchHostWatchdog.emplace([] {
        for (int i = 0; i < 120 && !g_quit.load(); ++i) {
            EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
                DWORD pid = 0;
                GetWindowThreadProcessId(hwnd, &pid);
                if (pid == GetCurrentProcessId()) {
                    WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc, 0);
                }
                return TRUE;
            }, 0);
            for (int s = 0; s < 10 && !g_quit.load(); ++s) {
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
};

struct Settings {
    std::wstring defaultSearchUrl = L"https://duckduckgo.com/?q={q}";
    std::vector<WebShortcut> webShortcuts;
    std::vector<CustomConversion> unitConversions;
    std::vector<std::wstring> excludedPaths;
    int maxAppResults = 6;
    int maxFileResults = 12;
    bool showKeyHints = true;
    bool filterNoisyPaths = true;
};

Settings g_settings;
std::mutex g_settingsMutex;
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
        c.fromUnit = tools::ToLower(tools::Trim(fromU.get()));
        c.toUnit = (toU.get() && *toU.get()) ? tools::Trim(toU.get()) : L"";
        c.formula = (formula.get() && *formula.get()) ? tools::Trim(formula.get()) : L"";
        c.category = (cat.get() && *cat.get()) ? tools::Trim(cat.get()) : L"Conversion";

        if (!c.fromUnit.empty() && !c.toUnit.empty() && !c.formula.empty()) {
            g_settings.unitConversions.push_back(std::move(c));
        }
    }
    if (g_settings.unitConversions.empty()) {
        g_settings.unitConversions.push_back({L"km", L"miles", L"x * 0.621371", L"Distance"});
        g_settings.unitConversions.push_back({L"c", L"\u00B0F", L"x * 9 / 5 + 32", L"Temperature"});
        g_settings.unitConversions.push_back({L"kg", L"lbs", L"x * 2.20462", L"Weight"});
        g_settings.unitConversions.push_back({L"m", L"feet", L"x * 3.28084", L"Length"});
        g_settings.unitConversions.push_back({L"cm", L"in", L"x / 2.54", L"Length"});
        g_settings.unitConversions.push_back({L"mb", L"GB", L"x / 1024", L"Storage"});
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

    g_settings.showKeyHints = Wh_GetIntSetting(L"showKeyHints") != 0;

    Wh_Log(L"=== settings: defSearch=%ls shortcuts=%zu maxApps=%d maxFiles=%d hints=%d filterNoise=%d excluded=%zu ===",
        g_settings.defaultSearchUrl.c_str(), g_settings.webShortcuts.size(),
        g_settings.maxAppResults, g_settings.maxFileResults,
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
};

static std::optional<std::vector<wuxc::Button>> g_appButtonsOpt;
static std::optional<std::vector<AppCardUI>> g_activeAppsOpt;

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
void HideAllOtherSearchBoxes(wux::DependencyObject const& root, int depth = 15);
void SyncOverlayBackground();
void RequestRender();
void TeardownStartMenuUi();
inline UINT GetTeardownMessage() {
    static UINT s_msg = RegisterWindowMessageW(L"Windhawk_StartMenuTeardown_start-everything");
    return s_msg;
}

[[clang::no_destroy]] wuxc::TextBox::TextChanged_revoker g_ourBoxChanged;
[[clang::no_destroy]] wux::UIElement::LostFocus_revoker g_ourBoxLost;
[[clang::no_destroy]] wux::DispatcherTimer g_openFocus{nullptr};

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
    try {
        if (auto border = FindMenuAcrylicBorder()) {
            if (auto brush = border.Background()) {
                if (g_resultsHost.Background() != brush) {
                    g_resultsHost.Background(brush);
                    Wh_Log(L"SyncOverlayBackground: synchronized background from AcrylicBorder (%ls)",
                        winrt::get_class_name(brush).c_str());
                }
                try {
                    auto cr = border.CornerRadius();
                    g_resultsHost.CornerRadius(cr);
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

        sb.Completed([](wf::IInspectable const&, wf::IInspectable const&) {
            if (g_isOverlayVisible.load() && g_resultsHost) {
                g_resultsHost.Opacity(1.0);
                g_resultsHost.IsHitTestVisible(true);
                if (g_resultsTranslate) g_resultsTranslate.Y(0.0);
            }
        });

        g_revealAnim = sb;
        sb.Begin();
    } catch (...) {}
}

void HideOverlayAnimated() {
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

        sb.Completed([](wf::IInspectable const&, wf::IInspectable const&) {
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
            }
        });

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

static HWND g_hCoreWindow = nullptr;

HWND GetOurCoreWindow() {
    if (g_hCoreWindow && IsWindow(g_hCoreWindow)) {
        return g_hCoreWindow;
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
        if (current == ours) {
            return;
        }

        SetForegroundWindow(ours);
        BringWindowToTop(ours);
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
        t.Tick([ticks](wf::IInspectable const& sender, wf::IInspectable const&) {
            auto timer = sender.try_as<wux::DispatcherTimer>();
            if (g_suppressRefocus.load()) {
                if (timer) timer.Stop();
                return;
            }
            HWND ours = GetOurCoreWindow();
            HWND fg = GetForegroundWindow();
            DWORD fgPid = 0;
            if (fg) GetWindowThreadProcessId(fg, &fgPid);
            // Reclaim foreground from SearchHost or other window if lost within the grace window
            if (fg != ours && fgPid != GetCurrentProcessId()) {
                TakeForeground(true);
            }
            FocusOurBoxNow();
            auto now = wux::Input::FocusManager::GetFocusedElement();
            if ((now && now == g_ourBox && fg == ours) || ++(*ticks) >= 10) {
                if (timer) timer.Stop();
            }
        });
        t.Start();
        g_openFocus = t;
    } catch (...) {
    }
}

void DismissStartMenu() {
    try {
        g_suppressRefocus.store(true);
        if (g_openFocus) {
            g_openFocus.Stop();
            g_openFocus = nullptr;
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

        HWND ours = GetOurCoreWindow();
        if (ours && IsWindow(ours)) {
            HWND fg = GetForegroundWindow();
            DWORD fgPid = 0;
            if (fg) GetWindowThreadProcessId(fg, &fgPid);
            if (fg == ours || fgPid == GetCurrentProcessId() || fg == nullptr) {
                keybd_event(VK_ESCAPE, 0, 0, 0);
                keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
                Wh_Log(L"DismissStartMenu: sent targeted Escape to CoreWindow %p", ours);
            }
        }
    } catch (...) {
    }
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

void HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl);

static HHOOK g_hGetMsgHook = nullptr;
static winrt::event_token g_charReceivedToken{};
static winrt::event_token g_keyDownToken{};
static winrt::event_token g_activatedToken{};
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

    if (vk == VK_DOWN || vk == VK_UP || vk == VK_RETURN) {
        if (g_isOverlayVisible.load()) {
            bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
            try {
                HandleNavigationKey(static_cast<winrt::Windows::System::VirtualKey>(vk), ctrl);
            } catch (...) {}
            return true;
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
        return 0;
    }
    if (uMsg == WM_ACTIVATE) {
        HWND otherHwnd = reinterpret_cast<HWND>(lParam);
        DWORD otherPid = 0;
        if (otherHwnd) GetWindowThreadProcessId(otherHwnd, &otherPid);

        if (LOWORD(wParam) != WA_INACTIVE) {
            g_suppressRefocus.store(false);
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
            TriggerMenuOpenFocus();
        } else {
            // If the other window is in our own process (e.g. context menu, flyout, tooltip), don't suppress refocus!
            if (otherPid == GetCurrentProcessId()) {
                Wh_Log(L"subclass: WM_ACTIVATE (inactive, internal other=%p) -> ignoring", otherHwnd);
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }

            g_suppressRefocus.store(true);
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
    } else if (uMsg == WM_WINDOWPOSCHANGED) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (wp && !(wp->flags & SWP_HIDEWINDOW)) {
            if (!IsOurWindowCloaked()) {
                HWND fg = GetForegroundWindow();
                if (fg != hWnd) {
                    DWORD fgPid = 0;
                    if (fg) GetWindowThreadProcessId(fg, &fgPid);
                    if (fgPid != GetCurrentProcessId()) {
                        Wh_Log(L"subclass: WM_WINDOWPOSCHANGED uncloaked, fg=%p (ours=%p) -> claiming foreground", fg, hWnd);
                        g_suppressRefocus.store(false);
                        TakeForeground(true);
                        TriggerMenuOpenFocus();
                    }
                }
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

HWINEVENTHOOK g_attachWatch = nullptr;

void CALLBACK AttachWatchProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                              LONG idObject, LONG idChild, DWORD, DWORD) {
    if ((event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_UNCLOAKED) ||
        !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_OBJECT_UNCLOAKED && hwnd == GetOurCoreWindow()) {
        g_suppressRefocus.store(false);
        TriggerMenuOpenFocus();
    }
    // In-context, so this is the thread that raised the event. The filter is
    // Window::Current() returning something rather than a class name, because
    // a name that changes across builds breaks quietly.
    TryAttachFromWindowRoot();
}

void StartAttachWatch() {
    if (g_attachWatch) {
        return;
    }
    g_attachWatch = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_UNCLOAKED,
                                    GetCurrentModuleHandle(), AttachWatchProc,
                                    GetCurrentProcessId(), 0,
                                    WINEVENT_INCONTEXT);
    Wh_Log(L"attach watch %ls", g_attachWatch ? L"installed" : L"FAILED");
}

void StopAttachWatch() {
    if (g_attachWatch) {
        UnhookWinEvent(g_attachWatch);
        g_attachWatch = nullptr;
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
    std::vector<BYTE> icon;  // BGRA, kIconSize square, or empty
    bool canRunAsAdmin = true;
    bool isSetting = false;
    bool isFolder = false;
    std::wstring copyText;   // text to copy to clipboard on activation
    std::wstring customGlyph; // Segoe Fluent glyph override (e.g. \uE1D0, \uE701, \uE88E)
};

// Fetched at 48, drawn at 24.
//
// These are two different things and conflating them is what made the first
// attempt look blurry. The draw size is in logical pixels, so on a display at
// 150% a 24-logical icon is 36 real pixels -- a 24px bitmap has to be
// stretched to fill it. Asking the shell for 48 and letting XAML scale down
// stays sharp to 200%, and costs nothing extra: the shell has these sizes
// already.
inline constexpr int kIconSize = 48;     // what we ask the shell for
inline constexpr int kIconDisplay = 24;  // what it occupies in the row

std::vector<Row> g_appRows;
std::vector<Row> g_fileRows;

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
        if (!ShellExecuteExW(&info)) {
            Wh_Log(L"open failed (%lu): %ls (admin=%d)", GetLastError(), path.c_str(), asAdmin ? 1 : 0);
        }
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

void OpenFileLocation(std::wstring path) {
    SpawnTrackedLaunch([path = std::move(path)] {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        std::wstring args = L"/select,\"" + path + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

void ShowPropertiesDialog(std::wstring path) {
    SpawnTrackedLaunch([path = std::move(path)] {
        // Sleep briefly to let DismissStartMenu()'s VK_ESCAPE pass through
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
            hHost = FindWindowW(kExplorerHelperClassName, kExplorerHelperWindowName);
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
            ShellExecuteExW(&sei);
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
            ShellExecuteExW(&sei);
        }
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

inline bool CopyTextToClipboard(const std::wstring& text) {
    if (text.empty()) return false;
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hGlobal) {
        void* ptr = GlobalLock(hGlobal);
        if (ptr) {
            memcpy(ptr, text.c_str(), bytes);
            GlobalUnlock(hGlobal);
            SetClipboardData(CF_UNICODETEXT, hGlobal);
        }
    }
    CloseClipboard();
    return true;
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
static int g_selectedApp = -1;
static uint64_t g_lastNavTick = 0;

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

    bool isLight = IsLightTheme();

    for (size_t i = 0; i < g_appButtonsOpt->size(); ++i) {
        auto& btn = (*g_appButtonsOpt)[i];
        if (!btn) continue;

        if (static_cast<int>(i) == index) {
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
        DismissStartMenu();
        OpenResult(row.openPath, false);
        Wh_Log(L"launch web: '%ls'", row.openPath.c_str());
        return;
    }
    int which = row.appIndex;
    if (which >= 0) {
        DismissStartMenu();
        if (asAdmin && !row.canRunAsAdmin) {
            asAdmin = false;
        }
        {
            std::lock_guard<std::mutex> lock(g_queryMutex);
            g_launchAsAdmin.store(asAdmin);
            g_launchRequest.store(which);
        }
        g_queryWake.notify_all();
        Wh_Log(L"launch app: index %d ('%ls'), asAdmin=%d", index, row.title.c_str(), asAdmin ? 1 : 0);
    }
}

void HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl) {
    uint64_t now = GetTickCount64();
    if (now - g_lastNavTick < 60) return;
    g_lastNavTick = now;

    if (key == winrt::Windows::System::VirtualKey::Down) {
        if (g_appButtonsOpt && !g_appButtonsOpt->empty()) {
            int next = (g_selectedApp < 0) ? 0 : g_selectedApp + 1;
            if (next >= static_cast<int>(g_appButtonsOpt->size())) {
                next = static_cast<int>(g_appButtonsOpt->size()) - 1;
            }
            SetAppSelection(next);
        }
    } else if (key == winrt::Windows::System::VirtualKey::Up) {
        if (g_appButtonsOpt && !g_appButtonsOpt->empty()) {
            int prev = (g_selectedApp <= 0) ? 0 : g_selectedApp - 1;
            SetAppSelection(prev);
        }
    } else if (key == winrt::Windows::System::VirtualKey::Enter) {
        if (!g_currentAppRows.empty()) {
            int target = (g_selectedApp >= 0) ? g_selectedApp : 0;
            LaunchSelectedApp(target, ctrl);
        }
    }
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

    wuxc::Grid root;
    root.Name(L"WindhawkEverythingResults");
    root.Margin(wux::ThicknessHelper::FromLengths(14, 14, 14, 10));
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
    searchBarBorder.Height(40);
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
    searchIcon.FontSize(14);
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
    box.FontSize(14);
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

    box.PreviewKeyDown([](wf::IInspectable const&, wux::Input::KeyRoutedEventArgs const& args) {
        try {
            auto key = args.Key();
            if (key == winrt::Windows::System::VirtualKey::Escape) {
                if (g_isOverlayVisible.load() || g_isHiding.load()) {
                    if (g_ourBox) g_ourBox.Text(L"");
                    HideOverlayAnimated();
                    args.Handled(true);
                    return;
                }
            }
            if (key == winrt::Windows::System::VirtualKey::Down ||
                key == winrt::Windows::System::VirtualKey::Up ||
                key == winrt::Windows::System::VirtualKey::Enter) {
                if (g_isOverlayVisible.load()) {
                    bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
                    HandleNavigationKey(key, ctrl);
                    args.Handled(true);
                }
            }
        } catch (...) {}
    });

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
    boltIcon.FontSize(11);
    boltIcon.Opacity(0.5);
    boltIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
    boltIcon.VerticalAlignment(wux::VerticalAlignment::Center);
    leftStatus.Children().Append(boltIcon);

    wuxc::TextBlock statusText;
    statusText.Text(L"Everything Search");
    statusText.FontSize(11);
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
        keyBlock.FontSize(9.5);
        keyBlock.FontWeight(wut::FontWeights::SemiBold());
        keyBlock.Opacity(0.75);
        keyBadge.Child(keyBlock);
        pair.Children().Append(keyBadge);

        wuxc::TextBlock actionBlock;
        actionBlock.Text(winrt::hstring{action});
        actionBlock.FontSize(10.5);
        actionBlock.Opacity(0.45);
        actionBlock.Margin(wux::ThicknessHelper::FromLengths(4, 0, 0, 0));
        actionBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        pair.Children().Append(actionBlock);

        return pair;
    };

    rightHints.Children().Append(makeKeyCap(L"\u2191\u2193", L"Select"));
    rightHints.Children().Append(makeKeyCap(L"\u21B5", L"Open"));
    rightHints.Children().Append(makeKeyCap(L"Ctrl+\u21B5", L"Admin"));
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
        total = g_totalMatches.load();
    }

    if (files.empty() && appNames.empty() && g_ourBox && g_ourBox.Text().empty()) {
        g_currentAppRows.clear();
        g_selectedApp = -1;
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
        } else if (!appNames.empty() && (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:"))) {
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
        icon.FontSize(11.5);
        icon.Opacity(0.6);
        icon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
        icon.VerticalAlignment(wux::VerticalAlignment::Center);
        leftStack.Children().Append(icon);

        wuxc::TextBlock titleBlock;
        titleBlock.Text(winrt::hstring{title});
        titleBlock.FontSize(11);
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
            badgeBlock.FontSize(9.5);
            badgeBlock.FontWeight(wut::FontWeights::SemiBold());
            badgeBlock.Opacity(0.7);
            badge.Child(badgeBlock);

            wuxc::Grid::SetColumn(badge, 1);
            headerGrid.Children().Append(badge);
        }

        return headerGrid;
    };

    bool isWebMode = (!appNames.empty() && (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:")));
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
        if (item.icon.size() == static_cast<size_t>(kIconSize) * kIconSize * 4) {
            wuxmi::WriteableBitmap bmp{kIconSize, kIconSize};
            auto buffer = bmp.PixelBuffer();
            auto access = buffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
            BYTE* dest = nullptr;
            if (SUCCEEDED(access->Buffer(&dest)) && dest) {
                memcpy(dest, item.icon.data(), item.icon.size());
                bmp.Invalidate();
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
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::FontIcon fallbackIcon;
            fallbackIcon.FontSize(15);
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
        name.FontSize(12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            wuxc::TextBlock sub;
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            sub.FontSize(10.5);
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

        button.PointerEntered([btn = button](wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
            if (!g_activeAppsOpt) return;
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    SetAppSelection(static_cast<int>(i));
                    break;
                }
            }
        });

        button.Click([btn = button](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            if (!g_activeAppsOpt) return;
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    LaunchSelectedApp(static_cast<int>(i), false);
                    return;
                }
            }
        });

        wuxc::MenuFlyout flyout;
        if (!item.copyText.empty()) {
            wuxc::MenuFlyoutItem copyItem;
            copyItem.Text(L"Copy to clipboard");
            wuxc::FontIcon copyIcon;
            copyIcon.Glyph(L"\uE8C8");
            copyItem.Icon(copyIcon);
            copyItem.Click([txt = item.copyText](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                tools::CopyTextToClipboard(txt);
            });
            flyout.Items().Append(copyItem);
        } else {
            bool isWebItem = item.openPath.starts_with(L"http:") || item.openPath.starts_with(L"https:");
            bool isSettingItem = item.isSetting;

            wuxc::MenuFlyoutItem openItem;
            openItem.Text(isWebItem ? L"Search in browser" : L"Open");
            wuxc::FontIcon openIcon;
            openIcon.Glyph(isWebItem ? L"\uE774" : L"\uE8A7");
            openItem.Icon(openIcon);
            openItem.Click([btn = button](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (!g_activeAppsOpt) return;
                for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                    if ((*g_activeAppsOpt)[i].button == btn) {
                        LaunchSelectedApp(static_cast<int>(i), false);
                        return;
                    }
                }
            });
            flyout.Items().Append(openItem);

            if (item.canRunAsAdmin && !isWebItem && !isSettingItem) {
                wuxc::MenuFlyoutItem adminItem;
                adminItem.Text(L"Run as administrator");
                wuxc::FontIcon adminIcon;
                adminIcon.Glyph(L"\uE7EF");
                adminItem.Icon(adminIcon);
                adminItem.Click([btn = button](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    if (!g_activeAppsOpt) return;
                    for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                        if ((*g_activeAppsOpt)[i].button == btn) {
                            LaunchSelectedApp(static_cast<int>(i), true);
                            return;
                        }
                    }
                });
                flyout.Items().Append(adminItem);
            }

            if (isWebItem) {
                std::wstring webUrl = item.openPath;
                wuxc::MenuFlyoutItem copyUrlItem;
                copyUrlItem.Text(L"Copy search link");
                wuxc::FontIcon copyIcon;
                copyIcon.Glyph(L"\uE8C8");
                copyUrlItem.Icon(copyIcon);
                copyUrlItem.Click([webUrl](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    CopyTextToClipboard(webUrl);
                });
                flyout.Items().Append(copyUrlItem);
            } else if (!isSettingItem && !item.openPath.empty()) {
                std::wstring locTarget = item.openPath;
                std::wstring appTitle = item.title;
                bool isFile = (GetFileAttributesW(locTarget.c_str()) != INVALID_FILE_ATTRIBUTES);

                if (isFile) {
                    wuxc::MenuFlyoutSeparator sep1;
                    flyout.Items().Append(sep1);

                    wuxc::MenuFlyoutItem locItem;
                    locItem.Text(L"Open file location");
                    wuxc::FontIcon locIcon;
                    locIcon.Glyph(L"\uE838");
                    locItem.Icon(locIcon);
                    locItem.Click([locTarget](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenu();
                        OpenFileLocation(locTarget);
                    });
                    flyout.Items().Append(locItem);

                    wuxc::MenuFlyoutItem copyPathItem;
                    copyPathItem.Text(L"Copy path");
                    wuxc::FontIcon copyPathIcon;
                    copyPathIcon.Glyph(L"\uE71B");
                    copyPathItem.Icon(copyPathIcon);
                    copyPathItem.Click([locTarget](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        tools::CopyTextToClipboard(locTarget);
                    });
                    flyout.Items().Append(copyPathItem);

                    wuxc::MenuFlyoutItem shortcutItem;
                    shortcutItem.Text(L"Create desktop shortcut");
                    wuxc::FontIcon shortcutIcon;
                    shortcutIcon.Glyph(L"\uE7C5");
                    shortcutItem.Icon(shortcutIcon);
                    shortcutItem.Click([locTarget, appTitle](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenu();
                        tools::CreateDesktopShortcut(locTarget, appTitle);
                    });
                    flyout.Items().Append(shortcutItem);

                    wuxc::MenuFlyoutSeparator sep2;
                    flyout.Items().Append(sep2);

                    wuxc::MenuFlyoutItem propItem;
                    propItem.Text(L"Properties");
                    wuxc::FontIcon propIcon;
                    propIcon.Glyph(L"\uE946");
                    propItem.Icon(propIcon);
                    propItem.Click([locTarget](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenu();
                        ShowPropertiesDialog(locTarget);
                    });
                    flyout.Items().Append(propItem);
                } else if (!locTarget.starts_with(L"ms-settings:") && !locTarget.starts_with(L"http:") && !locTarget.starts_with(L"https:")) {
                    wuxc::MenuFlyoutSeparator sep1;
                    flyout.Items().Append(sep1);

                    wuxc::MenuFlyoutItem shortcutItem;
                    shortcutItem.Text(L"Create desktop shortcut");
                    wuxc::FontIcon shortcutIcon;
                    shortcutIcon.Glyph(L"\uE7C5");
                    shortcutItem.Icon(shortcutIcon);
                    shortcutItem.Click([locTarget, appTitle](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenu();
                        tools::CreateDesktopShortcut(L"shell:AppsFolder\\" + locTarget, appTitle);
                    });
                    flyout.Items().Append(shortcutItem);
                }
            }
        }

        button.ContextFlyout(flyout);

        return AppCardUI{item.appIndex, item.title, item.openPath, button, item.canRunAsAdmin, item.isSetting};
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
        emptyIcon.FontSize(24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        wuxc::TextBlock emptyTitle;
        emptyTitle.Text(L"No matching applications");
        emptyTitle.FontSize(11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        wuxc::TextBlock emptySubtitle;
        emptySubtitle.Text(L"Check files or refine your query");
        emptySubtitle.FontSize(10.5);
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
        if (item.icon.size() == static_cast<size_t>(kIconSize) * kIconSize * 4) {
            wuxmi::WriteableBitmap bmp{kIconSize, kIconSize};
            auto buffer = bmp.PixelBuffer();
            auto access = buffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
            BYTE* dest = nullptr;
            if (SUCCEEDED(access->Buffer(&dest)) && dest) {
                memcpy(dest, item.icon.data(), item.icon.size());
                bmp.Invalidate();
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
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::FontIcon fallbackIcon;
            fallbackIcon.FontSize(15);
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
        name.FontSize(12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            wuxc::TextBlock sub;
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            sub.FontSize(10.5);
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

        if (!item.openPath.empty()) {
            std::wstring target = item.openPath;
            button.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                DismissStartMenu();
                OpenResult(target);
            });

            wuxc::MenuFlyout flyout;
            wuxc::MenuFlyoutItem openItem;
            openItem.Text(L"Open");
            wuxc::FontIcon openIcon;
            openIcon.Glyph(L"\uE8A7");
            openItem.Icon(openIcon);
            openItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                DismissStartMenu();
                OpenResult(target);
            });
            flyout.Items().Append(openItem);

            std::wstring lower = target;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
            bool canElevate = lower.ends_with(L".exe") || lower.ends_with(L".bat") ||
                              lower.ends_with(L".cmd") || lower.ends_with(L".ps1") ||
                              lower.ends_with(L".msc") || lower.ends_with(L".lnk");
            if (canElevate) {
                wuxc::MenuFlyoutItem adminItem;
                adminItem.Text(L"Run as administrator");
                wuxc::FontIcon adminIcon;
                adminIcon.Glyph(L"\uE7EF");
                adminItem.Icon(adminIcon);
                adminItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    DismissStartMenu();
                    OpenResult(target, true /* asAdmin */);
                });
                flyout.Items().Append(adminItem);
            }

            if (item.isFolder) {
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
                    termItem.Click([target, isPowerShell, asAdmin](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissStartMenu();
                        LaunchTerminal(target, isPowerShell, asAdmin);
                    });
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
            cutItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                tools::CopyOrCutFileToClipboard(target, true /* isCut */);
            });
            flyout.Items().Append(cutItem);

            wuxc::MenuFlyoutItem copyItem;
            copyItem.Text(L"Copy");
            wuxc::FontIcon copyIcon;
            copyIcon.Glyph(L"\uE8C8");
            copyItem.Icon(copyIcon);
            copyItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                tools::CopyOrCutFileToClipboard(target, false /* isCut */);
            });
            flyout.Items().Append(copyItem);

            wuxc::MenuFlyoutItem copyPathItem;
            copyPathItem.Text(L"Copy path");
            wuxc::FontIcon copyPathIcon;
            copyPathIcon.Glyph(L"\uE71B");
            copyPathItem.Icon(copyPathIcon);
            copyPathItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                tools::CopyTextToClipboard(target);
            });
            flyout.Items().Append(copyPathItem);

            wuxc::MenuFlyoutSeparator sep2;
            flyout.Items().Append(sep2);

            wuxc::MenuFlyoutItem locItem;
            locItem.Text(L"Open file location");
            wuxc::FontIcon locIcon;
            locIcon.Glyph(L"\uE838");
            locItem.Icon(locIcon);
            locItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                DismissStartMenu();
                OpenFileLocation(target);
            });
            flyout.Items().Append(locItem);

            wuxc::MenuFlyoutItem shortcutItem;
            shortcutItem.Text(L"Create desktop shortcut");
            wuxc::FontIcon shortcutIcon;
            shortcutIcon.Glyph(L"\uE7C5");
            shortcutItem.Icon(shortcutIcon);
            shortcutItem.Click([target, title = item.title](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                DismissStartMenu();
                tools::CreateDesktopShortcut(target, title);
            });
            flyout.Items().Append(shortcutItem);

            wuxc::MenuFlyoutSeparator sep3;
            flyout.Items().Append(sep3);

            wuxc::MenuFlyoutItem propItem;
            propItem.Text(L"Properties");
            wuxc::FontIcon propIcon;
            propIcon.Glyph(L"\uE946");
            propItem.Icon(propIcon);
            propItem.Click([target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                DismissStartMenu();
                ShowPropertiesDialog(target);
            });
            flyout.Items().Append(propItem);

            button.ContextFlyout(flyout);
        }
        return button;
    };

    // Files on the right.
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
        emptyIcon.FontSize(24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        wuxc::TextBlock emptyTitle;
        emptyTitle.Text(L"No matching files");
        emptyTitle.FontSize(11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        wuxc::TextBlock emptySubtitle;
        emptySubtitle.Text(L"Everything index returned 0 items");
        emptySubtitle.FontSize(10.5);
        emptySubtitle.Opacity(0.3);
        emptySubtitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptySubtitle.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        emptyStack.Children().Append(emptySubtitle);

        emptyCard.Child(emptyStack);
        g_resultsList.Children().Append(emptyCard);
    } else {
        for (const Row& file : files) {
            g_resultsList.Children().Append(makeFileRow(file));
        }
    }

    if (g_ourBox && g_ourBox.Text().empty()) {
        HideOverlayAnimated();
    } else {
        RevealOverlayAnimated();
    }
    Wh_Log(L"render: %zu apps, %zu files, host %.0fx%.0f", appNames.size(),
        files.size(), g_resultsHost.ActualWidth(),
        g_resultsHost.ActualHeight());
} catch (...) {
    Wh_Log(L"render failed %08X", static_cast<unsigned>(winrt::to_hresult()));
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

// Opens an app by its shell identity.
//
// By PIDL rather than by name: apps_index.h explains why -- the parsing names
// come in several shapes, including AUMIDs and known-folder GUIDs, and
// rebuilding a path from them fails outright for some. The PIDL works for all
// of them.
//
// Called only on the search thread, which owns the index and therefore the
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
            HINSTANCE hInst = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(hInst) > 32) {
                Wh_Log(L"launched URI setting: %ls", path.c_str());
                launched = true;
            } else {
                Wh_Log(L"URI launch failed (%ld): %ls", reinterpret_cast<INT_PTR>(hInst), path.c_str());
            }
        } else if (path.starts_with(L"control ")) {
            std::wstring params = path.substr(8);
            HINSTANCE hInst = ShellExecuteW(nullptr, asAdmin ? L"runas" : L"open", L"control.exe", params.c_str(), nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(hInst) > 32) {
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
        if (ShellExecuteExW(&info)) {
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

void SearchThreadMain() {
    // COM for the apps index: it enumerates shell:AppsFolder.
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    apps::Index appIndex;
    if (appIndex.Rebuild()) {
        g_lastAppIndexRebuildTick.store(GetTickCount64());
        Wh_Log(L"apps: indexed (%zu apps)", appIndex.Count());
    } else {
        Wh_Log(L"apps: index failed");
    }

    icons::ExtensionCache iconCache(kIconSize);

    // The apps behind the rows currently on screen, in the same order.
    std::vector<const apps::App*> lastHits;

    // One fetch per app, ever. Keyed by name because that is what identifies
    // an entry across index rebuilds.
    std::map<std::wstring, std::vector<BYTE>> appIconCache;

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
            g_queryWake.wait(lock, [] {
                return g_queryDirty.load() || g_searchQuit.load() || (g_launchRequest.load() >= 0) || g_appIndexNeedsRefresh.load();
            });
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
                    std::wstring name = app->name;
                    std::wstring targetPath = app->targetPath;
                    ITEMIDLIST* pidlClone = app->pidl ? ILClone(app->pidl.get()) : nullptr;
                    lock.unlock();

                    SpawnTrackedLaunch([name = std::move(name), targetPath = std::move(targetPath), pidlClone, asAdmin] {
                        LaunchAppAsync(name, targetPath, pidlClone, asAdmin);
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

        // Settle: if starting a fresh query from empty, search immediately with
        // zero delay so results are ready before overlay reveals.
        // For subsequent typing bursts, settle for 25ms so we search the newer text.
        if (!last.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
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

        if (query.empty()) {
            lastHits.clear();
            g_launchRequest.store(-1);
            {
                std::lock_guard<std::mutex> lock(g_resultsMutex);
                g_appRows.clear();
                g_fileRows.clear();
                g_totalMatches.store(0);
            }
            RequestRender();
            continue;
        }


        std::wstring qTrim = tools::Trim(query);
        std::wstring qLower = tools::ToLower(qTrim);

        bool isIpCommand = (qLower == L"/ip" || qLower.starts_with(L"/ip "));
        bool isCCommand = (qLower == L"/c" || qLower.starts_with(L"/c ") ||
                          (qLower.size() >= 3 && qLower[0] == L'/' && qLower[1] == L'c' &&
                           (iswdigit(qLower[2]) || qLower[2] == L'-' || qLower[2] == L'+' || qLower[2] == L'(')));

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

        if (isIpCommand || isCCommand) {
            pool.clear();
            total = 0;
            ms = 0;
        } else if (isExplicitWeb) {
            if (!explicitWeb.queryTerm.empty()) {
                auto start = std::chrono::steady_clock::now();
                if (client.Query(explicitWeb.queryTerm, ranker::kDefaultPool, &pool, &total)) {
                    ranker::Rank(&pool, explicitWeb.queryTerm, static_cast<size_t>(maxFiles), noisePtr);
                }
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now() - start)
                         .count();
            }
        } else {
            auto start = std::chrono::steady_clock::now();
            bool ok = client.Query(query, ranker::kDefaultPool, &pool, &total);
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - start)
                          .count();
            if (!ok) {
                Wh_Log(L"search: '%ls' failed (Everything running?)", query.c_str());
                pool.clear();
                total = 0;
            } else {
                ranker::Rank(&pool, query, static_cast<size_t>(maxFiles), noisePtr);
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
        } else if (isCCommand) {
            std::wstring cArg = tools::Trim(qTrim.substr(2));
            if (cArg.empty()) {
                Row r;
                r.title = L"Calculator & Unit Converter";
                r.subtitle = L"Usage: /c <expression> for math, or /c <number> [unit] for conversion \u2022 e.g. /c 100 * 5, /c 100 km, /c 50";
                r.customGlyph = L"\uE1D0";
                r.canRunAsAdmin = false;
                r.appIndex = static_cast<int>(lastHits.size());
                lastHits.push_back(nullptr);
                appRows.push_back(std::move(r));
            } else {
                double mathVal = 0.0;
                if (tools::EvaluateMath(cArg, mathVal)) {
                    Row r;
                    std::wstring formatted = tools::FormatCleanNumber(mathVal);
                    r.title = L"= " + formatted;
                    r.subtitle = cArg + L" \u2022 Press Enter to copy result";
                    r.copyText = formatted;
                    r.customGlyph = L"\uE1D0"; // Calculator
                    r.canRunAsAdmin = false;
                    r.appIndex = static_cast<int>(lastHits.size());
                    lastHits.push_back(nullptr);
                    appRows.push_back(std::move(r));
                } else {
                    double convNum = 0.0;
                    std::wstring convUnit;
                    bool isHelp = false;
                    if (tools::ParseConversionQuery(qTrim, convNum, convUnit, isHelp)) {
                        std::vector<CustomConversion> conversions;
                        {
                            std::lock_guard<std::mutex> lock(g_settingsMutex);
                            conversions = g_settings.unitConversions;
                        }

                        std::wstring normUnit = tools::NormalizeUnit(convUnit);
                        std::vector<Row> convRows;

                        for (const auto& c : conversions) {
                            if (!convUnit.empty()) {
                                std::wstring normFrom = tools::NormalizeUnit(c.fromUnit);
                                if (normFrom != normUnit && tools::ToLower(c.fromUnit) != convUnit) {
                                    continue;
                                }
                            }
                            double outVal = 0.0;
                            if (tools::EvaluateConversionFormula(c.formula, convNum, outVal)) {
                                Row r;
                                std::wstring numStr = tools::FormatCleanNumber(convNum);
                                std::wstring outStr = tools::FormatCleanNumber(outVal);
                                r.title = numStr + L" " + c.fromUnit + L" = " + outStr + L" " + c.toUnit;
                                r.subtitle = c.category + L" \u2022 Press Enter to copy " + outStr + L" " + c.toUnit;
                                r.copyText = outStr + L" " + c.toUnit;
                                r.customGlyph = L"\uE88E";
                                r.canRunAsAdmin = false;
                                r.appIndex = static_cast<int>(lastHits.size());
                                lastHits.push_back(nullptr);
                                convRows.push_back(std::move(r));
                            }
                        }

                        if (convUnit.empty() && convNum >= 0.0 && convNum <= 16777215.0 && (convNum == std::floor(convNum))) {
                            unsigned long long intVal = static_cast<unsigned long long>(convNum);
                            wchar_t hexBuf[32];
                            swprintf_s(hexBuf, L"0x%llX", intVal);
                            std::wstring binStr = L"0b";
                            if (intVal == 0) {
                                binStr += L"0";
                            } else {
                                for (int b = 31; b >= 0; --b) {
                                    if ((intVal >> b) & 1) {
                                        for (int j = b; j >= 0; --j) {
                                            binStr.push_back(((intVal >> j) & 1) ? L'1' : L'0');
                                        }
                                        break;
                                    }
                                }
                            }
                            wchar_t octBuf[32];
                            swprintf_s(octBuf, L"0o%llo", intVal);
                            std::wstring numStr = tools::FormatCleanNumber(convNum);

                            Row r;
                            r.title = numStr + L" = " + hexBuf + L" (Hex) = " + binStr + L" (Bin) = " + octBuf + L" (Oct)";
                            r.subtitle = L"Base Radix \u2022 Press Enter to copy " + std::wstring(hexBuf);
                            r.copyText = hexBuf;
                            r.customGlyph = L"\uE88E";
                            r.canRunAsAdmin = false;
                            r.appIndex = static_cast<int>(lastHits.size());
                            lastHits.push_back(nullptr);
                            convRows.push_back(std::move(r));
                        }

                        if (convRows.empty()) {
                            Row r;
                            if (!convUnit.empty()) {
                                r.title = L"Unknown Unit: \"" + convUnit + L"\"";
                                r.subtitle = L"No match in configured conversions. Configure custom units in Windhawk Settings.";
                            } else {
                                r.title = L"No Conversions Configured";
                                r.subtitle = L"Add unit conversion rules in Windhawk Mod Settings.";
                            }
                            r.customGlyph = L"\uE88E";
                            r.canRunAsAdmin = false;
                            r.appIndex = static_cast<int>(lastHits.size());
                            lastHits.push_back(nullptr);
                            convRows.push_back(std::move(r));
                        }

                        for (auto& cr : convRows) {
                            appRows.push_back(std::move(cr));
                        }
                    } else {
                        Row r;
                        r.title = L"Invalid Expression or Unit: \"" + cArg + L"\"";
                        r.subtitle = L"Usage: /c <expression> (e.g. /c 100 * 5) or /c <number> [unit] (e.g. /c 100 km)";
                        r.customGlyph = L"\uE1D0";
                        r.canRunAsAdmin = false;
                        r.appIndex = static_cast<int>(lastHits.size());
                        lastHits.push_back(nullptr);
                        appRows.push_back(std::move(r));
                    }
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
            for (const apps::Match& m : appIndex.Search(query, static_cast<size_t>(maxApps))) {
                if (!m.app) {
                    continue;
                }
                Row row;
                row.title = m.app->name;
                bool isSettingItem = m.app->isSetting;
                row.isSetting = isSettingItem;

                bool canAdmin = true;
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
                } else if (!m.app->exeNameLower.empty()) {
                    row.subtitle = m.app->exeNameLower + L".exe";
                    canAdmin = true;
                } else {
                    row.subtitle = L"Application";
                }
                row.canRunAsAdmin = canAdmin;
                row.openPath = m.app->targetPath;
                row.appIndex = static_cast<int>(lastHits.size());

                auto cached = appIconCache.find(m.app->name);
                if (cached == appIconCache.end()) {
                    std::vector<BYTE> pixels;
                    if (!m.app->isSetting || m.app->pidl) {
                        FetchAppIcon(m.app, kIconSize, &pixels);
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

            // Math expression check
            if (!query.empty()) {
                std::vector<Row> utilityRows;

                double mathVal = 0.0;
                std::wstring mathExpr = query;
                if (mathExpr.starts_with(L"=") || mathExpr.starts_with(L"/calc ")) {
                    if (mathExpr.starts_with(L"=")) mathExpr = mathExpr.substr(1);
                    else mathExpr = mathExpr.substr(6);
                }
                if (tools::EvaluateMath(mathExpr, mathVal)) {
                    Row r;
                    std::wstring formatted = tools::FormatCleanNumber(mathVal);
                    r.title = L"= " + formatted;
                    r.subtitle = tools::Trim(mathExpr) + L" \u2022 Press Enter to copy result";
                    r.copyText = formatted;
                    r.customGlyph = L"\uE1D0"; // Calculator
                    r.canRunAsAdmin = false;
                    r.appIndex = static_cast<int>(lastHits.size());
                    lastHits.push_back(nullptr);
                    utilityRows.push_back(std::move(r));
                }

                for (auto& ur : utilityRows) {
                    appRows.push_back(std::move(ur));
                }
            }
        }

        std::vector<Row> fileRows;
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
            // One shell call per distinct extension, not per row: the cache
            // answers from the registered file type without touching disk.
            if (const std::vector<BYTE>* pixels =
                    iconCache.Get(r.name, r.isFolder)) {
                row.icon = *pixels;
            }
            fileRows.push_back(std::move(row));
        }

        {
            std::lock_guard<std::mutex> lock(g_resultsMutex);
            g_appRows = appRows;
            g_fileRows = fileRows;
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

                                if (key == winrt::Windows::System::VirtualKey::Escape) {
                                    if (g_isOverlayVisible.load() || g_isHiding.load()) {
                                        if (g_ourBox) g_ourBox.Text(L"");
                                        HideOverlayAnimated();
                                        args.Handled(true);
                                        return;
                                    }
                                }

                                if (key == winrt::Windows::System::VirtualKey::Down ||
                                    key == winrt::Windows::System::VirtualKey::Up ||
                                    key == winrt::Windows::System::VirtualKey::Enter) {
                                    if (g_isOverlayVisible.load()) {
                                        bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
                                        HandleNavigationKey(key, ctrl);
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
                            if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated) {
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
                                TriggerMenuOpenFocus();
                            } else {
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
    try {
        StopAttachWatch();
    } catch (...) {}

    try {
        if (g_hCoreWindow && g_subclassed) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hCoreWindow, StartMenuSubclassProc);
            g_subclassed = false;
        }
    } catch (...) {}

    if (g_openFocus) {
        g_openFocus.Stop();
        g_openFocus = nullptr;
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
                });
        } catch (...) {}
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hCoreWindow) {
        RemovePropW(g_hCoreWindow, L"WindhawkStartMenuWindow");
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
        EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == GetCurrentProcessId()) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc);
            }
            return TRUE;
        }, 0);
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

    HWND hCore = g_hCoreWindow;
    if (hCore && IsWindow(hCore)) {
        DWORD_PTR result = 0;
        LRESULT lr = SendMessageTimeoutW(hCore, GetTeardownMessage(), 0, 0,
                                         SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &result);
        if (lr == 0) {
            Wh_Log(L"uninit: SendMessageTimeoutW timed out or failed (%lu); attempting direct teardown", GetLastError());
            try {
                TeardownStartMenuUi();
            } catch (...) {}
        }
    } else {
        try {
            TeardownStartMenuUi();
        } catch (...) {}
    }

    if (g_hGetMsgHook) {
        UnhookWindowsHookEx(g_hGetMsgHook);
        g_hGetMsgHook = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_queryMutex);
        g_searchQuit.store(true);
    }
    g_queryWake.notify_all();
    if (g_searchThread && g_searchThread->joinable()) {
        g_searchThread->join();
        g_searchThread.reset();
    }
    WaitForTrackedLaunches();
    Wh_Log(L"uninit: StartMenuExperienceHost teardown complete");
}
