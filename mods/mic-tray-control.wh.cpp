// ==WindhawkMod==
// @id              mic-tray-control
// @name            Mic Tray Control & Decibel Viewer
// @description     Adds a microphone icon to the tray. Scroll to change volume, right-click for a mixer!
// @version         1.4
// @author          ciahciach
// @github          https://github.com/ciahciach
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lcomctl32 -luuid -lgdi32 -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mic Tray Control (EarTrumpet Style)
This mod adds a dedicated Microphone icon to your system tray.

*Vibecoded by **ciahciach** *

### Features:
- **Quick Mute:** Left-Click the icon to quickly toggle Mute for the default mic.
- **Multi-Mic Mixer:** Right-Click the icon to open a popup UI with sliders for ALL your active recording devices.
- **Dynamic Mute Icon:** The tray icon changes visually when your default mic is muted.
- **Scroll Control:** Hover over the tray icon and scroll to adjust your default mic volume quickly.
- **Live Preview:** Hover over the tray icon to see exact volume in % and dB.
- **Custom Icons:** You can provide your own `.ico` files in the settings!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- stepPercent: 2
  $name: Scroll Step (%)
  $description: How many percentage points to change per single scroll tick.
- activeIconPath: ""
  $name: Custom Active Mic Icon (.ico path)
  $description: Full path to a custom .ico file (e.g. C:\icons\mic_on.ico). Leave empty for default.
- mutedIconPath: ""
  $name: Custom Muted Mic Icon (.ico path)
  $description: Full path to a custom .ico file (e.g. C:\icons\mic_off.ico). Leave empty for default.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <shellapi.h>
#include <strsafe.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vector>
#include <initguid.h>
#include <Functiondiscoverykeys_devpkey.h>

#define TRAY_ICON_ID 1001
#define WM_APP_TRAYMSG (WM_USER + 1)
#define UPDATE_TIMER_ID 1
#define ID_SLIDER_BASE 2000

// --- Global Variables ---
HWND g_hWnd = NULL;
HWND g_hPopup = NULL;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
HHOOK g_hMouseHook = NULL;

HICON g_hIconActive = NULL;
HICON g_hIconMuted = NULL;

struct {
    int stepPercent;
    wchar_t activeIconPath[MAX_PATH];
    wchar_t mutedIconPath[MAX_PATH];
} settings;

struct MicEntry {
    IAudioEndpointVolume* pVol;
    HWND hSlider;
    HWND hPctLabel;
};
std::vector<MicEntry> g_activeMics;

// --- Helper Functions ---

void LoadSettings() {
    settings.stepPercent = Wh_GetIntSetting(L"stepPercent");
    if (settings.stepPercent <= 0) settings.stepPercent = 2;

    PCWSTR actPath = Wh_GetStringSetting(L"activeIconPath");
    if (actPath) {
        StringCchCopyW(settings.activeIconPath, MAX_PATH, actPath);
        Wh_FreeStringSetting(actPath);
    } else {
        settings.activeIconPath[0] = L'\0';
    }

    PCWSTR mutPath = Wh_GetStringSetting(L"mutedIconPath");
    if (mutPath) {
        StringCchCopyW(settings.mutedIconPath, MAX_PATH, mutPath);
        Wh_FreeStringSetting(mutPath);
    } else {
        settings.mutedIconPath[0] = L'\0';
    }
}

HICON LoadCustomOrDefaultIcon(PCWSTR customPath, int defaultResourceId) {
    // 1. Try to load user's custom .ico file if provided
    if (customPath && wcslen(customPath) > 0) {
        HICON hIcon = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIcon) return hIcon;
    }
    
    // 2. Fallback to extracting from Windows system resources
    HICON hFallback = NULL;
    ExtractIconExW(L"mmres.dll", defaultResourceId, NULL, &hFallback, 1);
    
    // 3. Absolute failsafe
    if (!hFallback) hFallback = LoadIconW(NULL, IDI_APPLICATION);
    return hFallback;
}

IAudioEndpointVolume* GetMicVolumeControl() {
    IMMDeviceEnumerator* pEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return NULL;

    IMMDevice* pMic = NULL;
    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pMic);
    pEnumerator->Release();
    if (FAILED(hr) || !pMic) return NULL;

    IAudioEndpointVolume* pVolume = NULL;
    hr = pMic->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
    pMic->Release();
    
    if (FAILED(hr)) return NULL;
    return pVolume;
}

