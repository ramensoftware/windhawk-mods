// ==WindhawkMod==
// @id           explorer-visual-tweaks-dark
// @name         Explorer Visual Tweaks Dark
// @description  Custom selection highlights, disk progress bars, and a dark Preview Pane (background + text) for Explorer. Architecturally theme-independent — the defaults are just tuned for dark theme; set your own colors in Settings to use it with any theme.
// @version      1.0.0
// @author       VitalS
// @github       https://github.com/VitalSkib
// @include      explorer.exe
// @include      prevhost.exe
// @compilerOptions -lcomctl32 -luser32 -lgdi32 -luxtheme -lmsimg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Visual Tweaks Dark

A set of visual tweaks for File Explorer's dark theme:

- **Selection highlights** — replaces the built-in selection/hover/multi-select
  backgrounds in the file list and navigation pane with custom rounded
  backgrounds (configurable radius, fill and border colors), by hooking
  `DrawThemeBackground`/`DrawThemeBackgroundEx` in uxtheme.dll.
- **Navigation pane focus indicator** — a small colored pill next to the
  selected item in the folder tree, drawn the same way.
- **Disk usage progress bars** — redraws the drive-space bars in the
  navigation pane with a custom gradient fill and rounded corners, also via
  the uxtheme.dll hooks above.
- **Preview Pane background fix** — Explorer normally paints the Preview
  Pane frame with a fixed background color that doesn't match the file
  list's dark background. This mod replaces it with a configurable color
  (`191919` by default) by hooking `DirectUI::Element::PaintBackground`
  inside `DUI70.dll` and patching the DirectUI `Value` color field it
  paints from, directly, for the duration of the call. `DUI70.dll` isn't
  always loaded yet when the mod starts, so if it isn't, the hook is
  installed as soon as it loads, via `LdrRegisterDllNotification`.
- **Preview Pane text view (dark)** — the Preview Pane's plain-text view
  (a RICHEDIT50W control, used for `.txt` and similar files, hosted in
  `explorer.exe` and `prevhost.exe`) is themed separately, using the same
  `previewPaneBgColor` so the frame and the text view always match. Text
  color adjusts automatically for contrast against whatever background
  color is set, so it stays readable even if you set a light background.
  Its scrollbar is switched between the normal and `DarkMode_Explorer`
  visual style based on that same light/dark check, since native scrollbars
  aren't affected by `EM_SETBKGNDCOLOR`.

The selection-highlight styling also applies inside Open/Save dialogs
(`IFileDialog`) hosted in other applications, since those reuse Explorer's
own shell view internally.

Although this mod is named "Dark", none of it is hardwired to a dark
theme — every hook just paints whatever color is configured. The "Dark"
in the name refers only to the defaults: they're picked for dark theme.
To use it with any other theme, just set your own colors for selection,
progress bars, and Preview Pane in Settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerRadius: 2
  $name: Corner radius (Scale, 1-5)
  $description: >
    Visual corner rounding, border-radius.
    Max is 5 to prevent oval distortion on typical item heights.

- showBorder: true
  $name: Show selection border
  $description: Draws a 1 px border ring inside the selection background. When off, border color settings below have no effect.

- activeFillColor: "4D4D4D"
  $name: Active fill color (RRGGBB)
  $description: Fill color for single selection, focus state, and hover.

- activeBorderColor: "555555"
  $name: Active border color (RRGGBB)
  $description: Border ring color for single selection, focus state, and hover.

- multiFillColor: "454545"
  $name: Multi-select fill color (RRGGBB)
  $description: Fill color for Ctrl or Shift multi-select.

- multiBorderColor: "505050"
  $name: Multi-select border color (RRGGBB)
  $description: Border ring color for Ctrl or Shift multi-select.

- focusPillColor: "4CC2FF"
  $name: Focus pill color (RRGGBB)
  $description: >
    Color of the vertical focus indicator in the navigation pane.
    Set to same as fill color to hide it.

- showProgressBorder: true
  $name: Show progress bar border
  $description: Draw a 1 px border around the disk usage bar background.

- progressBorderColor: "404040"
  $name: Progress bar border color (RRGGBB)

- progressBgColor: "585858"
  $name: Progress bar background color (RRGGBB)

- progressFillColorStart: "0078D7"
  $name: Progress bar fill gradient start color (RRGGBB)

- progressFillColorEnd: "0094FE"
  $name: Progress bar fill gradient end color (RRGGBB)

- progressFillColorFullStart: "E43060"
  $name: Progress bar fill gradient start color (full/warning) (RRGGBB)

- progressFillColorFullEnd: "ED6050"
  $name: Progress bar fill gradient end color (full/warning) (RRGGBB)

- progressCornerRadius: 1
  $name: Progress bar corner radius
  $description: Corner radius in pixels for both background and fill (0 = no rounding).

- matchDetailsPaneBg: true
  $name: Fix Preview Pane background color
  $description: >
    Explorer paints the Preview Pane with a fixed background color that does not match
    the file list's dark background. Enabling this applies the color set below to the
    Preview Pane.

- previewPaneBgColor: "191919"
  $name: Preview Pane background color (RRGGBB)
  $description: >
    Background color applied to the Preview Pane, both its frame and its
    plain-text view (see matchDetailsPaneBg above and the Text Preview Dark
    module in the source).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <uxtheme.h>
#include <atomic>
#include <unordered_map>
#include <shared_mutex>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <cstdint>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

// ============================================================================
// Text Preview Dark module
// ----------------------------------------------------------------------------
// Self-contained fix for the Preview Pane's plain-text view (the RICHEDIT50W
// control hosted by "Shell Preview Extension Host(...Previewer)"): applies a
// dark background and light text color, since Explorer does not theme this
// control on its own. The background color is shared with the DUI70
// Preview Pane fix further below via previewPaneBgColor (see ApplySettings /
// Wh_ModSettingsChanged), so this module carries no color literal of its
// own beyond the built-in fallback.
//
// Everything in this namespace is private to the module; Init/AfterInit/
// Uninit/UpdateBackgroundColor are its only entry points, called from the
// matching Wh_Mod* functions at the bottom of this file.
// ============================================================================
namespace TextPreviewDark {

constexpr wchar_t kContainerClassName[] = L"Shell Preview Extension Host Previewer";
constexpr wchar_t kOuterContainerClassName[] = L"Shell Preview Extension Host";
constexpr wchar_t kRichEditClassName[] = L"RICHEDIT50W";
constexpr wchar_t kPropLayoutDone[] = L"Wh_InitLayoutDone";

// Shared with the Preview Pane fix via previewPaneBgColor (see ApplySettings /
// UpdateBackgroundColor).
std::atomic<COLORREF> g_targetBgColor{ RGB(0x19, 0x19, 0x19) };

// Single luminance check decides both text color and scrollbar theme.
void ApplyRichEditTextColor(HWND hWnd) {
    COLORREF bg = g_targetBgColor.load(std::memory_order_relaxed);
    int luminance = (GetRValue(bg) * 299 + GetGValue(bg) * 587 + GetBValue(bg) * 114) / 1000;
    bool isDark = luminance <= 128;

    CHARFORMAT2W cf = {};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = 0;
    cf.crTextColor = isDark ? RGB(0xD4, 0xD4, 0xD4) : RGB(0x20, 0x20, 0x20);
    SendMessageW(hWnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    // Scrollbar is non-client, so EM_SETBKGNDCOLOR doesn't reach it.
    SetWindowTheme(hWnd, isDark ? L"DarkMode_Explorer" : nullptr, nullptr);
    SendMessageW(hWnd, WM_THEMECHANGED, 0, 0);
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

HBRUSH g_containerBrush = nullptr;

// GCLP_HBRBACKGROUND can legitimately be 0 (class has no custom brush),
// so a null value here does not mean "not captured yet". g_originalCaptured
// disambiguates that.
HBRUSH g_originalClassBrush = nullptr;
volatile LONG g_originalCaptured = 0;

// Independent capture/restore state for the outer "Shell Preview Extension
// Host" container class. Kept separate from g_originalClassBrush /
// g_originalCaptured (which belong to "...Previewer") on purpose: these are
// two distinct window classes, and nothing guarantees their original class
// brushes are the same value, now or in a future Windows build.
HBRUSH g_originalOuterClassBrush = nullptr;
volatile LONG g_outerOriginalCaptured = 0;

// Set once Wh_ModUninit starts tearing down. Blocks any new subclass/brush
// swap that could otherwise race with cleanup (e.g. a new container window
// created while CreateWindowExW_Hook is still installed, mid-unload).
volatile LONG g_unloading = 0;

// True only if a restoration attempt was made and failed. Used to decide
// whether deleting g_containerBrush is safe, or would leave a dangling
// GCLP_HBRBACKGROUND pointer on the class.
volatile LONG g_classBrushRestoreFailed = 0;

// SetClassLongPtrW/GetClassLongPtrW can legitimately return 0 on success
// (e.g. when the previous/new value itself is 0), so success must be
// checked via GetLastError(), not via the return value alone.
LONG_PTR SetClassLongPtrChecked(HWND hWnd, int nIndex, LONG_PTR newValue, bool* outOk) {
    SetLastError(0);
    LONG_PTR ret = SetClassLongPtrW(hWnd, nIndex, newValue);
    if (ret == 0) {
        DWORD err = GetLastError();
        *outOk = (err == 0);
    } else {
        *outOk = true;
    }
    return ret;
}

HBRUSH GetContainerBrush() {
    HBRUSH existing = (HBRUSH)InterlockedCompareExchangePointer(
        (PVOID*)&g_containerBrush, nullptr, nullptr);
    if (existing) return existing;

    HBRUSH newBrush = CreateSolidBrush(g_targetBgColor.load(std::memory_order_relaxed));
    if (!newBrush) {
        Wh_Log(L"CreateSolidBrush failed, error=%lu", GetLastError());
        return nullptr;
    }

    HBRUSH prev = (HBRUSH)InterlockedCompareExchangePointer(
        (PVOID*)&g_containerBrush, newBrush, nullptr);
    if (prev) {
        DeleteObject(newBrush);
        return prev;
    }
    return newBrush;
}

LRESULT CALLBACK ContainerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_SIZE: {
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (!GetPropW(hWnd, kPropLayoutDone)) {
                SetPropW(hWnd, kPropLayoutDone, (HANDLE)1);
                RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }
            return result;
        }
        case WM_NCDESTROY:
            RemovePropW(hWnd, kPropLayoutDone);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ContainerSubclassProc);
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK FindRichEditChildCallback(HWND hWnd, LPARAM lParam) {
    wchar_t className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, kRichEditClassName) == 0) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;  // found it, stop enumerating
    }
    return TRUE;
}

