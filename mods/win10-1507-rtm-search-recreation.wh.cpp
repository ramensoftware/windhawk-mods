// ==WindhawkMod==
// @id              win10-1507-rtm-search-recreation
// @name            Windows 10 1507 Search Pane Recreation
// @description     This mod recreates the Windows 10 RTM (1507) Search on Windows 10 and Windows 11
// @version         1.0.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32 -luser32 -lgdi32 -lshlwapi -lshcore -lcomctl32 -lgdiplus -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 1507 Search Pane
This mod is a best-effort recreation of the Windows 10 1507 search-pane experience in a standalone, DPI-aware window. It is informed by the supplied decompiled Explorer material, but that material does not expose the native SearchUI renderer or prove pixel-perfect fidelity.

## Screenshot 

![Windows 10 Search](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/refs/heads/main/win10search.png)

## What this mod does
- This mod provides a Home surface, a local Start-menu shortcut catalog, a searchable Settings category browser, and an optional Web search action.
- This mod is search-only: it does not replace, hook, or draw the Start menu, and it does not modify Cortana or the native Windows Search index.
- The Home, AllApps, and Settings icons use Microsoft-documented Segoe MDL2 Assets glyphs (Home U+E80F, AllApps U+E71D, Settings U+E713), rendered with GDI+. App shortcut icons use documented Shell extraction and `DrawIconEx`.
- With `overTaskbar` off, this mod clips the animated window to the monitor work area so it does not paint over the taskbar. The 120 ms smoothstep animation is local behavior, not a recovered native SearchUI transition.

## Settings
- `searchHotkey`: This setting enables or disables the fallback Ctrl+Alt+S shortcut. It is enabled by default; turn it off if another application uses the same shortcut.
- `webSearch`: This setting shows or hides the Web footer action. When disabled, My stuff uses the available footer width.
- `interceptSearch`: When enabled, clicking the taskbar search button or pressing Win+S / Win+Q opens this pane instead of Windows Search.
- `overTaskbar`: When enabled and the taskbar is at the bottom, the pane extends over the taskbar row of the search box, so its text field replaces the box like in Windows 10 (the Start button stays visible). When disabled, the pane stays above the taskbar.

## Scope and limitations
- This mod is not a full Windows Search index: the app catalog comes from Start-menu `.lnk` shortcuts; UWP enumeration, files/documents indexing, native ranking, and taskbar-magnifier interception are not implemented.
- This mod targets a Windows 10 1507-inspired interface.

## Credits 
AdmnistratoX - Technical inspiration.
m417z - Technical inspiration 
Microsoft - Documentation 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- searchHotkey: true
  $name: Fallback search shortcut
  $description: This setting enables Ctrl+Alt+S to open or close this standalone search pane. Turn it off if another application uses this shortcut.
- webSearch: true
  $name: Web search footer action
  $description: This setting shows or hides the Web footer action and allows submitting typed queries to Bing.
- interceptSearch: true
  $name: Replace Windows Search
  $description: This setting makes Windows open this pane instead of Windows Search when the taskbar search button, Win+S or Win+Q is used.
- overTaskbar: true
  $name: Search bar over the taskbar
  $description: This setting makes the mod extend the pane over the taskbar row of the search box, so its text field takes the place of the box as in Windows 10 (the Start button stays visible). Only applies when the taskbar is at the bottom of the screen.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <shldisp.h>
#include <uiautomation.h>
#include <uxtheme.h>
#include <knownfolders.h>
#include <shellscalingapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <cwchar>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <memory>
#include <new>
#include <string>
#include <vector>
#include <utility>

using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t kWindowClass[] = L"Win10_1507_SearchOnlyPane";
constexpr wchar_t kLegacyTaskbarBoxClass[] = L"Win10_1507_TaskbarSearchBox";
constexpr wchar_t kTaskbarBoxClass[] = L"Win10_1507_TaskbarSearchBox_V2";
constexpr int kHotkeyId = 0x1507;
constexpr UINT_PTR kCaretTimer = 0xCA47;
constexpr UINT_PTR kSlideTimer = 0xCA48;
constexpr UINT_PTR kTaskbarSyncTimer = 0xCA49;
constexpr ULONGLONG kSlideDurationMs = 120;
constexpr UINT kSlideTimerIntervalMs = 10;
constexpr UINT kTaskbarSyncIntervalMs = 250;
constexpr UINT kRefreshHotkeyMessage = WM_APP + 0x57;
constexpr UINT kCatalogReadyMessage = WM_APP + 0x58;
constexpr UINT kToggleMessage = WM_APP + 0x59;
constexpr UINT kTaskbarBoxClickMessage = WM_APP + 0x5A;
constexpr int kPaneWidthBox96 = 392;
constexpr int kPaneWidthIcon96 = 346;
constexpr int kRailWidth96 = 48;
constexpr int kFooterHeightBox96 = 44;
constexpr int kQueryHeightIcon96 = 44;
constexpr int kFooterHeightIcon96 = 36;
constexpr int kSettingsRowHeight96 = 58;
constexpr int kMaxVisibleRows = 11;
constexpr int kSettingsCount = 9;

#ifndef TMT_BORDERCOLOR
#define TMT_BORDERCOLOR 3801
#endif
#ifndef TMT_FILLCOLOR
#define TMT_FILLCOLOR 3802
#endif
#ifndef TMT_TEXTCOLOR
#define TMT_TEXTCOLOR 3803
#endif
#ifndef CLEARTYPE_NATURAL_QUALITY
#define CLEARTYPE_NATURAL_QUALITY 6
#endif

enum class UiLabel {
    AllResults, Apps, Settings, MyStuff, Web, HomeWelcome, HomeInstruction,
    BrowseApps, BrowseSettings, SearchPlaceholder, NoResults, Open, OpenLocation, Indexing, CopyPath, Windows
};

enum class ResultKind { App, Run, Setting, ControlPanel, SettingsHome };

struct SearchEntry {
    std::wstring name;
    std::wstring shortcutPath;  // ShellExecute target; app entries are Start-menu .lnk paths.
    HICON icon = nullptr;       // Owned by the application catalog only.
    ResultKind kind = ResultKind::App;
    bool isExplorerShortcut = false;
    int settingsIndex = -1;
};

std::atomic<bool> g_searchHotkeyEnabled{true};
std::atomic<bool> g_webSearchEnabled{true};
std::atomic<DWORD> g_uiThreadId{0};
std::atomic<bool> g_stopping{false};
std::atomic<bool> g_interceptEnabled{true};
std::atomic<bool> g_overTaskbar{true};
std::atomic<HWND> g_paneWindow{nullptr};
std::atomic<HWND> g_taskbarBoxWindow{nullptr};
HHOOK g_mouseHook = nullptr;
HHOOK g_keyboardHook = nullptr;
bool g_swallowMouseUp = false;
ComPtr<IUIAutomation> g_uia;
HANDLE g_uiThread = nullptr;
HANDLE g_uiReadyEvent = nullptr;

struct FindHandle {
    HANDLE value = INVALID_HANDLE_VALUE;
    ~FindHandle() { if (value != INVALID_HANDLE_VALUE) FindClose(value); }
};

struct PidlHandle {
    PIDLIST_ABSOLUTE value = nullptr;
    ~PidlHandle() { if (value) ILFree(value); }
    PidlHandle(const PidlHandle&) = delete;
    PidlHandle& operator=(const PidlHandle&) = delete;
    PidlHandle() = default;
    explicit PidlHandle(PIDLIST_ABSOLUTE pidl) : value(pidl) {}
};

struct IconHandle {
    HICON value = nullptr;
    ~IconHandle() { if (value) DestroyIcon(value); }
    IconHandle(const IconHandle&) = delete;
    IconHandle& operator=(const IconHandle&) = delete;
    IconHandle() = default;
    HICON release() { HICON result=value; value=nullptr; return result; }
    void reset(HICON icon=nullptr) { if(value&&value!=icon)DestroyIcon(value); value=icon; }
};

struct CoInitScope {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ~CoInitScope() { if (SUCCEEDED(hr)) CoUninitialize(); }
};

std::wstring Lower(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(),
        [](wchar_t ch) { return static_cast<wchar_t>(std::towlower(ch)); });
    return text;
}

std::wstring JoinPath(const std::wstring& left, const wchar_t* right) {
    if (left.empty()) return right ? right : L"";
    std::wstring result = left;
    if (result.back() != L'\\') result.push_back(L'\\');
    if (right) result += right;
    return result;
}

std::wstring StripExtension(std::wstring name) {
    const size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) name.resize(dot);
    return name;
}

std::wstring StripLnkExtension(std::wstring name) {
    if (name.size() >= 4 && _wcsicmp(name.c_str() + name.size() - 4, L".lnk") == 0)
        name.resize(name.size() - 4);
    return name;
}

bool IsShortcut(const wchar_t* name) {
    const wchar_t* extension = PathFindExtensionW(name);
    return extension && _wcsicmp(extension, L".lnk") == 0;
}

HICON ExtractShortcutIcon(const std::wstring& shortcutPath) {
    PIDLIST_ABSOLUTE rawPidl = nullptr;
    SFGAOF attributes = 0;
    const HRESULT parseHr = SHParseDisplayName(shortcutPath.c_str(), nullptr, &rawPidl, 0, &attributes);
    PidlHandle pidl(rawPidl);
    if (FAILED(parseHr) || !pidl.value) return nullptr;

    ComPtr<IShellFolder> parentFolder;
    PCUITEMID_CHILD child = nullptr;
    HRESULT hr = SHBindToParent(pidl.value, IID_PPV_ARGS(parentFolder.GetAddressOf()), &child);
    if (FAILED(hr) || !parentFolder || !child) return nullptr;

    ComPtr<IExtractIconW> extractor;
    // Use the documented IExtractIconW IID directly: MinGW's __uuidof template
    // may be declared by the headers without a linkable specialization.
    static const IID iidExtractIconW = {
        0x000214FA, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    };
    hr = parentFolder->GetUIObjectOf(
        nullptr, 1, &child, iidExtractIconW, nullptr,
        reinterpret_cast<void**>(extractor.GetAddressOf()));
    if (FAILED(hr) || !extractor) return nullptr;

    wchar_t iconLocation[MAX_PATH]{};
    int iconIndex = 0;
    UINT flags = 0;
    hr = extractor->GetIconLocation(GIL_FORSHELL, iconLocation, ARRAYSIZE(iconLocation), &iconIndex, &flags);
    if (hr != S_OK) return nullptr;

    IconHandle largeIcon;
    IconHandle smallIcon;
    if (flags & GIL_NOTFILENAME) {
        const UINT largeSize = static_cast<UINT>((std::max)(48, GetSystemMetrics(SM_CXICON)));
        const UINT smallSize = static_cast<UINT>((std::max)(24, GetSystemMetrics(SM_CXSMICON)));
        const UINT requestedSizes = MAKELONG(largeSize, smallSize);
        hr = extractor->Extract(iconLocation, static_cast<UINT>(iconIndex),
                                &largeIcon.value, &smallIcon.value, requestedSizes);
        if (hr != S_OK) { largeIcon.reset(); smallIcon.reset(); }
    } else if (iconLocation[0]) {
        // The Shell-provided file/index pair can be handed to the documented extractor.
        ExtractIconExW(iconLocation, iconIndex, &largeIcon.value, &smallIcon.value, 1);
    }

    if (largeIcon.value && smallIcon.value == largeIcon.value) smallIcon.release();
    if (!largeIcon.value && smallIcon.value) return smallIcon.release();
    return largeIcon.release();
}

bool IsExplorerShortcut(const std::wstring& shortcutPath) {
    ComPtr<IShellLinkW> shellLink;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(shellLink.GetAddressOf()));
    if (FAILED(hr) || !shellLink) return false;
    ComPtr<IPersistFile> persistFile;
    hr = shellLink.As(&persistFile);
    if (FAILED(hr) || !persistFile) return false;
    hr = persistFile->Load(shortcutPath.c_str(), STGM_READ);
    if (FAILED(hr)) return false;
    wchar_t target[MAX_PATH]{};
    WIN32_FIND_DATAW findData{};
    hr = shellLink->GetPath(target, ARRAYSIZE(target), &findData, SLGP_RAWPATH);
    return SUCCEEDED(hr) && _wcsicmp(PathFindFileNameW(target), L"explorer.exe") == 0;
}

void AddDirectoryShortcuts(const std::wstring& directory, int depth,
                           std::vector<SearchEntry>& catalog) {
    if (g_stopping.load() || depth > 5 || directory.empty()) return;
    const std::wstring pattern = JoinPath(directory, L"*");
    WIN32_FIND_DATAW data{};
    FindHandle find{FindFirstFileW(pattern.c_str(), &data)};
    if (find.value == INVALID_HANDLE_VALUE) return;

    do {
        if (g_stopping.load()) break;
        if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0)
            continue;
        const std::wstring fullPath = JoinPath(directory, data.cFileName);
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Avoid directory junction loops and unrelated package/cache trees.
            if (!(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                AddDirectoryShortcuts(fullPath, depth + 1, catalog);
            continue;
        }
        if (!IsShortcut(data.cFileName)) continue;

        std::wstring displayName = StripExtension(data.cFileName);
        SHFILEINFOW fileInfo{};
        HICON icon = nullptr;
        // Ask the Shell for Explorer's display name (localized where the Shell provides one),
        // while requesting only the base icon so shortcut overlays stay suppressed.
        const DWORD_PTR imageList = SHGetFileInfoW(fullPath.c_str(), 0, &fileInfo, sizeof(fileInfo),
            SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_DISPLAYNAME);
        if (fileInfo.szDisplayName[0]) displayName = StripLnkExtension(fileInfo.szDisplayName);
        if (displayName.empty()) continue;
        const std::wstring key = Lower(displayName);
        const bool duplicate = std::any_of(catalog.begin(), catalog.end(),
            [&](const SearchEntry& entry) { return Lower(entry.name) == key; });
        if (duplicate) continue;
        icon = ExtractShortcutIcon(fullPath);
        if (!icon && imageList) icon = ImageList_GetIcon(reinterpret_cast<HIMAGELIST>(imageList),
                                                          fileInfo.iIcon, ILD_TRANSPARENT);
        SearchEntry entry{std::move(displayName), fullPath, icon, ResultKind::App};
        entry.isExplorerShortcut = IsExplorerShortcut(fullPath);
        catalog.push_back(std::move(entry));
    } while (FindNextFileW(find.value, &data));
}

std::wstring GetLocalizedControlPanelName() {
    PIDLIST_ABSOLUTE pidl = nullptr;
    const HRESULT hr = SHGetKnownFolderIDList(FOLDERID_ControlPanelFolder, 0, nullptr, &pidl);
    if (FAILED(hr) || !pidl) return {};
    SHFILEINFOW info{};
    const DWORD_PTR result = SHGetFileInfoW(reinterpret_cast<LPCWSTR>(pidl), 0,
        &info, sizeof(info), SHGFI_PIDL | SHGFI_DISPLAYNAME);
    ILFree(pidl);
    return result && info.szDisplayName[0] ? info.szDisplayName : L"";
}

void BuildAppCatalog(std::vector<SearchEntry>& catalog) {
    if(g_stopping.load())return;
    PWSTR rawPath = nullptr;
    const HRESULT userResult = SHGetKnownFolderPath(FOLDERID_Programs, KF_FLAG_DEFAULT, nullptr, &rawPath);
    if (SUCCEEDED(userResult) && rawPath) AddDirectoryShortcuts(rawPath, 0, catalog);
    if (rawPath) { CoTaskMemFree(rawPath); rawPath = nullptr; }
    const HRESULT commonResult = SHGetKnownFolderPath(FOLDERID_CommonPrograms, KF_FLAG_DEFAULT, nullptr, &rawPath);
    if (SUCCEEDED(commonResult) && rawPath) AddDirectoryShortcuts(rawPath, 0, catalog);
    if (rawPath) CoTaskMemFree(rawPath);
    const std::wstring controlPanelName = GetLocalizedControlPanelName();
    if (!controlPanelName.empty()) {
        auto existingControlPanel = std::find_if(catalog.begin(), catalog.end(), [&](const SearchEntry& entry) {
            return _wcsicmp(entry.name.c_str(), controlPanelName.c_str()) == 0;
        });
        if (existingControlPanel != catalog.end()) {
            existingControlPanel->kind = ResultKind::ControlPanel;
        } else {
            catalog.push_back({controlPanelName, L"control.exe", nullptr, ResultKind::ControlPanel});
        }
    }
    std::stable_sort(catalog.begin(), catalog.end(), [](const SearchEntry& a, const SearchEntry& b) {
        return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
    });
}

int MatchRank(const std::wstring& name, const std::wstring& query) {
    const std::wstring candidate = Lower(name);
    if (candidate == query) return 0;
    if (candidate.rfind(query, 0) == 0) return 1;
    const size_t pos = candidate.find(query);
    if (pos != std::wstring::npos && (pos == 0 || candidate[pos - 1] == L' ' ||
        candidate[pos - 1] == L'-' || candidate[pos - 1] == L'_')) return 2;
    return pos == std::wstring::npos ? 99 : 3;
}

struct GdiObject {
    HGDIOBJ value = nullptr;
    explicit GdiObject(HGDIOBJ object = nullptr) : value(object) {}
    ~GdiObject() { if (value) DeleteObject(value); }
    GdiObject(const GdiObject&) = delete;
    GdiObject& operator=(const GdiObject&) = delete;
};

struct MemoryDc {
    HDC value=nullptr;
    explicit MemoryDc(HDC dc=nullptr):value(dc){}
    ~MemoryDc(){if(value)DeleteDC(value);}
    MemoryDc(const MemoryDc&)=delete;
    MemoryDc& operator=(const MemoryDc&)=delete;
};
struct WindowDc {
    HWND window=nullptr;
    HDC value=nullptr;
    explicit WindowDc(HWND hwnd):window(hwnd),value(hwnd?GetDC(hwnd):nullptr){}
    ~WindowDc(){if(value)ReleaseDC(window,value);}
    WindowDc(const WindowDc&)=delete;
    WindowDc& operator=(const WindowDc&)=delete;
};
struct SelectedGdiObject {
    HDC dc=nullptr; HGDIOBJ old=nullptr;
    SelectedGdiObject(HDC target,HGDIOBJ object):dc(target){if(dc&&object)old=SelectObject(dc,object);}
    ~SelectedGdiObject(){if(dc&&old&&old!=HGDI_ERROR)SelectObject(dc,old);}
    SelectedGdiObject(const SelectedGdiObject&)=delete;
    SelectedGdiObject& operator=(const SelectedGdiObject&)=delete;
};

