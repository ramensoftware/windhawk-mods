// ==WindhawkMod==
// @id              no-system-frame
// @name            No System Frame For Selected Apps
// @description     Stops Windows from drawing its classic frame and title bar over programs that draw their own, such as Photoshop, DaVinci Resolve and Discord
// @name:ru         Без системной рамки для выбранных программ
// @description:ru  Не даёт Windows рисовать классическую рамку и заголовок поверх программ, которые рисуют их сами, - Photoshop, DaVinci Resolve, Discord и других
// @version         3.5
// @author          appEW
// @github          https://github.com/appEW
// @include         *
// @compilerOptions -lshlwapi -lcomctl32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No System Frame For Selected Apps

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

Some programs draw their own title bar. With the classic theme Windows draws
one as well, and because those windows have little or no non-client area left,
that drawing lands **on top of the program's own content**:

* a blue classic caption over the menu bar in **Adobe Photoshop** and
  **DaVinci Resolve**,
* a light 4 pixel frame around **Discord** and other Chromium/Electron windows.

Chromium and Firefox guard against this internally, which is why they look
fine. This mod adds the same guard from the outside, for a list of programs you
choose.

## The three modes

Which one a program needs depends on how it handles its non-client area, and
there is no way to tell from the outside.

For the light frame band around a Chromium/Electron window, go straight to
**Dark window frame** - it is the only thing that was measured to work, and it
costs one call per window.

### Do not paint the frame

For programs that hand their non-client messages to `DefWindowProc`, which then
paints a caption over their own title bar. **Photoshop, DaVinci Resolve.**

The mod hooks `DefWindowProcW`/`DefWindowProcA` and drops the drawing:
`WM_NCPAINT`, `WM_NCUAHDRAWCAPTION` (`0xAE`) and `WM_NCUAHDRAWFRAME` (`0xAF`)
are answered without painting, and `WM_NCACTIVATE` is answered without
repainting the caption.

`WM_SETTEXT`/`WM_SETICON` are a trap here. `DefWindowProc` repaints the caption
for them directly, and the textbook cure is to clear `WS_VISIBLE` around the
call - **do not**: taskbar replacements watch for that transient state and drop
the window from their list permanently. The mod lets the title update normally
and repaints the client over the caption pixels instead.

The program's own window procedure still receives every message, so its title
bar, hit testing and resizing keep working. Only the system's painting is lost.

### Remove the frame

For programs that keep a non-client band of their own and let Windows fill it
with the classic 3D frame. **Discord and other Chromium/Electron apps.**

Chromium keeps a few pixels of non-client area down the sides and along the
bottom on purpose, so that Windows handles resizing natively; with DWM nothing
is ever drawn there, but the classic theme fills it with `COLOR_3DFACE`. It
answers `WM_NCCALCSIZE` itself and **never calls `DefWindowProc`**, so the mode
above cannot reach it, and neither can blocking the paint.

So the mod subclasses the window - which puts it ahead of the program's own
procedure - and answers `WM_NCCALCSIZE` with the whole window rect. The band
stops existing, so nothing can be drawn into it.

That would normally kill resizing, since the resize grips lived in that band.
The mod puts them back without any resize loop of its own: it answers
`WM_NCHITTEST` for the outer few pixels with the usual `HTLEFT` / `HTTOPRIGHT`
/ ... codes, and passes `WM_NCLBUTTONDOWN` on those codes straight to
`DefWindowProc`, over the top of the program that would otherwise swallow it.
From there the system's own resize loop runs as it always does.

### Dark window frame

For a dark program whose frame band Windows paints light. **Discord.**

A Chromium/Electron window keeps `WS_CAPTION`/`WS_THICKFRAME` even with
`frame: false`, because those styles are what give it native snapping,
resizing, animations and a shadow. It then answers `WM_NCCALCSIZE` leaving a
band of a few pixels down the sides and along the bottom - deliberately, so
that **Windows keeps handling the resize**. DWM fills that band, and it takes
its colour from the app light/dark setting, not from the classic palette. So a
dark app on a light-themed system gets a light band around it.