// Structural check: confirms this "...Previewer" window genuinely hosts a
// RICHEDIT50W child, i.e. it's a real preview-pane text host and not just
// some unrelated window that happens to share the class name. Deliberately
// based on window-tree structure, not on which process owns anything, so
// it works the same whether the mod is loaded via @include or added later
// through the Inclusion List.
bool ContainsRichEditChild(HWND hWnd) {
    bool found = false;
    EnumChildWindows(hWnd, FindRichEditChildCallback, reinterpret_cast<LPARAM>(&found));
    return found;
}

// EnumWindows/EnumChildWindows walk every top-level window on the desktop
// and all of their children, regardless of which process owns them - not
// just this process's own windows. A matching class name can therefore
// belong to a window owned by a different process (e.g. explorer.exe's own
// enumeration reaching a different prevhost.exe instance's windows, or vice
// versa). Cross-process SetClassLongPtrW always fails with
// ERROR_ACCESS_DENIED, and subclassing a foreign-owned HWND is unsafe
// regardless, so any such window must be skipped before attempting either.
static bool IsOwnedByCurrentProcess(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}

bool TrySubclassIfContainer(HWND hWnd) {
    if (g_unloading) return false;

    wchar_t className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) || wcscmp(className, kContainerClassName) != 0) return false;
    if (!IsOwnedByCurrentProcess(hWnd)) return false;

    // Only proceed if this Previewer genuinely hosts a RICHEDIT50W child. At
    // creation time this child may not exist yet - in that case we simply
    // don't apply the swap now; TrySetupIfRichEdit retries this same call
    // once its RichEdit child actually appears (see below).
    if (!ContainsRichEditChild(hWnd)) return false;

    HBRUSH currentBrush = (HBRUSH)GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
    HBRUSH ourBrush = GetContainerBrush();
    if (!ourBrush) return false;

    if (currentBrush != ourBrush) {
        // Capture the original brush exactly once, race-free, including the
        // legitimate case where the original value is 0.
        if (InterlockedCompareExchange(&g_originalCaptured, 1, 0) == 0) {
            g_originalClassBrush = currentBrush;
        }

        bool ok = false;
        SetClassLongPtrChecked(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)ourBrush, &ok);
        if (!ok) {
            Wh_Log(L"SetClassLongPtrW(GCLP_HBRBACKGROUND) failed, error=%lu", GetLastError());
        }
    }

    if (g_unloading) return false;  // re-check: unload could have started mid-call
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ContainerSubclassProc, 0);
    return true;
}

// Handles the outer "Shell Preview Extension Host" container (the parent of
// "...Previewer"). This window is not subclassed and receives no message
// handling of its own: swapping GCLP_HBRBACKGROUND is the entire fix here,
// covering redraw artifacts that appear along its right edge during resize.
bool TryApplyOuterContainerBrush(HWND hWnd) {
    if (g_unloading) return false;

    wchar_t className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) || wcscmp(className, kOuterContainerClassName) != 0) return false;
    if (!IsOwnedByCurrentProcess(hWnd)) return false;

    HBRUSH currentBrush = (HBRUSH)GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
    HBRUSH ourBrush = GetContainerBrush();
    if (!ourBrush) return false;

    if (currentBrush != ourBrush) {
        // Capture the original brush exactly once, race-free, including the
        // legitimate case where the original value is 0. Independent of the
        // Previewer's own capture flag/value.
        if (InterlockedCompareExchange(&g_outerOriginalCaptured, 1, 0) == 0) {
            g_originalOuterClassBrush = currentBrush;
        }

        bool ok = false;
        SetClassLongPtrChecked(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)ourBrush, &ok);
        if (!ok) {
            Wh_Log(L"SetClassLongPtrW(GCLP_HBRBACKGROUND) failed for outer container, error=%lu", GetLastError());
        }
    }

    return true;
}

LRESULT CALLBACK RichEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_SETTEXT:
        case EM_STREAMIN: {
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            ApplyRichEditTextColor(hWnd);
            return result;
        }
        case WM_NCDESTROY:
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RichEditSubclassProc);
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool TrySetupIfRichEdit(HWND hWnd) {
    if (g_unloading) return false;

    wchar_t className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) || wcscmp(className, kRichEditClassName) != 0) return false;
    if (!IsOwnedByCurrentProcess(hWnd)) return false;

    COLORREF bg = g_targetBgColor.load(std::memory_order_relaxed);
    SendMessageW(hWnd, EM_SETBKGNDCOLOR, 0, (LPARAM)bg);
    ApplyRichEditTextColor(hWnd);

    if (g_unloading) return false;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, RichEditSubclassProc, 0);

    // Now that this RichEdit child genuinely exists, retry the parent
    // Previewer's brush swap in case it was skipped earlier for lacking a
    // RichEdit child at creation time. Idempotent/safe to call again if the
    // swap already happened.
    HWND hParent = GetParent(hWnd);
    if (hParent) TrySubclassIfContainer(hParent);

    return true;
}

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    TrySubclassIfContainer(hWnd);
    TryApplyOuterContainerBrush(hWnd);
    TrySetupIfRichEdit(hWnd);
    EnumChildWindows(hWnd, EnumWindowsCallback, 0);
    return TRUE;
}

// Single-pass teardown: removes both subclasses and restores the original
// class brush where applicable. GCLP_HBRBACKGROUND is a class-wide (not
// per-window) attribute, so restoring it on the first matching window is
// enough for the whole class; subsequent matches just repeat the same
// write, which is harmless and idempotent.
BOOL CALLBACK EnumWindowsUninitCallback(HWND hWnd, LPARAM lParam) {
    wchar_t className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && IsOwnedByCurrentProcess(hWnd)) {
        if (wcscmp(className, kContainerClassName) == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ContainerSubclassProc);

            if (g_originalCaptured) {
                bool ok = false;
                SetClassLongPtrChecked(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)g_originalClassBrush, &ok);
                if (!ok) {
                    Wh_Log(L"Failed to restore GCLP_HBRBACKGROUND, error=%lu", GetLastError());
                    InterlockedExchange(&g_classBrushRestoreFailed, 1);
                }
            }
        } else if (wcscmp(className, kOuterContainerClassName) == 0) {
            // No subclass to remove here (TryApplyOuterContainerBrush never
            // subclasses this window) - only the class brush is restored,
            // using this class's own captured original, independent of the
            // Previewer's.
            if (g_outerOriginalCaptured) {
                bool ok = false;
                SetClassLongPtrChecked(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)g_originalOuterClassBrush, &ok);
                if (!ok) {
                    Wh_Log(L"Failed to restore GCLP_HBRBACKGROUND for outer container, error=%lu", GetLastError());
                    InterlockedExchange(&g_classBrushRestoreFailed, 1);
                }
            }
        } else if (wcscmp(className, kRichEditClassName) == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RichEditSubclassProc);
        }
    }
    EnumChildWindows(hWnd, EnumWindowsUninitCallback, 0);
    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName) && !g_unloading) {
        if (wcscmp(lpClassName, kContainerClassName) == 0) TrySubclassIfContainer(hWnd);
        else if (wcscmp(lpClassName, kOuterContainerClassName) == 0) TryApplyOuterContainerBrush(hWnd);
        else if (wcscmp(lpClassName, kRichEditClassName) == 0) TrySetupIfRichEdit(hWnd);
    }
    return hWnd;
}

// ── Module lifecycle (called from the host mod's Wh_Mod* functions) ──────────

