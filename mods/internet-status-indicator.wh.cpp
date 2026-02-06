// ==WindhawkMod==
// @id              internet-status-indicator
// @name            Internet Status Indicator
// @description     Real-time network connectivity monitoring with visual indicators as a Tray Icon
// @version         0.8
// @author          ALMAS CP
// @github          https://github.com/almas-cp
// @homepage        https://github.com/almas-cp
// @include         windhawk.exe
// @compilerOptions -lwininet -lws2_32 -liphlpapi -lgdi32 -luser32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Internet Status Indicator
A simple network monitoring mod that provides real-time connectivity status
directly integrated into your system tray. This mod continuously monitors your internet
connection by pinging target hosts and provides visual feedback.


## Features
- **Real-time monitoring**: Continuous network connectivity checks
- **Simple ping-based checking**: Pings primary host, falls back to secondary if needed
- **Customizable visual indicators**: Choose colors and shapes for connected/disconnected states
- **Customizable settings**: Configure check intervals, target hosts, and timeouts
- **Lightweight**: Minimal system resource usage
- **Simple Bitmap Icons**: Clean, reliable icon rendering


## Visual Customization
- **Custom Colors**: Set different colors for connected and disconnected states
- **Shape Options**: Choose between circular and square icon shapes
- **Size Control**: Adjust icon size to your preference


## How it works
The mod performs periodic connectivity checks by:
1. Ping primary target host (default: 8.8.8.8)
2. If primary fails, ping secondary host (default: 1.1.1.1)
3. If either ping succeeds: Green icon (connected)
4. If both pings fail: Red icon (disconnected)


## Usage
Once installed, the mod runs automatically and provides:
- Continuous background monitoring
- Log entries showing connectivity status
- Configurable check intervals and targets
- Customizable visual appearance


Check the Windhawk log to see connectivity status updates in real-time.


## Note
This mod runs as part of the windhawk.exe process for better stability and resource management.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
# Network Status Indicator Settings
# Configure how the mod monitors your internet connectivity


- checkInterval: 5000
  $name: Check Interval (ms)
  $description: How often to check connectivity (minimum 1000ms recommended)


- targetHost: "8.8.8.8"
  $name: Primary Target Host
  $description: Primary host to ping for connectivity checks (Google DNS by default)


- secondaryHost: "1.1.1.1"
  $name: Secondary Target Host  
  $description: Secondary host to ping if primary fails (Cloudflare DNS)


- timeout: 3000
  $name: Ping Timeout (ms)
  $description: Maximum wait time for ping responses


- iconSize: 16
  $name: Tray Icon Size
  $description: Size of the tray icon in pixels (16-32 recommended)


- showTrayIcon: true
  $name: Show Tray Icon
  $description: Display network status indicator in system tray


- connectedColorR: 0
  $name: Connected Color - Red (0-255)
  $description: Red component of connected state color


- connectedColorG: 255
  $name: Connected Color - Green (0-255)
  $description: Green component of connected state color


- connectedColorB: 0
  $name: Connected Color - Blue (0-255)
  $description: Blue component of connected state color


- disconnectedColorR: 255
  $name: Disconnected Color - Red (0-255)
  $description: Red component of disconnected state color


- disconnectedColorG: 0
  $name: Disconnected Color - Green (0-255)
  $description: Green component of disconnected state color


- disconnectedColorB: 0
  $name: Disconnected Color - Blue (0-255)
  $description: Blue component of disconnected state color


- iconShape: 0
  $name: Icon Shape
  $description: 0=Circle, 1=Square


- logVerbose: true
  $name: Verbose Logging
  $description: Enable detailed connectivity status logging
*/
// ==/WindhawkModSettings==


// Fix include order - winsock2.h must come before windows.h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <functional>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1001


// Icon shape constants
#define SHAPE_CIRCLE 0
#define SHAPE_SQUARE 1


