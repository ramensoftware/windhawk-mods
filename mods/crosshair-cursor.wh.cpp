// ==WindhawkMod==
// @id              crosshair-cursor
// @name            Crosshair Cursor
// @description     Replace the arrow pointer with a customizable crosshair, an optional AutoCAD-style reticle, and a toggle hotkey
// @version         0.8
// @author          Jay Simons
// @github          https://github.com/designly1
// @homepage        https://yaa.bz
// @license         MIT
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @include         *
// @compilerOptions -lshell32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Crosshair Cursor

Replaces the standard arrow pointer with a crosshair.

- **Hair length** is a percentage of the monitor under the pointer. At 100 the
  horizontal hair runs from the left edge of that monitor to the right edge,
  and the vertical hair runs from top to bottom. Lower values draw a shorter
  cross, still passing through the pointer and kept on that monitor.
- **Color** is the hair color. Windhawk 2.0 shows a color picker. Windhawk
  1.7.3 shows a text field; type a color as `#RRGGBB`.
- **Thickness** is the hair thickness in pixels. Windhawk 2.0 shows a slider.
  Windhawk 1.7.3 shows a number field. Values are clamped to the same range.
- **Crosshair opacity** controls how see-through the horizontal and vertical
  hairs are, from 0 (invisible) to 100 (solid).
- **Reticle** adds an AutoCAD-style hollow square centered on the pointer.
  **Reticle size** controls the square's outer side length in pixels. The
  reticle uses the same color and thickness as the hairs. **Reticle opacity**
  sets how see-through the reticle outline is, independent of the hairs.

Text, link, wait, and resize pointers are left alone unless **Override app
cursors** is enabled. That option keeps the crosshair up when a program
changes the pointer, including links and text in a browser. The crosshair
still hides while an app hides the pointer.

**Toggle hotkey** shows or hides the crosshair without disabling the mod. In
Windhawk 2.0, click the shortcut badge and press the keys you want. In
Windhawk 1.7.3, type a shortcut such as `Ctrl+Alt+C`. Leave it empty for no
shortcut. Press the same shortcut again to bring the crosshair back.

The mod runs in its own process instead of inside Explorer. Disabling or
removing it restores the pointer scheme from Mouse settings. If a crash leaves
the arrow hidden, open Mouse settings and reapply the pointer scheme, or
enable this mod again and then disable it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- length: 15
  $name: Hair length
  $description: "Percentage of the screen each hair covers. 100 spans that screen edge to edge."
  #! $min: 1
  #! $max: 100
  #! $format: slider
- color: "#00FF00"
  $name: Color
  $description: "Hair color. Use #RRGGBB, for example #00FF00."
  #! $format: colorRgb
- thickness: 1
  $name: Thickness
  $description: Hair thickness in pixels.
  #! $min: 1
  #! $max: 64
  #! $format: slider
- crosshairOpacity: 50
  $name: Crosshair opacity
  $description: How opaque the crosshair hairs are, from 0 (transparent) to 100 (solid).
  #! $min: 0
  #! $max: 100
  #! $format: slider
- reticle: true
  $name: Reticle
  $description: Draw an AutoCAD-style hollow square at the center of the crosshair.
- reticleSize: 16
  $name: Reticle size
  $description: Outer side length of the reticle square in pixels.
  #! $min: 4
  #! $max: 128
  #! $format: slider
- reticleOpacity: 75
  $name: Reticle opacity
  $description: How opaque the reticle outline is, from 0 (transparent) to 100 (solid).
  #! $min: 0
  #! $max: 100
  #! $format: slider
- overrideAppCursors: true
  $name: Override app cursors
  $description: Keep the crosshair when a program changes the pointer, such as links and text in a browser.
- toggleHotkey: ""
  $name: Toggle hotkey
  $description: "Shortcut that shows or hides the crosshair. Click and press keys, or type Ctrl+Alt+C. Leave empty for none."
  #! $format: hotkey
*/
// ==/WindhawkModSettings==

#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#ifndef IDC_PIN
#define IDC_PIN MAKEINTRESOURCE(32671)
#endif
#ifndef IDC_PERSON
#define IDC_PERSON MAKEINTRESOURCE(32672)
#endif
// OCR_* values live behind OEMRESOURCE. Keep the arrow id available on both
// Windhawk 1.7 and 2.0 header sets.
#ifndef OCR_NORMAL
#define OCR_NORMAL 32512
#endif

constexpr int kMinLength = 1;
constexpr int kMaxLength = 100;
constexpr int kDefaultLength = 15;
constexpr int kMinThickness = 1;
constexpr int kMaxThickness = 64;
constexpr int kDefaultThickness = 1;
constexpr int kMinReticleSize = 4;
constexpr int kMaxReticleSize = 128;
constexpr int kMinOpacity = 0;
constexpr int kMaxOpacity = 100;
constexpr int kDefaultOpacity = 100;
constexpr COLORREF kDefaultColor = RGB(0, 255, 0);
constexpr UINT kTimerId = 1;
constexpr UINT_PTR kTimerIntervalMs = 16;
constexpr DWORD kBlankRecheckMs = 500;
constexpr int kBlankCursorSize = 32;
constexpr UINT WM_APP_UPDATE = WM_APP;
constexpr int kToggleHotkeyId = 1;
constexpr LONG kToggleOn = 1;
constexpr LONG kToggleOff = 2;
constexpr WCHAR kToggleStateName[] = L"Local\\WindhawkCustomCursorToggle";

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

