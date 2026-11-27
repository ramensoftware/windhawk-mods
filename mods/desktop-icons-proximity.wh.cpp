// ==WindhawkMod==
// @id              desktop-icons-proximity
// @name            Desktop Icons Proximity Reveal
// @description     Hide desktop icons until you hover over them or a custom area, with smooth fades and separate icon groups.
// @version         1.0.3
// @author          CTabdul15
// @github          https://github.com/CTabdul15
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Proximity Reveal

![Desktop Icons Proximity Reveal demo](https://raw.githubusercontent.com/CTabdul15/windhawk-mods/assets/desktop-icons-proximity-preview.gif)

Hide desktop icons when you are not using them. Hover near an icon or over a
custom area to fade them back in.

Features:

* Draw up to 32 reveal areas around your icons.
* Reveal each group separately, or show all icons together.
* Adjust fade speed, hide delay, and hover padding.
* Preview your areas and save them between sessions.
* Keep new folders and shortcuts visible while naming them, then hide them after
  an outside click or a configurable timeout (15 seconds by default).
* Selected icons stay visible until deselected; press **Ctrl+A** to reveal all.
  Disable **Keep selected icons visible** to turn this off.
* Keep icons permanently visible using **Always-visible icons (whitelist)**.
  Enter desktop names separated by semicolons, such as `Recycle Bin; Steam; Discord`.
  Names ignore case; `*` and `?` wildcards are supported.

## Setup

1. Enable **Show box editor panel** in Settings and apply.
2. Click **Edit boxes**. All icons appear, including those outside saved areas.
   Drag around your icons to add areas.
3. Click **Save & apply** or **Cancel** to restore normal hiding.
   Switch the panel setting off to close it.

The panel stays closed at startup. If its setting is already on, switch it off
and on again to reopen it.

Separate groups are enabled by default. Without active areas, hovering near any icon
shows all icons. Recycle Bin and folders work too.

## How this differs from other mods

[Desktop Icon Section Auto-Hide](https://windhawk.net/mods/desktop-icon-section-autohide)
uses a single section or box. This mod adds up to 32 separately animated areas,
a drag-to-draw editor, and temporary visibility for newly created items.
[Flashlight Desktop Icons](https://windhawk.net/mods/flashlight-desktop-icons) and
[Spotlight](https://windhawk.net/mods/transparent-desktop-icons-spotlight) reveal
icons near the cursor rather than inside saved groups.
[Smooth Desktop Icons Auto-Hide](https://windhawk.net/mods/smooth-desktop-icons-auto-hide)
and [ZenDesktop](https://windhawk.net/mods/zen-desktop-toggle-icons) focus on hiding
and restoring the desktop as a whole.

**Note:** Requires Windows 10/11 x64. Keep Windows' **Show desktop icons** enabled.
Hidden icons retain their Windows click targets; icons outside all boxes stay
hidden unless selected, whitelisted, temporarily revealed, or accessed by keyboard.
Redraw areas after changing monitors; avoid running other icon hiding mods.

Rendering hooks adapted from [Desktop Icons Transparency by zed712969-crypto](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-icons-transparency.wh.cpp).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepSelectedVisible: true
  $name: Keep selected icons visible
  $description: Selected icons stay visible until you deselect them. Press Ctrl+A on the desktop to select and reveal all icons. Applies even outside reveal areas.
- alwaysVisible: ""
  $name: Always-visible icons (whitelist)
  $description: Desktop icon names separated by semicolons, for example Recycle Bin; Steam; Discord. Use the names shown on your desktop. Case-insensitive; .lnk/.url suffixes are optional. Wildcards * and ? are supported.
- showNewItems: true
  $name: Keep new items visible temporarily
  $description: Newly added files, folders and shortcuts stay visible until the timeout or an outside click. An item being renamed always stays visible while you type.
- newItemSeconds: 15
  $name: New item visibility (seconds)
  $description: How long a new item stays visible. The timer restarts while naming it. Range 0-300; 0 means wait for an outside click with no timeout.
- dismissNewOnClick: true
  $name: Hide new items after clicking elsewhere
  $description: Dismiss temporary visibility when you click outside the item, including in another app. Whitelisted icons stay visible. Turn off to use only the timeout.
- boxHoverPadding: 10
  $name: Box hover padding (screen pixels)
  $description: Extra hover area outside every box so icons stay visible near screen edges. Does not change icon membership. Green outline is the icon box; dotted blue outline is the extended hover area. Range 0-1000.
- independentBoxes: true
  $name: Reveal each box independently
  $description: When enabled, hovering over a box reveals only the icons whose image centers are inside it. Other boxes stay hidden. Overlaps use the highest visibility. Requires active reveal boxes.
- showEditorPanel: false
  $name: Show box editor panel
  $description: Enable and apply to open the editor control panel. Disable and apply to close it and cancel any unsaved drawing. The desktop is dimmed only after clicking Edit boxes. No keyboard shortcut is registered.
- showPreview: false
  $name: Show box outlines
  $description: Click-through preview of all active boxes. Updates when settings are applied. The panel checkbox saves its preview preference. Changing this setting overrides it.
- manualBox: false
  $name: Use numeric box when saved boxes are inactive
  $description: Optional single numeric rectangle. Normally leave off and draw multiple boxes using the control panel.
- boxX: 0
  $name: Numeric box X (screen pixels)
  $description: Left coordinate relative to the primary screen. Negative values support monitors on the left.
- boxY: 0
  $name: Numeric box Y (screen pixels)
  $description: Top coordinate relative to the primary screen. Negative values support monitors above.
- boxWidth: 350
  $name: Numeric box width (screen pixels)
  $description: Range 5-65536. Only used when the numeric box option is enabled.
- boxHeight: 1000
  $name: Numeric box height (screen pixels)
  $description: Range 5-65536. Only used when the numeric box option is enabled.
- proximity: 20
  $name: Reveal distance (logical pixels)
  $description: Distance from ANY icon that reveals ALL icons when no reveal boxes are active. Range 0-400.
- hideDelay: 0
  $name: Hide delay (milliseconds)
  $description: Wait this long after leaving all reveal areas before fading out. Range 0-5000.
- fadeDuration: 220
  $name: Fade duration (milliseconds)
  $description: Time for a full fade. Set to 0 for instant transitions. Range 0-2000.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <vector>
#include <cwchar>
#include <cwctype>
#include <string>
#include <windhawk_utils.h>

static std::atomic<int> g_opacity{100}, g_distance{20}, g_delay{0}, g_fade{220};
static std::atomic<bool> g_stop{false};
static std::atomic<HWND> g_list{nullptr}, g_parent{nullptr};
static std::atomic<bool> g_independent{true};
static std::atomic<int> g_hoverPadding{10};
static HANDLE g_thread = nullptr, g_wake = nullptr;
static UINT g_tick = 0;
static thread_local bool g_inDesktopPaint = false;
static thread_local bool g_inDirectHook = false;
static thread_local bool g_inTextHook = false;

static void WakeWorker() {
    if (g_wake)
        SetEvent(g_wake);
}
static void RequestDesktopUpdate() {
    HWND list = g_list.load();
    if (list && !g_stop.load())
        PostMessageW(list, g_tick, 0, 0);
    WakeWorker();
}
// Editor transitions are infrequent. Complete this UI-thread refresh before
// showing the overlay so hidden icons are painted underneath it immediately.
static void RefreshEditorDesktop() {
    HWND list = g_list.load();
    if (list && !g_stop.load())
        SendMessageW(list, g_tick, 3, 0);
    WakeWorker();
}
// Configuration is copied under a lock; all native UI state belongs to the
// worker.
static constexpr size_t kMaxBoxes = 32;
static constexpr DWORD kBoxesMagic = 0x33585044;
struct BoxState {
    DWORD magic;
    DWORD count;
    DWORD enabled;
    RECT boxes[kMaxBoxes];
};
static SRWLOCK g_boxLock = SRWLOCK_INIT;
static BoxState g_boxState{};
static RECT g_numericBox{0, 0, 350, 1000};
static std::atomic<bool> g_manualBox{false}, g_preview{false},
    g_showControls{false};
static std::atomic<bool> g_editorActive{false}, g_panelRequest{false};
static std::atomic<unsigned> g_previewRevision{1};
static HWND g_outlineWindow = nullptr, g_editorWindow = nullptr,
            g_panel = nullptr;
static HWND g_status = nullptr, g_editButton = nullptr, g_saveButton = nullptr;
static HWND g_cancelButton = nullptr, g_undoButton = nullptr,
            g_clearButton = nullptr;
static HWND g_modeButton = nullptr, g_previewButton = nullptr;
static HINSTANCE g_overlayModule = nullptr;
static const WCHAR kOverlayClass[] = L"Windhawk.Proximity.BoxesOverlay.0.3";
static const WCHAR kPanelClass[] = L"Windhawk.Proximity.BoxesPanel.0.3";
static std::vector<RECT> g_editBoxes;
static RECT g_pendingBox{};
static POINT g_dragStart{};
static bool g_selecting = false;
static bool InsideBox(POINT p, RECT r) {
    return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}
static RECT HoverBox(RECT box) {
    int padding = g_hoverPadding.load();
    box.left -= padding;
    box.top -= padding;
    box.right += padding;
    box.bottom += padding;
    return box;
}
static bool InHoverBox(POINT p, RECT box) {
    return InsideBox(p, HoverBox(box));
}
static RECT NormalizeBox(POINT a, POINT b) {
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x),
            std::max(a.y, b.y)};
}
static bool ValidBox(RECT r) {
    return r.left >= -131072 && r.left <= 131072 && r.top >= -131072 &&
           r.top <= 131072 && r.right >= -131072 && r.bottom >= -131072 &&
           r.right <= 196608 && r.bottom <= 196608 && r.right - r.left >= 5 &&
           r.bottom - r.top >= 5 && r.right - r.left <= 65536 &&
           r.bottom - r.top <= 65536;
}
static BoxState Snapshot() {
    AcquireSRWLockShared(&g_boxLock);
    auto result = g_boxState;
    ReleaseSRWLockShared(&g_boxLock);
    return result;
}
static void CommitState(const BoxState& state) {
    AcquireSRWLockExclusive(&g_boxLock);
    g_boxState = state;
    ReleaseSRWLockExclusive(&g_boxLock);
    ++g_previewRevision;
    RequestDesktopUpdate();
}
static std::vector<RECT> ActiveBoxes() {
    std::vector<RECT> boxes;
    AcquireSRWLockShared(&g_boxLock);
    if (g_boxState.enabled) {
        for (DWORD i = 0; i < g_boxState.count; ++i)
            boxes.push_back(g_boxState.boxes[i]);
    } else if (g_manualBox.load())
        boxes.push_back(g_numericBox);
    ReleaseSRWLockShared(&g_boxLock);
    return boxes;
}
static bool StoreState(const BoxState& state) {
    // One complete snapshot, then a readback. A failed save never silently
    // applies.
    if (!Wh_SetBinaryValue(L"revealBoxes.v3", &state, sizeof(state)))
        return false;
    BoxState check{};
    return Wh_GetBinaryValue(L"revealBoxes.v3", &check, sizeof(check)) ==
               sizeof(check) &&
           std::memcmp(&state, &check, sizeof(check)) == 0;
}
static void UpdatePanel(const WCHAR* message = nullptr) {
    if (!g_panel)
        return;
    auto state = Snapshot();
    WCHAR text[512]{};
    if (message)
        swprintf(text, ARRAYSIZE(text), L"%ls", message);
    else if (g_editorActive.load())
        swprintf(
            text, ARRAYSIZE(text),
            L"Editing: %u boxes. Drag to ADD a box, then click Save & apply.\r\nUndo removes the last box. Esc cancels; Enter saves.",
            (unsigned)g_editBoxes.size());
    else
        swprintf(
            text, ARRAYSIZE(text),
            L"%u saved box(es). Active mode: %ls.\r\nClick Edit boxes to add areas. Show box editor panel in Settings opens/closes this panel.",
            (unsigned)state.count,
            state.enabled && state.count ? L"saved boxes"
            : g_manualBox.load()         ? L"numeric box"
                                         : L"icon proximity");
    SetWindowTextW(g_status, text);
    bool editing = g_editorActive.load();
    EnableWindow(g_editButton, !editing);
    EnableWindow(g_saveButton, editing);
    EnableWindow(g_cancelButton, editing);
    EnableWindow(g_undoButton, editing);
    EnableWindow(g_clearButton, editing);
    EnableWindow(g_modeButton, !editing && state.count > 0);
    SetWindowTextW(g_modeButton,
                   state.enabled ? L"Use icon proximity" : L"Use saved boxes");
    SendMessageW(g_previewButton, BM_SETCHECK,
                 g_preview.load() ? BST_CHECKED : BST_UNCHECKED, 0);
}
static void PaintBox(HWND w,
                     HDC dc,
                     RECT r,
                     COLORREF color,
                     int style = PS_SOLID,
                     int width = 3) {
    POINT origin{};
    ClientToScreen(w, &origin);
    OffsetRect(&r, -origin.x, -origin.y);
    HPEN pen = CreatePen(style, width, color);
    auto oldPen = SelectObject(dc, pen),
         oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, r.left, r.top, r.right, r.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}
static void CloseEditor() {
    if (g_editorWindow && GetCapture() == g_editorWindow)
        ReleaseCapture();
    g_selecting = false;
    g_editorActive = false;
    if (g_panel)
        SetWindowLongPtrW(g_panel, GWLP_HWNDPARENT, 0);
    if (g_editorWindow) {
        DestroyWindow(g_editorWindow);
        g_editorWindow = nullptr;
    }
    ++g_previewRevision;
    RefreshEditorDesktop();
}
static void ShowPanel(bool activate = true) {
    if (g_panel) {
        ShowWindow(g_panel, activate ? SW_SHOW : SW_SHOWNOACTIVATE);
        if (activate)
            SetForegroundWindow(g_panel);
    }
}
static void SaveEditor() {
    if (!g_editorActive.load() || g_selecting)
        return;
    BoxState next{};
    next.magic = kBoxesMagic;
    next.count = (DWORD)g_editBoxes.size();
    next.enabled = next.count ? 1 : 0;
    for (size_t i = 0; i < g_editBoxes.size(); ++i)
        next.boxes[i] = g_editBoxes[i];
    if (!StoreState(next)) {
        UpdatePanel(
            L"SAVE FAILED: Windhawk could not store/verify the boxes.\r\nYour edits remain open. Try again; Cancel keeps the previous active boxes.");
        return;
    }
    CommitState(next);
    CloseEditor();
    ShowPanel();
    WCHAR success[256]{};
    swprintf(
        success, ARRAYSIZE(success), L"Saved and applied %u box(es).\r\n%ls",
        (unsigned)next.count,
        next.count
            ? L"Saved boxes are active. Visibility follows your reveal settings."
            : L"No boxes remain. Icon proximity is active unless the numeric box is enabled.");
    UpdatePanel(success);
}
static LRESULT CALLBACK OverlayProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    bool editor = GetWindowLongPtrW(w, GWLP_USERDATA) == 1;
    if (msg == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(w, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        return TRUE;
    }
    if (msg == WM_ERASEBKGND)
        return 1;
    if (msg == WM_NCHITTEST && !editor)
        return HTTRANSPARENT;
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(w, &ps);
        RECT client{};
        GetClientRect(w, &client);
        FillRect(dc, &client,
                 reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        auto boxes = editor ? g_editBoxes : ActiveBoxes();
        for (const auto& r : boxes) {
            if (g_hoverPadding.load() > 0)
                PaintBox(w, dc, HoverBox(r), RGB(100, 180, 255), PS_DOT, 1);
            PaintBox(w, dc, r, RGB(0, 255, 170));
        }
        if (editor && g_selecting)
            PaintBox(w, dc, g_pendingBox, RGB(255, 210, 0));
        EndPaint(w, &ps);
        return 0;
    }
    if (!editor)
        return DefWindowProcW(w, msg, wp, lp);
    if (msg == WM_LBUTTONDOWN) {
        if (g_editBoxes.size() >= kMaxBoxes) {
            UpdatePanel(
                L"Maximum: 32 boxes. Use Undo or Clear before adding another.");
            return 0;
        }
        SetForegroundWindow(w);
        SetFocus(w);
        GetCursorPos(&g_dragStart);
        g_pendingBox = {g_dragStart.x, g_dragStart.y, g_dragStart.x,
                        g_dragStart.y};
        g_selecting = true;
        SetCapture(w);
        return 0;
    }
    if (msg == WM_MOUSEMOVE && g_selecting) {
        POINT p{};
        GetCursorPos(&p);
        g_pendingBox = NormalizeBox(g_dragStart, p);
        InvalidateRect(w, nullptr, FALSE);
        return 0;
    }
    if (msg == WM_LBUTTONUP && g_selecting) {
        POINT p{};
        GetCursorPos(&p);
        g_pendingBox = NormalizeBox(g_dragStart, p);
        g_selecting = false;
        ReleaseCapture();
        if (ValidBox(g_pendingBox) && g_editBoxes.size() < kMaxBoxes)
            g_editBoxes.push_back(g_pendingBox);
        InvalidateRect(w, nullptr, FALSE);
        UpdatePanel();
        return 0;
    }
    if (msg == WM_CAPTURECHANGED && g_selecting) {
        g_selecting = false;
        InvalidateRect(w, nullptr, FALSE);
        return 0;
    }
    if (msg == WM_KEYDOWN && wp == VK_RETURN) {
        SaveEditor();
        return 0;
    }
    if (msg == WM_KEYDOWN && wp == VK_ESCAPE) {
        CloseEditor();
        UpdatePanel(L"Canceled. Previous saved boxes are unchanged.");
        return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}
static HWND CreateOverlay(bool editor) {
    DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    if (!editor)
        ex |= WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
    HWND w = CreateWindowExW(ex, kOverlayClass, L"Reveal Box Editor", WS_POPUP,
                             GetSystemMetrics(SM_XVIRTUALSCREEN),
                             GetSystemMetrics(SM_YVIRTUALSCREEN),
                             GetSystemMetrics(SM_CXVIRTUALSCREEN),
                             GetSystemMetrics(SM_CYVIRTUALSCREEN), nullptr,
                             nullptr, g_overlayModule,
                             reinterpret_cast<void*>(editor ? 1 : 0));
    if (w)
        SetLayeredWindowAttributes(w, 0, editor ? 120 : 255,
                                   editor ? LWA_ALPHA : LWA_COLORKEY);
    return w;
}
static void StartEditor() {
    if (g_editorActive.load()) {
        ShowPanel();
        return;
    }
    auto state = Snapshot();
    g_editBoxes.clear();
    for (DWORD i = 0; i < state.count; ++i)
        g_editBoxes.push_back(state.boxes[i]);
    g_editorWindow = CreateOverlay(true);
    if (!g_editorWindow) {
        UpdatePanel(L"Could not open the editor overlay.");
        return;
    }
    g_editorActive = true;
    ++g_previewRevision;
    RefreshEditorDesktop();
    // An owned toolbar stays above the overlay even after clicking the overlay.
    SetWindowLongPtrW(g_panel, GWLP_HWNDPARENT,
                      reinterpret_cast<LONG_PTR>(g_editorWindow));
    if (g_outlineWindow)
        ShowWindow(g_outlineWindow, SW_HIDE);
    ShowWindow(g_editorWindow, SW_SHOW);
    // Keep a separate, non-layered toolbar ABOVE the full-screen overlay.
    ShowPanel();
    SetWindowPos(g_panel, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    UpdatePanel();
}
static HWND AddControl(HWND parent,
                       const WCHAR* type,
                       const WCHAR* text,
                       DWORD style,
                       int x,
                       int y,
                       int width,
                       int height,
                       int id) {
    HWND control = CreateWindowExW(
        0, type, text, WS_CHILD | WS_VISIBLE | style, x, y, width, height,
        parent, reinterpret_cast<HMENU>((INT_PTR)id), g_overlayModule, nullptr);
    SendMessageW(control, WM_SETFONT,
                 reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),
                 TRUE);
    return control;
}
static HFONT g_panelFont = nullptr;
static HWND g_hideButton = nullptr;
static void LayoutPanel(HWND window) {
    UINT dpi = GetDpiForWindow(window);
    NONCLIENTMETRICSW metrics{};
    metrics.cbSize = sizeof(metrics);
    LOGFONTW font{};
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics),
                                   &metrics, 0, dpi))
        font = metrics.lfMessageFont;
    else {
        font.lfHeight = -MulDiv(9, dpi, 72);
        wcscpy_s(font.lfFaceName, L"Segoe UI");
    }
    HFONT nextFont = CreateFontIndirectW(&font);
    struct ControlPosition {
        HWND handle;
        int x, y, width, height;
    };
    ControlPosition controls[] = {
        {g_status, 12, 12, 536, 52},      {g_editButton, 12, 76, 124, 30},
        {g_saveButton, 144, 76, 124, 30}, {g_cancelButton, 276, 76, 84, 30},
        {g_undoButton, 368, 76, 88, 30},  {g_clearButton, 464, 76, 84, 30},
        {g_modeButton, 12, 118, 190, 30}, {g_previewButton, 220, 118, 160, 30},
        {g_hideButton, 424, 118, 124, 30}};
    for (const auto& item : controls) {
        SetWindowPos(item.handle, nullptr, MulDiv(item.x, dpi, 96),
                     MulDiv(item.y, dpi, 96), MulDiv(item.width, dpi, 96),
                     MulDiv(item.height, dpi, 96),
                     SWP_NOZORDER | SWP_NOACTIVATE);
        if (nextFont)
            SendMessageW(item.handle, WM_SETFONT,
                         reinterpret_cast<WPARAM>(nextFont), TRUE);
    }
    if (nextFont) {
        if (g_panelFont)
            DeleteObject(g_panelFont);
        g_panelFont = nextFont;
    }
}

static LRESULT CALLBACK PanelProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CREATE) {
        g_status = AddControl(w, L"STATIC", L"", SS_LEFT, 12, 12, 536, 52, 0);
        g_editButton =
            AddControl(w, L"BUTTON", L"Edit boxes", BS_PUSHBUTTON | WS_TABSTOP,
                       12, 76, 124, 30, 101);
        g_saveButton =
            AddControl(w, L"BUTTON", L"Save && apply",
                       BS_DEFPUSHBUTTON | WS_TABSTOP, 144, 76, 124, 30, 102);
        g_cancelButton =
            AddControl(w, L"BUTTON", L"Cancel", BS_PUSHBUTTON | WS_TABSTOP, 276,
                       76, 84, 30, 103);
        g_undoButton =
            AddControl(w, L"BUTTON", L"Undo last", BS_PUSHBUTTON | WS_TABSTOP,
                       368, 76, 88, 30, 104);
        g_clearButton =
            AddControl(w, L"BUTTON", L"Clear all", BS_PUSHBUTTON | WS_TABSTOP,
                       464, 76, 84, 30, 105);
        g_modeButton =
            AddControl(w, L"BUTTON", L"Use icon proximity",
                       BS_PUSHBUTTON | WS_TABSTOP, 12, 118, 190, 30, 106);
        g_previewButton =
            AddControl(w, L"BUTTON", L"Show outlines",
                       BS_AUTOCHECKBOX | WS_TABSTOP, 220, 118, 160, 30, 107);
        g_hideButton =
            AddControl(w, L"BUTTON", L"Hide panel", BS_PUSHBUTTON | WS_TABSTOP,
                       424, 118, 124, 30, 108);
        LayoutPanel(w);
        return 0;
    }
    if (msg == WM_DPICHANGED) {
        auto rectangle = reinterpret_cast<RECT*>(lp);
        SetWindowPos(w, nullptr, rectangle->left, rectangle->top,
                     rectangle->right - rectangle->left,
                     rectangle->bottom - rectangle->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        LayoutPanel(w);
        return 0;
    }
    if (msg == WM_DESTROY && g_panelFont) {
        DeleteObject(g_panelFont);
        g_panelFont = nullptr;
    }
    if (msg == WM_COMMAND && HIWORD(wp) == BN_CLICKED) {
        switch (LOWORD(wp)) {
            case 101:
                StartEditor();
                break;
            case 102:
                SaveEditor();
                break;
            case 103:
                CloseEditor();
                UpdatePanel(L"Canceled. Previous saved boxes are unchanged.");
                break;
            case 104:
                if (g_editorActive.load() && !g_editBoxes.empty())
                    g_editBoxes.pop_back();
                if (g_editorWindow)
                    InvalidateRect(g_editorWindow, nullptr, FALSE);
                UpdatePanel();
                break;
            case 105:
                if (g_editorActive.load())
                    g_editBoxes.clear();
                if (g_editorWindow)
                    InvalidateRect(g_editorWindow, nullptr, FALSE);
                UpdatePanel();
                break;
            case 106: {
                auto state = Snapshot();
                state.magic = kBoxesMagic;
                state.enabled = state.enabled ? 0 : 1;
                if (StoreState(state)) {
                    CommitState(state);
                    UpdatePanel();
                } else
                    UpdatePanel(
                        L"Could not save the mode change. The active mode was not changed.");
                break;
            }
            case 107:
                g_preview = SendMessageW(g_previewButton, BM_GETCHECK, 0, 0) ==
                            BST_CHECKED;
                Wh_SetIntValue(L"previewOverride", g_preview.load() ? 1 : 0);
                ++g_previewRevision;
                RequestDesktopUpdate();
                break;
            case 108:
                if (g_editorActive.load()) {
                    CloseEditor();
                    UpdatePanel(
                        L"Canceled. Previous saved boxes are unchanged.");
                }
                ShowWindow(w, SW_HIDE);
                break;
        }
        return 0;
    }
    if (msg == WM_CLOSE) {
        if (g_editorActive.load()) {
            CloseEditor();
            UpdatePanel(L"Canceled. Previous saved boxes are unchanged.");
        }
        ShowWindow(w, SW_HIDE);
        return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}

// Name rules are updated by the settings thread; item lifetimes live on the UI
// thread.
static SRWLOCK g_rulesLock = SRWLOCK_INIT;
static std::vector<std::wstring> g_alwaysVisible;
static std::atomic<unsigned> g_rulesRevision{1};
static std::atomic<bool> g_revealNewItems{true}, g_dismissNewOnClick{true};
static std::atomic<bool> g_keepSelectedVisible{true};
static std::atomic<int> g_newItemSeconds{15};
struct DesktopItem {
    LPARAM cookie = 0;
    std::wstring name;
};
struct TemporaryItem {
    DesktopItem identity;
    ULONGLONG until = 0;
};
static std::vector<DesktopItem> g_knownItems;
static std::vector<TemporaryItem> g_temporaryItems;
static bool g_itemsInitialized = false;
static int g_renameIndex = -1;
static ULONGLONG g_suppressNewUntil = 0;
static unsigned g_seenRulesRevision = 0, g_previousButtons = 0;
static std::wstring NormalizeIconName(std::wstring name) {
    auto first = name.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return {};
    name = name.substr(first, name.find_last_not_of(L" \t\r\n") - first + 1);
    for (auto& c : name)
        c = (wchar_t)towlower(c);
    for (const auto* extension : {L".lnk", L".url"}) {
        size_t length = wcslen(extension);
        if (name.size() >= length &&
            name.compare(name.size() - length, length, extension) == 0) {
            name.resize(name.size() - length);
            break;
        }
    }
    return name;
}
static std::vector<std::wstring> ParseIconPatterns(const std::wstring& text) {
    std::vector<std::wstring> patterns;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find_first_of(L";,\r\n", start);
        auto name = NormalizeIconName(
            text.substr(start, end == std::wstring::npos ? end : end - start));
        if (!name.empty())
            patterns.push_back(std::move(name));
        if (end == std::wstring::npos)
            break;
        start = end + 1;
    }
    return patterns;
}
static bool MatchIconPattern(const std::wstring& text,
                             const std::wstring& pattern) {
    size_t t = 0, p = 0, star = std::wstring::npos, retry = 0;
    while (t < text.size()) {
        if (p < pattern.size() &&
            (pattern[p] == L'?' || pattern[p] == text[t])) {
            ++t;
            ++p;
        } else if (p < pattern.size() && pattern[p] == L'*') {
            star = p++;
            retry = t;
        } else if (star != std::wstring::npos) {
            p = star + 1;
            t = ++retry;
        } else
            return false;
    }
    while (p < pattern.size() && pattern[p] == L'*')
        ++p;
    return p == pattern.size();
}
static bool SameDesktopItem(const DesktopItem& a, const DesktopItem& b) {
    if (a.cookie && b.cookie)
        return a.cookie == b.cookie;
    return !a.name.empty() && a.name == b.name;
}
static DesktopItem ReadDesktopItem(HWND list, int index) {
    WCHAR name[1024]{};
    LVITEMW item{};
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = index;
    item.pszText = name;
    item.cchTextMax = ARRAYSIZE(name);
    // GETITEM also supports text callbacks used by virtual list views.
    bool got = SendMessageW(list, LVM_GETITEMW, 0,
                            reinterpret_cast<LPARAM>(&item)) != FALSE;
    if (!name[0]) {
        LVITEMW text{};
        text.pszText = name;
        text.cchTextMax = ARRAYSIZE(name);
        SendMessageW(list, LVM_GETITEMTEXTW, index,
                     reinterpret_cast<LPARAM>(&text));
    }
    return {got ? item.lParam : 0, NormalizeIconName(name)};
}
static std::vector<DesktopItem> ReadDesktopSnapshot(HWND list) {
    std::vector<DesktopItem> items;
    int count = ListView_GetItemCount(list);
    items.reserve(count > 0 ? count : 0);
    for (int i = 0; i < count; ++i)
        items.push_back(ReadDesktopItem(list, i));
    return items;
}
static ULONGLONG NewItemDeadline(ULONGLONG now) {
    int seconds = g_newItemSeconds.load();
    return seconds ? now + (ULONGLONG)seconds * 1000 : 0;
}
static bool IsTemporary(const DesktopItem& identity, ULONGLONG now) {
    for (const auto& item : g_temporaryItems)
        if (SameDesktopItem(item.identity, identity) &&
            (!item.until || now < item.until))
            return true;
    return false;
}
static bool GrantTemporary(const DesktopItem& identity, ULONGLONG now) {
    if (!g_revealNewItems.load() || now < g_suppressNewUntil ||
        identity.name.empty())
        return false;
    for (auto& item : g_temporaryItems) {
        if (SameDesktopItem(item.identity, identity)) {
            item.identity = identity;
            item.until = NewItemDeadline(now);
            return false;
        }
    }
    if (g_temporaryItems.empty()) {
        g_previousButtons = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0) |
                            ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 2 : 0) |
                            ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) ? 4 : 0);
    }
    g_temporaryItems.push_back({identity, NewItemDeadline(now)});
    return true;
}
static bool IsAlwaysVisible(const std::wstring& name) {
    bool visible = false;
    AcquireSRWLockShared(&g_rulesLock);
    for (const auto& pattern : g_alwaysVisible) {
        if (MatchIconPattern(name, pattern)) {
            visible = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_rulesLock);
    return visible;
}
static int EditingItem(HWND list) {
    if (!ListView_GetEditControl(list))
        return -1;
    return g_renameIndex >= 0 ? g_renameIndex
                              : ListView_GetNextItem(list, -1, LVNI_FOCUSED);
}
// Cached item names and whitelist matches are rebuilt only after shell changes
// or a rule change. Animation paints never request icon display names.
static bool g_itemsDirty = true;
static unsigned g_itemRetryBudget = 3;
static ULONGLONG g_itemRetryAt = 0;
static std::vector<bool> g_cachedWhitelist;
static void MarkItemsDirty(HWND list) {
    g_itemsDirty = true;
    g_itemRetryBudget = 3;
    if (list && !g_stop.load())
        PostMessageW(list, g_tick, 0, 0);
}
static bool SpecialItemVisible(HWND list, int index) {
    if (g_keepSelectedVisible.load() &&
        (ListView_GetItemState(list, index, LVIS_SELECTED) & LVIS_SELECTED))
        return true;
    if (EditingItem(list) == index)
        return true;
    if (index < 0 || (size_t)index >= g_knownItems.size())
        return false;
    return ((size_t)index < g_cachedWhitelist.size() &&
            g_cachedWhitelist[index]) ||
           IsTemporary(g_knownItems[index], GetTickCount64());
}
static void RefreshItemCache(HWND list, ULONGLONG now) {
    auto items = ReadDesktopSnapshot(list);
    bool incomplete = false;
    if (g_itemsInitialized) {
        for (const auto& identity : items) {
            bool known = false;
            for (const auto& previous : g_knownItems) {
                if (!previous.name.empty() &&
                    SameDesktopItem(previous, identity)) {
                    known = true;
                    break;
                }
            }
            if (!known)
                GrantTemporary(identity, now);
        }
    }
    g_cachedWhitelist.clear();
    g_cachedWhitelist.reserve(items.size());
    for (const auto& identity : items) {
        incomplete |= identity.name.empty();
        g_cachedWhitelist.push_back(IsAlwaysVisible(identity.name));
    }
    g_knownItems = std::move(items);
    g_itemsInitialized = true;
    g_itemsDirty = false;
    g_itemRetryAt = 0;
    if (incomplete && g_itemRetryBudget) {
        --g_itemRetryBudget;
        g_itemRetryAt = now + 150;
    }
    InvalidateRect(list, nullptr, TRUE);
}
static void UpdateItemPolicies(HWND list, ULONGLONG now) {
    bool changed = false;
    unsigned revision = g_rulesRevision.load();
    if (revision != g_seenRulesRevision) {
        g_seenRulesRevision = revision;
        g_itemsDirty = true;
    }
    if (g_itemsDirty || (g_itemRetryAt && now >= g_itemRetryAt))
        RefreshItemCache(list, now);
    if (!g_revealNewItems.load() && !g_temporaryItems.empty()) {
        g_temporaryItems.clear();
        changed = true;
    }
    int editing = EditingItem(list);
    if (editing >= 0 && (size_t)editing < g_knownItems.size())
        changed |= GrantTemporary(g_knownItems[editing], now);
    // The slow policy timer runs only while temporary items or editing exist.
    if (!g_temporaryItems.empty() && g_dismissNewOnClick.load()) {
        unsigned buttons = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0) |
                           ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 2 : 0) |
                           ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) ? 4 : 0);
        bool clicked = (buttons & ~g_previousButtons) != 0;
        g_previousButtons = buttons;
        if (clicked) {
            POINT p{};
            GetCursorPos(&p);
            HWND hitWindow = WindowFromPoint(p);
            int hit = -1;
            if (hitWindow == list || (hitWindow && IsChild(list, hitWindow))) {
                if (hitWindow == ListView_GetEditControl(list))
                    hit = editing;
                else {
                    ScreenToClient(list, &p);
                    LVHITTESTINFO test{};
                    test.pt = p;
                    hit = ListView_HitTest(list, &test);
                }
            }
            DesktopItem clickedItem =
                hit >= 0 && (size_t)hit < g_knownItems.size()
                    ? g_knownItems[hit]
                    : DesktopItem{};
            auto count = g_temporaryItems.size();
            std::erase_if(g_temporaryItems, [&](const TemporaryItem& item) {
                return !SameDesktopItem(item.identity, clickedItem);
            });
            if (g_temporaryItems.size() != count) {
                changed = true;
                g_suppressNewUntil = now + 500;
                g_itemsDirty = true;
            }
        }
    }
    auto count = g_temporaryItems.size();
    std::erase_if(g_temporaryItems, [&](const TemporaryItem& item) {
        if (item.until && now >= item.until)
            return true;
        if (g_itemRetryAt)
            return false;
        for (const auto& identity : g_knownItems)
            if (SameDesktopItem(item.identity, identity))
                return false;
        return true;
    });
    changed |= g_temporaryItems.size() != count;
    if (changed)
        InvalidateRect(list, nullptr, TRUE);
    if (g_itemsDirty && !g_stop.load())
        PostMessageW(list, g_tick, 0, 0);
}
static void ResetItemPolicies() {
    g_knownItems.clear();
    g_cachedWhitelist.clear();
    g_temporaryItems.clear();
    g_itemsInitialized = false;
    g_itemsDirty = true;
    g_itemRetryBudget = 3;
    g_itemRetryAt = 0;
    g_renameIndex = -1;
    g_suppressNewUntil = 0;
    g_seenRulesRevision = 0;
    g_previousButtons = 0;
}

