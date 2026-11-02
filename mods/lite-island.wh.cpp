// ==WindhawkMod==
// @id              lite-island
// @name            Lite Island
// @description     A themeable Dynamic Island at the top of the screen: clock, calendar, media with mixer, live tray icons, weather, control center, visualizer
// @version         2.1.2
// @author          kelcum
// @github          https://github.com/kelcum
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -ldwmapi -ld2d1 -ldwrite -lwindowscodecs -liphlpapi -lshcore -luuid -lgdi32 -lcomctl32 -lwinhttp -lwlanapi -ldxva2 -lwbemuuid -lpowrprof -lversion -lwinmm
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lite Island

A Dynamic Island for Windows 11, made to be fast and fully themeable.

## The island

* **Clock** with date and seconds. Click it for a **calendar**.
* **Media**: cover, title and artist, previous / play / next and a
  **visualizer** that follows the real audio level. Click it for the
  **media panel**: big cover, seekable timeline, shuffle and repeat, the
  app's own volume, the master volume and a **mixer** for every app.
* **Tray icons**: the real ones of your apps, live. Expand them inline with
  the arrow, or show them in a grid panel. Left, right and double clicks go
  to the app. Middle-click an icon (or use the pin in the grid) to keep it
  on the island.
* **Notifications**: the icon of the last app that notified you, with the
  number waiting in the notification center.
* **Weather** for your city (Open-Meteo, free, no key). Click it for the
  hourly and 5-day forecast.
* **Control center**: Wi-Fi, Bluetooth, microphone, dark mode, volume and
  brightness.
* **Power menu**: lock, sleep, sign out, restart and shut down (these last
  three ask for a second click).
* **Pop-outs**: new notifications, the next song, plugging in the charger,
  volume changes and finished timers grow out of the island for a moment.
* **Timer and stopwatch** (tabs in the clock's panel), shown on the island
  while they run.
* **Synced lyrics** (optional, from LRCLIB) on the island and in the media
  panel.
* **Scroll** anywhere on the island to change the volume.
* The **arrow on the right** folds the island into a thin line; hover the
  line to peek, click it to bring the island back.

## Looks

Solid, accent, glass, or **album art**: the playing cover, blurred and
darkened behind the island and its panels. Colors, sizes, corner radius,
font, opacity and every element can be changed in the settings.

## How it stays light

* The island runs in its own `windhawk.exe` process. The only thing in
  explorer is a small listener on the taskbar window that forwards tray icon
  updates, so a problem in the island can't take the taskbar down.
* It only redraws when something changes. Animations and the visualizer run
  only while they're visible; the visualizer only redraws its own pixels.
* Weather is fetched every 30 minutes, and only when the weather is shown.

## Notes

* Weather data by [Open-Meteo.com](https://open-meteo.com) (CC BY 4.0).
  Lyrics by [LRCLIB](https://lrclib.net) (off by default: only the song's
  title, artist and length are sent).
* Tray icons are learned as apps send them to the taskbar; apps are asked to
  send them again when the mod starts.
* Glass mode uses Windows' own blur, which can only round its corners
  Windows' way.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- elements:
  - power: true
    $name: Power button
  - notifications: true
    $name: Notifications (last app and count)
  - tray: inline
    $name: Tray icons
    $options:
    - inline: Inline, expanded with an arrow
    - panel: In a panel
    - off: Hidden
  - controlCenter: true
    $name: Control center button
  - volume: true
    $name: Volume icon
  - network: false
    $name: Network icon
  - battery: true
    $name: Battery icon (laptops)
  - weather: true
    $name: Weather
  - clock: true
    $name: Clock
  - media: true
    $name: Media
  - mediaControls: true
    $name: Media buttons
  - visualizer: true
    $name: Visualizer
  - collapseButton: true
    $name: Fold button
  $name: What the island shows
- background:
  - mode: albumArt
    $name: Background
    $options:
    - solid: Solid color
    - accent: Windows accent color
    - glass: Glass (Windows blur)
    - albumArt: Album art of the current track
  - fallback: solid
    $name: Album art fallback
    $description: Used when nothing plays or the track has no cover
    $options:
    - solid: Solid color
    - accent: Windows accent color
    - glass: Glass (Windows blur)
  - artWhenPaused: true
    $name: Keep the album art while paused
  - artFit: cover
    $name: Album art scaling
    $options:
    - cover: Cover
    - stretch: Stretch (more colors)
  - opacity: 96
    $name: Opacity (%)
  - artBlur: 50
    $name: Album art blur (0-100)
  - artDim: 55
    $name: Album art darkening (%)
  - artAutoDim: true
    $name: Darken bright covers more
  - artSaturation: 140
    $name: Album art saturation (%)
  - panelArt: true
    $name: Album art behind the panels too
  - glassType: acrylic
    $name: Glass type
    $options:
    - acrylic: Acrylic
    - blur: Blur
  $name: Background
- colors:
  - background: "#0C0C0E"
    $name: Background
    $description: "#RRGGBB or #AARRGGBB"
  - text: "#FFFFFF"
    $name: Text
  - icons: "#FFFFFF"
    $name: Icons
  - accent: ""
    $name: Accent
    $description: Empty = Windows accent color
  - border: "#1FFFFFFF"
    $name: Border
  - borderWidth: 1
    $name: Border width
  $name: Colors
- size:
  - height: 34
    $name: Height
  - cornerRadius: 17
    $name: Corner radius
  - paddingX: 8
    $name: Horizontal padding
  - spacing: 6
    $name: Space between groups
  - minWidth: 120
    $name: Minimum width
  - maxWidth: 1200
    $name: Maximum width
  - topOffset: 6
    $name: Distance from the top
  - fontSize: 13
    $name: Font size
  - iconSize: 15
    $name: Icon size
  - fontFamily: Segoe UI Variable Text
    $name: Font
  - boldClock: true
    $name: Bold time
  $name: Size and font
- clock:
  - seconds: true
    $name: Seconds
  - date: true
    $name: Date
  - timeFormat: ""
    $name: Time format
    $description: "Empty = system format. Examples: HH:mm:ss, h:mm tt"
  - dateFormat: ddd MMM d
    $name: Date format
  $name: Clock
- media:
  - art: true
    $name: Cover on the island
  - artist: true
    $name: Artist on the island
  - maxTextWidth: 220
    $name: Maximum title width
  - lyrics: false
    $name: Synced lyrics
    $description: Shows the current line instead of the title, from lrclib.net (sends the song's title, artist and length)
  $name: Media
- popups:
  - notifications: true
    $name: New notifications
  - track: true
    $name: Next song
  - charging: true
    $name: Charger plugged in or out
  - volume: true
    $name: Volume changes (also from keys and other apps)
  - seconds: 5
    $name: How long notifications stay (seconds)
  $name: Pop-outs
- weather:
  - city: ""
    $name: City
    $description: "Like: London, or Austin, Texas. Empty = Windows location (if allowed for desktop apps)"
  - units: fahrenheit
    $name: Units
    $options:
    - fahrenheit: °F, mph
    - celsius: °C, km/h
  $name: Weather
- visualizer:
  - bars: 6
    $name: Bars
  - color: text
    $name: Color
    $options:
    - text: Text color
    - accent: Accent color
  $name: Visualizer
- shortcuts:
  - - target: ""
      $name: Target
      $description: An exe, shortcut, folder, URL, or shell:AppsFolder\<AppUserModelID>
    - args: ""
      $name: Arguments
    - glyph: ""
      $name: Glyph (optional)
      $description: Hex code of a Segoe Fluent Icons glyph (like E8A7) drawn instead of the app's icon
  $name: Shortcuts on the island
- behavior:
  - scrollVolume: true
    $name: Scroll to change the volume
  - volumeStep: 2
    $name: Volume step (%)
  - animations: true
    $name: Animations
  - startFolded: false
    $name: Start folded into a line
  - peek: true
    $name: Hovering the line shows the island
  - hideFullscreen: true
    $name: Hide while a fullscreen app runs
  - reserveSpace: false
    $name: Reserve space at the top of the screen
  - monitor: 0
    $name: Monitor
    $description: 0 = the main monitor, 1, 2, ... = that monitor
  $name: Behavior
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>

#include <appmodel.h>
#include <audiopolicy.h>
#include <commctrl.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <endpointvolume.h>
#include <exdisp.h>
#include <highlevelmonitorconfigurationapi.h>
#include <iphlpapi.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <physicalmonitorenumerationapi.h>
#include <powrprof.h>
#include <shcore.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wbemidl.h>
#include <wincodec.h>
#include <winhttp.h>
#include <wlanapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Devices.Geolocation.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.ViewManagement.h>

using Microsoft::WRL::ComPtr;

// mingw only forward-declares this one.
struct IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
__CRT_UUID_DECL(IAudioMeterInformation, 0xC02216F6, 0x8C67, 0x4B5B, 0x9D, 0x00, 0xD0, 0x08,
                0xE7, 0x3E, 0x00, 0x64)
namespace wmc = winrt::Windows::Media::Control;

HMODULE g_module;
bool g_isExplorer;

constexpr WCHAR kIslandClass[] = L"LiteIsland_" WH_MOD_ID;
constexpr WCHAR kPanelClass[] = L"LiteIslandPanel_" WH_MOD_ID;
constexpr WCHAR kTipClass[] = L"LiteIslandTip_" WH_MOD_ID;
const UINT g_msgRequestTray =
    RegisterWindowMessage(L"LiteIsland_RequestTray_" WH_MOD_ID);
const UINT g_msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

// What Shell_NotifyIcon sends the taskbar (handles are 32-bit on the wire).
#pragma pack(push, 4)
struct NotifyIconData32 {
    DWORD cbSize;
    DWORD hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    UINT uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};
struct TrayNotifyData {
    DWORD signature;
    DWORD message;
    NotifyIconData32 nid;
};
#pragma pack(pop)
constexpr DWORD kTraySignature = 0x34753423;

// Explorer -> island.
constexpr ULONG_PTR kIpcTray = 0x4C49544E;
constexpr UINT kVersionUnknown = 0xFFFFFFFF;
struct TrayIpc {
    UINT32 op;   // 0 upsert, 1 remove, 2 clear
    UINT32 id;
    UINT64 owner;
    GUID guid;
    UINT32 hasGuid;
    UINT32 callback;
    UINT32 version;
    UINT32 hidden;
    UINT32 pid;
    UINT64 icon;
    WCHAR tip[128];
};

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (!len || len == ARRAYSIZE(path)) return false;
    PCWSTR name = wcsrchr(path, L'\\');
    return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
}

std::wstring GetStr(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Explorer part: listens to the taskbar window for tray icon updates.

namespace ex {

struct Icon {
    HWND owner = nullptr;
    UINT id = 0;
    GUID guid{};
    bool hasGuid = false;
    UINT callback = 0;
    UINT version = 0;
    HICON icon = nullptr;
    std::wstring tip;
    bool hidden = false;
    DWORD pid = 0;
};

std::vector<Icon> g_icons;
SRWLOCK g_lock = SRWLOCK_INIT;
HWND g_taskbar;
bool g_subclassed;
HANDLE g_waitThread;
HANDLE g_stop;
UINT g_runMsg;

HWND FindIsland() { return FindWindow(kIslandClass, nullptr); }

void Send(HWND island, const Icon* icon, UINT op) {
    TrayIpc d{};
    d.op = op;
    if (icon) {
        d.id = icon->id;
        d.owner = (UINT64)(ULONG_PTR)icon->owner;
        d.guid = icon->guid;
        d.hasGuid = icon->hasGuid;
        d.callback = icon->callback;
        d.version = icon->version;
        d.hidden = icon->hidden;
        d.pid = icon->pid;
        d.icon = (UINT64)(ULONG_PTR)icon->icon;
        wcsncpy_s(d.tip, icon->tip.c_str(), _TRUNCATE);
    }
    COPYDATASTRUCT cds{kIpcTray, sizeof(d), &d};
    DWORD_PTR result;
    SendMessageTimeout(island, WM_COPYDATA, (WPARAM)g_taskbar, (LPARAM)&cds,
                       SMTO_ABORTIFHUNG | SMTO_BLOCK, 300, &result);
}

void SendAll() {
    HWND island = FindIsland();
    if (!island) return;
    AcquireSRWLockShared(&g_lock);
    std::vector<Icon> icons = g_icons;
    ReleaseSRWLockShared(&g_lock);
    Send(island, nullptr, 2);
    for (auto& icon : icons) {
        if (IsWindow(icon.owner)) Send(island, &icon, 0);
    }
}

void OnTrayNotify(const COPYDATASTRUCT* cds) {
    constexpr size_t kHeader = offsetof(TrayNotifyData, nid);
    if (cds->dwData != 1 || !cds->lpData || cds->cbData < kHeader + 24) return;
    auto data = (const TrayNotifyData*)cds->lpData;
    if (data->signature != kTraySignature) return;
    const NotifyIconData32& nid = data->nid;
    size_t size = std::min<size_t>(nid.cbSize, cds->cbData - kHeader);
    auto has = [&](size_t offset, size_t len) { return size >= offset + len; };

    HWND owner = (HWND)(ULONG_PTR)nid.hWnd;
    bool hasGuid = (nid.uFlags & NIF_GUID) &&
                   has(offsetof(NotifyIconData32, guidItem), sizeof(GUID));

    UINT op = 0;
    Icon copy;
    bool send = false;
    AcquireSRWLockExclusive(&g_lock);
    auto it = std::find_if(g_icons.begin(), g_icons.end(), [&](const Icon& i) {
        return hasGuid ? (i.hasGuid && i.guid == nid.guidItem)
                       : (i.owner == owner && i.id == nid.uID);
    });
    switch (data->message) {
        case NIM_ADD:
        case NIM_MODIFY: {
            if (it == g_icons.end()) {
                if (data->message == NIM_MODIFY) break;
                Icon icon;
                icon.owner = owner;
                icon.id = nid.uID;
                icon.hasGuid = hasGuid;
                if (hasGuid) icon.guid = nid.guidItem;
                GetWindowThreadProcessId(owner, &icon.pid);
                g_icons.push_back(icon);
                it = g_icons.end() - 1;
            }
            if (nid.uFlags & NIF_MESSAGE) it->callback = nid.uCallbackMessage;
            if ((nid.uFlags & NIF_ICON) &&
                has(offsetof(NotifyIconData32, hIcon), sizeof(DWORD))) {
                if (it->icon) DestroyIcon(it->icon);
                it->icon = nid.hIcon ? CopyIcon((HICON)(ULONG_PTR)nid.hIcon)
                                     : nullptr;
            }
            if ((nid.uFlags & NIF_TIP) &&
                has(offsetof(NotifyIconData32, szTip), sizeof(WCHAR))) {
                size_t maxChars = std::min<size_t>(
                    128, (size - offsetof(NotifyIconData32, szTip)) /
                             sizeof(WCHAR));
                it->tip.assign(nid.szTip, wcsnlen(nid.szTip, maxChars));
            }
            if ((nid.uFlags & NIF_STATE) &&
                has(offsetof(NotifyIconData32, dwStateMask), sizeof(DWORD)) &&
                (nid.dwStateMask & NIS_HIDDEN)) {
                it->hidden = nid.dwState & NIS_HIDDEN;
            }
            copy = *it;
            send = true;
            break;
        }
        case NIM_SETVERSION:
            if (it != g_icons.end() &&
                has(offsetof(NotifyIconData32, uVersion), sizeof(UINT))) {
                it->version = nid.uVersion;
                copy = *it;
                send = true;
            }
            break;
        case NIM_DELETE:
            if (it != g_icons.end()) {
                copy = *it;
                copy.icon = nullptr;
                if (it->icon) DestroyIcon(it->icon);
                g_icons.erase(it);
                op = 1;
                send = true;
            }
            break;
    }
    std::erase_if(g_icons, [](const Icon& i) {
        if (IsWindow(i.owner)) return false;
        if (i.icon) DestroyIcon(i.icon);
        return true;
    });
    ReleaseSRWLockExclusive(&g_lock);

    if (send) {
        if (HWND island = FindIsland()) Send(island, &copy, op);
    }
}

LRESULT CALLBACK TaskbarSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                     UINT_PTR, DWORD_PTR) {
    if (msg == WM_COPYDATA) {
        OnTrayNotify((const COPYDATASTRUCT*)lp);
    } else if (msg == g_msgRequestTray && msg) {
        SendAll();
        return 0;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Runs a function on the thread that owns a window (needed to subclass it).
struct RunCall {
    void (*fn)(void*);
    void* param;
};

LRESULT CALLBACK RunHookProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        auto cwp = (const CWPSTRUCT*)lp;
        if (cwp->message == g_runMsg && cwp->wParam == 0x4C49) {
            auto call = (RunCall*)cwp->lParam;
            call->fn(call->param);
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

bool RunFromWindowThread(HWND hwnd, void (*fn)(void*), void* param) {
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        fn(param);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, RunHookProc, nullptr, tid);
    if (!hook) return false;
    RunCall call{fn, param};
    SendMessage(hwnd, g_runMsg, 0x4C49, (LPARAM)&call);
    UnhookWindowsHookEx(hook);
    return true;
}

// Icons whose apps never re-add them when asked (Spotify) survive a reload.
void SaveKept() {
    std::wstring value;
    AcquireSRWLockShared(&g_lock);
    for (const auto& i : g_icons) {
        WCHAR guid[64] = L"";
        StringFromGUID2(i.guid, guid, ARRAYSIZE(guid));
        std::wstring tip = i.tip.substr(0, 64);
        std::replace_if(tip.begin(), tip.end(),
                        [](WCHAR c) { return c == L'\t' || c == L'\n' || c == L'\r'; },
                        L' ');
        std::wstring line = std::to_wstring((UINT64)(ULONG_PTR)i.owner) + L"\t" +
                            std::to_wstring(i.id) + L"\t" + guid + L"\t" +
                            std::to_wstring(i.hasGuid) + L"\t" +
                            std::to_wstring(i.callback) + L"\t" +
                            std::to_wstring(i.version) + L"\t" +
                            std::to_wstring(i.hidden) + L"\t" +
                            std::to_wstring(i.pid) + L"\t" + tip + L"\n";
        if (value.size() + line.size() > 16000) break;
        value += line;
    }
    ReleaseSRWLockShared(&g_lock);
    Wh_SetStringValue(L"trayKept", value.c_str());
}

void LoadKept() {
    WCHAR buffer[16384];
    if (!Wh_GetStringValue(L"trayKept", buffer, ARRAYSIZE(buffer))) return;
    std::wstring all = buffer;
    size_t start = 0;
    AcquireSRWLockExclusive(&g_lock);
    while (start < all.size()) {
        size_t end = all.find(L'\n', start);
        if (end == std::wstring::npos) end = all.size();
        std::wstring line = all.substr(start, end - start);
        start = end + 1;
        std::vector<std::wstring> f;
        size_t p = 0;
        while (f.size() < 8) {
            size_t t = line.find(L'\t', p);
            if (t == std::wstring::npos) break;
            f.push_back(line.substr(p, t - p));
            p = t + 1;
        }
        if (f.size() < 8) continue;
        Icon i;
        i.owner = (HWND)(ULONG_PTR)_wcstoui64(f[0].c_str(), nullptr, 10);
        i.id = wcstoul(f[1].c_str(), nullptr, 10);
        CLSIDFromString(f[2].c_str(), &i.guid);
        i.hasGuid = f[3] == L"1";
        i.callback = wcstoul(f[4].c_str(), nullptr, 10);
        i.version = kVersionUnknown;
        i.hidden = f[6] == L"1";
        i.pid = wcstoul(f[7].c_str(), nullptr, 10);
        i.tip = line.substr(p);
        DWORD pid = 0;
        if (!IsWindow(i.owner) ||
            !GetWindowThreadProcessId(i.owner, &pid) || pid != i.pid) {
            continue;
        }
        if (HANDLE process =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
            WCHAR path[MAX_PATH];
            DWORD len = ARRAYSIZE(path);
            if (QueryFullProcessImageName(process, 0, path, &len)) {
                ExtractIconEx(path, 0, nullptr, &i.icon, 1);
            }
            CloseHandle(process);
        }
        g_icons.push_back(i);
    }
    ReleaseSRWLockExclusive(&g_lock);
    Wh_SetStringValue(L"trayKept", L"");
}

void SubclassOnThread(void*) {
    g_subclassed = SetWindowSubclass(g_taskbar, TaskbarSubclassProc, 0, 0);
}

void UnsubclassOnThread(void*) {
    RemoveWindowSubclass(g_taskbar, TaskbarSubclassProc, 0);
    g_subclassed = false;
}

HWND FindOwnTaskbar() {
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (taskbar && GetWindowThreadProcessId(taskbar, &pid) &&
        pid == GetCurrentProcessId()) {
        return taskbar;
    }
    return nullptr;
}

DWORD WINAPI WaitThreadProc(void*) {
    // The taskbar may not exist yet when explorer is starting.
    for (int i = 0; i < 600; i++) {
        if (HWND taskbar = FindOwnTaskbar()) {
            g_taskbar = taskbar;
            break;
        }
        if (WaitForSingleObject(g_stop, 200) == WAIT_OBJECT_0) return 0;
    }
    if (!g_taskbar || !RunFromWindowThread(g_taskbar, SubclassOnThread, nullptr) ||
        !g_subclassed) {
        Wh_Log(L"Couldn't listen to the taskbar");
        return 0;
    }
    LoadKept();
    if (GetStr(L"elements.tray") != L"off") {
        // Ask every app to add its tray icons again, so they pass by here.
        PostMessage(HWND_BROADCAST, g_msgTaskbarCreated, 0, 0);
    }
    SendAll();
    return 0;
}

BOOL Init() {
    g_runMsg = RegisterWindowMessage(L"LiteIsland_Run_" WH_MOD_ID);
    g_stop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    return g_stop != nullptr;
}

void AfterInit() {
    g_waitThread = CreateThread(nullptr, 0, WaitThreadProc, nullptr, 0, nullptr);
}

void BeforeUninit() {
    SetEvent(g_stop);
    if (g_waitThread) {
        WaitForSingleObject(g_waitThread, INFINITE);
        CloseHandle(g_waitThread);
        g_waitThread = nullptr;
    }
    if (g_subclassed && IsWindow(g_taskbar)) {
        RunFromWindowThread(g_taskbar, UnsubclassOnThread, nullptr);
    }
    SaveKept();
    AcquireSRWLockExclusive(&g_lock);
    for (auto& i : g_icons) {
        if (i.icon) DestroyIcon(i.icon);
    }
    g_icons.clear();
    ReleaseSRWLockExclusive(&g_lock);
}

}  // namespace ex

////////////////////////////////////////////////////////////////////////////////
// Island process: settings and shared state

constexpr UINT WM_APP_MEDIA = WM_APP + 1;
constexpr UINT WM_APP_NOTIF = WM_APP + 2;
constexpr UINT WM_APP_VOLUME = WM_APP + 3;
constexpr UINT WM_APP_AUDIODEV = WM_APP + 4;
constexpr UINT WM_APP_SETTINGS = WM_APP + 5;
constexpr UINT WM_APP_APPBAR = WM_APP + 6;
constexpr UINT WM_APP_REFRESH = WM_APP + 7;   // async data arrived
constexpr UINT WM_APP_WEATHER = WM_APP + 8;
constexpr UINT WM_APP_TOAST = WM_APP + 9;
constexpr UINT WM_APP_LYRICS = WM_APP + 10;

std::atomic<HWND> g_hwnd{nullptr};
HANDLE g_stopEvent;
HANDLE g_workEvent;
HANDLE g_uiThread;
HANDLE g_workerThread;

void PostToUi(UINT msg, WPARAM wp = 0, LPARAM lp = 0) {
    if (HWND hwnd = g_hwnd.load()) PostMessage(hwnd, msg, wp, lp);
}

enum class BgMode { Solid, Accent, Glass, AlbumArt };
enum class TrayMode { Inline, Panel, Off };

struct Shortcut {
    std::wstring target, args;
    WCHAR glyph = 0;
    ComPtr<ID2D1Bitmap> icon;
};

struct Settings {
    bool power = true, notif = true, controlCenter = true, volume = true;
    bool network = false, battery = true, weather = true, clock = true;
    bool media = true, mediaControls = true, visualizer = true;
    bool collapseButton = true;
    TrayMode tray = TrayMode::Inline;

    BgMode bgMode = BgMode::AlbumArt, fallback = BgMode::Solid;
    bool artWhenPaused = true, artStretch = false, artAutoDim = true;
    bool panelArt = true, glassAcrylic = true;
    float opacity = 0.96f, artDim = 0.55f, artSaturation = 1.4f;
    int artBlur = 50;

    D2D1_COLOR_F bg{}, text{}, icons{}, accent{}, border{};
    bool systemAccent = true;
    float borderWidth = 1;

    float height = 34, radius = 17, paddingX = 8, spacing = 6;
    float minWidth = 120, maxWidth = 1200, topOffset = 6;
    float fontSize = 13, iconSize = 15;
    std::wstring fontFamily;
    bool boldClock = true;

    bool seconds = true, date = true;
    std::wstring timeFormat, dateFormat;

    bool mediaArt = true, mediaArtist = true, lyrics = false;
    float mediaMaxText = 220;

    bool popNotif = true, popTrack = true, popCharging = true, popVolume = true;
    int popSeconds = 5;
    int monitor = 0;

    std::wstring city;
    bool celsius = false;

    int vizBars = 6;
    bool vizAccent = false;

    std::vector<Shortcut> shortcuts;

    bool scrollVolume = true;
    int volumeStep = 2;
    bool animations = true, startFolded = false, peek = true;
    bool hideFullscreen = true, reserveSpace = false;
};

Settings g_s;
std::atomic<bool> g_wantMedia{false}, g_wantArt{false}, g_wantNotif{false};

bool ParseColor(std::wstring str, D2D1_COLOR_F* out) {
    if (!str.empty() && str[0] == L'#') str.erase(0, 1);
    if (str.size() != 6 && str.size() != 8) return false;
    WCHAR* end;
    unsigned long v = wcstoul(str.c_str(), &end, 16);
    if (*end) return false;
    float a = str.size() == 8 ? ((v >> 24) & 0xFF) / 255.f : 1.f;
    *out = D2D1::ColorF(((v >> 16) & 0xFF) / 255.f, ((v >> 8) & 0xFF) / 255.f,
                        (v & 0xFF) / 255.f, a);
    return true;
}

D2D1_COLOR_F ColorSetting(PCWSTR name, D2D1_COLOR_F fallback) {
    D2D1_COLOR_F c;
    return ParseColor(GetStr(name), &c) ? c : fallback;
}

BgMode ParseBgMode(const std::wstring& v, BgMode def) {
    if (v == L"solid") return BgMode::Solid;
    if (v == L"accent") return BgMode::Accent;
    if (v == L"glass") return BgMode::Glass;
    if (v == L"albumArt") return BgMode::AlbumArt;
    return def;
}

float IntSetting(PCWSTR name, int lo, int hi) {
    return (float)std::clamp(Wh_GetIntSetting(name), lo, hi);
}

void LoadSettings() {
    Settings s;
    s.power = Wh_GetIntSetting(L"elements.power");
    s.notif = Wh_GetIntSetting(L"elements.notifications");
    std::wstring tray = GetStr(L"elements.tray");
    s.tray = tray == L"panel" ? TrayMode::Panel
             : tray == L"off" ? TrayMode::Off
                              : TrayMode::Inline;
    s.controlCenter = Wh_GetIntSetting(L"elements.controlCenter");
    s.volume = Wh_GetIntSetting(L"elements.volume");
    s.network = Wh_GetIntSetting(L"elements.network");
    s.battery = Wh_GetIntSetting(L"elements.battery");
    s.weather = Wh_GetIntSetting(L"elements.weather");
    s.clock = Wh_GetIntSetting(L"elements.clock");
    s.media = Wh_GetIntSetting(L"elements.media");
    s.mediaControls = Wh_GetIntSetting(L"elements.mediaControls");
    s.visualizer = Wh_GetIntSetting(L"elements.visualizer");
    s.collapseButton = Wh_GetIntSetting(L"elements.collapseButton");

    s.bgMode = ParseBgMode(GetStr(L"background.mode"), BgMode::AlbumArt);
    s.fallback = ParseBgMode(GetStr(L"background.fallback"), BgMode::Solid);
    if (s.fallback == BgMode::AlbumArt) s.fallback = BgMode::Solid;
    s.artWhenPaused = Wh_GetIntSetting(L"background.artWhenPaused");
    s.artStretch = GetStr(L"background.artFit") == L"stretch";
    s.opacity = IntSetting(L"background.opacity", 0, 100) / 100.f;
    s.artBlur = (int)IntSetting(L"background.artBlur", 0, 100);
    s.artDim = IntSetting(L"background.artDim", 0, 95) / 100.f;
    s.artAutoDim = Wh_GetIntSetting(L"background.artAutoDim");
    s.artSaturation = IntSetting(L"background.artSaturation", 0, 300) / 100.f;
    s.panelArt = Wh_GetIntSetting(L"background.panelArt");
    s.glassAcrylic = GetStr(L"background.glassType") != L"blur";

    s.bg = ColorSetting(L"colors.background", D2D1::ColorF(0x0C0C0E));
    s.text = ColorSetting(L"colors.text", D2D1::ColorF(D2D1::ColorF::White));
    s.icons = ColorSetting(L"colors.icons", D2D1::ColorF(D2D1::ColorF::White));
    s.systemAccent = !ParseColor(GetStr(L"colors.accent"), &s.accent);
    s.border = ColorSetting(L"colors.border", D2D1::ColorF(1, 1, 1, 0.12f));
    s.borderWidth = IntSetting(L"colors.borderWidth", 0, 10);

    s.height = IntSetting(L"size.height", 22, 120);
    s.radius = IntSetting(L"size.cornerRadius", 0, 100);
    s.paddingX = IntSetting(L"size.paddingX", 0, 100);
    s.spacing = IntSetting(L"size.spacing", 0, 100);
    s.minWidth = IntSetting(L"size.minWidth", 40, 6000);
    s.maxWidth = std::max(s.minWidth, IntSetting(L"size.maxWidth", 80, 6000));
    s.topOffset = IntSetting(L"size.topOffset", -50, 500);
    s.fontSize = IntSetting(L"size.fontSize", 6, 60);
    s.iconSize = IntSetting(L"size.iconSize", 6, 60);
    s.fontFamily = GetStr(L"size.fontFamily");
    if (s.fontFamily.empty()) s.fontFamily = L"Segoe UI Variable Text";
    s.boldClock = Wh_GetIntSetting(L"size.boldClock");

    s.seconds = Wh_GetIntSetting(L"clock.seconds");
    s.date = Wh_GetIntSetting(L"clock.date");
    s.timeFormat = GetStr(L"clock.timeFormat");
    s.dateFormat = GetStr(L"clock.dateFormat");

    s.mediaArt = Wh_GetIntSetting(L"media.art");
    s.mediaArtist = Wh_GetIntSetting(L"media.artist");
    s.mediaMaxText = IntSetting(L"media.maxTextWidth", 40, 2000);
    s.lyrics = Wh_GetIntSetting(L"media.lyrics");

    s.popNotif = Wh_GetIntSetting(L"popups.notifications");
    s.popTrack = Wh_GetIntSetting(L"popups.track");
    s.popCharging = Wh_GetIntSetting(L"popups.charging");
    s.popVolume = Wh_GetIntSetting(L"popups.volume");
    s.popSeconds = (int)IntSetting(L"popups.seconds", 1, 60);
    s.monitor = (int)IntSetting(L"behavior.monitor", 0, 16);

    s.city = GetStr(L"weather.city");
    s.celsius = GetStr(L"weather.units") == L"celsius";

    s.vizBars = (int)IntSetting(L"visualizer.bars", 3, 16);
    s.vizAccent = GetStr(L"visualizer.color") == L"accent";

    for (int i = 0; i < 32; i++) {
        PCWSTR target = Wh_GetStringSetting(L"shortcuts[%d].target", i);
        Shortcut sc;
        bool exists = target && *target;
        if (exists) sc.target = target;
        Wh_FreeStringSetting(target);
        if (!exists) {
            PCWSTR next = Wh_GetStringSetting(L"shortcuts[%d].target", i + 1);
            bool more = next && *next;
            Wh_FreeStringSetting(next);
            if (!more) break;
            continue;
        }
        PCWSTR args = Wh_GetStringSetting(L"shortcuts[%d].args", i);
        sc.args = args ? args : L"";
        Wh_FreeStringSetting(args);
        PCWSTR glyph = Wh_GetStringSetting(L"shortcuts[%d].glyph", i);
        if (glyph && *glyph) sc.glyph = (WCHAR)wcstoul(glyph, nullptr, 16);
        Wh_FreeStringSetting(glyph);
        s.shortcuts.push_back(std::move(sc));
    }

    s.scrollVolume = Wh_GetIntSetting(L"behavior.scrollVolume");
    s.volumeStep = (int)IntSetting(L"behavior.volumeStep", 1, 25);
    s.animations = Wh_GetIntSetting(L"behavior.animations");
    s.startFolded = Wh_GetIntSetting(L"behavior.startFolded");
    s.peek = Wh_GetIntSetting(L"behavior.peek");
    s.hideFullscreen = Wh_GetIntSetting(L"behavior.hideFullscreen");
    s.reserveSpace = Wh_GetIntSetting(L"behavior.reserveSpace");

    g_s = std::move(s);
    g_wantMedia = g_s.media || g_s.visualizer;
    g_wantArt = g_s.media || g_s.visualizer;
    g_wantNotif = g_s.notif || g_s.popNotif;
    SetEvent(g_workEvent);
}

////////////////////////////////////////////////////////////////////////////////
// Media sessions (worker thread)

constexpr int kArtSize = 160;

struct MediaSnapshot {
    bool hasSession = false, playing = false, canPrev = false, canNext = false;
    bool canSeek = false, canShuffle = false, shuffle = false;
    bool canRepeat = false;
    int repeat = 0;   // 0 off, 1 one, 2 all
    std::wstring title, artist, album, app;
    double position = 0, duration = 0;
    ULONGLONG positionTick = 0;
    std::vector<uint32_t> art;   // kArtSize^2 PBGRA
    unsigned artVersion = 0;
};

SRWLOCK g_mediaLock = SRWLOCK_INIT;
MediaSnapshot g_mediaShared;
std::atomic<int> g_mediaCommand{0};   // 1 prev 2 toggle 3 next 4 seek 5 shuffle 6 repeat
std::atomic<double> g_seekTarget{0};

bool DecodeSquare(IStream* stream, int size, std::vector<uint32_t>& out) {
    ComPtr<IWICImagingFactory> wic;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICBitmapClipper> clipper;
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICFormatConverter> converter;
    UINT w, h;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic))) ||
        FAILED(wic->CreateDecoderFromStream(stream, nullptr,
                                            WICDecodeMetadataCacheOnDemand,
                                            &decoder)) ||
        FAILED(decoder->GetFrame(0, &frame)) ||
        FAILED(frame->GetSize(&w, &h)) || !w || !h) {
        return false;
    }
    UINT side = std::min(w, h);
    WICRect crop{(INT)(w - side) / 2, (INT)(h - side) / 2, (INT)side,
                 (INT)side};
    if (FAILED(wic->CreateBitmapClipper(&clipper)) ||
        FAILED(clipper->Initialize(frame.Get(), &crop)) ||
        FAILED(wic->CreateBitmapScaler(&scaler)) ||
        FAILED(scaler->Initialize(clipper.Get(), size, size,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(scaler.Get(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeCustom))) {
        return false;
    }
    out.resize((size_t)size * size);
    return SUCCEEDED(converter->CopyPixels(nullptr, size * 4, size * size * 4,
                                           (BYTE*)out.data()));
}

bool LoadThumbnail(
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference const& ref,
    std::vector<uint32_t>& out) {
    auto stream = ref.OpenReadAsync().get();
    ComPtr<IStream> istream;
    if (!stream ||
        FAILED(CreateStreamOverRandomAccessStream(
            reinterpret_cast<::IUnknown*>(winrt::get_abi(stream)),
            IID_PPV_ARGS(&istream)))) {
        return false;
    }
    return DecodeSquare(istream.Get(), kArtSize, out);
}

double ToSeconds(winrt::Windows::Foundation::TimeSpan t) {
    return std::chrono::duration<double>(t).count();
}

class MediaWatcher {
   public:
    bool Active() const { return m_manager != nullptr; }
    bool HasSession() const { return m_last.hasSession; }

    bool Start() {
        try {
            m_manager = wmc::GlobalSystemMediaTransportControlsSessionManager::
                            RequestAsync()
                                .get();
        } catch (...) {
            m_manager = nullptr;
        }
        if (!m_manager) return false;
        auto wake = [](auto&&, auto&&) { SetEvent(g_workEvent); };
        m_sessionsToken = m_manager.SessionsChanged(wake);
        m_currentToken = m_manager.CurrentSessionChanged(wake);
        return true;
    }

    void Stop() {
        if (!m_manager) return;
        try {
            Unsubscribe();
            m_manager.SessionsChanged(m_sessionsToken);
            m_manager.CurrentSessionChanged(m_currentToken);
        } catch (...) {
        }
        m_manager = nullptr;
        m_artKey.clear();
        m_art.clear();
        Publish(MediaSnapshot{});
    }

    void Refresh() {
        if (!m_manager) return;
        MediaSnapshot snap;
        try {
            Resubscribe();
            auto session = PickSession();
            if (session) {
                auto info = session.GetPlaybackInfo();
                auto controls = info.Controls();
                snap.playing =
                    info.PlaybackStatus() ==
                    wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                        Playing;
                snap.canPrev = controls.IsPreviousEnabled();
                snap.canNext = controls.IsNextEnabled();
                snap.canSeek = controls.IsPlaybackPositionEnabled();
                snap.canShuffle = controls.IsShuffleEnabled();
                snap.canRepeat = controls.IsRepeatEnabled();
                if (auto shuffle = info.IsShuffleActive()) {
                    snap.shuffle = shuffle.Value();
                }
                if (auto repeat = info.AutoRepeatMode()) {
                    auto mode = repeat.Value();
                    snap.repeat =
                        mode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track
                            ? 1
                        : mode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List
                            ? 2
                            : 0;
                }
                snap.app = session.SourceAppUserModelId();
                auto timeline = session.GetTimelineProperties();
                double start = ToSeconds(timeline.StartTime());
                double end = ToSeconds(timeline.EndTime());
                double pos = ToSeconds(timeline.Position()) - start;
                if (snap.playing) {
                    auto age = winrt::clock::now() - timeline.LastUpdatedTime();
                    double ageSec = std::chrono::duration<double>(age).count();
                    if (ageSec > 0 && ageSec < 24 * 3600) pos += ageSec;
                }
                snap.duration = std::max(0.0, end - start);
                snap.position = std::clamp(pos, 0.0, std::max(snap.duration, pos));
                snap.positionTick = GetTickCount64();

                auto props = session.TryGetMediaPropertiesAsync().get();
                if (props) {
                    snap.title = props.Title();
                    snap.artist = props.Artist();
                    snap.album = props.AlbumTitle();
                    snap.hasSession = !snap.title.empty();
                    UpdateArt(session, props);
                }
            }
        } catch (...) {
            return;
        }
        if (!snap.hasSession) {
            m_artKey.clear();
            m_art.clear();
        }
        snap.art = m_art;
        snap.artVersion = m_artVersion;
        Publish(std::move(snap));
    }

    void RunCommand(int command) {
        try {
            auto session = PickSession();
            if (!session) return;
            auto info = session.GetPlaybackInfo();
            switch (command) {
                case 1: session.TrySkipPreviousAsync().get(); break;
                case 2: session.TryTogglePlayPauseAsync().get(); break;
                case 3: session.TrySkipNextAsync().get(); break;
                case 4: {
                    auto start = session.GetTimelineProperties().StartTime();
                    auto target = start + std::chrono::duration_cast<
                                              winrt::Windows::Foundation::TimeSpan>(
                                              std::chrono::duration<double>(
                                                  g_seekTarget.load()));
                    session.TryChangePlaybackPositionAsync(target.count()).get();
                    break;
                }
                case 5: {
                    auto shuffle = info.IsShuffleActive();
                    session.TryChangeShuffleActiveAsync(!(shuffle && shuffle.Value()))
                        .get();
                    break;
                }
                case 6: {
                    using Mode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                    auto repeat = info.AutoRepeatMode();
                    Mode mode = repeat ? repeat.Value() : Mode::None;
                    Mode next = mode == Mode::None    ? Mode::List
                                : mode == Mode::List ? Mode::Track
                                                     : Mode::None;
                    session.TryChangeAutoRepeatModeAsync(next).get();
                    break;
                }
            }
        } catch (...) {
        }
    }

   private:
    struct Sub {
        wmc::GlobalSystemMediaTransportControlsSession session{nullptr};
        winrt::event_token playback, props, timeline;
    };

    wmc::GlobalSystemMediaTransportControlsSession PickSession() {
        for (auto const& s : m_manager.GetSessions()) {
            if (s.GetPlaybackInfo().PlaybackStatus() ==
                wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                    Playing) {
                return s;
            }
        }
        return m_manager.GetCurrentSession();
    }

    void Resubscribe() {
        auto sessions = m_manager.GetSessions();
        bool same = sessions.Size() == m_subs.size();
        for (uint32_t i = 0; same && i < sessions.Size(); i++) {
            same = sessions.GetAt(i) == m_subs[i].session;
        }
        if (same) return;
        Unsubscribe();
        auto wake = [](auto&&, auto&&) { SetEvent(g_workEvent); };
        for (auto const& s : sessions) {
            Sub sub;
            sub.session = s;
            sub.playback = s.PlaybackInfoChanged(wake);
            sub.props = s.MediaPropertiesChanged(wake);
            sub.timeline = s.TimelinePropertiesChanged(wake);
            m_subs.push_back(std::move(sub));
        }
    }

    void Unsubscribe() {
        for (auto& sub : m_subs) {
            try {
                sub.session.PlaybackInfoChanged(sub.playback);
                sub.session.MediaPropertiesChanged(sub.props);
                sub.session.TimelinePropertiesChanged(sub.timeline);
            } catch (...) {
            }
        }
        m_subs.clear();
    }

    // Some apps send the track first and the cover a moment later.
    void UpdateArt(
        wmc::GlobalSystemMediaTransportControlsSession const& session,
        wmc::GlobalSystemMediaTransportControlsSessionMediaProperties const&
            props) {
        if (!g_wantArt) {
            if (!m_art.empty()) {
                m_art.clear();
                m_artVersion++;
            }
            m_artKey.clear();
            return;
        }
        std::wstring key = std::wstring(session.SourceAppUserModelId()) +
                           L"\n" + std::wstring(props.Title()) + L"\n" +
                           std::wstring(props.Artist()) + L"\n" +
                           std::wstring(props.AlbumTitle());
        if (key != m_artKey) {
            m_artKey = key;
            m_artTries = 0;
            if (!m_art.empty()) {
                m_art.clear();
                m_artVersion++;
            }
        }
        auto thumb = props.Thumbnail();
        if (m_art.empty() && thumb && m_artTries < 3) {
            m_artTries++;
            std::vector<uint32_t> pixels;
            try {
                if (LoadThumbnail(thumb, pixels)) {
                    m_art = std::move(pixels);
                    m_artVersion++;
                }
            } catch (...) {
            }
        }
    }

    void Publish(MediaSnapshot snap) {
        const MediaSnapshot& l = m_last;
        bool posJump = std::abs((snap.position) -
                                (l.position + (l.playing ? (snap.positionTick -
                                                            l.positionTick) /
                                                               1000.0
                                                         : 0))) > 1.5;
        if (snap.hasSession == l.hasSession && snap.playing == l.playing &&
            snap.canPrev == l.canPrev && snap.canNext == l.canNext &&
            snap.canSeek == l.canSeek && snap.shuffle == l.shuffle &&
            snap.repeat == l.repeat && snap.canShuffle == l.canShuffle &&
            snap.canRepeat == l.canRepeat && snap.title == l.title &&
            snap.artist == l.artist && snap.app == l.app &&
            snap.duration == l.duration && !posJump &&
            snap.artVersion == l.artVersion) {
            return;
        }
        m_last = snap;
        m_last.art.clear();
        AcquireSRWLockExclusive(&g_mediaLock);
        g_mediaShared = std::move(snap);
        ReleaseSRWLockExclusive(&g_mediaLock);
        PostToUi(WM_APP_MEDIA);
    }

    wmc::GlobalSystemMediaTransportControlsSessionManager m_manager{nullptr};
    winrt::event_token m_sessionsToken{}, m_currentToken{};
    std::vector<Sub> m_subs;
    std::wstring m_artKey;
    std::vector<uint32_t> m_art;
    unsigned m_artVersion = 0;
    int m_artTries = 0;
    MediaSnapshot m_last;
};

////////////////////////////////////////////////////////////////////////////////
// Notifications: count and last app, from Windows' notification database

std::atomic<int> g_notifCount{0};
SRWLOCK g_notifLock = SRWLOCK_INIT;
std::wstring g_notifApp;   // AppUserModelID of the latest notification

struct Toast {
    std::wstring app, title, body;
};
std::vector<Toast> g_toasts;   // new ones for the island to pop out

std::wstring Utf8ToWide(const std::string& s) {
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

std::wstring XmlUnescape(const std::wstring& s) {
    std::wstring out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] != L'&') {
            out += s[i];
            continue;
        }
        size_t semi = s.find(L';', i);
        if (semi == std::wstring::npos || semi - i > 10) {
            out += s[i];
            continue;
        }
        std::wstring e = s.substr(i + 1, semi - i - 1);
        if (e == L"amp") out += L'&';
        else if (e == L"lt") out += L'<';
        else if (e == L"gt") out += L'>';
        else if (e == L"quot") out += L'"';
        else if (e == L"apos") out += L'\'';
        else if (!e.empty() && e[0] == L'#') {
            unsigned long c = e.size() > 1 && (e[1] == L'x' || e[1] == L'X')
                                  ? wcstoul(e.c_str() + 2, nullptr, 16)
                                  : wcstoul(e.c_str() + 1, nullptr, 10);
            if (c > 0xFFFF) {
                c -= 0x10000;
                out += (WCHAR)(0xD800 + (c >> 10));
                out += (WCHAR)(0xDC00 + (c & 0x3FF));
            } else if (c) {
                out += (WCHAR)c;
            }
        } else {
            out += s.substr(i, semi - i + 1);
        }
        i = semi;
    }
    return out;
}

