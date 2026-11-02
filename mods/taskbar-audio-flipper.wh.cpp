// ==WindhawkMod==
// @id              taskbar-audio-flipper
// @name            Taskbar Audio Flipper
// @description     A taskbar widget that switches between your speakers, headsets and microphones with a click, and turns a webcam on and off
// @version         1.3.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -loleaut32 -luuid -lsetupapi -lcfgmgr32 -lversion -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Audio Flipper

![The widget on the taskbar, with its Output, Input and Webcam sections](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-audio-flipper/screenshot-1.3.png)

A taskbar widget, in the style of the Windows weather widget, for switching audio devices. It has
up to three sections, which you can each show or hide:

- **Output**: the active output device's icon, name and volume. Click it to switch to the next
  output (speakers → headset → monitor → …).
- **Input**: the same for your microphone.
- **Webcam**: the chosen webcam's name and whether it's on, with a camera icon. Click it to turn
  the webcam on or off.

On the Output and Input sections:

- **Hover**: a flyout opens above it with the device's name, a volume slider, and the device icon
  as a mute button, like the Windows 11 volume flyout. The output's has a volume mixer too: each
  app using the output, with its icon (click it to mute the app) and volume, and a gear for
  Windows' volume mixer settings. **More volume settings** at the bottom opens Windows' Sound
  settings.
- **Left-click**: switch to the next device.
- **Scroll**: turn the volume (or microphone level) up or down.
- **Middle-click**: mute / unmute.

After you change the output volume, the mod plays the Windows "Default Beep" ding at the new level,
like Windows' own volume slider. Turn it off with **Output → Ding when changing volume**.

Anywhere on the widget, **right-click** for the menu. The **Output**, **Input** and **Webcam** submenus each have a **Show on taskbar** switch for
their section, and the settings for it: picking a device directly, which devices are in the click
rotation, whether to switch the communications device too, and links to the Windows settings.

**Position** in the mod's settings puts the widget left of the notification area, of the Wi-Fi,
volume and battery icons, or of the clock, where the taskbar makes room for it. It can also go at
the left end of the taskbar, over it: that's free when the taskbar's icons are centered and the
Widgets button is off.

Devices are shown with the name they have in the Windows Sound control panel, and a Fluent icon for
their kind (speakers, headphones, headset, monitor or microphone) in the taskbar's color, like the
rest of Windows 11, struck through while muted. To change a device's name, choose **Rename
devices…** in the **Output** or **Input** submenu, right-click a device, and choose **Properties**.

## Webcam on/off

Turn it on under **Webcam → Show on taskbar** and pick the webcam to control in the same menu. The
camera icon has a slash through it while the webcam is off; click it to switch. Windows has no way to
rename a webcam, but **Webcam → Rename…** gives it a name to show on the widget.

Turning the webcam off works like **Disable device** in Device Manager: no app can use it until it's
turned back on. It stays off after a restart, and after the mod is turned off or removed. To turn it
back on without the mod, open Device Manager, find the webcam under **Cameras** (or **Imaging
devices**), right-click it and choose **Enable device**.

Windows only lets administrators turn devices on and off, so each switch runs Microsoft's PnP Utility
(`pnputil.exe`) as administrator, which may show a Windows prompt asking to allow it, depending on
your User Account Control settings.

## Next to the other widgets

![Taskbar Snipper, Taskbar Audio Flipper and Taskbar RDP Launcher side by side on the taskbar](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-widgets/side-by-side.png)

From the left: [Taskbar Snipper](https://windhawk.net/mods/taskbar-snipper),
[Taskbar Audio Flipper](https://windhawk.net/mods/taskbar-audio-flipper) and
[Taskbar RDP Launcher](https://windhawk.net/mods/taskbar-rdp-launcher). With their mods running
too, widgets in the same position sit side by side, in that order; right-click one and choose
**Move left** or **Move right** to change it.

## Good to know

- The mod needs Windows 11, whose taskbar the widget is part of. The first time it starts,
  Windhawk downloads Microsoft's symbols for the taskbar, to find its way in; that can take a
  moment.
- The widget is on the main display's taskbar only.

## Settings

The sections, the click rotation and the webcam settings are kept in the mod's storage, which
Windhawk removes along with the mod. The first time it runs, the mod takes on the settings of the
AudioFlipper app, if you have it (from `%APPDATA%\AudioFlipper\settings.txt`, which it doesn't
change). Use the app or the mod, not both: each shows its own widget.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: tray
  $name: Position
  $description: Where on the taskbar the widget goes. At the left end, it's over the taskbar, which is free when its icons are centered and the Widgets button is off. The Taskbar Snipper and Taskbar RDP Launcher widgets in the same position sit beside it.
  $options:
  - tray: Left of the notification area
  - systemIcons: Left of the Wi-Fi, volume and battery icons
  - clock: Left of the clock
  - left: At the left end of the taskbar
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <propidl.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <dbt.h>
#include <gdiplus.h>

#undef GetCurrentTime  // winbase.h's, which XAML's headers have as a method.

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Where the widget goes among the Taskbar Snipper, Taskbar Audio Flipper and Taskbar RDP Launcher
// widgets in the same position on the taskbar, from the left, until it's moved from its menu.
constexpr int kWidgetOrder = 1;

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Shared by the Taskbar Audio Flipper, Taskbar RDP Launcher and Taskbar Snipper mods
// (the same code is in all three, each keeping the parts it uses): the widget in
// the taskbar, popups, menus, the text prompt, the mod's storage and launching
// programs.

namespace gp = Gdiplus;

namespace {

// ---------------------------------------------------------------------------
// Strings and files

std::wstring Trim(const std::wstring& text) {
    size_t start = 0;
    size_t end = text.size();
    while (start < end && iswspace(text[start])) {
        start++;
    }
    while (end > start && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(start, end - start);
}

bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

std::vector<std::wstring> Split(const std::wstring& text, wchar_t separator) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    while (true) {
        size_t end = text.find(separator, start);
        std::wstring part = text.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start);
        if (!part.empty()) {
            parts.push_back(part);
        }
        if (end == std::wstring::npos) {
            return parts;
        }
        start = end + 1;
    }
}

std::wstring Join(const std::vector<std::wstring>& parts, const std::wstring& separator) {
    std::wstring joined;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i > 0) {
            joined += separator;
        }
        joined += parts[i];
    }
    return joined;
}

std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return {};
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    std::wstring wide(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), wide.data(), length);
    return wide;
}

std::wstring KnownFolder(REFKNOWNFOLDERID folder) {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(folder, KF_FLAG_DEFAULT, nullptr, &path))) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

std::wstring WindowsFolder() {
    WCHAR path[MAX_PATH];
    UINT length = GetSystemWindowsDirectoryW(path, MAX_PATH);
    return length > 0 && length < MAX_PATH ? std::wstring(path, length) : L"C:\\Windows";
}

// System32, for starting the programs in it.
std::wstring SystemFolder() {
    return WindowsFolder() + L"\\System32";
}

bool ReadFileBytes(const std::wstring& path, std::string& bytes) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(file, &size) && size.QuadPart < 16 * 1024 * 1024;
    if (ok) {
        bytes.resize((size_t)size.QuadPart);
        DWORD read = 0;
        ok = bytes.empty() || (ReadFile(file, bytes.data(), (DWORD)bytes.size(), &read, nullptr) && read == bytes.size());
    }
    CloseHandle(file);
    return ok;
}

// Lines of a text file in UTF-8 or UTF-16 (with a byte order mark), as .NET reads them: the app's
// settings.txt to import.
bool ReadTextLines(const std::wstring& path, std::vector<std::wstring>& lines) {
    std::string bytes;
    if (!ReadFileBytes(path, bytes)) {
        return false;
    }
    std::wstring text;
    if (bytes.size() >= 2 && (unsigned char)bytes[0] == 0xFF && (unsigned char)bytes[1] == 0xFE) {
        text.assign((const wchar_t*)(bytes.data() + 2), (bytes.size() - 2) / 2);
    } else if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF && (unsigned char)bytes[1] == 0xBB &&
               (unsigned char)bytes[2] == 0xBF) {
        text = Utf8ToWide(bytes.substr(3));
    } else {
        text = Utf8ToWide(bytes);
    }
    lines.clear();
    std::wstring line;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == L'\r' || text[i] == L'\n') {
            if (text[i] == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') {
                i++;
            }
            lines.push_back(line);
            line.clear();
        } else {
            line += text[i];
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }
    return true;
}

// "key=value" lines, as in the apps' settings.txt files.
std::vector<std::pair<std::wstring, std::wstring>> ReadSettingsFile(const std::wstring& path) {
    std::vector<std::pair<std::wstring, std::wstring>> entries;
    std::vector<std::wstring> lines;
    if (!ReadTextLines(path, lines)) {
        return entries;
    }
    for (const std::wstring& line : lines) {
        size_t separator = line.find(L'=');
        if (line.empty() || line[0] == L'#' || separator == std::wstring::npos || separator == 0) {
            continue;
        }
        entries.emplace_back(Trim(line.substr(0, separator)), Trim(line.substr(separator + 1)));
    }
    return entries;
}

bool ParseBool(const std::wstring& value, bool fallback) {
    if (EqualsIgnoreCase(value, L"true")) {
        return true;
    }
    if (EqualsIgnoreCase(value, L"false")) {
        return false;
    }
    return fallback;
}

// ---------------------------------------------------------------------------
// The mod's storage. Windhawk keeps one for the whole PC (and removes it with the mod), while the
// mod runs separately for each signed-in Windows user, so each user's values are kept apart under
// their SID.

namespace Storage {

std::wstring ReadUserSid() {
    std::wstring sid;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size = 0;
        PWSTR text = nullptr;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &text)) {
            sid = text;
            LocalFree(text);
        }
        CloseHandle(token);
    }
    return sid.empty() ? L"User" : sid;
}

// The user's SID, which stays the same for as long as the mod runs.
const std::wstring& UserSid() {
    static const std::wstring sid = ReadUserSid();
    return sid;
}

std::wstring ValueName(const wchar_t* name) {
    return UserSid() + L"." + name;
}

int GetInt(const wchar_t* name, int fallback) {
    return Wh_GetIntValue(ValueName(name).c_str(), fallback);
}

bool SetInt(const wchar_t* name, int value) {
    return Wh_SetIntValue(ValueName(name).c_str(), value);
}

std::wstring GetString(const wchar_t* name) {
    std::vector<WCHAR> buffer(8 * 1024);  // A webcam's ID and name, and lists of a few device IDs.
    size_t length = Wh_GetStringValue(ValueName(name).c_str(), buffer.data(), buffer.size());
    return std::wstring(buffer.data(), std::min(length, buffer.size()));
}

bool SetString(const wchar_t* name, const std::wstring& value) {
    return Wh_SetStringValue(ValueName(name).c_str(), value.c_str());
}

void Delete(const wchar_t* name) {
    Wh_DeleteValue(ValueName(name).c_str());
}

}  // namespace Storage

// ---------------------------------------------------------------------------
// Windows settings and processes

bool ReadUserFlag(const wchar_t* key, const wchar_t* valueName, bool fallback) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, key, valueName, RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS) {
        return fallback;
    }
    return value != 0;
}

// The Windows setting the popups follow. It's read once, and again when Windows says a setting
// changed (WM_SETTINGCHANGE), rather than on every paint.
struct ShellSettings {
    bool lightApps = true;  // "Choose your default app mode".

    static ShellSettings Read() {
        const wchar_t* personalize = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        ShellSettings settings;
        settings.lightApps = ReadUserFlag(personalize, L"AppsUseLightTheme", true);
        return settings;
    }
};

ShellSettings& CurrentShellSettings() {
    static ShellSettings settings = ShellSettings::Read();
    return settings;
}

void RefreshShellSettings() {
    CurrentShellSettings() = ShellSettings::Read();
}

bool AppsUseLightTheme() {
    return CurrentShellSettings().lightApps;
}

// Whether this process runs as administrator (Explorer doesn't; the app can).
bool IsProcessElevated() {
    static int elevated = -1;
    if (elevated < 0) {
        elevated = 0;
        HANDLE token = nullptr;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            TOKEN_ELEVATION elevation{};
            DWORD size = 0;
            if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
                elevated = elevation.TokenIsElevated ? 1 : 0;
            }
            CloseHandle(token);
        }
    }
    return elevated == 1;
}

// Starts a program, hidden if asked. Returns its process handle (to close) in *process when asked for.
bool Launch(const std::wstring& exe, const std::wstring& arguments, bool hidden = false, HANDLE* process = nullptr) {
    std::wstring commandLine = L"\"" + exe + L"\"" + (arguments.empty() ? L"" : L" " + arguments);
    std::vector<wchar_t> commandBuffer(commandLine.begin(), commandLine.end());
    commandBuffer.push_back(L'\0');
    STARTUPINFOW startup{sizeof(startup)};
    if (hidden) {
        startup.dwFlags = STARTF_USESHOWWINDOW;
        startup.wShowWindow = SW_HIDE;
    }
    PROCESS_INFORMATION info{};
    if (!CreateProcessW(exe.c_str(), commandBuffer.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &info)) {
        return false;
    }
    CloseHandle(info.hThread);
    if (process) {
        *process = info.hProcess;
    } else {
        CloseHandle(info.hProcess);
    }
    return true;
}

// Opens a URI like ms-settings:sound, as Explorer would.
bool OpenWithShell(const std::wstring& target) {
    return (INT_PTR)ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL) > 32;
}

// ---------------------------------------------------------------------------
// Displays

UINT DpiForPoint(POINT point) {
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 96;
    }
    return dpiX;
}

// ---------------------------------------------------------------------------
// Drawing

gp::Color Argb(int a, int r, int g, int b) {
    return gp::Color((BYTE)a, (BYTE)r, (BYTE)g, (BYTE)b);
}

// A rectangle with fully rounded ends.
void FillPill(gp::Graphics& graphics, const gp::Color& color, const gp::RectF& rect) {
    if (rect.Width <= 0 || rect.Height <= 0) {
        return;
    }
    float diameter = std::min(rect.Width, rect.Height);
    gp::GraphicsPath path;
    path.AddArc(rect.X, rect.Y, diameter, diameter, 90, 180);
    path.AddArc(rect.GetRight() - diameter, rect.Y, diameter, diameter, 270, 180);
    path.CloseFigure();
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
}

// The Windows icon font: Segoe Fluent Icons on Windows 11, Segoe MDL2 Assets on Windows 10.
const wchar_t* IconFontName() {
    static std::wstring name;
    static bool looked = false;
    if (!looked) {
        looked = true;
        for (const wchar_t* candidate : {L"Segoe Fluent Icons", L"Segoe MDL2 Assets"}) {
            gp::FontFamily family(candidate);
            if (family.IsAvailable()) {
                name = candidate;
                break;
            }
        }
    }
    return name.empty() ? nullptr : name.c_str();
}

const wchar_t* const kGlyphWarning = L"\uE7BA";

// Draws an icon-font glyph scaled to fit and centered in the target.
void DrawGlyph(gp::Graphics& graphics, const wchar_t* glyph, const gp::RectF& target, const gp::Color& color) {
    const wchar_t* fontName = IconFontName();
    gp::FontFamily family(fontName ? fontName : L"Segoe UI Symbol");
    gp::GraphicsPath path;
    path.AddString(fontName ? glyph : L"!", -1, &family, gp::FontStyleRegular, 100.0f, gp::PointF(0, 0),
                   gp::StringFormat::GenericTypographic());
    gp::RectF bounds;
    path.GetBounds(&bounds);
    if (bounds.Width <= 0 || bounds.Height <= 0) {
        return;
    }
    float scale = std::min(target.Width / bounds.Width, target.Height / bounds.Height);
    gp::Matrix transform;
    transform.Translate(target.X + (target.Width - bounds.Width * scale) / 2,
                        target.Y + (target.Height - bounds.Height * scale) / 2);
    transform.Scale(scale, scale);
    transform.Translate(-bounds.X, -bounds.Y);
    path.Transform(&transform);
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
}

enum class TextAlign { Left, Center, Right };

// One line of text, vertically centered in the rectangle; cut short with "…" when trim is set,
// otherwise never clipped.
void DrawTextLine(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, const gp::Color& color,
                  const gp::RectF& rect, TextAlign align = TextAlign::Left, bool trim = true) {
    gp::StringFormat format(trim ? gp::StringFormatFlagsNoWrap : gp::StringFormatFlagsNoWrap | gp::StringFormatFlagsNoClip);
    format.SetTrimming(trim ? gp::StringTrimmingEllipsisCharacter : gp::StringTrimmingNone);
    format.SetLineAlignment(gp::StringAlignmentCenter);
    format.SetAlignment(align == TextAlign::Left ? gp::StringAlignmentNear
                        : align == TextAlign::Center ? gp::StringAlignmentCenter
                                                     : gp::StringAlignmentFar);
    gp::SolidBrush brush(color);
    graphics.DrawString(text.c_str(), (INT)text.size(), &font, rect, &format, &brush);
}

// Text wrapped over as many lines as fit, cut short with "…".
void DrawTextBlock(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, const gp::Color& color,
                   const gp::RectF& rect) {
    gp::StringFormat format(gp::StringFormatFlagsLineLimit);
    format.SetTrimming(gp::StringTrimmingEllipsisWord);
    gp::SolidBrush brush(color);
    graphics.DrawString(text.c_str(), (INT)text.size(), &font, rect, &format, &brush);
}

std::unique_ptr<gp::Font> MakeFont(const wchar_t* family, float pixels) {
    return std::make_unique<gp::Font>(family, pixels, gp::FontStyleRegular, gp::UnitPixel);
}

// Popup colors for the "default app mode" (light or dark) chosen in Windows.
struct Palette {
    gp::Color background, foreground, secondary, accent, track, border;

    static Palette Current() {
        if (AppsUseLightTheme()) {
            return {Argb(255, 243, 243, 243), Argb(255, 27, 27, 27), Argb(255, 96, 96, 96),
                    Argb(255, 0, 95, 184), Argb(255, 196, 196, 196), Argb(255, 213, 213, 213)};
        }
        return {Argb(255, 32, 32, 32), Argb(255, 255, 255, 255), Argb(255, 197, 197, 197),
                Argb(255, 96, 205, 255), Argb(255, 80, 80, 80), Argb(255, 58, 58, 58)};
    }
};

// ---------------------------------------------------------------------------
// Window classes

// The window procedure of RegisterForwardingClass: forwards messages to the object of type T
// passed as the creation parameter, calling T::HandleMessage.
template <typename T>
LRESULT CALLBACK ForwardingWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCCREATE) {
        auto create = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
        ((T*)create->lpCreateParams)->hwnd_ = hwnd;
    }
    auto self = (T*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (self) {
        LRESULT result = 0;
        if (self->HandleMessage(message, wParam, lParam, result)) {
            return result;
        }
    }
    if (message == WM_NCDESTROY) {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        if (self) {
            self->hwnd_ = nullptr;
        }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// The mod's own module (its DLL), which its window classes belong to.
HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&ModuleInstance, &module);
    return module;
}

// The window classes registered so far, to unregister when the UI thread ends.
std::vector<std::wstring>& RegisteredClasses() {
    static std::vector<std::wstring> classes;
    return classes;
}

// Registers a window class whose windows forward their messages to an object of type T, passed
// as the creation parameter. Returns its name, or null if it couldn't be registered.
template <typename T>
const wchar_t* RegisterForwardingClass(const wchar_t* name) {
    std::vector<std::wstring>& classes = RegisteredClasses();
    if (std::find(classes.begin(), classes.end(), name) != classes.end()) {
        return name;
    }
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.hInstance = ModuleInstance();
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = name;
    windowClass.lpfnWndProc = ForwardingWindowProc<T>;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"Couldn't register window class %s: error %u", name, GetLastError());
        return nullptr;
    }
    classes.push_back(name);
    return name;
}

void UnregisterWindowClasses() {
    for (const std::wstring& name : RegisteredClasses()) {
        UnregisterClassW(name.c_str(), ModuleInstance());
    }
    RegisteredClasses().clear();
}

// A hidden window for timers and for results posted back from worker threads.
class MessageWindow {
public:
    std::function<void(UINT message, WPARAM wParam, LPARAM lParam)> onMessage;
    std::function<void(UINT_PTR timer)> onTimer;

    bool Create(const wchar_t* className) {
        if (const wchar_t* registered = RegisterForwardingClass<MessageWindow>(className)) {
            CreateWindowExW(0, registered, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, ModuleInstance(), this);
        }
        return hwnd_ != nullptr;
    }

    ~MessageWindow() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    HWND Handle() const { return hwnd_; }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        if (message == WM_TIMER) {
            if (onTimer) {
                onTimer(wParam);
            }
            return true;
        }
        if (message >= WM_APP && message < 0xC000) {
            if (onMessage) {
                onMessage(message, wParam, lParam);
            }
            return true;
        }
        return false;
    }

    HWND hwnd_ = nullptr;
};

// ---------------------------------------------------------------------------
// Popups

// Base for the popups: a borderless, always-on-top window that never takes focus (so it doesn't
// interrupt typing), with Windows 11 rounded corners. Painted double-buffered with GDI+.
class PopupWindow {
public:
    virtual ~PopupWindow() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool Visible() const { return hwnd_ && IsWindowVisible(hwnd_); }

    void Hide() {
        if (Visible()) {
            ShowWindow(hwnd_, SW_HIDE);
        }
    }

