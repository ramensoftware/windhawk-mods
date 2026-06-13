// ==WindhawkMod==
// @id              caffeine
// @name            Caffeine
// @description     Prevent your PC from sleeping or turning off the display with a simple tray icon toggle
// @version         0.7
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
tray icon, or right-click to choose a timed duration.


## Features
- **One-click toggle**: Left-click the tray icon to activate/deactivate
- **Timed activation**: Right-click for duration options (15 min to 4 hours, or indefinite)
- **Countdown display**: Tooltip shows remaining time when using timed mode
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


## Controls
- **Left-click**: Quick toggle — activates indefinitely if off, deactivates if on
- **Right-click**: Context menu with duration options:
  - Indefinite (keeps awake forever)
  - 15 Minutes
  - 30 Minutes
  - 1 Hour
  - 2 Hours
  - 4 Hours
  - Activate / Deactivate (shown based on current state)


## Visual Indicators
- **Active (Caffeinated)**: Bright icon (default: amber/orange ☕)
- **Inactive**: Dim icon (default: gray)
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
#define WM_SETTINGS_CHANGED (WM_USER + 2)
#define ID_TRAYICON 1002
#define ID_TIMER_COUNTDOWN 2001

// Icon shape constants
#define SHAPE_CIRCLE 0
#define SHAPE_SQUARE 1

// Context menu item IDs
#define IDM_INDEFINITE  4001
#define IDM_15MIN       4002
#define IDM_30MIN       4003
#define IDM_1HOUR       4004
#define IDM_2HOUR       4005
#define IDM_4HOUR       4006
#define IDM_DEACTIVATE  4007


// Activation mode
enum CaffeineMode {
    MODE_OFF = 0,
    MODE_INDEFINITE,
    MODE_TIMED
};


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
    bool currentActiveState;

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

    // Update icon and tooltip — pass empty tooltipExtra for default, or remaining time string
    void UpdateStatus(bool active, const wchar_t* tooltipExtra = nullptr) {
        if (!iconVisible || !isInitialized) return;

        currentActiveState = active;
        nid.hIcon = active ? hIconActive : hIconInactive;

        if (!active) {
            wcscpy_s(nid.szTip, L"Caffeine: Inactive");
        } else if (tooltipExtra && tooltipExtra[0] != L'\0') {
            swprintf_s(nid.szTip, L"Caffeine: Active (%s)", tooltipExtra);
        } else {
            wcscpy_s(nid.szTip, L"Caffeine: Active (Indefinite)");
        }

        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }

    void UpdateSettings(const CaffeineSettings& settings) {
        if (!isInitialized) return;

        CreateIcons(settings);

        // Refresh nid.hIcon to avoid dangling pointer after icon recreation
        nid.hIcon = currentActiveState ? hIconActive : hIconInactive;
    }

    bool IsVisible() const { return iconVisible; }

    // Show a balloon/toast notification
    void ShowBalloonNotification(const wchar_t* title, const wchar_t* message) {
        if (!iconVisible || !isInitialized) return;

        nid.uFlags |= NIF_INFO;
        wcscpy_s(nid.szInfoTitle, title);
        wcscpy_s(nid.szInfo, message);
        nid.dwInfoFlags = NIIF_INFO;
        nid.uTimeout = 5000;  // 5 seconds

        Shell_NotifyIcon(NIM_MODIFY, &nid);

        // Clear NIF_INFO flag so subsequent updates don't re-show the balloon
        nid.uFlags &= ~NIF_INFO;
        nid.szInfoTitle[0] = L'\0';
        nid.szInfo[0] = L'\0';
    }

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
    static std::function<void()> s_onTrayRightClick;
    static std::function<void()> s_onTimerTick;
    static std::function<void(UINT)> s_onMenuCommand;
    static std::function<void()> s_onSettingsChanged;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_TRAYICON:
                switch (LOWORD(lParam)) {
                    case WM_LBUTTONUP:
                        if (s_onTrayClick) {
                            s_onTrayClick();
                        }
                        return 0;
                    case WM_RBUTTONUP:
                        if (s_onTrayRightClick) {
                            s_onTrayRightClick();
                        }
                        return 0;
                }
                return 0;

            case WM_COMMAND:
                if (s_onMenuCommand) {
                    s_onMenuCommand(LOWORD(wParam));
                }
                return 0;

            case WM_TIMER:
                if (wParam == ID_TIMER_COUNTDOWN && s_onTimerTick) {
                    s_onTimerTick();
                }
                return 0;

            case WM_SETTINGS_CHANGED:
                if (s_onSettingsChanged) {
                    s_onSettingsChanged();
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
            KillTimer(hwnd, ID_TIMER_COUNTDOWN);
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

    void StartCountdownTimer() {
        if (hwnd) {
            SetTimer(hwnd, ID_TIMER_COUNTDOWN, 1000, NULL);  // Tick every second
        }
    }

    void StopCountdownTimer() {
        if (hwnd) {
            KillTimer(hwnd, ID_TIMER_COUNTDOWN);
        }
    }

    static void SetTaskbarCreatedCallback(std::function<void()> callback) {
        s_onTaskbarCreated = callback;
    }

    static void SetTrayClickCallback(std::function<void()> callback) {
        s_onTrayClick = callback;
    }

    static void SetTrayRightClickCallback(std::function<void()> callback) {
        s_onTrayRightClick = callback;
    }

    static void SetTimerTickCallback(std::function<void()> callback) {
        s_onTimerTick = callback;
    }

    static void SetMenuCommandCallback(std::function<void(UINT)> callback) {
        s_onMenuCommand = callback;
    }

    static void SetSettingsChangedCallback(std::function<void()> callback) {
        s_onSettingsChanged = callback;
    }
};

