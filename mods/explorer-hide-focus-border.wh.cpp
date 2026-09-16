// ==WindhawkMod==
// @id           explorer-hide-focus-border
// @name         File Explorer Focus Border Remover
// @name:vi-VN   Xóa viền focus trong File Explorer
// @description  Removes the white focus border around selected items in File Explorer while preserving the normal selection highlight.
// @description:vi-VN Xóa viền focus màu trắng quanh mục đang chọn trong File Explorer nhưng vẫn giữ nền chọn bình thường.
// @version      1.0.0
// @author       Hùng
// @github       https://github.com/NoMorePlz
// @include      explorer.exe
// @architecture x86-64
// @license      MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Focus Border Remover

Removes the white rectangular focus border around selected files, folders and
drives in File Explorer while keeping the normal selection highlight intact.

The mod is intentionally scoped to File Explorer folder windows. It does not
change the desktop selection rectangle and doesn't modify selection colors.

## Compatibility mode

The default setting only tracks `ItemsView` theme handles that are associated
with File Explorer folder windows (`CabinetWClass`). This is the safest mode.

If the border is still visible on a particular Windows build, turn off
**Strict File Explorer window detection** and restart Explorer once. In that
mode, every `ItemsView` theme handle inside `explorer.exe` can be affected.

## Notes

- The implementation relies on an undocumented File Explorer theme detail:
  `ItemsView`, part 3, states 1 and 2 are used for the focus border on current
  Windows builds. A future Windows update may change this.
- After installing the mod, restart Windows Explorer once if an already-open
  Explorer window still shows the border. Existing theme handles can predate
  the hooks and therefore won't be tracked until recreated.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- strictExplorerWindowDetection: true
  $name: Strict File Explorer window detection
  $description: Restrict the change to File Explorer folder windows (CabinetWClass). Turn this off only if the border is still visible, then restart Explorer once.
  $name:vi-VN: Chỉ nhận diện cửa sổ File Explorer
  $description:vi-VN: Chỉ áp dụng cho cửa sổ thư mục File Explorer (CabinetWClass). Chỉ tắt mục này nếu viền vẫn còn, sau đó khởi động lại Explorer một lần.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>

#include <cwchar>
#include <unordered_set>

struct Settings {
    bool strictExplorerWindowDetection;
};

Settings g_settings;

SRWLOCK g_themeLock = SRWLOCK_INIT;
std::unordered_set<HTHEME> g_itemsViewThemes;

HMODULE g_uxthemeModule = nullptr;
bool g_loadedUxTheme = false;

using OpenThemeData_t = HTHEME(WINAPI*)(HWND, LPCWSTR);
using OpenThemeDataEx_t = HTHEME(WINAPI*)(HWND, LPCWSTR, DWORD);
using OpenThemeDataForDpi_t = HTHEME(WINAPI*)(HWND, LPCWSTR, UINT);
using CloseThemeData_t = HRESULT(WINAPI*)(HTHEME);
using DrawThemeBackground_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int,
                                               LPCRECT, LPCRECT);
using DrawThemeBackgroundEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int,
                                                 LPCRECT, const DTBGOPTS*);

OpenThemeData_t OpenThemeData_Original = nullptr;
OpenThemeDataEx_t OpenThemeDataEx_Original = nullptr;
OpenThemeDataForDpi_t OpenThemeDataForDpi_Original = nullptr;
CloseThemeData_t CloseThemeData_Original = nullptr;
DrawThemeBackground_t DrawThemeBackground_Original = nullptr;
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original = nullptr;

bool IsCabinetWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        root = hwnd;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"CabinetWClass") == 0;
}

struct ThreadWindowSearchContext {
    bool found = false;
};

BOOL CALLBACK EnumThreadWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<ThreadWindowSearchContext*>(lParam);

    wchar_t className[64] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"CabinetWClass") == 0) {
        context->found = true;
        return FALSE;
    }

    return TRUE;
}