    int Px(float value) const { return (int)std::nearbyint(value * scale_); }  // Halves to even, like the apps' Math.Round.

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        switch (message) {
            case WM_MOUSEACTIVATE:
                result = MA_NOACTIVATE;
                return true;
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_PAINT: {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hwnd_, &paint);
                RECT client;
                GetClientRect(hwnd_, &client);
                int width = client.right;
                int height = client.bottom;
                if (width > 0 && height > 0) {
                    HDC memory = CreateCompatibleDC(dc);
                    HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);
                    HGDIOBJ previous = SelectObject(memory, bitmap);
                    {
                        gp::Graphics graphics(memory);
                        graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
                        graphics.SetTextRenderingHint(gp::TextRenderingHintClearTypeGridFit);
                        Palette colors = Palette::Current();
                        graphics.Clear(colors.background);
                        Paint(graphics, width, height, colors);
                        if (!roundedByWindows_) {
                            gp::Pen pen(colors.border, 1);
                            graphics.DrawRectangle(&pen, 0, 0, width - 1, height - 1);
                        }
                    }
                    BitBlt(dc, 0, 0, width, height, memory, 0, 0, SRCCOPY);
                    SelectObject(memory, previous);
                    DeleteObject(bitmap);
                    DeleteDC(memory);
                }
                EndPaint(hwnd_, &paint);
                return true;
            }
            case WM_LBUTTONDOWN:
                SetCapture(hwnd_);  // For the button's release.
                return true;
            case WM_LBUTTONUP:
                if (GetCapture() == hwnd_) {
                    ReleaseCapture();
                }
                OnMouseUp({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_TIMER:
                OnTimer(wParam);
                return true;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

protected:
    bool CreatePopup(const wchar_t* className) {
        if (const wchar_t* registered = RegisterForwardingClass<PopupWindow>(className)) {
            CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, registered, L"", WS_POPUP, 0, 0, 1, 1, nullptr,
                            nullptr, ModuleInstance(), this);
        }
        if (!hwnd_) {
            return false;
        }
        // Windows 11 rounded corners; fails harmlessly on Windows 10.
        DWORD preference = DWMWCP_ROUND;
        roundedByWindows_ = SUCCEEDED(DwmSetWindowAttribute(hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference)));
        return true;
    }

    // Sizes the popup in logical pixels for the monitor at the point, and puts it at (x, y).
    void Place(int x, int y, int logicalWidth, int logicalHeight) {
        SetWindowPos(hwnd_, HWND_TOPMOST, x, y, Px((float)logicalWidth), Px((float)logicalHeight), SWP_NOACTIVATE);
    }

    void SetScaleFor(POINT point) {
        float scale = DpiForPoint(point) / 96.0f;
        if (scale != scale_) {
            scale_ = scale;
            OnScaleChanged();
        }
    }

    void ShowNoActivate() {
        if (!IsWindowVisible(hwnd_)) {
            ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
        }
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    virtual void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) = 0;
    virtual void OnScaleChanged() {}
    virtual void OnMouseUp(POINT point) {}
    virtual void OnTimer(UINT_PTR timer) {}

    float scale_ = 1;
    bool roundedByWindows_ = false;
};

// The small popup in the bottom corner with a title and a line or two of detail, which hides
// itself after a moment. An icon callback can draw something other than a warning sign, and
// level (0-100) adds a level bar instead of the detail.
class MessagePopup : public PopupWindow {
public:
    struct Content {
        std::wstring title;
        std::wstring detail;
        std::function<void(gp::Graphics&, const gp::RectF&, const gp::Color&)> icon;
        int level = -1;
        bool levelMuted = false;
    };

    bool Create() { return CreatePopup(L"WidgetMessagePopup"); }

    void Show(const Content& content, int visibleMilliseconds = 4000) {
        content_ = content;
        POINT cursor;
        GetCursorPos(&cursor);
        SetScaleFor(cursor);
        // Bottom-right of the screen that was clicked on, just above the taskbar.
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &info);
        int width = Px(kWidth);
        int height = Px(kHeight);
        Place(info.rcWork.right - width - Px(12), info.rcWork.bottom - height - Px(12), kWidth, kHeight);
        ShowNoActivate();
        SetTimer(hwnd_, 1, visibleMilliseconds, nullptr);
    }

    void ShowMessage(const std::wstring& title, const std::wstring& detail) {
        Content content;
        content.title = title;
        content.detail = detail;
        Show(content);
    }

protected:
    static constexpr int kWidth = 360;
    static constexpr int kHeight = 84;

    void OnScaleChanged() override {
        titleFont_ = MakeFont(L"Segoe UI Semibold", 14.0f * scale_);
        detailFont_ = MakeFont(L"Segoe UI", 12.0f * scale_);
    }

    void OnTimer(UINT_PTR timer) override {
        KillTimer(hwnd_, timer);
        Hide();
    }

    void OnMouseUp(POINT point) override { Hide(); }

    void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) override {
        if (!titleFont_) {
            OnScaleChanged();
        }
        gp::RectF iconBounds((float)Px(18), (float)Px(26), (float)Px(32), (float)Px(32));
        if (content_.icon) {
            content_.icon(graphics, iconBounds, colors.foreground);
        } else {
            DrawGlyph(graphics, kGlyphWarning, gp::RectF(iconBounds.X, iconBounds.Y + Px(2), (float)Px(28), (float)Px(28)), colors.foreground);
        }
        float textLeft = (float)Px(66);
        float textWidth = width - textLeft - Px(18);
        DrawTextLine(graphics, content_.title, *titleFont_, colors.foreground, gp::RectF(textLeft, (float)Px(12), textWidth, (float)Px(24)));
        if (content_.level >= 0) {
            float labelWidth = (float)Px(48);
            DrawTextLine(graphics, content_.levelMuted ? L"Muted" : std::to_wstring(content_.level) + L"%", *detailFont_,
                         colors.secondary, gp::RectF(width - Px(18) - labelWidth, (float)Px(40), labelWidth, (float)Px(22)), TextAlign::Right);
            gp::RectF bar(textLeft, (float)Px(49), textWidth - labelWidth - Px(10), (float)Px(4));
            FillPill(graphics, colors.track, bar);
            if (!content_.levelMuted) {
                FillPill(graphics, colors.accent, gp::RectF(bar.X, bar.Y, bar.Width * std::min(100, content_.level) / 100.0f, bar.Height));
            }
        } else {
            DrawTextBlock(graphics, content_.detail, *detailFont_, colors.secondary,
                          gp::RectF(textLeft, (float)Px(38), textWidth, (float)Px(36)));
        }
    }

    Content content_;
    std::unique_ptr<gp::Font> titleFont_;
    std::unique_ptr<gp::Font> detailFont_;
};

// ---------------------------------------------------------------------------
// Dialogs

// An invisible, always-on-top window in the middle of the screen with the mouse, to open dialogs
// over: they then come up in front, where the mouse is, rather than behind other windows.
class DialogOwner {
public:
    DialogOwner() {
        // Like a WinForms dialog, keeps the thread's other windows (its popups) from taking clicks
        // while it's open. The widget, on the taskbar's thread, still takes them: what it asks for
        // is looked up again once the dialog closes.
        EnumThreadWindows(GetCurrentThreadId(), DisableWindow, (LPARAM)&disabled_);
        POINT cursor;
        GetCursorPos(&cursor);
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &info);
        int x = (info.rcWork.left + info.rcWork.right) / 2;
        int y = (info.rcWork.top + info.rcWork.bottom) / 2;
        hwnd_ = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, L"STATIC", L"", WS_POPUP, x, y, 1, 1,
                                nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (hwnd_) {
            SetLayeredWindowAttributes(hwnd_, 0, 0, LWA_ALPHA);
            ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
            SetForegroundWindow(hwnd_);
        }
    }

    ~DialogOwner() {
        for (HWND window : disabled_) {
            if (IsWindow(window)) {
                EnableWindow(window, TRUE);
            }
        }
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    HWND Handle() const { return hwnd_; }

private:
    static BOOL CALLBACK DisableWindow(HWND window, LPARAM data) {
        if (IsWindowEnabled(window)) {
            EnableWindow(window, FALSE);
            ((std::vector<HWND>*)data)->push_back(window);
        }
        return TRUE;
    }

    HWND hwnd_ = nullptr;
    std::vector<HWND> disabled_;
};

enum { kPromptLabelId = 100, kPromptEditId = 101 };

struct PromptState {
    const std::wstring* title;
    const std::wstring* prompt;
    const std::wstring* text;
    bool allowSpaces;
    bool allowEmpty;
    std::wstring result;
};

std::wstring PromptText(HWND dialog) {
    HWND edit = GetDlgItem(dialog, kPromptEditId);
    int length = GetWindowTextLengthW(edit);
    std::wstring text(length, L'\0');
    GetWindowTextW(edit, text.data(), length + 1);
    return Trim(text);
}

// Allows OK only for text that fits (not empty, no spaces, when that's asked for).
void ValidatePrompt(HWND dialog) {
    auto state = (PromptState*)GetWindowLongPtrW(dialog, DWLP_USER);
    std::wstring text = PromptText(dialog);
    bool ok = (state->allowEmpty || !text.empty()) &&
              (state->allowSpaces || std::none_of(text.begin(), text.end(), [](wchar_t c) { return iswspace(c) != 0; }));
    EnableWindow(GetDlgItem(dialog, IDOK), ok);
}

INT_PTR CALLBACK PromptDialogProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            auto state = (PromptState*)lParam;
            SetWindowLongPtrW(dialog, DWLP_USER, (LONG_PTR)state);
            SetWindowTextW(dialog, state->title->c_str());
            SetDlgItemTextW(dialog, kPromptLabelId, state->prompt->c_str());
            SetDlgItemTextW(dialog, kPromptEditId, state->text->c_str());
            ValidatePrompt(dialog);
            SetForegroundWindow(dialog);
            HWND edit = GetDlgItem(dialog, kPromptEditId);
            SetFocus(edit);
            SendMessageW(edit, EM_SETSEL, 0, -1);
            return FALSE;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == kPromptEditId && HIWORD(wParam) == EN_CHANGE) {
                ValidatePrompt(dialog);
            } else if (LOWORD(wParam) == IDOK) {
                ((PromptState*)GetWindowLongPtrW(dialog, DWLP_USER))->result = PromptText(dialog);
                EndDialog(dialog, IDOK);
            } else if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(dialog, IDCANCEL);
            }
            return TRUE;
    }
    return FALSE;
}

// Asks for a line of text in a small dialog. Returns nothing if cancelled.
std::optional<std::wstring> PromptForText(const std::wstring& title, const std::wstring& prompt,
                                          const std::wstring& acceptText, const std::wstring& initialText,
                                          bool allowSpaces, bool allowEmpty) {
    // An in-memory dialog template: a label, an edit box and two buttons, in dialog units.
    std::vector<WORD> buffer;
    auto addWord = [&](WORD word) { buffer.push_back(word); };
    auto addDword = [&](DWORD dword) {
        addWord(LOWORD(dword));
        addWord(HIWORD(dword));
    };
    auto addString = [&](const wchar_t* text) {
        for (; *text; text++) {
            addWord(*text);
        }
        addWord(0);
    };
    auto alignDword = [&]() {
        if (buffer.size() % 2) {
            addWord(0);
        }
    };
    auto addItem = [&](DWORD style, short x, short y, short cx, short cy, WORD id, WORD classAtom, const wchar_t* text) {
        alignDword();
        addDword(style | WS_CHILD | WS_VISIBLE);
        addDword(0);
        addWord(x);
        addWord(y);
        addWord(cx);
        addWord(cy);
        addWord(id);
        addWord(0xFFFF);
        addWord(classAtom);
        addString(text);
        addWord(0);
    };

    addDword(DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU);
    addDword(WS_EX_TOPMOST);
    addWord(4);
    addWord(0);
    addWord(0);
    addWord(280);
    addWord(80);
    addWord(0);  // No menu.
    addWord(0);  // Default class.
    addString(L"");
    addWord(9);
    addString(L"Segoe UI");
    addItem(SS_LEFT, 7, 7, 266, 22, kPromptLabelId, 0x0082, L"");
    addItem(ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP, 7, 32, 266, 14, kPromptEditId, 0x0081, L"");
    addItem(BS_DEFPUSHBUTTON | WS_TABSTOP, 161, 56, 54, 15, IDOK, 0x0080, acceptText.c_str());
    addItem(BS_PUSHBUTTON | WS_TABSTOP, 219, 56, 54, 15, IDCANCEL, 0x0080, L"Cancel");

    PromptState state{&title, &prompt, &initialText, allowSpaces, allowEmpty, {}};

    DialogOwner owner;
    INT_PTR result = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), (LPCDLGTEMPLATEW)buffer.data(), owner.Handle(),
                                             PromptDialogProc, (LPARAM)&state);
    if (result != IDOK) {
        return std::nullopt;
    }
    return state.result;
}

// ---------------------------------------------------------------------------
// Taskbar

// Explorer's taskbar, if it's this process's.
HWND OwnTaskbar() {
    for (HWND window = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr); window;
         window = FindWindowExW(nullptr, window, L"Shell_TrayWnd", nullptr)) {
        DWORD process = 0;
        GetWindowThreadProcessId(window, &process);
        if (process == GetCurrentProcessId()) {
            return window;
        }
    }
    return nullptr;
}

// Whether the process is an Explorer.
bool IsExplorerProcess(DWORD processId) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }
    WCHAR path[MAX_PATH];
    DWORD length = MAX_PATH;
    bool explorer = false;
    if (QueryFullProcessImageNameW(process, 0, path, &length)) {
        const wchar_t* name = wcsrchr(path, L'\\');
        explorer = _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }
    CloseHandle(process);
    return explorer;
}

// Whether this is the Explorer with the taskbar, or (while Explorer is starting) about to have it.
// Windhawk loads the mod into every Explorer process, folder windows' ("Launch folder windows in a
// separate process", started with "/factory,") too, and those never have it. Other apps'
// Shell_TrayWnd windows don't count: taskbar replacements such as RetroBar make one.
bool InTaskbarProcess() {
    std::wstring commandLine = GetCommandLineW();
    std::transform(commandLine.begin(), commandLine.end(), commandLine.begin(), towlower);
    if (commandLine.find(L"/factory,") != std::wstring::npos) {
        return false;
    }
    if (OwnTaskbar()) {
        return true;
    }
    for (HWND window = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr); window;
         window = FindWindowExW(nullptr, window, L"Shell_TrayWnd", nullptr)) {
        DWORD process = 0;
        GetWindowThreadProcessId(window, &process);
        if (IsExplorerProcess(process)) {
            return false;  // Another Explorer has it.
        }
    }
    return true;
}

UINT TaskbarCreatedMessage() {
    static const UINT message = RegisterWindowMessageW(L"TaskbarCreated");
    return message;
}

// A hidden top-level window of the app's thread: it owns the app's dialogs, and hears when a
// Windows setting or the devices change, which a window for messages only doesn't.
class OwnerWindow {
public:
    std::function<void()> onSettingChange;
    std::function<void(WPARAM event)> onDeviceChange;
    std::function<void()> onTaskbarCreated;  // Explorer made the taskbar (as it started, say).

    bool Create(const wchar_t* className) {
        if (const wchar_t* registered = RegisterForwardingClass<OwnerWindow>(className)) {
            CreateWindowExW(WS_EX_TOOLWINDOW, registered, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, ModuleInstance(), this);
        }
        return hwnd_ != nullptr;
    }

    ~OwnerWindow() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    HWND Handle() const { return hwnd_; }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        if (message == WM_SETTINGCHANGE && onSettingChange) {
            onSettingChange();
        } else if (message == WM_DEVICECHANGE && onDeviceChange) {
            onDeviceChange(wParam);
        } else if (message == TaskbarCreatedMessage() && onTaskbarCreated) {
            onTaskbarCreated();
        }
        return false;
    }

    HWND hwnd_ = nullptr;
};

// Forgets what the versions that dragged the widget along the taskbar kept: where it was, and its
// link with the other widgets.
void ForgetDraggedWidget() {
    for (const wchar_t* name : {L"TaskbarOffset", L"LinkGroupOrder", L"LinkGroupOffset", L"LinkGroupStamp", L"Link", L"LinkStamp"}) {
        Storage::Delete(name);
    }
}

// ---------------------------------------------------------------------------
// Menus: built on the app's thread, shown by the widget as a XAML menu on the taskbar's thread;
// the chosen item's action then runs back on the app's thread.

struct MenuEntry {
    enum class Kind { Item, Separator, Heading, Submenu };
    Kind kind = Kind::Item;
    std::wstring text;  // A tab, then the item's shortcut, if it has one.
    bool checked = false;
    bool enabled = true;
    int command = 0;                 // An item's: what the widget sends back when it's chosen.
    std::vector<MenuEntry> entries;  // A submenu's.
};

class Menu {
public:
    Menu() : actions_(std::make_shared<std::vector<std::function<void()>>>()) {}

    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    void Add(const std::wstring& text, std::function<void()> action, bool checked = false, bool enabled = true) {
        actions_->push_back(std::move(action));
        MenuEntry entry;
        entry.text = text;
        entry.checked = checked;
        entry.enabled = enabled;
        entry.command = (int)actions_->size();
        entries_.push_back(entry);
    }

    void AddSeparator() {
        MenuEntry entry;
        entry.kind = MenuEntry::Kind::Separator;
        entries_.push_back(entry);
    }

    // A line of text that can't be chosen, above the items it's about.
    void AddHeading(const std::wstring& text) {
        MenuEntry entry;
        entry.kind = MenuEntry::Kind::Heading;
        entry.text = text;
        entries_.push_back(entry);
    }

    // A submenu; items added to it share this menu's actions.
    Menu& AddSubmenu(const std::wstring& text, bool enabled = true) {
        submenus_.push_back(std::unique_ptr<Menu>(new Menu(actions_)));
        MenuEntry entry;
        entry.kind = MenuEntry::Kind::Submenu;
        entry.text = text;
        entry.enabled = enabled;
        entries_.push_back(entry);
        submenuEntries_.push_back(entries_.size() - 1);
        return *submenus_.back();
    }

    // What the widget shows.
    std::vector<MenuEntry> Entries() const {
        std::vector<MenuEntry> entries = entries_;
        for (size_t i = 0; i < submenus_.size(); i++) {
            entries[submenuEntries_[i]].entries = submenus_[i]->Entries();
        }
        return entries;
    }

    // Runs the action of the item the widget sent back.
    void Run(int command) {
        if (command > 0 && command <= (int)actions_->size() && (*actions_)[command - 1]) {
            auto actions = actions_;  // The action may show a new menu, ending this one.
            (*actions)[command - 1]();
        }
    }

private:
    explicit Menu(std::shared_ptr<std::vector<std::function<void()>>> actions) : actions_(std::move(actions)) {}

    std::shared_ptr<std::vector<std::function<void()>>> actions_;
    std::vector<MenuEntry> entries_;
    std::vector<std::unique_ptr<Menu>> submenus_;
    std::vector<size_t> submenuEntries_;  // Where each submenu is in entries_.
};

// ---------------------------------------------------------------------------
// The widget inside the taskbar (the same code is in the Taskbar Audio Flipper, Taskbar RDP
// Launcher and Taskbar Snipper mods). Windows 11 draws its taskbar with XAML, so the widget is
// XAML too: elements of its own, put into the taskbar's layout, which makes room for them. Getting
// to the taskbar's XAML takes a few of taskbar.dll's functions, which Windhawk finds by name in
// Microsoft's symbols, as other taskbar mods do.
//
// The elements belong to the taskbar's thread and are only touched there. The app works on its
// own thread: it posts changes to the elements (Bridge::Post), and they post clicks back to it
// (PostToApp), so neither ever waits for the other.

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

namespace TaskbarXaml {

// ---- taskbar.dll

void* g_taskBandSiteVftable = nullptr;  // CTaskBand's vftable for its ITaskListWndSite part.
using GetTaskbarHost_t = void*(WINAPI*)(void* self, void* result);
GetTaskbarHost_t g_getTaskbarHost = nullptr;
using FrameHeight_t = int(WINAPI*)(void* self);
FrameHeight_t g_frameHeight = nullptr;
using Decref_t = void(WINAPI*)(void* self);
Decref_t g_decref = nullptr;
using StartTaskbar_t = void(WINAPI*)(void* self);
StartTaskbar_t g_startTaskbarOriginal = nullptr;

// Called on the taskbar's thread when Explorer has made the taskbar (as it starts, or again).
void (*g_onTaskbarStarted)(HWND taskbar) = nullptr;

void WINAPI StartTaskbarHook(void* self) {
    g_startTaskbarOriginal(self);
    HWND taskbar = OwnTaskbar();
    if (g_onTaskbarStarted && taskbar) {
        g_onTaskbarStarted(taskbar);
    }
}

// Finds taskbar.dll's functions; false where there aren't any (Windows 10's taskbar isn't XAML).
bool HookTaskbarDll() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"No taskbar.dll: error %u", GetLastError());
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_taskBandSiteVftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getTaskbarHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"}, &g_startTaskbarOriginal, StartTaskbarHook},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

UINT RunOnWindowThreadMessage() {
    static const UINT message = RegisterWindowMessageW(L"TaskbarWidgets.RunOnWindowThread." WH_MOD_ID);
    return message;
}

