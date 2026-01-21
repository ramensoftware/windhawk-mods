// ==WindhawkMod==
// @id              hover-text-magnifier
// @name            Hover Text Magnifier (macOS-style)
// @description     On-cursor hover bubble with large text via UI Automation; optional pixel magnifier fallback.
// @version         1.2.0
// @author          Math Shamenson
// @github          https://github.com/insane66613
// @license         MIT
// @architecture    x86
// @architecture    x86-64
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luxtheme -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
**Hover Text Magnifier** is a powerful accessibility tool designed to make reading small text effortless, especially on large displays, TVs, or high-DPI monitors where you might sit at a distance.

Inspired by the macOS feature, it displays a high-contrast, large-text bubble for whatever is under your mouse cursor. Unlike standard magnifiers that zoom the entire screen and require constant panning, this tool intelligently extracts the *text itself* and renders it clearly in a dedicated overlay.

## Why use this?
*   **Distance Reading:** Perfect for using a TV as a monitor from the couch. Read file names, menu items, and terminal commands without squinting or leaning forward.
*   **Focus, Don't Zoom:** Magnifying the whole screen can be disorienting. This keeps your context visible while making the specific target readable.
*   **Reclaim Caps Lock:** Make the most underutilized key on your keyboard useful again! Use Caps Lock as a toggle switch to turn text magnification on/off instantly—turning wasted keyboard real estate into a productivity tool.

## Features
*   **Smart Text Extraction:** Uses UI Automation to grab text from buttons, menus, and documents.
*   **Seamless Fallback:** If text isn't available (like in an image), it automatically switches to a pixel magnifier.
*   **Dynamic Sizing:** The bubble expands to fit single words or full paragraphs.
*   **Customizable:** Build for your eyes—choose from High Contrast, Dark, Sepia, or Light themes. Customizable fonts and sizes.
*   **Trigger Modes:** Hold Ctrl (default) or toggle with Caps Lock (blocks original Caps Lock function when active).

## Usage
*   **Default:** Hold **Ctrl** and hover over any text to see it magnified.
*   **Toggle:** Go to settings and set Trigger Key to **CapsLock**. Tap CapsLock to toggle the magnifier on/off permanently.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: auto
  $name: Mode
  $description: auto=text first (UIA) then optional magnifier fallback.
  $options:
    - auto: Auto (Text then Magnifier fallback)
    - text: Text only
    - magnifier: Magnifier only

- textUnit: paragraph
  $name: Text capture mode
  $options:
    - word: Word (Single word)
    - line: Line (Single line)
    - paragraph: Paragraph (Game changing for reading!)

- triggerKey: ctrl
  $name: Trigger key
  $description: Hold key to show (or toggle with CapsLock).
  $options:
    - none: Always on
    - ctrl: Hold Ctrl
    - alt: Hold Alt
    - shift: Hold Shift
    - win: Hold Win
    - capslock: Toggle CapsLock (Prevents default CapsLock behavior)

- colorTheme: dark
  $name: Color Theme
  $options:
    - dark: Dark Mode (Dark Gray BG, White Text)
    - light: Light Mode (White BG, Black Text)
    - high_contrast_black: High Contrast Black
    - high_contrast_white: High Contrast White
    - sepia: Sepia / Soft Reading
    - custom: Custom (Use Advanced Settings)

- bubbleSize: medium
  $name: Bubble Size (Max Dimensions)
  $options:
    - small: Small (Compact)
    - medium: Medium (Standard)
    - large: Large (Long paragraphs)
    - huge: Huge (Full screen width)

- fontStyle: modern
  $name: Font Style
  $options:
    - modern: Modern (Segoe UI)
    - classic: Classic (Arial/Tahoma)
    - serif: Serif (Georgia)
    - monospace: Code (Consolas)

- fontSizePreset: medium
  $name: Text Size
  $options:
    - small: Small
    - medium: Medium
    - large: Large
    - extra_large: Extra Large

- zoomLevel: 250
  $name: Magnifier Zoom Level
  $options:
    - 150: 150%
    - 200: 200%
    - 250: 250%
    - 300: 300%
    - 350: 350%
    - 400: 400%
    - 500: 500%

- advancedSettings: false
  $name: Show Advanced / Custom Settings
  $description: Enable this to customize individual colors and fine-tune dimensions if 'Custom' theme is selected.

- _sep1: ""
  $name: "--- Advanced: Custom Appearance ---"

- customFontName: Segoe UI
  $name: Custom Font Name
- customTextColor: 0xF5F5F5
  $name: Custom Text Color (0xRRGGBB)
- customBackgroundColor: 0x141414
  $name: Custom Background Color (0xRRGGBB)
- customBorderColor: 0x5A5A5A
  $name: Custom Border Color (0xRRGGBB)
- customTextSize: 26
  $name: Custom Text Size (pt)

- _sep2: ""
  $name: "--- Advanced: Behavior ---"

- hideWhenNoText: false
  $name: Hide bubble when no text is found
- fallbackToMagnifier: true
  $name: Fallback to magnifier
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <commctrl.h>   // ensures HIMAGELIST exists for uxtheme.h
#include <uxtheme.h>
#include <windhawk_api.h>

#include <UIAutomation.h>
#include <objbase.h>
#include <oleauto.h>

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
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

