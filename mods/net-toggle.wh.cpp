// ==WindhawkMod==
// @id              net-toggle
// @name            Net-Toggle
// @description     Network toggle + DNS reachability monitor in your system tray
// @version         2.0.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -liphlpapi -lws2_32 -ladvapi32 -DWIN32_LEAN_AND_MEAN
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- dnsServer: "8.8.8.8"
  $name: DNS Server IP
  $description: IP address to ping for DNS reachability (leave blank to disable DNS monitoring)

- pingInterval: 10
  $name: Ping Interval (seconds)
  $description: How often to check DNS reachability
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Net-Toggle

A lightning-fast internet kill switch with DNS reachability monitoring — right in your taskbar!

Ever needed to quickly disconnect from the web without digging through Windows settings or ripping the ethernet cable out of the wall? Net-Toggle adds a clean, native-looking button directly to your system tray.

One click drops your connection. Click it again, and you're back online.

## Colors Legend (Tray Icon)

**🔴 Red** — Network is OFF

![Red](https://i.imgur.com/ilyJM0R.png)

**🟡 Yellow** — Network toggle or reset is in progress

![Yellow](https://i.imgur.com/jBnpNRK.png)

**🔵 Blue** — Network is ON, no DNS server configured

![Blue](https://i.imgur.com/hMRbXyu.png)

**🟢 Green** — Network is ON, DNS is reachable

![Green](https://i.imgur.com/4KACqME.png)

**⚪ Grey** — Network is ON, DNS is unreachable

![Grey](https://i.imgur.com/xoZgdiH.png)

## How to Use It

1. **Find the Icon:** Look in your system tray (bottom right of your screen, next to the clock) for the little `^` arrow. Hit it and look for the network icon.
2. **Left-click to Toggle:** Give the icon a single click.
3. **Middle-click for Full Reset:** Middle-click the icon to run a full network cycle reset — disables all adapters, flushes DNS, then re-enables them. This cuts all active connections.
4. **Right-click for Menu:** Disable/Enable network, open Network Settings, or exit.
5. **Approve the Prompt (on toggle/reset):** Windows will pop up a quick UAC screen asking for permission. Click **Yes**.

   > **Why do I need to accept the UAC?**
   >
   > Windows requires admin permission to physically turn off your network hardware or reset your connection.
   >
   > This is a built-in security feature to stop rogue background apps from doing this secretly.

6. **Configure DNS Monitoring:** In Windhawk mod settings, enter a DNS server IP (e.g. `8.8.8.8`) to enable reachability monitoring. The dot turns green when reachable, grey when not.

## Changelog

### v2.0.0
- **Complete rebuild.** Mod renamed to Net-Toggle.
- New: **DNS Monitoring** — The icon monitors your connection and changes color if your internet drops out (configurable in Mod Settings).
- New: **WiFi-style tray icon** with 5 color states (Red / Yellow / Blue / Green / Grey).
- New: Middle-click → **Full Network Reset** — disables all adapters, flushes DNS, re-enables them. Cuts all active connections.
- New: Right-click context menu — toggle network, open Network Settings, or exit.
- New: Donate button on the mod page.
- Improved: Tray icon is now independent from the Windhawk app and no longer groups with it in the taskbar.
- Improved: Tray icon persists reliably across Explorer restarts.
- Improved: Icon stays in sync even if you disable your adapter directly in Windows Settings.

### v1.0
- Initial release.
*/
// ==/WindhawkModReadme==

// iphlpapi.h only declares the netioapi APIs
// (GetIfTable2 / MIB_IF_TABLE2 / FreeMibTable) when the winsock2 types are
// already in scope. -DWIN32_LEAN_AND_MEAN in compiler options prevents windows.h
// from including the legacy winsock.h.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propidl.h>
#include <iphlpapi.h>
#include <math.h>

#define TRAY_ICON_ID 1
#define WM_TRAY_CALLBACK (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define WM_SETTINGS_CHANGED (WM_USER + 4)
#define WM_TRIGGER_PING (WM_USER + 5)
#define DNS_PING_TIMER_ID 2
#define DNS_RECOVERY_TIMER_ID 3

#define MENU_TOGGLE_NET    1
#define MENU_NET_SETTINGS  2
#define MENU_OPEN_WINDHAWK 9000

// Stable GUID that gives our tray icon a process-independent identity.
static const GUID NETTOGGLE_TRAY_GUID =
    {0x246764CF, 0xF857, 0x4399, {0x8D, 0x3D, 0x22, 0x76, 0x1A, 0x6A, 0xBD, 0x95}};

const DWORD CLICK_DEBOUNCE_MS = 10000;

static const DWORD POWERSHELL_TIMEOUT_MS  = 60000;  // 60s max for any PS command
static const DWORD NETWATCH_POLL_INTERVAL = 15000;  // 15s per fallback poll tick
static const DWORD NETWATCH_POLL_RETRIES  = 4;      // 4 × 15s = 60s then retry NotifyAddrChange
static const int   MIN_PING_INTERVAL_SEC  = 5;
static const int   DNS_TCP_TIMEOUT_SEC    = 2;
static const int   DNS_TCP_TIMEOUT_USEC   = 500000; // 0.5s (total 2.5s)

static volatile LONG g_isProcessingClick = 0;
static volatile LONG g_trayIconInstalled = 0;
static volatile LONG g_networkIsUp = 1;
static HANDLE g_trayThread = nullptr;
static volatile HWND g_trayHwnd = nullptr;
static HINSTANCE g_hInstance = nullptr;
static volatile DWORD g_lastClickTime = 0;
static UINT g_taskbarCreatedMsg = 0;
static HANDLE g_activeWorkerThread = nullptr;

// DNS monitoring
static volatile DWORD g_dnsServerIp = 0;
static DWORD g_pingIntervalMs = 10000;
static volatile LONG g_dnsIsReachable = 0;
static volatile LONG g_dnsWorkerRunning = 0;

// Network watch thread
static HANDLE g_netWatchThread = nullptr;
static HANDLE g_shutdownEvent = nullptr;

// Current tray icon handle (destroy before replace)
static HICON g_currentIcon = nullptr;

static WCHAR   g_windhawkPath[MAX_PATH]  = {};
static WCHAR   g_ddoresDllPath[MAX_PATH] = {};
static HICON   g_hWindHawkIcon = nullptr;
static HBITMAP g_hWindHawkBmp  = nullptr;

// helpers

void LogLastError(LPCWSTR context) {
    DWORD error = GetLastError();
    if (error == 0) return;

    LPWSTR errorMsg = nullptr;
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPWSTR)&errorMsg, 0, nullptr)) {
        Wh_Log(L"[ERROR] %s: %s (0x%X)", context, errorMsg, error);
        LocalFree(errorMsg);
    } else {
        Wh_Log(L"[ERROR] %s: Error %d", context, error);
    }
}

BOOL CheckActualNetworkState() {
    // Without GAA_FLAG_INCLUDE_ALL_INTERFACES, GetAdaptersAddresses only returns
    // adapters that have at least one IP address assigned. Disabled adapters have
    // no IP address and therefore do not appear — they return a count of 0.
    // (Disabled-NetAdapter sets IfOperStatusDown, not IfOperStatusNotPresent, so
    // filtering on OperStatus is unreliable for detecting administratively disabled
    // adapters. Relying on the OS to exclude address-less entries is cleaner.)
    // GetAdaptersAddresses uses a separate internal code path from
    // NotifyAddrChange/GetIfTable2, avoiding an iphlpapi.dll shared-handle
    // contamination that causes GetIfTable2 to return ERROR_INVALID_HANDLE after
    // NotifyAddrChange fails in injected contexts.
    ULONG flags = GAA_FLAG_SKIP_UNICAST | GAA_FLAG_SKIP_ANYCAST |
                  GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    ULONG size = 16384;
    BYTE* buf = (BYTE*)HeapAlloc(GetProcessHeap(), 0, size);
    if (!buf) return TRUE;

    ULONG ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                     (IP_ADAPTER_ADDRESSES*)buf, &size);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        HeapFree(GetProcessHeap(), 0, buf);
        buf = (BYTE*)HeapAlloc(GetProcessHeap(), 0, size);
        if (!buf) return TRUE;
        ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                   (IP_ADAPTER_ADDRESSES*)buf, &size);
    }
    if (ret != NO_ERROR) {
        Wh_Log(L"GetAdaptersAddresses failed (%lu), assuming ON", ret);
        HeapFree(GetProcessHeap(), 0, buf);
        return TRUE;
    }

    int count = 0;
    for (IP_ADAPTER_ADDRESSES* aa = (IP_ADAPTER_ADDRESSES*)buf; aa; aa = aa->Next) {
        if (aa->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;
        if (aa->IfType == IF_TYPE_TUNNEL) continue;
        if (aa->PhysicalAddressLength == 0) continue;
        count++;
    }
    HeapFree(GetProcessHeap(), 0, buf);
    return count > 0;
}

