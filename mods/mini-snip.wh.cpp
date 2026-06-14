// ==WindhawkMod==
// @id              mini-snip
// @name            MiniSnip
// @description     Lightweight screenshot capture overlay launched by Windhawk.
// @version         1.0.0
// @author          Mirochill
// @github          https://github.com/Mirochill
// @homepage        https://github.com/Mirochill/MiniSnip
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lmsimg32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MiniSnip

MiniSnip is a compact screenshot overlay. Windhawk starts it automatically in a
dedicated helper process, so there is no separate startup entry to maintain.

Press the configured hotkey, drag a rectangle to copy that region to the
clipboard, or click a highlighted window to copy that window. Hold Shift while
dragging to constrain the selection to a square.

The implementation is native Win32/GDI. It captures the desktop only when the
overlay opens, repaints only changed regions while the pointer moves, and keeps
no background capture loop running while idle.

## Notes

- Default hotkey: Alt+W.
- The copied bitmap stays local and is placed directly on the Windows
  clipboard.
- If another application already owns the hotkey, enable Windhawk logging and
  change the hotkey values in the settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyVirtualKey: 87
  $name: Hotkey virtual key
  $description: Decimal virtual-key code. 87 is W. Use 83 for S, 44 for Print Screen, etc.

- hotkeyModifiers: 1
  $name: Hotkey modifiers
  $description: "Bit mask: Alt=1, Control=2, Shift=4, Win=8. The default 1 means Alt."

- enableWindowClick: true
  $name: Click window to capture it
  $description: Highlight visible windows and copy the clicked window when no drag selection is made.

- constrainSquareWithShift: true
  $name: Shift constrains to square
  $description: Hold Shift while dragging to make a 1:1 selection.

- overlayShadeOpacity: 84
  $name: Overlay shade opacity
  $description: Opacity of the dark shade over the frozen desktop, from 0 to 220.

- selectionColor: "#5CC4FF"
  $name: Selection border color
  $description: Hex RGB color used for the dragged region.

- windowHighlightColor: "#FFBC52"
  $name: Window highlight color
  $description: Hex RGB color used for hover-to-capture windows.

- showHintBar: true
  $name: Show hint bar
  $description: Show the compact MiniSnip hint bar at the top of the overlay.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

namespace {

constexpr UINT kHotkeyId = 0x4D53;
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr int kDragThreshold = 4;
constexpr PCWSTR kToolWindowClass = L"MiniSnipWindhawkToolWindow";
constexpr PCWSTR kOverlayWindowClass = L"MiniSnipWindhawkOverlay";

struct Settings {
    UINT hotkeyVirtualKey = 'W';
    UINT hotkeyModifiers = MOD_ALT;
    bool enableWindowClick = true;
    bool constrainSquareWithShift = true;
    BYTE overlayShadeOpacity = 84;
    COLORREF selectionColor = RGB(92, 196, 255);
    COLORREF windowHighlightColor = RGB(255, 188, 82);
    bool showHintBar = true;
};

struct WindowInfo {
    HWND hwnd = nullptr;
    RECT rect{};
};

struct OverlayState {
    HWND hwnd = nullptr;
    RECT screenRect{};
    HBITMAP screenshot = nullptr;
    HDC screenshotDc = nullptr;
    std::vector<WindowInfo> windows;
    POINT dragStart{};
    POINT dragCurrent{};
    bool dragging = false;
    bool hasDragged = false;
    HWND hoverWindow = nullptr;
    HWND mouseDownWindow = nullptr;
};

Settings g_settings;
HWND g_toolWindow = nullptr;
HWND g_overlayWindow = nullptr;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
std::atomic<bool> g_stopWorker = false;
bool g_hotkeyRegistered = false;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(maxValue, value));
}

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

COLORREF ParseColorSetting(PCWSTR name, COLORREF fallback) {
    std::wstring value = GetStringSetting(name);
    if (value.size() == 7 && value[0] == L'#') {
        wchar_t* end = nullptr;
        unsigned long rgb = wcstoul(value.c_str() + 1, &end, 16);
        if (end && *end == L'\0' && rgb <= 0xFFFFFF) {
            return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
        }
    }

    return fallback;
}

