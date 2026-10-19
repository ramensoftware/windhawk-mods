// ==WindhawkMod==
// @id              macos-dock
// @name            macOS Dock
// @description     A macOS-style Dock: magnification on hover, bouncing icons, running app indicators, drag to rearrange and more
// @version         1.0.0
// @author          Rafael Mendes
// @github          https://github.com/RafaelMendes1407
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lversion -lshcore -ladvapi32 -ldwmapi -ld2d1 -ldwrite -lole32 -loleaut32 -luuid -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Dock

A Dock like the one in macOS, drawn by the mod itself (it's not a modified
taskbar). Behaviors taken from macOS:

- **Magnification**: icons near the mouse grow along a smooth curve, their
  neighbors make room and the Dock follows. The icon under the mouse stays
  under the cursor, like in macOS.
- **App name** in a tooltip above the icon on hover.
- **Indicator** (small dot) below running apps.
- **Launch bounce**: the icon bounces until the app's first window appears.
- **Attention bounce**: when an app flashes asking for attention, the icon
  bounces until you open the window.
- **Pinned and running apps**: pinned apps always stay in the Dock; running
  apps that aren't pinned appear after them, animating in and out.
- **Drag to rearrange**: drag an icon to change the order. Drag it out of the
  Dock and release to remove it (with the "poof" cloud). Dragging a running
  app into the Dock pins it.
- **Drop files**: drop files on an app to open them with it, on the Recycle
  Bin to delete them or on an empty area of the Dock to pin them.
- **Click**: opens the app, or brings its windows to the front.
  Middle click opens a new window.
- **Context menu** (right click): open windows, New window, Keep in Dock,
  Show in Explorer, Close.
- **Downloads** as a stack: one click shows the most recent files.
- **Recycle Bin**: full or empty icon, menu to empty it.
- **Smart hiding**: the Dock stays visible while no window covers it and only
  hides when an app (a maximized one, for example) is over it; moving the
  mouse to the screen edge brings it back. It can also always hide, and the
  Dock hides by itself for fullscreen apps.
- Dock at the bottom, left or right, with a blurred background.
- **Multiple monitors**: one Dock on each monitor, all with the same apps.
- **Minimize**: clicking the icon of the focused app minimizes it, and
  another click brings it back.

## How it works
The mod runs in `explorer.exe`. It can hide the Windows taskbar (the option
turns on the taskbar's auto-hide and keeps it from showing up); when the mod
is disabled, everything goes back to how it was.

Pinned apps are kept in the mod's storage. The first time, the mod imports
the shortcuts pinned to the taskbar.

## Tips
- Combine it with the **Global Menu Bar** mod to have the menu bar, the clock
  and the tray icons at the top, since the taskbar goes away.
- The Start button opens the Start menu; right clicking it opens the Win+X
  menu.
- Windows doesn't allow changing the target of the minimize animation. Mods
  that draw their own minimize animation can ask the Dock where the icon is
  (see below).

## For other mods
The `WindhawkMacDockController` window answers the registered message
`WindhawkMacDock.GetIconPoint` (`wParam` = the window): `MAKELONG(x, y)` of
the middle of the top edge of its icon, in physical pixels, or 0 if the
window isn't in the Dock. Use `SendMessageTimeout`.

Each `WindhawkMacDock` window has the properties `WindhawkMacDock.Edge`
(1 bottom, 2 left, 3 right) and `WindhawkMacDock.Reserved`: how many pixels,
from the monitor edge, the Dock takes (without magnification).

## Limitations
- Taskbar pins that aren't shortcuts (some Store apps) aren't imported; open
  the app and use "Keep in Dock".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: bottom
  $name: Position on screen
  $options:
  - bottom: Bottom
  - left: Left
  - right: Right
- monitors: all
  $name: Monitors
  $options:
  - all: One Dock on each monitor
  - primary: Primary monitor only
- iconSize: 48
  $name: Icon size
  $description: In pixels, at 100% scaling
- iconSpacing: 5
  $name: Icon spacing
  $description: In pixels, at 100% scaling
- magnification: true
  $name: Magnification
  $description: Enlarges the icons near the mouse, like in macOS
- maxScale: 180
  $name: Maximum magnification (%)
- magnificationRange: 3
  $name: Magnification range
  $description: How many icons on each side of the mouse grow along
- autoHideMode: smart
  $name: Automatically hide the Dock
  $options:
  - never: Never
  - smart: Only when a window covers the Dock
  - always: Always
- autoHideDelay: 200
  $name: Show delay (ms)
  $description: Time with the mouse at the screen edge before the Dock shows up
- edgeGap: 6
  $name: Distance from the screen edge
  $description: In pixels, at 100% scaling. Use 0 to attach the Dock to the edge
- reserveSpace: true
  $name: Reserve screen space
  $description: Maximized windows don't go under the Dock (only when the Dock never hides)
- hideTaskbar: true
  $name: Hide the Windows taskbar
- clickActive: minimize
  $name: Clicking the icon of the focused app
  $options:
  - minimize: Minimizes the window (like the taskbar)
  - activate: Does nothing (like macOS)
  - cycle: Cycles through the app's windows
- showStart: true
  $name: Show the Start button
- showRunning: true
  $name: Show running apps that aren't in the Dock
- showDownloads: true
  $name: Show the Downloads folder
- showTrash: true
  $name: Show the Recycle Bin
- showIndicators: true
  $name: Show running app indicators
- showLabels: true
  $name: Show the app name on hover
- launchBounce: true
  $name: Animate app launches
  $description: The icon bounces while the app opens
- attentionBounce: true
  $name: Bounce when an app asks for attention
- theme: auto
  $name: Theme
  $options:
  - auto: Follow Windows
  - dark: Dark
  - light: Light
- backdrop: blur
  $name: Background effect
  $options:
  - blur: Blur
  - acrylic: Acrylic
  - solid: Solid
- opacity: 45
  $name: Background color opacity (%)
  $description: Theme color applied over the effect. In solid mode, use high values
- importTaskbarPins: true
  $name: Import the apps pinned to the taskbar
  $description: Only the first time the mod runs
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#include <propsys.h>
#include <dwmapi.h>
#include <appmodel.h>
// Otherwise the Win32 macro renames ID2D1RenderTarget::DrawText.
#undef DrawText
#include <d2d1_1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

constexpr WCHAR kDockClass[] = L"WindhawkMacDock";
constexpr WCHAR kPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDragClass[] = L"WindhawkMacDockDrag";
constexpr WCHAR kControllerClass[] = L"WindhawkMacDockController";
constexpr WCHAR kAppsFolder[] = L"shell:AppsFolder\\";
constexpr size_t kAppsFolderLength = ARRAYSIZE(kAppsFolder) - 1;
constexpr WCHAR kExplorerAumid[] = L"Microsoft.Windows.Explorer";

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_APPBAR = WM_APP + 2;
constexpr UINT WM_APP_DROP = WM_APP + 3;
constexpr UINT WM_APP_MINRECT = WM_APP + 4;

constexpr UINT_PTR kTimerTick = 1;
constexpr UINT_PTR kTimerRefresh = 2;
constexpr UINT_PTR kTimerSlow = 3;
constexpr UINT_PTR kTimerCover = 4;

constexpr float kPi = 3.14159265f;
constexpr double kLaunchPeriod = 0.62;
constexpr double kAttentionPeriod = 0.45;
constexpr double kAttentionPause = 1.1;

// {374DE290-123F-4565-9164-39C4925E467B}
constexpr GUID kFolderDownloads = {
    0x374de290, 0x123f, 0x4565, {0x91, 0x64, 0x39, 0xc4, 0x92, 0x5e, 0x46, 0x7b}};
// {3EB685DB-65F9-4CF6-A03A-E3EF65729F3D}
constexpr GUID kFolderRoamingAppData = {
    0x3eb685db, 0x65f9, 0x4cf6, {0xa0, 0x3a, 0xe3, 0xef, 0x65, 0x72, 0x9f, 0x3d}};
// {AA509086-5CA9-4C25-8F95-589D3C07B48A}
constexpr GUID kClsidVirtualDesktopManager = {
    0xaa509086, 0x5ca9, 0x4c25, {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};
// {45BA127D-10A8-46EA-8AB7-56EA9078943C}
constexpr GUID kClsidApplicationActivationManager = {
    0x45ba127d, 0x10a8, 0x46ea, {0x8a, 0xb7, 0x56, 0xea, 0x90, 0x78, 0x94, 0x3c}};
// PKEY_AppUserModel_ID
constexpr PROPERTYKEY kPkeyAppUserModelId = {
    {0x9f4c2855, 0x9f79, 0x4b39, {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};

template <typename T>
T Clamp(T value, T low, T high) {
    return value < low ? low : value > high ? high : value;
}

float Approach(float value, float target, float step) {
    return value < target ? std::min<float>(value + step, target)
                          : std::max<float>(value - step, target);
}

float Smooth(float t) {
    t = Clamp(t, 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

float EaseOut(float t) {
    t = 1 - Clamp(t, 0.0f, 1.0f);
    return 1 - t * t * t;
}

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Edge { Bottom, Left, Right };
enum class Theme { Auto, Dark, Light };
enum class Backdrop { Blur, Acrylic, Solid };
enum class ActiveClick { Activate, Minimize, Cycle };
enum class AutoHide { Never, Smart, Always };

struct Settings {
    Edge edge;
    int iconSize;
    int iconSpacing;
    bool magnification;
    int maxScale;
    int range;
    AutoHide autoHide;
    int autoHideDelay;
    int edgeGap;
    bool reserveSpace;
    bool hideTaskbar;
    ActiveClick activeClick;
    bool showStart;
    bool showRunning;
    bool showDownloads;
    bool showTrash;
    bool showIndicators;
    bool showLabels;
    bool launchBounce;
    bool attentionBounce;
    Theme theme;
    Backdrop backdrop;
    int opacity;
    bool importTaskbarPins;
    bool allMonitors;
} g_settings;

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    auto& s = g_settings;

    std::wstring position = GetStringSetting(L"position");
    s.edge = position == L"left"    ? Edge::Left
             : position == L"right" ? Edge::Right
                                    : Edge::Bottom;
    s.iconSize = Clamp(Wh_GetIntSetting(L"iconSize"), 16, 128);
    s.iconSpacing = Clamp(Wh_GetIntSetting(L"iconSpacing"), 0, 64);
    s.magnification = Wh_GetIntSetting(L"magnification");
    s.maxScale = Clamp(Wh_GetIntSetting(L"maxScale"), 100, 300);
    s.range = Clamp(Wh_GetIntSetting(L"magnificationRange"), 1, 8);
    std::wstring autoHide = GetStringSetting(L"autoHideMode");
    s.autoHide = autoHide == L"never"    ? AutoHide::Never
                 : autoHide == L"always" ? AutoHide::Always
                                         : AutoHide::Smart;
    s.autoHideDelay = Clamp(Wh_GetIntSetting(L"autoHideDelay"), 0, 5000);
    s.edgeGap = Clamp(Wh_GetIntSetting(L"edgeGap"), 0, 64);
    s.reserveSpace = Wh_GetIntSetting(L"reserveSpace");
    s.hideTaskbar = Wh_GetIntSetting(L"hideTaskbar");

    std::wstring click = GetStringSetting(L"clickActive");
    s.activeClick = click == L"minimize" ? ActiveClick::Minimize
                    : click == L"cycle"  ? ActiveClick::Cycle
                                         : ActiveClick::Activate;

    s.showStart = Wh_GetIntSetting(L"showStart");
    s.showRunning = Wh_GetIntSetting(L"showRunning");
    s.showDownloads = Wh_GetIntSetting(L"showDownloads");
    s.showTrash = Wh_GetIntSetting(L"showTrash");
    s.showIndicators = Wh_GetIntSetting(L"showIndicators");
    s.showLabels = Wh_GetIntSetting(L"showLabels");
    s.launchBounce = Wh_GetIntSetting(L"launchBounce");
    s.attentionBounce = Wh_GetIntSetting(L"attentionBounce");

    std::wstring theme = GetStringSetting(L"theme");
    s.theme = theme == L"dark"    ? Theme::Dark
              : theme == L"light" ? Theme::Light
                                  : Theme::Auto;

    std::wstring backdrop = GetStringSetting(L"backdrop");
    s.backdrop = backdrop == L"acrylic" ? Backdrop::Acrylic
                 : backdrop == L"solid" ? Backdrop::Solid
                                        : Backdrop::Blur;
    s.opacity = Clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    s.importTaskbarPins = Wh_GetIntSetting(L"importTaskbarPins");
    s.allMonitors = GetStringSetting(L"monitors") != L"primary";
}

////////////////////////////////////////////////////////////////////////////////
// Utilities

std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuffW(s.data(), (DWORD)s.size());
    }
    return s;
}

bool SameText(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), (int)a.size(), b.c_str(),
                                (int)b.size(), TRUE) == CSTR_EQUAL;
}

bool StartsWithI(const std::wstring& s, PCWSTR prefix) {
    int n = (int)wcslen(prefix);
    return (int)s.size() >= n &&
           CompareStringOrdinal(s.c_str(), n, prefix, n, TRUE) == CSTR_EQUAL;
}

bool EndsWithI(const std::wstring& s, PCWSTR suffix) {
    int n = (int)wcslen(suffix);
    return (int)s.size() >= n &&
           CompareStringOrdinal(s.c_str() + s.size() - n, n, suffix, n,
                                TRUE) == CSTR_EQUAL;
}

std::wstring FileTitle(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.rfind(L'.');
    if (dot != std::wstring::npos && dot > 0) {
        name.resize(dot);
    }
    return name;
}

std::wstring ExpandEnvironment(const std::wstring& s) {
    WCHAR buffer[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), buffer, ARRAYSIZE(buffer));
    return n && n <= ARRAYSIZE(buffer) ? std::wstring(buffer) : s;
}

std::wstring KnownFolderPath(REFKNOWNFOLDERID id) {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path))) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

std::wstring GetFileDescription(const std::wstring& path) {
    DWORD handle;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (!size) {
        return {};
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, data.data())) {
        return {};
    }

    struct LangCodePage {
        WORD language;
        WORD codePage;
    }* translations;
    UINT len;
    if (!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                        (void**)&translations, &len) ||
        len < sizeof(LangCodePage)) {
        return {};
    }

    WCHAR subBlock[64];
    swprintf(subBlock, ARRAYSIZE(subBlock),
             L"\\StringFileInfo\\%04x%04x\\FileDescription",
             translations[0].language, translations[0].codePage);

    WCHAR* description;
    if (VerQueryValueW(data.data(), subBlock, (void**)&description, &len) &&
        len > 1) {
        return description;
    }
    return {};
}

std::wstring AppNameFromPath(const std::wstring& path) {
    std::wstring name = GetFileDescription(path);
    return name.empty() ? FileTitle(path) : name;
}

std::wstring DisplayName(IShellItem* item) {
    PWSTR name = nullptr;
    std::wstring result;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name) {
        result = name;
    }
    CoTaskMemFree(name);
    return result;
}

std::wstring ProcessPath(DWORD pid) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return {};
    }
    WCHAR path[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result.assign(path, size);
    }
    CloseHandle(process);
    return result;
}

std::wstring ProcessAumid(DWORD pid) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return {};
    }
    WCHAR aumid[130];  // APPLICATION_USER_MODEL_ID_MAX_LENGTH
    UINT32 length = ARRAYSIZE(aumid);
    std::wstring result;
    if (GetApplicationUserModelId(process, &length, aumid) == ERROR_SUCCESS) {
        result = aumid;
    }
    CloseHandle(process);
    return result;
}

std::wstring ReadAumid(IPropertyStore* store) {
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring result;
    if (SUCCEEDED(store->GetValue(kPkeyAppUserModelId, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        result = value.pwszVal;
    }
    PropVariantClear(&value);
    return result;
}

std::wstring WindowAumid(HWND hwnd) {
    ComPtr<IPropertyStore> store;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&store)))) {
        return {};
    }
    return ReadAumid(store.Get());
}

std::wstring MenuText(std::wstring text) {
    if (text.size() > 64) {
        text.resize(63);
        text += L"…";
    }
    for (size_t i = 0; (i = text.find(L'&', i)) != std::wstring::npos;
         i += 2) {
        text.insert(i, 1, L'&');
    }
    return text;
}

