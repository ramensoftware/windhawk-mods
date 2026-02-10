// ==WindhawkMod==
// @id              caffeine
// @name            Caffeine
// @description     Prevent your PC from sleeping or turning off the display with a simple tray icon toggle
// @version         1.0
// @author          ALMAS CP
// @github          https://github.com/almas-cp
// @homepage        https://github.com/almas-cp
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Caffeine
A lightweight system tray utility that prevents your computer from sleeping
or turning off the display. Toggle it on and off with a single click on the
tray icon.


## Features
- **One-click toggle**: Left-click the tray icon to activate/deactivate
- **Prevent sleep**: Keeps your system from entering sleep mode
- **Prevent display off**: Stops the monitor from turning off due to inactivity
- **Visual indicators**: Distinct colors for active (caffeinated) and inactive states
- **Customizable appearance**: Choose colors, shapes, and icon size
- **Auto-start option**: Optionally start in active state when the mod loads
- **Lightweight**: Uses the proper Windows API (`SetThreadExecutionState`) — no fake keypresses


## How it works
When activated, the mod calls `SetThreadExecutionState` with
`ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED` to tell Windows
that the system and display should stay on. When deactivated, it clears
these flags.


## Visual Indicators
- **Active (Caffeinated)**: Bright icon (default: amber/orange ☕)
- **Inactive**: Dim icon (default: gray)


## Usage
- **Left-click** the tray icon to toggle caffeine on/off
- Configure colors, shape, size, and auto-start behavior in mod settings
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
# Caffeine Settings

- autoActivate: false
  $name: Auto-Activate on Start
  $description: Automatically enable caffeine when the mod starts

- preventSleep: true
  $name: Prevent Sleep
  $description: Keep the system from entering sleep mode

- preventDisplayOff: true
  $name: Prevent Display Off
  $description: Keep the display from turning off due to inactivity

- iconSize: 16
  $name: Tray Icon Size
  $description: Size of the tray icon in pixels (16-32 recommended)

- activeColorR: 255
  $name: Active Color - Red (0-255)
  $description: Red component of the active (caffeinated) state color

- activeColorG: 180
  $name: Active Color - Green (0-255)
  $description: Green component of the active (caffeinated) state color

- activeColorB: 0
  $name: Active Color - Blue (0-255)
  $description: Blue component of the active (caffeinated) state color

- inactiveColorR: 128
  $name: Inactive Color - Red (0-255)
  $description: Red component of the inactive state color

- inactiveColorG: 128
  $name: Inactive Color - Green (0-255)
  $description: Green component of the inactive state color

- inactiveColorB: 128
  $name: Inactive Color - Blue (0-255)
  $description: Blue component of the inactive state color

- iconShape: 0
  $name: Icon Shape
  $description: 0=Circle, 1=Square

- logVerbose: true
  $name: Verbose Logging
  $description: Enable detailed status logging
*/
// ==/WindhawkModSettings==


#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <algorithm>
#include <functional>
#include <atomic>
#include <memory>
#include <cstring>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1002


// Icon shape constants
#define SHAPE_CIRCLE 0
#define SHAPE_SQUARE 1


struct CaffeineSettings {
    bool autoActivate;
    bool preventSleep;
    bool preventDisplayOff;
    int iconSize;
    int activeColorR;
    int activeColorG;
    int activeColorB;
    int inactiveColorR;
    int inactiveColorG;
    int inactiveColorB;
    int iconShape;
    bool logVerbose;
};


// ─── Tray Icon Management ────────────────────────────────────────────────────

class CaffeineTrayIcon {
private:
    NOTIFYICONDATA nid;
    HWND hwnd;
    HICON hIconActive;
    HICON hIconInactive;
    bool iconVisible;
    bool isInitialized;
    bool currentActiveState;  // Track state so UpdateSettings can refresh nid.hIcon

    int ClampColor(int value) {
        return std::max(0, std::min(255, value));
    }

