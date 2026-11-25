// ==WindhawkMod==
// @id              anima-windhawk-engine
// @name            AnimaWindhawkEngine
// @description     Displays images, animated GIFs and short clips as draggable desktop widgets.
// @version         0.1
// @author          TuBeo5866
// @github          https://github.com/TuBeo5866
// @donateUrl       https://ko-fi.com/tubeo5866
// @homepage        https://tubb3ll.xyz
// @include         windhawk.exe
// @compilerOptions -lkernel32 -lwtsapi32 -ld2d1 -ld3d11 -lruntimeobject -lwindowsapp -ldwmapi -luuid -lgdiplus -lgdi32 -lshell32 -luser32 -lcomctl32 -lcomdlg32 -lole32 -lurlmon -luxtheme -lwininet -lshlwapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# AnimaWindhawkEngine

💸 Don't have money to buy AnimaEngine but wanna try it? This mod is for you: a desktop media companion

## What's this?

**AnimaWindhawkEngine** is an open-source desktop mod built with [Windhawk](https://windhawk.net/) Engine that turns any images into free-floating "desktop media". Import it, and every image inside becomes its own living right on your desktop.

Like AnimaEngine, this mod lets you place animated media on your desktop. It runs through Windhawk without Steam and keeps the setup lightweight by leaving out the AI background remover. You can also group media by folder and set a separate frame-rate limit for each GIF.

## Features

- 🐾 **Free-floating desktop media** — any PNG, JPG, BMP, GIF, WebP, APNG, or experimental MP4 becomes an image that lives on your desktop, above or below other windows.
- 🖱️ **Drag & drop positioning** — click and drag any image to wherever you want it; positions are saved and restored across sessions.
- 🎛️ **Per-image controls** — toggle always-on-top, click-through, lock position, pause animation, or adjust scale, rotation, and opacity individually per image.
- 🧩 **AWHE Workshop** — built-in browser to discover, preview, and install community-made media directly from GitHub without leaving the app.
- 🗃️ **Media Manager window** — browse thumbnails, search your library and adjust each image from one window.
- 🌙 **Dark / Light / System theme** — the manager window follows your chosen theme or automatically matches Windows' system preference.
- 🎨 **Accent color picker** — choose from preset colors or lock to your Windows system accent color.
- ⌨️ **Global hotkey** — open the Media Manager from anywhere with a configurable key combo (default: Win + Alt + P).
- 🔄 **Hot-reload** — add, remove, or change images and hit Reload without restarting Windhawk.
- 💾 **Position memory** — remembered per image entry so every image wakes up exactly where you left it.
- 🔔 **System-tray icon** — open the manager or quit from the notification area.

## Where's the workshop? AWHE Browser here!

The **AWHE Browser** (AnimaWindhawkEngine Browser) is a built-in image discovery panel inside the Media Manager window. It lets you browse, preview and install community-made media directly from this repository.

- **Browse** — scroll through the community image grid, each shown with its preview thumbnail, name and author
- **Install** — hit Download on any image and it lands straight into your configured images folder and reloads automatically
- **Submit** — made something cool? Publish it for everyone via a pull request (see below)

### Network access

Opening the Workshop for the first time in a session or choosing **Refresh** requests the index from `raw.githubusercontent.com` in the `tubeo5866/anima-windhawk-engine` repository. The Workshop refreshes pack metadata and caches the index and previews for offline browsing. Install and Update download the selected pack’s media and animation frames from the same host. All other mod features work offline, including the tray icon and both windows.

## Desktop controls

Drop supported files onto the manager to import them. Right-click a desktop item and choose **Duplicate** for an independent copy. Over an interactive item, use Ctrl + wheel to resize, Shift + wheel to rotate, or Alt + wheel to adjust opacity. Ctrl + Alt + Shift + H temporarily hides or restores all visible items. App settings includes fullscreen hiding and edge snapping. Each item has its own flip, color and shadow settings. Animated media also supports speed, reverse, ping-pong and a loop limit.

## Scenes and playback

In App settings, save or open a scene JSON file to switch layouts. The sidebar shows up to three recent or scheduled profiles above Workshop. Scenes store local file references and per-item settings. Schedule a scene at a local 24-hour time while the mod is running. On each item, you can follow the active virtual desktop or fade near the pointer. MP4 audio starts muted. Forward playback supports volume control, and reverse playback is silent. The resource-saving dialog lets you allow covered or locked playback to pause and battery playback to slow down, or keep playing. You can revisit this choice in App settings.

## Usage

1. Open the manager from the tray icon or press **Win + Alt + P** (the default shortcut).
2. Click the arrow beside **Import media…** and choose **from images** to pick files or **from folder** to add a folder.
3. Select an item in the sidebar to adjust its scale, rotation, or opacity. Drag it into place on your desktop.
4. Done! Your changes are saved automatically.

## Publish your own images!

Find it great for everyone? Make a **pull request**!
(For updating images just do the same)

**Step 1**: Fork this repository.
**Step 2**: Put these files in your forked repository:

```
workshop/pets/<your-image-name>/
├── your-image.here       ← the actual image (.png .jpg .jpeg .bmp .gif .webp .apng .mp4)
├── preview.png           ← a square thumbnail that will show in the Browser (recommended: 256×256)
└── meta.json             ← metadata about your image (see format below)
```

**Step 3**: Fill in `meta.json` like this:

```json
{
  "name": "My Cool Cat",
  "author": "YourGitHubUsername",
  "description": "A little cat that sits on your desktop.",
  "tags": ["cat", "cute", "wtfwhyisitcute"],
  "version": "1.0",
  "file": "cat.gif",
  "preview": "preview.png"
}
```

For a sprite sheet, set `file` to the sheet PNG and add numeric `frameWidth`, `frameHeight`, and `fps`. Every tile plays in row-major order. For a PNG sequence, set `frames` to an ordered array of PNG filenames and `file` to the first frame. Bump the string `version` for every pack update.

**Step 4**: Add your folder name to the existing string array in `workshop/index.json` and open a pull request to the Workshop repository’s main branch.

That’s it! Once your pull request is approved and merged, your image will automatically appear in the Workshop tab inside the manager for everyone to download.

*(We'll manually review and merge to our workshop.)*

*(GitHub may take a few seconds or a few minutes to upload & update the repository, so your request may not appear immediately.)*

## Donate!

Though this mod and the whole workshop is completely free, I'm broke when building this.

So a little of your support helps the mod grow, even just a simple dollar.

Donate via the small blue button "❤️ Donate" at Windhawk mod page, or
via [Ko-fi link here](https://ko-fi.com/tubeo5866).

## License

Published under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

**Windhawk** is a customization app developed by **Ramen Software**.

Inspired by **AnimaEngine** app on [Steam](https://store.steampowered.com/app/3474900/Anima_Engine/)! It is a companion app developed and published by **MarroIndustries**.

The manager window is heavily made and based on by [Mac Island & Dock](https://windhawk.net/mods/mac-island-dock) mod, made by [caliberda](https://github.com/cesarkali).

(though I made mine so bad compared to it honestly)

## Support

- Discord: `tubeo5866#5287`
- Email: `support@tubb3ll.xyz`
- Telegram: `tubeo5866`

---
*/
// ==/WindhawkModReadme==

// Source code is published under the GNU General Public License v3.0.

#include <atomic>
#include <array>
#include <list>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <fstream>
#include <windows.h>
#include <windowsx.h>

#undef GetCurrentTime
#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <wtsapi32.h>
#include <climits>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <dwmapi.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <robuffer.h>
#include <roapi.h>
#include <memory>

#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <propsys.h>
#include <sddl.h>
#include <utility>
#include <shlwapi.h>
#include <urlmon.h>
#include <wininet.h>
#include <wincrypt.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <commdlg.h>
#include <cmath>
#include <cwctype>
#include <cwchar>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

// ============================================================================
// Settings model
// ============================================================================

struct GlobalSettings {
    bool scanSubfolders = false;
    std::vector<std::wstring> imagePaths;
    // -------------------
    bool alwaysOnTop = true;
    bool clickThrough = false;
    bool lockPosition = false;
    int scalePercent = 100;
    int rotationDegrees = 0;
    int opacityPercent = 100;
    bool paused = false;
    int maxFps = 20;
    bool trayIcon = true;
};

static GlobalSettings g_settings;

// ============================================================================
// Runtime item model
// ============================================================================

namespace json = winrt::Windows::Data::Json;
namespace fs = std::filesystem;
using FrameKey = std::array<int, 14>;
struct PetItem {
    FrameKey presented{};
    int presentedAlpha = -1;
    std::wstring sourcePath;
    GUID homeDesktop{};
    bool followDesktop = false, hoverFade = false, muted = true;
    bool nearCursor = false, covered = false, otherDesktop = false;
    int volume = 70;
    ULONGLONG audioCheck = 0;
    bool audioFailed = false, audioPlaying = false;
    winrt::Windows::Media::Playback::MediaPlayer audio{nullptr};
    std::wstring filePath;
    std::wstring registryKey;
    std::wstring folderKey;
    std::wstring fileName;
    Gdiplus::Image* image = nullptr;  // aliases decodedFrames when non-empty
    std::vector<Gdiplus::Bitmap*> decodedFrames;
    UINT frameCount = 1;
    std::vector<UINT> frameDelaysMs;
    UINT currentFrame = 0;
    bool isGif = false;
    int fpsLimit = 20;
    int monitorHz = 60;
    ULONGLONG lastFrameTick = 0;
    HWND hwnd = nullptr;
    // thumbnail bitmap for manager (96×96 HBITMAP, lazily created)
    HBITMAP hThumb = nullptr;
    int baseWidth = 1;
    int baseHeight = 1;
    int scalePercent = 100;
    int rotationDegrees = 0;
    int opacityPercent = 100;
    bool alwaysOnTop = true;
    bool clickThrough = false;
    bool lockPosition = false;
    bool paused = false;
    bool hidden = false;
    bool enabled = true;
    int posX = 80;
    int posY = 80;
    bool flipX = false, flipY = false, grayscale = false, shadow = false;
    bool reverse = false, pingPong = false, returning = false;
    int brightness = 100, hue = 0, tint = 0, speed = 100, loops = 0;
    unsigned completed = 0;
    int wheelRemainder = 0, wheelMode = 0;
    bool dragging = false;
    POINT dragStartCursor{};
    POINT dragOriginWnd{};
};

static std::vector<PetItem> g_items;
static bool hideFullscreen = true, fullscreen = false, panic = false;
static bool snapEdges = false, panicAvailable = false;

static bool conservePower = true, pauseCovered = true, onBattery = false,
            sessionLocked = false;
enum class SavingChoice { Ask, Allow, KeepPlaying };
static SavingChoice savingChoice = SavingChoice::Ask;
static HWND savingDialog = nullptr;
static bool lockOccurred = false;
static void ShowSavingDialog();
static bool SavingAllowed() {
    return savingChoice == SavingChoice::Allow;
}
static winrt::com_ptr<IVirtualDesktopManager> desktops;
static bool VisibleItem(const PetItem& item) {
    return item.enabled && !item.hidden && !panic && !fullscreen;
}

static void RefreshVisibility() {
    for (auto& item : g_items)
        if (item.hwnd)
            ShowWindow(item.hwnd,
                       VisibleItem(item) ? SW_SHOWNOACTIVATE : SW_HIDE);
}

static void UpdateFullscreen() {
    QUERY_USER_NOTIFICATION_STATE state{};
    bool hide = hideFullscreen &&
                SUCCEEDED(SHQueryUserNotificationState(&state)) &&
                (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
                 state == QUNS_PRESENTATION_MODE);
    HWND fg = GetForegroundWindow();
    if (!hide && hideFullscreen && fg && fg != GetShellWindow() &&
        fg != GetDesktopWindow() && IsWindowVisible(fg) && !IsIconic(fg)) {
        wchar_t cls[64]{};
        GetClassNameW(fg, cls, 64);
        RECT rect{};
        MONITORINFO info{sizeof(info)};
        DWORD pid{};
        GetWindowThreadProcessId(fg, &pid);
        hide = pid != GetCurrentProcessId() && wcscmp(cls, L"WorkerW") &&
               wcscmp(cls, L"Progman") && wcscmp(cls, L"Shell_TrayWnd") &&
               GetWindowRect(fg, &rect) &&
               GetMonitorInfoW(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST),
                               &info) &&
               rect.left <= info.rcMonitor.left &&
               rect.top <= info.rcMonitor.top &&
               rect.right >= info.rcMonitor.right &&
               rect.bottom >= info.rcMonitor.bottom;
    }
    if (fullscreen != hide) {
        fullscreen = hide;
        RefreshVisibility();
    }
}

// ============================================================================
// Forward declarations
// ============================================================================
static void RenderItem(PetItem& it);
static void ApplyTopmost(PetItem& it);
static void ApplyClickThrough(PetItem& it);
static void ReloadAll(bool rereadSettings = true);
static void ApplyTrayIcon();
static void ApplyTimer();

static void OpenPetManager(int focusIndex = -1);  // -1 = no pre-selection
static void ShowWelcomeGuide(bool recordFirstRun = false);
static void SaveStoredPosition(const std::wstring& fileName, int x, int y);
static void ForgetStoredPosition(const std::wstring& fileName);

// ============================================================================
// Thread / window handles
// ============================================================================

static UINT g_taskbarCreatedMsg = 0;
static std::atomic_bool workerQuitting{false};
static HWND g_hCtrlWnd = nullptr;
static HWND g_hManagerWnd = nullptr;
static DWORD g_threadId = 0;
static HANDLE g_hThread = nullptr;
static HANDLE g_threadReadyEvent = nullptr;
static ULONG_PTR g_gdiToken = 0;
static bool g_trayIconAdded = false;

static const wchar_t kItemWndClass[] = L"AnimaWindhawkEngine_Item";
static const wchar_t kCtrlWndClass[] = L"AnimaWindhawkEngine_Ctrl";
static const wchar_t kManagerWndClass[] = L"AnimaWindhawkEngine_Manager";

static const UINT kAnimTimerId = 1;
static const UINT kTrayMsg = WM_APP + 1;
static const UINT kTrayUid = 1;
static const UINT WM_APP_QUIT = WM_APP + 2;
static const UINT WM_APP_RELOAD = WM_APP + 3;
// Like WM_APP_RELOAD but skips ReadGlobalSettings — used when the manager UI
// has already updated g_settings in-memory and we just need to respawn pets.
static const UINT WM_APP_RELOAD_ITEMS_ONLY = WM_APP + 5;
// Posted to controller: open manager, lParam = pet index (-1 = none)
static const UINT WM_APP_OPEN_MANAGER = WM_APP + 4;
// Workshop messages (posted to g_hManagerWnd from worker threads)
static const UINT WM_WS_FETCH_DONE = WM_APP + 6;  // wParam=0 ok, 1=error
static const UINT WM_WS_INSTALL_DONE =
    WM_APP + 7;  // wParam=itemIndex, lParam=0 ok 1=err

// Workshop view modes
enum class WorkshopView { Grid = 0, List = 1, TwoCol = 2 };

// One entry fetched from GitHub
struct WorkshopItem {
    std::wstring metadata, installedVersion, revision;
    std::wstring slug;  // folder name under workshop/pets/
    std::wstring name;
    std::wstring author;
    std::wstring description;
    std::wstring version;
    std::wstring file;         // image filename (e.g. "cat.gif")
    std::wstring previewFile;  // usually "preview.png"
    std::vector<std::wstring> tags;
    // Runtime state
    std::wstring
        localPreviewPath;  // cached preview image on disk (empty = not yet)
    std::wstring
        localImagePath;  // installed image path (empty = not installed)
    bool installed = false;
    bool installing = false;
    HBITMAP hPreviewBmp =
        nullptr;  // loaded from localPreviewPath, scaled to thumb
};

static std::vector<WorkshopItem> g_workshopItems;
static std::wstring workshopTag;
static bool favoritesOnly = false;
static std::wstring g_workshopError;  // non-empty = last fetch failed
static bool g_workshopFetching = false;
static bool g_workshopFetched = false;
static WorkshopView g_workshopView = WorkshopView::Grid;
static std::wstring g_workshopFilter;  // lowercase search text

// Placeholder repo — replace with real URL once the repo exists.
static const wchar_t kWorkshopRepoOwner[] = L"tubeo5866";
static const wchar_t kWorkshopRepoName[] = L"anima-windhawk-engine";
// raw.githubusercontent.com base for file downloads
// https://raw.githubusercontent.com/<owner>/<repo>/main/workshop/pets/<slug>/<file>

// Welcome guide command to open the Workshop page.
#define IDC_NAV_WORKSHOP 261

// Hot-key ID registered with RegisterHotKey (Win+Alt+P → open manager)
#define HOTKEY_OPEN_MANAGER 1
static constexpr int kToggleMediaHotkey = 2;

// ============================================================================
// Menu command IDs
// ============================================================================
enum ItemMenuCmd {
    kCmdOpenManager = 1,
    kCmdToggleAlwaysOnTop,
    kCmdToggleClickThrough,
    kCmdToggleLock,
    kCmdTogglePause,
    kCmdBringToFront,
    kCmdHideThis,
    kCmdResetPosition,
    kCmdReloadAll,
};

// Commands for the "⋮" per-pet sidebar menu
enum PetMenuCmd {
    kPetMenuEnable = 500,
    kPetMenuDisable,
    kPetMenuClone,
    kPetMenuReload,
    kPetMenuDelete,
    kPetMenuHide,
    kPetMenuShow,
    kPetMenuBringToFront,
    kPetMenuResetPos,
    kPetMenuAlwaysOnTopOn,
    kPetMenuAlwaysOnTopOff,
    kPetMenuClickThroughOn,
    kPetMenuClickThroughOff,
    kPetMenuLockOn,
    kPetMenuLockOff,
};

enum TrayMenuCmd { kCmdOpenManagerTray = 101, kCmdQuit };

// ============================================================================
// Small helpers
// ============================================================================

static std::wstring GetFileNameFromPath(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

static bool HasSupportedExt(const std::wstring& fn) {
    size_t pos = fn.find_last_of(L'.');
    if (pos == std::wstring::npos)
        return false;
    std::wstring ext = fn.substr(pos);
    for (auto& c : ext)
        c = (wchar_t)towlower(c);
    static const wchar_t* kExts[] = {L".png", L".jpg",  L".jpeg", L".bmp",
                                     L".gif", L".webp", L".apng", L".mp4"};
    for (auto e : kExts)
        if (ext == e)
            return true;
    return false;
}

static bool ScanFolder(const std::wstring& folder,
                       bool recurse,
                       std::vector<std::wstring>& out) {
    if (folder.empty())
        return false;
    std::wstring base = folder;
    wchar_t last = base.empty() ? 0 : base.back();
    if (last != L'\\' && last != L'/')
        base += L'\\';
    std::wstring searchPat = base + L"*";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(searchPat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE)
        return GetLastError() == ERROR_FILE_NOT_FOUND;
    bool complete = true;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
            continue;
        std::wstring full = base + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recurse &&
                !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                if (!ScanFolder(full, recurse, out))
                    complete = false;
            }
        } else if (HasSupportedExt(fd.cFileName)) {
            out.push_back(full);
        }
    } while (FindNextFileW(h, &fd));
    complete = GetLastError() == ERROR_NO_MORE_FILES && complete;
    FindClose(h);
    return complete;
}

// ============================================================================
// Remembered positions
// ============================================================================

static std::wstring userPrefix, userFiles;
static bool configWritable = false;
static std::vector<std::wstring> imageOrder;
struct LibraryFolder {
    std::wstring key, path;
    bool expanded;
};
static std::vector<LibraryFolder> libraryFolders;

static void OpenConfig() {
    HANDLE token{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return;
    DWORD bytes{};
    GetTokenInformation(token, TokenUser, nullptr, 0, &bytes);
    std::vector<BYTE> data(bytes);
    LPWSTR sid{};
    if (bytes &&
        GetTokenInformation(token, TokenUser, data.data(), bytes, &bytes) &&
        ConvertSidToStringSidW(
            reinterpret_cast<TOKEN_USER*>(data.data())->User.Sid, &sid)) {
        userPrefix = std::wstring(sid) + L"\\";
        wchar_t root[32768];
        if (Wh_GetModStoragePath(root, std::size(root))) {
            userFiles = std::wstring(root) + L"\\" + sid;
            if (!CreateDirectoryW(userFiles.c_str(), nullptr) &&
                GetLastError() != ERROR_ALREADY_EXISTS)
                userFiles.clear();
            auto attrs = GetFileAttributesW(userFiles.c_str());
            if (attrs == INVALID_FILE_ATTRIBUTES ||
                !(attrs & FILE_ATTRIBUTE_DIRECTORY) ||
                (attrs & FILE_ATTRIBUTE_REPARSE_POINT))
                userFiles.clear();
        }
        LocalFree(sid);
        configWritable = true;
    }
    CloseHandle(token);
}
static std::wstring Setting(const std::wstring& name) {
    return userPrefix + L"Configurations\\" + name;
}
static std::wstring Record(const std::wstring& key, const std::wstring& field) {
    return userPrefix + L"Loaded_Images\\" + key + L"\\" + field;
}
static std::wstring ReadString(const std::wstring& key) {
    if (!configWritable)
        return {};
    std::wstring value(32768, L'\0');
    value.resize(Wh_GetStringValue(key.c_str(), value.data(), value.size()));
    return value;
}
static bool PutString(const std::wstring& key, const std::wstring& value) {
    if (configWritable && Wh_SetStringValue(key.c_str(), value.c_str()))
        return true;
    Wh_Log(L"Could not save %s", key.c_str());
    return false;
}
static std::wstring ReadText(PCWSTR name) {
    return ReadString(Setting(name));
}
static bool SaveText(PCWSTR name, const std::wstring& value) {
    return PutString(Setting(name), value);
}
static int Number(const std::wstring& key, int fallback) {
    return configWritable ? Wh_GetIntValue(key.c_str(), fallback) : fallback;
}
static bool PutNumber(const std::wstring& key, int value) {
    if (configWritable && Wh_SetIntValue(key.c_str(), value))
        return true;
    Wh_Log(L"Could not save %s", key.c_str());
    return false;
}
static DWORD ConfigNumber(PCWSTR name, DWORD fallback) {
    return Number(Setting(name), fallback);
}
static bool SaveNumber(PCWSTR name, DWORD value) {
    return PutNumber(Setting(name), value);
}
static std::wstring ListBank(const std::wstring& list, int bank) {
    return list + (bank ? L".B." : L".A.");
}
static std::vector<std::wstring> ReadList(const std::wstring& list) {
    std::vector<std::wstring> values;
    auto bank = ListBank(list, Number(list, 0));
    int count = Number(bank + L"Count", 0);
    for (int i = 0; i < count && i < 100000; ++i) {
        auto value = ReadString(bank + std::to_wstring(i));
        if (!value.empty())
            values.push_back(std::move(value));
    }
    return values;
}
static bool StoreList(const std::wstring& list,
                      const std::vector<std::wstring>& values) {
    if (!configWritable || values.size() > 100000)
        return false;
    int next = !Number(list, 0);
    auto bank = ListBank(list, next);
    for (size_t i = 0; i < values.size(); ++i)
        if (!PutString(bank + std::to_wstring(i), values[i]))
            return false;
    return PutNumber(bank + L"Count", static_cast<int>(values.size())) &&
           PutNumber(list, next);
}
static std::wstring FolderList() {
    return userPrefix + L"Loaded_Images\\Folders.Index";
}
static std::wstring ImageList() {
    return Setting(L"ImageOrder");
}
static void ReadLibraryFolders() {
    libraryFolders.clear();
    for (auto& key : ReadList(FolderList())) {
        auto path = ReadString(Record(key, L"Path"));
        if (!path.empty())
            libraryFolders.push_back(
                {key, path, Number(Record(key, L"Expanded"), 1) != 0});
    }
}
static void DeleteImageRecord(const std::wstring& key) {
    for (PCWSTR field : {L"Path",
                         L"Position",
                         L"FollowDesktop",
                         L"HoverFade",
                         L"Muted",
                         L"Volume",
                         L"DesktopId",
                         L"Enabled",
                         L"AlwaysOnTop",
                         L"ClickThrough",
                         L"LockPosition",
                         L"ScalePercent",
                         L"OpacityPercent",
                         L"RotationDegrees",
                         L"FpsLimit",
                         L"Paused",
                         L"Removed",
                         L"FlipHorizontal",
                         L"FlipVertical",
                         L"Grayscale",
                         L"Shadow",
                         L"Tint",
                         L"Brightness",
                         L"Hue",
                         L"PlaybackSpeed",
                         L"Reverse",
                         L"PingPong",
                         L"LoopCount"})
        Wh_DeleteValue(Record(key, field).c_str());
}
static bool RemoveLibraryFolder(const std::wstring& key) {
    auto folders = ReadList(FolderList());
    folders.erase(std::remove(folders.begin(), folders.end(), key),
                  folders.end());
    if (!StoreList(FolderList(), folders))
        return false;
    auto index = Record(key, L"Images.Index");
    for (auto& child : ReadList(index))
        DeleteImageRecord(child);
    for (int i = 0; i < 2; ++i) {
        auto bank = ListBank(index, i);
        int count = Number(bank + L"Count", 0);
        for (int n = 0; n < count && n < 100000; ++n)
            Wh_DeleteValue((bank + std::to_wstring(n)).c_str());
        Wh_DeleteValue((bank + L"Count").c_str());
    }
    Wh_DeleteValue(index.c_str());
    Wh_DeleteValue(Record(key, L"Expanded").c_str());
    Wh_DeleteValue(Record(key, L"Path").c_str());
    return true;
}
static std::wstring RecordName(const std::wstring& leaf) {
    std::wstring name;
    for (wchar_t ch : leaf) {
        if (ch == L'%' || ch == L'=' || ch == L'[' || ch == L']') {
            wchar_t escaped[6];
            swprintf_s(escaped, L"%%%04X", static_cast<unsigned>(ch));
            name += escaped;
        } else
            name += ch;
    }
    return name;
}
static std::wstring FreeRecord(const std::wstring& base) {
    for (unsigned n = 1;; ++n) {
        auto key = n == 1 ? base : base + L" (" + std::to_wstring(n) + L")";
        if (ReadString(Record(key, L"Path")).empty())
            return key;
    }
}
static bool RegisterLibraryFolder(const std::wstring& path) {
    if (!configWritable || path.empty())
        return false;
    for (auto& f : libraryFolders)
        if (!_wcsicmp(path.c_str(), f.path.c_str()))
            return true;
    auto name = GetFileNameFromPath(path);
    if (name.empty())
        name = L"Drive " + path.substr(0, 1);
    auto key = FreeRecord(L"Folders\\" + RecordName(name));
    auto keys = ReadList(FolderList());
    keys.push_back(key);
    if (!PutString(Record(key, L"Path"), path) ||
        !StoreList(FolderList(), keys))
        return false;
    libraryFolders.push_back({key, path, true});
    return true;
}
static std::wstring ImageKey(const std::wstring& path,
                             const std::vector<std::wstring>& used,
                             const std::wstring& folder = {}) {
    if (!configWritable || path.empty())
        return {};
    auto keys =
        folder.empty() ? imageOrder : ReadList(Record(folder, L"Images.Index"));
    for (auto& key : keys)
        if (std::find(used.begin(), used.end(), key) == used.end() &&
            !_wcsicmp(ReadString(Record(key, L"Path")).c_str(), path.c_str()))
            return key;
    auto key = FreeRecord((folder.empty() ? L"" : folder + L"\\") +
                          RecordName(GetFileNameFromPath(path)));
    if (!PutString(Record(key, L"Path"), path))
        return {};
    if (!folder.empty()) {
        keys.push_back(key);
        if (!StoreList(Record(folder, L"Images.Index"), keys))
            return {};
    }
    return key;
}
static bool ReadStoredPosition(const std::wstring& key, POINT& point) {
    return configWritable && !key.empty() &&
           Wh_GetBinaryValue(Record(key, L"Position").c_str(), &point,
                             sizeof(point)) == sizeof(point);
}
static void SaveStoredPosition(const std::wstring& key, int x, int y) {
    POINT point{x, y};
    if (configWritable && !key.empty() &&
        !ReadString(Record(key, L"Path")).empty())
        Wh_SetBinaryValue(Record(key, L"Position").c_str(), &point,
                          sizeof(point));
}
static void ForgetStoredPosition(const std::wstring& key) {
    if (configWritable && !key.empty())
        Wh_DeleteValue(Record(key, L"Position").c_str());
}
static void SyncImageState(PetItem& item, bool load) {
    if (!configWritable || item.registryKey.empty())
        return;
    auto sync = [&](PCWSTR field, auto& value, int low, int high) {
        auto key = Record(item.registryKey, field);
        if (load) {
            int saved = Number(key, INT_MIN);
            if (saved != INT_MIN) {
                value = std::clamp(saved, low, high);
                return;
            }
        }
        PutNumber(key, static_cast<int>(value));
    };
    sync(L"FollowDesktop", item.followDesktop, 0, 1);
    sync(L"HoverFade", item.hoverFade, 0, 1);
    sync(L"Muted", item.muted, 0, 1);
    sync(L"Volume", item.volume, 0, 100);
    sync(L"FlipHorizontal", item.flipX, 0, 1);
    sync(L"FlipVertical", item.flipY, 0, 1);
    sync(L"Grayscale", item.grayscale, 0, 1);
    sync(L"Shadow", item.shadow, 0, 1);
    sync(L"Tint", item.tint, 0, 100);
    sync(L"Brightness", item.brightness, 0, 200);
    sync(L"Hue", item.hue, 0, 359);
    sync(L"PlaybackSpeed", item.speed, 10, 400);
    sync(L"Reverse", item.reverse, 0, 1);
    sync(L"PingPong", item.pingPong, 0, 1);
    sync(L"LoopCount", item.loops, 0, 100);
    sync(L"Enabled", item.enabled, 0, 1);
    sync(L"AlwaysOnTop", item.alwaysOnTop, 0, 1);
    sync(L"ClickThrough", item.clickThrough, 0, 1);
    sync(L"LockPosition", item.lockPosition, 0, 1);
    sync(L"ScalePercent", item.scalePercent, 1, 500);
    sync(L"OpacityPercent", item.opacityPercent, 10, 100);
    sync(L"RotationDegrees", item.rotationDegrees, 0, 359);
    if (item.isGif)
        sync(L"FpsLimit", item.fpsLimit, 5, INT_MAX);
    if (item.frameCount > 1)
        sync(L"Paused", item.paused, 0, 1);
}

// Workshop infrastructure
// ============================================================================

// Cache Workshop downloads under the per-user mod storage directory.
static std::wstring GetWorkshopDir() {
    if (userFiles.empty())
        return {};
    auto path = userFiles + L"\\workshop";
    if (!CreateDirectoryW(path.c_str(), nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS)
        return {};
    auto attrs = GetFileAttributesW(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES ||
        !(attrs & FILE_ATTRIBUTE_DIRECTORY) ||
        (attrs & FILE_ATTRIBUTE_REPARSE_POINT))
        return {};
    return path;
}

// Build raw.githubusercontent.com URL for a file in the workshop
static bool SafeWorkshopName(const std::wstring& name) {
    if (name.empty() || name.size() > 200 || name.front() == L'.' ||
        name.back() == L'.' || name.find(L"..") != name.npos ||
        name.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrst"
                               L"uvwxyz0123456789._-") != name.npos)
        return false;
    auto stem = name.substr(0, name.find(L'.'));
    for (auto& c : stem)
        if (c >= L'a' && c <= L'z')
            c -= L'a' - L'A';
    if (stem == L"CON" || stem == L"NUL" || stem == L"AUX" || stem == L"PRN")
        return false;
    return !(stem.size() == 4 &&
             (stem.substr(0, 3) == L"COM" || stem.substr(0, 3) == L"LPT") &&
             stem[3] >= L'0' && stem[3] <= L'9');
}
static std::wstring WorkshopChild(const std::wstring& parent,
                                  const std::wstring& name) {
    if (parent.empty() || !SafeWorkshopName(name))
        return {};
    auto path = parent + L"\\" + name;
    wchar_t full[32768];
    DWORD len = GetFullPathNameW(path.c_str(), std::size(full), full, nullptr);
    if (!len || len >= std::size(full) || _wcsicmp(full, path.c_str()))
        return {};
    DWORD attrs = GetFileAttributesW(full);
    return attrs != INVALID_FILE_ATTRIBUTES &&
                   (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
               ? L""
               : path;
}
static std::wstring WorkshopRawUrl(const std::wstring& slug,
                                   const std::wstring& file) {
    return std::wstring(L"https://raw.githubusercontent.com/") +
           kWorkshopRepoOwner + L"/" + kWorkshopRepoName +
           L"/main/workshop/pets/" + slug + L"/" + file;
}

// Raw URL for workshop/index.json — the slug list, no API needed.
static std::wstring WorkshopIndexUrl() {
    return std::wstring(L"https://raw.githubusercontent.com/") +
           kWorkshopRepoOwner + L"/" + kWorkshopRepoName +
           L"/main/workshop/index.json";
}

// Simple blocking HTTP GET using WinINet — returns body as UTF-8 string.
static std::string HttpGet(const std::wstring& url) {
    HINTERNET hInet =
        InternetOpenW(L"AnimaWindhawkEngine/1.0", INTERNET_OPEN_TYPE_PRECONFIG,
                      nullptr, nullptr, 0);
    if (!hInet)
        return {};
    DWORD timeout = 15000;
    for (auto option :
         {INTERNET_OPTION_CONNECT_TIMEOUT, INTERNET_OPTION_RECEIVE_TIMEOUT})
        InternetSetOptionW(hInet, option, &timeout, sizeof(timeout));
    HINTERNET hConn =
        InternetOpenUrlW(hInet, url.c_str(), nullptr, 0,
                         INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                             INTERNET_FLAG_SECURE,
                         0);
    std::string body;
    DWORD status = 0, size = sizeof(status);
    if (hConn &&
        HttpQueryInfoW(hConn, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &status, &size, nullptr) &&
        status == 200) {
        char buf[4096];
        DWORD read = 0;
        while (InternetReadFile(hConn, buf, sizeof(buf), &read) && read) {
            if (body.size() + read > 1024 * 1024) {
                body.clear();
                break;
            }
            body.append(buf, read);
        }
    }
    if (hConn)
        InternetCloseHandle(hConn);
    InternetCloseHandle(hInet);
    return body;
}

// Download a URL to a local file path (blocking).
static bool HttpDownloadFile(const std::wstring& url,
                             const std::wstring& dest) {
    if (dest.empty())
        return false;
    HINTERNET session =
        InternetOpenW(L"AnimaWindhawkEngine/1.0", INTERNET_OPEN_TYPE_PRECONFIG,
                      nullptr, nullptr, 0);
    if (!session)
        return false;
    DWORD timeout = 15000;
    InternetSetOptionW(session, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                       sizeof(timeout));
    InternetSetOptionW(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                       sizeof(timeout));
    HINTERNET request =
        InternetOpenUrlW(session, url.c_str(), nullptr, 0,
                         INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE |
                             INTERNET_FLAG_NO_CACHE_WRITE,
                         0);
    DWORD status = 0, n = sizeof(status);
    bool ok =
        request &&
        HttpQueryInfoW(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &status, &n, nullptr) &&
        status == 200;
    auto temp = dest + L".part";
    HANDLE out = ok ? CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                                  CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr)
                    : INVALID_HANDLE_VALUE;
    if (out != INVALID_HANDLE_VALUE) {
        DWORD bytes = 0, total = 0;
        char buffer[32768];
        while ((ok = InternetReadFile(request, buffer, sizeof(buffer),
                                      &bytes) != FALSE) &&
               bytes) {
            DWORD written = 0;
            total += bytes;
            if (total > 128u * 1024 * 1024 ||
                !WriteFile(out, buffer, bytes, &written, nullptr) ||
                written != bytes) {
                ok = false;
                break;
            }
        }
        CloseHandle(out);
        ok = ok && total &&
             MoveFileExW(temp.c_str(), dest.c_str(), MOVEFILE_REPLACE_EXISTING);
        if (!ok)
            DeleteFileW(temp.c_str());
    } else
        ok = false;
    if (request)
        InternetCloseHandle(request);
    InternetCloseHandle(session);
    return ok;
}

// Keep this value in sync with the @version metadata above.
static const wchar_t* kModVersion = WH_MOD_VERSION;
static std::wstring g_windowsBuild = L"unknown";
static void LoadBuildInfo() {
    HKEY key;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
                      KEY_READ | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS) {
        wchar_t build[64]{};
        DWORD bytes = sizeof(build), type = 0, revision = 0;
        if (RegQueryValueExW(key, L"CurrentBuildNumber", nullptr, &type,
                             (BYTE*)build, &bytes) == ERROR_SUCCESS &&
            type == REG_SZ) {
            build[63] = 0;
            g_windowsBuild = build;
            bytes = sizeof(revision);
            if (RegQueryValueExW(key, L"UBR", nullptr, &type, (BYTE*)&revision,
                                 &bytes) == ERROR_SUCCESS &&
                type == REG_DWORD)
                g_windowsBuild += L"." + std::to_wstring(revision);
        }
        RegCloseKey(key);
    }
}

static std::string ReadSmallFile(const std::wstring& path) {
    std::ifstream f(fs::path(path), std::ios::binary | std::ios::ate);
    auto size = f.tellg();
    if (size <= 0 || size > 1024 * 1024)
        return {};
    std::string data(static_cast<size_t>(size), '\0');
    f.seekg(0);
    return f.read(data.data(), data.size()) ? data : std::string{};
}

static bool WriteSmallFile(const std::wstring& path, const std::string& data) {
    if (path.empty() || data.empty() || data.size() > 1024 * 1024)
        return false;
    GUID id{};
    wchar_t unique[40]{};
    if (FAILED(CoCreateGuid(&id)) || !StringFromGUID2(id, unique, 40))
        return false;
    std::wstring temp = path + unique;
    HANDLE f = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
        return false;
    DWORD written = 0;
    bool ok = WriteFile(f, data.data(), static_cast<DWORD>(data.size()),
                        &written, nullptr) &&
              written == data.size() && FlushFileBuffers(f);
    CloseHandle(f);
    if (ok &&
        MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING))
        return true;
    DeleteFileW(temp.c_str());
    return false;
}

static std::wstring PackKey(const std::wstring& slug, PCWSTR field) {
    return Setting((L"Workshop." + slug + L"." + field).c_str());
}

struct Pack {
    std::wstring file;
    std::vector<std::wstring> frames;
    int width{}, height{}, fps{12};
};

static Pack ParsePack(const json::JsonObject& meta) {
    Pack pack;
    auto number = [&](PCWSTR name, int fallback, int low, int high) {
        double value = meta.GetNamedNumber(name, fallback);
        if (!(value >= low && value <= high) || value != std::floor(value))
            throw std::invalid_argument("Invalid pack dimensions or rate");
        return static_cast<int>(value);
    };
    pack.file = meta.GetNamedString(L"file").c_str();
    if (!SafeWorkshopName(pack.file) || !HasSupportedExt(pack.file))
        throw std::invalid_argument("Invalid media name");
    pack.width = number(L"frameWidth", 0, 0, 512);
    pack.height = number(L"frameHeight", 0, 0, 512);
    pack.fps = number(L"fps", 12, 1, 60);
    if (meta.HasKey(L"frames")) {
        auto frames = meta.GetNamedArray(L"frames");
        if (!frames.Size() || frames.Size() > 300)
            throw std::invalid_argument("Invalid sequence length");
        for (auto v : frames) {
            std::wstring name(v.GetString());
            if (!SafeWorkshopName(name) ||
                _wcsicmp(PathFindExtensionW(name.c_str()), L".png"))
                throw std::invalid_argument("Invalid PNG frame");
            pack.frames.push_back(name);
        }
    }
    if ((pack.width == 0) != (pack.height == 0) ||
        (pack.width &&
         (!pack.frames.empty() ||
          _wcsicmp(PathFindExtensionW(pack.file.c_str()), L".png"))) ||
        (!pack.frames.empty() && pack.frames.front() != pack.file))
        throw std::invalid_argument("Invalid animation layout");
    return pack;
}

// ---- Workshop fetch thread ----
// Fetches the Workshop index and fills missing metadata and preview caches.
struct WorkshopFetchResult {
    std::vector<WorkshopItem> items;
    std::wstring error;
};

static DWORD WINAPI WorkshopFetchThread(LPVOID param) {
    auto* res = new WorkshopFetchResult();
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(hr)) {
        res->error = L"Could not initialize the Workshop worker.";
        PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 1, (LPARAM)res);
        return 0;
    }
    struct Apartment {
        ~Apartment() { RoUninitialize(); }
    } apartment;
    std::wstring wsDir = GetWorkshopDir();
    std::string listing = HttpGet(WorkshopIndexUrl());
    auto indexCache = WorkshopChild(wsDir, L"catalog.json");
    auto validIndex = [](const std::string& data) {
        try {
            json::JsonArray a = json::JsonArray::Parse(winrt::to_hstring(data));
            for (auto v : a)
                if (!SafeWorkshopName(std::wstring(v.GetString())))
                    return false;
            return true;
        } catch (...) {
            return false;
        }
    };
    bool online = validIndex(listing);
    if (online)
        WriteSmallFile(indexCache, listing);
    else
        listing = ReadSmallFile(indexCache);
    if (!validIndex(listing)) {
        res->error = L"Workshop unavailable. No cached catalog exists yet.";
        PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 1, (LPARAM)res);
        return 0;
    }

    std::vector<std::wstring> slugs;
    for (auto v : json::JsonArray::Parse(winrt::to_hstring(listing)))
        slugs.emplace_back(v.GetString().c_str());

    if (slugs.empty()) {
        res->error = L"No items found in the workshop yet. Check back later!";
        PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 1, (LPARAM)res);
        return 0;
    }

    // Step 2: for each slug, download meta.json and preview
    for (auto& slug : slugs) {
        WorkshopItem item;
        item.slug = slug;

        // Cache dirs per pet
        std::wstring petDir = WorkshopChild(wsDir, slug);
        if (petDir.empty())
            continue;
        CreateDirectoryW(petDir.c_str(), nullptr);

        // meta.json
        auto metaPath = WorkshopChild(petDir, L"meta.json");
        if (metaPath.empty())
            continue;
        auto data = online ? HttpGet(WorkshopRawUrl(slug, L"meta.json"))
                           : std::string{};
        auto parse = [&](const std::string& bytes) {
            try {
                auto meta = json::JsonObject::Parse(winrt::to_hstring(bytes));
                auto pack = ParsePack(meta);
                item.file = pack.file;
                item.name = meta.GetNamedString(L"name", L"").c_str();
                item.author = meta.GetNamedString(L"author", L"").c_str();
                item.description =
                    meta.GetNamedString(L"description", L"").c_str();
                item.version = meta.GetNamedString(L"version", L"").c_str();
                item.previewFile =
                    meta.GetNamedString(L"preview", L"preview.png").c_str();
                item.tags.clear();
                if (meta.HasKey(L"tags"))
                    for (auto t : meta.GetNamedArray(L"tags"))
                        item.tags.emplace_back(t.GetString().c_str());
                item.metadata = winrt::to_hstring(bytes).c_str();
                return true;
            } catch (...) {
                return false;
            }
        };
        if (parse(data))
            WriteSmallFile(metaPath, data);
        else if (!parse(ReadSmallFile(metaPath)))
            continue;
        if (item.name.empty())
            item.name = slug;
        if (item.previewFile.empty())
            item.previewFile = L"preview.png";

        // preview image
        auto previewName = item.previewFile;
        if (SafeWorkshopName(item.version))
            previewName = L"preview-" + item.version + L"-" + previewName;
        auto prevPath = WorkshopChild(petDir, previewName);
        auto instPath = WorkshopChild(petDir, item.file);
        if (instPath.empty() || prevPath.empty() ||
            !HasSupportedExt(item.file) || !HasSupportedExt(item.previewFile) ||
            !_wcsicmp(PathFindExtensionW(item.previewFile.c_str()), L".mp4"))
            continue;
        if (online &&
            GetFileAttributesW(prevPath.c_str()) == INVALID_FILE_ATTRIBUTES)
            HttpDownloadFile(WorkshopRawUrl(slug, item.previewFile), prevPath);
        if (GetFileAttributesW(prevPath.c_str()) != INVALID_FILE_ATTRIBUTES)
            PutString(PackKey(slug, L"CachedPreview"), previewName);
        else {
            prevPath = WorkshopChild(
                petDir, ReadString(PackKey(slug, L"CachedPreview")));
            if (prevPath.empty())
                prevPath = WorkshopChild(petDir, item.previewFile);
        }
        item.localPreviewPath = prevPath;

        // Mark as installed only if the file exists on disk AND is registered
        // in imagePaths — a file sitting in the cache without an imagePaths
        // entry means the pet isn't actually active in the manager.

        if (GetFileAttributesW(instPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            item.localImagePath = instPath;
        }

        auto anchor = ReadString(PackKey(slug, L"Anchor"));
        if (!anchor.empty())
            item.localImagePath = WorkshopChild(petDir, anchor);
        item.installedVersion = ReadString(PackKey(slug, L"Version"));
        res->items.push_back(std::move(item));
    }

    PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 0, (LPARAM)res);
    return 0;
}

