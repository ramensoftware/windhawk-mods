// ==WindhawkMod==
// @id              arabic-keyboard-layout-fixer
// @name            Arabic Keyboard Layout Fixer
// @description     Instantly converts mistyped Arabic/English text with Alt+Z across any application.
// @version         1.3.0
// @author          Omar
// @github          https://github.com/uomr
// @homepage        https://github.com/uomr/keyboard-layout-fixer
// @include         windhawk.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Arabic Keyboard Layout Fixer

Fixes the common frustration of typing Arabic text while the keyboard is accidentally set to English (or vice versa).

### How to use:
1. Select/highlight any mistyped text in any app (Notepad, Word, Browser, ERP, etc.).
2. Press **Alt + Z** (or your custom configured hotkey).
3. The text instantly flips to the correct language!

### Features:
- **Bi-directional (AR <-> EN):** Automatically detects language direction.
- **Smart Shift / Caps Lock fix:** Resolves accidental Shift / Caps-Lock misfires.
- **Configurable hotkeys:** Customize your hotkey from mod settings without conflicts.
- **Universal compatibility:** Works in modern browsers, Office, and legacy Win32/ERP applications.
- **Dedicated tool process:** Runs safely as an independent background tool mod.
- **Full clipboard preservation:** Restores all clipboard formats (images, files, text) and keeps history clean.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: "Alt + Z"
  $name: "Conversion Hotkey"
  $description: "The shortcut key used to convert mistyped text. Modifiers: Ctrl, Shift, Alt. Keys: A-Z, 0-9, F1-F24, Pause, Insert, Delete, Home, End, Space. Example: 'Alt + Z', 'Ctrl + Alt + L', or 'Pause'."
- enable_secondary_hotkey: false
  $name: "Enable Secondary Hotkey (Ctrl + Shift + X)"
  $description: "Enable Ctrl + Shift + X as an alternative conversion hotkey."
- play_sound: true
  $name: "Play Audio Notification"
  $description: "Play a subtle beep notification after converting text."
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cwctype>
#include <atomic>

// ── Settings & Hotkey Configuration ─────────────────────────────────────────
struct HotkeyConfig {
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    WORD vk = 'Z';
};

static DWORD PackHotkey(const HotkeyConfig& cfg) {
    return (DWORD)(cfg.vk & 0xFFFF) |
           (cfg.ctrl  ? 0x10000 : 0) |
           (cfg.shift ? 0x20000 : 0) |
           (cfg.alt   ? 0x40000 : 0);
}

static HotkeyConfig UnpackHotkey(DWORD packed) {
    HotkeyConfig cfg;
    cfg.vk    = (WORD)(packed & 0xFFFF);
    cfg.ctrl  = (packed & 0x10000) != 0;
    cfg.shift = (packed & 0x20000) != 0;
    cfg.alt   = (packed & 0x40000) != 0;
    return cfg;
}

static std::atomic<DWORD> g_hotkeyPrimaryPacked{ PackHotkey({ false, false, true, 'Z' }) }; // Default: Alt + Z
static std::atomic<bool> g_enableSecondaryHotkey{ false };
static std::atomic<bool> g_playSound{ true };

static std::wstring ToLowerStr(const std::wstring& s) {
    std::wstring res = s;
    for (wchar_t& c : res) c = (wchar_t)towlower(c);
    return res;
}

