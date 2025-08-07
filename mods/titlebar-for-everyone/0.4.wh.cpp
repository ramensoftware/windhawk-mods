// ==WindhawkMod==
// @id              titlebar-for-everyone
// @name            Titlebar For Everyone
// @description     Force native title bars and frames for various programs
// @version         0.4
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         chrome.exe
// @include         brave.exe
// @include         msedge.exe
// @include         opera.exe
// @include         yandex.exe
// @include         whale.exe
// @include         supermium.exe
// @include         chromium.exe
// @include         devenv.exe
// @include         steamwebhelper.exe
// @include         PowerToys.*.exe
// @include         olk.exe
// @include         WebViewHost.exe
// @include         winword.exe
// @include         excel.exe
// @include         powerpnt.exe
// @include         msaccess.exe
// @include         onenote.exe
// @include         outlook.exe
// @include         mspub.exe
// @include         hwp.exe
// @include         Photos.exe
// @include         MuseScore*.exe
// @include         ms-teams.exe
// @compilerOptions -lcomctl32 -luxtheme -lgdi32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Titlebar For Everyone
* Force native title bars and frames for various programs
## Supported programs
* Chromium browsers
    * Chrome
    * Edge
    * Supermium
    * Brave (known issues: Only works when HW acceleration is enabled and available)
    * Etc. (not tested)
    * Enabling the DWM mode (Aero/Mica) is recommended if you can. This will also remove the duplicate window controls
    * Also provides an option to use the classic button face color in the DWM extended area for Classic theme users
    * Vivaldi does work but is intentionally excluded from the default inclusion list. Use the built-in option to use native title bars instead
* Steam (non-VGUI)
    * Known issues: In the Basic style, the window control hover effects do not show
* WinUI apps
    * PowerToys apps
    * New Photos app
* WPF apps
    * Visual Studio (tested: 2022)
    * PowerToys Workspaces
* Office apps
    * Word, Excel, PowerPoint, OneNote, Access, Outlook, and Publisher
    * Tested: 2010 and LTSC 2021 (Win10 mode)
    * Known issues: Only Classic frames are shown, neither Basic nor DWM, unless it's using the Aero mode (in 2007 or 2010)
* Some WebView2 apps
    * New Outlook
    * Microsoft 365 Copilot
    * Microsoft Teams (new unified app, not the Electron-based Teams Classic)
* Qt apps
    * MuseScore
    * Known issues: If the mod loads after a window is open, the window content will look downscaled a bit
* Hangul Word Processor
    * Tested: 2010 (using the system style is recommended)
    * This mod also fixes the WM_NCACTIVATE bug present in the system style
