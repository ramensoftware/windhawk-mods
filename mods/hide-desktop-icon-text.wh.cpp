// ==WindhawkMod==
// @id              hide-desktop-icon-text
// @name            Hide Desktop Icon Text and Shortcut Arrows
// @description     Provides options to hide icon text labels (keeping folder names) and remove shortcut arrow overlays on the desktop.
// @version         1.5.0
// @author          kivsak
// @github          https://github.com/kivsak
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Provides options to hide desktop icon labels (keeping folder names) and remove the shortcut arrow overlay.

### Features
* **Hide Icon Text**: Hides text labels for files and shortcuts on the desktop while preserving folder names.
* **Hide Shortcut Arrows**: Removes the standard shortcut arrow overlay live without requiring registry edits or administrator rights.

### Notes
* The shortcut arrow removal affects the entire running Explorer session because the overlay resides in the shared system image list.
* Disabling the shortcut arrow feature (or the mod itself) restores the arrows only after Explorer restarts or the icon cache refreshes.

### Known limitation
Folder detection matches the drawn text against actual desktop folder names. If a *file* has
exactly the same name as a *folder* on the desktop, that file's label will also be kept.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hide_text: true
  $name: Hide desktop icon text
  $description: Hides text labels for files and shortcuts on the desktop (keeps folder names).
- hide_arrows: true
  $name: Remove shortcut arrows
  $description: Removes the shortcut arrow overlay from icons (applies to the whole Explorer session).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#include <string>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// Settings & State
// ---------------------------------------------------------------------------

struct {
    bool hide_text;
    bool hide_arrows;
} g_settings;

// Set only while the desktop SysListView32 is painting (per-thread).
thread_local bool g_isDrawingDesktop = false;

// Cache of folder display names on the desktop.
std::unordered_set<std::wstring> g_folderNames;
DWORD g_lastUpdateTick = 0;

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

static HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

static bool IsDesktopFolderView(HWND hWnd) {
    WCHAR buf[64];

    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"FolderView")) return false;

    HWND defView = GetAncestor(hWnd, GA_PARENT);
    if (!defView) return false;
    if (!GetClassName(defView, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SHELLDLL_DefView")) return false;

    HWND top = GetAncestor(defView, GA_PARENT);
    if (!top) return false;
    if ((!GetClassName(top, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"Progman")) &&
        top != GetShellWindow()) {
        return false;
    }
    return true;
}

static HWND GetDesktopFolderView() {
    HWND lv = FindChild(FindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                        L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ---------------------------------------------------------------------------
// Folder name cache
// ---------------------------------------------------------------------------

static void UpdateFolderCache() {
    DWORD now = GetTickCount();
    if (g_lastUpdateTick != 0 && now - g_lastUpdateTick < 1000) return;
    g_lastUpdateTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM;
                if (SUCCEEDED(pDesktop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attr)) &&
                    (attr & SFGAO_FOLDER) && !(attr & SFGAO_STREAM)) {
                    STRRET str;
                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str))) {
                        WCHAR name[MAX_PATH];
                        if (SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH))) {
                            fresh.insert(name);
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    g_folderNames = std::move(fresh);
}

static inline bool ShouldHideLabel(LPCWSTR text, int cch) {
    if (!g_settings.hide_text) return false;
    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    return g_folderNames.count(s) == 0;
}

// ---------------------------------------------------------------------------
// Subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK DesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT && g_settings.hide_text) {
        UpdateFolderCache();
        g_isDrawingDesktop = true;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_isDrawingDesktop = false;
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_isDrawingDesktop && lpchText && ShouldHideLabel(lpchText, cchText)) {
        return DrawTextW_Original(hdc, L"", 0, lprc, format);
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using DrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const void*);
DrawThemeTextEx_t DrawThemeTextEx_Original;
HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                    LPRECT pRect, const void* pOptions) {
    if (g_isDrawingDesktop && pszText && ShouldHideLabel(pszText, cchText)) {
        return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                        dwTextFlags, pRect, pOptions);
    }
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                    dwTextFlags, pRect, pOptions);
}

// ---------------------------------------------------------------------------
// Shortcut arrow removal
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

static HICON CreateTransparentIcon(int cx, int cy) {
    if (cx <= 0 || cy <= 0) return nullptr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = cx;
    bi.bV5Height      = cy;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hColor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor) return nullptr;

    if (bits) {
        memset(bits, 0, (size_t)cx * cy * 4);
    }

    int rowBytes = ((cx + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * cy, 0xFF);
    HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, maskBits.data());

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask  = hMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask) DeleteObject(hMask);
    return hIcon;
}

static void HideShortcutOverlay() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    if (linkOverlay <= 0) return;

    static const GUID iidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, iidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = CreateTransparentIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    piml->SetOverlayImage(idx, linkOverlay);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// Main Hooks
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         X, Y, nWidth, nHeight, hWndParent, hMenu,
                                         hInstance, lpParam);
    if (hWnd && IsDesktopFolderView(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListSubclass, 0);
    }
    return hWnd;
}

void LoadSettings() {
    g_settings.hide_text = Wh_GetIntSetting(L"hide_text") != 0;
    g_settings.hide_arrows = Wh_GetIntSetting(L"hide_arrows") != 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);

    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (DrawTextW_t)GetProcAddress(hUser32, "DrawTextW"))
            WindhawkUtils::SetFunctionHook(p, DrawTextW_Hook, &DrawTextW_Original);
    }

    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (DrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx"))
            WindhawkUtils::SetFunctionHook(p, DrawThemeTextEx_Hook, &DrawThemeTextEx_Original);
    }

    if (g_settings.hide_arrows) {
        HideShortcutOverlay();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_settings.hide_arrows) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass, 0);
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) {
            ListView_RedrawItems(hDesktop, 0, count - 1);
        }
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

void Wh_ModUninit() {
    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
