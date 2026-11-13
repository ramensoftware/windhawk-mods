// ==WindhawkMod==
// @id              keyboard-layout-text-converter
// @name            Keyboard Layout Text Converter
// @description     Convert mistyped text in place with a shortcut, adapting to installed keyboard layouts
// @version         0.0.1
// @author          OMAR ALAFA
// @github          https://github.com/omar-devloop
// @homepage        https://github.com/omar-devloop/keyboard-layout-text-converter
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lkernel32 -lshell32 -lgdi32 -lgdiplus -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keyboard Layout Text Converter

Converts mistyped text directly in place using a global shortcut, across most
Windows applications.

## How it works
- Press the configured shortcut while text is selected (recommended), or use the
  last-word fallback mode. The default shortcut is Alt+X.
- A regular key combination is swallowed, and the conversion starts as soon as
  the keys are released. Modifier-only shortcuts such as Ctrl+Shift are also
  supported, but they can collide with other Windows actions (for example
  switching the keyboard layout), so a regular combination is recommended.
- The mod copies the target text, detects the likely source/target keyboard
  layout among the keyboard layouts installed on your device, converts the text
  by key positions between layouts, then pastes the converted text back.
- By default the previous clipboard content is restored after replacement, in
  all of its formats (text, images, files and so on). Formats are kept up to
  128 MB each (256 MB in total); if the clipboard holds something larger, or
  only formats that can't be copied, the conversion is skipped instead of
  losing it.
- An optional small success toast can be enabled in the settings; it appears in
  the bottom-right screen corner after each conversion.

## Examples
- Typed with English layout but meant as another installed layout (and back),
  depending on the active/installed layouts and text.
- Works best when at least two keyboard layouts/languages are installed.

## Similar mods
- Arabic Keyboard Layout Fixer converts between a fixed Arabic/English pair.
- Quick translator & Layout-switcher converts between a fixed English/Russian
  pair, with optional online translation.
Both use a fixed layout pair. This mod builds the mapping from every installed
layout, and adds a previous-word fallback, cycle mode, optional layout
switching and a toast.

## Notes and limitations
- Replacement is clipboard-based (copy/paste simulation), so behavior can vary
  in apps with custom clipboard/shortcut handling.
- In console windows (conhost, Windows Terminal, ConEmu/Cmder, mintty, WezTerm
  and Alacritty) the copy uses Ctrl+Insert instead of Ctrl+C, because Ctrl+C
  without a selection discards the typed line and interrupts a running command.
  The previous-word fallback is skipped there, and the converted text is left
  on the clipboard instead of being pasted: a terminal selection covers the
  screen buffer rather than the input line, so pasting would add a duplicate.
  Delete the original and paste manually; the previous clipboard content isn't
  restored in that case. Other terminals can still receive Ctrl+C, which may
  interrupt the running program.
- If the clipboard holds content that can't be preserved (for example a single
  format above 128 MB), the mod beeps and does not convert, so nothing is lost.
- Editors such as VS Code or Visual Studio copy the current line when nothing
  is selected, so pressing the shortcut without a selection can paste an extra
  line there; the selection-only scope doesn't avoid this.
- The shortcut is swallowed globally, which shadows apps that use it themselves
  (for example Microsoft Word binds Alt+X); change the shortcut if needed.
- The mod may not be able to interact with elevated (UIPI) windows, secure
  fields, or some terminals/remote apps.
- Very short or ambiguous text can be hard to auto-detect; use a longer
  selection or adjust settings if needed.
- Runs as a Windhawk tool mod in a dedicated `windhawk.exe` process.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: "Alt+X"
  $name: "Conversion shortcut"
  $description: "Global shortcut to convert text. A regular combination such as Alt+X is recommended; modifier-only combinations (e.g. LCtrl+LShift) are supported too, but they can collide with Windows actions such as switching the keyboard layout. Use L/R prefixes to pick a side."
- mode: auto
  $name: "Conversion mode"
  $options:
    - auto: "Auto-detect layouts"
    - cycle: "Cycle target among installed layouts"
  $description: "Auto-detect picks the most plausible layout mapping. Cycle tries the next installed layout."
- scope: selection-first
  $name: "Capture scope"
  $options:
    - selection-first: "Selection, fallback to previous word"
    - selection-only: "Selection only"
    - last-word: "Previous word only"
  $description: "What text to convert when the shortcut is pressed."
- restoreClipboard: true
  $name: "Restore clipboard"
  $description: "Restore the previous clipboard content, in all of its formats, after conversion. Formats above 128 MB (or 256 MB in total) can't be preserved, and the conversion is skipped in that case."
- showNotification: false
  $name: "Show notification"
  $description: "Show a small success toast in the screen corner after a successful conversion."
- switchLayout: false
  $name: "Switch keyboard layout after convert"
  $description: "Try to switch the active keyboard layout to the detected target layout."
- actionDelayMs: 150
  $name: "Clipboard wait (ms)"
  $description: "Time in milliseconds to wait for the clipboard after copy, and to wait after pasting before the clipboard is restored. Values below 150 are raised to 150. Increase it if conversions fail in slow applications."
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <gdiplus.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr UINT WM_APP_CONVERT = WM_APP + 0x51;
constexpr UINT WM_APP_UPDATE_MOUSE_HOOK = WM_APP + 0x52;

// A modifier-only shortcut held longer than this is treated as regular
// modifier usage rather than a shortcut, which keeps Ctrl+Shift+drag and
// similar accidental triggers from converting text.
constexpr ULONGLONG kHotkeyMaxHoldMs = 1500;

// Clipboard snapshot limits. A 4K/5K screenshot is 30-50 MB, so the per-format
// limit leaves plenty of headroom. If a restorable format still can't be saved,
// the conversion is skipped instead of destroying it.
constexpr SIZE_T kClipboardFormatLimit = 128 * 1024 * 1024;
constexpr SIZE_T kClipboardTotalLimit = 256 * 1024 * 1024;

struct Hotkey {
    UINT modifiers = 0;  // MOD_* | MOD_NOREPEAT (we ignore norepeat in parsing)
    UINT vk = 0;         // 0 for modifier-only shortcuts such as LCtrl+LShift
    bool valid = false;
    bool modifierOnly = false;
    // Specific left/right modifier vks for modifier-only shortcuts, indexed by
    // class (0=ctrl, 1=alt, 2=shift, 3=win). 0 means "either side".
    UINT modVk[4] = {};
};

struct Settings {
    std::wstring hotkeyStr = L"Alt+X";
    Hotkey hotkey;
    std::wstring mode = L"auto";
    std::wstring scope = L"selection-first";
    bool restoreClipboard = true;
    bool switchLayout = false;
    bool showNotification = false;
    int actionDelayMs = 150;
};

struct KeyStroke {
    UINT vk = 0;
    bool shift = false;
};

struct LayoutMap {
    std::wstring charsForKey[256][2];  // [vk][shift] -> produced char(s)
    std::unordered_map<wchar_t, KeyStroke> keyForChar;
    std::unordered_map<wchar_t, KeyStroke> keyForCharShift;
};

struct ClipboardItem {
    UINT format = 0;
    HGLOBAL handle = nullptr;
};

// Clipboard data is kept in GlobalAlloc blocks rather than in std::vector, so
// that in the 32-bit tool process a failed allocation of a large format (e.g. a
// 4K screenshot) returns NULL instead of throwing.
struct ClipboardSnapshot {
    DWORD seq = 0;
    bool captured = false;
    std::vector<ClipboardItem> items;

    ClipboardSnapshot() = default;
    ClipboardSnapshot(const ClipboardSnapshot&) = delete;
    ClipboardSnapshot& operator=(const ClipboardSnapshot&) = delete;
    ~ClipboardSnapshot() {
        for (ClipboardItem& item : items) {
            if (item.handle) GlobalFree(item.handle);
        }
    }
};

Settings g_settings;
Hotkey g_hotkeyForHook;
std::mutex g_settingsMutex;
std::mutex g_layoutMutex;

HHOOK g_keyboardHook = nullptr;
HHOOK g_mouseHook = nullptr;
HANDLE g_workerThread = nullptr;    // hook + message pump thread
DWORD g_workerThreadId = 0;
HANDLE g_convertThread = nullptr;   // conversion thread, kept separate so the
DWORD g_convertThreadId = 0;        // hook thread never blocks input dispatch
HANDLE g_stopEvent = nullptr;
ULONG_PTR g_cycleIndex = 0;
UINT g_suppressedHotkeyVk = 0;

// Combo-release tracking. Only touched on the hook thread.
bool g_keyDown[256] = {};
bool g_hotkeyPending = false;
ULONGLONG g_hotkeyArmTick = 0;

std::vector<HKL> g_layouts;
std::unordered_map<UINT_PTR, LayoutMap> g_layoutMaps;

