// ==WindhawkMod==
// @id              text-capitalization
// @name            Text Capitalization
// @description     Change the case of selected text in any app: Title Case, Sentence case, UPPERCASE, lowercase, tOGGLE cASE
// @version         0.3.13
// @author          rockinggreninja-bot
// @github          https://github.com/rockinggreninja-bot
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -ladvapi32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Text Capitalization

Select text in almost any app, press a shortcut, and the selected text is
rewritten in place with a different letter case.

## Case modes

* **Title Case**: every word capitalized, except short minor words (a, an, the,
  and, of, to, ...) which stay lowercase unless they are the first or last word.
* **Sentence case**: only the first letter of each sentence/line is capitalized.
* **UPPERCASE**
* **lowercase**
* **tOGGLE cASE**: every letter's case is flipped.

## Usage

* **Ctrl+Alt+P** opens a small picker next to the caret (or the mouse pointer).
  It shows a read-only preview of the result. Use **Up/Down** to choose a case,
  **Enter** (or a mouse click) to apply it, **Escape** (or a click elsewhere) to
  cancel. The picker never takes keyboard focus from your editor.
* **Ctrl+Alt+M** applies the last-used case directly, without the picker.
* The last-used case is remembered across sessions.

Only the **left** Ctrl and **left** Alt keys trigger the shortcuts, so AltGr
combinations (AltGr+P, AltGr+C, ...) keep typing their normal characters.

## Customizing the shortcuts

Open the mod settings and edit the two shortcut fields. The format is the
modifiers followed by the key, joined with `+`:

* Modifiers: `ctrl`, `alt`, `shift` (at least `ctrl` or `alt` is required).
* Key: either a **letter** (`A`-`Z`), a digit (`0`-`9`), a function key
  (`F1`-`F24`), or a numeric **virtual-key code** (decimal such as `80`, or hex
  such as `0x50`).

Examples (the first two are equivalent, because virtual-key code 80 is `P`):

* `ctrl+alt+P`
* `ctrl+alt+80`
* `ctrl+shift+0x4D`
* `ctrl+alt+F9`

This works on every Windhawk version, including 1.7.3 which has no shortcut
recorder. Virtual-key codes:
https://learn.microsoft.com/windows/win32/inputdev/virtual-key-codes

If a shortcut can't be parsed, the default is used and a message is written to
the Windhawk log. The parsed result of each shortcut is logged too.

## How it works

The mod copies the selection with Ctrl+C, transforms the text, and pastes it
back with Ctrl+V. Your clipboard is snapshotted before and restored afterwards
(best-effort: only data stored as global memory is preserved, and the pasted text
is flagged so it stays out of clipboard history). Nothing is changed if the copy
produces no text, if you switch to another window in the meantime, or if
modifier keys are held for too long.

## Notes and limitations

* Console/terminal windows (conhost, Windows Terminal, mintty) are ignored,
  because Ctrl+C there means "interrupt" rather than "copy".
* Apps that copy the whole line when nothing is selected (some code editors)
  will have that line transformed.
* Apps running as administrator can't receive the simulated keystrokes unless
  Windhawk itself runs elevated.
* The mod runs in its own `windhawk.exe` process; it does not inject into or hook
  other applications.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- OpenPickerHotkey: "ctrl+alt+80"
  $name: Open picker from selected text
  $description: Copy the current selection into the read-only preview and open the case picker. Modifiers ctrl/alt/shift (ctrl or alt required; win is not supported). Empty disables the shortcut.
  #! $format: hotkey
- ApplyHotkey: "ctrl+alt+77"
  $name: Apply saved case immediately
  $description: Apply the saved case to the current selection without opening the picker. Modifiers ctrl/alt/shift (ctrl or alt required; win is not supported). Empty disables the shortcut.
  #! $format: hotkey
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <vector>
#include <windhawk_utils.h>

namespace {

////////////////////////////////////////////////////////////////////////////////
// Constants

constexpr int kModeCount = 5;
constexpr const wchar_t* kModeNames[kModeCount] = {
    L"Title Case", L"Sentence case", L"UPPERCASE", L"lowercase",
    L"tOGGLE cASE"};

constexpr wchar_t kPickerClass[] = L"WindhawkTextCapitalizationPicker";

constexpr UINT WM_APP_TRIGGER = WM_APP + 1;   // wParam: kTrigger*
constexpr UINT WM_APP_KEY = WM_APP + 2;       // wParam: VK_UP / VK_DOWN / ...
constexpr UINT WM_APP_DISMISS = WM_APP + 3;
constexpr UINT WM_APP_CAPTURED = WM_APP + 4;  // lParam: CaptureResult*
constexpr UINT WM_APP_JOB_DONE = WM_APP + 5;

constexpr WPARAM kTriggerOpen = 0;
constexpr WPARAM kTriggerApply = 1;

constexpr UINT_PTR kWatchTimer = 1;
constexpr UINT_PTR kHookTimer = 2;
constexpr UINT kHookRefreshMs = 15000;
constexpr UINT kWatchIntervalMs = 250;

constexpr DWORD kRepeatWindowMs = 1500;     // auto-repeat vs. stale swallow flag
constexpr DWORD kModifierWaitMs = 1500;     // wait for the user to release keys
constexpr DWORD kCopyTimeoutMs = 500;       // wait for Ctrl+C to fill clipboard
constexpr DWORD kPollMs = 10;
constexpr DWORD kPasteSettleMs = 700;       // unverifiable paste: wait this long
constexpr DWORD kPasteAfterRenderMs = 200;  // verified paste: let the app finish
constexpr DWORD kPasteConsumeMs = 1000;     // wait for the app to read the text
constexpr DWORD kTargetWaitMs = 400;        // wait for the target to be foreground
constexpr int kClipboardRetries = 60;
constexpr size_t kMaxSnapshotBytes = 64u * 1024u * 1024u;
constexpr size_t kMaxDisplayChars = 240;

////////////////////////////////////////////////////////////////////////////////
// Types

struct Shortcut {
    UINT vk = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;

    uint32_t Pack() const {
        return vk | (ctrl ? 0x100u : 0u) | (alt ? 0x200u : 0u) |
               (shift ? 0x400u : 0u);
    }

