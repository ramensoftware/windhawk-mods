// ==WindhawkMod==
// @id              mic-mute-hotkey-overlay
// @name            Global Hotkey Mute Microphone + Floating Overlay
// @description     Global hotkey untuk mute/unmute mic default + overlay indikator melayang, always-on-top, dengan animasi saat mic aktif
// @version         1.0.0
// @author          Farel Hanafi
// @github          https://github.com/Eliasilyz
// @homepage        https://farelhanafi.my.id/
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luser32 -lgdi32 -lgdiplus -ldwmapi -lpropsys
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Hotkey Mute Microphone

Registers a global hotkey to toggle mute on the system's default microphone
(capture device) and shows a floating, always-on-top overlay indicator.

- Overlay pulses/animates while the mic is unmuted.
- Overlay turns red/static with a slash when muted.
- Size, position and hotkey are all configurable via mod settings.
- Overlay auto-hides after a short delay unless "always show" is enabled.

Default hotkey: Ctrl+Alt+M

---

### Report a Bug
If you encounter any issues or have a feature suggestion, please open a report on the project's GitHub page:
👉 **[Report an Issue on GitHub](https://github.com/Eliasilyz/global-hotkey-mute/issues)**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyModifiers: 3
  $name: Hotkey modifiers
  $description: "Bitmask: 1=Alt, 2=Ctrl, 4=Shift, 8=Win. Default 3 = Ctrl+Alt"
- hotkeyVK: 77
  $name: Hotkey virtual-key code
  $description: "Decimal VK code, default 77 = 'M'"
- overlaySize: 72
  $name: Overlay size (px)
- overlayPosX: -1
  $name: Overlay X position
  $description: "-1 = auto (bottom-right)"
- overlayPosY: -1
  $name: Overlay Y position
  $description: "-1 = auto (bottom-right)"
- overlayMargin: 24
  $name: Overlay margin from screen edge (px, used only when position is auto)
- overlayDurationMs: 1500
  $name: Overlay auto-hide delay after toggle (ms)
- alwaysShow: false
  $name: Always show overlay (ignore auto-hide)
- clickThrough: true
  $name: Click-through overlay (don't block mouse)
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WINVER
#define WINVER 0x0600
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06000000
#endif

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <gdiplus.h>
#include <cmath>
#include <algorithm>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct ModSettings {
    UINT hotkeyModifiers;
    UINT hotkeyVK;
    int  overlaySize;
    int  overlayPosX;
    int  overlayPosY;
    int  overlayMargin;
    int  overlayDurationMs;
    bool alwaysShow;
    bool clickThrough;
} g_settings;

void LoadSettings() {
    g_settings.hotkeyModifiers  = (UINT)Wh_GetIntSetting(L"hotkeyModifiers");
    g_settings.hotkeyVK         = (UINT)Wh_GetIntSetting(L"hotkeyVK");
    g_settings.overlaySize      = (int)Wh_GetIntSetting(L"overlaySize");
    g_settings.overlayPosX      = (int)Wh_GetIntSetting(L"overlayPosX");
    g_settings.overlayPosY      = (int)Wh_GetIntSetting(L"overlayPosY");
    g_settings.overlayMargin    = (int)Wh_GetIntSetting(L"overlayMargin");
    g_settings.overlayDurationMs= (int)Wh_GetIntSetting(L"overlayDurationMs");
    g_settings.alwaysShow       = Wh_GetIntSetting(L"alwaysShow") != 0;
    g_settings.clickThrough     = Wh_GetIntSetting(L"clickThrough") != 0;

    if (g_settings.overlaySize < 24) g_settings.overlaySize = 24;
    if (g_settings.overlaySize > 512) g_settings.overlaySize = 512;
}

// ---------------------------------------------------------------------------
// Audio (Core Audio API)
// ---------------------------------------------------------------------------
IMMDeviceEnumerator*   g_pEnumerator   = nullptr;
IMMDevice*              g_pCaptureDev   = nullptr;
IAudioEndpointVolume*   g_pEndpointVol  = nullptr;

bool InitAudio() {
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                   CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                   (void**)&g_pEnumerator);
    if (FAILED(hr) || !g_pEnumerator) return false;

    hr = g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &g_pCaptureDev);
    if (FAILED(hr) || !g_pCaptureDev) return false;

    hr = g_pCaptureDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                  nullptr, (void**)&g_pEndpointVol);
    if (FAILED(hr)) return false;

    return true;
}

void CleanupAudio() {
    if (g_pEndpointVol) { g_pEndpointVol->Release();  g_pEndpointVol = nullptr; }
    if (g_pCaptureDev)  { g_pCaptureDev->Release();   g_pCaptureDev = nullptr; }
    if (g_pEnumerator)  { g_pEnumerator->Release();   g_pEnumerator = nullptr; }
}

bool IsMicMuted() {
    BOOL muted = FALSE;
    if (g_pEndpointVol) g_pEndpointVol->GetMute(&muted);
    return muted != FALSE;
}

