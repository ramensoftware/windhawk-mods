// ==WindhawkMod==
// @id              mouse-screen-edge-wrap
// @name            Mouse Screen Edge Wrap
// @name:zh-CN      鼠标屏幕边缘穿越
// @description     Allows mouse cursor to wrap around screen edges (horizontal & vertical). Creates an infinite screen effect.
// @description:zh-CN 允许鼠标光标在屏幕边缘之间循环穿越（水平与垂直方向），创造无限屏幕效果。
// @version         1.2
// @author          VisJoker
// @author:zh-CN    VisJoker
// @github          https://github.com/VisJoker
// @homepage        https://github.com/VisJoker/windhawk-mouse-wrap
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mouse Screen Edge Wrap

This mod creates an infinite screen effect by allowing the mouse cursor to wrap around screen edges.

## Features
- **Horizontal Wrap**: Move cursor to right edge → appears on left edge, and vice versa
- **Vertical Wrap** (optional): Move cursor to bottom edge → appears on top edge, and vice versa
- Smooth performance with configurable polling rate
- Low CPU usage

## How to Use
1. Enable the mod
2. Move your mouse to the right edge of the screen → it appears from the left
3. Move your mouse to the left edge of the screen → it appears from the right

## Settings
- **Enable Horizontal Wrap**: Toggle horizontal edge wrapping
- **Enable Vertical Wrap**: Toggle vertical edge wrapping
- **Edge Threshold**: Pixel distance from edge to trigger wrap (1-10px)
- **Polling Interval**: Detection frequency in milliseconds (default: 16ms ≈ 60FPS)

## Notes
- The mod works on the primary monitor only
- Some fullscreen applications may interfere with cursor wrapping
- Adjust polling interval if you experience performance issues

---

# 鼠标屏幕边缘穿越

本 Mod 通过允许鼠标在屏幕边缘之间循环穿越，创造出无限屏幕的效果。

## 功能特性
- **水平穿越**：鼠标移到右边缘 → 从左边缘出现，反之亦然
- **垂直穿越**（可选）：鼠标移到下边缘 → 从上边缘出现，反之亦然
- 流畅的性能，可配置的检测频率
- 低 CPU 占用

## 使用方法
1. 启用本 Mod
2. 将鼠标移到屏幕右边缘 → 它会从左边缘出现
3. 将鼠标移到屏幕左边缘 → 它会从右边缘出现

## 设置选项
- **启用水平穿越**：开启/关闭水平边缘穿越
- **启用垂直穿越**：开启/关闭垂直边缘穿越
- **边缘阈值**：触发穿越的边界像素距离（1-10像素）
- **轮询间隔**：检测频率，单位毫秒（默认：16毫秒 ≈ 60帧）

## 注意事项
- 本 Mod 仅在主显示器上工作
- 某些全屏应用可能会干扰光标穿越
- 如遇到性能问题，请调整轮询间隔
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabledHorizontal: true
  $name: Enable Horizontal Wrap
  $name:zh-CN: 启用水平穿越
  $description: Allow mouse to wrap between left and right screen edges
  $description:zh-CN: 允许鼠标在屏幕左右边缘之间循环穿越

- enabledVertical: false
  $name: Enable Vertical Wrap
  $name:zh-CN: 启用垂直穿越
  $description: Allow mouse to wrap between top and bottom screen edges
  $description:zh-CN: 允许鼠标在屏幕上下边缘之间循环穿越

- threshold: 3
  $name: Edge Threshold
  $name:zh-CN: 边缘阈值
  $description: Pixel distance from screen edge to trigger wrap (1-10 pixels)
  $description:zh-CN: 距离屏幕边缘多少像素时触发穿越（1-10像素）

- pollInterval: 16
  $name: Polling Interval (ms)
  $name:zh-CN: 轮询间隔（毫秒）
  $description: Detection frequency in milliseconds (default 16ms = 60FPS)
  $description:zh-CN: 检测频率，单位毫秒（默认16毫秒 = 60帧）
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>

// Mod settings structure
struct {
    bool enabledHorizontal;
    bool enabledVertical;
    int threshold;
    int pollInterval;
} settings;

// Global variables
std::atomic<bool> g_running(false);
std::atomic<bool> g_isWrapping(false);
HANDLE g_workerThread = NULL;
int g_screenWidth = 0;
int g_screenHeight = 0;

// Update screen resolution (only when changed)
void UpdateScreenResolution() {
    static int lastWidth = 0;
    static int lastHeight = 0;
    
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    
    if (width != lastWidth || height != lastHeight) {
        g_screenWidth = width;
        g_screenHeight = height;
        lastWidth = width;
        lastHeight = height;
        
        Wh_Log(L"Screen resolution updated: %dx%d", width, height);
    }
}