struct Settings {
    int length;
    int thickness;
    int reticleSize;
    int crosshairOpacity;
    int reticleOpacity;
    COLORREF color;
    bool reticle;
    bool forceOverride;
    UINT hotkeyModifiers;
    UINT hotkeyVk;
};

struct LineCache {
    HWND hwnd;
    int x;
    int y;
    int w;
    int h;
    COLORREF color;
    BYTE alpha;
    bool valid;
};

struct ReticleCache {
    HWND hwnd;
    int x;
    int y;
    int size;
    int thickness;
    COLORREF color;
    BYTE alpha;
    bool valid;
    bool shown;
};

struct App {
    Settings settings;
    LineCache horizontal;
    LineCache vertical;
    ReticleCache reticle;
    bool shown;
    bool cursorReplaced;
    bool haveCursorSample;
    HCURSOR sampledCursor;
    bool sampledCursorIsArrow;
    DWORD lastBlankCheck;
    DWORD threadId;
    HANDLE thread;
    HANDLE readyEvent;
    volatile LONG dirty;
    bool suspended;
    bool hotkeyRegistered;
    UINT registeredModifiers;
    UINT registeredVk;
};

App g_app;
LONG* g_toggleState;
HANDLE g_toggleMapping;

int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

BYTE OpacityPercentToAlpha(int percent) {
    percent = ClampInt(percent, kMinOpacity, kMaxOpacity);
    return static_cast<BYTE>(MulDiv(percent, 255, 100));
}

int HexDigit(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') {
        return ch - L'0';
    }
    if (ch >= L'a' && ch <= L'f') {
        return ch - L'a' + 10;
    }
    if (ch >= L'A' && ch <= L'F') {
        return ch - L'A' + 10;
    }
    return -1;
}

COLORREF ParseColor(PCWSTR text) {
    if (!text) {
        return kDefaultColor;
    }
    while (*text == L' ' || *text == L'\t') {
        text++;
    }
    if (*text == L'#') {
        text++;
    }

    unsigned value = 0;
    for (int i = 0; i < 6; i++) {
        int digit = HexDigit(text[i]);
        if (digit < 0) {
            return kDefaultColor;
        }
        value = (value << 4) | static_cast<unsigned>(digit);
    }
    return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

void RestoreSystemCursor();
bool ApplySystemCursors();
void SetSuspended(bool suspended);

void EnsureToggleState() {
    if (g_toggleState) {
        return;
    }

    // A new mapping is zero-filled. 0 and kToggleOn both mean visible.
    // kToggleOff means the hotkey hid the crosshair. Compare-exchange turns
    // the initial 0 into kToggleOn without clobbering a later kToggleOff.
    HANDLE mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr,
                                        PAGE_READWRITE, 0, sizeof(LONG),
                                        kToggleStateName);
    if (!mapping) {
        Wh_Log(L"Toggle state mapping failed: %u", GetLastError());
        return;
    }

    LONG* state = static_cast<LONG*>(
        MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LONG)));
    if (!state) {
        Wh_Log(L"Toggle state view failed: %u", GetLastError());
        CloseHandle(mapping);
        return;
    }

    InterlockedCompareExchange(state, kToggleOn, 0);
    g_toggleMapping = mapping;
    g_toggleState = state;
}

bool IsCrosshairSuspended() {
    EnsureToggleState();
    if (!g_toggleState) {
        return false;
    }
    return InterlockedCompareExchange(g_toggleState, 0, 0) == kToggleOff;
}

void PublishToggleState(bool suspended) {
    EnsureToggleState();
    if (g_toggleState) {
        InterlockedExchange(g_toggleState, suspended ? kToggleOff : kToggleOn);
    }
}

bool TokenEquals(const wchar_t* token, int length, const wchar_t* lowerName) {
    for (int i = 0; i < length; i++) {
        wchar_t ch = token[i];
        if (ch >= L'A' && ch <= L'Z') {
            ch = static_cast<wchar_t>(ch - L'A' + L'a');
        }
        if (lowerName[i] != ch) {
            return false;
        }
    }
    return lowerName[length] == L'\0';
}

