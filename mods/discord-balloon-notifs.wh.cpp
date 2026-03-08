// ==WindhawkMod==
// @id              discord-balloon-notifs
// @name            Discord Balloon Notifications
// @description     Converts Discord toast notifications to classic Windows balloon notifications
// @version         1.0
// @author          repensky
// @github          https://github.com/repensky
// @include         Discord.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -lwininet

// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Discord Balloon Notifications
Converts Discord's modern Windows toast notifications into balloon tips that appear from the system tray using Shell_NotifyIconW.

![Picture](https://raw.githubusercontent.com/repensky/local-wh-mods/refs/heads/main/image.png)

# WARNING!
The use of third party Discord clients with this mod is **NOT** supported! This mod is designed for the official Discord app. It may or may not function properly with unofficial Discord clients. However some are reported to work fine with Vesktop clients. Try on your own risk.

Enable this mod before starting Discord. If Discord is already running when you enable the mod, restart Discord manually. Otherwise, Discord may **crash** if not restarted on next notification.

## Requirement:
Ensure EnableLegacyBalloonNotifications is set to 1 in ``HKEY_CURRENT_USER\SOFTWARE\Policies\Microsoft\Windows\Explorer\EnableLegacyBalloonNotifications``

## Features:
- Notification sender and message is shown in the balloon
- Optionally shows the sender's profile picture as an icon in the balloon
- Configurable PFP icon size (Small 16px, Medium 24px, Large 32px)
- Clicking the balloon or tray icon brings Discord to the foreground
- Tray icon automatically removed when Discord exits
- Emoji decoding with an option to use basic glyphs supported by the font
- Right clicking on the tray icon features exiting Discord

## Note:
This is tested on **Windows 10 21H2 IoT Enterprise LTSC**. It may or may not function properly on other Windows versions.
**Windows 11** versions may need to use **ExplorerPatcher** for notifications to be shown in a balloon format.

## Note 2:
Balloon display duration is controlled by Windows.
To change, navigate to **Control Panel > Ease of Access Center > Use the computer without a display > How long should Windows notification dialog boxes stay open**

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepBasicEmoji: true
  $name: Use native unicode for supported emojis
  $description: When enabled, supported emojis are displayed as their Unicode symbols instead of being converted to shortcode format.
- showProfilePicture: true
  $name: Show profile pictures in balloon
  $description: Display the sender's Discord profile picture as an icon in the balloon notification
- iconSize: small
  $name: Profile picture size
  $description: Size of the profile picture icon in the balloon (only applies when profile pictures are enabled)
  $options:
  - small: Small (16x16)
  - medium: Medium (24x24)
  - large: Large (32x32)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <shellapi.h>
#include <roapi.h>
#include <winstring.h>
#include <tlhelp32.h>
#include <cwchar>
#include <wininet.h>

// Settings

enum IconSizeOption {
    ICON_SIZE_SMALL,
    ICON_SIZE_MEDIUM,
    ICON_SIZE_LARGE
};

struct {
    bool showProfilePicture;
    IconSizeOption iconSize;
    bool keepBasicEmoji;
} g_settings;

static void LoadSettings() {
    g_settings.showProfilePicture = Wh_GetIntSetting(L"showProfilePicture", 1) != 0;
    g_settings.keepBasicEmoji = Wh_GetIntSetting(L"keepBasicEmoji", 1) != 0;
    
    LPCWSTR iconSizeSetting = Wh_GetStringSetting(L"iconSize");
    if (wcscmp(iconSizeSetting, L"small") == 0) {
        g_settings.iconSize = ICON_SIZE_SMALL;
    } else if (wcscmp(iconSizeSetting, L"medium") == 0) {
        g_settings.iconSize = ICON_SIZE_MEDIUM;
    } else {
        g_settings.iconSize = ICON_SIZE_LARGE;
    }
    Wh_FreeStringSetting(iconSizeSetting);
}

// Helpers

#ifndef MIN_VAL
#define MIN_VAL(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Discord Process Monitor

static HANDLE g_hMonitorThread = nullptr;
static volatile bool g_stopMonitor = false;

static bool IsAnyDiscordRunning() {
    bool found = false;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return true;
    
    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);
    
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"Discord.exe") == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }
    
    CloseHandle(hSnap);
    return found;
}

// Find and click Discord's own tray icon to wake it up

struct TrayButtonInfo {
    HWND hToolbar;
    int buttonIndex;
    DWORD ownerPid;
};

static void FocusDiscordWindow() {
    WCHAR discordPath[MAX_PATH] = {};
    
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = {};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, L"Discord.exe") == 0) {
                    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                    if (hProc) {
                        DWORD pathSize = MAX_PATH;
                        if (QueryFullProcessImageNameW(hProc, 0, discordPath, &pathSize)) {
                            CloseHandle(hProc);
                            break;
                        }
                        CloseHandle(hProc);
                    }
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
    
    if (discordPath[0]) {
        Wh_Log(L"Launching Discord: %s", discordPath);
        ShellExecuteW(nullptr, nullptr, discordPath, nullptr, nullptr, SW_SHOWNORMAL);
    } else {
        // Fallback: try common paths or just "Discord"
        Wh_Log(L"Discord path not found, trying shell launch");
        ShellExecuteW(nullptr, nullptr, L"Discord", nullptr, nullptr, SW_SHOWNORMAL);
    }
}

// Tray Constructor
#define WM_BALLOON_SHOW   (WM_USER + 300)
#define WM_TRAY_CALLBACK  (WM_USER + 200)
#define WM_CHECK_DISCORD  (WM_USER + 301)
#define WM_ADD_TRAY_ICON  (WM_USER + 302)
#define IDM_EXIT_DISCORD  40001

static HWND g_hBalloonWnd = nullptr;
static bool g_iconAdded = false;
static CRITICAL_SECTION g_cs;
static HICON g_hAppIcon = nullptr;
static HANDLE g_hThread = nullptr;
static UINT g_wmTaskbarCreated = 0;

static WCHAR g_pendingTitle[256] = {};
static WCHAR g_pendingBody[512] = {};

static WCHAR g_lastImagePath[MAX_PATH] = {};
static HICON g_lastNotifIcon = nullptr;

static HANDLE g_hTrayMutex = nullptr;
static bool g_ownsTray = false;
static const WCHAR* TRAY_MUTEX_NAME = L"Local\\WindhawkDiscordBalloonTrayMutex";

static void EnsureTrayIcon() {
    if (!g_hBalloonWnd) return;
    if (g_iconAdded) return;
    if (!IsAnyDiscordRunning()) return;
    
    // Only one process creates the tray icon
    if (!g_ownsTray) {
        if (!g_hTrayMutex) {
            g_hTrayMutex = CreateMutexW(nullptr, FALSE, TRAY_MUTEX_NAME);
            if (!g_hTrayMutex) return;
        }
        
        DWORD result = WaitForSingleObject(g_hTrayMutex, 0);
        if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED) {
            g_ownsTray = true;
            Wh_Log(L"Acquired tray mutex (PID %lu)", GetCurrentProcessId());
        } else {
            // Another process owns it, don't create icon
            return;
        }
    }
    
    // Check if icon already exists (another process might have created it)
    NOTIFYICONDATAW nidCheck = {};
    nidCheck.cbSize = sizeof(nidCheck);
    nidCheck.hWnd = g_hBalloonWnd;
    nidCheck.uID = 1;
    nidCheck.uFlags = NIF_STATE;
    
    // Small delay to prevent race condition on first notification
    static bool s_firstTime = true;
    if (s_firstTime) {
        s_firstTime = false;
        Sleep(100);
    }
    
    NOTIFYICONDATAW nid;
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = g_hBalloonWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    nid.hIcon = g_hAppIcon ? g_hAppIcon : LoadIconW(nullptr, IDI_APPLICATION);
    wcsncpy(nid.szTip, L"Discord", ARRAYSIZE(nid.szTip) - 1);
    nid.szTip[ARRAYSIZE(nid.szTip) - 1] = L'\0';

    if (Shell_NotifyIconW(NIM_ADD, &nid)) {
        g_iconAdded = true;
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
        Wh_Log(L"Tray icon added (owner PID %lu)", GetCurrentProcessId());
    } else {
        // Icon might already exist, try modify instead
        DWORD err = GetLastError();
        Wh_Log(L"NIM_ADD failed (error %lu), trying NIM_MODIFY", err);
    }
}

// GDI+ PNG to HICON

typedef int (__stdcall *GdiplusStartup_t)(ULONG_PTR*, const void*, void*);
typedef void (__stdcall *GdiplusShutdown_t)(ULONG_PTR);
typedef int (__stdcall *GdipCreateBitmapFromFile_t)(const WCHAR*, void**);
typedef int (__stdcall *GdipCreateHICONFromBitmap_t)(void*, HICON*);
typedef int (__stdcall *GdipDisposeImage_t)(void*);
typedef int (__stdcall *GdipCreateBitmapFromScan0_t)(INT, INT, INT, INT, BYTE*, void**);
typedef int (__stdcall *GdipGetImageGraphicsContext_t)(void*, void**);
typedef int (__stdcall *GdipDrawImageRectI_t)(void*, void*, INT, INT, INT, INT);
typedef int (__stdcall *GdipDeleteGraphics_t)(void*);
typedef int (__stdcall *GdipSetInterpolationMode_t)(void*, int);
typedef int (__stdcall *GdipGraphicsClear_t)(void*, UINT);

