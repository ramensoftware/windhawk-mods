// ==WindhawkMod==
// @id              anima-windhawk-engine
// @name            AnimaWindhawkEngine
// @description     A mod, inspired by AnimaEngine, that doesn't make you pay for it.
// @version         0.1
// @author          TuBeo5866
// @github          https://github.com/tubeo5866
// @donateUrl       https://ko-fi.com/tubeo5866
// @homepage        https://tubb3ll.xyz
// @include         windhawk.exe
// @compilerOptions -lkernel32 -lktmw32 -lruntimeobject -lwindowsapp -ldwmapi -luuid -lgdiplus -lgdi32 -lshell32 -luser32 -lcomctl32 -lcomdlg32 -lole32 -lurlmon -luxtheme -lwininet -lshlwapi -lcrypt32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

![an ugly logo ](https://raw.githubusercontent.com/TuBeo5866/tubeo5866.github.io/refs/heads/main/files/icon.png)

# AnimaWindhawkEngine

💸 Don't have money to buy AnimaEngine but wanna try it? This is for you.

## What's this?

**AnimaWindhawkEngine** is an open-source desktop mod built with the [Windhawk](https://windhawk.net/) Engine that turns any images into free-floating "desktop media". Import it, and every image inside becomes its own living media, right on your desktop.

## Features

Just like AnimaEngine, plus:
- **Global hotkey** — open the Media Manager from anywhere with a configurable key combo (default: Win + Alt + P).
- **Hot-reload** — add, remove, or change images and hit Reload without restarting Windhawk.
- **Position memory** — remembered per image entry so every image wakes up exactly where you left it.
- **No AI remover** — 


## Where's the workshop? AWHE Browser here!

The **AWHE Browser** (AnimaWindhawkEngine Browser) is a built-in image discovery panel inside the Media Manager window. It lets you browse, preview, and install community-made media directly from this repository.

- **Browse** — scroll through the community image grid, each shown with its preview thumbnail, name, and author
- **Install** — hit Download on any image, and it lands straight into your configured images folder and reloads automatically
- **Submit** — made something cool? Publish it for everyone via a pull request (see below)

## Usage

Just like AnimaEngine, DesktopPets, MatrixPets, Shimeji, DeskPins and *other desktop companion* apps:
- **Move an image**: left-click and drag it (unless it's locked or click-through).
- **Per-image menu**: right-click an image for its own options (manager, always-on-top, click-through, lock, pause, bring to front, hide).
- **Media Manager**: open from the tray icon or an image's right-click menu to browse all images as thumbnails and tweak any of them.

You must have:
- [Windhawk](https://windhawk.net/) installed.
- Electricity

## Publish your own images!

Find it great for everyone? Make a **pull request**!
(For updating images, just do the same)

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

**Step 4**: Open a pull request from your fork back to this repo’s main branch.

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


#include <windows.h>
#include <windowsx.h>
#include <ktmw32.h>
#undef GetCurrentTime
#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <climits>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <dwmapi.h>
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



struct ImageOverride {
    std::wstring fileName;
    bool enabled      = true;
    int  alwaysOnTop  = -1;
    int  clickThrough = -1;
    int  lockPosition = -1;
    int  scalePercent    = 100;
    int  rotationDegrees = 0;
    int  opacityPercent  = 100;
    int  posX = -1;
    int  posY = -1;
};

struct GlobalSettings {
    bool useFolderMode   = false;
    std::wstring imagesFolder;
    bool scanSubfolders  = false;
    std::vector<std::wstring> imagePaths;
    bool alwaysOnTop     = true;
    bool clickThrough    = false;
    bool lockPosition    = false;
    int  scalePercent    = 100;
    int  rotationDegrees = 0;
    int  opacityPercent  = 100;
    bool paused          = false;
    int  maxFps          = 20;
    bool trayIcon          = true;
    std::vector<ImageOverride> overrides;
};

static GlobalSettings g_settings;


struct PetItem {
    std::wstring filePath;
    std::wstring registryKey;
    std::wstring folderKey;
    std::wstring fileName;
    Gdiplus::Image* image    = nullptr;
    std::vector<Gdiplus::Bitmap*> decodedFrames;
    UINT  frameCount         = 1;
    std::vector<UINT> frameDelaysMs;
    UINT  currentFrame       = 0;
    bool isGif = false;
    int fpsLimit = 20;
    int monitorHz = 60;
    ULONGLONG lastFrameTick  = 0;
    HWND  hwnd               = nullptr;
    HBITMAP hThumb           = nullptr;
    int  baseWidth  = 1;
    int  baseHeight = 1;
    int  scalePercent    = 100;
    int  rotationDegrees = 0;
    int  opacityPercent  = 100;
    bool alwaysOnTop  = true;
    bool clickThrough = false;
    bool lockPosition = false;
    bool paused  = false;
    bool hidden  = false;
    bool enabled = true;
    int  posX = 80;
    int  posY = 80;
    bool dragging = false;
    POINT dragStartCursor{};
    POINT dragOriginWnd{};
};

static std::vector<PetItem> g_items;

static void RenderItem(PetItem& it);
static void ApplyTopmost(PetItem& it);
static void ApplyClickThrough(PetItem& it);
static void ReloadAll(bool rereadSettings = true,bool importWindhawk = false);
static void ApplyTrayIcon();
static void ApplyTimer();
static void SaveGlobalSettings(const GlobalSettings&);
static void OpenPetManager(int focusIndex = -1);
static void ShowWelcomeGuide(bool recordFirstRun = false);
static void SaveStoredPosition(const std::wstring& fileName, int x, int y);
static void ForgetStoredPosition(const std::wstring& fileName);


static UINT g_taskbarCreatedMsg = 0;
static HWND    g_hCtrlWnd         = nullptr;
static HWND    g_hManagerWnd      = nullptr;
static DWORD   g_threadId         = 0;
static HANDLE  g_hThread          = nullptr;
static HANDLE  g_threadReadyEvent = nullptr;
static ULONG_PTR g_gdiToken       = 0;
static bool    g_trayIconAdded    = false;

static const wchar_t kItemWndClass[]    = L"AnimaWindhawkEngine_Item";
static const wchar_t kCtrlWndClass[]    = L"AnimaWindhawkEngine_Ctrl";
static const wchar_t kManagerWndClass[] = L"AnimaWindhawkEngine_Manager";

static const UINT kAnimTimerId  = 1;
static const UINT kTrayMsg      = WM_APP + 1;
static const UINT kTrayUid      = 1;
static const UINT WM_APP_QUIT   = WM_APP + 2;
static const UINT WM_APP_RELOAD            = WM_APP + 3;
static const UINT WM_APP_RELOAD_ITEMS_ONLY = WM_APP + 5;
static const UINT WM_APP_OPEN_MANAGER        = WM_APP + 4;
static const UINT WM_WS_FETCH_DONE           = WM_APP + 6;
static const UINT WM_WS_INSTALL_DONE         = WM_APP + 7;

enum class WorkshopView { Grid = 0, List = 1, TwoCol = 2 };

struct WorkshopItem {
    std::wstring slug;
    std::wstring name;
    std::wstring author;
    std::wstring description;
    std::wstring version;
    std::wstring file;
    std::wstring previewFile;
    std::vector<std::wstring> tags;
    std::wstring localPreviewPath;
    std::wstring localImagePath;
    bool         installed     = false;
    bool         installing    = false;
    HBITMAP      hPreviewBmp   = nullptr;
};

static std::vector<WorkshopItem> g_workshopItems;
static std::wstring              g_workshopError;
static bool                      g_workshopFetching = false;
static bool                      g_workshopFetched  = false;
static WorkshopView              g_workshopView     = WorkshopView::Grid;
static std::wstring              g_workshopFilter;

static const wchar_t kWorkshopRepoOwner[] = L"tubeo5866";
static const wchar_t kWorkshopRepoName[]  = L"anima-windhawk-engine";

#define IDC_NAV_WORKSHOP 261

#define HOTKEY_OPEN_MANAGER     1

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

enum PetMenuCmd {
    kPetMenuEnable       = 500,
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


static int ParseTriState(PCWSTR s) {
    if (!s || !*s) return -1;
    if (_wcsicmp(s, L"on")  == 0) return 1;
    if (_wcsicmp(s, L"off") == 0) return 0;
    return -1;
}

static std::wstring GetFileNameFromPath(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

static bool HasSupportedExt(const std::wstring& fn) {
    size_t pos = fn.find_last_of(L'.');
    if (pos == std::wstring::npos) return false;
    std::wstring ext = fn.substr(pos);
    for (auto& c : ext) c = (wchar_t)towlower(c);
    static const wchar_t* kExts[] = {L".png", L".jpg", L".jpeg", L".bmp", L".gif", L".webp", L".apng", L".mp4"};
    for (auto e : kExts) if (ext == e) return true;
    return false;
}

static bool ScanFolder(const std::wstring& folder, bool recurse, std::vector<std::wstring>& out) {
    if (folder.empty()) return false;
    std::wstring base = folder;
    wchar_t last = base.empty() ? 0 : base.back();
    if (last != L'\\' && last != L'/') base += L'\\';
    std::wstring searchPat = base + L"*";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(searchPat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return GetLastError()==ERROR_FILE_NOT_FOUND;
    bool complete=true;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        std::wstring full = base + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recurse && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                if (!ScanFolder(full, recurse, out)) complete=false;
            }
        } else if (HasSupportedExt(fd.cFileName)) {
            out.push_back(full);
        }
    } while (FindNextFileW(h, &fd));
    complete=GetLastError()==ERROR_NO_MORE_FILES && complete;
    FindClose(h);
    return complete;
}


static constexpr wchar_t kConfigPath[] = L"SOFTWARE\\AnimaWindhawkEngine";
static constexpr REGSAM kConfigView = KEY_WOW64_64KEY;
static constexpr wchar_t kSettingsPath[] = L"SOFTWARE\\AnimaWindhawkEngine\\Configurations";
static constexpr wchar_t kImagesPath[] = L"SOFTWARE\\AnimaWindhawkEngine\\Loaded_Images";
static HKEY imagesRoot = nullptr;
static HKEY configRoot = nullptr;
static std::vector<std::wstring> imageOrder;
struct RegistryKey {
    HKEY key{};
    ~RegistryKey() { if (key) RegCloseKey(key); }
};
static bool configWritable = false;

static std::wstring AppDataDirectory() {
    wchar_t path[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path))) return {};
    return std::wstring(path) + L"\\AnimaWindhawkEngine";
}

static LSTATUS SetPathList(HKEY key, const std::vector<std::wstring>& paths, PCWSTR name=L"ImagePaths") {
    std::vector<wchar_t> data;
    for (const auto& path : paths) {
        if (path.empty()) continue;
        data.insert(data.end(),path.begin(),path.end());
        data.push_back(0);
    }
    if (data.empty()) data.push_back(0);
    data.push_back(0);
    if (data.size()>MAXDWORD/sizeof(wchar_t)) return ERROR_INVALID_DATA;
    return RegSetValueExW(key,name,0,REG_MULTI_SZ,
                          reinterpret_cast<const BYTE*>(data.data()),(DWORD)(data.size()*sizeof(wchar_t)));
}

static void ConfigWriteResult(PCWSTR name,LSTATUS result) {
    if (result!=ERROR_SUCCESS)
        Wh_Log(L"[Config] Could not save %s under HKCU\\%s (error %ld).",name,kConfigPath,result);
}

static void WriteConfig(PCWSTR name,DWORD type,const void* data,DWORD size) {
    ConfigWriteResult(name,configWritable ? RegSetValueExW(configRoot,name,0,type,
        static_cast<const BYTE*>(data),size) : ERROR_ACCESS_DENIED);
}

static DWORD ConfigNumber(PCWSTR name,DWORD fallback) {
    DWORD value{},size=sizeof(value);
    return configRoot && RegGetValueW(configRoot,nullptr,name,RRF_RT_REG_DWORD,nullptr,&value,&size)==ERROR_SUCCESS
        ? value : fallback;
}

static std::vector<std::wstring> LegacySection(const std::wstring& file,PCWSTR section) {
    std::vector<wchar_t> buffer(1024);
    while (GetPrivateProfileSectionW(section,buffer.data(),(DWORD)buffer.size(),file.c_str())==buffer.size()-2)
        buffer.resize(buffer.size()*2);
    std::vector<std::wstring> entries;
    for (auto p=buffer.data(); *p; p+=wcslen(p)+1) entries.emplace_back(p);
    return entries;
}

static bool MissingValue(HKEY key,PCWSTR name) {
    return RegQueryValueExW(key,name,nullptr,nullptr,nullptr,nullptr)==ERROR_FILE_NOT_FOUND;
}

static LSTATUS CopyMachineValues(HKEY source,HKEY dest) {
    DWORD maxName{},maxData{};
    LSTATUS status=RegQueryInfoKeyW(source,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
                                    nullptr,&maxName,&maxData,nullptr,nullptr);
    if (status!=ERROR_SUCCESS) return status;
    std::vector<wchar_t> name(maxName+1);
    std::vector<BYTE> data(std::max<DWORD>(maxData,1));
    for (DWORD index=0;;) {
        DWORD chars=(DWORD)name.size(),bytes=(DWORD)data.size(),type{};
        status=RegEnumValueW(source,index,name.data(),&chars,nullptr,&type,data.data(),&bytes);
        if (status==ERROR_NO_MORE_ITEMS) return ERROR_SUCCESS;
        if (status==ERROR_MORE_DATA) {
            name.resize(std::max<size_t>(name.size(),16384));
            data.resize(std::max<size_t>(data.size(),bytes));
            continue;
        }
        if (status!=ERROR_SUCCESS) return status;
        status=RegSetValueExW(dest,name.data(),0,type,data.data(),bytes);
        if (status!=ERROR_SUCCESS) return status;
        ++index;
    }
}

static LSTATUS ImportLegacyConfig(HANDLE transaction) {
    HKEY root{};
    DWORD disposition{};
    LSTATUS status=RegCreateKeyTransactedW(HKEY_CURRENT_USER,kConfigPath,0,nullptr,0,
        KEY_READ|KEY_WRITE|kConfigView,nullptr,&root,&disposition,transaction,nullptr);
    if (status!=ERROR_SUCCESS) return status;
    struct Closer { HKEY key; ~Closer() { RegCloseKey(key); } } closeRoot{root};
    DWORD migrated{}, size=sizeof(migrated);
    if (RegGetValueW(root,nullptr,L"IniMigrated",RRF_RT_REG_DWORD,nullptr,&migrated,&size)==ERROR_SUCCESS && migrated)
        return ERROR_SUCCESS;
    if (disposition==REG_CREATED_NEW_KEY) {
        HKEY machine{};
        status=RegOpenKeyExW(HKEY_LOCAL_MACHINE,kConfigPath,0,KEY_READ|kConfigView,&machine);
        if (status==ERROR_SUCCESS) {
            Closer closeMachine{machine};
            DWORD complete{}, bytes=sizeof(complete);
            if (RegGetValueW(machine,nullptr,L"IniMigrated",RRF_RT_REG_DWORD,nullptr,&complete,&bytes)==ERROR_SUCCESS && complete) {
                status=CopyMachineValues(machine,root);
                if (status!=ERROR_SUCCESS) return status;
                HKEY oldPositions{},newPositions{};
                status=RegOpenKeyExW(machine,L"Positions",0,KEY_READ|kConfigView,&oldPositions);
                if (status==ERROR_FILE_NOT_FOUND) return ERROR_SUCCESS;
                if (status!=ERROR_SUCCESS) return status;
                Closer closeOld{oldPositions};
                status=RegCreateKeyTransactedW(root,L"Positions",0,nullptr,0,KEY_WRITE|kConfigView,
                                               nullptr,&newPositions,nullptr,transaction,nullptr);
                if (status!=ERROR_SUCCESS) return status;
                Closer closeNew{newPositions};
                return CopyMachineValues(oldPositions,newPositions);
            }
        } else if (status!=ERROR_FILE_NOT_FOUND && status!=ERROR_PATH_NOT_FOUND) {
            Wh_Log(L"[Config] Previous machine configuration is unavailable (%ld). Importing this user's INI settings.",status);
        }
    }
    auto dir=AppDataDirectory();
    if (dir.empty()) return ERROR_PATH_NOT_FOUND;
    for (PCWSTR name : {L"positions.ini", L"welcome.ini", L"repo-info.ini"}) {
        const auto path=dir+L"\\"+name;
        HANDLE file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                                nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (file!=INVALID_HANDLE_VALUE) { CloseHandle(file); continue; }
        DWORD error=GetLastError();
        if (error!=ERROR_FILE_NOT_FOUND && error!=ERROR_PATH_NOT_FOUND) return error;
    }
    auto ini=dir+L"\\positions.ini";
    auto positions=LegacySection(ini,L"Positions");
    if (!positions.empty()) {
        HKEY key{};
        status=RegCreateKeyTransactedW(root,L"Positions",0,nullptr,0,KEY_READ|KEY_WRITE|kConfigView,
                                        nullptr,&key,nullptr,transaction,nullptr);
        if (status!=ERROR_SUCCESS) return status;
        Closer closePositions{key};
        for (const auto& entry : positions) {
            auto eq=entry.rfind(L'=');
            if (eq==entry.npos || !eq) continue;
            auto name=entry.substr(0,eq);
            POINT point{};
            if (!MissingValue(key,name.c_str()) || swscanf(entry.c_str()+eq+1,L"%ld,%ld",&point.x,&point.y)!=2) continue;
            status=RegSetValueExW(key,name.c_str(),0,REG_BINARY,reinterpret_cast<const BYTE*>(&point),sizeof(point));
            if (status!=ERROR_SUCCESS) return status;
        }
    }
    if (MissingValue(root,L"ImagePaths")) {
        std::map<unsigned long,std::wstring> ordered;
        for (const auto& entry : LegacySection(ini,L"Paths")) {
            wchar_t* end=nullptr;
            unsigned long i=wcstoul(entry.c_str(),&end,10);
            if (end!=entry.c_str() && *end==L'=' && end[1]) ordered[i]=end+1;
        }
        std::vector<std::wstring> paths;
        for (const auto& pair : ordered) paths.push_back(pair.second);
        if (!paths.empty() && (status=SetPathList(root,paths))!=ERROR_SUCCESS) return status;
    }
    if (MissingValue(root,L"ManagerHotkey") && !LegacySection(ini,L"AppUI").empty()) {
        DWORD keys[]={GetPrivateProfileIntW(L"AppUI",L"HotkeyMods",MOD_WIN|MOD_ALT|MOD_NOREPEAT,ini.c_str()),
                      GetPrivateProfileIntW(L"AppUI",L"HotkeyVk",'P',ini.c_str())};
        status=RegSetValueExW(root,L"ManagerHotkey",0,REG_BINARY,reinterpret_cast<const BYTE*>(keys),sizeof(keys));
        if (status!=ERROR_SUCCESS) return status;
    }
    wchar_t value[64]{};
    if (MissingValue(root,L"WelcomeShown") && GetPrivateProfileStringW(L"Welcome",L"Shown",L"",value,64,(dir+L"\\welcome.ini").c_str())) {
        DWORD shown=wcstol(value,nullptr,10)!=0;
        status=RegSetValueExW(root,L"WelcomeShown",0,REG_DWORD,reinterpret_cast<const BYTE*>(&shown),sizeof(shown));
        if (status!=ERROR_SUCCESS) return status;
    }
    if (MissingValue(root,L"RepositoryCommit") && GetPrivateProfileStringW(L"Repository",L"Commit",L"",value,64,(dir+L"\\repo-info.ini").c_str())) {
        status=RegSetValueExW(root,L"RepositoryCommit",0,REG_SZ,reinterpret_cast<const BYTE*>(value),(DWORD)((wcslen(value)+1)*sizeof(wchar_t)));
        if (status!=ERROR_SUCCESS) return status;
    }
    migrated=1;
    return RegSetValueExW(root,L"IniMigrated",0,REG_DWORD,reinterpret_cast<const BYTE*>(&migrated),sizeof(migrated));
}

static LSTATUS MigrateLayout(HANDLE transaction) {
    RegistryKey root, settings, images, oldPositions, positions;
    auto create=[&](HKEY parent,PCWSTR path,RegistryKey& result) {
        return RegCreateKeyTransactedW(parent,path,0,nullptr,0,KEY_READ|KEY_WRITE|kConfigView,
            nullptr,&result.key,nullptr,transaction,nullptr);
    };
    LSTATUS status=create(HKEY_CURRENT_USER,kConfigPath,root);
    if (status!=ERROR_SUCCESS) return status;
    if ((status=create(root.key,L"Configurations",settings))!=ERROR_SUCCESS ||
        (status=create(root.key,L"Loaded_Images",images))!=ERROR_SUCCESS) return status;
    for (PCWSTR name : {L"WelcomeShown",L"ManagerHotkey",L"RepositoryCommit",L"IniMigrated",L"ImagePaths"}) {
        DWORD type{},bytes{};
        status=RegQueryValueExW(root.key,name,nullptr,&type,nullptr,&bytes);
        if (status==ERROR_FILE_NOT_FOUND) continue;
        if (status!=ERROR_SUCCESS) return status;
        std::vector<BYTE> data(bytes+1);
        status=RegQueryValueExW(root.key,name,nullptr,&type,data.data(),&bytes);
        if (status!=ERROR_SUCCESS) return status;
        if (MissingValue(settings.key,name)) {
            status=RegSetValueExW(settings.key,name,0,type,data.data(),bytes);
            if (status!=ERROR_SUCCESS) return status;
        }
        if ((status=RegDeleteValueW(root.key,name))!=ERROR_SUCCESS) return status;
    }
    status=RegOpenKeyTransactedW(root.key,L"Positions",0,KEY_READ|kConfigView,&oldPositions.key,transaction,nullptr);
    if (status==ERROR_FILE_NOT_FOUND) return ERROR_SUCCESS;
    if (status!=ERROR_SUCCESS) return status;
    if ((status=create(settings.key,L"LegacyPositions",positions))!=ERROR_SUCCESS ||
        (status=CopyMachineValues(oldPositions.key,positions.key))!=ERROR_SUCCESS) return status;
    RegCloseKey(oldPositions.key); oldPositions.key=nullptr;
    return RegDeleteKeyTransactedW(root.key,L"Positions",kConfigView,0,transaction,nullptr);
}

static void OpenConfig() {
    HANDLE token{};
    if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) {
        DWORD bytes{};
        GetTokenInformation(token,TokenUser,nullptr,0,&bytes);
        std::vector<BYTE> data(bytes);
        LPWSTR sid{};
        if (bytes && GetTokenInformation(token,TokenUser,data.data(),bytes,&bytes) &&
            ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(data.data())->User.Sid,&sid)) {
            Wh_Log(L"[Config] Registry path: HKEY_USERS\\%s\\%s (HKCU of the tool process, 64-bit view)",sid,kConfigPath);
            LocalFree(sid);
        }
        CloseHandle(token);
    }
    LSTATUS status=RegOpenKeyExW(HKEY_CURRENT_USER,kSettingsPath,0,KEY_READ|kConfigView,&configRoot);
    if (!configRoot || !ConfigNumber(L"IniMigrated",0)) {
        HANDLE tx=CreateTransaction(nullptr,nullptr,0,0,0,0,nullptr);
        if (tx!=INVALID_HANDLE_VALUE) {
            status=ImportLegacyConfig(tx);
            if (status==ERROR_SUCCESS) status=MigrateLayout(tx);
            if (status==ERROR_SUCCESS && !CommitTransaction(tx)) status=GetLastError();
            CloseHandle(tx);
        } else status=GetLastError();
        if (status!=ERROR_SUCCESS) {
            ConfigWriteResult(L"registry migration",status);
            if (!configRoot) RegOpenKeyExW(HKEY_CURRENT_USER,kConfigPath,0,KEY_READ|kConfigView,&configRoot);
            return;
        }
    }
    if (configRoot) { RegCloseKey(configRoot); configRoot=nullptr; }
    status=RegOpenKeyExW(HKEY_CURRENT_USER,kSettingsPath,0,KEY_READ|KEY_WRITE|kConfigView,&configRoot);
    if (status==ERROR_SUCCESS)
        status=RegCreateKeyExW(HKEY_CURRENT_USER,kImagesPath,0,nullptr,0,KEY_READ|KEY_WRITE|kConfigView,nullptr,&imagesRoot,nullptr);
    configWritable=status==ERROR_SUCCESS;
    if (!configWritable) {
        ConfigWriteResult(L"open registry for writing",status);
        if (!configRoot) RegOpenKeyExW(HKEY_CURRENT_USER,kSettingsPath,0,KEY_READ|kConfigView,&configRoot);
        RegOpenKeyExW(HKEY_CURRENT_USER,kImagesPath,0,KEY_READ|kConfigView,&imagesRoot);
    }
}

static std::wstring StringAt(HKEY key,PCWSTR subkey,PCWSTR name) {
    DWORD bytes{};
    if (!key || RegGetValueW(key,subkey,name,RRF_RT_REG_SZ,nullptr,nullptr,&bytes)!=ERROR_SUCCESS) return {};
    std::vector<wchar_t> data(bytes/sizeof(wchar_t)+1);
    return RegGetValueW(key,subkey,name,RRF_RT_REG_SZ,nullptr,data.data(),&bytes)==ERROR_SUCCESS ? data.data() : L"";
}
static void SaveNumber(PCWSTR name,DWORD value) { WriteConfig(name,REG_DWORD,&value,sizeof(value)); }

struct LibraryFolder {
    std::wstring key, path;
    bool expanded;
};
static std::vector<LibraryFolder> libraryFolders;