static bool ParseHotkey(const std::wstring& str, HotkeyConfig& out) {
    HotkeyConfig cfg{ false, false, false, 0 };
    if (str.empty()) return false;

    size_t start = 0;
    while (start < str.length()) {
        size_t end = str.find(L'+', start);
        if (end == std::wstring::npos) end = str.length();

        std::wstring token = str.substr(start, end - start);
        size_t first = token.find_first_not_of(L" \t\r\n");
        size_t last = token.find_last_not_of(L" \t\r\n");

        if (first == std::wstring::npos || last == std::wstring::npos) {
            return false;
        }

        token = token.substr(first, last - first + 1);
        std::wstring lower = ToLowerStr(token);

        if (lower == L"ctrl" || lower == L"control") {
            cfg.ctrl = true;
        } else if (lower == L"shift") {
            cfg.shift = true;
        } else if (lower == L"alt") {
            cfg.alt = true;
        } else if (lower == L"pause" || lower == L"break") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_PAUSE;
        } else if (lower == L"insert") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_INSERT;
        } else if (lower == L"delete") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_DELETE;
        } else if (lower == L"home") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_HOME;
        } else if (lower == L"end") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_END;
        } else if (lower == L"space") {
            if (cfg.vk != 0) return false;
            cfg.vk = VK_SPACE;
        } else if (lower.length() >= 2 && lower[0] == L'f' && iswdigit(lower[1])) {
            int fNum = _wtoi(lower.c_str() + 1);
            if (fNum >= 1 && fNum <= 24) {
                if (cfg.vk != 0) return false;
                cfg.vk = (WORD)(VK_F1 + fNum - 1);
            } else {
                return false;
            }
        } else if (token.length() == 1) {
            wchar_t ch = towupper(token[0]);
            if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
                if (cfg.vk != 0) return false;
                cfg.vk = (WORD)ch;
            } else {
                return false;
            }
        } else {
            return false;
        }
        start = end + 1;
    }

    if (!cfg.vk) return false;

    bool standalone = (cfg.vk == VK_PAUSE || (cfg.vk >= VK_F1 && cfg.vk <= VK_F24));
    if (!standalone && !cfg.ctrl && !cfg.alt) {
        return false;
    }

    out = cfg;
    return true;
}

static void LoadSettings() {
    WindhawkUtils::StringSetting hkStr(Wh_GetStringSetting(L"hotkey"));
    HotkeyConfig cfg;
    if (!hkStr.get() || !ParseHotkey(hkStr.get(), cfg)) {
        Wh_Log(L"Invalid or empty hotkey \"%s\", defaulting to Alt + Z", hkStr.get() ? hkStr.get() : L"");
        cfg = { false, false, true, 'Z' };
    }
    g_hotkeyPrimaryPacked.store(PackHotkey(cfg));
    g_enableSecondaryHotkey.store(Wh_GetIntSetting(L"enable_secondary_hotkey") != 0);
    g_playSound.store(Wh_GetIntSetting(L"play_sound") != 0);
}

// ── Global State ────────────────────────────────────────────────────────────
static HHOOK g_hHook = nullptr;
static HANDLE g_hWorkerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_hConversionThread = nullptr;

// ── Maps ────────────────────────────────────────────────────────────────────
static std::unordered_map<wchar_t, std::wstring> g_en2ar;
static std::unordered_map<wchar_t, wchar_t>      g_ar2en;

static void InitMaps() {
    struct KeyDef { wchar_t en; const wchar_t* ar; };
    static const KeyDef mapEn2Ar[] = {
        { L'`', L"ذ" },  { L'~', L"ّ" },
        { L'q', L"ض" },  { L'Q', L"َ" },
        { L'w', L"ص" },  { L'W', L"ً" },
        { L'e', L"ث" },  { L'E', L"ُ" },
        { L'r', L"ق" },  { L'R', L"ٌ" },
        { L't', L"ف" },  { L'T', L"لإ" },
        { L'y', L"غ" },  { L'Y', L"إ" },
        { L'u', L"ع" },  { L'U', L"‘" },
        { L'i', L"ه" },  { L'I', L"÷" },
        { L'o', L"خ" },  { L'O', L"×" },
        { L'p', L"ح" },  { L'P', L"؛" },
        { L'[', L"ج" },  { L'{', L"<" },
        { L']', L"د" },  { L'}', L">" },
        { L'a', L"ش" },  { L'A', L"ِ" },
        { L's', L"س" },  { L'S', L"ٍ" },
        { L'd', L"ي" },  { L'D', L"]" },
        { L'f', L"ب" },  { L'F', L"[" },
        { L'g', L"ل" },  { L'G', L"لأ" },
        { L'h', L"ا" },  { L'H', L"أ" },
        { L'j', L"ت" },  { L'J', L"ـ" },
        { L'k', L"ن" },  { L'K', L"،" },
        { L'l', L"م" },  { L'L', L"/" },
        { L';', L"ك" },  { L':', L":" },
        { L'\'', L"ط" }, { L'"', L"\"" },
        { L'z', L"ئ" },  { L'Z', L"~" },
        { L'x', L"ء" },  { L'X', L"ْ" },
        { L'c', L"ؤ" },  { L'C', L"}" },
        { L'v', L"ر" },  { L'V', L"{" },
        { L'b', L"لا" }, { L'B', L"لآ" },
        { L'n', L"ى" },  { L'N', L"آ" },
        { L'm', L"ة" },  { L'M', L"’" },
        { L',', L"و" },  { L'<', L"," },
        { L'.', L"ز" },  { L'>', L"." },
        { L'/', L"ظ" },  { L'?', L"؟" },
    };

    g_en2ar.clear();
    g_ar2en.clear();

    for (const auto& k : mapEn2Ar) {
        g_en2ar[k.en] = k.ar;
        if (wcslen(k.ar) == 1) {
            g_ar2en[k.ar[0]] = k.en;
        }
    }
}

