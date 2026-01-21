// ==WindhawkMod==
// @id              hover-text-magnifier-forked
// @name            Hover Text Magnifier (macOS-style) - Fork
// @description     On-cursor hover bubble with large text via UI Automation; optional pixel magnifier fallback.
// @version         1.1.0
// @author          Math Shamenson
// @include         *
// @compilerOptions -lgdi32 -luxtheme -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Hold a trigger key (default: Ctrl) to show a bubble near the cursor.

Modes:
- auto: Text first via UI Automation; fallback to pixel magnifier if enabled.
- text: Text only
- magnifier: Pixel magnifier only

Notes:
- Some apps do not expose text through UI Automation (games, video, custom renderers).
- Auto mode can fall back to the magnifier in those cases.
- You can customize colors and font in settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- triggerKey: ctrl
  $name: Trigger key
  $description: Hold this key to show the bubble (use "none" for always-on).
  $options:
    - none: Always on
    - ctrl: Ctrl
    - alt: Alt
    - shift: Shift
    - win: Win

- mode: auto
  $name: Mode
  $description: auto=text first (UIA) then optional magnifier fallback.
  $options:
    - auto: Auto (Text then Magnifier fallback)
    - text: Text only
    - magnifier: Magnifier only

- textUnit: word
  $name: Text unit (UI Automation)
  $options:
    - word: Word
    - line: Line
    - paragraph: Paragraph

- hideWhenNoText: false
  $name: Hide bubble when no text is found (Text/Auto)

- fallbackToMagnifier: true
  $name: Fallback to magnifier when no text is found (Auto)

- zoomPercent: 250
  $name: Magnifier zoom (%)

- bubbleWidth: 520
  $name: Bubble width (px)

- bubbleHeight: 160
  $name: Bubble height (px)

- offsetX: 24
  $name: Bubble offset X (px)

- offsetY: 24
  $name: Bubble offset Y (px)

- cornerRadius: 16
  $name: Corner radius (px)

- borderWidth: 1
  $name: Border width (px)

- textPointSize: 26
    $name: Text size (pt)

- fontName: Segoe UI
    $name: Font name

- fontWeight: 600
    $name: Font weight (100-900)

- textColor: 0xF5F5F5
    $name: Text color (0xRRGGBB)

- backgroundColor: 0x141414
    $name: Background color (0xRRGGBB)

- borderColor: 0x5A5A5A
    $name: Border color (0xRRGGBB)

- padding: 18
  $name: Text padding (px)

- updateIntervalMs: 16
  $name: Update interval (ms)

- uiaQueryMinIntervalMs: 60
  $name: Min UIA query interval (ms)

- maxTextLen: 220
  $name: Max extracted text length

- opacity: 245
  $name: Bubble opacity (0-255)
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <commctrl.h>   // ensures HIMAGELIST exists for uxtheme.h
#include <uxtheme.h>
#include <windhawk_utils.h>

#include <UIAutomation.h>
#include <objbase.h>
#include <oleauto.h>

#include <atomic>
#include <thread>
#include <string>
#include <algorithm>
#include <cmath>

static constexpr wchar_t kHostClassName[] = L"WH_HoverTextMagnifierHost";

typedef struct {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} MAGRECTANGLE;

typedef BOOL (WINAPI *PFNMAGINIT)(void);
typedef BOOL (WINAPI *PFNMAGUNINIT)(void);
typedef BOOL (WINAPI *PFNMAGSETWINDOWSOURCE)(HWND hwnd, MAGRECTANGLE rect);
typedef BOOL (WINAPI *PFNMAGSETWINDOWTRANSFORM)(HWND hwnd, float *matrix);
typedef BOOL (WINAPI *PFNMAGSETWINDOWFILTERLIST)(HWND hwnd, DWORD dwFilterMode, int count, HWND *pHWND);

static constexpr DWORD MW_FILTERMODE_EXCLUDE = 0;