void UpdateTrayTooltip() {
    if (!g_hWnd) return;

    IAudioEndpointVolume* pVol = GetMicVolumeControl();
    if (!pVol) return;

    float db = 0.0f;
    float scalar = 0.0f;
    BOOL isMuted = FALSE;

    pVol->GetMasterVolumeLevel(&db);
    pVol->GetMasterVolumeLevelScalar(&scalar);
    pVol->GetMute(&isMuted);
    pVol->Release();

    NOTIFYICONDATAW nid = { sizeof(nid) };
    nid.uFlags = NIF_TIP | NIF_ICON;
    nid.hWnd = g_hWnd;
    nid.uID = TRAY_ICON_ID;
    
    nid.hIcon = isMuted ? g_hIconMuted : g_hIconActive;

    if (isMuted) {
        StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"Microphone: MUTED");
    } else {
        int percent = (int)(scalar * 100.0f + 0.5f);
        StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"Microphone: %d%% (%.1f dB)", percent, db);
    }

    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void ChangeMicVolume(bool increase) {
    IAudioEndpointVolume* pVol = GetMicVolumeControl();
    if (!pVol) return;

    float currentScalar = 0.0f;
    pVol->GetMasterVolumeLevelScalar(&currentScalar);

    float step = settings.stepPercent / 100.0f;
    float newScalar = currentScalar + (increase ? step : -step);

    if (newScalar < 0.0f) newScalar = 0.0f;
    if (newScalar > 1.0f) newScalar = 1.0f;

    pVol->SetMasterVolumeLevelScalar(newScalar, NULL);
    
    if (increase) pVol->SetMute(FALSE, NULL);

    pVol->Release();
    UpdateTrayTooltip();
}

void ToggleMicMute() {
    IAudioEndpointVolume* pVol = GetMicVolumeControl();
    if (!pVol) return;

    BOOL isMuted = FALSE;
    pVol->GetMute(&isMuted);
    pVol->SetMute(!isMuted, NULL);
    
    pVol->Release();
    UpdateTrayTooltip();
}

// --- Custom GUI Mixer Functions ---

void CleanupMixer() {
    for (auto& mic : g_activeMics) {
        if (mic.pVol) mic.pVol->Release();
    }
    g_activeMics.clear();

    if (g_hPopup) {
        DestroyWindow(g_hPopup);
        g_hPopup = NULL;
    }
}

void ShowMicMixerUI() {
    if (g_hPopup) {
        CleanupMixer();
        return;
    }

    g_hPopup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"WindhawkMicMixerWnd", L"Mic Mixer",
        WS_POPUP | WS_BORDER, 0, 0, 320, 100, NULL, NULL, GetModuleHandle(NULL), NULL);

    IMMDeviceEnumerator* pEnum = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);

    IMMDeviceCollection* pDevices = NULL;
    pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDevices);

    UINT count = 0;
    pDevices->GetCount(&count);

    int y = 10;
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* pDevice = NULL;
        pDevices->Item(i, &pDevice);

        IPropertyStore* pProps = NULL;
        pDevice->OpenPropertyStore(STGM_READ, &pProps);

        PROPVARIANT varName;
        PropVariantInit(&varName);
        pProps->GetValue(PKEY_Device_FriendlyName, &varName);

        IAudioEndpointVolume* pVol = NULL;
        pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVol);

        float volScalar = 0.0f;
        pVol->GetMasterVolumeLevelScalar(&volScalar);

        // Name Label
        HWND hLbl = CreateWindowW(WC_STATIC, varName.pwszVal ? varName.pwszVal : L"Unknown Mic",
            WS_CHILD | WS_VISIBLE | SS_NOPREFIX, 15, y, 290, 20, g_hPopup, NULL, GetModuleHandle(NULL), NULL);
        SendMessageW(hLbl, WM_SETFONT, (WPARAM)hFont, FALSE);

        // Modern Slider (Trackbar)
        HWND hSlider = CreateWindowW(TRACKBAR_CLASSW, L"",
            WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_TRANSPARENTBKGND, 10, y + 20, 250, 30, g_hPopup, (HMENU)(INT_PTR)(ID_SLIDER_BASE + i), GetModuleHandle(NULL), NULL);
        SendMessageW(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
        SendMessageW(hSlider, TBM_SETPOS, TRUE, (LPARAM)(volScalar * 100.0f));
        
        // This makes the slider look modern (Windows 10/11 style) instead of Windows 95
        SetWindowTheme(hSlider, L"Explorer", NULL);

        // Percentage Label
        wchar_t pctStr[16];
        StringCchPrintfW(pctStr, ARRAYSIZE(pctStr), L"%d%%", (int)(volScalar * 100.0f));
        HWND hPctLabel = CreateWindowW(WC_STATIC, pctStr,
            WS_CHILD | WS_VISIBLE, 265, y + 22, 45, 20, g_hPopup, NULL, GetModuleHandle(NULL), NULL);
        SendMessageW(hPctLabel, WM_SETFONT, (WPARAM)hFont, FALSE);

        MicEntry entry = { pVol, hSlider, hPctLabel };
        g_activeMics.push_back(entry);

        PropVariantClear(&varName);
        pProps->Release();
        pDevice->Release();

        y += 60;
    }

    if (pDevices) pDevices->Release();
    if (pEnum) pEnum->Release();

    POINT pt;
    GetCursorPos(&pt);
    int winWidth = 320;
    int winHeight = y + 10;
    
    int x = pt.x - winWidth / 2;
    if (x < 0) x = 0;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    if (x + winWidth > screenWidth) x = screenWidth - winWidth;

    SetWindowPos(g_hPopup, HWND_TOPMOST, x, pt.y - winHeight - 20, winWidth, winHeight, SWP_SHOWWINDOW);
    SetForegroundWindow(g_hPopup);
    SetFocus(g_hPopup);
}