void ToggleMicMute() {
    if (!g_pEndpointVol) return;
    BOOL muted = FALSE;
    g_pEndpointVol->GetMute(&muted);
    g_pEndpointVol->SetMute(!muted, nullptr);
}

float g_animPhase = 0.0f;

// Animation "level" is a synthetic 0..1 pulse driven by g_animPhase while the
// mic is unmuted (no real peak-metering interface is used, since it isn't
// reliably available in the Windhawk compiler's SDK headers).
float GetAnimLevel(bool muted) {
    if (muted) return 0.0f;
    return 0.5f + 0.5f * sinf(g_animPhase * 6.2831853f * 0.6f);
}

// ---------------------------------------------------------------------------
// Overlay window
// ---------------------------------------------------------------------------
HWND g_hOverlay = nullptr;
ULONG_PTR g_gdiplusToken = 0;

bool  g_overlayVisible   = false;
DWORD g_lastToggleTick   = 0;

constexpr UINT_PTR TIMER_ANIM   = 1;
constexpr UINT     ANIM_MS      = 33; // ~30 fps

void ComputeOverlayRect(RECT* out) {
    int size = g_settings.overlaySize;
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    int x = g_settings.overlayPosX;
    int y = g_settings.overlayPosY;
    if (x < 0) x = sw - size - g_settings.overlayMargin;
    if (y < 0) y = sh - size - g_settings.overlayMargin;

    out->left = x;
    out->top = y;
    out->right = x + size;
    out->bottom = y + size;
}

// Draws the mic glyph + state animation into a premultiplied-alpha bitmap
// and pushes it to the layered window via UpdateLayeredWindow.
void RenderOverlay(HWND hwnd, float level, bool muted) {
    int size = g_settings.overlaySize;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDC = GetDC(nullptr);
    HBITMAP hBitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screenDC);
    if (!hBitmap || !bits) return;

    {
        Bitmap bmp(size, size, size * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.Clear(Color(0, 0, 0, 0));

        float cx = size / 2.0f;
        float cy = size / 2.0f;
        float baseR = size * 0.42f;

        Color colActive(230, 46, 204, 113);   // green, active
        Color colIdle(210, 90, 100, 110);     // muted grey/blue when unmuted+quiet
        Color colMuted(230, 231, 76, 60);     // red, muted

        // Pulsing ring when unmuted and receiving signal
        if (!muted) {
            for (int ring = 0; ring < 3; ring++) {
                float t = g_animPhase - ring * 0.33f;
                t = t - floorf(t);
                float ringR = baseR + t * baseR * 0.9f * (0.3f + level);
                int alpha = (int)((1.0f - t) * 120 * (0.25f + level));
                if (alpha < 0) alpha = 0;
                if (level > 0.03f) {
                    Pen pen(Color(alpha, 46, 204, 113), 3.0f);
                    g.DrawEllipse(&pen, cx - ringR, cy - ringR, ringR * 2, ringR * 2);
                }
            }
        }

        // Background disc
        SolidBrush bgBrush(muted ? colMuted : (level > 0.03f ? colActive : colIdle));
        g.FillEllipse(&bgBrush, cx - baseR, cy - baseR, baseR * 2, baseR * 2);

        Pen ringPen(Color(255, 255, 255), size * 0.03f);
        g.DrawEllipse(&ringPen, cx - baseR, cy - baseR, baseR * 2, baseR * 2);

        // Mic glyph (capsule body + stand)
        float bodyW = baseR * 0.55f;
        float bodyH = baseR * 1.05f;
        RectF bodyRect(cx - bodyW / 2, cy - bodyH * 0.62f, bodyW, bodyH);
        GraphicsPath body;
        float rad = bodyW / 2.0f;
        body.AddArc(bodyRect.X, bodyRect.Y, bodyW, rad * 2, 180, 180);
        body.AddArc(bodyRect.X, bodyRect.Y + bodyRect.Height - rad * 2, bodyW, rad * 2, 0, 180);
        body.CloseFigure();
        SolidBrush white(Color(255, 255, 255, 255));
        g.FillPath(&white, &body);

        Pen standPen(Color(255, 255, 255, 255), size * 0.045f);
        standPen.SetStartCap(LineCapRound);
        standPen.SetEndCap(LineCapRound);
        float standTop = cy + bodyH * 0.15f;
        float standBottom = cy + baseR * 0.55f;
        g.DrawArc(&standPen, cx - bodyW * 0.85f, cy - bodyH * 0.05f,
                  bodyW * 1.7f, bodyH * 0.75f, 20, 140);
        g.DrawLine(&standPen, cx, standTop + bodyH * 0.30f, cx, standBottom);
        g.DrawLine(&standPen, cx - baseR * 0.32f, standBottom, cx + baseR * 0.32f, standBottom);

        // Slash when muted
        if (muted) {
            Pen slashPen(Color(255, 255, 255, 255), size * 0.06f);
            slashPen.SetStartCap(LineCapRound);
            slashPen.SetEndCap(LineCapRound);
            g.DrawLine(&slashPen, cx - baseR * 0.9f, cy - baseR * 0.9f,
                       cx + baseR * 0.9f, cy + baseR * 0.9f);
        }
    }

    HDC memDC = CreateCompatibleDC(nullptr);
    HGDIOBJ oldBmp = SelectObject(memDC, hBitmap);

    RECT rc;
    ComputeOverlayRect(&rc);
    POINT ptDst = { rc.left, rc.top };
    SIZE  sizeWnd = { size, size };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hwnd, nullptr, &ptDst, &sizeWnd, memDC, &ptSrc,
                         0, &blend, ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    DeleteObject(hBitmap);
}

