// ==WindhawkMod==
// @id              desktop-folder-groups
// @name            Desktop Folder Groups
// @name:it         Gruppi di cartelle sul desktop
// @description     Browse desktop folders in an anchored hover panel with view buttons, drag-out and native context menus.
// @description:it  Esplora le cartelle del desktop in un pannello al passaggio del mouse, con viste, trascinamento e menu contestuali.
// @version         0.10.0
// @author          Shjne
// @github          https://github.com/Shjne
// @include         explorer.exe
// @architecture    amd64
// @license         MIT
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lgdi32 -ladvapi32 -ldwmapi -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Folder Groups

Hover over a real folder on the Windows desktop to browse its contents in an
anchored panel. Use any folder name; no group configuration is needed.

## Usage

- Create a desktop folder and put shortcuts or other items inside it.
- Hover over its icon to open the panel; click an item once to open it.
- Switch between Large icons (two columns), Medium icons (three columns), and
  List using the buttons. The settings also offer Extra large and Small icons.
- Right-click an item for its classic Windows Shell context menu, including
  Properties, Copy, Cut, Delete and Rename when available. This is the classic
  menu, not the compact Windows 11 menu. Shift exposes extended commands.
- Drag an item out onto the desktop to move it. Hold Ctrl to copy; Esc cancels.
  Windows handles the transfer and name conflicts. Actual files and folders,
  not only shortcuts, can be moved by these actions.
- Move away to close the panel, or press Esc. Double-clicking the original
  desktop folder still opens it normally in Explorer.

The initial view comes from settings. Quick-button choices last until the mod
reloads. The panel refreshes its contents when reopened or after a context-menu
command. Its base size is 400 wide by 600 high; scale adjusts text and icons
with it. The panel fits within the current monitor's work area.

## Appearance and tooltips

The panel follows the Windows application light/dark setting and uses system
colors in high contrast. Native scrollbars remain controlled by Windows.
The optional tooltip suppression temporarily suspends desktop tooltips while
this panel is open and re-enables them when it closes or the mod unloads.
Other mods which manage the same tooltip control may interact with this option.
English and Italian interface text follows the Windows UI language.

## Requirements and limitations

- Windows 11, x64 Explorer with the standard SysListView32 desktop. Development
  testing used Windows 11 Pro 26H2, build 26300.9818. Other builds, ARM64 and
  replacement desktop shells have not been validated.
- Local fixed-drive folders only. Virtual desktop items, folder shortcuts,
  network/removable locations and reparse-point folders are excluded, including
  some OneDrive configurations.
- Up to 100 non-hidden, non-system items are shown. One item can be dragged or
  operated on at a time. Drops into this panel and background context menus
  are not implemented; use the real folder in Explorer to add items.
- Shell icon extraction and commands run on a dedicated STA thread. Slow icon
  targets or Shell extensions can delay the panel and mod unloading. Active
  file operations and Shell calls are allowed to finish before unloading.
- Icon quality depends on each application's resources and the Windows icon
  cache. Recreate stale packaged-app shortcuts from shell:AppsFolder if their
  package updates leave generic icons.
- The submitter reports successful desktop testing of version 0.10.0 on their
  Windows 11 PC. Mixed-DPI monitors, high contrast, other Windows builds and
  third-party Shell extensions have not been individually verified.

Disabling the mod restores the regular desktop UI. File operations explicitly
chosen in the panel are real Windows file operations and are not reversed by
uninstalling the mod.

## Italiano

Passa sopra una cartella reale del desktop per aprire il pannello. I nomi delle
cartelle sono liberi. Un clic apre una voce; il tasto destro apre il menu Shell
classico. Trascina sul desktop per spostare, tieni Ctrl per copiare, Esc per
annullare. La scala mantiene le proporzioni L400 x H600. Sono escluse cartelle
virtuali, di rete e reparse point; il limite e di 100 elementi. I comandi sui
file agiscono realmente sui file e Windows gestisce eventuali conflitti.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openDelay: 350
  $name: Open delay (ms)
  $name:it: Ritardo apertura (ms)
  $description: From 150 to 3000 milliseconds.
  $description:it: Da 150 a 3000 millisecondi.
- closeDelay: 550
  $name: Close delay (ms)
  $name:it: Ritardo chiusura (ms)
  $description: From 200 to 3000 milliseconds.
  $description:it: Da 200 a 3000 millisecondi.
- enableDragOut: true
  $name: Allow dragging items out
  $name:it: Consenti trascinamento dal pannello
  $description: Drag onto the desktop to move an item. Hold Ctrl to copy. Windows handles name conflicts.
  $description:it: Trascina sul desktop per spostare. Tieni Ctrl per copiare. Windows gestisce i conflitti di nomi.
- hideFolderTooltips: true
  $name: Hide desktop tooltips while the panel is open
  $name:it: Nascondi fumetti del desktop con pannello aperto
  $description: Temporarily suspends the desktop tooltip control and re-enables it when the panel closes.
  $description:it: Sospende temporaneamente i fumetti del desktop e li riattiva alla chiusura del pannello.
- panelScale: 100
  $name: Panel scale (%)
  $name:it: Scala pannello (%)
  $description: "60 to 200. 100 = 400 wide by 600 high, 18 px text. Icons depend on the view. The panel keeps its 2:3 proportions."
  $description:it: "Da 60 a 200. 100 = L400 x H600 e testi 18 px. Le icone dipendono dalla vista. Proporzioni 2:3."
- viewMode: list
  $name: Initial view
  $name:it: Visualizzazione iniziale
  $description: Quick buttons change the view for the current session only.
  $description:it: I pulsanti rapidi cambiano vista solo per la sessione corrente.
  $options:
  - extraLarge: Extra large icons
  - large: Large icons
  - medium: Medium icons
  - small: Small icons
  - list: List
  $options:it:
  - extraLarge: Icone molto grandi
  - large: Icone grandi
  - medium: Icone medie
  - small: Icone piccole
  - list: Elenco
*/
// ==/WindhawkModSettings==

// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Shjne

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <servprov.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <algorithm>
#include <string>
#include <vector>
#include <new>