void SendKeys(std::initializer_list<WORD> keys) {
    std::vector<INPUT> inputs;
    for (WORD key : keys) {
        INPUT input{INPUT_KEYBOARD};
        input.ki.wVk = key;
        input.ki.dwFlags = key == VK_LWIN ? KEYEVENTF_EXTENDEDKEY : 0;
        inputs.push_back(input);
    }
    for (auto it = keys.end(); it != keys.begin();) {
        WORD key = *--it;
        INPUT input{INPUT_KEYBOARD};
        input.ki.wVk = key;
        input.ki.dwFlags = KEYEVENTF_KEYUP |
                           (key == VK_LWIN ? KEYEVENTF_EXTENDEDKEY : 0);
        inputs.push_back(input);
    }
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

void Run(PCWSTR file, PCWSTR params = nullptr) {
    ShellExecuteW(nullptr, nullptr, file, params, nullptr, SW_SHOWNORMAL);
}

////////////////////////////////////////////////////////////////////////////////
// Windows taskbar
//
// Hiding the window isn't enough: it keeps reserving space and shows up
// again by itself. We turn on its auto-hide (which frees the space) and the
// hooks keep it from being shown again.

std::atomic<bool> g_taskbarHidden;
bool g_changedAutohide;
UINT g_originalTaskbarState;

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR className[32];
    return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
           (!wcscmp(className, L"Shell_TrayWnd") ||
            !wcscmp(className, L"Shell_SecondaryTrayWnd"));
}

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (g_taskbarHidden && cmd != SW_HIDE && IsTaskbarWindow(hwnd)) {
        cmd = SW_HIDE;
    }
    return ShowWindow_Original(hwnd, cmd);
}

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hwnd,
                              HWND insertAfter,
                              int x,
                              int y,
                              int cx,
                              int cy,
                              UINT flags) {
    if (g_taskbarHidden && (flags & SWP_SHOWWINDOW) && IsTaskbarWindow(hwnd)) {
        flags = (flags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
    }
    return SetWindowPos_Original(hwnd, insertAfter, x, y, cx, cy, flags);
}

BOOL CALLBACK ShowTaskbarsProc(HWND hwnd, LPARAM show) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsTaskbarWindow(hwnd) &&
        (bool)IsWindowVisible(hwnd) != (bool)show) {
        // Asynchronous: the taskbar belongs to another thread, which may be busy.
        ShowWindowAsync(hwnd, show ? SW_SHOWNA : SW_HIDE);
    }
    return TRUE;
}

HWND FindOurTaskbar() {
    DWORD pid = 0;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) &&
                   pid == GetCurrentProcessId()
               ? tray
               : nullptr;
}

// A simulated Windows key doesn't open Start on recent Windows 11; the
// message to the taskbar does, even while it's hidden.
void OpenStartMenu() {
    if (HWND tray = FindOurTaskbar()) {
        PostMessageW(tray, WM_SYSCOMMAND, SC_TASKLIST, 0);
    } else {
        SendKeys({VK_LWIN});
    }
}

void SetTaskbarHidden(bool hide) {
    HWND tray = FindOurTaskbar();
    if (hide) {
        g_taskbarHidden = true;
        if (!tray) {
            return;  // The timer tries again once it exists.
        }
        if (!g_changedAutohide) {
            APPBARDATA abd{sizeof(abd)};
            abd.hWnd = tray;
            UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
            if (!(state & ABS_AUTOHIDE)) {
                abd.lParam = state | ABS_AUTOHIDE;
                SHAppBarMessage(ABM_SETSTATE, &abd);
                g_changedAutohide = true;
                g_originalTaskbarState = state;
                // To undo it even if explorer crashes before the end.
                Wh_SetIntValue(L"taskbarState", (int)state);
            }
        }
        EnumWindows(ShowTaskbarsProc, FALSE);
        return;
    }

    bool wasHidden = g_taskbarHidden.exchange(false);
    if (g_changedAutohide && tray) {
        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = tray;
        abd.lParam = g_originalTaskbarState;
        SHAppBarMessage(ABM_SETSTATE, &abd);
        g_changedAutohide = false;
        Wh_DeleteValue(L"taskbarState");
    }
    if (wasHidden) {
        EnumWindows(ShowTaskbarsProc, TRUE);
    }
}

bool TaskbarAutohides() {
    APPBARDATA abd{sizeof(abd)};
    return SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE;
}

////////////////////////////////////////////////////////////////////////////////
// Icon images

ComPtr<ID2D1Factory> g_d2d;
ComPtr<IDWriteFactory> g_dwrite;
ComPtr<ID2D1DCRenderTarget> g_target;
ComPtr<ID2D1DeviceContext> g_context;
ComPtr<ID2D1SolidColorBrush> g_brush;
UINT g_targetGeneration = 1;

struct Image {
    UINT size = 0;                // Square.
    std::vector<UINT32> pixels;   // Premultiplied BGRA, top to bottom.
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};
using ImagePtr = std::shared_ptr<Image>;

// Normalizes the alpha and crops small icons centered in a large image
// (old exes without a 256 px icon), otherwise they would look tiny.
ImagePtr FinishImage(std::vector<UINT32> pixels, int width, int height) {
    if (std::none_of(pixels.begin(), pixels.end(),
                     [](UINT32 p) { return (p >> 24) != 0; })) {
        for (auto& p : pixels) {
            p |= 0xFF000000;
        }
    }

    bool premultiplied = std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) {
        UINT a = p >> 24;
        return ((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a ||
               (p & 0xFF) > a;
    });
    if (!premultiplied) {
        for (auto& p : pixels) {
            UINT a = p >> 24;
            UINT r = ((p >> 16) & 0xFF) * a / 255;
            UINT g = ((p >> 8) & 0xFF) * a / 255;
            UINT b = (p & 0xFF) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((pixels[y * width + x] >> 24) > 8) {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }
    if (right < 0) {
        return nullptr;
    }

    int full = std::max(width, height);
    int content = std::max(right - left + 1, bottom - top + 1);
    int side, cx, cy;
    if (content < full * 0.6f) {
        side = std::min(full, (int)std::ceil(content * 1.08f));
        cx = (left + right + 1) / 2;
        cy = (top + bottom + 1) / 2;
    } else {
        side = full;
        cx = width / 2;
        cy = height / 2;
    }

    auto image = std::make_shared<Image>();
    image->size = side;
    image->pixels.assign((size_t)side * side, 0);
    int x0 = cx - side / 2, y0 = cy - side / 2;
    for (int y = 0; y < side; y++) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) {
            continue;
        }
        for (int x = 0; x < side; x++) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) {
                image->pixels[(size_t)y * side + x] = pixels[sy * width + sx];
            }
        }
    }
    return image;
}

ImagePtr ImageFromHBitmap(HBITMAP bitmap) {
    BITMAP bm{};
    if (!GetObjectW(bitmap, sizeof(bm), &bm) || bm.bmWidth <= 0) {
        return nullptr;
    }
    int width = bm.bmWidth, height = std::abs(bm.bmHeight);
    std::vector<UINT32> pixels((size_t)width * height);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, bitmap, 0, height, pixels.data(), &bi,
                          DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (!lines) {
        return nullptr;
    }
    return FinishImage(std::move(pixels), width, height);
}

ImagePtr ImageFromIcon(HICON icon, int size) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bitmap =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ old = SelectObject(dc, bitmap);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
    GdiFlush();
    SelectObject(dc, old);
    std::vector<UINT32> pixels((UINT32*)bits, (UINT32*)bits + size * size);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return FinishImage(std::move(pixels), size, size);
}

ImagePtr ImageFromShellItem(IShellItem* item, int size) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory))) ||
        FAILED(factory->GetImage(SIZE{size, size},
                                 SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                 &bitmap))) {
        return nullptr;
    }
    ImagePtr image = ImageFromHBitmap(bitmap);
    DeleteObject(bitmap);
    return image;
}

ImagePtr ImageFromParsingName(const std::wstring& name, int size) {
    ComPtr<IShellItem> item;
    if (FAILED(SHCreateItemFromParsingName(name.c_str(), nullptr,
                                           IID_PPV_ARGS(&item)))) {
        return nullptr;
    }
    return ImageFromShellItem(item.Get(), size);
}

HICON GetWindowIcon(HWND hwnd) {
    DWORD_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result) &&
        result) {
        return (HICON)result;
    }
    if (HICON icon = (HICON)GetClassLongPtrW(hwnd, GCLP_HICON)) {
        return icon;
    }
    return (HICON)GetClassLongPtrW(hwnd, GCLP_HICONSM);
}

// The Recycle Bin has different icons when full and empty, kept in the registry.
ImagePtr LoadTrashImage(bool full, int size) {
    constexpr WCHAR kUserKey[] =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CLSID\\"
        L"{645FF040-5081-101B-9F08-00AA002F954E}\\DefaultIcon";
    constexpr WCHAR kClassKey[] =
        L"CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\DefaultIcon";
    PCWSTR valueName = full ? L"full" : L"empty";

    WCHAR value[MAX_PATH * 2] = L"";
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, kUserKey, valueName,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
                     nullptr, value, &bytes) != ERROR_SUCCESS) {
        bytes = sizeof(value);
        RegGetValueW(HKEY_CLASSES_ROOT, kClassKey, valueName,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
                     nullptr, value, &bytes);
    }

    std::wstring location = ExpandEnvironment(value);
    int index = 0;
    size_t comma = location.rfind(L',');
    if (comma != std::wstring::npos) {
        index = _wtoi(location.c_str() + comma + 1);
        location.resize(comma);
    }
    location.erase(std::remove(location.begin(), location.end(), L'"'),
                   location.end());

    HICON icon = nullptr;
    if (!location.empty() &&
        SHDefExtractIconW(location.c_str(), index, 0, &icon, nullptr,
                          MAKELONG(size, 16)) == S_OK &&
        icon) {
        ImagePtr image = ImageFromIcon(icon, size);
        DestroyIcon(icon);
        if (image) {
            return image;
        }
    }
    return ImageFromParsingName(L"shell:RecycleBinFolder", size);
}

ID2D1Bitmap* GetBitmap(Image& image) {
    if (image.generation != g_targetGeneration) {
        image.generation = g_targetGeneration;
        image.bitmap.Reset();
        g_target->CreateBitmap(
            D2D1::SizeU(image.size, image.size), image.pixels.data(),
            image.size * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            &image.bitmap);
    }
    return image.bitmap.Get();
}

HBITMAP IconToMenuBitmap(HICON icon, int size) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bitmap =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ old = SelectObject(dc, bitmap);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
    SelectObject(dc, old);
    DeleteDC(dc);
    return bitmap;
}

////////////////////////////////////////////////////////////////////////////////
// Dock items

enum class Kind { Start, App, Separator, Downloads, Trash };
enum class Bounce { None, Launch, Attention };

// An app's identity: AUMID and executable path, lowercase for comparing, and
// the originals for using.
struct AppIds {
    std::wstring aumid, path;
    std::wstring aumidRaw, pathRaw;
    bool pseudo = false;  // Window without a readable process: each one is an item.
};

bool IdsMatch(const std::wstring& aumidA,
              const std::wstring& pathA,
              const std::wstring& aumidB,
              const std::wstring& pathB) {
    if (!aumidA.empty() && !aumidB.empty()) {
        return aumidA == aumidB;
    }
    return !pathA.empty() && pathA == pathB;
}

struct Item {
    Kind kind = Kind::App;
    bool pinned = false;
    std::wstring target;  // What ShellExecute opens.
    std::wstring name;
    std::wstring aumid, path, aumidRaw, pathRaw;
    bool pseudo = false;
    ImagePtr icon;
    bool iconTried = false;
    std::vector<HWND> windows;  // In Z order, most recent first.
    HWND iconWindow = nullptr;

    int group = 0;
    double order = 0;

    // Animations.
    float presence = 0;  // 0 = out of the Dock, 1 = taking all of its space.
    bool removing = false;
    bool hidden = false;  // Removed with the cloud: only the gap closes.
    float running = 0;
    float label = 0;
    float shift = 0;  // Offset left over from a reorder.
    Bounce bounce = Bounce::None;
    double bounceStart = 0;
    bool bounceStop = false;
    double bounceStopAt = 0;
    size_t launchWindows = 0;

    // Layout of the current frame.
    float slot = 0, mslot = 0, baseCenter = 0, scale = 1, center = 0;
    float size = 0, bottomV = 0;
    float labelWidth = 0;
    UINT labelDpi = 0;

    // The window minimized last: a click on the icon brings it back.
    HWND lastMinimized = nullptr;
};

std::vector<std::unique_ptr<Item>> g_items;
std::vector<std::wstring> g_pins;
double g_orderCounter = 0;
bool g_ready;  // After the first build, new items animate in.

struct Layout {
    float plateU0 = 0, plateU1 = 0;
    float plateV0 = 0;
    float magTopV = 0;
    Item* hovered = nullptr;
    bool valid = false;
};

struct PressState {
    Item* item = nullptr;
    POINT point{};
} g_press;

struct DragState {
    Item* item = nullptr;
    bool active = false;
    bool out = false;
    POINT grab{};  // Cursor relative to the icon center.
    float size = 0;
    bool settling = false;
    double settleStart = 0;
    POINT settleFrom{};
    bool poof = false;
    double poofStart = 0;
    POINT poofAt{};
} g_drag;

struct DropState {
    bool active = false;
    POINT point{};
    std::vector<std::wstring> files;
    ComPtr<IDataObject> data;
    Item* target = nullptr;
} g_drop;

struct PendingDrop {
    Item* target = nullptr;
    Kind kind = Kind::App;
    std::vector<std::wstring> files;
    ComPtr<IDataObject> data;
    int insertIndex = 0;
};

void ClearHovered(Item* item);

void ClearReferences(Item* item) {
    ClearHovered(item);
    if (g_press.item == item) {
        g_press.item = nullptr;
    }
    if (g_drag.item == item) {
        g_drag.item = nullptr;
        g_drag.active = g_drag.settling = false;
    }
    if (g_drop.target == item) {
        g_drop.target = nullptr;
    }
}

bool ItemExists(Item* item) {
    return std::any_of(g_items.begin(), g_items.end(),
                       [item](const auto& it) { return it.get() == item; });
}

bool ResolveLink(const std::wstring& link, AppIds& ids) {
    ComPtr<IShellLinkW> shellLink;
    ComPtr<IPersistFile> file;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&shellLink))) ||
        FAILED(shellLink.As(&file)) ||
        FAILED(file->Load(link.c_str(), STGM_READ))) {
        return false;
    }
    WCHAR path[MAX_PATH * 2] = L"";
    if (shellLink->GetPath(path, ARRAYSIZE(path), nullptr, SLGP_RAWPATH) ==
            S_OK &&
        *path) {
        ids.pathRaw = ExpandEnvironment(path);
    }
    ComPtr<IPropertyStore> store;
    if (SUCCEEDED(shellLink.As(&store))) {
        ids.aumidRaw = ReadAumid(store.Get());
    }
    return true;
}

AppIds ResolveTarget(const std::wstring& target) {
    AppIds ids;
    if (StartsWithI(target, kAppsFolder)) {
        ids.aumidRaw = target.substr(kAppsFolderLength);
    } else if (EndsWithI(target, L".lnk")) {
        ResolveLink(target, ids);
    } else {
        ids.pathRaw = target;
    }
    ids.aumid = ToLower(ids.aumidRaw);
    ids.path = ToLower(ids.pathRaw);
    return ids;
}

std::wstring TargetName(const std::wstring& target) {
    if (!StartsWithI(target, kAppsFolder) && EndsWithI(target, L".exe")) {
        return AppNameFromPath(target);
    }
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(target.c_str(), nullptr,
                                              IID_PPV_ARGS(&item)))) {
        std::wstring name = DisplayName(item.Get());
        if (!name.empty()) {
            return name;
        }
    }
    return FileTitle(target);
}

void SetIds(Item& item, const AppIds& ids) {
    item.aumid = ids.aumid;
    item.path = ids.path;
    item.aumidRaw = ids.aumidRaw;
    item.pathRaw = ids.pathRaw;
    item.pseudo = ids.pseudo;
}

bool Matches(const Item& item, const AppIds& ids) {
    return IdsMatch(item.aumid, item.path, ids.aumid, ids.path);
}

////////////////////////////////////////////////////////////////////////////////
// Pinned apps

void SavePins() {
    std::wstring joined;
    for (const auto& pin : g_pins) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += pin;
    }
    Wh_SetStringValue(L"pins", joined.c_str());
    Wh_SetIntValue(L"pinsSaved", 1);
}

std::vector<std::wstring> ImportTaskbarPins() {
    std::vector<std::wstring> pins;
    std::wstring dir = KnownFolderPath(kFolderRoamingAppData);
    if (dir.empty()) {
        return pins;
    }
    dir += L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\"
           L"TaskBar\\";

    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileW((dir + L"*.lnk").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) {
        return pins;
    }
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            pins.push_back(dir + fd.cFileName);
        }
    } while (FindNextFileW(find, &fd));
    FindClose(find);

    // File Explorer comes first, like the Finder.
    std::stable_partition(pins.begin(), pins.end(), [](const std::wstring& pin) {
        AppIds ids = ResolveTarget(pin);
        return ids.aumid == ToLower(kExplorerAumid) ||
               EndsWithI(ids.path, L"\\explorer.exe");
    });
    return pins;
}

void LoadPins() {
    g_pins.clear();
    if (Wh_GetIntValue(L"pinsSaved", 0)) {
        std::vector<WCHAR> buffer(65536);
        Wh_GetStringValue(L"pins", buffer.data(), buffer.size());
        std::wstring all = buffer.data();
        size_t start = 0;
        while (start <= all.size()) {
            size_t end = all.find(L'\n', start);
            if (end == std::wstring::npos) {
                end = all.size();
            }
            if (end > start) {
                g_pins.push_back(all.substr(start, end - start));
            }
            start = end + 1;
        }
        return;
    }

    if (g_settings.importTaskbarPins) {
        g_pins = ImportTaskbarPins();
    }
    if (g_pins.empty()) {
        g_pins = {
            std::wstring(kAppsFolder) + kExplorerAumid,
            std::wstring(kAppsFolder) +
                L"windows.immersivecontrolpanel_cw5n1h2txyewy!"
                L"microsoft.windows.immersivecontrolpanel",
        };
    }
    SavePins();
}