static HICON LoadPngAsIcon(const WCHAR* path, int targetSize, int drawSize, int offsetX, int offsetY) {
    if (!path || !path[0]) return nullptr;

    DWORD attrs = GetFileAttributesW(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) return nullptr;

    HMODULE hGdiPlus = LoadLibraryW(L"gdiplus.dll");
    if (!hGdiPlus) return nullptr;

    auto pStartup = (GdiplusStartup_t)GetProcAddress(hGdiPlus, "GdiplusStartup");
    auto pShutdown = (GdiplusShutdown_t)GetProcAddress(hGdiPlus, "GdiplusShutdown");
    auto pFromFile = (GdipCreateBitmapFromFile_t)GetProcAddress(hGdiPlus, "GdipCreateBitmapFromFile");
    auto pCreateHICON = (GdipCreateHICONFromBitmap_t)GetProcAddress(hGdiPlus, "GdipCreateHICONFromBitmap");
    auto pDispose = (GdipDisposeImage_t)GetProcAddress(hGdiPlus, "GdipDisposeImage");
    auto pFromScan0 = (GdipCreateBitmapFromScan0_t)GetProcAddress(hGdiPlus, "GdipCreateBitmapFromScan0");
    auto pGetGfx = (GdipGetImageGraphicsContext_t)GetProcAddress(hGdiPlus, "GdipGetImageGraphicsContext");
    auto pDrawRect = (GdipDrawImageRectI_t)GetProcAddress(hGdiPlus, "GdipDrawImageRectI");
    auto pDelGfx = (GdipDeleteGraphics_t)GetProcAddress(hGdiPlus, "GdipDeleteGraphics");
    auto pSetInterp = (GdipSetInterpolationMode_t)GetProcAddress(hGdiPlus, "GdipSetInterpolationMode");
    auto pClear = (GdipGraphicsClear_t)GetProcAddress(hGdiPlus, "GdipGraphicsClear");

    if (!pStartup || !pShutdown || !pFromFile || !pCreateHICON || !pDispose || 
        !pFromScan0 || !pGetGfx || !pDrawRect || !pDelGfx) {
        FreeLibrary(hGdiPlus);
        return nullptr;
    }

    struct { UINT32 ver; void* cb; BOOL noThread; BOOL noCodecs; } input = {1, nullptr, FALSE, FALSE};
    ULONG_PTR token = 0;

    if (pStartup(&token, &input, nullptr) != 0) {
        FreeLibrary(hGdiPlus);
        return nullptr;
    }

    HICON hIcon = nullptr;
    void* pBitmap = nullptr;

    if (pFromFile(path, &pBitmap) == 0 && pBitmap) {
        void* pTarget = nullptr;
        if (pFromScan0(targetSize, targetSize, 0, 0x0026200A, nullptr, &pTarget) == 0 && pTarget) {
            void* pGfx = nullptr;
            if (pGetGfx(pTarget, &pGfx) == 0 && pGfx) {
                if (pClear) pClear(pGfx, 0x00000000);
                if (pSetInterp) pSetInterp(pGfx, 7);
                pDrawRect(pGfx, pBitmap, offsetX, offsetY, drawSize, drawSize);
                pDelGfx(pGfx);
            }
            pCreateHICON(pTarget, &hIcon);
            pDispose(pTarget);
        }
        pDispose(pBitmap);
    }

    pShutdown(token);
    return hIcon;
}

static HICON LoadPngAsIconSimple(const WCHAR* path, int size) {
    return LoadPngAsIcon(path, size, size, 0, 0);
}

// Ellipsis helper

static void TruncateWithEllipsis(WCHAR* dest, const WCHAR* src, int maxLen) {
    if (!dest || !src || maxLen <= 0) return;
    
    int srcLen = (int)wcslen(src);
    if (srcLen <= maxLen) {
        wcsncpy(dest, src, maxLen);
        dest[MIN_VAL(srcLen, maxLen)] = L'\0';
    } else {
        if (maxLen <= 3) {
            for (int i = 0; i < maxLen; i++) dest[i] = L'.';
            dest[maxLen] = L'\0';
        } else {
            wcsncpy(dest, src, maxLen - 3);
            dest[maxLen - 3] = L'.';
            dest[maxLen - 2] = L'.';
            dest[maxLen - 1] = L'.';
            dest[maxLen] = L'\0';
        }
    }
}

// Balloon Constructor

static void DoShowBalloon(const WCHAR* title, const WCHAR* body) {
    if (!g_hBalloonWnd) return;
    EnsureTrayIcon();
    if (!g_iconAdded || !g_ownsTray) return;

    if (g_lastNotifIcon) {
        DestroyIcon(g_lastNotifIcon);
        g_lastNotifIcon = nullptr;
    }

    bool useCustomIcon = false;
    bool useLargeIconFlag = false;
    
    if (g_settings.showProfilePicture && g_lastImagePath[0]) {
        switch (g_settings.iconSize) {
            case ICON_SIZE_SMALL:
                g_lastNotifIcon = LoadPngAsIconSimple(g_lastImagePath, 16);
                useLargeIconFlag = false;
                break;
            case ICON_SIZE_MEDIUM:
                g_lastNotifIcon = LoadPngAsIcon(g_lastImagePath, 32, 24, 4, 0);
                useLargeIconFlag = true;
                break;
            case ICON_SIZE_LARGE:
            default:
                g_lastNotifIcon = LoadPngAsIconSimple(g_lastImagePath, 32);
                useLargeIconFlag = true;
                break;
        }
        if (g_lastNotifIcon) useCustomIcon = true;
    }

    NOTIFYICONDATAW nid;
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = g_hBalloonWnd;
    nid.uID = 1;
    nid.uFlags = NIF_INFO | NIF_TIP | NIF_SHOWTIP;
    wcsncpy(nid.szTip, L"Discord", ARRAYSIZE(nid.szTip) - 1);
    nid.szTip[ARRAYSIZE(nid.szTip) - 1] = L'\0';

    if (useCustomIcon) {
        nid.dwInfoFlags = NIIF_USER;
        if (useLargeIconFlag) nid.dwInfoFlags |= NIIF_LARGE_ICON;
        nid.hBalloonIcon = g_lastNotifIcon;
    } else {
        nid.dwInfoFlags = NIIF_INFO;
    }

    TruncateWithEllipsis(nid.szInfoTitle,
        (title && title[0]) ? title : L"Discord",
        ARRAYSIZE(nid.szInfoTitle) - 1);
    TruncateWithEllipsis(nid.szInfo,
        (body && body[0]) ? body : L"New message",
        ARRAYSIZE(nid.szInfo) - 1);

    Shell_NotifyIconW(NIM_MODIFY, &nid);
    Wh_Log(L"Balloon: \"%s\" - \"%s\"", nid.szInfoTitle, nid.szInfo);
}

static void ShowBalloonNotification(const WCHAR* title, const WCHAR* body) {
    EnterCriticalSection(&g_cs);
    wcsncpy(g_pendingTitle, title ? title : L"Discord", ARRAYSIZE(g_pendingTitle) - 1);
    wcsncpy(g_pendingBody, body ? body : L"New message", ARRAYSIZE(g_pendingBody) - 1);
    LeaveCriticalSection(&g_cs);

    if (g_hBalloonWnd)
        PostMessageW(g_hBalloonWnd, WM_BALLOON_SHOW, 0, 0);
}

static void RemoveTrayIcon() {
    if (g_iconAdded && g_hBalloonWnd && g_ownsTray) {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = g_hBalloonWnd;
        nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        g_iconAdded = false;
        Wh_Log(L"Tray icon removed");
    }
    if (g_ownsTray && g_hTrayMutex) {
        ReleaseMutex(g_hTrayMutex);
        g_ownsTray = false;
    }
}

static void ShowTrayContextMenu(HWND hWnd) {
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;
    
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT_DISCORD, L"Exit Discord");
    
    // Required for the menu to work properly with tray icons
    SetForegroundWindow(hWnd);
    
    UINT cmd = TrackPopupMenu(hMenu, 
        TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
        pt.x, pt.y, 0, hWnd, nullptr);
    
    DestroyMenu(hMenu);
    
    if (cmd == IDM_EXIT_DISCORD) {
        // Find and terminate all Discord processes
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe = {};
            pe.dwSize = sizeof(pe);
            
            if (Process32FirstW(hSnap, &pe)) {
                do {
                    if (_wcsicmp(pe.szExeFile, L"Discord.exe") == 0) {
                        HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                        if (hProc) {
                            TerminateProcess(hProc, 0);
                            CloseHandle(hProc);
                            Wh_Log(L"Terminated Discord process (PID %lu)", pe.th32ProcessID);
                        }
                    }
                } while (Process32NextW(hSnap, &pe));
            }
            CloseHandle(hSnap);
        }
    }
    
    // Required to dismiss the menu if user clicks elsewhere
    PostMessageW(hWnd, WM_NULL, 0, 0);
}

