// ==WindhawkMod==
// @id              fully-custom-winver
// @name            Fully Customizable WinVer
// @description     Fully customize your winver.exe by even "replacing" the logo!
// @version         1.0.0
// @author          ItsTauTvyDas
// @github          https://github.com/ItsTauTvyDas
// @twitter         https://x.com/ItsTauTvyDas
// @homepage        https://itstautvydas.me
// @include         winver.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lole32 -luuid -ldwmapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Customizable winver.exe

Features:
* Modify all controls (text, color, location, size, visibility)
* Custom logo (it just creates a custom child window in the logo area, transparency is supported!)
* Change the title
* Change window parameters (make it always top, custom size, open location)
* Change background color

**!! Please check notes below to not get confused about some things !!**

All control internal (user) IDs were identified using **winspy** application.

Supported logo image formats:
* BMP
* JPEG (.jpg, .jpeg, .jpe, .jfif)
* PNG
* GIF (supports animation and transparent background too\*)
* TIFF (.tif, .tiff)
* ICO
* WMF / EMF
* WEBP, HEIC, ACIF and other ones can also be supported if codec is installed.

Available palceholders:
* `{default.<id>}` - Inserts the default control text. Replace `<id>` with the control ID (capitalize each word and remove spaces).
* `{default.<id>::5}` - Removes first 5 characters from control's text.
* `{default.<id>::5/9}` - Substrings control's text, skips the first 5 characters and only returns next 9 characters.

Examples with `{default...}` placeholders.
* `{default.Copyright::0/1} Yoursoft {default.Copyright::12}` - take the copyright sign (get text from 0th to 1th place), show your text and add back the remaining text. This basically replaces "Microsoft" word.

# Screenshots