int PinIndex(const std::wstring& target) {
    for (size_t i = 0; i < g_pins.size(); i++) {
        if (SameText(g_pins[i], target)) {
            return (int)i;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Open windows

struct WindowInfo {
    DWORD pid = 0;
    double time = 0;
    AppIds ids;
};

std::unordered_map<HWND, WindowInfo> g_windowCache;
ComPtr<IVirtualDesktopManager> g_desktops;
// Invisible window that receives the timers and the system notifications;
// the Docks come and go with the monitors.
std::atomic<HWND> g_controller;
HWND g_dragWnd;

bool IsOurWindow(HWND hwnd);

bool IsOnCurrentDesktop(HWND hwnd) {
    BOOL on = TRUE;
    if (g_desktops &&
        SUCCEEDED(g_desktops->IsWindowOnCurrentVirtualDesktop(hwnd, &on))) {
        return on;
    }
    return true;
}

// GetWindowText sends messages to explorer's own windows and would hang
// the Dock along with a hung window.
bool HasTitle(HWND hwnd) {
    WCHAR title[2];
    return InternalGetWindowText(hwnd, title, ARRAYSIZE(title)) > 0;
}

bool IsTaskWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsOurWindow(hwnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_APPWINDOW) &&
        ((exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
         GetWindow(hwnd, GW_OWNER) || !HasTitle(hwnd))) {
        return false;
    }

    WCHAR className[64] = L"";
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    static const PCWSTR kSkip[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow"};
    for (PCWSTR skip : kSkip) {
        if (!wcscmp(className, skip)) {
            return false;
        }
    }

    // Windows on other virtual desktops are "cloaked" by the shell, but the
    // app is still running. The others really are hidden.
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        return (cloaked & DWM_CLOAKED_SHELL) && !(cloaked & DWM_CLOAKED_APP) &&
               !IsOnCurrentDesktop(hwnd);
    }
    return true;
}

BOOL CALLBACK FindCoreWindowProc(HWND hwnd, LPARAM param) {
    auto* pid = (DWORD*)param;
    WCHAR className[64];
    DWORD childPid = 0;
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        !wcscmp(className, L"Windows.UI.Core.CoreWindow") &&
        GetWindowThreadProcessId(hwnd, &childPid) && childPid != *pid) {
        *pid = childPid;
        return FALSE;
    }
    return TRUE;
}

const WindowInfo& GetWindowInfo(HWND hwnd, double now) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    auto found = g_windowCache.find(hwnd);
    // Some apps (Chrome, for example) set the AUMID right after creating the
    // window, so the first reads are redone.
    if (found != g_windowCache.end() && found->second.pid == pid &&
        now - found->second.time > 3) {
        return found->second;
    }

    WindowInfo info;
    info.pid = pid;
    info.time = found != g_windowCache.end() && found->second.pid == pid
                    ? found->second.time
                    : now;

    WCHAR className[64] = L"";
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    DWORD contentPid = pid;
    if (!wcscmp(className, L"ApplicationFrameWindow")) {
        EnumChildWindows(hwnd, FindCoreWindowProc, (LPARAM)&contentPid);
    }

    std::wstring aumid = WindowAumid(hwnd);
    if (aumid.empty() && (!wcscmp(className, L"CabinetWClass") ||
                          !wcscmp(className, L"ExploreWClass"))) {
        aumid = kExplorerAumid;
    }
    if (aumid.empty()) {
        aumid = ProcessAumid(contentPid);
    }
    std::wstring path = ProcessPath(contentPid);
    if (aumid.empty() && path.empty()) {
        WCHAR pseudo[32];
        swprintf(pseudo, ARRAYSIZE(pseudo), L"hwnd:%p", hwnd);
        path = pseudo;
        info.ids.pseudo = true;
    }
    info.ids.aumidRaw = aumid;
    info.ids.pathRaw = path;
    info.ids.aumid = ToLower(aumid);
    info.ids.path = ToLower(path);
    return g_windowCache[hwnd] = std::move(info);
}

void ComputeBase();

// Reorders the items and turns the position jump into an animated offset.
void Reorder() {
    std::unordered_map<Item*, float> before;
    for (auto& it : g_items) {
        before[it.get()] = it->baseCenter;
    }
    std::stable_sort(g_items.begin(), g_items.end(),
                     [](const auto& a, const auto& b) {
                         return a->group != b->group ? a->group < b->group
                                                     : a->order < b->order;
                     });
    ComputeBase();
    if (!g_ready) {
        return;
    }
    for (auto& it : g_items) {
        if (it->presence > 0 && it->slot > 0) {
            it->shift += before[it.get()] - it->baseCenter;
        }
    }
}

Item* CreateRunningItem(HWND hwnd, const WindowInfo& info) {
    auto item = std::make_unique<Item>();
    item->kind = Kind::App;
    item->group = 2;
    item->order = ++g_orderCounter;
    item->presence = g_ready ? 0 : 1;
    item->iconWindow = hwnd;
    SetIds(*item, info.ids);

    if (!item->aumidRaw.empty()) {
        std::wstring parsing = kAppsFolder + item->aumidRaw;
        ComPtr<IShellItem> shellItem;
        if (SUCCEEDED(SHCreateItemFromParsingName(
                parsing.c_str(), nullptr, IID_PPV_ARGS(&shellItem)))) {
            item->target = parsing;
            item->name = DisplayName(shellItem.Get());
        }
    }
    if (item->target.empty() && !item->pseudo) {
        item->target = item->pathRaw;
        item->name = AppNameFromPath(item->pathRaw);
    }
    if (item->name.empty()) {
        WCHAR title[256] = L"";
        InternalGetWindowText(hwnd, title, ARRAYSIZE(title));
        item->name = title;
    }

    g_items.push_back(std::move(item));
    return g_items.back().get();
}

BOOL CALLBACK CollectTaskWindowsProc(HWND hwnd, LPARAM param) {
    if (IsTaskWindow(hwnd)) {
        ((std::vector<HWND>*)param)->push_back(hwnd);
    }
    return TRUE;
}

void RefreshWindows() {
    double now = Now();
    std::vector<HWND> windows;
    EnumWindows(CollectTaskWindowsProc, (LPARAM)&windows);

    for (auto i = g_windowCache.begin(); i != g_windowCache.end();) {
        i = IsWindow(i->first) ? std::next(i) : g_windowCache.erase(i);
    }

    for (auto& it : g_items) {
        it->windows.clear();
    }

    bool created = false;
    for (HWND hwnd : windows) {
        const WindowInfo& info = GetWindowInfo(hwnd, now);
        Item* match = nullptr;
        for (auto& it : g_items) {
            if (it->kind == Kind::App && it->pinned && Matches(*it, info.ids)) {
                match = it.get();
                break;
            }
        }
        if (!match && g_settings.showRunning) {
            for (auto& it : g_items) {
                if (it->kind == Kind::App && !it->pinned &&
                    Matches(*it, info.ids)) {
                    match = it.get();
                    break;
                }
            }
            if (!match) {
                match = CreateRunningItem(hwnd, info);
                created = true;
            }
        }
        if (!match) {
            continue;
        }
        if (match->removing && !match->hidden) {
            match->removing = false;
        }
        match->windows.push_back(hwnd);
    }

    for (auto& it : g_items) {
        if (it->kind == Kind::App && !it->pinned && it->windows.empty()) {
            it->removing = true;
        }
    }
    if (created) {
        Reorder();
    }
}

Item* ItemForWindow(HWND hwnd) {
    for (auto& it : g_items) {
        if (std::find(it->windows.begin(), it->windows.end(), hwnd) !=
            it->windows.end()) {
            return it.get();
        }
    }
    return nullptr;
}

// Applies g_pins to the items: creates, converts and unpins what changed.
void SyncPins() {
    for (auto& it : g_items) {
        if (it->kind != Kind::App || !it->pinned ||
            PinIndex(it->target) >= 0) {
            continue;
        }
        it->pinned = false;
        if (it->windows.empty()) {
            it->removing = true;
        } else {
            it->group = 2;
            it->order = ++g_orderCounter;
        }
    }

    for (size_t i = 0; i < g_pins.size(); i++) {
        const std::wstring& pin = g_pins[i];
        Item* found = nullptr;
        for (auto& it : g_items) {
            if (it->kind == Kind::App && it->pinned &&
                SameText(it->target, pin)) {
                found = it.get();
                break;
            }
        }
        if (!found) {
            AppIds ids = ResolveTarget(pin);
            // A running app that was just pinned stays the same item.
            for (auto& it : g_items) {
                if (it->kind == Kind::App && !it->pinned && !it->hidden &&
                    (SameText(it->target, pin) ||
                     IdsMatch(it->aumid, it->path, ids.aumid, ids.path))) {
                    found = it.get();
                    break;
                }
            }
            if (found) {
                found->target = pin;
            } else {
                auto item = std::make_unique<Item>();
                item->kind = Kind::App;
                item->target = pin;
                SetIds(*item, ids);
                item->name = TargetName(pin);
                item->presence = g_ready ? 0 : 1;
                g_items.push_back(std::move(item));
                found = g_items.back().get();
            }
        }
        found->pinned = true;
        found->removing = found->hidden = false;
        found->group = 1;
        found->order = (double)i;
    }

    RefreshWindows();
    Reorder();
}

////////////////////////////////////////////////////////////////////////////////
// Geometry
//
// The layout uses Dock coordinates: u along the edge (in screen
// coordinates) and v the distance to the edge. This way the three
// positions share the same code.

struct Geometry {
    HMONITOR monitor = nullptr;
    RECT monitorRect{};
    UINT dpi = 96;
    float scale = 1;
    float icon = 48, gap = 4, pad = 6, plate = 60, radius = 18, edgeGap = 6;
    float separator = 20, maxScale = 1.8f, range = 150;
    float labelHeight = 26, labelGap = 10;
    int thickness = 200;
    int edgeCoord = 0;
    int axisStart = 0, axisEnd = 0;
    int iconPixels = 96;
};

struct Span {
    int origin = 0;
    int length = 1;
};

// One Dock per monitor. The items are the same in all of them; position,
// magnification, hiding and windows belong to each one.
struct Dock {
    std::wstring device;  // Monitor name, which survives display changes.
    HMONITOR monitor = nullptr;
    HWND wnd = nullptr, plate = nullptr;
    Geometry geo;
    Span span;
    Layout layout;
    ComPtr<IDWriteTextFormat> labelFormat;
    float mag = 0;           // 0..1, magnification in and out.
    float hideProgress = 0;  // 0 = visible, 1 = hidden.
    bool hovering = false;
    bool wantShow = true;
    bool fullscreen = false;
    bool covered = false;  // A window is over the Dock.
    double edgeSince = 0;
    double lastInside = 0;
    float lastCursorU = 0;
    float baseStart = 0, baseTotal = 0;
    RECT plateRect{};
    bool appBar = false;
    RECT appBarRect{};
    bool dirty = true;
    bool wasAnimating = false;
};

std::vector<std::unique_ptr<Dock>> g_docks;
Dock* g_d;       // The Dock being computed or drawn right now.
Dock* g_active;  // The one on the mouse's monitor; outside loops, g_d is it.
int g_iconPixels = 96;  // The largest among the Docks; the icons are shared.

bool IsOurWindow(HWND hwnd) {
    if (hwnd == g_controller || hwnd == g_dragWnd) {
        return true;
    }
    for (auto& d : g_docks) {
        if (hwnd == d->wnd || hwnd == d->plate) {
            return true;
        }
    }
    return false;
}

void ClearHovered(Item* item) {
    for (auto& d : g_docks) {
        if (d->layout.hovered == item) {
            d->layout.hovered = nullptr;
        }
    }
}

void MarkDirty() {
    for (auto& d : g_docks) {
        d->dirty = true;
    }
}