namespace {
bool italianUI = false;
PCWSTR Text(PCWSTR english, PCWSTR italian) { return italianUI ? italian : english; }
// All UI and COM objects belong to the dedicated STA thread.
HANDLE stopEvent = nullptr, worker = nullptr;
HWND panel = nullptr, entries = nullptr, desktopList = nullptr;
HWND quickButtons[3]{};
constexpr int QuickButtonFirst = 101;
constexpr UINT_PTR ShutdownTimer = 0xDF01;
HFONT quickFont = nullptr;
double contentScale = 1.0;
void ResizeEntries(int width, int height);
IFolderView* folderView = nullptr;
IShellFolder* desktopFolder = nullptr;
HFONT font = nullptr;
HIMAGELIST icons = nullptr;
HBRUSH panelBrush = nullptr;
bool themeInitialized = false, themeApplying = false, themeDark = false, themeHighContrast = false;
COLORREF themeBackground = CLR_INVALID, themeForeground = CLR_INVALID;
ULONGLONG themeCheckAt = 0;
std::vector<std::wstring> targets;
std::wstring candidate, opened, dismissedFolder;
RECT anchor{};
ULONGLONG candidateSince = 0, outsideSince = 0, retryAt = 0;
int openDelay = 350, closeDelay = 550;
int panelScale = 100;
int iconPixels = 24;
bool enableDragOut = true, hideFolderTooltips = true, dragging = false;
bool contextMenuOpen = false, renaming = false;
IContextMenu2* activeMenu2 = nullptr;
IContextMenu3* activeMenu3 = nullptr;
ULONGLONG ignoreClickUntil = 0;
HWND suspendedDesktopTooltip = nullptr;

void RestoreDesktopTooltip() {
    if (IsWindow(suspendedDesktopTooltip))
        SendMessageW(suspendedDesktopTooltip, TTM_ACTIVATE, TRUE, 0);
    suspendedDesktopTooltip = nullptr;
}

void SuppressDesktopTooltip() {
    if (!hideFolderTooltips || !IsWindow(desktopList)) return;
    HWND tooltip = (HWND)SendMessageW(desktopList, LVM_GETTOOLTIPS, 0, 0);
    DWORD owner = 0;
    if (!IsWindow(tooltip)) return;
    GetWindowThreadProcessId(tooltip, &owner);
    if (owner != GetCurrentProcessId()) return;
    if (suspendedDesktopTooltip != tooltip) {
        RestoreDesktopTooltip();
        suspendedDesktopTooltip = tooltip;
    }
    SendMessageW(tooltip, TTM_ACTIVATE, FALSE, 0);
    SendMessageW(tooltip, TTM_POP, 0, 0);
}
enum class ViewMode { List, Small, Medium, Large, ExtraLarge };
ViewMode viewMode = ViewMode::List;
bool IconGrid() {
    return viewMode == ViewMode::Medium || viewMode == ViewMode::Large || viewMode == ViewMode::ExtraLarge;
}
int BaseIconPixels() {
    switch (viewMode) {
        case ViewMode::Small: return 16;
        case ViewMode::Medium: return 48;
        case ViewMode::Large: return 96;
        case ViewMode::ExtraLarge: return 256;
        default: return 24;
    }
}
int GridCellWidth() {
    RECT client{};
    GetClientRect(panel, &client);
    int columns = viewMode == ViewMode::Medium ? 3 : (viewMode == ViewMode::Large ? 2 : 1);
    // Reserve vertical-scrollbar space consistently, even for short folders.
    int available = std::max(12, (int)client.right - GetSystemMetrics(SM_CXVSCROLL) - 4);
    if (viewMode == ViewMode::Medium || viewMode == ViewMode::Large)
        return std::max(4, available / columns);
    return std::max(iconPixels + (int)(24 * contentScale), (int)(132 * contentScale));
}
bool escapeWasDown = false;
constexpr wchar_t WindowClass[] = L"Windhawk.DesktopFolderGroups.0.1";

void ApplyWindowsTheme(bool force = false) {
    if (!panel || !entries || themeApplying) return;
    HIGHCONTRASTW contrast{sizeof(contrast)};
    bool highContrast = SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON);
    DWORD light = 1, size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    bool dark = !highContrast && light == 0;
    COLORREF background = dark ? RGB(32, 32, 32) : GetSysColor(COLOR_WINDOW);
    COLORREF foreground = dark ? RGB(240, 240, 240) : GetSysColor(COLOR_WINDOWTEXT);
    if (!force && themeInitialized && dark == themeDark && highContrast == themeHighContrast &&
        background == themeBackground && foreground == themeForeground) return;
    themeApplying = true;
    HBRUSH brush = CreateSolidBrush(background);
    if (brush) {
        if (panelBrush) DeleteObject(panelBrush);
        panelBrush = brush;
    }
    themeDark = dark;
    themeHighContrast = highContrast;
    themeBackground = background;
    themeForeground = foreground;
    themeInitialized = true;
    BOOL darkTitle = dark;
    // DWMWA_USE_IMMERSIVE_DARK_MODE (Windows 11). No process-wide theme changes:
    // this code runs inside Explorer and only styles its own popup.
    DwmSetWindowAttribute(panel, 20, &darkTitle, sizeof(darkTitle));
    // Classic drawing in dark/high-contrast mode respects explicit list colors.
    // Native scrollbar/title-bar details remain under Windows control.
    SetWindowTheme(entries, dark || highContrast ? L"" : L"Explorer", nullptr);
    ListView_SetBkColor(entries, background);
    ListView_SetTextBkColor(entries, background);
    ListView_SetTextColor(entries, foreground);
    RedrawWindow(panel, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    themeApplying = false;
}

void ReleaseDesktop() {
    RestoreDesktopTooltip();
    desktopList = nullptr;
    if (desktopFolder) desktopFolder->Release();
    if (folderView) folderView->Release();
    desktopFolder = nullptr;
    folderView = nullptr;
}

bool ConnectDesktop() {
    ReleaseDesktop();
    DWORD owner = 0;
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) return false;
    GetWindowThreadProcessId(shellWindow, &owner);
    if (owner != GetCurrentProcessId()) return false;
    IShellWindows* windows = nullptr;
    IDispatch* dispatch = nullptr;
    IServiceProvider* provider = nullptr;
    IShellBrowser* browser = nullptr;
    IShellView* view = nullptr;
    VARIANT location{}, root{};
    long unused = 0;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                 IID_PPV_ARGS(&windows));
    if (SUCCEEDED(hr)) hr = windows->FindWindowSW(&location, &root, SWC_DESKTOP,
                              &unused, SWFO_NEEDDISPATCH, &dispatch);
    if (SUCCEEDED(hr) && dispatch) hr = dispatch->QueryInterface(IID_PPV_ARGS(&provider));
    else hr = E_FAIL;
    if (SUCCEEDED(hr)) hr = provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    if (SUCCEEDED(hr)) hr = browser->QueryActiveShellView(&view);
    if (SUCCEEDED(hr)) hr = view->QueryInterface(IID_PPV_ARGS(&folderView));
    if (SUCCEEDED(hr)) hr = folderView->GetFolder(IID_PPV_ARGS(&desktopFolder));
    HWND viewWindow = nullptr;
    if (SUCCEEDED(hr)) hr = view->GetWindow(&viewWindow);
    if (SUCCEEDED(hr)) desktopList = FindWindowExW(viewWindow, nullptr, L"SysListView32", nullptr);
    if (view) view->Release();
    if (browser) browser->Release();
    if (provider) provider->Release();
    if (dispatch) dispatch->Release();
    if (windows) windows->Release();
    DWORD listOwner = 0;
    if (desktopList) GetWindowThreadProcessId(desktopList, &listOwner);
    if (!desktopList || listOwner != GetCurrentProcessId()) {
        ReleaseDesktop();
        Wh_Log(L"Desktop unavailable: HRESULT 0x%08X. Retrying in 3 seconds.", (unsigned)hr);
        return false;
    }
    Wh_Log(L"Desktop connected, ListView=%p", desktopList);
    return true;
}