BOOL RunPowerShellCommand(LPCWSTR psCommand, BOOL targetState) {
    Wh_Log(L"Executing PowerShell command");

    WCHAR cmdArgs[2048];
    if (wsprintfW(cmdArgs, L"-NoProfile -NonInteractive -WindowStyle Hidden -Command \"%s\"", psCommand) <= 0) {
        Wh_Log(L"Failed to format command");
        return FALSE;
    }

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
    sei.hwnd = nullptr;
    sei.lpVerb = L"runas";
    sei.lpFile = L"powershell.exe";
    sei.lpParameters = cmdArgs;
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            Wh_Log(L"UAC prompt cancelled by user");
        } else {
            Wh_Log(L"ShellExecuteEx failed: %d", error);
        }
        return FALSE;
    }

    Wh_Log(L"UAC cleared. Waiting for PowerShell to complete...");

    if (!sei.hProcess) {
        Wh_Log(L"No process handle returned");
        return FALSE;
    }

    DWORD waitResult = WaitForSingleObject(sei.hProcess, POWERSHELL_TIMEOUT_MS);
    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Process timed out, terminating");
        TerminateProcess(sei.hProcess, 1);
        CloseHandle(sei.hProcess);
        return FALSE;
    }

    DWORD exitCode = 1;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode)) {
        LogLastError(L"GetExitCodeProcess");
        CloseHandle(sei.hProcess);
        return FALSE;
    }

    CloseHandle(sei.hProcess);
    Wh_Log(L"Process exited with code: %d", exitCode);
    if (exitCode == 0) {
        InterlockedExchange(&g_networkIsUp, targetState ? 1 : 0);
        return TRUE;
    } else {
        Wh_Log(L"PowerShell command failed (exit %d) — network state unchanged", exitCode);
        return FALSE;
    }
}

// ==============================================================================
// Feature B — ICMP DNS Ping
// ==============================================================================

