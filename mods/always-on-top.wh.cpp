// ==WindhawkMod==
// @id         always-on-top
// @name       Always On Top Windows
// @description Pin/unpin active window with Ctrl+Alt+T
// @version    1.2
// @author     AhmedAwad7
// @github     https://github.com/AhmedAwad7
// @license    MIT
// @include    windhawk.exe
// @compilerOptions -luser32 -lwinmm -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always On Top Windows

![Screenrecord](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/Demo.gif)

## English

Press **`Ctrl+Alt+T`** to pin/unpin the active window on top of all other windows.

This mod helps you multitask by pinning important windows so they always remain visible.

**Features:**
- 🔊 Audio notifications to indicate pinning/unpinning, with the ability to customize or disable them.
- 🖥️ A floating text displayed at the bottom center of the screen, above the taskbar, to indicate pinning/unpinning.
![Floating Text](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/Floating_text.gif)
- ⚡ Event-based shortcut (`RegisterHotKey`) with no continuous polling.
- 🧹 Lightweight: Runs in a dedicated `windhawk.exe` process.

**System Requirements:**
- 📀 Windows 7 \ 8 \ 8.1 \ 10 \ 11
- 📊 Windhawk application (latest version preferred)

**Contributing:**
Contributors are welcome! You can open Issues or Pull Requests on GitHub.
---
**How to Handle Settings Within the Mod**
---
- You can turn off sounds, turn off the floating text, or enable both together—it's up to you:
![Settings](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/settings.gif)
- You can also customize the pin/unpin sounds by placing your preferred audio files in a folder and entering the path in the text box, as shown in this example:
![Custom Sound](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/custom_sound.png)

---

## العربية

اضغط **`Ctrl+Alt+T`** لتثبيت/إلغاء تثبيت النافذة النشطة فوق جميع النوافذ الأخرى.

هذا المود يساعدك في تعدد المهام عن طريق تثبيت النوافذ المهمة لتظل مرئية دائماً.

**المميزات:**
- 🔊 إشعارات صوتية لتوضيح التثبيت\إلغاء التثبيت مع إمكانية تخصيصها أو تعطيلها.
- 🖥️ نص عائم في أسفل منتصف الشاشة فوق شريط المهام لتوضيح التثبيت\إلغاء التثبيت.
![Floating Text](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/Floating_text.gif)
- ⚡ اختصار يعتمد على الأحداث (`RegisterHotKey`) بدون استقصاء مستمر.
- 🧹 خفيف الوزن: يعمل في عملية مخصصة لـ `windhawk.exe`.

**متطلبات النظام:**
- 📀 ويندوز 7 \ 8 \ 8.1 \ 10 \ 11
- 📊 تطبيق Windhawk ويفضل أحدث إصدار