const wchar_t* CaffeineTrayWindow::CLASS_NAME = L"CaffeineTrayWindow";
UINT CaffeineTrayWindow::s_uTaskbarRestart = 0;
std::function<void()> CaffeineTrayWindow::s_onTaskbarCreated = nullptr;
std::function<void()> CaffeineTrayWindow::s_onTrayClick = nullptr;
std::function<void()> CaffeineTrayWindow::s_onTrayRightClick = nullptr;
std::function<void()> CaffeineTrayWindow::s_onTimerTick = nullptr;
std::function<void(UINT)> CaffeineTrayWindow::s_onMenuCommand = nullptr;
std::function<void()> CaffeineTrayWindow::s_onSettingsChanged = nullptr;


// ─── Core Caffeine Controller ────────────────────────────────────────────────

class CaffeineController {
private:
    CaffeineSettings settings;
    std::unique_ptr<CaffeineTrayWindow> trayWindow;
    std::unique_ptr<CaffeineTrayIcon> trayIcon;
    bool trayIconInitialized;

    // Activation state
    CaffeineMode currentMode;
    ULONGLONG activationTickMs;   // GetTickCount64() when activated
    ULONGLONG durationMs;         // 0 for indefinite
    int timedMinutes;             // Original duration in minutes (for menu checkmark)

    void ApplyExecutionState() {
        EXECUTION_STATE flags = ES_CONTINUOUS;

        if (currentMode != MODE_OFF) {
            if (settings.preventSleep) {
                flags |= ES_SYSTEM_REQUIRED;
            }
            if (settings.preventDisplayOff) {
                flags |= ES_DISPLAY_REQUIRED;
            }
        }

        SetThreadExecutionState(flags);

        if (settings.logVerbose) {
            if (currentMode != MODE_OFF) {
                Wh_Log(L"☕ Caffeine active — Sleep:%s Display:%s",
                       settings.preventSleep ? L"blocked" : L"allowed",
                       settings.preventDisplayOff ? L"blocked" : L"allowed");
            } else {
                Wh_Log(L"💤 Caffeine inactive — system power policy restored");
            }
        }
    }

