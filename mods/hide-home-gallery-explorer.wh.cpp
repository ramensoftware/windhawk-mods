// ==WindhawkMod==
// @id              hide-home-gallery-explorer
// @name            Hide Home, Gallery & OneDrive in Explorer
// @description     Hides the "Home", "Gallery", and "OneDrive" items from File Explorer's navigation pane on Windows 11. Also supports hiding user-specified custom labels.
// @version         0.3
// @author          rinosaur681
// @github          https://github.com/rinosaur681
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Hide Home, Gallery & OneDrive in Explorer

This mod removes the "Home", "Gallery", and optionally "OneDrive" items from the File Explorer navigation pane in Windows 11
by finding the TreeView control and deleting those entries. It scans periodically in case Explorer re-inserts them during
refresh/reload. You can also specify custom labels (single or list) to hide, with match options (exact/contains/startsWith)
and case sensitivity.

What’s new in v0.3:
- Eliminates flashing: periodic scans do a dry-run first and only freeze/redraw when deletions are needed.
- Optional OneDrive toggle with configurable label.
- Custom items hiding: type one or more labels, choose match mode, and case sensitivity.

What’s new in v0.2:
- Prevents the nav pane from jumping to the bottom at first open.
- Adds timing.initialDelayMs and timing.scanIntervalMs settings.

Notes:
- Locale: matching is text-based ("Home", "Gallery", "OneDrive"). Adjust the strings in the settings if your OS is not English (UK/US).
- No registry changes are made. Unload the mod to restore the items immediately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hide:
  - home: true
  - gallery: true
  - onedrive: true
  - homeText: Home
  - galleryText: Gallery
  - onedriveText: OneDrive
  - customEnabled: false
  - customText: ""
  - customList: ""          # comma/semicolon/newline-separated list (e.g., "Network; This PC")
  - matchMode: exact        # exact | contains | startsWith
  - caseSensitive: false
  $name: Items to hide
  $description: Toggle specific items and adjust their labels for non-English systems. To hide custom items, enable 'customEnabled', type one item in 'customText' or multiple in 'customList' (separated by comma/semicolon or newline), and choose match mode.

- timing:
  - scanIntervalMs: 300
  - initialDelayMs: 0
  $name: Timing
  $description: Scan interval and optional first-time delay (per window). Use 0ms delay to hide immediately with no flash; use 200–400ms if you prefer to wait for Explorer to finish its initial layout before pruning.
*/
// ==/WindhawkModSettings==

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cwctype>

#pragma comment(lib, "comctl32.lib")

struct {
    bool hideHome;
    bool hideGallery;
    bool hideOneDrive;
    std::wstring homeText;
    std::wstring galleryText;
    std::wstring onedriveText;

    bool customEnabled;
    bool customCaseSensitive;
    int customMatchMode; // 0=exact, 1=contains, 2=startsWith
    std::vector<std::wstring> customTexts;

    int scanIntervalMs;   // how often to scan (ms)
    int initialDelayMs;   // per-window first-time delay before pruning (ms)
} g_settings;

static volatile bool g_stopWorker = false;
static HANDLE g_workerThread = NULL;

// Track windows we've done the initial prune for
static std::unordered_set<HWND> g_prunedOnce;
// Track first-seen ticks for windows to honor initialDelayMs
static std::unordered_map<HWND, ULONGLONG> g_firstSeenTick;

// ---------------- Utilities ----------------

// Case-insensitive wide string compare (exact)
static bool iequals(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i) {
        wint_t ca = towlower(a[i]);
        wint_t cb = towlower(b[i]);
        if (ca != cb) return false;
    }
    return true;
}

static std::wstring trim(const std::wstring& s) {
    size_t i = 0, j = s.size();
    while (i < j && iswspace(s[i])) ++i;
    while (j > i && iswspace(s[j - 1])) --j;
    return s.substr(i, j - i);
}

static std::vector<std::wstring> split_multi(const std::wstring& s) {
    std::vector<std::wstring> out;
    std::wstring token;
    for (wchar_t ch : s) {
        if (ch == L',' || ch == L';' || ch == L'\n' || ch == L'\r') {
            std::wstring t = trim(token);
            if (!t.empty()) out.push_back(t);
            token.clear();
        } else {
            token.push_back(ch);
        }
    }
    std::wstring t = trim(token);
    if (!t.empty()) out.push_back(t);
    return out;
}

static bool equals_cs(const std::wstring& a, const std::wstring& b) {
    return a == b;
}