std::wstring FolderUnderMouse(POINT screen, RECT& bounds) {
    if (!desktopList || !IsWindowVisible(desktopList) || WindowFromPoint(screen) != desktopList)
        return {};
    DWORD owner = 0;
    GetWindowThreadProcessId(desktopList, &owner);
    if (owner != GetCurrentProcessId()) return {};
    POINT client = screen;
    ScreenToClient(desktopList, &client);
    LVHITTESTINFO hit{};
    hit.pt = client;
    // Same-process synchronous messages keep pointer arguments alive until handled.
    LRESULT result = SendMessageW(desktopList, LVM_HITTEST, 0, (LPARAM)&hit);
    if (result < 0 || !(hit.flags & LVHT_ONITEM)) return {};
    // Resolve the view index through the Shell, never through the displayed label.
    PITEMID_CHILD item = nullptr;
    if (FAILED(folderView->Item(hit.iItem, &item))) return {};
    STRRET name{};
    wchar_t path[32768]{};
    HRESULT hr = desktopFolder->GetDisplayNameOf(item, SHGDN_FORPARSING, &name);
    if (SUCCEEDED(hr)) hr = StrRetToBufW(&name, item, path, ARRAYSIZE(path));
    CoTaskMemFree(item);
    if (FAILED(hr)) return {};
    // Only local fixed-drive filesystem folders are supported.
    if (path[1] != L':' || GetDriveTypeW((std::wstring(path, 3)).c_str()) != DRIVE_FIXED) return {};
    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT)) return {};
    bounds = {LVIR_BOUNDS, 0, 0, 0};
    if (!SendMessageW(desktopList, LVM_GETITEMRECT, hit.iItem, (LPARAM)&bounds)) return {};
    MapWindowPoints(desktopList, nullptr, (POINT*)&bounds, 2);
    return path;
}

void HidePanel() {
    RestoreDesktopTooltip();
    ShowWindow(panel, SW_HIDE);
    opened.clear();
    outsideSince = 0;
}

void DismissPanel() {
    dismissedFolder = opened.empty() ? candidate : opened;
    HidePanel();
}

void Launch(int index) {
    if (dragging || contextMenuOpen || renaming || GetTickCount64() < ignoreClickUntil) return;
    if (index < 0 || index >= (int)targets.size()) return;
    // Copy before hiding. ShellExecuteEx opens .lnk/.url through Windows associations.
    std::wstring target = targets[index];
    HidePanel();
    candidate.clear();
    SHELLEXECUTEINFOW execute{sizeof(execute)};
    execute.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    execute.hwnd = panel;
    execute.lpFile = target.c_str();
    execute.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&execute)) {
        DWORD error = GetLastError();
        Wh_Log(L"Launch failed (%lu): %s", error, target.c_str());
        MessageBoxW(panel, Text(L"Windows could not open this item. Check the shortcut.", L"Windows non ha potuto aprire questa voce. Verifica il collegamento."),
                    L"Desktop Folder Groups", MB_OK | MB_ICONWARNING);
    }
}

