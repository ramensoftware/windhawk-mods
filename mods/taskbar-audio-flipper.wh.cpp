// ==WindhawkMod==
// @id              taskbar-audio-flipper
// @name            Taskbar Audio Flipper
// @description     A taskbar widget that switches between your speakers, headsets and microphones with a click, and turns a webcam on and off
// @version         1.0.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -loleaut32 -luuid -lsetupapi -lcfgmgr32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Audio Flipper

A taskbar widget, in the style of the Windows weather widget, for switching audio devices. It has
up to three sections, which you can each show or hide:

- **Output**: the active output device's icon, name and volume. Click it to switch to the next
  output (speakers → headset → monitor → …).
- **Input**: the same for your microphone.
- **Webcam**: the chosen webcam's name and whether it's on, with a camera icon. Click it to turn
  the webcam on or off.

On the Output and Input sections:

- **Hover**: a flyout opens above it with the device's name, a volume slider, and the device icon
  as a mute button, like the Windows 11 volume flyout.
- **Left-click**: switch to the next device.
- **Scroll**: turn the volume (or microphone level) up or down.
- **Middle-click**: mute / unmute.

After you change the output volume, the mod plays the Windows "Default Beep" ding at the new level,
like Windows' own volume slider. Turn it off with **Output → Ding when changing volume**.

Anywhere on the widget, **right-click** for the menu and **drag** left or right to move it along the
taskbar. The **Output**, **Input** and **Webcam** submenus each have a **Show on taskbar** switch for
their section, and the settings for it: picking a device directly, which devices are in the click
rotation, whether to switch the communications device too, and links to the Windows settings.

Devices are shown with the name and icon they have in the Windows Sound control panel. To change
them, choose **Rename or change device icons…** in the **Output** or **Input** submenu, right-click a
device, and choose **Properties**.

## Webcam on/off

Turn it on under **Webcam → Show on taskbar** and pick the webcam to control in the same menu. The
camera icon has a slash through it while the webcam is off; click it to switch. Windows has no way to
rename a webcam, but **Webcam → Rename…** gives it a name to show on the widget.

Turning the webcam off works like **Disable device** in Device Manager: no app can use it until it's
turned back on. Windows only lets administrators do that, so each switch shows a Windows prompt
asking to allow "PnP Utility" (Microsoft's `pnputil.exe`), unless Windhawk itself runs as
administrator.

## Linking with Taskbar RDP Launcher

With the Taskbar RDP Launcher mod running too, the two widgets can be joined into one: right-click
either, choose **Link with RDPLauncher** (or **Link with AudioFlipper**) and which side it goes on.
Linked, they sit side by side with a divider between them, dragging either moves both, and if one
hides the other takes its place.

## Settings

The sections, the click rotation and the webcam settings are kept in
`%APPDATA%\AudioFlipper\settings.txt`.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <propidl.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <taskschd.h>
#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace {

const wchar_t kSelfApp[] = L"AudioFlipper";
const wchar_t kPartnerApp[] = L"RDPLauncher";
const wchar_t kWidgetClassName[] = L"AudioFlipper.Mod.Widget";

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Shared by the AudioFlipper and RDPLauncher mods (the same code is in both):
// the taskbar widget window, popups, menus, the text prompt, the link between
// the two widgets, settings files and launching programs.

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


std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return {};
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    std::wstring wide(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), wide.data(), length);
    return wide;
}

std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) {
        return {};
    }
    int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string narrow(length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), narrow.data(), length, nullptr, nullptr);
    return narrow;
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

std::wstring SystemFolder() {
    WCHAR path[MAX_PATH];
    UINT length = GetSystemDirectoryW(path, MAX_PATH);
    return length > 0 && length < MAX_PATH ? std::wstring(path, length) : L"C:\\Windows\\System32";
}

std::wstring ExpandEnvironment(const std::wstring& text) {
    DWORD length = ExpandEnvironmentStringsW(text.c_str(), nullptr, 0);
    if (length == 0) {
        return text;
    }
    std::wstring expanded(length, L'\0');
    ExpandEnvironmentStringsW(text.c_str(), expanded.data(), length);
    expanded.resize(wcslen(expanded.c_str()));
    return expanded;
}


std::wstring DirectoryOf(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
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

// Writes over a file, keeping its attributes: Windows won't recreate a hidden
// file (like Default.rdp) in place, so it's unhidden meanwhile.
bool WriteFileBytes(const std::wstring& path, const std::string& bytes) {
    CreateDirectories(DirectoryOf(path));
    DWORD attributes = GetFileAttributesW(path.c_str());
    // Overwriting a hidden file needs the attribute cleared first (and put back after). A read-only
    // one stays read-only, so writing it fails.
    bool restore = attributes != INVALID_FILE_ATTRIBUTES && (attributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
    if (restore) {
        SetFileAttributesW(path.c_str(), attributes & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
    }
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    bool ok = file != INVALID_HANDLE_VALUE;
    if (ok) {
        DWORD written = 0;
        ok = bytes.empty() || (WriteFile(file, bytes.data(), (DWORD)bytes.size(), &written, nullptr) && written == bytes.size());
        CloseHandle(file);
    }
    if (restore) {
        SetFileAttributesW(path.c_str(), attributes);
    }
    return ok;
}

// Lines of a text file in UTF-8 or UTF-16 (with a byte order mark), as .NET reads them.
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

// Writes lines as UTF-8 without a byte order mark (as the apps' settings files are) or as UTF-16
// with one (as Remote Desktop Connection writes .rdp files).
bool WriteTextLines(const std::wstring& path, const std::vector<std::wstring>& lines, bool utf16) {
    std::wstring text;
    for (const std::wstring& line : lines) {
        text += line;
        text += L"\r\n";
    }
    if (utf16) {
        std::string bytes("\xFF\xFE", 2);
        bytes.append((const char*)text.data(), text.size() * sizeof(wchar_t));
        return WriteFileBytes(path, bytes);
    }
    return WriteFileBytes(path, WideToUtf8(text));
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

// .NET writes booleans as "True"/"False".
std::wstring BoolText(bool value) {
    return value ? L"True" : L"False";
}

// Menu text: "&" would underline the next letter.
std::wstring MenuText(const std::wstring& text) {
    std::wstring escaped;
    for (wchar_t c : text) {
        if (c == L'&') {
            escaped += L'&';
        }
        escaped += c;
    }
    return escaped;
}

// ---------------------------------------------------------------------------
// Windows settings and processes

bool ReadPersonalizeFlag(const wchar_t* valueName, bool fallback) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     valueName, RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS) {
        return fallback;
    }
    return value != 0;
}

// "Choose your default Windows mode", which the taskbar follows.
bool TaskbarUsesLightTheme() {
    return ReadPersonalizeFlag(L"SystemUsesLightTheme", false);
}

// "Choose your default app mode", which the popups follow.
bool AppsUseLightTheme() {
    return ReadPersonalizeFlag(L"AppsUseLightTheme", true);
}

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

struct LaunchOptions {
    bool hidden = false;
    // Windhawk may run as administrator; programs opened for the user shouldn't, so they're
    // started as children of the desktop (Explorer), which gives them its ordinary rights.
    bool asUser = true;
};

// Starts a program. Returns its process handle (to close) in *process when asked for.
bool Launch(const std::wstring& exe, const std::wstring& arguments, const LaunchOptions& options,
            HANDLE* process = nullptr, DWORD* processId = nullptr) {
    std::wstring commandLine = L"\"" + exe + L"\"" + (arguments.empty() ? L"" : L" " + arguments);
    std::vector<wchar_t> commandBuffer(commandLine.begin(), commandLine.end());
    commandBuffer.push_back(L'\0');

    STARTUPINFOEXW startup{};
    startup.StartupInfo.cb = sizeof(STARTUPINFOW);
    if (options.hidden) {
        startup.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
        startup.StartupInfo.wShowWindow = SW_HIDE;
    }

    HANDLE parent = nullptr;
    std::vector<BYTE> attributeBuffer;
    DWORD flags = 0;
    if (options.asUser && IsProcessElevated()) {
        DWORD shellProcessId = 0;
        if (HWND shell = GetShellWindow()) {
            GetWindowThreadProcessId(shell, &shellProcessId);
        }
        if (shellProcessId) {
            parent = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, shellProcessId);
        }
        SIZE_T size = 0;
        if (parent) {
            InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
            attributeBuffer.resize(size);
            auto attributes = (LPPROC_THREAD_ATTRIBUTE_LIST)attributeBuffer.data();
            if (InitializeProcThreadAttributeList(attributes, 1, 0, &size)) {
                if (UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parent,
                                              sizeof(parent), nullptr, nullptr)) {
                    startup.lpAttributeList = attributes;
                    startup.StartupInfo.cb = sizeof(STARTUPINFOEXW);
                    flags |= EXTENDED_STARTUPINFO_PRESENT;
                } else {
                    DeleteProcThreadAttributeList(attributes);
                }
            }
        }
    }

    PROCESS_INFORMATION info{};
    BOOL started = CreateProcessW(exe.c_str(), commandBuffer.data(), nullptr, nullptr, FALSE, flags, nullptr,
                                  nullptr, &startup.StartupInfo, &info);
    if (startup.lpAttributeList) {
        DeleteProcThreadAttributeList(startup.lpAttributeList);
    }
    if (parent) {
        CloseHandle(parent);
    }
    if (!started) {
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

// Opens a document, a URI like ms-settings:sound, or a program, as Explorer would.
bool OpenWithShell(const std::wstring& target, const std::wstring& arguments = L"") {
    return (INT_PTR)ShellExecuteW(nullptr, L"open", target.c_str(), arguments.empty() ? nullptr : arguments.c_str(),
                                  nullptr, SW_SHOWNORMAL) > 32;
}

// A mutex both the mod and the app's exe use to keep to one widget. Anyone may open it, so the
// exe (not running as administrator) still sees one the mod made while Windhawk is.
class SingleInstance {
public:
    explicit SingleInstance(const wchar_t* name) : name_(name) {}

    ~SingleInstance() {
        if (mutex_) {
            if (owned_) {
                ReleaseMutex(mutex_);
            }
            CloseHandle(mutex_);
        }
    }

    // True once this is the only copy; false while the app's exe is running.
    bool TryAcquire() {
        if (owned_) {
            return true;
        }
        if (!mutex_) {
            PSECURITY_DESCRIPTOR descriptor = nullptr;
            SECURITY_ATTRIBUTES security{sizeof(security)};
            if (ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;GA;;;WD)(A;;GA;;;SY)S:(ML;;NW;;;LW)",
                                                                     SDDL_REVISION_1, &descriptor, nullptr)) {
                security.lpSecurityDescriptor = descriptor;
            }
            mutex_ = CreateMutexW(descriptor ? &security : nullptr, FALSE, name_);
            DWORD error = GetLastError();
            if (descriptor) {
                LocalFree(descriptor);
            }
            if (!mutex_) {
                // The exe running as administrator made the mutex, which only administrators may
                // open; anything else, carry on without it.
                return error != ERROR_ACCESS_DENIED;
            }
        }
        DWORD result = WaitForSingleObject(mutex_, 0);
        owned_ = result == WAIT_OBJECT_0 || result == WAIT_ABANDONED;
        return owned_;
    }

private:
    const wchar_t* name_;
    HANDLE mutex_ = nullptr;
    bool owned_ = false;
};

// ---------------------------------------------------------------------------
// Taskbar

HWND TaskbarWindow() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}

// The main taskbar and its monitor, when the taskbar is horizontal and on screen (not slid away
// by auto-hide).
bool TryGetVisibleTaskbar(RECT& bar, RECT& monitor) {
    HWND window = TaskbarWindow();
    if (!window || !IsWindowVisible(window) || !GetWindowRect(window, &bar)) {
        return false;
    }
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &info)) {
        return false;
    }
    monitor = info.rcMonitor;
    int width = bar.right - bar.left;
    int height = bar.bottom - bar.top;
    RECT visible{};
    IntersectRect(&visible, &bar, &monitor);
    return width > height && height > 0 && visible.bottom - visible.top >= height - 2;
}

// Whether the taskbar icons are left-aligned rather than centered (a Windows 11 setting).
bool TaskbarIconsLeftAligned() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