static std::vector<std::wstring> Subkeys(HKEY parent) {
    std::vector<std::wstring> names;
    wchar_t name[256];
    for (DWORD i=0;;++i) {
        DWORD chars=256;
        auto result=RegEnumKeyExW(parent,i,name,&chars,nullptr,nullptr,nullptr,nullptr);
        if (result!=ERROR_SUCCESS) {
            if (result!=ERROR_NO_MORE_ITEMS) ConfigWriteResult(L"Enumerate folders",result);
            break;
        }
        names.emplace_back(name,chars);
    }
    return names;
}
static void ReadLibraryFolders() {
    libraryFolders.clear();
    RegistryKey folders;
    if (!imagesRoot || RegOpenKeyExW(imagesRoot,L"Folders",0,KEY_READ,&folders.key)!=ERROR_SUCCESS) return;
    for (const auto& name : Subkeys(folders.key)) {
        auto path=StringAt(folders.key,name.c_str(),L"Path");
        if (path.empty()) continue;
        DWORD expanded=1,size=sizeof(expanded);
        RegGetValueW(folders.key,name.c_str(),L"Expanded",RRF_RT_REG_DWORD,nullptr,&expanded,&size);
        libraryFolders.push_back({L"Folders\\"+name,path,expanded!=0});
    }
}
static bool RemoveLibraryFolder(const std::wstring& folderKey) {
    if (!configWritable) return false;
    RegistryKey folders;
    auto status=RegOpenKeyExW(imagesRoot,L"Folders",0,KEY_READ|KEY_WRITE|DELETE|kConfigView,&folders.key);
    if (status==ERROR_SUCCESS)
        status=RegDeleteTreeW(folders.key,GetFileNameFromPath(folderKey).c_str());
    ConfigWriteResult(L"Remove library folder",status);
    return status==ERROR_SUCCESS;
}
static bool RegisterLibraryFolder(const std::wstring& path) {
    if (!configWritable || path.empty()) return false;
    for (const auto& folder : libraryFolders)
        if (!_wcsicmp(path.c_str(),folder.path.c_str())) return true;
    auto base=GetFileNameFromPath(path);
    if (base.empty()) base=L"Drive "+path.substr(0,1);
    for (unsigned i=1;;++i) {
        auto suffix=i==1 ? std::wstring{} : L" ("+std::to_wstring(i)+L")";
        auto name=L"Folders\\"+base.substr(0,255-suffix.size())+suffix;
        RegistryKey folder;
        DWORD disposition{};
        auto status=RegCreateKeyExW(imagesRoot,name.c_str(),0,nullptr,0,KEY_READ|KEY_WRITE,nullptr,&folder.key,&disposition);
        if (status!=ERROR_SUCCESS) { ConfigWriteResult(L"Folders",status); return false; }
        if (disposition!=REG_CREATED_NEW_KEY) continue;
        status=RegSetValueExW(folder.key,L"Path",0,REG_SZ,reinterpret_cast<const BYTE*>(path.c_str()),(DWORD)((path.size()+1)*sizeof(wchar_t)));
        if (status!=ERROR_SUCCESS) {
            RegDeleteKeyExW(imagesRoot,name.c_str(),kConfigView,0);
            ConfigWriteResult(L"Folder path",status);
            return false;
        }
        libraryFolders.push_back({name,path,true});
        return true;
    }
}

static std::wstring ImageKey(const std::wstring& path,const std::vector<std::wstring>& used,const std::wstring& folderKey={}) {
    if (!imagesRoot || path.empty()) return {};
    RegistryKey folder;
    HKEY parent=imagesRoot;
    std::wstring prefix;
    if (!folderKey.empty()) {
        if (RegOpenKeyExW(imagesRoot,folderKey.c_str(),0,KEY_READ|(configWritable ? KEY_WRITE : 0),&folder.key)!=ERROR_SUCCESS) return {};
        parent=folder.key;
        prefix=folderKey+L"\\";
    }
    const auto base=GetFileNameFromPath(path);
    if (base.empty()) return {};
    wchar_t existing[256];
    DWORD index=0, chars=256;
    LSTATUS status;
    while ((status=RegEnumKeyExW(parent,index++,existing,&chars,nullptr,nullptr,nullptr,nullptr))==ERROR_SUCCESS) {
        if ((!folderKey.empty() || _wcsicmp(existing,L"Folders")!=0) &&
            std::find(used.begin(),used.end(),prefix+existing)==used.end() &&
            !_wcsicmp(StringAt(parent,existing,L"Path").c_str(),path.c_str())) return prefix+existing;
        chars=256;
    }
    if (status!=ERROR_NO_MORE_ITEMS || !configWritable) return {};
    for (unsigned i=1;;++i) {
        std::wstring suffix=i==1 ? L"" : L" ("+std::to_wstring(i)+L")";
        auto name=base.substr(0,255-suffix.size())+suffix;
        RegistryKey image;
        DWORD disposition{};
        status=RegCreateKeyExW(parent,name.c_str(),0,nullptr,0,KEY_READ|KEY_WRITE|kConfigView,
            nullptr,&image.key,&disposition);
        if (status!=ERROR_SUCCESS) { ConfigWriteResult(name.c_str(),status); return {}; }
        if (disposition!=REG_CREATED_NEW_KEY) continue;
        status=RegSetValueExW(image.key,L"Path",0,REG_SZ,reinterpret_cast<const BYTE*>(path.c_str()),(DWORD)((path.size()+1)*sizeof(wchar_t)));
        if (status!=ERROR_SUCCESS) {
            RegDeleteKeyExW(parent,name.c_str(),kConfigView,0);
            ConfigWriteResult(L"Path",status);
            return {};
        }
        POINT point{};
        DWORD bytes=sizeof(point);
        if (RegGetValueW(configRoot,L"LegacyPositions",base.c_str(),RRF_RT_REG_BINARY,nullptr,&point,&bytes)==ERROR_SUCCESS && bytes==sizeof(point))
            ConfigWriteResult(L"Position",RegSetValueExW(image.key,L"Position",0,REG_BINARY,reinterpret_cast<BYTE*>(&point),bytes));
        return prefix+name;
    }
}
static bool ReadStoredPosition(const std::wstring& key,POINT& point) {
    DWORD bytes=sizeof(point);
    return imagesRoot && !key.empty() && RegGetValueW(imagesRoot,key.c_str(),L"Position",RRF_RT_REG_BINARY,nullptr,&point,&bytes)==ERROR_SUCCESS && bytes==sizeof(point);
}
static void SaveStoredPosition(const std::wstring& key,int x,int y) {
    POINT point{x,y};
    if (!configWritable || key.empty()) return;
    RegistryKey image;
    auto status=RegOpenKeyExW(imagesRoot,key.c_str(),0,KEY_SET_VALUE|kConfigView,&image.key);
    if (status==ERROR_FILE_NOT_FOUND || status==ERROR_PATH_NOT_FOUND) return;
    if (status==ERROR_SUCCESS)
        status=RegSetValueExW(image.key,L"Position",0,REG_BINARY,reinterpret_cast<const BYTE*>(&point),sizeof(point));
    ConfigWriteResult(L"Position",status);
}
static void ForgetStoredPosition(const std::wstring& key) {
    if (!configWritable || key.empty()) return;
    auto status=RegDeleteKeyValueW(imagesRoot,key.c_str(),L"Position");
    if (status!=ERROR_FILE_NOT_FOUND) ConfigWriteResult(L"Position",status);
}
static void SyncImageState(PetItem& item,bool load) {
    if ((!load && !configWritable) || !imagesRoot || item.registryKey.empty()) return;
    RegistryKey key;
    auto status=RegOpenKeyExW(imagesRoot,item.registryKey.c_str(),0,KEY_READ|(configWritable ? KEY_SET_VALUE : 0)|kConfigView,&key.key);
    if (status!=ERROR_SUCCESS) { ConfigWriteResult(item.registryKey.c_str(),status); return; }
    auto sync=[&](PCWSTR name,auto& field,int low,int high) {
        DWORD value=static_cast<DWORD>(field),bytes=sizeof(value);
        if (load) {
            auto result=RegGetValueW(key.key,nullptr,name,RRF_RT_REG_DWORD,nullptr,&value,&bytes);
            if (result==ERROR_SUCCESS) { field=std::clamp(static_cast<int>(value),low,high); return; }
            if (result!=ERROR_FILE_NOT_FOUND) return;
        }
        if (configWritable) ConfigWriteResult(name,RegSetValueExW(key.key,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&value),sizeof(value)));
    };
    sync(L"Enabled",item.enabled,0,1);
    sync(L"AlwaysOnTop",item.alwaysOnTop,0,1);
    sync(L"ClickThrough",item.clickThrough,0,1);
    sync(L"LockPosition",item.lockPosition,0,1);
    sync(L"ScalePercent",item.scalePercent,1,500);
    sync(L"OpacityPercent",item.opacityPercent,10,100);
    sync(L"RotationDegrees",item.rotationDegrees,0,359);
    if (item.isGif) sync(L"FpsLimit",item.fpsLimit,5,INT_MAX);
    if (item.frameCount>1) sync(L"Paused",item.paused,0,1);
}


static std::wstring GetWorkshopDir() {
    wchar_t la[MAX_PATH] = {};
    GetEnvironmentVariableW(L"LOCALAPPDATA", la, MAX_PATH);
    std::wstring d = std::wstring(la) + L"\\AnimaWindhawkEngine\\workshop";
    CreateDirectoryW((std::wstring(la) + L"\\AnimaWindhawkEngine").c_str(), nullptr);
    CreateDirectoryW(d.c_str(), nullptr);
    return d;
}

static std::wstring JsonGetString(const std::string& json, const char* key) {
    std::string k = std::string("\"") + key + "\"";
    size_t p = json.find(k);
    if (p == std::string::npos) return {};
    p = json.find('"', p + k.size());
    if (p == std::string::npos) return {};
    size_t start = p + 1;
    size_t end   = start;
    while (end < json.size() && (json[end] != '"' || (end > 0 && json[end-1] == '\\'))) end++;
    std::string val = json.substr(start, end - start);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, val.c_str(), -1, nullptr, 0);
    std::wstring out(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, val.c_str(), -1, &out[0], wlen);
    if (!out.empty() && out.back() == 0) out.pop_back();
    return out;
}

static std::vector<std::wstring> JsonGetStringArray(const std::string& json, const char* key) {
    std::string k = std::string("\"") + key + "\"";
    size_t p = json.find(k);
    if (p == std::string::npos) return {};
    p = json.find('[', p + k.size());
    if (p == std::string::npos) return {};
    size_t end = json.find(']', p);
    if (end == std::string::npos) return {};
    std::string arr = json.substr(p + 1, end - p - 1);
    std::vector<std::wstring> out;
    size_t i = 0;
    while ((i = arr.find('"', i)) != std::string::npos) {
        size_t s = i + 1, e = s;
        while (e < arr.size() && (arr[e] != '"' || arr[e-1] == '\\')) e++;
        std::string sv = arr.substr(s, e - s);
        int wl = MultiByteToWideChar(CP_UTF8, 0, sv.c_str(), -1, nullptr, 0);
        std::wstring w(wl, 0);
        MultiByteToWideChar(CP_UTF8, 0, sv.c_str(), -1, &w[0], wl);
        if (!w.empty() && w.back() == 0) w.pop_back();
        out.push_back(w);
        i = e + 1;
    }
    return out;
}

static std::wstring WorkshopRawUrl(const std::wstring& slug, const std::wstring& file) {
    return std::wstring(L"https://raw.githubusercontent.com/")
         + kWorkshopRepoOwner + L"/" + kWorkshopRepoName
         + L"/main/workshop/pets/" + slug + L"/" + file;
}

static std::wstring WorkshopIndexUrl() {
    return std::wstring(L"https://raw.githubusercontent.com/")
         + kWorkshopRepoOwner + L"/" + kWorkshopRepoName
         + L"/main/workshop/index.json";
}

static std::string HttpGet(const std::wstring& url) {
    HINTERNET hInet = InternetOpenW(L"AnimaWindhawkEngine/1.0",
                                    INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInet) return {};
    HINTERNET hConn = InternetOpenUrlW(hInet, url.c_str(), nullptr, 0,
                                       INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                                       INTERNET_FLAG_SECURE, 0);
    std::string body;
    if (hConn) {
        char buf[4096];
        DWORD read = 0;
        while (InternetReadFile(hConn, buf, sizeof(buf), &read) && read)
            body.append(buf, read);
        InternetCloseHandle(hConn);
    }
    InternetCloseHandle(hInet);
    return body;
}

static bool HttpDownloadFile(const std::wstring& url, const std::wstring& dest) {
    HRESULT hr = URLDownloadToFileW(nullptr, url.c_str(), dest.c_str(), 0, nullptr);
    return SUCCEEDED(hr);
}

static const wchar_t* kModVersion = WH_MOD_VERSION;
static std::wstring g_repoCommit = L"unknown";
static std::wstring g_windowsBuild = L"unknown";
static bool ValidCommit(const std::wstring& sha) {
    return (sha.size() == 8 || sha.size() == 40) &&
        sha.find_first_not_of(L"0123456789abcdefABCDEF") == std::wstring::npos;
}
static void LoadBuildInfo() {
    wchar_t hash[64]{};
    DWORD bytes=sizeof(hash);
    if (configRoot) RegGetValueW(configRoot,nullptr,L"RepositoryCommit",RRF_RT_REG_SZ|RRF_ZEROONFAILURE,nullptr,hash,&bytes);
    if (ValidCommit(hash)) g_repoCommit = std::wstring(hash).substr(0,8);
    HKEY key;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                     0, KEY_READ | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS) {
        wchar_t build[64]{}; DWORD bytes=sizeof(build), type=0, revision=0;
        if (RegQueryValueExW(key,L"CurrentBuildNumber",nullptr,&type,(BYTE*)build,&bytes)==ERROR_SUCCESS && type==REG_SZ) {
            build[63]=0; g_windowsBuild=build;
            bytes=sizeof(revision);
            if (RegQueryValueExW(key,L"UBR",nullptr,&type,(BYTE*)&revision,&bytes)==ERROR_SUCCESS && type==REG_DWORD)
                g_windowsBuild += L"." + std::to_wstring(revision);
        }
        RegCloseKey(key);
    }
}

struct WorkshopFetchResult {
    std::vector<WorkshopItem> items;
    std::wstring              error;
    std::wstring              commit;
};

static DWORD WINAPI WorkshopFetchThread(LPVOID param) {
    auto* res = new WorkshopFetchResult();
    std::wstring wsDir = GetWorkshopDir();
    std::wstring sha = JsonGetString(HttpGet(L"https://api.github.com/repos/" +
        std::wstring(kWorkshopRepoOwner) + L"/" + kWorkshopRepoName + L"/commits/HEAD"), "sha");
    if (ValidCommit(sha)) res->commit = sha.substr(0,8);

    std::string listing = HttpGet(WorkshopIndexUrl());
    if (listing.empty()) {
        res->error = L"Could not reach the workshop index. Check your internet connection.";
        PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 1, (LPARAM)res);
        return 0;
    }

    std::vector<std::wstring> slugs;
    {
        size_t arrStart = listing.find('[');
        size_t arrEnd   = listing.find(']');
        if (arrStart != std::string::npos && arrEnd != std::string::npos && arrEnd > arrStart) {
            size_t p = arrStart;
            while ((p = listing.find('"', p + 1)) != std::string::npos && p < arrEnd) {
                size_t q = listing.find('"', p + 1);
                if (q == std::string::npos || q >= arrEnd) break;
                std::string s = listing.substr(p + 1, q - p - 1);
                if (!s.empty()) {
                    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
                    if (n > 1) {
                        std::wstring ws(n - 1, L'\0');
                        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws[0], n);
                        slugs.push_back(std::move(ws));
                    }
                }
                p = q + 1;
            }
        }
    }

    if (slugs.empty()) {
        res->error = L"No items found in the workshop yet. Check back later!";
        PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 1, (LPARAM)res);
        return 0;
    }

    for (auto& slug : slugs) {
        WorkshopItem item;
        item.slug = slug;

        std::wstring petDir = wsDir + L"\\" + slug;
        CreateDirectoryW(petDir.c_str(), nullptr);

        std::wstring metaPath = petDir + L"\\meta.json";
        if (GetFileAttributesW(metaPath.c_str()) == INVALID_FILE_ATTRIBUTES)
            HttpDownloadFile(WorkshopRawUrl(slug, L"meta.json"), metaPath);

        {
            HANDLE hf = CreateFileW(metaPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                    nullptr, OPEN_EXISTING, 0, nullptr);
            if (hf != INVALID_HANDLE_VALUE) {
                DWORD sz = GetFileSize(hf, nullptr);
                std::string buf(sz, '\0');
                DWORD rd; ReadFile(hf, &buf[0], sz, &rd, nullptr);
                CloseHandle(hf);
                item.name        = JsonGetString(buf, "name");
                item.author      = JsonGetString(buf, "author");
                item.description = JsonGetString(buf, "description");
                item.version     = JsonGetString(buf, "version");
                item.file        = JsonGetString(buf, "file");
                item.previewFile = JsonGetString(buf, "preview");
                item.tags        = JsonGetStringArray(buf, "tags");
            }
        }
        if (item.name.empty()) item.name = slug;
        if (item.previewFile.empty()) item.previewFile = L"preview.png";

        std::wstring prevPath = petDir + L"\\" + item.previewFile;
        if (GetFileAttributesW(prevPath.c_str()) == INVALID_FILE_ATTRIBUTES)
            HttpDownloadFile(WorkshopRawUrl(slug, item.previewFile), prevPath);
        item.localPreviewPath = prevPath;

        std::wstring instPath = petDir + L"\\" + item.file;
        if (GetFileAttributesW(instPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            item.localImagePath = instPath;
        }

        res->items.push_back(std::move(item));
    }

    PostMessageW(g_hCtrlWnd, WM_WS_FETCH_DONE, 0, (LPARAM)res);
    return 0;
}

struct WorkshopInstallParam {
    int         index;
    std::wstring slug;
    std::wstring file;
    std::wstring destDir;
};

static DWORD WINAPI WorkshopInstallThread(LPVOID param) {
    auto* p = (WorkshopInstallParam*)param;
    std::wstring url  = WorkshopRawUrl(p->slug, p->file);
    std::wstring dest = p->destDir + L"\\" + p->file;
    CreateDirectoryW(p->destDir.c_str(), nullptr);
    bool ok = HttpDownloadFile(url, dest);
    int idx = p->index;
    delete p;
    if (g_hCtrlWnd)
        PostMessageW(g_hCtrlWnd, WM_WS_INSTALL_DONE, (WPARAM)idx, ok ? 0 : 1);
    return 0;
}

static std::vector<HANDLE> workshopWorkers;
static bool StartWorkshopWorker(LPTHREAD_START_ROUTINE proc, void* parameter) {
    for (auto it=workshopWorkers.begin(); it!=workshopWorkers.end();) {
        if (WaitForSingleObject(*it,0)==WAIT_OBJECT_0) {
            CloseHandle(*it);
            it=workshopWorkers.erase(it);
        } else ++it;
    }
    HANDLE worker=CreateThread(nullptr,0,proc,parameter,0,nullptr);
    if (!worker) return false;
    workshopWorkers.push_back(worker);
    return true;
}
static void Workshop_StartFetch() {
    if (g_workshopFetching) return;
    g_workshopFetching = true;
    g_workshopError.clear();
    if (!StartWorkshopWorker(WorkshopFetchThread,nullptr)) {
        g_workshopFetching=false;
        g_workshopFetched=true;
        g_workshopError=L"Could not start the workshop download.";
    }
}

static void Workshop_StartInstall(int idx) {
    if (idx < 0 || idx >= (int)g_workshopItems.size()) return;
    auto& it = g_workshopItems[idx];
    if (it.installing || g_workshopFetching) return;
    it.installing = true;
    auto* p = new WorkshopInstallParam{
        idx, it.slug, it.file,
        GetWorkshopDir() + L"\\" + it.slug
    };
    if (!StartWorkshopWorker(WorkshopInstallThread,p)) {
        it.installing=false;
        delete p;
        g_workshopError=L"Could not start the installation.";
    }
}

static void SaveImagePaths(const std::vector<std::wstring>& paths,const std::wstring& removedKey = {});

static void Workshop_Uninstall(int idx) {
    if (idx < 0 || idx >= (int)g_workshopItems.size()) return;
    auto& it = g_workshopItems[idx];
    if (!it.installed || it.installing) return;

    auto& paths = g_settings.imagePaths;
    paths.erase(
        std::remove_if(paths.begin(), paths.end(), [&](const std::wstring& p) {
            return _wcsicmp(p.c_str(), it.localImagePath.c_str()) == 0;
        }),
        paths.end()
    );
    SaveImagePaths(paths);

    it.installed = false;
    if (g_hManagerWnd)
        PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
}