    HICON CreateCustomBitmapIcon(COLORREF color, int size, int shape) {
        BITMAPINFO bi = {{0}};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = size;
        bi.bmiHeader.biHeight = -size; // Top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        DWORD* pixels;
        HDC hdc = GetDC(NULL);
        HBITMAP hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
        ReleaseDC(NULL, hdc);

        if (!hbm) return NULL;

        // Convert RGB to BGR for bitmap and add alpha
        DWORD bitmapColor = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color & 0xFF0000) >> 16) | 0xFF000000;
        DWORD transparent = 0x00000000;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int index = y * size + x;
                bool shouldFill = false;

                if (shape == SHAPE_CIRCLE) {
                    int centerX = size / 2;
                    int centerY = size / 2;
                    int radius = size / 2 - 1;
                    int dx = x - centerX;
                    int dy = y - centerY;
                    shouldFill = (dx * dx + dy * dy <= radius * radius);
                } else {
                    int border = 1;
                    shouldFill = (x >= border && x < size - border &&
                                  y >= border && y < size - border);
                }

                pixels[index] = shouldFill ? bitmapColor : transparent;
            }
        }

        // Create mask bitmap
        HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);
        if (!hbmMask) {
            DeleteObject(hbm);
            return NULL;
        }

        HDC hdcMask = CreateCompatibleDC(NULL);
        SelectObject(hdcMask, hbmMask);

        RECT rect = {0, 0, size, size};
        FillRect(hdcMask, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

        HBRUSH hbrBlack = CreateSolidBrush(RGB(0, 0, 0));
        SelectObject(hdcMask, hbrBlack);
        SelectObject(hdcMask, GetStockObject(NULL_PEN));

        if (shape == SHAPE_CIRCLE) {
            int margin = 1;
            Ellipse(hdcMask, margin, margin, size - margin, size - margin);
        } else {
            int border = 1;
            Rectangle(hdcMask, border, border, size - border, size - border);
        }

        DeleteObject(hbrBlack);
        DeleteDC(hdcMask);

        ICONINFO iconInfo = {0};
        iconInfo.fIcon = TRUE;
        iconInfo.hbmColor = hbm;
        iconInfo.hbmMask = hbmMask;

        HICON hIcon = CreateIconIndirect(&iconInfo);

        DeleteObject(hbm);
        DeleteObject(hbmMask);

        return hIcon;
    }

    void CreateIcons(const CaffeineSettings& settings) {
        if (hIconActive) DestroyIcon(hIconActive);
        if (hIconInactive) DestroyIcon(hIconInactive);
        hIconActive = NULL;
        hIconInactive = NULL;

        COLORREF activeColor = RGB(
            ClampColor(settings.activeColorR),
            ClampColor(settings.activeColorG),
            ClampColor(settings.activeColorB)
        );

        COLORREF inactiveColor = RGB(
            ClampColor(settings.inactiveColorR),
            ClampColor(settings.inactiveColorG),
            ClampColor(settings.inactiveColorB)
        );

        hIconActive = CreateCustomBitmapIcon(activeColor, settings.iconSize, settings.iconShape);
        hIconInactive = CreateCustomBitmapIcon(inactiveColor, settings.iconSize, settings.iconShape);
    }