    // Format remaining time into a readable string
    void FormatRemainingTime(wchar_t* buffer, size_t bufferSize, ULONGLONG remainingMs) {
        ULONGLONG totalSeconds = remainingMs / 1000;
        ULONGLONG hours = totalSeconds / 3600;
        ULONGLONG minutes = (totalSeconds % 3600) / 60;
        ULONGLONG seconds = totalSeconds % 60;

        if (hours > 0) {
            swprintf_s(buffer, bufferSize, L"%lluh %llum left", hours, minutes);
        } else if (minutes > 0) {
            swprintf_s(buffer, bufferSize, L"%llum %llus left", minutes, seconds);
        } else {
            swprintf_s(buffer, bufferSize, L"%llus left", seconds);
        }
    }

    void UpdateTooltip() {
        if (!trayIcon || !trayIcon->IsVisible()) return;

        if (currentMode == MODE_OFF) {
            trayIcon->UpdateStatus(false);
        } else if (currentMode == MODE_INDEFINITE) {
            trayIcon->UpdateStatus(true, L"Indefinite");
        } else {
            // Timed — show remaining
            ULONGLONG now = GetTickCount64();
            ULONGLONG elapsed = now - activationTickMs;
            if (elapsed >= durationMs) {
                // Time's up — will be handled by timer tick
                trayIcon->UpdateStatus(true, L"Expiring...");
            } else {
                wchar_t timeStr[64];
                FormatRemainingTime(timeStr, 64, durationMs - elapsed);
                trayIcon->UpdateStatus(true, timeStr);
            }
        }
    }

public:
    CaffeineController() : trayIconInitialized(false),
                           currentMode(MODE_OFF),
                           activationTickMs(0),
                           durationMs(0),
                           timedMinutes(0) {
        trayWindow = std::make_unique<CaffeineTrayWindow>();
        trayIcon = std::make_unique<CaffeineTrayIcon>();
    }