bool SetContentScale(double scale) {
    int newIconPixels = std::max(8, (int)(BaseIconPixels() * scale + 0.5));
    if (viewMode == ViewMode::Large || viewMode == ViewMode::Medium) {
        int spaceForIcon = std::max(8, GridCellWidth() - std::max(4, (int)(24 * scale)));
        // Medium retains its size; large fills each of the two columns.
        newIconPixels = viewMode == ViewMode::Large ? spaceForIcon : std::min(newIconPixels, spaceForIcon);
    }
    int textPixels = std::max(8, (int)(18 * scale + 0.5));
    HFONT newFont = CreateFontW(-textPixels, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HIMAGELIST newIcons = ImageList_Create(newIconPixels, newIconPixels,
        ILC_COLOR32 | ILC_MASK, 16, 16);
    if (!newFont || !newIcons) {
        if (newFont) DeleteObject(newFont);
        if (newIcons) ImageList_Destroy(newIcons);
        Wh_Log(L"Failed to scale text and icons");
        return false;
    }
    SendMessageW(entries, WM_SETFONT, (WPARAM)newFont, TRUE);
    // Detach both slots before destroying an image list: switching from an
    // icon grid to rows must not leave a dangling handle in the unused slot.
    ListView_SetImageList(entries, nullptr, LVSIL_NORMAL);
    ListView_SetImageList(entries, nullptr, LVSIL_SMALL);
    // The selected view determines which image-list slot the control uses.
    ListView_SetImageList(entries, newIcons, IconGrid() ? LVSIL_NORMAL : LVSIL_SMALL);
    if (font) DeleteObject(font);
    if (icons) ImageList_Destroy(icons);
    font = newFont;
    icons = newIcons;
    iconPixels = newIconPixels;
    contentScale = scale;
    HFONT newQuickFont = CreateFontW(-std::max(8, (int)(13 * scale + 0.5)),
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    if (newQuickFont) {
        for (HWND button : quickButtons)
            if (button) SendMessageW(button, WM_SETFONT, (WPARAM)newQuickFont, TRUE);
        if (quickFont) DeleteObject(quickFont);
        quickFont = newQuickFont;
    }
    if (IconGrid()) {
        int cellWidth = GridCellWidth();
        int cellHeight = iconPixels + 2 * textPixels + (int)(24 * scale);
        ListView_SetIconSpacing(entries, cellWidth, cellHeight);
    }
    return true;
}

HICON GetEntryIcon(const std::wstring& target) {
    SHFILEINFOW info{};
    // Ask the Shell for its high-resolution system icon before scaling it.
    if (SHGetFileInfoW(target.c_str(), 0, &info, sizeof(info), SHGFI_SYSICONINDEX)) {
        IImageList* systemImages = nullptr;
        int imageSize = iconPixels > 48 ? SHIL_JUMBO : (iconPixels > 32 ? SHIL_EXTRALARGE : SHIL_LARGE);
        if (SUCCEEDED(SHGetImageList(imageSize, IID_PPV_ARGS(&systemImages)))) {
            HICON icon = nullptr;
            systemImages->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon);
            systemImages->Release();
            if (icon) return icon;
        }
    }
    if (SHGetFileInfoW(target.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_LARGEICON))
        return info.hIcon;
    return nullptr;
}

// System image lists can contain a small drawing in a much larger transparent
// canvas. Scale the visible drawing, not that canvas. Preserve aspect ratio.
HICON NormalizeIcon(HICON icon, int pixels) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) return nullptr;
    BITMAP bitmap{};
    GetObjectW(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bitmap), &bitmap);
    int width = bitmap.bmWidth;
    int height = info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2;
    if (info.hbmColor) DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    if (width < 1 || height < 1 || width > 1024 || height > 1024) return nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc) return nullptr;
    auto makeBitmap = [dc](int w, int h, DWORD** data) {
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        return CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)data, nullptr, 0);
    };
    DWORD *source = nullptr, *maskPixels = nullptr, *destination = nullptr;
    HBITMAP sourceBitmap = makeBitmap(width, height, &source);
    HBITMAP maskBitmap = makeBitmap(width, height, &maskPixels);
    HBITMAP destinationBitmap = makeBitmap(pixels, pixels, &destination);
    HICON result = nullptr;
    if (sourceBitmap && maskBitmap && destinationBitmap) {
        std::fill_n(source, width * height, 0u);
        HGDIOBJ previous = SelectObject(dc, sourceBitmap);
        BOOL rendered = DrawIconEx(dc, 0, 0, icon, width, height, 0, nullptr, DI_NORMAL);
        SelectObject(dc, maskBitmap);
        std::fill_n(maskPixels, width * height, 0x00FFFFFFu);
        DrawIconEx(dc, 0, 0, icon, width, height, 0, nullptr, DI_MASK);
        SelectObject(dc, previous);
        GdiFlush();
        bool hasAlpha = false;
        for (int i = 0; i < width * height; i++)
            if (source[i] >> 24) { hasAlpha = true; break; }
        int left = width, top = height, right = -1, bottom = -1;
        for (int y = 0; y < height; y++) for (int x = 0; x < width; x++) {
            int i = y * width + x;
            // Older icons use a monochrome mask instead of an alpha channel.
            if (!hasAlpha) source[i] = (maskPixels[i] & 0xFF) < 128 ?
                source[i] | 0xFF000000u : 0u;
            if ((source[i] >> 24) > 8) {
                left = std::min(left, x); top = std::min(top, y);
                right = std::max(right, x); bottom = std::max(bottom, y);
            }
        }
        std::fill_n(destination, pixels * pixels, 0u);
        if (rendered && right >= left && bottom >= top) {
            int cropWidth = right - left + 1, cropHeight = bottom - top + 1;
            double factor = std::min((double)pixels / cropWidth, (double)pixels / cropHeight);
            int drawWidth = std::max(1, (int)(cropWidth * factor + 0.5));
            int drawHeight = std::max(1, (int)(cropHeight * factor + 0.5));
            int offsetX = (pixels - drawWidth) / 2, offsetY = (pixels - drawHeight) / 2;
            // Bilinear interpolation of premultiplied BGRA retains transparent edges.
            for (int y = 0; y < drawHeight; y++) for (int x = 0; x < drawWidth; x++) {
                double sx = std::clamp((x + 0.5) * cropWidth / drawWidth - 0.5, 0.0, (double)cropWidth - 1);
                double sy = std::clamp((y + 0.5) * cropHeight / drawHeight - 0.5, 0.0, (double)cropHeight - 1);
                int x0 = (int)sx, y0 = (int)sy;
                int x1 = std::min(x0 + 1, cropWidth - 1), y1 = std::min(y0 + 1, cropHeight - 1);
                double fx = sx - x0, fy = sy - y0;
                DWORD samples[] = {source[(top+y0)*width+left+x0], source[(top+y0)*width+left+x1],
                    source[(top+y1)*width+left+x0], source[(top+y1)*width+left+x1]};
                DWORD value = 0;
                for (int shift = 0; shift < 32; shift += 8) {
                    double a = ((samples[0] >> shift) & 255) * (1-fx) + ((samples[1] >> shift) & 255) * fx;
                    double b = ((samples[2] >> shift) & 255) * (1-fx) + ((samples[3] >> shift) & 255) * fx;
                    value |= (DWORD)(a * (1-fy) + b * fy + 0.5) << shift;
                }
                destination[(offsetY+y)*pixels+offsetX+x] = value;
            }
            std::vector<BYTE> maskData(((pixels + 15) / 16) * 2 * pixels, 0);
            HBITMAP outputMask = CreateBitmap(pixels, pixels, 1, 1, maskData.data());
            if (outputMask) {
                ICONINFO output{};
                output.fIcon = TRUE;
                output.hbmColor = destinationBitmap;
                output.hbmMask = outputMask;
                result = CreateIconIndirect(&output);
                DeleteObject(outputMask);
            }
        }
    }
    if (sourceBitmap) DeleteObject(sourceBitmap);
    if (maskBitmap) DeleteObject(maskBitmap);
    if (destinationBitmap) DeleteObject(destinationBitmap);
    DeleteDC(dc);
    return result;
}