static HBITMAP Workshop_LoadPreview(const std::wstring& path, int thumbW, int thumbH) {
    if (path.empty() || GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return nullptr;
    Gdiplus::Bitmap* src = Gdiplus::Bitmap::FromFile(path.c_str());
    if (!src || src->GetLastStatus() != Gdiplus::Ok) { delete src; return nullptr; }

    HDC hdc = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, thumbW, thumbH);
    HBITMAP old  = (HBITMAP)SelectObject(memDC, hBmp);

    HBRUSH bg = CreateSolidBrush(RGB(30, 30, 44));
    RECT r = {0, 0, thumbW, thumbH};
    FillRect(memDC, &r, bg);
    DeleteObject(bg);

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

static std::vector<int> Workshop_FilteredIndices() {
    std::vector<int> out;
    for (int i = 0; i < (int)g_workshopItems.size(); i++) {
        if (g_workshopFilter.empty()) { out.push_back(i); continue; }
        auto& it = g_workshopItems[i];
        auto lc  = [](std::wstring s) { for (auto& c : s) c = (wchar_t)towlower(c); return s; };
        if (lc(it.name).find(g_workshopFilter) != std::wstring::npos ||
            lc(it.author).find(g_workshopFilter) != std::wstring::npos ||
            lc(it.description).find(g_workshopFilter) != std::wstring::npos) {
            out.push_back(i); continue;
        }
        for (auto& t : it.tags)
            if (lc(t).find(g_workshopFilter) != std::wstring::npos) { out.push_back(i); break; }
    }
    return out;
}

static void ReadPathList(PCWSTR name,std::vector<std::wstring>& out) {
    out.clear();
    if (!configRoot) return;
    DWORD bytes{};
    LSTATUS status=RegGetValueW(configRoot,nullptr,name,RRF_RT_REG_MULTI_SZ,nullptr,nullptr,&bytes);
    while (status==ERROR_SUCCESS || status==ERROR_MORE_DATA) {
        std::vector<wchar_t> data(bytes/sizeof(wchar_t)+2,0);
        DWORD capacity=bytes;
        status=RegGetValueW(configRoot,nullptr,name,RRF_RT_REG_MULTI_SZ,nullptr,data.data(),&capacity);
        if (status==ERROR_MORE_DATA) {
            status=RegGetValueW(configRoot,nullptr,name,RRF_RT_REG_MULTI_SZ,nullptr,nullptr,&bytes);
            continue;
        }
        if (status==ERROR_SUCCESS)
            for (const wchar_t* path=data.data(); *path; path+=wcslen(path)+1) out.emplace_back(path);
        break;
    }
}

static void SaveImagePaths(const std::vector<std::wstring>& paths,const std::wstring& removedKey) {
    if (!configWritable) return;
    std::vector<std::wstring> keys,used;
    if (!removedKey.empty()) used.push_back(removedKey);
    for (const auto& path : paths) {
        if (path.empty()) continue;
        std::wstring key;
        for (const auto& old : imageOrder) {
            if (std::find(used.begin(),used.end(),old)==used.end() &&
                !_wcsicmp(StringAt(imagesRoot,old.c_str(),L"Path").c_str(),path.c_str())) {
                key=old;
                break;
            }
        }
        if (key.empty()) key=ImageKey(path,used);
        if (key.empty()) return;
        used.push_back(key);
        keys.push_back(key);
    }
    auto status=SetPathList(configRoot,keys,L"ImageOrder");
    ConfigWriteResult(L"ImageOrder",status);
    if (status!=ERROR_SUCCESS) return;
    for (const auto& old : imageOrder)
        if (std::find(keys.begin(),keys.end(),old)==keys.end())
            ConfigWriteResult(old.c_str(),RegDeleteKeyExW(imagesRoot,old.c_str(),kConfigView,0));
    imageOrder=std::move(keys);
    RegDeleteValueW(configRoot,L"ImagePaths");
}
static void LoadImagePaths(std::vector<std::wstring>& out) {
    if (!configRoot || MissingValue(configRoot,L"ImageOrder")) {
        ReadPathList(L"ImagePaths",out);
        return;
    }
    std::vector<std::wstring> saved;
    ReadPathList(L"ImageOrder",saved);
    out.clear();
    imageOrder.clear();
    for (const auto& key : saved) {
        if (key.empty() || key.find(L'\\')!=key.npos) continue;
        auto path=StringAt(imagesRoot,key.c_str(),L"Path");
        if (!path.empty()) { out.push_back(path); imageOrder.push_back(key); }
    }
}

static const ImageOverride* FindOverride(const GlobalSettings& s, const std::wstring& fileName) {
    for (auto& ov : s.overrides)
        if (!ov.fileName.empty() && _wcsicmp(ov.fileName.c_str(), fileName.c_str()) == 0)
            return &ov;
    return nullptr;
}

static std::vector<UINT> GetFrameDelays(Gdiplus::Image* img, UINT frameCount) {
    std::vector<UINT> delays(frameCount, 100);
    UINT size = img->GetPropertyItemSize(PropertyTagFrameDelay);
    if (size > 0) {
        std::vector<BYTE> buf(size);
        if (img->GetPropertyItem(PropertyTagFrameDelay, size, (Gdiplus::PropertyItem*)buf.data()) == Gdiplus::Ok) {
            auto* pi   = (Gdiplus::PropertyItem*)buf.data();
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


static HBITMAP CreateThumb(PetItem& it, int sz = 96) {
    if (!it.image) return nullptr;
    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  sz;
    bmi.bmiHeader.biHeight      = -sz;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   bits  = nullptr;
    HBITMAP hBmp  = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP hOld  = (HBITMAP)SelectObject(memDC, hBmp);

    if (bits) {
        BYTE* px = (BYTE*)bits;
        for (int y = 0; y < sz; y++) {
            for (int x = 0; x < sz; x++) {
                bool checker = ((x / 8) + (y / 8)) % 2 == 0;
                BYTE v = checker ? 0x55 : 0x44;
                int idx = (y * sz + x) * 4;
                px[idx]   = v;
                px[idx+1] = v;
                px[idx+2] = v;
                px[idx+3] = 0xFF;
            }
        }

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


static void RenderItem(PetItem& it) {
    if (!it.hwnd || !it.image) return;

    double scale = (it.scalePercent <= 0 ? 1 : it.scalePercent) / 100.0;
    int dstW = std::max(1, (int)std::lround(it.baseWidth  * scale));
    int dstH = std::max(1, (int)std::lround(it.baseHeight * scale));

    double rad  = it.rotationDegrees * 3.14159265358979323846 / 180.0;
    double cosA = std::fabs(std::cos(rad));
    double sinA = std::fabs(std::sin(rad));
    int canvasW = std::max(1, (int)std::lround(dstW * cosA + dstH * sinA));
    int canvasH = std::max(1, (int)std::lround(dstW * sinA + dstH * cosA));

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  canvasW;
    bmi.bmiHeader.biHeight      = -canvasH;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP hOld = (HBITMAP)SelectObject(memDC, hBmp);

    if (bits) {
        Gdiplus::Bitmap canvasBmp(canvasW, canvasH, canvasW * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&canvasBmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.TranslateTransform((Gdiplus::REAL)(canvasW / 2.0), (Gdiplus::REAL)(canvasH / 2.0));
        g.RotateTransform((Gdiplus::REAL)it.rotationDegrees);
        g.TranslateTransform((Gdiplus::REAL)(-dstW / 2.0), (Gdiplus::REAL)(-dstH / 2.0));

        int op = std::max(10, std::min(100, it.opacityPercent));
        if (op < 100) {
            Gdiplus::ColorMatrix cm = {{
                {1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0},
                {0,0,0,(Gdiplus::REAL)(op/100.0),0}, {0,0,0,0,1}
            }};
            Gdiplus::ImageAttributes attr;
            attr.SetColorMatrix(&cm);
            Gdiplus::Rect dr(0, 0, dstW, dstH);
            g.DrawImage(it.image, dr, 0, 0,
                        (Gdiplus::REAL)it.baseWidth, (Gdiplus::REAL)it.baseHeight,
                        Gdiplus::UnitPixel, &attr);
        } else {
            g.DrawImage(it.image, 0, 0, dstW, dstH);
        }
    }

    RECT wr{};
    GetWindowRect(it.hwnd, &wr);
    SetWindowPos(it.hwnd, nullptr, 0, 0, canvasW, canvasH,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    POINT ptSrc = {0, 0};
    POINT ptDst = {wr.left, wr.top};
    SIZE  sizeWnd = {canvasW, canvasH};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(it.hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(memDC, hOld);
    if (hBmp) DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

static void ApplyTopmost(PetItem& it) {
    SetWindowPos(it.hwnd,
                 it.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void ApplyClickThrough(PetItem& it) {
    LONG_PTR ex = GetWindowLongPtrW(it.hwnd, GWL_EXSTYLE);
    if (it.clickThrough) ex |=  WS_EX_TRANSPARENT;
    else                 ex &= ~WS_EX_TRANSPARENT;
    SetWindowLongPtrW(it.hwnd, GWL_EXSTYLE, ex);
}


static int g_managerSel = -1;
static HICON g_hManagerIcon = nullptr;
enum class ManagerTheme { Dark = 0, Light = 1, System = 2 };
static ManagerTheme g_managerTheme = ManagerTheme::Dark;
static COLORREF g_accentChoices[5] = {
    RGB(0x7E, 0x57, 0xC2),
    RGB(0x5B, 0xA8, 0xE0),
    RGB(0x4C, 0xC0, 0x8A),
    RGB(0xE0, 0x6B, 0x6B),
    RGB(0xE0, 0xA6, 0x4C),
};
static int g_accentIndex = 0;
static const int kAccentIndexSystem = 5;

static COLORREF GetSystemAccentColor() {
    DWORD color = 0xFF6200EE, size = sizeof(color);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\DWM",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AccentColor", nullptr, nullptr, (LPBYTE)&color, &size);
        RegCloseKey(hKey);
    }
    BYTE r = (BYTE)(color & 0xFF);
    BYTE g2 = (BYTE)((color >> 8) & 0xFF);
    BYTE b = (BYTE)((color >> 16) & 0xFF);
    return RGB(r, g2, b);
}

static UINT g_hotkeyMods = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
static UINT g_hotkeyVk   = 'P';
static bool   g_hotkeyCapturing = false;
static HHOOK  g_hKeyHook        = nullptr;
static bool   g_winKeyDown      = false;
static bool   g_ctrlKeyDown     = false;
static unsigned g_altKeysDown = 0;
static bool   g_shiftKeyDown    = false;


static std::wstring HotkeyToString(UINT mods, UINT vk) {
    if (mods == 0 && vk == 0) return L"(none)";
    std::wstring s;
    if (mods & MOD_WIN)     s += L"Win + ";
    if (mods & MOD_CONTROL) s += L"Ctrl + ";
    if (mods & MOD_ALT)     s += L"Alt + ";
    if (mods & MOD_SHIFT)   s += L"Shift + ";
    UINT sc = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    LONG lParam = (LONG)(sc << 16);
    switch (vk) {
    case VK_INSERT: case VK_DELETE: case VK_HOME:  case VK_END:
    case VK_PRIOR:  case VK_NEXT:   case VK_LEFT:  case VK_RIGHT:
    case VK_UP:     case VK_DOWN:   case VK_DIVIDE: case VK_NUMLOCK:
    case VK_RCONTROL: case VK_RMENU:
        lParam |= (1 << 24);
        break;
    }
    wchar_t buf[64] = {};
    bool nameOk = sc && GetKeyNameTextW(lParam, buf, 64) && buf[0] &&
                  !(wcscmp(buf, L"Alt") == 0 && vk != VK_MENU) &&
                  !(wcscmp(buf, L"Ctrl") == 0 && vk != VK_CONTROL) &&
                  !(wcscmp(buf, L"Shift") == 0 && vk != VK_SHIFT);
    if (nameOk) {
        s += buf;
    } else {
        if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) {
            s += (wchar_t)vk;
        } else if (vk >= VK_F1 && vk <= VK_F24) {
            wchar_t tmp[8]; swprintf_s(tmp, L"F%u", vk - VK_F1 + 1);
            s += tmp;
        } else if (vk >= 0x20 && vk <= 0x7E) {
            s += (wchar_t)vk;
        } else {
            wchar_t tmp[8]; swprintf_s(tmp, L"0x%02X", vk);
            s += tmp;
        }
    }
    return s;
}

static void ReapplyHotkey() {
    if (g_hCtrlWnd) {
        UnregisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER);
        if (g_hotkeyVk) RegisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER, g_hotkeyMods, g_hotkeyVk);
    }
}

static void SaveHotkey() {
    DWORD keys[]={g_hotkeyMods,g_hotkeyVk};
    WriteConfig(L"ManagerHotkey",REG_BINARY,keys,sizeof(keys));
}
static void LoadHotkey() {
    DWORD keys[2]{}, bytes=sizeof(keys);
    if (configRoot && RegGetValueW(configRoot,nullptr,L"ManagerHotkey",RRF_RT_REG_BINARY,nullptr,keys,&bytes)==ERROR_SUCCESS && bytes==sizeof(keys)) {
        g_hotkeyMods=keys[0];
        g_hotkeyVk=keys[1];
    }
}

static void StartKeyCapture();
static void StopKeyCapture();

static LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    auto* ks   = (KBDLLHOOKSTRUCT*)lParam;
    UINT  vk   = ks->vkCode;
    bool  down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

    switch (vk) {
    case VK_LWIN:     case VK_RWIN:
        g_winKeyDown   = down;
        if (g_hotkeyCapturing) return 1;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    case VK_CONTROL:  case VK_LCONTROL: case VK_RCONTROL:
        g_ctrlKeyDown  = down;
        if (!g_hotkeyCapturing) return CallNextHookEx(nullptr, nCode, wParam, lParam);
        return 1;

    case VK_SHIFT:    case VK_LSHIFT:   case VK_RSHIFT:
        g_shiftKeyDown = down;
        if (!g_hotkeyCapturing) return CallNextHookEx(nullptr, nCode, wParam, lParam);
        return 1;

    case VK_MENU: case VK_LMENU: case VK_RMENU: {
        unsigned side=(vk==VK_RMENU || (vk==VK_MENU && (ks->flags & LLKHF_EXTENDED))) ? 2u : 1u;
        if (down) g_altKeysDown |= side;
        else g_altKeysDown &= ~side;
        if (!g_hotkeyCapturing) return CallNextHookEx(nullptr,nCode,wParam,lParam);
        return 1;
    }
    }

    if (!g_hotkeyCapturing || !down)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    if (vk == VK_ESCAPE) {
        g_hotkeyCapturing = false;
        StopKeyCapture();
        ReapplyHotkey();
        if (g_hManagerWnd) PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
        return 1;
    }

    UINT mods = MOD_NOREPEAT;
    if (g_altKeysDown)             mods |= MOD_ALT;
    if (g_ctrlKeyDown)             mods |= MOD_CONTROL;
    if (g_shiftKeyDown)            mods |= MOD_SHIFT;
    if (g_winKeyDown)              mods |= MOD_WIN;

    bool hasRealMod = (mods & (MOD_WIN | MOD_ALT | MOD_CONTROL)) != 0;
    bool isBareTypeable = !hasRealMod &&
                          ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9'));
    if (isBareTypeable) {
        if (g_hManagerWnd) PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
        return 1;
    }

    g_hotkeyCapturing = false;
    g_hotkeyMods = mods;
    g_hotkeyVk   = vk;
    StopKeyCapture();
    ReapplyHotkey();
    SaveHotkey();
    if (g_hManagerWnd) PostMessageW(g_hManagerWnd, WM_APP + 20, 0, 0);
    return 1;
}

static void StartKeyCapture() {
    if (!g_hKeyHook) {
        g_altKeysDown=(GetAsyncKeyState(VK_LMENU)&0x8000 ? 1u : 0u) |
                      (GetAsyncKeyState(VK_RMENU)&0x8000 ? 2u : 0u);
        g_hKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, LLKeyProc, nullptr, 0);
    }
}

static void StopKeyCapture() {
    if (g_hKeyHook) {
        UnhookWindowsHookEx(g_hKeyHook);
        g_hKeyHook = nullptr;
    }
    g_altKeysDown=0;
    g_winKeyDown = g_ctrlKeyDown = g_shiftKeyDown = false;
}
static bool SystemPrefersLightTheme() {
    DWORD value = 1, size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value != 0;
}

static void RunImageMenuAction(int idx,int cmd) {
    auto& pet=g_items[idx];

    switch (cmd) {
    case kPetMenuHide:
        pet.hidden = true;
        if (pet.hwnd) ShowWindow(pet.hwnd, SW_HIDE);
        break;
    case kPetMenuShow:
        pet.hidden = false;
        if (pet.enabled && pet.hwnd) ShowWindow(pet.hwnd, SW_SHOWNOACTIVATE);
        break;
    case kPetMenuBringToFront:
        if (pet.hwnd) SetWindowPos(pet.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        break;
    case kPetMenuResetPos:
        pet.posX = 80; pet.posY = 80;
        if (pet.hwnd) SetWindowPos(pet.hwnd, nullptr, pet.posX, pet.posY, 0, 0,
                                   SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        ForgetStoredPosition(pet.registryKey);
        break;
    case kPetMenuClone: {
        if (!g_settings.useFolderMode) {
            for (auto& p : g_settings.imagePaths) {
                if (p == pet.filePath) {
                    g_settings.imagePaths.push_back(p);
                    SaveImagePaths(g_settings.imagePaths);
                    break;
                }
            }
        }
        PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
        break;
    }
    case kPetMenuReload:
        PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
        break;
    case kPetMenuDelete:
        if (!pet.folderKey.empty()) {
            DWORD removed=1;
            if (configWritable && !pet.registryKey.empty())
                ConfigWriteResult(L"Removed",RegSetKeyValueW(imagesRoot,pet.registryKey.c_str(),L"Removed",REG_DWORD,&removed,sizeof(removed)));
        } else {
            auto& paths = g_settings.imagePaths;
            auto key=std::find(imageOrder.begin(),imageOrder.end(),pet.registryKey);
            size_t index=static_cast<size_t>(key-imageOrder.begin());
            if (key!=imageOrder.end() && index<paths.size()) paths.erase(paths.begin()+index);
            else if (auto found=std::find(paths.begin(),paths.end(),pet.filePath); found!=paths.end()) paths.erase(found);
            SaveImagePaths(paths,pet.registryKey);
        }
        PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
        break;
    }

}
static void ShowAllHidden();
namespace xaml = winrt::Windows::UI::Xaml;
namespace controls = xaml::Controls;
namespace hosting = xaml::Hosting;
using controls::StackPanel;
using controls::TextBlock;
using controls::Button;

static hosting::WindowsXamlManager managerRuntime{nullptr};
static hosting::DesktopWindowXamlSource managerSource{nullptr};
static winrt::com_ptr<IDesktopWindowXamlSourceNative2> managerInput;
static HWND islandWindow{};
static controls::Grid managerRoot{nullptr};
enum class Page { Pet, Workshop, App };
static Page page = Page::Pet;
static bool updatingList = false;

template<class T> static T Named(const wchar_t* name) {
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
    return xaml::Media::SolidColorBrush({255, GetRValue(color), GetGValue(color), GetBValue(color)});
}
static bool LightManager() {
    return g_managerTheme == ManagerTheme::Light ||
        (g_managerTheme == ManagerTheme::System && SystemPrefersLightTheme());
}
static auto ManagerAccent() {
    return managerRoot.Resources().Lookup(winrt::box_value(L"AnimaAccent")).as<xaml::Media::SolidColorBrush>();
}
struct NavigationMotion {
    xaml::FrameworkElement anchor{nullptr};
    double x{}, y{}, height{16}, startX{}, startY{}, startHeight{}, targetX{}, targetY{};
    ULONGLONG started{};
    bool visible{};
};
static NavigationMotion navMotion;
static constexpr UINT_PTR kNavTimer=91;

static void DrawNavPill() {
    auto transform=Named<xaml::Media::CompositeTransform>(L"NavTransform");
    transform.TranslateX(navMotion.x);
    transform.TranslateY(navMotion.y-navMotion.height/2);
    transform.ScaleY(navMotion.height/16);
}
static void StepNavPill(HWND hwnd) {
    auto& m=navMotion;
    if (!m.started) return;
    double t=std::min(1.0,(GetTickCount64()-m.started)/260.0);
    double ease=1-std::pow(1-t,3);
    double wave=std::sin(3.141592653589793*t);
    double distance=m.targetY-m.startY;
    m.x=m.startX+(m.targetX-m.startX)*ease;
    m.y=m.startY+distance*ease+std::clamp(distance*0.06,-5.0,5.0)*std::sin(2*3.141592653589793*t)*(-t);
    m.height=16+(m.startHeight-16)*(1-ease)+std::min(22.0,std::abs(distance)*0.28)*wave*wave;
    DrawNavPill();
    if (t>=1) { KillTimer(hwnd,kNavTimer); m.started=0; }
}
static void PositionNavPill(HWND hwnd) {
    if (!managerRoot || updatingList) return;
    auto list=Named<controls::ListView>(L"Pets");
    auto overlay=Named<controls::Canvas>(L"NavOverlay");
    xaml::FrameworkElement anchor{nullptr};
    if (page==Page::Pet) {
        auto item=list.SelectedItem();
        if (item) anchor=item.try_as<controls::ListViewItem>();
    } else anchor=Named<Button>(page==Page::App ? L"App" : L"Workshop");
    auto pill=Named<controls::Border>(L"NavPill");
    bool visible=anchor && anchor.IsLoaded() && anchor.ActualHeight()>0;
    winrt::Windows::Foundation::Point pt{};
    if (visible) {
        pt=anchor.TransformToVisual(overlay).TransformPoint({page==Page::Pet ? 4.f : 3.f,(float)(anchor.ActualHeight()/2)});
        if (page==Page::Pet) {
            auto top=list.TransformToVisual(overlay).TransformPoint({0,0}).Y;
            visible=pt.Y-8>=top && pt.Y+8<=top+list.ActualHeight();
        }
    }
    auto& m=navMotion;
    if (!visible) {
        if (m.visible) pill.Opacity(0);
        KillTimer(hwnd,kNavTimer);
        m=NavigationMotion{};
        return;
    }
    if (m.anchor==anchor && m.targetX==pt.X && m.targetY==pt.Y) return;
    BOOL animate=TRUE;
    bool move=m.visible && m.anchor!=anchor;
    m.anchor=anchor;
    m.targetX=pt.X; m.targetY=pt.Y;
    if (move && SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animate,0) && animate &&
        SetTimer(hwnd,kNavTimer,16,nullptr)) {
        m.startX=m.x; m.startY=m.y; m.startHeight=m.height;
        m.started=GetTickCount64();
    } else {
        KillTimer(hwnd,kNavTimer);
        m.started=0; m.x=pt.X; m.y=pt.Y; m.height=16;
        DrawNavPill();
    }
    if (!m.visible) pill.Opacity(1);
    m.visible=true;
}
static void UpdateNavigation() {
    const Page pages[]={Page::Workshop,Page::App};
    const wchar_t* names[]={L"Workshop",L"App"};
    for (int i=0;i<2;++i) {
        auto button=Named<Button>(names[i]);
        bool selected=pages[i]==page;
        button.Background(xaml::Media::SolidColorBrush(selected
            ? (LightManager() ? winrt::Windows::UI::Color{16,0,0,0} : winrt::Windows::UI::Color{20,255,255,255})
            : winrt::Windows::UI::Color{0,0,0,0}));
    }
    if (g_hManagerWnd) PositionNavPill(g_hManagerWnd);
}
static constexpr UINT kApplyManagerAppearance=WM_APP+32;
static int appliedTheme=-1;
static COLORREF appliedAccent=CLR_INVALID;
static bool StyleManager(HWND hwnd) {
    BOOL dark = !LightManager();
    COLORREF colorValue=g_accentIndex==kAccentIndexSystem ? GetSystemAccentColor() : g_accentChoices[g_accentIndex];
    if (appliedTheme==dark && appliedAccent==colorValue) return false;
    bool themeChanged=appliedTheme!=dark;
    appliedTheme=dark;
    appliedAccent=colorValue;
    if (themeChanged) {
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
        DWORD corners = 2;
        DwmSetWindowAttribute(hwnd, 33, &corners, sizeof(corners));
        COLORREF caption = dark ? RGB(30,30,30) : RGB(243,243,243);
        DwmSetWindowAttribute(hwnd,35,&caption,sizeof(caption));
        managerRoot.RequestedTheme(dark ? xaml::ElementTheme::Dark : xaml::ElementTheme::Light);
        managerRoot.Background(Brush(dark ? RGB(30,30,30) : RGB(243,243,243)));
    }
    auto accent = ManagerAccent();
    accent.Color(Brush(colorValue).Color());
    UpdateNavigation();
    auto primary = Named<Button>(L"Import");
    primary.Background(accent);
    auto importOptions=Named<Button>(L"ImportOptions");
    importOptions.Background(accent);
    auto color = accent.Color();
    primary.Foreground(Brush((299*color.R + 587*color.G + 114*color.B >= 148000)
        ? RGB(0,0,0) : RGB(255,255,255)));
    importOptions.Foreground(primary.Foreground());
    return true;
}
static StackPanel Group(StackPanel parent, const wchar_t* title) {
    controls::Border card;
    if (*title) {
        auto heading = Label(title,14);
        heading.FontWeight({600});
        heading.Margin({2,8,0,-4});
        parent.Children().Append(heading);
    }
    card.Padding(*title ? xaml::Thickness{14,2,14,2} : xaml::Thickness{14,12,14,12});
    card.BorderThickness({1,1,1,1});
    card.BorderBrush(Brush(LightManager() ? RGB(222,222,222) : RGB(56,56,56)));
    card.CornerRadius({8,8,8,8});
    card.Background(Brush(LightManager() ? RGB(252,252,252) : RGB(44,44,44)));
    StackPanel body;
    body.Spacing(*title ? 0 : 8);
    card.Child(body);
    parent.Children().Append(card);
    return body;
}
template<class F> static Button Action(StackPanel parent, const wchar_t* title, F run) {
    Button button;
    button.Content(winrt::box_value(winrt::hstring(title)));
    button.Click([run](auto const&, auto const&) { run(); });
    parent.Children().Append(button);
    return button;
}
static void SyncChoiceState(controls::Primitives::ToggleButton const& button) {
    auto sync=[](auto const& sender,auto const&) {
        auto control=sender.template as<controls::Primitives::ToggleButton>();
        auto checked=control.IsChecked();
        xaml::VisualStateManager::GoToState(control,checked && checked.Value() ? L"AnimaOn" : L"AnimaOff",false);
    };
    button.Loaded(sync);
    button.Checked(sync);
    button.Unchecked(sync);
}
static controls::Grid FieldRow(StackPanel parent) {
    if (parent.Children().Size()) {
        controls::Border divider;
        divider.Height(1);
        divider.Background(Brush(LightManager() ? RGB(230,230,230) : RGB(56,56,56)));
        parent.Children().Append(divider);
    }
    controls::Grid grid;
    grid.MinHeight(40);
    grid.Padding({2,4,2,4});
    controls::ColumnDefinition left,right;
    right.Width({1,xaml::GridUnitType::Auto});
    grid.ColumnDefinitions().Append(left);
    grid.ColumnDefinitions().Append(right);
    grid.ColumnSpacing(12);
    parent.Children().Append(grid);
    return grid;
}
static controls::Grid SettingRow(StackPanel parent,const wchar_t* title,const wchar_t* description = nullptr) {
    auto row = FieldRow(parent);
    StackPanel text;
    text.VerticalAlignment(xaml::VerticalAlignment::Center);
    text.Children().Append(Label(title));
    if (description) {
        auto note = Label(description,12);
        note.Opacity(0.70);
        text.Children().Append(note);
    }
    row.Children().Append(text);
    return row;
}
template<class F> static auto Toggle(StackPanel parent,const wchar_t* title,bool on,F run) {
    auto row=SettingRow(parent,title);
    controls::Primitives::ToggleButton toggle;
    toggle.Template(managerRoot.Resources().Lookup(winrt::box_value(L"PillSwitch")).as<controls::ControlTemplate>());
    toggle.UseSystemFocusVisuals(true);
    toggle.Background(ManagerAccent());
    SyncChoiceState(toggle);
    toggle.IsChecked(on);
    xaml::Automation::AutomationProperties::SetName(toggle,title);
    toggle.Checked([run](auto&&,auto&&) { run(true); });
    toggle.Unchecked([run](auto&&,auto&&) { run(false); });
    controls::Grid::SetColumn(toggle,1);
    row.Children().Append(toggle);
    return toggle;
}
template<class F> static void Choice(StackPanel parent,const wchar_t* title,
        std::initializer_list<const wchar_t*> options,int selected,F run) {
    auto row=SettingRow(parent,title);
    controls::Grid choices;
    choices.ColumnSpacing(4);
    choices.RowSpacing(4);
    for (int i=0;i<3;++i) choices.ColumnDefinitions().Append(controls::ColumnDefinition());
    auto style=managerRoot.Resources().Lookup(winrt::box_value(L"RadioPill")).as<controls::ControlTemplate>();
    int i=0;
    for (auto option : options) {
        if (i%3==0) choices.RowDefinitions().Append(controls::RowDefinition());
        controls::RadioButton radio;
        radio.GroupName(title);
        radio.Template(style);
        radio.Background(ManagerAccent());
        radio.MinWidth(0);
        radio.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
        radio.UseSystemFocusVisuals(true);
        radio.Content(winrt::box_value(winrt::hstring(option)));
        SyncChoiceState(radio);
        radio.IsChecked(i==selected);
        controls::Grid::SetRow(radio,i/3);
        controls::Grid::SetColumn(radio,i%3);
        choices.Children().Append(radio);
        radio.Checked([run,i](auto&&,auto&&) { run(i); });
        ++i;
    }
    controls::Grid::SetColumn(choices,1);
    row.Children().Append(choices);
}
template<class F> static void Dropdown(StackPanel parent,const wchar_t* title,const wchar_t* description,
        std::initializer_list<const wchar_t*> options,int selected,F run) {
    auto row = SettingRow(parent,title,description);
    auto values = std::vector<std::wstring>(options.begin(),options.end());
    Button field;
    field.Width(156);
    field.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
    controls::Grid face;
    auto value = Label(values.at(selected));
    value.Margin({0,0,20,0});
    controls::FontIcon arrow;
    arrow.Glyph(L"\uE70D");
    arrow.FontSize(12);
    arrow.HorizontalAlignment(xaml::HorizontalAlignment::Right);
    face.Children().Append(value);
    face.Children().Append(arrow);
    field.Content(face);
    xaml::Automation::AutomationProperties::SetName(field,std::wstring(title)+L", "+values[selected]);
    controls::Grid::SetColumn(field,1);
    row.Children().Append(field);

    controls::Flyout menu;
    menu.Placement(controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    menu.FlyoutPresenterStyle(managerRoot.Resources().Lookup(winrt::box_value(L"OptionMenu")).as<xaml::Style>());
    auto weakMenu = winrt::make_weak(menu);
    auto current = std::make_shared<int>(selected);
    auto changed=std::make_shared<bool>(false);
    menu.Closed([changed](auto&&,auto&&) {
        if (*changed && g_hManagerWnd) PostMessageW(g_hManagerWnd,kApplyManagerAppearance,1,0);
        *changed=false;
    });
    StackPanel list;
    list.RequestedTheme(managerRoot.RequestedTheme());
    list.Width(field.Width()-10);
    for (int i=0;i<(int)values.size();++i) {
        Button option;
        option.Template(managerRoot.Resources().Lookup(winrt::box_value(L"ActionFace")).as<controls::ControlTemplate>());
        option.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
        option.HorizontalContentAlignment(xaml::HorizontalAlignment::Left);
        option.Padding({4,5,4,5});
        option.BorderThickness({0,0,0,0});
        option.CornerRadius({4,4,4,4});
        option.Background(xaml::Media::SolidColorBrush(LightManager()
            ? winrt::Windows::UI::Color{static_cast<BYTE>(i==*current ? 16 : 0),0,0,0}
            : winrt::Windows::UI::Color{static_cast<BYTE>(i==*current ? 20 : 0),255,255,255}));
        StackPanel line;
        line.Orientation(controls::Orientation::Horizontal);
        line.Spacing(8);
        controls::Border mark;
        mark.Width(3); mark.Height(16); mark.CornerRadius({1.5,1.5,1.5,1.5});
        mark.Background(ManagerAccent());
        mark.Opacity(i==*current ? 1 : 0);
        line.Children().Append(mark);
        line.Children().Append(Label(values[i]));
        option.Content(line);
        xaml::Automation::AutomationProperties::SetName(option,values[i]);
        option.Click([weakMenu,current,i,run,changed](auto&&,auto&&) {
            auto menu=weakMenu.get();
            if (*current!=i) {
                *current=i;
                *changed=true;
                run(i);
            }
            if (menu) menu.Hide();
        });
        list.Children().Append(option);
    }
    menu.Content(list);
    field.Flyout(menu);
}
static void ReloadItems() {
    PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
}
static void AddImages() {
    wchar_t files[32768]{};
    OPENFILENAMEW picker{sizeof(picker)};
    picker.hwndOwner = g_hManagerWnd;
    picker.lpstrFilter = L"Supported media\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.webp;*.apng;*.mp4\0MP4 video (experimental)\0*.mp4\0All files\0*.*\0";
    picker.lpstrFile = files;
    picker.nMaxFile = std::size(files);
    picker.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&picker)) return;
    std::wstring first = files;
    const wchar_t* next = files + first.size() + 1;
    if (!*next) g_settings.imagePaths.push_back(first);
    else for (; *next; next += wcslen(next) + 1)
        g_settings.imagePaths.push_back(first + L"\\" + next);
    g_settings.useFolderMode = false;
    SaveNumber(L"UseFolderMode",0);
    SaveImagePaths(g_settings.imagePaths);
    ReloadItems();
}
static controls::Image Thumbnail(HBITMAP handle, int width, int height) {
    controls::Image image;
    image.Width(width);
    image.Height(height);
    BITMAP bm{};
    if (handle && GetObjectW(handle,sizeof(bm),&bm)) {
        xaml::Media::Imaging::WriteableBitmap bitmap(bm.bmWidth,bm.bmHeight);
        BITMAPINFO info{};
        info.bmiHeader = {sizeof(BITMAPINFOHEADER),bm.bmWidth,-bm.bmHeight,1,32,BI_RGB};
        auto buffer = bitmap.PixelBuffer();
        BYTE* bytes{};
        winrt::check_hresult(buffer.as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
        buffer.Length((uint32_t)(bm.bmWidth*bm.bmHeight*4));
        HDC dc = GetDC(nullptr);
        int lines = GetDIBits(dc,handle,0,bm.bmHeight,bytes,&info,DIB_RGB_COLORS);
        ReleaseDC(nullptr,dc);
        if (lines == bm.bmHeight) {
            for (unsigned i=3; i<buffer.Length(); i+=4) bytes[i]=255;
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
    more.Padding({5,0,5,0});
    more.Width(28);
    more.Background(xaml::Media::SolidColorBrush({0,0,0,0}));
    xaml::Automation::AutomationProperties::SetName(more,L"Actions for "+name);
    return more;
}
static void PetList() {
    auto list = Named<controls::ListView>(L"Pets");
    auto query = Named<controls::TextBox>(L"Search").Text();
    updatingList = true;
    list.Items().Clear();
    auto appendItem=[&](int i) {
        auto& pet=g_items[i];
        if (!query.empty() && !StrStrIW(pet.fileName.c_str(),query.c_str())) return;
        controls::ListViewItem row;
        row.Template(managerRoot.Resources().Lookup(winrt::box_value(L"PetRowFace")).as<controls::ControlTemplate>());
        row.UseSystemFocusVisuals(true);
        row.Tag(winrt::box_value(i));
        controls::Grid content;
        for (auto unit : {xaml::GridUnitType::Auto,xaml::GridUnitType::Auto,xaml::GridUnitType::Star,xaml::GridUnitType::Auto}) {
            controls::ColumnDefinition column;
            column.Width({1,unit});
            content.ColumnDefinitions().Append(column);
        }
        content.ColumnDefinitions().GetAt(0).Width({3,xaml::GridUnitType::Pixel});
        content.ColumnSpacing(4);
        row.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
        if (!pet.hThumb) pet.hThumb = CreateThumb(pet);
        auto thumb=Thumbnail(pet.hThumb,36,36);
        controls::Grid::SetColumn(thumb,1);
        content.Children().Append(thumb);
        auto name = Label(pet.fileName);
        name.VerticalAlignment(xaml::VerticalAlignment::Center);
        name.TextWrapping(xaml::TextWrapping::NoWrap);
        name.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        controls::Grid::SetColumn(name,2);
        content.Children().Append(name);
        Button more=OverflowButton(pet.fileName);
        more.Click([i,key=pet.registryKey,path=pet.filePath](auto const& sender,auto const&) {
            auto current=[i,key,path] {
                return i<(int)g_items.size() && g_items[i].registryKey==key && g_items[i].filePath==path;
            };
            if (!current()) return;
            controls::MenuFlyout menu;
            auto add=[&](PCWSTR caption,int command,bool enabled=true) {
                controls::MenuFlyoutItem item;
                item.Template(managerRoot.Resources().Lookup(winrt::box_value(L"MenuEntryFace")).as<controls::ControlTemplate>());
                item.UseSystemFocusVisuals(true);
                item.Text(caption);
                item.IsEnabled(enabled);
                item.Click([i,current,command](auto&&,auto&&) {
                    if (current()) RunImageMenuAction(i,command);
                });
                menu.Items().Append(item);
            };
            const auto& media=g_items[i];
            add(media.hidden ? L"Show" : L"Hide",media.hidden ? kPetMenuShow : kPetMenuHide);
            add(L"Bring to front",kPetMenuBringToFront);
            menu.Items().Append(controls::MenuFlyoutSeparator{});
            add(L"Reset position",kPetMenuResetPos);
            add(L"Clone",kPetMenuClone,media.folderKey.empty());
            add(L"Reload image",kPetMenuReload);
            menu.Items().Append(controls::MenuFlyoutSeparator{});
            add(L"Remove from list",kPetMenuDelete,media.folderKey.empty() || configWritable);
            menu.ShowAt(sender.template as<Button>());
        });
        controls::Grid::SetColumn(more,3);
        content.Children().Append(more);
        if (!pet.folderKey.empty()) row.Margin({12,1,0,1});
        row.Content(content);
        list.Items().Append(row);
        if (page == Page::Pet && i == g_managerSel) list.SelectedItem(row);
    };
    for (int i=0;i<(int)g_items.size();++i) if (g_items[i].folderKey.empty()) appendItem(i);
    for (int n=0;n<(int)libraryFolders.size();++n) {
        const auto& folder=libraryFolders[n];
        if (std::none_of(g_items.begin(),g_items.end(),[&](const PetItem& item) {
            return item.folderKey==folder.key;
        })) continue;
        controls::ListViewItem header;
        header.Tag(winrt::box_value(-1-n));
        header.HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
        controls::Grid line;
        for (auto unit : {xaml::GridUnitType::Auto,xaml::GridUnitType::Auto,xaml::GridUnitType::Star,xaml::GridUnitType::Auto}) {
            controls::ColumnDefinition column;
            column.Width({1,unit});
            line.ColumnDefinitions().Append(column);
        }
        line.ColumnSpacing(6);
        controls::FontIcon arrow,icon;
        bool expanded=folder.expanded || !query.empty();
        arrow.Glyph(expanded ? L"\uE70D" : L"\uE76C"); arrow.FontSize(10);
        icon.Glyph(L"\uE8B7"); icon.FontSize(16);
        line.Children().Append(arrow);
        controls::Grid::SetColumn(icon,1);
        line.Children().Append(icon);
        auto name=GetFileNameFromPath(folder.path);
        if (name.empty()) name=folder.path;
        auto caption=Label(name);
        caption.TextWrapping(xaml::TextWrapping::NoWrap);
        caption.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        caption.VerticalAlignment(xaml::VerticalAlignment::Center);
        controls::Grid::SetColumn(caption,2);
        line.Children().Append(caption);
        Button more=OverflowButton(name);
        controls::MenuFlyout menu;
        controls::MenuFlyoutItem remove;
        remove.Template(managerRoot.Resources().Lookup(winrt::box_value(L"MenuEntryFace")).as<controls::ControlTemplate>());
        remove.UseSystemFocusVisuals(true);
        remove.Text(L"Remove from list");
        remove.IsEnabled(configWritable);
        remove.Click([key=folder.key](auto&&,auto&&) {
            if (RemoveLibraryFolder(key)) ReloadItems();
        });
        menu.Items().Append(remove);
        more.Flyout(menu);
        controls::Grid::SetColumn(more,3);
        line.Children().Append(more);
        header.Content(line);
        controls::ToolTipService::SetToolTip(header,winrt::box_value(winrt::hstring(folder.path)));
        list.Items().Append(header);
        if (expanded) for (int i=0;i<(int)g_items.size();++i)
            if (g_items[i].folderKey==folder.key) appendItem(i);
    }
    updatingList = false;
}
static PetItem* SelectedPet() {
    return g_managerSel >= 0 && g_managerSel < (int)g_items.size() ? &g_items[g_managerSel] : nullptr;
}
static controls::Grid SliderFace(controls::Slider const& slider) {
    controls::Grid host,face;
    host.Height(36);
    face.IsHitTestVisible(false);
    face.Margin({10,0,10,0});
    controls::Border rail,fill,knob,dot;
    for (auto part : {rail,fill}) {
        part.Height(4);
        part.CornerRadius({2,2,2,2});
        part.VerticalAlignment(xaml::VerticalAlignment::Center);
        face.Children().Append(part);
    }
    rail.Background(Brush(LightManager() ? RGB(206,206,206) : RGB(94,94,94)));
    fill.Background(ManagerAccent());
    fill.HorizontalAlignment(xaml::HorizontalAlignment::Left);
    knob.Width(20); knob.Height(20);
    knob.Margin({-10,0,-10,0});
    knob.CornerRadius({10,10,10,10});
    knob.HorizontalAlignment(xaml::HorizontalAlignment::Left);
    knob.VerticalAlignment(xaml::VerticalAlignment::Center);
    knob.Background(Brush(LightManager() ? RGB(252,252,252) : RGB(67,67,67)));
    knob.BorderBrush(rail.Background());
    knob.BorderThickness({1,1,1,1});
    dot.Width(10); dot.Height(10);
    dot.HorizontalAlignment(xaml::HorizontalAlignment::Center);
    dot.VerticalAlignment(xaml::VerticalAlignment::Center);
    dot.Background(ManagerAccent()); dot.CornerRadius({7,7,7,7});
    knob.Child(dot);
    xaml::Media::TranslateTransform position;
    knob.RenderTransform(position);
    face.Children().Append(knob);
    slider.Opacity(0);
    auto weakSlider=winrt::make_weak(slider);
    auto weakFace=winrt::make_weak(face);
    auto update=[weakSlider,weakFace,position,fill] {
        auto s=weakSlider.get();
        auto f=weakFace.get();
        if (!s || !f || s.Maximum()<=s.Minimum()) return;
        double x=f.ActualWidth()*(s.Value()-s.Minimum())/(s.Maximum()-s.Minimum());
        fill.Width(x); position.X(x);
    };
    face.SizeChanged([update](auto&&,auto&&) { update(); });
    slider.ValueChanged([update](auto&&,auto&&) { update(); });
    slider.PointerEntered([dot](auto&&,auto&&) { dot.Width(14); dot.Height(14); });
    slider.PointerExited([dot](auto&&,auto&&) { dot.Width(10); dot.Height(10); });
    slider.GotFocus([knob](auto&&,auto&&) { knob.BorderBrush(ManagerAccent()); knob.BorderThickness({2,2,2,2}); });
    slider.LostFocus([knob,rail](auto&&,auto&&) { knob.BorderBrush(rail.Background()); knob.BorderThickness({1,1,1,1}); });
    host.Children().Append(slider);
    host.Children().Append(face);
    return host;
}
static int MonitorRefreshRate(HWND hwnd) {
    MONITORINFOEXW info{};
    info.cbSize=sizeof(info);
    DEVMODEW mode{};
    mode.dmSize=sizeof(mode);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST),&info) &&
        EnumDisplaySettingsW(info.szDevice,ENUM_CURRENT_SETTINGS,&mode) && mode.dmDisplayFrequency>=5)
        return (int)mode.dmDisplayFrequency;
    return 60;
}
static void PetPage(StackPanel body) {
    auto* pet = SelectedPet();
    if (!pet) {
        body.Children().Append(Label(L"Select an item from the media library or import a file."));
        Action(body,L"Import media…",AddImages);
        return;
    }
    auto behavior = Group(body,L"Behavior");
    auto flag = [&](const wchar_t* name, bool PetItem::*field) {
        Toggle(behavior,name,pet->*field,[field](bool value) {
            auto* item = SelectedPet();
            if (!item) return;
            item->*field = value;
            SyncImageState(*item,false);
            ApplyTopmost(*item);
            ApplyClickThrough(*item);
            if (item->hwnd) ShowWindow(item->hwnd,item->enabled && !item->hidden ? SW_SHOWNOACTIVATE : SW_HIDE);
        });
    };
    flag(L"Enabled",&PetItem::enabled);
    flag(L"Always on top",&PetItem::alwaysOnTop);
    flag(L"Click-through",&PetItem::clickThrough);
    flag(L"Lock position",&PetItem::lockPosition);
    if (pet->frameCount > 1) flag(L"Pause animation",&PetItem::paused);
    auto appearance = Group(body,L"Appearance");
    auto slider = [&](const wchar_t* name,int low,int high,int PetItem::*field) {
        controls::Slider control;
        xaml::Automation::AutomationProperties::SetName(control,name);
        control.Maximum(high);
        control.Minimum(low);
        control.StepFrequency(1);
        control.Value(std::clamp(pet->*field,low,high));
        controls::TextBox number;
        xaml::Automation::AutomationProperties::SetName(number,name);
        number.Width(60);
        number.HorizontalAlignment(xaml::HorizontalAlignment::Left);
        number.Text(std::to_wstring((int)control.Value()));
        auto weakSlider=winrt::make_weak(control);
        number.TextChanging([weakSlider,low,high](controls::TextBox const& box,auto const&) {
            auto text=box.Text();
            wchar_t* end=nullptr;
            long value=wcstol(text.c_str(),&end,10);
            if (end==text.c_str() || *end || value<low || value>high) return;
            if (auto slider=weakSlider.get(); slider && slider.Value()!=value) slider.Value(value);
        });
        number.LostFocus([weakSlider,low,high](auto const& sender,auto const&) {
            auto box=sender.template as<controls::TextBox>();
            if (auto slider=weakSlider.get()) {
                auto text=box.Text();
                wchar_t* end=nullptr;
                long value=wcstol(text.c_str(),&end,10);
                if (end!=text.c_str() && *end==0)
                    slider.Value(std::clamp(value,(long)low,(long)high));
                box.Text(std::to_wstring((int)slider.Value()));
            }
        });
        int index=g_managerSel;
        control.ValueChanged([field,number,index](auto const&, controls::Primitives::RangeBaseValueChangedEventArgs const& args) {
            if (index != g_managerSel) return;
            if (auto* item = SelectedPet()) {
                item->*field = (int)args.NewValue();
                SyncImageState(*item,false);
                auto text=number.Text();
                wchar_t* end=nullptr;
                long typed=wcstol(text.c_str(),&end,10);
                if (end==text.c_str() || *end || typed!=item->*field)
                    number.Text(std::to_wstring(item->*field));
                if (field==&PetItem::fpsLimit) ApplyTimer();
                else RenderItem(*item);
            }
        });
        auto row=SettingRow(appearance,name);
        row.ColumnDefinitions().GetAt(0).Width({2,xaml::GridUnitType::Star});
        row.ColumnDefinitions().GetAt(1).Width({3,xaml::GridUnitType::Star});
        controls::ColumnDefinition numeric;
        numeric.Width({1,xaml::GridUnitType::Auto});
        row.ColumnDefinitions().Append(numeric);
        auto track=SliderFace(control);
        controls::Grid::SetColumn(track,1);
        row.Children().Append(track);
        controls::Grid::SetColumn(number,2);
        row.Children().Append(number);
    };
    slider(L"Scale (%)",1,500,&PetItem::scalePercent);
    slider(L"Rotation (°)",0,359,&PetItem::rotationDegrees);
    slider(L"Opacity (%)",10,100,&PetItem::opacityPercent);
    if (pet->isGif) slider(L"FPS Limit",5,pet->monitorHz,&PetItem::fpsLimit);
}
static void ImportFolder() {
    BROWSEINFOW info{};
    info.hwndOwner=g_hManagerWnd;
    info.lpszTitle=L"Import media from folder";
    info.ulFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
    if (auto pidl=SHBrowseForFolderW(&info)) {
        wchar_t path[MAX_PATH]{};
        bool picked=SHGetPathFromIDListW(pidl,path)!=FALSE;
        CoTaskMemFree(pidl);
        if (picked && RegisterLibraryFolder(path)) ReloadItems();
        else if (picked) MessageBoxW(g_hManagerWnd,L"The folder could not be saved.",L"Import media",MB_ICONWARNING);
    }
}
static void WorkshopCards(StackPanel cards) {
    cards.Children().Clear();
    if (g_workshopFetching || !g_workshopError.empty()) {
        cards.Children().Append(Label(g_workshopFetching ? L"Loading workshop…" : g_workshopError));
        return;
    }
    auto matches = Workshop_FilteredIndices();
    if (matches.empty()) cards.Children().Append(Label(L"No matching media."));
    controls::Grid row{nullptr};
    int columns = g_workshopView == WorkshopView::List ? 1 :
        (g_workshopView == WorkshopView::TwoCol ? 2 : std::max(2,(int)(managerRoot.ActualWidth()-280)/210));
    int slot = 0;
    for (int idx : matches) {
        if (!slot) {
            row = controls::Grid();
            row.ColumnSpacing(8);
            for (int i=0;i<columns;++i) row.ColumnDefinitions().Append(controls::ColumnDefinition());
            cards.Children().Append(row);
        }
        StackPanel cell;
        controls::Grid::SetColumn(cell,slot);
        row.Children().Append(cell);
        slot = (slot+1)%columns;
        const auto& item = g_workshopItems[idx];
        auto card = Group(cell,L"");
        if (!item.localPreviewPath.empty()) {
            auto& bmp = g_workshopItems[idx].hPreviewBmp;
            if (!bmp) bmp = Workshop_LoadPreview(item.localPreviewPath,128,96);
            if (bmp) card.Children().Append(Thumbnail(bmp,108,81));
        }
        card.Children().Append(Label(item.name,18));
        card.Children().Append(Label(item.author + L"  ·  " + item.version));
        card.Children().Append(Label(item.description));
        std::wstring tags;
        for (const auto& tag : item.tags) {
            if (!tags.empty()) tags += L"  ";
            tags += L"#" + tag;
        }
        if (!tags.empty()) card.Children().Append(Label(tags,12));
        auto install = Action(card,item.installing ? L"Installing…" : item.installed ? L"Reinstall" : L"Install",[idx] {
            Workshop_StartInstall(idx);
            RenderPage();
        });
        install.IsEnabled(!item.installing);
        if (item.installed) Action(card,L"Uninstall",[idx] { Workshop_Uninstall(idx); RenderPage(); }).IsEnabled(!item.installing);
    }
}
static void WorkshopPage(StackPanel body) {
    if (!g_workshopFetched && !g_workshopFetching) Workshop_StartFetch();
    controls::TextBox search;
    search.PlaceholderText(L"Search names, authors or tags");
    search.Text(g_workshopFilter);
    auto tools = Group(body,L"Browse");
    auto toolbar = StackPanel();
    toolbar.Orientation(controls::Orientation::Horizontal);
    toolbar.Spacing(8);
    toolbar.Margin({0,4,0,4});
    search.MinWidth(160);
    search.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
    auto searchRow=FieldRow(tools);
    searchRow.Children().Append(search);
    controls::Grid::SetColumn(toolbar,1);
    searchRow.Children().Append(toolbar);
    StackPanel cards;
    cards.Spacing(8);
    search.TextChanged([cards](auto const& sender, auto const&) {
        g_workshopFilter = sender.template as<controls::TextBox>().Text().c_str();
        for (auto& c : g_workshopFilter) c = (wchar_t)towlower(c);
        WorkshopCards(cards);
    });
    Choice(tools,L"Layout",{L"Grid",L"List",L"Two columns"},(int)g_workshopView,[cards](int index) {
        g_workshopView = (WorkshopView)index;
        SaveNumber(L"WorkshopLayout",index);
        WorkshopCards(cards);
    });
    auto refresh = Action(toolbar,L"Refresh",[] { Workshop_StartFetch(); RenderPage(); });
    refresh.IsEnabled(!g_workshopFetching && std::none_of(g_workshopItems.begin(),g_workshopItems.end(),
        [](auto& item) { return item.installing; }));
    body.Children().Append(cards);
    WorkshopCards(cards);
}
static constexpr wchar_t managerRunKey[]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static ULONGLONG CurrentLogon() {
    HANDLE token{};
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) return 0;
    TOKEN_STATISTICS stats{};
    DWORD bytes{};
    bool ok=GetTokenInformation(token,TokenStatistics,&stats,sizeof(stats),&bytes)!=FALSE;
    CloseHandle(token);
    return ok ? (static_cast<ULONGLONG>(static_cast<DWORD>(stats.AuthenticationId.HighPart))<<32)|stats.AuthenticationId.LowPart : 0;
}
static bool ManagerAtSignIn() {
    DWORD size{};
    return RegGetValueW(HKEY_CURRENT_USER,managerRunKey,L"AnimaWindhawkEngine",RRF_RT_REG_SZ,nullptr,nullptr,&size)==ERROR_SUCCESS;
}
static LSTATUS SetManagerStartup(bool enabled) {
    if (!enabled) return RegDeleteKeyValueW(HKEY_CURRENT_USER,managerRunKey,L"AnimaWindhawkEngine");
    std::wstring host(32768,L'\0');
    DWORD length=GetModuleFileNameW(nullptr,host.data(),(DWORD)host.size());
    if (!length || length>=host.size()) return ERROR_BAD_PATHNAME;
    host.resize(length);
    auto launcher=host.substr(0,host.find_last_of(L"\\")+1)+L"windhawk.exe";
    if (GetFileAttributesW(launcher.c_str())==INVALID_FILE_ATTRIBUTES) return ERROR_FILE_NOT_FOUND;
    std::wstring command=L"\""+launcher+L"\" -tray-only";
    if (StringAt(HKEY_CURRENT_USER,managerRunKey,L"AnimaWindhawkEngine")==command) return ERROR_SUCCESS;
    return RegSetKeyValueW(HKEY_CURRENT_USER,managerRunKey,L"AnimaWindhawkEngine",REG_SZ,
        command.c_str(),(DWORD)((command.size()+1)*sizeof(wchar_t)));
}
static bool FirstManagerStartThisLogon() {
    auto id=CurrentLogon();
    std::wstring marker=L"Volatile Environment\\AnimaWindhawkEngine-"+std::to_wstring(id);
    RegistryKey key;
    DWORD disposition{};
    return id && RegCreateKeyExW(HKEY_CURRENT_USER,marker.c_str(),0,nullptr,REG_OPTION_VOLATILE,KEY_QUERY_VALUE,nullptr,&key.key,&disposition)==ERROR_SUCCESS &&
        disposition==REG_CREATED_NEW_KEY;
}
static void OpenManagerAtSignIn() {
    if (!ManagerAtSignIn()) return;
    ConfigWriteResult(L"Manager startup",SetManagerStartup(true));
    if (FirstManagerStartThisLogon()) PostThreadMessageW(g_threadId,WM_APP_OPEN_MANAGER,static_cast<WPARAM>(-1),0);
}
static void AppPage(StackPanel body) {
    auto colors = Group(body,L"Appearance");
    Dropdown(colors,L"Theme",L"Choose how the manager appears.",{L"Dark",L"Light",L"System"},(int)g_managerTheme,[](int index) {
        g_managerTheme = (ManagerTheme)index;
        SaveNumber(L"ManagerTheme",index);

    });
    Dropdown(colors,L"Accent",L"Used for selections and active controls.",{L"Purple",L"Blue",L"Green",L"Red",L"Orange",L"Windows accent"},g_accentIndex,[](int index) {
        g_accentIndex=index;
        SaveNumber(L"Accent",index);

    });
    auto startup = Group(body,L"Startup");
    Toggle(startup,L"Launch this manager at sign-in",ManagerAtSignIn(),[](bool on) {
        auto err=SetManagerStartup(on);
        if (on && err==ERROR_SUCCESS) FirstManagerStartThisLogon();
        if (err!=ERROR_SUCCESS) {
            MessageBoxW(g_hManagerWnd,L"Windows could not update the startup setting.",L"AnimaWindhawkEngine",MB_ICONWARNING);
            RenderPage();
        }
    });
    Toggle(startup,L"Tray icon",g_settings.trayIcon,[](bool on) {
        g_settings.trayIcon=on;
        SaveNumber(L"TrayIcon",on);
        ApplyTrayIcon();
    });
    auto keys = Group(body,L"Manager hotkey");
    auto shortcut = g_hotkeyCapturing ? std::wstring(L"Press a shortcut, or Escape to cancel.") : HotkeyToString(g_hotkeyMods,g_hotkeyVk);
    auto shortcutRow = SettingRow(keys,L"Open manager",shortcut.c_str());
    StackPanel shortcuts;
    shortcuts.Orientation(controls::Orientation::Horizontal);
    shortcuts.Spacing(6);
    controls::Grid::SetColumn(shortcuts,1);
    shortcutRow.Children().Append(shortcuts);
    Action(shortcuts,L"Set shortcut",[] {
        UnregisterHotKey(g_hCtrlWnd,HOTKEY_OPEN_MANAGER);
        StartKeyCapture();
        g_hotkeyCapturing = g_hKeyHook != nullptr;
        if (!g_hotkeyCapturing) ReapplyHotkey();
        RenderPage();
    });
    Action(shortcuts,L"Clear shortcut",[] {
        StopKeyCapture();
        g_hotkeyCapturing=false;
        g_hotkeyMods=g_hotkeyVk=0;
        SaveHotkey();
        ReapplyHotkey();
        RenderPage();
    });
    Action(body,L"Replay welcome guide",[] { ShowWelcomeGuide(); });
}
static void RenderPage() {
    if (!managerRoot) return;
    auto body = Named<StackPanel>(L"Body");
    body.Children().Clear();
    if (!configWritable) body.Children().Append(Label(L"Changes cannot be saved to your user registry. See the Windhawk log for the access or migration error."));
    const wchar_t* title = L"Media";
    switch (page) {
    case Page::Pet:
        if (SelectedPet()) title=SelectedPet()->fileName.c_str();
        PetPage(body); break;
    case Page::Workshop: title=L"Workshop"; WorkshopPage(body); break;
    case Page::App: title=L"App settings"; AppPage(body); break;
    }
    UpdateNavigation();
    Named<TextBlock>(L"Title").Text(title);
    Named<TextBlock>(L"Build").Text(std::wstring(L"v")+kModVersion+L" · "+g_repoCommit+L" · b."+g_windowsBuild);
}
static void Navigate(Page next,bool mediaChanged=false) {
    bool changed=page!=next || mediaChanged;
    page=next;
    if (page!=Page::Pet) Named<controls::ListView>(L"Pets").SelectedIndex(-1);
    Named<controls::ScrollViewer>(L"Scroll").ScrollToVerticalOffset(0);
    RenderPage();
    if (changed) {
        using xaml::Media::Animation::Storyboard;
        auto slide=managerRoot.Resources().Lookup(winrt::box_value(L"PageSlide")).as<Storyboard>();
        slide.Stop();
        BOOL animate=TRUE;
        if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animate,0) && animate) {
            Storyboard::SetTarget(slide.Children().GetAt(0),Named<xaml::Media::TranslateTransform>(L"PageOffset"));
            slide.Begin();
        }
    }
}
static HANDLE managerManifest = INVALID_HANDLE_VALUE;

static HANDLE LoadManagerManifest() {
    static constexpr char xml[] = R"MANIFEST(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
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
    if (!length) winrt::throw_last_error();
    if (length >= MAX_PATH) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE));
    if (!GetTempFileNameW(directory, L"AWX", 0, path)) winrt::throw_last_error();
    struct TempFile {
        const wchar_t* path;
        ~TempFile() { DeleteFileW(path); }
    } cleanup{path};

    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, nullptr, TRUNCATE_EXISTING,
                              FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (file == INVALID_HANDLE_VALUE) winrt::throw_last_error();
    DWORD written = 0;
    constexpr DWORD bytes = sizeof(xml) - 1;
    DWORD error = WriteFile(file, xml, bytes, &written, nullptr) ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);
    if (error) winrt::throw_hresult(HRESULT_FROM_WIN32(error));
    if (written != bytes) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_WRITE_FAULT));

    ACTCTXW description{};
    description.cbSize = sizeof(description);
    description.lpSource = path;
    HANDLE context = CreateActCtxW(&description);
    if (context == INVALID_HANDLE_VALUE) winrt::throw_last_error();
    return context;
}

