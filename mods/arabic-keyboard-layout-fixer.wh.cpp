// ==WindhawkMod==
// @id              arabic-keyboard-layout-fixer
// @name            Arabic Keyboard Layout Fixer
// @description     Instantly converts mistyped Arabic/English text with Alt+Z across any application.
// @version         1.1.0
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
  $description: "The shortcut key used to convert mistyped text (e.g. Alt + Z, Ctrl + Alt + L, Pause, F8). Default: Alt + Z."
- enable_secondary_hotkey: false
  $name: "Enable Secondary Hotkey (Ctrl + Shift + X)"
  $description: "Enable Ctrl + Shift + X as an alternative conversion hotkey."
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cwctype>

// ── Settings & Hotkey Configuration ─────────────────────────────────────────
struct HotkeyConfig {
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    WORD vk = 'Z';
};

static HotkeyConfig g_hotkeyPrimary = { false, false, true, 'Z' }; // Default: Alt + Z
static bool g_enableSecondaryHotkey = false;

static std::wstring ToLowerStr(const std::wstring& s) {
    std::wstring res = s;
    for (wchar_t& c : res) c = (wchar_t)towlower(c);
    return res;
}

static void ParseHotkey(const std::wstring& str, HotkeyConfig& cfg, bool defCtrl, bool defShift, bool defAlt, WORD defVk) {
    cfg.ctrl = false; cfg.shift = false; cfg.alt = false; cfg.vk = 0;
    if (str.empty()) {
        cfg.ctrl = defCtrl; cfg.shift = defShift; cfg.alt = defAlt; cfg.vk = defVk;
        return;
    }
    size_t start = 0;
    while (start < str.length()) {
        size_t end = str.find(L'+', start);
        if (end == std::string::npos) end = str.length();

        std::wstring token = str.substr(start, end - start);
        size_t first = token.find_first_not_of(L" \t\r\n");
        size_t last = token.find_last_not_of(L" \t\r\n");

        if (first != std::string::npos && last != std::string::npos) {
            token = token.substr(first, last - first + 1);
            std::wstring lower = ToLowerStr(token);

            if (lower == L"ctrl" || lower == L"control") cfg.ctrl = true;
            else if (lower == L"shift") cfg.shift = true;
            else if (lower == L"alt") cfg.alt = true;
            else if (lower == L"pause" || lower == L"break") cfg.vk = VK_PAUSE;
            else if (lower == L"insert") cfg.vk = VK_INSERT;
            else if (lower == L"delete") cfg.vk = VK_DELETE;
            else if (lower == L"home") cfg.vk = VK_HOME;
            else if (lower == L"end") cfg.vk = VK_END;
            else if (lower == L"space") cfg.vk = VK_SPACE;
            else if (lower.length() >= 2 && lower[0] == L'f' && iswdigit(lower[1])) {
                int fNum = _wtoi(lower.c_str() + 1);
                if (fNum >= 1 && fNum <= 24) cfg.vk = (WORD)(VK_F1 + fNum - 1);
            } else if (token.length() == 1) {
                wchar_t ch = towupper(token[0]);
                if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) cfg.vk = (WORD)ch;
            }
        }
        start = end + 1;
    }
    if (cfg.vk == 0) cfg.vk = defVk;
}

static void LoadSettings() {
    PCWSTR hkStr = Wh_GetStringSetting(L"hotkey");
    if (hkStr) {
        ParseHotkey(hkStr, g_hotkeyPrimary, false, false, true, 'Z');
        Wh_FreeStringSetting(hkStr);
    }
    g_enableSecondaryHotkey = Wh_GetIntSetting(L"enable_secondary_hotkey") != 0;
}

// ── Global State ────────────────────────────────────────────────────────────
static HHOOK g_hHook = nullptr;
static HANDLE g_hWorkerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_hConversionThread = nullptr;

// ── Maps ────────────────────────────────────────────────────────────────────
static std::unordered_map<wchar_t, std::wstring> g_en2ar;
static std::unordered_map<wchar_t, wchar_t> g_ar2en;

static bool IsArabicLetter(wchar_t c) {
    return (c >= 0x0600 && c <= 0x06FF);
}