bool ParseKeyToken(const wchar_t* token, int length, UINT* vk) {
    if (length <= 0) {
        return false;
    }

    bool digits = true;
    for (int i = 0; i < length; i++) {
        if (token[i] < L'0' || token[i] > L'9') {
            digits = false;
            break;
        }
    }
    if (digits) {
        // A single digit is that key. Windhawk's hotkey editor stores every
        // other key as a decimal virtual-key code of two or more digits.
        if (length == 1) {
            *vk = token[0];
            return true;
        }
        unsigned value = 0;
        for (int i = 0; i < length; i++) {
            value = value * 10 + static_cast<unsigned>(token[i] - L'0');
            if (value > 255) {
                return false;
            }
        }
        if (value == 0) {
            return false;
        }
        *vk = value;
        return true;
    }

    if (length == 1) {
        wchar_t ch = token[0];
        if (ch >= L'a' && ch <= L'z') {
            ch = static_cast<wchar_t>(ch - L'a' + L'A');
        }
        if (ch >= L'A' && ch <= L'Z') {
            *vk = ch;
            return true;
        }
        return false;
    }

    struct KeyName {
        const wchar_t* name;
        UINT vk;
    };
    const KeyName names[] = {
        {L"backspace", VK_BACK}, {L"tab", VK_TAB},       {L"enter", VK_RETURN},
        {L"return", VK_RETURN},  {L"esc", VK_ESCAPE},    {L"escape", VK_ESCAPE},
        {L"space", VK_SPACE},    {L"spacebar", VK_SPACE},{L"pageup", VK_PRIOR},
        {L"pgup", VK_PRIOR},     {L"pagedown", VK_NEXT}, {L"pgdn", VK_NEXT},
        {L"end", VK_END},        {L"home", VK_HOME},     {L"left", VK_LEFT},
        {L"up", VK_UP},          {L"right", VK_RIGHT},   {L"down", VK_DOWN},
        {L"insert", VK_INSERT},  {L"ins", VK_INSERT},    {L"delete", VK_DELETE},
        {L"del", VK_DELETE},     {L"f1", VK_F1},         {L"f2", VK_F2},
        {L"f3", VK_F3},          {L"f4", VK_F4},         {L"f5", VK_F5},
        {L"f6", VK_F6},          {L"f7", VK_F7},         {L"f8", VK_F8},
        {L"f9", VK_F9},          {L"f10", VK_F10},       {L"f11", VK_F11},
        {L"f12", VK_F12},
    };
    for (const KeyName& name : names) {
        if (TokenEquals(token, length, name.name)) {
            *vk = name.vk;
            return true;
        }
    }
    return false;
}

bool ParseHotkey(PCWSTR text, UINT* modifiersOut, UINT* vkOut) {
    *modifiersOut = 0;
    *vkOut = 0;
    if (!text) {
        return false;
    }

    UINT modifiers = 0;
    UINT vk = 0;
    bool haveKey = false;
    const wchar_t* cursor = text;
    while (*cursor) {
        while (*cursor == L' ' || *cursor == L'\t') {
            cursor++;
        }
        if (*cursor == L'\0') {
            break;
        }

        const wchar_t* start = cursor;
        while (*cursor && *cursor != L'+') {
            cursor++;
        }
        int length = static_cast<int>(cursor - start);
        while (length > 0 &&
               (start[length - 1] == L' ' || start[length - 1] == L'\t')) {
            length--;
        }
        if (*cursor == L'+') {
            cursor++;
        }
        if (length <= 0) {
            continue;
        }

        if (TokenEquals(start, length, L"ctrl") ||
            TokenEquals(start, length, L"control")) {
            modifiers |= MOD_CONTROL;
            continue;
        }
        if (TokenEquals(start, length, L"alt")) {
            modifiers |= MOD_ALT;
            continue;
        }
        if (TokenEquals(start, length, L"shift")) {
            modifiers |= MOD_SHIFT;
            continue;
        }
        if (TokenEquals(start, length, L"win") ||
            TokenEquals(start, length, L"windows") ||
            TokenEquals(start, length, L"meta")) {
            modifiers |= MOD_WIN;
            continue;
        }
        if (TokenEquals(start, length, L"norepeat")) {
            continue;
        }

        UINT parsed = 0;
        if (haveKey || !ParseKeyToken(start, length, &parsed)) {
            return false;
        }
        vk = parsed;
        haveKey = true;
    }

    if (!haveKey || vk == 0) {
        return false;
    }
    *modifiersOut = modifiers;
    *vkOut = vk;
    return true;
}

void UpdateHotkeyRegistration() {
    HWND hwnd = g_app.horizontal.hwnd;
    if (!hwnd) {
        return;
    }

    UINT modifiers =
        g_app.settings.hotkeyVk ? (g_app.settings.hotkeyModifiers | MOD_NOREPEAT)
                                : 0;
    UINT vk = g_app.settings.hotkeyVk;
    if (g_app.hotkeyRegistered && g_app.registeredModifiers == modifiers &&
        g_app.registeredVk == vk) {
        return;
    }

    if (g_app.hotkeyRegistered) {
        UnregisterHotKey(hwnd, kToggleHotkeyId);
        g_app.hotkeyRegistered = false;
    }
    g_app.registeredModifiers = 0;
    g_app.registeredVk = 0;
    if (vk == 0) {
        return;
    }

    if (!RegisterHotKey(hwnd, kToggleHotkeyId, modifiers, vk)) {
        Wh_Log(L"RegisterHotKey failed: %u", GetLastError());
        return;
    }
    g_app.hotkeyRegistered = true;
    g_app.registeredModifiers = modifiers;
    g_app.registeredVk = vk;
    Wh_Log(L"Registered toggle hotkey modifiers=%u vk=%u", modifiers, vk);
}