// initialBgColor seeds g_targetBgColor from previewPaneBgColor, so the very
// first brush this module creates already matches the DUI70 Preview Pane fix.
bool Init(COLORREF initialBgColor) {
    g_targetBgColor.store(initialBgColor, std::memory_order_relaxed);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    FARPROC pCreateWindowExW = hUser32 ? GetProcAddress(hUser32, "CreateWindowExW") : nullptr;
    if (!pCreateWindowExW) return false;

    Wh_SetFunctionHook((void*)pCreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    return true;
}

void AfterInit() {
    EnumWindows(EnumWindowsCallback, 0);
}

void Uninit() {
    // Block any further subclassing/brush swaps from this point on, even
    // though the CreateWindowExW hook technically stays installed until
    // Windhawk unhooks it after this function returns.
    InterlockedExchange(&g_unloading, 1);

    EnumWindows(EnumWindowsUninitCallback, 0);

    // Only free the brush if we're certain neither class still references
    // it. The brush is shared between the Previewer and the outer container
    // (same target color, one GDI object for both), so
    // g_classBrushRestoreFailed is intentionally shared too: it is only
    // ever set inside the g_originalCaptured / g_outerOriginalCaptured
    // branches above, so checking it alone already covers "no capture" (never
    // set) and "capture + failed restore on either class" (set) correctly.
    // If restoration failed anywhere, leak intentionally: a leaked GDI
    // handle is recoverable (process exit), a dangling class brush pointer
    // is a use-after-free on the next WM_ERASEBKGND.
    if (g_containerBrush) {
        if (!g_classBrushRestoreFailed) {
            DeleteObject(g_containerBrush);
        } else {
            Wh_Log(L"Leaking container brush intentionally: class restoration failed");
        }
        g_containerBrush = nullptr;
    }

    g_originalClassBrush = nullptr;
    g_originalCaptured = 0;
    g_originalOuterClassBrush = nullptr;
    g_outerOriginalCaptured = 0;
    g_classBrushRestoreFailed = 0;
}

// Called from the host mod's Wh_ModSettingsChanged whenever previewPaneBgColor
// changes, to keep this module's RichEdit background in sync.
void UpdateBackgroundColor(COLORREF newColor) {
    if (g_unloading) return;
    if (g_targetBgColor.exchange(newColor, std::memory_order_relaxed) == newColor) return;

    HBRUSH newBrush = CreateSolidBrush(newColor);
    if (!newBrush) {
        Wh_Log(L"CreateSolidBrush failed during color refresh, error=%lu", GetLastError());
        return;
    }
    HBRUSH oldBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)&g_containerBrush, newBrush);

    // Reuses the same window walk as startup: TrySubclassIfContainer /
    // TryApplyOuterContainerBrush / TrySetupIfRichEdit already swap in
    // whatever GetContainerBrush() currently returns and refresh the RichEdit
    // background, so this call repaints every live window with the new color.
    EnumWindows(EnumWindowsCallback, 0);

    // Safe to free now: the walk above already repointed every live window
    // away from it.
    if (oldBrush) DeleteObject(oldBrush);
}

}  // namespace TextPreviewDark

// ── Function pointer typedefs ─────────────────────────────────────────────────

typedef HRESULT      (WINAPI *DrawThemeBackground_t)(HTHEME, HDC, int, int, const RECT*, const RECT*);
typedef HRESULT      (WINAPI *DrawThemeBackgroundEx_t)(HTHEME, HDC, int, int, const RECT*, const DTBGOPTS*);
typedef HTHEME       (WINAPI *OpenThemeData_t)(HWND, LPCWSTR);
typedef HTHEME       (WINAPI *OpenThemeDataEx_t)(HWND, LPCWSTR, DWORD);
typedef HRESULT      (WINAPI *CloseThemeData_t)(HTHEME);
// DUI70.dll internal Element methods (Preview Pane background fix — see the
// "DUI70 Preview Pane background fix" section further below). Undocumented
// decorated symbols, resolved by name in Wh_ModInit / TryInstallPreviewPaneFix.
typedef void (__fastcall *PaintBackground_t)(
    void* pThis, void* hdc, void* pValue,
    const RECT* prcBounds, const RECT* prcInvalid, const RECT* a6, const RECT* a7);
typedef UINT (__fastcall *GetID_t)(void* pThis);

static DrawThemeBackground_t   pOrigDrawThemeBg          = nullptr;
static DrawThemeBackgroundEx_t pOrigDrawThemeBgEx        = nullptr;
static OpenThemeData_t         pOrigOpenThemeData        = nullptr;
static OpenThemeDataEx_t       pOrigOpenThemeDataEx      = nullptr;
static CloseThemeData_t        pOrigCloseThemeData       = nullptr;
static PaintBackground_t       pOrigPaintBackground       = nullptr;
static GetID_t                 pGetID                     = nullptr;

// ── Runtime settings (atomic for safe cross-thread access) ────────────────────

static std::atomic<int>  g_borderWidth { 1 };

// ── Progress / Details pane settings ─────────────────────────────────────────
//
// Read in ApplySettings, consumed only from draw hooks on the UI thread —
// no atomics needed; values are written before hooks fire and only
// rewritten on Wh_ModSettingsChanged which Windhawk serializes with the UI.

struct ProgressSettings {
    bool     detailsBgMatchEnabled = true; // matchDetailsPaneBg setting
    COLORREF previewPaneBgColor    = RGB(0x19, 0x19, 0x19); // previewPaneBgColor setting
    bool     showProgressBorder    = true;
    COLORREF progressBorder        = RGB(0x40, 0x40, 0x40);
    COLORREF progressBg            = RGB(0x58, 0x58, 0x58);
    COLORREF progressFillStart     = RGB(0x00, 0x78, 0xD7);
    COLORREF progressFillEnd       = RGB(0x00, 0x94, 0xFE);
    COLORREF progressFillFullStart = RGB(0xE4, 0x30, 0x60);
    COLORREF progressFillFullEnd   = RGB(0xED, 0x60, 0x50);
    int      progressCornerRadius  = 1;
} g_prog;

// ── Custom 9-patch background resources ──────────────────────────────────────
//
// Two slots: [0] = active selection / hover, [1] = multi-select.
// Matrix size: 2m+3 (guard-band design).
// Three centre columns/rows [m, m+1, m+2] are mathematically identical so
// AlphaBlend's bilinear sampling reads identical pixels during edge-stretch
// → zero colour bleed (the "phantom inner ring" fix).
//
// cornerRadius setting = visual radius (1-5). Internal m = radius * 2.
// Border ring: hard alpha step, no gradient between border and fill.

static HDC     g_hCustomBgDC[2]  = { nullptr, nullptr };
static HBITMAP g_hCustomBgBmp[2] = { nullptr, nullptr };
static int     g_ninePatchMargin = 4; // internal m; shared by both slots

// ── Focus pill resource ───────────────────────────────────────────────────────
//
// Fixed 3×18 px ARGB DIB, pill shape (radius = 1.5 px).
// Drawn in the nav pane (TreeView) for S5 and S6 only.
// Position: left edge = pRect->left + borderWidth, vertically centred.

static HDC     g_hFocusPillDC  = nullptr;
static HBITMAP g_hFocusPillBmp = nullptr;

static const int PILL_W = 3;
static const int PILL_H = 18;

// ── Process identity flags ────────────────────────────────────────────────────
//
// g_hostIsExplorer: true when running inside explorer.exe (lets IsShellHwnd
//   skip the parent walk).
// g_hasDesktopAccess: true when GetDC(NULL) works (set once in Wh_ModInit).
//   Sandboxed renderer processes (Firefox e10s, Chrome GPU/renderer) block
//   Win32k syscalls, so GetDC(NULL) returns NULL there — when false,
//   Wh_ModInit skips hook installation entirely, since nothing here can
//   render without a desktop. InitCustomNinePatch/InitFocusPill check this
//   too, in case Wh_ModSettingsChanged fires later in such a process.

static bool g_hostIsExplorer   = false;
static bool g_hasDesktopAccess = false;

// ── Settings helpers ──────────────────────────────────────────────────────────

// Reads a hex color setting by name and outputs r/g/b bytes.
// Accepts both RRGGBB and #RRGGBB formats.
static void ParseHexColor(const wchar_t* name, const wchar_t* hardDefault,
                           BYTE& r, BYTE& g, BYTE& b)
{
    PCWSTR val = Wh_GetStringSetting(name);
    const wchar_t* src = (val && val[0] != L'\0') ? val : hardDefault;
    if (src[0] == L'#') src++;
    unsigned ir = 0, ig = 0, ib = 0;
    swscanf_s(src, L"%02x%02x%02x", &ir, &ig, &ib);
    r = (BYTE)ir; g = (BYTE)ig; b = (BYTE)ib;
    if (val) Wh_FreeStringSetting(val);
}

// Reads a hex color setting by name and returns a COLORREF.
// Accepts both RRGGBB and #RRGGBB formats.
static COLORREF ParseHexColorRef(const wchar_t* name, COLORREF fallback) {
    // Reuse ParseHexColor; format the fallback as a hex string for the hardDefault param.
    // Simpler: just open the setting directly here to avoid a string conversion.
    PCWSTR val = Wh_GetStringSetting(name);
    if (!val || val[0] == L'\0') {
        if (val) Wh_FreeStringSetting(val);
        return fallback;
    }
    const wchar_t* src = (val[0] == L'#') ? val + 1 : val;
    unsigned ir = 0, ig = 0, ib = 0;
    COLORREF result = (swscanf_s(src, L"%02x%02x%02x", &ir, &ig, &ib) == 3)
        ? RGB((BYTE)ir, (BYTE)ig, (BYTE)ib)
        : fallback;
    Wh_FreeStringSetting(val);
    return result;
}

// Wh_GetIntSetting returns 0 for both "unset" and explicit 0,
// so we fall back to def when the value is zero.
static int GetIntOr(const wchar_t* name, int def) {
    int v = Wh_GetIntSetting(name);
    return (v != 0) ? v : def;
}

// ── Resource lifecycle ────────────────────────────────────────────────────────

static void FreeNinePatch(int idx) {
    if (idx < 0 || idx > 1) return;
    // DeleteDC first: it deselects the bitmap, making it safe to delete.
    if (g_hCustomBgDC[idx])  { DeleteDC(g_hCustomBgDC[idx]);      g_hCustomBgDC[idx]  = nullptr; }
    if (g_hCustomBgBmp[idx]) { DeleteObject(g_hCustomBgBmp[idx]); g_hCustomBgBmp[idx] = nullptr; }
}