    static Shortcut Unpack(uint32_t v) {
        Shortcut s;
        s.vk = v & 0xFFu;
        s.ctrl = (v & 0x100u) != 0;
        s.alt = (v & 0x200u) != 0;
        s.shift = (v & 0x400u) != 0;
        return s;
    }
};

const Shortcut kDefaultOpen{'P', true, true, false};
const Shortcut kDefaultApply{'M', true, true, false};

struct TargetInfo {
    HWND fg = nullptr;
    POINT anchor{};
};

struct ClipFormat {
    UINT format;
    std::vector<BYTE> data;
};
using ClipSnapshot = std::vector<ClipFormat>;

struct CaptureResult {
    TargetInfo target;
    std::wstring text;
    ClipSnapshot snapshot;  // clipboard as it was before the copy
    std::wstring results[kModeCount];  // computed off the UI thread
};

enum class JobKind { Open, Apply, Paste };

struct Job {
    JobKind kind = JobKind::Open;
    TargetInfo target;
    std::wstring text;      // Paste only: text to paste
    ClipSnapshot snapshot;  // Paste only: fallback clipboard to restore
};

struct Theme {
    COLORREF bg, previewBg, border, text, selection, selectionText;
};

struct Layout {
    int dpi = 96;
    int width = 0;
    int height = 0;
    int margin = 0;
    int pad = 0;
    int rowsTop = 0;
    int rowH = 0;
    int radius = 0;
    RECT preview{};
};

struct PickerData {
    TargetInfo target;
    ClipSnapshot snapshot;
    std::wstring original;
    std::wstring results[kModeCount];
    std::wstring display[kModeCount];
    int sel = 0;
    int pressed = -1;
    int misses = 0;
    POINT showCursor{};
    Layout layout;
    Theme theme{};
};

////////////////////////////////////////////////////////////////////////////////
// Globals

// Written by the settings thread, read by the keyboard hook.
std::atomic<uint32_t> g_openPacked{0};
std::atomic<uint32_t> g_applyPacked{0};

// Shared between the UI thread and the worker thread.
std::atomic<bool> g_busy{false};
std::atomic<int> g_caseMode{0};

// The text offered for pasting. It is placed on the clipboard with delayed
// rendering, so the UI thread (the clipboard owner) learns the moment the target
// app actually reads it.
SRWLOCK g_pasteLock = SRWLOCK_INIT;
std::wstring g_pasteText;
HANDLE g_pasteRendered = nullptr;  // manual-reset event, set on render
std::atomic<bool> g_stopping{false};

// UI thread only (the low-level hooks run on the UI thread as well).
HWND g_wnd = nullptr;
HHOOK g_keyboardHook = nullptr;
HHOOK g_mouseHook = nullptr;
HANDLE g_worker = nullptr;
HFONT g_font = nullptr;
UINT g_fontDpi = 0;
bool g_pickerVisible = false;
int g_pendingTrigger = -1;  // trigger pressed while a job was still running
PickerData g_picker;
bool g_swallowed[256] = {};
DWORD g_swallowTime[256] = {};

using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
HMODULE g_shcore = nullptr;
GetDpiForMonitor_t g_getDpiForMonitor = nullptr;

// Start-up handshake with the UI thread.
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
HANDLE g_uiReady = nullptr;
bool g_uiOk = false;

////////////////////////////////////////////////////////////////////////////////
// Shortcut parsing

std::wstring Trim(const std::wstring& s) {
    const size_t b = s.find_first_not_of(L" \t");
    if (b == std::wstring::npos) {
        return L"";
    }
    const size_t e = s.find_last_not_of(L" \t");
    return s.substr(b, e - b + 1);
}

std::wstring LowerAscii(std::wstring s) {
    for (wchar_t& c : s) {
        if (c >= L'A' && c <= L'Z') {
            c = static_cast<wchar_t>(c - L'A' + L'a');
        }
    }
    return s;
}

bool IsNumberText(const std::wstring& t, size_t from, int base) {
    if (from >= t.size()) {
        return false;
    }
    for (size_t i = from; i < t.size(); ++i) {
        const wchar_t c = t[i];
        const bool dec = c >= L'0' && c <= L'9';
        const bool hex = (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
        if (!dec && !(base == 16 && hex)) {
            return false;
        }
    }
    return true;
}

// Letter, digit, F1-F24, or a numeric VK code (decimal or 0x hex). Returns 0 if
// the token is not a valid key.
UINT ParseKeyToken(const std::wstring& t) {
    if (t.empty()) {
        return 0;
    }

    if (t.size() == 1) {
        wchar_t c = t[0];
        if (c >= L'a' && c <= L'z') {
            c = static_cast<wchar_t>(c - L'a' + L'A');
        }
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return static_cast<UINT>(c);
        }
        return 0;
    }

    if ((t[0] == L'F' || t[0] == L'f') && IsNumberText(t, 1, 10)) {
        const unsigned long n = wcstoul(t.c_str() + 1, nullptr, 10);
        return (n >= 1 && n <= 24) ? static_cast<UINT>(VK_F1 + n - 1) : 0;
    }

    int base = 10;
    size_t offset = 0;
    if (t.size() > 2 && t[0] == L'0' && (t[1] == L'x' || t[1] == L'X')) {
        base = 16;
        offset = 2;
    }
    if (!IsNumberText(t, offset, base)) {
        return 0;
    }

    const unsigned long v = wcstoul(t.c_str() + offset, nullptr, base);
    if (v < 1 || v > 254) {
        return 0;
    }
    if (v == VK_SHIFT || v == VK_CONTROL || v == VK_MENU || v == VK_LWIN ||
        v == VK_RWIN || (v >= VK_LSHIFT && v <= VK_RMENU)) {
        return 0;  // modifiers can't be the main key
    }
    return static_cast<UINT>(v);
}

bool ParseShortcut(const wchar_t* text, Shortcut* out) {
    if (!text) {
        return false;
    }

    std::vector<std::wstring> tokens;
    const std::wstring str = text;
    size_t pos = 0;
    for (;;) {
        const size_t plus = str.find(L'+', pos);
        const size_t len =
            plus == std::wstring::npos ? std::wstring::npos : plus - pos;
        tokens.push_back(Trim(str.substr(pos, len)));
        if (plus == std::wstring::npos) {
            break;
        }
        pos = plus + 1;
    }
    if (tokens.size() < 2) {
        return false;
    }

    Shortcut s;
    for (size_t i = 0; i + 1 < tokens.size(); ++i) {
        const std::wstring m = LowerAscii(tokens[i]);
        if (m == L"ctrl" || m == L"control") {
            s.ctrl = true;
        } else if (m == L"alt") {
            s.alt = true;
        } else if (m == L"shift") {
            s.shift = true;
        } else {
            return false;
        }
    }
    if (!s.ctrl && !s.alt) {
        return false;  // a bare key or Shift+key would hijack normal typing
    }

    s.vk = ParseKeyToken(tokens.back());
    if (!s.vk) {
        return false;
    }

    *out = s;
    return true;
}

std::wstring FormatShortcut(const Shortcut& s) {
    std::wstring r;
    if (s.ctrl) {
        r += L"ctrl+";
    }
    if (s.alt) {
        r += L"alt+";
    }
    if (s.shift) {
        r += L"shift+";
    }
    if ((s.vk >= 'A' && s.vk <= 'Z') || (s.vk >= '0' && s.vk <= '9')) {
        r += static_cast<wchar_t>(s.vk);
    } else if (s.vk >= VK_F1 && s.vk <= VK_F24) {
        r += L"F" + std::to_wstring(s.vk - VK_F1 + 1);
    } else {
        r += std::to_wstring(s.vk);
    }
    return r;
}

Shortcut LoadShortcutSetting(const wchar_t* name, const Shortcut& fallback) {
    Shortcut result = fallback;
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(name);
    const wchar_t* v = value.get();
    if (!v || !*v) {
        result.vk = 0;
        result.ctrl = result.alt = result.shift = false;
        Wh_Log(L"%s is empty; shortcut disabled", name);
        return result;
    }
    if (ParseShortcut(v, &result)) {
        Wh_Log(L"%s = \"%s\" -> %s", name, v, FormatShortcut(result).c_str());
    } else {
        result = fallback;
        Wh_Log(L"%s = \"%s\" is invalid, using %s", name, v,
               FormatShortcut(fallback).c_str());
    }
    return result;
}

void LoadSettings() {
    g_openPacked = LoadShortcutSetting(L"OpenPickerHotkey", kDefaultOpen).Pack();
    g_applyPacked =
        LoadShortcutSetting(L"ApplyHotkey", kDefaultApply).Pack();
}

////////////////////////////////////////////////////////////////////////////////
// Key state helpers

bool KeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool AnyModifierDown() {
    return KeyDown(VK_LCONTROL) || KeyDown(VK_RCONTROL) ||
           KeyDown(VK_LMENU) || KeyDown(VK_RMENU) || KeyDown(VK_LSHIFT) ||
           KeyDown(VK_RSHIFT) || KeyDown(VK_LWIN) || KeyDown(VK_RWIN);
}

bool IsModifierVk(DWORD vk) {
    return vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU ||
           vk == VK_LWIN || vk == VK_RWIN ||
           (vk >= VK_LSHIFT && vk <= VK_RMENU);
}

// Modifiers are read from the async key state when the main key arrives; the
// hook keeps no modifier state, so a missed key-up can never leave one stuck.
// Only the left Ctrl / left Alt count. Right Alt (AltGr, which Windows reports
// as Ctrl+RightAlt) never triggers a shortcut.
bool MatchesShortcut(const Shortcut& s, DWORD vk) {
    if (!s.vk || vk != s.vk) {
        return false;
    }
    if (KeyDown(VK_RMENU) || KeyDown(VK_LWIN) || KeyDown(VK_RWIN)) {
        return false;
    }

    const bool lctrl = KeyDown(VK_LCONTROL);
    const bool rctrl = KeyDown(VK_RCONTROL);
    if (s.ctrl ? !lctrl : (lctrl || rctrl)) {
        return false;
    }
    if (s.alt != KeyDown(VK_LMENU)) {
        return false;
    }
    return s.shift == KeyDown(VK_SHIFT);
}

////////////////////////////////////////////////////////////////////////////////
// Case transformations

std::wstring MapCase(const std::wstring& s, DWORD flag) {
    if (s.empty()) {
        return s;
    }
    const int n = static_cast<int>(s.size());
    std::wstring out(s.size(), L'\0');
    const int r = LCMapStringEx(LOCALE_NAME_INVARIANT, flag, s.c_str(), n,
                                out.data(), n, nullptr, nullptr, 0);
    if (r != n) {
        Wh_Log(L"Case mapping failed (error %lu)", GetLastError());
        return s;
    }
    return out;
}

bool IsSpaceChar(wchar_t c) {
    return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' ||
           c == 0x00A0;
}

bool IsClosingPunct(wchar_t c) {
    return c == L'"' || c == L'\'' || c == L')' || c == L']' || c == L'}' ||
           c == 0x2019 || c == 0x201D || c == 0x00BB;
}

bool IsMinorWord(const std::wstring& w) {
    static const wchar_t* const kMinor[] = {
        L"a",  L"an", L"and", L"as",  L"at",  L"but", L"by",
        L"for", L"in", L"nor", L"of",  L"on",  L"or",  L"per",
        L"the", L"to", L"via", L"vs"};
    for (const wchar_t* m : kMinor) {
        if (w == m) {
            return true;
        }
    }
    return false;
}

std::wstring TitleCase(const std::wstring& s) {
    const std::wstring lower = MapCase(s, LCMAP_LOWERCASE);
    const std::wstring upper = MapCase(s, LCMAP_UPPERCASE);
    if (lower.size() != s.size() || upper.size() != s.size()) {
        return s;
    }

    auto isWordChar = [&](size_t i) {
        const wchar_t c = lower[i];
        if (IsCharAlphaNumericW(c)) {
            return true;
        }
        // Apostrophes inside a word ("don't") don't split it.
        return (c == L'\'' || c == 0x2019) && i > 0 && i + 1 < lower.size() &&
               IsCharAlphaNumericW(lower[i - 1]) &&
               IsCharAlphaNumericW(lower[i + 1]);
    };

    std::vector<std::pair<size_t, size_t>> words;
    for (size_t i = 0; i < lower.size();) {
        if (!isWordChar(i)) {
            ++i;
            continue;
        }
        const size_t start = i;
        while (i < lower.size() && isWordChar(i)) {
            ++i;
        }
        words.emplace_back(start, i);
    }

    std::wstring out = lower;
    for (size_t k = 0; k < words.size(); ++k) {
        const size_t start = words[k].first;
        const std::wstring word =
            lower.substr(start, words[k].second - start);
        if (k == 0 || k + 1 == words.size() || !IsMinorWord(word)) {
            out[start] = upper[start];
        }
    }
    return out;
}

std::wstring SentenceCase(const std::wstring& s) {
    const std::wstring lower = MapCase(s, LCMAP_LOWERCASE);
    const std::wstring upper = MapCase(s, LCMAP_UPPERCASE);
    if (lower.size() != s.size() || upper.size() != s.size()) {
        return s;
    }

    std::wstring out = lower;
    bool startOfSentence = true;
    for (size_t i = 0; i < out.size(); ++i) {
        const wchar_t c = out[i];
        if (IsCharAlphaNumericW(c)) {
            if (startOfSentence) {
                out[i] = upper[i];
                startOfSentence = false;
            }
        } else if (c == L'\r' || c == L'\n') {
            startOfSentence = true;
        } else if (c == L'.' || c == L'!' || c == L'?' || c == 0x2026) {
            // A terminator only ends a sentence when whitespace (or the end of
            // the text) follows, so "3.5" and "example.com" stay untouched.
            size_t j = i + 1;
            while (j < out.size() && IsClosingPunct(out[j])) {
                ++j;
            }
            if (j >= out.size() || IsSpaceChar(out[j])) {
                startOfSentence = true;
            }
        }
    }
    return out;
}

std::wstring ToggleCase(const std::wstring& s) {
    const std::wstring lower = MapCase(s, LCMAP_LOWERCASE);
    const std::wstring upper = MapCase(s, LCMAP_UPPERCASE);
    if (lower.size() != s.size() || upper.size() != s.size()) {
        return s;
    }

    std::wstring out = s;
    for (size_t i = 0; i < s.size(); ++i) {
        if (lower[i] != s[i]) {
            out[i] = lower[i];  // was uppercase
        } else if (upper[i] != s[i]) {
            out[i] = upper[i];  // was lowercase
        }
    }
    return out;
}

std::wstring Transform(int mode, const std::wstring& s) {
    switch (mode) {
        case 0:
            return TitleCase(s);
        case 1:
            return SentenceCase(s);
        case 2:
            return MapCase(s, LCMAP_UPPERCASE);
        case 3:
            return MapCase(s, LCMAP_LOWERCASE);
        default:
            return ToggleCase(s);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Clipboard

bool OpenClipboardRetry() {
    for (int i = 0; i < kClipboardRetries; ++i) {
        // A real owner window is required, otherwise SetClipboardData fails
        // after EmptyClipboard.
        if (OpenClipboard(g_wnd)) {
            return true;
        }
        Sleep(10);
    }
    return false;
}

bool IsSnapshotFormat(UINT f) {
    switch (f) {
        case CF_BITMAP:
        case CF_PALETTE:
        case CF_METAFILEPICT:
        case CF_ENHMETAFILE:
        case CF_OWNERDISPLAY:
        case CF_DSPTEXT:
        case CF_DSPBITMAP:
        case CF_DSPMETAFILEPICT:
        case CF_DSPENHMETAFILE:
            return false;  // not plain global memory (or synthesized)
    }
    return !(f >= CF_PRIVATEFIRST && f <= CF_GDIOBJLAST);
}

bool SnapshotClipboard(ClipSnapshot& snap) {
    snap.clear();
    if (!OpenClipboardRetry()) {
        return false;
    }

    size_t total = 0;
    for (UINT f = EnumClipboardFormats(0); f; f = EnumClipboardFormats(f)) {
        if (!IsSnapshotFormat(f)) {
            continue;
        }
        HANDLE h = GetClipboardData(f);
        if (!h) {
            continue;
        }
        const SIZE_T size = GlobalSize(h);
        if (!size || total + size > kMaxSnapshotBytes) {
            continue;
        }
        const BYTE* p = static_cast<const BYTE*>(GlobalLock(h));
        if (!p) {
            continue;
        }
        snap.push_back({f, std::vector<BYTE>(p, p + size)});
        GlobalUnlock(h);
        total += size;
    }

    CloseClipboard();
    return true;
}

void RestoreClipboard(const ClipSnapshot& snap) {
    if (!OpenClipboardRetry()) {
        return;
    }

    EmptyClipboard();
    for (const ClipFormat& f : snap) {
        HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, f.data.size());
        if (!h) {
            continue;
        }
        void* p = GlobalLock(h);
        if (!p) {
            GlobalFree(h);
            continue;
        }
        memcpy(p, f.data.data(), f.data.size());
        GlobalUnlock(h);
        if (!SetClipboardData(f.format, h)) {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

bool ReadClipboardText(std::wstring& out) {
    if (!OpenClipboardRetry()) {
        return false;
    }

    bool ok = false;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t* p = static_cast<const wchar_t*>(GlobalLock(h));
        if (p) {
            const size_t maxLen = GlobalSize(h) / sizeof(wchar_t);
            size_t n = 0;
            while (n < maxLen && p[n]) {
                ++n;
            }
            out.assign(p, n);
            GlobalUnlock(h);
            ok = true;
        }
    }

    CloseClipboard();
    return ok;
}

void SetClipboardDword(const wchar_t* formatName, DWORD value) {
    const UINT format = RegisterClipboardFormatW(formatName);
    if (!format) {
        return;
    }
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
    if (!h) {
        return;
    }
    DWORD* p = static_cast<DWORD*>(GlobalLock(h));
    if (!p) {
        GlobalFree(h);
        return;
    }
    *p = value;
    GlobalUnlock(h);
    if (!SetClipboardData(format, h)) {
        GlobalFree(h);
    }
}

// Offers `text` on the clipboard without rendering it yet. Windows asks the
// owner window (WM_RENDERFORMAT) when an app actually reads it.
bool SetClipboardTextDelayed(const std::wstring& text) {
    AcquireSRWLockExclusive(&g_pasteLock);
    g_pasteText = text;
    ReleaseSRWLockExclusive(&g_pasteLock);
    ResetEvent(g_pasteRendered);

    if (!OpenClipboardRetry()) {
        return false;
    }

    bool ok = false;
    if (EmptyClipboard()) {
        SetClipboardData(CF_UNICODETEXT, nullptr);
        ok = IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
    }
    if (ok) {
        // Keep the temporary text out of clipboard history / cloud sync.
        SetClipboardDword(L"ExcludeClipboardContentFromMonitorProcessing", 1);
        SetClipboardDword(L"CanIncludeInClipboardHistory", 0);
        SetClipboardDword(L"CanUploadToCloudClipboard", 0);
    }

    CloseClipboard();
    return ok;
}

// UI thread: answers WM_RENDERFORMAT / WM_RENDERALLFORMATS for the paste text.
void RenderPasteText(bool openClipboard) {
    std::wstring text;
    AcquireSRWLockShared(&g_pasteLock);
    text = g_pasteText;
    ReleaseSRWLockShared(&g_pasteLock);
    if (text.empty()) {
        return;
    }

    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!h) {
        return;
    }
    void* p = GlobalLock(h);
    if (!p) {
        GlobalFree(h);
        return;
    }
    memcpy(p, text.c_str(), bytes);
    GlobalUnlock(h);

    // For WM_RENDERFORMAT the requester already has the clipboard open.
    if (openClipboard && !OpenClipboard(g_wnd)) {
        GlobalFree(h);
        return;
    }
    if (!SetClipboardData(CF_UNICODETEXT, h)) {
        GlobalFree(h);
    }
    if (openClipboard) {
        CloseClipboard();
    }
    SetEvent(g_pasteRendered);
}

////////////////////////////////////////////////////////////////////////////////
// Selection capture / replacement (worker thread)

bool SendCtrlChord(WORD key) {
    const WORD vks[4] = {VK_CONTROL, key, key, VK_CONTROL};
    INPUT in[4] = {};
    for (int i = 0; i < 4; ++i) {
        in[i].type = INPUT_KEYBOARD;
        in[i].ki.wVk = vks[i];
        in[i].ki.wScan =
            static_cast<WORD>(MapVirtualKeyW(vks[i], MAPVK_VK_TO_VSC));
        in[i].ki.dwFlags = i >= 2 ? KEYEVENTF_KEYUP : 0;
    }
    return SendInput(4, in, sizeof(INPUT)) == 4;
}

// The shortcut's own modifiers would turn the simulated Ctrl+C into
// Ctrl+Alt+C, so wait until every modifier is physically released.
bool WaitForModifiersReleased(DWORD timeoutMs) {
    const DWORD start = GetTickCount();
    while (AnyModifierDown()) {
        if (GetTickCount() - start >= timeoutMs) {
            return false;
        }
        Sleep(kPollMs);
    }
    Sleep(30);  // let the app process the key-ups before simulated input
    return true;
}

// Ctrl+C means "interrupt" in terminals, so never send it there.
bool IsTerminalWindow(HWND hwnd) {
    static const wchar_t* const kClasses[] = {
        L"ConsoleWindowClass", L"CASCADIA_HOSTING_WINDOW_CLASS", L"mintty",
        L"VirtualConsoleClass", L"PuTTY"};
    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    for (const wchar_t* c : kClasses) {
        if (wcscmp(cls, c) == 0) {
            return true;
        }
    }
    return false;
}

bool GetTarget(TargetInfo& t) {
    HWND fg = GetForegroundWindow();
    if (!fg || IsTerminalWindow(fg)) {
        return false;
    }

    GUITHREADINFO gti = {sizeof(gti)};
    if (!GetGUIThreadInfo(GetWindowThreadProcessId(fg, nullptr), &gti)) {
        return false;
    }

    t.fg = fg;

    // Prefer the caret as anchor; many modern apps don't report one, so fall
    // back to the mouse pointer.
    bool haveCaret = false;
    if (gti.hwndCaret && (gti.rcCaret.right > gti.rcCaret.left ||
                          gti.rcCaret.bottom > gti.rcCaret.top)) {
        POINT p = {gti.rcCaret.left, gti.rcCaret.bottom};
        if (ClientToScreen(gti.hwndCaret, &p) &&
            MonitorFromPoint(p, MONITOR_DEFAULTTONULL)) {
            t.anchor = p;
            haveCaret = true;
        }
    }
    if (!haveCaret) {
        GetCursorPos(&t.anchor);
    }
    return true;
}

bool CaptureSelection(TargetInfo& target,
                      std::wstring& text,
                      ClipSnapshot& snapshot) {
    if (!WaitForModifiersReleased(kModifierWaitMs)) {
        Wh_Log(L"Capture aborted: modifier keys still held");
        return false;
    }
    // The foreground window can read as null for a moment while windows
    // change; give it a few tries before giving up.
    bool haveTarget = false;
    for (int i = 0; i < 10 && !haveTarget; ++i) {
        haveTarget = GetTarget(target);
        if (!haveTarget) {
            Sleep(20);
        }
    }
    if (!haveTarget) {
        Wh_Log(L"Capture aborted: no usable foreground window");
        return false;
    }

    if (!SnapshotClipboard(snapshot)) {
        Wh_Log(L"Capture aborted: clipboard is locked");
        return false;  // don't touch a clipboard we can't back up
    }

    // Empty the clipboard so a sequence-number change proves the app copied.
    if (!OpenClipboardRetry()) {
        Wh_Log(L"Capture aborted: clipboard is locked");
        return false;
    }
    EmptyClipboard();
    CloseClipboard();
    const DWORD sequence = GetClipboardSequenceNumber();

    bool copied = false;
    for (int attempt = 0; attempt < 2 && !copied; ++attempt) {
        if (!SendCtrlChord('C')) {
            break;
        }
        for (DWORD waited = 0; waited < kCopyTimeoutMs; waited += kPollMs) {
            Sleep(kPollMs);
            if (GetClipboardSequenceNumber() != sequence &&
                IsClipboardFormatAvailable(CF_UNICODETEXT)) {
                copied = ReadClipboardText(text);
                break;
            }
        }
    }

    RestoreClipboard(snapshot);
    if (!copied || text.empty()) {
        Wh_Log(L"Capture: nothing was copied (no selection?)");
        return false;
    }
    return true;
}

bool WaitForTarget(const TargetInfo& target) {
    for (DWORD waited = 0; waited < kTargetWaitMs; waited += kPollMs) {
        if (GetForegroundWindow() == target.fg) {
            return true;
        }
        Sleep(kPollMs);
    }
    return GetForegroundWindow() == target.fg;
}

bool PasteText(const TargetInfo& target,
               const std::wstring& text,
               ClipSnapshot& snapshot) {
    if (!WaitForModifiersReleased(kModifierWaitMs)) {
        Wh_Log(L"Paste aborted: modifier keys still held");
        return false;
    }
    if (!WaitForTarget(target)) {
        Wh_Log(L"Paste aborted: the foreground window changed");
        return false;
    }

    // Prefer a fresh snapshot; if the clipboard is briefly locked, fall back to
    // the one taken at capture time instead of giving up.
    ClipSnapshot fresh;
    if (SnapshotClipboard(fresh)) {
        snapshot = std::move(fresh);
    }

    if (!SetClipboardTextDelayed(text)) {
        Wh_Log(L"Paste aborted: could not set the clipboard");
        RestoreClipboard(snapshot);
        return false;
    }

    // If something (a clipboard manager) already read the text, Windows won't
    // report the app's own read, so this paste can't be verified or retried.
    const bool verifiable =
        WaitForSingleObject(g_pasteRendered, 50) != WAIT_OBJECT_0;

    bool sent = false;
    bool consumed = false;
    for (int attempt = 0; attempt < 2 && !consumed && !g_stopping; ++attempt) {
        if (!WaitForTarget(target)) {
            Wh_Log(L"Paste aborted: the foreground window changed");
            break;
        }
        if (!SendCtrlChord('V')) {
            Wh_Log(L"Paste failed: SendInput was blocked");
            break;
        }
        sent = true;
        if (!verifiable) {
            break;
        }
        consumed = WaitForSingleObject(g_pasteRendered, kPasteConsumeMs) ==
                       WAIT_OBJECT_0 &&
                   !g_stopping;
        if (!consumed && !g_stopping) {
            Wh_Log(L"Paste: the app did not read the clipboard (attempt %d)",
                   attempt + 1);
        }
    }

    if (sent && !g_stopping) {
        Sleep(consumed ? kPasteAfterRenderMs : kPasteSettleMs);
    }

    // Only restore if nobody else took over the clipboard in the meantime.
    if (GetClipboardOwner() == g_wnd) {
        RestoreClipboard(snapshot);
    }
    AcquireSRWLockExclusive(&g_pasteLock);
    g_pasteText.clear();
    ReleaseSRWLockExclusive(&g_pasteLock);
    return sent;
}

DWORD WINAPI WorkerProc(LPVOID param) {
    std::unique_ptr<Job> job(static_cast<Job*>(param));

    switch (job->kind) {
        case JobKind::Open: {
            auto* result = new CaptureResult;
            bool delivered = false;
            if (CaptureSelection(result->target, result->text,
                                 result->snapshot)) {
                for (int i = 0; i < kModeCount; ++i) {
                    result->results[i] = Transform(i, result->text);
                }
                // On success the UI thread owns (and frees) `result`.
                delivered = PostMessageW(g_wnd, WM_APP_CAPTURED, 0,
                                         reinterpret_cast<LPARAM>(result)) != 0;
            } else {
                MessageBeep(static_cast<UINT>(-1));
            }
            if (delivered) {
                return 0;
            }
            delete result;
            break;
        }
        case JobKind::Apply: {
            TargetInfo target;
            std::wstring text;
            ClipSnapshot snapshot;
            if (CaptureSelection(target, text, snapshot)) {
                const std::wstring changed = Transform(g_caseMode.load(), text);
                if (changed != text) {
                    PasteText(target, changed, snapshot);
                }
            } else {
                MessageBeep(static_cast<UINT>(-1));
            }
            break;
        }
        case JobKind::Paste:
            PasteText(job->target, job->text, job->snapshot);
            break;
    }

    PostMessageW(g_wnd, WM_APP_JOB_DONE, 0, 0);
    return 0;
}

// UI thread: hands the job over to a fresh worker thread, which frees it. If
// the job can't be started it is freed here. At most one job runs at a time.
void StartJob(Job* job) {
    if (g_busy.exchange(true)) {
        delete job;
        return;
    }
    HANDLE h = CreateThread(nullptr, 0, WorkerProc, job, 0, nullptr);
    if (!h) {
        g_busy = false;
        delete job;
        return;
    }
    if (g_worker) {
        CloseHandle(g_worker);
    }
    g_worker = h;
}

void StartTrigger(int trigger) {
    auto* job = new Job;
    job->kind = trigger == static_cast<int>(kTriggerOpen) ? JobKind::Open
                                                          : JobKind::Apply;
    StartJob(job);
}

// Runs a shortcut that was pressed while the previous job was still busy.
bool RunPendingTrigger() {
    if (g_pendingTrigger < 0) {
        return false;
    }
    const int trigger = g_pendingTrigger;
    g_pendingTrigger = -1;
    StartTrigger(trigger);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Picker UI (UI thread)

int Scale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

UINT DpiForPoint(POINT pt) {
    UINT dpi = 96;
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    UINT x = 0, y = 0;
    if (mon && g_getDpiForMonitor &&
        SUCCEEDED(g_getDpiForMonitor(mon, 0 /* MDT_EFFECTIVE_DPI */, &x, &y)) &&
        x) {
        dpi = x;
    }
    return dpi;
}

void EnsureFont(UINT dpi) {
    if (g_font && g_fontDpi == dpi) {
        return;
    }
    if (g_font) {
        DeleteObject(g_font);
    }
    LOGFONTW lf = {};
    lf.lfHeight = -MulDiv(9, static_cast<int>(dpi), 72);
    lf.lfWeight = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lstrcpynW(lf.lfFaceName, L"Segoe UI", LF_FACESIZE);
    g_font = CreateFontIndirectW(&lf);
    g_fontDpi = dpi;
}

Layout MakeLayout(UINT dpi) {
    HDC dc = GetDC(nullptr);
    HGDIOBJ oldFont = SelectObject(dc, g_font);
    TEXTMETRICW tm = {};
    GetTextMetricsW(dc, &tm);
    SelectObject(dc, oldFont);
    ReleaseDC(nullptr, dc);

    Layout l;
    l.dpi = static_cast<int>(dpi);
    l.margin = Scale(10, dpi);
    l.pad = Scale(8, dpi);
    l.width = Scale(320, dpi);
    l.radius = Scale(10, dpi);
    l.preview = {l.margin, l.margin, l.width - l.margin,
                 l.margin + 3 * tm.tmHeight + 2 * l.pad};
    l.rowsTop = l.preview.bottom + Scale(8, dpi);
    l.rowH = tm.tmHeight + Scale(12, dpi);
    l.height = l.rowsTop + kModeCount * l.rowH + l.margin;
    return l;
}

Theme GetTheme() {
    DWORD light = 1;
    DWORD size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                 &size);
    if (!light) {
        return {RGB(32, 32, 32),    RGB(45, 45, 45),    RGB(72, 72, 72),
                RGB(240, 240, 240), RGB(0, 95, 184),    RGB(255, 255, 255)};
    }
    return {RGB(249, 249, 249), RGB(255, 255, 255), RGB(205, 205, 205),
            RGB(28, 28, 28),    RGB(0, 103, 192),   RGB(255, 255, 255)};
}

std::wstring MakeDisplay(const std::wstring& s) {
    std::wstring d = s.substr(0, kMaxDisplayChars);
    for (wchar_t& c : d) {
        if (c == L'\r' || c == L'\n' || c == L'\t') {
            c = L' ';
        }
    }
    if (s.size() > kMaxDisplayChars) {
        d += L'\u2026';
    }
    return d;
}

void FillSolid(HDC dc, const RECT& rc, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rc, brush);
    DeleteObject(brush);
}

void PaintPicker(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);

    const Layout& l = g_picker.layout;
    const Theme& th = g_picker.theme;

    HDC mem = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, l.width, l.height);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);
    HGDIOBJ oldFont = SelectObject(mem, g_font);
    SetBkMode(mem, TRANSPARENT);

    const RECT full = {0, 0, l.width, l.height};
    FillSolid(mem, full, th.bg);
    FillSolid(mem, l.preview, th.previewBg);

    RECT textRc = l.preview;
    InflateRect(&textRc, -l.pad, -l.pad);
    SetTextColor(mem, th.text);
    DrawTextW(mem, g_picker.display[g_picker.sel].c_str(), -1, &textRc,
              DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | DT_EDITCONTROL);

    for (int i = 0; i < kModeCount; ++i) {
        RECT row = {l.margin, l.rowsTop + i * l.rowH, l.width - l.margin,
                    l.rowsTop + (i + 1) * l.rowH};
        const bool selected = i == g_picker.sel;
        if (selected) {
            HBRUSH brush = CreateSolidBrush(th.selection);
            HGDIOBJ oldBrush = SelectObject(mem, brush);
            HGDIOBJ oldPen = SelectObject(mem, GetStockObject(NULL_PEN));
            RoundRect(mem, row.left, row.top, row.right + 1, row.bottom + 1,
                      l.radius / 2, l.radius / 2);
            SelectObject(mem, oldPen);
            SelectObject(mem, oldBrush);
            DeleteObject(brush);
        }
        SetTextColor(mem, selected ? th.selectionText : th.text);
        row.left += l.pad + Scale(2, static_cast<UINT>(l.dpi));
        DrawTextW(mem, kModeNames[i], -1, &row,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    }

    HBRUSH border = CreateSolidBrush(th.border);
    FrameRect(mem, &full, border);
    DeleteObject(border);

    BitBlt(dc, 0, 0, l.width, l.height, mem, 0, 0, SRCCOPY);

    SelectObject(mem, oldFont);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

int RowFromY(int y) {
    const Layout& l = g_picker.layout;
    if (y < l.rowsTop || l.rowH <= 0) {
        return -1;
    }
    const int row = (y - l.rowsTop) / l.rowH;
    return row < kModeCount ? row : -1;
}

void HidePicker() {
    g_pickerVisible = false;
    KillTimer(g_wnd, kWatchTimer);
    if (GetCapture() == g_wnd) {
        ReleaseCapture();
    }
    ShowWindow(g_wnd, SW_HIDE);

    ClipSnapshot().swap(g_picker.snapshot);
    std::wstring().swap(g_picker.original);
    for (int i = 0; i < kModeCount; ++i) {
        std::wstring().swap(g_picker.results[i]);
        std::wstring().swap(g_picker.display[i]);
    }
}

void ShowPicker(CaptureResult& capture) {
    PickerData& p = g_picker;
    p.target = capture.target;
    p.snapshot = std::move(capture.snapshot);
    p.original = std::move(capture.text);
    for (int i = 0; i < kModeCount; ++i) {
        p.results[i] = std::move(capture.results[i]);
        p.display[i] = MakeDisplay(p.results[i]);
    }
    p.sel = g_caseMode.load();
    p.pressed = -1;
    p.misses = 0;
    p.theme = GetTheme();
    GetCursorPos(&p.showCursor);

    const UINT dpi = DpiForPoint(p.target.anchor);
    EnsureFont(dpi);
    p.layout = MakeLayout(dpi);
    const Layout& l = p.layout;

    // Place below the anchor, flipping above it when there is no room, and
    // keep the whole picker inside the monitor's work area.
    RECT work = {0, 0, GetSystemMetrics(SM_CXSCREEN),
                 GetSystemMetrics(SM_CYSCREEN)};
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromPoint(p.target.anchor, MONITOR_DEFAULTTONEAREST);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        work = mi.rcWork;
    }
    int x = p.target.anchor.x;
    int y = p.target.anchor.y + Scale(8, dpi);
    if (y + l.height > work.bottom) {
        y = p.target.anchor.y - l.height - Scale(28, dpi);
    }
    if (x + l.width > work.right) {
        x = work.right - l.width;
    }
    x = x < work.left ? work.left : x;
    y = y < work.top ? work.top : y;

    SetWindowPos(g_wnd, HWND_TOPMOST, x, y, l.width, l.height,
                 SWP_NOACTIVATE);
    SetWindowRgn(g_wnd,
                 CreateRoundRectRgn(0, 0, l.width + 1, l.height + 1, l.radius,
                                    l.radius),
                 FALSE);
    InvalidateRect(g_wnd, nullptr, FALSE);
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateWindow(g_wnd);

    g_pickerVisible = true;
    SetTimer(g_wnd, kWatchTimer, kWatchIntervalMs, nullptr);
}