void LoadSettings() {
    Settings next;
    int key = Wh_GetIntSetting(L"hotkeyVirtualKey");
    next.hotkeyVirtualKey = key > 0 ? static_cast<UINT>(key) : 'W';

    int modifiers = Wh_GetIntSetting(L"hotkeyModifiers");
    if (modifiers < 0 || modifiers > 15) {
        modifiers = MOD_ALT;
    }

    next.hotkeyModifiers = static_cast<UINT>(modifiers);
    next.enableWindowClick = Wh_GetIntSetting(L"enableWindowClick") != 0;
    next.constrainSquareWithShift =
        Wh_GetIntSetting(L"constrainSquareWithShift") != 0;
    next.overlayShadeOpacity = static_cast<BYTE>(
        ClampInt(Wh_GetIntSetting(L"overlayShadeOpacity"), 0, 220));
    next.selectionColor =
        ParseColorSetting(L"selectionColor", RGB(92, 196, 255));
    next.windowHighlightColor =
        ParseColorSetting(L"windowHighlightColor", RGB(255, 188, 82));
    next.showHintBar = Wh_GetIntSetting(L"showHintBar") != 0;

    g_settings = next;
}

RECT GetVirtualScreenRect() {
    RECT rect{};
    rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rect.right = rect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rect.bottom = rect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return rect;
}

int RectWidth(const RECT& rect) {
    return rect.right - rect.left;
}

int RectHeight(const RECT& rect) {
    return rect.bottom - rect.top;
}

bool IsRectEmptyOrTiny(const RECT& rect) {
    return RectWidth(rect) <= 1 || RectHeight(rect) <= 1;
}

RECT OffsetRectCopy(RECT rect, int dx, int dy) {
    OffsetRect(&rect, dx, dy);
    return rect;
}

RECT InflateRectCopy(RECT rect, int dx, int dy) {
    InflateRect(&rect, dx, dy);
    return rect;
}

POINT ClampPointToClient(HWND hwnd, POINT point) {
    RECT client{};
    GetClientRect(hwnd, &client);
    point.x = ClampInt(point.x, client.left, client.right);
    point.y = ClampInt(point.y, client.top, client.bottom);
    return point;
}

bool IsSquareSelectionActive() {
    return g_settings.constrainSquareWithShift &&
           (GetKeyState(VK_SHIFT) & 0x8000) != 0;
}

POINT ConstrainToSquare(POINT start, POINT current, RECT bounds) {
    int dx = current.x - start.x;
    int dy = current.y - start.y;
    int side = std::min(std::abs(dx), std::abs(dy));
    POINT constrained{
        start.x + (dx < 0 ? -side : side),
        start.y + (dy < 0 ? -side : side),
    };

    constrained.x = ClampInt(constrained.x, bounds.left, bounds.right);
    constrained.y = ClampInt(constrained.y, bounds.top, bounds.bottom);
    return constrained;
}

RECT NormalizeSelection(POINT start, POINT current) {
    RECT rect{
        std::min(start.x, current.x),
        std::min(start.y, current.y),
        std::max(start.x, current.x),
        std::max(start.y, current.y),
    };
    return rect;
}

RECT GetSelectionRect(const OverlayState* state) {
    POINT current = state->dragCurrent;
    if (IsSquareSelectionActive()) {
        RECT client{};
        GetClientRect(state->hwnd, &client);
        current = ConstrainToSquare(state->dragStart, current, client);
    }

    return NormalizeSelection(state->dragStart, current);
}

bool IsWindowCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(
               hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

bool ShouldCaptureWindow(HWND hwnd, HWND overlayWindow) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || hwnd == overlayWindow ||
        IsWindowCloaked(hwnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) != 0) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect) || IsRectEmptyOrTiny(rect)) {
        return false;
    }

    wchar_t className[64]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }

    return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* state = reinterpret_cast<OverlayState*>(lParam);
    if (ShouldCaptureWindow(hwnd, state->hwnd)) {
        RECT rect{};
        GetWindowRect(hwnd, &rect);
        RECT clipped{};
        if (IntersectRect(&clipped, &rect, &state->screenRect)) {
            state->windows.push_back(WindowInfo{hwnd, clipped});
        }
    }

    return TRUE;
}

HWND FindWindowAtPoint(const OverlayState* state, POINT screenPoint) {
    if (!g_settings.enableWindowClick) {
        return nullptr;
    }

    for (const WindowInfo& window : state->windows) {
        if (PtInRect(&window.rect, screenPoint)) {
            return window.hwnd;
        }
    }

    return nullptr;
}