// The <text> elements of a toast's XML: the first is the title.
void ParseToast(const std::string& xml, std::wstring& title, std::wstring& body) {
    std::vector<std::wstring> texts;
    size_t pos = 0;
    while ((pos = xml.find("<text", pos)) != std::string::npos) {
        char next = pos + 5 < xml.size() ? xml[pos + 5] : 0;
        size_t tagEnd = xml.find('>', pos);
        if (tagEnd == std::string::npos) break;
        if ((next != ' ' && next != '>') || xml[tagEnd - 1] == '/') {
            pos = tagEnd;
            continue;
        }
        size_t close = xml.find("</text>", tagEnd);
        if (close == std::string::npos) break;
        std::string tag = xml.substr(pos, tagEnd - pos);
        if (tag.find("attribution") == std::string::npos) {
            std::wstring t = XmlUnescape(Utf8ToWide(xml.substr(tagEnd + 1, close - tagEnd - 1)));
            if (!t.empty()) texts.push_back(t);
        }
        pos = close + 7;
    }
    if (!texts.empty()) title = texts[0];
    for (size_t i = 1; i < texts.size(); i++) {
        if (!body.empty()) body += L"  ";
        body += texts[i];
    }
    for (auto* s : {&title, &body}) {
        std::replace_if(s->begin(), s->end(), [](WCHAR c) { return c == L'\n' || c == L'\r'; },
                        L' ');
    }
}

class NotifWatcher {
   public:
    bool Active() const { return m_active; }
    HANDLE ChangeHandle() const { return m_change; }

    void Start() {
        if (m_active) return;
        m_active = true;
        m_lastCount = -1;
        m_lastApp.clear();
        if (!m_sqlite) {
            m_sqlite = LoadLibraryEx(L"winsqlite3.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (m_sqlite) {
                m_open = (open_t)GetProcAddress(m_sqlite, "sqlite3_open_v2");
                m_prepare =
                    (prepare_t)GetProcAddress(m_sqlite, "sqlite3_prepare_v2");
                m_step = (simple_t)GetProcAddress(m_sqlite, "sqlite3_step");
                m_int = (int_t)GetProcAddress(m_sqlite, "sqlite3_column_int64");
                m_text =
                    (text_t)GetProcAddress(m_sqlite, "sqlite3_column_text16");
                m_finalize =
                    (simple_t)GetProcAddress(m_sqlite, "sqlite3_finalize");
                m_close = (simple_t)GetProcAddress(m_sqlite, "sqlite3_close");
                m_busy = (busy_t)GetProcAddress(m_sqlite, "sqlite3_busy_timeout");
                m_bind = (bind_t)GetProcAddress(m_sqlite, "sqlite3_bind_int64");
                m_blob = (text_t)GetProcAddress(m_sqlite, "sqlite3_column_blob");
                m_bytes = (bytes_t)GetProcAddress(m_sqlite, "sqlite3_column_bytes");
            }
        }
        WCHAR dir[MAX_PATH];
        if (ExpandEnvironmentStrings(
                L"%LOCALAPPDATA%\\Microsoft\\Windows\\Notifications", dir,
                ARRAYSIZE(dir))) {
            std::wstring db = std::wstring(dir) + L"\\wpndatabase.db";
            int n = WideCharToMultiByte(CP_UTF8, 0, db.c_str(), -1, nullptr, 0,
                                        nullptr, nullptr);
            m_path.resize(n);
            WideCharToMultiByte(CP_UTF8, 0, db.c_str(), -1, m_path.data(), n,
                                nullptr, nullptr);
            m_change = FindFirstChangeNotification(
                dir, FALSE,
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_FILE_NAME);
            if (m_change == INVALID_HANDLE_VALUE) m_change = nullptr;
        }
        Query();
    }

    void Stop() {
        if (m_change) {
            FindCloseChangeNotification(m_change);
            m_change = nullptr;
        }
        m_active = false;
    }

    void Unload() {
        Stop();
        if (m_sqlite) {
            FreeLibrary(m_sqlite);
            m_sqlite = nullptr;
        }
    }

    void OnChange() {
        if (m_change) FindNextChangeNotification(m_change);
    }

    void Query() {
        if (!m_active || !m_open || !m_prepare || !m_step || !m_int ||
            !m_text || !m_finalize || !m_close || !m_busy || m_path.empty()) {
            return;
        }
        void* db = nullptr;
        if (m_open(m_path.c_str(), &db, 1 /* read-only */, nullptr) != 0) {
            if (db) m_close(db);
            return;
        }
        m_busy(db, 300);
        int count = -1;
        std::wstring app;
        void* stmt = nullptr;
        if (m_prepare(db,
                      "SELECT COUNT(*) FROM Notification WHERE Type = 'toast'",
                      -1, &stmt, nullptr) == 0) {
            if (m_step(stmt) == 100) count = (int)m_int(stmt, 0);
            m_finalize(stmt);
        }
        if (m_prepare(db,
                      "SELECT h.PrimaryId FROM Notification n JOIN "
                      "NotificationHandler h ON h.RecordId = n.HandlerId "
                      "WHERE n.Type = 'toast' ORDER BY n.ArrivalTime DESC "
                      "LIMIT 1",
                      -1, &stmt, nullptr) == 0) {
            if (m_step(stmt) == 100) {
                if (auto text = (PCWSTR)m_text(stmt, 0)) app = text;
            }
            m_finalize(stmt);
        }
        if (m_lastId < 0) {
            if (m_prepare(db, "SELECT MAX(Id) FROM Notification", -1, &stmt, nullptr) == 0) {
                if (m_step(stmt) == 100) m_lastId = m_int(stmt, 0);
                m_finalize(stmt);
            }
        } else if (m_bind && m_blob && m_bytes &&
                   m_prepare(db,
                             "SELECT n.Id, h.PrimaryId, n.Payload FROM Notification n "
                             "JOIN NotificationHandler h ON h.RecordId = n.HandlerId "
                             "WHERE n.Id > ?1 AND n.Type = 'toast' ORDER BY n.Id",
                             -1, &stmt, nullptr) == 0) {
            m_bind(stmt, 1, m_lastId);
            std::vector<Toast> toasts;
            while (m_step(stmt) == 100) {
                m_lastId = std::max(m_lastId, m_int(stmt, 0));
                Toast t;
                if (auto a = (PCWSTR)m_text(stmt, 1)) t.app = a;
                auto data = (const char*)m_blob(stmt, 2);
                int len = m_bytes(stmt, 2);
                if (data && len > 0) ParseToast(std::string(data, len), t.title, t.body);
                if (!t.title.empty() || !t.body.empty()) toasts.push_back(std::move(t));
            }
            m_finalize(stmt);
            if (!toasts.empty()) {
                AcquireSRWLockExclusive(&g_notifLock);
                for (auto& t : toasts) g_toasts.push_back(std::move(t));
                if (g_toasts.size() > 6) g_toasts.erase(g_toasts.begin(), g_toasts.end() - 6);
                ReleaseSRWLockExclusive(&g_notifLock);
                PostToUi(WM_APP_TOAST);
            }
        }
        m_close(db);
        if (count >= 0 && (count != m_lastCount || app != m_lastApp)) {
            m_lastCount = count;
            m_lastApp = app;
            g_notifCount = count;
            AcquireSRWLockExclusive(&g_notifLock);
            g_notifApp = app;
            ReleaseSRWLockExclusive(&g_notifLock);
            PostToUi(WM_APP_NOTIF);
        }
    }

   private:
    using open_t = int(WINAPI*)(const char*, void**, int, const char*);
    using prepare_t = int(WINAPI*)(void*, const char*, int, void**,
                                   const char**);
    using simple_t = int(WINAPI*)(void*);
    using int_t = long long(WINAPI*)(void*, int);
    using text_t = const void*(WINAPI*)(void*, int);
    using busy_t = int(WINAPI*)(void*, int);
    using bind_t = int(WINAPI*)(void*, int, long long);
    using bytes_t = int(WINAPI*)(void*, int);

    long long m_lastId = -1;
    bind_t m_bind = nullptr;
    text_t m_blob = nullptr;
    bytes_t m_bytes = nullptr;
    bool m_active = false;
    HANDLE m_change = nullptr;
    HMODULE m_sqlite = nullptr;
    std::string m_path;
    int m_lastCount = -1;
    std::wstring m_lastApp;
    open_t m_open = nullptr;
    prepare_t m_prepare = nullptr;
    simple_t m_step = nullptr, m_finalize = nullptr, m_close = nullptr;
    int_t m_int = nullptr;
    text_t m_text = nullptr;
    busy_t m_busy = nullptr;
};

DWORD WINAPI WorkerThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    {
        MediaWatcher media;
        NotifWatcher notif;
        ULONGLONG nextMediaTry = 0;
        bool notifPending = false;

        while (true) {
            if (g_wantMedia && !media.Active() &&
                GetTickCount64() >= nextMediaTry) {
                if (!media.Start()) nextMediaTry = GetTickCount64() + 10000;
            } else if (!g_wantMedia && media.Active()) {
                media.Stop();
            }
            if (g_wantNotif && !notif.Active()) {
                notif.Start();
            } else if (!g_wantNotif && notif.Active()) {
                notif.Stop();
            }

            if (int command = g_mediaCommand.exchange(0)) {
                media.RunCommand(command);
            }
            media.Refresh();
            if (notifPending) {
                notifPending = false;
                notif.Query();
            }

            HANDLE handles[3] = {g_stopEvent, g_workEvent};
            DWORD count = 2;
            if (notif.ChangeHandle()) handles[count++] = notif.ChangeHandle();

            // Events do the work; timeouts are safety nets.
            DWORD timeout = INFINITE;
            if (media.HasSession()) timeout = 5000;
            if (g_wantMedia && !media.Active()) timeout = 10000;
            if (notif.Active()) timeout = std::min<DWORD>(timeout, 15000);

            DWORD r = WaitForMultipleObjects(count, handles, FALSE, timeout);
            if (r == WAIT_OBJECT_0) break;
            if (r == WAIT_OBJECT_0 + 2) {
                notif.OnChange();
                if (WaitForSingleObject(g_stopEvent, 400) == WAIT_OBJECT_0) break;
                notifPending = true;
            } else if (r == WAIT_TIMEOUT) {
                notifPending = notif.Active();
            }
        }
        media.Stop();
        notif.Unload();
    }
    winrt::uninit_apartment();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Background jobs (thread pool): weather, radios, brightness

std::atomic<int> g_jobs{0};

void CALLBACK RunAsyncProc(PTP_CALLBACK_INSTANCE, void* context) {
    auto job = (std::function<void()>*)context;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        try {
            (*job)();
        } catch (...) {
        }
    }
    if (SUCCEEDED(hr)) CoUninitialize();
    delete job;
    g_jobs--;
}

void RunAsync(std::function<void()> fn) {
    auto job = new std::function<void()>(std::move(fn));
    g_jobs++;
    if (!TrySubmitThreadpoolCallback(RunAsyncProc, job, nullptr)) {
        delete job;
        g_jobs--;
    }
}

std::string HttpGet(const std::wstring& url) {
    std::string body;
    URL_COMPONENTS uc{sizeof(uc)};
    WCHAR host[256], path[2048];
    uc.lpszHostName = host;
    uc.dwHostNameLength = ARRAYSIZE(host);
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = ARRAYSIZE(path);
    WCHAR extra[2048];
    uc.lpszExtraInfo = extra;
    uc.dwExtraInfoLength = ARRAYSIZE(extra);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &uc)) return body;
    std::wstring object = std::wstring(path) + extra;
    HINTERNET session = WinHttpOpen(L"LiteIsland/2.0",
                                    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME,
                                    WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return body;
    WinHttpSetTimeouts(session, 5000, 5000, 10000, 10000);
    HINTERNET connect = WinHttpConnect(session, host, uc.nPort, 0);
    HINTERNET request =
        connect ? WinHttpOpenRequest(connect, L"GET", object.c_str(), nullptr,
                                     WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     uc.nScheme == INTERNET_SCHEME_HTTPS
                                         ? WINHTTP_FLAG_SECURE
                                         : 0)
                : nullptr;
    DWORD status = 0, size = sizeof(status);
    if (request &&
        WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(request, nullptr) &&
        WinHttpQueryHeaders(request,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                            WINHTTP_NO_HEADER_INDEX) &&
        status == 200) {
        DWORD available;
        while (WinHttpQueryDataAvailable(request, &available) && available &&
               body.size() < 1 << 20) {
            std::string chunk(available, '\0');
            DWORD read = 0;
            if (!WinHttpReadData(request, chunk.data(), available, &read) ||
                !read) {
                break;
            }
            body.append(chunk.data(), read);
        }
    }
    if (request) WinHttpCloseHandle(request);
    if (connect) WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return body;
}

std::wstring UrlEncode(const std::wstring& text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0,
                                nullptr, nullptr);
    std::string utf8(n > 0 ? n - 1 : 0, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, utf8.data(), n, nullptr,
                        nullptr);
    std::wstring out;
    for (unsigned char c : utf8) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out += (WCHAR)c;
        } else {
            WCHAR buf[4];
            swprintf_s(buf, L"%%%02X", c);
            out += buf;
        }
    }
    return out;
}

struct WeatherHour {
    std::wstring label;
    int code = 0;
    bool day = true;
    float temp = 0;
};
struct WeatherDay {
    std::wstring label;
    int code = 0;
    float lo = 0, hi = 0;
};
struct Weather {
    bool ok = false;
    std::wstring place;
    float temp = 0, feels = 0, wind = 0, lo = 0, hi = 0;
    int humidity = 0, code = 0;
    bool day = true;
    std::vector<WeatherHour> hours;
    std::vector<WeatherDay> days;
};

SRWLOCK g_weatherLock = SRWLOCK_INIT;
Weather g_weather;
std::atomic<bool> g_weatherBusy{false};