struct NetworkSettings {
    int checkInterval;
    std::string targetHost;
    std::string secondaryHost;
    int timeout;
    bool logVerbose;
    int iconSize;
    bool showTrayIcon;
    // Custom color settings
    int connectedColorR;
    int connectedColorG;
    int connectedColorB;
    int disconnectedColorR;
    int disconnectedColorG;
    int disconnectedColorB;
    int iconShape; // 0=Circle, 1=Square
};


// System tray icon management
class TrayIconManager {
private:
    NOTIFYICONDATA nid;
    HWND hwnd;
    HICON hIconConnected;
    HICON hIconDisconnected;
    bool iconVisible;
    bool currentConnectionState;
    bool isInitialized;
    
    // Clamp color values to valid range
    int ClampColor(int value) {
        return std::max(0, std::min(255, value));
    }
    
    // Create bitmap icon with custom color and shape
    HICON CreateCustomBitmapIcon(COLORREF color, int size, int shape) {
        // Create bitmap info
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
        DWORD transparent = 0x00000000; // Fully transparent
        
        // Fill bitmap based on shape
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int index = y * size + x;
                bool shouldFill = false;
                
                if (shape == SHAPE_CIRCLE) {
                    // Circle shape - calculate distance from center
                    int centerX = size / 2;
                    int centerY = size / 2;
                    int radius = size / 2 - 1; // Leave 1 pixel border
                    
                    int dx = x - centerX;
                    int dy = y - centerY;
                    int distanceSquared = dx * dx + dy * dy;
                    
                    shouldFill = (distanceSquared <= radius * radius);
                } else {
                    // Square shape - leave 1 pixel border
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
        
        // Fill mask - 0 for visible parts, 1 for transparent
        HDC hdcMask = CreateCompatibleDC(NULL);
        SelectObject(hdcMask, hbmMask);
        
        // Fill entire mask with white (transparent)
        RECT rect = {0, 0, size, size};
        FillRect(hdcMask, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        
        // Draw black (opaque) parts for the shape
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
        
        // Create icon
        ICONINFO iconInfo = {0};
        iconInfo.fIcon = TRUE;
        iconInfo.hbmColor = hbm;
        iconInfo.hbmMask = hbmMask;
        
        HICON hIcon = CreateIconIndirect(&iconInfo);
        
        DeleteObject(hbm);
        DeleteObject(hbmMask);
        
        return hIcon;
    }
    
    // Create icons with current settings
    void CreateIcons(const NetworkSettings& settings) {
        // Clean up existing icons
        if (hIconConnected) DestroyIcon(hIconConnected);
        if (hIconDisconnected) DestroyIcon(hIconDisconnected);
        hIconConnected = NULL;
        hIconDisconnected = NULL;
        
        // Create colors from settings
        COLORREF connectedColor = RGB(
            ClampColor(settings.connectedColorR),
            ClampColor(settings.connectedColorG),
            ClampColor(settings.connectedColorB)
        );
        
        COLORREF disconnectedColor = RGB(
            ClampColor(settings.disconnectedColorR),
            ClampColor(settings.disconnectedColorG),
            ClampColor(settings.disconnectedColorB)
        );
        
        // Create icons
        hIconConnected = CreateCustomBitmapIcon(connectedColor, settings.iconSize, settings.iconShape);
        hIconDisconnected = CreateCustomBitmapIcon(disconnectedColor, settings.iconSize, settings.iconShape);
    }
    
public:
    TrayIconManager() : hwnd(NULL), hIconConnected(NULL), hIconDisconnected(NULL), 
                        iconVisible(false), currentConnectionState(false), isInitialized(false) {
        memset(&nid, 0, sizeof(nid));
    }
    
    ~TrayIconManager() {
        Hide();
        if (hIconConnected) DestroyIcon(hIconConnected);
        if (hIconDisconnected) DestroyIcon(hIconDisconnected);
    }
    
    bool Initialize(HWND window, const NetworkSettings& settings) {
        if (isInitialized) {
            return true;
        }
        
        hwnd = window;
        
        // Create icons with settings
        CreateIcons(settings);
        
        if (!hIconConnected || !hIconDisconnected) {
            return false;
        }
        
        // Initialize NOTIFYICONDATA
        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = ID_TRAYICON;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = hIconDisconnected;
        wcscpy_s(nid.szTip, L"Internet Status: Starting...");
        
        isInitialized = true;
        return true;
    }
    
    bool Show() {
        if (!hwnd || !isInitialized) return false;
        if (iconVisible) return true;
        
        bool result = Shell_NotifyIcon(NIM_ADD, &nid) != FALSE;
        if (result) {
            iconVisible = true;
        }
        return result;
    }
    
    bool Hide() {
        if (!iconVisible) return true;
        
        bool result = Shell_NotifyIcon(NIM_DELETE, &nid) != FALSE;
        if (result) {
            iconVisible = false;
        }
        return result;
    }
    
    void UpdateStatus(bool connected, bool forceUpdate = false) {
        if (!iconVisible || !isInitialized) return;
        if (!forceUpdate && currentConnectionState == connected) return;
        
        currentConnectionState = connected;
        nid.hIcon = connected ? hIconConnected : hIconDisconnected;
        
        const wchar_t* status = connected ? L"Connected" : L"Disconnected";
        swprintf_s(nid.szTip, L"Internet Status: %s", status);
        
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }
    
    void UpdateSettings(const NetworkSettings& settings) {
        if (!isInitialized) return;
        
        bool wasVisible = iconVisible;
        bool wasConnected = currentConnectionState;
        
        CreateIcons(settings);
        
        if (wasVisible) {
            nid.hIcon = wasConnected ? hIconConnected : hIconDisconnected;
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
    }
    
    bool IsVisible() const { return iconVisible; }
    
    // Recreate the tray icon (called when taskbar restarts)
    bool RecreateTrayIcon() {
        if (!isInitialized || !hwnd) return false;
        
        Wh_Log(L"🔄 Taskbar restarted, recreating tray icon...");
        
        // The icon was automatically removed when explorer/taskbar restarted
        // So we just need to add it again
        iconVisible = false;
        
        bool result = Shell_NotifyIcon(NIM_ADD, &nid) != FALSE;
        if (result) {
            iconVisible = true;
            Wh_Log(L"✅ Tray icon recreated successfully");
        } else {
            Wh_Log(L"❌ Failed to recreate tray icon");
        }
        return result;
    }
    
    void Cleanup() {
        Hide();
        isInitialized = false;
        currentConnectionState = false;
    }
};


// Hidden window for tray icon messages
class TrayWindow {
private:
    HWND hwnd;
    static const wchar_t* CLASS_NAME;
    static UINT s_uTaskbarRestart;  // Message ID for TaskbarCreated
    static std::function<void()> s_onTaskbarCreated;  // Callback when taskbar restarts
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_TRAYICON:
                return 0;
            default:
                // Handle TaskbarCreated message
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
    TrayWindow() : hwnd(NULL) {}
    
    ~TrayWindow() {
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
            L"Internet Status Indicator",
            0,
            0, 0, 0, 0,
            HWND_MESSAGE,
            NULL,
            hInstance,
            NULL
        );
        
        if (hwnd) {
            // Register the TaskbarCreated message
            // This message is broadcast when the taskbar is created/recreated
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
    
    // Set the callback for when taskbar is created/recreated
    static void SetTaskbarCreatedCallback(std::function<void()> callback) {
        s_onTaskbarCreated = callback;
    }
};


const wchar_t* TrayWindow::CLASS_NAME = L"InternetStatusTrayWindow";
UINT TrayWindow::s_uTaskbarRestart = 0;
std::function<void()> TrayWindow::s_onTaskbarCreated = nullptr;


class InternetStatusMonitor {
private:
    std::atomic<bool> isRunning{false};
    std::atomic<bool> isConnected{false};
    std::thread monitorThread;
    NetworkSettings settings;
    HANDLE hIcmpFile;
    
    std::unique_ptr<TrayWindow> trayWindow;
    std::unique_ptr<TrayIconManager> trayIcon;
    bool trayIconInitialized;
    
    // For interruptible waiting
    std::condition_variable cv;
    std::mutex cv_mutex;
    
public:
    InternetStatusMonitor() : hIcmpFile(INVALID_HANDLE_VALUE), 
                              trayIconInitialized(false) {
        WSAData wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        
        trayWindow = std::make_unique<TrayWindow>();
        trayIcon = std::make_unique<TrayIconManager>();
    }
    
    ~InternetStatusMonitor() {
        Stop();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            IcmpCloseHandle(hIcmpFile);
        }
        
        if (trayIcon) {
            trayIcon->Cleanup();
        }
        
        WSACleanup();
    }
    
    bool InitializeTrayIcon() {
        if (!settings.showTrayIcon || trayIconInitialized) return true;
        
        if (!trayWindow->Create()) {
            Wh_Log(L"❌ Failed to create tray window.");
            return false;
        }
        
        // Set up the TaskbarCreated callback to recreate the icon when explorer restarts
        TrayWindow::SetTaskbarCreatedCallback([this]() {
            OnTaskbarCreated();
        });
        
        if (!trayIcon->Initialize(trayWindow->GetHandle(), settings)) {
            Wh_Log(L"❌ Failed to initialize tray icon manager.");
            return false;
        }
        if (!trayIcon->Show()) {
            Wh_Log(L"❌ Failed to show tray icon.");
            return false;
        }
        
        trayIconInitialized = true;
        return true;
    }
    
    // Called when TaskbarCreated message is received (explorer restarted)
    void OnTaskbarCreated() {
        if (!trayIconInitialized || !trayIcon) return;
        
        // Recreate the tray icon
        if (trayIcon->RecreateTrayIcon()) {
            // Update with current connection status
            trayIcon->UpdateStatus(isConnected.load(), true);
        }
    }
    
    bool InitializeIcmp() {
        hIcmpFile = IcmpCreateFile();
        return hIcmpFile != INVALID_HANDLE_VALUE;
    }
    
    bool PingHost(const std::string& hostname) {
        if (hIcmpFile == INVALID_HANDLE_VALUE) {
            if (!InitializeIcmp()) return false;
        }
        
        struct addrinfo hints = {0};
        struct addrinfo* result = nullptr;
        hints.ai_family = AF_INET;
        
        if (getaddrinfo(hostname.c_str(), nullptr, &hints, &result) != 0) {
            return false;
        }
        
        struct sockaddr_in* addr = (struct sockaddr_in*)result->ai_addr;
        IPAddr destIP = addr->sin_addr.s_addr;
        freeaddrinfo(result);
        
        char sendData[] = "NetworkStatusCheck";
        DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
        LPVOID replyBuffer = malloc(replySize);
        
        if (!replyBuffer) return false;
        
        // Use shorter timeout for faster shutdown
        DWORD pingTimeout = std::min((DWORD)settings.timeout, 1000UL);
        
        DWORD numReplies = IcmpSendEcho(
            hIcmpFile, destIP, sendData, sizeof(sendData),
            nullptr, replyBuffer, replySize, pingTimeout
        );
        
        bool success = false;
        if (numReplies > 0) {
            PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)replyBuffer;
            success = (reply->Status == IP_SUCCESS);
        }
        
        free(replyBuffer);
        return success;
    }
    
    void PerformConnectivityCheck() {
        // Check if we should stop before doing any work
        if (!isRunning.load()) return;
        
        bool wasConnected = isConnected.load();
        
        // SIMPLIFIED LOGIC: Just ping primary host, then secondary if primary fails
        bool primaryPing = PingHost(settings.targetHost);
        bool secondaryPing = false;
        
        // Check again if we should stop (ping might have taken time)
        if (!isRunning.load()) return;
        
        if (!primaryPing) {
            secondaryPing = PingHost(settings.secondaryHost);
        }
        
        // Check one more time if we should stop
        if (!isRunning.load()) return;
        
        // If either ping succeeds, we're connected. That's it!
        bool currentlyConnected = primaryPing || secondaryPing;
        
        // Update connection state if changed
        if (currentlyConnected != wasConnected) {
            isConnected.store(currentlyConnected);
            
            if (currentlyConnected) {
                Wh_Log(L"🟢 Internet connection established");
            } else {
                Wh_Log(L"🔴 Internet connection lost");
            }
        }
        
        // Update tray icon
        if (settings.showTrayIcon && trayIcon && trayIcon->IsVisible()) {
            trayIcon->UpdateStatus(currentlyConnected, false);
        }
        
        // Verbose logging
        if (settings.logVerbose) {
            Wh_Log(L"Status: Primary=%s, Secondary=%s, Result=%s (%s)",
                   primaryPing ? L"✓" : L"✗",
                   secondaryPing ? L"✓" : L"✗", 
                   currentlyConnected ? L"CONNECTED" : L"DISCONNECTED",
                   currentlyConnected ? L"🟢" : L"🔴");
        }
    }
    
    void MonitorLoop() {
        Wh_Log(L"🚀 Internet Status Monitor started (windhawk.exe)");
        
        Sleep(1000);
        PerformConnectivityCheck();
        
        while (isRunning.load()) {
            auto startTime = std::chrono::steady_clock::now();
            
            try {
                PerformConnectivityCheck();
            } catch (...) {
                Wh_Log(L"⚠️ Error during connectivity check");
            }
            
            // Check if we should stop before waiting
            if (!isRunning.load()) break;
            
            auto endTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime).count();
            
            int sleepTime = settings.checkInterval - (int)elapsed;
            if (sleepTime > 0) {
                // Use condition variable for interruptible wait
                std::unique_lock<std::mutex> lock(cv_mutex);
                cv.wait_for(lock, std::chrono::milliseconds(sleepTime), 
                           [this] { return !isRunning.load(); });
            }
        }
        
        Wh_Log(L"🛑 Internet Status Monitor stopped");
    }
    
    void Start() {
        if (isRunning.load()) return;
        
        isRunning.store(true);
        monitorThread = std::thread(&InternetStatusMonitor::MonitorLoop, this);
    }
    
    void Stop() {
        if (!isRunning.load()) return;
        
        Wh_Log(L"🔄 Stopping Internet Status Monitor...");
        isRunning.store(false);
        
        // Wake up the monitoring thread immediately
        cv.notify_all();
        
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
        
        Wh_Log(L"✅ Internet Status Monitor stopped successfully");
    }
    
    void UpdateSettings(const NetworkSettings& newSettings) {
        settings = newSettings;
        
        if (settings.checkInterval < 1000) {
            settings.checkInterval = 1000;
        }
        
        if (settings.iconShape < 0 || settings.iconShape > 1) {
            settings.iconShape = 0;
        }
        
        if (settings.showTrayIcon && !trayIconInitialized) {
            InitializeTrayIcon();
            if (trayIcon && trayIcon->IsVisible()) {
                trayIcon->UpdateStatus(isConnected.load(), true);
            }
        } else if (!settings.showTrayIcon && trayIconInitialized) {
            if (trayIcon) {
                trayIcon->Cleanup();
            }
            trayIconInitialized = false;
        } else if (trayIconInitialized && trayIcon) {
            trayIcon->UpdateSettings(settings);
        }
        
        Wh_Log(L"⚙️ Settings updated - Interval: %dms, Primary: %S, Secondary: %S",
               settings.checkInterval, settings.targetHost.c_str(), settings.secondaryHost.c_str());
    }
};


static std::unique_ptr<InternetStatusMonitor> g_monitor;


void LoadSettings() {
    NetworkSettings settings;
    
    settings.checkInterval = Wh_GetIntSetting(L"checkInterval");
    
    PCWSTR targetHostW = Wh_GetStringSetting(L"targetHost");
    PCWSTR secondaryHostW = Wh_GetStringSetting(L"secondaryHost");
    
    int len = WideCharToMultiByte(CP_UTF8, 0, targetHostW, -1, nullptr, 0, nullptr, nullptr);
    settings.targetHost.resize(len - 1);
    WideCharToMultiByte(CP_UTF8, 0, targetHostW, -1, &settings.targetHost[0], len, nullptr, nullptr);
    
    len = WideCharToMultiByte(CP_UTF8, 0, secondaryHostW, -1, nullptr, 0, nullptr, nullptr);
    settings.secondaryHost.resize(len - 1);
    WideCharToMultiByte(CP_UTF8, 0, secondaryHostW, -1, &settings.secondaryHost[0], len, nullptr, nullptr);
    
    Wh_FreeStringSetting(targetHostW);
    Wh_FreeStringSetting(secondaryHostW);
    
    settings.timeout = Wh_GetIntSetting(L"timeout");
    settings.iconSize = Wh_GetIntSetting(L"iconSize");
    settings.showTrayIcon = Wh_GetIntSetting(L"showTrayIcon") != 0;
    
    settings.connectedColorR = Wh_GetIntSetting(L"connectedColorR");
    settings.connectedColorG = Wh_GetIntSetting(L"connectedColorG");
    settings.connectedColorB = Wh_GetIntSetting(L"connectedColorB");
    settings.disconnectedColorR = Wh_GetIntSetting(L"disconnectedColorR");
    settings.disconnectedColorG = Wh_GetIntSetting(L"disconnectedColorG");
    settings.disconnectedColorB = Wh_GetIntSetting(L"disconnectedColorB");
    settings.iconShape = Wh_GetIntSetting(L"iconShape");
    
    settings.logVerbose = Wh_GetIntSetting(L"logVerbose") != 0;
    
    if (g_monitor) {
        g_monitor->UpdateSettings(settings);
    }
}


BOOL WhTool_ModInit() {
    Wh_Log(L"🌐 Initializing Internet Status Indicator (windhawk.exe)");
    
    try {
        g_monitor = std::make_unique<InternetStatusMonitor>();
        LoadSettings();
        
        if (!g_monitor->InitializeTrayIcon()) {
            Wh_Log(L"⚠️ Tray icon initialization failed, continuing without it");
        }
        
        g_monitor->Start();
        
        Wh_Log(L"✅ Internet Status Indicator initialized successfully");
        return TRUE;
    } catch (...) {
        Wh_Log(L"❌ Failed to initialize Internet Status Indicator");
        return FALSE;
    }
}


void WhTool_ModUninit() {
    Wh_Log(L"🔄 Uninitializing Internet Status Indicator");
    
    if (g_monitor) {
        g_monitor->Stop();
        g_monitor.reset();
    }
    
    PostQuitMessage(0);
    
    Wh_Log(L"✅ Internet Status Indicator uninitialized");
}


void WhTool_ModSettingsChanged() {
    Wh_Log(L"⚙️ Internet Status Indicator settings changed");
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


// FIX: Implement a proper message loop.
// The previous implementation with ExitThread(0) terminated the main thread,
// which is required for handling GUI messages for the tray icon. This new
// implementation runs a message loop until WhTool_ModUninit posts a quit message.
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
    ExitProcess(0);  // ← FIXED: Added the missing ExitProcess call
}
