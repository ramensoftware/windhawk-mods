// ==WindhawkMod==
// @id              window-close-fx
// @name            Window Closing Animations
// @description     Applies 7 custom modern window closing animations with multi-process protection.
// @version         1.0.0
// @author          TheSerphh (Sourav Gope)
// @github          TheSerphh
// @include         *
// @exclude         dwm.exe
// @exclude         windhawk.exe
// @exclude         csrss.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         lsass.exe
// @exclude         smss.exe
// @exclude         svchost.exe
// @exclude         services.exe
// @compilerOptions -lgdi32 -lgdiplus -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Closing Animations (7-in-1 Modern Edition)

Bring modern, fluid, and nostalgic window closing animations to your Windows desktop. This mod replaces the standard instant-vanish window close with high-performance graphical effects.

### 🌟 Available Animations
*   🔥 **Fire / Burn:** The classic KDE Compiz effect with procedural heat gradients and rising embers.
*   💥 **Kinetic Shatter:** The window shatters into glass-like shards that explode outward.
*   📺 **CRT TV Shutoff:** The nostalgic retro TV shutdown effect.
*   👾 **Digital Glitch:** The window tears into horizontal digital strips that dissolve.
*   🧊 **Origami Fold:** A sharp, geometric squish into a 2D vertical line.
*   👁️ **Circular Iris:** A smooth, camera-lens style circular mask collapse.
*   🔲 **Mosaic Pixelate:** A high-performance grid of shrinking squares.

### 🛡️ Smart Protection Engine
This mod features a built-in sandbox detector. It dynamically detects and avoids injecting into UWP apps (Snipping Tool, Photos, Calculator) and Shell Experience hosts, preventing the crashes commonly associated with global GDI+ UI hooking.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- effectType: pixelate
  $name: Window Closing Animation
  $description: Choose the animation effect applied on close.
  $options:
    - fire: Fire / Burn (KDE)
    - shatter: Kinetic Shatter
    - glitch: Digital Glitch
    - crt: CRT TV Shutoff
    - fold: Origami Geometric Fold
    - iris: Circular Iris Collapse
    - pixelate: Mosaic Block Dissolve
- animDuration: 800
  $name: Animation Duration (ms)
  $description: Total time for the window to disintegrate.
- emberCount: 50
  $name: Spark Density (Fire Only)
  $description: Number of floating fire particles emitted.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <gdiplus.h>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace Gdiplus;

ULONG_PTR g_gdiplusToken;
bool g_gdiInitialized = false;

int g_animDuration = 800;
int g_emberCount = 50;
std::wstring g_effectType = L"pixelate";

std::atomic<int> g_activeAnimationCount{0};

#define WM_PROCEED_WITH_CLOSE (WM_USER + 0x4242)

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// --- Shared Utilities ---
inline float EaseInQuad(float t) { return t * t; }
inline float EaseInCubic(float t) { return t * t * t; }

Color GetFireColor(float heat, BYTE alpha = 255) {
    if (heat > 0.85f) return Color(alpha, 255, 255, (BYTE)(200 * (heat - 0.85f) / 0.15f));
    if (heat > 0.60f) return Color(alpha, 255, (BYTE)(220 * (heat - 0.6f) / 0.25f), 0);
    if (heat > 0.30f) return Color(alpha, 255, (BYTE)(80 * (heat - 0.3f) / 0.3f), 0);
    if (heat > 0.10f) return Color(alpha, (BYTE)(180 * heat / 0.3f), 10, 0);
    return Color((BYTE)(alpha * 0.4f), 40, 40, 40);
}

struct Ember { float x, y, vx, vy, life, decay, size; };
struct Shard { float x, y, vx, vy, angle, rotSpeed; Rect src; int w, h; };
struct PixelBlock { RectF src; float delay; };