This mode asks DWM for a dark frame on that window only
(`DWMWA_USE_IMMERSIVE_DARK_MODE`), which leaves the rest of the system light.

Measured on Discord: band `243,243,243` -> `32,32,32`, against content
`44,45,50`. For comparison, `DWMWA_BORDER_COLOR`, `DWMWA_CAPTION_COLOR` and
`DWMWA_NCRENDERING_POLICY` all return `S_OK` and change nothing, and
`DWMWA_SYSTEMBACKDROP_TYPE = NONE` only makes the band transparent, so it picks
up whatever happens to be behind the window.

## Maximized windows: not solved

Windows places a maximized window one border width outside the work area on
every side, so on a multi-monitor desktop a few pixels of its frame are drawn
on the screen next door. Three ways to stop that were implemented and measured
on Windows 11 24H2, and **none of them work**:

* `SetWindowRgn` clipping the overhang away - region verified present and
  geometrically correct, frame still drawn outside it, pixel-identical to no
  region at all. A window region clips the window's own painting, not what DWM
  composites around it. This is also why `classic-maximized-windows-fix`, whose
  whole mechanism is `SetWindowRgn`, does nothing here.
* `WM_GETMINMAXINFO` answered with the work area.
* `WM_WINDOWPOSCHANGING` clamped to the work area.

For the last two the subclass was confirmed to be receiving messages - the same
subclass answering `WM_NCCALCSIZE` visibly changes the client area - and the
maximized rect still came out as the work area inflated by the border width.
Chromium re-asserts its own placement.

What does help is **Dark window frame**: the overhang is still drawn, but dark
instead of light, so on a dark program it stops being obvious.

## Notes

* Only real application windows are handled: top level, not a tool window, not
  owned by another window. Dialogs and palettes keep their normal caption.
* The mod runs **inside** the program, so Windhawk must be allowed to inject
  into it. Check both Windhawk's global process exclusion list and this mod's
  own *Exclude* list under *Advanced*.
* This is meant for a classic theme setup. It does nothing on a themed system.

---

## По-русски

Некоторые программы рисуют заголовок окна сами. При классической теме Windows
рисует свой заголовок тоже, и поскольку у таких окон почти не осталось
неклиентской области, он оказывается **поверх содержимого программы**:

* синий классический заголовок поверх строки меню в **Adobe Photoshop** и
  **DaVinci Resolve**;
* светлая рамка в 4 пикселя вокруг **Discord** и других окон Chromium/Electron.

Chromium и Firefox защищаются от этого изнутри, поэтому выглядят нормально.
Мод добавляет такую же защиту снаружи - для списка программ, который вы
задаёте в настройках.

### Режимы

* **Do not paint the frame** (не рисовать рамку) - для программ, которые
  передают неклиентские сообщения в `DefWindowProc`: Photoshop, DaVinci
  Resolve. Системная отрисовка заголовка отбрасывается, всё остальное -
  заголовок программы, перетаскивание, изменение размера - продолжает работать.
* **Remove the frame** (убрать рамку) - для программ, которые оставляют себе
  узкую неклиентскую полосу и дают Windows залить её классической рамкой:
  Discord и другие Chromium/Electron. Полоса убирается, а зоны изменения
  размера по краям окна мод возвращает сам.
* **Dark window frame** (тёмная рамка) - для тёмной программы, вокруг которой
  Windows рисует светлую полосу рамки: Discord. Мод просит DWM нарисовать
  тёмную рамку только для этого окна. Для светлой полосы вокруг окна
  Chromium/Electron начинайте с этого режима - только он проверенно помогает.

Развёрнутое окно Windows ставит на ширину рамки за пределы рабочей области,
поэтому на соседнем мониторе видно несколько пикселей его рамки. Три способа
это убрать были проверены на Windows 11 24H2, и ни один не работает; режим
**Dark window frame** делает эту полосу хотя бы тёмной.

