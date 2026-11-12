// ==WindhawkMod==
// @id              always-on-top-plus
// @name            Always On Top Windows Plus
// @description     Pin any window above all others with a hotkey, with a highlight border, opacity and sounds
// @version         2.4.0
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @license         MIT
// @donateUrl       https://ko-fi.com/DavidHiFi
// @include         windhawk.exe
// @compilerOptions -luser32 -lwinmm -lgdi32 -ldwmapi -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always On Top Windows Plus

Pin the window you are working in above everything else with one shortcut, and unpin it
with the same shortcut. A thin colored border shows which windows are pinned.

![Always On Top Windows Plus preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/always-on-top-plus.png)

## Features
- **Pin and unpin with a shortcut.** The default is Win+Ctrl+T, and you can change every part of it.
- **Highlight border.** A colored ring around each pinned window. It follows the window
  as it moves and resizes, uses your Windows accent color or a custom color, and can round
  its corners to match the window.
- **See-through windows.** Two extra shortcuts make a pinned window more or less
  transparent, in steps, so you can keep an eye on what is behind it.
- **Sounds and a small status card.** A short sound and a small card at the bottom of the
  screen confirm each pin and unpin. Both can be turned off, and you can pick your own sounds.
- **Skip the apps you choose.** List the apps that should never be pinned.
- **Stays pinned.** If Windows drops the always-on-top state, the mod puts it back, and
  pinned windows keep their border across screen and desktop changes.

## How to use
1. Click into the window you want to keep on top.
2. Press the pin shortcut (Win+Ctrl+T).
3. Press it again to release the window.

Shortcut table:

| Shortcut | Action |
| --- | --- |
| Win+Ctrl+T | Pin or unpin the active window |
| Win+Ctrl++ | Make the active pinned window more opaque |
| Win+Ctrl+- | Make the active pinned window more transparent |

## Notes
- Windhawk runs this mod in its own small process, so it works with every app.
- Windows of apps that run as administrator cannot be pinned, unless portable Windhawk runs as administrator.
- The border cannot be drawn around windows that block it, such as some full-screen games.

## Credits
Based on [Always On Top Windows](https://windhawk.net/mods/always-on-top) by AhmedAwad7.
Brings the behavior of the [Always On Top utility in Microsoft PowerToys](https://learn.microsoft.com/windows/powertoys/always-on-top)
to Windhawk. MIT license.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyWin: true
  $name: Use the Windows key
  $description: Part of the pin shortcut. Pick at least one modifier key.
- hotkeyCtrl: true
  $name: Use the Ctrl key
  $description: Part of the pin shortcut. Pick at least one modifier key.
- hotkeyAlt: false
  $name: Use the Alt key
  $description: Part of the pin shortcut. Pick at least one modifier key.
- hotkeyShift: false
  $name: Use the Shift key
  $description: Part of the pin shortcut. Pick at least one modifier key.
- hotkeyKey: "84"
  $name: Pin shortcut key
  $description: 'The last key of the pin shortcut, together with the modifier keys above. Disabled turns the pin shortcut off (Suggested: T).'
  $options:
  - "0": "Disabled"
  - "65": "A"
  - "66": "B"
  - "67": "C"
  - "68": "D"
  - "69": "E"
  - "70": "F"
  - "71": "G"
  - "72": "H"
  - "73": "I"
  - "74": "J"
  - "75": "K"
  - "76": "L"
  - "77": "M"
  - "78": "N"
  - "79": "O"
  - "80": "P"
  - "81": "Q"
  - "82": "R"
  - "83": "S"
  - "84": "T"
  - "85": "U"
  - "86": "V"
  - "87": "W"
  - "88": "X"
  - "89": "Y"
  - "90": "Z"
  - "48": "0"
  - "49": "1"
  - "50": "2"
  - "51": "3"
  - "52": "4"
  - "53": "5"
  - "54": "6"
  - "55": "7"
  - "56": "8"
  - "57": "9"
  - "112": "F1"
  - "113": "F2"
  - "114": "F3"
  - "115": "F4"
  - "116": "F5"
  - "117": "F6"
  - "118": "F7"
  - "119": "F8"
  - "120": "F9"
  - "121": "F10"
  - "122": "F11"
  - "123": "F12"
  - "32": "Space"
  - "13": "Enter"
  - "9": "Tab"
  - "8": "Backspace"
  - "46": "Delete"
  - "36": "Home"
  - "35": "End"
  - "33": "Page up"
  - "34": "Page down"
  - "37": "Left arrow"
  - "38": "Up arrow"
  - "39": "Right arrow"
  - "40": "Down arrow"
  - "186": "Semicolon"
  - "187": "Plus (+)"
  - "188": "Comma (,)"
  - "189": "Minus (-)"
  - "190": "Period (.)"
  - "191": "Slash (/)"
  - "192": "Backquote (`)"
  - "219": "Left bracket ([)"
  - "220": "Backslash (\\)"
  - "221": "Right bracket (])"
  - "222": "Quote (')"
  - "19": "Pause"
  - "44": "Print screen"
- hotkeyOpacityUp: "187"
  $name: More opaque shortcut key
  $description: 'Makes the active pinned window less see-through, in steps, using the same modifier keys as the pin shortcut (Suggested: +).'
  $options:
  - "0": "Disabled"
  - "187": "Plus (+)"
  - "189": "Minus (-)"
  - "65": "A"
  - "66": "B"
  - "67": "C"
  - "68": "D"
  - "69": "E"
  - "70": "F"
  - "71": "G"
  - "72": "H"
  - "73": "I"
  - "74": "J"
  - "75": "K"
  - "76": "L"
  - "77": "M"
  - "78": "N"
  - "79": "O"
  - "80": "P"
  - "81": "Q"
  - "82": "R"
  - "83": "S"
  - "84": "T"
  - "85": "U"
  - "86": "V"
  - "87": "W"
  - "88": "X"
  - "89": "Y"
  - "90": "Z"
  - "48": "0"
  - "49": "1"
  - "50": "2"
  - "51": "3"
  - "52": "4"
  - "53": "5"
  - "54": "6"
  - "55": "7"
  - "56": "8"
  - "57": "9"
  - "112": "F1"
  - "113": "F2"
  - "114": "F3"
  - "115": "F4"
  - "116": "F5"
  - "117": "F6"
  - "118": "F7"
  - "119": "F8"
  - "120": "F9"
  - "121": "F10"
  - "122": "F11"
  - "123": "F12"
- hotkeyOpacityDown: "189"
  $name: More transparent shortcut key
  $description: 'Makes the active pinned window more see-through, in steps, using the same modifier keys as the pin shortcut (Suggested: -).'
  $options:
  - "0": "Disabled"
  - "187": "Plus (+)"
  - "189": "Minus (-)"
  - "65": "A"
  - "66": "B"
  - "67": "C"
  - "68": "D"
  - "69": "E"
  - "70": "F"
  - "71": "G"
  - "72": "H"
  - "73": "I"
  - "74": "J"
  - "75": "K"
  - "76": "L"
  - "77": "M"
  - "78": "N"
  - "79": "O"
  - "80": "P"
  - "81": "Q"
  - "82": "R"
  - "83": "S"
  - "84": "T"
  - "85": "U"
  - "86": "V"
  - "87": "W"
  - "88": "X"
  - "89": "Y"
  - "90": "Z"
  - "48": "0"
  - "49": "1"
  - "50": "2"
  - "51": "3"
  - "52": "4"
  - "53": "5"
  - "54": "6"
  - "55": "7"
  - "56": "8"
  - "57": "9"
  - "112": "F1"
  - "113": "F2"
  - "114": "F3"
  - "115": "F4"
  - "116": "F5"
  - "117": "F6"
  - "118": "F7"
  - "119": "F8"
  - "120": "F9"
  - "121": "F10"
  - "122": "F11"
  - "123": "F12"
- frameEnabled: true
  $name: Show a border around the pinned window
  $description: When on, pinned windows get a colored ring so you can see they are pinned.
- frameColorMode: gradient
  $name: Border color
  $description: 'A gradient between the two colors below, your Windows accent color, or one plain color (Suggested: Gradient).'
  $options:
  - gradient: Gradient
  - accent: Windows accent color
  - custom: One color
- frameColor: "#F38BA8"
  $name: Border color, start
  $description: 'Top-left color of the gradient, or the only color when the border color is set to One color. A hex color (Suggested: #F38BA8, Catppuccin Mocha red).'
- frameColorTo: "#F9E2AF"
  $name: Border color, end
  $description: 'Bottom-right color of the gradient (Suggested: #F9E2AF, Catppuccin Mocha yellow).'
- frameOpacity: 100
  $name: Border opacity
  $description: 'How solid the border looks. 0 to 100 (Suggested: 100).'
- frameThickness: 4
  $name: Border thickness
  $description: 'Thickness of the border in pixels. 1 to 20 (Suggested: 4).'
- frameRoundCorners: true
  $name: Enable rounded corners
  $description: When on, the border rounds its corners to match the pinned window.
- frameCornerRadius: 0
  $name: Border corner radius
  $description: 'Corner radius of the border in pixels. 0 uses the Windows default, 8 for normal windows and 4 for small ones. Set it to match a corner mod (Suggested: 0).'
- soundEnabled: true
  $name: Play a sound when pinning a window
  $description: Plays the sounds below when you pin or unpin a window, and when you change opacity.
- soundOn: C:\Windows\Media\Speech On.wav
  $name: Sound for pin
  $description: A .wav file. If the file is missing, a system sound plays instead.
- soundOff: C:\Windows\Media\Speech Off.wav
  $name: Sound for unpin
  $description: A .wav file. If the file is missing, a system sound plays instead.
- soundOpacityUp: C:\Windows\Media\Windows Hardware Insert.wav
  $name: Sound for more opaque
  $description: A .wav file. If the file is missing, a system sound plays instead.
- soundOpacityDown: C:\Windows\Media\Windows Hardware Remove.wav
  $name: Sound for more transparent
  $description: A .wav file. If the file is missing, a system sound plays instead.
- notificationsEnabled: true
  $name: Show the status card
  $description: A small card at the bottom of the screen says Pinned or Unpinned.
- notificationDuration: 5000
  $name: Status card display time
  $description: 'How long the status card stays on screen, in milliseconds. 500 to 10000 (Suggested: 5000).'
- gameModeBlock: true
  $name: Do not activate when Game Mode is on
  $description: Stops the pin shortcut from working while a full-screen game is running.
- excludedApps: ""
  $name: Excluded apps
  $description: Apps that can never be pinned, separated by semicolons, for example Notepad.exe;Calc.exe. A partial name matches too, so Notepad blocks Notepad.exe and Notepad++.exe.
- showInSystemMenu: false
  $name: Show Always on Top in the title bar context menu
  $description: Adds an Always on Top entry to the menu you get when you right-click a window title bar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <math.h>
#include <algorithm>
#include <shellapi.h>
#include <mmsystem.h>
#include <map>
#include <unordered_set>
#include <string>
#include <vector>

// Non-ASCII literals avoided on purpose; keep this file pure ASCII.

static const WCHAR kHostClassName[] = L"AotPlusHostClass";
static const WCHAR kBorderClassName[] = L"AotPlusBorderClass";
static const WCHAR kNotifyClassName[] = L"AotPlusNotifyClass";
static const WCHAR kPinProp[] = L"AotPlus_Pinned";

static const UINT kHotkeyIdPin = 1;
static const UINT kHotkeyIdOpacityUp = 2;
static const UINT kHotkeyIdOpacityDown = 3;
static const UINT kBorderTimerId = 123;
static const UINT kVerifyTimerId = 124;
static const UINT kMaintainTimerId = 125;

static const UINT WM_APP_QUIT = WM_APP + 1;
static const UINT WM_APP_SETTINGS = WM_APP + 2;
static const UINT WM_APP_REBORDER = WM_APP + 3;
static const UINT WM_HOOK_SET = WM_APP + 20;

// Older SDK headers lack the cloak events.
static const DWORD EVENT_OBJECT_CLOAKED_AOT = 0x8017;
static const DWORD EVENT_OBJECT_UNCLOAKED_AOT = 0x8018;

static const UINT kSysMenuCommandId = 0xEFE0;
static const ULONG_PTR kSysMenuOwnerTag = 0x414F5450; // 'AOTP'

// PowerToys Always On Top transparency limits.
static const int kMinOpacityPercent = 20;
static const int kMaxOpacityPercent = 100;
static const int kOpacityStepPercent = 10;

enum SoundKind {
    kSoundPin,
    kSoundUnpin,
    kSoundOpacityUp,
    kSoundOpacityDown,
};

struct Settings {
    UINT hotkeyMods = MOD_WIN | MOD_CONTROL;
    UINT hotkeyVk = 'T';
    UINT opacityUpVk = VK_OEM_PLUS;
    UINT opacityDownVk = VK_OEM_MINUS;
    bool frameEnabled = true;
    bool frameUseAccent = false;
    bool frameGradient = true;
    COLORREF frameColor = RGB(0xF3, 0x8B, 0xA8);
    COLORREF frameColorTo = RGB(0xF9, 0xE2, 0xAF);
    int frameOpacity = 100;
    int frameThickness = 4;
    bool frameRoundCorners = true;
    int frameCornerRadius = 0;
    bool soundEnabled = true;
    std::wstring soundOn = L"C:\\Windows\\Media\\Speech On.wav";
    std::wstring soundOff = L"C:\\Windows\\Media\\Speech Off.wav";
    std::wstring soundOpacityUp = L"C:\\Windows\\Media\\Windows Hardware Insert.wav";
    std::wstring soundOpacityDown = L"C:\\Windows\\Media\\Windows Hardware Remove.wav";
    bool notificationsEnabled = true;
    int notificationDuration = 5000;
    bool gameModeBlock = true;
    std::vector<std::wstring> excludedApps;
    bool showInSystemMenu = false;
};

static CRITICAL_SECTION g_cs;
static Settings g_settings;

static HWND g_hostWnd = nullptr;
static HANDLE g_mainThread = nullptr;
// Main thread only: the hooks, the pinned-window map and the menu windows.
static std::vector<HWINEVENTHOOK> g_eventHooks; // only while a window is pinned
static std::vector<HWINEVENTHOOK> g_menuHooks;  // only while the menu entry is on
static void UpdateBorderHooks();
static HWND g_lastSysMenuWnd = nullptr;

// Windows we pinned, with the border state for each.
struct BorderEntry {
    HWND border = nullptr;
    HWND target = nullptr;
    ID2D1HwndRenderTarget* rt = nullptr;
    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_SIZE_U rtSize = {};
    // What is on screen now, so a tick that changes nothing does no work.
    bool shown = false;
    bool drawn = false;
    RECT lastFrame = {};
    COLORREF lastColor = 0;
    COLORREF lastColorTo = 0;
    bool lastGradient = false;
    float lastOpacity = -1.0f;
    int lastThickness = -1;
    float lastRadius = -1.0f;
    float lastTuck = -1.0f;
    int lastW = 0;
    int lastH = 0;
};
static std::map<HWND, BorderEntry> g_borders;

struct SavedLayered {
    bool hadLayered = false;
    BYTE alpha = 255;
    bool usedColorKey = false;
    COLORREF colorKey = 0;
    bool valid = false;
};
static std::map<HWND, SavedLayered> g_savedLayered;

// A pin or unpin is sent without waiting on the other app, then checked here
// a moment later, so a slow or frozen app can never stall the shortcut.
struct PendingCheck {
    HWND wnd;
    bool wantTopmost;
    int tries;
    ULONGLONG due;
};
static std::vector<PendingCheck> g_pendingChecks;
static bool g_menuEverEnabled = false;
// Windows whose system menu we added the entry to. Main thread only.
static std::unordered_set<HWND> g_menuWindows;

// ----- D2D, bound at runtime so the mod links without d2d1 import libs -----

static const IID kD2DFactoryIID = {
    0x06152247, 0x6f50, 0x465a, {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};

typedef HRESULT(WINAPI* PFN_D2D1CreateFactory)(D2D1_FACTORY_TYPE, REFIID,
                                               const D2D1_FACTORY_OPTIONS*, void**);

static ID2D1Factory* GetD2DFactory() {
    static ID2D1Factory* factory = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE d2d = LoadLibraryW(L"d2d1.dll");
        if (d2d) {
            auto create = (PFN_D2D1CreateFactory)GetProcAddress(d2d, "D2D1CreateFactory");
            if (create) {
                D2D1_FACTORY_OPTIONS options = {};
                options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
                create(D2D1_FACTORY_TYPE_MULTI_THREADED, kD2DFactoryIID, &options,
                       (void**)&factory);
            }
        }
    }
    return factory;
}

// ----- IVirtualDesktopManager, declared locally for older headers -----

struct IAotVirtualDesktopManager : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentDesktop(HWND, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(HWND, GUID*) = 0;
};

static const GUID kCLSID_VirtualDesktopManager = {
    0xaa509086, 0x5ca9, 0x4c25, {0x8f, 0x95, 0x58, 0x1d, 0x3c, 0x64, 0x3f, 0x89}};
static const GUID kIID_IVirtualDesktopManager = {
    0x37236034, 0x3fa1, 0x4d0d, {0xa3, 0xfc, 0x4d, 0x2a, 0x4c, 0x90, 0xdb, 0xdc}};

// Created and released on the main thread, which owns the COM apartment.
static IAotVirtualDesktopManager* g_vdm = nullptr;
static bool g_vdmTried = false;

static IAotVirtualDesktopManager* GetVdm() {
    if (!g_vdmTried) {
        g_vdmTried = true;
        CoCreateInstance(kCLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL,
                         kIID_IVirtualDesktopManager, (void**)&g_vdm);
        if (!g_vdm) {
            Wh_Log(L"[aot-plus] no virtual desktop manager; desktop checks pass everything");
        }
    }
    return g_vdm;
}

static void ReleaseVdm() {
    if (g_vdm) {
        g_vdm->Release();
        g_vdm = nullptr;
    }
    g_vdmTried = false;
}

static bool IsWindowOnCurrentDesktop(HWND wnd) {
    auto* vdm = GetVdm();
    if (!vdm) {
        return true;
    }
    BOOL on = TRUE;
    if (FAILED(vdm->IsWindowOnCurrentDesktop(wnd, &on))) {
        return true;
    }
    return on != FALSE;
}

// ----- Small helpers -----

static std::wstring GetWindowProcessPath(HWND wnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(wnd, &pid);
    std::wstring path;
    if (!pid) {
        return path;
    }
    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc) {
        return path;
    }
    WCHAR buf[MAX_PATH * 2] = {};
    DWORD size = ARRAYSIZE(buf);
    if (QueryFullProcessImageNameW(proc, 0, buf, &size) && size > 0) {
        path = buf;
    }
    CloseHandle(proc);
    return path;
}

static bool IsWindowProcessElevated(HWND wnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(wnd, &pid);
    if (!pid) {
        return false;
    }
    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc) {
        return false;
    }
    bool elevated = false;
    HANDLE token = nullptr;
    if (OpenProcessToken(proc, TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elev = {};
        DWORD ret = 0;
        if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &ret)) {
            elevated = elev.TokenIsElevated != 0;
        }
        CloseHandle(token);
    }
    CloseHandle(proc);
    return elevated;
}