static LRESULT CALLBACK BalloonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_wmTaskbarCreated && msg == g_wmTaskbarCreated) {
        Wh_Log(L">>> TaskbarCreated received! Explorer restarted. g_ownsTray=%d g_iconAdded=%d", 
               g_ownsTray, g_iconAdded);
        g_iconAdded = false;
        // Delay to let explorer fully initialize
        SetTimer(hWnd, 1, 2000, nullptr);
        return 0;
    }
    
    if (msg == WM_TIMER && wParam == 1) {
        KillTimer(hWnd, 1);
        if (g_ownsTray && IsAnyDiscordRunning() && !g_iconAdded) {
            Wh_Log(L"Re-adding tray icon after explorer restart");
            
            NOTIFYICONDATAW nid;
            ZeroMemory(&nid, sizeof(nid));
            nid.cbSize = sizeof(NOTIFYICONDATAW);
            nid.hWnd = g_hBalloonWnd;
            nid.uID = 1;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
            nid.uCallbackMessage = WM_TRAY_CALLBACK;
            nid.hIcon = g_hAppIcon ? g_hAppIcon : LoadIconW(nullptr, IDI_APPLICATION);
            wcsncpy(nid.szTip, L"Discord", ARRAYSIZE(nid.szTip) - 1);
            nid.szTip[ARRAYSIZE(nid.szTip) - 1] = L'\0';

            if (Shell_NotifyIconW(NIM_ADD, &nid)) {
                g_iconAdded = true;
                nid.uVersion = NOTIFYICON_VERSION_4;
                Shell_NotifyIconW(NIM_SETVERSION, &nid);
                Wh_Log(L"Tray icon restored after explorer restart");
            } else {
                Wh_Log(L"Failed to restore tray icon, will retry via monitor thread");
            }
        }
        return 0;
    }

    if (msg == WM_BALLOON_SHOW) {
        WCHAR title[256], body[512];
        EnterCriticalSection(&g_cs);
        wcscpy_s(title, g_pendingTitle);
        wcscpy_s(body, g_pendingBody);
        LeaveCriticalSection(&g_cs);
        DoShowBalloon(title, body);
        return 0;
    }

        if (msg == WM_TRAY_CALLBACK) {
        UINT event = LOWORD(lParam);
        if (event == NIN_BALLOONUSERCLICK ||
            event == WM_LBUTTONUP ||
            event == WM_LBUTTONDBLCLK)
        {
            FocusDiscordWindow();
        }
        else if (event == WM_RBUTTONUP) {
            ShowTrayContextMenu(hWnd);
        }
        return 0;
    }
    
    if (msg == WM_CHECK_DISCORD) {
        // Tray icon will be created when first notification arrives or by monitor thread
        Wh_Log(L"Balloon thread started (PID %lu)", GetCurrentProcessId());
        return 0;
    }
    
    if (msg == WM_ADD_TRAY_ICON) {
        if (!g_iconAdded) {
            EnsureTrayIcon();
        }
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static DWORD WINAPI DiscordMonitorThread(LPVOID) {
    DWORD lastExplorerPid = 0;
    
    // Get initial explorer PID
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray) {
        GetWindowThreadProcessId(hTray, &lastExplorerPid);
    }
    
    while (!g_stopMonitor) {
        Sleep(1000);  // Check every second
        if (g_stopMonitor) break;
        
        if (g_hBalloonWnd) {
            bool discordRunning = IsAnyDiscordRunning();
            
            // Check if explorer restarted by comparing Shell_TrayWnd PID
            HWND hTrayNow = FindWindowW(L"Shell_TrayWnd", nullptr);
            DWORD currentExplorerPid = 0;
            if (hTrayNow) {
                GetWindowThreadProcessId(hTrayNow, &currentExplorerPid);
            }
            
            if (currentExplorerPid != 0 && lastExplorerPid != 0 && 
                currentExplorerPid != lastExplorerPid) {
                // Explorer restarted!
                Wh_Log(L"Detected explorer restart (PID %lu -> %lu)", 
                       lastExplorerPid, currentExplorerPid);
                g_iconAdded = false;
                lastExplorerPid = currentExplorerPid;
                
                // Wait a bit for explorer to fully initialize
                Sleep(2000);
            }
            
            if (hTrayNow && lastExplorerPid == 0) {
                lastExplorerPid = currentExplorerPid;
            }
            
            if (g_ownsTray) {
                if (!discordRunning) {
                    PostMessageW(g_hBalloonWnd, WM_CHECK_DISCORD, 0, 0);
                } else if (!g_iconAdded) {
                    Wh_Log(L"Tray owner but icon not added, re-adding...");
                    PostMessageW(g_hBalloonWnd, WM_ADD_TRAY_ICON, 0, 0);
                }
            } else {
                if (discordRunning) {
                    PostMessageW(g_hBalloonWnd, WM_ADD_TRAY_ICON, 0, 0);
                }
            }
        }
    }
    return 0;
}

static DWORD WINAPI BalloonThread(LPVOID) {
    g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    Wh_Log(L"Registered TaskbarCreated message: %u", g_wmTaskbarCreated);
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = BalloonWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkDiscordBalloonClass";
    RegisterClassW(&wc);

    g_hBalloonWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,  // Don't show in taskbar
        wc.lpszClassName, 
        L"", 
        WS_POPUP,  // No frame
        0, 0, 0, 0,  // Zero size = invisible
        nullptr,  // No parent (NOT HWND_MESSAGE)
        nullptr, 
        wc.hInstance, 
        nullptr);

    if (!g_hBalloonWnd) {
        Wh_Log(L"Failed to create balloon window");
        return 1;
    }
    
    Wh_Log(L"Balloon window created: %p", g_hBalloonWnd);
    
    // Allow TaskbarCreated message through UIPI
    if (g_wmTaskbarCreated) {
        typedef BOOL (WINAPI *ChangeWindowMessageFilter_t)(UINT, DWORD);
        typedef BOOL (WINAPI *ChangeWindowMessageFilterEx_t)(HWND, UINT, DWORD, void*);
        
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            // Try ChangeWindowMessageFilterEx first
            auto pChangeFilterEx = (ChangeWindowMessageFilterEx_t)GetProcAddress(hUser32, "ChangeWindowMessageFilterEx");
            if (pChangeFilterEx) {
                if (pChangeFilterEx(g_hBalloonWnd, g_wmTaskbarCreated, 1 /*MSGFLT_ALLOW*/, nullptr)) {
                    Wh_Log(L"ChangeWindowMessageFilterEx succeeded");
                } else {
                    Wh_Log(L"ChangeWindowMessageFilterEx failed: %lu", GetLastError());
                }
            }
            
            // Also try the process-wide filter
            auto pChangeFilter = (ChangeWindowMessageFilter_t)GetProcAddress(hUser32, "ChangeWindowMessageFilter");
            if (pChangeFilter) {
                if (pChangeFilter(g_wmTaskbarCreated, 1 /*MSGFLT_ADD*/)) {
                    Wh_Log(L"ChangeWindowMessageFilter succeeded");
                } else {
                    Wh_Log(L"ChangeWindowMessageFilter failed: %lu", GetLastError());
                }
            }
        }
    }

    // Tray icon will be created when first notification arrives or by monitor thread
    Wh_Log(L"Balloon thread started (PID %lu)", GetCurrentProcessId());

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    RemoveTrayIcon();
    return 0;
}

// XML string parsing

static void DecodeXmlEntities(WCHAR* str) {
    if (!str) return;

    WCHAR* read = str;
    WCHAR* write = str;

    while (*read) {
        if (*read == L'&') {
            if (wcsncmp(read, L"&amp;", 5) == 0) {
                *write++ = L'&'; read += 5;
            } else if (wcsncmp(read, L"&lt;", 4) == 0) {
                *write++ = L'<'; read += 4;
            } else if (wcsncmp(read, L"&gt;", 4) == 0) {
                *write++ = L'>'; read += 4;
            } else if (wcsncmp(read, L"&apos;", 6) == 0) {
                *write++ = L'\''; read += 6;
            } else if (wcsncmp(read, L"&quot;", 6) == 0) {
                *write++ = L'"'; read += 6;
            } else if (wcsncmp(read, L"&#x", 3) == 0 || wcsncmp(read, L"&#X", 3) == 0) {
                WCHAR* afterNum = nullptr;
                unsigned long val = wcstoul(read + 3, &afterNum, 16);
                if (afterNum && *afterNum == L';' && val > 0 && val <= 0xFFFF) {
                    *write++ = (WCHAR)val;
                    read = afterNum + 1;
                } else {
                    *write++ = *read++;
                }
            } else if (wcsncmp(read, L"&#", 2) == 0) {
                WCHAR* afterNum = nullptr;
                unsigned long val = wcstoul(read + 2, &afterNum, 10);
                if (afterNum && *afterNum == L';' && val > 0 && val <= 0xFFFF) {
                    *write++ = (WCHAR)val;
                    read = afterNum + 1;
                } else {
                    *write++ = *read++;
                }
            } else {
                *write++ = *read++;
            }
        } else {
            *write++ = *read++;
        }
    }
    *write = L'\0';
}

// Emoji to Discord name mapping

struct DynamicEmojiEntry {
    WCHAR surrogates[16];  // The actual emoji Unicode string
    WCHAR name[64];        // Discord shortcode like ":smile:"
};

#define MAX_EMOJI_ENTRIES 8192

static DynamicEmojiEntry* g_dynamicEmojiTable = nullptr;
static volatile LONG g_emojiCount = 0;
static volatile LONG g_emojiTableReady = 0;

static WCHAR g_emojiCachePath[MAX_PATH] = {};
struct BasicGlyphEntry {
    WCHAR codepoint;  // Single BMP codepoint
};

#define MAX_BASIC_GLYPHS 512

static BasicGlyphEntry* g_basicGlyphTable = nullptr;
static volatile LONG g_basicGlyphCount = 0;
static volatile LONG g_basicGlyphReady = 0;
static WCHAR g_basicGlyphCachePath[MAX_PATH] = {};

static void GetBasicGlyphCachePath() {
    WCHAR winDir[MAX_PATH];
    GetWindowsDirectoryW(winDir, MAX_PATH);
    swprintf_s(g_basicGlyphCachePath, L"%s\\Temp\\windhawk_discord_basic_glyphs.json", winDir);
}

