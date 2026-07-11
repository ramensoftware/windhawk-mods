// ==WindhawkMod==
// @id         always-on-top
// @name       Always On Top Windows
// @description Pin/unpin active window with Ctrl+Shift+T (with speech notifications)
// @version    1.0
// @author     AhmedAwad7
// @github     https://github.com/AhmedAwad7
// @license    MIT
// @include    *
// @compilerOptions -luser32 -lsapi -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Always On Top Windows
Windhawk Mod
==========================

![Screenrecord](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/gif.gif)

لتفعيل تثبيت\إلغاء التثبيت فوق النوافذ الأخري أستخدم أختصار `Ctrl+Shift+T`

من مميزات هذه الإضافة أنها ستساعدك في تعدد المهام

لأن هدف هذا المود هو تثبيت النافذة فوق التطبيقات الأخري

نرحب بالمساهمين سواء كان لديك اقتراح Issues أو التعديل على الكود Pull requests على Github

يعتمد هذا المود على تقنية Ease of Access Center لنطق إذا كان التثبيت يعمل أم لا 

إذا كان هناك مشكلة في هذه التقنية داخل الويندوز فأحتمال أنه لا يصدر صوت كلام عند الضغط على الأختصار

**متطلبات المود**

Windhawk ولكن لا يكون قديم جداً
- ويندوز 7 --> يعمل بكفائة
- ويندوز 8 --> يعمل بكفائة
- ويندوز 8.1 --> يعمل بكفائة
- ويندوز 10\11 --> لم أجربه (إذا كان يعمل بكفاءة معك فأخبرني في Issues على Github)
---
Always On Top Windows
Windhawk Mod
==========================

To enable/disable staying on top of other windows, use the shortcut `Ctrl+Shift+T`

One of the features of this addition is that it will help you multitask

Because the goal of this mod is to pin the window on top of other applications

We welcome contributors, whether you have suggestions (Issues) or code modifications (Pull requests) on Github

This mod relies on the Ease of Access Center technology to announce whether the pin is active or not

If there is an issue with this technology in Windows, it's possible that no speech will be issued when pressing the shortcut

**Mod Requirements**

Windhawk, but not too old
- Windows 7 --> Works efficiently
- Windows 8 --> Works efficiently
- Windows 8.1 --> Works efficiently
- Windows 10/11 --> I haven't tried it (if it works well for you, please let me know in Issues on Github)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <vector>
#include <sapi.h>      // لـ ISpVoice
#include <sphelper.h>  // لـ SPF_DEFAULT (اختياري)
#include <comdef.h>    // للتعامل مع COM

#define KEY_1 VK_CONTROL
#define KEY_2 VK_SHIFT
#define KEY_3 'T'

// ===== المتغيرات العامة =====
// ===== Global variables =====
std::vector<HWND> g_trackedWindows;
CRITICAL_SECTION g_cs;
volatile BOOL g_running = TRUE;
BOOL g_isExplorer = FALSE;
ISpVoice* g_pVoice = NULL;  // مؤشر لـ SAPI

// ===== دالة النطق =====
// ===== Speech function =====
void Speak(PCWSTR text) {
    if (!g_pVoice) return;  // إذا لم يتم تهيئة SAPI، نتجاوز // If SAPI isn't initialized, we skip it 
    g_pVoice->Speak(text, SPF_DEFAULT, NULL);
}

// ===== دالة تبديل التثبيت =====
// ===== Switch Pin function =====
void ToggleTopMost() {
    if (!g_isExplorer) return;

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
        // إلغاء التثبيت // Unpin
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        EnterCriticalSection(&g_cs);
        for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end(); ++it) {
            if (*it == hWnd) { g_trackedWindows.erase(it); break; }
        }
        LeaveCriticalSection(&g_cs);
        Speak(L"Desable Pin");  // 🔊 إشعار صوتي // Notification Speech
        Wh_Log(L"⬇️ إلغاء تثبيت النافذة %p + تشغيل صوت Speech Off", hWnd);
    } else {
        // تثبيت النافذة // Pin
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        EnterCriticalSection(&g_cs);
        g_trackedWindows.push_back(hWnd);
        LeaveCriticalSection(&g_cs);
        Speak(L"Enable Pin");  // 🔊 إشعار صوتي // Notification Speech
        Wh_Log(L"⬆️ تثبيت النافذة %p + تشغيل صوت Speech On", hWnd);
    }
}

// ===== خيط المراقبة =====
// ===== Monitoring thread =====
DWORD WINAPI MonitorThread(LPVOID lpParam) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    bool wasPressed = false;

    while (g_running) {
        bool k1 = (GetAsyncKeyState(KEY_1) & 0x8000) != 0;
        bool k2 = (GetAsyncKeyState(KEY_2) & 0x8000) != 0;
        bool k3 = (GetAsyncKeyState(KEY_3) & 0x8000) != 0;
        bool pressed = k1 && k2 && k3;

        if (pressed && !wasPressed) {
            ToggleTopMost();
        }
        wasPressed = pressed;

        Sleep(50);
    }
    return 0;
}

// ===== دالة مساعدة لاستخراج اسم العملية =====
// ===== Helper function to get the process name =====
PCWSTR GetProcessName() {
    static WCHAR processName[MAX_PATH] = {0};
    if (processName[0] != 0) return processName;
    WCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    PCWSTR p = wcsrchr(path, L'\\');
    if (p) p++;
    else p = path;
    wcscpy_s(processName, MAX_PATH, p);
    return processName;
}

// ===== دالة التهيئة =====
// ===== Initialization function =====
BOOL Wh_ModInit() {
    Wh_Log(L"🔧 بدء تحميل المود v1.0 (مع الإشعارات الصوتية)");

    InitializeCriticalSection(&g_cs);

    // تهيئة COM (ضروري لـ SAPI)
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        Wh_Log(L"❌ فشل تهيئة COM");
    } else {
        // إنشاء كائن SAPI
        hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&g_pVoice);
        if (FAILED(hr)) {
            Wh_Log(L"❌ فشل إنشاء كائن SAPI (رمز: %lx)", hr);
        } else {
            Wh_Log(L"✅ تم تهيئة SAPI بنجاح");
        }
    }

    PCWSTR procName = GetProcessName();
    if (_wcsicmp(procName, L"explorer.exe") == 0) {
        g_isExplorer = TRUE;
        Wh_Log(L"✅ هذه العملية هي explorer.exe (ستنفذ التبديل)");
    } else {
        Wh_Log(L"ℹ️ هذه العملية: %s (لن تنفذ التبديل)", procName);
    }

    HANDLE hThread = CreateThread(NULL, 0, MonitorThread, NULL, 0, NULL);
    if (!hThread) {
        Wh_Log(L"❌ فشل إنشاء خيط المراقبة");
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    CloseHandle(hThread);

    Wh_Log(L"✅ تم تحميل المود بنجاح");
    return TRUE;
}

// ===== دالة إنهاء المود =====
// ===== Mod Exit Function =====
void Wh_ModUninit() {
    g_running = FALSE;
    Sleep(100);

    EnterCriticalSection(&g_cs);
    g_trackedWindows.clear();
    LeaveCriticalSection(&g_cs);

    // تنظيف SAPI // SAPI Cleaning
    if (g_pVoice) {
        g_pVoice->Release();
        g_pVoice = NULL;
    }
    CoUninitialize();

    DeleteCriticalSection(&g_cs);
    Wh_Log(L"🛑 تم إلغاء تحميل المود");
}