static bool IsCurrentProcessElevated() {
    bool elevated = false;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elev = {};
        DWORD ret = 0;
        if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &ret)) {
            elevated = elev.TokenIsElevated != 0;
        }
        CloseHandle(token);
    }
    return elevated;
}

static bool GameModeActive() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SHQueryUserNotificationState(&state) != S_OK) {
        return false;
    }
    return state == QUNS_RUNNING_D3D_FULL_SCREEN;
}

// PowerToys rule: an entry matches when it appears in the process path right
// after the last backslash (so "Notepad" matches Notepad and Notepad++), or
// when it appears anywhere in the window title.
static bool IsExcludedWindow(HWND wnd) {
    std::vector<std::wstring> apps;
    EnterCriticalSection(&g_cs);
    apps = g_settings.excludedApps;
    LeaveCriticalSection(&g_cs);
    if (apps.empty()) {
        return false;
    }

    std::wstring path = GetWindowProcessPath(wnd);
    if (!path.empty()) {
        CharUpperBuffW(path.data(), (DWORD)path.length());
        for (const auto& row : apps) {
            const auto pos = path.rfind(row);
            const auto lastSlash = path.rfind(L'\\');
            if (pos != std::wstring::npos && pos <= lastSlash + 1 &&
                pos + row.length() > lastSlash) {
                return true;
            }
        }
    }

    WCHAR title[256] = {};
    int len = GetWindowTextW(wnd, title, 256);
    if (len > 0) {
        std::wstring titleStr(title);
        CharUpperBuffW(titleStr.data(), (DWORD)titleStr.length());
        for (const auto& row : apps) {
            if (titleStr.find(row) != std::wstring::npos) {
                return true;
            }
        }
    }
    return false;
}

// Shell surfaces that must never be pinned.
static bool IsSystemSurface(HWND wnd) {
    WCHAR cls[64] = {};
    if (GetClassNameW(wnd, cls, 64) == 0) {
        return false;
    }
    static const LPCWSTR kBlocked[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow",
        L"TopLevelWindowForOverflowXamlIsland", L"TaskListEvents",
    };
    for (LPCWSTR name : kBlocked) {
        if (wcscmp(cls, name) == 0) {
            return true;
        }
    }
    return false;
}

static bool IsWindowCloaked(HWND wnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(wnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked != 0;
}

static bool IsTopmost(HWND wnd) {
    return (GetWindowLongW(wnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
}

static bool IsOwnWindow(HWND wnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(wnd, &pid);
    return pid == GetCurrentProcessId();
}

// Z-order flags for another app's window: never activate it, and never wait
// on it. SWP_ASYNCWINDOWPOS posts the change to the app's own thread, so a
// busy app (Steam, a frozen game) cannot hold up the shortcut.
static UINT ForeignPosFlags(HWND wnd) {
    UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
    if (!IsOwnWindow(wnd)) {
        flags |= SWP_ASYNCWINDOWPOS;
    }
    return flags;
}

// GetDpiForWindow and GetDpiForMonitor are missing from the compiler's
// headers, so bind them at runtime and fall back to GetDeviceCaps.
typedef UINT(WINAPI* PFN_GetDpiForWindow)(HWND);

static UINT GetWindowDpi(HWND wnd) {
    static PFN_GetDpiForWindow fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        fn = user32 ? (PFN_GetDpiForWindow)GetProcAddress(user32, "GetDpiForWindow") : nullptr;
    }
    if (fn) {
        UINT dpi = fn(wnd);
        if (dpi) {
            return dpi;
        }
    }
    HDC hdc = GetDC(wnd);
    UINT dpi = hdc ? (UINT)GetDeviceCaps(hdc, LOGPIXELSX) : 96;
    if (hdc) {
        ReleaseDC(wnd, hdc);
    }
    return dpi ? dpi : 96;
}

typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, UINT, UINT*, UINT*);

static UINT GetMonitorDpi(HMONITOR mon) {
    static PFN_GetDpiForMonitor fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE shcore = LoadLibraryW(L"Shcore.dll");
        fn = shcore ? (PFN_GetDpiForMonitor)GetProcAddress(shcore, "GetDpiForMonitor") : nullptr;
    }
    UINT x = 0;
    UINT y = 0;
    if (fn && SUCCEEDED(fn(mon, 0 /* MDT_EFFECTIVE_DPI */, &x, &y)) && x) {
        return x;
    }
    return 96;
}

static float ScalingFactor(HWND wnd) {
    UINT dpi = GetWindowDpi(wnd);
    if (dpi == 0) {
        dpi = 96;
    }
    return dpi / 96.0f;
}

// Corner radius in px at 96 dpi: the frameCornerRadius setting when it is set,
// otherwise the Windows default passed in (8 normal, 4 small).
static float SystemCornerRadius(float windowsRadius) {
    int custom;
    EnterCriticalSection(&g_cs);
    custom = g_settings.frameCornerRadius;
    LeaveCriticalSection(&g_cs);
    return custom > 0 ? (float)custom : windowsRadius;
}

// Corner radius of a window in px at 96 dpi: the DWM corner preference, then
// the radius setting. Maximized windows have square corners.
static float WindowCornersRadius(HWND wnd) {
    if (IsZoomed(wnd)) {
        return 0.0f;
    }
    int pref = DWMWCP_DEFAULT;
    DwmGetWindowAttribute(wnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    switch (pref) {
        case DWMWCP_ROUND:
            return SystemCornerRadius(8.0f);
        case DWMWCP_ROUNDSMALL:
            return SystemCornerRadius(4.0f);
        case DWMWCP_DEFAULT: {
            // Windows rounds framed windows on its own; bare popups stay square.
            LONG style = GetWindowLongW(wnd, GWL_STYLE);
            if ((style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME)) {
                return SystemCornerRadius(8.0f);
            }
            return 0.0f;
        }
        default:
            return 0.0f;
    }
}

// Visible frame bounds of the target window, expanded by the border thickness,
// exactly like PowerToys GetFrameRect.
static bool GetTrackingFrameRect(HWND wnd, RECT* out) {
    if (FAILED(DwmGetWindowAttribute(wnd, DWMWA_EXTENDED_FRAME_BOUNDS, out, sizeof(RECT)))) {
        return false;
    }
    int thickness;
    EnterCriticalSection(&g_cs);
    thickness = g_settings.frameThickness;
    LeaveCriticalSection(&g_cs);
    int border = (int)(thickness * ScalingFactor(wnd));
    out->top -= border;
    out->left -= border;
    out->right += border;
    out->bottom += border;
    return true;
}

static COLORREF HexToRGB(const std::wstring& hex, COLORREF fallback) {
    std::wstring s = hex;
    // Trim spaces and an optional leading '#'.
    size_t start = s.find_first_not_of(L" \t");
    if (start == std::wstring::npos) {
        return fallback;
    }
    size_t end = s.find_last_not_of(L" \t");
    s = s.substr(start, end - start + 1);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return fallback;
    }
    wchar_t* stop = nullptr;
    long long v = wcstoll(s.c_str(), &stop, 16);
    if (stop && *stop != L'\0') {
        return fallback;
    }
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static UINT ParseVkString(PCWSTR s, UINT fallback) {
    if (!s || !*s) {
        return fallback;
    }
    wchar_t* stop = nullptr;
    unsigned long v = wcstoul(s, &stop, 10);
    if (stop && *stop != L'\0') {
        return fallback;
    }
    if (v > 255) {
        return fallback;
    }
    return (UINT)v;
}

// Accent color from the DWM colorization value (ABGR), like the PowerToys
// accent setting but without WinRT.
static COLORREF GetAccentColor() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\DWM", L"ColorizationColor",
                     RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return RGB(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF);
    }
    return RGB(0, 173, 239);
}