LRESULT CALLBACK AnimWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// --- Main Animation Engine ---
void ExecuteAnimation(HWND hTargetWnd, RECT rc, HBITMAP hCapturedBmp, std::wstring effect, bool postClose, HANDLE hWaitEvent = NULL) {
    g_activeAnimationCount++;

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = AnimWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkAnimOverlay";
    RegisterClassW(&wc);

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    
    int padX = 0, padY = 0, offsetX = 0, offsetY = 0;
    
    if (effect == L"fire") {
        padY = 80; offsetY = 80;
    } else if (effect == L"shatter") {
        padX = 100; padY = 150; offsetX = 50;
    }

    int overlayW = w + padX;
    int overlayH = h + padY;

    HWND hOverlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        wc.lpszClassName, L"", WS_POPUP,
        rc.left - offsetX, rc.top - offsetY, 
        overlayW, overlayH,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!hOverlay) {
        DeleteObject(hCapturedBmp);
        if (hWaitEvent) SetEvent(hWaitEvent);
        if (postClose) PostMessageW(hTargetWnd, WM_PROCEED_WITH_CLOSE, 0, 0);
        g_activeAnimationCount--;
        return;
    }
    ShowWindow(hOverlay, SW_SHOW);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = overlayW;
    bmi.bmiHeader.biHeight = -overlayH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* pPixels = nullptr;
    HBITMAP hOverlayBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void**)&pPixels, NULL, 0);
    SelectObject(hdcMem, hOverlayBmp);

    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Bitmap original(hCapturedBmp, NULL);

    std::vector<Ember> embers;
    std::vector<Shard> shards;
    std::vector<PixelBlock> pBlocks;

    if (effect == L"fire") {
        for (int i = 0; i < g_emberCount; ++i) {
            embers.push_back({ (float)(rand() % w) + offsetX, (float)(h + offsetY), ((rand() % 100) / 50.0f - 1.0f) * 1.5f, -((rand() % 100) / 20.0f + 2.5f), 1.0f, (rand() % 50 + 20) / 1000.0f, (float)(rand() % 4 + 2) });
        }
    } else if (effect == L"shatter") {
        int cols = 10, rows = 10;
        int cw = w / cols, ch = h / rows;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                Shard s;
                s.w = cw; s.h = ch;
                s.src = Rect(c * cw, r * ch, cw, ch);
                s.x = (float)(c * cw + offsetX); 
                s.y = (float)(r * ch + offsetY);
                s.vx = ((rand() % 100) / 50.0f - 1.0f) * 4.0f;
                s.vy = ((rand() % 100) / 50.0f - 1.0f) * 4.0f - 2.0f;
                s.angle = 0; 
                s.rotSpeed = ((rand() % 100) / 50.0f - 1.0f) * 8.0f;
                shards.push_back(s);
            }
        }
    } else if (effect == L"pixelate") {
        int pSize = 40; 
        for(int py = 0; py < h; py += pSize) {
            for(int px = 0; px < w; px += pSize) {
                 PixelBlock pb;
                 pb.src = RectF((float)px, (float)py, (float)std::min(pSize, w-px), (float)std::min(pSize, h-py));
                 pb.delay = (rand() % 100) / 100.0f * 0.5f; 
                 pBlocks.push_back(pb);
            }
        }
    }

    LARGE_INTEGER freq, start, current;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    
    float progress = 0.0f;
    float lastElapsedMs = 0.0f;

    while (progress < 1.0f) {
        QueryPerformanceCounter(&current);
        float elapsedMs = (float)(current.QuadPart - start.QuadPart) * 1000.0f / (float)freq.QuadPart;
        progress = elapsedMs / (float)g_animDuration;
        if (progress > 1.0f) progress = 1.0f;

        float deltaFrames = (elapsedMs - lastElapsedMs) / 16.666f; 
        lastElapsedMs = elapsedMs;

        graphics.Clear(Color(0, 0, 0, 0));
        BYTE masterAlpha = 255;

        // 1. FIRE
        if (effect == L"fire") {
            float burnY = h * (1.0f - progress);
            int remainingHeight = (int)burnY;
            if (remainingHeight > 0) {
                Rect destRect(offsetX, offsetY, w, remainingHeight);
                graphics.DrawImage(&original, destRect, 0, 0, w, remainingHeight, UnitPixel);
                LinearGradientBrush charBrush(Point(0, offsetY + remainingHeight - 14), Point(0, offsetY + remainingHeight), Color(0, 0, 0, 0), Color(240, 20, 10, 0));
                graphics.FillRectangle(&charBrush, offsetX, offsetY + remainingHeight - 14, w, 14);
            }
            for (int x = 0; x < w; x += 12) {
                float wave = sinf((x * 0.08f) + (progress * 15.0f)) * 14.0f + ((rand() % 10) - 5.0f);
                float flameHeight = (35.0f + wave) * (1.0f - progress * 0.4f);
                float baseFlY = offsetY + burnY;
                SolidBrush outer(GetFireColor(0.45f, 210)), inner(GetFireColor(0.80f, 240)), core(GetFireColor(0.95f, 255));
                PointF outerPts[3] = { PointF(offsetX + x - 8, baseFlY), PointF(offsetX + x + 6, baseFlY - flameHeight), PointF(offsetX + x + 20, baseFlY) };
                graphics.FillPolygon(&outer, outerPts, 3);
                PointF innerPts[3] = { PointF(offsetX + x - 3, baseFlY), PointF(offsetX + x + 6, baseFlY - flameHeight * 0.7f), PointF(offsetX + x + 15, baseFlY) };
                graphics.FillPolygon(&inner, innerPts, 3);
            }
            for (auto& em : embers) {
                if (em.life <= 0.0f || em.y < 0) {
                    em.x = (float)(rand() % w) + offsetX; em.y = offsetY + burnY + (rand() % 10 - 5);
                    em.life = 1.0f; em.vx = ((rand() % 100) / 50.0f - 1.0f) * 2.0f; em.vy = -((rand() % 100) / 20.0f + 2.5f);
                }
                em.x += em.vx * deltaFrames; 
                em.y += em.vy * deltaFrames; 
                em.life -= em.decay * deltaFrames;
                SolidBrush emberBrush(GetFireColor(em.life, (BYTE)(em.life * 255)));
                graphics.FillEllipse(&emberBrush, em.x, em.y, em.size, em.size);
            }
        } 
        
        // 2. SHATTER
        else if (effect == L"shatter") {
            if (progress > 0.5f) masterAlpha = (BYTE)(255 * (1.0f - (progress - 0.5f) * 2.0f));
            for (auto& s : shards) {
                s.vy += 0.8f * deltaFrames;
                s.x += s.vx * deltaFrames; 
                s.y += s.vy * deltaFrames; 
                s.angle += s.rotSpeed * deltaFrames;
                GraphicsState state = graphics.Save();
                graphics.TranslateTransform(s.x + s.w / 2.0f, s.y + s.h / 2.0f);
                graphics.RotateTransform(s.angle);
                graphics.TranslateTransform(-(s.x + s.w / 2.0f), -(s.y + s.h / 2.0f));
                graphics.DrawImage(&original, (int)s.x, (int)s.y, s.src.X, s.src.Y, s.w, s.h, UnitPixel);
                graphics.Restore(state);
            }
        }

        // 3. GLITCH
        else if (effect == L"glitch") {
            masterAlpha = (BYTE)(255 * (1.0f - progress));
            int y = 0;
            while (y < h) {
                int stripH = rand() % 25 + 5;
                int shiftX = (rand() % 60) - 30; 
                if (rand() % 100 < 30) shiftX = 0; 
                shiftX = (int)(shiftX * progress * 2.0f);
                Rect destRect(offsetX + shiftX, offsetY + y, w, stripH);
                graphics.DrawImage(&original, destRect, 0, y, w, stripH, UnitPixel);
                y += stripH;
            }
        }

        // 4. CRT TV
        else if (effect == L"crt") {
            float yRatio = (progress < 0.5f) ? (1.0f - progress * 1.95f) : 0.02f;
            float xRatio = (progress < 0.5f) ? 1.0f : (1.0f - (progress - 0.5f) * 2.0f);
            if (yRatio < 0.0f) yRatio = 0.0f;
            if (xRatio < 0.0f) xRatio = 0.0f;
            int curW = (int)(w * xRatio);
            int curH = (int)(h * yRatio);
            int curX = offsetX + (w - curW) / 2;
            int curY = offsetY + (h - curH) / 2;
            graphics.DrawImage(&original, curX, curY, curW, curH);
            if (progress > 0.3f && progress < 0.8f) {
                SolidBrush white(Color(255, 255, 255, 255));
                graphics.FillRectangle(&white, curX, offsetY + (h / 2) - 2, curW, 4);
            }
        }

        // 5. ORIGAMI FOLD
        else if (effect == L"fold") {
            float scaleX = 1.0f, scaleY = 1.0f;
            if (progress < 0.5f) {
                scaleX = 1.0f - EaseInCubic(progress * 2.0f);
                if (scaleX < 0.01f) scaleX = 0.01f;
            } else {
                scaleX = 0.01f; 
                scaleY = 1.0f - EaseInCubic((progress - 0.5f) * 2.0f);
                if (scaleY < 0.0f) scaleY = 0.0f;
            }
            GraphicsState state = graphics.Save();
            graphics.TranslateTransform(offsetX + w/2.0f, offsetY + h/2.0f);
            graphics.ScaleTransform(scaleX, scaleY);
            graphics.TranslateTransform(-w/2.0f, -h/2.0f);
            graphics.DrawImage(&original, 0, 0, w, h);
            graphics.Restore(state);
        }

        // 6. IRIS REVEAL
        else if (effect == L"iris") {
            float p = EaseInCubic(progress);
            float maxRadius = sqrtf((float)(w*w + h*h)) / 2.0f;
            float currentRadius = maxRadius * (1.0f - p);
            
            if (currentRadius > 0.5f) {
                GraphicsState state = graphics.Save();
                GraphicsPath path;
                path.AddEllipse(offsetX + w/2.0f - currentRadius, offsetY + h/2.0f - currentRadius, currentRadius * 2, currentRadius * 2);
                graphics.SetClip(&path);
                graphics.DrawImage(&original, offsetX, offsetY, w, h);
                graphics.Restore(state);
            }
        }

        // 7. HIGH-PERFORMANCE PIXELATE
        else if (effect == L"pixelate") {
            for (auto& b : pBlocks) {
                float p = (progress - b.delay) / 0.5f; 
                if (p < 0.0f) p = 0.0f;
                if (p >= 1.0f) continue;
                
                float scale = 1.0f - EaseInQuad(p);
                float bw = b.src.Width * scale;
                float bh = b.src.Height * scale;
                float bx = offsetX + b.src.X + (b.src.Width - bw) / 2.0f;
                float by = offsetY + b.src.Y + (b.src.Height - bh) / 2.0f;
                
                graphics.DrawImage(&original, RectF(bx, by, bw, bh), b.src.X, b.src.Y, b.src.Width, b.src.Height, UnitPixel);
            }
        }

        POINT ptSrc = { 0, 0 };
        POINT ptDst = { rc.left - offsetX, rc.top - offsetY };
        SIZE sz = { overlayW, overlayH };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };

        UpdateLayeredWindow(hOverlay, hdcScreen, &ptDst, &sz, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        Sleep(1); 
    }

    DestroyWindow(hOverlay);
    DeleteObject(hOverlayBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    DeleteObject(hCapturedBmp);

    if (hWaitEvent) {
        SetEvent(hWaitEvent);
    }

    if (postClose) {
        PostMessageW(hTargetWnd, WM_PROCEED_WITH_CLOSE, 0, 0);
    }

    g_activeAnimationCount--;
}