// Toast notification state (conversion thread only).
HWND g_toastHwnd = nullptr;
ULONGLONG g_toastStartTick = 0;
bool g_toastClassRegistered = false;
wchar_t g_toastClass[64] = {};
HINSTANCE g_toastInstance = nullptr;
HDC g_toastMemDC = nullptr;
HBITMAP g_toastDib = nullptr;
HGDIOBJ g_toastOldDib = nullptr;
void* g_toastBits = nullptr;
BYTE g_toastAlpha = 255;
int g_toastPad = 0;
int g_toastCardW = 0;
int g_toastCardH = 0;
RECT g_toastCloseRect = {};

// GDI+ lifetime.
ULONG_PTR g_gdiplusToken = 0;
bool g_gdiplusReady = false;

std::wstring Trim(const std::wstring& s) {
    size_t a = 0;
    while (a < s.size() && iswspace(s[a])) a++;
    size_t b = s.size();
    while (b > a && iswspace(s[b - 1])) b--;
    return s.substr(a, b - a);
}

std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

UINT ParseVkToken(const std::wstring& tokenRaw) {
    std::wstring token = ToLower(Trim(tokenRaw));
    if (token.empty()) return 0;
    if (token.size() == 1) {
        wchar_t c = token[0];
        if (c >= L'a' && c <= L'z') return (UINT)(L'A' + (c - L'a'));
        if (c >= L'0' && c <= L'9') return (UINT)c;
        switch (c) {
            case L';': case L':': return VK_OEM_1;
            case L'=': case L'+': return VK_OEM_PLUS;
            case L',': case L'<': return VK_OEM_COMMA;
            case L'-': case L'_': return VK_OEM_MINUS;
            case L'.': case L'>': return VK_OEM_PERIOD;
            case L'/': case L'?': return VK_OEM_2;
            case L'`': case L'~': return VK_OEM_3;
            case L'[': case L'{': return VK_OEM_4;
            case L'\\': case L'|': return VK_OEM_5;
            case L']': case L'}': return VK_OEM_6;
            case L'\'': case L'"': return VK_OEM_7;
            default: return 0;
        }
    }
    if (token[0] >= L'0' && token[0] <= L'9') {
        DWORD value = wcstoul(token.c_str(), nullptr, 0);
        return (value >= 1 && value <= 0xFF) ? (UINT)value : 0;
    }
    if (token == L"space") return VK_SPACE;
    if (token == L"tab") return VK_TAB;
    if (token == L"enter" || token == L"return") return VK_RETURN;
    if (token == L"esc" || token == L"escape") return VK_ESCAPE;
    if (token == L"backspace" || token == L"bksp") return VK_BACK;
    if (token == L"delete" || token == L"del") return VK_DELETE;
    if (token == L"insert" || token == L"ins") return VK_INSERT;
    if (token == L"home") return VK_HOME;
    if (token == L"end") return VK_END;
    if (token == L"pageup" || token == L"pgup") return VK_PRIOR;
    if (token == L"pagedown" || token == L"pgdn") return VK_NEXT;
    if (token == L"left") return VK_LEFT;
    if (token == L"right") return VK_RIGHT;
    if (token == L"up") return VK_UP;
    if (token == L"down") return VK_DOWN;
    if (token == L"pause" || token == L"break") return VK_PAUSE;
    if (token == L"capslock" || token == L"caps") return VK_CAPITAL;
    if (token == L"scrolllock") return VK_SCROLL;
    if (token == L"numlock") return VK_NUMLOCK;
    if (token == L"printscreen" || token == L"prtsc" || token == L"snapshot") {
        return VK_SNAPSHOT;
    }
    if (token == L"apps" || token == L"contextmenu") return VK_APPS;
    if (token == L"semicolon") return VK_OEM_1;
    if (token == L"equals" || token == L"equal" || token == L"plus") {
        return VK_OEM_PLUS;
    }
    if (token == L"comma") return VK_OEM_COMMA;
    if (token == L"minus" || token == L"dash") return VK_OEM_MINUS;
    if (token == L"period" || token == L"dot") return VK_OEM_PERIOD;
    if (token == L"slash") return VK_OEM_2;
    if (token == L"backtick" || token == L"grave") return VK_OEM_3;
    if (token == L"lbracket" || token == L"bracketleft") return VK_OEM_4;
    if (token == L"backslash") return VK_OEM_5;
    if (token == L"rbracket" || token == L"bracketright") return VK_OEM_6;
    if (token == L"quote" || token == L"apostrophe") return VK_OEM_7;
    if (token.rfind(L"f", 0) == 0 && token.size() <= 3) {
        int n = _wtoi(token.c_str() + 1);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    return 0;
}

Hotkey ParseHotkey(const std::wstring& text) {
    Hotkey hk;
    UINT mods = 0;
    UINT vk = 0;
    bool badToken = false;
    auto addMod = [&](UINT mod, int idx, UINT sideVk) {
        mods |= mod;
        if (sideVk) hk.modVk[idx] = sideVk;
    };
    size_t start = 0;
    while (start <= text.size()) {
        size_t plus = text.find(L'+', start);
        std::wstring part = Trim(text.substr(start, plus == std::wstring::npos ? std::wstring::npos : plus - start));
        std::wstring low = ToLower(part);
        if (low.empty()) {
            // "Ctrl++" or a trailing "+" is a typo, not a modifier-only combo.
            badToken = true;
        } else if (low == L"ctrl" || low == L"control") {
            addMod(MOD_CONTROL, 0, 0);
        } else if (low == L"lctrl" || low == L"leftctrl" || low == L"leftcontrol") {
            addMod(MOD_CONTROL, 0, VK_LCONTROL);
        } else if (low == L"rctrl" || low == L"rightctrl" || low == L"rightcontrol") {
            addMod(MOD_CONTROL, 0, VK_RCONTROL);
        } else if (low == L"alt" || low == L"menu") {
            addMod(MOD_ALT, 1, 0);
        } else if (low == L"lalt" || low == L"leftalt") {
            addMod(MOD_ALT, 1, VK_LMENU);
        } else if (low == L"ralt" || low == L"rightalt") {
            addMod(MOD_ALT, 1, VK_RMENU);
        } else if (low == L"shift") {
            addMod(MOD_SHIFT, 2, 0);
        } else if (low == L"lshift" || low == L"leftshift") {
            addMod(MOD_SHIFT, 2, VK_LSHIFT);
        } else if (low == L"rshift" || low == L"rightshift") {
            addMod(MOD_SHIFT, 2, VK_RSHIFT);
        } else if (low == L"win" || low == L"windows" || low == L"meta") {
            addMod(MOD_WIN, 3, 0);
        } else if (low == L"lwin" || low == L"leftwin") {
            addMod(MOD_WIN, 3, VK_LWIN);
        } else if (low == L"rwin" || low == L"rightwin") {
            addMod(MOD_WIN, 3, VK_RWIN);
        } else {
            UINT parsed = ParseVkToken(part);
            if (parsed && !vk) {
                vk = parsed;
            } else {
                // Unknown key name or more than one non-modifier key.
                badToken = true;
            }
        }
        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }
    hk.modifiers = mods;
    hk.vk = vk;
    // An unknown or duplicate key name invalidates the whole shortcut; it must
    // not silently degrade to a modifier-only one.
    hk.modifierOnly = !badToken && vk == 0 && mods != 0;
    hk.valid = !badToken && ((vk != 0) || hk.modifierOnly);
    if (vk != 0) {
        // Side-specific info is only meaningful for modifier-only shortcuts.
        for (auto& m : hk.modVk) m = 0;
    }
    return hk;
}

void LoadSettings() {
    Settings s;
    WindhawkUtils::StringSetting hotkeySetting =
        WindhawkUtils::StringSetting::make(L"hotkey");
    if (*hotkeySetting.get()) s.hotkeyStr = hotkeySetting.get();

    WindhawkUtils::StringSetting modeSetting =
        WindhawkUtils::StringSetting::make(L"mode");
    if (*modeSetting.get()) s.mode = modeSetting.get();

    WindhawkUtils::StringSetting scopeSetting =
        WindhawkUtils::StringSetting::make(L"scope");
    if (*scopeSetting.get()) s.scope = scopeSetting.get();

    s.restoreClipboard = Wh_GetIntSetting(L"restoreClipboard") != 0;
    s.switchLayout = Wh_GetIntSetting(L"switchLayout") != 0;
    s.showNotification = Wh_GetIntSetting(L"showNotification") != 0;
    s.actionDelayMs = Wh_GetIntSetting(L"actionDelayMs");
    if (s.actionDelayMs < 150) s.actionDelayMs = 150;
    if (s.actionDelayMs > 2000) s.actionDelayMs = 2000;

    s.hotkey = ParseHotkey(s.hotkeyStr);

    Wh_Log(L"Settings: hotkey=%s valid=%d mode=%s scope=%s "
           L"restoreClipboard=%d switchLayout=%d showNotification=%d delay=%d",
           s.hotkeyStr.c_str(), s.hotkey.valid ? 1 : 0, s.mode.c_str(),
           s.scope.c_str(), s.restoreClipboard ? 1 : 0,
           s.switchLayout ? 1 : 0, s.showNotification ? 1 : 0,
           s.actionDelayMs);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
    g_hotkeyForHook = s.hotkey;
}

Settings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

Hotkey GetHotkeyForHook() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_hotkeyForHook;
}

