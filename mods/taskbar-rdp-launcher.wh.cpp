// ==WindhawkMod==
// @id              taskbar-rdp-launcher
// @name            Taskbar RDP Launcher
// @description     A taskbar widget that starts Remote Desktop full screen on the number of displays you pick
// @version         1.3.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -loleaut32 -luuid -lcomdlg32 -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar RDP Launcher

![The widget on the taskbar, with a computer and buttons for 1, 2 and 3 displays](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-rdp-launcher/screenshot-1.3.png)

A taskbar widget, in the style of the Windows weather widget, that starts Remote Desktop
full screen on as many of your displays as you want. Instead of editing `use multimon` and
`selectedmonitors` in an .rdp file (after looking up the display numbers with `mstsc /l`), click a
number.

- **Number buttons**: click **1**, **2** or **3** to connect full screen on that many displays.
  There's one button for each display you have.
- **Hover a number** to see a map of your displays, laid out the way they are on your desk. The
  ones that button uses are highlighted, and the session's main display (where the remote taskbar
  goes) has a bar along its bottom. **Click a display to add it or take it off.**
- **The computer** shows its address, or the name you gave it over its address. Click it (or scroll
  over it) to go to the next of your computers.
- **Right-click** for the menu: add computers (by name, from a saved .rdp file, or from Remote
  Desktop Connection's recent list), rename or remove them, and open their Remote Desktop settings.
- **Position** in the mod's settings puts the widget left of the notification area, of the Wi-Fi,
  volume and battery icons, or of the clock, where the taskbar makes room for it. It can also go
  at the left end of the taskbar, over it: that's free when the taskbar's icons are centered and
  the Widgets button is off.

Out of the box, **1** is your main display, **2** adds the display nearest to it, and so on up to
all of them. The buttons are remembered for each arrangement of displays, so a laptop that's
sometimes docked keeps separate buttons for the desk and for the road.

## How it connects

- Computers added by name connect with `mstsc /v:<computer>`, like typing the computer into Remote
  Desktop Connection. Since the April 2026 Windows update, Remote Desktop shows a security warning
  every time an .rdp file is opened, but not for computers typed in, so this avoids it.

  Remote Desktop Connection takes the display options for these from its own settings file,
  `Default.rdp` in Documents. So just before the session starts, the mod sets `screen mode id`,
  `use multimon`, `span monitors` and `selectedmonitors` there (creating the file, hidden, if there
  isn't one), and as soon as the session ends, it puts back what they were. It notes what they were
  in its own storage first: if the mod is turned off during a session, they're put back right away,
  and if Explorer restarts during one, they're put back when the session ends. A setting you change
  in Remote Desktop Connection in the meantime is left as you set it.
- Saved .rdp files are never changed: the mod opens a copy with the display options set (and gets
  the warning, as any .rdp file does), and deletes the copy when the session ends. If the file is
  signed and its signature covers the display options, the copy goes without the signature, so
  Remote Desktop shows it as from an unknown publisher.
- Remote Desktop numbers displays its own way (not as Windows Settings does), so the first time you
  connect on some of your displays, the mod reads Remote Desktop's list (`mstsc /l`, whose box may
  flash) and remembers it until your displays change. **Read display numbers again** in the menu
  asks again.

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

The computers, their names and the display buttons are kept in the mod's storage, which Windhawk
removes along with the mod. The first time it runs, the mod takes on the computers and display
buttons of the RDPLauncher app, if you have it (from `%APPDATA%\RDPLauncher\settings.txt`, which it
doesn't change). Use the app or the mod, not both: each shows its own widget.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: tray
  $name: Position
  $description: Where on the taskbar the widget goes. At the left end, it's over the taskbar, which is free when its icons are centered and the Widgets button is off. The Taskbar Snipper and Taskbar Audio Flipper widgets in the same position sit beside it.
  $options:
  - tray: Left of the notification area
  - systemIcons: Left of the Wi-Fi, volume and battery icons
  - clock: Left of the clock
  - left: At the left end of the taskbar
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <commdlg.h>
#include <tlhelp32.h>
#include <uiautomation.h>
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
#include <winrt/Windows.UI.Xaml.Media.h>

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
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Where the widget goes among the Taskbar Snipper, Taskbar Audio Flipper and Taskbar RDP Launcher
// widgets in the same position on the taskbar, from the left, until it's moved from its menu.
constexpr int kWidgetOrder = 2;

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

std::wstring ToLower(std::wstring text) {
    for (wchar_t& c : text) {
        c = (wchar_t)towlower(c);
    }
    return text;
}

bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

bool StartsWithIgnoreCase(const std::wstring& text, const std::wstring& prefix) {
    return text.size() >= prefix.size() &&
           _wcsnicmp(text.c_str(), prefix.c_str(), prefix.size()) == 0;
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

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring DirectoryOf(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}

std::wstring FileNameOf(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

std::wstring FileNameWithoutExtension(const std::wstring& path) {
    std::wstring name = FileNameOf(path);
    size_t dot = name.find_last_of(L'.');
    return dot == std::wstring::npos || dot == 0 ? name : name.substr(0, dot);
}

void CreateDirectories(const std::wstring& path) {
    if (!path.empty()) {
        SHCreateDirectoryExW(nullptr, path.c_str(), nullptr);
    }
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

// Writes a file in one step: the new content goes to a file next to it, which then takes its place,
// so a failed write never leaves it empty or cut short. A file being replaced keeps its attributes
// (Default.rdp is hidden); a read-only one stays as it was, and writing it fails.
bool WriteFileBytes(const std::wstring& path, const std::string& bytes) {
    CreateDirectories(DirectoryOf(path));
    std::wstring temporary = path + L".new";
    HANDLE file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    bool ok = bytes.empty() || (WriteFile(file, bytes.data(), (DWORD)bytes.size(), &written, nullptr) && written == bytes.size());
    CloseHandle(file);
    if (ok) {
        ok = FileExists(path) ? ReplaceFileW(path.c_str(), temporary.c_str(), nullptr, REPLACEFILE_IGNORE_MERGE_ERRORS, nullptr, nullptr)
                              : MoveFileExW(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING);
    }
    if (!ok) {
        DeleteFileW(temporary.c_str());
    }
    return ok;
}

// Lines of a text file in UTF-8 or UTF-16 (with a byte order mark), as .NET reads them: .rdp files,
// and the app's settings.txt to import.
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

// Writes lines as UTF-16 with a byte order mark, as Remote Desktop Connection writes .rdp files.
bool WriteTextLines(const std::wstring& path, const std::vector<std::wstring>& lines) {
    std::wstring text;
    for (const std::wstring& line : lines) {
        text += line;
        text += L"\r\n";
    }
    std::string bytes("\xFF\xFE", 2);
    bytes.append((const char*)text.data(), text.size() * sizeof(wchar_t));
    return WriteFileBytes(path, bytes);
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

bool ParseInt(const std::wstring& value, int& result) {
    if (value.empty()) {
        return false;
    }
    wchar_t* end = nullptr;
    long parsed = wcstol(value.c_str(), &end, 10);
    if (!end || *end != L'\0') {
        return false;
    }
    result = (int)parsed;
    return true;
}

// ---------------------------------------------------------------------------
// The mod's storage. Windhawk keeps one for the whole PC (and removes it with the mod), while the
// mod runs separately for each signed-in Windows user, so each user's values and files are kept
// apart under their SID.

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
    std::vector<WCHAR> buffer(64 * 1024);  // The computers and display buttons take a few KB.
    size_t length = Wh_GetStringValue(ValueName(name).c_str(), buffer.data(), buffer.size());
    return std::wstring(buffer.data(), std::min(length, buffer.size()));
}

bool SetString(const wchar_t* name, const std::wstring& value) {
    return Wh_SetStringValue(ValueName(name).c_str(), value.c_str());
}

void Delete(const wchar_t* name) {
    Wh_DeleteValue(ValueName(name).c_str());
}

// The user's folder in the mod's storage, for files.
std::wstring Folder() {
    WCHAR path[MAX_PATH];
    size_t length = Wh_GetModStoragePath(path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return {};
    }
    return std::wstring(path, length) + L"\\" + UserSid();
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

// Starts a program. Returns its process handle (to close) in *process, and its ID in *processId,
// when asked for.
bool Launch(const std::wstring& exe, const std::wstring& arguments, HANDLE* process = nullptr, DWORD* processId = nullptr) {
    std::wstring commandLine = L"\"" + exe + L"\"" + (arguments.empty() ? L"" : L" " + arguments);
    std::vector<wchar_t> commandBuffer(commandLine.begin(), commandLine.end());
    commandBuffer.push_back(L'\0');
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION info{};
    if (!CreateProcessW(exe.c_str(), commandBuffer.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &info)) {
        return false;
    }
    CloseHandle(info.hThread);
    if (processId) {
        *processId = info.dwProcessId;
    }
    if (process) {
        *process = info.hProcess;
    } else {
        CloseHandle(info.hProcess);
    }
    return true;
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

// One line of text, vertically centered in the rectangle, cut short with "…".
void DrawTextLine(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, const gp::Color& color,
                  const gp::RectF& rect) {
    gp::StringFormat format(gp::StringFormatFlagsNoWrap);
    format.SetTrimming(gp::StringTrimmingEllipsisCharacter);
    format.SetLineAlignment(gp::StringAlignmentCenter);
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
    gp::Color background, foreground, secondary, border;

    static Palette Current() {
        if (AppsUseLightTheme()) {
            return {Argb(255, 243, 243, 243), Argb(255, 27, 27, 27), Argb(255, 96, 96, 96), Argb(255, 213, 213, 213)};
        }
        return {Argb(255, 32, 32, 32), Argb(255, 255, 255, 255), Argb(255, 197, 197, 197), Argb(255, 58, 58, 58)};
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

// The small popup in the bottom corner with a warning sign, a title and a line or two of detail,
// which hides itself after a moment.
class MessagePopup : public PopupWindow {
public:
    bool Create() { return CreatePopup(L"WidgetMessagePopup"); }

    void ShowMessage(const std::wstring& title, const std::wstring& detail) {
        title_ = title;
        detail_ = detail;
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
        SetTimer(hwnd_, 1, 4000, nullptr);
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
        DrawGlyph(graphics, kGlyphWarning, gp::RectF((float)Px(18), (float)Px(28), (float)Px(28), (float)Px(28)), colors.foreground);
        float textLeft = (float)Px(66);
        float textWidth = width - textLeft - Px(18);
        DrawTextLine(graphics, title_, *titleFont_, colors.foreground, gp::RectF(textLeft, (float)Px(12), textWidth, (float)Px(24)));
        DrawTextBlock(graphics, detail_, *detailFont_, colors.secondary, gp::RectF(textLeft, (float)Px(38), textWidth, (float)Px(36)));
    }

    std::wstring title_;
    std::wstring detail_;
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
// Windows setting or the displays change, which a window for messages only doesn't.
class OwnerWindow {
public:
    std::function<void()> onSettingChange;
    std::function<void()> onDisplayChange;
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
        } else if (message == WM_DISPLAYCHANGE && onDisplayChange) {
            onDisplayChange();
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
    enum class Kind { Item, Separator, Submenu };
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
[[clang::no_destroy]] std::vector<wuxc::MenuFlyoutItem::Click_revoker> g_menuClicks;
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
    g_menuClicks.clear();
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
                g_menuClicks.push_back(
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
            g_menuClicks.push_back(item.Click(winrt::auto_revoke, [chosen, command](auto const&, auto const&) { *chosen = command; }));
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

// Worker threads (reading Remote Desktop's display numbers) whose results come back to the UI
// thread as messages. Finished ones are closed as new ones start.
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
// RDP Launcher: displays, Remote Desktop's display IDs, .rdp files, computers and settings.

namespace {

// ---------------------------------------------------------------------------
// Displays

// One of this PC's displays, as Windows arranges them (Settings > System > Display).
struct LocalDisplay {
    RECT bounds{};  // Physical pixels, the same coordinates "mstsc /l" lists.
    bool primary = false;
    std::wstring deviceName;  // \\.\DISPLAY1
    // Place in Windows' list of display outputs (\\.\DISPLAY1 is 0), counting unused ones. Remote
    // Desktop's display IDs seem to follow it, so it's the guess when "mstsc /l" can't be read.
    int outputIndex = 0;
    std::wstring name = L"Display";  // The monitor's name, like "DELL U2720Q".

    int Width() const { return bounds.right - bounds.left; }
    int Height() const { return bounds.bottom - bounds.top; }

    // Identifies the display by where it is, which is what matters to Remote Desktop.
    std::wstring Key() const {
        return std::to_wstring(Width()) + L"x" + std::to_wstring(Height()) + L"@" + std::to_wstring(bounds.left) + L"," +
               std::to_wstring(bounds.top);
    }

    std::wstring Resolution() const { return std::to_wstring(Width()) + L" \u00D7 " + std::to_wstring(Height()); }
};

using Displays = std::vector<LocalDisplay>;
using Choice = std::vector<std::wstring>;  // Display keys.

BOOL CALLBACK AddDisplay(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    auto displays = (Displays*)data;
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(monitor, &info)) {
        LocalDisplay display;
        display.bounds = info.rcMonitor;
        display.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
        display.deviceName = info.szDevice;
        displays->push_back(display);
    }
    return TRUE;
}

// The displays left to right (then top to bottom), in physical pixels even where displays use
// different scaling, so they line up with what Remote Desktop sees.
Displays GetDisplays() {
    std::map<std::wstring, int> outputIndexes;
    DISPLAY_DEVICEW device{sizeof(device)};
    for (DWORD i = 0; i < 64 && EnumDisplayDevicesW(nullptr, i, &device, 0); i++) {
        outputIndexes[ToLower(device.DeviceName)] = (int)i;
        device.cb = sizeof(device);
    }

    // Explorer (and the app) are per-monitor DPI aware, so these are physical pixels.
    Displays displays;
    EnumDisplayMonitors(nullptr, nullptr, AddDisplay, (LPARAM)&displays);
    for (size_t i = 0; i < displays.size(); i++) {
        auto found = outputIndexes.find(ToLower(displays[i].deviceName));
        displays[i].outputIndex = found != outputIndexes.end() ? found->second : (int)i;
    }
    std::sort(displays.begin(), displays.end(), [](const LocalDisplay& a, const LocalDisplay& b) {
        return a.bounds.left != b.bounds.left ? a.bounds.left < b.bounds.left : a.bounds.top < b.bounds.top;
    });
    return displays;
}

// Monitor names ("DELL U2720Q", "Built-in display") from the display configuration, the names
// Settings > System > Display shows.
void LoadDisplayNames(Displays& displays) {
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    LONG result = ERROR_INSUFFICIENT_BUFFER;
    // The configuration can change between asking for the sizes and reading it; try again then.
    for (int attempt = 0; attempt < 3 && result == ERROR_INSUFFICIENT_BUFFER; attempt++) {
        UINT32 pathCount = 0;
        UINT32 modeCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS) {
            return;
        }
        paths.resize(pathCount);
        modes.resize(modeCount);
        result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);
        paths.resize(pathCount);
    }
    if (result != ERROR_SUCCESS) {
        return;
    }
    std::map<std::wstring, std::wstring> names;
    for (const DISPLAYCONFIG_PATH_INFO& path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = path.sourceInfo.adapterId;
        source.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS) {
            continue;
        }
        DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
        target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target.header.size = sizeof(target);
        target.header.adapterId = path.targetInfo.adapterId;
        target.header.id = path.targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) {
            continue;
        }
        std::wstring name = Trim(target.monitorFriendlyDeviceName);
        if (name.empty() && (target.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                             target.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                             target.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED)) {
            name = L"Built-in display";
        }
        // A duplicated (mirrored) desktop has several monitors; keep the first name.
        std::wstring gdiName = ToLower(source.viewGdiDeviceName);
        if (!name.empty() && !names.count(gdiName)) {
            names[gdiName] = name;
        }
    }
    for (LocalDisplay& display : displays) {
        auto found = names.find(ToLower(display.deviceName));
        if (found != names.end()) {
            display.name = found->second;
        }
    }
}

std::wstring KeyWithPrimary(const LocalDisplay& display) {
    return (display.primary ? L"*" : L"") + display.Key();
}

// Identifies an arrangement of displays, so display buttons can be remembered for each one.
std::wstring SetupKey(const Displays& displays) {
    std::vector<std::wstring> keys;
    for (const LocalDisplay& display : displays) {
        keys.push_back(KeyWithPrimary(display));
    }
    std::sort(keys.begin(), keys.end());
    return Join(keys, L";");
}

// Identifies the arrangement and which outputs the displays are on: Remote Desktop's display IDs
// stay the same while this does, so they're remembered for it.
std::wstring OutputsKey(const Displays& displays) {
    std::vector<std::wstring> keys;
    for (const LocalDisplay& display : displays) {
        keys.push_back(KeyWithPrimary(display) + L"=" + display.deviceName);
    }
    std::sort(keys.begin(), keys.end());
    return Join(keys, L";");
}

bool Contains(const Choice& choice, const std::wstring& key) {
    return std::find(choice.begin(), choice.end(), key) != choice.end();
}

// One button per number of displays: 1 is the main display, 2 adds the display nearest to it,
// and so on up to all of them.
std::vector<Choice> DefaultChoices(const Displays& displays) {
    if (displays.empty()) {
        return {};
    }
    const LocalDisplay* primary = &displays[0];
    for (const LocalDisplay& display : displays) {
        if (display.primary) {
            primary = &display;
        }
    }
    auto center = [](const LocalDisplay& d) {
        return std::pair<double, double>((d.bounds.left + d.bounds.right) / 2.0, (d.bounds.top + d.bounds.bottom) / 2.0);
    };
    auto [primaryX, primaryY] = center(*primary);
    std::vector<const LocalDisplay*> nearestFirst;
    for (const LocalDisplay& display : displays) {
        nearestFirst.push_back(&display);
    }
    std::stable_sort(nearestFirst.begin(), nearestFirst.end(), [&](const LocalDisplay* a, const LocalDisplay* b) {
        if ((a == primary) != (b == primary)) {
            return a == primary;
        }
        auto [ax, ay] = center(*a);
        auto [bx, by] = center(*b);
        double da = std::hypot(ax - primaryX, ay - primaryY);
        double db = std::hypot(bx - primaryX, by - primaryY);
        if (da != db) {
            return da < db;
        }
        return a->bounds.left != b->bounds.left ? a->bounds.left < b->bounds.left : a->bounds.top < b->bounds.top;
    });
    std::vector<Choice> choices;
    for (size_t count = 1; count <= nearestFirst.size(); count++) {
        Choice choice;
        for (size_t i = 0; i < count; i++) {
            choice.push_back(nearestFirst[i]->Key());
        }
        choices.push_back(choice);
    }
    return choices;
}

// The chosen displays in the order Remote Desktop gets them. The first becomes the main display
// (with the taskbar) in the session: this PC's main display if it's chosen, otherwise the leftmost
// chosen one.
std::vector<const LocalDisplay*> InSessionOrder(const Displays& displays, const Choice& chosen) {
    std::vector<const LocalDisplay*> ordered;
    for (const LocalDisplay& display : displays) {
        if (Contains(chosen, display.Key())) {
            ordered.push_back(&display);
        }
    }
    std::stable_sort(ordered.begin(), ordered.end(), [](const LocalDisplay* a, const LocalDisplay* b) {
        if (a->primary != b->primary) {
            return a->primary;
        }
        return a->bounds.left != b->bounds.left ? a->bounds.left < b->bounds.left : a->bounds.top < b->bounds.top;
    });
    return ordered;
}

// Saved buttons for this arrangement of displays, if they all still fit it; otherwise the defaults.
std::vector<Choice> ChoicesForSetup(const Displays& displays, const std::vector<Choice>* saved) {
    if (saved && !saved->empty()) {
        bool fits = std::all_of(saved->begin(), saved->end(), [&](const Choice& choice) {
            return !choice.empty() && std::all_of(choice.begin(), choice.end(), [&](const std::wstring& key) {
                return std::any_of(displays.begin(), displays.end(), [&](const LocalDisplay& d) { return d.Key() == key; });
            });
        });
        if (fits) {
            return *saved;
        }
    }
    return DefaultChoices(displays);
}

std::wstring DescribeCount(size_t count) {
    return count == 1 ? L"1 display" : std::to_wstring(count) + L" displays";
}

// ---------------------------------------------------------------------------
// Remote Desktop's display IDs (for the .rdp "selectedmonitors" setting). They aren't the numbers
// Windows Settings shows, so they're read from "mstsc /l", and remembered until the displays change.

using DisplayIds = std::map<std::wstring, int>;  // By display key.

std::wstring MstscPath() {
    return SystemFolder() + L"\\mstsc.exe";
}

bool CoversAll(const DisplayIds& ids, const Displays& displays) {
    return std::all_of(displays.begin(), displays.end(), [&](const LocalDisplay& d) { return ids.count(d.Key()) > 0; });
}

// The IDs for the displays, in the same order. Displays without one get their place in Windows'
// list of display outputs, which Remote Desktop's IDs seem to follow; guessed says whether that
// happened.
std::vector<int> IdsInOrder(const std::vector<const LocalDisplay*>& displays, const DisplayIds& ids, bool& guessed) {
    guessed = false;
    std::vector<int> ordered;
    for (const LocalDisplay* display : displays) {
        auto found = ids.find(display->Key());
        if (found != ids.end()) {
            ordered.push_back(found->second);
        } else {
            ordered.push_back(display->outputIndex);
            guessed = true;
        }
    }
    return ordered;
}

std::wstring ClassOf(HWND window) {
    WCHAR name[256] = {};
    GetClassNameW(window, name, ARRAYSIZE(name));
    return name;
}

// A window's text, including another process's: unlike GetWindowText, WM_GETTEXT is answered by
// the window itself. Empty if it doesn't answer within a moment.
std::wstring WindowTextOf(HWND window) {
    DWORD_PTR length = 0;
    if (!SendMessageTimeoutW(window, WM_GETTEXTLENGTH, 0, 0, SMTO_ABORTIFHUNG, 200, &length) || length == 0) {
        return {};
    }
    length = std::min<DWORD_PTR>(length, 65536);
    std::wstring text(length + 1, L'\0');
    DWORD_PTR copied = 0;
    if (!SendMessageTimeoutW(window, WM_GETTEXT, length + 1, (LPARAM)text.data(), SMTO_ABORTIFHUNG, 200, &copied)) {
        return {};
    }
    text.resize(wcslen(text.c_str()));
    return text;
}

BOOL CALLBACK AppendChildText(HWND child, LPARAM data) {
    auto text = (std::wstring*)data;
    *text += L"\n" + WindowTextOf(child);
    return TRUE;
}

// The text of a window and of all the controls in it (a message box keeps its text in a label).
std::wstring ReadWindowTexts(HWND window) {
    std::wstring text = WindowTextOf(window);
    EnumChildWindows(window, AppendChildText, (LPARAM)&text);
    return text;
}

// The names UI Automation gives a window's parts; a task dialog draws its text itself, where only
// UI Automation can read it.
std::wstring ReadAutomationNames(HWND window) {
    std::wstring text;
    IUIAutomation* automation = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&automation)) &&
        FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                (void**)&automation))) {
        return text;
    }
    // Short timeouts, so a dialog that stops responding can't hold up the mod turning off, which
    // waits for this thread.
    IUIAutomation2* timeouts = nullptr;
    if (SUCCEEDED(automation->QueryInterface(__uuidof(IUIAutomation2), (void**)&timeouts)) && timeouts) {
        timeouts->put_ConnectionTimeout(2000);
        timeouts->put_TransactionTimeout(2000);
        timeouts->Release();
    }
    IUIAutomationElement* root = nullptr;
    IUIAutomationCondition* condition = nullptr;
    IUIAutomationElementArray* elements = nullptr;
    if (SUCCEEDED(automation->ElementFromHandle(window, &root)) && root &&
        SUCCEEDED(automation->CreateTrueCondition(&condition)) &&
        SUCCEEDED(root->FindAll(TreeScope_Descendants, condition, &elements)) && elements) {
        int count = 0;
        elements->get_Length(&count);
        for (int i = 0; i < count; i++) {
            IUIAutomationElement* element = nullptr;
            if (SUCCEEDED(elements->GetElement(i, &element)) && element) {
                BSTR name = nullptr;
                if (SUCCEEDED(element->get_CurrentName(&name)) && name) {
                    text += L"\n";
                    text += name;
                    SysFreeString(name);
                }
                element->Release();
            }
        }
    }
    if (elements) {
        elements->Release();
    }
    if (condition) {
        condition->Release();
    }
    if (root) {
        root->Release();
    }
    automation->Release();
    return text;
}

std::wstring ProcessImageName(DWORD processId) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }
    WCHAR path[MAX_PATH] = {};
    DWORD size = MAX_PATH;
    std::wstring name;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        name = FileNameOf(path);
    }
    CloseHandle(process);
    return name;
}