![Running cat](https://i.imgur.com/uGKh0wE.gif)

![winver too cute](https://i.imgur.com/HjpaLTc.png)

![all dark](https://i.imgur.com/ZEgkLqK.gif)

# Notes

* Tested only on Windows 11!
* Information about previous label texts are not retained. To restore the original content, simply restart *winver.exe*.
* Gifs do not support controls being in front (if you change z-indexing), the gif will be always *drawn* on top!
* To restore the **license info** link, use <A> and </A> tags (similar to HTML): - `Click <A>here</A> to view license information`.
* If you want to hide the current logo, set background color to the same color (240, 240, 240) and the logo will get overdrawn.
* **Settings might not apply correctly (via "Save settings" button) if you have winver.exe opened! Better to restart it.**

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logo:
  - enabled: TRUE
    $name: "Enabled"
  - center: TRUE
    $name: "Center"
  - x: 0
    $name: "X"
  - y: 10
    $name: "Y"
  - width: 0
    $name: "Width"
    $description: "Set to 0 for full image's width"
  - height: -1
    $name: "Height"
    $description: "Set to -1 for max height needed (72 pixels), set to 0 for default size"
  - path: "C:/Windows/System32/DisplaySystemToastIcon.png"
    $name: "Path to the logo"
    $description: "Check mod's description for supported image formats"
  - clearBackground: TRUE
    $name: "Clear logo's background for transparent animated gifs"
  $name: "Custom logo"
- startPosition:
  - enabled: TRUE
    $name: "Enabled"
  - center: TRUE
    $name: "Center the window"
    $description: "If enabled, X and Y will act as offsets"
  - monitor: 0
    $name: "Monitor"
    $description: "If 0 then it chooses monitor where the window appeared by default"
  - x: 0
    $name: "X"
  - y: 0
    $name: "Y"
  $name: "Window open position"
- windowSize:
  - enabled: TRUE
    $name: "Enabled"
  - width: 0
    $name: "Width"
    $description: "Set to 0 for default size"
  - height: 0
    $name: "Height"
    $description: "Set to 0 for default size"
  - expandControls: TRUE
    $name: "Expand controls"
    $description: "Match control's match window's width (only works if width/height is modified, restart winver.exe to take effect)"
  - exceptionalOkButton: TRUE
    $name: "Vertically realign OK button"
    $description: "Calculates Y offset"
  $name: "Window size"
- removeFirstFocus: FALSE
  $name: "Remove focus on start up"
- alwaysOnTop: FALSE
  $name: "Make window always on top"
- customIcon: "shell32.dll;23"
  $name: "Custom widnow icon"
  $description: "Can be either DLL with icon index (shell32.dll;23) or absolute path to .ico, .cur or .bmp"
- removeTitleIcon: TRUE
  $name: "Remove icon from title bar"
- restoreCloseButton: TRUE
  $name: "Show close button"
  $description: "Enable this if you want to still see close button after titlebar icon removal"
- windowColors:
  - title: ""
    $name: "Title text color"
  - border: ""
    $name: "Border's color"
  - bar: "241, 243, 249"
    $name: "Title bar color"
  - background: "240, 240, 240"
    $name: "Background color"
    $description: "Be aware that this remove the original Windows logo if custom logo is not enabled!"
  - separator: ""
    $name: "Separator color"
  - textColor: "0, 0, 0"
    $name: "Default text color"
  $name: "Change window colors"
  $description: "Can be left empty for default. Specify color in RGB (e.g., 255, 16, 74)"
- titleText: ""
  $name: "Custom title text"
  $description: "Add a space if you don't want title at all"
- modifyWindows:
  - - id: Copyright
      $options:
      - LeftIcon: "Left icon"
      - Separator: "Separator"
      - ProductName: "Product name"
      - Version: "Version"
      - Copyright: "Copyright"
      - Spacer1: "Spacer 1"
      - LegalNotice: "Legal notice"
      - Spacer2: "Spacer 2"
      - LicenseInfo: "License info"
      - RegisteredUser1: "Registered user 1"
      - RegisteredUser2: "Registered user 2"
      - OkButton: "Ok button"
      $name: "Label"
    - label: "{default.Copyright::0/1} Nekosoft {default.Copyright::12}"
      $name: "Rename label"
      $description: "Check mod's description for placeholders use!"
    - hidden: FALSE
      $name: "Hidden"
    - relativePosition: FALSE
      $name: "New location relative to the original one"
    - position: ""
      $name: "Location"
      $description: "Format: x;y;width;height, d stands for default which will be replaced with original value. If nothing is specified, position is defaulted"
    - zIndex: "Default"
      $options:
      - Default: "Default"
      - Bottom: "Bottom"
      - Top: "Top"
      - LogoImage: "Logo Image"
      - LeftIcon: "Left icon"
      - Separator: "Separator"
      - ProductName: "Product name"
      - Version: "Version"
      - Copyright: "Copyright"
      - Spacer1: "Spacer 1"
      - LegalNotice: "Legal notice"
      - Spacer2: "Spacer 2"
      - LicenseInfo: "License info"
      - RegisteredUser1: "Registered user 1"
      - RegisteredUser2: "Registered user 2"
      - OkButton: "Ok button"
      $name: "Insert on top of"
      $description: "Set to Default for default Z index ordering (might need a restart of winver.exe because original z index order does not get saved!)"
    - textColor: ""
      $name: "Text color"
  $name: Modify controls
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <format>
#include <string>
#include <unordered_map>
#include <windhawk_utils.h>
#include <gdiplus.h>
#include <vector>
#include <dwmapi.h>

#define MOD_TITLE L"WindHawk: Customizable WinVer"
#define LOGO_BACKGROUND_ID 100
#define LOGO_IMAGE_ID 101
#define MAX_LOGO_HEIGHT 72
#define DEFAULT_BAR_BACKGROUND RGB(241, 243, 249)
#define DEFAULT_BACKGROUND RGB(240, 240, 240)
#define DEFAULT_TEXT_COLOR RGB(0, 0, 0)

#define WM_CAPTURE_LOGO_BACKGROUND WM_USER + 1

#define PrintRect(name, rect) Wh_Log(L"%ls RECT -> [%d, %d, %d, %d], calucated width and height -> [%d, %d]", name, rect.left, rect.top, rect.right, rect.bottom, rect.right - rect.left, rect.bottom - rect.top)
#define PrintRect2(name, rect) Wh_Log(L"%ls RECT* -> [%d, %d, %d, %d], calucated width and height -> [%d, %d]", name, rect.left, rect.top, rect.right + rect.left, rect.bottom + rect.top, rect.right, rect.bottom)

struct WindowAttributes {
    std::wstring id, label;
    BOOL hidden;
    INT x, y, width, height;
    HWND hWnd;
};

struct DefinedWindowAttributes : WindowAttributes {
    std::wstring position, insertAfter;
    BOOL relativePosition, hasTextColor;
    COLORREF textColor;
    void pos();
};

static const std::unordered_map<std::wstring, INT> g_controlIds = {
    {L"LeftIcon",        0x3009},
    {L"Separator",       0x3327},
    {L"ProductName",     0x3500},
    {L"Version",         0x350B},
    {L"Copyright",       0x350A},
    {L"Spacer1",         0xCA03},
    {L"LegalNotice",     0x3513},
    {L"Spacer2",         0x350D},
    {L"LicenseInfo",     0x3512},
    {L"RegisteredUser1", 0x3507},
    {L"RegisteredUser2", 0x3508},
    {L"OkButton",        0x0001}
};

struct MonitorInfoEx {
    HMONITOR handle;
    MONITORINFOEX info;
};

struct GifAnimation {
    std::unique_ptr<Gdiplus::Image> img;
    GUID dim = Gdiplus::FrameDimensionTime;
    std::vector<UINT> delays10ms;
    UINT frame = 0, count = 0, timerId = 1;
    INT width, height;
    BOOL valid() const {
        return img && count > 1;
    }
};

LRESULT CALLBACK WndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);

std::unordered_map<std::wstring, WindowAttributes> g_defaultWindowAttributes;
std::unordered_map<std::wstring, DefinedWindowAttributes> g_definedWindowAttributes;
std::vector<std::wstring> g_errorMessages;
HWINEVENTHOOK g_hHook;
HWND g_mainWindow, g_logoImage;
std::unique_ptr<Gdiplus::Bitmap> g_logoImageBitmap;
GifAnimation g_gif;
ULONG_PTR g_gdiToken;
COLORREF g_windowBackgroundColor = DEFAULT_BACKGROUND, g_windowDefaultTextColor = DEFAULT_TEXT_COLOR, g_separatorColor;
BOOL g_hasSeparatorColor, g_colorizeBackground, g_canAnimatedDrawLogo, g_hasDefaultTextColor, g_removeFirstFocus;

INT DefaultPosition(INT i, const std::wstring& id) {
    auto dwa = g_defaultWindowAttributes[id];
    switch (i) {
        case 0: return dwa.x;
        case 1: return dwa.y;
        case 2: return dwa.width;
        case 3: return dwa.height;
        default: return NULL;
    }
}

void DefinedWindowAttributes::pos() {
    if (position.empty()) {
        x = DefaultPosition(0, id);
        y = DefaultPosition(1, id);
        width = DefaultPosition(2, id);
        height = DefaultPosition(3, id);
        return;
    }
    INT* array[] = { &x, &y, &width, &height };

    size_t start = 0, pos;
    for (INT i = 0; i < 4; i++) {
        pos = position.find(L';', start);
        if (pos == std::wstring::npos) {
            pos = position.size();
        }
        if (pos < start) {
            *array[i] = DefaultPosition(i, id);
            continue;
        }
        INT value;
        auto part = position.substr(start, pos - start);
        if (part.empty() || part == L"d") {
            value = DefaultPosition(i, id);
        } else {
            value = _wtoi(part.c_str());
        }
        *array[i] = value;
        start = pos + 1;
    }
}

HICON LoadCustomIcon(const std::wstring& source) {
    size_t pos = source.find(L';');
    // <dll>;<index>
    if (pos != std::wstring::npos) {
        auto dllPath = source.substr(0, pos);
        INT index = _wtoi(source.substr(pos + 1).c_str());

        HICON hIcon = nullptr;
        ExtractIconExW(dllPath.c_str(), index, &hIcon, nullptr, 1);
        return hIcon;
    }

    // full path to .ico
    return (HICON)LoadImageW(nullptr, source.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
}

WindowAttributes GetWindowAttributes(HWND hWnd, const std::wstring& id) {
    WindowAttributes attr = {};
    attr.id = id;
    attr.hWnd = hWnd;

    RECT rect;
    if (GetWindowRect(hWnd, &rect)) {
        MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), reinterpret_cast<POINT*>(&rect), 2);
        attr.x = rect.left;
        attr.y = rect.top;
        attr.width = rect.right - rect.left;
        attr.height = rect.bottom - rect.top;
    }

    WCHAR buffer[256];
    INT len = GetWindowTextW(hWnd, buffer, 256);
    if (len > 0) {
        attr.label.assign(buffer, len);
    }

    return attr;
}

void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) {
        return;
    }
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::wstring::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