LRESULT CALLBACK RunOnWindowThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto call = (const CWPSTRUCT*)lParam;
        if (call->message == RunOnWindowThreadMessage()) {
            (*(const std::function<void()>*)call->lParam)();
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs the function on the window's thread, and waits for it.
bool RunOnWindowThread(HWND window, const std::function<void()>& run) {
    DWORD thread = GetWindowThreadProcessId(window, nullptr);
    if (!thread) {
        return false;
    }
    if (thread == GetCurrentThreadId()) {
        run();
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnWindowThreadHook, nullptr, thread);
    if (!hook) {
        return false;
    }
    SendMessageW(window, RunOnWindowThreadMessage(), 0, (LPARAM)&run);
    UnhookWindowsHookEx(hook);
    return true;
}

bool IsReadable(const void* address, size_t size) {
    MEMORY_BASIC_INFORMATION memory;
    if (!VirtualQuery(address, &memory, sizeof(memory)) || memory.State != MEM_COMMIT || (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    return (const BYTE*)address + size <= (const BYTE*)memory.BaseAddress + memory.RegionSize;
}

// Where TaskbarHost keeps the taskbar's XAML element, from the start of TaskbarHost::FrameHeight,
// which reads it; 0 if its code isn't as expected.
size_t ElementOffset() {
#if defined(__x86_64__)
    // sub rsp, imm8; add rcx, imm8
    const BYTE* code = (const BYTE*)g_frameHeight;
    if (IsReadable(code, 8) && code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC && code[4] == 0x48 && code[5] == 0x83 &&
        code[6] == 0xC1 && code[7] <= 0x7F) {
        return code[7];
    }
#elif defined(__aarch64__)
    // pacibsp; stp fp, lr, [sp, #...]!; mov fp, sp; ldr x0, [x0, #imm]!
    const DWORD* code = (const DWORD*)g_frameHeight;
    if (IsReadable(code, sizeof(DWORD) * 4) && code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD && code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        return (code[3] >> 12) & 0xFF;
    }
#endif
    return 0;
}

// The taskbar's XAML (on its thread): the task band behind the taskbar window has the
// TaskbarHost, which has the taskbar's XAML element.
wux::XamlRoot GetXamlRoot(HWND taskbar) {
    HWND taskSwitcher = (HWND)GetPropW(taskbar, L"TaskbandHWND");
    void* taskBand = taskSwitcher ? (void*)GetWindowLongPtrW(taskSwitcher, 0) : nullptr;
    if (!taskBand || !g_taskBandSiteVftable || !g_getTaskbarHost || !g_frameHeight || !g_decref) {
        return nullptr;
    }
    // Its ITaskListWndSite part: the first slot with that vftable.
    void** site = (void**)taskBand;
    for (int i = 0;; i++, site++) {
        if (i > 20 || !IsReadable(site, sizeof(void*))) {
            return nullptr;
        }
        if (*site == g_taskBandSiteVftable) {
            break;
        }
    }
    void* host[2] = {};  // A std::shared_ptr<TaskbarHost>: the object, and its reference count.
    g_getTaskbarHost(site, host);
    wux::XamlRoot root{nullptr};
    size_t offset = ElementOffset();
    if (host[0] && offset && IsReadable((BYTE*)host[0] + offset, sizeof(IUnknown*))) {
        if (auto element = *(IUnknown**)((BYTE*)host[0] + offset)) {
            wux::FrameworkElement frameworkElement{nullptr};
            if (SUCCEEDED(element->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frameworkElement))) &&
                frameworkElement) {
                root = frameworkElement.XamlRoot();
            }
        }
    }
    if (host[1]) {
        g_decref(host[1]);
    }
    return root;
}

wux::FrameworkElement FindByName(wux::DependencyObject const& parent, std::wstring_view name, int depth = 64) {
    int count = parent && depth > 0 ? wuxm::VisualTreeHelper::GetChildrenCount(parent) : 0;
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<wux::FrameworkElement>(); element && std::wstring_view(element.Name()) == name) {
            return element;
        }
        if (auto found = FindByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// The child of the panel that is, or holds, the element.
wux::FrameworkElement ChildHolding(wuxc::Panel const& panel, wux::FrameworkElement element) {
    while (element) {
        auto parent = wuxm::VisualTreeHelper::GetParent(element);
        if (parent == panel) {
            return element;
        }
        element = parent.try_as<wux::FrameworkElement>();
    }
    return nullptr;
}

// ---- Where the widget goes: a slot in the taskbar's layout. Each slot is one horizontal panel,
// shared by the widgets put there, in order of their places (StoredPlace).

enum class Slot {
    Tray,         // Left of the notification area.
    SystemIcons,  // Left of the Wi-Fi, volume and battery icons.
    Clock,        // Left of the clock.
    Left,         // The left end of the taskbar, over it (free when its icons are centered and the Widgets button is off).
};

Slot ReadSlot() {
    auto value = WindhawkUtils::StringSetting::make(L"position");
    return wcscmp(value, L"systemIcons") == 0 ? Slot::SystemIcons
           : wcscmp(value, L"clock") == 0     ? Slot::Clock
           : wcscmp(value, L"left") == 0      ? Slot::Left
                                              : Slot::Tray;
}

std::wstring PanelName(Slot slot) {
    return L"TaskbarWidgets.Panel." + std::to_wstring((int)slot);
}

wuxc::StackPanel NewPanel(Slot slot) {
    wuxc::StackPanel panel;
    panel.Name(PanelName(slot));
    panel.Orientation(wuxc::Orientation::Horizontal);
    panel.VerticalAlignment(wux::VerticalAlignment::Stretch);
    return panel;
}

// The slot's panel, made if it isn't there yet: among the notification area's elements, which
// make room for it. They're in a row (a StackPanel) in recent builds of Windows 11, and in the
// columns of a Grid in earlier ones. Null while the taskbar's layout isn't ready.
wuxc::StackPanel PanelFor(wux::FrameworkElement const& root, Slot slot) {
    std::wstring name = PanelName(slot);
    auto tray = FindByName(root, L"SystemTrayFrameGrid").try_as<wuxc::Panel>();
    if (slot == Slot::Left) {
        // The taskbar's frame is one of the root's children.
        wux::FrameworkElement frame{nullptr};
        int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < count && !frame; i++) {
            auto child = wuxm::VisualTreeHelper::GetChild(root, i).try_as<wux::FrameworkElement>();
            if (child && winrt::get_class_name(child) == L"Taskbar.TaskbarFrame") {
                frame = child;
            }
        }
        auto rootGrid = frame ? FindByName(frame, L"RootGrid").try_as<wuxc::Grid>() : nullptr;
        if (!tray || !rootGrid) {
            return nullptr;
        }
        if (auto panel = FindByName(rootGrid, name, 1).try_as<wuxc::StackPanel>()) {
            return panel;
        }
        wuxc::StackPanel panel = NewPanel(slot);
        panel.HorizontalAlignment(wux::HorizontalAlignment::Left);
        wuxc::Grid::SetColumnSpan(panel, std::max(1, (int)rootGrid.ColumnDefinitions().Size()));
        wuxc::Grid::SetRowSpan(panel, std::max(1, (int)rootGrid.RowDefinitions().Size()));
        wuxc::Canvas::SetZIndex(panel, 1000);
        rootGrid.Children().Append(panel);
        return panel;
    }
    if (!tray) {
        return nullptr;
    }
    if (auto panel = FindByName(tray, name, 1).try_as<wuxc::StackPanel>()) {
        return panel;
    }
    // What it goes before: the notification area's first element, or the one holding the
    // system icons or the clock.
    wux::FrameworkElement anchor{nullptr};
    if (slot != Slot::Tray) {
        anchor = ChildHolding(tray, FindByName(tray, slot == Slot::Clock ? L"NotificationCenterButton" : L"ControlCenterButton"));
        if (!anchor) {
            return nullptr;
        }
    }
    wuxc::StackPanel panel = NewPanel(slot);
    auto grid = tray.try_as<wuxc::Grid>();
    if (!grid) {
        uint32_t index = 0;
        if (anchor) {
            tray.Children().IndexOf(anchor, index);
        }
        tray.Children().InsertAt(index, panel);
        return panel;
    }
    int column = anchor ? wuxc::Grid::GetColumn(anchor) : 0;
    wuxc::ColumnDefinition definition;
    definition.Width(wux::GridLengthHelper::Auto());
    grid.ColumnDefinitions().InsertAt(column, definition);
    for (auto const& child : grid.Children()) {
        if (auto element = child.try_as<wux::FrameworkElement>(); element && wuxc::Grid::GetColumn(element) >= column) {
            wuxc::Grid::SetColumn(element, wuxc::Grid::GetColumn(element) + 1);
        }
    }
    wuxc::Grid::SetColumn(panel, column);
    wuxc::Grid::SetRowSpan(panel, std::max(1, (int)grid.RowDefinitions().Size()));
    grid.Children().Append(panel);
    return panel;
}

// Takes the panel out (and its column, in a grid) once the last widget in it has gone.
void RemovePanelIfEmpty(wuxc::StackPanel const& panel) {
    if (panel.Children().Size() > 0) {
        return;
    }
    auto parent = wuxm::VisualTreeHelper::GetParent(panel).try_as<wuxc::Panel>();
    uint32_t index = 0;
    if (!parent || !parent.Children().IndexOf(panel, index)) {
        return;
    }
    parent.Children().RemoveAt(index);
    auto grid = parent.try_as<wuxc::Grid>();
    if (!grid || grid.Name() != L"SystemTrayFrameGrid") {
        return;  // The taskbar's own grid (Slot::Left) has no column of the panel's.
    }
    int column = wuxc::Grid::GetColumn(panel);
    if (column >= (int)grid.ColumnDefinitions().Size()) {
        return;
    }
    grid.ColumnDefinitions().RemoveAt(column);
    for (auto const& child : grid.Children()) {
        if (auto element = child.try_as<wux::FrameworkElement>(); element && wuxc::Grid::GetColumn(element) > column) {
            wuxc::Grid::SetColumn(element, wuxc::Grid::GetColumn(element) - 1);
        }
    }
}

// ---- Between the app's thread and the taskbar's

// The app's window for the widget's messages; null while the app isn't running.
std::atomic<HWND> g_appWindow{nullptr};

// Messages from the widget to the app's window.
constexpr UINT kAttachedMessage = WM_APP + 0x300;     // The widget is in the taskbar (again): send it everything.
constexpr UINT kClickMessage = WM_APP + 0x301;        // wParam: the part. lParam: the MouseButton.
constexpr UINT kWheelMessage = WM_APP + 0x302;        // wParam: the part. lParam: notches (up is positive).
constexpr UINT kMenuCommandMessage = WM_APP + 0x304;  // wParam: the command chosen in the menu.

enum class MouseButton { Left, Middle, Right };

void PostToApp(UINT message, WPARAM wParam, LPARAM lParam) {
    if (HWND window = g_appWindow) {
        PostMessageW(window, message, wParam, lParam);
    }
}

// Runs changes to the widget on the taskbar's thread, posted from the app's (through the XAML
// dispatcher, without waiting for them). None run once the mod is turning off.
class Bridge {
public:
    void SetDispatcher(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher, HWND taskbar) {
        AcquireSRWLockExclusive(&lock_);
        dispatcher_ = dispatcher;
        taskbar_ = taskbar;
        ReleaseSRWLockExclusive(&lock_);
    }

    // Changes posted before the widget is in the taskbar are dropped: the app sends everything
    // once it is (kAttachedMessage).
    void Post(std::function<void()> change) {
        AcquireSRWLockShared(&lock_);
        auto dispatcher = dispatcher_;
        ReleaseSRWLockShared(&lock_);
        if (!dispatcher || stopping_) {
            return;
        }
        try {
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [this, change = std::move(change)]() {
                if (!stopping_) {
                    try {
                        change();
                    } catch (...) {
                        Wh_Log(L"Couldn't change the widget");
                    }
                }
            });
        } catch (...) {
        }
    }

    // As the mod turns off: no more changes, and waits for those posted already to have run (as
    // nothing), since they're the mod's code, which Windhawk unloads next. Not on the taskbar's
    // thread.
    void Drain() {
        stopping_ = true;
        AcquireSRWLockShared(&lock_);
        auto dispatcher = dispatcher_;
        HWND taskbar = taskbar_;
        ReleaseSRWLockShared(&lock_);
        if (!dispatcher || !IsWindow(taskbar)) {
            return;
        }
        HANDLE ran = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        try {
            // At a lower priority than the changes, so it runs after all of them.
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [ran]() { SetEvent(ran); });
            WaitForSingleObject(ran, INFINITE);
        } catch (...) {
        }
        CloseHandle(ran);
    }

private:
    SRWLOCK lock_ = SRWLOCK_INIT;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher_{nullptr};
    HWND taskbar_ = nullptr;
    std::atomic<bool> stopping_{false};
};

// The widget's state on the taskbar's thread is in globals that are never destroyed by
// themselves: Wh_ModUninit lets go of them on the taskbar's thread, as XAML needs. When Explorer
// exits without turning the mod off, the taskbar's thread has gone already, and destructors run
// then would let go of XAML on another thread, after XAML itself may have gone.
[[clang::no_destroy]] std::optional<Bridge> g_bridge{std::in_place};

// ---- The widget's elements (on the taskbar's thread)

// What a mod's widget shows: it builds its elements, and lets go of them once they're out of the
// taskbar.
class View {
public:
    virtual ~View() = default;
    virtual wux::FrameworkElement Build() = 0;
    virtual void Forget() = 0;
};

// The widget's place among the widgets in its slot, from the left: a number kept in the mod's
// storage, and on the widget's element (as its Tag) for the others to see. Moving a widget gives it
// a number between its new neighbors', so theirs stay as they are.
constexpr int kPlaceUnit = 1 << 16;

int StoredPlace() {
    return Storage::GetInt(L"WidgetPlace", kWidgetOrder * kPlaceUnit);
}

int PlaceOf(wux::FrameworkElement const& element) {
    return winrt::unbox_value_or<int>(element.Tag(), 0);
}

[[clang::no_destroy]] wuxc::MenuFlyout g_menu{nullptr};  // The widget's menu while it's open.
winrt::event_token g_menuClosed{};
// Its items' Click handlers, the mod's code, which come off as it closes (or the mod turns off).
[[clang::no_destroy]] std::optional<std::vector<wuxc::MenuFlyoutItem::Click_revoker>> g_menuClicks{std::in_place};
// Where the widget was last right-clicked, to open its menu there.
[[clang::no_destroy]] wux::FrameworkElement g_menuTarget{nullptr};
winrt::Windows::Foundation::Point g_menuPoint{};

// Lets go of the menu, hiding it first if asked, with the mod's handlers taken off it: none of them
// runs once the mod's code has gone. Tells the app nothing (it may be gone).
void ForgetMenu(bool hide) {
    if (g_menu) {
        try {
            g_menu.Closed(g_menuClosed);
            if (hide) {
                g_menu.Hide();
            }
        } catch (...) {
        }
        g_menu = nullptr;
    }
    g_menuClicks->clear();
}

class WidgetHost {
public:
    // Puts the view's elements into the slot (once the taskbar's layout is ready, trying again
    // for a few seconds while it isn't), and tells the app.
    void Attach(HWND taskbar, Slot slot, View* view) {
        Detach();
        taskbar_ = taskbar;
        slot_ = slot;
        view_ = view;
        attempts_ = 0;
        TryAttach();
    }

    // Takes the elements out (and the slot's panel, if they were the last in it).
    void Detach() {
        StopRetrying();
        ForgetMenu(true);
        TakeOut();
        Forget();
    }

    // Lets go of the elements without taking them out: the taskbar they were in has gone, and
    // Explorer has made it again.
    void Forget() {
        StopRetrying();
        ForgetMenu(false);
        g_menuTarget = nullptr;
        element_ = nullptr;
        if (view_) {
            view_->Forget();
        }
    }

    wux::FrameworkElement Element() const { return element_; }

    // Whether there's another widget on the left (direction -1) or right (1) in the slot.
    bool CanMove(int direction) {
        uint32_t index = 0;
        auto panel = SlotPanel(index);
        return panel && (int)index + direction >= 0 && (int)index + direction < (int)panel.Children().Size();
    }

    // Moves the widget past its neighbor on the left (direction -1) or right (1), to stay there.
    void Move(int direction) {
        uint32_t index = 0;
        auto panel = SlotPanel(index);
        int count = panel ? (int)panel.Children().Size() : 0;
        int target = (int)index + direction;
        if (target < 0 || target >= count) {
            return;
        }
        auto placeAt = [&](int i) { return PlaceOf(panel.Children().GetAt(i).as<wux::FrameworkElement>()); };
        // Halfway between the neighbor and the one beyond it (or a step past the neighbor, at the end).
        int neighbor = placeAt(target);
        int beyond = target + direction >= 0 && target + direction < count ? placeAt(target + direction) : neighbor + direction * 2 * kPlaceUnit;
        int place = neighbor + (beyond - neighbor) / 2;
        Storage::SetInt(L"WidgetPlace", place);
        element_.Tag(winrt::box_value(place));
        panel.Children().RemoveAt(index);
        panel.Children().InsertAt(target, element_);
    }

private:
    void TakeOut() {
        try {
            if (element_) {
                if (auto panel = wuxm::VisualTreeHelper::GetParent(element_).try_as<wuxc::StackPanel>()) {
                    uint32_t index = 0;
                    if (panel.Children().IndexOf(element_, index)) {
                        panel.Children().RemoveAt(index);
                    }
                    RemovePanelIfEmpty(panel);
                }
            }
        } catch (...) {
            Wh_Log(L"Couldn't take the widget out of the taskbar");
        }
    }

    wuxc::StackPanel SlotPanel(uint32_t& index) {
        auto panel = element_ ? wuxm::VisualTreeHelper::GetParent(element_).try_as<wuxc::StackPanel>() : nullptr;
        return panel && panel.Children().IndexOf(element_, index) ? panel : nullptr;
    }

    void TryAttach() {
        try {
            auto root = GetXamlRoot(taskbar_);
            auto content = root ? root.Content().try_as<wux::FrameworkElement>() : nullptr;
            auto panel = content ? PanelFor(content, slot_) : nullptr;
            if (!panel) {
                RetrySoon();
                return;
            }
            element_ = view_->Build();
            int place = StoredPlace();
            element_.Tag(winrt::box_value(place));
            // In order among the widgets in the slot.
            uint32_t index = 0;
            for (; index < panel.Children().Size(); index++) {
                auto other = panel.Children().GetAt(index).try_as<wux::FrameworkElement>();
                if (other && PlaceOf(other) > place) {
                    break;
                }
            }
            panel.Children().InsertAt(index, element_);
            g_bridge->SetDispatcher(element_.Dispatcher(), taskbar_);
            PostToApp(kAttachedMessage, 0, 0);
        } catch (...) {
            // The taskbar may still be setting up, as Explorer starts: try again.
            Wh_Log(L"Couldn't put the widget in the taskbar yet");
            TakeOut();
            element_ = nullptr;
            if (view_) {
                view_->Forget();
            }
            RetrySoon();
        }
    }

    // Tries again a moment later, for up to a minute: as Explorer starts, the taskbar's XAML comes
    // some time after its window. With a timer of the thread's, as XAML's own need XAML there.
    void RetrySoon() {
        if (++attempts_ > 240) {
            Wh_Log(L"Couldn't find the taskbar's layout");
            return;
        }
        if (!retryTimer_) {
            retrying_ = this;
            retryTimer_ = SetTimer(nullptr, 0, 250, RetryTimerProc);
        }
    }

    static void CALLBACK RetryTimerProc(HWND, UINT, UINT_PTR timer, DWORD) {
        if (retrying_ && timer == retrying_->retryTimer_) {
            retrying_->StopRetrying();
            retrying_->TryAttach();
        } else {
            KillTimer(nullptr, timer);
        }
    }

    void StopRetrying() {
        if (retryTimer_) {
            KillTimer(nullptr, retryTimer_);
            retryTimer_ = 0;
        }
    }

    HWND taskbar_ = nullptr;
    Slot slot_ = Slot::Tray;
    View* view_ = nullptr;
    wux::FrameworkElement element_{nullptr};
    UINT_PTR retryTimer_ = 0;
    static inline WidgetHost* retrying_ = nullptr;
    int attempts_ = 0;
};

[[clang::no_destroy]] std::optional<WidgetHost> g_host{std::in_place};  // Not destroyed: see g_bridge.

// The colors of the widget's highlights, as the taskbar's buttons have them. Its text and icons
// follow the taskbar's light or dark theme by themselves.
struct Highlights {
    winrt::Windows::UI::Color hover;
    winrt::Windows::UI::Color pressed;
};

Highlights HighlightsFor(wux::FrameworkElement const& element) {
    if (element.ActualTheme() == wux::ElementTheme::Light) {
        return {{0x12, 0x1C, 0x1C, 0x1C}, {0x0B, 0x1C, 0x1C, 0x1C}};
    }
    return {{0x18, 0xFF, 0xFF, 0xFF}, {0x10, 0xFF, 0xFF, 0xFF}};
}

// A part of the widget to click, scroll or hover, highlighted like the taskbar's buttons. What's
// done to it goes to the app, as messages with the part's number.
class Part {
public:
    // For the widget's own flyouts, on the taskbar's thread.
    std::function<void(bool entered)> onHover;
    std::function<void()> onPress;
    std::function<void()> onClick;  // Left-clicked, after the app is told.

