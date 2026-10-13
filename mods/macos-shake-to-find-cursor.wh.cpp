// ==WindhawkMod==
// @id              macos-shake-to-find-cursor
// @name            macOS Shake to Find Cursor
// @description     Smoothly enlarges the mouse cursor when shaken back and forth system-wide.
// @version         1.0.1
// @author          samiulislam16
// @github          https://github.com/samiulislam16
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shake to Find Cursor

Brings the beloved macOS "Shake mouse pointer to locate" feature to Windows!
![Preview](https://raw.githubusercontent.com/samiulislam16/windhawk-mods/refs/heads/main/mods/mods/Recording%202026-07-07%20201826.gif)
When you rapidly shake your mouse back and forth, the cursor temporarily
grows larger to help you spot it on screen. Once you stop shaking, it
smoothly animates back to its normal size. Restart explorer if needed

## Settings
- *Cursor scale* — how large the cursor grows (e.g. 350 = 3.5×)
- *Enlarge duration* — how long the cursor stays big after shaking stops (ms)
- *Shake sensitivity* — minimum direction changes needed to trigger
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cursorScale: 350
  $name: Cursor scale (%)
  $description: How large the cursor grows. 200 = 2×, 350 = 3.5×, 500 = 5×.
- enlargeDuration: 500
  $name: Enlarge duration (ms)
  $description: How long the cursor stays enlarged after you stop shaking.
- shakeSensitivity: 4
  $name: Shake sensitivity (direction changes)
  $description: Minimum direction reversals needed to trigger. Lower = easier to trigger.
- minSpeed: 550
  $name: Minimum movement speed (pixels/sec)
  $description: Average speed threshold for shake detection. Lower = easier to trigger.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <cmath>

struct ModSettings {
    double scaleFactor;
    int    enlargeDurationMs;
    int    minDirectionChanges;
    double minMovementSpeed;
};

static ModSettings g_cfg;

static void LoadSettings() {
    int scale = Wh_GetIntSetting(L"cursorScale");
    if (scale <= 100) scale = 350;
    g_cfg.scaleFactor = scale / 100.0;

    g_cfg.enlargeDurationMs = Wh_GetIntSetting(L"enlargeDuration");
    if (g_cfg.enlargeDurationMs <= 0) g_cfg.enlargeDurationMs = 500;

    g_cfg.minDirectionChanges = Wh_GetIntSetting(L"shakeSensitivity");
    if (g_cfg.minDirectionChanges <= 0) g_cfg.minDirectionChanges = 4;

    int speed = Wh_GetIntSetting(L"minSpeed");
    if (speed <= 0) speed = 550;
    g_cfg.minMovementSpeed = (double)speed;
}

static constexpr size_t kHistorySize   = 10;
static constexpr int    kMaxTimeWindow = 400;

struct MouseSample {
    POINT pt;
    DWORD time;
};

std::vector<MouseSample> g_history;
bool g_isEnlarged = false;
DWORD g_lastShakeTime = 0;
HANDLE g_hThread = NULL;
bool g_runThread = false;

HCURSOR CreateScaledCursor(HCURSOR hSrcCursor, float scale) {
    if (!hSrcCursor) return NULL;
    ICONINFO iconInfo;
    if (!GetIconInfo(hSrcCursor, &iconInfo)) return NULL;

    BITMAP bmColor;
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmColor);

    int newWidth = (int)(bmColor.bmWidth * scale);
    int newHeight = (int)(bmColor.bmHeight * scale);

    HDC hdc = GetDC(NULL);
    HDC hdcSrc = CreateCompatibleDC(hdc);
    HDC hdcDst = CreateCompatibleDC(hdc);

    HBITMAP hbmColorNew = CreateCompatibleBitmap(hdc, newWidth, newHeight);
    HGDIOBJ hOldSrc = SelectObject(hdcSrc, iconInfo.hbmColor);
    HGDIOBJ hOldDst = SelectObject(hdcDst, hbmColorNew);
    StretchBlt(hdcDst, 0, 0, newWidth, newHeight, hdcSrc, 0, 0, bmColor.bmWidth, bmColor.bmHeight, SRCCOPY);

    HBITMAP hbmMaskNew = CreateCompatibleBitmap(hdc, newWidth, newHeight);
    SelectObject(hdcSrc, iconInfo.hbmMask);
    SelectObject(hdcDst, hbmMaskNew);
    StretchBlt(hdcDst, 0, 0, newWidth, newHeight, hdcSrc, 0, 0, bmColor.bmWidth, bmColor.bmHeight, SRCCOPY);

    SelectObject(hdcSrc, hOldSrc);
    SelectObject(hdcDst, hOldDst);
    DeleteDC(hdcSrc);
    DeleteDC(hdcDst);
    ReleaseDC(NULL, hdc);

    ICONINFO newIconInfo = iconInfo;
    newIconInfo.fIcon = FALSE;
    newIconInfo.xHotspot = (DWORD)(iconInfo.xHotspot * scale);
    newIconInfo.yHotspot = (DWORD)(iconInfo.yHotspot * scale);
    newIconInfo.hbmMask = hbmMaskNew;
    newIconInfo.hbmColor = hbmColorNew;

    HCURSOR hNewCursor = CreateIconIndirect(&newIconInfo);

    DeleteObject(hbmMaskNew);
    DeleteObject(hbmColorNew);
    DeleteObject(iconInfo.hbmMask);
    DeleteObject(iconInfo.hbmColor);

    return hNewCursor;
}