// ---- Workshop install thread ----
struct WorkshopInstallParam {
    int index;
    std::wstring slug;
    std::wstring metadata;
    std::wstring revision;
};

static bool LoadPackFrames(PetItem&, const Pack&, const std::wstring&, size_t&);
static DWORD WINAPI WorkshopInstallThread(LPVOID param) {
    std::unique_ptr<WorkshopInstallParam> p(
        static_cast<WorkshopInstallParam*>(param));
    if (FAILED(RoInitialize(RO_INIT_MULTITHREADED))) {
        PostMessageW(g_hCtrlWnd, WM_WS_INSTALL_DONE, p->index, 1);
        return 0;
    }
    bool ok = false;
    auto root = WorkshopChild(GetWorkshopDir(), p->slug);
    auto dir = WorkshopChild(root, p->revision);
    try {
        auto pack = ParsePack(json::JsonObject::Parse(p->metadata));
        if (dir.empty() || !CreateDirectoryW(dir.c_str(), nullptr))
            throw std::runtime_error("Cannot stage pack");
        std::set<std::wstring> files(pack.frames.begin(), pack.frames.end());
        files.insert(pack.file);
        size_t totalBytes = 0;
        for (const auto& name : files) {
            auto dest = WorkshopChild(dir, name);
            if (dest.empty() ||
                !HttpDownloadFile(WorkshopRawUrl(p->slug, name), dest))
                throw std::runtime_error("Missing pack file");
            std::error_code ec;
            auto size = fs::file_size(dest, ec);
            if (ec || size > 128u * 1024 * 1024 ||
                totalBytes + size > 256u * 1024 * 1024)
                throw std::runtime_error("Pack exceeds download limit");
            totalBytes += static_cast<size_t>(size);
        }
        if (pack.width || !pack.frames.empty()) {
            PetItem test;
            size_t decoded = 0;
            test.sourcePath = WorkshopChild(dir, pack.file);
            if (!LoadPackFrames(test, pack, dir, decoded))
                throw std::runtime_error("Invalid animation");
            for (auto* frame : test.decodedFrames)
                delete frame;
        }
        ok = WriteSmallFile(WorkshopChild(dir, L"package.json"),
                            winrt::to_string(p->metadata));
    } catch (...) {
    }
    if (!ok && !dir.empty()) {
        std::error_code ec;
        fs::remove_all(dir, ec);
    }
    if (g_hCtrlWnd)
        PostMessageW(g_hCtrlWnd, WM_WS_INSTALL_DONE, p->index, ok ? 0 : 1);
    RoUninitialize();
    return 0;
}

static std::vector<HANDLE> workshopWorkers;
static bool StartWorkshopWorker(LPTHREAD_START_ROUTINE proc, void* parameter) {
    for (auto it = workshopWorkers.begin(); it != workshopWorkers.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = workshopWorkers.erase(it);
        } else
            ++it;
    }
    HANDLE worker = CreateThread(nullptr, 0, proc, parameter, 0, nullptr);
    if (!worker)
        return false;
    workshopWorkers.push_back(worker);
    return true;
}
static void Workshop_StartFetch() {
    if (g_workshopFetching)
        return;
    g_workshopFetching = true;
    g_workshopError.clear();
    if (!StartWorkshopWorker(WorkshopFetchThread, nullptr)) {
        g_workshopFetching = false;
        g_workshopFetched = true;
        g_workshopError = L"Could not start the workshop download.";
    }
}

static void Workshop_StartInstall(int idx) {
    if (idx < 0 || idx >= (int)g_workshopItems.size())
        return;
    auto& it = g_workshopItems[idx];
    if (it.installing || g_workshopFetching)
        return;
    it.installing = true;
    GUID id{};
    if (FAILED(CoCreateGuid(&id))) {
        it.installing = false;
        return;
    }
    wchar_t guid[40]{};
    StringFromGUID2(id, guid, 40);
    it.revision = L"rev-" + std::wstring(guid + 1, 36);
    auto* p = new WorkshopInstallParam{idx, it.slug, it.metadata, it.revision};
    if (!StartWorkshopWorker(WorkshopInstallThread, p)) {
        it.installing = false;
        delete p;
        g_workshopError = L"Could not start the installation.";
    }
}

static void SaveImagePaths(
    const std::vector<std::wstring>& paths,
    const std::wstring& removedKey = {});  // defined below

// Remove a pack from imagePaths (and save) without deleting the cached file,
// so re-installing is instant.  Called from the UI thread.
static void Workshop_Uninstall(int idx) {
    if (idx < 0 || idx >= (int)g_workshopItems.size())
        return;
    auto& it = g_workshopItems[idx];
    if (!it.installed || it.installing)
        return;

    // Remove every matching entry from imagePaths
    auto& paths = g_settings.imagePaths;
    paths.erase(std::remove_if(paths.begin(), paths.end(),
                               [&](const std::wstring& p) {
                                   return _wcsicmp(p.c_str(),
                                                   it.localImagePath.c_str()) ==
                                          0;
                               }),
                paths.end());
    SaveImagePaths(paths);

    it.installed = false;
    // Trigger the pet list to reload so the removed pet disappears immediately
    if (g_hManagerWnd)
        PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
}

// Load preview bitmap for an item (scaled to thumbW×thumbH).
static HBITMAP Workshop_LoadPreview(const std::wstring& path,
                                    int thumbW,
                                    int thumbH) {
    if (path.empty() ||
        GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return nullptr;
    Gdiplus::Bitmap* src = Gdiplus::Bitmap::FromFile(path.c_str());
    if (!src || src->GetLastStatus() != Gdiplus::Ok) {
        delete src;
        return nullptr;
    }

    HDC hdc = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, thumbW, thumbH);
    HBITMAP old = (HBITMAP)SelectObject(memDC, hBmp);

    // Fill background
    HBRUSH bg = CreateSolidBrush(RGB(30, 30, 44));
    RECT r = {0, 0, thumbW, thumbH};
    FillRect(memDC, &r, bg);
    DeleteObject(bg);

    // Draw scaled, centred, preserving aspect ratio
    float sw = (float)src->GetWidth(), sh = (float)src->GetHeight();
    float scale = std::min((float)thumbW / sw, (float)thumbH / sh);
    int dw = (int)(sw * scale), dh = (int)(sh * scale);
    int dx = (thumbW - dw) / 2, dy = (thumbH - dh) / 2;

    {
        Gdiplus::Graphics g(memDC);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.DrawImage(src, dx, dy, dw, dh);
    }
    delete src;

    SelectObject(memDC, old);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdc);
    return hBmp;
}

// Returns items matching g_workshopFilter (case-insensitive on
// name/author/tags).
static std::vector<int> Workshop_FilteredIndices() {
    std::vector<int> out;
    std::wistringstream query(g_workshopFilter);
    std::vector<std::wstring> terms;
    for (std::wstring term; query >> term;)
        terms.push_back(term);
    auto contains = [](const std::wstring& value, const std::wstring& term) {
        return StrStrIW(value.c_str(), term.c_str()) != nullptr;
    };
    for (int i = 0; i < (int)g_workshopItems.size(); i++) {
        const auto& item = g_workshopItems[i];
        if ((favoritesOnly && !Number(PackKey(item.slug, L"Favorite"), 0)) ||
            (!workshopTag.empty() &&
             std::find(item.tags.begin(), item.tags.end(), workshopTag) ==
                 item.tags.end()))
            continue;
        bool match =
            std::all_of(terms.begin(), terms.end(), [&](const auto& term) {
                if (term.front() == L'@')
                    return term.size() > 1 &&
                           contains(item.author, term.substr(1));
                if (term.front() == L'#')
                    return term.size() > 1 &&
                           std::any_of(item.tags.begin(), item.tags.end(),
                                       [&](const auto& tag) {
                                           return contains(tag, term.substr(1));
                                       });
                return contains(item.name, term);
            });
        if (match)
            out.push_back(i);
    }
    return out;
}

// ---------------------------------------------------------------------------
// Publish the ordered IDs after saving all new records.
static void SaveImagePaths(const std::vector<std::wstring>& paths,
                           const std::wstring& removedKey) {
    if (!configWritable)
        return;
    std::vector<std::wstring> used, keys;
    if (!removedKey.empty())
        used.push_back(removedKey);
    for (auto& path : paths) {
        auto key = ImageKey(path, used);
        if (key.empty())
            return;
        used.push_back(key);
        keys.push_back(key);
    }
    if (!StoreList(ImageList(), keys))
        return;
    for (auto& key : imageOrder)
        if (std::find(keys.begin(), keys.end(), key) == keys.end())
            DeleteImageRecord(key);
    imageOrder = std::move(keys);
}
static void LoadImagePaths(std::vector<std::wstring>& out) {
    out.clear();
    imageOrder.clear();
    for (auto& key : ReadList(ImageList())) {
        auto path = ReadString(Record(key, L"Path"));
        if (!path.empty()) {
            out.push_back(path);
            imageOrder.push_back(key);
        }
    }
}
static std::vector<UINT> GetFrameDelays(Gdiplus::Image* img, UINT frameCount) {
    std::vector<UINT> delays(frameCount, 100);
    UINT size = img->GetPropertyItemSize(PropertyTagFrameDelay);
    if (size > 0) {
        std::vector<BYTE> buf(size);
        if (img->GetPropertyItem(PropertyTagFrameDelay, size,
                                 (Gdiplus::PropertyItem*)buf.data()) ==
            Gdiplus::Ok) {
            auto* pi = (Gdiplus::PropertyItem*)buf.data();
            auto* vals = (UINT32*)pi->value;
            UINT n = pi->length / sizeof(UINT32);
            for (UINT i = 0; i < frameCount && i < n; i++) {
                UINT ms = vals[i] * 10;
                delays[i] = ms ? ms : 100;
            }
        }
    }
    return delays;
}

// ============================================================================
// Thumbnail helper  (square HBITMAP, drawn directly in the sidebar's owner-draw
// paint)
// ============================================================================

static HBITMAP CreateThumb(PetItem& it, int sz = 96) {
    if (!it.image)
        return nullptr;
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = sz;
    bmi.bmiHeader.biHeight = -sz;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP hOld = (HBITMAP)SelectObject(memDC, hBmp);

    if (bits) {
        // Fill checkerboard background so transparency is visible
        BYTE* px = (BYTE*)bits;
        for (int y = 0; y < sz; y++) {
            for (int x = 0; x < sz; x++) {
                bool checker = ((x / 8) + (y / 8)) % 2 == 0;
                BYTE v = checker ? 0x55 : 0x44;
                int idx = (y * sz + x) * 4;
                px[idx] = v;
                px[idx + 1] = v;
                px[idx + 2] = v;
                px[idx + 3] = 0xFF;
            }
        }

        // Draw first frame of the image scaled into sz×sz
        int iw = it.baseWidth, ih = it.baseHeight;
        float ratio = std::min((float)sz / iw, (float)sz / ih);
        int dw = std::max(1, (int)(iw * ratio));
        int dh = std::max(1, (int)(ih * ratio));
        int ox = (sz - dw) / 2;
        int oy = (sz - dh) / 2;

        Gdiplus::Bitmap bmp(sz, sz, sz * 4, PixelFormat32bppARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&bmp);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.DrawImage(it.image, ox, oy, dw, dh);
    }

    SelectObject(memDC, hOld);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
    return hBmp;
}

// ============================================================================
// Rendering  (unchanged from v1.0)
// ============================================================================

static POINT RecoverPosition(const PetItem& it, POINT position) {
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(MonitorFromPoint(position, MONITOR_DEFAULTTONEAREST),
                         &info))
        return position;
    const RECT& work = info.rcWork;
    const double radians = it.rotationDegrees * 3.141592653589793 / 180;
    const double scale = it.scalePercent / 100.0;
    const double c = fabs(cos(radians)), s = fabs(sin(radians));
    auto extent = [&](double pixels) {
        return std::max(1L, std::lround(scale * pixels)) + (it.shadow ? 8 : 0);
    };
    const LONG w = extent(it.baseWidth * c + it.baseHeight * s);
    const LONG h = extent(it.baseHeight * c + it.baseWidth * s);
    return {
        std::clamp(position.x, work.left, std::max(work.left, work.right - w)),
        std::clamp(position.y, work.top, std::max(work.top, work.bottom - h))};
}

static bool AdvancePlayback(PetItem& it) {
    const UINT last = it.frameCount - 1;
    const UINT start = it.reverse ? last : 0;
    const int step = it.reverse != it.returning ? -1 : 1;
    int next = static_cast<int>(it.currentFrame) + step;
    if (next < 0 || next > static_cast<int>(last)) {
        if (it.pingPong && !it.returning) {
            it.returning = true;
            next = static_cast<int>(it.currentFrame) - step;
        } else {
            if (++it.completed == static_cast<unsigned>(it.loops) && it.loops)
                return false;
            it.returning = false;
            next = it.pingPong ? static_cast<int>(start) - step
                               : static_cast<int>(start);
        }
    }
    it.currentFrame = static_cast<UINT>(next);
    return true;
}

// The render thread owns every cached bitmap. Selection into a DC is temporary.
struct CachedSurface {
    HWND owner{};
    FrameKey key{};
    HBITMAP bitmap{};
    void* pixels{};
    int width{}, height{};
    size_t bytes{};
    ~CachedSurface() {
        if (bitmap)
            DeleteObject(bitmap);
    }
};
static std::list<CachedSurface> frameCache;
static size_t cachedBytes = 0;
static constexpr size_t frameBudget = 64u * 1024 * 1024;
static constexpr size_t maxSurfaces = 128;
static void RenderItem(PetItem& it) {
    if (!it.hwnd || !it.image || !VisibleItem(it) || it.otherDesktop ||
        (SavingAllowed() && (it.covered || sessionLocked)))
        return;

    auto* visual = it.image;
    double scale = (it.scalePercent <= 0 ? 1 : it.scalePercent) / 100.0;
    int dstW = std::max(1, (int)std::lround(it.baseWidth * scale));
    int dstH = std::max(1, (int)std::lround(it.baseHeight * scale));

    double rad = it.rotationDegrees * 3.14159265358979323846 / 180.0;
    double cosA = std::fabs(std::cos(rad));
    double sinA = std::fabs(std::sin(rad));
    int canvasW = std::max(1, (int)std::lround(dstW * cosA + dstH * sinA));
    int canvasH = std::max(1, (int)std::lround(dstW * sinA + dstH * cosA));

    if (it.shadow) {
        canvasW += 8;
        canvasH += 8;
    }
    int alpha = it.hoverFade && it.nearCursor ? 75 : 255;
    FrameKey key{
        static_cast<int>(it.currentFrame),
        canvasW,
        canvasH,
        it.scalePercent,
        it.rotationDegrees,
        it.opacityPercent,
        it.flipX,
        it.flipY,
        it.grayscale,
        it.shadow,
        it.brightness,
        it.hue,
        it.tint,
        it.frameCount > 1};
    if (it.presentedAlpha == alpha && it.presented == key)
        return;
    auto found = std::find_if(
        frameCache.begin(), frameCache.end(), [&](const auto& entry) {
            return entry.owner == it.hwnd && entry.key == key;
        });
    bool hit = found != frameCache.end();
    auto size64 = static_cast<ULONGLONG>(canvasW) * canvasH * 4;
    if (size64 > SIZE_MAX || canvasW > INT_MAX / 4)
        return;
    size_t bytes = static_cast<size_t>(size64);
    CachedSurface temporary;
    CachedSurface* surface = &temporary;
    if (hit) {
        frameCache.splice(frameCache.begin(), frameCache, found);
        surface = &frameCache.front();
    } else if (bytes <= frameBudget) {
        while (cachedBytes + bytes > frameBudget ||
               frameCache.size() >= maxSurfaces) {
            auto& last = frameCache.back();
            cachedBytes -= last.bytes;
            if (!temporary.bitmap && last.width == canvasW &&
                last.height == canvasH) {
                std::swap(temporary.bitmap, last.bitmap);
                temporary.pixels = last.pixels;
            }
            frameCache.pop_back();
        }
        frameCache.emplace_front();
        surface = &frameCache.front();
        std::swap(surface->bitmap, temporary.bitmap);
        surface->pixels = temporary.pixels;
        surface->bytes = bytes;
        surface->width = canvasW;
        surface->height = canvasH;
        cachedBytes += bytes;
    }
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);
    if (!hit && !surface->bitmap) {
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = canvasW;
        bmi.bmiHeader.biHeight = -canvasH;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        surface->bitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS,
                                           &surface->pixels, nullptr, 0);
    }
    auto old = memDC && surface->bitmap ? SelectObject(memDC, surface->bitmap)
                                        : nullptr;
    if (!old || old == HGDI_ERROR) {
        if (!hit && surface != &temporary) {
            cachedBytes -= surface->bytes;
            frameCache.pop_front();
        }
        if (memDC)
            DeleteDC(memDC);
        if (screenDC)
            ReleaseDC(nullptr, screenDC);
        return;
    }
    if (!hit) {
        surface->owner = it.hwnd;
        surface->key = key;
        void* bits = surface->pixels;
        Gdiplus::Bitmap canvasBmp(canvasW, canvasH, canvasW * 4,
                                  PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&canvasBmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(
            it.frameCount > 1
                ? Gdiplus::InterpolationModeBilinear
                : Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.TranslateTransform((Gdiplus::REAL)(canvasW / 2.0),
                             (Gdiplus::REAL)(canvasH / 2.0));
        g.RotateTransform((Gdiplus::REAL)it.rotationDegrees);
        g.ScaleTransform(it.flipX ? -1.f : 1.f, it.flipY ? -1.f : 1.f);
        g.TranslateTransform((Gdiplus::REAL)(-dstW / 2.0),
                             (Gdiplus::REAL)(-dstH / 2.0));

        int op = std::max(10, std::min(100, it.opacityPercent));
        if (it.shadow) {
            Gdiplus::ColorMatrix shade{};
            shade.m[3][3] = 0.35f * op / 100;
            shade.m[4][4] = 1;
            Gdiplus::ImageAttributes shadow;
            shadow.SetColorMatrix(&shade);
            g.DrawImage(visual, Gdiplus::Rect(3, 3, dstW, dstH), 0, 0,
                        (INT)visual->GetWidth(), (INT)visual->GetHeight(),
                        Gdiplus::UnitPixel, &shadow);
            g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        }
        if (op < 100 || it.grayscale || it.brightness != 100 || it.hue ||
            it.tint) {
            Gdiplus::ColorMatrix cm = {
                {{1, 0, 0, 0, 0},
                 {0, 1, 0, 0, 0},
                 {0, 0, 1, 0, 0},
                 {0, 0, 0, (Gdiplus::REAL)(op / 100.0), 0},
                 {0, 0, 0, 0, 1}}};
            const double angle = it.hue * 3.141592653589793 / 180;
            const double c = cos(angle), t = (1 - c) / 3,
                         u = sin(angle) / sqrt(3.0);
            const double rotation[3][3] = {{c + t, t + u, t - u},
                                           {t - u, c + t, t + u},
                                           {t + u, t - u, c + t}};
            const double luminance[] = {0.2126, 0.7152, 0.0722};
            const double warm[] = {1, 0.78, 0.55};
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    double channel =
                        it.grayscale ? luminance[i] : rotation[i][j];
                    cm.m[i][j] =
                        static_cast<float>(channel * it.brightness / 100 *
                                           (1 + (warm[j] - 1) * it.tint / 100));
                }
            }
            Gdiplus::ImageAttributes attr;
            attr.SetColorMatrix(&cm);
            Gdiplus::Rect dr(0, 0, dstW, dstH);
            g.DrawImage(visual, dr, 0, 0, (Gdiplus::REAL)visual->GetWidth(),
                        (Gdiplus::REAL)visual->GetHeight(), Gdiplus::UnitPixel,
                        &attr);
        } else {
            g.DrawImage(visual, 0, 0, dstW, dstH);
        }
    }

    RECT wr{};
    GetWindowRect(it.hwnd, &wr);
    if (wr.right - wr.left != canvasW || wr.bottom - wr.top != canvasH)
        SetWindowPos(it.hwnd, nullptr, 0, 0, canvasW, canvasH,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    POINT ptSrc = {0, 0};
    POINT ptDst = {wr.left, wr.top};
    SIZE sizeWnd = {canvasW, canvasH};
    BLENDFUNCTION blend = {
        AC_SRC_OVER, 0,
        static_cast<BYTE>(it.hoverFade && it.nearCursor ? 75 : 255),
        AC_SRC_ALPHA};
    if (UpdateLayeredWindow(it.hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc,
                            0, &blend, ULW_ALPHA)) {
        it.presented = key;
        it.presentedAlpha = alpha;
    }
    SelectObject(memDC, old);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

static void ApplyTopmost(PetItem& it) {
    SetWindowPos(it.hwnd, it.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0,
                 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void ApplyClickThrough(PetItem& it) {
    LONG_PTR ex = GetWindowLongPtrW(it.hwnd, GWL_EXSTYLE);
    if (it.clickThrough)
        ex |= WS_EX_TRANSPARENT;
    else
        ex &= ~WS_EX_TRANSPARENT;
    SetWindowLongPtrW(it.hwnd, GWL_EXSTYLE, ex);
}

// ============================================================================
// Manager window  — helpers to read/write controls
// ============================================================================

// Index of the pet currently shown in the right panel (-1 = none)
static int g_managerSel = -1;
// Window icon for the manager (destroyed with the window)
static HICON g_hManagerIcon = nullptr;
enum class ManagerTheme { Dark = 0, Light = 1, System = 2 };
static ManagerTheme g_managerTheme = ManagerTheme::Dark;
static COLORREF g_accentChoices[5] = {
    RGB(0x7E, 0x57, 0xC2),  // purple (default)
    RGB(0x5B, 0xA8, 0xE0),  // blue
    RGB(0x4C, 0xC0, 0x8A),  // green
    RGB(0xE0, 0x6B, 0x6B),  // red
    RGB(0xE0, 0xA6, 0x4C),  // orange
};
static int g_accentIndex = 0;
// Index 5 = "use system accent color" (not in g_accentChoices[] array)
static const int kAccentIndexSystem = 5;

// Read the Windows Dwm/UxTheme accent color from the registry.
static COLORREF GetSystemAccentColor() {
    DWORD color = 0xFF6200EE, size = sizeof(color);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AccentColor", nullptr, nullptr, (LPBYTE)&color,
                         &size);
        RegCloseKey(hKey);
    }
    // DWM stores it as ABGR; convert to COLORREF (BGR in low 24 bits → RGB
    // swap).
    BYTE r = (BYTE)(color & 0xFF);
    BYTE g2 = (BYTE)((color >> 8) & 0xFF);
    BYTE b = (BYTE)((color >> 16) & 0xFF);
    return RGB(r, g2, b);
}

// Manager open hotkey (default: Win+Alt+P).
// g_hotkeyMods uses the RegisterHotKey MOD_* flags; 0 means "disabled".
static UINT g_hotkeyMods = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
static UINT g_hotkeyVk = 'P';
static bool g_hotkeyCapturing =
    false;  // true while waiting for the user to press a new combo
static HHOOK g_hKeyHook =
    nullptr;  // low-level keyboard hook active only during capture
static bool g_winKeyDown =
    false;  // tracked inside LLKeyProc; GetAsyncKeyState is unreliable for
            // modifier keys in LL hooks
static bool g_ctrlKeyDown = false;  // same — VK_CONTROL/VK_LCONTROL/VK_RCONTROL
static unsigned g_altKeysDown = 0;  // bit 0: left Alt, bit 1: right Alt
static bool g_shiftKeyDown = false;  // same — VK_SHIFT/VK_LSHIFT/VK_RSHIFT

// ----------------------------------------------------------------------------
// Hotkey helpers
// ----------------------------------------------------------------------------

// Build a human-readable string like "Win + Alt + P" from mods + vk.
static std::wstring HotkeyToString(UINT mods, UINT vk) {
    if (mods == 0 && vk == 0)
        return L"(none)";
    std::wstring s;
    if (mods & MOD_WIN)
        s += L"Win + ";
    if (mods & MOD_CONTROL)
        s += L"Ctrl + ";
    if (mods & MOD_ALT)
        s += L"Alt + ";
    if (mods & MOD_SHIFT)
        s += L"Shift + ";
    // MapVirtualKeyW → scan code, then GetKeyNameTextW for a locale-aware name.
    // Do NOT pass the extended-key bit for regular keys (letters, digits,
    // F-keys) — only extended keys (numpad /, arrows, ins/del/home/end etc.)
    // need it. Also mask off any top bits; the lParam format wants bits 16-23 =
    // scan code, bit 24 = extended, bits 25-31 = 0.
    UINT sc = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    LONG lParam = (LONG)(sc << 16);
    // Mark extended bit for keys that need it so GetKeyNameTextW names them
    // correctly.
    switch (vk) {
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_DIVIDE:
        case VK_NUMLOCK:
        case VK_RCONTROL:
        case VK_RMENU:
            lParam |= (1 << 24);  // extended key bit
            break;
    }
    wchar_t buf[64] = {};
    // GetKeyNameTextW can return "Alt" for letter scan codes when the system
    // keyboard state still has Alt down (e.g. immediately after capture).
    // Guard: if the result is "Alt" but vk is not VK_MENU, discard it.
    bool nameOk = sc && GetKeyNameTextW(lParam, buf, 64) && buf[0] &&
                  !(wcscmp(buf, L"Alt") == 0 && vk != VK_MENU) &&
                  !(wcscmp(buf, L"Ctrl") == 0 && vk != VK_CONTROL) &&
                  !(wcscmp(buf, L"Shift") == 0 && vk != VK_SHIFT);
    if (nameOk) {
        s += buf;
    } else {
        // Reliable fallbacks in priority order:
        // 1. Letters A-Z and digits 0-9 — just use the VK directly.
        // 2. VkKeyScan printable result for other VKs.
        // 3. Hex as last resort.
        if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) {
            s += (wchar_t)vk;
        } else if (vk >= VK_F1 && vk <= VK_F24) {
            wchar_t tmp[8];
            swprintf_s(tmp, L"F%u", vk - VK_F1 + 1);
            s += tmp;
        } else if (vk >= 0x20 && vk <= 0x7E) {
            s += (wchar_t)vk;
        } else {
            wchar_t tmp[8];
            swprintf_s(tmp, L"0x%02X", vk);
            s += tmp;
        }
    }
    return s;
}

// Keep the shortcut on the controller so it also works after the manager
// closes.
static std::wstring hotkeyNotice;

static bool WindowsShortcut(UINT mods, UINT vk) {
    mods &= MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    if (vk == VK_F12)
        return true;
    if (mods == MOD_WIN) {
        if ((vk >= '0' && vk <= '9') ||
            (vk >= 'A' && vk <= 'Z' &&
             wcschr(L"ABCDEFGHIJKLMNOPQRSTUVWXZ", vk)))
            return true;
        return vk == VK_TAB || vk == VK_SPACE || vk == VK_ESCAPE ||
               vk == VK_HOME || vk == VK_PAUSE || vk == VK_SNAPSHOT ||
               (vk >= VK_LEFT && vk <= VK_DOWN);
    }
    if (mods == (MOD_WIN | MOD_SHIFT))
        return vk == 'S' || vk == 'R' || vk == 'M' ||
               (vk >= '0' && vk <= '9') || (vk >= VK_LEFT && vk <= VK_DOWN);
    if (mods == (MOD_WIN | MOD_CONTROL))
        return vk == 'D' || vk == VK_F4 || vk == VK_LEFT || vk == VK_RIGHT;
    if (mods == MOD_ALT)
        return vk == VK_TAB || vk == VK_ESCAPE || vk == VK_F4 || vk == VK_SPACE;
    return (mods == MOD_CONTROL && vk == VK_ESCAPE) ||
           (mods == (MOD_CONTROL | MOD_SHIFT) && vk == VK_ESCAPE) ||
           (mods == (MOD_CONTROL | MOD_ALT) && vk == VK_DELETE);
}

static void ReapplyHotkey() {
    if (g_hCtrlWnd) {
        UnregisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER);
        if (g_hotkeyVk && (WindowsShortcut(g_hotkeyMods, g_hotkeyVk) ||
                           !RegisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER,
                                           g_hotkeyMods, g_hotkeyVk))) {
            hotkeyNotice = L"Saved shortcut unavailable. Choose another.";
        } else {
            hotkeyNotice.clear();
        }
    }
}

// Store both halves of the shortcut in a single registry write.
static void SaveHotkey() {
    DWORD keys[] = {g_hotkeyMods, g_hotkeyVk};
    if (configWritable)
        Wh_SetBinaryValue(Setting(L"ManagerHotkey").c_str(), keys,
                          sizeof(keys));
}
static void LoadHotkey() {
    DWORD keys[2]{}, bytes = sizeof(keys);
    if (configWritable && Wh_GetBinaryValue(Setting(L"ManagerHotkey").c_str(),
                                            keys, bytes) == sizeof(keys)) {
        if (keys[1] && !(keys[0] & (MOD_ALT | MOD_CONTROL | MOD_WIN)) &&
            (keys[1] < VK_F1 || keys[1] > VK_F24))
            return;
        g_hotkeyMods = keys[0];
        g_hotkeyVk = keys[1];
    }
}

// Install / remove the low-level keyboard hook used during capture.
static void StartKeyCapture();  // forward declaration
static void StopKeyCapture();

// Low-level keyboard hook — active only while g_hotkeyCapturing is true.
// Sees every key before the shell or RegisterHotKey intercepts it, so
// Win+[letter], Win+Ctrl+[letter], etc. all arrive here.
static LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION || !g_hotkeyCapturing ||
        GetForegroundWindow() != g_hManagerWnd)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    auto* ks = (KBDLLHOOKSTRUCT*)lParam;
    UINT vk = ks->vkCode;
    bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

    // ----------------------------------------------------------------
    // Track every modifier ourselves — GetAsyncKeyState is unreliable
    // inside a low-level hook because the kernel hasn't updated the
    // async key-state table yet when the hook fires.
    // ----------------------------------------------------------------
    switch (vk) {
        case VK_LWIN:
        case VK_RWIN:
            g_winKeyDown = down;
            // Always swallow Win events during capture so Start Menu never
            // opens.
            if (g_hotkeyCapturing)
                return 1;
            return CallNextHookEx(nullptr, nCode, wParam, lParam);

        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            g_ctrlKeyDown = down;
            if (!g_hotkeyCapturing)
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            return 1;  // swallow modifier-only events during capture

        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            g_shiftKeyDown = down;
            if (!g_hotkeyCapturing)
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            return 1;

        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU: {
            unsigned side = (vk == VK_RMENU ||
                             (vk == VK_MENU && (ks->flags & LLKHF_EXTENDED)))
                                ? 2u
                                : 1u;
            if (down)
                g_altKeysDown |= side;
            else
                g_altKeysDown &= ~side;
            if (!g_hotkeyCapturing)
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            return 1;
        }
    }

    // From here: a non-modifier key event.
    if (!g_hotkeyCapturing || !down)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    // Escape cancels capture.
    if (vk == VK_ESCAPE) {
        g_hotkeyCapturing = false;
        StopKeyCapture();
        ReapplyHotkey();
        if (g_hManagerWnd)
            PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
        return 1;
    }

    // Commit — use only hook-tracked state, never GetAsyncKeyState.
    UINT mods = MOD_NOREPEAT;
    if (g_altKeysDown)
        mods |= MOD_ALT;
    if (g_ctrlKeyDown)
        mods |= MOD_CONTROL;
    if (g_shiftKeyDown)
        mods |= MOD_SHIFT;
    if (g_winKeyDown)
        mods |= MOD_WIN;

    // Reject bare letters and digits with no real modifier — they would eat
    // normal typing whenever a window is focused.
    bool hasRealMod = (mods & (MOD_WIN | MOD_ALT | MOD_CONTROL)) != 0;
    if (!hasRealMod && (vk < VK_F1 || vk > VK_F24)) {
        // Stay in capture mode; update display to tell the user.
        if (g_hManagerWnd)
            PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
        return 1;  // swallow but don't commit
    }

    if (WindowsShortcut(mods, vk) ||
        !RegisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER, mods, vk)) {
        hotkeyNotice =
            HotkeyToString(mods, vk) +
            L" is unavailable. Try another shortcut, or press Escape.";
        PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
        return 1;
    }

    hotkeyNotice.clear();
    g_hotkeyCapturing = false;
    g_hotkeyMods = mods;
    g_hotkeyVk = vk;
    StopKeyCapture();
    SaveHotkey();
    if (g_hManagerWnd)
        PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
    return 1;  // swallow the key
}

