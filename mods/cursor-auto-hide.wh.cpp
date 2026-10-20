// ==WindhawkMod==
// @id              cursor-auto-hide
// @name            Cursor auto-hide on idle
// @description     Hides the mouse cursor after a configurable idle timeout and shows it again as soon as the mouse moves or a button is clicked
// @version         1.1
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         windhawk.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Cursor auto-hide on idle

The mouse cursor usually stays on screen even when you are not using the
mouse, covering videos, text and presentations.

This mod hides the cursor after it has not moved for a configurable
number of seconds (5 by default). The cursor reappears immediately on
the first movement or click.

Cursor changes happen at runtime only: your cursor scheme is never
written to the registry, and any mouse activity restores it right away.

Only the system cursors are replaced. Applications that draw their own
cursor, such as games or drawing tools, keep showing it.

A global hotkey turns the auto-hide on and off without opening the
settings. The shortcut works in any application and the cursor stays
visible for as long as the feature is switched off. The switch is a
runtime one: it starts enabled again the next time Windhawk starts.

The shortcut does not have to be typed. Tick "Record the shortcut by
pressing it" in the settings, press Save, then press the combination you
want within 60 seconds. While the window is open, a note in the bottom
right corner of the screen says so. The mod writes the combination to the
clipboard in the format the field expects, so it can be pasted in instead
of being typed.

Click a window other than the Windhawk settings before pressing the
combination. While the settings window keeps the focus, every key you
press also lands in the settings fields, so the values you are about to
save can change under you and the field you type into is not the one you
will paste into afterwards.

Three rules decide whether a combination is accepted. It has to
include Ctrl, Alt or Win, because the shortcut is global and takes its
key away from every application - Shift+A would stop capital A from
being typed and Shift+End would stop text selection, while Ctrl+Alt+H
collides with nothing. F1 to F24, Pause and ScrollLock are the
exception and may be used alone or with Shift. Every other key takes
part as well, punctuation, lock keys and numeric keypad included, and
the main block gives you ", . / [ ] \ ' ; - = `". And the combination
has to be pressed before you click the field and paste, because
recording is still open at that point, so pressing Ctrl+V records
Ctrl+V instead of your combination. Recording ends on the first
accepted combination, after 60 seconds, or when the box is unticked
and Save is pressed again.

Recording is opened by ticking the box and pressing Save, and by nothing
else: later saves with the box still ticked do not open a new window, so
nothing you press in between can reach the clipboard. To record another
shortcut, untick the box, press Save, then tick it and press Save again.

The mod runs in a dedicated Windhawk process (as a tool mod) and does
not inject into other applications.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleSeconds: 5
  $name: Idle timeout (seconds)
  $description: The cursor is hidden this many seconds after the last mouse movement or click. Set to 0 to disable hiding.
- toggleHotkey: ""
  $name: Toggle hotkey
  $description: A global shortcut that turns the auto-hide on and off, for example Ctrl+Shift+F9. Write it as modifiers joined with plus signs and a key name, such as Ctrl+Alt+H, Shift+F9 or Win+F10. Modifiers are Ctrl, Alt, Shift and Win; keys are A to Z, 0 to 9, F1 to F24, Esc, Space, Tab, Enter, Backspace, the navigation keys, the punctuation keys, the lock keys and the numeric keypad. Because the shortcut is global it takes its key away from every application, so it needs Ctrl, Alt or Win; F1 to F24, Pause and ScrollLock are the exception and may be used alone or with Shift. Leave empty to switch it off, and a shortcut another program already holds is logged while the mod keeps running without one.
- recordHotkey: false
  $name: Record the shortcut by pressing it
  $description: Tick this box and press Save, then press the shortcut you want within 60 seconds. A note appears in the bottom right corner of the screen so you know recording has started. Click a window other than this one before pressing, because while these settings keep the focus your keys also go into the fields below and change what you are about to save. The shortcut is put on the clipboard in the format the field above expects, so press it first, then click that field, press Ctrl+V, and untick this box; if recording is still open then, Ctrl+V is what gets recorded. The same rules as above apply, so a shortcut needs Ctrl, Alt or Win unless it is F1 to F24, Pause or ScrollLock. Recording stops after the first shortcut, after 60 seconds, or when you untick the box and press Save again. It opens only when you tick the box, so to record again you untick, press Save, then tick and press Save again.