static bool contains_ci(const std::wstring& hay, const std::wstring& needle) {
    if (needle.empty()) return false;
    std::wstring h(hay), n(needle);
    for (auto& c : h) c = towlower(c);
    for (auto& c : n) c = towlower(c);
    return h.find(n) != std::wstring::npos;
}

static bool startswith_ci(const std::wstring& hay, const std::wstring& needle) {
    if (needle.empty()) return false;
    if (hay.size() < needle.size()) return false;
    for (size_t i = 0; i < needle.size(); ++i) {
        if (towlower(hay[i]) != towlower(needle[i])) return false;
    }
    return true;
}

static bool match_string(const std::wstring& text, const std::wstring& pattern, bool caseSensitive, int mode) {
    if (pattern.empty()) return false;
    if (caseSensitive) {
        switch (mode) {
        case 0: return equals_cs(text, pattern);
        case 1: return text.find(pattern) != std::wstring::npos;
        case 2: return text.rfind(pattern, 0) == 0; // startsWith
        default: return equals_cs(text, pattern);
        }
    } else {
        switch (mode) {
        case 0: return iequals(text, pattern);
        case 1: return contains_ci(text, pattern);
        case 2: return startswith_ci(text, pattern);
        default: return iequals(text, pattern);
        }
    }
}

// Recursively find a child window by class name
static HWND FindChildByClass(HWND parent, const wchar_t* cls) {
    for (HWND h = FindWindowEx(parent, NULL, NULL, NULL); h; h = FindWindowEx(parent, h, NULL, NULL)) {
        wchar_t c[256] = {};
        GetClassNameW(h, c, 255);
        if (wcscmp(c, cls) == 0) {
            return h;
        }
        HWND deeper = FindChildByClass(h, cls);
        if (deeper)
            return deeper;
    }
    return NULL;
}

// Enumerate top-level Explorer windows (CabinetWClass) belonging to THIS process
static std::vector<HWND> GetExplorerWindows() {
    struct EnumCtx {
        std::vector<HWND>* windows;
        DWORD pid;
    } ctx{};

    std::vector<HWND> result;
    ctx.windows = &result;
    ctx.pid = GetCurrentProcessId();

    EnumWindows([](HWND hwnd, LPARAM lParam)->BOOL {
        auto* ctx = reinterpret_cast<EnumCtx*>(lParam);

        // Only consider visible CabinetWClass windows
        wchar_t cls[256] = {};
        GetClassNameW(hwnd, cls, 255);
        if (wcscmp(cls, L"CabinetWClass") != 0 || !IsWindowVisible(hwnd)) {
            return TRUE;
        }

        // Filter by process ID to avoid touching other explorer.exe instances
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == ctx->pid) {
            ctx->windows->push_back(hwnd);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));

    return result;
}


// Get the navigation pane TreeView (SysTreeView32) inside a CabinetWClass window
static HWND GetExplorerNavTree(HWND explorerHwnd) {
    // Win11 Explorer typically contains a SysTreeView32 under multiple nested hosts.
    // We recursively search for the first SysTreeView32 we can find.
    return FindChildByClass(explorerHwnd, L"SysTreeView32");
}

// Get the text of a TreeView item
static std::wstring GetTreeItemText(HWND hTree, HTREEITEM hItem) {
    wchar_t buf[512];
    TVITEMW tvi = {};
    tvi.mask = TVIF_TEXT;
    tvi.hItem = hItem;
    tvi.pszText = buf;
    tvi.cchTextMax = ARRAYSIZE(buf);
    if (SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
        return std::wstring(buf);
    }
    return L"";
}

// Scroll nav tree to the very top and set caret to root
static void ScrollNavToTop(HWND hTree) {
    if (!hTree) return;
    HTREEITEM root = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    if (!root) return;

    // Select root and ensure it’s visible
    SendMessageW(hTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)root);
    SendMessageW(hTree, TVM_ENSUREVISIBLE, 0, (LPARAM)root);
    // Force scroll to very top (extra safety)
    SendMessageW(hTree, WM_VSCROLL, SB_TOP, 0);
}