// --- Eligibility Check ---
bool IsRealTopLevelWindow(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;
    if (!IsWindowVisible(hWnd)) return false;
    if (GetWindow(hWnd, GW_OWNER) != NULL) return false; 
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;

    wchar_t className[256];
    GetClassNameW(hWnd, className, 256);
    static const wchar_t* blockedClasses[] = {
        L"tooltips_class32", L"#32768", L"ComboLBox", L"Auto-Suggest",
        L"Windows.UI.Core.CoreWindow", L"IME", L"MSCTFIME UI",
        L"Shell_TrayWnd", L"Progman", L"WorkerW", L"Xaml_WindowedPopupClass",
        L"ApplicationFrameWindow", L"NotifyIconOverflowWindow"
    };
    for (auto& c : blockedClasses) {
        if (wcsstr(className, c)) return false;
    }

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w < 150 || h < 150) return false;

    return true;
}

// --- Universal Intercept Engine ---
bool TriggerAnimIfReady(HWND hWnd, bool isManualIntercept, bool waitForFinish = false) {
    if (!IsRealTopLevelWindow(hWnd)) return false;

    if (!g_gdiInitialized) {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
        g_gdiInitialized = true;
    }

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcTarget = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, w, h);
    SelectObject(hdcTarget, hBmp);
    PrintWindow(hWnd, hdcTarget, PW_RENDERFULLCONTENT);
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcTarget);

    SetPropW(hWnd, L"WindhawkAnimDone", (HANDLE)1);
    
    ShowWindow(hWnd, SW_HIDE);

    HANDLE hWaitEvent = NULL;
    if (waitForFinish) {
        hWaitEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    }

    std::thread(ExecuteAnimation, hWnd, rc, hBmp, g_effectType, isManualIntercept, hWaitEvent).detach();

    if (waitForFinish && hWaitEvent) {
        DWORD timeout = g_animDuration + 1000; 
        while (true) {
            DWORD res = MsgWaitForMultipleObjects(1, &hWaitEvent, FALSE, timeout, QS_ALLINPUT);
            if (res == WAIT_OBJECT_0) {
                break; 
            } else if (res == WAIT_OBJECT_0 + 1) {
                MSG msg;
                while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            } else {
                break; 
            }
        }
        CloseHandle(hWaitEvent);
    }

    return true;
}

