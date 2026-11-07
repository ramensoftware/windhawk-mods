// ==WindhawkMod==
// @id              arabic-keyboard-layout-fixer
// @name            Arabic Keyboard Layout Fixer
// @description     Instantly converts mistyped Arabic/English text with Alt+Z across any application.
// @version         1.0.0
// @author          Omar
// @github          https://github.com/uomr
// @homepage        https://github.com/uomr/keyboard-layout-fixer
// @include         windhawk.exe
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
- **Dedicated tool process:** Runs safely as an independent background tool mod.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <string>
#include <unordered_map>
#include <cwctype>

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
    if (!OpenClipboard(nullptr)) return text;
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
    if (!OpenClipboard(nullptr)) return;
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
    if (!activeWnd) return nullptr;

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

static bool IsConsoleWindow(HWND hWnd) {
    if (!hWnd) return false;
    wchar_t className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"ConsoleWindowClass") == 0) return true;
    }
    return false;
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
    HWND hForeground = GetForegroundWindow();
    if (IsConsoleWindow(hForeground)) {
        return 0; // Skip terminal windows to avoid sending SIGINT (Ctrl+C break)
    }

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

    // ERP WM_COPY fallback using SendMessageTimeout (prevents hangs if target unresponsive)
    if (selected.empty() && hTarget) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hTarget, WM_COPY, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result);
        Sleep(50);
        selected = GetClipboardText();
        if (!selected.empty()) usedWmCopy = true;
    }

    if (selected.empty()) {
        SetClipboardText(backup);
        return 0;
    }

    std::wstring converted = ConvertSmart(selected);
    if (converted == selected) {
        SetClipboardText(backup);
        return 0;
    }

    SetClipboardText(converted);
    Sleep(40);

    // Paste: mutually exclusive to avoid double-pasting
    if (usedWmCopy && hTarget) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hTarget, WM_PASTE, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result);
    } else {
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event('V', 0, 0, 0);
        Sleep(35);
        keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    }

    // 250ms delay before restoring clipboard to give asynchronous apps time to read
    Sleep(250);
    SetClipboardText(backup);
    MessageBeep(MB_OK);

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
            // Check if previous conversion finished before starting a new one
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
        Wh_Log(L"Arabic Keyboard Fixer: Failed to install hook.");
        return 0;
    }
    Wh_Log(L"Arabic Keyboard Fixer: Hook installed successfully.");

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
    Wh_Log(L"Arabic Keyboard Fixer: Initializing tool mod...");

    g_hWorkerThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"Arabic Keyboard Fixer: Failed to spawn hook thread.");
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    Wh_Log(L"Arabic Keyboard Fixer: Unloading tool mod...");
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, 1500);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }
    if (g_hConversionThread) {
        WaitForSingleObject(g_hConversionThread, 1500);
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