BOOL LoadColor(const std::wstring& settingsName, COLORREF& color) {
    auto colorStr = Wh_GetStringSetting(settingsName.c_str());
    if (*colorStr) {
        INT r = 0, g = 0, b = 0;
        if (swscanf(colorStr, L"%d%*[,]%d%*[,]%d", &r, &g, &b) == 3) {
            color = RGB(r, g, b);
            Wh_FreeStringSetting(colorStr);
            return TRUE;
        }
    } 
    Wh_FreeStringSetting(colorStr);
    return FALSE;
}

BOOL LoadColor(const std::wstring& settingsName, COLORREF& color, COLORREF defaultColor) {
    if (!LoadColor(settingsName, color)) {
        color = defaultColor;
        return FALSE;
    }
    return TRUE;
}

void LoadSettings() {
    g_definedWindowAttributes.clear();
    g_hasDefaultTextColor = LoadColor(L"windowColors.textColor", g_windowDefaultTextColor, DEFAULT_TEXT_COLOR);
    g_removeFirstFocus = Wh_GetIntSetting(L"removeFirstFocus");

    for (INT i = 0;; i++) {
        auto option = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"modifyWindows[%d].id", i));
        if (!*option) {
            break;
        }

        auto label = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"modifyWindows[%d].label", i));
        auto position = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"modifyWindows[%d].position", i));
        auto zIndex = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"modifyWindows[%d].zIndex", i));

        DefinedWindowAttributes attrs = {};
        attrs.id = option;
        attrs.label = *label ? label.get() : L"";
        attrs.position = *position ? position.get() : L"";
        attrs.relativePosition = Wh_GetIntSetting(L"modifyWindows[%d].relativePosition", i);
        attrs.hidden = Wh_GetIntSetting(L"modifyWindows[%d].hidden", i);
        attrs.hasTextColor = LoadColor(std::format(L"modifyWindows[{}].textColor", i).c_str(), attrs.textColor , g_windowDefaultTextColor);
        attrs.insertAfter = *zIndex ? zIndex.get() : L"Default";
        g_definedWindowAttributes[option.get()] = attrs;
    }

    g_colorizeBackground = LoadColor(L"windowColors.background", g_windowBackgroundColor, DEFAULT_BACKGROUND);
    g_hasSeparatorColor = LoadColor(L"windowColors.separator", g_separatorColor);
}

RECT LoadPosition(const std::wstring& settingsPrefix, LONG minWidth, LONG minHeight) {
    RECT rect;
    rect.left = Wh_GetIntSetting((settingsPrefix + L".x").c_str());
    rect.top = Wh_GetIntSetting((settingsPrefix + L".y").c_str());
    rect.right = Wh_GetIntSetting((settingsPrefix + L".width").c_str());
    rect.bottom = Wh_GetIntSetting((settingsPrefix + L".height").c_str());
    if (minWidth > 0) {
        rect.right = std::max(minWidth, rect.right);
    }
    if (minHeight > 0) {
        rect.bottom = std::max(minHeight, rect.bottom);
    }
    return rect;
}

std::wstring _substr(std::wstring str, size_t pos, INT npos) {
    if (npos < 0) {
        return str.substr(pos);
    }
    return str.substr(pos, npos);
}

void ApplyPlaceholder(std::wstring& str) {
    for (auto& id : g_controlIds) {
        auto label = g_defaultWindowAttributes[id.first].label;
        std::wstring placeholder0 = L"{default." + id.first + L"}";
        ReplaceAll(str, placeholder0, label);
        for (size_t pos = 0; (pos = str.find(L"{default." + id.first, pos)) != std::wstring::npos;) {
            std::wstring placeholder1 = L"{default." + id.first + L"::";
            if (str.contains(placeholder1)) {
                size_t placeholderPos = str.find(placeholder1);
                size_t placeholderPosEnd = placeholder1.length();
                INT current = 0;
                INT positions[2] = {
                    0, -1
                };
                std::wstring position = L"";
                for (size_t pos = placeholderPos + placeholder1.length(); pos < str.length(); pos++) {
                    placeholderPosEnd++;
                    if (iswdigit(str[pos])) {
                        position += str[pos];
                    } else if (str[pos] == L'/' || str[pos] == L'}') {
                        // Very dirty way of doing this lol
                        positions[current] = stoi(position);
                        if (str[pos] == L'}') {
                            break;
                        }
                        position = L"";
                        current = 1;
                    }
                }
                ReplaceAll(str, str.substr(placeholderPos, placeholderPosEnd), label.substr(positions[0], positions[1]));
            }
        }
    }
}

