// ==WindhawkMod==
// @id              internet-status-indicator
// @name            Internet Status Indicator
// @description     Real-time network connectivity monitoring with visual indicators as a Tray Icon
// @version         0.7
// @author          ALMAS CP
// @github          https://github.com/almas-cp
// @homepage        https://github.com/almas-cp
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lwininet -lws2_32 -liphlpapi -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Internet Status Indicator
A comprehensive network monitoring mod that provides real-time connectivity status
directly integrated into your system. This mod continuously monitors your internet
connection and provides visual feedback about network availability.

## Features
- **Real-time monitoring**: Continuous network connectivity checks
- **Multiple check methods**: Uses both ping and HTTP requests for accurate status
- **Customizable visual indicators**: Choose colors and shapes for connected/disconnected states
- **Customizable settings**: Configure check intervals, target hosts, and timeouts
- **Lightweight**: Minimal system resource usage
- **Simple Bitmap Icons**: Clean, reliable icon rendering

## Visual Customization
- **Custom Colors**: Set different colors for connected and disconnected states
- **Shape Options**: Choose between circular and square icon shapes
- **Size Control**: Adjust icon size to your preference

## How it works
The mod performs periodic connectivity checks using:
1. ICMP ping to specified target hosts
2. HTTP connectivity verification
3. Network adapter status monitoring

## Usage
Once installed, the mod runs automatically with explorer.exe and provides:
- Continuous background monitoring
- Log entries showing connectivity status
- Configurable check intervals and targets
- Customizable visual appearance

Check the Windhawk log to see connectivity status updates in real-time.
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
  $name: Target Host
  $description: Primary host to ping for connectivity checks (Google DNS by default)

- secondaryHost: "1.1.1.1"
  $name: Secondary Host  
  $description: Fallback host for connectivity verification (Cloudflare DNS)

- timeout: 3000
  $name: Timeout (ms)
  $description: Maximum wait time for connectivity checks

- enableHttpCheck: true
  $name: Enable HTTP Checks
  $description: Perform HTTP connectivity tests in addition to ping

- httpTestUrl: "http://www.google.com/generate_204"
  $name: HTTP Test URL
  $description: URL used for HTTP connectivity verification

- enableSpeedTest: false
  $name: Enable Speed Testing
  $description: Periodically test network speeds (may use bandwidth)

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
    bool enableHttpCheck;
    std::string httpTestUrl;
    bool enableSpeedTest;
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
        BITMAPINFO bi = {0};
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
            Wh_Log(L"⚠️ TrayIconManager already initialized, skipping");
            return true;
        }
        
        hwnd = window;
        
        // Ensure any existing icon is removed first
        Shell_NotifyIcon(NIM_DELETE, &nid);
        
        // Create icons with settings
        CreateIcons(settings);
        
        if (!hIconConnected || !hIconDisconnected) {
            Wh_Log(L"❌ Failed to create icons");
            return false;
        }
        
        // Initialize NOTIFYICONDATA
        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = ID_TRAYICON;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = hIconDisconnected; // Start with disconnected state
        wcscpy_s(nid.szTip, L"Internet Status: Starting...");
        
        isInitialized = true;
        Wh_Log(L"✅ TrayIconManager initialized");
        return true;
    }
    
    bool Show() {
        if (!hwnd || !isInitialized) {
            Wh_Log(L"❌ Cannot show icon - not initialized");
            return false;
        }
        
        if (iconVisible) {
            Wh_Log(L"⚠️ Icon already visible");
            return true;
        }
        
        bool result = Shell_NotifyIcon(NIM_ADD, &nid) != FALSE;
        if (result) {
            iconVisible = true;
            Wh_Log(L"✅ Tray icon shown successfully");
        } else {
            Wh_Log(L"❌ Failed to show tray icon (Shell_NotifyIcon returned error)");
        }
        return result;
    }
    
    bool Hide() {
        if (!iconVisible) return true;
        
        bool result = Shell_NotifyIcon(NIM_DELETE, &nid) != FALSE;
        if (result) {
            iconVisible = false;
            Wh_Log(L"✅ Tray icon hidden");
        }
        return result;
    }
    
    void UpdateStatus(bool connected, bool forceUpdate = false) {
        if (!iconVisible || !isInitialized) return;
        
        // Always update on force or state change
        if (!forceUpdate && currentConnectionState == connected) return;
        
        currentConnectionState = connected;
        nid.hIcon = connected ? hIconConnected : hIconDisconnected;
        
        // Update tooltip
        const wchar_t* status = connected ? L"Connected" : L"Disconnected";
        const wchar_t* indicator = connected ? L"🟢" : L"🔴";
        swprintf_s(nid.szTip, L"Internet Status: %s %s", status, indicator);
        
        // Update the icon
        bool success = Shell_NotifyIcon(NIM_MODIFY, &nid) != FALSE;
        
        if (!success) {
            Wh_Log(L"⚠️ Icon update failed, attempting to re-add");
            // If modify fails, try to re-add the icon
            Shell_NotifyIcon(NIM_DELETE, &nid);
            Sleep(50);
            Shell_NotifyIcon(NIM_ADD, &nid);
        }
    }
    
    void UpdateSettings(const NetworkSettings& settings) {
        if (!isInitialized) return;
        
        bool wasVisible = iconVisible;
        bool wasConnected = currentConnectionState;
        
        // Update icons without hiding/showing
        CreateIcons(settings);
        
        if (wasVisible) {
            // Update current icon immediately
            nid.hIcon = wasConnected ? hIconConnected : hIconDisconnected;
            Shell_NotifyIcon(NIM_MODIFY, &nid);
            Wh_Log(L"✅ Icon appearance updated");
        }
    }
    
    bool IsVisible() const { return iconVisible; }
    bool IsInitialized() const { return isInitialized; }
    
    void Cleanup() {
        if (iconVisible) {
            Hide();
        }
        isInitialized = false;
        currentConnectionState = false;
        Wh_Log(L"✅ TrayIconManager cleaned up");
    }
};