*/
// ==/WindhawkModSettings==

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <atomic>
#include <string>

#include <windhawk_utils.h>

// The setting can be updated from another thread while the worker thread
// reads it, so an atomic type is used to avoid a data race.
std::atomic<int> g_idleSeconds{5};

// Parsed form of the toggle hotkey setting: the settings callback only
// rewrites these, the worker thread is what registers the shortcut.
std::atomic<UINT> g_hotkeyModifiers{0};
std::atomic<UINT> g_hotkeyVk{0};
// Set when the shortcut has to be registered or removed again.
std::atomic<bool> g_hotkeyDirty{true};

// Recording mode: armed by the settings callback, driven by the worker.
std::atomic<bool> g_recordRequested{false};
// The "record the shortcut by pressing it" checkbox as last saved.
std::atomic<bool> g_recordEnabled{false};

HANDLE g_stopEvent = nullptr;
HANDLE g_threadHandle = nullptr;

// The state below is only touched by the worker thread: raw input
// events and the idle check are processed on that thread.
ULONGLONG g_lastActivity = 0;
bool g_hidden = false;
POINT g_lastPolledPos{};
// Runtime switch driven by the hotkey: true while the idle fade works.
bool g_fadeEnabled = true;
// Whether the worker window currently owns a registered shortcut.
bool g_hotkeyRegistered = false;
// Recording state, touched by the worker thread only.
bool g_recording = false;
ULONGLONG g_recordDeadline = 0;
bool g_recordWarned = false;
// The note shown on screen while recording is open.
HWND g_recordHint = nullptr;

// Only the settings callback thread touches these: they compare the settings
// against the previous save to tell a ticked box from an edited field.
std::wstring g_prevToggleHotkey;
bool g_prevRecordEnabled = false;

constexpr int kHotkeyId = 1;
// How long the recording window stays open after a settings save.
constexpr ULONGLONG kRecordWindowMs = 60000;

// Restores the cursors if the process dies while they are modified.
LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter = nullptr;

// OEM resource cursor IDs (see winuser.h). <windows.h> only declares the
// OCR_* names when OEMRESOURCE is defined before its first inclusion, so
// the values are spelled out here to avoid include-order surprises.
// HideCursors replaces every cursor in this list.
constexpr int kSystemCursorIds[] = {
    32512,  // OCR_NORMAL
    32513,  // OCR_IBEAM
    32514,  // OCR_WAIT
    32515,  // OCR_CROSS
    32516,  // OCR_UP
    32642,  // OCR_SIZENWSE
    32643,  // OCR_SIZENESW
    32644,  // OCR_SIZEWE
    32645,  // OCR_SIZENS
    32646,  // OCR_SIZEALL
    32648,  // OCR_NO
    32649,  // OCR_HAND
    32650,  // OCR_APPSTARTING
    32651,  // OCR_HELP
    32631,  // NWPen (handwriting)
    32671,  // Pin (location select)
    32672,  // Person (person select)
};

struct NamedKey {
    const WCHAR* name;
    UINT vk;
};