void ApplyOverlayPositionAndSize(HWND hwnd) {
    RECT rc;
    ComputeOverlayRect(&rc);
    SetWindowPos(hwnd, HWND_TOPMOST, rc.left, rc.top,
                 rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOACTIVATE);
}

void ShowOverlayTemporarily(HWND hwnd) {
    g_lastToggleTick = GetTickCount();
    if (!g_overlayVisible) {
        g_overlayVisible = true;
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }
    SetTimer(hwnd, TIMER_ANIM, ANIM_MS, nullptr);
}

void RegisterGlobalHotkey(HWND hwnd);

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY: {
            if (wParam == 1) {
                ToggleMicMute();
                ShowOverlayTemporarily(hwnd);
                RenderOverlay(hwnd, GetAnimLevel(IsMicMuted()), IsMicMuted());
            }
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                bool muted = IsMicMuted();
                g_animPhase += 0.045f;
                if (g_animPhase > 1.0f) g_animPhase -= 1.0f;

                RenderOverlay(hwnd, GetAnimLevel(muted), muted);

                DWORD elapsed = GetTickCount() - g_lastToggleTick;
                if (!g_settings.alwaysShow &&
                    elapsed > (DWORD)g_settings.overlayDurationMs) {
                    ShowWindow(hwnd, SW_HIDE);
                    g_overlayVisible = false;
                    KillTimer(hwnd, TIMER_ANIM);
                }
            }
            return 0;
        }
        case WM_APP + 1: {
            // Settings changed: re-apply hotkey, size and position live
            RegisterGlobalHotkey(hwnd);
            ApplyOverlayPositionAndSize(hwnd);
            RenderOverlay(hwnd, GetAnimLevel(IsMicMuted()), IsMicMuted());
            return 0;
        }
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ANIM);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

const wchar_t kClassName[] = L"WH_MicMuteOverlayWnd";

HWND CreateOverlayWindow() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassEx(&wc);

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    if (g_settings.clickThrough) exStyle |= WS_EX_TRANSPARENT;

    RECT rc;
    ComputeOverlayRect(&rc);

    HWND hwnd = CreateWindowEx(exStyle, kClassName, L"MicMuteOverlay",
                                WS_POPUP,
                                rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                                nullptr, nullptr, wc.hInstance, nullptr);
    return hwnd;
}

void RegisterGlobalHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, 1);
    UINT mods = 0;
    if (g_settings.hotkeyModifiers & 1) mods |= MOD_ALT;
    if (g_settings.hotkeyModifiers & 2) mods |= MOD_CONTROL;
    if (g_settings.hotkeyModifiers & 4) mods |= MOD_SHIFT;
    if (g_settings.hotkeyModifiers & 8) mods |= MOD_WIN;
    mods |= MOD_NOREPEAT;
    RegisterHotKey(hwnd, 1, mods, g_settings.hotkeyVK);
}

// ---------------------------------------------------------------------------
// Worker thread: owns the overlay/hotkey window and its message loop
// ---------------------------------------------------------------------------
HANDLE g_hThread = nullptr;
DWORD  g_threadId = 0;

DWORD WINAPI ModThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput gdiInput;
    GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr);

    if (!InitAudio()) {
        Wh_Log(L"MicMuteOverlay: failed to init audio endpoint");
    }

    g_hOverlay = CreateOverlayWindow();
    if (g_hOverlay) {
        RegisterGlobalHotkey(g_hOverlay);
        if (g_settings.alwaysShow) {
            ShowOverlayTemporarily(g_hOverlay);
        }
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hOverlay) {
        UnregisterHotKey(g_hOverlay, 1);
        DestroyWindow(g_hOverlay);
        g_hOverlay = nullptr;
    }
    UnregisterClass(kClassName, GetModuleHandle(nullptr));

    CleanupAudio();
    if (g_gdiplusToken) GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings();
    g_hThread = CreateThread(nullptr, 0, ModThreadProc, nullptr, 0, &g_threadId);
    return g_hThread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hOverlay) {
        PostMessage(g_hOverlay, WM_APP + 1, 0, 0);
    }
}

void Wh_ModUninit() {
    if (g_hOverlay) {
        PostMessage(g_hOverlay, WM_QUIT, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}