bool IsExtendedVk(UINT vk) {
    switch (vk) {
        case VK_RMENU: case VK_RCONTROL: case VK_INSERT: case VK_DELETE:
        case VK_HOME: case VK_END: case VK_PRIOR: case VK_NEXT:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
        case VK_NUMLOCK: case VK_CANCEL: case VK_SNAPSHOT: case VK_DIVIDE:
        case VK_LWIN: case VK_RWIN: case VK_APPS:
            return true;
        default:
            return false;
    }
}

void SendKeyCombo(UINT vk, bool ctrl, bool shift, bool alt, bool win) {
    std::vector<INPUT> inputs;
    auto add = [&](WORD k, bool down, bool extended = false) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = k;
        in.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
        if (extended) in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        inputs.push_back(in);
    };
    if (ctrl) add(VK_CONTROL, true);
    if (shift) add(VK_SHIFT, true);
    if (alt) add(VK_MENU, true);
    if (win) add(VK_LWIN, true, true);
    add((WORD)vk, true, IsExtendedVk(vk));
    add((WORD)vk, false, IsExtendedVk(vk));
    if (win) add(VK_LWIN, false, true);
    if (alt) add(VK_MENU, false);
    if (shift) add(VK_SHIFT, false);
    if (ctrl) add(VK_CONTROL, false);
    if (!inputs.empty()) SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

bool WaitForClipboardChange(DWORD seqBefore, DWORD timeoutMs) {
    ULONGLONG start = GetTickCount64();
    for (;;) {
        if (GetClipboardSequenceNumber() != seqBefore) return true;
        if (GetTickCount64() - start >= timeoutMs) return false;
        Sleep(5);
    }
}

void RequestConversion() {
    if (g_convertThreadId) {
        PostThreadMessage(g_convertThreadId, WM_APP_CONVERT, 0, 0);
    }
}

bool AnyModifierDown() {
    return g_keyDown[VK_LCONTROL] || g_keyDown[VK_RCONTROL] || g_keyDown[VK_CONTROL] ||
           g_keyDown[VK_LMENU] || g_keyDown[VK_RMENU] || g_keyDown[VK_MENU] ||
           g_keyDown[VK_LSHIFT] || g_keyDown[VK_RSHIFT] || g_keyDown[VK_SHIFT] ||
           g_keyDown[VK_LWIN] || g_keyDown[VK_RWIN];
}

bool HotkeyModifiersMatch(const Hotkey& hk) {
    bool ctrl = g_keyDown[VK_LCONTROL] || g_keyDown[VK_RCONTROL] || g_keyDown[VK_CONTROL];
    bool alt = g_keyDown[VK_LMENU] || g_keyDown[VK_RMENU] || g_keyDown[VK_MENU];
    bool shift = g_keyDown[VK_LSHIFT] || g_keyDown[VK_RSHIFT] || g_keyDown[VK_SHIFT];
    bool win = g_keyDown[VK_LWIN] || g_keyDown[VK_RWIN];
    return ctrl == ((hk.modifiers & MOD_CONTROL) != 0) &&
           alt == ((hk.modifiers & MOD_ALT) != 0) &&
           shift == ((hk.modifiers & MOD_SHIFT) != 0) &&
           win == ((hk.modifiers & MOD_WIN) != 0);
}

bool IsModifierVk(UINT vk) {
    switch (vk) {
        case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL:
        case VK_LMENU: case VK_RMENU: case VK_MENU:
        case VK_LSHIFT: case VK_RSHIFT: case VK_SHIFT:
        case VK_LWIN: case VK_RWIN:
            return true;
        default:
            return false;
    }
}

// Modifier-only shortcut (e.g. LCtrl+LShift): exactly the configured modifier
// classes must be held, on the configured side when a side was specified.
bool ModifierOnlyHotkeyMatch(const Hotkey& hk) {
    bool ctrl = g_keyDown[VK_LCONTROL] || g_keyDown[VK_RCONTROL];
    bool alt = g_keyDown[VK_LMENU] || g_keyDown[VK_RMENU];
    bool shift = g_keyDown[VK_LSHIFT] || g_keyDown[VK_RSHIFT];
    bool win = g_keyDown[VK_LWIN] || g_keyDown[VK_RWIN];
    if (ctrl != ((hk.modifiers & MOD_CONTROL) != 0) ||
        alt != ((hk.modifiers & MOD_ALT) != 0) ||
        shift != ((hk.modifiers & MOD_SHIFT) != 0) ||
        win != ((hk.modifiers & MOD_WIN) != 0)) {
        return false;
    }
    for (UINT sideVk : hk.modVk) {
        if (sideVk && !g_keyDown[sideVk]) return false;
    }
    return true;
}

// True once every key of the combo (and any stray modifier) has been released.
bool HotkeyComboReleased(const Hotkey& hk) {
    if (hk.vk != 0 && hk.vk < 256 && g_keyDown[hk.vk]) return false;
    return !AnyModifierDown();
}

bool OpenClipboardWithRetry(HWND owner = nullptr) {
    ULONGLONG start = GetTickCount64();
    for (;;) {
        if (OpenClipboard(owner)) return true;
        if (GetTickCount64() - start >= 100) return false;
        Sleep(2);
    }
}

UINT GetExcludeClipboardFormat() {
    static UINT format =
        RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    return format;
}

// Keeps our own clipboard writes out of the Win+V history and Cloud Clipboard.
void AddClipboardExclusionMarker() {
    UINT format = GetExcludeClipboardFormat();
    if (!format) return;
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
    if (!h) return;
    DWORD* p = (DWORD*)GlobalLock(h);
    if (p) {
        *p = 1;
        GlobalUnlock(h);
    }
    if (!SetClipboardData(format, h)) GlobalFree(h);
}

// Formats which hold a GDI handle or private data must not be copied as a
// plain memory block.
bool IsSnapshotSkippedFormat(UINT format) {
    if (format == CF_BITMAP || format == CF_ENHMETAFILE ||
        format == CF_METAFILEPICT || format == CF_PALETTE ||
        format == CF_OWNERDISPLAY) {
        return true;
    }
    if (format >= CF_DSPTEXT && format <= CF_DSPENHMETAFILE) return true;
    if (format >= CF_PRIVATEFIRST && format <= CF_PRIVATELAST) return true;
    if (format >= CF_GDIOBJFIRST && format <= CF_GDIOBJLAST) return true;
    return false;
}

