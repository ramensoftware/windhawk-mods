// ==WindhawkMod==
// @id              mic-tray-control
// @name            Mic Tray Control & Decibel Viewer
// @description     Adds a microphone icon to the tray. Scroll to change volume, right-click for a mixer!
// @version         1.11
// @author          ciahciach
// @github          https://github.com/ciahciach
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lcomctl32 -luuid -lgdi32 -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mic Tray Control (EarTrumpet Style)
This mod adds a dedicated Microphone icon to your system tray.

*Vibecoded by **ciahciach**.*

### Features:
- **Quick Mute:** Left-Click the icon to quickly toggle Mute for the default mic.
- **Multi-Mic Mixer:** Right-Click the icon to open a popup UI with sliders for ALL your active recording devices.
- **Dynamic Mute Icon:** The tray icon changes visually when your default mic is muted.
- **Scroll Control:** Hover over the tray icon and scroll to adjust your default mic volume quickly.
- **Live Preview:** Hover over the tray icon to see exact volume in % and dB.
- **Step Modes:** Choose between Percentage (%) or exact Decibel (dB) steps when scrolling.
- **Custom Icons:** You can provide your own `.ico` files in the settings, otherwise it uses a crisp native icon generated on the fly!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useDbStep: false
  $name: Use Decibels (dB) for Scroll Step
  $description: If checked, scrolling changes volume by exact dB. If unchecked, it uses Percentage (%).
- stepPercent: 2
  $name: Scroll Step (%)
  $description: How many percentage points to change per tick (if dB mode is disabled).
- stepDb: "0.5"
  $name: Scroll Step (dB)
  $description: How many decibels to change per tick (if dB mode is enabled). Use dot for decimals.
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
    bool useDbStep;
    int stepPercent;
    float stepDb;
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
    settings.useDbStep = Wh_GetIntSetting(L"useDbStep") != 0;
    
    settings.stepPercent = Wh_GetIntSetting(L"stepPercent");
    if (settings.stepPercent <= 0) settings.stepPercent = 2;

    PCWSTR strDb = Wh_GetStringSetting(L"stepDb");
    settings.stepDb = strDb ? (float)_wtof(strDb) : 0.5f;
    if (strDb) Wh_FreeStringSetting(strDb);

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

HICON GenerateFontIcon(bool isMuted) {
    int renderSize = 32;
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = renderSize;
    bmi.bmiHeader.biHeight = renderSize; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hbmColor = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hbmMask = CreateBitmap(renderSize, renderSize, 1, 1, NULL);

    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmColor);

    memset(pBits, 0, renderSize * renderSize * 4);

    int fontSize = -MulDiv(20, GetDeviceCaps(hdcScreen, LOGPIXELSY), 72); 
    HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                              ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe MDL2 Assets");
    if (!hFont) hFont = CreateFontW(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                                    ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI Symbol");

    HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

    SetTextColor(hdcMem, RGB(255, 255, 255));
    SetBkMode(hdcMem, TRANSPARENT);
    
    wchar_t glyph = 0xE720; 
    RECT rc = {0, 0, renderSize, renderSize};
    DrawTextW(hdcMem, &glyph, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DWORD* pixels = (DWORD*)pBits;
    for (int i = 0; i < renderSize * renderSize; i++) {
        BYTE alpha = GetRValue(pixels[i]); 
        if (alpha > 0) {
            pixels[i] = (alpha << 24) | 0x00FFFFFF; 
        } else {
            pixels[i] = 0;
        }
    }

    if (isMuted) {
        for (int i = 5; i < renderSize - 5; i++) {
            int x = i;
            int y = renderSize - 1 - i; 
            
            for (int dx = -2; dx <= 2; dx++) { 
                int px = x + dx;
                if (px >= 0 && px < renderSize) {
                    pixels[y * renderSize + px] = 0xFFFF3C3C; 
                }
            }
        }
    }

    SelectObject(hdcMem, hOldFont);
    SelectObject(hdcMem, hOldBmp);

    ICONINFO ii = {0};
    ii.fIcon = TRUE;
    ii.hbmMask = hbmMask;
    ii.hbmColor = hbmColor;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hFont);
    DeleteObject(hbmColor);
    DeleteObject(hbmMask);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return hIcon;
}

HICON LoadCustomOrDefaultIcon(PCWSTR customPath, bool isMuted) {
    if (customPath && wcslen(customPath) > 0) {
        HICON hIcon = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIcon) return hIcon;
    }
    
    HICON hIcon = GenerateFontIcon(isMuted);
    if (!hIcon) hIcon = LoadIconW(NULL, IDI_APPLICATION); // Ultimate Failsafe
    return hIcon;
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

    if (settings.useDbStep) {
        float currentDb = 0.0f;
        pVol->GetMasterVolumeLevel(&currentDb);

        float minDb = 0.0f, maxDb = 0.0f, stepHw = 0.0f;
        pVol->GetVolumeRange(&minDb, &maxDb, &stepHw);

        float newDb = currentDb + (increase ? settings.stepDb : -settings.stepDb);
        if (newDb < minDb) newDb = minDb;
        if (newDb > maxDb) newDb = maxDb;

        pVol->SetMasterVolumeLevel(newDb, NULL);
    } else {
        float currentScalar = 0.0f;
        pVol->GetMasterVolumeLevelScalar(&currentScalar);

        float step = settings.stepPercent / 100.0f;
        float newScalar = currentScalar + (increase ? step : -step);

        if (newScalar < 0.0f) newScalar = 0.0f;
        if (newScalar > 1.0f) newScalar = 1.0f;

        pVol->SetMasterVolumeLevelScalar(newScalar, NULL);
    }
    
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

        HWND hLbl = CreateWindowW(WC_STATIC, varName.pwszVal ? varName.pwszVal : L"Unknown Mic",
            WS_CHILD | WS_VISIBLE | SS_NOPREFIX, 15, y, 290, 20, g_hPopup, NULL, GetModuleHandle(NULL), NULL);
        SendMessageW(hLbl, WM_SETFONT, (WPARAM)hFont, FALSE);

        HWND hSlider = CreateWindowW(TRACKBAR_CLASSW, L"",
            WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_TRANSPARENTBKGND, 10, y + 20, 250, 30, g_hPopup, (HMENU)(INT_PTR)(ID_SLIDER_BASE + i), GetModuleHandle(NULL), NULL);
        SendMessageW(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
        SendMessageW(hSlider, TBM_SETPOS, TRUE, (LPARAM)(volScalar * 100.0f));
        SetWindowTheme(hSlider, L"Explorer", NULL);

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

LRESULT CALLBACK MixerWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                CleanupMixer();
            }
            break;
            
        case WM_CTLCOLORSTATIC: {
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
                ToggleMicMute();
            } else if (lParam == WM_RBUTTONUP) {
                ShowMicMixerUI();
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

    g_hIconActive = LoadCustomOrDefaultIcon(settings.activeIconPath, false);
    g_hIconMuted = LoadCustomOrDefaultIcon(settings.mutedIconPath, true);
    
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

    g_hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"WindhawkMicTrayControlWnd", WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    ReloadIcons();

    NOTIFYICONDATAW nid = { sizeof(NOTIFYICONDATAW) };
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
    nid.cbSize = sizeof(NOTIFYICONDATAW);
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

// --- Mod Core Functions ---

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_hThread = CreateThread(NULL, 0, ModThread, NULL, 0, &g_dwThreadId);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
        g_dwThreadId = 0;
        g_hThread = NULL;
    }
}


bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
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

    if (isExcluded) {
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