// ----- Settings -----

static void LoadSettingsLocked() {
    g_settings.hotkeyMods = 0;
    if (Wh_GetIntSetting(L"hotkeyWin")) {
        g_settings.hotkeyMods |= MOD_WIN;
    }
    if (Wh_GetIntSetting(L"hotkeyCtrl")) {
        g_settings.hotkeyMods |= MOD_CONTROL;
    }
    if (Wh_GetIntSetting(L"hotkeyAlt")) {
        g_settings.hotkeyMods |= MOD_ALT;
    }
    if (Wh_GetIntSetting(L"hotkeyShift")) {
        g_settings.hotkeyMods |= MOD_SHIFT;
    }
    if (g_settings.hotkeyMods == 0) {
        g_settings.hotkeyMods = MOD_WIN | MOD_CONTROL;
    }

    PCWSTR key = Wh_GetStringSetting(L"hotkeyKey");
    // 0 is Disabled: no pin shortcut is registered at all.
    g_settings.hotkeyVk = ParseVkString(key, 'T');
    Wh_FreeStringSetting(key);

    PCWSTR up = Wh_GetStringSetting(L"hotkeyOpacityUp");
    g_settings.opacityUpVk = ParseVkString(up, VK_OEM_PLUS);
    Wh_FreeStringSetting(up);

    PCWSTR down = Wh_GetStringSetting(L"hotkeyOpacityDown");
    g_settings.opacityDownVk = ParseVkString(down, VK_OEM_MINUS);
    Wh_FreeStringSetting(down);

    g_settings.frameEnabled = Wh_GetIntSetting(L"frameEnabled") != 0;

    PCWSTR colorMode = Wh_GetStringSetting(L"frameColorMode");
    g_settings.frameUseAccent = wcscmp(colorMode, L"accent") == 0;
    g_settings.frameGradient = !g_settings.frameUseAccent && wcscmp(colorMode, L"custom") != 0;
    Wh_FreeStringSetting(colorMode);

    PCWSTR color = Wh_GetStringSetting(L"frameColor");
    g_settings.frameColor = HexToRGB(color, RGB(0xF3, 0x8B, 0xA8));
    Wh_FreeStringSetting(color);

    PCWSTR colorTo = Wh_GetStringSetting(L"frameColorTo");
    g_settings.frameColorTo = HexToRGB(colorTo, RGB(0xF9, 0xE2, 0xAF));
    Wh_FreeStringSetting(colorTo);

    int opacity = Wh_GetIntSetting(L"frameOpacity");
    if (opacity < 0) {
        opacity = 0;
    }
    if (opacity > 100) {
        opacity = 100;
    }
    g_settings.frameOpacity = opacity;

    int thickness = Wh_GetIntSetting(L"frameThickness");
    if (thickness < 1) {
        thickness = 1;
    }
    if (thickness > 20) {
        thickness = 20;
    }
    g_settings.frameThickness = thickness;

    g_settings.frameRoundCorners = Wh_GetIntSetting(L"frameRoundCorners") != 0;

    int cornerRadius = Wh_GetIntSetting(L"frameCornerRadius");
    if (cornerRadius < 0) {
        cornerRadius = 0;
    }
    if (cornerRadius > 64) {
        cornerRadius = 64;
    }
    g_settings.frameCornerRadius = cornerRadius;

    g_settings.soundEnabled = Wh_GetIntSetting(L"soundEnabled") != 0;

    PCWSTR on = Wh_GetStringSetting(L"soundOn");
    g_settings.soundOn = (on && *on) ? on : L"C:\\Windows\\Media\\Speech On.wav";
    Wh_FreeStringSetting(on);

    PCWSTR off = Wh_GetStringSetting(L"soundOff");
    g_settings.soundOff = (off && *off) ? off : L"C:\\Windows\\Media\\Speech Off.wav";
    Wh_FreeStringSetting(off);

    PCWSTR upSnd = Wh_GetStringSetting(L"soundOpacityUp");
    g_settings.soundOpacityUp =
        (upSnd && *upSnd) ? upSnd : L"C:\\Windows\\Media\\Windows Hardware Insert.wav";
    Wh_FreeStringSetting(upSnd);

    PCWSTR downSnd = Wh_GetStringSetting(L"soundOpacityDown");
    g_settings.soundOpacityDown =
        (downSnd && *downSnd) ? downSnd : L"C:\\Windows\\Media\\Windows Hardware Remove.wav";
    Wh_FreeStringSetting(downSnd);

    g_settings.notificationsEnabled = Wh_GetIntSetting(L"notificationsEnabled") != 0;

    int duration = Wh_GetIntSetting(L"notificationDuration");
    if (duration < 500) {
        duration = 500;
    }
    if (duration > 10000) {
        duration = 10000;
    }
    g_settings.notificationDuration = duration;

    g_settings.gameModeBlock = Wh_GetIntSetting(L"gameModeBlock") != 0;

    PCWSTR apps = Wh_GetStringSetting(L"excludedApps");
    g_settings.excludedApps.clear();
    if (apps && *apps) {
        std::wstring list = apps;
        size_t pos = 0;
        while (pos < list.size()) {
            size_t next = list.find(L';', pos);
            std::wstring item = list.substr(pos, (next == std::wstring::npos)
                                                     ? std::wstring::npos
                                                     : next - pos);
            // Trim spaces around the entry.
            size_t s = item.find_first_not_of(L" \t");
            if (s != std::wstring::npos) {
                size_t e = item.find_last_not_of(L" \t");
                item = item.substr(s, e - s + 1);
                CharUpperBuffW(item.data(), (DWORD)item.length());
                g_settings.excludedApps.push_back(item);
            }
            if (next == std::wstring::npos) {
                break;
            }
            pos = next + 1;
        }
    }
    Wh_FreeStringSetting(apps);

    g_settings.showInSystemMenu = Wh_GetIntSetting(L"showInSystemMenu") != 0;
}

static void LoadSettings() {
    EnterCriticalSection(&g_cs);
    LoadSettingsLocked();
    LeaveCriticalSection(&g_cs);
    Wh_Log(L"[aot-plus] settings loaded");
}

static Settings GetSettingsCopy() {
    EnterCriticalSection(&g_cs);
    Settings copy = g_settings;
    LeaveCriticalSection(&g_cs);
    return copy;
}

// ----- Sound -----

static void PlaySoundFor(SoundKind kind) {
    Settings s = GetSettingsCopy();
    if (!s.soundEnabled) {
        return;
    }
    const std::wstring* path = nullptr;
    switch (kind) {
        case kSoundPin:
            path = &s.soundOn;
            break;
        case kSoundUnpin:
            path = &s.soundOff;
            break;
        case kSoundOpacityUp:
            path = &s.soundOpacityUp;
            break;
        case kSoundOpacityDown:
            path = &s.soundOpacityDown;
            break;
    }
    if (!path || path->empty()) {
        return;
    }
    if (GetFileAttributesW(path->c_str()) != INVALID_FILE_ATTRIBUTES) {
        if (PlaySoundW(path->c_str(), nullptr, SND_FILENAME | SND_ASYNC)) {
            return;
        }
    }
    if (PlaySoundW(path->c_str(), nullptr, SND_ALIAS | SND_ASYNC)) {
        return;
    }
    MessageBeep(kind == kSoundPin ? MB_ICONASTERISK : MB_ICONEXCLAMATION);
}

// ----- Status card: Windows' acrylic glass on its own thread -----
//
// The card has its own thread, so it always fades out on time, even while the
// main thread waits on a slow window. It is a layered popup with Windows'
// acrylic blur behind it. Each frame is drawn by Direct2D and timed to the
// compositor (DwmFlush), so the fade and slide follow the display's refresh.

enum CardIcon { kCardIconNone, kCardIconPin, kCardIconUnpin };

struct CardRequest {
    std::wstring text;
    CardIcon icon = kCardIconNone;
    int durationMs = 5000;
};

static const UINT WM_CARD_SHOW = WM_APP + 10;
static const UINT WM_CARD_QUIT = WM_APP + 11;
static const double kCardFadeIn = 0.18;
// Mocha base over the acrylic blur: Windows' tint (AABBGGRR) plus an even wash
// drawn by the card, together dark enough that text behind is unreadable.
static const DWORD kCardAcrylicTint = 0xB82E1E1E;
static const float kCardWash = 0.28f;
static const double kCardFadeOut = 0.22;

static HANDLE g_cardThread = nullptr;
static HANDLE g_cardReady = nullptr;
static HWND g_cardWnd = nullptr;