static void InitMaps() {
    struct MapEntry { wchar_t en; const wchar_t* ar; };
    MapEntry entries[] = {
        { L'`', L"ذ" }, { L'q', L"ض" }, { L'w', L"ص" }, { L'e', L"ث" }, { L'r', L"ق" },
        { L't', L"ف" }, { L'y', L"غ" }, { L'u', L"ع" }, { L'i', L"ه" }, { L'o', L"خ" },
        { L'p', L"ح" }, { L'[', L"ج" }, { L']', L"د" }, { L'a', L"ش" }, { L's', L"س" },
        { L'd', L"ي" }, { L'f', L"ب" }, { L'g', L"ل" }, { L'h', L"ا" }, { L'j', L"ت" },
        { L'k', L"ن" }, { L'l', L"م" }, { L';', L"ك" }, { L'\'', L"ط" }, { L'z', L"ئ" },
        { L'x', L"ء" }, { L'c', L"ؤ" }, { L'v', L"ر" }, { L'b', L"لا" }, { L'n', L"ى" },
        { L'm', L"ة" }, { L',', L"و" }, { L'.', L"ز" }, { L'/', L"ظ" },
        // Shifted / Uppercase mappings
        { L'~', L"ّ" }, { L'Q', L"َ" }, { L'W', L"ً" }, { L'E', L"ُ" }, { L'R', L"ٌ" },
        { L'T', L"لإ" }, { L'Y', L"إ" }, { L'U', L"'" }, { L'I', L"÷" }, { L'O', L"×" },
        { L'P', L"؛" }, { L'{', L"<" }, { L'}', L">" }, { L'A', L"ِ" }, { L'S', L"ٍ" },
        { L'D', L"]" }, { L'F', L"[" }, { L'G', L"لأ" }, { L'H', L"أ" }, { L'J', L"ـ" },
        { L'K', L"،" }, { L'L', L"/" }, { L':', L":" }, { L'\"', L"\"" }, { L'Z', L"~" },
        { L'X', L"ْ" }, { L'C', L"}" }, { L'V', L"{" }, { L'B', L"لآ" }, { L'N', L"آ" },
        { L'M', L"'" }, { L'<', L"," }, { L'>', L"." }, { L'?', L"؟" }
    };

    for (const auto& item : entries) {
        g_en2ar[item.en] = item.ar;
        if (wcslen(item.ar) == 1) {
            g_ar2en[item.ar[0]] = item.en;
        }
    }
    g_ar2en[L'ل'] = L'g'; // Fallback
}

// ── Conversion Logic ────────────────────────────────────────────────────────
static std::wstring ArToEn(const std::wstring& text) {
    std::wstring out;
    size_t len = text.length();
    for (size_t i = 0; i < len; ++i) {
        if (text[i] == L'ل' && i + 1 < len) {
            wchar_t next = text[i + 1];
            if (next == L'آ') { out += L'B'; ++i; continue; }
            if (next == L'إ') { out += L'T'; ++i; continue; }
            if (next == L'أ') { out += L'G'; ++i; continue; }
            if (next == L'ا') { out += L'b'; ++i; continue; }
        }
        auto it = g_ar2en.find(text[i]);
        if (it != g_ar2en.end()) out += it->second;
        else out += text[i];
    }
    return out;
}

static std::wstring EnToAr(const std::wstring& text) {
    std::wstring out;
    for (wchar_t c : text) {
        auto it = g_en2ar.find(c);
        if (it != g_en2ar.end()) {
            std::wstring res = it->second;
            if (iswupper(c) && !IsArabicLetter(res[0])) {
                wchar_t lower = towlower(c);
                auto itLower = g_en2ar.find(lower);
                if (itLower != g_en2ar.end() && IsArabicLetter(itLower->second[0])) {
                    res = itLower->second;
                }
            }
            out += res;
        } else {
            out += c;
        }
    }
    return out;
}

static std::wstring ConvertSmart(const std::wstring& text) {
    if (text.empty()) return text;
    int en = 0, ar = 0;
    for (wchar_t c : text) {
        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) en++;
        else if (IsArabicLetter(c)) ar++;
    }
    return (ar >= en && ar > 0) ? ArToEn(text) : EnToAr(text);
}

// ── Full Clipboard Snapshot & Preservation ──────────────────────────────────
struct ClipboardSnapshot {
    struct Entry {
        UINT format;
        std::vector<BYTE> bytes;
    };
    std::vector<Entry> formats;
};

static bool OpenClipboardRetrying(HWND hWnd = nullptr, bool wait = true) {
    for (int i = 0; i < (wait ? 10 : 1); i++) {
        if (OpenClipboard(hWnd)) return true;
        if (wait) Sleep(20);
    }
    return false;
}

static ClipboardSnapshot SaveClipboard() {
    constexpr size_t kMaxBytes = 32 * 1024 * 1024;
    ClipboardSnapshot snapshot;
    if (!OpenClipboardRetrying()) return snapshot;

    size_t total = 0;
    for (UINT format = 0; (format = EnumClipboardFormats(format)) != 0;) {
        if (format == CF_BITMAP || format == CF_ENHMETAFILE || format == CF_METAFILEPICT ||
            format == CF_PALETTE || format == CF_OWNERDISPLAY || format == CF_DSPBITMAP ||
            format == CF_DSPENHMETAFILE || format == CF_DSPMETAFILEPICT ||
            format == CF_DIBV5 || format == CF_TEXT || format == CF_OEMTEXT) {
            continue;
        }
        HANDLE data = GetClipboardData(format);
        SIZE_T size = data ? GlobalSize(data) : 0;
        if (!size || total + size > kMaxBytes) continue;

        if (const void* p = GlobalLock(data)) {
            snapshot.formats.push_back({ format, std::vector<BYTE>((const BYTE*)p, (const BYTE*)p + size) });
            total += size;
            GlobalUnlock(data);
        }
    }
    CloseClipboard();
    return snapshot;
}