void LoadSettings() {
    bool wasOverride = g_app.settings.forceOverride;
    g_app.settings.length =
        ClampInt(Wh_GetIntSetting(L"length"), kMinLength, kMaxLength);
    g_app.settings.thickness =
        ClampInt(Wh_GetIntSetting(L"thickness"), kMinThickness, kMaxThickness);
    g_app.settings.reticleSize = ClampInt(Wh_GetIntSetting(L"reticleSize"),
                                          kMinReticleSize, kMaxReticleSize);
    g_app.settings.crosshairOpacity =
        ClampInt(Wh_GetIntSetting(L"crosshairOpacity"), kMinOpacity, kMaxOpacity);
    g_app.settings.reticleOpacity =
        ClampInt(Wh_GetIntSetting(L"reticleOpacity"), kMinOpacity, kMaxOpacity);
    g_app.settings.reticle = Wh_GetIntSetting(L"reticle") != 0;
    g_app.settings.forceOverride = Wh_GetIntSetting(L"overrideAppCursors") != 0;

    PCWSTR colorText = Wh_GetStringSetting(L"color");
    g_app.settings.color = ParseColor(colorText);
    Wh_FreeStringSetting(colorText);

    PCWSTR hotkeyText = Wh_GetStringSetting(L"toggleHotkey");
    UINT modifiers = 0;
    UINT vk = 0;
    if (!ParseHotkey(hotkeyText, &modifiers, &vk)) {
        modifiers = 0;
        vk = 0;
        if (hotkeyText && hotkeyText[0] != L'\0') {
            Wh_Log(L"Could not parse toggle hotkey: %s", hotkeyText);
        }
    }
    Wh_FreeStringSetting(hotkeyText);
    g_app.settings.hotkeyModifiers = modifiers;
    g_app.settings.hotkeyVk = vk;

    Wh_Log(L"Settings length=%d thickness=%d color=%06X crosshairOpacity=%d "
           L"reticle=%d reticleSize=%d reticleOpacity=%d override=%d hotkey=%u,%u",
           g_app.settings.length, g_app.settings.thickness,
           static_cast<unsigned>(g_app.settings.color) & 0xFFFFFF,
           g_app.settings.crosshairOpacity, g_app.settings.reticle ? 1 : 0,
           g_app.settings.reticleSize, g_app.settings.reticleOpacity,
           g_app.settings.forceOverride ? 1 : 0, modifiers, vk);

    UpdateHotkeyRegistration();

    if (vk == 0) {
        PublishToggleState(false);
        if (g_app.suspended) {
            SetSuspended(false);
        }
    }

    if (!g_app.suspended && g_app.cursorReplaced &&
        wasOverride != g_app.settings.forceOverride) {
        RestoreSystemCursor();
        ApplySystemCursors();
    }
}

int PlaceSpan(int cursor, int origin, int span, int length) {
    if (length >= span) {
        return origin;
    }
    int start = cursor - length / 2;
    int limit = origin + span;
    if (start < origin) {
        start = origin;
    }
    if (start + length > limit) {
        start = limit - length;
    }
    return start;
}

bool BitmapIsFullyTransparent(HBITMAP bitmap) {
    BITMAP info;
    if (!GetObject(bitmap, sizeof(info), &info) || info.bmBitsPixel != 32 ||
        info.bmWidth <= 0 || info.bmHeight <= 0 || info.bmWidth > 256 ||
        info.bmHeight > 256) {
        return false;
    }

    BITMAPINFOHEADER header = {};
    header.biSize = sizeof(header);
    header.biWidth = info.bmWidth;
    header.biHeight = -info.bmHeight;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;

    int count = info.bmWidth * info.bmHeight;
    DWORD* pixels = static_cast<DWORD*>(
        HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(count) * sizeof(DWORD)));
    if (!pixels) {
        return false;
    }

    HDC screen = GetDC(nullptr);
    int copied = screen ? GetDIBits(screen, bitmap, 0, info.bmHeight, pixels,
                                     reinterpret_cast<BITMAPINFO*>(&header),
                                     DIB_RGB_COLORS)
                        : 0;
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    bool transparent = copied == info.bmHeight;
    for (int i = 0; transparent && i < count; i++) {
        if ((pixels[i] >> 24) != 0) {
            transparent = false;
        }
    }
    HeapFree(GetProcessHeap(), 0, pixels);
    return transparent;
}