class ManagerManifestScope {
    ULONG_PTR cookie_{};
public:
    ManagerManifestScope() {
        if (managerManifest == INVALID_HANDLE_VALUE) managerManifest = LoadManagerManifest();
        if (!ActivateActCtx(managerManifest, &cookie_)) winrt::throw_last_error();
    }
    ~ManagerManifestScope() { DeactivateActCtx(0, cookie_); }
    ManagerManifestScope(const ManagerManifestScope&) = delete;
    ManagerManifestScope& operator=(const ManagerManifestScope&) = delete;
};

static const wchar_t* managerStartupStage = L"Create manager window";
static std::wstring managerStartupError;

static void AttachIsland(HWND hwnd, hosting::DesktopWindowXamlSource& source, HWND& child,
                         winrt::com_ptr<IDesktopWindowXamlSourceNative2>& input, const wchar_t*& stage) {
    stage=L"Initialize WindowsXamlManager";
    if (!managerRuntime) managerRuntime=hosting::WindowsXamlManager::InitializeForCurrentThread();
    stage=L"Create desktop XAML source";
    source=hosting::DesktopWindowXamlSource();
    auto native=source.as<IDesktopWindowXamlSourceNative>();
    stage=L"Attach desktop XAML source";
    winrt::check_hresult(native->AttachToWindow(hwnd));
    winrt::check_hresult(native->get_WindowHandle(&child));
    input=source.try_as<IDesktopWindowXamlSourceNative2>();
}