* Electron apps and CEF apps using `cef_window_create_top_level`
    * Not included by default, but adding them manually to the inclusion list should work
    * However, using this mod against those is not recommended
    * For CEF apps using `cef_window_create_top_level`: use [CEF/Spotify Tweaks](https://windhawk.net/mods/cef-titlebar-enabler-universal) instead. Add to the inclusion list manually for anything other than Spotify
    * For Electron apps: try finding a built-in option to enable native frames first. If missing, patch files in `resources\app(.asar)` to override `frame: false` or `titleBarStyle: 'hidden'` to `frame: true` or `titleBarStyle: 'default'`
    * 1Password: add `"appearance.useCustomTitleBar": false` to `%localappdata%\1Password\settings\settings.json`
    * VSCode & forks (including Windhawk UI): Press F1, search and click `Preferences: Open Settings (UI)`, find `Window: Title Bar Style`, and set it to `native`
* Programs that this mod can remove duplicate window controls
    * Chromium browsers (noted above)
    * WinUI apps (PowerToys, New Photos)
    * New Teams
## Not supported programs
* UWP apps
    * Use [this mod](https://winclassic.net/thread/2041/remove-windows-10s-uwp-titlebars) instead
* VMware Workstation/Player 16+
    * Use [this patch](https://winclassic.net/thread/1568/change-vmware-titlebar-native-windows) instead
* Firefox and derivatives
    * Just enable the title bar option in the customize page, or disable `browser.tabs.inTitlebar` in `about:config`
* NVIDIA apps
    * In the executable folder, look for `<exe name without extension>.json` (usually in the same directory or in `\Resources`), and change `nv-custom-black-window=true` to false
## Notes
* This mod does not deal with visual inconsistencies, such as duplicate window controls, except for programs noted above
* Forcing the addition of your program to the inclusion list may work even if it's missing from the default list if it falls within the categories above (e.g., Chromium, WinUI)
* For other apps, you'll need to try adjusting the subclassing logic to get your target window to be subclassed properly
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- btnfacechrome: false
  $name: Chromium - Paint Win32 ButtonFace color to the DWM area
  $name:ko-KR: Chromium - DWM 영역에 Win32 버튼 표면 색상 칠하기
  $description: "In Chromium browsers, this option will draw the classic Windows button face color to the background of the title/tab bar\n
    DWM frames must be enabled for this to work. For recent versions of Chromium, this has three requirements:\n
    * Using Windows 11 22H2 (build 22621) or higher. On older versions, you can spoof your Windows version with Application Verifier or Version Spoof mod\n
    * Option to show accent colors to title bars in the personalization settings must be disabled\n
    * 'Windows 11 Mica titlebar' must be enabled in 'chrome://flags'\n
    Hardware acceleration also must be enabled and working for this option to work\n
    Recommended for users of Classic, XP, or Basic themes"
  $description:ko-KR: "Chromium 브라우저에서, 이 옵션은 고전 Windows 버튼 표면 색상을 제목 표시줄 및 탭 표시줄 영역에 그립니다\n
    DWM 테두리가 활성화되어있어야 합니다. 최근 버전의 Chromium에서 이는 다음 세 가지를 요구합니다:\n
    * Windows 11 22H2 (빌드 22621) 이상이여야 합니다. 하위 버전에서는 Application Verifier 또는 Version Spoof 모드를 이용하여 Windows 버전을 속일 수 있습니다\n
    * 개인 설정에서 제목 표시줄에 테마 컬러를 표시하는 옵션은 비활성화되어야 합니다\n
    * 'chrome://flags'에서 'Windows 11 Mica titlebar'를 활성화해야 합니다\n
    하드웨어 가속 또한 켜져 있고 사용 가능해야 합니다\n
    고전, XP, 또는 베이직 테마 사용자에게 권장됩니다"
- clientedge: keep
  $name: Client edge settings
  $name:ko-KR: 클라이언트 영역 테두리 설정
  $options:
  - keep: Keep application behavior
  - add: Always show
  - remove: Always hide
  $options:ko-KR:
  - keep: 응용 프로그램 동작 유지
  - add: 항상 표시
  - remove: 항상 숨기기
- notoolwinsteam: true
  $name: Steam - Hide tool window frames
  $name:ko-KR: Steam - 도구 창 테두리 숨기기
*/
// ==/WindhawkModSettings==

#include <shlwapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <uxtheme.h>
#include <cwchar>
#include <string>
#include <fstream>
#include <sstream>

#ifndef WS_EX_NOREDIRECTIONBITMAP // WH 1.4
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

enum WorkingMode {
    MODE_DEFAULT,
    MODE_OUTLOOK,
    MODE_CHROMIUM,
    MODE_VS
};

enum ClientEdgeSetting {
    CE_KEEP,
    CE_ADD,
    CE_REMOVE
};

struct tb4e_settings {
    BOOL btnFaceChrome = FALSE;
    ClientEdgeSetting clientEdgeSetting = CE_KEEP;
    BOOL noToolWinSteam = FALSE;
} tb4e_settings;

WorkingMode mode = MODE_DEFAULT;

BOOL isSteam = FALSE;
wchar_t steamIndexHtml[MAX_PATH];
wchar_t steamIndexHtmlModded[MAX_PATH];

BOOL isBrave = FALSE;

#pragma region Subclassing
LRESULT CALLBACK HideSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_SHOWWINDOW:
            return 0;
        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if (pos->flags & SWP_SHOWWINDOW) {
                pos->flags &= ~SWP_SHOWWINDOW;
                pos->flags |= SWP_HIDEWINDOW;
            }
            break;
        }
        case WM_WINDOWPOSCHANGED: {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if ((pos->flags & SWP_SHOWWINDOW) && IsWindowVisible(hWnd)) {
                ShowWindow(hWnd, SW_HIDE);  // Backup in case it still becomes visible
            }
            break;
        }
        default:
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_PAINT:
        {
            if (tb4e_settings.btnFaceChrome && mode == MODE_CHROMIUM && FindWindowExW(hWnd, NULL, L"Intermediate D3D Window", NULL))
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                RECT rect;
                GetClientRect(hWnd, &rect);
                FillRect(hdc, &rect, (HBRUSH)GetSysColorBrush(COLOR_BTNFACE));
                EndPaint(hWnd, &ps);
                return 0;
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
        break;
        case WM_NCCALCSIZE:
        {
            if (mode == MODE_OUTLOOK) {
                DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
        }
        case WM_NCACTIVATE:
        case WM_NCPAINT:
        case WM_NCHITTEST:
        case WM_NCLBUTTONDOWN:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        case WM_DWMNCRENDERINGCHANGED:
            // Fix issues with hardware accelerated apps and basic themers
            RECT rect;
            GetWindowRect(hWnd, &rect);
            SetWindowPos(hWnd, NULL, rect.left, rect.top + 1, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
            SetWindowPos(hWnd, NULL, rect.left, rect.top, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        default:
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
}

void ProcessWindow(HWND hWnd, bool onlyUpdateStyle = false) {
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    bool isToolWindow = (exStyle & WS_EX_TOOLWINDOW) != 0;

    if (isSteam && isToolWindow) {
        if (tb4e_settings.noToolWinSteam) {
            style &= ~(WS_CAPTION | WS_BORDER);
        } else {
            style |= WS_CAPTION | WS_BORDER;
        }
        SetWindowLongW(hWnd, GWL_STYLE, style);
        if (!onlyUpdateStyle) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
                Wh_Log(L"Subclassed %p", hWnd);
            }
        }
        return;
    }

    // Exclude tool windows or child windows
    if (isToolWindow || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return;
    }

    // Fix a specific issue with Visual Studio's "Show/Hide debug targets" window
    // It simply renders nothing when WM_NCCALCSIZE is subclassed
    if (mode == MODE_VS && (style & 0x60C0000) != 0 && exStyle == 0x80101) {
        return;
    }

    if (!onlyUpdateStyle) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
            Wh_Log(L"Subclassed %p", hWnd);
        }
    }

    style |= WS_CAPTION | WS_SYSMENU;
    switch (tb4e_settings.clientEdgeSetting) {
        case CE_ADD:
            exStyle |= WS_EX_CLIENTEDGE;
            break;
        case CE_REMOVE:
            exStyle &= ~WS_EX_CLIENTEDGE;
            break;
        case CE_KEEP:
            // do nothing
            // can't do anything on config change unfortunately
    }

    SetWindowLongW(hWnd, GWL_STYLE, style);
    SetWindowLongW(hWnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

void SetWinUICustomControlsVisibility(HWND parent, BOOL visible) {
    HWND ncInputSrcHwnd = FindWindowExW(parent, NULL, L"InputNonClientPointerSource", NULL);
    HWND windowControlHwnd = FindWindowExW(parent, NULL, L"ReunionWindowingCaptionControls", NULL);
    HWND teamsOverlayHwnd = FindWindowExW(parent, NULL, L"TeamsOverlay", NULL);
    if (ncInputSrcHwnd) {
        ShowWindow(ncInputSrcHwnd, visible);
        if (visible) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(ncInputSrcHwnd, HideSubclassProc);
            Wh_Log(L"Restored InputNonClientPointerSource");
        } else {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(ncInputSrcHwnd, HideSubclassProc, 0)) {
                Wh_Log(L"Hid InputNonClientPointerSource (%p)", ncInputSrcHwnd);
            }
        }
    }
    if (windowControlHwnd) {
        ShowWindow(windowControlHwnd, visible);
        if (visible) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(windowControlHwnd, HideSubclassProc);
            Wh_Log(L"Restored TeamsOverlay");
        } else {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(windowControlHwnd, HideSubclassProc, 0)) {
                Wh_Log(L"Hid TeamsOverlay (%p)", windowControlHwnd);
            }
        }
    }
    if (teamsOverlayHwnd) {
        ShowWindow(teamsOverlayHwnd, visible);
        if (visible) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(teamsOverlayHwnd, HideSubclassProc);
            Wh_Log(L"Restored ReunionWindowingCaptionControls");
        } else {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(teamsOverlayHwnd, HideSubclassProc, 0)) {
                Wh_Log(L"Hid ReunionWindowingCaptionControls (%p)", teamsOverlayHwnd);
            }
        }
    }
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow())
        return TRUE;

    bool isTarget = false;

    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Subclass all relevant windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        // Chromium browsers / Electron apps / CEF top-level windows
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            mode = MODE_CHROMIUM;
            isTarget = true;
        // WPF apps
        } else if (wcsncmp(className, L"HwndWrapper[", 12) == 0) {
            isTarget = true;
        // WinUI
        } else if (wcsncmp(className, L"WinUIDesktopWin32WindowClass", 28) == 0) {
            isTarget = true;
            SetWinUICustomControlsVisibility(hWnd, FALSE);
        // Steam
        } else if (wcscmp(className, L"SDL_app") == 0) {
            isTarget = true;
        // Microsoft 365 Copilot
        } else if (wcscmp(className, L"OfficeApp-Frame") == 0) {
            isTarget = true;
        // New Outlook
        } else if (wcscmp(className, L"Olk Host") == 0) {
            mode = MODE_OUTLOOK;
            isTarget = true;
        // Microsoft Office apps
        } else if (wcscmp(className, L"OpusApp") == 0 || // Word
            wcscmp(className, L"XLMAIN") == 0 || // Excel
            wcscmp(className, L"PPTFrameClass") == 0 || // PowerPoint
            wcscmp(className, L"OMain") == 0 || // Access
            wcscmp(className, L"Framework::CFrame") == 0 || // OneNote
            wcscmp(className, L"rctrl_renwnd32") == 0 || // Outlook
            wcscmp(className, L"MSWinPub") == 0 // Publisher
        ) {
            isTarget = true;
        // Hangul Word Processor
        } else if (wcsncmp(className, L"HwpApp", 6) == 0) {
            isTarget = true;
        // Qt apps
        } else if (wcsstr(className, L"QWindowIcon") != 0) {
            isTarget = true;
        // New Teams
        } else if (wcsstr(className, L"TeamsWebView") != 0) {
            isTarget = true;
            SetWinUICustomControlsVisibility(hWnd, FALSE);
        }

        if (isTarget) {
            ProcessWindow(hWnd);
        }
    }
    return TRUE;
}

BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Unsubclass all windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
        SetWinUICustomControlsVisibility(hWnd, TRUE);
        if (lParam == 1) {
            SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        }
    }
    return TRUE;
}

BOOL CALLBACK UpdateEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        ProcessWindow(hWnd, true);
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
    }
    return TRUE;
}
#pragma endregion

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    if ((dwExStyle & WS_EX_NOREDIRECTIONBITMAP) != 0) { // This makes the GDI surface invisible
        dwExStyle &= ~WS_EX_NOREDIRECTIONBITMAP;        // Just purge this cuz it's bad for Basic/Classic users anyway
    }                                                   // It must be set in CreateWindowExW. Not changeable with SetWindowLongW

    bool isTarget = false;

    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        Wh_Log(L"%s", className);
        // Chromium browsers / Electron apps / CEF top-level windows
        // Subclass late for Brave cuz somehow early subclassing makes it invisible
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            if (dwStyle & WS_CAPTION) {
                mode = MODE_CHROMIUM;
                if (!isBrave) {
                    isTarget = true;
                }
            }
        // WinUI
        } else if (wcsncmp(className, L"WinUIDesktopWin32WindowClass", 28) == 0) {
            if (dwStyle & WS_CAPTION) {
                isTarget = true;
                SetWinUICustomControlsVisibility(hWnd, FALSE);
            }
        // Steam with workarounds
        } else if (isSteam && wcscmp(className, L"Chrome_RenderWidgetHostHWND") == 0) {
            EnumWindows(InitEnumWindowsProc, 1);
        // Microsoft 365 Copilot
        } else if (wcscmp(className, L"OfficeApp-Frame") == 0) {
            isTarget = true;
        // New Outlook
        } else if (wcscmp(className, L"Olk Host") == 0) {
            mode = MODE_OUTLOOK;
            isTarget = true;
        // Hangul Word Processor
        } else if (wcsncmp(className, L"HwpApp", 6) == 0) {
            isTarget = true;
        // Qt apps
        } else if (wcsstr(className, L"QWindowIcon") != 0) {
            isTarget = true;
        // New Teams
        } else if (wcsstr(className, L"TeamsWebView") != 0) {
            isTarget = true;
            SetWinUICustomControlsVisibility(hWnd, FALSE);
        // WinUI / Teams custom controls
        } else if (wcsstr(className, L"InputNonClientPointerSource") != 0 ||
            wcsstr(className, L"ReunionWindowingCaptionControls") != 0 ||
            wcsstr(className, L"TeamsOverlay") != 0
        ) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, HideSubclassProc, 0)) {
                Wh_Log(L"Hid %s (%p)", className, hWnd);
            }
        }

        if (isTarget) {
            ProcessWindow(hWnd);
        }
    }
    return hWnd;
}