BOOL PingIp(DWORD ipAddr) {
    // Uses a TCP connect to port 53 instead of IcmpSendEcho.
    // IcmpCreateFile fails with ERROR_INVALID_HANDLE (6) inside this process
    // after Disable-NetAdapter removes all adapters from the IP stack — the ICMP
    // kernel device path doesn't recover until the process restarts. Winsock TCP
    // sockets are not affected by adapter disable/enable cycles.
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        Wh_Log(L"PingIp: socket() failed (%d)", WSAGetLastError());
        return FALSE;
    }

    u_long nonBlocking = 1;
    ioctlsocket(sock, FIONBIO, &nonBlocking);

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ipAddr;  // network byte order from InetPtonW
    addr.sin_port = htons(53);

    int connectResult = connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    int connectErr = (connectResult == SOCKET_ERROR) ? WSAGetLastError() : 0;

    BOOL reachable = FALSE;
    if (connectResult == 0) {
        reachable = TRUE;
    } else if (connectErr == WSAEWOULDBLOCK || connectErr == WSAEINPROGRESS) {
        fd_set writeSet, exceptSet;
        FD_ZERO(&writeSet);
        FD_ZERO(&exceptSet);
        FD_SET(sock, &writeSet);
        FD_SET(sock, &exceptSet);
        TIMEVAL tv = {DNS_TCP_TIMEOUT_SEC, DNS_TCP_TIMEOUT_USEC};
        int sel = select(0, nullptr, &writeSet, &exceptSet, &tv);
        if (sel > 0 && FD_ISSET(sock, &writeSet)) {
            int sockErr = 0;
            int optLen = sizeof(sockErr);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&sockErr), &optLen);
            // CONNREFUSED means the host is up but port 53 closed (extremely unlikely for 8.8.8.8)
            reachable = (sockErr == 0 || sockErr == WSAECONNREFUSED);
        } else {
            Wh_Log(L"PingIp: TCP:53 timed out or select error (%d)", WSAGetLastError());
        }
    } else {
        Wh_Log(L"PingIp: connect() failed immediately (%d)", connectErr);
    }

    closesocket(sock);
    return reachable;
}

// ==============================================================================
// Feature C — Wifi-Shape Icon
// ==============================================================================

// Draws a wifi fan (3 arc bands + center dot) into a 32-bit pre-multiplied-alpha
// DIB.  We work at 2× resolution then down-sample 2×2 → 1 for cheap anti-aliasing.
//
// Arc geometry: the fan is centred at the bottom-centre of the icon, spanning
// ±65° either side of straight-up (i.e. the sweep covers 130° of arc).
// Three rings at outer/mid/inner radii with a proportional pen width, plus a
// small filled dot at the origin.

static void DrawWifiIcon(DWORD* px, int W, int H, BYTE r, BYTE g, BYTE b) {
    // Arc parameters (in icon-pixel units)
    float cx = W * 0.5f;
    float cy = H * 0.88f - 3.0f;   // origin sits near the bottom

    float outerR = W * 0.598f;
    float midR   = W * 0.403f;
    float innerR = W * 0.221f;
    float dotR   = W * 0.098f;
    float penW   = W * 0.143f;  // arc stroke width

    // Arc spans ±65° from straight-up (270° in standard coords)
    const float PI = 3.14159265f;
    float halfSweep = 65.0f * PI / 180.0f;
    float baseAngle = 270.0f * PI / 180.0f;    // pointing up
    float a0 = baseAngle - halfSweep;
    float a1 = baseAngle + halfSweep;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float dist = sqrtf(dx * dx + dy * dy);

            // Determine if pixel centre is on one of the three arc bands
            bool onArc = false;
            if (dist > dotR) {
                float radii[3] = { outerR, midR, innerR };
                for (int i = 0; i < 3; i++) {
                    if (fabsf(dist - radii[i]) <= penW * 0.5f) {
                        // Check angular range
                        float angle = atan2f(dy, dx);
                        // Normalise to [0, 2π)
                        if (angle < 0) angle += 2.0f * PI;
                        float a0n = a0, a1n = a1;
                        if (a0n < 0) a0n += 2.0f * PI;
                        if (a1n < 0) a1n += 2.0f * PI;
                        bool inSweep = (a0n <= a1n)
                            ? (angle >= a0n && angle <= a1n)
                            : (angle >= a0n || angle <= a1n);
                        if (inSweep) { onArc = true; break; }
                    }
                }
            }

            // Determine if pixel is in the centre dot
            bool onDot = (dist <= dotR);

            if (onArc || onDot) {
                // Simple coverage fraction for the outermost ring edge (cheap AA)
                float alpha = 1.0f;
                if (onArc) {
                    // Edge softening: ramp alpha over 1px at outer boundary
                    float nearestR = 0;
                    float radii[3] = { outerR, midR, innerR };
                    float minDelta = 1e9f;
                    for (int i = 0; i < 3; i++) {
                        float d = fabsf(dist - radii[i]);
                        if (d < minDelta) { minDelta = d; nearestR = radii[i]; }
                    }
                    float edge = fabsf(dist - nearestR) - (penW * 0.5f - 1.0f);
                    if (edge > 0) alpha = 1.0f - edge;
                    if (alpha < 0) alpha = 0;
                }
                DWORD a8 = (DWORD)(alpha * 255.0f + 0.5f);
                if (a8 > 255) a8 = 255;
                DWORD pr = (r * a8 + 127) / 255;
                DWORD pg = (g * a8 + 127) / 255;
                DWORD pb = (b * a8 + 127) / 255;
                px[y * W + x] = (a8 << 24) | (pr << 16) | (pg << 8) | pb;
            }
        }
    }
}