public:
    CaffeineTrayIcon() : hwnd(NULL), hIconActive(NULL), hIconInactive(NULL),
                         iconVisible(false), isInitialized(false),
                         currentActiveState(false) {
        memset(&nid, 0, sizeof(nid));
    }

    ~CaffeineTrayIcon() {
        Hide();
        if (hIconActive) DestroyIcon(hIconActive);
        if (hIconInactive) DestroyIcon(hIconInactive);
    }

    bool Initialize(HWND window, const CaffeineSettings& settings) {
        if (isInitialized) return true;

        hwnd = window;
        CreateIcons(settings);

        if (!hIconActive || !hIconInactive) return false;

        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = ID_TRAYICON;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = hIconInactive;
        wcscpy_s(nid.szTip, L"Caffeine: Inactive");

        isInitialized = true;
        return true;
    }

    bool Show() {
        if (!hwnd || !isInitialized) return false;
        if (iconVisible) return true;

        Wh_Log(L"🔄 Attempting to show caffeine tray icon...");

        const int maxRetries = 15;
        const int retryDelayMs = 1000;

        for (int attempt = 1; attempt <= maxRetries; attempt++) {
            bool result = Shell_NotifyIcon(NIM_ADD, &nid) != FALSE;
            if (result) {
                iconVisible = true;
                Wh_Log(L"✅ Caffeine tray icon shown (attempt %d)", attempt);
                return true;
            }

            if (attempt < maxRetries) {
                Wh_Log(L"⏳ Tray icon add failed (attempt %d/%d), retrying...",
                       attempt, maxRetries);

                DWORD startTick = GetTickCount();
                while (GetTickCount() - startTick < (DWORD)retryDelayMs) {
                    MSG msg;
                    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);

                        if (iconVisible) {
                            Wh_Log(L"✅ Tray icon created via TaskbarCreated");
                            return true;
                        }
                    }
                    Sleep(50);
                }
            }
        }

        Wh_Log(L"⚠️ Failed to show tray icon after %d attempts", maxRetries);
        return false;
    }

    bool Hide() {
        if (!iconVisible) return true;

        bool result = Shell_NotifyIcon(NIM_DELETE, &nid) != FALSE;
        if (result) iconVisible = false;
        return result;
    }

    void UpdateStatus(bool active) {
        if (!iconVisible || !isInitialized) return;

        currentActiveState = active;
        nid.hIcon = active ? hIconActive : hIconInactive;
        const wchar_t* status = active ? L"Active (Keeping awake)" : L"Inactive";
        swprintf_s(nid.szTip, L"Caffeine: %s", status);

        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }

    void UpdateSettings(const CaffeineSettings& settings) {
        if (!isInitialized) return;

        CreateIcons(settings);

        // Refresh nid.hIcon to avoid dangling pointer after icon recreation
        nid.hIcon = currentActiveState ? hIconActive : hIconInactive;
    }

    bool IsVisible() const { return iconVisible; }

    bool RecreateTrayIcon() {
        if (!isInitialized || !hwnd) return false;

        Wh_Log(L"🔄 Taskbar restarted, recreating caffeine tray icon...");
        iconVisible = false;

        bool result = Shell_NotifyIcon(NIM_ADD, &nid) != FALSE;
        if (result) {
            iconVisible = true;
            Wh_Log(L"✅ Caffeine tray icon recreated");
        } else {
            Wh_Log(L"❌ Failed to recreate caffeine tray icon");
        }
        return result;
    }

    void Cleanup() {
        Hide();
        isInitialized = false;
    }
};


// ─── Hidden Window for Tray Messages ─────────────────────────────────────────

class CaffeineTrayWindow {
private:
    HWND hwnd;
    static const wchar_t* CLASS_NAME;
    static UINT s_uTaskbarRestart;
    static std::function<void()> s_onTaskbarCreated;
    static std::function<void()> s_onTrayClick;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_TRAYICON:
                switch (LOWORD(lParam)) {
                    case WM_LBUTTONUP:
                        if (s_onTrayClick) {
                            s_onTrayClick();
                        }
                        return 0;
                }
                return 0;
            default:
                if (s_uTaskbarRestart != 0 && uMsg == s_uTaskbarRestart) {
                    Wh_Log(L"📢 TaskbarCreated message received");
                    if (s_onTaskbarCreated) {
                        s_onTaskbarCreated();
                    }
                    return 0;
                }
                break;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    CaffeineTrayWindow() : hwnd(NULL) {}

    ~CaffeineTrayWindow() {
        if (hwnd) {
            DestroyWindow(hwnd);
        }
    }

    bool Create() {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Caffeine",
            WS_OVERLAPPED,
            0, 0, 0, 0,
            NULL,  // Not HWND_MESSAGE — we need to receive broadcasts
            NULL,
            hInstance,
            NULL
        );

        if (hwnd) {
            s_uTaskbarRestart = RegisterWindowMessage(L"TaskbarCreated");
            if (s_uTaskbarRestart == 0) {
                Wh_Log(L"⚠️ Failed to register TaskbarCreated message");
            } else {
                Wh_Log(L"✅ Registered TaskbarCreated message (ID: %u)", s_uTaskbarRestart);
            }
        }