**المساهمة:**
نرحب بالمساهمين! يمكنكم فتح Issues أو Pull Requests على GitHub.
---
**طريقة التعامل مع الإعدادات داخل المود**
---
- يمكنك إيقاف الاصوات أو إيقاف النص العائم أو تشغيلهما معاً، الأمر يرجع لك:
![Settings](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/settings.gif)
- ويمكنك تخصيص صوت التثبيت\إلغاء التثبيت عن طريق وضع اصواتك المفضلة في مجلد وكتابة المسار في المستطيل مثل هذا المثال:
![Custom Sound](https://raw.githubusercontent.com/AhmedAwad7/Always-On-Top-Windows-Windhawk-Mod-/main/Screenrecord/custom_sound.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundEnabled: true
  $name: |
      Enable Sounds
      تفعيل الصوت
  $description: |
      Enable or disable sound notifications
      تفعيل أو تعطيل إشعارات الصوت
- soundOn: C:\Windows\Media\Speech On.wav
  $name: |
      Sound for Pin
      صوت للتثبيت
  $description: |
      When choosing a custom sound for pin, the audio file should be in .wav format
      عند اختيار صوت مخصص لوضع التثبيت، يجب أن يكون صيغة الملف الصوتي .wav
- soundOff: C:\Windows\Media\Speech Off.wav
  $name: |
      Sound for Unpin
      صوت لإلغاء التثبيت
  $description: |
      When choosing a custom sound for unpin, the audio file should be in .wav format
      عند اختيار صوت مخصص لوضع إلغاء التثبيت، يجب أن يكون صيغة الملف الصوتي .wav
- notificationsEnabled: true
  $name: |
      Enable Notifications
      تفعيل الإشعارات
  $description: |
      Show floating text notifications
      إظهار إشعارات نصية عائمة
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <vector>
#include <string>
#include <mmsystem.h>

#define WM_USER_QUIT   (WM_USER + 101)
#define WM_HIDE_NOTIFICATION (WM_USER + 200)
#define HOTKEY_ID 1
#define HOTKEY_MODIFIERS (MOD_CONTROL | MOD_ALT)   // Ctrl+Alt
#define HOTKEY_KEY 'T'

#define NOTIFICATION_TIMER_ID 1001
#define NOTIFICATION_DISPLAY_DURATION 5000  // 5 sec

// ===== قائمة النوافذ المثبتة =====
// ===== List of pinned windows =====
std::vector<HWND> g_trackedWindows;
CRITICAL_SECTION g_cs;
HWND g_hwndMod = nullptr;
HANDLE g_hThread = nullptr;

// ===== نافذة الإشعارات =====
// ===== Notification window =====
HWND g_notificationHwnd = nullptr;

// ===== إعدادات الصوت والإشعارات =====
// ===== Sound and notifications settings =====
struct Settings {
    bool soundEnabled = true;
    std::wstring soundOn = L"C:\Windows\Media\Speech On.wav";
    std::wstring soundOff = L"C:\Windows\Media\Speech Off.wav";
    bool notificationsEnabled = true;
} g_settings;

// ===== قراءة الإعدادات =====
// ===== Reading settings =====
void LoadSettings() {
    g_settings.soundEnabled = Wh_GetIntSetting(L"soundEnabled", 1) != 0;

    PCWSTR onStr = Wh_GetStringSetting(L"soundOn", L"C:\Windows\Media\Speech On.wav");
    if (onStr) { g_settings.soundOn = onStr; Wh_FreeStringSetting(onStr); }

    PCWSTR offStr = Wh_GetStringSetting(L"soundOff", L"C:\Windows\Media\Speech Off.wav");
    if (offStr) { g_settings.soundOff = offStr; Wh_FreeStringSetting(offStr); }

    g_settings.notificationsEnabled = Wh_GetIntSetting(L"notificationsEnabled", 1) != 0;

    Wh_Log(L"✅ Settings loaded: soundEnabled=%d, soundOn='%s', soundOff='%s', notifications=%d",
           g_settings.soundEnabled, g_settings.soundOn.c_str(), g_settings.soundOff.c_str(),
           g_settings.notificationsEnabled);
}

// ===== تشغيل صوت من النظام =====
// ===== Play a sound from the system =====
void PlaySystemSound(bool isPinned) {
    if (!g_settings.soundEnabled) {
        Wh_Log(L"🔇 Sound disabled");
        return;
    }

    const std::wstring& soundStr = isPinned ? g_settings.soundOn : g_settings.soundOff;
    if (soundStr.empty()) {
        Wh_Log(L"⚠️ Sound name is empty, skipping");
        return;
    }

    bool isFilePath = (soundStr.find(L'\\') != std::wstring::npos ||
                       soundStr.find(L':') != std::wstring::npos ||
                       soundStr.find(L'.') != std::wstring::npos);
    BOOL result = FALSE;

    if (isFilePath) {
        if (GetFileAttributes(soundStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            result = PlaySound(soundStr.c_str(), NULL, SND_FILENAME | SND_ASYNC);
            if (result) {
                Wh_Log(L"🔊 Played file: %s", soundStr.c_str());
                return;
            } else {
                Wh_Log(L"⚠️ Failed to play file: %s", soundStr.c_str());
            }
        } else {
            Wh_Log(L"⚠️ File not found: %s", soundStr.c_str());
        }
    }

    result = PlaySound(soundStr.c_str(), NULL, SND_ALIAS | SND_ASYNC);
    if (result) {
        Wh_Log(L"🔊 Played system sound alias: %s", soundStr.c_str());
        return;
    }

    Wh_Log(L"🔊 Using MessageBeep as fallback");
    MessageBeep(isPinned ? MB_ICONASTERISK : MB_ICONEXCLAMATION);
}

// ===== نافذة الإشعارات العائمة =====
// ===== Floating notification window =====
LRESULT CALLBACK NotificationWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        HBRUSH bgBrush = CreateSolidBrush(RGB(72, 72, 72));
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(225, 255, 255));
        SelectObject(hdc, bgBrush);
        SelectObject(hdc, borderPen);
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 12, 12);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        PCWSTR text = (PCWSTR)GetProp(hwnd, L"NotificationText");
        if (text) {
            
            DrawText(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        DeleteObject(bgBrush);
        DeleteObject(borderPen);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_TIMER:
        if (wParam == NOTIFICATION_TIMER_ID) {
            ShowWindow(hwnd, SW_HIDE);
            KillTimer(hwnd, NOTIFICATION_TIMER_ID);
        }
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, NOTIFICATION_TIMER_ID);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowNotification(PCWSTR text, bool isPinned) {
    if (!g_settings.notificationsEnabled) return;

    if (g_notificationHwnd && IsWindow(g_notificationHwnd)) {
        DestroyWindow(g_notificationHwnd);
        g_notificationHwnd = nullptr;
    }

    // الحصول على مساحة العمل (المنطقة التي لا يشغلها شريط المهام)
    // Getting the workspace (the area not taken up by the taskbar)
    RECT rcWork;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

    const int width = 200;
    const int height = 40;
    const int marginBottom = 20;  // المسافة من أسفل منطقة العمل//The distance from the bottom of the work area

    // حساب الموضع: منتصف الشاشة أفقياً، أسفل منطقة العمل عمودياً
    // Positioning: center of the screen horizontally, below the workspace vertically
    int x = (rcWork.left + rcWork.right - width) / 2;
    int y = rcWork.bottom - height - marginBottom;

    const wchar_t* CLASS_NAME = L"NotificationClass";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = NotificationWndProc;
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassEx(&wc);

    g_notificationHwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"",
        WS_POPUP,
        x, y,
        width, height,
        nullptr, nullptr, nullptr, nullptr
    );

    if (!g_notificationHwnd) {
        Wh_Log(L"❌ Failed to create notification window");
        return;
    }

    SetProp(g_notificationHwnd, L"NotificationText", (HANDLE)text);
    SetLayeredWindowAttributes(g_notificationHwnd, 0, 230, LWA_ALPHA);
    ShowWindow(g_notificationHwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_notificationHwnd);

    SetTimer(g_notificationHwnd, NOTIFICATION_TIMER_ID, NOTIFICATION_DISPLAY_DURATION, nullptr);
    Wh_Log(L"🔔 Notification shown: %s", text);
}

