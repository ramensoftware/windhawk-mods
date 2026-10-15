// ==WindhawkMod==
// @id              taskbar-rdp-launcher
// @name            Taskbar RDP Launcher
// @description     A taskbar widget that starts Remote Desktop full screen on the number of displays you pick
// @version         1.0.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -loleaut32 -luuid -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar RDP Launcher

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
- **Drag** the widget left or right to move it along the taskbar.

Out of the box, **1** is your main display, **2** adds the display nearest to it, and so on up to
all of them. The buttons are remembered for each arrangement of displays, so a laptop that's
sometimes docked keeps separate buttons for the desk and for the road.

## How it connects

- Computers added by name connect with `mstsc /v:<computer>`, like typing the computer into Remote
  Desktop Connection, after setting the display options in the settings Remote Desktop Connection
  uses for that (`Default.rdp` in Documents; a copy from before the first change is kept in
  `%LOCALAPPDATA%\RDPLauncher`). Since the April 2026 Windows update, Remote Desktop shows a security
  warning every time an .rdp file is opened, but not for computers typed in, so this avoids it.
- Saved .rdp files are never changed: the mod opens a copy with the display options set (and gets
  the warning, as any .rdp file does).
- Remote Desktop numbers displays its own way (not as Windows Settings does), so the first time you
  connect on some of your displays, the mod reads Remote Desktop's list (`mstsc /l`, whose box may
  flash) and remembers it until your displays change. **Read display numbers again** in the menu
  asks again.

## Linking with Taskbar Audio Flipper

With the Taskbar Audio Flipper mod running too, the two widgets can be joined into one: right-click
either, choose **Link with AudioFlipper** (or **Link with RDPLauncher**) and which side it goes on.
Linked, they sit side by side with a divider between them, dragging either moves both, and if one
hides the other takes its place.

## Settings

The computers, their names and the display buttons are kept in `%APPDATA%\RDPLauncher\settings.txt`.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <commdlg.h>
#include <tlhelp32.h>
#include <uiautomation.h>
#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <vector>

namespace {

const wchar_t kSelfApp[] = L"RDPLauncher";
const wchar_t kPartnerApp[] = L"AudioFlipper";
const wchar_t kWidgetClassName[] = L"RDPLauncher.Mod.Widget";

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

void DrawRoundedRect(gp::Graphics& graphics, const gp::Color& color, const gp::RectF& rect, float radius, float width) {
    if (rect.Width <= 0 || rect.Height <= 0) {
        return;
    }
    gp::GraphicsPath path;
    AddRoundedRect(path, rect, radius);
    gp::Pen pen(color, width);
    graphics.DrawPath(&pen, &path);
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

    Displays displays;
    DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    EnumDisplayMonitors(nullptr, nullptr, AddDisplay, (LPARAM)&displays);
    if (previous) {
        SetThreadDpiAwarenessContext(previous);
    }
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

std::wstring AppDataFolder() {
    return KnownFolder(FOLDERID_LocalAppData) + L"\\RDPLauncher";
}

std::wstring DisplayIdsLogPath() {
    return AppDataFolder() + L"\\display-numbers.log";
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
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                (void**)&automation))) {
        return text;
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
    const std::vector<DWORD>* alreadyRunning;
    std::map<DWORD, bool>* checkedProcesses;
    std::vector<HWND> found;
};