// Matching is case-insensitive, but this spelling is also what hotkey
// recording puts on the clipboard, and it has to go back into the
// settings field unchanged, so keep it readable.
//
// One table serves both directions: KeyNameToVk looks a name up and
// VkToKeyName searches for the key, so every shortcut that can be typed
// can also be recorded and pasted back. A recorded shortcut must
// therefore never land on a key that is missing here - it would be
// dropped in silence. Virtual-key codes for the punctuation keys are
// tied to the physical key rather than to the layout, so these names
// describe the US layout; they still parse to the same key on any
// layout, which is what RegisterHotKey needs.
static const NamedKey kNamedKeys[] = {
    {L"Esc", VK_ESCAPE},       {L"Escape", VK_ESCAPE},
    {L"Space", VK_SPACE},      {L"Tab", VK_TAB},
    {L"Enter", VK_RETURN},     {L"Return", VK_RETURN},
    {L"Backspace", VK_BACK},   {L"Insert", VK_INSERT},
    {L"Delete", VK_DELETE},    {L"Home", VK_HOME},
    {L"End", VK_END},          {L"PageUp", VK_PRIOR},
    {L"PageDown", VK_NEXT},    {L"Left", VK_LEFT},
    {L"Right", VK_RIGHT},      {L"Up", VK_UP},
    {L"Down", VK_DOWN},

    // Punctuation on the main block. "=" is written as "=" and never as
    // "+", because ParseHotkey splits the setting on "+".
    {L"-", VK_OEM_MINUS},      {L"=", VK_OEM_PLUS},
    {L",", VK_OEM_COMMA},      {L".", VK_OEM_PERIOD},
    {L";", VK_OEM_1},          {L"/", VK_OEM_2},
    {L"`", VK_OEM_3},          {L"[", VK_OEM_4},
    {L"\\", VK_OEM_5},         {L"]", VK_OEM_6},
    {L"'", VK_OEM_7},          {L"<", VK_OEM_102},

    // Lock keys and the rest of the modifiers' neighbours.
    {L"CapsLock", VK_CAPITAL}, {L"NumLock", VK_NUMLOCK},
    {L"ScrollLock", VK_SCROLL},{L"PrtSc", VK_SNAPSHOT},
    {L"Pause", VK_PAUSE},      {L"Menu", VK_APPS},

    // Numeric keypad. The operator names are spelled out so that none of
    // them contains "+", which ParseHotkey would split on.
    {L"Numpad0", VK_NUMPAD0},  {L"Numpad1", VK_NUMPAD1},
    {L"Numpad2", VK_NUMPAD2},  {L"Numpad3", VK_NUMPAD3},
    {L"Numpad4", VK_NUMPAD4},  {L"Numpad5", VK_NUMPAD5},
    {L"Numpad6", VK_NUMPAD6},  {L"Numpad7", VK_NUMPAD7},
    {L"Numpad8", VK_NUMPAD8},  {L"Numpad9", VK_NUMPAD9},
    {L"NumpadStar", VK_MULTIPLY},
    {L"NumpadPlus", VK_ADD},
    {L"NumpadMinus", VK_SUBTRACT},
    {L"NumpadDot", VK_DECIMAL},
    {L"NumpadSlash", VK_DIVIDE},
};

// The only keys a shortcut may use without Ctrl, Alt or Win. RegisterHotKey
// takes its key away from every application in the session, so nearly every
// shortcut has to carry one of the three strong modifiers: Shift+A would
// stop capital A from being typed, Shift+End would stop text selection,
// Shift+Delete would stop cutting, and a bare Delete would stop deleting.
// Function keys, Pause and ScrollLock have no role in normal typing, so
// they stay usable alone or with Shift - they are the keys people reach
// for once the combinations that mean something are all taken.
static bool AllowsWeakModifiers(UINT vk) {
    return (vk >= VK_F1 && vk <= VK_F24) || vk == VK_PAUSE ||
           vk == VK_SCROLL;
}

// Maps a key name to a virtual key: single letters and digits use their
// own code, F1..F24 use VK_F1..VK_F24, everything else comes from the
// table above. Returns 0 when the name is unknown.
static UINT KeyNameToVk(const WCHAR* name) {
    if (name[0] != L'\0' && name[1] == L'\0') {
        WCHAR c = name[0];
        if (c >= L'a' && c <= L'z') {
            c = c - L'a' + L'A';
        }
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return (UINT)c;
        }
    }

    if ((name[0] == L'F' || name[0] == L'f') && name[1] != L'\0') {
        int n = 0;
        bool digits = true;
        for (const WCHAR* p = name + 1; *p; p++) {
            if (*p < L'0' || *p > L'9') {
                digits = false;
                break;
            }
            n = n * 10 + (*p - L'0');
        }
        if (digits && n >= 1 && n <= 24) {
            return VK_F1 + (n - 1);
        }
    }

    for (const NamedKey& k : kNamedKeys) {
        if (_wcsicmp(name, k.name) == 0) {
            return k.vk;
        }
    }
    return 0;
}