static void StartKeyCapture() {
    if (!g_hKeyHook) {
        g_altKeysDown = (GetAsyncKeyState(VK_LMENU) & 0x8000 ? 1u : 0u) |
                        (GetAsyncKeyState(VK_RMENU) & 0x8000 ? 2u : 0u);
        g_hKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, LLKeyProc, nullptr, 0);
    }
}

static void StopKeyCapture() {
    if (g_hKeyHook) {
        UnhookWindowsHookEx(g_hKeyHook);
        g_hKeyHook = nullptr;
    }
    // Reset all tracked modifier state — meaningless outside the hook.
    g_altKeysDown = 0;
    g_winKeyDown = g_ctrlKeyDown = g_shiftKeyDown = false;
}
static bool SystemPrefersLightTheme() {
    DWORD value = 1,
          size = sizeof(value);  // default: dark (AppsUseLightTheme=0)
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\P"
                      L"ersonalize",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
                         (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value != 0;
}

static void RunImageMenuAction(int idx, int cmd) {
    auto& pet = g_items[idx];

    switch (cmd) {
        case kPetMenuHide:
            pet.hidden = true;
            if (pet.hwnd)
                ShowWindow(pet.hwnd, SW_HIDE);
            break;
        case kPetMenuShow:
            pet.hidden = false;
            if (VisibleItem(pet) && pet.hwnd)
                ShowWindow(pet.hwnd, SW_SHOWNOACTIVATE);
            break;
        case kPetMenuBringToFront:
            if (pet.hwnd)
                SetWindowPos(pet.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;
        case kPetMenuResetPos:
            pet.posX = 80;
            pet.posY = 80;
            if (pet.hwnd)
                SetWindowPos(pet.hwnd, nullptr, pet.posX, pet.posY, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            ForgetStoredPosition(pet.registryKey);
            break;
        case kPetMenuClone: {
            if (!configWritable)
                break;
            auto paths = g_settings.imagePaths;
            paths.push_back(pet.filePath);
            SaveImagePaths(paths);
            if (imageOrder.size() != paths.size())
                break;
            g_settings.imagePaths = std::move(paths);
            PetItem copy = pet;
            copy.registryKey = imageOrder.back();
            SyncImageState(copy, false);
            SaveStoredPosition(copy.registryKey, pet.posX + 24, pet.posY + 24);
            PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
            break;
        }
        case kPetMenuReload:
            // Force re-decode of this image only by triggering a full reload.
            PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
            break;
        case kPetMenuDelete:
            // Remove from explicit path list and reload
            if (!pet.folderKey.empty()) {
                DWORD removed = 1;
                if (configWritable && !pet.registryKey.empty())
                    PutNumber(Record(pet.registryKey, L"Removed"), removed);
            } else {
                auto& paths = g_settings.imagePaths;
                auto key = std::find(imageOrder.begin(), imageOrder.end(),
                                     pet.registryKey);
                size_t index = static_cast<size_t>(key - imageOrder.begin());
                if (key != imageOrder.end() && index < paths.size())
                    paths.erase(paths.begin() + index);
                else if (auto found = std::find(paths.begin(), paths.end(),
                                                pet.filePath);
                         found != paths.end())
                    paths.erase(found);
                SaveImagePaths(paths, pet.registryKey);
            }
            PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
            break;
    }
}
static void ShowAllHidden();
namespace xaml = winrt::Windows::UI::Xaml;
namespace controls = xaml::Controls;
namespace hosting = xaml::Hosting;
using controls::Button;
using controls::StackPanel;
using controls::TextBlock;

// Created and released on the engine's STA, which also owns the pet collection.
static hosting::WindowsXamlManager managerRuntime{nullptr};
static hosting::DesktopWindowXamlSource managerSource{nullptr};
static winrt::com_ptr<IDesktopWindowXamlSourceNative2> managerInput;
static HWND islandWindow{};
static controls::Grid managerRoot{nullptr};
enum class Page { Pet, Workshop, App };
static Page page = Page::Pet;
static bool updatingList = false;

template <class T>
static T Named(const wchar_t* name) {
    return managerRoot.FindName(name).as<T>();
}
static TextBlock Label(const std::wstring& text, double size = 14) {
    TextBlock label;
    label.Text(text);
    label.FontSize(size);
    label.TextWrapping(xaml::TextWrapping::Wrap);
    return label;
}
static auto Brush(COLORREF color) {
    return xaml::Media::SolidColorBrush(
        {255, GetRValue(color), GetGValue(color), GetBValue(color)});
}
static bool LightManager() {
    return g_managerTheme == ManagerTheme::Light ||
           (g_managerTheme == ManagerTheme::System &&
            SystemPrefersLightTheme());
}
static auto ManagerAccent() {
    return managerRoot.Resources()
        .Lookup(winrt::box_value(L"AnimaAccent"))
        .as<xaml::Media::SolidColorBrush>();
}
// The shared marker lives above the list and footer, in sidebar coordinates.
struct NavigationMotion {
    xaml::FrameworkElement anchor{nullptr};
    double x{}, y{}, height{16}, startX{}, startY{}, startHeight{}, targetX{},
        targetY{};
    ULONGLONG started{};
    bool visible{};
};
static NavigationMotion navMotion;
static constexpr UINT_PTR kNavTimer = 91;

static void DrawNavPill() {
    auto transform = Named<xaml::Media::CompositeTransform>(L"NavTransform");
    transform.TranslateX(navMotion.x);
    transform.TranslateY(navMotion.y - navMotion.height / 2);
    transform.ScaleY(navMotion.height / 16);
}
static void StepNavPill(HWND hwnd) {
    auto& m = navMotion;
    if (!m.started)
        return;
    double t = std::min(1.0, (GetTickCount64() - m.started) / 260.0);
    double ease = 1 - std::pow(1 - t, 3);
    double wave = std::sin(3.141592653589793 * t);
    double distance = m.targetY - m.startY;
    m.x = m.startX + (m.targetX - m.startX) * ease;
    m.y = m.startY + distance * ease +
          std::clamp(distance * 0.06, -5.0, 5.0) *
              std::sin(2 * 3.141592653589793 * t) * (-t);
    m.height = 16 + (m.startHeight - 16) * (1 - ease) +
               std::min(22.0, std::abs(distance) * 0.28) * wave * wave;
    DrawNavPill();
    if (t >= 1) {
        KillTimer(hwnd, kNavTimer);
        m.started = 0;
    }
}
static void PositionNavPill(HWND hwnd) {
    if (!managerRoot || updatingList)
        return;
    auto list = Named<controls::ListView>(L"Pets");
    auto overlay = Named<controls::Canvas>(L"NavOverlay");
    xaml::FrameworkElement anchor{nullptr};
    if (page == Page::Pet) {
        auto item = list.SelectedItem();
        if (item)
            anchor = item.try_as<controls::ListViewItem>();
    } else
        anchor = Named<Button>(page == Page::App ? L"App" : L"Workshop");
    auto pill = Named<controls::Border>(L"NavPill");
    bool visible =
        anchor && anchor.IsLoaded() && anchor.ActualHeight() > 0 &&
        (page != Page::Pet || Named<controls::Grid>(L"MediaPane").Visibility() == xaml::Visibility::Visible);
    winrt::Windows::Foundation::Point pt{};
    if (visible) {
        pt = anchor.TransformToVisual(overlay).TransformPoint(
            {page == Page::Pet ? 4.f : 3.f,
             (float)(anchor.ActualHeight() / 2)});
        if (page == Page::Pet) {
            auto top = list.TransformToVisual(overlay).TransformPoint({0, 0}).Y;
            visible = pt.Y - 8 >= top && pt.Y + 8 <= top + list.ActualHeight();
        }
    }
    auto& m = navMotion;
    if (!visible) {
        if (m.visible)
            pill.Opacity(0);
        KillTimer(hwnd, kNavTimer);
        m = NavigationMotion{};
        return;
    }
    if (m.anchor == anchor && m.targetX == pt.X && m.targetY == pt.Y)
        return;
    BOOL animate = TRUE;
    bool move = m.visible && m.anchor != anchor;
    m.anchor = anchor;
    m.targetX = pt.X;
    m.targetY = pt.Y;
    if (move &&
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animate, 0) &&
        animate && SetTimer(hwnd, kNavTimer, 16, nullptr)) {
        m.startX = m.x;
        m.startY = m.y;
        m.startHeight = m.height;
        m.started = GetTickCount64();
    } else {
        KillTimer(hwnd, kNavTimer);
        m.started = 0;
        m.x = pt.X;
        m.y = pt.Y;
        m.height = 16;
        DrawNavPill();
    }
    if (!m.visible)
        pill.Opacity(1);
    m.visible = true;
}
static void UpdateNavigation() {
    const Page pages[] = {Page::Workshop, Page::App};
    const wchar_t* names[] = {L"Workshop", L"App"};
    for (int i = 0; i < 2; ++i) {
        auto button = Named<Button>(names[i]);
        bool selected = pages[i] == page;
        button.Background(xaml::Media::SolidColorBrush(
            selected ? (LightManager()
                            ? winrt::Windows::UI::Color{16, 0, 0, 0}
                            : winrt::Windows::UI::Color{20, 255, 255, 255})
                     : winrt::Windows::UI::Color{0, 0, 0, 0}));
    }
    if (g_hManagerWnd)
        PositionNavPill(g_hManagerWnd);
}
static constexpr UINT kApplyManagerAppearance = WM_APP + 32;
static int appliedTheme = -1;
static COLORREF appliedAccent = CLR_INVALID;
static bool StyleManager(HWND hwnd) {
    BOOL dark = !LightManager();
    COLORREF colorValue = g_accentIndex == kAccentIndexSystem
                              ? GetSystemAccentColor()
                              : g_accentChoices[g_accentIndex];
    if (appliedTheme == dark && appliedAccent == colorValue)
        return false;
    bool themeChanged = appliedTheme != dark;
    appliedTheme = dark;
    appliedAccent = colorValue;
    if (themeChanged) {
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
        DWORD corners = 2;
        DwmSetWindowAttribute(hwnd, 33, &corners, sizeof(corners));
        COLORREF caption = dark ? RGB(30, 30, 30) : RGB(243, 243, 243);
        DwmSetWindowAttribute(hwnd, 35, &caption, sizeof(caption));
        managerRoot.RequestedTheme(dark ? xaml::ElementTheme::Dark
                                        : xaml::ElementTheme::Light);
        managerRoot.Background(
            Brush(dark ? RGB(30, 30, 30) : RGB(243, 243, 243)));
    }
    auto accent = ManagerAccent();
    accent.Color(Brush(colorValue).Color());
    UpdateNavigation();
    auto primary = Named<Button>(L"Import");
    primary.Background(accent);
    auto importOptions = Named<Button>(L"ImportOptions");
    importOptions.Background(accent);
    auto color = accent.Color();
    primary.Foreground(
        Brush((299 * color.R + 587 * color.G + 114 * color.B >= 148000)
                  ? RGB(0, 0, 0)
                  : RGB(255, 255, 255)));
    importOptions.Foreground(primary.Foreground());
    return true;
}
static StackPanel Group(StackPanel parent, const wchar_t* title) {
    controls::Border card;
    if (*title) {
        auto heading = Label(title, 14);
        heading.FontWeight({600});
        heading.Margin({2, 8, 0, -4});
        parent.Children().Append(heading);
    }
    card.Padding(*title ? xaml::Thickness{14, 2, 14, 2}
                        : xaml::Thickness{14, 12, 14, 12});
    card.BorderThickness({1, 1, 1, 1});
    card.BorderBrush(
        Brush(LightManager() ? RGB(222, 222, 222) : RGB(56, 56, 56)));
    card.CornerRadius({8, 8, 8, 8});
    card.Background(
        Brush(LightManager() ? RGB(252, 252, 252) : RGB(44, 44, 44)));
    StackPanel body;
    body.Spacing(*title ? 0 : 8);
    card.Child(body);
    parent.Children().Append(card);
    return body;
}
template <class F>
static Button Action(StackPanel parent, const wchar_t* title, F run) {
    Button button;
    button.Content(winrt::box_value(winrt::hstring(title)));
    button.Click([run](auto const&, auto const&) { run(); });
    parent.Children().Append(button);
    return button;
}
static bool ManagerAnimationsEnabled();
static void SyncChoiceState(controls::Primitives::ToggleButton const& button) {
    auto sync = [](auto const& sender, bool animate) {
        auto control = sender.template as<controls::Primitives::ToggleButton>();
        auto checked = control.IsChecked();
        xaml::VisualStateManager::GoToState(
            control, checked && checked.Value() ? L"AnimaOn" : L"AnimaOff",
            animate && control.IsLoaded() && ManagerAnimationsEnabled());
    };
    button.Loaded([sync](auto const& sender, auto const&) { sync(sender, false); });
    auto changed = [sync](auto const& sender, auto const&) { sync(sender, true); };
    button.Checked(changed);
    button.Unchecked(changed);
}
static controls::Grid FieldRow(StackPanel parent) {
    if (parent.Children().Size()) {
        controls::Border divider;
        divider.Height(1);
        divider.Background(
            Brush(LightManager() ? RGB(230, 230, 230) : RGB(56, 56, 56)));
        parent.Children().Append(divider);
    }
    controls::Grid grid;
    grid.MinHeight(40);
    grid.Padding({2, 4, 2, 4});
    controls::ColumnDefinition left, right;
    right.Width({1, xaml::GridUnitType::Auto});
    grid.ColumnDefinitions().Append(left);
    grid.ColumnDefinitions().Append(right);
    grid.ColumnSpacing(12);
    parent.Children().Append(grid);
    return grid;
}
static controls::Grid SettingRow(StackPanel parent,
                                 const wchar_t* title,
                                 const wchar_t* description = nullptr) {
    auto row = FieldRow(parent);
    StackPanel text;
    text.VerticalAlignment(xaml::VerticalAlignment::Center);
    text.Children().Append(Label(title));
    if (description) {
        auto note = Label(description, 12);
        note.Opacity(0.70);
        text.Children().Append(note);
    }
    row.Children().Append(text);
    return row;
}
template <class F>
static auto Toggle(StackPanel parent, const wchar_t* title, bool on, F run) {
    auto row = SettingRow(parent, title);
    controls::Primitives::ToggleButton toggle;
    toggle.Template(managerRoot.Resources()
                        .Lookup(winrt::box_value(L"PillSwitch"))
                        .as<controls::ControlTemplate>());
    toggle.UseSystemFocusVisuals(true);
    toggle.Background(ManagerAccent());
    SyncChoiceState(toggle);
    toggle.IsChecked(on);
    xaml::Automation::AutomationProperties::SetName(toggle, title);
    toggle.Checked([run](auto&&, auto&&) { run(true); });
    toggle.Unchecked([run](auto&&, auto&&) { run(false); });
    controls::Grid::SetColumn(toggle, 1);
    row.Children().Append(toggle);
    return toggle;
}
template <class F>
static void Choice(StackPanel parent,
                   const wchar_t* title,
                   std::initializer_list<const wchar_t*> options,
                   int selected,
                   F run) {
    auto row = SettingRow(parent, title);
    controls::Grid choices;
    choices.ColumnSpacing(4);
    choices.RowSpacing(4);
    for (int i = 0; i < 3; ++i)
        choices.ColumnDefinitions().Append(controls::ColumnDefinition());
    auto style = managerRoot.Resources()
                     .Lookup(winrt::box_value(L"RadioPill"))
                     .as<controls::ControlTemplate>();
    int i = 0;
    for (auto option : options) {
        if (i % 3 == 0)
            choices.RowDefinitions().Append(controls::RowDefinition());
        controls::RadioButton radio;
        radio.GroupName(title);
        radio.Template(style);
        radio.Background(ManagerAccent());
        radio.MinWidth(0);
        radio.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
        radio.UseSystemFocusVisuals(true);
        radio.Content(winrt::box_value(winrt::hstring(option)));
        SyncChoiceState(radio);
        radio.IsChecked(i == selected);
        controls::Grid::SetRow(radio, i / 3);
        controls::Grid::SetColumn(radio, i % 3);
        choices.Children().Append(radio);
        radio.Checked([run, i](auto&&, auto&&) { run(i); });
        ++i;
    }
    controls::Grid::SetColumn(choices, 1);
    row.Children().Append(choices);
}
static bool ManagerAnimationsEnabled() {
    BOOL enabled = FALSE;
    return SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0) &&
           enabled;
}

static void FadeTabFocus(const winrt::Windows::Foundation::IInspectable&,
                         const xaml::RoutedEventArgs& args) {
    if (!managerRoot || !ManagerAnimationsEnabled())
        return;
    auto target = args.OriginalSource().try_as<controls::Control>();
    if (!target || target.FocusState() != xaml::FocusState::Keyboard)
        return;
    using xaml::Media::Animation::Storyboard;
    auto fade = managerRoot.Resources()
                    .Lookup(winrt::box_value(L"TabFade"))
                    .as<Storyboard>();
    fade.Stop();
    Storyboard::SetTarget(fade.Children().GetAt(0), target);
    fade.Begin();
}

static controls::Flyout DropdownFlyout(
    controls::Primitives::FlyoutPlacementMode placement) {
    controls::Flyout menu;
    menu.Placement(placement);
    menu.FlyoutPresenterStyle(managerRoot.Resources()
                                  .Lookup(winrt::box_value(L"OptionMenu"))
                                  .as<xaml::Style>());
    return menu;
}

static void StyleDropdownOption(Button option) {
    option.Template(managerRoot.Resources()
                        .Lookup(winrt::box_value(L"ActionFace"))
                        .as<controls::ControlTemplate>());
    option.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
    option.HorizontalContentAlignment(xaml::HorizontalAlignment::Left);
    option.Padding({4, 5, 4, 5});
    option.BorderThickness({0, 0, 0, 0});
    option.CornerRadius({4, 4, 4, 4});
}

static controls::Flyout ActionDropdown(
    controls::Primitives::FlyoutPlacementMode placement) {
    auto menu = DropdownFlyout(placement);
    StackPanel list;
    if (placement ==
            controls::Primitives::FlyoutPlacementMode::TopEdgeAlignedRight &&
        ManagerAnimationsEnabled()) {
        xaml::Media::Animation::EntranceThemeTransition entrance;
        entrance.FromHorizontalOffset(0);
        entrance.FromVerticalOffset(12);
        xaml::Media::Animation::TransitionCollection transitions;
        transitions.Append(entrance);
        list.Transitions(transitions);
    }
    list.MinWidth(144);
    list.Spacing(2);
    menu.Opening([list](auto&&, auto&&) {
        if (managerRoot)
            list.RequestedTheme(managerRoot.RequestedTheme());
    });
    list.GotFocus(FadeTabFocus);
    menu.Content(list);
    return menu;
}

template <class F>
static void AddDropdownAction(controls::Flyout menu,
                              PCWSTR caption,
                              F run,
                              bool enabled = true) {
    Button option;
    StyleDropdownOption(option);
    option.Padding({10, 5, 10, 5});
    option.Background(xaml::Media::SolidColorBrush({0, 0, 0, 0}));
    option.Content(winrt::box_value(winrt::hstring(caption)));
    option.IsEnabled(enabled);
    option.Click([weak = winrt::make_weak(menu), run](auto&&, auto&&) {
        if (auto flyout = weak.get())
            flyout.Hide();
        run();
    });
    menu.Content().as<StackPanel>().Children().Append(option);
}

static void DropdownDivider(controls::Flyout menu) {
    controls::Border divider;
    divider.Height(1);
    divider.Margin({6, 2, 6, 2});
    divider.Background(
        Brush(LightManager() ? RGB(210, 210, 210) : RGB(65, 65, 65)));
    menu.Content().as<StackPanel>().Children().Append(divider);
}

// A field-width flyout uses the same selection marker as the sidebar.
template <class F>
static void Dropdown(StackPanel parent,
                     const wchar_t* title,
                     const wchar_t* description,
                     const std::vector<std::wstring>& values,
                     int selected,
                     F run,
                     bool appearance = true) {
    auto row = SettingRow(parent, title, description);
    Button field;
    field.Width(156);
    field.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
    controls::Grid face;
    auto value = Label(values.at(selected));
    value.Margin({0, 0, 20, 0});
    controls::FontIcon arrow;
    arrow.Glyph(L"\uE70D");
    arrow.FontSize(12);
    arrow.HorizontalAlignment(xaml::HorizontalAlignment::Right);
    face.Children().Append(value);
    face.Children().Append(arrow);
    field.Content(face);
    xaml::Automation::AutomationProperties::SetName(
        field, std::wstring(title) + L", " + values[selected]);
    controls::Grid::SetColumn(field, 1);
    row.Children().Append(field);

    auto menu = DropdownFlyout(
        controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    auto weakMenu = winrt::make_weak(menu);
    auto current = std::make_shared<int>(selected);
    auto changed = std::make_shared<bool>(false);
    menu.Closed([changed, appearance](auto&&, auto&&) {
        if (*changed && g_hManagerWnd)
            PostMessageW(g_hManagerWnd,
                         appearance ? kApplyManagerAppearance : WM_APP + 42, 1,
                         0);
        *changed = false;
    });
    StackPanel list;
    list.RequestedTheme(managerRoot.RequestedTheme());
    list.GotFocus(FadeTabFocus);
    list.Width(field.Width() - 10);
    for (int i = 0; i < (int)values.size(); ++i) {
        Button option;
        StyleDropdownOption(option);
        option.Background(xaml::Media::SolidColorBrush(
            LightManager()
                ? winrt::Windows::UI::Color{static_cast<BYTE>(
                                                i == *current ? 16 : 0),
                                            0, 0, 0}
                : winrt::Windows::UI::Color{
                      static_cast<BYTE>(i == *current ? 20 : 0), 255, 255,
                      255}));
        StackPanel line;
        line.Orientation(controls::Orientation::Horizontal);
        line.Spacing(8);
        controls::Border mark;
        mark.Width(3);
        mark.Height(16);
        mark.CornerRadius({1.5, 1.5, 1.5, 1.5});
        mark.Background(ManagerAccent());
        mark.Opacity(i == *current ? 1 : 0);
        line.Children().Append(mark);
        line.Children().Append(Label(values[i]));
        option.Content(line);
        xaml::Automation::AutomationProperties::SetName(option, values[i]);
        option.Click([weakMenu, current, i, run, changed](auto&&, auto&&) {
            auto menu = weakMenu.get();
            if (*current != i) {
                *current = i;
                *changed = true;
                run(i);
            }
            if (menu)
                menu.Hide();
        });
        list.Children().Append(option);
    }
    controls::ScrollViewer choices;
    choices.MaxHeight(320);
    choices.VerticalScrollBarVisibility(controls::ScrollBarVisibility::Auto);
    choices.Content(list);
    menu.Content(choices);
    field.Flyout(menu);
}
static void ReloadItems() {
    PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
}
static void AddImages() {
    wchar_t files[32768]{};
    OPENFILENAMEW picker{sizeof(picker)};
    picker.hwndOwner = g_hManagerWnd;
    picker.lpstrFilter =
        L"Supported "
        L"media\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.webp;*.apng;*.mp4\0MP4 video "
        L"(experimental)\0*.mp4\0All files\0*.*\0";
    picker.lpstrFile = files;
    picker.nMaxFile = std::size(files);
    picker.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER |
                   OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&picker))
        return;
    std::wstring first = files;
    const wchar_t* next = files + first.size() + 1;
    if (!*next)
        g_settings.imagePaths.push_back(first);
    else
        for (; *next; next += wcslen(next) + 1)
            g_settings.imagePaths.push_back(first + L"\\" + next);
    SaveImagePaths(g_settings.imagePaths);
    ReloadItems();
}
static LRESULT CALLBACK ForwardDroppedMedia(HWND hwnd,
                                            UINT message,
                                            WPARAM wp,
                                            LPARAM lp,
                                            UINT_PTR id,
                                            DWORD_PTR owner) {
    if (message == WM_DROPFILES)
        return SendMessageW(reinterpret_cast<HWND>(owner), message, wp, lp);
    if (message == WM_NCDESTROY)
        RemoveWindowSubclass(hwnd, ForwardDroppedMedia, id);
    return DefSubclassProc(hwnd, message, wp, lp);
}

static void ImportDroppedMedia(HDROP drop) {
    auto paths = g_settings.imagePaths;
    const UINT count = DragQueryFileW(drop, 0xffffffff, nullptr, 0);
    for (UINT i = 0; i < count; ++i) {
        UINT length = DragQueryFileW(drop, i, nullptr, 0);
        std::wstring path(length + 1, L'\0');
        if (!length ||
            DragQueryFileW(drop, i, path.data(), length + 1) != length)
            continue;
        path.resize(length);
        DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY) && HasSupportedExt(path))
            paths.push_back(std::move(path));
    }
    DragFinish(drop);
    if (configWritable && paths.size() != g_settings.imagePaths.size()) {
        SaveImagePaths(paths);
        if (imageOrder.size() == paths.size()) {
            g_settings.imagePaths = std::move(paths);
            ReloadItems();
        }
    }
}

static controls::Image Thumbnail(HBITMAP handle, int width, int height) {
    controls::Image image;
    image.Width(width);
    image.Height(height);
    BITMAP bm{};
    if (handle && GetObjectW(handle, sizeof(bm), &bm)) {
        xaml::Media::Imaging::WriteableBitmap bitmap(bm.bmWidth, bm.bmHeight);
        BITMAPINFO info{};
        info.bmiHeader = {
            sizeof(BITMAPINFOHEADER), bm.bmWidth, -bm.bmHeight, 1, 32, BI_RGB};
        auto buffer = bitmap.PixelBuffer();
        BYTE* bytes{};
        winrt::check_hresult(
            buffer.as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(
                &bytes));
        buffer.Length((uint32_t)(bm.bmWidth * bm.bmHeight * 4));
        HDC dc = GetDC(nullptr);
        int lines =
            GetDIBits(dc, handle, 0, bm.bmHeight, bytes, &info, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (lines == bm.bmHeight) {
            for (unsigned i = 3; i < buffer.Length(); i += 4)
                bytes[i] = 255;
            bitmap.Invalidate();
            image.Source(bitmap);
        }
    }
    return image;
}
static void RenderPage();
static Button OverflowButton(const std::wstring& name) {
    Button more;
    more.Content(winrt::box_value(L"⋮"));
    more.Padding({5, 0, 5, 0});
    more.Width(28);
    more.Background(xaml::Media::SolidColorBrush({0, 0, 0, 0}));
    xaml::Automation::AutomationProperties::SetName(more,
                                                    L"Actions for " + name);
    return more;
}
static void PetList(int changedFolder = -1) {
    auto list = Named<controls::ListView>(L"Pets");
    auto query = Named<controls::TextBox>(L"Search").Text();
    updatingList = true;
    auto rows = list.Items();
    unsigned insertAt = 0;
    if (changedFolder < 0)
        rows.Clear();
    auto appendItem = [&](int i) {
        auto& pet = g_items[i];
        if (!query.empty() && !StrStrIW(pet.fileName.c_str(), query.c_str()))
            return;
        controls::ListViewItem row;
        row.Template(managerRoot.Resources()
                         .Lookup(winrt::box_value(L"PetRowFace"))
                         .as<controls::ControlTemplate>());
        row.UseSystemFocusVisuals(true);
        row.Tag(winrt::box_value(i));
        controls::Grid content;
        for (auto unit : {xaml::GridUnitType::Auto, xaml::GridUnitType::Auto,
                          xaml::GridUnitType::Star, xaml::GridUnitType::Auto}) {
            controls::ColumnDefinition column;
            column.Width({1, unit});
            content.ColumnDefinitions().Append(column);
        }
        content.ColumnDefinitions().GetAt(0).Width(
            {3, xaml::GridUnitType::Pixel});
        content.ColumnSpacing(4);
        row.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
        if (!pet.hThumb)
            pet.hThumb = CreateThumb(pet);
        auto thumb = Thumbnail(pet.hThumb, 36, 36);
        controls::Grid::SetColumn(thumb, 1);
        content.Children().Append(thumb);
        auto name = Label(pet.fileName);
        name.VerticalAlignment(xaml::VerticalAlignment::Center);
        name.TextWrapping(xaml::TextWrapping::NoWrap);
        name.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        controls::Grid::SetColumn(name, 2);
        content.Children().Append(name);
        Button more = OverflowButton(pet.fileName);
        more.Click([i, key = pet.registryKey, path = pet.filePath](
                       auto const& sender, auto const&) {
            auto current = [i, key, path] {
                return i < (int)g_items.size() &&
                       g_items[i].registryKey == key &&
                       g_items[i].filePath == path;
            };
            if (!current())
                return;
            auto menu =
                ActionDropdown(controls::Primitives::FlyoutPlacementMode::
                                   BottomEdgeAlignedLeft);
            auto add = [&](PCWSTR caption, int command, bool enabled = true) {
                AddDropdownAction(
                    menu, caption,
                    [i, current, command] {
                        if (current())
                            RunImageMenuAction(i, command);
                    },
                    enabled);
            };
            const auto& media = g_items[i];
            add(media.hidden ? L"Show" : L"Hide",
                media.hidden ? kPetMenuShow : kPetMenuHide);
            add(L"Bring to front", kPetMenuBringToFront);
            DropdownDivider(menu);
            add(L"Reset position", kPetMenuResetPos);
            add(L"Clone", kPetMenuClone, configWritable);
            add(L"Reload image", kPetMenuReload);
            DropdownDivider(menu);
            add(L"Remove from list", kPetMenuDelete,
                media.folderKey.empty() || configWritable);
            menu.ShowAt(sender.template as<Button>());
        });
        controls::Grid::SetColumn(more, 3);
        content.Children().Append(more);
        if (!pet.folderKey.empty())
            row.Margin({12, 1, 0, 1});
        row.Content(content);
        if (changedFolder >= 0)
            rows.InsertAt(insertAt++, row);
        else
            rows.Append(row);
        if (page == Page::Pet && i == g_managerSel)
            list.SelectedItem(row);
    };
    if (changedFolder >= 0) {
        // Keep the other realized rows in place so XAML can animate just the
        // inserted and removed children, including movement of following rows.
        if (changedFolder < static_cast<int>(libraryFolders.size())) {
            const auto& folder = libraryFolders[changedFolder];
            for (unsigned pos = 0; pos < rows.Size(); ++pos) {
                auto header = rows.GetAt(pos).as<controls::ListViewItem>();
                if (winrt::unbox_value<int>(header.Tag()) != -1 - changedFolder)
                    continue;
                bool expanded = folder.expanded || !query.empty();
                auto line = header.Content().as<controls::Grid>();
                line.Children().GetAt(0).as<controls::FontIcon>().Glyph(
                    expanded ? L"\uE70D" : L"\uE76C");
                insertAt = pos + 1;
                auto isChild = [&] {
                    if (insertAt >= rows.Size())
                        return false;
                    int tag = winrt::unbox_value<int>(
                        rows.GetAt(insertAt).as<controls::ListViewItem>().Tag());
                    return tag >= 0 && tag < static_cast<int>(g_items.size()) &&
                           g_items[tag].folderKey == folder.key;
                };
                list.SelectedIndex(-1);
                if (!expanded) {
                    while (isChild())
                        rows.RemoveAt(insertAt);
                } else if (!isChild()) {
                    for (int i = 0; i < static_cast<int>(g_items.size()); ++i)
                        if (g_items[i].folderKey == folder.key)
                            appendItem(i);
                }
                // Restore the active media row if it is still visible.
                if (page == Page::Pet && g_managerSel >= 0)
                    for (unsigned i = 0; i < rows.Size(); ++i)
                        if (winrt::unbox_value<int>(rows.GetAt(i).as<controls::ListViewItem>().Tag()) == g_managerSel) {
                            list.SelectedIndex(static_cast<int>(i));
                            break;
                        }
                break;
            }
        }
        updatingList = false;
        return;
    }
    for (int i = 0; i < (int)g_items.size(); ++i)
        if (g_items[i].folderKey.empty())
            appendItem(i);
    for (int n = 0; n < (int)libraryFolders.size(); ++n) {
        const auto& folder = libraryFolders[n];
        if (std::none_of(g_items.begin(), g_items.end(),
                         [&](const PetItem& item) {
                             return item.folderKey == folder.key;
                         }))
            continue;
        controls::ListViewItem header;
        header.Tag(winrt::box_value(-1 - n));
        header.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
        controls::Grid line;
        for (auto unit : {xaml::GridUnitType::Auto, xaml::GridUnitType::Auto,
                          xaml::GridUnitType::Star, xaml::GridUnitType::Auto}) {
            controls::ColumnDefinition column;
            column.Width({1, unit});
            line.ColumnDefinitions().Append(column);
        }
        line.ColumnSpacing(6);
        controls::FontIcon arrow, icon;
        bool expanded = folder.expanded || !query.empty();
        arrow.Glyph(expanded ? L"\uE70D" : L"\uE76C");
        arrow.FontSize(10);
        icon.Glyph(L"\uE8B7");
        icon.FontSize(16);
        line.Children().Append(arrow);
        controls::Grid::SetColumn(icon, 1);
        line.Children().Append(icon);
        auto name = GetFileNameFromPath(folder.path);
        if (name.empty())
            name = folder.path;
        auto caption = Label(name);
        caption.TextWrapping(xaml::TextWrapping::NoWrap);
        caption.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        caption.VerticalAlignment(xaml::VerticalAlignment::Center);
        controls::Grid::SetColumn(caption, 2);
        line.Children().Append(caption);
        Button more = OverflowButton(name);
        auto menu = ActionDropdown(
            controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
        AddDropdownAction(
            menu, L"Remove from list",
            [key = folder.key] {
                if (RemoveLibraryFolder(key))
                    ReloadItems();
            },
            configWritable);
        more.Flyout(menu);
        controls::Grid::SetColumn(more, 3);
        line.Children().Append(more);
        header.Content(line);
        controls::ToolTipService::SetToolTip(
            header, winrt::box_value(winrt::hstring(folder.path)));
        list.Items().Append(header);
        if (expanded)
            for (int i = 0; i < (int)g_items.size(); ++i)
                if (g_items[i].folderKey == folder.key)
                    appendItem(i);
    }
    updatingList = false;
}
static PetItem* SelectedPet() {
    return g_managerSel >= 0 && g_managerSel < (int)g_items.size()
               ? &g_items[g_managerSel]
               : nullptr;
}
static controls::Grid SliderFace(controls::Slider const& slider) {
    controls::Grid host, face;
    host.Height(36);
    face.IsHitTestVisible(false);
    face.Margin({10, 0, 10, 0});
    controls::Border rail, fill, knob, dot;
    for (auto part : {rail, fill}) {
        part.Height(4);
        part.CornerRadius({2, 2, 2, 2});
        part.VerticalAlignment(xaml::VerticalAlignment::Center);
        face.Children().Append(part);
    }
    rail.Background(
        Brush(LightManager() ? RGB(206, 206, 206) : RGB(94, 94, 94)));
    fill.Background(ManagerAccent());
    fill.HorizontalAlignment(xaml::HorizontalAlignment::Left);
    knob.Width(20);
    knob.Height(20);
    knob.Margin({-10, 0, -10, 0});
    knob.CornerRadius({10, 10, 10, 10});
    knob.HorizontalAlignment(xaml::HorizontalAlignment::Left);
    knob.VerticalAlignment(xaml::VerticalAlignment::Center);
    knob.Background(
        Brush(LightManager() ? RGB(252, 252, 252) : RGB(67, 67, 67)));
    knob.BorderBrush(rail.Background());
    knob.BorderThickness({1, 1, 1, 1});
    dot.Width(10);
    dot.Height(10);
    dot.HorizontalAlignment(xaml::HorizontalAlignment::Center);
    dot.VerticalAlignment(xaml::VerticalAlignment::Center);
    dot.Background(ManagerAccent());
    dot.CornerRadius({7, 7, 7, 7});
    knob.Child(dot);
    xaml::Media::TranslateTransform position;
    knob.RenderTransform(position);
    face.Children().Append(knob);
    slider.Opacity(0);
    auto weakSlider = winrt::make_weak(slider);
    auto weakFace = winrt::make_weak(face);
    auto update = [weakSlider, weakFace, position, fill] {
        auto s = weakSlider.get();
        auto f = weakFace.get();
        if (!s || !f || s.Maximum() <= s.Minimum())
            return;
        double x = f.ActualWidth() * (s.Value() - s.Minimum()) /
                   (s.Maximum() - s.Minimum());
        fill.Width(x);
        position.X(x);
    };
    face.SizeChanged([update](auto&&, auto&&) { update(); });
    slider.ValueChanged([update](auto&&, auto&&) { update(); });
    slider.PointerEntered([dot](auto&&, auto&&) {
        dot.Width(14);
        dot.Height(14);
    });
    slider.PointerExited([dot](auto&&, auto&&) {
        dot.Width(10);
        dot.Height(10);
    });
    slider.GotFocus([knob](auto&&, auto&&) {
        knob.BorderBrush(ManagerAccent());
        knob.BorderThickness({2, 2, 2, 2});
    });
    slider.LostFocus([knob, rail](auto&&, auto&&) {
        knob.BorderBrush(rail.Background());
        knob.BorderThickness({1, 1, 1, 1});
    });
    host.Children().Append(slider);
    host.Children().Append(face);
    return host;
}
static int MonitorRefreshRate(HWND hwnd) {
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    DEVMODEW mode{};
    mode.dmSize = sizeof(mode);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                        &info) &&
        EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
        mode.dmDisplayFrequency >= 5)
        return (int)mode.dmDisplayFrequency;
    return 60;
}
namespace workshopExport {
struct Draft {
    fs::path source, destination;
    std::vector<BYTE> preview;
    std::wstring author, description, version = L"1.0", tags, frames;
    Pack animation;
    bool details = false;
};

enum Field {
    Paste = 100,
    Browse,
    Author,
    Description,
    Version,
    Tags,
    Mode,
    Width,
    Height,
    Frames,
    Fps
};

static std::wstring Trim(std::wstring text) {
    auto first = text.find_first_not_of(L" \r\n\t");
    if (first == text.npos)
        return {};
    return text.substr(first, text.find_last_not_of(L" \r\n\t") - first + 1);
}

static std::wstring FieldText(HWND dlg, int id) {
    HWND edit = GetDlgItem(dlg, id);
    std::wstring text(GetWindowTextLengthW(edit) + 1, L'\0');
    text.resize(
        GetWindowTextW(edit, text.data(), static_cast<int>(text.size())));
    return Trim(text);
}

static std::vector<std::wstring> Split(const std::wstring& text,
                                       wchar_t separator) {
    std::wistringstream input(text);
    std::vector<std::wstring> result;
    std::wstring part;
    while (std::getline(input, part, separator))
        if (auto value = Trim(part); !value.empty())
            result.push_back(value);
    return result;
}

static std::vector<BYTE> ReadPreview(const fs::path& path) {
    if (_wcsicmp(path.extension().c_str(), L".png"))
        return {};
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    auto size = in.tellg();
    if (size <= 0 || size > 256 * 1024 * 1024)
        return {};
    std::vector<BYTE> bytes(static_cast<size_t>(size));
    in.seekg(0);
    if (!in.read(reinterpret_cast<char*>(bytes.data()), size))
        return {};
    return bytes;
}

static bool IsPng(const std::vector<BYTE>& bytes) {
    constexpr BYTE signature[]{137, 80, 78, 71, 13, 10, 26, 10};
    if (bytes.size() < sizeof(signature) ||
        memcmp(bytes.data(), signature, sizeof(signature)))
        return false;
    winrt::com_ptr<IStream> stream;
    stream.attach(
        SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size())));
    if (!stream)
        return false;
    Gdiplus::Bitmap bitmap(stream.get());
    GUID format{};
    return bitmap.GetRawFormat(&format) == Gdiplus::Ok &&
           IsEqualGUID(format, Gdiplus::ImageFormatPNG) && bitmap.GetWidth() &&
           bitmap.GetHeight();
}