typedef HRESULT(WINAPI* PFN_DWriteCreateFactory)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
static const IID kDWriteFactoryIID = {
    0xb859ee5a, 0xd838, 0x4b5b, {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

// Card state, touched only on the card thread.
struct CardState {
    IDWriteFactory* dw = nullptr;
    ID2D1DCRenderTarget* rt = nullptr;
    HDC memDC = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int bmpW = 0;
    int bmpH = 0;
    bool haveNerdFont = false;

    std::wstring text;
    CardIcon icon = kCardIconNone;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    float scale = 1.0f;
    bool hover = false;
    bool tracking = false;

    enum Phase { kHidden, kIn, kHold, kOut } phase = kHidden;
    double phaseStart = 0;
    double holdUntil = 0;
    double holdMs = 5000;
    float alpha = 0;
    float alphaAtPhaseStart = 0;
    double lastRaise = 0;
};
static CardState g_cardState;

static double NowSeconds() {
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart) {
        QueryPerformanceFrequency(&freq);
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / (double)freq.QuadPart;
}

static float EaseOutCubic(float t) {
    t = 1.0f - t;
    return 1.0f - t * t * t;
}

static float EaseInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static D2D1_COLOR_F CardColor(UINT32 rgb, float a) {
    return D2D1_COLOR_F{((rgb >> 16) & 0xFF) / 255.0f, ((rgb >> 8) & 0xFF) / 255.0f,
                        (rgb & 0xFF) / 255.0f, a};
}

static IDWriteTextFormat* CardTextFormat(float size, bool icon) {
    CardState& c = g_cardState;
    IDWriteTextFormat* format = nullptr;
    PCWSTR family = c.haveNerdFont ? L"FiraCode Nerd Font" : L"Segoe UI Variable Text";
    if (FAILED(c.dw->CreateTextFormat(family, nullptr,
                                      icon ? DWRITE_FONT_WEIGHT_NORMAL
                                           : DWRITE_FONT_WEIGHT_MEDIUM,
                                      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                      size, L"en-us", &format))) {
        return nullptr;
    }
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    return format;
}

static PCWSTR CardGlyph(CardIcon icon) {
    // Material Design pin and pin-off glyphs from the Nerd Font.
    switch (icon) {
        case kCardIconPin:
            return L"\U000F0403";
        case kCardIconUnpin:
            return L"\U000F0404";
        default:
            return nullptr;
    }
}

static float MeasureText(IDWriteTextFormat* format, PCWSTR text, float* height) {
    IDWriteTextLayout* layout = nullptr;
    float width = 0;
    if (SUCCEEDED(g_cardState.dw->CreateTextLayout(text, (UINT32)wcslen(text), format,
                                                   10000.0f, 1000.0f, &layout))) {
        DWRITE_TEXT_METRICS m = {};
        layout->GetMetrics(&m);
        width = m.widthIncludingTrailingWhitespace;
        if (height) {
            *height = m.height;
        }
        layout->Release();
    }
    return width;
}

static void CardFreeSurface() {
    CardState& c = g_cardState;
    if (c.memDC) {
        SelectObject(c.memDC, c.oldBmp);
        DeleteDC(c.memDC);
        c.memDC = nullptr;
    }
    if (c.bmp) {
        DeleteObject(c.bmp);
        c.bmp = nullptr;
    }
    c.bmpW = c.bmpH = 0;
}

// Draws the card into the premultiplied bitmap that UpdateLayeredWindow shows.
static void CardRender() {
    CardState& c = g_cardState;
    if (c.w <= 0 || c.h <= 0 || !c.rt) {
        return;
    }
    if (!c.memDC || c.bmpW != c.w || c.bmpH != c.h) {
        CardFreeSurface();
        HDC screen = GetDC(nullptr);
        c.memDC = CreateCompatibleDC(screen);
        ReleaseDC(nullptr, screen);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = c.w;
        bi.bmiHeader.biHeight = -c.h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        void* bits = nullptr;
        c.bmp = CreateDIBSection(c.memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!c.bmp) {
            CardFreeSurface();
            return;
        }
        c.oldBmp = SelectObject(c.memDC, c.bmp);
        c.bmpW = c.w;
        c.bmpH = c.h;
    }

    RECT bind = {0, 0, c.w, c.h};
    if (FAILED(c.rt->BindDC(c.memDC, &bind))) {
        return;
    }
    const float s = c.scale;
    const float w = (float)c.w;
    const float h = (float)c.h;
    // DWM rounds the card's corners with the system radius (the radius setting
    // when it is set). The ring is everything outside an inner outline concentric
    // with that curve, and DWM's own anti-aliased clip makes its outer edge, so
    // the ring keeps one thickness all the way round.
    const float radius = std::min(SystemCornerRadius(8.0f) * s, h / 2.0f);
    const float stroke = std::max(2.0f, roundf(2.0f * s));

    ID2D1SolidColorBrush* brush = nullptr;
    c.rt->BeginDraw();
    c.rt->Clear(D2D1_COLOR_F{0, 0, 0, 0});
    if (SUCCEEDED(c.rt->CreateSolidColorBrush(CardColor(0x1E1E2E, kCardWash), &brush))) {
        // One even Mocha wash over the blur: dark glass, no sheen, no gradient.
        c.rt->FillRectangle({0, 0, w, h}, brush);

        // The ring: blue at rest, blue into mauve under the mouse.
        ID2D1Geometry* ringGeo = nullptr;
        {
            ID2D1RectangleGeometry* outer = nullptr;
            ID2D1RoundedRectangleGeometry* inner = nullptr;
            float innerRadius = std::max(0.0f, radius - stroke);
            D2D1_ROUNDED_RECT innerRR = {{stroke, stroke, w - stroke, h - stroke}, innerRadius,
                                         innerRadius};
            GetD2DFactory()->CreateRectangleGeometry({0, 0, w, h}, &outer);
            GetD2DFactory()->CreateRoundedRectangleGeometry(innerRR, &inner);
            if (outer && inner) {
                ID2D1Geometry* parts[2] = {outer, inner};
                ID2D1GeometryGroup* group = nullptr;
                if (SUCCEEDED(GetD2DFactory()->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE,
                                                                   parts, 2, &group))) {
                    ringGeo = group;
                }
            }
            if (outer) {
                outer->Release();
            }
            if (inner) {
                inner->Release();
            }
        }
        ID2D1LinearGradientBrush* gradient = nullptr;
        if (c.hover) {
            D2D1_GRADIENT_STOP stops[2] = {{0.0f, CardColor(0x89B4FA, 1.0f)},
                                           {1.0f, CardColor(0xCBA6F7, 1.0f)}};
            ID2D1GradientStopCollection* coll = nullptr;
            if (SUCCEEDED(c.rt->CreateGradientStopCollection(stops, 2, &coll))) {
                D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp = {{0, 0}, {w, h}};
                c.rt->CreateLinearGradientBrush(lp, {1.0f, D2D1::IdentityMatrix()}, coll,
                                                &gradient);
                coll->Release();
            }
        }
        if (ringGeo) {
            if (gradient) {
                c.rt->FillGeometry(ringGeo, gradient);
            } else {
                brush->SetColor(CardColor(0x89B4FA, 0.9f));
                c.rt->FillGeometry(ringGeo, brush);
            }
            ringGeo->Release();
        }
        if (gradient) {
            gradient->Release();
        }

        // Icon and text, centred together.
        IDWriteTextFormat* textFormat = CardTextFormat(13.0f * s, false);
        PCWSTR glyph = c.haveNerdFont ? CardGlyph(c.icon) : nullptr;
        IDWriteTextFormat* iconFormat = glyph ? CardTextFormat(16.0f * s, true) : nullptr;
        if (textFormat) {
            float textW = MeasureText(textFormat, c.text.c_str(), nullptr);
            float iconW = iconFormat ? MeasureText(iconFormat, glyph, nullptr) : 0.0f;
            float gap = iconFormat ? 8.0f * s : 0.0f;
            float left = (w - (iconW + gap + textW)) / 2.0f;
            if (iconFormat) {
                brush->SetColor(c.icon == kCardIconPin ? CardColor(0x89B4FA, 1.0f)
                                                       : CardColor(0xA6ADC8, 1.0f));
                c.rt->DrawTextW(glyph, (UINT32)wcslen(glyph), iconFormat,
                                {left, 0, left + iconW + 1, h}, brush);
            }
            brush->SetColor(CardColor(0xCDD6F4, 1.0f));
            float tx = left + iconW + gap;
            c.rt->DrawTextW(c.text.c_str(), (UINT32)c.text.size(), textFormat,
                            {tx, 0, tx + textW + 1, h}, brush);
            textFormat->Release();
        }
        if (iconFormat) {
            iconFormat->Release();
        }
        brush->Release();
    }
    if (c.rt->EndDraw() == (HRESULT)D2DERR_RECREATE_TARGET) {
        c.rt->Release();
        c.rt = nullptr;
        D2D1_RENDER_TARGET_PROPERTIES props = {};
        props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        props.pixelFormat = {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED};
        GetD2DFactory()->CreateDCRenderTarget(&props, &c.rt);
    }
}

// Shows the bitmap at the current fade level; position slides with the fade.
static void CardPresent() {
    CardState& c = g_cardState;
    if (!g_cardWnd || !c.memDC) {
        return;
    }
    float slide = 10.0f * c.scale * (1.0f - c.alpha);
    POINT dst = {c.x, c.y + (int)roundf(slide)};
    SIZE size = {c.w, c.h};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)roundf(c.alpha * 255.0f), AC_SRC_ALPHA};
    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(g_cardWnd, screen, &dst, &size, c.memDC, &src, 0, &blend, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

// Above every window, pinned ones too: a pinned window becomes topmost after
// the card was, so the card is raised again while it shows.
static void CardRaise(bool force) {
    CardState& c = g_cardState;
    double now = NowSeconds();
    if (!force && now - c.lastRaise < 0.1) {
        return;
    }
    c.lastRaise = now;
    SetWindowPos(g_cardWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

static void CardStartPhase(CardState::Phase phase) {
    CardState& c = g_cardState;
    c.phase = phase;
    c.phaseStart = NowSeconds();
    c.alphaAtPhaseStart = c.alpha;
}

static void CardLayout(const CardRequest& req) {
    CardState& c = g_cardState;
    HWND fg = GetForegroundWindow();
    HMONITOR mon = MonitorFromWindow(fg ? fg : g_cardWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    RECT work = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    UINT dpi = 96;
    if (GetMonitorInfoW(mon, &mi)) {
        work = mi.rcWork;
        dpi = GetMonitorDpi(mon);
    }
    c.scale = (dpi ? dpi : 96) / 96.0f;
    c.text = req.text;
    c.icon = req.icon;

    float s = c.scale;
    float textH = 0;
    float textW = 0;
    float iconW = 0;
    if (IDWriteTextFormat* tf = CardTextFormat(13.0f * s, false)) {
        textW = MeasureText(tf, c.text.c_str(), &textH);
        tf->Release();
    }
    PCWSTR glyph = c.haveNerdFont ? CardGlyph(c.icon) : nullptr;
    if (glyph) {
        if (IDWriteTextFormat* icf = CardTextFormat(16.0f * s, true)) {
            iconW = MeasureText(icf, glyph, nullptr) + 8.0f * s;
            icf->Release();
        }
    }
    c.w = (int)ceilf(textW + iconW + 2 * 18.0f * s);
    c.h = (int)ceilf(std::max(38.0f * s, textH + 2 * 10.0f * s));
    c.x = work.left + ((work.right - work.left) - c.w) / 2;
    c.y = work.bottom - c.h - (int)(24.0f * s);
}

static void CardShow(CardRequest* req) {
    CardState& c = g_cardState;
    c.holdMs = std::max(500, req->durationMs);
    CardLayout(*req);
    CardRender();
    if (c.phase == CardState::kHidden) {
        c.alpha = 0;
        CardPresent();
        ShowWindow(g_cardWnd, SW_SHOWNOACTIVATE);
    }
    CardRaise(true);
    if (c.phase == CardState::kHidden || c.phase == CardState::kOut) {
        CardStartPhase(CardState::kIn);
    }
    c.holdUntil = NowSeconds() + kCardFadeIn + c.holdMs / 1000.0;
    CardPresent();
}

// One animation step. Returns true while a frame-by-frame fade runs.
static bool CardTick() {
    CardState& c = g_cardState;
    double now = NowSeconds();
    double t = now - c.phaseStart;
    switch (c.phase) {
        case CardState::kHidden:
            return false;
        case CardState::kIn: {
            double dur = kCardFadeIn * std::max(0.25f, 1.0f - c.alphaAtPhaseStart);
            float p = (float)std::min(1.0, t / dur);
            c.alpha = c.alphaAtPhaseStart + (1.0f - c.alphaAtPhaseStart) * EaseOutCubic(p);
            CardRaise(false);
            CardPresent();
            if (p >= 1.0f) {
                c.alpha = 1.0f;
                CardStartPhase(CardState::kHold);
            }
            return true;
        }
        case CardState::kHold:
            CardRaise(false);
            if (c.hover) {
                c.holdUntil = std::max(c.holdUntil, now + 1.0);
            } else if (now >= c.holdUntil) {
                CardStartPhase(CardState::kOut);
                return true;
            }
            return false;
        case CardState::kOut: {
            double dur = kCardFadeOut * std::max(0.25f, c.alphaAtPhaseStart);
            float p = (float)std::min(1.0, t / dur);
            c.alpha = c.alphaAtPhaseStart * (1.0f - EaseInOutCubic(p));
            CardPresent();
            if (p >= 1.0f) {
                c.alpha = 0;
                c.phase = CardState::kHidden;
                ShowWindow(g_cardWnd, SW_HIDE);
                return false;
            }
            return true;
        }
    }
    return false;
}

static LRESULT CALLBACK CardWndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    CardState& c = g_cardState;
    switch (msg) {
        case WM_CARD_SHOW: {
            CardRequest* req = (CardRequest*)lp;
            if (req) {
                CardShow(req);
                delete req;
            }
            return 0;
        }
        case WM_CARD_QUIT:
            DestroyWindow(wnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE:
            if (!c.tracking) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, wnd, 0};
                c.tracking = TrackMouseEvent(&tme) != FALSE;
            }
            if (!c.hover && c.phase != CardState::kHidden) {
                c.hover = true;
                CardRender();
                if (c.phase == CardState::kOut) {
                    CardStartPhase(CardState::kIn);
                }
                CardPresent();
            }
            return 0;
        case WM_MOUSELEAVE:
            c.tracking = false;
            if (c.hover) {
                c.hover = false;
                c.holdUntil = std::max(c.holdUntil, NowSeconds() + 1.2);
                CardRender();
                CardPresent();
            }
            return 0;
        case WM_LBUTTONUP:
            // A click dismisses the card.
            if (c.phase == CardState::kIn || c.phase == CardState::kHold) {
                c.hover = false;
                CardStartPhase(CardState::kOut);
            }
            return 0;
    }
    return DefWindowProcW(wnd, msg, wp, lp);
}

static void CardApplyGlass(HWND wnd) {
    // Windows' acrylic blur behind the window, tinted Mocha base.
    struct AccentPolicy {
        int state;
        int flags;
        DWORD gradientColor;
        int animationId;
    };
    struct CompositionData {
        int attribute;
        void* data;
        SIZE_T size;
    };
    typedef BOOL(WINAPI* PFN_SetWindowCompositionAttribute)(HWND, CompositionData*);
    static PFN_SetWindowCompositionAttribute setAttr =
        (PFN_SetWindowCompositionAttribute)GetProcAddress(GetModuleHandleW(L"user32.dll"),
                                                          "SetWindowCompositionAttribute");
    if (setAttr) {
        AccentPolicy accent = {4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */, 2, kCardAcrylicTint, 0};
        CompositionData data = {19 /* WCA_ACCENT_POLICY */, &accent, sizeof(accent)};
        setAttr(wnd, &data);
    }
    DWORD corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(wnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    COLORREF noBorder = 0xFFFFFFFE; // DWMWA_COLOR_NONE: the card draws its own ring
    DwmSetWindowAttribute(wnd, 34 /* DWMWA_BORDER_COLOR */, &noBorder, sizeof(noBorder));
    BOOL dark = TRUE;
    DwmSetWindowAttribute(wnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    BOOL excluded = TRUE;
    DwmSetWindowAttribute(wnd, DWMWA_EXCLUDED_FROM_PEEK, &excluded, sizeof(excluded));
}

static DWORD WINAPI CardThread(LPVOID) {
    CardState& c = g_cardState;
    HMODULE dwrite = LoadLibraryW(L"dwrite.dll");
    auto create = dwrite ? (PFN_DWriteCreateFactory)GetProcAddress(dwrite, "DWriteCreateFactory")
                         : nullptr;
    if (create) {
        create(DWRITE_FACTORY_TYPE_SHARED, kDWriteFactoryIID, (IUnknown**)&c.dw);
    }
    if (c.dw) {
        IDWriteFontCollection* fonts = nullptr;
        if (SUCCEEDED(c.dw->GetSystemFontCollection(&fonts, FALSE))) {
            UINT32 index = 0;
            BOOL exists = FALSE;
            fonts->FindFamilyName(L"FiraCode Nerd Font", &index, &exists);
            c.haveNerdFont = exists != FALSE;
            fonts->Release();
        }
    }
    if (ID2D1Factory* factory = GetD2DFactory()) {
        D2D1_RENDER_TARGET_PROPERTIES props = {};
        props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        props.pixelFormat = {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED};
        factory->CreateDCRenderTarget(&props, &c.rt);
        if (c.rt) {
            c.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        }
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = CardWndProc;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kNotifyClassName;
    RegisterClassExW(&wc);
    if (c.dw && c.rt) {
        g_cardWnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            kNotifyClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, nullptr, nullptr);
    }
    if (g_cardWnd) {
        CardApplyGlass(g_cardWnd);
    } else {
        Wh_Log(L"[aot-plus] the status card could not be created");
    }
    SetEvent(g_cardReady);

    if (g_cardWnd) {
        MSG msg;
        bool running = true;
        while (running) {
            bool animating = c.phase == CardState::kIn || c.phase == CardState::kOut;
            if (!animating) {
                // Sleep until a message, or until the hold time runs out.
                DWORD wait = INFINITE;
                if (c.phase == CardState::kHold) {
                    double left = c.holdUntil - NowSeconds();
                    wait = (DWORD)std::max(0.0, std::min(left * 1000.0, 100.0));
                }
                MsgWaitForMultipleObjects(0, nullptr, FALSE, wait, QS_ALLINPUT);
            }
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (!running) {
                break;
            }
            if (CardTick()) {
                // One frame per composition pass.
                if (FAILED(DwmFlush())) {
                    Sleep(8);
                }
            }
        }
    }

    g_cardWnd = nullptr;
    CardFreeSurface();
    if (c.rt) {
        c.rt->Release();
        c.rt = nullptr;
    }
    if (c.dw) {
        c.dw->Release();
        c.dw = nullptr;
    }
    UnregisterClassW(kNotifyClassName, nullptr);
    return 0;
}

static void StartCardThread() {
    g_cardReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cardThread = CreateThread(nullptr, 0, CardThread, nullptr, 0, nullptr);
    if (g_cardThread && g_cardReady) {
        WaitForSingleObject(g_cardReady, 5000);
    }
}

static void StopCardThread() {
    if (g_cardWnd) {
        PostMessageW(g_cardWnd, WM_CARD_QUIT, 0, 0);
    }
    if (g_cardThread) {
        if (WaitForSingleObject(g_cardThread, 3000) == WAIT_TIMEOUT) {
            Wh_Log(L"[aot-plus] the status card thread did not stop in time");
        }
        CloseHandle(g_cardThread);
        g_cardThread = nullptr;
    }
    if (g_cardReady) {
        CloseHandle(g_cardReady);
        g_cardReady = nullptr;
    }
}

static void ShowNotification(PCWSTR text, CardIcon icon = kCardIconNone) {
    Settings s = GetSettingsCopy();
    if (!s.notificationsEnabled || !text || !g_cardWnd) {
        return;
    }
    CardRequest* req = new CardRequest{text, icon, s.notificationDuration};
    if (!PostMessageW(g_cardWnd, WM_CARD_SHOW, 0, (LPARAM)req)) {
        delete req;
    }
}

// ----- Border window (D2D ring on a color-keyed layered window) -----

static bool EnsureRenderTarget(BorderEntry* entry, int width, int height) {
    if (entry->rt && entry->rtSize.width == (UINT32)width &&
        entry->rtSize.height == (UINT32)height) {
        return true;
    }
    ID2D1Factory* factory = GetD2DFactory();
    if (!factory) {
        return false;
    }
    if (entry->rt) {
        D2D1_SIZE_U size = {(UINT32)width, (UINT32)height};
        if (SUCCEEDED(entry->rt->Resize(size))) {
            entry->rtSize = size;
            return true;
        }
        entry->brush->Release();
        entry->brush = nullptr;
        entry->rt->Release();
        entry->rt = nullptr;
        entry->rtSize = {};
    }
    D2D1_PIXEL_FORMAT pixFmt;
    pixFmt.format = DXGI_FORMAT_UNKNOWN;
    pixFmt.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    D2D1_RENDER_TARGET_PROPERTIES props;
    props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat = pixFmt;
    props.dpiX = 96.f;
    props.dpiY = 96.f;
    props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps;
    hwndProps.hwnd = entry->border;
    hwndProps.pixelSize = {(UINT32)width, (UINT32)height};
    hwndProps.presentOptions = D2D1_PRESENT_OPTIONS_NONE;
    if (FAILED(factory->CreateHwndRenderTarget(props, hwndProps, &entry->rt))) {
        return false;
    }
    entry->rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    float alpha = GetSettingsCopy().frameOpacity / 100.0f;
    D2D1_COLOR_F color = {0, 0, 0, alpha};
    if (FAILED(entry->rt->CreateSolidColorBrush(color, &entry->brush))) {
        entry->rt->Release();
        entry->rt = nullptr;
        return false;
    }
    entry->rtSize = {(UINT32)width, (UINT32)height};
    return true;
}

// Helpers so the geometry creation reads cleanly.
static HRESULT factory_create_rounded(ID2D1Factory* f, const D2D1_ROUNDED_RECT& rr,
                                      ID2D1RoundedRectangleGeometry** out) {
    return f->CreateRoundedRectangleGeometry(rr, out);
}
static HRESULT factory_create_rect(ID2D1Factory* f, const D2D1_RECT_F& rect,
                                   ID2D1RectangleGeometry** out) {
    return f->CreateRectangleGeometry(rect, out);
}

static D2D1_COLOR_F BorderColorF(COLORREF color, float opacity) {
    return {GetRValue(color) / 255.f, GetGValue(color) / 255.f, GetBValue(color) / 255.f,
            opacity};
}

static void RenderBorder(BorderEntry* entry, COLORREF color, COLORREF colorTo, bool gradient,
                         float opacity, int thickness, float radius, float tuck) {
    RECT client;
    GetClientRect(entry->border, &client);
    int w = client.right - client.left;
    int h = client.bottom - client.top;
    if (w <= 0 || h <= 0) {
        return;
    }
    if (!EnsureRenderTarget(entry, w, h)) {
        return;
    }

    entry->brush->SetColor(BorderColorF(color, opacity));

    // The border window is the window's frame grown by the thickness. The ring
    // is concentric with the window's own corners: its outer edge is the frame
    // grown by the thickness (radius + thickness), its inner edge reaches tuck
    // pixels over the window's edge (radius - tuck). The ring sits above the
    // window, so that overlap covers any thin border another mod draws there.
    float tk = (float)thickness;
    float l = tk;
    float t = tk;
    float r = w - tk;
    float b = h - tk;

    ID2D1Geometry* geoms[2] = {};
    UINT32 geomCount = 0;
    if (radius > 0.0f) {
        D2D1_ROUNDED_RECT outerRR = {{0, 0, (float)w, (float)h}, radius + tk, radius + tk};
        float innerRadius = radius - tuck > 0 ? radius - tuck : 0.0f;
        D2D1_ROUNDED_RECT innerRR = {{l + tuck, t + tuck, r - tuck, b - tuck}, innerRadius,
                                     innerRadius};
        ID2D1RoundedRectangleGeometry* outer = nullptr;
        ID2D1RoundedRectangleGeometry* innerGeo = nullptr;
        if (FAILED(factory_create_rounded(GetD2DFactory(), outerRR, &outer))) {
            return;
        }
        geoms[geomCount++] = outer;
        if (innerRR.rect.left < innerRR.rect.right &&
            innerRR.rect.top < innerRR.rect.bottom) {
            if (SUCCEEDED(factory_create_rounded(GetD2DFactory(), innerRR, &innerGeo))) {
                geoms[geomCount++] = innerGeo;
            }
        }
    } else {
        D2D1_RECT_F outer = {0, 0, (float)w, (float)h};
        D2D1_RECT_F innerRect = {l + tuck, t + tuck, r - tuck, b - tuck};
        ID2D1RectangleGeometry* outerG = nullptr;
        ID2D1RectangleGeometry* innerG = nullptr;
        if (FAILED(factory_create_rect(GetD2DFactory(), outer, &outerG))) {
            return;
        }
        geoms[geomCount++] = outerG;
        if (innerRect.left < innerRect.right && innerRect.top < innerRect.bottom) {
            if (SUCCEEDED(factory_create_rect(GetD2DFactory(), innerRect, &innerG))) {
                geoms[geomCount++] = innerG;
            }
        }
    }
    if (geomCount == 0) {
        return;
    }

    ID2D1GeometryGroup* group = nullptr;
    if (FAILED(GetD2DFactory()->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, geoms,
                                                    geomCount, &group))) {
        for (UINT32 i = 0; i < geomCount; i++) {
            geoms[i]->Release();
        }
        return;
    }

    entry->rt->BeginDraw();
    entry->rt->Clear(D2D1_COLOR_F{0.f, 0.f, 0.f, 0.f});
    ID2D1LinearGradientBrush* gradientBrush = nullptr;
    if (gradient) {
        D2D1_GRADIENT_STOP stops[2] = {{0.0f, BorderColorF(color, opacity)},
                                       {1.0f, BorderColorF(colorTo, opacity)}};
        ID2D1GradientStopCollection* stopList = nullptr;
        if (SUCCEEDED(entry->rt->CreateGradientStopCollection(stops, 2, &stopList))) {
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp = {{0.0f, 0.0f}, {(float)w, (float)h}};
            entry->rt->CreateLinearGradientBrush(lp, stopList, &gradientBrush);
            stopList->Release();
        }
    }
    if (gradientBrush) {
        entry->rt->FillGeometry(group, gradientBrush);
    } else {
        entry->rt->FillGeometry(group, entry->brush);
    }
    HRESULT hr = entry->rt->EndDraw(nullptr, nullptr);
    if (hr == (HRESULT)D2DERR_RECREATE_TARGET) {
        entry->brush->Release();
        entry->brush = nullptr;
        entry->rt->Release();
        entry->rt = nullptr;
        entry->rtSize = {};
    }

    if (gradientBrush) {
        gradientBrush->Release();
    }
    group->Release();
    for (UINT32 i = 0; i < geomCount; i++) {
        geoms[i]->Release();
    }
}

static bool IsClassName(HWND wnd, const wchar_t* name) {
    wchar_t cls[64];
    return wnd && GetClassNameW(wnd, cls, 64) && wcscmp(cls, name) == 0;
}

// The ring goes above its window and above the thin ring Seamless Borders
// keeps directly over it, so the thick ring covers that line. Returns the
// highest of those Seamless strips, or the window itself when it has none.
static HWND RingAnchor(HWND target, HWND border) {
    HWND top = target;
    HWND wnd = GetWindow(target, GW_HWNDPREV);
    for (int i = 0; wnd && i < 8; i++) {
        if (wnd != border && !IsClassName(wnd, L"SeamlessBordersPart")) {
            break;
        }
        if (wnd != border) {
            top = wnd;
        }
        wnd = GetWindow(wnd, GW_HWNDPREV);
    }
    return top;
}

// Keeps the ring on its window: hidden while the window is minimized, hidden
// or on another desktop; moved only when the frame or z-order changed; redrawn
// only when its size, color, thickness or corners changed.
static void UpdateBorder(BorderEntry* entry, HWND target) {
    if (!entry->border || !IsWindow(entry->border)) {
        return;
    }
    RECT frame;
    bool show = IsWindow(target) && IsWindowVisible(target) && !IsIconic(target) &&
                !IsWindowCloaked(target) && GetTrackingFrameRect(target, &frame);
    if (!show) {
        if (entry->shown) {
            ShowWindow(entry->border, SW_HIDE);
            entry->shown = false;
        }
        return;
    }
    // The window came to this desktop but the ring stayed on the old one.
    if (IsWindowCloaked(entry->border)) {
        PostMessageW(g_hostWnd, WM_APP_REBORDER, (WPARAM)target, 0);
        return;
    }

    int w = frame.right - frame.left;
    int h = frame.bottom - frame.top;
    bool moved = !EqualRect(&frame, &entry->lastFrame);
    HWND anchor = RingAnchor(target, entry->border);
    HWND above = GetWindow(anchor, GW_HWNDPREV);
    bool stacked = above == entry->border;
    if (moved || !stacked || !entry->shown) {
        UINT flags = SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOOWNERZORDER;
        if (!entry->shown) {
            flags |= SWP_SHOWWINDOW;
        }
        HWND insertAfter = above ? above : HWND_TOPMOST;
        if (stacked) {
            flags |= SWP_NOZORDER;
        }
        // Just above its window: click-through, and it only paints the ring.
        SetWindowPos(entry->border, insertAfter, frame.left, frame.top, w, h, flags);
        entry->lastFrame = frame;
        entry->shown = true;
    }

    Settings s = GetSettingsCopy();
    float scale = ScalingFactor(target);
    COLORREF color = s.frameUseAccent ? GetAccentColor() : s.frameColor;
    float opacity = s.frameOpacity / 100.0f;
    int thickness = (int)(s.frameThickness * scale);
    float radius = s.frameRoundCorners ? WindowCornersRadius(target) * scale : 0.0f;
    // Only when Seamless Borders strips sit over the window: reach one scaled
    // pixel past the edge line they draw (it overlaps the window by 1 px at
    // 100%), plus one for its soft edge. Otherwise the ring ends at the edge.
    float tuck = anchor != target ? std::max(1.0f, roundf(scale)) + 1.0f : 0.0f;
    bool gradient = s.frameGradient;
    COLORREF colorTo = s.frameColorTo;
    if (entry->drawn && entry->lastW == w && entry->lastH == h &&
        entry->lastColor == color && entry->lastColorTo == colorTo &&
        entry->lastGradient == gradient && entry->lastOpacity == opacity &&
        entry->lastThickness == thickness && entry->lastRadius == radius &&
        entry->lastTuck == tuck) {
        return;
    }
    RenderBorder(entry, color, colorTo, gradient, opacity, thickness, radius, tuck);
    entry->drawn = entry->rt != nullptr;
    entry->lastW = w;
    entry->lastH = h;
    entry->lastColor = color;
    entry->lastColorTo = colorTo;
    entry->lastGradient = gradient;
    entry->lastOpacity = opacity;
    entry->lastThickness = thickness;
    entry->lastRadius = radius;
    entry->lastTuck = tuck;
}

static LRESULT CALLBACK BorderWndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (wp == kBorderTimerId) {
                BorderEntry* entry = (BorderEntry*)GetWindowLongPtrW(wnd, GWLP_USERDATA);
                if (entry && entry->target) {
                    UpdateBorder(entry, entry->target);
                }
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(wnd, &ps);
            EndPaint(wnd, &ps);
            BorderEntry* entry = (BorderEntry*)GetWindowLongPtrW(wnd, GWLP_USERDATA);
            if (entry && entry->target) {
                entry->drawn = false;
                UpdateBorder(entry, entry->target);
            }
            return 0;
        }
        case WM_NCDESTROY: {
            KillTimer(wnd, kBorderTimerId);
            BorderEntry* entry = (BorderEntry*)GetWindowLongPtrW(wnd, GWLP_USERDATA);
            if (entry) {
                if (entry->brush) {
                    entry->brush->Release();
                    entry->brush = nullptr;
                }
                if (entry->rt) {
                    entry->rt->Release();
                    entry->rt = nullptr;
                }
                SetWindowLongPtrW(wnd, GWLP_USERDATA, 0);
            }
            break;
        }
        case WM_ERASEBKGND:
            return TRUE;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
    }
    return DefWindowProcW(wnd, msg, wp, lp);
}

static bool AssignBorder(HWND target) {
    Settings s = GetSettingsCopy();
    // std::map nodes are stable, so the entry keeps its address for as long
    // as the border window lives; its map slot is erased only after
    // DestroyWindow, so the window proc never sees a dangling pointer.
    BorderEntry& stored = g_borders[target];
    if (stored.border && IsWindow(stored.border)) {
        return true; // already has a border
    }
    stored.target = target;
    if (!s.frameEnabled || !IsWindowOnCurrentDesktop(target)) {
        return true;
    }

    RECT frame;
    if (!GetTrackingFrameRect(target, &frame)) {
        return false;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = BorderWndProc;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kBorderClassName;
    RegisterClassExW(&wc);

    HWND border = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kBorderClassName, L"",
        WS_POPUP | WS_DISABLED, frame.left, frame.top, frame.right - frame.left,
        frame.bottom - frame.top, nullptr, nullptr, nullptr, nullptr);
    if (!border) {
        Wh_Log(L"[aot-plus] failed to create the border window");
        return false;
    }

    stored.border = border;
    SetWindowLongPtrW(border, GWLP_USERDATA, (LONG_PTR)&stored);

    // PowerToys transparency recipe: black color key plus a far-off blur region.
    int pos = -GetSystemMetrics(SM_CXVIRTUALSCREEN) - 8;
    HRGN hrgn = CreateRectRgn(pos, 0, pos + 1, 1);
    if (hrgn) {
        DWM_BLURBEHIND bb = {DWM_BB_ENABLE | DWM_BB_BLURREGION, TRUE, hrgn, FALSE};
        DwmEnableBlurBehindWindow(border, &bb);
        DeleteObject(hrgn);
    }
    SetLayeredWindowAttributes(border, RGB(0, 0, 0), 0, LWA_COLORKEY);
    SetLayeredWindowAttributes(border, 0, 255, LWA_ALPHA);

    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(border, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                          sizeof(excludedFromPeek));

    // The ring follows moves through window events; this slow tick only
    // catches z-order, accent color and desktop changes.
    SetTimer(border, kBorderTimerId, 200, nullptr);
    UpdateBorder(&stored, target);
    return true;
}

// Destroys only the ring; the window stays tracked as pinned.
static void DestroyBorderWindow(BorderEntry& entry) {
    if (entry.border && IsWindow(entry.border)) {
        KillTimer(entry.border, kBorderTimerId);
        DestroyWindow(entry.border);
    }
    HWND target = entry.target;
    entry = BorderEntry{};
    entry.target = target;
}

// Stops tracking the window and destroys its ring.
static void DestroyBorderFor(HWND target) {
    auto it = g_borders.find(target);
    if (it == g_borders.end()) {
        return;
    }
    DestroyBorderWindow(it->second);
    g_borders.erase(it);
}

// ----- Pin state -----

static bool PinWindow(HWND wnd) {
    SetPropW(wnd, kPinProp, (HANDLE)1);
    if (!SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, ForeignPosFlags(wnd))) {
        DWORD err = GetLastError();
        RemovePropW(wnd, kPinProp);
        SetLastError(err);
        return false;
    }
    return true;
}