        return hwnd != NULL;
    }

    HWND GetHandle() const { return hwnd; }

    static void SetTaskbarCreatedCallback(std::function<void()> callback) {
        s_onTaskbarCreated = callback;
    }

    static void SetTrayClickCallback(std::function<void()> callback) {
        s_onTrayClick = callback;
    }
};

const wchar_t* CaffeineTrayWindow::CLASS_NAME = L"CaffeineTrayWindow";
UINT CaffeineTrayWindow::s_uTaskbarRestart = 0;
std::function<void()> CaffeineTrayWindow::s_onTaskbarCreated = nullptr;
std::function<void()> CaffeineTrayWindow::s_onTrayClick = nullptr;


// ─── Core Caffeine Controller ────────────────────────────────────────────────

class CaffeineController {
private:
    CaffeineSettings settings;
    std::unique_ptr<CaffeineTrayWindow> trayWindow;
    std::unique_ptr<CaffeineTrayIcon> trayIcon;
    std::atomic<bool> isActive{false};
    bool trayIconInitialized;

    void ApplyExecutionState() {
        EXECUTION_STATE flags = ES_CONTINUOUS;

        if (isActive.load()) {
            if (settings.preventSleep) {
                flags |= ES_SYSTEM_REQUIRED;
            }
            if (settings.preventDisplayOff) {
                flags |= ES_DISPLAY_REQUIRED;
            }
        }
        // If inactive, ES_CONTINUOUS alone clears previous flags

        SetThreadExecutionState(flags);

        if (settings.logVerbose) {
            if (isActive.load()) {
                Wh_Log(L"☕ Caffeine active — Sleep:%s Display:%s",
                       settings.preventSleep ? L"blocked" : L"allowed",
                       settings.preventDisplayOff ? L"blocked" : L"allowed");
            } else {
                Wh_Log(L"💤 Caffeine inactive — system power policy restored");
            }
        }
    }

public:
    CaffeineController() : trayIconInitialized(false) {
        trayWindow = std::make_unique<CaffeineTrayWindow>();
        trayIcon = std::make_unique<CaffeineTrayIcon>();
    }

    ~CaffeineController() {
        // Note: Deactivate() is called explicitly in WhTool_ModUninit before
        // g_controller.reset() to ensure power policy is restored cleanly.
        // We only clean up tray resources here.
        if (trayIcon) {
            trayIcon->Cleanup();
        }
    }

    bool InitializeTrayIcon() {
        if (trayIconInitialized) return true;

        if (!trayWindow->Create()) {
            Wh_Log(L"❌ Failed to create caffeine tray window");
            return false;
        }

        // Set up TaskbarCreated callback
        CaffeineTrayWindow::SetTaskbarCreatedCallback([this]() {
            OnTaskbarCreated();
        });

        // Set up tray click callback for toggling
        CaffeineTrayWindow::SetTrayClickCallback([this]() {
            Toggle();
        });

        if (!trayIcon->Initialize(trayWindow->GetHandle(), settings)) {
            Wh_Log(L"❌ Failed to initialize caffeine tray icon");
            return false;
        }

        if (!trayIcon->Show()) {
            Wh_Log(L"❌ Failed to show caffeine tray icon");
            return false;
        }

        trayIconInitialized = true;
        return true;
    }

    void OnTaskbarCreated() {
        if (!trayIconInitialized || !trayIcon) return;

        if (trayIcon->RecreateTrayIcon()) {
            trayIcon->UpdateStatus(isActive.load());
        }
    }

    void Activate() {
        if (isActive.load()) return;

        isActive.store(true);
        ApplyExecutionState();

        if (trayIcon && trayIcon->IsVisible()) {
            trayIcon->UpdateStatus(true);
        }

        Wh_Log(L"☕ Caffeine activated");
    }