static fs::path DroppedPng(HDROP drop) {
    if (DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0) != 1)
        return {};
    std::wstring path(DragQueryFileW(drop, 0, nullptr, 0) + 1, L'\0');
    path.resize(
        DragQueryFileW(drop, 0, path.data(), static_cast<UINT>(path.size())));
    return path;
}

static std::vector<BYTE> ClipboardPng(HWND dlg) {
    std::vector<BYTE> bytes;
    fs::path path;
    if (!OpenClipboard(dlg))
        return bytes;
    struct Close {
        ~Close() { CloseClipboard(); }
    } close;
    for (auto format : {L"PNG", L"image/png"}) {
        HANDLE data = GetClipboardData(RegisterClipboardFormatW(format));
        SIZE_T size = data ? GlobalSize(data) : 0;
        if (size && size <= 256 * 1024 * 1024) {
            bytes.resize(size);
            if (auto p = GlobalLock(data)) {
                memcpy(bytes.data(), p, size);
                GlobalUnlock(data);
                return bytes;
            }
            bytes.clear();
        }
    }
    if (auto drop = GetClipboardData(CF_HDROP))
        path = DroppedPng(static_cast<HDROP>(drop));
    return path.empty() ? bytes : ReadPreview(path);
}

static void AcceptPreview(HWND dlg, Draft& draft, std::vector<BYTE> bytes) {
    if (!IsPng(bytes)) {
        MessageBoxW(dlg,
                    L"Choose a PNG file or paste an image copied as PNG. "
                    L"If your image uses another format, convert it to PNG and "
                    L"try again.",
                    L"PNG preview required", MB_ICONWARNING);
        return;
    }
    draft.preview = std::move(bytes);
    EndDialog(dlg, IDOK);
}

static int Number(HWND dlg, int id, int maximum, int fallback = 0) {
    auto text = FieldText(dlg, id);
    if (text.empty() && fallback)
        return fallback;
    if (text.empty() || text.find_first_not_of(L"0123456789") != text.npos)
        throw std::wstring(L"Enter a whole number from 1 to ") +
            std::to_wstring(maximum) + L".";
    long n = wcstol(text.c_str(), nullptr, 10);
    if (n < 1 || n > maximum)
        throw std::wstring(L"Enter a whole number from 1 to ") +
            std::to_wstring(maximum) + L".";
    return static_cast<int>(n);
}

static void Save(HWND dlg, Draft& draft) {
    auto tags = Split(FieldText(dlg, Tags), L',');
    auto author = FieldText(dlg, Author),
         description = FieldText(dlg, Description),
         version = FieldText(dlg, Version);
    if (author.empty() || description.empty() || version.empty() ||
        tags.empty())
        throw std::wstring(
            L"Enter an author, description, version, and at least one tag.");
    if (fs::exists(draft.destination))
        throw std::wstring(
            L"This export folder already exists. Move or rename it and try "
            L"again.\n\n") +
            draft.destination.wstring();

    json::JsonObject meta;
    auto put = [&](PCWSTR key, const std::wstring& value) {
        meta.SetNamedValue(key, json::JsonValue::CreateStringValue(value));
    };
    auto putNumber = [&](PCWSTR key, int value) {
        meta.SetNamedValue(key, json::JsonValue::CreateNumberValue(value));
    };
    put(L"name", draft.source.stem().wstring());
    put(L"author", author);
    put(L"description", description);
    put(L"version", version);
    put(L"preview", L"preview.png");
    json::JsonArray tagList;
    for (auto& tag : tags)
        tagList.Append(json::JsonValue::CreateStringValue(tag));
    meta.SetNamedValue(L"tags", tagList);

    auto filename = draft.source.filename().wstring();
    if (!SafeWorkshopName(filename) ||
        !_wcsicmp(filename.c_str(), L"preview.png"))
        filename = L"media" + draft.source.extension().wstring();
    put(L"file", filename);
    int mode =
        static_cast<int>(SendDlgItemMessageW(dlg, Mode, CB_GETCURSEL, 0, 0));
    std::vector<std::pair<fs::path, std::wstring>> assets{
        {draft.source, filename}};
    if (mode && _wcsicmp(draft.source.extension().c_str(), L".png"))
        throw std::wstring(
            L"Select a PNG item to export a sprite sheet or PNG sequence.");
    if (mode)
        putNumber(L"fps", Number(dlg, Fps, 60, 12));
    if (mode == 1) {
        putNumber(L"frameWidth", Number(dlg, Width, 512));
        putNumber(L"frameHeight", Number(dlg, Height, 512));
    } else if (mode == 2) {
        auto names = Split(FieldText(dlg, Frames), L';');
        if (names.empty() || names.size() > 300 ||
            names.front() != draft.source.filename().wstring())
            throw std::wstring(
                L"List 1–300 PNG filenames separated by semicolons. Start with "
                L"the selected item's exact filename.");
        json::JsonArray sequence;
        assets.clear();
        for (const auto& name : names) {
            fs::path path(name);
            if (path.filename() != path || path.has_root_path() ||
                name.find_first_of(L"/\\:") != name.npos ||
                _wcsicmp(path.extension().c_str(), L".png"))
                throw std::wstring(
                    L"Each frame must be a PNG filename beside the selected "
                    L"item.");
            auto out = L"frame-" + std::to_wstring(assets.size() + 1) + L".png";
            assets.emplace_back(draft.source.parent_path() / path, out);
            sequence.Append(json::JsonValue::CreateStringValue(out));
        }
        put(L"file", assets.front().second);
        meta.SetNamedValue(L"frames", sequence);
    }
    auto pack = ParsePack(meta);
    fs::create_directories(draft.destination.parent_path());
    GUID guid{};
    wchar_t suffix[40]{};
    if (FAILED(CoCreateGuid(&guid)) || !StringFromGUID2(guid, suffix, 40))
        throw std::wstring(L"Could not create an export folder.");
    auto staging = draft.destination.parent_path() / suffix;
    if (!fs::create_directory(staging))
        throw std::wstring(L"Could not create an export folder.");
    try {
        uintmax_t total = 0;
        for (auto& [source, out] : assets) {
            auto size = fs::file_size(source);
            if (size > 128u * 1024 * 1024 ||
                (total += size) > 256u * 1024 * 1024)
                throw std::wstring(
                    L"The media exceeds the Workshop download limit.");
            fs::copy_file(source, staging / out);
        }
        if (mode) {
            PetItem test;
            test.sourcePath = (staging / pack.file).wstring();
            size_t decoded = 0;
            if (!LoadPackFrames(test, pack, staging.wstring(), decoded))
                throw std::wstring(
                    L"Check tile dimensions or frame sizes. Frames must match, "
                    L"fit within 512 × 512, and meet the Workshop frame and "
                    L"memory limits.");
            for (auto* frame : test.decodedFrames)
                delete frame;
        }
        std::ofstream preview(staging / L"preview.png", std::ios::binary);
        preview.write(reinterpret_cast<const char*>(draft.preview.data()),
                      draft.preview.size());
        preview.close();
        if (!preview || !WriteSmallFile((staging / L"meta.json").wstring(),
                                        winrt::to_string(meta.Stringify())))
            throw std::wstring(L"Could not write the preview or metadata.");
        if (!MoveFileW(staging.c_str(), draft.destination.c_str()))
            throw std::wstring(
                L"Could not publish the export folder. Check its location and "
                L"permissions.");
    } catch (...) {
        std::error_code ec;
        fs::remove_all(staging, ec);
        throw;
    }
}

static HWND Control(HWND dlg,
                    PCWSTR cls,
                    PCWSTR text,
                    int id,
                    int x,
                    int y,
                    int w,
                    int h,
                    DWORD style) {
    RECT r{x, y, x + w, y + h};
    MapDialogRect(dlg, &r);
    HWND child =
        CreateWindowExW(!wcscmp(cls, L"EDIT") ? WS_EX_CLIENTEDGE : 0, cls, text,
                        WS_CHILD | WS_VISIBLE | style, r.left, r.top,
                        r.right - r.left, r.bottom - r.top, dlg,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                        GetModuleHandleW(nullptr), nullptr);
    if (!child)
        throw std::wstring(L"Could not create the export controls.");
    SendMessageW(child, WM_SETFONT, SendMessageW(dlg, WM_GETFONT, 0, 0), TRUE);
    return child;
}

static LRESULT CALLBACK
PasteKey(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR) {
    if (msg == WM_KEYDOWN && wp == 'V' && GetKeyState(VK_CONTROL) < 0) {
        PostMessageW(GetParent(hwnd), WM_COMMAND, Paste, 0);
        return 0;
    }
    if (msg == WM_NCDESTROY)
        RemoveWindowSubclass(hwnd, PasteKey, id);
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void UpdateFields(HWND dlg) {
    auto mode = SendDlgItemMessageW(dlg, Mode, CB_GETCURSEL, 0, 0);
    EnableWindow(GetDlgItem(dlg, Width), mode == 1);
    EnableWindow(GetDlgItem(dlg, Height), mode == 1);
    EnableWindow(GetDlgItem(dlg, Frames), mode == 2);
    EnableWindow(GetDlgItem(dlg, Fps), mode != 0);
}

static void BuildDialog(HWND dlg, Draft& draft) {
    SetWindowTextW(dlg, draft.details ? L"Workshop pack details"
                                      : L"Choose a Workshop preview");
    auto label = [&](PCWSTR text, int y, int w, int h) {
        Control(dlg, L"STATIC", text, -1, 10, y, w, h, 0);
    };
    auto button = [&](PCWSTR text, int id, int x, int y, int w) {
        auto hwnd = Control(
            dlg, L"BUTTON", text, id, x, y, w, 16,
            WS_TABSTOP | (id == IDOK ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON));
        if (!draft.details)
            SetWindowSubclass(hwnd, PasteKey, 1, 0);
    };
    if (!draft.details) {
        DragAcceptFiles(dlg, TRUE);
        label(
            L"Drop one PNG file here, paste a PNG from the clipboard (Ctrl+V), "
            L"or browse for a file.",
            12, 277, 33);
        button(L"Paste PNG", Paste, 10, 52, 82);
        button(L"Browse…", Browse, 98, 52, 82);
        button(L"Cancel", IDCANCEL, 204, 52, 78);
        return;
    }
    auto field = [&](PCWSTR title, int id, int y, std::wstring value) {
        label(title, y + 2, 102, 14);
        Control(dlg, L"EDIT", value.c_str(), id, 116, y, 200, 14,
                ES_AUTOHSCROLL | WS_TABSTOP);
    };
    field(L"Author *", Author, 10, draft.author);
    field(L"Description *", Description, 29, draft.description);
    field(L"Version *", Version, 48, draft.version);
    field(L"Tags * (comma-separated)", Tags, 67, draft.tags);
    label(L"Animation format", 89, 100, 14);
    auto combo = Control(dlg, L"COMBOBOX", L"", Mode, 116, 86, 200, 70,
                         WS_TABSTOP | CBS_DROPDOWNLIST);
    for (auto title : {L"Original media", L"PNG sprite sheet", L"PNG sequence"})
        SendMessageW(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(title));
    SendMessageW(combo, CB_SETCURSEL,
                 draft.animation.width             ? 1
                 : !draft.animation.frames.empty() ? 2
                                                   : 0,
                 0);
    auto dimension = [](int n) {
        return n ? std::to_wstring(n) : std::wstring{};
    };
    field(L"Tile width (px)", Width, 106, dimension(draft.animation.width));
    field(L"Tile height (px)", Height, 125, dimension(draft.animation.height));
    field(L"Frames (in order)", Frames, 144, draft.frames);
    field(L"Frames per second", Fps, 163, std::to_wstring(draft.animation.fps));
    label(
        L"For a sequence, enter PNG filenames from the same folder, separated "
        L"by semicolons. Start with the selected image.",
        185, 306, 27);
    button(L"Export", IDOK, 149, 215, 80);
    button(L"Cancel", IDCANCEL, 236, 215, 80);
    UpdateFields(dlg);
}

static INT_PTR CALLBACK Procedure(HWND dlg, UINT msg, WPARAM wp, LPARAM lp) {
    auto* draft = reinterpret_cast<Draft*>(GetWindowLongPtrW(dlg, DWLP_USER));
    try {
        if (msg == WM_INITDIALOG) {
            draft = reinterpret_cast<Draft*>(lp);
            SetWindowLongPtrW(dlg, DWLP_USER, lp);
            BuildDialog(dlg, *draft);
            return TRUE;
        }
        if (!draft)
            return FALSE;
        if (!draft->details && msg == WM_DROPFILES) {
            auto drop = reinterpret_cast<HDROP>(wp);
            struct Finish {
                HDROP drop;
                ~Finish() { DragFinish(drop); }
            } finish{drop};
            AcceptPreview(dlg, *draft, ReadPreview(DroppedPng(drop)));
            return TRUE;
        }
        if (msg == WM_CLOSE || (msg == WM_COMMAND && LOWORD(wp) == IDCANCEL)) {
            EndDialog(dlg, IDCANCEL);
            return TRUE;
        }
        if (msg != WM_COMMAND)
            return FALSE;
        if (!draft->details && LOWORD(wp) == Paste)
            AcceptPreview(dlg, *draft, ClipboardPng(dlg));
        else if (!draft->details && LOWORD(wp) == Browse) {
            wchar_t path[32768]{};
            OPENFILENAMEW pick{sizeof(pick)};
            pick.hwndOwner = dlg;
            pick.lpstrFilter = L"PNG image\0*.png\0All files\0*.*\0";
            pick.lpstrFile = path;
            pick.nMaxFile = std::size(path);
            pick.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameW(&pick))
                AcceptPreview(dlg, *draft, ReadPreview(path));
        } else if (draft->details && LOWORD(wp) == Mode &&
                   HIWORD(wp) == CBN_SELCHANGE)
            UpdateFields(dlg);
        else if (draft->details && LOWORD(wp) == IDOK) {
            Save(dlg, *draft);
            EndDialog(dlg, IDOK);
        } else
            return FALSE;
        return TRUE;
    } catch (const std::wstring& reason) {
        MessageBoxW(dlg, reason.c_str(), L"Workshop export", MB_ICONWARNING);
    } catch (...) {
        MessageBoxW(dlg,
                    L"Could not complete this step. Check that all named files "
                    L"exist and that the output folder is writable.",
                    L"Workshop export", MB_ICONWARNING);
    }
    if (msg == WM_INITDIALOG)
        EndDialog(dlg, IDCANCEL);
    return TRUE;
}

static bool ShowStep(Draft& draft) {
    // Variable-length fields follow the DLGTEMPLATE header on WORD boundaries.
    alignas(DWORD) WORD words[96]{};
    auto* dialog = reinterpret_cast<DLGTEMPLATE*>(words);
    dialog->style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME |
                    DS_CENTER | DS_SETFONT;
    dialog->cx = draft.details ? 326 : 292;
    dialog->cy = draft.details ? 241 : 80;
    WORD* font = reinterpret_cast<WORD*>(dialog + 1) + 3;
    *font++ = 9;
    memcpy(font, L"Segoe UI", sizeof(L"Segoe UI"));
    auto result = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), dialog,
                                          g_hManagerWnd, Procedure,
                                          reinterpret_cast<LPARAM>(&draft));
    if (result == -1)
        throw std::wstring(L"Could not open the export dialog.");
    return result == IDOK;
}

static void Run() {
    auto* item = SelectedPet();
    if (!item)
        return;
    try {
        Draft draft;
        draft.source = item->sourcePath;
        if (!fs::is_regular_file(draft.source) ||
            !HasSupportedExt(draft.source.wstring()))
            throw std::wstring(L"The selected media file is unavailable.");
        PWSTR local = nullptr;
        winrt::check_hresult(
            SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local));
        fs::path root(local);
        CoTaskMemFree(local);
        draft.destination = root / L"AnimaWindhawkEngine" / L"Export Packs" /
                            draft.source.stem() / L"pets";
        wchar_t user[257]{};
        DWORD count = std::size(user);
        if (GetUserNameW(user, &count))
            draft.author = user;
        if (item->sourcePath != item->filePath) {
            auto metadata = ReadSmallFile(
                (draft.source.parent_path() / L"package.json").wstring());
            if (!metadata.empty()) {
                auto meta =
                    json::JsonObject::Parse(winrt::to_hstring(metadata));
                draft.animation = ParsePack(meta);
                draft.description =
                    meta.GetNamedString(L"description", L"").c_str();
                draft.version = meta.GetNamedString(L"version", L"1.0").c_str();
                if (meta.HasKey(L"tags"))
                    for (auto tag : meta.GetNamedArray(L"tags")) {
                        if (!draft.tags.empty())
                            draft.tags += L", ";
                        draft.tags += tag.GetString().c_str();
                    }
            }
        }
        for (const auto& name : draft.animation.frames) {
            if (!draft.frames.empty())
                draft.frames += L"; ";
            draft.frames += name;
        }
        if (draft.frames.empty())
            draft.frames = draft.source.filename().wstring();
        if (!ShowStep(draft))
            return;
        draft.details = true;
        if (!ShowStep(draft))
            return;
        auto message = L"Workshop pack exported to:\n" +
                       draft.destination.wstring() +
                       L"\n\n1. Fork tubeo5866/anima-windhawk-engine on GitHub."
                       L"\n2. Choose a unique folder name, such as "
                       L"example-pet-name. Upload the contents of this export "
                       L"to workshop/pets/example-pet-name/ in your fork."
                       L"\n3. Add that same folder name to workshop/index.json."
                       L"\n4. Open a pull request to the original repository.";
        MessageBoxW(g_hManagerWnd, message.c_str(), L"Workshop export complete",
                    MB_ICONINFORMATION);
    } catch (const std::wstring& reason) {
        MessageBoxW(g_hManagerWnd, reason.c_str(), L"Workshop export",
                    MB_ICONWARNING);
    } catch (...) {
        MessageBoxW(g_hManagerWnd,
                    L"Could not prepare the selected item for export.",
                    L"Workshop export", MB_ICONWARNING);
    }
}
}  // namespace workshopExport

static void PetPage(StackPanel body) {
    auto* pet = SelectedPet();
    if (!pet) {
        body.Children().Append(
            Label(L"Select an item from the media library or import a file."));
        Action(body, L"Import media…", AddImages);
        return;
    }
    auto behavior = Group(body, L"Behavior");
    auto flag = [&](const wchar_t* name, bool PetItem::* field) {
        Toggle(behavior, name, pet->*field, [field](bool value) {
            auto* item = SelectedPet();
            if (!item)
                return;
            item->*field = value;
            if (field == &PetItem::reverse || field == &PetItem::pingPong ||
                field == &PetItem::paused) {
                item->completed = 0;
                item->returning = false;
            }
            RenderItem(*item);
            SyncImageState(*item, false);
            ApplyTopmost(*item);
            ApplyClickThrough(*item);
            if (item->hwnd)
                ShowWindow(item->hwnd,
                           VisibleItem(*item) ? SW_SHOWNOACTIVATE : SW_HIDE);
        });
    };
    flag(L"Enabled", &PetItem::enabled);
    flag(L"Always on top", &PetItem::alwaysOnTop);
    flag(L"Click-through", &PetItem::clickThrough);
    flag(L"Lock position", &PetItem::lockPosition);
    if (pet->frameCount > 1)
        flag(L"Pause animation", &PetItem::paused);
    flag(L"Follow the active virtual desktop", &PetItem::followDesktop);
    flag(L"Fade near the pointer", &PetItem::hoverFade);
    if (!_wcsicmp(PathFindExtensionW(pet->sourcePath.c_str()), L".mp4"))
        flag(L"Mute audio", &PetItem::muted);
    flag(L"Flip horizontally", &PetItem::flipX);
    flag(L"Flip vertically", &PetItem::flipY);
    flag(L"Grayscale", &PetItem::grayscale);
    flag(L"Drop shadow", &PetItem::shadow);
    if (pet->frameCount > 1) {
        flag(L"Reverse playback", &PetItem::reverse);
        flag(L"Ping-pong playback", &PetItem::pingPong);
    }
    auto appearance = Group(body, L"Appearance");
    auto slider = [&](const wchar_t* name, int low, int high,
                      int PetItem::* field) {
        controls::Slider control;
        xaml::Automation::AutomationProperties::SetName(control, name);
        control.Maximum(high);
        control.Minimum(low);
        control.StepFrequency(1);
        control.Value(std::clamp(pet->*field, low, high));
        controls::TextBox number;
        xaml::Automation::AutomationProperties::SetName(number, name);
        number.Width(60);
        number.HorizontalAlignment(xaml::HorizontalAlignment::Left);
        number.Text(std::to_wstring((int)control.Value()));
        auto weakSlider = winrt::make_weak(control);
        number.TextChanging(
            [weakSlider, low, high](controls::TextBox const& box, auto const&) {
                auto text = box.Text();
                wchar_t* end = nullptr;
                long value = wcstol(text.c_str(), &end, 10);
                if (end == text.c_str() || *end || value < low || value > high)
                    return;
                if (auto slider = weakSlider.get();
                    slider && slider.Value() != value)
                    slider.Value(value);
            });
        number.LostFocus(
            [weakSlider, low, high](auto const& sender, auto const&) {
                auto box = sender.template as<controls::TextBox>();
                if (auto slider = weakSlider.get()) {
                    auto text = box.Text();
                    wchar_t* end = nullptr;
                    long value = wcstol(text.c_str(), &end, 10);
                    if (end != text.c_str() && *end == 0)
                        slider.Value(std::clamp(value, (long)low, (long)high));
                    box.Text(std::to_wstring((int)slider.Value()));
                }
            });
        int index = g_managerSel;
        control.ValueChanged(
            [field, number, index](
                auto const&,
                controls::Primitives::RangeBaseValueChangedEventArgs const&
                    args) {
                if (index != g_managerSel)
                    return;
                if (auto* item = SelectedPet()) {
                    item->*field = (int)args.NewValue();
                    if (field == &PetItem::loops)
                        item->completed = 0;
                    SyncImageState(*item, false);
                    auto text = number.Text();
                    wchar_t* end = nullptr;
                    long typed = wcstol(text.c_str(), &end, 10);
                    if (end == text.c_str() || *end || typed != item->*field)
                        number.Text(std::to_wstring(item->*field));
                    if (field == &PetItem::fpsLimit)
                        ApplyTimer();
                    else
                        RenderItem(*item);
                }
            });
        auto row = SettingRow(appearance, name);
        row.ColumnDefinitions().GetAt(0).Width({2, xaml::GridUnitType::Star});
        row.ColumnDefinitions().GetAt(1).Width({3, xaml::GridUnitType::Star});
        controls::ColumnDefinition numeric;
        numeric.Width({1, xaml::GridUnitType::Auto});
        row.ColumnDefinitions().Append(numeric);
        auto track = SliderFace(control);
        controls::Grid::SetColumn(track, 1);
        row.Children().Append(track);
        controls::Grid::SetColumn(number, 2);
        row.Children().Append(number);
    };
    slider(L"Scale (%)", 1, 500, &PetItem::scalePercent);
    slider(L"Rotation (°)", 0, 359, &PetItem::rotationDegrees);
    slider(L"Opacity (%)", 10, 100, &PetItem::opacityPercent);
    slider(L"Brightness (%)", 0, 200, &PetItem::brightness);
    if (!_wcsicmp(PathFindExtensionW(pet->sourcePath.c_str()), L".mp4"))
        slider(L"Volume (%)", 0, 100, &PetItem::volume);
    slider(L"Warm tint (%)", 0, 100, &PetItem::tint);
    slider(L"Hue (°)", 0, 359, &PetItem::hue);
    if (pet->frameCount > 1) {
        appearance = Group(body, L"Playback");
        slider(L"Speed (%)", 10, 400, &PetItem::speed);
        slider(L"Loop count (0 = endless)", 0, 100, &PetItem::loops);
        Action(appearance, L"Restart playback", [] {
            auto* item = SelectedPet();
            if (!item || item->frameCount <= 1)
                return;
            item->returning = false;
            item->completed = 0;
            item->paused = false;
            item->currentFrame = item->reverse ? item->frameCount - 1 : 0;
            if (item->decodedFrames.empty())
                item->image->SelectActiveFrame(&Gdiplus::FrameDimensionTime,
                                               item->currentFrame);
            else
                item->image = item->decodedFrames[item->currentFrame];
            item->lastFrameTick = GetTickCount64();
            RenderItem(*item);
            SyncImageState(*item, false);
            RenderPage();
        });
    }
    if (pet->isGif)
        slider(L"FPS Limit", 5, pet->monitorHz, &PetItem::fpsLimit);
    auto publishing = Group(body, L"Workshop");
    Action(publishing, L"Export to Workshop…", workshopExport::Run);
}
static void ImportFolder() {
    BROWSEINFOW info{};
    info.hwndOwner = g_hManagerWnd;
    info.lpszTitle = L"Import media from folder";
    info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    if (auto pidl = SHBrowseForFolderW(&info)) {
        wchar_t path[MAX_PATH]{};
        bool picked = SHGetPathFromIDListW(pidl, path) != FALSE;
        CoTaskMemFree(pidl);
        if (picked && RegisterLibraryFolder(path))
            ReloadItems();
        else if (picked)
            MessageBoxW(g_hManagerWnd, L"The folder could not be saved.",
                        L"Import media", MB_ICONWARNING);
    }
}
static void WorkshopCards(StackPanel cards) {
    cards.Children().Clear();
    if (!g_workshopError.empty() && !g_workshopItems.empty())
        cards.Children().Append(Label(g_workshopError));
    if (g_workshopFetching ||
        (!g_workshopError.empty() && g_workshopItems.empty())) {
        cards.Children().Append(
            Label(g_workshopFetching ? L"Loading workshop…" : g_workshopError));
        return;
    }
    auto matches = Workshop_FilteredIndices();
    if (matches.empty())
        cards.Children().Append(Label(L"No matching media."));
    controls::Grid row{nullptr};
    int columns =
        g_workshopView == WorkshopView::List
            ? 1
            : (g_workshopView == WorkshopView::TwoCol
                   ? 2
                   : std::max(2, (int)(managerRoot.ActualWidth() - 280) / 315));
    int slot = 0;
    for (int idx : matches) {
        if (!slot) {
            row = controls::Grid();
            row.ColumnSpacing(8);
            for (int i = 0; i < columns; ++i)
                row.ColumnDefinitions().Append(controls::ColumnDefinition());
            cards.Children().Append(row);
        }
        StackPanel cell;
        controls::Grid::SetColumn(cell, slot);
        row.Children().Append(cell);
        slot = (slot + 1) % columns;
        const auto& item = g_workshopItems[idx];
        auto card = Group(cell, L"");
        if (!item.localPreviewPath.empty()) {
            auto& bmp = g_workshopItems[idx].hPreviewBmp;
            if (!bmp)
                bmp = Workshop_LoadPreview(item.localPreviewPath, 128, 96);
            if (bmp)
                card.Children().Append(Thumbnail(bmp, 108, 81));
        }
        card.Children().Append(Label(item.name, 18));
        card.Children().Append(Label(item.author + L"  ·  " + item.version));
        card.Children().Append(Label(item.description));
        std::wstring tags;
        for (const auto& tag : item.tags) {
            if (!tags.empty())
                tags += L"  ";
            tags += L"#" + tag;
        }
        if (!tags.empty())
            card.Children().Append(Label(tags, 12));
        StackPanel actions;
        actions.Orientation(controls::Orientation::Horizontal);
        actions.Spacing(4);
        controls::ScrollViewer actionScroll;
        actionScroll.HorizontalScrollMode(controls::ScrollMode::Enabled);
        actionScroll.HorizontalScrollBarVisibility(
            controls::ScrollBarVisibility::Auto);
        actionScroll.VerticalScrollBarVisibility(
            controls::ScrollBarVisibility::Disabled);
        actionScroll.Content(actions);
        card.Children().Append(actionScroll);
        bool favorite = Number(PackKey(item.slug, L"Favorite"), 0) != 0;
        Action(actions, favorite ? L"Unfavorite" : L"Favorite",
               [idx, favorite] {
                   PutNumber(PackKey(g_workshopItems[idx].slug, L"Favorite"),
                             !favorite);
                   RenderPage();
               });
        auto install =
            Action(actions,
                   item.installing ? L"Installing…"
                   : item.installed && item.version != item.installedVersion
                       ? L"Update"
                   : item.installed ? L"Reinstall"
                                    : L"Install",
                   [idx] {
                       Workshop_StartInstall(idx);
                       RenderPage();
                   });
        install.IsEnabled(!item.installing);
        if (item.installed)
            Action(actions, L"Uninstall", [idx] {
                Workshop_Uninstall(idx);
                RenderPage();
            }).IsEnabled(!item.installing);
    }
}
static void AddSearchIcon(controls::Grid host, controls::TextBox search) {
    search.Padding({34, 4, 10, 4});
    controls::FontIcon icon;
    icon.FontFamily(
        xaml::Media::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    icon.Glyph(L"\uE721");
    icon.FontSize(14);
    icon.Opacity(0.75);
    icon.Margin({10, 0, 0, 0});
    icon.HorizontalAlignment(xaml::HorizontalAlignment::Left);
    icon.VerticalAlignment(xaml::VerticalAlignment::Center);
    icon.IsHitTestVisible(false);
    xaml::Automation::AutomationProperties::SetAccessibilityView(
        icon, xaml::Automation::Peers::AccessibilityView::Raw);
    host.Children().Append(icon);
}

static void WorkshopPage(StackPanel body) {
    if (!g_workshopFetched && !g_workshopFetching)
        Workshop_StartFetch();
    controls::TextBox search;
    search.PlaceholderText(L"Search names, @authors or #tags");
    search.Text(g_workshopFilter);
    auto tools = Group(body, L"Browse");
    auto toolbar = StackPanel();
    toolbar.Orientation(controls::Orientation::Horizontal);
    toolbar.Spacing(8);
    toolbar.Margin({0, 4, 0, 4});
    search.MinWidth(160);
    search.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
    auto searchRow = FieldRow(tools);
    searchRow.Children().Append(search);
    AddSearchIcon(searchRow, search);
    controls::Grid::SetColumn(toolbar, 1);
    searchRow.Children().Append(toolbar);
    StackPanel cards;
    cards.Spacing(8);
    search.TextChanged([cards](auto const& sender, auto const&) {
        g_workshopFilter =
            sender.template as<controls::TextBox>().Text().c_str();
        for (auto& c : g_workshopFilter)
            c = (wchar_t)towlower(c);
        WorkshopCards(cards);
    });
    Toggle(tools, L"Favorites only", favoritesOnly, [cards](bool value) {
        favoritesOnly = value;
        WorkshopCards(cards);
    });
    std::set<std::wstring> unique;
    for (const auto& item : g_workshopItems)
        unique.insert(item.tags.begin(), item.tags.end());
    std::vector<std::wstring> tags{L"All tags"};
    tags.insert(tags.end(), unique.begin(), unique.end());
    auto found = std::find(tags.begin() + 1, tags.end(), workshopTag);
    int selected =
        found == tags.end() ? 0 : static_cast<int>(found - tags.begin());
    if (!selected)
        workshopTag.clear();
    Dropdown(
        tools, L"Tag", nullptr, tags, selected,
        [tags](int i) { workshopTag = i ? tags[i] : L""; }, false);
    Choice(tools, L"Layout", {L"Grid", L"List", L"Two columns"},
           (int)g_workshopView, [cards](int index) {
               g_workshopView = (WorkshopView)index;
               SaveNumber(L"WorkshopLayout", index);
               WorkshopCards(cards);
           });
    auto refresh = Action(toolbar, L"Refresh", [] {
        Workshop_StartFetch();
        RenderPage();
    });
    refresh.IsEnabled(!g_workshopFetching &&
                      std::none_of(g_workshopItems.begin(),
                                   g_workshopItems.end(),
                                   [](auto& item) { return item.installing; }));
    body.Children().Append(cards);
    WorkshopCards(cards);
}

static ULONGLONG CurrentLogon() {
    HANDLE token{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return 0;
    TOKEN_STATISTICS stats{};
    DWORD bytes{};
    bool ok = GetTokenInformation(token, TokenStatistics, &stats, sizeof(stats),
                                  &bytes) != FALSE;
    CloseHandle(token);
    return ok ? (static_cast<ULONGLONG>(
                     static_cast<DWORD>(stats.AuthenticationId.HighPart))
                 << 32) |
                    stats.AuthenticationId.LowPart
              : 0;
}
static bool ManagerAtSignIn() {
    return ConfigNumber(L"OpenAtLogon", 0) != 0;
}
static bool FirstManagerStartThisLogon() {
    auto id = CurrentLogon();
    if (!id)
        return false;
    auto value = std::to_wstring(id);
    return ReadText(L"ManagerLogon") != value &&
           SaveText(L"ManagerLogon", value);
}
// Adopt only the exact startup command written by the private build.
static void MigrateStartupOption() {
    constexpr auto run = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    wchar_t oldCommand[32768]{}, host[32768];
    DWORD bytes = sizeof(oldCommand);
    if (!configWritable ||
        RegGetValueW(HKEY_CURRENT_USER, run, L"AnimaWindhawkEngine",
                     RRF_RT_REG_SZ, nullptr, oldCommand,
                     &bytes) != ERROR_SUCCESS)
        return;
    DWORD n = GetModuleFileNameW(nullptr, host, std::size(host));
    if (!n || n >= std::size(host))
        return;
    std::wstring dir(host, n);
    auto split = dir.find_last_of(L"\\");
    if (split == dir.npos)
        return;
    auto expected =
        L"\"" + dir.substr(0, split + 1) + L"windhawk.exe\" -tray-only";
    if (_wcsicmp(oldCommand, expected.c_str()))
        return;
    if (Number(Setting(L"OpenAtLogon"), -1) < 0 &&
        !SaveNumber(L"OpenAtLogon", 1))
        return;
    auto status =
        RegDeleteKeyValueW(HKEY_CURRENT_USER, run, L"AnimaWindhawkEngine");
    if (status != ERROR_SUCCESS)
        Wh_Log(L"Could not remove previous startup entry (%ld)", status);
}
static void OpenManagerAtSignIn() {
    if (!ManagerAtSignIn())
        return;
    if (FirstManagerStartThisLogon())
        PostMessageW(g_hCtrlWnd, WM_APP_OPEN_MANAGER, static_cast<WPARAM>(-1),
                     0);
}
static std::wstring DecoderPath() {
    return ReadText(L"FFmpegPath");
}
static void ChooseDecoder() {
    wchar_t file[32768]{};
    OPENFILENAMEW pick{sizeof(pick)};
    pick.hwndOwner = g_hManagerWnd;
    pick.lpstrFilter = L"FFmpeg executable\0ffmpeg.exe\0\0";
    pick.lpstrFile = file;
    pick.nMaxFile = std::size(file);
    pick.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&pick))
        return;
    if (_wcsicmp(GetFileNameFromPath(file).c_str(), L"ffmpeg.exe")) {
        MessageBoxW(g_hManagerWnd, L"Choose ffmpeg.exe.", L"Media decoder",
                    MB_ICONINFORMATION);
        return;
    }
    if (SaveText(L"FFmpegPath", file))
        ReloadItems();
}
static std::wstring ScenePicker(bool save) {
    wchar_t path[32768]{};
    OPENFILENAMEW dialog{sizeof(dialog)};
    dialog.hwndOwner = g_hManagerWnd;
    dialog.lpstrFilter = L"Media layout (*.json)\0*.json\0\0";
    dialog.lpstrDefExt = L"json";
    dialog.lpstrFile = path;
    dialog.nMaxFile = std::size(path);
    dialog.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST |
                   (save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
    return (save ? GetSaveFileNameW(&dialog) : GetOpenFileNameW(&dialog)) ? path
                                                                          : L"";
}

// Scenes describe visible media and overrides. Cache and binary media stay
// local.
static bool SaveScene(const std::wstring& path) {
    if (g_items.size() > 1000)
        return false;
    json::JsonArray list;
    for (const auto& item : g_items) {
        json::JsonObject entry;
        auto save = [&](PCWSTR key, int value) {
            entry.SetNamedValue(key, json::JsonValue::CreateNumberValue(value));
        };
        entry.SetNamedValue(L"path",
                            json::JsonValue::CreateStringValue(item.filePath));
        entry.SetNamedValue(
            L"id", json::JsonValue::CreateStringValue(item.registryKey));
        save(L"x", item.posX);
        save(L"y", item.posY);
        save(L"hidden", item.hidden);
        save(L"FollowDesktop", item.followDesktop);
        save(L"HoverFade", item.hoverFade);
        save(L"Muted", item.muted);
        save(L"Volume", item.volume);
        save(L"FlipHorizontal", item.flipX);
        save(L"FlipVertical", item.flipY);
        save(L"Grayscale", item.grayscale);
        save(L"Shadow", item.shadow);
        save(L"Tint", item.tint);
        save(L"Brightness", item.brightness);
        save(L"Hue", item.hue);
        save(L"PlaybackSpeed", item.speed);
        save(L"Reverse", item.reverse);
        save(L"PingPong", item.pingPong);
        save(L"LoopCount", item.loops);
        save(L"Enabled", item.enabled);
        save(L"AlwaysOnTop", item.alwaysOnTop);
        save(L"ClickThrough", item.clickThrough);
        save(L"LockPosition", item.lockPosition);
        save(L"ScalePercent", item.scalePercent);
        save(L"OpacityPercent", item.opacityPercent);
        save(L"RotationDegrees", item.rotationDegrees);
        save(L"FpsLimit", item.fpsLimit);
        save(L"Paused", item.paused);
        list.Append(entry);
    }
    json::JsonObject scene;
    scene.SetNamedValue(L"format", json::JsonValue::CreateStringValue(
                                       L"AnimaWindhawkEngine.scene.1"));
    scene.SetNamedValue(L"media", list);
    return WriteSmallFile(path, winrt::to_string(scene.Stringify()));
}

static bool LoadScene(const std::wstring& path) {
    if (!configWritable)
        return false;
    try {
        auto scene =
            json::JsonObject::Parse(winrt::to_hstring(ReadSmallFile(path)));
        if (scene.GetNamedString(L"format") != L"AnimaWindhawkEngine.scene.1")
            return false;
        auto list = scene.GetNamedArray(L"media");
        if (list.Size() > 1000)
            return false;
        struct Entry {
            std::wstring path, id;
            PetItem settings;
        };
        std::vector<Entry> entries;
        for (const auto& value : list) {
            auto obj = value.GetObject();
            Entry e;
            e.path = obj.GetNamedString(L"path").c_str();
            e.id = obj.GetNamedString(L"id", L"").c_str();
            if (!fs::path(e.path).is_absolute() ||
                e.path.starts_with(L"\\\\") || !HasSupportedExt(e.path) ||
                PathIsUNCW(e.path.c_str()) ||
                GetFileAttributesW(e.path.c_str()) == INVALID_FILE_ATTRIBUTES)
                return false;
            auto& item = e.settings;
            auto readField = [&](PCWSTR key, auto& field, int low, int high) {
                auto n = obj.GetNamedNumber(key, static_cast<int>(field));
                if (!(n >= low && n <= high) || std::floor(n) != n)
                    throw std::invalid_argument("Invalid scene value");
                field = static_cast<int>(n);
            };
            readField(L"x", item.posX, INT_MIN, INT_MAX);
            readField(L"y", item.posY, INT_MIN, INT_MAX);
            readField(L"hidden", item.hidden, 0, 1);
            readField(L"FollowDesktop", item.followDesktop, 0, 1);
            readField(L"HoverFade", item.hoverFade, 0, 1);
            readField(L"Muted", item.muted, 0, 1);
            readField(L"Volume", item.volume, 0, 100);
            readField(L"FlipHorizontal", item.flipX, 0, 1);
            readField(L"FlipVertical", item.flipY, 0, 1);
            readField(L"Grayscale", item.grayscale, 0, 1);
            readField(L"Shadow", item.shadow, 0, 1);
            readField(L"Tint", item.tint, 0, 100);
            readField(L"Brightness", item.brightness, 0, 200);
            readField(L"Hue", item.hue, 0, 359);
            readField(L"PlaybackSpeed", item.speed, 10, 400);
            readField(L"Reverse", item.reverse, 0, 1);
            readField(L"PingPong", item.pingPong, 0, 1);
            readField(L"LoopCount", item.loops, 0, 100);
            readField(L"Enabled", item.enabled, 0, 1);
            readField(L"AlwaysOnTop", item.alwaysOnTop, 0, 1);
            readField(L"ClickThrough", item.clickThrough, 0, 1);
            readField(L"LockPosition", item.lockPosition, 0, 1);
            readField(L"ScalePercent", item.scalePercent, 1, 500);
            readField(L"OpacityPercent", item.opacityPercent, 10, 100);
            readField(L"RotationDegrees", item.rotationDegrees, 0, 359);
            readField(L"FpsLimit", item.fpsLimit, 5, INT_MAX);
            readField(L"Paused", item.paused, 0, 1);
            entries.push_back(std::move(e));
        }
        // Resolve each occurrence separately, including clones with identical
        // paths.
        std::vector<bool> used(g_items.size());
        auto resolve = [&](const Entry& entry) -> int {
            int fallback = -1;
            for (size_t i = 0; i < g_items.size(); ++i) {
                if (used[i] ||
                    _wcsicmp(g_items[i].filePath.c_str(), entry.path.c_str()))
                    continue;
                if (g_items[i].registryKey == entry.id)
                    return static_cast<int>(i);
                if (fallback < 0)
                    fallback = static_cast<int>(i);
            }
            return fallback;
        };
        auto paths = g_settings.imagePaths;
        for (const auto& e : entries) {
            int i = resolve(e);
            if (i < 0)
                paths.push_back(e.path);
            else
                used[i] = true;
        }
        if (paths.size() != g_settings.imagePaths.size()) {
            SaveImagePaths(paths);
            if (imageOrder.size() != paths.size())
                return false;
            g_settings.imagePaths = std::move(paths);
            ReloadAll(false);
        }
        used.assign(g_items.size(), false);
        std::vector<int> order;
        for (const auto& e : entries) {
            int i = resolve(e);
            if (i < 0)
                return false;
            used[i] = true;
            order.push_back(i);
        }
        for (size_t n = 0; n < entries.size(); ++n) {
            auto& e = entries[n];
            auto& live = g_items[order[n]];
            e.settings.registryKey = live.registryKey;
            e.settings.isGif = live.isGif;
            e.settings.frameCount = live.frameCount;
            SyncImageState(e.settings, false);
            e.settings.baseWidth = live.baseWidth;
            e.settings.baseHeight = live.baseHeight;
            auto pos =
                RecoverPosition(e.settings, {e.settings.posX, e.settings.posY});
            live.posX = pos.x;
            live.posY = pos.y;
            live.hidden = e.settings.hidden;
            if (live.hwnd)
                SetWindowPos(live.hwnd, nullptr, pos.x, pos.y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            SaveStoredPosition(live.registryKey, pos.x, pos.y);
        }
        for (size_t i = 0; i < used.size(); ++i)
            if (!used[i]) {
                g_items[i].enabled = false;
                SyncImageState(g_items[i], false);
            }
        ReloadAll(false);
        return true;
    } catch (...) {
        return false;
    }
}

static void RememberProfile(const std::wstring& path);
static void PopulateProfiles() {
    if (!managerRoot)
        return;
    auto panel = Named<StackPanel>(L"ProfileLinks");
    panel.Children().Clear();
    auto recent = ReadList(Setting(L"RecentProfiles"));
    for (const auto& time : ReadList(Setting(L"SceneTimes"))) {
        auto file = ReadText((L"Scene." + time).c_str());
        if (!file.empty() &&
            std::none_of(recent.begin(), recent.end(), [&](const auto& p) {
                return !_wcsicmp(p.c_str(), file.c_str());
            }))
            recent.push_back(file);
    }

    auto styleProfileLink = [](Button button) {
        button.Style(managerRoot.Resources()
                         .Lookup(winrt::box_value(L"SidebarGroupButton"))
                         .as<xaml::Style>());
        button.Padding({30, 6, 10, 6});
    };
    if (recent.empty()) {
        auto button = Action(panel, L"Open profile…", [] {
            auto path = ScenePicker(false);
            if (path.empty())
                return;
            if (LoadScene(path))
                RememberProfile(path);
            else
                MessageBoxW(g_hManagerWnd,
                            L"Could not open the selected layout.", L"Profiles",
                            MB_ICONWARNING);
        });
        styleProfileLink(button);
    }
    for (size_t i = 0; i < std::min<size_t>(3, recent.size()); ++i) {
        auto path = recent[i];
        auto title = fs::path(path).stem().wstring();
        auto button = Action(panel, title.c_str(), [path] {
            if (LoadScene(path))
                RememberProfile(path);
            else
                MessageBoxW(g_hManagerWnd,
                            L"Could not open this profile. Check the scene and "
                            L"its media files.",
                            L"Profiles", MB_ICONWARNING);
        });
        styleProfileLink(button);
        controls::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring(path)));
    }
}
static void RememberProfile(const std::wstring& path) {
    auto recent = ReadList(Setting(L"RecentProfiles"));
    recent.erase(std::remove_if(recent.begin(), recent.end(),
                                [&](const auto& p) {
                                    return !_wcsicmp(p.c_str(), path.c_str());
                                }),
                 recent.end());
    recent.insert(recent.begin(), path);
    if (recent.size() > 3)
        recent.resize(3);
    if (StoreList(Setting(L"RecentProfiles"), recent))
        PopulateProfiles();
}
static void SaveLayoutAs() {
    auto path = ScenePicker(true);
    if (path.empty())
        return;
    if (SaveScene(path))
        RememberProfile(path);
    else
        MessageBoxW(g_hManagerWnd, L"Could not save the layout.", L"Layouts",
                    MB_ICONWARNING);
}