void UpdateGeometry() {
    HMONITOR monitor = g_d->monitor;
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    auto& g = g_d->geo;
    bool dpiChanged = g.dpi != dpiX || !g_d->labelFormat;
    g.monitor = monitor;
    g.monitorRect = mi.rcMonitor;
    g.dpi = dpiX;
    g.scale = dpiX / 96.0f;

    const auto& s = g_settings;
    g.icon = std::round(s.iconSize * g.scale);
    g.gap = std::round(s.iconSpacing * g.scale);
    g.pad = std::round(g.icon * 0.12f);
    g.plate = g.icon + 2 * g.pad;
    // The blur only comes out rounded with DWM's own corner (8 px at 100%),
    // so the drawn background uses the same radius. Solid can be rounder.
    g.radius = s.backdrop == Backdrop::Solid ? std::round(g.plate * 0.3f)
                                             : 8 * g.scale;
    g.edgeGap = std::round(s.edgeGap * g.scale);
    g.separator = std::round(g.icon * 0.45f);
    g.maxScale = s.magnification ? s.maxScale / 100.0f : 1.0f;
    g.range = s.range * (g.icon + g.gap);
    g.labelHeight = std::round(26 * g.scale);
    g.labelGap = std::round(10 * g.scale);
    // On the sides the name tooltip sits next to the icon, so the window
    // needs its width, not its height.
    float labelSpace = s.edge == Edge::Bottom ? g.labelHeight : 260 * g.scale;
    g.thickness = (int)std::ceil(g.edgeGap + g.pad +
                                 g.icon * std::max<float>(g.maxScale, 1.9f) +
                                 g.labelGap + labelSpace + 6 * g.scale);
    g.iconPixels = std::min(256, (int)std::ceil(g.icon * g.maxScale));

    RECT mon = mi.rcMonitor;
    switch (s.edge) {
        case Edge::Bottom:
            g.edgeCoord = mon.bottom;
            g.axisStart = mon.left;
            g.axisEnd = mon.right;
            break;
        case Edge::Left:
            g.edgeCoord = mon.left;
            g.axisStart = mon.top;
            g.axisEnd = mon.bottom;
            break;
        case Edge::Right:
            g.edgeCoord = mon.right;
            g.axisStart = mon.top;
            g.axisEnd = mon.bottom;
            break;
    }

    // With the taskbar visible on the same edge, the Dock sits above it.
    HWND tray = g_taskbarHidden ? nullptr : FindOurTaskbar();
    RECT rc;
    if (tray && IsWindowVisible(tray) &&
        MonitorFromWindow(tray, MONITOR_DEFAULTTONULL) == monitor &&
        !TaskbarAutohides() && GetWindowRect(tray, &rc)) {
        switch (s.edge) {
            case Edge::Bottom:
                if (rc.top > (mon.top + mon.bottom) / 2) {
                    g.edgeCoord = rc.top;
                }
                break;
            case Edge::Left:
                if (rc.right < (mon.left + mon.right) / 2) {
                    g.edgeCoord = rc.right;
                }
                break;
            case Edge::Right:
                if (rc.left > (mon.left + mon.right) / 2) {
                    g.edgeCoord = rc.left;
                }
                break;
        }
    }

    // For other mods (the fullscreen Start leaves this strip free).
    if (g_d->wnd) {
        int reserved = std::lround(g_d->geo.edgeGap + g_d->geo.plate);
        switch (s.edge) {
            case Edge::Bottom:
                reserved += mon.bottom - g.edgeCoord;
                break;
            case Edge::Left:
                reserved += g.edgeCoord - mon.left;
                break;
            case Edge::Right:
                reserved += mon.right - g.edgeCoord;
                break;
        }
        SetPropW(g_d->wnd, L"WindhawkMacDock.Edge",
                 (HANDLE)(INT_PTR)((int)s.edge + 1));
        SetPropW(g_d->wnd, L"WindhawkMacDock.Reserved",
                 (HANDLE)(INT_PTR)reserved);
    }

    if (dpiChanged && g_dwrite) {
        g_d->labelFormat.Reset();
        g_dwrite->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            13 * g.scale, L"", &g_d->labelFormat);
        if (g_d->labelFormat) {
            g_d->labelFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_d->labelFormat->SetParagraphAlignment(
                DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_d->labelFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
}

void ScreenToDock(POINT pt, float& u, float& v) {
    float x = pt.x + 0.5f, y = pt.y + 0.5f;
    switch (g_settings.edge) {
        case Edge::Bottom:
            u = x;
            v = g_d->geo.edgeCoord - y;
            break;
        case Edge::Left:
            u = y;
            v = x - g_d->geo.edgeCoord;
            break;
        case Edge::Right:
            u = y;
            v = g_d->geo.edgeCoord - x;
            break;
    }
}

POINT DockToScreen(float u, float v) {
    switch (g_settings.edge) {
        case Edge::Left:
            return {std::lround(g_d->geo.edgeCoord + v), std::lround(u)};
        case Edge::Right:
            return {std::lround(g_d->geo.edgeCoord - v), std::lround(u)};
        default:
            return {std::lround(u), std::lround(g_d->geo.edgeCoord - v)};
    }
}

D2D1_POINT_2F ToLocal(float u, float v) {
    float a = u - g_d->span.origin;
    switch (g_settings.edge) {
        case Edge::Left:
            return {v, a};
        case Edge::Right:
            return {g_d->geo.thickness - v, a};
        default:
            return {a, g_d->geo.thickness - v};
    }
}

D2D1_RECT_F LocalRect(float u0, float u1, float v0, float v1) {
    D2D1_POINT_2F p = ToLocal(u0, v0), q = ToLocal(u1, v1);
    return {std::min(p.x, q.x), std::min(p.y, q.y), std::max(p.x, q.x),
            std::max(p.y, q.y)};
}

RECT DockWindowRect() {
    int e = g_d->geo.edgeCoord, t = g_d->geo.thickness;
    int o = g_d->span.origin, l = g_d->span.length;
    switch (g_settings.edge) {
        case Edge::Left:
            return {e, o, e + t, o + l};
        case Edge::Right:
            return {e - t, o, e, o + l};
        default:
            return {o, e - t, o + l, e};
    }
}

////////////////////////////////////////////////////////////////////////////////
// Layout and animation

bool g_menuOpen;
bool g_trashFull;

float BaseWidth(const Item& item) {
    return item.kind == Kind::Separator ? g_d->geo.separator
                                        : g_d->geo.icon + g_d->geo.gap;
}

void ComputeBase() {
    float total = 0;
    for (auto& it : g_items) {
        it->slot = BaseWidth(*it) * Smooth(it->presence);
        total += it->slot;
    }
    float u = (g_d->geo.axisStart + g_d->geo.axisEnd) / 2.0f - total / 2;
    g_d->baseStart = u;
    g_d->baseTotal = total;
    for (auto& it : g_items) {
        it->baseCenter = u + it->slot / 2;
        u += it->slot;
    }
}

double BouncePeriod(Bounce bounce) {
    return bounce == Bounce::Launch ? kLaunchPeriod : kAttentionPeriod;
}

// Parabolic bounces, like a ball: rises slowing down and falls speeding up.
float BounceHeight(const Item& item, double now) {
    if (item.bounce == Bounce::None) {
        return 0;
    }
    double period = BouncePeriod(item.bounce);
    double t = now - item.bounceStart;
    double phase;
    if (item.bounce == Bounce::Attention) {
        double group = 3 * period + kAttentionPause;
        double tg = std::fmod(t, group);
        if (tg >= 3 * period) {
            return 0;
        }
        phase = std::fmod(tg, period) / period;
    } else {
        phase = std::fmod(t, period) / period;
    }
    float amplitude =
        g_d->geo.icon * (item.bounce == Bounce::Launch ? 0.5f : 0.8f);
    return amplitude * 4 * (float)(phase * (1 - phase));
}

void StartBounce(Item& item, Bounce bounce) {
    item.bounce = bounce;
    item.bounceStart = Now();
    item.bounceStop = false;
    item.launchWindows = item.windows.size();
}

// Asks to stop at the end of the current bounce.
void StopBounce(Item& item, double now) {
    if (item.bounce == Bounce::None || item.bounceStop) {
        return;
    }
    double period = BouncePeriod(item.bounce);
    double t = now - item.bounceStart;
    item.bounceStop = true;
    if (item.bounce == Bounce::Attention) {
        double tg = std::fmod(t, 3 * period + kAttentionPause);
        item.bounceStopAt =
            tg >= 3 * period ? t : t - std::fmod(tg, period) + period;
    } else {
        item.bounceStopAt = (std::floor(t / period) + 1) * period;
    }
}

void ComputeLayout(float cursorU, double now) {
    ComputeBase();

    float mag = Smooth(g_d->mag);
    float extra = g_d->geo.maxScale - 1;
    float total = 0;
    for (auto& it : g_items) {
        float scale = 1;
        if (mag > 0 && extra > 0) {
            float d = std::fabs(cursorU - it->baseCenter);
            if (d < g_d->geo.range) {
                scale += extra * mag * 0.5f *
                         (1 + std::cos(kPi * d / g_d->geo.range));
            }
        }
        it->scale = scale;
        it->mslot = it->slot * scale;
        total += it->mslot;
    }

    // The point under the cursor stays under the cursor after magnifying.
    float x = Clamp(cursorU - g_d->baseStart, 0.0f, g_d->baseTotal);
    float magnifiedX = 0, acc = 0;
    bool found = false;
    for (auto& it : g_items) {
        if (!found && it->slot > 0 && x <= acc + it->slot) {
            magnifiedX += (x - acc) / it->slot * it->mslot;
            found = true;
        } else if (!found) {
            magnifiedX += it->mslot;
        }
        acc += it->slot;
    }
    float start = mag > 0 ? g_d->baseStart + (x - magnifiedX) : g_d->baseStart;

    float endPad = std::max<float>(0, g_d->geo.pad - g_d->geo.gap / 2);
    float u0 = start - endPad, u1 = start + total + endPad;
    float fix = 0;
    if (u1 > g_d->geo.axisEnd) {
        fix = g_d->geo.axisEnd - u1;
    }
    if (u0 + fix < g_d->geo.axisStart) {
        fix = g_d->geo.axisStart - u0;
    }
    start += fix;

    float hideOffset = Smooth(g_d->hideProgress) *
                       (g_d->geo.edgeGap + g_d->geo.plate + 4 * g_d->geo.scale);
    // In whole pixels, so the drawing and the blur line up exactly.
    g_d->layout.plateU0 = std::round(u0 + fix);
    g_d->layout.plateU1 = std::round(u1 + fix);
    g_d->layout.plateV0 = std::round(g_d->geo.edgeGap - hideOffset);
    g_d->layout.magTopV = g_d->geo.edgeGap + g_d->geo.pad + g_d->geo.icon * (1 + extra * mag);
    g_d->layout.hovered = nullptr;

    float u = start;
    for (auto& it : g_items) {
        float slotCenter = u + it->mslot / 2;
        it->center = slotCenter + it->shift;
        it->size = g_d->geo.icon * it->scale * EaseOut(it->presence);
        it->bottomV = g_d->layout.plateV0 + g_d->geo.pad + BounceHeight(*it, now);
        if (g_d->hovering && it->kind != Kind::Separator && !it->removing &&
            it->presence > 0.5f &&
            std::fabs(cursorU - slotCenter) <= it->mslot / 2) {
            g_d->layout.hovered = it.get();
        }
        u += it->mslot;
    }
    g_d->layout.valid = true;
}

// The fullscreen Start (another mod) leaves the Dock strip free: while it's
// open on this monitor, the Dock shows up even with windows over it.
bool StartMenuOpenHere() {
    HWND start = FindWindowW(L"WindhawkFullscreenStart", nullptr);
    return start && IsWindowVisible(start) &&
           MonitorFromWindow(start, MONITOR_DEFAULTTONULL) == g_d->geo.monitor;
}

void UpdateHover(float cu, float cv, double now) {
    // Dragging, clicking or opening a menu only holds the Dock where it happens.
    bool interacting = g_d == g_active && (g_drag.active || g_menuOpen ||
                                           g_drop.active || g_press.item);

    bool inU = g_d->layout.valid && cu >= g_d->layout.plateU0 &&
               cu <= g_d->layout.plateU1;
    float plateTop = g_d->geo.edgeGap + g_d->geo.plate;
    float top = g_d->hovering ? std::max(g_d->layout.magTopV, plateTop) : plateTop;
    bool inZone = inU && cv >= -2 && cv <= top;
    bool autoHide = g_settings.autoHide == AutoHide::Always ||
                    (g_settings.autoHide == AutoHide::Smart && g_d->covered);

    if (g_d->fullscreen && !interacting) {
        g_d->wantShow = false;
        g_d->edgeSince = 0;
    } else if (!autoHide || interacting || StartMenuOpenHere()) {
        g_d->wantShow = true;
        g_d->lastInside = now;
    } else if (g_d->wantShow) {
        if (inZone) {
            g_d->lastInside = now;
        } else if (now - g_d->lastInside > 0.35) {
            g_d->wantShow = false;
        }
    } else {
        bool atEdge = cv > -2 && cv < 2 && cu >= g_d->baseStart - g_d->geo.icon &&
                      cu <= g_d->baseStart + g_d->baseTotal + g_d->geo.icon;
        if (!atEdge) {
            g_d->edgeSince = 0;
        } else if (!g_d->edgeSince) {
            g_d->edgeSince = now;
        }
        if (atEdge && now - g_d->edgeSince >= g_settings.autoHideDelay / 1000.0) {
            g_d->wantShow = true;
            g_d->lastInside = now;
        }
    }

    g_d->hovering = g_d->wantShow && g_d->hideProgress < 0.5f && inZone && !g_menuOpen;
}

// Magnification and hiding of the current Dock.
bool AnimateDock(float dt) {
    bool animating = false;

    float magTarget =
        g_d->hovering && g_settings.magnification && g_d->geo.maxScale > 1 ? 1 : 0;
    g_d->mag = Approach(g_d->mag, magTarget, dt / (magTarget > g_d->mag ? 0.14f : 0.24f));
    animating |= g_d->mag != magTarget;

    float hideTarget = g_d->wantShow ? 0 : 1;
    g_d->hideProgress = Approach(g_d->hideProgress, hideTarget, dt / 0.3f);
    animating |= g_d->hideProgress != hideTarget;
    return animating;
}

// Item animations, the same on all Docks. The name only shows up on the
// active Dock, so it follows the item under that Dock's mouse.
bool AnimateItems(float dt, double now) {
    bool animating = false;

    for (auto& it : g_items) {
        bool collapse = it->removing ||
                        (g_drag.item == it.get() && g_drag.active && g_drag.out);
        float target = collapse ? 0 : 1;
        it->presence = Approach(it->presence, target, dt / 0.3f);
        animating |= it->presence != target;

        float running = it->windows.empty() ? 0 : 1;
        it->running = Approach(it->running, running, dt / 0.2f);
        animating |= it->running != running;

        float label = g_settings.showLabels && g_d->layout.hovered == it.get() &&
                              !g_drag.active && !g_menuOpen &&
                              g_d->hideProgress == 0
                          ? 1
                          : 0;
        it->label = Approach(it->label, label, dt / 0.1f);
        animating |= it->label != label;

        if (it->shift != 0) {
            it->shift *= std::exp(-dt * 14);
            if (std::fabs(it->shift) < 0.3f) {
                it->shift = 0;
            }
            animating = true;
        }

        if (it->bounce != Bounce::None) {
            double t = now - it->bounceStart;
            if (it->bounce == Bounce::Launch &&
                (it->windows.size() > it->launchWindows || t > 8)) {
                StopBounce(*it, now);
            } else if (it->bounce == Bounce::Attention &&
                       t > 6 * (3 * kAttentionPeriod + kAttentionPause)) {
                StopBounce(*it, now);
            }
            if (it->bounceStop && t >= it->bounceStopAt) {
                it->bounce = Bounce::None;
            }
            animating = true;
        }
    }

    // During a menu the items must keep existing.
    if (!g_menuOpen) {
        for (auto i = g_items.begin(); i != g_items.end();) {
            Item* it = i->get();
            if (it->removing && it->presence <= 0 && g_drag.item != it &&
                g_press.item != it) {
                ClearReferences(it);
                i = g_items.erase(i);
            } else {
                ++i;
            }
        }
    }

    return animating || g_drag.active || g_drag.settling || g_drag.poof;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

struct Colors {
    D2D1_COLOR_F plate, border, separator, dot, labelBack, labelBorder,
        labelText, placeholder;
    bool dark;
} g_colors;

bool SystemUsesLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                 L"Personalize",
                 L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                 &size);
    return value != 0;
}

void UpdateColors() {
    bool light = g_settings.theme == Theme::Light ||
                 (g_settings.theme == Theme::Auto && SystemUsesLightTheme());
    float alpha = g_settings.opacity / 100.0f;
    auto& c = g_colors;
    if (light) {
        c.plate = D2D1::ColorF(0.97f, 0.97f, 0.98f, alpha);
        c.border = D2D1::ColorF(0, 0, 0, 0.10f);
        c.separator = D2D1::ColorF(0, 0, 0, 0.22f);
        c.dot = D2D1::ColorF(0, 0, 0, 0.62f);
        c.labelBack = D2D1::ColorF(0.95f, 0.95f, 0.96f, 0.96f);
        c.labelBorder = D2D1::ColorF(0, 0, 0, 0.12f);
        c.labelText = D2D1::ColorF(0.1f, 0.1f, 0.1f);
        c.placeholder = D2D1::ColorF(0, 0, 0, 0.15f);
        c.dark = false;
    } else {
        c.plate = D2D1::ColorF(0.15f, 0.15f, 0.16f, alpha);
        c.border = D2D1::ColorF(1, 1, 1, 0.16f);
        c.separator = D2D1::ColorF(1, 1, 1, 0.28f);
        c.dot = D2D1::ColorF(1, 1, 1, 0.75f);
        c.labelBack = D2D1::ColorF(0.17f, 0.17f, 0.18f, 0.94f);
        c.labelBorder = D2D1::ColorF(1, 1, 1, 0.12f);
        c.labelText = D2D1::ColorF(0.95f, 0.95f, 0.95f);
        c.placeholder = D2D1::ColorF(1, 1, 1, 0.15f);
        c.dark = true;
    }
}

struct Surface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ old = nullptr;
    int width = 0, height = 0;
};
Surface g_dockSurface, g_dragSurface;
bool g_dragShown;

void FreeSurface(Surface& s) {
    if (s.dc) {
        SelectObject(s.dc, s.old);
        DeleteObject(s.bitmap);
        DeleteDC(s.dc);
    }
    s = {};
}

bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc && s.width >= width && s.height >= height) {
        return true;
    }
    width = std::max(width, s.width);
    height = std::max(height, s.height);
    FreeSurface(s);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.dc = CreateCompatibleDC(nullptr);
    s.bitmap = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap) {
        DeleteDC(s.dc);
        s = {};
        return false;
    }
    s.old = SelectObject(s.dc, s.bitmap);
    s.width = width;
    s.height = height;
    return true;
}

bool EnsureTarget() {
    if (g_target) {
        return true;
    }
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &g_target))) {
        return false;
    }
    g_target.As(&g_context);
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_target->CreateSolidColorBrush(D2D1::ColorF(0, 0), &g_brush);
    g_targetGeneration++;
    return true;
}

void ResetTarget() {
    g_brush.Reset();
    g_context.Reset();
    g_target.Reset();
}

void EnsureIcon(Item& item) {
    if (item.iconTried) {
        return;
    }
    item.iconTried = true;
    int size = g_iconPixels;
    switch (item.kind) {
        case Kind::App: {
            if (!item.target.empty()) {
                item.icon = ImageFromParsingName(item.target, size);
            }
            if (!item.icon && !item.pathRaw.empty() && !item.pseudo) {
                item.icon = ImageFromParsingName(item.pathRaw, size);
            }
            HWND window =
                item.windows.empty() ? item.iconWindow : item.windows[0];
            if (!item.icon && window && IsWindow(window)) {
                if (HICON icon = GetWindowIcon(window)) {
                    item.icon = ImageFromIcon(icon, std::min(size, 64));
                }
            }
            break;
        }
        case Kind::Downloads: {
            ComPtr<IShellItem> folder;
            if (SUCCEEDED(SHGetKnownFolderItem(kFolderDownloads,
                                               KF_FLAG_DEFAULT, nullptr,
                                               IID_PPV_ARGS(&folder)))) {
                item.icon = ImageFromShellItem(folder.Get(), size);
            }
            break;
        }
        case Kind::Trash:
            item.icon = LoadTrashImage(g_trashFull, size);
            break;
        default:
            break;
    }
}

void ResetIcons() {
    for (auto& it : g_items) {
        it->icon.reset();
        it->iconTried = false;
    }
}

void DrawImage(Image& image, const D2D1_RECT_F& rect, float opacity) {
    ID2D1Bitmap* bitmap = GetBitmap(image);
    if (!bitmap) {
        return;
    }
    if (g_context) {
        g_context->DrawBitmap(bitmap, &rect, opacity,
                              D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC,
                              nullptr, nullptr);
    } else {
        g_target->DrawBitmap(bitmap, rect, opacity,
                             D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
}

// Darkens only the icon's pixels, like macOS does on click.
void DarkenImage(Image& image, const D2D1_RECT_F& rect, float amount) {
    ID2D1Bitmap* bitmap = GetBitmap(image);
    if (!bitmap) {
        return;
    }
    g_brush->SetColor(D2D1::ColorF(0, 0, 0, amount));
    g_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    g_target->FillOpacityMask(bitmap, g_brush.Get(),
                              D2D1_OPACITY_MASK_CONTENT_GRAPHICS, &rect,
                              nullptr);
    g_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void DrawStartIcon(const D2D1_RECT_F& rect, float opacity, float darken) {
    float inset = (rect.right - rect.left) * 0.07f;
    D2D1_RECT_F tile{rect.left + inset, rect.top + inset, rect.right - inset,
                     rect.bottom - inset};
    float side = tile.right - tile.left;
    float radius = side * 0.23f;
    auto rounded = D2D1::RoundedRect(tile, radius, radius);

    D2D1_GRADIENT_STOP stops[] = {
        {0, D2D1::ColorF(0.36f, 0.78f, 1.0f, opacity)},
        {1, D2D1::ColorF(0.0f, 0.38f, 0.88f, opacity)},
    };
    ComPtr<ID2D1GradientStopCollection> collection;
    ComPtr<ID2D1LinearGradientBrush> gradient;
    if (SUCCEEDED(g_target->CreateGradientStopCollection(stops, 2,
                                                         &collection)) &&
        SUCCEEDED(g_target->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties({tile.left, tile.top},
                                                {tile.left, tile.bottom}),
            collection.Get(), &gradient))) {
        g_target->FillRoundedRectangle(rounded, gradient.Get());
    }

    float logo = side * 0.44f;
    float gap = logo * 0.07f;
    float cell = (logo - gap) / 2;
    float lx = (tile.left + tile.right) / 2 - logo / 2;
    float ly = (tile.top + tile.bottom) / 2 - logo / 2;
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, opacity));
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float x = lx + i * (cell + gap), y = ly + j * (cell + gap);
            g_target->FillRectangle({x, y, x + cell, y + cell}, g_brush.Get());
        }
    }

    if (darken > 0) {
        g_brush->SetColor(D2D1::ColorF(0, 0, 0, darken * opacity));
        g_target->FillRoundedRectangle(rounded, g_brush.Get());
    }
}