void FetchWeather(std::wstring city, bool celsius) {
    namespace json = winrt::Windows::Data::Json;
    double lat = 0, lon = 0;
    std::wstring place;
    if (!city.empty()) {
        std::string body = HttpGet(
            L"https://geocoding-api.open-meteo.com/v1/search?count=1&format=json&name=" +
            UrlEncode(city));
        json::JsonObject root{nullptr};
        if (body.empty() || !json::JsonObject::TryParse(winrt::to_hstring(body), root)) {
            return;
        }
        auto results = root.GetNamedArray(L"results", nullptr);
        if (!results || results.Size() == 0) return;
        auto first = results.GetObjectAt(0);
        lat = first.GetNamedNumber(L"latitude");
        lon = first.GetNamedNumber(L"longitude");
        place = first.GetNamedString(L"name", L"").c_str();
    } else {
        using namespace winrt::Windows::Devices::Geolocation;
        if (Geolocator::RequestAccessAsync().get() !=
            GeolocationAccessStatus::Allowed) {
            return;
        }
        Geolocator geo;
        auto pos = geo.GetGeopositionAsync().get().Coordinate().Point().Position();
        lat = pos.Latitude;
        lon = pos.Longitude;
        place = L"Current location";
    }

    WCHAR coords[96];
    swprintf_s(coords, L"latitude=%.4f&longitude=%.4f", lat, lon);
    std::wstring url =
        std::wstring(L"https://api.open-meteo.com/v1/forecast?") + coords +
        L"&current=temperature_2m,apparent_temperature,relative_humidity_2m,"
        L"weather_code,wind_speed_10m,is_day"
        L"&hourly=temperature_2m,weather_code,is_day"
        L"&daily=weather_code,temperature_2m_max,temperature_2m_min"
        L"&timezone=auto&forecast_days=6" +
        (celsius ? L"" : L"&temperature_unit=fahrenheit&wind_speed_unit=mph");
    std::string body = HttpGet(url);
    json::JsonObject root{nullptr};
    if (body.empty() || !json::JsonObject::TryParse(winrt::to_hstring(body), root)) {
        return;
    }

    Weather w;
    w.place = place;
    auto current = root.GetNamedObject(L"current");
    w.temp = (float)current.GetNamedNumber(L"temperature_2m");
    w.feels = (float)current.GetNamedNumber(L"apparent_temperature");
    w.humidity = (int)current.GetNamedNumber(L"relative_humidity_2m");
    w.code = (int)current.GetNamedNumber(L"weather_code");
    w.wind = (float)current.GetNamedNumber(L"wind_speed_10m");
    w.day = current.GetNamedNumber(L"is_day") != 0;
    std::wstring now = current.GetNamedString(L"time").c_str();

    auto hourly = root.GetNamedObject(L"hourly");
    auto hTime = hourly.GetNamedArray(L"time");
    auto hTemp = hourly.GetNamedArray(L"temperature_2m");
    auto hCode = hourly.GetNamedArray(L"weather_code");
    auto hDay = hourly.GetNamedArray(L"is_day");
    uint32_t first = 0;
    for (uint32_t i = 0; i < hTime.Size(); i++) {
        if (std::wstring(hTime.GetStringAt(i).c_str()).compare(0, 13, now, 0, 13) == 0) {
            first = i + 1;
            break;
        }
    }
    for (uint32_t i = first; i < hTime.Size() && w.hours.size() < 6; i++) {
        std::wstring t = hTime.GetStringAt(i).c_str();
        SYSTEMTIME st{};
        st.wHour = (WORD)_wtoi(t.substr(11, 2).c_str());
        WCHAR label[32];
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOMINUTESORSECONDS, &st,
                        nullptr, label, ARRAYSIZE(label));
        w.hours.push_back({label, (int)hCode.GetNumberAt(i),
                           hDay.GetNumberAt(i) != 0,
                           (float)hTemp.GetNumberAt(i)});
    }

    auto daily = root.GetNamedObject(L"daily");
    auto dTime = daily.GetNamedArray(L"time");
    auto dCode = daily.GetNamedArray(L"weather_code");
    auto dMax = daily.GetNamedArray(L"temperature_2m_max");
    auto dMin = daily.GetNamedArray(L"temperature_2m_min");
    for (uint32_t i = 0; i < dTime.Size(); i++) {
        std::wstring t = dTime.GetStringAt(i).c_str();
        SYSTEMTIME st{};
        st.wYear = (WORD)_wtoi(t.substr(0, 4).c_str());
        st.wMonth = (WORD)_wtoi(t.substr(5, 2).c_str());
        st.wDay = (WORD)_wtoi(t.substr(8, 2).c_str());
        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);
        FileTimeToSystemTime(&ft, &st);
        WCHAR label[32] = L"Today";
        if (i > 0) {
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, L"ddd", label,
                            ARRAYSIZE(label), nullptr);
        }
        w.days.push_back({label, (int)dCode.GetNumberAt(i),
                          (float)dMin.GetNumberAt(i), (float)dMax.GetNumberAt(i)});
    }
    if (!w.days.empty()) {
        w.lo = w.days[0].lo;
        w.hi = w.days[0].hi;
    }
    w.ok = true;
    AcquireSRWLockExclusive(&g_weatherLock);
    g_weather = std::move(w);
    ReleaseSRWLockExclusive(&g_weatherLock);
    PostToUi(WM_APP_WEATHER);
}

void RequestWeather() {
    if (g_weatherBusy.exchange(true)) return;
    RunAsync([city = g_s.city, celsius = g_s.celsius] {
        try {
            FetchWeather(city, celsius);
        } catch (...) {
        }
        g_weatherBusy = false;
    });
}

struct LyricLine {
    double time;
    std::wstring text;
};

SRWLOCK g_lyricsLock = SRWLOCK_INIT;
std::wstring g_lyricsKey;
std::vector<LyricLine> g_lyrics;

std::vector<LyricLine> ParseLrc(const std::wstring& lrc) {
    std::vector<LyricLine> lines;
    size_t start = 0;
    while (start < lrc.size()) {
        size_t end = lrc.find(L'\n', start);
        if (end == std::wstring::npos) end = lrc.size();
        std::wstring line = lrc.substr(start, end - start);
        start = end + 1;
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        std::vector<double> times;
        size_t p = 0;
        while (p < line.size() && line[p] == L'[') {
            size_t close = line.find(L']', p);
            if (close == std::wstring::npos) break;
            int mm = 0;
            double ss = 0;
            if (swscanf_s(line.c_str() + p + 1, L"%d:%lf", &mm, &ss) == 2) {
                times.push_back(mm * 60 + ss);
            }
            p = close + 1;
        }
        std::wstring text = line.substr(std::min(p, line.size()));
        while (!text.empty() && text[0] == L' ') text.erase(0, 1);
        for (double t : times) lines.push_back({t, text});
    }
    std::stable_sort(lines.begin(), lines.end(),
                     [](const LyricLine& a, const LyricLine& b) { return a.time < b.time; });
    return lines;
}

void RequestLyrics(const std::wstring& title, const std::wstring& artist,
                   const std::wstring& album, double duration) {
    std::wstring key = title + L"\n" + artist;
    AcquireSRWLockExclusive(&g_lyricsLock);
    bool same = key == g_lyricsKey;
    if (!same) {
        g_lyricsKey = key;
        g_lyrics.clear();
    }
    ReleaseSRWLockExclusive(&g_lyricsLock);
    if (same) return;
    PostToUi(WM_APP_LYRICS);
    if (title.empty()) return;
    RunAsync([key, title, artist, album, duration] {
        namespace json = winrt::Windows::Data::Json;
        auto synced = [](const json::JsonObject& o) -> std::wstring {
            auto v = o.GetNamedValue(L"syncedLyrics", nullptr);
            return v && v.ValueType() == json::JsonValueType::String ? v.GetString().c_str()
                                                                       : L"";
        };
        std::wstring query = L"track_name=" + UrlEncode(title) + L"&artist_name=" +
                             UrlEncode(artist);
        std::wstring lrc;
        bool reached = false;
        if (duration > 0 && !album.empty()) {
            std::string body = HttpGet(L"https://lrclib.net/api/get?" + query + L"&album_name=" +
                                       UrlEncode(album) + L"&duration=" +
                                       std::to_wstring((int)std::lround(duration)));
            reached |= !body.empty();
            json::JsonObject o{nullptr};
            if (!body.empty() && json::JsonObject::TryParse(winrt::to_hstring(body), o)) {
                lrc = synced(o);
            }
        }
        if (lrc.empty()) {
            std::string body = HttpGet(L"https://lrclib.net/api/search?" + query);
            reached |= !body.empty();
            json::JsonArray list{nullptr};
            if (!body.empty() && json::JsonArray::TryParse(winrt::to_hstring(body), list)) {
                for (uint32_t i = 0; i < list.Size() && lrc.empty(); i++) {
                    if (list.GetAt(i).ValueType() != json::JsonValueType::Object) continue;
                    auto o = list.GetObjectAt(i);
                    double d = o.GetNamedNumber(L"duration", 0);
                    if (duration > 0 && d > 0 && std::abs(d - duration) > 3) continue;
                    lrc = synced(o);
                }
            }
        }
        if (!reached) {
            // LRCLIB didn't answer: forget this song so it's asked again later.
            AcquireSRWLockExclusive(&g_lyricsLock);
            if (g_lyricsKey == key) g_lyricsKey.clear();
            ReleaseSRWLockExclusive(&g_lyricsLock);
            return;
        }
        auto lines = ParseLrc(lrc);
        AcquireSRWLockExclusive(&g_lyricsLock);
        if (g_lyricsKey == key) g_lyrics = std::move(lines);
        ReleaseSRWLockExclusive(&g_lyricsLock);
        PostToUi(WM_APP_LYRICS);
    });
}

// WMO weather code -> emoji and words.
std::wstring WeatherEmoji(int code, bool day) {
    if (code == 0) return day ? L"☀️" : L"\U0001F319";
    if (code == 1) return day ? L"\U0001F324️" : L"\U0001F319";
    if (code == 2) return day ? L"⛅" : L"☁️";
    if (code == 3) return L"☁️";
    if (code == 45 || code == 48) return L"\U0001F32B️";
    if (code >= 51 && code <= 57) return L"\U0001F326️";
    if ((code >= 61 && code <= 67) || (code >= 80 && code <= 82)) {
        return L"\U0001F327️";
    }
    if ((code >= 71 && code <= 77) || code == 85 || code == 86) {
        return L"\U0001F328️";
    }
    if (code >= 95) return L"⛈️";
    return L"\U0001F321️";
}

PCWSTR WeatherWords(int code) {
    if (code == 0) return L"Clear";
    if (code == 1) return L"Mostly clear";
    if (code == 2) return L"Partly cloudy";
    if (code == 3) return L"Cloudy";
    if (code == 45 || code == 48) return L"Fog";
    if (code >= 51 && code <= 57) return L"Drizzle";
    if (code >= 61 && code <= 67) return L"Rain";
    if (code >= 71 && code <= 77) return L"Snow";
    if (code >= 80 && code <= 82) return L"Showers";
    if (code == 85 || code == 86) return L"Snow showers";
    if (code >= 95) return L"Thunderstorm";
    return L"";
}

// Radios (Wi-Fi, Bluetooth): -1 missing, 0 off, 1 on.
std::atomic<int> g_wifiState{-1}, g_btState{-1};
SRWLOCK g_ssidLock = SRWLOCK_INIT;
std::wstring g_ssid;

std::wstring ReadSsid() {
    HANDLE client;
    DWORD version;
    std::wstring ssid;
    if (WlanOpenHandle(2, nullptr, &version, &client) != ERROR_SUCCESS) {
        return ssid;
    }
    PWLAN_INTERFACE_INFO_LIST list = nullptr;
    if (WlanEnumInterfaces(client, nullptr, &list) == ERROR_SUCCESS) {
        for (DWORD i = 0; i < list->dwNumberOfItems && ssid.empty(); i++) {
            auto& itf = list->InterfaceInfo[i];
            if (itf.isState != wlan_interface_state_connected) continue;
            PWLAN_CONNECTION_ATTRIBUTES attrs = nullptr;
            DWORD size = 0;
            if (WlanQueryInterface(client, &itf.InterfaceGuid,
                                   wlan_intf_opcode_current_connection, nullptr,
                                   &size, (void**)&attrs, nullptr) ==
                ERROR_SUCCESS) {
                auto& s = attrs->wlanAssociationAttributes.dot11Ssid;
                int n = MultiByteToWideChar(CP_UTF8, 0, (char*)s.ucSSID,
                                            s.uSSIDLength, nullptr, 0);
                ssid.resize(n);
                MultiByteToWideChar(CP_UTF8, 0, (char*)s.ucSSID, s.uSSIDLength,
                                    ssid.data(), n);
                WlanFreeMemory(attrs);
            }
        }
        WlanFreeMemory(list);
    }
    WlanCloseHandle(client, nullptr);
    return ssid;
}

void RefreshRadios(int setKind = 0, bool on = false) {
    RunAsync([setKind, on] {
        using namespace winrt::Windows::Devices::Radios;
        if (Radio::RequestAccessAsync().get() != RadioAccessStatus::Allowed) {
            g_wifiState = g_btState = -1;
        } else {
            int wifi = -1, bt = -1;
            for (auto const& radio : Radio::GetRadiosAsync().get()) {
                RadioKind kind = radio.Kind();
                int k = kind == RadioKind::WiFi        ? 1
                        : kind == RadioKind::Bluetooth ? 2
                                                       : 0;
                if (!k) continue;
                if (k == setKind) {
                    radio.SetStateAsync(on ? RadioState::On : RadioState::Off)
                        .get();
                }
                int state = radio.State() == RadioState::On ? 1 : 0;
                (k == 1 ? wifi : bt) = std::max(k == 1 ? wifi : bt, state);
            }
            g_wifiState = wifi;
            g_btState = bt;
        }
        if (setKind == 1 && on) Sleep(2500);   // give it time to connect
        std::wstring ssid = ReadSsid();
        AcquireSRWLockExclusive(&g_ssidLock);
        g_ssid = ssid;
        ReleaseSRWLockExclusive(&g_ssidLock);
        PostToUi(WM_APP_REFRESH);
    });
}

// Brightness: the laptop panel through WMI, or the main monitor through DDC/CI.
std::atomic<int> g_brightness{-1};
std::atomic<int> g_brightnessTarget{-1};
std::atomic<bool> g_brightnessBusy{false};

bool WmiBrightness(int* get, int set) {
    ComPtr<IWbemLocator> locator;
    ComPtr<IWbemServices> services;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&locator)))) {
        return false;
    }
    BSTR ns = SysAllocString(L"ROOT\\WMI");
    HRESULT hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0,
                                        nullptr, nullptr, &services);
    SysFreeString(ns);
    if (FAILED(hr)) return false;
    CoSetProxyBlanket(services.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                      nullptr, RPC_C_AUTHN_LEVEL_CALL,
                      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    auto query = [&](PCWSTR text) {
        ComPtr<IEnumWbemClassObject> items;
        ComPtr<IWbemClassObject> item;
        BSTR lang = SysAllocString(L"WQL");
        BSTR q = SysAllocString(text);
        services->ExecQuery(lang, q,
                            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                            nullptr, &items);
        SysFreeString(lang);
        SysFreeString(q);
        ULONG got = 0;
        if (items) items->Next(3000, 1, &item, &got);
        return item;
    };
    if (get) {
        auto item = query(L"SELECT CurrentBrightness FROM WmiMonitorBrightness "
                          L"WHERE Active=TRUE");
        VARIANT v;
        VariantInit(&v);
        if (!item || FAILED(item->Get(L"CurrentBrightness", 0, &v, nullptr, nullptr))) {
            return false;
        }
        *get = v.vt == VT_UI1 ? v.bVal : v.vt == VT_I4 ? v.lVal : -1;
        VariantClear(&v);
        return *get >= 0;
    }
    auto item = query(L"SELECT * FROM WmiMonitorBrightnessMethods WHERE Active=TRUE");
    if (!item) return false;
    VARIANT path;
    VariantInit(&path);
    ComPtr<IWbemClassObject> cls, inDef, in;
    BSTR clsName = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR method = SysAllocString(L"WmiSetBrightness");
    bool ok = SUCCEEDED(item->Get(L"__PATH", 0, &path, nullptr, nullptr)) &&
              SUCCEEDED(services->GetObject(clsName, 0, nullptr, &cls, nullptr)) &&
              SUCCEEDED(cls->GetMethod(method, 0, &inDef, nullptr)) &&
              SUCCEEDED(inDef->SpawnInstance(0, &in));
    if (ok) {
        VARIANT timeout, value;
        VariantInit(&timeout);
        timeout.vt = VT_I4;
        timeout.lVal = 0;
        VariantInit(&value);
        value.vt = VT_UI1;
        value.bVal = (BYTE)set;
        in->Put(L"Timeout", 0, &timeout, 0);
        in->Put(L"Brightness", 0, &value, 0);
        ok = SUCCEEDED(services->ExecMethod(path.bstrVal, method, 0, nullptr,
                                            in.Get(), nullptr, nullptr));
    }
    VariantClear(&path);
    SysFreeString(clsName);
    SysFreeString(method);
    return ok;
}

bool DdcBrightness(int* get, int set) {
    HMONITOR mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    DWORD count = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(mon, &count) || !count) {
        return false;
    }
    std::vector<PHYSICAL_MONITOR> monitors(count);
    if (!GetPhysicalMonitorsFromHMONITOR(mon, count, monitors.data())) {
        return false;
    }
    bool ok = false;
    for (auto& m : monitors) {
        DWORD lo, cur, hi;
        if (!GetMonitorBrightness(m.hPhysicalMonitor, &lo, &cur, &hi) || hi <= lo) {
            continue;
        }
        if (get) {
            *get = (int)((cur - lo) * 100 / (hi - lo));
        } else {
            SetMonitorBrightness(m.hPhysicalMonitor, lo + set * (hi - lo) / 100);
        }
        ok = true;
        break;
    }
    DestroyPhysicalMonitors(count, monitors.data());
    return ok;
}

std::atomic<int> g_brightnessKind{0};   // 0 unknown, 1 WMI, 2 DDC, -1 none

void ReadBrightness() {
    RunAsync([] {
        int value = -1;
        if (WmiBrightness(&value, 0)) {
            g_brightnessKind = 1;
        } else if (DdcBrightness(&value, 0)) {
            g_brightnessKind = 2;
        } else {
            g_brightnessKind = -1;
        }
        g_brightness = value;
        PostToUi(WM_APP_REFRESH);
    });
}

void SetBrightness(int value) {
    g_brightness = value;
    g_brightnessTarget = value;
    if (g_brightnessBusy.exchange(true)) return;
    RunAsync([] {
        int target;
        while ((target = g_brightnessTarget.exchange(-1)) >= 0) {
            if (g_brightnessKind == 1) {
                WmiBrightness(nullptr, target);
            } else if (g_brightnessKind == 2) {
                DdcBrightness(nullptr, target);
            }
        }
        g_brightnessBusy = false;
    });
}

bool IsDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValue(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

void SetDarkMode(bool dark) {
    DWORD value = dark ? 0 : 1;
    HKEY key;
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
        RegSetValueEx(key, L"AppsUseLightTheme", 0, REG_DWORD, (BYTE*)&value,
                      sizeof(value));
        RegSetValueEx(key, L"SystemUsesLightTheme", 0, REG_DWORD, (BYTE*)&value,
                      sizeof(value));
        RegCloseKey(key);
    }
    RunAsync([] {
        DWORD_PTR result;
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                           (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 1000,
                           &result);
    });
}

////////////////////////////////////////////////////////////////////////////////
// Audio: master volume, microphone, per-app sessions, level meter (UI thread)

class AudioEvents final : public IAudioEndpointVolumeCallback,
                          public IMMNotificationClient {
   public:
    STDMETHODIMP QueryInterface(REFIID riid, void** out) override {
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *out = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else if (riid == __uuidof(IMMNotificationClient)) {
            *out = static_cast<IMMNotificationClient*>(this);
        } else {
            *out = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++m_ref; }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG ref = --m_ref;
        if (!ref) delete this;
        return ref;
    }
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        PostToUi(WM_APP_VOLUME);
        return S_OK;
    }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow, ERole role, LPCWSTR) override {
        if (role == eConsole) PostToUi(WM_APP_AUDIODEV);
        return S_OK;
    }
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

   private:
    std::atomic<ULONG> m_ref{1};
};

struct MixSession {
    DWORD pid = 0;
    bool system = false;
    std::wstring name, exe, family;
    float volume = 1;
    bool muted = false;
    std::vector<ComPtr<ISimpleAudioVolume>> controls;
    ComPtr<IAudioMeterInformation> meter;
};

std::wstring ProcessPath(DWORD pid) {
    std::wstring result;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        WCHAR path[MAX_PATH];
        DWORD len = ARRAYSIZE(path);
        if (QueryFullProcessImageName(process, 0, path, &len)) result = path;
        CloseHandle(process);
    }
    return result;
}

std::wstring ProcessFamily(DWORD pid) {
    std::wstring result;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        WCHAR family[128];
        UINT32 len = ARRAYSIZE(family);
        if (GetPackageFamilyName(process, &len, family) == ERROR_SUCCESS) {
            result = family;
        }
        CloseHandle(process);
    }
    return result;
}

std::wstring FileDescription(const std::wstring& path) {
    static std::map<std::wstring, std::wstring> cache;
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;
    std::wstring result;
    DWORD handle;
    DWORD size = GetFileVersionInfoSize(path.c_str(), &handle);
    std::vector<BYTE> data(size);
    struct Lang {
        WORD lang, codepage;
    }* langs = nullptr;
    UINT len = 0;
    if (size && GetFileVersionInfo(path.c_str(), 0, size, data.data()) &&
        VerQueryValue(data.data(), L"\\VarFileInfo\\Translation", (void**)&langs,
                      &len) &&
        len >= sizeof(Lang)) {
        WCHAR key[64];
        swprintf_s(key, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                   langs[0].lang, langs[0].codepage);
        PCWSTR value = nullptr;
        if (VerQueryValue(data.data(), key, (void**)&value, &len) && value) {
            result = value;
        }
    }
    if (result.empty()) {
        PCWSTR name = PathFindFileName(path.c_str());
        result = name;
        PathRemoveExtension(result.data());
        result.resize(wcslen(result.c_str()));
    }
    cache[path] = result;
    return result;
}

struct Audio {
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IAudioEndpointVolume> endpoint, mic;
    ComPtr<IAudioMeterInformation> endpointMeter;
    ComPtr<IAudioSessionManager2> sessions;
    AudioEvents* events = nullptr;
    float level = 0;
    bool muted = false;
    bool micMuted = false;
    bool hasMic = false;
    std::vector<MixSession> mix;

    void Init() {
        events = new AudioEvents();
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&enumerator)))) {
            enumerator->RegisterEndpointNotificationCallback(events);
        }
        OpenDefault();
    }

    void OpenDefault() {
        if (endpoint) endpoint->UnregisterControlChangeNotify(events);
        if (mic) mic->UnregisterControlChangeNotify(events);
        endpoint.Reset();
        mic.Reset();
        endpointMeter.Reset();
        sessions.Reset();
        mix.clear();
        ComPtr<IMMDevice> device, capture;
        if (enumerator &&
            SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
            if (SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume),
                                           CLSCTX_INPROC_SERVER, nullptr,
                                           (void**)endpoint.GetAddressOf()))) {
                endpoint->RegisterControlChangeNotify(events);
            }
            device->Activate(__uuidof(IAudioMeterInformation), CLSCTX_INPROC_SERVER,
                             nullptr, (void**)endpointMeter.GetAddressOf());
            device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER,
                             nullptr, (void**)sessions.GetAddressOf());
        }
        if (enumerator &&
            SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &capture)) &&
            SUCCEEDED(capture->Activate(__uuidof(IAudioEndpointVolume),
                                        CLSCTX_INPROC_SERVER, nullptr,
                                        (void**)mic.GetAddressOf()))) {
            mic->RegisterControlChangeNotify(events);
        }
        Read();
    }

    void Read() {
        BOOL m = FALSE;
        if (endpoint && SUCCEEDED(endpoint->GetMasterVolumeLevelScalar(&level)) &&
            SUCCEEDED(endpoint->GetMute(&m))) {
            muted = m;
        } else {
            level = 0;
            muted = true;
        }
        hasMic = mic && SUCCEEDED(mic->GetMute(&m));
        micMuted = hasMic && m;
    }

    void SetLevel(float v) {
        if (!endpoint) return;
        v = std::clamp(std::round(v * 100) / 100, 0.f, 1.f);
        endpoint->SetMasterVolumeLevelScalar(v, nullptr);
        if (v > 0 && muted) endpoint->SetMute(FALSE, nullptr);
        Read();
    }

    void Step(int percent) { SetLevel(level + percent / 100.f); }

    void ToggleMute() {
        if (endpoint) endpoint->SetMute(!muted, nullptr);
        Read();
    }

    void ToggleMic() {
        if (mic) mic->SetMute(!micMuted, nullptr);
        Read();
    }

    // Groups the device's sessions by process.
    void RefreshMix() {
        mix.clear();
        ComPtr<IAudioSessionEnumerator> list;
        int count = 0;
        if (!sessions || FAILED(sessions->GetSessionEnumerator(&list)) ||
            FAILED(list->GetCount(&count))) {
            return;
        }
        for (int i = 0; i < count; i++) {
            ComPtr<IAudioSessionControl> control;
            ComPtr<IAudioSessionControl2> control2;
            ComPtr<ISimpleAudioVolume> volume;
            AudioSessionState state;
            if (FAILED(list->GetSession(i, &control)) ||
                FAILED(control.As(&control2)) || FAILED(control.As(&volume)) ||
                FAILED(control->GetState(&state)) ||
                state == AudioSessionStateExpired) {
                continue;
            }
            DWORD pid = 0;
            control2->GetProcessId(&pid);
            bool system = control2->IsSystemSoundsSession() == S_OK;
            auto it = std::find_if(mix.begin(), mix.end(), [&](const MixSession& m) {
                return system ? m.system : (!m.system && m.pid == pid);
            });
            if (it == mix.end()) {
                MixSession m;
                m.pid = pid;
                m.system = system;
                if (system) {
                    m.name = L"System sounds";
                } else {
                    m.exe = ProcessPath(pid);
                    m.family = ProcessFamily(pid);
                    LPWSTR display = nullptr;
                    if (SUCCEEDED(control->GetDisplayName(&display)) && display &&
                        *display && display[0] != L'@') {
                        m.name = display;
                    }
                    CoTaskMemFree(display);
                    if (m.name.empty() && !m.exe.empty()) m.name = FileDescription(m.exe);
                    if (m.name.empty()) continue;
                }
                volume->GetMasterVolume(&m.volume);
                BOOL mute = FALSE;
                volume->GetMute(&mute);
                m.muted = mute;
                control.As(&m.meter);
                mix.push_back(std::move(m));
                it = mix.end() - 1;
            }
            it->controls.push_back(volume);
        }
        std::stable_sort(mix.begin(), mix.end(), [](const MixSession& a, const MixSession& b) {
            return a.system > b.system;
        });
    }

    void SetSessionVolume(MixSession& m, float v) {
        m.volume = std::clamp(v, 0.f, 1.f);
        for (auto& c : m.controls) {
            c->SetMasterVolume(m.volume, nullptr);
            if (m.muted && v > 0) c->SetMute(FALSE, nullptr);
        }
        if (v > 0) m.muted = false;
    }

    void ToggleSessionMute(MixSession& m) {
        m.muted = !m.muted;
        for (auto& c : m.controls) c->SetMute(m.muted, nullptr);
    }

    // The session playing the given media app (matched by package or exe name).
    MixSession* FindMediaSession(const std::wstring& aumid) {
        if (aumid.empty()) return nullptr;
        std::wstring id = aumid;
        CharLowerBuff(id.data(), (DWORD)id.size());
        size_t bang = id.find(L'!');
        std::wstring family = bang != std::wstring::npos ? id.substr(0, bang) : L"";
        for (auto& m : mix) {
            if (m.system) continue;
            std::wstring fam = m.family;
            CharLowerBuff(fam.data(), (DWORD)fam.size());
            if (!family.empty() && fam == family) return &m;
            std::wstring exe = PathFindFileName(m.exe.c_str());
            CharLowerBuff(exe.data(), (DWORD)exe.size());
            std::wstring stem = exe.substr(0, exe.rfind(L'.'));
            if (!stem.empty() && (id == exe || id == stem ||
                                  id.find(stem) != std::wstring::npos)) {
                return &m;
            }
        }
        return nullptr;
    }

    float Peak(const std::wstring& aumid) {
        float peak = 0;
        if (MixSession* m = FindMediaSession(aumid); m && m->meter) {
            m->meter->GetPeakValue(&peak);
        } else if (endpointMeter) {
            endpointMeter->GetPeakValue(&peak);
        }
        return peak;
    }

    void Uninit() {
        mix.clear();
        if (endpoint) endpoint->UnregisterControlChangeNotify(events);
        if (mic) mic->UnregisterControlChangeNotify(events);
        endpoint.Reset();
        mic.Reset();
        endpointMeter.Reset();
        sessions.Reset();
        if (enumerator) {
            enumerator->UnregisterEndpointNotificationCallback(events);
            enumerator.Reset();
        }
        if (events) {
            events->Release();
            events = nullptr;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Battery and network (polled every 30 s)

struct Status {
    bool hasBattery = false, charging = false;
    int batteryPercent = 0;
    enum Net { Offline, Wifi, Ethernet } net = Offline;

    bool Read() {
        Status old = *this;
        SYSTEM_POWER_STATUS power;
        if (GetSystemPowerStatus(&power)) {
            hasBattery = !(power.BatteryFlag & 128) && power.BatteryFlag != 255;
            charging = power.ACLineStatus == 1;
            batteryPercent =
                power.BatteryLifePercent <= 100 ? power.BatteryLifePercent : 0;
        }
        net = ReadNetwork();
        return old.hasBattery != hasBattery || old.charging != charging ||
               old.batteryPercent != batteryPercent || old.net != net;
    }

    static Net ReadNetwork() {
        ULONG size = 16 * 1024;
        std::vector<BYTE> buffer(size);
        ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST |
                      GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
        auto list = (IP_ADAPTER_ADDRESSES*)buffer.data();
        ULONG r = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, list, &size);
        if (r == ERROR_BUFFER_OVERFLOW) {
            buffer.resize(size);
            list = (IP_ADAPTER_ADDRESSES*)buffer.data();
            r = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, list, &size);
        }
        if (r != NO_ERROR) return Offline;
        Net result = Offline;
        for (auto a = list; a; a = a->Next) {
            if (a->OperStatus != IfOperStatusUp || !a->FirstGatewayAddress ||
                a->IfType == IF_TYPE_SOFTWARE_LOOPBACK || a->IfType == IF_TYPE_TUNNEL) {
                continue;
            }
            if (a->IfType == IF_TYPE_IEEE80211) return Wifi;
            result = Ethernet;
        }
        return result;
    }
};

////////////////////////////////////////////////////////////////////////////////
// System actions

D2D1_COLOR_F SystemAccent() {
    try {
        // Windows itself uses the light variant on dark surfaces.
        using winrt::Windows::UI::ViewManagement::UIColorType;
        winrt::Windows::UI::ViewManagement::UISettings ui;
        bool dark = 0.2126f * g_s.bg.r + 0.7152f * g_s.bg.g + 0.0722f * g_s.bg.b < 0.5f;
        auto c = ui.GetColorValue(dark ? UIColorType::AccentLight2 : UIColorType::AccentDark1);
        return D2D1::ColorF(c.R / 255.f, c.G / 255.f, c.B / 255.f);
    } catch (...) {
    }
    DWORD color;
    BOOL opaque;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return D2D1::ColorF(((color >> 16) & 0xFF) / 255.f,
                            ((color >> 8) & 0xFF) / 255.f, (color & 0xFF) / 255.f);
    }
    return D2D1::ColorF(0x0078D4);
}