    ~CaffeineController() {
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

        // Callbacks
        CaffeineTrayWindow::SetTaskbarCreatedCallback([this]() {
            OnTaskbarCreated();
        });

        CaffeineTrayWindow::SetTrayClickCallback([this]() {
            Toggle();
        });

        CaffeineTrayWindow::SetTrayRightClickCallback([this]() {
            ShowContextMenu();
        });

        CaffeineTrayWindow::SetTimerTickCallback([this]() {
            OnTimerTick();
        });

        CaffeineTrayWindow::SetMenuCommandCallback([this](UINT id) {
            OnMenuCommand(id);
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
            UpdateTooltip();
        }
    }

    // ── Activation controls ──────────────────────────────────────────────

    void ActivateIndefinite() {
        // Stop any running timer
        trayWindow->StopCountdownTimer();

        currentMode = MODE_INDEFINITE;
        activationTickMs = 0;
        durationMs = 0;
        timedMinutes = 0;

        ApplyExecutionState();
        UpdateTooltip();
        SaveState();

        Wh_Log(L"☕ Caffeine activated (Indefinite)");
    }

    void ActivateTimed(int minutes) {
        currentMode = MODE_TIMED;
        activationTickMs = GetTickCount64();
        durationMs = (ULONGLONG)minutes * 60ULL * 1000ULL;
        timedMinutes = minutes;

        ApplyExecutionState();
        UpdateTooltip();
        SaveState();

        // Start the 1-second countdown timer
        trayWindow->StartCountdownTimer();

        Wh_Log(L"☕ Caffeine activated (%d minutes)", minutes);
    }

    void Deactivate(bool showNotification = false) {
        if (currentMode == MODE_OFF) return;

        trayWindow->StopCountdownTimer();

        // Show balloon notification if this was a timer expiry
        if (showNotification && trayIcon && trayIcon->IsVisible()) {
            wchar_t msg[128];
            if (timedMinutes > 0) {
                if (timedMinutes >= 60) {
                    swprintf_s(msg, L"Your %d hour caffeine timer has ended.",
                               timedMinutes / 60);
                } else {
                    swprintf_s(msg, L"Your %d minute caffeine timer has ended.",
                               timedMinutes);
                }
            } else {
                wcscpy_s(msg, L"Caffeine has been deactivated.");
            }
            trayIcon->ShowBalloonNotification(L"☕ Caffeine Expired", msg);
        }

        currentMode = MODE_OFF;
        activationTickMs = 0;
        durationMs = 0;
        timedMinutes = 0;

        ApplyExecutionState();
        UpdateTooltip();
        ClearState();

        Wh_Log(L"💤 Caffeine deactivated");
    }

    void Toggle() {
        if (currentMode != MODE_OFF) {
            Deactivate();
        } else {
            ActivateIndefinite();
        }
    }

    // ── Timer tick (called every second while timed mode is active) ──────

    void OnTimerTick() {
        if (currentMode != MODE_TIMED) {
            trayWindow->StopCountdownTimer();
            return;
        }

        ULONGLONG now = GetTickCount64();
        ULONGLONG elapsed = now - activationTickMs;

        if (elapsed >= durationMs) {
            // Time's up!
            Wh_Log(L"⏰ Caffeine timer expired");
            Deactivate(true);  // true = show expiry notification
        } else {
            // Update tooltip with remaining time
            UpdateTooltip();
        }
    }

    // ── Context menu ─────────────────────────────────────────────────────

    void ShowContextMenu() {
        HMENU hMenu = CreatePopupMenu();
        if (!hMenu) return;

        // Build menu items with checkmarks for current mode
        UINT indefiniteFlags = MF_STRING;
        if (currentMode == MODE_INDEFINITE) indefiniteFlags |= MF_CHECKED;
        AppendMenu(hMenu, indefiniteFlags, IDM_INDEFINITE, L"Indefinite");

        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

        // Timed options
        struct { UINT id; int minutes; const wchar_t* label; } timeOptions[] = {
            { IDM_15MIN,  15,  L"15 Minutes" },
            { IDM_30MIN,  30,  L"30 Minutes" },
            { IDM_1HOUR,  60,  L"1 Hour"     },
            { IDM_2HOUR,  120, L"2 Hours"    },
            { IDM_4HOUR,  240, L"4 Hours"    },
        };

        for (auto& opt : timeOptions) {
            UINT flags = MF_STRING;
            bool isActiveOption = (currentMode == MODE_TIMED && timedMinutes == opt.minutes);
            if (isActiveOption) {
                flags |= MF_CHECKED;

                // Append remaining time to the label
                ULONGLONG elapsed = GetTickCount64() - activationTickMs;
                if (elapsed < durationMs) {
                    wchar_t timeStr[64];
                    FormatRemainingTime(timeStr, 64, durationMs - elapsed);
                    wchar_t labelWithTime[128];
                    swprintf_s(labelWithTime, L"%s  (%s)", opt.label, timeStr);
                    AppendMenu(hMenu, flags, opt.id, labelWithTime);
                } else {
                    AppendMenu(hMenu, flags, opt.id, opt.label);
                }
            } else {
                AppendMenu(hMenu, flags, opt.id, opt.label);
            }
        }

        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

        if (currentMode == MODE_OFF) {
            AppendMenu(hMenu, MF_STRING, IDM_INDEFINITE, L"Activate");
        } else {
            AppendMenu(hMenu, MF_STRING, IDM_DEACTIVATE, L"Deactivate");
        }

        // Show the menu at cursor position
        POINT pt;
        GetCursorPos(&pt);

        // Required so the menu dismisses when clicking elsewhere
        SetForegroundWindow(trayWindow->GetHandle());

        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                       pt.x, pt.y, 0, trayWindow->GetHandle(), NULL);

        // Required after TrackPopupMenu per MSDN
        PostMessage(trayWindow->GetHandle(), WM_NULL, 0, 0);

        DestroyMenu(hMenu);
    }

    void OnMenuCommand(UINT id) {
        switch (id) {
            case IDM_INDEFINITE:
                ActivateIndefinite();
                break;
            case IDM_15MIN:
                ActivateTimed(15);
                break;
            case IDM_30MIN:
                ActivateTimed(30);
                break;
            case IDM_1HOUR:
                ActivateTimed(60);
                break;
            case IDM_2HOUR:
                ActivateTimed(120);
                break;
            case IDM_4HOUR:
                ActivateTimed(240);
                break;
            case IDM_DEACTIVATE:
                Deactivate();
                break;
        }
    }