struct RegionFade {
    RECT box;
    double alpha;
    ULONGLONG lastInside;
    double target = 0;
};
// All region animations and item drawing context live on the desktop UI thread.
static std::vector<RegionFade> g_regions;
static bool g_regionsEnabled = false;
static ULONGLONG g_forceRevealUntil = 0;
static thread_local int g_itemOpacity = -1;
static int RenderOpacity() {
    // Only icon-item drawing is faded. Non-item drawing includes the native
    // drag-selection rectangle and the final desktop buffer composite.
    return g_itemOpacity >= 0 ? g_itemOpacity : 100;
}
static double FadeToward(double current, double target, double step) {
    return target > current ? std::min(target, current + step)
                            : std::max(target, current - step);
}
static int AlphaAtPoint(POINT p, const std::vector<RegionFade>& regions) {
    int alpha = 0;
    for (const auto& region : regions)
        if (InsideBox(p, region.box))
            alpha = std::max(alpha, (int)std::lround(region.alpha));
    return alpha;
}
static bool UpdateRegions(const std::vector<RECT>& boxes,
                          POINT cursor,
                          bool onDesktop,
                          bool holdVisible,
                          bool forceVisible,
                          ULONGLONG now,
                          double step) {
    std::vector<RegionFade> next;
    bool changed = boxes.size() != g_regions.size();
    for (size_t i = 0; i < boxes.size(); ++i) {
        RegionFade state{boxes[i], 0, 0};
        for (const auto& previous : g_regions) {
            if (EqualRect(&previous.box, &boxes[i])) {
                state = previous;
                break;
            }
        }
        if (i >= g_regions.size() || !EqualRect(&g_regions[i].box, &boxes[i]))
            changed = true;
        int before = (int)std::lround(state.alpha);
        bool hovered = onDesktop && InHoverBox(cursor, state.box);
        // Context menus/renaming/dragging keep already-visible groups
        // available.
        if (hovered || (holdVisible && state.alpha > 0))
            state.lastInside = now;
        bool delayed = state.lastInside != 0 &&
                       now - state.lastInside < (ULONGLONG)g_delay.load();
        state.target = (forceVisible || hovered || delayed) ? 100.0 : 0.0;
        state.alpha = FadeToward(state.alpha, state.target, step);
        changed |= (int)std::lround(state.alpha) != before;
        next.push_back(state);
    }
    g_regions = std::move(next);
    return changed;
}
static int ItemOpacity(HWND list, int index) {
    if (SpecialItemVisible(list, index))
        return 100;
    if (g_editorActive.load() || GetTickCount64() < g_forceRevealUntil)
        return 100;
    if (!g_independent.load() || !g_regionsEnabled)
        return g_opacity.load();
    RECT icon{};
    if (!ListView_GetItemRect(list, index, &icon, LVIR_ICON))
        return 0;
    POINT center{icon.left + (icon.right - icon.left) / 2,
                 icon.top + (icon.bottom - icon.top) / 2};
    ClientToScreen(list, &center);
    return AlphaAtPoint(center, g_regions);
}
static LRESULT CALLBACK
DesktopParentSubclass(HWND w, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_NOTIFY && lp) {
        auto header = reinterpret_cast<NMHDR*>(lp);
        if (header->hwndFrom == g_list.load()) {
            HWND list = header->hwndFrom;
            if (header->code == LVN_BEGINLABELEDITW ||
                header->code == LVN_BEGINLABELEDITA) {
                // The iItem field has the same layout in ANSI and Unicode
                // notifications.
                auto info = reinterpret_cast<NMLVDISPINFOW*>(lp);
                g_renameIndex = info->item.iItem;
                GrantTemporary(ReadDesktopItem(list, g_renameIndex),
                               GetTickCount64());
                InvalidateRect(list, nullptr, TRUE);
                auto result = DefSubclassProc(w, msg, wp, lp);
                if (result)
                    g_renameIndex = -1;
                MarkItemsDirty(list);
                return result;
            }
            if (header->code == LVN_ENDLABELEDITW ||
                header->code == LVN_ENDLABELEDITA) {
                // Keep the old identity until the next scan adopts the
                // committed name.
                if (g_renameIndex >= 0)
                    GrantTemporary(ReadDesktopItem(list, g_renameIndex),
                                   GetTickCount64());
                auto result = DefSubclassProc(w, msg, wp, lp);
                g_renameIndex = -1;
                MarkItemsDirty(list);
                InvalidateRect(list, nullptr, TRUE);
                return result;
            }
            if (g_keepSelectedVisible.load() &&
                header->code == LVN_ITEMCHANGED) {
                auto change = reinterpret_cast<NMLISTVIEW*>(lp);
                if ((change->uChanged & LVIF_STATE) &&
                    ((change->uOldState ^ change->uNewState) & LVIS_SELECTED))
                    InvalidateRect(list, nullptr, TRUE);
            }
            if (g_keepSelectedVisible.load() &&
                header->code == LVN_ODSTATECHANGED) {
                auto change = reinterpret_cast<NMLVODSTATECHANGE*>(lp);
                if ((change->uOldState ^ change->uNewState) & LVIS_SELECTED)
                    InvalidateRect(list, nullptr, TRUE);
            }
            if (header->code == LVN_INSERTITEM) {
                auto change = reinterpret_cast<NMLISTVIEW*>(lp);
                if (g_itemsInitialized)
                    GrantTemporary(ReadDesktopItem(list, change->iItem),
                                   GetTickCount64());
                MarkItemsDirty(list);
                InvalidateRect(list, nullptr, TRUE);
            }
        }
        auto draw = reinterpret_cast<NMLVCUSTOMDRAW*>(lp);
        if (draw->nmcd.hdr.hwndFrom == g_list.load() &&
            draw->nmcd.hdr.code == NM_CUSTOMDRAW && g_inDesktopPaint) {
            DWORD stage = draw->nmcd.dwDrawStage;
            if (stage == CDDS_PREPAINT) {
                g_itemOpacity = -1;
                return DefSubclassProc(w, msg, wp, lp) | CDRF_NOTIFYITEMDRAW |
                       CDRF_NOTIFYPOSTPAINT;
            }
            if (stage == CDDS_ITEMPREPAINT) {
                g_itemOpacity = ItemOpacity(draw->nmcd.hdr.hwndFrom,
                                            (int)draw->nmcd.dwItemSpec);
                if (g_itemOpacity == 0) {
                    g_itemOpacity = -1;
                    return CDRF_SKIPDEFAULT;
                }
                auto result = DefSubclassProc(w, msg, wp, lp);
                return result | CDRF_NOTIFYPOSTPAINT;
            }
            if (stage == CDDS_ITEMPOSTPAINT || stage == CDDS_POSTPAINT) {
                auto result = DefSubclassProc(w, msg, wp, lp);
                g_itemOpacity = -1;
                return result;
            }
        }
    }
    if (msg == WM_NCDESTROY) {
        g_parent = nullptr;
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(w,
                                                         DesktopParentSubclass);
    }
    return DefSubclassProc(w, msg, wp, lp);
}