// Subclassic VS windows in CreateWindowExW doesn't work for some reason, so we have to do it in ShowWindow
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_original;
BOOL WINAPI ShowWindow_hook(HWND hWnd, int nCmdShow) {
    wchar_t className[256];
    GetClassName(hWnd, className, 256);
    Wh_Log(L"ShowWindow_hook: %s", className);
    // WPF apps
    if (wcsncmp(className, L"HwndWrapper[", 12) == 0) {
        ProcessWindow(hWnd);
    // Microsoft Office apps
    } else if (wcscmp(className, L"OpusApp") == 0 || // Word
        wcscmp(className, L"XLMAIN") == 0 || // Excel
        wcscmp(className, L"PPTFrameClass") == 0 || // PowerPoint
        wcscmp(className, L"OMain") == 0 || // Access
        wcscmp(className, L"rctrl_renwnd32") == 0 || // Outlook
        wcscmp(className, L"MSWinPub") == 0 // Publisher
    ) {
        ProcessWindow(hWnd);
    // OneNote with workarounds
    } else if (wcscmp(className, L"Framework::CFrameApp") == 0) {
        EnumWindows(InitEnumWindowsProc, 1);
    }
    return ShowWindow_original(hWnd, nCmdShow);
}

using SetWindowThemeAttribute_t = decltype(&SetWindowThemeAttribute);
SetWindowThemeAttribute_t SetWindowThemeAttribute_original;
HRESULT WINAPI SetWindowThemeAttribute_hook(HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute) {
    Wh_Log(L"SetWindowThemeAttribute_hook");
    if (eAttribute == WTA_NONCLIENT) {
        // Ignore this to make sure DWM window controls are visible
        return S_OK;
    } else {
        return SetWindowThemeAttribute_original(hwnd, eAttribute, pvAttribute, cbAttribute);
    }
}