void KillGifTimer() {
    if (g_logoImage && g_gif.timerId) {
        KillTimer(g_logoImage, g_gif.timerId);
        g_gif.timerId = 0;
    }
}

UINT ClampDelayMs(UINT d10) {
    UINT ms = d10 ? d10 * 10U : 100U;
    return ms < 10 ? 10 : ms;
}

BOOL LoadGifAnimationData(Gdiplus::Image* img, GifAnimation& ga) {
    UINT sz = img->GetPropertyItemSize(PropertyTagFrameDelay);
    if (sz == 0) {
        return FALSE;
    }
    auto buf = std::unique_ptr<BYTE[]>(new BYTE[sz]);
    auto pi = reinterpret_cast<Gdiplus::PropertyItem*>(buf.get());
    if (img->GetPropertyItem(PropertyTagFrameDelay, sz, pi) != Gdiplus::Ok) {
        return FALSE;
    }
    ga.count = img->GetFrameCount(&ga.dim);
    ga.delays10ms.resize(ga.count);
    auto d = reinterpret_cast<UINT*>(pi->value);
    for (UINT i = 0; i < ga.count; ++i) {
        ga.delays10ms[i] = d[i];
    }
    return ga.count > 1;
}

void CalculateImageSize(Gdiplus::Image* img, INT& width, INT& height, RECT* rect) {
    if (height == -1) {
        height = MAX_LOGO_HEIGHT;
    }

    if (width == 0 && height > 0) {
        width = static_cast<INT>(img->GetWidth() * height / img->GetHeight());
    }

    if (width == 0) {
        width = img->GetWidth();
    }

    if (height == 0 && width > 0) {
        height = static_cast<INT>(img->GetHeight() * width / img->GetWidth());
    }

    if (rect) {
        rect->right = width;
        rect->bottom = height;
    }
}

BOOL InitGdi() {
    if (g_gdiToken == 0) {
        Gdiplus::GdiplusStartupInput input;
        return Gdiplus::GdiplusStartup(&g_gdiToken, &input, nullptr) == Gdiplus::Ok;
    }
    return TRUE;
}

std::unique_ptr<Gdiplus::Bitmap> LoadStaticImageFile(const std::wstring& path, INT width, INT height, RECT* rect) {
    if (!InitGdi()) {
        return nullptr;
    }

    std::unique_ptr<Gdiplus::Image> img(Gdiplus::Image::FromFile(path.c_str(), FALSE));
    if (!img || img->GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }

    CalculateImageSize(img.get(), width, height, rect);

    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppPARGB);
    Gdiplus::Graphics g(bitmap.get());
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.DrawImage(img.get(), Gdiplus::Rect(0, 0, width, height), 0, 0, img->GetWidth(), img->GetHeight(), Gdiplus::UnitPixel);
    return bitmap;
}

BOOL LoadAnimatedImageFile(const std::wstring& path, INT& width, INT& height, RECT* rect) {
    if (!InitGdi()) {
        return FALSE;
    }

    std::unique_ptr<Gdiplus::Image> img(Gdiplus::Image::FromFile(path.c_str(), FALSE));
    if (!img || img->GetLastStatus() != Gdiplus::Ok) {
        return FALSE;
    }

    GUID dim = Gdiplus::FrameDimensionTime;
    UINT frameCount = img->GetFrameCount(&dim);
    if (frameCount <= 1) {
        return FALSE;
    }

    GifAnimation ga;
    if (!LoadGifAnimationData(img.get(), ga)) {
        return FALSE;
    }

    CalculateImageSize(img.get(), width, height, rect);

    ga.img = std::move(img);
    ga.width = width;
    ga.height = height;
    g_gif = std::move(ga);
    return TRUE;
}

BOOL CALLBACK EnumMonitorsProc(HMONITOR handle, HDC, LPRECT, LPARAM monitorsParam) {
    auto monitors = reinterpret_cast<std::vector<MonitorInfoEx>*>(monitorsParam);
    MonitorInfoEx info = {};
    info.handle = handle;
    info.info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfoW(handle, &info.info);
    monitors->push_back(info);
    return TRUE;
}

BOOL FileExists(PCWSTR path) {
    if (!path || !*path) {
        return FALSE;
    }
    DWORD attrs = GetFileAttributesW(path);
    return (attrs != INVALID_FILE_ATTRIBUTES) && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

void DeleteLogoImage() {
    g_logoImageBitmap.reset();
    KillGifTimer();
    g_gif = {};
}

void CenterWindow(HWND hWnd, RECT& rect, INT& x, INT& y) {
    std::vector<MonitorInfoEx> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc, reinterpret_cast<LPARAM>(&monitors));

    HMONITOR monitor = nullptr;
    const RECT* monitorRect = nullptr;

    INT index = Wh_GetIntSetting(L"startPosition.monitor") - 1;
    if (index < 0 || index >= (INT)monitors.size()) {
        monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(MONITORINFO) };
        if (!GetMonitorInfoW(monitor, &mi)) {
            return;
        }
        monitorRect = &mi.rcWork;
    } else {
        monitor = monitors[index].handle;
        monitorRect = &monitors[index].info.rcWork;
    }

    x = monitorRect->left + ((monitorRect->right - monitorRect->left) - (rect.right - rect.left)) / 2;
    y = monitorRect->top  + ((monitorRect->bottom - monitorRect->top) - (rect.bottom - rect.top)) / 2;
}