bool IsFileExplorerContext(HWND hwnd) {
    if (!g_settings.strictExplorerWindowDetection) {
        return true;
    }

    if (IsCabinetWindow(hwnd)) {
        return true;
    }

    // DirectUI can occasionally open theme data without a useful HWND.
    // In that case, accept it only if the current GUI thread owns a File
    // Explorer folder window.
    ThreadWindowSearchContext context;
    EnumThreadWindows(GetCurrentThreadId(), EnumThreadWindowsProc,
                      reinterpret_cast<LPARAM>(&context));
    return context.found;
}

bool TokenEndsWithItemsView(const wchar_t* begin, const wchar_t* end) {
    while (begin < end && (*begin == L' ' || *begin == L'\t')) {
        ++begin;
    }
    while (end > begin && (end[-1] == L' ' || end[-1] == L'\t')) {
        --end;
    }

    // Theme class lists can contain namespace-like prefixes, e.g.
    // "Explorer::ItemsView". Compare only the final token.
    const wchar_t* tail = begin;
    for (const wchar_t* p = begin; p + 1 < end; ++p) {
        if (p[0] == L':' && p[1] == L':') {
            tail = p + 2;
            ++p;
        }
    }

    constexpr wchar_t kItemsView[] = L"ItemsView";
    constexpr size_t kItemsViewLen = ARRAYSIZE(kItemsView) - 1;
    const size_t tailLen = static_cast<size_t>(end - tail);

    return tailLen == kItemsViewLen &&
           _wcsnicmp(tail, kItemsView, kItemsViewLen) == 0;
}

bool IsItemsViewClassList(LPCWSTR classList) {
    if (!classList || !*classList) {
        return false;
    }

    const wchar_t* p = classList;
    while (*p) {
        const wchar_t* end = wcschr(p, L';');
        if (!end) {
            end = p + wcslen(p);
        }

        if (TokenEndsWithItemsView(p, end)) {
            return true;
        }

        if (*end == L'\0') {
            break;
        }
        p = end + 1;
    }

    return false;
}

void TrackTheme(HTHEME hTheme) {
    if (!hTheme) {
        return;
    }

    AcquireSRWLockExclusive(&g_themeLock);
    g_itemsViewThemes.insert(hTheme);
    ReleaseSRWLockExclusive(&g_themeLock);
}

void UntrackTheme(HTHEME hTheme) {
    if (!hTheme) {
        return;
    }

    AcquireSRWLockExclusive(&g_themeLock);
    g_itemsViewThemes.erase(hTheme);
    ReleaseSRWLockExclusive(&g_themeLock);
}

bool IsTrackedItemsViewTheme(HTHEME hTheme) {
    AcquireSRWLockShared(&g_themeLock);
    const bool result = g_itemsViewThemes.find(hTheme) != g_itemsViewThemes.end();
    ReleaseSRWLockShared(&g_themeLock);
    return result;
}

void ClearTrackedThemes() {
    AcquireSRWLockExclusive(&g_themeLock);
    g_itemsViewThemes.clear();
    ReleaseSRWLockExclusive(&g_themeLock);
}

void MaybeTrackTheme(HTHEME hTheme, HWND hwnd, LPCWSTR classList) {
    if (!hTheme || !IsItemsViewClassList(classList) ||
        !IsFileExplorerContext(hwnd)) {
        return;
    }

    TrackTheme(hTheme);
    Wh_Log(L"Tracking ItemsView theme handle %p", hTheme);
}

HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR pszClassList) {
    HTHEME hTheme = OpenThemeData_Original(hwnd, pszClassList);
    MaybeTrackTheme(hTheme, hwnd, pszClassList);
    return hTheme;
}

HTHEME WINAPI OpenThemeDataEx_Hook(HWND hwnd,
                                   LPCWSTR pszClassList,
                                   DWORD dwFlags) {
    HTHEME hTheme = OpenThemeDataEx_Original(hwnd, pszClassList, dwFlags);
    MaybeTrackTheme(hTheme, hwnd, pszClassList);
    return hTheme;
}

HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd,
                                       LPCWSTR pszClassList,
                                       UINT dpi) {
    HTHEME hTheme = OpenThemeDataForDpi_Original(hwnd, pszClassList, dpi);
    MaybeTrackTheme(hTheme, hwnd, pszClassList);
    return hTheme;
}