HICON CreateColoredDotIcon(BOOL netUp, BOOL dnsUp, BOOL hasDns, BOOL pending) {
    int cx = GetSystemMetrics(SM_CXSMICON);
    int cy = GetSystemMetrics(SM_CYSMICON);

    COLORREF iconColor;
    if (pending)         iconColor = RGB(240, 180, 0);   // Yellow
    else if (!netUp)     iconColor = RGB(220, 50,  50);  // Red
    else if (!hasDns)    iconColor = RGB(70,  130, 255); // Blue
    else if (dnsUp)      iconColor = RGB(60,  220, 60);  // Green
    else                 iconColor = RGB(160, 160, 160); // Grey

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = cx;
    bmi.bmiHeader.biHeight      = -cy;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdcScreen);
    if (!hBmp) return nullptr;

    if (bits) {
        DWORD* pixels = (DWORD*)bits;
        memset(pixels, 0, (size_t)cx * cy * 4);
        DrawWifiIcon(pixels, cx, cy,
                     GetRValue(iconColor), GetGValue(iconColor), GetBValue(iconColor));
    }

    HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, nullptr);
    if (hMask) {
        HDC hdcMask = CreateCompatibleDC(nullptr);
        HGDIOBJ hOld = SelectObject(hdcMask, hMask);
        PatBlt(hdcMask, 0, 0, cx, cy, WHITENESS);
        SelectObject(hdcMask, hOld);
        DeleteDC(hdcMask);
    }

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hBmp;
    ii.hbmMask  = hMask ? hMask : hBmp;
    HICON hResult = CreateIconIndirect(&ii);

    DeleteObject(hBmp);
    if (hMask) DeleteObject(hMask);

    return hResult;
}

// ==============================================================================
// Tray Icon
// ==============================================================================

void AddOrUpdateTrayIcon(HWND hWnd, BOOL enabled, BOOL isAdd) {
    BOOL dnsUp = (g_dnsIsReachable == 1);
    BOOL hasDns = (g_dnsServerIp != 0);
    BOOL pending = (g_isProcessingClick != 0);

    HICON hNewIcon = CreateColoredDotIcon(enabled, dnsUp, hasDns, pending);
    if (!hNewIcon) {
        Wh_Log(L"AddOrUpdateTrayIcon: CreateColoredDotIcon failed");
        return;
    }

    // Destroy old icon to prevent handle leak
    if (g_currentIcon) {
        DestroyIcon(g_currentIcon);
        g_currentIcon = nullptr;
    }

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP | NIF_ICON;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    if (g_isProcessingClick == 1) {
        wsprintfW(nid.szTip, L"Net-Toggle: toggling\u2026");
    } else if (g_isProcessingClick == 2) {
        wsprintfW(nid.szTip, L"Net-Toggle: refreshing\u2026");
    } else if (enabled) {
        if (!hasDns)
            wsprintfW(nid.szTip, L"Net-Toggle: ON");
        else if (dnsUp)
            wsprintfW(nid.szTip, L"Net-Toggle: ON | DNS reachable");
        else
            wsprintfW(nid.szTip, L"Net-Toggle: ON | DNS unreachable");
    } else {
        wsprintfW(nid.szTip, L"Net-Toggle: OFF (click to enable)");
    }

    nid.hIcon = hNewIcon;
    nid.uFlags |= NIF_GUID;
    nid.guidItem = NETTOGGLE_TRAY_GUID;

    if (isAdd) {
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            LogLastError(L"Shell_NotifyIcon NIM_ADD");
            DestroyIcon(hNewIcon);
            return;
        }
        InterlockedExchange(&g_trayIconInstalled, 1);
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd = hWnd;
        nidVer.uID = TRAY_ICON_ID;
        nidVer.uFlags = NIF_GUID;
        nidVer.guidItem = NETTOGGLE_TRAY_GUID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    } else {
        if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
            LogLastError(L"Shell_NotifyIcon NIM_MODIFY");
            DestroyIcon(hNewIcon);
            return;
        }
    }

    g_currentIcon = hNewIcon;
}

// ==============================================================================
// Toggle Logic
// ==============================================================================

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"WorkerThread: CoInitializeEx failed (0x%X)", hrCo);
        return 1;
    }
    BOOL enable = (BOOL)(UINT_PTR)lpParam;

    Wh_Log(L"Toggling network adapters: %s", enable ? L"ENABLE" : L"DISABLE");
    LPCWSTR command = enable
        ? L"Get-NetAdapter -Physical | Enable-NetAdapter -Confirm:$false"
        : L"Get-NetAdapter -Physical | Disable-NetAdapter -Confirm:$false";

    BOOL success = RunPowerShellCommand(command, enable);

    if (success) {
        success = TRUE;
        Wh_Log(L"Network %s operation completed successfully", enable ? L"enable" : L"disable");
    } else {
        Wh_Log(L"Network toggle operation failed or cancelled");
        success = FALSE;
    }

    InterlockedExchange(&g_isProcessingClick, 0);

    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)(g_networkIsUp == 1), 0);
        // After a successful enable the NetWatch poll fallback may miss the state
        // change (g_networkIsUp was already pre-set to 1 by RunPowerShellCommand).
        // Trigger a recovery ping explicitly so DNS state updates promptly.
        if (success && enable && g_networkIsUp) {
            PostMessageW(g_trayHwnd, WM_TRIGGER_PING, 0, 0);
        }
    }
    if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();
    return 0;
}