// Recursively delete or count matching items, return count affected
static int PruneItemsCount(HWND hTree, HTREEITEM hItem, bool deleteItems) {
    if (!hItem) return 0;

    int affected = 0;

    // Store siblings first (because deleting current affects sibling pointers)
    std::vector<HTREEITEM> siblings;
    for (HTREEITEM s = hItem; s != NULL; s = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)s)) {
        siblings.push_back(s);
    }

    for (HTREEITEM s : siblings) {
        // Recurse into children first
        HTREEITEM child = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)s);
        if (child) {
            affected += PruneItemsCount(hTree, child, deleteItems);
        }

        // Check this item's text
        std::wstring text = GetTreeItemText(hTree, s);

        // Built-ins: exact for Home/Gallery, contains for OneDrive (to catch “OneDrive – Personal” etc.)
        bool matchHome     = g_settings.hideHome     && match_string(text, g_settings.homeText,     false, 0);
        bool matchGallery  = g_settings.hideGallery  && match_string(text, g_settings.galleryText,  false, 0);
        bool matchOneDrive = g_settings.hideOneDrive && match_string(text, g_settings.onedriveText, false, 1);

        // Custom list: user-configurable match mode + case sensitivity
        bool matchCustom = false;
        if (g_settings.customEnabled && !g_settings.customTexts.empty()) {
            for (const auto& pat : g_settings.customTexts) {
                if (match_string(text, pat, g_settings.customCaseSensitive, g_settings.customMatchMode)) {
                    matchCustom = true;
                    break;
                }
            }
        }

        if (matchHome || matchGallery || matchOneDrive || matchCustom) {
            if (deleteItems) {
                SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)s);
            }
            ++affected;
            // (No need to process its children further; deletion removes the subtree)
        }
    }

    return affected;
}

// Remove items from a given TreeView, return true if anything changed
static bool RemoveHomeAndGallery(HWND hTree) {
    if (!hTree) return false;

    HTREEITEM root = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    if (!root) return false;

    // First do a dry-run to check if anything would be removed.
    int wouldDelete = PruneItemsCount(hTree, root, false);
    if (wouldDelete <= 0) {
        // Nothing to do; avoid touching redraw or scroll = no flash
        return false;
    }

    // Freeze redraw to avoid flicker and unwanted auto-scroll during deletions
    SendMessageW(hTree, WM_SETREDRAW, FALSE, 0);
    int deleted = PruneItemsCount(hTree, root, true);
    SendMessageW(hTree, WM_SETREDRAW, TRUE, 0);

    if (deleted > 0) {
        // After modifying the tree, reposition to top
        ScrollNavToTop(hTree);
        InvalidateRect(hTree, nullptr, TRUE);
        UpdateWindow(hTree);
        Wh_Log(L"[HideHG] Pruned %d items and reset nav pane to top", deleted);
        return true;
    }

    return false;
}

// ---------------- Windhawk integration ----------------

static void LoadSettings() {
    // Items
    g_settings.hideHome     = !!Wh_GetIntSetting(L"hide.home");
    g_settings.hideGallery  = !!Wh_GetIntSetting(L"hide.gallery");
    g_settings.hideOneDrive = !!Wh_GetIntSetting(L"hide.onedrive");

    {
        PCWSTR s = Wh_GetStringSetting(L"hide.homeText");
        g_settings.homeText = s && *s ? s : L"Home";
        Wh_FreeStringSetting(s);
    }
    {
        PCWSTR s = Wh_GetStringSetting(L"hide.galleryText");
        g_settings.galleryText = s && *s ? s : L"Gallery";
        Wh_FreeStringSetting(s);
    }
    {
        PCWSTR s = Wh_GetStringSetting(L"hide.onedriveText");
        g_settings.onedriveText = s && *s ? s : L"OneDrive";
        Wh_FreeStringSetting(s);
    }

    // Custom options
    g_settings.customEnabled = !!Wh_GetIntSetting(L"hide.customEnabled");
    g_settings.customCaseSensitive = !!Wh_GetIntSetting(L"hide.caseSensitive");

    // matchMode: exact|contains|startsWith
    g_settings.customMatchMode = 0; // default exact
    if (PCWSTR s = Wh_GetStringSetting(L"hide.matchMode")) {
        if (s && *s) {
            if (wcscmp(s, L"contains") == 0) g_settings.customMatchMode = 1;
            else if (wcscmp(s, L"startsWith") == 0) g_settings.customMatchMode = 2;
        }
        Wh_FreeStringSetting(s);
    }

    g_settings.customTexts.clear();
    if (PCWSTR s1 = Wh_GetStringSetting(L"hide.customText")) {
        std::wstring t = trim(s1 ? s1 : L"");
        if (!t.empty()) g_settings.customTexts.push_back(t);
        Wh_FreeStringSetting(s1);
    }
    if (PCWSTR s2 = Wh_GetStringSetting(L"hide.customList")) {
        std::wstring lst = s2 ? s2 : L"";
        auto items = split_multi(lst);
        for (auto& it : items) g_settings.customTexts.push_back(it);
        Wh_FreeStringSetting(s2);
    }

    // Timing
    int interval = Wh_GetIntSetting(L"timing.scanIntervalMs");
    if (interval <= 0) {
        // Backward-compat: accept old key if present
        interval = Wh_GetIntSetting(L"hide.scanIntervalMs");
    }
    // Keep it responsive by default; clamp to sane bounds
    if (interval < 100) interval = 300;
    if (interval > 10000) interval = 10000;
    g_settings.scanIntervalMs = interval;

    int initDelay = Wh_GetIntSetting(L"timing.initialDelayMs");
    if (initDelay < 0) initDelay = 0;
    if (initDelay > 5000) initDelay = 5000;
    g_settings.initialDelayMs = initDelay;

    Wh_Log(L"[HideHG] Settings: hideHome=%d hideGallery=%d hideOneDrive=%d customEnabled=%d customCount=%d matchMode=%d caseSensitive=%d scanInterval=%dms initialDelay=%dms",
        g_settings.hideHome, g_settings.hideGallery, g_settings.hideOneDrive,
        g_settings.customEnabled, (int)g_settings.customTexts.size(),
        g_settings.customMatchMode, g_settings.customCaseSensitive,
        g_settings.scanIntervalMs, g_settings.initialDelayMs);
}