static void BuildManager(HWND hwnd) {
    managerStartupStage=L"Activate manager manifest";
    ManagerManifestScope manifest;
    AttachIsland(hwnd,managerSource,islandWindow,managerInput,managerStartupStage);
    managerStartupStage = L"Build manager controls";
    managerRoot=xaml::Markup::XamlReader::Load(LR"XAML(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
  <Grid.Resources>
    <Storyboard x:Key="PageSlide">
      <DoubleAnimation Storyboard.TargetProperty="X" From="14" To="0" Duration="0:0:0.16" FillBehavior="Stop">
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
    <ControlTemplate x:Key="MenuEntryFace" TargetType="MenuFlyoutItem">
      <Grid Background="Transparent" MinHeight="32">
        <VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates">
          <VisualState x:Name="Normal"/>
          <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="EntrySurface.Opacity" Value="0.09"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="EntrySurface.Opacity" Value="0.14"/></VisualState.Setters></VisualState>
          <VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="EntryLabel.Opacity" Value="0.4"/></VisualState.Setters></VisualState>
        </VisualStateGroup></VisualStateManager.VisualStateGroups>
        <Border x:Name="EntrySurface" CornerRadius="5" Margin="3,1" Background="{TemplateBinding Foreground}" Opacity="0"/>
        <TextBlock x:Name="EntryLabel" Text="{TemplateBinding Text}" Foreground="{TemplateBinding Foreground}" Margin="12,4" VerticalAlignment="Center"/>
      </Grid>
    </ControlTemplate>
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
          <VisualStateGroup x:Name="ChoiceStates"><VisualState x:Name="AnimaOff"/><VisualState x:Name="AnimaOn"><VisualState.Setters>
            <Setter Target="OnTrack.Opacity" Value="1"/><Setter Target="Knob.HorizontalAlignment" Value="Right"/>
            <Setter Target="Knob.Background" Value="White"/>
          </VisualState.Setters></VisualState></VisualStateGroup>
          <VisualStateGroup x:Name="CommonStates"><VisualState x:Name="Normal"/><VisualState x:Name="PointerOver"><VisualState.Setters>
            <Setter Target="Knob.Width" Value="14"/><Setter Target="Knob.Height" Value="14"/>
          </VisualState.Setters></VisualState><VisualState x:Name="Pressed"><VisualState.Setters>
            <Setter Target="Knob.Width" Value="16"/>
          </VisualState.Setters></VisualState></VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
        <Border Height="20" Width="42" CornerRadius="10" BorderThickness="1" BorderBrush="{TemplateBinding Foreground}" Opacity="0.65"/>
        <Border x:Name="OnTrack" Height="20" Width="42" CornerRadius="10" Background="{TemplateBinding Background}" Opacity="0"/>
        <Border x:Name="Knob" Width="12" Height="12" CornerRadius="6" Margin="5,0" HorizontalAlignment="Left" VerticalAlignment="Center" Background="{TemplateBinding Foreground}"/>
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
  </Grid.Resources>
  <Grid.ColumnDefinitions><ColumnDefinition Width="234"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
  <Grid Margin="12">
    <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
      <Grid Grid.Row="1" RowSpacing="3"><Grid.RowDefinitions><RowDefinition Height="36"/><RowDefinition Height="36"/></Grid.RowDefinitions>
        <Button x:Name="Workshop" Grid.Row="0" AutomationProperties.Name="Workshop" HorizontalAlignment="Stretch" HorizontalContentAlignment="Left" Padding="14,0" BorderThickness="0">
          <StackPanel Orientation="Horizontal" Spacing="10"><FontIcon Glyph="&#xE719;" FontSize="16"/><TextBlock Text="Workshop"/></StackPanel>
        </Button>
        <Button x:Name="App" Grid.Row="1" AutomationProperties.Name="App settings" HorizontalAlignment="Stretch" HorizontalContentAlignment="Left" Padding="14,0" BorderThickness="0">
          <StackPanel Orientation="Horizontal" Spacing="10"><FontIcon Glyph="&#xE713;" FontSize="16"/><TextBlock Text="App settings"/></StackPanel>
        </Button>

      </Grid>
    <Grid Grid.Row="0" Margin="0,0,0,6">
      <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/><RowDefinition Height="*"/></Grid.RowDefinitions>
      <TextBlock Text="Media library" FontWeight="SemiBold" Margin="8,0,0,6"/>
      <TextBox x:Name="Search" Grid.Row="1" PlaceholderText="Search media"/>
      <ListView x:Name="Pets" Grid.Row="2" Margin="0,6,0,0" SelectionMode="Single"/>
    </Grid>
    <StackPanel Grid.Row="2" Margin="0,8,0,0" Spacing="6">
      <Grid ColumnSpacing="2">
        <Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="34"/></Grid.ColumnDefinitions>
        <Button x:Name="Import" Content="Import media…" HorizontalAlignment="Stretch"/>
        <Button x:Name="ImportOptions" Grid.Column="1" AutomationProperties.Name="Import options" HorizontalAlignment="Stretch" Padding="0">
          <FontIcon Glyph="&#xE70D;" FontSize="11"/>
          <Button.Flyout><Flyout Placement="TopEdgeAlignedRight">
            <StackPanel MinWidth="144" Spacing="2">
              <StackPanel.Transitions><TransitionCollection><EntranceThemeTransition FromVerticalOffset="12" FromHorizontalOffset="0"/></TransitionCollection></StackPanel.Transitions>
              <Button Style="{StaticResource ManagerButtonStyle}" Tag="images" Content="from images" HorizontalAlignment="Stretch"/>
              <Button Style="{StaticResource ManagerButtonStyle}" Tag="folder" Content="from folder" HorizontalAlignment="Stretch"/>
            </StackPanel>
          </Flyout></Button.Flyout>
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
  <Grid Grid.Column="1" Margin="24,20,16,0">
    <Grid.RenderTransform><TranslateTransform x:Name="PageOffset"/></Grid.RenderTransform>
    <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="*"/></Grid.RowDefinitions>
    <TextBlock x:Name="Title" FontSize="28" FontWeight="SemiBold" Margin="0,0,0,20" TextTrimming="CharacterEllipsis"/>
    <ScrollViewer x:Name="Scroll" Grid.Row="1" HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto">
      <StackPanel x:Name="Body" Spacing="12" Padding="0,0,12,16"/>
    </ScrollViewer>
  </Grid>
</Grid>)XAML").as<controls::Grid>();
    managerRoot.LayoutUpdated([hwnd](auto&&,auto&&){ if (managerRoot) PositionNavPill(hwnd); });
    managerSource.Content(managerRoot);
    RECT client{};
    GetClientRect(hwnd,&client);
    SetWindowPos(islandWindow,nullptr,0,0,client.right,client.bottom,SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
    appliedTheme=-1;
    StyleManager(hwnd);
    LoadBuildInfo();
    Named<Button>(L"Workshop").Click([](auto&&,auto&&){ Navigate(Page::Workshop); });
    Named<Button>(L"App").Click([](auto&&,auto&&){ Navigate(Page::App); });
    auto importMenu=Named<Button>(L"ImportOptions").Flyout().as<controls::Flyout>();
    for (auto element : importMenu.Content().as<StackPanel>().Children()) {
        auto choice=element.as<Button>();
        bool folder=winrt::unbox_value<winrt::hstring>(choice.Tag())==L"folder";
        choice.Click([folder,weak=winrt::make_weak(importMenu)](auto&&,auto&&) {
            if (auto menu=weak.get()) menu.Hide();
            PostMessageW(g_hManagerWnd,WM_APP+33,folder,0);
        });
    }
    Named<Button>(L"Import").Click([](auto&&,auto&&){ AddImages(); });
    Named<controls::TextBox>(L"Search").TextChanged([](auto&&,auto&&){ PetList(); });
    Named<controls::ListView>(L"Pets").SelectionChanged([](auto const& sender,controls::SelectionChangedEventArgs const&) {
        if (updatingList) return;
        auto item = sender.template as<controls::ListView>().SelectedItem();
        if (!item || !managerRoot) return;
        int index=winrt::unbox_value<int>(item.template as<controls::ListViewItem>().Tag());
        if (index<0) {
            auto& folder=libraryFolders.at(-index-1);
            folder.expanded=!folder.expanded;
            DWORD value=folder.expanded;
            if (configWritable) ConfigWriteResult(L"Expanded",RegSetKeyValueW(imagesRoot,folder.key.c_str(),L"Expanded",REG_DWORD,&value,sizeof(value)));
            PostMessageW(g_hManagerWnd,WM_APP+34,0,0);
            return;
        }
        bool changed=g_managerSel!=index;
        g_managerSel=index;
        Navigate(Page::Pet,changed);
    });
    page=Page::Pet;
    g_managerSel=g_items.empty() ? -1 : 0;
    PetList();
    RenderPage();
}
static void CloseManagerIsland() {
    navMotion=NavigationMotion{};
    managerInput=nullptr;
    islandWindow=nullptr;
    if (managerSource) {
        managerSource.Content(nullptr);
        managerSource.Close();
    }
    managerRoot=nullptr;
    managerSource=nullptr;
}
static LRESULT CALLBACK ManagerWndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        switch (msg) {
        case WM_TIMER: if (wp==kNavTimer) { StepNavPill(hwnd); return 0; } break;
        case WM_CREATE: BuildManager(hwnd); return 0;
        case WM_APP+34: if (managerRoot) PetList(); return 0;
        case WM_APP+33: if (wp) ImportFolder(); else AddImages(); return 0;
        case WM_SIZE:
            if (islandWindow) SetWindowPos(islandWindow,nullptr,0,0,LOWORD(lp),HIWORD(lp),SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
            return 0;
        case WM_DPICHANGED: {
            auto r = reinterpret_cast<RECT*>(lp);
            SetWindowPos(hwnd,nullptr,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);
            return 0;
        }
        case WM_GETMINMAXINFO: {
            UINT dpi=GetDpiForWindow(hwnd);
            reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize={MulDiv(660,dpi,96),MulDiv(490,dpi,96)};
            return 0;
        }
        case WM_SETFOCUS:
            if (managerSource) managerSource.NavigateFocus(hosting::XamlSourceFocusNavigationRequest(hosting::XamlSourceFocusNavigationReason::First));
            return 0;
        case WM_COMMAND:
            if (LOWORD(wp)==IDC_NAV_WORKSHOP && managerRoot) Navigate(Page::Workshop);
            return 0;
        case WM_APP_OPEN_MANAGER:
            if ((size_t)wp<g_items.size()) {
                bool changed=page!=Page::Pet || g_managerSel!=(int)wp;
                g_managerSel=(int)wp;
                page=Page::Pet;
                Named<controls::TextBox>(L"Search").Text(L"");
                PetList();
                Navigate(page,changed);
            }
            return 0;
        case WM_APP_RELOAD:
            PetList();
            RenderPage();
            return 0;
        case WM_THEMECHANGED: case WM_SETTINGCHANGE:
            if (managerRoot) PostMessageW(hwnd,kApplyManagerAppearance,0,0);
            return 0;
        case kApplyManagerAppearance:
            if (managerRoot) {
                if (StyleManager(hwnd) || wp) RenderPage();
            }
            return 0;
        case WM_APP+20:
            if (page==Page::App && managerRoot) RenderPage();
            return 0;
        case WM_DESTROY:
            StopKeyCapture();
            if (g_hotkeyCapturing) { g_hotkeyCapturing=false; ReapplyHotkey(); }
            CloseManagerIsland();
            g_hManagerWnd=nullptr;
            if (g_hManagerIcon) { DestroyIcon(g_hManagerIcon); g_hManagerIcon=nullptr; }
            return 0;
        }
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Manager message %u failed (0x%08X): %s",msg,(unsigned)error.code().value,error.message().c_str());
        if (msg==WM_CREATE) {
            wchar_t code[16]{};
            swprintf_s(code, L"0x%08X", static_cast<unsigned>(error.code().value));
            managerStartupError = std::wstring(managerStartupStage) + L" (" + code + L")\n\n" + error.message().c_str();
            Wh_Log(L"[Manager] %s", managerStartupError.c_str());
            CloseManagerIsland();
            return -1;
        }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}



static HICON LoadManagerIcon() {
    std::wstring directory=AppDataDirectory();
    if (directory.empty()) return nullptr;
    CreateDirectoryW(directory.c_str(),nullptr);
    std::wstring iconPath=directory+L"\\icon.png";

    if (GetFileAttributesW(iconPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        const wchar_t* kUrl =
            L"https://raw.githubusercontent.com/TuBeo5866/tubeo5866.github.io"
            L"/refs/heads/main/files/icon.png";

        HRESULT hr = URLDownloadToFileW(nullptr, kUrl, iconPath.c_str(), 0, nullptr);
        if (FAILED(hr)) {
            Wh_Log(L"Icon download failed (hr=0x%08X) — window will use default icon", hr);
            return nullptr;
        }
    }

    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(iconPath.c_str());
    if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok) {
        delete bmp;
        Wh_Log(L"Icon load failed — window will use default icon");
        return nullptr;
    }

    HICON hIcon = nullptr;
    bmp->GetHICON(&hIcon);
    delete bmp;
    return hIcon;
}


static void OpenPetManager(int focusIndex) {
    if (g_hManagerWnd) {
        ShowWindow(g_hManagerWnd, SW_RESTORE);
        SetForegroundWindow(g_hManagerWnd);
        if (focusIndex >= 0)
            PostMessageW(g_hManagerWnd, WM_APP_OPEN_MANAGER, (WPARAM)focusIndex, 0);
        return;
    }



    managerStartupError.clear();
    auto previousDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    g_hManagerWnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        kManagerWndClass,
        L"AnimaWindhawkEngine",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        MulDiv(920,GetDpiForSystem(),96), MulDiv(700,GetDpiForSystem(),96),
        nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);

    if (previousDpi) SetThreadDpiAwarenessContext(previousDpi);
    if (!g_hManagerWnd) {
        MessageBoxW(g_hCtrlWnd,managerStartupError.empty() ? L"Windows could not create the manager window." : managerStartupError.c_str(),L"AnimaWindhawkEngine",MB_ICONERROR);
        return;
    }

    if (!g_hManagerIcon)
        g_hManagerIcon = LoadManagerIcon();
    if (g_hManagerIcon) {
        SendMessageW(g_hManagerWnd, WM_SETICON, ICON_BIG,   (LPARAM)g_hManagerIcon);
        SendMessageW(g_hManagerWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hManagerIcon);
    }

    ShowWindow(g_hManagerWnd, SW_SHOW);
    UpdateWindow(g_hManagerWnd);
    SetForegroundWindow(g_hManagerWnd);

    if (focusIndex >= 0)
        PostMessageW(g_hManagerWnd, WM_APP_OPEN_MANAGER, (WPARAM)focusIndex, 0);
}