static PFNMAGINIT                 pfnMagInit = nullptr;
static PFNMAGUNINIT               pfnMagUninit = nullptr;
static PFNMAGSETWINDOWSOURCE      pfnMagSetWindowSource = nullptr;
static PFNMAGSETWINDOWTRANSFORM   pfnMagSetWindowTransform = nullptr;
static PFNMAGSETWINDOWFILTERLIST  pfnMagSetWindowFilterList = nullptr;

enum class TriggerKey { None, Ctrl, Alt, Shift, Win };
enum class Mode { Auto, TextOnly, MagnifierOnly };
enum class HoverTextUnit { Word, Line, Paragraph }; // avoid UIA TextUnit name collision

struct AppSettings {
    TriggerKey triggerKey = TriggerKey::Ctrl;
    Mode mode = Mode::Auto;
    HoverTextUnit textUnit = HoverTextUnit::Word;

    bool hideWhenNoText = false;
    bool fallbackToMagnifier = true;

    int zoomPercent = 250;

    int bubbleWidth = 520;
    int bubbleHeight = 160;
    int offsetX = 24;
    int offsetY = 24;

    int cornerRadius = 16;
    int borderWidth = 1;
    int textPointSize = 26;
    std::wstring fontName = L"Segoe UI";
    int fontWeight = 600;
    int textColor = 0xF5F5F5;
    int backgroundColor = 0x141414;
    int borderColor = 0x5A5A5A;
    int padding = 18;

    int updateIntervalMs = 16;
    int uiaQueryMinIntervalMs = 60;
    int maxTextLen = 220;

    int opacity = 245;
};

struct RuntimeState {
    std::atomic<bool> running{false};
    std::thread worker;
    DWORD threadId = 0;

    HWND hwndHost = nullptr;
    HWND hwndMag = nullptr;

    HMODULE hMagnification = nullptr;
    bool magReady = false;

    IUIAutomation* uia = nullptr;
    bool uiaReady = false;

    bool comInitedHere = false;

    POINT lastCursor{ -1, -1 };
    DWORD lastUiaQueryTick = 0;

    bool visible = false;
    bool showingText = false;
    std::wstring currentText;

    AppSettings cfg;
};

static RuntimeState g;

static UINT WM_APP_RELOAD_SETTINGS = WM_APP + 1;
static UINT WM_APP_EXIT_THREAD     = WM_APP + 2;

static int ClampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static COLORREF ColorFromRGBInt(int rgb) {
    int r = (rgb >> 16) & 0xFF;
    int g = (rgb >> 8) & 0xFF;
    int b = rgb & 0xFF;
    return RGB(r, g, b);
}

static std::wstring TrimAndCollapse(const std::wstring& in) {
    std::wstring s = in;
    for (auto& ch : s) {
        if (ch == L'\r' || ch == L'\n' || ch == L'\t') ch = L' ';
    }
    while (!s.empty() && s.front() == L' ') s.erase(s.begin());
    while (!s.empty() && s.back() == L' ')  s.pop_back();

    std::wstring out;
    out.reserve(s.size());
    bool prevSpace = false;
    for (wchar_t c : s) {
        bool space = (c == L' ');
        if (space) {
            if (!prevSpace) out.push_back(L' ');
        } else {
            out.push_back(c);
        }
        prevSpace = space;
    }
    return out;
}

static bool IsTriggerDown(TriggerKey k) {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    switch (k) {
        case TriggerKey::None:  return true;
        case TriggerKey::Ctrl:  return down(VK_LCONTROL) || down(VK_RCONTROL);
        case TriggerKey::Alt:   return down(VK_LMENU) || down(VK_RMENU);
        case TriggerKey::Shift: return down(VK_LSHIFT) || down(VK_RSHIFT);
        case TriggerKey::Win:   return down(VK_LWIN) || down(VK_RWIN);
        default: return false;
    }
}