    // plainOnClick: drops the highlight on a click, as the click shows something that copies the
    // screen.
    wuxc::Border Make(int part, wux::UIElement const& content, const wchar_t* name, bool plainOnClick = false) {
        Forget();
        part_ = part;
        plainOnClick_ = plainOnClick;
        hovered_ = pressed_ = false;
        wheelRemainder_ = 0;
        border_ = wuxc::Border();
        border_.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4));
        border_.Margin(wux::ThicknessHelper::FromLengths(2, 4, 2, 4));
        border_.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{}));  // Clear, but it takes the mouse.
        border_.Child(content);
        wux::Automation::AutomationProperties::SetName(border_, name);
        enteredRevoker_ = border_.PointerEntered(winrt::auto_revoke, [this](auto const&, auto const&) {
            hovered_ = true;
            Paint();
            if (onHover) {
                onHover(true);
            }
        });
        exitedRevoker_ = border_.PointerExited(winrt::auto_revoke, [this](auto const&, auto const&) {
            hovered_ = pressed_ = false;
            Paint();
            if (onHover) {
                onHover(false);
            }
        });
        captureLostRevoker_ = border_.PointerCaptureLost(winrt::auto_revoke, [this](auto const&, auto const&) {
            pressed_ = false;
            Paint();
        });
        pressedRevoker_ = border_.PointerPressed(winrt::auto_revoke, [this](auto const&, wux::Input::PointerRoutedEventArgs const& args) {
            pressed_ = true;
            Paint();
            border_.CapturePointer(args.Pointer());
            args.Handled(true);
            if (onPress) {
                onPress();
            }
        });
        releasedRevoker_ = border_.PointerReleased(winrt::auto_revoke, [this](auto const&, wux::Input::PointerRoutedEventArgs const& args) { OnReleased(args); });
        // So the taskbar doesn't open its own menu too.
        rightTappedRevoker_ = border_.RightTapped(winrt::auto_revoke, [](auto const&, wux::Input::RightTappedRoutedEventArgs const& args) { args.Handled(true); });
        wheelRevoker_ = border_.PointerWheelChanged(winrt::auto_revoke, [this](auto const&, wux::Input::PointerRoutedEventArgs const& args) {
            // Precision touchpads scroll by fractions of a notch; they're added up.
            wheelRemainder_ += args.GetCurrentPoint(border_).Properties().MouseWheelDelta();
            int notches = wheelRemainder_ / WHEEL_DELTA;
            wheelRemainder_ -= notches * WHEEL_DELTA;
            if (notches != 0) {
                PostToApp(kWheelMessage, part_, notches);
            }
            args.Handled(true);
        });
        themeRevoker_ = border_.ActualThemeChanged(winrt::auto_revoke, [this](auto const&, auto const&) { Paint(); });
        return border_;
    }

    wuxc::Border Border() const { return border_; }

    // Lets go of its element, taking its handlers off it first: the element may outlive this.
    void Forget() {
        enteredRevoker_.revoke();
        exitedRevoker_.revoke();
        captureLostRevoker_.revoke();
        pressedRevoker_.revoke();
        releasedRevoker_.revoke();
        rightTappedRevoker_.revoke();
        wheelRevoker_.revoke();
        themeRevoker_.revoke();
        border_ = nullptr;
    }

    ~Part() { Forget(); }

private:
    void OnReleased(wux::Input::PointerRoutedEventArgs const& args) {
        auto point = args.GetCurrentPoint(border_);
        bool wasPressed = pressed_;
        pressed_ = false;
        border_.ReleasePointerCapture(args.Pointer());
        args.Handled(true);
        auto position = point.Position();
        // Pressing and sliding off it before letting go doesn't click it.
        bool click = wasPressed && position.X >= 0 && position.Y >= 0 && position.X < border_.ActualWidth() &&
                     position.Y < border_.ActualHeight();
        using winrt::Windows::UI::Input::PointerUpdateKind;
        PointerUpdateKind kind = point.Properties().PointerUpdateKind();
        if (click && kind == PointerUpdateKind::LeftButtonReleased && plainOnClick_) {
            hovered_ = false;
        }
        Paint();
        if (!click) {
            return;
        }
        switch (kind) {
            case PointerUpdateKind::LeftButtonReleased:
                PostToApp(kClickMessage, part_, (LPARAM)MouseButton::Left);
                if (onClick) {
                    onClick();
                }
                break;
            case PointerUpdateKind::MiddleButtonReleased:
                PostToApp(kClickMessage, part_, (LPARAM)MouseButton::Middle);
                break;
            case PointerUpdateKind::RightButtonReleased:
                g_menuTarget = border_;
                g_menuPoint = position;
                PostToApp(kClickMessage, part_, (LPARAM)MouseButton::Right);
                break;
            default:
                break;
        }
    }

    void Paint() {
        if (!border_) {
            return;
        }
        Highlights highlights = HighlightsFor(border_);
        border_.Background().as<wuxm::SolidColorBrush>().Color(pressed_   ? highlights.pressed
                                                                : hovered_ ? highlights.hover
                                                                           : winrt::Windows::UI::Color{});
    }

    int part_ = 0;
    bool plainOnClick_ = false;
    wuxc::Border border_{nullptr};
    wux::UIElement::PointerEntered_revoker enteredRevoker_;
    wux::UIElement::PointerExited_revoker exitedRevoker_;
    wux::UIElement::PointerCaptureLost_revoker captureLostRevoker_;
    wux::UIElement::PointerPressed_revoker pressedRevoker_;
    wux::UIElement::PointerReleased_revoker releasedRevoker_;
    wux::UIElement::RightTapped_revoker rightTappedRevoker_;
    wux::UIElement::PointerWheelChanged_revoker wheelRevoker_;
    wux::FrameworkElement::ActualThemeChanged_revoker themeRevoker_;
    bool hovered_ = false;
    bool pressed_ = false;
    int wheelRemainder_ = 0;
};

// Closes a flyout opened by hovering a part of the widget once the mouse is off both the flyout and
// the part for a moment (it may be crossing the gap between them), but not while a mouse button is
// held (dragging a slider, say). The flyout's own dismissal can't see the mouse leave for the
// desktop, only for the rest of the taskbar.
class HoverCloser {
public:
    // Watches the flyout, just opened from its part, which the mouse is on; close closes it.
    void Start(wuxc::Primitives::FlyoutBase const& flyout, wux::FrameworkElement const& content, std::function<void()> close) {
        Stop();
        close_ = std::move(close);
        partHovered_ = true;
        flyoutHovered_ = false;
        awayTicks_ = 0;
        openedRevoker_ = flyout.Opened(winrt::auto_revoke, [this, content](auto const&, auto const&) { WatchPresenter(content); });
        if (!timer_) {
            timer_ = wux::DispatcherTimer();
            timer_.Interval(std::chrono::milliseconds(100));
            tickRevoker_ = timer_.Tick(winrt::auto_revoke, [this](auto const&, auto const&) { Tick(); });
        }
        timer_.Start();
    }

    // The mouse came onto the flyout's part, or left it.
    void PartHovered(bool hovered) { partHovered_ = hovered; }

    void Stop() {
        if (timer_) {
            timer_.Stop();
        }
        openedRevoker_.revoke();
        enteredRevoker_.revoke();
        exitedRevoker_.revoke();
        close_ = nullptr;
    }

    // Lets go of its timer too, as the widget goes.
    void Forget() {
        Stop();
        tickRevoker_.revoke();
        timer_ = nullptr;
    }

private:
    // Hears the mouse on the flyout's presenter, around the content with its padding.
    void WatchPresenter(wux::FrameworkElement const& content) {
        wux::DependencyObject element = content;
        while (element && !element.try_as<wuxc::FlyoutPresenter>()) {
            element = wuxm::VisualTreeHelper::GetParent(element);
        }
        wux::UIElement target = element ? element.as<wux::UIElement>() : content.as<wux::UIElement>();
        enteredRevoker_ = target.PointerEntered(winrt::auto_revoke, [this](auto const&, auto const&) { flyoutHovered_ = true; });
        exitedRevoker_ = target.PointerExited(winrt::auto_revoke, [this](auto const&, auto const&) { flyoutHovered_ = false; });
    }

    void Tick() {
        bool held = GetAsyncKeyState(VK_LBUTTON) < 0 || GetAsyncKeyState(VK_RBUTTON) < 0 || GetAsyncKeyState(VK_MBUTTON) < 0;
        if (partHovered_ || flyoutHovered_ || held) {
            awayTicks_ = 0;
            return;
        }
        if (++awayTicks_ >= 3 && close_) {
            timer_.Stop();
            auto close = close_;  // Closing stops this, letting go of close_.
            close();
        }
    }

    std::function<void()> close_;
    wux::DispatcherTimer timer_{nullptr};
    wux::DispatcherTimer::Tick_revoker tickRevoker_;
    wuxc::Primitives::FlyoutBase::Opened_revoker openedRevoker_;
    wux::UIElement::PointerEntered_revoker enteredRevoker_;
    wux::UIElement::PointerExited_revoker exitedRevoker_;
    bool partHovered_ = false;
    bool flyoutHovered_ = false;
    int awayTicks_ = 0;
};

wuxc::FontIcon MakeGlyph(const wchar_t* glyph, double size) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    return icon;
}

void AddMenuEntries(winrt::Windows::Foundation::Collections::IVector<wuxc::MenuFlyoutItemBase> const& items,
                    const std::vector<MenuEntry>& entries, std::shared_ptr<int> chosen) {
    for (const MenuEntry& entry : entries) {
        size_t tab = entry.text.find(L'\t');
        winrt::hstring text(entry.text.substr(0, tab));
        switch (entry.kind) {
            case MenuEntry::Kind::Separator:
                items.Append(wuxc::MenuFlyoutSeparator());
                break;
            case MenuEntry::Kind::Heading: {
                wuxc::MenuFlyoutItem item;
                item.Text(text);
                item.IsEnabled(false);
                items.Append(item);
                break;
            }
            case MenuEntry::Kind::Submenu: {
                wuxc::MenuFlyoutSubItem submenu;
                submenu.Text(text);
                submenu.IsEnabled(entry.enabled);
                AddMenuEntries(submenu.Items(), entry.entries, chosen);
                items.Append(submenu);
                break;
            }
            case MenuEntry::Kind::Item: {
                wuxc::MenuFlyoutItem item{nullptr};
                if (entry.checked) {
                    wuxc::ToggleMenuFlyoutItem toggle;
                    toggle.IsChecked(true);
                    item = toggle;
                } else {
                    item = wuxc::MenuFlyoutItem();
                }
                item.Text(text);
                if (tab != std::wstring::npos) {
                    item.KeyboardAcceleratorTextOverride(entry.text.substr(tab + 1));
                }
                item.IsEnabled(entry.enabled);
                g_menuClicks->push_back(
                    item.Click(winrt::auto_revoke, [chosen, command = entry.command](auto const&, auto const&) { *chosen = command; }));
                items.Append(item);
                break;
            }
        }
    }
}

// Commands of the widget's own, for moving it among the others in its slot.
constexpr int kMoveLeftCommand = -1;
constexpr int kMoveRightCommand = -2;

// Opens the menu where the widget was last right-clicked, with Move left and Move right when other
// widgets share its slot. The chosen item goes to the app once the menu has closed, so what it
// does (a screenshot, say) doesn't have the menu in it.
void ShowMenu(const std::vector<MenuEntry>& entries) {
    auto target = g_menuTarget && g_menuTarget.XamlRoot() ? g_menuTarget : g_host->Element();
    if (!target || !target.XamlRoot()) {
        return;
    }
    ForgetMenu(true);
    auto chosen = std::make_shared<int>(0);
    wuxc::MenuFlyout menu;
    AddMenuEntries(menu.Items(), entries, chosen);
    if (g_host->CanMove(-1) || g_host->CanMove(1)) {
        menu.Items().Append(wuxc::MenuFlyoutSeparator());
        for (int command : {kMoveLeftCommand, kMoveRightCommand}) {
            wuxc::MenuFlyoutItem item;
            item.Text(command == kMoveLeftCommand ? L"Move left" : L"Move right");
            item.IsEnabled(g_host->CanMove(command == kMoveLeftCommand ? -1 : 1));
            g_menuClicks->push_back(item.Click(winrt::auto_revoke, [chosen, command](auto const&, auto const&) { *chosen = command; }));
            menu.Items().Append(item);
        }
    }
    g_menuClosed = menu.Closed([chosen](auto const& sender, auto const&) {
        if (g_menu == sender) {
            ForgetMenu(false);  // This handler too, which runs to its end.
        }
        if (*chosen == kMoveLeftCommand || *chosen == kMoveRightCommand) {
            g_host->Move(*chosen == kMoveLeftCommand ? -1 : 1);
        } else if (*chosen) {
            PostToApp(kMenuCommandMessage, *chosen, 0);
        }
    });
    menu.ShouldConstrainToRootBounds(false);  // Above the taskbar, outside its XAML.
    wuxc::Primitives::FlyoutShowOptions options;
    options.Position(g_menuPoint);
    options.Placement(wuxc::Primitives::FlyoutPlacementMode::TopEdgeAlignedLeft);
    g_menu = menu;
    menu.ShowAt(target, options);
}

}  // namespace TaskbarXaml

// Worker threads (switching the webcam) whose results come back to the UI thread as messages.
// Finished ones are closed as new ones start.
class WorkerThreads {
public:
    bool Start(LPTHREAD_START_ROUTINE run, void* parameter) {
        threads_.erase(std::remove_if(threads_.begin(), threads_.end(),
                                      [](HANDLE thread) {
                                          bool finished = WaitForSingleObject(thread, 0) == WAIT_OBJECT_0;
                                          if (finished) {
                                              CloseHandle(thread);
                                          }
                                          return finished;
                                      }),
                       threads_.end());
        HANDLE thread = CreateThread(nullptr, 0, run, parameter, 0, nullptr);
        if (!thread) {
            return false;
        }
        threads_.push_back(thread);
        return true;
    }

    // Waits for any still running, however long that takes: the mod's code mustn't be running when
    // Windhawk unloads it from Explorer. Tell them to stop first.
    void WaitAll() {
        for (HANDLE thread : threads_) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
        threads_.clear();
    }

    ~WorkerThreads() { WaitAll(); }

private:
    std::vector<HANDLE> threads_;
};

// Runs the UI on its own thread with a message loop, for Wh_ModInit/Wh_ModUninit.
class UiThread {
public:
    // run is called on the new thread (with COM and GDI+ set up) and returns an object that lives
    // until the mod is unloaded, or null if it couldn't start (the thread then ends). Returns once
    // the thread is running, without waiting for that: Wh_ModInit can run on Explorer's main
    // thread while Explorer starts.
    bool Start(std::function<std::shared_ptr<void>()> run) {
        run_ = std::move(run);
        ready_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId_);
        if (!thread_) {
            CloseHandle(ready_);
            ready_ = nullptr;
            return false;
        }
        WaitForSingleObject(ready_, INFINITE);  // Its message queue is there from then on, for Stop.
        return true;
    }

    // Ends the thread, waiting for it however long that takes: Windhawk unloads the mod from
    // Explorer as soon as Wh_ModUninit returns, and the mod's code still running then would crash
    // Explorer.
    void Stop() {
        if (!thread_) {
            return;
        }
        PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
        WaitForSingleObject(thread_, INFINITE);
        CloseHandle(thread_);
        thread_ = nullptr;
        CloseHandle(ready_);
        ready_ = nullptr;
    }

private:
    static DWORD WINAPI ThreadProc(void* parameter) {
        auto self = (UiThread*)parameter;
        MSG message;
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // Creates the message queue.
        SetEvent(self->ready_);
        HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ULONG_PTR gdiplusToken = 0;
        gp::GdiplusStartupInput startup;
        gp::GdiplusStartup(&gdiplusToken, &startup, nullptr);
        {
            std::shared_ptr<void> app = self->run_();
            while (app && GetMessageW(&message, nullptr, 0, 0) > 0) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        UnregisterWindowClasses();
        gp::GdiplusShutdown(gdiplusToken);
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
        return 0;
    }

    std::function<std::shared_ptr<void>()> run_;
    HANDLE thread_ = nullptr;
    HANDLE ready_ = nullptr;
    DWORD threadId_ = 0;
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// AudioFlipper: audio devices, their icons, webcams and settings.

namespace {

// ---------------------------------------------------------------------------
// Audio devices, with the Windows Core Audio API.

const CLSID kClsidMMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const IID kIidMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const IID kIidAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};
const IID kIidAudioSessionManager2 = {0x77AA99A0, 0x1BD6, 0x484F, {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};
const IID kIidAudioSessionControl2 = {0xBFB7FF88, 0x7239, 0x4FC9, {0x8F, 0xA2, 0x07, 0xC9, 0x50, 0xBE, 0x9C, 0x6D}};
const IID kIidSimpleAudioVolume = {0x87CE5498, 0x68D6, 0x44E5, {0x92, 0x15, 0x6D, 0xA4, 0x7E, 0xF8, 0x83, 0xD8}};

// PKEY_Device_FriendlyName: the name plus the audio hardware ("Headset (4- USB Audio Device)").
const PROPERTYKEY kKeyFriendlyName = {{0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}}, 14};
// PKEY_Device_DeviceDesc: for an endpoint, its own name ("Headset").
const PROPERTYKEY kKeyEndpointName = {{0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}}, 2};
// PKEY_DeviceInterface_FriendlyName: the audio hardware ("4- USB Audio Device").
const PROPERTYKEY kKeyInterfaceName = {{0x026E516E, 0xB814, 0x414B, {0x83, 0xCD, 0x85, 0x6D, 0x6F, 0xEF, 0x48, 0x22}}, 2};
// PKEY_AudioEndpoint_FormFactor.
const PROPERTYKEY kKeyFormFactor = {{0x1DA5D803, 0xD492, 0x4EDD, {0x8C, 0x23, 0xE0, 0xC0, 0xFF, 0xEE, 0x7F, 0x0E}}, 0};

// Windows has no public API for changing the default audio device. IPolicyConfig is the
// undocumented interface the Sound control panel uses; it has been stable since Windows 7 and is
// what other device switchers (EarTrumpet, SoundSwitch) rely on.
const CLSID kClsidPolicyConfig = {0x870AF99C, 0x171D, 0x4F9E, {0xAF, 0x0D, 0xE6, 0x3D, 0xF4, 0x0C, 0x2B, 0xC9}};
const IID kIidPolicyConfig = {0xF8679F50, 0x850A, 0x41CF, {0x9C, 0x72, 0x43, 0x0F, 0x29, 0x02, 0x90, 0xC8}};

struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// Releases a COM pointer when it goes out of scope.
template <typename T>
class ComPtr {
public:
    ComPtr() = default;
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ~ComPtr() { Reset(); }
    T* operator->() const { return pointer_; }
    T* Get() const { return pointer_; }
    T** Put() {
        Reset();
        return &pointer_;
    }
    void Reset() {
        if (pointer_) {
            pointer_->Release();
            pointer_ = nullptr;
        }
    }
    explicit operator bool() const { return pointer_ != nullptr; }

private:
    T* pointer_ = nullptr;
};

const IID kIidMMNotificationClient = {0x7991EEC9, 0x7E89, 0x4D85, {0x83, 0x90, 0x6C, 0x70, 0x3C, 0xEC, 0x60, 0xC0}};
const IID kIidAudioEndpointVolumeCallback = {0x657804FA, 0xD6AD, 0x4496, {0x8A, 0x60, 0x35, 0x27, 0x52, 0xAF, 0x4F, 0x89}};

// Tells the UI thread, by posting a message to a window, when something the widget shows has
// changed: the default devices, a device's name or icon, or the default devices' volume or mute.
// Windows calls it on its own threads. While a message waits to be handled, no more are posted;
// Handled lets them through again.
class AudioChangeListener final : public IMMNotificationClient, public IAudioEndpointVolumeCallback {
public:
    AudioChangeListener(HWND window, UINT message) : window_(window), message_(message) {}

    void Handled() { pending_ = false; }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG references = --references_;
        if (references == 0) {
            delete this;
        }
        return references;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (iid == IID_IUnknown || iid == kIidMMNotificationClient) {
            *object = static_cast<IMMNotificationClient*>(this);
        } else if (iid == kIidAudioEndpointVolumeCallback) {
            *object = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else {
            *object = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return Changed(); }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return Changed(); }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return Changed(); }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole role, LPCWSTR) override {
        return role == eConsole ? Changed() : S_OK;
    }

    // Only for the names and kind; Windows reports many other properties changing as well.
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY key) override {
        for (const PROPERTYKEY& shown : {kKeyFriendlyName, kKeyEndpointName, kKeyInterfaceName, kKeyFormFactor}) {
            if (key.fmtid == shown.fmtid && key.pid == shown.pid) {
                return Changed();
            }
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override { return Changed(); }

private:
    HRESULT Changed() {
        if (!pending_.exchange(true)) {
            PostMessageW(window_, message_, 0, 0);
        }
        return S_OK;
    }

    HWND window_;
    UINT message_;
    std::atomic<ULONG> references_{1};
    std::atomic<bool> pending_{false};
};

enum class DeviceKind { Speakers, Headphones, Headset, Display, Microphone };

struct AudioDevice {
    std::wstring id;
    std::wstring name;         // The device's own name, which can be changed in the Sound control panel ("Headset").
    std::wstring description;  // The audio hardware ("4- USB Advanced Audio Device").
    std::wstring fullName;     // Name plus the audio hardware ("Headset (4- USB Advanced Audio Device)").
    DeviceKind kind = DeviceKind::Speakers;
};

struct AudioStatus {
    AudioDevice device;
    float level = 0;  // Volume from 0 to 1.
    bool muted = false;

    int Percent() const { return (int)std::lround(level * 100); }

    bool operator==(const AudioStatus& other) const {
        return device.id == other.device.id && device.name == other.device.name && device.description == other.device.description &&
               device.fullName == other.device.fullName && device.kind == other.device.kind && level == other.level &&
               muted == other.muted;
    }
};

std::wstring ReadStringProperty(IPropertyStore* store, const PROPERTYKEY& key) {
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(store->GetValue(key, &value)) && value.vt == VT_LPWSTR && value.pwszVal) {
        text = value.pwszVal;
    }
    PropVariantClear(&value);
    return text;
}

// Values of the EndpointFormFactor enum (mmdeviceapi.h).
DeviceKind ReadKind(IPropertyStore* store) {
    PROPVARIANT value;
    PropVariantInit(&value);
    DeviceKind kind = DeviceKind::Speakers;
    if (SUCCEEDED(store->GetValue(kKeyFormFactor, &value)) && value.vt == VT_UI4) {
        switch (value.ulVal) {
            case 3:  // Headphones
                kind = DeviceKind::Headphones;
                break;
            case 4:  // Microphone
                kind = DeviceKind::Microphone;
                break;
            case 5:  // Headset
            case 6:  // Handset
                kind = DeviceKind::Headset;
                break;
            case 9:  // DigitalAudioDisplayDevice (HDMI / DisplayPort monitor)
                kind = DeviceKind::Display;
                break;
        }
    }
    PropVariantClear(&value);
    return kind;
}

