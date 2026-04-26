// ==WindhawkMod==
// @id              osu-tray-profile
// @name            osu!Profile in Taskbar
// @description     Displays PP, rank, and avatar from rhythm game osu! (standard mode) next to the system tray
// @version         3.9.1
// @author          antoshika
// @github          https://github.com/Antoshika
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lwinhttp -lurlmon -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🟣 osu!Profile in Taskbar

Makes your stats always visible.
This mod adds a neat widget directly to the Windows taskbar that shows your current PP, rank, and profile picture automaticaly updating in the background.
![Taskbar](https://i.imgur.com/nrJVcw0.png)

## ⚙️ How to use:
In order for the tweak to collect your statistics, you need to use your API, so you need to create it:
1. Open [OAuth settings of your osu! profile](https://osu.ppy.sh/home/account/edit#oauth).
2. Click **New OAuth Application** - come up with any name **(Example: Taskbar)**, **"Application callback URL"** is optional.
3. Copy the generated `Client ID` & `Client Secret`.
4. Go back to Windhawk, open the **Settings** tab and paste the copied data by cell with your nickname.
_(you can use your old nickname "XATCYHE MIKU, XATCYHE_MIKU, antoshika")_

## ⚠️ Problems:
* **"✎ check 'Settings'"**: You didn't fill in the required fields in the settings.
* **"⛔ error"**: Invalid Client ID or Client Secret. Make sure that you have copied them completely and without spaces at the end.
---
*🥬 Im here: 💙 [hatsunemiku39.ru](http://hatsunemiku39.ru) // 🟣 [osu!profile](https://osu.ppy.sh/users/18815482) // 📶 [Discord](https://discord.gg/3jBQs9buYe)*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- api:
  - client_id: ""
    $name: Client ID
    $description: Enter the ID of the application created in your osu account settings!
  - client_secret: ""
    $name: Client Secret
    $description: Enter your application's secret key
  - username: ""
    $name: Nickname
    $description: Your nickname from osu!
  $name: osu! API
  $description: You need to create an OAuth application in your osu profile settings!
- update:
  - interval: 300
    $name: Update time (in seconds)
    $description: Frequency of statistics updates (recommended 300 sec = 5 min)
  $name: Latency
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <urlmon.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <thread>
#include <string>
#include <atomic>
#include <vector>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "urlmon.lib")

using namespace Gdiplus;

std::wstring g_clientId = L"";
std::wstring g_clientSecret = L"";
std::wstring g_username = L"";
int g_updateInterval = 300;

std::thread g_uiThread;
std::thread g_netThread;
std::atomic<bool> g_running{ false };
std::atomic<bool> g_forceUpdate{ false };
std::atomic<bool> g_needsRedraw{ false };
std::atomic<bool> g_isUpdating{ false };
HWND g_overlayHwnd = NULL;
SRWLOCK g_statsLock = SRWLOCK_INIT;
std::wstring g_displayName = L"Loading...";
std::wstring g_displayStats = L"";
std::wstring g_avatarPath = L"";
int g_consecutiveErrors = 0;

void LoadSettings() {
    PCWSTR clientIdStr = Wh_GetStringSetting(L"api.client_id");
    g_clientId = clientIdStr ? clientIdStr : L"";
    Wh_FreeStringSetting(clientIdStr);

    PCWSTR clientSecretStr = Wh_GetStringSetting(L"api.client_secret");
    g_clientSecret = clientSecretStr ? clientSecretStr : L"";
    Wh_FreeStringSetting(clientSecretStr);

    PCWSTR usernameStr = Wh_GetStringSetting(L"api.username");
    g_username = usernameStr ? usernameStr : L"";
    Wh_FreeStringSetting(usernameStr);

    g_updateInterval = Wh_GetIntSetting(L"update.interval");
    if (g_updateInterval < 5) g_updateInterval = 5;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string FormatWithDots(std::string num) {
    if (num.length() <= 3) return num;
    int insertPosition = num.length() - 3;
    while (insertPosition > 0) {
        num.insert(insertPosition, ".");
        insertPosition -= 3;
    }
    return num;
}

void FetchOsuStats() {
    if (g_clientId.empty() || g_clientSecret.empty() || g_username.empty()) {
        AcquireSRWLockExclusive(&g_statsLock);
        g_consecutiveErrors = 1;
        g_displayName = L"✎ check \"Settings\"";
        g_displayStats = L"";
        g_avatarPath = L"";
        ReleaseSRWLockExclusive(&g_statsLock);
        return;
    }

    HINTERNET hSession = NULL;
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = {0};
    LPCWSTR userAgent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";

    if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig)) {
        if (proxyConfig.lpszProxy) {
            hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NAMED_PROXY, proxyConfig.lpszProxy, proxyConfig.lpszProxyBypass, 0);
        }
        if (proxyConfig.lpszAutoConfigUrl) GlobalFree(proxyConfig.lpszAutoConfigUrl);
        if (proxyConfig.lpszProxy) GlobalFree(proxyConfig.lpszProxy);
        if (proxyConfig.lpszProxyBypass) GlobalFree(proxyConfig.lpszProxyBypass);
    }

    if (!hSession) {
        hSession = WinHttpOpen(userAgent, 4, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    }

    if (!hSession) {
        hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    }

    if (!hSession) return;

    WinHttpSetTimeouts(hSession, 10000, 10000, 10000, 10000);

    DWORD secureProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));

    HINTERNET hConnect = WinHttpConnect(hSession, L"osu.ppy.sh", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return; }

    HINTERNET hRequestAuth = WinHttpOpenRequest(hConnect, L"POST", L"/oauth/token", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    
    std::wstring contentType = L"Content-Type: application/x-www-form-urlencoded\r\n";
    std::wstring postDataW = L"client_id=" + g_clientId + L"&client_secret=" + g_clientSecret + L"&grant_type=client_credentials&scope=public";
    std::string postData = WStringToString(postDataW);

    BOOL bResults = WinHttpSendRequest(hRequestAuth, contentType.c_str(), (DWORD)-1, (LPVOID)postData.c_str(), (DWORD)postData.length(), (DWORD)postData.length(), 0);
    
    std::string token = "";
    std::string response = "";
    DWORD dwError = 0;

    if (bResults && WinHttpReceiveResponse(hRequestAuth, NULL)) {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        do {
            WinHttpQueryDataAvailable(hRequestAuth, &dwSize);
            if (dwSize == 0) break;
            char* pszOutBuffer = new char[dwSize + 1];
            if (WinHttpReadData(hRequestAuth, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                pszOutBuffer[dwDownloaded] = '\0';
                response += pszOutBuffer;
            }
            delete[] pszOutBuffer;
        } while (dwSize > 0);

        size_t tokenPos = response.find("\"access_token\":\"");
        if (tokenPos != std::string::npos) {
            tokenPos += 16;
            size_t tokenEnd = response.find("\"", tokenPos);
            token = response.substr(tokenPos, tokenEnd - tokenPos);
        }
    } else {
        dwError = GetLastError();
    }
    WinHttpCloseHandle(hRequestAuth);

    if (token.empty()) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        AcquireSRWLockExclusive(&g_statsLock);
        
        g_consecutiveErrors++;
        
        if (dwError != 0) {
            if (g_consecutiveErrors <= 4) {
                g_displayName = L"Loading...";
            } else {
                g_displayName = L"⛔ Net Error: " + std::to_wstring(dwError);
            }
            Wh_Log(L"Network Error: %lu", dwError);
        } else {
            g_displayName = L"⛔ API Error";
            Wh_Log(L"API Auth Failed. Server Response: %hs", response.c_str());
        }
        
        g_displayStats = L"";
        g_avatarPath = L"";
        ReleaseSRWLockExclusive(&g_statsLock);
        return;
    }

    std::wstring userPath = L"/api/v2/users/" + g_username;
    HINTERNET hRequestUser = WinHttpOpenRequest(hConnect, L"GET", userPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    
    std::wstring authHeader = L"Authorization: Bearer " + StringToWString(token) + L"\r\n";
    bResults = WinHttpSendRequest(hRequestUser, authHeader.c_str(), (DWORD)-1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    std::string userResponse;

    if (bResults && WinHttpReceiveResponse(hRequestUser, NULL)) {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        do {
            WinHttpQueryDataAvailable(hRequestUser, &dwSize);
            if (dwSize == 0) break;
            char* pszOutBuffer = new char[dwSize + 1];
            if (WinHttpReadData(hRequestUser, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                pszOutBuffer[dwDownloaded] = '\0';
                userResponse += pszOutBuffer;
            }
            delete[] pszOutBuffer;
        } while (dwSize > 0);
    }
    WinHttpCloseHandle(hRequestUser);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    if (userResponse.empty() || userResponse.find("\"error\"") != std::string::npos || userResponse.find("\"authentication\"") != std::string::npos) {
        Wh_Log(L"User Fetch Failed. Server Response: %hs", userResponse.c_str());
        AcquireSRWLockExclusive(&g_statsLock);
        
        g_consecutiveErrors++;

        if (g_consecutiveErrors <= 4) {
            g_displayName = L"Loading...";
        } else {
            g_displayName = L"⛔ User Error";
        }
        
        g_displayStats = L"";
        g_avatarPath = L"";
        ReleaseSRWLockExclusive(&g_statsLock);
        return;
    }

    std::string pp = "0", rank = "0", username = "Unknown", avatarUrl = "";
    
    size_t unPos = userResponse.find("\"username\":\"");
    if (unPos != std::string::npos) {
        unPos += 12;
        size_t unEnd = userResponse.find("\"", unPos);
        username = userResponse.substr(unPos, unEnd - unPos);
    }

    size_t ppPos = userResponse.find("\"pp\":");
    if (ppPos != std::string::npos) {
        ppPos += 5;
        size_t ppEnd = userResponse.find(",", ppPos);
        pp = userResponse.substr(ppPos, ppEnd - ppPos);
        size_t dotPos = pp.find(".");
        if (dotPos != std::string::npos) {
            pp = pp.substr(0, dotPos);
        }
    }

    size_t rankPos = userResponse.find("\"global_rank\":");
    if (rankPos != std::string::npos) {
        rankPos += 14;
        size_t rankEnd = userResponse.find(",", rankPos);
        rank = userResponse.substr(rankPos, rankEnd - rankPos);
    }

    size_t avPos = userResponse.find("\"avatar_url\":\"");
    if (avPos != std::string::npos) {
        avPos += 14;
        size_t avEnd = userResponse.find("\"", avPos);
        avatarUrl = userResponse.substr(avPos, avEnd - avPos);
        size_t pos = 0;
        while ((pos = avatarUrl.find("\\/", pos)) != std::string::npos) {
            avatarUrl.replace(pos, 2, "/");
            pos += 1;
        }
    }

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring localAvatarPath = std::wstring(tempPath) + L"osu_avatar.jpg";

    if (!avatarUrl.empty()) {
        URLDownloadToFileW(NULL, StringToWString(avatarUrl).c_str(), localAvatarPath.c_str(), 0, NULL);
    }

    AcquireSRWLockExclusive(&g_statsLock);
    g_consecutiveErrors = 0;
    g_displayName = StringToWString(username);
    g_displayStats = L"PP: " + StringToWString(pp) + L"pp // #" + StringToWString(FormatWithDots(rank));
    g_avatarPath = localAvatarPath;
    ReleaseSRWLockExclusive(&g_statsLock);
}

void DrawOverlay(HWND hwnd) {
    AcquireSRWLockShared(&g_statsLock);
    std::wstring name = g_displayName;
    std::wstring stats = g_isUpdating ? L"uno momento..." : g_displayStats;
    std::wstring avPath = g_avatarPath;
    ReleaseSRWLockShared(&g_statsLock);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 200;
    bmi.bmiHeader.biHeight = -50; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits;
    HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    HGDIOBJ hOld = SelectObject(hdcMem, hBmp);

    {
        Graphics graphics(hdcMem);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
        graphics.Clear(Color(0, 0, 0, 0)); 

        FontFamily fontFamily(L"Segoe UI");
        Font fontName(&fontFamily, 14, FontStyleBold, UnitPixel);
        Font fontStats(&fontFamily, 12, FontStyleRegular, UnitPixel);
        SolidBrush textBrush(Color(255, 255, 255, 255));

        if (avPath.empty()) {
            StringFormat format;
            format.SetAlignment(StringAlignmentCenter);
            format.SetLineAlignment(StringAlignmentCenter);
            
            RectF rect(0, 0, 200, 50);

            if (g_isUpdating && g_consecutiveErrors == 0) {
                graphics.DrawString(L"uno momento...", -1, &fontName, rect, &format, &textBrush);
            } else {
                graphics.DrawString(name.c_str(), -1, &fontName, rect, &format, &textBrush);
            }
        } else {
            Image image(avPath.c_str());
            
            if (image.GetLastStatus() == Ok) {
                Bitmap resized(32, 32, &graphics);
                Graphics gResize(&resized);
                gResize.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                gResize.Clear(Color(0, 0, 0, 0));
                gResize.DrawImage(&image, 0, 0, 32, 32);

                TextureBrush tBrush(&resized);
                Matrix mat(1.0f, 0.0f, 0.0f, 1.0f, 9.0f, 9.0f);
                tBrush.SetTransform(&mat);

                GraphicsPath path;
                int x = 9, y = 9, w = 32, h = 32, d = 12;
                
                path.AddArc(x, y, d, d, 180, 90);
                path.AddArc(x + w - d, y, d, d, 270, 90);
                path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
                path.AddArc(x, y + h - d, d, d, 90, 90);
                path.CloseFigure();

                graphics.FillPath(&tBrush, &path);
            }

            graphics.DrawString(name.c_str(), -1, &fontName, PointF(50, 6), &textBrush);
            graphics.DrawString(stats.c_str(), -1, &fontStats, PointF(50, 24), &textBrush);
        }
    }

    POINT ptSrc = {0, 0};
    SIZE size = {200, 50};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    
    UpdateLayeredWindow(hwnd, hdcScreen, NULL, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

void NetThreadFunc() {
    while (g_running) {
        g_isUpdating = true;
        g_needsRedraw = true;

        FetchOsuStats();

        AcquireSRWLockShared(&g_statsLock);
        bool hasError = (g_consecutiveErrors > 0);
        ReleaseSRWLockShared(&g_statsLock);

        g_isUpdating = false;
        g_needsRedraw = true;

        int currentInterval = hasError ? 15 : g_updateInterval;

        for(int i = 0; i < currentInterval && g_running; i++) {
            if (g_forceUpdate) {
                g_forceUpdate = false;
                break;
            }
            Sleep(1000);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            if (!g_username.empty()) {
                std::wstring url = L"https://osu.ppy.sh/users/" + g_username;
                ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            return 0;
        }
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
        case WM_TIMER: {
            if (wParam == 1) {
                int width = 200;
                int height = 50;
                int x = 0, y = 0;
                bool posFound = false;
                HWND insertAfter = NULL;
                UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;

                HWND trayWnd = FindWindowW(L"Shell_TrayWnd", NULL);
                if (trayWnd) {
                    HWND trayNotifyWnd = FindWindowExW(trayWnd, NULL, L"TrayNotifyWnd", NULL);
                    RECT rect;
                    if (trayNotifyWnd && GetWindowRect(trayNotifyWnd, &rect)) {
                        x = rect.left - width;
                        y = rect.top + ((rect.bottom - rect.top) - height) / 2;
                        posFound = true;
                    } else if (GetWindowRect(trayWnd, &rect)) {
                        x = rect.right - width - 250;
                        y = rect.top + ((rect.bottom - rect.top) - height) / 2;
                        posFound = true;
                    }

                    HWND wndPrev = GetWindow(trayWnd, GW_HWNDPREV);
                    if (wndPrev == hwnd) {
                        flags |= SWP_NOZORDER;
                    } else {
                        insertAfter = wndPrev ? wndPrev : ((GetWindowLongW(trayWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP);
                    }
                }

                if (!posFound) {
                    RECT workArea;
                    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
                    x = workArea.right - width;
                    y = workArea.bottom - height;
                    insertAfter = HWND_TOPMOST;
                    flags &= ~SWP_NOZORDER;
                }

                SetWindowPos(hwnd, insertAfter, x, y, width, height, flags);
                
                if (g_needsRedraw) {
                    DrawOverlay(hwnd);
                    g_needsRedraw = false;
                }
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void UiThreadFunc() {
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    {
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"OsuTrayOverlayClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);

        g_overlayHwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            wc.lpszClassName, L"OsuStats",
            WS_POPUP,
            0, 0, 200, 50, 
            NULL, NULL, wc.hInstance, NULL
        );

        SetTimer(g_overlayHwnd, 1, 100, NULL);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (!g_running) break;
        }

        DestroyWindow(g_overlayHwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }

    GdiplusShutdown(gdiplusToken);
}

BOOL WhTool_ModInit() {
    LoadSettings(); 
    
    g_running = true;
    g_uiThread = std::thread(UiThreadFunc);
    g_netThread = std::thread(NetThreadFunc);
    
    return TRUE;
}

void WhTool_ModUninit() {
    g_running = false;

    if (g_overlayHwnd) {
        PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
    }

    if (g_uiThread.joinable()) g_uiThread.join();
    if (g_netThread.joinable()) g_netThread.join();
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    g_forceUpdate = true;
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

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID L"\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"" WH_MOD_ID L"\"",
               currentProcessPath);
    
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
