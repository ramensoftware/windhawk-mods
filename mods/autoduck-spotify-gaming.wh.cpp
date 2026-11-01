// ==WindhawkMod==
// @id              autoduck-spotify-gaming
// @name            AutoDuck: Spotify Gaming Mode
// @description     Smoothly lowers Spotify volume in games with an aggressive always-on-top minimal game overlay
// @version         1.0
// @author          F3RXY
// @github          https://github.com/F3RXY
// @include         explorer.exe
// @compilerOptions -lole32 -ladvapi32 -lshell32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AutoDuck: Spotify Gaming Mode
Automatically and smoothly lowers (ducks) Spotify volume in the Windows Volume Mixer during gameplay, and automatically displays an ultra-compact floating media overlay (`210x24 px`) over active games in your chosen screen corner.

### Key Features:
* **Aggressive Topmost Layering:** Periodically re-applies top-most positioning to fight exclusive fullscreen occlusion.
* **Auto-Show in Games Only:** Hidden on the desktop; automatically pops up over games and hides when you exit.
* **Dynamic Play/Pause Button:** Automatically switches between `II` (pause) and `>` (play) based on Spotify's real playback state.
* **No-Focus Ultra-Minimal GUI:** Compact bar (`<<`, `II/>`, `>>` + track title) that takes up zero screen space and never steals game focus when clicked.
* **4 Screen Corners:** Snap the overlay to Top-Left, Top-Right, Bottom-Left, or Bottom-Right in settings.
* **Mouse Wheel Volume:** Scroll up/down directly over the mini-bar to fine-tune Spotify volume.
* **Anti-Cheat Safe:** Runs exclusively inside `explorer.exe` without injecting into any game processes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetVolume: 30
  $name: Target Spotify Volume in Game (%)
  $description: The volume level (0-100) to lower Spotify to while a game is running.
- fadeDuration: 1500
  $name: Smooth Fade Duration (ms)
  $description: Duration of the smooth fade-out and fade-in transition in milliseconds (set to 0 for instant change).
- showGui: true
  $name: Enable Overlay
  $description: Turn the floating game overlay ON or OFF (can also be toggled anytime with Alt + M).
- guiCorner: bottom_left
  $name: GUI Screen Position (Corner)
  $description: Select which corner of the screen the overlay should stick to during gameplay.
  $options:
  - top_left: Top-Left (Lewy górny róg)
  - top_right: Top-Right (Prawy górny róg)
  - bottom_left: Bottom-Left (Lewy dolny róg)
  - bottom_right: Bottom-Right (Prawy dolny róg)
- guiMargin: 16
  $name: Corner Margin (px)
  $description: Distance from the screen edges in pixels.
- detectSteam: true
  $name: Detect Steam Games
  $description: Automatically check the Steam RunningAppID registry flag.
- detectFullscreen: true
  $name: Detect Fullscreen 3D Games
  $description: Automatically detect non-Steam games running in Direct3D fullscreen mode.
- customProcesses: "Cyberpunk2077.exe;FarCry6.exe;witcher3.exe"
  $name: Custom Game Processes / Folders (semicolon-separated)
  $description: Executable names or folder path fragments (e.g., "witcher3.exe;D:\Games\"). Case-insensitive.
- checkInterval: 1500
  $name: Check Interval (ms)
  $description: How often to check if a game is running (in milliseconds).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <psapi.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <cwctype>
#include <cmath>
#include <algorithm>

#define WM_UPDATE_GUI_STATE (WM_USER + 101)
#define HOTKEY_TOGGLE_GUI   1

#define GUI_WIDTH  210
#define GUI_HEIGHT 24

enum CornerPos {
    CORNER_TOP_LEFT = 0,
    CORNER_TOP_RIGHT,
    CORNER_BOTTOM_LEFT,
    CORNER_BOTTOM_RIGHT
};

struct {
    int targetVolume;
    int fadeDuration;
    bool showGui;
    CornerPos guiCorner;
    int guiMargin;
    bool detectSteam;
    bool detectFullscreen;
    std::vector<std::wstring> customFilters;
    int checkInterval;
} g_settings;

std::atomic<bool> g_stopThread{false};
std::atomic<bool> g_isGameActive{false};
HANDLE g_wakeEvent = NULL;
std::thread g_workerThread;
std::thread g_guiThread;

HWND g_hGuiWnd = NULL;
bool g_guiManualOverride = false;
bool g_guiVisibleState = true;
int g_hoveredBtn = 0; // 0 = none, 1 = <<, 2 = II/>, 3 = >>
std::wstring g_currentTrack = L"Spotify";
int g_displayedVol = 100;
bool g_isPlaying = true;

bool g_isDucked = false;
float g_savedVolume = 1.0f;

std::wstring ToLower(std::wstring str) {
    std::transform(str.begin(), str.end(), str.begin(), ::towlower);
    return str;
}

void ParseCustomProcesses(PCWSTR rawStr) {
    g_settings.customFilters.clear();
    if (!rawStr) return;
    std::wstring input = rawStr;
    size_t start = 0;
    size_t end = input.find(L';');
    while (end != std::wstring::npos) {
        std::wstring token = input.substr(start, end - start);
        if (!token.empty()) g_settings.customFilters.push_back(ToLower(token));
        start = end + 1;
        end = input.find(L';', start);
    }
    std::wstring lastToken = input.substr(start);
    if (!lastToken.empty()) g_settings.customFilters.push_back(ToLower(lastToken));
}

bool IsSpotifyProcess(DWORD pid) {
    if (pid == 0) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;

    WCHAR path[MAX_PATH];
    DWORD size = MAX_PATH;
    bool isSpotify = false;

    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        const WCHAR* fileName = wcsrchr(path, L'\\');
        fileName = fileName ? (fileName + 1) : path;
        if (_wcsicmp(fileName, L"Spotify.exe") == 0) {
            isSpotify = true;
        }
    }
    CloseHandle(hProc);
    return isSpotify;
}