static void FreeFocusPill() {
    // DeleteDC first: it deselects the bitmap, making it safe to delete.
    if (g_hFocusPillDC)  { DeleteDC(g_hFocusPillDC);      g_hFocusPillDC  = nullptr; }
    if (g_hFocusPillBmp) { DeleteObject(g_hFocusPillBmp); g_hFocusPillBmp = nullptr; }
}

// ── 9-patch generation ────────────────────────────────────────────────────────

static void InitCustomNinePatch(int idx, int m, int borderWidth,
                                 BYTE fillR, BYTE fillG, BYTE fillB,
                                 BYTE borR,  BYTE borG,  BYTE borB)
{
    if (idx < 0 || idx > 1) return;
    if (!g_hasDesktopAccess) return; // sandboxed process — no GDI desktop access
    FreeNinePatch(idx);
    g_ninePatchMargin = m;

    const int size = 2 * m + 3;

    HDC hdcScreen = GetDC(NULL);
    HDC hdc = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi    = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = size;
    bmi.bmiHeader.biHeight      = -size; // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   pBits = nullptr;
    HBITMAP hBmp  = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);

    if (!hBmp || !pBits) { if (hBmp) DeleteObject(hBmp); DeleteDC(hdc); return; }

    SelectObject(hdc, hBmp);
    g_hCustomBgDC[idx]  = hdc;
    g_hCustomBgBmp[idx] = hBmp;

    BYTE* pixels = (BYTE*)pBits;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            // Layout: [0..m-1] corner | [m..m+2] centre band (3 identical cols/rows) | [m+3..2m+2] corner
            bool inCX = (x >= m && x <= m + 2);
            bool inCY = (y >= m && y <= m + 2);
            float insideDist;

            if (inCX || inCY) {
                int dx = std::min(x, 2 * m + 2 - x);
                int dy = std::min(y, 2 * m + 2 - y);
                insideDist = (float)std::min(dx, dy);
            } else {
                // Corner quadrant: 2D SDF, arc centre = inner corner pixel.
                float cx = (x < m) ? (float)m : (float)(m + 2);
                float cy = (y < m) ? (float)m : (float)(m + 2);
                float ddx = fabsf((float)x - cx) - 0.5f;
                float ddy = fabsf((float)y - cy) - 0.5f;
                insideDist = (float)m - sqrtf(ddx * ddx + ddy * ddy);
            }

            float alpha  = std::max(0.0f, std::min(1.0f, insideDist + 1.0f));
            bool  isBord = (borderWidth > 0) && (insideDist < (float)borderWidth);
            BYTE  r = isBord ? borR : fillR;
            BYTE  g = isBord ? borG : fillG;
            BYTE  b = isBord ? borB : fillB;

            BYTE  a = (BYTE)(255.0f * alpha + 0.5f);
            int   i = (y * size + x) * 4;
            pixels[i + 0] = (BYTE)((b * a) / 255); // premultiplied BGRA
            pixels[i + 1] = (BYTE)((g * a) / 255);
            pixels[i + 2] = (BYTE)((r * a) / 255);
            pixels[i + 3] = a;
        }
    }
}

// ── Focus pill generation ─────────────────────────────────────────────────────

static void InitFocusPill(BYTE colR, BYTE colG, BYTE colB) {
    if (!g_hasDesktopAccess) return; // sandboxed process — no GDI desktop access
    FreeFocusPill();

    const float radius = PILL_W / 2.0f; // 1.5 px — natural pill shape for a 3 px wide bar

    HDC hdcScreen = GetDC(NULL);
    HDC hdc = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi    = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = PILL_W;
    bmi.bmiHeader.biHeight      = -PILL_H;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   pBits = nullptr;
    HBITMAP hBmp  = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);

    if (!hBmp || !pBits) { if (hBmp) DeleteObject(hBmp); DeleteDC(hdc); return; }

    SelectObject(hdc, hBmp);
    g_hFocusPillDC  = hdc;
    g_hFocusPillBmp = hBmp;

    BYTE* pixels = (BYTE*)pBits;
    for (int y = 0; y < PILL_H; ++y) {
        for (int x = 0; x < PILL_W; ++x) {
            // Pill SDF: clamp pixel centre to the inner spine, measure distance.
            // radius = W/2 collapses the spine to a single centre point
            // → symmetric rounded caps and straight fully-opaque sides.
            float px = x + 0.5f, py = y + 0.5f;
            float cx = std::max(radius, std::min((float)PILL_W - radius, px));
            float cy = std::max(radius, std::min((float)PILL_H - radius, py));
            float dist = sqrtf((px - cx) * (px - cx) + (py - cy) * (py - cy));
            float insideDist = radius - dist;

            float alpha = std::max(0.0f, std::min(1.0f, insideDist + 0.5f));
            BYTE  a     = (BYTE)(255.0f * alpha + 0.5f);

            int i = (y * PILL_W + x) * 4;
            pixels[i + 0] = (BYTE)((colB * a) / 255); // premultiplied BGRA
            pixels[i + 1] = (BYTE)((colG * a) / 255);
            pixels[i + 2] = (BYTE)((colR * a) / 255);
            pixels[i + 3] = a;
        }
    }
}

// ── Drawing functions ─────────────────────────────────────────────────────────

static void DrawNinePatch(HDC hdc, const RECT* pRect, int idx) {
    if (idx < 0 || idx > 1 || !g_hCustomBgDC[idx]) return;

    int dx = pRect->left, dy = pRect->top;
    int dw = pRect->right - dx, dh = pRect->bottom - dy;
    if (dw <= 0 || dh <= 0) return;

    int  m   = g_ninePatchMargin;
    HDC  src = g_hCustomBgDC[idx];
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    if (dw <= m * 2 || dh <= m * 2) {
        // Item smaller than 2 corner radii — scale the whole matrix down.
        AlphaBlend(hdc, dx, dy, dw, dh, src, 0, 0, 2*m+3, 2*m+3, bf);
        return;
    }

    int mw = dw - m * 2;
    int mh = dh - m * 2;

    // 4 corners (m×m exact copy)
    AlphaBlend(hdc, dx,          dy,          m, m, src, 0,     0,     m, m, bf);
    AlphaBlend(hdc, dx + dw - m, dy,          m, m, src, m + 3, 0,     m, m, bf);
    AlphaBlend(hdc, dx,          dy + dh - m, m, m, src, 0,     m + 3, m, m, bf);
    AlphaBlend(hdc, dx + dw - m, dy + dh - m, m, m, src, m + 3, m + 3, m, m, bf);
    // 4 edges (1 px reference strip stretched; identical guard neighbours prevent bleed)
    AlphaBlend(hdc, dx + m,      dy,          mw, m,  src, m+1, 0,     1, m,  bf);
    AlphaBlend(hdc, dx + m,      dy + dh - m, mw, m,  src, m+1, m + 3, 1, m,  bf);
    AlphaBlend(hdc, dx,          dy + m,      m,  mh, src, 0,   m + 1, m, 1,  bf);
    AlphaBlend(hdc, dx + dw - m, dy + m,      m,  mh, src, m+3, m + 1, m, 1,  bf);
    // Centre (single pixel stretched to fill)
    AlphaBlend(hdc, dx + m, dy + m, mw, mh, src, m+1, m+1, 1, 1, bf);
}

// Draws the focus pill vertically centred in pRect, left edge = pRect->left + borderWidth.
// Called only for nav pane (TreeView) S5 and S6 states.
static void DrawFocusPill(HDC hdc, const RECT* pRect) {
    if (!g_hFocusPillDC) return;
    int h = pRect->bottom - pRect->top;
    if (h < PILL_H) return;

    int bw = g_borderWidth.load(std::memory_order_relaxed);
    int px = pRect->left + bw;
    int py = pRect->top + (h - PILL_H) / 2;

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(hdc, px, py, PILL_W, PILL_H,
               g_hFocusPillDC, 0, 0, PILL_W, PILL_H, bf);
}

// ── Progress bar drawing ──────────────────────────────────────────────────────

// Walk ancestor chain to find CabinetWClass — confirms we're in Explorer.
// Returns true if hwnd is NULL (buffered DC — assume This PC context).
static bool IsDriveListDC(HDC hdc) {
    HWND hwnd = WindowFromDC(hdc);
    if (!hwnd) return true;
    WCHAR cls[256];
    HWND prev = nullptr;
    for (HWND cur = hwnd; cur && cur != prev; cur = GetAncestor(cur, GA_PARENT)) {
        prev = cur;
        if (GetClassNameW(cur, cls, 256) && wcscmp(cls, L"CabinetWClass") == 0)
            return true;
    }
    return false;
}

// Left-to-right gradient fill using GradientFill (msimg32).
static void DrawGradientFill(HDC hdc, const RECT* r, COLORREF cStart, COLORREF cEnd) {
    TRIVERTEX v[2] = {};
    v[0].x = r->left;  v[0].y = r->top;
    v[1].x = r->right; v[1].y = r->bottom;
    v[0].Red   = (COLOR16)(GetRValue(cStart) << 8);
    v[0].Green = (COLOR16)(GetGValue(cStart) << 8);
    v[0].Blue  = (COLOR16)(GetBValue(cStart) << 8);
    v[1].Red   = (COLOR16)(GetRValue(cEnd) << 8);
    v[1].Green = (COLOR16)(GetGValue(cEnd) << 8);
    v[1].Blue  = (COLOR16)(GetBValue(cEnd) << 8);
    GRADIENT_RECT gr = {0, 1};
    GradientFill(hdc, v, 2, &gr, 1, GRADIENT_FILL_RECT_H);
}