DWORD WINAPI ResetWorkerThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"ResetWorkerThread: CoInitializeEx failed (0x%X)", hrCo);
        return 1;
    }
    Wh_Log(L"Executing network blackout reset (disable adapters → flush DNS → re-enable)");

    // Full adapter cycle: kills all active connections, flushes DNS, restores adapters.
    // ipconfig /release+renew omitted — adapter disable/enable already resets DHCP state.
    // netsh winsock/ip reset excluded — they require a reboot to take effect.
    WCHAR cmdArgs[] =
        L"-NoProfile -NonInteractive -WindowStyle Hidden -Command \""
        L"Get-NetAdapter -Physical | Disable-NetAdapter -Confirm:$false; "
        L"Start-Sleep -Seconds 2; "
        L"ipconfig /flushdns; "
        L"Get-NetAdapter -Physical | Enable-NetAdapter -Confirm:$false\"";

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
    sei.hwnd = nullptr;
    sei.lpVerb = L"runas";
    sei.lpFile = L"powershell.exe";
    sei.lpParameters = cmdArgs;
    sei.nShow = SW_HIDE;

    BOOL resetOk = FALSE;
    if (ShellExecuteExW(&sei)) {
        if (sei.hProcess) {
            WaitForSingleObject(sei.hProcess, POWERSHELL_TIMEOUT_MS);
            CloseHandle(sei.hProcess);
        }
        Wh_Log(L"Network reset completed");
        resetOk = TRUE;
    } else {
        Wh_Log(L"Network reset failed to start or was cancelled");
    }

    InterlockedExchange(&g_isProcessingClick, 0);
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)(g_networkIsUp == 1), 0);
        if (resetOk && g_networkIsUp) {
            // Use 15s settle (wParam=1) to give DHCP/routing time to stabilise
            // after the release/renew before we attempt the DNS ping.
            PostMessageW(g_trayHwnd, WM_TRIGGER_PING, 1, 0);
        }
    }
    if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();
    return 0;
}

void ProcessNetworkReset() {
    LONG prev = InterlockedCompareExchange(&g_isProcessingClick, 2, 0);
    if (prev != 0) {
        Wh_Log(L"Already processing a click, ignoring reset request");
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_lastClickTime < CLICK_DEBOUNCE_MS) {
        Wh_Log(L"Click debounced (cooldown active)");
        InterlockedExchange(&g_isProcessingClick, 0);
        return;
    }
    g_lastClickTime = now;

    Wh_Log(L"Processing network reset (Middle Click)");

    // Show yellow immediately
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)g_networkIsUp, 0);
    }

    if (g_activeWorkerThread) {
        CloseHandle(g_activeWorkerThread);
        g_activeWorkerThread = nullptr;
    }
    
    DWORD threadId;
    g_activeWorkerThread = CreateThread(nullptr, 0, ResetWorkerThreadProc, nullptr, 0, &threadId);
    if (!g_activeWorkerThread) {
        InterlockedExchange(&g_isProcessingClick, 0);
    }
}

void ProcessTrayClick() {
    LONG prev = InterlockedCompareExchange(&g_isProcessingClick, 1, 0);
    if (prev != 0) {
        Wh_Log(L"Already processing a click, ignoring");
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_lastClickTime < CLICK_DEBOUNCE_MS) {
        Wh_Log(L"Click debounced (cooldown active)");
        InterlockedExchange(&g_isProcessingClick, 0);
        return;
    }
    g_lastClickTime = now;

    BOOL targetState = (InterlockedOr(&g_networkIsUp, 0) == 0);
    Wh_Log(L"Processing network toggle click. Target state: %s", targetState ? L"ON" : L"OFF");

    // Show yellow immediately
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)g_networkIsUp, 0);
    }

    if (g_activeWorkerThread) {
        CloseHandle(g_activeWorkerThread);
        g_activeWorkerThread = nullptr;
    }

    DWORD threadId;
    g_activeWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, (LPVOID)(UINT_PTR)targetState, 0, &threadId);
    if (!g_activeWorkerThread) {
        InterlockedExchange(&g_isProcessingClick, 0);
    }
}

// ==============================================================================
// DNS Ping Handler
// ==============================================================================

DWORD WINAPI DnsPingWorkerProc(LPVOID lpParam) {
    DWORD ipAddr = (DWORD)(UINT_PTR)lpParam;
    BOOL reachable = PingIp(ipAddr);
    InterlockedExchange(&g_dnsIsReachable, reachable ? 1 : 0);
    HWND hwnd = g_trayHwnd;
    if (hwnd) {
        PostMessageW(hwnd, WM_UPDATE_TRAY_STATE,
                     (WPARAM)(InterlockedOr(&g_networkIsUp, 0) == 1), 0);
    }
    InterlockedExchange(&g_dnsWorkerRunning, 0);
    return 0;
}

void OnDnsPingTimer(HWND hWnd) {
    if (g_dnsServerIp == 0) return;

    if (InterlockedOr(&g_networkIsUp, 0) == 0) {
        Wh_Log(L"Network is OFF, skipping DNS ping");
        InterlockedExchange(&g_dnsIsReachable, 0);
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return;
    }

    if (InterlockedCompareExchange(&g_dnsWorkerRunning, 1, 0) != 0) {
        Wh_Log(L"DNS ping already in progress, skipping");
        return;
    }

    Wh_Log(L"Triggering DNS reachability check...");
    DWORD ipAddr = g_dnsServerIp;
    HANDLE hThread = CreateThread(nullptr, 0, DnsPingWorkerProc,
                                  (LPVOID)(UINT_PTR)ipAddr, 0, nullptr);
    if (!hThread) {
        InterlockedExchange(&g_dnsWorkerRunning, 0);
    } else {
        CloseHandle(hThread);
    }
}