std::wstring GetSpotifyTrackTitle() {
    std::wstring title = L"Spotify";
    HWND hwnd = NULL;
    while ((hwnd = FindWindowExW(NULL, hwnd, L"Chrome_WidgetWin_1", NULL)) != NULL) {
        if (!IsWindowVisible(hwnd)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (IsSpotifyProcess(pid)) {
            WCHAR buf[256];
            if (GetWindowTextW(hwnd, buf, 256) > 0) {
                std::wstring t(buf);
                if (!t.empty() && t != L"GDI+ Window") {
                    title = t;
                    if (t.find(L" - ") == std::wstring::npos && t == L"Spotify") {
                        g_isPlaying = false;
                    } else {
                        g_isPlaying = true;
                    }
                    break;
                }
            }
        }
    }
    return title;
}

void SendMediaAction(WORD vkKey) {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vkKey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vkKey;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));

    if (vkKey == VK_MEDIA_PLAY_PAUSE) {
        g_isPlaying = !g_isPlaying;
    }
}

bool AccessSpotifyVolume(bool setVolume, float targetVal, float* outCurrentVal) {
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioSessionManager2* pSessionManager = NULL;
    IAudioSessionEnumerator* pSessionEnumerator = NULL;
    bool found = false;

    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator))) {
        return false;
    }

    if (SUCCEEDED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice))) {
        if (SUCCEEDED(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                        NULL, (void**)&pSessionManager))) {
            if (SUCCEEDED(pSessionManager->GetSessionEnumerator(&pSessionEnumerator))) {
                int sessionCount = 0;
                pSessionEnumerator->GetCount(&sessionCount);

                for (int i = 0; i < sessionCount; i++) {
                    IAudioSessionControl* pSessionControl = NULL;
                    if (SUCCEEDED(pSessionEnumerator->GetSession(i, &pSessionControl))) {
                        IAudioSessionControl2* pSessionControl2 = NULL;
                        if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2))) {
                            DWORD pid = 0;
                            if (SUCCEEDED(pSessionControl2->GetProcessId(&pid)) && IsSpotifyProcess(pid)) {
                                ISimpleAudioVolume* pSimpleVolume = NULL;
                                if (SUCCEEDED(pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleVolume))) {
                                    if (outCurrentVal && !found) {
                                        pSimpleVolume->GetMasterVolume(outCurrentVal);
                                    }
                                    if (setVolume) {
                                        pSimpleVolume->SetMasterVolume(targetVal, NULL);
                                    }
                                    found = true;
                                    pSimpleVolume->Release();
                                }
                            }
                            pSessionControl2->Release();
                        }
                        pSessionControl->Release();
                    }
                }
                pSessionEnumerator->Release();
            }
            pSessionManager->Release();
        }
        pDevice->Release();
    }
    pEnumerator->Release();
    return found;
}

bool IsSteamGameRunning() {
    DWORD appId = 0;
    DWORD dataSize = sizeof(appId);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Valve\\Steam",
        L"RunningAppID",
        RRF_RT_REG_DWORD,
        NULL,
        &appId,
        &dataSize
    );
    return (status == ERROR_SUCCESS && appId != 0);
}