std::wstring GetClipboardUnicodeText() {
    std::wstring text;
    if (!OpenClipboardWithRetry()) return text;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t* p = (const wchar_t*)GlobalLock(h);
        if (p) {
            text = p;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return text;
}

// Copies the whole clipboard into the snapshot. Returns false if a restorable
// format couldn't be captured; the caller must then leave the clipboard alone,
// because the synthetic Ctrl+C would destroy content that can't be put back.
bool SaveClipboard(ClipboardSnapshot& snapshot) {
    if (!OpenClipboardWithRetry()) return false;

    snapshot.seq = GetClipboardSequenceNumber();

    SIZE_T total = 0;
    bool ok = true;
    bool skippedAny = false;
    for (UINT format = 0; (format = EnumClipboardFormats(format)) != 0;) {
        if (IsSnapshotSkippedFormat(format)) {
            // GDI handles and private formats can't be copied into a block.
            skippedAny = true;
            continue;
        }

        HANDLE h = GetClipboardData(format);
        if (!h) {
            // The owner has no data for this format. Synthesized aliases such
            // as CF_DIBV5 can report NULL, and there is nothing to lose by not
            // restoring a format that reports no data.
            Wh_Log(L"Clipboard format %u has no data", format);
            continue;
        }
        SIZE_T size = GlobalSize(h);
        if (size == 0) {
            // An empty block has nothing to restore, and failing the whole
            // snapshot over it would block every conversion while it's there.
            Wh_Log(L"Clipboard format %u is empty", format);
            continue;
        }
        if (size > kClipboardFormatLimit ||
            total + size > kClipboardTotalLimit) {
            Wh_Log(L"Clipboard format %u is too large (%zu bytes, %zu total)",
                   format, size, total);
            ok = false;
            break;
        }
        const void* src = GlobalLock(h);
        if (!src) {
            Wh_Log(L"Clipboard format %u could not be locked", format);
            ok = false;
            break;
        }
        HGLOBAL copy = GlobalAlloc(GMEM_MOVEABLE, size);
        void* dst = copy ? GlobalLock(copy) : nullptr;
        if (!dst) {
            GlobalUnlock(h);
            if (copy) GlobalFree(copy);
            Wh_Log(L"Out of memory saving clipboard format %u (%zu bytes)",
                   format, size);
            ok = false;
            break;
        }
        memcpy(dst, src, size);
        GlobalUnlock(copy);
        GlobalUnlock(h);
        snapshot.items.push_back(ClipboardItem{format, copy});
        total += size;
    }

    // Nothing restorable was found (e.g. a lone CF_BITMAP): restoring would
    // empty the clipboard, so treat it as a failure as well.
    if (ok && snapshot.items.empty() && skippedAny) {
        Wh_Log(L"Clipboard has no format that can be saved and restored");
        ok = false;
    }

    CloseClipboard();
    snapshot.captured = ok;
    return ok;
}

// Restores a snapshot, unless the clipboard was replaced in the meantime.
// Takes ownership of the saved blocks: SetClipboardData frees them, so handing
// them over avoids a second copy of every format (and of its peak memory).
void RestoreClipboard(ClipboardSnapshot& snapshot) {
    if (!snapshot.captured) return;
    // Nothing restorable: leave whatever is on the clipboard instead of
    // emptying it, so no data is lost.
    if (snapshot.items.empty()) return;
    if (GetClipboardSequenceNumber() == snapshot.seq) return;
    if (!OpenClipboardWithRetry()) return;

    EmptyClipboard();
    for (ClipboardItem& item : snapshot.items) {
        if (!item.handle) continue;
        if (SetClipboardData(item.format, item.handle)) {
            item.handle = nullptr;  // now owned by the clipboard
        } else {
            Wh_Log(L"Failed to restore clipboard format %u", item.format);
            GlobalFree(item.handle);
            item.handle = nullptr;
        }
    }
    AddClipboardExclusionMarker();
    CloseClipboard();
}

bool SetClipboardUnicodeText(const std::wstring& text) {
    // Never write an empty clipboard: "restoring" no text would wipe the
    // formats that aren't plain text.
    if (text.empty()) return false;
    if (!OpenClipboardWithRetry()) return false;
    EmptyClipboard();
    SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!h) {
        CloseClipboard();
        return false;
    }
    void* p = GlobalLock(h);
    if (p) {
        memcpy(p, text.c_str(), bytes);
        GlobalUnlock(h);
    }
    bool ok = SetClipboardData(CF_UNICODETEXT, h) != nullptr;
    if (ok) {
        AddClipboardExclusionMarker();
    } else {
        GlobalFree(h);
    }
    CloseClipboard();
    return ok;
}

void BuildLayoutMap(HKL hkl, LayoutMap& map) {
    map = {};
    for (int shift = 0; shift <= 1; shift++) {
        BYTE state[256] = {};
        if (shift) state[VK_SHIFT] = 0x80;
        for (UINT vk = 0x20; vk < 256; vk++) {
            if (vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
                vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
                vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
                vk == VK_LWIN || vk == VK_RWIN || vk == VK_APPS ||
                vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL ||
                vk == VK_PROCESSKEY || vk == VK_SNAPSHOT) {
                continue;
            }
            // Focus on keys likely to produce characters.
            if ((vk > 0x5A && vk < 0x60) || (vk > 0x6F && vk < 0xBA) || vk > 0xC0 || (vk >= 0x30 && vk <= 0x39) || (vk >= 0x41 && vk <= 0x5A) || vk == VK_SPACE || (vk >= VK_OEM_1 && vk <= VK_OEM_3) || (vk >= VK_OEM_4 && vk <= VK_OEM_8) || (vk >= VK_OEM_102 && vk <= VK_OEM_102)) {
                // allowed range candidates
            } else {
                continue;
            }

            UINT scan = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC_EX, hkl);
            WCHAR buf[8] = {};
            BYTE localState[256];
            memcpy(localState, state, sizeof(localState));
            int rc = ToUnicodeEx(vk, scan, localState, buf, 8, 0, hkl);
            if (rc == -1) {
                // Flush pending dead-key state on the local state, not a shared one.
                WCHAR dummy[8] = {};
                ToUnicodeEx(VK_SPACE, MapVirtualKeyEx(VK_SPACE, MAPVK_VK_TO_VSC_EX, hkl), localState, dummy, 8, 0, hkl);
                continue;
            }
            if (rc > 0) {
                map.charsForKey[vk][shift] = std::wstring(1, buf[0]);
            }
        }
    }

    // The reverse maps are built after both tables are filled, so that
    // uppercase aliases pick the keystroke that actually produces them. For
    // example, an uppercase letter whose shifted output is punctuation must
    // keep Shift, otherwise the punctuation converts to the unshifted symbol.
    for (int shift = 0; shift <= 1; shift++) {
        const int otherShift = shift ? 0 : 1;
        for (UINT vk = 0x20; vk < 256; vk++) {
            const std::wstring& produced = map.charsForKey[vk][shift];
            if (produced.size() != 1) continue;
            wchar_t ch = produced[0];
            auto add = [&](wchar_t c, bool needsShift) {
                auto& rev = needsShift ? map.keyForCharShift : map.keyForChar;
                if (rev.find(c) == rev.end()) rev[c] = KeyStroke{vk, needsShift};
            };
            add(ch, shift != 0);
            if (iswalpha(ch)) {
                wchar_t other = shift ? (wchar_t)towlower(ch)
                                      : (wchar_t)towupper(ch);
                const std::wstring& otherProduced =
                    map.charsForKey[vk][otherShift];
                // Keep Shift when this key actually produces the alias only
                // with the other Shift state. For the unshifted pass the other
                // state is shifted; for the shifted pass it is unshifted.
                bool otherMatches =
                    otherProduced.size() == 1 && otherProduced[0] == other;
                add(other, otherMatches ? (otherShift != 0) : (shift != 0));
            }
        }
    }
}

// Caller must hold g_layoutMutex.
void EnsureLayoutMapsBuiltLocked() {
    for (HKL hkl : g_layouts) {
        UINT_PTR key = (UINT_PTR)hkl;
        if (g_layoutMaps.find(key) == g_layoutMaps.end()) {
            LayoutMap map;
            BuildLayoutMap(hkl, map);
            g_layoutMaps.emplace(key, std::move(map));
        }
    }
}

bool RefreshLayoutsIfNeeded(bool forceRebuild = false) {
    std::lock_guard<std::mutex> lock(g_layoutMutex);
    int count = GetKeyboardLayoutList(0, nullptr);
    if (count <= 0) return false;
    std::vector<HKL> layouts(count);
    count = GetKeyboardLayoutList(count, layouts.data());
    if (count <= 0) return false;
    layouts.resize(count);

    bool changed = forceRebuild || (layouts.size() != g_layouts.size());
    if (!changed) {
        for (size_t i = 0; i < layouts.size(); i++) {
            if (layouts[i] != g_layouts[i]) { changed = true; break; }
        }
    }
    if (changed) {
        g_layouts = std::move(layouts);
        g_layoutMaps.clear();
    }
    if (g_layouts.empty()) return false;
    EnsureLayoutMapsBuiltLocked();
    return true;
}

// Caller must hold g_layoutMutex.
LayoutMap& GetOrBuildLayoutMapLocked(HKL hkl) {
    UINT_PTR key = (UINT_PTR)hkl;
    auto it = g_layoutMaps.find(key);
    if (it != g_layoutMaps.end()) return it->second;
    LayoutMap map;
    BuildLayoutMap(hkl, map);
    auto [insIt, _] = g_layoutMaps.emplace(key, std::move(map));
    return insIt->second;
}

HKL GetForegroundKeyboardLayout() {
    HWND fg = GetForegroundWindow();
    if (!fg) return GetKeyboardLayout(0);
    DWORD tid = GetWindowThreadProcessId(fg, nullptr);
    if (!tid) return GetKeyboardLayout(0);
    HKL hkl = GetKeyboardLayout(tid);
    return hkl ? hkl : GetKeyboardLayout(0);
}

bool IsLetterLike(wchar_t c) { return iswalpha((wint_t)c) != 0; }