// ==============================================================================
// Feature D — Right-Click Context Menu
// ==============================================================================

static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static void ApplyContextMenuTheme(HWND hWnd, bool dark) {
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (!ux) return;
    using Fn135 = int(WINAPI*)(int);
    using Fn133 = bool(WINAPI*)(HWND, bool);
    using Fn136 = void(WINAPI*)();
    if (auto f = (Fn135)GetProcAddress(ux, MAKEINTRESOURCEA(135))) f(dark ? 2 : 0);
    if (auto f = (Fn133)GetProcAddress(ux, MAKEINTRESOURCEA(133))) f(hWnd, dark);
    if (auto f = (Fn136)GetProcAddress(ux, MAKEINTRESOURCEA(136))) f();
}

void ShowContextMenu(HWND hWnd) {
    HMENU hMenu = CreatePopupMenu();
    BOOL netUp = (g_networkIsUp == 1);
    AppendMenuW(hMenu, MF_STRING, MENU_TOGGLE_NET,
                netUp ? L"Disable Network" : L"Enable Network");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, MENU_NET_SETTINGS, L"Open Network Settings");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open Windhawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    POINT pt; GetCursorPos(&pt);
    bool dark = IsSystemDarkMode();
    ApplyContextMenuTheme(hWnd, dark);
    SetForegroundWindow(hWnd);
    int cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    switch (cmd) {
        case MENU_TOGGLE_NET:
            ProcessTrayClick();
            break;
        case MENU_NET_SETTINGS:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network",
                          nullptr, nullptr, SW_SHOW);
            break;
        case MENU_OPEN_WINDHAWK: {
            SHELLEXECUTEINFOW sei = {sizeof(sei)};
            sei.lpFile = g_windhawkPath;
            sei.nShow  = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
            break;
        }
    }
}

// ==============================================================================
// Tray Window
// ==============================================================================

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CALLBACK) {
        if (LOWORD(lParam) == WM_LBUTTONUP) {
            ProcessTrayClick();
        } else if (LOWORD(lParam) == WM_RBUTTONUP) {
            ShowContextMenu(hWnd);
        } else if (LOWORD(lParam) == WM_MBUTTONUP) {
            ProcessNetworkReset();
        }
        return 0;
    } else if (msg == WM_UPDATE_TRAY_STATE) {
        AddOrUpdateTrayIcon(hWnd, (BOOL)wParam, FALSE);
        return 0;
    } else if (msg == WM_TRIGGER_PING) {
        // wParam=0: normal re-enable — 6s settle for DHCP/init.
        // wParam=1: post-blackout-reset — 15s settle; adapter cycle + DHCP needs more time.
        UINT settleMs = (wParam == 1) ? 15000 : 6000;
        SetTimer(hWnd, DNS_RECOVERY_TIMER_ID, settleMs, nullptr);
        return 0;
    } else if (msg == WM_SETTINGS_CHANGED) {
        // Re-read settings on the tray thread
        DWORD newIp = 0;
        {
            auto dnsIp = WindhawkUtils::StringSetting::make(L"dnsServer");
            if (dnsIp.get()[0]) {
                if (InetPtonW(AF_INET, dnsIp.get(), &newIp) != 1) {
                    Wh_Log(L"Invalid DNS server IP '%s' — DNS monitoring disabled", dnsIp.get());
                    newIp = 0;
                }
            }
        }
        
        InterlockedExchange(&g_dnsServerIp, newIp);
        int intervalSec = Wh_GetIntSetting(L"pingInterval");
        if (intervalSec < MIN_PING_INTERVAL_SEC) intervalSec = MIN_PING_INTERVAL_SEC;
        g_pingIntervalMs = (DWORD)intervalSec * 1000;

        KillTimer(hWnd, DNS_PING_TIMER_ID);
        if (newIp != 0) {
            SetTimer(hWnd, DNS_PING_TIMER_ID, g_pingIntervalMs, nullptr);
            // Immediate first ping
            OnDnsPingTimer(hWnd);
        }
        AddOrUpdateTrayIcon(hWnd, (BOOL)(g_networkIsUp == 1), FALSE);
        return 0;
    } else if (msg == WM_TIMER) {
        if (wParam == DNS_PING_TIMER_ID) {
            OnDnsPingTimer(hWnd);
        } else if (wParam == DNS_RECOVERY_TIMER_ID) {
            KillTimer(hWnd, DNS_RECOVERY_TIMER_ID);
            OnDnsPingTimer(hWnd);
        }
        return 0;
    } else if (msg == WM_CLOSE) {
        KillTimer(hWnd, DNS_PING_TIMER_ID);
        KillTimer(hWnd, DNS_RECOVERY_TIMER_ID);
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = hWnd;
        nid.uID = TRAY_ICON_ID;
        nid.uFlags = NIF_GUID;
        nid.guidItem = NETTOGGLE_TRAY_GUID;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DestroyWindow(hWnd);
        return 0;
    } else if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        Wh_Log(L"Explorer restarted — re-adding tray icon");
        AddOrUpdateTrayIcon(hWnd, (BOOL)(g_networkIsUp == 1), TRUE);
        if (g_dnsServerIp != 0) OnDnsPingTimer(hWnd);  // verify state asynchronously
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ==============================================================================
// Feature G — Event-Driven Adapter Watch Thread
// ==============================================================================