static bool IsBasicGlyphCacheFresh() {
    if (g_basicGlyphCachePath[0] == L'\0') return false;
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (!GetFileAttributesExW(g_basicGlyphCachePath, GetFileExInfoStandard, &attrs)) return false;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER fileTime, currentTime;
    fileTime.LowPart = attrs.ftLastWriteTime.dwLowDateTime;
    fileTime.HighPart = attrs.ftLastWriteTime.dwHighDateTime;
    currentTime.LowPart = now.dwLowDateTime;
    currentTime.HighPart = now.dwHighDateTime;
    ULONGLONG sevenDays = (ULONGLONG)7 * 24 * 60 * 60 * 10000000ULL;
    return (currentTime.QuadPart - fileTime.QuadPart) < sevenDays;
}

static char* DownloadBasicGlyphJson(DWORD* outSize) {
    *outSize = 0;
    
    HINTERNET hInternet = InternetOpenW(L"WindhawkDiscordBalloon/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) return nullptr;
    
    HINTERNET hUrl = InternetOpenUrlW(hInternet,
        L"https://raw.githubusercontent.com/repensky/local-wh-mods/refs/heads/main/discord-basic-glyphs.json", //Downloads basic glyphs list
        nullptr, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE,
        0);
    
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return nullptr;
    }
    
    DWORD capacity = 32 * 1024;
    char* buf = (char*)HeapAlloc(GetProcessHeap(), 0, capacity);
    if (!buf) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return nullptr;
    }
    
    DWORD totalRead = 0;
    DWORD bytesRead;
    char tmp[4096];
    while (InternetReadFile(hUrl, tmp, sizeof(tmp), &bytesRead) && bytesRead > 0) {
        if (totalRead + bytesRead >= capacity) {
            capacity *= 2;
            char* newBuf = (char*)HeapReAlloc(GetProcessHeap(), 0, buf, capacity);
            if (!newBuf) { HeapFree(GetProcessHeap(), 0, buf); buf = nullptr; break; }
            buf = newBuf;
        }
        if (buf) {
            memcpy(buf + totalRead, tmp, bytesRead);
            totalRead += bytesRead;
        }
    }
    
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    
    if (buf) {
        buf[totalRead] = '\0';
        *outSize = totalRead;
        Wh_Log(L"Downloaded basic glyph JSON: %lu bytes", totalRead);
    }
    return buf;
}

static void SaveBasicGlyphCache(const char* json, DWORD size) {
    if (g_basicGlyphCachePath[0] == L'\0' || !json) return;
    HANDLE hFile = CreateFileW(g_basicGlyphCachePath, GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hFile, json, size, &written, nullptr);
        CloseHandle(hFile);
    }
}

static char* LoadBasicGlyphCache(DWORD* outSize) {
    *outSize = 0;
    if (g_basicGlyphCachePath[0] == L'\0') return nullptr;
    HANDLE hFile = CreateFileW(g_basicGlyphCachePath, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return nullptr;
    DWORD fileSize = GetFileSize(hFile, nullptr);
    if (fileSize == 0 || fileSize > 1 * 1024 * 1024) {
        CloseHandle(hFile);
        return nullptr;
    }
    char* buf = (char*)HeapAlloc(GetProcessHeap(), 0, fileSize + 1);
    if (!buf) { CloseHandle(hFile); return nullptr; }
    DWORD bytesRead;
    if (!ReadFile(hFile, buf, fileSize, &bytesRead, nullptr)) {
        HeapFree(GetProcessHeap(), 0, buf);
        CloseHandle(hFile);
        return nullptr;
    }
    CloseHandle(hFile);
    buf[bytesRead] = '\0';
    *outSize = bytesRead;
    return buf;
}

static void ParseBasicGlyphJson(const char* json, DWORD jsonSize) {
    if (!json || !g_basicGlyphTable) return;
    
    // Format: { "glyphs": ["263A", "2639", "2764", ...] }
    const char* p = strstr(json, "\"glyphs\"");
    if (!p) { Wh_Log(L"No 'glyphs' key in basic glyph JSON"); return; }
    
    p += 8;
    while (*p == ' ' || *p == ':' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p != '[') { Wh_Log(L"Expected '[' after glyphs"); return; }
    p++;
    
    LONG count = 0;
    
    while (*p && count < MAX_BASIC_GLYPHS) {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') p++;
        if (*p == ']') break;
        
        if (*p == '"') {
            p++;
            char hex[8] = {};
            int hi = 0;
            while (*p && *p != '"' && hi < 6) {
                hex[hi++] = *p++;
            }
            hex[hi] = '\0';
            if (*p == '"') p++;
            
            unsigned long val = strtoul(hex, nullptr, 16);
            if (val > 0 && val <= 0xFFFF) {
                g_basicGlyphTable[count].codepoint = (WCHAR)val;
                count++;
            }
        } else {
            p++;
        }
    }
    
    InterlockedExchange(&g_basicGlyphCount, count);
    InterlockedExchange(&g_basicGlyphReady, 1);
    Wh_Log(L"Loaded %ld basic glyph entries", count);
}

static bool IsBasicEmojiGlyph(WCHAR c) {
    if (!InterlockedCompareExchange(&g_basicGlyphReady, 1, 1)) return false;
    
    LONG count = g_basicGlyphCount;
    for (LONG i = 0; i < count; i++) {
        if (g_basicGlyphTable[i].codepoint == c) return true;
    }
    return false;
}

static int IsBasicEmojiSequence(const WCHAR* str) {
    if (!str || !*str) return 0;
    
    WCHAR c = *str;
    if (!IsBasicEmojiGlyph(c)) return 0;
    
    int len = 1;
    while (str[len] >= 0xFE00 && str[len] <= 0xFE0F) len++;
    
    return len;
}

static const WCHAR* BASIC_GLYPH_DOWNLOAD_MUTEX_NAME = L"Global\\WindhawkDiscordBasicGlyphDownload";

static DWORD WINAPI BasicGlyphLoadThread(LPVOID) {
    GetBasicGlyphCachePath();
    
    char* json = nullptr;
    DWORD jsonSize = 0;
    
    if (IsBasicGlyphCacheFresh()) {
        json = LoadBasicGlyphCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseBasicGlyphJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
            return 0;
        }
        if (json) HeapFree(GetProcessHeap(), 0, json);
    }
    
    HANDLE hDownloadMutex = CreateMutexW(nullptr, FALSE, BASIC_GLYPH_DOWNLOAD_MUTEX_NAME);
    if (!hDownloadMutex) {
        json = LoadBasicGlyphCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseBasicGlyphJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else if (json) {
            HeapFree(GetProcessHeap(), 0, json);
        }
        return 0;
    }
    
    DWORD waitResult = WaitForSingleObject(hDownloadMutex, 30000);
    
    if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED) {
        if (IsBasicGlyphCacheFresh()) {
            json = LoadBasicGlyphCache(&jsonSize);
            if (json && jsonSize > 0) {
                Wh_Log(L"Another process already downloaded basic glyph cache");
                ParseBasicGlyphJson(json, jsonSize);
                HeapFree(GetProcessHeap(), 0, json);
                ReleaseMutex(hDownloadMutex);
                CloseHandle(hDownloadMutex);
                return 0;
            }
            if (json) HeapFree(GetProcessHeap(), 0, json);
        }
        
        Wh_Log(L"Downloading basic glyph JSON (PID %lu)", GetCurrentProcessId());
        json = DownloadBasicGlyphJson(&jsonSize);
        if (json && jsonSize > 0) {
            SaveBasicGlyphCache(json, jsonSize);
            ParseBasicGlyphJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else {
            if (json) HeapFree(GetProcessHeap(), 0, json);
            json = LoadBasicGlyphCache(&jsonSize);
            if (json && jsonSize > 0) {
                Wh_Log(L"Using stale basic glyph cache as fallback");
                ParseBasicGlyphJson(json, jsonSize);
                HeapFree(GetProcessHeap(), 0, json);
            } else {
                Wh_Log(L"No basic glyph data available");
                if (json) HeapFree(GetProcessHeap(), 0, json);
            }
        }
        
        ReleaseMutex(hDownloadMutex);
    } else {
        Wh_Log(L"Basic glyph download mutex timeout, waiting for cache...");
        Sleep(5000);
        json = LoadBasicGlyphCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseBasicGlyphJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else {
            Wh_Log(L"No basic glyph data available after wait");
            if (json) HeapFree(GetProcessHeap(), 0, json);
        }
    }
    
    CloseHandle(hDownloadMutex);
    return 0;
}

static void GetEmojiCachePath() {
    WCHAR winDir[MAX_PATH];
    GetWindowsDirectoryW(winDir, MAX_PATH);
    swprintf_s(g_emojiCachePath, L"%s\\Temp\\windhawk_discord_emojis.json", winDir);
}

// Download the JSON file from GitHub
static char* DownloadEmojiJson(DWORD* outSize) {
    *outSize = 0;
    
    HINTERNET hInternet = InternetOpenW(L"WindhawkDiscordBalloon/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) return nullptr;
    
    HINTERNET hUrl = InternetOpenUrlW(hInternet,
        L"https://raw.githubusercontent.com/repensky/local-wh-mods/refs/heads/main/discord-emojis.json", //Downloads Discord emoji database
        nullptr, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE,
        0);
    
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return nullptr;
    }
    
    DWORD capacity = 256 * 1024;
    char* buf = (char*)HeapAlloc(GetProcessHeap(), 0, capacity);
    if (!buf) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return nullptr;
    }
    
    DWORD totalRead = 0;
    DWORD bytesRead;
    char tmp[8192];
    while (InternetReadFile(hUrl, tmp, sizeof(tmp), &bytesRead) && bytesRead > 0) {
        if (totalRead + bytesRead >= capacity) {
            capacity *= 2;
            char* newBuf = (char*)HeapReAlloc(GetProcessHeap(), 0, buf, capacity);
            if (!newBuf) { HeapFree(GetProcessHeap(), 0, buf); buf = nullptr; break; }
            buf = newBuf;
        }
        if (buf) {
            memcpy(buf + totalRead, tmp, bytesRead);
            totalRead += bytesRead;
        }
    }
    
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    
    if (buf) {
        buf[totalRead] = '\0';
        *outSize = totalRead;
        Wh_Log(L"Downloaded emoji JSON: %lu bytes", totalRead);
    }
    return buf;
}