bool GetWindowInfoByHandle(const OverlayState* state,
                           HWND hwnd,
                           WindowInfo* info) {
    if (!hwnd) {
        return false;
    }

    for (const WindowInfo& window : state->windows) {
        if (window.hwnd == hwnd) {
            *info = window;
            return true;
        }
    }

    return false;
}

void InvalidateWindowHighlight(OverlayState* state, HWND hwnd) {
    WindowInfo info;
    if (!GetWindowInfoByHandle(state, hwnd, &info)) {
        return;
    }

    RECT client = OffsetRectCopy(info.rect, -state->screenRect.left,
                                 -state->screenRect.top);
    client = InflateRectCopy(client, 8, 8);
    InvalidateRect(state->hwnd, &client, FALSE);
}

void InvalidateSelectionTransition(HWND hwnd, RECT previous, RECT current) {
    RECT dirty{};
    UnionRect(&dirty, &previous, &current);
    dirty = InflateRectCopy(dirty, 48, 36);
    InvalidateRect(hwnd, &dirty, FALSE);
}

HBITMAP CaptureScreenBitmap(const RECT& screenRect, HDC* outDc) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }

    HDC memoryDc = CreateCompatibleDC(screenDc);
    HBITMAP bitmap = CreateCompatibleBitmap(
        screenDc, RectWidth(screenRect), RectHeight(screenRect));
    if (memoryDc && bitmap) {
        HGDIOBJ old = SelectObject(memoryDc, bitmap);
        BitBlt(memoryDc, 0, 0, RectWidth(screenRect), RectHeight(screenRect),
               screenDc, screenRect.left, screenRect.top, SRCCOPY);
        SelectObject(memoryDc, old);
        SelectObject(memoryDc, bitmap);
        *outDc = memoryDc;
    } else {
        if (bitmap) {
            DeleteObject(bitmap);
            bitmap = nullptr;
        }
        if (memoryDc) {
            DeleteDC(memoryDc);
        }
    }

    ReleaseDC(nullptr, screenDc);
    return bitmap;
}

void AlphaFillRect(HDC dc, const RECT& rect, COLORREF color, BYTE opacity) {
    int width = RectWidth(rect);
    int height = RectHeight(rect);
    if (width <= 0 || height <= 0 || opacity == 0) {
        return;
    }

    HDC sourceDc = CreateCompatibleDC(dc);
    HBITMAP sourceBitmap = CreateCompatibleBitmap(dc, width, height);
    if (!sourceDc || !sourceBitmap) {
        if (sourceBitmap) {
            DeleteObject(sourceBitmap);
        }
        if (sourceDc) {
            DeleteDC(sourceDc);
        }
        return;
    }

    HGDIOBJ old = SelectObject(sourceDc, sourceBitmap);
    HBRUSH brush = CreateSolidBrush(color);
    RECT local{0, 0, width, height};
    FillRect(sourceDc, &local, brush);
    DeleteObject(brush);

    BLENDFUNCTION blend{AC_SRC_OVER, 0, opacity, 0};
    AlphaBlend(dc, rect.left, rect.top, width, height, sourceDc, 0, 0, width,
               height, blend);

    SelectObject(sourceDc, old);
    DeleteObject(sourceBitmap);
    DeleteDC(sourceDc);
}