static LRESULT CALLBACK ItemWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PetItem* it = (PetItem*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_NCCREATE: {
        auto* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
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
    case WM_MOUSEMOVE:
        if (it && it->dragging) {
            POINT cur{};
            GetCursorPos(&cur);
            int dx = cur.x - it->dragStartCursor.x;
            int dy = cur.y - it->dragStartCursor.y;
            SetWindowPos(hwnd, nullptr,
                         it->dragOriginWnd.x + dx, it->dragOriginWnd.y + dy,
                         0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        return 0;
    case WM_CAPTURECHANGED:
    case WM_LBUTTONUP:
        if (it && it->dragging) {
            it->dragging = false;
            if (msg==WM_LBUTTONUP && GetCapture()==hwnd) ReleaseCapture();
            RECT r{};
            if (!GetWindowRect(hwnd,&r)) return 0;
            it->posX = r.left;
            it->posY = r.top;
            if (it->isGif && it->monitorHz!=MonitorRefreshRate(hwnd)) {
                it->monitorHz=MonitorRefreshRate(hwnd);
                ApplyTimer();
                if (managerRoot && page==Page::Pet && SelectedPet()==it) RenderPage();
            }
            SaveStoredPosition(it->registryKey, it->posX, it->posY);
        }
        return 0;
    case WM_RBUTTONUP: {
        if (!it) return 0;
        int idx = -1;
        for (int i = 0; i < (int)g_items.size(); i++)
            if (&g_items[i] == it) { idx = i; break; }

        HMENU m = CreatePopupMenu();
        AppendMenuW(m, MF_STRING, kCmdOpenManager,       L"Open in Media Manager");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING | (it->alwaysOnTop  ? MF_CHECKED : 0), kCmdToggleAlwaysOnTop,  L"Always on top");
        AppendMenuW(m, MF_STRING | (it->clickThrough ? MF_CHECKED : 0), kCmdToggleClickThrough, L"Click-through");
        AppendMenuW(m, MF_STRING | (it->lockPosition ? MF_CHECKED : 0), kCmdToggleLock,         L"Lock position");
        AppendMenuW(m, MF_STRING | (it->paused       ? MF_CHECKED : 0), kCmdTogglePause,        L"Pause animation");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, kCmdBringToFront, L"Bring to front");
        AppendMenuW(m, MF_STRING, kCmdHideThis,     L"Hide this item");
        AppendMenuW(m, MF_STRING, kCmdResetPosition,L"Reset position");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, kCmdReloadAll,    L"Reload all images");

        POINT pt{};
        GetCursorPos(&pt);
        SetForegroundWindow(hwnd);
        int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
        DestroyMenu(m);

        switch (cmd) {
        case kCmdOpenManager:
            OpenPetManager(idx);
            break;
        case kCmdToggleAlwaysOnTop:
            it->alwaysOnTop = !it->alwaysOnTop;
            ApplyTopmost(*it);
            SyncImageState(*it,false);
            if (managerRoot && page==Page::Pet && idx==g_managerSel) RenderPage();
            break;
        case kCmdToggleClickThrough:
            it->clickThrough = !it->clickThrough;
            ApplyClickThrough(*it);
            SyncImageState(*it,false);
            if (managerRoot && page==Page::Pet && idx==g_managerSel) RenderPage();
            break;
        case kCmdToggleLock:
            it->lockPosition = !it->lockPosition;
            SyncImageState(*it,false);
            if (managerRoot && page==Page::Pet && idx==g_managerSel) RenderPage();
            break;
        case kCmdTogglePause:
            it->paused = !it->paused;
            SyncImageState(*it,false);
            if (managerRoot && page==Page::Pet && idx==g_managerSel) RenderPage();
            break;
        case kCmdBringToFront:
            SetWindowPos(hwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
            break;
        case kCmdHideThis:
            it->hidden = true;
            ShowWindow(hwnd, SW_HIDE);
            break;
        case kCmdResetPosition:
            it->posX = 80; it->posY = 80;
            SetWindowPos(hwnd, nullptr, it->posX, it->posY, 0, 0,
                         SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
            ForgetStoredPosition(it->registryKey);
            break;
        case kCmdReloadAll:
            PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
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
    if (it.alwaysOnTop)  exStyle |= WS_EX_TOPMOST;
    if (it.clickThrough) exStyle |= WS_EX_TRANSPARENT;
    HWND hwnd = CreateWindowExW(exStyle, kItemWndClass, it.fileName.c_str(),
                                WS_POPUP, it.posX, it.posY,
                                it.baseWidth, it.baseHeight,
                                nullptr, nullptr,
                                GetModuleHandleW(nullptr), &it);
    if (hwnd && it.enabled) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    return hwnd;
}


static void SaveGlobalSettings(const GlobalSettings& s) {
    if (!configWritable) return;
    SaveNumber(L"TrayIcon",s.trayIcon);
    SaveNumber(L"UseFolderMode",s.useFolderMode);
    SaveNumber(L"ScanSubfolders",s.scanSubfolders);
    SaveNumber(L"AlwaysOnTop",s.alwaysOnTop);
    SaveNumber(L"ClickThrough",s.clickThrough);
    SaveNumber(L"LockPosition",s.lockPosition);
    SaveNumber(L"ScalePercent",s.scalePercent);
    SaveNumber(L"OpacityPercent",s.opacityPercent);
    SaveNumber(L"RotationDegrees",s.rotationDegrees);
    SaveNumber(L"Paused",s.paused);
    SaveNumber(L"MaxFps",s.maxFps);
    WriteConfig(L"ImagesFolder",REG_SZ,s.imagesFolder.c_str(),(DWORD)((s.imagesFolder.size()+1)*sizeof(wchar_t)));
}
static void ReadGlobalSettings(GlobalSettings& s,bool importWindhawk) {
    s.useFolderMode    = Wh_GetIntSetting(L"UseFolderMode") != 0;

    PCWSTR folder = Wh_GetStringSetting(L"ImagesFolder");
    s.imagesFolder = folder ? folder : L"";
    Wh_FreeStringSetting(folder);

    s.scanSubfolders   = Wh_GetIntSetting(L"ScanSubfolders") != 0;

    s.imagePaths.clear();
    for (int i = 0;; i++) {
        PCWSTR p = Wh_GetStringSetting(L"ImagePaths[%d]", i);
        bool has = p && *p;
        if (!has) { Wh_FreeStringSetting(p); break; }
        s.imagePaths.emplace_back(p);
        Wh_FreeStringSetting(p);
    }
    auto windhawkPaths=s.imagePaths;
    LoadImagePaths(s.imagePaths);
    bool hasOrder=configRoot && !MissingValue(configRoot,L"ImageOrder");
    if (!windhawkPaths.empty() && (importWindhawk || !hasOrder)) s.imagePaths=std::move(windhawkPaths);
    if (!hasOrder || importWindhawk) SaveImagePaths(s.imagePaths);
    s.alwaysOnTop      = Wh_GetIntSetting(L"Global_AlwaysOnTop") != 0;
    s.clickThrough     = Wh_GetIntSetting(L"Global_ClickThrough") != 0;
    s.lockPosition     = Wh_GetIntSetting(L"Global_LockPosition") != 0;
    s.scalePercent     = Wh_GetIntSetting(L"Global_ScalePercent");
    s.rotationDegrees  = Wh_GetIntSetting(L"Global_RotationDegrees");
    s.opacityPercent   = Wh_GetIntSetting(L"Global_OpacityPercent");
    s.paused           = Wh_GetIntSetting(L"Global_Paused") != 0;
    s.maxFps           = Wh_GetIntSetting(L"MaxFps");
    s.trayIcon         = Wh_GetIntSetting(L"TrayIcon") != 0;

    if (s.opacityPercent <= 0) s.opacityPercent = 100;
    if (s.scalePercent <= 0) s.scalePercent = 100;
    if (s.maxFps       <= 0) s.maxFps       = 20;

    if (!importWindhawk && configRoot) {
        s.trayIcon=ConfigNumber(L"TrayIcon",s.trayIcon)!=0;
        s.useFolderMode=ConfigNumber(L"UseFolderMode",s.useFolderMode)!=0;
        s.scanSubfolders=ConfigNumber(L"ScanSubfolders",s.scanSubfolders)!=0;
        s.alwaysOnTop=ConfigNumber(L"AlwaysOnTop",s.alwaysOnTop)!=0;
        s.clickThrough=ConfigNumber(L"ClickThrough",s.clickThrough)!=0;
        s.lockPosition=ConfigNumber(L"LockPosition",s.lockPosition)!=0;
        s.scalePercent=ConfigNumber(L"ScalePercent",s.scalePercent);
        s.opacityPercent=ConfigNumber(L"OpacityPercent",s.opacityPercent);
        s.rotationDegrees=ConfigNumber(L"RotationDegrees",s.rotationDegrees);
        s.paused=ConfigNumber(L"Paused",s.paused)!=0;
        s.maxFps=ConfigNumber(L"MaxFps",s.maxFps);
        if (!MissingValue(configRoot,L"ImagesFolder")) s.imagesFolder=StringAt(configRoot,nullptr,L"ImagesFolder");
    }
    s.maxFps=std::max(1,s.maxFps);
    SaveGlobalSettings(s);
    s.overrides.clear();
    for (int i = 0;; i++) {
        PCWSTR name = Wh_GetStringSetting(L"ImageOverrides[%d].FileName", i);
        bool hasName = name && *name;
        if (!hasName) { Wh_FreeStringSetting(name); break; }

        ImageOverride ov;
        ov.fileName = name;
        Wh_FreeStringSetting(name);

        ov.enabled = Wh_GetIntSetting(L"ImageOverrides[%d].Enabled", i) != 0;

        PCWSTR aot = Wh_GetStringSetting(L"ImageOverrides[%d].AlwaysOnTop", i);
        ov.alwaysOnTop = ParseTriState(aot); Wh_FreeStringSetting(aot);

        PCWSTR ct = Wh_GetStringSetting(L"ImageOverrides[%d].ClickThrough", i);
        ov.clickThrough = ParseTriState(ct); Wh_FreeStringSetting(ct);

        PCWSTR lp = Wh_GetStringSetting(L"ImageOverrides[%d].LockPosition", i);
        ov.lockPosition = ParseTriState(lp); Wh_FreeStringSetting(lp);

        ov.scalePercent    = Wh_GetIntSetting(L"ImageOverrides[%d].ScalePercent",    i);
        ov.rotationDegrees = Wh_GetIntSetting(L"ImageOverrides[%d].RotationDegrees", i);
        ov.opacityPercent  = Wh_GetIntSetting(L"ImageOverrides[%d].OpacityPercent",  i);
        ov.posX            = Wh_GetIntSetting(L"ImageOverrides[%d].PosX", i);
        ov.posY            = Wh_GetIntSetting(L"ImageOverrides[%d].PosY", i);

        s.overrides.push_back(ov);
    }
}


static bool NeedsMediaDecoder(const std::wstring& path) {
    const wchar_t* ext = PathFindExtensionW(path.c_str());
    if (!_wcsicmp(ext, L".webp") || !_wcsicmp(ext, L".apng") ||
        !_wcsicmp(ext, L".mp4")) return true;
    if (_wcsicmp(ext, L".png")) return false;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    BYTE h[8]; DWORD got = 0;
    const BYTE sig[8] = {137,80,78,71,13,10,26,10};
    bool animated = false;
    if (ReadFile(f, h, 8, &got, nullptr) && got == 8 && !memcmp(h, sig, 8)) {
        LARGE_INTEGER size = {}; GetFileSizeEx(f, &size);
        LONGLONG offset = 8;
        while (offset + 12 <= size.QuadPart && ReadFile(f, h, 8, &got, nullptr) && got == 8) {
            DWORD len = (DWORD(h[0]) << 24) | (DWORD(h[1]) << 16) |
                        (DWORD(h[2]) << 8) | h[3];
            if (offset + 12 + len > size.QuadPart) break;
            if (!memcmp(h + 4, "acTL", 4)) { animated = true; break; }
            if (!memcmp(h + 4, "IDAT", 4) || !memcmp(h + 4, "IEND", 4)) break;
            LARGE_INTEGER skip; skip.QuadPart = LONGLONG(len) + 4;
            if (!SetFilePointerEx(f, skip, nullptr, FILE_CURRENT)) break;
            offset += LONGLONG(len) + 12;
        }
    }
    CloseHandle(f);
    return animated;
}

static std::wstring MediaQuote(const std::wstring& arg) {
    std::wstring out = L"\"";
    size_t slashes = 0;
    for (wchar_t c : arg) {
        if (c == L'\\') { ++slashes; continue; }
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
        wchar_t name[32]; swprintf_s(name, L"\\frame-%04u.png", n);
        return dir + name;
    }
    ~MediaTempFrames() {
        if (dir.empty()) return;
        for (UINT n = 1; n <= 301; ++n) DeleteFileW(Frame(n).c_str());
        RemoveDirectoryW(dir.c_str());
    }
};

static HANDLE g_ffmpegSetupProcess = nullptr;
static HANDLE g_ffmpegSetupJob = nullptr;
static bool g_ffmpegSetupAttempted = false;
static bool g_ffmpegSetupStopping = false;
static ULONGLONG g_ffmpegSetupStarted = 0;
static std::wstring g_ffmpegSetupTemp;
static std::wstring g_ffmpegSetupStage;
static std::wstring g_ffmpegSetupError;

static bool g_ffmpegMediaBlocked = false;
static void StartFFmpegSetup(const std::wstring& exe);
static void ShowFFmpegSetupToast();

enum class FFmpegToastState { Progress, Success, Failure };
static FFmpegToastState g_ffmpegToastState = FFmpegToastState::Progress;
static std::wstring g_ffmpegToastTitle = L"FFmpeg setup";
static std::wstring g_ffmpegToastBody;
static std::wstring g_ffmpegLastFailure;
static DWORD g_ffmpegProgressOffset = 0;
static std::string g_ffmpegProgressPending;
static ULONGLONG g_ffmpegProgressPoll = 0;

static void SetFFmpegSetupStatus(FFmpegToastState state, const std::wstring& title,
                                 const std::wstring& body) {
    g_ffmpegToastState = state;
    g_ffmpegToastTitle = title;
    g_ffmpegToastBody = body;
    Wh_Log(L"[FFmpeg] %s", body.c_str());
    ShowFFmpegSetupToast();
}

static void ReadFFmpegSetupProgress(bool force = false) {
    if (g_ffmpegSetupTemp.empty()) return;
    ULONGLONG now = GetTickCount64();
    if (!force && now - g_ffmpegProgressPoll < 250) return;
    g_ffmpegProgressPoll = now;
    HANDLE f = CreateFileW((g_ffmpegSetupTemp + L"\\progress.log").c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER offset; offset.QuadPart = g_ffmpegProgressOffset;
    if (SetFilePointerEx(f, offset, nullptr, FILE_BEGIN)) {
        char bytes[4096]; DWORD got = 0;
        while (g_ffmpegProgressOffset < 65536 && ReadFile(f, bytes, sizeof(bytes), &got, nullptr) && got) {
            g_ffmpegProgressOffset += got;
            g_ffmpegProgressPending.append(bytes, got);
            size_t end;
            while ((end = g_ffmpegProgressPending.find('\n')) != std::string::npos) {
                std::string line = g_ffmpegProgressPending.substr(0, end);
                g_ffmpegProgressPending.erase(0, end + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (line.empty()) continue;
                int count = MultiByteToWideChar(CP_UTF8, 0, line.data(), (int)line.size(), nullptr, 0);
                if (count <= 0) continue;
                std::wstring status(count, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, line.data(), (int)line.size(), &status[0], count);
                SetFFmpegSetupStatus(FFmpegToastState::Progress, L"Setting up FFmpeg", status);
            }
        }
    }
    CloseHandle(f);
}

static HANDLE g_nativeToastProcess = nullptr, g_nativeToastJob = nullptr;
static HANDLE g_nativeToastRetry = nullptr, g_nativeToastOpen = nullptr, g_nativeToastStop = nullptr;
static bool g_nativeToastAttempted = false;
static std::wstring g_nativeToastDir, g_nativeToastShortcut, g_nativeToastLastPayload;
static const wchar_t kToastAppId[] = L"AnimaWindhawkEngine.FFmpeg";
static std::wstring PSString(const std::wstring& value);

static std::wstring ToastEncoded(const std::wstring& script) {
    DWORD count = 0;
    if (!CryptBinaryToStringW((const BYTE*)script.data(), (DWORD)(script.size()*sizeof(wchar_t)),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &count)) return {};
    std::wstring result(count, L'\0');
    if (!CryptBinaryToStringW((const BYTE*)script.data(), (DWORD)(script.size()*sizeof(wchar_t)),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &result[0], &count)) return {};
    while (!result.empty() && !result.back()) result.pop_back();
    return result;
}

static bool ToastRegString(const std::wstring& key, const wchar_t* name, const std::wstring& value) {
    HKEY h = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, nullptr, 0, KEY_SET_VALUE, nullptr, &h, nullptr)) return false;
    LONG result = RegSetValueExW(h, name, 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)((value.size()+1)*sizeof(wchar_t)));
    RegCloseKey(h); return result == ERROR_SUCCESS;
}

static bool RegisterNativeToast(const std::wstring& powershell, const std::wstring& retryName,
                                const std::wstring& openName) {
    auto signalCommand = [&](const std::wstring& event) {
        std::wstring script = L"try{$e=[Threading.EventWaitHandle]::OpenExisting(" + PSString(event) +
            L");$e.Set()|Out-Null;$e.Dispose()}catch{}";
        return MediaQuote(powershell) + L" -NoProfile -NonInteractive -WindowStyle Hidden -EncodedCommand " + ToastEncoded(script);
    };
    std::wstring retryCmd = signalCommand(retryName), openCmd = signalCommand(openName);
    for (auto pair : {std::make_pair(L"anima-windhawk-ffmpeg-retry", retryCmd),
                      std::make_pair(L"anima-windhawk-manager", openCmd)}) {
        std::wstring key = L"Software\\Classes\\" + std::wstring(pair.first);
        if (!ToastRegString(key, nullptr, L"URL:AnimaWindhawkEngine") ||
            !ToastRegString(key, L"URL Protocol", L"") ||
            !ToastRegString(key + L"\\shell\\open\\command", nullptr, pair.second)) return false;
    }
    std::wstring appKey = L"Software\\Classes\\AppUserModelId\\" + std::wstring(kToastAppId);
    if (!ToastRegString(appKey, L"DisplayName", L"AnimaWindhawkEngine") ||
        !ToastRegString(appKey, L"CustomActivator", L"{EFDCFD6B-B481-4190-8B67-30B470795A18}")) return false;
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool uninit = SUCCEEDED(init);
    IShellLinkW* link = nullptr;
    const CLSID shellLink = {0x00021401,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
    HRESULT hr = CoCreateInstance(shellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link));
    if (SUCCEEDED(hr)) {
        hr = link->SetPath(powershell.c_str());
        std::wstring args = openCmd.substr(MediaQuote(powershell).size()+1);
        if (SUCCEEDED(hr)) hr = link->SetArguments(args.c_str());
        if (SUCCEEDED(hr)) hr = link->SetDescription(L"AnimaWindhawkEngine notifications and manager");
        IPropertyStore* props = nullptr;
        if (SUCCEEDED(hr)) hr = link->QueryInterface(IID_PPV_ARGS(&props));
        if (SUCCEEDED(hr)) {
            const PROPERTYKEY appIdKey = {{0x9f4c2855,0x9f79,0x4b39,{0xa8,0xd0,0xe1,0xd4,0x2d,0xe1,0xd5,0xf3}},5};
            const PROPERTYKEY activatorKey = {appIdKey.fmtid,26};
            GUID stub = {0xefdcfd6b,0xb481,0x4190,{0x8b,0x67,0x30,0xb4,0x70,0x79,0x5a,0x18}};
            PROPVARIANT value = {}; value.vt = VT_LPWSTR; value.pwszVal = const_cast<PWSTR>(kToastAppId);
            hr = props->SetValue(appIdKey, value);
            value.vt = VT_CLSID; value.puuid = &stub;
            if (SUCCEEDED(hr)) hr = props->SetValue(activatorKey, value);
            if (SUCCEEDED(hr)) hr = props->Commit();
            props->Release();
        }
        IPersistFile* file = nullptr;
        if (SUCCEEDED(hr)) hr = link->QueryInterface(IID_PPV_ARGS(&file));
        if (SUCCEEDED(hr)) { hr = file->Save(g_nativeToastShortcut.c_str(), TRUE); file->Release(); }
        link->Release();
    }
    if (uninit) CoUninitialize();
    return SUCCEEDED(hr);
}

static void StopNativeToasts() {
    if (g_nativeToastStop) SetEvent(g_nativeToastStop);
    if (g_nativeToastProcess && WaitForSingleObject(g_nativeToastProcess, 5000) != WAIT_OBJECT_0) {
        if (g_nativeToastJob) TerminateJobObject(g_nativeToastJob, 1);
        WaitForSingleObject(g_nativeToastProcess, INFINITE);
    }
    for (HANDLE h : {g_nativeToastProcess,g_nativeToastJob,g_nativeToastRetry,g_nativeToastOpen,g_nativeToastStop})
        if (h) CloseHandle(h);
    g_nativeToastProcess=g_nativeToastJob=g_nativeToastRetry=g_nativeToastOpen=g_nativeToastStop=nullptr;
    if (!g_nativeToastDir.empty()) {
        for (auto name : {L"notice.xml",L"notice.tmp",L"error.txt"}) DeleteFileW((g_nativeToastDir+L"\\"+name).c_str());
        RemoveDirectoryW(g_nativeToastDir.c_str()); g_nativeToastDir.clear();
    }
    if (!g_nativeToastShortcut.empty()) { DeleteFileW(g_nativeToastShortcut.c_str()); g_nativeToastShortcut.clear(); }
    if (g_nativeToastAttempted) {
        RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\anima-windhawk-ffmpeg-retry");
        RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\anima-windhawk-manager");
        RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\AppUserModelId\\AnimaWindhawkEngine.FFmpeg");
    }
}

static bool StartNativeToasts() {
    if (g_nativeToastProcess) return true;
    if (g_nativeToastAttempted || g_ffmpegSetupStopping) return false;
    g_nativeToastAttempted = true;
    auto fail = []() { Wh_Log(L"[FFmpeg] Native notification initialization failed (Win32 %lu). Setup logs remain available.",GetLastError()); StopNativeToasts(); return false; };
    wchar_t temp[MAX_PATH], unique[MAX_PATH], system[MAX_PATH], programs[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH,temp);
    if (!n || n>=MAX_PATH || !GetTempFileNameW(temp,L"AWN",0,unique)) return fail();
    DeleteFileW(unique);
    if (!CreateDirectoryW(unique,nullptr)) return fail();
    g_nativeToastDir=unique;
    n=GetSystemDirectoryW(system,MAX_PATH);
    if (!n || n>=MAX_PATH || FAILED(SHGetFolderPathW(nullptr,CSIDL_PROGRAMS,nullptr,0,programs))) return fail();
    g_nativeToastShortcut=std::wstring(programs)+L"\\AnimaWindhawkEngine Notifications.lnk";
    std::wstring powershell=std::wstring(system)+L"\\WindowsPowerShell\\v1.0\\powershell.exe";
    std::wstring prefix=L"Local\\AWHE.Toast."+std::to_wstring(GetCurrentProcessId());
    std::wstring retryName=prefix+L".Retry",openName=prefix+L".Open",stopName=prefix+L".Stop";
    HANDLE token=nullptr; DWORD tokenBytes=0;
    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token))return fail();
    GetTokenInformation(token,TokenUser,nullptr,0,&tokenBytes);
    std::vector<BYTE> tokenData(tokenBytes);
    bool tokenOk=tokenBytes && GetTokenInformation(token,TokenUser,tokenData.data(),tokenBytes,&tokenBytes);
    CloseHandle(token);
    LPWSTR sid=nullptr;
    if(!tokenOk || !ConvertSidToStringSidW(((TOKEN_USER*)tokenData.data())->User.Sid,&sid))return fail();
    std::wstring sddl=L"D:P(A;;GA;;;SY)(A;;GA;;;"+std::wstring(sid)+L")S:(ML;;NW;;;ME)";
    LocalFree(sid);
    PSECURITY_DESCRIPTOR descriptor=nullptr;
    if(!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(),SDDL_REVISION_1,&descriptor,nullptr))return fail();
    SECURITY_ATTRIBUTES security={sizeof(security),descriptor,FALSE};
    g_nativeToastRetry=CreateEventW(&security,FALSE,FALSE,retryName.c_str());
    g_nativeToastOpen=CreateEventW(&security,FALSE,FALSE,openName.c_str());
    g_nativeToastStop=CreateEventW(&security,TRUE,FALSE,stopName.c_str());
    LocalFree(descriptor);
    if (!g_nativeToastRetry || !g_nativeToastOpen || !g_nativeToastStop ||
        !RegisterNativeToast(powershell,retryName,openName)) return fail();
    std::wstring script=L"$work="+PSString(g_nativeToastDir)+L";$stopName="+PSString(stopName)+L";\n"+LR"NATIVE(
$ErrorActionPreference='Stop'
try {
    [Windows.UI.Notifications.ToastNotificationManager,Windows.UI.Notifications,ContentType=WindowsRuntime] | Out-Null
    [Windows.UI.Notifications.ToastNotification,Windows.UI.Notifications,ContentType=WindowsRuntime] | Out-Null
    [Windows.UI.Notifications.NotificationData,Windows.UI.Notifications,ContentType=WindowsRuntime] | Out-Null
    [Windows.Data.Xml.Dom.XmlDocument,Windows.Data.Xml.Dom.XmlDocument,ContentType=WindowsRuntime] | Out-Null
    $app='AnimaWindhawkEngine.FFmpeg'
    $notifier=[Windows.UI.Notifications.ToastNotificationManager]::CreateToastNotifier($app)
    if ($notifier.Setting.ToString() -ne 'Enabled') { throw ('Notifications are disabled: '+$notifier.Setting) }
    $stop=[Threading.EventWaitHandle]::OpenExisting($stopName)
    $last='';$lastKind='';$sequence=[uint32]0
    while (!$stop.WaitOne(250)) {
        $path=Join-Path $work 'notice.xml'
        if (!(Test-Path -LiteralPath $path)) { continue }
        try { $text=[IO.File]::ReadAllText($path);$notice=[xml]$text } catch { continue }
        if ($text -eq $last) { continue }
        $last=$text;$kind=[string]$notice.notice.kind
        $data=[Windows.UI.Notifications.NotificationData]::new()
        $data.Values.Add('title',[string]$notice.notice.title)
        $data.Values.Add('status',[string]$notice.notice.body)
        $sequence++;$data.SequenceNumber=$sequence
        if ($kind -eq '0' -and $lastKind -eq '0') {
            # A dismissed progress notification stays dismissed. No repeated popups.
            $notifier.Update($data,'setup','ffmpeg') | Out-Null
        } else {
            $actions='';$progress=''
            if ($kind -eq '2') {
                $actions='<actions><action content="Retry FFmpeg setup" activationType="protocol" arguments="anima-windhawk-ffmpeg-retry:"/></actions>'
            } elseif ($kind -eq '0') {
                $progress='<progress title="FFmpeg setup" value="indeterminate" status="Working..."/>'
            }
            $xml=[Windows.Data.Xml.Dom.XmlDocument]::new()
            $xml.LoadXml('<toast activationType="protocol" launch="anima-windhawk-manager:"><visual><binding template="ToastGeneric"><text>{title}</text><text>{status}</text>'+$progress+'</binding></visual>'+$actions+'<audio silent="true"/></toast>')
            $toast=[Windows.UI.Notifications.ToastNotification]::new($xml)
            $toast.Tag='setup';$toast.Group='ffmpeg';$toast.Data=$data
            $toast.ExpirationTime=[DateTimeOffset]::Now.AddHours(12)
            $notifier.Show($toast)
        }
        $lastKind=$kind
    }
} catch {
    try { [IO.File]::WriteAllText((Join-Path $work 'error.txt'),$_.Exception.Message) } catch {}
} finally {
    try { [Windows.UI.Notifications.ToastNotificationManager]::History.Remove('setup','ffmpeg','AnimaWindhawkEngine.FFmpeg') } catch {}
    if ($stop) { $stop.Dispose() }
}
)NATIVE";
    std::wstring encoded=ToastEncoded(script);
    if (encoded.empty()) return fail();
    std::wstring cmd=MediaQuote(powershell)+L" -NoLogo -NoProfile -NonInteractive -WindowStyle Hidden -EncodedCommand "+encoded;
    g_nativeToastJob=CreateJobObjectW(nullptr,nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={};
    limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!g_nativeToastJob || !SetInformationJobObject(g_nativeToastJob,JobObjectExtendedLimitInformation,&limits,sizeof(limits))) return fail();
    STARTUPINFOW si={};si.cb=sizeof(si);PROCESS_INFORMATION pi={};
    if (!CreateProcessW(powershell.c_str(),&cmd[0],nullptr,nullptr,FALSE,CREATE_NO_WINDOW|CREATE_SUSPENDED,nullptr,
        g_nativeToastDir.c_str(),&si,&pi)) return fail();
    if (!AssignProcessToJobObject(g_nativeToastJob,pi.hProcess) || ResumeThread(pi.hThread)==DWORD(-1)) {
        TerminateProcess(pi.hProcess,1);WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return fail();
    }
    CloseHandle(pi.hThread);g_nativeToastProcess=pi.hProcess;
    Wh_Log(L"[FFmpeg] Native Windows notifications initialized");return true;
}

static std::wstring ToastXmlEscape(const std::wstring& text) {
    std::wstring result;
    for(wchar_t c:text) {
        switch(c) {
        case L'&':result+=L"&amp;";break;
        case L'<':result+=L"&lt;";break;
        case L'>':result+=L"&gt;";break;
        case L'"':result+=L"&quot;";break;
        case L'\'':result+=L"&apos;";break;
        default:if(c>=32 || c==L'\n' || c==L'\r' || c==L'\t')result+=c;
        }
    }
    return result;
}

static void ShowFFmpegSetupToast() {
    if (g_ffmpegSetupStopping || !StartNativeToasts()) return;
    std::wstring xml=L"<notice kind=\""+std::to_wstring((int)g_ffmpegToastState)+L"\"><title>"+
        ToastXmlEscape(g_ffmpegToastTitle)+L"</title><body>"+ToastXmlEscape(g_ffmpegToastBody)+L"</body></notice>";
    if(xml==g_nativeToastLastPayload)return;
    int count=WideCharToMultiByte(CP_UTF8,0,xml.data(),(int)xml.size(),nullptr,0,nullptr,nullptr);
    if(count<=0)return;
    std::string bytes(count,0);WideCharToMultiByte(CP_UTF8,0,xml.data(),(int)xml.size(),&bytes[0],count,nullptr,nullptr);
    std::wstring temp=g_nativeToastDir+L"\\notice.tmp",dest=g_nativeToastDir+L"\\notice.xml";
    HANDLE f=CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(f==INVALID_HANDLE_VALUE)return;
    DWORD written=0;bool ok=WriteFile(f,bytes.data(),(DWORD)bytes.size(),&written,nullptr) && written==bytes.size();
    CloseHandle(f);
    if(ok && MoveFileExW(temp.c_str(),dest.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))g_nativeToastLastPayload=xml;
}