BOOL CALLBACK CollectMstscWindow(HWND window, LPARAM data) {
    auto search = (WindowSearch*)data;
    DWORD owner = 0;
    GetWindowThreadProcessId(window, &owner);
    bool candidate = owner == search->processId;
    // The box normally belongs to the process just started, but accept a dialog from any mstsc
    // that wasn't already running, in case it hands the work over.
    if (!candidate && ClassOf(window) == L"#32770" &&
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
    if (!Launch(MstscPath(), L"/l", LaunchOptions{}, &process, &processId)) {
        log += L"mstsc /l didn't start.\r\n";
        return {};
    }

    std::wstring result;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 5000) {
        WindowSearch search{processId, &alreadyRunning, &checkedProcesses, {}};
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
        bool exited = WaitForSingleObject(process, 0) == WAIT_OBJECT_0;
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
// they can't all be read, writes what was found to display-numbers.log.
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
        CreateDirectories(AppDataFolder());
        WriteFileBytes(DisplayIdsLogPath(), WideToUtf8(log));
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

    // Sets a setting; type is "i" for numbers and "s" for text.
    void Set(const std::wstring& name, const std::wstring& type, const std::wstring& value) {
        std::wstring line = name + L":" + type + L":" + value;
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
    bool Save(const std::wstring& path) const { return WriteTextLines(path, lines_, true); }

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
// Settings, shared with RDPLauncher.exe: %APPDATA%\RDPLauncher\settings.txt.

struct Settings {
    std::vector<Connection> connections;
    std::wstring currentConnectionKey;
    bool showOnTaskbar = true;  // The app's; the mod always shows the widget (turn the mod off instead).
    int taskbarOffset = -1;
    std::map<std::wstring, std::vector<Choice>> displayButtons;  // By setup key.
    std::map<std::wstring, DisplayIds> displayIds;               // By outputs key.

    static std::wstring Path() { return KnownFolder(FOLDERID_RoamingAppData) + L"\\RDPLauncher\\settings.txt"; }

    void Load() {
        for (const auto& [key, value] : ReadSettingsFile(Path())) {
            if (key == L"Connection") {
                // Connection=<key>|<name it was given>, or just the key.
                size_t bar = value.find(L'|');
                std::optional<Connection> connection = Connection::FromKey(bar == std::wstring::npos ? value : value.substr(0, bar));
                if (connection && !FindConnection(connection->Key())) {
                    if (bar != std::wstring::npos) {
                        connection->customName = Trim(value.substr(bar + 1));
                    }
                    connections.push_back(*connection);
                }
            } else if (key == L"CurrentConnection") {
                currentConnectionKey = value;
            } else if (key == L"ShowOnTaskbar") {
                showOnTaskbar = ParseBool(value, showOnTaskbar);
            } else if (key == L"TaskbarOffset") {
                ParseInt(value, taskbarOffset);
            } else if (key == L"DisplayButton") {
                // DisplayButton=<setup key>|<display key>;<display key>…, one line per button.
                size_t bar = value.find(L'|');
                if (bar != std::wstring::npos && bar > 0) {
                    Choice choice = Split(value.substr(bar + 1), L';');
                    if (!choice.empty()) {
                        displayButtons[value.substr(0, bar)].push_back(choice);
                    }
                }
            } else if (key == L"DisplayIds") {
                // DisplayIds=<outputs key>|<display key>=<id>;<display key>=<id>…
                size_t bar = value.find(L'|');
                if (bar != std::wstring::npos && bar > 0) {
                    DisplayIds ids;
                    for (const std::wstring& pair : Split(value.substr(bar + 1), L';')) {
                        size_t equals = pair.find_last_of(L'=');
                        int id;
                        if (equals != std::wstring::npos && equals > 0 && ParseInt(pair.substr(equals + 1), id)) {
                            ids[pair.substr(0, equals)] = id;
                        }
                    }
                    if (!ids.empty()) {
                        displayIds[value.substr(0, bar)] = ids;
                    }
                }
            }
        }
    }

    bool Save() const {
        std::vector<std::wstring> lines{
            L"# RDPLauncher settings",
            L"ShowOnTaskbar=" + BoolText(showOnTaskbar),
            L"TaskbarOffset=" + std::to_wstring(taskbarOffset),
            L"CurrentConnection=" + currentConnectionKey,
        };
        for (const Connection& connection : connections) {
            lines.push_back(L"Connection=" + connection.Key() +
                            (Trim(connection.customName).empty() ? L"" : L"|" + Trim(connection.customName)));
        }
        for (const auto& [setup, buttons] : displayButtons) {
            for (const Choice& button : buttons) {
                lines.push_back(L"DisplayButton=" + setup + L"|" + Join(button, L";"));
            }
        }
        for (const auto& [outputs, ids] : displayIds) {
            std::vector<std::wstring> pairs;
            for (const auto& [display, id] : ids) {
                pairs.push_back(display + L"=" + std::to_wstring(id));
            }
            lines.push_back(L"DisplayIds=" + outputs + L"|" + Join(pairs, L";"));
        }
        return WriteTextLines(Path(), lines, false);
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
};

// ---------------------------------------------------------------------------
// Starting Remote Desktop Connection (mstsc.exe) on the chosen displays.
//
// Since April 2026 Windows shows a security warning every time an .rdp file is opened, but not
// for a computer typed into Remote Desktop Connection or given with "mstsc /v". So a computer
// added by name is started with /v, after setting the displays in the settings Remote Desktop
// Connection uses for that (Default.rdp, which its own Display tab changes too). A saved .rdp file
// is copied with the displays set and the copy is opened, leaving the original as it was.

std::wstring DefaultRdpPath() {
    return KnownFolder(FOLDERID_Documents) + L"\\Default.rdp";
}

const std::vector<std::wstring>& DisplaySettingNames() {
    static const std::vector<std::wstring> names{L"screen mode id", L"use multimon", L"span monitors", L"selectedmonitors"};
    return names;
}

std::wstring SafeFileName(const std::wstring& name) {
    std::wstring safe;
    for (wchar_t c : name) {
        safe += (c < 32 || wcschr(L"<>:\"/\\|?*", c)) ? L'_' : c;
    }
    safe = Trim(safe);
    return safe.empty() ? L"Remote Desktop" : safe;
}

// Starts a full-screen session on the displays with the given IDs (the session's main display
// first), or on all displays when there are none. Returns an error message, or nothing.
std::optional<std::wstring> StartFullScreen(const Connection& connection, const std::vector<int>* displayIds) {
    auto setDisplays = [&](RdpFile& file) {
        file.Set(L"screen mode id", L"i", L"2");
        file.Set(L"use multimon", L"i", L"1");
        file.Set(L"span monitors", L"i", L"0");
        if (displayIds) {
            std::vector<std::wstring> ids;
            for (int id : *displayIds) {
                ids.push_back(std::to_wstring(id));
            }
            file.Set(L"selectedmonitors", L"s", Join(ids, L","));
        } else {
            file.Remove(L"selectedmonitors");
        }
    };

    if (!connection.rdpFile.empty()) {
        RdpFile file;
        if (!file.Load(connection.rdpFile)) {
            return L"Couldn't read " + connection.rdpFile;
        }
        file.DropSignatureIfCovers(DisplaySettingNames());
        setDisplays(file);
        // Remote Desktop Connection shows the file name in its title bar, so name it after the connection.
        std::wstring copy = AppDataFolder() + L"\\Sessions\\" + SafeFileName(connection.Name()) + L".rdp";
        if (!file.Save(copy)) {
            return L"Couldn't write " + copy;
        }
        if (!Launch(MstscPath(), L"\"" + copy + L"\"", LaunchOptions{})) {
            return std::wstring(L"Remote Desktop Connection didn't start.");
        }
        return std::nullopt;
    }

    std::wstring defaults = DefaultRdpPath();
    bool existed = FileExists(defaults);
    RdpFile file;
    if (existed) {
        // Never replace the settings with just the display ones because they couldn't be read
        // (OneDrive hasn't downloaded the file, or Remote Desktop Connection is writing it).
        if (!file.Load(defaults)) {
            return L"Couldn't read " + defaults;
        }
        // A copy of Default.rdp from before it was first changed.
        std::wstring backup = AppDataFolder() + L"\\Default.rdp.backup";
        if (!FileExists(backup)) {
            CreateDirectories(AppDataFolder());
            CopyFileW(defaults.c_str(), backup.c_str(), TRUE);
        }
    }
    setDisplays(file);
    if (!file.Save(defaults)) {
        return L"Couldn't write " + defaults;
    }
    if (!existed) {
        SetFileAttributesW(defaults.c_str(), FILE_ATTRIBUTE_HIDDEN);  // As Remote Desktop Connection keeps it.
    }
    // Computer names have no spaces (the add dialog doesn't take them).
    if (!Launch(MstscPath(), L"/v:" + connection.host, LaunchOptions{})) {
        return std::wstring(L"Remote Desktop Connection didn't start.");
    }
    return std::nullopt;
}

// Opens the connection's settings in Remote Desktop Connection, to change the ones other than
// displays: its .rdp file, or for a computer name the defaults (Default.rdp).
bool EditConnection(const Connection& connection) {
    std::wstring path = connection.rdpFile.empty() ? DefaultRdpPath() : connection.rdpFile;
    // Without a Default.rdp yet, just open Remote Desktop Connection; it saves one on connecting.
    return Launch(MstscPath(), FileExists(path) ? L"/edit \"" + path + L"\"" : L"", LaunchOptions{});
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// RDP Launcher: the taskbar widget, the display picker flyout and the app.

namespace {

// ---------------------------------------------------------------------------
// Icons, drawn to match the Windows 11 taskbar at any size.

// Outlines a monitor on its stand, filling the box; returns the inside of its screen.
gp::RectF DrawMonitor(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float stroke) {
    float half = stroke / 2;
    gp::RectF screen(box.X + half, box.Y + box.Height * 0.1f + half, box.Width - stroke, box.Height * 0.62f - stroke);
    DrawRoundedRect(graphics, color, screen, box.Width * 0.09f, stroke);
    gp::Pen pen(color, stroke);
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapRound);
    float middle = box.X + box.Width / 2;
    float baseY = box.Y + box.Height * 0.9f - half;
    graphics.DrawLine(&pen, middle, screen.GetBottom() + half, middle, baseY);
    graphics.DrawLine(&pen, box.X + box.Width * 0.3f, baseY, box.X + box.Width * 0.7f, baseY);
    screen.Inflate(-half, -half);
    return screen;
}

// A monitor with an arrow on its screen: Remote Desktop.
void DrawRemoteDesktop(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color) {
    float stroke = std::max(1.25f, std::min(box.Width, box.Height) * 0.08f);
    gp::RectF screen = DrawMonitor(graphics, box, color, stroke);
    gp::Pen pen(color, stroke);
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapRound);
    pen.SetLineJoin(gp::LineJoinRound);
    float left = screen.X + screen.Width * 0.32f;
    float right = screen.X + screen.Width * 0.68f;
    float top = screen.Y + screen.Height * 0.26f;
    float bottom = screen.Y + screen.Height * 0.74f;
    float head = screen.Width * 0.2f;
    graphics.DrawLine(&pen, left, bottom, right, top);
    gp::PointF arrowHead[] = {{right - head, top}, {right, top}, {right, top + head}};
    graphics.DrawLines(&pen, arrowHead, 3);
}

// A monitor outline with a number on its screen, for the display buttons.
void DrawNumberedMonitor(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, const std::wstring& number,
                         float fontPixels) {
    float stroke = std::max(1.25f, std::min(box.Width, box.Height) * 0.07f);
    gp::RectF screen = DrawMonitor(graphics, box, color, stroke);
    // The screen is shorter than a line of text, so draw the digits as a shape and center what's inked.
    gp::FontFamily family(L"Segoe UI Semibold");
    gp::GraphicsPath path;
    path.AddString(number.c_str(), -1, family.IsAvailable() ? &family : gp::FontFamily::GenericSansSerif(),
                   gp::FontStyleRegular, fontPixels, gp::PointF(0, 0), gp::StringFormat::GenericDefault());
    gp::RectF inked;
    path.GetBounds(&inked);
    if (inked.Width <= 0 || inked.Height <= 0) {
        return;
    }
    gp::Matrix transform;
    transform.Translate(screen.X + (screen.Width - inked.Width) / 2 - inked.X, screen.Y + (screen.Height - inked.Height) / 2 - inked.Y);
    path.Transform(&transform);
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
}

// A tick, as on a chosen display in the display picker.
void DrawCheck(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color) {
    float s = std::min(box.Width, box.Height);
    gp::Pen pen(color, std::max(1.5f, s * 0.12f));
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapRound);
    pen.SetLineJoin(gp::LineJoinRound);
    gp::PointF points[] = {{box.X + s * 0.2f, box.Y + s * 0.53f}, {box.X + s * 0.41f, box.Y + s * 0.73f}, {box.X + s * 0.8f, box.Y + s * 0.3f}};
    graphics.DrawLines(&pen, points, 3);
}

// ---------------------------------------------------------------------------
// The flyout above a display button: a map of this PC's displays with the ones the button uses
// highlighted. Clicking a display adds it to the button or takes it off.

class DisplayFlyout : public HoverFlyout {
public:
    std::function<void(const std::wstring& displayKey)> onToggle;

    bool Create() { return CreatePopup(L"RDPLauncher.Mod.DisplayFlyout"); }

    void SetContent(const std::wstring& connectionName, const Displays& displays, const Choice& chosen) {
        connectionName_ = connectionName;
        displays_ = displays;
        chosen_ = chosen;
        Invalidate();
    }

protected:
    int LogicalWidth() override { return 320; }
    int LogicalHeight() override { return 216; }

    void OnScaleChanged() override {
        titleFont_ = MakeFont(L"Segoe UI Semibold", 14.0f * scale_);
        detailFont_ = MakeFont(L"Segoe UI", 12.0f * scale_);
    }

    void OnHidden() override {
        HoverFlyout::OnHidden();
        hoveredKey_.clear();
        pressedKey_.clear();
    }

    void OnMouseMove(POINT point) override {
        std::wstring key = KeyAt(point);
        if (key != hoveredKey_) {
            hoveredKey_ = key;
            SetHandCursor(!key.empty());
            Invalidate();
        }
    }

    void OnMouseDown(POINT point) override { pressedKey_ = KeyAt(point); }

    void OnMouseUp(POINT point) override {
        std::wstring key = KeyAt(point);
        if (!key.empty() && key == pressedKey_ && onToggle) {
            onToggle(key);
        }
        pressedKey_.clear();
    }

    void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) override {
        if (!titleFont_) {
            OnScaleChanged();
        }
        std::vector<const LocalDisplay*> sessionOrder = InSessionOrder(displays_, chosen_);
        const LocalDisplay* mainDisplay = sessionOrder.empty() ? nullptr : sessionOrder[0];

        DrawTextLine(graphics, connectionName_ + L" on " + DescribeCount(sessionOrder.size()), *titleFont_, colors.foreground,
                     gp::RectF((float)Px(16), (float)Px(12), (float)(width - Px(32)), (float)Px(22)));
        DrawTextLine(graphics, L"Click a display to add or remove it.", *detailFont_, colors.secondary,
                     gp::RectF((float)Px(16), (float)Px(34), (float)(width - Px(32)), (float)Px(18)));

        for (const auto& [display, rect] : LayOutMap()) {
            DrawDisplay(graphics, *display, rect, Contains(chosen_, display->Key()), display == mainDisplay, colors);
        }

        // What the hovered display is, or what the main display is.
        const LocalDisplay* described = mainDisplay;
        for (const LocalDisplay& display : displays_) {
            if (display.Key() == hoveredKey_) {
                described = &display;
            }
        }
        if (described) {
            std::wstring text = described->name + L" · " + described->Resolution();
            if (described == mainDisplay) {
                text += L" · main display";
            } else if (!Contains(chosen_, described->Key())) {
                text += L" · not used";
            }
            DrawTextLine(graphics, text, *detailFont_, colors.secondary,
                         gp::RectF((float)Px(16), (float)(height - Px(36)), (float)(width - Px(32)), (float)Px(22)), TextAlign::Center);
        }
    }

private:
    // Where each display is drawn: the real arrangement, scaled to fit the map and centered.
    std::vector<std::pair<const LocalDisplay*, gp::RectF>> LayOutMap() const {
        std::vector<std::pair<const LocalDisplay*, gp::RectF>> layout;
        if (displays_.empty()) {
            return layout;
        }
        RECT client{};
        GetClientRect(hwnd_, &client);
        gp::RectF map((float)Px(16), (float)Px(62), (float)(client.right - Px(32)), (float)Px(110));
        LONG left = LONG_MAX, top = LONG_MAX, right = LONG_MIN, bottom = LONG_MIN;
        for (const LocalDisplay& display : displays_) {
            left = std::min(left, display.bounds.left);
            top = std::min(top, display.bounds.top);
            right = std::max(right, display.bounds.right);
            bottom = std::max(bottom, display.bounds.bottom);
        }
        float scale = std::min(map.Width / (float)std::max<LONG>(1, right - left), map.Height / (float)std::max<LONG>(1, bottom - top));
        float offsetX = map.X + (map.Width - (right - left) * scale) / 2;
        float offsetY = map.Y + (map.Height - (bottom - top) * scale) / 2;
        float gap = (float)Px(2);  // Separates displays that touch.
        for (const LocalDisplay& display : displays_) {
            gp::RectF rect(offsetX + (display.bounds.left - left) * scale, offsetY + (display.bounds.top - top) * scale,
                           display.Width() * scale, display.Height() * scale);
            rect.Inflate(-gap, -gap);
            layout.emplace_back(&display, rect);
        }
        return layout;
    }

    std::wstring KeyAt(POINT point) const {
        for (const auto& [display, rect] : LayOutMap()) {
            if (rect.Contains((float)point.x, (float)point.y)) {
                return display->Key();
            }
        }
        return {};
    }

    // A chosen display is filled with the accent color and ticked; the session's main display also
    // gets a taskbar along its bottom edge.
    void DrawDisplay(gp::Graphics& graphics, const LocalDisplay& display, const gp::RectF& rect, bool chosen, bool main,
                     const Palette& colors) {
        bool hovered = display.Key() == hoveredKey_;
        float radius = (float)Px(4);
        if (chosen) {
            FillRoundedRect(graphics, colors.accent, rect, radius);
            if (hovered) {
                FillRoundedRect(graphics, colors.hover, rect, radius);
            }
        } else {
            FillRoundedRect(graphics, hovered ? colors.track : colors.hover, rect, radius);
            DrawRoundedRect(graphics, colors.track, rect, radius, std::max(1.0f, (float)Px(1)));
        }
        gp::Color mark = chosen ? colors.onAccent : colors.secondary;
        if (main) {
            float barHeight = std::max(2.0f, std::min((float)Px(5), rect.Height * 0.1f));
            FillRoundedRect(graphics, WithAlpha(150, mark),
                            gp::RectF(rect.X + Px(3), rect.GetBottom() - barHeight - Px(3), rect.Width - Px(6), barHeight), barHeight / 2);
        }
        float check = std::min((float)Px(20), std::min(rect.Width, rect.Height) * 0.5f);
        if (chosen && check >= Px(8)) {
            DrawCheck(graphics, gp::RectF(rect.X + (rect.Width - check) / 2, rect.Y + (rect.Height - check) / 2, check, check), mark);
        }
    }

    std::wstring connectionName_;
    Displays displays_;
    Choice chosen_;
    std::wstring hoveredKey_;
    std::wstring pressedKey_;
    std::unique_ptr<gp::Font> titleFont_;
    std::unique_ptr<gp::Font> detailFont_;
};

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

class RdpWidget : public TaskbarWidget {
public:
    std::function<void()> onConnectionClicked;
    std::function<void(int steps)> onConnectionStep;
    std::function<void(int button)> onConnect;
    std::function<void(int button, const std::wstring& displayKey)> onToggleDisplay;
    std::function<void()> onMenu;

    bool Create(int offset) {
        if (!TaskbarWidget::Create(offset) || !flyout_.Create()) {
            return false;
        }
        flyout_.onToggle = [this](const std::wstring& key) {
            if (flyoutButton_ >= 0 && onToggleDisplay) {
                onToggleDisplay(flyoutButton_, key);
            }
        };
        return true;
    }

    void SetContent(const WidgetContent& content) {
        content_ = content;
        if (flyoutButton_ >= 0 && flyoutButton_ < (int)content_.buttons.size()) {
            flyout_.SetContent(content_.title, content_.displays, content_.buttons[flyoutButton_]);
        } else {
            flyout_.Hide();
        }
        ContentChanged();
    }

    void HideFlyout() { flyout_.Hide(); }

protected:
    static constexpr int kNone = -1;
    static constexpr int kConnection = 0;  // Display button i is section i + 1.
    static constexpr UINT_PTR kHoverTimer = 2;

    struct Section {
        int id;
        int x;
        int width;
    };

    int DefaultOffset(int barWidth, int width) override {
        // With centered taskbar icons the left end is free, but the Widgets (weather) button sits in
        // the corner, so start to its right. With left-aligned icons the middle is free. Either way
        // leave room for AudioFlipper's widget, which starts in the same places.
        return TaskbarIconsLeftAligned() ? barWidth / 2 + Px(140) : Px(180 + 280);
    }

    void OnScaleChanged() override {
        nameFont_ = MakeFont(L"Segoe UI Semibold", 12.0f * Scale());
        detailFont_ = MakeFont(L"Segoe UI", 12.0f * Scale());
        measuredWidths_.clear();
    }

    int LayOut() override {
        sections_.clear();
        int x = 0;
        sections_.push_back({kConnection, 0, MeasureConnectionWidth()});
        x += sections_.back().width;
        for (size_t i = 0; i < content_.buttons.size(); i++) {
            sections_.push_back({(int)i + 1, x, Px(40)});
            x += Px(40);
        }
        return x;
    }

    void Draw(gp::Graphics& graphics, int width, int height, bool light) override {
        gp::Color foreground = light ? Argb(255, 28, 28, 28) : Argb(255, 255, 255, 255);
        gp::Color secondary = light ? Argb(255, 90, 90, 90) : Argb(255, 205, 205, 205);
        gp::Color highlight = WithAlpha(light ? 18 : 24, foreground);
        for (const Section& section : sections_) {
            if (!Dragging() && hovered_ == section.id) {
                FillRoundedRect(graphics, highlight,
                                gp::RectF((float)(section.x + Px(2)), (float)Px(4), (float)(section.width - Px(4)), (float)(height - Px(8))),
                                (float)Px(4));
            }
            if (section.id == kConnection) {
                DrawConnection(graphics, section, height, foreground, secondary);
                continue;
            }
            int button = section.id - 1;
            if (button == 0) {
                gp::Pen divider(WithAlpha(light ? 50 : 60, foreground), 1);
                graphics.DrawLine(&divider, (float)section.x, (float)Px(14), (float)section.x, (float)(height - Px(14)));
            }
            gp::Color iconColor = content_.busyButton == button ? WithAlpha(110, foreground) : foreground;
            gp::RectF icon(section.x + (section.width - Px(26)) / 2.0f, height / 2.0f - Px(12), (float)Px(26), (float)Px(24));
            DrawNumberedMonitor(graphics, icon, iconColor, std::to_wstring(content_.buttons[button].size()), 11.0f * Scale());
        }
    }

    void OnHover(POINT point) override {
        int section = SectionAt(point);
        if (section == hovered_) {
            return;
        }
        hovered_ = section;
        Render();
        // Display buttons open their flyout after a moment; an open flyout closes itself once the
        // mouse is away from it and its button.
        KillTimer(Handle(), kHoverTimer);
        if (section > kConnection) {
            hoverTarget_ = section;
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
        int button = hoverTarget_ - 1;
        if (Visible() && !Pressed() && PtInRect(&bounds, cursor) && button >= 0 && button < (int)content_.buttons.size()) {
            flyoutButton_ = button;
            flyout_.SetContent(content_.title, content_.displays, content_.buttons[button]);
            flyout_.ShowAbove(bounds);
        }
    }

    void OnClick(POINT point, MouseButton mouseButton) override {
        KillTimer(Handle(), kHoverTimer);
        flyout_.Hide();
        int section = SectionAt(point);
        if (mouseButton == MouseButton::Right) {
            if (onMenu) {
                onMenu();
            }
        } else if (mouseButton == MouseButton::Left && section == kConnection) {
            if (onConnectionClicked) {
                onConnectionClicked();
            }
        } else if (mouseButton == MouseButton::Left && section > kConnection) {
            if (onConnect) {
                onConnect(section - 1);
            }
        }
    }

    void OnWheel(POINT point, int notches) override {
        if (SectionAt(point) == kConnection && onConnectionStep) {
            onConnectionStep(-notches);
        }
    }

    void OnDragStart() override {
        KillTimer(Handle(), kHoverTimer);
        flyout_.Hide();
    }

    void OnPlaced() override {
        if (flyout_.Visible()) {
            flyout_.SetAnchor(SectionScreenBounds(flyoutButton_ + 1));
        }
    }

    void OnHidden() override { flyout_.Hide(); }

private:
    int SectionAt(POINT point) const {
        for (const Section& section : sections_) {
            if (point.x >= section.x && point.x < section.x + section.width) {
                return section.id;
            }
        }
        return kNone;
    }

    RECT SectionScreenBounds(int id) const {
        for (const Section& section : sections_) {
            if (section.id == id) {
                return ScreenBoundsOf(section.x, section.width);
            }
        }
        return {};
    }

    // Fits the name and address in full, like the weather widget sizes itself to its text.
    int MeasureConnectionWidth() {
        std::wstring key = content_.title + L"\n" + content_.detail;
        auto found = measuredWidths_.find(key);
        if (found != measuredWidths_.end()) {
            return found->second;
        }
        gp::Bitmap measuring(1, 1, PixelFormat32bppPARGB);
        gp::Graphics graphics(&measuring);
        graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
        float textWidth = std::max(MeasureTextWidth(graphics, content_.title, *nameFont_),
                                   MeasureTextWidth(graphics, content_.detail, *detailFont_));
        int width = std::max(Px(120), std::min(Px(400), Px(44) + (int)std::ceil(textWidth) + Px(4) + Px(10)));
        measuredWidths_[key] = width;
        return width;
    }

    // The Remote Desktop icon, then the name over the address, like the weather widget's two lines;
    // just the name, centered, without an address.
    void DrawConnection(gp::Graphics& graphics, const Section& section, int height, const gp::Color& foreground,
                        const gp::Color& secondary) {
        float middle = height / 2.0f;
        float iconSize = (float)Px(24);
        DrawRemoteDesktop(graphics, gp::RectF(section.x + (float)Px(10), middle - iconSize / 2, iconSize, iconSize), foreground);
        float textLeft = section.x + (float)Px(44);
        float textWidth = section.x + section.width - textLeft - Px(10);
        // The section is sized to fit the text, so it's only cut short when it's as wide as it gets.
        bool trim = section.width >= Px(400);
        if (content_.detail.empty()) {
            DrawTextLine(graphics, content_.title, *nameFont_, foreground, gp::RectF(textLeft, middle - Px(9), textWidth, (float)Px(18)),
                         TextAlign::Left, trim);
            return;
        }
        DrawTextLine(graphics, content_.title, *nameFont_, foreground, gp::RectF(textLeft, middle - Px(18), textWidth, (float)Px(18)),
                     TextAlign::Left, trim);
        DrawTextLine(graphics, content_.detail, *detailFont_, secondary, gp::RectF(textLeft, middle + Px(1), textWidth, (float)Px(17)),
                     TextAlign::Left, trim);
    }

    WidgetContent content_;
    DisplayFlyout flyout_;
    std::vector<Section> sections_;
    std::map<std::wstring, int> measuredWidths_;
    std::unique_ptr<gp::Font> nameFont_;
    std::unique_ptr<gp::Font> detailFont_;
    int hovered_ = kNone;
    int hoverTarget_ = kNone;
    int flyoutButton_ = -1;
};

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
        // RDPLauncher.exe and the mod show the same widget; while the exe runs, the mod waits.
        if (!instance_.TryAcquire()) {
            Wh_Log(L"RDPLauncher.exe is running; waiting for it to exit");
            SetTimer(messages_.Handle(), kWaitForAppTimer, 3000, nullptr);
            return true;
        }
        return StartWidget();
    }

    ~RdpApp() {
        widget_.Destroy();
        SetEvent(cancel_);  // A display-number read in progress closes mstsc and stops.
        workers_.WaitAll(5000);
        CloseHandle(cancel_);
    }

private:
    static constexpr UINT_PTR kRefreshTimer = 1;
    static constexpr UINT_PTR kCooldownTimer = 2;
    static constexpr UINT_PTR kWaitForAppTimer = 3;
    static constexpr UINT kIdsReadMessage = WM_APP + 1;
    // Keeps a button busy for a moment after starting a session, so a double-click starts one session.
    static constexpr UINT kStartCooldownMilliseconds = 2000;

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

    bool StartWidget() {
        if (!widget_.Create(settings_.taskbarOffset) || !popup_.Create()) {
            return false;
        }
        widget_.onOffsetChanged = [this](int offset) {
            settings_.taskbarOffset = offset;
            Save();
        };
        widget_.onConnect = [this](int button) { ConnectWithButton(button); };
        widget_.onConnectionClicked = [this]() {
            // Goes to the next computer (the menu is on right-click); before there are any, it's the
            // widget's "Click to add a computer".
            if (settings_.connections.empty()) {
                AddComputer();
            } else {
                StepConnection(1);
            }
        };
        widget_.onConnectionStep = [this](int steps) { StepConnection(steps); };
        widget_.onToggleDisplay = [this](int button, const std::wstring& key) { ToggleDisplay(button, key); };
        widget_.onMenu = [this]() { ShowMenu(); };
        RefreshDisplays();
        ConnectionChanged();
        widget_.SetShown(true);
        SetTimer(messages_.Handle(), kRefreshTimer, 1000, nullptr);
        return true;
    }

    void OnTimer(UINT_PTR timer) {
        if (timer == kRefreshTimer) {
            RefreshDisplays();
            RefreshWidget();
        } else if (timer == kCooldownTimer) {
            EndStart();
        } else if (timer == kWaitForAppTimer && instance_.TryAcquire()) {
            KillTimer(messages_.Handle(), kWaitForAppTimer);
            settings_ = Settings();
            settings_.Load();  // The exe may have changed them.
            StartWidget();
        }
    }

    void Save() {
        if (!settings_.Save()) {
            popup_.ShowMessage(L"Couldn't save settings", Settings::Path());
        }
    }

    // Notices displays being added, removed or rearranged, and picks the display buttons for the
    // new arrangement.
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
        if (signature != lastSignature_) {
            lastSignature_ = signature;
            widget_.SetContent(content);
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
                               L"Guessed them. Details: %LOCALAPPDATA%\\RDPLauncher\\display-numbers.log");
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
        if (std::optional<std::wstring> error = StartFullScreen(connection, displayIds)) {
            popup_.ShowMessage(L"Couldn't start Remote Desktop", *error);
            EndStart();
            return;
        }
        SetTimer(messages_.Handle(), kCooldownTimer, kStartCooldownMilliseconds, nullptr);
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

        Menu menu;
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

        // Linking the widget with AudioFlipper's into one, on either side; offered while AudioFlipper
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
        widget_.HideFlyout();
        // Above the widget, which keeps itself on top and would otherwise cover the menu.
        menu.Show(widget_.Handle(), {cursor.x, bounds.top});
    }

    Settings settings_;
    SingleInstance instance_{L"Local\\RDPLauncher.SingleInstance"};
    MessageWindow messages_;
    RdpWidget widget_;
    MessagePopup popup_;
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
    bool starting_ = false;
    int busyButton_ = -1;
};

UiThread g_uiThread;

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    return g_uiThread.Start([]() -> std::shared_ptr<void> {
        auto app = std::make_shared<RdpApp>();
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