    // ── Settings ─────────────────────────────────────────────────────────

    void UpdateSettings(const CaffeineSettings& newSettings) {
        settings = newSettings;

        if (settings.iconShape < 0 || settings.iconShape > 1) {
            settings.iconShape = 0;
        }

        // Re-apply execution state if currently active (settings may have changed)
        if (currentMode != MODE_OFF) {
            ApplyExecutionState();
        }

        // Update tray icon appearance
        if (trayIconInitialized && trayIcon) {
            trayIcon->UpdateSettings(settings);
            UpdateTooltip();
        }

        Wh_Log(L"⚙️ Caffeine settings updated — AutoActivate:%s Sleep:%s Display:%s",
               settings.autoActivate ? L"yes" : L"no",
               settings.preventSleep ? L"blocked" : L"allowed",
               settings.preventDisplayOff ? L"blocked" : L"allowed");
    }

    bool IsActive() const { return currentMode != MODE_OFF; }
    CaffeineMode GetMode() const { return currentMode; }
    const CaffeineSettings& GetSettings() const { return settings; }
    HWND GetTrayWindowHandle() const { return trayWindow ? trayWindow->GetHandle() : NULL; }

    // ── State persistence ────────────────────────────────────────────────

    void SaveState() {
        // Save: mode (1=indefinite, 2=timed), timedMinutes, and wall-clock
        // activation time (seconds since epoch) for timed mode
        Wh_SetIntValue(L"savedMode", (int)currentMode);
        Wh_SetIntValue(L"savedTimedMinutes", timedMinutes);

        if (currentMode == MODE_TIMED) {
            // Save the wall-clock time when the timer should expire
            // (current time + remaining ms, stored as seconds since epoch)
            ULONGLONG remaining = 0;
            ULONGLONG elapsed = GetTickCount64() - activationTickMs;
            if (elapsed < durationMs) {
                remaining = durationMs - elapsed;
            }
            // Store expiry as a FILETIME-based timestamp (100ns intervals)
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            ULARGE_INTEGER now;
            now.LowPart = ft.dwLowDateTime;
            now.HighPart = ft.dwHighDateTime;
            // Convert remaining ms to 100ns intervals and add to now
            ULONGLONG expiryTime = now.QuadPart + (remaining * 10000ULL);
            // Store as two 32-bit ints (high and low parts)
            Wh_SetIntValue(L"savedExpiryHigh", (int)(expiryTime >> 32));
            Wh_SetIntValue(L"savedExpiryLow", (int)(expiryTime & 0xFFFFFFFF));
        }

        Wh_Log(L"💾 State saved (mode=%d, minutes=%d)", (int)currentMode, timedMinutes);
    }

    void ClearState() {
        Wh_SetIntValue(L"savedMode", 0);
        Wh_SetIntValue(L"savedTimedMinutes", 0);
        Wh_SetIntValue(L"savedExpiryHigh", 0);
        Wh_SetIntValue(L"savedExpiryLow", 0);
        Wh_Log(L"💾 Saved state cleared");
    }