enum class TriggerKey { None, Ctrl, Alt, Shift, Win, CapsLock };
enum class Mode { Auto, TextOnly, MagnifierOnly };
enum class HoverTextUnit { Word, Line, Paragraph }; // avoid UIA TextUnit name collision
enum class TextAlign { Left, Center, Right };

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
    TextAlign textAlign = TextAlign::Left;
    int maxLines = 4;
    bool textShadow = true;
    int shadowOffsetX = 1;
    int shadowOffsetY = 1;
    int shadowColor = 0x000000;
    int outlineWidth = 0;
    int outlineColor = 0x000000;
    int backgroundColor = 0x141414;
    int borderColor = 0x5A5A5A;
    int padding = 18;

    int updateIntervalMs = 16;
    int uiaQueryMinIntervalMs = 60;
    int maxTextLen = 220;

    int opacity = 245;
    int autoHideDelayMs = 0;
};

struct RuntimeState {
    std::atomic<bool> running{false};
    std::thread worker;
    std::thread uiaThread;
    
    // Shared state between Worker and UIA Thread
    std::mutex uiaMutex;
    std::condition_variable uiaCv;
    bool uiaThreadShouldExit = false;
    bool uiaWorkAvailable = false;
    POINT uiaTargetPt = {0,0};
    std::wstring uiaResultText;
    bool uiaResultValid = false;

    DWORD threadId = 0;

    HHOOK hKeyboardHook = nullptr;
    std::atomic<bool> capsLockToggleState{false};

    HWND hwndHost = nullptr;
    HWND hwndMag = nullptr;

    HMODULE hMagnification = nullptr;
    bool magReady = false;

    // uia is now owned by uiaThread, do not access from worker thread!
    IUIAutomation* uia = nullptr;
    bool uiaReady = false;
    bool comInitedHere = false;

    POINT lastCursor{ -1, -1 };
    DWORD lastUiaQueryTick = 0;

    bool visible = false;
    bool showingText = false;
    bool showingMag = false;
    std::wstring currentText;

    DWORD lastTriggerUpTick = 0;
    bool triggerWasDown = false;

    // Debounce for text loss to prevent flicker
    DWORD lastValidTextTick = 0;

    float dpiScale = 1.0f;
    int effectiveBubbleWidth = 520;
    int effectiveBubbleHeight = 160;
    int effectiveOffsetX = 24;
    int effectiveOffsetY = 24;
    int effectiveCornerRadius = 16;
    int effectiveBorderWidth = 1;
    int effectivePadding = 18;

    // Last committed window state
    RECT lastWindowRect = { 0, 0, 0, 0 };
    // Cached Region state to prevent flicker
    int lastRgnW = 0;
    int lastRgnH = 0;
    int lastRgnRadius = -1;

    // Cached Layout
    std::wstring cachedFittedText;
    std::wstring lastFittedTextSource;
    int lastFittedWidth = 0;
    int lastFittedHeight = 0;

    // Cached GDI objects
    HFONT hFont = nullptr;
    HBRUSH hBgBrush = nullptr;
    HPEN hBorderPen = nullptr;

    AppSettings cfg;
};

static RuntimeState g;

static void UiaWorkerThread();

static UINT WM_APP_RELOAD_SETTINGS = WM_APP + 1;
static UINT WM_APP_EXIT_THREAD     = WM_APP + 2;

static int ClampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int RoundToInt(float v) {
    return static_cast<int>(std::round(v));
}

static UINT GetTextAlignFlags() {
    switch (g.cfg.textAlign) {
        case TextAlign::Center: return DT_CENTER;
        case TextAlign::Right:  return DT_RIGHT;
        default:                return DT_LEFT;
    }
}