void SendWinHotkey(WORD vk) {
    INPUT in[4] = {};
    for (auto& i : in) i.type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_LWIN;
    in[1].ki.wVk = vk;
    in[2].ki.wVk = vk;
    in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].ki.wVk = VK_LWIN;
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(in), in, sizeof(INPUT));
}

bool IsProcessElevated() {
    HANDLE token;
    TOKEN_ELEVATION elevation{};
    DWORD size;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return false;
    bool elevated = GetTokenInformation(token, TokenElevation, &elevation,
                                        sizeof(elevation), &size) &&
                    elevation.TokenIsElevated;
    CloseHandle(token);
    return elevated;
}

// Launches through explorer's desktop so it doesn't inherit Windhawk's rights.
bool ShellExecuteAsUser(const std::wstring& file, const std::wstring& args) {
    ComPtr<IShellWindows> windows;
    ComPtr<IDispatch> dispatch, viewDispatch, appDispatch;
    ComPtr<IServiceProvider> provider;
    ComPtr<IShellBrowser> browser;
    ComPtr<IShellView> view;
    ComPtr<IShellFolderViewDual> folderView;
    ComPtr<IShellDispatch2> shell;
    VARIANT empty{};
    long hwnd;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_PPV_ARGS(&windows))) ||
        windows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &hwnd,
                              SWFO_NEEDDISPATCH, &dispatch) != S_OK ||
        FAILED(dispatch.As(&provider)) ||
        FAILED(provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser))) ||
        FAILED(browser->QueryActiveShellView(&view)) ||
        FAILED(view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&viewDispatch))) ||
        FAILED(viewDispatch.As(&folderView)) ||
        FAILED(folderView->get_Application(&appDispatch)) ||
        FAILED(appDispatch.As(&shell))) {
        return false;
    }
    BSTR bstrFile = SysAllocString(file.c_str());
    VARIANT vArgs{};
    vArgs.vt = VT_BSTR;
    vArgs.bstrVal = SysAllocString(args.c_str());
    HRESULT hr = shell->ShellExecute(bstrFile, vArgs, empty, empty, empty);
    SysFreeString(bstrFile);
    VariantClear(&vArgs);
    return SUCCEEDED(hr);
}

void Launch(const std::wstring& target, const std::wstring& args = L"") {
    if (IsProcessElevated() && ShellExecuteAsUser(target, args)) return;
    ShellExecute(nullptr, nullptr, target.c_str(),
                 args.empty() ? nullptr : args.c_str(), nullptr, SW_SHOWNORMAL);
}

bool EnableShutdownPrivilege() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return false;
    }
    TOKEN_PRIVILEGES tp{1};
    LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(token, FALSE, &tp, 0, nullptr, nullptr);
    bool ok = GetLastError() == ERROR_SUCCESS;
    CloseHandle(token);
    return ok;
}

enum class PowerAction { Lock, Sleep, SignOut, Restart, ShutDown };

void DoPower(PowerAction action) {
    switch (action) {
        case PowerAction::Lock:
            LockWorkStation();
            break;
        case PowerAction::Sleep:
            RunAsync([] { SetSuspendState(FALSE, FALSE, FALSE); });
            break;
        case PowerAction::SignOut:
            ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_FLAG_PLANNED);
            break;
        case PowerAction::Restart:
            EnableShutdownPrivilege();
            ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
            break;
        case PowerAction::ShutDown:
            EnableShutdownPrivilege();
            ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF,
                          SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
            break;
    }
}

void SetAccentBlur(HWND hwnd, int state) {
    struct AccentPolicy {
        int state, flags;
        DWORD color;
        int animation;
    };
    struct CompositionData {
        int attribute;
        void* data;
        SIZE_T size;
    };
    using Fn = BOOL(WINAPI*)(HWND, CompositionData*);
    static auto fn = (Fn)GetProcAddress(GetModuleHandle(L"user32.dll"),
                                        "SetWindowCompositionAttribute");
    if (!fn) return;
    AccentPolicy policy{state, state == 4 ? 2 : 0, 0x01000000, 0};
    CompositionData data{19, &policy, sizeof(policy)};
    fn(hwnd, &data);
}

////////////////////////////////////////////////////////////////////////////////
// Drawing (UI thread). One software Direct2D target draws every window.

namespace Glyph {
constexpr WCHAR Previous = 0xE892, Next = 0xE893, Play = 0xE768, Pause = 0xE769,
                Music = 0xEC4F, Mute = 0xE74F, Volume0 = 0xE992, Bell = 0xEA8F,
                Wifi = 0xE701, Ethernet = 0xE839, Globe = 0xE774,
                Battery0 = 0xEBA0, BatteryCharging0 = 0xEBAB, Power = 0xE7E8,
                Lock = 0xE72E, Sleep = 0xE708, SignOut = 0xF3B1, Restart = 0xE777,
                Controls = 0xE9E9, ChevronLeft = 0xE96F, ChevronRight = 0xE970,
                ChevronUp = 0xE96D, ChevronDown = 0xE96E, Bluetooth = 0xE702,
                Mic = 0xE720, MicOff = 0xEC54, Moon = 0xE708, Sun = 0xE706,
                Settings = 0xE713, Shuffle = 0xE8B1, Repeat = 0xE8EE,
                RepeatOne = 0xE8ED, Pin = 0xE718, Pinned = 0xE840,
                ChevronL = 0xE76B, ChevronR = 0xE76C, QuickSettings = 0xE9E9;
}

enum Align { kLeft, kCenter, kRight };

struct Gfx {
    ComPtr<ID2D1Factory1> d2d;
    ComPtr<IDWriteFactory> dw;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1SolidColorBrush> brush;
    ComPtr<ID2D1Layer> layer;
    std::wstring iconFont = L"Segoe MDL2 Assets";
    std::map<std::wstring, ComPtr<IDWriteTextFormat>> formats;
    std::map<std::wstring, ComPtr<ID2D1Bitmap>> icons;
    float scale = 1;
};
Gfx g_gfx;

inline float S(float v) { return v * g_gfx.scale; }

D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float a) {
    c.a *= a;
    return c;
}

D2D1_COLOR_F Mix(D2D1_COLOR_F a, D2D1_COLOR_F b, float t) {
    return D2D1::ColorF(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
                        a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
}

float Luminance(D2D1_COLOR_F c) {
    return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
}

float Lerp(float a, float b, float t) { return a + (b - a) * t; }

IDWriteTextFormat* Font(float px, int weight = 400, Align align = kLeft,
                        PCWSTR family = nullptr) {
    if (!family) family = g_s.fontFamily.c_str();
    WCHAR key[160];
    swprintf_s(key, L"%s|%d|%d|%d", family, (int)(px * 100), weight, align);
    auto it = g_gfx.formats.find(key);
    if (it != g_gfx.formats.end()) return it->second.Get();
    ComPtr<IDWriteTextFormat> fmt;
    if (FAILED(g_gfx.dw->CreateTextFormat(
            family, nullptr, (DWRITE_FONT_WEIGHT)weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, px, L"", &fmt))) {
        g_gfx.dw->CreateTextFormat(L"Segoe UI", nullptr, (DWRITE_FONT_WEIGHT)weight,
                                   DWRITE_FONT_STYLE_NORMAL,
                                   DWRITE_FONT_STRETCH_NORMAL, px, L"", &fmt);
    }
    if (!fmt) return nullptr;
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    fmt->SetTextAlignment(align == kCenter  ? DWRITE_TEXT_ALIGNMENT_CENTER
                          : align == kRight ? DWRITE_TEXT_ALIGNMENT_TRAILING
                                            : DWRITE_TEXT_ALIGNMENT_LEADING);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ComPtr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(g_gfx.dw->CreateEllipsisTrimmingSign(fmt.Get(), &ellipsis))) {
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        fmt->SetTrimming(&trimming, ellipsis.Get());
    }
    g_gfx.formats[key] = fmt;
    return fmt.Get();
}

float TextWidth(const std::wstring& text, float px, int weight = 400,
                PCWSTR family = nullptr) {
    IDWriteTextFormat* fmt = Font(px, weight, kLeft, family);
    if (!fmt || text.empty()) return 0;
    ComPtr<IDWriteTextLayout> layout;
    DWRITE_TEXT_METRICS m{};
    if (FAILED(g_gfx.dw->CreateTextLayout(text.c_str(), (UINT32)text.size(), fmt,
                                          10000, 1000, &layout)) ||
        FAILED(layout->GetMetrics(&m))) {
        return 0;
    }
    return std::ceil(m.widthIncludingTrailingWhitespace);
}

void DrawStr(const std::wstring& text, D2D1_RECT_F rc, D2D1_COLOR_F color,
             float px, int weight = 400, Align align = kLeft,
             PCWSTR family = nullptr, bool colorFont = false) {
    IDWriteTextFormat* fmt = Font(px, weight, align, family);
    if (text.empty() || !fmt || color.a <= 0) return;
    g_gfx.brush->SetColor(color);
    g_gfx.rt->DrawText(text.c_str(), (UINT32)text.size(), fmt, rc,
                       g_gfx.brush.Get(),
                       colorFont ? (D2D1_DRAW_TEXT_OPTIONS)4 /* color font */
                                 : D2D1_DRAW_TEXT_OPTIONS_NONE);
}

void DrawGlyph(WCHAR glyph, D2D1_RECT_F rc, D2D1_COLOR_F color, float px) {
    DrawStr(std::wstring(1, glyph), rc, color, px, 400, kCenter,
            g_gfx.iconFont.c_str());
}

void DrawEmoji(const std::wstring& emoji, D2D1_RECT_F rc, float px,
               Align align = kCenter, float alpha = 1) {
    DrawStr(emoji, rc, D2D1::ColorF(1, 1, 1, alpha), px, 400, align,
            L"Segoe UI Emoji", true);
}

void Fill(D2D1_RECT_F rc, float radius, D2D1_COLOR_F color) {
    if (color.a <= 0) return;
    g_gfx.brush->SetColor(color);
    if (radius > 0) {
        g_gfx.rt->FillRoundedRectangle(D2D1::RoundedRect(rc, radius, radius),
                                       g_gfx.brush.Get());
    } else {
        g_gfx.rt->FillRectangle(rc, g_gfx.brush.Get());
    }
}

void Stroke(D2D1_RECT_F rc, float radius, D2D1_COLOR_F color, float width) {
    if (color.a <= 0 || width <= 0) return;
    g_gfx.brush->SetColor(color);
    g_gfx.rt->DrawRoundedRectangle(D2D1::RoundedRect(rc, radius, radius),
                                   g_gfx.brush.Get(), width);
}

void FillWithBitmap(ID2D1Bitmap* bmp, D2D1_ROUNDED_RECT rr, bool stretch,
                    float opacity) {
    if (!bmp) return;
    ComPtr<ID2D1BitmapBrush> bb;
    if (FAILED(g_gfx.rt->CreateBitmapBrush(
            bmp,
            D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR),
            &bb))) {
        return;
    }
    D2D1_SIZE_F size = bmp->GetSize();
    float w = rr.rect.right - rr.rect.left, h = rr.rect.bottom - rr.rect.top;
    float sx = w / size.width, sy = h / size.height;
    if (!stretch) sx = sy = std::max(sx, sy);
    float tx = rr.rect.left + (w - size.width * sx) / 2;
    float ty = rr.rect.top + (h - size.height * sy) / 2;
    bb->SetTransform(D2D1::Matrix3x2F::Scale(sx, sy) *
                     D2D1::Matrix3x2F::Translation(tx, ty));
    bb->SetOpacity(opacity);
    g_gfx.rt->FillRoundedRectangle(rr, bb.Get());
}

void DrawBitmap(ID2D1Bitmap* bmp, D2D1_RECT_F rc, float opacity = 1) {
    if (bmp) {
        g_gfx.rt->DrawBitmap(bmp, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
}

D2D1_RECT_F Centered(D2D1_RECT_F rc, float w, float h) {
    float cx = (rc.left + rc.right) / 2, cy = (rc.top + rc.bottom) / 2;
    return D2D1::RectF(std::round(cx - w / 2), std::round(cy - h / 2),
                       std::round(cx - w / 2) + w, std::round(cy - h / 2) + h);
}

D2D1_RECT_F Offset(D2D1_RECT_F rc, float dx, float dy) {
    return D2D1::RectF(rc.left + dx, rc.top + dy, rc.right + dx, rc.bottom + dy);
}

bool Contains(D2D1_RECT_F rc, float x, float y) {
    return x >= rc.left && x < rc.right && y >= rc.top && y < rc.bottom;
}

ComPtr<ID2D1Bitmap> MakeBitmap(const std::vector<uint32_t>& px, int w, int h) {
    ComPtr<ID2D1Bitmap> bmp;
    auto props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    g_gfx.rt->CreateBitmap(D2D1::SizeU(w, h), px.data(), w * 4, props, &bmp);
    return bmp;
}

ComPtr<ID2D1Bitmap> BitmapFromIcon(HICON icon) {
    ComPtr<ID2D1Bitmap> result;
    ComPtr<IWICBitmap> wicBitmap;
    ComPtr<IWICFormatConverter> converter;
    if (icon && g_gfx.wic &&
        SUCCEEDED(g_gfx.wic->CreateBitmapFromHICON(icon, &wicBitmap)) &&
        SUCCEEDED(g_gfx.wic->CreateFormatConverter(&converter)) &&
        SUCCEEDED(converter->Initialize(wicBitmap.Get(), GUID_WICPixelFormat32bppPBGRA,
                                        WICBitmapDitherTypeNone, nullptr, 0,
                                        WICBitmapPaletteTypeCustom))) {
        g_gfx.rt->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &result);
    }
    return result;
}

// An icon for a path, shell parsing name or "shell:AppsFolder\<AUMID>".
ID2D1Bitmap* ShellIcon(const std::wstring& target, std::wstring* name = nullptr) {
    auto it = g_gfx.icons.find(target);
    if (it != g_gfx.icons.end() && !name) return it->second.Get();
    SHFILEINFO info{};
    PIDLIST_ABSOLUTE pidl = nullptr;
    UINT flags = SHGFI_ICON | SHGFI_LARGEICON | (name ? SHGFI_DISPLAYNAME : 0);
    if (SUCCEEDED(SHParseDisplayName(target.c_str(), nullptr, &pidl, 0, nullptr))) {
        SHGetFileInfo((PCWSTR)pidl, 0, &info, sizeof(info), SHGFI_PIDL | flags);
        CoTaskMemFree(pidl);
    } else {
        SHGetFileInfo(target.c_str(), 0, &info, sizeof(info), flags);
    }
    if (name) *name = info.szDisplayName;
    ComPtr<ID2D1Bitmap> bmp = BitmapFromIcon(info.hIcon);
    if (info.hIcon) DestroyIcon(info.hIcon);
    g_gfx.icons[target] = bmp;
    return bmp.Get();
}

////////////////////////////////////////////////////////////////////////////////
// Layered windows

struct LWin {
    HWND hwnd = nullptr;
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    int w = 0, h = 0;
    bool glass = false, visible = false;
    POINT pos{};

    bool Ensure(int width, int height) {
        width = std::max(width, 1);
        height = std::max(height, 1);
        if (bmp && width == w && height == h) return true;
        if (!dc) dc = CreateCompatibleDC(nullptr);
        if (bmp) {
            SelectObject(dc, old);
            DeleteObject(bmp);
            bmp = nullptr;
        }
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits;
        bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp) return false;
        old = SelectObject(dc, bmp);
        w = width;
        h = height;
        return true;
    }

    void Free() {
        if (dc) {
            if (bmp) {
                SelectObject(dc, old);
                DeleteObject(bmp);
            }
            DeleteDC(dc);
        }
        dc = nullptr;
        bmp = nullptr;
        w = h = 0;
    }

    void Begin(const D2D1_RECT_F* clip = nullptr) {
        RECT rc{0, 0, w, h};
        g_gfx.rt->BindDC(dc, &rc);
        g_gfx.rt->BeginDraw();
        if (clip) g_gfx.rt->PushAxisAlignedClip(*clip, D2D1_ANTIALIAS_MODE_ALIASED);
        g_gfx.rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    }

    bool End(const D2D1_RECT_F* clip = nullptr) {
        if (clip) g_gfx.rt->PopAxisAlignedClip();
        return g_gfx.rt->EndDraw() != D2DERR_RECREATE_TARGET;
    }

    void Present(int x, int y, BYTE alpha = 255, const RECT* dirty = nullptr) {
        pos = {x, y};
        if (glass) {
            SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
            InvalidateRect(hwnd, nullptr, FALSE);
            return;
        }
        POINT src{0, 0};
        SIZE size{w, h};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UPDATELAYEREDWINDOWINFO info{sizeof(info)};
        info.pptDst = &pos;
        info.psize = &size;
        info.hdcSrc = dc;
        info.pptSrc = &src;
        info.pblend = &blend;
        info.dwFlags = ULW_ALPHA;
        info.prcDirty = dirty;
        UpdateLayeredWindowIndirect(hwnd, &info);
    }

    void SetGlass(bool on, float radius) {
        if (on == glass) return;
        glass = on;
        LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (on) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
            SetAccentBlur(hwnd, g_s.glassAcrylic ? 4 : 3);
            int corner = radius < 1 ? 1 : radius < 6 ? 3 : 2;
            DwmSetWindowAttribute(hwnd, 33 /* corner preference */, &corner,
                                  sizeof(corner));
            D2D1_COLOR_F b = g_s.border;
            COLORREF border = g_s.borderWidth > 0 && b.a > 0
                                  ? RGB(b.r * 255, b.g * 255, b.b * 255)
                                  : 0xFFFFFFFE;
            DwmSetWindowAttribute(hwnd, 34 /* border color */, &border, sizeof(border));
        } else {
            SetAccentBlur(hwnd, 0);
            int corner = 1;
            DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        }
    }

    void Show(bool on) {
        if (on == visible) return;
        visible = on;
        ShowWindow(hwnd, on ? SW_SHOWNOACTIVATE : SW_HIDE);
    }

    void Paint() {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (dc) BitBlt(hdc, 0, 0, w, h, dc, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Animations

void RequestFrame();

struct Anim {
    float value = 0, from = 0, to = 0;
    ULONGLONG start = 0;
    DWORD duration = 0;

    void Snap(float v) {
        value = from = to = v;
        duration = 0;
    }
    void Go(float target, DWORD ms = 220) {
        if (target == to && (duration || value == target)) return;
        if (!g_s.animations || !ms) {
            Snap(target);
            return;
        }
        from = value;
        to = target;
        start = GetTickCount64();
        duration = ms;
        RequestFrame();
    }
    bool Active() const { return duration != 0; }
    void Tick(ULONGLONG now) {
        if (!duration) return;
        float t = std::clamp((float)(now - start) / duration, 0.f, 1.f);
        float e = 1 - (1 - t) * (1 - t) * (1 - t);
        value = from + (to - from) * e;
        if (t >= 1) {
            value = to;
            duration = 0;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Album art

void BlurLine(const uint32_t* src, uint32_t* dst, int n, int stride, int r) {
    int d = 2 * r + 1;
    for (int i = 0; i < n; i++) {
        uint32_t sum[4] = {};
        for (int k = -r; k <= r; k++) {
            uint32_t p = src[std::clamp(i + k, 0, n - 1) * stride];
            sum[0] += p & 0xFF;
            sum[1] += (p >> 8) & 0xFF;
            sum[2] += (p >> 16) & 0xFF;
            sum[3] += p >> 24;
        }
        dst[i * stride] =
            (sum[0] / d) | (sum[1] / d) << 8 | (sum[2] / d) << 16 | (sum[3] / d) << 24;
    }
}

void Blur(std::vector<uint32_t>& px, int w, int h, int r) {
    if (r <= 0) return;
    std::vector<uint32_t> tmp(px.size());
    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < h; y++) BlurLine(&px[y * w], &tmp[y * w], w, 1, r);
        for (int x = 0; x < w; x++) BlurLine(&tmp[x], &px[x], h, w, r);
    }
}

void Saturate(std::vector<uint32_t>& px, float s) {
    for (auto& p : px) {
        float b = p & 0xFF, g = (p >> 8) & 0xFF, r = (p >> 16) & 0xFF;
        float a = p >> 24;
        float gray = 0.299f * r + 0.587f * g + 0.114f * b;
        auto f = [&](float c) { return (uint32_t)std::clamp(gray + (c - gray) * s, 0.f, a); };
        p = f(b) | f(g) << 8 | f(r) << 16 | (uint32_t)a << 24;
    }
}

float AverageLuminance(const std::vector<uint32_t>& px) {
    double sum = 0;
    for (auto p : px) {
        sum += 0.0722 * (p & 0xFF) + 0.7152 * ((p >> 8) & 0xFF) +
               0.2126 * ((p >> 16) & 0xFF);
    }
    return px.empty() ? 0 : (float)(sum / px.size() / 255);
}

constexpr int kArtBgSize = kArtSize / 2;

// Half size (box filter), saturated and blurred.
std::vector<uint32_t> MakeArtBackground(const std::vector<uint32_t>& raw) {
    constexpr int n = kArtBgSize, f = 2;
    std::vector<uint32_t> small((size_t)n * n);
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            uint32_t sum[4] = {};
            for (int dy = 0; dy < f; dy++) {
                for (int dx = 0; dx < f; dx++) {
                    uint32_t p = raw[(y * f + dy) * kArtSize + x * f + dx];
                    for (int c = 0; c < 4; c++) sum[c] += (p >> (c * 8)) & 0xFF;
                }
            }
            int k = f * f;
            small[y * n + x] =
                (sum[0] / k) | (sum[1] / k) << 8 | (sum[2] / k) << 16 | (sum[3] / k) << 24;
        }
    }
    Saturate(small, g_s.artSaturation);
    Blur(small, n, n, (int)std::lround(g_s.artBlur * 0.17f));
    return small;
}

////////////////////////////////////////////////////////////////////////////////
// The island

constexpr UINT_PTR kTimerClock = 1, kTimerStatus = 2, kTimerOsd = 3,
                   kTimerFrame = 4, kTimerTip = 5, kTimerPeek = 6,
                   kTimerWeather = 7, kTimerPanel = 8, kTimerArm = 9,
                   kTimerActivity = 10, kTimerLyrics = 11, kTimerAlarm = 12;
constexpr WCHAR kGlyphTimer = 0xE916, kGlyphStopwatch = 0xE823, kGlyphCharging = 0xEA93;
constexpr DWORD kOsdMs = 1500;
#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif

enum class Act {
    None, Power, Notif, TrayToggle, Tray, Shortcut, Control, Volume, Network,
    Battery, Weather, Clock, Media, Prev, Play, Next, Fold, Timer, Stopwatch
};
enum class Kind { Glyph, Clock, Media, Bitmap, Emoji, Viz, VolumeBar, Text, Chip };

struct Item {
    Kind kind = Kind::Glyph;
    Act act = Act::None;
    D2D1_RECT_F rc{};
    int index = 0;
    std::wstring text, text2, badge, tip;
    WCHAR glyph = 0;
    ID2D1Bitmap* bmp = nullptr;
    float alpha = 1;
    bool trayGroup = false;
};

struct TrayIcon {
    HWND owner = nullptr;
    UINT id = 0;
    GUID guid{};
    bool hasGuid = false;
    UINT callback = 0, version = 0;
    DWORD pid = 0;
    bool hidden = false;
    std::wstring tip, key;
    ComPtr<ID2D1Bitmap> bmp;
};

std::vector<TrayIcon> g_tray;
std::vector<std::wstring> g_pinnedTray;

struct Island {
    LWin win;
    RECT monitor{}, work{};
    MediaSnapshot media;
    unsigned artVersion = ~0u;
    std::vector<uint32_t> artRaw;
    ComPtr<ID2D1Bitmap> artThumb, artBg;
    float artLuma = 0;
    D2D1_COLOR_F accent{};
    int notifCount = 0;
    std::wstring notifApp;
    Audio audio;
    Status status;
    Weather weather;
    bool fullscreen = false, appbar = false, appbarReserved = false;

    std::vector<Item> items;
    float targetW = 0, H = 0;
    Anim width, fold, hlX, hlW, hlA, trayFade;
    bool folded = false, peeking = false, trayOpen = false;
    int hover = -1, pressed = -1;
    bool tracking = false;
    ULONGLONG osdUntil = 0;
    float lastNormalWidth = 0;
    int wheel = 0;

    float bars[16] = {};
    ULONGLONG lastMixRefresh = 0;
    UINT frameInterval = 0;

    float shapeX = 0, shapeY = 0, shapeW = 0, shapeH = 0, dx = 0, itemsY = 0;
    int winX = 0, winY = 0;

    // Pop-outs: the island grows to show one at a time.
    struct Activity {
        int kind = 0;   // 1 notification, 2 song, 3 charging, 4 timer
        std::wstring label, title, body;
        ComPtr<ID2D1Bitmap> icon;
        WCHAR glyph = 0;
        D2D1_COLOR_F glyphBg{};
        DWORD ms = 4000;
    };
    std::vector<Activity> activities;
    bool activityShown = false;
    ULONGLONG activityUntil = 0;
    Anim height, act;
    std::wstring lastTitle;
    bool mediaSeen = false;
    bool charging = false, chargingKnown = false;
    float lastLevel = -1;
    bool lastMuted = false;

    bool timerRunning = false, timerPaused = false;
    LONGLONG timerTotal = 5 * 60000, timerLeft = 0;
    ULONGLONG timerEnd = 0;
    bool swRunning = false;
    LONGLONG swAccum = 0;
    ULONGLONG swStart = 0;
    std::vector<LONGLONG> laps;

    std::vector<LyricLine> lyrics;
    int lyricIndex = -1;
    ULONGLONG nextLyricsTry = 0;
};

Island g_is;
LWin g_tip;

void TogglePanel(int kind, D2D1_RECT_F anchor);
void OpenTimePanel(int tab, D2D1_RECT_F anchor);
void ClosePanel();
bool PanelOpen();
int PanelKindOpen();
void RefreshPanel();
bool TickPanel(ULONGLONG now);
void Update();

std::wstring FormatTime(const SYSTEMTIME& st) {
    WCHAR buf[128] = {};
    if (g_s.timeFormat.empty()) {
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, g_s.seconds ? 0 : TIME_NOSECONDS,
                        &st, nullptr, buf, ARRAYSIZE(buf));
    } else {
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_s.timeFormat.c_str(),
                        buf, ARRAYSIZE(buf));
    }
    return buf;
}

std::wstring FormatDate(const SYSTEMTIME& st, PCWSTR format) {
    WCHAR buf[128] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, format && *format ? 0 : DATE_SHORTDATE,
                    &st, format && *format ? format : nullptr, buf, ARRAYSIZE(buf),
                    nullptr);
    return buf;
}

std::wstring Digits0(std::wstring s) {
    for (auto& c : s) {
        if (c >= L'0' && c <= L'9') c = L'0';
    }
    return s;
}

WCHAR VolumeGlyph() {
    auto& a = g_is.audio;
    if (a.muted || !a.endpoint) return Glyph::Mute;
    int step = a.level <= 0.001f ? 0 : a.level < 0.34f ? 1 : a.level < 0.67f ? 2 : 3;
    return Glyph::Volume0 + step;
}

bool OsdActive() { return GetTickCount64() < g_is.osdUntil; }

BgMode CurrentBackground() {
    if (g_s.bgMode != BgMode::AlbumArt) return g_s.bgMode;
    if (g_is.artBg && g_is.media.hasSession &&
        (g_is.media.playing || g_s.artWhenPaused)) {
        return BgMode::AlbumArt;
    }
    return g_s.fallback;
}

void PaintBackground(D2D1_ROUNDED_RECT shape, bool panel, float alpha = 1) {
    BgMode bg = CurrentBackground();
    if (panel && bg == BgMode::AlbumArt && !g_s.panelArt) bg = g_s.fallback;
    float opacity = g_s.opacity * alpha;
    switch (bg) {
        case BgMode::Solid:
            g_gfx.brush->SetColor(WithAlpha(g_s.bg, opacity));
            g_gfx.rt->FillRoundedRectangle(shape, g_gfx.brush.Get());
            break;
        case BgMode::Accent:
            g_gfx.brush->SetColor(WithAlpha(g_is.accent, opacity));
            g_gfx.rt->FillRoundedRectangle(shape, g_gfx.brush.Get());
            break;
        case BgMode::Glass:
            Fill(shape.rect, 0, WithAlpha(g_s.bg, opacity * 0.55f));
            break;
        case BgMode::AlbumArt: {
            FillWithBitmap(g_is.artBg.Get(), shape, g_s.artStretch, opacity);
            float dim = std::min(0.9f, g_s.artDim + (panel ? 0.12f : 0));
            if (g_s.artAutoDim && Luminance(g_s.text) > 0.5f && g_is.artLuma > 0.3f) {
                dim = std::max(dim, 1 - 0.3f / g_is.artLuma);
            }
            g_gfx.brush->SetColor(D2D1::ColorF(0, 0, 0, std::min(dim, 0.92f) * opacity));
            g_gfx.rt->FillRoundedRectangle(shape, g_gfx.brush.Get());
            break;
        }
    }
}

