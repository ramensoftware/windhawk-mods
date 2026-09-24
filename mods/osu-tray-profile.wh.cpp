// ==WindhawkMod==
// @id              osu-tray-profile
// @name            osu!Profile in Taskbar
// @description     Displays PP, rank, and avatar from rhythm game osu! (standard mode) next to the system tray
// @version         3.9.3
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
_(you can use your previous nickname "XATCYHE MIKU, XATCYHE_MIKU, antoshika")_

## ⚠️ Problems:
* **"✎ check 'Settings'"**: You didn't fill in the required fields in the settings.
* **"⛔ Net Error"**: The widget cannot connect to the internet.
* **"⛔ API Error" / "⛔ User Error"**: Invalid Client ID, Client Secret or Username. Make sure that you have copied them completely and without spaces at the end.
* **"⛔ Rate Limited"**: The osu! API has temporarily limited your requests (or Cloudflare challenged the connection). The widget will automatically wait 60 seconds and recover on its own.
* **"⛔ HTTP [code]"**: A specific network or server error occurred (e.g. HTTP 404 - if the user is completely missing or HTTP - 500/502 for server issues).
---
*🥬 Im here: 💙 [hatsunemiku39.ru](http://hatsunemiku39.ru) // 🟣 [osu!profile](https://osu.ppy.sh/users/18815482) // 📶 [Discord](https://discord.gg/3jBQs9buYe)*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- api:
  - client_id: ""
    $name: "Client ID"
    $description: "Enter the ID of the application created in your osu account settings!"
  - client_secret: ""
    $name: "Client Secret"
    $description: "Enter your application's secret key"
  - username: ""
    $name: "Nickname"
    $description: "Your nickname from osu!"
  $name: "osu! API"
  $description: "You need to create an OAuth application in your osu profile settings!"
- update:
  - interval: 300
    $name: "Update time (in seconds)"
    $description: "Frequency of statistics updates (recommended 300 sec = 5 min)"
  $name: "Latency"
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
#include <optional>
#include <cctype>
#include <cstdio>

using namespace Gdiplus;

std::wstring g_clientId = L"";
std::wstring g_clientSecret = L"";
std::wstring g_username = L"";
std::string g_accessToken = "";
int g_updateInterval = 300;

[[clang::no_destroy]] std::optional<std::thread> g_uiThread;
[[clang::no_destroy]] std::optional<std::thread> g_netThread;
std::atomic<bool> g_running{ false };
std::atomic<bool> g_forceUpdate{ false };
std::atomic<bool> g_needsRedraw{ true };
std::atomic<bool> g_isUpdating{ false };
HWND g_overlayHwnd = NULL;
SRWLOCK g_statsLock = SRWLOCK_INIT;
std::wstring g_displayName = L"Loading...";
std::wstring g_displayStats = L"";
std::wstring g_avatarPath = L"";
int g_consecutiveErrors = 0;
int g_lastDpi = 96;

int GetTaskbarDpi() {
    HWND trayWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (trayWnd) {
        return GetDpiForWindow(trayWnd);
    }
    return 96;
}

void LoadSettings() {
    AcquireSRWLockExclusive(&g_statsLock);
    
    PCWSTR clientIdStr = Wh_GetStringSetting(L"api.client_id");
    g_clientId = clientIdStr;
    Wh_FreeStringSetting(clientIdStr);

    PCWSTR clientSecretStr = Wh_GetStringSetting(L"api.client_secret");
    g_clientSecret = clientSecretStr;
    Wh_FreeStringSetting(clientSecretStr);

    PCWSTR usernameStr = Wh_GetStringSetting(L"api.username");
    g_username = usernameStr;
    Wh_FreeStringSetting(usernameStr);

    g_updateInterval = Wh_GetIntSetting(L"update.interval");
    if (g_updateInterval < 5) g_updateInterval = 5;

    g_accessToken.clear();
    
    ReleaseSRWLockExclusive(&g_statsLock);
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

std::wstring EncodeUsername(const std::wstring& username) {
    std::string utf8 = WStringToString(username);
    std::string encoded;
    for (unsigned char c : utf8) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.') {
            encoded += c;
        } else {
            char buf[5];
            sprintf_s(buf, "%%%02X", c);
            encoded += buf;
        }
    }
    return StringToWString(encoded);
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

std::string ParseJsonString(const std::string& json, const std::string& key) {
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) return "";
    
    size_t endOfValue = json.find_first_of(",}", colonPos);
    if (endOfValue == std::string::npos) endOfValue = json.length();

    size_t quoteStart = json.find("\"", colonPos);
    if (quoteStart == std::string::npos || quoteStart > endOfValue) return "";
    
    size_t quoteEnd = json.find("\"", quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";
    
    return json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

std::string ParseJsonNumber(const std::string& json, const std::string& key) {
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "0";
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) return "0";

    size_t endOfValue = json.find_first_of(",}", colonPos);
    if (endOfValue == std::string::npos) endOfValue = json.length();

    size_t start = json.find_first_of("0123456789-", colonPos);
    if (start == std::string::npos || start > endOfValue) return "0";

    size_t end = json.find_first_not_of("0123456789.", start);
    if (end == std::string::npos || end > endOfValue) end = endOfValue;

    std::string num = json.substr(start, end - start);
    size_t dotPos = num.find(".");
    if (dotPos != std::string::npos) {
        num = num.substr(0, dotPos);
    }
    return num;
}

void FetchOsuStats() {
    AcquireSRWLockShared(&g_statsLock);
    std::wstring clientId = g_clientId;
    std::wstring clientSecret = g_clientSecret;
    std::wstring username = g_username;
    std::string cachedToken = g_accessToken;
    ReleaseSRWLockShared(&g_statsLock);

    if (clientId.empty() || clientSecret.empty() || username.empty()) {
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
    LPCWSTR userAgent = L"osu-tray-profile/" WH_MOD_VERSION;

    if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig)) {
        if (proxyConfig.lpszProxy) {
            hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NAMED_PROXY, proxyConfig.lpszProxy, proxyConfig.lpszProxyBypass, 0);
        }
        if (proxyConfig.lpszAutoConfigUrl) GlobalFree(proxyConfig.lpszAutoConfigUrl);
        if (proxyConfig.lpszProxy) GlobalFree(proxyConfig.lpszProxy);
        if (proxyConfig.lpszProxyBypass) GlobalFree(proxyConfig.lpszProxyBypass);
    }

    if (!hSession) hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return;

    WinHttpSetTimeouts(hSession, 10000, 10000, 10000, 10000);
    DWORD secureProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
    WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));

    HINTERNET hConnect = WinHttpConnect(hSession, L"osu.ppy.sh", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return; }

    std::string token = cachedToken;
    LPCWSTR acceptTypes[] = { L"application/json", NULL };

    if (token.empty()) {
        HINTERNET hRequestAuth = WinHttpOpenRequest(hConnect, L"POST", L"/oauth/token", NULL, WINHTTP_NO_REFERER, acceptTypes, WINHTTP_FLAG_SECURE);
        std::wstring contentType = L"Content-Type: application/x-www-form-urlencoded\r\n";
        std::wstring postDataW = L"client_id=" + clientId + L"&client_secret=" + clientSecret + L"&grant_type=client_credentials&scope=public";
        std::string postData = WStringToString(postDataW);

        BOOL bResults = WinHttpSendRequest(hRequestAuth, contentType.c_str(), (DWORD)-1, (LPVOID)postData.c_str(), (DWORD)postData.length(), (DWORD)postData.length(), 0);
        
        std::string response = "";
        DWORD statusCode = 0;
        DWORD dwSize = sizeof(statusCode);

        if (bResults && WinHttpReceiveResponse(hRequestAuth, NULL)) {
            WinHttpQueryHeaders(hRequestAuth, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
            
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

            token = ParseJsonString(response, "access_token");
        }
        WinHttpCloseHandle(hRequestAuth);

        if (token.empty()) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            AcquireSRWLockExclusive(&g_statsLock);
            g_consecutiveErrors++;
            if (statusCode == 429) {
                g_displayName = L"⛔ Rate Limited";
            } else if (statusCode == 0) {
                g_displayName = L"⛔ Net Error";
            } else {
                g_displayName = L"⛔ HTTP " + std::to_wstring(statusCode);
            }
            g_displayStats = L"";
            g_avatarPath = L"";
            ReleaseSRWLockExclusive(&g_statsLock);
            Wh_Log(L"Auth fetch failed: HTTP %lu, Response: %hs", statusCode, response.c_str());
            return;
        }

        AcquireSRWLockExclusive(&g_statsLock);
        g_accessToken = token;
        ReleaseSRWLockExclusive(&g_statsLock);
    }
    
    std::wstring userPath = L"/api/v2/users/@" + EncodeUsername(username);
    HINTERNET hRequestUser = WinHttpOpenRequest(hConnect, L"GET", userPath.c_str(), NULL, WINHTTP_NO_REFERER, acceptTypes, WINHTTP_FLAG_SECURE);
    
    std::wstring authHeader = L"Authorization: Bearer " + StringToWString(token) + L"\r\n";
    BOOL bResults = WinHttpSendRequest(hRequestUser, authHeader.c_str(), (DWORD)-1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    std::string userResponse;
    DWORD userStatusCode = 0;
    DWORD dwSize = sizeof(userStatusCode);

    if (bResults && WinHttpReceiveResponse(hRequestUser, NULL)) {
        WinHttpQueryHeaders(hRequestUser, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &userStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
        
        if (userStatusCode == 401) {
            WinHttpCloseHandle(hRequestUser);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            AcquireSRWLockExclusive(&g_statsLock);
            g_accessToken = ""; 
            ReleaseSRWLockExclusive(&g_statsLock);
            g_forceUpdate = true;
            return;
        }

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

    if (userResponse.empty() || userStatusCode != 200) {
        AcquireSRWLockExclusive(&g_statsLock);
        g_consecutiveErrors++;
        if (userStatusCode == 429) {
            g_displayName = L"⛔ Rate Limited";
        } else if (userStatusCode == 0) {
            g_displayName = L"⛔ Net Error";
        } else {
            g_displayName = L"⛔ HTTP " + std::to_wstring(userStatusCode);
        }
        g_displayStats = L"";
        g_avatarPath = L"";
        ReleaseSRWLockExclusive(&g_statsLock);
        Wh_Log(L"User fetch failed: HTTP %lu, Response: %hs", userStatusCode, userResponse.c_str());
        return;
    }

    std::string parsedUsername = ParseJsonString(userResponse, "username");
    if (parsedUsername.empty()) parsedUsername = "Unknown";
    
    std::string pp = ParseJsonNumber(userResponse, "pp");
    std::string rank = ParseJsonNumber(userResponse, "global_rank");
    std::string avatarUrl = ParseJsonString(userResponse, "avatar_url");
    
    size_t pos = 0;
    while ((pos = avatarUrl.find("\\/", pos)) != std::string::npos) {
        avatarUrl.replace(pos, 2, "/");
        pos += 1;
    }

    WCHAR storagePath[MAX_PATH];
    std::wstring localAvatarPath;
    if (Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        localAvatarPath = std::wstring(storagePath) + L"\\avatar.jpg";
    }

    if (!avatarUrl.empty() && !localAvatarPath.empty()) {
        URLDownloadToFileW(NULL, StringToWString(avatarUrl).c_str(), localAvatarPath.c_str(), 0, NULL);
    }

    AcquireSRWLockExclusive(&g_statsLock);
    g_consecutiveErrors = 0;
    g_displayName = StringToWString(parsedUsername);
    std::wstring displayRank = (rank == "0") ? L"-" : StringToWString(FormatWithDots(rank));
    g_displayStats = L"PP: " + StringToWString(pp) + L"pp // #" + displayRank;
    g_avatarPath = localAvatarPath;
    ReleaseSRWLockExclusive(&g_statsLock);
}

void DrawOverlay(HWND hwnd) {
    AcquireSRWLockShared(&g_statsLock);
    std::wstring name = g_displayName;
    std::wstring stats = g_isUpdating ? L"uno momento..." : g_displayStats;
    std::wstring avPath = g_avatarPath;
    bool hasError = (g_consecutiveErrors > 0);
    ReleaseSRWLockShared(&g_statsLock);

    int dpi = GetTaskbarDpi();
    
    int scaledWidth = MulDiv(200, dpi, 96);
    int scaledHeight = MulDiv(50, dpi, 96);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = scaledWidth;
    bmi.bmiHeader.biHeight = -scaledHeight; 
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
        
        int s14 = MulDiv(14, dpi, 96);
        int s12_f = MulDiv(12, dpi, 96);
        int s32 = MulDiv(32, dpi, 96);
        int s9 = MulDiv(9, dpi, 96);
        int s12 = MulDiv(12, dpi, 96);
        int s50 = MulDiv(50, dpi, 96);
        int s6 = MulDiv(6, dpi, 96);
        int s24 = MulDiv(24, dpi, 96);

        FontFamily fontFamily(L"Segoe UI");
        Font fontName(&fontFamily, s14, FontStyleBold, UnitPixel);
        Font fontStats(&fontFamily, s12_f, FontStyleRegular, UnitPixel);
        SolidBrush textBrush(Color(255, 255, 255, 255));

        if (avPath.empty()) {
            StringFormat format;
            format.SetAlignment(StringAlignmentCenter);
            format.SetLineAlignment(StringAlignmentCenter);
            RectF rect(0, 0, scaledWidth, scaledHeight);

            if (g_isUpdating && !hasError) {
                graphics.DrawString(L"uno momento...", -1, &fontName, rect, &format, &textBrush);
            } else {
                graphics.DrawString(name.c_str(), -1, &fontName, rect, &format, &textBrush);
            }
        } else {
            Image image(avPath.c_str());
            if (image.GetLastStatus() == Ok) {
                Bitmap resized(s32, s32, &graphics);
                Graphics gResize(&resized);
                gResize.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                gResize.Clear(Color(0, 0, 0, 0));
                gResize.DrawImage(&image, 0, 0, s32, s32);

                TextureBrush tBrush(&resized);
                Matrix mat(1.0f, 0.0f, 0.0f, 1.0f, (float)s9, (float)s9);
                tBrush.SetTransform(&mat);

                GraphicsPath path;
                path.AddArc(s9, s9, s12, s12, 180, 90);
                path.AddArc(s9 + s32 - s12, s9, s12, s12, 270, 90);
                path.AddArc(s9 + s32 - s12, s9 + s32 - s12, s12, s12, 0, 90);
                path.AddArc(s9, s9 + s32 - s12, s12, s12, 90, 90);
                path.CloseFigure();

                graphics.FillPath(&tBrush, &path);
            }

            graphics.DrawString(name.c_str(), -1, &fontName, PointF(s50, s6), &textBrush);
            graphics.DrawString(stats.c_str(), -1, &fontStats, PointF(s50, s24), &textBrush);
        }
    }

    POINT ptSrc = {0, 0};
    SIZE size = {scaledWidth, scaledHeight};
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
        int currentInterval = hasError ? 60 : g_updateInterval;

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
            AcquireSRWLockShared(&g_statsLock);
            std::wstring username = g_username;
            ReleaseSRWLockShared(&g_statsLock);

            if (!username.empty()) {
                std::wstring safeUsername = EncodeUsername(username);
                std::wstring url = L"https://osu.ppy.sh/users/" + safeUsername;
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
                int dpi = GetTaskbarDpi();
                if (dpi != g_lastDpi) {
                    g_lastDpi = dpi;
                    g_needsRedraw = true;
                }

                int width = MulDiv(200, dpi, 96);
                int height = MulDiv(50, dpi, 96);
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
                        x = rect.right - width - MulDiv(250, dpi, 96);
                        y = rect.top + ((rect.bottom - rect.top) - height) / 2;
                        posFound = true;
                    }

                    HWND wndPrev = GetWindow(trayWnd, GW_HWNDPREV);
                    
                    while (wndPrev && wndPrev != hwnd) {
                        WCHAR className[256];
                        GetClassNameW(wndPrev, className, 256);
                        if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
                            wcscmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
                            wcscmp(className, L"StartMenuExperienceHost") == 0 ||
                            wcscmp(className, L"SearchHost") == 0 ||
                            wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
                            wndPrev = GetWindow(wndPrev, GW_HWNDPREV);
                        } else {
                            break;
                        }
                    }

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
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
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
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"OsuStats",
            WS_POPUP,
            0, 0, 200, 50, 
            NULL, NULL, wc.hInstance, NULL
        );

        SetTimer(g_overlayHwnd, 1, 500, NULL);

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
    g_uiThread.emplace(UiThreadFunc);
    g_netThread.emplace(NetThreadFunc);
    
    return TRUE;
}

void WhTool_ModUninit() {
    g_running = false;

    if (g_overlayHwnd) {
        PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
    }

    if (g_uiThread && g_uiThread->joinable()) {
        g_uiThread->join();
        g_uiThread.reset();
    }
    if (g_netThread && g_netThread->joinable()) {
        g_netThread->join();
        g_netThread.reset();
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    g_forceUpdate = true;
}

// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
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