static void LoadSettings() {
    PCWSTR trig = Wh_GetStringSetting(L"triggerKey");
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    PCWSTR unit = Wh_GetStringSetting(L"textUnit");
    PCWSTR fontName = Wh_GetStringSetting(L"fontName");

    if (trig && wcscmp(trig, L"none") == 0) g.cfg.triggerKey = TriggerKey::None;
    else if (trig && wcscmp(trig, L"alt") == 0) g.cfg.triggerKey = TriggerKey::Alt;
    else if (trig && wcscmp(trig, L"shift") == 0) g.cfg.triggerKey = TriggerKey::Shift;
    else if (trig && wcscmp(trig, L"win") == 0) g.cfg.triggerKey = TriggerKey::Win;
    else g.cfg.triggerKey = TriggerKey::Ctrl;

    if (mode && wcscmp(mode, L"text") == 0) g.cfg.mode = Mode::TextOnly;
    else if (mode && wcscmp(mode, L"magnifier") == 0) g.cfg.mode = Mode::MagnifierOnly;
    else g.cfg.mode = Mode::Auto;

    if (unit && wcscmp(unit, L"line") == 0) g.cfg.textUnit = HoverTextUnit::Line;
    else if (unit && wcscmp(unit, L"paragraph") == 0) g.cfg.textUnit = HoverTextUnit::Paragraph;
    else g.cfg.textUnit = HoverTextUnit::Word;

    if (fontName && *fontName) g.cfg.fontName = fontName;

    if (trig) Wh_FreeStringSetting(trig);
    if (mode) Wh_FreeStringSetting(mode);
    if (unit) Wh_FreeStringSetting(unit);
    if (fontName) Wh_FreeStringSetting(fontName);

    g.cfg.hideWhenNoText = Wh_GetIntSetting(L"hideWhenNoText") != 0;
    g.cfg.fallbackToMagnifier = Wh_GetIntSetting(L"fallbackToMagnifier") != 0;

    g.cfg.zoomPercent = ClampInt(Wh_GetIntSetting(L"zoomPercent"), 100, 800);

    g.cfg.bubbleWidth  = ClampInt(Wh_GetIntSetting(L"bubbleWidth"), 200, 2000);
    g.cfg.bubbleHeight = ClampInt(Wh_GetIntSetting(L"bubbleHeight"), 80, 1200);
    g.cfg.offsetX      = ClampInt(Wh_GetIntSetting(L"offsetX"), -800, 800);
    g.cfg.offsetY      = ClampInt(Wh_GetIntSetting(L"offsetY"), -800, 800);

    g.cfg.cornerRadius  = ClampInt(Wh_GetIntSetting(L"cornerRadius"), 0, 80);
    g.cfg.borderWidth   = ClampInt(Wh_GetIntSetting(L"borderWidth"), 0, 16);
    g.cfg.textPointSize = ClampInt(Wh_GetIntSetting(L"textPointSize"), 10, 120);
    g.cfg.fontWeight    = ClampInt(Wh_GetIntSetting(L"fontWeight"), 100, 900);
    g.cfg.textColor     = ClampInt(Wh_GetIntSetting(L"textColor"), 0x000000, 0xFFFFFF);
    g.cfg.backgroundColor = ClampInt(Wh_GetIntSetting(L"backgroundColor"), 0x000000, 0xFFFFFF);
    g.cfg.borderColor   = ClampInt(Wh_GetIntSetting(L"borderColor"), 0x000000, 0xFFFFFF);
    g.cfg.padding       = ClampInt(Wh_GetIntSetting(L"padding"), 4, 120);

    g.cfg.updateIntervalMs      = ClampInt(Wh_GetIntSetting(L"updateIntervalMs"), 8, 100);
    g.cfg.uiaQueryMinIntervalMs = ClampInt(Wh_GetIntSetting(L"uiaQueryMinIntervalMs"), 20, 500);
    g.cfg.maxTextLen            = ClampInt(Wh_GetIntSetting(L"maxTextLen"), 40, 2000);

    g.cfg.opacity = ClampInt(Wh_GetIntSetting(L"opacity"), 40, 255);
}