void RebuildArtBitmaps() {
    auto& is = g_is;
    is.artThumb.Reset();
    is.artBg.Reset();
    if (is.artRaw.size() != (size_t)kArtSize * kArtSize) return;
    is.artThumb = MakeBitmap(is.artRaw, kArtSize, kArtSize);
    if (g_s.bgMode == BgMode::AlbumArt) {
        auto bg = MakeArtBackground(is.artRaw);
        is.artLuma = AverageLuminance(bg);
        is.artBg = MakeBitmap(bg, kArtBgSize, kArtBgSize);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Tray icons (from the explorer part)

std::wstring TrayKey(const TrayIcon& t) {
    std::wstring exe = PathFindFileName(ProcessPath(t.pid).c_str());
    CharLowerBuff(exe.data(), (DWORD)exe.size());
    WCHAR guid[64] = L"";
    if (t.hasGuid) StringFromGUID2(t.guid, guid, ARRAYSIZE(guid));
    return exe + L"|" + (t.hasGuid ? std::wstring(guid) : std::to_wstring(t.id));
}

bool IsPinned(const TrayIcon& t) {
    return std::find(g_pinnedTray.begin(), g_pinnedTray.end(), t.key) !=
           g_pinnedTray.end();
}

void LoadPinnedTray() {
    g_pinnedTray.clear();
    WCHAR buf[4096];
    if (!Wh_GetStringValue(L"pinnedTray", buf, ARRAYSIZE(buf))) return;
    std::wstring all = buf;
    size_t start = 0;
    while (start < all.size()) {
        size_t end = all.find(L'\n', start);
        if (end == std::wstring::npos) end = all.size();
        if (end > start) g_pinnedTray.push_back(all.substr(start, end - start));
        start = end + 1;
    }
}

void TogglePin(const TrayIcon& t) {
    auto it = std::find(g_pinnedTray.begin(), g_pinnedTray.end(), t.key);
    if (it != g_pinnedTray.end()) {
        g_pinnedTray.erase(it);
    } else {
        g_pinnedTray.push_back(t.key);
    }
    std::wstring all;
    for (auto& k : g_pinnedTray) all += k + L"\n";
    Wh_SetStringValue(L"pinnedTray", all.c_str());
}

void TrayClick(const TrayIcon& t, UINT what) {
    if (!IsWindow(t.owner) || !t.callback) return;
    AllowSetForegroundWindow(t.pid);
    POINT pt;
    GetCursorPos(&pt);
    bool v4 = t.version >= 4 && t.version != kVersionUnknown;
    auto post = [&](UINT msg) {
        if (v4) {
            PostMessage(t.owner, t.callback, MAKEWPARAM(pt.x, pt.y), MAKELPARAM(msg, t.id));
        } else {
            PostMessage(t.owner, t.callback, t.id, msg);
        }
    };
    switch (what) {
        case WM_LBUTTONUP:
            post(WM_LBUTTONDOWN);
            post(WM_LBUTTONUP);
            if (v4) post(NIN_SELECT);
            break;
        case WM_RBUTTONUP:
            post(WM_RBUTTONDOWN);
            post(WM_RBUTTONUP);
            if (v4) post(WM_CONTEXTMENU);
            break;
        case WM_LBUTTONDBLCLK:
            post(WM_LBUTTONDBLCLK);
            post(WM_LBUTTONUP);
            break;
    }
}

void OnTrayIpc(const TrayIpc& d) {
    auto it = std::find_if(g_tray.begin(), g_tray.end(), [&](const TrayIcon& t) {
        return d.hasGuid ? (t.hasGuid && t.guid == d.guid)
                         : (t.owner == (HWND)(ULONG_PTR)d.owner && t.id == d.id);
    });
    if (d.op == 2) {
        g_tray.clear();
        return;
    }
    if (d.op == 1) {
        if (it != g_tray.end()) g_tray.erase(it);
        return;
    }
    if (it == g_tray.end()) {
        g_tray.push_back({});
        it = g_tray.end() - 1;
    }
    it->owner = (HWND)(ULONG_PTR)d.owner;
    it->id = d.id;
    it->guid = d.guid;
    it->hasGuid = d.hasGuid;
    it->callback = d.callback;
    it->version = d.version;
    it->pid = d.pid;
    it->hidden = d.hidden;
    it->tip.assign(d.tip, wcsnlen(d.tip, 128));
    if (it->key.empty()) it->key = TrayKey(*it);
    it->bmp = BitmapFromIcon((HICON)(ULONG_PTR)d.icon);
    std::erase_if(g_tray, [](const TrayIcon& t) { return !IsWindow(t.owner); });
}

////////////////////////////////////////////////////////////////////////////////
// Timer, stopwatch, lyrics

LONGLONG TimerRemaining() {
    auto& is = g_is;
    if (is.timerRunning) return std::max<LONGLONG>(0, (LONGLONG)(is.timerEnd - GetTickCount64()));
    return is.timerPaused ? is.timerLeft : is.timerTotal;
}

LONGLONG StopwatchElapsed() {
    auto& is = g_is;
    return is.swAccum + (is.swRunning ? (LONGLONG)(GetTickCount64() - is.swStart) : 0);
}

std::wstring FormatMs(LONGLONG ms, bool centis) {
    LONGLONG s = ms / 1000;
    WCHAR buf[32];
    if (centis) {
        swprintf_s(buf, L"%02lld:%02lld.%02lld", s / 60, s % 60, ms / 10 % 100);
    } else if (s >= 3600) {
        swprintf_s(buf, L"%lld:%02lld:%02lld", s / 3600, s / 60 % 60, s % 60);
    } else {
        swprintf_s(buf, L"%lld:%02lld", s / 60, s % 60);
    }
    return buf;
}

double MediaPosition() {
    auto& m = g_is.media;
    double pos = m.position;
    if (m.playing) pos += (GetTickCount64() - m.positionTick) / 1000.0;
    return m.duration > 0 ? std::clamp(pos, 0.0, m.duration) : pos;
}

int CurrentLyricIndex() {
    auto& lines = g_is.lyrics;
    if (lines.empty() || !g_is.media.hasSession) return -1;
    double pos = MediaPosition() + 0.2;
    int index = -1;
    for (int i = 0; i < (int)lines.size() && lines[i].time <= pos; i++) index = i;
    return index;
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Group {
    std::vector<Item> items;
    float w = 0;
};

void BuildLayout() {
    auto& is = g_is;
    const float H = std::round(S(g_s.height));
    const float icon = S(g_s.iconSize);
    const float btn = std::round(icon + S(14));
    const float fs = S(g_s.fontSize);
    const float gap = S(g_s.spacing);
    const float pad = S(g_s.paddingX);
    const float maxW = S(g_s.maxWidth);
    is.H = H;

    auto add = [&](Group& g, Kind kind, float w, Act act) -> Item& {
        Item item;
        item.kind = kind;
        item.act = act;
        item.rc = D2D1::RectF(g.w, 0, g.w + w, H);
        g.items.push_back(std::move(item));
        g.w += w;
        return g.items.back();
    };
    auto glyph = [&](Group& g, WCHAR c, Act act, PCWSTR tip = L"") -> Item& {
        Item& item = add(g, Kind::Glyph, btn, act);
        item.glyph = c;
        item.tip = tip;
        return item;
    };

    std::vector<Group> groups;
    if (OsdActive()) {
        Group g;
        glyph(g, VolumeGlyph(), Act::Volume);
        add(g, Kind::VolumeBar, std::round(S(110)), Act::None);
        g.w += S(8);
        Item& pct = add(g, Kind::Text, TextWidth(L"100%", fs), Act::None);
        pct.text = std::to_wstring((int)std::lround(is.audio.level * 100)) + L"%";
        groups.push_back(std::move(g));
    } else {
        std::vector<int> trayList;
        for (int i = 0; i < (int)g_tray.size(); i++) {
            if (!g_tray[i].hidden && g_tray[i].bmp && !IsPinned(g_tray[i])) {
                trayList.push_back(i);
            }
        }
        int maxTray = (int)trayList.size();
        Group left, center, right;
        for (int attempt = 0; attempt < 64; attempt++) {
            left = Group{};
            center = Group{};
            right = Group{};

            if (g_s.power) glyph(left, Glyph::Power, Act::Power, L"Power");
            if (g_s.notif) {
                ID2D1Bitmap* appIcon =
                    is.notifCount > 0 && !is.notifApp.empty()
                        ? ShellIcon(L"shell:AppsFolder\\" + is.notifApp)
                        : nullptr;
                Item& n = appIcon ? add(left, Kind::Bitmap, btn, Act::Notif)
                                  : glyph(left, Glyph::Bell, Act::Notif);
                n.bmp = appIcon;
                n.tip = L"Notifications";
                if (is.notifCount > 0) {
                    n.badge = is.notifCount > 99 ? L"99+" : std::to_wstring(is.notifCount);
                }
            }
            if (g_s.tray == TrayMode::Inline && !g_tray.empty()) {
                glyph(left, is.trayOpen ? Glyph::ChevronRight : Glyph::ChevronLeft,
                      Act::TrayToggle, is.trayOpen ? L"Hide icons" : L"Show icons");
                if (is.trayOpen) {
                    for (int k = 0; k < std::min(maxTray, (int)trayList.size()); k++) {
                        Item& t = add(left, Kind::Bitmap, btn, Act::Tray);
                        t.index = trayList[k];
                        t.bmp = g_tray[t.index].bmp.Get();
                        t.tip = g_tray[t.index].tip;
                        t.trayGroup = true;
                    }
                }
            } else if (g_s.tray == TrayMode::Panel && !g_tray.empty()) {
                glyph(left, Glyph::ChevronDown, Act::TrayToggle, L"Tray icons");
            }
            if (g_s.tray != TrayMode::Off) {
                for (int i = 0; i < (int)g_tray.size(); i++) {
                    if (g_tray[i].hidden || !g_tray[i].bmp || !IsPinned(g_tray[i])) continue;
                    Item& t = add(left, Kind::Bitmap, btn, Act::Tray);
                    t.index = i;
                    t.bmp = g_tray[i].bmp.Get();
                    t.tip = g_tray[i].tip;
                }
            }
            for (int i = 0; i < (int)g_s.shortcuts.size(); i++) {
                auto& sc = g_s.shortcuts[i];
                Item& t = sc.glyph ? glyph(left, sc.glyph, Act::Shortcut)
                                   : add(left, Kind::Bitmap, btn, Act::Shortcut);
                if (!sc.glyph) t.bmp = sc.icon.Get();
                t.index = i;
                t.tip = PathFindFileName(sc.target.c_str());
            }
            if (g_s.controlCenter) glyph(left, Glyph::Controls, Act::Control, L"Control center");
            if (g_s.volume) {
                glyph(left, VolumeGlyph(), Act::Volume,
                      is.audio.muted ? L"Muted"
                                     : (L"Volume " +
                                        std::to_wstring((int)std::lround(is.audio.level * 100)) +
                                        L"%")
                                           .c_str());
            }
            if (g_s.network) {
                Item& n = glyph(left,
                                is.status.net == Status::Wifi       ? Glyph::Wifi
                                : is.status.net == Status::Ethernet ? Glyph::Ethernet
                                                                    : Glyph::Globe,
                                Act::Network, L"Network");
                if (is.status.net == Status::Offline) n.alpha = 0.4f;
            }
            if (g_s.battery && is.status.hasBattery) {
                int step = std::clamp(is.status.batteryPercent / 10, 0, 10);
                Item& b = glyph(left,
                                (is.status.charging ? Glyph::BatteryCharging0
                                                    : Glyph::Battery0) +
                                    step,
                                Act::Battery);
                b.tip = std::to_wstring(is.status.batteryPercent) + L"%" +
                        (is.status.charging ? L", charging" : L"");
            }
            if (g_s.weather && is.weather.ok) {
                std::wstring temp = std::to_wstring((int)std::lround(is.weather.temp)) + L"°";
                Item& w = add(left, Kind::Emoji,
                              std::round(icon * 1.3f + S(4) + TextWidth(temp, fs, 500) + S(12)),
                              Act::Weather);
                w.text = WeatherEmoji(is.weather.code, is.weather.day);
                w.text2 = temp;
                w.tip = WeatherWords(is.weather.code);
            }

            auto chip = [&](WCHAR g, const std::wstring& text, Act act) {
                float w = std::round(icon + S(6) + TextWidth(Digits0(text), fs, 600) + S(20));
                Item& c = add(center, Kind::Chip, w, act);
                c.glyph = g;
                c.text = text;
            };
            if (is.timerRunning || is.timerPaused) {
                chip(kGlyphTimer, FormatMs(TimerRemaining() + 999, false), Act::Timer);
            }
            if (is.swRunning) chip(kGlyphStopwatch, FormatMs(StopwatchElapsed(), false), Act::Stopwatch);
            if (g_s.clock) {
                SYSTEMTIME st;
                GetLocalTime(&st);
                Item& c = add(center, Kind::Clock, 0, Act::Clock);
                c.text = g_s.date ? FormatDate(st, g_s.dateFormat.c_str()) : L"";
                c.text2 = FormatTime(st);
                float w = TextWidth(Digits0(c.text2), fs, g_s.boldClock ? 600 : 400) + S(2);
                if (!c.text.empty()) w += TextWidth(c.text, fs) + S(10);
                w = std::round(w + S(16));
                c.rc.right = c.rc.left + w;
                center.w = c.rc.right;
            }

            if (g_s.media && is.media.hasSession) {
                std::wstring text = is.media.title;
                if (g_s.mediaArtist && !is.media.artist.empty()) {
                    text += L" · " + is.media.artist;
                }
                float textW = std::min(TextWidth(text, fs), S(g_s.mediaMaxText));
                if (g_s.lyrics && is.media.playing && is.lyricIndex >= 0 &&
                    !is.lyrics[is.lyricIndex].text.empty()) {
                    text = L"♪  " + is.lyrics[is.lyricIndex].text;
                    textW = S(g_s.mediaMaxText);
                }
                float artW = g_s.mediaArt ? std::round(H * 0.6f) + S(8) : 0;
                Item& m = add(right, Kind::Media, std::round(artW + textW + S(16)), Act::Media);
                m.text = text;
                if (g_s.mediaControls) {
                    Item& p = glyph(right, Glyph::Previous, Act::Prev);
                    p.alpha = is.media.canPrev ? 1 : 0.4f;
                    glyph(right, is.media.playing ? Glyph::Pause : Glyph::Play, Act::Play);
                    Item& n = glyph(right, Glyph::Next, Act::Next);
                    n.alpha = is.media.canNext ? 1 : 0.4f;
                }
                if (g_s.visualizer) {
                    add(right, Kind::Viz,
                        std::round(g_s.vizBars * S(2.5f) + (g_s.vizBars - 1) * S(2) + S(14)),
                        Act::Media);
                }
            }
            if (g_s.collapseButton) {
                glyph(right, is.folded ? Glyph::ChevronDown : Glyph::ChevronUp, Act::Fold,
                      is.folded ? L"Keep open" : L"Fold");
            }

            float content = left.w + center.w + right.w;
            int n = !left.items.empty() + !center.items.empty() + !right.items.empty();
            content += gap * std::max(0, n - 1);
            float over = content + 2 * pad - maxW;
            if (over > 0 && is.trayOpen && maxTray > 0) {
                maxTray--;
                continue;
            }
            if (over > 0) {
                // Shrink the media title.
                for (size_t i = 0; i < right.items.size(); i++) {
                    if (right.items[i].kind != Kind::Media) continue;
                    float shrink = std::min(over, right.items[i].rc.right -
                                                      right.items[i].rc.left - S(80));
                    if (shrink <= 0) break;
                    right.items[i].rc.right -= shrink;
                    for (size_t j = i + 1; j < right.items.size(); j++) {
                        right.items[j].rc.left -= shrink;
                        right.items[j].rc.right -= shrink;
                    }
                    right.w -= shrink;
                    break;
                }
            }
            break;
        }
        for (Group* g : {&left, &center, &right}) {
            if (!g->items.empty()) groups.push_back(std::move(*g));
        }
    }

    float content = 0;
    for (auto& g : groups) content += g.w;
    content += gap * std::max(0, (int)groups.size() - 1);
    float W = std::clamp(content + 2 * pad, S(g_s.minWidth), maxW);
    if (OsdActive()) {
        W = std::max(W, is.lastNormalWidth);
    } else {
        is.lastNormalWidth = W;
    }
    W = std::round(W);
    if (is.activityShown) W = std::max(W, std::round(S(380)));
    is.items.clear();
    float x = std::round((W - content) / 2);
    for (auto& g : groups) {
        for (auto& item : g.items) {
            item.rc.left += x;
            item.rc.right += x;
            is.items.push_back(std::move(item));
        }
        x += g.w + gap;
    }
    is.targetW = W;
    if (is.hover >= (int)is.items.size()) is.hover = -1;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

void DrawBadge(D2D1_RECT_F iconRc, const std::wstring& text) {
    float h = std::round(S(13));
    float w = std::max(h, TextWidth(text, S(9), 700) + S(7));
    D2D1_RECT_F rc = D2D1::RectF(iconRc.right - w * 0.6f, iconRc.top - S(4),
                                 iconRc.right - w * 0.6f + w, iconRc.top - S(4) + h);
    Fill(rc, h / 2, D2D1::ColorF(0xE5484D));
    DrawStr(text, rc, D2D1::ColorF(D2D1::ColorF::White), S(9), 700, kCenter);
}

void DrawVisualizer(D2D1_RECT_F rc, float alpha) {
    int n = g_s.vizBars;
    float bw = S(2.5f), gap = S(2);
    float total = n * bw + (n - 1) * gap;
    float x = std::round((rc.left + rc.right - total) / 2);
    float cy = (rc.top + rc.bottom) / 2;
    float maxH = (rc.bottom - rc.top) * 0.46f;
    D2D1_COLOR_F color = WithAlpha(g_s.vizAccent ? g_is.accent : g_s.text, alpha);
    for (int i = 0; i < n; i++) {
        float h = std::max(bw, g_is.bars[i] * maxH);
        Fill(D2D1::RectF(x, cy - h / 2, x + bw, cy + h / 2), bw / 2, color);
        x += bw + gap;
    }
}

void DrawActivity(D2D1_RECT_F shape, float a) {
    auto& is = g_is;
    if (is.activities.empty() || a <= 0.01f) return;
    const auto& ac = is.activities.front();
    float h = shape.bottom - shape.top;
    float pad = S(14);
    float side = std::round(h - 2 * S(12));
    if (side < S(20)) return;
    D2D1_RECT_F ir = D2D1::RectF(shape.left + pad, shape.top + (h - side) / 2,
                                 shape.left + pad + side, shape.top + (h + side) / 2);
    if (ac.icon && ac.kind == 2) {
        FillWithBitmap(ac.icon.Get(), D2D1::RoundedRect(ir, S(10), S(10)), false, a);
    } else if (ac.icon) {
        Fill(ir, S(12), WithAlpha(g_s.text, 0.1f * a));
        float s = std::round(side * 0.62f);
        DrawBitmap(ac.icon.Get(), Centered(ir, s, s), a);
    } else {
        Fill(ir, side / 2, WithAlpha(ac.glyphBg, a));
        DrawGlyph(ac.glyph ? ac.glyph : Glyph::Bell, ir, WithAlpha(g_s.text, a), side * 0.42f);
    }
    float tx = ir.right + S(12), right = shape.right - pad - S(4);
    float lh = ac.label.empty() ? 0 : S(16), th = S(20), bh = ac.body.empty() ? 0 : S(18);
    float top = std::round(shape.top + (h - lh - th - bh) / 2);
    if (lh) {
        DrawStr(ac.label, D2D1::RectF(tx, top, right, top + lh), WithAlpha(g_s.text, 0.6f * a),
                S(11), 500);
    }
    top += lh;
    DrawStr(ac.title, D2D1::RectF(tx, top, right, top + th), WithAlpha(g_s.text, a), S(14), 600);
    top += th;
    if (bh) {
        DrawStr(ac.body, D2D1::RectF(tx, top, right, top + bh), WithAlpha(g_s.text, 0.75f * a),
                S(12.5f));
    }
}

void DrawIsland(bool vizOnly) {
    auto& is = g_is;
    const float f = is.fold.value;
    D2D1_RECT_F shape = D2D1::RectF(is.shapeX, is.shapeY, is.shapeX + is.shapeW,
                                    is.shapeY + is.shapeH);
    float radius = std::min(S(g_s.radius), is.shapeH / 2);
    radius = Lerp(radius, is.shapeH / 2, f);
    D2D1_ROUNDED_RECT rr{shape, radius, radius};

    if (f > 0.5f) {
        // Invisible, but catches the mouse around the line.
        Fill(D2D1::RectF(0, 0, (float)is.win.w, (float)is.win.h), 0,
             D2D1::ColorF(0, 0, 0, 1 / 255.f));
    }
    PaintBackground(rr, false, 1 - f * 0.6f);
    if (f > 0) Fill(shape, radius, WithAlpha(g_s.text, 0.5f * f));
    if (g_s.borderWidth > 0 && !is.win.glass && f < 1) {
        float bw = S(g_s.borderWidth);
        Stroke(D2D1::RectF(shape.left + bw / 2, shape.top + bw / 2,
                           shape.right - bw / 2, shape.bottom - bw / 2),
               std::max(0.f, radius - bw / 2), WithAlpha(g_s.border, 1 - f), bw);
    }

    float alpha = std::clamp(1 - f * 2.5f, 0.f, 1.f);
    if (alpha <= 0) return;
    float actA = is.act.value * alpha;
    alpha *= 1 - is.act.value;

    bool clip = is.width.Active() || f > 0 || is.height.Active() || is.act.value > 0;
    ComPtr<ID2D1RoundedRectangleGeometry> geometry;
    if (clip && g_gfx.layer &&
        SUCCEEDED(g_gfx.d2d->CreateRoundedRectangleGeometry(rr, &geometry))) {
        g_gfx.rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geometry.Get()),
                            g_gfx.layer.Get());
    } else {
        clip = false;
    }

    if (!vizOnly) DrawActivity(shape, actA);
    const float H = is.H;
    if (!vizOnly && is.hlA.value > 0.01f) {
        float hh = H - S(8);
        D2D1_RECT_F hl = D2D1::RectF(is.dx + is.hlX.value, is.itemsY + (H - hh) / 2,
                                     is.dx + is.hlX.value + is.hlW.value,
                                     is.itemsY + (H + hh) / 2);
        Fill(hl, hh / 2, WithAlpha(g_s.icons, 0.14f * is.hlA.value * alpha));
    }

    const float icon = S(g_s.iconSize);
    const float fs = S(g_s.fontSize);
    for (const Item& item : is.items) {
        if (vizOnly && item.kind != Kind::Viz) continue;
        D2D1_RECT_F rc = Offset(item.rc, is.dx, is.itemsY);
        float a = item.alpha * alpha * (item.trayGroup ? is.trayFade.value : 1);
        if (a <= 0.01f) continue;
        switch (item.kind) {
            case Kind::Glyph:
                DrawGlyph(item.glyph, rc, WithAlpha(g_s.icons, a), icon);
                if (!item.badge.empty()) DrawBadge(Centered(rc, icon, icon), item.badge);
                break;
            case Kind::Bitmap: {
                float side = std::round(icon * 1.15f);
                D2D1_RECT_F ir = Centered(rc, side, side);
                DrawBitmap(item.bmp, ir, a);
                if (!item.badge.empty()) DrawBadge(ir, item.badge);
                break;
            }
            case Kind::Clock: {
                float timeW = TextWidth(Digits0(item.text2), fs, g_s.boldClock ? 600 : 400) + S(2);
                D2D1_RECT_F inner = D2D1::RectF(rc.left + S(8), rc.top, rc.right - S(8), rc.bottom);
                if (!item.text.empty()) {
                    DrawStr(item.text, D2D1::RectF(inner.left, rc.top, inner.right - timeW, rc.bottom),
                            WithAlpha(g_s.text, 0.85f * a), fs, 400, kLeft);
                }
                DrawStr(item.text2, D2D1::RectF(inner.right - timeW, rc.top, inner.right, rc.bottom),
                        WithAlpha(g_s.text, a), fs, g_s.boldClock ? 600 : 400, kCenter);
                break;
            }
            case Kind::Media: {
                float x = rc.left + S(8);
                if (g_s.mediaArt) {
                    float side = std::round(H * 0.6f);
                    D2D1_RECT_F ar = D2D1::RectF(x, rc.top + (H - side) / 2, x + side,
                                                 rc.top + (H + side) / 2);
                    if (is.artThumb) {
                        FillWithBitmap(is.artThumb.Get(), D2D1::RoundedRect(ar, S(4), S(4)),
                                       false, a);
                    } else {
                        Fill(ar, S(4), WithAlpha(g_s.text, 0.15f * a));
                        DrawGlyph(Glyph::Music, ar, WithAlpha(g_s.icons, a), side * 0.55f);
                    }
                    x = ar.right + S(8);
                }
                DrawStr(item.text, D2D1::RectF(x, rc.top, rc.right - S(8), rc.bottom),
                        WithAlpha(g_s.text, a), fs);
                break;
            }
            case Kind::Emoji: {
                D2D1_RECT_F er = D2D1::RectF(rc.left + S(6), rc.top, rc.left + S(6) + icon * 1.3f,
                                             rc.bottom);
                DrawEmoji(item.text, er, icon * 1.05f, kCenter, a);
                DrawStr(item.text2, D2D1::RectF(er.right + S(4), rc.top, rc.right, rc.bottom),
                        WithAlpha(g_s.text, a), fs, 500);
                break;
            }
            case Kind::Viz:
                DrawVisualizer(rc, a);
                break;
            case Kind::VolumeBar: {
                float bh = std::max(3.f, std::round(S(4)));
                D2D1_RECT_F track = D2D1::RectF(rc.left, (rc.top + rc.bottom - bh) / 2, rc.right,
                                                (rc.top + rc.bottom + bh) / 2);
                Fill(track, bh / 2, WithAlpha(g_s.text, 0.25f * a));
                float level = is.audio.muted ? 0 : is.audio.level;
                if (level > 0) {
                    track.right = rc.left + std::max(bh, (rc.right - rc.left) * level);
                    Fill(track, bh / 2, WithAlpha(is.accent, a));
                }
                break;
            }
            case Kind::Text:
                DrawStr(item.text, rc, WithAlpha(g_s.text, a), fs, 400, kCenter);
                break;
            case Kind::Chip: {
                float ch = std::round(H - S(12));
                D2D1_RECT_F pill = D2D1::RectF(rc.left + S(2), rc.top + (H - ch) / 2, rc.right - S(2),
                                               rc.top + (H + ch) / 2);
                Fill(pill, ch / 2, WithAlpha(is.accent, 0.22f * a));
                D2D1_RECT_F gr = D2D1::RectF(pill.left + S(6), pill.top, pill.left + S(6) + icon,
                                             pill.bottom);
                DrawGlyph(item.glyph, gr, WithAlpha(g_s.icons, 0.85f * a), icon * 0.85f);
                DrawStr(item.text, D2D1::RectF(gr.right + S(4), pill.top, pill.right - S(6), pill.bottom),
                        WithAlpha(g_s.text, a), fs, 600, kCenter);
                break;
            }
        }
    }
    if (clip) g_gfx.rt->PopLayer();
}

void ApplyAppbar();

void RenderIsland(bool vizOnly = false) {
    auto& is = g_is;
    if (!is.win.hwnd || !g_gfx.rt) return;
    const float f = is.fold.value;
    const float lineW = S(64), lineH = std::max(3.f, std::round(S(4)));
    is.shapeW = std::round(Lerp(is.width.value, lineW, f));
    is.shapeH = std::round(Lerp(is.height.value, lineH, f));
    float top = Lerp(S(g_s.topOffset), S(3), f);
    bool pad = f > 0.5f;
    int winW = (int)std::ceil(pad ? std::max(is.shapeW, lineW + S(90)) : is.shapeW);
    float winTop = pad ? 0 : top;
    int winH = (int)std::ceil(pad ? std::max(top + is.shapeH + S(6), S(16)) : is.shapeH);
    is.shapeX = std::round((winW - is.shapeW) / 2);
    is.shapeY = std::round(top - winTop);
    is.dx = is.shapeX + std::round((is.shapeW - is.targetW) / 2);
    is.itemsY = is.shapeY + std::round((is.shapeH - is.H) / 2);
    int monW = is.monitor.right - is.monitor.left;
    int x = is.monitor.left + (monW - winW) / 2;
    int y = is.monitor.top + (int)std::round(winTop);

    bool glass = CurrentBackground() == BgMode::Glass && f == 0;
    if (glass != is.win.glass) {
        is.win.SetGlass(glass, g_s.radius);
        vizOnly = false;
    }
    if (winW != is.win.w || winH != is.win.h || x != is.win.pos.x || y != is.win.pos.y) {
        vizOnly = false;
    }
    if (!is.win.Ensure(winW, winH)) return;

    D2D1_RECT_F vizRect{};
    if (vizOnly) {
        auto it = std::find_if(is.items.begin(), is.items.end(),
                               [](const Item& i) { return i.kind == Kind::Viz; });
        if (it == is.items.end() || is.act.value > 0) return;
        vizRect = Offset(it->rc, is.dx, is.itemsY);
    }
    is.win.Begin(vizOnly ? &vizRect : nullptr);
    DrawIsland(vizOnly);
    if (!is.win.End(vizOnly ? &vizRect : nullptr)) return;

    RECT dirty{(LONG)vizRect.left, (LONG)vizRect.top, (LONG)std::ceil(vizRect.right),
               (LONG)std::ceil(vizRect.bottom)};
    is.win.Present(x, y, 255, vizOnly && !is.win.glass ? &dirty : nullptr);
    is.win.Show(!(g_s.hideFullscreen && is.fullscreen));
}

////////////////////////////////////////////////////////////////////////////////
// Animation frames and the visualizer

bool UpdateVisualizer(ULONGLONG now) {
    auto& is = g_is;
    bool run = g_s.visualizer && g_s.media && is.media.hasSession && is.media.playing &&
               is.fold.value < 0.5f && is.win.visible && !OsdActive();
    float peak = 0;
    if (run) {
        if (now - is.lastMixRefresh > 5000) {
            is.audio.RefreshMix();
            is.lastMixRefresh = now;
        }
        peak = std::pow(std::clamp(is.audio.Peak(is.media.app), 0.f, 1.f), 0.8f);
    }
    float t = (now % 100000) / 1000.f;
    bool moving = false;
    for (int i = 0; i < g_s.vizBars; i++) {
        float wave = std::fabs(std::sin(t * (2.3f + i * 0.9f) + i * 1.7f)) *
                     (0.6f + 0.4f * std::fabs(std::sin(t * (1.1f + i * 0.37f))));
        float target = run ? std::clamp(peak * (0.2f + 0.8f * wave), 0.f, 1.f) : 0;
        float k = target > is.bars[i] ? 0.6f : 0.2f;
        is.bars[i] += (target - is.bars[i]) * k;
        if (is.bars[i] < 0.004f) {
            is.bars[i] = 0;
        } else {
            moving = true;
        }
    }
    return run || moving;
}

void SetFrameInterval(UINT ms) {
    if (g_is.frameInterval == ms) return;
    g_is.frameInterval = ms;
    if (ms) {
        SetTimer(g_is.win.hwnd, kTimerFrame, ms, nullptr);
    } else {
        KillTimer(g_is.win.hwnd, kTimerFrame);
    }
}

void RequestFrame() {
    if (g_is.win.hwnd && g_is.frameInterval != 15) SetFrameInterval(15);
}

void StartVisualizerIfNeeded() {
    if (g_s.visualizer && g_is.media.playing && g_is.frameInterval == 0) {
        SetFrameInterval(33);
    }
}

void OnFrame() {
    auto& is = g_is;
    ULONGLONG now = GetTickCount64();
    bool animating = false;
    for (Anim* a : {&is.width, &is.fold, &is.hlX, &is.hlW, &is.hlA, &is.trayFade, &is.height,
                    &is.act}) {
        if (a->Active()) {
            a->Tick(now);
            animating = true;
        }
    }
    bool panelAnimating = TickPanel(now);
    bool viz = UpdateVisualizer(now);
    if (animating) {
        RenderIsland();
    } else if (viz) {
        RenderIsland(true);
    }
    if (animating || panelAnimating) {
        SetFrameInterval(15);
    } else if (viz) {
        SetFrameInterval(33);
    } else {
        SetFrameInterval(0);
    }
}

void Update() {
    auto& is = g_is;
    BuildLayout();
    float targetH = is.activityShown ? std::round(S(76)) : is.H;
    if (is.width.value == 0) {
        is.width.Snap(is.targetW);
        is.height.Snap(targetH);
    } else {
        is.width.Go(is.targetW, 240);
        is.height.Go(targetH, 260);
    }
    if (is.hover >= 0 && is.hover < (int)is.items.size()) {
        const Item& h = is.items[is.hover];
        is.hlX.Go(h.rc.left, 120);
        is.hlW.Go(h.rc.right - h.rc.left, 120);
    }
    RenderIsland();
    StartVisualizerIfNeeded();
}

////////////////////////////////////////////////////////////////////////////////
// Tooltip

void HideTip() {
    KillTimer(g_is.win.hwnd, kTimerTip);
    g_tip.Show(false);
}

void ShowTip(const std::wstring& text, D2D1_RECT_F anchor) {
    if (text.empty() || !g_tip.hwnd || PanelOpen()) return;
    float fs = S(12);
    float w = std::round(TextWidth(text, fs) + S(20));
    w = std::min(w, S(360));
    float h = std::round(S(26));
    if (!g_tip.Ensure((int)w, (int)h)) return;
    g_tip.Begin();
    D2D1_RECT_F rc = D2D1::RectF(0, 0, w, h);
    Fill(rc, h / 2, D2D1::ColorF(0.1f, 0.1f, 0.11f, 0.96f));
    Stroke(D2D1::RectF(0.5f, 0.5f, w - 0.5f, h - 0.5f), h / 2 - 0.5f,
           D2D1::ColorF(1, 1, 1, 0.1f), 1);
    DrawStr(text, D2D1::RectF(S(10), 0, w - S(10), h), D2D1::ColorF(D2D1::ColorF::White), fs,
            400, kCenter);
    if (!g_tip.End()) return;
    int x = (int)std::round((anchor.left + anchor.right - w) / 2);
    x = std::clamp(x, (int)g_is.work.left + 4, (int)g_is.work.right - (int)w - 4);
    int y = (int)std::round(anchor.bottom + S(6));
    g_tip.Present(x, y);
    g_tip.Show(true);
}

D2D1_RECT_F ItemScreenRect(const Item& item) {
    auto& is = g_is;
    return Offset(item.rc, is.win.pos.x + is.dx, is.win.pos.y + is.itemsY);
}

////////////////////////////////////////////////////////////////////////////////
// Input

int HitTest(int x, int y) {
    auto& is = g_is;
    if (is.activityShown) return -1;
    float fx = x - is.dx, fy = y - is.itemsY;
    for (int i = 0; i < (int)is.items.size(); i++) {
        const auto& it = is.items[i];
        if (it.act == Act::None) continue;
        if (it.trayGroup && is.trayFade.value < 0.5f) continue;
        if (fx >= it.rc.left && fx < it.rc.right && fy >= -S(4) && fy < is.H + S(4)) {
            return i;
        }
    }
    return -1;
}

void SetHover(int hit) {
    auto& is = g_is;
    if (hit == is.hover) return;
    bool had = is.hover >= 0;
    is.hover = hit;
    HideTip();
    if (hit >= 0) {
        const Item& h = is.items[hit];
        if (had) {
            is.hlX.Go(h.rc.left, 140);
            is.hlW.Go(h.rc.right - h.rc.left, 140);
        } else {
            is.hlX.Snap(h.rc.left);
            is.hlW.Snap(h.rc.right - h.rc.left);
        }
        is.hlA.Go(1, 120);
        if (!h.tip.empty()) SetTimer(is.win.hwnd, kTimerTip, 550, nullptr);
    } else {
        is.hlA.Go(0, 150);
    }
    RequestFrame();
}

////////////////////////////////////////////////////////////////////////////////
// Pop-outs

void ShowActivity() {
    auto& is = g_is;
    HWND hwnd = is.win.hwnd;
    if (is.activities.empty() || PanelOpen()) {
        if (is.activityShown) {
            is.activityShown = false;
            is.act.Go(0, 200);
            KillTimer(hwnd, kTimerActivity);
            if (is.folded && !is.peeking) is.fold.Go(1, 260);
            Update();
        }
        return;
    }
    bool was = is.activityShown;
    is.activityShown = true;
    is.activityUntil = GetTickCount64() + is.activities.front().ms;
    is.act.Go(1, 260);
    if (is.fold.to > 0) is.fold.Go(0, 260);
    is.hover = -1;
    is.hlA.Snap(0);
    HideTip();
    SetTimer(hwnd, kTimerActivity, 200, nullptr);
    if (was) {
        RenderIsland();
    } else {
        Update();
    }
}

void PushActivity(Island::Activity a) {
    auto& is = g_is;
    if (g_s.hideFullscreen && is.fullscreen && a.kind != 4) return;
    // A newer song or charger state replaces the one waiting.
    if (a.kind == 2 || a.kind == 3) {
        for (size_t i = 0; i < is.activities.size(); i++) {
            if (is.activities[i].kind == a.kind) {
                is.activities[i] = a;
                if (i == 0 && is.activityShown) {
                    is.activityUntil = GetTickCount64() + a.ms;
                    RenderIsland();
                }
                return;
            }
        }
    }
    is.activities.push_back(std::move(a));
    if (is.activities.size() > 5) is.activities.erase(is.activities.begin() + 1);
    if (!is.activityShown) ShowActivity();
}

void NextActivity() {
    auto& is = g_is;
    if (!is.activities.empty()) is.activities.erase(is.activities.begin());
    if (is.activities.empty()) {
        ShowActivity();
    } else {
        is.activityUntil = GetTickCount64() + is.activities.front().ms;
        is.act.Snap(0.35f);
        is.act.Go(1, 200);
        RenderIsland();
    }
}

void OnActivityTimer() {
    auto& is = g_is;
    if (!is.activityShown) {
        KillTimer(is.win.hwnd, kTimerActivity);
        return;
    }
    POINT pt;
    GetCursorPos(&pt);
    RECT rc;
    GetWindowRect(is.win.hwnd, &rc);
    ULONGLONG now = GetTickCount64();
    if (PtInRect(&rc, pt)) is.activityUntil = std::max(is.activityUntil, now + 1200);
    if (now >= is.activityUntil) NextActivity();
}

void OnActivityClick() {
    auto& is = g_is;
    if (is.activities.empty()) return;
    int kind = is.activities.front().kind;
    D2D1_RECT_F anchor = D2D1::RectF((float)is.win.pos.x, (float)is.win.pos.y,
                                     (float)is.win.pos.x + is.win.w,
                                     (float)is.win.pos.y + is.shapeY + is.H);
    is.activities.clear();
    ShowActivity();
    if (kind == 1) SendWinHotkey('N');
    if (kind == 2) TogglePanel(2, anchor);
}

void OnTrackChanged() {
    auto& is = g_is;
    if (!g_s.popTrack || !is.media.playing) return;
    Island::Activity a;
    a.kind = 2;
    a.label = L"Now playing";
    a.title = is.media.title;
    a.body = is.media.artist;
    a.icon = is.artThumb;
    a.glyph = Glyph::Music;
    a.glyphBg = WithAlpha(is.accent, 0.5f);
    a.ms = 3000;
    PushActivity(std::move(a));
}

void CheckCharging() {
    auto& is = g_is;
    auto& st = is.status;
    if (!st.hasBattery) return;
    if (is.chargingKnown && st.charging != is.charging && g_s.popCharging) {
        Island::Activity a;
        a.kind = 3;
        a.label = L"Battery";
        a.title = st.charging ? L"Charging" : L"On battery";
        a.body = std::to_wstring(st.batteryPercent) + L"%";
        a.glyph = st.charging ? kGlyphCharging
                              : Glyph::Battery0 + std::clamp(st.batteryPercent / 10, 0, 10);
        a.glyphBg = st.charging ? D2D1::ColorF(0x30D158) : WithAlpha(g_s.text, 0.2f);
        a.ms = 2500;
        PushActivity(std::move(a));
    }
    is.charging = st.charging;
    is.chargingKnown = true;
}

void ArmClockTimer();

void TimerStartPause() {
    auto& is = g_is;
    ULONGLONG now = GetTickCount64();
    if (is.timerRunning) {
        is.timerLeft = TimerRemaining();
        is.timerRunning = false;
        is.timerPaused = true;
        KillTimer(is.win.hwnd, kTimerAlarm);
    } else {
        LONGLONG left = is.timerPaused ? is.timerLeft : is.timerTotal;
        is.timerEnd = now + left;
        is.timerRunning = true;
        is.timerPaused = false;
        SetTimer(is.win.hwnd, kTimerAlarm, (UINT)left, nullptr);
    }
    ArmClockTimer();
    Update();
}

void TimerCancel() {
    auto& is = g_is;
    is.timerRunning = is.timerPaused = false;
    KillTimer(is.win.hwnd, kTimerAlarm);
    Update();
}

void TimerFinished() {
    auto& is = g_is;
    KillTimer(is.win.hwnd, kTimerAlarm);
    if (!is.timerRunning) return;
    if (TimerRemaining() > 50) {
        SetTimer(is.win.hwnd, kTimerAlarm, (UINT)TimerRemaining(), nullptr);
        return;
    }
    is.timerRunning = false;
    Island::Activity a;
    a.kind = 4;
    a.label = L"Timer";
    a.title = L"Time's up";
    a.body = FormatMs(is.timerTotal, false) + L" timer";
    a.glyph = kGlyphTimer;
    a.glyphBg = D2D1::ColorF(0xFF9F0A);
    a.ms = 12000;
    PushActivity(std::move(a));
    PlaySound(L"SystemNotification", nullptr, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
    Update();
}

void StopwatchToggle() {
    auto& is = g_is;
    if (is.swRunning) {
        is.swAccum = StopwatchElapsed();
        is.swRunning = false;
    } else {
        is.swStart = GetTickCount64();
        is.swRunning = true;
    }
    ArmClockTimer();
    Update();
}

void StopwatchLapReset() {
    auto& is = g_is;
    if (is.swRunning) {
        is.laps.insert(is.laps.begin(), StopwatchElapsed());
        if (is.laps.size() > 50) is.laps.pop_back();
    } else {
        is.swAccum = 0;
        is.laps.clear();
    }
    Update();
}

void UpdateLyricsTimer() {
    auto& is = g_is;
    if (g_s.lyrics && !is.lyrics.empty() && is.media.playing) {
        SetTimer(is.win.hwnd, kTimerLyrics, 200, nullptr);
    } else {
        KillTimer(is.win.hwnd, kTimerLyrics);
    }
}

void OnLyricsTick() {
    int index = CurrentLyricIndex();
    if (index != g_is.lyricIndex) {
        g_is.lyricIndex = index;
        Update();
        if (PanelKindOpen() == 2) RefreshPanel();
    }
}

void Unfold() {
    g_is.folded = false;
    g_is.peeking = false;
    g_is.fold.Go(0, 260);
    Update();
}

void Fold() {
    ClosePanel();
    HideTip();
    g_is.folded = true;
    g_is.peeking = false;
    g_is.hover = -1;
    g_is.hlA.Snap(0);
    g_is.fold.Go(1, 260);
    Update();
}

void DoAction(const Item& item) {
    auto& is = g_is;
    D2D1_RECT_F anchor = ItemScreenRect(item);
    HideTip();
    switch (item.act) {
        case Act::Power: TogglePanel(4, anchor); break;
        case Act::Notif: SendWinHotkey('N'); break;
        case Act::TrayToggle:
            if (g_s.tray == TrayMode::Panel) {
                TogglePanel(6, anchor);
            } else {
                is.trayOpen = !is.trayOpen;
                is.trayFade.Snap(0);
                if (is.trayOpen) is.trayFade.Go(1, 260);
                Update();
            }
            break;
        case Act::Tray:
            if (item.index < (int)g_tray.size()) TrayClick(g_tray[item.index], WM_LBUTTONUP);
            break;
        case Act::Shortcut:
            if (item.index < (int)g_s.shortcuts.size()) {
                Launch(g_s.shortcuts[item.index].target, g_s.shortcuts[item.index].args);
            }
            break;
        case Act::Control:
        case Act::Network:
        case Act::Battery: TogglePanel(3, anchor); break;
        case Act::Volume:
        case Act::Media: TogglePanel(2, anchor); break;
        case Act::Weather: TogglePanel(5, anchor); break;
        case Act::Clock: OpenTimePanel(0, anchor); break;
        case Act::Prev: g_mediaCommand = 1; SetEvent(g_workEvent); break;
        case Act::Play: g_mediaCommand = 2; SetEvent(g_workEvent); break;
        case Act::Next: g_mediaCommand = 3; SetEvent(g_workEvent); break;
        case Act::Timer: OpenTimePanel(1, anchor); break;
        case Act::Stopwatch: OpenTimePanel(2, anchor); break;
        case Act::Fold:
            if (is.folded) {
                Unfold();
            } else {
                Fold();
            }
            break;
        case Act::None: break;
    }
}

void OnMediaChanged() {
    auto& is = g_is;
    bool appChanged;
    AcquireSRWLockShared(&g_mediaLock);
    appChanged = g_mediaShared.app != is.media.app;
    is.media = g_mediaShared;
    ReleaseSRWLockShared(&g_mediaLock);
    if (is.media.artVersion != is.artVersion) {
        is.artVersion = is.media.artVersion;
        is.artRaw = std::move(is.media.art);
        RebuildArtBitmaps();
        // The cover often arrives a moment after the song.
        if (!is.activities.empty() && is.activities.front().kind == 2 &&
            is.activities.front().title == is.media.title) {
            is.activities.front().icon = is.artThumb;
        }
    }
    is.media.art.clear();
    if (appChanged) {
        is.audio.RefreshMix();
        is.lastMixRefresh = GetTickCount64();
    }
    if (is.media.hasSession && is.media.title != is.lastTitle) {
        if (is.mediaSeen) OnTrackChanged();
        is.lastTitle = is.media.title;
        is.mediaSeen = true;
        is.lyricIndex = -1;
        if (g_s.lyrics) {
            RequestLyrics(is.media.title, is.media.artist, is.media.album, is.media.duration);
        }
    }
    UpdateLyricsTimer();
    Update();
    RefreshPanel();
}

////////////////////////////////////////////////////////////////////////////////
// Panels (one at a time, under the island)

enum PanelKind { kNone = 0, kCalendar = 1, kMediaPanel = 2, kControl = 3, kPower = 4,
                 kWeatherPanel = 5, kTrayPanel = 6 };

enum PanelAct {
    PA_None, PA_CalPrev, PA_CalNext, PA_CalToday, PA_Seek, PA_Prev, PA_Play,
    PA_Next, PA_Shuffle, PA_Repeat, PA_AppVol, PA_AppMute, PA_Vol, PA_Mute,
    PA_OpenMixer, PA_MixerToggle, PA_SessVol, PA_SessMute, PA_Wifi, PA_Bt,
    PA_Mic, PA_Dark, PA_Bright, PA_Settings, PA_Quick, PA_PowerPanel,
    PA_Power, PA_TrayIcon, PA_TrayPin, PA_OpenMeteo, PA_DateSettings, PA_Tab,
    PA_TimerPreset, PA_TimerAdd, PA_TimerStart, PA_TimerCancel, PA_SwToggle, PA_SwLap
};

enum class WK { Text, Glyph, Emoji, Bitmap, Art, Fill, Slider, Tile, Button, Cell, Line, Range,
                Tab, Pill };

struct Widget {
    WK kind = WK::Text;
    D2D1_RECT_F rc{};
    int act = PA_None;
    int arg = 0;
    std::wstring text, text2;
    WCHAR glyph = 0;
    ID2D1Bitmap* bmp = nullptr;
    float value = 0, value2 = 0;
    bool on = false, enabled = true, hoverable = false, danger = false;
    float px = 13;
    int weight = 400;
    Align align = kLeft;
    D2D1_COLOR_F color{1, 1, 1, 1};
};

struct Panel {
    LWin win;
    int kind = kNone;
    std::vector<Widget> w;
    float W = 0, H = 0;
    D2D1_RECT_F anchor{};
    int x = 0, y = 0;
    Anim show;
    bool closing = false;
    int hover = -1, drag = -1;
    float dragValue = 0;
    int calYear = 0, calMonth = 0;
    bool mixerOpen = false;
    int armed = -1;
    int tab = 0;   // clock panel: 0 calendar, 1 timer, 2 stopwatch
    bool tracking = false;
};

Panel g_panel;
std::map<std::wstring, std::wstring> g_appNames;

bool PanelOpen() { return g_panel.kind != kNone && !g_panel.closing; }
int PanelKindOpen() { return PanelOpen() ? g_panel.kind : kNone; }

D2D1_COLOR_F SubText(float a = 0.62f) { return WithAlpha(g_s.text, a); }

Widget& AddW(WK kind, D2D1_RECT_F rc, int act = PA_None, int arg = 0) {
    Widget w;
    w.kind = kind;
    w.rc = rc;
    w.act = act;
    w.arg = arg;
    w.color = g_s.text;
    w.hoverable = act != PA_None;
    g_panel.w.push_back(std::move(w));
    return g_panel.w.back();
}

Widget& AddText(const std::wstring& text, D2D1_RECT_F rc, float px, int weight = 400,
                Align align = kLeft, D2D1_COLOR_F color = {}, int act = PA_None) {
    Widget& w = AddW(WK::Text, rc, act);
    w.text = text;
    w.px = px;
    w.weight = weight;
    w.align = align;
    w.color = color.a > 0 ? color : g_s.text;
    w.hoverable = false;
    return w;
}

Widget& AddGlyph(WCHAR glyph, D2D1_RECT_F rc, float px, int act = PA_None, int arg = 0) {
    Widget& w = AddW(WK::Glyph, rc, act, arg);
    w.glyph = glyph;
    w.px = px;
    w.color = g_s.icons;
    return w;
}

Widget& AddSlider(D2D1_RECT_F rc, float value, int act, int arg = 0) {
    Widget& w = AddW(WK::Slider, rc, act, arg);
    w.value = std::clamp(value, 0.f, 1.f);
    return w;
}

void AddLine(float y, float pad) {
    AddW(WK::Line, D2D1::RectF(pad, y, g_panel.W - pad, y + std::max(1.f, std::round(S(1)))));
}

std::wstring Percent(float v) { return std::to_wstring((int)std::lround(v * 100)) + L"%"; }

std::wstring Duration(double seconds) {
    int s = (int)std::max(0.0, seconds);
    WCHAR buf[32];
    if (s >= 3600) {
        swprintf_s(buf, L"%d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);
    } else {
        swprintf_s(buf, L"%d:%02d", s / 60, s % 60);
    }
    return buf;
}

// A row: icon (glyph or bitmap) + slider + percentage.
void AddSliderRow(float& y, float pad, WCHAR glyph, ID2D1Bitmap* bmp, int iconAct, int iconArg,
                  float value, int sliderAct, int sliderArg, bool dim = false) {
    float W = g_panel.W;
    float rowH = S(32);
    D2D1_RECT_F iconRc = D2D1::RectF(pad - S(4), y, pad + S(26), y + rowH);
    if (bmp) {
        Widget& b = AddW(WK::Bitmap, iconRc, iconAct, iconArg);
        b.bmp = bmp;
        b.value = dim ? 0.4f : 1;
    } else {
        Widget& g = AddGlyph(glyph, iconRc, S(15), iconAct, iconArg);
        if (dim) g.color = WithAlpha(g.color, 0.5f);
    }
    float pctW = S(40);
    AddSlider(D2D1::RectF(pad + S(36), y, W - pad - pctW, y + rowH), value, sliderAct, sliderArg);
    AddText(Percent(value), D2D1::RectF(W - pad - pctW, y, W - pad, y + rowH), S(11.5f), 400,
            kRight, SubText());
    y += rowH + S(2);
}

////////////////////////////////////////////////////////////////////////////////

void BuildCalendar(float pad, float y) {
    auto& p = g_panel;
    float W = p.W;
    SYSTEMTIME now;
    GetLocalTime(&now);
    WCHAR buf[128];
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, nullptr, buf, ARRAYSIZE(buf));
    AddText(buf, D2D1::RectF(pad, y, W - pad, y + S(40)), S(30), 600);
    y += S(40);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &now, nullptr, buf,
                    ARRAYSIZE(buf), nullptr);
    AddText(buf, D2D1::RectF(pad, y, W - pad, y + S(18)), S(12.5f), 400, kLeft, SubText(),
            PA_DateSettings);
    y += S(30);

    if (!p.calYear) {
        p.calYear = now.wYear;
        p.calMonth = now.wMonth;
    }
    SYSTEMTIME first{};
    first.wYear = (WORD)p.calYear;
    first.wMonth = (WORD)p.calMonth;
    first.wDay = 1;
    FILETIME ft;
    SystemTimeToFileTime(&first, &ft);
    FileTimeToSystemTime(&ft, &first);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &first, L"MMMM yyyy", buf, ARRAYSIZE(buf),
                    nullptr);
    AddText(buf, D2D1::RectF(pad, y, W - pad - S(96), y + S(28)), S(15), 600);
    float bx = W - pad - S(28) * 3 + S(4);
    AddGlyph(Glyph::ChevronL, D2D1::RectF(bx, y, bx + S(28), y + S(28)), S(11), PA_CalPrev);
    Widget& today = AddGlyph(0xE91F, D2D1::RectF(bx + S(28), y, bx + S(56), y + S(28)), S(8),
                             PA_CalToday);
    today.color = g_is.accent;
    AddGlyph(Glyph::ChevronR, D2D1::RectF(bx + S(56), y, bx + S(84), y + S(28)), S(11),
             PA_CalNext);
    y += S(36);

    WCHAR fdw[4] = L"6";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, fdw, ARRAYSIZE(fdw));
    int firstDay = _wtoi(fdw);   // 0 = Monday
    float cw = (W - 2 * pad) / 7;
    for (int i = 0; i < 7; i++) {
        int day = (firstDay + i) % 7;
        GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTESTDAYNAME1 + day, buf,
                        ARRAYSIZE(buf));
        AddText(buf, D2D1::RectF(pad + i * cw, y, pad + (i + 1) * cw, y + S(20)), S(11), 500,
                kCenter, SubText(0.55f));
    }
    y += S(24);

    int dowMon = (first.wDayOfWeek + 6) % 7;
    int lead = (dowMon - firstDay + 7) % 7;
    auto daysIn = [](int year, int month) {
        static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        return month == 2 && leap ? 29 : d[month - 1];
    };
    int days = daysIn(p.calYear, p.calMonth);
    int prevMonth = p.calMonth == 1 ? 12 : p.calMonth - 1;
    int prevDays = daysIn(p.calMonth == 1 ? p.calYear - 1 : p.calYear, prevMonth);
    float ch = S(32);
    for (int cell = 0; cell < 42; cell++) {
        int r = cell / 7, c = cell % 7;
        int day = cell - lead + 1;
        bool inMonth = day >= 1 && day <= days;
        int shown = inMonth ? day : day < 1 ? prevDays + day : day - days;
        D2D1_RECT_F rc = D2D1::RectF(pad + c * cw, y + r * ch, pad + (c + 1) * cw,
                                     y + (r + 1) * ch);
        Widget& w = AddW(WK::Cell, rc);
        w.text = std::to_wstring(shown);
        w.enabled = inMonth;
        w.on = inMonth && day == now.wDay && p.calMonth == now.wMonth && p.calYear == now.wYear;
        w.hoverable = inMonth;
    }
    y += 6 * ch + S(8);
    AddLine(y, pad);
    y += S(10);

    // ISO week number and day of year.
    auto dayOfYear = [&](int yr, int mo, int d) {
        int n = d;
        for (int m = 1; m < mo; m++) n += daysIn(yr, m);
        return n;
    };
    int doy = dayOfYear(now.wYear, now.wMonth, now.wDay);
    int wday = now.wDayOfWeek == 0 ? 7 : now.wDayOfWeek;
    int week = (doy - wday + 10) / 7;
    if (week < 1) week = 52;
    if (week > 52) {
        int jan1 = ((wday - doy) % 7 + 7 + 1) % 7;
        if (jan1 != 4) week = 1;
    }
    swprintf_s(buf, L"Week %d  ·  Day %d of %d", week, doy,
               365 + (daysIn(now.wYear, 2) == 29));
    AddText(buf, D2D1::RectF(pad, y, W - pad, y + S(18)), S(11.5f), 400, kLeft, SubText(0.55f));
    y += S(18);
    p.H = std::round(y + pad);
}

