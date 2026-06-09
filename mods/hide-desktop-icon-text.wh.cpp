// ==WindhawkMod==
// @id            hide-desktop-icon-text
// @name          Hide Desktop Icon Text and Shortcut Arrows
// @description   Provides advanced options to independently hide text labels for apps, files, and folders, plus flawlessly removes shortcut arrows and the UAC shield.
// @version       2.5.0
// @author        kivsak
// @github        https://github.com/kivsak
// @include       explorer.exe
// @architecture  x86-64
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
👉 PLEASE BUY ME A CHEESE SANDWICH: https://www.patreon.com/c/kivsak

Tired of the stiff, boring default text labels cluttering your beautiful wallpaper? Inspired by minimalist aesthetic setups and clean UI design, this cinema-grade mod completely overhauls how your desktop icons are displayed.

Unlike older, buggy alternatives, this mod features a completely rewritten memory-based alpha-channel engine that guarantees **zero visual artifacts** — completely destroying the infamous Windows 11 "black square" bug while perfectly preserving drop shadows and modern rounded corners!

🌟 **Features**
* **Triple-Threat Text Engine**: Granular control! Hide text labels independently for **Programs/Games**, **Regular Files**, and **Folders**.
* **Smart Detection**: Dynamically scans your desktop structure to automatically differentiate between a shortcut to an app and a simple document.
* **Flawless Overlays Removal**: Deletes those tiny blue shortcut arrows AND the yellow/blue UAC Admin Shields live, using a multi-layer draw intercept engine for full Win 11 compatibility.

### Notes
* The overlay removal affects the entire running Explorer session. Disabling the feature restores them only after Explorer restarts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hide_programs: true
  $name: Hide Programs & Games
  $description: Removes text labels from executable shortcuts and apps (.exe, .lnk, .url).
- hide_files: true
  $name: Hide Regular Files
  $description: Removes text labels from documents, images, and other regular files.
- hide_folders: false
  $name: Hide Folders
  $description: Removes text labels from folders.
- hide_arrows: true
  $name: Hide Shortcut Arrows
  $description: Removes the small shortcut arrow overlay from icons (Zero black squares!).
- hide_shield: true
  $name: Hide UAC Shield Overlay
  $description: Removes the yellow and blue administrator shield from shortcuts (Win 11 Compatible).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// Settings & State
// ---------------------------------------------------------------------------

struct {
    bool hide_programs;
    bool hide_files;
    bool hide_folders;
    bool hide_arrows;
    bool hide_shield;
} g_settings;

// Set only while the desktop SysListView32 is painting (per-thread).
thread_local bool g_isDrawingDesktop = false;

// Overlay tracking
int g_linkOverlay = -1;
std::vector<int> g_shieldOverlays;
int g_shieldSysIdx = -1;

// Item categories
enum class ItemType { Folder, Program, File };

// Cache of item types on the desktop.
std::unordered_map<std::wstring, ItemType> g_itemTypes;
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

    std::unordered_map<std::wstring, ItemType> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_LINK;
                if (SUCCEEDED(pDesktop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attr))) {
                    STRRET str;
                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str))) {
                        WCHAR name[MAX_PATH];
                        if (SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH))) {

                            ItemType type = ItemType::File;

                            if ((attr & SFGAO_FOLDER) && !(attr & SFGAO_STREAM)) {
                                type = ItemType::Folder;
                            } else {
                                bool isProgram = false;
                                if (attr & SFGAO_LINK) {
                                    isProgram = true; // Most desktop shortcuts point to apps
                                } else {
                                    STRRET pathStr;
                                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &pathStr))) {
                                        WCHAR path[MAX_PATH];
                                        if (SUCCEEDED(StrRetToBufW(&pathStr, pidl, path, MAX_PATH))) {
                                            LPCWSTR ext = PathFindExtensionW(path);
                                            if (ext) {
                                                if (_wcsicmp(ext, L".exe") == 0 ||
                                                    _wcsicmp(ext, L".bat") == 0 ||
                                                    _wcsicmp(ext, L".cmd") == 0 ||
                                                    _wcsicmp(ext, L".url") == 0 ||
                                                    _wcsicmp(ext, L".lnk") == 0) {
                                                    isProgram = true;
                                                }
                                            }
                                        }
                                    }
                                }
                                type = isProgram ? ItemType::Program : ItemType::File;
                            }
                            fresh[name] = type;
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    g_itemTypes = std::move(fresh);
}