bool MonochromeMaskIsTransparent(HBITMAP mask) {
    BITMAP info;
    if (!GetObject(mask, sizeof(info), &info) || info.bmBitsPixel != 1 ||
        info.bmWidth <= 0 || info.bmHeight <= 0 || info.bmWidth > 256 ||
        info.bmHeight > 512 || info.bmWidthBytes <= 0) {
        return false;
    }

    SIZE_T bytes = static_cast<SIZE_T>(info.bmHeight) * info.bmWidthBytes;
    BYTE* bits = static_cast<BYTE*>(HeapAlloc(GetProcessHeap(), 0, bytes));
    if (!bits) {
        return false;
    }
    bool transparent = GetBitmapBits(mask, static_cast<LONG>(bytes), bits) != 0;
    int half = info.bmHeight / 2;
    int usedBytes = (info.bmWidth + 7) / 8;
    if (usedBytes > info.bmWidthBytes) {
        usedBytes = info.bmWidthBytes;
    }
    for (int y = 0; transparent && y < info.bmHeight; y++) {
        bool xorRow = half > 0 && y >= half;
        BYTE expected = xorRow ? 0x00 : 0xFF;
        const BYTE* row = bits + static_cast<SIZE_T>(y) * info.bmWidthBytes;
        for (int x = 0; x < usedBytes; x++) {
            if (row[x] != expected) {
                transparent = false;
                break;
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, bits);
    return transparent;
}

bool CursorIsBlank(HCURSOR cursor) {
    ICONINFO iconInfo = {};
    if (!GetIconInfo(cursor, &iconInfo)) {
        return false;
    }
    bool blank = false;
    if (iconInfo.hbmColor) {
        blank = BitmapIsFullyTransparent(iconInfo.hbmColor);
    } else if (iconInfo.hbmMask) {
        blank = MonochromeMaskIsTransparent(iconInfo.hbmMask);
    }
    if (iconInfo.hbmMask) {
        DeleteObject(iconInfo.hbmMask);
    }
    if (iconInfo.hbmColor) {
        DeleteObject(iconInfo.hbmColor);
    }
    return blank;
}

bool IsNonArrowSystemCursor(HCURSOR cursor) {
    const LPCWSTR ids[] = {
        IDC_IBEAM,       IDC_WAIT,     IDC_CROSS,     IDC_UPARROW,
        IDC_SIZE,        IDC_ICON,     IDC_SIZENWSE,  IDC_SIZENESW,
        IDC_SIZEWE,      IDC_SIZENS,   IDC_SIZEALL,   IDC_NO,
        IDC_HAND,        IDC_APPSTARTING, IDC_HELP,   IDC_PIN,
        IDC_PERSON,
    };
    for (LPCWSTR id : ids) {
        if (cursor == LoadCursor(nullptr, id)) {
            return true;
        }
    }
    return false;
}

bool IsDefaultArrow(HCURSOR cursor) {
    if (!cursor) {
        return false;
    }
    if (cursor == g_app.sampledCursor && g_app.haveCursorSample) {
        return g_app.sampledCursorIsArrow;
    }

    bool isArrow = false;
    if (cursor == LoadCursor(nullptr, IDC_ARROW)) {
        isArrow = true;
    } else if (!IsNonArrowSystemCursor(cursor)) {
        isArrow = CursorIsBlank(cursor);
    }

    g_app.sampledCursor = cursor;
    g_app.sampledCursorIsArrow = isArrow;
    g_app.haveCursorSample = true;
    return isArrow;
}

HCURSOR CreateBlankCursor() {
    // Monochrome cursor: AND 1 and XOR 0 leaves the screen unchanged. A color
    // cursor with a zero mask is drawn black, which showed up as a box.
    constexpr int kMaskBytes = kBlankCursorSize * ((kBlankCursorSize + 15) / 16) * 2;
    BYTE andMask[kMaskBytes];
    BYTE xorMask[kMaskBytes];
    memset(andMask, 0xFF, sizeof(andMask));
    memset(xorMask, 0x00, sizeof(xorMask));
    return CreateCursor(nullptr, 0, 0, kBlankCursorSize, kBlankCursorSize, andMask,
                        xorMask);
}

void RestoreSystemCursor() {
    if (!g_app.cursorReplaced) {
        return;
    }
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    g_app.cursorReplaced = false;
    g_app.haveCursorSample = false;
    Wh_Log(L"Restored system cursors");
}

bool ApplySystemCursors() {
    // With override off, only the arrow is replaced. With it on, the standard
    // link, text, wait, and resize cursors are replaced too.
    const DWORD arrowOnly[] = {OCR_NORMAL};
    const DWORD allCursors[] = {
        32512, 32513, 32514, 32515, 32516, 32640, 32641, 32642, 32643,
        32644, 32645, 32646, 32648, 32649, 32650, 32651, 32671, 32672,
    };
    const DWORD* ids = arrowOnly;
    int count = ARRAYSIZE(arrowOnly);
    if (g_app.settings.forceOverride) {
        ids = allCursors;
        count = ARRAYSIZE(allCursors);
    }

    bool any = false;
    for (int i = 0; i < count; i++) {
        HCURSOR cursor = CreateBlankCursor();
        if (!cursor) {
            Wh_Log(L"CreateBlankCursor failed");
            continue;
        }
        if (!SetSystemCursor(cursor, ids[i])) {
            DestroyCursor(cursor);
            continue;
        }
        any = true;
    }
    g_app.cursorReplaced = any;
    g_app.haveCursorSample = false;
    g_app.lastBlankCheck = GetTickCount();
    return any;
}

void MaintainBlankCursor() {
    DWORD now = GetTickCount();
    if (now - g_app.lastBlankCheck < kBlankRecheckMs) {
        return;
    }
    g_app.lastBlankCheck = now;

    HCURSOR arrow = LoadCursor(nullptr, IDC_ARROW);
    if (arrow && !CursorIsBlank(arrow)) {
        Wh_Log(L"Arrow cursor was reset; applying the blank cursor again");
        ApplySystemCursors();
        return;
    }
    if (!g_app.settings.forceOverride) {
        return;
    }
    HCURSOR hand = LoadCursor(nullptr, IDC_HAND);
    if (hand && !CursorIsBlank(hand)) {
        Wh_Log(L"App cursors were reset; applying the blank cursors again");
        ApplySystemCursors();
    }
}

bool PaintSolid(HWND hwnd, int width, int height, COLORREF color, BYTE alpha) {
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP bitmap =
        screen ? CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0)
               : nullptr;
    HDC memory = bitmap ? CreateCompatibleDC(screen) : nullptr;
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    if (!bitmap || !memory || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (memory) {
            DeleteDC(memory);
        }
        return false;
    }

    DWORD* pixels = static_cast<DWORD*>(bits);
    DWORD pixel = color | 0xFF000000;
    int count = width * height;
    for (int i = 0; i < count; i++) {
        pixels[i] = pixel;
    }

    HGDIOBJ previous = SelectObject(memory, bitmap);
    POINT origin = {};
    SIZE size = {width, height};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;
    BOOL painted =
        UpdateLayeredWindow(hwnd, nullptr, nullptr, &size, memory, &origin, 0,
                            &blend, ULW_ALPHA);
    SelectObject(memory, previous);
    DeleteObject(bitmap);
    DeleteDC(memory);
    return painted;
}

bool PaintSquareOutline(HWND hwnd, int size, int thickness, COLORREF color,
                          BYTE alpha) {
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP bitmap =
        screen ? CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0)
               : nullptr;
    HDC memory = bitmap ? CreateCompatibleDC(screen) : nullptr;
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    if (!bitmap || !memory || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (memory) {
            DeleteDC(memory);
        }
        return false;
    }

    DWORD* pixels = static_cast<DWORD*>(bits);
    DWORD borderPixel = color | 0xFF000000;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            bool border = x < thickness || x >= size - thickness ||
                          y < thickness || y >= size - thickness;
            pixels[y * size + x] = border ? borderPixel : 0;
        }
    }

    HGDIOBJ previous = SelectObject(memory, bitmap);
    POINT origin = {};
    SIZE windowSize = {size, size};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;
    BOOL painted =
        UpdateLayeredWindow(hwnd, nullptr, nullptr, &windowSize, memory, &origin,
                            0, &blend, ULW_ALPHA);
    SelectObject(memory, previous);
    DeleteObject(bitmap);
    DeleteDC(memory);
    return painted;
}

