// ==WindhawkMod==
// @id         always-on-top
// @name       Always On Top Windows
// @description Pin/unpin active window with Ctrl+Alt+T
// @version    1.1
// @author     AhmedAwad7
// @github     https://github.com/AhmedAwad7
// @license    MIT
// @include    explorer.exe
// @compilerOptions -luser32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always On Top Windows

![Screenrecord](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/gif.gif)

## English

Press **`Ctrl+Alt+T`** to pin/unpin the active window on top of all other windows.

This mod helps you multitask by keeping important windows always visible.

**Features:**
- 🔊 Sound notifications to indicate pin/unpin actions.
- ⚡ Event-driven hotkey using `RegisterHotKey` (no continuous polling).
- 🧹 Lightweight: runs in a dedicated `explorer.exe` process.

**Requirements:**
- Windhawk (latest version recommended).
- Windows 7 → Works efficiently
- Windows 8 → Works efficiently
- Windows 8.1 → Works efficiently
- Windows 10/11 → Not tested yet (if it works well for you, please let me know in Issues on GitHub)

**Contributing:**
We welcome contributors! Feel free to open Issues or Pull Requests on GitHub.
---
## العربية

اضغط **`Ctrl+Alt+T`** لتثبيت/إلغاء تثبيت النافذة النشطة فوق جميع النوافذ الأخرى.

هذا المود يساعدك في تعدد المهام عن طريق تثبيت النوافذ المهمة لتظل مرئية دائماً.

**المميزات:**
- 🔊 إشعارات صوتية لتوضيح التثبيت\إلغاء التثبيت.
- ⚡ اختصار يعتمد على الأحداث (`RegisterHotKey`) بدون استقصاء مستمر.
- 🧹 خفيف الوزن: يعمل في عملية مخصصة لـ `explorer.exe`.

**متطلبات النظام:**
- Windhawk (يفضل الإصدار الأحدث).
- ويندوز 7 --> يعمل بكفاءة
- ويندوز 8 --> يعمل بكفاءة
- ويندوز 8.1 --> يعمل بكفاءة
- ويندوز 10\11 --> لم أجربه (إذا كان يعمل بكفاءة معك فأخبرني في Issues على Github)

**المساهمة:**
نرحب بالمساهمين! يمكنكم فتح Issues أو Pull Requests على GitHub.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <vector>
#include <mmsystem.h>

#define WM_USER_QUIT   (WM_USER + 101)
#define HOTKEY_ID 1
#define HOTKEY_MODIFIERS (MOD_CONTROL | MOD_ALT)   // Ctrl+Alt
#define HOTKEY_KEY 'T'

// ===== قائمة النوافذ المثبتة =====
// ===== List of pinned windows =====
std::vector<HWND> g_trackedWindows;
CRITICAL_SECTION g_cs;
HWND g_hwndMod = nullptr;

// ===== تشغيل صوت من النظام =====
// ===== Play a sound from the system =====
void PlaySystemSound(bool isPinned) {
    // 1. حاول تشغيل ملف Speech On/Off مباشرة
    // 1. Try running the Speech On/Off file directly
    LPCWSTR soundPath = isPinned ? 
        L"C:\\Windows\\Media\\Speech On.wav" : 
        L"C:\\Windows\\Media\\Speech Off.wav";
    
    if (GetFileAttributes(soundPath) != INVALID_FILE_ATTRIBUTES) {
        if (PlaySound(soundPath, NULL, SND_FILENAME | SND_ASYNC)) {
            Wh_Log(L"🔊 Played: %s", isPinned ? L"Speech On" : L"Speech Off");
            return;
        }
    }
    
    // 2. البديل: أصوات النظام المضمونة
    // 2. Alternative: Guaranteed system votes
    LPCWSTR soundAlias = isPinned ? L"SystemAsterisk" : L"SystemExclamation";
    if (PlaySound(soundAlias, NULL, SND_ALIAS | SND_ASYNC)) {
        Wh_Log(L"🔊 Played system sound: %s", soundAlias);
        return;
    }
    
    // 3. الحل الأخير: MessageBeep (يعمل دائماً)
    // 3. Last solution: MessageBeep (always works)
    Wh_Log(L"🔊 Using MessageBeep as fallback");
    MessageBeep(isPinned ? MB_ICONASTERISK : MB_ICONEXCLAMATION);
}

