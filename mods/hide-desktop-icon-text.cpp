// ==WindhawkMod==
// @id              hide-desktop-icon-text
// @name            Ultimate Desktop Minimizer (No Text, No Arrows)
// @description     Hides icon names AND shortcut arrow overlays on the desktop. Keeps names for folders.
// @version         1.4.0
// @author          Твоє ім'я
// @github          https://github.com/твій_профіль
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -luxtheme -lole32 -luuid -lshlwapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Hides desktop icon labels (keeping folder names) and removes the shortcut arrow overlay.

### Features
* Hides text labels for files and shortcuts on the desktop; keeps folder names.
* Removes the shortcut arrow overlay by retargeting its image-list slot to a
  transparent image (no registry edit, no admin rights, applies live).
* Text hiding affects only the Desktop.

### Notes
* The shortcut arrow is removed for the whole running Explorer session, not just
  the desktop, because the overlay lives in the shared system image list.
* Disabling the mod restores the arrow only after Explorer is restarted or its
  icon cache refreshes — there is no API to read back the original overlay image.

### Known limitation
Folder detection matches the drawn text against actual desktop folder names. If a *file* has
exactly the same name as a *folder* on the desktop, that file's label will also be kept.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <uxtheme.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#include <string>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// Set only while the desktop SysListView32 is painting (per-thread).
thread_local bool g_isDrawingDesktop = false;

// Cache of folder display names on the desktop. Touched only by the desktop
// UI thread, so no extra locking is required.
std::unordered_set<std::wstring> g_folderNames;
DWORD g_lastUpdateTick = 0;

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

static HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

// True if hWnd is the desktop's "FolderView" SysListView32 in this process.
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

// Returns the desktop FolderView belonging to the current process, or null.
static HWND GetDesktopFolderView() {
    HWND lv = FindChild(FindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                        L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ---------------------------------------------------------------------------
// Folder name cache (so folder labels stay visible)
// ---------------------------------------------------------------------------

static void UpdateFolderCache() {
    DWORD now = GetTickCount();
    if (g_lastUpdateTick != 0 && now - g_lastUpdateTick < 1000) return;
    g_lastUpdateTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(
                nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM;
                if (SUCCEEDED(pDesktop->GetAttributesOf(
                        1, (LPCITEMIDLIST*)&pidl, &attr)) &&
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
    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    return g_folderNames.count(s) == 0;  // hide everything that is not a folder
}

// ---------------------------------------------------------------------------
// Subclass: marks the WM_PAINT window of the desktop listview
// ---------------------------------------------------------------------------

LRESULT CALLBACK DesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT) {
        UpdateFolderCache();          // refresh once per paint, not per label
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

// --- text (classic) ---
using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_isDrawingDesktop && lpchText && ShouldHideLabel(lpchText, cchText)) {
        return DrawTextW_Original(hdc, L"", 0, lprc, format);
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

// --- text (themed, with shadow) ---
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);
DrawThemeTextEx_t DrawThemeTextEx_Original;
HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                    LPRECT pRect, const DTTOPTS* pOptions) {
    if (g_isDrawingDesktop && pszText && ShouldHideLabel(pszText, cchText)) {
        return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                        dwTextFlags, pRect, pOptions);
    }
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                    dwTextFlags, pRect, pOptions);
}

// ---------------------------------------------------------------------------
// Shortcut arrow removal
//
// The shortcut arrow is a standard icon overlay living in the system image
// list. Instead of fighting the draw path, we point the link-overlay slot at a
// fully transparent image, so every shortcut renders with an invisible arrow.
// This is in-process only (reverts when Explorer next restarts) and needs no
// registry write or admin rights.
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

// Builds a fully transparent 32-bpp icon of the given size.
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
        memset(bits, 0, (size_t)cx * cy * 4);   // alpha = 0 everywhere => transparent
    }

    // The overlay is composited via the AND mask, so it must be all 1s
    // (fully transparent) — an all-0 mask renders as an opaque black square.
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

// Reassigns the shortcut overlay slot to a transparent image in every system
// image list size, so shortcut arrows become invisible live.
static void HideShortcutOverlay() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    if (linkOverlay <= 0) {
        Wh_Log(L"Could not resolve link overlay index (%d).", linkOverlay);
        return;
    }

    // IID_IImageList {46EB5926-582E-4017-9FDF-E8998DAA0950} — defined locally so we
    // don't depend on uuid.lib, which this toolchain doesn't link by default.
    static const GUID iidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { SHIL_SMALL, SHIL_LARGE, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO };
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
// Hook to catch the desktop listview when it (re)appears (for text hiding)
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

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (DrawTextW_t)GetProcAddress(hUser32, "DrawTextW"))
            WindhawkUtils::SetFunctionHook(p, DrawTextW_Hook, &DrawTextW_Original);
    }

    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (DrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx"))
            WindhawkUtils::SetFunctionHook(p, DrawThemeTextEx_Hook, &DrawThemeTextEx_Original);
    }

    // Make shortcut arrows invisible by retargeting the link-overlay slot.
    HideShortcutOverlay();

    return TRUE;
}

void Wh_ModAfterInit() {
    // Flush the icon cache so icons that were already composited with the arrow
    // get re-composited with the now-transparent overlay — no Explorer restart.
    // This does NOT re-read the registry, so it won't bring the arrow back.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    if (HWND hDesktop = GetDesktopFolderView()) {
        // ModAfterInit runs on a Windhawk thread, so the cross-thread-safe variant
        // is required — this is what makes the mod apply WITHOUT restarting Explorer.
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass, 0);

        // Force a full repaint so hidden labels and the now-transparent shortcut
        // overlay take effect immediately, without restarting Explorer.
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
        // Cross-thread-safe removal, so we never leave a dangling subclass
        // pointing into the about-to-be-unloaded mod DLL.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}