static bool IsArabicChar(wchar_t c) {
    return (c >= 0x0600 && c <= 0x06FF);
}

static std::wstring ArToEn(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        wchar_t c = s[i];
        if (c == 0xFEFB || c == 0xFEFC) { out += L'b'; continue; } // Lam-Alef (isolated / final)
        if (c == 0xFEF7 || c == 0xFEF8) { out += L'G'; continue; } // Lam-Alef with Hamza Above (isolated / final -> G)
        if (c == 0xFEF9 || c == 0xFEFA) { out += L'T'; continue; } // Lam-Alef with Hamza Below (isolated / final -> T)
        if (c == 0xFEF5 || c == 0xFEF6) { out += L'B'; continue; } // Lam-Alef with Madda Above (isolated / final -> B)

        if (c == L'ل' && (i + 1) < s.size()) {
            wchar_t next = s[i + 1];
            if      (next == L'ا') { out += L'b'; ++i; continue; }
            else if (next == L'إ') { out += L'T'; ++i; continue; }
            else if (next == L'أ') { out += L'G'; ++i; continue; }
            else if (next == L'آ') { out += L'B'; ++i; continue; }
        }

        auto it = g_ar2en.find(c);
        out += (it != g_ar2en.end()) ? it->second : c;
    }
    return out;
}

static std::wstring EnToAr(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size() * 2);
    for (wchar_t c : s) {
        auto it = g_en2ar.find(c);
        if (it != g_en2ar.end()) {
            const std::wstring& mapped = it->second;
            if (c >= L'A' && c <= L'Z') {
                bool isNonArabicSymbol = (!mapped.empty() && !IsArabicChar(mapped[0]));
                if (isNonArabicSymbol) {
                    wchar_t lower = (wchar_t)towlower(c);
                    auto itLower = g_en2ar.find(lower);
                    if (itLower != g_en2ar.end()) {
                        out += itLower->second;
                        continue;
                    }
                }
            }
            out += mapped;
        } else {
            out += c;
        }
    }
    return out;
}

static std::wstring ConvertSmart(const std::wstring& s) {
    int arCount = 0;
    int enCount = 0;
    for (wchar_t c : s) {
        if (IsArabicChar(c)) arCount++;
        else if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) enCount++;
    }
    if (arCount == 0 && enCount == 0) return s; // Keep text with no letters unchanged
    return (arCount >= enCount) ? ArToEn(s) : EnToAr(s);
}

// ── Multi-Format Clipboard Backup & Restoration ──────────────────────────────
struct ClipboardSnapshot {
    struct Item {
        UINT format;
        std::vector<BYTE> data;
    };
    std::vector<Item> items;
};

static BOOL OpenClipboardRetrying(HWND hwnd = nullptr, int maxAttempts = 10, DWORD delayMs = 20) {
    for (int i = 0; i < maxAttempts; ++i) {
        if (OpenClipboard(hwnd)) return TRUE;
        Sleep(delayMs);
    }
    return FALSE;
}

static UINT GetExcludeClipboardFormat() {
    static UINT s_fmt = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    return s_fmt;
}

static ClipboardSnapshot SaveClipboard() {
    ClipboardSnapshot snapshot;
    if (!OpenClipboardRetrying(nullptr)) return snapshot;

    UINT fmt = 0;
    while ((fmt = EnumClipboardFormats(fmt)) != 0) {
        if (fmt == CF_BITMAP || fmt == CF_ENHMETAFILE || fmt == CF_METAFILEPICT) continue;
        if (fmt >= 0xC000) {
            WCHAR name[256];
            if (GetClipboardFormatNameW(fmt, name, 256) > 0) {
                if (wcsstr(name, L"ExcludeClipboardContentFromMonitorProcessing") != nullptr) continue;
            }
        }

        HANDLE hData = GetClipboardData(fmt);
        if (!hData) continue;

        SIZE_T size = GlobalSize(hData);
        if (size == 0 || size > 32 * 1024 * 1024) continue;

        LPVOID ptr = GlobalLock(hData);
        if (ptr) {
            ClipboardSnapshot::Item item;
            item.format = fmt;
            item.data.resize(size);
            memcpy(item.data.data(), ptr, size);
            GlobalUnlock(hData);
            snapshot.items.push_back(std::move(item));
        }
    }

    CloseClipboard();
    return snapshot;
}