Мод работает **внутри** программы, поэтому Windhawk должен иметь возможность
в неё внедриться: проверьте глобальный список исключений Windhawk и список
*Exclude* этого мода на вкладке *Advanced*. Мод предназначен для классической
темы и ничего не делает при обычной теме.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- programs:
  - - name: Photoshop.exe
      $name: Program
      $name:ru: Программа
      $description: >-
        File name or full path. '*' matches any number of characters, '?'
        matches a single one.
      $description:ru: >-
        Имя файла или полный путь. '*' - любое число любых символов, '?' - один
        символ.
    - mode: nopaint
      $name: Mode
      $name:ru: Режим
      $options:
      - nopaint: Do not paint the frame - for programs that use DefWindowProc
      - strip: Remove the frame - subclasses the window, for Chromium/Electron
      - darkframe: Dark window frame - for a dark app with a light frame band
      $options:ru:
      - nopaint: Не рисовать рамку - для программ, использующих DefWindowProc
      - strip: Убрать рамку - подмена оконной процедуры, для Chromium/Electron
      - darkframe: Тёмная рамка окна - для тёмной программы со светлой полосой рамки
  - - name: Resolve.exe
    - mode: nopaint
  - - name: Discord.exe
    - mode: darkframe
  $name: Programs
  $name:ru: Программы
  $description: >-
    The programs Windows must not draw a frame for. The first matching entry
    wins; other programs are left alone.
  $description:ru: >-
    Программы, которым Windows не должна рисовать рамку. Срабатывает первая
    подходящая запись; остальные программы не затрагиваются.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shlwapi.h>

// Undocumented messages user32 sends to have the caption and frame drawn.
#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME 0x00AF
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

enum class Mode {
    NoPaint,
    Strip,
    DarkFrame,
};

Mode g_mode = Mode::NoPaint;

using DefWindowProcW_t = decltype(&DefWindowProcW);
using DefWindowProcA_t = decltype(&DefWindowProcA);
using CreateWindowExW_t = decltype(&CreateWindowExW);

DefWindowProcW_t DefWindowProcW_Orig;
DefWindowProcA_t DefWindowProcA_Orig;
CreateWindowExW_t CreateWindowExW_Orig;

////////////////////////////////////////////////////////////////////////////////
// Which windows to touch

bool IsTargetWindow(HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    if (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) {
        return false;
    }

    if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }

    // Dialogs and floating palettes are owned by the main window; leaving them
    // alone keeps their normal caption.
    return GetWindow(hWnd, GW_OWNER) == nullptr;
}

// Makes the window recalculate its frame and repaint the client area, so a
// caption the system already painted there disappears.
void RefreshFrame(HWND hWnd) {
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                     SWP_NOACTIVATE | SWP_FRAMECHANGED);
    RedrawWindow(hWnd, nullptr, nullptr,
                 RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

// DWM fills the frame band a Chromium window keeps for its resize grips, and
// picks the colour from the app light/dark setting - so a dark app on a light
// system ends up ringed in light grey. This asks for a dark frame on this one
// window, leaving the rest of the system alone.
void ApplyDarkFrame(HWND hWnd, bool dark) {
    BOOL value = dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                          sizeof(value));
}

////////////////////////////////////////////////////////////////////////////////////
// Maximized windows

////////////////////////////////////////////////////////////////////////////////
// Resize grips for "remove the frame"
//
// Taking the non-client band away takes the resize grips with it. Rather than
// running a resize loop of its own, the mod hands the edges back to Windows:
// report the usual hit test codes for them, and let DefWindowProc see the
// click that follows.

// How wide the grip is once the band itself is gone.
int GripThickness(HWND hWnd, int metric) {
    UINT dpi = GetDpiForWindow(hWnd);
    if (!dpi) {
        return GetSystemMetrics(metric) + GetSystemMetrics(SM_CXPADDEDBORDER);
    }

    return GetSystemMetricsForDpi(metric, dpi) +
           GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
}