// The following state is owned by the desktop UI thread.
static ULONGLONG g_lastNear = 0, g_lastTick = 0;
static double g_alpha = 100;
static bool g_dragging = false, g_mouseTracking = false;
static char g_timerTokens[3];
static bool g_timerRunning[3]{};
static UINT_PTR DesktopTimerId(int index) {
    return reinterpret_cast<UINT_PTR>(&g_timerTokens[index]);
}
static void SetDesktopTimer(HWND list, int index, bool needed, UINT interval) {
    if (needed && !g_timerRunning[index])
        g_timerRunning[index] =
            SetTimer(list, DesktopTimerId(index), interval, nullptr) != 0;
    else if (!needed && g_timerRunning[index]) {
        KillTimer(list, DesktopTimerId(index));
        g_timerRunning[index] = false;
    }
}
static void StopDesktopTimers(HWND list) {
    for (int i = 0; i < 3; ++i) {
        KillTimer(list, DesktopTimerId(i));
        g_timerRunning[i] = false;
    }
    if (g_mouseTracking) {
        TRACKMOUSEEVENT track{sizeof(track), TME_CANCEL | TME_LEAVE, list, 0};
        TrackMouseEvent(&track);
        g_mouseTracking = false;
    }
}
static void Repaint(HWND w) {
    InvalidateRect(w, nullptr, TRUE);
}
static void Reveal(HWND w) {
    g_lastNear = GetTickCount64();
    g_forceRevealUntil = g_lastNear + 1500;
    g_alpha = 100;
    g_opacity = 100;
    Repaint(w);
}
static bool IsDesktopSurface(HWND list, POINT p) {
    HWND hit = WindowFromPoint(p);
    return hit == list || (hit && IsChild(list, hit)) ||
           hit == GetParent(list) || hit == GetAncestor(list, GA_ROOT);
}
static bool NearAnyIcon(HWND w, POINT p) {
    ScreenToClient(w, &p);
    int count = ListView_GetItemCount(w);
    int radius = MulDiv(g_distance.load(), GetDpiForWindow(w), 96);
    for (int i = 0; i < count; ++i) {
        RECT r{};
        if (!ListView_GetItemRect(w, i, &r, LVIR_ICON))
            continue;
        double dx =
            std::max({double(r.left - p.x), 0.0, double(p.x - r.right)});
        double dy =
            std::max({double(r.top - p.y), 0.0, double(p.y - r.bottom)});
        if (dx * dx + dy * dy <= double(radius) * radius)
            return true;
    }
    return false;
}
static bool InRevealArea(HWND w, POINT p, const std::vector<RECT>& boxes) {
    if (boxes.empty())
        return NearAnyIcon(w, p);
    for (const auto& r : boxes)
        if (InHoverBox(p, r))
            return true;
    return false;
}
static void Tick(HWND w) {
    if (g_stop.load())
        return;
    ULONGLONG now = GetTickCount64();
    UpdateItemPolicies(w, now);
    auto elapsed = g_timerRunning[0] && g_lastTick
                       ? std::min<ULONGLONG>(now - g_lastTick, 100)
                       : 0;
    g_lastTick = now;
    auto boxes = ActiveBoxes();
    POINT p{};
    bool onDesktop = GetCursorPos(&p) && IsDesktopSurface(w, p);
    bool hovered = onDesktop && InRevealArea(w, p, boxes);
    bool editing = ListView_GetEditControl(w) != nullptr;
    GUITHREADINFO gui{};
    gui.cbSize = sizeof(gui);
    bool menu = GetGUIThreadInfo(GetCurrentThreadId(), &gui) &&
                (gui.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE));
    if (g_dragging && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        g_dragging = false;
    bool editor = g_editorActive.load();
    bool forced = now < g_forceRevealUntil;
    bool hold = editing || menu || g_dragging;
    if (hovered || hold || editor)
        g_lastNear = now;
    bool delayed = g_lastNear && now - g_lastNear < (ULONGLONG)g_delay.load();
    double target = hovered || hold || editor || forced || delayed ? 100 : 0;
    int duration = g_fade.load();
    double step = editor || !duration ? 100.0 : 100.0 * elapsed / duration;
    g_alpha = editor ? 100.0 : FadeToward(g_alpha, target, step);
    bool independent = g_independent.load() && !boxes.empty();
    bool changedMode = independent != g_regionsEnabled;
    g_regionsEnabled = independent;
    bool animation = false, waiting = false;
    if (independent) {
        bool changed = UpdateRegions(boxes, p, onDesktop, hold,
                                     editor || forced, now, step);
        int maximum = editor || forced ? 100 : 0;
        for (const auto& region : g_regions) {
            maximum = std::max(maximum, (int)std::lround(region.alpha));
            animation |= region.alpha != region.target;
            bool inside = onDesktop && InHoverBox(p, region.box);
            waiting |= !inside && !hold && region.lastInside &&
                       now - region.lastInside < (ULONGLONG)g_delay.load();
        }
        bool changedAlpha = g_opacity.exchange(maximum) != maximum;
        if (changed || changedAlpha || changedMode)
            Repaint(w);
    } else {
        g_regions.clear();
        int alpha = (int)std::lround(g_alpha);
        if (g_opacity.exchange(alpha) != alpha || changedMode)
            Repaint(w);
        animation = g_alpha != target;
        waiting = delayed && !hovered && !hold && !editor;
    }
    SetDesktopTimer(w, 0, !editor && (animation || waiting || forced), 16);
    SetDesktopTimer(w, 1, !editor && g_opacity.load() > 0, 150);
    SetDesktopTimer(
        w, 2, !g_temporaryItems.empty() || editing || g_itemRetryAt != 0, 100);
}
static LRESULT CALLBACK
DesktopSubclass(HWND w, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == g_tick) {
        if (wp == 2) {
            StopDesktopTimers(w);
            return 0;
        }
        if (wp == 1)
            MarkItemsDirty(w);
        Tick(w);
        if (wp == 1 || wp == 3) {
            Repaint(w);
            UpdateWindow(w);
        }
        return 0;
    }
    if (msg == WM_TIMER) {
        for (int i = 0; i < 3; ++i) {
            if (wp == DesktopTimerId(i)) {
                Tick(w);
                return 0;
            }
        }
    }
    if (msg == WM_MOUSEMOVE) {
        if (!g_mouseTracking) {
            TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, w, 0};
            g_mouseTracking = TrackMouseEvent(&track) != FALSE;
        }
        Tick(w);
    }
    if (msg == WM_MOUSELEAVE) {
        g_mouseTracking = false;
        Tick(w);
    }
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
        msg == WM_MBUTTONDOWN) {
        if (g_opacity.load() > 0)
            g_dragging = true;
        Tick(w);
    }
    if (msg == WM_LBUTTONUP || msg == WM_CAPTURECHANGED) {
        g_dragging = false;
        Tick(w);
    }
    if (msg == WM_KEYDOWN) {
        bool modifier = wp == VK_CONTROL || wp == VK_SHIFT || wp == VK_MENU ||
                        wp == VK_LWIN || wp == VK_RWIN;
        bool selectAll = wp == 'A' && (GetKeyState(VK_CONTROL) & 0x8000);
        int focused = ListView_GetNextItem(w, -1, LVNI_FOCUSED);
        if (!modifier && !selectAll &&
            (g_opacity.load() < 100 ||
             (focused >= 0 && ItemOpacity(w, focused) < 100)))
            Reveal(w);
        // Preserve native type-to-find, navigation, rename and Delete behavior.
        auto result = DefSubclassProc(w, msg, wp, lp);
        if (!modifier) {
            focused = ListView_GetNextItem(w, -1, LVNI_FOCUSED);
            if (focused >= 0 && ItemOpacity(w, focused) < 100)
                Reveal(w);
            Tick(w);
            Repaint(w);
        }
        return result;
    }
    if (msg == WM_PAINT || msg == WM_PRINTCLIENT) {
        bool previous = g_inDesktopPaint;
        g_inDesktopPaint = true;
        int previousOpacity = g_itemOpacity;
        g_itemOpacity = -1;
        auto result = DefSubclassProc(w, msg, wp, lp);
        g_inDesktopPaint = previous;
        g_itemOpacity = previousOpacity;
        return result;
    }
    if (msg == WM_NCDESTROY) {
        StopDesktopTimers(w);
        g_list = nullptr;
        g_lastTick = 0;
        g_dragging = false;
        g_regions.clear();
        g_regionsEnabled = false;
        g_forceRevealUntil = 0;
        ResetItemPolicies();
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(w, DesktopSubclass);
        WakeWorker();
    }
    bool itemsChanged = msg == LVM_INSERTITEMW || msg == LVM_INSERTITEMA ||
                        msg == LVM_DELETEITEM || msg == LVM_DELETEALLITEMS ||
                        msg == LVM_SETITEMCOUNT || msg == LVM_SETITEMW ||
                        msg == LVM_SETITEMA || msg == LVM_SETITEMTEXTW ||
                        msg == LVM_SETITEMTEXTA || msg == LVM_SORTITEMS ||
                        msg == LVM_SORTITEMSEX;
    if (itemsChanged) {
        auto result = DefSubclassProc(w, msg, wp, lp);
        MarkItemsDirty(w);
        return result;
    }
    return DefSubclassProc(w, msg, wp, lp);
}