static void ManageLayoutFile(bool remove) {
    auto source = ScenePicker(false);
    if (source.empty())
        return;
    auto bytes = ReadSmallFile(source);
    try {
        auto scene = json::JsonObject::Parse(winrt::to_hstring(bytes));
        if (scene.GetNamedString(L"format") != L"AnimaWindhawkEngine.scene.1")
            throw std::invalid_argument("layout format");
        scene.GetNamedArray(L"media");
    } catch (...) {
        MessageBoxW(g_hManagerWnd,
                    L"Select an AnimaWindhawkEngine layout JSON file.",
                    L"Layouts", MB_ICONWARNING);
        return;
    }
    if (!remove) {
        auto dest = ScenePicker(true);
        if (dest.empty() || !_wcsicmp(source.c_str(), dest.c_str()))
            return;
        if (WriteSmallFile(dest, bytes))
            RememberProfile(dest);
        else
            MessageBoxW(g_hManagerWnd, L"Could not copy the layout.",
                        L"Layouts", MB_ICONWARNING);
        return;
    }
    auto prompt = L"Delete this saved layout file?\n\n" + source;
    if (MessageBoxW(g_hManagerWnd, prompt.c_str(), L"Delete layout",
                    MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
        return;
    if (!DeleteFileW(source.c_str())) {
        MessageBoxW(g_hManagerWnd, L"Could not delete the layout.", L"Layouts",
                    MB_ICONWARNING);
        return;
    }
    auto recent = ReadList(Setting(L"RecentProfiles"));
    std::erase_if(recent, [&](const auto& p) {
        return !_wcsicmp(p.c_str(), source.c_str());
    });
    StoreList(Setting(L"RecentProfiles"), recent);
    auto slots = ReadList(Setting(L"SceneTimes"));
    std::erase_if(slots, [&](const auto& t) {
        auto key = L"Scene." + t;
        if (_wcsicmp(ReadText(key.c_str()).c_str(), source.c_str()))
            return false;
        Wh_DeleteValue(Setting(key).c_str());
        return true;
    });
    StoreList(Setting(L"SceneTimes"), slots);
    PopulateProfiles();
    RenderPage();
}

static void SceneControls(StackPanel body) {
    auto group = Group(body, L"Scenes");
    auto layoutRow = SettingRow(group, L"Layouts");
    StackPanel files;
    files.Orientation(controls::Orientation::Horizontal);
    files.Spacing(6);
    controls::Grid::SetColumn(files, 1);
    layoutRow.Children().Append(files);
    Action(files, L"Save…", SaveLayoutAs);
    Action(files, L"Open…", [] {
        auto path = ScenePicker(false);
        if (path.empty())
            return;
        if (LoadScene(path))
            RememberProfile(path);
        else
            MessageBoxW(g_hManagerWnd,
                        L"Could not restore this layout. Check that its media "
                        L"files exist.",
                        L"Scenes", MB_ICONWARNING);
    });
    Button manage;
    manage.Content(winrt::box_value(L"Manage layouts…"));
    auto fileMenu = ActionDropdown(
        controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedRight);
    AddDropdownAction(fileMenu, L"Save current layout…", SaveLayoutAs);
    AddDropdownAction(fileMenu, L"Copy layout…",
                      [] { ManageLayoutFile(false); });
    AddDropdownAction(fileMenu, L"Delete layout…",
                      [] { ManageLayoutFile(true); });
    manage.Flyout(fileMenu);
    files.Children().Append(manage);
    auto row = SettingRow(group, L"Daily scene",
                          L"Enter a local time (24-hour HH:MM).");
    StackPanel actions;
    actions.Orientation(controls::Orientation::Horizontal);
    actions.Spacing(4);
    controls::TextBox time;
    time.Width(76);
    time.PlaceholderText(L"HH:MM");
    actions.Children().Append(time);
    Action(actions, L"Schedule…", [time] {
        std::wstring t(time.Text());
        if (t.size() != 5 || t[2] != L':' ||
            t.find_first_not_of(L"0123456789:") != t.npos ||
            t.find(L':') != 2 || t.rfind(L':') != 2 || t.substr(0, 2) > L"23" ||
            t.substr(3) > L"59") {
            MessageBoxW(g_hManagerWnd, L"Use a time from 00:00 to 23:59.",
                        L"Scenes", MB_ICONINFORMATION);
            return;
        }
        auto file = ScenePicker(false);
        if (file.empty())
            return;
        auto slots = ReadList(Setting(L"SceneTimes"));
        if (std::find(slots.begin(), slots.end(), t) == slots.end())
            slots.push_back(t);
        if (PutString(Setting((L"Scene." + t).c_str()), file))
            StoreList(Setting(L"SceneTimes"), slots);
        PopulateProfiles();
        RenderPage();
    });
    controls::Grid::SetColumn(actions, 1);
    row.Children().Append(actions);
    for (auto& t : ReadList(Setting(L"SceneTimes"))) {
        auto name =
            fs::path(ReadText((L"Scene." + t).c_str())).stem().wstring();
        auto line = SettingRow(group, (t + L" · " + name).c_str());
        Button remove;
        remove.Content(winrt::box_value(L"Remove"));
        remove.Click([t](auto&&, auto&&) {
            auto slots = ReadList(Setting(L"SceneTimes"));
            slots.erase(std::remove(slots.begin(), slots.end(), t),
                        slots.end());
            if (StoreList(Setting(L"SceneTimes"), slots))
                Wh_DeleteValue(Setting((L"Scene." + t).c_str()).c_str());
            PopulateProfiles();
            RenderPage();
        });
        controls::Grid::SetColumn(remove, 1);
        line.Children().Append(remove);
    }
}

static void CheckSceneSchedule() {
    SYSTEMTIME now{};
    GetLocalTime(&now);
    static DWORD lastMinute = 0xffffffff;
    DWORD stamp =
        (((now.wYear * 13u + now.wMonth) * 32 + now.wDay) * 24 + now.wHour) *
            60 +
        now.wMinute;
    if (stamp == lastMinute)
        return;
    lastMinute = stamp;
    wchar_t slot[8];
    swprintf_s(slot, L"%02u:%02u", now.wHour, now.wMinute);
    auto path = ReadText((std::wstring(L"Scene.") + slot).c_str());
    if (path.empty())
        return;
    if (LoadScene(path))
        RememberProfile(path);
    else
        Wh_Log(L"Could not restore scheduled scene: %s", path.c_str());
}

static void AppPage(StackPanel body) {
    SceneControls(body);
    auto colors = Group(body, L"Appearance");
    Dropdown(colors, L"Theme", L"Choose how the manager appears.",
             {L"Dark", L"Light", L"System"}, (int)g_managerTheme,
             [](int index) {
                 g_managerTheme = (ManagerTheme)index;
                 SaveNumber(L"ManagerTheme", index);
             });
    Dropdown(
        colors, L"Accent", L"Used for selections and active controls.",
        {L"Purple", L"Blue", L"Green", L"Red", L"Orange", L"Windows accent"},
        g_accentIndex, [](int index) {
            g_accentIndex = index;
            SaveNumber(L"Accent", index);
        });
    auto desktop = Group(body, L"Desktop media");
    Action(desktop, L"Choose automatic playback behavior…",
           [] { ShowSavingDialog(); });
    Toggle(desktop, L"Limit animation to 8 FPS on battery", conservePower,
           [](bool on) {
               conservePower = on;
               SaveNumber(L"ConservePower", on);
           });
    Toggle(desktop, L"Pause fully covered media", pauseCovered, [](bool on) {
        pauseCovered = on;
        SaveNumber(L"PauseCovered", on);
    });
    Toggle(desktop, L"Hide during fullscreen apps and presentations",
           hideFullscreen, [](bool enabled) {
               hideFullscreen = enabled;
               SaveNumber(L"HideFullscreen", enabled);
               UpdateFullscreen();
           });
    Toggle(desktop, L"Snap to monitor work-area edges", snapEdges,
           [](bool enabled) {
               snapEdges = enabled;
               SaveNumber(L"SnapEdges", enabled);
           });
    SettingRow(
        desktop, L"Hide or show all media",
        panicAvailable ? L"Ctrl + Alt + Shift + H" : L"Shortcut unavailable");
    auto startup = Group(body, L"Startup");
    Toggle(startup, L"Launch this manager at sign-in", ManagerAtSignIn(),
           [](bool on) {
               if (!SaveNumber(L"OpenAtLogon", on)) {
                   RenderPage();
                   return;
               }
               if (on)
                   FirstManagerStartThisLogon();
           });
    Toggle(startup, L"Tray icon", g_settings.trayIcon, [](bool on) {
        g_settings.trayIcon = on;
        SaveNumber(L"TrayIcon", on);
        ApplyTrayIcon();
    });
    auto decoder = Group(body, L"Media decoder");
    auto path = DecoderPath();
    auto decoderRow = SettingRow(
        decoder,
        path.empty() ? L"Choose FFmpeg for WebP, APNG and MP4." : path.c_str());
    StackPanel decoderActions;
    decoderActions.VerticalAlignment(xaml::VerticalAlignment::Center);
    controls::Grid::SetColumn(decoderActions, 1);
    decoderRow.Children().Append(decoderActions);
    Action(decoderActions, L"Choose ffmpeg.exe", [] { ChooseDecoder(); });
    auto keys = Group(body, L"Manager hotkey");
    auto shortcut =
        g_hotkeyCapturing
            ? std::wstring(L"Press a shortcut, or Escape to cancel.")
            : HotkeyToString(g_hotkeyMods, g_hotkeyVk);
    if (!hotkeyNotice.empty())
        shortcut = hotkeyNotice;
    auto shortcutRow = SettingRow(keys, L"Open manager", shortcut.c_str());
    StackPanel shortcuts;
    shortcuts.Orientation(controls::Orientation::Horizontal);
    shortcuts.Spacing(6);
    controls::Grid::SetColumn(shortcuts, 1);
    shortcutRow.Children().Append(shortcuts);
    Action(shortcuts, L"Set shortcut", [] {
        UnregisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER);
        hotkeyNotice.clear();
        StartKeyCapture();
        g_hotkeyCapturing = g_hKeyHook != nullptr;
        if (!g_hotkeyCapturing)
            ReapplyHotkey();
        RenderPage();
    });
    Action(shortcuts, L"Clear shortcut", [] {
        StopKeyCapture();
        g_hotkeyCapturing = false;
        g_hotkeyMods = g_hotkeyVk = 0;
        SaveHotkey();
        ReapplyHotkey();
        RenderPage();
    });
    auto footer = FieldRow(body);
    StackPanel left, right;
    controls::Grid::SetColumn(right, 1);
    footer.Children().Append(left);
    footer.Children().Append(right);
    Action(left, L"Replay welcome guide", [] { ShowWelcomeGuide(); });
    Action(right, L"Keyboard shortcuts...", [] {
        std::wstring help =
            L"Open manager: " + HotkeyToString(g_hotkeyMods, g_hotkeyVk);
        help += L"\nHide or show all media: ";
        help += panicAvailable ? L"Ctrl + Alt + Shift + H" : L"Unavailable";
        help +=
            L"\n\nOver desktop media\nCtrl + wheel: resize\nShift + wheel: "
            L"rotate\nAlt + wheel: change opacity"
            L"\n\nIn the manager\nTab / Shift + Tab: next / previous "
            L"control\nEscape: cancel shortcut capture";
        MessageBoxW(g_hManagerWnd, help.c_str(), L"Keyboard shortcuts",
                    MB_OK | MB_ICONINFORMATION);
    });
}
static void RenderPage() {
    if (!managerRoot)
        return;
    auto body = Named<StackPanel>(L"Body");
    body.Children().Clear();
    if (!configWritable)
        body.Children().Append(
            Label(L"Changes cannot be saved to Windhawk storage. See the "
                  L"Windhawk log for the access or migration error."));
    const wchar_t* title = L"Media";
    switch (page) {
        case Page::Pet:
            if (SelectedPet())
                title = SelectedPet()->fileName.c_str();
            PetPage(body);
            break;
        case Page::Workshop:
            title = L"Workshop";
            WorkshopPage(body);
            break;
        case Page::App:
            title = L"App settings";
            AppPage(body);
            break;
    }
    UpdateNavigation();
    Named<TextBlock>(L"Title").Text(title);
    Named<TextBlock>(L"Build").Text(std::wstring(L"v") + kModVersion +
                                    L" · b." + g_windowsBuild);
}
static void Navigate(Page next, int mediaIndex = -1) {
    bool changed =
        page != next || (mediaIndex >= 0 && mediaIndex != g_managerSel);
    bool lower = static_cast<int>(next) > static_cast<int>(page);
    if (mediaIndex >= 0) {
        if (page == Page::Pet) {
            lower = mediaIndex > g_managerSel;
            for (auto item : Named<controls::ListView>(L"Pets").Items()) {
                auto tag = winrt::unbox_value<int>(
                    item.as<controls::ListViewItem>().Tag());
                if (tag == g_managerSel || tag == mediaIndex) {
                    lower = tag == g_managerSel;
                    break;
                }
            }
        }
        g_managerSel = mediaIndex;
    }
    page = next;
    if (page != Page::Pet)
        Named<controls::ListView>(L"Pets").SelectedIndex(-1);
    Named<controls::ScrollViewer>(L"Scroll").ScrollToVerticalOffset(0);
    RenderPage();
    if (changed) {
        using xaml::Media::Animation::Storyboard;
        auto slide = managerRoot.Resources()
                         .Lookup(winrt::box_value(L"PageSlide"))
                         .as<Storyboard>();
        slide.Stop();
        if (ManagerAnimationsEnabled()) {
            auto movement = slide.Children()
                                .GetAt(0)
                                .as<xaml::Media::Animation::DoubleAnimation>();
            movement.From(lower ? 16.0 : -16.0);
            Storyboard::SetTarget(
                movement,
                Named<xaml::Media::TranslateTransform>(L"PageOffset"));
            Storyboard::SetTarget(slide.Children().GetAt(1),
                                  Named<controls::Grid>(L"PagePanel"));
            slide.Begin();
        }
    }
}
// The executable belongs to Windhawk. Supply the unpackaged XAML declaration
// through a private activation context while creating the desktop island.
static HANDLE managerManifest =
    INVALID_HANDLE_VALUE;  // owned by the engine STA

static HANDLE LoadManagerManifest() {
    static constexpr char xml[] =
        R"MANIFEST(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <assemblyIdentity name="AnimaWindhawkEngine.Manager" version="1.0.0.0" type="win32" processorArchitecture="*"/>
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application>
      <maxversiontested Id="10.0.18362.1"/>
      <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
    </application>
  </compatibility>
</assembly>)MANIFEST";
    wchar_t directory[MAX_PATH]{}, path[MAX_PATH]{};
    DWORD length = GetTempPathW(MAX_PATH, directory);
    if (!length)
        winrt::throw_last_error();
    if (length >= MAX_PATH)
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE));
    if (!GetTempFileNameW(directory, L"AWX", 0, path))
        winrt::throw_last_error();
    struct TempFile {
        const wchar_t* path;
        ~TempFile() { DeleteFileW(path); }
    } cleanup{path};

    HANDLE file =
        CreateFileW(path, GENERIC_WRITE, 0, nullptr, TRUNCATE_EXISTING,
                    FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        winrt::throw_last_error();
    DWORD written = 0;
    constexpr DWORD bytes = sizeof(xml) - 1;
    DWORD error = WriteFile(file, xml, bytes, &written, nullptr)
                      ? ERROR_SUCCESS
                      : GetLastError();
    CloseHandle(file);
    if (error)
        winrt::throw_hresult(HRESULT_FROM_WIN32(error));
    if (written != bytes)
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_WRITE_FAULT));

    ACTCTXW description{};
    description.cbSize = sizeof(description);
    description.lpSource = path;
    HANDLE context = CreateActCtxW(&description);
    if (context == INVALID_HANDLE_VALUE)
        winrt::throw_last_error();
    return context;
}

class ManagerManifestScope {
    ULONG_PTR cookie_{};

   public:
    ManagerManifestScope() {
        if (managerManifest == INVALID_HANDLE_VALUE)
            managerManifest = LoadManagerManifest();
        if (!ActivateActCtx(managerManifest, &cookie_))
            winrt::throw_last_error();
    }
    ~ManagerManifestScope() { DeactivateActCtx(0, cookie_); }
    ManagerManifestScope(const ManagerManifestScope&) = delete;
    ManagerManifestScope& operator=(const ManagerManifestScope&) = delete;
};

static const wchar_t* managerStartupStage = L"Create manager window";
static std::wstring managerStartupError;

static void AttachIsland(HWND hwnd,
                         hosting::DesktopWindowXamlSource& source,
                         HWND& child,
                         winrt::com_ptr<IDesktopWindowXamlSourceNative2>& input,
                         const wchar_t*& stage) {
    stage = L"Initialize WindowsXamlManager";
    if (!managerRuntime)
        managerRuntime =
            hosting::WindowsXamlManager::InitializeForCurrentThread();
    stage = L"Create desktop XAML source";
    source = hosting::DesktopWindowXamlSource();
    auto native = source.as<IDesktopWindowXamlSourceNative>();
    stage = L"Attach desktop XAML source";
    winrt::check_hresult(native->AttachToWindow(hwnd));
    winrt::check_hresult(native->get_WindowHandle(&child));
    input = source.try_as<IDesktopWindowXamlSourceNative2>();
}

static constexpr UINT_PTR kGroupTimer = 92;
struct SidebarMotion {
    bool profiles{};
    double progress{}, start{};
    ULONGLONG since{};
};
static SidebarMotion sidebarMotion;

static void StepSidebar(HWND hwnd) {
    auto& motion = sidebarMotion;
    double t = motion.since ? std::min(1.0, (GetTickCount64() - motion.since) / 200.0) : 1;
    double goal = motion.profiles ? 1.0 : 0.0;
    double f = goal + (motion.start - goal) * std::pow(1.0 - t, 3);
    motion.progress = f;
    auto media = Named<controls::Grid>(L"MediaPane");
    auto links = Named<StackPanel>(L"ProfileLinks");
    auto group = Named<controls::Grid>(L"LibraryGroup");
    auto mediaRow = Named<controls::RowDefinition>(L"MediaListRow");
    auto profilesRow = Named<controls::RowDefinition>(L"ProfilesRow");
    media.Opacity(1 - f);
    links.Opacity(f);
    Named<xaml::Media::RotateTransform>(L"MediaAngle").Angle(90 * (1 - f));
    Named<xaml::Media::RotateTransform>(L"ProfilesAngle").Angle(90 * f);
    if (t >= 1) {
        KillTimer(hwnd, kGroupTimer);
        motion.since = 0;
        media.Visibility(motion.profiles ? xaml::Visibility::Collapsed : xaml::Visibility::Visible);
        links.Visibility(motion.profiles ? xaml::Visibility::Visible : xaml::Visibility::Collapsed);
        mediaRow.Height({motion.profiles ? 0.0 : 1.0, motion.profiles ? xaml::GridUnitType::Pixel : xaml::GridUnitType::Star});
        profilesRow.Height({1, xaml::GridUnitType::Auto});
        return;
    }
    double room = std::max(0.0, group.ActualHeight() -
        Named<Button>(L"MediaHeader").ActualHeight() - Named<Button>(L"ProfilesHeader").ActualHeight());
    links.Measure({static_cast<float>(group.ActualWidth()), INFINITY});
    mediaRow.Height({room * (1 - f), xaml::GridUnitType::Pixel});
    profilesRow.Height({std::min(room, static_cast<double>(links.DesiredSize().Height)) * f, xaml::GridUnitType::Pixel});
}

static void SelectSidebarGroup(HWND hwnd, bool profiles) {
    auto& motion = sidebarMotion;
    if (motion.profiles == profiles)
        return;
    motion.profiles = profiles;
    motion.start = motion.progress;
    for (auto name : {L"MediaPane", L"ProfileLinks"})
        Named<xaml::FrameworkElement>(name).Visibility(xaml::Visibility::Visible);
    Named<controls::Grid>(L"MediaPane").IsHitTestVisible(!profiles);
    Named<StackPanel>(L"ProfileLinks").IsHitTestVisible(profiles);
    motion.since = ManagerAnimationsEnabled() && SetTimer(hwnd, kGroupTimer, 16, nullptr)
                       ? GetTickCount64() : 0;
    StepSidebar(hwnd);
}