void DrawBorder(HDC dc, const RECT& rect, COLORREF color, int width) {
    HPEN pen = CreatePen(PS_SOLID, width, color);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
    Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

void DrawTextPill(HDC dc,
                  const RECT& rect,
                  COLORREF background,
                  COLORREF foreground,
                  PCWSTR text) {
    HBRUSH brush = CreateSolidBrush(background);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, foreground);
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, font);
    RECT textRect = rect;
    DrawTextW(dc, text, -1, &textRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void DrawHintBar(HDC dc, const RECT& client) {
    if (!g_settings.showHintBar) {
        return;
    }

    RECT bar{};
    int width = 420;
    bar.left = client.left + (RectWidth(client) - width) / 2;
    bar.right = bar.left + width;
    bar.top = client.top + 18;
    bar.bottom = bar.top + 48;
    DrawTextPill(dc, bar, RGB(18, 24, 34), RGB(245, 248, 252),
                 L"MiniSnip    Drag region    Click window    Esc");
}

void DrawSelectionLabel(HDC dc, const RECT& selection) {
    wchar_t label[64]{};
    swprintf_s(label, L"%d x %d", RectWidth(selection), RectHeight(selection));

    RECT labelRect{};
    labelRect.left = selection.left;
    labelRect.top = std::max<LONG>(0, selection.top - 28);
    labelRect.right = labelRect.left + 104;
    labelRect.bottom = labelRect.top + 24;
    DrawTextPill(dc, labelRect, RGB(20, 27, 38), RGB(245, 248, 252), label);
}

void PaintOverlay(OverlayState* state, HDC dc, const PAINTSTRUCT& ps) {
    RECT clip = ps.rcPaint;
    if (IsRectEmptyOrTiny(clip)) {
        return;
    }

    BitBlt(dc, clip.left, clip.top, RectWidth(clip), RectHeight(clip),
           state->screenshotDc, clip.left, clip.top, SRCCOPY);
    AlphaFillRect(dc, clip, RGB(0, 0, 0), g_settings.overlayShadeOpacity);

    if (state->dragging && state->hasDragged) {
        RECT selection = GetSelectionRect(state);
        RECT visibleSelection{};
        if (IntersectRect(&visibleSelection, &selection, &clip)) {
            BitBlt(dc, visibleSelection.left, visibleSelection.top,
                   RectWidth(visibleSelection), RectHeight(visibleSelection),
                   state->screenshotDc, visibleSelection.left,
                   visibleSelection.top, SRCCOPY);
        }

        DrawBorder(dc, selection, g_settings.selectionColor, 2);
        DrawSelectionLabel(dc, selection);
    } else if (state->hoverWindow) {
        WindowInfo info;
        if (GetWindowInfoByHandle(state, state->hoverWindow, &info)) {
            RECT client = OffsetRectCopy(info.rect, -state->screenRect.left,
                                         -state->screenRect.top);
            DrawBorder(dc, client, g_settings.windowHighlightColor, 2);

            RECT label{};
            label.left = client.left;
            label.top = std::max<LONG>(0, client.top - 28);
            label.right = label.left + 118;
            label.bottom = label.top + 24;
            DrawTextPill(dc, label, RGB(38, 29, 14), RGB(255, 245, 225),
                         L"Click to copy");
        }
    }

    RECT client{};
    GetClientRect(state->hwnd, &client);
    DrawHintBar(dc, client);
}

void CopyScreenRectToClipboard(OverlayState* state, RECT screenRect) {
    RECT clipped{};
    if (!IntersectRect(&clipped, &screenRect, &state->screenRect) ||
        IsRectEmptyOrTiny(clipped)) {
        return;
    }

    int width = RectWidth(clipped);
    int height = RectHeight(clipped);
    HDC screenDc = GetDC(nullptr);
    HDC destDc = CreateCompatibleDC(screenDc);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDc, width, height);
    if (!destDc || !bitmap) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (destDc) {
            DeleteDC(destDc);
        }
        if (screenDc) {
            ReleaseDC(nullptr, screenDc);
        }
        return;
    }

    HGDIOBJ old = SelectObject(destDc, bitmap);
    BitBlt(destDc, 0, 0, width, height, state->screenshotDc,
           clipped.left - state->screenRect.left,
           clipped.top - state->screenRect.top, SRCCOPY);
    SelectObject(destDc, old);
    DeleteDC(destDc);
    ReleaseDC(nullptr, screenDc);

    for (int attempt = 0; attempt < 4; attempt++) {
        if (OpenClipboard(state->hwnd)) {
            EmptyClipboard();
            if (SetClipboardData(CF_BITMAP, bitmap)) {
                CloseClipboard();
                return;
            }
            CloseClipboard();
        }

        Sleep(35);
    }

    DeleteObject(bitmap);
}