float MeasureText(const std::wstring& text) {
    if (!g_d->labelFormat || text.empty()) {
        return 0;
    }
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                          g_d->labelFormat.Get(), 10000, 1000,
                                          &layout))) {
        return 0;
    }
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    return metrics.widthIncludingTrailingWhitespace;
}

// Tooltip with the name, starting at v (already above the icon).
void DrawLabel(const std::wstring& text,
               float textWidth,
               float u,
               float v,
               float alpha,
               float spanStart,
               float spanEnd) {
    if (!g_d->labelFormat || text.empty() || alpha <= 0) {
        return;
    }
    float s = g_d->geo.scale;
    bool bottom = g_settings.edge == Edge::Bottom;
    float maxWidth = bottom ? 420 * s : g_d->geo.thickness - v - 2 * s;
    float width = std::min<float>(textWidth + 20 * s, maxWidth);
    float height = g_d->geo.labelHeight;
    float along = bottom ? width : height;
    float across = bottom ? height : width;
    float u0 = Clamp(u - along / 2, spanStart + 4 * s,
                     std::max(spanStart + 4 * s, spanEnd - 4 * s - along));
    D2D1_RECT_F rect = LocalRect(u0, u0 + along, v, v + across);
    float radius = 6 * s;
    auto rounded = D2D1::RoundedRect(rect, radius, radius);

    D2D1_COLOR_F back = g_colors.labelBack;
    back.a *= alpha;
    g_brush->SetColor(back);
    g_target->FillRoundedRectangle(rounded, g_brush.Get());
    D2D1_COLOR_F border = g_colors.labelBorder;
    border.a *= alpha;
    g_brush->SetColor(border);
    g_target->DrawRoundedRectangle(rounded, g_brush.Get(), 1);

    D2D1_COLOR_F color = g_colors.labelText;
    color.a *= alpha;
    g_brush->SetColor(color);
    D2D1_RECT_F textRect{rect.left + 10 * s, rect.top, rect.right - 10 * s,
                         rect.bottom};
    g_target->DrawText(text.c_str(), (UINT32)text.size(), g_d->labelFormat.Get(),
                       textRect, g_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

float ItemLabelWidth(Item& item) {
    if (item.labelDpi != g_d->geo.dpi) {
        item.labelDpi = g_d->geo.dpi;
        item.labelWidth = MeasureText(item.name);
    }
    return item.labelWidth;
}

void DrawItem(Item& item) {
    if (item.mslot < 0.5f) {
        return;
    }
    const auto& g = g_d->geo;

    if (item.kind == Kind::Separator) {
        float inset = g.pad * 0.7f;
        D2D1_POINT_2F a = ToLocal(item.center, g_d->layout.plateV0 + inset);
        D2D1_POINT_2F b =
            ToLocal(item.center, g_d->layout.plateV0 + g.plate - inset);
        D2D1_COLOR_F color = g_colors.separator;
        color.a *= Smooth(item.presence);
        g_brush->SetColor(color);
        g_target->DrawLine(a, b, g_brush.Get(), std::max<float>(1, std::round(g.scale)));
        return;
    }

    bool dragged = g_drag.item == &item && (g_drag.active || g_drag.settling);
    if (!item.hidden && !dragged && item.size > 0.5f) {
        D2D1_RECT_F rect =
            LocalRect(item.center - item.size / 2, item.center + item.size / 2,
                      item.bottomV, item.bottomV + item.size);
        float opacity = Smooth(item.presence);
        float darken = 0;
        if ((g_press.item == &item && g_d->layout.hovered == &item &&
             !g_drag.active) ||
            (g_drop.active && g_drop.target == &item)) {
            darken = 0.35f;
        }

        if (item.kind == Kind::Start) {
            DrawStartIcon(rect, opacity, darken);
        } else {
            EnsureIcon(item);
            if (item.icon) {
                DrawImage(*item.icon, rect, opacity);
                if (darken > 0) {
                    DarkenImage(*item.icon, rect, darken * opacity);
                }
            } else {
                float inset = item.size * 0.08f;
                D2D1_RECT_F box{rect.left + inset, rect.top + inset,
                                rect.right - inset, rect.bottom - inset};
                float r = item.size * 0.2f;
                D2D1_COLOR_F color = g_colors.placeholder;
                color.a *= opacity;
                g_brush->SetColor(color);
                g_target->FillRoundedRectangle(D2D1::RoundedRect(box, r, r),
                                               g_brush.Get());
            }
        }
    }

    if (g_settings.showIndicators && item.kind == Kind::App &&
        item.running > 0.01f) {
        float radius = std::max<float>(1.5f * g.scale, g.icon * 0.045f);
        D2D1_POINT_2F center =
            ToLocal(item.center, g_d->layout.plateV0 + g.pad * 0.45f);
        D2D1_COLOR_F color = g_colors.dot;
        color.a *= item.running * Smooth(item.presence);
        g_brush->SetColor(color);
        g_target->FillEllipse(D2D1::Ellipse(center, radius, radius),
                              g_brush.Get());
    }
}

void DrawDockContents() {
    const auto& g = g_d->geo;
    D2D1_RECT_F plate = LocalRect(g_d->layout.plateU0, g_d->layout.plateU1,
                                  g_d->layout.plateV0, g_d->layout.plateV0 + g.plate);
    auto rounded = D2D1::RoundedRect(plate, g.radius, g.radius);
    g_brush->SetColor(g_colors.plate);
    g_target->FillRoundedRectangle(rounded, g_brush.Get());
    float stroke = std::max<float>(1, std::round(g.scale));
    D2D1_RECT_F inner{plate.left + stroke / 2, plate.top + stroke / 2,
                      plate.right - stroke / 2, plate.bottom - stroke / 2};
    g_brush->SetColor(g_colors.border);
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(inner, g.radius - stroke / 2, g.radius - stroke / 2),
        g_brush.Get(), stroke);

    for (auto& it : g_items) {
        DrawItem(*it);
    }

    float spanStart = (float)g_d->span.origin;
    float spanEnd = (float)(g_d->span.origin + g_d->span.length);
    for (auto& it : g_items) {
        if (g_d == g_active && it->label > 0 &&
            it->kind != Kind::Separator) {
            DrawLabel(it->name, ItemLabelWidth(*it), it->center,
                      it->bottomV + it->size + g.labelGap, Smooth(it->label),
                      spanStart, spanEnd);
        }
    }
}

void UpdateSpan() {
    float margin = std::max<float>(g_d->geo.icon * 2, 160 * g_d->geo.scale);
    int start = (int)std::floor(g_d->layout.plateU0 - margin);
    int end = (int)std::ceil(g_d->layout.plateU1 + margin);
    start = std::max(start, g_d->geo.axisStart);
    end = std::min(end, g_d->geo.axisEnd);
    if (end <= start) {
        end = start + 1;
    }
    g_d->span = {start, end - start};
}

void RenderDock() {
    if (!EnsureTarget()) {
        return;
    }
    UpdateSpan();
    RECT rc = DockWindowRect();
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (!EnsureSurface(g_dockSurface, width, height)) {
        return;
    }
    RECT bind{0, 0, width, height};
    if (FAILED(g_target->BindDC(g_dockSurface.dc, &bind))) {
        return;
    }
    g_target->BeginDraw();
    g_target->Clear(D2D1::ColorF(0, 0));
    if (g_d->hideProgress < 0.999f) {
        DrawDockContents();
    }
    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
        ResetTarget();
        return;
    }

    POINT dst{rc.left, rc.top}, src{0, 0};
    SIZE size{width, height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_d->wnd, nullptr, &dst, &size, g_dockSurface.dc, &src,
                        0, &blend, ULW_ALPHA);
}

////////////////////////////////////////////////////////////////////////////////
// Blurred background
//
// A regular window with the DWM effect, the exact size of the Dock
// background, sits right below the layered window where everything is drawn.

struct AccentPolicy {
    INT accentState;
    INT accentFlags;
    INT gradientColor;
    INT animationId;
};

struct WindowCompositionAttribData {
    DWORD attrib;
    PVOID data;
    SIZE_T size;
};

constexpr DWORD WCA_ACCENT_POLICY = 19;
constexpr INT ACCENT_DISABLED = 0;
constexpr INT ACCENT_ENABLE_BLURBEHIND = 3;
constexpr INT ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;


void SetAccent(HWND hwnd, INT state, DWORD abgr) {
    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);
    static auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWindowCompositionAttribute) {
        return;
    }
    AccentPolicy policy{state, state ? 2 : 0, (INT)abgr, 0};
    WindowCompositionAttribData data{WCA_ACCENT_POLICY, &policy,
                                     sizeof(policy)};
    setWindowCompositionAttribute(hwnd, &data);
}

void ApplyPlateBackdrop() {
    HWND hwnd = g_d->plate;

    // Makes DWM respect the window's alpha channel, without blur.
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    blur.fEnable = TRUE;
    blur.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
    DwmEnableBlurBehindWindow(hwnd, &blur);
    DeleteObject(blur.hRgnBlur);

    BOOL dark = g_colors.dark;
    DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));
    UINT corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));
    COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(hwnd, kDwmBorderColor, &border, sizeof(border));

    D2D1_COLOR_F c = g_colors.plate;
    DWORD abgr = (1u << 24) | ((DWORD)(c.b * 255) << 16) |
                 ((DWORD)(c.g * 255) << 8) | (DWORD)(c.r * 255);
    switch (g_settings.backdrop) {
        case Backdrop::Blur:
            SetAccent(hwnd, ACCENT_ENABLE_BLURBEHIND, 0);
            break;
        case Backdrop::Acrylic:
            SetAccent(hwnd, ACCENT_ENABLE_ACRYLICBLURBEHIND, abgr);
            break;
        case Backdrop::Solid:
            SetAccent(hwnd, ACCENT_DISABLED, 0);
            break;
    }
    g_d->plateRect = {};
}

void UpdatePlateWindow() {
    bool show = g_settings.backdrop != Backdrop::Solid &&
                g_d->hideProgress < 0.999f && g_d->layout.valid;

    // DWM ignores the window region (SetWindowRgn) when drawing the blur,
    // so the window has the exact size of the background and the corners
    // come from DWM's own rounding. The part that already slid past the
    // edge is cut, otherwise it would show up on the monitor below.
    POINT a = DockToScreen(g_d->layout.plateU0, std::max(0.0f, g_d->layout.plateV0));
    POINT b = DockToScreen(g_d->layout.plateU1, g_d->layout.plateV0 + g_d->geo.plate);
    RECT rc{std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x),
            std::max(a.y, b.y)};
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    // Too short, DWM's rounded corners don't fit.
    show = show && width > 0 && height >= 2 * g_d->geo.radius;
    if (!show) {
        if (IsWindowVisible(g_d->plate)) {
            ShowWindow(g_d->plate, SW_HIDE);
        }
        g_d->plateRect = {};
        return;
    }
    if (EqualRect(&rc, &g_d->plateRect) && IsWindowVisible(g_d->plate)) {
        return;
    }
    SetWindowPos(g_d->plate, nullptr, rc.left, rc.top, width, height,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    g_d->plateRect = rc;
}

////////////////////////////////////////////////////////////////////////////////
// Dragged icon and the removal cloud

POINT g_lastCursor{-1, -1};

void DrawPoof(D2D1_POINT_2F center, float t) {
    float e = EaseOut(t);
    float base = g_d->geo.icon * 0.9f;
    float alpha = std::pow(1 - t, 1.5f);
    D2D1_COLOR_F light = g_colors.dark ? D2D1::ColorF(0.92f, 0.92f, 0.94f)
                                       : D2D1::ColorF(0.85f, 0.85f, 0.88f);
    for (int k = 0; k < 7; k++) {
        float angle = k * 2 * kPi / 7 + 0.3f + t * 0.6f;
        float dist = base * (0.12f + 0.38f * e);
        float radius = base * (0.2f + 0.1f * e) * (1 - 0.35f * t);
        D2D1_POINT_2F p{center.x + std::cos(angle) * dist,
                        center.y + std::sin(angle) * dist};
        g_brush->SetColor(D2D1::ColorF(light.r, light.g, light.b, 0.9f * alpha));
        g_target->FillEllipse(D2D1::Ellipse(p, radius, radius), g_brush.Get());
    }
    float radius = base * 0.3f * (1 + 0.4f * e) * (1 - 0.5f * t);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.8f * alpha));
    g_target->FillEllipse(D2D1::Ellipse(center, radius, radius), g_brush.Get());
}