static void PollNativeToasts() {
    if(g_nativeToastRetry && WaitForSingleObject(g_nativeToastRetry,0)==WAIT_OBJECT_0 &&
        !g_ffmpegSetupProcess && g_ffmpegMediaBlocked && !g_ffmpegSetupStopping) {
        wchar_t local[MAX_PATH];
        if(SUCCEEDED(SHGetFolderPathW(nullptr,CSIDL_LOCAL_APPDATA,nullptr,0,local))) {
            Wh_Log(L"[FFmpeg] Retry requested from Windows notification");
            g_ffmpegSetupError.clear();g_ffmpegSetupAttempted=false;
            StartFFmpegSetup(std::wstring(local)+L"\\AnimaWindhawkEngine\\ffmpeg.exe");
        }
    }
    if(g_nativeToastOpen && WaitForSingleObject(g_nativeToastOpen,0)==WAIT_OBJECT_0)OpenPetManager();
    if(g_nativeToastProcess && WaitForSingleObject(g_nativeToastProcess,0)==WAIT_OBJECT_0) {
        HANDLE f=CreateFileW((g_nativeToastDir+L"\\error.txt").c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        std::wstring error=L"Notification host exited. Check Windows notification settings.";
        if(f!=INVALID_HANDLE_VALUE) {
            char buf[4096];DWORD got=0;
            if(ReadFile(f,buf,sizeof(buf),&got,nullptr)&&got) {
                int n=MultiByteToWideChar(CP_UTF8,0,buf,got,nullptr,0);
                if(n>0){error.resize(n);MultiByteToWideChar(CP_UTF8,0,buf,got,&error[0],n);}
            }
            CloseHandle(f);
        }
        Wh_Log(L"[FFmpeg] Native notifications: %s",error.c_str());StopNativeToasts();
    }
}

static void FailFFmpegSetup() {
    bool newlyBlocked = !g_ffmpegMediaBlocked;
    g_ffmpegMediaBlocked = true;
    if (g_ffmpegLastFailure != g_ffmpegSetupError || g_ffmpegToastState != FFmpegToastState::Failure) {
        g_ffmpegLastFailure = g_ffmpegSetupError;
        SetFFmpegSetupStatus(FFmpegToastState::Failure, L"FFmpeg setup failed",
            L"Could not download, extract or install FFmpeg. WebP, APNG and MP4 are unavailable. Use Retry below.");
        Wh_Log(L"[FFmpeg] Failure details: %s", g_ffmpegSetupError.c_str());
    } else ShowFFmpegSetupToast();
    if (newlyBlocked) PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
}

static std::wstring PSString(const std::wstring& value) {
    std::wstring result = L"'";
    for (wchar_t c : value) { result += c; if (c == L'\'') result += c; }
    return result + L"'";
}

static void CleanupFFmpegSetup() {
    if (g_ffmpegSetupProcess) {
        CloseHandle(g_ffmpegSetupProcess); g_ffmpegSetupProcess = nullptr;
    }
    if (g_ffmpegSetupJob) { CloseHandle(g_ffmpegSetupJob); g_ffmpegSetupJob = nullptr; }
    if (!g_ffmpegSetupStage.empty()) {
        DeleteFileW(g_ffmpegSetupStage.c_str()); g_ffmpegSetupStage.clear();
    }
    if (!g_ffmpegSetupTemp.empty()) {
        for (auto name : {L"7zr.exe", L"ffmpeg.7z", L"ffmpeg.exe", L"installed.flag", L"error.txt", L"progress.log"})
            DeleteFileW((g_ffmpegSetupTemp + L"\\" + name).c_str());
        RemoveDirectoryW(g_ffmpegSetupTemp.c_str());
        g_ffmpegSetupTemp.clear();
    }
}

static void StopFFmpegSetup() {
    g_ffmpegSetupStopping = true;
    StopNativeToasts();
    if (g_ffmpegSetupProcess) {
        Wh_Log(L"[FFmpeg] Setup cancelled: mod is shutting down");
        if (g_ffmpegSetupJob) TerminateJobObject(g_ffmpegSetupJob, 1);
        WaitForSingleObject(g_ffmpegSetupProcess, INFINITE);
    }
    CleanupFFmpegSetup();
}

static void StartFFmpegSetup(const std::wstring& exe) {
    if (g_ffmpegSetupAttempted || g_ffmpegSetupStopping) return;
    DWORD attributes=GetFileAttributesW(exe.c_str());
    if (attributes!=INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        if (g_ffmpegMediaBlocked) {
            g_ffmpegMediaBlocked=false;
            g_ffmpegSetupError.clear();
            SetFFmpegSetupStatus(FFmpegToastState::Success,L"FFmpeg is ready",L"Using the installed ffmpeg.exe.");
            PostThreadMessageW(g_threadId,WM_APP_RELOAD_ITEMS_ONLY,0,0);
        }
        return;
    }
    DWORD error=GetLastError();
    g_ffmpegSetupAttempted=true;
    if (attributes!=INVALID_FILE_ATTRIBUTES) {
        g_ffmpegSetupError=L"The ffmpeg.exe installation path is a directory.";
        return;
    }
    if (error!=ERROR_FILE_NOT_FOUND && error!=ERROR_PATH_NOT_FOUND) {
        g_ffmpegSetupError=L"Cannot inspect ffmpeg.exe (Windows error "+std::to_wstring(error)+L").";
        return;
    }
    g_ffmpegLastFailure.clear();
    g_ffmpegProgressOffset = 0;
    g_ffmpegProgressPending.clear();
    g_ffmpegProgressPoll = 0;
    SetFFmpegSetupStatus(FFmpegToastState::Progress, L"Setting up FFmpeg",
        g_ffmpegMediaBlocked ? L"Retrying FFmpeg setup..." : L"Starting FFmpeg setup...");
    Wh_Log(L"[FFmpeg] Installation target: %s", exe.c_str());
    wchar_t temp[MAX_PATH], unique[MAX_PATH], system[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, temp);
    if (!n || n >= MAX_PATH || !GetTempFileNameW(temp, L"AWF", 0, unique)) {
        g_ffmpegSetupError = L"Cannot create the FFmpeg setup temporary folder."; return;
    }
    DeleteFileW(unique);
    if (!CreateDirectoryW(unique, nullptr)) {
        g_ffmpegSetupError = L"Cannot create the FFmpeg setup temporary folder."; return;
    }
    g_ffmpegSetupTemp = unique;
    std::wstring installDir = exe.substr(0, exe.find_last_of(L"\\/"));
    g_ffmpegSetupStage = installDir + L"\\ffmpeg-" + GetFileNameFromPath(unique) + L".new.exe";
    std::wstring script = L"$work=" + PSString(g_ffmpegSetupTemp) +
        L"; $dest=" + PSString(exe) + L"; $stage=" + PSString(g_ffmpegSetupStage) + LR"PS(
$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
$base = 'https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.7z'
$headers = @{ 'User-Agent' = 'AnimaWindhawkEngine-FFmpegSetup'; 'Cache-Control' = 'no-cache' }
function Report([string]$message) {
    # Progress reporting must never make an otherwise valid installation fail.
    try { [IO.File]::AppendAllText((Join-Path $work 'progress.log'), $message + "`n", [Text.UTF8Encoding]::new($false)) } catch {}
}
function TextGet([string]$url) {
    $response = Invoke-WebRequest -UseBasicParsing -Uri $url -Headers $headers -TimeoutSec 30
    $content = $response.Content
    # Windows PowerShell can return binary Content for .ver/.sha256 responses.
    # Decode bytes before invoking string methods; casting bytes to string is not decoding.
    if ($content -is [byte[]]) {
        $text = [Text.Encoding]::UTF8.GetString($content)
    } elseif ($content -is [string]) {
        $text = $content
    } else {
        throw ('Unexpected HTTP content type for ' + $url)
    }
    return $text.TrimStart([char]0xFEFF).Trim()
}
function Download([string]$url, [string]$path) {
    Invoke-WebRequest -UseBasicParsing -Uri $url -Headers $headers -TimeoutSec 300 -OutFile $path
    if (!(Test-Path -LiteralPath $path -PathType Leaf) -or (Get-Item -LiteralPath $path).Length -eq 0) {
        throw 'Downloaded file is empty.'
    }
    Report ('Downloaded ' + [IO.Path]::GetFileName($path) + ' (' + [Math]::Round((Get-Item -LiteralPath $path).Length / 1MB, 1) + ' MiB).')
}
function HashCheck([string]$path, [string]$expected) {
    if ($expected -notmatch '^[0-9a-fA-F]{64}$') { throw 'Invalid SHA-256 metadata.' }
    if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ine $expected) {
        throw 'SHA-256 verification failed; nothing was installed. Retry after restarting the mod.'
    }
}
try {
    if ([IO.File]::Exists($dest)) { exit 0 }
    Report 'Checking the latest FFmpeg release...'
    $version = TextGet ($base + '.ver')
    if ($version -notmatch '^[0-9]+(\.[0-9]+){1,3}$') { throw 'Invalid FFmpeg release version.' }
    Report ('Latest FFmpeg release: ' + $version)
    # GitHub's release asset URL pins 7zr to the same release as its digest.
    Report 'Checking the latest 7-Zip release...'
    $release = (TextGet 'https://api.github.com/repos/ip7z/7zip/releases/latest') | ConvertFrom-Json
    $asset = @($release.assets | Where-Object { $_.name -eq '7zr.exe' })
    if ($asset.Count -ne 1) { throw 'Latest 7-Zip release has no unique 7zr.exe asset.' }
    $url = [string]$asset[0].browser_download_url
    if ($url -notmatch '^https://github\.com/ip7z/7zip/releases/download/[0-9.]+/7zr\.exe$') {
        throw 'Unexpected 7zr download URL.'
    }
    $digest = [string]$asset[0].digest
    if ($digest -notmatch '^sha256:([0-9a-fA-F]{64})$') { throw '7zr SHA-256 is unavailable.' }
    $sevenHash = $Matches[1]
    $seven = Join-Path $work '7zr.exe'
    Report ('Downloading 7zr.exe (7-Zip ' + $release.tag_name + ')...')
    Download $url $seven
    Report 'Verifying the 7zr.exe SHA-256 checksum...'
    HashCheck $seven $sevenHash
    $archive = Join-Path $work 'ffmpeg.7z'
    Report 'Fetching the FFmpeg archive checksum...'
    $archiveHash = ((TextGet ($base + '.sha256')) -split '\s+')[0]
    Report 'Downloading FFmpeg essentials. This may take a few minutes...'
    Download $base $archive
    Report 'Verifying the FFmpeg archive SHA-256 checksum...'
    HashCheck $archive $archiveHash
    # The rolling URL may change during download. Never mislabel the installed build.
    if ((TextGet ($base + '.ver')) -ne $version) { throw 'FFmpeg release changed during download. Use Retry FFmpeg setup to try again.' }
    # Flatten only ffmpeg.exe from the archive's versioned bin directory.
    Report 'Extracting ffmpeg.exe with 7zr...'
    & $seven e $archive ('-o' + $work) '-r' 'ffmpeg.exe' '-y' '-bd' '-bso0' '-bsp0' '-bse0'
    if ($LASTEXITCODE -ne 0) { throw '7zr could not extract FFmpeg.' }
    $candidate = Join-Path $work 'ffmpeg.exe'
    if (!(Test-Path -LiteralPath $candidate -PathType Leaf)) { throw 'Archive did not contain ffmpeg.exe.' }
    Report 'Checking that ffmpeg.exe can run...'
    & $candidate '-hide_banner' '-version' *> $null
    if ($LASTEXITCODE -ne 0) { throw 'Downloaded FFmpeg cannot run on this Windows installation.' }
    Report 'Copying ffmpeg.exe to the installation folder...'
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($dest)) | Out-Null
    [IO.File]::Copy($candidate, $stage, $true)
    Report 'Installing the verified FFmpeg executable...'
    try { [IO.File]::Move($stage, $dest) }
    catch [IO.IOException] { if ([IO.File]::Exists($dest)) { exit 0 }; throw }
    [IO.File]::WriteAllText((Join-Path $work 'installed.flag'), $version)
    exit 0
} catch {
    [IO.File]::WriteAllText((Join-Path $work 'error.txt'), $_.Exception.Message)
    exit 1
} finally {
    Report 'Cleaning up temporary setup files...'
    foreach ($name in @('7zr.exe', 'ffmpeg.7z', 'ffmpeg.exe')) {
        Remove-Item -LiteralPath (Join-Path $work $name) -Force -ErrorAction SilentlyContinue
    }
    Remove-Item -LiteralPath $stage -Force -ErrorAction SilentlyContinue
}
)PS";
    DWORD chars = 0;
    DWORD bytes = (DWORD)(script.size() * sizeof(wchar_t));
    bool encoded = CryptBinaryToStringW((const BYTE*)script.data(), bytes,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &chars) != FALSE;
    std::wstring base64(chars, L'\0');
    if (encoded) encoded = CryptBinaryToStringW((const BYTE*)script.data(), bytes,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64[0], &chars) != FALSE;
    while (!base64.empty() && !base64.back()) base64.pop_back();
    n = GetSystemDirectoryW(system, MAX_PATH);
    if (!encoded || !n || n >= MAX_PATH) {
        g_ffmpegSetupError = L"Cannot prepare automatic FFmpeg setup."; CleanupFFmpegSetup(); return;
    }
    std::wstring powershell = std::wstring(system) + L"\\WindowsPowerShell\\v1.0\\powershell.exe";
    std::wstring cmd = MediaQuote(powershell) + L" -NoLogo -NoProfile -NonInteractive -WindowStyle Hidden -EncodedCommand " + base64;
    g_ffmpegSetupJob = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!g_ffmpegSetupJob || !SetInformationJobObject(g_ffmpegSetupJob,
        JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
        g_ffmpegSetupError = L"Cannot initialize the FFmpeg setup job."; CleanupFFmpegSetup(); return;
    }
    STARTUPINFOW si = {}; si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(powershell.c_str(), &cmd[0], nullptr, nullptr, FALSE,
        CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, g_ffmpegSetupTemp.c_str(), &si, &pi)) {
        g_ffmpegSetupError = L"Cannot start Windows PowerShell for automatic FFmpeg setup.";
        CleanupFFmpegSetup(); return;
    }
    g_ffmpegSetupProcess = pi.hProcess;
    bool started = AssignProcessToJobObject(g_ffmpegSetupJob, pi.hProcess) &&
                   ResumeThread(pi.hThread) != DWORD(-1);
    CloseHandle(pi.hThread);
    if (!started) {
        TerminateProcess(pi.hProcess, 1); WaitForSingleObject(pi.hProcess, INFINITE);
        g_ffmpegSetupError = L"Cannot start the FFmpeg setup job."; CleanupFFmpegSetup(); return;
    }
    g_ffmpegSetupStarted = GetTickCount64();
    ShowFFmpegSetupToast();
}

static void EnsureFFmpegOnEnable() {
    if (g_ffmpegSetupAttempted || g_ffmpegSetupStopping) return;
    wchar_t local[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, local))) {
        g_ffmpegSetupError = L"Cannot locate Local AppData for startup FFmpeg setup.";
        FailFFmpegSetup();
        return;
    }
    std::wstring exe = std::wstring(local) + L"\\AnimaWindhawkEngine\\ffmpeg.exe";
    StartFFmpegSetup(exe);
    if (!g_ffmpegSetupError.empty()) FailFFmpegSetup();
}