static void SaveEmojiCache(const char* json, DWORD size) {
    if (g_emojiCachePath[0] == L'\0' || !json) return;
    HANDLE hFile = CreateFileW(g_emojiCachePath, GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hFile, json, size, &written, nullptr);
        CloseHandle(hFile);
    }
}

static char* LoadEmojiCache(DWORD* outSize) {
    *outSize = 0;
    if (g_emojiCachePath[0] == L'\0') return nullptr;
    HANDLE hFile = CreateFileW(g_emojiCachePath, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return nullptr;
    DWORD fileSize = GetFileSize(hFile, nullptr);
    if (fileSize == 0 || fileSize > 10 * 1024 * 1024) {
        CloseHandle(hFile);
        return nullptr;
    }
    char* buf = (char*)HeapAlloc(GetProcessHeap(), 0, fileSize + 1);
    if (!buf) { CloseHandle(hFile); return nullptr; }
    DWORD bytesRead;
    if (!ReadFile(hFile, buf, fileSize, &bytesRead, nullptr)) {
        HeapFree(GetProcessHeap(), 0, buf);
        CloseHandle(hFile);
        return nullptr;
    }
    CloseHandle(hFile);
    buf[bytesRead] = '\0';
    *outSize = bytesRead;
    return buf;
}

static bool IsEmojiCacheFresh() {
    if (g_emojiCachePath[0] == L'\0') return false;
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (!GetFileAttributesExW(g_emojiCachePath, GetFileExInfoStandard, &attrs)) return false;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER fileTime, currentTime;
    fileTime.LowPart = attrs.ftLastWriteTime.dwLowDateTime;
    fileTime.HighPart = attrs.ftLastWriteTime.dwHighDateTime;
    currentTime.LowPart = now.dwLowDateTime;
    currentTime.HighPart = now.dwHighDateTime;
    ULONGLONG sevenDays = (ULONGLONG)7 * 24 * 60 * 60 * 10000000ULL;
    return (currentTime.QuadPart - fileTime.QuadPart) < sevenDays;
}

// UTF-8 decode helper: decode one codepoint from UTF-8, advance pointer
static unsigned int DecodeUtf8Char(const char*& p) {
    unsigned char c = (unsigned char)*p;
    unsigned int cp = 0;
    if (c < 0x80) {
        cp = c; p++;
    } else if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1F;
        cp = (cp << 6) | ((unsigned char)p[1] & 0x3F);
        p += 2;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0F;
        cp = (cp << 6) | ((unsigned char)p[1] & 0x3F);
        cp = (cp << 6) | ((unsigned char)p[2] & 0x3F);
        p += 3;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07;
        cp = (cp << 6) | ((unsigned char)p[1] & 0x3F);
        cp = (cp << 6) | ((unsigned char)p[2] & 0x3F);
        cp = (cp << 6) | ((unsigned char)p[3] & 0x3F);
        p += 4;
    } else {
        p++;
    }
    return cp;
}

// Write a Unicode codepoint as UTF-16 into a WCHAR buffer, return chars written
static int CodepointToUtf16(unsigned int cp, WCHAR* out) {
    if (cp <= 0xFFFF) {
        out[0] = (WCHAR)cp;
        return 1;
    } else if (cp <= 0x10FFFF) {
        cp -= 0x10000;
        out[0] = (WCHAR)(0xD800 | (cp >> 10));
        out[1] = (WCHAR)(0xDC00 | (cp & 0x3FF));
        return 2;
    }
    return 0;
}

// Convert a UTF-8 JSON string value to UTF-16, handling \uXXXX escapes
// src points AFTER the opening quote, we read until closing quote
static bool JsonStringToWide(const char*& p, WCHAR* out, int outMax) {
    int pos = 0;
    // p should be right after opening "
    while (*p && *p != '"' && pos < outMax - 2) {
        if (*p == '\\') {
            p++;
            switch (*p) {
                case '"': out[pos++] = L'"'; p++; break;
                case '\\': out[pos++] = L'\\'; p++; break;
                case '/': out[pos++] = L'/'; p++; break;
                case 'n': out[pos++] = L'\n'; p++; break;
                case 'r': out[pos++] = L'\r'; p++; break;
                case 't': out[pos++] = L'\t'; p++; break;
                case 'u': {
                    p++;  // skip 'u'
                    char hex[5] = {};
                    for (int i = 0; i < 4 && *p; i++) hex[i] = *p++;
                    unsigned long val = strtoul(hex, nullptr, 16);
                    
                    // Check for surrogate pair
                    if (val >= 0xD800 && val <= 0xDBFF && p[0] == '\\' && p[1] == 'u') {
                        p += 2;  // skip \u
                        char hex2[5] = {};
                        for (int i = 0; i < 4 && *p; i++) hex2[i] = *p++;
                        unsigned long val2 = strtoul(hex2, nullptr, 16);
                        if (val2 >= 0xDC00 && val2 <= 0xDFFF) {
                            out[pos++] = (WCHAR)val;
                            if (pos < outMax - 1) out[pos++] = (WCHAR)val2;
                        } else {
                            // Not a valid pair, emit first and rewind
                            int w = CodepointToUtf16((unsigned int)val, out + pos);
                            pos += w;
                        }
                    } else {
                        int w = CodepointToUtf16((unsigned int)val, out + pos);
                        pos += w;
                    }
                    break;
                }
                default: out[pos++] = (WCHAR)*p; p++; break;
            }
        } else {
            // Raw UTF-8 byte(s) -> decode to codepoint -> encode as UTF-16
            unsigned int cp = DecodeUtf8Char(p);
            int w = CodepointToUtf16(cp, out + pos);
            pos += w;
        }
    }
    if (*p == '"') p++;  // skip closing quote
    out[pos] = L'\0';
    return pos > 0;
}

// Skip a JSON value (string, number, object, array, bool, null)
static void SkipJsonValue(const char*& p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p == '"') {
        p++;  // skip opening quote
        while (*p && *p != '"') {
            if (*p == '\\') { p++; if (*p) p++; }
            else p++;
        }
        if (*p == '"') p++;
    } else if (*p == '[') {
        int depth = 1; p++;
        while (*p && depth > 0) {
            if (*p == '[') depth++;
            else if (*p == ']') depth--;
            else if (*p == '"') { p++; while (*p && *p != '"') { if (*p == '\\') { p++; if (*p) p++; } else p++; } if (*p == '"') p++; continue; }
            p++;
        }
    } else if (*p == '{') {
        int depth = 1; p++;
        while (*p && depth > 0) {
            if (*p == '{') depth++;
            else if (*p == '}') depth--;
            else if (*p == '"') { p++; while (*p && *p != '"') { if (*p == '\\') { p++; if (*p) p++; } else p++; } if (*p == '"') p++; continue; }
            p++;
        }
    } else {
        while (*p && *p != ',' && *p != '}' && *p != ']') p++;
    }
}

static void ParseEmojiJson(const char* json, DWORD jsonSize) {
    if (!json || !g_dynamicEmojiTable) return;
    
    // Find "emojis" key
    const char* p = strstr(json, "\"emojis\"");
    if (!p) { Wh_Log(L"No 'emojis' key in JSON"); return; }
    
    p += 8;  // skip "emojis"
    while (*p == ' ' || *p == ':' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p != '[') { Wh_Log(L"Expected '[' after emojis"); return; }
    p++;  // skip '['
    
    LONG count = 0;
    
    while (*p && count < MAX_EMOJI_ENTRIES) {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') p++;
        if (*p == ']') break;
        if (*p != '{') { p++; continue; }
        p++;  // skip '{'
        
        WCHAR firstName[64] = {};
        WCHAR surrogates[16] = {};
        bool gotName = false;
        bool gotSurrogates = false;
        
        while (*p && *p != '}') {
            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') p++;
            if (*p == '}') break;
            
            // Parse key
            if (*p != '"') { p++; continue; }
            p++;  // skip opening quote
            
            // Read key name inline
            char key[32] = {};
            int ki = 0;
            while (*p && *p != '"' && ki < 30) {
                if (*p == '\\') { p++; if (*p) key[ki++] = *p++; }
                else key[ki++] = *p++;
            }
            key[ki] = '\0';
            if (*p == '"') p++;
            
            while (*p == ' ' || *p == ':' || *p == '\t' || *p == '\n' || *p == '\r') p++;
            
            if (strcmp(key, "names") == 0) {
                // Parse array, grab first name only
                if (*p == '[') {
                    p++;
                    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
                    if (*p == '"') {
                        p++;  // skip opening quote
                        // Read name chars as ASCII into firstName
                        int ni = 0;
                        while (*p && *p != '"' && ni < 62) {
                            if (*p == '\\') { p++; if (*p) firstName[ni++] = (WCHAR)*p++; }
                            else firstName[ni++] = (WCHAR)*p++;
                        }
                        firstName[ni] = L'\0';
                        if (*p == '"') p++;
                        gotName = (ni > 0);
                    }
                    // Skip rest of array
                    int depth = 1;
                    while (*p && depth > 0) {
                        if (*p == '[') depth++;
                        else if (*p == ']') depth--;
                        else if (*p == '"') { p++; while (*p && *p != '"') { if (*p == '\\') { p++; if (*p) p++; } else p++; } if (*p == '"') p++; continue; }
                        p++;
                    }
                } else {
                    SkipJsonValue(p);
                }
            } else if (strcmp(key, "surrogates") == 0) {
                if (*p == '"') {
                    p++;  // skip opening quote
                    gotSurrogates = JsonStringToWide(p, surrogates, ARRAYSIZE(surrogates));
                } else {
                    SkipJsonValue(p);
                }
            } else {
                SkipJsonValue(p);
            }
        }
        if (*p == '}') p++;
        
        if (gotName && gotSurrogates && surrogates[0] && firstName[0]) {
            DynamicEmojiEntry& e = g_dynamicEmojiTable[count];
            wcscpy_s(e.surrogates, surrogates);
            // Format as :name:
            swprintf_s(e.name, L":%s:", firstName);
            count++;
        }
    }
    
    // Sort by surrogate length descending (simple bubble sort, runs once)
    for (LONG i = 0; i < count - 1; i++) {
        for (LONG j = 0; j < count - 1 - i; j++) {
            if (wcslen(g_dynamicEmojiTable[j].surrogates) < wcslen(g_dynamicEmojiTable[j+1].surrogates)) {
                DynamicEmojiEntry tmp = g_dynamicEmojiTable[j];
                g_dynamicEmojiTable[j] = g_dynamicEmojiTable[j+1];
                g_dynamicEmojiTable[j+1] = tmp;
            }
        }
    }
    
    // Use InterlockedExchange to ensure visibility
    InterlockedExchange(&g_emojiCount, count);
    InterlockedExchange(&g_emojiTableReady, 1);
    Wh_Log(L"Loaded %ld emoji entries from JSON", count);
}