// ===== تبديل التثبيت =====
// ===== Switch Pin =====
void ToggleTopMost() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd) return;
    
    wchar_t className[64];
    GetClassName(hWnd, className, 64);
    if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) return;

    bool isPinned = false;
    EnterCriticalSection(&g_cs);
    for (HWND w : g_trackedWindows) {
        if (w == hWnd) { isPinned = true; break; }
    }
    LeaveCriticalSection(&g_cs);

    if (isPinned) {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        EnterCriticalSection(&g_cs);
        for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end(); ++it) {
            if (*it == hWnd) { g_trackedWindows.erase(it); break; }
        }
        LeaveCriticalSection(&g_cs);
        PlaySystemSound(false);
        Wh_Log(L"🔽 Unpinned window %p", hWnd);
    } else {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        EnterCriticalSection(&g_cs);
        g_trackedWindows.push_back(hWnd);
        LeaveCriticalSection(&g_cs);
        PlaySystemSound(true);
        Wh_Log(L"🔼 Pinned window %p", hWnd);
    }
}

// ===== تنظيف النوافذ المغلقة =====
// ===== Cleaning closed windows =====
void CleanupDeadWindows() {
    EnterCriticalSection(&g_cs);
    for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end(); ) {
        if (!IsWindow(*it)) it = g_trackedWindows.erase(it);
        else ++it;
    }
    LeaveCriticalSection(&g_cs);
}

// ===== معالج الرسائل =====
// ===== Message processor =====
LRESULT CALLBACK ModWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_HOTKEY:
        if (wParam == HOTKEY_ID) {
            Wh_Log(L"⌨️ Hotkey pressed");
            ToggleTopMost();
            CleanupDeadWindows();
        }
        return 0;
    case WM_USER_QUIT:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ===== الخيط الرئيسي =====
// ===== The main thread =====
DWORD WINAPI MainThread(LPVOID lpParam) {
    const wchar_t* CLASS_NAME = L"TopMostModClass";
    
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = ModWndProc;
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"❌ Failed to register window class");
        return 0;
    }

    g_hwndMod = CreateWindowEx(0, CLASS_NAME, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
    if (!g_hwndMod) {
        Wh_Log(L"❌ Failed to create message window");
        return 0;
    }

    // تسجيل الاختصار
    // Shortcut registration
    if (!RegisterHotKey(g_hwndMod, HOTKEY_ID, HOTKEY_MODIFIERS, HOTKEY_KEY)) {
        Wh_Log(L"⚠️ Failed to register hotkey (may be already used)");
    }

    Wh_Log(L"✅ Mod loaded in explorer.exe, waiting for hotkey...");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hwndMod) {
        UnregisterHotKey(g_hwndMod, HOTKEY_ID);
        DestroyWindow(g_hwndMod);
        g_hwndMod = nullptr;
    }
    UnregisterClass(CLASS_NAME, nullptr);

    Wh_Log(L"🛑 Main thread exiting");
    return 0;
}

// ===== دوال المود =====
// ===== Mode functions =====
BOOL Wh_ModInit() {
    Wh_Log(L"🔧 Initializing mod in explorer.exe...");
    InitializeCriticalSection(&g_cs);
    
    HANDLE hThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    if (!hThread) {
        Wh_Log(L"❌ Failed to create main thread");
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    CloseHandle(hThread);
    
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"🛑 Unloading mod...");
    if (g_hwndMod) PostMessage(g_hwndMod, WM_USER_QUIT, 0, 0);
    Sleep(200);
    EnterCriticalSection(&g_cs);
    g_trackedWindows.clear();
    LeaveCriticalSection(&g_cs);
    DeleteCriticalSection(&g_cs);
    Wh_Log(L"✅ Mod unloaded");
}