static std::wstring FitTextToHeight(HDC hdc, const std::wstring& text, int maxWidth, int maxHeight, UINT flags) {
    if (text.empty()) return text;

    RECT rc{ 0, 0, maxWidth, maxHeight };
    RECT calc = rc;
    DrawTextW(hdc, text.c_str(), (int)text.size(), &calc, flags | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
    if (calc.bottom <= maxHeight) return text;

    std::wstring ell = L"…";
    int lo = 0;
    int hi = (int)text.size();
    std::wstring best = ell;

    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        std::wstring candidate = text.substr(0, mid);
        candidate += ell;

        calc = rc;
        DrawTextW(hdc, candidate.c_str(), (int)candidate.size(), &calc, flags | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);

        if (calc.bottom <= maxHeight) {
            best = candidate;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return best;
}

static COLORREF ColorFromRGBInt(int rgb) {

    int r = (rgb >> 16) & 0xFF;
    int g = (rgb >> 8) & 0xFF;
    int b = rgb & 0xFF;
    return RGB(r, g, b);
}

static void FreeGraphicsResources() {
    if (g.hFont) { DeleteObject(g.hFont); g.hFont = nullptr; }
    if (g.hBgBrush) { DeleteObject(g.hBgBrush); g.hBgBrush = nullptr; }
    if (g.hBorderPen) { DeleteObject(g.hBorderPen); g.hBorderPen = nullptr; }
}

static void UpdateGraphicsResources() {
    FreeGraphicsResources();
    g.lastFittedTextSource.clear();

    LOGFONTW lf{};
    lf.lfHeight = -RoundToInt(g.cfg.textPointSize * g.dpiScale * 96.0f / 72.0f); // Approximate point-to-pixel
    lf.lfWeight = g.cfg.fontWeight;
    lstrcpynW(lf.lfFaceName, g.cfg.fontName.c_str(), LF_FACESIZE);
    lf.lfQuality = CLEARTYPE_QUALITY;
    g.hFont = CreateFontIndirectW(&lf);

    g.hBgBrush = CreateSolidBrush(ColorFromRGBInt(g.cfg.backgroundColor));

    if (g.effectiveBorderWidth > 0) {
        g.hBorderPen = CreatePen(PS_SOLID, g.effectiveBorderWidth, ColorFromRGBInt(g.cfg.borderColor));
    }
}


static float GetDpiScaleForPoint(POINT pt) {
    typedef HRESULT(WINAPI* GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE hShcore = GetModuleHandleW(L"Shcore.dll");
        if (!hShcore) hShcore = LoadLibraryW(L"Shcore.dll");
        if (!hShcore) return nullptr;
        return (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
    }();

    const int MDT_EFFECTIVE_DPI = 0;
    HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (pGetDpiForMonitor && hm) {
        UINT dpiX = 0, dpiY = 0;
        if (SUCCEEDED(pGetDpiForMonitor(hm, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX > 0) {
            return (float)dpiX / 96.0f;
        }
    }

    HDC hdc = GetDC(nullptr);
    int dpi = hdc ? GetDeviceCaps(hdc, LOGPIXELSX) : 96;
    if (hdc) ReleaseDC(nullptr, hdc);
    if (dpi <= 0) dpi = 96;
    return (float)dpi / 96.0f;
}

static void UpdateEffectiveSizing(POINT pt) {
    float newScale = GetDpiScaleForPoint(pt);
    bool scaleChanged = (std::abs(newScale - g.dpiScale) > 0.001f);
    g.dpiScale = newScale;

    g.effectiveBubbleWidth = std::max(1, RoundToInt(g.cfg.bubbleWidth * g.dpiScale));
    g.effectiveBubbleHeight = std::max(1, RoundToInt(g.cfg.bubbleHeight * g.dpiScale));
    g.effectiveOffsetX = RoundToInt(g.cfg.offsetX * g.dpiScale);
    g.effectiveOffsetY = RoundToInt(g.cfg.offsetY * g.dpiScale);
    g.effectiveCornerRadius = std::max(0, RoundToInt(g.cfg.cornerRadius * g.dpiScale));
    g.effectiveBorderWidth = std::max(0, RoundToInt(g.cfg.borderWidth * g.dpiScale));
    g.effectivePadding = std::max(1, RoundToInt(g.cfg.padding * g.dpiScale));

    // If scale changed or resources are missing (first run), recreate them
    if (scaleChanged || !g.hFont) {
        UpdateGraphicsResources();
    }
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

static void EnsureCapsLockOff() {
    if ((GetKeyState(VK_CAPITAL) & 0x1) == 0) return;

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CAPITAL;

    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

static bool IsTriggerDown(TriggerKey k) {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    switch (k) {
        case TriggerKey::None:     return true;
        case TriggerKey::Ctrl:     return down(VK_LCONTROL) || down(VK_RCONTROL);
        case TriggerKey::Alt:      return down(VK_LMENU) || down(VK_RMENU);
        case TriggerKey::Shift:    return down(VK_LSHIFT) || down(VK_RSHIFT);
        case TriggerKey::Win:      return down(VK_LWIN) || down(VK_RWIN);
        case TriggerKey::CapsLock: return g.capsLockToggleState;
        default: return false;
    }
}

struct StringSetting {
    PCWSTR value;
    StringSetting(PCWSTR name) : value(Wh_GetStringSetting(name)) {}
    ~StringSetting() { if (value) Wh_FreeStringSetting(value); }
    operator PCWSTR() const { return value; }
};

static void LoadSettings() {
    StringSetting trig(L"triggerKey");
    StringSetting mode(L"mode");
    StringSetting unit(L"textUnit");
    StringSetting fontName(L"fontName");

    if (trig && wcscmp(trig, L"none") == 0) g.cfg.triggerKey = TriggerKey::None;
    else if (trig && wcscmp(trig, L"alt") == 0) g.cfg.triggerKey = TriggerKey::Alt;
    else if (trig && wcscmp(trig, L"shift") == 0) g.cfg.triggerKey = TriggerKey::Shift;
    else if (trig && wcscmp(trig, L"win") == 0) g.cfg.triggerKey = TriggerKey::Win;
    else if (trig && wcscmp(trig, L"capslock") == 0) g.cfg.triggerKey = TriggerKey::CapsLock;
    else g.cfg.triggerKey = TriggerKey::Ctrl;

    if (mode && wcscmp(mode, L"text") == 0) g.cfg.mode = Mode::TextOnly;
    else if (mode && wcscmp(mode, L"magnifier") == 0) g.cfg.mode = Mode::MagnifierOnly;
    else g.cfg.mode = Mode::Auto;

    if (unit && wcscmp(unit, L"line") == 0) g.cfg.textUnit = HoverTextUnit::Line;
    else if (unit && wcscmp(unit, L"paragraph") == 0) g.cfg.textUnit = HoverTextUnit::Paragraph;
    else g.cfg.textUnit = HoverTextUnit::Word;

    // --- Font Style Presets ---
    StringSetting fStyle(L"fontStyle");
    if (fStyle && wcscmp(fStyle, L"monospace") == 0) g.cfg.fontName = L"Consolas";
    else if (fStyle && wcscmp(fStyle, L"serif") == 0) g.cfg.fontName = L"Georgia";
    else if (fStyle && wcscmp(fStyle, L"classic") == 0) g.cfg.fontName = L"Arial";
    else g.cfg.fontName = L"Segoe UI"; // modern/default

    // --- Size Preset ---
    StringSetting fSize(L"fontSizePreset");
    int sizePt = 26;
    if (fSize && wcscmp(fSize, L"small") == 0) sizePt = 18;
    else if (fSize && wcscmp(fSize, L"large") == 0) sizePt = 32;
    else if (fSize && wcscmp(fSize, L"extra_large") == 0) sizePt = 42;
    else sizePt = 24; // medium
    g.cfg.textPointSize = sizePt;

    // --- Bubble Size Preset ---
    StringSetting bSize(L"bubbleSize");
    if (bSize && wcscmp(bSize, L"small") == 0) {
        g.cfg.bubbleWidth = 300; g.cfg.bubbleHeight = 160; g.cfg.maxLines = 10; g.cfg.maxTextLen = 500;
    } else if (bSize && wcscmp(bSize, L"large") == 0) {
        g.cfg.bubbleWidth = 800; g.cfg.bubbleHeight = 400; g.cfg.maxLines = 40; g.cfg.maxTextLen = 2000;
    } else if (bSize && wcscmp(bSize, L"huge") == 0) {
        g.cfg.bubbleWidth = 1400; g.cfg.bubbleHeight = 800; g.cfg.maxLines = 80; g.cfg.maxTextLen = 4000;
    } else { // medium
        g.cfg.bubbleWidth = 500; g.cfg.bubbleHeight = 250; g.cfg.maxLines = 20; g.cfg.maxTextLen = 1000;
    }

    // --- Zoom Preset ---
    g.cfg.zoomPercent = ClampInt(Wh_GetIntSetting(L"zoomLevel"), 150, 500);
    if (g.cfg.zoomPercent == 0) g.cfg.zoomPercent = 250;

    // --- Color Theme Presets ---
    StringSetting theme(L"colorTheme");
    
    // Default Dark
    g.cfg.backgroundColor = 0x141414;
    g.cfg.textColor = 0xF5F5F5;
    g.cfg.borderColor = 0x5A5A5A;
    g.cfg.shadowColor = 0x000000;
    
    if (theme) {
        if (wcscmp(theme, L"light") == 0) {
            g.cfg.backgroundColor = 0xFFFFFF;
            g.cfg.textColor = 0x000000;
            g.cfg.borderColor = 0xCCCCCC;
            g.cfg.shadowColor = 0xAAAAAA;
        } else if (wcscmp(theme, L"high_contrast_black") == 0) {
            g.cfg.backgroundColor = 0x000000;
            g.cfg.textColor = 0xFFFFFF;
            g.cfg.borderColor = 0xFFFFFF;
            g.cfg.borderWidth = 2;
            g.cfg.textShadow = false;
        } else if (wcscmp(theme, L"high_contrast_white") == 0) {
            g.cfg.backgroundColor = 0xFFFFFF;
            g.cfg.textColor = 0x000000;
            g.cfg.borderColor = 0x000000;
            g.cfg.borderWidth = 2;
            g.cfg.textShadow = false;
        } else if (wcscmp(theme, L"sepia") == 0) {
            g.cfg.backgroundColor = 0xF4ECD8;
            g.cfg.textColor = 0x5F4B32;
            g.cfg.borderColor = 0xD4C4A8;
            g.cfg.shadowColor = 0xD4C4A8;
        } else if (wcscmp(theme, L"custom") == 0) {
            // Read advanced custom settings overrides
             PCWSTR cFont = Wh_GetStringSetting(L"customFontName");
             if (cFont && *cFont) g.cfg.fontName = cFont;
             Wh_FreeStringSetting(cFont);

             int cSize = Wh_GetIntSetting(L"customTextSize");
             if (cSize > 0) g.cfg.textPointSize = ClampInt(cSize, 5, 200);

             g.cfg.textColor = Wh_GetIntSetting(L"customTextColor");
             g.cfg.backgroundColor = Wh_GetIntSetting(L"customBackgroundColor");
             g.cfg.borderColor = Wh_GetIntSetting(L"customBorderColor");
        }
    }

    g.cfg.hideWhenNoText = Wh_GetIntSetting(L"hideWhenNoText") != 0;
    g.cfg.fallbackToMagnifier = Wh_GetIntSetting(L"fallbackToMagnifier") != 0;

    g.cfg.opacity = 245; // Fixed for simplicity
    g.cfg.cornerRadius = 16;
    g.cfg.padding = 18;
    g.cfg.offsetX = 24; g.cfg.offsetY = 24;

    g.cfg.updateIntervalMs      = 16; // Fixed 60fps
    g.cfg.uiaQueryMinIntervalMs = 60; // Fixed sensible default

    /*
    Wh_Log(L"Settings Loaded: Trigger=%d, Mode=%d, Theme=%s, Zoom=%d", 
        (int)g.cfg.triggerKey, (int)g.cfg.mode, theme ? theme.value : L"default", g.cfg.zoomPercent);
    */
    // Refresh graphics resources since settings (colors/fonts) changed
    // We need a valid DPI scale; if not set yet, guess 1.0 or wait until first paint.
    // Ideally we update them on TickUpdate/sizing, but fonts/colors only change here.
    // Re-creating them in UpdateEffectiveSizing would be too frequent.
    // Let's assume standard DPI for initial load, or rely on UpdateEffectiveSizing calling us?
    // Actually, UpdateEffectiveSizing changes sizes, which affects font size.
    // So we should just flag them dirty or update them when sizing changes.
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (g.cfg.triggerKey == TriggerKey::CapsLock && p->vkCode == VK_CAPITAL) {
            if (wParam == WM_KEYDOWN) {
                g.capsLockToggleState = !g.capsLockToggleState;
                // Log toggle state for debugging
                // Wh_Log(L"CapsLock Toggled: %s", g.capsLockToggleState ? L"ON" : L"OFF");
            }
            return 1; // Block the key
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static void InstallHooks() {
    if (!g.hKeyboardHook) {
        g.hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleW(nullptr), 0);
        if (!g.hKeyboardHook) {
            Wh_Log(L"Failed to install keyboard hook. CapsLock toggle will not work.");
        }
    }
}

static void UninstallHooks() {
    if (g.hKeyboardHook) {
        UnhookWindowsHookEx(g.hKeyboardHook);
        g.hKeyboardHook = nullptr;
    }
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

    hr = CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&g.uia);
    if (FAILED(hr) || !g.uia) {
        g.uiaReady = false;
        if (g.comInitedHere) {
            CoUninitialize();
            g.comInitedHere = false;
        }
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
    hr = el->GetCurrentPatternAs(UIA_TextPattern2Id, IID_IUIAutomationTextPattern2, (void**)&tp2);
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
    hr = el->GetCurrentPatternAs(UIA_TextPatternId, IID_IUIAutomationTextPattern, (void**)&tp);
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
    hr = el->GetCurrentPatternAs(UIA_ValuePatternId, IID_IUIAutomationValuePattern, (void**)&vp);
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
    if (w == g.lastRgnW && h == g.lastRgnH && radius == g.lastRgnRadius) {
        return;
    }
    g.lastRgnW = w;
    g.lastRgnH = h;
    g.lastRgnRadius = radius;

    if (radius <= 0) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }
    // Only update if dimensions changed? SetWindowRgn already optimizes?
    // Actually, calling SetWindowRgn repeatedly might cause flicker.
    // Ideally we track the last applied RGN size. 
    // For now we'll assume the caller (TickUpdate) handles preventing redundant layout updates.
    HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
    if (!SetWindowRgn(hwnd, rgn, TRUE)) {
        DeleteObject(rgn);
    }
}

static SIZE MeasureContentSize(const std::wstring& text, int maxW, int maxH) {
    SIZE sz{ std::max(100, g.effectiveCornerRadius * 2 + 20), std::max(40, g.effectiveCornerRadius * 2 + 20) };
    
    if (text.empty() || !g.hFont) return sz;

    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) return sz;

    HGDIOBJ oldFont = SelectObject(hdc, g.hFont);
    
    // 1. Measure single line height for maxLines calc
    RECT rcCalc = {0,0,0,0};
    DrawTextW(hdc, L"Ay", -1, &rcCalc, DT_CALCRECT | DT_NOPREFIX);
    int lineHeight = rcCalc.bottom - rcCalc.top;
    
    // 2. Measure full text block with wrapping
    int availW = std::max(1, maxW - g.effectivePadding * 2);
    rcCalc = {0, 0, availW, 0};
    DrawTextW(hdc, text.c_str(), (int)text.size(), &rcCalc, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);

    int textW = rcCalc.right - rcCalc.left;
    int textH = rcCalc.bottom - rcCalc.top;

    // 3. Constrain height by maxLines
    if (g.cfg.maxLines > 0) {
        int maxTextH = g.cfg.maxLines * lineHeight;
        if (textH > maxTextH) textH = maxTextH;
    }

    SelectObject(hdc, oldFont);
    DeleteDC(hdc);

    sz.cx = textW + g.effectivePadding * 2;
    sz.cy = textH + g.effectivePadding * 2;

    int minDim = g.effectiveCornerRadius * 2;
    if (sz.cx < minDim) sz.cx = minDim;
    if (sz.cy < minDim) sz.cy = minDim;

    if (sz.cx > maxW) sz.cx = maxW;
    if (sz.cy > maxH) sz.cy = maxH;

    return sz;
}

static void EnsureVisibility(bool show) {
    if (!g.hwndHost) return;
    if (show && !g.visible) {
        ShowWindow(g.hwndHost, SW_SHOWNOACTIVATE);
        g.visible = true;
    } else if (!show && g.visible) {
        ShowWindow(g.hwndHost, SW_HIDE);
        g.visible = false;
        // Reset last state so next show forces update
        g.lastWindowRect = { 0, 0, 0, 0 };
    }
}

static void PositionBubbleNearCursor(HWND hwnd, POINT pt, int w, int h) {
    RECT work = GetWorkAreaForPoint(pt);
    int workL = (int)work.left, workT = (int)work.top, workR = (int)work.right, workB = (int)work.bottom;

    int x = pt.x + g.effectiveOffsetX;
    int y = pt.y + g.effectiveOffsetY;

    if (x + w > workR) x = pt.x - g.effectiveOffsetX - w;
    if (y + h > workB) y = pt.y - g.effectiveOffsetY - h;

    x = std::max(workL, std::min(x, workR - w));
    y = std::max(workT, std::min(y, workB - h));

    RECT newRect = { x, y, x + w, y + h };
    if (newRect.left == g.lastWindowRect.left &&
        newRect.top == g.lastWindowRect.top &&
        newRect.right == g.lastWindowRect.right &&
        newRect.bottom == g.lastWindowRect.bottom) 
    {
        // Position and size haven't changed.
        return;
    }

    g.lastWindowRect = newRect;

    // Use SWP_NOREDRAW to prevent flicker, we'll invalidate manually
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_NOREDRAW);
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd);
}

static void UpdateMagnifierSource(HWND hwndMag, POINT pt, int w, int h) {
    if (!g.magReady || !hwndMag) return;

    float zoom = (float)g.cfg.zoomPercent / 100.0f;
    if (zoom < 1.0f) zoom = 1.0f;

    int srcW = RoundToInt((float)w / zoom);
    int srcH = RoundToInt((float)h / zoom);

    RECT work = GetWorkAreaForPoint(pt);
    LONG workL = work.left, workT = work.top, workR = work.right, workB = work.bottom;

    int halfWLow  = srcW / 2;
    int halfWHigh = (srcW + 1) / 2;
    int halfHLow  = srcH / 2;
    int halfHHigh = (srcH + 1) / 2;

    MAGRECTANGLE src = {
        (LONG)(pt.x - halfWLow),
        (LONG)(pt.y - halfHLow),
        (LONG)(pt.x + halfWHigh),
        (LONG)(pt.y + halfHHigh)
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

            if (g.hBgBrush) {
                FillRect(hdc, &rc, g.hBgBrush);
            }

            if (g.effectiveBorderWidth > 0 && g.hBorderPen) {
                HGDIOBJ oldPen = SelectObject(hdc, g.hBorderPen);
                HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

                int inset = g.effectiveBorderWidth / 2;
                RoundRect(
                    hdc,
                    inset, inset,
                    (rc.right - rc.left) - inset,
                    (rc.bottom - rc.top) - inset,
                    g.effectiveCornerRadius * 2,
                    g.effectiveCornerRadius * 2
                );

                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
            }

            if (g.showingText && g.hFont) {
                HGDIOBJ oldFont = SelectObject(hdc, g.hFont);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, ColorFromRGBInt(g.cfg.textColor));

                RECT tr = rc;
                tr.left   += g.effectivePadding;
                tr.top    += g.effectivePadding;
                tr.right  -= g.effectivePadding;
                tr.bottom -= g.effectivePadding;

                LOGFONTW lf{};
                GetObjectW(g.hFont, sizeof(lf), &lf);
                int lineHeight = (int)std::abs(lf.lfHeight);
                int maxHeight = std::max(1, (g.effectivePadding * 2) + (g.cfg.maxLines * lineHeight));
                int availHeight = std::max(1, (int)(tr.bottom - tr.top));
                int targetHeight = std::min(availHeight, maxHeight);
                int targetWidth = std::max(1, (int)(tr.right - tr.left));
                std::wstring text;

                if (g.currentText == g.lastFittedTextSource && 
                    targetWidth == g.lastFittedWidth &&
                    targetHeight == g.lastFittedHeight &&
                    !g.lastFittedTextSource.empty()) 
                {
                    text = g.cachedFittedText;
                } else {
                    text = FitTextToHeight(
                        hdc,
                        g.currentText,
                        targetWidth,
                        targetHeight,
                        GetTextAlignFlags()
                    );
                    g.lastFittedTextSource = g.currentText;
                    g.lastFittedWidth = targetWidth;
                    g.lastFittedHeight = targetHeight;
                    g.cachedFittedText = text;
                }

                UINT alignFlags = GetTextAlignFlags();

                if (g.cfg.outlineWidth > 0) {
                    SetTextColor(hdc, ColorFromRGBInt(g.cfg.outlineColor));
                    int ow = g.cfg.outlineWidth;
                    for (int dy = -ow; dy <= ow; ++dy) {
                        for (int dx = -ow; dx <= ow; ++dx) {
                            if (dx == 0 && dy == 0) continue;
                            RECT trO = tr;
                            OffsetRect(&trO, dx, dy);
                            DrawTextW(hdc, text.c_str(), -1, &trO,
                                      DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | alignFlags);
                        }
                    }
                }

                if (g.cfg.textShadow) {
                    SetTextColor(hdc, ColorFromRGBInt(g.cfg.shadowColor));
                    RECT trS = tr;
                    OffsetRect(&trS, g.cfg.shadowOffsetX, g.cfg.shadowOffsetY);
                    DrawTextW(hdc, text.c_str(), -1, &trS,
                              DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | alignFlags);
                }

                SetTextColor(hdc, ColorFromRGBInt(g.cfg.textColor));
                DrawTextW(hdc, text.c_str(), -1, &tr,
                          DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | alignFlags);

                SelectObject(hdc, oldFont);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void UiaWorkerThread() {
    // Wh_Log(L"UiaWorkerThread: Started");
    
    // Separate COM init for this thread
    if (!InitUIA()) {
        Wh_Log(L"UiaWorkerThread: InitUIA failed.");
        // We continue running to process exit signals, but we won't get text.
    }

    while (true) {
        std::unique_lock<std::mutex> lock(g.uiaMutex);
        g.uiaCv.wait(lock, []{ return g.uiaThreadShouldExit || g.uiaWorkAvailable; });

        if (g.uiaThreadShouldExit) break;

        POINT targetPt = g.uiaTargetPt;
        g.uiaWorkAvailable = false;
        lock.unlock();

        // Perform potentially blocking UIA call without holding mutex
        std::wstring text;
        bool success = TryExtractTextAtPoint(targetPt, text);

        lock.lock();
        if (success) {
            g.uiaResultText = text;
            g.uiaResultValid = true;
        } else {
            g.uiaResultValid = false;
        }
        lock.unlock();
    }

    UninitUIA();
    // Wh_Log(L"UiaWorkerThread: Exited");
}

static bool CreateWindows() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kHostClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    ATOM atom = RegisterClassExW(&wc);
    if (!atom) {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

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

    if (g.magReady) {
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
    } else {
        g.hwndMag = nullptr;
    }

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

    bool triggerDown = IsTriggerDown(g.cfg.triggerKey);
    DWORD nowTick = GetTickCount();

    if (g.cfg.triggerKey == TriggerKey::CapsLock) {
        EnsureCapsLockOff();
    }

    static bool dbgWasTriggerDown = false;
    if (triggerDown != dbgWasTriggerDown) {
        // Wh_Log(L"TickUpdate: Trigger %s (Key: %d)", triggerDown ? L"DOWN" : L"UP", (int)g.cfg.triggerKey);
        dbgWasTriggerDown = triggerDown;
    }

    if (!triggerDown) {
        if (g.cfg.autoHideDelayMs > 0) {
            if (g.triggerWasDown) {
                g.lastTriggerUpTick = nowTick;
                g.triggerWasDown = false;
            }

            if (nowTick - g.lastTriggerUpTick >= (DWORD)g.cfg.autoHideDelayMs) {
                EnsureVisibility(false);
                return;
            }
        } else {
            EnsureVisibility(false);
            g.triggerWasDown = false;
            return;
        }
    } else {
        g.triggerWasDown = true;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        EnsureVisibility(false);
        return;
    }

    UpdateEffectiveSizing(pt);

    int w = g.effectiveBubbleWidth;
    int h = g.effectiveBubbleHeight;

    // PositionBubbleNearCursor was here, moved down for dynamic sizing

    const bool wantText = (g.cfg.mode == Mode::TextOnly || g.cfg.mode == Mode::Auto);
    const bool wantMag  = (g.cfg.mode == Mode::MagnifierOnly || g.cfg.mode == Mode::Auto);

    bool haveText = false;
    std::wstring text;

    if (wantText) {
        if (nowTick - g.lastUiaQueryTick >= (DWORD)g.cfg.uiaQueryMinIntervalMs) {
            g.lastUiaQueryTick = nowTick;
            
            // Dispatch work to UIA thread
            {
                std::lock_guard<std::mutex> lock(g.uiaMutex);
                g.uiaTargetPt = pt;
                g.uiaWorkAvailable = true;
            }
            g.uiaCv.notify_one();
        }
        
        // Retrieve latest result
        {
            std::lock_guard<std::mutex> lock(g.uiaMutex);
            if (g.uiaResultValid) {
                text = g.uiaResultText;
                haveText = true;
                g.lastValidTextTick = nowTick;
            }
        }

        static bool dbgLoggedNoText = false;
        if (!haveText) {
             // Hysteresis: If we lost text, check if we should debounce
            if (g.showingText && !g.currentText.empty() && 
                (nowTick - g.lastValidTextTick < 300)) // 300ms grace period
            {
                haveText = true;
                text = g.currentText;
            } else if (!dbgLoggedNoText && triggerDown) {
                // Wh_Log(L"TickUpdate: No text found via UIA.");
                dbgLoggedNoText = true;
            }
        } else {
            dbgLoggedNoText = false;
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

    // Dynamic Sizing Logic
    if (showText) {
        SIZE sz = MeasureContentSize(text, g.effectiveBubbleWidth, g.effectiveBubbleHeight);
        w = sz.cx;
        h = sz.cy;
    }

    if (showText || showMag) {
        PositionBubbleNearCursor(g.hwndHost, pt, w, h);
        ApplyRoundedRegion(g.hwndHost, w, h, g.effectiveCornerRadius);
        EnsureVisibility(true);
    }

    bool modeChanged = (showText != g.showingText) || (showMag != g.showingMag);
    bool textChanged = showText && (text != g.currentText);

    g.showingText = showText;
    g.showingMag = showMag;

    if (showText) {
        g.currentText = text;
        if (g.hwndMag) ShowWindow(g.hwndMag, SW_HIDE);
        if (textChanged || modeChanged) {
            InvalidateRect(g.hwndHost, nullptr, TRUE);
            UpdateWindow(g.hwndHost);
            // Wh_Log(L"Showing Text: %s", text.substr(0, std::min<size_t>(text.size(), 30)).c_str());
        }
    } else if (showMag) {
        ShowWindow(g.hwndMag, SW_SHOWNA);
        SetWindowPos(g.hwndMag, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

        if (pfnMagSetWindowFilterList) {
            HWND exclude[] = { g.hwndHost, g.hwndMag };
            pfnMagSetWindowFilterList(g.hwndMag, MW_FILTERMODE_EXCLUDE, 2, exclude);
        }

        UpdateMagnifierSource(g.hwndMag, pt, w, h);
        if (modeChanged) {
            InvalidateRect(g.hwndHost, nullptr, TRUE);
            UpdateWindow(g.hwndHost);
            Wh_Log(L"Showing Magnifier");
        }
    } else {
        EnsureVisibility(false);
    }
}

static void WorkerThread() {
    // g.threadId = GetCurrentThreadId();
    // Wh_Log(L"WorkerThread: Started. ThreadId=%u", g.threadId);

    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE); // ensure queue exists

    LoadSettings();

    // Use LL Hook if CapsLock needed
    if (g.cfg.triggerKey == TriggerKey::CapsLock) {
        InstallHooks();
        EnsureCapsLockOff();
    } else {
        UninstallHooks();
    }
    
    // InitUIA moved to UiaWorkerThread

    if (!InitMagnification()) {
        Wh_Log(L"WorkerThread: InitMagnification failed. Magnifier mode will not work.");
    }

    if (!CreateWindows()) {
        Wh_Log(L"WorkerThread: CreateWindows failed. Exiting.");
        // g.running = false;
        UninitMagnification();
        // UninitUIA();
        return;
    }
    // Wh_Log(L"WorkerThread: Window created. HWNDHost=%p", g.hwndHost);

    g.running = true;
    
    // Start separate UIA/Text thread
    g.uiaThread = std::thread(UiaWorkerThread);

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
                if (g.cfg.triggerKey == TriggerKey::CapsLock) InstallHooks(); else UninstallHooks();
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

    UninstallHooks();

    // Signal UIA thread to exit
    {
        std::lock_guard<std::mutex> lock(g.uiaMutex);
        g.uiaThreadShouldExit = true;
    }
    g.uiaCv.notify_all();

    if (g.uiaThread.joinable()) {
        g.uiaThread.join();
    }

    FreeGraphicsResources();
    UninitMagnification();
    // UIA uninit moved to thread
    // UninitUIA();

}



BOOL WhTool_ModInit() {
    g.worker = std::thread(WorkerThread);
    g.threadId = GetThreadId((HANDLE)g.worker.native_handle());
    return TRUE;
}

void WhTool_ModUninit() {
    if (g.threadId != 0) {
        PostThreadMessage(g.threadId, WM_APP_EXIT_THREAD, 0, 0);
        PostThreadMessage(g.threadId, WM_QUIT, 0, 0);
    }
    // Give the thread a moment to process the exit message
    if (g.worker.joinable()) {
        // Wait up to 500ms for clean exit
        DWORD start = GetTickCount();
        while (g.running && (GetTickCount() - start < 500)) {
            Sleep(10);
        }
        
        // If still running, detach and let ExitProcess clean it up.
        // Calling join() here would hang if the thread is blocked (e.g. in UIA call).
        if (g.running) {
             g.worker.detach();
             Wh_Log(L"WorkerThread didn't exit in time. Detaching.");
        } else {
             g.worker.join();
        }
    }
    g.running = false;
}

void WhTool_ModSettingsChanged() {
    if (g.threadId != 0) {
        PostThreadMessage(g.threadId, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}


//////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
bool g_wrongProcess;
bool g_shouldExitProcess;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    if (g_shouldExitProcess) {
        // Just exit the main thread. Since we didn't start any worker threads
        // (because initialization failed), this will naturally cause the process
        // to exit when the last thread dies. This avoids aggressive ExitProcess
        // calls that might race with the injector.
        ExitThread(0);
    }
    // Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    // Safety check: Ensure we only run in windhawk.exe
    // This protects against cases where the @include rule is ignored or overridden
    WCHAR processPath[MAX_PATH];
    DWORD processPathLen = GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    if (processPathLen == 0 || processPathLen >= ARRAYSIZE(processPath)) {
        // If we can't reliably identify the host process, stay dormant to avoid unload issues.
        g_wrongProcess = true;
        return TRUE;
    }

    const WCHAR* processName = wcsrchr(processPath, L'\\');
    processName = processName ? processName + 1 : processPath;
    if (_wcsicmp(processName, L"windhawk.exe") != 0) {
        // Quietly exit for non-Windhawk processes to prevent noise.
        // Returning FALSE causes "Process prohibits dynamic code" in some sandboxed apps (e.g. Claude)
        // because Windhawk attempts to unload the DLL. Returning TRUE keeps it loaded but dormant.
        g_wrongProcess = true;
        return TRUE;
    }

    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        g_wrongProcess = true;
        return TRUE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        // Avoid unload attempts in the service process.
        g_wrongProcess = true;
        return TRUE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            g_shouldExitProcess = true;
        }

        if (!g_shouldExitProcess && GetLastError() == ERROR_ALREADY_EXISTS) {
            // This happens during mod reload if the old process hasn't exited yet.
            // Log it so we know why we are terminating.
            // ...
            
            DWORD waitRes = WaitForSingleObject(g_toolModProcessMutex, 2000);
            if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_ABANDONED) {
                 // Wh_Log(L"Previous instance exited/abandoned mutex. Proceeding.");
            } else {
                 Wh_Log(L"Previous instance still running after timeout. Exiting new instance.");
                 g_shouldExitProcess = true;
            }
        }

        if (!g_shouldExitProcess && !WhTool_ModInit()) {
            Wh_Log(L"WhTool_ModInit failed");
            g_shouldExitProcess = true;
        }


        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        // Another tool-mod process; stay dormant to avoid unload issues.
        g_wrongProcess = true;
        return TRUE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher || g_wrongProcess) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher || g_wrongProcess) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

