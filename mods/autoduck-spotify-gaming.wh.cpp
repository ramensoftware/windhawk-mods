// ==WindhawkMod==
// @id              autoduck-spotify-gaming
// @name            AutoDuck: Spotify Gaming Mode
// @description     Smoothly lowers Spotify volume in games with an auto-hiding ultra-minimal game overlay
// @version         1.0
// @author          F3RXY
// @github          https://github.com/F3RXY
// @include         windhawk.exe
// @compilerOptions -lole32 -ladvapi32 -lshell32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AutoDuck: Spotify Gaming Mode
Automatically and smoothly lowers (ducks) Spotify volume in the Windows Volume Mixer during gameplay, and automatically displays an ultra-compact floating media overlay over active games in your chosen screen corner.

### Features:
* **Tool-Mode Architecture:** Runs safely as a Windhawk tool process, preventing any desktop/explorer instability.
* **Auto-Show in Games Only:** Hidden on the desktop; automatically pops up over fullscreen/borderless games on the correct monitor.
* **Dynamic Play/Pause Button:** Automatically switches between `II` and `>` based on Spotify's playback state.
* **High-DPI & Multi-Monitor Support:** Fully scales with display DPI and tracks the active game monitor.
* **Persistent Volume Recovery:** Safely remembers and restores original Spotify volume even after crashes or unexpected terminations.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetVolume: 30
  $name: Target Spotify Volume in Game (%)
  $description: The volume level (0-100) to lower Spotify to while a game is running.
- fadeDuration: 1500
  $name: Smooth Fade Duration (ms)
  $description: Duration of the smooth fade-out and fade-in transition in milliseconds.
- showGui: true
  $name: Enable Overlay
  $description: Turn the floating game overlay ON or OFF.
- guiCorner: bottom_left
  $name: GUI Screen Position (Corner)
  $description: Select which corner of the screen the overlay should stick to during gameplay.
  $options:
  - top_left: Top-Left
  - top_right: Top-Right
  - bottom_left: Bottom-Left
  - bottom_right: Bottom-Right
  $options:pl-PL
  - top_left: Lewy górny róg
  - top_right: Prawy górny róg
  - bottom_left: Lewy dolny róg
  - bottom_right: Prawy dolny róg
- guiMargin: 16
  $name: Corner Margin (px)
  $description: Distance from the screen edges in pixels.
- toggleHotkey: "Alt+M"
  $name: Overlay Toggle Hotkey
  $description: Global shortcut to toggle the overlay visibility (e.g. Alt+M, leave empty to disable).
- detectSteam: true
  $name: Detect Steam Games
  $description: Automatically check the Steam RunningAppID registry flag.
- detectFullscreen: true
  $name: Detect Fullscreen 3D Games
  $description: Automatically detect non-Steam games running in Direct3D fullscreen mode.
- customProcesses: "Cyberpunk2077.exe;FarCry6.exe;witcher3.exe"
  $name: Custom Game Processes / Folders (semicolon-separated)
  $description: Executable names or folder path fragments. Case-insensitive.
- checkInterval: 1500
  $name: Check Interval (ms)
  $description: How often to check if a game is running (in milliseconds).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <sddl.h>
#include <optional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <cwctype>
#include <cmath>
#include <algorithm>

#define WM_UPDATE_GUI_STATE (WM_USER + 101)
#define HOTKEY_TOGGLE_GUI   1

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
    std::wstring toggleHotkey;
    bool detectSteam;
    bool detectFullscreen;
    std::vector<std::wstring> customFilters;
    int checkInterval;
} g_settings;

[[clang::no_destroy]] std::optional<std::thread> g_workerThread;
[[clang::no_destroy]] std::optional<std::thread> g_guiThread;

std::atomic<bool> g_stopThread{false};
std::atomic<bool> g_isGameActive{false};
std::atomic<bool> g_settingsChanged{false};
HANDLE g_wakeEvent = NULL;
HANDLE g_guiReadyEvent = NULL;

HWND g_hGuiWnd = NULL;
bool g_guiManualOverride = false;
bool g_guiVisibleState = true;
int g_hoveredBtn = 0;
std::wstring g_currentTrack = L"Spotify";
int g_displayedVol = 100;
bool g_isPlaying = true;

bool g_isDucked = false;
std::wstring g_userSidStr;