void SyncLine(LineCache* line, int x, int y, int width, int height, COLORREF color,
              BYTE alpha) {
    if (width < 1) {
        width = 1;
    }
    if (height < 1) {
        height = 1;
    }
    if (width > 32767) {
        width = 32767;
    }
    if (height > 32767) {
        height = 32767;
    }

    bool appearanceChanged = !line->valid || line->w != width || line->h != height ||
                             line->color != color || line->alpha != alpha;
    if (appearanceChanged && !PaintSolid(line->hwnd, width, height, color, alpha)) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
        return;
    }

    bool moved = !line->valid || line->x != x || line->y != y || appearanceChanged ||
                 !g_app.shown;
    if (moved) {
        SetWindowPos(line->hwnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
    }

    line->x = x;
    line->y = y;
    line->w = width;
    line->h = height;
    line->color = color;
    line->alpha = alpha;
    line->valid = true;
}

void SyncReticle(int x, int y, int size, int thickness, COLORREF color, BYTE alpha) {
    ReticleCache* reticle = &g_app.reticle;
    thickness = ClampInt(thickness, 1, size / 2);
    bool appearanceChanged =
        !reticle->valid || reticle->size != size ||
        reticle->thickness != thickness || reticle->color != color ||
        reticle->alpha != alpha;
    if (appearanceChanged &&
        !PaintSquareOutline(reticle->hwnd, size, thickness, color, alpha)) {
        Wh_Log(L"Reticle UpdateLayeredWindow failed: %u", GetLastError());
        return;
    }

    bool moved = !reticle->valid || reticle->x != x || reticle->y != y ||
                 appearanceChanged || !reticle->shown;
    if (moved) {
        SetWindowPos(reticle->hwnd, HWND_TOPMOST, x, y, size, size,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
    }

    reticle->x = x;
    reticle->y = y;
    reticle->size = size;
    reticle->thickness = thickness;
    reticle->color = color;
    reticle->alpha = alpha;
    reticle->valid = true;
    reticle->shown = true;
}

void HideReticle() {
    if (!g_app.reticle.shown) {
        return;
    }
    ShowWindow(g_app.reticle.hwnd, SW_HIDE);
    g_app.reticle.shown = false;
}

void HideCrosshair() {
    if (g_app.shown) {
        ShowWindow(g_app.horizontal.hwnd, SW_HIDE);
        ShowWindow(g_app.vertical.hwnd, SW_HIDE);
        g_app.shown = false;
    }
    HideReticle();
}

void NudgeCursor() {
    // A one-pixel move makes the window under the pointer call SetCursor again,
    // so processes that already swapped in a blank cursor pick up the toggle.
    POINT point;
    if (!GetCursorPos(&point)) {
        return;
    }
    SetCursorPos(point.x + 1, point.y);
    SetCursorPos(point.x, point.y);
}

void UpdateCrosshair() {
    if (InterlockedExchange(&g_app.dirty, 0)) {
        LoadSettings();
    }
    if (g_app.suspended) {
        HideCrosshair();
        return;
    }
    MaintainBlankCursor();

    CURSORINFO cursorInfo = {};
    cursorInfo.cbSize = sizeof(cursorInfo);
    POINT point = {};
    bool cursorVisible = GetCursorInfo(&cursorInfo) && GetCursorPos(&point) &&
                         (cursorInfo.flags & CURSOR_SHOWING) != 0 &&
                         (cursorInfo.flags & 0x2) == 0;
    bool useCrosshair =
        cursorVisible && (g_app.settings.forceOverride ||
                          IsDefaultArrow(cursorInfo.hCursor));
    if (!useCrosshair) {
        HideCrosshair();
        return;
    }

    RECT monitor = {};
    HMONITOR monitorHandle = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitorHandle || !GetMonitorInfoW(monitorHandle, &monitorInfo)) {
        HideCrosshair();
        return;
    }
    monitor = monitorInfo.rcMonitor;

    int monitorWidth = monitor.right - monitor.left;
    int monitorHeight = monitor.bottom - monitor.top;
    if (monitorWidth < 1 || monitorHeight < 1) {
        HideCrosshair();
        return;
    }

    int hairWidth = ClampInt(MulDiv(monitorWidth, g_app.settings.length, 100), 1,
                             monitorWidth);
    int hairHeight = ClampInt(MulDiv(monitorHeight, g_app.settings.length, 100), 1,
                              monitorHeight);
    int thickness = g_app.settings.thickness;
    BYTE crosshairAlpha =
        OpacityPercentToAlpha(g_app.settings.crosshairOpacity);
    BYTE reticleAlpha = OpacityPercentToAlpha(g_app.settings.reticleOpacity);
    int horizontalX = PlaceSpan(point.x, monitor.left, monitorWidth, hairWidth);
    int verticalY = PlaceSpan(point.y, monitor.top, monitorHeight, hairHeight);

    SyncLine(&g_app.horizontal, horizontalX, point.y - thickness / 2, hairWidth,
             thickness, g_app.settings.color, crosshairAlpha);
    SyncLine(&g_app.vertical, point.x - thickness / 2, verticalY, thickness,
             hairHeight, g_app.settings.color, crosshairAlpha);
    g_app.shown = true;

    if (g_app.settings.reticle) {
        int size = g_app.settings.reticleSize;
        SyncReticle(point.x - size / 2, point.y - size / 2, size, thickness,
                    g_app.settings.color, reticleAlpha);
    } else {
        HideReticle();
    }
}