DWORD WINAPI NetWatchThreadProc(LPVOID) {
    Wh_Log(L"NetWatch thread started");
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!hEvent) {
        Wh_Log(L"NetWatch: CreateEvent failed");
        return 1;
    }

    while (true) {
        HANDLE notifyHandle = nullptr;
        OVERLAPPED ov = {};
        ov.hEvent = hEvent;

        DWORD nacRet = NotifyAddrChange(&notifyHandle, &ov);
        if (nacRet != ERROR_IO_PENDING && nacRet != NO_ERROR) {
            Wh_Log(L"NetWatch: NotifyAddrChange failed (%d) — polling for %ds then retrying",
                   nacRet, (NETWATCH_POLL_RETRIES * NETWATCH_POLL_INTERVAL) / 1000);
            // Bounded fallback: poll NETWATCH_POLL_RETRIES × NETWATCH_POLL_INTERVAL,
            // then retry NotifyAddrChange so event-driven mode is restored after adapters recover.
            // Skip polls while a toggle/reset is in flight to avoid Yellow→Red flicker.
            bool shutdown = false;
            for (DWORD i = 0; i < NETWATCH_POLL_RETRIES; i++) {
                DWORD r = WaitForSingleObject(g_shutdownEvent, NETWATCH_POLL_INTERVAL);
                if (r == WAIT_OBJECT_0) { shutdown = true; break; }
                if (g_isProcessingClick != 0) continue;
                BOOL newState = CheckActualNetworkState();
                LONG oldState = g_networkIsUp;
                InterlockedExchange(&g_networkIsUp, newState ? 1 : 0);
                if (newState != (oldState == 1) && g_trayHwnd) {
                    PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)newState, 0);
                    if (newState) PostMessageW(g_trayHwnd, WM_TRIGGER_PING, 0, 0);
                }
            }
            if (shutdown) {
                CloseHandle(hEvent);
                return 0;
            }
            Wh_Log(L"NetWatch: retrying NotifyAddrChange after polling fallback");
            continue;
        }

        HANDLE waits[2] = { hEvent, g_shutdownEvent };
        DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);

        if (r == WAIT_OBJECT_0) {
            // Adapter state changed externally.
            // Skip while a toggle/reset is in flight — the worker owns g_networkIsUp
            // during that window and will post the definitive state when done.
            CloseHandle(notifyHandle);
            if (g_isProcessingClick == 0) {
                BOOL newState = CheckActualNetworkState();
                InterlockedExchange(&g_networkIsUp, newState ? 1 : 0);
                if (g_trayHwnd) {
                    PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)newState, 0);
                    if (newState) {
                        PostMessageW(g_trayHwnd, WM_TRIGGER_PING, 0, 0);
                    }
                }
            }
        } else {
            // Shutdown signal
            if (notifyHandle) {
                CancelIPChangeNotify(&ov);
                // The handle is automatically closed by the system, no need to CloseHandle
            }
            break;
        }
    }

    CloseHandle(hEvent);
    Wh_Log(L"NetWatch thread exiting");
    return 0;
}

// ==============================================================================
// Tray Thread
// ==============================================================================

DWORD WINAPI TrayThreadProc(LPVOID) {
    Wh_Log(L"Tray thread started");

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"TrayThread: CoInitializeEx failed (0x%X)", hrCo);
        return 1;
    }
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    BOOL initialState = CheckActualNetworkState();
    InterlockedExchange(&g_networkIsUp, initialState ? 1 : 0);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"NetToggleWindowClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        LogLastError(L"RegisterClassW");
        if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"Net-Toggle",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hInstance, nullptr
    );

    if (!hWnd) {
        LogLastError(L"CreateWindowExW");
        UnregisterClassW(wc.lpszClassName, g_hInstance);
        if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();
        return 1;
    }

    g_trayHwnd = hWnd;

    // Unique AUMID so the OS doesn't group this icon with Windhawk's main window.
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt = VT_LPWSTR;
        var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (var.pwszVal) {
            wcscpy_s(var.pwszVal, MAX_PATH, L"BlackPaw.NetToggle");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit();
        pps->Release();
    }

    AddOrUpdateTrayIcon(hWnd, initialState, TRUE);
    Wh_Log(L"Tray icon installed. Initial state: %s", initialState ? L"ON" : L"OFF");

    // Start DNS ping timer if configured
    if (g_dnsServerIp != 0) {
        SetTimer(hWnd, DNS_PING_TIMER_ID, g_pingIntervalMs, nullptr);
        // Immediate first ping
        OnDnsPingTimer(hWnd);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"Tray message loop ending");

    HICON oldIcon = (HICON)InterlockedExchangePointer((PVOID*)&g_currentIcon, nullptr);
    if (oldIcon) {
        DestroyIcon(oldIcon);
    }

    UnregisterClassW(wc.lpszClassName, g_hInstance);
    InterlockedExchange(&g_trayIconInstalled, 0);
    g_trayHwnd = nullptr;
    if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();

    return 0;
}