void UpdatePosition(HWND hWnd) {
    RECT rectWindow;

    if (Wh_GetIntSetting(L"windowSize.enabled")) {
        GetWindowRect(hWnd, &rectWindow);
        INT width = Wh_GetIntSetting(L"windowSize.width"), height = Wh_GetIntSetting(L"windowSize.height");
        UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION;
        if (width <= 0 && height <= 0) {
            width = 0;
            height = 0;
            flags = 0;
        } else if (width <= 0) {
            width = rectWindow.right - rectWindow.left;
        } else if (height <= 0) {
            height = rectWindow.bottom - rectWindow.top;
        }
        if (flags != 0) {
            Wh_Log(L"Window size -> %d x %d", width, height);
            SetWindowPos(hWnd, nullptr, 0, 0, width, height, flags);
        }
    }

    if (Wh_GetIntSetting(L"startPosition.enabled")) {
        GetWindowRect(hWnd, &rectWindow);
        RECT rectOffset = LoadPosition(
            L"startPosition",
            GetSystemMetrics(SM_CXMINTRACK),
            GetSystemMetrics(SM_CYMINTRACK)
        );
        INT x = 0, y = 0;
        if (Wh_GetIntSetting(L"startPosition.center")) {
            CenterWindow(hWnd, rectWindow, x, y);
        }
        x += rectOffset.left;
        y += rectOffset.top;
        Wh_Log(L"Positioning window at %d %d", x, y);
        SetWindowPos(hWnd, Wh_GetIntSetting(L"alwaysOnTop") ? HWND_TOPMOST : HWND_NOTOPMOST, x, y, 0, 0, SWP_NOSIZE);
    }
}

void UpdateWindowColors(HWND hWnd) {
    COLORREF color; // Reuable variable
    LoadColor(L"windowColors.title", color, GetSysColor(COLOR_CAPTIONTEXT));
    DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &color, sizeof(color));
    LoadColor(L"windowColors.bar", color, DEFAULT_BAR_BACKGROUND);
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
    LoadColor(L"windowColors.border", color, GetSysColor(COLOR_WINDOW));
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
}

void UpdateTitle(HWND hWnd) {
    static WCHAR originalTitle[256] = L"";
    if (originalTitle[0] == L'\0') {
        GetWindowTextW(hWnd, originalTitle, ARRAYSIZE(originalTitle));
    }
    auto titleText = Wh_GetStringSetting(L"titleText");
    if (titleText && *titleText) {
        SetWindowTextW(hWnd, titleText);
    } else {
        SetWindowTextW(hWnd, originalTitle);
    }
    Wh_FreeStringSetting(titleText);
}

void PostGifBackgroundRedrawMessage(HWND hWnd) {
    g_canAnimatedDrawLogo = FALSE;
    PostMessageW(hWnd, WM_CAPTURE_LOGO_BACKGROUND, 0, 0);
}

void UpdateLogo(HWND hWnd, BOOL processBackground) {
    if (g_logoImage) {
        DeleteLogoImage();
        ShowWindow(g_logoImage, SW_HIDE);
    }

    if (processBackground) {
        KillGifTimer();
        PostGifBackgroundRedrawMessage(hWnd);
    }

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    if (!g_logoImage) {
        g_logoImage = CreateWindowExW(0, WC_STATICW, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)1, GetModuleHandleW(nullptr), nullptr);
        WindhawkUtils::SetWindowSubclassFromAnyThread(g_logoImage, WndSubclassProc, LOGO_IMAGE_ID);
        Wh_Log(L"Logo's image handle -> %p", g_logoImage);
    }

    if (!Wh_GetIntSetting(L"logo.enabled")) {
        ShowWindow(g_logoImage, SW_HIDE);
        return;
    }

    auto path = Wh_GetStringSetting(L"logo.path");
    if (!path || !*path || !FileExists(path)) {
        Wh_Log(L"Invalid logo file!");
        g_errorMessages.push_back(std::format(L"Path to custom logo is invalid or the file doesn't exist!\n\n{}", path));
        Wh_FreeStringSetting(path);
        ShowWindow(g_logoImage, SW_HIDE);
        return;
    }

    RECT rect = LoadPosition(L"logo", 0L, -1L);
    PrintRect2(L"Logo position from settings", rect);

    Wh_Log(L"Loading %ls", path);

    // Try loading animated image
    INT width = rect.right, height = rect.bottom;
    if (LoadAnimatedImageFile(path, width, height, &rect)) {
        Wh_Log(L"Image type -> animated");
        Wh_FreeStringSetting(path);
        g_logoImageBitmap = nullptr;
        Wh_Log(L"Animated logo image loaded");
        if (Wh_GetIntSetting(L"logo.center")) {
            PrintRect2(L"Logo position before centering", rect);
            rect.left = (clientRect.right - clientRect.left) / 2 - rect.right / 2;
            PrintRect2(L"Logo position after centering", rect);
        }
        MoveWindow(g_logoImage, rect.left, rect.top, rect.right, rect.bottom, TRUE);
        g_gif.frame = 0;
        g_gif.img->SelectActiveFrame(&g_gif.dim, g_gif.frame);
        SetTimer(g_logoImage, g_gif.timerId, ClampDelayMs(g_gif.delays10ms[g_gif.frame % g_gif.count]), nullptr);
        PrintRect2(L"Logo position", rect);
    } else {
        Wh_Log(L"Image type -> static");
        g_gif = {};
        g_logoImageBitmap = LoadStaticImageFile(path, rect.right, rect.bottom, &rect);
        Wh_FreeStringSetting(path);
        if (g_logoImageBitmap) {
            Wh_Log(L"Logo image loaded");
            if (Wh_GetIntSetting(L"logo.center")) {
                PrintRect2(L"Logo position before centering", rect);
                rect.left = (clientRect.right - clientRect.left) / 2 - rect.right / 2;
                PrintRect2(L"Logo position after centering", rect);
            }
            MoveWindow(g_logoImage, rect.left, rect.top, rect.right, rect.bottom, TRUE);
            PrintRect2(L"Logo position", rect);
        } else {
            Wh_Log(L"Failed to load logo: %ls", GetLastError());
        }
    }
    ShowWindow(g_logoImage, SW_SHOW);
}