static const WCHAR* EMOJI_DOWNLOAD_MUTEX_NAME = L"Global\\WindhawkDiscordEmojiDownload";

static DWORD WINAPI EmojiLoadThread(LPVOID) {
    GetEmojiCachePath();
    
    char* json = nullptr;
    DWORD jsonSize = 0;
    
    // Try fresh cache first - no lock needed for reading
    if (IsEmojiCacheFresh()) {
        json = LoadEmojiCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseEmojiJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
            return 0;
        }
        if (json) HeapFree(GetProcessHeap(), 0, json);
    }
    
    // Need to download - use a named mutex so only one process downloads
    HANDLE hDownloadMutex = CreateMutexW(nullptr, FALSE, EMOJI_DOWNLOAD_MUTEX_NAME);
    if (!hDownloadMutex) {
        // Can't create mutex, try stale cache
        json = LoadEmojiCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseEmojiJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else if (json) {
            HeapFree(GetProcessHeap(), 0, json);
        }
        return 0;
    }
    
    DWORD waitResult = WaitForSingleObject(hDownloadMutex, 30000);  // 30s timeout
    
    if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED) {
        // Got the lock - check if another process already downloaded while we waited
        if (IsEmojiCacheFresh()) {
            json = LoadEmojiCache(&jsonSize);
            if (json && jsonSize > 0) {
                Wh_Log(L"Another process already downloaded emoji cache");
                ParseEmojiJson(json, jsonSize);
                HeapFree(GetProcessHeap(), 0, json);
                ReleaseMutex(hDownloadMutex);
                CloseHandle(hDownloadMutex);
                return 0;
            }
            if (json) HeapFree(GetProcessHeap(), 0, json);
        }
        
        Wh_Log(L"Downloading emoji JSON (PID %lu)", GetCurrentProcessId());
        json = DownloadEmojiJson(&jsonSize);
        if (json && jsonSize > 0) {
            SaveEmojiCache(json, jsonSize);
            ParseEmojiJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else {
            if (json) HeapFree(GetProcessHeap(), 0, json);
            // Fallback to stale cache
            json = LoadEmojiCache(&jsonSize);
            if (json && jsonSize > 0) {
                Wh_Log(L"Using stale emoji cache as fallback");
                ParseEmojiJson(json, jsonSize);
                HeapFree(GetProcessHeap(), 0, json);
            } else {
                Wh_Log(L"No emoji data available");
                if (json) HeapFree(GetProcessHeap(), 0, json);
            }
        }
        
        ReleaseMutex(hDownloadMutex);
    } else {
        // Timeout or error - another process is probably downloading, wait and use cache
        Wh_Log(L"Emoji download mutex timeout, waiting for cache...");
        Sleep(5000);
        json = LoadEmojiCache(&jsonSize);
        if (json && jsonSize > 0) {
            ParseEmojiJson(json, jsonSize);
            HeapFree(GetProcessHeap(), 0, json);
        } else {
            Wh_Log(L"No emoji data available after wait");
            if (json) HeapFree(GetProcessHeap(), 0, json);
        }
    }
    
    CloseHandle(hDownloadMutex);
    return 0;
}

// Convert a single Unicode codepoint (U+10000+) to surrogate pair length
static int GetEmojiLength(const WCHAR* str) {
    if (!str || !*str) return 0;
    WCHAR c = *str;
    
    // Surrogate pair (emoji above U+FFFF)
    if (c >= 0xD800 && c <= 0xDBFF && *(str+1) >= 0xDC00 && *(str+1) <= 0xDFFF) {
        int len = 2;
        // Skip variation selectors (U+FE00-U+FE0F)
        while (str[len] >= 0xFE00 && str[len] <= 0xFE0F) len++;
        // Skip ZWJ (U+200D) and following emoji
        if (str[len] == 0x200D) {
            int next = GetEmojiLength(str + len + 1);
            if (next > 0) len += 1 + next;
        }
        return len;
    }
    
    // Single-char emoji in BMP
    if ((c >= 0x2600 && c <= 0x27BF) ||  // Misc symbols, dingbats
        (c >= 0x2700 && c <= 0x27BF) ||
        (c >= 0x2B00 && c <= 0x2BFF) ||
        (c >= 0x2000 && c <= 0x206F) ||  // General punctuation
        (c >= 0x2100 && c <= 0x21FF) ||  // Letterlike, arrows
        (c >= 0x2300 && c <= 0x23FF) ||  // Misc technical
        (c >= 0x25A0 && c <= 0x25FF) ||  // Geometric shapes
        (c >= 0x2900 && c <= 0x297F) ||  // Supplemental arrows
        (c == 0x200D) ||                  // ZWJ
        (c == 0x20E3) ||                  // Combining enclosing keycap
        (c >= 0xFE00 && c <= 0xFE0F) ||  // Variation selectors
        (c == 0x00A9) || (c == 0x00AE))  // Copyright/reserved glyphs © ®
    {
        int len = 1;
        while (str[len] >= 0xFE00 && str[len] <= 0xFE0F) len++;
        if (str[len] == 0x200D) {
            int next = GetEmojiLength(str + len + 1);
            if (next > 0) len += 1 + next;
        }
        return len;
    }
    
    return 0;
}

static const WCHAR* LookupEmoji(const WCHAR* str, int* matchLen) {
    if (!str || !*str) return nullptr;
    if (!InterlockedCompareExchange(&g_emojiTableReady, 1, 1)) return nullptr;
    
    LONG count = InterlockedCompareExchange(&g_emojiCount, 0, 0);
    // Read actual count without modifying
    count = g_emojiCount;
    
    for (LONG i = 0; i < count; i++) {
        const WCHAR* sur = g_dynamicEmojiTable[i].surrogates;
        int eLen = (int)wcslen(sur);
        if (eLen > 0 && wcsncmp(str, sur, eLen) == 0) {
            int totalLen = eLen;
            while (str[totalLen] >= 0xFE00 && str[totalLen] <= 0xFE0F) totalLen++;
            *matchLen = totalLen;
            return g_dynamicEmojiTable[i].name;
        }
    }
    
    return nullptr;
}

static void ConvertEmojisToNames(WCHAR* str, int bufSize) {
    if (!str || !str[0]) return;
    
    WCHAR temp[1024] = {};
    int tempPos = 0;
    int srcPos = 0;
    int srcLen = (int)wcslen(str);
    
    while (srcPos < srcLen && tempPos < (int)ARRAYSIZE(temp) - 32) {
        // If keepBasicEmoji is enabled, check if this is a basic BMP emoji to exempt
        if (g_settings.keepBasicEmoji) {
            int basicLen = IsBasicEmojiSequence(str + srcPos);
            if (basicLen > 0) {
                // Copy the basic emoji glyph as-is (skip the lookup)
                for (int k = 0; k < basicLen && tempPos < (int)ARRAYSIZE(temp) - 1; k++) {
                    temp[tempPos++] = str[srcPos++];
                }
                continue;
            }
        }
        
        // Try to match a known emoji
        int matchLen = 0;
        const WCHAR* name = LookupEmoji(str + srcPos, &matchLen);
        
        if (name) {
            int nameLen = (int)wcslen(name);
            if (tempPos + nameLen < (int)ARRAYSIZE(temp) - 1) {
                wcscpy(temp + tempPos, name);
                tempPos += nameLen;
                srcPos += matchLen;
                continue;
            }
        }
        
        // Check if it's an unknown emoji (surrogate pair not in table)
        int emojiLen = GetEmojiLength(str + srcPos);
        if (emojiLen > 0) {
            if (g_settings.keepBasicEmoji && IsBasicEmojiGlyph(str[srcPos])) {
                temp[tempPos++] = str[srcPos++];
                while (srcPos < srcLen && str[srcPos] >= 0xFE00 && str[srcPos] <= 0xFE0F) {
                    temp[tempPos++] = str[srcPos++];
                }
                continue;
            }
            // Unknown emoji - skip it entirely
            srcPos += emojiLen;
            continue;
        }
        
        // Regular character
        temp[tempPos++] = str[srcPos++];
    }
    
    temp[tempPos] = L'\0';
    wcsncpy(str, temp, bufSize - 1);
    str[bufSize - 1] = L'\0';
}