std::vector<DWORD> MstscProcessIds() {
    std::vector<DWORD> ids;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return ids;
    }
    PROCESSENTRY32W entry{sizeof(entry)};
    for (BOOL more = Process32FirstW(snapshot, &entry); more; more = Process32NextW(snapshot, &entry)) {
        if (_wcsicmp(entry.szExeFile, L"mstsc.exe") == 0) {
            ids.push_back(entry.th32ProcessID);
        }
    }
    CloseHandle(snapshot);
    return ids;
}

struct WindowSearch {
    DWORD processId;
    bool handedOver;  // The process exited without showing anything.
    const std::vector<DWORD>* alreadyRunning;
    std::map<DWORD, bool>* checkedProcesses;
    std::vector<HWND> found;
};

BOOL CALLBACK CollectMstscWindow(HWND window, LPARAM data) {
    auto search = (WindowSearch*)data;
    DWORD owner = 0;
    GetWindowThreadProcessId(window, &owner);
    bool candidate = owner == search->processId;
    // The box belongs to the process just started. Only if that exits without showing anything
    // (handing the work over) is a dialog from another mstsc that wasn't already running accepted,
    // so a Remote Desktop dialog the user has open is left alone.
    if (!candidate && search->handedOver && ClassOf(window) == L"#32770" &&
        std::find(search->alreadyRunning->begin(), search->alreadyRunning->end(), owner) == search->alreadyRunning->end()) {
        auto checked = search->checkedProcesses->find(owner);
        if (checked == search->checkedProcesses->end()) {
            checked = search->checkedProcesses->emplace(owner, EqualsIgnoreCase(ProcessImageName(owner), L"mstsc.exe")).first;
        }
        candidate = checked->second;
    }
    if (candidate) {
        search->found.push_back(window);
    }
    return TRUE;
}