// Worker thread - polls mouse position
DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    POINT lastPos = { -1, -1 };
    
    Wh_Log(L"Worker thread started");
    
    while (g_running) {
        // Only process if enabled and not currently wrapping
        if ((settings.enabledHorizontal || settings.enabledVertical) && !g_isWrapping) {
            UpdateScreenResolution();
            
            POINT cursorPos;
            if (GetCursorPos(&cursorPos)) {
                // Skip if position hasn't changed
                if (cursorPos.x != lastPos.x || cursorPos.y != lastPos.y) {
                    lastPos = cursorPos;
                    
                    bool needsWrap = false;
                    int newX = cursorPos.x;
                    int newY = cursorPos.y;
                    
                    // Check horizontal edges
                    if (settings.enabledHorizontal) {
                        // Right edge -> Left edge
                        if (cursorPos.x >= g_screenWidth - settings.threshold) {
                            newX = settings.threshold + 1;
                            needsWrap = true;
                        }
                        // Left edge -> Right edge
                        else if (cursorPos.x < settings.threshold) {
                            newX = g_screenWidth - settings.threshold - 2;
                            needsWrap = true;
                        }
                    }
                    
                    // Check vertical edges
                    if (settings.enabledVertical) {
                        // Bottom edge -> Top edge
                        if (cursorPos.y >= g_screenHeight - settings.threshold) {
                            newY = settings.threshold + 1;
                            needsWrap = true;
                        }
                        // Top edge -> Bottom edge
                        else if (cursorPos.y < settings.threshold) {
                            newY = g_screenHeight - settings.threshold - 2;
                            needsWrap = true;
                        }
                    }
                    
                    // Perform cursor wrap
                    if (needsWrap) {
                        g_isWrapping = true;
                        SetCursorPos(newX, newY);
                        Wh_Log(L"Mouse wrapped: (%d, %d) -> (%d, %d)", 
                               cursorPos.x, cursorPos.y, newX, newY);
                        
                        // Update lastPos to prevent re-triggering
                        lastPos.x = newX;
                        lastPos.y = newY;
                        
                        g_isWrapping = false;
                    }
                }
            }
        }
        
        // Control polling rate to reduce CPU usage
        Sleep(settings.pollInterval > 0 ? settings.pollInterval : 16);
    }
    
    Wh_Log(L"Worker thread stopped");
    return 0;
}

// Load settings from Windhawk
void LoadSettings() {
    settings.enabledHorizontal = Wh_GetIntSetting(L"enabledHorizontal") != 0;
    settings.enabledVertical = Wh_GetIntSetting(L"enabledVertical") != 0;
    settings.threshold = Wh_GetIntSetting(L"threshold");
    settings.pollInterval = Wh_GetIntSetting(L"pollInterval");
    
    // Validate threshold range
    if (settings.threshold < 1) settings.threshold = 1;
    if (settings.threshold > 10) settings.threshold = 10;
    
    // Validate polling interval
    if (settings.pollInterval < 1) settings.pollInterval = 1;
    if (settings.pollInterval > 100) settings.pollInterval = 100;
    
    Wh_Log(L"Settings loaded: H=%d, V=%d, threshold=%d, interval=%dms",
           settings.enabledHorizontal, settings.enabledVertical, 
           settings.threshold, settings.pollInterval);
}

// Mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"========================================");
    Wh_Log(L"Mouse Screen Edge Wrap Mod v1.2");
    Wh_Log(L"Initializing...");
    Wh_Log(L"========================================");
    
    LoadSettings();
    
    // Initialize screen dimensions
    g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_screenHeight = GetSystemMetrics(SM_CYSCREEN);
    Wh_Log(L"Primary screen: %dx%d", g_screenWidth, g_screenHeight);
    
    // Create worker thread
    g_running = true;
    g_workerThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    
    if (g_workerThread == NULL) {
        Wh_Log(L"ERROR: Failed to create worker thread!");
        return FALSE;
    }
    
    Wh_Log(L"Mod initialized successfully");
    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit() {
    Wh_Log(L"========================================");
    Wh_Log(L"Shutting down...");
    Wh_Log(L"========================================");
    
    // Signal thread to stop
    g_running = false;
    
    // Wait for thread to finish
    if (g_workerThread != NULL) {
        WaitForSingleObject(g_workerThread, 1000);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }
    
    Wh_Log(L"Mod unloaded");
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}