static void ConvertShortcodesToBasicGlyphs(WCHAR* str, int bufSize) {
    if (!str || !str[0]) return;
    if (!g_settings.keepBasicEmoji) return;
    
    // Hardcoded mappings: shortcode -> basic glyph
    struct ShortcodeMapping {
        const WCHAR* shortcode;
        WCHAR glyph;
    };
    
    static const ShortcodeMapping mappings[] = {
        { L":smile:", 0x263A },           // ☺
        { L":slight_smile:", 0x263A },    // ☺
        { L":frowning:", 0x2639 },        // ☹
        { L":slight_frown:", 0x2639 },    // ☹
        { L":frowning2:", 0x2639 },       // ☹
        { L":heavy_check_mark:", 0x2714 },// ✔
        { L":white_check_mark:", 0x2714 },// ✔
        { L":ballot_box_with_check:", 0x2714 },// ✔
        { L":musical_note:", 0x266B },    // ♪
        { L":notes:", 0x266A },           // ♫
        { L":umbrella:", 0x2602 },        // ☂
        { L":snowman:", 0x2603 },         // ☃
        { nullptr, 0 }
    };
    
    WCHAR temp[1024] = {};
    int tempPos = 0;
    int srcPos = 0;
    int srcLen = (int)wcslen(str);
    
    while (srcPos < srcLen && tempPos < (int)ARRAYSIZE(temp) - 2) {
        bool replaced = false;
        
        if (str[srcPos] == L':') {
            for (int m = 0; mappings[m].shortcode != nullptr; m++) {
                int scLen = (int)wcslen(mappings[m].shortcode);
                if (srcPos + scLen <= srcLen && wcsncmp(str + srcPos, mappings[m].shortcode, scLen) == 0) {
                    temp[tempPos++] = mappings[m].glyph;
                    srcPos += scLen;
                    replaced = true;
                    break;
                }
            }
        }
        
        if (!replaced) {
            temp[tempPos++] = str[srcPos++];
        }
    }
    
    temp[tempPos] = L'\0';
    wcsncpy(str, temp, bufSize - 1);
    str[bufSize - 1] = L'\0';
}

static void StripInvisibleChars(WCHAR* str) {
    if (!str) return;

    WCHAR* read = str;
    WCHAR* write = str;

    while (*read) {
        WCHAR c = *read;

        // Skip invisible formatting characters (but NOT emoji-related ones)
        if (c >= 0x200B && c <= 0x200D) { read++; continue; }  // ZWJ kept by emoji converter
        if (c >= 0x200E && c <= 0x200F) { read++; continue; }
        if (c >= 0x2028 && c <= 0x202F) { read++; continue; }
        if (c >= 0x2060 && c <= 0x2069) { read++; continue; }
        if (c == 0xFEFF) { read++; continue; }
        if (c == 0xFFFC || c == 0xFFFD) { read++; continue; }

        *write++ = *read++;
    }
    *write = L'\0';

    // Trim leading spaces
    WCHAR* start = str;
    while (*start == L' ') start++;
    if (start != str) {
        WCHAR* dst = str;
        while (*start) *dst++ = *start++;
        *dst = L'\0';
    }

    // Trim trailing spaces
    int len = (int)wcslen(str);
    while (len > 0 && str[len - 1] == L' ') {
        str[--len] = L'\0';
    }

    // Collapse multiple spaces
    read = str;
    write = str;
    bool lastWasSpace = false;
    while (*read) {
        if (*read == L' ') {
            if (!lastWasSpace) *write++ = *read;
            lastWasSpace = true;
        } else {
            *write++ = *read;
            lastWasSpace = false;
        }
        read++;
    }
    *write = L'\0';
}

static WCHAR g_lastXmlTitle[256] = {};
static WCHAR g_lastXmlBody[512] = {};
static bool g_haveXmlText = false;

static void ParseTextFromXmlString(const WCHAR* xml) {
    g_lastXmlTitle[0] = L'\0';
    g_lastXmlBody[0] = L'\0';
    g_lastImagePath[0] = L'\0';
    g_haveXmlText = false;
    if (!xml) return;

    const WCHAR* imgTag = wcsstr(xml, L"<image");
    if (imgTag) {
        const WCHAR* srcAttr = wcsstr(imgTag, L"src='");
        if (!srcAttr) srcAttr = wcsstr(imgTag, L"src=\"");
        if (srcAttr) {
            srcAttr += 5;
            WCHAR quote = *(srcAttr - 1);
            const WCHAR* srcEnd = wcschr(srcAttr, quote);
            if (srcEnd) {
                int len = (int)(srcEnd - srcAttr);
                int cl = MIN_VAL(len, (int)ARRAYSIZE(g_lastImagePath) - 1);
                wcsncpy(g_lastImagePath, srcAttr, cl);
                g_lastImagePath[cl] = L'\0';
                DecodeXmlEntities(g_lastImagePath);
                for (WCHAR* p = g_lastImagePath; *p; p++) {
                    if (*p == L'/') *p = L'\\';
                }
            }
        }
    }

    int textIndex = 0;
    const WCHAR* pos = xml;

    while (pos && textIndex < 3) {
        const WCHAR* tagStart = wcsstr(pos, L"<text");
        if (!tagStart) break;

        const WCHAR* contentStart = wcschr(tagStart, L'>');
        if (!contentStart) break;
        contentStart++;

        if (*(contentStart - 2) == L'/') {
            pos = contentStart;
            textIndex++;
            continue;
        }

        const WCHAR* contentEnd = wcsstr(contentStart, L"</text>");
        if (!contentEnd) break;

        int contentLen = (int)(contentEnd - contentStart);
        if (contentLen > 0) {
            if (textIndex == 0) {
                int cl = MIN_VAL(contentLen, (int)ARRAYSIZE(g_lastXmlTitle) - 1);
                wcsncpy(g_lastXmlTitle, contentStart, cl);
                g_lastXmlTitle[cl] = L'\0';
                g_haveXmlText = true;
            } else {
                int cur = (int)wcslen(g_lastXmlBody);
                if (cur > 0 && cur < (int)ARRAYSIZE(g_lastXmlBody) - 2) {
                    g_lastXmlBody[cur] = L'\n';
                    cur++;
                }
                int cl = MIN_VAL(contentLen, (int)ARRAYSIZE(g_lastXmlBody) - cur - 1);
                wcsncpy(g_lastXmlBody + cur, contentStart, cl);
                g_lastXmlBody[cur + cl] = L'\0';
            }
        }

        pos = contentEnd + 7;
        textIndex++;
    }

    DecodeXmlEntities(g_lastXmlTitle);
    DecodeXmlEntities(g_lastXmlBody);
    ConvertEmojisToNames(g_lastXmlTitle, ARRAYSIZE(g_lastXmlTitle));
    ConvertEmojisToNames(g_lastXmlBody, ARRAYSIZE(g_lastXmlBody));
    ConvertShortcodesToBasicGlyphs(g_lastXmlTitle, ARRAYSIZE(g_lastXmlTitle));
    ConvertShortcodesToBasicGlyphs(g_lastXmlBody, ARRAYSIZE(g_lastXmlBody));
    StripInvisibleChars(g_lastXmlTitle);
    StripInvisibleChars(g_lastXmlBody);

    if (g_haveXmlText) {
        Wh_Log(L"Parsed: title=\"%s\" body=\"%s\"", g_lastXmlTitle, g_lastXmlBody);
    }
}

// Vtable patching