void RenderDrag(double now) {
    bool visible = g_drag.active || g_drag.settling || g_drag.poof;
    if (!visible) {
        if (g_dragShown) {
            ShowWindow(g_dragWnd, SW_HIDE);
            g_dragShown = false;
        }
        return;
    }
    if (!EnsureTarget()) {
        return;
    }

    const auto& g = g_d->geo;
    float iconArea = g.icon * std::max<float>(g.maxScale, 1) * 1.1f;
    int width = (int)std::ceil(std::max<float>(g.icon * 3, 220 * g.scale));
    int height = (int)std::ceil(iconArea + g.labelHeight + g.labelGap + 8 * g.scale);

    Item* item = g_drag.item;
    POINT center{};
    float size = g_drag.size;
    bool drawIcon = item && (g_drag.active || g_drag.settling);
    if (g_drag.active) {
        center = {g_lastCursor.x - g_drag.grab.x, g_lastCursor.y - g_drag.grab.y};
    } else if (g_drag.settling && item) {
        float t = (float)((now - g_drag.settleStart) / 0.2);
        float e = EaseOut(t);
        POINT to = DockToScreen(item->center, item->bottomV + item->size / 2);
        center = {std::lround(g_drag.settleFrom.x + (to.x - g_drag.settleFrom.x) * e),
                  std::lround(g_drag.settleFrom.y + (to.y - g_drag.settleFrom.y) * e)};
        size = g_drag.size + (item->size - g_drag.size) * e;
        if (t >= 1) {
            g_drag.settling = false;
            g_drag.item = nullptr;
            drawIcon = false;
        }
    } else {
        center = g_drag.poofAt;
    }

    int left = center.x - width / 2;
    int top = center.y - (height - (int)(iconArea / 2));
    float cx = width / 2.0f, cy = height - iconArea / 2;

    if (!EnsureSurface(g_dragSurface, width, height)) {
        return;
    }
    RECT bind{0, 0, width, height};
    if (FAILED(g_target->BindDC(g_dragSurface.dc, &bind))) {
        return;
    }
    g_target->BeginDraw();
    g_target->Clear(D2D1::ColorF(0, 0));

    if (drawIcon && item) {
        EnsureIcon(*item);
        D2D1_RECT_F rect{cx - size / 2, cy - size / 2, cx + size / 2,
                         cy + size / 2};
        if (item->icon) {
            DrawImage(*item->icon, rect, 1);
        }
        if (g_drag.active && g_drag.out) {
            // The "Remove" tooltip above the icon, always horizontal.
            static const std::wstring kRemove = L"Remove";
            float textWidth = MeasureText(kRemove);
            float w = textWidth + 20 * g.scale, h = g.labelHeight;
            D2D1_RECT_F label{cx - w / 2, rect.top - g.labelGap - h, cx + w / 2,
                              rect.top - g.labelGap};
            float radius = 6 * g.scale;
            g_brush->SetColor(g_colors.labelBack);
            g_target->FillRoundedRectangle(D2D1::RoundedRect(label, radius, radius),
                                           g_brush.Get());
            g_brush->SetColor(g_colors.labelText);
            if (g_d->labelFormat) {
                g_target->DrawText(kRemove.c_str(), (UINT32)kRemove.size(),
                                   g_d->labelFormat.Get(), label, g_brush.Get());
            }
        }
    }

    if (g_drag.poof) {
        float t = (float)((now - g_drag.poofStart) / 0.45);
        if (t >= 1) {
            g_drag.poof = false;
        } else {
            DrawPoof({cx, cy}, t);
        }
    }

    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
        ResetTarget();
        return;
    }

    POINT dst{left, top}, src{0, 0};
    SIZE sz{width, height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_dragWnd, nullptr, &dst, &sz, g_dragSurface.dc, &src,
                        0, &blend, ULW_ALPHA);
    if (!g_dragShown) {
        // Above the Docks, which may have been created later.
        SetWindowPos(g_dragWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_SHOWWINDOW);
        g_dragShown = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Frame loop

double g_lastTick;
bool g_fastTimer;

void UpdateDrag(float cu, float cv);

// The Dock of the monitor of a screen point.
Dock* DockAt(POINT pt) {
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    for (auto& d : g_docks) {
        if (d->monitor == monitor) {
            return d.get();
        }
    }
    return nullptr;
}

void Tick() {
    if (g_docks.empty() || !g_active) {
        return;
    }
    double now = Now();
    float dt = (float)Clamp(now - g_lastTick, 0.0, 0.05);
    g_lastTick = now;

    POINT pt = g_lastCursor;
    if (g_drop.active) {
        pt = g_drop.point;
    } else {
        GetCursorPos(&pt);
    }
    bool moved = pt.x != g_lastCursor.x || pt.y != g_lastCursor.y;
    g_lastCursor = pt;

    // During a click, drag or menu, the active Dock doesn't change. When
    // dropping files it follows the drag point.
    if (!g_drag.active && !g_menuOpen && !g_press.item) {
        if (Dock* d = DockAt(pt)) {
            g_active = d;
        }
    }
    g_d = g_active;
    bool itemsAnimating = AnimateItems(dt, now);
    bool fast = itemsAnimating || g_press.item;

    // The others first and the active one last: the items keep its layout,
    // which clicks, menus and dragging use.
    std::vector<Dock*> order;
    for (auto& d : g_docks) {
        if (d.get() != g_active) {
            order.push_back(d.get());
        }
    }
    order.push_back(g_active);

    for (Dock* d : order) {
        g_d = d;
        float cu = d->lastCursorU, cv = 1e9f;  // Mouse far from the others.
        if (d == g_active) {
            ScreenToDock(pt, cu, cv);
        }
        UpdateHover(cu, cv, now);
        if (d == g_active && g_drag.active) {
            UpdateDrag(cu, cv);
        }
        bool animating = AnimateDock(dt) || itemsAnimating;
        // On leaving, the magnification fades from the last point under the mouse.
        if (d->hovering) {
            d->lastCursorU = cu;
        }
        ComputeLayout(d->lastCursorU, now);

        // The frame where the animation ends is drawn too; without it, the
        // background stayed on screen after the Dock hid.
        if (animating || d->wasAnimating || d->dirty ||
            (moved && (d->hovering || d->mag > 0))) {
            RenderDock();
            UpdatePlateWindow();
            d->dirty = false;
        }
        d->wasAnimating = animating;
        fast = fast || animating || d->hovering;
    }
    RenderDrag(now);

    if (fast != g_fastTimer) {
        g_fastTimer = fast;
        SetTimer(g_controller, kTimerTick, fast ? 8 : 50, nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Actions

void ActivateWindow(HWND hwnd) {
    // Elevated apps (Windhawk, for example) ignore ShowWindow coming from
    // explorer; SwitchToThisWindow restores any minimized window.
    if (IsIconic(hwnd)) {
        SwitchToThisWindow(hwnd, TRUE);
        return;
    }
    if (!SetForegroundWindow(hwnd)) {
        SwitchToThisWindow(hwnd, TRUE);
    }
}

void Launch(Item& item, const std::wstring& params = {}) {
    if (item.target.empty()) {
        return;
    }
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_ASYNCOK;
    sei.lpFile = item.target.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (g_settings.launchBounce) {
        StartBounce(item, Bounce::Launch);
    }
    if (!ShellExecuteExW(&sei)) {
        item.bounce = Bounce::None;
    }
}

bool Contains(const std::vector<HWND>& windows, HWND hwnd) {
    return hwnd && std::find(windows.begin(), windows.end(), hwnd) !=
                       windows.end();
}

void ClickApp(Item& item) {
    RefreshWindows();
    if (item.windows.empty()) {
        Launch(item);
        return;
    }

    HWND foreground = GetForegroundWindow();
    HWND root = foreground ? GetAncestor(foreground, GA_ROOTOWNER) : nullptr;
    HWND active = Contains(item.windows, foreground) ? foreground
                  : Contains(item.windows, root)     ? root
                                                     : nullptr;
    // When the last window is minimized, Windows leaves it as the focused
    // window; it doesn't count as active, otherwise the click wouldn't restore it.
    if (active && IsIconic(active)) {
        active = nullptr;
    }
    std::vector<HWND> windows = item.windows;

    // The window minimized last comes back with the click, even if another
    // window of the app got the focus when it was minimized.
    if (Contains(windows, item.lastMinimized) && IsIconic(item.lastMinimized)) {
        ActivateWindow(item.lastMinimized);
        return;
    }

    if (!active) {
        // Like in macOS, all the app's windows come to the front, and the most
        // recent one gets the focus. If they're all minimized, the most recent
        // one is restored.
        HWND target = nullptr;
        for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
            if (!IsIconic(*it)) {
                SetWindowPos(*it, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                 SWP_ASYNCWINDOWPOS);
                target = *it;
            }
        }
        ActivateWindow(target ? target : windows[0]);
        return;
    }

    switch (g_settings.activeClick) {
        case ActiveClick::Activate:
            break;
        case ActiveClick::Minimize:
            item.lastMinimized = active;
            ShowWindowAsync(active, SW_MINIMIZE);
            break;
        case ActiveClick::Cycle:
            if (windows.size() > 1) {
                ActivateWindow(windows.back());
            }
            break;
    }
}

UINT TrackMenu(HMENU menu, Item* item) {
    POINT pt;
    GetCursorPos(&pt);
    UINT flags = TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY;
    float v = item ? item->bottomV + item->size + 8 * g_d->geo.scale
                   : g_d->layout.plateV0 + g_d->geo.plate + 8 * g_d->geo.scale;
    POINT anchor = DockToScreen(item ? item->center : 0, v);
    switch (g_settings.edge) {
        case Edge::Bottom:
            flags |= TPM_BOTTOMALIGN | (item ? TPM_CENTERALIGN : TPM_LEFTALIGN);
            pt.y = anchor.y;
            if (item) {
                pt.x = anchor.x;
            }
            break;
        case Edge::Left:
            flags |= TPM_LEFTALIGN | (item ? TPM_VCENTERALIGN : TPM_TOPALIGN);
            pt.x = anchor.x;
            if (item) {
                pt.y = anchor.y;
            }
            break;
        case Edge::Right:
            flags |= TPM_RIGHTALIGN | (item ? TPM_VCENTERALIGN : TPM_TOPALIGN);
            pt.x = anchor.x;
            if (item) {
                pt.y = anchor.y;
            }
            break;
    }

    HWND previous = GetForegroundWindow();
    g_menuOpen = true;
    MarkDirty();
    SetForegroundWindow(g_d->wnd);
    UINT cmd = TrackPopupMenuEx(menu, flags, pt.x, pt.y, g_d->wnd, nullptr);
    PostMessageW(g_d->wnd, WM_NULL, 0, 0);
    g_menuOpen = false;
    if (GetForegroundWindow() == g_d->wnd && previous && IsWindow(previous)) {
        SetForegroundWindow(previous);
    }
    return cmd;
}

constexpr UINT kCmdWindowBase = 1;
constexpr UINT kCmdOpen = 1000;
constexpr UINT kCmdPin = 1001;
constexpr UINT kCmdReveal = 1002;
constexpr UINT kCmdClose = 1003;
constexpr UINT kCmdFileBase = 2000;
constexpr UINT kCmdOpenFolder = 2999;
constexpr UINT kCmdEmptyTrash = 3000;
constexpr UINT kCmdOpenTrash = 3001;
constexpr UINT kCmdShowDesktop = 4000;
constexpr UINT kCmdTaskManager = 4001;
constexpr UINT kCmdTaskbarSettings = 4002;

void TogglePin(Item& item) {
    int index = PinIndex(item.target);
    if (index >= 0) {
        g_pins.erase(g_pins.begin() + index);
    } else {
        g_pins.push_back(item.target);
    }
    SyncPins();
    SavePins();
}

void ShowAppMenu(Item& item) {
    RefreshWindows();
    std::vector<HWND> windows = item.windows;
    if (windows.size() > 30) {
        windows.resize(30);
    }
    HWND foreground = GetForegroundWindow();
    HWND root = foreground ? GetAncestor(foreground, GA_ROOTOWNER) : nullptr;

    HMENU menu = CreatePopupMenu();
    for (size_t i = 0; i < windows.size(); i++) {
        WCHAR title[256] = L"";
        InternalGetWindowText(windows[i], title, ARRAYSIZE(title));
        std::wstring text = MenuText(*title ? title : item.name.c_str());
        if (IsIconic(windows[i])) {
            text += L" (minimizada)";
        }
        UINT flags = MF_STRING;
        if (windows[i] == foreground || windows[i] == root) {
            flags |= MF_CHECKED;
        }
        AppendMenuW(menu, flags, kCmdWindowBase + i, text.c_str());
    }
    if (!windows.empty()) {
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }
    UINT canTarget = item.target.empty() ? MF_GRAYED : 0;
    AppendMenuW(menu, MF_STRING | canTarget, kCmdOpen,
                windows.empty() ? L"Open" : L"New window");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | canTarget | (item.pinned ? MF_CHECKED : 0),
                kCmdPin, L"Keep in Dock");
    if (!item.pathRaw.empty() && !item.pseudo) {
        AppendMenuW(menu, MF_STRING, kCmdReveal, L"Show in Explorer");
    }
    if (!windows.empty()) {
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, kCmdClose,
                    windows.size() > 1 ? L"Close all windows"
                                       : L"Close window");
    }

    UINT cmd = TrackMenu(menu, &item);
    DestroyMenu(menu);

    if (cmd >= kCmdWindowBase && cmd < kCmdWindowBase + windows.size()) {
        ActivateWindow(windows[cmd - kCmdWindowBase]);
    } else if (cmd == kCmdOpen) {
        Launch(item);
    } else if (cmd == kCmdPin) {
        TogglePin(item);
    } else if (cmd == kCmdReveal) {
        std::wstring params = L"/select,\"" + item.pathRaw + L"\"";
        Run(L"explorer.exe", params.c_str());
    } else if (cmd == kCmdClose) {
        for (HWND hwnd : windows) {
            PostMessageW(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        }
    }
}

// The Downloads folder works as a stack: shows the most recent items.
void ShowDownloadsMenu(Item& item) {
    std::wstring dir = KnownFolderPath(kFolderDownloads);
    struct Entry {
        std::wstring name;
        FILETIME time;
    };
    std::vector<Entry> entries;
    WIN32_FIND_DATAW fd;
    HANDLE find = dir.empty() ? INVALID_HANDLE_VALUE
                              : FindFirstFileExW((dir + L"\\*").c_str(),
                                                 FindExInfoBasic, &fd,
                                                 FindExSearchNameMatch,
                                                 nullptr, 0);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..") ||
                (fd.dwFileAttributes &
                 (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
                continue;
            }
            entries.push_back({fd.cFileName, fd.ftCreationTime});
        } while (FindNextFileW(find, &fd));
        FindClose(find);
    }
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return CompareFileTime(&a.time, &b.time) > 0;
    });
    if (entries.size() > 15) {
        entries.resize(15);
    }

    HMENU menu = CreatePopupMenu();
    std::vector<HBITMAP> bitmaps;
    int iconSize = std::lround(16 * g_d->geo.scale);
    for (size_t i = 0; i < entries.size(); i++) {
        std::wstring path = dir + L"\\" + entries[i].name;
        std::wstring text = MenuText(entries[i].name);
        MENUITEMINFOW mii{sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_STRING;
        mii.wID = kCmdFileBase + (UINT)i;
        mii.dwTypeData = text.data();
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_SMALLICON) &&
            sfi.hIcon) {
            if (HBITMAP bitmap = IconToMenuBitmap(sfi.hIcon, iconSize)) {
                bitmaps.push_back(bitmap);
                mii.fMask |= MIIM_BITMAP;
                mii.hbmpItem = bitmap;
            }
            DestroyIcon(sfi.hIcon);
        }
        InsertMenuItemW(menu, (UINT)i, TRUE, &mii);
    }
    if (entries.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"No files");
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kCmdOpenFolder, L"Open in Explorer");

    UINT cmd = TrackMenu(menu, &item);
    DestroyMenu(menu);
    for (HBITMAP bitmap : bitmaps) {
        DeleteObject(bitmap);
    }

    if (cmd >= kCmdFileBase && cmd < kCmdFileBase + entries.size()) {
        std::wstring path = dir + L"\\" + entries[cmd - kCmdFileBase].name;
        ShellExecuteW(nullptr, nullptr, path.c_str(), nullptr, dir.c_str(),
                      SW_SHOWNORMAL);
    } else if (cmd == kCmdOpenFolder) {
        Run(dir.c_str());
    }
}

bool QueryTrashFull() {
    SHQUERYRBINFO info{sizeof(info)};
    return SUCCEEDED(SHQueryRecycleBinW(nullptr, &info)) && info.i64NumItems > 0;
}

void UpdateTrashState() {
    bool full = QueryTrashFull();
    if (full == g_trashFull) {
        return;
    }
    g_trashFull = full;
    for (auto& it : g_items) {
        if (it->kind == Kind::Trash) {
            it->icon.reset();
            it->iconTried = false;
        }
    }
    MarkDirty();
}

void ShowTrashMenu(Item& item) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, kCmdOpenTrash, L"Open");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_trashFull ? 0 : MF_GRAYED),
                kCmdEmptyTrash, L"Empty Recycle Bin");
    UINT cmd = TrackMenu(menu, &item);
    DestroyMenu(menu);
    if (cmd == kCmdOpenTrash) {
        Run(L"shell:RecycleBinFolder");
    } else if (cmd == kCmdEmptyTrash) {
        SHEmptyRecycleBinW(nullptr, nullptr, 0);
        UpdateTrashState();
    }
}

void ShowDockMenu() {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, kCmdShowDesktop, L"Show desktop");
    AppendMenuW(menu, MF_STRING, kCmdTaskManager, L"Task Manager");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kCmdTaskbarSettings,
                L"Taskbar settings");
    UINT cmd = TrackMenu(menu, nullptr);
    DestroyMenu(menu);
    if (cmd == kCmdShowDesktop) {
        SendKeys({VK_LWIN, 'D'});
    } else if (cmd == kCmdTaskManager) {
        Run(L"taskmgr.exe");
    } else if (cmd == kCmdTaskbarSettings) {
        Run(L"ms-settings:taskbar");
    }
}

void OnItemClick(Item& item) {
    switch (item.kind) {
        case Kind::Start:
            OpenStartMenu();
            break;
        case Kind::App:
            ClickApp(item);
            break;
        case Kind::Downloads:
            ShowDownloadsMenu(item);
            break;
        case Kind::Trash:
            Run(L"shell:RecycleBinFolder");
            break;
        default:
            break;
    }
}