bool IsFullscreen3DGameRunning() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN) {
            return true;
        }
    }
    return false;
}

bool IsCustomGameRunning() {
    if (g_settings.customFilters.empty()) return false;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    bool found = false;

    if (Process32FirstW(hSnap, &pe32)) {
        do {
            if (pe32.th32ProcessID <= 4) continue;

            std::wstring exeName = ToLower(pe32.szExeFile);
            for (const auto& filter : g_settings.customFilters) {
                if (exeName == filter) {
                    found = true;
                    break;
                }
            }
            if (found) break;

            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProc) {
                WCHAR path[MAX_PATH];
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
                    std::wstring fullPath = ToLower(path);
                    for (const auto& filter : g_settings.customFilters) {
                        if (fullPath.find(filter) != std::wstring::npos) {
                            found = true;
                            break;
                        }
                    }
                }
                CloseHandle(hProc);
            }
        } while (!found && Process32NextW(hSnap, &pe32));
    }

    CloseHandle(hSnap);
    return found;
}

bool IsAnyGameRunning() {
    if (g_settings.detectSteam && IsSteamGameRunning()) return true;
    if (g_settings.detectFullscreen && IsFullscreen3DGameRunning()) return true;
    if (IsCustomGameRunning()) return true;
    return false;
}

void SmoothFadeSpotify(float startVol, float endVol, int durationMs) {
    if (durationMs <= 0 || std::abs(endVol - startVol) < 0.01f) {
        AccessSpotifyVolume(true, endVol, NULL);
        g_displayedVol = (int)std::round(endVol * 100.0f);
        if (g_hGuiWnd) InvalidateRect(g_hGuiWnd, NULL, FALSE);
        return;
    }

    const int stepTimeMs = 25;
    int steps = durationMs / stepTimeMs;
    if (steps < 1) steps = 1;

    for (int i = 1; i <= steps; i++) {
        if (g_stopThread.load()) break;
        float t = (float)i / (float)steps;
        float currentVol = startVol + (endVol - startVol) * t;
        AccessSpotifyVolume(true, currentVol, NULL);
        g_displayedVol = (int)std::round(currentVol * 100.0f);
        if (g_hGuiWnd) InvalidateRect(g_hGuiWnd, NULL, FALSE);
        Sleep(stepTimeMs);
    }
    AccessSpotifyVolume(true, endVol, NULL);
    g_displayedVol = (int)std::round(endVol * 100.0f);
    if (g_hGuiWnd) InvalidateRect(g_hGuiWnd, NULL, FALSE);
}

void GetCornerPosition(int* outX, int* outY) {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    int m = g_settings.guiMargin;
    switch (g_settings.guiCorner) {
        case CORNER_TOP_RIGHT:
            *outX = screenW - GUI_WIDTH - m;
            *outY = m;
            break;
        case CORNER_BOTTOM_LEFT:
            *outX = m;
            *outY = screenH - GUI_HEIGHT - m;
            break;
        case CORNER_BOTTOM_RIGHT:
            *outX = screenW - GUI_WIDTH - m;
            *outY = screenH - GUI_HEIGHT - m;
            break;
        case CORNER_TOP_LEFT:
        default:
            *outX = m;
            *outY = m;
            break;
    }
}

RECT GetBtnRect(int btnId) {
    if (btnId == 1) return {136, 3, 158, 21}; // <<
    if (btnId == 2) return {160, 3, 182, 21}; // II / >
    if (btnId == 3) return {184, 3, 206, 21}; // >>
    return {0, 0, 0, 0};
}