static void RestoreClipboard(const ClipboardSnapshot& snapshot) {
    if (!OpenClipboardRetrying(nullptr)) return;

    EmptyClipboard();

    if (snapshot.items.empty()) {
        CloseClipboard();
        return;
    }

    UINT excludeFmt = GetExcludeClipboardFormat();
    if (excludeFmt) {
        HGLOBAL hEx = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hEx) {
            PDWORD p = (PDWORD)GlobalLock(hEx);
            if (p) { *p = 1; GlobalUnlock(hEx); }
            if (!SetClipboardData(excludeFmt, hEx)) {
                GlobalFree(hEx);
            }
        }
    }

    for (const auto& item : snapshot.items) {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, item.data.size());
        if (hMem) {
            LPVOID dst = GlobalLock(hMem);
            if (dst) {
                memcpy(dst, item.data.data(), item.data.size());
                GlobalUnlock(hMem);
                if (!SetClipboardData(item.format, hMem)) {
                    GlobalFree(hMem);
                }
            } else {
                GlobalFree(hMem);
            }
        }
    }

    CloseClipboard();
}

static std::wstring GetClipboardText() {
    std::wstring res;
    if (!OpenClipboardRetrying(nullptr)) return res;

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData) {
        auto* text = (const wchar_t*)GlobalLock(hData);
        if (text) {
            res = text;
            GlobalUnlock(hData);
        }
    }
    CloseClipboard();
    return res;
}

static void SetClipboardText(const std::wstring& text) {
    if (!OpenClipboardRetrying(nullptr)) return;

    EmptyClipboard();

    UINT excludeFmt = GetExcludeClipboardFormat();
    if (excludeFmt) {
        HGLOBAL hEx = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hEx) {
            PDWORD p = (PDWORD)GlobalLock(hEx);
            if (p) { *p = 1; GlobalUnlock(hEx); }
            SetClipboardData(excludeFmt, hEx);
        }
    }

    SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        auto* dst = (wchar_t*)GlobalLock(hMem);
        if (dst) {
            memcpy(dst, text.c_str(), bytes);
            GlobalUnlock(hMem);
            if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
                GlobalFree(hMem);
            }
        } else {
            GlobalFree(hMem);
        }
    }
    CloseClipboard();
}

static HWND GetFocusedControlHandle() {
    HWND fg = GetForegroundWindow();
    if (!fg) return nullptr;

    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(fg, &pid);
    GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
    if (GetGUIThreadInfo(tid, &gti) && gti.hwndFocus) {
        return gti.hwndFocus;
    }
    return fg;
}