BOOL CALLBACK DescribeChild(HWND child, LPARAM data) {
    auto log = (std::wstring*)data;
    *log += L"  " + ClassOf(child) + L": " + WindowTextOf(child) + L"\r\n";
    return TRUE;
}

// "mstsc /l" shows lines like "0: 1920 x 1080; (0, 0, 1919, 1079)": the ID, the size, and the
// left, top, right and bottom pixels.
const std::wregex& DisplayLinePattern() {
    static const std::wregex pattern(
        LR"((\d+)\s*:\s*(\d+)\s*[x\u00D7]\s*(\d+)\s*;\s*\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\))");
    return pattern;
}

// Runs "mstsc /l", which shows the list in a message box, moves the box off the screen, reads its
// text and closes it. Empty if it couldn't, with what it found in the log. Stops early (closing
// mstsc) when cancel is set, as the mod is turned off.
std::wstring ReadMonitorList(std::wstring& log, HANDLE cancel, bool& cancelled) {
    cancelled = false;
    std::vector<DWORD> alreadyRunning = MstscProcessIds();
    std::map<DWORD, bool> checkedProcesses;
    std::map<HWND, DWORD> firstSeen;
    HANDLE process = nullptr;
    DWORD processId = 0;
    if (!Launch(MstscPath(), L"/l", &process, &processId)) {
        log += L"mstsc /l didn't start.\r\n";
        return {};
    }

    std::wstring result;
    DWORD start = GetTickCount();
    bool handedOver = false;
    while (GetTickCount() - start < 5000) {
        bool exited = WaitForSingleObject(process, 0) == WAIT_OBJECT_0;
        handedOver = handedOver || (exited && firstSeen.empty());
        WindowSearch search{processId, handedOver, &alreadyRunning, &checkedProcesses, {}};
        EnumWindows(CollectMstscWindow, (LPARAM)&search);
        for (HWND window : search.found) {
            if (!firstSeen.count(window)) {
                firstSeen[window] = GetTickCount();
                // Out of sight while it's read; it stays "shown" so it can still be read.
                if (ClassOf(window) == L"#32770") {
                    SetWindowPos(window, nullptr, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
            std::wstring text = ReadWindowTexts(window);
            if (std::regex_search(text, DisplayLinePattern())) {
                result = text;
                break;
            }
            if (GetTickCount() - firstSeen[window] >= 300) {
                text = ReadAutomationNames(window);
                if (std::regex_search(text, DisplayLinePattern())) {
                    result = text;
                    break;
                }
            }
        }
        if (!result.empty()) {
            break;
        }
        if (exited && firstSeen.empty() && GetTickCount() - start > 1500) {
            break;
        }
        if (WaitForSingleObject(cancel, 20) != WAIT_TIMEOUT) {
            cancelled = true;
            break;
        }
    }

    if (result.empty() && !cancelled) {
        WCHAR time[64] = {};
        SYSTEMTIME now;
        GetLocalTime(&now);
        swprintf_s(time, L"%04d-%02d-%02d %02d:%02d:%02d", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
        log += L"Couldn't read the list from \"mstsc /l\" (" + std::wstring(time) + L").\r\n";
        DWORD exitCode = STILL_ACTIVE;
        GetExitCodeProcess(process, &exitCode);
        log += exitCode == STILL_ACTIVE ? L"mstsc was still running.\r\n"
                                        : L"mstsc exited with code " + std::to_wstring(exitCode) + L".\r\n";
        if (firstSeen.empty()) {
            log += L"It showed no windows.\r\n";
        }
        for (const auto& [window, seen] : firstSeen) {
            DWORD owner = 0;
            GetWindowThreadProcessId(window, &owner);
            log += L"Window of process " + std::to_wstring(owner) + L", class " + ClassOf(window) +
                   (IsWindowVisible(window) ? L", visible:\r\n" : L", hidden:\r\n");
            log += L"  Title: " + WindowTextOf(window) + L"\r\n";
            EnumChildWindows(window, DescribeChild, (LPARAM)&log);
            std::wstring names = ReadAutomationNames(window);
            std::replace(names.begin(), names.end(), L'\n', L'|');
            log += L"  UI Automation: " + names + L"\r\n";
        }
    }

    for (const auto& [window, seen] : firstSeen) {
        PostMessageW(window, WM_CLOSE, 0, 0);
    }
    if (WaitForSingleObject(process, 0) != WAIT_OBJECT_0) {
        TerminateProcess(process, 0);
        WaitForSingleObject(process, 2000);
    }
    CloseHandle(process);
    return result;
}

// Pairs mstsc's list with the displays by position. When the positions don't line up exactly
// (scaling can make them differ) but there are as many of each, pairs them in left-to-right order.
DisplayIds MatchDisplayIds(const std::wstring& text, const Displays& displays) {
    struct Entry {
        int id;
        int left;
        int top;
    };
    std::vector<Entry> entries;
    for (std::wsregex_iterator match(text.begin(), text.end(), DisplayLinePattern()), end; match != end; ++match) {
        Entry entry{_wtoi((*match)[1].str().c_str()), _wtoi((*match)[4].str().c_str()), _wtoi((*match)[5].str().c_str())};
        if (std::none_of(entries.begin(), entries.end(), [&](const Entry& e) { return e.id == entry.id; })) {
            entries.push_back(entry);
        }
    }

    DisplayIds ids;
    std::vector<int> used;
    for (const LocalDisplay& display : displays) {
        for (const Entry& entry : entries) {
            if (entry.left == display.bounds.left && entry.top == display.bounds.top) {
                ids[display.Key()] = entry.id;
                used.push_back(entry.id);
                break;
            }
        }
    }
    std::sort(used.begin(), used.end());
    bool distinct = std::adjacent_find(used.begin(), used.end()) == used.end();
    if (ids.size() == displays.size() && distinct) {
        return ids;
    }
    if (entries.size() != displays.size()) {
        return ids;
    }
    ids.clear();
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.left != b.left ? a.left < b.left : a.top < b.top;
    });
    for (size_t i = 0; i < entries.size(); i++) {
        ids[displays[i].Key()] = entries[i].id;  // displays are sorted left to right already.
    }
    return ids;
}

// Reads Remote Desktop's display IDs. Takes up to a few seconds; runs on a worker thread. When
// they can't all be read, logs what was found (seen with logging turned on for the mod).
DisplayIds QueryDisplayIds(const Displays& displays, HANDLE cancel) {
    std::wstring log;
    bool cancelled;
    std::wstring text = ReadMonitorList(log, cancel, cancelled);
    DisplayIds ids = text.empty() ? DisplayIds{} : MatchDisplayIds(text, displays);
    if (!CoversAll(ids, displays) && !cancelled) {
        if (!text.empty()) {
            log += L"Read the list, but it doesn't match the displays:\r\n" + text + L"\r\n";
        }
        log += L"\r\nDisplays:\r\n";
        for (const LocalDisplay& display : displays) {
            log += L"  " + display.deviceName + L" (output " + std::to_wstring(display.outputIndex) + L"): " + display.Key() +
                   (display.primary ? L", main" : L"") + L"\r\n";
        }
        for (const std::wstring& line : Split(log, L'\n')) {
            Wh_Log(L"%s", Trim(line).c_str());
        }
    }
    return ids;
}

// ---------------------------------------------------------------------------
// .rdp files: one "name:type:value" setting per line, like "use multimon:i:1".

class RdpFile {
public:
    bool Load(const std::wstring& path) { return ReadTextLines(path, lines_); }

    // The value of a setting, or nothing when the file doesn't have it.
    std::optional<std::wstring> Get(const std::wstring& name) const {
        int index = IndexOf(name);
        if (index < 0) {
            return std::nullopt;
        }
        const std::wstring& line = lines_[index];
        size_t typeEnd = line.find(L':', line.find(L':') + 1);
        return typeEnd == std::wstring::npos ? std::wstring() : line.substr(typeEnd + 1);
    }

    // A setting's whole line, or nothing when the file doesn't have it.
    std::optional<std::wstring> Line(const std::wstring& name) const {
        int index = IndexOf(name);
        return index >= 0 ? std::optional<std::wstring>(lines_[index]) : std::nullopt;
    }

    // Sets a setting; type is "i" for numbers and "s" for text.
    void Set(const std::wstring& name, const std::wstring& type, const std::wstring& value) {
        SetLine(name, name + L":" + type + L":" + value);
    }

    // Sets a setting to a whole line, as Line returned it.
    void SetLine(const std::wstring& name, const std::wstring& line) {
        int index = IndexOf(name);
        if (index >= 0) {
            lines_[index] = line;
        } else {
            lines_.push_back(line);
        }
    }

    void Remove(const std::wstring& name) {
        for (int index = IndexOf(name); index >= 0; index = IndexOf(name)) {
            lines_.erase(lines_.begin() + index);
        }
    }

    // A signed file (from an IT department or Azure Virtual Desktop) has a signature covering the
    // settings named in "signscope". Changing any of those breaks the signature, so drop it:
    // Remote Desktop Connection then treats the file as unsigned rather than as tampered with.
    void DropSignatureIfCovers(const std::vector<std::wstring>& changedNames) {
        std::optional<std::wstring> scope = Get(L"signscope");
        if (!scope || !Get(L"signature")) {
            return;
        }
        std::vector<std::wstring> covered;
        for (const std::wstring& name : Split(*scope, L',')) {
            covered.push_back(Normalize(name));
        }
        for (const std::wstring& name : changedNames) {
            if (std::find(covered.begin(), covered.end(), Normalize(name)) != covered.end()) {
                Remove(L"signature");
                Remove(L"signscope");
                return;
            }
        }
    }

    // Saves the file as UTF-16, like Remote Desktop Connection does, keeping its attributes.
    bool Save(const std::wstring& path) const { return WriteTextLines(path, lines_); }

    // Whether there are any settings left in it.
    bool Empty() const {
        return std::all_of(lines_.begin(), lines_.end(), [](const std::wstring& line) { return Trim(line).empty(); });
    }

private:
    // Setting names ignore case, and "signscope" writes them in a different style ("Full Address"
    // for "full address", "UseMultimon" for "use multimon"), so compare them without spaces.
    static std::wstring Normalize(const std::wstring& name) {
        std::wstring normalized;
        for (wchar_t c : name) {
            if (c != L' ') {
                normalized += (wchar_t)towlower(c);
            }
        }
        return normalized;
    }

    int IndexOf(const std::wstring& name) const {
        std::wstring wanted = Normalize(name);
        for (size_t i = 0; i < lines_.size(); i++) {
            size_t separator = lines_[i].find(L':');
            if (separator != std::wstring::npos && separator > 0 && Normalize(lines_[i].substr(0, separator)) == wanted) {
                return (int)i;
            }
        }
        return -1;
    }

    std::vector<std::wstring> lines_;
};

// ---------------------------------------------------------------------------
// Computers to connect to: a computer name (using the settings Remote Desktop Connection keeps in
// Default.rdp) or a saved .rdp file with its own settings.

struct Connection {
    std::wstring host;     // For connections without an .rdp file.
    std::wstring rdpFile;  // For connections with one.
    std::wstring customName;  // Given with Rename.

    // The name given to it, else the .rdp file's name, else the computer name or address.
    std::wstring Name() const {
        if (!Trim(customName).empty()) {
            return customName;
        }
        return rdpFile.empty() ? host : FileNameWithoutExtension(rdpFile);
    }

    // Identifies the connection (whatever it's called) in the settings file.
    std::wstring Key() const { return rdpFile.empty() ? L"host:" + host : L"file:" + rdpFile; }

    static std::optional<Connection> FromKey(const std::wstring& key) {
        if (StartsWithIgnoreCase(key, L"host:") && key.size() > 5) {
            return Connection{Trim(key.substr(5)), L"", L""};
        }
        if (StartsWithIgnoreCase(key, L"file:") && key.size() > 5) {
            return Connection{L"", key.substr(5), L""};
        }
        return std::nullopt;
    }

    // The computer the connection goes to: the name itself, or the .rdp file's "full address".
    std::wstring ReadAddress() const {
        if (rdpFile.empty()) {
            return host;
        }
        RdpFile file;
        if (!file.Load(rdpFile)) {
            return {};
        }
        return Trim(file.Get(L"full address").value_or(L""));
    }
};

// Computers recently connected to with Remote Desktop Connection, newest first (the list its
// "Computer" box suggests).
std::vector<std::wstring> RecentComputers() {
    std::vector<std::wstring> computers;
    for (int i = 0; i < 10; i++) {
        WCHAR value[512] = {};
        DWORD size = sizeof(value);
        std::wstring name = L"MRU" + std::to_wstring(i);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Terminal Server Client\\Default", name.c_str(),
                         RRF_RT_REG_SZ, nullptr, value, &size) == ERROR_SUCCESS) {
            std::wstring computer = Trim(value);
            if (!computer.empty() && std::none_of(computers.begin(), computers.end(),
                                                  [&](const std::wstring& c) { return EqualsIgnoreCase(c, computer); })) {
                computers.push_back(computer);
            }
        }
    }
    return computers;
}