std::wstring ToLower(std::wstring str) {
    std::transform(str.begin(), str.end(), str.begin(), ::towlower);
    return str;
}

std::wstring GetUserSidString() {
    if (!g_userSidStr.empty()) return g_userSidStr;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        DWORD cbTokenInfo = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &cbTokenInfo);
        if (cbTokenInfo > 0) {
            PTOKEN_USER pTokenUser = (PTOKEN_USER)LocalAlloc(LPTR, cbTokenInfo);
            if (pTokenUser && GetTokenInformation(hToken, TokenUser, pTokenUser, cbTokenInfo, &cbTokenInfo)) {
                LPWSTR stringSid = NULL;
                if (ConvertSidToStringSidW(pTokenUser->User.Sid, &stringSid)) {
                    g_userSidStr = stringSid;
                    LocalFree(stringSid);
                }
            }
            if (pTokenUser) LocalFree(pTokenUser);
        }
        CloseHandle(hToken);
    }
    if (g_userSidStr.empty()) g_userSidStr = L"DefaultUser";
    return g_userSidStr;
}

void SaveSavedVolume(float vol) {
    std::wstring valName = L"savedVol_" + GetUserSidString();
    Wh_SetIntSetting(valName.c_str(), (int)std::round(vol * 100.0f));
}

float GetSavedVolume() {
    std::wstring valName = L"savedVol_" + GetUserSidString();
    int v = Wh_GetIntSetting(valName.c_str());
    if (v <= 0 || v > 100) return 1.0f;
    return (float)v / 100.0f;
}

void ClearSavedVolume() {
    std::wstring valName = L"savedVol_" + GetUserSidString();
    Wh_SetIntSetting(valName.c_str(), 0);
}

void ParseCustomProcesses(const std::wstring& rawStr) {
    g_settings.customFilters.clear();
    if (rawStr.empty()) return;
    size_t start = 0;
    size_t end = rawStr.find(L';');
    while (end != std::wstring::npos) {
        std::wstring token = rawStr.substr(start, end - start);
        if (!token.empty()) g_settings.customFilters.push_back(ToLower(token));
        start = end + 1;
        end = rawStr.find(L';', start);
    }
    std::wstring lastToken = rawStr.substr(start);
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
                    if (t == L"Spotify") {
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

            if (filter.find(L'\\') != std::wstring::npos) {
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

void GetCornerPosition(HWND hwnd, int* outX, int* outY, int guiW, int guiH) {
    HWND hForeground = GetForegroundWindow();
    HMONITOR hMonitor = MonitorFromWindow(hForeground ? hForeground : GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(hMonitor, &mi);

    int m = g_settings.guiMargin;
    int screenW = mi.rcMonitor.right - mi.rcMonitor.left;
    int screenH = mi.rcMonitor.bottom - mi.rcMonitor.top;

    switch (g_settings.guiCorner) {
        case CORNER_TOP_RIGHT:
            *outX = mi.rcMonitor.left + screenW - guiW - m;
            *outY = mi.rcMonitor.top + m;
            break;
        case CORNER_BOTTOM_LEFT:
            *outX = mi.rcMonitor.left + m;
            *outY = mi.rcMonitor.top + screenH - guiH - m;
            break;
        case CORNER_BOTTOM_RIGHT:
            *outX = mi.rcMonitor.left + screenW - guiW - m;
            *outY = mi.rcMonitor.top + screenH - guiH - m;
            break;
        case CORNER_TOP_LEFT:
        default:
            *outX = mi.rcMonitor.left + m;
            *outY = mi.rcMonitor.top + m;
            break;
    }
}

RECT GetBtnRect(int btnId, int guiH) {
    int btnW = MulDiv(22, GetDpiForWindow(g_hGuiWnd), 96);
    int btnH = guiH - MulDiv(6, GetDpiForWindow(g_hGuiWnd), 96);
    int spacing = MulDiv(2, GetDpiForWindow(g_hGuiWnd), 96);
    int rightX = MulDiv(210, GetDpiForWindow(g_hGuiWnd), 96) - MulDiv(4, GetDpiForWindow(g_hGuiWnd), 96);

    if (btnId == 3) return {rightX - btnW, MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96), rightX, guiH - MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96)};
    if (btnId == 2) return {rightX - (btnW * 2) - spacing, MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96), rightX - btnW - spacing, guiH - MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96)};
    if (btnId == 1) return {rightX - (btnW * 3) - (spacing * 2), MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96), rightX - (btnW * 2) - (spacing * 2), guiH - MulDiv(3, GetDpiForWindow(g_hGuiWnd), 96)};
    return {0, 0, 0, 0};
}