// Parses a shortcut written as "Ctrl+Shift+F9" into RegisterHotKey
// arguments. Returns false when a token is not understood, so the mod
// can log it instead of registering a key the user did not ask for.
static bool ParseHotkey(const WCHAR* text, UINT* outModifiers, UINT* outVk) {
    UINT modifiers = 0;
    UINT vk = 0;
    bool haveKey = false;

    const WCHAR* p = text;
    while (*p != L'\0') {
        // Read one token, up to the next '+'. Surrounding spaces are
        // allowed so "Ctrl + Shift + F9" parses as well.
        WCHAR token[64];
        size_t len = 0;
        while (*p != L'\0' && *p != L'+') {
            if (*p != L' ' && *p != L'\t') {
                if (len < ARRAYSIZE(token) - 1) {
                    token[len++] = *p;
                }
            }
            p++;
        }
        token[len] = L'\0';
        if (*p == L'+') {
            p++;
        }

        if (len == 0) {
            continue;  // an empty segment, such as a trailing "+"
        }

        if (_wcsicmp(token, L"ctrl") == 0 || _wcsicmp(token, L"control") == 0) {
            modifiers |= MOD_CONTROL;
        } else if (_wcsicmp(token, L"alt") == 0) {
            modifiers |= MOD_ALT;
        } else if (_wcsicmp(token, L"shift") == 0) {
            modifiers |= MOD_SHIFT;
        } else if (_wcsicmp(token, L"win") == 0 ||
                   _wcsicmp(token, L"windows") == 0) {
            modifiers |= MOD_WIN;
        } else {
            UINT key = KeyNameToVk(token);
            if (key == 0 || haveKey) {
                return false;
            }
            vk = key;
            haveKey = true;
        }
    }

    if (!haveKey) {
        return false;
    }
    // The shortcut is registered globally, so without Ctrl, Alt or Win
    // only the keys listed in AllowsWeakModifiers are usable: "Shift+End"
    // would stop text selection and a bare "Delete" would stop deleting.
    if (!AllowsWeakModifiers(vk) &&
        !(modifiers & (MOD_CONTROL | MOD_ALT | MOD_WIN))) {
        return false;
    }
    *outModifiers = modifiers;
    *outVk = vk;
    return true;
}

// fromSettingsCallback is true only for saves made from the settings
// window: a restart with the box still ticked must not start listening
// for keys on its own.
void LoadSettings(bool fromSettingsCallback) {
    int idleSeconds = Wh_GetIntSetting(L"idleSeconds");
    if (idleSeconds < 0) {
        idleSeconds = 0;
    }
    g_idleSeconds.store(idleSeconds);

    // An empty setting means "no shortcut". A setting the parser does
    // not understand is treated the same way and reported by the caller.
    UINT modifiers = 0;
    UINT vk = 0;
    bool valid = false;
    // StringSetting frees the value it holds when it goes out of scope,
    // so there is nothing to release by hand here. The one-argument
    // constructor does not read a setting, it takes ownership of whatever
    // pointer it is given, so it has to be make() that calls
    // Wh_GetStringSetting.
    auto toggleHotkeySetting =
        WindhawkUtils::StringSetting::make(L"toggleHotkey");
    std::wstring hotkeyText = toggleHotkeySetting.get();
    if (!hotkeyText.empty()) {
        valid = ParseHotkey(hotkeyText.c_str(), &modifiers, &vk);
        if (!valid) {
            Wh_Log(L"Toggle hotkey not accepted: %s (an unknown token, or a "
                   L"key used without Ctrl, Alt or Win)",
                   toggleHotkeySetting.get());
        }
    }

    g_hotkeyModifiers.store(valid ? modifiers : 0);
    g_hotkeyVk.store(valid ? vk : 0);
    // Ask the worker thread to (re)register the shortcut.
    g_hotkeyDirty.store(true);

    bool record = Wh_GetIntSetting(L"recordHotkey") != 0;
    // The box is a stored setting, not a button, so it stays ticked after
    // recording. Recording is therefore armed by nothing but the
    // transition from unticked to ticked: a later save that simply
    // re-saves the settings - changing the idle timeout, say - must not
    // open a capture window, because the first combination pressed in any
    // application during it, Alt+Tab or Ctrl+C included, would overwrite
    // the clipboard. The other excluded case is a save which also edits
    // the shortcut field: that save already carries a shortcut of its own.
    bool fieldChanged =
        fromSettingsCallback && (hotkeyText != g_prevToggleHotkey);
    bool justTicked =
        fromSettingsCallback && record && !g_prevRecordEnabled;
    g_prevToggleHotkey = hotkeyText;
    g_prevRecordEnabled = record;
    g_recordEnabled.store(record);
    if (justTicked && !fieldChanged) {
        g_recordRequested.store(true);
    }
}