static void BuildManager(HWND hwnd) {
    managerStartupStage = L"Activate manager manifest";
    ManagerManifestScope manifest;
    AttachIsland(hwnd, managerSource, islandWindow, managerInput,
                 managerStartupStage);
    managerStartupStage = L"Build manager controls";
    managerRoot = xaml::Markup::XamlReader::Load(
                      LR"XAML(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
  <Grid.Resources>
    <ResourceDictionary>
    <ResourceDictionary.ThemeDictionaries>
      <ResourceDictionary x:Key="Default">
        <SolidColorBrush x:Key="GroupLabelBrush" Color="#FFFFFF"/>
        <SolidColorBrush x:Key="GroupHoverBrush" Color="#B0B0B0"/>
      </ResourceDictionary>
      <ResourceDictionary x:Key="Light">
        <SolidColorBrush x:Key="GroupLabelBrush" Color="#383838"/>
        <SolidColorBrush x:Key="GroupHoverBrush" Color="#101010"/>
      </ResourceDictionary>
      <ResourceDictionary x:Key="HighContrast">
        <SolidColorBrush x:Key="GroupLabelBrush" Color="{ThemeResource SystemColorButtonTextColor}"/>
        <SolidColorBrush x:Key="GroupHoverBrush" Color="{ThemeResource SystemColorHighlightColor}"/>
      </ResourceDictionary>
    </ResourceDictionary.ThemeDictionaries>
    <Storyboard x:Key="PageSlide">
      <DoubleAnimation Storyboard.TargetProperty="Y" From="14" To="0" Duration="0:0:0.16" FillBehavior="Stop">
        <DoubleAnimation.EasingFunction><CubicEase EasingMode="EaseOut"/></DoubleAnimation.EasingFunction>
      </DoubleAnimation>
      <DoubleAnimation Storyboard.TargetProperty="Opacity" From="0.42" To="1" Duration="0:0:0.16" FillBehavior="Stop"/>
    </Storyboard>
    <Storyboard x:Key="TabFade">
      <DoubleAnimation Storyboard.TargetProperty="Opacity" From="0.45" To="1" Duration="0:0:0.18" FillBehavior="Stop">
        <DoubleAnimation.EasingFunction><CubicEase EasingMode="EaseOut"/></DoubleAnimation.EasingFunction>
      </DoubleAnimation>
    </Storyboard>
    <SolidColorBrush x:Key="AnimaAccent" Color="#7E57C2"/>
    <Style x:Key="OptionMenu" TargetType="FlyoutPresenter">
      <Setter Property="Padding" Value="4"/><Setter Property="MinWidth" Value="0"/><Setter Property="CornerRadius" Value="8"/>
    </Style>
    <ControlTemplate x:Key="ActionFace" TargetType="Button">
      <Grid Background="Transparent">
        <VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates">
          <VisualState x:Name="Normal"/>
          <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="Tint.Opacity" Value="0.07"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="Tint.Opacity" Value="0.12"/><Setter Target="Caption.Opacity" Value="0.78"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="Caption.Opacity" Value="0.36"/></VisualState.Setters></VisualState>
        </VisualStateGroup></VisualStateManager.VisualStateGroups>
        <Border CornerRadius="{TemplateBinding CornerRadius}" Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="{TemplateBinding BorderThickness}"/>
        <Border x:Name="Tint" CornerRadius="{TemplateBinding CornerRadius}" Background="{TemplateBinding Foreground}" Opacity="0"/>
        <ContentPresenter x:Name="Caption" Content="{TemplateBinding Content}" Foreground="{TemplateBinding Foreground}" Margin="{TemplateBinding Padding}" VerticalAlignment="{TemplateBinding VerticalContentAlignment}" HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"/>
      </Grid>
    </ControlTemplate>
    <Style x:Key="SidebarGroupButton" TargetType="Button">
      <Setter Property="HorizontalAlignment" Value="Stretch"/>
      <Setter Property="HorizontalContentAlignment" Value="Left"/>
      <Setter Property="UseSystemFocusVisuals" Value="True"/>
      <Setter Property="Padding" Value="10,6"/>
      <Setter Property="MinHeight" Value="32"/>
      <Setter Property="FontWeight" Value="Bold"/>
      <Setter Property="Foreground" Value="{ThemeResource GroupLabelBrush}"/>
      <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="Button">
        <Grid Background="Transparent">
          <VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates">
            <VisualState x:Name="Normal"/>
            <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="Caption.Foreground" Value="{ThemeResource GroupHoverBrush}"/></VisualState.Setters></VisualState>
            <VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="Caption.Foreground" Value="{ThemeResource GroupHoverBrush}"/></VisualState.Setters></VisualState>
            <VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="Caption.Opacity" Value="0.45"/></VisualState.Setters></VisualState>
          </VisualStateGroup></VisualStateManager.VisualStateGroups>
          <ContentPresenter x:Name="Caption" Content="{TemplateBinding Content}" Foreground="{TemplateBinding Foreground}" FontWeight="{TemplateBinding FontWeight}" Margin="{TemplateBinding Padding}" HorizontalAlignment="Left" VerticalAlignment="Center"/>
        </Grid>
      </ControlTemplate></Setter.Value></Setter>
    </Style>
    <Style x:Key="ManagerButtonStyle" TargetType="Button">
      <Setter Property="Template" Value="{StaticResource ActionFace}"/>
      <Setter Property="UseSystemFocusVisuals" Value="True"/>
      <Setter Property="Background" Value="{ThemeResource SystemControlBackgroundBaseLowBrush}"/>
      <Setter Property="BorderThickness" Value="0"/><Setter Property="MinHeight" Value="32"/>
      <Setter Property="CornerRadius" Value="5"/><Setter Property="Padding" Value="14,7"/>
    </Style>
    <Style TargetType="Button" BasedOn="{StaticResource ManagerButtonStyle}"/>
    <Style TargetType="TextBox">
      <Setter Property="Padding" Value="10,4"/><Setter Property="MinHeight" Value="32"/>
      <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="TextBox">
        <Grid Background="Transparent">
          <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
              <VisualState x:Name="Normal"/>
              <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="FieldTint.Opacity" Value="0.09"/></VisualState.Setters></VisualState>
              <VisualState x:Name="Focused"><VisualState.Setters><Setter Target="Underline.Height" Value="2"/><Setter Target="Underline.Background" Value="{StaticResource AnimaAccent}"/><Setter Target="Underline.Opacity" Value="1"/></VisualState.Setters></VisualState>
              <VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="ContentElement.Opacity" Value="0.4"/></VisualState.Setters></VisualState>
            </VisualStateGroup>
          </VisualStateManager.VisualStateGroups>
          <Border x:Name="FieldTint" Background="{TemplateBinding Foreground}" CornerRadius="4" Opacity="0.055"/>
          <ScrollViewer x:Name="ContentElement" Margin="{TemplateBinding Padding}" VerticalAlignment="Center" IsTabStop="False" ZoomMode="Disabled" HorizontalScrollBarVisibility="Hidden" VerticalScrollBarVisibility="Hidden"/>
          <ContentPresenter x:Name="PlaceholderTextContentPresenter" Margin="{TemplateBinding Padding}" Content="{TemplateBinding PlaceholderText}" Opacity="0.65" Foreground="{TemplateBinding Foreground}" VerticalAlignment="Center" IsHitTestVisible="False"/>
          <Border x:Name="Underline" VerticalAlignment="Bottom" Height="1" Margin="2,0" Background="{TemplateBinding Foreground}" Opacity="0.5" IsHitTestVisible="False"/>
        </Grid>
      </ControlTemplate></Setter.Value></Setter>
    </Style>
    <Style TargetType="ListViewItem"><Setter Property="CornerRadius" Value="5"/><Setter Property="Margin" Value="0,1"/><Setter Property="Padding" Value="4,2"/></Style>
    <ControlTemplate x:Key="PetRowFace" TargetType="ListViewItem">
      <Grid Background="Transparent" MinHeight="40">
        <VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates">
          <VisualState x:Name="Normal"/>
          <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.05"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.04"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Selected"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.08"/></VisualState.Setters></VisualState>
          <VisualState x:Name="PointerOverSelected"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.11"/></VisualState.Setters></VisualState>
          <VisualState x:Name="PressedSelected"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.07"/></VisualState.Setters></VisualState>
        </VisualStateGroup></VisualStateManager.VisualStateGroups>
        <Border x:Name="Surface" CornerRadius="5" Background="{TemplateBinding Foreground}" Opacity="0"/>
        <ContentPresenter Content="{TemplateBinding Content}" VerticalAlignment="Center" HorizontalContentAlignment="Stretch" Margin="4,2"/>
      </Grid>
    </ControlTemplate>
    <ControlTemplate x:Key="PillSwitch" TargetType="ToggleButton">
      <Grid Background="Transparent" Width="44" Height="32">
        <VisualStateManager.VisualStateGroups>
          <VisualStateGroup x:Name="ChoiceStates">
            <VisualStateGroup.Transitions>
              <VisualTransition GeneratedDuration="0:0:0.16"><VisualTransition.GeneratedEasingFunction><CubicEase EasingMode="EaseOut"/></VisualTransition.GeneratedEasingFunction></VisualTransition>
            </VisualStateGroup.Transitions>
            <VisualState x:Name="AnimaOff"><Storyboard>
              <DoubleAnimation Storyboard.TargetName="ThumbSlide" Storyboard.TargetProperty="X" To="0" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="OnTrack" Storyboard.TargetProperty="Opacity" To="0" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="CheckedKnob" Storyboard.TargetProperty="Opacity" To="0" Duration="0"/>
            </Storyboard></VisualState>
            <VisualState x:Name="AnimaOn"><Storyboard>
              <DoubleAnimation Storyboard.TargetName="ThumbSlide" Storyboard.TargetProperty="X" To="20" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="OnTrack" Storyboard.TargetProperty="Opacity" To="1" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="CheckedKnob" Storyboard.TargetProperty="Opacity" To="1" Duration="0"/>
            </Storyboard></VisualState>
          </VisualStateGroup>
          <VisualStateGroup x:Name="CommonStates"><VisualStateGroup.Transitions><VisualTransition GeneratedDuration="0:0:0.08"/></VisualStateGroup.Transitions>
            <VisualState x:Name="Normal"><Storyboard>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleX" To="1" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleY" To="1" Duration="0"/>
            </Storyboard></VisualState>
            <VisualState x:Name="PointerOver"><Storyboard>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleX" To="1.16" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleY" To="1.16" Duration="0"/>
            </Storyboard></VisualState>
            <VisualState x:Name="Pressed"><Storyboard>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleX" To="1.45" Duration="0"/>
              <DoubleAnimation Storyboard.TargetName="ThumbScale" Storyboard.TargetProperty="ScaleY" To="1.16" Duration="0"/>
            </Storyboard></VisualState>
          </VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
        <Border Height="20" Width="42" CornerRadius="10" BorderThickness="1" BorderBrush="{TemplateBinding Foreground}" Opacity="0.65"/>
        <Border x:Name="OnTrack" Height="20" Width="42" CornerRadius="10" Background="{TemplateBinding Background}" Opacity="0"/>
        <Grid Width="12" Height="12" Margin="6,0,0,0" HorizontalAlignment="Left" VerticalAlignment="Center">
          <Grid.RenderTransform><TranslateTransform x:Name="ThumbSlide"/></Grid.RenderTransform>
          <Grid RenderTransformOrigin="0.5,0.5">
            <Grid.RenderTransform><ScaleTransform x:Name="ThumbScale"/></Grid.RenderTransform>
            <Border CornerRadius="6" Background="{TemplateBinding Foreground}"/>
            <Border x:Name="CheckedKnob" CornerRadius="6" Background="White" Opacity="0"/>
          </Grid>
        </Grid>
      </Grid>
    </ControlTemplate>
    <ControlTemplate x:Key="RadioPill" TargetType="RadioButton">
      <Grid Background="Transparent" MinHeight="34">
        <VisualStateManager.VisualStateGroups>
          <VisualStateGroup x:Name="ChoiceStates"><VisualState x:Name="AnimaOff"/><VisualState x:Name="AnimaOn"><VisualState.Setters>
            <Setter Target="Selected.Opacity" Value="0.16"/><Setter Target="Dot.Opacity" Value="1"/><Setter Target="Ring.BorderBrush" Value="{StaticResource AnimaAccent}"/>
          </VisualState.Setters></VisualState></VisualStateGroup>
          <VisualStateGroup x:Name="CommonStates"><VisualState x:Name="Normal"/><VisualState x:Name="PointerOver"><VisualState.Setters>
            <Setter Target="Hover.Opacity" Value="0.09"/>
          </VisualState.Setters></VisualState><VisualState x:Name="Pressed"><VisualState.Setters>
            <Setter Target="Hover.Opacity" Value="0.15"/>
          </VisualState.Setters></VisualState></VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
        <Border x:Name="Hover" CornerRadius="6" Background="{TemplateBinding Foreground}" Opacity="0"/>
        <Border x:Name="Selected" CornerRadius="6" Background="{TemplateBinding Background}" Opacity="0"/>
        <StackPanel Orientation="Horizontal" Spacing="6" Margin="8,0" VerticalAlignment="Center">
          <Border x:Name="Ring" Width="16" Height="16" CornerRadius="8" BorderThickness="1" BorderBrush="{TemplateBinding Foreground}">
            <Border x:Name="Dot" Width="8" Height="8" CornerRadius="4" Background="{TemplateBinding Background}" Opacity="0"/>
          </Border>
          <ContentPresenter Content="{TemplateBinding Content}" VerticalAlignment="Center"/>
        </StackPanel>
      </Grid>
    </ControlTemplate>
    </ResourceDictionary>
  </Grid.Resources>
  <Grid.ColumnDefinitions><ColumnDefinition Width="234"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
  <Grid Margin="12">
    <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
    <Grid x:Name="LibraryGroup" Grid.Row="0" Margin="0,0,0,6">
      <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition x:Name="MediaListRow" Height="*"/><RowDefinition Height="Auto"/><RowDefinition x:Name="ProfilesRow" Height="Auto"/></Grid.RowDefinitions>
      <Button x:Name="MediaHeader" Style="{StaticResource SidebarGroupButton}" AutomationProperties.Name="Media library">
        <StackPanel Orientation="Horizontal" Spacing="8">
          <FontIcon x:Name="MediaChevron" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="12" Glyph="&#xE76C;" RenderTransformOrigin="0.5,0.5"><FontIcon.RenderTransform><RotateTransform x:Name="MediaAngle" Angle="90"/></FontIcon.RenderTransform></FontIcon>
          <TextBlock Text="Media library" FontWeight="Bold" VerticalAlignment="Center"/>
        </StackPanel>
      </Button>
      <Grid x:Name="MediaPane" Grid.Row="1">
        <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="*"/></Grid.RowDefinitions>
        <Grid x:Name="LibrarySearchField" Margin="0,4,0,0">
        <TextBox x:Name="Search" PlaceholderText="Search media"/>
      </Grid>
        <ListView x:Name="Pets" Grid.Row="1" Margin="0,6,0,0" SelectionMode="Single">
          <ListView.ItemContainerTransitions><TransitionCollection><AddDeleteThemeTransition/><ReorderThemeTransition/></TransitionCollection></ListView.ItemContainerTransitions>
        </ListView>
      </Grid>
      <Button x:Name="ProfilesHeader" Grid.Row="2" Style="{StaticResource SidebarGroupButton}" AutomationProperties.Name="Profiles">
        <StackPanel Orientation="Horizontal" Spacing="8">
          <FontIcon x:Name="ProfilesChevron" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="12" Glyph="&#xE76C;" RenderTransformOrigin="0.5,0.5"><FontIcon.RenderTransform><RotateTransform x:Name="ProfilesAngle" Angle="0"/></FontIcon.RenderTransform></FontIcon>
          <TextBlock Text="Profiles" FontWeight="Bold" VerticalAlignment="Center"/>
        </StackPanel>
      </Button>
      <StackPanel x:Name="ProfileLinks" Grid.Row="3" Spacing="2" Visibility="Collapsed"/>
    </Grid>
    <StackPanel x:Name="NavBottom" Grid.Row="1" Spacing="3">
      <Button x:Name="Workshop" Height="36" AutomationProperties.Name="Workshop" HorizontalAlignment="Stretch" HorizontalContentAlignment="Left" Padding="14,0" BorderThickness="0">
        <StackPanel Orientation="Horizontal" Spacing="10"><FontIcon Glyph="&#xE719;" FontSize="16"/><TextBlock Text="Workshop"/></StackPanel>
      </Button>
      <Button x:Name="App" Height="36" AutomationProperties.Name="App settings" HorizontalAlignment="Stretch" HorizontalContentAlignment="Left" Padding="14,0" BorderThickness="0">
        <StackPanel Orientation="Horizontal" Spacing="10"><FontIcon Glyph="&#xE713;" FontSize="16"/><TextBlock Text="App settings"/></StackPanel>
      </Button>
    </StackPanel>
    <StackPanel Grid.Row="2" Margin="0,8,0,0" Spacing="6">
      <Grid ColumnSpacing="2">
        <Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="34"/></Grid.ColumnDefinitions>
        <Button x:Name="Import" Content="Import media…" HorizontalAlignment="Stretch"/>
        <Button x:Name="ImportOptions" Grid.Column="1" AutomationProperties.Name="Import options" HorizontalAlignment="Stretch" Padding="0">
          <FontIcon Glyph="&#xE70D;" FontSize="11"/>
        </Button>
      </Grid>
      <TextBlock x:Name="Build" Opacity="0.7" TextAlignment="Center" FontSize="11" TextWrapping="Wrap"/>
    </StackPanel>
    <Canvas x:Name="NavOverlay" Grid.RowSpan="3" IsHitTestVisible="False">
      <Border x:Name="NavPill" Width="3" Height="16" CornerRadius="1.5" Opacity="0" Background="{StaticResource AnimaAccent}">
        <Border.RenderTransform><CompositeTransform x:Name="NavTransform"/></Border.RenderTransform>
      </Border>
    </Canvas>
  </Grid>
  <Grid x:Name="PagePanel" Grid.Column="1" Margin="24,20,16,0">
    <Grid.RenderTransform><TranslateTransform x:Name="PageOffset"/></Grid.RenderTransform>
    <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="*"/></Grid.RowDefinitions>
    <TextBlock x:Name="Title" FontSize="28" FontWeight="SemiBold" Margin="0,0,0,20" TextTrimming="CharacterEllipsis"/>
    <ScrollViewer x:Name="Scroll" Grid.Row="1" HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto">
      <StackPanel x:Name="Body" Spacing="12" Padding="0,0,12,16"/>
    </ScrollViewer>
  </Grid>
</Grid>)XAML")
                      .as<controls::Grid>();
    managerRoot.GotFocus(FadeTabFocus);
    managerRoot.LayoutUpdated([hwnd](auto&&, auto&&) {
        if (managerRoot)
            PositionNavPill(hwnd);
    });
    Named<Button>(L"MediaHeader").Click([hwnd](auto&&, auto&&) {
        SelectSidebarGroup(hwnd, false);
    });
    Named<Button>(L"ProfilesHeader").Click([hwnd](auto&&, auto&&) {
        SelectSidebarGroup(hwnd, true);
    });
    managerSource.Content(managerRoot);
    PopulateProfiles();
    RECT client{};
    GetClientRect(hwnd, &client);
    SetWindowPos(islandWindow, nullptr, 0, 0, client.right, client.bottom,
                 SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    appliedTheme = -1;
    StyleManager(hwnd);
    LoadBuildInfo();
    Named<Button>(L"Workshop").Click([](auto&&, auto&&) {
        Navigate(Page::Workshop);
    });
    Named<Button>(L"App").Click([](auto&&, auto&&) { Navigate(Page::App); });
    auto importMenu = ActionDropdown(
        controls::Primitives::FlyoutPlacementMode::TopEdgeAlignedRight);
    AddDropdownAction(importMenu, L"from images",
                      [] { PostMessageW(g_hManagerWnd, WM_APP + 33, 0, 0); });
    AddDropdownAction(importMenu, L"from folder",
                      [] { PostMessageW(g_hManagerWnd, WM_APP + 33, 1, 0); });
    Named<Button>(L"ImportOptions").Flyout(importMenu);
    Named<Button>(L"Import").Click([](auto&&, auto&&) { AddImages(); });
    AddSearchIcon(Named<controls::Grid>(L"LibrarySearchField"),
                  Named<controls::TextBox>(L"Search"));
    Named<controls::TextBox>(L"Search").TextChanged(
        [](auto&&, auto&&) { PetList(); });
    Named<controls::ListView>(L"Pets").SelectionChanged(
        [](auto const& sender, controls::SelectionChangedEventArgs const&) {
            if (updatingList)
                return;
            auto item = sender.template as<controls::ListView>().SelectedItem();
            if (!item || !managerRoot)
                return;
            int index = winrt::unbox_value<int>(
                item.template as<controls::ListViewItem>().Tag());
            if (index < 0) {
                auto& folder = libraryFolders.at(-index - 1);
                folder.expanded = !folder.expanded;
                DWORD value = folder.expanded;
                if (configWritable)
                    PutNumber(Record(folder.key, L"Expanded"), value);
                PostMessageW(g_hManagerWnd, WM_APP + 34, static_cast<WPARAM>(-1 - index), 0);
                return;
            }
            Navigate(Page::Pet, index);
        });
    page = Page::Pet;
    g_managerSel = g_items.empty() ? -1 : 0;
    PetList();
    RenderPage();
}
static void CloseManagerIsland() {
    navMotion = NavigationMotion{};
    sidebarMotion = {};
    managerInput = nullptr;
    islandWindow = nullptr;
    if (managerSource) {
        managerSource.Content(nullptr);
        managerSource.Close();
    }
    managerRoot = nullptr;
    managerSource = nullptr;
}
static LRESULT CALLBACK ManagerWndProc(HWND hwnd,
                                       UINT msg,
                                       WPARAM wp,
                                       LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE && g_hotkeyCapturing) {
                    g_hotkeyCapturing = false;
                    StopKeyCapture();
                    ReapplyHotkey();
                    PostMessageW(hwnd, WM_APP + 20, 0, 0);
                }
                break;
            case WM_TIMER:
                if (wp == kGroupTimer && managerRoot) {
                    StepSidebar(hwnd);
                    return 0;
                }
                if (wp == kNavTimer) {
                    StepNavPill(hwnd);
                    return 0;
                }
                break;
            case WM_CREATE:
                BuildManager(hwnd);
                DragAcceptFiles(hwnd, TRUE);
                if (islandWindow &&
                    SetWindowSubclass(islandWindow, ForwardDroppedMedia, 1,
                                      reinterpret_cast<DWORD_PTR>(hwnd)))
                    DragAcceptFiles(islandWindow, TRUE);
                return 0;
            case WM_DROPFILES:
                ImportDroppedMedia(reinterpret_cast<HDROP>(wp));
                return 0;
            case WM_APP + 42:
                if (managerRoot && page == Page::Workshop)
                    RenderPage();
                return 0;
            case WM_APP + 34:
                if (managerRoot)
                    PetList(static_cast<int>(wp));
                return 0;
            case WM_APP + 33:
                if (wp)
                    ImportFolder();
                else
                    AddImages();
                return 0;
            case WM_SIZE:
                if (islandWindow)
                    SetWindowPos(
                        islandWindow, nullptr, 0, 0, LOWORD(lp), HIWORD(lp),
                        SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
                return 0;
            case WM_DPICHANGED: {
                auto r = reinterpret_cast<RECT*>(lp);
                SetWindowPos(hwnd, nullptr, r->left, r->top, r->right - r->left,
                             r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
                return 0;
            }
            case WM_GETMINMAXINFO: {
                UINT dpi = GetDpiForWindow(hwnd);
                reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize = {
                    MulDiv(660, dpi, 96), MulDiv(490, dpi, 96)};
                return 0;
            }
            case WM_SETFOCUS:
                if (managerSource)
                    managerSource.NavigateFocus(
                        hosting::XamlSourceFocusNavigationRequest(
                            hosting::XamlSourceFocusNavigationReason::First));
                return 0;
            case WM_COMMAND:
                if (LOWORD(wp) == IDC_NAV_WORKSHOP && managerRoot)
                    Navigate(Page::Workshop);
                return 0;
            case WM_APP_OPEN_MANAGER:
                if ((size_t)wp < g_items.size()) {
                    Named<controls::TextBox>(L"Search").Text(L"");
                    Navigate(Page::Pet, static_cast<int>(wp));
                    PetList();
                }
                return 0;
            case WM_APP_RELOAD:
                PetList();
                RenderPage();
                return 0;
            case WM_THEMECHANGED:
            case WM_SETTINGCHANGE:
                if (managerRoot)
                    PostMessageW(hwnd, kApplyManagerAppearance, 0, 0);
                return 0;
            case kApplyManagerAppearance:
                if (managerRoot) {
                    if (StyleManager(hwnd) || wp)
                        RenderPage();
                }
                return 0;
            case WM_APP + 20:
                if (page == Page::App && managerRoot)
                    RenderPage();
                return 0;
            case WM_DESTROY:
                StopKeyCapture();
                if (g_hotkeyCapturing) {
                    g_hotkeyCapturing = false;
                    ReapplyHotkey();
                }
                CloseManagerIsland();
                g_hManagerWnd = nullptr;
                return 0;
        }
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Manager message %u failed (0x%08X): %s", msg,
               (unsigned)error.code().value, error.message().c_str());
        if (msg == WM_CREATE) {
            wchar_t code[16]{};
            swprintf_s(code, L"0x%08X",
                       static_cast<unsigned>(error.code().value));
            managerStartupError = std::wstring(managerStartupStage) + L" (" +
                                  code + L")\n\n" + error.message().c_str();
            Wh_Log(L"[Manager] %s", managerStartupError.c_str());
            CloseManagerIsland();
            return -1;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================================
// Embedded project logo shared by the tray and both windows.
// ============================================================================

static std::wstring taskbarIconPath;
static bool taskbarLaunch = false;

static void CacheTaskbarIcon(const D2D1_MAPPED_RECT& pixels, UINT size) {
    if (userFiles.empty() || !size || size > 255)
        return;
    // An ICO directory followed by a bottom-up 32-bit DIB and its AND mask.
    DWORD stride = size * 4, maskStride = ((size + 31) / 32) * 4;
    DWORD imageBytes = sizeof(BITMAPINFOHEADER) + (stride + maskStride) * size;
    std::string ico;
    auto append = [&](const auto& v) {
        ico.append(reinterpret_cast<const char*>(&v), sizeof(v));
    };
    append(WORD{0});
    append(WORD{1});
    append(WORD{1});
    append(static_cast<BYTE>(size));
    append(static_cast<BYTE>(size));
    append(WORD{0});
    append(WORD{1});
    append(WORD{32});
    append(imageBytes);
    append(DWORD{22});
    BITMAPINFOHEADER header{};
    header.biSize = sizeof(header);
    header.biWidth = size;
    header.biHeight = size * 2;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biSizeImage = stride * size;
    append(header);
    std::string mask(maskStride * size, '\0');
    for (UINT y = 0; y < size; ++y) {
        auto row = pixels.bits + (size - 1 - y) * pixels.pitch;
        for (UINT x = 0; x < size; ++x) {
            const auto* pixel = row + 4 * x;
            BYTE a = pixel[3];
            if (!a)
                mask[y * maskStride + x / 8] |=
                    static_cast<char>(0x80 >> (x % 8));
            for (int c = 0; c < 3; ++c)
                append(static_cast<BYTE>(
                    a ? std::min(255, (pixel[c] * 255 + a / 2) / a) : 0));
            append(a);
        }
    }
    ico += mask;
    auto path = userFiles + L"\\manager.ico";
    if (ReadSmallFile(path) == ico || WriteSmallFile(path, ico))
        taskbarIconPath = path;
}

static void SetTaskbarIdentity(HWND hwnd) {
    if (!hwnd || taskbarIconPath.empty())
        return;
    wchar_t exe[32768]{};
    DWORD n = GetModuleFileNameW(nullptr, exe, std::size(exe));
    if (!n || n >= std::size(exe))
        return;
    winrt::com_ptr<IPropertyStore> props;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(props.put()))))
        return;
    // System.AppUserModel properties: relaunch command, icon, display name and
    // ID.
    const GUID appModel = {0x9f4c2855,
                           0x9f79,
                           0x4b39,
                           {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}};
    auto set = [&](DWORD id, const std::wstring& value) {
        PROPERTYKEY key{appModel, id};
        PROPVARIANT pv{};
        pv.vt = VT_LPWSTR;
        pv.pwszVal = const_cast<LPWSTR>(value.c_str());
        return props->SetValue(key, pv);
    };
    auto command = std::wstring(L"\"") + exe +
                   L"\" -tool-mod \"" WH_MOD_ID L"\" -anima-manager";
    if (FAILED(set(2, command)) || FAILED(set(3, taskbarIconPath + L",0")) ||
        FAILED(set(4, L"AnimaWindhawkEngine")) ||
        FAILED(set(5, L"TuBeo5866.AnimaWindhawkEngine.Manager")) ||
        FAILED(props->Commit()))
        Wh_Log(L"Unable to set the manager taskbar icon properties");
}

static HICON LoadManagerIcon() {
    static constexpr char logo[] =
        R"svg(<svg viewBox="0 0 64 64" version="1.1" xmlns="http://www.w3.org/2000/svg" >
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 55.5 1 L 56 5.5 L 55 5.5 L 55.5 1 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 23.5 7 L 25 9.5 L 24 9.5 L 23.5 7 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 7 13 L 9 13.5 L 8 18 L 6 17.5 L 7 13 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 27.5 19 L 28 21.5 L 27 21.5 L 27.5 19 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 11.5 20 L 21 20.5 L 11.5 21 L 11.5 20 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 56.5 20 L 57 22.5 L 56 22.5 L 56.5 20 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 34.5 25 L 35 28.5 L 34 28.5 L 34.5 25 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 23.5 26 L 24 29.5 L 23 29.5 L 23.5 26 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 8.5 29 L 7.5 31 L 8.5 29 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 53.5 30 L 54 32.5 L 53 32.5 L 53.5 30 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 4.5 39 L 5 63.5 L 4 63.5 L 4.5 39 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 25.5 39 L 26 46.5 L 25 46.5 L 25.5 39 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 44.5 40 L 45 47.5 L 44 47.5 L 44.5 40 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 44.5 52 L 45 59.5 L 44 59.5 L 44.5 52 Z " />
<path fill="rgb(0,1,1)" stroke="rgb(0,1,1)" stroke-width="1" opacity="0.06666666666666667" d="M 25.5 53 L 26 63.5 L 25 63.5 L 25.5 53 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 14.5 0 Q 20.5 -1.5 22 1.5 L 20.5 1 Q 15 -0.5 13.5 2 L 14.5 0 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 12.5 3 L 13 5.5 L 11 6.5 L 12.5 3 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 22.5 3 L 23 7.5 L 22 7.5 L 22.5 3 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 23.5 9 L 26 13.5 L 26 17.5 L 25 17.5 L 25 13.5 Q 21.9 12.4 23.5 9 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 9.5 10 L 10 13.5 L 9 13.5 L 9.5 10 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 13.5 10 L 14 12.5 L 11 17.5 L 11 21 L 21 21 L 21 17.5 Q 18.3 16.8 19 12.5 L 20 12.5 L 22 20.5 L 20.5 22 L 11.5 22 Q 9.3 21.3 10 17.5 Q 13.3 15.3 13.5 10 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 8.5 15 L 9 17.5 L 7 19.5 L 7 22.5 Q 3.6 24.6 3.5 30 L 3 26.5 L 6 22.5 L 6 19.5 L 8.5 15 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 26.5 19 Q 25.9 24.1 29 25.5 L 26 23.5 L 26.5 19 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 11.5 25 L 22 25.5 L 11.5 26 L 11.5 25 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 23.5 25 L 25 28.5 L 24 28.5 L 23.5 25 Z " />
<path fill="rgb(104,70,182)" stroke="rgb(104,70,182)" stroke-width="1" opacity="0.9019607843137255" d="M 6.5 30 L 7 33.5 L 3.5 35 L 2 31.5 L 3 31.5 Q 1.9 35.7 6 34 L 6.5 30 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 36.5 0 L 35.5 2 L 36.5 0 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 57.5 0 L 56.5 2 L 57.5 0 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 15 5 Q 18 3.8 17 7.5 L 15 8.5 L 15 5 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 10.5 6 L 11 8.5 L 10 8.5 L 10.5 6 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 55.5 6 L 54.5 8 L 55.5 6 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 60.5 7 L 59.5 9 L 60.5 7 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 43.5 8 L 44.5 10 L 43.5 8 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 38.5 12 L 39.5 14 L 38 13.5 L 37.5 15 L 37 13.5 L 38.5 12 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 37.5 17 L 36.5 19 L 37.5 17 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 52.5 17 L 51.5 19 L 52.5 17 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 57.5 17 L 56.5 19 L 57.5 17 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 42.5 23 L 43.5 25 L 42.5 23 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 54.5 28 L 53.5 30 L 54.5 28 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 52.5 33 L 51.5 35 L 52.5 33 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 4.5 36 L 5 38.5 L 4 38.5 L 4.5 36 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 25.5 36 L 26 38.5 L 25 38.5 L 25.5 36 Z " />
<path fill="rgb(91,78,126)" stroke="rgb(91,78,126)" stroke-width="1" opacity="0.5490196078431373" d="M 55.5 52 L 58 52.5 L 55.5 53 L 55.5 52 Z " />
<path fill="rgb(0,0,0)" stroke="rgb(0,0,0)" stroke-width="1" opacity="0" d="M 0 0 L 13 0.5 L 12 6 Q 9 4.8 10 8.5 L 9 13 Q 5.7 11.7 7 16.5 L 6 21.5 L 4 23.5 L 4.5 25 Q 1.5 24.1 3 28.5 L 2 35 L 7 35 L 7 31.5 Q 10.1 29.8 8.5 28 L 10 28 L 10.5 26 L 23 26 L 25 35 L 33 35 L 33 30.5 L 35 28.5 L 36 19 Q 40 20.4 38 13.5 L 41 15.5 L 42 25 Q 46 23.6 44 30.5 L 45 35 Q 52.1 36.8 54 32.5 L 55 23 Q 58 24.3 57 20.5 L 56.5 19 Q 59.5 19.9 58 15.5 L 62 0 L 64 0.5 L 64 64 L 60 64 L 59.5 60 L 45 60 L 45 52 Q 53.4 51 58 53 L 58 48 L 45 48 L 45 40 L 59.5 40 L 60 36 L 39 36 L 39 63.5 L 38.5 64 L 31 64 L 31 36.5 L 30.5 36 L 25 36 L 25 48 L 10 48 L 10 36 L 4 36 L 4 63.5 L 0 64 L 0 0 Z " />
<path fill="rgb(0,0,0)" stroke="rgb(0,0,0)" stroke-width="1" opacity="0" d="M 22.5 0 L 36 0.5 L 33 10.5 L 34 13 Q 30 11.6 32 18.5 L 31 25 L 29 25 Q 30.1 22.3 27.5 23 L 28 21.5 L 26 16.5 L 26 12.5 L 24 10.5 L 25 8.5 L 23 6.5 L 22.5 0 Z " />
<path fill="rgb(0,0,0)" stroke="rgb(0,0,0)" stroke-width="1" opacity="0" d="M 42 0 L 57 0.5 L 55 1 L 54 12.5 L 52 15.5 L 52.5 17 Q 49.8 16.4 51 22.5 L 49.5 29 L 48 19 Q 44 20.4 46 13.5 L 43 7.5 L 43 5.5 L 42 0 Z " />
<path fill="rgb(0,0,0)" stroke="rgb(0,0,0)" stroke-width="1" opacity="0" d="M 14 10 Q 19.2 7.9 18 12.5 L 21 20 L 11.5 20 L 11 18.5 Q 15.1 16.2 14 10 Z " />
<path fill="rgb(0,0,0)" stroke="rgb(0,0,0)" stroke-width="1" opacity="0" d="M 11.5 52 L 25 52 L 25 64 L 10 64 L 10 53.5 L 11.5 52 Z " />
<path fill="rgb(127,126,128)" stroke="rgb(127,126,128)" stroke-width="1" opacity="0.984313725490196" d="M 37.5 0 Q 41.3 -0.8 42 1.5 L 43 8.5 L 46 14.5 L 46 18.5 L 48 20.5 L 49.5 30 L 51 19.5 L 54 13.5 L 54 8.5 Q 57 7.5 56 2.5 L 58.5 0 L 62 1.5 L 61 6.5 L 59 8.5 L 58 16.5 L 56 18.5 L 55 27.5 L 50.5 35 Q 46 36 45 33.5 L 44 24.5 L 42 22.5 L 41 14.5 L 38.5 12 L 37 14.5 L 38 16.5 L 36 18.5 L 33 33.5 L 31.5 35 Q 26.3 36.3 25 33.5 Q 24 27 28.5 26 L 31 24.5 L 32 14.5 L 34 12.5 L 35 7.5 L 35 2.5 L 37.5 0 Z " />
<path fill="rgb(127,126,128)" stroke="rgb(127,126,128)" stroke-width="1" opacity="0.984313725490196" d="M 5 37 L 9 37 L 9 47.5 L 10.5 49 L 24.5 49 Q 28.8 47.3 27 39.5 L 26 37 L 30 37.5 L 30 64 L 27 63.5 L 27 53.5 L 24.5 51 L 11.5 51 L 9 52 L 9 63.5 L 6 64 L 6 39.5 L 5 37 Z " />
<path fill="rgb(144,98,252)" stroke="rgb(144,98,252)" stroke-width="1" opacity="0.996078431372549" d="M 14.5 1 Q 20.5 -0.5 22 2.5 L 22 7.5 L 25 13.5 L 26 23.5 L 28 26.5 L 25 28.5 L 23.5 25 L 10.5 25 L 7 28.5 L 6 34 Q 1.5 35.8 3 30.5 L 7 22.5 L 7 19.5 L 9 17.5 L 10 9.5 L 14.5 1 Z M 15 4 L 15 9 L 10 18 Q 9 21 12 22 L 21 22 L 22 21 L 20 13 L 17 9 Q 19 3 15 4 Z " />
<path fill="rgb(43,86,126)" stroke="rgb(43,86,126)" stroke-width="1" opacity="0.996078431372549" d="M 40 37 L 58.5 37 L 59 38.5 Q 57.8 41.3 56.5 39 L 45.5 39 L 44 40.5 Q 42.3 47.3 45.5 49 L 56.5 49 L 57 52 L 44 52 L 44 59.5 L 45.5 61 L 59 61 L 58.5 64 L 40 64 L 40 37 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 5.5 36 Q 9.3 35.3 10 37.5 L 10 48 L 24.5 48 L 24.5 49 L 10.5 49 L 9 47.5 L 9 37.5 L 5.5 37 L 5.5 36 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 26.5 36 Q 30.3 35.3 31 37.5 L 30.5 64 L 30 37 Q 25.6 38 26.5 36 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 5.5 39 L 6 63.5 L 5 63.5 L 5.5 39 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 26.5 39 L 27 46.5 L 26 46.5 L 26.5 39 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 11.5 51 L 25 51.5 L 11.5 52 L 11.5 51 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 9.5 52 L 10 63.5 L 9 63.5 L 9.5 52 Z " />
<path fill="rgb(93,93,93)" stroke="rgb(93,93,93)" stroke-width="1" opacity="0.8941176470588236" d="M 26.5 53 L 27 63.5 L 26 63.5 L 26.5 53 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 16.5 8 L 17.5 10 L 16.5 8 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 10.5 25 L 8.5 28 L 10.5 25 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 39 36 L 58.5 36 L 60 37.5 L 58.5 40 L 59 38.5 L 58.5 37 L 40 37 L 39.5 64 L 39 36 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 45.5 39 L 57 39.5 L 45.5 40 L 45.5 39 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 45.5 48 L 56.5 48 L 58 49.5 L 57.5 52 L 57 49 L 45.5 49 L 45.5 48 Z " />
<path fill="rgb(35,56,86)" stroke="rgb(35,56,86)" stroke-width="1" opacity="0.7686274509803922" d="M 45.5 60 L 58.5 60 L 60 61.5 L 58.5 61 L 45.5 61 L 45.5 60 Z " />
</svg>)svg";
    constexpr UINT size = 64;
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(reinterpret_cast<const BYTE*>(logo),
                                    sizeof(logo) - 1));
    if (!stream)
        return nullptr;

    // Render once in memory. WARP also works without a hardware GPU.
    winrt::com_ptr<ID3D11Device> d3d;
    winrt::com_ptr<IDXGIDevice> dxgi;
    winrt::com_ptr<ID2D1Factory1> factory;
    winrt::com_ptr<ID2D1Device> device;
    winrt::com_ptr<ID2D1DeviceContext> context;
    winrt::com_ptr<ID2D1DeviceContext5> svgContext;
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                 D3D11_SDK_VERSION, d3d.put(), nullptr,
                                 nullptr)) ||
        FAILED(d3d->QueryInterface(IID_PPV_ARGS(dxgi.put()))) ||
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory1), nullptr,
                                 factory.put_void())) ||
        FAILED(factory->CreateDevice(dxgi.get(), device.put())) ||
        FAILED(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                           context.put())) ||
        FAILED(context->QueryInterface(IID_PPV_ARGS(svgContext.put()))))
        return nullptr;

    winrt::com_ptr<ID2D1SvgDocument> document;
    winrt::com_ptr<ID2D1Bitmap1> target;
    winrt::com_ptr<ID2D1Bitmap1> readback;
    D2D1_BITMAP_PROPERTIES1 properties{};
    properties.pixelFormat = {DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED};
    properties.dpiX = properties.dpiY = 96;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    if (FAILED(svgContext->CreateSvgDocument(stream.get(), {size, size},
                                             document.put())) ||
        FAILED(context->CreateBitmap({size, size}, nullptr, 0, &properties,
                                     target.put())))
        return nullptr;

    context->SetTarget(target.get());
    context->BeginDraw();
    const D2D1_COLOR_F transparent{};
    context->Clear(&transparent);
    svgContext->DrawSvgDocument(document.get());
    HRESULT drawResult = context->EndDraw();
    context->SetTarget(nullptr);
    properties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    if (FAILED(drawResult) ||
        FAILED(context->CreateBitmap({size, size}, nullptr, 0, &properties,
                                     readback.put())) ||
        FAILED(readback->CopyFromBitmap(nullptr, target.get(), nullptr)))
        return nullptr;

    D2D1_MAPPED_RECT pixels{};
    if (FAILED(readback->Map(D2D1_MAP_OPTIONS_READ, &pixels)))
        return nullptr;
    HICON icon = nullptr;
    {
        Gdiplus::Bitmap bitmap(size, size, pixels.pitch, PixelFormat32bppPARGB,
                               pixels.bits);
        bitmap.GetHICON(&icon);
    }
    if (icon)
        CacheTaskbarIcon(pixels, size);
    readback->Unmap();
    return icon;
}

// ============================================================================
// Open (or focus) the manager window
// ============================================================================