void CommitSelection(int index) {
    if (!g_pickerVisible || index < 0 || index >= kModeCount) {
        return;
    }

    g_caseMode = index;
    Wh_SetIntValue(L"ActiveMode", index);

    auto* job = new Job;
    job->kind = JobKind::Paste;
    job->target = g_picker.target;
    job->snapshot = std::move(g_picker.snapshot);
    job->text = std::move(g_picker.results[index]);
    const bool changed = job->text != g_picker.original;

    HidePicker();
    if (changed) {
        StartJob(job);
    } else {
        delete job;
    }
}

// Closes the picker when the user switches to another window, and keeps it on
// top of other topmost windows.
void OnWatchTimer() {
    if (!g_pickerVisible) {
        return;
    }
    if (GetForegroundWindow() != g_picker.target.fg) {
        if (++g_picker.misses >= 2) {
            HidePicker();
            return;
        }
    } else {
        g_picker.misses = 0;
    }

    if (!IsWindowVisible(g_wnd)) {
        ShowWindow(g_wnd, SW_SHOWNOACTIVATE);
    }
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam);

// (Re)installs both low-level hooks. Windows silently drops a low-level hook
// whose thread was ever too slow to answer, so this also runs periodically to
// make sure the shortcuts keep working.
bool InstallHooks() {
    HINSTANCE instance = GetModuleHandleW(nullptr);
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
    }
    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, instance, 0);
    g_mouseHook =
        SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, instance, 0);
    return g_keyboardHook && g_mouseHook;
}