static void RestoreClipboard(const ClipboardSnapshot& snapshot) {
    if (!OpenClipboardRetrying()) return;
    EmptyClipboard();
    for (const auto& entry : snapshot.formats) {
        if (HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, entry.bytes.size())) {
            if (void* p = GlobalLock(memory)) {
                memcpy(p, entry.bytes.data(), entry.bytes.size());
                GlobalUnlock(memory);
                if (SetClipboardData(entry.format, memory)) {
                    continue;
                }
            }
            GlobalFree(memory);
        }
    }
    if (!snapshot.formats.empty()) {
        static const UINT cfExclude = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
        if (HGLOBAL flag = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD))) {
            if (!SetClipboardData(cfExclude, flag)) {
                GlobalFree(flag);
            }
        }
    }
    CloseClipboard();
}

static std::wstring GetClipboardText() {
    std::wstring text;
    if (!OpenClipboardRetrying()) return text;
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData) {
        wchar_t* p = (wchar_t*)GlobalLock(hData);
        if (p) {
            text = p;
            GlobalUnlock(hData);
        }
    }
    CloseClipboard();
    return text;
}

static void SetClipboardText(const std::wstring& text) {
    if (!OpenClipboardRetrying()) return;
    EmptyClipboard();
    if (!text.empty()) {
        size_t bytes = (text.length() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (hMem) {
            void* p = GlobalLock(hMem);
            if (p) {
                memcpy(p, text.c_str(), bytes);
                GlobalUnlock(hMem);
                if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
                    GlobalFree(hMem);
                }
            } else {
                GlobalFree(hMem);
            }
        }
    }
    static const UINT cfExclude = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    if (HGLOBAL flag = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD))) {
        if (!SetClipboardData(cfExclude, flag)) {
            GlobalFree(flag);
        }
    }
    CloseClipboard();
}

// ── Target Control Handle (via GetGUIThreadInfo — No AttachThreadInput) ─────
static HWND GetFocusedControlHandle() {
    HWND activeWnd = GetForegroundWindow();
    if (!activeWnd) return nullptr;

    GUITHREADINFO gti = { sizeof(gti) };
    if (GetGUIThreadInfo(GetWindowThreadProcessId(activeWnd, nullptr), &gti) && gti.hwndFocus) {
        return gti.hwndFocus;
    }
    return activeWnd;
}

static bool IsConsoleWindow(HWND hWnd) {
    if (!hWnd) return false;
    wchar_t className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"ConsoleWindowClass") == 0 ||
            wcscmp(className, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0) return true;
    }
    return false;
}

// ── Keyboard Synthesis via SendInput ────────────────────────────────────────
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
    INPUT inputs[4] = {};
    // Tap Ctrl to suppress SC_KEYMENU
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_MENU; // Alt
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = VK_CONTROL;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_SHIFT;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

// ── Conversion Execution Worker ─────────────────────────────────────────────
static DWORD WINAPI ExecuteConversionThread(LPVOID) {
    HWND hForeground = GetForegroundWindow();
    if (IsConsoleWindow(hForeground)) {
        return 0; // Skip console/terminal windows
    }

    ReleaseModifiers();
    Sleep(80);

    HWND hTarget = GetFocusedControlHandle();

    // 1. Save full clipboard snapshot
    ClipboardSnapshot backup = SaveClipboard();
    DWORD seqBefore = GetClipboardSequenceNumber();

    // Helper: accept copied text ONLY if clipboard sequence number changed
    auto readCopiedText = [&]() -> std::wstring {
        if (GetClipboardSequenceNumber() == seqBefore) return {};
        return GetClipboardText();
    };

    bool usedWmCopy = false;

    // 2. Send Ctrl+C via SendInput
    SendCtrlKey('C');

    std::wstring selected;
    for (int i = 0; i < 15; ++i) {
        Sleep(25);
        selected = readCopiedText();
        if (!selected.empty()) break;
    }

    // Web input copy retry
    if (selected.empty()) {
        Sleep(50);
        SendCtrlKey('C');
        for (int i = 0; i < 15; ++i) {
            Sleep(25);
            selected = readCopiedText();
            if (!selected.empty()) break;
        }
    }

    // ERP WM_COPY fallback with SendMessageTimeout
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
    MessageBeep(MB_OK);

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

        bool isPrimary = (alt == g_hotkeyPrimary.alt &&
                          ctrl == g_hotkeyPrimary.ctrl &&
                          shft == g_hotkeyPrimary.shift &&
                          !win &&
                          kb->vkCode == g_hotkeyPrimary.vk);

        bool isSecondary = (g_enableSecondaryHotkey &&
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
static DWORD WINAPI HookThreadProc(LPVOID) {
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (!g_hHook) {
        Wh_Log(L"Failed to install hook.");
        return 0;
    }
    Wh_Log(L"Hook installed successfully.");

    MSG msg;
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

    g_hWorkerThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Failed to spawn hook thread.");
        return FALSE;
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