void CloseOverlay(HWND hwnd) {
    DestroyWindow(hwnd);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<OverlayState*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CREATE: {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            state = reinterpret_cast<OverlayState*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(state));
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);
            if (state) {
                PaintOverlay(state, dc, ps);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (!state) {
                break;
            }

            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            point = ClampPointToClient(hwnd, point);
            state->dragStart = point;
            state->dragCurrent = point;
            state->dragging = true;
            state->hasDragged = false;
            state->mouseDownWindow = state->hoverWindow;
            SetCapture(hwnd);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!state) {
                break;
            }

            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            point = ClampPointToClient(hwnd, point);
            if (state->dragging) {
                RECT previous = state->hasDragged ? GetSelectionRect(state)
                                                  : RECT{0, 0, 0, 0};
                bool wasDragging = state->hasDragged;
                state->dragCurrent = point;
                state->hasDragged =
                    state->hasDragged ||
                    std::abs(point.x - state->dragStart.x) >= kDragThreshold ||
                    std::abs(point.y - state->dragStart.y) >= kDragThreshold;
                RECT current = state->hasDragged ? GetSelectionRect(state)
                                                 : RECT{0, 0, 0, 0};
                if (state->hasDragged) {
                    InvalidateSelectionTransition(hwnd, previous, current);
                }
                if (!wasDragging && state->hoverWindow) {
                    InvalidateWindowHighlight(state, state->hoverWindow);
                }
                return 0;
            }

            POINT screenPoint{state->screenRect.left + point.x,
                              state->screenRect.top + point.y};
            HWND nextHover = FindWindowAtPoint(state, screenPoint);
            if (nextHover != state->hoverWindow) {
                InvalidateWindowHighlight(state, state->hoverWindow);
                InvalidateWindowHighlight(state, nextHover);
                state->hoverWindow = nextHover;
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (!state || !state->dragging) {
                break;
            }

            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            state->dragCurrent = ClampPointToClient(hwnd, point);
            ReleaseCapture();
            state->dragging = false;

            if (state->hasDragged) {
                RECT selection = GetSelectionRect(state);
                RECT screenSelection = OffsetRectCopy(
                    selection, state->screenRect.left, state->screenRect.top);
                CopyScreenRectToClipboard(state, screenSelection);
                CloseOverlay(hwnd);
                return 0;
            }

            POINT screenPoint{state->screenRect.left + state->dragCurrent.x,
                              state->screenRect.top + state->dragCurrent.y};
            HWND clickedWindow = FindWindowAtPoint(state, screenPoint);
            if (clickedWindow && clickedWindow == state->mouseDownWindow) {
                WindowInfo info;
                if (GetWindowInfoByHandle(state, clickedWindow, &info)) {
                    CopyScreenRectToClipboard(state, info.rect);
                }
            }

            CloseOverlay(hwnd);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                CloseOverlay(hwnd);
                return 0;
            }
            if (wParam == VK_SHIFT && state && state->dragging &&
                state->hasDragged) {
                RECT selection = InflateRectCopy(GetSelectionRect(state), 48, 36);
                InvalidateRect(hwnd, &selection, FALSE);
                return 0;
            }
            break;

        case WM_KEYUP:
            if (wParam == VK_SHIFT && state && state->dragging &&
                state->hasDragged) {
                RECT selection = InflateRectCopy(GetSelectionRect(state), 48, 36);
                InvalidateRect(hwnd, &selection, FALSE);
                return 0;
            }
            break;

        case WM_DESTROY:
            g_overlayWindow = nullptr;
            return 0;

        case WM_NCDESTROY:
            if (state) {
                if (state->screenshotDc) {
                    DeleteDC(state->screenshotDc);
                }
                if (state->screenshot) {
                    DeleteObject(state->screenshot);
                }
                delete state;
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            }
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowOverlay() {
    if (g_overlayWindow && IsWindow(g_overlayWindow)) {
        SetForegroundWindow(g_overlayWindow);
        return;
    }

    auto* state = new OverlayState();
    state->screenRect = GetVirtualScreenRect();
    state->screenshot =
        CaptureScreenBitmap(state->screenRect, &state->screenshotDc);
    if (!state->screenshot || !state->screenshotDc) {
        delete state;
        return;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kOverlayWindowClass, L"MiniSnip",
        WS_POPUP | WS_VISIBLE, state->screenRect.left, state->screenRect.top,
        RectWidth(state->screenRect), RectHeight(state->screenRect), nullptr,
        nullptr, GetModuleHandleW(nullptr), state);
    if (!hwnd) {
        if (state->screenshotDc) {
            DeleteDC(state->screenshotDc);
        }
        if (state->screenshot) {
            DeleteObject(state->screenshot);
        }
        delete state;
        return;
    }

    state->hwnd = hwnd;
    g_overlayWindow = hwnd;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(state));
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

bool RegisterMiniSnipHotkey() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_toolWindow, kHotkeyId);
        g_hotkeyRegistered = false;
    }

    UINT modifiers = g_settings.hotkeyModifiers | MOD_NOREPEAT;
    if (!RegisterHotKey(g_toolWindow, kHotkeyId, modifiers,
                        g_settings.hotkeyVirtualKey)) {
        Wh_Log(L"RegisterHotKey failed for modifiers=%u vk=%u error=%lu",
               modifiers, g_settings.hotkeyVirtualKey, GetLastError());
        return false;
    }

    g_hotkeyRegistered = true;
    return true;
}