void SetSuspended(bool suspended) {
    if (g_app.suspended == suspended) {
        return;
    }
    g_app.suspended = suspended;
    PublishToggleState(suspended);
    Wh_Log(L"Crosshair %s", suspended ? L"off" : L"on");
    if (suspended) {
        HideCrosshair();
        RestoreSystemCursor();
    } else {
        ApplySystemCursors();
        UpdateCrosshair();
    }
    NudgeCursor();
}

LRESULT CALLBACK CrosshairWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    switch (message) {
        case WM_HOTKEY:
            if (wParam == kToggleHotkeyId) {
                SetSuspended(!g_app.suspended);
            }
            return 0;
        case WM_TIMER:
        case WM_APP_UPDATE:
        case WM_DISPLAYCHANGE:
            UpdateCrosshair();
            return 0;
        case WM_INPUT: {
            RAWINPUT input = {};
            UINT size = sizeof(input);
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &input,
                            &size, sizeof(RAWINPUTHEADER));
            UpdateCrosshair();
            return 0;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

HWND CreateCrosshairWindow(HINSTANCE instance) {
    return CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                               WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                           L"WindhawkCustomCursorCrosshair", L"", WS_POPUP, 0, 0,
                           1, 1, nullptr, nullptr, instance, nullptr);
}

DWORD WINAPI CrosshairThread(void*) {
    // Resolved at runtime so the same source links with the Windhawk 1.7 headers.
    using SetProcessDpiAwarenessContext_t = BOOL(WINAPI*)(void*);
    auto setDpiAwareness =
        reinterpret_cast<SetProcessDpiAwarenessContext_t>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetProcessDpiAwarenessContext"));
    if (!setDpiAwareness || !setDpiAwareness(reinterpret_cast<void*>(static_cast<LONG_PTR>(-4)))) {
        SetProcessDPIAware();
    }

    g_app.threadId = GetCurrentThreadId();
    MSG wake = {};
    PeekMessageW(&wake, nullptr, 0, 0, PM_NOREMOVE);

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = CrosshairWndProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = L"WindhawkCustomCursorCrosshair";
    if (!RegisterClassW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        SetEvent(g_app.readyEvent);
        return 1;
    }

    g_app.horizontal.hwnd = CreateCrosshairWindow(instance);
    g_app.vertical.hwnd = CreateCrosshairWindow(instance);
    g_app.reticle.hwnd = CreateCrosshairWindow(instance);
    if (!g_app.horizontal.hwnd || !g_app.vertical.hwnd ||
        !g_app.reticle.hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        SetEvent(g_app.readyEvent);
        return 1;
    }

    RAWINPUTDEVICE mouse = {};
    mouse.usUsagePage = 0x01;
    mouse.usUsage = 0x02;
    mouse.dwFlags = RIDEV_INPUTSINK;
    mouse.hwndTarget = g_app.horizontal.hwnd;
    if (!RegisterRawInputDevices(&mouse, 1, sizeof(mouse))) {
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
    }

    EnsureToggleState();
    LoadSettings();
    if (g_app.settings.hotkeyVk) {
        SetSuspended(IsCrosshairSuspended());
    } else {
        PublishToggleState(false);
    }
    if (!g_app.suspended) {
        ApplySystemCursors();
    }
    UpdateCrosshair();
    SetTimer(g_app.horizontal.hwnd, kTimerId, kTimerIntervalMs, nullptr);
    SetEvent(g_app.readyEvent);

    MSG message = {};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    KillTimer(g_app.horizontal.hwnd, kTimerId);
    if (g_app.hotkeyRegistered) {
        UnregisterHotKey(g_app.horizontal.hwnd, kToggleHotkeyId);
        g_app.hotkeyRegistered = false;
    }
    HideCrosshair();
    RestoreSystemCursor();
    DestroyWindow(g_app.horizontal.hwnd);
    DestroyWindow(g_app.vertical.hwnd);
    DestroyWindow(g_app.reticle.hwnd);
    g_app.horizontal.hwnd = nullptr;
    g_app.vertical.hwnd = nullptr;
    g_app.reticle.hwnd = nullptr;
    UnregisterClassW(L"WindhawkCustomCursorCrosshair", instance);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    WCHAR hostPath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, hostPath, ARRAYSIZE(hostPath));
    Wh_Log(L"Host %s", hostPath);

    g_app.readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_app.readyEvent) {
        return FALSE;
    }

    g_app.thread = CreateThread(nullptr, 0, CrosshairThread, nullptr, 0, nullptr);
    if (!g_app.thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    WaitForSingleObject(g_app.readyEvent, 10000);
    return g_app.horizontal.hwnd && g_app.vertical.hwnd && g_app.reticle.hwnd
               ? TRUE
               : FALSE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    InterlockedExchange(&g_app.dirty, 1);
    if (g_app.horizontal.hwnd) {
        PostMessageW(g_app.horizontal.hwnd, WM_APP_UPDATE, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_app.threadId) {
        PostThreadMessageW(g_app.threadId, WM_QUIT, 0, 0);
    }
    if (g_app.thread) {
        WaitForSingleObject(g_app.thread, 10000);
        CloseHandle(g_app.thread);
        g_app.thread = nullptr;
    }
    if (g_app.readyEvent) {
        CloseHandle(g_app.readyEvent);
        g_app.readyEvent = nullptr;
    }
    RestoreSystemCursor();
}

// Installed in every other process when the mod is loaded there. Browsers keep
// their own cursor handles, so replacing the system cursors is not enough.
using SetCursor_t = HCURSOR(WINAPI*)(HCURSOR);
SetCursor_t SetCursor_Original;
HCURSOR g_injectedBlank;
HCURSOR g_lastAppCursor;
volatile LONG g_forceOverride;
bool g_isInjectedHelper;

bool ExeNameIs(const wchar_t* exeName) {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        return false;
    }
    const WCHAR* base = path;
    for (const WCHAR* scan = path; *scan; scan++) {
        if (*scan == L'\\' || *scan == L'/') {
            base = scan + 1;
        }
    }
    return lstrcmpiW(base, exeName) == 0;
}

