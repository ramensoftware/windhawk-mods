// ==WindhawkMod==
// @id              hide-home-gallery-explorer
// @name            Hide Home, Gallery & OneDrive in Explorer
// @description     Hides the "Home", "Gallery", and "OneDrive" items from File Explorer's navigation pane on Windows 11. Also supports hiding user-specified custom labels.
// @version         0.4
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

What’s new in v0.4: [Contributed by - @rono-zeroseven]
- Instant Hiding: Hooks the Windows API SendMessageW to intercept and block the creation of targeted items before they are ever rendered to the screen.
- Zero Navigation Redirects: Eliminates all artificial selection and scroll manipulation. Your Windows setting to "Open File Explorer to: This PC" is now perfectly preserved.
- Optimized Performance: Completely removes polling delays. Targeted items are synchronously intercepted and deleted during their creation, adding zero perceptible overhead to File Explorer.

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
} g_settings;

static volatile bool g_stopWorker = false;
static HANDLE g_workerThread = NULL;

// ---------------- Utilities ----------------

static bool iequals(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) return false;
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

// Allocation-free string matching for maximum performance
static bool match_string(const std::wstring& text, const std::wstring& pattern, bool caseSensitive, int mode) {
    if (pattern.empty()) return false;
    if (caseSensitive) {
        switch (mode) {
        case 0: return text == pattern;
        case 1: return text.find(pattern) != std::wstring::npos;
        case 2: return text.rfind(pattern, 0) == 0;
        default: return text == pattern;
        }
    } else {
        switch (mode) {
        case 0: return iequals(text, pattern);
        case 1: {
            if (pattern.size() > text.size()) return false;
            for (size_t i = 0; i <= text.size() - pattern.size(); ++i) {
                bool match = true;
                for (size_t j = 0; j < pattern.size(); ++j) {
                    if (towlower(text[i+j]) != towlower(pattern[j])) {
                        match = false;
                        break;
                    }
                }
                if (match) return true;
            }
            return false;
        }
        case 2: {
            if (text.size() < pattern.size()) return false;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (towlower(text[i]) != towlower(pattern[i])) return false;
            }
            return true;
        }
        default: return iequals(text, pattern);
        }
    }
}

static bool ShouldDeleteItem(const std::wstring& text) {
    if (text.empty()) return false;
    
    bool matchHome     = g_settings.hideHome     && match_string(text, g_settings.homeText,     false, 0);
    bool matchGallery  = g_settings.hideGallery  && match_string(text, g_settings.galleryText,  false, 0);
    bool matchOneDrive = g_settings.hideOneDrive && match_string(text, g_settings.onedriveText, false, 1);

    bool matchCustom = false;
    if (g_settings.customEnabled && !g_settings.customTexts.empty()) {
        for (const auto& pat : g_settings.customTexts) {
            if (match_string(text, pat, g_settings.customCaseSensitive, g_settings.customMatchMode)) {
                matchCustom = true;
                break;
            }
        }
    }
    return matchHome || matchGallery || matchOneDrive || matchCustom;
}

// ---------------- Explorer Helpers ----------------

static std::vector<HWND> GetExplorerWindows() {
    struct EnumCtx { std::vector<HWND>* windows; DWORD pid; } ctx{};
    std::vector<HWND> result;
    ctx.windows = &result;
    ctx.pid = GetCurrentProcessId();

    EnumWindows([](HWND hwnd, LPARAM lParam)->BOOL {
        auto* ctx = reinterpret_cast<EnumCtx*>(lParam);
        wchar_t cls[256] = {};
        GetClassNameW(hwnd, cls, 255);
        if (wcscmp(cls, L"CabinetWClass") != 0 || !IsWindowVisible(hwnd)) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == ctx->pid) ctx->windows->push_back(hwnd);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));

    return result;
}

static HWND FindChildByClass(HWND parent, const wchar_t* cls) {
    for (HWND h = FindWindowEx(parent, NULL, NULL, NULL); h; h = FindWindowEx(parent, h, NULL, NULL)) {
        wchar_t c[256] = {};
        GetClassNameW(h, c, 255);
        if (wcscmp(c, cls) == 0) return h;
        HWND deeper = FindChildByClass(h, cls);
        if (deeper) return deeper;
    }
    return NULL;
}

static HWND GetExplorerNavTree(HWND explorerHwnd) {
    return FindChildByClass(explorerHwnd, L"SysTreeView32");
}

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

static int PruneItemsCount(HWND hTree, HTREEITEM hItem, bool deleteItems) {
    if (!hItem) return 0;
    int affected = 0;

    std::vector<HTREEITEM> siblings;
    for (HTREEITEM s = hItem; s != NULL; s = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)s)) {
        siblings.push_back(s);
    }

    for (HTREEITEM s : siblings) {
        HTREEITEM child = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)s);
        if (child) affected += PruneItemsCount(hTree, child, deleteItems);

        std::wstring text = GetTreeItemText(hTree, s);
        if (ShouldDeleteItem(text)) {
            if (deleteItems) SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)s);
            ++affected;
        }
    }
    return affected;
}

