// ==WindhawkMod==
// @id win10-taskbar-volume-slider
// @name Windows 10 Taskbar Volume Slider
// @description Adds a permanently visible volume slider inside the Windows 10 taskbar.
// @version 1.0.0
// @author didrmt1 (Prompt: Gemini)
// @github https://github.com/didrmt1
// @include explorer.exe
// @compilerOptions -lole32 -lgdi32 -lcomctl32 -luxtheme
// ==/WindhawkMod==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <algorithm>

#include <windhawk_utils.h>

static int g_width = 200;
static bool g_showPercent = true;

static HWND g_taskbar = nullptr;
static HWND g_slider = nullptr;
static float g_currentVolume = 0.0f;
static HANDLE g_hThread = nullptr;
static IAudioEndpointVolume* g_volumeEndpoint = nullptr;

static void InitAudioEndpoint()
{
    if (g_volumeEndpoint) return;
    IMMDeviceEnumerator* enumerator = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator))))
    {
        IMMDevice* device = nullptr;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
        {
            device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&g_volumeEndpoint);
            device->Release();
        }
        enumerator->Release();
    }
}

static void UninitAudioEndpoint()
{
    if (g_volumeEndpoint)
    {
        g_volumeEndpoint->Release();
        g_volumeEndpoint = nullptr;
    }
}

static float GetVolume()
{
    if (!g_volumeEndpoint) InitAudioEndpoint();
    if (!g_volumeEndpoint) return g_currentVolume;

    float volume = g_currentVolume;
    g_volumeEndpoint->GetMasterVolumeLevelScalar(&volume);
    return volume;
}

static void SetVolume(float value)
{
    if (!g_volumeEndpoint) InitAudioEndpoint();
    if (!g_volumeEndpoint) return;

    value = std::clamp(value, 0.0f, 1.0f);
    g_volumeEndpoint->SetMasterVolumeLevelScalar(value, nullptr);
}

static void PositionSlider()
{
    if (!g_taskbar || !IsWindow(g_taskbar) || !g_slider || !IsWindow(g_slider))
        return;

    RECT taskbarRc{};
    GetClientRect(g_taskbar, &taskbarRc);

    HWND tray = FindWindowExW(g_taskbar, nullptr, L"TrayNotifyWnd", nullptr);
    int right = taskbarRc.right;
    if (tray)
    {
        RECT trayRc{};
        GetWindowRect(tray, &trayRc);
        POINT p{trayRc.left, trayRc.top};
        ScreenToClient(g_taskbar, &p);
        right = p.x;
    }

    int h = taskbarRc.bottom - taskbarRc.top;
    int x = right - g_width - 6;
    if (x < 0) x = 0;

    SetWindowPos(g_slider, HWND_TOP, x, 0, g_width, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static LRESULT CALLBACK SliderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool dragging = false;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
        dragging = true;
        SetCapture(hwnd);
        {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            int track = std::max(1, (int)(rc.right - (g_showPercent ? 45 : 20)));
            int x = GET_X_LPARAM(lParam);
            float v = (float)(x - 10) / track;
            SetVolume(v);
            g_currentVolume = v;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:
        if (dragging && (wParam & MK_LBUTTON))
        {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            int track = std::max(1, (int)(rc.right - (g_showPercent ? 45 : 20)));
            int x = GET_X_LPARAM(lParam);
            float v = (float)(x - 10) / track;
            SetVolume(v);
            g_currentVolume = v;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_LBUTTONUP:
        if (dragging)
        {
            dragging = false;
            ReleaseCapture();
        }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc{};
        GetClientRect(hwnd, &rc);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        if (FAILED(DrawThemeParentBackground(hwnd, memDC, &rc)))
        {
            HBRUSH bg = CreateSolidBrush(RGB(16, 16, 16));
            FillRect(memDC, &rc, bg);
            DeleteObject(bg);
        }

        int trackR = rc.right - (g_showPercent ? 35 : 10);
        int pos = 10 + (int)((trackR - 10) * g_currentVolume);
        int cy = rc.bottom / 2;

        HPEN pen = CreatePen(PS_SOLID, 2, RGB(160, 160, 160));
        HPEN oldPen = (HPEN)SelectObject(memDC, pen);
        MoveToEx(memDC, 10, cy, nullptr);
        LineTo(memDC, trackR, cy);
        DeleteObject(SelectObject(memDC, oldPen));

        HBRUSH fill = CreateSolidBrush(RGB(0, 120, 215));
        RECT fillRc{10, cy - 1, pos, cy + 2};
        FillRect(memDC, &fillRc, fill);

        Ellipse(memDC, pos - 5, cy - 5, pos + 5, cy + 5);
        DeleteObject(fill);

        if (g_showPercent)
        {
            wchar_t text[16];
            wsprintfW(text, L"%d", (int)(g_currentVolume * 100.0f + 0.5f));
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(235, 235, 235));
            RECT textRc{rc.right - 32, 0, rc.right, rc.bottom};
            DrawTextW(memDC, text, -1, &textRc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
        }

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (!dragging)
        {
            float v = GetVolume();
            if (v != g_currentVolume)
            {
                g_currentVolume = v;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        PositionSlider();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI SliderThreadProc(LPVOID)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    for (int i = 0; i < 20; ++i)
    {
        g_taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (g_taskbar) break;
        Sleep(250);
    }

    if (!g_taskbar)
    {
        CoUninitialize();
        return 0;
    }

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    UnregisterClassW(L"WindhawkTaskbarSliderClass", hInst);

    WNDCLASSW wc{};
    wc.lpfnWndProc = SliderProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindhawkTaskbarSliderClass";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    g_slider = CreateWindowExW(
        WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"VolumeSlider",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0, g_width, 30,
        g_taskbar, nullptr, hInst, nullptr);

    if (g_slider)
    {
        InitAudioEndpoint();
        g_currentVolume = GetVolume();
        SetTimer(g_slider, 1, 1000, nullptr);
        PositionSlider();

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UninitAudioEndpoint();
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit()
{
    g_width = Wh_GetIntValue(L"width", 200);
    g_showPercent = Wh_GetIntValue(L"showPercent", 1) != 0;

    g_hThread = CreateThread(nullptr, 0, SliderThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit()
{
    if (g_slider && IsWindow(g_slider))
    {
        SendMessageW(g_slider, WM_CLOSE, 0, 0);
    }
    if (g_hThread)
    {
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    UnregisterClassW(L"WindhawkTaskbarSliderClass", hInst);
}