LRESULT EdgeHitTest(HWND hWnd, LPARAM lParam) {
    // A maximized window has no edges to grab, and a window without a sizing
    // border was never resizable to begin with.
    if (IsZoomed(hWnd) ||
        !(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_THICKFRAME)) {
        return HTNOWHERE;
    }

    RECT rect;
    if (!GetWindowRect(hWnd, &rect)) {
        return HTNOWHERE;
    }

    int gripX = GripThickness(hWnd, SM_CXSIZEFRAME);
    int gripY = GripThickness(hWnd, SM_CYSIZEFRAME);
    int x = (int)(short)LOWORD(lParam);
    int y = (int)(short)HIWORD(lParam);

    bool left = x < rect.left + gripX;
    bool right = x >= rect.right - gripX;
    bool top = y < rect.top + gripY;
    bool bottom = y >= rect.bottom - gripY;

    if (top && left) return HTTOPLEFT;
    if (top && right) return HTTOPRIGHT;
    if (bottom && left) return HTBOTTOMLEFT;
    if (bottom && right) return HTBOTTOMRIGHT;
    if (left) return HTLEFT;
    if (right) return HTRIGHT;
    if (top) return HTTOP;
    if (bottom) return HTBOTTOM;
    return HTNOWHERE;
}

bool IsResizeHitCode(WPARAM hit) {
    return hit >= HTLEFT && hit <= HTBOTTOMRIGHT;
}

////////////////////////////////////////////////////////////////////////////////
// The subclass
//
// Needed in both modes: for "remove the frame" it is the only place that can
// answer WM_NCCALCSIZE ahead of a program that never calls DefWindowProc, and
// in either mode it is where a maximized window gets noticed. In "do not paint
// the frame" mode it swallows nothing at all.