bool ReadDevice(IMMDevice* device, AudioDevice& info) {
    PWSTR id = nullptr;
    if (FAILED(device->GetId(&id))) {
        return false;
    }
    info.id = id;
    CoTaskMemFree(id);
    ComPtr<IPropertyStore> store;
    if (FAILED(device->OpenPropertyStore(STGM_READ, store.Put()))) {
        return false;
    }
    info.fullName = ReadStringProperty(store.Get(), kKeyFriendlyName);
    info.name = ReadStringProperty(store.Get(), kKeyEndpointName);
    if (Trim(info.name).empty()) {
        info.name = info.fullName.empty() ? L"Unknown device" : info.fullName;
    }
    if (info.fullName.empty()) {
        info.fullName = info.name;
    }
    info.description = ReadStringProperty(store.Get(), kKeyInterfaceName);
    info.kind = ReadKind(store.Get());
    return true;
}

int CompareNames(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT, NORM_IGNORECASE, a.c_str(), -1, b.c_str(), -1, nullptr, nullptr, 0) - 2;
}

// An icon, as the volume mixer shows it.
struct IconImage {
    int size = 0;
    std::vector<uint8_t> pixels;  // Premultiplied BGRA, top row first.
};

// An app using the default output, as the volume mixer shows it: its sessions there, together.
struct AppVolume {
    DWORD processId = 0;  // 0 for Windows' system sounds.
    std::wstring name;
    std::shared_ptr<const IconImage> icon;  // None for the system sounds, or a program without one.
    float level = 0;
    bool muted = false;

    bool operator==(const AppVolume& other) const = default;
};

// The program a process runs, or nothing when it can't be read.
std::wstring ProgramOf(DWORD processId) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }
    WCHAR path[MAX_PATH];
    DWORD length = MAX_PATH;
    std::wstring program = QueryFullProcessImageNameW(process, 0, path, &length) ? path : L"";
    CloseHandle(process);
    return program;
}

// A program's name as Windows shows it: its description ("Google Chrome"), or its file name.
std::wstring ProgramName(const std::wstring& path) {
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    std::vector<BYTE> info(size);
    struct Translation {
        WORD language;
        WORD codePage;
    }* translations = nullptr;
    UINT length = 0;
    if (size && GetFileVersionInfoW(path.c_str(), 0, size, info.data()) &&
        VerQueryValueW(info.data(), L"\\VarFileInfo\\Translation", (void**)&translations, &length) && length >= sizeof(Translation)) {
        wchar_t key[64];
        swprintf(key, 64, L"\\StringFileInfo\\%04x%04x\\FileDescription", translations[0].language, translations[0].codePage);
        wchar_t* description = nullptr;
        if (VerQueryValueW(info.data(), key, (void**)&description, &length) && length > 1 && !Trim(description).empty()) {
            return Trim(description);
        }
    }
    std::wstring name = path.substr(path.find_last_of(L'\\') + 1);
    size_t dot = name.find_last_of(L'.');
    return dot == std::wstring::npos ? name : name.substr(0, dot);
}

// A program's icon, at 32 pixels.
std::shared_ptr<const IconImage> ProgramIcon(const std::wstring& path) {
    SHFILEINFOW file{};
    if (!SHGetFileInfoW(path.c_str(), 0, &file, sizeof(file), SHGFI_ICON | SHGFI_LARGEICON) || !file.hIcon) {
        return nullptr;
    }
    ICONINFO info{};
    std::shared_ptr<IconImage> image;
    if (GetIconInfo(file.hIcon, &info) && info.hbmColor) {
        BITMAP bitmap{};
        GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap);
        int size = bitmap.bmWidth;
        if (size > 0 && size <= 256 && bitmap.bmHeight == size) {
            BITMAPINFO format{};
            format.bmiHeader = {sizeof(BITMAPINFOHEADER), size, -size, 1, 32, BI_RGB};
            image = std::make_shared<IconImage>();
            image->size = size;
            image->pixels.resize((size_t)size * size * 4);
            HDC screen = GetDC(nullptr);
            bool read = GetDIBits(screen, info.hbmColor, 0, size, image->pixels.data(), &format, DIB_RGB_COLORS) == size;
            // Icons without an alpha channel take their transparency from the mask.
            bool hasAlpha = false;
            for (size_t i = 3; i < image->pixels.size(); i += 4) {
                hasAlpha = hasAlpha || image->pixels[i] != 0;
            }
            if (read && !hasAlpha && info.hbmMask) {
                std::vector<uint8_t> mask(image->pixels.size());
                if (GetDIBits(screen, info.hbmMask, 0, size, mask.data(), &format, DIB_RGB_COLORS) == size) {
                    for (size_t i = 0; i < mask.size(); i += 4) {
                        image->pixels[i + 3] = mask[i] ? 0 : 255;
                    }
                }
            }
            ReleaseDC(nullptr, screen);
            // Premultiplied, as XAML takes it.
            for (size_t i = 0; i < image->pixels.size(); i += 4) {
                uint8_t alpha = image->pixels[i + 3];
                for (int c = 0; c < 3; c++) {
                    image->pixels[i + c] = (uint8_t)(image->pixels[i + c] * alpha / 255);
                }
            }
            if (!read) {
                image = nullptr;
            }
        }
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    DestroyIcon(file.hIcon);
    return image;
}

// Lists output (eRender) and input (eCapture) devices, reads the default device's volume and
// changes the default device. Used on the UI thread.
class AudioDevices {
public:
    AudioDevices() {
        CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL, kIidMMDeviceEnumerator, (void**)enumerator_.Put());
    }

    ~AudioDevices() {
        for (CachedVolume& cache : volumes_) {
            ForgetVolume(cache);
        }
        if (listener_) {
            if (enumerator_) {
                enumerator_->UnregisterEndpointNotificationCallback(listener_);
            }
            listener_->Release();
        }
    }

    AudioDevices(const AudioDevices&) = delete;
    AudioDevices& operator=(const AudioDevices&) = delete;

    // Posts the message to the window when the default devices, their names, icons, volume or mute
    // change, or devices come and go. ChangeHandled lets the next one through.
    void Listen(HWND window, UINT message) {
        listener_ = new AudioChangeListener(window, message);
        if (enumerator_) {
            enumerator_->RegisterEndpointNotificationCallback(listener_);
        }
    }

    void ChangeHandled() {
        if (listener_) {
            listener_->Handled();
        }
    }

    // Enabled, connected devices in that direction, sorted by name.
    std::vector<AudioDevice> ActiveDevices(EDataFlow flow) {
        std::vector<AudioDevice> devices;
        ComPtr<IMMDeviceCollection> collection;
        if (!enumerator_ || FAILED(enumerator_->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, collection.Put()))) {
            return devices;
        }
        UINT count = 0;
        collection->GetCount(&count);
        for (UINT i = 0; i < count; i++) {
            ComPtr<IMMDevice> device;
            AudioDevice info;
            if (SUCCEEDED(collection->Item(i, device.Put())) && ReadDevice(device.Get(), info)) {
                devices.push_back(info);
            }
        }
        std::sort(devices.begin(), devices.end(), [](const AudioDevice& a, const AudioDevice& b) {
            int byName = CompareNames(a.name, b.name);
            return byName != 0 ? byName < 0 : CompareNames(a.fullName, b.fullName) < 0;
        });
        return devices;
    }

    // ID of the default device in that direction, or empty if there's none.
    std::wstring DefaultDeviceId(EDataFlow flow) {
        ComPtr<IMMDevice> device;
        PWSTR id = nullptr;
        if (!enumerator_ || FAILED(enumerator_->GetDefaultAudioEndpoint(flow, eConsole, device.Put())) || FAILED(device->GetId(&id))) {
            return {};
        }
        std::wstring result = id;
        CoTaskMemFree(id);
        return result;
    }

    // The default device in that direction with its volume, or nothing if there's none. When it
    // can't be read (usually the device was unplugged between calls), sets failed so the caller can
    // keep what it had; the next change starts over.
    std::optional<AudioStatus> DefaultStatus(EDataFlow flow, bool* failed = nullptr) {
        CachedVolume& cache = volumes_[flow == eCapture ? 1 : 0];
        ComPtr<IMMDevice> device;
        AudioStatus status;
        auto fail = [&]() -> std::optional<AudioStatus> {
            ForgetVolume(cache);
            if (failed) {
                *failed = true;
            }
            return std::nullopt;
        };
        if (!enumerator_ || FAILED(enumerator_->GetDefaultAudioEndpoint(flow, eConsole, device.Put()))) {
            ForgetVolume(cache);
            return std::nullopt;
        }
        // The name and icon are read every time so renames in Windows show up straight away.
        if (!ReadDevice(device.Get(), status.device)) {
            return fail();
        }
        if (!cache.control || cache.deviceId != status.device.id) {
            // A new default device: listen to its volume instead of the last one's.
            ForgetVolume(cache);
            if (FAILED(device->Activate(kIidAudioEndpointVolume, CLSCTX_ALL, nullptr, (void**)cache.control.Put()))) {
                return fail();
            }
            cache.deviceId = status.device.id;
            if (listener_) {
                cache.control->RegisterControlChangeNotify(listener_);
            }
        }
        BOOL muted = FALSE;
        if (FAILED(cache.control->GetMasterVolumeLevelScalar(&status.level)) || FAILED(cache.control->GetMute(&muted))) {
            return fail();
        }
        status.muted = muted != FALSE;
        return status;
    }

    // Makes the device the default for its direction (and for calls, if asked).
    HRESULT SetDefaultDevice(const std::wstring& id, bool includeCommunications) {
        ComPtr<IPolicyConfig> policy;
        HRESULT result = CoCreateInstance(kClsidPolicyConfig, nullptr, CLSCTX_ALL, kIidPolicyConfig, (void**)policy.Put());
        if (FAILED(result)) {
            return result;
        }
        for (ERole role : {eConsole, eMultimedia, eCommunications}) {
            if (role == eCommunications && !includeCommunications) {
                continue;
            }
            result = policy->SetDefaultEndpoint(id.c_str(), role);
            if (FAILED(result)) {
                return result;
            }
        }
        return S_OK;
    }

    void ToggleDefaultMute(EDataFlow flow) {
        if (!DefaultStatus(flow)) {
            return;
        }
        IAudioEndpointVolume* volume = volumes_[flow == eCapture ? 1 : 0].control.Get();
        BOOL muted = FALSE;
        if (SUCCEEDED(volume->GetMute(&muted))) {
            volume->SetMute(!muted, nullptr);
        }
    }

    // Changes the default device's volume by whole percentage points, unmuting when turning it up.
    void ChangeDefaultVolume(EDataFlow flow, int points) {
        if (std::optional<AudioStatus> status = DefaultStatus(flow)) {
            SetVolume(volumes_[flow == eCapture ? 1 : 0].control.Get(), (status->Percent() + points) / 100.0f, points > 0);
        }
    }

    // The apps using the default output, as the volume mixer shows them: in Windows' order, with
    // the system sounds last.
    std::vector<AppVolume> AppVolumes() {
        std::vector<AppVolume> apps;
        ForEachSession([&](DWORD processId, ISimpleAudioVolume* volume) {
            if (std::any_of(apps.begin(), apps.end(), [&](const AppVolume& app) { return app.processId == processId; })) {
                return;  // Its first session speaks for the app.
            }
            AppVolume app;
            app.processId = processId;
            BOOL muted = FALSE;
            if (FAILED(volume->GetMasterVolume(&app.level)) || FAILED(volume->GetMute(&muted))) {
                return;
            }
            app.muted = muted != FALSE;
            if (processId == 0) {
                app.name = L"System sounds";
            } else {
                std::wstring program = ProgramOf(processId);
                if (program.empty()) {
                    return;  // Gone, or not ours to read.
                }
                auto known = programs_.find(program);
                if (known == programs_.end()) {
                    known = programs_.emplace(program, std::make_pair(ProgramName(program), ProgramIcon(program))).first;
                }
                app.name = known->second.first;
                app.icon = known->second.second;
            }
            apps.push_back(app);
        });
        std::stable_partition(apps.begin(), apps.end(), [](const AppVolume& app) { return app.processId != 0; });
        return apps;
    }

    // Sets an app's volume (0 to 1) on the default output, unmuting it when turned up.
    void SetAppVolume(DWORD processId, float level) {
        ForEachSession([&](DWORD sessionProcess, ISimpleAudioVolume* volume) {
            if (sessionProcess == processId) {
                volume->SetMasterVolume(std::max(0.0f, std::min(1.0f, level)), nullptr);
                if (level > 0) {
                    volume->SetMute(FALSE, nullptr);
                }
            }
        });
    }

    // Mutes an app on the default output, or unmutes it when it's muted.
    void ToggleAppMute(DWORD processId) {
        bool anyPlaying = false;
        ForEachSession([&](DWORD sessionProcess, ISimpleAudioVolume* volume) {
            BOOL muted = FALSE;
            if (sessionProcess == processId && SUCCEEDED(volume->GetMute(&muted)) && !muted) {
                anyPlaying = true;
            }
        });
        ForEachSession([&](DWORD sessionProcess, ISimpleAudioVolume* volume) {
            if (sessionProcess == processId) {
                volume->SetMute(anyPlaying, nullptr);
            }
        });
    }

    // Sets the default device's volume (0 to 1), unmuting it like the Windows volume slider does.
    // Called for every slider movement, so it reuses the default device's volume control, which is
    // swapped for the new one's as soon as Windows says the default device changed.
    void SetDefaultVolume(EDataFlow flow, float level) {
        CachedVolume& cache = volumes_[flow == eCapture ? 1 : 0];
        if (!cache.control && !DefaultStatus(flow)) {
            return;
        }
        if (!SetVolume(cache.control.Get(), level, level > 0)) {
            ForgetVolume(cache);  // The device went away; the next change picks up the new default.
        }
    }

private:
    struct CachedVolume {
        std::wstring deviceId;
        ComPtr<IAudioEndpointVolume> control;
    };

    // Calls visit with each live sound session on the default output: its process (0 for the
    // system sounds) and its volume.
    template <typename Visit>
    void ForEachSession(Visit&& visit) {
        ComPtr<IMMDevice> device;
        ComPtr<IAudioSessionManager2> manager;
        ComPtr<IAudioSessionEnumerator> sessions;
        if (!enumerator_ || FAILED(enumerator_->GetDefaultAudioEndpoint(eRender, eConsole, device.Put())) ||
            FAILED(device->Activate(kIidAudioSessionManager2, CLSCTX_ALL, nullptr, (void**)manager.Put())) ||
            FAILED(manager->GetSessionEnumerator(sessions.Put()))) {
            return;
        }
        int count = 0;
        sessions->GetCount(&count);
        for (int i = 0; i < count; i++) {
            ComPtr<IAudioSessionControl> control;
            ComPtr<IAudioSessionControl2> control2;
            ComPtr<ISimpleAudioVolume> volume;
            AudioSessionState state = AudioSessionStateExpired;
            if (FAILED(sessions->GetSession(i, control.Put())) || FAILED(control->GetState(&state)) || state == AudioSessionStateExpired ||
                FAILED(control->QueryInterface(kIidAudioSessionControl2, (void**)control2.Put())) ||
                FAILED(control->QueryInterface(kIidSimpleAudioVolume, (void**)volume.Put()))) {
                continue;
            }
            DWORD processId = 0;
            bool system = control2->IsSystemSoundsSession() == S_OK;
            if (!system && (FAILED(control2->GetProcessId(&processId)) || processId == 0)) {
                continue;
            }
            visit(system ? 0 : processId, volume.Get());
        }
    }

    void ForgetVolume(CachedVolume& cache) {
        if (cache.control && listener_) {
            cache.control->UnregisterControlChangeNotify(listener_);
        }
        cache.control.Reset();
        cache.deviceId.clear();
    }

    static bool SetVolume(IAudioEndpointVolume* volume, float level, bool unmute) {
        if (FAILED(volume->SetMasterVolumeLevelScalar(std::max(0.0f, std::min(1.0f, level)), nullptr))) {
            return false;
        }
        BOOL muted = FALSE;
        if (unmute && SUCCEEDED(volume->GetMute(&muted)) && muted) {
            volume->SetMute(FALSE, nullptr);
        }
        return true;
    }

    ComPtr<IMMDeviceEnumerator> enumerator_;
    AudioChangeListener* listener_ = nullptr;
    // The default device's volume control, per direction, kept between refreshes and only
    // reactivated when the default device changes.
    CachedVolume volumes_[2];
    // The programs' names and icons, read once each.
    std::map<std::wstring, std::pair<std::wstring, std::shared_ptr<const IconImage>>> programs_;
};

// ---------------------------------------------------------------------------
// Icons: glyphs from the Windows icon font, in the taskbar's color, like its own icons.

const wchar_t* const kGlyphMute = L"\uE74F";
const wchar_t* const kGlyphMicOff = L"\uEC54";
const wchar_t* const kGlyphSpeakers = L"\uE7F5";
const wchar_t* const kGlyphHeadphone = L"\uE7F6";
const wchar_t* const kGlyphHeadset = L"\uE95B";
const wchar_t* const kGlyphTvMonitor = L"\uE7F4";
const wchar_t* const kGlyphMicrophone = L"\uE720";
const wchar_t* const kGlyphVideo = L"\uE714";

// The glyph for a muted (or missing) output or input: a struck-through speaker or microphone.
const wchar_t* MutedGlyph(EDataFlow flow) {
    return flow == eCapture ? kGlyphMicOff : kGlyphMute;
}

// The glyph for the output's or input's kind, or the muted one when there's no device. Inputs of
// other kinds (line in, S/PDIF, Remote Audio) are microphones, and outputs of other kinds speakers.
const wchar_t* GlyphFor(EDataFlow flow, const std::optional<AudioStatus>& status) {
    if (!status) {
        return MutedGlyph(flow);
    }
    switch (status->device.kind) {
        case DeviceKind::Headphones:
            return kGlyphHeadphone;
        case DeviceKind::Headset:
            return kGlyphHeadset;
        case DeviceKind::Display:
            return kGlyphTvMonitor;
        case DeviceKind::Microphone:
            return kGlyphMicrophone;
        default:
            return flow == eCapture ? kGlyphMicrophone : kGlyphSpeakers;
    }
}

// Draws an icon-font glyph at one font size, centered in the box by its ink, the way the taskbar
// draws its own glyphs: unlike DrawGlyph, it doesn't stretch each glyph to fill the box, so they all
// keep the same size and weight. Returns where the ink went.
gp::RectF DrawGlyphAtSize(gp::Graphics& graphics, const wchar_t* glyph, float emSize, const gp::RectF& box, const gp::Color& color) {
    const wchar_t* fontName = IconFontName();
    if (!fontName) {
        DrawGlyph(graphics, glyph, box, color);  // Draws a placeholder.
        return box;
    }
    gp::FontFamily family(fontName);
    gp::GraphicsPath path;
    path.AddString(glyph, -1, &family, gp::FontStyleRegular, emSize, gp::PointF(0, 0), gp::StringFormat::GenericTypographic());
    gp::RectF ink;
    path.GetBounds(&ink);
    if (ink.Width <= 0 || ink.Height <= 0) {
        return box;
    }
    gp::RectF placed(box.X + (box.Width - ink.Width) / 2, box.Y + (box.Height - ink.Height) / 2, ink.Width, ink.Height);
    gp::Matrix transform;
    transform.Translate(placed.X - ink.X, placed.Y - ink.Y);
    path.Transform(&transform);
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
    return placed;
}

// Draws the icon of the output or input device: a glyph at three quarters of the box's height, about
// the size of the taskbar's own icons, struck through when the device is muted.
void DrawDeviceIcon(gp::Graphics& graphics, EDataFlow flow, const std::optional<AudioStatus>& status, const gp::RectF& bounds,
                    const gp::Color& color) {
    bool muted = status && status->muted;
    DrawGlyphAtSize(graphics, muted ? MutedGlyph(flow) : GlyphFor(flow, status), bounds.Height * 0.75f, bounds, color);
}

// ---------------------------------------------------------------------------
// Webcams: found with SetupAPI, and turned on or off the way Device Manager's Enable/Disable device
// does, with Windows' PnP Utility (pnputil.exe), which needs administrator rights.

struct Webcam {
    std::wstring instanceId;  // "USB\VID_046D&PID_085E&MI_00\7&1A2B3C&0&0000"
    std::wstring name;
    bool enabled = true;  // False when disabled (as with Disable device in Device Manager).
};

bool DevNodeEnabled(DEVINST devInst) {
    ULONG status = 0;
    ULONG problem = 0;
    if (CM_Get_DevNode_Status(&status, &problem, devInst, 0) != CR_SUCCESS) {
        return true;
    }
    return !((status & DN_HAS_PROBLEM) && problem == CM_PROB_DISABLED);
}

// Whether the device can capture video. Webcams can; scanners, which share the "Imaging devices"
// setup class with older webcams, can't. Turned-off webcams count too.
bool CapturesVideo(const std::wstring& instanceId) {
    // KSCATEGORY_VIDEO_CAMERA, and KSCATEGORY_CAPTURE for drivers from before Windows 10 had it.
    const GUID categories[] = {{0xE5323777, 0xF976, 0x4F5B, {0x9B, 0x55, 0xB9, 0x46, 0x99, 0xC4, 0x6E, 0x44}},
                               {0x65E8773D, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}}};
    for (const GUID& category : categories) {
        ULONG size = 0;
        if (CM_Get_Device_Interface_List_SizeW(&size, (LPGUID)&category, (DEVINSTID_W)instanceId.c_str(),
                                               CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES) == CR_SUCCESS &&
            size > 1) {
            return true;
        }
    }
    return false;
}