static BOOL CALLBACK FindDesktop(HWND root, LPARAM param) {
    HWND view = FindWindowExW(root, nullptr, L"SHELLDLL_DefView", nullptr);
    HWND list = view ? FindWindowExW(view, nullptr, L"SysListView32", nullptr)
                     : nullptr;
    DWORD pid = 0;
    if (list)
        GetWindowThreadProcessId(list, &pid);
    if (pid == GetCurrentProcessId()) {
        *reinterpret_cast<HWND*>(param) = list;
        return FALSE;
    }
    return TRUE;
}
static DWORD WINAPI Worker(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OverlayProc),
                       &g_overlayModule);
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = g_overlayModule;
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32515));
    bool overlayReady = RegisterClassExW(&wc) != 0;
    wc.lpfnWndProc = PanelProc;
    wc.lpszClassName = kPanelClass;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    bool panelReady = RegisterClassExW(&wc) != 0;
    if (panelReady) {
        UINT dpi = GetDpiForSystem();
        RECT outer{0, 0, MulDiv(560, dpi, 96), MulDiv(164, dpi, 96)};
        AdjustWindowRectExForDpi(&outer, WS_CAPTION | WS_SYSMENU, FALSE,
                                 WS_EX_TOPMOST | WS_EX_TOOLWINDOW, dpi);
        g_panel = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kPanelClass,
            L"Desktop Icons - Reveal Boxes", WS_CAPTION | WS_SYSMENU, 80, 80,
            outer.right - outer.left, outer.bottom - outer.top, nullptr,
            nullptr, g_overlayModule, nullptr);
        UpdatePanel();
    }
    unsigned lastRevision = 0;
    bool previousVisible = false;
    UINT_PTR discoveryTimer = 0;
    while (!g_stop.load()) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (g_editorActive.load() && message.message == WM_KEYDOWN) {
                if (message.wParam == VK_RETURN) {
                    SaveEditor();
                    continue;
                }
                if (message.wParam == VK_ESCAPE) {
                    CloseEditor();
                    UpdatePanel(
                        L"Canceled. Previous saved boxes are unchanged.");
                    continue;
                }
            }
            if (g_panel && IsDialogMessageW(g_panel, &message))
                continue;
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (g_panelRequest.exchange(false)) {
            if (g_showControls.load())
                ShowPanel(false);
            else {
                CloseEditor();
                if (g_panel)
                    ShowWindow(g_panel, SW_HIDE);
            }
            UpdatePanel();
        }
        unsigned revision = g_previewRevision.load();
        if (revision != lastRevision) {
            bool visible = g_preview.load() && !g_editorActive.load() &&
                           !ActiveBoxes().empty();
            if (visible && overlayReady && !g_outlineWindow)
                g_outlineWindow = CreateOverlay(false);
            if (g_outlineWindow) {
                if (visible != previousVisible)
                    ShowWindow(g_outlineWindow,
                               visible ? SW_SHOWNOACTIVATE : SW_HIDE);
                if (visible) {
                    SetWindowPos(g_outlineWindow, HWND_TOPMOST,
                                 GetSystemMetrics(SM_XVIRTUALSCREEN),
                                 GetSystemMetrics(SM_YVIRTUALSCREEN),
                                 GetSystemMetrics(SM_CXVIRTUALSCREEN),
                                 GetSystemMetrics(SM_CYVIRTUALSCREEN),
                                 SWP_NOACTIVATE);
                    InvalidateRect(g_outlineWindow, nullptr, FALSE);
                    if (g_panel && IsWindowVisible(g_panel))
                        SetWindowPos(g_panel, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                }
            }
            previousVisible = visible && g_outlineWindow;
            lastRevision = revision;
        }
        HWND list = g_list.load();
        if (!list || !IsWindow(list)) {
            HWND found = nullptr;
            EnumWindows(FindDesktop, reinterpret_cast<LPARAM>(&found));
            if (found) {
                HWND parent = GetParent(found);
                if (parent && WindhawkUtils::SetWindowSubclassFromAnyThread(
                                  parent, DesktopParentSubclass, 0)) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                            found, DesktopSubclass, 0)) {
                        g_parent = parent;
                        g_list = found;
                        list = found;
                        PostMessageW(found, g_tick, 1, 0);
                    } else
                        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                            parent, DesktopParentSubclass);
                }
            }
        }
        if (list && discoveryTimer) {
            KillTimer(nullptr, discoveryTimer);
            discoveryTimer = 0;
        } else if (!list && !discoveryTimer)
            discoveryTimer = SetTimer(nullptr, 0, 2000, nullptr);
        // No periodic worker wakeups while the desktop exists. UI changes,
        // settings, shutdown, and the temporary discovery timer wake this pump.
        MsgWaitForMultipleObjects(1, &g_wake, FALSE, INFINITE, QS_ALLINPUT);
    }
    if (discoveryTimer)
        KillTimer(nullptr, discoveryTimer);
    CloseEditor();
    if (g_outlineWindow) {
        DestroyWindow(g_outlineWindow);
        g_outlineWindow = nullptr;
    }
    if (g_panel) {
        DestroyWindow(g_panel);
        g_panel = nullptr;
    }
    if (panelReady)
        UnregisterClassW(kPanelClass, g_overlayModule);
    if (overlayReady)
        UnregisterClassW(kOverlayClass, g_overlayModule);
    return 0;
}
// 1. Hook for ImageList_DrawIndirect
using ImageList_DrawIndirect_t = decltype(&ImageList_DrawIndirect);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original = nullptr;

BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    if (pimldp && g_inDesktopPaint) {
        if (RenderOpacity() <= 0) {
            return TRUE;
        }
        if (RenderOpacity() >= 100) {
            return ImageList_DrawIndirect_Original(pimldp);
        }

        IMAGELISTDRAWPARAMS params = {};
        memcpy(&params, pimldp,
               std::min<DWORD>(pimldp->cbSize, sizeof(params)));
        params.cbSize = sizeof(params);

        if (!(params.fState & ILS_ALPHA)) {
            params.Frame = 255;
        }
        params.fState |= ILS_ALPHA;
        params.Frame = (DWORD)((params.Frame * RenderOpacity()) / 100);

        g_inDirectHook = true;
        BOOL result = ImageList_DrawIndirect_Original(&params);
        g_inDirectHook = false;
        return result;
    }
    return ImageList_DrawIndirect_Original(pimldp);
}

// 2. Hook for GdiAlphaBlend
using GdiAlphaBlend_t = BOOL(WINAPI*)(HDC hdcDest,
                                      int xoriginDest,
                                      int yoriginDest,
                                      int wDest,
                                      int hDest,
                                      HDC hdcSrc,
                                      int xoriginSrc,
                                      int yoriginSrc,
                                      int wSrc,
                                      int hSrc,
                                      BLENDFUNCTION ftn);