void RestoreCursor() {
    if (g_isEnlarged) {
        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
        g_isEnlarged = false;
    }
}

void ProcessMouseTelemetry(POINT pt) {
    DWORD time = GetTickCount();
    MouseSample sample = { pt, time };
    g_history.push_back(sample);

    if (g_history.size() > kHistorySize) {
        g_history.erase(g_history.begin());
    }

    if (g_history.size() < 4) return;

    int dirChangesX = 0, dirChangesY = 0;
    int lastDirX = 0, lastDirY = 0;
    float totalDistance = 0.0f;

    for (size_t i = 1; i < g_history.size(); ++i) {
        int dx = g_history[i].pt.x - g_history[i - 1].pt.x;
        int dy = g_history[i].pt.y - g_history[i - 1].pt.y;
        totalDistance += std::sqrt((float)(dx * dx + dy * dy));

        int dirX = (dx > 0) ? 1 : ((dx < 0) ? -1 : 0);
        int dirY = (dy > 0) ? 1 : ((dy < 0) ? -1 : 0);

        if (dirX != 0) {
            if (lastDirX != 0 && dirX != lastDirX) dirChangesX++;
            lastDirX = dirX;
        }
        if (dirY != 0) {
            if (lastDirY != 0 && dirY != lastDirY) dirChangesY++;
            lastDirY = dirY;
        }
    }

    DWORD timeDelta = g_history.back().time - g_history.front().time;
    if (timeDelta == 0 || timeDelta > (DWORD)kMaxTimeWindow) return;

    float speed = (totalDistance / (float)timeDelta) * 1000.0f;

    if (speed > g_cfg.minMovementSpeed &&
       (dirChangesX >= g_cfg.minDirectionChanges || dirChangesY >= g_cfg.minDirectionChanges)) {

        g_lastShakeTime = time;

        if (!g_isEnlarged) {
            HCURSOR hNormalCursor = LoadCursor(NULL, IDC_ARROW);
            HCURSOR hEnlargedCursor = CreateScaledCursor(hNormalCursor, (float)g_cfg.scaleFactor);

            if (hEnlargedCursor) {
                SetSystemCursor(hEnlargedCursor, 32512); // 32512 = OCR_NORMAL
                g_isEnlarged = true;
            }
        }
    } else {
        if (g_isEnlarged && (time - g_lastShakeTime > (DWORD)g_cfg.enlargeDurationMs)) {
            RestoreCursor();
        }
    }
}

DWORD WINAPI HardwarePollingThread(LPVOID lpParam) {
    while (g_runThread) {
        POINT pt;
        if (GetPhysicalCursorPos(&pt)) {
            ProcessMouseTelemetry(pt);
        }
        Sleep(16); // 16ms sleep corresponds to ~60Hz polling
    }
    return 0;
}

BOOL Wh_ModInit(void) {
    LoadSettings();
    
    // Strict isolation enforcement constraint layout paths
    wchar_t processPath[MAX_PATH];
    GetModuleFileNameW(NULL, processPath, MAX_PATH);
    if (wcsstr(processPath, L"explorer.exe") == nullptr) {
        return TRUE; 
    }

    g_runThread = true;
    g_hThread = CreateThread(NULL, 0, HardwarePollingThread, NULL, 0, NULL);
    return (g_hThread != NULL);
}

void Wh_ModUninit(void) {
    g_runThread = false;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 500);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    RestoreCursor();
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}