int ScriptBucket(wchar_t c) {
    if ((c >= 0x0041 && c <= 0x024F) || (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) return 1; // Latin-ish
    if (c >= 0x0400 && c <= 0x052F) return 2; // Cyrillic
    if (c >= 0x0590 && c <= 0x05FF) return 3; // Hebrew
    if (c >= 0x0600 && c <= 0x06FF) return 4; // Arabic
    if (c >= 0x0370 && c <= 0x03FF) return 5; // Greek
    return 0;
}

int LayoutScriptHint(HKL hkl) {
    LANGID lang = LOWORD((UINT_PTR)hkl);
    WORD primary = PRIMARYLANGID(lang);
    switch (primary) {
        case LANG_RUSSIAN: case LANG_UKRAINIAN: case LANG_BELARUSIAN: case LANG_BULGARIAN:
            return 2;
        case LANG_HEBREW:
            return 3;
        case LANG_ARABIC: case LANG_PERSIAN: case LANG_URDU:
            return 4;
        case LANG_GREEK:
            return 5;
        default:
            return 1; // default Latin bucket
    }
}

// Caller must hold g_layoutMutex.
std::wstring ConvertUsingPair(const std::wstring& text, HKL source, HKL target, int& score) {
    LayoutMap& src = GetOrBuildLayoutMapLocked(source);
    LayoutMap& dst = GetOrBuildLayoutMapLocked(target);
    std::wstring out;
    out.reserve(text.size());
    score = 0;
    int convertible = 0;
    int targetScript = LayoutScriptHint(target);
    for (wchar_t c : text) {
        const KeyStroke* ksPtr = nullptr;
        auto it = src.keyForChar.find(c);
        if (it != src.keyForChar.end()) ksPtr = &it->second;
        if (!ksPtr) {
            auto it2 = src.keyForCharShift.find(c);
            if (it2 != src.keyForCharShift.end()) ksPtr = &it2->second;
        }
        if (ksPtr) {
            const KeyStroke& ks = *ksPtr;
            const std::wstring& mapped = dst.charsForKey[ks.vk][ks.shift ? 1 : 0];
            if (!mapped.empty()) {
                wchar_t outCh = mapped[0];
                // Preserve basic letter case when possible.
                if (IsLetterLike(c) && IsLetterLike(outCh)) {
                    if (iswupper(c)) outCh = (wchar_t)towupper(outCh);
                    else if (iswlower(c)) outCh = (wchar_t)towlower(outCh);
                }
                out += outCh;
                convertible++;
                score += 2;
                int sb = ScriptBucket(outCh);
                if (sb == 0 || sb == targetScript) score += 1;
                continue;
            }
        }
        out += c;
        if (!IsLetterLike(c)) score += 1;
    }
    // Prefer conversions that actually change a meaningful portion of convertible text.
    if (out != text) score += convertible * 2;
    else score -= 5;
    return out;
}

bool ActivateTargetKeyboardLayout(HKL hkl) {
    if (!hkl) return false;
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    return PostMessage(fg, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl) != FALSE;
}

bool ConvertTextAutoOrCycle(const std::wstring& input, const Settings& s, std::wstring& output, HKL& chosenTarget) {
    output.clear();
    chosenTarget = nullptr;
    std::lock_guard<std::mutex> lock(g_layoutMutex);
    if (input.empty() || g_layouts.size() < 2) return false;

    // Ensure maps exist (cheap if cached).
    for (HKL hkl : g_layouts) GetOrBuildLayoutMapLocked(hkl);

    HKL active = GetForegroundKeyboardLayout();

    if (s.mode == L"cycle") {
        // Pick a source layout that best "owns" the input characters, then cycle target.
        HKL src = active ? active : g_layouts[0];
        int bestSrcScore = -1000000;
        for (HKL cand : g_layouts) {
            LayoutMap& m = GetOrBuildLayoutMapLocked(cand);
            int sc = 0;
            for (wchar_t c : input) {
                if (m.keyForChar.find(c) != m.keyForChar.end() || m.keyForCharShift.find(c) != m.keyForCharShift.end()) sc += 2;
                else if (!IsLetterLike(c)) sc += 1;
            }
            if (cand == active) sc += 1;
            if (sc > bestSrcScore) { bestSrcScore = sc; src = cand; }
        }
        size_t start = 0;
        for (size_t i = 0; i < g_layouts.size(); i++) if (g_layouts[i] == src) { start = i; break; }
        size_t idx = (start + 1 + (g_cycleIndex % (g_layouts.size() - 1))) % g_layouts.size();
        if (idx == start) idx = (idx + 1) % g_layouts.size();
        g_cycleIndex++;
        HKL target = g_layouts[idx];
        int score = 0;
        output = ConvertUsingPair(input, src, target, score);
        chosenTarget = target;
        return output != input;
    }

    int bestScore = -1000000;
    std::wstring bestOut;
    HKL bestTarget = nullptr;

    auto scriptMatchScore = [&](HKL src, HKL dst, const std::wstring& outText) {
        int s = 0;
        int srcHint = LayoutScriptHint(src);
        int dstHint = LayoutScriptHint(dst);
        for (wchar_t c : input) {
            int sb = ScriptBucket(c);
            if (sb == 0) continue;
            if (sb == srcHint) s += 1;
            if (sb == dstHint) s -= 1;
        }
        for (wchar_t c : outText) {
            int sb = ScriptBucket(c);
            if (sb == 0) continue;
            if (sb == dstHint) s += 2;
            if (sb == srcHint) s -= 1;
        }
        if (src == active) s += 1;
        return s;
    };

    for (HKL src : g_layouts) {
        for (HKL dst : g_layouts) {
            if (src == dst) continue;
            int score = 0;
            std::wstring out = ConvertUsingPair(input, src, dst, score);
            if (out.empty() || out == input) continue;
            score += scriptMatchScore(src, dst, out);
            if (score > bestScore) {
                bestScore = score;
                bestOut = std::move(out);
                bestTarget = dst;
            }
        }
    }

    if (bestOut.empty() || bestOut == input) return false;
    output = std::move(bestOut);
    chosenTarget = bestTarget;
    return true;
}

// Safety net: conversion is normally requested on combo release already.
void WaitForModifierKeysRelease(DWORD timeoutMs = 250) {
    DWORD start = GetTickCount();
    for (;;) {
        bool down = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                    (GetAsyncKeyState(VK_MENU) & 0x8000) ||
                    (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                    (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
                    (GetAsyncKeyState(VK_RWIN) & 0x8000);
        if (!down) return;
        if (GetTickCount() - start > timeoutMs) return;
        Sleep(10);
    }
}

constexpr const wchar_t* TOAST_MESSAGE = L"Text converted";
constexpr UINT_PTR TOAST_TIMER_ID = 1;
constexpr UINT TOAST_TIMER_MS = 16;
constexpr ULONGLONG TOAST_HOLD_MS = 1100;
constexpr ULONGLONG TOAST_FADE_OUT_MS = 240;

struct ToastLayout {
    float scale = 1.0f;
    int width = 0;
    int height = 0;
    int pad = 0;
    int cardW = 0;
    int cardH = 0;
    int iconRadius = 0;
    int iconCx = 0;
    int iconCy = 0;
    int textLeft = 0;
    int textWidth = 0;
    int closeCx = 0;
    int closeCy = 0;
    int closeHalf = 0;
};

int ToastPx(int value, float scale) {
    return (int)(value * scale + 0.5f);
}

void AddRoundedRect(Gdiplus::GraphicsPath& path, const Gdiplus::RectF& rect, float radius) {
    float d = radius * 2.0f;
    if (d > rect.Width) d = rect.Width;
    if (d > rect.Height) d = rect.Height;
    if (d <= 0.0f) {
        path.AddRectangle(rect);
        return;
    }
    path.AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rect.GetRight() - d, rect.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rect.GetRight() - d, rect.GetBottom() - d, d, d, 0.0f, 90.0f);
    path.AddArc(rect.X, rect.GetBottom() - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

// Paints the toast into a 32bpp premultiplied-alpha bitmap:
// white pill card, thin #e5e7eb border, soft rgba(0,0,0,0.05) shadow,
// black circular check badge (#111827), #4b5563 text and a #6b7280 close icon.
void RenderToast(Gdiplus::Bitmap& bitmap, const ToastLayout& layout) {
    Gdiplus::Graphics g(&bitmap);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    const float pad = (float)layout.pad;
    const Gdiplus::RectF card(pad, pad, (float)layout.cardW, (float)layout.cardH);
    const float radius = card.Height / 2.0f;  // pill shape

    // Soft shadow: stacked low-alpha rounded layers approximating rgba(0,0,0,0.05).
    for (int i = 8; i >= 1; --i) {
        Gdiplus::RectF shadow(card.X - i, card.Y - i + 2.0f * layout.scale,
                              card.Width + i * 2.0f, card.Height + i * 2.0f);
        Gdiplus::GraphicsPath shadowPath;
        AddRoundedRect(shadowPath, shadow, radius + i);
        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(2, 0, 0, 0));
        g.FillPath(&shadowBrush, &shadowPath);
    }

    // White card with a thin neutral border.
    Gdiplus::RectF borderRect(card.X + 0.5f, card.Y + 0.5f, card.Width - 1.0f,
                              card.Height - 1.0f);
    Gdiplus::GraphicsPath cardPath;
    AddRoundedRect(cardPath, borderRect, radius - 0.5f);
    Gdiplus::SolidBrush cardBrush(Gdiplus::Color(255, 255, 255, 255));
    g.FillPath(&cardBrush, &cardPath);
    Gdiplus::Pen borderPen(Gdiplus::Color(255, 229, 231, 235), 1.0f);
    g.DrawPath(&borderPen, &cardPath);

    // Black circular success badge with a white check mark.
    const float iconR = (float)layout.iconRadius;
    const float iconCx = (float)layout.iconCx;
    const float iconCy = (float)layout.iconCy;
    Gdiplus::SolidBrush iconBrush(Gdiplus::Color(255, 17, 24, 39));  // #111827
    g.FillEllipse(&iconBrush, iconCx - iconR, iconCy - iconR, iconR * 2.0f, iconR * 2.0f);

    Gdiplus::Pen checkPen(Gdiplus::Color(255, 255, 255, 255),
                          2.0f * layout.scale > 1.0f ? 2.0f * layout.scale : 1.0f);
    checkPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
    checkPen.SetLineJoin(Gdiplus::LineJoinRound);
    Gdiplus::PointF check[3] = {
        Gdiplus::PointF(iconCx - iconR * 0.46f, iconCy + iconR * 0.02f),
        Gdiplus::PointF(iconCx - iconR * 0.14f, iconCy + iconR * 0.36f),
        Gdiplus::PointF(iconCx + iconR * 0.48f, iconCy - iconR * 0.36f),
    };
    g.DrawLines(&checkPen, check, 3);

    // Body text (#4b5563).
    Gdiplus::Font font(L"Segoe UI", 14.0f * layout.scale, Gdiplus::FontStyleRegular,
                       Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 75, 85, 99));  // #4b5563
    Gdiplus::StringFormat textFormat;
    textFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    textFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    textFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    textFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::RectF textRect((float)layout.textLeft, pad, (float)layout.textWidth,
                            (float)layout.cardH);
    g.DrawString(TOAST_MESSAGE, -1, &font, textRect, &textFormat, &textBrush);

    // Close icon (#6b7280).
    const float closeCx = (float)layout.closeCx;
    const float closeCy = (float)layout.closeCy;
    const float closeHalf = (float)layout.closeHalf;
    Gdiplus::Pen closePen(Gdiplus::Color(255, 107, 114, 128),
                          1.5f * layout.scale > 1.0f ? 1.5f * layout.scale : 1.0f);
    closePen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
    g.DrawLine(&closePen, closeCx - closeHalf, closeCy - closeHalf, closeCx + closeHalf,
               closeCy + closeHalf);
    g.DrawLine(&closePen, closeCx + closeHalf, closeCy - closeHalf, closeCx - closeHalf,
               closeCy + closeHalf);
}

void DestroyToastResources() {    if (g_toastMemDC) {
        if (g_toastOldDib) {
            SelectObject(g_toastMemDC, g_toastOldDib);
            g_toastOldDib = nullptr;
        }
        DeleteDC(g_toastMemDC);
        g_toastMemDC = nullptr;
    }
    if (g_toastDib) {
        DeleteObject(g_toastDib);
        g_toastDib = nullptr;
    }
    g_toastBits = nullptr;
    g_toastAlpha = 255;
}

void StopGdiplus() {
    if (g_gdiplusReady) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        g_gdiplusReady = false;
    }
}

bool PointInToastCard(POINT pt) {
    if (g_toastCardW <= 0 || g_toastCardH <= 0) return false;
    const int left = g_toastPad;
    const int top = g_toastPad;
    const int right = left + g_toastCardW;
    const int bottom = top + g_toastCardH;
    if (pt.x < left || pt.x >= right || pt.y < top || pt.y >= bottom) return false;

    const int radius = g_toastCardH / 2;
    int cx = -1;
    int cy = -1;
    if (pt.x < left + radius) {
        cx = left + radius;
    } else if (pt.x >= right - radius) {
        cx = right - radius - 1;
    }
    if (pt.y < top + radius) {
        cy = top + radius;
    } else if (pt.y >= bottom - radius) {
        cy = bottom - radius - 1;
    }
    if (cx < 0 || cy < 0) return true;

    const int dx = pt.x - cx;
    const int dy = pt.y - cy;
    return dx * dx + dy * dy <= radius * radius;
}

void UpdateToastAlpha(HWND hwnd, BYTE alpha) {
    if (!g_toastMemDC) return;
    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) return;
    POINT dst = {rc.left, rc.top};
    SIZE size = {rc.right - rc.left, rc.bottom - rc.top};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, nullptr, &dst, &size, g_toastMemDC, &src, 0, &blend, ULW_ALPHA);
}