// Connected webcams, including turned-off ones, sorted by name.
std::vector<Webcam> GetWebcams() {
    // Device setup classes webcams install under: "Cameras", and "Imaging devices" for older drivers.
    const GUID cameraClass = {0xCA3E7AB9, 0xB4C3, 0x4AE6, {0x82, 0x51, 0x57, 0x9E, 0xF9, 0x33, 0x89, 0x0F}};
    const GUID imagingClass = {0x6BDD1FC6, 0x810F, 0x11D0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}};
    std::vector<Webcam> webcams;
    for (const GUID& classGuid : {cameraClass, imagingClass}) {
        HDEVINFO set = SetupDiGetClassDevsW(&classGuid, nullptr, nullptr, DIGCF_PRESENT);
        if (set == INVALID_HANDLE_VALUE) {
            continue;
        }
        SP_DEVINFO_DATA device{sizeof(device)};
        for (DWORD index = 0; SetupDiEnumDeviceInfo(set, index, &device); index++) {
            WCHAR id[512] = {};
            if (!SetupDiGetDeviceInstanceIdW(set, &device, id, ARRAYSIZE(id), nullptr) ||
                std::any_of(webcams.begin(), webcams.end(), [&](const Webcam& w) { return w.instanceId == id; }) ||
                (!IsEqualGUID(classGuid, cameraClass) && !CapturesVideo(id))) {
                continue;
            }
            Webcam webcam;
            webcam.instanceId = id;
            for (DWORD property : {(DWORD)SPDRP_FRIENDLYNAME, (DWORD)SPDRP_DEVICEDESC}) {
                WCHAR name[512] = {};
                if (webcam.name.empty() &&
                    SetupDiGetDeviceRegistryPropertyW(set, &device, property, nullptr, (BYTE*)name, sizeof(name) - sizeof(WCHAR), nullptr)) {
                    webcam.name = Trim(name);
                }
            }
            if (webcam.name.empty()) {
                webcam.name = webcam.instanceId;
            }
            webcam.enabled = DevNodeEnabled(device.DevInst);
            webcams.push_back(webcam);
        }
        SetupDiDestroyDeviceInfoList(set);
    }
    std::sort(webcams.begin(), webcams.end(), [](const Webcam& a, const Webcam& b) { return CompareNames(a.name, b.name) < 0; });
    return webcams;
}