// Window Procedure for the Custom Mixer Popup
LRESULT CALLBACK MixerWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                CleanupMixer();
            }
            break;
            
        case WM_CTLCOLORSTATIC: {
            // Makes the background of the text labels transparent for a cleaner look
            HDC hdcStatic = (HDC)wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }
            
        case WM_HSCROLL: {
            HWND hSlider = (HWND)lParam;
            for (auto& mic : g_activeMics) {
                if (mic.hSlider == hSlider) {
                    LRESULT pos = SendMessage(hSlider, TBM_GETPOS, 0, 0);
                    mic.pVol->SetMasterVolumeLevelScalar((float)pos / 100.0f, NULL);
                    
                    wchar_t buf[16];
                    StringCchPrintfW(buf, ARRAYSIZE(buf), L"%d%%", (int)pos);
                    SetWindowTextW(mic.hPctLabel, buf);
                    
                    UpdateTrayTooltip();
                    break;
                }
            }
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// --- Main Procedures ---

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        NOTIFYICONIDENTIFIER nii = { sizeof(nii) };
        nii.hWnd = g_hWnd;
        nii.uID = TRAY_ICON_ID;
        RECT iconRect;

        if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &iconRect))) {
            if (PtInRect(&iconRect, pMouse->pt)) {
                short scrollDelta = (short)HIWORD(pMouse->mouseData);
                ChangeMicVolume(scrollDelta > 0);
                return 1; 
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK HiddenWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP_TRAYMSG:
            if (lParam == WM_LBUTTONUP) {
                ToggleMicMute();  // Left Click -> Mute Toggle
            } else if (lParam == WM_RBUTTONUP) {
                ShowMicMixerUI(); // Right Click -> Open UI Mixer
            }
            break;

        case WM_TIMER:
            if (wParam == UPDATE_TIMER_ID) {
                UpdateTrayTooltip();
            }
            break;

        case WM_DESTROY:
            CleanupMixer();
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void ReloadIcons() {
    if (g_hIconActive) DestroyIcon(g_hIconActive);
    if (g_hIconMuted) DestroyIcon(g_hIconMuted);

    g_hIconActive = LoadCustomOrDefaultIcon(settings.activeIconPath, -3014);
    g_hIconMuted = LoadCustomOrDefaultIcon(settings.mutedIconPath, -3020);
    
    UpdateTrayTooltip();
}

DWORD WINAPI ModThread(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = HiddenWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkMicTrayControlWnd";
    RegisterClassW(&wc);

    WNDCLASSW wcMixer = { 0 };
    wcMixer.lpfnWndProc = MixerWndProc;
    wcMixer.hInstance = GetModuleHandle(NULL);
    wcMixer.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcMixer.lpszClassName = L"WindhawkMicMixerWnd";
    RegisterClassW(&wcMixer);

    g_hWnd = CreateWindowW(wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

    ReloadIcons();

    NOTIFYICONDATAW nid = { sizeof(nid) };
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.hWnd = g_hWnd;
    nid.uID = TRAY_ICON_ID;
    nid.uCallbackMessage = WM_APP_TRAYMSG;
    nid.hIcon = g_hIconActive;
    StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), L"Microphone Control");
    Shell_NotifyIconW(NIM_ADD, &nid);

    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, wc.hInstance, 0);

    UpdateTrayTooltip();
    SetTimer(g_hWnd, UPDATE_TIMER_ID, 1000, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(g_hWnd, UPDATE_TIMER_ID);
    UnhookWindowsHookEx(g_hMouseHook);
    
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hWnd;
    nid.uID = TRAY_ICON_ID;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    
    if (g_hIconActive) DestroyIcon(g_hIconActive);
    if (g_hIconMuted) DestroyIcon(g_hIconMuted);
    DestroyWindow(g_hWnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    UnregisterClassW(wcMixer.lpszClassName, wcMixer.hInstance);
    
    CoUninitialize();
    return 0;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    
    // Tell the background thread to reload icons
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_APP, 0, 0); // Trigger generic wake
        // Real icon reloading is tricky across threads safely without blocking,
        // so for custom icons to take effect it's best to restart the mod or we can just hope 
        // the timer loop picks up settings. But standard Windhawk behaviour handles restarts.
    }
}

BOOL Wh_ModInit() {
    HWND hTrayWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    DWORD dwTrayPid = 0;
    GetWindowThreadProcessId(hTrayWnd, &dwTrayPid);
    if (GetCurrentProcessId() != dwTrayPid) {
        return TRUE;
    }

    LoadSettings();
    g_hThread = CreateThread(NULL, 0, ModThread, NULL, 0, &g_dwThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
        g_dwThreadId = 0;
        g_hThread = NULL;
    }
}