LRESULT CALLBACK ToastWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST: {
            POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
            ScreenToClient(hwnd, &pt);
            // Only the visible card is clickable; the shadow area is click-through.
            return PointInToastCard(pt) ? HTCLIENT : HTTRANSPARENT;
        }
        case WM_SETCURSOR: {
            POINT pt = {};
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            bool overClose = pt.x >= g_toastCloseRect.left && pt.x < g_toastCloseRect.right &&
                             pt.y >= g_toastCloseRect.top && pt.y < g_toastCloseRect.bottom;
            SetCursor(LoadCursorW(nullptr, overClose ? IDC_HAND : IDC_ARROW));
            return TRUE;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            DestroyWindow(hwnd);
            return 0;
        case WM_TIMER:
            if (wParam == TOAST_TIMER_ID) {
                const ULONGLONG elapsed = GetTickCount64() - g_toastStartTick;
                const ULONGLONG end = TOAST_HOLD_MS + TOAST_FADE_OUT_MS;
                BYTE alpha;
                if (elapsed < TOAST_HOLD_MS) {
                    alpha = 255;
                } else if (elapsed < end) {
                    alpha = (BYTE)(255 * (end - elapsed) / TOAST_FADE_OUT_MS);
                } else {
                    KillTimer(hwnd, TOAST_TIMER_ID);
                    DestroyWindow(hwnd);
                    return 0;
                }
                if (alpha != g_toastAlpha) {
                    g_toastAlpha = alpha;
                    UpdateToastAlpha(hwnd, alpha);
                }
            }
            return 0;
        case WM_DESTROY:
            DestroyToastResources();
            if (g_toastHwnd == hwnd) {
                g_toastHwnd = nullptr;
            }
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

// Fixed corner success toast, independent of the active window.
void ShowToast() {
    if (!g_gdiplusReady) {
        Wh_Log(L"Toast skipped: GDI+ is not initialized");
        return;
    }

    if (g_toastHwnd) {
        DestroyWindow(g_toastHwnd);
        g_toastHwnd = nullptr;
    }

    if (!g_toastClassRegistered) {
        HINSTANCE inst = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ToastWndProc), &inst);
        if (!inst) {
            inst = GetModuleHandleW(nullptr);
        }
        g_toastInstance = inst;

        // Unique per load, so reloading the mod can never collide with a stale class.
        _snwprintf_s(g_toastClass, _countof(g_toastClass), _TRUNCATE,
                     L"WindhawkKLTC_Toast_%llx", (unsigned long long)GetTickCount64());

        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = ToastWndProc;
        wc.hInstance = inst;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = g_toastClass;
        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"RegisterClassEx(toast) failed: %u", GetLastError());
            return;
        }
        g_toastClassRegistered = true;
        Wh_Log(L"Toast class registered: %s", g_toastClass);
    }

    // The toast follows the monitor under the cursor, so use that monitor's DPI.
    POINT cursor = {};
    GetCursorPos(&cursor);
    HMONITOR mon = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);

    float scale = 1.0f;
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (mon && SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX >= 48) {
        scale = (float)dpiX / 96.0f;
    } else {
        UINT dpi = GetDpiForSystem();
        if (dpi >= 48) {
            scale = (float)dpi / 96.0f;
        }
    }

    // Measure the body text with the exact font used for rendering.
    float measuredText = 0.0f;
    {
        Gdiplus::Font font(L"Segoe UI", 14.0f * scale, Gdiplus::FontStyleRegular,
                           Gdiplus::UnitPixel);
        HDC screenDC = GetDC(nullptr);
        if (screenDC) {
            {
                Gdiplus::Graphics measure(screenDC);
                measure.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
                Gdiplus::RectF bounds;
                measure.MeasureString(TOAST_MESSAGE, -1, &font, Gdiplus::PointF(0.0f, 0.0f),
                                      &bounds);
                measuredText = bounds.Width;
            }
            ReleaseDC(nullptr, screenDC);
        }
    }

    ToastLayout layout;
    layout.scale = scale;
    layout.pad = ToastPx(12, scale);
    layout.cardH = ToastPx(48, scale);
    layout.iconRadius = ToastPx(9, scale);

    const int leftInset = ToastPx(15, scale);
    const int iconGap = ToastPx(12, scale);
    const int closeGap = ToastPx(12, scale);
    const int closeSize = ToastPx(10, scale);
    const int rightInset = ToastPx(16, scale);

    layout.cardW = leftInset + layout.iconRadius * 2 + iconGap + (int)(measuredText + 0.5f) +
                   closeGap + closeSize + rightInset;
    layout.width = layout.cardW + layout.pad * 2;
    layout.height = layout.cardH + layout.pad * 2;

    layout.iconCx = layout.pad + leftInset + layout.iconRadius;
    layout.iconCy = layout.pad + layout.cardH / 2;
    layout.textLeft = layout.iconCx + layout.iconRadius + iconGap;
    layout.textWidth = layout.pad + layout.cardW - rightInset - closeSize - closeGap -
                       layout.textLeft;
    layout.closeCx = layout.pad + layout.cardW - rightInset - closeSize / 2;
    layout.closeCy = layout.iconCy;
    layout.closeHalf = closeSize / 2;

    // Bottom-right corner of the monitor under the cursor.
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);

    const int margin = ToastPx(12, scale);
    int x = mi.rcWork.right - layout.width - margin;
    int y = mi.rcWork.bottom - layout.height - margin;
    if (x < mi.rcWork.left + margin) x = mi.rcWork.left + margin;
    if (y < mi.rcWork.top + margin) y = mi.rcWork.top + margin;

    // Offscreen 32bpp surface for per-pixel alpha (UpdateLayeredWindow).
    HDC screenDC = GetDC(nullptr);
    if (!screenDC) {
        Wh_Log(L"Toast: GetDC failed");
        return;
    }
    g_toastMemDC = CreateCompatibleDC(screenDC);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = layout.width;
    bmi.bmiHeader.biHeight = -layout.height;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    g_toastDib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &g_toastBits, nullptr, 0);
    ReleaseDC(nullptr, screenDC);
    if (!g_toastMemDC || !g_toastDib || !g_toastBits) {
        Wh_Log(L"Toast DIB creation failed: %u", GetLastError());
        DestroyToastResources();
        return;
    }
    g_toastOldDib = SelectObject(g_toastMemDC, g_toastDib);
    if (!g_toastOldDib) {
        Wh_Log(L"Toast SelectObject failed: %u", GetLastError());
        DestroyToastResources();
        return;
    }

    {
        Gdiplus::Bitmap bitmap(layout.width, layout.height, layout.width * 4,
                               PixelFormat32bppPARGB, (BYTE*)g_toastBits);
        if (bitmap.GetLastStatus() != Gdiplus::Ok) {
            Wh_Log(L"Toast bitmap creation failed");
            DestroyToastResources();
            return;
        }
        RenderToast(bitmap, layout);
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_toastClass, L"",
        WS_POPUP, x, y, layout.width, layout.height, nullptr, nullptr, g_toastInstance,
        nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx(toast) failed: %u", GetLastError());
        DestroyToastResources();
        return;
    }

    POINT dst = {x, y};
    SIZE size = {layout.width, layout.height};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(hwnd, nullptr, &dst, &size, g_toastMemDC, &src, 0, &blend,
                             ULW_ALPHA)) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }

    g_toastHwnd = hwnd;
    g_toastAlpha = 255;
    g_toastPad = layout.pad;
    g_toastCardW = layout.cardW;
    g_toastCardH = layout.cardH;
    const int closeHitPad = ToastPx(6, scale);
    g_toastCloseRect = {layout.closeCx - layout.closeHalf - closeHitPad,
                        layout.closeCy - layout.closeHalf - closeHitPad,
                        layout.closeCx + layout.closeHalf + closeHitPad,
                        layout.closeCy + layout.closeHalf + closeHitPad};

    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(hwnd);
    g_toastStartTick = GetTickCount64();
    SetTimer(hwnd, TOAST_TIMER_ID, TOAST_TIMER_MS, nullptr);
    Wh_Log(L"Toast shown at %d,%d (%dx%d)", x, y, layout.width, layout.height);
}