// Hidden window for tray icon messages
class TrayWindow {
private:
    HWND hwnd;
    static const wchar_t* CLASS_NAME;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_TRAYICON:
                if (LOWORD(lParam) == WM_RBUTTONUP || LOWORD(lParam) == WM_LBUTTONUP) {
                    // Could add context menu here in the future
                    return 0;
                }
                break;
                
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
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
            0, // Hidden window
            0, 0, 0, 0,
            HWND_MESSAGE, // Message-only window
            NULL,
            hInstance,
            NULL
        );
        
        return hwnd != NULL;
    }
    
    HWND GetHandle() const { return hwnd; }
};

const wchar_t* TrayWindow::CLASS_NAME = L"InternetStatusTrayWindow";

class InternetStatusMonitor {
private:
    std::atomic<bool> isRunning{false};
    std::atomic<bool> isConnected{false};
    std::thread monitorThread;
    NetworkSettings settings;
    HANDLE hIcmpFile;
    
    // Network status tracking
    DWORD lastConnectedTime;
    DWORD lastDisconnectedTime;
    int consecutiveFailures;
    
    // Tray icon components
    std::unique_ptr<TrayWindow> trayWindow;
    std::unique_ptr<TrayIconManager> trayIcon;
    bool trayIconInitialized;
    
public:
    InternetStatusMonitor() : hIcmpFile(INVALID_HANDLE_VALUE), 
                             lastConnectedTime(0), 
                             lastDisconnectedTime(0),
                             consecutiveFailures(0),
                             trayIconInitialized(false) {
        WSAData wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        
        // Initialize tray components
        trayWindow = std::make_unique<TrayWindow>();
        trayIcon = std::make_unique<TrayIconManager>();
    }
    
    ~InternetStatusMonitor() {
        Stop();
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            IcmpCloseHandle(hIcmpFile);
        }
        
        // Clean up tray icon
        if (trayIcon) {
            trayIcon->Cleanup();
        }
        