void UpdateZIndex(HWND hWnd, HWND item, std::wstring& insertAfterStr) {
    if (insertAfterStr != L"Default") {
        return;
    }

    HWND insertAfter;
    if (insertAfterStr == L"Top") {
        insertAfter = HWND_TOP;
    } else if (insertAfterStr == L"Bottom") {
        insertAfter = HWND_BOTTOM;
    } else if (insertAfterStr == L"LogoImage") {
        insertAfter = g_logoImage;
    } else {
        auto _window = g_controlIds.find(insertAfterStr);
        if (_window == g_controlIds.end()) {
            return;
        }
        insertAfter = GetDlgItem(hWnd, _window->second);
        if (!insertAfter) {
            return;
        }
    }

    Wh_Log(L"Z-INDEX: %p %ls", insertAfter, insertAfterStr.c_str());
    SetWindowPos(item, insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void UpdateControls(HWND hWnd) {
    if (g_defaultWindowAttributes.empty()) {
        for (auto& id : g_controlIds) {
            HWND item = GetDlgItem(hWnd, id.second);
            if (!item) {
                Wh_Log(L"Failed to find %ls (%d) window", id.first.c_str(), id.second);
                continue;
            }
            g_defaultWindowAttributes[id.first] = GetWindowAttributes(item, id.first);
        }
    }

    for (auto& entry : g_definedWindowAttributes) {
        entry.second.pos();
        auto id = g_controlIds.find(entry.first);
        if (id != g_controlIds.end()) {
            HWND item = GetDlgItem(hWnd, id->second);
            if (!item) {
                continue;
            }

            entry.second.hWnd = item;
            Wh_Log(L"Found child window %ls (%d)", id->first.c_str(), id->second);

            // Modify caption
            auto label = entry.second.label;
            ApplyPlaceholder(label);
            SetWindowTextW(item, label.c_str());

            // Modify visibility
            ShowWindow(item, entry.second.hidden ? SW_HIDE : SW_SHOW);

            DefinedWindowAttributes copy = entry.second;
            if (copy.relativePosition) {
                copy.x += g_defaultWindowAttributes[id->first].x;
                copy.y += g_defaultWindowAttributes[id->first].y;
            }
            
            // Modify position
            MoveWindow(item, copy.x, copy.y, entry.second.width, entry.second.height, TRUE);

            // Modify Z index
            UpdateZIndex(hWnd, item, entry.second.insertAfter);

            RECT rect;
            GetWindowRect(item, &rect);
            MapWindowPoints(nullptr, hWnd, reinterpret_cast<POINT*>(&rect), 2);
            PrintRect(L"Window (new) position", rect);
        }
    }
}

void SetCustomIcon(HWND hWnd) {
    auto customIcon = Wh_GetStringSetting(L"customIcon");
    if (*customIcon) {
        HICON hIcon = LoadCustomIcon(customIcon);
        if (hIcon) {
            SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            if (Wh_GetIntSetting(L"removeTitleIcon")) {
                SendMessageW(hWnd, WM_SETICON, ICON_SMALL, 0);
                LONG style = GetWindowLongW(hWnd, GWL_STYLE);
                style &= ~WS_SYSMENU;
                SetWindowLongW(hWnd, GWL_STYLE, style);
            } else {
                SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            }
        } else {
            g_errorMessages.push_back(std::format(L"Failed to load icon ({}), please check the settings!", customIcon));
        }
    }
    Wh_FreeStringSetting(customIcon);
}

void ToggleCloseButton(HWND hWnd) {
    if (Wh_GetIntSetting(L"restoreCloseButton")) {
        LONG style = GetWindowLongW(hWnd, GWL_STYLE);
        style |= WS_SYSMENU;
        SetWindowLongW(hWnd, GWL_STYLE, style);
    }
}

void ExpandControls(HWND hWnd, RECT originalWindowRect) {
    if (Wh_GetIntSetting(L"windowSize.enabled") && Wh_GetIntSetting(L"windowSize.expandControls")) {
        Wh_Log(L"EXPAND CONTROLS");
        RECT windowRect;
        GetWindowRect(hWnd, &windowRect);
        INT diffX = (windowRect.right - windowRect.left) - (originalWindowRect.right - originalWindowRect.left);
        INT diffY = (windowRect.bottom - windowRect.top) - (originalWindowRect.bottom - originalWindowRect.top);
        Wh_Log(L"Difference between old and new item sizes: x -> %d, y -> %d", diffX, diffY);
        for (auto& control : g_defaultWindowAttributes) {
            if (control.first == L"LeftIcon") {
                continue;
            }
            if (IsWindow(control.second.hWnd)) {
                RECT rect;
                GetWindowRect(control.second.hWnd, &rect);
                MapWindowPoints(nullptr, hWnd, reinterpret_cast<POINT*>(&rect), 2);
                if (control.first == L"OkButton") {
                    rect.left += diffX;
                    if (Wh_GetIntSetting(L"windowSize.exceptionalOkButton")) {
                        rect.bottom += diffY;
                        rect.top += diffY;
                    }
                }
                MoveWindow(control.second.hWnd, rect.left, rect.top, rect.right - rect.left + diffX, rect.bottom - rect.top, TRUE);
            }
        }
    }
}

void Update(HWND hWnd, BOOL processLogobackground, BOOL repositionWindow) {
    Wh_Log(L"UPDATING CUSTOM ICON");
    SetCustomIcon(hWnd);
    if (repositionWindow) {
        Wh_Log(L"UPDATING POSITION");
        UpdatePosition(hWnd);
    }
    Wh_Log(L"UPDATING CONTROLS");
    UpdateControls(hWnd);
    Wh_Log(L"UPDATING TITLE");
    UpdateTitle(hWnd);
    Wh_Log(L"UPDATING WINDOW COLORS");
    UpdateWindowColors(hWnd);
    Wh_Log(L"UPDATING LOGO");
    UpdateLogo(hWnd, processLogobackground);
    Wh_Log(L"DONE");
}

void ShowErrorsIfAny(HWND hWnd) {
    for (auto& message : g_errorMessages) {
        MessageBoxW(hWnd, message.c_str(), MOD_TITLE, MB_OK | MB_ICONERROR);
    }
    g_errorMessages.clear();
}

HBITMAP CapturePixelsToBitmap(const RECT& rect) {
    RECT clientRect;
    GetClientRect(g_mainWindow, &clientRect);
    const INT W = clientRect.right;
    const INT H = clientRect.bottom;

    HDC hdc = GetDC(g_mainWindow);
    if (!hdc) {
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* fullBits = nullptr;
    HBITMAP fullBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &fullBits, nullptr, 0);
    if (!fullBmp) {
        ReleaseDC(g_mainWindow, hdc);
        return nullptr;
    }

    HDC fullDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldFull = SelectObject(fullDC, fullBmp);

    if (!PrintWindow(g_mainWindow, fullDC, PW_CLIENTONLY | PW_RENDERFULLCONTENT)) {
        BitBlt(fullDC, 0, 0, W, H, hdc, 0, 0, SRCCOPY);
    }

    const INT width  = rect.right  - rect.left;
    const INT height = rect.bottom - rect.top;

    BITMAPINFO bmiCut = bmi;
    bmiCut.bmiHeader.biWidth  = width;
    bmiCut.bmiHeader.biHeight = -height;

    void* cutBits = nullptr;
    HBITMAP cutBmp = CreateDIBSection(hdc, &bmiCut, DIB_RGB_COLORS, &cutBits, nullptr, 0);
    if (!cutBmp) {
        SelectObject(fullDC, oldFull);
        DeleteObject(fullBmp);
        DeleteDC(fullDC);
        ReleaseDC(g_mainWindow, hdc);
        return nullptr;
    }

    HDC cutDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldCut = SelectObject(cutDC, cutBmp);

    BitBlt(cutDC, 0, 0, width, height, fullDC, rect.left, rect.top, SRCCOPY);

    SelectObject(fullDC, oldFull);
    SelectObject(cutDC, oldCut);
    DeleteDC(fullDC);
    DeleteDC(cutDC);
    DeleteObject(fullBmp);
    ReleaseDC(g_mainWindow, hdc);

    return cutBmp;
}

// Window processing for both main window and its children (dwRefData = 0 is main window, anything else - child)
LRESULT CALLBACK WndSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    static COLORREF c_windowBackgroundColor, c_separatorColor;
    static HBRUSH c_windowBackgroundBrush, c_separatorBrush;
    static const INT separatorId = g_controlIds.find(L"Separator")->second;
    static HBITMAP behindLogoImage;

    if (c_windowBackgroundColor != g_windowBackgroundColor) {
        c_windowBackgroundBrush = CreateSolidBrush(g_windowBackgroundColor);
        c_windowBackgroundColor = g_windowBackgroundColor;
    }

    if (g_hasSeparatorColor && c_separatorColor != g_separatorColor) {
        c_separatorBrush = CreateSolidBrush(g_separatorColor);
        c_separatorColor = g_separatorColor;
    }

    if ((uMsg == WM_CTLCOLORSTATIC || uMsg == WM_CTLCOLORBTN) && dwRefData == 0) { // Set background color for controls
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        for (auto& control : g_defaultWindowAttributes) {
            if (control.second.hWnd == (HWND)lParam) {
                auto found = g_definedWindowAttributes.find(control.first);
                if (found != g_definedWindowAttributes.end() && found->second.hasTextColor) {
                    SetTextColor(hdc, found->second.textColor);
                } else if (g_hasDefaultTextColor) {
                    SetTextColor(hdc, g_windowDefaultTextColor);
                }
            }
        }
        return reinterpret_cast<LRESULT>(c_windowBackgroundBrush);
    } else if (uMsg == WM_ERASEBKGND && dwRefData == LOGO_IMAGE_ID) {
        return FALSE;
    } else if (uMsg == WM_PAINT) {
        if (dwRefData == 0) { // Set background color for window itself
            if (g_colorizeBackground) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                FillRect(hdc, &ps.rcPaint, c_windowBackgroundBrush);
                EndPaint(hWnd, &ps);
                return FALSE;
            }
        } else if (dwRefData == LOGO_IMAGE_ID && (g_logoImageBitmap || g_gif.valid())) { // Draw custom logo
            if (g_gif.valid()) {
                if (!g_canAnimatedDrawLogo) {
                    return FALSE;
                }
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                RECT rc;
                GetClientRect(hWnd, &rc);
                INT width = rc.right;
                INT height = rc.bottom;

                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP memBmp = CreateCompatibleBitmap(hdc, width, height);
                HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

                if (behindLogoImage) { // Paint background
                    HDC srcDC = CreateCompatibleDC(hdc);
                    HBITMAP oldSrc = (HBITMAP)SelectObject(srcDC, behindLogoImage);
                    BITMAP bm;
                    GetObjectW(behindLogoImage, sizeof(bm), &bm);
                    BitBlt(memDC, 0, 0, bm.bmWidth, bm.bmHeight, srcDC, 0, 0, SRCCOPY);
                    SelectObject(srcDC, oldSrc);
                    DeleteDC(srcDC);
                }

                Gdiplus::Graphics g(memDC);
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.DrawImage(g_gif.img.get(), 0, 0, g_gif.width, g_gif.height);

                BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, oldBmp);
                DeleteObject(memBmp);
                DeleteDC(memDC);
                EndPaint(hWnd, &ps);
            } else {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                Gdiplus::Graphics g(hdc);
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.DrawImage(g_logoImageBitmap.get(), 0, 0, g_logoImageBitmap->GetWidth(), g_logoImageBitmap->GetHeight());
                EndPaint(hWnd, &ps);
            }
            return FALSE;
        } else if (static_cast<INT>(dwRefData) == separatorId && g_hasSeparatorColor) { // Recolor separator
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT mRect;
            GetClientRect(hWnd, &mRect);
            // There seems to be some white pixels left out on both sides
            mRect.left -= 1;
            mRect.right += 1;
            FrameRect(hdc, &mRect, c_separatorBrush);
            EndPaint(hWnd, &ps);
            return FALSE;
        }
    } else if (uMsg == WM_DESTROY && dwRefData == 0) {
        DeleteObject(c_windowBackgroundBrush);
        DeleteObject(c_separatorBrush);
        c_windowBackgroundBrush = nullptr;
        c_separatorBrush = nullptr;
        behindLogoImage = nullptr;
    } else if (uMsg == WM_CAPTURE_LOGO_BACKGROUND && dwRefData == 0) {
        if (!g_canAnimatedDrawLogo && g_gif.valid()) {
            RECT rect;
            GetClientRect(hWnd, &rect);
            MapWindowPoints(g_logoImage, g_mainWindow, reinterpret_cast<POINT*>(&rect), 2);
            // The combination of two lines below fixes animated logo's background (it would randomly be black sometimes)
            ShowWindow(g_mainWindow, SW_SHOW); // Force window to be shown just in case
            Sleep(10); // Sleeping seems to help
            behindLogoImage = CapturePixelsToBitmap(rect);
            if (!behindLogoImage) {
                Wh_Log(L"Failed to capture background for animated logo");
            }
            g_canAnimatedDrawLogo = TRUE;
            RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
        }
    } else if (uMsg == WM_TIMER) {
        if (g_gif.valid() && wParam == g_gif.timerId && dwRefData == LOGO_IMAGE_ID) {
            if (!g_canAnimatedDrawLogo) {
                return FALSE;
            }
            g_gif.frame = (g_gif.frame + 1) % g_gif.count;
            g_gif.img->SelectActiveFrame(&g_gif.dim, g_gif.frame);
            SetTimer(hWnd, g_gif.timerId, ClampDelayMs(g_gif.delays10ms[g_gif.frame]), nullptr);
            RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
            return FALSE;
        }
    } else if (uMsg == WM_DESTROY && dwRefData == LOGO_IMAGE_ID) {
        KillGifTimer();
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hWnd || !IsWindow(hWnd)) {
        return;
    }

    if (event == EVENT_OBJECT_CREATE && !g_mainWindow) {
        if (!GetWindow(hWnd, GW_OWNER)) {
            g_mainWindow = hWnd;
            Wh_Log(L"Window created");
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, WndSubclassProc, 0);
            for (auto& entry : g_controlIds) {
                HWND item = GetDlgItem(hWnd, entry.second);
                if (!item) {
                    Wh_Log(L"Failed to find window child (%p)", entry.second);
                    continue;
                }
                WindhawkUtils::SetWindowSubclassFromAnyThread(item, WndSubclassProc, entry.second);
            }
            RECT originalWindowRect;
            GetWindowRect(hWnd, &originalWindowRect);
            Update(hWnd, FALSE, TRUE);
            ExpandControls(hWnd, originalWindowRect);
        }
    } else if (g_mainWindow == hWnd) {
        Wh_Log(L"Window showed");
        ToggleCloseButton(hWnd);
        UnhookWinEvent(g_hHook); // No more needed
        g_hHook = nullptr;
        ShowErrorsIfAny(hWnd);
        if (g_removeFirstFocus) {
            SetFocus(nullptr);
        }
        PostMessageW(hWnd, WM_CAPTURE_LOGO_BACKGROUND, 0, 0);
    }
}