LRESULT CALLBACK GuiWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 1000, NULL);
            RegisterHotKey(hwnd, HOTKEY_TOGGLE_GUI, MOD_ALT | MOD_NOREPEAT, 'M');
            return 0;
        }

        case WM_HOTKEY: {
            if (wParam == HOTKEY_TOGGLE_GUI && g_settings.showGui) {
                g_guiManualOverride = true;
                g_guiVisibleState = !IsWindowVisible(hwnd);
                ShowWindow(hwnd, g_guiVisibleState ? SW_SHOWNOACTIVATE : SW_HIDE);
            }
            return 0;
        }

        case WM_TIMER: {
            if (!IsWindowVisible(hwnd)) return 0;
            g_currentTrack = GetSpotifyTrackTitle();
            
            // Agresywne wymuszanie pozycji TOPMOST co sekundę, aby gra nie przykryła nakładki
            int posX = 16, posY = 16;
            GetCornerPosition(&posX, &posY);
            SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, GUI_WIDTH, GUI_HEIGHT, SWP_NOACTIVATE | SWP_SHOWWINDOW);

            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_UPDATE_GUI_STATE: {
            if (!g_settings.showGui) {
                ShowWindow(hwnd, SW_HIDE);
            } else {
                int posX = 16, posY = 16;
                GetCornerPosition(&posX, &posY);
                SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, GUI_WIDTH, GUI_HEIGHT, SWP_NOACTIVATE | SWP_SHOWWINDOW);
                
                bool shouldShow = g_isGameActive.load();
                if (g_guiManualOverride) shouldShow = g_guiVisibleState;

                ShowWindow(hwnd, shouldShow ? SW_SHOWNOACTIVATE : SW_HIDE);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int newHover = 0;
            for (int i = 1; i <= 3; i++) {
                RECT r = GetBtnRect(i);
                if (PtInRect(&r, pt)) {
                    newHover = i;
                    break;
                }
            }
            if (newHover != g_hoveredBtn) {
                g_hoveredBtn = newHover;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (g_hoveredBtn != 0) {
                g_hoveredBtn = 0;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            for (int i = 1; i <= 3; i++) {
                RECT r = GetBtnRect(i);
                if (PtInRect(&r, pt)) {
                    if (i == 1) SendMediaAction(VK_MEDIA_PREV_TRACK);
                    if (i == 2) SendMediaAction(VK_MEDIA_PLAY_PAUSE);
                    if (i == 3) SendMediaAction(VK_MEDIA_NEXT_TRACK);
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }
            ReleaseCapture();
            SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            float curVol = g_displayedVol / 100.0f;
            AccessSpotifyVolume(false, 0.0f, &curVol);
            curVol += (delta > 0) ? 0.05f : -0.05f;
            if (curVol < 0.0f) curVol = 0.0f;
            if (curVol > 1.0f) curVol = 1.0f;
            AccessSpotifyVolume(true, curVol, NULL);
            g_displayedVol = (int)std::round(curVol * 100.0f);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

            HBRUSH bgBrush = CreateSolidBrush(RGB(16, 16, 20));
            FillRect(memDC, &rc, bgBrush);
            DeleteObject(bgBrush);

            HBRUSH borderBrush = CreateSolidBrush(RGB(42, 42, 52));
            FrameRect(memDC, &rc, borderBrush);
            DeleteObject(borderBrush);

            RECT accentRect = {1, 1, 4, rc.bottom - 1};
            HBRUSH accentBrush = CreateSolidBrush(RGB(29, 185, 84));
            FillRect(memDC, &accentRect, accentBrush);
            DeleteObject(accentBrush);

            SetBkMode(memDC, TRANSPARENT);
            HFONT hFontTitle = CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            HFONT hFontBtns = CreateFontW(11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

            SelectObject(memDC, hFontTitle);
            SetTextColor(memDC, RGB(235, 235, 240));
            RECT titleRect = {9, 0, 132, GUI_HEIGHT};
            DrawTextW(memDC, g_currentTrack.c_str(), -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

            SelectObject(memDC, hFontBtns);
            const WCHAR* playPauseText = g_isPlaying ? L"II" : L">";
            const WCHAR* btnLabels[3] = {L"<<", playPauseText, L">>"};
            for (int i = 1; i <= 3; i++) {
                RECT br = GetBtnRect(i);
                HBRUSH btnBrush = CreateSolidBrush(g_hoveredBtn == i ? RGB(29, 185, 84) : RGB(32, 32, 40));
                FillRect(memDC, &br, btnBrush);
                DeleteObject(btnBrush);

                SetTextColor(memDC, g_hoveredBtn == i ? RGB(12, 12, 12) : RGB(225, 225, 235));
                DrawTextW(memDC, btnLabels[i - 1], -1, &br, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            DeleteObject(hFontTitle);
            DeleteObject(hFontBtns);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBmp);
            DeleteObject(memBmp);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY: {
            UnregisterHotKey(hwnd, HOTKEY_TOGGLE_GUI);
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void GuiThreadProc() {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
    wc.lpfnWndProc = GuiWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindhawkAutoDuckSpotifyOSD";
    RegisterClassExW(&wc);

    int posX = 16, posY = 16;
    GetCornerPosition(&posX, &posY);

    g_hGuiWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc.lpszClassName,
        L"AutoDuck Spotify Mini-OSD",
        WS_POPUP,
        posX, posY, GUI_WIDTH, GUI_HEIGHT,
        NULL, NULL, wc.hInstance, NULL
    );

    if (g_hGuiWnd) {
        HRGN hRgn = CreateRoundRectRgn(0, 0, GUI_WIDTH + 1, GUI_HEIGHT + 1, 8, 8);
        SetWindowRgn(g_hGuiWnd, hRgn, TRUE);
        SetLayeredWindowAttributes(g_hGuiWnd, 0, 230, LWA_ALPHA);

        ShowWindow(g_hGuiWnd, SW_HIDE);

        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
}

void LoadSettings() {
    g_settings.targetVolume = Wh_GetIntSetting(L"targetVolume");
    if (g_settings.targetVolume < 0) g_settings.targetVolume = 0;
    if (g_settings.targetVolume > 100) g_settings.targetVolume = 100;

    g_settings.fadeDuration = Wh_GetIntSetting(L"fadeDuration");
    if (g_settings.fadeDuration < 0) g_settings.fadeDuration = 0;
    if (g_settings.fadeDuration > 10000) g_settings.fadeDuration = 10000;

    g_settings.showGui = Wh_GetIntSetting(L"showGui") != 0;

    PCWSTR cornerStr = Wh_GetStringSetting(L"guiCorner");
    if (wcscmp(cornerStr, L"top_right") == 0) {
        g_settings.guiCorner = CORNER_TOP_RIGHT;
    } else if (wcscmp(cornerStr, L"bottom_left") == 0) {
        g_settings.guiCorner = CORNER_BOTTOM_LEFT;
    } else if (wcscmp(cornerStr, L"bottom_right") == 0) {
        g_settings.guiCorner = CORNER_BOTTOM_RIGHT;
    } else {
        g_settings.guiCorner = CORNER_TOP_LEFT;
    }
    Wh_FreeStringSetting(cornerStr);

    g_settings.guiMargin = Wh_GetIntSetting(L"guiMargin");
    if (g_settings.guiMargin < 0) g_settings.guiMargin = 0;

    g_settings.detectSteam = Wh_GetIntSetting(L"detectSteam") != 0;
    g_settings.detectFullscreen = Wh_GetIntSetting(L"detectFullscreen") != 0;

    PCWSTR customStr = Wh_GetStringSetting(L"customProcesses");
    ParseCustomProcesses(customStr);
    Wh_FreeStringSetting(customStr);

    g_settings.checkInterval = Wh_GetIntSetting(L"checkInterval");
    if (g_settings.checkInterval < 500) g_settings.checkInterval = 500;
}

void WorkerThreadProc() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    while (!g_stopThread.load()) {
        bool gameRunning = IsAnyGameRunning();
        g_isGameActive.store(gameRunning);

        if (g_hGuiWnd) {
            PostMessageW(g_hGuiWnd, WM_UPDATE_GUI_STATE, 0, 0);
        }

        if (gameRunning && !g_isDucked) {
            float currentVol = 1.0f;
            if (AccessSpotifyVolume(false, 0.0f, &currentVol)) {
                g_savedVolume = currentVol;
                float targetVol = g_settings.targetVolume / 100.0f;
                g_isDucked = true;
                SmoothFadeSpotify(currentVol, targetVol, g_settings.fadeDuration);
            }
        } else if (!gameRunning && g_isDucked) {
            float currentVol = g_settings.targetVolume / 100.0f;
            AccessSpotifyVolume(false, 0.0f, &currentVol);
            g_isDucked = false;
            SmoothFadeSpotify(currentVol, g_savedVolume, g_settings.fadeDuration);
        }

        WaitForSingleObject(g_wakeEvent, g_settings.checkInterval);
    }

    if (g_isDucked) {
        AccessSpotifyVolume(true, g_savedVolume, NULL);
        g_isDucked = false;
    }

    CoUninitialize();
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopThread = false;
    g_wakeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    g_guiThread = std::thread(GuiThreadProc);
    g_workerThread = std::thread(WorkerThreadProc);
    return TRUE;
}

BOOL Wh_ModUninit() {
    g_stopThread = true;
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
    if (g_hGuiWnd) {
        PostMessageW(g_hGuiWnd, WM_CLOSE, 0, 0);
    }
    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }
    if (g_guiThread.joinable()) {
        g_guiThread.join();
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = NULL;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_guiManualOverride = false;
    if (g_hGuiWnd) {
        PostMessageW(g_hGuiWnd, WM_UPDATE_GUI_STATE, 0, 0);
    }
    if (g_isDucked) {
        float currentVol = 0.3f;
        if (AccessSpotifyVolume(false, 0.0f, &currentVol)) {
            SmoothFadeSpotify(currentVol, g_settings.targetVolume / 100.0f, 400);
        }
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}