void OnItemRightClick(Item* item) {
    if (!item || item->kind == Kind::Separator) {
        ShowDockMenu();
        return;
    }
    switch (item->kind) {
        case Kind::Start:
            SendKeys({VK_LWIN, 'X'});
            break;
        case Kind::App:
            ShowAppMenu(*item);
            break;
        case Kind::Downloads: {
            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, kCmdOpenFolder, L"Open in Explorer");
            UINT cmd = TrackMenu(menu, item);
            DestroyMenu(menu);
            if (cmd == kCmdOpenFolder) {
                Run(KnownFolderPath(kFolderDownloads).c_str());
            }
            break;
        }
        case Kind::Trash:
            ShowTrashMenu(*item);
            break;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Dragging icons

void StartDrag(POINT pt) {
    Item* item = g_press.item;
    g_drag = {};
    g_drag.item = item;
    g_drag.active = true;
    g_drag.size = std::max(item->size, g_d->geo.icon);
    POINT center = DockToScreen(item->center, item->bottomV + item->size / 2);
    g_drag.grab = {pt.x - center.x, pt.y - center.y};
    // Dragging a running app already treats it as pinned; if it's dropped
    // outside, it goes back to being just a running app.
    if (!item->pinned) {
        g_pins.push_back(item->target);
        SyncPins();
    }
}

void UpdateDrag(float cu, float cv) {
    Item* item = g_drag.item;
    if (!item) {
        g_drag.active = false;
        return;
    }
    g_drag.out = cv > std::max(g_d->layout.magTopV, g_d->geo.edgeGap + g_d->geo.plate) +
                          g_d->geo.icon * 0.3f ||
                 cu < g_d->layout.plateU0 - g_d->geo.icon ||
                 cu > g_d->layout.plateU1 + g_d->geo.icon;
    if (g_drag.out) {
        return;
    }

    int current = PinIndex(item->target);
    if (current < 0) {
        return;
    }
    int index = 0;
    for (auto& it : g_items) {
        if (it.get() != item && it->kind == Kind::App && it->pinned &&
            it->center < cu) {
            index++;
        }
    }
    if (index != current) {
        std::wstring pin = g_pins[current];
        g_pins.erase(g_pins.begin() + current);
        g_pins.insert(g_pins.begin() + std::min<size_t>(index, g_pins.size()),
                      pin);
        SyncPins();
    }
}

void EndDrag() {
    Item* item = g_drag.item;
    g_drag.active = false;
    if (!item) {
        return;
    }
    double now = Now();
    if (g_drag.out) {
        int index = PinIndex(item->target);
        if (index >= 0) {
            g_pins.erase(g_pins.begin() + index);
        }
        SyncPins();
        if (item->windows.empty()) {
            item->removing = item->hidden = true;
        }
        g_drag.item = nullptr;
        g_drag.poof = true;
        g_drag.poofStart = now;
        g_drag.poofAt = g_lastCursor;
    } else {
        g_drag.settling = true;
        g_drag.settleStart = now;
        g_drag.settleFrom = {g_lastCursor.x - g_drag.grab.x,
                             g_lastCursor.y - g_drag.grab.y};
    }
    g_drag.out = false;
    SavePins();
}

////////////////////////////////////////////////////////////////////////////////
// Dropping files

std::vector<std::wstring> GetDropFiles(IDataObject* data) {
    std::vector<std::wstring> files;
    FORMATETC format{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM medium{};
    if (FAILED(data->GetData(&format, &medium))) {
        return files;
    }
    if (auto drop = (HDROP)GlobalLock(medium.hGlobal)) {
        UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
        for (UINT i = 0; i < count; i++) {
            UINT length = DragQueryFileW(drop, i, nullptr, 0);
            std::wstring file(length, L'\0');
            DragQueryFileW(drop, i, file.data(), length + 1);
            files.push_back(std::move(file));
        }
        GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
    return files;
}

DWORD PickEffect(DWORD allowed, std::initializer_list<DWORD> preferred) {
    for (DWORD effect : preferred) {
        if (allowed & effect) {
            return effect;
        }
    }
    return DROPEFFECT_NONE;
}

DWORD DropEffect(DWORD allowed) {
    Item* item = g_d->layout.hovered;
    g_drop.target = nullptr;
    if (g_drop.files.empty()) {
        return DROPEFFECT_NONE;
    }
    if (item && item->kind == Kind::App &&
        (!item->target.empty() || !item->pathRaw.empty())) {
        g_drop.target = item;
        return PickEffect(allowed,
                          {DROPEFFECT_COPY, DROPEFFECT_LINK, DROPEFFECT_MOVE});
    }
    if (item && item->kind == Kind::Trash) {
        g_drop.target = item;
        return PickEffect(allowed, {DROPEFFECT_MOVE});
    }
    if (g_d->hovering && (!item || item->kind != Kind::App)) {
        return PickEffect(allowed, {DROPEFFECT_LINK, DROPEFFECT_COPY});
    }
    return DROPEFFECT_NONE;
}

int PinInsertIndex(float cu) {
    int index = 0;
    for (auto& it : g_items) {
        if (it->kind == Kind::App && it->pinned && it->center < cu) {
            index++;
        }
    }
    return index;
}

class DropTarget final : public IDropTarget {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refs);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG refs = InterlockedDecrement(&m_refs);
        if (!refs) {
            delete this;
        }
        return refs;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data,
                                        DWORD,
                                        POINTL pt,
                                        DWORD* effect) override {
        g_drop.active = true;
        g_drop.data = data;
        g_drop.files = GetDropFiles(data);
        g_drop.point = {pt.x, pt.y};
        Tick();
        *effect = DropEffect(*effect);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* effect) override {
        g_drop.point = {pt.x, pt.y};
        Tick();
        *effect = DropEffect(*effect);
        MarkDirty();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        g_drop = {};
        MarkDirty();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* data,
                                   DWORD,
                                   POINTL pt,
                                   DWORD* effect) override {
        g_drop.point = {pt.x, pt.y};
        Tick();
        *effect = DropEffect(*effect);
        if (*effect != DROPEFFECT_NONE) {
            // The work is deferred, so whoever dragged doesn't hang.
            auto* pending = new PendingDrop;
            pending->target = g_drop.target;
            pending->kind = g_drop.target ? g_drop.target->kind : Kind::Separator;
            pending->files = g_drop.files;
            pending->data = data;
            float cu, cv;
            ScreenToDock(g_drop.point, cu, cv);
            pending->insertIndex = PinInsertIndex(cu);
            PostMessageW(g_controller, WM_APP_DROP, 0, (LPARAM)pending);
        }
        g_drop = {};
        MarkDirty();
        return S_OK;
    }

   private:
    LONG m_refs = 1;
};

void OpenFilesWith(Item& item,
                   const std::vector<std::wstring>& files,
                   IDataObject* data) {
    bool wasRunning = !item.windows.empty();
    if (item.aumid == ToLower(kExplorerAumid)) {
        for (const auto& file : files) {
            bool folder = GetFileAttributesW(file.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
            std::wstring params =
                (folder ? L"\"" : L"/select,\"") + file + L"\"";
            Run(L"explorer.exe", params.c_str());
        }
        return;
    }

    // Store apps don't have an executable to call with the files.
    bool packaged = !item.aumidRaw.empty() &&
                    (item.pathRaw.empty() ||
                     item.path.find(L"\\windowsapps\\") != std::wstring::npos);
    if (packaged && data) {
        ComPtr<IApplicationActivationManager> manager;
        ComPtr<IShellItemArray> items;
        DWORD pid = 0;
        if (SUCCEEDED(CoCreateInstance(kClsidApplicationActivationManager,
                                       nullptr, CLSCTX_LOCAL_SERVER,
                                       IID_PPV_ARGS(&manager))) &&
            SUCCEEDED(SHCreateShellItemArrayFromDataObject(
                data, IID_PPV_ARGS(&items))) &&
            SUCCEEDED(manager->ActivateForFile(item.aumidRaw.c_str(),
                                               items.Get(), L"open", &pid))) {
            if (!wasRunning && g_settings.launchBounce) {
                StartBounce(item, Bounce::Launch);
            }
            return;
        }
    }

    std::wstring params;
    for (const auto& file : files) {
        if (!params.empty()) {
            params += L' ';
        }
        params += L"\"" + file + L"\"";
    }
    std::wstring program = !item.pathRaw.empty() && !item.pseudo
                               ? item.pathRaw
                               : item.target;
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_ASYNCOK;
    sei.lpFile = program.c_str();
    sei.lpParameters = params.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&sei) && !wasRunning && g_settings.launchBounce) {
        StartBounce(item, Bounce::Launch);
    }
}

void RecycleFiles(const std::vector<std::wstring>& files) {
    std::wstring from;
    for (const auto& file : files) {
        from += file;
        from.push_back(L'\0');
    }
    from.push_back(L'\0');
    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    op.fFlags = FOF_ALLOWUNDO;
    SHFileOperationW(&op);
    UpdateTrashState();
}

void HandleDrop(PendingDrop* drop) {
    Item* target = drop->target && ItemExists(drop->target) ? drop->target
                                                            : nullptr;
    if (target && target->kind == Kind::App) {
        OpenFilesWith(*target, drop->files, drop->data.Get());
    } else if (target && target->kind == Kind::Trash) {
        RecycleFiles(drop->files);
    } else if (!drop->target) {
        int index = Clamp(drop->insertIndex, 0, (int)g_pins.size());
        for (const auto& file : drop->files) {
            if (PinIndex(file) < 0) {
                g_pins.insert(g_pins.begin() + index++, file);
            }
        }
        SyncPins();
        SavePins();
    }
    delete drop;
}

////////////////////////////////////////////////////////////////////////////////
// Windows

UINT g_shellHookMsg;
UINT g_taskbarCreatedMsg;
UINT g_iconQueryMsg;
int g_slowTicks;

bool IsFullscreenForeground() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || IsOurWindow(hwnd)) {
        return false;
    }
    WCHAR className[64] = L"";
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (!wcscmp(className, L"Progman") || !wcscmp(className, L"WorkerW") ||
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != g_d->geo.monitor ||
        (GetWindowLongW(hwnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION) {
        return false;
    }
    RECT rc;
    const RECT& mon = g_d->geo.monitorRect;
    return GetWindowRect(hwnd, &rc) && rc.left <= mon.left &&
           rc.top <= mon.top && rc.right >= mon.right &&
           rc.bottom >= mon.bottom;
}

// An app window over an area of the screen. Menus, tooltips, overlays and
// bars from other mods don't count.
bool CoversArea(HWND hwnd, const RECT& area) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsOurWindow(hwnd)) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT)) {
        return false;
    }
    WCHAR className[64] = L"";
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    static const PCWSTR kSkip[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow"};
    for (PCWSTR skip : kSkip) {
        if (!wcscmp(className, skip)) {
            return false;
        }
    }
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        return false;
    }
    // Without the invisible resize border, which goes past the visible outline.
    RECT rc;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                                     sizeof(rc))) &&
        !GetWindowRect(hwnd, &rc)) {
        return false;
    }
    RECT overlap;
    return IntersectRect(&overlap, &rc, &area);
}

// With smart hiding the Dock only hides when some window is over its place
// (without magnification).
bool IsDockCovered() {
    if (g_settings.autoHide != AutoHide::Smart) {
        return false;
    }
    ComputeBase();
    float endPad = std::max<float>(0, g_d->geo.pad - g_d->geo.gap / 2);
    POINT a = DockToScreen(g_d->baseStart - endPad, 0);
    POINT b = DockToScreen(g_d->baseStart + g_d->baseTotal + endPad,
                           g_d->geo.edgeGap + g_d->geo.plate);
    struct Search {
        RECT area;
        bool covered;
    } search{{std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x),
              std::max(a.y, b.y)},
             false};
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            auto* s = (Search*)param;
            s->covered = CoversArea(hwnd, s->area);
            return !s->covered;
        },
        (LPARAM)&search);
    return search.covered;
}

HWINEVENTHOOK g_coverHooks[3];
bool g_coverPending;

void CALLBACK OnWinEvent(HWINEVENTHOOK,
                         DWORD event,
                         HWND hwnd,
                         LONG idObject,
                         LONG idChild,
                         DWORD,
                         DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZESTART) {
        if (Item* item = ItemForWindow(hwnd)) {
            item->lastMinimized = hwnd;
        }
    }
    if (g_settings.autoHide != AutoHide::Smart || g_coverPending ||
        GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }
    // Dragging a window generates events nonstop; merges them into one pass.
    g_coverPending = true;
    SetTimer(g_controller, kTimerCover, 50, nullptr);
}

void HookWindowChanges() {
    const DWORD ranges[][2] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (int i = 0; i < 3; i++) {
        g_coverHooks[i] = SetWinEventHook(
            ranges[i][0], ranges[i][1], nullptr, OnWinEvent, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
    }
}

void UnhookWindowChanges() {
    for (auto& hook : g_coverHooks) {
        if (hook) {
            UnhookWinEvent(hook);
            hook = nullptr;
        }
    }
}

void UpdateAppBar() {
    bool want =
        g_settings.reserveSpace && g_settings.autoHide == AutoHide::Never;
    APPBARDATA abd{sizeof(abd)};
    abd.hWnd = g_d->wnd;
    abd.uCallbackMessage = WM_APP_APPBAR;
    if (!want) {
        if (g_d->appBar) {
            SHAppBarMessage(ABM_REMOVE, &abd);
            g_d->appBar = false;
        }
        return;
    }
    if (!g_d->appBar) {
        if (!SHAppBarMessage(ABM_NEW, &abd)) {
            return;
        }
        g_d->appBar = true;
        g_d->appBarRect = {};
    }

    int thickness = std::lround(g_d->geo.edgeGap + g_d->geo.plate);
    abd.rc = g_d->geo.monitorRect;
    switch (g_settings.edge) {
        case Edge::Bottom:
            abd.uEdge = ABE_BOTTOM;
            abd.rc.bottom = g_d->geo.edgeCoord;
            abd.rc.top = g_d->geo.edgeCoord - thickness;
            break;
        case Edge::Left:
            abd.uEdge = ABE_LEFT;
            abd.rc.left = g_d->geo.edgeCoord;
            abd.rc.right = g_d->geo.edgeCoord + thickness;
            break;
        case Edge::Right:
            abd.uEdge = ABE_RIGHT;
            abd.rc.right = g_d->geo.edgeCoord;
            abd.rc.left = g_d->geo.edgeCoord - thickness;
            break;
    }
    // Changing the work area generates WM_SETTINGCHANGE; only sent if it changed.
    if (!EqualRect(&abd.rc, &g_d->appBarRect)) {
        g_d->appBarRect = abd.rc;
        SHAppBarMessage(ABM_SETPOS, &abd);
    }
}

// Runs fn on each Dock and returns to the active one, with the items in its layout.
template <typename F>
void ForEachDock(F fn) {
    for (auto& d : g_docks) {
        g_d = d.get();
        fn(*d);
    }
    g_d = g_active;
    if (g_d) {
        ComputeLayout(g_d->lastCursorU, Now());
    }
}

void UpdateDockStates() {
    ForEachDock([](Dock& d) {
        d.fullscreen = IsFullscreenForeground();
        d.covered = IsDockCovered();
    });
}

// Windows sometimes leaves "always on top" windows below regular ones (after
// a fullscreen app, for example), and the Dock vanishes behind a maximized
// window. If a regular window got on top, the Dock goes back to the top.
void KeepDocksOnTop() {
    for (auto& d : g_docks) {
        if (d->fullscreen || !d->wnd) {
            continue;
        }
        bool below = false;
        for (HWND h = GetWindow(d->wnd, GW_HWNDPREV); h;
             h = GetWindow(h, GW_HWNDPREV)) {
            if (IsWindowVisible(h) &&
                !(GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
                below = true;
                break;
            }
        }
        if (!below) {
            continue;
        }
        // The Dock window is owned by the background one and rises with it.
        for (HWND h : {d->plate, d->wnd}) {
            if (h) {
                SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }
    }
}

// After changing the monitor, the resolution or the scaling.
void ReapplyGeometry() {
    UpdateGeometry();
    g_d->appBarRect = {};
    UpdateAppBar();
    g_d->fullscreen = IsFullscreenForeground();
    g_d->covered = IsDockCovered();
    g_d->dirty = true;
}

bool CreateDockWindows(Dock& dock);

void DestroyDock(Dock& dock) {
    g_d = &dock;
    if (dock.appBar) {
        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = dock.wnd;
        SHAppBarMessage(ABM_REMOVE, &abd);
        dock.appBar = false;
    }
    if (dock.wnd) {
        RevokeDragDrop(dock.wnd);
        RemovePropW(dock.wnd, L"WindhawkMacDock.Edge");
        RemovePropW(dock.wnd, L"WindhawkMacDock.Reserved");
    }
    // The background window is the owner: destroying it takes the Dock's along.
    if (dock.plate) {
        DestroyWindow(dock.plate);
    }
}

// One Dock for each monitor (or only for the primary one), creating and
// removing them as monitors are connected and disconnected.
void SyncDocks() {
    using Monitors = std::vector<std::pair<HMONITOR, std::wstring>>;
    Monitors monitors;
    auto add = [](HMONITOR monitor, Monitors& list) {
        MONITORINFOEXW mi{};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(monitor, &mi)) {
            list.emplace_back(monitor, mi.szDevice);
        }
    };
    if (g_settings.allMonitors) {
        EnumDisplayMonitors(
            nullptr, nullptr,
            [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
                MONITORINFOEXW mi{};
                mi.cbSize = sizeof(mi);
                if (GetMonitorInfoW(monitor, &mi)) {
                    ((Monitors*)param)->emplace_back(monitor, mi.szDevice);
                }
                return TRUE;
            },
            (LPARAM)&monitors);
    }
    if (monitors.empty()) {
        POINT origin{0, 0};
        add(MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY), monitors);
    }

    for (auto i = g_docks.begin(); i != g_docks.end();) {
        Dock* dock = i->get();
        auto found = std::find_if(
            monitors.begin(), monitors.end(),
            [dock](const auto& m) { return m.second == dock->device; });
        if (found != monitors.end()) {
            dock->monitor = found->first;
            ++i;
            continue;
        }
        if (g_active == dock) {
            g_active = nullptr;
        }
        DestroyDock(*dock);
        i = g_docks.erase(i);
    }

    for (const auto& [monitor, device] : monitors) {
        if (std::any_of(g_docks.begin(), g_docks.end(),
                        [&](const auto& d) { return d->device == device; })) {
            continue;
        }
        auto dock = std::make_unique<Dock>();
        dock->device = device;
        dock->monitor = monitor;
        // A new Dock rises from the edge.
        dock->hideProgress = g_ready ? 1 : 0;
        if (CreateDockWindows(*dock)) {
            g_docks.push_back(std::move(dock));
        }
    }

    g_d = nullptr;
    if (g_docks.empty()) {
        g_active = nullptr;
        return;
    }
    if (!g_active) {
        POINT pt;
        GetCursorPos(&pt);
        g_active = DockAt(pt);
    }
    if (!g_active) {
        g_active = g_docks[0].get();
    }

    int oldPixels = g_iconPixels;
    g_iconPixels = 0;
    ForEachDock([](Dock& d) {
        ReapplyGeometry();
        if (!d.lastCursorU) {
            d.lastCursorU = (d.geo.axisStart + d.geo.axisEnd) / 2.0f;
        }
        g_iconPixels = std::max(g_iconPixels, d.geo.iconPixels);
    });
    if (g_iconPixels != oldPixels) {
        ResetIcons();
    }
    for (auto& it : g_items) {
        it->labelDpi = 0;
    }
}

void EnsureStaticItems() {
    auto ensure = [](Kind kind, bool want, int group, const std::wstring& name) {
        auto found = std::find_if(g_items.begin(), g_items.end(),
                                  [kind](const auto& it) { return it->kind == kind; });
        if (!want) {
            if (found != g_items.end()) {
                ClearReferences(found->get());
                g_items.erase(found);
            }
            return;
        }
        if (found != g_items.end()) {
            return;
        }
        auto item = std::make_unique<Item>();
        item->kind = kind;
        item->group = group;
        item->name = name;
        item->presence = g_ready ? 0 : 1;
        g_items.push_back(std::move(item));
    };

    std::wstring downloads = L"Downloads";
    ComPtr<IShellItem> folder;
    if (SUCCEEDED(SHGetKnownFolderItem(kFolderDownloads, KF_FLAG_DEFAULT,
                                       nullptr, IID_PPV_ARGS(&folder)))) {
        std::wstring name = DisplayName(folder.Get());
        if (!name.empty()) {
            downloads = name;
        }
    }

    const auto& s = g_settings;
    ensure(Kind::Start, s.showStart, 0, L"Start");
    ensure(Kind::Separator, s.showDownloads || s.showTrash, 3, L"");
    ensure(Kind::Downloads, s.showDownloads, 4, downloads);
    ensure(Kind::Trash, s.showTrash, 5, L"Recycle Bin");
    Reorder();
}

void ApplySettings() {
    UpdateColors();
    SetTaskbarHidden(g_settings.hideTaskbar);
    SyncDocks();
    if (g_docks.empty()) {
        return;
    }
    EnsureStaticItems();
    if (!g_settings.showRunning) {
        RefreshWindows();
    }
    ForEachDock([](Dock& d) {
        ApplyPlateBackdrop();
        d.covered = IsDockCovered();
        d.dirty = true;
    });
    Tick();
}

void OnShellHook(WPARAM code, HWND hwnd) {
    switch (code) {
        case HSHELL_WINDOWCREATED:
        case HSHELL_WINDOWDESTROYED:
            SetTimer(g_controller, kTimerRefresh, 60, nullptr);
            break;
        case HSHELL_WINDOWACTIVATED:
        case HSHELL_RUDEAPPACTIVATED:
            if (Item* item = ItemForWindow(hwnd)) {
                if (item->bounce == Bounce::Attention) {
                    StopBounce(*item, Now());
                }
            }
            SetTimer(g_controller, kTimerRefresh, 60, nullptr);
            break;
        case HSHELL_FLASH: {
            if (!g_settings.attentionBounce || GetForegroundWindow() == hwnd) {
                break;
            }
            Item* item = ItemForWindow(hwnd);
            if (!item) {
                RefreshWindows();
                item = ItemForWindow(hwnd);
            }
            if (item && item->bounce == Bounce::None) {
                StartBounce(*item, Bounce::Attention);
                Tick();
            }
            break;
        }
    }
}

// Where the window shrinks to when minimizing, and comes back from: the
// app's icon on the Dock of its monitor, not the hidden taskbar button.
bool GetMinRect(HWND hwnd, RECT& rc) {
    Item* item = ItemForWindow(hwnd);
    if (!item) {
        RefreshWindows();
        item = ItemForWindow(hwnd);
    }
    // Once minimized, the window is already parked at -32000 and the nearest
    // monitor would always be the primary one; where it left from counts.
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    RECT current;
    WINDOWPLACEMENT placement{sizeof(placement)};
    if ((IsIconic(hwnd) ||
         (GetWindowRect(hwnd, &current) && current.left <= -30000)) &&
        GetWindowPlacement(hwnd, &placement)) {
        monitor =
            MonitorFromRect(&placement.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
    }
    Dock* dock = g_active;
    for (auto& d : g_docks) {
        if (d->monitor == monitor) {
            dock = d.get();
        }
    }
    if (!item || !dock) {
        return false;
    }
    // May arrive in the middle of a loop over the Docks; returns to the one it was on.
    Dock* previous = g_d;
    g_d = dock;
    ComputeLayout(dock->lastCursorU, Now());
    float size = std::max(item->size, g_d->geo.icon * 0.5f);
    float v = std::max(0.0f, item->bottomV);
    POINT a = DockToScreen(item->center - size / 2, v);
    POINT b = DockToScreen(item->center + size / 2, v + size);
    rc = {std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x),
          std::max(a.y, b.y)};
    g_d = previous;
    if (g_d) {
        ComputeLayout(g_d->lastCursorU, Now());
    }
    return true;
}

// For other mods, like an adapted minimize animation: the middle of the top
// edge of the window's icon, or 0 if it isn't in the Dock. Fits in 32 bits,
// so it also works for 32-bit processes.
LRESULT QueryIconPoint(HWND hwnd) {
    RECT rc;
    if (!IsWindow(hwnd) || !GetMinRect(hwnd, rc)) {
        return 0;
    }
    int x = (rc.left + rc.right) / 2;
    int y = rc.top;
    // With the Dock hidden, the icon is off screen; the animation goes to the
    // edge, at the point where the Dock shows up.
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi)) {
        const RECT& m = mi.rcMonitor;
        x = std::clamp<int>(x, m.left, m.right - 1);
        y = std::clamp<int>(y, m.top, std::max<int>(m.top, m.bottom -
                                                         (rc.bottom - rc.top)));
    }
    return (LRESULT)(DWORD)MAKELONG((WORD)(SHORT)x, (WORD)(SHORT)y);
}