static bool InitMagnification() {
    g.hMagnification = LoadLibraryW(L"Magnification.dll");
    if (!g.hMagnification) return false;

    pfnMagInit = (PFNMAGINIT)GetProcAddress(g.hMagnification, "MagInitialize");
    pfnMagUninit = (PFNMAGUNINIT)GetProcAddress(g.hMagnification, "MagUninitialize");
    pfnMagSetWindowSource = (PFNMAGSETWINDOWSOURCE)GetProcAddress(g.hMagnification, "MagSetWindowSource");
    pfnMagSetWindowTransform = (PFNMAGSETWINDOWTRANSFORM)GetProcAddress(g.hMagnification, "MagSetWindowTransform");
    pfnMagSetWindowFilterList = (PFNMAGSETWINDOWFILTERLIST)GetProcAddress(g.hMagnification, "MagSetWindowFilterList");

    if (!pfnMagInit || !pfnMagUninit || !pfnMagSetWindowSource || !pfnMagSetWindowTransform)
        return false;

    if (!pfnMagInit())
        return false;

    g.magReady = true;
    return true;
}

static void UninitMagnification() {
    if (g.magReady && pfnMagUninit) pfnMagUninit();
    g.magReady = false;

    if (g.hMagnification) {
        FreeLibrary(g.hMagnification);
        g.hMagnification = nullptr;
    }
}

static bool InitUIA() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        g.comInitedHere = true;
    } else if (hr == RPC_E_CHANGED_MODE) {
        g.comInitedHere = false;
    } else {
        return false;
    }

    hr = CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g.uia));
    if (FAILED(hr) || !g.uia) {
        g.uiaReady = false;
        return false;
    }

    g.uiaReady = true;
    return true;
}

static void UninitUIA() {
    if (g.uia) {
        g.uia->Release();
        g.uia = nullptr;
    }
    g.uiaReady = false;

    if (g.comInitedHere) {
        CoUninitialize();
        g.comInitedHere = false;
    }
}

static bool TryExtractTextAtPoint(POINT pt, std::wstring& outText) {
    outText.clear();
    if (!g.uiaReady || !g.uia) return false;

    IUIAutomationElement* el = nullptr;
    HRESULT hr = g.uia->ElementFromPoint(pt, &el);
    if (FAILED(hr) || !el) return false;

    auto pickUnit = []() -> TextUnit {
        switch (g.cfg.textUnit) {
            case HoverTextUnit::Line:      return TextUnit_Line;
            case HoverTextUnit::Paragraph: return TextUnit_Paragraph;
            default:                       return TextUnit_Word;
        }
    };

    IUIAutomationTextPattern2* tp2 = nullptr;
    hr = el->GetCurrentPatternAs(UIA_TextPattern2Id, IID_PPV_ARGS(&tp2));
    if (SUCCEEDED(hr) && tp2) {
        IUIAutomationTextRange* range = nullptr;
        hr = tp2->RangeFromPoint(pt, &range);
        if (SUCCEEDED(hr) && range) {
            range->ExpandToEnclosingUnit(pickUnit());
            BSTR b = nullptr;
            range->GetText(g.cfg.maxTextLen, &b);
            if (b) { outText.assign(b, SysStringLen(b)); SysFreeString(b); }
            range->Release();
        }
        tp2->Release();
        el->Release();
        outText = TrimAndCollapse(outText);
        return !outText.empty();
    }

    IUIAutomationTextPattern* tp = nullptr;
    hr = el->GetCurrentPatternAs(UIA_TextPatternId, IID_PPV_ARGS(&tp));
    if (SUCCEEDED(hr) && tp) {
        IUIAutomationTextRange* range = nullptr;
        hr = tp->RangeFromPoint(pt, &range);
        if (SUCCEEDED(hr) && range) {
            range->ExpandToEnclosingUnit(pickUnit());
            BSTR b = nullptr;
            range->GetText(g.cfg.maxTextLen, &b);
            if (b) { outText.assign(b, SysStringLen(b)); SysFreeString(b); }
            range->Release();
        }
        tp->Release();
        el->Release();
        outText = TrimAndCollapse(outText);
        return !outText.empty();
    }

    IUIAutomationValuePattern* vp = nullptr;
    hr = el->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&vp));
    if (SUCCEEDED(hr) && vp) {
        BSTR b = nullptr;
        vp->get_CurrentValue(&b);
        if (b) { outText.assign(b, SysStringLen(b)); SysFreeString(b); }
        vp->Release();
        el->Release();
        outText = TrimAndCollapse(outText);
        if ((int)outText.size() > g.cfg.maxTextLen) outText.resize(g.cfg.maxTextLen);
        return !outText.empty();
    }

    BSTR name = nullptr;
    el->get_CurrentName(&name);
    if (name) { outText.assign(name, SysStringLen(name)); SysFreeString(name); }
    el->Release();
    outText = TrimAndCollapse(outText);
    if ((int)outText.size() > g.cfg.maxTextLen) outText.resize(g.cfg.maxTextLen);
    return !outText.empty();
}