// Reverse of KeyNameToVk: writes a name that ParseHotkey accepts back for
// the given virtual key. Returns false when the key has no name, which
// means it cannot be written into the setting at all.
static bool VkToKeyName(UINT vk, WCHAR* out, size_t outCount) {
    if (outCount == 0) {
        return false;
    }
    if ((vk >= L'A' && vk <= L'Z') || (vk >= L'0' && vk <= L'9')) {
        if (outCount < 2) {
            return false;
        }
        out[0] = (WCHAR)vk;
        out[1] = L'\0';
        return true;
    }
    if (vk >= VK_F1 && vk <= VK_F24) {
        // "F24" is the longest result, so four characters plus the
        // terminator always fit once there is room for five.
        if (outCount < 5) {
            return false;
        }
        swprintf_s(out, outCount, L"F%u", vk - VK_F1 + 1);
        return true;
    }
    for (const NamedKey& k : kNamedKeys) {
        if (k.vk == vk) {
            size_t length = wcslen(k.name);
            if (length + 1 > outCount) {
                return false;
            }
            memcpy(out, k.name, (length + 1) * sizeof(WCHAR));
            return true;
        }
    }
    return false;
}

// Builds exactly the text the settings field accepts, so a recorded
// shortcut can be pasted straight back into it.
static std::wstring FormatHotkey(UINT modifiers, UINT vk) {
    std::wstring text;
    if (modifiers & MOD_CONTROL) {
        text += L"Ctrl+";
    }
    if (modifiers & MOD_ALT) {
        text += L"Alt+";
    }
    if (modifiers & MOD_SHIFT) {
        text += L"Shift+";
    }
    if (modifiers & MOD_WIN) {
        text += L"Win+";
    }
    WCHAR name[32];
    if (VkToKeyName(vk, name, ARRAYSIZE(name))) {
        text += name;
    }
    return text;
}

// Puts text on the clipboard. The window is passed as the owner rather
// than nullptr: with no owner, EmptyClipboard leaves the clipboard ownerless
// and SetClipboardData is then allowed to fail. Which call broke is handed
// back to the caller, because "OpenClipboard failed" would otherwise be
// logged even when it was the allocation that failed.
static bool CopyToClipboard(HWND owner,
                            const std::wstring& text,
                            PCWSTR* failedCall,
                            DWORD* failedError) {
    *failedCall = L"";
    *failedError = 0;
    auto fail = [&](PCWSTR call) {
        *failedCall = call;
        *failedError = GetLastError();
        return false;
    };

    if (!OpenClipboard(owner)) {
        return fail(L"OpenClipboard");
    }
    bool ok = false;
    do {
        if (!EmptyClipboard()) {
            fail(L"EmptyClipboard");
            break;
        }
        size_t bytes = (text.size() + 1) * sizeof(WCHAR);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (!memory) {
            fail(L"GlobalAlloc");
            break;
        }
        void* buffer = GlobalLock(memory);
        if (!buffer) {
            fail(L"GlobalLock");
            GlobalFree(memory);
            break;
        }
        memcpy(buffer, text.c_str(), bytes);
        GlobalUnlock(memory);
        if (!SetClipboardData(CF_UNICODETEXT, memory)) {
            fail(L"SetClipboardData");
            GlobalFree(memory);
            break;
        }
        // Ownership of memory passes to the clipboard on success.
        ok = true;
    } while (false);
    CloseClipboard();
    return ok;
}

// Resolved at run time rather than imported: GetDpiForWindow only exists
// from Windows 10 1607 on, and the classic 96 is the right answer for any
// system that does not have it.
static UINT DpiForWindow(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static GetDpiForWindow_t pGetDpiForWindow = (GetDpiForWindow_t)
        GetProcAddress(GetModuleHandle(L"user32.dll"), "GetDpiForWindow");
    if (!pGetDpiForWindow) {
        return 96;
    }
    UINT dpi = pGetDpiForWindow(hwnd);
    return dpi ? dpi : 96;
}