static constexpr char kEmbeddedWin10LogoBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAAS30lEQVR42u2dSWxd13nHf9859777Jj4OEsVBEjXLEWUnbpW0SbNR"
    "gLZAu0tbehe0RYtk5biJEztuF4/cFBmQdmF0kaxboBAXLdBlU8RBFkkKq0kdUXKlOLaVWIonyqLIN91zztfFvRwsyUNiKxHF8weo"
    "90i+R957v9/5pkudT4jaPmqrgacMnA7Mo8xj4WyF5Zbn/pXAuZYhGTRQk1GxI7i+IvVlKkHprOUkkpOaCsFO48IK//DAzyRe1e0i"
    "FdoIJxF+8L9VKkNNKtUbMN2j82wDmCb0BwzCKvXhAYOupWYyfKfDS4ljf72Oy0ewsptgRrBmmnxwNgJwZyWg60/1tka9+eWbbyhf"
    "osL8Ulp80jBMv+65UhU46Vi+1GQsVMCtsHD/gC+dP44Lh7HSwVQV3z8MyXFwx4Bx4BAwik1bZEPQWf6LCMB7XZUbhlM4g2FxEc7M"
    "BeQ2Bm9/OwHg5KvKQw/5W3+cCosYvvuDBiYzjDQCyz/p8+Qf9wF4/OlhqlXPChUMk6TZUcLgONhhCMcQGUPDDMgENh3BVsBYUAUN"
    "4HNQD955bAV8/qkIwPsRjxckMKeW2aUaKy3Px74/YOkjKde7o9iKJ5Eqtp/jsw6DrkUSg/QMJktIbM6KC+yurcFaoGMzQhgjMS28"
    "HMaaQPAtRI+i3I8wAcygTGGkgs3ApgWAqhBc8eGdIhJQFFQQAUUQFVQUmxhcBOBXcOmli26r4STC975XoTXWgKEOC3s7tJ+vsra6"
    "l6Si4K8D4LsZmgUai6+w+ska6mqY1jC23yTYEYwdAzdJYD9iDqK6F2Mm0TANNMiaghgIoVjBhYFBw6aRBQU1qBRGRt7atkqIALxt"
    "PH6LeD23aJgdFzgduIqFs9AYFG59rVJhKslYZY3QCWhzisw+AOaniPbJwz4q6RG8vw9jJwjuCNhdaJhGaFCpC5jCbuoh+NJlB0A9"
    "KjetZGQ9afjlT/WeBkDl9ga82YW3DczDAnrr61WYw/D7Zw1XblgYDwCcP+lZlCJ2t89VuNpTvnHK8cTlEUx+hLx3HzaZwPvDiLRQ"
    "vQ8xu4C92KSKTUFMYbfgtsTlAKp+4zgE2XDXb7eSf+VLdM8B0Da0T6/HY93MpEsYHvnhCJVqna+duMLDFzPG/iWn+ckar7kElzgS"
    "l9CtDpO6Hma1S/3ja/QvjOK7GWZkjTxPqegBgqaophhzDMxJgpsGmUE4gMgQQZtUagZTOIUiHvv1RwXZkvipAeGOGXlHAbCePS8t"
    "WcjrLPcDT350ZeP7n346ZZQ6vlojyZp41+Ho2qt85sM5D1/MSAYNsnQEsXVyN4yGEaw5hsh+vJvCmhlUDyI0kWSYJCuz61B8uMH6"
    "KgYNDiSA2mKp/4aMvL0B2Civ9LZuG+D84uaxjx42ACQ3Mlq7UsZHejw602PujGGxLLeeuDjOwE2Q8TqOAxg7Rp4fJkmmUX8czCga"
    "DiMyjLGjRQmVlNl16apDAHUKhDfHZDVFNJbtsaC2AJDcVZn1LUmYCu2nLJwuYjBLCSzB9EnlPGHDuG1VFiQAOZ/+RsraJ2q0n1d4"
    "wW38yNCfxphD5PIfiIwDDWpDYAyEMmqs18nBKd5puZJvZ2RbPi8Pd/s6Urkjq/cMhiUKo8ydscw+2OCFlwZkQ55TwDP1FiZZ5cnj"
    "mw0ORuHw4Q5XnlKu7dpFoxoYdDp8/cE12ueaDGoNet2cFrCaDJNkinQrhOQAwizq9gAHSGsPMOi8yNHZP+XcJcPYQFk4mfP5s7sw"
    "tQdJK/9ZxGVfrGRESxfNHU287lIPcAcyzDIef+usYaqaseqU0Am43SlpqDBUD/Ryj6GK74whporIZQ6dWOEzkhdAXDiIaAWtDQid"
    "FDUfBD2G0d2YZAbvjyIyDDqFTWsk1fJ3e0jrsPbqd/jqydO0zzVZfUlp/mGXBQl84ccfx9jvlh7H7NyK972FgLK/fZvSqY0wj+Hq"
    "WSGpZvRNhq9UsGmFZHCDodEOXDN4mlDrktg36IdRjPkgz12Y4PFLTRgcQWQSH45jeuMge0lMHZsV7hrAuiIeBwduoLjcF64ah5JC"
    "CVLTKezaDCd2qVjdqkQVSn6FGlpBNjthS/PK/LwiokVNTRmvyYFV2uearMlHSRKl88YYNhnH+KPQn8BzFGvGQCcxSZM0A6qFB14v"
    "ocjXn/sthtvMrkWkOI8ylZD1QvsWbhXORYvfAkBbi2W1fCnlSenz6I8aJC7hGh1+/1Tgu5camDVD1w84dSrnJ8+W/vaKZ2kQ6P7J"
    "bh57NvCl833EjOPCUYQpVA5BuJ9+Morx4xCmMEmrKKGSsm/ty5sTOYRc6eeb2fWtJZS9NTTLW4ehqHcJwIIE2s+PUPOzfP78ayTZ"
    "y1Qvr/HNTzi+CTx+boRgatQy5fkLAFOYZC+dN8axyQE0nUUYITCN6BhptYExpZFD4arNupGdMvCbNyhua+SbDRtteWcB+NzlGmsr"
    "VfL8PPXGNGEwQ3eiwRMXh8jzo2gyCRwnmGkIhxB2IdTJGiAWEl+u5rJOzrsBQdEt2fX6qizcczTyXQVAa8Wzaj9As/5ZBt1JjJ2G"
    "MAkmo9rccj/ZF3eggivc9cCt34WS0uWWtyfEbNp06/2VaOS7NATcP+CxpT1krU9u/OFAUHD9gO8XKxlud4PCbtg3GnfbytDWhCA5"
    "veuevOdwAy3vMxsQi5AgJKXBTbT2vQbAVQSDIGIBU5RV0cg7B4DskiF4IfZGdigASdWgxhIJ2Kke4I0qhiy2R3cqAFERgKgIQFQE"
    "ICoCEBUBiIoAREUAoiIAURGAqAhAVAQgKgIQFQGIigBERQCiIgBREYCoCEBUBCAqAhAVAYiKAERFAKIiAFERgKgIQFQEICoCEBUB"
    "uPsUt467PQBZ1d/zZzlfDoUQ46PJbwbA5aMbO3vdc/u1TcDKQJl/IePzP96PMoYGJe5Lt25tMSTpNYx2yn14741dIjZGtr1cPCzn"
    "Spb18LYfbV+GQzEgkif08iMoLfCu2OBxY6/f7Ui0R1FUi3MImYFOoL9iqaZjSKVebCStZmfviCNK8IJokuCylzC9WeojCXm3HGq0"
    "XcH2Cdkw5GstADo9CzimpmBwQ3DeUBmqbOw6viMXf/mPrcDqq6MJ1e4Qfduhe72N64eN2iBseXwvdcSdfP/68Um5rZ2oEkJCCC/Q"
    "VsPKz/rwc1jY2+GLFxQjH2CwOo+qxTDYOMff1PH/ut6/9Tqppojk2Ioi9sWEQfIKDftfLBz9t3suxYHuxmc2fwVN/5Wvnngh5gCl"
    "vnhhaDMjams5O2Bp+/rHqz3l2k8Ds3PrM4RuVfvbCZze3uf5fl2rb344L3bxVpV7vlEyd8ZyRm1sCG05f9WtW8Kq0F5KWd2VFuXT"
    "BG8qpd6u1n5b/RreH54PtKaUFSu0VizLlbwYSPUWE1A+/XTKUFIpfvfL73AM2+D83837u4mQtTyt/X3AcBLlIfHCYz88iRrHK8sv"
    "snvPBIP0GmODHjS2h4tcuSr8/GMDFsXzaU3hbJ2hxHH5Yo+Z41Vm6o6pY46nLx7HhDrd3nNcuXSDyQcTxtKd4w1Wrgqtfs70kHDl"
    "lLIgjkefPSQ89swfENIWaeUBfN8TtI8xAQ3b4+KIUQjrU7M9aT3Bda9QPfHPcKkJ6YCrrxtajT9CtAJyH0IO9LfNOb6/qXEFNGAz"
    "wQ9eLi7AYxf+jOb4IoO1YkDEtuuWrQ8G85ANw8qVH/DV2Y/ylQtDdNY8nOrRebaBCb9FpfWdYjP0nZgKaDn8A0gz6Lz25wmP/mgP"
    "eOhed7j+9u6Ti3oCFtVibrDpB5iCBQk8fuEUsI/eSg8xyQ7fG1lxPUG1l9D80Gt0zjuMFFOeZDu3AhEEC9gt9U4x4m6e7/C3//d7"
    "qFZBtRxws1PNX7SHnKaG5UvpNjf6u0RDlOAk3gvaEg7MuuHjwIgdq/gnYTsegKqvgGRoiFdjRwLgrqWgSbwUOxWArKlFMyUq5gBR"
    "EYCoCEBUBCAqAhAVAYiKAERFAKIiAFERgKgIQFQEICoCEBUBiIoAREUAoiIAURGAqAhAVAQgKgIQFQGIigBERQCiIgBREYCo7QJA"
    "f6RHoI/E/zi/MwFwvYAEH3fR3qkAjA0UYzXaP+YAUTsTgFcDKi5eip0KwNUhgWAodo8LqHoUB+qLwQsa9w64h5UUe+YCJhGSSkKS"
    "FY5BPQQPIYfgFEWLiSKyucWaqimexRJi+wLQrFt6/jnyzkMoGf0bB8BOoOE46DRi9oGMUqmtz5kpd5sMJSC+AEQkFJCoUKSUUnIR"
    "4birARijz+WwAvw3YeAwk2/w9ak1AB6+2KLJPpyfQF2Lfn8Gm8wS3FHENlCdRmSUpNLCphaxRSQJYd1zlLBoeJP3UJXCi9whzyES"
    "w9a7BuCR433gxY2vqAqXz1hm5xReGLCcP8eTJ85vfP8LF49gpIpSA46BKm7gUTeFD4cwMo3KATSMI3YPqlXSaoKxILbwGLoOiAfw"
    "RZqxBY7Ce0TP8WsBoLC63LR61gcs9mir2ZwmAnD2Kj1SfJKRJm8Q8gyMAzmLy/rUXUI3DYhPSNiL6EG8O0jeH0ZkEtXjiNRR3Q8M"
    "UanZAg6zHk42HymngG0e43sILSqEZ6TYDDvqJgDexmUWo1c2NxF8/MI+1NVoZi8xf+w1HloUZj+SsmIFs2xY1ZSaWLLQp2NfI9Fl"
    "nHsRY34GlWv4GxlSncT4KiYZI187CjIOMoOyD5EDoMOo7qbSsBizWYiEvHgeHIRQVC2IboFD37pqEcWed3FH1NsC8Euo6q+wMmzp"
    "9gKfvZRy5HfrrF5LIXE0647BaorROjd6woHWDR4pw0dbDf0fD9OsBsguQ9bjRr+Jtc9Q/8UrLHyi6EV8/XKNV/JxTD6B65xA7R7w"
    "J0D2AYcgNEDGsWlKklmkBCTk4HNT5CGlfgE0bwQWUB67uA91Y4QQYnh5LwAs3D+46Sv9d3S9uhFart30zV4BR9swp5ZFAo9KF7gM"
    "XKatZ1mQQPtchYX7c554Zg/e7gadQd04Tvfi3QFsMk0IhxBaBDeB0qSthuVLjvMXi1X/Oa2QSBUhoGpAfDFBU0ARRGUnlrPym/29"
    "G+MJ9E3AzC2aIgl9qmxVnwYu1Vnt7CKt7gZ5HR9epe77LNw/4OHvt8iahzFpH+9a2FR4/kf/w+x4YP6057OXKtT9FCEcIc2+hRgw"
    "aRkxAnhX9j1c2QzbyIfYrFjuoX6HErCJweWf2h4n9MTFccLKgK98+Drtc0XjatWl7MkM1/KE/kivGIb0lGF1/xi9geefZl/feP/D"
    "FzMa7rfxkiM6inIA9ASwC5HDqB5EzDDGtLAVNhLFjXyjLGeDDwU1t5Sz26vfse0AaH+7DFXjVVZdnWbSgYYrpoQ9p7wxUiVrNJl4"
    "7XWutoZJg/L3D7xS2qVYz3NqOfrj3dCAvCskoc+XP1iEpEd+OEK9Nklwu6EygboHMbKHoDOIHEB1H6opSVYlqZQVi9vS7yjLWRR0"
    "a1Iq3JWhZdsBsLVHMY/cOhRShTMYHhL/jj/jjFqWnhLmT/viZ5Ureqsef24Y3z2EqbyGydawvsWgexTMEQj7MJVdqDuOyATBzyDS"
    "JK1bRMrZy7rZ7/A5qNeNSkrXQ6BQhhaJALy/ZyfvWNq+1XvmFg2zH0lhLaGXHkEZ4O3rhFdu0NpXo7OW87UTNwD4u58eoN+bJgmv"
    "ItUqvnsckn3ASZTdCAeAIQj7MUmNpFpa34Bu8R4aFF0Hdz0pvcOd0nsbgPcz9LQN5+eFOWBpXln5qwxWaphqQrPXoVezcA26lSpa"
    "fYMnj/dpP19l+doQ9dokhFE01FAzgrVTeP8BTDqOuv2I7CboFEKVbKg0TCg/fJmYBlDVN1cs70OnNALwLvSlZ0bJQ0pIHFlQ+kbI"
    "1gQmA/m1AVlthGBquGQVu1YhTzoMuR5UDP2eJVQbGFfHhRXU3WBQd1TySVJZJRvusjZoQe/jEK5jmUHNFOiHQPeUPY9pkBRjEtL6"
    "JiDrSal3yvot/M1c493dod0CQBwU8VZ67tkVZseF86cVFmFuDpaeEk7OKMzAEldvyUXamnB+UVl8yN+m5wFb77lAh7kz/16+dh26"
    "w+RhFVjDpNNY+R2834d2LOqPIGYv6g+C2YNNRxEjJJnZaIBubaOrBiiP7013aLeUuEbT6AHuWM6x3gB708rb8nn5elVh/inL8l5L"
    "wzWRnqEnFao6wGd7MEmXvPsyzSRnrbIfzR029eBnQCeB+0APIvYg6nej7EVkgkodbGVLWFlPSAN4FzAJaPjrCMDdBtPnvlclG6kT"
    "khq1Y1eKoZfPDWP8XsLKS3z51Ap/88Iwydo0Ib3OP9730ka+sja3B8MMKhk2HSbks4iZIuh9iI4UrXQaiB0ia0L/xl9GAO62Mnfr"
    "3zI8/vQwplKlP/AM7XJc79ZIgmJ7Xaotpe9SQlLDhR5DrkfHCnUr9DNLfm2AWTXUR1L6QXEmw9oqXlpYfwjSg5C/8P+Hp67/r2Ga"
    "uQAAAABJRU5ErkJggg=="
    ;

IStream* g_embeddedLogoStream=nullptr;
std::unique_ptr<Gdiplus::Bitmap> g_embeddedWin10Logo;

bool DecodeBase64(const char* input,std::vector<BYTE>& output){
    unsigned accumulator=0;int bits=0;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(input);*p;++p){
        int value=-1;
        if(*p>='A'&&*p<='Z')value=*p-'A';
        else if(*p>='a'&&*p<='z')value=*p-'a'+26;
        else if(*p>='0'&&*p<='9')value=*p-'0'+52;
        else if(*p=='+')value=62;else if(*p=='/')value=63;else if(*p=='=')break;
        if(value<0)continue;
        accumulator=(accumulator<<6)|static_cast<unsigned>(value);bits+=6;
        if(bits>=8){bits-=8;output.push_back(static_cast<BYTE>((accumulator>>bits)&0xff));
            accumulator&=bits?((1u<<bits)-1u):0u;}
    }
    return !output.empty();
}

bool InitializeEmbeddedWin10Logo(){
    std::vector<BYTE> png;
    if(!DecodeBase64(kEmbeddedWin10LogoBase64,png))return false;
    HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,png.size());if(!memory)return false;
    void* target=GlobalLock(memory);if(!target){GlobalFree(memory);return false;}
    memcpy(target,png.data(),png.size());GlobalUnlock(memory);
    HRESULT hr=CreateStreamOnHGlobal(memory,TRUE,&g_embeddedLogoStream);
    if(FAILED(hr)){GlobalFree(memory);return false;}
    g_embeddedWin10Logo.reset(Gdiplus::Bitmap::FromStream(g_embeddedLogoStream,FALSE));
    if(!g_embeddedWin10Logo||g_embeddedWin10Logo->GetLastStatus()!=Gdiplus::Ok){
        g_embeddedWin10Logo.reset();g_embeddedLogoStream->Release();g_embeddedLogoStream=nullptr;return false;
    }
    return true;
}

void ShutdownEmbeddedWin10Logo(){
    g_embeddedWin10Logo.reset();
    if(g_embeddedLogoStream){g_embeddedLogoStream->Release();g_embeddedLogoStream=nullptr;}
}

bool DrawEmbeddedWin10Logo(HDC dc,int x,int y,int size){
    if(!g_embeddedWin10Logo||!dc||size<=0)return false;
    Gdiplus::Graphics graphics(dc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    return graphics.DrawImage(g_embeddedWin10Logo.get(),x,y,size,size)==Gdiplus::Ok;
}

// Windows 10's built-in Segoe MDL2 Assets font provides the documented Microsoft glyphs
// used for Home (E80F), AllApps (E71D), and Settings (E713).
bool DrawMdl2GlyphGdiPlus(HDC dc, wchar_t glyph, int x, int y, int size, COLORREF color) {
    if (!dc || size <= 0) return false;
    Gdiplus::Graphics g(dc);
    if (g.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::FontFamily family(L"Segoe MDL2 Assets");
    if (family.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::Font font(&family, static_cast<Gdiplus::REAL>(size) * .94f,
                       Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    if (font.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    const Gdiplus::Color c(255,GetRValue(color),GetGValue(color),GetBValue(color));
    Gdiplus::SolidBrush brush(c);
    const Gdiplus::RectF box(static_cast<Gdiplus::REAL>(x),static_cast<Gdiplus::REAL>(y),
                             static_cast<Gdiplus::REAL>(size),static_cast<Gdiplus::REAL>(size));
    g.DrawString(&glyph,1,&font,box,&format,&brush);
    return g.GetLastStatus() == Gdiplus::Ok;
}

void DrawVectorIconGdiPlus(HDC dc, int kind, int x, int y, int size, COLORREF color) {
    if (!dc || size <= 0) return;
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    const Gdiplus::Color c(255,GetRValue(color),GetGValue(color),GetBValue(color));
    Gdiplus::Pen pen(c,(std::max)(1.2f,size/12.0f));
    Gdiplus::SolidBrush brush(c);
    const float s=static_cast<float>(size), ox=static_cast<float>(x), oy=static_cast<float>(y);
    if(kind==5){ // rounded hamburger menu button
        pen.SetStartCap(Gdiplus::LineCapRound);pen.SetEndCap(Gdiplus::LineCapRound);
        for(int i=0;i<3;++i){const float yy=oy+s*(.25f+.25f*i);g.DrawLine(&pen,ox+s*.08f,yy,ox+s*.92f,yy);}
    } else if(kind==0){ // house
        Gdiplus::PointF roof[]={{ox+s*.15f,oy+s*.48f},{ox+s*.50f,oy+s*.17f},{ox+s*.85f,oy+s*.48f}};
        g.DrawLines(&pen,roof,3); g.DrawRectangle(&pen,ox+s*.27f,oy+s*.46f,s*.46f,s*.40f);
        g.DrawRectangle(&pen,ox+s*.44f,oy+s*.63f,s*.14f,s*.23f);
    } else if(kind==3){ // Windows 10 perspective mark, not four equal Windows 11 panes
        Gdiplus::PointF panes[4][4]={
            {{ox,oy+s*.10f},{ox+s*.43f,oy+s*.10f},{ox+s*.43f,oy+s*.47f},{ox,oy+s*.47f}},
            {{ox+s*.54f,oy+s*.10f},{ox+s,oy},{ox+s,oy+s*.47f},{ox+s*.54f,oy+s*.47f}},
            {{ox,oy+s*.55f},{ox+s*.43f,oy+s*.55f},{ox+s*.43f,oy+s*.92f},{ox,oy+s*.92f}},
            {{ox+s*.54f,oy+s*.55f},{ox+s,oy+s*.53f},{ox+s,oy+s},{ox+s*.54f,oy+s*.92f}}
        };
        for(auto& pane:panes)g.FillPolygon(&brush,pane,4);
    } else if(kind==1){ // Apps: framed application window with varied tiles.
        pen.SetLineJoin(Gdiplus::LineJoinRound);
        Gdiplus::GraphicsPath window;
        const float radius=s*.12f;
        window.AddArc(ox+s*.08f,oy+s*.08f,radius*2,radius*2,180,90);
        window.AddArc(ox+s*.92f-radius*2,oy+s*.08f,radius*2,radius*2,270,90);
        window.AddArc(ox+s*.92f-radius*2,oy+s*.92f-radius*2,radius*2,radius*2,0,90);
        window.AddArc(ox+s*.08f,oy+s*.92f-radius*2,radius*2,radius*2,90,90);
        window.CloseFigure();
        g.DrawPath(&pen,&window);
        g.DrawLine(&pen,ox+s*.10f,oy+s*.34f,ox+s*.90f,oy+s*.34f);
        g.FillEllipse(&brush,ox+s*.19f,oy+s*.18f,s*.07f,s*.07f);
        g.FillEllipse(&brush,ox+s*.32f,oy+s*.18f,s*.07f,s*.07f);
        Gdiplus::SolidBrush lightBrush(Gdiplus::Color(110,GetRValue(color),GetGValue(color),GetBValue(color)));
        g.FillRectangle(&brush,ox+s*.20f,oy+s*.46f,s*.25f,s*.18f);
        g.FillRectangle(&lightBrush,ox+s*.54f,oy+s*.46f,s*.25f,s*.18f);
        g.FillRectangle(&lightBrush,ox+s*.20f,oy+s*.70f,s*.25f,s*.14f);
        g.FillRectangle(&brush,ox+s*.54f,oy+s*.70f,s*.25f,s*.14f);
    } else if(kind==4){
        g.DrawEllipse(&pen,ox+s*.12f,oy+s*.10f,s*.52f,s*.52f);
        g.DrawLine(&pen,ox+s*.60f,oy+s*.60f,ox+s*.91f,oy+s*.91f);
    } else if(kind==9){ // Search box magnifier matching Segoe MDL2 Assets U+E721 (lens top-right, handle bottom-left)
        g.DrawEllipse(&pen,ox+s*.32f,oy+s*.08f,s*.58f,s*.58f);
        g.DrawLine(&pen,ox+s*.38f,oy+s*.62f,ox+s*.08f,oy+s*.92f);
    } else if(kind==6){ // File Explorer folder, GDI+ vector fallback for the shell shortcut.
        Gdiplus::PointF folder[]={{ox+s*.08f,oy+s*.27f},{ox+s*.40f,oy+s*.27f},
            {ox+s*.49f,oy+s*.40f},{ox+s*.92f,oy+s*.40f},
            {ox+s*.86f,oy+s*.86f},{ox+s*.10f,oy+s*.86f}};
        g.DrawPolygon(&pen,folder,ARRAYSIZE(folder));
        g.DrawLine(&pen,ox+s*.11f,oy+s*.47f,ox+s*.88f,oy+s*.47f);
    } else if(kind==7){ // Run dialog/window glyph.
        g.DrawRectangle(&pen,ox+s*.12f,oy+s*.16f,s*.76f,s*.68f);
        g.DrawLine(&pen,ox+s*.12f,oy+s*.34f,ox+s*.88f,oy+s*.34f);
        g.DrawLine(&pen,ox+s*.29f,oy+s*.49f,ox+s*.43f,oy+s*.61f);
        g.DrawLine(&pen,ox+s*.43f,oy+s*.61f,ox+s*.29f,oy+s*.72f);
        g.DrawLine(&pen,ox+s*.51f,oy+s*.72f,ox+s*.72f,oy+s*.72f);
    } else if(kind==8){ // Control Panel as a compact settings grid.
        const float a=s*.29f, gap=s*.12f, startX=ox+s*.12f,startY=oy+s*.12f;
        for(int r=0;r<2;++r)for(int col=0;col<2;++col){
            const float xx=startX+col*(a+gap), yy=startY+r*(a+gap);
            g.DrawRectangle(&pen,xx,yy,a,a);
        }
    } else {
        // Gear geometry copied from DrawPlaceGlyph(Settings) in the separate Start mod;
        // this rendition uses the search UI's blue accent on its light surface.
        Gdiplus::PointF teeth[16];
        const float cx=ox+s*.5f, cy=oy+s*.5f;
        for(int i=0;i<16;++i){
            const float angle=static_cast<float>(i)*3.14159265f/8.0f-1.5707963f;
            const float radius=(i%2==0)?s*.46f:s*.34f;
            teeth[i]={cx+std::cos(angle)*radius,cy+std::sin(angle)*radius};
        }
        Gdiplus::GraphicsPath gear;
        gear.AddPolygon(teeth,ARRAYSIZE(teeth));
        g.DrawPath(&pen,&gear);
        g.DrawEllipse(&pen,cx-s*.13f,cy-s*.13f,s*.26f,s*.26f);
    }
}

void DrawSettingsCategoryIcon(HDC dc,int item,int centerX,int centerY,int size,COLORREF color) {
    if(!dc||size<=0)return;
    Gdiplus::Graphics g(dc);
    if(g.GetLastStatus()!=Gdiplus::Ok)return;
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    const Gdiplus::Color c(255,GetRValue(color),GetGValue(color),GetBValue(color));
    Gdiplus::Pen pen(c,(std::max)(1.0f,size/14.0f));
    pen.SetStartCap(Gdiplus::LineCapRound); pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);
    Gdiplus::SolidBrush brush(c);
    const float s=static_cast<float>(size), x=static_cast<float>(centerX)-s*.5f;
    const float y=static_cast<float>(centerY)-s*.5f, cx=static_cast<float>(centerX), cy=static_cast<float>(centerY);
    auto line=[&](float x1,float y1,float x2,float y2){g.DrawLine(&pen,x1,y1,x2,y2);};
    switch(item){
    case 0: // Display: monitor and stand.
        g.DrawRectangle(&pen,x+s*.10f,y+s*.14f,s*.80f,s*.56f);
        line(cx,y+s*.70f,cx,y+s*.84f); line(x+s*.34f,y+s*.86f,x+s*.66f,y+s*.86f);
        break;
    case 1: // Bluetooth: recognizable angular rune.
        line(cx,y+s*.07f,cx,y+s*.93f);
        line(cx,y+s*.07f,x+s*.79f,y+s*.31f); line(x+s*.79f,y+s*.31f,cx,cy);
        line(cx,cy,x+s*.79f,y+s*.69f); line(x+s*.79f,y+s*.69f,cx,y+s*.93f);
        line(x+s*.22f,y+s*.29f,cx,cy); line(x+s*.22f,y+s*.71f,cx,cy);
        break;
    case 2: // Network: globe with meridians and latitude lines.
        g.DrawEllipse(&pen,x+s*.10f,y+s*.10f,s*.80f,s*.80f);
        g.DrawEllipse(&pen,x+s*.33f,y+s*.10f,s*.34f,s*.80f);
        line(x+s*.13f,cy,x+s*.87f,cy);
        g.DrawArc(&pen,x+s*.16f,y+s*.22f,s*.68f,s*.30f,0,180);
        g.DrawArc(&pen,x+s*.16f,y+s*.48f,s*.68f,s*.30f,180,180);
        break;
    case 3: { // Personalization: a clear monitor with a paintbrush over its screen.
        g.DrawRectangle(&pen,x+s*.07f,y+s*.13f,s*.59f,s*.53f);
        line(x+s*.365f,y+s*.66f,x+s*.365f,y+s*.82f);
        line(x+s*.23f,y+s*.85f,x+s*.50f,y+s*.85f);
        Gdiplus::Pen brushPen(c,(std::max)(1.2f,size/12.0f));
        brushPen.SetStartCap(Gdiplus::LineCapRound); brushPen.SetEndCap(Gdiplus::LineCapRound);
        brushPen.SetWidth((std::max)(1.3f,size/11.0f));
        g.DrawLine(&brushPen,x+s*.55f,y+s*.77f,x+s*.82f,y+s*.34f);
        Gdiplus::PointF bristles[]={{x+s*.77f,y+s*.29f},{x+s*.88f,y+s*.35f},{x+s*.82f,y+s*.44f}};
        g.DrawPolygon(&brushPen,bristles,ARRAYSIZE(bristles));
        line(x+s*.80f,y+s*.18f,x+s*.80f,y+s*.10f);
        line(x+s*.91f,y+s*.23f,x+s*.98f,y+s*.20f);
        break;
    }
    case 4: // Apps: four app tiles.
        for(int r=0;r<2;++r)for(int col=0;col<2;++col)
            g.FillRectangle(&brush,x+s*(.12f+col*.42f),y+s*(.12f+r*.42f),s*.32f,s*.32f);
        break;
    case 5: // Date & time: clock face and hands.
        g.DrawEllipse(&pen,x+s*.10f,y+s*.10f,s*.80f,s*.80f);
        line(cx,y+s*.23f,cx,cy); line(cx,cy,x+s*.72f,y+s*.61f);
        break;
    case 6: // Accessibility: person with open arms and legs.
        g.DrawEllipse(&pen,x+s*.10f,y+s*.10f,s*.80f,s*.80f);
        g.FillEllipse(&brush,x+s*.43f,y+s*.23f,s*.14f,s*.14f);
        line(cx,y+s*.39f,cx,y+s*.64f);
        line(x+s*.26f,y+s*.46f,x+s*.74f,y+s*.46f);
        line(cx,y+s*.64f,x+s*.31f,y+s*.82f); line(cx,y+s*.64f,x+s*.69f,y+s*.82f);
        break;
    case 7: { // Privacy: shield and check.
        Gdiplus::PointF shield[]={{cx,y+s*.08f},{x+s*.84f,y+s*.23f},{x+s*.80f,y+s*.60f},
            {cx,y+s*.91f},{x+s*.20f,y+s*.60f},{x+s*.16f,y+s*.23f}};
        g.DrawPolygon(&pen,shield,ARRAYSIZE(shield));
        line(x+s*.32f,y+s*.50f,x+s*.46f,y+s*.64f); line(x+s*.46f,y+s*.64f,x+s*.70f,y+s*.36f);
        break;
    }
    case 8: // Windows Update: two circulating arrows.
        g.DrawArc(&pen,x+s*.17f,y+s*.17f,s*.66f,s*.66f,205,125);
        g.DrawArc(&pen,x+s*.17f,y+s*.17f,s*.66f,s*.66f,25,125);
        { Gdiplus::PointF a[]={{x+s*.12f,y+s*.24f},{x+s*.34f,y+s*.20f},{x+s*.22f,y+s*.39f}};
          Gdiplus::PointF b[]={{x+s*.88f,y+s*.76f},{x+s*.66f,y+s*.80f},{x+s*.78f,y+s*.61f}};
          g.FillPolygon(&brush,a,3); g.FillPolygon(&brush,b,3); }
        break;
    default: { // Search-pane Settings gear, same silhouette as the separate Start mod.
        Gdiplus::PointF teeth[16];
        for(int i=0;i<16;++i){
            const float angle=static_cast<float>(i)*3.14159265f/8.0f-1.5707963f;
            const float radius=(i%2==0)?s*.46f:s*.34f;
            teeth[i]={cx+std::cos(angle)*radius,cy+std::sin(angle)*radius};
        }
        Gdiplus::GraphicsPath gear; gear.AddPolygon(teeth,ARRAYSIZE(teeth));
        g.DrawPath(&pen,&gear); g.DrawEllipse(&pen,cx-s*.13f,cy-s*.13f,s*.26f,s*.26f);
        break;
    }
    }
}

UINT EffectiveDpiForWindow(HWND hwnd, HMONITOR monitor) {
    if (!monitor && hwnd) {
        monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    }
    UINT dpiX = 96, dpiY = 96;
    if (monitor && SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX)
        return dpiX;
    using GetDpiForWindowFn = UINT (WINAPI*)(HWND);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getDpiForWindow = user32 ? reinterpret_cast<GetDpiForWindowFn>(
        GetProcAddress(user32, "GetDpiForWindow")) : nullptr;
    if (hwnd && getDpiForWindow) {
        const UINT dpi = getDpiForWindow(hwnd);
        if (dpi) return dpi;
    }
    WindowDc dc(hwnd);
    if (dc.value) {
        const int dpi = GetDeviceCaps(dc.value, LOGPIXELSX);
        if (dpi > 0) return static_cast<UINT>(dpi);
    }
    return 96;
}

// The click/hotkey that opens the pane is swallowed by a low-level hook, so Windows may not
// grant this process the right to take the foreground. Sharing input state with the current
// foreground thread (documented AttachThreadInput) makes activation reliable, so typed keys
// reach this pane instead of the taskbar or Windows Search.
bool ForceForeground(HWND hwnd) {
    if (!hwnd) return false;
    const HWND foreground = GetForegroundWindow();
    const DWORD foregroundThread = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    const DWORD thisThread = GetCurrentThreadId();
    const bool attached = foregroundThread && foregroundThread != thisThread &&
                          AttachThreadInput(thisThread, foregroundThread, TRUE);
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
    SetFocus(hwnd);
    if (attached) AttachThreadInput(thisThread, foregroundThread, FALSE);
    return GetForegroundWindow() == hwnd;
}


struct SearchBarTheme {
    COLORREF bgColor = RGB(255, 255, 255);
    COLORREF borderColor = RGB(204, 204, 204);
    COLORREF iconColor = RGB(33, 33, 33);
    COLORREF placeholderColor = RGB(45, 45, 45);
    COLORREF textColor = RGB(0, 0, 0);
    COLORREF caretColor = RGB(0, 0, 0);
    COLORREF selectionBg = RGB(0, 120, 215);
    COLORREF selectionText = RGB(255, 255, 255);
};

COLORREF BlendColor(COLORREF a, COLORREF b, int weightA, int weightB) {
    const int total = (std::max)(1, weightA + weightB);
    const int r = (GetRValue(a) * weightA + GetRValue(b) * weightB) / total;
    const int g = (GetGValue(a) * weightA + GetGValue(b) * weightB) / total;
    const int bl = (GetBValue(a) * weightA + GetBValue(b) * weightB) / total;
    return RGB(r, g, bl);
}

// Queries documented Windows theme / system color APIs (SystemParametersInfoW for High Contrast,
// UxTheme OpenThemeData/GetThemeColor, Personalize Light/Dark registry settings, and GetSysColor).
SearchBarTheme QuerySearchBarTheme(HWND hwnd, bool active, bool hovered) {
    SearchBarTheme theme{};
    try {
        theme.selectionBg = GetSysColor(COLOR_HIGHLIGHT);
        theme.selectionText = GetSysColor(COLOR_HIGHLIGHTTEXT);

        HIGHCONTRASTW hc{};
        hc.cbSize = sizeof(hc);
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
            (hc.dwFlags & HCF_HIGHCONTRASTON)) {
            theme.bgColor = GetSysColor(COLOR_WINDOW);
            theme.borderColor = GetSysColor((active || hovered) ? COLOR_HIGHLIGHT : COLOR_WINDOWFRAME);
            theme.iconColor = GetSysColor(COLOR_WINDOWTEXT);
            theme.placeholderColor = GetSysColor(COLOR_GRAYTEXT);
            theme.textColor = GetSysColor(COLOR_WINDOWTEXT);
            theme.caretColor = GetSysColor(COLOR_WINDOWTEXT);
            return theme;
        }

        DWORD appsLight = 1, systemLight = 1, size = sizeof(DWORD);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &appsLight, &size);
        size = sizeof(DWORD);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &systemLight, &size);

        const bool darkMode = (appsLight == 0 && systemLight == 0);
        if (darkMode) {
            const COLORREF sysHighlight = GetSysColor(COLOR_HIGHLIGHT);
            theme.bgColor = active ? RGB(32, 32, 32) : (hovered ? RGB(68, 68, 68) : RGB(51, 51, 51));
            theme.borderColor = active ? sysHighlight : (hovered ? RGB(145, 145, 145) : RGB(110, 110, 110));
            theme.iconColor = RGB(240, 240, 240);
            theme.placeholderColor = RGB(205, 205, 205);
            theme.textColor = RGB(255, 255, 255);
            theme.caretColor = RGB(255, 255, 255);
            return theme;
        }

        COLORREF winBg = GetSysColor(COLOR_WINDOW);
        COLORREF winText = GetSysColor(COLOR_WINDOWTEXT);
        COLORREF grayText = GetSysColor(COLOR_GRAYTEXT);
        COLORREF shadow3d = GetSysColor(COLOR_3DSHADOW);
        COLORREF light3d = GetSysColor(COLOR_3DLIGHT);

        if (HTHEME hTheme = OpenThemeData(hwnd, L"Edit")) {
            COLORREF themedFill = 0, themedText = 0, themedBorder = 0;
            if (SUCCEEDED(GetThemeColor(hTheme, 0, 0, TMT_FILLCOLOR, &themedFill)))
                winBg = themedFill;
            if (SUCCEEDED(GetThemeColor(hTheme, 0, 0, TMT_TEXTCOLOR, &themedText)))
                winText = themedText;
            if (SUCCEEDED(GetThemeColor(hTheme, 0, 0, TMT_BORDERCOLOR, &themedBorder)))
                light3d = themedBorder;
            CloseThemeData(hTheme);
        }

        theme.bgColor = winBg;
        // On Windows 10, COLOR_3DSHADOW is RGB(160,160,160). Blending winBg (255) and shadow3d (160)
        // with weights (4, 5) produces ~RGB(202,202,202) for the idle border, and (1, 3) produces
        // ~RGB(184,184,184) when active or hovered so the top border stays clearly visible.
        const COLORREF normalBorder = BlendColor(winBg, shadow3d, 4, 5);
        const COLORREF activeOrHoverBorder = BlendColor(winBg, shadow3d, 1, 3);
        theme.borderColor = (active || hovered) ? activeOrHoverBorder : normalBorder;
        theme.iconColor = BlendColor(winText, winBg, 7, 1);           // ~RGB(33,33,33) on white
        theme.placeholderColor = BlendColor(winText, grayText, 2, 1); // ~RGB(36,36,36) on white
        theme.textColor = winText;
        theme.caretColor = winText;
        (void)light3d;
    } catch (const std::exception&) {
        Wh_Log(L"[SearchOnly] exception in QuerySearchBarTheme");
    } catch (...) {
        Wh_Log(L"[SearchOnly] unknown exception in QuerySearchBarTheme");
    }
    return theme;
}

bool HasVisibleChars(const std::wstring& s) {
    for (wchar_t c : s) {
        if (!iswspace(c) && c != L'\0') return true;
    }
    return false;
}

std::wstring ReadWindowTextSafe(HWND hwnd) {
    try {
        if (!hwnd || !IsWindow(hwnd)) return {};
        wchar_t buf[256]{};
        int len = InternalGetWindowText(hwnd, buf, ARRAYSIZE(buf));
        if (len <= 0) {
            DWORD_PTR res = 0;
            if (SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(buf),
                                    reinterpret_cast<LPARAM>(buf),
                                    SMTO_ABORTIFHUNG, 40, &res) && res > 0) {
                len = static_cast<int>(res);
            }
        }
        if (len <= 0) return {};
        buf[ARRAYSIZE(buf) - 1] = L'\0';
        std::wstring result(buf);
        return HasVisibleChars(result) ? result : std::wstring{};
    } catch (...) {
        return {};
    }
}