// Create a rounded clip region in device coordinates for fill clipping.
// CreateRoundRectRgn works in device coords; pRect from DrawThemeBackground
// is in logical coords. Under BufferedPaint the viewport origin is shifted,
// so LPtoDP is required. +1 on right/bottom: CreateRoundRectRgn's inclusive
// right/bottom vs RECT's exclusive convention.
static HRGN CreateClipRgnForFill(HDC hdc, const RECT* logicalRect, int d) {
    POINT pts[2] = { {logicalRect->left,  logicalRect->top},
                     {logicalRect->right, logicalRect->bottom} };
    LPtoDP(hdc, pts, 2);
    return CreateRoundRectRgn(pts[0].x, pts[0].y,
                              pts[1].x + 1, pts[1].y + 1, d, d);
}

static void DrawProgressBar(HDC hdc, const RECT* pRect, int iPartId, int iStateId) {
    // DPI-scaled corner radius.
    // WindowFromDC may return NULL for buffered DCs — fall back to GetDpiForSystem.
    int radius = g_prog.progressCornerRadius;
    if (radius > 0) {
        typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
        typedef UINT (WINAPI *GetDpiForSystem_t)();
        static auto pGetDpiW = (GetDpiForWindow_t)
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
        static auto pGetDpiS = (GetDpiForSystem_t)
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForSystem");

        UINT dpi = 96;
        HWND hwnd = WindowFromDC(hdc);
        if (hwnd && pGetDpiW)
            dpi = pGetDpiW(hwnd);
        else if (pGetDpiS)
            dpi = pGetDpiS();

        radius = MulDiv(radius, (int)dpi, 96);
    }
    int d = radius * 2; // diameter for CreateRoundRectRgn

    if (iPartId == 11) {
        // ── Track background ──────────────────────────────────────────────────
        // FillRgn/FillRect accept logical coordinates — no LPtoDP needed here.
        if (d > 0) {
            HRGN hRgn = CreateRoundRectRgn(
                pRect->left,    pRect->top,
                pRect->right+1, pRect->bottom+1, d, d);
            if (hRgn) {
                HBRUSH hBr = CreateSolidBrush(g_prog.progressBg);
                if (hBr) { FillRgn(hdc, hRgn, hBr); DeleteObject(hBr); }
                DeleteObject(hRgn);
            }
        } else {
            HBRUSH hBr = CreateSolidBrush(g_prog.progressBg);
            if (hBr) { FillRect(hdc, pRect, hBr); DeleteObject(hBr); }
        }

        // ── 1 px border (optional) ────────────────────────────────────────────
        if (g_prog.showProgressBorder) {
            if (d > 0) {
                HRGN hOuter = CreateRoundRectRgn(
                    pRect->left,    pRect->top,
                    pRect->right+1, pRect->bottom+1, d, d);
                HRGN hInner = CreateRoundRectRgn(
                    pRect->left+1,  pRect->top+1,
                    pRect->right,   pRect->bottom,   d, d);
                if (hOuter && hInner) {
                    CombineRgn(hOuter, hOuter, hInner, RGN_DIFF);
                    HBRUSH hBr = CreateSolidBrush(g_prog.progressBorder);
                    if (hBr) { FillRgn(hdc, hOuter, hBr); DeleteObject(hBr); }
                }
                if (hOuter) DeleteObject(hOuter);
                if (hInner) DeleteObject(hInner);
            } else {
                HBRUSH hBr = CreateSolidBrush(g_prog.progressBorder);
                if (hBr) { FrameRect(hdc, pRect, hBr); DeleteObject(hBr); }
            }
        }
    }
    else if (iPartId == 5) {
        // ── Fill bar ──────────────────────────────────────────────────────────
        // iStateId == 2: disk full / warning gradient.
        bool isFull = (iStateId == 2);
        COLORREF cS = isFull ? g_prog.progressFillFullStart : g_prog.progressFillStart;
        COLORREF cE = isFull ? g_prog.progressFillFullEnd   : g_prog.progressFillEnd;

        // Inset left/top/bottom by 1 px when border shown so fill sits inside
        // the border ring. Right edge is Explorer-controlled (fill level).
        RECT fill = *pRect;
        if (g_prog.showProgressBorder) {
            fill.left   += 1;
            fill.top    += 1;
            fill.bottom -= 1;
        }

        if (d > 0) {
            // Clip region must be in device coords.
            // SaveDC/RestoreDC preserves any existing HDC clip (BufferedPaint).
            int saved = SaveDC(hdc);
            HRGN hClip = CreateClipRgnForFill(hdc, &fill, d);
            if (hClip) {
                ExtSelectClipRgn(hdc, hClip, RGN_AND);
                DrawGradientFill(hdc, &fill, cS, cE);
                DeleteObject(hClip);
            }
            RestoreDC(hdc, saved);
        } else {
            DrawGradientFill(hdc, &fill, cS, cE);
        }
    }
}

// Uses uxtheme ordinal 74 (GetThemeClass) to identify the Progress theme.
static bool IsProgressTheme(HTHEME hTheme) {
    typedef HRESULT (WINAPI *GetThemeClass_t)(HTHEME, LPWSTR, INT);
    static auto fn = (GetThemeClass_t)
        GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74));
    if (!fn) return false;
    WCHAR buf[64] = {};
    if (SUCCEEDED(fn(hTheme, buf, 64)))
        return wcscmp(buf, L"Progress") == 0;
    return false;
}

// ── Shell HWND detection ──────────────────────────────────────────────────────
//
// True if hwnd is an Explorer shell view or lives inside one — covers both
// explorer.exe itself (every HWND qualifies) and IFileDialog hosted in any
// process (Firefox, Brave, etc.), which creates the same SHELLDLL_DefView
// subtree inside a #32770 dialog:
//   #32770 "Open"/"Save As"
//     └─ DirectUIHWND          ← IFileDialog chrome (toolbar, breadcrumb)
//     └─ SHELLDLL_DefView      ← shell view container
//          └─ DirectUIHWND     ← theme draws happen here
//          └─ SysListView32    ← theme draws happen here
//          └─ SysTreeView32    ← nav pane
//
// For explorer.exe the process name is checked once (g_hostIsExplorer) and
// cached; elsewhere, walk up looking for SHELLDLL_DefView.

static bool IsShellHwnd(HWND hwnd) {
    // In explorer.exe every HWND is ours — no walk needed.
    if (g_hostIsExplorer) return true;

    // In other processes: only accept HWNDs that are children of a
    // SHELLDLL_DefView (IFileDialog shell view container).
    wchar_t cls[64];
    for (HWND h = hwnd; h; h = GetParent(h)) {
        cls[0] = 0;
        GetClassNameW(h, cls, 64);
        if (wcscmp(cls, L"SHELLDLL_DefView") == 0) return true;
    }
    return false;
}

// ── Theme handle registry ─────────────────────────────────────────────────────

static std::unordered_map<HTHEME, bool> g_listViewThemes;
static std::unordered_map<HTHEME, bool> g_focusRectThemes;
static std::unordered_map<HTHEME, bool> g_desktopThemes;
static std::shared_mutex                g_themeMutex;

// ── Per-thread state ──────────────────────────────────────────────────────────

static thread_local bool g_inAutoDetect = false;
static thread_local HDC  g_lastP3S2hdc  = nullptr; // HDC seen in last P3S2 call; used to detect ghost P1S3

// ── Theme registry helpers ────────────────────────────────────────────────────

static void RegisterLVTheme(HTHEME h) {
    if (!h) return;
    std::unique_lock lock(g_themeMutex);
    g_listViewThemes[h] = true;
}
static void RegisterFocusTheme(HTHEME h) {
    if (!h) return;
    std::unique_lock lock(g_themeMutex);
    g_focusRectThemes[h] = true;
}
static void RegisterDesktopTheme(HTHEME h) {
    if (!h) return;
    std::unique_lock lock(g_themeMutex);
    g_desktopThemes[h] = true;
}
static bool IsListViewTheme(HTHEME h) {
    std::shared_lock lock(g_themeMutex);
    return g_listViewThemes.count(h) > 0;
}
static bool IsFocusRectTheme(HTHEME h) {
    std::shared_lock lock(g_themeMutex);
    return g_focusRectThemes.count(h) > 0;
}
static bool IsDesktopTheme(HTHEME h) {
    std::shared_lock lock(g_themeMutex);
    return g_desktopThemes.count(h) > 0;
}

// ── Desktop detection ─────────────────────────────────────────────────────────

static std::vector<HWND> g_desktopListViewHwnds;

static void FindDesktopListViews() {
    g_desktopListViewHwnds.clear();
    auto tryAdd = [](HWND host) {
        if (!host) return;
        HWND dv = FindWindowExW(host, nullptr, L"SHELLDLL_DefView", nullptr);
        if (!dv) return;
        HWND lv = FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
        if (lv) g_desktopListViewHwnds.push_back(lv);
    };
    tryAdd(FindWindowW(L"Progman", nullptr));
    HWND w = nullptr;
    while ((w = FindWindowExW(nullptr, w, L"WorkerW", nullptr)) != nullptr)
        tryAdd(w);
}