static void OpenPetManager(int focusIndex) {
    if (g_hManagerWnd) {
        // Already open — just focus it and, optionally, select a pet
        ShowWindow(g_hManagerWnd, SW_RESTORE);
        SetForegroundWindow(g_hManagerWnd);
        if (focusIndex >= 0)
            PostMessageW(g_hManagerWnd, WM_APP_OPEN_MANAGER, (WPARAM)focusIndex,
                         0);
        return;
    }

    managerStartupError.clear();
    auto previousDpi = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    g_hManagerWnd = CreateWindowExW(
        WS_EX_APPWINDOW, kManagerWndClass, L"AnimaWindhawkEngine",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT,
        MulDiv(920, GetDpiForSystem(), 96), MulDiv(700, GetDpiForSystem(), 96),
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (previousDpi)
        SetThreadDpiAwarenessContext(previousDpi);
    if (!g_hManagerWnd) {
        MessageBoxW(g_hCtrlWnd,
                    managerStartupError.empty()
                        ? L"Windows could not create the manager window."
                        : managerStartupError.c_str(),
                    L"AnimaWindhawkEngine", MB_ICONERROR);
        return;
    }

    if (!g_hManagerIcon)
        g_hManagerIcon = LoadManagerIcon();
    if (g_hManagerIcon) {
        SendMessageW(g_hManagerWnd, WM_SETICON, ICON_BIG,
                     (LPARAM)g_hManagerIcon);
        SendMessageW(g_hManagerWnd, WM_SETICON, ICON_SMALL,
                     (LPARAM)g_hManagerIcon);
    }

    SetTaskbarIdentity(g_hManagerWnd);
    ShowWindow(g_hManagerWnd, SW_SHOW);
    UpdateWindow(g_hManagerWnd);
    SetForegroundWindow(g_hManagerWnd);

    if (focusIndex >= 0)
        PostMessageW(g_hManagerWnd, WM_APP_OPEN_MANAGER, (WPARAM)focusIndex, 0);
}

// ============================================================================
// Item window
// ============================================================================

static LRESULT CALLBACK ItemWndProc(HWND hwnd,
                                    UINT msg,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    PetItem* it = (PetItem*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_NCCREATE: {
            auto* cs = (CREATESTRUCTW*)lParam;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              (LONG_PTR)cs->lpCreateParams);
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
        case WM_LBUTTONDOWN:
            if (it && !it->lockPosition && !it->clickThrough) {
                SetCapture(hwnd);
                it->dragging = true;
                GetCursorPos(&it->dragStartCursor);
                RECT r{};
                GetWindowRect(hwnd, &r);
                it->dragOriginWnd = {r.left, r.top};
            }
            return 0;
        case WM_MOUSEWHEEL: {
            int mode = GetKeyState(VK_MENU) < 0                     ? 3
                       : (GET_KEYSTATE_WPARAM(wParam) & MK_SHIFT)   ? 2
                       : (GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL) ? 1
                                                                    : 0;
            if (!it || !mode)
                break;
            if (it->wheelMode != mode)
                it->wheelRemainder = 0;
            it->wheelMode = mode;
            it->wheelRemainder += GET_WHEEL_DELTA_WPARAM(wParam);
            int steps = it->wheelRemainder / WHEEL_DELTA;
            it->wheelRemainder %= WHEEL_DELTA;
            if (!steps)
                return 0;
            if (mode == 1)
                it->scalePercent =
                    std::clamp(it->scalePercent + 5 * steps, 1, 500);
            else if (mode == 2)
                it->rotationDegrees =
                    ((it->rotationDegrees + 5 * steps) % 360 + 360) % 360;
            else
                it->opacityPercent =
                    std::clamp(it->opacityPercent + 5 * steps, 10, 100);
            SyncImageState(*it, false);
            RenderItem(*it);
            if (managerRoot && page == Page::Pet && SelectedPet() == it)
                RenderPage();
            return 0;
        }
        case WM_MOUSEMOVE:
            if (it && it->dragging) {
                POINT cur{};
                GetCursorPos(&cur);
                int dx = cur.x - it->dragStartCursor.x;
                int dy = cur.y - it->dragStartCursor.y;
                POINT pos{it->dragOriginWnd.x + dx, it->dragOriginWnd.y + dy};
                RECT rect{};
                MONITORINFO monitor{sizeof(monitor)};
                if (snapEdges && GetWindowRect(hwnd, &rect) &&
                    GetMonitorInfoW(
                        MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST),
                        &monitor)) {
                    const RECT& work = monitor.rcWork;
                    LONG right = work.right - (rect.right - rect.left);
                    LONG bottom = work.bottom - (rect.bottom - rect.top);
                    int tolerance = MulDiv(12, GetDpiForWindow(hwnd), 96);
                    if (labs(pos.x - work.left) <= tolerance)
                        pos.x = work.left;
                    else if (labs(pos.x - right) <= tolerance)
                        pos.x = right;
                    if (labs(pos.y - work.top) <= tolerance)
                        pos.y = work.top;
                    else if (labs(pos.y - bottom) <= tolerance)
                        pos.y = bottom;
                }
                SetWindowPos(hwnd, nullptr, pos.x, pos.y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        case WM_CAPTURECHANGED:
        case WM_LBUTTONUP:
            if (it && it->dragging) {
                it->dragging = false;
                if (msg == WM_LBUTTONUP && GetCapture() == hwnd)
                    ReleaseCapture();
                RECT r{};
                if (!GetWindowRect(hwnd, &r))
                    return 0;
                it->posX = r.left;
                it->posY = r.top;
                if (it->isGif && it->monitorHz != MonitorRefreshRate(hwnd)) {
                    it->monitorHz = MonitorRefreshRate(hwnd);
                    ApplyTimer();
                    if (managerRoot && page == Page::Pet && SelectedPet() == it)
                        RenderPage();
                }
                SaveStoredPosition(it->registryKey, it->posX, it->posY);
            }
            return 0;
        case WM_RBUTTONUP: {
            if (!it)
                return 0;
            // Find index
            int idx = -1;
            for (int i = 0; i < (int)g_items.size(); i++)
                if (&g_items[i] == it) {
                    idx = i;
                    break;
                }

            const auto menuKey = it->registryKey;
            HMENU m = CreatePopupMenu();
            AppendMenuW(m, MF_STRING, kCmdOpenManager,
                        L"Open in Media Manager");
            AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(m, MF_STRING | (it->alwaysOnTop ? MF_CHECKED : 0),
                        kCmdToggleAlwaysOnTop, L"Always on top");
            AppendMenuW(m, MF_STRING | (it->clickThrough ? MF_CHECKED : 0),
                        kCmdToggleClickThrough, L"Click-through");
            AppendMenuW(m, MF_STRING | (it->lockPosition ? MF_CHECKED : 0),
                        kCmdToggleLock, L"Lock position");
            AppendMenuW(m, MF_STRING | (it->paused ? MF_CHECKED : 0),
                        kCmdTogglePause, L"Pause animation");
            AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(m, MF_STRING, kCmdBringToFront, L"Bring to front");
            AppendMenuW(m, MF_STRING, kCmdHideThis, L"Hide this item");
            AppendMenuW(m, configWritable ? MF_STRING : MF_GRAYED,
                        kPetMenuClone, L"Duplicate");
            AppendMenuW(m, MF_STRING, kCmdResetPosition, L"Reset position");
            AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(m, MF_STRING, kCmdReloadAll, L"Reload all images");

            POINT pt{};
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x,
                                     pt.y, 0, hwnd, nullptr);
            DestroyMenu(m);

            if (idx < 0 || idx >= static_cast<int>(g_items.size()) ||
                g_items[idx].hwnd != hwnd ||
                g_items[idx].registryKey != menuKey)
                return 0;
            it = &g_items[idx];
            switch (cmd) {
                case kPetMenuClone:
                    RunImageMenuAction(idx, kPetMenuClone);
                    break;
                case kCmdOpenManager:
                    OpenPetManager(idx);
                    break;
                case kCmdToggleAlwaysOnTop:
                    it->alwaysOnTop = !it->alwaysOnTop;
                    ApplyTopmost(*it);
                    SyncImageState(*it, false);
                    if (managerRoot && page == Page::Pet && idx == g_managerSel)
                        RenderPage();
                    break;
                case kCmdToggleClickThrough:
                    it->clickThrough = !it->clickThrough;
                    ApplyClickThrough(*it);
                    SyncImageState(*it, false);
                    if (managerRoot && page == Page::Pet && idx == g_managerSel)
                        RenderPage();
                    break;
                case kCmdToggleLock:
                    it->lockPosition = !it->lockPosition;
                    SyncImageState(*it, false);
                    if (managerRoot && page == Page::Pet && idx == g_managerSel)
                        RenderPage();
                    break;
                case kCmdTogglePause:
                    it->paused = !it->paused;
                    SyncImageState(*it, false);
                    if (managerRoot && page == Page::Pet && idx == g_managerSel)
                        RenderPage();
                    break;
                case kCmdBringToFront:
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    break;
                case kCmdHideThis:
                    it->hidden = true;
                    ShowWindow(hwnd, SW_HIDE);
                    break;
                case kCmdResetPosition:
                    it->posX = 80;
                    it->posY = 80;
                    SetWindowPos(hwnd, nullptr, it->posX, it->posY, 0, 0,
                                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                    ForgetStoredPosition(it->registryKey);
                    break;
                case kCmdReloadAll:
                    PostMessageW(g_hCtrlWnd, WM_APP_RELOAD, 0, 0);
                    break;
            }
            return 0;
        }
        case WM_NCHITTEST:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static HWND CreateItemWindow(PetItem& it) {
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    if (it.alwaysOnTop)
        exStyle |= WS_EX_TOPMOST;
    if (it.clickThrough)
        exStyle |= WS_EX_TRANSPARENT;
    HWND hwnd =
        CreateWindowExW(exStyle, kItemWndClass, it.fileName.c_str(), WS_POPUP,
                        it.posX, it.posY, it.baseWidth, it.baseHeight, nullptr,
                        nullptr, GetModuleHandleW(nullptr), &it);
    if (hwnd && VisibleItem(it))
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    return hwnd;
}

// ============================================================================
// Settings loading
// ============================================================================

// One-time, read-only import of the private build's current HKCU format.
static void ImportPreviousLibrary() {
    if (!configWritable || ConfigNumber(L"LibraryInitialized", 0))
        return;
    HKEY root{};
    auto status =
        RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\AnimaWindhawkEngine", 0,
                      KEY_READ | KEY_WOW64_64KEY, &root);
    if (status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND) {
        SaveNumber(L"LibraryInitialized", 1);
        return;
    }
    if (status != ERROR_SUCCESS)
        return;
    bool ok = true;
    auto copy = [&](const std::wstring& source, PCWSTR field,
                    const std::wstring& dest, DWORD type) {
        DWORD size{};
        auto rc = RegGetValueW(root, source.c_str(), field, type, nullptr,
                               nullptr, &size);
        if (rc == ERROR_FILE_NOT_FOUND || rc == ERROR_PATH_NOT_FOUND)
            return;
        if (rc != ERROR_SUCCESS || size > 65536) {
            ok = false;
            return;
        }
        std::vector<DWORD> data(size / sizeof(DWORD) + 1, 0);
        if (RegGetValueW(root, source.c_str(), field, type, nullptr,
                         data.data(), &size) != ERROR_SUCCESS) {
            ok = false;
            return;
        }
        if (type == RRF_RT_REG_SZ)
            ok = PutString(dest, reinterpret_cast<PCWSTR>(data.data())) && ok;
        else if (type == RRF_RT_REG_DWORD && size == sizeof(DWORD))
            ok = PutNumber(dest, data[0]) && ok;
        else if (type == RRF_RT_REG_BINARY && size == 8)
            ok = Wh_SetBinaryValue(dest.c_str(), data.data(), size) && ok;
    };
    auto children = [&](const std::wstring& source) {
        std::vector<std::wstring> names;
        HKEY parent{};
        auto rc = RegOpenKeyExW(root, source.c_str(), 0, KEY_READ, &parent);
        if (rc == ERROR_FILE_NOT_FOUND || rc == ERROR_PATH_NOT_FOUND)
            return names;
        if (rc != ERROR_SUCCESS) {
            ok = false;
            return names;
        }
        wchar_t name[256];
        for (DWORD i = 0;; ++i) {
            DWORD len = std::size(name);
            rc = RegEnumKeyExW(parent, i, name, &len, nullptr, nullptr, nullptr,
                               nullptr);
            if (rc == ERROR_NO_MORE_ITEMS)
                break;
            if (rc != ERROR_SUCCESS) {
                ok = false;
                break;
            }
            names.emplace_back(name, len);
        }
        RegCloseKey(parent);
        return names;
    };
    for (auto field :
         {L"WelcomeShown", L"ManagerTheme", L"Accent", L"WorkshopLayout",
          L"TrayIcon", L"QuitNoticeShown", L"ScanSubfolders", L"MaxFps",
          L"AlwaysOnTop", L"ClickThrough", L"LockPosition", L"ScalePercent",
          L"OpacityPercent", L"RotationDegrees", L"Paused"})
        copy(L"Configurations", field, Setting(field), RRF_RT_REG_DWORD);
    copy(L"Configurations", L"ManagerHotkey", Setting(L"ManagerHotkey"),
         RRF_RT_REG_BINARY);
    auto image = [&](const std::wstring& src, const std::wstring& key) {
        copy(src, L"Path", Record(key, L"Path"), RRF_RT_REG_SZ);
        copy(src, L"Position", Record(key, L"Position"), RRF_RT_REG_BINARY);
        for (auto field :
             {L"Enabled", L"AlwaysOnTop", L"ClickThrough", L"LockPosition",
              L"ScalePercent", L"OpacityPercent", L"RotationDegrees", L"Paused",
              L"FpsLimit", L"Removed"})
            copy(src, field, Record(key, field), RRF_RT_REG_DWORD);
    };
    std::vector<std::wstring> single, folders;
    DWORD bytes{};
    auto rc = RegGetValueW(root, L"Configurations", L"ImageOrder",
                           RRF_RT_REG_MULTI_SZ, nullptr, nullptr, &bytes);
    if (rc == ERROR_SUCCESS && bytes <= 16 * 1024 * 1024) {
        std::vector<wchar_t> data(bytes / sizeof(wchar_t) + 2, 0);
        if (RegGetValueW(root, L"Configurations", L"ImageOrder",
                         RRF_RT_REG_MULTI_SZ, nullptr, data.data(),
                         &bytes) == ERROR_SUCCESS) {
            for (PCWSTR name = data.data(); *name; name += wcslen(name) + 1) {
                if (wcschr(name, L'\\'))
                    continue;
                auto key = RecordName(name);
                image(std::wstring(L"Loaded_Images\\") + name, key);
                single.push_back(key);
            }
        } else
            ok = false;
    } else if (rc != ERROR_FILE_NOT_FOUND && rc != ERROR_PATH_NOT_FOUND)
        ok = false;
    for (auto& name : children(L"Loaded_Images\\Folders")) {
        auto src = L"Loaded_Images\\Folders\\" + name;
        auto key = L"Folders\\" + RecordName(name);
        copy(src, L"Path", Record(key, L"Path"), RRF_RT_REG_SZ);
        copy(src, L"Expanded", Record(key, L"Expanded"), RRF_RT_REG_DWORD);
        std::vector<std::wstring> items;
        for (auto& child : children(src)) {
            auto item = key + L"\\" + RecordName(child);
            image(src + L"\\" + child, item);
            items.push_back(item);
        }
        ok = StoreList(Record(key, L"Images.Index"), items) && ok;
        folders.push_back(key);
    }
    RegCloseKey(root);
    if (ok)
        ok = StoreList(ImageList(), single) && StoreList(FolderList(), folders);
    if (!ok) {
        configWritable = false;
        Wh_Log(L"Private library import was incomplete; restart to retry.");
    } else
        SaveNumber(L"LibraryInitialized", 1);
}

static void ReadGlobalSettings(GlobalSettings& s) {
    s = GlobalSettings{};
    LoadImagePaths(s.imagePaths);
    s.trayIcon = ConfigNumber(L"TrayIcon", s.trayIcon) != 0;
    s.alwaysOnTop = ConfigNumber(L"AlwaysOnTop", s.alwaysOnTop) != 0;
    s.clickThrough = ConfigNumber(L"ClickThrough", s.clickThrough) != 0;
    s.lockPosition = ConfigNumber(L"LockPosition", s.lockPosition) != 0;
    s.scanSubfolders = ConfigNumber(L"ScanSubfolders", s.scanSubfolders) != 0;
    s.scalePercent =
        std::clamp<int>(ConfigNumber(L"ScalePercent", s.scalePercent), 1, 500);
    s.opacityPercent = std::clamp<int>(
        ConfigNumber(L"OpacityPercent", s.opacityPercent), 10, 100);
    s.rotationDegrees = std::clamp<int>(
        ConfigNumber(L"RotationDegrees", s.rotationDegrees), 0, 359);
    s.maxFps = std::clamp<int>(ConfigNumber(L"MaxFps", s.maxFps), 1, 1000);
    s.paused = ConfigNumber(L"Paused", s.paused) != 0;
}

static bool NeedsMediaDecoder(const std::wstring& path) {
    const wchar_t* ext = PathFindExtensionW(path.c_str());
    if (!_wcsicmp(ext, L".webp") || !_wcsicmp(ext, L".apng") ||
        !_wcsicmp(ext, L".mp4"))
        return true;
    if (_wcsicmp(ext, L".png"))
        return false;
    // APNG is often named .png: inspect chunk headers, not compressed pixels.
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
        return false;
    BYTE h[8];
    DWORD got = 0;
    const BYTE sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    bool animated = false;
    if (ReadFile(f, h, 8, &got, nullptr) && got == 8 && !memcmp(h, sig, 8)) {
        LARGE_INTEGER size = {};
        GetFileSizeEx(f, &size);
        LONGLONG offset = 8;
        while (offset + 12 <= size.QuadPart &&
               ReadFile(f, h, 8, &got, nullptr) && got == 8) {
            DWORD len = (DWORD(h[0]) << 24) | (DWORD(h[1]) << 16) |
                        (DWORD(h[2]) << 8) | h[3];
            if (offset + 12 + len > size.QuadPart)
                break;
            if (!memcmp(h + 4, "acTL", 4)) {
                animated = true;
                break;
            }
            if (!memcmp(h + 4, "IDAT", 4) || !memcmp(h + 4, "IEND", 4))
                break;
            LARGE_INTEGER skip;
            skip.QuadPart = LONGLONG(len) + 4;
            if (!SetFilePointerEx(f, skip, nullptr, FILE_CURRENT))
                break;
            offset += LONGLONG(len) + 12;
        }
    }
    CloseHandle(f);
    return animated;
}

// Windows CRT argument quoting; CreateProcess is called directly, never
// cmd.exe.
static std::wstring MediaQuote(const std::wstring& arg) {
    std::wstring out = L"\"";
    size_t slashes = 0;
    for (wchar_t c : arg) {
        if (c == L'\\') {
            ++slashes;
            continue;
        }
        out.append(c == L'"' ? slashes * 2 + 1 : slashes, L'\\');
        slashes = 0;
        out += c;
    }
    out.append(slashes * 2, L'\\');
    return out + L"\"";
}

struct MediaTempFrames {
    std::wstring dir;
    std::wstring Frame(UINT n) const {
        wchar_t name[32];
        swprintf_s(name, L"\\frame-%04u.png", n);
        return dir + name;
    }
    ~MediaTempFrames() {
        if (dir.empty())
            return;
        for (UINT n = 1; n <= 301; ++n)
            DeleteFileW(Frame(n).c_str());
        RemoveDirectoryW(dir.c_str());
    }
};

static bool LoadDecodedMedia(PetItem& it,
                             size_t& totalBytes,
                             std::wstring& error) {
    auto exe = DecoderPath();
    if (exe.empty() ||
        GetFileAttributesW(exe.c_str()) == INVALID_FILE_ATTRIBUTES) {
        error =
            L"Choose an installed ffmpeg.exe in App settings to load this "
            L"format.";
        return false;
    }
    wchar_t temp[MAX_PATH], unique[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, temp);
    if (!n || n >= MAX_PATH || !GetTempFileNameW(temp, L"AWE", 0, unique)) {
        error = L"Cannot create temporary frame directory.";
        return false;
    }
    DeleteFileW(unique);
    if (!CreateDirectoryW(unique, nullptr)) {
        error = L"Cannot create temporary frame directory.";
        return false;
    }
    MediaTempFrames frames;
    frames.dir = unique;
    // Decode only local media, no audio or network protocols. Do not
    // shell-expand filenames. 301st frame is a sentinel to reject overlong
    // input, never truncate.
    std::wstring cmd =
        MediaQuote(exe) +
        L" -hide_banner -loglevel error -nostdin -y -protocol_whitelist "
        L"file,pipe"
        L" -threads 2 -i " +
        MediaQuote(it.sourcePath) + L" -map 0:v:0 -an -sn -dn -vf " +
        MediaQuote(
            L"scale=w='min(512,iw)':h='min(512,ih)':force_original_aspect_"
            L"ratio=decrease,fps=20") +
        L" -frames:v 301 -threads 2 -c:v png -pix_fmt rgba -f image2 " +
        MediaQuote(frames.dir + L"\\frame-%04d.png");
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    // Closing the job on timeout/unload also terminates the decoder.
    HANDLE job = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
    limits.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!job || !SetInformationJobObject(job, JobObjectExtendedLimitInformation,
                                         &limits, sizeof(limits))) {
        if (job)
            CloseHandle(job);
        error = L"Cannot initialize decoder job.";
        return false;
    }
    if (!CreateProcessW(exe.c_str(), &cmd[0], nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr,
                        frames.dir.c_str(), &si, &pi)) {
        CloseHandle(job);
        error = L"Cannot start ffmpeg.exe.";
        return false;
    }
    bool assigned = AssignProcessToJobObject(job, pi.hProcess) != FALSE;
    bool started = assigned && ResumeThread(pi.hThread) != DWORD(-1);
    DWORD wait =
        started ? WaitForSingleObject(pi.hProcess, 60000) : WAIT_FAILED;
    if (wait != WAIT_OBJECT_0) {
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, 5000);
    }
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(job);
    if (wait != WAIT_OBJECT_0 || code != 0) {
        error = wait == WAIT_TIMEOUT
                    ? L"Decoding exceeded 60 seconds. Use a shorter clip."
                    : L"FFmpeg could not decode this file. Check the file and "
                      L"update FFmpeg (animated WebP needs a recent build).";
        return false;
    }
    if (GetFileAttributesW(frames.Frame(301).c_str()) !=
        INVALID_FILE_ATTRIBUTES) {
        error = L"Clip exceeds 300 frames (about 15 seconds). Trim it first.";
        return false;
    }
    size_t bytes = 0;
    for (UINT i = 1; i <= 300; ++i) {
        std::wstring framePath = frames.Frame(i);
        if (GetFileAttributesW(framePath.c_str()) == INVALID_FILE_ATTRIBUTES)
            break;
        Gdiplus::Bitmap source(framePath.c_str());
        UINT w = source.GetWidth(), h = source.GetHeight();
        size_t cost = size_t(w) * size_t(h) * 4;
        if (source.GetLastStatus() != Gdiplus::Ok || !w || !h || w > 512 ||
            h > 512 || bytes + cost > 128u * 1024 * 1024 ||
            totalBytes + bytes + cost > 256u * 1024 * 1024) {
            error =
                L"Invalid frame or decoded memory limit exceeded. Use fewer or "
                L"smaller media files.";
            break;
        }
        auto* bitmap =
            source.Clone(0, 0, (INT)w, (INT)h, PixelFormat32bppPARGB);
        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
            delete bitmap;
            error = L"Cannot allocate decoded frame.";
            break;
        }
        it.decodedFrames.push_back(bitmap);
        bytes += cost;
    }
    if (!error.empty() || it.decodedFrames.empty()) {
        for (auto* frame : it.decodedFrames)
            delete frame;
        it.decodedFrames.clear();
        if (error.empty())
            error = L"No decodable video/image frames were found.";
        return false;
    }
    totalBytes += bytes;
    it.image = it.decodedFrames.front();
    it.baseWidth = (int)it.image->GetWidth();
    it.baseHeight = (int)it.image->GetHeight();
    it.frameCount = (UINT)it.decodedFrames.size();
    it.frameDelaysMs.assign(it.frameCount, 50);
    return true;
}

static void SampleEnvironment() {
    SYSTEM_POWER_STATUS power{};
    onBattery = GetSystemPowerStatus(&power) &&
                (power.ACLineStatus == 0 || power.SystemStatusFlag == 1);
    POINT cursor{};
    GetCursorPos(&cursor);
    HWND fg = GetForegroundWindow();
    GUID desktop{};
    BOOL fgCurrent = FALSE;
    bool haveDesktop =
        desktops && fg &&
        SUCCEEDED(desktops->IsWindowOnCurrentVirtualDesktop(fg, &fgCurrent)) &&
        fgCurrent && SUCCEEDED(desktops->GetWindowDesktopId(fg, &desktop)) &&
        !IsEqualGUID(desktop, GUID_NULL);
    for (auto& item : g_items) {
        if (!item.hwnd)
            continue;
        BOOL current = TRUE;
        if (desktops &&
            SUCCEEDED(desktops->IsWindowOnCurrentVirtualDesktop(item.hwnd,
                                                                &current)) &&
            !current && item.followDesktop && haveDesktop)
            current =
                SUCCEEDED(desktops->MoveWindowToDesktop(item.hwnd, desktop));
        if (!item.followDesktop && current && configWritable && haveDesktop &&
            !IsEqualGUID(item.homeDesktop, desktop)) {
            GUID actual{};
            if (SUCCEEDED(desktops->GetWindowDesktopId(item.hwnd, &actual)) &&
                !IsEqualGUID(actual, GUID_NULL) &&
                !IsEqualGUID(item.homeDesktop, actual)) {
                item.homeDesktop = actual;
                Wh_SetBinaryValue(
                    Record(item.registryKey, L"DesktopId").c_str(), &actual,
                    sizeof(actual));
            }
        }
        item.otherDesktop = !current;
        RECT rect{};
        if (!GetWindowRect(item.hwnd, &rect))
            continue;
        RECT hoverBounds = rect;
        InflateRect(&hoverBounds, 18, 18);
        bool hover = item.hoverFade && PtInRect(&hoverBounds, cursor);
        if (item.nearCursor != hover) {
            item.nearCursor = hover;
            RenderItem(item);
        }
        item.covered = false;
        if (!pauseCovered || !VisibleItem(item) || !item.hwnd)
            continue;
        // Subtract opaque windows above this item. Ignore cloaked, layered and
        // shaped windows.
        HRGN remaining = CreateRectRgnIndirect(&rect);
        if (!remaining)
            continue;
        for (HWND above = GetWindow(item.hwnd, GW_HWNDPREV); above;
             above = GetWindow(above, GW_HWNDPREV)) {
            if (!IsWindowVisible(above) || IsIconic(above) ||
                (GetWindowLongPtrW(above, GWL_EXSTYLE) &
                 (WS_EX_LAYERED | WS_EX_TRANSPARENT)))
                continue;
            DWORD cloaked{};
            RECT bounds{}, overlap{};
            if (FAILED(DwmGetWindowAttribute(above, DWMWA_CLOAKED, &cloaked,
                                             sizeof(cloaked))) ||
                cloaked ||
                FAILED(DwmGetWindowAttribute(above, DWMWA_EXTENDED_FRAME_BOUNDS,
                                             &bounds, sizeof(bounds))))
                continue;
            InflateRect(&bounds, -8, -8);
            if (!IntersectRect(&overlap, &rect, &bounds))
                continue;
            HRGN area = CreateRectRgnIndirect(&overlap);
            if (!area)
                continue;
            int shape = GetWindowRgn(above, area);
            int result = SIMPLEREGION;
            if (shape == ERROR) {
                SetRectRgn(area, overlap.left, overlap.top, overlap.right,
                           overlap.bottom);
                result = CombineRgn(remaining, remaining, area, RGN_DIFF);
            }
            DeleteObject(area);
            if (result == NULLREGION) {
                item.covered = true;
                break;
            }
        }
        DeleteObject(remaining);
    }
}

static void UpdateAudio(PetItem& item, bool audible) {
    try {
        if (!audible || item.muted || !item.volume || item.audioFailed ||
            item.reverse || item.returning) {
            if (item.audioPlaying) {
                item.audio.Pause();
                item.audioPlaying = false;
                item.audioCheck = 0;
            }
            return;
        }
        if (_wcsicmp(PathFindExtensionW(item.sourcePath.c_str()), L".mp4"))
            return;
        auto now = GetTickCount64();
        if (item.audioPlaying && now < item.audioCheck)
            return;
        item.audioCheck = now + 150;
        if (!item.audio) {
            wchar_t uri[32768];
            DWORD n = std::size(uri);
            winrt::check_hresult(
                UrlCreateFromPathW(item.sourcePath.c_str(), uri, &n, 0));
            item.audio = winrt::Windows::Media::Playback::MediaPlayer();
            item.audio.AutoPlay(false);
            item.audio.CommandManager().IsEnabled(false);
            item.audio.Source(
                winrt::Windows::Media::Core::MediaSource::CreateFromUri(
                    winrt::Windows::Foundation::Uri(uri)));
        }
        if (!item.audioPlaying) {
            item.audio.Play();
            item.audioPlaying = true;
        }
        auto playback = item.audio.PlaybackSession();
        using State = winrt::Windows::Media::Playback::MediaPlaybackState;
        if (playback.PlaybackState() == State::Opening ||
            playback.PlaybackState() == State::None)
            return;
        auto expected = std::chrono::milliseconds(item.currentFrame * 50);
        auto delta = playback.Position() - expected;
        if (delta > std::chrono::milliseconds(200) ||
            delta < -std::chrono::milliseconds(200))
            playback.Position(expected);
        int fps = SavingAllowed() && conservePower && onBattery
                      ? std::min(8, g_settings.maxFps)
                      : g_settings.maxFps;
        int frameMs = std::max(5000 / item.speed, (1000 + fps - 1) / fps);
        playback.PlaybackRate(50.0 / frameMs);
        if (playback.PlaybackState() == State::Paused)
            item.audio.Play();
        item.audio.Volume(item.volume / 100.0);

    } catch (const winrt::hresult_error&) {
        item.audio = nullptr;
        item.audioPlaying = false;
        item.audioFailed = true;
        Wh_Log(L"Audio unavailable for %s", item.fileName.c_str());
    }
}

static bool LoadPackFrames(PetItem& item,
                           const Pack& pack,
                           const std::wstring& dir,
                           size_t& totalBytes) {
    std::vector<std::unique_ptr<Gdiplus::Bitmap>> frames;
    size_t bytes = 0;
    auto append = [&](Gdiplus::Bitmap& image, UINT x, UINT y, UINT w, UINT h) {
        size_t cost = size_t(w) * h * 4;
        if (!w || !h || w > 512 || h > 512 || frames.size() >= 300 ||
            bytes + cost > 128u * 1024 * 1024 ||
            totalBytes + bytes + cost > 256u * 1024 * 1024 ||
            (!frames.empty() &&
             (frames[0]->GetWidth() != w || frames[0]->GetHeight() != h)))
            return false;
        std::unique_ptr<Gdiplus::Bitmap> copy(image.Clone(
            static_cast<INT>(x), static_cast<INT>(y), static_cast<INT>(w),
            static_cast<INT>(h), PixelFormat32bppPARGB));
        if (!copy || copy->GetLastStatus() != Gdiplus::Ok)
            return false;
        frames.push_back(std::move(copy));
        bytes += cost;
        return true;
    };
    if (pack.width) {
        Gdiplus::Bitmap sheet(item.sourcePath.c_str());
        UINT w = sheet.GetWidth(), h = sheet.GetHeight();
        if (!w || !h || w % pack.width || h % pack.height ||
            static_cast<ULONGLONG>(w) * h > 32u * 1024 * 1024)
            return false;
        for (UINT y = 0; y < h; y += pack.height)
            for (UINT x = 0; x < w; x += pack.width)
                if (!append(sheet, x, y, pack.width, pack.height))
                    return false;
    } else {
        for (const auto& name : pack.frames) {
            Gdiplus::Bitmap frame(WorkshopChild(dir, name).c_str());
            if (!append(frame, 0, 0, frame.GetWidth(), frame.GetHeight()))
                return false;
        }
    }
    if (frames.empty())
        return false;
    for (auto& frame : frames)
        item.decodedFrames.push_back(frame.release());
    item.image = item.decodedFrames.front();
    item.baseWidth = item.image->GetWidth();
    item.baseHeight = item.image->GetHeight();
    item.frameCount = static_cast<UINT>(frames.size());
    item.frameDelaysMs.assign(item.frameCount, 1000 / pack.fps);
    totalBytes += bytes;
    return true;
}

static void DestroyAllItems() {
    frameCache.clear();
    cachedBytes = 0;
    for (auto& it : g_items) {
        if (it.audio) {
            try {
                it.audio.Close();
            } catch (const winrt::hresult_error&) {
            }
            it.audio = nullptr;
        }
        if (it.hwnd)
            DestroyWindow(it.hwnd);
        if (it.decodedFrames.empty())
            delete it.image;
        else
            for (auto* frame : it.decodedFrames)
                delete frame;
        if (it.hThumb)
            DeleteObject(it.hThumb);
    }
    g_items.clear();
}

static void ApplyTimer() {
    int fastest = g_settings.maxFps;
    for (const auto& pet : g_items)
        if (pet.isGif && pet.frameCount > 1)
            fastest = std::max(fastest, std::min(pet.monitorHz, pet.fpsLimit));
    int periodMs = std::max(1, 1000 / fastest);
    if (periodMs > 500)
        periodMs = 500;
    SetTimer(g_hCtrlWnd, kAnimTimerId, (UINT)periodMs, nullptr);
}

static void RemoveTrayIcon() {
    if (!g_trayIconAdded)
        return;
    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = g_hCtrlWnd;
    nid.uID = kTrayUid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    g_trayIconAdded = false;
}

static void ApplyTrayIcon() {
    if (g_settings.trayIcon && !g_trayIconAdded) {
        // Lazily load the manager icon if it hasn't been loaded yet
        // (e.g. when ApplyTrayIcon is called from ReloadAll before any
        // manager window has been opened).
        if (!g_hManagerIcon)
            g_hManagerIcon = LoadManagerIcon();

        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = g_hCtrlWnd;
        nid.uID = kTrayUid;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = kTrayMsg;
        nid.hIcon = g_hManagerIcon
                        ? g_hManagerIcon
                        : LoadIconW(nullptr, (PCWSTR)IDI_APPLICATION);
        wcsncpy_s(nid.szTip, L"AnimaWindhawkEngine", _TRUNCATE);
        g_trayIconAdded = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
    } else if (!g_settings.trayIcon && g_trayIconAdded) {
        RemoveTrayIcon();
    }
}

static void ReloadAll(bool rereadSettings) {
    // Close manager if open (it will re-open if user wants)
    // Actually keep manager open but signal it to refresh
    bool managerWasOpen = g_hManagerWnd != nullptr;

    std::wstring selectedKey;
    if (auto selected = SelectedPet())
        selectedKey = selected->registryKey;
    std::set<std::wstring> hiddenItems;
    std::map<std::wstring, POINT> positions;
    std::map<std::wstring, std::vector<POINT>> unnamedPositions;
    for (const auto& item : g_items) {
        RECT rect{};
        POINT pos{item.posX, item.posY};
        if (item.hwnd && GetWindowRect(item.hwnd, &rect))
            pos = {rect.left, rect.top};
        if (item.registryKey.empty())
            unnamedPositions[item.filePath].push_back(pos);
        else
            positions[item.registryKey] = pos;
        if (item.hidden && !item.registryKey.empty())
            hiddenItems.insert(item.registryKey);
    }
    DestroyAllItems();
    // Only re-read from Windhawk settings when triggered externally (e.g. mod
    // reload, tray "Reload all").  When triggered from the manager UI the live
    // g_settings is already up-to-date and must NOT be overwritten, otherwise
    // in-memory changes (like a freshly added image path) get wiped.
    if (rereadSettings)
        ReadGlobalSettings(g_settings);
    ApplyTrayIcon();

    ReadLibraryFolders();
    std::vector<std::pair<std::wstring, std::wstring>> files;
    for (const auto& path : g_settings.imagePaths)
        if (HasSupportedExt(path) &&
            GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES)
            files.emplace_back(path, L"");
    std::set<std::wstring> emptyFolders;
    auto scan = [&](const std::wstring& path, const std::wstring& key) {
        std::vector<std::wstring> paths;
        if (ScanFolder(path, g_settings.scanSubfolders, paths) && !key.empty())
            emptyFolders.insert(key);
        std::sort(paths.begin(), paths.end());
        for (auto& file : paths)
            files.emplace_back(std::move(file), key);
    };
    for (const auto& folder : libraryFolders)
        scan(folder.path, folder.key);
    g_items.reserve(files.size());

    static std::vector<std::wstring> warnedVideos;
    std::wstring failures;
    size_t decodedBytes = 0;
    std::vector<std::wstring> usedKeys;
    const int screenWidth = std::max(1, GetSystemMetrics(SM_CXSCREEN));
    const int screenHeight = std::max(1, GetSystemMetrics(SM_CYSCREEN));
    const int inset = std::min({80, screenWidth - 1, screenHeight - 1});
    const int cascadeSlots =
        1 + (std::min(screenWidth, screenHeight) - 1 - inset) / 40;
    for (const auto& [path, folderKey] : files) {
        if (workerQuitting)
            return;
        std::wstring fn = GetFileNameFromPath(path);

        PetItem it;
        it.isGif = !_wcsicmp(PathFindExtensionW(path.c_str()), L".gif");
        it.fpsLimit = std::max(5, g_settings.maxFps);
        it.filePath = path;
        it.fileName = fn;
        it.folderKey = folderKey;
        if (!folderKey.empty()) {
            it.registryKey = ImageKey(path, usedKeys, folderKey);
            if (Number(Record(it.registryKey, L"Removed"), 0))
                continue;
            // A missing decoder must preserve the saved folder.
            emptyFolders.erase(folderKey);
        }
        it.sourcePath = path;
        auto parent = fs::path(path).parent_path();
        auto slug = parent.filename().wstring();
        Pack pack;
        std::wstring dir;
        if (SafeWorkshopName(slug) &&
            !_wcsicmp(parent.parent_path().c_str(), GetWorkshopDir().c_str()) &&
            !_wcsicmp(ReadString(PackKey(slug, L"Anchor")).c_str(),
                      fn.c_str())) {
            dir = WorkshopChild(parent.wstring(),
                                ReadString(PackKey(slug, L"Active")));
            try {
                pack = ParsePack(json::JsonObject::Parse(winrt::to_hstring(
                    ReadSmallFile(WorkshopChild(dir, L"package.json")))));
                it.sourcePath = WorkshopChild(dir, pack.file);
            } catch (...) {
                continue;
            }
        }
        if (pack.width || !pack.frames.empty()) {
            if (!LoadPackFrames(it, pack, dir, decodedBytes)) {
                failures += fn + L": invalid sprite or PNG sequence.\n";
                continue;
            }
        } else if (NeedsMediaDecoder(it.sourcePath)) {
            if (_wcsicmp(PathFindExtensionW(path.c_str()), L".mp4") == 0 &&
                std::find(warnedVideos.begin(), warnedVideos.end(), path) ==
                    warnedVideos.end()) {
                warnedVideos.push_back(path);
                MessageBoxW(g_hManagerWnd,
                            L"MP4 support is experimental. Importing an MP4 "
                            L"file may cause performance drops "
                            L"and temporarily freeze the manager while "
                            L"decoding. Audio is muted by default. "
                            L"Short, low-resolution clips are recommended.",
                            L"Experimental MP4 import", MB_OK | MB_ICONWARNING);
            }
            std::wstring error;
            if (!LoadDecodedMedia(it, decodedBytes, error)) {
                if (error.empty())
                    continue;
                Wh_Log(L"Media import failed: %s: %s", path.c_str(),
                       error.c_str());
                if (failures.size() < 4000)
                    failures += fn + L": " + error + L"\n\n";
                continue;
            }
        } else {
            auto* img = new Gdiplus::Image(it.sourcePath.c_str());
            if (img->GetLastStatus() != Gdiplus::Ok) {
                delete img;
                continue;
            }
            it.image = img;
            it.baseWidth = std::max(1, (int)img->GetWidth());
            it.baseHeight = std::max(1, (int)img->GetHeight());
            it.frameCount = img->GetFrameCount(&Gdiplus::FrameDimensionTime);
            if (!it.frameCount)
                it.frameCount = 1;
            it.frameDelaysMs = it.frameCount > 1
                                   ? GetFrameDelays(img, it.frameCount)
                                   : std::vector<UINT>{100};
        }

        it.alwaysOnTop = g_settings.alwaysOnTop;
        it.clickThrough = g_settings.clickThrough;
        it.lockPosition = g_settings.lockPosition;
        it.scalePercent = g_settings.scalePercent;
        it.rotationDegrees = g_settings.rotationDegrees;
        it.opacityPercent = g_settings.opacityPercent;
        it.paused = it.frameCount <= 1 || g_settings.paused;

        if (folderKey.empty()) {
            for (const auto& key : imageOrder) {
                if (std::find(usedKeys.begin(), usedKeys.end(), key) ==
                        usedKeys.end() &&
                    !_wcsicmp(ReadString(Record(key, L"Path")).c_str(),
                              path.c_str())) {
                    it.registryKey = key;
                    break;
                }
            }
        }
        if (it.registryKey.empty() && folderKey.empty())
            it.registryKey = ImageKey(path, usedKeys);
        usedKeys.push_back(it.registryKey);
        it.hidden = hiddenItems.count(it.registryKey) != 0;
        SyncImageState(it, true);
        if (it.reverse && it.frameCount > 1) {
            it.currentFrame = it.frameCount - 1;
            if (it.decodedFrames.empty())
                it.image->SelectActiveFrame(&Gdiplus::FrameDimensionTime,
                                            it.currentFrame);
            else
                it.image = it.decodedFrames[it.currentFrame];
        }
        POINT storedPos{};
        const int offset =
            inset + 40 * static_cast<int>(g_items.size() % cascadeSlots);
        POINT fallback{offset, offset};
        bool hasStoredPos = ReadStoredPosition(it.registryKey, storedPos);
        POINT position = hasStoredPos ? storedPos : fallback;
        auto live = positions.find(it.registryKey);
        if (live != positions.end())
            position = live->second;
        else if (it.registryKey.empty()) {
            auto& prior = unnamedPositions[path];
            if (!prior.empty()) {
                position = prior.front();
                prior.erase(prior.begin());
            }
        }
        position = RecoverPosition(it, position);
        it.posX = position.x;
        it.posY = position.y;
        if (!hasStoredPos || storedPos.x != position.x ||
            storedPos.y != position.y)
            SaveStoredPosition(it.registryKey, it.posX, it.posY);

        g_items.push_back(std::move(it));
        PetItem& ref = g_items.back();
        ref.hwnd = CreateItemWindow(ref);
        if (ref.hwnd && desktops && configWritable) {
            auto key = Record(ref.registryKey, L"DesktopId");
            GUID home{};
            bool restored =
                !ref.followDesktop &&
                Wh_GetBinaryValue(key.c_str(), &home, sizeof(home)) ==
                    sizeof(home) &&
                SUCCEEDED(desktops->MoveWindowToDesktop(ref.hwnd, home));
            if (restored)
                ref.homeDesktop = home;
            else if (SUCCEEDED(desktops->GetWindowDesktopId(ref.hwnd, &home)) &&
                     !IsEqualGUID(home, GUID_NULL)) {
                ref.homeDesktop = home;
                Wh_SetBinaryValue(key.c_str(), &home, sizeof(home));
            }
        }
        if (ref.isGif)
            ref.monitorHz = MonitorRefreshRate(ref.hwnd);
        if (ref.hwnd)
            RenderItem(ref);
    }

    libraryFolders.erase(
        std::remove_if(libraryFolders.begin(), libraryFolders.end(),
                       [&](const LibraryFolder& folder) {
                           return emptyFolders.count(folder.key) &&
                                  RemoveLibraryFolder(folder.key);
                       }),
        libraryFolders.end());
    Wh_Log(
        L"Loaded %zu media items across %zu folders and the individual image "
        L"list",
        g_items.size(), libraryFolders.size());
    ApplyTimer();
    if (!workerQuitting && !failures.empty())
        MessageBoxW(g_hManagerWnd, failures.c_str(),
                    L"Some media could not be imported",
                    MB_OK | MB_ICONWARNING);

    // Notify manager window to refresh thumbnails
    if (managerWasOpen && g_hManagerWnd) {
        g_managerSel = -1;
        for (int i = 0; !selectedKey.empty() && i < (int)g_items.size(); ++i)
            if (g_items[i].registryKey == selectedKey) {
                g_managerSel = i;
                break;
            }
        SendMessageW(g_hManagerWnd, WM_APP_RELOAD, 0, 0);
    }
}

static void ShowAllHidden() {
    for (auto& it : g_items)
        if (it.hidden && it.hwnd) {
            it.hidden = false;
            if (VisibleItem(it))
                ShowWindow(it.hwnd, SW_SHOWNOACTIVATE);
        }
}

// One-time, modeless guide: uses the worker's message loop so pets keep
// animating.
static HWND g_hWelcomeWnd = nullptr;
static HICON g_hWelcomeIcon =
    nullptr;  // owned copy; manager may close independently