LRESULT CALLBACK ToolWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY:
            if (wParam == kHotkeyId) {
                ShowOverlay();
                return 0;
            }
            break;

        case kMsgReloadSettings:
            LoadSettings();
            RegisterMiniSnipHotkey();
            return 0;

        case WM_DESTROY:
            if (g_hotkeyRegistered) {
                UnregisterHotKey(hwnd, kHotkeyId);
                g_hotkeyRegistered = false;
            }
            g_toolWindow = nullptr;
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterWindowClasses() {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW toolClass{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = ToolWndProc,
        .hInstance = instance,
        .lpszClassName = kToolWindowClass,
    };
    if (!RegisterClassExW(&toolClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    WNDCLASSEXW overlayClass{
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = OverlayWndProc,
        .hInstance = instance,
        .hCursor = LoadCursorW(nullptr, IDC_CROSS),
        .lpszClassName = kOverlayWindowClass,
    };
    if (!RegisterClassExW(&overlayClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    return true;
}

DWORD WINAPI WorkerThreadProc(void*) {
    g_workerThreadId = GetCurrentThreadId();
    LoadSettings();
    if (!RegisterWindowClasses()) {
        Wh_Log(L"RegisterWindowClasses failed");
        return 1;
    }

    g_toolWindow = CreateWindowExW(0, kToolWindowClass, L"MiniSnip", 0, 0, 0, 0,
                                   0, HWND_MESSAGE, nullptr,
                                   GetModuleHandleW(nullptr), nullptr);
    if (!g_toolWindow) {
        Wh_Log(L"CreateWindowExW tool window failed: %lu", GetLastError());
        return 1;
    }

    RegisterMiniSnipHotkey();

    MSG msg{};
    while (!g_stopWorker.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_overlayWindow && IsWindow(g_overlayWindow)) {
        DestroyWindow(g_overlayWindow);
    }
    if (g_toolWindow && IsWindow(g_toolWindow)) {
        DestroyWindow(g_toolWindow);
    }

    return 0;
}

BOOL WhTool_ModInit() {
    g_stopWorker = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_toolWindow) {
        PostMessageW(g_toolWindow, kMsgReloadSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_stopWorker = true;
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
}

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL IsExcludedExplorerCommandLine() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        return FALSE;
    }

    BOOL excluded = FALSE;
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            excluded = TRUE;
            break;
        }
    }

    LocalFree(argv);
    return excluded;
}

enum class ToolProcessKind {
    NormalExplorer,
    OtherToolMod,
    CurrentToolMod,
    Excluded,
};

ToolProcessKind GetToolProcessKind() {
    if (IsExcludedExplorerCommandLine()) {
        return ToolProcessKind::Excluded;
    }

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        return ToolProcessKind::NormalExplorer;
    }

    ToolProcessKind kind = ToolProcessKind::NormalExplorer;
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            kind = wcscmp(argv[i + 1], WH_MOD_ID) == 0
                       ? ToolProcessKind::CurrentToolMod
                       : ToolProcessKind::OtherToolMod;
            break;
        }
    }

    LocalFree(argv);
    return kind;
}

bool LaunchToolModProcess() {
    WCHAR currentProcessPath[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, currentProcessPath,
                                      ARRAYSIZE(currentProcessPath));
    if (length == 0 || length == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileNameW failed");
        return false;
    }

    WCHAR commandLine[MAX_PATH + 64]{};
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
    }
    if (!kernelModule) {
        Wh_Log(L"No kernel module");
        return false;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
        LPPROCESS_INFORMATION, PHANDLE);

    auto createProcessInternal =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal) {
        Wh_Log(L"No CreateProcessInternalW");
        return false;
    }

    STARTUPINFOW startupInfo{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr,
                               nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr,
                               nullptr, &startupInfo, &processInfo, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed: %lu", GetLastError());
        return false;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    ToolProcessKind kind = GetToolProcessKind();
    if (kind == ToolProcessKind::Excluded ||
        kind == ToolProcessKind::OtherToolMod) {
        return FALSE;
    }

    if (kind == ToolProcessKind::CurrentToolMod) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex ||
            GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        auto* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) +
            ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
        return TRUE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isToolModProcessLauncher) {
        LaunchToolModProcess();
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}