BOOL CALLBACK EnumMessageBoxProc(HWND hwnd, LPARAM) {
    if (GetWindow(hwnd, GW_OWNER) == g_mainWindow) {
        WCHAR cls[64];
        GetClassNameW(hwnd, cls, _countof(cls));
        if (wcscmp(cls, L"#32770") == 0) {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        }
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_hHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr, WinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_hHook) {
        UnhookWinEvent(g_hHook);
    }
    // Partially reset some stuff
    if (IsWindow(g_logoImage)) {
        DeleteLogoImage();
        PostMessageW(g_logoImage, WM_CLOSE, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_logoImage, WndSubclassProc);
    }
    if (IsWindow(g_mainWindow)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_mainWindow, WndSubclassProc);
        EnumWindows(EnumMessageBoxProc, 0);
        for (auto& entry : g_controlIds) {
            HWND item = GetDlgItem(g_mainWindow, entry.second);
            if (!item) {
                continue;
            }
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(item, WndSubclassProc);
        }
        RedrawWindow(g_mainWindow, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_ERASENOW);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    if (IsWindow(g_mainWindow)) {
        Update(g_mainWindow, TRUE, FALSE);
        RedrawWindow(g_mainWindow, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_ERASENOW);
        ToggleCloseButton(g_mainWindow);
        ShowErrorsIfAny(g_mainWindow);
    }
}