static bool UnpinWindow(HWND wnd) {
    RemovePropW(wnd, kPinProp);
    return SetWindowPos(wnd, HWND_NOTOPMOST, 0, 0, 0, 0, ForeignPosFlags(wnd)) != FALSE;
}

// Per-window transparency, ported from PowerToys.

static void ApplyWindowAlpha(HWND wnd, int percent) {
    // Style changes wait on the app, so a frozen one is left alone.
    if (!wnd || !IsWindow(wnd) || IsHungAppWindow(wnd)) {
        return;
    }
    percent = std::max(kMinOpacityPercent, std::min(kMaxOpacityPercent, percent));

    LONG exStyle = GetWindowLongW(wnd, GWL_EXSTYLE);
    bool layered = (exStyle & WS_EX_LAYERED) != 0;

    if (g_savedLayered.find(wnd) == g_savedLayered.end()) {
        SavedLayered state;
        state.hadLayered = layered;
        if (layered) {
            BYTE alpha = 255;
            COLORREF key = 0;
            DWORD flags = 0;
            if (!GetLayeredWindowAttributes(wnd, &key, &alpha, &flags)) {
                return;
            }
            state.alpha = alpha;
            state.usedColorKey = (flags & LWA_COLORKEY) != 0;
            state.colorKey = key;
        }
        state.valid = true;
        g_savedLayered[wnd] = state;
    }

    if (layered) {
        SetWindowLongW(wnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        SetWindowPos(wnd, nullptr, 0, 0, 0, 0,
                     ForeignPosFlags(wnd) | SWP_NOZORDER | SWP_FRAMECHANGED);
        exStyle = GetWindowLongW(wnd, GWL_EXSTYLE);
    }
    BYTE alphaValue = (BYTE)((255 * percent + 50) / 100);
    SetWindowLongW(wnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(wnd, 0, alphaValue, LWA_ALPHA);
}

static void RestoreWindowAlpha(HWND wnd) {
    if (!wnd || !IsWindow(wnd)) {
        g_savedLayered.erase(wnd);
        return;
    }
    if (IsHungAppWindow(wnd)) {
        return;
    }
    auto it = g_savedLayered.find(wnd);
    if (it == g_savedLayered.end()) {
        return;
    }
    const SavedLayered& state = it->second;
    LONG exStyle = GetWindowLongW(wnd, GWL_EXSTYLE);
    if (state.hadLayered) {
        if (exStyle & WS_EX_LAYERED) {
            SetWindowLongW(wnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
            exStyle = GetWindowLongW(wnd, GWL_EXSTYLE);
        }
        SetWindowLongW(wnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        DWORD flags = LWA_ALPHA;
        if (state.usedColorKey) {
            flags |= LWA_COLORKEY;
        }
        SetLayeredWindowAttributes(wnd, state.colorKey, state.alpha, flags);
        SetWindowPos(wnd, nullptr, 0, 0, 0, 0,
                     ForeignPosFlags(wnd) | SWP_NOZORDER | SWP_FRAMECHANGED);
    } else if (exStyle & WS_EX_LAYERED) {
        SetLayeredWindowAttributes(wnd, 0, 255, LWA_ALPHA);
        SetWindowLongW(wnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        SetWindowPos(wnd, nullptr, 0, 0, 0, 0,
                     ForeignPosFlags(wnd) | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    g_savedLayered.erase(it);
}

static void StepWindowTransparency(HWND wnd, int delta) {
    if (!wnd || !IsWindow(wnd) || !GetPropW(wnd, kPinProp)) {
        return;
    }
    int current = kMaxOpacityPercent;
    LONG exStyle = GetWindowLongW(wnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        BYTE alpha = 255;
        if (GetLayeredWindowAttributes(wnd, nullptr, &alpha, nullptr)) {
            current = (alpha * 100 + 127) / 255;
        }
    }
    int next = std::max(kMinOpacityPercent, std::min(kMaxOpacityPercent, current + delta));
    if (next != current) {
        ApplyWindowAlpha(wnd, next);
        PlaySoundFor(delta > 0 ? kSoundOpacityUp : kSoundOpacityDown);
        Wh_Log(L"[aot-plus] opacity %d", next);
    }
}

// ----- System menu entry -----

static bool HasOurMenuCommand(HMENU menu) {
    if (!menu) {
        return false;
    }
    if (GetMenuState(menu, kSysMenuCommandId, MF_BYCOMMAND) == (UINT)-1) {
        return false;
    }
    MENUITEMINFOW info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_DATA;
    return GetMenuItemInfoW(menu, kSysMenuCommandId, FALSE, &info) &&
           info.dwItemData == kSysMenuOwnerTag;
}

static void UpdateSystemMenuItem(HWND wnd) {
    if (!wnd || !IsWindow(wnd)) {
        return;
    }
    Settings s = GetSettingsCopy();
    if (s.showInSystemMenu) {
        g_menuEverEnabled = true;
    } else if (!g_menuEverEnabled) {
        // The menu entry was never added, so there is nothing to tidy.
        return;
    }
    HMENU menu = GetSystemMenu(wnd, FALSE);
    if (!menu) {
        return;
    }
    if (!s.showInSystemMenu) {
        if (HasOurMenuCommand(menu)) {
            RemoveMenu(menu, kSysMenuCommandId, MF_BYCOMMAND);
        }
        g_menuWindows.erase(wnd);
        return;
    }

    MENUITEMINFOW info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_DATA;
    info.wID = kSysMenuCommandId;
    info.fState = GetPropW(wnd, kPinProp) ? MFS_CHECKED : MFS_UNCHECKED;
    info.dwTypeData = (LPWSTR)L"Always on top";
    info.dwItemData = kSysMenuOwnerTag;

    if (!HasOurMenuCommand(menu)) {
        if (InsertMenuItemW(menu, SC_CLOSE, FALSE, &info)) {
            g_menuWindows.insert(wnd);
        }
    } else {
        info.fMask = MIIM_STATE | MIIM_STRING;
        SetMenuItemInfoW(menu, kSysMenuCommandId, FALSE, &info);
        g_menuWindows.insert(wnd);
    }
}

// Takes our entry out of every system menu we added it to. Used when the
// setting is turned off and at teardown.
static void RemoveSystemMenuItems() {
    for (HWND wnd : g_menuWindows) {
        if (!IsWindow(wnd)) {
            continue;
        }
        HMENU menu = GetSystemMenu(wnd, FALSE);
        if (HasOurMenuCommand(menu)) {
            RemoveMenu(menu, kSysMenuCommandId, MF_BYCOMMAND);
        }
    }
    g_menuWindows.clear();
}

// Forgets closed windows so the set does not grow.
static void PruneMenuWindows() {
    for (auto it = g_menuWindows.begin(); it != g_menuWindows.end();) {
        it = IsWindow(*it) ? std::next(it) : g_menuWindows.erase(it);
    }
}

// ----- Toggle -----

static bool HasPendingCheck(HWND wnd) {
    for (const PendingCheck& c : g_pendingChecks) {
        if (c.wnd == wnd) {
            return true;
        }
    }
    return false;
}

static void DropPendingCheck(HWND wnd) {
    g_pendingChecks.erase(std::remove_if(g_pendingChecks.begin(), g_pendingChecks.end(),
                                         [wnd](const PendingCheck& c) { return c.wnd == wnd; }),
                          g_pendingChecks.end());
}

static void QueueCheck(HWND wnd, bool wantTopmost) {
    DropPendingCheck(wnd);
    g_pendingChecks.push_back({wnd, wantTopmost, 0, GetTickCount64() + 400});
    SetTimer(g_hostWnd, kVerifyTimerId, 100, nullptr);
}

// Stops tracking a window: ring gone, transparency undone, and topmost
// cleared too when asked.
static void ForgetWindow(HWND wnd, bool unpin) {
    if (unpin && IsWindow(wnd)) {
        UnpinWindow(wnd);
    }
    RestoreWindowAlpha(wnd);
    DestroyBorderFor(wnd);
    DropPendingCheck(wnd);
}

// Checks each pin or unpin a moment after it was sent. A change that did not
// land is sent twice more; then the card says what went wrong.
static void RunPendingChecks() {
    ULONGLONG now = GetTickCount64();
    std::vector<PendingCheck> keep;
    std::vector<PendingCheck> failed;
    for (PendingCheck c : g_pendingChecks) {
        if (!IsWindow(c.wnd) || IsTopmost(c.wnd) == c.wantTopmost) {
            continue;
        }
        if (now < c.due) {
            keep.push_back(c);
            continue;
        }
        if (c.tries < 2 && !IsHungAppWindow(c.wnd)) {
            SetWindowPos(c.wnd, c.wantTopmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                         ForeignPosFlags(c.wnd));
            c.tries++;
            c.due = now + 500;
            keep.push_back(c);
            continue;
        }
        failed.push_back(c);
    }
    g_pendingChecks = keep;
    if (g_pendingChecks.empty()) {
        KillTimer(g_hostWnd, kVerifyTimerId);
    }

    for (const PendingCheck& c : failed) {
        if (IsHungAppWindow(c.wnd)) {
            // The change is queued in the app; it lands once the app wakes up.
            ShowNotification(L"This window is not responding yet");
            Wh_Log(L"[aot-plus] %p is not responding; the change is queued", c.wnd);
            continue;
        }
        if (c.wantTopmost) {
            ForgetWindow(c.wnd, false);
            RemovePropW(c.wnd, kPinProp);
            ShowNotification(L"This window would not stay on top");
            Wh_Log(L"[aot-plus] pin of %p did not hold", c.wnd);
        } else {
            ShowNotification(L"This window keeps itself on top");
            Wh_Log(L"[aot-plus] unpin of %p did not hold", c.wnd);
        }
    }
}

// Once a second: forget windows that closed or hid, and put back topmost on
// pinned windows that lost it (some apps drop it after a minimize).
static void MaintainPinned() {
    std::vector<HWND> gone;
    std::vector<HWND> lost;
    for (auto& [wnd, entry] : g_borders) {
        if (!IsWindow(wnd) || (!IsWindowVisible(wnd) && !IsIconic(wnd))) {
            gone.push_back(wnd);
        } else if (!IsIconic(wnd) && !IsTopmost(wnd)) {
            lost.push_back(wnd);
        }
    }
    for (HWND wnd : gone) {
        Wh_Log(L"[aot-plus] %p closed or hid, no longer pinned", wnd);
        ForgetWindow(wnd, true);
    }
    for (HWND wnd : lost) {
        if (!HasPendingCheck(wnd) && !IsHungAppWindow(wnd)) {
            PinWindow(wnd);
        }
    }
    PruneMenuWindows();
    UpdateBorderHooks(); // drops the hooks once nothing is pinned
}

// The window the shortcut acts on: the top-level window of whatever has
// focus, never a shell surface or one of this mod's own windows.
static HWND ResolveCommandTarget(HWND wnd) {
    if (!wnd || !IsWindow(wnd)) {
        return nullptr;
    }
    if (HWND root = GetAncestor(wnd, GA_ROOT)) {
        wnd = root;
    }
    if (!IsWindowVisible(wnd) || IsOwnWindow(wnd) || IsSystemSurface(wnd)) {
        return nullptr;
    }
    return wnd;
}

static void ProcessCommandImpl(HWND wnd) {
    wnd = ResolveCommandTarget(wnd);
    if (!wnd) {
        Wh_Log(L"[aot-plus] no window to pin");
        return;
    }
    Settings s = GetSettingsCopy();
    if (s.gameModeBlock && GameModeActive()) {
        Wh_Log(L"[aot-plus] game mode is on, skipped");
        return;
    }
    // Windows blocks a normal app from moving an administrator's window.
    if (!IsCurrentProcessElevated() && IsWindowProcessElevated(wnd)) {
        ShowNotification(L"Cannot pin apps running as administrator");
        Wh_Log(L"[aot-plus] %p runs as administrator, skipped", wnd);
        return;
    }

    bool tracked = g_borders.find(wnd) != g_borders.end();
    bool pinned = tracked || GetPropW(wnd, kPinProp) || IsTopmost(wnd);
    if (pinned) {
        ForgetWindow(wnd, false);
        if (!UnpinWindow(wnd)) {
            DWORD err = GetLastError();
            ShowNotification(L"Could not unpin this window");
            Wh_Log(L"[aot-plus] failed to unpin %p: %lu", wnd, err);
            return;
        }
        QueueCheck(wnd, false);
        PlaySoundFor(kSoundUnpin);
        ShowNotification(L"Unpinned", kCardIconUnpin);
        Wh_Log(L"[aot-plus] unpinned %p", wnd);
    } else {
        if (IsExcludedWindow(wnd)) {
            Wh_Log(L"[aot-plus] window is excluded, skipped");
            return;
        }
        if (!PinWindow(wnd)) {
            DWORD err = GetLastError();
            ShowNotification(L"Could not pin this window");
            Wh_Log(L"[aot-plus] failed to pin %p: %lu", wnd, err);
            return;
        }
        AssignBorder(wnd);
        QueueCheck(wnd, true);
        PlaySoundFor(kSoundPin);
        ShowNotification(L"Pinned", kCardIconPin);
        Wh_Log(L"[aot-plus] pinned %p", wnd);
    }
    UpdateSystemMenuItem(wnd);
}

static void ProcessCommand(HWND wnd) {
    ProcessCommandImpl(wnd);
    UpdateBorderHooks(); // a window was pinned or unpinned
}

// ----- Win events -----

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND wnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    switch (event) {
        case EVENT_SYSTEM_MENUPOPUPSTART: {
            if (idObject == OBJID_SYSMENU && wnd) {
                Settings s = GetSettingsCopy();
                g_lastSysMenuWnd = s.showInSystemMenu ? wnd : nullptr;
                UpdateSystemMenuItem(wnd);
            }
            return;
        }
        case EVENT_SYSTEM_MENUPOPUPEND: {
            if (idObject == OBJID_SYSMENU && wnd == g_lastSysMenuWnd) {
                g_lastSysMenuWnd = nullptr;
            }
            return;
        }
        case EVENT_OBJECT_INVOKED: {
            Settings s = GetSettingsCopy();
            if (!s.showInSystemMenu) {
                return;
            }
            if (idChild != (LONG)kSysMenuCommandId) {
                return;
            }
            bool related = idObject == OBJID_SYSMENU || idObject == OBJID_MENU ||
                           idObject == OBJID_CLIENT;
            if (!related && (!g_lastSysMenuWnd || !IsWindow(g_lastSysMenuWnd))) {
                return;
            }
            HWND command = nullptr;
            HWND candidates[3] = {g_lastSysMenuWnd, wnd, GetForegroundWindow()};
            for (HWND cand : candidates) {
                if (cand && IsWindow(cand) && !command) {
                    HMENU menu = GetSystemMenu(cand, FALSE);
                    if (HasOurMenuCommand(menu)) {
                        command = cand;
                    }
                }
            }
            if (command) {
                ProcessCommand(command);
            }
            return;
        }
        default:
            break;
    }

    if (!wnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        // Caret, scroll bar and other child-object events: not a window move.
        return;
    }
    auto it = g_borders.find(wnd);
    if (it == g_borders.end()) {
        if (event == EVENT_SYSTEM_FOREGROUND) {
            UpdateSystemMenuItem(wnd);
        }
        return;
    }

    switch (event) {
        case EVENT_OBJECT_LOCATIONCHANGE:
        case EVENT_SYSTEM_MOVESIZEEND:
        case EVENT_SYSTEM_MINIMIZESTART:
        case EVENT_OBJECT_CLOAKED_AOT:
        case EVENT_OBJECT_UNCLOAKED_AOT:
            UpdateBorder(&it->second, wnd);
            break;
        case EVENT_SYSTEM_MINIMIZEEND:
            // Some apps drop topmost across a minimize.
            if (!IsTopmost(wnd)) {
                PinWindow(wnd);
            }
            UpdateBorder(&it->second, wnd);
            break;
        case EVENT_SYSTEM_FOREGROUND:
            UpdateSystemMenuItem(wnd);
            UpdateBorder(&it->second, wnd);
            break;
        case EVENT_OBJECT_DESTROY:
            Wh_Log(L"[aot-plus] %p closed", wnd);
            ForgetWindow(wnd, false);
            break;
        default:
            break;
    }
}

// These events fire for every window on the desktop (location changes include
// every cursor and caret move), so the hooks exist only while at least one
// window is pinned. Main thread only; never called from inside WinEventProc.
static void UpdateBorderHooks() {
    if (g_borders.empty()) {
        for (HWINEVENTHOOK hook : g_eventHooks) {
            UnhookWinEvent(hook);
        }
        g_eventHooks.clear();
        return;
    }
    if (!g_eventHooks.empty()) {
        return;
    }
    const DWORD events[] = {
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_SYSTEM_MINIMIZESTART,
        EVENT_SYSTEM_MINIMIZEEND,    EVENT_SYSTEM_MOVESIZEEND,
        EVENT_OBJECT_DESTROY,        EVENT_OBJECT_CLOAKED_AOT,
        EVENT_OBJECT_UNCLOAKED_AOT,
    };
    for (DWORD ev : events) {
        HWINEVENTHOOK hook = SetWinEventHook(ev, ev, nullptr, WinEventProc, 0, 0,
                                             WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (hook) {
            g_eventHooks.push_back(hook);
        } else {
            Wh_Log(L"[aot-plus] failed to set the win event hook for %lu", ev);
        }
    }
}

static void UpdateMenuHooks(bool enable) {
    if (enable) {
        if (!g_menuHooks.empty()) {
            return;
        }
        // The foreground event is what puts the entry into each window's menu.
        const DWORD events[] = {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MENUPOPUPSTART,
                                EVENT_SYSTEM_MENUPOPUPEND, EVENT_OBJECT_INVOKED};
        for (DWORD ev : events) {
            HWINEVENTHOOK hook = SetWinEventHook(ev, ev, nullptr, WinEventProc, 0, 0,
                                                 WINEVENT_OUTOFCONTEXT |
                                                     WINEVENT_SKIPOWNPROCESS);
            if (hook) {
                g_menuHooks.push_back(hook);
            }
        }
    } else {
        for (HWINEVENTHOOK hook : g_menuHooks) {
            UnhookWinEvent(hook);
        }
        g_menuHooks.clear();
    }
}

// ----- Windows still pinned from before a restart -----

// A pinned window carries this mod's mark (a window property), so after a
// restart those windows get their ring back. Other apps' own topmost windows
// carry no mark and are left alone.
static BOOL CALLBACK AdoptEnumProc(HWND wnd, LPARAM param) {
    auto* list = (std::vector<HWND>*)param;
    if (GetPropW(wnd, kPinProp) && IsWindowVisible(wnd) && !IsOwnWindow(wnd)) {
        list->push_back(wnd);
    }
    return TRUE;
}

static void AdoptPinnedWindows() {
    std::vector<HWND> list;
    EnumWindows(AdoptEnumProc, (LPARAM)&list);
    for (HWND wnd : list) {
        if (g_borders.find(wnd) != g_borders.end()) {
            continue;
        }
        if (IsExcludedWindow(wnd)) {
            UnpinWindow(wnd);
            continue;
        }
        Wh_Log(L"[aot-plus] %p was pinned before, keeping it pinned", wnd);
        if (!IsTopmost(wnd)) {
            PinWindow(wnd);
        }
        AssignBorder(wnd);
    }
}

// ----- Hotkeys -----

// Windows keeps some shortcuts for itself (Win+Alt+T belongs to Game Bar), so
// RegisterHotKey fails for them. The default Win+Ctrl+T is free, so the hook
// only runs when the user picks a chord Windows holds. Those fall back to a low-level keyboard hook
// that catches the chord before the shell sees it.
struct FallbackHotkey {
    UINT id;
    UINT mods;
    UINT vk;
    bool down;
    DWORD lastDownTime;
};

// The keyboard hook lives on its own high-priority thread that does nothing
// else. Windows silently drops a low-level hook whose thread is slow to
// answer, so the hook never shares a thread with window work.
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_hookReady = nullptr;
static std::vector<FallbackHotkey> g_fallbackHotkeys; // hook thread only
static HHOOK g_keyboardHook = nullptr;                // hook thread only

static UINT CurrentModifiers() {
    UINT mods = 0;
    if ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) mods |= MOD_WIN;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
    if (GetAsyncKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
    return mods;
}

static LRESULT CALLBACK FallbackKeyboardProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && g_hostWnd) {
        const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
        bool isDown = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
        bool isUp = wp == WM_KEYUP || wp == WM_SYSKEYUP;
        if (k->vkCode != 0xE8) {
            for (FallbackHotkey& h : g_fallbackHotkeys) {
                if (k->vkCode != h.vk) {
                    continue;
                }
                if (isDown && CurrentModifiers() == h.mods) {
                    // Held keys repeat every few dozen ms. A long gap means the
                    // key-up was missed, so this is a fresh press, not a repeat.
                    bool repeat = h.down && (DWORD)(k->time - h.lastDownTime) < 1200;
                    h.lastDownTime = k->time;
                    if (!repeat) {
                        h.down = true;
                        if (h.mods & (MOD_WIN | MOD_ALT)) {
                            // The chord key is swallowed, so other hooks would see a
                            // bare Win or Alt tap (Start, Flow Launcher, menu bar).
                            // A dummy key while the modifier is still held stops that.
                            INPUT in[2] = {};
                            in[0].type = in[1].type = INPUT_KEYBOARD;
                            in[0].ki.wVk = in[1].ki.wVk = 0xE8;
                            in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(2, in, sizeof(INPUT));
                        }
                        PostMessageW(g_hostWnd, WM_HOTKEY, h.id, 0);
                    }
                    return 1;
                }
                if (isUp && h.down) {
                    h.down = false;
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

// Installs a fresh hook before removing the old one, so no key is missed when
// the key list changes.
static void ArmHook() {
    HHOOK old = g_keyboardHook;
    g_keyboardHook = nullptr;
    if (!g_fallbackHotkeys.empty()) {
        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, FallbackKeyboardProc,
                                           GetModuleHandleW(nullptr), 0);
        if (!g_keyboardHook) {
            Wh_Log(L"[aot-plus] keyboard hook failed: %lu", GetLastError());
        }
    }
    if (old) {
        UnhookWindowsHookEx(old);
    }
}

static DWORD WINAPI HookThread(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE); // makes the queue
    SetEvent(g_hookReady);
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_HOOK_SET) {
            auto* list = (std::vector<FallbackHotkey>*)msg.lParam;
            g_fallbackHotkeys = list ? *list : std::vector<FallbackHotkey>();
            delete list;
            ArmHook();
        } else {
            DispatchMessageW(&msg);
        }
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    g_fallbackHotkeys.clear();
    return 0;
}

static void StartHookThread() {
    g_hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_hookThreadId);
    if (g_hookThread && g_hookReady) {
        WaitForSingleObject(g_hookReady, 5000);
    } else {
        Wh_Log(L"[aot-plus] the keyboard hook thread could not start");
    }
}

static void StopHookThread() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_hookThread, 3000) == WAIT_TIMEOUT) {
            Wh_Log(L"[aot-plus] the keyboard hook thread did not stop in time");
        }
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }
    if (g_hookReady) {
        CloseHandle(g_hookReady);
        g_hookReady = nullptr;
    }
}

// Hands the hook thread its key list (an empty list removes the hook).
static void SendHookList(const std::vector<FallbackHotkey>& list) {
    if (!g_hookThreadId) {
        return;
    }
    auto* copy = new std::vector<FallbackHotkey>(list);
    if (!PostThreadMessageW(g_hookThreadId, WM_HOOK_SET, 0, (LPARAM)copy)) {
        delete copy;
    }
}

static void UnregisterOurHotkeys(HWND host) {
    UnregisterHotKey(host, kHotkeyIdPin);
    UnregisterHotKey(host, kHotkeyIdOpacityUp);
    UnregisterHotKey(host, kHotkeyIdOpacityDown);
    SendHookList({});
}

static void RegisterOneHotkey(HWND host, std::vector<FallbackHotkey>* fallback, UINT id,
                              UINT mods, UINT vk, const wchar_t* name) {
    if (!vk) {
        return;
    }
    if (RegisterHotKey(host, id, mods | MOD_NOREPEAT, vk)) {
        return;
    }
    Wh_Log(L"[aot-plus] %s shortcut is held by Windows or another app (mods=%u vk=%u), using the keyboard hook",
           name, mods, vk);
    fallback->push_back({id, mods, vk, false, 0});
}

static void RegisterOurHotkeys(HWND host) {
    UnregisterHotKey(host, kHotkeyIdPin);
    UnregisterHotKey(host, kHotkeyIdOpacityUp);
    UnregisterHotKey(host, kHotkeyIdOpacityDown);
    Settings s = GetSettingsCopy();
    std::vector<FallbackHotkey> fallback;
    RegisterOneHotkey(host, &fallback, kHotkeyIdPin, s.hotkeyMods, s.hotkeyVk, L"pin");
    RegisterOneHotkey(host, &fallback, kHotkeyIdOpacityUp, s.hotkeyMods, s.opacityUpVk,
                      L"more opaque");
    RegisterOneHotkey(host, &fallback, kHotkeyIdOpacityDown, s.hotkeyMods, s.opacityDownVk,
                      L"more transparent");
    SendHookList(fallback);
}

// ----- Host window and main thread -----

static LRESULT CALLBACK HostWndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_HOTKEY: {
            HWND fg = GetForegroundWindow();
            if (!fg) {
                return 0;
            }
            if (wp == kHotkeyIdPin) {
                ProcessCommand(fg);
            } else if (wp == kHotkeyIdOpacityUp) {
                StepWindowTransparency(fg, kOpacityStepPercent);
            } else if (wp == kHotkeyIdOpacityDown) {
                StepWindowTransparency(fg, -kOpacityStepPercent);
            }
            return 0;
        }
        case WM_TIMER:
            if (wp == kVerifyTimerId) {
                RunPendingChecks();
            } else if (wp == kMaintainTimerId) {
                MaintainPinned();
            }
            return 0;
        case WM_APP_REBORDER: {
            HWND target = (HWND)wp;
            auto it = g_borders.find(target);
            if (it != g_borders.end()) {
                DestroyBorderWindow(it->second);
                AssignBorder(target);
            }
            return 0;
        }
        case WM_APP_SETTINGS: {
            Settings s = GetSettingsCopy();
            RegisterOurHotkeys(wnd);
            // Redraw every ring so thickness, color and corners apply; windows
            // that became excluded are unpinned.
            std::vector<HWND> keys;
            for (auto& [trackedWnd, entry] : g_borders) {
                keys.push_back(trackedWnd);
            }
            for (HWND trackedWnd : keys) {
                if (IsWindow(trackedWnd) && IsExcludedWindow(trackedWnd)) {
                    ForgetWindow(trackedWnd, true);
                    Wh_Log(L"[aot-plus] unpinned excluded window %p", trackedWnd);
                    continue;
                }
                auto it = g_borders.find(trackedWnd);
                if (it == g_borders.end()) {
                    continue;
                }
                DestroyBorderWindow(it->second);
                if (s.frameEnabled && IsWindow(trackedWnd) && IsWindowVisible(trackedWnd)) {
                    AssignBorder(trackedWnd);
                }
            }
            UpdateBorderHooks();
            if (!s.showInSystemMenu) {
                RemoveSystemMenuItems();
            }
            UpdateMenuHooks(s.showInSystemMenu);
            g_lastSysMenuWnd = nullptr;
            UpdateSystemMenuItem(GetForegroundWindow());
            return 0;
        }
        case WM_APP_QUIT:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(wnd, msg, wp, lp);
}

// MainThread sets this once the host window exists, or on failure, so
// WhTool_ModInit never returns before g_hostWnd is known.
static HANDLE g_mainReady = nullptr;
static bool g_mainStartOk = false; // written before g_mainReady is set

static DWORD WINAPI MainThread(LPVOID) {
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com)) {
        Wh_Log(L"[aot-plus] CoInitializeEx failed; desktop checks pass everything");
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWndProc;
    wc.lpszClassName = kHostClassName;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"[aot-plus] failed to register the host window class");
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
        SetEvent(g_mainReady);
        return 0;
    }

    g_hostWnd = CreateWindowExW(0, kHostClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                nullptr, nullptr, nullptr);
    if (!g_hostWnd) {
        Wh_Log(L"[aot-plus] failed to create the host window");
        UnregisterClassW(kHostClassName, nullptr);
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
        SetEvent(g_mainReady);
        return 0;
    }

    g_mainStartOk = true;
    SetEvent(g_mainReady);

    LoadSettings();
    GetD2DFactory(); // created once here, before the card thread shares it
    StartCardThread();
    StartHookThread();
    RegisterOurHotkeys(g_hostWnd);
    {
        Settings s = GetSettingsCopy();
        UpdateMenuHooks(s.showInSystemMenu);
    }
    AdoptPinnedWindows();
    UpdateBorderHooks();
    SetTimer(g_hostWnd, kMaintainTimerId, 1000, nullptr);
    if (HWND fg = GetForegroundWindow()) {
        UpdateSystemMenuItem(fg);
    }
    Wh_Log(L"[aot-plus] running, waiting for the hotkey");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Teardown on this thread so every window dies here, not cross-thread.
    // Keys first, so no shortcut lands halfway through.
    UnregisterHotKey(g_hostWnd, kHotkeyIdPin);
    UnregisterHotKey(g_hostWnd, kHotkeyIdOpacityUp);
    UnregisterHotKey(g_hostWnd, kHotkeyIdOpacityDown);
    StopHookThread();
    KillTimer(g_hostWnd, kMaintainTimerId);
    KillTimer(g_hostWnd, kVerifyTimerId);
    g_pendingChecks.clear();

    for (HWINEVENTHOOK hook : g_menuHooks) {
        UnhookWinEvent(hook);
    }
    g_menuHooks.clear();
    for (HWINEVENTHOOK hook : g_eventHooks) {
        UnhookWinEvent(hook);
    }
    g_eventHooks.clear();
    RemoveSystemMenuItems();

    for (auto& [wnd, entry] : g_borders) {
        if (IsWindow(wnd)) {
            UnpinWindow(wnd);
        }
        RestoreWindowAlpha(wnd);
        DestroyBorderWindow(entry);
    }
    g_borders.clear();
    g_savedLayered.clear();

    StopCardThread();

    DestroyWindow(g_hostWnd);
    g_hostWnd = nullptr;
    UnregisterClassW(kHostClassName, nullptr);
    UnregisterClassW(kBorderClassName, nullptr);
    ReleaseVdm();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }

    Wh_Log(L"[aot-plus] main thread exiting");
    return 0;
}