// Worker thread: handle first-time open cleanly, then periodic pruning
static DWORD WINAPI WorkerProc(LPVOID) {
    // Ensure comctl is initialized (for TreeView)
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_TREEVIEW_CLASSES };
    InitCommonControlsEx(&icc);

    while (!g_stopWorker) {
        auto explorers = GetExplorerWindows();

        // Clean up closed windows from the tracking sets
        {
            std::vector<HWND> toEraseOnce;
            for (HWND h : g_prunedOnce) {
                if (!IsWindow(h)) toEraseOnce.push_back(h);
            }
            for (HWND h : toEraseOnce) g_prunedOnce.erase(h);

            std::vector<HWND> toEraseSeen;
            for (auto& kv : g_firstSeenTick) {
                if (!IsWindow(kv.first)) toEraseSeen.push_back(kv.first);
            }
            for (HWND h : toEraseSeen) g_firstSeenTick.erase(h);
        }

        ULONGLONG now = GetTickCount64();

        for (HWND ex : explorers) {
            HWND tree = GetExplorerNavTree(ex);
            if (!tree || !IsWindow(tree)) continue;

            // First time we see this window: record the time
            if (g_firstSeenTick.find(ex) == g_firstSeenTick.end()) {
                g_firstSeenTick[ex] = now;
            }

            bool firstPassDone = (g_prunedOnce.find(ex) != g_prunedOnce.end());
            ULONGLONG elapsed = now - g_firstSeenTick[ex];

            if (!firstPassDone) {
                // Optional initial delay to let Explorer finish layout (configurable)
                if ((int)elapsed < g_settings.initialDelayMs) {
                    continue; // wait until delay elapses
                }

                // FIRST SIGHT: pin top, prune, pin top again — prevents scroll-to-bottom at open
                ScrollNavToTop(tree);

                SendMessageW(tree, WM_SETREDRAW, FALSE, 0);
                HTREEITEM root = (HTREEITEM)SendMessageW(tree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
                if (root) (void)PruneItemsCount(tree, root, true);
                SendMessageW(tree, WM_SETREDRAW, TRUE, 0);

                ScrollNavToTop(tree);
                InvalidateRect(tree, nullptr, TRUE);
                UpdateWindow(tree);

                g_prunedOnce.insert(ex);
                Wh_Log(L"[HideHG] Initial prune done for window 0x%p", ex);
            } else {
                // Subsequent scans: prune only if items got re-added; avoid touching scroll when no change
                (void)RemoveHomeAndGallery(tree);
            }
        }

        // Sleep for the configured interval
        int delay = g_settings.scanIntervalMs;
        for (int ms = 0; ms < delay && !g_stopWorker; ms += 50) {
            Sleep(50);
        }
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopWorker = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopWorker = true;
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 4000);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }
    g_prunedOnce.clear();
    g_firstSeenTick.clear();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