static RECT GetWorkAreaForPoint(POINT pt) {
    RECT work{};
    HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (hm && GetMonitorInfo(hm, &mi)) {
        work = mi.rcWork;
    } else {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &work, 0);
    }
    return work;
}

static void ApplyRoundedRegion(HWND hwnd, int w, int h, int radius) {
    if (radius <= 0) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }
    HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
    SetWindowRgn(hwnd, rgn, TRUE);
}

static void EnsureVisibility(bool show) {
    if (!g.hwndHost) return;
    if (show && !g.visible) {
        ShowWindow(g.hwndHost, SW_SHOWNOACTIVATE);
        g.visible = true;
    } else if (!show && g.visible) {
        ShowWindow(g.hwndHost, SW_HIDE);
        g.visible = false;
    }
}

static void PositionBubbleNearCursor(HWND hwnd, POINT pt, int w, int h) {
    RECT work = GetWorkAreaForPoint(pt);
    int workL = (int)work.left, workT = (int)work.top, workR = (int)work.right, workB = (int)work.bottom;

    int x = pt.x + g.cfg.offsetX;
    int y = pt.y + g.cfg.offsetY;

    if (x + w > workR) x = pt.x - g.cfg.offsetX - w;
    if (y + h > workB) y = pt.y - g.cfg.offsetY - h;

    x = std::max(workL, std::min(x, workR - w));
    y = std::max(workT, std::min(y, workB - h));

    // Use SWP_NOREDRAW to prevent flicker, we'll invalidate manually
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
}

static void UpdateMagnifierSource(HWND hwndMag, POINT pt, int w, int h) {
    if (!g.magReady || !hwndMag) return;

    float zoom = (float)g.cfg.zoomPercent / 100.0f;
    if (zoom < 1.0f) zoom = 1.0f;

    int srcW = (int)std::round((float)w / zoom);
    int srcH = (int)std::round((float)h / zoom);

    RECT work = GetWorkAreaForPoint(pt);
    LONG workL = work.left, workT = work.top, workR = work.right, workB = work.bottom;

    MAGRECTANGLE src = {
        (LONG)(pt.x - srcW / 2),
        (LONG)(pt.y - srcH / 2),
        (LONG)(pt.x + srcW / 2),
        (LONG)(pt.y + srcH / 2)
    };

    src.left   = std::max(workL, src.left);
    src.top    = std::max(workT, src.top);
    src.right  = std::min(workR, src.right);
    src.bottom = std::min(workB, src.bottom);

    pfnMagSetWindowSource(hwndMag, src);

    float m[9] = { zoom,0,0, 0,zoom,0, 0,0,1 };
    pfnMagSetWindowTransform(hwndMag, m);

    InvalidateRect(hwndMag, nullptr, FALSE);
    UpdateWindow(hwndMag);
}