// ---------------------------------------------------------------------------
// Settings, kept in the mod's storage. The first time the mod runs for a user, it takes on
// RDPLauncher.exe's settings from %APPDATA%\RDPLauncher\settings.txt, which it only reads.

struct Settings {
    std::vector<Connection> connections;
    std::wstring currentConnectionKey;
    std::map<std::wstring, std::vector<Choice>> displayButtons;  // By setup key.
    std::map<std::wstring, DisplayIds> displayIds;               // By outputs key.

    void Load() {
        if (!Storage::GetInt(L"Imported", 0)) {
            Import();
            Save();
            Storage::SetInt(L"Imported", 1);
            return;
        }
        for (const std::wstring& line : Split(Storage::GetString(L"Connections"), L'\n')) {
            AddConnectionLine(line);
        }
        currentConnectionKey = Storage::GetString(L"CurrentConnection");
        for (const std::wstring& line : Split(Storage::GetString(L"DisplayButtons"), L'\n')) {
            AddDisplayButtonLine(line);
        }
        for (const std::wstring& line : Split(Storage::GetString(L"DisplayIds"), L'\n')) {
            AddDisplayIdsLine(line);
        }
    }

    bool Save() const {
        std::vector<std::wstring> connectionLines;
        for (const Connection& connection : connections) {
            connectionLines.push_back(connection.Key() + (Trim(connection.customName).empty() ? L"" : L"|" + Trim(connection.customName)));
        }
        std::vector<std::wstring> buttonLines;
        for (const auto& [setup, buttons] : displayButtons) {
            for (const Choice& button : buttons) {
                buttonLines.push_back(setup + L"|" + Join(button, L";"));
            }
        }
        std::vector<std::wstring> idLines;
        for (const auto& [outputs, ids] : displayIds) {
            std::vector<std::wstring> pairs;
            for (const auto& [display, id] : ids) {
                pairs.push_back(display + L"=" + std::to_wstring(id));
            }
            idLines.push_back(outputs + L"|" + Join(pairs, L";"));
        }
        bool saved = Storage::SetString(L"Connections", Join(connectionLines, L"\n"));
        saved = Storage::SetString(L"CurrentConnection", currentConnectionKey) && saved;
        saved = Storage::SetString(L"DisplayButtons", Join(buttonLines, L"\n")) && saved;
        return Storage::SetString(L"DisplayIds", Join(idLines, L"\n")) && saved;
    }

    Connection* FindConnection(const std::wstring& key) {
        for (Connection& connection : connections) {
            if (EqualsIgnoreCase(connection.Key(), key)) {
                return &connection;
            }
        }
        return nullptr;
    }

    Connection* CurrentConnection() {
        Connection* current = FindConnection(currentConnectionKey);
        return current ? current : connections.empty() ? nullptr : &connections[0];
    }

    // Adds the connection (unless it's already there) and makes it the current one.
    void AddConnection(const Connection& connection) {
        if (!FindConnection(connection.Key())) {
            connections.push_back(connection);
        }
        currentConnectionKey = FindConnection(connection.Key())->Key();
    }

    void RemoveConnection(const std::wstring& key) {
        connections.erase(std::remove_if(connections.begin(), connections.end(),
                                         [&](const Connection& c) { return EqualsIgnoreCase(c.Key(), key); }),
                          connections.end());
        if (EqualsIgnoreCase(currentConnectionKey, key)) {
            currentConnectionKey = connections.empty() ? L"" : connections[0].Key();
        }
    }

private:
    // RDPLauncher.exe's settings.txt has "key=value" lines; the values of its Connection,
    // DisplayButton and DisplayIds lines are the lines of the mod's values with those names.
    void Import() {
        std::wstring path = KnownFolder(FOLDERID_RoamingAppData) + L"\\RDPLauncher\\settings.txt";
        for (const auto& [key, value] : ReadSettingsFile(path)) {
            if (key == L"Connection") {
                AddConnectionLine(value);
            } else if (key == L"CurrentConnection") {
                currentConnectionKey = value;
            } else if (key == L"DisplayButton") {
                AddDisplayButtonLine(value);
            } else if (key == L"DisplayIds") {
                AddDisplayIdsLine(value);
            }
        }
        if (!connections.empty()) {
            Wh_Log(L"Imported %d computers from %s", (int)connections.size(), path.c_str());
        }
    }

    // <connection key>|<name it was given>, or just the key.
    void AddConnectionLine(const std::wstring& line) {
        size_t bar = line.find(L'|');
        std::optional<Connection> connection = Connection::FromKey(bar == std::wstring::npos ? line : line.substr(0, bar));
        if (connection && !FindConnection(connection->Key())) {
            if (bar != std::wstring::npos) {
                connection->customName = Trim(line.substr(bar + 1));
            }
            connections.push_back(*connection);
        }
    }

    // <setup key>|<display key>;<display key>…, one line per button.
    void AddDisplayButtonLine(const std::wstring& line) {
        size_t bar = line.find(L'|');
        if (bar != std::wstring::npos && bar > 0) {
            Choice choice = Split(line.substr(bar + 1), L';');
            if (!choice.empty()) {
                displayButtons[line.substr(0, bar)].push_back(choice);
            }
        }
    }

    // <outputs key>|<display key>=<id>;<display key>=<id>…
    void AddDisplayIdsLine(const std::wstring& line) {
        size_t bar = line.find(L'|');
        if (bar == std::wstring::npos || bar == 0) {
            return;
        }
        DisplayIds ids;
        for (const std::wstring& pair : Split(line.substr(bar + 1), L';')) {
            size_t equals = pair.find_last_of(L'=');
            int id;
            if (equals != std::wstring::npos && equals > 0 && ParseInt(pair.substr(equals + 1), id)) {
                ids[pair.substr(0, equals)] = id;
            }
        }
        if (!ids.empty()) {
            displayIds[line.substr(0, bar)] = ids;
        }
    }
};

// ---------------------------------------------------------------------------
// Starting Remote Desktop Connection (mstsc.exe) on the chosen displays.
//
// Since April 2026 Windows shows a security warning every time an .rdp file is opened, but not
// for a computer typed into Remote Desktop Connection or given with "mstsc /v". So a computer
// added by name is started with /v, after setting the displays in the settings Remote Desktop
// Connection uses for that: Default.rdp in Documents, which its own Display tab changes too. Its
// display settings go back to what they were as soon as the session ends (DefaultRdpChange). A
// saved .rdp file is copied with the displays set and the copy is opened, leaving the original as
// it was; the copy is deleted when the session ends.

std::wstring DefaultRdpPath() {
    return KnownFolder(FOLDERID_Documents) + L"\\Default.rdp";
}

std::wstring SessionsFolder() {
    std::wstring storage = Storage::Folder();
    return storage.empty() ? std::wstring() : storage + L"\\Sessions";
}

std::wstring SafeFileName(const std::wstring& name) {
    std::wstring safe;
    for (wchar_t c : name) {
        safe += (c < 32 || wcschr(L"<>:\"/\\|?*", c)) ? L'_' : c;
    }
    safe = Trim(safe);
    return safe.empty() ? L"Remote Desktop" : safe;
}

// A display setting in an .rdp file; no value means the setting is left out.
struct RdpSetting {
    std::wstring name;
    std::wstring type;  // "i" for numbers, "s" for text.
    std::optional<std::wstring> value;
};

// The settings for a full-screen session on the displays with the given IDs (the session's main
// display first), or on all displays when there are none.
std::vector<RdpSetting> FullScreenSettings(const std::vector<int>* displayIds) {
    std::optional<std::wstring> selected;
    if (displayIds) {
        std::vector<std::wstring> ids;
        for (int id : *displayIds) {
            ids.push_back(std::to_wstring(id));
        }
        selected = Join(ids, L",");
    }
    return {{L"screen mode id", L"i", L"2"},
            {L"use multimon", L"i", L"1"},
            {L"span monitors", L"i", L"0"},
            {L"selectedmonitors", L"s", selected}};
}

void ApplySettings(RdpFile& file, const std::vector<RdpSetting>& settings) {
    for (const RdpSetting& setting : settings) {
        if (setting.value) {
            file.Set(setting.name, setting.type, *setting.value);
        } else {
            file.Remove(setting.name);
        }
    }
}