// Hook 1: DefWindowProcW 
using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_PROCEED_WITH_CLOSE) {
        DestroyWindow(hWnd);
        return 0;
    }

    if ((uMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE) || uMsg == WM_CLOSE) {
        if (!GetPropW(hWnd, L"WindhawkAnimDone")) {
            if (TriggerAnimIfReady(hWnd, true, false)) return 0; 
        }
    }
    return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
}

// Hook 2: ShowWindow 
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE && !GetPropW(hWnd, L"WindhawkAnimDone")) {
        TriggerAnimIfReady(hWnd, false, true); 
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// Hook 3: SetWindowPos 
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if ((uFlags & SWP_HIDEWINDOW) && !GetPropW(hWnd, L"WindhawkAnimDone")) {
        TriggerAnimIfReady(hWnd, false, true); 
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// Hook 4: DestroyWindow 
using DestroyWindow_t = decltype(&DestroyWindow);
DestroyWindow_t DestroyWindow_Original;
BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    if (!GetPropW(hWnd, L"WindhawkAnimDone")) {
        TriggerAnimIfReady(hWnd, false, true); 
    }
    return DestroyWindow_Original(hWnd);
}

// Hook 5: ExitProcess (Guards Firefox & multi-process browser shutdowns)
// Using raw typedef and Wh_SetFunctionHook to prevent strict template deduction failures
using ExitProcess_t = decltype(&ExitProcess);
ExitProcess_t ExitProcess_Original;
DECLSPEC_NORETURN VOID WINAPI ExitProcess_Hook(UINT uExitCode) {
    int waitLimit = 0;
    while (g_activeAnimationCount.load() > 0 && waitLimit < (g_animDuration + 200)) {
        Sleep(15);
        waitLimit += 15;
    }
    ExitProcess_Original(uExitCode);
}

// --- Initialization & Smart Sandbox Detector ---
void Wh_ModSettingsChanged() {
    PCWSTR effect = Wh_GetStringSetting(L"effectType");
    g_effectType = effect ? effect : L"pixelate";
    Wh_FreeStringSetting(effect);

    g_animDuration = Wh_GetIntSetting(L"animDuration");
    if (g_animDuration < 100) g_animDuration = 100;

    g_emberCount = Wh_GetIntSetting(L"emberCount");
    if (g_emberCount < 10) g_emberCount = 10;
}

BOOL Wh_ModInit() {
    // SMART SANDBOX DETECTOR: Prevent injection into UWP and Shell system apps
    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
        std::wstring pathStr = exePath;
        std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), ::towlower);

        if (pathStr.find(L"\\windowsapps\\") != std::wstring::npos || 
            pathStr.find(L"\\systemapps\\") != std::wstring::npos ||
            pathStr.find(L"applicationframehost.exe") != std::wstring::npos ||
            pathStr.find(L"shellexperiencehost.exe") != std::wstring::npos ||
            pathStr.find(L"startmenuexperiencehost.exe") != std::wstring::npos ||
            pathStr.find(L"screenclippinghost.exe") != std::wstring::npos) {
            
            return FALSE; // Silently abort loading to prevent UWP crashes
        }
    }

    Wh_ModSettingsChanged();
    WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(DestroyWindow, DestroyWindow_Hook, &DestroyWindow_Original);
    
    // Bypass WindhawkUtils for ExitProcess to avoid strict C++ '__declspec(noreturn)' template errors
    Wh_SetFunctionHook((void*)&ExitProcess, (void*)ExitProcess_Hook, (void**)&ExitProcess_Original);
    
    return TRUE;
}

void Wh_ModUninit() {
    if (g_gdiInitialized) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiInitialized = false;
    }
}