LRESULT CALLBACK GuiWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int s_dpi = 96;
    switch (msg) {
        case WM_CREATE: {
            s_dpi = GetDpiForWindow(hwnd);
            SetTimer(hwnd, 1, 1000, NULL);
            if (!g_settings.toggleHotkey.empty() && g_settings.toggleHotkey == L"Alt+M") {
                RegisterHotKey(hwnd, HOTKEY_TOGGLE_GUI, MOD_ALT | MOD_NOREPEAT, 'M');
            }
            return 0;
        }

        case WM_DPICHANGED: {
            s_dpi = LOWORD(wParam);
            RECT* prc = (RECT*)lParam;
            SetWindowPos(hwnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, SWP_NOZORDER | SWP_NOACTIVATE);
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
            
            int guiW = MulDiv(210, s_dpi, 96);
            int guiH = MulDiv(24, s_dpi, 96);
            int posX = 16, posY = 16;
            GetCornerPosition(hwnd, &posX, &posY, guiW, guiH);
            SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, guiW, guiH, SWP_NOACTIVATE | SWP_SHOWWINDOW);

            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_UPDATE_GUI_STATE: {
            if (!g_settings.showGui) {
                ShowWindow(hwnd, SW_HIDE);
            } else {
                int guiW = MulDiv(210, s_dpi, 96);
                int guiH = MulDiv(24, s_dpi, 96);
                int posX = 16, posY = 16;
                GetCornerPosition(hwnd, &posX, &posY, guiW, guiH);
                SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, guiW, guiH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
                
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
            int guiH = MulDiv(24, s_dpi, 96);
            int newHover = 0;
            for (int i = 1; i <= 3; i++) {
                RECT r = GetBtnRect(i, guiH);
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
            int guiH = MulDiv(24, s_dpi, 96);
            for (int i = 1; i <= 3; i++) {
                RECT r = GetBtnRect(i, guiH);
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
            int fontTitleSz = MulDiv(12, s_dpi, 96);
            int fontBtnSz = MulDiv(11, s_dpi, 96);

            HFONT hFontTitle = CreateFontW(fontTitleSz, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            HFONT hFontBtns = CreateFontW(fontBtnSz, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

            SelectObject(memDC, hFontTitle);
            SetTextColor(memDC, RGB(235, 235, 240));
            int titleRightLimit = rc.right - MulDiv(70, s_dpi, 96);
            RECT titleRect = {MulDiv(9, s_dpi, 96), 0, titleRightLimit, rc.bottom};
            DrawTextW(memDC, g_currentTrack.c_str(), -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

            SelectObject(memDC, hFontBtns);
            const WCHAR* playPauseText = g_isPlaying ? L"II" : L">";
            const WCHAR* btnLabels[3] = {L"<<", playPauseText, L">>"};
            for (int i = 1; i <= 3; i++) {
                RECT br = GetBtnRect(i, rc.bottom);
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

    int guiW = MulDiv(210, 96, 96);
    int guiH = MulDiv(24, 96, 96);
    int posX = 16, posY = 16;
    
    HWND dummyHwnd = GetDesktopWindow();
    HMONITOR hMon = MonitorFromWindow(dummyHwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(hMon, &mi);
    posX = mi.rcMonitor.left + 16;
    posY = mi.rcMonitor.top + 16;

    g_hGuiWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc.lpszClassName,
        L"AutoDuck Spotify Mini-OSD",
        WS_POPUP,
        posX, posY, guiW, guiH,
        NULL, NULL, wc.hInstance, NULL
    );

    if (g_hGuiWnd) {
        HRGN hRgn = CreateRoundRectRgn(0, 0, guiW + 1, guiH + 1, 8, 8);
        SetWindowRgn(g_hGuiWnd, hRgn, TRUE);
        SetLayeredWindowAttributes(g_hGuiWnd, 0, 230, LWA_ALPHA);
        ShowWindow(g_hGuiWnd, SW_HIDE);
    }

    SetEvent(g_guiReadyEvent);

    if (g_hGuiWnd) {
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

    PCWSTR hotkeyStr = Wh_GetStringSetting(L"toggleHotkey");
    g_settings.toggleHotkey = hotkeyStr ? hotkeyStr : L"";
    Wh_FreeStringSetting(hotkeyStr);

    g_settings.detectSteam = Wh_GetIntSetting(L"detectSteam") != 0;
    g_settings.detectFullscreen = Wh_GetIntSetting(L"detectFullscreen") != 0;

    PCWSTR customStr = Wh_GetStringSetting(L"customProcesses");
    ParseCustomProcesses(customStr ? customStr : L"");
    Wh_FreeStringSetting(customStr);

    g_settings.checkInterval = Wh_GetIntSetting(L"checkInterval");
    if (g_settings.checkInterval < 500) g_settings.checkInterval = 500;
}

void WorkerThreadProc() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    while (!g_stopThread.load()) {
        if (g_settingsChanged.exchange(false)) {
            LoadSettings();
        }

        bool gameRunning = IsAnyGameRunning();
        g_isGameActive.store(gameRunning);

        if (g_hGuiWnd) {
            PostMessageW(g_hGuiWnd, WM_UPDATE_GUI_STATE, 0, 0);
        }

        if (gameRunning && !g_isDucked) {
            float currentVol = 1.0f;
            if (AccessSpotifyVolume(false, 0.0f, &currentVol)) {
                SaveSavedVolume(currentVol);
                float targetVol = g_settings.targetVolume / 100.0f;
                g_isDucked = true;
                SmoothFadeSpotify(currentVol, targetVol, g_settings.fadeDuration);
            }
        } else if (!gameRunning && g_isDucked) {
            float savedVol = GetSavedVolume();
            float currentVol = g_settings.targetVolume / 100.0f;
            AccessSpotifyVolume(false, 0.0f, &currentVol);
            
            SmoothFadeSpotify(currentVol, savedVol, g_settings.fadeDuration);
            
            float verifyVol = 0.0f;
            if (AccessSpotifyVolume(false, 0.0f, &verifyVol) && std::abs(verifyVol - savedVol) < 0.05f) {
                ClearSavedVolume();
                g_isDucked = false;
            }
        } else if (!gameRunning && !g_isDucked) {
            float orphanVol = 0.0f;
            float savedVol = GetSavedVolume();
            if (savedVol < 0.99f && AccessSpotifyVolume(false, 0.0f, &orphanVol) && std::abs(orphanVol - (g_settings.targetVolume / 100.0f)) < 0.05f) {
                SmoothFadeSpotify(orphanVol, savedVol, g_settings.fadeDuration);
                ClearSavedVolume();
            }
        }

        WaitForSingleObject(g_wakeEvent, g_settings.checkInterval);
    }

    if (g_isDucked) {
        float savedVol = GetSavedVolume();
        AccessSpotifyVolume(true, savedVol, NULL);
        float verifyVol = 0.0f;
        if (AccessSpotifyVolume(false, 0.0f, &verifyVol) && std::abs(verifyVol - savedVol) < 0.05f) {
            ClearSavedVolume();
        }
    }

    CoUninitialize();
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopThread = false;
    g_wakeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    g_guiReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    g_guiThread.emplace(GuiThreadProc);
    WaitForSingleObject(g_guiReadyEvent, 3000);

    g_workerThread.emplace(WorkerThreadProc);
    return TRUE;
}

void WhTool_ModUninit() {
    g_stopThread = true;
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
    if (g_hGuiWnd) {
        PostMessageW(g_hGuiWnd, WM_CLOSE, 0, 0);
    }

    if (g_workerThread.has_value()) {
        if (g_workerThread->joinable()) {
            g_workerThread->join();
        }
        g_workerThread.reset();
    }

    if (g_guiThread.has_value()) {
        if (g_guiThread->joinable()) {
            g_guiThread->join();
        }
        g_guiThread.reset();
    }

    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = NULL;
    }
    if (g_guiReadyEvent) {
        CloseHandle(g_guiReadyEvent);
        g_guiReadyEvent = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    g_settingsChanged.store(true);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

// ==WindhawkModToolLauncher==
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    if (!WhTool_ModInit()) {
        return 1;
    }

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    WhTool_ModUninit();
    return 0;
}
// ==/WindhawkModToolLauncher==