static bool IsConsoleWindow(HWND hwnd) {
    if (!hwnd) return false;
    WCHAR className[128] = { 0 };
    if (GetClassNameW(hwnd, className, 127) > 0) {
        if (_wcsicmp(className, L"ConsoleWindowClass") == 0 ||
            _wcsicmp(className, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0) {
            return true;
        }
    }
    return false;
}

static void SendCtrlKey(WORD vk) {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = vk;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(4, inputs, sizeof(INPUT));
}

static void ReleaseModifiers() {
    INPUT inputs[5] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_CONTROL;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = VK_MENU;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_SHIFT;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[4].type = INPUT_KEYBOARD;
    inputs[4].ki.wVk = VK_RMENU;
    inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(5, inputs, sizeof(INPUT));
}

// ── Conversion Execution Thread ─────────────────────────────────────────────
static DWORD WINAPI ExecuteConversionThread(LPVOID) {
    HWND hTarget = GetFocusedControlHandle();
    if (IsConsoleWindow(hTarget) || IsConsoleWindow(GetForegroundWindow())) {
        return 0; // Avoid terminal interrupt
    }

    ReleaseModifiers();
    Sleep(50);

    // 1. Snapshot entire current clipboard
    ClipboardSnapshot backup = SaveClipboard();
    DWORD seqBefore = GetClipboardSequenceNumber();

    auto readCopiedText = [&]() -> std::wstring {
        if (GetClipboardSequenceNumber() == seqBefore) return {};
        return GetClipboardText();
    };

    // 2. Perform copy
    SendCtrlKey('C');

    std::wstring selected;
    for (int i = 0; i < 6; ++i) {
        Sleep(25);
        selected = readCopiedText();
        if (!selected.empty()) break;
    }

    if (selected.empty()) {
        SendCtrlKey('C');
        for (int i = 0; i < 4; ++i) {
            Sleep(25);
            selected = readCopiedText();
            if (!selected.empty()) break;
        }
    }

    bool usedWmCopy = false;
    if (selected.empty() && hTarget) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hTarget, WM_COPY, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result);
        Sleep(50);
        selected = readCopiedText();
        if (!selected.empty()) usedWmCopy = true;
    }

    if (selected.empty()) {
        RestoreClipboard(backup);
        return 0;
    }

    // 3. Convert text
    std::wstring converted = ConvertSmart(selected);
    if (converted == selected) {
        RestoreClipboard(backup);
        return 0;
    }

    // 4. Put converted text in clipboard
    SetClipboardText(converted);
    Sleep(40);

    // 5. Paste: mutually exclusive to avoid double-pasting
    if (usedWmCopy && hTarget) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hTarget, WM_PASTE, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result);
    } else {
        SendCtrlKey('V');
    }

    // 6. Allow target application to process paste before restoring original clipboard
    Sleep(250);
    RestoreClipboard(backup);
    if (g_playSound.load()) MessageBeep(MB_OK);

    return 0;
}

// ── Hook Callback (Strict Modifier Matching & No Injected Keys) ─────────────
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;

        if (kb->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        bool alt  = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;
        bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shft = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
        bool win  = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

        HotkeyConfig primaryHk = UnpackHotkey(g_hotkeyPrimaryPacked.load());
        bool enableSecondary = g_enableSecondaryHotkey.load();

        bool isPrimary = (alt == primaryHk.alt &&
                          ctrl == primaryHk.ctrl &&
                          shft == primaryHk.shift &&
                          !win &&
                          kb->vkCode == primaryHk.vk);

        bool isSecondary = (enableSecondary &&
                            ctrl && shft && !alt && !win &&
                            kb->vkCode == 'X');

        if (isPrimary || isSecondary) {
            if (!g_hConversionThread ||
                WaitForSingleObject(g_hConversionThread, 0) == WAIT_OBJECT_0) {
                if (g_hConversionThread) CloseHandle(g_hConversionThread);
                g_hConversionThread = CreateThread(nullptr, 0, ExecuteConversionThread, nullptr, 0, nullptr);
            }
            return 1; // Consume keystroke immediately
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// ── Worker Thread for Hook Message Loop ─────────────────────────────────────
static HANDLE g_hHookThreadReadyEvent = nullptr;

static DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);

    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (!g_hHook) {
        Wh_Log(L"Failed to install hook.");
        if (g_hHookThreadReadyEvent) SetEvent(g_hHookThreadReadyEvent);
        return 0;
    }
    Wh_Log(L"Hook installed successfully.");

    if (g_hHookThreadReadyEvent) {
        SetEvent(g_hHookThreadReadyEvent);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
    return 0;
}

// ── Windhawk Tool Mod Callbacks ─────────────────────────────────────────────
BOOL WhTool_ModInit() {
    InitMaps();
    LoadSettings();
    Wh_Log(L"Initializing tool mod...");

    g_hHookThreadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_hWorkerThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Failed to spawn hook thread.");
        if (g_hHookThreadReadyEvent) {
            CloseHandle(g_hHookThreadReadyEvent);
            g_hHookThreadReadyEvent = nullptr;
        }
        return FALSE;
    }

    if (g_hHookThreadReadyEvent) {
        WaitForSingleObject(g_hHookThreadReadyEvent, 5000);
        CloseHandle(g_hHookThreadReadyEvent);
        g_hHookThreadReadyEvent = nullptr;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading tool mod...");
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, INFINITE);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }
    if (g_hConversionThread) {
        WaitForSingleObject(g_hConversionThread, INFINITE);
        CloseHandle(g_hConversionThread);
        g_hConversionThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

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