HCURSOR WINAPI SetCursor_Hook(HCURSOR cursor) {
    if (!IsCrosshairSuspended() &&
        InterlockedCompareExchange(&g_forceOverride, 0, 0) && cursor &&
        g_injectedBlank && cursor != g_injectedBlank) {
        g_lastAppCursor = cursor;
        cursor = g_injectedBlank;
    }
    return SetCursor_Original(cursor);
}

void ApplyInjectedOverride() {
    if (IsCrosshairSuspended() ||
        !InterlockedCompareExchange(&g_forceOverride, 0, 0) || !g_injectedBlank ||
        !SetCursor_Original) {
        return;
    }
    HCURSOR current = GetCursor();
    if (!current || current == g_injectedBlank) {
        return;
    }
    g_lastAppCursor = current;
    SetCursor_Original(g_injectedBlank);
}

BOOL WhInjected_ModInit() {
    g_isInjectedHelper = true;
    EnsureToggleState();
    InterlockedExchange(&g_forceOverride,
                        Wh_GetIntSetting(L"overrideAppCursors") != 0 ? 1 : 0);
    g_injectedBlank = CreateBlankCursor();
    if (!Wh_SetFunctionHook((void*)SetCursor, (void*)SetCursor_Hook,
                            (void**)&SetCursor_Original)) {
        Wh_Log(L"SetCursor hook failed");
    }
    return TRUE;
}

void WhInjected_ModAfterInit() {
    ApplyInjectedOverride();
}

void WhInjected_ModSettingsChanged() {
    InterlockedExchange(&g_forceOverride,
                        Wh_GetIntSetting(L"overrideAppCursors") != 0 ? 1 : 0);
    if (!g_injectedBlank) {
        g_injectedBlank = CreateBlankCursor();
    }
    if (g_forceOverride) {
        ApplyInjectedOverride();
        return;
    }
    if (g_lastAppCursor && GetCursor() == g_injectedBlank) {
        SetCursor(g_lastAppCursor);
    }
}

void WhInjected_ModUninit() {
    if (g_lastAppCursor && GetCursor() == g_injectedBlank) {
        InterlockedExchange(&g_forceOverride, 0);
        SetCursor(g_lastAppCursor);
    }
}

////////////////////////////////////////////////////////////////////////////////
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
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        // Windhawk 1.7 launches this process with -tool-mod. Windhawk 2.0's
        // tool host uses -windhawk-tool-mod. Accept either.
        if (wcscmp(argv[i], L"-tool-mod") == 0 ||
            wcscmp(argv[i], L"-windhawk-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    // Windhawk 1.7 loads this mod in windhawk.exe and that copy starts the
    // tool process. Every other process only overrides app cursor changes.
    if (ExeNameIs(L"windhawk.exe")) {
        g_isToolModProcessLauncher = true;
        return TRUE;
    }

    return WhInjected_ModInit();
}

void Wh_ModAfterInit() {
    if (g_isInjectedHelper) {
        WhInjected_ModAfterInit();
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                          1];
    // swprintf_s's size-deducing overload is not declared for every Windhawk
    // compiler target. Pass the buffer size to the standard wide swprintf.
    swprintf(commandLine, ARRAYSIZE(commandLine), L"\"%s\" -tool-mod \"%s\"",
             currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
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
    if (g_isToolModProcessLauncher) {
        return;
    }

    if (g_isInjectedHelper) {
        WhInjected_ModSettingsChanged();
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    if (g_isInjectedHelper) {
        WhInjected_ModUninit();
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