static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT; // click-through
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            HBRUSH bg = CreateSolidBrush(ColorFromRGBInt(g.cfg.backgroundColor));
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);

            if (g.cfg.borderWidth > 0) {
                HPEN pen = CreatePen(PS_SOLID, g.cfg.borderWidth, ColorFromRGBInt(g.cfg.borderColor));
                HGDIOBJ oldPen = SelectObject(hdc, pen);
                HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

                int inset = g.cfg.borderWidth / 2;
                RoundRect(
                    hdc,
                    inset, inset,
                    (rc.right - rc.left) - inset,
                    (rc.bottom - rc.top) - inset,
                    g.cfg.cornerRadius * 2,
                    g.cfg.cornerRadius * 2
                );

                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(pen);
            }

            if (g.showingText) {
                LOGFONTW lf{};
                lf.lfHeight = -MulDiv(g.cfg.textPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
                lf.lfWeight = g.cfg.fontWeight;
                lstrcpynW(lf.lfFaceName, g.cfg.fontName.c_str(), LF_FACESIZE);

                HFONT font = CreateFontIndirectW(&lf);
                HGDIOBJ oldFont = SelectObject(hdc, font);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, ColorFromRGBInt(g.cfg.textColor));

                RECT tr = rc;
                tr.left   += g.cfg.padding;
                tr.top    += g.cfg.padding;
                tr.right  -= g.cfg.padding;
                tr.bottom -= g.cfg.padding;

                DrawTextW(hdc, g.currentText.c_str(), -1, &tr,
                          DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX);

                SelectObject(hdc, oldFont);
                DeleteObject(font);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool CreateWindows() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kHostClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassExW(&wc);

    DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    DWORD style = WS_POPUP;

    g.hwndHost = CreateWindowExW(
        ex, kHostClassName, L"HoverTextHost",
        style,
        0, 0, 10, 10,
        nullptr, nullptr, wc.hInstance, nullptr
    );
    if (!g.hwndHost) return false;

    SetLayeredWindowAttributes(g.hwndHost, 0, (BYTE)g.cfg.opacity, LWA_ALPHA);

    g.hwndMag = CreateWindowExW(
        WS_EX_TRANSPARENT,
        L"Magnifier",
        L"HoverTextMagnifier",
        WS_CHILD,
        0, 0, 10, 10,
        g.hwndHost,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if (g.hwndMag) {
        SetWindowTheme(g.hwndMag, L"", L"");
        ShowWindow(g.hwndMag, SW_HIDE);
    }

    ShowWindow(g.hwndHost, SW_HIDE);
    g.visible = false;
    return true;
}

static void DestroyWindows() {
    if (g.hwndMag) {
        DestroyWindow(g.hwndMag);
        g.hwndMag = nullptr;
    }
    if (g.hwndHost) {
        DestroyWindow(g.hwndHost);
        g.hwndHost = nullptr;
    }
    UnregisterClassW(kHostClassName, GetModuleHandleW(nullptr));
}

static void TickUpdate() {
    if (!g.hwndHost) return;

    if (!IsTriggerDown(g.cfg.triggerKey)) {
        EnsureVisibility(false);
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        EnsureVisibility(false);
        return;
    }

    int w = std::max(1, g.cfg.bubbleWidth);
    int h = std::max(1, g.cfg.bubbleHeight);

    PositionBubbleNearCursor(g.hwndHost, pt, w, h);
    ApplyRoundedRegion(g.hwndHost, w, h, g.cfg.cornerRadius);
    EnsureVisibility(true);

    const bool wantText = (g.cfg.mode == Mode::TextOnly || g.cfg.mode == Mode::Auto);
    const bool wantMag  = (g.cfg.mode == Mode::MagnifierOnly || g.cfg.mode == Mode::Auto);

    bool haveText = false;
    std::wstring text;

    DWORD nowTick = GetTickCount();

    if (wantText && g.uiaReady) {
        if (nowTick - g.lastUiaQueryTick >= (DWORD)g.cfg.uiaQueryMinIntervalMs || pt.x != g.lastCursor.x || pt.y != g.lastCursor.y) {
            g.lastUiaQueryTick = nowTick;
            haveText = TryExtractTextAtPoint(pt, text);
        } else {
            haveText = g.showingText && !g.currentText.empty();
            text = g.currentText;
        }
    }

    g.lastCursor = pt;

    bool showText = false;
    bool showMag = false;

    if (g.cfg.mode == Mode::TextOnly) {
        showText = haveText;
        if (!haveText && !g.cfg.hideWhenNoText) {
            text = L"(no text detected)";
            showText = true;
        }
    } else if (g.cfg.mode == Mode::MagnifierOnly) {
        showMag = (g.magReady && g.hwndMag != nullptr);
    } else { // Auto
        if (haveText) {
            showText = true;
        } else if (g.cfg.fallbackToMagnifier && wantMag && g.magReady && g.hwndMag) {
            showMag = true;
        } else {
            if (g.cfg.hideWhenNoText) {
                EnsureVisibility(false);
                return;
            }
            text = L"(no text detected)";
            showText = true;
        }
    }

    g.showingText = showText;

    if (showText) {
        g.currentText = text;
        if (g.hwndMag) ShowWindow(g.hwndMag, SW_HIDE);
        InvalidateRect(g.hwndHost, nullptr, TRUE);
        UpdateWindow(g.hwndHost);
    } else if (showMag) {
        ShowWindow(g.hwndMag, SW_SHOWNA);
        SetWindowPos(g.hwndMag, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

        if (pfnMagSetWindowFilterList) {
            HWND exclude[] = { g.hwndHost, g.hwndMag };
            pfnMagSetWindowFilterList(g.hwndMag, MW_FILTERMODE_EXCLUDE, 2, exclude);
        }

        UpdateMagnifierSource(g.hwndMag, pt, w, h);
        InvalidateRect(g.hwndHost, nullptr, TRUE);
        UpdateWindow(g.hwndHost);
    } else {
        EnsureVisibility(false);
    }
}

static void WorkerThread() {
    g.threadId = GetCurrentThreadId();
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE); // ensure queue exists

    LoadSettings();
    InitUIA();           // best-effort
    InitMagnification(); // best-effort

    if (!CreateWindows()) {
        g.running = false;
        UninitMagnification();
        UninitUIA();
        return;
    }

    g.running = true;

    while (g.running) {
        MsgWaitForMultipleObjectsEx(
            0, nullptr,
            (DWORD)g.cfg.updateIntervalMs,
            QS_ALLINPUT,
            MWMO_INPUTAVAILABLE
        );

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT || msg.message == WM_APP_EXIT_THREAD) {
                g.running = false;
                break;
            }
            if (msg.message == WM_APP_RELOAD_SETTINGS) {
                LoadSettings();
                if (g.hwndHost) {
                    SetLayeredWindowAttributes(g.hwndHost, 0, (BYTE)g.cfg.opacity, LWA_ALPHA);
                }
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!g.running) break;
        TickUpdate();
    }

    EnsureVisibility(false);
    DestroyWindows();

    UninitMagnification();
    UninitUIA();
}

BOOL Wh_ModInit() {
    g.running = true;
    g.worker = std::thread(WorkerThread);
    return TRUE;
}

void Wh_ModUninit() {
    if (g.threadId != 0) {
        PostThreadMessage(g.threadId, WM_APP_EXIT_THREAD, 0, 0);
        PostThreadMessage(g.threadId, WM_QUIT, 0, 0);
    }
    g.running = false;
    if (g.worker.joinable()) g.worker.join();
}

void Wh_ModSettingsChanged() {
    if (g.threadId != 0) {
        PostThreadMessage(g.threadId, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}