struct TaskbarSearchScan {
    HWND trayHwnd = nullptr;
    HWND dummyHwnd = nullptr;
    HWND button4101Hwnd = nullptr;
    HWND button4103Hwnd = nullptr;
    std::wstring childPlaceholder;
};

BOOL CALLBACK ScanTrayChildrenProc(HWND hwnd, LPARAM lParam) {
    try {
        auto* scan = reinterpret_cast<TaskbarSearchScan*>(lParam);
        if (!scan) return FALSE;
        wchar_t cls[64]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        const int id = GetDlgCtrlID(hwnd);
        if (_wcsicmp(cls, L"TrayDummySearchControl") == 0 || id == 4100) {
            scan->dummyHwnd = hwnd;
            if (scan->childPlaceholder.empty()) {
                std::wstring text = ReadWindowTextSafe(hwnd);
                if (!text.empty()) scan->childPlaceholder = std::move(text);
            }
        } else if (id == 4101) {
            scan->button4101Hwnd = hwnd;
            std::wstring text = ReadWindowTextSafe(hwnd);
            if (!text.empty()) scan->childPlaceholder = std::move(text);
        } else if (id == 4103) {
            scan->button4103Hwnd = hwnd;
        } else if (scan->dummyHwnd && IsChild(scan->dummyHwnd, hwnd) && scan->childPlaceholder.empty()) {
            std::wstring text = ReadWindowTextSafe(hwnd);
            if (!text.empty()) scan->childPlaceholder = std::move(text);
        }
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in ScanTrayChildrenProc");
    }
    return TRUE;
}

HWND FindTrayForMonitor(HMONITOR targetMonitor) {
    HWND primaryTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primaryTray && targetMonitor) {
        if (MonitorFromWindow(primaryTray, MONITOR_DEFAULTTONEAREST) == targetMonitor)
            return primaryTray;
        HWND secondary = nullptr;
        while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
            if (MonitorFromWindow(secondary, MONITOR_DEFAULTTONEAREST) == targetMonitor)
                return secondary;
        }
    }
    return primaryTray;
}

std::wstring QueryNativeSearchPlaceholderFromSystem(const TaskbarSearchScan& scan) {
    try {
        if (!scan.childPlaceholder.empty()) return scan.childPlaceholder;

        // Query UI Automation on the taskbar's 4101/4100 search box control.
        if (g_uia || SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                                IID_PPV_ARGS(g_uia.GetAddressOf())))) {
            HWND targetHwnd = scan.button4101Hwnd ? scan.button4101Hwnd : scan.dummyHwnd;
            if (targetHwnd) {
                ComPtr<IUIAutomationElement> el;
                if (SUCCEEDED(g_uia->ElementFromHandle(targetHwnd, el.GetAddressOf())) && el) {
                    BSTR name = nullptr;
                    if (SUCCEEDED(el->get_CurrentName(&name)) && name && *name) {
                        std::wstring result(name);
                        SysFreeString(name);
                        return result;
                    }
                    if (name) SysFreeString(name);
                }
            }
            if (scan.trayHwnd) {
                ComPtr<IUIAutomationElement> root;
                if (SUCCEEDED(g_uia->ElementFromHandle(scan.trayHwnd, root.GetAddressOf())) && root) {
                    VARIANT val;
                    VariantInit(&val);
                    val.vt = VT_BSTR;
                    val.bstrVal = SysAllocString(L"4101");
                    ComPtr<IUIAutomationCondition> cond;
                    if (SUCCEEDED(g_uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, val, cond.GetAddressOf())) && cond) {
                        ComPtr<IUIAutomationElement> el4101;
                        if (SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond.Get(), el4101.GetAddressOf())) && el4101) {
                            BSTR name = nullptr;
                            if (SUCCEEDED(el4101->get_CurrentName(&name)) && name && *name) {
                                std::wstring result(name);
                                SysFreeString(name);
                                VariantClear(&val);
                                return result;
                            }
                            if (name) SysFreeString(name);
                        }
                    }
                    VariantClear(&val);
                }
            }
        }

        // Check Windows Search registry hint values if present.
        static const wchar_t* const kSubKeys[] = {
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Search\\Flighting\\0\\SearchBoxText",
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Search\\Flighting\\1\\SearchBoxText",
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Search"
        };
        static const wchar_t* const kValNames[] = { L"Value", L"Value", L"SearchboxText" };
        for (size_t i = 0; i < ARRAYSIZE(kSubKeys); ++i) {
            wchar_t buf[256]{};
            DWORD bytes = sizeof(buf);
            if (RegGetValueW(HKEY_CURRENT_USER, kSubKeys[i], kValNames[i],
                             RRF_RT_REG_SZ, nullptr, buf, &bytes) == ERROR_SUCCESS && buf[0]) {
                return std::wstring(buf);
            }
        }

        // Check explorer.exe's own localized string resources via documented LoadStringW.
        HMODULE hExplorer = GetModuleHandleW(nullptr);
        static const UINT kCandidateIds[] = { 11330, 11331, 11329, 11231, 11232, 11220, 5131 };
        for (UINT id : kCandidateIds) {
            wchar_t buf[256]{};
            if (LoadStringW(hExplorer, id, buf, ARRAYSIZE(buf)) > 0 && buf[0]) {
                return std::wstring(buf);
            }
        }
    } catch (const std::bad_alloc&) {
        Wh_Log(L"[SearchOnly] allocation failure in QueryNativeSearchPlaceholderFromSystem");
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in QueryNativeSearchPlaceholderFromSystem");
    }
    return {};
}

struct TaskbarSearchState {
    HWND trayHwnd = nullptr;
    bool isWideBox = false;
    bool hasBoxRect = false;
    RECT boxRect{};
    std::wstring nativePlaceholder;
};

// Per Microsoft Win32 documentation (GetGUIThreadInfo / FindWindowExW / #32768),
// check whether any context menu or shell flyout is active so we never alter topmost
// Z-order while a menu is open on screen.
bool IsAnyPopupMenuOrFlyoutActive(HWND trayHwnd) {
    try {
        if (g_stopping.load()) return true;
        if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) return true;
        GUITHREADINFO gti{};
        gti.cbSize = sizeof(gti);
        if (GetGUIThreadInfo(0, &gti)) {
            if ((gti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE)) != 0 ||
                gti.hwndMenuOwner != nullptr) {
                return true;
            }
        }
        if (trayHwnd && IsWindow(trayHwnd)) {
            const DWORD trayTid = GetWindowThreadProcessId(trayHwnd, nullptr);
            GUITHREADINFO trayGti{};
            trayGti.cbSize = sizeof(trayGti);
            if (trayTid && GetGUIThreadInfo(trayTid, &trayGti)) {
                if ((trayGti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE)) != 0 ||
                    trayGti.hwndMenuOwner != nullptr) {
                    return true;
                }
            }
        }
        HWND menuHwnd = nullptr;
        while ((menuHwnd = FindWindowExW(nullptr, menuHwnd, L"#32768", nullptr)) != nullptr) {
            if (IsWindowVisible(menuHwnd)) return true;
        }
        if (HWND fg = GetForegroundWindow()) {
            wchar_t cls[64]{};
            if (GetClassNameW(fg, cls, ARRAYSIZE(cls)) > 0) {
                if (wcscmp(cls, L"#32768") == 0 ||
                    wcscmp(cls, L"Xaml_WindowedPopupClass") == 0 ||
                    wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
                    wcscmp(cls, L"LauncherTipWnd") == 0 ||
                    wcscmp(cls, L"NotifyIconOverflowWindow") == 0 ||
                    wcscmp(cls, L"TaskListThumbnailWnd") == 0 ||
                    wcscmp(cls, L"MultitaskingViewFrame") == 0) {
                    return true;
                }
            }
        }
    } catch (...) {
    }
    return false;
}

// Walks upward in Z-order (GW_HWNDPREV) from targetHwnd to check whether testHwnd is
// already above targetHwnd without calling SetWindowPos unnecessarily.
bool IsWindowAboveInZOrder(HWND testHwnd, HWND targetHwnd) {
    if (!testHwnd || !targetHwnd || !IsWindow(testHwnd) || !IsWindow(targetHwnd)) return false;
    int steps = 0;
    for (HWND cur = GetWindow(targetHwnd, GW_HWNDPREV); cur && steps < 256;
         cur = GetWindow(cur, GW_HWNDPREV), ++steps) {
        if (cur == testHwnd) return true;
    }
    return false;
}

TaskbarSearchState InspectTaskbarSearch(HMONITOR targetMonitor, bool needPlaceholder = true) {
    TaskbarSearchState state{};
    if (g_stopping.load()) return state;
    try {
        HWND tray = FindTrayForMonitor(targetMonitor);
        state.trayHwnd = tray;

        TaskbarSearchScan scan{};
        scan.trayHwnd = tray;
        if (tray && IsWindow(tray)) {
            EnumChildWindows(tray, ScanTrayChildrenProc, reinterpret_cast<LPARAM>(&scan));
        }
        if (needPlaceholder) {
            state.nativePlaceholder = QueryNativeSearchPlaceholderFromSystem(scan);
        } else if (!scan.childPlaceholder.empty()) {
            state.nativePlaceholder = scan.childPlaceholder;
        }

        int regMode = -1;
        DWORD modeVal = 0, modeSize = sizeof(modeVal);
        if (RegGetValueW(HKEY_CURRENT_USER,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Search",
                         L"SearchboxTaskbarMode", RRF_RT_REG_DWORD, nullptr,
                         &modeVal, &modeSize) == ERROR_SUCCESS) {
            regMode = static_cast<int>(modeVal);
        }

        if (tray && IsWindow(tray)) {
            RECT trayRect{};
            if (GetWindowRect(tray, &trayRect)) {
                const int trayW = trayRect.right - trayRect.left;
                const int trayH = trayRect.bottom - trayRect.top;
                if (trayW > 0 && trayH > 0 && trayW >= trayH) {
                    auto checkWideHwnd = [&](HWND h) -> bool {
                        if (!h || !IsWindowVisible(h)) return false;
                        RECT r{};
                        if (!GetWindowRect(h, &r)) return false;
                        const int w = r.right - r.left;
                        const int hgt = r.bottom - r.top;
                        if (w > trayH * 2 && hgt > 0) {
                            state.isWideBox = true;
                            state.hasBoxRect = true;
                            state.boxRect = r;
                            return true;
                        }
                        return false;
                    };

                    if (!checkWideHwnd(scan.dummyHwnd) && !checkWideHwnd(scan.button4101Hwnd)) {
                        if (scan.button4103Hwnd && IsWindowVisible(scan.button4103Hwnd)) {
                            RECT r{};
                            if (GetWindowRect(scan.button4103Hwnd, &r)) {
                                const int w = r.right - r.left;
                                if (w > trayH * 2) {
                                    state.isWideBox = true;
                                    state.hasBoxRect = true;
                                    state.boxRect = r;
                                } else if (w > 0) {
                                    state.isWideBox = false;
                                }
                            }
                        } else if (regMode == 2 || regMode == 3) {
                            // Registry indicates wide search box mode on horizontal taskbar.
                            if (g_uia || SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                                                    CLSCTX_INPROC_SERVER,
                                                                    IID_PPV_ARGS(g_uia.GetAddressOf())))) {
                                ComPtr<IUIAutomationElement> root;
                                if (SUCCEEDED(g_uia->ElementFromHandle(tray, root.GetAddressOf())) && root) {
                                    static const wchar_t* const kIds[] = { L"4100", L"4101", L"4103" };
                                    for (const wchar_t* idStr : kIds) {
                                        VARIANT val;
                                        VariantInit(&val);
                                        val.vt = VT_BSTR;
                                        val.bstrVal = SysAllocString(idStr);
                                        ComPtr<IUIAutomationCondition> cond;
                                        if (SUCCEEDED(g_uia->CreatePropertyCondition(
                                                UIA_AutomationIdPropertyId, val, cond.GetAddressOf())) && cond) {
                                            ComPtr<IUIAutomationElement> found;
                                            if (SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond.Get(),
                                                                          found.GetAddressOf())) && found) {
                                                BOOL off = FALSE;
                                                RECT r{};
                                                found->get_CurrentIsOffscreen(&off);
                                                found->get_CurrentBoundingRectangle(&r);
                                                if (!off && (r.right - r.left) > trayH * 2) {
                                                    state.isWideBox = true;
                                                    state.hasBoxRect = true;
                                                    state.boxRect = r;
                                                    VariantClear(&val);
                                                    break;
                                                }
                                            }
                                        }
                                        VariantClear(&val);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::bad_alloc&) {
        Wh_Log(L"[SearchOnly] allocation failure in InspectTaskbarSearch");
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in InspectTaskbarSearch");
    }
    return state;
}

// Draws the documented Windows 10 Search glyph (U+E721 in Segoe MDL2 Assets) at full
// 1:1 em height (16epx = 20x20px at 120 DPI) without the 0.94f shrinkage used on rail icons,
// and without Gdiplus::StringFormat::GenericTypographic() (which allocates a static DLL buffer).
bool DrawSearchBoxMagnifierIcon(HDC dc, int x, int y, int size, COLORREF color) {
    if (!dc || size <= 0) return false;
    Gdiplus::Graphics g(dc);
    if (g.GetLastStatus() == Gdiplus::Ok) {
        Gdiplus::FontFamily family(L"Segoe MDL2 Assets");
        if (family.GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Font font(&family, static_cast<Gdiplus::REAL>(size),
                               Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            if (font.GetLastStatus() == Gdiplus::Ok) {
                Gdiplus::StringFormat format;
                format.SetAlignment(Gdiplus::StringAlignmentCenter);
                format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap |
                                      Gdiplus::StringFormatFlagsNoClip);
                g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
                const Gdiplus::Color c(255, GetRValue(color), GetGValue(color), GetBValue(color));
                Gdiplus::SolidBrush brush(c);
                const wchar_t glyph = L'\xE721';
                const Gdiplus::RectF box(static_cast<Gdiplus::REAL>(x),
                                         static_cast<Gdiplus::REAL>(y),
                                         static_cast<Gdiplus::REAL>(size),
                                         static_cast<Gdiplus::REAL>(size));
                if (g.DrawString(&glyph, 1, &font, box, &format, &brush) == Gdiplus::Ok)
                    return true;
            }
        }
    }
    return false;
}

// Draws the wide taskbar search box using documented theme/system colors,
// the documented Segoe MDL2 Assets Search icon (U+E721 at 16epx), and the native Windows
// search box typography (Segoe UI at 15epx, per Microsoft's Windows 10 type ramp).
void DrawThemedSearchBoxBar(HDC dc, HWND hwnd, const RECT& boxRc, int dpi,
                            const std::wstring& query, const std::wstring& placeholder,
                            bool active, bool hovered, bool caretVisible,
                            size_t caretPos, bool hasSelection,
                            size_t selectionAnchor, size_t selectionEnd) {
    try {
        if (dpi <= 0) {
            dpi = static_cast<int>(EffectiveDpiForWindow(hwnd, nullptr));
        }
        const int boxH = boxRc.bottom - boxRc.top;
        if (boxH >= 46 && boxH <= 52 && dpi < 120) dpi = 120;
        else if (boxH >= 56 && boxH <= 64 && dpi < 144) dpi = 144;
        else if (boxH >= 76 && dpi < 192) dpi = 192;
        const auto scale = [dpi](int v) { return MulDiv(v, dpi, 96); };
        const SearchBarTheme theme = QuerySearchBarTheme(hwnd, active, hovered);

        GdiObject bgBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(theme.bgColor)));
        GdiObject borderBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(theme.borderColor)));
        if (bgBrush.value) FillRect(dc, &boxRc, reinterpret_cast<HBRUSH>(bgBrush.value));
        if (borderBrush.value) FrameRect(dc, &boxRc, reinterpret_cast<HBRUSH>(borderBrush.value));

        // Ensure the top border separating the footer from the search box is always clearly visible.
        if (active) {
            const COLORREF topLineColor = BlendColor(theme.borderColor, GetSysColor(COLOR_3DSHADOW), 1, 1);
            GdiObject topPen(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID, 1, topLineColor)));
            SelectedGdiObject selTopPen(dc, topPen.value);
            if (topPen.value) {
                MoveToEx(dc, boxRc.left, boxRc.top, nullptr);
                LineTo(dc, boxRc.right, boxRc.top);
            }
        }

        // Documented Windows 10 taskbar search box icon metrics: 16epx icon at left + 12epx.
        const int iconSize = scale(16);
        const int iconX = boxRc.left + scale(12);
        const int iconY = boxRc.top + ((boxRc.bottom - boxRc.top) - iconSize) / 2;
        if (!DrawSearchBoxMagnifierIcon(dc, iconX, iconY, iconSize, theme.iconColor))
            DrawVectorIconGdiPlus(dc, 9, iconX, iconY, iconSize, theme.iconColor);

        // Documented Windows 10 Body / SearchBox font size: 15epx Segoe UI Regular.
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(
            -scale(15), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI")));
        SelectedGdiObject selectedFont(dc, font.value);
        SetBkMode(dc, TRANSPARENT);

        RECT queryText{boxRc.left + scale(40), boxRc.top + 1,
                       boxRc.right - scale(8), boxRc.bottom};
        if (query.empty()) {
            SetTextColor(dc, theme.placeholderColor);
            DrawTextW(dc, placeholder.c_str(), -1, &queryText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            if (active && caretVisible) {
                GdiObject caret(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID, 1, theme.caretColor)));
                SelectedGdiObject selectedCaret(dc, caret.value);
                if (caret.value) {
                    MoveToEx(dc, queryText.left, queryText.top + scale(8), nullptr);
                    LineTo(dc, queryText.left, queryText.bottom - scale(8));
                }
            }
        } else {
            SetTextColor(dc, theme.textColor);
            DrawTextW(dc, query.c_str(), -1, &queryText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            SIZE before{};
            GetTextExtentPoint32W(dc, query.c_str(),
                                  static_cast<int>((std::min)(caretPos, query.size())), &before);
            if (active && hasSelection) {
                const size_t a = (std::min)(selectionAnchor, selectionEnd);
                const size_t b = (std::max)(selectionAnchor, selectionEnd);
                SIZE sa{}, sb{};
                GetTextExtentPoint32W(dc, query.c_str(), static_cast<int>(a), &sa);
                GetTextExtentPoint32W(dc, query.c_str(), static_cast<int>(b), &sb);
                RECT selection{queryText.left + sa.cx, queryText.top + scale(4),
                               queryText.left + sb.cx, queryText.bottom - scale(4)};
                GdiObject selectionBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(theme.selectionBg)));
                if (selectionBrush.value)
                    FillRect(dc, &selection, reinterpret_cast<HBRUSH>(selectionBrush.value));
                RECT selectedText{selection.left, queryText.top, selection.right, queryText.bottom};
                SetTextColor(dc, theme.selectionText);
                DrawTextW(dc, query.substr(a, b - a).c_str(), static_cast<int>(b - a),
                          &selectedText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                SetTextColor(dc, theme.textColor);
            }
            if (active && caretVisible && !hasSelection) {
                const int caretX = (std::min)(
                    static_cast<int>(queryText.right) - scale(1),
                    static_cast<int>(queryText.left) + static_cast<int>(before.cx));
                GdiObject caret(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID, 1, theme.caretColor)));
                SelectedGdiObject selectedCaret(dc, caret.value);
                if (caret.value) {
                    MoveToEx(dc, caretX, queryText.top + scale(8), nullptr);
                    LineTo(dc, caretX, queryText.bottom - scale(8));
                }
            }
        }
    } catch (const std::bad_alloc&) {
        Wh_Log(L"[SearchOnly] allocation failure in DrawThemedSearchBoxBar");
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in DrawThemedSearchBoxBar");
    }
}