static void PollFFmpegSetup() {
    PollNativeToasts();
    if (!g_ffmpegSetupProcess) {
        if (!g_ffmpegSetupError.empty()) FailFFmpegSetup();
        return;
    }
    ReadFFmpegSetupProgress();
    if (WaitForSingleObject(g_ffmpegSetupProcess, 0) != WAIT_OBJECT_0) {
        if (GetTickCount64() - g_ffmpegSetupStarted < 10 * 60 * 1000) return;
        TerminateJobObject(g_ffmpegSetupJob, 1);
        WaitForSingleObject(g_ffmpegSetupProcess, INFINITE);
        g_ffmpegSetupError = L"Automatic FFmpeg setup timed out. Use Retry FFmpeg setup to try again.";
    }
    ReadFFmpegSetupProgress(true);
    DWORD code = 1; GetExitCodeProcess(g_ffmpegSetupProcess, &code);
    bool installed = GetFileAttributesW((g_ffmpegSetupTemp + L"\\installed.flag").c_str()) != INVALID_FILE_ATTRIBUTES;
    if (code != 0 && g_ffmpegSetupError.empty()) {
        HANDLE f = CreateFileW((g_ffmpegSetupTemp + L"\\error.txt").c_str(), GENERIC_READ,
            FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (f != INVALID_HANDLE_VALUE) {
            char text[4096]; DWORD got = 0;
            if (ReadFile(f, text, sizeof(text), &got, nullptr) && got) {
                int len = MultiByteToWideChar(CP_UTF8, 0, text, got, nullptr, 0);
                g_ffmpegSetupError.resize(len);
                if (len) MultiByteToWideChar(CP_UTF8, 0, text, got, &g_ffmpegSetupError[0], len);
            }
            CloseHandle(f);
        }
        if (g_ffmpegSetupError.empty()) g_ffmpegSetupError = L"Automatic FFmpeg setup failed. Use Retry FFmpeg setup to try again.";
    }
    bool failed = !g_ffmpegSetupError.empty();
    CleanupFFmpegSetup();
    if (failed) {
        FailFFmpegSetup();
        ShowFFmpegSetupToast();
    } else {
        g_ffmpegMediaBlocked = false;
        SetFFmpegSetupStatus(FFmpegToastState::Success, L"FFmpeg is ready",
            installed ? L"FFmpeg installed successfully. Reloading your saved media..."
                      : L"The installed FFmpeg is ready to use.");
        PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
    }
}

static bool LoadDecodedMedia(PetItem& it, size_t& totalBytes, std::wstring& error) {
    wchar_t local[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, local))) {
        g_ffmpegSetupError = L"Cannot locate Local AppData.";
        FailFFmpegSetup(); return false;
    }
    std::wstring exe = std::wstring(local) + L"\\AnimaWindhawkEngine\\ffmpeg.exe";
    StartFFmpegSetup(exe);
    if (!g_ffmpegSetupError.empty()) {
        FailFFmpegSetup();
        return false;
    }
    if (g_ffmpegMediaBlocked) {
        ShowFFmpegSetupToast();
        return false;
    }
    DWORD attrs = GetFileAttributesW(exe.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        if (!g_ffmpegSetupProcess) {
            g_ffmpegSetupError = L"FFmpeg is missing. Use Retry FFmpeg setup.";
            FailFFmpegSetup();
        }
        return false;
    }
    wchar_t temp[MAX_PATH], unique[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, temp);
    if (!n || n >= MAX_PATH || !GetTempFileNameW(temp, L"AWE", 0, unique)) {
        error = L"Cannot create temporary frame directory."; return false;
    }
    DeleteFileW(unique);
    if (!CreateDirectoryW(unique, nullptr)) {
        error = L"Cannot create temporary frame directory."; return false;
    }
    MediaTempFrames frames; frames.dir = unique;
    std::wstring cmd = MediaQuote(exe) +
        L" -hide_banner -loglevel error -nostdin -y -protocol_whitelist file,pipe"
        L" -threads 2 -i " + MediaQuote(it.filePath) +
        L" -map 0:v:0 -an -sn -dn -vf " +
        MediaQuote(L"scale=w='min(512,iw)':h='min(512,ih)':force_original_aspect_ratio=decrease,fps=20") +
        L" -frames:v 301 -threads 2 -c:v png -pix_fmt rgba -f image2 " +
        MediaQuote(frames.dir + L"\\frame-%04d.png");
    STARTUPINFOW si = {}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    HANDLE job = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!job || !SetInformationJobObject(job, JobObjectExtendedLimitInformation,
                                         &limits, sizeof(limits))) {
        if (job) CloseHandle(job);
        error = L"Cannot initialize decoder job."; return false;
    }
    if (!CreateProcessW(exe.c_str(), &cmd[0], nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, frames.dir.c_str(), &si, &pi)) {
        CloseHandle(job); error = L"Cannot start ffmpeg.exe."; return false;
    }
    bool assigned = AssignProcessToJobObject(job, pi.hProcess) != FALSE;
    bool started = assigned && ResumeThread(pi.hThread) != DWORD(-1);
    DWORD wait = started ? WaitForSingleObject(pi.hProcess, 60000) : WAIT_FAILED;
    if (wait != WAIT_OBJECT_0) {
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, 5000);
    }
    DWORD code = 1; GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess); CloseHandle(job);
    if (wait != WAIT_OBJECT_0 || code != 0) {
        error = wait == WAIT_TIMEOUT ? L"Decoding exceeded 60 seconds. Use a shorter clip." :
            L"FFmpeg could not decode this file. Check the file and update FFmpeg (animated WebP needs a recent build).";
        return false;
    }
    if (GetFileAttributesW(frames.Frame(301).c_str()) != INVALID_FILE_ATTRIBUTES) {
        error = L"Clip exceeds 300 frames (about 15 seconds). Trim it first."; return false;
    }
    size_t bytes = 0;
    for (UINT i = 1; i <= 300; ++i) {
        std::wstring framePath = frames.Frame(i);
        if (GetFileAttributesW(framePath.c_str()) == INVALID_FILE_ATTRIBUTES) break;
        Gdiplus::Bitmap source(framePath.c_str());
        UINT w = source.GetWidth(), h = source.GetHeight();
        size_t cost = size_t(w) * size_t(h) * 4;
        if (source.GetLastStatus() != Gdiplus::Ok || !w || !h || w > 512 || h > 512 ||
            bytes + cost > 128u * 1024 * 1024 || totalBytes + bytes + cost > 256u * 1024 * 1024) {
            error = L"Invalid frame or decoded memory limit exceeded. Use fewer or smaller media files."; break;
        }
        auto* bitmap = source.Clone(0, 0, (INT)w, (INT)h, PixelFormat32bppPARGB);
        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
            delete bitmap; error = L"Cannot allocate decoded frame."; break;
        }
        it.decodedFrames.push_back(bitmap);
        bytes += cost;
    }
    if (!error.empty() || it.decodedFrames.empty()) {
        for (auto* frame : it.decodedFrames) delete frame;
        it.decodedFrames.clear();
        if (error.empty()) error = L"No decodable video/image frames were found.";
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

static void DestroyAllItems() {
    for (auto& it : g_items) {
        if (it.hwnd)   DestroyWindow(it.hwnd);
        if (it.decodedFrames.empty()) delete it.image;
        else for (auto* frame : it.decodedFrames) delete frame;
        if (it.hThumb) DeleteObject(it.hThumb);
    }
    g_items.clear();
}

static void ApplyTimer() {
    int fastest=g_settings.maxFps;
    for (const auto& pet : g_items)
        if (pet.isGif && pet.frameCount>1) fastest=std::max(fastest,std::min(pet.monitorHz,pet.fpsLimit));
    int periodMs = std::max(1,1000 / fastest);
    if (periodMs > 500) periodMs = 500;
    SetTimer(g_hCtrlWnd, kAnimTimerId, (UINT)periodMs, nullptr);
}

static void RemoveTrayIcon() {
    if (!g_trayIconAdded) return;
    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = g_hCtrlWnd;
    nid.uID  = kTrayUid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    g_trayIconAdded = false;
}

static void ApplyTrayIcon() {
    if (g_settings.trayIcon && !g_trayIconAdded) {
        if (!g_hManagerIcon)
            g_hManagerIcon = LoadManagerIcon();

        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd             = g_hCtrlWnd;
        nid.uID              = kTrayUid;
        nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = kTrayMsg;
        nid.hIcon            = g_hManagerIcon
                                   ? g_hManagerIcon
                                   : LoadIconW(nullptr, (PCWSTR)IDI_APPLICATION);
        wcsncpy_s(nid.szTip, L"AnimaWindhawkEngine", _TRUNCATE);
        g_trayIconAdded = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
    } else if (!g_settings.trayIcon && g_trayIconAdded) {
        RemoveTrayIcon();
    }
}

static void ReloadAll(bool rereadSettings,bool importWindhawk) {
    bool managerWasOpen = g_hManagerWnd != nullptr;

    std::wstring selectedKey;
    if (auto selected=SelectedPet()) selectedKey=selected->registryKey;
    std::map<std::wstring,POINT> positions;
    std::map<std::wstring,std::vector<POINT>> unnamedPositions;
    for (const auto& item : g_items) {
        RECT rect{};
        POINT pos{item.posX,item.posY};
        if (item.hwnd && GetWindowRect(item.hwnd,&rect)) pos={rect.left,rect.top};
        if (item.registryKey.empty()) unnamedPositions[item.filePath].push_back(pos);
        else positions[item.registryKey]=pos;
    }
    DestroyAllItems();
    if (rereadSettings)
        ReadGlobalSettings(g_settings,importWindhawk);
    ApplyTrayIcon();

    ReadLibraryFolders();
    bool legacyFolder=g_settings.useFolderMode && !g_settings.imagesFolder.empty();
    bool migrated=legacyFolder && RegisterLibraryFolder(g_settings.imagesFolder);
    std::vector<std::pair<std::wstring,std::wstring>> files;
    if (!legacyFolder || migrated) {
        for (const auto& path : g_settings.imagePaths)
            if (HasSupportedExt(path) && GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES)
                files.emplace_back(path,L"");
    }
    std::set<std::wstring> emptyFolders;
    auto scan=[&](const std::wstring& path,const std::wstring& key) {
        std::vector<std::wstring> paths;
        if (ScanFolder(path,g_settings.scanSubfolders,paths) && !key.empty()) emptyFolders.insert(key);
        std::sort(paths.begin(),paths.end());
        for (auto& file : paths) files.emplace_back(std::move(file),key);
    };
    for (const auto& folder : libraryFolders) scan(folder.path,folder.key);
    if (legacyFolder && !migrated) scan(g_settings.imagesFolder,L"");
    g_items.reserve(files.size());

    static std::vector<std::wstring> warnedVideos;
    std::wstring failures;
    size_t decodedBytes = 0;
    std::vector<std::wstring> usedKeys;
    const int screenWidth=std::max(1,GetSystemMetrics(SM_CXSCREEN));
    const int screenHeight=std::max(1,GetSystemMetrics(SM_CYSCREEN));
    const int inset=std::min({80,screenWidth-1,screenHeight-1});
    const int cascadeSlots=1+(std::min(screenWidth,screenHeight)-1-inset)/40;
    for (const auto& [path,folderKey] : files) {
        std::wstring fn = GetFileNameFromPath(path);
        const ImageOverride* ov = FindOverride(g_settings, fn);

        PetItem it;
        it.isGif = !_wcsicmp(PathFindExtensionW(path.c_str()),L".gif");
        it.fpsLimit = std::max(5,g_settings.maxFps);
        it.filePath = path;
        it.fileName = fn;
        it.folderKey=folderKey;
        if (!folderKey.empty()) {
            it.registryKey=ImageKey(path,usedKeys,folderKey);
            if (migrated && !it.registryKey.empty() && _wcsicmp(g_settings.imagesFolder.c_str(),StringAt(imagesRoot,folderKey.c_str(),L"Path").c_str())==0) {
                for (const auto& old : Subkeys(imagesRoot)) {
                    if (_wcsicmp(StringAt(imagesRoot,old.c_str(),L"Path").c_str(),path.c_str())) continue;
                    RegistryKey from,to;
                    if (RegOpenKeyExW(imagesRoot,old.c_str(),0,KEY_READ,&from.key)==ERROR_SUCCESS &&
                        RegOpenKeyExW(imagesRoot,it.registryKey.c_str(),0,KEY_READ|KEY_WRITE,&to.key)==ERROR_SUCCESS &&
                        MissingValue(to.key,L"Enabled"))
                        ConfigWriteResult(L"Folder image migration",CopyMachineValues(from.key,to.key));
                    break;
                }
            }
            DWORD removed{},size=sizeof(removed);
            if (!it.registryKey.empty() && RegGetValueW(imagesRoot,it.registryKey.c_str(),L"Removed",RRF_RT_REG_DWORD,nullptr,&removed,&size)==ERROR_SUCCESS && removed) continue;
            emptyFolders.erase(folderKey);
        }
        if (NeedsMediaDecoder(path)) {
            if (_wcsicmp(PathFindExtensionW(path.c_str()), L".mp4") == 0 &&
                std::find(warnedVideos.begin(), warnedVideos.end(), path) == warnedVideos.end()) {
                warnedVideos.push_back(path);
                MessageBoxW(g_hManagerWnd,
                    L"MP4 support is experimental. Importing an MP4 file may cause performance drops "
                    L"and temporarily freeze the manager while decoding. Playback is silent. "
                    L"Short, low-resolution clips are recommended.",
                    L"Experimental MP4 import", MB_OK | MB_ICONWARNING);
            }
            std::wstring error;
            if (!LoadDecodedMedia(it, decodedBytes, error)) {
                if (error.empty()) continue;
                Wh_Log(L"Media import failed: %s: %s", path.c_str(), error.c_str());
                if (failures.size() < 4000) failures += fn + L": " + error + L"\n\n";
                continue;
            }
        } else {
            auto* img = new Gdiplus::Image(path.c_str());
            if (img->GetLastStatus() != Gdiplus::Ok) { delete img; continue; }
            it.image = img;
            it.baseWidth = std::max(1, (int)img->GetWidth());
            it.baseHeight = std::max(1, (int)img->GetHeight());
            it.frameCount = img->GetFrameCount(&Gdiplus::FrameDimensionTime);
            if (!it.frameCount) it.frameCount = 1;
            it.frameDelaysMs = it.frameCount > 1
                ? GetFrameDelays(img, it.frameCount) : std::vector<UINT>{100};
        }

        it.alwaysOnTop  = (ov && ov->alwaysOnTop  != -1) ? ov->alwaysOnTop  == 1 : g_settings.alwaysOnTop;
        it.clickThrough = (ov && ov->clickThrough  != -1) ? ov->clickThrough == 1 : g_settings.clickThrough;
        it.lockPosition = (ov && ov->lockPosition  != -1) ? ov->lockPosition == 1 : g_settings.lockPosition;
        it.scalePercent    = std::clamp((ov && ov->scalePercent > 0) ? ov->scalePercent : g_settings.scalePercent,1,500);
        it.rotationDegrees = ov ? ov->rotationDegrees : g_settings.rotationDegrees;
        it.opacityPercent  = (ov && ov->opacityPercent > 0) ? ov->opacityPercent : g_settings.opacityPercent;
        it.paused = it.frameCount > 1 ? g_settings.paused : true;
        it.enabled = !ov || ov->enabled;

        if (folderKey.empty()) {
            for (const auto& key : imageOrder) {
                if (std::find(usedKeys.begin(),usedKeys.end(),key)==usedKeys.end() &&
                    !_wcsicmp(StringAt(imagesRoot,key.c_str(),L"Path").c_str(),path.c_str())) {
                    it.registryKey=key;
                    break;
                }
            }
        }
        if (it.registryKey.empty() && folderKey.empty()) it.registryKey=ImageKey(path,usedKeys);
        usedKeys.push_back(it.registryKey);
        SyncImageState(it,true);
        POINT storedPos{};
        const int offset=inset+40*static_cast<int>(g_items.size()%cascadeSlots);
        POINT fallback{offset,offset};
        if (ov && ov->posX>=0) fallback.x=ov->posX;
        if (ov && ov->posY>=0) fallback.y=ov->posY;
        if (!MonitorFromPoint(fallback,MONITOR_DEFAULTTONULL)) fallback={offset,offset};
        bool hasStoredPos=ReadStoredPosition(it.registryKey,storedPos);
        POINT position=hasStoredPos ? storedPos : fallback;
        auto live=positions.find(it.registryKey);
        if (live!=positions.end()) position=live->second;
        else if (it.registryKey.empty()) {
            auto& prior=unnamedPositions[path];
            if (!prior.empty()) { position=prior.front(); prior.erase(prior.begin()); }
        }
        if (!MonitorFromPoint(position,MONITOR_DEFAULTTONULL)) position=fallback;
        it.posX=position.x;
        it.posY=position.y;
        if (!hasStoredPos || storedPos.x!=position.x || storedPos.y!=position.y)
            SaveStoredPosition(it.registryKey,it.posX,it.posY);

        g_items.push_back(std::move(it));
        PetItem& ref = g_items.back();
        ref.hwnd = CreateItemWindow(ref);
        if (ref.isGif) ref.monitorHz=MonitorRefreshRate(ref.hwnd);
        if (ref.hwnd) RenderItem(ref);
    }

    libraryFolders.erase(std::remove_if(libraryFolders.begin(),libraryFolders.end(),
        [&](const LibraryFolder& folder) {
            return emptyFolders.count(folder.key) && RemoveLibraryFolder(folder.key);
        }),libraryFolders.end());
    if (migrated) { g_settings.useFolderMode=false; SaveNumber(L"UseFolderMode",0); }
    Wh_Log(L"Loaded %zu media items across %zu folders and the individual image list",g_items.size(),libraryFolders.size());
    ApplyTimer();
    if (!failures.empty()) MessageBoxW(g_hManagerWnd, failures.c_str(),
        L"Some media could not be imported", MB_OK | MB_ICONWARNING);

    if (managerWasOpen && g_hManagerWnd) {
        g_managerSel=-1;
        for (int i=0;!selectedKey.empty() && i<(int)g_items.size();++i)
            if (g_items[i].registryKey==selectedKey) { g_managerSel=i; break; }
        SendMessageW(g_hManagerWnd, WM_APP_RELOAD, 0, 0);
    }
}


static void ShowAllHidden() {
    for (auto& it : g_items)
        if (it.hidden && it.hwnd) { it.hidden = false; if (it.enabled) ShowWindow(it.hwnd, SW_SHOWNOACTIVATE); }
}



static HWND g_hWelcomeWnd = nullptr;
static HICON g_hWelcomeIcon = nullptr;
static const wchar_t kWelcomeWndClass[] = L"AnimaWindhawkEngine_Welcome";
static const UINT kShowWelcome = WM_APP + 30;
static HWND readyDialog=nullptr;
static INT_PTR CALLBACK ReadyDialogProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM) {
    if (msg==WM_INITDIALOG) {
        SetWindowTextW(hwnd,L"AnimaWindhawkEngine");
        auto add=[&](PCWSTR cls,PCWSTR text,DWORD style,RECT bounds,int id) {
            MapDialogRect(hwnd,&bounds);
            HWND control=CreateWindowExW(0,cls,text,WS_CHILD|WS_VISIBLE|style,
                bounds.left,bounds.top,bounds.right-bounds.left,bounds.bottom-bounds.top,
                hwnd,reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),GetModuleHandleW(nullptr),nullptr);
            if (control) SendMessageW(control,WM_SETFONT,SendMessageW(hwnd,WM_GETFONT,0,0),TRUE);
            return control;
        };
        add(L"STATIC",L"AnimaWindhawkEngine loaded successfully and is ready to use.",
            SS_LEFT,{12,10,220,36},-1);
        SetFocus(add(L"BUTTON",L"OK",WS_TABSTOP|BS_DEFPUSHBUTTON,{168,42,220,57},IDOK));
        return FALSE;
    }
    if (msg==WM_CLOSE || (msg==WM_COMMAND && (LOWORD(wp)==IDOK || LOWORD(wp)==IDCANCEL))) {
        DestroyWindow(hwnd);
        if (!ConfigNumber(L"WelcomeShown",0)) PostMessageW(g_hCtrlWnd,kShowWelcome,0,0);
        return TRUE;
    }
    if (msg==WM_NCDESTROY) readyDialog=nullptr;
    return FALSE;
}
static void ShowReadyDialog() {
    struct alignas(DWORD) ReadyTemplate {
        DLGTEMPLATE dialog;
        WORD menu, windowClass, title, pointSize;
        wchar_t font[9];
    };
    static_assert(offsetof(ReadyTemplate,menu)==sizeof(DLGTEMPLATE));
    const ReadyTemplate layout={{WS_POPUP|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|DS_CENTER|DS_SETFONT,
        WS_EX_APPWINDOW|WS_EX_TOPMOST,0,0,0,232,65},0,0,0,9,L"Segoe UI"};
    readyDialog=CreateDialogIndirectParamW(GetModuleHandleW(nullptr),&layout.dialog,
        nullptr,ReadyDialogProc,0);
    if (readyDialog) {
        SetWindowPos(readyDialog,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
        SetForegroundWindow(readyDialog);
        UpdateWindow(readyDialog);
        Wh_Log(L"Startup dialog visible: %d",IsWindowVisible(readyDialog));
    }
    else {
        Wh_Log(L"Could not show the startup dialog (Windows error %lu)",GetLastError());
        if (!ConfigNumber(L"WelcomeShown",0)) PostMessageW(g_hCtrlWnd,kShowWelcome,0,0);
    }
}
static const int kWelcomeWorkshop = 901, kWelcomeManager = 902, kWelcomeRepo = 903;
static const wchar_t kWelcomeRepoUrl[] = L"https://github.com/tubeo5866/anima-windhawk-engine";
static hosting::DesktopWindowXamlSource welcomeSource{nullptr};
static winrt::com_ptr<IDesktopWindowXamlSourceNative2> welcomeInput;
static HWND welcomeIsland{};
static const wchar_t* welcomeStage{};

static void BuildWelcome(HWND hwnd) {
    welcomeStage=L"Activate welcome manifest";
    ManagerManifestScope manifest;
    AttachIsland(hwnd,welcomeSource,welcomeIsland,welcomeInput,welcomeStage);
    welcomeStage=L"Build welcome controls";
    auto root=xaml::Markup::XamlReader::Load(LR"GUIDE(
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
</Grid>)GUIDE").as<controls::Grid>();
    std::wstring hint;
    if (g_hotkeyVk) hint=L"Open the manager with "+HotkeyToString(g_hotkeyMods,g_hotkeyVk)+L". ";
    hint+=L"You can also use the tray icon or right-click an image. Change the shortcut in App settings.";
    root.FindName(L"Shortcut").as<TextBlock>().Text(hint);
    auto bind=[&](PCWSTR name,int command) {
        root.FindName(name).as<controls::Primitives::ButtonBase>().Click([hwnd,command](auto&&,auto&&) {
            PostMessageW(hwnd,WM_COMMAND,MAKEWPARAM(command,BN_CLICKED),0);
        });
    };
    bind(L"Browse",kWelcomeWorkshop); bind(L"Manage",kWelcomeManager);
    bind(L"Dismiss",IDCANCEL); bind(L"Repo",kWelcomeRepo);
    welcomeSource.Content(root);
    BOOL dark=TRUE; DWORD corners=2; COLORREF caption=RGB(32,32,32);
    DwmSetWindowAttribute(hwnd,20,&dark,sizeof(dark));
    DwmSetWindowAttribute(hwnd,33,&corners,sizeof(corners));
    DwmSetWindowAttribute(hwnd,35,&caption,sizeof(caption));
    RECT rc{}; GetClientRect(hwnd,&rc);
    SetWindowPos(welcomeIsland,nullptr,0,0,rc.right,rc.bottom,SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}
static void CloseWelcomeIsland() {
    welcomeInput=nullptr;
    welcomeIsland=nullptr;
    auto source=std::exchange(welcomeSource,hosting::DesktopWindowXamlSource{nullptr});
    if (source) {
        try { source.Close(); }
        catch (const winrt::hresult_error& e) { Wh_Log(L"[Welcome] Close failed: %s",e.message().c_str()); }
    }
}
static LRESULT CALLBACK WelcomeWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        try { BuildWelcome(hwnd); return 0; }
        catch (const winrt::hresult_error& e) {
            Wh_Log(L"[Welcome] %s failed (0x%08X): %s",welcomeStage,(unsigned)e.code().value,e.message().c_str());
            CloseWelcomeIsland(); return -1;
        }
    case WM_SIZE:
        if (welcomeIsland) SetWindowPos(welcomeIsland,nullptr,0,0,LOWORD(lp),HIWORD(lp),SWP_NOZORDER|SWP_NOACTIVATE);
        return 0;
    case WM_SETFOCUS:
        if (welcomeIsland) SetFocus(welcomeIsland);
        return 0;
    case WM_DPICHANGED: {
        auto r=reinterpret_cast<RECT*>(lp);
        SetWindowPos(hwnd,nullptr,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp)==kWelcomeRepo && HIWORD(wp)==BN_CLICKED) {
            if ((INT_PTR)ShellExecuteW(hwnd, L"open", kWelcomeRepoUrl, nullptr, nullptr, SW_SHOWNORMAL) <= 32)
                MessageBoxW(hwnd, L"Could not open your browser. Visit:\nhttps://github.com/tubeo5866/anima-windhawk-engine",
                    L"GitHub repository", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        if (LOWORD(wp)==IDCANCEL) { DestroyWindow(hwnd); return 0; }
        if (LOWORD(wp)==kWelcomeWorkshop || LOWORD(wp)==kWelcomeManager) {
            bool workshop = LOWORD(wp)==kWelcomeWorkshop;
            OpenPetManager();
            if (g_hManagerWnd) {
                ShowWindow(g_hManagerWnd,SW_RESTORE);
                SetForegroundWindow(g_hManagerWnd);
                if (workshop) PostMessageW(g_hManagerWnd,WM_COMMAND,
                    MAKEWPARAM(IDC_NAV_WORKSHOP,BN_CLICKED),0);
                DestroyWindow(hwnd);
            }
            return 0;
        }
        break;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        CloseWelcomeIsland();
        SendMessageW(hwnd,WM_SETICON,ICON_BIG,0);
        SendMessageW(hwnd,WM_SETICON,ICON_SMALL,0);
        if (g_hWelcomeIcon) { DestroyIcon(g_hWelcomeIcon); g_hWelcomeIcon=nullptr; }
        return 0;
    case WM_NCDESTROY:
        if (g_hWelcomeWnd==hwnd) g_hWelcomeWnd=nullptr;
        break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

static void ShowWelcomeGuide(bool recordFirstRun) {
    if (g_hWelcomeWnd) {
        ShowWindow(g_hWelcomeWnd,SW_RESTORE);
        SetForegroundWindow(g_hWelcomeWnd);
        return;
    }
    auto oldDpi=SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    UINT dpi=GetDpiForSystem();
    RECT r{0,0,MulDiv(620,dpi,96),MulDiv(540,dpi,96)};
    DWORD style=WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectExForDpi(&r,style,FALSE,WS_EX_APPWINDOW,dpi);
    POINT cursor{}; GetCursorPos(&cursor);
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromPoint(cursor,MONITOR_DEFAULTTONEAREST),&monitor);
    int w=r.right-r.left, h=r.bottom-r.top;
    int x=monitor.rcWork.left+std::max(0,((int)(monitor.rcWork.right-monitor.rcWork.left)-w)/2);
    int y=monitor.rcWork.top+std::max(0,((int)(monitor.rcWork.bottom-monitor.rcWork.top)-h)/2);
    g_hWelcomeWnd=CreateWindowExW(WS_EX_APPWINDOW,
        kWelcomeWndClass,L"Welcome to AnimaWindhawkEngine",style,x,y,w,h,
        nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    if (oldDpi) SetThreadDpiAwarenessContext(oldDpi);
    if (!g_hWelcomeWnd) { Wh_Log(L"Could not create welcome guide"); return; }
    g_hWelcomeIcon = g_hManagerIcon ? CopyIcon(g_hManagerIcon) : LoadManagerIcon();
    if (g_hWelcomeIcon) {
        SendMessageW(g_hWelcomeWnd, WM_SETICON, ICON_BIG, (LPARAM)g_hWelcomeIcon);
        SendMessageW(g_hWelcomeWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hWelcomeIcon);
    }
    ShowWindow(g_hWelcomeWnd,SW_SHOWNORMAL);
    UpdateWindow(g_hWelcomeWnd);
    if (recordFirstRun) {
        DWORD shown=1;
        WriteConfig(L"WelcomeShown",REG_DWORD,&shown,sizeof(shown));
    }
}


static HANDLE disableProcess{};
static bool quitRequested=false;

static bool elevatedDisableAttempt=false;

static DWORD StartDisableCommand(HWND hwnd,bool elevated) {
    std::wstring host(32768,L'\0');
    DWORD length=GetModuleFileNameW(nullptr,host.data(),(DWORD)host.size());
    if (!length) return GetLastError();
    if (length>=host.size()) return ERROR_INSUFFICIENT_BUFFER;
    host.resize(length);
    std::wstring cli=host.substr(0,host.find_last_of(L"\\")+1)+L"windhawk-cli.exe";
    std::wstring args=L"mod disable "+MediaQuote(WH_MOD_ID)+L" --yes";
    if (elevated) {
        elevatedDisableAttempt=true;
        SHELLEXECUTEINFOW launch{sizeof(launch)};
        launch.fMask=SEE_MASK_NOCLOSEPROCESS|SEE_MASK_NOASYNC|SEE_MASK_FLAG_NO_UI;
        launch.hwnd=hwnd;
        launch.lpVerb=L"runas";
        launch.lpFile=cli.c_str();
        launch.lpParameters=args.c_str();
        launch.nShow=SW_HIDE;
        if (!ShellExecuteExW(&launch)) return GetLastError();
        if (!launch.hProcess) return ERROR_INVALID_HANDLE;
        disableProcess=launch.hProcess;
    } else {
        std::wstring command=MediaQuote(cli)+L" "+args;
        STARTUPINFOW startup{sizeof(startup)};
        PROCESS_INFORMATION child{};
        if (!CreateProcessW(cli.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,
                            nullptr,nullptr,&startup,&child)) return GetLastError();
        CloseHandle(child.hThread);
        disableProcess=child.hProcess;
    }
    return ERROR_SUCCESS;
}

static void QuitFromTray(HWND hwnd) {
    if (quitRequested) return;
    quitRequested=true;
    if (!ConfigNumber(L"QuitNoticeShown",0)) {
        if (!MessageBoxW(hwnd,L"To start the mod, open Windhawk and toggle the mod back",
                         L"Hold on, before exiting",MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND)) {
            quitRequested=false;
            return;
        }
        SaveNumber(L"QuitNoticeShown",1);
    }

    elevatedDisableAttempt=false;
    DWORD error=StartDisableCommand(hwnd,false);
    if (error==ERROR_SUCCESS) return;
    if (error==ERROR_ELEVATION_REQUIRED) {
        error=StartDisableCommand(hwnd,true);
        if (error==ERROR_SUCCESS) return;
        if (error==ERROR_CANCELLED) { quitRequested=false; return; }
    } else if (error==ERROR_FILE_NOT_FOUND || error==ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"[Quit] Windhawk CLI unavailable. Stopping this session.");
        PostThreadMessageW(g_threadId,WM_APP_QUIT,0,0);
        return;
    }
    Wh_Log(L"[Quit] Could not start Windhawk CLI (%lu)",error);
    quitRequested=false;
    MessageBoxW(hwnd,L"Open Windhawk and disable AnimaWindhawkEngine to quit.",
                L"Could not disable the mod",MB_OK|MB_ICONWARNING);
}

static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_WS_FETCH_DONE: {
        auto* res = (WorkshopFetchResult*)lParam;
        if (!res->commit.empty()) {
            g_repoCommit = res->commit;
            WriteConfig(L"RepositoryCommit",REG_SZ,g_repoCommit.c_str(),(DWORD)((g_repoCommit.size()+1)*sizeof(wchar_t)));

        }
        g_workshopFetching = false;
        g_workshopFetched  = true;
        if (wParam == 0) {
            for (auto& item : g_workshopItems) if (item.hPreviewBmp) DeleteObject(item.hPreviewBmp);
            g_workshopItems = std::move(res->items);
            for (auto& item : g_workshopItems) {
                item.installed = !item.localImagePath.empty() && std::any_of(g_settings.imagePaths.begin(), g_settings.imagePaths.end(),
                    [&](const auto& path) { return _wcsicmp(path.c_str(),item.localImagePath.c_str())==0; });
            }
            g_workshopError.clear();
        } else {
            g_workshopError = res->error;
        }
        delete res;
        if (managerRoot && page==Page::Workshop) RenderPage();
        return 0;
    }
    case WM_WS_INSTALL_DONE: {
        int  idx = (int)wParam;
        bool ok  = (lParam == 0);
        if (idx >= 0 && idx < (int)g_workshopItems.size()) {
            auto& it = g_workshopItems[idx];
            it.installing = false;
            if (ok) {
                it.installed      = true;
                it.localImagePath = GetWorkshopDir() + L"\\" + it.slug + L"\\" + it.file;
                bool already = false;
                for (auto& p : g_settings.imagePaths)
                    if (p == it.localImagePath) { already = true; break; }
                if (!already) {
                    g_settings.imagePaths.push_back(it.localImagePath);
                    SaveImagePaths(g_settings.imagePaths);
                    PostThreadMessageW(g_threadId, WM_APP_RELOAD_ITEMS_ONLY, 0, 0);
                }
            }
        }
        if (!ok) g_workshopError=L"Installation failed. Refresh to try again.";
        if (managerRoot && page==Page::Workshop) RenderPage();
        return 0;
    }

    case WM_DISPLAYCHANGE:
        for (auto& pet : g_items) if (pet.isGif) pet.monitorHz=MonitorRefreshRate(pet.hwnd);
        if (g_settings.maxFps>0) ApplyTimer();
        if (managerRoot && page==Page::Pet) RenderPage();
        return 0;
    case kShowWelcome:
        ShowWelcomeGuide(true);
        return 0;
    case WM_HOTKEY:
        if (wParam == HOTKEY_OPEN_MANAGER)
            OpenPetManager();
        return 0;

    case WM_TIMER:
        if (wParam == kAnimTimerId) {
            if (disableProcess && WaitForSingleObject(disableProcess,0)==WAIT_OBJECT_0) {
                DWORD code=1;
                GetExitCodeProcess(disableProcess,&code);
                CloseHandle(disableProcess);
                disableProcess=nullptr;
                if (code==0) {
                    PostThreadMessageW(g_threadId,WM_APP_QUIT,0,0);
                } else {
                    Wh_Log(L"[Quit] Windhawk CLI returned %lu (elevated retry: %d)",code,elevatedDisableAttempt);
                    if (code==12 && !elevatedDisableAttempt) {
                        DWORD error=StartDisableCommand(hwnd,true);
                        if (error==ERROR_SUCCESS) return 0;
                        if (error==ERROR_CANCELLED) { quitRequested=false; return 0; }
                        Wh_Log(L"[Quit] Elevated CLI launch failed (%lu)",error);
                    }
                    MessageBoxW(hwnd,L"Open Windhawk and disable AnimaWindhawkEngine to quit.",
                                L"Could not disable the mod",MB_OK|MB_ICONWARNING);
                    quitRequested=false;
                }
            }
            PollFFmpegSetup();
            ULONGLONG now = GetTickCount64();
            for (auto& it : g_items) {
                if (it.frameCount <= 1 || it.paused || it.frameDelaysMs.empty()) continue;
                UINT delay = it.frameDelaysMs[it.currentFrame % it.frameDelaysMs.size()];
                int fps=it.isGif ? std::min(it.fpsLimit,it.monitorHz) : g_settings.maxFps;
                delay=std::max(delay,1u+999u/static_cast<UINT>(fps));
                if (now - it.lastFrameTick >= delay) {
                    it.currentFrame = (it.currentFrame + 1) % it.frameCount;
                    if (it.decodedFrames.empty())
                        it.image->SelectActiveFrame(&Gdiplus::FrameDimensionTime, it.currentFrame);
                    else it.image = it.decodedFrames[it.currentFrame];
                    RenderItem(it);
                    it.lastFrameTick = now;
                }
            }
        }
        return 0;

    case kTrayMsg: {
        UINT evt = (UINT)lParam;
        if (evt == WM_LBUTTONUP) {
            OpenPetManager(-1);
            return 0;
        }
        if (evt == WM_RBUTTONUP) {
            HMENU m = CreatePopupMenu();
            AppendMenuW(m, MF_STRING, kCmdOpenManagerTray, L"Open Pet Manager…");
            AppendMenuW(m, MF_STRING, kCmdQuit, L"Quit");

            POINT pt{}; GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(m);

            if (cmd==kCmdOpenManagerTray) OpenPetManager(-1);
            if (cmd==kCmdQuit) QuitFromTray(hwnd);
        }
        return 0;
    }
    }

    if (g_taskbarCreatedMsg != 0 && msg == g_taskbarCreatedMsg) {
        g_trayIconAdded = false;
        ApplyTrayIcon();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


static DWORD WINAPI WorkerThreadProc(LPVOID) {
    const HRESULT apartment = RoInitialize(RO_INIT_SINGLETHREADED);
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);

    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hCtrlWnd = CreateWindowExW(WS_EX_TOOLWINDOW, kCtrlWndClass,
                                  L"AnimaWindhawkEngineController", WS_POPUP,
                                  0, 0, 0, 0, nullptr, nullptr,
                                  GetModuleHandleW(nullptr), nullptr);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    OpenConfig();
    g_managerTheme=static_cast<ManagerTheme>(std::min<DWORD>(2,ConfigNumber(L"ManagerTheme",0)));
    g_accentIndex=std::min<DWORD>(kAccentIndexSystem,ConfigNumber(L"Accent",0));
    g_workshopView=static_cast<WorkshopView>(std::min<DWORD>(2,ConfigNumber(L"WorkshopLayout",0)));
    LoadHotkey();
    if (configWritable) {
        if (MissingValue(configRoot,L"ManagerHotkey")) SaveHotkey();
        if (MissingValue(configRoot,L"ManagerTheme")) SaveNumber(L"ManagerTheme",(DWORD)g_managerTheme);
        if (MissingValue(configRoot,L"Accent")) SaveNumber(L"Accent",g_accentIndex);
        if (MissingValue(configRoot,L"WorkshopLayout")) SaveNumber(L"WorkshopLayout",(DWORD)g_workshopView);
    }
    if (g_hotkeyVk)
        RegisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER, g_hotkeyMods, g_hotkeyVk);

    ReloadAll();
    OpenManagerAtSignIn();
    EnsureFFmpegOnEnable();

    if (g_threadReadyEvent) SetEvent(g_threadReadyEvent);
    if (g_hCtrlWnd) ShowReadyDialog();

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
            for (const auto& item : g_items) {
                RECT rect{};
                if (item.hwnd && GetWindowRect(item.hwnd,&rect))
                    SaveStoredPosition(item.registryKey,rect.left,rect.top);
            }
            for (HANDLE worker : workshopWorkers) {
                WaitForSingleObject(worker,INFINITE);
                CloseHandle(worker);
            }
            workshopWorkers.clear();
            MSG result{};
            while (PeekMessageW(&result,g_hCtrlWnd,WM_WS_FETCH_DONE,WM_WS_INSTALL_DONE,PM_REMOVE))
                DispatchMessageW(&result);
            for (auto& item : g_workshopItems) if (item.hPreviewBmp) DeleteObject(item.hPreviewBmp);
            if (disableProcess) { CloseHandle(disableProcess); disableProcess=nullptr; }
            StopFFmpegSetup();
            if (readyDialog) DestroyWindow(readyDialog);
            if (g_hWelcomeWnd) DestroyWindow(g_hWelcomeWnd);
            RemoveTrayIcon();
            if (g_hManagerWnd) { DestroyWindow(g_hManagerWnd); g_hManagerWnd = nullptr; }
            DestroyAllItems();
            if (g_hCtrlWnd) { DestroyWindow(g_hCtrlWnd); g_hCtrlWnd=nullptr; }
            if (managerRuntime) { managerRuntime.Close(); managerRuntime=nullptr; }
            break;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_RELOAD) {
            ReloadAll(true,msg.wParam!=0);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_RELOAD_ITEMS_ONLY) {
            ReloadAll(false);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_OPEN_MANAGER) {
            OpenPetManager((int)msg.wParam);
            continue;
        }
        if (readyDialog && (msg.hwnd==readyDialog || IsChild(readyDialog,msg.hwnd)) &&
            IsDialogMessageW(readyDialog,&msg)) continue;
        BOOL handled=FALSE;
        if (welcomeInput && g_hWelcomeWnd && (msg.hwnd==g_hWelcomeWnd || IsChild(g_hWelcomeWnd,msg.hwnd)))
            welcomeInput->PreTranslateMessage(&msg,&handled);
        if (managerInput && g_hManagerWnd && (msg.hwnd==g_hManagerWnd || IsChild(g_hManagerWnd,msg.hwnd)))
            managerInput->PreTranslateMessage(&msg,&handled);
        if (handled) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (managerManifest != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(managerManifest);
        managerManifest = INVALID_HANDLE_VALUE;
    }
    if (imagesRoot) { RegCloseKey(imagesRoot); imagesRoot=nullptr; }
    imageOrder.clear();
    if (configRoot) { RegCloseKey(configRoot); configRoot=nullptr; }
    configWritable=false;
    if (SUCCEEDED(apartment)) RoUninitialize();
    return 0;
}


BOOL WhTool_ModInit() {
    Wh_Log(L"AnimaWindhawkEngine v1.1: init");

    Gdiplus::GdiplusStartupInput gdiInput;
    if (Gdiplus::GdiplusStartup(&g_gdiToken, &gdiInput, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return FALSE;
    }

    WNDCLASSEXW wcItem = {sizeof(wcItem)};
    wcItem.lpfnWndProc   = ItemWndProc;
    wcItem.hInstance     = GetModuleHandleW(nullptr);
    wcItem.lpszClassName = kItemWndClass;
    wcItem.hCursor       = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcItem);

    WNDCLASSEXW wcCtrl = {sizeof(wcCtrl)};
    wcCtrl.lpfnWndProc   = ControllerWndProc;
    wcCtrl.hInstance     = GetModuleHandleW(nullptr);
    wcCtrl.lpszClassName = kCtrlWndClass;
    RegisterClassExW(&wcCtrl);

    WNDCLASSEXW wcMgr = {sizeof(wcMgr)};
    wcMgr.lpfnWndProc   = ManagerWndProc;
    wcMgr.hInstance     = GetModuleHandleW(nullptr);
    wcMgr.lpszClassName = kManagerWndClass;
    wcMgr.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcMgr.hCursor       = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcMgr);

    WNDCLASSEXW wcWelcome = {sizeof(wcWelcome)};
    wcWelcome.lpfnWndProc = WelcomeWndProc;
    wcWelcome.hInstance = GetModuleHandleW(nullptr);
    wcWelcome.lpszClassName = kWelcomeWndClass;
    wcWelcome.hCursor = LoadCursorW(nullptr, (PCWSTR)IDC_ARROW);
    RegisterClassExW(&wcWelcome);

    g_threadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_threadId);
    if (!g_hThread) { Wh_Log(L"Failed to start worker thread"); return FALSE; }
    WaitForSingleObject(g_threadReadyEvent, 5000);

    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hCtrlWnd) UnregisterHotKey(g_hCtrlWnd, HOTKEY_OPEN_MANAGER);
    Wh_Log(L"AnimaWindhawkEngine v1.1: uninit");

    if (g_hThread) {
        PostThreadMessageW(g_threadId, WM_APP_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    if (g_threadReadyEvent) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
    }

    UnregisterClassW(kWelcomeWndClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kItemWndClass,    GetModuleHandleW(nullptr));
    UnregisterClassW(kCtrlWndClass,    GetModuleHandleW(nullptr));
    UnregisterClassW(kManagerWndClass, GetModuleHandleW(nullptr));

    if (g_gdiToken) { Gdiplus::GdiplusShutdown(g_gdiToken); g_gdiToken = 0; }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"AnimaWindhawkEngine v1.1: settings changed");
    if (g_threadId) PostThreadMessageW(g_threadId, WM_APP_RELOAD, 1, 0);
}



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