// Whether a full-screen app (game, video, presentation) is in front on the monitor.
// shellReportsFullScreen is Windows' own verdict, the notification that makes the taskbar step
// aside; screen overlays such as screenshot tools cover the screen too, but Windows doesn't
// count them, so neither does this.
bool IsFullScreenAppActive(const RECT& monitor, HWND ignore, bool shellReportsFullScreen) {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) && state == QUNS_RUNNING_D3D_FULL_SCREEN) {
        return true;
    }
    if (!shellReportsFullScreen) {
        return false;
    }
    // The notification doesn't say which monitor, so also check the app in front covers this one.
    HWND window = GetForegroundWindow();
    RECT rect;
    if (!window || window == ignore || !GetWindowRect(window, &rect)) {
        return false;
    }
    WCHAR className[256] = {};
    GetClassNameW(window, className, ARRAYSIZE(className));
    for (const wchar_t* shellClass : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        if (wcscmp(className, shellClass) == 0) {
            return false;
        }
    }
    return rect.left <= monitor.left && rect.top <= monitor.top && rect.right >= monitor.right &&
           rect.bottom >= monitor.bottom;
}

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

gp::Color WithAlpha(int alpha, const gp::Color& color) {
    return gp::Color((BYTE)alpha, color.GetR(), color.GetG(), color.GetB());
}