bool ShowFolder(const std::wstring& path, RECT bounds) {
    ApplyWindowsTheme();
    ListView_DeleteAllItems(entries);
    targets.clear();
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(MonitorFromRect(&bounds, MONITOR_DEFAULTTONEAREST), &monitor)) return false;
    const RECT& area = monitor.rcWork;
    // Fit using a single factor, never clamp width and height independently.
    double scale = std::min({panelScale / 100.0,
        (area.right - area.left) / 400.0, (area.bottom - area.top) / 600.0});
    // Exact 2:3 width:height ratio, also after fitting to a small work area.
    int unit = std::max(1, (int)(100 * scale));
    int width = 4 * unit, height = 6 * unit;
    scale = unit / 100.0;
    // Establish the actual client width before allocating/scaling icons.
    SetWindowPos(panel, nullptr, 0, 0, width, height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (!SetContentScale(scale)) return false;
    WIN32_FIND_DATAW file{};
    HANDLE search = FindFirstFileW((path + L"\\*").c_str(), &file);
    if (search == INVALID_HANDLE_VALUE) return false;
    std::vector<std::wstring> names;
    do {
        if (file.cFileName[0] == L'.' &&
            (!file.cFileName[1] || (file.cFileName[1] == L'.' && !file.cFileName[2]))) continue;
        if (file.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) continue;
        names.emplace_back(file.cFileName);
    } while (names.size() < 100 && FindNextFileW(search, &file));
    FindClose(search);
    std::sort(names.begin(), names.end(), [](const auto& a, const auto& b) {
        return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
    });
    for (const auto& name : names) {
        auto target = path + L"\\" + name;
        int imageIndex = -1;
        HICON icon = GetEntryIcon(target);
        if (icon) {
            HICON resized = NormalizeIcon(icon, iconPixels);
            if (!resized) resized = (HICON)CopyImage(icon, IMAGE_ICON, iconPixels, iconPixels, 0);
            imageIndex = ImageList_AddIcon(icons, resized ? resized : icon);
            if (resized && resized != icon) DestroyIcon(resized);
            DestroyIcon(icon);
        }
        std::wstring label = name;
        const wchar_t* extension = PathFindExtensionW(label.c_str());
        if (!_wcsicmp(extension, L".lnk") || !_wcsicmp(extension, L".url"))
            label.resize(extension - label.c_str());
        LVITEMW item{};
        item.mask = LVIF_TEXT | LVIF_IMAGE;
        item.iItem = (int)targets.size();
        item.pszText = label.data();
        item.iImage = imageIndex;
        ListView_InsertItem(entries, &item);
        targets.push_back(std::move(target));
    }
    int x = bounds.right + 4;
    if (x + width > area.right) x = bounds.left - width - 4;
    x = std::clamp(x, (int)area.left, (int)area.right - width);
    int y = std::clamp((int)bounds.top, (int)area.top, (int)area.bottom - height);
    std::wstring title = PathFindFileNameW(path.c_str());
    title += names.empty() ? Text(L" — Empty folder", L" — Cartella vuota") : Text(L" — click to open", L" — clic per aprire");
    SetWindowTextW(panel, title.c_str());
    anchor = bounds;
    opened = path;
    outsideSince = 0;
    SuppressDesktopTooltip();
    SetWindowPos(panel, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    RECT client{};
    GetClientRect(panel, &client);
    ResizeEntries(client.right, client.bottom);
    for (HWND button : quickButtons) if (button) InvalidateRect(button, nullptr, TRUE);
    Wh_Log(L"Panel opened: %s (%u items)", path.c_str(), (unsigned)targets.size());
    return true;
}

class PanelDropSource final : public IDropSource {
    LONG references = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IDropSource)) {
            *object = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&references); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = InterlockedDecrement(&references);
        if (!remaining) delete this;
        return remaining;
    }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD keys) override {
        if (escape || WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return DRAGDROP_S_CANCEL;
        return (keys & MK_LBUTTON) ? S_OK : DRAGDROP_S_DROP;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

void BeginEntryDrag(int index) {
    if (!enableDragOut || dragging || contextMenuOpen || renaming || index < 0 || index >= (int)targets.size()) return;
    // The Shell data object represents the actual shortcut/file, not its target
    // program. Explorer's drop target performs the move/copy and conflict UI.
    std::wstring target = targets[index];
    PIDLIST_ABSOLUTE absolute = ILCreateFromPathW(target.c_str());
    if (!absolute) return;
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    IDataObject* data = nullptr;
    HRESULT hr = SHBindToParent(absolute, IID_PPV_ARGS(&parent), &child);
    if (SUCCEEDED(hr)) hr = parent->GetUIObjectOf(panel, 1, &child, IID_IDataObject, nullptr, (void**)&data);
    if (parent) parent->Release();
    CoTaskMemFree(absolute);
    if (FAILED(hr) || !data) {
        Wh_Log(L"Preparing drag failed: 0x%08X", (unsigned)hr);
        return;
    }
    auto source = new (std::nothrow) PanelDropSource();
    if (!source) { data->Release(); return; }
    // Prefer moving out of a group; Ctrl still requests a copy from Windows.
    HGLOBAL preferred = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
    if (preferred) {
        auto value = (DWORD*)GlobalLock(preferred);
        if (value) {
            *value = DROPEFFECT_MOVE;
            GlobalUnlock(preferred);
            FORMATETC format{(CLIPFORMAT)RegisterClipboardFormatW(CFSTR_PREFERREDDROPEFFECT),
                nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
            STGMEDIUM medium{};
            medium.tymed = TYMED_HGLOBAL;
            medium.hGlobal = preferred;
            if (FAILED(data->SetData(&format, &medium, TRUE))) GlobalFree(preferred);
        } else GlobalFree(preferred);
    }
    dragging = true;
    HidePanel();
    candidate.clear();
    ReleaseCapture();
    DWORD effect = DROPEFFECT_NONE;
    hr = SHDoDragDrop(entries, data, source, DROPEFFECT_MOVE | DROPEFFECT_COPY, &effect);
    source->Release();
    data->Release();
    dragging = false;
    ignoreClickUntil = GetTickCount64() + 350;
    candidate.clear();
    // Reopening enumerates the folder again, reflecting moved or copied items.
    Wh_Log(L"Drag finished: result=0x%08X effect=%lu", (unsigned)hr, effect);
}

void ResizeEntries(int width, int height) {
    if (!entries) return;
    int margin = std::max(2, (int)(6 * contentScale));
    int buttonHeight = std::max(20, (int)(30 * contentScale));
    int barHeight = std::min(height, buttonHeight + 2 * margin);
    int available = std::max(0, width - 4 * margin);
    for (int i = 0; i < 3; i++) {
        int left = margin + i * available / 3 + i * margin;
        int right = margin + (i + 1) * available / 3 + i * margin;
        if (quickButtons[i]) MoveWindow(quickButtons[i], left, margin,
            std::max(0, right - left), std::min(buttonHeight, std::max(0, height - margin)), TRUE);
    }
    MoveWindow(entries, 0, barHeight, width, std::max(0, height - barHeight), TRUE);
    if (IconGrid()) {
        int textPixels = std::max(8, (int)(18 * contentScale + 0.5));
        ListView_SetIconSpacing(entries, GridCellWidth(),
            iconPixels + 2 * textPixels + (int)(24 * contentScale));
        ListView_Arrange(entries, LVA_DEFAULT);
    }
    // Reserve scrollbar space even before rows are populated, avoiding a
    // horizontal scrollbar when the vertical scrollbar appears later.
    if (viewMode == ViewMode::List)
        ListView_SetColumnWidth(entries, 0,
            std::max(1, width - GetSystemMetrics(SM_CXVSCROLL) - 4));
}

ViewMode QuickMode(int index) {
    return index == 0 ? ViewMode::Large : (index == 1 ? ViewMode::Medium : ViewMode::List);
}

void SwitchQuickView(int index) {
    if (dragging || contextMenuOpen || renaming) return;
    if (index < 0 || index > 2 || opened.empty() || viewMode == QuickMode(index)) return;
    int selected = ListView_GetNextItem(entries, -1, LVNI_SELECTED);
    std::wstring selectedPath = selected >= 0 && selected < (int)targets.size() ? targets[selected] : L"";
    std::wstring path = opened;
    viewMode = QuickMode(index);
    ListView_DeleteAllItems(entries);
    LONG_PTR style = GetWindowLongPtrW(entries, GWL_STYLE);
    style &= ~(LVS_TYPEMASK | LVS_AUTOARRANGE | LVS_NOCOLUMNHEADER);
    style |= IconGrid() ? (LVS_ICON | LVS_AUTOARRANGE) : (LVS_REPORT | LVS_NOCOLUMNHEADER);
    SetWindowLongPtrW(entries, GWL_STYLE, style);
    SetWindowPos(entries, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    ListView_SetExtendedListViewStyle(entries, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP |
        (viewMode == ViewMode::List ? LVS_EX_FULLROWSELECT : 0));
    if (!ShowFolder(path, anchor)) { HidePanel(); return; }
    for (int i = 0; i < (int)targets.size(); i++) {
        if (!selectedPath.empty() && targets[i] == selectedPath) {
            ListView_SetItemState(entries, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(entries, i, FALSE);
            break;
        }
    }
    outsideSince = 0;
}

bool DrawQuickButton(DRAWITEMSTRUCT* item) {
    int index = (int)item->CtlID - QuickButtonFirst;
    if (item->CtlType != ODT_BUTTON || index < 0 || index > 2) return false;
    bool active = viewMode == QuickMode(index);
    bool pressed = (item->itemState & ODS_SELECTED) != 0;
    bool highlight = active || pressed;
    COLORREF background = highlight ? GetSysColor(COLOR_HIGHLIGHT) : themeBackground;
    COLORREF foreground = highlight ? GetSysColor(COLOR_HIGHLIGHTTEXT) : themeForeground;
    HBRUSH brush = CreateSolidBrush(background);
    if (brush) { FillRect(item->hDC, &item->rcItem, brush); DeleteObject(brush); }
    HBRUSH border = CreateSolidBrush(highlight ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_GRAYTEXT));
    if (border) { FrameRect(item->hDC, &item->rcItem, border); DeleteObject(border); }
    int saved = SaveDC(item->hDC);
    if (quickFont) SelectObject(item->hDC, quickFont);
    SetBkMode(item->hDC, TRANSPARENT);
    SetTextColor(item->hDC, foreground);
    wchar_t label[64]{};
    GetWindowTextW(item->hwndItem, label, ARRAYSIZE(label));
    RECT text = item->rcItem;
    InflateRect(&text, -4, -2);
    DrawTextW(item->hDC, label, -1, &text, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    if (item->itemState & ODS_FOCUS) { InflateRect(&text, -1, -1); DrawFocusRect(item->hDC, &text); }
    if (saved) RestoreDC(item->hDC, saved);
    return true;
}

void Tick() {
    if (dragging || contextMenuOpen || renaming) return;
    ULONGLONG now = GetTickCount64();
    if (now >= themeCheckAt) {
        ApplyWindowsTheme();
        themeCheckAt = now + 1000;
    }
    POINT mouse{};
    GetCursorPos(&mouse);
    bool escapeDown = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
    if (escapeDown && !escapeWasDown) {
        DismissPanel();
    }
    escapeWasDown = escapeDown;
    if (!IsWindow(desktopList)) {
        HidePanel();
        if (now >= retryAt) { ConnectDesktop(); retryAt = now + 3000; }
        return;
    }
    if (!opened.empty()) {
        SuppressDesktopTooltip();
        RECT popup{};
        GetWindowRect(panel, &popup);
        HWND under = WindowFromPoint(mouse);
        bool insidePanel = (under == panel || IsChild(panel, under)) && PtInRect(&popup, mouse);
        bool insideAnchor = under == desktopList && PtInRect(&anchor, mouse);
        if (insidePanel || insideAnchor) outsideSince = 0;
        else if (!outsideSince) outsideSince = now;
        else if (now - outsideSince >= (ULONGLONG)closeDelay) HidePanel();
        return;
    }
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        candidate.clear();
        return;
    }
    RECT bounds{};
    auto path = FolderUnderMouse(mouse, bounds);
    if (path.empty()) { candidate.clear(); dismissedFolder.clear(); return; }
    if (path == dismissedFolder) return;
    dismissedFolder.clear();
    if (path != candidate) { candidate = path; candidateSince = now; }
    else if (now >= candidateSince && now - candidateSince >= (ULONGLONG)openDelay) {
        if (!ShowFolder(path, bounds)) candidateSince = now + 3000;
    }
}

void ShowEntryContextMenu(int index, POINT point) {
    if (dragging || contextMenuOpen || renaming || index < 0 || index >= (int)targets.size()) return;
    std::wstring target = targets[index];
    std::wstring folder = opened;
    RECT bounds = anchor;
    contextMenuOpen = true;
    ListView_SetItemState(entries, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_SetItemState(entries, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    PIDLIST_ABSOLUTE absolute = ILCreateFromPathW(target.c_str());
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    IContextMenu* menu = nullptr;
    HRESULT hr = absolute ? SHBindToParent(absolute, IID_PPV_ARGS(&parent), &child) : E_FAIL;
    if (SUCCEEDED(hr)) hr = parent->GetUIObjectOf(panel, 1, &child, IID_IContextMenu, nullptr, (void**)&menu);
    HMENU popup = nullptr;
    bool startRename = false;
    bool executed = false;
    if (SUCCEEDED(hr) && menu) {
        popup = CreatePopupMenu();
        UINT flags = CMF_NORMAL | CMF_EXPLORE | CMF_CANRENAME;
        if (GetKeyState(VK_SHIFT) & 0x8000) flags |= CMF_EXTENDEDVERBS;
        hr = popup ? menu->QueryContextMenu(popup, 0, 1, 0x7FFF, flags) : E_OUTOFMEMORY;
        if (SUCCEEDED(hr)) {
            menu->QueryInterface(IID_PPV_ARGS(&activeMenu3));
            if (!activeMenu3) menu->QueryInterface(IID_PPV_ARGS(&activeMenu2));
            SetForegroundWindow(panel);
            UINT command = TrackPopupMenuEx(popup, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                point.x, point.y, panel, nullptr);
            if (command >= 1 && command <= 0x7FFF) {
                wchar_t verb[128]{};
                menu->GetCommandString(command - 1, GCS_VERBW, nullptr, (LPSTR)verb, ARRAYSIZE(verb));
                startRename = _wcsicmp(verb, L"rename") == 0;
                if (!startRename) {
                    CMINVOKECOMMANDINFOEX invoke{};
                    invoke.cbSize = sizeof(invoke);
                    invoke.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE | CMIC_MASK_NOASYNC;
                    if (GetKeyState(VK_SHIFT) & 0x8000) invoke.fMask |= CMIC_MASK_SHIFT_DOWN;
                    if (GetKeyState(VK_CONTROL) & 0x8000) invoke.fMask |= CMIC_MASK_CONTROL_DOWN;
                    invoke.hwnd = panel;
                    invoke.lpVerb = MAKEINTRESOURCEA(command - 1);
                    invoke.lpVerbW = MAKEINTRESOURCEW(command - 1);
                    invoke.lpDirectoryW = folder.c_str();
                    invoke.nShow = SW_SHOWNORMAL;
                    invoke.ptInvoke = point;
                    hr = menu->InvokeCommand((CMINVOKECOMMANDINFO*)&invoke);
                    executed = SUCCEEDED(hr);
                }
            }
            // Popup menu ownership remains valid through invocation and teardown.
            if (activeMenu3) activeMenu3->Release();
            if (activeMenu2) activeMenu2->Release();
            activeMenu3 = nullptr;
            activeMenu2 = nullptr;
            PostMessageW(panel, WM_NULL, 0, 0);
        }
    }
    if (popup) DestroyMenu(popup);
    if (menu) menu->Release();
    if (parent) parent->Release();
    if (absolute) CoTaskMemFree(absolute);
    contextMenuOpen = false;
    outsideSince = 0;
    ignoreClickUntil = GetTickCount64() + 350;
    if (FAILED(hr)) {
        Wh_Log(L"Context menu failed: 0x%08X", (unsigned)hr);
        MessageBoxW(panel, Text(L"Windows could not complete this operation.", L"Windows non ha potuto completare questa operazione."),
            L"Desktop Folder Groups", MB_OK | MB_ICONWARNING);
    }
    if (startRename && !opened.empty()) {
        SetFocus(entries);
        ListView_EditLabel(entries, index);
    } else if (executed && !opened.empty()) {
        if (!ShowFolder(folder, bounds)) HidePanel();
    }
}

bool EndEntryRename(NMLVDISPINFOW* change) {
    bool renamed = false;
    int index = change->item.iItem;
    if (change->item.pszText && change->item.pszText[0] && index >= 0 && index < (int)targets.size()) {
        std::wstring target = targets[index], newName = change->item.pszText;
        const wchar_t* extension = PathFindExtensionW(target.c_str());
        // Labels hide shortcut extensions; preserve the shortcut's file type.
        if ((!_wcsicmp(extension, L".lnk") || !_wcsicmp(extension, L".url")) &&
            _wcsicmp(PathFindExtensionW(newName.c_str()), extension)) newName += extension;
        PIDLIST_ABSOLUTE absolute = ILCreateFromPathW(target.c_str());
        IShellFolder* parent = nullptr;
        PCUITEMID_CHILD child = nullptr;
        HRESULT hr = absolute ? SHBindToParent(absolute, IID_PPV_ARGS(&parent), &child) : E_FAIL;
        if (SUCCEEDED(hr)) hr = parent->SetNameOf(panel, child, newName.c_str(), (SHGDNF)(SHGDN_INFOLDER | SHGDN_FORPARSING), nullptr);
        if (parent) parent->Release();
        if (absolute) CoTaskMemFree(absolute);
        if (SUCCEEDED(hr)) {
            targets[index] = target.substr(0, target.find_last_of(L"\\/") + 1) + newName;
            renamed = true;
        } else {
            Wh_Log(L"Rename failed: 0x%08X", (unsigned)hr);
            MessageBoxW(panel, Text(L"Could not rename the item. Check the name, permissions, or an existing file with the same name.", L"Impossibile rinominare la voce. Verifica il nome, i permessi o un eventuale file omonimo."),
                L"Desktop Folder Groups", MB_OK | MB_ICONWARNING);
        }
    }
    renaming = false;
    outsideSince = 0;
    ignoreClickUntil = GetTickCount64() + 350;
    return renamed;
}

LRESULT CALLBACK PanelProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    // Forward owner-drawn menu and dynamic submenu messages to Shell extensions.
    bool menuMessage = message == WM_INITMENUPOPUP || message == WM_MENUCHAR ||
        message == WM_UNINITMENUPOPUP ||
        (message == WM_DRAWITEM && wParam == 0) || (message == WM_MEASUREITEM && wParam == 0);
    if (menuMessage && activeMenu3) {
        LRESULT result = 0;
        if (activeMenu3->HandleMenuMsg2(message, wParam, lParam, &result) == S_OK) return result;
    } else if (menuMessage && activeMenu2 && message != WM_MENUCHAR) {
        if (activeMenu2->HandleMenuMsg(message, wParam, lParam) == S_OK) return 0;
    }
    switch (message) {
        case WM_TIMER:
            if (wParam == ShutdownTimer && stopEvent &&
                WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
                // OLE and popup menus run nested message loops. Give those
                // loops a cancellation path before waiting for the STA to exit.
                if (contextMenuOpen) EndMenu();
                if (renaming) SendMessageW(entries, LVM_CANCELEDITLABEL, 0, 0);
                if (dragging) PostMessageW(entries, WM_KEYDOWN, VK_ESCAPE, 0);
                return 0;
            }
            break;
        case WM_CONTEXTMENU:
            if ((HWND)wParam == entries && GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam) == -1) {
                int index = ListView_GetNextItem(entries, -1, LVNI_SELECTED);
                RECT item{};
                if (index >= 0 && ListView_GetItemRect(entries, index, &item, LVIR_BOUNDS)) {
                    POINT point{item.left, item.bottom};
                    ClientToScreen(entries, &point);
                    ShowEntryContextMenu(index, point);
                }
                return 0;
            }
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL && !renaming && !contextMenuOpen && !dragging) {
                DismissPanel();
                return 0;
            }
            if (LOWORD(wParam) == IDOK && GetFocus() == entries) {
                Launch(ListView_GetNextItem(entries, -1, LVNI_SELECTED));
                return 0;
            }
            if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) >= QuickButtonFirst &&
                LOWORD(wParam) < QuickButtonFirst + 3) {
                SwitchQuickView(LOWORD(wParam) - QuickButtonFirst);
                return 0;
            }
            break;
        case WM_DRAWITEM:
            if (DrawQuickButton((DRAWITEMSTRUCT*)lParam)) return TRUE;
            break;
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
            ApplyWindowsTheme(true);
            break;
        case WM_ERASEBKGND:
            if (panelBrush) {
                RECT client{};
                GetClientRect(window, &client);
                FillRect((HDC)wParam, &client, panelBrush);
                return 1;
            }
            break;
        case WM_SIZE:
            ResizeEntries(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_CLOSE: DismissPanel(); return 0;
        case WM_DISPLAYCHANGE: HidePanel(); return 0;
        case WM_NOTIFY: {
            auto header = (NMHDR*)lParam;
            if (header->hwndFrom == entries && header->code == NM_RCLICK) {
                auto click = (NMITEMACTIVATE*)lParam;
                POINT point = click->ptAction;
                ClientToScreen(entries, &point);
                ShowEntryContextMenu(click->iItem, point);
                return 0;
            }
            if (header->hwndFrom == entries && header->code == LVN_BEGINLABELEDITW) {
                if (dragging || contextMenuOpen) return TRUE;
                renaming = true;
                return FALSE;
            }
            if (header->hwndFrom == entries && header->code == LVN_ENDLABELEDITW) {
                return EndEntryRename((NMLVDISPINFOW*)lParam) ? TRUE : FALSE;
            }
            if (header->hwndFrom == entries && header->code == LVN_BEGINDRAG) {
                BeginEntryDrag(((NMLISTVIEW*)lParam)->iItem);
                return 0;
            }
            if (header->hwndFrom == entries && header->code == NM_CLICK) {
                Launch(((NMITEMACTIVATE*)lParam)->iItem);
                return 0;
            }
            if (header->hwndFrom == entries && header->code == LVN_KEYDOWN) {
                auto key = (NMLVKEYDOWN*)lParam;
                if (key->wVKey == VK_RETURN) Launch(ListView_GetNextItem(entries, -1, LVNI_SELECTED));
                if (key->wVKey == VK_ESCAPE) DismissPanel();
            }
            break;
        }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

DWORD WINAPI Run(void*) {
    HRESULT com = OleInitialize(nullptr);
    if (FAILED(com)) { Wh_Log(L"OLE initialization failed"); return 1; }
    HINSTANCE instance = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&Run, &instance);
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&controls);
    WNDCLASSW cls{};
    cls.hInstance = instance;
    cls.lpfnWndProc = PanelProc;
    cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    cls.lpszClassName = WindowClass;
    if (!RegisterClassW(&cls)) { OleUninitialize(); return 1; }
    panel = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WindowClass, L"Desktop Folder Groups",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_BORDER, 0, 0, 400, 600, nullptr, nullptr, instance, nullptr);
    DWORD viewStyle = IconGrid() ? (LVS_ICON | LVS_AUTOARRANGE) :
        (viewMode == ViewMode::Small ? (LVS_SMALLICON | LVS_AUTOARRANGE) : (LVS_REPORT | LVS_NOCOLUMNHEADER));
    if (panel) entries = CreateWindowExW(0, WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | viewStyle | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS | LVS_EDITLABELS,
        0, 0, 0, 0, panel, (HMENU)1, instance, nullptr);
    if (entries) {
        if (!SetTimer(panel, ShutdownTimer, 100, nullptr)) {
            Wh_Log(L"Could not create modal-loop shutdown timer (%lu)", GetLastError());
        }
        ApplyWindowsTheme();
        const wchar_t* labels[] = {Text(L"Large icons", L"Icone grandi"), Text(L"Medium icons", L"Icone medie"), Text(L"List", L"Elenco")};
        for (int i = 0; i < 3; i++) {
            quickButtons[i] = CreateWindowExW(0, L"BUTTON", labels[i],
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                0, 0, 0, 0, panel, (HMENU)(INT_PTR)(QuickButtonFirst + i), instance, nullptr);
        }
        ListView_SetExtendedListViewStyle(entries, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP |
            (viewMode == ViewMode::List ? LVS_EX_FULLROWSELECT : 0));
        LVCOLUMNW column{};
        column.mask = LVCF_WIDTH | LVCF_TEXT;
        column.cx = 400;
        wchar_t columnName[32]{};
        lstrcpynW(columnName, Text(L"Item", L"Voce"), ARRAYSIZE(columnName));
        column.pszText = columnName;
        ListView_InsertColumn(entries, 0, &column);
        RECT client{};
        GetClientRect(panel, &client);
        ResizeEntries(client.right, client.bottom);
        ConnectDesktop();
        // A waitable stop event ensures unloading wakes the UI thread without posting callbacks.
        while (true) {
            DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, 80, QS_ALLINPUT);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (ListView_GetEditControl(entries) || !IsDialogMessageW(panel, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            Tick();
        }
    } else Wh_Log(L"Panel creation failed (%lu)", GetLastError());
    RestoreDesktopTooltip();
    if (panel) {
        KillTimer(panel, ShutdownTimer);
        DestroyWindow(panel);
    }
    panel = entries = nullptr;
    for (HWND& button : quickButtons) button = nullptr;
    if (quickFont) DeleteObject(quickFont);
    quickFont = nullptr;
    if (panelBrush) DeleteObject(panelBrush);
    panelBrush = nullptr;
    themeInitialized = false;
    if (icons) ImageList_Destroy(icons);
    icons = nullptr;
    if (font) DeleteObject(font);
    font = nullptr;
    targets.clear();
    candidate.clear();
    opened.clear();
    ReleaseDesktop();
    UnregisterClassW(WindowClass, instance);
    OleUninitialize();
    return 0;
}
} // namespace

BOOL Wh_ModInit() {
    italianUI = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_ITALIAN;
    // Only the process owning the desktop may run a panel.
    DWORD desktopProcess = 0;
    HWND shell = GetShellWindow();
    if (shell) {
        GetWindowThreadProcessId(shell, &desktopProcess);
        if (desktopProcess != GetCurrentProcessId()) return FALSE;
    }
    openDelay = std::clamp(Wh_GetIntSetting(L"openDelay"), 150, 3000);
    closeDelay = std::clamp(Wh_GetIntSetting(L"closeDelay"), 200, 3000);
    enableDragOut = Wh_GetIntSetting(L"enableDragOut") != 0;
    hideFolderTooltips = Wh_GetIntSetting(L"hideFolderTooltips") != 0;
    int requestedScale = Wh_GetIntSetting(L"panelScale");
    panelScale = requestedScale > 0 ? std::clamp(requestedScale, 60, 200) : 100;
    viewMode = ViewMode::List;
    PCWSTR mode = Wh_GetStringSetting(L"viewMode");
    if (mode) {
        if (wcscmp(mode, L"small") == 0) viewMode = ViewMode::Small;
        else if (wcscmp(mode, L"medium") == 0) viewMode = ViewMode::Medium;
        else if (wcscmp(mode, L"large") == 0) viewMode = ViewMode::Large;
        else if (wcscmp(mode, L"extraLarge") == 0) viewMode = ViewMode::ExtraLarge;
        Wh_FreeStringSetting(mode);
    }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;
    worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) { CloseHandle(stopEvent); stopEvent = nullptr; return FALSE; }
    return TRUE;
}

void Wh_ModUninit() {
    if (stopEvent) SetEvent(stopEvent);
    // Never unload code while its UI/COM thread is still using it.
    if (worker) { WaitForSingleObject(worker, INFINITE); CloseHandle(worker); }
    if (stopEvent) CloseHandle(stopEvent);
    worker = stopEvent = nullptr;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