LRESULT CALLBACK PickerWndProc(HWND hwnd,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_RENDERFORMAT:
            if (wParam == CF_UNICODETEXT) {
                RenderPasteText(false);
            }
            return 0;

        case WM_RENDERALLFORMATS:
            RenderPasteText(true);
            return 0;

        case WM_PAINT:
            PaintPicker(hwnd);
            return 0;

        case WM_DPICHANGED:
            return 0;  // the picker is sized explicitly on every show

        case WM_MOUSEMOVE: {
            // A picker that appears under a stationary pointer generates a
            // synthetic move; ignore it so the saved case stays selected.
            POINT cursor;
            GetCursorPos(&cursor);
            if (cursor.x == g_picker.showCursor.x &&
                cursor.y == g_picker.showCursor.y) {
                return 0;
            }
            const int row = RowFromY(static_cast<short>(HIWORD(lParam)));
            if (g_pickerVisible && row >= 0 && row != g_picker.sel) {
                g_picker.sel = row;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN:
            g_picker.pressed = RowFromY(static_cast<short>(HIWORD(lParam)));
            SetCapture(hwnd);
            return 0;

        case WM_LBUTTONUP: {
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            const int row = RowFromY(static_cast<short>(HIWORD(lParam)));
            const int pressed = g_picker.pressed;
            g_picker.pressed = -1;
            if (row >= 0 && row == pressed) {
                CommitSelection(row);
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == kWatchTimer) {
                OnWatchTimer();
            } else if (wParam == kHookTimer) {
                InstallHooks();
            }
            return 0;

        case WM_APP_TRIGGER:
            HidePicker();
            if (g_busy) {
                g_pendingTrigger = static_cast<int>(wParam);
            } else {
                StartTrigger(static_cast<int>(wParam));
            }
            return 0;

        case WM_APP_KEY:
            if (!g_pickerVisible) {
                return 0;
            }
            if (wParam == VK_UP) {
                g_picker.sel = (g_picker.sel + kModeCount - 1) % kModeCount;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == VK_DOWN) {
                g_picker.sel = (g_picker.sel + 1) % kModeCount;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == VK_RETURN) {
                CommitSelection(g_picker.sel);
            } else if (wParam == VK_ESCAPE) {
                HidePicker();
            }
            return 0;

        case WM_APP_DISMISS:
            HidePicker();
            return 0;

        case WM_APP_CAPTURED: {
            std::unique_ptr<CaptureResult> capture(
                reinterpret_cast<CaptureResult*>(lParam));
            g_busy = false;
            if (!RunPendingTrigger()) {
                ShowPicker(*capture);
            }
            return 0;
        }

        case WM_APP_JOB_DONE:
            g_busy = false;
            RunPendingTrigger();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks (run on the UI thread; they only post messages, never block)

// Releasing Alt with no other key in between activates the app's menu bar
// (Notepad, WordPad, Office, ...), which steals focus from the editor and makes
// the simulated Ctrl+C / Ctrl+V miss. A dummy key stroke while Alt is still
// held marks it as part of a combination.
void SendMenuMask() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 0xE8;  // unassigned virtual key
    in[1].ki.wVk = 0xE8;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

void Swallow(DWORD vk, DWORD time) {
    g_swallowed[vk] = true;
    g_swallowTime[vk] = time;
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    const DWORD vk = kb->vkCode;
    if ((kb->flags & LLKHF_INJECTED) || vk >= 256) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // A swallowed key-down is paired with swallowing its key-up. If the key-up
    // is ever missed, the stale flag expires (kRepeatWindowMs) and the next
    // press goes through normally instead of being eaten.
    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        if (g_swallowed[vk]) {
            g_swallowed[vk] = false;
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const bool repeat =
        g_swallowed[vk] && (kb->time - g_swallowTime[vk]) < kRepeatWindowMs;
    if (g_swallowed[vk] && !repeat) {
        g_swallowed[vk] = false;
    }

    if (g_pickerVisible) {
        if (vk == VK_UP || vk == VK_DOWN) {
            PostMessageW(g_wnd, WM_APP_KEY, vk, 0);
            Swallow(vk, kb->time);
            return 1;
        }
        if (vk == VK_RETURN || vk == VK_ESCAPE) {
            if (!repeat) {
                PostMessageW(g_wnd, WM_APP_KEY, vk, 0);
            }
            Swallow(vk, kb->time);
            return 1;
        }
        if (!IsModifierVk(vk) && !repeat) {
            PostMessageW(g_wnd, WM_APP_DISMISS, 0, 0);  // key still passes on
        }
    }

    if (repeat) {
        g_swallowTime[vk] = kb->time;
        return 1;
    }

    const Shortcut open = Shortcut::Unpack(g_openPacked.load());
    const Shortcut apply = Shortcut::Unpack(g_applyPacked.load());
    if (MatchesShortcut(open, vk)) {
        PostMessageW(g_wnd, WM_APP_TRIGGER, kTriggerOpen, 0);
        if (open.alt) {
            SendMenuMask();
        }
    } else if (MatchesShortcut(apply, vk)) {
        PostMessageW(g_wnd, WM_APP_TRIGGER, kTriggerApply, 0);
        if (apply.alt) {
            SendMenuMask();
        }
    } else {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    Swallow(vk, kb->time);
    return 1;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && g_pickerVisible) {
        switch (wParam) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN: {
                const auto* ms =
                    reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
                RECT rc;
                if (GetWindowRect(g_wnd, &rc) && !PtInRect(&rc, ms->pt)) {
                    PostMessageW(g_wnd, WM_APP_DISMISS, 0, 0);
                }
                break;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// UI thread

DWORD WINAPI UiThreadProc(LPVOID) {
    // Per-monitor DPI awareness (v2) for this thread, so coordinates and sizes
    // are physical pixels and the picker is crisp on high-DPI monitors.
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    auto setAwareness = reinterpret_cast<SetThreadDpiAwarenessContext_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "SetThreadDpiAwarenessContext"));
    if (setAwareness) {
        setAwareness(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4)));
    }

    g_shcore = LoadLibraryW(L"shcore.dll");
    if (g_shcore) {
        g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
            GetProcAddress(g_shcore, "GetDpiForMonitor"));
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = PickerWndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kPickerClass;
    const bool classRegistered = RegisterClassW(&wc) != 0;

    if (classRegistered) {
        g_wnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, kPickerClass,
            L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, instance, nullptr);
    }
    g_uiOk = g_wnd && InstallHooks();
    if (g_uiOk) {
        SetTimer(g_wnd, kHookTimer, kHookRefreshMs, nullptr);
    }
    SetEvent(g_uiReady);

    if (g_uiOk) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    // Let an in-flight copy/paste finish so the clipboard gets restored.
    g_stopping = true;
    SetEvent(g_pasteRendered);
    if (g_worker) {
        WaitForSingleObject(g_worker, 3000);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }

    if (g_wnd) {
        DestroyWindow(g_wnd);
        g_wnd = nullptr;
    }
    if (classRegistered) {
        UnregisterClassW(kPickerClass, instance);
    }
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }
    if (g_shcore) {
        FreeLibrary(g_shcore);
        g_shcore = nullptr;
    }
    return 0;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    LoadSettings();

    int mode = Wh_GetIntValue(L"ActiveMode", 0);
    g_caseMode = (mode >= 0 && mode < kModeCount) ? mode : 0;

    g_pasteRendered = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_pasteRendered || !g_uiReady) {
        return FALSE;
    }

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0,
                              &g_uiThreadId);
    if (!g_uiThread) {
        CloseHandle(g_uiReady);
        g_uiReady = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_uiReady, 10000);
    CloseHandle(g_uiReady);
    g_uiReady = nullptr;

    if (!g_uiOk) {
        Wh_Log(L"Failed to create the picker window or install hooks");
        WaitForSingleObject(g_uiThread, 2000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_uiThread) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, 8000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_pasteRendered) {
        CloseHandle(g_pasteRendered);
        g_pasteRendered = nullptr;
    }
}

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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