void AddRoundedRect(gp::GraphicsPath& path, const gp::RectF& rect, float radius) {
    float diameter = std::max(0.1f, std::min(radius * 2, std::min(rect.Width, rect.Height)));
    path.AddArc(rect.X, rect.Y, diameter, diameter, 180, 90);
    path.AddArc(rect.GetRight() - diameter, rect.Y, diameter, diameter, 270, 90);
    path.AddArc(rect.GetRight() - diameter, rect.GetBottom() - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.X, rect.GetBottom() - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
}

void FillRoundedRect(gp::Graphics& graphics, const gp::Color& color, const gp::RectF& rect, float radius) {
    if (rect.Width <= 0 || rect.Height <= 0) {
        return;
    }
    gp::GraphicsPath path;
    AddRoundedRect(path, rect, radius);
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
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

// The width DrawTextLine needs for the text, padding included.
float MeasureTextWidth(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font) {
    gp::StringFormat format(gp::StringFormatFlagsNoWrap | gp::StringFormatFlagsNoClip);
    gp::RectF bounds;
    graphics.MeasureString(text.c_str(), (INT)text.size(), &font, gp::PointF(0, 0), &format, &bounds);
    return bounds.Width;
}

std::unique_ptr<gp::Font> MakeFont(const wchar_t* family, float pixels) {
    return std::make_unique<gp::Font>(family, pixels, gp::FontStyleRegular, gp::UnitPixel);
}

// Popup colors for the "default app mode" (light or dark) chosen in Windows.
struct Palette {
    gp::Color background, foreground, secondary, accent, track, border, hover, thumbRing, onAccent;

    static Palette Current() {
        if (AppsUseLightTheme()) {
            return {Argb(255, 243, 243, 243), Argb(255, 27, 27, 27), Argb(255, 96, 96, 96),
                    Argb(255, 0, 95, 184), Argb(255, 196, 196, 196), Argb(255, 213, 213, 213),
                    Argb(14, 0, 0, 0), Argb(255, 255, 255, 255), Argb(255, 255, 255, 255)};
        }
        return {Argb(255, 32, 32, 32), Argb(255, 255, 255, 255), Argb(255, 197, 197, 197),
                Argb(255, 96, 205, 255), Argb(255, 80, 80, 80), Argb(255, 58, 58, 58),
                Argb(16, 255, 255, 255), Argb(255, 69, 69, 69), Argb(255, 0, 0, 0)};
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

// Registers a window class whose windows forward their messages to an object of type T, passed
// as the creation parameter.
template <typename T>
const wchar_t* RegisterForwardingClass(const wchar_t* name) {
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = name;
    windowClass.lpfnWndProc = ForwardingWindowProc<T>;
    RegisterClassExW(&windowClass);  // Fails harmlessly when it's already registered.
    return name;
}

// A hidden window for timers and for results posted back from worker threads.
class MessageWindow {
public:
    std::function<void(UINT message, WPARAM wParam, LPARAM lParam)> onMessage;
    std::function<void(UINT_PTR timer)> onTimer;

    bool Create(const wchar_t* className) {
        CreateWindowExW(0, RegisterForwardingClass<MessageWindow>(className), L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                        nullptr, GetModuleHandleW(nullptr), this);
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

    RECT Bounds() const {
        RECT rect{};
        if (hwnd_) {
            GetWindowRect(hwnd_, &rect);
        }
        return rect;
    }

    void Hide() {
        if (Visible()) {
            ShowWindow(hwnd_, SW_HIDE);
            OnHidden();
        }
    }

    void Invalidate() {
        if (hwnd_) {
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
    }

    float Scale() const { return scale_; }

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
            case WM_MOUSEMOVE:
                if (!trackingMouse_) {
                    TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd_, 0};
                    trackingMouse_ = TrackMouseEvent(&track);
                    OnMouseEnter();
                }
                OnMouseMove({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_MOUSELEAVE:
                trackingMouse_ = false;
                OnMouseLeave();
                return true;
            case WM_LBUTTONDOWN:
                SetCapture(hwnd_);
                OnMouseDown({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_LBUTTONUP:
                if (GetCapture() == hwnd_) {
                    ReleaseCapture();
                }
                OnMouseUp({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_MOUSEWHEEL:
                OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
                return true;
            case WM_TIMER:
                OnTimer(wParam);
                return true;
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    SetCursor(LoadCursorW(nullptr, handCursor_ ? IDC_HAND : IDC_ARROW));
                    result = TRUE;
                    return true;
                }
                return false;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

protected:
    bool CreatePopup(const wchar_t* className) {
        CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                        RegisterForwardingClass<PopupWindow>(className), L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                        GetModuleHandleW(nullptr), this);
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

    void SetHandCursor(bool hand) { handCursor_ = hand; }

    virtual void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) = 0;
    virtual void OnScaleChanged() {}
    virtual void OnHidden() {}
    virtual void OnMouseEnter() {}
    virtual void OnMouseMove(POINT point) {}
    virtual void OnMouseLeave() {}
    virtual void OnMouseDown(POINT point) {}
    virtual void OnMouseUp(POINT point) {}
    virtual void OnWheel(int delta) {}
    virtual void OnTimer(UINT_PTR timer) {}

    float scale_ = 1;
    bool roundedByWindows_ = false;
    bool trackingMouse_ = false;
    bool handCursor_ = false;
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
// Menus

// A popup menu built from items with actions; Show runs the chosen item's action.
class Menu {
public:
    Menu() : menu_(CreatePopupMenu()), actions_(std::make_shared<std::vector<std::function<void()>>>()) {}

    ~Menu() {
        if (owner_ && menu_) {
            DestroyMenu(menu_);
        }
    }

    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    void Add(const std::wstring& text, std::function<void()> action, bool checked = false, bool enabled = true) {
        actions_->push_back(std::move(action));
        UINT flags = MF_STRING | (checked ? MF_CHECKED : 0) | (enabled ? 0 : MF_GRAYED);
        AppendMenuW(menu_, flags, actions_->size(), MenuText(text).c_str());
    }

    // A line of text that can't be clicked, as a heading.
    void AddHeading(const std::wstring& text) { AppendMenuW(menu_, MF_STRING | MF_GRAYED, 0, MenuText(text).c_str()); }

    void AddSeparator() { AppendMenuW(menu_, MF_SEPARATOR, 0, nullptr); }

    // A submenu; items added to it share this menu's actions.
    Menu& AddSubmenu(const std::wstring& text, bool enabled = true) {
        submenus_.push_back(std::unique_ptr<Menu>(new Menu(actions_)));
        Menu& submenu = *submenus_.back();
        AppendMenuW(menu_, MF_POPUP | (enabled ? 0 : MF_GRAYED), (UINT_PTR)submenu.menu_, MenuText(text).c_str());
        return submenu;
    }

    // Shows the menu above the point (the widget's top edge) and runs the chosen action.
    void Show(HWND owner, POINT point) {
        // Like a tray icon does, take the foreground first so the menu closes when clicking elsewhere.
        SetForegroundWindow(owner);
        UINT command = TrackPopupMenuEx(menu_, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                                        point.x, point.y, owner, nullptr);
        PostMessageW(owner, WM_NULL, 0, 0);
        if (command > 0 && command <= actions_->size() && (*actions_)[command - 1]) {
            auto actions = actions_;  // The action may rebuild menus.
            (*actions)[command - 1]();
        }
    }

private:
    explicit Menu(std::shared_ptr<std::vector<std::function<void()>>> actions)
        : menu_(CreatePopupMenu()), actions_(std::move(actions)), owner_(false) {}

    HMENU menu_;
    std::shared_ptr<std::vector<std::function<void()>>> actions_;
    std::vector<std::unique_ptr<Menu>> submenus_;
    bool owner_ = true;  // Submenus are destroyed with the menu they're in.
};

// ---------------------------------------------------------------------------
// Dialogs

// An invisible, always-on-top window in the middle of the screen with the mouse, to open dialogs
// over: they then come up in front, where the mouse is, rather than behind other windows.
class DialogOwner {
public:
    DialogOwner() {
        // Like a WinForms dialog, keeps the thread's other windows (the widget and its popups) from
        // taking clicks while it's open.
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
// The link between AudioFlipper's and RDPLauncher's widgets (the same in both, and in both apps'
// exes). Linked, they sit side by side as one widget, in the order chosen, with a divider
// between them, and dragging either moves both.
//
// The link (on or off, which app is on the left, and where the pair is) is kept in
// HKCU\Software\TaskbarWidgetLink, which both read. Each widget window is titled
// "<app> taskbar widget" so the other can find it, read its width and post it a message whenever
// something changes; each also re-checks on its own placement timer.

namespace WidgetLink {

const wchar_t kKeyPath[] = L"Software\\TaskbarWidgetLink";

struct State {
    bool linked = false;
    std::wstring leftApp;  // "AudioFlipper" or "RDPLauncher".
    int offset = -1;       // The pair's left edge from the taskbar's left edge; negative for the default.

    bool SelfOnLeft() const { return leftApp != kPartnerApp; }
};

std::wstring WindowTitle(const wchar_t* app) {
    return std::wstring(app) + L" taskbar widget";
}

// Posted to the other widget when the link, or this widget's size or place, changes.
UINT ChangedMessage() {
    static UINT message = RegisterWindowMessageW(L"TaskbarWidgetLink.Changed");
    return message;
}

State Read() {
    State state;
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kKeyPath, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return state;
    }
    DWORD linked = 0;
    DWORD size = sizeof(linked);
    if (RegGetValueW(key, nullptr, L"Linked", RRF_RT_REG_DWORD, nullptr, &linked, &size) == ERROR_SUCCESS) {
        state.linked = linked != 0;
    }
    WCHAR text[64] = {};
    size = sizeof(text);
    if (RegGetValueW(key, nullptr, L"Left", RRF_RT_REG_SZ, nullptr, text, &size) == ERROR_SUCCESS) {
        state.leftApp = text;
    }
    text[0] = 0;
    size = sizeof(text);
    int offset;
    if (RegGetValueW(key, nullptr, L"Offset", RRF_RT_REG_SZ, nullptr, text, &size) == ERROR_SUCCESS && ParseInt(text, offset)) {
        state.offset = offset;
    }
    RegCloseKey(key);
    return state;
}

void WriteOffset(int offset) {
    std::wstring text = std::to_wstring(offset);
    RegSetKeyValueW(HKEY_CURRENT_USER, kKeyPath, L"Offset", REG_SZ, text.c_str(), (DWORD)((text.size() + 1) * sizeof(wchar_t)));
}

void Write(bool linked, const std::wstring& leftApp, int offset) {
    DWORD value = linked ? 1 : 0;
    RegSetKeyValueW(HKEY_CURRENT_USER, kKeyPath, L"Linked", REG_DWORD, &value, sizeof(value));
    RegSetKeyValueW(HKEY_CURRENT_USER, kKeyPath, L"Left", REG_SZ, leftApp.c_str(), (DWORD)((leftApp.size() + 1) * sizeof(wchar_t)));
    WriteOffset(offset);
}

HWND FindPartner() {
    return FindWindowW(nullptr, WindowTitle(kPartnerApp).c_str());
}

// Whether the other app (or its mod) is running with a version that can link.
bool PartnerRunning() {
    return FindPartner() != nullptr;
}

// The other app's widget, when it's on the taskbar right now.
bool TryGetPartnerBounds(RECT& bounds) {
    HWND window = FindPartner();
    return window && IsWindowVisible(window) && GetWindowRect(window, &bounds) && bounds.right > bounds.left;
}

void NotifyPartner() {
    if (HWND window = FindPartner()) {
        PostMessageW(window, ChangedMessage(), 0, 0);
    }
}

// Where a linked widget goes: the pair starts at groupOffset (kept on the taskbar), with the left
// app first. Both apps work it out the same way from the same numbers, so they meet without a
// gap. Without the other widget showing, this one takes the pair's place.
int PlaceInGroup(int barLeft, int barWidth, int ownWidth, int partnerWidth, bool selfOnLeft, int groupOffset, int& groupLeft) {
    int total = std::min(ownWidth + partnerWidth, barWidth);
    groupLeft = barLeft + std::max(0, std::min(groupOffset, barWidth - total));
    return selfOnLeft || partnerWidth == 0 ? groupLeft : std::min(groupLeft + partnerWidth, barLeft + barWidth - ownWidth);
}

}  // namespace WidgetLink

// ---------------------------------------------------------------------------
// The taskbar widget

// A taskbar button in the style of the Windows weather widget. Windows 11 has no API for adding
// things to the taskbar, so this is a transparent, always-on-top window kept just above the
// taskbar. It moves back above the taskbar when the taskbar is clicked, and hides when the
// taskbar auto-hides or a full-screen app is in front. To know about full-screen apps it registers
// as an app bar (without reserving any screen space), the same way the taskbar learns to step
// aside for them. Drag it to move it along the taskbar.
//
// Subclasses lay out and draw the content and handle clicks.
class TaskbarWidget {
public:
    virtual ~TaskbarWidget() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        if (foregroundHook_) {
            UnhookWinEvent(foregroundHook_);
        }
        if (widgetForHook_ == this) {
            widgetForHook_ = nullptr;
        }
    }

    // Called with the widget's new offset after it's been dragged (while not linked).
    std::function<void(int offset)> onOffsetChanged;

    bool Create(int offset) {
        offset_ = offset;
        std::wstring title = WidgetLink::WindowTitle(kSelfApp);
        CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                        RegisterForwardingClass<TaskbarWidget>(kWidgetClassName), title.c_str(), WS_POPUP, 0, 0, 1, 1,
                        nullptr, nullptr, GetModuleHandleW(nullptr), this);
        if (!hwnd_) {
            return false;
        }
        // Windhawk may run as administrator; let the other widget's and the shell's messages through.
        ChangeWindowMessageFilterEx(hwnd_, WidgetLink::ChangedMessage(), MSGFLT_ALLOW, nullptr);
        appBarMessage_ = RegisterWindowMessageW((std::wstring(kSelfApp) + L".AppBarNotification").c_str());
        ChangeWindowMessageFilterEx(hwnd_, appBarMessage_, MSGFLT_ALLOW, nullptr);
        APPBARDATA data{sizeof(data)};
        data.hWnd = hwnd_;
        data.uCallbackMessage = appBarMessage_;
        SHAppBarMessage(ABM_NEW, &data);
        return true;
    }

    void Destroy() {
        if (!hwnd_) {
            return;
        }
        SetShown(false);
        APPBARDATA data{sizeof(data)};
        data.hWnd = hwnd_;
        SHAppBarMessage(ABM_REMOVE, &data);
        DestroyWindow(hwnd_);
    }

    HWND Handle() const { return hwnd_; }

    int Offset() const { return offset_; }

    bool Visible() const { return hwnd_ && IsWindowVisible(hwnd_); }

    RECT Bounds() const {
        RECT rect{};
        GetWindowRect(hwnd_, &rect);
        return rect;
    }

    // Shows or hides the widget; it only watches the taskbar while it's meant to be shown.
    void SetShown(bool shown) {
        if (shown == shown_) {
            return;
        }
        shown_ = shown;
        if (shown) {
            SetTimer(hwnd_, kPlacementTimer, 250, nullptr);
            if (!foregroundHook_) {
                widgetForHook_ = this;
                foregroundHook_ = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                                                  OnForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
            }
        } else {
            KillTimer(hwnd_, kPlacementTimer);
            if (foregroundHook_) {
                UnhookWinEvent(foregroundHook_);
                foregroundHook_ = nullptr;
            }
        }
        UpdatePlacement(true);
    }

    // Re-renders after the content changed (and resizes if its width did).
    void ContentChanged() { UpdatePlacement(true); }

    // Links this widget with the other app's, with this one on the given side, or unlinks them.
    void SetLink(bool linked, bool selfOnLeft) {
        WidgetLink::State current = WidgetLink::Read();
        int groupOffset = current.offset;
        if (linked && !current.linked) {
            // Start the pair where the leftmost of the two widgets is now.
            RECT bounds = Bounds();
            int left = Visible() ? bounds.left : taskbar_.left + std::max(0, offset_);
            RECT partner;
            if (WidgetLink::TryGetPartnerBounds(partner)) {
                left = Visible() ? std::min(left, (int)partner.left) : (int)partner.left;
            }
            groupOffset = std::max(0, left - (int)taskbar_.left);
        }
        WidgetLink::Write(linked, selfOnLeft ? kSelfApp : kPartnerApp, groupOffset);
        UpdatePlacement(true);
        WidgetLink::NotifyPartner();
    }

    float Scale() const { return scale_; }

    int Px(float value) const { return (int)std::nearbyint(value * scale_); }  // Halves to even, like the apps' Math.Round.

    // Screen bounds of a part of the widget, from client x and width.
    RECT ScreenBoundsOf(int x, int width) const {
        RECT bounds = Bounds();
        return {bounds.left + x, bounds.top, bounds.left + x + width, bounds.bottom};
    }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        if (message == WidgetLink::ChangedMessage()) {
            // The other app's widget moved, changed size, or changed the link.
            UpdatePlacement(false);
            return true;
        }
        if (appBarMessage_ && message == appBarMessage_) {
            // Windows says a full-screen app opened or closed (lParam is nonzero while one is open).
            if (wParam == ABN_FULLSCREENAPP) {
                shellReportsFullScreen_ = lParam != 0;
                UpdatePlacement(false);
            }
            return true;
        }
        switch (message) {
            case WM_MOUSEACTIVATE:
                result = MA_NOACTIVATE;
                return true;
            case WM_TIMER:
                if (wParam == kPlacementTimer) {
                    UpdatePlacement(false);
                } else {
                    OnTimer(wParam);
                }
                return true;
            case WM_MOUSEMOVE: {
                POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (!trackingMouse_) {
                    TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd_, 0};
                    trackingMouse_ = TrackMouseEvent(&track);
                }
                if (!pressed_) {
                    OnHover(point);
                    return true;
                }
                POINT cursor;
                GetCursorPos(&cursor);
                int distance = cursor.x - dragStartCursorX_;
                if (!dragging_ && std::abs(distance) < GetSystemMetrics(SM_CXDRAG)) {
                    return true;
                }
                if (!dragging_) {
                    dragging_ = true;
                    OnDragStart();
                }
                int dragged = std::max(0, dragStartOffset_ + distance);
                if (wasLinked_) {
                    WidgetLink::WriteOffset(dragged);  // Moves the pair; the other widget follows when told.
                } else {
                    offset_ = dragged;
                }
                UpdatePlacement(true);
                return true;
            }
            case WM_MOUSELEAVE:
                trackingMouse_ = false;
                OnHoverLeave();
                return true;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK: {
                SetCapture(hwnd_);
                POINT cursor;
                GetCursorPos(&cursor);
                pressed_ = true;
                dragging_ = false;
                dragStartCursorX_ = cursor.x;
                dragStartOffset_ = groupLeft_ - taskbar_.left;
                return true;
            }
            case WM_LBUTTONUP: {
                bool wasPressed = pressed_;
                bool wasDragging = dragging_;
                pressed_ = false;
                dragging_ = false;
                if (GetCapture() == hwnd_) {
                    ReleaseCapture();
                }
                if (wasDragging) {
                    EndDrag();
                } else if (wasPressed) {
                    POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    RECT client;
                    GetClientRect(hwnd_, &client);
                    // Pressing and sliding off the widget before letting go doesn't click it.
                    if (PtInRect(&client, point)) {
                        OnClick(point, MouseButton::Left);
                    }
                }
                return true;
            }
            case WM_CAPTURECHANGED:
                // Something else took the mouse mid-press (a menu opened by a right-click): the press
                // ends there, and a drag stays where it got to.
                if ((HWND)lParam != hwnd_ && pressed_) {
                    bool wasDragging = dragging_;
                    pressed_ = false;
                    dragging_ = false;
                    if (wasDragging) {
                        EndDrag();
                    }
                }
                return false;
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
                OnClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)},
                        message == WM_MBUTTONUP ? MouseButton::Middle : MouseButton::Right);
                return true;
            case WM_MOUSEWHEEL: {
                // Precision touchpads send fractions of a notch; add them up.
                wheelRemainder_ += GET_WHEEL_DELTA_WPARAM(wParam);
                int notches = wheelRemainder_ / WHEEL_DELTA;
                wheelRemainder_ -= notches * WHEEL_DELTA;
                if (notches != 0) {
                    POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    ScreenToClient(hwnd_, &point);
                    OnWheel(point, notches);
                }
                return true;
            }
            case WM_DISPLAYCHANGE:
            case WM_SETTINGCHANGE:
                UpdatePlacement(true);
                return false;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

protected:
    enum class MouseButton { Left, Middle, Right };

    // The widget's content width, in pixels, for the current scale.
    virtual int LayOut() = 0;
    // Draws the content (the background, drag highlight and link divider are drawn already).
    virtual void Draw(gp::Graphics& graphics, int width, int height, bool lightTaskbar) = 0;
    // Where the widget goes when it hasn't been dragged anywhere yet.
    virtual int DefaultOffset(int barWidth, int width) = 0;
    virtual void OnScaleChanged() {}
    virtual void OnHover(POINT point) {}
    virtual void OnHoverLeave() {}
    virtual void OnClick(POINT point, MouseButton button) {}
    virtual void OnWheel(POINT point, int notches) {}
    virtual void OnDragStart() {}
    virtual void OnTimer(UINT_PTR timer) {}
    // After the widget has been placed (to move popups anchored to it), or hidden.
    virtual void OnPlaced() {}
    virtual void OnHidden() {}

    bool Dragging() const { return dragging_; }

    bool Pressed() const { return pressed_; }

    bool LightTaskbar() const { return lightTaskbar_; }

    // Moves the widget onto the taskbar and above it, or hides it when it shouldn't show.
    void UpdatePlacement(bool contentChanged) {
        if (!hwnd_) {
            return;
        }
        RECT bar;
        RECT monitor;
        if (!shown_ || !TryGetVisibleTaskbar(bar, monitor) || IsFullScreenAppActive(monitor, hwnd_, shellReportsFullScreen_)) {
            if (IsWindowVisible(hwnd_)) {
                ShowWindow(hwnd_, SW_HIDE);
                if (WidgetLink::Read().linked) {
                    WidgetLink::NotifyPartner();  // So it takes the pair's place.
                }
            }
            OnHidden();
            return;
        }

        // Scale with the taskbar's display.
        UINT dpi = GetDpiForWindow(TaskbarWindow());
        if (dpi == 0) {
            dpi = DpiForPoint({(bar.left + bar.right) / 2, (bar.top + bar.bottom) / 2});
        }
        float scale = dpi / 96.0f;
        if (scale > 0 && scale != scale_) {
            scale_ = scale;
            OnScaleChanged();
            contentChanged = true;
        }
        bool light = TaskbarUsesLightTheme();
        if (light != lightTaskbar_) {
            lightTaskbar_ = light;
            contentChanged = true;
        }

        int barWidth = bar.right - bar.left;
        int width = std::min(LayOut(), barWidth);
        taskbar_ = bar;

        WidgetLink::State link = WidgetLink::Read();
        RECT bounds = Bounds();
        if (wasLinked_ && !link.linked && IsWindowVisible(hwnd_)) {
            // Just unlinked: stay where the pair left this widget.
            offset_ = std::max(0, (int)(bounds.left - bar.left));
            if (onOffsetChanged) {
                onOffsetChanged(offset_);
            }
        }
        wasLinked_ = link.linked;

        int x;
        int partnerWidth = 0;
        if (link.linked) {
            // Side by side with the other app's widget, wherever the pair was put.
            RECT partner;
            if (WidgetLink::TryGetPartnerBounds(partner)) {
                partnerWidth = partner.right - partner.left;
            }
            int groupOffset = link.offset >= 0 ? link.offset : DefaultOffset(barWidth, width + partnerWidth);
            x = WidgetLink::PlaceInGroup(bar.left, barWidth, width, partnerWidth, link.SelfOnLeft(), groupOffset, groupLeft_);
        } else {
            int preferredOffset = offset_ >= 0 ? offset_ : DefaultOffset(barWidth, width);
            x = bar.left + std::max(0, std::min(preferredOffset, barWidth - width));
            groupLeft_ = x;
        }
        bool divider = partnerWidth > 0 && !link.SelfOnLeft();
        RECT target{x, bar.top, x + width, bar.bottom};

        bool moved = !EqualRect(&target, &bounds) || !IsWindowVisible(hwnd_);
        if (contentChanged || moved || divider != dividerAtLeft_) {
            dividerAtLeft_ = divider;
            Render(target);
        }
        if (!IsWindowVisible(hwnd_)) {
            ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
        }
        if (moved && link.linked) {
            WidgetLink::NotifyPartner();  // So it moves up against this widget.
        }
        OnPlaced();
        StayAboveTaskbar();
    }

    void Render() { Render(Bounds()); }

private:
    static constexpr UINT_PTR kPlacementTimer = 1;

    // Keeps where a drag left the widget (or the pair).
    void EndDrag() {
        if (wasLinked_) {
            WidgetLink::WriteOffset(groupLeft_ - taskbar_.left);  // Where the pair ended up after clamping.
            Render();
        } else {
            offset_ = Bounds().left - taskbar_.left;  // Where it ended up after clamping.
            Render();
            if (onOffsetChanged) {
                onOffsetChanged(offset_);
            }
        }
    }

    void Render(const RECT& bounds) {
        int width = bounds.right - bounds.left;
        int height = bounds.bottom - bounds.top;
        if (width <= 0 || height <= 0) {
            return;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (bitmap && bits) {
            HGDIOBJ previous = SelectObject(memory, bitmap);
            {
                gp::Bitmap canvas(width, height, width * 4, PixelFormat32bppPARGB, (BYTE*)bits);
                gp::Graphics graphics(&canvas);
                // Nearly invisible, but fully transparent pixels would let clicks fall through to the taskbar.
                graphics.Clear(Argb(1, 0, 0, 0));
                graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
                graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
                gp::Color foreground = lightTaskbar_ ? Argb(255, 28, 28, 28) : Argb(255, 255, 255, 255);
                if (dragging_) {
                    FillRoundedRect(graphics, WithAlpha(lightTaskbar_ ? 18 : 24, foreground),
                                    gp::RectF((float)Px(2), (float)Px(4), (float)(width - Px(4)), (float)(height - Px(8))), (float)Px(4));
                }
                if (dividerAtLeft_) {
                    // Linked with the other app's widget on the left: the divider between the two.
                    gp::Pen pen(WithAlpha(lightTaskbar_ ? 50 : 60, foreground), 1);
                    graphics.DrawLine(&pen, 1.0f, (float)Px(14), 1.0f, (float)(height - Px(14)));
                }
                Draw(graphics, width, height, lightTaskbar_);
            }
            POINT position{bounds.left, bounds.top};
            SIZE size{width, height};
            POINT source{0, 0};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            UpdateLayeredWindow(hwnd_, screen, &position, &size, memory, &source, 0, &blend, ULW_ALPHA);
            SelectObject(memory, previous);
        }
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
    }

    // The taskbar comes to the front whenever it's clicked, so move back above it. Only just above
    // it, though: windows that belong over the taskbar, like a screenshot tool's overlay or an
    // always-on-top window, stay over the widget too.
    void StayAboveTaskbar() {
        HWND taskbar = TaskbarWindow();
        if (!taskbar) {
            return;
        }
        for (HWND window = GetWindow(taskbar, GW_HWNDPREV); window; window = GetWindow(window, GW_HWNDPREV)) {
            if (window == hwnd_) {
                return;  // Already above the taskbar.
            }
        }
        // Placing the widget after (below) the window just above the taskbar puts it directly over the taskbar.
        HWND justAbove = GetWindow(taskbar, GW_HWNDPREV);
        SetWindowPos(hwnd_, justAbove ? justAbove : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    // Another app came to the front, which may have put the taskbar over the widget.
    static void CALLBACK OnForegroundChanged(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
        if (widgetForHook_) {
            widgetForHook_->UpdatePlacement(false);
        }
    }

    static inline TaskbarWidget* widgetForHook_ = nullptr;

    bool shown_ = false;
    int offset_ = -1;  // Pixels from the taskbar's left edge; negative means the default position.
    RECT taskbar_{};
    float scale_ = 0;
    bool lightTaskbar_ = false;
    bool shellReportsFullScreen_ = false;
    UINT appBarMessage_ = 0;
    HWINEVENTHOOK foregroundHook_ = nullptr;
    bool trackingMouse_ = false;
    bool pressed_ = false;
    bool dragging_ = false;
    int dragStartCursorX_ = 0;
    int dragStartOffset_ = 0;
    int wheelRemainder_ = 0;
    int groupLeft_ = 0;  // Left edge of the linked pair on screen (or of this widget alone).
    bool wasLinked_ = false;
    bool dividerAtLeft_ = false;  // Linked, with the other app's widget on the left.
};

// The flyout shown above a part of the widget while hovering it. It stays open while the mouse is
// on its way up from the widget, and closes as soon as the mouse leaves the flyout.
class HoverFlyout : public PopupWindow {
public:
    // Shows the flyout centered above the anchor (the hovered part of the widget).
    void ShowAbove(const RECT& anchor) {
        anchor_ = anchor;
        POINT center{(anchor.left + anchor.right) / 2, anchor.top};
        SetScaleFor(center);
        PositionAboveAnchor();
        lastCursorNear_ = GetTickCount();
        if (!Visible()) {
            cursorEntered_ = false;
        }
        ShowNoActivate();
        SetTimer(hwnd_, kHideCheckTimer, 100, nullptr);
    }

    // Follows the widget when it moves or changes width.
    void SetAnchor(const RECT& anchor) {
        if (EqualRect(&anchor, &anchor_)) {
            return;
        }
        anchor_ = anchor;
        if (Visible()) {
            PositionAboveAnchor();
        }
    }

protected:
    virtual int LogicalWidth() = 0;
    virtual int LogicalHeight() = 0;
    // Keeps the flyout open (while dragging a slider, for example).
    virtual bool HoldOpen() { return false; }

    void OnTimer(UINT_PTR timer) override {
        if (timer == kHideCheckTimer) {
            HideWhenCursorLeaves();
        }
    }

    void OnHidden() override { KillTimer(hwnd_, kHideCheckTimer); }

    void OnMouseEnter() override { cursorEntered_ = true; }

    void OnMouseLeave() override {
        if (!HoldOpen()) {
            Hide();
        }
    }

    void PositionAboveAnchor() {
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(MonitorFromRect(&anchor_, MONITOR_DEFAULTTONEAREST), &info);
        int width = Px((float)LogicalWidth());
        int height = Px((float)LogicalHeight());
        int x = (anchor_.left + anchor_.right - width) / 2;
        x = std::max((int)info.rcWork.left + Px(8), std::min(x, (int)info.rcWork.right - width - Px(8)));
        int y = std::min(anchor_.top, info.rcWork.bottom) - height - Px(8);
        Place(x, y, LogicalWidth(), LogicalHeight());
    }

    // Backs up WM_MOUSELEAVE, which Windows doesn't always send, and handles the trip up from the widget.
    void HideWhenCursorLeaves() {
        if (HoldOpen()) {
            return;
        }
        POINT cursor;
        GetCursorPos(&cursor);
        RECT bounds = Bounds();
        if (PtInRect(&bounds, cursor)) {
            cursorEntered_ = true;
            return;
        }
        if (cursorEntered_) {
            Hide();
            return;
        }
        // Not in the flyout yet: the widget and the gap above it count as on the way.
        RECT gap{anchor_.left, bounds.bottom, anchor_.right, std::max(bounds.bottom, anchor_.top)};
        if (PtInRect(&anchor_, cursor) || PtInRect(&gap, cursor)) {
            lastCursorNear_ = GetTickCount();
        } else if (GetTickCount() - lastCursorNear_ > kApproachGraceMilliseconds) {
            Hide();
        }
    }

    static constexpr UINT_PTR kHideCheckTimer = 7;
    // How long the mouse may be off both the widget and the flyout on its way up to the flyout.
    static constexpr DWORD kApproachGraceMilliseconds = 350;

    RECT anchor_{};
    DWORD lastCursorNear_ = 0;
    bool cursorEntered_ = false;
};

// Runs the UI on its own thread with a message loop, for WhTool_ModInit/WhTool_ModUninit.
// Worker threads (reading display numbers, switching the webcam) whose results come back to the UI
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

    // Gives any still running (one may be waiting on a UAC prompt) a little time to finish, well
    // within the time the mod has to unload.
    void WaitAll(DWORD milliseconds) {
        DWORD start = GetTickCount();
        for (HANDLE thread : threads_) {
            DWORD elapsed = GetTickCount() - start;
            WaitForSingleObject(thread, elapsed < milliseconds ? milliseconds - elapsed : 0);
            CloseHandle(thread);
        }
        threads_.clear();
    }

    ~WorkerThreads() { WaitAll(0); }

private:
    std::vector<HANDLE> threads_;
};

class UiThread {
public:
    // run is called on the new thread (with COM and GDI+ set up) and returns an object that lives
    // until the mod is unloaded.
    bool Start(std::function<std::shared_ptr<void>()> run) {
        run_ = std::move(run);
        ready_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId_);
        if (!thread_) {
            return false;
        }
        WaitForSingleObject(ready_, 10000);
        return true;
    }

    void Stop() {
        if (!thread_) {
            return;
        }
        PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
        WaitForSingleObject(thread_, 10000);
        CloseHandle(thread_);
        thread_ = nullptr;
        CloseHandle(ready_);
    }

private:
    static DWORD WINAPI ThreadProc(void* parameter) {
        auto self = (UiThread*)parameter;
        // Real pixels everywhere, scaled per display.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ULONG_PTR gdiplusToken = 0;
        gp::GdiplusStartupInput startup;
        gp::GdiplusStartup(&gdiplusToken, &startup, nullptr);
        MSG message;
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // Creates the message queue.
        SetEvent(self->ready_);
        {
            std::shared_ptr<void> app = self->run_();
            while (GetMessageW(&message, nullptr, 0, 0) > 0) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
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

// PKEY_Device_FriendlyName: the name plus the audio hardware ("Headset (4- USB Audio Device)").
const PROPERTYKEY kKeyFriendlyName = {{0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}}, 14};
// PKEY_Device_DeviceDesc: for an endpoint, its own name ("Headset").
const PROPERTYKEY kKeyEndpointName = {{0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}}, 2};
// PKEY_DeviceInterface_FriendlyName: the audio hardware ("4- USB Audio Device").
const PROPERTYKEY kKeyInterfaceName = {{0x026E516E, 0xB814, 0x414B, {0x83, 0xCD, 0x85, 0x6D, 0x6F, 0xEF, 0x48, 0x22}}, 2};
// PKEY_DeviceClass_IconPath: the icon chosen in the Sound control panel ("%SystemRoot%\System32\mmres.dll,-3011").
const PROPERTYKEY kKeyIconPath = {{0x259ABFFC, 0x50A7, 0x47CE, {0xAF, 0x08, 0x68, 0xC9, 0xA7, 0xD7, 0x33, 0x66}}, 12};
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

enum class DeviceKind { Speakers, Headphones, Headset, Display, Microphone };

struct AudioDevice {
    std::wstring id;
    std::wstring name;         // The device's own name, which can be changed in the Sound control panel ("Headset").
    std::wstring description;  // The audio hardware ("4- USB Advanced Audio Device").
    std::wstring fullName;     // Name plus the audio hardware ("Headset (4- USB Advanced Audio Device)").
    DeviceKind kind = DeviceKind::Speakers;
    std::wstring iconPath;
};

struct AudioStatus {
    AudioDevice device;
    float level = 0;  // Volume from 0 to 1.
    bool muted = false;

    int Percent() const { return (int)std::lround(level * 100); }

    bool operator==(const AudioStatus& other) const {
        return device.id == other.device.id && device.name == other.device.name && device.description == other.device.description &&
               device.fullName == other.device.fullName && device.iconPath == other.device.iconPath && level == other.level &&
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
    info.iconPath = ReadStringProperty(store.Get(), kKeyIconPath);
    return true;
}

int CompareNames(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT, NORM_IGNORECASE, a.c_str(), -1, b.c_str(), -1, nullptr, nullptr, 0) - 2;
}

// Lists output (eRender) and input (eCapture) devices, reads the default device's volume and
// changes the default device. Used on the UI thread.
class AudioDevices {
public:
    AudioDevices() {
        CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL, kIidMMDeviceEnumerator, (void**)enumerator_.Put());
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
    // keep what it had; the next poll starts over.
    std::optional<AudioStatus> DefaultStatus(EDataFlow flow, bool* failed = nullptr) {
        CachedVolume& cache = volumes_[flow == eCapture ? 1 : 0];
        ComPtr<IMMDevice> device;
        AudioStatus status;
        auto fail = [&]() -> std::optional<AudioStatus> {
            cache.control.Reset();
            cache.deviceId.clear();
            if (failed) {
                *failed = true;
            }
            return std::nullopt;
        };
        if (!enumerator_ || FAILED(enumerator_->GetDefaultAudioEndpoint(flow, eConsole, device.Put()))) {
            cache.control.Reset();
            cache.deviceId.clear();
            return std::nullopt;
        }
        // The name and icon are read every time so renames in Windows show up straight away.
        if (!ReadDevice(device.Get(), status.device)) {
            return fail();
        }
        if (!cache.control || cache.deviceId != status.device.id) {
            cache.control.Reset();
            if (FAILED(device->Activate(kIidAudioEndpointVolume, CLSCTX_ALL, nullptr, (void**)cache.control.Put()))) {
                return fail();
            }
            cache.deviceId = status.device.id;
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

    // Sets the default device's volume (0 to 1), unmuting it like the Windows volume slider does.
    // Called for every slider movement, so it reuses the volume control from the last poll.
    void SetDefaultVolume(EDataFlow flow, float level) {
        CachedVolume& cache = volumes_[flow == eCapture ? 1 : 0];
        if (!cache.control && !DefaultStatus(flow)) {
            return;
        }
        if (!SetVolume(cache.control.Get(), level, level > 0)) {
            cache.control.Reset();  // The device went away; the next poll picks up the new default.
            cache.deviceId.clear();
        }
    }

private:
    struct CachedVolume {
        std::wstring deviceId;
        ComPtr<IAudioEndpointVolume> control;
    };

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
    // The default device's volume control, per direction, kept between polls and only reactivated
    // when the default device changes.
    CachedVolume volumes_[2];
};

// ---------------------------------------------------------------------------
// Device icons: the ones chosen for each device in the Sound control panel (Properties > Change
// Icon), or a glyph from the Windows icon font.

const wchar_t* GlyphFor(const std::optional<AudioStatus>& status) {
    if (!status) {
        return L"";  // Mute
    }
    switch (status->device.kind) {
        case DeviceKind::Headphones:
            return L"";
        case DeviceKind::Headset:
            return L"";
        case DeviceKind::Display:
            return L"";
        case DeviceKind::Microphone:
            return L"";
        default:
            return L"";
    }
}

// A copy of an icon as a bitmap, with its transparency (from its alpha channel, or its mask for
// older icons without one).
std::shared_ptr<gp::Bitmap> IconToBitmap(HICON icon) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return nullptr;
    }
    std::shared_ptr<gp::Bitmap> result;
    BITMAP colorInfo{};
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(colorInfo), &colorInfo)) {
        int width = colorInfo.bmWidth;
        int height = colorInfo.bmHeight;
        BITMAPINFO header{};
        header.bmiHeader.biSize = sizeof(header.bmiHeader);
        header.bmiHeader.biWidth = width;
        header.bmiHeader.biHeight = -height;
        header.bmiHeader.biPlanes = 1;
        header.bmiHeader.biBitCount = 32;
        header.bmiHeader.biCompression = BI_RGB;
        std::vector<DWORD> pixels((size_t)width * height);
        std::vector<DWORD> mask((size_t)width * height);
        HDC screen = GetDC(nullptr);
        bool ok = GetDIBits(screen, info.hbmColor, 0, height, pixels.data(), &header, DIB_RGB_COLORS) == height;
        bool hasAlpha = std::any_of(pixels.begin(), pixels.end(), [](DWORD pixel) { return (pixel >> 24) != 0; });
        if (ok && !hasAlpha && info.hbmMask) {
            ok = GetDIBits(screen, info.hbmMask, 0, height, mask.data(), &header, DIB_RGB_COLORS) == height;
            for (size_t i = 0; ok && i < pixels.size(); i++) {
                pixels[i] = (pixels[i] & 0x00FFFFFF) | ((mask[i] & 0x00FFFFFF) ? 0 : 0xFF000000);  // White mask is see-through.
            }
        }
        ReleaseDC(nullptr, screen);
        if (ok) {
            result = std::make_shared<gp::Bitmap>(width, height, PixelFormat32bppARGB);
            gp::Rect rect(0, 0, width, height);
            gp::BitmapData data;
            if (result->LockBits(&rect, gp::ImageLockModeWrite, PixelFormat32bppARGB, &data) == gp::Ok) {
                for (int y = 0; y < height; y++) {
                    memcpy((BYTE*)data.Scan0 + (size_t)y * data.Stride, pixels.data() + (size_t)y * width, (size_t)width * 4);
                }
                result->UnlockBits(&data);
            } else {
                result.reset();
            }
        }
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    return result;
}

// Device icons by size and icon path. Emptied before GDI+ shuts down (ClearDeviceIcons).
std::map<std::wstring, std::shared_ptr<gp::Bitmap>>& DeviceIconCache() {
    static std::map<std::wstring, std::shared_ptr<gp::Bitmap>> cache;
    return cache;
}

void ClearDeviceIcons() {
    DeviceIconCache().clear();
}

// The device's Windows icon at the given pixel size, or null if it can't be loaded.
gp::Bitmap* DeviceIcon(const std::wstring& iconPath, int size) {
    std::map<std::wstring, std::shared_ptr<gp::Bitmap>>& cache = DeviceIconCache();
    if (Trim(iconPath).empty() || size <= 0) {
        return nullptr;
    }
    std::wstring key = std::to_wstring(size) + L"|" + iconPath;
    auto found = cache.find(key);
    if (found != cache.end()) {
        return found->second.get();  // Failures are cached too, so they aren't retried on every poll.
    }
    // Icon paths look like "%SystemRoot%\System32\mmres.dll,-3011"; a negative index is a resource ID.
    std::wstring file = Trim(iconPath);
    if (!file.empty() && file[0] == L'@') {
        file = file.substr(1);
    }
    int index = 0;
    size_t comma = file.find_last_of(L',');
    if (comma != std::wstring::npos && comma > 0 && ParseInt(Trim(file.substr(comma + 1)), index)) {
        file = file.substr(0, comma);
    }
    file = Trim(file);
    if (file.size() >= 2 && file.front() == L'"' && file.back() == L'"') {
        file = file.substr(1, file.size() - 2);
    }
    file = ExpandEnvironment(file);
    std::shared_ptr<gp::Bitmap> bitmap;
    HICON icon = nullptr;
    if (SUCCEEDED(SHDefExtractIconW(file.c_str(), index, 0, &icon, nullptr, MAKELONG(size, 0))) && icon) {
        bitmap = IconToBitmap(icon);
        DestroyIcon(icon);
    }
    cache[key] = bitmap;
    return bitmap.get();
}

void DrawMuteBadge(gp::Graphics& graphics, const gp::RectF& bounds) {
    float size = bounds.Width * 0.6f;
    gp::RectF badge(bounds.GetRight() - size, bounds.GetBottom() - size, size, size);
    gp::SolidBrush fill(Argb(255, 196, 43, 28));
    graphics.FillEllipse(&fill, badge);
    gp::Pen pen(Argb(255, 255, 255, 255), std::max(1.2f, size / 7.0f));
    float inset = size * 0.32f;
    graphics.DrawLine(&pen, badge.X + inset, badge.Y + inset, badge.GetRight() - inset, badge.GetBottom() - inset);
    graphics.DrawLine(&pen, badge.GetRight() - inset, badge.Y + inset, badge.X + inset, badge.GetBottom() - inset);
}

// Draws the device's Windows icon, or a glyph when there's no device or its icon can't be loaded.
// Muted devices are faded, with a mute badge.
void DrawDeviceIcon(gp::Graphics& graphics, const std::optional<AudioStatus>& status, const gp::RectF& bounds, const gp::Color& glyphColor) {
    gp::Bitmap* icon = status ? DeviceIcon(status->device.iconPath, (int)std::min(bounds.Width, bounds.Height)) : nullptr;
    bool muted = status && status->muted;
    if (!icon) {
        DrawGlyph(graphics, GlyphFor(status), bounds, muted ? WithAlpha(115, glyphColor) : glyphColor);
    } else {
        gp::InterpolationMode previous = graphics.GetInterpolationMode();
        graphics.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
        gp::ImageAttributes attributes;
        gp::ColorMatrix matrix = {{{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, muted ? 0.45f : 1.0f, 0}, {0, 0, 0, 0, 1}}};
        attributes.SetColorMatrix(&matrix);
        graphics.DrawImage(icon, bounds, 0, 0, (gp::REAL)icon->GetWidth(), (gp::REAL)icon->GetHeight(), gp::UnitPixel, &attributes);
        graphics.SetInterpolationMode(previous);
    }
    if (muted) {
        DrawMuteBadge(graphics, bounds);
    }
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

// Connected webcams, including turned-off ones, sorted by name.
std::vector<Webcam> GetWebcams() {
    // Device setup classes webcams install under: "Cameras", and "Imaging devices" for older drivers.
    const GUID classes[] = {{0xCA3E7AB9, 0xB4C3, 0x4AE6, {0x82, 0x51, 0x57, 0x9E, 0xF9, 0x33, 0x89, 0x0F}},
                            {0x6BDD1FC6, 0x810F, 0x11D0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}}};
    std::vector<Webcam> webcams;
    for (const GUID& classGuid : classes) {
        HDEVINFO set = SetupDiGetClassDevsW(&classGuid, nullptr, nullptr, DIGCF_PRESENT);
        if (set == INVALID_HANDLE_VALUE) {
            continue;
        }
        SP_DEVINFO_DATA device{sizeof(device)};
        for (DWORD index = 0; SetupDiEnumDeviceInfo(set, index, &device); index++) {
            WCHAR id[512] = {};
            if (!SetupDiGetDeviceInstanceIdW(set, &device, id, ARRAYSIZE(id), nullptr) ||
                std::any_of(webcams.begin(), webcams.end(), [&](const Webcam& w) { return w.instanceId == id; })) {
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

// Whether the webcam is turned on, or nothing when it isn't connected. Cheap enough to poll.
std::optional<bool> WebcamEnabled(const std::wstring& instanceId) {
    DEVINST devInst = 0;
    if (instanceId.empty() || CM_Locate_DevNodeW(&devInst, (DEVINSTID_W)instanceId.c_str(), CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) {
        return std::nullopt;
    }
    return DevNodeEnabled(devInst);
}

enum class WebcamChange { Done, RestartNeeded, Cancelled, Failed };

const DWORD kErrorSuccessRebootRequired = 3010;

// Waits for a process and returns its exit code.
DWORD WaitForExitCode(HANDLE process) {
    WaitForSingleObject(process, 60000);
    DWORD exitCode = 1;
    GetExitCodeProcess(process, &exitCode);
    CloseHandle(process);
    return exitCode;
}

const CLSID kClsidTaskScheduler = {0x0F87369F, 0xA4E5, 0x4CFC, {0xBD, 0x3E, 0x73, 0xE6, 0x15, 0x45, 0x72, 0xDD}};
const IID kIidTaskService = {0x2FABA4C7, 0x4DA9, 0x4013, {0x96, 0x97, 0x20, 0xCC, 0x3F, 0xD4, 0x0F, 0x85}};

// Runs AudioFlipper.exe's scheduled task that turns the webcam on or off (set up with its "Switch
// without asking each time"; starting one needs no prompt) and waits for the webcam to change.
// Nothing when the tasks aren't set up (or were deleted), to fall back to the UAC prompt.
std::optional<WebcamChange> RunWebcamTask(const std::wstring& instanceId, bool enable) {
    ComPtr<ITaskService> service;
    VARIANT empty;
    VariantInit(&empty);
    if (FAILED(CoCreateInstance(kClsidTaskScheduler, nullptr, CLSCTX_INPROC_SERVER, kIidTaskService, (void**)service.Put())) ||
        FAILED(service->Connect(empty, empty, empty, empty))) {
        return std::nullopt;
    }
    ComPtr<ITaskFolder> folder;
    ComPtr<IRegisteredTask> task;
    ComPtr<IRunningTask> running;
    BSTR root = SysAllocString(L"\\");
    BSTR name = SysAllocString(enable ? L"AudioFlipper Webcam On" : L"AudioFlipper Webcam Off");
    bool started = SUCCEEDED(service->GetFolder(root, folder.Put())) && SUCCEEDED(folder->GetTask(name, task.Put())) &&
                   SUCCEEDED(task->Run(empty, running.Put()));
    SysFreeString(root);
    SysFreeString(name);
    if (!started) {
        return std::nullopt;
    }
    // The task runs in the background; watch for the webcam to change.
    for (int waited = 0; waited < 10000; waited += 200) {
        if (WebcamEnabled(instanceId) == enable) {
            return WebcamChange::Done;
        }
        Sleep(200);
    }
    LONG result = 0;
    if (SUCCEEDED(task->get_LastTaskResult(&result)) && result == (LONG)kErrorSuccessRebootRequired) {
        return WebcamChange::RestartNeeded;
    }
    return WebcamChange::Failed;
}

// Turns the webcam on or off. Runs on a worker thread (with COM): it may wait for a UAC prompt,
// which comes up over the owner window.
WebcamChange SetWebcamEnabled(const std::wstring& instanceId, bool enable, bool useTasks, HWND owner) {
    // Only ever hand pnputil the ID of a webcam that's actually connected, never anything else read
    // from the settings file, since it runs with administrator rights.
    std::vector<Webcam> webcams = GetWebcams();
    if (instanceId.empty() || instanceId.find(L'"') != std::wstring::npos ||
        std::none_of(webcams.begin(), webcams.end(), [&](const Webcam& w) { return w.instanceId == instanceId; })) {
        return WebcamChange::Failed;
    }

    if (useTasks) {
        if (std::optional<WebcamChange> result = RunWebcamTask(instanceId, enable)) {
            return *result;
        }
    }

    std::wstring pnputil = SystemFolder() + L"\\pnputil.exe";
    std::wstring arguments = (enable ? L"/enable-device \"" : L"/disable-device \"") + instanceId + L"\"";
    DWORD exitCode = 1;
    if (IsProcessElevated()) {
        // Windhawk runs as administrator, so no prompt is needed.
        HANDLE process = nullptr;
        LaunchOptions options;
        options.hidden = true;
        options.asUser = false;
        if (!Launch(pnputil, arguments, options, &process)) {
            return WebcamChange::Failed;
        }
        exitCode = WaitForExitCode(process);
    } else {
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
        exitCode = WaitForExitCode(execute.hProcess);
    }
    // Trust the device's actual state over pnputil's exit code.
    if (exitCode == kErrorSuccessRebootRequired) {
        return WebcamChange::RestartNeeded;
    }
    return WebcamEnabled(instanceId) == enable ? WebcamChange::Done : WebcamChange::Failed;
}

// ---------------------------------------------------------------------------
// Settings, shared with AudioFlipper.exe: %APPDATA%\AudioFlipper\settings.txt.

struct Settings {
    bool switchCommunications = true;       // Also make the new output the default communications device.
    bool switchInputCommunications = true;  // Also make the new input the default communications device.
    bool showPopup = true;                  // Show the popup when the output changes (while the widget isn't showing it).
    bool volumeDing = true;                 // Play the Windows "Default Beep" after changing the output volume.
    bool showOutput = true;
    bool showInput = false;
    bool showWebcam = false;
    int taskbarOffset = -1;
    std::wstring webcamId;
    std::wstring webcamName;
    std::wstring webcamTasksId;  // The webcam AudioFlipper.exe's no-prompt tasks are set up for.
    std::vector<std::wstring> excluded;
    // Names given to webcams (Windows has no way to rename one), by device instance ID, in the
    // order they were given.
    std::vector<std::pair<std::wstring, std::wstring>> webcamNicknames;

    static std::wstring Path() { return KnownFolder(FOLDERID_RoamingAppData) + L"\\AudioFlipper\\settings.txt"; }

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
        for (const auto& [key, value] : ReadSettingsFile(Path())) {
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
                // "<instance ID><tab><name>"; device instance IDs never contain tabs.
                size_t tab = value.find(L'\t');
                if (tab != std::wstring::npos && tab > 0 && tab < value.size() - 1) {
                    std::wstring id = value.substr(0, tab);
                    if (!WebcamNickname(id)) {
                        webcamNicknames.emplace_back(id, value.substr(tab + 1));
                    } else {
                        SetWebcamNickname(id, value.substr(tab + 1));
                    }
                }
            } else if (key == L"WebcamTasksId") {
                webcamTasksId = value;
            } else if (key == L"TaskbarOffset") {
                ParseInt(value, taskbarOffset);
            } else if (key == L"Exclude") {
                SetExcluded(value, true);
            }
        }
    }

    bool Save() const {
        std::vector<std::wstring> lines{
            L"# AudioFlipper settings",
            L"SwitchCommunications=" + BoolText(switchCommunications),
            L"ShowPopup=" + BoolText(showPopup),
            L"ShowOutput=" + BoolText(showOutput),
            L"ShowInput=" + BoolText(showInput),
            L"VolumeDing=" + BoolText(volumeDing),
            L"SwitchInputCommunications=" + BoolText(switchInputCommunications),
            L"TaskbarOffset=" + std::to_wstring(taskbarOffset),
            L"ShowWebcam=" + BoolText(showWebcam),
            L"WebcamId=" + webcamId,
            L"WebcamName=" + webcamName,
            L"WebcamTasksId=" + webcamTasksId,
        };
        for (const std::wstring& id : excluded) {
            lines.push_back(L"Exclude=" + id);
        }
        for (const auto& [id, nickname] : webcamNicknames) {
            lines.push_back(L"WebcamNickname=" + id + L"\t" + nickname);
        }
        return WriteTextLines(Path(), lines, false);
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// AudioFlipper: the taskbar widget, the volume flyout and the app.

namespace {

// A video-camera outline: a body and a lens, with a slash through it when off.
void DrawWebcamIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, bool off, const gp::Color& behind) {
    float s = box.Width;
    FillRoundedRect(graphics, color, gp::RectF(box.X + s * 0.04f, box.Y + s * 0.26f, s * 0.62f, s * 0.48f), s * 0.1f);
    gp::PointF lens[] = {{box.X + s * 0.70f, box.Y + s * 0.42f}, {box.X + s * 0.96f, box.Y + s * 0.27f},
                         {box.X + s * 0.96f, box.Y + s * 0.73f}, {box.X + s * 0.70f, box.Y + s * 0.58f}};
    gp::SolidBrush brush(color);
    graphics.FillPolygon(&brush, lens, 4);
    if (!off) {
        return;
    }
    gp::PointF start(box.X + s * 0.06f, box.Y + s * 0.06f);
    gp::PointF end(box.X + s * 0.94f, box.Y + s * 0.94f);
    // Cut a gap around the slash so it reads clearly against the icon.
    gp::CompositingMode previous = graphics.GetCompositingMode();
    graphics.SetCompositingMode(gp::CompositingModeSourceCopy);
    gp::Pen gap(behind, s * 0.26f);
    gap.SetStartCap(gp::LineCapRound);
    gap.SetEndCap(gp::LineCapRound);
    graphics.DrawLine(&gap, start, end);
    graphics.SetCompositingMode(previous);
    gp::Pen slash(color, s * 0.1f);
    slash.SetStartCap(gp::LineCapRound);
    slash.SetEndCap(gp::LineCapRound);
    graphics.DrawLine(&slash, start, end);
}

// ---------------------------------------------------------------------------
// The flyout shown above the output or input section while hovering it, like the Windows 11
// volume flyout: the device's name, a volume slider, and the device icon as a mute button.

class VolumeFlyout : public HoverFlyout {
public:
    std::function<void(float level)> onVolumeChange;  // While dragging or clicking the slider, 0 to 1.
    std::function<void()> onVolumeChangeFinished;     // When the slider is let go.
    std::function<void(int notches)> onVolumeSteps;
    std::function<void()> onMute;

    bool Create() { return CreatePopup(L"AudioFlipper.Mod.VolumeFlyout"); }

    void SetStatus(const std::optional<AudioStatus>& status) {
        status_ = status;
        if (holdingDragLevel_ && status && std::abs(status->level - dragLevel_) < 0.005f) {
            holdingDragLevel_ = false;
        }
        Invalidate();
    }

protected:
    int LogicalWidth() override { return 320; }
    int LogicalHeight() override { return 104; }
    bool HoldOpen() override { return draggingSlider_; }

    void OnScaleChanged() override {
        titleFont_ = MakeFont(L"Segoe UI Semibold", 14.0f * scale_);
        detailFont_ = MakeFont(L"Segoe UI", 12.0f * scale_);
        valueFont_ = MakeFont(L"Segoe UI", 14.0f * scale_);
    }

    void OnHidden() override {
        HoverFlyout::OnHidden();
        draggingSlider_ = false;
        holdingDragLevel_ = false;
        pressingMute_ = false;
    }

    void OnMouseDown(POINT point) override {
        if (!status_) {
            return;
        }
        if (Contains(MuteButton(), point)) {
            pressingMute_ = true;
            Invalidate();
        } else if (Contains(SliderHit(), point)) {
            draggingSlider_ = true;
            dragLevel_ = status_->level;
            SetVolumeFromX(point.x);
        }
    }

    void OnMouseMove(POINT point) override {
        if (draggingSlider_) {
            SetVolumeFromX(point.x);
            return;
        }
        bool overMute = Contains(MuteButton(), point);
        bool overSlider = Contains(SliderHit(), point);
        if (overMute != hoveringMute_ || overSlider != hoveringSlider_) {
            hoveringMute_ = overMute;
            hoveringSlider_ = overSlider;
            Invalidate();
        }
    }

    void OnMouseUp(POINT point) override {
        if (draggingSlider_) {
            draggingSlider_ = false;
            holdingDragLevel_ = true;
            releasedAt_ = GetTickCount();
            Invalidate();
            if (onVolumeChangeFinished) {
                onVolumeChangeFinished();
            }
            RECT client;
            GetClientRect(hwnd_, &client);
            if (!PtInRect(&client, point)) {
                Hide();  // Released outside the flyout.
            }
        } else if (pressingMute_) {
            pressingMute_ = false;
            Invalidate();
            if (Contains(MuteButton(), point) && onMute) {
                onMute();
            }
        }
    }

    void OnMouseLeave() override {
        hoveringMute_ = false;
        hoveringSlider_ = false;
        if (draggingSlider_) {
            Invalidate();
        } else {
            Hide();
        }
    }

    void OnWheel(int delta) override {
        wheelRemainder_ += delta;
        int notches = wheelRemainder_ / WHEEL_DELTA;
        wheelRemainder_ -= notches * WHEEL_DELTA;
        if (notches != 0 && onVolumeSteps) {
            onVolumeSteps(notches);
        }
    }

    void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) override {
        if (!titleFont_) {
            OnScaleChanged();
        }
        if (!status_) {
            DrawTextLine(graphics, L"No device", *titleFont_, colors.foreground,
                         gp::RectF((float)Px(16), (float)Px(12), (float)(width - Px(32)), (float)Px(22)));
            return;
        }
        // Name and hardware, as on the Sound control panel's Playback tab.
        DrawTextLine(graphics, status_->device.name, *titleFont_, colors.foreground,
                     gp::RectF((float)Px(16), (float)Px(12), (float)(width - Px(32)), (float)Px(22)));
        if (!status_->device.description.empty()) {
            DrawTextLine(graphics, status_->device.description, *detailFont_, colors.secondary,
                         gp::RectF((float)Px(16), (float)Px(33), (float)(width - Px(32)), (float)Px(18)));
        }

        // Device icon, which doubles as the mute button.
        RECT button = MuteButton();
        if (hoveringMute_ || pressingMute_) {
            FillRoundedRect(graphics, colors.hover,
                            gp::RectF((float)button.left, (float)button.top, (float)(button.right - button.left), (float)(button.bottom - button.top)),
                            (float)Px(4));
        }
        float iconSize = (float)Px(24);
        DrawDeviceIcon(graphics, status_,
                       gp::RectF(button.left + (button.right - button.left - iconSize) / 2, button.top + (button.bottom - button.top - iconSize) / 2,
                                 iconSize, iconSize),
                       colors.foreground);

        // Slider, drawn like the Windows 11 one: an accent fill and a ring with a dot that grows on hover.
        float level = DisplayedLevel();
        gp::RectF track = Track();
        gp::Color fill = status_->muted && !draggingSlider_ ? colors.secondary : colors.accent;
        FillPill(graphics, colors.track, track);
        FillPill(graphics, fill, gp::RectF(track.X, track.Y, track.Width * level, track.Height));
        gp::PointF center(track.X + track.Width * level, track.Y + track.Height / 2);
        float ring = (float)Px(10);
        float dot = (float)(draggingSlider_ ? Px(5) : hoveringSlider_ ? Px(7) : Px(6));
        gp::SolidBrush ringBrush(colors.thumbRing);
        graphics.FillEllipse(&ringBrush, center.X - ring, center.Y - ring, ring * 2, ring * 2);
        gp::Pen ringPen(colors.border, 1);
        graphics.DrawEllipse(&ringPen, center.X - ring, center.Y - ring, ring * 2, ring * 2);
        gp::SolidBrush dotBrush(fill);
        graphics.FillEllipse(&dotBrush, center.X - dot, center.Y - dot, dot * 2, dot * 2);

        DrawTextLine(graphics, std::to_wstring((int)std::lround(level * 100)), *valueFont_, colors.foreground,
                     gp::RectF((float)(width - Px(54)), (float)Px(64), (float)Px(40), (float)Px(24)), TextAlign::Right);
    }

private:
    float DisplayedLevel() const {
        if (draggingSlider_ || (holdingDragLevel_ && GetTickCount() - releasedAt_ < 500)) {
            return dragLevel_;
        }
        return status_ ? status_->level : 0.0f;
    }

    static bool Contains(const RECT& rect, POINT point) { return PtInRect(&rect, point); }

    RECT MuteButton() const { return {Px(10), Px(58), Px(46), Px(94)}; }

    RECT SliderHit() const {
        RECT client{};
        GetClientRect(hwnd_, &client);
        return {Px(50), Px(58), client.right - Px(52), Px(94)};
    }

    gp::RectF Track() const {
        RECT client{};
        GetClientRect(hwnd_, &client);
        return gp::RectF((float)Px(60), (float)Px(74), (float)(client.right - Px(60) - Px(62)), (float)Px(4));
    }

    void SetVolumeFromX(int x) {
        gp::RectF track = Track();
        float level = std::max(0.0f, std::min(1.0f, (x - track.X) / track.Width));
        if (level == dragLevel_ && draggingSlider_) {
            Invalidate();
            return;
        }
        dragLevel_ = level;
        // Repaint right away, before changing the volume, so the thumb keeps up with the mouse.
        Invalidate();
        UpdateWindow(hwnd_);
        if (onVolumeChange) {
            onVolumeChange(level);
        }
    }

    std::optional<AudioStatus> status_;
    std::unique_ptr<gp::Font> titleFont_;
    std::unique_ptr<gp::Font> detailFont_;
    std::unique_ptr<gp::Font> valueFont_;
    bool draggingSlider_ = false;
    float dragLevel_ = 0;
    // After a drag, keep showing where the slider was released until the new volume comes back
    // from Windows, so the thumb doesn't jump back for a moment.
    bool holdingDragLevel_ = false;
    DWORD releasedAt_ = 0;
    bool hoveringSlider_ = false;
    bool hoveringMute_ = false;
    bool pressingMute_ = false;
    int wheelRemainder_ = 0;
};

// ---------------------------------------------------------------------------
// The widget, with up to three sections:
// - Output: the active output's icon, name and volume. Click to switch output, scroll to change
//   volume, middle-click to mute, hover for a volume slider.
// - Input: the same for the active microphone.
// - Webcam: a camera icon (struck through while off) with the chosen webcam's name over "On" or
//   "Off"; click to switch it.
// Right-click anywhere for the menu, and drag to move the widget along the taskbar.

struct WebcamStatus {
    std::wstring name;  // The webcam's name, or the name it was given in AudioFlipper.
    bool enabled = true;
    bool busy = false;  // Being turned on or off (waiting on the UAC prompt or Windows).

    bool operator==(const WebcamStatus& other) const = default;
};

class AfWidget : public TaskbarWidget {
public:
    enum Part { kNone, kOutput, kInput, kWebcam };

    std::function<void(EDataFlow)> onNext;
    std::function<void(EDataFlow)> onMute;
    std::function<void(EDataFlow, int notches)> onVolumeSteps;
    std::function<void(EDataFlow, float level)> onVolumeChange;
    std::function<void(EDataFlow)> onVolumeChangeFinished;
    std::function<void()> onWebcamToggle;
    std::function<void()> onMenu;

    bool Create(int offset) {
        if (!TaskbarWidget::Create(offset) || !flyout_.Create()) {
            return false;
        }
        flyout_.onVolumeChange = [this](float level) {
            if (FlowOf(flyoutPart_) && onVolumeChange) {
                onVolumeChange(*FlowOf(flyoutPart_), level);
            }
        };
        flyout_.onVolumeChangeFinished = [this]() {
            if (FlowOf(flyoutPart_) && onVolumeChangeFinished) {
                onVolumeChangeFinished(*FlowOf(flyoutPart_));
            }
        };
        flyout_.onVolumeSteps = [this](int notches) {
            if (FlowOf(flyoutPart_) && onVolumeSteps) {
                onVolumeSteps(*FlowOf(flyoutPart_), notches);
            }
        };
        flyout_.onMute = [this]() {
            if (FlowOf(flyoutPart_) && onMute) {
                onMute(*FlowOf(flyoutPart_));
            }
        };
        return true;
    }

    // Shows or hides the output and input sections. The widget hides when no section is shown.
    void SetSections(bool showOutput, bool showInput) {
        showOutput_ = showOutput;
        showInput_ = showInput;
        SectionsChanged();
    }

    void SetStatus(const std::optional<AudioStatus>& output, const std::optional<AudioStatus>& input) {
        output_ = output;
        input_ = input;
        if (flyoutPart_ != kNone) {
            flyout_.SetStatus(StatusOf(flyoutPart_));
        }
        ContentChanged();
    }

    // The webcam's state, or nothing to hide the webcam section.
    void SetWebcam(const std::optional<WebcamStatus>& webcam) {
        if (webcam == webcam_) {
            return;
        }
        bool sectionsChanged = webcam.has_value() != webcam_.has_value();
        webcam_ = webcam;
        if (sectionsChanged) {
            SectionsChanged();
        } else {
            ContentChanged();
        }
    }

    // Whether the output section is on screen right now.
    bool ShowsOutput() const { return Visible() && showOutput_; }

    void HideFlyout() { flyout_.Hide(); }

protected:
    static constexpr UINT_PTR kHoverTimer = 2;

    struct Section {
        Part part;
        int x;
        int width;
    };

    int DefaultOffset(int barWidth, int width) override {
        // With centered taskbar icons the left end is free, but the Widgets (weather) button sits in
        // the corner, so start just to its right. With left-aligned icons the middle is free.
        return TaskbarIconsLeftAligned() ? (barWidth - width) / 2 : Px(180);
    }

    void OnScaleChanged() override {
        // Same type sizes as the taskbar's weather widget.
        nameFont_ = MakeFont(L"Segoe UI Semibold", 12.0f * Scale());
        volumeFont_ = MakeFont(L"Segoe UI", 12.0f * Scale());
        measuredWidths_.clear();
    }

    int LayOut() override {
        sections_.clear();
        int x = 0;
        auto add = [&](Part part, int width) {
            sections_.push_back({part, x, width});
            x += width;
        };
        if (showOutput_) {
            add(kOutput, MeasureAudioWidth(NameOf(kOutput)));
        }
        if (showInput_) {
            add(kInput, MeasureAudioWidth(NameOf(kInput)));
        }
        if (webcam_) {
            add(kWebcam, MeasureWebcamWidth(webcam_->name));
        }
        return x;
    }

    void Draw(gp::Graphics& graphics, int width, int height, bool light) override {
        gp::Color foreground = light ? Argb(255, 28, 28, 28) : Argb(255, 255, 255, 255);
        gp::Color highlight = WithAlpha(light ? 18 : 24, foreground);
        for (size_t i = 0; i < sections_.size(); i++) {
            const Section& section = sections_[i];
            bool isHovered = !Dragging() && hovered_ == section.part;
            if (isHovered) {
                FillRoundedRect(graphics, highlight,
                                gp::RectF((float)(section.x + Px(2)), (float)Px(4), (float)(section.width - Px(4)), (float)(height - Px(8))),
                                (float)Px(4));
            }
            if (i > 0) {
                gp::Pen divider(WithAlpha(light ? 50 : 60, foreground), 1);
                graphics.DrawLine(&divider, (float)section.x, (float)Px(14), (float)section.x, (float)(height - Px(14)));
            }
            if (section.part == kWebcam) {
                DrawWebcamSection(graphics, section, height, light, foreground, isHovered ? highlight : Argb(1, 0, 0, 0));
            } else {
                DrawAudioSection(graphics, section, height, light, foreground);
            }
        }
    }

    void OnHover(POINT point) override {
        Part part = PartAt(point);
        if (part == hovered_) {
            return;
        }
        hovered_ = part;
        Render();
        // The output and input sections open the volume flyout after a moment; an open flyout
        // closes itself once the mouse is away from it and its section.
        KillTimer(Handle(), kHoverTimer);
        if (FlowOf(part)) {
            hoverTarget_ = part;
            SetTimer(Handle(), kHoverTimer, 400, nullptr);
        }
    }

    void OnHoverLeave() override {
        if (hovered_ != kNone) {
            hovered_ = kNone;
            Render();
        }
        KillTimer(Handle(), kHoverTimer);
    }

    void OnTimer(UINT_PTR timer) override {
        if (timer != kHoverTimer) {
            return;
        }
        KillTimer(Handle(), kHoverTimer);
        RECT bounds = SectionScreenBounds(hoverTarget_);
        POINT cursor;
        GetCursorPos(&cursor);
        if (Visible() && !Pressed() && PtInRect(&bounds, cursor)) {
            flyoutPart_ = hoverTarget_;
            flyout_.SetStatus(StatusOf(flyoutPart_));
            flyout_.ShowAbove(bounds);
        }
    }

    void OnClick(POINT point, MouseButton button) override {
        Part part = PartAt(point);
        if (button == MouseButton::Right) {
            KillTimer(Handle(), kHoverTimer);
            flyout_.Hide();
            if (onMenu) {
                onMenu();
            }
        } else if (button == MouseButton::Middle) {
            if (FlowOf(part) && onMute) {
                onMute(*FlowOf(part));
            }
        } else if (part == kWebcam) {
            if (onWebcamToggle) {
                onWebcamToggle();
            }
        } else if (FlowOf(part) && onNext) {
            onNext(*FlowOf(part));
        }
    }

    void OnWheel(POINT point, int notches) override {
        if (FlowOf(PartAt(point)) && onVolumeSteps) {
            onVolumeSteps(*FlowOf(PartAt(point)), notches);
        }
    }

    void OnDragStart() override {
        KillTimer(Handle(), kHoverTimer);
        flyout_.Hide();
    }

    void OnPlaced() override {
        if (flyout_.Visible()) {
            flyout_.SetAnchor(SectionScreenBounds(flyoutPart_));
        }
    }

    void OnHidden() override { flyout_.Hide(); }

private:
    static std::optional<EDataFlow> FlowOf(Part part) {
        if (part == kOutput) {
            return eRender;
        }
        if (part == kInput) {
            return eCapture;
        }
        return std::nullopt;
    }

    const std::optional<AudioStatus>& StatusOf(Part part) const { return part == kInput ? input_ : output_; }

    std::wstring NameOf(Part part) const {
        const std::optional<AudioStatus>& status = StatusOf(part);
        if (status) {
            return status->device.name;
        }
        return part == kInput ? L"No input device" : L"No output device";
    }

    void SectionsChanged() {
        SetShown(showOutput_ || showInput_ || webcam_.has_value());
        if ((flyoutPart_ == kOutput && !showOutput_) || (flyoutPart_ == kInput && !showInput_)) {
            flyout_.Hide();
        }
        ContentChanged();
    }

    Part PartAt(POINT point) const {
        for (const Section& section : sections_) {
            if (point.x >= section.x && point.x < section.x + section.width) {
                return section.part;
            }
        }
        return kNone;
    }

    RECT SectionScreenBounds(Part part) const {
        for (const Section& section : sections_) {
            if (section.part == part) {
                return ScreenBoundsOf(section.x, section.width);
            }
        }
        return {};
    }

    // Fits the device name, like the weather widget sizes itself to its text.
    int MeasureAudioWidth(const std::wstring& name) {
        auto found = measuredWidths_.find(name);
        if (found != measuredWidths_.end()) {
            return found->second;
        }
        gp::Bitmap measuring(1, 1, PixelFormat32bppPARGB);
        gp::Graphics graphics(&measuring);
        graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
        int textWidth = (int)std::ceil(MeasureTextWidth(graphics, name, *nameFont_));
        int width = std::max(Px(140), std::min(Px(260), Px(44) + textWidth + Px(20)));
        measuredWidths_[name] = width;
        return width;
    }

    // Fits the webcam's name, up to a limit (longer names are cut short with "…").
    int MeasureWebcamWidth(const std::wstring& name) {
        std::wstring key = L"webcam|" + name;
        auto found = measuredWidths_.find(key);
        if (found != measuredWidths_.end()) {
            return found->second;
        }
        gp::Bitmap measuring(1, 1, PixelFormat32bppPARGB);
        gp::Graphics graphics(&measuring);
        graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
        int textWidth = (int)std::ceil(MeasureTextWidth(graphics, name, *nameFont_));
        int width = std::max(Px(100), Px(40) + std::min(Px(160), textWidth) + Px(20));
        measuredWidths_[key] = width;
        return width;
    }

    // Camera icon, then the webcam's name over "On" or "Off", like the audio sections' two lines.
    void DrawWebcamSection(gp::Graphics& graphics, const Section& section, int height, bool light, const gp::Color& foreground,
                           const gp::Color& behindIcon) {
        gp::Color secondary = light ? Argb(255, 90, 90, 90) : Argb(255, 205, 205, 205);
        gp::Color iconColor = webcam_->busy ? WithAlpha(110, foreground) : foreground;
        float middle = height / 2.0f;
        float camera = (float)Px(20);
        DrawWebcamIcon(graphics, gp::RectF(section.x + (float)Px(12), middle - camera / 2, camera, camera), iconColor, !webcam_->enabled,
                       behindIcon);

        float textLeft = section.x + (float)Px(40);
        float textWidth = section.x + section.width - textLeft - Px(10);
        const wchar_t* state = webcam_->busy ? (webcam_->enabled ? L"Turning off…" : L"Turning on…") : (webcam_->enabled ? L"On" : L"Off");
        DrawTextLine(graphics, webcam_->name, *nameFont_, foreground, gp::RectF(textLeft, middle - Px(18), textWidth, (float)Px(18)));
        DrawTextLine(graphics, state, *volumeFont_, secondary, gp::RectF(textLeft, middle + Px(1), textWidth, (float)Px(17)));
    }

    // Device icon, then its name over "45%" and a level bar, like the weather widget's two lines.
    void DrawAudioSection(gp::Graphics& graphics, const Section& section, int height, bool light, const gp::Color& foreground) {
        const std::optional<AudioStatus>& status = StatusOf(section.part);
        gp::Color secondary = light ? Argb(255, 90, 90, 90) : Argb(255, 205, 205, 205);
        gp::Color accent = light ? Argb(255, 0, 95, 184) : Argb(255, 96, 205, 255);
        gp::Color track = WithAlpha(light ? 60 : 80, foreground);

        float middle = height / 2.0f;
        float iconSize = (float)Px(24);
        DrawDeviceIcon(graphics, status, gp::RectF(section.x + (float)Px(10), middle - iconSize / 2, iconSize, iconSize), foreground);

        float textLeft = section.x + (float)Px(44);
        float textWidth = section.x + section.width - textLeft - Px(10);
        DrawTextLine(graphics, NameOf(section.part), *nameFont_, foreground, gp::RectF(textLeft, middle - Px(18), textWidth, (float)Px(18)));
        if (!status) {
            return;
        }
        float labelWidth = (float)Px(38);
        DrawTextLine(graphics, status->muted ? L"Muted" : std::to_wstring(status->Percent()) + L"%", *volumeFont_, secondary,
                     gp::RectF(textLeft, middle + Px(1), labelWidth + Px(8), (float)Px(17)));
        gp::RectF bar(textLeft + labelWidth + Px(4), middle + Px(8), textWidth - labelWidth - Px(4), (float)std::max(2, Px(3)));
        FillPill(graphics, track, bar);
        if (!status->muted) {
            FillPill(graphics, accent, gp::RectF(bar.X, bar.Y, bar.Width * std::min(100, status->Percent()) / 100.0f, bar.Height));
        }
    }

    VolumeFlyout flyout_;
    std::vector<Section> sections_;
    std::map<std::wstring, int> measuredWidths_;
    std::unique_ptr<gp::Font> nameFont_;
    std::unique_ptr<gp::Font> volumeFont_;
    bool showOutput_ = false;
    bool showInput_ = false;
    std::optional<AudioStatus> output_;
    std::optional<AudioStatus> input_;
    std::optional<WebcamStatus> webcam_;  // Nothing hides the webcam section.
    Part hovered_ = kNone;
    Part hoverTarget_ = kNone;  // The section the hover timer will open the flyout for.
    Part flyoutPart_ = kNone;   // The section the flyout is showing.
};

// ---------------------------------------------------------------------------
// The app: polls the default devices and the webcam, updates the widget and runs the menu.

class AfApp {
public:
    bool Start() {
        LoadSettings();
        if (!messages_.Create(L"AudioFlipper.Mod.Messages")) {
            return false;
        }
        messages_.onTimer = [this](UINT_PTR timer) { OnTimer(timer); };
        messages_.onMessage = [this](UINT message, WPARAM wParam, LPARAM lParam) { OnMessage(message, wParam, lParam); };
        // AudioFlipper.exe and the mod show the same widget; while the exe runs, the mod waits.
        if (!instance_.TryAcquire()) {
            Wh_Log(L"AudioFlipper.exe is running; waiting for it to exit");
            SetTimer(messages_.Handle(), kWaitForAppTimer, 3000, nullptr);
            return true;
        }
        return StartWidget();
    }

    ~AfApp() {
        widget_.Destroy();
        workers_.WaitAll(5000);
        ClearDeviceIcons();
    }

private:
    static constexpr UINT_PTR kRefreshTimer = 1;
    static constexpr UINT_PTR kVolumeRefreshTimer = 2;
    static constexpr UINT_PTR kDingTimer = 3;
    static constexpr UINT_PTR kWaitForAppTimer = 4;
    static constexpr UINT kWebcamChangedMessage = WM_APP + 1;
    static constexpr int kVolumeStepPercent = 2;  // Same step as the Windows volume flyout.

    struct WebcamRequest {
        HWND replyTo;
        HWND owner;  // For the UAC prompt.
        std::wstring instanceId;
        std::wstring name;
        bool enable;
        bool useTasks;
        WebcamChange result = WebcamChange::Failed;
    };

    bool StartWidget() {
        if (!widget_.Create(settings_.taskbarOffset) || !popup_.Create()) {
            return false;
        }
        widget_.onOffsetChanged = [this](int offset) {
            settings_.taskbarOffset = offset;
            Save();
        };
        widget_.onNext = [this](EDataFlow flow) { SwitchToNext(flow); };
        widget_.onMute = [this](EDataFlow flow) {
            audio_.ToggleDefaultMute(flow);
            RefreshStatus(false);
        };
        widget_.onVolumeSteps = [this](EDataFlow flow, int notches) { ChangeVolume(flow, notches * kVolumeStepPercent); };
        widget_.onVolumeChange = [this](EDataFlow flow, float level) {
            audio_.SetDefaultVolume(flow, level);
            RefreshSoon();
        };
        widget_.onVolumeChangeFinished = [this](EDataFlow flow) {
            if (flow == eRender) {
                Ding();
            }
        };
        widget_.onWebcamToggle = [this]() { ToggleWebcam(); };
        widget_.onMenu = [this]() { ShowMenu(); };
        RefreshStatus(false);
        RefreshWebcam();
        widget_.SetSections(settings_.showOutput, settings_.showInput);
        SetTimer(messages_.Handle(), kRefreshTimer, 500, nullptr);
        return true;
    }

    void OnTimer(UINT_PTR timer) {
        switch (timer) {
            case kRefreshTimer:
                RefreshStatus(false);
                RefreshWebcam();
                break;
            case kVolumeRefreshTimer:
                KillTimer(messages_.Handle(), kVolumeRefreshTimer);
                volumeRefreshPending_ = false;
                RefreshStatus(false);
                break;
            case kDingTimer:
                Ding();
                break;
            case kWaitForAppTimer:
                if (instance_.TryAcquire()) {
                    KillTimer(messages_.Handle(), kWaitForAppTimer);
                    LoadSettings();  // The exe may have changed them.
                    StartWidget();
                }
                break;
        }
    }

    void LoadSettings() {
        settings_ = Settings();
        settings_.Load();
        // The exe can turn off both the output and input sections, since it has its tray icon; the
        // widget is the mod's only way in, so one stays shown.
        if (!settings_.showOutput && !settings_.showInput) {
            settings_.showOutput = true;
        }
    }

    void Save() {
        if (!settings_.Save()) {
            popup_.ShowMessage(L"Couldn't save settings", Settings::Path());
        }
    }

    // Polls the default output and input with their volumes, updating the widget when anything
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
            widget_.SetStatus(output, input);
        }
        lastOutput_ = output;
        lastInput_ = input;
        hasRefreshed_ = true;
        if ((showPopup || outputChanged) && settings_.showPopup && !widget_.ShowsOutput()) {
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
            DrawDeviceIcon(graphics, status, bounds, color);
        };
        popup_.Show(content, 1800);
    }

    // Slider drags and fast scrolling change the volume dozens of times a second; refresh the widget
    // at a steady rate instead of after every change.
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
    // AudioFlipper's own name for it, shown on the widget and in the menu (and AudioFlipper.exe's).
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
        widget_.SetWebcam(status);
    }

    static DWORD WINAPI WebcamThread(void* parameter) {
        auto request = (WebcamRequest*)parameter;
        HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        request->result = SetWebcamEnabled(request->instanceId, request->enable, request->useTasks, request->owner);
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
        SetForegroundWindow(widget_.Handle());
        auto request = new WebcamRequest{messages_.Handle(), widget_.Handle(), settings_.webcamId, name, !*enabled,
                                         !settings_.webcamTasksId.empty() && settings_.webcamTasksId == settings_.webcamId};
        if (!workers_.Start(WebcamThread, request)) {
            delete request;
            webcamBusy_ = false;
            RefreshWebcam();
        }
    }

    void OnMessage(UINT message, WPARAM wParam, LPARAM lParam) {
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
            RefreshStatus(false);  // Poll first so a newly shown section appears with its device straight away.
            widget_.SetSections(settings_.showOutput, settings_.showInput);
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
        // The classic Sound control panel (Playback or Recording tab), where a device can be renamed or
        // given a different icon via right-click > Properties.
        submenu.Add(L"Rename or change device icons…", [isOutput]() {
            Launch(SystemFolder() + L"\\control.exe", isOutput ? L"mmsys.cpl,,0" : L"mmsys.cpl,,1", LaunchOptions{});
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
        Menu menu;
        AddAudioMenu(menu, eRender);
        AddAudioMenu(menu, eCapture);
        AddWebcamMenu(menu);

        // Linking the widget with RDPLauncher's into one, on either side; offered while RDPLauncher
        // is running (or they're linked).
        WidgetLink::State link = WidgetLink::Read();
        if (link.linked || WidgetLink::PartnerRunning()) {
            menu.AddSeparator();
            Menu& linkMenu = menu.AddSubmenu(std::wstring(L"Link with ") + kPartnerApp);
            bool selfOnLeft = link.SelfOnLeft();
            linkMenu.Add(L"Not linked", [this, selfOnLeft]() { widget_.SetLink(false, selfOnLeft); }, !link.linked);
            linkMenu.Add(std::wstring(kPartnerApp) + L" on the left", [this]() { widget_.SetLink(true, false); }, link.linked && !selfOnLeft);
            linkMenu.Add(std::wstring(kPartnerApp) + L" on the right", [this]() { widget_.SetLink(true, true); }, link.linked && selfOnLeft);
        }

        RECT bounds = widget_.Bounds();
        POINT cursor;
        GetCursorPos(&cursor);
        // Above the widget, which keeps itself on top and would otherwise cover the menu.
        menu.Show(widget_.Handle(), {cursor.x, bounds.top});
    }

    Settings settings_;
    SingleInstance instance_{L"Local\\AudioFlipper.SingleInstance"};
    AudioDevices audio_;
    MessageWindow messages_;
    AfWidget widget_;
    MessagePopup popup_;
    WorkerThreads workers_;
    std::optional<AudioStatus> lastOutput_;
    std::optional<AudioStatus> lastInput_;
    bool hasRefreshed_ = false;
    bool volumeRefreshPending_ = false;
    bool webcamBusy_ = false;
};

UiThread g_uiThread;

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    return g_uiThread.Start([]() -> std::shared_ptr<void> {
        auto app = std::make_shared<AfApp>();
        if (!app->Start()) {
            Wh_Log(L"Couldn't start");
        }
        return app;
    });
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    g_uiThread.Stop();
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