// When restoring, Windows asks the taskbar's task window (MSTaskSwWClass)
// where the window comes back from, and the hidden taskbar answered with
// the screen corner. When minimizing it doesn't ask anything. Runs on the
// taskbar's thread and forwards the question to the Dock.
HWND g_taskman;

LRESULT CALLBACK TaskmanSubclassProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR) {
    if (msg == g_shellHookMsg && g_shellHookMsg &&
        wParam == HSHELL_GETMINRECT && lParam) {
        DWORD_PTR handled = 0;
        HWND controller = g_controller;
        if (controller &&
            SendMessageTimeoutW(controller, WM_APP_MINRECT, 0, lParam,
                                SMTO_ABORTIFHUNG, 200, &handled) &&
            handled) {
            return TRUE;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Also redone when the taskbar is recreated (explorer restarted).
void HookTaskman() {
    using GetTaskmanWindow_t = HWND(WINAPI*)();
    static auto getTaskmanWindow = (GetTaskmanWindow_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetTaskmanWindow");
    HWND taskman = getTaskmanWindow ? getTaskmanWindow() : nullptr;
    if (taskman == g_taskman && (!taskman || IsWindow(taskman))) {
        return;
    }
    if (g_taskman && IsWindow(g_taskman)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_taskman,
                                                         TaskmanSubclassProc);
    }
    g_taskman = nullptr;
    DWORD pid = 0;
    if (taskman && GetWindowThreadProcessId(taskman, &pid) &&
        pid == GetCurrentProcessId() &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            taskman, TaskmanSubclassProc, 0)) {
        g_taskman = taskman;
    }
}

void UnhookTaskman() {
    if (g_taskman && IsWindow(g_taskman)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_taskman,
                                                         TaskmanSubclassProc);
    }
    g_taskman = nullptr;
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    if (msg == g_iconQueryMsg && g_iconQueryMsg) {
        return QueryIconPoint((HWND)wParam);
    }
    if (msg == g_shellHookMsg && g_shellHookMsg) {
        if (wParam == HSHELL_GETMINRECT) {
            auto* info = (SHELLHOOKINFO*)lParam;
            return info && GetMinRect(info->hwnd, info->rc);
        }
        OnShellHook(wParam, (HWND)lParam);
        return 0;
    }
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg) {
        if (g_settings.hideTaskbar) {
            SetTaskbarHidden(true);
        }
        HookTaskman();
        return 0;
    }

    switch (msg) {
        case WM_APP_MINRECT: {
            auto* info = (SHELLHOOKINFO*)lParam;
            return info && GetMinRect(info->hwnd, info->rc);
        }

        case WM_TIMER:
            if (wParam == kTimerRefresh) {
                KillTimer(hwnd, kTimerRefresh);
                RefreshWindows();
                UpdateDockStates();
                MarkDirty();
            } else if (wParam == kTimerCover) {
                KillTimer(hwnd, kTimerCover);
                g_coverPending = false;
                UpdateDockStates();
                KeepDocksOnTop();
            } else if (wParam == kTimerSlow) {
                RefreshWindows();
                UpdateDockStates();
                KeepDocksOnTop();
                if (g_settings.hideTaskbar) {
                    SetTaskbarHidden(true);
                }
                HookTaskman();
                if (++g_slowTicks % 3 == 0) {
                    UpdateTrashState();
                }
            }
            Tick();
            return 0;

        case WM_APP_DROP:
            HandleDrop((PendingDrop*)lParam);
            Tick();
            return 0;

        case WM_APP_RELOAD:
            LoadSettings();
            ApplySettings();
            return 0;

        case WM_SETTINGCHANGE:
            if (lParam && !wcscmp((PCWSTR)lParam, L"ImmersiveColorSet")) {
                UpdateColors();
                ForEachDock([](Dock&) { ApplyPlateBackdrop(); });
                MarkDirty();
            } else if (wParam == SPI_SETWORKAREA) {
                ForEachDock([](Dock&) { UpdateGeometry(); });
                MarkDirty();
            }
            Tick();
            return 0;

        case WM_DISPLAYCHANGE:
            SyncDocks();
            Tick();
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, kTimerTick);
            KillTimer(hwnd, kTimerRefresh);
            KillTimer(hwnd, kTimerSlow);
            KillTimer(hwnd, kTimerCover);
            UnhookWindowChanges();
            UnhookTaskman();
            DeregisterShellHookWindow(hwnd);
            for (auto& d : g_docks) {
                DestroyDock(*d);
            }
            g_docks.clear();
            g_d = g_active = nullptr;
            if (g_dragWnd) {
                DestroyWindow(g_dragWnd);
                g_dragWnd = nullptr;
            }
            SetTaskbarHidden(false);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK DockWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {
            if (g_press.item && !g_drag.active && (wParam & MK_LBUTTON) &&
                g_press.item->kind == Kind::App &&
                !g_press.item->target.empty()) {
                POINT pt;
                GetCursorPos(&pt);
                int threshold = std::lround(6 * g_d->geo.scale);
                if (std::abs(pt.x - g_press.point.x) > threshold ||
                    std::abs(pt.y - g_press.point.y) > threshold) {
                    StartDrag(pt);
                }
            }
            Tick();
            return 0;
        }

        case WM_LBUTTONDOWN:
            Tick();
            g_press = {};
            if (g_d->layout.hovered) {
                g_press.item = g_d->layout.hovered;
                GetCursorPos(&g_press.point);
                SetCapture(hwnd);
            }
            MarkDirty();
            Tick();
            return 0;

        case WM_LBUTTONUP: {
            Tick();
            Item* item = g_press.item;
            bool dragging = g_drag.active;
            g_press = {};
            if (dragging) {
                EndDrag();
            }
            ReleaseCapture();
            if (!dragging && item && item == g_d->layout.hovered) {
                OnItemClick(*item);
            }
            MarkDirty();
            Tick();
            return 0;
        }

        case WM_CAPTURECHANGED:
            if (g_drag.active) {
                EndDrag();
            }
            g_press = {};
            MarkDirty();
            return 0;

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            return 0;

        case WM_RBUTTONUP:
            Tick();
            OnItemRightClick(g_d->layout.hovered);
            Tick();
            return 0;

        case WM_MBUTTONUP:
            Tick();
            if (Item* item = g_d->layout.hovered;
                item && item->kind == Kind::App) {
                Launch(*item);
            }
            return 0;

        case WM_APP_APPBAR:
            if (wParam == ABN_POSCHANGED) {
                ForEachDock([hwnd](Dock& d) {
                    if (d.wnd == hwnd) {
                        d.appBarRect = {};
                        UpdateAppBar();
                    }
                });
            }
            return 0;

        case WM_DPICHANGED:
            // Arrives in the middle of drawing, when moving the window; deferred.
            PostMessageW(g_controller, WM_DISPLAYCHANGE, 0, 0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK PlateWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            // Black with zero alpha: only the DWM effect shows.
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            FillRect(dc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK DragWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam);

constexpr DWORD kDockExStyle =
    WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;

bool CreateDockWindows(Dock& dock) {
    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    dock.plate = CreateWindowExW(kDockExStyle, kPlateClass, L"", WS_POPUP, 0,
                                 0, 0, 0, nullptr, nullptr, instance, nullptr);
    if (!dock.plate) {
        return false;
    }
    dock.wnd = CreateWindowExW(kDockExStyle | WS_EX_LAYERED, kDockClass,
                               L"Dock", WS_POPUP, 0, 0, 0, 0, dock.plate,
                               nullptr, instance, nullptr);
    if (!dock.wnd) {
        DestroyWindow(dock.plate);
        return false;
    }

    g_d = &dock;
    ApplyPlateBackdrop();
    ShowWindow(dock.wnd, SW_SHOWNOACTIVATE);

    if (auto* target = new DropTarget) {
        RegisterDragDrop(dock.wnd, target);
        target->Release();
    }
    return true;
}

bool CreateController() {
    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    HWND controller = CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"",
                                      WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                      instance, nullptr);
    if (!controller) {
        return false;
    }
    g_controller = controller;
    g_dragWnd = CreateWindowExW(
        kDockExStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT, kDragClass, L"",
        WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);

    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(controller);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    // Apps of any integrity level can ask.
    g_iconQueryMsg = RegisterWindowMessageW(L"WindhawkMacDock.GetIconPoint");
    ChangeWindowMessageFilterEx(controller, g_iconQueryMsg, MSGFLT_ALLOW,
                                nullptr);

    HookWindowChanges();
    HookTaskman();
    SetTimer(controller, kTimerSlow, 1000, nullptr);
    SetTimer(controller, kTimerTick, 50, nullptr);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Dock thread

HANDLE g_thread;
HANDLE g_readyEvent;

DWORD WINAPI DockThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    OleInitialize(nullptr);

    LoadSettings();
    UpdateColors();

    int savedState = Wh_GetIntValue(L"taskbarState", -1);
    if (savedState >= 0) {
        g_changedAutohide = true;
        g_originalTaskbarState = (UINT)savedState;
    }

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g_dwrite.GetAddressOf());
    CoCreateInstance(kClsidVirtualDesktopManager, nullptr, CLSCTX_ALL,
                     IID_PPV_ARGS(&g_desktops));

    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = DockWndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kDockClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW pc{sizeof(pc)};
    pc.lpfnWndProc = PlateWndProc;
    pc.hInstance = instance;
    pc.lpszClassName = kPlateClass;
    RegisterClassExW(&pc);

    WNDCLASSEXW dc{sizeof(dc)};
    dc.lpfnWndProc = DragWndProc;
    dc.hInstance = instance;
    dc.lpszClassName = kDragClass;
    RegisterClassExW(&dc);

    WNDCLASSEXW cc{sizeof(cc)};
    cc.lpfnWndProc = ControllerWndProc;
    cc.hInstance = instance;
    cc.lpszClassName = kControllerClass;
    RegisterClassExW(&cc);

    bool ok = g_d2d && g_dwrite && CreateController();
    if (ok) {
        SetTaskbarHidden(g_settings.hideTaskbar);
        Wh_DeleteValue(L"monitor");  // From the version where the Dock switched monitors.
        g_trashFull = QueryTrashFull();
        SyncDocks();
        if (g_docks.empty()) {
            DestroyWindow(g_controller);
            ok = false;
        }
    }
    if (ok) {
        LoadPins();
        EnsureStaticItems();
        SyncPins();
        g_lastTick = Now();
        UpdateDockStates();
        MarkDirty();
        Tick();
        g_ready = true;
    }
    SetEvent(g_readyEvent);

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Failed to create the Dock");
        SetTaskbarHidden(false);
    }
    g_controller = nullptr;

    // Discards dropped files that never got handled.
    MSG msg;
    while (PeekMessageW(&msg, nullptr, WM_APP_DROP, WM_APP_DROP, PM_REMOVE)) {
        delete (PendingDrop*)msg.lParam;
    }

    g_items.clear();
    g_windowCache.clear();
    g_drop = {};
    FreeSurface(g_dockSurface);
    FreeSurface(g_dragSurface);
    ResetTarget();
    g_desktops.Reset();
    g_dwrite.Reset();
    g_d2d.Reset();

    UnregisterClassW(kDockClass, instance);
    UnregisterClassW(kPlateClass, instance);
    UnregisterClassW(kDragClass, instance);
    UnregisterClassW(kControllerClass, instance);
    OleUninitialize();
    return 0;
}

// With "Launch folder windows in a separate process", folder windows run in
// their own explorer.exe; the Dock stays only in the taskbar's process.
bool IsShellExplorer() {
    std::wstring commandLine = ToLower(GetCommandLineW());
    return commandLine.find(L"/factory") == std::wstring::npos &&
           commandLine.find(L"-embedding") == std::wstring::npos &&
           commandLine.find(L"/separate") == std::wstring::npos;
}

bool g_isShell;

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_isShell = IsShellExplorer();
    if (!g_isShell) {
        Wh_Log(L"Folder windows process, the Dock stays in the taskbar "
               L"process");
        return TRUE;
    }

    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                       (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, DockThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (!g_thread) {
        return;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (HWND controller = g_controller) {
        PostMessageW(controller, WM_CLOSE, 0, 0);
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    if (HWND controller = g_controller) {
        PostMessageW(controller, WM_APP_RELOAD, 0, 0);
    }
    return TRUE;
}