// A small always-on-top note shown while the shortcut window is open, so
// that recording is something the user sees rather than has to guess at.
// Owned by the worker thread, which creates, shows and destroys it.
void ShowRecordHint() {
    if (g_recordHint) {
        return;
    }
    WCHAR text[160];
    swprintf_s(text,
               L"Cursor auto-hide: click another window, then press the "
               L"shortcut within %u seconds.",
               (unsigned)(kRecordWindowMs / 1000));

    // The note belongs on the monitor the pointer is over, not on the
    // primary one whatever the user is looking at, and its size has to
    // follow that monitor's scale: 620x64 carries the text with room to
    // spare at 96 DPI, but the text grows with the display scale and
    // would be cut off from 125% upwards.
    POINT cursor{};
    GetCursorPos(&cursor);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    RECT area{};
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {
        area = monitorInfo.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &area, 0);
    }

    const int width = 620;
    const int height = 64;
    // WS_EX_NOACTIVATE matters: the hint must not steal focus from the
    // settings window the user is working in.
    // No font is set: asking GDI for one would add a gdi32 link
    // dependency, and the control already paints with the system font.
    // SS_CENTERIMAGE keeps that font vertically centred in the note.
    g_recordHint = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, L"STATIC", text,
        WS_POPUP | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
        area.right - width - 12, area.bottom - height - 12, width, height,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!g_recordHint) {
        Wh_Log(L"Recording hint window failed: %u", GetLastError());
        return;
    }

    // The window was created at its 96 DPI size and is still hidden, so
    // it can be scaled to the monitor it sits on before being shown.
    int dpi = (int)DpiForWindow(g_recordHint);
    int noteWidth = MulDiv(width, dpi, 96);
    int noteHeight = MulDiv(height, dpi, 96);
    int margin = MulDiv(12, dpi, 96);
    SetWindowPos(g_recordHint, nullptr, area.right - noteWidth - margin,
                 area.bottom - noteHeight - margin, noteWidth, noteHeight,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    ShowWindow(g_recordHint, SW_SHOWNOACTIVATE);
    UpdateWindow(g_recordHint);
}

void HideRecordHint() {
    if (!g_recordHint) {
        return;
    }
    DestroyWindow(g_recordHint);
    g_recordHint = nullptr;
}

static bool IsModifierVk(UINT vk) {
    switch (vk) {
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_LWIN:
        case VK_RWIN:
            return true;
        default:
            return false;
    }
}

// One poll of the open recording window: returns true when a shortcut was
// found and copied to the clipboard, which closes the window.
//
// The keyboard is read with GetAsyncKeyState rather than raw input. Raw
// input does not report a key-down at all while Ctrl and Alt are both
// held - which is exactly the shape of most shortcuts - so depending on
// it would make recording fail for the common case.
static bool RecordPollStep(HWND hwnd) {
    UINT modifiers = 0;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        modifiers |= MOD_CONTROL;
    }
    if (GetAsyncKeyState(VK_MENU) & 0x8000) {
        modifiers |= MOD_ALT;
    }
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        modifiers |= MOD_SHIFT;
    }
    if ((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
        (GetAsyncKeyState(VK_RWIN) & 0x8000)) {
        modifiers |= MOD_WIN;
    }
    // The rule is the one the settings field applies, so nothing can be
    // recorded that the field would then refuse: without Ctrl, Alt or Win
    // only F keys, Pause and ScrollLock qualify, and Shift+A is waited out
    // instead of being put on the clipboard.
    const bool strongModifiers =
        (modifiers & (MOD_CONTROL | MOD_ALT | MOD_WIN)) != 0;

    // VK_BACK is the first keyboard key; 1..6 are the mouse buttons,
    // which can be held down while a shortcut is pressed.
    for (UINT vk = VK_BACK; vk < 256; vk++) {
        if (IsModifierVk(vk) || !(GetAsyncKeyState(vk) & 0x8000)) {
            continue;
        }
        if (!strongModifiers && !AllowsWeakModifiers(vk)) {
            continue;
        }
        WCHAR name[32];
        if (!VkToKeyName(vk, name, ARRAYSIZE(name))) {
            if (!g_recordWarned) {
                g_recordWarned = true;
                Wh_Log(L"Key 0x%X has no name and cannot be recorded", vk);
            }
            return false;
        }
        std::wstring text = FormatHotkey(modifiers, vk);
        PCWSTR failedCall = L"";
        DWORD failedError = 0;
        if (!CopyToClipboard(hwnd, text, &failedCall, &failedError)) {
            if (!g_recordWarned) {
                g_recordWarned = true;
                Wh_Log(L"Could not put %s on the clipboard: %s failed: %u",
                       text.c_str(), failedCall, failedError);
            }
            return false;
        }
        Wh_Log(L"Recorded %s - paste it into the toggle hotkey field",
               text.c_str());
        return true;
    }
    return false;
}