static bool PatchVtableEntry(void** vtbl, int index, void* hookFn, void** origFn) {
    void* current = vtbl[index];
    if (current == hookFn) return true;

    *origFn = current;
    DWORD oldProtect;
    if (VirtualProtect(&vtbl[index], sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        vtbl[index] = hookFn;
        VirtualProtect(&vtbl[index], sizeof(void*), oldProtect, &oldProtect);
        return true;
    }
    if (VirtualProtect(&vtbl[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        vtbl[index] = hookFn;
        VirtualProtect(&vtbl[index], sizeof(void*), oldProtect, &oldProtect);
        return true;
    }
    return false;
}

// LoadXml hooking

typedef HRESULT (STDMETHODCALLTYPE *LoadXml_t)(void* pThis, HSTRING xml);
static LoadXml_t LoadXml_Orig = nullptr;
static bool g_loadXmlHooked = false;

static const GUID IID_IXmlDocumentIO =
    {0x6CD0E74E, 0xEE65, 0x4489, {0x9E,0xBF,0xCA,0x43,0xE8,0x7B,0xA6,0x37}};

static HRESULT STDMETHODCALLTYPE LoadXml_Hook(void* pThis, HSTRING xml) {
    UINT32 len = 0;
    const WCHAR* xmlStr = WindowsGetStringRawBuffer(xml, &len);

    if (xmlStr && len > 0) {
        if (wcsstr(xmlStr, L"<toast")) {
            ParseTextFromXmlString(xmlStr);
        }
    }

    return LoadXml_Orig(pThis, xml);
}

static void TryHookLoadXml(void* pXmlDocInstance) {
    if (g_loadXmlHooked || !pXmlDocInstance) return;

    void* pDocIO = nullptr;
    HRESULT hr = ((IUnknown*)pXmlDocInstance)->QueryInterface(IID_IXmlDocumentIO, &pDocIO);
    if (FAILED(hr) || !pDocIO) return;

    void** ioVtbl = *(void***)pDocIO;
    if (PatchVtableEntry(ioVtbl, 6, (void*)LoadXml_Hook, (void**)&LoadXml_Orig)) {
        g_loadXmlHooked = true;
        Wh_Log(L"Hooked LoadXml via vtable patch");
    }

    ((IUnknown*)pDocIO)->Release();
}

// ToastNotifier_Show hooking

typedef HRESULT (STDMETHODCALLTYPE *ToastNotifier_Show_t)(void* pThis, void* pNotification);
static ToastNotifier_Show_t ToastNotifier_Show_Orig = nullptr;
static bool g_showHooked = false;

static HRESULT STDMETHODCALLTYPE ToastNotifier_Show_Hook(void* pThis, void* pNotification) {
    Wh_Log(L"Show: toast suppressed");
    return S_OK;
}

// CreateToastNotification hooking

typedef HRESULT (STDMETHODCALLTYPE *CreateToastNotification_t)(void* pThis, void* pXmlDoc, void** ppNotification);
static CreateToastNotification_t CreateToastNotification_Orig = nullptr;
static bool g_notifFactoryHooked = false;

static HRESULT STDMETHODCALLTYPE CreateToastNotification_Hook(void* pThis, void* pXmlDoc, void** ppNotification) {
    if (g_haveXmlText) {
        ShowBalloonNotification(
            g_lastXmlTitle[0] ? g_lastXmlTitle : L"Discord",
            g_lastXmlBody[0] ? g_lastXmlBody : L"New message"
        );
        g_haveXmlText = false;
    } else {
        ShowBalloonNotification(L"Discord", L"New message");
    }

    return CreateToastNotification_Orig(pThis, pXmlDoc, ppNotification);
}

static void TryHookNotificationFactory(void* pFactory) {
    if (g_notifFactoryHooked || !pFactory) return;

    void** vtbl = *(void***)pFactory;
    if (!vtbl) return;

    if (PatchVtableEntry(vtbl, 6, (void*)CreateToastNotification_Hook, (void**)&CreateToastNotification_Orig)) {
        g_notifFactoryHooked = true;
        Wh_Log(L"Hooked CreateToastNotification via vtable patch");
    }
}

// Proactive Show hooking

static bool g_inProactiveHook = false;

static void ProactivelyHookShow() {
    if (g_showHooked || g_inProactiveHook) return;
    g_inProactiveHook = true;

    static const GUID IID_IToastNotificationManagerStatics =
        {0x50AC103F, 0xD235, 0x4598, {0xBB,0xEF,0x98,0xFE,0x4D,0x1A,0x3A,0xD4}};

    HSTRING hClassName = nullptr;
    WindowsCreateString(
        L"Windows.UI.Notifications.ToastNotificationManager",
        (UINT32)wcslen(L"Windows.UI.Notifications.ToastNotificationManager"),
        &hClassName);

    void* pManagerStatics = nullptr;
    HRESULT hr = RoGetActivationFactory(hClassName, IID_IToastNotificationManagerStatics, &pManagerStatics);
    WindowsDeleteString(hClassName);

    if (FAILED(hr) || !pManagerStatics) {
        g_inProactiveHook = false;
        return;
    }

    void** managerVtbl = *(void***)pManagerStatics;
    typedef HRESULT (STDMETHODCALLTYPE *CreateToastNotifierWithId_t)(void* pThis, HSTRING appId, void** ppNotifier);
    CreateToastNotifierWithId_t createWithId = (CreateToastNotifierWithId_t)managerVtbl[7];

    void* pNotifier = nullptr;
    HSTRING hAppId = nullptr;
    WindowsCreateString(L"Discord", 7, &hAppId);
    hr = createWithId(pManagerStatics, hAppId, &pNotifier);
    WindowsDeleteString(hAppId);

    if (SUCCEEDED(hr) && pNotifier) {
        void** notifierVtbl = *(void***)pNotifier;
        void* showAddr = notifierVtbl[6];
        Wh_SetFunctionHook(showAddr, (void*)ToastNotifier_Show_Hook, (void**)&ToastNotifier_Show_Orig);
        Wh_ApplyHookOperations();
        g_showHooked = true;
        Wh_Log(L"Hooked IToastNotifier::Show at %p", showAddr);
        ((IUnknown*)pNotifier)->Release();
    }

    ((IUnknown*)pManagerStatics)->Release();
    g_inProactiveHook = false;
}

// RoGetActivationFactory hooking

typedef HRESULT (WINAPI *RoGetActivationFactory_t)(HSTRING classId, REFIID iid, void** factory);
static RoGetActivationFactory_t RoGetActivationFactory_Orig = nullptr;

HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING classId, REFIID iid, void** factory) {
    HRESULT hr = RoGetActivationFactory_Orig(classId, iid, factory);

    if (SUCCEEDED(hr) && factory && *factory) {
        UINT32 len = 0;
        const WCHAR* name = WindowsGetStringRawBuffer(classId, &len);
        if (!name) return hr;

        if (wcscmp(name, L"Windows.UI.Notifications.ToastNotification") == 0) {
            TryHookNotificationFactory(*factory);
            if (!g_showHooked) ProactivelyHookShow();
        }
        else if (!g_showHooked && !g_inProactiveHook && wcsstr(name, L"ToastNotificationManager")) {
            ProactivelyHookShow();
        }
    }

    return hr;
}

// RoActivateInstance hooking

typedef HRESULT (WINAPI *RoActivateInstance_t)(HSTRING classId, void** instance);
static RoActivateInstance_t RoActivateInstance_Orig = nullptr;

HRESULT WINAPI RoActivateInstance_Hook(HSTRING classId, void** instance) {
    HRESULT hr = RoActivateInstance_Orig(classId, instance);

    if (SUCCEEDED(hr) && instance && *instance && !g_loadXmlHooked) {
        UINT32 len = 0;
        const WCHAR* name = WindowsGetStringRawBuffer(classId, &len);
        if (name && wcscmp(name, L"Windows.Data.Xml.Dom.XmlDocument") == 0) {
            TryHookLoadXml(*instance);
        }
    }

    return hr;
}

// Windhawk functions

BOOL Wh_ModInit(void) {
    Wh_Log(L"Discord Balloon Notifications mod started (PID %lu)", GetCurrentProcessId());

    LoadSettings();

    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH)) {
        g_hAppIcon = ExtractIconW(GetModuleHandleW(nullptr), exePath, 0);
        if (g_hAppIcon == (HICON)1) g_hAppIcon = nullptr;
    }

    InitializeCriticalSection(&g_cs);

    // Allocate emoji table
    g_dynamicEmojiTable = (DynamicEmojiEntry*)HeapAlloc(GetProcessHeap(), 
        HEAP_ZERO_MEMORY, sizeof(DynamicEmojiEntry) * MAX_EMOJI_ENTRIES);
    
    // Allocate basic glyph table
    g_basicGlyphTable = (BasicGlyphEntry*)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, sizeof(BasicGlyphEntry) * MAX_BASIC_GLYPHS);
    
    // Start emoji loading in background
    HANDLE hEmojiThread = CreateThread(nullptr, 0, EmojiLoadThread, nullptr, 0, nullptr);
    if (hEmojiThread) CloseHandle(hEmojiThread);
    
    // Start basic glyph loading in background
    HANDLE hGlyphThread = CreateThread(nullptr, 0, BasicGlyphLoadThread, nullptr, 0, nullptr);
    if (hGlyphThread) CloseHandle(hGlyphThread);

    g_hThread = CreateThread(nullptr, 0, BalloonThread, nullptr, 0, nullptr);
    Sleep(200);
    
    g_stopMonitor = false;
    g_hMonitorThread = CreateThread(nullptr, 0, DiscordMonitorThread, nullptr, 0, nullptr);

    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase) combase = LoadLibraryW(L"combase.dll");

    if (combase) {
        void* p1 = (void*)GetProcAddress(combase, "RoGetActivationFactory");
        void* p2 = (void*)GetProcAddress(combase, "RoActivateInstance");

        if (p1) Wh_SetFunctionHook(p1, (void*)RoGetActivationFactory_Hook, (void**)&RoGetActivationFactory_Orig);
        if (p2) Wh_SetFunctionHook(p2, (void*)RoActivateInstance_Hook, (void**)&RoActivateInstance_Orig);
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}

void Wh_ModUninit(void) {
    g_stopMonitor = true;
    if (g_hMonitorThread) {
        WaitForSingleObject(g_hMonitorThread, 3000);
        CloseHandle(g_hMonitorThread);
        g_hMonitorThread = nullptr;
    }

    RemoveTrayIcon();

    if (g_lastNotifIcon) {
        DestroyIcon(g_lastNotifIcon);
        g_lastNotifIcon = nullptr;
    }

    if (g_hBalloonWnd)
        PostMessageW(g_hBalloonWnd, WM_QUIT, 0, 0);

    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
    }

    if (g_hAppIcon)
        DestroyIcon(g_hAppIcon);

    if (g_hTrayMutex) {
        if (g_ownsTray) {
            ReleaseMutex(g_hTrayMutex);
            g_ownsTray = false;
        }
        CloseHandle(g_hTrayMutex);
        g_hTrayMutex = nullptr;
    }

    if (g_dynamicEmojiTable) {
        HeapFree(GetProcessHeap(), 0, g_dynamicEmojiTable);
        g_dynamicEmojiTable = nullptr;
    }
    
    if (g_basicGlyphTable) {
        HeapFree(GetProcessHeap(), 0, g_basicGlyphTable);
        g_basicGlyphTable = nullptr;
    }

    DeleteCriticalSection(&g_cs);
    UnregisterClassW(L"WindhawkDiscordBalloonClass", GetModuleHandleW(nullptr));
}