// ----- Tool mod lifecycle -----

BOOL WhTool_ModInit() {
    Wh_Log(L"[aot-plus] initializing as a tool mod");
    InitializeCriticalSection(&g_cs);
    LoadSettings();

    g_mainReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_mainReady) {
        g_mainThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    if (!g_mainThread) {
        Wh_Log(L"[aot-plus] failed to start the main thread");
        if (g_mainReady) {
            CloseHandle(g_mainReady);
            g_mainReady = nullptr;
        }
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    // MainThread sets the event on every path, so this wait always ends.
    WaitForSingleObject(g_mainReady, INFINITE);
    CloseHandle(g_mainReady);
    g_mainReady = nullptr;
    if (!g_mainStartOk) {
        WaitForSingleObject(g_mainThread, INFINITE);
        CloseHandle(g_mainThread);
        g_mainThread = nullptr;
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"[aot-plus] settings changed");
    LoadSettings();
    if (g_hostWnd) {
        PostMessageW(g_hostWnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"[aot-plus] unloading");
    if (g_hostWnd) {
        PostMessageW(g_hostWnd, WM_APP_QUIT, 0, 0);
    }
    if (g_mainThread) {
        WaitForSingleObject(g_mainThread, INFINITE);
        CloseHandle(g_mainThread);
        g_mainThread = nullptr;
    }
    DeleteCriticalSection(&g_cs);
    Wh_Log(L"[aot-plus] unloaded");
}

// clang-format off
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
// clang-format on