// A fully transparent 32x32 cursor: the AND mask keeps the screen, the
// XOR mask changes nothing.
HCURSOR CreateBlankCursor() {
    BYTE andMask[128];
    for (BYTE& b : andMask) {
        b = 0xFF;
    }
    BYTE xorMask[128] = {};
    return CreateCursor(GetModuleHandle(nullptr), 0, 0, 32, 32, andMask,
                        xorMask);
}

// SetSystemCursor takes ownership of the handle it receives (and
// destroys it), so every system cursor gets its own copy.
void HideCursors() {
    // Remember that the cursors are modified, so the next start of this
    // tool can put them back if the process dies without cleaning up.
    Wh_SetIntValue(L"cursorsModified", 1);

    HCURSOR blank = CreateBlankCursor();
    if (!blank) {
        Wh_Log(L"CreateCursor failed");
        return;
    }
    for (int id : kSystemCursorIds) {
        HCURSOR copy =
            (HCURSOR)CopyImage(blank, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
        if (copy && !SetSystemCursor(copy, id)) {
            // Ownership transfers only on success.
            DestroyCursor(copy);
        }
    }
    DestroyCursor(blank);
    Wh_Log(L"Cursors hidden");
}

// Reloads the current cursor scheme; runtime replacements are dropped.
void RestoreCursors() {
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_SetIntValue(L"cursorsModified", 0);
    Wh_Log(L"Cursors restored");
}

// Cursors left transparent by a crash would stay that way, so restore
// them as early as possible and pass the exception on.
LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS* exceptionInfo) {
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    return g_previousExceptionFilter
               ? g_previousExceptionFilter(exceptionInfo)
               : EXCEPTION_CONTINUE_SEARCH;
}

// A press-and-hold without movement (holding a scrollbar arrow, pausing
// mid-drag) is not an idle mouse: the cursors stay visible.
bool IsMouseButtonHeld() {
    constexpr int kButtons[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
                                VK_XBUTTON1, VK_XBUTTON2};
    for (int button : kButtons) {
        if (GetAsyncKeyState(button) & 0x8000) {
            return true;
        }
    }
    return false;
}

// Registers or removes the toggle shortcut on the worker window. Called
// from the worker thread only, because RegisterHotKey binds the key to
// the thread that owns the window.
void UpdateHotkey(HWND hwnd) {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(hwnd, kHotkeyId);
        g_hotkeyRegistered = false;
        Wh_Log(L"Toggle hotkey unregistered");
    }

    UINT vk = g_hotkeyVk.load();
    if (vk == 0) {
        // Nothing will be able to switch auto-hide back on any more, so it
        // must not be left switched off: the run-time toggle has no other
        // way to be reached, and no on-screen state shows that it is off.
        g_fadeEnabled = true;
        return;  // no shortcut configured
    }

    UINT modifiers = g_hotkeyModifiers.load() | MOD_NOREPEAT;
    if (!RegisterHotKey(hwnd, kHotkeyId, modifiers, vk)) {
        Wh_Log(L"RegisterHotKey failed: %u (key already taken?)",
               GetLastError());
        // Same as above: with the shortcut unavailable, the switch would
        // stay wherever the last key press left it until a restart.
        g_fadeEnabled = true;
        return;
    }
    g_hotkeyRegistered = true;
    Wh_Log(L"Toggle hotkey registered: modifiers=0x%X vk=0x%X", modifiers, vk);
}

// Flips the fade switch. Turning it off has to bring the cursor back at
// once, otherwise it would stay invisible until the next mouse move.
void ToggleFade() {
    g_fadeEnabled = !g_fadeEnabled;
    g_lastActivity = GetTickCount64();  // restart the idle countdown
    Wh_Log(L"Auto-hide %s", g_fadeEnabled ? L"enabled" : L"disabled");
    if (!g_fadeEnabled && g_hidden) {
        g_hidden = false;
        RestoreCursors();
    }
}