// Whether the webcam is turned on, or nothing when it isn't connected. Cheap enough to check on
// every device change.
std::optional<bool> WebcamEnabled(const std::wstring& instanceId) {
    DEVINST devInst = 0;
    if (instanceId.empty() || CM_Locate_DevNodeW(&devInst, (DEVINSTID_W)instanceId.c_str(), CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) {
        return std::nullopt;
    }
    return DevNodeEnabled(devInst);
}

enum class WebcamChange { Done, RestartNeeded, Cancelled, Failed };

const DWORD kErrorSuccessRebootRequired = 3010;

// Waits for a process and returns its exit code; or, if stop is set first, leaves it running and
// returns a failure.
DWORD WaitForExitCode(HANDLE process, HANDLE stop) {
    HANDLE handles[] = {process, stop};
    DWORD exitCode = 1;
    if (WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
        GetExitCodeProcess(process, &exitCode);
    }
    CloseHandle(process);
    return exitCode;
}

// Turns the webcam on or off. Runs on a worker thread (with COM): it may wait for a UAC prompt,
// which comes up over the owner window.
WebcamChange SetWebcamEnabled(const std::wstring& instanceId, bool enable, HWND owner, HANDLE stop) {
    // Only ever hand pnputil the ID of a webcam that's actually connected, never anything else read
    // from the mod's storage, since it runs with administrator rights.
    std::vector<Webcam> webcams = GetWebcams();
    if (instanceId.empty() || instanceId.find(L'"') != std::wstring::npos ||
        std::none_of(webcams.begin(), webcams.end(), [&](const Webcam& w) { return w.instanceId == instanceId; })) {
        return WebcamChange::Failed;
    }

    std::wstring arguments = (enable ? L"/enable-device \"" : L"/disable-device \"") + instanceId + L"\"";
    DWORD exitCode = 1;
    if (IsProcessElevated()) {
        // Running as administrator already (as the app can be), so no prompt is needed.
        HANDLE process = nullptr;
        if (!Launch(SystemFolder() + L"\\pnputil.exe", arguments, true, &process)) {
            return WebcamChange::Failed;
        }
        exitCode = WaitForExitCode(process, stop);
    } else {
        std::wstring pnputil = SystemFolder() + L"\\pnputil.exe";
        SHELLEXECUTEINFOW execute{sizeof(execute)};
        execute.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
        execute.hwnd = owner;
        execute.lpVerb = L"runas";
        execute.lpFile = pnputil.c_str();
        execute.lpParameters = arguments.c_str();
        execute.nShow = SW_HIDE;
        if (!ShellExecuteExW(&execute) || !execute.hProcess) {
            return GetLastError() == ERROR_CANCELLED ? WebcamChange::Cancelled : WebcamChange::Failed;  // "No" on the UAC prompt.
        }
        exitCode = WaitForExitCode(execute.hProcess, stop);
    }
    // Trust the device's actual state over pnputil's exit code.
    if (exitCode == kErrorSuccessRebootRequired) {
        return WebcamChange::RestartNeeded;
    }
    return WebcamEnabled(instanceId) == enable ? WebcamChange::Done : WebcamChange::Failed;
}

// ---------------------------------------------------------------------------
// Settings, kept in the mod's storage. The first time the mod runs for a user, it takes on
// AudioFlipper.exe's settings from %APPDATA%\AudioFlipper\settings.txt, which it only reads.

struct Settings {
    bool switchCommunications = true;       // Also make the new output the default communications device.
    bool switchInputCommunications = true;  // Also make the new input the default communications device.
    bool showPopup = true;                  // Show the popup when the output changes (while the widget isn't showing it).
    bool volumeDing = true;                 // Play the Windows "Default Beep" after changing the output volume.
    bool showOutput = true;
    bool showInput = false;
    bool showWebcam = false;
    std::wstring webcamId;
    std::wstring webcamName;
    std::vector<std::wstring> excluded;
    // Names given to webcams (Windows has no way to rename one), by device instance ID, in the
    // order they were given.
    std::vector<std::pair<std::wstring, std::wstring>> webcamNicknames;

    bool SwitchesCommunications(EDataFlow flow) const { return flow == eCapture ? switchInputCommunications : switchCommunications; }

    bool IsExcluded(const std::wstring& id) const {
        return std::any_of(excluded.begin(), excluded.end(), [&](const std::wstring& e) { return EqualsIgnoreCase(e, id); });
    }

    void SetExcluded(const std::wstring& id, bool exclude) {
        excluded.erase(std::remove_if(excluded.begin(), excluded.end(), [&](const std::wstring& e) { return EqualsIgnoreCase(e, id); }),
                       excluded.end());
        if (exclude) {
            excluded.push_back(id);
        }
    }

    // The name given to a webcam, or nothing to use the device's own name.
    std::optional<std::wstring> WebcamNickname(const std::wstring& instanceId) const {
        for (const auto& [id, nickname] : webcamNicknames) {
            if (EqualsIgnoreCase(id, instanceId)) {
                return nickname;
            }
        }
        return std::nullopt;
    }

    // Names a webcam; an empty name goes back to the device's own name.
    void SetWebcamNickname(const std::wstring& instanceId, std::wstring nickname) {
        // Settings are stored one per line, so keep names to a single line.
        for (wchar_t& c : nickname) {
            if (c == L'\r' || c == L'\n' || c == L'\t') {
                c = L' ';
            }
        }
        nickname = Trim(nickname);
        auto found = std::find_if(webcamNicknames.begin(), webcamNicknames.end(),
                                  [&](const auto& entry) { return EqualsIgnoreCase(entry.first, instanceId); });
        if (nickname.empty()) {
            if (found != webcamNicknames.end()) {
                webcamNicknames.erase(found);
            }
        } else if (found != webcamNicknames.end()) {
            found->second = nickname;
        } else {
            webcamNicknames.emplace_back(instanceId, nickname);
        }
    }

    void Load() {
        if (!Storage::GetInt(L"Imported", 0)) {
            Import();
            Save();
            Storage::SetInt(L"Imported", 1);
            return;
        }
        switchCommunications = Storage::GetInt(L"SwitchCommunications", switchCommunications) != 0;
        switchInputCommunications = Storage::GetInt(L"SwitchInputCommunications", switchInputCommunications) != 0;
        showPopup = Storage::GetInt(L"ShowPopup", showPopup) != 0;
        volumeDing = Storage::GetInt(L"VolumeDing", volumeDing) != 0;
        showOutput = Storage::GetInt(L"ShowOutput", showOutput) != 0;
        showInput = Storage::GetInt(L"ShowInput", showInput) != 0;
        showWebcam = Storage::GetInt(L"ShowWebcam", showWebcam) != 0;
        webcamId = Storage::GetString(L"WebcamId");
        webcamName = Storage::GetString(L"WebcamName");
        for (const std::wstring& id : Split(Storage::GetString(L"Excluded"), L'\n')) {
            SetExcluded(id, true);
        }
        for (const std::wstring& line : Split(Storage::GetString(L"WebcamNicknames"), L'\n')) {
            AddNicknameLine(line);
        }
    }

    bool Save() const {
        std::vector<std::wstring> nicknameLines;
        for (const auto& [id, nickname] : webcamNicknames) {
            nicknameLines.push_back(id + L"\t" + nickname);
        }
        bool saved = Storage::SetInt(L"SwitchCommunications", switchCommunications);
        saved = Storage::SetInt(L"SwitchInputCommunications", switchInputCommunications) && saved;
        saved = Storage::SetInt(L"ShowPopup", showPopup) && saved;
        saved = Storage::SetInt(L"VolumeDing", volumeDing) && saved;
        saved = Storage::SetInt(L"ShowOutput", showOutput) && saved;
        saved = Storage::SetInt(L"ShowInput", showInput) && saved;
        saved = Storage::SetInt(L"ShowWebcam", showWebcam) && saved;
        saved = Storage::SetString(L"WebcamId", webcamId) && saved;
        saved = Storage::SetString(L"WebcamName", webcamName) && saved;
        saved = Storage::SetString(L"Excluded", Join(excluded, L"\n")) && saved;
        return Storage::SetString(L"WebcamNicknames", Join(nicknameLines, L"\n")) && saved;
    }

private:
    // AudioFlipper.exe's settings.txt has "key=value" lines.
    void Import() {
        std::wstring path = KnownFolder(FOLDERID_RoamingAppData) + L"\\AudioFlipper\\settings.txt";
        for (const auto& [key, value] : ReadSettingsFile(path)) {
            if (key == L"SwitchCommunications") {
                switchCommunications = ParseBool(value, switchCommunications);
            } else if (key == L"ShowPopup") {
                showPopup = ParseBool(value, showPopup);
            } else if (key == L"ShowOutput" || key == L"ShowOnTaskbar") {  // Before 1.1 there was one switch for the whole widget.
                showOutput = ParseBool(value, showOutput);
            } else if (key == L"VolumeDing") {
                volumeDing = ParseBool(value, volumeDing);
            } else if (key == L"ShowInput") {
                showInput = ParseBool(value, showInput);
            } else if (key == L"SwitchInputCommunications") {
                switchInputCommunications = ParseBool(value, switchInputCommunications);
            } else if (key == L"ShowWebcam") {
                showWebcam = ParseBool(value, showWebcam);
            } else if (key == L"WebcamId") {
                webcamId = value;
            } else if (key == L"WebcamName") {
                webcamName = value;
            } else if (key == L"WebcamNickname") {
                AddNicknameLine(value);
            } else if (key == L"Exclude") {
                SetExcluded(value, true);
            }
        }
    }

    // "<instance ID><tab><name>"; device instance IDs never contain tabs.
    void AddNicknameLine(const std::wstring& line) {
        size_t tab = line.find(L'\t');
        if (tab != std::wstring::npos && tab > 0 && tab < line.size() - 1) {
            SetWebcamNickname(line.substr(0, tab), line.substr(tab + 1));
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// AudioFlipper: the widget, its volume flyout and the app.

namespace {

// ---------------------------------------------------------------------------
// The widget, with up to three sections:
// - Output: the active output's icon, name and volume. Click to switch output, scroll to change
//   volume, middle-click to mute, hover for a volume slider.
// - Input: the same for the active microphone.
// - Webcam: a camera icon (struck through while off) with the chosen webcam's name over "On" or
//   "Off"; click to switch it.
// Right-click anywhere for the menu.

struct WebcamStatus {
    std::wstring name;  // The webcam's name, or the name it was given in AudioFlipper.
    bool enabled = true;
    bool busy = false;  // Being turned on or off (waiting on the UAC prompt or Windows).

    bool operator==(const WebcamStatus& other) const = default;
};

// What the widget shows; the app sends it.
struct WidgetContent {
    bool showOutput = false;
    bool showInput = false;
    std::optional<AudioStatus> output;
    std::optional<AudioStatus> input;
    std::optional<WebcamStatus> webcam;  // Nothing hides the webcam section.
    std::vector<AppVolume> apps;         // While the output's flyout is open, for its volume mixer.
};

// The widget's parts: the sections, then the volume flyout's mute buttons and settings links.
constexpr int kOutputPart = 0;
constexpr int kInputPart = 1;
constexpr int kWebcamPart = 2;
constexpr int kOutputMutePart = 3;
constexpr int kInputMutePart = 4;
constexpr int kSoundSettingsPart = 5;  // "More volume settings" (or sound settings, for the input).
constexpr int kMixerSettingsPart = 6;  // The volume mixer's gear.
// From the volume flyout to the app. wParam: the section's part (output or input).
constexpr UINT kVolumeMessage = WM_APP + 0x310;      // lParam: the slider's level, 0 to 1000.
constexpr UINT kVolumeDoneMessage = WM_APP + 0x311;  // The slider was let go.
// From the volume mixer to the app.
constexpr UINT kMixerMessage = WM_APP + 0x312;      // wParam: 1 when it opened, 0 when it closed.
constexpr UINT kAppVolumeMessage = WM_APP + 0x313;  // wParam: the app's process. lParam: its level, 0 to 1000.
constexpr UINT kAppMuteMessage = WM_APP + 0x314;    // wParam: the app's process.

const wchar_t* const kGlyphSettings = L"\uE713";
const wchar_t* const kGlyphApp = L"\uECAA";  // For an app without an icon.

std::optional<EDataFlow> FlowOfPart(int part) {
    if (part == kOutputPart || part == kOutputMutePart) {
        return eRender;
    }
    if (part == kInputPart || part == kInputMutePart) {
        return eCapture;
    }
    return std::nullopt;
}

using winrt::Windows::UI::Color;

wuxm::SolidColorBrush Brush(Color color) {
    return wuxm::SolidColorBrush(color);
}

// Windows' accent color for the taskbar's light or dark theme, read once on the taskbar's thread
// and kept, rather than each time the widget is drawn. Read again once Windows' settings change
// (ForgetAccentColors).
struct AccentColors {
    Color light{255, 0, 95, 184};
    Color dark{255, 96, 205, 255};
    bool read = false;
};

AccentColors g_accentColors;

Color AccentColor(bool light) {
    if (!g_accentColors.read) {
        g_accentColors = {};
        g_accentColors.read = true;
        try {
            using winrt::Windows::UI::ViewManagement::UIColorType;
            winrt::Windows::UI::ViewManagement::UISettings settings;
            g_accentColors.light = settings.GetColorValue(UIColorType::AccentDark1);
            g_accentColors.dark = settings.GetColorValue(UIColorType::AccentLight2);
        } catch (...) {
        }
    }
    return light ? g_accentColors.light : g_accentColors.dark;
}

void ForgetAccentColors() {
    g_accentColors.read = false;
}

// The colors for the taskbar's light or dark theme, with Windows' accent color.
struct ThemeColors {
    Color foreground, accent, track, muted;
};

ThemeColors ThemeColorsFor(bool light) {
    ThemeColors colors;
    colors.foreground = light ? Color{255, 28, 28, 28} : Color{255, 255, 255, 255};
    colors.accent = AccentColor(light);
    colors.track = light ? Color{60, 28, 28, 28} : Color{80, 255, 255, 255};
    colors.muted = light ? Color{255, 96, 96, 96} : Color{255, 197, 197, 197};
    return colors;
}

wuxc::TextBlock MakeText(double size, bool semibold) {
    wuxc::TextBlock text;
    text.FontSize(size);
    if (semibold) {
        text.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    }
    text.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    return text;
}

// Content for a widget part: the icon, then the rest of the part's width for the text, so it fills
// the part (and is cut short with "…" when the part is as wide as it gets).
wuxc::Grid MakeIconAndText(wux::UIElement const& icon, wux::FrameworkElement const& text) {
    wuxc::Grid grid;
    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(wux::GridLengthHelper::Auto());
    grid.ColumnDefinitions().Append(iconColumn);
    grid.ColumnDefinitions().Append(wuxc::ColumnDefinition());
    grid.Children().Append(icon);
    wuxc::Grid::SetColumn(text, 1);
    grid.Children().Append(text);
    return grid;
}

const wchar_t* DeviceGlyph(EDataFlow flow, const std::optional<AudioStatus>& status) {
    return status && status->muted ? MutedGlyph(flow) : GlyphFor(flow, status);
}

std::wstring DeviceName(EDataFlow flow, const std::optional<AudioStatus>& status) {
    if (status) {
        return status->device.name;
    }
    return flow == eCapture ? L"No input device" : L"No output device";
}

// On the taskbar's thread.
class AfView : public TaskbarXaml::View {
public:
    wux::FrameworkElement Build() override {
        row_ = wuxc::StackPanel();
        row_.Orientation(wuxc::Orientation::Horizontal);
        themeRevoker_ = row_.ActualThemeChanged(winrt::auto_revoke, [this](auto const&, auto const&) { Show(content_); });
        row_.Children().Append(BuildAudio(output_, kOutputPart));
        row_.Children().Append(BuildAudio(input_, kInputPart));
        row_.Children().Append(BuildWebcam());
        Show(content_);
        return row_;
    }

    void Forget() override {
        CloseFlyout();
        closer_.Forget();
        StopHoverTimer();
        hoverTimer_ = nullptr;
        themeRevoker_.revoke();
        for (AudioSection* section : {&output_, &input_}) {
            section->part.Forget();
            section->icon = nullptr;
            section->name = nullptr;
            section->level = nullptr;
            section->bar = nullptr;
            section->fill = nullptr;
            section->track = nullptr;
        }
        webcam_.part.Forget();
        webcam_.icon = nullptr;
        webcam_.slash = nullptr;
        webcam_.name = nullptr;
        webcam_.state = nullptr;
        row_ = nullptr;
    }

    void Show(const WidgetContent& content) {
        content_ = content;
        if (!row_) {
            return;
        }
        ThemeColors colors = ThemeColorsFor(row_.ActualTheme() == wux::ElementTheme::Light);
        ShowAudio(output_, eRender, content_.showOutput, content_.output, colors);
        ShowAudio(input_, eCapture, content_.showInput, content_.input, colors);
        ShowWebcam(colors);
        if (flyout_) {
            bool shown = flyoutPart_ == kOutputPart ? content_.showOutput : content_.showInput;
            if (shown) {
                ShowFlyoutStatus();
            } else {
                CloseFlyout();
            }
        }
    }

private:
    struct AudioSection {
        TaskbarXaml::Part part;
        wuxc::FontIcon icon{nullptr};
        wuxc::TextBlock name{nullptr};
        wuxc::TextBlock level{nullptr};
        wuxc::Grid bar{nullptr};
        wuxc::Border fill{nullptr};
        wuxc::Border track{nullptr};
    };

    struct WebcamSection {
        TaskbarXaml::Part part;
        wuxc::Grid icon{nullptr};
        wux::Shapes::Line slash{nullptr};
        wuxc::TextBlock name{nullptr};
        wuxc::TextBlock state{nullptr};
    };

    // The device's icon, then its name over "45%" and a level bar, like the weather widget's two
    // lines.
    wuxc::Border BuildAudio(AudioSection& section, int part) {
        section.icon = TaskbarXaml::MakeGlyph(kGlyphSpeakers, 18);
        section.icon.Width(24);
        section.icon.Margin(wux::ThicknessHelper::FromLengths(8, 0, 0, 0));
        wuxc::StackPanel lines;
        lines.VerticalAlignment(wux::VerticalAlignment::Center);
        lines.Margin(wux::ThicknessHelper::FromLengths(10, 0, 12, 0));
        section.name = MakeText(12, true);
        lines.Children().Append(section.name);
        wuxc::Grid second;
        wuxc::ColumnDefinition labelColumn;
        labelColumn.Width(wux::GridLengthHelper::Auto());
        second.ColumnDefinitions().Append(labelColumn);
        second.ColumnDefinitions().Append(wuxc::ColumnDefinition());
        section.level = MakeText(12, false);
        section.level.Opacity(0.75);
        section.level.MinWidth(36);  // As wide for "5%" as for "100%", so the widget keeps its width.
        second.Children().Append(section.level);
        // The bar, as long as the name above it or a little more: the fill takes the volume's share
        // of the width, in the first of two columns.
        section.bar = wuxc::Grid();
        section.bar.Height(3);
        section.bar.MinWidth(28);
        section.bar.VerticalAlignment(wux::VerticalAlignment::Center);
        section.bar.Margin(wux::ThicknessHelper::FromLengths(4, 2, 0, 0));
        section.bar.ColumnDefinitions().Append(wuxc::ColumnDefinition());
        section.bar.ColumnDefinitions().Append(wuxc::ColumnDefinition());
        section.track = wuxc::Border();
        section.track.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(1.5));
        wuxc::Grid::SetColumnSpan(section.track, 2);
        section.fill = wuxc::Border();
        section.fill.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(1.5));
        section.bar.Children().Append(section.track);
        section.bar.Children().Append(section.fill);
        wuxc::Grid::SetColumn(section.bar, 1);
        second.Children().Append(section.bar);
        lines.Children().Append(second);
        auto border = section.part.Make(part, MakeIconAndText(section.icon, lines), part == kOutputPart ? L"Output" : L"Input");
        // Sized to fit the device's name, like the weather widget sizes itself to its text.
        border.MaxWidth(256);
        section.part.onHover = [this, part](bool entered) { OnSectionHover(part, entered); };
        section.part.onPress = [this]() {
            StopHoverTimer();
            CloseFlyout();
        };
        return border;
    }

    void ShowAudio(AudioSection& section, EDataFlow flow, bool shown, const std::optional<AudioStatus>& status, const ThemeColors& colors) {
        section.part.Border().Visibility(shown ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        section.icon.Glyph(DeviceGlyph(flow, status));
        section.name.Text(DeviceName(flow, status));
        bool hasLevel = status.has_value();
        section.level.Visibility(hasLevel ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        section.bar.Visibility(hasLevel ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        if (!status) {
            return;
        }
        section.level.Text(status->muted ? L"Muted" : std::to_wstring(status->Percent()) + L"%");
        int percent = status->muted ? 0 : std::clamp(status->Percent(), 0, 100);
        section.bar.ColumnDefinitions().GetAt(0).Width(wux::GridLengthHelper::FromValueAndType(percent, wux::GridUnitType::Star));
        section.bar.ColumnDefinitions().GetAt(1).Width(wux::GridLengthHelper::FromValueAndType(100 - percent, wux::GridUnitType::Star));
        section.fill.Visibility(percent > 0 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        section.track.Background(Brush(colors.track));
        section.fill.Background(Brush(colors.accent));
    }

    // The camera icon (struck through while off), then the webcam's name over "On" or "Off".
    wuxc::Border BuildWebcam() {
        webcam_.icon = wuxc::Grid();
        webcam_.icon.Width(24);
        webcam_.icon.Height(24);
        webcam_.icon.Margin(wux::ThicknessHelper::FromLengths(8, 0, 0, 0));
        webcam_.icon.VerticalAlignment(wux::VerticalAlignment::Center);
        webcam_.icon.Children().Append(TaskbarXaml::MakeGlyph(kGlyphVideo, 18));
        webcam_.slash = wux::Shapes::Line();
        webcam_.slash.X1(4);
        webcam_.slash.Y1(4);
        webcam_.slash.X2(20);
        webcam_.slash.Y2(20);
        webcam_.slash.StrokeThickness(1.5);
        webcam_.slash.StrokeStartLineCap(wuxm::PenLineCap::Round);
        webcam_.slash.StrokeEndLineCap(wuxm::PenLineCap::Round);
        webcam_.icon.Children().Append(webcam_.slash);
        wuxc::StackPanel lines;
        lines.VerticalAlignment(wux::VerticalAlignment::Center);
        lines.Margin(wux::ThicknessHelper::FromLengths(6, 0, 12, 0));
        webcam_.name = MakeText(12, true);
        webcam_.state = MakeText(12, false);
        webcam_.state.Opacity(0.75);
        lines.Children().Append(webcam_.name);
        lines.Children().Append(webcam_.state);
        auto border = webcam_.part.Make(kWebcamPart, MakeIconAndText(webcam_.icon, lines), L"Webcam");
        border.MaxWidth(216);  // Longer names are cut short.
        webcam_.part.onHover = [this](bool) { StopHoverTimer(); };
        webcam_.part.onPress = [this]() { CloseFlyout(); };
        return border;
    }

    void ShowWebcam(const ThemeColors& colors) {
        const std::optional<WebcamStatus>& webcam = content_.webcam;
        webcam_.part.Border().Visibility(webcam ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        if (!webcam) {
            return;
        }
        webcam_.icon.Opacity(webcam->busy ? 0.43 : 1.0);
        webcam_.slash.Stroke(Brush(colors.foreground));
        webcam_.slash.Visibility(webcam->enabled ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        webcam_.name.Text(webcam->name);
        webcam_.state.Text(webcam->busy ? (webcam->enabled ? L"Turning off…" : L"Turning on…") : (webcam->enabled ? L"On" : L"Off"));
    }

    // ---- The volume flyout, shown above the output or input section while hovering it, like the
    // Windows 11 volume flyout: the device's name, a volume slider, and the device icon as a mute
    // button. It opens after a moment of hovering, and closes once the mouse moves away from it.

    void OnSectionHover(int part, bool entered) {
        if (flyout_ && part == flyoutPart_) {
            closer_.PartHovered(entered);
        }
        StopHoverTimer();
        hoveredPart_ = entered ? part : -1;
        if (!entered || (flyout_ && flyoutPart_ == part)) {
            return;
        }
        if (!hoverTimer_) {
            hoverTimer_ = wux::DispatcherTimer();
            hoverTimer_.Interval(std::chrono::milliseconds(400));
            hoverTimer_.Tick([this](auto const&, auto const&) {
                StopHoverTimer();
                if (hoveredPart_ == kOutputPart || hoveredPart_ == kInputPart) {
                    OpenFlyout(hoveredPart_);
                }
            });
        }
        hoverTimer_.Start();
    }

    void StopHoverTimer() {
        if (hoverTimer_) {
            hoverTimer_.Stop();
        }
    }

    void OpenFlyout(int part) {
        CloseFlyout();
        flyoutPart_ = part;
        bool output = part == kOutputPart;
        wuxc::StackPanel panel;
        panel.Width(288);

        // The device and its volume.
        wuxc::StackPanel master;
        flyoutTitle_ = MakeText(14, true);
        flyoutDetail_ = MakeText(12, false);
        flyoutDetail_.Opacity(0.75);
        flyoutDetail_.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        master.Children().Append(flyoutTitle_);
        master.Children().Append(flyoutDetail_);
        wuxc::Grid controls;
        controls.Margin(wux::ThicknessHelper::FromLengths(-4, 8, 0, 0));
        wuxc::ColumnDefinition iconColumn;
        iconColumn.Width(wux::GridLengthHelper::Auto());
        wuxc::ColumnDefinition valueColumn;
        valueColumn.Width(wux::GridLengthHelper::FromPixels(40));
        controls.ColumnDefinitions().Append(iconColumn);
        controls.ColumnDefinitions().Append(wuxc::ColumnDefinition());
        controls.ColumnDefinitions().Append(valueColumn);
        // The device icon, which doubles as the mute button.
        flyoutIcon_ = TaskbarXaml::MakeGlyph(kGlyphSpeakers, 18);
        auto mute = muteButton_.Make(output ? kOutputMutePart : kInputMutePart, flyoutIcon_, L"Mute");
        mute.Width(36);
        mute.Height(36);
        controls.Children().Append(mute);
        slider_ = MakeVolumeSlider();
        wuxc::Grid::SetColumn(slider_, 1);
        sliderChanged_ = slider_.ValueChanged(winrt::auto_revoke, [this](auto const&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
            flyoutValue_.Text(std::to_wstring((int)std::lround(args.NewValue())));
            if (!settingSlider_) {
                sliderMovedAt_ = GetTickCount();
                TaskbarXaml::PostToApp(kVolumeMessage, flyoutPart_, (LPARAM)std::lround(args.NewValue() * 10));
            }
        });
        // The slider handles the mouse itself; this hears it being let go anyway.
        releasedHandler_ = winrt::box_value(wux::Input::PointerEventHandler([this](auto const&, auto const&) {
            TaskbarXaml::PostToApp(kVolumeDoneMessage, flyoutPart_, 0);
        }));
        slider_.AddHandler(wux::UIElement::PointerReleasedEvent(), releasedHandler_, true);
        controls.Children().Append(slider_);
        flyoutValue_ = MakeText(14, false);
        flyoutValue_.HorizontalAlignment(wux::HorizontalAlignment::Right);
        flyoutValue_.VerticalAlignment(wux::VerticalAlignment::Center);
        wuxc::Grid::SetColumn(flyoutValue_, 2);
        controls.Children().Append(flyoutValue_);
        master.Children().Append(controls);
        // Scrolling over the device turns its volume, as over the section.
        wheelHandler_ = winrt::box_value(wux::Input::PointerEventHandler([this](auto const&, wux::Input::PointerRoutedEventArgs const& args) {
            wheelRemainder_ += args.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
            int notches = wheelRemainder_ / WHEEL_DELTA;
            wheelRemainder_ -= notches * WHEEL_DELTA;
            if (notches != 0) {
                TaskbarXaml::PostToApp(TaskbarXaml::kWheelMessage, flyoutPart_, notches);
            }
            args.Handled(true);
        }));
        master.AddHandler(wux::UIElement::PointerWheelChangedEvent(), wheelHandler_, true);
        flyoutMaster_ = master;
        panel.Children().Append(master);

        // The output's volume mixer, like Windows': each app's icon, which mutes it, and its volume,
        // with the gear for the Volume mixer settings.
        if (output) {
            panel.Children().Append(MakeSeparator());
            wuxc::Grid header;
            auto heading = MakeText(14, true);
            heading.Text(L"Volume mixer");
            heading.VerticalAlignment(wux::VerticalAlignment::Center);
            header.Children().Append(heading);
            auto gear = mixerSettings_.Make(kMixerSettingsPart, TaskbarXaml::MakeGlyph(kGlyphSettings, 16), L"Volume mixer settings");
            gear.Width(32);
            gear.Height(32);
            gear.HorizontalAlignment(wux::HorizontalAlignment::Right);
            mixerSettings_.onClick = [this]() { CloseFlyoutSoon(); };
            header.Children().Append(gear);
            panel.Children().Append(header);
            appsPanel_ = wuxc::StackPanel();
            wuxc::ScrollViewer scroller;
            scroller.MaxHeight(240);
            scroller.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
            scroller.Content(appsPanel_);
            panel.Children().Append(scroller);
            noApps_ = MakeText(12, false);
            noApps_.Opacity(0.75);
            noApps_.Text(L"No apps are using sound.");
            noApps_.Margin(wux::ThicknessHelper::FromLengths(0, 4, 0, 4));
            panel.Children().Append(noApps_);
        }

        panel.Children().Append(MakeSeparator());
        auto moreText = MakeText(14, false);
        moreText.Text(output ? L"More volume settings" : L"More sound settings");
        moreText.Margin(wux::ThicknessHelper::FromLengths(4, 6, 8, 6));
        auto more = moreSettings_.Make(kSoundSettingsPart, moreText, output ? L"More volume settings" : L"More sound settings");
        more.HorizontalAlignment(wux::HorizontalAlignment::Left);
        more.Margin(wux::ThicknessHelper::FromLengths(-6, 2, 0, -6));
        moreSettings_.onClick = [this]() { CloseFlyoutSoon(); };
        panel.Children().Append(more);

        flyout_ = wuxc::Flyout();
        flyout_.ShouldConstrainToRootBounds(false);  // Above the taskbar, outside its XAML.
        flyout_.Content(panel);
        flyoutClosed_ = flyout_.Closed(winrt::auto_revoke, [this](auto const&, auto const&) { ForgetFlyout(); });
        ShowFlyoutStatus();
        wuxc::Primitives::FlyoutShowOptions options;
        options.Placement(wuxc::Primitives::FlyoutPlacementMode::Top);
        options.ShowMode(wuxc::Primitives::FlyoutShowMode::TransientWithDismissOnPointerMoveAway);
        AudioSection& section = output ? output_ : input_;
        closer_.Start(flyout_, panel, [this]() { CloseFlyout(); });
        flyout_.ShowAt(section.part.Border(), options);
        if (output) {
            TaskbarXaml::PostToApp(kMixerMessage, 1, 0);  // The app sends the apps while it's open.
        }
    }

    static wuxc::Slider MakeVolumeSlider() {
        wuxc::Slider slider;
        slider.Minimum(0);
        slider.Maximum(100);
        slider.StepFrequency(1);
        slider.IsThumbToolTipEnabled(false);
        slider.VerticalAlignment(wux::VerticalAlignment::Center);
        slider.Margin(wux::ThicknessHelper::FromLengths(8, 0, 8, 0));
        return slider;
    }

    static wuxc::Border MakeSeparator() {
        wuxc::Border line;
        line.Height(1);
        line.Margin(wux::ThicknessHelper::FromLengths(-12, 10, -12, 6));
        line.Background(Brush(Color{0x30, 0x80, 0x80, 0x80}));
        return line;
    }

    // A settings link was pressed: the flyout goes as Settings opens.
    void CloseFlyoutSoon() {
        closer_.Stop();
        if (flyout_) {
            try {
                flyout_.Hide();  // Its Closed lets go of it, once the click is over.
            } catch (...) {
            }
        }
    }

    // While muted, a slider's fill is grey.
    void ShowMuted(wuxc::Slider const& slider, bool muted) {
        if (muted) {
            slider.Foreground(Brush(ThemeColorsFor(row_.ActualTheme() == wux::ElementTheme::Light).muted));
        } else {
            slider.ClearValue(wuxc::Control::ForegroundProperty());
        }
    }

    void ShowFlyoutStatus() {
        EDataFlow flow = *FlowOfPart(flyoutPart_);
        const std::optional<AudioStatus>& status = flow == eRender ? content_.output : content_.input;
        flyoutTitle_.Text(status ? status->device.name : L"No device");
        // Name and hardware, as on the Sound control panel's Playback tab.
        std::wstring description = status ? status->device.description : L"";
        flyoutDetail_.Text(description);
        flyoutDetail_.Visibility(description.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        flyoutIcon_.Glyph(DeviceGlyph(flow, status));
        muteButton_.Border().IsHitTestVisible(status.has_value());
        slider_.IsEnabled(status.has_value());
        ShowMuted(slider_, status && status->muted);
        // While the slider is being moved, it shows where it's moved to, not the volume Windows has
        // caught up with.
        if (status && GetTickCount() - sliderMovedAt_ > 500) {
            settingSlider_ = true;
            slider_.Value(status->Percent());
            settingSlider_ = false;
        }
        flyoutValue_.Text(std::to_wstring((int)std::lround(slider_.Value())));
        ShowMixer();
    }

    // ---- The volume mixer's rows

    struct AppRow {
        DWORD processId = 0;
        std::wstring name;
        wuxc::Grid grid{nullptr};
        TaskbarXaml::Part button;
        wuxc::Slider slider{nullptr};
        wuxc::Primitives::RangeBase::ValueChanged_revoker changed;
        bool setting = false;
        DWORD movedAt = 0;
    };

    // The app's icon (a button that mutes it), then its volume.
    std::unique_ptr<AppRow> MakeAppRow(const AppVolume& app) {
        auto row = std::make_unique<AppRow>();
        row->processId = app.processId;
        row->name = app.name;
        wux::UIElement icon{nullptr};
        if (app.icon) {
            wuxm::Imaging::WriteableBitmap bitmap(app.icon->size, app.icon->size);
            auto buffer = bitmap.PixelBuffer();
            memcpy(buffer.data(), app.icon->pixels.data(), std::min<size_t>(buffer.Capacity(), app.icon->pixels.size()));
            bitmap.Invalidate();
            wuxc::Image image;
            image.Source(bitmap);
            image.Width(20);
            image.Height(20);
            icon = image;
        } else {
            icon = TaskbarXaml::MakeGlyph(app.processId == 0 ? kGlyphSpeakers : kGlyphApp, 18);
        }
        auto button = row->button.Make(-1, icon, app.name.c_str());
        button.Width(36);
        button.Height(36);
        wuxc::ToolTipService::SetToolTip(button, winrt::box_value(winrt::hstring(app.name)));
        DWORD processId = app.processId;
        row->button.onClick = [processId]() { TaskbarXaml::PostToApp(kAppMuteMessage, processId, 0); };
        row->slider = MakeVolumeSlider();
        wuxc::Grid::SetColumn(row->slider, 1);
        AppRow* raw = row.get();
        row->changed = row->slider.ValueChanged(winrt::auto_revoke, [raw](auto const&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& args) {
            if (!raw->setting) {
                raw->movedAt = GetTickCount();
                TaskbarXaml::PostToApp(kAppVolumeMessage, raw->processId, (LPARAM)std::lround(args.NewValue() * 10));
            }
        });
        row->grid = wuxc::Grid();
        row->grid.Margin(wux::ThicknessHelper::FromLengths(-4, 0, 40, 0));  // Its slider lines up with the device's.
        wuxc::ColumnDefinition iconColumn;
        iconColumn.Width(wux::GridLengthHelper::Auto());
        row->grid.ColumnDefinitions().Append(iconColumn);
        row->grid.ColumnDefinitions().Append(wuxc::ColumnDefinition());
        row->grid.Children().Append(button);
        row->grid.Children().Append(row->slider);
        return row;
    }

    // The rows are made again only when the apps change, so a slider being moved stays.
    void ShowMixer() {
        if (!appsPanel_) {
            return;
        }
        const std::vector<AppVolume>& apps = content_.apps;
        bool same = rows_.size() == apps.size();
        for (size_t i = 0; same && i < apps.size(); i++) {
            same = rows_[i]->processId == apps[i].processId && rows_[i]->name == apps[i].name;
        }
        if (!same) {
            appsPanel_.Children().Clear();
            rows_.clear();
            for (const AppVolume& app : apps) {
                rows_.push_back(MakeAppRow(app));
                appsPanel_.Children().Append(rows_.back()->grid);
            }
        }
        noApps_.Visibility(apps.empty() ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        for (size_t i = 0; i < apps.size(); i++) {
            AppRow& row = *rows_[i];
            row.button.Border().Opacity(apps[i].muted ? 0.5 : 1.0);
            ShowMuted(row.slider, apps[i].muted);
            if (GetTickCount() - row.movedAt > 500) {
                row.setting = true;
                row.slider.Value(std::lround(apps[i].level * 100));
                row.setting = false;
            }
        }
    }

    void CloseFlyout() {
        if (!flyout_) {
            return;
        }
        auto flyout = flyout_;
        ForgetFlyout();
        try {
            flyout.Hide();
        } catch (...) {
        }
    }

    void ForgetFlyout() {
        if (flyoutPart_ == kOutputPart) {
            TaskbarXaml::PostToApp(kMixerMessage, 0, 0);
        }
        closer_.Stop();
        flyoutClosed_.revoke();
        sliderChanged_.revoke();
        if (slider_ && releasedHandler_) {
            slider_.RemoveHandler(wux::UIElement::PointerReleasedEvent(), releasedHandler_);
        }
        if (flyoutMaster_ && wheelHandler_) {
            flyoutMaster_.RemoveHandler(wux::UIElement::PointerWheelChangedEvent(), wheelHandler_);
        }
        releasedHandler_ = nullptr;
        wheelHandler_ = nullptr;
        muteButton_.Forget();
        mixerSettings_.Forget();
        moreSettings_.Forget();
        if (appsPanel_) {
            appsPanel_.Children().Clear();
        }
        rows_.clear();
        appsPanel_ = nullptr;
        noApps_ = nullptr;
        flyout_ = nullptr;
        flyoutMaster_ = nullptr;
        flyoutTitle_ = nullptr;
        flyoutDetail_ = nullptr;
        flyoutIcon_ = nullptr;
        slider_ = nullptr;
        flyoutValue_ = nullptr;
        flyoutPart_ = -1;
        wheelRemainder_ = 0;
        sliderMovedAt_ = 0;
    }

    WidgetContent content_;
    wuxc::StackPanel row_{nullptr};
    wux::FrameworkElement::ActualThemeChanged_revoker themeRevoker_;
    AudioSection output_;
    AudioSection input_;
    WebcamSection webcam_;
    wux::DispatcherTimer hoverTimer_{nullptr};
    int hoveredPart_ = -1;
    // The volume flyout, while it's open.
    wuxc::Flyout flyout_{nullptr};
    wuxc::Primitives::FlyoutBase::Closed_revoker flyoutClosed_;
    int flyoutPart_ = -1;
    wuxc::StackPanel flyoutMaster_{nullptr};
    wuxc::TextBlock flyoutTitle_{nullptr};
    wuxc::TextBlock flyoutDetail_{nullptr};
    wuxc::FontIcon flyoutIcon_{nullptr};
    TaskbarXaml::Part muteButton_;
    TaskbarXaml::Part mixerSettings_;
    TaskbarXaml::Part moreSettings_;
    TaskbarXaml::HoverCloser closer_;
    wuxc::Slider slider_{nullptr};
    wuxc::Primitives::RangeBase::ValueChanged_revoker sliderChanged_;
    // Handlers added with AddHandler, as added, to take off again.
    winrt::Windows::Foundation::IInspectable releasedHandler_{nullptr};
    winrt::Windows::Foundation::IInspectable wheelHandler_{nullptr};
    wuxc::TextBlock flyoutValue_{nullptr};
    bool settingSlider_ = false;
    DWORD sliderMovedAt_ = 0;
    int wheelRemainder_ = 0;
    wuxc::StackPanel appsPanel_{nullptr};
    wuxc::TextBlock noApps_{nullptr};
    std::vector<std::unique_ptr<AppRow>> rows_;
};

[[clang::no_destroy]] std::optional<AfView> g_view{std::in_place};  // Not destroyed: see TaskbarXaml::g_bridge.

// Puts the widget in the taskbar where the settings say (on the taskbar's thread).
void AttachWidget(HWND taskbar) {
    TaskbarXaml::g_host->Attach(taskbar, TaskbarXaml::ReadSlot(), &*g_view);
}

// Puts the widget in the taskbar if it isn't there (from the app's thread): Explorer said it made
// the taskbar, which the widget may have missed as Explorer started.
void AttachIfMissing() {
    if (HWND taskbar = OwnTaskbar()) {
        TaskbarXaml::RunOnWindowThread(taskbar, [taskbar]() {
            if (!TaskbarXaml::g_host->Element()) {
                AttachWidget(taskbar);
            }
        });
    }
}

// ---------------------------------------------------------------------------
// The app: follows the default devices and the webcam, updates the widget and runs the menu.

class AfApp {
public:
    bool Start() {
        LoadSettings();
        if (!messages_.Create(L"AudioFlipper.Mod.Messages")) {
            return false;
        }
        messages_.onTimer = [this](UINT_PTR timer) { OnTimer(timer); };
        messages_.onMessage = [this](UINT message, WPARAM wParam, LPARAM lParam) { OnMessage(message, wParam, lParam); };
        audio_.Listen(messages_.Handle(), kAudioChangedMessage);
        return StartWidget();
    }

    ~AfApp() {
        TaskbarXaml::g_appWindow = nullptr;
        // A webcam switch in progress stops waiting for pnputil. (One at a UAC prompt waits for the
        // answer: the prompt can't be cancelled.)
        SetEvent(stop_);
        workers_.WaitAll();
        CloseHandle(stop_);
    }

private:
    static constexpr UINT_PTR kVolumeRefreshTimer = 2;
    static constexpr UINT_PTR kDingTimer = 3;
    static constexpr UINT_PTR kMixerTimer = 4;  // Reads the apps again while the volume mixer is open.
    static constexpr UINT kWebcamChangedMessage = WM_APP + 1;
    static constexpr UINT kAudioChangedMessage = WM_APP + 2;
    static constexpr int kVolumeStepPercent = 2;  // Same step as the Windows volume flyout.

    struct WebcamRequest {
        HWND replyTo;
        HWND owner;  // For the UAC prompt.
        HANDLE stop;  // Set when the mod is turned off.
        std::wstring instanceId;
        std::wstring name;
        bool enable;
        WebcamChange result = WebcamChange::Failed;
    };

    bool StartWidget() {
        if (!owner_.Create(L"AudioFlipper.Mod.Owner") || !popup_.Create()) {
            return false;
        }
        ForgetDraggedWidget();
        owner_.onSettingChange = [this]() {
            RefreshShellSettings();
            // Windows' accent color may have changed.
            TaskbarXaml::g_bridge->Post([]() { ForgetAccentColors(); });
            SendContent();
        };
        owner_.onTaskbarCreated = []() { AttachIfMissing(); };
        owner_.onDeviceChange = [this](WPARAM event) {
            // Windows tells every top-level window when a device is added or removed, or turned on
            // or off (as Device Manager does).
            if (event == DBT_DEVNODES_CHANGED) {
                RefreshWebcam();
            }
        };
        content_.showOutput = settings_.showOutput;
        content_.showInput = settings_.showInput;
        RefreshStatus(false);
        RefreshWebcam();
        // The widget may be in the taskbar already, or will say when it is.
        TaskbarXaml::g_appWindow = messages_.Handle();
        SendContent();
        return true;
    }

    // Tells the widget what to show.
    void SendContent() {
        TaskbarXaml::g_bridge->Post([content = content_]() { g_view->Show(content); });
    }

    void SetSections(bool showOutput, bool showInput) {
        content_.showOutput = showOutput;
        content_.showInput = showInput;
        SendContent();
    }

    void OnWidgetClick(int part, TaskbarXaml::MouseButton button) {
        std::optional<EDataFlow> flow = FlowOfPart(part);
        if (part < kOutputPart || part > kMixerSettingsPart) {
            return;  // The volume mixer's apps, which say so themselves.
        }
        if (part == kSoundSettingsPart || part == kMixerSettingsPart) {
            if (button == TaskbarXaml::MouseButton::Left) {
                OpenWithShell(part == kMixerSettingsPart ? L"ms-settings:apps-volume" : L"ms-settings:sound");
            }
        } else if (button == TaskbarXaml::MouseButton::Right) {
            ShowMenu();
        } else if (button == TaskbarXaml::MouseButton::Middle || part == kOutputMutePart || part == kInputMutePart) {
            if (flow) {
                audio_.ToggleDefaultMute(*flow);
                RefreshStatus(false);
            }
        } else if (part == kWebcamPart) {
            ToggleWebcam();
        } else if (flow) {
            SwitchToNext(*flow);
        }
    }

    void OnTimer(UINT_PTR timer) {
        switch (timer) {
            case kVolumeRefreshTimer:
                KillTimer(messages_.Handle(), kVolumeRefreshTimer);
                volumeRefreshPending_ = false;
                RefreshStatus(false);
                break;
            case kDingTimer:
                Ding();
                break;
            case kMixerTimer:
                RefreshApps();
                break;
        }
    }

    // Sends the volume mixer the apps, when they or their volumes have changed.
    void RefreshApps() {
        std::vector<AppVolume> apps = audio_.AppVolumes();
        if (apps != content_.apps) {
            content_.apps = apps;
            SendContent();
        }
    }

    void LoadSettings() {
        settings_ = Settings();
        settings_.Load();
        // The app (imported from) can turn off both the output and input sections, since it has its
        // tray icon; the widget is the mod's only way in, so one stays shown.
        if (!settings_.showOutput && !settings_.showInput) {
            settings_.showOutput = true;
        }
    }

    void Save() {
        if (!settings_.Save()) {
            popup_.ShowMessage(L"Couldn't save settings", L"Windhawk couldn't store the mod's settings.");
        }
    }

    // Reads the default output and input with their volumes, updating the widget when anything
    // changed. The popup is for the output: it's shown when asked to, or when the output was changed
    // elsewhere, but only while the widget isn't showing the output already.
    void RefreshStatus(bool showPopup) {
        // A device that can't be read just now keeps its last status.
        bool outputFailed = false;
        bool inputFailed = false;
        std::optional<AudioStatus> output = audio_.DefaultStatus(eRender, &outputFailed);
        std::optional<AudioStatus> input = settings_.showInput ? audio_.DefaultStatus(eCapture, &inputFailed) : std::nullopt;
        if (outputFailed) {
            output = lastOutput_;
        }
        if (inputFailed) {
            input = lastInput_;
        }
        bool outputChanged = hasRefreshed_ && (output ? output->device.id : L"") != (lastOutput_ ? lastOutput_->device.id : L"");
        if (!hasRefreshed_ || output != lastOutput_ || input != lastInput_) {
            content_.output = output;
            content_.input = input;
            SendContent();
        }
        lastOutput_ = output;
        lastInput_ = input;
        hasRefreshed_ = true;
        if ((showPopup || outputChanged) && settings_.showPopup && !settings_.showOutput) {
            ShowStatusPopup(output);
        }
    }

    void ShowStatusPopup(const std::optional<AudioStatus>& output) {
        if (!output) {
            popup_.ShowMessage(L"No output device", L"Enable an output in Windows Sound settings.");
            return;
        }
        MessagePopup::Content content;
        content.title = output->device.name;
        content.level = output->Percent();
        content.levelMuted = output->muted;
        AudioStatus status = *output;
        content.icon = [status](gp::Graphics& graphics, const gp::RectF& bounds, const gp::Color& color) {
            DrawDeviceIcon(graphics, eRender, status, bounds, color);
        };
        popup_.Show(content, 1800);
    }

    // Slider drags and fast scrolling change the volume dozens of times a second, and Windows reports
    // each change; refresh the widget at a steady rate instead of after every one.
    void RefreshSoon() {
        if (!volumeRefreshPending_) {
            volumeRefreshPending_ = true;
            SetTimer(messages_.Handle(), kVolumeRefreshTimer, 30, nullptr);
        }
    }

    // The Windows "Default Beep" (the ding Windows' own volume slider makes), on the default output,
    // so the new volume can be heard.
    void Ding() {
        KillTimer(messages_.Handle(), kDingTimer);
        if (settings_.volumeDing) {
            MessageBeep(MB_OK);
        }
    }

    void ChangeVolume(EDataFlow flow, int points) {
        audio_.ChangeDefaultVolume(flow, points);
        RefreshSoon();
        if (flow == eRender && settings_.volumeDing) {
            SetTimer(messages_.Handle(), kDingTimer, 250, nullptr);  // Ding once scrolling pauses, not on every notch.
        }
    }

    // Makes the device after the current default (in name order, skipping devices left out of the
    // rotation) the default.
    void SwitchToNext(EDataFlow flow) {
        std::vector<AudioDevice> devices = audio_.ActiveDevices(flow);
        std::vector<AudioDevice> rotation;
        for (const AudioDevice& device : devices) {
            if (!settings_.IsExcluded(device.id)) {
                rotation.push_back(device);
            }
        }
        if (rotation.empty()) {
            rotation = devices;  // Everything is left out: rotate through all of them instead.
        }
        if (rotation.empty()) {
            popup_.ShowMessage(flow == eRender ? L"No output devices" : L"No input devices",
                               flow == eRender ? L"Enable an output in Windows Sound settings." : L"Enable a microphone in Windows Sound settings.");
            return;
        }
        std::wstring currentId = audio_.DefaultDeviceId(flow);
        size_t index = 0;
        for (size_t i = 0; i < rotation.size(); i++) {
            if (rotation[i].id == currentId) {
                index = i + 1;
            }
        }
        const AudioDevice& next = rotation[index % rotation.size()];
        if (next.id != currentId) {
            SwitchTo(next.id, flow);
        }
    }

    void SwitchTo(const std::wstring& id, EDataFlow flow) {
        if (FAILED(audio_.SetDefaultDevice(id, settings_.SwitchesCommunications(flow)))) {
            popup_.ShowMessage(flow == eRender ? L"Couldn't switch output" : L"Couldn't switch input", L"Windows didn't allow it.");
            return;
        }
        RefreshStatus(flow == eRender);
    }

    // The chosen webcam's name: the one given in AudioFlipper, or its own.
    std::wstring WebcamDisplayName() const {
        if (std::optional<std::wstring> nickname = settings_.WebcamNickname(settings_.webcamId)) {
            return *nickname;
        }
        return settings_.webcamName.empty() ? L"Webcam" : settings_.webcamName;
    }

    // Names the webcam in AudioFlipper. Windows has no way to rename a webcam, so this is
    // AudioFlipper's own name for it, shown on the widget and in the menu.
    void RenameWebcam(const Webcam& webcam) {
        std::optional<std::wstring> name = PromptForText(
            L"Rename webcam",
            L"Name to show for this webcam in AudioFlipper. Leave it empty to use its own name, \"" + webcam.name + L"\".",
            L"Rename", settings_.WebcamNickname(webcam.instanceId).value_or(webcam.name), true, true);
        if (!name) {
            return;
        }
        settings_.SetWebcamNickname(webcam.instanceId, *name == webcam.name ? L"" : *name);
        Save();
        RefreshWebcam();
    }

    // Shows the chosen webcam's state on the widget, or hides the webcam section.
    void RefreshWebcam() {
        std::optional<WebcamStatus> status;
        if (settings_.showWebcam && !settings_.webcamId.empty()) {
            if (std::optional<bool> enabled = WebcamEnabled(settings_.webcamId)) {
                status = WebcamStatus{WebcamDisplayName(), *enabled, webcamBusy_};
            }
        }
        if (status != content_.webcam) {
            content_.webcam = status;
            SendContent();
        }
    }

    static DWORD WINAPI WebcamThread(void* parameter) {
        auto request = (WebcamRequest*)parameter;
        HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        request->result = SetWebcamEnabled(request->instanceId, request->enable, request->owner, request->stop);
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
        if (!PostMessageW(request->replyTo, kWebcamChangedMessage, 0, (LPARAM)request)) {
            delete request;
        }
        return 0;
    }

    void ToggleWebcam() {
        if (webcamBusy_ || settings_.webcamId.empty()) {
            return;
        }
        std::wstring name = WebcamDisplayName();
        std::optional<bool> enabled = WebcamEnabled(settings_.webcamId);
        if (!enabled) {
            popup_.ShowMessage(L"Webcam not found", name + L" isn't connected.");
            return;
        }
        webcamBusy_ = true;
        RefreshWebcam();
        // Windows only shows a UAC prompt up front for the foreground app (otherwise it just flashes
        // on the taskbar), and the widget never takes focus by itself.
        SetForegroundWindow(owner_.Handle());
        auto request = new WebcamRequest{messages_.Handle(), owner_.Handle(), stop_, settings_.webcamId, name, !*enabled};
        if (!workers_.Start(WebcamThread, request)) {
            delete request;
            webcamBusy_ = false;
            RefreshWebcam();
        }
    }

    void OnMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case TaskbarXaml::kAttachedMessage:
                SendContent();
                return;
            case TaskbarXaml::kClickMessage:
                OnWidgetClick((int)wParam, (TaskbarXaml::MouseButton)lParam);
                return;
            case TaskbarXaml::kWheelMessage:
                if (std::optional<EDataFlow> flow = FlowOfPart((int)wParam)) {
                    ChangeVolume(*flow, (int)lParam * kVolumeStepPercent);
                }
                return;
            case TaskbarXaml::kMenuCommandMessage:
                if (auto menu = std::move(menu_)) {
                    menu->Run((int)wParam);
                }
                return;
            case kVolumeMessage:
                if (std::optional<EDataFlow> flow = FlowOfPart((int)wParam)) {
                    audio_.SetDefaultVolume(*flow, std::clamp((float)lParam / 1000.0f, 0.0f, 1.0f));
                    RefreshSoon();
                }
                return;
            case kMixerMessage:
                if (wParam) {
                    RefreshApps();
                    SetTimer(messages_.Handle(), kMixerTimer, 500, nullptr);
                } else {
                    KillTimer(messages_.Handle(), kMixerTimer);
                }
                return;
            case kAppVolumeMessage:
                audio_.SetAppVolume((DWORD)wParam, std::clamp((float)lParam / 1000.0f, 0.0f, 1.0f));
                return;
            case kAppMuteMessage:
                audio_.ToggleAppMute((DWORD)wParam);
                RefreshApps();
                return;
            case kVolumeDoneMessage:
                if (FlowOfPart((int)wParam) == eRender) {
                    Ding();
                }
                return;
        }
        if (message == kAudioChangedMessage) {
            audio_.ChangeHandled();
            RefreshSoon();
            return;
        }
        if (message != kWebcamChangedMessage) {
            return;
        }
        std::unique_ptr<WebcamRequest> request((WebcamRequest*)lParam);
        webcamBusy_ = false;
        RefreshWebcam();
        std::wstring change = (request->enable ? L"on " : L"off ") + request->name;
        if (request->result == WebcamChange::RestartNeeded) {
            popup_.ShowMessage(L"Restart to finish", L"Windows will finish turning " + change + L" after a restart.");
        } else if (request->result == WebcamChange::Failed) {
            popup_.ShowMessage(L"Couldn't turn " + change, L"Try it from Camera settings in the Webcam menu.");
        }
    }

    // The Output or Input submenu: showing it on the widget, the devices and their settings.
    void AddAudioMenu(Menu& menu, EDataFlow flow) {
        bool isOutput = flow == eRender;
        Menu& submenu = menu.AddSubmenu(isOutput ? L"Output" : L"Input");
        bool shown = isOutput ? settings_.showOutput : settings_.showInput;
        bool otherShown = isOutput ? settings_.showInput : settings_.showOutput;
        // The widget is the mod's only way in, so one of the two stays shown.
        submenu.Add(L"Show on taskbar", [this, isOutput, shown]() {
            (isOutput ? settings_.showOutput : settings_.showInput) = !shown;
            Save();
            RefreshStatus(false);  // Read the devices first so a newly shown section appears with its device straight away.
            SetSections(settings_.showOutput, settings_.showInput);
        }, shown, !shown || otherShown);
        submenu.AddSeparator();

        std::vector<AudioDevice> devices = audio_.ActiveDevices(flow);
        std::wstring currentId = audio_.DefaultDeviceId(flow);
        submenu.AddHeading(isOutput ? L"Output device" : L"Input device");
        if (devices.empty()) {
            submenu.AddHeading(isOutput ? L"No active outputs" : L"No active inputs");
        }
        for (const AudioDevice& device : devices) {
            std::wstring id = device.id;
            std::wstring label = settings_.IsExcluded(id) ? device.fullName + L"  (not in rotation)" : device.fullName;
            submenu.Add(label, [this, id, flow]() { SwitchTo(id, flow); }, id == currentId);
        }

        submenu.AddSeparator();
        Menu& rotation = submenu.AddSubmenu(L"Include in click rotation", !devices.empty());
        for (const AudioDevice& device : devices) {
            std::wstring id = device.id;
            bool included = !settings_.IsExcluded(id);
            rotation.Add(device.fullName, [this, id, included]() {
                settings_.SetExcluded(id, included);
                Save();
            }, included);
        }
        bool communications = settings_.SwitchesCommunications(flow);
        submenu.Add(L"Also switch communications device", [this, isOutput, communications]() {
            (isOutput ? settings_.switchCommunications : settings_.switchInputCommunications) = !communications;
            Save();
        }, communications);
        if (isOutput) {
            submenu.Add(L"Ding when changing volume", [this]() {
                settings_.volumeDing = !settings_.volumeDing;
                Save();
            }, settings_.volumeDing);
            submenu.Add(L"Show popup on change", [this]() {
                settings_.showPopup = !settings_.showPopup;
                Save();
            }, settings_.showPopup);
        }

        submenu.AddSeparator();
        // The classic Sound control panel (Playback or Recording tab), where a device can be renamed via
        // right-click > Properties.
        submenu.Add(L"Rename devices…", [isOutput]() {
            Launch(SystemFolder() + L"\\control.exe", isOutput ? L"mmsys.cpl,,0" : L"mmsys.cpl,,1");
        });
        submenu.Add(L"Sound settings…", []() { OpenWithShell(L"ms-settings:sound"); });
    }

    void AddWebcamMenu(Menu& menu) {
        std::vector<Webcam> webcams = GetWebcams();
        Menu& submenu = menu.AddSubmenu(L"Webcam");
        submenu.Add(L"Show on taskbar", [this, webcams]() {
            settings_.showWebcam = !settings_.showWebcam;
            if (settings_.showWebcam && settings_.webcamId.empty() && !webcams.empty()) {
                settings_.webcamId = webcams[0].instanceId;
                settings_.webcamName = webcams[0].name;
            }
            Save();
            RefreshWebcam();
        }, settings_.showWebcam);
        submenu.AddSeparator();
        submenu.AddHeading(L"Webcam to control");
        if (webcams.empty()) {
            submenu.AddHeading(L"No webcams found");
        }
        for (const Webcam& webcam : webcams) {
            Webcam choice = webcam;
            std::optional<std::wstring> nickname = settings_.WebcamNickname(webcam.instanceId);
            std::wstring label = nickname ? *nickname + L" (" + webcam.name + L")" : webcam.name;
            submenu.Add(webcam.enabled ? label : label + L"  (off)", [this, choice]() {
                settings_.webcamId = choice.instanceId;
                settings_.webcamName = choice.name;
                settings_.showWebcam = true;
                Save();
                RefreshWebcam();
            }, webcam.instanceId == settings_.webcamId);
        }
        submenu.AddSeparator();
        for (const Webcam& webcam : webcams) {
            if (webcam.instanceId == settings_.webcamId) {
                Webcam selected = webcam;
                submenu.Add((webcam.enabled ? L"Turn off " : L"Turn on ") + WebcamDisplayName(), [this]() { ToggleWebcam(); }, false,
                            !webcamBusy_);
                submenu.Add(L"Rename…", [this, selected]() { RenameWebcam(selected); });
            }
        }
        submenu.Add(L"Camera settings…", []() { OpenWithShell(L"ms-settings:camera"); });
    }

    void ShowMenu() {
        menu_ = std::make_unique<Menu>();
        Menu& menu = *menu_;
        AddAudioMenu(menu, eRender);
        AddAudioMenu(menu, eCapture);
        AddWebcamMenu(menu);
        TaskbarXaml::g_bridge->Post([entries = menu.Entries()]() { TaskbarXaml::ShowMenu(entries); });
    }

    Settings settings_;
    AudioDevices audio_;
    MessageWindow messages_;
    OwnerWindow owner_;
    MessagePopup popup_;
    std::unique_ptr<Menu> menu_;  // The widget's menu, while it may be open.
    WidgetContent content_;       // What the widget shows.
    WorkerThreads workers_;
    HANDLE stop_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);  // Set when the mod is turned off.
    std::optional<AudioStatus> lastOutput_;
    std::optional<AudioStatus> lastInput_;
    bool hasRefreshed_ = false;
    bool volumeRefreshPending_ = false;
    bool webcamBusy_ = false;
};

UiThread g_uiThread;
std::atomic<bool> g_unloading{false};

// Explorer has made the taskbar again (after it crashed, say): the widget goes in the new one.
void OnTaskbarStarted(HWND taskbar) {
    if (!g_unloading) {
        TaskbarXaml::g_host->Forget();
        AttachWidget(taskbar);
    }
}

void AttachToTaskbar() {
    if (HWND taskbar = OwnTaskbar()) {
        TaskbarXaml::RunOnWindowThread(taskbar, [taskbar]() { AttachWidget(taskbar); });
    }
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    if (!InTaskbarProcess()) {
        return FALSE;
    }
    // Windows 10's taskbar isn't XAML, and hasn't these.
    if (!TaskbarXaml::HookTaskbarDll()) {
        Wh_Log(L"Couldn't find the taskbar's functions");
        return FALSE;
    }
    TaskbarXaml::g_onTaskbarStarted = OnTaskbarStarted;
    return g_uiThread.Start([]() -> std::shared_ptr<void> {
        auto app = std::make_shared<AfApp>();
        if (!app->Start()) {
            Wh_Log(L"Couldn't start");
            return nullptr;
        }
        return app;
    });
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    AttachToTaskbar();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    AttachToTaskbar();  // In the position chosen.
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    g_uiThread.Stop();
    TaskbarXaml::g_bridge->Drain();
    if (HWND taskbar = OwnTaskbar()) {
        // Lets go of the widget's XAML on the taskbar's thread. Without a taskbar it's kept, as it
        // can't be let go of anywhere else (see TaskbarXaml::g_bridge).
        TaskbarXaml::RunOnWindowThread(taskbar, []() {
            TaskbarXaml::g_host->Detach();
            TaskbarXaml::g_menuClicks.reset();
            TaskbarXaml::g_host.reset();
            g_view.reset();
            TaskbarXaml::g_bridge.reset();
        });
    }
}