// ===== تبديل التثبيت =====
// ===== Switch installation =====
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
        ShowNotification(L"📍 Unpinned", false);
        Wh_Log(L"🔽 Unpinned window %p", hWnd);
    } else {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        EnterCriticalSection(&g_cs);
        g_trackedWindows.push_back(hWnd);
        LeaveCriticalSection(&g_cs);
        PlaySystemSound(true);
        ShowNotification(L"📌 Pinned", true);
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

    if (!RegisterHotKey(g_hwndMod, HOTKEY_ID, HOTKEY_MODIFIERS, HOTKEY_KEY)) {
        Wh_Log(L"⚠️ Failed to register hotkey (may be already used)");
    }

    Wh_Log(L"✅ Mod loaded in windhawk.exe, waiting for hotkey...");

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

// ============================================================
// ===== دوال المود كأداة (Tool Mod) =====
// ===== Mod functions as a tool (Tool Mod) =====
// ============================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"🔧 Initializing mod as a tool in windhawk.exe...");
    InitializeCriticalSection(&g_cs);
    LoadSettings();

    g_hThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    if (!g_hThread) {
        Wh_Log(L"❌ Failed to create main thread");
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"⚙️ Settings changed, reloading...");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"🛑 Unloading tool mod...");

    if (g_hwndMod) {
        PostMessage(g_hwndMod, WM_USER_QUIT, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    EnterCriticalSection(&g_cs);
    g_trackedWindows.clear();
    LeaveCriticalSection(&g_cs);

    if (g_notificationHwnd && IsWindow(g_notificationHwnd)) {
        DestroyWindow(g_notificationHwnd);
        g_notificationHwnd = nullptr;
    }

    DeleteCriticalSection(&g_cs);
    Wh_Log(L"✅ Tool mod unloaded");
}

// ============================================================
// ===== كود "Launcher" =====
// ===== "Launcher" code =====
// ============================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
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
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    BOOL processCreated = FALSE;
    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    HMODULE kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernelbase.dll");
    }

    if (kernelModule) {
        using CreateProcessInternalW_t = BOOL(WINAPI*)(
            HANDLE hUserToken,
            LPCWSTR lpApplicationName,
            LPWSTR lpCommandLine,
            LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes,
            WINBOOL bInheritHandles,
            DWORD dwCreationFlags,
            LPVOID lpEnvironment,
            LPCWSTR lpCurrentDirectory,
            LPSTARTUPINFOW lpStartupInfo,
            LPPROCESS_INFORMATION lpProcessInformation,
            PHANDLE hRestrictedUserToken
        );

        CreateProcessInternalW_t pCreateProcessInternalW =
            (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");

        if (pCreateProcessInternalW) {
            processCreated = pCreateProcessInternalW(
                nullptr, currentProcessPath, commandLine, nullptr, nullptr,
                FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr);
        }
    }

    if (!processCreated) {
        Wh_Log(L"⚠️ CreateProcessInternalW failed, falling back to CreateProcess");
        processCreated = CreateProcess(
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi
        );
    }

    if (!processCreated) {
        Wh_Log(L"❌ CreateProcess failed (error: %d)", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    Wh_Log(L"✅ Tool mod launcher process created successfully");
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