static bool IsDesktopWindow(HWND hwnd) {
    if (!hwnd) return false;
    for (HWND lv : g_desktopListViewHwnds)
        if (lv == hwnd) return true;
    wchar_t cls[64];
    for (HWND h = hwnd; h; h = GetParent(h)) {
        cls[0] = 0;
        GetClassNameW(h, cls, 64);
        if (wcscmp(cls, L"Progman")         == 0 ||
            wcscmp(cls, L"WorkerW")          == 0 ||
            wcscmp(cls, L"SHELLDLL_DefView") == 0)
            return true;
    }
    return false;
}

// ── OpenThemeData hooks ───────────────────────────────────────────────────────
//
// Gate: only register HTHEME values for HWNDs that belong to a shell view.
// This prevents Firefox's own uxtheme handles from ever entering our registry.

// Returns true if this theme class name is an LV/ItemsView selection theme we want to intercept.
// Critically: must NOT match ScrollBar, Toolbar, Rebar, etc. that also appear inside SHELLDLL_DefView.
static bool IsLVThemeClass(LPCWSTR cls) {
    if (!cls) return false;
    return wcsstr(cls, L"ItemsView") || wcsstr(cls, L"ListView");
}

HTHEME WINAPI OpenThemeDataHook(HWND hwnd, LPCWSTR cls) {
    HTHEME h = pOrigOpenThemeData(hwnd, cls);
    if (!h) return h;

    if (!cls) return h; // nothing useful without a class name

    // WinUI3 shell pattern: opens ItemsView theme with hwnd=NULL.
    // This is the central panel selection theme. Always register it.
    if (!hwnd && IsLVThemeClass(cls)) {
        RegisterLVTheme(h);
        return h;
    }

    if (!hwnd) return h; // null hwnd with non-LV class — ignore

    // For real HWNDs: only process shell HWNDs, and only LV/ItemsView class names.
    // This excludes ScrollBar, Toolbar, Rebar etc. that happen to live inside
    // SHELLDLL_DefView — they must never enter our registry.
    if (IsDesktopWindow(hwnd)) { RegisterDesktopTheme(h); return h; }
    if (IsShellHwnd(hwnd) && IsLVThemeClass(cls))
        RegisterLVTheme(h);

    return h;
}

HTHEME WINAPI OpenThemeDataExHook(HWND hwnd, LPCWSTR cls, DWORD flags) {
    HTHEME h = pOrigOpenThemeDataEx(hwnd, cls, flags);
    if (!h) return h;

    if (!cls) return h;

    if (!hwnd && IsLVThemeClass(cls)) {
        RegisterLVTheme(h);
        return h;
    }

    if (!hwnd) return h;

    if (IsDesktopWindow(hwnd)) { RegisterDesktopTheme(h); return h; }
    if (IsShellHwnd(hwnd) && IsLVThemeClass(cls))
        RegisterLVTheme(h);

    return h;
}

HRESULT WINAPI CloseThemeDataHook(HTHEME h) {
    {
        std::unique_lock lock(g_themeMutex);
        g_listViewThemes.erase(h);
        g_focusRectThemes.erase(h);
        g_desktopThemes.erase(h);
    }
    return pOrigCloseThemeData(h);
}

// ── Startup seeding ───────────────────────────────────────────────────────────

static BOOL CALLBACK SeedProc(HWND hwnd, LPARAM) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    if (wcscmp(cls, L"SysListView32") == 0 || wcscmp(cls, L"DirectUIHWND") == 0) {
        HTHEME h = GetWindowTheme(hwnd);
        if (h) {
            if (IsDesktopWindow(hwnd)) RegisterDesktopTheme(h);
            else                       RegisterLVTheme(h);
        }
    }
    return TRUE;
}

// ── DUI70 Preview Pane background fix ─────────────────────────────────────
//
// Fixes the Preview Pane background by natively patching the DirectUI Value
// during rendering.
//
// ── DirectUI Value Offset Abstraction ───────────────────────────────────────

struct DirectUIColorPayload {
    uint8_t reserved[0x10]; // Diagnostically verified offset (+0x10)
    DWORD   color;          // ARGB / COLORREF payload
};

static std::atomic<bool> g_modInitReturned{ false };
static std::atomic<bool> g_hookInstalled{ false };

// ── Native LDR Types for DLL Load Monitoring ───────────────────────────────

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
    ULONG Flags;
    PUNICODE_STRING FullDllName;
    PUNICODE_STRING BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
    ULONG Loaded;
    LDR_DLL_LOADED_NOTIFICATION_DATA LoadedData;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef VOID (NTAPI *PLDR_DLL_NOTIFICATION_FUNCTION)(
    ULONG NotificationReason,
    PLDR_DLL_NOTIFICATION_DATA NotificationData,
    PVOID Context
);

typedef NTSTATUS (NTAPI *pfnLdrRegisterDllNotification)(
    ULONG Flags,
    PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
    PVOID Context,
    PVOID *Cookie
);

typedef NTSTATUS (NTAPI *pfnLdrUnregisterDllNotification)(
    PVOID Cookie
);

static PVOID g_dllNotificationCookie = nullptr;

static void UnregisterDllNotification();

// ── Target Atoms ────────────────────────────────────────────────────────────

static ATOM g_atomReadingPane          = 0;
static ATOM g_atomCoverSheetTransition = 0;
static ATOM g_atomCoverSheetNoSel      = 0;
static std::atomic<bool> g_atomsResolved { false };

static void EnsureAtomsResolved() {
    if (g_atomsResolved.load(std::memory_order_acquire))
        return;

    ATOM rp = AddAtomW(L"ReadingPane");
    if (!rp) return;

    g_atomReadingPane          = rp;
    g_atomCoverSheetTransition = AddAtomW(L"RPaneCoverSheet_Transition");
    g_atomCoverSheetNoSel      = AddAtomW(L"RPaneCoverSheet_NoSel");
    g_atomsResolved.store(true, std::memory_order_release);
}

inline bool IsTargetAtom(ATOM atom) {
    return (atom != 0 && (atom == g_atomReadingPane ||
                          atom == g_atomCoverSheetTransition ||
                          atom == g_atomCoverSheetNoSel));
}

// ── Pure Color Scope (Zero Domain Knowledge) ────────────────────────────────

struct DirectUIColorScope {
    DWORD* pColorField = nullptr;
    DWORD  origColor   = 0;

    DirectUIColorScope(DWORD* pTargetColor, COLORREF newColorRef) {
        if (!pTargetColor) return;

        pColorField = pTargetColor;
        origColor   = *pColorField;

        BYTE r = GetRValue(newColorRef);
        BYTE g = GetGValue(newColorRef);
        BYTE b = GetBValue(newColorRef);

        DWORD alpha = origColor & 0xFF000000;
        *pColorField = alpha | (r) | (g << 8) | (b << 16);
    }

    ~DirectUIColorScope() {
        if (pColorField) {
            *pColorField = origColor;
        }
    }
};

// ── Hooks ───────────────────────────────────────────────────────────────────

void __fastcall PaintBackgroundHook(
    void* pThis, void* hdc, void* pValue,
    const RECT* prcBounds, const RECT* prcInvalid,
    const RECT* a6, const RECT* a7)
{
    ATOM atom = 0;
    if (g_prog.detailsBgMatchEnabled) {
        EnsureAtomsResolved();
        if (pGetID && pThis) {
            atom = pGetID(pThis);
        }
    }

    if (IsTargetAtom(atom) && pValue != nullptr) {
        auto* payload = static_cast<DirectUIColorPayload*>(pValue);
        DirectUIColorScope scope(&payload->color, g_prog.previewPaneBgColor);

        pOrigPaintBackground(pThis, hdc, pValue, prcBounds, prcInvalid, a6, a7);
    } else {
        pOrigPaintBackground(pThis, hdc, pValue, prcBounds, prcInvalid, a6, a7);
    }
}

// ── DUI70 Dynamic Installation ──────────────────────────────────────────────

static std::atomic<bool> g_readyLogged{ false };
static void LogReadyOnce() {
    if (!g_readyLogged.exchange(true)) {
        Wh_Log(L"Explorer Visual Tweaks Dark v1.0.0 ready — theme hooks installed, "
               L"DirectUI atoms resolved, Preview Pane fix active.");
    }
}

static bool TryInstallPreviewPaneFix() {
    if (pOrigPaintBackground) return true;

    HMODULE hDUI70 = GetModuleHandleW(L"DUI70.dll");
    if (!hDUI70) return false;

    void* pPaintBg = (void*)GetProcAddress(hDUI70,
        "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@AEBUtagRECT@@222@Z");
    pGetID = (GetID_t)GetProcAddress(hDUI70, "?GetID@Element@DirectUI@@QEAAGXZ");

    if (!pPaintBg || !pGetID) {
        Wh_Log(L"ERROR: Failed to resolve DirectUI exports from DUI70.dll.");
        return false;
    }

    if (Wh_SetFunctionHook(pPaintBg, (void*)PaintBackgroundHook, (void**)&pOrigPaintBackground)) {
        UnregisterDllNotification();

        if (g_modInitReturned.load(std::memory_order_acquire)) {
            Wh_ApplyHookOperations();
        }
        return true;
    } else {
        Wh_Log(L"ERROR: Wh_SetFunctionHook failed for PaintBackground.");
    }

    return false;
}

// ── DLL Load Watcher Callback ───────────────────────────────────────────────