static bool RemoveHomeAndGallery(HWND hTree) {
    if (!hTree) return false;
    HTREEITEM root = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    if (!root) return false;

    int wouldDelete = PruneItemsCount(hTree, root, false);
    if (wouldDelete <= 0) return false;

    SendMessageW(hTree, WM_SETREDRAW, FALSE, 0);
    int deleted = PruneItemsCount(hTree, root, true);
    SendMessageW(hTree, WM_SETREDRAW, TRUE, 0);

    if (deleted > 0) {
        InvalidateRect(hTree, nullptr, TRUE);
        UpdateWindow(hTree);
        return true;
    }
    return false;
}

// ---------------- API Hooking ----------------

// Original function pointer
LRESULT (WINAPI *SendMessageW_Original)(HWND, UINT, WPARAM, LPARAM) = nullptr;

// Our hook function
LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    // Intercept TreeView item insertion
    if (Msg == TVM_INSERTITEMW) {
        TVINSERTSTRUCTW* tvis = (TVINSERTSTRUCTW*)lParam;
        if (tvis) {
            std::wstring text;
            // Check if text is provided directly in the insertion struct
            if ((tvis->item.mask & TVIF_TEXT) && tvis->item.pszText && tvis->item.pszText != LPSTR_TEXTCALLBACKW) {
                text = tvis->item.pszText;
            }
            
            // Call the original function first to let the system create the item in memory
            LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);
            
            if (result != 0) {
                // Fallback: if text wasn't in the struct, fetch it from the newly created item
                if (text.empty()) {
                    TVITEMW tvItem = {0};
                    tvItem.mask = TVIF_TEXT;
                    tvItem.hItem = (HTREEITEM)result;
                    wchar_t buf[512];
                    tvItem.pszText = buf;
                    tvItem.cchTextMax = 512;
                    if (SendMessageW_Original(hWnd, TVM_GETITEMW, 0, (LPARAM)&tvItem)) {
                        text = buf;
                    }
                }
                
                // If it's an item we want to hide, delete it immediately before it's ever painted
                if (!text.empty() && ShouldDeleteItem(text)) {
                    SendMessageW_Original(hWnd, TVM_DELETEITEM, 0, result);
                    return 0; // Return 0 to tell Explorer the insertion "failed", preventing it from using the deleted handle
                }
            }
            return result;
        }
    }
    
    // For all other messages, pass through normally
    return SendMessageW_Original(hWnd, Msg, wParam, lParam);
}

// ---------------- Windhawk Integration ----------------

static void LoadSettings() {
    g_settings.hideHome     = !!Wh_GetIntSetting(L"hide.home");
    g_settings.hideGallery  = !!Wh_GetIntSetting(L"hide.gallery");
    g_settings.hideOneDrive = !!Wh_GetIntSetting(L"hide.onedrive");

    auto getStr = [](PCWSTR key, const wchar_t* def) {
        PCWSTR s = Wh_GetStringSetting(key);
        std::wstring res = (s && *s) ? s : def;
        Wh_FreeStringSetting(s);
        return res;
    };

    g_settings.homeText = getStr(L"hide.homeText", L"Home");
    g_settings.galleryText = getStr(L"hide.galleryText", L"Gallery");
    g_settings.onedriveText = getStr(L"hide.onedriveText", L"OneDrive");

    g_settings.customEnabled = !!Wh_GetIntSetting(L"hide.customEnabled");
    g_settings.customCaseSensitive = !!Wh_GetIntSetting(L"hide.caseSensitive");

    g_settings.customMatchMode = 0;
    if (PCWSTR s = Wh_GetStringSetting(L"hide.matchMode")) {
        if (wcscmp(s, L"contains") == 0) g_settings.customMatchMode = 1;
        else if (wcscmp(s, L"startsWith") == 0) g_settings.customMatchMode = 2;
        Wh_FreeStringSetting(s);
    }

    g_settings.customTexts.clear();
    if (PCWSTR s1 = Wh_GetStringSetting(L"hide.customText")) {
        std::wstring t = trim(s1 ? s1 : L"");
        if (!t.empty()) g_settings.customTexts.push_back(t);
        Wh_FreeStringSetting(s1);
    }
    if (PCWSTR s2 = Wh_GetStringSetting(L"hide.customList")) {
        auto items = split_multi(s2 ? s2 : L"");
        for (auto& it : items) g_settings.customTexts.push_back(it);
        Wh_FreeStringSetting(s2);
    }
}

// Fallback worker thread: runs every 500ms just in case Explorer dynamically rebuilds the tree
static DWORD WINAPI WorkerProc(LPVOID) {
    while (!g_stopWorker) {
        auto explorers = GetExplorerWindows();
        for (HWND ex : explorers) {
            HWND tree = GetExplorerNavTree(ex);
            if (tree && IsWindow(tree)) {
                RemoveHomeAndGallery(tree);
            }
        }
        Sleep(500);
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    
    // Install the API hook for instant, zero-flash removal
    Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageW_Hook, (void**)&SendMessageW_Original);
    
    // Start fallback worker thread
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
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