Widget& AddPill(const std::wstring& text, D2D1_RECT_F rc, int act, int arg = 0, bool primary = false) {
    Widget& w = AddW(WK::Pill, rc, act, arg);
    w.text = text;
    w.on = primary;
    return w;
}

void BuildTimer(float pad, float y) {
    auto& p = g_panel;
    auto& is = g_is;
    float W = p.W;
    bool active = is.timerRunning || is.timerPaused;
    LONGLONG left = TimerRemaining();
    AddText(FormatMs(active ? left + 999 : is.timerTotal, false),
            D2D1::RectF(pad, y, W - pad, y + S(62)), S(46), 600, kCenter);
    y += S(66);
    if (active) {
        AddSlider(D2D1::RectF(pad + S(8), y, W - pad - S(8), y + S(10)),
                  1 - (float)left / std::max<LONGLONG>(1, is.timerTotal), PA_None);
        y += S(22);
    } else {
        static const int presets[] = {1, 3, 5, 10, 15, 25, 45, 60};
        float gap = S(8), cw = (W - 2 * pad - 3 * gap) / 4, ch = S(32);
        for (int i = 0; i < 8; i++) {
            float x = pad + (i % 4) * (cw + gap), cy = y + (i / 4) * (ch + gap);
            Widget& w = AddPill(std::to_wstring(presets[i]) + L" min",
                                D2D1::RectF(x, cy, x + cw, cy + ch), PA_TimerPreset, presets[i]);
            w.danger = is.timerTotal == presets[i] * 60000LL;
        }
        y += 2 * ch + gap + S(10);
        float hw = (W - 2 * pad - gap) / 2;
        AddPill(L"− 1 min", D2D1::RectF(pad, y, pad + hw, y + ch), PA_TimerAdd, -1);
        AddPill(L"+ 1 min", D2D1::RectF(pad + hw + gap, y, W - pad, y + ch), PA_TimerAdd, 1);
        y += ch + S(14);
    }
    float bh = S(40), gap = S(8);
    if (active) {
        float hw = (W - 2 * pad - gap) / 2;
        AddPill(is.timerRunning ? L"Pause" : L"Resume", D2D1::RectF(pad, y, pad + hw, y + bh),
                PA_TimerStart, 0, true);
        AddPill(L"Cancel", D2D1::RectF(pad + hw + gap, y, W - pad, y + bh), PA_TimerCancel);
    } else {
        AddPill(L"Start", D2D1::RectF(pad, y, W - pad, y + bh), PA_TimerStart, 0, true);
    }
    y += bh;
    p.H = std::round(y + pad);
}

void BuildStopwatch(float pad, float y) {
    auto& p = g_panel;
    auto& is = g_is;
    float W = p.W;
    AddText(FormatMs(StopwatchElapsed(), true), D2D1::RectF(pad, y, W - pad, y + S(62)), S(42),
            600, kCenter);
    y += S(70);
    float bh = S(40), gap = S(8), hw = (W - 2 * pad - gap) / 2;
    AddPill(is.swRunning ? L"Stop" : L"Start", D2D1::RectF(pad, y, pad + hw, y + bh), PA_SwToggle,
            0, true);
    AddPill(is.swRunning ? L"Lap" : L"Reset", D2D1::RectF(pad + hw + gap, y, W - pad, y + bh),
            PA_SwLap);
    y += bh + S(10);
    for (size_t i = 0; i < is.laps.size() && i < 6; i++) {
        LONGLONG split = is.laps[i] - (i + 1 < is.laps.size() ? is.laps[i + 1] : 0);
        AddLine(y, pad);
        AddText(L"Lap " + std::to_wstring(is.laps.size() - i),
                D2D1::RectF(pad, y, W / 2, y + S(28)), S(12.5f), 400, kLeft, SubText());
        AddText(FormatMs(split, true), D2D1::RectF(W / 2, y, W - pad, y + S(28)), S(12.5f), 500,
                kRight);
        y += S(28);
    }
    p.H = std::round(y + pad);
}

void BuildTime(float pad) {
    auto& p = g_panel;
    p.W = std::round(S(300));
    float W = p.W, y = pad;
    float th = S(30);
    Widget& bg = AddW(WK::Fill, D2D1::RectF(pad, y, W - pad, y + th));
    bg.color = WithAlpha(g_s.text, 0.07f);
    PCWSTR names[] = {L"Calendar", L"Timer", L"Stopwatch"};
    float tw = (W - 2 * pad) / 3;
    for (int i = 0; i < 3; i++) {
        Widget& t = AddW(WK::Tab, D2D1::RectF(pad + i * tw, y, pad + (i + 1) * tw, y + th), PA_Tab, i);
        t.text = names[i];
        t.on = p.tab == i;
    }
    y += th + S(14);
    if (p.tab == 1) {
        BuildTimer(pad, y);
    } else if (p.tab == 2) {
        BuildStopwatch(pad, y);
    } else {
        BuildCalendar(pad, y);
    }
}