GdiAlphaBlend_t GdiAlphaBlend_Original = nullptr;

BOOL WINAPI GdiAlphaBlend_Hook(HDC hdcDest,
                               int xoriginDest,
                               int yoriginDest,
                               int wDest,
                               int hDest,
                               HDC hdcSrc,
                               int xoriginSrc,
                               int yoriginSrc,
                               int wSrc,
                               int hSrc,
                               BLENDFUNCTION ftn) {
    if (!g_inDirectHook && !g_inTextHook && g_inDesktopPaint) {
        if (RenderOpacity() <= 0) {
            return TRUE;
        }

        if (RenderOpacity() < 100) {
            ftn.SourceConstantAlpha =
                (BYTE)((ftn.SourceConstantAlpha * RenderOpacity()) / 100);
        }
    }
    return GdiAlphaBlend_Original(hdcDest, xoriginDest, yoriginDest, wDest,
                                  hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc,
                                  hSrc, ftn);
}

// 3. Hook for DrawShadowText
using DrawShadowText_t = decltype(&DrawShadowText);
DrawShadowText_t DrawShadowText_Original = nullptr;

int WINAPI DrawShadowText_Hook(HDC hdc,
                               LPCWSTR pszText,
                               UINT cch,
                               RECT* prc,
                               DWORD dwFlags,
                               COLORREF crText,
                               COLORREF crShadow,
                               int ixOffset,
                               int iyOffset) {
    if (!g_inDesktopPaint || g_inTextHook) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    if (dwFlags & DT_CALCRECT) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    if (RenderOpacity() >= 100) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    if (RenderOpacity() <= 0) {
        return (prc ? (prc->bottom - prc->top) : 1);
    }

    if (!prc) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    int pad = 4;
    int x = prc->left - pad;
    int y = prc->top - pad;
    int w = (prc->right - prc->left) + pad * 2;
    int h = (prc->bottom - prc->top) + pad * 2;

    if (w <= 0 || h <= 0) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    if (!hMemDC) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);
    if (!hBmp) {
        DeleteDC(hMemDC);
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                       crShadow, ixOffset, iyOffset);
    }

    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = {pad, pad, pad + (prc->right - prc->left),
                      pad + (prc->bottom - prc->top)};

    g_inTextHook = true;
    int result =
        DrawShadowText_Original(hMemDC, pszText, cch, &localRect, dwFlags,
                                crText, crShadow, ixOffset, iyOffset);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = (BYTE)((RenderOpacity() * 255) / 100);
    bf.AlphaFormat = 0;

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// 4. Hook for DrawTextW
using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original = nullptr;