        WSACleanup();
    }
    
    bool InitializeTrayIcon() {
        if (!settings.showTrayIcon) {
            Wh_Log(L"ℹ️ Tray icon disabled in settings");
            return true;
        }
        
        if (trayIconInitialized) {
            Wh_Log(L"⚠️ Tray icon already initialized");
            return true;
        }
        
        if (!trayWindow->Create()) {
            Wh_Log(L"❌ Failed to create tray window");
            return false;
        }
        
        if (!trayIcon->Initialize(trayWindow->GetHandle(), settings)) {
            Wh_Log(L"❌ Failed to initialize tray icon");
            return false;
        }
        
        if (!trayIcon->Show()) {
            Wh_Log(L"❌ Failed to show tray icon");
            return false;
        }
        
        trayIconInitialized = true;
        
        const wchar_t* shapeText = (settings.iconShape == SHAPE_CIRCLE) ? L"Circle" : L"Square";
        Wh_Log(L"✅ Tray icon initialized successfully using Custom Bitmap (%s, Connected: RGB(%d,%d,%d), Disconnected: RGB(%d,%d,%d))", 
               shapeText,
               settings.connectedColorR, settings.connectedColorG, settings.connectedColorB,
               settings.disconnectedColorR, settings.disconnectedColorG, settings.disconnectedColorB);
        return true;
    }
    
    bool InitializeIcmp() {
        hIcmpFile = IcmpCreateFile();
        return hIcmpFile != INVALID_HANDLE_VALUE;
    }
    
    bool PingHost(const std::string& hostname) {
        if (hIcmpFile == INVALID_HANDLE_VALUE) {
            if (!InitializeIcmp()) {
                return false;
            }
        }
        
        // Convert hostname to IP
        struct addrinfo hints = {0};
        struct addrinfo* result = nullptr;
        hints.ai_family = AF_INET;
        
        if (getaddrinfo(hostname.c_str(), nullptr, &hints, &result) != 0) {
            if (settings.logVerbose) {
                Wh_Log(L"DNS resolution failed for %S", hostname.c_str());
            }
            return false;
        }
        
        struct sockaddr_in* addr = (struct sockaddr_in*)result->ai_addr;
        IPAddr destIP = addr->sin_addr.s_addr;
        freeaddrinfo(result);
        
        // Ping parameters
        char sendData[] = "NetworkStatusCheck";
        DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
        LPVOID replyBuffer = malloc(replySize);
        
        if (!replyBuffer) return false;
        
        DWORD numReplies = IcmpSendEcho(
            hIcmpFile,
            destIP,
            sendData,
            sizeof(sendData),
            nullptr,
            replyBuffer,
            replySize,
            settings.timeout
        );
        
        bool success = false;
        if (numReplies > 0) {
            PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)replyBuffer;
            success = (reply->Status == IP_SUCCESS);
            
            if (success && settings.logVerbose) {
                Wh_Log(L"Ping to %S successful - RTT: %dms", 
                       hostname.c_str(), reply->RoundTripTime);
            }
        }
        
        free(replyBuffer);
        return success;
    }
    
    bool CheckHttpConnectivity() {
        if (!settings.enableHttpCheck) return true;
        
        HINTERNET hInternet = InternetOpenA("NetworkStatusIndicator/1.0",
                                          INTERNET_OPEN_TYPE_PRECONFIG,
                                          nullptr, nullptr, 0);
        if (!hInternet) return false;
        
        HINTERNET hUrl = InternetOpenUrlA(hInternet,
                                        settings.httpTestUrl.c_str(),
                                        nullptr, 0,
                                        INTERNET_FLAG_NO_CACHE_WRITE |
                                        INTERNET_FLAG_NO_UI |
                                        INTERNET_FLAG_PRAGMA_NOCACHE |
                                        INTERNET_FLAG_RELOAD,
                                        0);
        
        bool success = (hUrl != nullptr);
        
        if (hUrl) {
            InternetCloseHandle(hUrl);
        }
        InternetCloseHandle(hInternet);
        
        return success;
    }
    
    bool CheckNetworkAdapters() {
        PIP_ADAPTER_INFO adapterInfo = nullptr;
        ULONG bufferSize = 0;
        
        // Get required buffer size
        GetAdaptersInfo(nullptr, &bufferSize);
        adapterInfo = (PIP_ADAPTER_INFO)malloc(bufferSize);
        
        if (!adapterInfo) return false;
        
        bool hasActiveAdapter = false;
        if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
            PIP_ADAPTER_INFO adapter = adapterInfo;
            while (adapter) {
                if (adapter->Type == MIB_IF_TYPE_ETHERNET ||
                    adapter->Type == IF_TYPE_IEEE80211) {
                    // Check if adapter has valid IP
                    if (strcmp(adapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0) {
                        hasActiveAdapter = true;
                        break;
                    }
                }
                adapter = adapter->Next;
            }
        }
        
        free(adapterInfo);
        return hasActiveAdapter;
    }
    
    void PerformConnectivityCheck() {
        bool wasConnected = isConnected.load();
        bool currentlyConnected = false;
        
        // Multi-layered connectivity check
        bool adapterCheck = CheckNetworkAdapters();
        bool primaryPing = false;
        bool secondaryPing = false;
        bool httpCheck = false;
        
        if (adapterCheck) {
            primaryPing = PingHost(settings.targetHost);
            if (!primaryPing) {
                secondaryPing = PingHost(settings.secondaryHost);
            }
            httpCheck = CheckHttpConnectivity();
        }
        
        // Determine connectivity status
        currentlyConnected = adapterCheck && (primaryPing || secondaryPing) && 
                           (httpCheck || !settings.enableHttpCheck);
        
        // Update internal state
        if (currentlyConnected != wasConnected) {
            isConnected.store(currentlyConnected);
            
            if (currentlyConnected) {
                lastConnectedTime = GetTickCount();
                consecutiveFailures = 0;
                Wh_Log(L"🌐 Internet connection established");
            } else {
                lastDisconnectedTime = GetTickCount();
                Wh_Log(L"❌ Internet connection lost");
            }
        }
        
        // ALWAYS update tray icon to ensure it reflects current status
        if (settings.showTrayIcon && trayIcon && trayIcon->IsVisible()) {
            trayIcon->UpdateStatus(currentlyConnected, true); // Always force update
        }
        
        // Reset consecutive failures if connected
        if (currentlyConnected && consecutiveFailures > 0) {
            consecutiveFailures = 0;
        } else if (!currentlyConnected) {
            consecutiveFailures++;
        }
        
        // Verbose logging
        if (settings.logVerbose) {
            const wchar_t* shapeText = (settings.iconShape == SHAPE_CIRCLE) ? L"Circle" : L"Square";
            
            Wh_Log(L"Status: Adapter=%s, Primary=%s, Secondary=%s, HTTP=%s, Result=%s, Shape=%s, IconState=%s",
                   adapterCheck ? L"OK" : L"FAIL",
                   primaryPing ? L"OK" : L"FAIL",
                   secondaryPing ? L"OK" : L"FAIL",
                   httpCheck ? L"OK" : L"FAIL",
                   currentlyConnected ? L"CONNECTED" : L"DISCONNECTED",
                   shapeText,
                   currentlyConnected ? L"GREEN" : L"RED");
        }
    }
    
    void MonitorLoop() {
        Wh_Log(L"🚀 Internet Status Monitor started");
        
        // Perform immediate initial check
        Sleep(1000); // Brief delay for initialization
        PerformConnectivityCheck();
        
        while (isRunning.load()) {
            auto startTime = std::chrono::steady_clock::now();
            
            try {
                PerformConnectivityCheck();
            } catch (...) {
                Wh_Log(L"⚠️ Error during connectivity check");
            }
            
            auto endTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime).count();
            
            // Sleep for remaining interval time
            int sleepTime = settings.checkInterval - (int)elapsed;
            if (sleepTime > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
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
        
        isRunning.store(false);
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
    
    void UpdateSettings(const NetworkSettings& newSettings) {
        bool visualChanged = (settings.connectedColorR != newSettings.connectedColorR) ||
                            (settings.connectedColorG != newSettings.connectedColorG) ||
                            (settings.connectedColorB != newSettings.connectedColorB) ||
                            (settings.disconnectedColorR != newSettings.disconnectedColorR) ||
                            (settings.disconnectedColorG != newSettings.disconnectedColorG) ||
                            (settings.disconnectedColorB != newSettings.disconnectedColorB) ||
                            (settings.iconShape != newSettings.iconShape) ||
                            (settings.iconSize != newSettings.iconSize);
        
        bool trayIconToggled = (settings.showTrayIcon != newSettings.showTrayIcon);
        
        settings = newSettings;
        
        // Ensure minimum interval
        if (settings.checkInterval < 1000) {
            settings.checkInterval = 1000;
        }
        
        // Ensure valid shape
        if (settings.iconShape < 0 || settings.iconShape > 1) {
            settings.iconShape = 0; // Default to circle
        }
        
        // Handle tray icon toggling
        if (trayIconToggled) {
            if (settings.showTrayIcon && !trayIconInitialized) {
                // Enable tray icon
                InitializeTrayIcon();
                if (trayIcon && trayIcon->IsVisible()) {
                    trayIcon->UpdateStatus(isConnected.load(), true);
                }
            } else if (!settings.showTrayIcon && trayIconInitialized) {
                // Disable tray icon
                if (trayIcon) {
                    trayIcon->Cleanup();
                }
                trayIconInitialized = false;
            }
        } 
        // Handle visual changes to existing tray icon
        else if (visualChanged && settings.showTrayIcon && trayIconInitialized && trayIcon) {
            Wh_Log(L"🎨 Updating icon appearance");
            trayIcon->UpdateSettings(settings);
        }
        
        const wchar_t* shapeText = (settings.iconShape == SHAPE_CIRCLE) ? L"Circle" : L"Square";
        Wh_Log(L"⚙️ Settings updated - Interval: %dms, Target: %S, TrayIcon: %s, Shape: %s, Connected: RGB(%d,%d,%d), Disconnected: RGB(%d,%d,%d)",
               settings.checkInterval, settings.targetHost.c_str(),
               settings.showTrayIcon ? L"ON" : L"OFF", shapeText,
               settings.connectedColorR, settings.connectedColorG, settings.connectedColorB,
               settings.disconnectedColorR, settings.disconnectedColorG, settings.disconnectedColorB);
    }
    
    bool IsConnected() const {
        return isConnected.load();
    }
    
    void GetStatusInfo(DWORD* lastConnected, DWORD* lastDisconnected, int* failures) const {
        if (lastConnected) *lastConnected = lastConnectedTime;
        if (lastDisconnected) *lastDisconnected = lastDisconnectedTime;
        if (failures) *failures = consecutiveFailures;
    }
};

// Global monitor instance
static std::unique_ptr<InternetStatusMonitor> g_monitor;

void LoadSettings() {
    NetworkSettings settings;
    
    settings.checkInterval = Wh_GetIntSetting(L"checkInterval");
    
    // Convert wide string settings to narrow strings
    PCWSTR targetHostW = Wh_GetStringSetting(L"targetHost");
    PCWSTR secondaryHostW = Wh_GetStringSetting(L"secondaryHost");
    PCWSTR httpTestUrlW = Wh_GetStringSetting(L"httpTestUrl");
    
    // Convert to narrow strings
    int len = WideCharToMultiByte(CP_UTF8, 0, targetHostW, -1, nullptr, 0, nullptr, nullptr);
    settings.targetHost.resize(len - 1);
    WideCharToMultiByte(CP_UTF8, 0, targetHostW, -1, &settings.targetHost[0], len, nullptr, nullptr);
    
    len = WideCharToMultiByte(CP_UTF8, 0, secondaryHostW, -1, nullptr, 0, nullptr, nullptr);
    settings.secondaryHost.resize(len - 1);
    WideCharToMultiByte(CP_UTF8, 0, secondaryHostW, -1, &settings.secondaryHost[0], len, nullptr, nullptr);
    
    len = WideCharToMultiByte(CP_UTF8, 0, httpTestUrlW, -1, nullptr, 0, nullptr, nullptr);
    settings.httpTestUrl.resize(len - 1);
    WideCharToMultiByte(CP_UTF8, 0, httpTestUrlW, -1, &settings.httpTestUrl[0], len, nullptr, nullptr);
    
    settings.timeout = Wh_GetIntSetting(L"timeout");
    settings.enableHttpCheck = Wh_GetIntSetting(L"enableHttpCheck") != 0;
    settings.enableSpeedTest = Wh_GetIntSetting(L"enableSpeedTest") != 0;
    settings.iconSize = Wh_GetIntSetting(L"iconSize");
    settings.showTrayIcon = Wh_GetIntSetting(L"showTrayIcon") != 0;
    
    // Load color settings
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

BOOL Wh_ModInit() {
    Wh_Log(L"🌐 Initializing Internet Status Indicator (Custom Bitmap)");
    
    try {
        g_monitor = std::make_unique<InternetStatusMonitor>();
        LoadSettings();
        
        // Initialize tray icon
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

void Wh_ModUninit() {
    Wh_Log(L"🔄 Uninitializing Internet Status Indicator");
    
    if (g_monitor) {
        g_monitor->Stop();
        g_monitor.reset();
    }
    
    Wh_Log(L"✅ Internet Status Indicator uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"⚙️ Internet Status Indicator settings changed");
    LoadSettings();
}