DWORD WINAPI WorkerThread(LPVOID /*param*/) {
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPolledPos);

    // Raw input delivers WM_INPUT for every mouse movement, button and
    // wheel event to a message-only window. Unlike a low-level hook it
    // is not part of the session's input path, so the work done here
    // (replacing the cursors) can never delay the mouse, and Windows
    // never drops the notification for being slow.
    HWND hwnd = CreateWindowEx(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, nullptr, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed");
        return 1;
    }

    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;  // Generic desktop controls
    device.usUsage = 0x02;      // Mouse
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = hwnd;
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
        DestroyWindow(hwnd);
        return 1;
    }
    Wh_Log(L"Raw input registered");
    g_hotkeyDirty.exchange(false);  // registering right here anyway
    UpdateHotkey(hwnd);

    bool stopping = false;
    while (!stopping) {
        // Wake up on raw input, on the stop event, or after roughly the
        // interval used for the idle check. While the cursor is hidden
        // the loop runs more often so that movement is noticed quickly.
        // While a recording window is open the keys are polled, so the
        // wait has to be short enough not to miss a quick press.
        DWORD wait =
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                       g_recording ? 20
                                                   : (g_hidden ? 100 : 500),
                                       QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            stopping = true;
        }

        bool mouseActivity = false;
        bool hotkeyPressed = false;
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_INPUT) {
                mouseActivity = true;
            } else if (msg.message == WM_HOTKEY && (int)msg.wParam == kHotkeyId) {
                hotkeyPressed = true;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);  // DefWindowProc frees the raw data.
        }
        if (stopping) {
            break;
        }

        // The settings callback runs on another thread and cannot
        // touch the worker window, so re-registering is done here.
        if (g_hotkeyDirty.exchange(false)) {
            UpdateHotkey(hwnd);
        }

        if (hotkeyPressed) {
            ToggleFade();
        }

        // Hotkey recording: opened by ticking the box and saving, closed
        // by the first shortcut, by the timeout, or by unticking the box.
        if (g_recordRequested.exchange(false)) {
            g_recording = true;
            g_recordWarned = false;
            g_recordDeadline = GetTickCount64() + kRecordWindowMs;
            Wh_Log(L"Recording a shortcut, press it within %u seconds",
                   (unsigned)(kRecordWindowMs / 1000));
            ShowRecordHint();
        }
        if (g_recording) {
            if (!g_recordEnabled.load()) {
                g_recording = false;
                Wh_Log(L"Recording cancelled");
            } else if (GetTickCount64() >= g_recordDeadline) {
                g_recording = false;
                Wh_Log(L"Recording timed out, nothing was pressed");
            } else if (RecordPollStep(hwnd)) {
                g_recording = false;
            }
        }
        if (!g_recording) {
            HideRecordHint();  // a no-op unless the note is still up
        }

        if (mouseActivity) {
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        }

        // Polling the cursor position is a safety net for the cases raw
        // input doesn't cover, and it reads global state, so no special
        // privileges are needed. When it fails, another desktop (the
        // lock screen, a UAC prompt) has the input: activity there
        // can't be observed, so the cursors must not stay hidden while
        // the mod can't see the mouse.
        POINT polledPos;
        if (!GetCursorPos(&polledPos)) {
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        } else if (polledPos.x != g_lastPolledPos.x ||
                   polledPos.y != g_lastPolledPos.y) {
            g_lastPolledPos = polledPos;
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        }

        int idleSeconds = g_idleSeconds.load();
        if (idleSeconds <= 0 || !g_fadeEnabled) {
            // Hiding is disabled, by the setting or by the hotkey; make
            // sure the cursor is shown again if it was hidden when the
            // switch was flipped.
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
            continue;
        }

        ULONGLONG now = GetTickCount64();
        if (!g_hidden &&
            now - g_lastActivity >= (ULONGLONG)idleSeconds * 1000 &&
            !IsMouseButtonHeld()) {
            HideCursors();
            g_hidden = true;
        }
    }

    HideRecordHint();
    if (g_hotkeyRegistered) {
        UnregisterHotKey(hwnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    DestroyWindow(hwnd);
    if (g_hidden) {
        g_hidden = false;
        RestoreCursors();
    }
    Wh_Log(L"Worker stopped");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings(false);

    // The previous run ended while the cursors were modified (a crash or
    // a process killed from Task Manager), so put the normal cursors back.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreCursors();
    }

    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(RestoreCursorsOnCrash);

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_threadHandle = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings(true);
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    // Wait for the worker thread without a timeout: it may be inside
    // HideCursors or RestoreCursors, and unloading the mod while it
    // runs would crash the process.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    // Safety net in case the worker couldn't clean up in time.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreCursors();
    }

    SetUnhandledExceptionFilter(g_previousExceptionFilter);
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