void BuildMedia(float pad) {
    auto& p = g_panel;
    auto& is = g_is;
    auto& m = is.media;
    p.W = std::round(S(340));
    float W = p.W, y = pad;

    if (m.hasSession) {
        float art = std::round(S(76));
        D2D1_RECT_F ar = D2D1::RectF(pad, y, pad + art, y + art);
        if (is.artThumb) {
            Widget& a = AddW(WK::Art, ar, PA_Play);
            a.bmp = is.artThumb.Get();
            a.hoverable = false;
        } else {
            Widget& f = AddW(WK::Fill, ar);
            f.color = WithAlpha(g_s.text, 0.12f);
            AddGlyph(Glyph::Music, ar, S(28));
        }
        float tx = ar.right + S(14);
        AddText(m.title, D2D1::RectF(tx, y + S(8), W - pad, y + S(30)), S(16), 600);
        AddText(m.artist, D2D1::RectF(tx, y + S(30), W - pad, y + S(50)), S(13), 400, kLeft,
                SubText(0.75f));
        auto nameIt = g_appNames.find(m.app);
        if (nameIt == g_appNames.end()) {
            std::wstring name;
            ShellIcon(L"shell:AppsFolder\\" + m.app, &name);
            if (name.empty() || name.find(L'\\') != std::wstring::npos) {
                MixSession* s = is.audio.FindMediaSession(m.app);
                name = s && !s->exe.empty() ? FileDescription(s->exe) : m.app;
            }
            nameIt = g_appNames.emplace(m.app, name).first;
        }
        ID2D1Bitmap* appIcon = ShellIcon(L"shell:AppsFolder\\" + m.app);
        float ay = y + S(54);
        if (appIcon) {
            Widget& b = AddW(WK::Bitmap, D2D1::RectF(tx, ay + S(2), tx + S(14), ay + S(16)));
            b.bmp = appIcon;
            b.value = 1;
            tx += S(19);
        }
        AddText(nameIt->second, D2D1::RectF(tx, ay, W - pad, ay + S(18)), S(11.5f), 400, kLeft,
                SubText(0.55f));
        y += art + S(14);

        if (m.duration > 0) {
            double pos = m.position;
            if (m.playing) pos += (GetTickCount64() - m.positionTick) / 1000.0;
            pos = std::clamp(pos, 0.0, m.duration);
            int seekIndex = (int)p.w.size();
            AddSlider(D2D1::RectF(pad, y, W - pad, y + S(14)), (float)(pos / m.duration),
                      m.canSeek ? PA_Seek : PA_None);
            if (p.drag == seekIndex) pos = p.dragValue * m.duration;
            y += S(16);
            AddText(Duration(pos), D2D1::RectF(pad, y, W / 2, y + S(14)), S(11), 400, kLeft,
                    SubText(0.55f));
            AddText(L"-" + Duration(m.duration - pos), D2D1::RectF(W / 2, y, W - pad, y + S(14)),
                    S(11), 400, kRight, SubText(0.55f));
            y += S(20);
        }

        float b = S(38), big = S(48);
        std::vector<std::pair<WCHAR, int>> buttons;
        if (m.canShuffle) buttons.push_back({Glyph::Shuffle, PA_Shuffle});
        buttons.push_back({Glyph::Previous, PA_Prev});
        buttons.push_back({m.playing ? Glyph::Pause : Glyph::Play, PA_Play});
        buttons.push_back({Glyph::Next, PA_Next});
        if (m.canRepeat) buttons.push_back({m.repeat == 1 ? Glyph::RepeatOne : Glyph::Repeat, PA_Repeat});
        float total = 0;
        for (auto& bt : buttons) total += bt.second == PA_Play ? big : b;
        total += S(10) * (buttons.size() - 1);
        float x = std::round((W - total) / 2);
        for (auto& bt : buttons) {
            float size = bt.second == PA_Play ? big : b;
            D2D1_RECT_F rc = D2D1::RectF(x, y + (big - size) / 2, x + size, y + (big + size) / 2);
            if (bt.second == PA_Play) {
                Widget& f = AddW(WK::Fill, rc);
                f.color = WithAlpha(g_s.text, 0.12f);
            }
            Widget& g = AddGlyph(bt.first, rc, bt.second == PA_Play ? S(20) : S(15), bt.second);
            if ((bt.second == PA_Prev && !m.canPrev) || (bt.second == PA_Next && !m.canNext)) {
                g.color = WithAlpha(g.color, 0.4f);
            }
            if ((bt.second == PA_Shuffle && m.shuffle) || (bt.second == PA_Repeat && m.repeat)) {
                g.color = g_is.accent;
            }
            x += size + S(10);
        }
        y += big + S(14);

        if (g_s.lyrics && !is.lyrics.empty()) {
            int i = is.lyricIndex;
            auto line = [&](int k) {
                return k >= 0 && k < (int)is.lyrics.size() ? is.lyrics[k].text : L"";
            };
            AddText(line(i - 1), D2D1::RectF(pad, y, W - pad, y + S(18)), S(12), 400, kCenter,
                    SubText(0.4f));
            AddText(i >= 0 ? line(i) : L"\u266A", D2D1::RectF(pad, y + S(18), W - pad, y + S(42)),
                    S(14.5f), 600, kCenter);
            AddText(line(i + 1), D2D1::RectF(pad, y + S(42), W - pad, y + S(60)), S(12), 400,
                    kCenter, SubText(0.4f));
            y += S(70);
        }

        if (MixSession* s = is.audio.FindMediaSession(m.app)) {
            int index = (int)(s - is.audio.mix.data());
            AddSliderRow(y, pad, 0, ShellIcon(s->exe), PA_AppMute, index,
                         s->muted ? 0 : s->volume, PA_AppVol, index, s->muted);
        }
    } else {
        AddText(L"Nothing playing", D2D1::RectF(pad, y, W - pad, y + S(24)), S(14), 600, kLeft,
                SubText(0.75f));
        y += S(34);
    }

    AddSliderRow(y, pad, VolumeGlyph(), nullptr, PA_Mute, 0,
                 is.audio.muted ? 0 : is.audio.level, PA_Vol, 0, is.audio.muted);
    Widget& open = AddText(L"Open Windows mixer", D2D1::RectF(pad, y, W - pad, y + S(24)),
                           S(12), 400, kLeft, SubText(0.7f), PA_OpenMixer);
    open.hoverable = true;
    y += S(30);
    AddLine(y, pad);
    y += S(6);
    AddGlyph(p.mixerOpen ? Glyph::ChevronDown : Glyph::ChevronRight,
             D2D1::RectF(pad - S(6), y, pad + S(16), y + S(30)), S(10), PA_MixerToggle);
    Widget& head = AddText(L"Mixer", D2D1::RectF(pad + S(18), y, W - pad, y + S(30)), S(13), 600,
                           kLeft, {}, PA_MixerToggle);
    head.hoverable = true;
    y += S(32);

    if (p.mixerOpen) {
        float maxH = (g_is.work.bottom - g_is.work.top) - S(140);
        for (int i = 0; i < (int)is.audio.mix.size() && y + S(46) < maxH; i++) {
            auto& s = is.audio.mix[i];
            float rowH = S(44);
            D2D1_RECT_F iconRc = D2D1::RectF(pad - S(4), y + S(6), pad + S(28), y + S(38));
            if (s.system) {
                AddGlyph(Glyph::Volume0 + 3, iconRc, S(16), PA_SessMute, i).color =
                    WithAlpha(g_s.icons, s.muted ? 0.4f : 1);
            } else {
                Widget& bm = AddW(WK::Bitmap, iconRc, PA_SessMute, i);
                bm.bmp = ShellIcon(s.exe);
                bm.value = s.muted ? 0.4f : 1;
            }
            float tx = pad + S(38);
            AddText(s.name, D2D1::RectF(tx, y + S(2), W - pad - S(44), y + S(22)), S(12.5f), 500);
            AddSlider(D2D1::RectF(tx, y + S(22), W - pad - S(44), y + S(38)),
                      s.muted ? 0 : s.volume, PA_SessVol, i);
            AddText(Percent(s.muted ? 0 : s.volume),
                    D2D1::RectF(W - pad - S(40), y + S(22), W - pad, y + S(38)), S(11.5f), 400,
                    kRight, SubText());
            y += rowH;
        }
    }
    p.H = std::round(y + pad - S(6));
}

void BuildControl(float pad) {
    auto& p = g_panel;
    auto& is = g_is;
    p.W = std::round(S(320));
    float W = p.W, y = pad;
    float gap = S(10);
    float tw = std::round((W - 2 * pad - gap) / 2), th = std::round(S(62));

    std::wstring ssid;
    AcquireSRWLockShared(&g_ssidLock);
    ssid = g_ssid;
    ReleaseSRWLockShared(&g_ssidLock);

    struct TileDef {
        WCHAR glyph;
        std::wstring label, sub;
        bool on, enabled;
        int act;
    };
    int wifi = g_wifiState, bt = g_btState;
    TileDef tiles[] = {
        {Glyph::Wifi, L"Wi-Fi",
         wifi < 0 ? L"Unavailable" : wifi ? (ssid.empty() ? L"On" : ssid) : L"Off", wifi == 1,
         wifi >= 0, PA_Wifi},
        {Glyph::Bluetooth, L"Bluetooth", bt < 0 ? L"Unavailable" : bt ? L"On" : L"Off", bt == 1,
         bt >= 0, PA_Bt},
        {is.audio.micMuted ? Glyph::MicOff : Glyph::Mic, L"Microphone",
         !is.audio.hasMic ? L"None" : is.audio.micMuted ? L"Muted" : L"On",
         is.audio.hasMic && !is.audio.micMuted, is.audio.hasMic, PA_Mic},
        {Glyph::Moon, L"Dark mode", IsDarkMode() ? L"On" : L"Off", IsDarkMode(), true, PA_Dark},
    };
    for (int i = 0; i < 4; i++) {
        float x = pad + (i % 2) * (tw + gap);
        float ty = y + (i / 2) * (th + gap);
        Widget& t = AddW(WK::Tile, D2D1::RectF(x, ty, x + tw, ty + th), tiles[i].act);
        t.glyph = tiles[i].glyph;
        t.text = tiles[i].label;
        t.text2 = tiles[i].sub;
        t.on = tiles[i].on;
        t.enabled = tiles[i].enabled;
        if (!t.enabled) t.act = PA_None;
    }
    y += 2 * th + gap + S(14);

    AddSliderRow(y, pad, VolumeGlyph(), nullptr, PA_Mute, 0, is.audio.muted ? 0 : is.audio.level,
                 PA_Vol, 0, is.audio.muted);
    if (g_brightness >= 0) {
        AddSliderRow(y, pad, Glyph::Sun, nullptr, PA_None, 0, g_brightness / 100.f, PA_Bright, 0);
    }
    y += S(8);
    AddLine(y, pad);
    y += S(10);

    struct ButtonDef {
        WCHAR glyph;
        PCWSTR label;
        int act;
    } buttons[] = {{Glyph::Settings, L"Settings", PA_Settings},
                   {Glyph::QuickSettings, L"Quick settings", PA_Quick},
                   {Glyph::Power, L"Power", PA_PowerPanel}};
    float bw = std::round((W - 2 * pad - 2 * S(8)) / 3);
    for (int i = 0; i < 3; i++) {
        float x = pad + i * (bw + S(8));
        Widget& b = AddW(WK::Button, D2D1::RectF(x, y, x + bw, y + S(52)), buttons[i].act);
        b.glyph = buttons[i].glyph;
        b.text = buttons[i].label;
        b.align = kCenter;
    }
    y += S(52);
    p.H = std::round(y + pad);
}

void BuildPower(float pad) {
    auto& p = g_panel;
    p.W = std::round(S(250));
    float W = p.W, y = pad - S(4);
    struct Row {
        WCHAR glyph;
        PCWSTR label, confirm;
        PowerAction action;
    } rows[] = {
        {Glyph::Lock, L"Lock", nullptr, PowerAction::Lock},
        {Glyph::Sleep, L"Sleep", nullptr, PowerAction::Sleep},
        {Glyph::SignOut, L"Sign out", L"Click again to sign out", PowerAction::SignOut},
        {Glyph::Restart, L"Restart", L"Click again to restart", PowerAction::Restart},
        {Glyph::Power, L"Shut down", L"Click again to shut down", PowerAction::ShutDown},
    };
    for (int i = 0; i < 5; i++) {
        Widget& b = AddW(WK::Button, D2D1::RectF(pad - S(6), y, W - pad + S(6), y + S(40)),
                         PA_Power, i);
        b.glyph = rows[i].glyph;
        bool armed = p.armed == i;
        b.text = armed ? rows[i].confirm : rows[i].label;
        b.danger = armed;
        b.align = kLeft;
        y += S(42);
    }
    p.H = std::round(y + pad - S(6));
}

void BuildWeather(float pad) {
    auto& p = g_panel;
    auto& w = g_is.weather;
    p.W = std::round(S(330));
    float W = p.W, y = pad;
    if (!w.ok) {
        AddText(g_s.city.empty() ? L"Set your city in the mod's settings"
                                 : L"Weather isn't available right now",
                D2D1::RectF(pad, y, W - pad, y + S(24)), S(13), 500);
        p.H = std::round(y + S(24) + pad);
        return;
    }
    const wchar_t* deg = L"°";
    auto t = [&](float v) { return std::to_wstring((int)std::lround(v)) + deg; };
    AddText(w.place, D2D1::RectF(pad, y, W - pad, y + S(18)), S(13), 500, kLeft, SubText(0.75f));
    y += S(22);
    Widget& e = AddW(WK::Emoji, D2D1::RectF(pad - S(4), y, pad + S(58), y + S(60)));
    e.text = WeatherEmoji(w.code, w.day);
    e.px = S(42);
    AddText(t(w.temp), D2D1::RectF(pad + S(64), y, pad + S(180), y + S(60)), S(40), 600);
    AddText(WeatherWords(w.code), D2D1::RectF(W / 2 + S(10), y + S(8), W - pad, y + S(30)), S(14),
            600, kRight);
    AddText(L"H " + t(w.hi) + L"   L " + t(w.lo),
            D2D1::RectF(W / 2 + S(10), y + S(30), W - pad, y + S(50)), S(12.5f), 400, kRight,
            SubText(0.75f));
    y += S(66);
    std::wstring details = L"Feels like " + t(w.feels) + L"  ·  Humidity " +
                           std::to_wstring(w.humidity) + L"%  ·  Wind " +
                           std::to_wstring((int)std::lround(w.wind)) +
                           (g_s.celsius ? L" km/h" : L" mph");
    AddText(details, D2D1::RectF(pad, y, W - pad, y + S(18)), S(11.5f), 400, kLeft, SubText());
    y += S(28);
    AddLine(y, pad);
    y += S(10);

    if (!w.hours.empty()) {
        float cw = (W - 2 * pad) / w.hours.size();
        for (size_t i = 0; i < w.hours.size(); i++) {
            float x = pad + i * cw;
            AddText(w.hours[i].label, D2D1::RectF(x, y, x + cw, y + S(16)), S(11), 400, kCenter,
                    SubText());
            Widget& he = AddW(WK::Emoji, D2D1::RectF(x, y + S(18), x + cw, y + S(44)));
            he.text = WeatherEmoji(w.hours[i].code, w.hours[i].day);
            he.px = S(19);
            AddText(t(w.hours[i].temp), D2D1::RectF(x, y + S(46), x + cw, y + S(64)), S(13), 500,
                    kCenter);
        }
        y += S(72);
        AddLine(y, pad);
        y += S(6);
    }

    float lo = 1e9f, hi = -1e9f;
    for (auto& d : w.days) {
        lo = std::min(lo, d.lo);
        hi = std::max(hi, d.hi);
    }
    for (auto& d : w.days) {
        float rowH = S(30);
        AddText(d.label, D2D1::RectF(pad, y, pad + S(56), y + rowH), S(13), 500);
        Widget& de = AddW(WK::Emoji, D2D1::RectF(pad + S(56), y, pad + S(84), y + rowH));
        de.text = WeatherEmoji(d.code, true);
        de.px = S(16);
        AddText(t(d.lo), D2D1::RectF(pad + S(88), y, pad + S(128), y + rowH), S(12.5f), 400,
                kRight, SubText());
        Widget& r = AddW(WK::Range, D2D1::RectF(pad + S(138), y, W - pad - S(48), y + rowH));
        r.value = hi > lo ? (d.lo - lo) / (hi - lo) : 0;
        r.value2 = hi > lo ? (d.hi - lo) / (hi - lo) : 1;
        AddText(t(d.hi), D2D1::RectF(W - pad - S(44), y, W - pad, y + rowH), S(12.5f), 500,
                kRight);
        y += rowH;
    }
    y += S(6);
    Widget& credit = AddText(L"Weather data by Open-Meteo.com",
                             D2D1::RectF(pad, y, W - pad, y + S(16)), S(10.5f), 400, kLeft,
                             SubText(0.45f), PA_OpenMeteo);
    credit.hoverable = true;
    y += S(16);
    p.H = std::round(y + pad);
}

void BuildTray(float pad) {
    auto& p = g_panel;
    int cols = 6;
    float tile = std::round(S(44)), gap = S(4);
    p.W = std::round(2 * pad + cols * tile + (cols - 1) * gap);
    float y = pad;
    int n = 0;
    std::wstring label;
    for (int i = 0; i < (int)g_tray.size(); i++) {
        const auto& t = g_tray[i];
        if (t.hidden || !t.bmp) continue;
        float x = pad + (n % cols) * (tile + gap);
        float ty = y + (n / cols) * (tile + gap);
        Widget& w = AddW(WK::Button, D2D1::RectF(x, ty, x + tile, ty + tile), PA_TrayIcon, i);
        w.bmp = t.bmp.Get();
        int index = (int)p.w.size() - 1;
        bool pinned = IsPinned(t);
        if (p.hover == index || (p.hover >= 0 && p.hover < (int)p.w.size() &&
                                 p.w[p.hover].act == PA_TrayPin && p.w[p.hover].arg == i)) {
            label = t.tip;
        }
        if (pinned || p.hover == index) {
            Widget& pin = AddGlyph(pinned ? Glyph::Pinned : Glyph::Pin,
                                   D2D1::RectF(x + tile - S(17), ty + S(1), x + tile - S(1), ty + S(17)),
                                   S(9), PA_TrayPin, i);
            pin.color = pinned ? g_is.accent : WithAlpha(g_s.icons, 0.85f);
        }
        n++;
    }
    int rows = std::max(1, (n + cols - 1) / cols);
    y += rows * tile + (rows - 1) * gap + S(10);
    if (!n) {
        AddText(L"No tray icons yet", D2D1::RectF(pad, pad, p.W - pad, pad + S(24)), S(13), 500,
                kCenter, SubText());
    }
    AddText(label, D2D1::RectF(pad, y, p.W - pad, y + S(18)), S(12), 500, kCenter, SubText(0.8f));
    y += S(18);
    p.H = std::round(y + pad);
}

////////////////////////////////////////////////////////////////////////////////

void DrawWidget(const Widget& w, bool hovered, bool dragging) {
    const float a = 1;
    switch (w.kind) {
        case WK::Text:
            if (hovered && w.act) {
                DrawStr(w.text, w.rc, g_s.text, w.px, w.weight, w.align);
            } else {
                DrawStr(w.text, w.rc, w.color, w.px, w.weight, w.align);
            }
            break;
        case WK::Glyph:
            if (hovered && w.act) {
                float s = std::min(w.rc.right - w.rc.left, w.rc.bottom - w.rc.top);
                Fill(Centered(w.rc, s, s), s / 2, WithAlpha(g_s.icons, 0.12f));
            }
            DrawGlyph(w.glyph, w.rc, w.color, w.px);
            break;
        case WK::Emoji:
            DrawEmoji(w.text, w.rc, w.px, w.align == kLeft ? kCenter : w.align);
            break;
        case WK::Bitmap: {
            if (hovered && w.act) {
                float s = std::min(w.rc.right - w.rc.left, w.rc.bottom - w.rc.top);
                Fill(Centered(w.rc, s, s), s / 2, WithAlpha(g_s.icons, 0.12f));
            }
            float s = std::min(std::min(w.rc.right - w.rc.left, w.rc.bottom - w.rc.top), S(20));
            DrawBitmap(w.bmp, Centered(w.rc, s, s), w.value > 0 ? w.value : 1);
            break;
        }
        case WK::Art:
            FillWithBitmap(w.bmp, D2D1::RoundedRect(w.rc, S(10), S(10)), false, 1);
            break;
        case WK::Fill: {
            float s = std::min(w.rc.right - w.rc.left, w.rc.bottom - w.rc.top);
            bool round = std::abs((w.rc.right - w.rc.left) - (w.rc.bottom - w.rc.top)) < 1 ||
                         s < S(36);
            Fill(w.rc, round ? s / 2 : S(10), w.color);
            break;
        }
        case WK::Slider: {
            bool active = (hovered || dragging) && w.act;
            float th = std::round(active ? S(6) : S(4));
            float cy = (w.rc.top + w.rc.bottom) / 2;
            D2D1_RECT_F track = D2D1::RectF(w.rc.left, cy - th / 2, w.rc.right, cy + th / 2);
            Fill(track, th / 2, WithAlpha(g_s.text, 0.2f));
            float x = w.rc.left + (w.rc.right - w.rc.left) * w.value;
            if (w.value > 0) {
                Fill(D2D1::RectF(w.rc.left, track.top, std::max(x, w.rc.left + th), track.bottom),
                     th / 2, w.act ? g_is.accent : WithAlpha(g_s.text, 0.7f));
            }
            if (active) {
                float k = S(14);
                Fill(D2D1::RectF(x - k / 2, cy - k / 2, x + k / 2, cy + k / 2), k / 2,
                     D2D1::ColorF(D2D1::ColorF::White));
            }
            break;
        }
        case WK::Tile: {
            D2D1_COLOR_F bg = w.on ? g_is.accent : WithAlpha(g_s.text, hovered ? 0.16f : 0.1f);
            if (w.on && hovered) bg = Mix(bg, D2D1::ColorF(D2D1::ColorF::White), 0.12f);
            if (!w.enabled) bg = WithAlpha(g_s.text, 0.05f);
            Fill(w.rc, S(14), bg);
            bool dark = w.on && Luminance(g_is.accent) > 0.6f;
            D2D1_COLOR_F fg = dark ? D2D1::ColorF(D2D1::ColorF::Black) : g_s.text;
            if (!w.enabled) fg = WithAlpha(g_s.text, 0.4f);
            float h = w.rc.bottom - w.rc.top;
            D2D1_RECT_F ic = D2D1::RectF(w.rc.left + S(8), w.rc.top + (h - S(36)) / 2,
                                         w.rc.left + S(44), w.rc.top + (h + S(36)) / 2);
            DrawGlyph(w.glyph, ic, fg, S(17));
            float tx = ic.right + S(4);
            DrawStr(w.text, D2D1::RectF(tx, w.rc.top + h / 2 - S(18), w.rc.right - S(8), w.rc.top + h / 2),
                    fg, S(12.5f), 600);
            DrawStr(w.text2, D2D1::RectF(tx, w.rc.top + h / 2, w.rc.right - S(8), w.rc.top + h / 2 + S(17)),
                    WithAlpha(fg, 0.7f), S(11));
            break;
        }
        case WK::Button: {
            D2D1_COLOR_F bg = w.danger ? D2D1::ColorF(0xC42B1C)
                                       : WithAlpha(g_s.text, hovered ? 0.14f : (w.bmp ? 0 : 0.06f));
            Fill(w.rc, S(10), bg);
            if (w.bmp) {
                float s = std::round(S(24));
                DrawBitmap(w.bmp, Centered(w.rc, s, s));
                break;
            }
            float h = w.rc.bottom - w.rc.top;
            if (w.align == kCenter) {
                DrawGlyph(w.glyph, D2D1::RectF(w.rc.left, w.rc.top + S(6), w.rc.right, w.rc.top + S(28)),
                          g_s.icons, S(16));
                DrawStr(w.text, D2D1::RectF(w.rc.left + S(4), w.rc.top + S(28), w.rc.right - S(4),
                                            w.rc.bottom - S(4)),
                        SubText(0.85f), S(11), 400, kCenter);
            } else {
                DrawGlyph(w.glyph, D2D1::RectF(w.rc.left + S(6), w.rc.top, w.rc.left + S(38), w.rc.bottom),
                          g_s.icons, S(15));
                DrawStr(w.text, D2D1::RectF(w.rc.left + S(44), w.rc.top, w.rc.right - S(8), w.rc.top + h),
                        g_s.text, S(13), 500);
            }
            break;
        }
        case WK::Cell: {
            float s = std::min(w.rc.right - w.rc.left, w.rc.bottom - w.rc.top) - S(4);
            D2D1_RECT_F c = Centered(w.rc, s, s);
            if (w.on) {
                Fill(c, s / 2, g_is.accent);
            } else if (hovered && w.enabled) {
                Fill(c, s / 2, WithAlpha(g_s.text, 0.12f));
            }
            D2D1_COLOR_F fg = w.on ? (Luminance(g_is.accent) > 0.6f ? D2D1::ColorF(0, 0, 0)
                                                                     : D2D1::ColorF(1, 1, 1))
                                   : WithAlpha(g_s.text, w.enabled ? 0.92f : 0.3f);
            DrawStr(w.text, w.rc, fg, S(12.5f), w.on ? 700 : 400, kCenter);
            break;
        }
        case WK::Line:
            Fill(w.rc, 0, WithAlpha(g_s.text, 0.1f));
            break;
        case WK::Tab: {
            float h = w.rc.bottom - w.rc.top;
            D2D1_RECT_F in = D2D1::RectF(w.rc.left + S(3), w.rc.top + S(3), w.rc.right - S(3),
                                         w.rc.bottom - S(3));
            if (w.on) {
                Fill(in, (h - S(6)) / 2, WithAlpha(g_s.text, 0.16f));
            } else if (hovered) {
                Fill(in, (h - S(6)) / 2, WithAlpha(g_s.text, 0.07f));
            }
            DrawStr(w.text, w.rc, WithAlpha(g_s.text, w.on ? 1 : 0.7f), S(12.5f), w.on ? 600 : 400,
                    kCenter);
            break;
        }
        case WK::Pill: {
            float h = w.rc.bottom - w.rc.top;
            D2D1_COLOR_F bg = w.on ? g_is.accent : WithAlpha(g_s.text, w.danger ? 0.22f : 0.09f);
            if (hovered) bg = w.on ? Mix(bg, D2D1::ColorF(D2D1::ColorF::White), 0.12f)
                                   : WithAlpha(g_s.text, w.danger ? 0.26f : 0.15f);
            Fill(w.rc, h / 2, bg);
            D2D1_COLOR_F fg = w.on && Luminance(g_is.accent) > 0.6f ? D2D1::ColorF(0, 0, 0)
                                                                     : g_s.text;
            DrawStr(w.text, w.rc, fg, S(13), w.on ? 600 : 500, kCenter);
            break;
        }
        case WK::Range: {
            float th = S(4), cy = (w.rc.top + w.rc.bottom) / 2;
            D2D1_RECT_F track = D2D1::RectF(w.rc.left, cy - th / 2, w.rc.right, cy + th / 2);
            Fill(track, th / 2, WithAlpha(g_s.text, 0.12f));
            float width = w.rc.right - w.rc.left;
            D2D1_RECT_F fill = D2D1::RectF(w.rc.left + width * w.value, track.top,
                                           w.rc.left + width * std::max(w.value2, w.value + 0.04f),
                                           track.bottom);
            ComPtr<ID2D1GradientStopCollection> stops;
            D2D1_GRADIENT_STOP s[2] = {{0, D2D1::ColorF(0x5AC8FA)}, {1, D2D1::ColorF(0xFFB340)}};
            ComPtr<ID2D1LinearGradientBrush> brush;
            if (SUCCEEDED(g_gfx.rt->CreateGradientStopCollection(s, 2, &stops)) &&
                SUCCEEDED(g_gfx.rt->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(w.rc.left, cy),
                                                        D2D1::Point2F(w.rc.right, cy)),
                    stops.Get(), &brush))) {
                g_gfx.rt->FillRoundedRectangle(D2D1::RoundedRect(fill, th / 2, th / 2), brush.Get());
            }
            break;
        }
    }
    (void)a;
}

void BuildPanel() {
    auto& p = g_panel;
    p.w.clear();
    float pad = std::round(S(16));
    switch (p.kind) {
        case kCalendar: BuildTime(pad); break;
        case kMediaPanel: BuildMedia(pad); break;
        case kControl: BuildControl(pad); break;
        case kPower: BuildPower(pad); break;
        case kWeatherPanel: BuildWeather(pad); break;
        case kTrayPanel: BuildTray(pad); break;
    }
}

void PresentPanel() {
    auto& p = g_panel;
    float v = p.show.value;
    int y = p.y + (int)std::round((1 - v) * -S(10));
    p.win.Present(p.x, y, (BYTE)std::lround(255 * std::clamp(v, 0.f, 1.f)));
}

void RenderPanel() {
    auto& p = g_panel;
    if (!p.kind || !p.win.hwnd) return;
    BuildPanel();
    if (p.drag >= 0 && p.drag < (int)p.w.size() && p.w[p.drag].kind == WK::Slider) {
        p.w[p.drag].value = p.dragValue;
    }
    int w = (int)p.W, h = (int)p.H;
    p.win.SetGlass(CurrentBackground() == BgMode::Glass, 16);
    if (!p.win.Ensure(w, h)) return;
    p.win.Begin();
    float r = S(16);
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(0, 0, p.W, p.H), r, r);
    PaintBackground(rr, true);
    if (g_s.borderWidth > 0 && !p.win.glass) {
        float bw = S(g_s.borderWidth);
        Stroke(D2D1::RectF(bw / 2, bw / 2, p.W - bw / 2, p.H - bw / 2), r - bw / 2, g_s.border, bw);
    }
    for (int i = 0; i < (int)p.w.size(); i++) {
        DrawWidget(p.w[i], i == p.hover && p.w[i].hoverable, i == p.drag);
    }
    if (!p.win.End()) return;

    float cx = (p.anchor.left + p.anchor.right) / 2;
    p.x = (int)std::round(cx - p.W / 2);
    p.x = std::clamp(p.x, (int)g_is.work.left + 8, std::max((int)g_is.work.left + 8,
                                                             (int)g_is.work.right - w - 8));
    p.y = (int)std::round(g_is.win.pos.y + g_is.shapeY + g_is.H + S(8));
    PresentPanel();
}

void RefreshPanel() {
    if (PanelOpen()) RenderPanel();
}

void UpdatePanelTimer() {
    auto& p = g_panel;
    UINT ms = 0;
    if (p.kind == kCalendar) {
        ms = p.tab == 2 ? (g_is.swRunning ? 50 : 0) : p.tab == 1 ? (g_is.timerRunning ? 250 : 0) : 1000;
    } else if (p.kind == kMediaPanel) {
        ms = 1000;
    }
    if (ms) {
        SetTimer(p.win.hwnd, kTimerPanel, ms, nullptr);
    } else {
        KillTimer(p.win.hwnd, kTimerPanel);
    }
}