// ==============================================================================
// TOOL MOD IMPLEMENTATION
// ==============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"Net-Toggle Mod Init");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Wh_Log(L"WSAStartup failed");
        return FALSE;
    }

    g_hInstance = GetModuleHandleW(nullptr);
    if (!g_hInstance) {
        Wh_Log(L"Failed to get module handle");
        return FALSE;
    }

    Wh_Log(L"Using WiFi-style arc icon");

    // Enable dark mode for context menus app-wide
    {
        HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
        if (ux) {
            using FnSetMode = void(WINAPI*)(int);
            using FnAllow   = bool(WINAPI*)(bool);
            if (auto f = (FnSetMode)GetProcAddress(ux, MAKEINTRESOURCEA(135)))
                f(1);
            else if (auto f = (FnAllow)GetProcAddress(ux, MAKEINTRESOURCEA(132)))
                f(true);
        }
    }

    // Windhawk executable path (for "Open Windhawk" menu item)
    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    // Load Windhawk icon from ddores.dll for the context menu
    UINT sysLen = GetSystemDirectoryW(g_ddoresDllPath, MAX_PATH);
    if (sysLen > 0 && sysLen < MAX_PATH - 12)
        lstrcatW(g_ddoresDllPath, L"\\ddores.dll");
    else
        lstrcpyW(g_ddoresDllPath, L"ddores.dll");

    int whIconIndices[] = {98, 94, 95, 6};
    for (int idx : whIconIndices) {
        ExtractIconExW(g_ddoresDllPath, idx, nullptr, &g_hWindHawkIcon, 1);
        if (g_hWindHawkIcon) break;
    }
    if (g_hWindHawkIcon) {
        ICONINFO ii = {};
        if (GetIconInfo(g_hWindHawkIcon, &ii)) {
            g_hWindHawkBmp = ii.hbmColor ? ii.hbmColor : ii.hbmMask;
            if (ii.hbmColor && ii.hbmMask) DeleteObject(ii.hbmMask);
        }
    }

    // Read initial settings
    {
        DWORD newIp = 0;
        auto dnsIp = WindhawkUtils::StringSetting::make(L"dnsServer");
        if (dnsIp.get()[0]) {
            if (InetPtonW(AF_INET, dnsIp.get(), &newIp) != 1) {
                Wh_Log(L"Invalid DNS server IP '%s' — DNS monitoring disabled", dnsIp.get());
                newIp = 0;
            } else {
                Wh_Log(L"DNS server configured: %s", dnsIp.get());
            }
        } else {
            Wh_Log(L"No DNS server configured — DNS monitoring disabled");
        }
        InterlockedExchange(&g_dnsServerIp, newIp);
    }

    int intervalSec = Wh_GetIntSetting(L"pingInterval");
    if (intervalSec < MIN_PING_INTERVAL_SEC) intervalSec = MIN_PING_INTERVAL_SEC;
    g_pingIntervalMs = (DWORD)intervalSec * 1000;

    // Create shutdown event (manual-reset, initially non-signalled)
    g_shutdownEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_shutdownEvent) {
        LogLastError(L"CreateEvent(shutdownEvent)");
        return FALSE;
    }

    // Start tray thread
    DWORD threadId = 0;
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, &threadId);
    if (!g_trayThread) {
        LogLastError(L"CreateThread(tray)");
        CloseHandle(g_shutdownEvent);
        g_shutdownEvent = nullptr;
        return FALSE;
    }

    // Start net watch thread
    g_netWatchThread = CreateThread(nullptr, 0, NetWatchThreadProc, nullptr, 0, &threadId);
    if (!g_netWatchThread) {
        LogLastError(L"CreateThread(netWatch)");
        // Non-fatal — notify watch is a best-effort feature
        g_netWatchThread = nullptr;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Net-Toggle Mod Uninit");

    // Step 1: signal shutdown
    if (g_shutdownEvent) {
        SetEvent(g_shutdownEvent);
    }

    // Step 2: close tray window (triggers PostQuitMessage)
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_CLOSE, 0, 0);
    }

    // Step 3: wait for threads
    HANDLE waitThreads[3] = {};
    DWORD waitCount = 0;
    if (g_trayThread) waitThreads[waitCount++] = g_trayThread;
    if (g_netWatchThread) waitThreads[waitCount++] = g_netWatchThread;
    if (g_activeWorkerThread) waitThreads[waitCount++] = g_activeWorkerThread;

    if (waitCount > 0) {
        Wh_Log(L"Waiting for %d threads to exit...", waitCount);
        DWORD waitResult = WaitForMultipleObjects(waitCount, waitThreads, TRUE, 5000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Threads did not exit in time; ExitProcess will clean up");
        }
    }

    // Step 4: cleanup handles
    if (g_trayThread) {
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }
    if (g_netWatchThread) {
        CloseHandle(g_netWatchThread);
        g_netWatchThread = nullptr;
    }
    if (g_shutdownEvent) {
        CloseHandle(g_shutdownEvent);
        g_shutdownEvent = nullptr;
    }
    if (g_activeWorkerThread) {
        CloseHandle(g_activeWorkerThread);
        g_activeWorkerThread = nullptr;
    }

    // Step 5: destroy current icon safely
    HICON oldIcon = (HICON)InterlockedExchangePointer((PVOID*)&g_currentIcon, nullptr);
    if (oldIcon) {
        DestroyIcon(oldIcon);
    }

    if (g_hWindHawkBmp)  { DeleteObject(g_hWindHawkBmp);  g_hWindHawkBmp  = nullptr; }
    if (g_hWindHawkIcon) { DestroyIcon(g_hWindHawkIcon);  g_hWindHawkIcon = nullptr; }

    WSACleanup();
    Wh_Log(L"Net-Toggle Mod Uninit complete");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
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