int WINAPI DrawTextW_Hook(HDC hdc,
                          LPCWSTR lpchText,
                          int cchText,
                          LPRECT lprc,
                          UINT format) {
    if (!g_inDesktopPaint || g_inTextHook) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (format & DT_CALCRECT) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (RenderOpacity() >= 100) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (RenderOpacity() <= 0) {
        return (lprc ? (lprc->bottom - lprc->top) : 1);
    }

    if (!lprc) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    int pad = 4;
    int x = lprc->left - pad;
    int y = lprc->top - pad;
    int w = (lprc->right - lprc->left) + pad * 2;
    int h = (lprc->bottom - lprc->top) + pad * 2;

    if (w <= 0 || h <= 0) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    if (!hMemDC) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);
    if (!hBmp) {
        DeleteDC(hMemDC);
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    SetTextColor(hMemDC, GetTextColor(hdc));
    SetBkMode(hMemDC, TRANSPARENT);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = {pad, pad, pad + (lprc->right - lprc->left),
                      pad + (lprc->bottom - lprc->top)};

    g_inTextHook = true;
    int result =
        DrawTextW_Original(hMemDC, lpchText, cchText, &localRect, format);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = (BYTE)((RenderOpacity() * 255) / 100);
    bf.AlphaFormat = 0;

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// 5. Hook for ExtTextOutW
using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(HDC hdc,
                             int x,
                             int y,
                             UINT options,
                             const RECT* lprect,
                             LPCWSTR lpString,
                             UINT c,
                             const INT* lpDx) {
    if (g_inDesktopPaint && !g_inTextHook) {
        if (RenderOpacity() <= 0) {
            return TRUE;
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

static bool g_settingsLoaded = false;
static bool g_lastPreviewSetting = false;
static void LoadSettings() {
    g_keepSelectedVisible = Wh_GetIntSetting(L"keepSelectedVisible") != 0;
    g_revealNewItems = Wh_GetIntSetting(L"showNewItems") != 0;
    g_dismissNewOnClick = Wh_GetIntSetting(L"dismissNewOnClick") != 0;
    g_newItemSeconds = std::clamp(Wh_GetIntSetting(L"newItemSeconds"), 0, 300);
    auto names = WindhawkUtils::StringSetting::make(L"alwaysVisible");
    auto patterns = ParseIconPatterns(names.get());
    AcquireSRWLockExclusive(&g_rulesLock);
    g_alwaysVisible = std::move(patterns);
    ReleaseSRWLockExclusive(&g_rulesLock);
    ++g_rulesRevision;
    g_hoverPadding = std::clamp(Wh_GetIntSetting(L"boxHoverPadding"), 0, 1000);
    g_independent = Wh_GetIntSetting(L"independentBoxes") != 0;
    g_manualBox = Wh_GetIntSetting(L"manualBox") != 0;
    bool show = Wh_GetIntSetting(L"showEditorPanel") != 0;
    bool previousShow = g_showControls.exchange(show);
    if (g_settingsLoaded && previousShow != show)
        g_panelRequest = true;
    bool previewSetting = Wh_GetIntSetting(L"showPreview") != 0;
    if (!g_settingsLoaded) {
        int savedPreview = Wh_GetIntValue(L"previewOverride", -1);
        g_preview = savedPreview < 0 ? previewSetting : savedPreview != 0;
    } else if (previewSetting != g_lastPreviewSetting) {
        g_preview = previewSetting;
        Wh_DeleteValue(L"previewOverride");
    }
    g_lastPreviewSetting = previewSetting;
    int x = std::clamp(Wh_GetIntSetting(L"boxX"), -131072, 131072);
    int y = std::clamp(Wh_GetIntSetting(L"boxY"), -131072, 131072);
    int width = std::clamp(Wh_GetIntSetting(L"boxWidth"), 5, 65536);
    int height = std::clamp(Wh_GetIntSetting(L"boxHeight"), 5, 65536);
    AcquireSRWLockExclusive(&g_boxLock);
    g_numericBox = {x, y, x + width, y + height};
    ReleaseSRWLockExclusive(&g_boxLock);
    g_distance = std::clamp(Wh_GetIntSetting(L"proximity"), 0, 400);
    g_delay = std::clamp(Wh_GetIntSetting(L"hideDelay"), 0, 5000);
    g_fade = std::clamp(Wh_GetIntSetting(L"fadeDuration"), 0, 2000);
    ++g_previewRevision;
    RequestDesktopUpdate();
    g_settingsLoaded = true;
}
BOOL Wh_ModInit() {
    DWORD shellPid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    if (shellPid && shellPid != GetCurrentProcessId())
        return FALSE;
    BoxState saved{};
    if (Wh_GetBinaryValue(L"revealBoxes.v3", &saved, sizeof(saved)) ==
            sizeof(saved) &&
        saved.magic == kBoxesMagic && saved.count <= kMaxBoxes &&
        saved.enabled <= 1) {
        bool valid = true;
        for (DWORD i = 0; i < saved.count; ++i)
            if (!ValidBox(saved.boxes[i]))
                valid = false;
        if (valid)
            CommitState(saved);
    }
    LoadSettings();
    g_tick = RegisterWindowMessageW(L"DesktopIconsProximityReveal.Tick.0.1");
    if (!g_tick)
        return FALSE;
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_wake)
        return FALSE;
    bool ok = WindhawkUtils::SetFunctionHook(ImageList_DrawIndirect,
                                             ImageList_DrawIndirect_Hook,
                                             &ImageList_DrawIndirect_Original);
    ok &= WindhawkUtils::SetFunctionHook(DrawShadowText, DrawShadowText_Hook,
                                         &DrawShadowText_Original);
    ok &= WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook,
                                         &DrawTextW_Original);
    ok &= WindhawkUtils::SetFunctionHook(ExtTextOutW, ExtTextOutW_Hook,
                                         &ExtTextOutW_Original);
    HMODULE gdi = GetModuleHandleW(L"gdi32full.dll");
    if (!gdi)
        gdi = GetModuleHandleW(L"gdi32.dll");
    auto blend = gdi ? reinterpret_cast<GdiAlphaBlend_t>(
                           GetProcAddress(gdi, "GdiAlphaBlend"))
                     : nullptr;
    if (blend)
        ok &= WindhawkUtils::SetFunctionHook(blend, GdiAlphaBlend_Hook,
                                             &GdiAlphaBlend_Original);
    if (!ok) {
        CloseHandle(g_wake);
        g_wake = nullptr;
        return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread)
        Wh_Log(L"Could not start proximity worker; icons remain visible.");
}
void Wh_ModSettingsChanged() {
    LoadSettings();
}
void Wh_ModUninit() {
    g_stop = true;
    if (g_wake)
        SetEvent(g_wake);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    HWND w = g_list.load();
    if (w && IsWindow(w))
        SendMessageW(w, g_tick, 2, 0);
    g_list = nullptr;
    g_opacity = 100;
    HWND parent = g_parent.exchange(nullptr);
    if (parent && IsWindow(parent))
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent,
                                                         DesktopParentSubclass);
    if (w && IsWindow(w)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(w, DesktopSubclass);
        Repaint(w);
    }
    if (g_wake) {
        CloseHandle(g_wake);
        g_wake = nullptr;
    }
}