static inline bool ShouldHideLabel(LPCWSTR text, int cch) {
    if (!g_settings.hide_programs && !g_settings.hide_files && !g_settings.hide_folders) return false;

    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    auto it = g_itemTypes.find(s);
    ItemType type = (it != g_itemTypes.end()) ? it->second : ItemType::File;

    if (type == ItemType::Program && g_settings.hide_programs) return true;
    if (type == ItemType::File && g_settings.hide_files) return true;
    if (type == ItemType::Folder && g_settings.hide_folders) return true;

    return false;
}

// ---------------------------------------------------------------------------
// Subclass
// ---------------------------------------------------------------------------

static void InitAndPatchImageLists();

static constexpr UINT_PTR kResumeTimerId = 0x4948; // "HI" in hex

static void ReapplyOverlayPatches() {
    if (g_settings.hide_arrows || g_settings.hide_shield) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        InitAndPatchImageLists();
    }
    if (HWND hDesktop = GetDesktopFolderView()) {
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) ListView_RedrawItems(hDesktop, 0, count - 1);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

LRESULT CALLBACK ProgmanSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_POWERBROADCAST && wParam == PBT_APMRESUMEAUTOMATIC) {
        SetTimer(hWnd, kResumeTimerId, 2000, nullptr);
    }
    if (uMsg == WM_TIMER && wParam == kResumeTimerId) {
        KillTimer(hWnd, kResumeTimerId);
        ReapplyOverlayPatches();
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT) {
        if (g_settings.hide_programs || g_settings.hide_files || g_settings.hide_folders) {
            UpdateFolderCache();
        }
        g_isDrawingDesktop = true;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_isDrawingDesktop = false;
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Text Hooks
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
// Transparent Icon Builder
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

    int maskStride = ((cx + 31) / 32) * 4;
    std::vector<BYTE> maskBits((size_t)maskStride * cy, 0xFF);
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

// A persistent blank icon, used by the SHGetStockIconInfo hook so callers
// always get a valid handle for SIID_SHIELD.
HICON g_hBlankShieldIcon = nullptr;

static void InitAndPatchImageLists() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))GetProcAddress(hShell32, "SHGetIconOverlayIndexW");

    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    if (g_settings.hide_arrows) {
        g_linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    }

    g_shieldOverlays.clear();
    g_shieldSysIdx = -1;
    if (g_settings.hide_shield) {
        const struct { LPCWSTR path; int id; } targets[] = {
            { L"shell32.dll", 77 }, { L"imageres.dll", 77 }, { L"imageres.dll", 78 }
        };
        for (const auto& t : targets) {
            int idx = pSHGetIconOverlayIndex(t.path, t.id);
            if (idx > 0) g_shieldOverlays.push_back(idx);
        }

        SHSTOCKICONINFO sii = { sizeof(sii) };
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX, &sii))) {
            g_shieldSysIdx = sii.iSysImageIndex;
        }
    }

    static const GUID iidImageList = {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};
    const int sizes[] = { 1, 0, 2, 3, 4 };

    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, iidImageList, (void**)&piml)) || !piml) continue;

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = CreateTransparentIcon(cx, cy)) {
                int blankIdx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &blankIdx)) && blankIdx >= 0) {

                    if (g_settings.hide_arrows && g_linkOverlay > 0) {
                        piml->SetOverlayImage(blankIdx, g_linkOverlay);
                    }
                    if (g_settings.hide_shield) {
                        for (int so : g_shieldOverlays) {
                            piml->SetOverlayImage(blankIdx, so);
                        }
                    }
                }

                if (g_settings.hide_shield && g_shieldSysIdx >= 0) {
                    int dummyIdx = -1;
                    piml->ReplaceIcon(g_shieldSysIdx, hBlank, &dummyIdx);
                }

                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// Layer 2: Live Draw Intercept
// ---------------------------------------------------------------------------

using ImageList_DrawIndirect_t = BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original;
BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    // No g_isDrawingDesktop gate: overlays can be drawn from any thread (esp. on Win 11)
    // and the mod is explicitly designed to be process-wide for overlays.
    if (pimldp && (pimldp->fStyle & ILD_OVERLAYMASK)) {
        int overlayIndex = (pimldp->fStyle & ILD_OVERLAYMASK) >> 8;
        bool stripOverlay = false;

        if (g_settings.hide_arrows && overlayIndex == g_linkOverlay) {
            stripOverlay = true;
        }

        if (g_settings.hide_shield) {
            for (int so : g_shieldOverlays) {
                if (overlayIndex == so) {
                    stripOverlay = true;
                    break;
                }
            }
        }

        if (stripOverlay) {
            pimldp->fStyle &= ~ILD_OVERLAYMASK;
        }
    }
    return ImageList_DrawIndirect_Original(pimldp);
}