using SetParent_t = decltype(&SetParent);
SetParent_t SetParent_original;
HWND WINAPI SetParent_hook(HWND hWndChild, HWND hWndNewParent) {
    wchar_t className[256];
    GetClassName(hWndChild, className, 256);
    Wh_Log(L"SetParent_hook: hWndChild=%p (%s), hWndNewParent=%p", hWndChild, className, hWndNewParent);
    HWND hWndOldParent = SetParent_original(hWndChild, hWndNewParent);
    if (mode == MODE_CHROMIUM && wcscmp(className, L"Intermediate D3D Window") == 0) {
        if (isBrave) {
            ProcessWindow(hWndNewParent);
        }
        if (tb4e_settings.btnFaceChrome) {
            RedrawWindow(hWndNewParent, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
    }
    return hWndOldParent;
}

#pragma region Steam stuff
// From VSCode Tweaker by m417z
using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t CreateFileW_original;
HANDLE WINAPI CreateFileW_hook(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    PCWSTR compareFileName = lpFileName;
    if (compareFileName[0] == '\\' &&
        compareFileName[1] == '\\' &&
        compareFileName[2] == '?' &&
        compareFileName[3] == '\\') {
        compareFileName += 4;
    }

    if (wcsicmp(compareFileName, steamIndexHtml) == 0) {
        Wh_Log(L"lpFileName = %s", compareFileName);
        Wh_Log(L"=>");
        Wh_Log(L"lpFileName = %s", steamIndexHtmlModded);
        lpFileName = steamIndexHtmlModded;
    }

    return CreateFileW_original(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile
    );
}

BOOL PrepareSteamIndexHtml() {
    wchar_t tempPath[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempPath))
        return FALSE;

    if (!PathCombineW(steamIndexHtmlModded, tempPath, L"tb4e-steam-index.html"))
        return FALSE;

    std::ifstream inFile(steamIndexHtml, std::ios::binary);
    if (!inFile)
        return FALSE;

    std::ostringstream buffer;
    buffer << inFile.rdbuf();
    std::string html = buffer.str();
    inFile.close();

    // Hook window.open to override browserType to 3
    const char* injectScript =
        "<script>window.open=(()=>{const o=window.open;return(s,t,f)=>{const u=new URL(s);u.searchParams.set('browserType','3');return o(u.toString(),t,f);};})();</script>";

    // Look for the first <script tag
    size_t insertPos = html.find("<script");
    if (insertPos == std::string::npos)
        return FALSE;

    html.insert(insertPos, injectScript);

    std::ofstream outFile(steamIndexHtmlModded, std::ios::binary);
    if (!outFile)
        return FALSE;

    outFile.write(html.data(), html.size());
    outFile.close();

    Wh_Log(L"Modified steam index.html written to: %s", steamIndexHtmlModded);
    return TRUE;
}
#pragma endregion

void LoadSettings() {
    tb4e_settings.btnFaceChrome = Wh_GetIntSetting(L"btnfacechrome");
    LPCWSTR ceSetting = Wh_GetStringSetting(L"clientedge");
    if (wcscmp(ceSetting, L"add") == 0) {
        tb4e_settings.clientEdgeSetting = CE_ADD;
    } else if (wcscmp(ceSetting, L"remove") == 0) {
        tb4e_settings.clientEdgeSetting = CE_REMOVE;
    }
    Wh_FreeStringSetting(ceSetting);
    tb4e_settings.noToolWinSteam = Wh_GetIntSetting(L"notoolwinsteam");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    #ifdef _WIN64
        Wh_Log(L"Init - x86_64");
    #else
        Wh_Log(L"Init - x86");
    #endif

    // Check if this process is auxiliary process by checking if the arguments contain --type=
    LPWSTR args = GetCommandLineW();
    if (wcsstr(args, L"--type=") != NULL && wcsstr(args, L"--type=gpu-process") == NULL) {
        Wh_Log(L"Auxiliary process detected, skipping");
        return FALSE;
    }

    // Check if the app is Steam
    wchar_t modulePath[MAX_PATH];
    GetModuleFileName(NULL, modulePath, MAX_PATH);
    isSteam = wcsstr(_wcsupr(modulePath), L"STEAMWEBHELPER.EXE") != NULL;
    BOOL steamPrepared = FALSE;
    if (isSteam) {
        Wh_Log(L"Steam detected");
        PathRemoveFileSpecW(modulePath);

        wchar_t temp[MAX_PATH];
        PathCombineW(temp, modulePath, L"..\\..\\..\\steamui\\index.html");
        GetFullPathNameW(temp, MAX_PATH, steamIndexHtml, NULL);

        steamPrepared = PrepareSteamIndexHtml();
    }

    isBrave = wcsstr(modulePath, L"BRAVE.EXE") != NULL;

    if (wcsstr(modulePath, L"DEVENV.EXE") != NULL) {
        Wh_Log(L"Visual Studio detected");
        mode = MODE_VS;
    }

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook,
                       (void**)&CreateWindowExW_original);
    Wh_SetFunctionHook((void*)SetWindowThemeAttribute, (void*)SetWindowThemeAttribute_hook,
                       (void**)&SetWindowThemeAttribute_original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_hook,
                       (void**)&ShowWindow_original);
    Wh_SetFunctionHook((void*)SetParent, (void*)SetParent_hook,
                       (void**)&SetParent_original);
    if (isSteam && steamPrepared) {
        Wh_SetFunctionHook((void*)CreateFileW, (void*)CreateFileW_hook,
                           (void**)&CreateFileW_original);
    }

    EnumWindows(InitEnumWindowsProc, 1);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    EnumWindows(UninitEnumWindowsProc, 1);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(UpdateEnumWindowsProc, 1);
}