    void RestoreState() {
        int savedMode = Wh_GetIntValue(L"savedMode", 0);
        if (savedMode == 0) return;  // No saved state

        int savedMinutes = Wh_GetIntValue(L"savedTimedMinutes", 0);

        if (savedMode == (int)MODE_INDEFINITE) {
            Wh_Log(L"🔄 Restoring indefinite caffeine state");
            ActivateIndefinite();
        } else if (savedMode == (int)MODE_TIMED && savedMinutes > 0) {
            // Calculate remaining time from saved expiry timestamp
            ULONGLONG expiryHigh = (ULONGLONG)(unsigned int)Wh_GetIntValue(L"savedExpiryHigh", 0);
            ULONGLONG expiryLow = (ULONGLONG)(unsigned int)Wh_GetIntValue(L"savedExpiryLow", 0);
            ULONGLONG expiryTime = (expiryHigh << 32) | expiryLow;

            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            ULARGE_INTEGER now;
            now.LowPart = ft.dwLowDateTime;
            now.HighPart = ft.dwHighDateTime;

            if (expiryTime > now.QuadPart) {
                // Still time remaining — calculate how many minutes are left
                ULONGLONG remainingMs = (expiryTime - now.QuadPart) / 10000ULL;
                int remainingMinutes = (int)(remainingMs / 60000ULL);
                if (remainingMinutes < 1) remainingMinutes = 1;  // At least 1 minute

                Wh_Log(L"🔄 Restoring timed caffeine (%d min remaining of %d min)",
                       remainingMinutes, savedMinutes);

                // Activate with the remaining time, but keep the original
                // timedMinutes for the menu checkmark
                currentMode = MODE_TIMED;
                durationMs = remainingMs;
                activationTickMs = GetTickCount64();
                timedMinutes = savedMinutes;

                ApplyExecutionState();
                UpdateTooltip();
                trayWindow->StartCountdownTimer();
            } else {
                // Timer already expired while mod was unloaded
                Wh_Log(L"⏰ Saved timer had already expired, staying inactive");
                ClearState();
            }
        }
    }
};


// ─── Globals ─────────────────────────────────────────────────────────────────

static std::unique_ptr<CaffeineController> g_controller;
static HWND g_trayWindowHwnd;


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


// Worker thread procedure — owns the message loop, tray window, and all
// thread-affine API calls (SetThreadExecutionState, etc.).
DWORD WINAPI CaffeineWorkerThread(LPVOID /*param*/) {
    Wh_Log(L"☕ Caffeine worker thread started");

    try {
        g_controller = std::make_unique<CaffeineController>();
        LoadSettings();

        // Set up the settings-changed callback so it reloads on this thread
        CaffeineTrayWindow::SetSettingsChangedCallback([]() {
            Wh_Log(L"⚙️ Caffeine settings changed (on worker thread)");
            LoadSettings();
        });

        if (!g_controller->InitializeTrayIcon()) {
            Wh_Log(L"⚠️ Tray icon initialization failed, continuing without it");
        }

        // Store the tray window HWND for cross-thread message posting
        g_trayWindowHwnd = g_controller->GetTrayWindowHandle();

        // Try to restore previous state first
        g_controller->RestoreState();

        // If no state was restored and auto-activate is on, activate
        if (!g_controller->IsActive() && g_controller->GetSettings().autoActivate) {
            g_controller->ActivateIndefinite();
        }

        Wh_Log(L"✅ Caffeine mod initialized, entering message loop");

        // Message loop — keeps the tray icon alive and processes timer/tray events
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Wh_Log(L"🔄 Caffeine worker thread message loop exited");

        if (g_controller) {
            g_controller->Deactivate();
            g_controller.reset();
        }
    } catch (...) {
        Wh_Log(L"❌ Exception in Caffeine worker thread");
    }

    return 0;
}


BOOL WhTool_ModInit() {
    Wh_Log(L"☕ Initializing Caffeine mod (windhawk.exe)");

    // Spawn a dedicated thread that owns the message loop and all
    // thread-affine state (SetThreadExecutionState, tray window, etc.).
    HANDLE hThread = CreateThread(nullptr, 0, CaffeineWorkerThread,
                                  nullptr, 0, nullptr);
    if (!hThread) {
        Wh_Log(L"❌ Failed to create worker thread");
        return FALSE;
    }

    CloseHandle(hThread);
    return TRUE;
}


void WhTool_ModUninit() {
    // The process will exit shortly after this call, so there's no much
    // need to clean up here.
}


void WhTool_ModSettingsChanged() {
    // Post a message to the tray window so the settings reload happens
    // on the worker thread (same thread that calls SetThreadExecutionState).
    if (g_trayWindowHwnd) {
        PostMessage(g_trayWindowHwnd, WM_SETTINGS_CHANGED, 0, 0);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// [https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)
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