class SearchPane {
public:
    ~SearchPane() { Destroy(); }

    bool Create(HINSTANCE instance) {
        instance_ = instance;
        UnregisterClassW(kWindowClass, instance_);
        UnregisterClassW(kLegacyTaskbarBoxClass, instance_);
        UnregisterClassW(kTaskbarBoxClass, instance_);

        WNDCLASSW wc{};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = instance_;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kWindowClass;
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

        WNDCLASSW boxWc{};
        boxWc.lpfnWndProc = TaskbarBoxWndProc;
        boxWc.hInstance = instance_;
        boxWc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        boxWc.lpszClassName = kTaskbarBoxClass;
        if (!RegisterClassW(&boxWc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"[SearchOnly] taskbar box class registration failed: %u", GetLastError());
        }

        hwnd_ = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kWindowClass, L"Search", WS_POPUP | WS_CLIPCHILDREN,
            0, 0, 1, 1, nullptr, nullptr, instance_, this);
        if (!hwnd_) return false;
        catalogWindow_=hwnd_;
        g_paneWindow=hwnd_;

        taskbarBoxHwnd_ = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kTaskbarBoxClass, L"", WS_POPUP | WS_CLIPCHILDREN,
            0, 0, 1, 1, nullptr, nullptr, instance_, this);
        if (taskbarBoxHwnd_) {
            g_taskbarBoxWindow = taskbarBoxHwnd_;
        }

        UpdateHotkey();
        SetTimer(hwnd_, kTaskbarSyncTimer, kTaskbarSyncIntervalMs, nullptr);
        SyncTaskbarBox();
        catalogThread_=CreateThread(nullptr,0,CatalogThreadProc,this,0,nullptr);
        if(!catalogThread_){catalogReady_=true;Wh_Log(L"[SearchOnly] background catalog thread failed: %u",GetLastError());}
        Wh_Log(L"[SearchOnly] ready; application catalog is loading asynchronously");
        return true;
    }

    static void DestroyCatalog(std::vector<SearchEntry>& catalog){for(auto& entry:catalog)if(entry.icon){DestroyIcon(entry.icon);entry.icon=nullptr;}catalog.clear();}
    static DWORD WINAPI CatalogThreadProc(void* context){
        auto* self=static_cast<SearchPane*>(context);
        std::vector<SearchEntry>* catalog=nullptr;
        try{
            CoInitScope com;
            catalog=new std::vector<SearchEntry>();
            BuildAppCatalog(*catalog);
            HWND target=self->catalogWindow_.load();
            if(self->catalogStopping_.load()||g_stopping.load()||!target||
               !PostMessageW(target,kCatalogReadyMessage,0,reinterpret_cast<LPARAM>(catalog))){
                DestroyCatalog(*catalog);delete catalog;
            }
        }catch(...){
            if(catalog){DestroyCatalog(*catalog);delete catalog;}
            HWND target=self->catalogWindow_.load();
            if(!self->catalogStopping_.load()&&!g_stopping.load()&&target)
                PostMessageW(target,kCatalogReadyMessage,1,0);
            Wh_Log(L"[SearchOnly] application catalog scan failed");
        }
        return 0;
    }

    void UpdateHotkey() {
        if (!hwnd_) return;
        if (hotkeyRegistered_) {
            UnregisterHotKey(hwnd_, kHotkeyId);
            hotkeyRegistered_ = false;
        }
        if (!g_searchHotkeyEnabled.load()) return;
        if (RegisterHotKey(hwnd_, kHotkeyId, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'S')) {
            hotkeyRegistered_ = true;
            Wh_Log(L"[SearchOnly] Ctrl+Alt+S registered");
        } else {
            Wh_Log(L"[SearchOnly] Ctrl+Alt+S unavailable (error %u)", GetLastError());
        }
    }

    void UpdateWebVisibility() {
        if (!g_webSearchEnabled.load() && hoveredFooter_ == 1) hoveredFooter_ = -1;
        SyncTaskbarBox();
        if (hwnd_) InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void Destroy() {
        visible_ = false;
        slideActive_ = false;
        customMenuOpen_ = false;
        g_paneWindow = nullptr;
        g_taskbarBoxWindow = nullptr;
        catalogStopping_ = true;
        if (hwnd_) {
            KillTimer(hwnd_, kTaskbarSyncTimer);
            KillTimer(hwnd_, kSlideTimer);
            KillTimer(hwnd_, kCaretTimer);
        }
        if (catalogThread_) {
            WaitForSingleObject(catalogThread_, INFINITE);
            CloseHandle(catalogThread_);
            catalogThread_ = nullptr;
        }
        if (taskbarBoxHwnd_) {
            HWND box = taskbarBoxHwnd_;
            taskbarBoxHwnd_ = nullptr;
            taskbarBoxOwner_ = nullptr;
            if (IsWindow(box)) {
                ShowWindow(box, SW_HIDE);
                DestroyWindow(box);
            }
        }
        if (hwnd_) {
            MSG pending{};
            while (PeekMessageW(&pending, hwnd_, kCatalogReadyMessage, kCatalogReadyMessage, PM_REMOVE)) {
                auto* catalog = reinterpret_cast<std::vector<SearchEntry>*>(pending.lParam);
                if (catalog) { DestroyCatalog(*catalog); delete catalog; }
            }
            if (hotkeyRegistered_) UnregisterHotKey(hwnd_, kHotkeyId);
            hotkeyRegistered_ = false;
            HWND window = hwnd_;
            hwnd_ = nullptr;
            catalogWindow_ = nullptr;
            if (IsWindow(window)) {
                ShowWindow(window, SW_HIDE);
                DestroyWindow(window);
            }
        }
        for (auto& entry : apps_) {
            if (entry.icon) DestroyIcon(entry.icon);
            entry.icon = nullptr;
        }
        apps_.clear();
    }

private:
    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    bool hotkeyRegistered_ = false;
    bool visible_ = false;
    bool slideActive_ = false;
    bool slideHidesAtEnd_ = false;
    ULONGLONG slideStartTick_ = 0;
    RECT slideFromRect_{};
    RECT slideToRect_{};
    bool workAreaClipActive_ = false;
    bool catalogReady_ = false;
    std::atomic<bool> catalogStopping_{false};
    std::atomic<HWND> catalogWindow_{nullptr};
    HANDLE catalogThread_ = nullptr;
    bool customMenuOpen_ = false;
    int customMenuTarget_ = -1, customMenuHover_ = -1;
    RECT customMenuRect_{};
    std::vector<int> customMenuActions_; // 1=open, 2=show location, 3=copy shortcut path
    std::wstring query_;
    size_t caretPos_=0, selectionAnchor_=0, selectionEnd_=0;
    bool hasSelection_=false, caretVisible_=true, pendingHighSurrogate_=false, textSelecting_=false;
    wchar_t pendingHigh_=0;
    std::vector<SearchEntry> apps_;
    std::vector<SearchEntry> results_;
    std::vector<int> rowHeights_;
    int settingsHeaderHeight_ = 0;
    enum class SearchScope { All, Apps, Settings };
    int selected_ = 0;
    int scrollOffset_ = 0;
    bool scrollbarDragging_ = false;
    int scrollbarDragStartY_ = 0;
    int scrollbarDragStartOffset_ = 0;
    SearchScope scope_ = SearchScope::All;
    UINT dpi_ = 96;
    UINT taskbarBoxDpi_ = 96;
    RECT workArea_{};
    int bottomInset_ = 0;   // taskbar thickness covered by the pane (0 when not covering)
    RECT startHole_{};      // screen rect (Start button column) that must stay uncovered
    HWND taskbarBoxHwnd_ = nullptr;
    HWND taskbarBoxOwner_ = nullptr;
    RECT taskbarBoxRect_{};
    bool hasBoxRect_ = false;
    bool wideBoxMode_ = false;
    bool taskbarBoxHovered_ = false;
    std::wstring nativeSearchPlaceholder_;
    int paneX_ = 0;
    int paneWidth_ = 0;
    int panelHeight_ = 0;
    int settingsScrollOffset_ = 0;
    int settingsContentHeight_ = 0;
    RECT homeAppsCard_{};
    RECT homeSettingsCard_{};
    std::vector<RECT> settingsCategoryRects_;
    int hoveredResult_ = -1;
    int hoveredSettings_ = -1;
    int hoveredHome_ = -1;
    int hoveredNav_ = -1;
    int hoveredFooter_ = -1;

    int Scale(int value) const { return MulDiv(value, static_cast<int>(dpi_), 96); }

    void InvalidatePane() {
        if (hwnd_) InvalidateRect(hwnd_, nullptr, FALSE);
        if (taskbarBoxHwnd_ && IsWindowVisible(taskbarBoxHwnd_)) {
            InvalidateRect(taskbarBoxHwnd_, nullptr, FALSE);
            UpdateWindow(taskbarBoxHwnd_);
        }
    }

    int QueryHeight() const {
        // In wideBoxMode_, the search bar is the locked taskbarBoxHwnd_ window on the taskbar,
        // so hwnd_ ("the other part of search") only contains the upper pane above the taskbar.
        if (wideBoxMode_) return 0;
        return Scale(kQueryHeightIcon96);
    }

    int FooterHeight() const {
        return Scale(wideBoxMode_ ? kFooterHeightBox96 : kFooterHeightIcon96);
    }

    int CustomMenuRowHeight() const {
        return Scale(wideBoxMode_ ? 40 : 34);
    }

    const wchar_t* EffectiveSearchPlaceholder() const {
        if (!nativeSearchPlaceholder_.empty())
            return nativeSearchPlaceholder_.c_str();
        return Text(UiLabel::SearchPlaceholder);
    }

    int ComputePaneWidth(LONG workWidth) const {
        if (wideBoxMode_) {
            if (hasBoxRect_) {
                const LONG boxEnd = taskbarBoxRect_.right - paneX_;
                if (boxEnd >= Scale(280) && boxEnd <= workWidth)
                    return static_cast<int>(boxEnd);
            }
            return static_cast<int>((std::min)(static_cast<LONG>(Scale(kPaneWidthBox96)), workWidth));
        }
        return static_cast<int>((std::min)(static_cast<LONG>(Scale(kPaneWidthIcon96)), workWidth));
    }

    void RefreshTaskbarSearchState(HMONITOR monitor) {
        if (g_stopping.load()) return;
        try {
            const TaskbarSearchState state = InspectTaskbarSearch(monitor, nativeSearchPlaceholder_.empty());
            if (!state.nativePlaceholder.empty() && nativeSearchPlaceholder_ != state.nativePlaceholder) {
                nativeSearchPlaceholder_ = state.nativePlaceholder;
                Wh_Log(L"[SearchOnly] resolved native Windows search string: '%s'",
                       nativeSearchPlaceholder_.c_str());
            }
            wideBoxMode_ = state.isWideBox && g_overTaskbar.load();
            hasBoxRect_ = state.hasBoxRect;
            if (state.hasBoxRect) taskbarBoxRect_ = state.boxRect;
        } catch (...) {
            Wh_Log(L"[SearchOnly] exception in RefreshTaskbarSearchState");
        }
    }

    void SyncTaskbarBox() {
        if (!taskbarBoxHwnd_ || g_stopping.load()) return;
        try {
            const TaskbarSearchState state = InspectTaskbarSearch(nullptr, nativeSearchPlaceholder_.empty());
            if (!state.nativePlaceholder.empty() && nativeSearchPlaceholder_ != state.nativePlaceholder) {
                nativeSearchPlaceholder_ = state.nativePlaceholder;
                Wh_Log(L"[SearchOnly] resolved native Windows search string: '%s'",
                       nativeSearchPlaceholder_.c_str());
                InvalidateRect(taskbarBoxHwnd_, nullptr, FALSE);
            }

            // Keep taskbarBoxHwnd_ LOCKED in place on the taskbar at all times in wideBoxMode_
            // (both when the pane is closed and when the pane is open/animating).
            const bool shouldShow = g_overTaskbar.load() &&
                                    state.isWideBox && state.hasBoxRect &&
                                    state.trayHwnd && IsWindowVisible(state.trayHwnd);
            if (!shouldShow) {
                if (IsWindowVisible(taskbarBoxHwnd_)) {
                    ShowWindow(taskbarBoxHwnd_, SW_HIDE);
                    taskbarBoxHovered_ = false;
                }
                return;
            }

            wideBoxMode_ = true;
            hasBoxRect_ = true;
            taskbarBoxRect_ = state.boxRect;
            taskbarBoxOwner_ = state.trayHwnd;

            HMONITOR mon = MonitorFromRect(&state.boxRect, MONITOR_DEFAULTTONEAREST);
            if (!mon) mon = MonitorFromWindow(state.trayHwnd, MONITOR_DEFAULTTONEAREST);
            UINT newDpi = EffectiveDpiForWindow(taskbarBoxHwnd_, mon);
            const int boxH = state.boxRect.bottom - state.boxRect.top;
            if (boxH >= 46 && boxH <= 52 && newDpi < 120) newDpi = 120;
            else if (boxH >= 56 && boxH <= 64 && newDpi < 144) newDpi = 144;
            else if (boxH >= 76 && newDpi < 192) newDpi = 192;
            if (newDpi && newDpi != taskbarBoxDpi_) {
                taskbarBoxDpi_ = newDpi;
                InvalidateRect(taskbarBoxHwnd_, nullptr, FALSE);
            }

            POINT pt{};
            const bool hov = GetCursorPos(&pt) && PtInRect(&state.boxRect, pt);
            if (hov != taskbarBoxHovered_) {
                taskbarBoxHovered_ = hov;
                InvalidateRect(taskbarBoxHwnd_, nullptr, FALSE);
            }

            RECT cur{};
            const bool vis = IsWindowVisible(taskbarBoxHwnd_) != FALSE;
            GetWindowRect(taskbarBoxHwnd_, &cur);
            if (!vis || !EqualRect(&cur, &state.boxRect)) {
                HWND prevAboveTray = GetWindow(state.trayHwnd, GW_HWNDPREV);
                HWND insertAfter = (prevAboveTray && prevAboveTray != taskbarBoxHwnd_)
                    ? prevAboveTray : HWND_TOPMOST;
                SetWindowPos(taskbarBoxHwnd_, insertAfter,
                             state.boxRect.left, state.boxRect.top,
                             state.boxRect.right - state.boxRect.left,
                             state.boxRect.bottom - state.boxRect.top,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
                InvalidateRect(taskbarBoxHwnd_, nullptr, FALSE);
                UpdateWindow(taskbarBoxHwnd_);
            } else if (!IsAnyPopupMenuOrFlyoutActive(state.trayHwnd) &&
                       !IsWindowAboveInZOrder(taskbarBoxHwnd_, state.trayHwnd)) {
                // Place taskbarBoxHwnd_ immediately above Shell_TrayWnd (below prevAboveTray)
                // so it never jumps above any other topmost popup or context menu.
                HWND prevAboveTray = GetWindow(state.trayHwnd, GW_HWNDPREV);
                HWND insertAfter = (prevAboveTray && prevAboveTray != taskbarBoxHwnd_)
                    ? prevAboveTray : HWND_TOPMOST;
                SetWindowPos(taskbarBoxHwnd_, insertAfter, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        } catch (const std::bad_alloc&) {
            Wh_Log(L"[SearchOnly] allocation failure in SyncTaskbarBox");
        } catch (...) {
            Wh_Log(L"[SearchOnly] exception in SyncTaskbarBox");
        }
    }

    int ListTop() const { return Scale(6); }

    int ListBottom() const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_, &client)) return 0;
        return static_cast<int>(client.bottom) - (QueryHeight() + FooterHeight());
    }

    int ListViewportHeight() const { return (std::max)(0, ListBottom() - ListTop()); }

    int TotalListContentHeight() const {
        int total = 0;
        bool settingsHeader = false;
        for (size_t i = 0; i < results_.size(); ++i) {
            if (results_[i].kind == ResultKind::Setting && !settingsHeader) {
                total += settingsHeaderHeight_;
                settingsHeader = true;
            }
            total += i < rowHeights_.size() ? rowHeights_[i] : Scale(44);
        }
        if (results_.empty() && !query_.empty()) total = Scale(36);
        return total;
    }

    int ContentHeightForResults() const {
        const int chrome = Scale(12) + QueryHeight() + FooterHeight();
        const int available = static_cast<int>(workArea_.bottom - workArea_.top);
        const int maxHeight = available > 0 ? available : INT_MAX;
        const int maxListHeight = (std::max)(0, maxHeight - chrome);
        const int baseRowHeight = (std::max)(1, Scale(44));
        const int fitSlots = (std::max)(1, (std::min)(kMaxVisibleRows,
            maxListHeight / baseRowHeight));
        const int blankViewport = (std::min)(maxListHeight, fitSlots * baseRowHeight);

        int resultHeight = 0;
        bool settingsHeader = false;
        int shown = 0;
        for (size_t i = 0; i < results_.size() && shown < fitSlots; ++i) {
            const int header = results_[i].kind == ResultKind::Setting && !settingsHeader
                ? settingsHeaderHeight_ : 0;
            const int row = i < rowHeights_.size() ? rowHeights_[i] : baseRowHeight;
            if (shown > 0 && resultHeight + header + row > maxListHeight) break;
            resultHeight += header + row;
            if (header) settingsHeader = true;
            ++shown;
            if (resultHeight >= maxListHeight) break;
        }

        // Keep a useful empty search viewport without showing an unfiltered app list.
        const int listHeight = (std::max)(blankViewport, (std::min)(resultHeight, maxListHeight));
        return (std::max)(1, (std::min)(chrome + listHeight, maxHeight));
    }

    // Chooses the area the pane may occupy. When the taskbar has the wide search box
    // (wideBoxMode_) and "over the taskbar" is enabled, the pane reaches the monitor bottom
    // so its themed search bar sits on the taskbar's search-box row while the Start-button
    // column (startHole_) stays uncovered. When the taskbar shows only the search icon
    // without text (!wideBoxMode_), the pane stays above the taskbar like the previous version.
    void ApplyMonitorInfo(const MONITORINFO& info) {
        workArea_ = info.rcWork;
        bottomInset_ = 0;
        startHole_ = RECT{};
        if (wideBoxMode_ && hasBoxRect_ &&
            taskbarBoxRect_.top >= info.rcWork.bottom - Scale(4) &&
            taskbarBoxRect_.top <= info.rcMonitor.bottom) {
            workArea_.bottom = (std::min)(info.rcWork.bottom, taskbarBoxRect_.top);
        }
    }

    void ClampScrollOffset() {
        scrollOffset_ = (std::max)(0, (std::min)(scrollOffset_,
            TotalListContentHeight() - ListViewportHeight()));
    }

    bool GetSettingsScrollbarGeometry(int& trackTop, int& trackBottom,
                                      int& thumbTop, int& thumbHeight) const {
        const int viewport = ListViewportHeight();
        trackTop = ListTop();
        trackBottom = ListBottom();
        const int trackHeight = trackBottom-trackTop;
        if (viewport <= 0 || trackHeight <= 0 || settingsContentHeight_ <= viewport) return false;
        const int maxScroll = settingsContentHeight_-viewport;
        thumbHeight = (std::min)(trackHeight,
            (std::max)(Scale(26),MulDiv(trackHeight,viewport,settingsContentHeight_)));
        const int travel = (std::max)(0,trackHeight-thumbHeight);
        thumbTop = trackTop+(maxScroll?MulDiv(travel,settingsScrollOffset_,maxScroll):0);
        return true;
    }

    bool GetScrollbarGeometry(int& trackTop, int& trackBottom,
                              int& thumbTop, int& thumbHeight) const {
        const int total = TotalListContentHeight();
        const int viewport = ListViewportHeight();
        trackTop = ListTop();
        trackBottom = ListBottom();
        const int trackHeight = trackBottom - trackTop;
        if (viewport <= 0 || trackHeight <= 0 || total <= viewport) return false;
        const int maxScroll = total - viewport;
        thumbHeight = (std::min)(trackHeight,
            (std::max)(Scale(24), MulDiv(trackHeight, viewport, total)));
        const int travel = (std::max)(0, trackHeight - thumbHeight);
        thumbTop = trackTop + (maxScroll ? MulDiv(travel, scrollOffset_, maxScroll) : 0);
        return true;
    }

    void ResizeToContent() {
        if (!hwnd_ || !visible_) return;
        const int height = ContentHeightForResults();
        panelHeight_ = height;
        const int y = static_cast<int>(workArea_.bottom) - height;
        const RECT targetRect{paneX_, y, paneX_ + paneWidth_, y + height};
        if (slideActive_ && !slideHidesAtEnd_) {
            // Keep an in-progress entrance smooth if typing/catalog updates change its height.
            slideToRect_ = targetRect;
        } else {
            if (SetWindowPos(hwnd_, nullptr, paneX_, y, paneWidth_, height,
                             SWP_NOACTIVATE | SWP_NOZORDER))
                UpdateWorkAreaClip();
        }
        ClampScrollOffset();
    }

    void EnsureSelectedVisible() {
        if (selected_ < 0 || selected_ >= static_cast<int>(results_.size())) return;
        int rowTop = 0;
        bool settingsHeader = false;
        for (int i = 0; i < selected_; ++i) {
            if (results_[i].kind == ResultKind::Setting && !settingsHeader) {
                rowTop += settingsHeaderHeight_;
                settingsHeader = true;
            }
            rowTop += i < static_cast<int>(rowHeights_.size()) ? rowHeights_[i] : Scale(44);
        }
        if (results_[selected_].kind == ResultKind::Setting && !settingsHeader)
            rowTop += settingsHeaderHeight_;
        const int rowHeight = selected_ < static_cast<int>(rowHeights_.size())
            ? rowHeights_[selected_] : Scale(44);
        const int viewport = ListViewportHeight();
        if (rowTop < scrollOffset_) scrollOffset_ = rowTop;
        else if (rowTop + rowHeight > scrollOffset_ + viewport)
            scrollOffset_ = rowTop + rowHeight - viewport;
        ClampScrollOffset();
    }


    static LRESULT CALLBACK TaskbarBoxWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        SearchPane* self = reinterpret_cast<SearchPane*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (message == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = static_cast<SearchPane*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);
        try {
            switch (message) {
            case WM_MOUSEACTIVATE:
                return MA_NOACTIVATE;
            case WM_SETCURSOR: {
                if (LOWORD(lParam) == HTCLIENT) {
                    POINT pt{};
                    GetCursorPos(&pt);
                    ScreenToClient(hwnd, &pt);
                    const int dpi = static_cast<int>(self->taskbarBoxDpi_ ? self->taskbarBoxDpi_ : self->dpi_);
                    const bool inText = self->visible_ && pt.x >= MulDiv(40, dpi, 96);
                    SetCursor(LoadCursorW(nullptr, inText ? IDC_IBEAM : IDC_ARROW));
                    return TRUE;
                }
                break;
            }
            case WM_MOUSEMOVE: {
                TRACKMOUSEEVENT tme{};
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
                if (!self->taskbarBoxHovered_) {
                    self->taskbarBoxHovered_ = true;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            case WM_MOUSELEAVE:
                if (self->taskbarBoxHovered_) {
                    self->taskbarBoxHovered_ = false;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            case WM_ERASEBKGND:
                return 1;
            case WM_PAINT: {
                PAINTSTRUCT ps{};
                HDC dc = BeginPaint(hwnd, &ps);
                if (dc) {
                    RECT client{};
                    GetClientRect(hwnd, &client);
                    const int w = client.right - client.left;
                    const int h = client.bottom - client.top;
                    if (w > 0 && h > 0) {
                        MemoryDc buffer(CreateCompatibleDC(dc));
                        GdiObject bmp(reinterpret_cast<HGDIOBJ>(CreateCompatibleBitmap(dc, w, h)));
                        const int dpi = static_cast<int>(self->taskbarBoxDpi_ ? self->taskbarBoxDpi_ : self->dpi_);
                        const std::wstring placeholder = self->EffectiveSearchPlaceholder();
                        const bool active = self->visible_;
                        const std::wstring emptyQuery;
                        const std::wstring& q = active ? self->query_ : emptyQuery;
                        const bool caretVis = active && self->caretVisible_;
                        const bool hasSel = active && self->hasSelection_;
                        if (buffer.value && bmp.value) {
                            SelectedGdiObject sel(buffer.value, bmp.value);
                            DrawThemedSearchBoxBar(buffer.value, hwnd, client, dpi, q, placeholder,
                                                   active, self->taskbarBoxHovered_, caretVis,
                                                   self->caretPos_, hasSel,
                                                   self->selectionAnchor_, self->selectionEnd_);
                            BitBlt(dc, 0, 0, w, h, buffer.value, 0, 0, SRCCOPY);
                        } else {
                            DrawThemedSearchBoxBar(dc, hwnd, client, dpi, q, placeholder,
                                                   active, self->taskbarBoxHovered_, caretVis,
                                                   self->caretPos_, hasSel,
                                                   self->selectionAnchor_, self->selectionEnd_);
                        }
                    }
                }
                EndPaint(hwnd, &ps);
                return 0;
            }
            case WM_NCDESTROY:
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
                return DefWindowProcW(hwnd, message, wParam, lParam);
            }
            return DefWindowProcW(hwnd, message, wParam, lParam);
        } catch (const std::bad_alloc&) {
            Wh_Log(L"[SearchOnly] allocation failed in TaskbarBoxWndProc (msg=%u)", message);
            return 0;
        } catch (...) {
            Wh_Log(L"[SearchOnly] unexpected exception in TaskbarBoxWndProc (msg=%u)", message);
            return 0;
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        SearchPane* self = reinterpret_cast<SearchPane*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (message == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = static_cast<SearchPane*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);
        try {
            return self->HandleMessage(hwnd, message, wParam, lParam);
        } catch (const std::bad_alloc&) {
            Wh_Log(L"[SearchOnly] allocation failed while processing window message %u", message);
            return 0;
        } catch (...) {
            Wh_Log(L"[SearchOnly] unexpected exception while processing window message %u", message);
            return 0;
        }
    }

    void Toggle() {
        if (visible_) { Hide(); return; }
        Show();
    }

    void SetScope(SearchScope scope) {
        scope_ = scope;
        settingsScrollOffset_ = 0;
        Filter();
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void Show() {
        RECT reopenFromRect{};
        const bool reversingClose = slideActive_ && IsWindowVisible(hwnd_);
        if (reversingClose) GetWindowRect(hwnd_, &reopenFromRect);
        // RTM CDesktopHost chooses a popup monitor from a runtime anchor/rectangle.
        // This standalone search-only window uses the cursor monitor and documented
        // monitor/work-area APIs; it never calls the private ViewPositionController.
        query_.clear();caretPos_=0;ClearSelection();caretVisible_=true;pendingHighSurrogate_=false;
        results_.clear();
        rowHeights_.clear();
        selected_ = 0;
        scrollOffset_ = 0;
        scope_ = SearchScope::All;

        POINT anchor{};
        if (!GetCursorPos(&anchor)) anchor = POINT{0,0};
        HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
        if (!monitor) monitor = MonitorFromPoint(POINT{0,0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info{}; info.cbSize = sizeof(info);
        if (!monitor || !GetMonitorInfoW(monitor, &info)) return;

        RefreshTaskbarSearchState(monitor);
        SyncTaskbarBox();

        ApplyMonitorInfo(info);
        const LONG workWidth = workArea_.right - workArea_.left;
        const LONG workHeight = workArea_.bottom - workArea_.top;
        paneX_ = static_cast<int>(workArea_.left);

        // Move the hidden window onto the chosen monitor before asking USER32 for
        // that window's effective DPI. This respects Explorer's DPI-awareness context.
        const int provisionalWidth = static_cast<int>((std::min)(
            static_cast<LONG>(wideBoxMode_ ? kPaneWidthBox96 : kPaneWidthIcon96), workWidth));
        if (!SetWindowPos(hwnd_, nullptr, paneX_, static_cast<int>(workArea_.top), provisionalWidth,
                          static_cast<int>((std::max)(1L, workHeight)),
                          SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW)) return;
        const UINT dpiX = EffectiveDpiForWindow(hwnd_, monitor);
        dpi_ = dpiX;
        taskbarBoxDpi_ = dpiX;
        ApplyMonitorInfo(info);
        paneWidth_ = ComputePaneWidth(workWidth);

        // Establish the final measured client width first. Filter() measures wrapped
        // rows; the panel then shows at most 11 rows and stays bottom-aligned to work rc.
        if (!SetWindowPos(hwnd_, nullptr, paneX_, static_cast<int>(workArea_.top),
                          paneWidth_, static_cast<int>((std::max)(1L, workHeight)),
                          SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW)) return;
        Filter();
        const int height = ContentHeightForResults();
        panelHeight_ = height;
        const int y = static_cast<int>(workArea_.bottom) - height;
        RECT targetRect{paneX_, y, paneX_ + paneWidth_, y + height};
        RECT fromRect{paneX_, y + Scale(18), paneX_ + paneWidth_, y + height + Scale(18)};
        if (reversingClose) fromRect = reopenFromRect;
        if (!SetWindowPos(hwnd_, HWND_TOPMOST, fromRect.left, fromRect.top,
                          fromRect.right-fromRect.left, fromRect.bottom-fromRect.top,
                          SWP_NOACTIVATE | SWP_SHOWWINDOW)) return;
        UpdateWorkAreaClip();
        visible_ = true;
        SyncTaskbarBox();
        SetTimer(hwnd_,kCaretTimer,530,nullptr);
        BeginSlideAnimation(fromRect, targetRect, false);
        ClampScrollOffset();
        const bool gotForeground = ForceForeground(hwnd_);
        SyncTaskbarBox();
        InvalidatePane();
        Wh_Log(L"[SearchOnly] pane %s the foreground", gotForeground ? L"has" : L"did NOT get");
        Wh_Log(L"[SearchOnly] monitor work area=%ldx%ld dpi=%u pane=%dx%d at (%d,%d)",
            workWidth, workHeight, dpiX, paneWidth_, panelHeight_, paneX_, y);
    }

    void Hide() {
        if (!hwnd_ || !visible_) return;
        visible_ = false;
        customMenuOpen_ = false; customMenuActions_.clear(); customMenuTarget_ = customMenuHover_ = -1;
        KillTimer(hwnd_,kCaretTimer);
        InvalidatePane();
        if (IsWindowVisible(hwnd_)) {
            RECT fromRect{}; GetWindowRect(hwnd_, &fromRect);
            RECT toRect = fromRect;
            OffsetRect(&toRect, 0, Scale(18));
            BeginSlideAnimation(fromRect, toRect, true);
        } else {
            ClearSearchState();
            SyncTaskbarBox();
        }
    }

    void ClearSearchState() {
        query_.clear();
        results_.clear();
        rowHeights_.clear();
        selected_ = 0;
        scrollOffset_ = 0;
    }

    void ClearWorkAreaClip() {
        if (!hwnd_ || !workAreaClipActive_) return;
        if (SetWindowRgn(hwnd_, nullptr, TRUE)) {
            workAreaClipActive_ = false;
        } else {
            Wh_Log(L"[SearchOnly] clearing work-area clip failed: %u", GetLastError());
        }
    }

    // Like the separate Start mod, clip each animation frame so the window never
    // paints over the taskbar. rcWork comes from documented GetMonitorInfoW and
    // excludes the taskbar/appbar area; the region is local to the current window.
    void UpdateWorkAreaClip() {
        if (!hwnd_) return;
        RECT windowRect{};
        if (!GetWindowRect(hwnd_, &windowRect)) return;
        RECT visibleRect{};
        if (!IntersectRect(&visibleRect, &windowRect, &workArea_)) {
            GdiObject emptyRegion(reinterpret_cast<HGDIOBJ>(CreateRectRgn(0,0,0,0)));
            if (!emptyRegion.value) return;
            if (SetWindowRgn(hwnd_, reinterpret_cast<HRGN>(emptyRegion.value), TRUE)) {
                emptyRegion.value = nullptr; // ownership transfers to USER32 on success
                workAreaClipActive_ = true;
            }
            return;
        }
        RECT holeRect{};
        const bool hasHole = bottomInset_ > 0 && IntersectRect(&holeRect, &startHole_, &windowRect);
        if (EqualRect(&visibleRect, &windowRect) && !hasHole) {
            ClearWorkAreaClip();
            return;
        }
        GdiObject visibleRegion(reinterpret_cast<HGDIOBJ>(CreateRectRgn(
            visibleRect.left-windowRect.left, visibleRect.top-windowRect.top,
            visibleRect.right-windowRect.left, visibleRect.bottom-windowRect.top)));
        if (!visibleRegion.value) return;
        if (hasHole) {
            GdiObject holeRegion(reinterpret_cast<HGDIOBJ>(CreateRectRgn(
                holeRect.left-windowRect.left, holeRect.top-windowRect.top,
                holeRect.right-windowRect.left, holeRect.bottom-windowRect.top)));
            if (holeRegion.value)
                CombineRgn(reinterpret_cast<HRGN>(visibleRegion.value),
                           reinterpret_cast<HRGN>(visibleRegion.value),
                           reinterpret_cast<HRGN>(holeRegion.value), RGN_DIFF);
        }
        if (SetWindowRgn(hwnd_, reinterpret_cast<HRGN>(visibleRegion.value), TRUE)) {
            visibleRegion.value = nullptr; // ownership transfers to USER32 on success
            workAreaClipActive_ = true;
        } else {
            Wh_Log(L"[SearchOnly] setting work-area clip failed: %u", GetLastError());
        }
    }

    void SetAnimatedWindowRect(const RECT& rect) {
        if (!hwnd_) return;
        if (SetWindowPos(hwnd_, nullptr, rect.left, rect.top,
            rect.right-rect.left, rect.bottom-rect.top, SWP_NOACTIVATE | SWP_NOZORDER))
            UpdateWorkAreaClip();
    }

    void CompleteSlideAnimation() {
        if (!slideActive_) return;
        KillTimer(hwnd_, kSlideTimer);
        const bool hideAtEnd = slideHidesAtEnd_;
        slideActive_ = false;
        SetAnimatedWindowRect(slideToRect_);
        if (hideAtEnd && !visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            ClearWorkAreaClip();
            ClearSearchState();
            SyncTaskbarBox();
        } else {
            UpdateWorkAreaClip();
        }
    }

    void BeginSlideAnimation(const RECT& fromRect, const RECT& toRect, bool hideAtEnd) {
        if (slideActive_) KillTimer(hwnd_, kSlideTimer);
        slideFromRect_ = fromRect;
        slideToRect_ = toRect;
        slideHidesAtEnd_ = hideAtEnd;
        slideStartTick_ = GetTickCount64();
        slideActive_ = true;
        if (!SetTimer(hwnd_, kSlideTimer, kSlideTimerIntervalMs, nullptr)) {
            Wh_Log(L"[SearchOnly] slide timer unavailable; completing transition immediately");
            CompleteSlideAnimation();
        }
    }

    void UpdateSlideAnimation() {
        if (!slideActive_) return;
        const ULONGLONG elapsed = GetTickCount64() - slideStartTick_;
        const double t = (std::min)(1.0, static_cast<double>(elapsed) / kSlideDurationMs);
        const double eased = t*t*(3.0-2.0*t); // Same smoothstep interpolation as the separate Start mod.
        auto lerp = [eased](LONG a, LONG b) {
            return static_cast<LONG>(std::lround(a + (b-a)*eased));
        };
        RECT rect{};
        rect.left = lerp(slideFromRect_.left, slideToRect_.left);
        rect.top = lerp(slideFromRect_.top, slideToRect_.top);
        rect.right = lerp(slideFromRect_.right, slideToRect_.right);
        rect.bottom = lerp(slideFromRect_.bottom, slideToRect_.bottom);
        SetAnimatedWindowRect(rect);
        if (t >= 1.0) CompleteSlideAnimation();
    }

    static size_t LanguageColumn() {
        switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
            case 0x10: return 0; case 0x09: return 1; case 0x0A: return 2;
            case 0x0C: return 3; case 0x29: return 4; case 0x01: return 5;
            case 0x16: return 6; case 0x04: return 7; case 0x11: return 8;
            case 0x15: return 9; case 0x19: return 10; default: return 1;
        }
    }

    static const wchar_t* Text(UiLabel label) {
        static const wchar_t* const strings[][11] = {
            {L"Tutti",L"All",L"Todo",L"Tout",L"همه",L"الكل",L"Tudo",L"全部",L"すべて",L"Wszystko",L"Все"},
            {L"App",L"Apps",L"Aplicaciones",L"Applications",L"برنامه‌ها",L"التطبيقات",L"Aplicações",L"应用",L"アプリ",L"Aplikacje",L"Приложения"},
            {L"Impostazioni",L"Settings",L"Configuración",L"Paramètres",L"تنظیمات",L"الإعدادات",L"Definições",L"设置",L"設定",L"Ustawienia",L"Параметры"},
            {L"I miei elementi",L"My stuff",L"Mis cosas",L"Mes éléments",L"موارد من",L"محتوياتي",L"Os meus itens",L"我的内容",L"自分の項目",L"Moje elementy",L"Мои данные"},
            {L"Web",L"Web",L"Web",L"Web",L"وب",L"الويب",L"Web",L"Web",L"Web",L"Sieć",L"Интернет"},
            {L"Cerca app e impostazioni",L"Search apps and settings",L"Busca aplicaciones y configuración",L"Rechercher des applications et des paramètres",L"جستجوی برنامه‌ها و تنظیمات",L"ابحث عن التطبيقات والإعدادات",L"Pesquise aplicações e definições",L"搜索应用和设置",L"アプリと設定を検索",L"Wyszukuj aplikacje i ustawienia",L"Поиск приложений и параметров"},
            {L"Digita oppure scegli una sezione",L"Type or choose a section",L"Escribe o elige una sección",L"Saisissez un texte ou choisissez une section",L"تایپ کنید یا بخشی را انتخاب کنید",L"اكتب أو اختر قسماً",L"Escreva ou escolha uma secção",L"输入内容或选择一个类别",L"入力するかセクションを選択",L"Wpisz lub wybierz sekcję",L"Введите текст или выберите раздел"},
            {L"Sfoglia le app installate",L"Browse installed apps",L"Explorar las aplicaciones instaladas",L"Parcourir les applications installées",L"مرور برنامه‌های نصب‌شده",L"استعراض التطبيقات المثبتة",L"Ver aplicações instaladas",L"浏览已安装的应用",L"インストール済みアプリを表示",L"Przeglądaj zainstalowane aplikacje",L"Просмотр установленных приложений"},
            {L"Sfoglia le impostazioni di Windows",L"Browse Windows settings",L"Explorar la configuración de Windows",L"Parcourir les paramètres Windows",L"مرور تنظیمات ویندوز",L"استعراض إعدادات Windows",L"Ver definições do Windows",L"浏览 Windows 设置",L"Windows の設定を表示",L"Przeglądaj ustawienia systemu Windows",L"Просмотр параметров Windows"},
            {L"Digita per cercare",L"Type to search",L"Escribe para buscar",L"Saisissez votre recherche",L"برای جستجو تایپ کنید",L"اكتب للبحث",L"Escreva para pesquisar",L"输入内容进行搜索",L"入力して検索",L"Wpisz, aby wyszukać",L"Введите запрос для поиска"},
            {L"Nessun risultato",L"No results",L"No hay resultados",L"Aucun résultat",L"نتیجه‌ای نیست",L"لا توجد نتائج",L"Sem resultados",L"没有结果",L"結果がありません",L"Brak wyników",L"Нет результатов"},
            {L"Apri",L"Open",L"Abrir",L"Ouvrir",L"باز کردن",L"فتح",L"Abrir",L"打开",L"開く",L"Otwórz",L"Открыть"},
            {L"Apri percorso file",L"Open file location",L"Abrir ubicación del archivo",L"Ouvrir l’emplacement du fichier",L"باز کردن محل فایل",L"فتح موقع الملف",L"Abrir localização do ficheiro",L"打开文件所在位置",L"ファイルの場所を開く",L"Otwórz lokalizację pliku",L"Открыть расположение файла"},
            {L"Indicizzazione...",L"Indexing...",L"Indexando...",L"Indexation...",L"در حال فهرست‌سازی...",L"جارٍ الفهرسة...",L"A indexar...",L"正在索引...",L"インデックス作成中...",L"Indeksowanie...",L"Индексирование..."},
            {L"Copia percorso",L"Copy path",L"Copiar ruta",L"Copier le chemin",L"کپی مسیر",L"نسخ المسار",L"Copiar caminho",L"复制路径",L"パスをコピー",L"Kopiuj ścieżkę",L"Копировать путь"},
            {L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows",L"Windows"}
        };
        return strings[static_cast<size_t>(label)][LanguageColumn()];
    }

    static const wchar_t* RunLabel() {
        static const wchar_t* labels[] = {
            L"Esegui", L"Run", L"Ejecutar", L"Exécuter", L"اجرا", L"تشغيل",
            L"Executar", L"运行", L"実行", L"Uruchom", L"Выполнить"
        };
        LANGID lang = GetUserDefaultUILanguage();
        switch (PRIMARYLANGID(lang)) {
            case 0x10: return labels[0]; case 0x09: return labels[1];
            case 0x0A: return labels[2]; case 0x0C: return labels[3];
            case 0x29: return labels[4]; case 0x01: return labels[5];
            case 0x16: return labels[6]; case 0x04: return labels[7];
            case 0x11: return labels[8]; case 0x15: return labels[9];
            case 0x19: return labels[10]; default: return labels[1];
        }
    }

    static const wchar_t* SettingsLabel(int item) {
        static const wchar_t* const labels[kSettingsCount][11] = {
            {L"Schermo",L"Display",L"Pantalla",L"Affichage",L"صفحه‌نمایش",L"الشاشة",L"Ecrã",L"显示",L"ディスプレイ",L"Ekran",L"Дисплей"},
            {L"Bluetooth",L"Bluetooth",L"Bluetooth",L"Bluetooth",L"بلوتوث",L"Bluetooth",L"Bluetooth",L"蓝牙",L"Bluetooth",L"Bluetooth",L"Bluetooth"},
            {L"Rete e Internet",L"Network & Internet",L"Red e Internet",L"Réseau et Internet",L"شبکه و اینترنت",L"الشبكة والإنترنت",L"Rede e Internet",L"网络和 Internet",L"ネットワークとインターネット",L"Sieć i Internet",L"Сеть и Интернет"},
            {L"Personalizzazione",L"Personalization",L"Personalización",L"Personnalisation",L"شخصی‌سازی",L"تخصيص",L"Personalização",L"个性化",L"個人用設定",L"Personalizacja",L"Персонализация"},
            {L"App e funzionalità",L"Apps & features",L"Aplicaciones",L"Applications",L"برنامه‌ها",L"التطبيقات",L"Aplicações",L"应用",L"アプリ",L"Aplikacje",L"Приложения"},
            {L"Data e ora",L"Date & time",L"Fecha y hora",L"Date et heure",L"تاریخ و زمان",L"التاريخ والوقت",L"Data e hora",L"日期和时间",L"日付と時刻",L"Data i godzina",L"Дата и время"},
            {L"Accessibilità dello schermo",L"Accessibility display",L"Accesibilidad de pantalla",L"Affichage - accessibilité",L"دسترسی‌پذیری نمایشگر",L"إمكانية وصول العرض",L"Acessibilidade do ecrã",L"辅助功能显示",L"アクセシビリティ表示",L"Ułatwienia dostępu — ekran",L"Специальные возможности: экран"},
            {L"Privacy",L"Privacy",L"Privacidad",L"Confidentialité",L"حریم خصوصی",L"الخصوصية",L"Privacidade",L"隐私",L"プライバシー",L"Prywatność",L"Конфиденциальность"},
            {L"Windows Update",L"Windows Update",L"Windows Update",L"Windows Update",L"به‌روزرسانی ویندوز",L"Windows Update",L"Windows Update",L"Windows 更新",L"Windows Update",L"Windows Update",L"Центр обновления Windows"}
        };
        return labels[item][LanguageColumn()];
    }

    static const wchar_t* SettingsUri(int item) {
        static const wchar_t* const destinations[kSettingsCount] = {
            L"ms-settings:display", L"ms-settings:bluetooth", L"ms-settings:network-status",
            L"ms-settings:personalization", L"ms-settings:appsfeatures",
            L"ms-settings:dateandtime", L"ms-settings:easeofaccess-display",
            L"ms-settings:privacy", L"ms-settings:windowsupdate"
        };
        return destinations[item];
    }

    void Filter() {
        results_.clear();
        rowHeights_.clear();
        selected_ = 0;
        scrollOffset_ = 0;
        const std::wstring q = Lower(query_);
        // The All section stays a welcome/search surface. The Apps list appears only
        // after the user explicitly opens Apps; Settings opens its local category browser.
        if (q.empty()) {
            if (scope_ == SearchScope::Apps) results_ = apps_;
            MeasureResultRows();
            if (visible_) ResizeToContent();
            return;
        }
        const bool runMatch = q == L"run" || q == Lower(RunLabel());
        const bool settingsAppAlias = q == L"immersivecontrolpanel" || q == L"systemsettings.exe";
        const bool settingsQuery = settingsAppAlias || q == L"settings" || q == Lower(Text(UiLabel::Settings));

        if (scope_ != SearchScope::Settings) {
            for (const auto& app : apps_) {
                const bool explorerAlias = app.isExplorerShortcut &&
                    (q == L"explorer" || q == L"explorer.exe" || q == L"file explorer" || q == L"windows explorer");
                const bool controlPanelAlias = app.kind == ResultKind::ControlPanel &&
                    (q == L"control" || q == L"control panel" || q == L"control.exe");
                if (q.empty() || Lower(app.name).find(q) != std::wstring::npos || explorerAlias || controlPanelAlias)
                    results_.push_back(app);
            }
            if (!q.empty()) {
                std::stable_sort(results_.begin(), results_.end(), [&](const SearchEntry& a, const SearchEntry& b) {
                    return MatchRank(a.name, q) < MatchRank(b.name, q);
                });
            }
            if (runMatch && scope_ == SearchScope::All) {
                SearchEntry run{};
                run.name = RunLabel();
                run.kind = ResultKind::Run;
                results_.insert(results_.begin(), std::move(run));
            }
        }

        if (scope_ != SearchScope::Apps) {
            if (settingsAppAlias) {
                SearchEntry settingsApp{};
                settingsApp.name = Text(UiLabel::Settings);
                settingsApp.shortcutPath = L"ms-settings:";
                settingsApp.kind = ResultKind::SettingsHome;
                results_.push_back(std::move(settingsApp));
            }
            for (int i = 0; i < kSettingsCount; ++i) {
                SearchEntry setting{};
                setting.name = SettingsLabel(i);
                setting.shortcutPath = SettingsUri(i);
                setting.kind = ResultKind::Setting;
                setting.settingsIndex = i;
                const std::wstring destination = Lower(setting.shortcutPath);
                const bool include = (settingsQuery && !settingsAppAlias) ||
                    Lower(setting.name).find(q) != std::wstring::npos ||
                    destination.find(q) != std::wstring::npos;
                if (include) results_.push_back(std::move(setting));
            }
        }

        MeasureResultRows();
        if (visible_) ResizeToContent();
    }

    void MeasureResultRows() {
        rowHeights_.clear();
        settingsHeaderHeight_ = Scale(20);
        if (!hwnd_ || results_.empty()) return;
        RECT client{};
        if (!GetClientRect(hwnd_, &client)) return;
        const int rail = Scale(kRailWidth96);
        const int textLeft = rail + Scale(48);
        const int textWidth = (std::max)(1, static_cast<int>(client.right) - textLeft - Scale(18));
        WindowDc dc(hwnd_);
        if (!dc.value) return;
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(wideBoxMode_ ? 15 : 14),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI")));
        SelectedGdiObject selectedFont(dc.value,font.value);
        rowHeights_.reserve(results_.size());
        for (const auto& result : results_) {
            RECT measure{0,0,textWidth,0};
            DrawTextW(dc.value,result.name.c_str(),-1,&measure,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);
            const int measuredTextHeight = (std::max)(Scale(12), static_cast<int>(measure.bottom-measure.top));
            const int minimum = Scale(result.kind == ResultKind::Setting ? 48 : 44);
            const int padding = Scale(result.kind == ResultKind::Setting ? 12 : 10);
            rowHeights_.push_back((std::max)(minimum, measuredTextHeight + padding));
        }
    }

    int ResultAtPoint(int y) const {
        if (y < ListTop() || y >= ListBottom()) return -1;
        int target = y - ListTop() + scrollOffset_;
        int cursor = 0;
        bool settingsHeader = false;
        for (size_t i = 0; i < results_.size(); ++i) {
            if (results_[i].kind == ResultKind::Setting && !settingsHeader) {
                cursor += settingsHeaderHeight_;
                settingsHeader = true;
            }
            const int rowHeight = i < rowHeights_.size() ? rowHeights_[i] : Scale(44);
            if (target >= cursor && target < cursor + rowHeight) return static_cast<int>(i);
            cursor += rowHeight;
        }
        return -1;
    }

    size_t CaretFromBoxPixel(int boxClientX) {
        HWND refHwnd = taskbarBoxHwnd_ ? taskbarBoxHwnd_ : hwnd_;
        WindowDc dc(refHwnd);
        if (!dc.value) return query_.size();
        const int dpi = static_cast<int>(taskbarBoxDpi_ ? taskbarBoxDpi_ : dpi_);
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(
            -MulDiv(15, dpi, 96), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI")));
        SelectedGdiObject selectedFont(dc.value, font.value);
        const int start = MulDiv(40, dpi, 96);
        if (boxClientX <= start) return 0;
        size_t result = query_.size();
        for (size_t i = 0; i < query_.size(); ++i) {
            SIZE extent{};
            GetTextExtentPoint32W(dc.value, query_.c_str(), static_cast<int>(i + 1), &extent);
            if (boxClientX < start + extent.cx) {
                SIZE previous{};
                if (i) GetTextExtentPoint32W(dc.value, query_.c_str(), static_cast<int>(i), &previous);
                result = (boxClientX < start + (previous.cx + extent.cx) / 2) ? i : i + 1;
                break;
            }
        }
        return result;
    }

    size_t CaretFromPixel(int x) {
        if (wideBoxMode_) {
            return CaretFromBoxPixel(x - Scale(kRailWidth96));
        }
        WindowDc dc(hwnd_);
        if (!dc.value) return query_.size();
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(14),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe UI")));
        SelectedGdiObject selectedFont(dc.value,font.value);
        const int start=Scale(kRailWidth96)+Scale(12);
        size_t result=query_.size();
        for(size_t i=0;i<query_.size();++i){
            SIZE extent{};
            GetTextExtentPoint32W(dc.value,query_.c_str(),static_cast<int>(i+1),&extent);
            if(x<start+extent.cx){
                SIZE previous{};
                if(i)GetTextExtentPoint32W(dc.value,query_.c_str(),static_cast<int>(i),&previous);
                result=(x<start+(previous.cx+extent.cx)/2)?i:i+1;
                break;
            }
        }
        return result;
    }
    void ClearSelection(){hasSelection_=false;selectionAnchor_=selectionEnd_=caretPos_;}
    void ReplaceSelection(const std::wstring& insert){
        if(hasSelection_){
            const size_t a=(std::min)(selectionAnchor_,selectionEnd_),b=(std::max)(selectionAnchor_,selectionEnd_);
            query_.erase(a,b-a);caretPos_=a;
        }
        if(query_.size()+insert.size()>256){ClearSelection();return;}
        query_.insert(caretPos_,insert);caretPos_+=insert.size();ClearSelection();caretVisible_=true;
        Filter();InvalidatePane();
    }
    void DeletePrevious(){
        if(hasSelection_){ReplaceSelection(L"");return;}
        if(caretPos_==0)return;
        size_t start=caretPos_-1;
        if(start>0&&query_[start]>=0xDC00&&query_[start]<=0xDFFF&&query_[start-1]>=0xD800&&query_[start-1]<=0xDBFF)--start;
        query_.erase(start,caretPos_-start);caretPos_=start;ClearSelection();caretVisible_=true;Filter();InvalidatePane();
    }
    void DeleteNext(){
        if(hasSelection_){ReplaceSelection(L"");return;}
        if(caretPos_>=query_.size())return;
        size_t count=1;
        if(query_[caretPos_]>=0xD800&&query_[caretPos_]<=0xDBFF&&caretPos_+1<query_.size()&&query_[caretPos_+1]>=0xDC00&&query_[caretPos_+1]<=0xDFFF)count=2;
        query_.erase(caretPos_,count);ClearSelection();caretVisible_=true;Filter();InvalidatePane();
    }
    void MoveCaret(size_t position,bool extend){
        position=(std::min)(position,query_.size());
        if(extend){if(!hasSelection_)selectionAnchor_=caretPos_;selectionEnd_=position;hasSelection_=selectionAnchor_!=selectionEnd_;}
        else ClearSelection();
        caretPos_=position;caretVisible_=true;InvalidatePane();
    }
    void CopySelection(){
        if(!hasSelection_)return;
        const size_t a=(std::min)(selectionAnchor_,selectionEnd_),b=(std::max)(selectionAnchor_,selectionEnd_);
        if(!OpenClipboard(hwnd_))return;
        EmptyClipboard();HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,(b-a+1)*sizeof(wchar_t));
        if(memory){void* data=GlobalLock(memory);if(data){memcpy(data,query_.data()+a,(b-a)*sizeof(wchar_t));static_cast<wchar_t*>(data)[b-a]=0;GlobalUnlock(memory);if(!SetClipboardData(CF_UNICODETEXT,memory))GlobalFree(memory);}else GlobalFree(memory);}
        CloseClipboard();
    }
    std::wstring ReadClipboardText(){
        std::wstring text;if(!OpenClipboard(hwnd_))return text;HANDLE handle=GetClipboardData(CF_UNICODETEXT);
        if(handle){const wchar_t* data=static_cast<const wchar_t*>(GlobalLock(handle));if(data){for(;*data;++data)if(*data!=L'\r'&&*data!=L'\n'&&*data!=L'\t')text.push_back(*data);GlobalUnlock(handle);}}
        CloseClipboard();return text;
    }

    void SearchWeb() {
        if(!g_webSearchEnabled.load() || query_.empty()) return;
        const int bytes=WideCharToMultiByte(CP_UTF8,0,query_.c_str(),static_cast<int>(query_.size()),nullptr,0,nullptr,nullptr);
        if(bytes<=0) return;
        std::string utf8(static_cast<size_t>(bytes),'\0');
        WideCharToMultiByte(CP_UTF8,0,query_.c_str(),static_cast<int>(query_.size()),&utf8[0],bytes,nullptr,nullptr);
        static const char hex[]="0123456789ABCDEF";
        std::string encoded; encoded.reserve(utf8.size()*3);
        for(unsigned char ch:utf8){
            if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch=='-'||ch=='_'||ch=='.'||ch=='~') encoded.push_back(static_cast<char>(ch));
            else {encoded.push_back('%');encoded.push_back(hex[ch>>4]);encoded.push_back(hex[ch&15]);}
        }
        std::wstring url=L"https://www.bing.com/search?q=";
        for(char ch:encoded)url.push_back(static_cast<wchar_t>(ch));
        Hide();
        ShellExecuteW(nullptr,L"open",url.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
    }

    void OpenSelected() {
        if (selected_ < 0 || selected_ >= static_cast<int>(results_.size())) return;
        const SearchEntry item = results_[selected_];
        Hide();
        if (item.kind == ResultKind::Run) {
            ComPtr<IShellDispatch> shell;
            const HRESULT hr=CoCreateInstance(CLSID_Shell,nullptr,CLSCTX_INPROC_SERVER,
                IID_IShellDispatch,reinterpret_cast<void**>(shell.GetAddressOf()));
            if(SUCCEEDED(hr)&&shell) shell->FileRun();
            else Wh_Log(L"[SearchOnly] IShellDispatch::FileRun failed: 0x%08X",static_cast<unsigned>(hr));
        } else if (item.kind == ResultKind::ControlPanel) {
            ShellExecuteW(nullptr, L"open", L"control.exe", nullptr, nullptr, SW_SHOWNORMAL);
        } else if (!item.shortcutPath.empty()) {
            ShellExecuteW(nullptr, L"open", item.shortcutPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    }

    int DrawWrappedText(HDC dc, const wchar_t* text, int x, int y, int width,
                        COLORREF color, int fontDip, int weight, RECT* outRect = nullptr) {
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(fontDip),0,0,0,weight,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI")));
        SelectedGdiObject selectedFont(dc,font.value);
        RECT measured{x,y,x+width,y+Scale(1000)};
        DrawTextW(dc,text,-1,&measured,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);
        const int height = (std::max)(Scale(fontDip+2),static_cast<int>(measured.bottom-y));
        RECT paint{x,y,x+width,y+height};
        SetTextColor(dc,color);
        DrawTextW(dc,text,-1,&paint,DT_LEFT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX);
        if (outRect) *outRect = paint;
        return height;
    }

    void DrawHomePage(HDC dc, const RECT& client, int rail) {
        homeAppsCard_ = {};
        homeSettingsCard_ = {};
        const int left = rail+Scale(16);
        const int right = static_cast<int>(client.right)-Scale(18);
        const int width = (std::max)(1,right-left);
        int y = ListTop()+Scale(14)-scrollOffset_;
        const int titleHeight = DrawWrappedText(dc,Text(UiLabel::HomeWelcome),left,y,width,
            RGB(0,120,215),19,FW_NORMAL);
        y += titleHeight+Scale(6);
        const int hintHeight = DrawWrappedText(dc,Text(UiLabel::HomeInstruction),left,y,width,
            RGB(75,75,75),13,FW_NORMAL);
        y += hintHeight+Scale(8);

        // The old double-ring emblem was a placeholder resembling an assistant logo; keep Home neutral.
        y += Scale(24);

        const int cardHeight = Scale(78);
        const int gap = Scale(12);
        const int cardLeft = left, cardRight = right;
        homeAppsCard_ = RECT{cardLeft,y,cardRight,y+cardHeight};
        homeSettingsCard_ = RECT{cardLeft,y+cardHeight+gap,cardRight,y+2*cardHeight+gap};
        GdiObject cardBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(248,248,248))));
        GdiObject cardHoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(235,245,252))));
        GdiObject cardBorderBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(220,220,220))));
        if(cardBrush.value){
            FillRect(dc,&homeAppsCard_,reinterpret_cast<HBRUSH>(cardBrush.value));
            FillRect(dc,&homeSettingsCard_,reinterpret_cast<HBRUSH>(cardBrush.value));
        }
        if(hoveredHome_==0&&cardHoverBrush.value)FillRect(dc,&homeAppsCard_,reinterpret_cast<HBRUSH>(cardHoverBrush.value));
        if(hoveredHome_==1&&cardHoverBrush.value)FillRect(dc,&homeSettingsCard_,reinterpret_cast<HBRUSH>(cardHoverBrush.value));
        if(cardBorderBrush.value){
            FrameRect(dc,&homeAppsCard_,reinterpret_cast<HBRUSH>(cardBorderBrush.value));
            FrameRect(dc,&homeSettingsCard_,reinterpret_cast<HBRUSH>(cardBorderBrush.value));
        }
        if(!DrawMdl2GlyphGdiPlus(dc,L'\xE71D',cardLeft+Scale(18),homeAppsCard_.top+Scale(26),Scale(24),RGB(0,120,215)))
            DrawVectorIconGdiPlus(dc,1,cardLeft+Scale(18),homeAppsCard_.top+Scale(26),Scale(24),RGB(0,120,215));
        if(!DrawMdl2GlyphGdiPlus(dc,L'\xE713',cardLeft+Scale(18),homeSettingsCard_.top+Scale(26),Scale(24),RGB(0,120,215)))
            DrawVectorIconGdiPlus(dc,2,cardLeft+Scale(18),homeSettingsCard_.top+Scale(26),Scale(24),RGB(0,120,215));
        const int textLeft = cardLeft+Scale(58), textWidth = cardRight-textLeft-Scale(10);
        DrawWrappedText(dc,Text(UiLabel::Apps),textLeft,homeAppsCard_.top+Scale(10),textWidth,
                        RGB(32,32,32),15,FW_SEMIBOLD);
        DrawWrappedText(dc,Text(UiLabel::BrowseApps),textLeft,homeAppsCard_.top+Scale(39),textWidth,
                        RGB(90,90,90),12,FW_NORMAL);
        DrawWrappedText(dc,Text(UiLabel::Settings),textLeft,homeSettingsCard_.top+Scale(10),textWidth,
                        RGB(32,32,32),15,FW_SEMIBOLD);
        DrawWrappedText(dc,Text(UiLabel::BrowseSettings),textLeft,homeSettingsCard_.top+Scale(39),textWidth,
                        RGB(90,90,90),12,FW_NORMAL);
    }

    void DrawSettingsPage(HDC dc, const RECT& client, int rail) {
        settingsCategoryRects_.clear();
        const int left=rail, right=static_cast<int>(client.right)-Scale(2);
        const int headingLeft=left+Scale(48)-static_cast<int>(client.right*0.065f);
        const int width=(std::max)(1,right-headingLeft);
        const COLORREF dark=RGB(32,32,32), muted=RGB(100,100,100), blue=RGB(0,120,215);
        int y=ListTop()+Scale(14)-settingsScrollOffset_;
        y+=DrawWrappedText(dc,Text(UiLabel::Settings),headingLeft,y,width,dark,20,FW_SEMIBOLD)+Scale(4);
        y+=DrawWrappedText(dc,Text(UiLabel::Windows),headingLeft,y,width,muted,12,FW_NORMAL)+Scale(8);
        const int rowHeight=Scale(kSettingsRowHeight96);
        GdiObject rowBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(250,250,250))));
        GdiObject rowPen(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID,(std::max)(1,Scale(1)),RGB(230,230,230))));
        for(int i=0;i<kSettingsCount;++i){
            RECT row{left,y,right,y+rowHeight};
            settingsCategoryRects_.push_back(row);
            if(rowBrush.value&&rowPen.value){
                {
                    SelectedGdiObject selectedRowBrush(dc,rowBrush.value);
                    SelectedGdiObject selectedRowPen(dc,rowPen.value);
                    Rectangle(dc,row.left,row.top,row.right,row.bottom);
                }
            }
            if(i==hoveredSettings_){
                GdiObject hoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(235,245,252))));
                if(hoverBrush.value)FillRect(dc,&row,reinterpret_cast<HBRUSH>(hoverBrush.value));
            }
            const int cy=row.top+rowHeight/2;
            DrawSettingsCategoryIcon(dc,i,left+Scale(20),cy,Scale(20),blue);
            RECT label{left+Scale(48),row.top,right-Scale(12),row.bottom};
            SetTextColor(dc,dark);
            DrawTextW(dc,SettingsLabel(i),-1,&label,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
            y+=rowHeight;
        }
        settingsContentHeight_=y+settingsScrollOffset_-ListTop()+Scale(8);
        const int viewport=ListViewportHeight();
        const int maxScroll=(std::max)(0,settingsContentHeight_-viewport);
        settingsScrollOffset_=(std::max)(0,(std::min)(settingsScrollOffset_,maxScroll));
        int trackTop=0,trackBottom=0,thumbTop=0,thumbHeight=0;
        if(GetSettingsScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight)){
            RECT track{client.right-Scale(7),trackTop,client.right-Scale(2),trackBottom};
            RECT thumb{track.left,thumbTop,track.right,thumbTop+thumbHeight};
            GdiObject trackBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(238,238,238))));
            GdiObject thumbBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(145,145,145))));
            if(trackBrush.value&&thumbBrush.value){
                FillRect(dc,&track,reinterpret_cast<HBRUSH>(trackBrush.value));
                FillRect(dc,&thumb,reinterpret_cast<HBRUSH>(thumbBrush.value));
            }
        }
    }

    void ShowContextMenu(int index, POINT point) {
        if (index < 0 || index >= static_cast<int>(results_.size())) return;
        const SearchEntry& item = results_[static_cast<size_t>(index)];
        customMenuActions_.clear();
        customMenuActions_.push_back(1); // Open is implemented for every result kind.
        if (item.kind == ResultKind::App && !item.shortcutPath.empty()) {
            customMenuActions_.push_back(2);
            customMenuActions_.push_back(3);
        }
        customMenuTarget_ = index;
        customMenuHover_ = 0;
        const int width = (std::min)(Scale(236), (std::max)(Scale(176), paneWidth_ - Scale(56)));
        const int rowHeight = CustomMenuRowHeight();
        const int height = rowHeight * static_cast<int>(customMenuActions_.size()) + Scale(8);
        RECT client{};
        GetClientRect(hwnd_, &client);
        const int minX = Scale(kRailWidth96) + Scale(4);
        const int maxX = (std::max)(minX, static_cast<int>(client.right) - width - Scale(4));
        const int minY = Scale(8);
        const int maxY = (std::max)(minY, ListBottom() - height);
        customMenuRect_.left = std::clamp(static_cast<int>(point.x), minX, maxX);
        customMenuRect_.top = std::clamp(static_cast<int>(point.y), minY, maxY);
        customMenuRect_.right = customMenuRect_.left + width;
        customMenuRect_.bottom = customMenuRect_.top + height;
        customMenuOpen_ = true;
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    int CustomMenuHit(POINT point) const {
        if (!customMenuOpen_ || !PtInRect(&customMenuRect_, point)) return -1;
        const int row = (point.y - customMenuRect_.top - Scale(4)) / CustomMenuRowHeight();
        return row >= 0 && row < static_cast<int>(customMenuActions_.size()) ? row : -1;
    }

    void CloseCustomMenu() {
        if (!customMenuOpen_) return;
        customMenuOpen_ = false;
        customMenuActions_.clear();
        customMenuTarget_ = customMenuHover_ = -1;
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void CopyPathToClipboard(const std::wstring& path) {
        if (path.empty() || !OpenClipboard(hwnd_)) return;
        EmptyClipboard();
        const SIZE_T bytes = (path.size() + 1) * sizeof(wchar_t);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (memory) {
            void* target = GlobalLock(memory);
            if (target) {
                memcpy(target, path.c_str(), bytes);
                GlobalUnlock(memory);
                if (!SetClipboardData(CF_UNICODETEXT, memory)) GlobalFree(memory);
            } else GlobalFree(memory);
        }
        CloseClipboard();
    }

    void OpenResultLocation(const SearchEntry& item) {
        if (item.kind != ResultKind::App || item.shortcutPath.empty()) return;
        PIDLIST_ABSOLUTE itemPidl = ILCreateFromPathW(item.shortcutPath.c_str());
        PIDLIST_ABSOLUTE parentPidl = itemPidl ? ILCloneFull(itemPidl) : nullptr;
        if (itemPidl && parentPidl && ILRemoveLastID(parentPidl)) {
            PCUITEMID_CHILD child = ILFindLastID(itemPidl);
            PCUITEMID_CHILD children[] = {child};
            const HRESULT hr = SHOpenFolderAndSelectItems(parentPidl, 1, children, 0);
            if (FAILED(hr)) Wh_Log(L"[SearchOnly] SHOpenFolderAndSelectItems failed: 0x%08X", static_cast<unsigned>(hr));
        }
        if (parentPidl) ILFree(parentPidl);
        if (itemPidl) ILFree(itemPidl);
    }

    void ExecuteCustomMenuAction(int row) {
        if (!customMenuOpen_ || row < 0 || row >= static_cast<int>(customMenuActions_.size())) {
            CloseCustomMenu(); return;
        }
        const int action = customMenuActions_[static_cast<size_t>(row)];
        if (action >= 10 && action <= 12) {
            CloseCustomMenu();
            SetScope(action == 10 ? SearchScope::All : action == 11 ? SearchScope::Apps : SearchScope::Settings);
            return;
        }
        if (customMenuTarget_ < 0 || customMenuTarget_ >= static_cast<int>(results_.size())) {
            CloseCustomMenu(); return;
        }
        const int target = customMenuTarget_;
        const SearchEntry item = results_[static_cast<size_t>(target)];
        CloseCustomMenu();
        if (action == 1) { selected_ = target; OpenSelected(); }
        else if (action == 2) OpenResultLocation(item);
        else if (action == 3) CopyPathToClipboard(item.shortcutPath);
    }

    void DrawCustomMenu(HDC dc) {
        if (!customMenuOpen_ || customMenuActions_.empty()) return;
        if (!wideBoxMode_) {
            RECT shadow = customMenuRect_;
            OffsetRect(&shadow, Scale(3), Scale(3));
            GdiObject shadowBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(195,195,195))));
            if (shadowBrush.value) FillRect(dc, &shadow, reinterpret_cast<HBRUSH>(shadowBrush.value));
        }
        GdiObject backgroundBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(255,255,255))));
        GdiObject borderBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(wideBoxMode_ ? RGB(0,0,0) : RGB(130,130,130))));
        GdiObject hoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(wideBoxMode_ ? RGB(230,230,230) : RGB(225,239,252))));
        if (backgroundBrush.value) FillRect(dc, &customMenuRect_, reinterpret_cast<HBRUSH>(backgroundBrush.value));
        if (borderBrush.value) {
            FrameRect(dc, &customMenuRect_, reinterpret_cast<HBRUSH>(borderBrush.value));
            if (wideBoxMode_) {
                RECT inner = customMenuRect_;
                InflateRect(&inner, -1, -1);
                FrameRect(dc, &inner, reinterpret_cast<HBRUSH>(borderBrush.value));
            }
        }
        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(wideBoxMode_ ? 14 : 13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI")));
        SelectedGdiObject selectedFont(dc,font.value);
        SetBkMode(dc, TRANSPARENT);
        const int rowHeight = CustomMenuRowHeight();
        for (size_t i = 0; i < customMenuActions_.size(); ++i) {
            RECT row{customMenuRect_.left + Scale(1), customMenuRect_.top + Scale(4) + static_cast<int>(i) * rowHeight,
                     customMenuRect_.right - Scale(1), customMenuRect_.top + Scale(4) + static_cast<int>(i + 1) * rowHeight};
            if (static_cast<int>(i) == customMenuHover_ && hoverBrush.value)
                FillRect(dc, &row, reinterpret_cast<HBRUSH>(hoverBrush.value));
            const int action = customMenuActions_[i];
            const wchar_t* label = action == 1 ? Text(UiLabel::Open) : action == 2 ? Text(UiLabel::OpenLocation) :
                action == 3 ? Text(UiLabel::CopyPath) : action == 10 ? Text(UiLabel::AllResults) :
                action == 11 ? Text(UiLabel::Apps) : Text(UiLabel::Settings);
            row.left += Scale(12);
            SetTextColor(dc, RGB(32,32,32));
            DrawTextW(dc, label, -1, &row, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }

    void ShowScopeMenu(POINT point) {
        customMenuActions_ = {10, 11, 12};
        customMenuTarget_ = -1;
        customMenuHover_ = 0;
        const int width = (std::min)(Scale(236), (std::max)(Scale(176), paneWidth_ - Scale(56)));
        const int height = CustomMenuRowHeight() * 3 + Scale(8);
        RECT client{}; GetClientRect(hwnd_, &client);
        const int minX = Scale(kRailWidth96) + Scale(4);
        const int maxX = (std::max)(minX, static_cast<int>(client.right) - width - Scale(4));
        const int minY = Scale(8), maxY = (std::max)(minY, ListBottom() - height);
        customMenuRect_ = {std::clamp(static_cast<int>(point.x), minX, maxX), std::clamp(static_cast<int>(point.y), minY, maxY), 0, 0};
        customMenuRect_.right = customMenuRect_.left + width;
        customMenuRect_.bottom = customMenuRect_.top + height;
        customMenuOpen_ = true;
        InvalidateRect(hwnd_, &customMenuRect_, FALSE);
    }

    // Draws a single-line result name with the matched part in bold (as in the original pane).
    // Returns false when there is no match or the text would not fit on one line, so the
    // caller keeps the regular wrapped/ellipsized drawing path.
    bool DrawNameWithMatch(HDC dc, const std::wstring& name, const RECT& rc) {
        if (query_.empty() || name.empty()) return false;
        const std::wstring lowerName = Lower(name), lowerQuery = Lower(query_);
        if (lowerName.size() != name.size()) return false;
        const size_t pos = lowerName.find(lowerQuery);
        if (pos == std::wstring::npos) return false;
        HGDIOBJ current = GetCurrentObject(dc, OBJ_FONT);
        LOGFONTW lf{};
        if (!current || !GetObjectW(current, sizeof(lf), &lf)) return false;
        lf.lfWeight = FW_BOLD;
        GdiObject bold(reinterpret_cast<HGDIOBJ>(CreateFontIndirectW(&lf)));
        if (!bold.value) return false;
        const std::wstring before = name.substr(0, pos);
        const std::wstring match = name.substr(pos, query_.size());
        const std::wstring after = name.substr(pos + query_.size());
        SIZE sb{}, sm{}, sa{};
        GetTextExtentPoint32W(dc, before.c_str(), static_cast<int>(before.size()), &sb);
        {
            SelectedGdiObject selectedBold(dc, bold.value);
            GetTextExtentPoint32W(dc, match.c_str(), static_cast<int>(match.size()), &sm);
        }
        GetTextExtentPoint32W(dc, after.c_str(), static_cast<int>(after.size()), &sa);
        if (sb.cx + sm.cx + sa.cx > rc.right - rc.left) return false;
        TEXTMETRICW tm{};
        if (!GetTextMetricsW(dc, &tm)) return false;
        const int y = rc.top + ((rc.bottom - rc.top) - tm.tmHeight) / 2;
        int x = rc.left;
        TextOutW(dc, x, y, before.c_str(), static_cast<int>(before.size())); x += sb.cx;
        {
            SelectedGdiObject selectedBold(dc, bold.value);
            TextOutW(dc, x, y, match.c_str(), static_cast<int>(match.size())); x += sm.cx;
        }
        TextOutW(dc, x, y, after.c_str(), static_cast<int>(after.size()));
        return true;
    }

    void Draw(HDC dc) {
        RECT client{};
        GetClientRect(hwnd_, &client);
        const bool settingsPage = scope_ == SearchScope::Settings && query_.empty();
        const COLORREF bodyColor = RGB(255,255,255);
        GdiObject white(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(bodyColor)));
        GdiObject railBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(wideBoxMode_ ? RGB(17,17,17) : RGB(24,24,24))));
        GdiObject queryBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(245,245,245))));
        GdiObject selectedBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(190,218,237))));
        GdiObject navSelectedBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(0,120,215))));
        if (!white.value || !railBrush.value || !queryBrush.value || !selectedBrush.value || !navSelectedBrush.value) return;
        FillRect(dc, &client, reinterpret_cast<HBRUSH>(white.value));
        const int rail = (std::min)(Scale(kRailWidth96), static_cast<int>(client.right));
        const int queryTop = client.bottom - QueryHeight();
        const int bodyBottom = queryTop;
        const int footerTop = bodyBottom - FooterHeight();
        RECT railRc{0,0,rail,client.bottom};
        RECT queryRc{rail,queryTop,client.right,client.bottom};
        FillRect(dc, &railRc, reinterpret_cast<HBRUSH>(railBrush.value));
        if (!wideBoxMode_) {
            FillRect(dc, &queryRc, reinterpret_cast<HBRUSH>(queryBrush.value));
        }
        const int selectedNavTop = scope_ == SearchScope::All ? 48 :
                                   (scope_ == SearchScope::Apps ? 96 : 144);
        RECT homeSelected{0,Scale(selectedNavTop),rail,Scale(selectedNavTop+48)};
        FillRect(dc,&homeSelected,reinterpret_cast<HBRUSH>(navSelectedBrush.value));
        const int railBottomRef = client.bottom - bottomInset_;
        if(hoveredNav_==3||hoveredNav_==4){
            RECT hover=hoveredNav_==3?RECT{0,0,rail,Scale(48)}:
                RECT{0,railBottomRef-Scale(40),rail,railBottomRef};
            GdiObject hoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(45,55,64))));
            if(hoverBrush.value)FillRect(dc,&hover,reinterpret_cast<HBRUSH>(hoverBrush.value));
        }

        const int cx = rail/2;
        const int iconCell=Scale(22);
        const int navCenters[]={Scale(72),Scale(120),Scale(168)};
        const int selectedNav=scope_==SearchScope::All?0:(scope_==SearchScope::Apps?1:2);
        static const wchar_t navGlyphs[]={L'\xE80F',L'\xE71D',L'\xE713'};
        for(int i=0;i<3;++i){
            if(i==hoveredNav_ && i!=selectedNav){
                RECT hover{0,Scale(48+i*48),rail,Scale(48+(i+1)*48)};
                GdiObject hoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(45,55,64))));
                if(hoverBrush.value)FillRect(dc,&hover,reinterpret_cast<HBRUSH>(hoverBrush.value));
            }
            if(!DrawMdl2GlyphGdiPlus(dc,navGlyphs[i],cx-iconCell/2,navCenters[i]-iconCell/2,
                                     iconCell,RGB(245,245,245)))
                DrawVectorIconGdiPlus(dc,i,cx-iconCell/2,navCenters[i]-iconCell/2,iconCell,RGB(245,245,245));
        }
        // Menu glyph is a rounded Win10-style hamburger; its hit target opens the scope menu.
        DrawVectorIconGdiPlus(dc,5,cx-Scale(12),Scale(12),Scale(24),RGB(245,245,245));
        GdiObject pen(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID,(std::max)(1,Scale(1)),RGB(245,245,245))));
        SelectedGdiObject selectedPen(dc,pen.value);
        const int clearY=railBottomRef-Scale(20),clearR=Scale(5);
        MoveToEx(dc,cx-clearR,clearY-clearR,nullptr);LineTo(dc,cx+clearR,clearY+clearR);
        MoveToEx(dc,cx+clearR,clearY-clearR,nullptr);LineTo(dc,cx-clearR,clearY+clearR);

        GdiObject font(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(wideBoxMode_ ? 15 : 14),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI")));
        SelectedGdiObject selectedFont(dc,font.value);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, settingsPage?RGB(245,245,245):RGB(32,32,32));
        const int listClipState = SaveDC(dc);
        const int clipRight = settingsPage ? client.right-Scale(2) : client.right-Scale(8);
        if (listClipState) IntersectClipRect(dc,rail,ListTop(),clipRight,footerTop);
        int cursor = ListTop() - scrollOffset_;
        bool settingsHeader = false;
        if (settingsPage) {
            DrawSettingsPage(dc,client,rail);
        } else if (scope_ == SearchScope::All && query_.empty()) {
            DrawHomePage(dc,client,rail);
        } else for (size_t i=0;i<results_.size();++i) {
            const SearchEntry& item = results_[i];
            if (item.kind == ResultKind::Setting && !settingsHeader) {
                RECT header{rail+Scale(48)-static_cast<int>(client.right*0.065f),cursor,client.right,cursor+settingsHeaderHeight_};
                if (wideBoxMode_) {
                    GdiObject headerFont(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(13),0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,
                        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI")));
                    SelectedGdiObject selectedHeaderFont(dc,headerFont.value);
                    DrawTextW(dc,Text(UiLabel::Settings),-1,&header,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
                } else {
                    DrawTextW(dc,Text(UiLabel::Settings),-1,&header,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
                }
                cursor += settingsHeaderHeight_;
                settingsHeader = true;
            }
            const int rowHeight = i < rowHeights_.size() ? rowHeights_[i] : Scale(44);
            RECT row{rail,cursor,client.right,cursor+rowHeight};
            if(row.bottom<=ListTop()||row.top>=footerTop){cursor+=rowHeight;continue;}
            if (static_cast<int>(i) == selected_) FillRect(dc,&row,reinterpret_cast<HBRUSH>(selectedBrush.value));
            else if(static_cast<int>(i)==hoveredResult_){
                GdiObject hoverBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(235,245,252))));
                if(hoverBrush.value)FillRect(dc,&row,reinterpret_cast<HBRUSH>(hoverBrush.value));
            }
            const int glyphSize = Scale(wideBoxMode_ ? 16 : 22);
            const int glyphX = rail+Scale(wideBoxMode_ ? 13 : 10), glyphY = cursor+(rowHeight-glyphSize)/2;
            const int appIconSize = Scale(32);
            const int appIconX = rail+Scale(8), appIconY = cursor+(rowHeight-appIconSize)/2;
            if(item.kind==ResultKind::Setting || item.kind==ResultKind::SettingsHome) {
                DrawSettingsCategoryIcon(dc,item.settingsIndex,glyphX+glyphSize/2,glyphY+glyphSize/2,
                                         glyphSize,RGB(0,120,215));
            } else if(item.kind==ResultKind::ControlPanel) {
                DrawVectorIconGdiPlus(dc,8,appIconX,appIconY,appIconSize,RGB(0,120,215));
            } else if(item.kind==ResultKind::Run) {
                DrawVectorIconGdiPlus(dc,7,appIconX,appIconY,appIconSize,RGB(90,90,90));
            } else if(item.isExplorerShortcut) {
                DrawVectorIconGdiPlus(dc,6,appIconX,appIconY,appIconSize,RGB(0,120,215));
            } else if(item.icon) {
                // App HICONs retain their alpha/mask through DrawIconEx; extraction requested a larger raster.
                DrawIconEx(dc,appIconX,appIconY,item.icon,appIconSize,appIconSize,0,nullptr,DI_NORMAL);
            } else {
                if(!DrawMdl2GlyphGdiPlus(dc,L'\xE71D',appIconX,appIconY,appIconSize,RGB(90,90,90)))
                    DrawVectorIconGdiPlus(dc,1,appIconX,appIconY,appIconSize,RGB(90,90,90));
            }
            RECT text{rail+Scale(48),cursor+Scale(2),client.right-Scale(10),cursor+rowHeight-Scale(2)};
            SetTextColor(dc,RGB(32,32,32));
            if(!wideBoxMode_ || !DrawNameWithMatch(dc,item.name,text))
                DrawTextW(dc,item.name.c_str(),-1,&text,DT_LEFT|DT_VCENTER|DT_WORDBREAK|DT_END_ELLIPSIS);
            cursor += rowHeight;
        }
        if(scope_==SearchScope::Apps&&query_.empty()&&!catalogReady_){
            RECT loading{rail+Scale(14),ListTop()+Scale(12),client.right-Scale(16),ListTop()+Scale(42)};
            SetTextColor(dc,RGB(90,90,90));DrawTextW(dc,Text(UiLabel::Indexing),-1,&loading,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        }
        if (!settingsPage && !(scope_ == SearchScope::All && query_.empty()) &&
            results_.empty() && !query_.empty()) {
            RECT empty{rail+Scale(12),ListTop()-scrollOffset_,client.right-Scale(12),ListTop()+Scale(36)-scrollOffset_};
            SetTextColor(dc,RGB(32,32,32));
            DrawTextW(dc,Text(UiLabel::NoResults),-1,&empty,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        }
        if (listClipState) RestoreDC(dc,listClipState);

        int trackTop = 0, trackBottom = 0, thumbTop = 0, thumbHeight = 0;
        if (!settingsPage && GetScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight)) {
            RECT track{client.right-Scale(7),trackTop,client.right-Scale(2),trackBottom};
            GdiObject trackBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(238,238,238))));
            GdiObject thumbBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(150,150,150))));
            if (trackBrush.value && thumbBrush.value) {
                FillRect(dc,&track,reinterpret_cast<HBRUSH>(trackBrush.value));
                RECT thumb{track.left,thumbTop,track.right,thumbTop+thumbHeight};
                FillRect(dc,&thumb,reinterpret_cast<HBRUSH>(thumbBrush.value));
            }
        }

        const bool webEnabled = g_webSearchEnabled.load();
        const int footerMid = webEnabled ? rail+(client.right-rail)/2 : static_cast<int>(client.right);
        RECT myStuffTab{rail,footerTop,footerMid,bodyBottom};
        RECT webTab{footerMid,footerTop,client.right,bodyBottom};
        GdiObject tabSelected(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(248,248,248))));
        GdiObject tabHover(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(235,243,249))));
        if(tabSelected.value){FillRect(dc,&myStuffTab,reinterpret_cast<HBRUSH>(tabSelected.value));FillRect(dc,&webTab,reinterpret_cast<HBRUSH>(tabSelected.value));}
        if(hoveredFooter_==0&&tabHover.value)FillRect(dc,&myStuffTab,reinterpret_cast<HBRUSH>(tabHover.value));
        if(hoveredFooter_==1&&tabHover.value)FillRect(dc,&webTab,reinterpret_cast<HBRUSH>(tabHover.value));
        {
            GdiObject separator(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID,1,RGB(220,220,220))));
            SelectedGdiObject selectedSeparator(dc,separator.value);
            if (separator.value) {
                MoveToEx(dc,rail,footerTop,nullptr);
                LineTo(dc,client.right,footerTop);
            }
        }
        const int logoY = footerTop + Scale(wideBoxMode_ ? 14 : 9);
        const int webY = footerTop + Scale(wideBoxMode_ ? 13 : 8);
        if(!DrawEmbeddedWin10Logo(dc,rail+Scale(8),logoY,Scale(16)))
            DrawVectorIconGdiPlus(dc,3,rail+Scale(8),logoY,Scale(16),RGB(0,120,215));
        if (webEnabled) DrawVectorIconGdiPlus(dc,4,footerMid+Scale(8),webY,Scale(18),RGB(95,95,95));
        {
            GdiObject footerFont(reinterpret_cast<HGDIOBJ>(CreateFontW(-Scale(wideBoxMode_ ? 14 : 12),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI")));
            SelectedGdiObject selectedFooterFont(dc,footerFont.value);
            SetTextColor(dc,RGB(40,40,40));
            RECT myText{rail+Scale(29),footerTop,footerMid-Scale(2),bodyBottom};
            RECT webText{footerMid+Scale(30),footerTop,client.right-Scale(3),bodyBottom};
            DrawTextW(dc,Text(UiLabel::MyStuff),-1,&myText,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
            if (webEnabled) DrawTextW(dc,Text(UiLabel::Web),-1,&webText,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        }
        if (!wideBoxMode_) {
            RECT queryText{rail+Scale(12),queryTop+Scale(1),client.right-Scale(8),client.bottom-Scale(1)};
            if(query_.empty()) {
                SetTextColor(dc,RGB(120,120,120));
                DrawTextW(dc,Text(UiLabel::SearchPlaceholder),-1,&queryText,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
                if(caretVisible_){
                    GdiObject caret(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID,1,RGB(20,20,20))));
                    SelectedGdiObject selectedCaret(dc,caret.value);
                    if(caret.value){MoveToEx(dc,queryText.left,queryText.top+Scale(9),nullptr);LineTo(dc,queryText.left,queryText.bottom-Scale(9));}
                }
            } else {
                SetTextColor(dc,RGB(32,32,32));
                DrawTextW(dc,query_.c_str(),-1,&queryText,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
                SIZE before{};GetTextExtentPoint32W(dc,query_.c_str(),static_cast<int>((std::min)(caretPos_,query_.size())),&before);
                if(hasSelection_){
                    const size_t a=(std::min)(selectionAnchor_,selectionEnd_),b=(std::max)(selectionAnchor_,selectionEnd_);
                    SIZE sa{},sb{};GetTextExtentPoint32W(dc,query_.c_str(),static_cast<int>(a),&sa);GetTextExtentPoint32W(dc,query_.c_str(),static_cast<int>(b),&sb);
                    RECT selection{queryText.left+sa.cx,queryText.top+Scale(5),queryText.left+sb.cx,queryText.bottom-Scale(5)};
                    GdiObject selectionBrush(reinterpret_cast<HGDIOBJ>(CreateSolidBrush(RGB(0,120,215))));
                    if(selectionBrush.value)FillRect(dc,&selection,reinterpret_cast<HBRUSH>(selectionBrush.value));
                    RECT selectedText{selection.left,queryText.top,selection.right,queryText.bottom};SetTextColor(dc,RGB(255,255,255));
                    DrawTextW(dc,query_.substr(a,b-a).c_str(),static_cast<int>(b-a),&selectedText,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
                    SetTextColor(dc,RGB(32,32,32));
                }
                if(caretVisible_&&!hasSelection_){
                    const int caretX=(std::min)(
                        static_cast<int>(queryText.right)-Scale(1),
                        static_cast<int>(queryText.left)+static_cast<int>(before.cx));
                    GdiObject caret(reinterpret_cast<HGDIOBJ>(CreatePen(PS_SOLID,1,RGB(20,20,20))));
                    SelectedGdiObject selectedCaret(dc,caret.value);
                    if(caret.value){MoveToEx(dc,caretX,queryText.top+Scale(9),nullptr);LineTo(dc,caretX,queryText.bottom-Scale(9));}
                }
            }
        }
    }

    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_HOTKEY:
            if (wParam == kHotkeyId) { Toggle(); return 0; }
            break;
        case kRefreshHotkeyMessage:
            UpdateHotkey(); UpdateWebVisibility(); SyncTaskbarBox(); return 0;
        case kToggleMessage:
            Toggle(); return 0;
        case kTaskbarBoxClickMessage:
            if (!visible_) {
                Show();
            } else {
                ForceForeground(hwnd_);
                caretPos_ = CaretFromBoxPixel(static_cast<int>(wParam));
                selectionAnchor_ = selectionEnd_ = caretPos_;
                hasSelection_ = false;
                caretVisible_ = true;
                InvalidatePane();
            }
            return 0;
        case WM_SETTINGCHANGE:
        case WM_SYSCOLORCHANGE:
        case WM_THEMECHANGED:
            SyncTaskbarBox();
            InvalidatePane();
            break;
        case kCatalogReadyMessage: {
            std::unique_ptr<std::vector<SearchEntry>> catalog(reinterpret_cast<std::vector<SearchEntry>*>(lParam));
            if(catalog)apps_=std::move(*catalog);
            catalogReady_=true;Filter();InvalidatePane();
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && visible_) {
                const HWND foreground = GetForegroundWindow();
                if (foreground && foreground == taskbarBoxHwnd_) {
                    ForceForeground(hwnd_);
                    return 0;
                }
                wchar_t foregroundClass[64]{};
                if (foreground) GetClassNameW(foreground, foregroundClass, ARRAYSIZE(foregroundClass));
                Wh_Log(L"[SearchOnly] pane deactivated; foreground window class='%s'", foregroundClass);
                Hide();
            }
            return 0;
        case WM_TIMER:
            if(wParam==kCaretTimer){caretVisible_=!caretVisible_;InvalidatePane();return 0;}
            if(wParam==kSlideTimer&&slideActive_){ UpdateSlideAnimation(); return 0; }
            if(wParam==kTaskbarSyncTimer){ SyncTaskbarBox(); return 0; }
            break;
        case WM_CHAR:
            if (customMenuOpen_) return 0;
            if(wParam>=0xD800&&wParam<=0xDBFF){pendingHigh_=static_cast<wchar_t>(wParam);pendingHighSurrogate_=true;return 0;}
            if(wParam>=0xDC00&&wParam<=0xDFFF){if(pendingHighSurrogate_){std::wstring pair;pair.push_back(pendingHigh_);pair.push_back(static_cast<wchar_t>(wParam));pendingHighSurrogate_=false;ReplaceSelection(pair);}return 0;}
            pendingHighSurrogate_=false;
            if(wParam>=32&&wParam!=127){std::wstring one(1,static_cast<wchar_t>(wParam));ReplaceSelection(one);}
            return 0;
        case WM_KEYDOWN: {
            if (customMenuOpen_) {
                if (wParam == VK_ESCAPE) { CloseCustomMenu(); return 0; }
                if (wParam == VK_RETURN) { ExecuteCustomMenuAction(customMenuHover_); return 0; }
                if ((wParam == VK_UP || wParam == VK_DOWN) && !customMenuActions_.empty()) {
                    const int count = static_cast<int>(customMenuActions_.size());
                    customMenuHover_ = (customMenuHover_ + (wParam == VK_DOWN ? 1 : count - 1)) % count;
                    InvalidateRect(hwnd_, &customMenuRect_, FALSE);
                }
                return 0;
            }
            const bool ctrl=(GetKeyState(VK_CONTROL)&0x8000)!=0,shift=(GetKeyState(VK_SHIFT)&0x8000)!=0;
            if(ctrl&&wParam=='A'){selectionAnchor_=0;selectionEnd_=query_.size();caretPos_=selectionEnd_;hasSelection_=!query_.empty();InvalidatePane();return 0;}
            if(ctrl&&wParam=='C'){CopySelection();return 0;}
            if(ctrl&&wParam=='V'){ReplaceSelection(ReadClipboardText());return 0;}
            if(wParam==VK_ESCAPE){Hide();return 0;}
            if(wParam==VK_BACK){DeletePrevious();return 0;}
            if(wParam==VK_DELETE){DeleteNext();return 0;}
            if(wParam==VK_HOME){MoveCaret(0,shift);return 0;}
            if(wParam==VK_END){MoveCaret(query_.size(),shift);return 0;}
            if(wParam==VK_LEFT){size_t p=caretPos_;if(hasSelection_&&!shift)p=(std::min)(selectionAnchor_,selectionEnd_);else if(p>0){--p;if(p>0&&query_[p]>=0xDC00&&query_[p]<=0xDFFF&&query_[p-1]>=0xD800&&query_[p-1]<=0xDBFF)--p;}MoveCaret(p,shift);return 0;}
            if(wParam==VK_RIGHT){size_t p=caretPos_;if(hasSelection_&&!shift)p=(std::max)(selectionAnchor_,selectionEnd_);else if(p<query_.size()){++p;if(p<query_.size()&&query_[p-1]>=0xD800&&query_[p-1]<=0xDBFF&&query_[p]>=0xDC00&&query_[p]<=0xDFFF)++p;}MoveCaret(p,shift);return 0;}
            if(wParam==VK_UP||wParam==VK_DOWN||wParam==VK_PRIOR||wParam==VK_NEXT){
                if(!results_.empty()){const int n=static_cast<int>(results_.size());int step=(wParam==VK_UP?-1:wParam==VK_DOWN?1:wParam==VK_PRIOR?-8:8);selected_=(std::max)(0,(std::min)(n-1,selected_+step));EnsureSelectedVisible();InvalidateRect(hwnd_,nullptr,FALSE);}return 0;
            }
            if(wParam==VK_RETURN){OpenSelected();return 0;}
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (customMenuOpen_) return 0;
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            RECT client{}; GetClientRect(hwnd_, &client);
            int trackTop=0,trackBottom=0,thumbTop=0,thumbHeight=0;
            const bool settingsPage = scope_ == SearchScope::Settings && query_.empty();
            const bool hasScrollbar = x >= client.right-Scale(10) &&
                (settingsPage
                    ? GetSettingsScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight)
                    : GetScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight));
            if (hasScrollbar && y >= trackTop && y < trackBottom &&
                y >= thumbTop && y < thumbTop+thumbHeight) {
                scrollbarDragging_ = true;
                scrollbarDragStartY_ = y;
                scrollbarDragStartOffset_ = settingsPage ? settingsScrollOffset_ : scrollOffset_;
                SetCapture(hwnd_);
                return 0;
            }
            if(x>=Scale(kRailWidth96)&&y>=client.bottom-QueryHeight()){
                SetFocus(hwnd_);caretPos_=CaretFromPixel(x);selectionAnchor_=selectionEnd_=caretPos_;
                hasSelection_=false;textSelecting_=true;caretVisible_=true;SetCapture(hwnd_);
                InvalidateRect(hwnd_,nullptr,FALSE);return 0;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (customMenuOpen_) {
                TRACKMOUSEEVENT tme{}; tme.cbSize = sizeof(tme); tme.dwFlags = TME_LEAVE; tme.hwndTrack = hwnd_;
                TrackMouseEvent(&tme);
                const int hit = CustomMenuHit(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                if (hit != customMenuHover_) { customMenuHover_ = hit; InvalidateRect(hwnd_, &customMenuRect_, FALSE); }
                return 0;
            }
            if (scrollbarDragging_) {
                int trackTop=0,trackBottom=0,thumbTop=0,thumbHeight=0;
                const bool settingsPage = scope_ == SearchScope::Settings && query_.empty();
                const bool hasScrollbar = settingsPage
                    ? GetSettingsScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight)
                    : GetScrollbarGeometry(trackTop,trackBottom,thumbTop,thumbHeight);
                if (hasScrollbar) {
                    const int maxScroll = settingsPage
                        ? settingsContentHeight_-ListViewportHeight()
                        : TotalListContentHeight()-ListViewportHeight();
                    const int travel = (std::max)(1,trackBottom-trackTop-thumbHeight);
                    const int offset = scrollbarDragStartOffset_ + MulDiv(
                        GET_Y_LPARAM(lParam)-scrollbarDragStartY_,maxScroll,travel);
                    if (settingsPage) settingsScrollOffset_ = (std::max)(0,(std::min)(offset,maxScroll));
                    else { scrollOffset_ = offset; ClampScrollOffset(); }
                    InvalidateRect(hwnd_,nullptr,FALSE);
                }
                return 0;
            }
            if(textSelecting_){
                RECT client{};GetClientRect(hwnd_,&client);
                const int qtop=client.bottom-QueryHeight();
                if(GET_Y_LPARAM(lParam)>=qtop){caretPos_=CaretFromPixel(GET_X_LPARAM(lParam));selectionEnd_=caretPos_;hasSelection_=selectionAnchor_!=selectionEnd_;caretVisible_=true;InvalidateRect(hwnd_,nullptr,FALSE);}
                return 0;
            }
            TRACKMOUSEEVENT tme{}; tme.cbSize=sizeof(tme); tme.dwFlags=TME_LEAVE; tme.hwndTrack=hwnd_;
            TrackMouseEvent(&tme);
            const int x=GET_X_LPARAM(lParam), y=GET_Y_LPARAM(lParam);
            RECT client{}; GetClientRect(hwnd_,&client);
            const int rail=Scale(kRailWidth96), queryTop=client.bottom-QueryHeight();
            const int footerTop=queryTop-FooterHeight();
            int newNav=-1,newFooter=-1,newHome=-1,newSettings=-1,newResult=-1;
            if(x<rail){
                if(y>=client.bottom-bottomInset_-Scale(40)&&y<client.bottom-bottomInset_)newNav=4;
                else if(y<Scale(48))newNav=3;
                else if(y<Scale(96))newNav=0;
                else if(y<Scale(144))newNav=1;
                else if(y<Scale(192))newNav=2;
            }
            if(y>=footerTop&&y<queryTop&&x>=rail){
                const bool webEnabled = g_webSearchEnabled.load();
                const int mid=webEnabled ? rail+(client.right-rail)/2 : static_cast<int>(client.right);
                newFooter=(webEnabled && x>=mid)?1:0;
            }
            if(scope_==SearchScope::All&&query_.empty()){
                POINT pt{x,y}; if(PtInRect(&homeAppsCard_,pt))newHome=0; else if(PtInRect(&homeSettingsCard_,pt))newHome=1;
            } else if(scope_==SearchScope::Settings&&query_.empty()){
                POINT pt{x,y}; for(size_t i=0;i<settingsCategoryRects_.size();++i)if(PtInRect(&settingsCategoryRects_[i],pt)){newSettings=static_cast<int>(i);break;}
            } else if(x>=rail)newResult=ResultAtPoint(y);
            if(newNav!=hoveredNav_||newFooter!=hoveredFooter_||newHome!=hoveredHome_||
               newSettings!=hoveredSettings_||newResult!=hoveredResult_){
                hoveredNav_=newNav;hoveredFooter_=newFooter;hoveredHome_=newHome;
                hoveredSettings_=newSettings;hoveredResult_=newResult;
                InvalidateRect(hwnd_,nullptr,FALSE);
            }
            break;
        }
        case WM_MOUSELEAVE:
            if (customMenuOpen_ && customMenuHover_ != -1) { customMenuHover_ = -1; InvalidateRect(hwnd_, &customMenuRect_, FALSE); }
            if(hoveredNav_!=-1||hoveredFooter_!=-1||hoveredHome_!=-1||hoveredSettings_!=-1||hoveredResult_!=-1){
                hoveredNav_=hoveredFooter_=hoveredHome_=hoveredSettings_=hoveredResult_=-1;
                InvalidateRect(hwnd_,nullptr,FALSE);
            }
            return 0;
        case WM_LBUTTONUP: {
            if (customMenuOpen_) {
                const int hit = CustomMenuHit(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                if (hit >= 0) ExecuteCustomMenuAction(hit); else CloseCustomMenu();
                return 0;
            }
            if (scrollbarDragging_) {
                scrollbarDragging_ = false;
                if (GetCapture() == hwnd_) ReleaseCapture();
                return 0;
            }
            if(textSelecting_){textSelecting_=false;if(GetCapture()==hwnd_)ReleaseCapture();return 0;}
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            RECT client{};
            GetClientRect(hwnd_, &client);
            const int rail = Scale(kRailWidth96);
            if (x < rail) {
                if (y >= client.bottom-bottomInset_-Scale(40) && y < client.bottom-bottomInset_) {
                    // Route through documented WM_CLOSE; this pane animates closed
                    // instead of destroying its Explorer-owned window immediately.
                    if (!PostMessageW(hwnd_,WM_CLOSE,0,0)) Hide();
                } else if (y < Scale(48)) {
                    ShowScopeMenu(POINT{x,y});
                } else if (y < Scale(96)) {
                    SetScope(SearchScope::All);
                } else if (y < Scale(144)) {
                    SetScope(SearchScope::Apps);
                } else if (y < Scale(192)) {
                    SetScope(SearchScope::Settings);
                }
                return 0;
            }
            if (y >= ListBottom() && y < client.bottom-QueryHeight()) {
                const bool webEnabled = g_webSearchEnabled.load();
                const int mid=webEnabled ? rail+(client.right-rail)/2 : static_cast<int>(client.right);
                if(!webEnabled || x<mid) SetScope(SearchScope::All);
                else if(!query_.empty()) SearchWeb();
                else SetFocus(hwnd_);
                return 0;
            }
            if (scope_ == SearchScope::Settings && query_.empty()) {
                POINT point{x,y};
                for(int i=0;i<static_cast<int>(settingsCategoryRects_.size());++i){
                    if(PtInRect(&settingsCategoryRects_[i],point)){
                        const wchar_t* uri=SettingsUri(i);
                        Hide();
                        ShellExecuteW(nullptr,L"open",uri,nullptr,nullptr,SW_SHOWNORMAL);
                        return 0;
                    }
                }
                if (x >= client.right-Scale(10) && y >= ListTop() && y < ListBottom() &&
                    settingsContentHeight_ > ListViewportHeight()) {
                    const int page=(std::max)(Scale(44),ListViewportHeight());
                    settingsScrollOffset_ += y < (ListTop()+ListBottom())/2 ? -page : page;
                    settingsScrollOffset_=(std::max)(0,(std::min)(settingsScrollOffset_,
                        settingsContentHeight_-ListViewportHeight()));
                    InvalidateRect(hwnd_,nullptr,FALSE);
                    return 0;
                }
                SetFocus(hwnd_);
                return 0;
            }
            if (scope_ == SearchScope::All && query_.empty()) {
                POINT point{x,y};
                if (PtInRect(&homeAppsCard_,point)) SetScope(SearchScope::Apps);
                else if (PtInRect(&homeSettingsCard_,point)) SetScope(SearchScope::Settings);
                else SetFocus(hwnd_);
                return 0;
            }
            if (y >= client.bottom-QueryHeight()) {
                SetFocus(hwnd_);caretPos_=query_.size();ClearSelection();caretVisible_=true;InvalidateRect(hwnd_,nullptr,FALSE);
                return 0;
            }
            if (x >= client.right-Scale(10) && y >= ListTop() && y < ListBottom() &&
                TotalListContentHeight() > ListViewportHeight()) {
                const int page = (std::max)(Scale(44), ListViewportHeight());
                scrollOffset_ += y < (ListTop()+ListBottom())/2 ? -page : page;
                ClampScrollOffset();
                InvalidateRect(hwnd_,nullptr,FALSE);
                return 0;
            }
            const int index = ResultAtPoint(y);
            if (index >= 0) { selected_ = index; OpenSelected(); }
            else SetFocus(hwnd_);
            return 0;
        }
        case WM_CAPTURECHANGED:
            scrollbarDragging_ = false;textSelecting_=false;
            return 0;
        case WM_MOUSEWHEEL: {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (scope_ == SearchScope::Settings && query_.empty()) {
                settingsScrollOffset_ -= MulDiv(delta,Scale(3*44),WHEEL_DELTA);
                settingsScrollOffset_=(std::max)(0,(std::min)(settingsScrollOffset_,
                    (std::max)(0,settingsContentHeight_-ListViewportHeight())));
            } else {
                scrollOffset_ -= MulDiv(delta, Scale(3*44), WHEEL_DELTA);
                ClampScrollOffset();
            }
            InvalidateRect(hwnd_,nullptr,FALSE);
            return 0;
        }
        case WM_DPICHANGED: {
            dpi_ = LOWORD(wParam) ? LOWORD(wParam) : 96;
            RECT* suggested = reinterpret_cast<RECT*>(lParam);
            HMONITOR monitor = suggested ? MonitorFromRect(suggested, MONITOR_DEFAULTTONEAREST)
                                         : MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info{}; info.cbSize = sizeof(info);
            if (monitor && GetMonitorInfoW(monitor, &info)) {
                RefreshTaskbarSearchState(monitor);
                ApplyMonitorInfo(info);
                paneX_ = static_cast<int>(workArea_.left);
                paneWidth_ = ComputePaneWidth(workArea_.right - workArea_.left);
                const int provisionalHeight = panelHeight_ > 0 ? panelHeight_ :
                    static_cast<int>((std::max)(1L, workArea_.bottom-workArea_.top));
                SetWindowPos(hwnd, nullptr, paneX_,
                    static_cast<int>(workArea_.bottom)-provisionalHeight, paneWidth_, provisionalHeight,
                    SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
                Filter();
                InvalidateRect(hwnd,nullptr,FALSE);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            if (customMenuOpen_) { CloseCustomMenu(); return 0; }
            const int index = ResultAtPoint(GET_Y_LPARAM(lParam));
            if (index >= 0) ShowContextMenu(index, POINT{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)});
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps{}; HDC dc=BeginPaint(hwnd_,&ps);
            if(dc){
                RECT client{};GetClientRect(hwnd_,&client);
                const int width=static_cast<int>(client.right),height=static_cast<int>(client.bottom);
                if(width>0&&height>0){
                    MemoryDc buffer(CreateCompatibleDC(dc));
                    GdiObject bitmap(reinterpret_cast<HGDIOBJ>(CreateCompatibleBitmap(dc,width,height)));
                    if(buffer.value&&bitmap.value){
                        SelectedGdiObject selected(buffer.value,bitmap.value);
                        Draw(buffer.value);
                        DrawCustomMenu(buffer.value);
                        BitBlt(dc,0,0,width,height,buffer.value,0,0,SRCCOPY);
                    }else { Draw(dc); DrawCustomMenu(dc); }
                }
            }
            EndPaint(hwnd_,&ps); return 0;
        }
        case WM_SETCURSOR: {
            if(LOWORD(lParam)==HTCLIENT){
                POINT pt{}; GetCursorPos(&pt); ScreenToClient(hwnd_,&pt);
                RECT client{}; GetClientRect(hwnd_,&client);
                const int rail=Scale(kRailWidth96);
                const bool inQuery=pt.y>=client.bottom-QueryHeight() && pt.x>=rail;
                // Windows 10 keeps the ordinary arrow over navigation/options; only
                // the text-entry field uses the I-beam. Never show a hand cursor.
                SetCursor(LoadCursorW(nullptr,inQuery?IDC_IBEAM:IDC_ARROW));
                return TRUE;
            }
            break;
        }
        case WM_ERASEBKGND: return 1;
        case WM_CLOSE: Hide(); return 0;
        case WM_NCDESTROY:
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            return DefWindowProcW(hwnd,message,wParam,lParam);
        }
        return DefWindowProcW(hwnd,message,wParam,lParam);
    }
};

bool MatchesSearchId(const wchar_t* text) {
    if (!text || !*text) return false;
    // Verified from real logs:
    // - "4103": Win10 taskbar search icon button (TrayButton)
    // - "4100" / "4101" / "4102": Win10 taskbar wide search box (TrayDummySearchControl and inner controls)
    if (wcscmp(text, L"4100") == 0 ||
        wcscmp(text, L"4101") == 0 ||
        wcscmp(text, L"4102") == 0 ||
        wcscmp(text, L"4103") == 0) {
        return true;
    }
    std::wstring s = Lower(text);
    return s.find(L"searchbutton") != std::wstring::npos ||
           s.find(L"searchbox") != std::wstring::npos ||
           s.find(L"searchicon") != std::wstring::npos ||
           s.find(L"traydummysearchcontrol") != std::wstring::npos ||
           s.find(L"cortana") != std::wstring::npos;
}

bool IsTaskbarSearchButtonAt(POINT pt) {
    try {
        HWND h = WindowFromPoint(pt);
        if (h && h == g_taskbarBoxWindow.load()) return true;
        HWND root = h ? GetAncestor(h, GA_ROOT) : nullptr;
        if (!root) return false;
        if (root == g_taskbarBoxWindow.load()) return true;
        wchar_t cls[64]{};
        GetClassNameW(root, cls, ARRAYSIZE(cls));
        if (wcscmp(cls, L"Shell_TrayWnd") != 0 && wcscmp(cls, L"Shell_SecondaryTrayWnd") != 0)
            return false;

        if (HWND box = g_taskbarBoxWindow.load(); box && IsWindowVisible(box)) {
            RECT boxRect{};
            if (GetWindowRect(box, &boxRect) && PtInRect(&boxRect, pt))
                return true;
        }

        if (g_uia ||
            SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(g_uia.GetAddressOf())))) {
            ComPtr<IUIAutomationElement> element;
            if (SUCCEEDED(g_uia->ElementFromPoint(pt, element.GetAddressOf())) && element) {
                ComPtr<IUIAutomationTreeWalker> walker;
                g_uia->get_ControlViewWalker(walker.GetAddressOf());

                // In box mode the click lands on a child (e.g. id="4101") inside the search
                // box container (id="4100", class="TrayDummySearchControl"), so check the
                // clicked element and up to 4 ancestors by both AutomationId and ClassName.
                for (int depth = 0; depth < 5 && element; ++depth) {
                    BSTR id = nullptr;
                    element->get_CurrentAutomationId(&id);
                    BSTR uiaCls = nullptr;
                    element->get_CurrentClassName(&uiaCls);
                    Wh_Log(L"[SearchOnly] click chain depth=%d id='%s' class='%s'",
                           depth, id ? id : L"", uiaCls ? uiaCls : L"");
                    const bool match = MatchesSearchId(id) || MatchesSearchId(uiaCls);
                    SysFreeString(id);
                    SysFreeString(uiaCls);
                    if (match) return true;
                    if (!walker) break;
                    ComPtr<IUIAutomationElement> parent;
                    if (FAILED(walker->GetParentElement(element.Get(), parent.GetAddressOf()))) break;
                    element = parent;
                }
            }
        }

        // Fallback: inspect the Win32 HWND chain up to the tray root for the wide search box
        // (TrayDummySearchControl) or search control IDs (4100..4103).
        for (HWND cur = h; cur && cur != root; cur = GetParent(cur)) {
            wchar_t childCls[64]{};
            GetClassNameW(cur, childCls, ARRAYSIZE(childCls));
            const int ctrlId = GetDlgCtrlID(cur);
            if (MatchesSearchId(childCls) || (ctrlId >= 4100 && ctrlId <= 4103)) {
                Wh_Log(L"[SearchOnly] click matched HWND class='%s' ctrlId=%d", childCls, ctrlId);
                return true;
            }
        }
    } catch (const std::bad_alloc&) {
        Wh_Log(L"[SearchOnly] allocation failure in IsTaskbarSearchButtonAt");
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in IsTaskbarSearchButtonAt");
    }
    return false;
}

LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    try {
        if (code == HC_ACTION && g_interceptEnabled.load() && !g_stopping.load()) {
            const auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            if (ms && !(ms->flags & LLMHF_INJECTED)) {
                if (wParam == WM_LBUTTONDOWN && g_paneWindow.load() && IsTaskbarSearchButtonAt(ms->pt)) {
                    g_swallowMouseUp = true;
                    if (HWND box = g_taskbarBoxWindow.load(); box && IsWindowVisible(box)) {
                        RECT boxRect{};
                        if (GetWindowRect(box, &boxRect) && PtInRect(&boxRect, ms->pt)) {
                            PostMessageW(g_paneWindow.load(), kTaskbarBoxClickMessage,
                                         static_cast<WPARAM>(ms->pt.x - boxRect.left),
                                         static_cast<LPARAM>(ms->pt.y - boxRect.top));
                            return 1;
                        }
                    }
                    PostMessageW(g_paneWindow.load(), kToggleMessage, 0, 0);
                    return 1;
                }
                if (wParam == WM_LBUTTONUP && g_swallowMouseUp) {
                    g_swallowMouseUp = false;
                    return 1;
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in MouseHookProc");
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam) {
    try {
        if (code == HC_ACTION && g_interceptEnabled.load() && !g_stopping.load() &&
            wParam == WM_KEYDOWN && g_paneWindow.load()) {
            const auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            const bool win = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
            const bool other = ((GetAsyncKeyState(VK_CONTROL) | GetAsyncKeyState(VK_MENU) |
                                 GetAsyncKeyState(VK_SHIFT)) & 0x8000) != 0;
            if (kb && !(kb->flags & LLKHF_INJECTED) && win && !other && (kb->vkCode == 'S' || kb->vkCode == 'Q')) {
                PostMessageW(g_paneWindow.load(), kToggleMessage, 0, 0);
                // Dummy key so that releasing Win does not open the Start menu.
                INPUT in[2]{};
                in[0].type = in[1].type = INPUT_KEYBOARD;
                in[0].ki.wVk = in[1].ki.wVk = 0xE8;
                in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, in, sizeof(INPUT));
                return 1;
            }
        }
    } catch (...) {
        Wh_Log(L"[SearchOnly] exception in KeyboardHookProc");
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD WINAPI UiThreadProc(void*) {
    MSG queue{};
    PeekMessageW(&queue,nullptr,WM_USER,WM_USER,PM_NOREMOVE);
    g_uiThreadId = GetCurrentThreadId();
    if(g_uiReadyEvent)SetEvent(g_uiReadyEvent);
    HMODULE user32=GetModuleHandleW(L"user32.dll");
    using SetDpiContextFn=DPI_AWARENESS_CONTEXT (WINAPI*)(DPI_AWARENESS_CONTEXT);
    auto setDpiContext=user32?reinterpret_cast<SetDpiContextFn>(GetProcAddress(user32,"SetThreadDpiAwarenessContext")):nullptr;
    if(setDpiContext)setDpiContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ULONG_PTR gdiplusToken=0;
    Gdiplus::GdiplusStartupInput gdiplusInput;
    try {
        const Gdiplus::Status gdiplusStatus=Gdiplus::GdiplusStartup(&gdiplusToken,&gdiplusInput,nullptr);
        if(gdiplusStatus!=Gdiplus::Ok) Wh_Log(L"[SearchOnly] GDI+ startup failed: %d",static_cast<int>(gdiplusStatus));
        else if(!InitializeEmbeddedWin10Logo()) Wh_Log(L"[SearchOnly] embedded Windows 10 logo could not be decoded");
        CoInitScope com;
        if (FAILED(com.hr)) Wh_Log(L"[SearchOnly] COM init failed: 0x%08X", static_cast<unsigned>(com.hr));
        SearchPane pane;
        if (!pane.Create(GetModuleHandleW(nullptr))) {
            Wh_Log(L"[SearchOnly] search window creation failed: %u", GetLastError());
            pane.Destroy();
            g_uia.Reset();
            UnregisterClassW(kTaskbarBoxClass, GetModuleHandleW(nullptr));
            UnregisterClassW(kLegacyTaskbarBoxClass, GetModuleHandleW(nullptr));
            UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
            ShutdownEmbeddedWin10Logo();
            if(gdiplusToken) Gdiplus::GdiplusShutdown(gdiplusToken);
            g_uiThreadId = 0;
            return 1;
        }
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, GetModuleHandleW(nullptr), 0);
        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandleW(nullptr), 0);
        MSG message{};
        while (!g_stopping.load()) {
            const BOOL result = GetMessageW(&message,nullptr,0,0);
            if (result <= 0) break;
            if (message.hwnd == nullptr && message.message == kRefreshHotkeyMessage) {
                pane.UpdateHotkey();
                pane.UpdateWebVisibility();
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
        if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }
        pane.Destroy();
        g_uia.Reset();
        UnregisterClassW(kTaskbarBoxClass, GetModuleHandleW(nullptr));
        UnregisterClassW(kLegacyTaskbarBoxClass, GetModuleHandleW(nullptr));
        UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
        ShutdownEmbeddedWin10Logo();
        if(gdiplusToken) Gdiplus::GdiplusShutdown(gdiplusToken);
        g_uiThreadId = 0;
        return 0;
    } catch (const std::bad_alloc&) {
        Wh_Log(L"[SearchOnly] allocation failure on UI thread");
    } catch (...) {
        Wh_Log(L"[SearchOnly] unexpected exception on UI thread");
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }
    g_uia.Reset();
    UnregisterClassW(kTaskbarBoxClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kLegacyTaskbarBoxClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
    ShutdownEmbeddedWin10Logo();
    if(gdiplusToken) Gdiplus::GdiplusShutdown(gdiplusToken);
    g_uiThreadId = 0;
    return 1;
}

} // namespace

bool IsMainExplorerProcess() {
    HWND shell=GetShellWindow();
    if(shell){
        DWORD shellPid=0;GetWindowThreadProcessId(shell,&shellPid);
        return shellPid==GetCurrentProcessId();
    }
    std::wstring command=GetCommandLineW();
    std::transform(command.begin(),command.end(),command.begin(),[](wchar_t c){return static_cast<wchar_t>(std::towlower(c));});
    return command.find(L" /factory")==std::wstring::npos&&
           command.find(L" /separate")==std::wstring::npos&&
           command.find(L" -embedding")==std::wstring::npos;
}

void Wh_ModUninit();

BOOL Wh_ModInit() {
    try {
        if(!IsMainExplorerProcess()){
            Wh_Log(L"[SearchOnly] not the Explorer shell process; skipping this instance");
            return FALSE;
        }
        g_stopping = false;
        g_searchHotkeyEnabled = Wh_GetIntSetting(L"searchHotkey") != 0;
        g_webSearchEnabled = Wh_GetIntSetting(L"webSearch") != 0;
        g_interceptEnabled = Wh_GetIntSetting(L"interceptSearch") != 0;
        g_overTaskbar = Wh_GetIntSetting(L"overTaskbar") != 0;
        g_uiReadyEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        if(!g_uiReadyEvent){Wh_Log(L"[SearchOnly] ready-event creation failed: %u",GetLastError());return FALSE;}
        g_uiThread = CreateThread(nullptr,0,UiThreadProc,nullptr,0,nullptr);
        if (!g_uiThread) {
            Wh_Log(L"[SearchOnly] UI thread creation failed: %u", GetLastError());
            CloseHandle(g_uiReadyEvent);g_uiReadyEvent=nullptr;
            return FALSE;
        }
        if(WaitForSingleObject(g_uiReadyEvent,5000)!=WAIT_OBJECT_0){
            Wh_Log(L"[SearchOnly] UI thread did not create its message queue in time");
            Wh_ModUninit();
            return FALSE;
        }
        return TRUE;
    } catch (...) {
        Wh_Log(L"[SearchOnly] unexpected exception in Wh_ModInit");
        return FALSE;
    }
}

void Wh_ModSettingsChanged() {
    try {
        g_searchHotkeyEnabled = Wh_GetIntSetting(L"searchHotkey") != 0;
        g_webSearchEnabled = Wh_GetIntSetting(L"webSearch") != 0;
        g_interceptEnabled = Wh_GetIntSetting(L"interceptSearch") != 0;
        g_overTaskbar = Wh_GetIntSetting(L"overTaskbar") != 0;
        const DWORD threadId = g_uiThreadId.load();
        if (threadId) PostThreadMessageW(threadId,kRefreshHotkeyMessage,0,0);
    } catch (...) {
        Wh_Log(L"[SearchOnly] unexpected exception in Wh_ModSettingsChanged");
    }
}

void Wh_ModUninit() {
    try {
        g_stopping = true;
        if(g_uiThread){
            while(WaitForSingleObject(g_uiThread,0)==WAIT_TIMEOUT){
                const DWORD threadId=g_uiThreadId.load();
                if(threadId&&PostThreadMessageW(threadId,WM_QUIT,0,0))break;
                Sleep(10);
            }
            WaitForSingleObject(g_uiThread,INFINITE);
            CloseHandle(g_uiThread);g_uiThread=nullptr;
        }
        if(g_uiReadyEvent){CloseHandle(g_uiReadyEvent);g_uiReadyEvent=nullptr;}
        g_uiThreadId=0;
    } catch (...) {
        Wh_Log(L"[SearchOnly] unexpected exception in Wh_ModUninit");
    }
}