LRESULT CALLBACK FrameSubclassProc(HWND hWnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCCALCSIZE:
            if (g_mode != Mode::Strip) {
                break;
            }
            // Leaving the rectangle untouched makes the client area cover the
            // whole window, so no band is left to draw into.
            return 0;

        case WM_NCHITTEST: {
            if (g_mode != Mode::Strip) {
                break;
            }
            LRESULT hit = EdgeHitTest(hWnd, lParam);
            if (hit != HTNOWHERE) {
                return hit;
            }
            break;
        }

        case WM_NCLBUTTONDOWN:
            // The program would swallow this - it does not believe it has a
            // frame. Handing it to DefWindowProc starts the system's own
            // resize loop, which is all the edges ever needed.
            if (g_mode == Mode::Strip && IsResizeHitCode(wParam)) {
                return DefWindowProcW(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_SETTINGCHANGE:
            // A light/dark switch is broadcast as ImmersiveColorSet and resets
            // the frame back to the system setting.
            if (g_mode == Mode::DarkFrame && lParam &&
                wcscmp(reinterpret_cast<PCWSTR>(lParam), L"ImmersiveColorSet") ==
                    0) {
                ApplyDarkFrame(hWnd, true);
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void AttachWindow(HWND hWnd) {
    if (!IsTargetWindow(hWnd)) {
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, FrameSubclassProc,
                                                      0)) {
        WCHAR className[64] = L"";
        GetClassNameW(hWnd, className, ARRAYSIZE(className));
        Wh_Log(L"Subclassed %08X (%s)", (DWORD)(ULONG_PTR)hWnd, className);
    }

    if (g_mode == Mode::DarkFrame) {
        ApplyDarkFrame(hWnd, true);
    }

    // A caption the system painted before the mod attached stays on screen
    // until something repaints over it.
    RefreshFrame(hWnd);
}

void DetachWindow(HWND hWnd) {
    if (!IsTargetWindow(hWnd)) {
        return;
    }

    if (g_mode == Mode::DarkFrame) {
        ApplyDarkFrame(hWnd, false);
    }

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, FrameSubclassProc);
    RefreshFrame(hWnd);
}

////////////////////////////////////////////////////////////////////////////////
// "Do not paint the frame": DefWindowProc, for programs that do call it

LRESULT HandleFrameMessage(HWND hWnd,
                           UINT uMsg,
                           WPARAM wParam,
                           LPARAM lParam,
                           bool unicode,
                           bool* handled) {
    *handled = true;

    switch (uMsg) {
        case WM_NCPAINT:
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME:
            return 0;

        case WM_NCACTIVATE:
            // TRUE means "activation accepted"; skipping DefWindowProc is what
            // keeps it from repainting the caption.
            return TRUE;

        case WM_SETTEXT:
        case WM_SETICON: {
            // DefWindowProc repaints the caption for these itself, without
            // going through WM_NCPAINT. Clearing WS_VISIBLE around the call is
            // the textbook cure and it must NOT be used: taskbar replacements
            // watch for that transient state and drop the window from their
            // list for good. Update the title normally, then repaint the
            // client over whatever caption pixels it emitted.
            LRESULT result =
                unicode ? DefWindowProcW_Orig(hWnd, uMsg, wParam, lParam)
                        : DefWindowProcA_Orig(hWnd, uMsg, wParam, lParam);
            RedrawWindow(hWnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ALLCHILDREN);
            return result;
        }
    }

    *handled = false;
    return 0;
}

// DefWindowProc sees a great many messages, so the cheap message test comes
// first and only the handful below ever look the window up.
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    switch (uMsg) {
        case WM_NCPAINT:
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME:
        case WM_NCACTIVATE:
        case WM_SETTEXT:
        case WM_SETICON:
            if (IsTargetWindow(hWnd)) {
                bool handled = false;
                LRESULT result = HandleFrameMessage(hWnd, uMsg, wParam, lParam,
                                                    true, &handled);
                if (handled) {
                    return result;
                }
            }
            break;
    }

    return DefWindowProcW_Orig(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    switch (uMsg) {
        case WM_NCPAINT:
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME:
        case WM_NCACTIVATE:
        case WM_SETTEXT:
        case WM_SETICON:
            if (IsTargetWindow(hWnd)) {
                bool handled = false;
                LRESULT result = HandleFrameMessage(hWnd, uMsg, wParam, lParam,
                                                    false, &handled);
                if (handled) {
                    return result;
                }
            }
            break;
    }

    return DefWindowProcA_Orig(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Catching windows

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName,
                                     dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                     hMenu, hInstance, lpParam);
    if (hWnd) {
        AttachWindow(hWnd);
    }
    return hWnd;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (lParam) {
        AttachWindow(hWnd);
    } else {
        DetachWindow(hWnd);
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

// Returns false if this program is not in the list, in which case the mod has
// nothing to do here and unloads.
bool LoadSettings() {
    WCHAR programPath[MAX_PATH];
    DWORD length =
        GetModuleFileNameW(nullptr, programPath, ARRAYSIZE(programPath));
    if (!length || length == ARRAYSIZE(programPath)) {
        return false;
    }

    PCWSTR programFileName = wcsrchr(programPath, L'\\');
    programFileName = programFileName ? programFileName + 1 : programPath;

    for (int i = 0;; i++) {
        PCWSTR name = Wh_GetStringSetting(L"programs[%d].name", i);
        bool hasName = *name;
        bool matches = hasName && (PathMatchSpecW(programFileName, name) ||
                                   PathMatchSpecW(programPath, name));
        Wh_FreeStringSetting(name);

        if (!hasName) {
            return false;
        }

        if (!matches) {
            continue;
        }

        PCWSTR mode = Wh_GetStringSetting(L"programs[%d].mode", i);
        if (wcscmp(mode, L"strip") == 0) {
            g_mode = Mode::Strip;
        } else if (wcscmp(mode, L"darkframe") == 0) {
            g_mode = Mode::DarkFrame;
        } else {
            g_mode = Mode::NoPaint;
        }
        Wh_FreeStringSetting(mode);

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    if (!LoadSettings()) {
        return FALSE;
    }

    PCWSTR modeName = g_mode == Mode::Strip       ? L"strip"
                      : g_mode == Mode::DarkFrame ? L"darkframe"
                                                  : L"nopaint";
    Wh_Log(L"Init, mode=%s", modeName);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Orig);

    if (g_mode == Mode::NoPaint) {
        Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook,
                           (void**)&DefWindowProcW_Orig);
        Wh_SetFunctionHook((void*)DefWindowProcA, (void*)DefWindowProcA_Hook,
                           (void**)&DefWindowProcA_Orig);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 1);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    EnumWindows(EnumWindowsProc, 0);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // The program list and the mode decide which hooks are installed, so a
    // full reload is the only sane way to apply a change.
    *bReload = TRUE;
    return TRUE;
}