bool IsConsoleWindow(HWND hwnd) {
    if (!hwnd) return false;
    WCHAR className[64] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) return false;
    // Terminals where a selection lives on the screen buffer rather than in the
    // input line, and where Ctrl+C can reach the running program. Copying uses
    // Ctrl+Insert there; terminals without a Ctrl+Insert binding simply leave
    // nothing on the clipboard instead of sending ^C.
    static const wchar_t* const kConsoleClasses[] = {
        L"ConsoleWindowClass",             // conhost
        L"CASCADIA_HOSTING_WINDOW_CLASS",  // Windows Terminal
        L"VirtualConsoleClass",            // ConEmu / Cmder
        L"mintty",                         // Git Bash / Cygwin
        L"org.wezfurlong.wezterm",         // WezTerm
        L"Alacritty",                      // Alacritty
    };
    for (const wchar_t* name : kConsoleClasses) {
        if (_wcsicmp(className, name) == 0) return true;
    }
    return false;
}

void PerformConversion() {
    ULONGLONG tStart = GetTickCount64();
    Wh_Log(L"PerformConversion start");
    Settings s = GetSettingsCopy();
    if (!s.hotkey.valid) {
        Wh_Log(L"Hotkey is not valid: %s", s.hotkeyStr.c_str());
        return;
    }

    // Normally a no-op: conversion is requested once the combo is released.
    WaitForModifierKeysRelease();
    ULONGLONG tWaitDone = GetTickCount64();

    // Pick up layouts added/removed at runtime; cheap when unchanged.
    RefreshLayoutsIfNeeded();

    // Snapshot before the synthetic copy replaces the clipboard. If anything
    // can't be saved, skip the conversion rather than destroy it.
    ClipboardSnapshot snapshot;
    if (s.restoreClipboard && !SaveClipboard(snapshot)) {
        Wh_Log(L"Clipboard could not be saved completely; skipping conversion "
               L"to avoid losing its content");
        // The shortcut would otherwise look like it did nothing.
        MessageBeep(MB_ICONWARNING);
        return;
    }

    HWND foreground = GetForegroundWindow();
    const bool console = IsConsoleWindow(foreground);

    std::wstring selected;
    bool captured = false;
    bool usedFallbackWord = false;
    const DWORD clipboardDelayMs = (DWORD)s.actionDelayMs;

    if (s.scope == L"selection-first" || s.scope == L"selection-only") {
        DWORD seq = GetClipboardSequenceNumber();
        if (console) {
            // Ctrl+C without a selection discards the typed line and interrupts
            // a running command in conhost and Windows Terminal, which both
            // bind Ctrl+Insert to copy instead.
            SendKeyCombo(VK_INSERT, true, false, false, false);
        } else {
            SendKeyCombo('C', true, false, false, false);
        }
        if (WaitForClipboardChange(seq, clipboardDelayMs)) {
            selected = GetClipboardUnicodeText();
            captured = !selected.empty();
        }
    }

    // The previous-word fallback sends Ctrl+C, which is unsafe in consoles.
    if (!captured && s.scope != L"selection-only" && !console) {
        DWORD seq = GetClipboardSequenceNumber();
        SendKeyCombo(VK_LEFT, true, true, false, false);  // Ctrl+Shift+Left
        SendKeyCombo('C', true, false, false, false);     // Ctrl+C
        if (WaitForClipboardChange(seq, clipboardDelayMs)) {
            selected = GetClipboardUnicodeText();
            usedFallbackWord = !selected.empty();
        }
    }

    ULONGLONG tCopyDone = GetTickCount64();

    if (selected.empty()) {
        Wh_Log(L"No text captured for conversion (wait=%llums copy=%llums)",
               tWaitDone - tStart, tCopyDone - tWaitDone);
        if (s.restoreClipboard) RestoreClipboard(snapshot);
        return;
    }

    std::wstring converted;
    HKL targetLayout = nullptr;
    bool convertedOk = ConvertTextAutoOrCycle(selected, s, converted, targetLayout);
    ULONGLONG tConvertDone = GetTickCount64();
    if (!convertedOk || converted.empty() || converted == selected) {
        Wh_Log(L"Conversion produced no change (len=%zu)", selected.size());
        if (usedFallbackWord) {
            SendKeyCombo(VK_RIGHT, false, false, false, false);
        }
        if (s.restoreClipboard) RestoreClipboard(snapshot);
        return;
    }

    if (!SetClipboardUnicodeText(converted)) {
        Wh_Log(L"Failed to set converted clipboard text");
        if (s.restoreClipboard) RestoreClipboard(snapshot);
        return;
    }

    if (console) {
        // A terminal selection covers the screen buffer, not the editable input
        // line, so a paste would insert a duplicate instead of replacing the
        // original. Leave the converted text on the clipboard for a manual
        // paste, and skip the restore so it isn't immediately overwritten.
        Wh_Log(L"Console window: converted text left on the clipboard");
        if (s.showNotification) ShowToast();
        return;
    }

    SendKeyCombo('V', true, false, false, false);  // Ctrl+V
    ULONGLONG tPasteDone = GetTickCount64();

    Sleep(s.restoreClipboard ? clipboardDelayMs : 40);

    if (s.switchLayout && targetLayout) {
        ActivateTargetKeyboardLayout(targetLayout);
    }

    if (s.restoreClipboard) {
        RestoreClipboard(snapshot);
    }

    // After the post-paste wait, so the toast never blocks its own timer and
    // click handling while the conversion thread sleeps.
    if (s.showNotification) {
        ShowToast();
    }
    ULONGLONG tEnd = GetTickCount64();
    Wh_Log(L"Conversion applied (len %zu -> %zu) wait=%llums copy=%llums "
           L"convert=%llums paste=%llums total=%llums",
           selected.size(), converted.size(), tWaitDone - tStart,
           tCopyDone - tWaitDone, tConvertDone - tCopyDone,
           tPasteDone - tConvertDone, tEnd - tStart);
}