// ---------------------------------------------------------------------------
// Layer 3: Bake-in Interception (the actual UAC shield killer)
// On Win 11 the UAC shield is composited INTO the HICON via SHGetFileInfo
// (SHGFI_ADDOVERLAYS / SHGFI_OVERLAYINDEX). The image-list paths above never
// see a separate overlay flag — the shield is already painted into the icon
// pixels. Strip those flags at the source.
// ---------------------------------------------------------------------------

using SHGetFileInfoW_t = decltype(&SHGetFileInfoW);
SHGetFileInfoW_t SHGetFileInfoW_Original;
DWORD_PTR WINAPI SHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes,
                                     SHFILEINFOW* psfi, UINT cbFileInfo, UINT uFlags) {
    if (g_settings.hide_shield || g_settings.hide_arrows) {
        // SHGFI_ADDOVERLAYS bakes every registered overlay (incl. UAC shield) into the HICON.
        // SHGFI_OVERLAYINDEX returns the overlay index in iIcon's upper bits.
        // Removing both forces shell to give back a clean icon without composition.
        uFlags &= ~(SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX);
    }
    return SHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

using SHGetStockIconInfo_t = decltype(&SHGetStockIconInfo);
SHGetStockIconInfo_t SHGetStockIconInfo_Original;
HRESULT WINAPI SHGetStockIconInfo_Hook(SHSTOCKICONID siid, UINT uFlags, SHSTOCKICONINFO* psii) {
    HRESULT hr = SHGetStockIconInfo_Original(siid, uFlags, psii);

    if (g_settings.hide_shield && SUCCEEDED(hr) && siid == SIID_SHIELD && psii) {
        // Replace any returned icon handle with a transparent one. Callers that copy/paint
        // SIID_SHIELD (toolbars, button decorators, certain shell extensions) now paint nothing.
        if ((uFlags & SHGSI_ICON) && psii->hIcon) {
            if (!g_hBlankShieldIcon) {
                int cx = GetSystemMetrics((uFlags & SHGSI_SMALLICON) ? SM_CXSMICON : SM_CXICON);
                int cy = GetSystemMetrics((uFlags & SHGSI_SMALLICON) ? SM_CYSMICON : SM_CYICON);
                g_hBlankShieldIcon = CreateTransparentIcon(cx, cy);
            }
            if (g_hBlankShieldIcon) {
                DestroyIcon(psii->hIcon);
                psii->hIcon = CopyIcon(g_hBlankShieldIcon);
            }
        }
    }
    return hr;
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
    g_settings.hide_programs = Wh_GetIntSetting(L"hide_programs") != 0;
    g_settings.hide_files = Wh_GetIntSetting(L"hide_files") != 0;
    g_settings.hide_folders = Wh_GetIntSetting(L"hide_folders") != 0;
    g_settings.hide_arrows = Wh_GetIntSetting(L"hide_arrows") != 0;
    g_settings.hide_shield = Wh_GetIntSetting(L"hide_shield") != 0;
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

    if (HMODULE hComctl = GetModuleHandle(L"comctl32.dll")) {
        if (auto p = (ImageList_DrawIndirect_t)GetProcAddress(hComctl, "ImageList_DrawIndirect"))
            WindhawkUtils::SetFunctionHook(p, ImageList_DrawIndirect_Hook, &ImageList_DrawIndirect_Original);
    }

    if (HMODULE hShell32 = GetModuleHandle(L"shell32.dll")) {
        if (auto p = (SHGetFileInfoW_t)GetProcAddress(hShell32, "SHGetFileInfoW"))
            WindhawkUtils::SetFunctionHook(p, SHGetFileInfoW_Hook, &SHGetFileInfoW_Original);
        if (auto p = (SHGetStockIconInfo_t)GetProcAddress(hShell32, "SHGetStockIconInfo"))
            WindhawkUtils::SetFunctionHook(p, SHGetStockIconInfo_Hook, &SHGetStockIconInfo_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // SHChangeNotify must happen BEFORE we patch image lists. Otherwise it
    // invalidates the icon cache and silently throws our SetOverlayImage /
    // ReplaceIcon patches away, leaving the original arrows/shield in place.
    if (g_settings.hide_arrows || g_settings.hide_shield) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        InitAndPatchImageLists();
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

    if (HWND hProgman = GetShellWindow()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hProgman, ProgmanSubclass, 0);
    }
}

void Wh_ModUninit() {
    if (HWND hProgman = GetShellWindow()) {
        KillTimer(hProgman, kResumeTimerId);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hProgman, ProgmanSubclass);
    }

    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }

    if (g_hBlankShieldIcon) {
        DestroyIcon(g_hBlankShieldIcon);
        g_hBlankShieldIcon = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