void FinishClose() {
    auto& p = g_panel;
    p.win.Show(false);
    p.kind = kNone;
    p.closing = false;
    p.drag = -1;
    p.hover = -1;
    KillTimer(p.win.hwnd, kTimerPanel);
    KillTimer(p.win.hwnd, kTimerArm);
    if (g_is.peeking) SetTimer(g_is.win.hwnd, kTimerPeek, 600, nullptr);
    if (!g_is.activities.empty()) ShowActivity();
}

void ClosePanel() {
    auto& p = g_panel;
    if (!p.kind || p.closing) return;
    p.closing = true;
    if (p.drag >= 0) ReleaseCapture();
    p.show.Go(0, 120);
    if (!p.show.Active() || p.win.glass) FinishClose();
}

bool TickPanel(ULONGLONG now) {
    auto& p = g_panel;
    if (!p.show.Active()) return false;
    p.show.Tick(now);
    if (p.closing && p.show.value <= 0) {
        FinishClose();
        return false;
    }
    PresentPanel();
    return p.show.Active();
}

void TogglePanel(int kind, D2D1_RECT_F anchor) {
    auto& p = g_panel;
    if (PanelOpen() && p.kind == kind) {
        ClosePanel();
        return;
    }
    bool wasOpen = PanelOpen();
    if (g_is.activityShown) {
        g_is.activities.clear();
        ShowActivity();
    }
    p.kind = kind;
    p.closing = false;
    p.anchor = anchor;
    p.hover = p.drag = p.armed = -1;
    p.calYear = p.calMonth = 0;
    switch (kind) {
        case kMediaPanel:
            g_is.audio.RefreshMix();
            break;
        case kControl:
            g_is.audio.Read();
            RefreshRadios();
            ReadBrightness();
            break;
    }
    RenderPanel();
    if (!wasOpen) {
        p.show.Snap(0);
        p.show.Go(1, 180);
        if (!p.show.Active()) PresentPanel();
    }
    p.win.Show(true);
    SetForegroundWindow(p.win.hwnd);
    UpdatePanelTimer();
}

void OpenTimePanel(int tab, D2D1_RECT_F anchor) {
    auto& p = g_panel;
    if (PanelOpen() && p.kind == kCalendar) {
        if (p.tab == tab) {
            ClosePanel();
        } else {
            p.tab = tab;
            RenderPanel();
            UpdatePanelTimer();
        }
        return;
    }
    p.tab = tab;
    TogglePanel(kCalendar, anchor);
}

int PanelHit(int x, int y) {
    auto& p = g_panel;
    for (int i = (int)p.w.size() - 1; i >= 0; i--) {
        const Widget& w = p.w[i];
        if (!w.act) continue;
        D2D1_RECT_F rc = w.rc;
        if (w.kind == WK::Slider) {
            rc.top -= S(6);
            rc.bottom += S(6);
        }
        if (Contains(rc, (float)x, (float)y)) return i;
    }
    return -1;
}

void ApplySlider(int index, int x, bool final) {
    auto& p = g_panel;
    if (index < 0 || index >= (int)p.w.size()) return;
    Widget& w = p.w[index];
    float v = std::clamp((x - w.rc.left) / std::max(1.f, w.rc.right - w.rc.left), 0.f, 1.f);
    w.value = v;
    p.dragValue = v;
    auto& audio = g_is.audio;
    switch (w.act) {
        case PA_Seek:
            if (final) {
                g_seekTarget = v * g_is.media.duration;
                g_mediaCommand = 4;
                SetEvent(g_workEvent);
            }
            break;
        case PA_Vol:
            audio.SetLevel(v);
            break;
        case PA_AppVol:
        case PA_SessVol:
            if (w.arg < (int)audio.mix.size()) audio.SetSessionVolume(audio.mix[w.arg], v);
            break;
        case PA_Bright:
            SetBrightness((int)std::lround(v * 100));
            break;
    }
    RenderPanel();
    Update();
}

void PanelClick(int index, UINT button) {
    auto& p = g_panel;
    if (index < 0 || index >= (int)p.w.size()) return;
    Widget w = p.w[index];
    auto& audio = g_is.audio;
    if (w.act == PA_TrayIcon) {
        if (w.arg < (int)g_tray.size()) {
            if (button == WM_MBUTTONUP) {
                TogglePin(g_tray[w.arg]);
                Update();
                RenderPanel();
            } else {
                TrayClick(g_tray[w.arg], button);
            }
        }
        return;
    }
    if (button != WM_LBUTTONUP) return;
    switch (w.act) {
        case PA_CalPrev:
            if (--p.calMonth < 1) {
                p.calMonth = 12;
                p.calYear--;
            }
            break;
        case PA_CalNext:
            if (++p.calMonth > 12) {
                p.calMonth = 1;
                p.calYear++;
            }
            break;
        case PA_CalToday:
            p.calYear = p.calMonth = 0;
            break;
        case PA_DateSettings:
            Launch(L"ms-settings:dateandtime");
            ClosePanel();
            return;
        case PA_Prev: g_mediaCommand = 1; SetEvent(g_workEvent); break;
        case PA_Play: g_mediaCommand = 2; SetEvent(g_workEvent); break;
        case PA_Next: g_mediaCommand = 3; SetEvent(g_workEvent); break;
        case PA_Shuffle: g_mediaCommand = 5; SetEvent(g_workEvent); break;
        case PA_Repeat: g_mediaCommand = 6; SetEvent(g_workEvent); break;
        case PA_Mute: audio.ToggleMute(); break;
        case PA_AppMute:
        case PA_SessMute:
            if (w.arg < (int)audio.mix.size()) audio.ToggleSessionMute(audio.mix[w.arg]);
            break;
        case PA_OpenMixer:
            Launch(L"ms-settings:apps-volume");
            ClosePanel();
            return;
        case PA_MixerToggle:
            p.mixerOpen = !p.mixerOpen;
            if (p.mixerOpen) audio.RefreshMix();
            break;
        case PA_Wifi: RefreshRadios(1, g_wifiState != 1); g_wifiState = g_wifiState == 1 ? 0 : 1; break;
        case PA_Bt: RefreshRadios(2, g_btState != 1); g_btState = g_btState == 1 ? 0 : 1; break;
        case PA_Mic: audio.ToggleMic(); break;
        case PA_Dark: SetDarkMode(!IsDarkMode()); break;
        case PA_Settings:
            Launch(L"ms-settings:");
            ClosePanel();
            return;
        case PA_Quick:
            ClosePanel();
            SendWinHotkey('A');
            return;
        case PA_PowerPanel:
            TogglePanel(kPower, p.anchor);
            return;
        case PA_Power: {
            bool needsConfirm = w.arg >= 2;
            if (needsConfirm && p.armed != w.arg) {
                p.armed = w.arg;
                SetTimer(p.win.hwnd, kTimerArm, 4000, nullptr);
                break;
            }
            ClosePanel();
            DoPower((PowerAction)w.arg);
            return;
        }
        case PA_TrayPin:
            if (w.arg < (int)g_tray.size()) {
                TogglePin(g_tray[w.arg]);
                Update();
            }
            break;
        case PA_OpenMeteo:
            Launch(L"https://open-meteo.com");
            ClosePanel();
            return;
        case PA_Tab:
            p.tab = w.arg;
            break;
        case PA_TimerPreset:
            g_is.timerTotal = w.arg * 60000LL;
            break;
        case PA_TimerAdd:
            g_is.timerTotal = std::clamp<LONGLONG>(g_is.timerTotal + w.arg * 60000LL, 60000,
                                                   24 * 3600000LL);
            break;
        case PA_TimerStart:
            TimerStartPause();
            break;
        case PA_TimerCancel:
            TimerCancel();
            break;
        case PA_SwToggle:
            StopwatchToggle();
            break;
        case PA_SwLap:
            StopwatchLapReset();
            break;
    }
    UpdatePanelTimer();
    RenderPanel();
    Update();
}

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& p = g_panel;
    switch (msg) {
        case WM_MOUSEMOVE: {
            if (!p.tracking) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                p.tracking = TrackMouseEvent(&tme);
            }
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (p.drag >= 0) {
                ApplySlider(p.drag, x, false);
                return 0;
            }
            int hit = PanelHit(x, y);
            if (hit != p.hover) {
                p.hover = hit;
                RenderPanel();
            }
            SetCursor(LoadCursor(nullptr, hit >= 0 ? IDC_HAND : IDC_ARROW));
            return 0;
        }
        case WM_MOUSELEAVE:
            p.tracking = false;
            if (p.hover >= 0 && p.drag < 0) {
                p.hover = -1;
                RenderPanel();
            }
            return 0;
        case WM_SETCURSOR:
            SetCursor(LoadCursor(nullptr, p.hover >= 0 ? IDC_HAND : IDC_ARROW));
            return TRUE;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK: {
            int hit = PanelHit(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (hit >= 0 && p.w[hit].kind == WK::Slider) {
                p.drag = hit;
                SetCapture(hwnd);
                ApplySlider(hit, GET_X_LPARAM(lp), false);
            } else if (msg == WM_LBUTTONDBLCLK && hit >= 0 && p.w[hit].act == PA_TrayIcon) {
                PanelClick(hit, WM_LBUTTONDBLCLK);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (p.drag >= 0) {
                int drag = p.drag;
                ApplySlider(drag, GET_X_LPARAM(lp), true);
                p.drag = -1;
                ReleaseCapture();
                RenderPanel();
                return 0;
            }
            PanelClick(PanelHit(GET_X_LPARAM(lp), GET_Y_LPARAM(lp)), WM_LBUTTONUP);
            return 0;
        }
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            PanelClick(PanelHit(GET_X_LPARAM(lp), GET_Y_LPARAM(lp)), msg);
            return 0;
        case WM_MOUSEWHEEL: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            int hit = PanelHit(pt.x, pt.y);
            if (hit >= 0 && p.w[hit].kind == WK::Slider && p.w[hit].act != PA_Seek) {
                Widget& w = p.w[hit];
                float v = std::clamp(w.value + (GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 0.02f : -0.02f), 0.f, 1.f);
                ApplySlider(hit, (int)std::round(w.rc.left + v * (w.rc.right - w.rc.left)), true);
            }
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) ClosePanel();
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) ClosePanel();
            return 0;
        case WM_TIMER:
            if (wp == kTimerPanel) {
                if (p.drag < 0) RenderPanel();
            } else if (wp == kTimerArm) {
                KillTimer(hwnd, kTimerArm);
                p.armed = -1;
                RenderPanel();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_PAINT:
            p.win.Paint();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK TipProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (msg == WM_CLOSE) return 0;
    return DefWindowProc(hwnd, msg, wp, lp);
}

////////////////////////////////////////////////////////////////////////////////
// Island window

struct MonitorPick {
    int want, index;
    HMONITOR found;
};

BOOL CALLBACK PickMonitorProc(HMONITOR mon, HDC, LPRECT, LPARAM lp) {
    auto pick = (MonitorPick*)lp;
    if (++pick->index == pick->want) {
        pick->found = mon;
        return FALSE;
    }
    return TRUE;
}

void UpdateMonitor() {
    auto& is = g_is;
    HMONITOR mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (g_s.monitor > 0) {
        MonitorPick pick{g_s.monitor, 0, nullptr};
        EnumDisplayMonitors(nullptr, nullptr, PickMonitorProc, (LPARAM)&pick);
        if (pick.found) mon = pick.found;
    }
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfo(mon, &mi)) {
        is.monitor = mi.rcMonitor;
        is.work = mi.rcWork;
    }
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    float scale = dpiX / 96.f;
    if (scale != g_gfx.scale) {
        g_gfx.scale = scale;
        g_gfx.formats.clear();
    }
}

void ArmClockTimer() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    bool seconds = g_s.seconds || g_s.timeFormat.find(L's') != std::wstring::npos ||
                   g_is.timerRunning || g_is.swRunning;
    UINT delay = seconds ? 1000 - st.wMilliseconds
                         : (60 - st.wSecond) * 1000 - st.wMilliseconds;
    SetTimer(g_is.win.hwnd, kTimerClock, delay + 15, nullptr);
}

void ApplyAppbar() {
    auto& is = g_is;
    bool want = g_s.hideFullscreen || g_s.reserveSpace;
    APPBARDATA abd{sizeof(abd), is.win.hwnd, WM_APP_APPBAR};
    if (want && !is.appbar) {
        is.appbar = SHAppBarMessage(ABM_NEW, &abd);
    } else if (!want && is.appbar) {
        SHAppBarMessage(ABM_REMOVE, &abd);
        is.appbar = is.appbarReserved = false;
        is.fullscreen = false;
    }
    if (!is.appbar) return;
    if (g_s.reserveSpace) {
        int h = (int)std::lround(S(g_s.height + std::max(0.f, g_s.topOffset) * 2));
        abd.uEdge = ABE_TOP;
        abd.rc = is.monitor;
        abd.rc.bottom = is.monitor.top + h;
        SHAppBarMessage(ABM_QUERYPOS, &abd);
        abd.rc.bottom = abd.rc.top + h;
        SHAppBarMessage(ABM_SETPOS, &abd);
        is.appbarReserved = true;
    } else if (is.appbarReserved) {
        SHAppBarMessage(ABM_REMOVE, &abd);
        is.appbarReserved = false;
        is.appbar = SHAppBarMessage(ABM_NEW, &abd);
    }
}

void LoadShortcutIcons() {
    for (auto& sc : g_s.shortcuts) {
        sc.icon.Reset();
        if (!sc.glyph) sc.icon = ShellIcon(sc.target);
    }
}

void PickIconFont() {
    g_gfx.iconFont = L"Segoe MDL2 Assets";
    ComPtr<IDWriteFontCollection> fonts;
    UINT32 index;
    BOOL exists = FALSE;
    if (SUCCEEDED(g_gfx.dw->GetSystemFontCollection(&fonts)) &&
        SUCCEEDED(fonts->FindFamilyName(L"Segoe Fluent Icons", &index, &exists)) && exists) {
        g_gfx.iconFont = L"Segoe Fluent Icons";
    }
}

// Accent fills must stay visible on dark backgrounds (gray accents aren't).
D2D1_COLOR_F ResolveAccent() {
    D2D1_COLOR_F c = g_s.systemAccent ? SystemAccent() : g_s.accent;
    float l = Luminance(c);
    if (g_s.systemAccent && Luminance(g_s.bg) < 0.5f && l < 0.45f) {
        c = Mix(c, D2D1::ColorF(D2D1::ColorF::White), (0.45f - l) / (1 - l));
    }
    return c;
}

void ApplySettings() {
    auto& is = g_is;
    LoadSettings();
    is.accent = ResolveAccent();
    g_gfx.formats.clear();
    UpdateMonitor();
    RebuildArtBitmaps();
    LoadShortcutIcons();
    is.status.Read();
    ApplyAppbar();
    ArmClockTimer();
    if (g_s.weather) {
        RequestWeather();
        SetTimer(is.win.hwnd, kTimerWeather, 30 * 60 * 1000, nullptr);
    } else {
        KillTimer(is.win.hwnd, kTimerWeather);
    }
    if (g_s.tray == TrayMode::Off) is.trayOpen = false;
    if (!g_s.lyrics) {
        is.lyrics.clear();
        is.lyricIndex = -1;
    } else if (is.media.hasSession) {
        RequestLyrics(is.media.title, is.media.artist, is.media.album, is.media.duration);
    }
    UpdateLyricsTimer();
    Update();
    RefreshPanel();
}

bool CreateRenderTarget() {
    g_gfx.rt.Reset();
    g_gfx.brush.Reset();
    g_gfx.layer.Reset();
    g_gfx.icons.clear();
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(g_gfx.d2d->CreateDCRenderTarget(&props, &g_gfx.rt)) ||
        FAILED(g_gfx.rt->CreateSolidColorBrush(D2D1::ColorF(0), &g_gfx.brush))) {
        return false;
    }
    g_gfx.rt->CreateLayer(&g_gfx.layer);
    g_gfx.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    return true;
}

LRESULT CALLBACK IslandProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& is = g_is;
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_SETCURSOR:
            SetCursor(LoadCursor(nullptr, is.hover >= 0 ? IDC_HAND : IDC_ARROW));
            return TRUE;

        case WM_MOUSEMOVE: {
            if (!is.tracking) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                is.tracking = TrackMouseEvent(&tme);
            }
            KillTimer(hwnd, kTimerPeek);
            if (is.activityShown) {
                SetCursor(LoadCursor(nullptr, IDC_HAND));
                return 0;
            }
            if (is.folded && !is.peeking) {
                if (g_s.peek) {
                    is.peeking = true;
                    is.fold.Go(0, 240);
                    Update();
                }
                return 0;
            }
            if (is.fold.value > 0.3f) return 0;
            SetHover(HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp)));
            return 0;
        }

        case WM_MOUSELEAVE:
            is.tracking = false;
            is.pressed = -1;
            SetHover(-1);
            if (is.peeking && !PanelOpen()) SetTimer(hwnd, kTimerPeek, 600, nullptr);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK: {
            if (is.folded && !is.peeking) {
                Unfold();
                return 0;
            }
            int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (msg == WM_LBUTTONDBLCLK && hit >= 0 && is.items[hit].act == Act::Tray) {
                TrayClick(g_tray[is.items[hit].index], WM_LBUTTONDBLCLK);
                is.pressed = -1;
                return 0;
            }
            is.pressed = hit;
            return 0;
        }

        case WM_LBUTTONUP: {
            if (is.activityShown) {
                OnActivityClick();
                is.pressed = -1;
                return 0;
            }
            int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (hit >= 0 && hit == is.pressed) {
                Item item = is.items[hit];
                DoAction(item);
            }
            is.pressed = -1;
            return 0;
        }

        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            if (is.activityShown) {
                NextActivity();
                return 0;
            }
            int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (hit >= 0 && is.items[hit].act == Act::Tray &&
                is.items[hit].index < (int)g_tray.size()) {
                TrayIcon& t = g_tray[is.items[hit].index];
                if (msg == WM_RBUTTONUP) {
                    HideTip();
                    TrayClick(t, WM_RBUTTONUP);
                } else {
                    TogglePin(t);
                    Update();
                }
            }
            return 0;
        }

        case WM_MOUSEWHEEL:
            if (g_s.scrollVolume && is.audio.endpoint) {
                is.wheel += GET_WHEEL_DELTA_WPARAM(wp);
                int steps = is.wheel / WHEEL_DELTA;
                is.wheel %= WHEEL_DELTA;
                if (steps) {
                    is.audio.Step(steps * g_s.volumeStep);
                    is.osdUntil = GetTickCount64() + kOsdMs;
                    SetTimer(hwnd, kTimerOsd, kOsdMs + 20, nullptr);
                    HideTip();
                    Update();
                    RefreshPanel();
                }
            }
            return 0;

        case WM_TIMER:
            switch (wp) {
                case kTimerClock:
                    ArmClockTimer();
                    if (!is.folded || is.peeking) Update();
                    // Lyrics that couldn't be fetched are asked for again each minute.
                    if (g_s.lyrics && is.media.hasSession && is.media.playing &&
                        is.lyrics.empty() && GetTickCount64() >= is.nextLyricsTry) {
                        is.nextLyricsTry = GetTickCount64() + 60000;
                        RequestLyrics(is.media.title, is.media.artist, is.media.album,
                                      is.media.duration);
                    }
                    break;
                case kTimerStatus:
                    if (is.status.Read()) {
                        CheckCharging();
                        Update();
                    }
                    break;
                case kTimerOsd:
                    KillTimer(hwnd, kTimerOsd);
                    Update();
                    break;
                case kTimerFrame:
                    OnFrame();
                    break;
                case kTimerTip:
                    KillTimer(hwnd, kTimerTip);
                    if (is.hover >= 0 && is.hover < (int)is.items.size()) {
                        ShowTip(is.items[is.hover].tip, ItemScreenRect(is.items[is.hover]));
                    }
                    break;
                case kTimerPeek: {
                    KillTimer(hwnd, kTimerPeek);
                    POINT pt;
                    GetCursorPos(&pt);
                    RECT rc;
                    GetWindowRect(hwnd, &rc);
                    if (is.peeking && !PanelOpen() && !PtInRect(&rc, pt)) {
                        is.peeking = false;
                        is.hover = -1;
                        is.hlA.Snap(0);
                        HideTip();
                        is.fold.Go(1, 240);
                        Update();
                    }
                    break;
                }
                case kTimerWeather:
                    RequestWeather();
                    break;
                case kTimerActivity:
                    OnActivityTimer();
                    break;
                case kTimerLyrics:
                    OnLyricsTick();
                    break;
                case kTimerAlarm:
                    TimerFinished();
                    break;
            }
            return 0;

        case WM_PAINT:
            is.win.Paint();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_COPYDATA: {
            auto cds = (const COPYDATASTRUCT*)lp;
            if (cds && cds->dwData == kIpcTray && cds->cbData >= sizeof(TrayIpc)) {
                OnTrayIpc(*(const TrayIpc*)cds->lpData);
                Update();
                if (PanelKindOpen() == kTrayPanel) RenderPanel();
                return TRUE;
            }
            return FALSE;
        }

        case WM_APP_MEDIA:
            OnMediaChanged();
            return 0;

        case WM_APP_NOTIF:
            is.notifCount = g_notifCount;
            AcquireSRWLockShared(&g_notifLock);
            is.notifApp = g_notifApp;
            ReleaseSRWLockShared(&g_notifLock);
            Update();
            return 0;

        case WM_APP_VOLUME: {
            float before = is.audio.level;
            bool wasMuted = is.audio.muted;
            is.audio.Read();
            // Volume keys and other apps get the same pop-out as scrolling.
            bool changed = std::abs(is.audio.level - before) > 0.004f || is.audio.muted != wasMuted;
            if (changed && g_s.popVolume && !PanelOpen() && !is.activityShown &&
                (!is.folded || is.peeking)) {
                is.osdUntil = GetTickCount64() + kOsdMs;
                SetTimer(hwnd, kTimerOsd, kOsdMs + 20, nullptr);
            }
            Update();
            if (g_panel.drag < 0) RefreshPanel();
            return 0;
        }

        case WM_APP_AUDIODEV:
            is.audio.OpenDefault();
            is.audio.RefreshMix();
            Update();
            RefreshPanel();
            return 0;

        case WM_APP_SETTINGS:
            ApplySettings();
            return 0;

        case WM_APP_REFRESH:
            if (g_panel.drag < 0) RefreshPanel();
            return 0;

        case WM_APP_TOAST: {
            std::vector<Toast> toasts;
            AcquireSRWLockExclusive(&g_notifLock);
            toasts.swap(g_toasts);
            ReleaseSRWLockExclusive(&g_notifLock);
            if (!g_s.popNotif) return 0;
            for (auto& t : toasts) {
                Island::Activity a;
                a.kind = 1;
                std::wstring name;
                if (!t.app.empty()) a.icon = ShellIcon(L"shell:AppsFolder\\" + t.app, &name);
                if (name.empty() || name.find(L'!') != std::wstring::npos) name = L"Notification";
                a.label = name;
                a.title = t.title.empty() ? name : t.title;
                a.body = t.body;
                a.glyph = Glyph::Bell;
                a.glyphBg = WithAlpha(is.accent, 0.5f);
                a.ms = g_s.popSeconds * 1000;
                PushActivity(std::move(a));
            }
            return 0;
        }

        case WM_APP_LYRICS:
            AcquireSRWLockShared(&g_lyricsLock);
            is.lyrics = g_lyrics;
            ReleaseSRWLockShared(&g_lyricsLock);
            is.lyricIndex = -1;
            UpdateLyricsTimer();
            OnLyricsTick();
            Update();
            return 0;

        case WM_APP_WEATHER:
            AcquireSRWLockShared(&g_weatherLock);
            is.weather = g_weather;
            ReleaseSRWLockShared(&g_weatherLock);
            Update();
            if (PanelKindOpen() == kWeatherPanel) RenderPanel();
            return 0;

        case WM_APP_APPBAR:
            if (wp == ABN_FULLSCREENAPP) {
                bool fs = lp != 0;
                if (fs) {
                    WCHAR cls[64] = {};
                    GetClassName(GetForegroundWindow(), cls, ARRAYSIZE(cls));
                    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) fs = false;
                }
                if (fs != is.fullscreen) {
                    is.fullscreen = fs;
                    if (fs) {
                        ClosePanel();
                        HideTip();
                    }
                    RenderIsland();
                }
            } else if (wp == ABN_POSCHANGED && g_s.reserveSpace) {
                ApplyAppbar();
            }
            return 0;

        case WM_DPICHANGED:
        case WM_DISPLAYCHANGE:
            UpdateMonitor();
            ApplyAppbar();
            RebuildArtBitmaps();
            Update();
            return 0;

        case WM_SETTINGCHANGE:
            if (lp && !wcscmp((PCWSTR)lp, L"ImmersiveColorSet") && g_s.systemAccent) {
                is.accent = ResolveAccent();
            }
            if (wp == SPI_SETWORKAREA) UpdateMonitor();
            Update();
            RefreshPanel();
            return 0;

        case WM_DWMCOLORIZATIONCOLORCHANGED:
            if (g_s.systemAccent) {
                is.accent = ResolveAccent();
                Update();
            }
            return 0;

        case WM_TIMECHANGE:
            ArmClockTimer();
            Update();
            return 0;

        case WM_POWERBROADCAST:
            if (wp == PBT_APMPOWERSTATUSCHANGE || wp == PBT_APMRESUMEAUTOMATIC) {
                is.status.Read();
                CheckCharging();
                ArmClockTimer();
                if (wp == PBT_APMRESUMEAUTOMATIC && g_s.weather) RequestWeather();
                Update();
            }
            return TRUE;

        case WM_CLOSE:
            return 0;
    }

    if (msg == g_msgTaskbarCreated && msg) {
        // Explorer restarted: its icons come back as apps re-add them.
        is.appbar = is.appbarReserved = false;
        ApplyAppbar();
        g_tray.clear();
        Update();
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

HWND CreateLayered(PCWSTR cls, DWORD extraEx) {
    return CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | extraEx, cls,
                          L"Lite Island", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_module,
                          nullptr);
}

DWORD WINAPI UiThreadProc(void*) {
    auto& is = g_is;
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1),
                                 nullptr, (void**)g_gfx.d2d.GetAddressOf())) ||
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   (IUnknown**)g_gfx.dw.GetAddressOf())) ||
        !CreateRenderTarget()) {
        Wh_Log(L"Direct2D unavailable");
        winrt::uninit_apartment();
        return 1;
    }
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(&g_gfx.wic));
    PickIconFont();

    WNDCLASSEX wc{sizeof(wc)};
    wc.style = CS_DBLCLKS;
    wc.hInstance = g_module;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpfnWndProc = IslandProc;
    wc.lpszClassName = kIslandClass;
    RegisterClassEx(&wc);
    wc.lpfnWndProc = PanelProc;
    wc.lpszClassName = kPanelClass;
    RegisterClassEx(&wc);
    wc.style = 0;
    wc.lpfnWndProc = TipProc;
    wc.lpszClassName = kTipClass;
    RegisterClassEx(&wc);

    is.win.hwnd = CreateLayered(kIslandClass, WS_EX_NOACTIVATE);
    g_panel.win.hwnd = CreateLayered(kPanelClass, 0);
    g_tip.hwnd = CreateLayered(kTipClass, WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
    if (!is.win.hwnd || !g_panel.win.hwnd || !g_tip.hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        winrt::uninit_apartment();
        return 1;
    }
    // Explorer (maybe less privileged) sends tray icons and restarts.
    ChangeWindowMessageFilterEx(is.win.hwnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
    ChangeWindowMessageFilterEx(is.win.hwnd, g_msgTaskbarCreated, MSGFLT_ALLOW, nullptr);
    ChangeWindowMessageFilterEx(is.win.hwnd, WM_APP_APPBAR, MSGFLT_ALLOW, nullptr);
    for (HWND h : {is.win.hwnd, g_panel.win.hwnd, g_tip.hwnd}) {
        BOOL exclude = TRUE;
        DwmSetWindowAttribute(h, DWMWA_EXCLUDED_FROM_PEEK, &exclude, sizeof(exclude));
    }
    g_hwnd = is.win.hwnd;

    LoadPinnedTray();
    is.audio.Init();
    SetTimer(is.win.hwnd, kTimerStatus, 30000, nullptr);
    ApplySettings();
    if (g_s.startFolded) {
        is.folded = true;
        is.fold.Snap(1);
        Update();
    }
    if (HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr)) {
        PostMessage(taskbar, g_msgRequestTray, 0, 0);
    }

    while (true) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (r != WAIT_OBJECT_0 + 1) break;
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    g_hwnd = nullptr;
    if (is.appbar) {
        APPBARDATA abd{sizeof(abd), is.win.hwnd};
        SHAppBarMessage(ABM_REMOVE, &abd);
    }
    is.audio.Uninit();
    DestroyWindow(g_tip.hwnd);
    DestroyWindow(g_panel.win.hwnd);
    DestroyWindow(is.win.hwnd);
    UnregisterClass(kTipClass, g_module);
    UnregisterClass(kPanelClass, g_module);
    UnregisterClass(kIslandClass, g_module);
    g_tray.clear();
    for (auto& sc : g_s.shortcuts) sc.icon.Reset();
    is.artThumb.Reset();
    is.artBg.Reset();
    g_gfx.icons.clear();
    g_gfx.formats.clear();
    g_gfx.layer.Reset();
    g_gfx.brush.Reset();
    g_gfx.rt.Reset();
    g_gfx.wic.Reset();
    g_gfx.dw.Reset();
    g_gfx.d2d.Reset();
    g_tip.Free();
    g_panel.win.Free();
    is.win.Free();
    winrt::uninit_apartment();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod entry points (the dedicated windhawk.exe process)

BOOL WhTool_ModInit() {
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_workEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_workEvent) return FALSE;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    return g_uiThread && g_workerThread;
}

void WhTool_ModSettingsChanged() { PostToUi(WM_APP_SETTINGS); }

void WhTool_ModUninit() {
    SetEvent(g_stopEvent);
    for (HANDLE t : {g_uiThread, g_workerThread}) {
        if (t) {
            WaitForSingleObject(t, 5000);
            CloseHandle(t);
        }
    }
    for (int i = 0; i < 50 && g_jobs > 0; i++) Sleep(100);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// This mod also targets explorer.exe, where each callback runs the small tray
// listener instead.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&Wh_ModInit, &g_module);

    if (IsExplorerProcess()) {
        g_isExplorer = true;
        return ex::Init();
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 ||
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
    if (g_isExplorer) {
        ex::AfterInit();
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

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
        LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr,
                                 FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi,
                                 nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModBeforeUninit() {
    if (g_isExplorer) ex::BeforeUninit();
}

void Wh_ModSettingsChanged() {
    if (g_isExplorer || g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer || g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
