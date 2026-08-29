// ==WindhawkMod==
// @id win10-taskbar-volume-slider
// @name Windows 10 Taskbar Volume Slider
// @description Adds a permanently visible volume slider inside the Windows 10 taskbar.
// @version 1.0.3
// @author didrmt1
// @github https://github.com/didrmt1
// @include explorer.exe
// @compilerOptions -lole32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- width: 200
  $name: Slider width
  $description: Width of the volume slider in pixels.
- showPercent: true
  $name: Show volume percentage
  $description: Display numeric percentage next to the slider.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Windows 10 Taskbar Volume Slider
<img width="548" height="331" alt="20260826_214734" src="https://raw.githubusercontent.com/didrmt1/windhawk-mods/main/mods/screenshot9.png" />
Adds a permanently visible volume slider directly on the Windows 10 taskbar (left side of the system tray).

### Features
- Real-time master volume control via mouse drag directly on the taskbar.
- Displays volume percentage text.
- Synchronizes with theme/accent background of the taskbar.

*Developed with AI assistance (Prompt: Gemini).*
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <algorithm>

static int g_width = 200;
static bool g_showPercent = true;

static HWND g_taskbar = nullptr;
static HWND g_slider = nullptr;
static float g_currentVolume = 0.0f;
static HANDLE g_hThread = nullptr;
static HANDLE g_hStopEvent = nullptr;
static HMODULE g_hModule = nullptr;

static bool IsWindows10()
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi) };
    DWORDLONG const mask = VerSetConditionMask(
        VerSetConditionMask(
            VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL),
            VER_MINORVERSION, VER_EQUAL),
        VER_BUILDNUMBER, VER_LESS);
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    osvi.dwBuildNumber = 22000;
    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, mask) != FALSE;
}

struct EnumData {
    HWND result;
    DWORD pid;
};

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    EnumData* data = reinterpret_cast<EnumData*>(lParam);
    DWORD pid = 0;
    WCHAR cls[32] = {0};
    if (GetWindowThreadProcessId(hWnd, &pid) && pid == data->pid &&
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0)
    {
        data->result = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd()
{
    EnumData data{ nullptr, GetCurrentProcessId() };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.result;
}

static float GetMasterVolume()
{
    float vol = 0.0f;
    IMMDeviceEnumerator* enumerator = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator))))
    {
        IMMDevice* device = nullptr;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
        {
            IAudioEndpointVolume* endpoint = nullptr;
            if (SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&endpoint)))
            {
                endpoint->GetMasterVolumeLevelScalar(&vol);
                endpoint->Release();
            }
            device->Release();
        }
        enumerator->Release();
    }
    return vol;
}

static void SetMasterVolume(float value)
{
    value = std::clamp(value, 0.0f, 1.0f);
    IMMDeviceEnumerator* enumerator = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator))))
    {
        IMMDevice* device = nullptr;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
        {
            IAudioEndpointVolume* endpoint = nullptr;
            if (SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&endpoint)))
            {
                endpoint->SetMasterVolumeLevelScalar(value, nullptr);
                endpoint->Release();
            }
            device->Release();
        }
        enumerator->Release();
    }
}

static void LoadSettings()
{
    g_width = std::clamp(Wh_GetIntSetting(L"width"), 50, 500);
    g_showPercent = Wh_GetIntSetting(L"showPercent") != 0;
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
            v = std::clamp(v, 0.0f, 1.0f);
            SetMasterVolume(v);
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
            v = std::clamp(v, 0.0f, 1.0f);
            SetMasterVolume(v);
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

    case WM_CAPTURECHANGED:
        dragging = false;
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

        HPEN thumbPen = CreatePen(PS_SOLID, 1, RGB(0, 120, 215));
        HPEN oldThumbPen = (HPEN)SelectObject(memDC, thumbPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, fill);
        Ellipse(memDC, pos - 5, cy - 5, pos + 5, cy + 5);
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldThumbPen);
        DeleteObject(thumbPen);
        DeleteObject(fill);

        if (g_showPercent)
        {
            wchar_t text[16];
            swprintf_s(text, L"%d%%", (int)(g_currentVolume * 100.0f + 0.5f));
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(235, 235, 235));
            RECT textRc{rc.right - 35, 0, rc.right, rc.bottom};
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
            float v = GetMasterVolume();
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
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    int retries = 0;
    while (WaitForSingleObject(g_hStopEvent, 250) == WAIT_TIMEOUT)
    {
        g_taskbar = FindCurrentProcessTaskbarWnd();
        if (g_taskbar) break;
        if (++retries > 40)
            break;
    }

    if (!g_taskbar || WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0)
    {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 0;
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = SliderProc;
    wc.hInstance = g_hModule;
    wc.lpszClassName = L"WindhawkTaskbarSliderClass";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    
    if (!RegisterClassW(&wc))
    {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 0;
    }

    g_slider = CreateWindowExW(
        WS_EX_NOACTIVATE,
        L"WindhawkTaskbarSliderClass",
        L"VolumeSlider",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0, g_width, 30,
        g_taskbar, nullptr, g_hModule, nullptr);

    if (g_slider)
    {
        g_currentVolume = GetMasterVolume();
        SetTimer(g_slider, 1, 1000, nullptr);
        PositionSlider();

        MSG msg;
        for (;;)
        {
            DWORD w = MsgWaitForMultipleObjects(1, &g_hStopEvent, FALSE, INFINITE, QS_ALLINPUT);
            if (w == WAIT_OBJECT_0) break;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT) goto done;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

done:
    if (g_slider)
    {
        DestroyWindow(g_slider);
        g_slider = nullptr;
    }

    UnregisterClassW(L"WindhawkTaskbarSliderClass", g_hModule);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

BOOL Wh_ModInit()
{
    if (!IsWindows10())
        return FALSE;

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit), &g_hModule);

    LoadSettings();

    g_hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hThread = CreateThread(nullptr, 0, SliderThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit()
{
    if (g_hStopEvent)
    {
        SetEvent(g_hStopEvent);
    }

    if (g_hThread)
    {
        DWORD r;
        do {
            r = MsgWaitForMultipleObjects(1, &g_hThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (r == WAIT_OBJECT_0 + 1)
            {
                MSG m;
                PeekMessageW(&m, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (r == WAIT_OBJECT_0 + 1);

        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_hStopEvent)
    {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    if (g_slider && IsWindow(g_slider))
    {
        PositionSlider();
        InvalidateRect(g_slider, nullptr, FALSE);
    }
}
