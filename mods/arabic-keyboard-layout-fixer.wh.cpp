// ==WindhawkMod==
// @id              arabic-keyboard-layout-fixer
// @name            Arabic Keyboard Layout Fixer
// @description     Instantly converts mistyped Arabic/English text with Alt+Z across any application.
// @version         1.0.0
// @author          Omar
// @github          https://github.com/uomr
// @homepage        https://github.com/uomr/keyboard-layout-fixer
// @include         explorer.exe
// @compilerOptions -luser32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Arabic Keyboard Layout Fixer

Fixes the common frustration of typing Arabic text while the keyboard is accidentally set to English (or vice versa).

### How to use:
1. Select/highlight any mistyped text in any app (Notepad, Word, Browser, ERP, etc.).
2. Press **Alt + Z** (or **Ctrl + Shift + X**).
3. The text instantly flips to the correct language!

### Features:
- **Bi-directional (AR <-> EN):** Automatically detects language direction.
- **Smart Shift / Caps Lock fix:** Resolves accidental Shift / Caps-Lock misfires.
- **Universal compatibility:** Works in modern browsers and legacy Win32/ERP applications.
- **Zero background overhead:** Runs natively inside Windows Explorer.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <string>
#include <unordered_map>
#include <cwctype>

// ── Global State ────────────────────────────────────────────────────────────
static HHOOK g_hHook = NULL;
static HANDLE g_hWorkerThread = NULL;
static DWORD g_workerThreadId = 0;
static volatile LONG g_isConverting = 0;

// ── Maps ────────────────────────────────────────────────────────────────────
static std::unordered_map<wchar_t, std::wstring> g_en2ar;
static std::unordered_map<wchar_t, wchar_t> g_ar2en;

static bool IsArabicLetter(wchar_t c) {
    return (c >= 0x0600 && c <= 0x06FF);
}

static void InitMaps() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

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
        // Ligature handling
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
            // Caps-Lock / Shift fallback to lowercase Arabic letter
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

// ── Clipboard Helpers ───────────────────────────────────────────────────────
static std::wstring GetClipboardText() {
    std::wstring text;
    if (!OpenClipboard(NULL)) return text;
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
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    if (!text.empty()) {
        size_t bytes = (text.length() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (hMem) {
            void* p = GlobalLock(hMem);
            if (p) {
                memcpy(p, text.c_str(), bytes);
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
    }
    CloseClipboard();
}

static HWND GetFocusedControlHandle() {
    HWND activeWnd = GetForegroundWindow();
    if (!activeWnd) return NULL;

    DWORD pid = 0;
    DWORD activeThreadId = GetWindowThreadProcessId(activeWnd, &pid);
    DWORD currentThreadId = GetCurrentThreadId();

    if (activeThreadId != currentThreadId) {
        AttachThreadInput(currentThreadId, activeThreadId, TRUE);
        HWND focusWnd = GetFocus();
        AttachThreadInput(currentThreadId, activeThreadId, FALSE);
        return focusWnd ? focusWnd : activeWnd;
    }
    return activeWnd;
}

static void ReleaseModifiers() {
    // Tap Ctrl before releasing Alt to prevent Windows from activating the window menu bar (SC_KEYMENU)
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
}

// ── Conversion Execution Worker ─────────────────────────────────────────────
static DWORD WINAPI ExecuteConversionThread(LPVOID) {
    ReleaseModifiers();
    Sleep(80);

    HWND hTarget = GetFocusedControlHandle();
    std::wstring backup = GetClipboardText();
    SetClipboardText(L"");

    bool usedWmCopy = false;

    // Try Ctrl+C
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event('C', 0, 0, 0);
    Sleep(35);
    keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

    std::wstring selected;
    for (int i = 0; i < 15; ++i) {
        Sleep(25);
        selected = GetClipboardText();
        if (!selected.empty()) break;
    }

    // Web input copy retry
    if (selected.empty()) {
        Sleep(50);
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event('C', 0, 0, 0);
        Sleep(35);
        keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

        for (int i = 0; i < 15; ++i) {
            Sleep(25);
            selected = GetClipboardText();
            if (!selected.empty()) break;
        }
    }

    // ERP WM_COPY fallback
    if (selected.empty() && hTarget) {
        SendMessage(hTarget, WM_COPY, 0, 0);
        Sleep(50);
        selected = GetClipboardText();
        if (!selected.empty()) usedWmCopy = true;
    }

    if (selected.empty()) {
        SetClipboardText(backup);
        InterlockedExchange(&g_isConverting, 0);
        return 0;
    }

    std::wstring converted = ConvertSmart(selected);
    if (converted == selected) {
        SetClipboardText(backup);
        InterlockedExchange(&g_isConverting, 0);
        return 0;
    }

    SetClipboardText(converted);
    Sleep(40);

    // Paste: mutually exclusive to avoid double-pasting
    if (usedWmCopy && hTarget) {
        SendMessage(hTarget, WM_PASTE, 0, 0);
    } else {
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event('V', 0, 0, 0);
        Sleep(35);
        keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    }

    Sleep(150);
    SetClipboardText(backup);
    MessageBeep(MB_OK);

    InterlockedExchange(&g_isConverting, 0);
    return 0;
}

// ── Hook Callback ───────────────────────────────────────────────────────────
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        bool alt  = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;
        bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shft = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;

        bool trigger =
            (alt && kb->vkCode == 'Z') ||               // Alt + Z
            (ctrl && shft && kb->vkCode == 'X');        // Ctrl + Shift + X

        if (trigger) {
            if (InterlockedCompareExchange(&g_isConverting, 1, 0) == 0) {
                HANDLE hThread = CreateThread(NULL, 0, ExecuteConversionThread, NULL, 0, NULL);
                if (hThread) CloseHandle(hThread);
            }
            return 1; // Consume keystroke immediately
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// ── Worker Thread for Hook Message Loop ─────────────────────────────────────
static DWORD WINAPI HookThreadProc(LPVOID) {
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hHook) {
        Wh_Log(L"Arabic Keyboard Fixer: Failed to install hook.");
        return 0;
    }
    Wh_Log(L"Arabic Keyboard Fixer: Hook installed successfully.");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
    return 0;
}

// ── Windhawk Entry Points ───────────────────────────────────────────────────
BOOL Wh_ModInit() {
    InitMaps();
    Wh_Log(L"Arabic Keyboard Fixer: Initializing mod...");

    g_hWorkerThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Arabic Keyboard Fixer: Failed to spawn hook thread.");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Arabic Keyboard Fixer: Unloading mod...");
    if (g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, 1500);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }
}