// The display settings the mod changed in Default.rdp, kept in its storage until they've been put
// back: whether the file was there, and for each setting its line from before (nothing when the
// file didn't have it) and the values the mod has given it since. A setting is only put back while
// it still has one of those values, so a change made in Remote Desktop Connection meanwhile stays.
struct DefaultRdpChange {
    struct Setting {
        std::optional<std::wstring> lineBefore;
        std::vector<std::optional<std::wstring>> valuesGiven;
    };

    bool fileExisted = true;
    std::map<std::wstring, Setting> settings;  // By name.

    // Stored as lines: "existed" or "created", then "<name>\t<line before>\t<value given>…" for each
    // setting, where each line or value is "=" and its text, or "-" for none.
    static std::optional<DefaultRdpChange> Load() {
        std::vector<std::wstring> lines = Split(Storage::GetString(L"DefaultRdpChange"), L'\n');
        if (lines.empty()) {
            return std::nullopt;
        }
        DefaultRdpChange change;
        change.fileExisted = lines[0] != L"created";
        for (size_t i = 1; i < lines.size(); i++) {
            std::vector<std::wstring> fields = Split(lines[i], L'\t');
            if (fields.size() < 2) {
                continue;
            }
            Setting& setting = change.settings[fields[0]];
            setting.lineBefore = Decode(fields[1]);
            for (size_t j = 2; j < fields.size(); j++) {
                setting.valuesGiven.push_back(Decode(fields[j]));
            }
        }
        return change;
    }

    bool Save() const {
        std::vector<std::wstring> lines{fileExisted ? L"existed" : L"created"};
        for (const auto& [name, setting] : settings) {
            std::wstring line = name + L"\t" + Encode(setting.lineBefore);
            for (const std::optional<std::wstring>& value : setting.valuesGiven) {
                line += L"\t" + Encode(value);
            }
            lines.push_back(line);
        }
        return Storage::SetString(L"DefaultRdpChange", Join(lines, L"\n"));
    }

    static void Forget() { Storage::Delete(L"DefaultRdpChange"); }

private:
    static std::wstring Encode(const std::optional<std::wstring>& text) { return text ? L"=" + *text : L"-"; }

    static std::optional<std::wstring> Decode(const std::wstring& field) {
        return field[0] == L'=' ? std::optional<std::wstring>(field.substr(1)) : std::nullopt;
    }
};

// Sets the display settings in Default.rdp for a session started with /v. What it had before is
// noted first (unless that's noted already, from a session that hasn't ended), so it can be put
// back even if the mod stops (or Explorer restarts) before the session ends.
std::optional<std::wstring> SetDefaultRdpDisplays(const std::vector<RdpSetting>& settings) {
    std::wstring path = DefaultRdpPath();
    bool existed = FileExists(path);
    RdpFile file;
    // Never replace the settings with just the display ones because they couldn't be read
    // (OneDrive hasn't downloaded the file, or Remote Desktop Connection is writing it).
    if (existed && !file.Load(path)) {
        return L"Couldn't read " + path;
    }
    std::optional<DefaultRdpChange> change = DefaultRdpChange::Load();
    if (!change) {
        change = DefaultRdpChange();
        change->fileExisted = existed;
    }
    for (const RdpSetting& setting : settings) {
        auto [entry, added] = change->settings.try_emplace(setting.name);
        if (added) {
            entry->second.lineBefore = file.Line(setting.name);
        }
        std::vector<std::optional<std::wstring>>& given = entry->second.valuesGiven;
        if (std::find(given.begin(), given.end(), setting.value) == given.end()) {
            given.push_back(setting.value);
        }
    }
    if (!change->Save()) {
        return std::wstring(L"Couldn't note Remote Desktop's display settings to put them back later.");
    }
    ApplySettings(file, settings);
    if (!file.Save(path)) {
        return L"Couldn't write " + path;
    }
    if (!existed) {
        SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN);  // As Remote Desktop Connection keeps it.
    }
    return std::nullopt;
}

// The sessions started with /v (so using Default.rdp's display settings), by process ID and start
// time (IDs get reused), kept in the mod's storage so they're still known if Explorer restarts
// during one.
struct SessionMark {
    DWORD processId = 0;
    ULONGLONG started = 0;

    bool operator==(const SessionMark& other) const = default;
};

ULONGLONG ProcessStartTime(HANDLE process) {
    FILETIME created, exited, kernel, user;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user)) {
        return 0;
    }
    return ((ULONGLONG)created.dwHighDateTime << 32) | created.dwLowDateTime;
}

std::vector<SessionMark> LoadSessionMarks() {
    std::vector<SessionMark> marks;
    for (const std::wstring& line : Split(Storage::GetString(L"DefaultRdpSessions"), L'\n')) {
        std::vector<std::wstring> fields = Split(line, L'\t');
        if (fields.size() == 2) {
            marks.push_back({(DWORD)wcstoul(fields[0].c_str(), nullptr, 10), wcstoull(fields[1].c_str(), nullptr, 10)});
        }
    }
    return marks;
}

void SaveSessionMarks(const std::vector<SessionMark>& marks) {
    if (marks.empty()) {
        Storage::Delete(L"DefaultRdpSessions");
        return;
    }
    std::vector<std::wstring> lines;
    for (const SessionMark& mark : marks) {
        lines.push_back(std::to_wstring(mark.processId) + L"\t" + std::to_wstring(mark.started));
    }
    Storage::SetString(L"DefaultRdpSessions", Join(lines, L"\n"));
}

// The session's process, if it's still running (the same process: ID and start time), or null.
HANDLE OpenRunningSession(const SessionMark& mark) {
    HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, mark.processId);
    if (!process) {
        return nullptr;
    }
    if (WaitForSingleObject(process, 0) != WAIT_TIMEOUT || ProcessStartTime(process) != mark.started) {
        CloseHandle(process);
        return nullptr;
    }
    return process;
}

// Puts Default.rdp's display settings back to what they were before the mod changed them, those
// that still have a value the mod gave them. Then forgets the change, unless keepNote is set: a
// session that's still running may save Default.rdp when it ends, so it's checked again next time.
void RestoreDefaultRdp(bool keepNote) {
    std::optional<DefaultRdpChange> change = DefaultRdpChange::Load();
    if (!change) {
        return;
    }
    std::wstring path = DefaultRdpPath();
    if (FileExists(path)) {
        RdpFile file;
        if (!file.Load(path)) {
            Wh_Log(L"Couldn't read %s to put its display settings back", path.c_str());
            return;
        }
        bool changed = false;
        for (const auto& [name, setting] : change->settings) {
            std::optional<std::wstring> value = file.Get(name);
            if (std::find(setting.valuesGiven.begin(), setting.valuesGiven.end(), value) == setting.valuesGiven.end()) {
                continue;  // Changed since, in Remote Desktop Connection: that stays.
            }
            if (setting.lineBefore) {
                file.SetLine(name, *setting.lineBefore);
            } else {
                file.Remove(name);
            }
            changed = true;
        }
        // A Default.rdp the mod created goes again, unless Remote Desktop Connection saved its own
        // settings in it meanwhile.
        bool done = !change->fileExisted && file.Empty() ? DeleteFileW(path.c_str()) : !changed || file.Save(path);
        if (!done) {
            Wh_Log(L"Couldn't put the display settings back in %s", path.c_str());
            return;
        }
    }
    if (!keepNote) {
        DefaultRdpChange::Forget();
    }
}

// A session that was started: Remote Desktop Connection's process, and what to tidy up when it ends.
struct StartedSession {
    HANDLE process = nullptr;
    std::wstring copy;           // The .rdp copy it was opened from.
    bool setDefaultRdp = false;  // Started with /v, after setting the displays in Default.rdp.
};

// Starts a full-screen session on the displays with the given IDs (the session's main display
// first), or on all displays when there are none. Returns an error message, or nothing.
std::optional<std::wstring> StartFullScreen(const Connection& connection, const std::vector<int>* displayIds,
                                            StartedSession& started) {
    std::vector<RdpSetting> settings = FullScreenSettings(displayIds);
    if (!connection.rdpFile.empty()) {
        RdpFile file;
        if (!file.Load(connection.rdpFile)) {
            return L"Couldn't read " + connection.rdpFile;
        }
        std::vector<std::wstring> names;
        for (const RdpSetting& setting : settings) {
            names.push_back(setting.name);
        }
        file.DropSignatureIfCovers(names);
        ApplySettings(file, settings);
        // Remote Desktop Connection shows the file name in its title bar, so name it after the connection.
        std::wstring folder = SessionsFolder();
        std::wstring copy = folder + L"\\" + SafeFileName(connection.Name()) + L".rdp";
        if (folder.empty() || !file.Save(copy)) {
            return L"Couldn't write " + copy;
        }
        if (!Launch(MstscPath(), L"\"" + copy + L"\"", &started.process)) {
            DeleteFileW(copy.c_str());
            return std::wstring(L"Remote Desktop Connection didn't start.");
        }
        started.copy = copy;
        return std::nullopt;
    }

    // Computer names typed in have no spaces (the add dialog doesn't take them), but ones from Remote
    // Desktop Connection's recent list aren't checked, so the name is quoted, and can't have a quote.
    if (connection.host.find(L'"') != std::wstring::npos) {
        return L"Remote Desktop can't connect to " + connection.host;
    }
    if (std::optional<std::wstring> error = SetDefaultRdpDisplays(settings)) {
        return error;
    }
    started.setDefaultRdp = true;
    if (!Launch(MstscPath(), L"/v:\"" + connection.host + L"\"", &started.process)) {
        return std::wstring(L"Remote Desktop Connection didn't start.");
    }
    return std::nullopt;
}

// Deletes the .rdp copies sessions were started from that are still there (when the mod stopped
// before they ended).
void DeleteSessionCopies() {
    std::wstring folder = SessionsFolder();
    if (folder.empty()) {
        return;
    }
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folder + L"\\*.rdp").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        DeleteFileW((folder + L"\\" + found.cFileName).c_str());
    } while (FindNextFileW(search, &found));
    FindClose(search);
}