static VOID NTAPI DllNotificationCallback(
    ULONG NotificationReason,
    PLDR_DLL_NOTIFICATION_DATA NotificationData,
    PVOID Context)
{
    // 1 = LDR_DLL_NOTIFICATION_REASON_LOADED
    if (NotificationReason == 1 && NotificationData && NotificationData->LoadedData.BaseDllName) {
        if (NotificationData->LoadedData.BaseDllName->Buffer) {
            if (_wcsicmp(NotificationData->LoadedData.BaseDllName->Buffer, L"DUI70.dll") == 0) {
                if (!g_hookInstalled.exchange(true)) {
                    if (!TryInstallPreviewPaneFix()) {
                        g_hookInstalled.store(false, std::memory_order_release);
                    } else {
                        LogReadyOnce();
                    }
                }
            }
        }
    }
}

static void RegisterDllNotification() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return;

    auto pfnRegister = (pfnLdrRegisterDllNotification)GetProcAddress(hNtdll, "LdrRegisterDllNotification");
    if (!pfnRegister) {
        Wh_Log(L"ERROR: LdrRegisterDllNotification is unavailable.");
        return;
    }

    PVOID cookie = nullptr;
    NTSTATUS status = pfnRegister(0, DllNotificationCallback, nullptr, &cookie);
    if (NT_SUCCESS(status)) {
        g_dllNotificationCookie = cookie;
    } else {
        Wh_Log(L"ERROR: LdrRegisterDllNotification failed with status: 0x%X", status);
    }
}

static void UnregisterDllNotification() {
    if (!g_dllNotificationCookie) return;

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return;

    auto pfnUnregister = (pfnLdrUnregisterDllNotification)GetProcAddress(hNtdll, "LdrUnregisterDllNotification");
    if (!pfnUnregister) {
        Wh_Log(L"ERROR: LdrUnregisterDllNotification is unavailable.");
        return;
    }

    NTSTATUS status = pfnUnregister(g_dllNotificationCookie);
    if (NT_SUCCESS(status)) {
        g_dllNotificationCookie = nullptr;
    } else {
        Wh_Log(L"ERROR: LdrUnregisterDllNotification failed with status: 0x%X", status);
    }
}

// ── Draw hooks ────────────────────────────────────────────────────────────────
//
// DrawThemeBackground  → TreeView / nav pane (SysTreeView32), uses BufferedPaint per item.
// DrawThemeBackgroundEx → central panel (WinUI3 / DirectComposition).
//
// Part/State mapping (confirmed via logging):
//   P3S2  = ghost/transition bg (focus rect theme or LV theme, size > 30×15)
//   P1S2  = hover (not selected)        → slot 0 (active style)
//   P1S3  = pressed/click
//   P1S5  = selected, no keyboard focus → pass-through (system dark bg kept as-is)
//   P1S6  = active selection            → slot 0 (active)
//   P11/5 = progress bar track/fill     → DrawProgressBar (explorer.exe only, CabinetWClass check)
//
// Ghost suppression (nav pane):
//   P3S2 sets g_lastP3S2hdc. A P1S3 on the same HDC is a ghost transition —
//   suppress it (return S_OK, no draw). This clears the old item's custom bg
//   without leaving a visible artefact, because each TreeView item renders
//   into its own BufferedPaint HDC.
//
// These hooks only act on HTHEME handles registered via OpenThemeData(Ex)Hook —
// i.e. handles for shell-view HWNDs only.
// Unknown handles (Firefox's own themes, Notepad++ etc.) fall through immediately.

HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    const RECT* pRect, const RECT* pClipRect)
{
    if (IsDesktopTheme(hTheme))
        return pOrigDrawThemeBg(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

    if (pRect) {
        // Progress bars (This PC disk usage): part=11 track, part=5 fill.
        // IsDriveListDC ensures we only fire in CabinetWClass context.
        if ((iPartId == 11 || iPartId == 5) &&
            IsProgressTheme(hTheme) && IsDriveListDC(hdc))
        {
            DrawProgressBar(hdc, pRect, iPartId, iStateId);
            return S_OK;
        }

        // Part 3: ghost / transition background
        if (iPartId == 3) {
            bool isFocus = IsFocusRectTheme(hTheme);
            if (!isFocus && !g_inAutoDetect) {
                g_inAutoDetect = true;
                isFocus = IsThemePartDefined(hTheme, 3, 1) &&
                          !IsThemePartDefined(hTheme, 11, 0);
                g_inAutoDetect = false;
                if (isFocus) RegisterFocusTheme(hTheme);
            }
            if (isFocus || IsListViewTheme(hTheme)) {
                int w = pRect->right - pRect->left, h = pRect->bottom - pRect->top;
                if (w > 30 && h > 15) {
                    if (iStateId == 2) {
                        g_lastP3S2hdc = hdc;
                        DrawNinePatch(hdc, pRect, 0);
                        return S_OK;
                    }
                    return S_OK;
                }
                return S_OK;
            }
        }

        // Part 1: nav pane item background (TreeView / SysTreeView32)
        if (iPartId == 1) {
            bool isLV = IsListViewTheme(hTheme);
            if (!isLV && !g_inAutoDetect) {
                g_inAutoDetect = true;
                isLV = IsThemePartDefined(hTheme, 1, 3) &&
                       IsThemePartDefined(hTheme, 1, 6);
                g_inAutoDetect = false;
                if (isLV) RegisterLVTheme(hTheme);
            }

            if (isLV) {
                if (iStateId == 2) {
                    DrawNinePatch(hdc, pRect, 0); // hover — same style as active selection
                    return S_OK;
                }
                if (iStateId == 3) {
                    bool ctrlShift = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                                     (GetAsyncKeyState(VK_SHIFT)   & 0x8000);
                    if (ctrlShift) {
                        DrawNinePatch(hdc, pRect, 1); // multi-select, no pill
                        return S_OK;
                    }
                    // Suppress without drawing: clears old item's custom bg.
                    // System follows with S6 on a fresh HDC for the new item.
                    return S_OK;
                }
                if (iStateId == 5) {
                    // Selected, no keyboard focus: keep system background, add pill.
                    pOrigDrawThemeBg(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
                    DrawFocusPill(hdc, pRect);
                    return S_OK;
                }
                if (iStateId == 6) {
                    DrawNinePatch(hdc, pRect, 0);
                    DrawFocusPill(hdc, pRect);
                    return S_OK;
                }
            }
        }
    }
    return pOrigDrawThemeBg(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT WINAPI HookedDrawThemeBackgroundEx(
    HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    const RECT* pRect, const DTBGOPTS* pOptions)
{
    if (IsDesktopTheme(hTheme))
        return pOrigDrawThemeBgEx(hTheme, hdc, iPartId, iStateId, pRect, pOptions);

    if (pRect) {
        // Progress bars (This PC disk usage): part=11 track, part=5 fill.
        if ((iPartId == 11 || iPartId == 5) &&
            IsProgressTheme(hTheme) && IsDriveListDC(hdc))
        {
            DrawProgressBar(hdc, pRect, iPartId, iStateId);
            return S_OK;
        }

        // Part 3: ghost / transition background
        if (iPartId == 3) {
            bool isFocus = IsFocusRectTheme(hTheme);
            if (!isFocus && !g_inAutoDetect) {
                g_inAutoDetect = true;
                isFocus = IsThemePartDefined(hTheme, 3, 1) &&
                          !IsThemePartDefined(hTheme, 11, 0);
                g_inAutoDetect = false;
                if (isFocus) RegisterFocusTheme(hTheme);
            }
            if (isFocus || IsListViewTheme(hTheme)) {
                int w = pRect->right - pRect->left, h = pRect->bottom - pRect->top;
                if (w > 30 && h > 15) {
                    if (iStateId == 2) {
                        g_lastP3S2hdc = hdc;
                        DrawNinePatch(hdc, pRect, 0);
                        return S_OK;
                    }
                    return S_OK;
                }
                return S_OK;
            }
        }

        // Part 1: central panel item background (WinUI3 / DirectComposition)
        if (iPartId == 1) {
            bool isLV = IsListViewTheme(hTheme);
            if (!isLV && !g_inAutoDetect) {
                g_inAutoDetect = true;
                isLV = IsThemePartDefined(hTheme, 1, 3) &&
                       IsThemePartDefined(hTheme, 1, 6);
                g_inAutoDetect = false;
                if (isLV) RegisterLVTheme(hTheme);
            }
            if (isLV) {
                if (iStateId == 2) {
                    DrawNinePatch(hdc, pRect, 0); // hover — same style as active selection
                    return S_OK;
                }
                if (iStateId == 3) {
                    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                        (GetAsyncKeyState(VK_SHIFT)   & 0x8000)) {
                        DrawNinePatch(hdc, pRect, 1); // multi-select
                        return S_OK;
                    }
                    if (hdc == g_lastP3S2hdc) return S_OK;
                    DrawNinePatch(hdc, pRect, 1); // single press in central panel → multi style
                    return S_OK;
                }
                if (iStateId == 5) {
                    // Selected, no keyboard focus: keep system background as-is
                    return pOrigDrawThemeBgEx(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
                }
                if (iStateId == 6) {
                    DrawNinePatch(hdc, pRect, 0); // no pill in central panel
                    return S_OK;
                }
            }
        }
    }
    return pOrigDrawThemeBgEx(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
}

// ── Settings application ──────────────────────────────────────────────────────

static void ApplySettings() {
    int vr = GetIntOr(L"cornerRadius", 2);
    if (vr < 1) vr = 1;
    if (vr > 5) vr = 5;
    int m = vr * 2;

    int bw = Wh_GetIntSetting(L"showBorder") ? 1 : 0;
    g_borderWidth.store(bw, std::memory_order_relaxed);

    BYTE aFR, aFG, aFB, aBR, aBG, aBB;
    BYTE mFR, mFG, mFB, mBR, mBG, mBB;
    BYTE pR,  pG,  pB;

    ParseHexColor(L"activeFillColor",   L"4D4D4D", aFR, aFG, aFB);
    ParseHexColor(L"activeBorderColor", L"555555", aBR, aBG, aBB);
    ParseHexColor(L"multiFillColor",    L"454545", mFR, mFG, mFB);
    ParseHexColor(L"multiBorderColor",  L"505050", mBR, mBG, mBB);
    ParseHexColor(L"focusPillColor",    L"4CC2FF", pR,  pG,  pB);

    InitCustomNinePatch(0, m, bw, aFR, aFG, aFB, aBR, aBG, aBB);
    InitCustomNinePatch(1, m, bw, mFR, mFG, mFB, mBR, mBG, mBB);
    InitFocusPill(pR, pG, pB);

    // Progress / Details pane settings
    g_prog.progressCornerRadius = Wh_GetIntSetting(L"progressCornerRadius");
    if (g_prog.progressCornerRadius < 0) g_prog.progressCornerRadius = 0;

    g_prog.showProgressBorder = Wh_GetIntSetting(L"showProgressBorder") != 0;

    // Preview Pane background fix (DUI70 PaintBackground hook).
    g_prog.detailsBgMatchEnabled = Wh_GetIntSetting(L"matchDetailsPaneBg") != 0;
    g_prog.previewPaneBgColor    = ParseHexColorRef(L"previewPaneBgColor", RGB(0x19, 0x19, 0x19));

    g_prog.progressBorder        = ParseHexColorRef(L"progressBorderColor",       RGB(0x40, 0x40, 0x40));
    g_prog.progressBg            = ParseHexColorRef(L"progressBgColor",            RGB(0x58, 0x58, 0x58));
    g_prog.progressFillStart     = ParseHexColorRef(L"progressFillColorStart",     RGB(0x00, 0x78, 0xD7));
    g_prog.progressFillEnd       = ParseHexColorRef(L"progressFillColorEnd",       RGB(0x00, 0x94, 0xFE));
    g_prog.progressFillFullStart = ParseHexColorRef(L"progressFillColorFullStart", RGB(0xE4, 0x30, 0x60));
    g_prog.progressFillFullEnd   = ParseHexColorRef(L"progressFillColorFullEnd",   RGB(0xED, 0x60, 0x50));
}

// ── Mod lifecycle ─────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    // Cache whether we are running inside explorer.exe.
    // Used by IsShellHwnd (skip parent walk in explorer.exe).
    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    g_hostIsExplorer = (_wcsicmp(exeName, L"explorer.exe") == 0);

    // Probe desktop access before any GDI calls. Sandboxed renderer processes
    // (Firefox e10s, Chrome GPU/renderer) block Win32k syscalls, so
    // GetDC(NULL) returns NULL there — explorer.exe always has access, so
    // the probe is skipped for it. WSF_VISIBLE is deliberately not used:
    // explorer.exe can be injected before attaching to the visible window
    // station, which would give a false "no desktop" negative here.
    if (g_hostIsExplorer) {
        g_hasDesktopAccess = true;
    } else {
        HDC hdcProbe = GetDC(NULL);
        if (hdcProbe) {
            ReleaseDC(NULL, hdcProbe);
            g_hasDesktopAccess = true;
        } else {
            g_hasDesktopAccess = false;
        }
    }

    // Nothing here can render without a desktop (selection/progress/pill all
    // need GDI; Preview Pane match is explorer.exe-only anyway). Sandboxed
    // child processes never own a visible themed dialog themselves — an
    // unsandboxed broker does — so skip hook installation entirely rather
    // than installing hooks that can only ever no-op.
    if (!g_hasDesktopAccess) return TRUE;

    // GDI resources are needed in any process that may render IFileDialog shell content.
    ApplySettings();

    // Text Preview Dark module: best-effort, a failure here only disables
    // this one module — it never fails the rest of Wh_ModInit.
    if (!TextPreviewDark::Init(g_prog.previewPaneBgColor)) {
        Wh_Log(L"WARNING: TextPreviewDark::Init failed — Preview Pane text view fix disabled");
    }

    // LoadLibraryW, not GetModuleHandleW: in explorer.exe uxtheme.dll is
    // already loaded, but in a foreign process added via the Inclusion List
    // (Firefox, Notepad, etc.) it may not be loaded yet at this point,
    // depending on injection timing. Explicitly loading it guarantees the
    // exports below can always be resolved, regardless of host process.
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme) { Wh_Log(L"ERROR: uxtheme.dll failed to load"); return FALSE; }

    pOrigOpenThemeData   = (OpenThemeData_t)  (void*)GetProcAddress(hUxTheme, "OpenThemeData");
    pOrigOpenThemeDataEx = (OpenThemeDataEx_t)(void*)GetProcAddress(hUxTheme, "OpenThemeDataEx");
    pOrigCloseThemeData  = (CloseThemeData_t) (void*)GetProcAddress(hUxTheme, "CloseThemeData");
    void* pBg            = (void*)GetProcAddress(hUxTheme, "DrawThemeBackground");
    void* pBgEx          = (void*)GetProcAddress(hUxTheme, "DrawThemeBackgroundEx");

    if (pOrigOpenThemeData)
        Wh_SetFunctionHook((void*)pOrigOpenThemeData,   (void*)OpenThemeDataHook,          (void**)&pOrigOpenThemeData);
    if (pOrigOpenThemeDataEx)
        Wh_SetFunctionHook((void*)pOrigOpenThemeDataEx, (void*)OpenThemeDataExHook,        (void**)&pOrigOpenThemeDataEx);
    if (pOrigCloseThemeData)
        Wh_SetFunctionHook((void*)pOrigCloseThemeData,  (void*)CloseThemeDataHook,         (void**)&pOrigCloseThemeData);
    if (pBg)
        Wh_SetFunctionHook(pBg,          (void*)HookedDrawThemeBackground,   (void**)&pOrigDrawThemeBg);
    if (pBgEx)
        Wh_SetFunctionHook(pBgEx,        (void*)HookedDrawThemeBackgroundEx, (void**)&pOrigDrawThemeBgEx);

    if (!pOrigDrawThemeBg || !pOrigDrawThemeBgEx) {
        Wh_Log(L"ERROR: failed to hook DrawThemeBackground(Ex)");
        return FALSE;
    }

    // Deliberately explorer.exe-only for now, even though nothing here is
    // technically explorer-specific (DUI70.dll, the atom names, and
    // PaintBackground are the same shared shell components in any process).
    // Kept scoped because: 1) unlike the selection-highlight hooks above,
    // this one is only verified against explorer.exe's own Preview Pane;
    // 2) it matches DirectUI elements purely by atom name, so a future
    // Windows build (or some other app) reusing "ReadingPane" or
    // "RPaneCoverSheet_*" for an unrelated element elsewhere in the UI
    // would repaint that element too. Lifting this to other processes
    // should wait for that to be checked against real hosts first.
    //
    // Try immediate installation if DUI70 is already in memory; if it isn't
    // loaded yet, watch for its future load event via
    // LdrRegisterDllNotification (see TryInstallPreviewPaneFix /
    // RegisterDllNotification above).
    if (g_hostIsExplorer) {
        if (!TryInstallPreviewPaneFix()) {
            RegisterDllNotification();
        } else {
            LogReadyOnce();
        }
    }

    // Desktop exclusion and seeding only make sense in explorer.exe.
    if (g_hostIsExplorer) {
        FindDesktopListViews();
        for (HWND lv : g_desktopListViewHwnds) {
            HTHEME h = GetWindowTheme(lv);
            if (h) RegisterDesktopTheme(h);
        }
        if (g_desktopListViewHwnds.empty())
            Wh_Log(L"WARNING: no Desktop ListView found");
        EnumWindows(SeedProc, 0);
    }

    // Set only once Wh_ModInit is about to return, so TryInstallPreviewPaneFix
    // can tell (via this flag) whether a later, notification-triggered
    // install happened after Windhawk's normal end-of-Wh_ModInit hook
    // application already ran — in which case it must call
    // Wh_ApplyHookOperations itself.
    g_modInitReturned.store(true, std::memory_order_release);
    return TRUE;
}

void Wh_ModAfterInit() {
    TextPreviewDark::AfterInit();
}

void Wh_ModSettingsChanged() {
    ApplySettings();
    TextPreviewDark::UpdateBackgroundColor(g_prog.previewPaneBgColor);
}

void Wh_ModUninit() {
    TextPreviewDark::Uninit();

    // Cancel a pending DUI70 load watch (if the hook was never installed)
    // and release the atoms acquired via AddAtomW in EnsureAtomsResolved.
    UnregisterDllNotification();
    if (g_atomReadingPane) { DeleteAtom(g_atomReadingPane); g_atomReadingPane = 0; }
    if (g_atomCoverSheetTransition) { DeleteAtom(g_atomCoverSheetTransition); g_atomCoverSheetTransition = 0; }
    if (g_atomCoverSheetNoSel) { DeleteAtom(g_atomCoverSheetNoSel); g_atomCoverSheetNoSel = 0; }

    {
        std::unique_lock lock(g_themeMutex);
        g_listViewThemes.clear();
        g_focusRectThemes.clear();
        g_desktopThemes.clear();
    }
    g_desktopListViewHwnds.clear();
    FreeNinePatch(0);
    FreeNinePatch(1);
    FreeFocusPill();
}