// AutoHotkey-style "menu mask": a dummy keypress so that a swallowed Win or
// Alt combo doesn't look like a lone modifier tap to Windows (Start menu or
// menu bar activation).
void SendMenuMaskKey() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0xE8;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0xE8;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

// A modifier key-up can be lost when it happens on the secure desktop (Win+L,
// UAC, Ctrl+Alt+Del), where the hook isn't called. GetAsyncKeyState reports the
// state before the current event, so the current key is skipped.
void ResyncModifierState(UINT currentVk) {
    const UINT modifiers[] = {VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU,
                              VK_LSHIFT, VK_RSHIFT, VK_LWIN, VK_RWIN};
    for (UINT vk : modifiers) {
        if (vk != currentVk) {
            g_keyDown[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
        }
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const KBDLLHOOKSTRUCT* kb = (const KBDLLHOOKSTRUCT*)lParam;
        if (kb && kb->vkCode < 256 && !(kb->flags & LLKHF_INJECTED)) {
            const UINT vk = kb->vkCode;
            const Hotkey hk = GetHotkeyForHook();
            ResyncModifierState(vk);
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                const bool wasDown = g_keyDown[vk];
                g_keyDown[vk] = true;
                if (hk.valid) {
                    if (hk.modifierOnly) {
                        if (!g_hotkeyPending) {
                            if (ModifierOnlyHotkeyMatch(hk)) {
                                g_hotkeyPending = true;
                                g_hotkeyArmTick = GetTickCount64();
                            }
                        } else if (!IsModifierVk(vk) || !ModifierOnlyHotkeyMatch(hk)) {
                            // Another key or an extra modifier went down: this is a
                            // regular shortcut such as Ctrl+Shift+T, not a bare combo.
                            g_hotkeyPending = false;
                        }
                    } else if (vk == hk.vk) {
                        if (!wasDown && !g_hotkeyPending && HotkeyModifiersMatch(hk)) {
                            Wh_Log(L"Hotkey matched, waiting for release");
                            g_suppressedHotkeyVk = vk;
                            g_hotkeyPending = true;
                            g_hotkeyArmTick = GetTickCount64();
                            if (hk.modifiers & (MOD_ALT | MOD_WIN)) {
                                SendMenuMaskKey();
                            }
                            return 1;  // suppress hotkey keydown
                        }
                        if (g_hotkeyPending || g_suppressedHotkeyVk == vk) {
                            return 1;  // swallow auto-repeat while the combo is held
                        }
                    } else if (g_hotkeyPending && !IsModifierVk(vk)) {
                        g_hotkeyPending = false;
                    }
                }
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                g_keyDown[vk] = false;
                const bool suppress = (g_suppressedHotkeyVk != 0 && vk == g_suppressedHotkeyVk);
                if (suppress) {
                    g_suppressedHotkeyVk = 0;
                }
                if (g_hotkeyPending && hk.valid && HotkeyComboReleased(hk)) {
                    const bool heldTooLong =
                        hk.modifierOnly &&
                        GetTickCount64() - g_hotkeyArmTick > kHotkeyMaxHoldMs;
                    g_hotkeyPending = false;
                    if (!heldTooLong) {
                        Wh_Log(L"Hotkey released, requesting conversion");
                        RequestConversion();
                    }
                }
                if (suppress) {
                    return 1;  // suppress the matching keyup to avoid stray input
                }
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// A mouse button or wheel between a modifier-only hotkey press and release
// means the user is doing something else (Ctrl+Shift+click, Ctrl+Shift+wheel,
// ...). Regular combos are unaffected, since they suppress their key.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam != WM_MOUSEMOVE) {
        if (g_suppressedHotkeyVk == 0) {
            g_hotkeyPending = false;
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// Hook thread only. The mouse hook exists to cancel armed modifier-only
// shortcuts, so it isn't installed for regular combos such as the default
// Alt+X, where the combo key is swallowed instead.
void UpdateMouseHook() {
    bool needed = GetHotkeyForHook().modifierOnly;
    if (needed && !g_mouseHook) {
        g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                                       GetModuleHandle(nullptr), 0);
        if (!g_mouseHook) {
            Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) failed: %u", GetLastError());
        }
    } else if (!needed && g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Build the layout maps before installing the hook, so the first shortcut
    // doesn't have to wait for them.
    if (!RefreshLayoutsIfNeeded(true)) {
        Wh_Log(L"No keyboard layouts found or failed to enumerate layouts");
    } else {
        Wh_Log(L"Keyboard layouts enumerated and maps built");
    }

    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                      GetModuleHandle(nullptr), 0);
    if (!g_keyboardHook) {
        Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %u", GetLastError());
    } else {
        Wh_Log(L"Keyboard hook installed (thread=%lu)", GetCurrentThreadId());
    }

    // Only needed for modifier-only shortcuts; see UpdateMouseHook.
    UpdateMouseHook();

    for (;;) {
        DWORD wait = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                               QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto done;
            }
            if (msg.message == WM_APP_UPDATE_MOUSE_HOOK) {
                UpdateMouseHook();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

done:
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

DWORD WINAPI ConversionThreadProc(LPVOID) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    Wh_Log(L"Conversion thread started (thread=%lu)", GetCurrentThreadId());

    for (;;) {
        DWORD wait = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                               QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto done;
            }
            if (msg.message == WM_APP_CONVERT) {
                PerformConversion();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

done:
    if (g_toastHwnd) {
        DestroyWindow(g_toastHwnd);
        g_toastHwnd = nullptr;
    }
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Mod init (pid=%lu)", GetCurrentProcessId());
    LoadSettings();

    Gdiplus::GdiplusStartupInput gdiplusInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) ==
        Gdiplus::Ok) {
        g_gdiplusReady = true;
    } else {
        g_gdiplusToken = 0;
        Wh_Log(L"GdiplusStartup failed; notifications are disabled");
    }

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        StopGdiplus();
        return FALSE;
    }

    g_convertThread = CreateThread(nullptr, 0, ConversionThreadProc, nullptr, 0,
                                   &g_convertThreadId);
    if (!g_convertThread) {
        Wh_Log(L"CreateThread (conversion) failed: %u", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        StopGdiplus();
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread (hook) failed: %u", GetLastError());
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_convertThread, INFINITE);
        CloseHandle(g_convertThread);
        g_convertThread = nullptr;
        g_convertThreadId = 0;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        StopGdiplus();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    // The mouse hook is only needed for modifier-only shortcuts.
    if (g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, WM_APP_UPDATE_MOUSE_HOOK, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    // Wait for both threads before tearing anything down: Windhawk unmaps the
    // mod right after this returns.
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
    if (g_convertThread) {
        WaitForSingleObject(g_convertThread, INFINITE);
        CloseHandle(g_convertThread);
        g_convertThread = nullptr;
        g_convertThreadId = 0;
    }
    if (g_toastClassRegistered && g_toastClass[0]) {
        UnregisterClassW(g_toastClass, g_toastInstance);
        g_toastClassRegistered = false;
        g_toastClass[0] = L'\0';
        g_toastInstance = nullptr;
    }
    StopGdiplus();
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    memset(g_keyDown, 0, sizeof(g_keyDown));
    g_hotkeyPending = false;
    g_suppressedHotkeyVk = 0;
    g_layoutMaps.clear();
    g_layouts.clear();
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
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
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

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
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

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

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

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