// Opens the connection's settings in Remote Desktop Connection, to change the ones other than
// displays: its .rdp file, or for a computer name the defaults (Default.rdp).
bool EditConnection(const Connection& connection) {
    std::wstring path = connection.rdpFile.empty() ? DefaultRdpPath() : connection.rdpFile;
    // Without a Default.rdp yet, just open Remote Desktop Connection; it saves one on connecting.
    return Launch(MstscPath(), FileExists(path) ? L"/edit \"" + path + L"\"" : L"");
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// RDP Launcher: the taskbar widget, the display picker flyout and the app.

namespace {

// ---------------------------------------------------------------------------
// Icons: glyphs from the Windows icon font, like the taskbar's own.

const wchar_t* const kGlyphThisPC = L"\uEC4E";
const wchar_t* const kGlyphMonitor = L"\uE7F4";  // TVMonitor
const wchar_t* const kGlyphCheckMark = L"\uE73E";

// ---------------------------------------------------------------------------
// The widget: the computer's name (over its address, once it's been given a name), then a button
// for each set of displays ("1", "2", "3"). Click a number to start Remote Desktop on that many
// displays; hover it to see (and change) which ones. Click or scroll over the computer to go
// through the computers. Right-click anywhere for the menu.

struct WidgetContent {
    std::wstring title;
    std::wstring detail;  // The second line; empty for just the title.
    Displays displays;
    std::vector<Choice> buttons;  // Empty when there's nothing to connect to.
    int busyButton = -1;          // The button whose session is starting.
};

// The widget's parts: the computer, then display button i as part i + 1.
constexpr int kConnectionPart = 0;
// From the display map to the app: a display was clicked, to add to the button or take off it.
// wParam: the display button. lParam: the display's place in WidgetContent::displays.
constexpr UINT kToggleDisplayMessage = WM_APP + 0x310;

using winrt::Windows::UI::Color;

wuxm::SolidColorBrush Brush(Color color) {
    return wuxm::SolidColorBrush(color);
}

Color ColorWithAlpha(BYTE alpha, Color color) {
    color.A = alpha;
    return color;
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

// The display map's colors, for the taskbar's light or dark theme, with Windows' accent color.
struct MapColors {
    Color accent, onAccent, secondary, track, hover;
};

MapColors MapColorsFor(bool light) {
    MapColors colors;
    colors.accent = AccentColor(light);
    colors.onAccent = light ? Color{255, 255, 255, 255} : Color{255, 0, 0, 0};
    colors.secondary = light ? Color{255, 96, 96, 96} : Color{255, 197, 197, 197};
    colors.track = light ? Color{255, 196, 196, 196} : Color{255, 80, 80, 80};
    colors.hover = light ? Color{14, 0, 0, 0} : Color{16, 255, 255, 255};
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

// On the taskbar's thread.
class RdpView : public TaskbarXaml::View {
public:
    wux::FrameworkElement Build() override {
        row_ = wuxc::StackPanel();
        row_.Orientation(wuxc::Orientation::Horizontal);

        // The This PC icon, then the name over the address, like the weather widget's two lines;
        // just the name without an address.
        auto icon = TaskbarXaml::MakeGlyph(kGlyphThisPC, 18);
        icon.Width(24);
        icon.Margin(wux::ThicknessHelper::FromLengths(8, 0, 0, 0));
        wuxc::StackPanel lines;
        lines.VerticalAlignment(wux::VerticalAlignment::Center);
        lines.Margin(wux::ThicknessHelper::FromLengths(10, 0, 12, 0));
        title_ = MakeText(12, true);
        detail_ = MakeText(12, false);
        detail_.Opacity(0.75);
        lines.Children().Append(title_);
        lines.Children().Append(detail_);
        auto connection = connection_.Make(kConnectionPart, MakeIconAndText(icon, lines), L"Computer");
        // Sized to fit the name and address, like the weather widget sizes itself to its text.
        connection.MaxWidth(396);
        connection_.onHover = [this](bool) { StopHoverTimer(); };
        connection_.onPress = [this]() { CloseFlyout(); };
        row_.Children().Append(connection);

        buttonsPanel_ = wuxc::StackPanel();
        buttonsPanel_.Orientation(wuxc::Orientation::Horizontal);
        row_.Children().Append(buttonsPanel_);

        Show(content_);
        return row_;
    }

    void Forget() override {
        CloseFlyout();
        closer_.Forget();
        StopHoverTimer();
        hoverTimer_ = nullptr;
        connection_.Forget();
        buttons_.clear();
        row_ = nullptr;
        title_ = nullptr;
        detail_ = nullptr;
        buttonsPanel_ = nullptr;
    }

    void Show(const WidgetContent& content) {
        content_ = content;
        if (!row_) {
            return;
        }
        title_.Text(content_.title);
        detail_.Text(content_.detail);
        detail_.Visibility(content_.detail.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        while (buttons_.size() < content_.buttons.size()) {
            AddButton();
        }
        while (buttons_.size() > content_.buttons.size()) {
            buttonsPanel_.Children().RemoveAtEnd();
            buttons_.pop_back();
        }
        for (size_t i = 0; i < buttons_.size(); i++) {
            DisplayButton& button = *buttons_[i];
            size_t count = content_.buttons[i].size();
            button.number.Text(std::to_wstring(count));
            // Dimmed while its session starts.
            button.icon.Opacity(content_.busyButton == (int)i ? 0.43 : 1.0);
            wux::Automation::AutomationProperties::SetName(button.part.Border(), L"Connect on " + DescribeCount(count));
        }
        if (flyout_) {
            if (flyoutButton_ < (int)content_.buttons.size()) {
                FillMap();
            } else {
                CloseFlyout();
            }
        }
    }

private:
    struct DisplayButton {
        TaskbarXaml::Part part;
        wuxc::Grid icon{nullptr};
        wuxc::TextBlock number{nullptr};
    };

    // The monitor glyph with the number of displays on its screen.
    void AddButton() {
        int index = (int)buttons_.size();
        auto button = std::make_unique<DisplayButton>();
        button->icon = wuxc::Grid();
        button->icon.Children().Append(TaskbarXaml::MakeGlyph(kGlyphMonitor, 20));
        button->number = MakeText(11, true);
        button->number.HorizontalAlignment(wux::HorizontalAlignment::Center);
        button->number.VerticalAlignment(wux::VerticalAlignment::Center);
        button->number.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 3));  // On the screen, above the stand.
        button->icon.Children().Append(button->number);
        auto border = button->part.Make(index + 1, button->icon, L"Connect");
        border.Width(36);
        button->part.onHover = [this, index](bool entered) { OnButtonHover(index, entered); };
        button->part.onPress = [this]() {
            StopHoverTimer();
            CloseFlyout();
        };
        buttonsPanel_.Children().Append(border);
        buttons_.push_back(std::move(button));
    }

    // A display button opens its map after a moment of hovering it; the map closes itself once the
    // mouse moves away from it.
    void OnButtonHover(int button, bool entered) {
        if (flyout_ && button == flyoutButton_) {
            closer_.PartHovered(entered);
        }
        StopHoverTimer();
        hoveredButton_ = entered ? button : -1;
        if (!entered || (flyout_ && flyoutButton_ == button)) {
            return;
        }
        if (!hoverTimer_) {
            hoverTimer_ = wux::DispatcherTimer();
            hoverTimer_.Interval(std::chrono::milliseconds(400));
            hoverTimer_.Tick([this](auto const&, auto const&) {
                StopHoverTimer();
                if (hoveredButton_ >= 0 && hoveredButton_ < (int)buttons_.size()) {
                    OpenFlyout(hoveredButton_);
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

    void OpenFlyout(int button) {
        CloseFlyout();
        flyoutButton_ = button;
        hoveredDisplay_ = -1;
        pressedDisplay_ = -1;
        map_ = wuxc::StackPanel();
        map_.Width(288);
        mapLoaded_ = map_.Loaded(winrt::auto_revoke, [this](auto const&, auto const&) { FillMap(); });  // In the flyout's theme.
        flyout_ = wuxc::Flyout();
        flyout_.ShouldConstrainToRootBounds(false);  // Above the taskbar, outside its XAML.
        flyout_.Content(map_);
        flyoutClosed_ = flyout_.Closed(winrt::auto_revoke, [this](auto const&, auto const&) { ForgetFlyout(); });
        FillMap();
        wuxc::Primitives::FlyoutShowOptions options;
        options.Placement(wuxc::Primitives::FlyoutPlacementMode::Top);
        options.ShowMode(wuxc::Primitives::FlyoutShowMode::TransientWithDismissOnPointerMoveAway);
        closer_.Start(flyout_, map_, [this]() { CloseFlyout(); });
        flyout_.ShowAt(buttons_[button]->part.Border(), options);
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
        closer_.Stop();
        flyoutClosed_.revoke();
        mapLoaded_.revoke();
        if (map_) {
            map_.Children().Clear();  // Its displays' handlers go with them.
        }
        cells_.clear();
        map_ = nullptr;
        description_ = nullptr;
        flyout_ = nullptr;
        flyoutButton_ = -1;
    }

    // The map of this PC's displays: the ones the button uses are highlighted, and the session's
    // main display (where the remote taskbar goes) has a bar along its bottom.
    void FillMap() {
        if (!map_ || flyoutButton_ < 0 || flyoutButton_ >= (int)content_.buttons.size()) {
            return;
        }
        map_.Children().Clear();
        cells_.clear();
        const Choice& chosen = content_.buttons[flyoutButton_];
        std::vector<const LocalDisplay*> sessionOrder = InSessionOrder(content_.displays, chosen);
        mainDisplay_ = -1;
        for (size_t i = 0; i < content_.displays.size(); i++) {
            if (!sessionOrder.empty() && &content_.displays[i] == sessionOrder[0]) {
                mainDisplay_ = (int)i;
            }
        }
        bool light = (map_.IsLoaded() ? map_.ActualTheme() : row_.ActualTheme()) == wux::ElementTheme::Light;
        colors_ = MapColorsFor(light);

        auto title = MakeText(14, true);
        title.Text(content_.title + L" on " + DescribeCount(sessionOrder.size()));
        map_.Children().Append(title);
        auto hint = MakeText(12, false);
        hint.Text(L"Click a display to add or remove it.");
        hint.Foreground(Brush(colors_.secondary));
        hint.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        map_.Children().Append(hint);

        // The real arrangement, scaled to fit and centered.
        const double width = 288;
        const double height = 110;
        wuxc::Canvas canvas;
        canvas.Width(width);
        canvas.Height(height);
        canvas.Margin(wux::ThicknessHelper::FromLengths(0, 12, 0, 0));
        LONG left = LONG_MAX, top = LONG_MAX, right = LONG_MIN, bottom = LONG_MIN;
        for (const LocalDisplay& display : content_.displays) {
            left = std::min(left, display.bounds.left);
            top = std::min(top, display.bounds.top);
            right = std::max(right, display.bounds.right);
            bottom = std::max(bottom, display.bounds.bottom);
        }
        double scale = std::min(width / std::max<LONG>(1, right - left), height / std::max<LONG>(1, bottom - top));
        double offsetX = (width - (right - left) * scale) / 2;
        double offsetY = (height - (bottom - top) * scale) / 2;
        const double gap = 2;  // Separates displays that touch.
        for (size_t i = 0; i < content_.displays.size(); i++) {
            const LocalDisplay& display = content_.displays[i];
            double cellWidth = std::max(1.0, display.Width() * scale - gap * 2);
            double cellHeight = std::max(1.0, display.Height() * scale - gap * 2);
            Cell cell;
            cell.chosen = Contains(chosen, display.Key());
            cell.border = wuxc::Border();
            cell.border.Width(cellWidth);
            cell.border.Height(cellHeight);
            cell.border.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4));
            wuxc::Canvas::SetLeft(cell.border, offsetX + (display.bounds.left - left) * scale + gap);
            wuxc::Canvas::SetTop(cell.border, offsetY + (display.bounds.top - top) * scale + gap);
            wuxc::Grid inside;
            Color mark = cell.chosen ? colors_.onAccent : colors_.secondary;
            if ((int)i == mainDisplay_) {
                double barHeight = std::max(2.0, std::min(5.0, cellHeight * 0.1));
                wuxc::Border bar;
                bar.Height(barHeight);
                bar.VerticalAlignment(wux::VerticalAlignment::Bottom);
                bar.Margin(wux::ThicknessHelper::FromLengths(3, 0, 3, 3));
                bar.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(barHeight / 2));
                bar.Background(Brush(ColorWithAlpha(150, mark)));
                inside.Children().Append(bar);
            }
            double check = std::min(20.0, std::min(cellWidth, cellHeight) * 0.5);
            if (cell.chosen && check >= 8) {
                auto tick = TaskbarXaml::MakeGlyph(kGlyphCheckMark, check);
                tick.Foreground(Brush(mark));
                tick.HorizontalAlignment(wux::HorizontalAlignment::Center);
                tick.VerticalAlignment(wux::VerticalAlignment::Center);
                inside.Children().Append(tick);
            }
            cell.border.Child(inside);
            int index = (int)i;
            cell.border.PointerEntered([this, index](auto const&, auto const&) {
                hoveredDisplay_ = index;
                PaintMap();
            });
            cell.border.PointerExited([this, index](auto const&, auto const&) {
                if (hoveredDisplay_ == index) {
                    hoveredDisplay_ = -1;
                }
                pressedDisplay_ = -1;
                PaintMap();
            });
            cell.border.PointerPressed([this, index](auto const&, wux::Input::PointerRoutedEventArgs const& args) {
                pressedDisplay_ = index;
                args.Handled(true);
            });
            cell.border.PointerReleased([this, index](auto const&, wux::Input::PointerRoutedEventArgs const& args) {
                args.Handled(true);
                if (pressedDisplay_ == index && flyoutButton_ >= 0) {
                    TaskbarXaml::PostToApp(kToggleDisplayMessage, flyoutButton_, index);
                }
                pressedDisplay_ = -1;
            });
            canvas.Children().Append(cell.border);
            cells_.push_back(cell);
        }
        map_.Children().Append(canvas);

        description_ = MakeText(12, false);
        description_.Foreground(Brush(colors_.secondary));
        description_.HorizontalAlignment(wux::HorizontalAlignment::Center);
        description_.Margin(wux::ThicknessHelper::FromLengths(0, 12, 0, 0));
        map_.Children().Append(description_);
        PaintMap();
    }

    // A chosen display is filled with the accent color and ticked; the hovered one is lighter.
    void PaintMap() {
        for (size_t i = 0; i < cells_.size(); i++) {
            Cell& cell = cells_[i];
            bool hovered = (int)i == hoveredDisplay_;
            if (cell.chosen) {
                cell.border.Background(Brush(hovered ? ColorWithAlpha(215, colors_.accent) : colors_.accent));
                cell.border.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
            } else {
                cell.border.Background(Brush(hovered ? colors_.track : colors_.hover));
                cell.border.BorderBrush(Brush(colors_.track));
                cell.border.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
            }
        }
        // What the hovered display is, or what the main display is.
        int described = hoveredDisplay_ >= 0 ? hoveredDisplay_ : mainDisplay_;
        if (!description_ || described < 0 || described >= (int)content_.displays.size()) {
            if (description_) {
                description_.Text(L"");
            }
            return;
        }
        const LocalDisplay& display = content_.displays[described];
        std::wstring text = display.name + L" · " + display.Resolution();
        if (described == mainDisplay_) {
            text += L" · main display";
        } else if (!cells_[described].chosen) {
            text += L" · not used";
        }
        description_.Text(text);
    }

    struct Cell {
        wuxc::Border border{nullptr};
        bool chosen = false;
    };

    WidgetContent content_;
    wuxc::StackPanel row_{nullptr};
    TaskbarXaml::Part connection_;
    wuxc::TextBlock title_{nullptr};
    wuxc::TextBlock detail_{nullptr};
    wuxc::StackPanel buttonsPanel_{nullptr};
    std::vector<std::unique_ptr<DisplayButton>> buttons_;
    wux::DispatcherTimer hoverTimer_{nullptr};
    int hoveredButton_ = -1;
    // The display map, while it's open.
    wuxc::Flyout flyout_{nullptr};
    wuxc::Primitives::FlyoutBase::Closed_revoker flyoutClosed_;
    wux::FrameworkElement::Loaded_revoker mapLoaded_;
    wuxc::StackPanel map_{nullptr};
    wuxc::TextBlock description_{nullptr};
    std::vector<Cell> cells_;
    MapColors colors_{};
    int flyoutButton_ = -1;
    int mainDisplay_ = -1;
    int hoveredDisplay_ = -1;
    int pressedDisplay_ = -1;
    TaskbarXaml::HoverCloser closer_;
};

[[clang::no_destroy]] std::optional<RdpView> g_view{std::in_place};  // Not destroyed: see TaskbarXaml::g_bridge.

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
// The app: keeps the widget up to date, starts sessions and runs the menu.

class RdpApp {
public:
    bool Start() {
        settings_.Load();
        if (!messages_.Create(L"RDPLauncher.Mod.Messages")) {
            return false;
        }
        messages_.onTimer = [this](UINT_PTR timer) { OnTimer(timer); };
        messages_.onMessage = [this](UINT message, WPARAM wParam, LPARAM lParam) { OnMessage(message, wParam, lParam); };
        // Tidy up after sessions that were still running when the mod last stopped. Ones started
        // with /v that are still running (Explorer restarted during them) keep their display
        // settings in Default.rdp until they end, as they may save it then.
        if (!WatchRunningSessions()) {
            RestoreDefaultRdp(false);
        }
        DeleteSessionCopies();
        return StartWidget();
    }

    ~RdpApp() {
        TaskbarXaml::g_appWindow = nullptr;
        SetEvent(cancel_);  // A display-number read in progress closes mstsc and stops.
        workers_.WaitAll();
        CloseHandle(cancel_);
        bool sessionUsesDefaultRdp = false;
        for (const std::unique_ptr<Session>& session : sessions_) {
            UnregisterWaitEx(session->wait, INVALID_HANDLE_VALUE);
            CloseHandle(session->process);
            sessionUsesDefaultRdp = sessionUsesDefaultRdp || session->setDefaultRdp;
        }
        // Turned off during a session: put Default.rdp back now anyway.
        RestoreDefaultRdp(sessionUsesDefaultRdp);
    }

private:
    static constexpr UINT_PTR kDisplaysSettledTimer = 1;
    static constexpr UINT_PTR kCooldownTimer = 2;
    static constexpr UINT kIdsReadMessage = WM_APP + 1;
    static constexpr UINT kSessionEndedMessage = WM_APP + 2;
    // Keeps a button busy for a moment after starting a session, so a double-click starts one session.
    static constexpr UINT kStartCooldownMilliseconds = 2000;

    // A running session, watched for its end.
    struct Session {
        HWND replyTo;
        HANDLE process;
        HANDLE wait;
        std::wstring copy;
        bool setDefaultRdp;
    };

    struct IdsQuery {
        HWND replyTo;
        HANDLE cancel;
        Displays displays;
        std::wstring outputsKey;
        Choice chosen;
        std::wstring connectionKey;
        DisplayIds ids;
    };

    static DWORD WINAPI ReadIdsThread(void* parameter) {
        auto query = (IdsQuery*)parameter;
        HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        query->ids = QueryDisplayIds(query->displays, query->cancel);
        if (SUCCEEDED(com)) {
            CoUninitialize();
        }
        if (!PostMessageW(query->replyTo, kIdsReadMessage, 0, (LPARAM)query)) {
            delete query;
        }
        return 0;
    }

    // Runs on a thread pool thread when a session's Remote Desktop Connection exits.
    static VOID CALLBACK OnSessionExited(PVOID parameter, BOOLEAN) {
        auto session = (Session*)parameter;
        PostMessageW(session->replyTo, kSessionEndedMessage, 0, (LPARAM)session);
    }

    bool StartWidget() {
        if (!owner_.Create(L"RDPLauncher.Mod.Owner") || !popup_.Create()) {
            return false;
        }
        ForgetDraggedWidget();
        owner_.onSettingChange = []() {
            RefreshShellSettings();
            // Windows' accent color may have changed: the display map reads it again.
            TaskbarXaml::g_bridge->Post([]() { ForgetAccentColors(); });
        };
        owner_.onTaskbarCreated = []() { AttachIfMissing(); };
        owner_.onDisplayChange = [this]() {
            RefreshDisplays();
            RefreshWidget();
            // Monitor names can show up a moment later; look again once things have settled.
            SetTimer(messages_.Handle(), kDisplaysSettledTimer, 1500, nullptr);
        };
        RefreshDisplays();
        ConnectionChanged();
        // The widget may be in the taskbar already, or will say when it is.
        TaskbarXaml::g_appWindow = messages_.Handle();
        SendContent();
        return true;
    }

    // Tells the widget what to show.
    void SendContent() {
        TaskbarXaml::g_bridge->Post([content = content_]() { g_view->Show(content); });
    }

    void OnWidgetClick(int part, TaskbarXaml::MouseButton button) {
        if (button == TaskbarXaml::MouseButton::Right) {
            ShowMenu();
        } else if (button == TaskbarXaml::MouseButton::Left && part == kConnectionPart) {
            // Goes to the next computer (the menu is on right-click); before there are any, it's the
            // widget's "Click to add a computer".
            if (settings_.connections.empty()) {
                AddComputer();
            } else {
                StepConnection(1);
            }
        } else if (button == TaskbarXaml::MouseButton::Left) {
            ConnectWithButton(part - 1);
        }
    }

    void OnTimer(UINT_PTR timer) {
        if (timer == kDisplaysSettledTimer) {
            KillTimer(messages_.Handle(), kDisplaysSettledTimer);
            displayDetails_.clear();  // Reads the monitor names again.
            RefreshDisplays();
            RefreshWidget();
        } else if (timer == kCooldownTimer) {
            EndStart();
        }
    }

    void Save() {
        if (!settings_.Save()) {
            popup_.ShowMessage(L"Couldn't save settings", L"Windhawk couldn't store the mod's settings.");
        }
    }

    // Notices displays being added, removed or rearranged (told by WM_DISPLAYCHANGE), and picks the
    // display buttons for the new arrangement.
    void RefreshDisplays() {
        Displays current = GetDisplays();
        std::wstring key = SetupKey(current);
        std::wstring details = key + L"|";
        for (const LocalDisplay& display : current) {
            details += display.deviceName + L"#" + std::to_wstring(display.outputIndex) + L";";
        }
        if (details == displayDetails_) {
            return;
        }
        LoadDisplayNames(current);
        displays_ = current;
        displayDetails_ = details;
        if (key != setupKey_) {
            setupKey_ = key;
            auto saved = settings_.displayButtons.find(key);
            buttons_ = ChoicesForSetup(displays_, saved == settings_.displayButtons.end() ? nullptr : &saved->second);
        }
    }

    // Updates the widget when anything it shows has changed.
    void RefreshWidget() {
        Connection* connection = settings_.CurrentConnection();
        WidgetContent content;
        content.title = connection ? connection->Name() : L"Remote Desktop";
        // The connection's name over its address, or just the address when it has no other name.
        content.detail = !connection ? L"Click to add a computer"
                         : !connectionAddress_.empty() && !EqualsIgnoreCase(connectionAddress_, connection->Name()) ? connectionAddress_
                                                                                                                    : L"";
        content.displays = displays_;
        if (connection) {
            content.buttons = buttons_;
        }
        content.busyButton = busyButton_;

        std::wstring signature = content.title + L"\n" + content.detail + L"\n" + displayDetails_ + L"\n" +
                                 std::to_wstring(busyButton_);
        for (const Choice& button : content.buttons) {
            signature += L"\n" + Join(button, L";");
        }
        for (const LocalDisplay& display : displays_) {
            signature += L"\n" + display.name;
        }
        if (signature != lastSignature_) {
            lastSignature_ = signature;
            content_ = content;
            SendContent();
        }
    }

    // After switching to (or adding or removing) a connection.
    void ConnectionChanged() {
        Connection* connection = settings_.CurrentConnection();
        connectionAddress_ = connection ? connection->ReadAddress() : L"";
        RefreshWidget();
    }

    void ConnectWithButton(int button) {
        Connection* connection = settings_.CurrentConnection();
        if (!connection) {
            AddComputer();
            return;
        }
        if (button < 0 || button >= (int)buttons_.size() || starting_) {
            return;
        }
        starting_ = true;
        busyButton_ = button;
        RefreshWidget();

        Choice chosen = buttons_[button];
        std::vector<const LocalDisplay*> ordered = InSessionOrder(displays_, chosen);
        if (ordered.size() >= displays_.size()) {
            Finish(*connection, nullptr);  // All of them, which needs no IDs.
            return;
        }
        std::wstring outputsKey = OutputsKey(displays_);
        auto saved = settings_.displayIds.find(outputsKey);
        if ((saved != settings_.displayIds.end() && CoversAll(saved->second, displays_)) || unreadableOutputs_.count(outputsKey)) {
            DisplayIds ids = saved != settings_.displayIds.end() ? saved->second : DisplayIds{};
            bool guessed;
            std::vector<int> ordering = IdsInOrder(ordered, ids, guessed);
            Finish(*connection, &ordering);
            return;
        }

        // Read them from "mstsc /l" off the UI thread; the result comes back as kIdsReadMessage.
        auto query = new IdsQuery{messages_.Handle(), cancel_, displays_, outputsKey, chosen, connection->Key(), {}};
        if (!workers_.Start(ReadIdsThread, query)) {
            delete query;
            starting_ = false;
            busyButton_ = -1;
            RefreshWidget();
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
                if (wParam == kConnectionPart) {
                    StepConnection(-(int)lParam);
                }
                return;
            case TaskbarXaml::kMenuCommandMessage:
                if (auto menu = std::move(menu_)) {
                    menu->Run((int)wParam);
                }
                return;
            case kToggleDisplayMessage:
                if (lParam >= 0 && lParam < (LPARAM)displays_.size()) {
                    ToggleDisplay((int)wParam, displays_[lParam].Key());
                }
                return;
        }
        if (message == kSessionEndedMessage) {
            OnSessionEnded((Session*)lParam);
            return;
        }
        if (message != kIdsReadMessage) {
            return;
        }
        std::unique_ptr<IdsQuery> query((IdsQuery*)lParam);
        if (CoversAll(query->ids, query->displays)) {
            settings_.displayIds[query->outputsKey] = query->ids;
            Save();
        } else {
            unreadableOutputs_.insert(query->outputsKey);
            popup_.ShowMessage(L"Couldn't read Remote Desktop's display numbers",
                               L"Guessed them. To see why, turn on logging for the mod in Windhawk and try again.");
        }
        Connection* connection = settings_.FindConnection(query->connectionKey);
        if (!connection) {
            starting_ = false;
            busyButton_ = -1;
            RefreshWidget();
            return;
        }
        std::vector<const LocalDisplay*> ordered = InSessionOrder(query->displays, query->chosen);
        bool guessed;
        std::vector<int> ordering = IdsInOrder(ordered, query->ids, guessed);
        Finish(*connection, &ordering);
    }

    // Starts the session and keeps the button busy for a moment.
    void Finish(const Connection& connection, const std::vector<int>* displayIds) {
        StartedSession started;
        std::optional<std::wstring> error = StartFullScreen(connection, displayIds, started);
        if (started.process) {
            if (WatchSession(started) && started.setDefaultRdp) {
                std::vector<SessionMark> marks = LoadSessionMarks();
                marks.push_back({GetProcessId(started.process), ProcessStartTime(started.process)});
                SaveSessionMarks(marks);
            }
        } else if (started.setDefaultRdp) {
            RestoreDefaultRdpWhenNoSessions();
        }
        if (error) {
            popup_.ShowMessage(L"Couldn't start Remote Desktop", *error);
            EndStart();
            return;
        }
        SetTimer(messages_.Handle(), kCooldownTimer, kStartCooldownMilliseconds, nullptr);
    }

    // Watches for the session to end, to tidy up after it. Returns whether it's watched.
    bool WatchSession(const StartedSession& started) {
        auto session = std::make_unique<Session>(Session{messages_.Handle(), started.process, nullptr, started.copy, started.setDefaultRdp});
        if (!RegisterWaitForSingleObject(&session->wait, session->process, OnSessionExited, session.get(), INFINITE,
                                         WT_EXECUTEONLYONCE)) {
            // It can't be watched; the mod tidies up when it next starts.
            CloseHandle(session->process);
            return false;
        }
        sessions_.push_back(std::move(session));
        return true;
    }

    // Watches the sessions started with /v before the mod last stopped that are still running.
    // Returns whether there are any.
    bool WatchRunningSessions() {
        std::vector<SessionMark> running;
        for (const SessionMark& mark : LoadSessionMarks()) {
            StartedSession started;
            started.process = OpenRunningSession(mark);
            started.setDefaultRdp = true;
            if (started.process && WatchSession(started)) {
                running.push_back(mark);
            }
        }
        SaveSessionMarks(running);
        return !running.empty();
    }

    // A session ended: deletes its .rdp copy (unless another session uses one by the same name),
    // and puts Default.rdp's display settings back once no session needs them.
    void OnSessionEnded(Session* ended) {
        auto found = std::find_if(sessions_.begin(), sessions_.end(), [&](const std::unique_ptr<Session>& s) { return s.get() == ended; });
        if (found == sessions_.end()) {
            return;
        }
        std::unique_ptr<Session> session = std::move(*found);
        sessions_.erase(found);
        UnregisterWaitEx(session->wait, INVALID_HANDLE_VALUE);
        if (session->setDefaultRdp) {
            std::vector<SessionMark> marks = LoadSessionMarks();
            SessionMark ended{GetProcessId(session->process), ProcessStartTime(session->process)};
            marks.erase(std::remove(marks.begin(), marks.end(), ended), marks.end());
            SaveSessionMarks(marks);
        }
        CloseHandle(session->process);
        if (!session->copy.empty() && std::none_of(sessions_.begin(), sessions_.end(), [&](const std::unique_ptr<Session>& s) {
                return EqualsIgnoreCase(s->copy, session->copy);
            })) {
            DeleteFileW(session->copy.c_str());
        }
        if (session->setDefaultRdp) {
            RestoreDefaultRdpWhenNoSessions();
        }
    }

    void RestoreDefaultRdpWhenNoSessions() {
        if (std::none_of(sessions_.begin(), sessions_.end(), [](const std::unique_ptr<Session>& s) { return s->setDefaultRdp; })) {
            RestoreDefaultRdp(false);
        }
    }

    void EndStart() {
        KillTimer(messages_.Handle(), kCooldownTimer);
        starting_ = false;
        busyButton_ = -1;
        RefreshWidget();
    }

    // Adds the display to the button's displays, or takes it off (but never the last one).
    void ToggleDisplay(int button, const std::wstring& key) {
        if (button < 0 || button >= (int)buttons_.size() ||
            std::none_of(displays_.begin(), displays_.end(), [&](const LocalDisplay& d) { return d.Key() == key; })) {
            return;
        }
        Choice& choice = buttons_[button];
        auto found = std::find(choice.begin(), choice.end(), key);
        if (found != choice.end()) {
            if (choice.size() == 1) {
                return;
            }
            choice.erase(found);
        } else {
            choice.push_back(key);
        }
        settings_.displayButtons[setupKey_] = buttons_;
        Save();
        RefreshWidget();
    }

    void StepConnection(int steps) {
        int count = (int)settings_.connections.size();
        if (count < 2) {
            return;
        }
        int index = 0;
        Connection* current = settings_.CurrentConnection();
        for (int i = 0; i < count; i++) {
            if (current && settings_.connections[i].Key() == current->Key()) {
                index = i;
            }
        }
        int next = ((index + steps) % count + count) % count;
        SelectConnection(settings_.connections[next].Key());
    }

    void SelectConnection(const std::wstring& key) {
        settings_.currentConnectionKey = key;
        Save();
        ConnectionChanged();
    }

    void AddConnection(const Connection& connection) {
        settings_.AddConnection(connection);
        Save();
        ConnectionChanged();
    }

    void AddComputer() {
        std::optional<std::wstring> computer = PromptForText(
            L"Add a computer",
            L"Computer name or IP address, as you'd type it in Remote Desktop Connection "
            L"(for example workpc, workpc.contoso.com or 192.168.1.20:3390):",
            L"Add", L"", false, false);
        if (computer && !computer->empty()) {
            AddConnection(Connection{*computer, L"", L""});
        }
    }

    void AddRdpFile() {
        WCHAR path[MAX_PATH] = {};
        std::wstring documents = KnownFolder(FOLDERID_Documents);
        DialogOwner owner;
        OPENFILENAMEW open{sizeof(open)};
        open.hwndOwner = owner.Handle();
        open.lpstrFilter = L"Remote Desktop connections (*.rdp)\0*.rdp\0All files (*.*)\0*.*\0";
        open.lpstrFile = path;
        open.nMaxFile = MAX_PATH;
        open.lpstrInitialDir = documents.c_str();
        open.lpstrTitle = L"Add a saved .rdp file";
        open.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
        if (GetOpenFileNameW(&open)) {
            WCHAR full[MAX_PATH] = {};
            DWORD length = GetFullPathNameW(path, MAX_PATH, full, nullptr);
            AddConnection(Connection{L"", length > 0 && length < MAX_PATH ? full : path, L""});
        }
    }

    // Names the connection on the widget; an empty name goes back to showing its address.
    void RenameConnection(const std::wstring& key) {
        Connection* connection = settings_.FindConnection(key);
        if (!connection) {
            return;
        }
        std::wstring address = connection->ReadAddress();
        std::optional<std::wstring> name = PromptForText(
            L"Rename " + connection->Name(),
            L"Name to show for " + (address.empty() ? connection->Name() : address) + L" (leave it empty to show just the address):",
            L"Rename", connection->customName, true, true);
        connection = settings_.FindConnection(key);
        if (name && connection) {
            connection->customName = *name;
            Save();
            ConnectionChanged();
        }
    }

    // Forgets the display IDs for these displays, so they're read again on the next connection.
    void ReadDisplayIdsAgain() {
        std::wstring outputsKey = OutputsKey(displays_);
        settings_.displayIds.erase(outputsKey);
        unreadableOutputs_.erase(outputsKey);
        Save();
    }

    void ResetDisplayButtons() {
        settings_.displayButtons.erase(setupKey_);
        buttons_ = DefaultChoices(displays_);
        Save();
        RefreshWidget();
    }

    void ShowMenu() {
        ConnectionChanged();  // An .rdp file may have been edited since.
        Connection* current = settings_.CurrentConnection();
        std::wstring currentKey = current ? current->Key() : L"";

        menu_ = std::make_unique<Menu>();
        Menu& menu = *menu_;
        Menu& computers = menu.AddSubmenu(L"Computers");
        for (const Connection& connection : settings_.connections) {
            std::wstring key = connection.Key();
            std::wstring label = !connection.host.empty() && connection.Name() != connection.host
                                     ? connection.Name() + L"  (" + connection.host + L")"
                                     : connection.Name();
            computers.Add(label, [this, key]() { SelectConnection(key); }, key == currentKey);
        }
        if (!settings_.connections.empty()) {
            computers.AddSeparator();
        }
        computers.Add(L"Add a computer…", [this]() { AddComputer(); });
        computers.Add(L"Add a saved .rdp file…", [this]() { AddRdpFile(); });
        std::vector<std::wstring> recent;
        for (const std::wstring& computer : RecentComputers()) {
            if (std::none_of(settings_.connections.begin(), settings_.connections.end(),
                             [&](const Connection& c) { return EqualsIgnoreCase(c.host, computer); })) {
                recent.push_back(computer);
            }
        }
        if (!recent.empty()) {
            Menu& recentMenu = computers.AddSubmenu(L"Add a recent computer");
            for (const std::wstring& computer : recent) {
                recentMenu.Add(computer, [this, computer]() { AddConnection(Connection{computer, L"", L""}); });
            }
        }
        if (current) {
            computers.AddSeparator();
            computers.Add(L"Remove " + current->Name() + L" from the list", [this, currentKey]() {
                settings_.RemoveConnection(currentKey);
                Save();
                ConnectionChanged();
            });

            menu.Add(L"Rename " + current->Name() + L"…", [this, currentKey]() { RenameConnection(currentKey); });
            std::wstring editLabel = current->rdpFile.empty() ? L"Default Remote Desktop settings…"
                                                              : L"Edit " + FileNameOf(current->rdpFile) + L"…";
            Connection snapshot = *current;
            menu.Add(editLabel, [this, snapshot]() {
                if (!EditConnection(snapshot)) {
                    popup_.ShowMessage(L"Couldn't open Remote Desktop Connection", MstscPath());
                }
            });
        }
        menu.Add(L"Reset display buttons", [this]() { ResetDisplayButtons(); }, false, settings_.displayButtons.count(setupKey_) > 0);
        menu.Add(L"Read display numbers again", [this]() { ReadDisplayIdsAgain(); }, false, displays_.size() > 1);
        TaskbarXaml::g_bridge->Post([entries = menu.Entries()]() { TaskbarXaml::ShowMenu(entries); });
    }

    Settings settings_;
    MessageWindow messages_;
    OwnerWindow owner_;
    MessagePopup popup_;
    std::unique_ptr<Menu> menu_;  // The widget's menu, while it may be open.
    WidgetContent content_;       // What the widget shows.
    WorkerThreads workers_;
    HANDLE cancel_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);  // Set when the mod is turned off.
    Displays displays_;
    std::wstring setupKey_;
    std::wstring displayDetails_;  // Also changes when the same arrangement comes back on other outputs.
    std::vector<Choice> buttons_;
    std::wstring connectionAddress_;
    std::wstring lastSignature_;
    // Arrangements of outputs whose display IDs "mstsc /l" didn't give this time; they're guessed
    // instead of making every connection wait for it again.
    std::set<std::wstring> unreadableOutputs_;
    std::vector<std::unique_ptr<Session>> sessions_;
    bool starting_ = false;
    int busyButton_ = -1;
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
        auto app = std::make_shared<RdpApp>();
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
            TaskbarXaml::g_host.reset();
            g_view.reset();
            TaskbarXaml::g_bridge.reset();
        });
    }
}