HRESULT WINAPI CloseThemeData_Hook(HTHEME hTheme) {
    UntrackTheme(hTheme);
    return CloseThemeData_Original(hTheme);
}

bool ShouldSuppressFocusBorder(HTHEME hTheme, int partId, int stateId) {
    // Current File Explorer DirectUI ItemsView behavior:
    //   part 3, state 1 = normal focus border
    //   part 3, state 2 = hot focus border
    // The selection fill is painted separately, so suppressing this part keeps
    // the normal selected-item highlight intact.
    return partId == 3 && (stateId == 1 || stateId == 2) &&
           IsTrackedItemsViewTheme(hTheme);
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    if (ShouldSuppressFocusBorder(hTheme, iPartId, iStateId)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId,
                                        pRect, pClipRect);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME hTheme,
                                           HDC hdc,
                                           int iPartId,
                                           int iStateId,
                                           LPCRECT pRect,
                                           const DTBGOPTS* pOptions) {
    if (ShouldSuppressFocusBorder(hTheme, iPartId, iStateId)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId,
                                          pRect, pOptions);
}

void LoadSettings() {
    g_settings.strictExplorerWindowDetection =
        Wh_GetIntSetting(L"strictExplorerWindowDetection") != 0;
}

bool HookExport(HMODULE module,
                const char* name,
                void* hook,
                void** original,
                bool required = true) {
    void* target = reinterpret_cast<void*>(GetProcAddress(module, name));
    if (!target) {
        if (required) {
            Wh_Log(L"Required uxtheme export not found: %S", name);
            return false;
        }
        return true;
    }

    if (!Wh_SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Failed to hook uxtheme export: %S", name);
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing File Explorer Focus Border Remover");
    LoadSettings();

    g_uxthemeModule = GetModuleHandleW(L"uxtheme.dll");
    if (!g_uxthemeModule) {
        g_uxthemeModule = LoadLibraryW(L"uxtheme.dll");
        g_loadedUxTheme = g_uxthemeModule != nullptr;
    }

    if (!g_uxthemeModule) {
        Wh_Log(L"Couldn't load uxtheme.dll");
        return FALSE;
    }

    if (!HookExport(g_uxthemeModule, "OpenThemeData",
                    reinterpret_cast<void*>(OpenThemeData_Hook),
                    reinterpret_cast<void**>(&OpenThemeData_Original)) ||
        !HookExport(g_uxthemeModule, "OpenThemeDataEx",
                    reinterpret_cast<void*>(OpenThemeDataEx_Hook),
                    reinterpret_cast<void**>(&OpenThemeDataEx_Original), false) ||
        !HookExport(g_uxthemeModule, "OpenThemeDataForDpi",
                    reinterpret_cast<void*>(OpenThemeDataForDpi_Hook),
                    reinterpret_cast<void**>(&OpenThemeDataForDpi_Original), false) ||
        !HookExport(g_uxthemeModule, "CloseThemeData",
                    reinterpret_cast<void*>(CloseThemeData_Hook),
                    reinterpret_cast<void**>(&CloseThemeData_Original)) ||
        !HookExport(g_uxthemeModule, "DrawThemeBackground",
                    reinterpret_cast<void*>(DrawThemeBackground_Hook),
                    reinterpret_cast<void**>(&DrawThemeBackground_Original)) ||
        !HookExport(g_uxthemeModule, "DrawThemeBackgroundEx",
                    reinterpret_cast<void*>(DrawThemeBackgroundEx_Hook),
                    reinterpret_cast<void**>(&DrawThemeBackgroundEx_Original))) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ClearTrackedThemes();

    Wh_Log(L"Settings changed. Restart Explorer once to recreate theme handles.");
}

void Wh_ModUninit() {
    ClearTrackedThemes();

    if (g_loadedUxTheme && g_uxthemeModule) {
        FreeLibrary(g_uxthemeModule);
        g_uxthemeModule = nullptr;
        g_loadedUxTheme = false;
    }

    Wh_Log(L"Uninitialized File Explorer Focus Border Remover");
}