    void Deactivate() {
        if (!isActive.load()) return;

        isActive.store(false);
        ApplyExecutionState();

        if (trayIcon && trayIcon->IsVisible()) {
            trayIcon->UpdateStatus(false);
        }

        Wh_Log(L"💤 Caffeine deactivated");
    }

    void Toggle() {
        if (isActive.load()) {
            Deactivate();
        } else {
            Activate();
        }
    }

    void UpdateSettings(const CaffeineSettings& newSettings) {
        bool wasActive = isActive.load();
        settings = newSettings;

        if (settings.iconShape < 0 || settings.iconShape > 1) {
            settings.iconShape = 0;
        }

        // Update the execution state if currently active (settings may have changed)
        if (wasActive) {
            ApplyExecutionState();
        }

        // Update tray icon appearance
        if (trayIconInitialized && trayIcon) {
            trayIcon->UpdateSettings(settings);
            trayIcon->UpdateStatus(wasActive);
        }

        Wh_Log(L"⚙️ Caffeine settings updated — AutoActivate:%s Sleep:%s Display:%s",
               settings.autoActivate ? L"yes" : L"no",
               settings.preventSleep ? L"blocked" : L"allowed",
               settings.preventDisplayOff ? L"blocked" : L"allowed");
    }

    bool IsActive() const { return isActive.load(); }

    const CaffeineSettings& GetSettings() const { return settings; }
};


// ─── Globals ─────────────────────────────────────────────────────────────────

static std::unique_ptr<CaffeineController> g_controller;


void LoadSettings() {
    CaffeineSettings settings;

    settings.autoActivate = Wh_GetIntSetting(L"autoActivate") != 0;
    settings.preventSleep = Wh_GetIntSetting(L"preventSleep") != 0;
    settings.preventDisplayOff = Wh_GetIntSetting(L"preventDisplayOff") != 0;
    settings.iconSize = Wh_GetIntSetting(L"iconSize");
    settings.activeColorR = Wh_GetIntSetting(L"activeColorR");
    settings.activeColorG = Wh_GetIntSetting(L"activeColorG");
    settings.activeColorB = Wh_GetIntSetting(L"activeColorB");
    settings.inactiveColorR = Wh_GetIntSetting(L"inactiveColorR");
    settings.inactiveColorG = Wh_GetIntSetting(L"inactiveColorG");
    settings.inactiveColorB = Wh_GetIntSetting(L"inactiveColorB");
    settings.iconShape = Wh_GetIntSetting(L"iconShape");
    settings.logVerbose = Wh_GetIntSetting(L"logVerbose") != 0;

    if (g_controller) {
        g_controller->UpdateSettings(settings);
    }
}


BOOL WhTool_ModInit() {
    Wh_Log(L"☕ Initializing Caffeine mod (windhawk.exe)");

    try {
        g_controller = std::make_unique<CaffeineController>();
        LoadSettings();

        if (!g_controller->InitializeTrayIcon()) {
            Wh_Log(L"⚠️ Tray icon initialization failed, continuing without it");
        }

        // Auto-activate if configured
        if (g_controller->GetSettings().autoActivate) {
            g_controller->Activate();
        }

        Wh_Log(L"✅ Caffeine mod initialized successfully");
        return TRUE;
    } catch (...) {
        Wh_Log(L"❌ Failed to initialize Caffeine mod");
        return FALSE;
    }
}


void WhTool_ModUninit() {
    Wh_Log(L"🔄 Uninitializing Caffeine mod");

    if (g_controller) {
        g_controller->Deactivate();  // Always restore power policy on exit
        g_controller.reset();
    }

    PostQuitMessage(0);

    Wh_Log(L"✅ Caffeine mod uninitialized");
}


void WhTool_ModSettingsChanged() {
    Wh_Log(L"⚙️ Caffeine settings changed");
    LoadSettings();
}


////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// [https://github.com/ramensoftware/windhawk-mods/pull/1916](https://github.com/ramensoftware/windhawk-mods/pull/1916)
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
    Wh_Log(L"Tool mod process entry point hooked. Starting message loop.");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"Tool mod message loop exited. Process will terminate.");
}


BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
                                 nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
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