static const wchar_t kWelcomeWndClass[] = L"AnimaWindhawkEngine_Welcome";
static const UINT kShowWelcome = WM_APP + 30;
// Keep the worker message loop running while the startup notice is visible.
static HWND readyDialog = nullptr;
static INT_PTR CALLBACK ReadyDialogProc(HWND hwnd,
                                        UINT msg,
                                        WPARAM wp,
                                        LPARAM lp) {
    if (msg == WM_INITDIALOG) {
        if (!g_hManagerIcon)
            g_hManagerIcon = LoadManagerIcon();
        if (g_hManagerIcon) {
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL,
                         reinterpret_cast<LPARAM>(g_hManagerIcon));
            SendMessageW(hwnd, WM_SETICON, ICON_BIG,
                         reinterpret_cast<LPARAM>(g_hManagerIcon));
            SetTaskbarIdentity(hwnd);
        }
        if (lp)
            savingDialog = hwnd;
        SetWindowTextW(hwnd, lp ? L"Pause media to save resources?"
                                : L"AnimaWindhawkEngine");
        auto add = [&](PCWSTR cls, PCWSTR text, DWORD style, RECT bounds,
                       int id) {
            MapDialogRect(hwnd, &bounds);
            HWND control = CreateWindowExW(
                0, cls, text, WS_CHILD | WS_VISIBLE | style, bounds.left,
                bounds.top, bounds.right - bounds.left,
                bounds.bottom - bounds.top, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                GetModuleHandleW(nullptr), nullptr);
            if (control)
                SendMessageW(control, WM_SETFONT,
                             SendMessageW(hwnd, WM_GETFONT, 0, 0), TRUE);
            return control;
        };
        if (lp) {
            add(L"STATIC",
                L"Allow automatic playback changes? Fully covered media and "
                L"locked sessions can pause. On battery, animation can slow to "
                L"8 FPS. You can change this choice in App settings.",
                SS_LEFT, {12, 10, 270, 62}, -1);
            add(L"BUTTON", L"Allow saving", WS_TABSTOP, {60, 68, 158, 88},
                IDYES);
            SetFocus(add(L"BUTTON", L"Keep playing",
                         WS_TABSTOP | BS_DEFPUSHBUTTON, {164, 68, 268, 88},
                         IDNO));
            return FALSE;
        }
        add(L"STATIC",
            L"AnimaWindhawkEngine loaded successfully and is ready to use.",
            SS_LEFT, {12, 10, 220, 36}, -1);
        SetFocus(add(L"BUTTON", L"OK", WS_TABSTOP | BS_DEFPUSHBUTTON,
                     {168, 42, 220, 57}, IDOK));
        return FALSE;
    }
    if (hwnd == savingDialog) {
        if (msg == WM_CLOSE ||
            (msg == WM_COMMAND &&
             (LOWORD(wp) == IDYES || LOWORD(wp) == IDNO ||
              LOWORD(wp) == IDCANCEL || LOWORD(wp) == IDOK))) {
            savingChoice = msg == WM_COMMAND && LOWORD(wp) == IDYES
                               ? SavingChoice::Allow
                               : SavingChoice::KeepPlaying;
            SaveNumber(L"SavingChoice", static_cast<DWORD>(savingChoice));
            DestroyWindow(hwnd);
            return TRUE;
        }
        if (msg == WM_NCDESTROY)
            savingDialog = nullptr;
        return FALSE;
    }
    if (msg == WM_CLOSE ||
        (msg == WM_COMMAND && (LOWORD(wp) == IDOK || LOWORD(wp) == IDCANCEL))) {
        DestroyWindow(hwnd);
        if (!ConfigNumber(L"WelcomeShown", 0))
            PostMessageW(g_hCtrlWnd, kShowWelcome, 0, 0);
        return TRUE;
    }
    if (msg == WM_NCDESTROY)
        readyDialog = nullptr;
    return FALSE;
}
static HWND CreateNotice(bool saving) {
    struct alignas(DWORD) ReadyTemplate {
        DLGTEMPLATE dialog;
        WORD menu, windowClass, title, pointSize;
        wchar_t font[9];
    };
    static_assert(offsetof(ReadyTemplate, menu) == sizeof(DLGTEMPLATE));
    ReadyTemplate layout = {{WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
                                 DS_CENTER | DS_SETFONT,
                             WS_EX_APPWINDOW | WS_EX_TOPMOST, 0, 0, 0, 232, 65},
                            0,
                            0,
                            0,
                            9,
                            L"Segoe UI"};
    if (saving) {
        layout.dialog.cx = 280;
        layout.dialog.cy = 96;
    }
    return CreateDialogIndirectParamW(GetModuleHandleW(nullptr), &layout.dialog,
                                      nullptr, ReadyDialogProc, saving ? 1 : 0);
}
static void ShowSavingDialog() {
    if (savingDialog) {
        SetForegroundWindow(savingDialog);
        return;
    }
    if (sessionLocked)
        return;
    if (!CreateNotice(true))
        Wh_Log(L"Could not display resource-saving prompt.");
}
static void ShowReadyDialog() {
    readyDialog = CreateNotice(false);
    if (readyDialog) {
        SetWindowPos(readyDialog, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(readyDialog);
        UpdateWindow(readyDialog);
        Wh_Log(L"Startup dialog visible: %d", IsWindowVisible(readyDialog));
    } else {
        Wh_Log(L"Could not show the startup dialog (Windows error %lu)",
               GetLastError());
        if (!ConfigNumber(L"WelcomeShown", 0))
            PostMessageW(g_hCtrlWnd, kShowWelcome, 0, 0);
    }
}
static const int kWelcomeWorkshop = 901, kWelcomeManager = 902,
                 kWelcomeRepo = 903;
static const wchar_t kWelcomeRepoUrl[] =
    L"https://github.com/tubeo5866/anima-windhawk-engine";
static hosting::DesktopWindowXamlSource welcomeSource{nullptr};
static winrt::com_ptr<IDesktopWindowXamlSourceNative2> welcomeInput;
static HWND welcomeIsland{};
static const wchar_t* welcomeStage{};

static void BuildWelcome(HWND hwnd) {
    welcomeStage = L"Activate welcome manifest";
    ManagerManifestScope manifest;
    AttachIsland(hwnd, welcomeSource, welcomeIsland, welcomeInput,
                 welcomeStage);
    welcomeStage = L"Build welcome controls";
    auto root = xaml::Markup::XamlReader::Load(
                    LR"GUIDE(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
      RequestedTheme="Dark" Background="#202020" Padding="24,20">
  <Grid.Resources>
    <Style x:Key="GuideCard" TargetType="Border">
      <Setter Property="CornerRadius" Value="8"/><Setter Property="Padding" Value="16,12"/>
      <Setter Property="Background" Value="#2B2B2B"/><Setter Property="BorderBrush" Value="#383838"/><Setter Property="BorderThickness" Value="1"/>
    </Style>
    <Style TargetType="Button"><Setter Property="CornerRadius" Value="5"/><Setter Property="Padding" Value="14,7"/></Style>
  </Grid.Resources>
  <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
  <ScrollViewer HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto">
    <StackPanel Spacing="10">
      <TextBlock Text="Welcome to AnimaWindhawkEngine" FontSize="24" FontWeight="SemiBold" TextWrapping="Wrap"/>
      <TextBlock Text="Your mod is ready. Here is how to get your first image on the desktop." TextWrapping="Wrap" Margin="0,0,0,10"/>
      <Border Style="{StaticResource GuideCard}"><StackPanel Spacing="6">
        <TextBlock Text="1   Add your first image" Foreground="#C4A7FF" FontWeight="SemiBold"/>
        <TextBlock Text="Choose Install in the Workshop, or use Import media in the media library. You can import PNG, JPG, BMP, GIF, WebP, APNG, or experimental MP4 files." TextWrapping="Wrap"/>
      </StackPanel></Border>
      <Border Style="{StaticResource GuideCard}"><StackPanel Spacing="6">
        <TextBlock Text="2   Make it yours" Foreground="#C4A7FF" FontWeight="SemiBold"/>
        <TextBlock Text="Select an item in the sidebar to adjust its scale, rotation, and opacity. Drag it to move it. If it stays in place, turn off click-through and unlock its position." TextWrapping="Wrap"/>
      </StackPanel></Border>
      <Border Style="{StaticResource GuideCard}"><StackPanel Spacing="6">
        <TextBlock Text="3   Find the manager again" Foreground="#C4A7FF" FontWeight="SemiBold"/>
        <TextBlock x:Name="Shortcut" TextWrapping="Wrap"/>
      </StackPanel></Border>
    </StackPanel>
  </ScrollViewer>
  <StackPanel Grid.Row="1" Margin="0,14,0,0" Spacing="6">
    <Grid>
      <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition Width="Auto"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
      <Button x:Name="Browse" Content="Browse Workshop" Background="#7954BC" Foreground="White"/>
      <Button x:Name="Manage" Content="Open manager" Grid.Column="1" Margin="8,0,0,0"/>
      <Button x:Name="Dismiss" Content="Close" Grid.Column="2" HorizontalAlignment="Right"/>
    </Grid>
    <HyperlinkButton x:Name="Repo" Padding="4,6" AutomationProperties.Name="GitHub repository">
      <StackPanel Orientation="Horizontal" Spacing="8">
        <Path Width="18" Height="18" Stretch="Uniform" Fill="#C4A7FF" Data="M12,1 C5.989 0.939 1.066 5.759 1 11.77 C1.023 16.45 4.06 20.582 8.52 22 C9.07 22.1 9.27 21.77 9.27 21.48 L9.27 19.65 C6.21 20.3 5.56 18.21 5.56 18.21 C5.356 17.554 4.923 16.993 4.34 16.63 C3.34 15.97 4.42 15.98 4.42 15.98 C5.12 16.074 5.739 16.483 6.1 17.09 C6.746 18.21 8.169 18.605 9.3 17.98 C9.356 17.432 9.604 16.922 10 16.54 C7.56 16.27 5 15.35 5 11.22 C4.978 10.143 5.376 9.099 6.11 8.31 C5.776 7.385 5.815 6.366 6.22 5.47 C6.22 5.47 7.15 5.18 9.22 6.57 C11.022 6.09 12.918 6.09 14.72 6.57 C16.82 5.18 17.72 5.47 17.72 5.47 C18.125 6.366 18.164 7.385 17.83 8.31 C18.581 9.085 19 10.121 19 11.2 C19 15.34 16.42 16.25 14 16.52 C14.537 17.041 14.812 17.775 14.75 18.52 L14.75 21.47 C14.75 21.82 14.95 22.1 15.5 21.99 C19.947 20.563 22.973 16.44 23 11.77 C22.934 5.759 18.011 0.939 12 1 Z"/>
        <TextBlock Text="GitHub: tubeo5866/anima-windhawk-engine" Foreground="#C4A7FF" TextWrapping="Wrap"/>
      </StackPanel>
    </HyperlinkButton>
  </StackPanel>
</Grid>)GUIDE")
                    .as<controls::Grid>();
    std::wstring hint;
    if (g_hotkeyVk)
        hint = L"Open the manager with " +
               HotkeyToString(g_hotkeyMods, g_hotkeyVk) + L". ";
    hint +=
        L"You can also use the tray icon or right-click an image. Change the "
        L"shortcut in App settings.";
    root.FindName(L"Shortcut").as<TextBlock>().Text(hint);
    // Post actions so the source can close after the XAML click has returned.
    auto bind = [&](PCWSTR name, int command) {
        root.FindName(name).as<controls::Primitives::ButtonBase>().Click(
            [hwnd, command](auto&&, auto&&) {
                PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(command, BN_CLICKED),
                             0);
            });
    };
    bind(L"Browse", kWelcomeWorkshop);
    bind(L"Manage", kWelcomeManager);
    bind(L"Dismiss", IDCANCEL);
    bind(L"Repo", kWelcomeRepo);
    welcomeSource.Content(root);
    BOOL dark = TRUE;
    DWORD corners = 2;
    COLORREF caption = RGB(32, 32, 32);
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 33, &corners, sizeof(corners));
    DwmSetWindowAttribute(hwnd, 35, &caption, sizeof(caption));
    RECT rc{};
    GetClientRect(hwnd, &rc);
    SetWindowPos(welcomeIsland, nullptr, 0, 0, rc.right, rc.bottom,
                 SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
}
static void CloseWelcomeIsland() {
    welcomeInput = nullptr;
    welcomeIsland = nullptr;
    auto source =
        std::exchange(welcomeSource, hosting::DesktopWindowXamlSource{nullptr});
    if (source) {
        try {
            source.Close();
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"[Welcome] Close failed: %s", e.message().c_str());
        }
    }
}
static LRESULT CALLBACK WelcomeWndProc(HWND hwnd,
                                       UINT msg,
                                       WPARAM wp,
                                       LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            try {
                BuildWelcome(hwnd);
                return 0;
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"[Welcome] %s failed (0x%08X): %s", welcomeStage,
                       (unsigned)e.code().value, e.message().c_str());
                CloseWelcomeIsland();
                return -1;
            }
        case WM_SIZE:
            if (welcomeIsland)
                SetWindowPos(welcomeIsland, nullptr, 0, 0, LOWORD(lp),
                             HIWORD(lp), SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        case WM_SETFOCUS:
            if (welcomeIsland)
                SetFocus(welcomeIsland);
            return 0;
        case WM_DPICHANGED: {
            auto r = reinterpret_cast<RECT*>(lp);
            SetWindowPos(hwnd, nullptr, r->left, r->top, r->right - r->left,
                         r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wp) == kWelcomeRepo && HIWORD(wp) == BN_CLICKED) {
                if ((INT_PTR)ShellExecuteW(hwnd, L"open", kWelcomeRepoUrl,
                                           nullptr, nullptr,
                                           SW_SHOWNORMAL) <= 32)
                    MessageBoxW(hwnd,
                                L"Could not open your browser. "
                                L"Visit:\nhttps://github.com/tubeo5866/"
                                L"anima-windhawk-engine",
                                L"GitHub repository",
                                MB_OK | MB_ICONINFORMATION);
                return 0;
            }
            if (LOWORD(wp) == IDCANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (LOWORD(wp) == kWelcomeWorkshop ||
                LOWORD(wp) == kWelcomeManager) {
                bool workshop = LOWORD(wp) == kWelcomeWorkshop;
                OpenPetManager();
                if (g_hManagerWnd) {
                    ShowWindow(g_hManagerWnd, SW_RESTORE);
                    SetForegroundWindow(g_hManagerWnd);
                    if (workshop)
                        PostMessageW(g_hManagerWnd, WM_COMMAND,
                                     MAKEWPARAM(IDC_NAV_WORKSHOP, BN_CLICKED),
                                     0);
                    DestroyWindow(hwnd);
                }
                return 0;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            CloseWelcomeIsland();
            SendMessageW(hwnd, WM_SETICON, ICON_BIG, 0);
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, 0);
            if (g_hWelcomeIcon) {
                DestroyIcon(g_hWelcomeIcon);
                g_hWelcomeIcon = nullptr;
            }
            return 0;
        case WM_NCDESTROY:
            if (g_hWelcomeWnd == hwnd)
                g_hWelcomeWnd = nullptr;
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void ShowWelcomeGuide(bool recordFirstRun) {
    if (g_hWelcomeWnd) {
        ShowWindow(g_hWelcomeWnd, SW_RESTORE);
        SetForegroundWindow(g_hWelcomeWnd);
        return;
    }
    auto oldDpi = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    UINT dpi = GetDpiForSystem();
    RECT r{0, 0, MulDiv(620, dpi, 96), MulDiv(540, dpi, 96)};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectExForDpi(&r, style, FALSE, WS_EX_APPWINDOW, dpi);
    POINT cursor{};
    GetCursorPos(&cursor);
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST),
                    &monitor);
    int w = r.right - r.left, h = r.bottom - r.top;
    int x = monitor.rcWork.left +
            std::max(
                0, ((int)(monitor.rcWork.right - monitor.rcWork.left) - w) / 2);
    int y = monitor.rcWork.top +
            std::max(
                0, ((int)(monitor.rcWork.bottom - monitor.rcWork.top) - h) / 2);
    g_hWelcomeWnd =
        CreateWindowExW(WS_EX_APPWINDOW, kWelcomeWndClass,
                        L"Welcome to AnimaWindhawkEngine", style, x, y, w, h,
                        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (oldDpi)
        SetThreadDpiAwarenessContext(oldDpi);
    if (!g_hWelcomeWnd) {
        Wh_Log(L"Could not create welcome guide");
        return;
    }
    // The welcome window owns its icon independently of the manager.
    // This also works on first run before the manager has ever been opened.
    if (!g_hManagerIcon)
        g_hManagerIcon = LoadManagerIcon();
    g_hWelcomeIcon = g_hManagerIcon ? CopyIcon(g_hManagerIcon) : nullptr;
    if (g_hWelcomeIcon) {
        SendMessageW(g_hWelcomeWnd, WM_SETICON, ICON_BIG,
                     (LPARAM)g_hWelcomeIcon);
        SendMessageW(g_hWelcomeWnd, WM_SETICON, ICON_SMALL,
                     (LPARAM)g_hWelcomeIcon);
    }
    SetTaskbarIdentity(g_hWelcomeWnd);
    ShowWindow(g_hWelcomeWnd, SW_SHOWNORMAL);
    UpdateWindow(g_hWelcomeWnd);
    if (recordFirstRun) {
        DWORD shown = 1;
        SaveNumber(L"WelcomeShown", shown);
    }
}

// ============================================================================
// Controller window
// ============================================================================

static HANDLE disableProcess{};
static bool quitRequested = false;

static bool elevatedDisableAttempt = false;

static DWORD StartDisableCommand(HWND hwnd, bool elevated) {
    std::wstring host(32768, L'\0');
    DWORD length = GetModuleFileNameW(nullptr, host.data(), (DWORD)host.size());
    if (!length)
        return GetLastError();
    if (length >= host.size())
        return ERROR_INSUFFICIENT_BUFFER;
    host.resize(length);
    std::wstring cli =
        host.substr(0, host.find_last_of(L"\\") + 1) + L"windhawk-cli.exe";
    std::wstring args = L"mod disable " + MediaQuote(WH_MOD_ID) + L" --yes";
    if (elevated) {
        elevatedDisableAttempt = true;
        SHELLEXECUTEINFOW launch{sizeof(launch)};
        launch.fMask =
            SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
        launch.hwnd = hwnd;
        launch.lpVerb = L"runas";
        launch.lpFile = cli.c_str();
        launch.lpParameters = args.c_str();
        launch.nShow = SW_HIDE;
        if (!ShellExecuteExW(&launch))
            return GetLastError();
        if (!launch.hProcess)
            return ERROR_INVALID_HANDLE;
        disableProcess = launch.hProcess;
    } else {
        std::wstring command = MediaQuote(cli) + L" " + args;
        STARTUPINFOW startup{sizeof(startup)};
        PROCESS_INFORMATION child{};
        if (!CreateProcessW(cli.c_str(), command.data(), nullptr, nullptr,
                            FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                            &child))
            return GetLastError();
        CloseHandle(child.hThread);
        disableProcess = child.hProcess;
    }
    return ERROR_SUCCESS;
}

static void QuitFromTray(HWND hwnd) {
    if (quitRequested)
        return;
    quitRequested = true;
    if (!ConfigNumber(L"QuitNoticeShown", 0)) {
        if (!MessageBoxW(
                hwnd,
                L"To start the mod, open Windhawk and toggle the mod back",
                L"Hold on, before exiting",
                MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND)) {
            quitRequested = false;
            return;
        }
        SaveNumber(L"QuitNoticeShown", 1);
    }

    elevatedDisableAttempt = false;
    DWORD error = StartDisableCommand(hwnd, false);
    if (error == ERROR_SUCCESS)
        return;
    if (error == ERROR_ELEVATION_REQUIRED) {
        error = StartDisableCommand(hwnd, true);
        if (error == ERROR_SUCCESS)
            return;
        if (error == ERROR_CANCELLED) {
            quitRequested = false;
            return;
        }
    } else if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"[Quit] Windhawk CLI unavailable. Stopping this session.");
        PostMessageW(g_hCtrlWnd, WM_APP_QUIT, 0, 0);
        return;
    }
    Wh_Log(L"[Quit] Could not start Windhawk CLI (%lu)", error);
    quitRequested = false;
    MessageBoxW(hwnd, L"Open Windhawk and disable AnimaWindhawkEngine to quit.",
                L"Could not disable the mod", MB_OK | MB_ICONWARNING);
}

static BOOL CALLBACK CloseWorkerDialog(HWND hwnd, LPARAM) {
    wchar_t cls[32]{};
    if (GetClassNameW(hwnd, cls, std::size(cls)) && !wcscmp(cls, L"#32770") &&
        hwnd != readyDialog)
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}
static int pendingReload = 0;
static bool managerRequested = false;
static int requestedIndex = -1;
static LRESULT CALLBACK ControllerWndProc(HWND hwnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (msg) {
        case WM_APP_QUIT:
            workerQuitting = true;
            EndMenu();
            EnumThreadWindows(GetCurrentThreadId(), CloseWorkerDialog, 0);
            PostQuitMessage(0);
            return 0;
        case WM_APP_RELOAD:
        case WM_APP_RELOAD_ITEMS_ONLY:
            pendingReload =
                std::max(pendingReload, msg == WM_APP_RELOAD ? 2 : 1);
            return 0;
        case WM_APP_OPEN_MANAGER:
            managerRequested = true;
            requestedIndex = (int)wParam;
            return 0;
        case WM_WS_FETCH_DONE: {
            auto* res = (WorkshopFetchResult*)lParam;
            g_workshopFetching = false;
            g_workshopFetched = true;
            if (wParam == 0) {
                for (auto& item : g_workshopItems)
                    if (item.hPreviewBmp)
                        DeleteObject(item.hPreviewBmp);
                g_workshopItems = std::move(res->items);
                for (auto& item : g_workshopItems) {
                    item.installed =
                        !item.localImagePath.empty() &&
                        std::any_of(
                            g_settings.imagePaths.begin(),
                            g_settings.imagePaths.end(), [&](const auto& path) {
                                return _wcsicmp(path.c_str(),
                                                item.localImagePath.c_str()) ==
                                       0;
                            });
                }
                g_workshopError.clear();
            } else {
                g_workshopError = res->error;
            }
            delete res;
            if (managerRoot && page == Page::Workshop)
                RenderPage();
            return 0;
        }
        case WM_WS_INSTALL_DONE: {
            int idx = (int)wParam;
            bool ok = (lParam == 0);
            if (idx >= 0 && idx < (int)g_workshopItems.size()) {
                auto& it = g_workshopItems[idx];
                it.installing = false;
                if (ok) {
                    auto root = WorkshopChild(GetWorkshopDir(), it.slug);
                    auto anchor = ReadString(PackKey(it.slug, L"Anchor"));
                    if (anchor.empty())
                        anchor = it.file;
                    auto path = WorkshopChild(root, anchor);
                    auto src = WorkshopChild(WorkshopChild(root, it.revision),
                                             it.file);
                    ok = !path.empty() && !src.empty() &&
                         (GetFileAttributesW(path.c_str()) !=
                              INVALID_FILE_ATTRIBUTES ||
                          CopyFileW(src.c_str(), path.c_str(), TRUE)) &&
                         PutString(PackKey(it.slug, L"Anchor"), anchor) &&
                         PutString(PackKey(it.slug, L"Active"), it.revision);
                    if (ok) {
                        PutString(PackKey(it.slug, L"Version"), it.version);
                        it.installedVersion = it.version;
                        it.localImagePath = path;
                        if (!it.installed) {
                            g_settings.imagePaths.push_back(path);
                            SaveImagePaths(g_settings.imagePaths);
                        }
                        it.installed = true;
                        PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_ITEMS_ONLY, 0,
                                     0);
                    }
                }
            }
            if (!ok)
                g_workshopError = L"Installation failed. Refresh to try again.";
            if (managerRoot && page == Page::Workshop)
                RenderPage();
            return 0;
        }

        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK) {
                sessionLocked = true;
                lockOccurred = true;
            }
            if (wParam == WTS_SESSION_UNLOCK)
                sessionLocked = false;
            return 0;
        case WM_DISPLAYCHANGE:
            for (auto& pet : g_items) {
                POINT pos = RecoverPosition(pet, {pet.posX, pet.posY});
                if (pos.x != pet.posX || pos.y != pet.posY) {
                    pet.posX = pos.x;
                    pet.posY = pos.y;
                    if (pet.hwnd)
                        SetWindowPos(
                            pet.hwnd, nullptr, pos.x, pos.y, 0, 0,
                            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                    SaveStoredPosition(pet.registryKey, pos.x, pos.y);
                }
                if (pet.isGif)
                    pet.monitorHz = MonitorRefreshRate(pet.hwnd);
            }
            if (g_settings.maxFps > 0)
                ApplyTimer();
            if (managerRoot && page == Page::Pet)
                RenderPage();
            return 0;
        case kShowWelcome:
            ShowWelcomeGuide(true);
            return 0;
        case WM_HOTKEY:
            if (wParam == kToggleMediaHotkey) {
                panic = !panic;
                RefreshVisibility();
                return 0;
            }
            if (wParam == HOTKEY_OPEN_MANAGER)
                OpenPetManager();
            return 0;

        case WM_TIMER:
            if (wParam == kAnimTimerId) {
                if (disableProcess &&
                    WaitForSingleObject(disableProcess, 0) == WAIT_OBJECT_0) {
                    DWORD code = 1;
                    GetExitCodeProcess(disableProcess, &code);
                    CloseHandle(disableProcess);
                    disableProcess = nullptr;
                    if (code == 0) {
                        PostMessageW(g_hCtrlWnd, WM_APP_QUIT, 0, 0);
                    } else {
                        Wh_Log(
                            L"[Quit] Windhawk CLI returned %lu (elevated "
                            L"retry: %d)",
                            code, elevatedDisableAttempt);
                        if (code == 12 && !elevatedDisableAttempt) {
                            DWORD error = StartDisableCommand(hwnd, true);
                            if (error == ERROR_SUCCESS)
                                return 0;
                            if (error == ERROR_CANCELLED) {
                                quitRequested = false;
                                return 0;
                            }
                            Wh_Log(L"[Quit] Elevated CLI launch failed (%lu)",
                                   error);
                        }
                        MessageBoxW(hwnd,
                                    L"Open Windhawk and disable "
                                    L"AnimaWindhawkEngine to quit.",
                                    L"Could not disable the mod",
                                    MB_OK | MB_ICONWARNING);
                        quitRequested = false;
                    }
                }

                ULONGLONG now = GetTickCount64();
                static ULONGLONG nextFullscreenCheck = 0;
                if (now >= nextFullscreenCheck) {
                    UpdateFullscreen();
                    SampleEnvironment();
                    // Present pending edits when a previously suppressed item
                    // becomes visible.
                    for (auto& item : g_items)
                        RenderItem(item);
                    if (savingChoice == SavingChoice::Ask && !sessionLocked &&
                        !savingDialog &&
                        std::any_of(g_items.begin(), g_items.end(),
                                    [](const auto& item) {
                                        return VisibleItem(item) &&
                                               !item.otherDesktop &&
                                               item.frameCount > 1 && !item.paused &&
                                               (lockOccurred || item.covered ||
                                                (conservePower && onBattery));
                                    }))
                        ShowSavingDialog();
                    CheckSceneSchedule();
                    nextFullscreenCheck = now + 500;
                }
                for (auto& it : g_items) {
                    bool active =
                        VisibleItem(it) && !it.otherDesktop &&
                        !(SavingAllowed() && (sessionLocked || it.covered));
                    UpdateAudio(it, active && !it.paused &&
                                        !(it.loops &&
                                          it.completed >=
                                              static_cast<unsigned>(it.loops)));
                    if (!active ||
                        (it.loops &&
                         it.completed >= static_cast<unsigned>(it.loops)))
                        continue;
                    if (it.frameCount <= 1 || it.paused ||
                        it.frameDelaysMs.empty())
                        continue;
                    UINT delay = it.frameDelaysMs[it.currentFrame %
                                                  it.frameDelaysMs.size()];
                    int fps = it.isGif ? std::min(it.fpsLimit, it.monitorHz)
                                       : g_settings.maxFps;
                    if (SavingAllowed() && conservePower && onBattery)
                        fps = std::min(fps, 8);
                    delay = static_cast<UINT>(static_cast<ULONGLONG>(delay) *
                                              100 / it.speed);
                    delay = std::max(delay, 1u + 999u / static_cast<UINT>(fps));
                    if (now - it.lastFrameTick >= delay) {
                        if (!AdvancePlayback(it))
                            continue;
                        if (it.decodedFrames.empty())
                            it.image->SelectActiveFrame(
                                &Gdiplus::FrameDimensionTime, it.currentFrame);
                        else
                            it.image = it.decodedFrames[it.currentFrame];
                        RenderItem(it);
                        it.lastFrameTick = now;
                    }
                }
            }
            return 0;

        case kTrayMsg: {
            UINT evt = (UINT)lParam;
            // Single left-click — open the manager directly.
            if (evt == WM_LBUTTONUP) {
                OpenPetManager(-1);
                return 0;
            }
            // Right-click — show the context menu.
            if (evt == WM_RBUTTONUP) {
                HMENU m = CreatePopupMenu();
                AppendMenuW(m, MF_STRING, kCmdOpenManagerTray,
                            L"Open Pet Manager…");
                AppendMenuW(m, MF_STRING, kCmdQuit, L"Quit");

                POINT pt{};
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                         pt.x, pt.y, 0, hwnd, nullptr);
                DestroyMenu(m);

                if (cmd == kCmdOpenManagerTray)
                    OpenPetManager(-1);
                if (cmd == kCmdQuit)
                    QuitFromTray(hwnd);
            }
            return 0;
        }
    }

    // Explorer restarted the shell — re-add the tray icon unconditionally.
    // Shell_NotifyIcon icons are destroyed when Explorer restarts, so we must
    // call NIM_ADD again even though g_trayIconAdded was already true.
    if (g_taskbarCreatedMsg != 0 && msg == g_taskbarCreatedMsg) {
        g_trayIconAdded = false;  // force NIM_ADD instead of NIM_MODIFY
        ApplyTrayIcon();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Worker thread
// ============================================================================

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    const HRESULT apartment = RoInitialize(RO_INIT_SINGLETHREADED);
    // Ensure comctl32 v6
    INITCOMMONCONTROLSEX icc = {sizeof(icc),
                                ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);

    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hCtrlWnd =
        CreateWindowExW(WS_EX_TOOLWINDOW, kCtrlWndClass,
                        L"AnimaWindhawkEngineController", WS_POPUP, 0, 0, 0, 0,
                        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (g_threadReadyEvent)
        SetEvent(g_threadReadyEvent);
    if (!g_hCtrlWnd) {
        if (SUCCEEDED(apartment))
            RoUninitialize();
        return 1;
    }
    // Register the shell broadcast sent whenever Explorer recreates the
    // taskbar. We handle it in ControllerWndProc to re-add the tray icon after
    // a reboot or an Explorer restart wipes all notification-area icons.
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    // Load the user's saved hotkey (or default Win+Alt+P) then register it.
    OpenConfig();
    ImportPreviousLibrary();
    MigrateStartupOption();
    g_managerTheme = static_cast<ManagerTheme>(
        std::min<DWORD>(2, ConfigNumber(L"ManagerTheme", 0)));
    g_accentIndex =
        std::min<DWORD>(kAccentIndexSystem, ConfigNumber(L"Accent", 0));
    g_workshopView = static_cast<WorkshopView>(
        std::min<DWORD>(2, ConfigNumber(L"WorkshopLayout", 0)));
    savingChoice = static_cast<SavingChoice>(
        std::min<DWORD>(2, ConfigNumber(L"SavingChoice", 0)));
    lockOccurred = false;
    conservePower = ConfigNumber(L"ConservePower", 1) != 0;
    pauseCovered = ConfigNumber(L"PauseCovered", 1) != 0;
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(desktops.put()));
    WTSRegisterSessionNotification(g_hCtrlWnd, NOTIFY_FOR_THIS_SESSION);
    hideFullscreen = ConfigNumber(L"HideFullscreen", 1) != 0;
    snapEdges = ConfigNumber(L"SnapEdges", 0) != 0;
    UpdateFullscreen();
    LoadHotkey();
    ReapplyHotkey();
    panic = false;
    panicAvailable =
        RegisterHotKey(g_hCtrlWnd, kToggleMediaHotkey,
                       MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT,
                       'H') != FALSE;

    if (!workerQuitting)
        ReloadAll();
    if (!workerQuitting) {
        if (taskbarLaunch)
            OpenPetManager();
        else
            OpenManagerAtSignIn();
        ShowReadyDialog();
    }

    while (!workerQuitting) {
        if (pendingReload) {
            int reload = pendingReload;
            pendingReload = 0;
            ReloadAll(reload == 2);
            continue;
        }
        if (managerRequested) {
            managerRequested = false;
            OpenPetManager(requestedIndex);
            continue;
        }
        if (GetMessageW(&msg, nullptr, 0, 0) <= 0)
            break;
        if (savingDialog &&
            (msg.hwnd == savingDialog || IsChild(savingDialog, msg.hwnd)) &&
            IsDialogMessageW(savingDialog, &msg))
            continue;
        if (readyDialog &&
            (msg.hwnd == readyDialog || IsChild(readyDialog, msg.hwnd)) &&
            IsDialogMessageW(readyDialog, &msg))
            continue;
        BOOL handled = FALSE;
        if (welcomeInput && g_hWelcomeWnd &&
            (msg.hwnd == g_hWelcomeWnd || IsChild(g_hWelcomeWnd, msg.hwnd)))
            welcomeInput->PreTranslateMessage(&msg, &handled);
        if (managerInput && g_hManagerWnd &&
            (msg.hwnd == g_hManagerWnd || IsChild(g_hManagerWnd, msg.hwnd)))
            managerInput->PreTranslateMessage(&msg, &handled);
        if (handled)
            continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    for (const auto& item : g_items) {
        RECT rect{};
        if (item.hwnd && GetWindowRect(item.hwnd, &rect))
            SaveStoredPosition(item.registryKey, rect.left, rect.top);
    }
    for (HANDLE worker : workshopWorkers) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
    workshopWorkers.clear();
    MSG result{};
    while (PeekMessageW(&result, g_hCtrlWnd, WM_WS_FETCH_DONE,
                        WM_WS_INSTALL_DONE, PM_REMOVE))
        DispatchMessageW(&result);
    for (auto& item : g_workshopItems)
        if (item.hPreviewBmp)
            DeleteObject(item.hPreviewBmp);
    if (disableProcess) {
        CloseHandle(disableProcess);
        disableProcess = nullptr;
    }

    if (savingDialog)
        DestroyWindow(savingDialog);
    if (readyDialog)
        DestroyWindow(readyDialog);
    if (g_hWelcomeWnd)
        DestroyWindow(g_hWelcomeWnd);
    RemoveTrayIcon();
    if (g_hManagerWnd) {
        DestroyWindow(g_hManagerWnd);
        g_hManagerWnd = nullptr;
    }
    desktops = nullptr;
    WTSUnRegisterSessionNotification(g_hCtrlWnd);
    DestroyAllItems();
    if (g_hCtrlWnd) {
        DestroyWindow(g_hCtrlWnd);
        g_hCtrlWnd = nullptr;
    }
    if (managerRuntime) {
        managerRuntime.Close();
        managerRuntime = nullptr;
    }
    if (managerManifest != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(managerManifest);
        managerManifest = INVALID_HANDLE_VALUE;
    }
    imageOrder.clear();
    configWritable = false;
    if (SUCCEEDED(apartment))
        RoUninitialize();
    return 0;
}

// ============================================================================
// Mod entry points
// ============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"AnimaWindhawkEngine v1.1: init");

    Gdiplus::GdiplusStartupInput gdiInput;
    if (Gdiplus::GdiplusStartup(&g_gdiToken, &gdiInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return FALSE;
    }

    WNDCLASSEXW wcItem = {sizeof(wcItem)};
    wcItem.lpfnWndProc = ItemWndProc;
    wcItem.hInstance = GetModuleHandleW(nullptr);
    wcItem.lpszClassName = kItemWndClass;
    wcItem.hCursor = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcItem);

    WNDCLASSEXW wcCtrl = {sizeof(wcCtrl)};
    wcCtrl.lpfnWndProc = ControllerWndProc;
    wcCtrl.hInstance = GetModuleHandleW(nullptr);
    wcCtrl.lpszClassName = kCtrlWndClass;
    RegisterClassExW(&wcCtrl);

    WNDCLASSEXW wcMgr = {sizeof(wcMgr)};
    wcMgr.lpfnWndProc = ManagerWndProc;
    wcMgr.hInstance = GetModuleHandleW(nullptr);
    wcMgr.lpszClassName = kManagerWndClass;
    wcMgr.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcMgr.hCursor = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcMgr);

    WNDCLASSEXW wcWelcome = {sizeof(wcWelcome)};
    wcWelcome.lpfnWndProc = WelcomeWndProc;
    wcWelcome.hInstance = GetModuleHandleW(nullptr);
    wcWelcome.lpszClassName = kWelcomeWndClass;
    wcWelcome.hCursor = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcWelcome);

    g_threadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_threadReadyEvent)
        return FALSE;
    g_hThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_threadId);
    if (!g_hThread) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        Wh_Log(L"Failed to start worker thread");
        return FALSE;
    }
    if (WaitForSingleObject(g_threadReadyEvent, 5000) == WAIT_OBJECT_0 &&
        !g_hCtrlWnd) {
        CloseHandle(g_hThread);
        g_hThread = nullptr;
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hCtrlWnd)
        UnregisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER);
    Wh_Log(L"AnimaWindhawkEngine v1.1: uninit");

    if (g_hThread) {
        workerQuitting = true;
        if (g_hCtrlWnd)
            PostMessageW(g_hCtrlWnd, WM_APP_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    if (g_threadReadyEvent) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
    }

    UnregisterClassW(kWelcomeWndClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kItemWndClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kCtrlWndClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kManagerWndClass, GetModuleHandleW(nullptr));

    if (g_hManagerIcon) {
        DestroyIcon(g_hManagerIcon);
        g_hManagerIcon = nullptr;
    }

    if (g_gdiToken) {
        Gdiplus::GdiplusShutdown(g_gdiToken);
        g_gdiToken = 0;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"AnimaWindhawkEngine v1.1: settings changed");
    if (g_hCtrlWnd)
        PostMessageW(g_hCtrlWnd, WM_APP_RELOAD, 1, 0);
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
    Wh_Log(L"Hooked into Windhawk successfully. This mod now never dies...");
    Wh_Log(L"...until Windhawk dies, this will die too.");
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

    for (int i = 1; i < argc; ++i)
        if (!wcscmp(argv[i], L"-anima-manager") && isCurrentToolModProcess)
            taskbarLaunch = true;
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
            if (taskbarLaunch) {
                auto ctrl = FindWindowW(kCtrlWndClass,
                                        L"AnimaWindhawkEngineController");
                if (ctrl) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(ctrl, &pid);
                    AllowSetForegroundWindow(pid);
                    PostMessageW(ctrl, WM_APP_OPEN_MANAGER,
                                 static_cast<WPARAM>(-1), 0);
                    ExitProcess(0);
                }
            }
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
