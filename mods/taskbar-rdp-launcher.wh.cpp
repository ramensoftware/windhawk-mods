// ==WindhawkMod==
// @id              taskbar-rdp-launcher
// @name            Taskbar RDP Launcher
// @description     A taskbar widget that starts Remote Desktop full screen on the number of displays you pick
// @version         1.2.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -loleaut32 -luuid -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar RDP Launcher

![The widget on the taskbar, with a computer and buttons for 1, 2 and 3 displays](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-rdp-launcher/screenshot-1.2.png)

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

## Linking with the other widgets

![Taskbar Snipper, Taskbar Audio Flipper and Taskbar RDP Launcher linked on the taskbar](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-widgets/linked.png)

From the left: [Taskbar Snipper](https://windhawk.net/mods/taskbar-snipper),
[Taskbar Audio Flipper](https://windhawk.net/mods/taskbar-audio-flipper) and
[Taskbar RDP Launcher](https://windhawk.net/mods/taskbar-rdp-launcher).

With the Taskbar Audio Flipper or Taskbar Snipper mod running too, their widgets can be joined with
this one into one: right-click any of them, open **Link widgets**, and choose **Link with** the other. Linked,
they sit side by side with a divider between them, and dragging any of them moves them all; **Move
this widget left** and **right** change their order. A widget that hides leaves no gap.

Update the linked widgets' mods together (Taskbar Audio Flipper and Taskbar RDP Launcher 1.2 or
later, Taskbar Snipper 1.1 or later): their widgets are inside the taskbar now, where older versions
don't look for them, so an older one overlaps the others and doesn't follow them when they're
dragged, reordered or unlinked.

## Settings

The computers, their names and the display buttons are kept in the mod's storage, which Windhawk
removes along with the mod. The first time it runs, the mod takes on the computers and display
buttons of the RDPLauncher app, if you have it (from `%APPDATA%\RDPLauncher\settings.txt`, which it
doesn't change). Use the app or the mod, not both: each shows its own widget.
*/
// ==/WindhawkModReadme==

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

#include <algorithm>
#include <atomic>
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

const wchar_t kWidgetClassName[] = L"RDPLauncher.Mod.Widget";
const wchar_t kWidgetTitle[] = L"Taskbar RDP Launcher";
// This widget's place among the widgets that can link (WidgetLink::kMembers), and the one it was
// linked with as a pair before there were three.
constexpr int kSelfMember = 1;
constexpr int kLegacyPartner = 0;

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Shared by the Taskbar Audio Flipper, Taskbar RDP Launcher and Taskbar Snipper mods
// (the same code is in all three, each keeping the parts it uses): the taskbar
// widget window, popups, menus, the text prompt, the link between the widgets, the
// mod's storage and launching programs.

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

bool IsWow64() {
    BOOL wow64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &wow64) && wow64;
}

std::wstring WindowsFolder() {
    WCHAR path[MAX_PATH];
    UINT length = GetSystemWindowsDirectoryW(path, MAX_PATH);
    return length > 0 && length < MAX_PATH ? std::wstring(path, length) : L"C:\\Windows";
}

// System32, for starting the programs in it. Explorer and the app are 64-bit on 64-bit Windows,
// where System32 is the real one; a 32-bit process there gets the 32-bit SysWOW64 instead, and
// "Sysnative" reaches the real System32 from it.
std::wstring NativeSystemFolder() {
    return WindowsFolder() + (IsWow64() ? L"\\Sysnative" : L"\\System32");
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

// The Windows settings the widget and popups follow. They're read once, and again when Windows
// says a setting changed (WM_SETTINGCHANGE), rather than on every placement update or paint.
struct ShellSettings {
    bool lightTaskbar = false;      // "Choose your default Windows mode", which the taskbar follows.
    bool lightApps = true;          // "Choose your default app mode", which the popups follow.
    bool iconsLeftAligned = false;  // Taskbar icons left-aligned rather than centered (Windows 11).

    static ShellSettings Read() {
        const wchar_t* personalize = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        ShellSettings settings;
        settings.lightTaskbar = ReadUserFlag(personalize, L"SystemUsesLightTheme", false);
        settings.lightApps = ReadUserFlag(personalize, L"AppsUseLightTheme", true);
        settings.iconsLeftAligned =
            !ReadUserFlag(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", true);
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

bool TaskbarUsesLightTheme() {
    return CurrentShellSettings().lightTaskbar;
}

bool AppsUseLightTheme() {
    return CurrentShellSettings().lightApps;
}

bool TaskbarIconsLeftAligned() {
    return CurrentShellSettings().iconsLeftAligned;
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
// Taskbar

// Whether this is an Explorer process.
bool InExplorer() {
    static const bool inExplorer = []() {
        WCHAR path[MAX_PATH];
        DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
        if (length == 0 || length >= MAX_PATH) {
            return false;
        }
        const wchar_t* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }();
    return inExplorer;
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

// Explorer's taskbar (in Explorer, this process's own if it has one). Other apps' Shell_TrayWnd
// windows don't count: taskbar replacements such as RetroBar make one, so tray icons reach them.
HWND TaskbarWindow() {
    HWND explorers = nullptr;
    for (HWND window = FindWindowExW(nullptr, nullptr, L"Shell_TrayWnd", nullptr); window;
         window = FindWindowExW(nullptr, window, L"Shell_TrayWnd", nullptr)) {
        DWORD process = 0;
        GetWindowThreadProcessId(window, &process);
        if (process == GetCurrentProcessId() && InExplorer()) {
            return window;
        }
        if (!explorers && IsExplorerProcess(process)) {
            explorers = window;
        }
    }
    return explorers;
}

// Whether this is an Explorer process for folder windows ("Launch folder windows in a separate
// process"), started with "/factory,". Those never have the taskbar.
bool InFolderWindowProcess() {
    std::wstring commandLine = GetCommandLineW();
    std::transform(commandLine.begin(), commandLine.end(), commandLine.begin(), towlower);
    return InExplorer() && commandLine.find(L"/factory,") != std::wstring::npos;
}

// The taskbar to put the widget in, if there is one. In Explorer, only the process that has it:
// Windhawk loads the mod into Explorer's folder-window processes too. (The app puts the widget in
// Explorer's taskbar from its own process.)
HWND WidgetTaskbar() {
    HWND taskbar = TaskbarWindow();
    DWORD process = 0;
    if (taskbar && InExplorer() && GetWindowThreadProcessId(taskbar, &process) && process != GetCurrentProcessId()) {
        return nullptr;
    }
    return taskbar;
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
    gp::Color background, foreground, secondary, accent, track, border, hover, onAccent;

    static Palette Current() {
        if (AppsUseLightTheme()) {
            return {Argb(255, 243, 243, 243), Argb(255, 27, 27, 27), Argb(255, 96, 96, 96),
                    Argb(255, 0, 95, 184), Argb(255, 196, 196, 196), Argb(255, 213, 213, 213),
                    Argb(14, 0, 0, 0), Argb(255, 255, 255, 255)};
        }
        return {Argb(255, 32, 32, 32), Argb(255, 255, 255, 255), Argb(255, 197, 197, 197),
                Argb(255, 96, 205, 255), Argb(255, 80, 80, 80), Argb(255, 58, 58, 58),
                Argb(16, 255, 255, 255), Argb(255, 0, 0, 0)};
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

    void SetHandCursor(bool hand) { handCursor_ = hand; }

    virtual void Paint(gp::Graphics& graphics, int width, int height, const Palette& colors) = 0;
    virtual void OnScaleChanged() {}
    virtual void OnHidden() {}
    virtual void OnMouseEnter() {}
    virtual void OnMouseMove(POINT point) {}
    virtual void OnMouseLeave() {}
    virtual void OnMouseDown(POINT point) {}
    virtual void OnMouseUp(POINT point) {}
    virtual void OnTimer(UINT_PTR timer) {}

    float scale_ = 1;
    bool roundedByWindows_ = false;
    bool trackingMouse_ = false;
    bool handCursor_ = false;
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
// Linking the Taskbar Audio Flipper, Taskbar RDP Launcher and Taskbar Snipper widgets (the same
// code is in all three mods). Linked widgets sit side by side as one, in the order chosen, with a
// divider between them, and dragging any of them moves them all.
//
// Each mod keeps its own copy of the group (which widgets are in it, left to right, and where it
// is) in its storage, and shows it to the others as properties of its widget window, stamped with
// when it last changed: the newest copy wins. Each widget shows its id on its window too, finds the
// others by theirs (the ones it knows, by their window class), and posts them a message whenever
// its group, size or place changes.

namespace WidgetLink {

struct Member {
    const wchar_t* name;         // As the menus show it.
    const wchar_t* windowClass;  // Of its widget window.
};

// Every widget that can link, by id (its place here). Ids stay as they are: a new widget goes at
// the end, up to 15 of them.
constexpr Member kMembers[] = {
    {L"Audio Flipper", L"AudioFlipper.Mod.Widget"},
    {L"RDP Launcher", L"RDPLauncher.Mod.Widget"},
    {L"Snipper", L"TaskbarSnipper.Mod.Widget"},
};
constexpr int kMemberCount = ARRAYSIZE(kMembers);

// The format of the group shown to the other widgets and kept in storage, in the top four bits of
// its order. A widget skips copies in a format it doesn't know. Widgets added later don't need a new
// one: an id a widget doesn't know stays in the group as it is.
constexpr unsigned kFormat = 1;
constexpr int kOrderSlots = 7;  // Widgets in a group: four bits each, below the format.

struct Group {
    std::vector<int> order;  // The widgets in it, by id, left to right.
    int offset = -1;         // Its left edge from the taskbar's left edge; negative for the default.
    int stamp = 0;           // When it last changed, in seconds since 2020: the newest copy wins.

    bool Has(int member) const { return std::find(order.begin(), order.end(), member) != order.end(); }

    // Whether this widget is linked with another.
    bool Linked() const { return order.size() >= 2 && Has(kSelfMember); }

    // The order as one number: the format, then four bits for each widget, left to right (its id
    // plus one; none after the last).
    int PackOrder() const {
        unsigned packed = kFormat << 28;
        for (size_t i = 0; i < order.size() && i < (size_t)kOrderSlots; i++) {
            packed |= (unsigned)((order[i] + 1) & 15) << (4 * i);
        }
        return (int)packed;
    }

    // Reads the order. False for a format this widget doesn't know.
    bool UnpackOrder(int packed) {
        if ((unsigned)packed >> 28 != kFormat) {
            return false;
        }
        order.clear();
        for (int i = 0; i < kOrderSlots; i++) {
            int member = (int)(((unsigned)packed >> (4 * i)) & 15) - 1;
            if (member < 0) {
                break;
            }
            if (!Has(member)) {
                order.push_back(member);
            }
        }
        return true;
    }
};

// What each widget shows on its window: its id (so any widget can find it, even one added after
// this one), and its copy of the group.
const wchar_t kMemberProperty[] = L"TaskbarWidgetLink.Mod.Member";
const wchar_t kOrderProperty[] = L"TaskbarWidgetLink.Mod.Order";
const wchar_t kOffsetProperty[] = L"TaskbarWidgetLink.Mod.Offset";
const wchar_t kStampProperty[] = L"TaskbarWidgetLink.Mod.GroupStamp";

// Posted to the other widgets when the group, or this widget's size or place, changes.
UINT ChangedMessage() {
    static UINT message = RegisterWindowMessageW(L"TaskbarWidgetLink.Mod.GroupChanged");
    return message;
}

// A stamp for a change made now, newer than the given one even within the same second.
int NewStamp(int newerThan) {
    SYSTEMTIME epoch{2020, 1, 3, 1};
    FILETIME epochTime;
    FILETIME now;
    SystemTimeToFileTime(&epoch, &epochTime);
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER start{{epochTime.dwLowDateTime, epochTime.dwHighDateTime}};
    ULARGE_INTEGER current{{now.dwLowDateTime, now.dwHighDateTime}};
    int seconds = current.QuadPart > start.QuadPart ? (int)((current.QuadPart - start.QuadPart) / 10000000) : 0;
    return std::max(seconds, newerThan + 1);
}

Group Load() {
    Group group;
    int stamp = Storage::GetInt(L"LinkGroupStamp", 0);
    if (stamp != 0 && group.UnpackOrder(Storage::GetInt(L"LinkGroupOrder", 0))) {
        group.offset = Storage::GetInt(L"LinkGroupOffset", -1);
        group.stamp = stamp;
        return group;
    }
    // Before there were three, two widgets linked as a pair, kept as Link (on or off, whether this
    // widget was on the left, and the offset) and LinkStamp. The stamp counts even when they were
    // unlinked: unlinking while the other widget's mod wasn't running still wins.
    group = {};
    if (kLegacyPartner >= 0) {
        int pair = Storage::GetInt(L"Link", 0);
        if (pair & 1) {
            group.order = (pair & 2) ? std::vector<int>{kSelfMember, kLegacyPartner} : std::vector<int>{kLegacyPartner, kSelfMember};
            group.offset = (pair >> 2) - 1;
        }
        group.stamp = Storage::GetInt(L"LinkStamp", 0);
    }
    return group;
}

void Save(const Group& group) {
    Storage::SetInt(L"LinkGroupOrder", group.PackOrder());
    Storage::SetInt(L"LinkGroupOffset", group.offset);
    Storage::SetInt(L"LinkGroupStamp", group.stamp);
    // The pair link from before is in the group now.
    Storage::Delete(L"Link");
    Storage::Delete(L"LinkStamp");
}

// Shows this widget's id and copy of the group on its window, for the other widgets to read.
void Publish(HWND window, const Group& group) {
    SetPropW(window, kMemberProperty, (HANDLE)(INT_PTR)(kSelfMember + 1));
    SetPropW(window, kOrderProperty, (HANDLE)(INT_PTR)group.PackOrder());
    SetPropW(window, kOffsetProperty, (HANDLE)(INT_PTR)(std::max(-1, group.offset) + 1));
    SetPropW(window, kStampProperty, (HANDLE)(INT_PTR)group.stamp);
}

void Unpublish(HWND window) {
    RemovePropW(window, kMemberProperty);
    RemovePropW(window, kOrderProperty);
    RemovePropW(window, kOffsetProperty);
    RemovePropW(window, kStampProperty);
}

BOOL CALLBACK AddOtherWidget(HWND window, LPARAM widgets) {
    int member = (int)(INT_PTR)GetPropW(window, kMemberProperty) - 1;
    if (member >= 0 && member != kSelfMember) {
        ((std::vector<std::pair<int, HWND>>*)widgets)->emplace_back(member, window);
    }
    return TRUE;
}

// The widget windows of the other widgets that are running, with their ids: top-level windows, and
// windows inside the taskbar (a widget's mod that Windhawk loads into Explorer puts it there).
std::vector<std::pair<int, HWND>> OtherWidgets() {
    std::vector<std::pair<int, HWND>> widgets;
    EnumWindows(AddOtherWidget, (LPARAM)&widgets);
    if (HWND taskbar = TaskbarWindow()) {
        EnumChildWindows(taskbar, AddOtherWidget, (LPARAM)&widgets);
    }
    return widgets;
}

HWND FindMember(int member) {
    if (member == kSelfMember || member < 0) {
        return nullptr;
    }
    if (member < kMemberCount) {
        if (HWND window = FindWindowW(kMembers[member].windowClass, nullptr)) {
            return window;
        }
        HWND taskbar = TaskbarWindow();
        return taskbar ? FindWindowExW(taskbar, nullptr, kMembers[member].windowClass, nullptr) : nullptr;
    }
    // One added after this widget: by the id it shows.
    for (const auto& [id, window] : OtherWidgets()) {
        if (id == member) {
            return window;
        }
    }
    return nullptr;
}

// Whether the widget's mod is running (this one always is).
bool Running(int member) {
    return member == kSelfMember || FindMember(member) != nullptr;
}

// The newest copy of the group among the other widgets that are running.
std::optional<Group> ReadNewest() {
    std::optional<Group> newest;
    for (const auto& [member, window] : OtherWidgets()) {
        int stamp = (int)(INT_PTR)GetPropW(window, kStampProperty);
        Group group;
        if (stamp != 0 && (!newest || stamp > newest->stamp) && group.UnpackOrder((int)(INT_PTR)GetPropW(window, kOrderProperty))) {
            group.offset = (int)(INT_PTR)GetPropW(window, kOffsetProperty) - 1;
            group.stamp = stamp;
            newest = group;
        }
    }
    return newest;
}

// The widget, when it's on the taskbar right now.
bool TryGetBounds(int member, RECT& bounds) {
    HWND window = FindMember(member);
    return window && IsWindowVisible(window) && GetWindowRect(window, &bounds) && bounds.right > bounds.left;
}

void NotifyOthers() {
    for (const auto& [member, window] : OtherWidgets()) {
        PostMessageW(window, ChangedMessage(), 0, 0);
    }
}

}  // namespace WidgetLink

// ---------------------------------------------------------------------------
// The taskbar widget

// The widget's window inside the taskbar, owned by a thread that does nothing else. Windows joins
// the input of a child window's thread with its parent's, so while the thread that owns the
// widget's window is busy, the taskbar gets no mouse or keyboard input either. This thread only
// shows the pictures the UI thread draws, and passes the mouse on to it, so it's never busy.
class WidgetWindow {
public:
    // Messages to the UI thread's window given to Start: the widget's window is in a taskbar
    // (wParam is the window, or null if it couldn't be made), or is gone (wParam), along with its
    // taskbar or for good. Its mouse messages, and the other widgets' notices, come as they are.
    static constexpr UINT kAttachedMessage = WM_APP + 0x200;
    static constexpr UINT kDetachedMessage = WM_APP + 0x201;
    // Its mouse messages (WM_MOUSEFIRST to WM_MOUSELAST) come as this plus their place in that range,
    // as a menu's modal loop takes every message in that range from its thread's queue for itself,
    // even those posted to its other windows.
    static constexpr UINT kMouseMessage = WM_APP + 0x210;

    ~WidgetWindow() { Stop(); }

    bool Start(HWND input) {
        input_ = input;
        windowClass_ = RegisterWidgetClass();
        if (!windowClass_) {
            return false;
        }
        ready_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId_);
        if (!thread_) {
            Stop();
            return false;
        }
        WaitForSingleObject(ready_, INFINITE);  // Its message queue is there from then on.
        return true;
    }

    // Destroys the window and ends the thread.
    void Stop() {
        if (thread_) {
            PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
            WaitForSingleObject(thread_, INFINITE);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
        if (ready_) {
            CloseHandle(ready_);
            ready_ = nullptr;
        }
        if (picture_) {
            DeleteObject(picture_);
            picture_ = nullptr;
        }
    }

    // Puts the window in the taskbar, unless it's in one already.
    void Attach(HWND taskbar) { PostThreadMessageW(threadId_, kAttachMessage, 0, (LPARAM)taskbar); }

    // Shows the picture (which this then owns) at the position in the taskbar's client area. With
    // notify, the linked widgets are told once it shows (so they see where it is now).
    void Present(HBITMAP picture, POINT position, SIZE size, bool notify) {
        AcquireSRWLockExclusive(&lock_);
        HBITMAP old = picture_;
        picture_ = picture;
        position_ = position;
        size_ = size;
        ReleaseSRWLockExclusive(&lock_);
        if (old) {
            DeleteObject(old);
        }
        if (HWND window = window_) {
            PostMessageW(window, kPresentMessage, notify, 0);
        }
    }

    // The latest point the mouse moved to, when the UI thread gets the move posted to it.
    LPARAM TakeMouseMove() {
        movePending_ = false;
        return movePoint_;
    }

    // Shows or hides the window; with notify, the linked widgets are told after.
    void SetVisible(bool visible, bool notify) {
        if (HWND window = window_) {
            PostMessageW(window, kVisibleMessage, visible, notify);
        }
    }

private:
    static constexpr UINT kAttachMessage = WM_APP + 0x202;   // To the thread.
    static constexpr UINT kPresentMessage = WM_APP + 0x203;  // To the window.
    static constexpr UINT kVisibleMessage = WM_APP + 0x204;  // To the window.

    static const wchar_t* RegisterWidgetClass() {
        std::vector<std::wstring>& classes = RegisteredClasses();
        if (std::find(classes.begin(), classes.end(), kWidgetClassName) != classes.end()) {
            return kWidgetClassName;
        }
        WNDCLASSEXW windowClass{sizeof(windowClass)};
        windowClass.hInstance = ModuleInstance();
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        windowClass.lpszClassName = kWidgetClassName;
        windowClass.lpfnWndProc = WindowProc;
        if (!RegisterClassExW(&windowClass)) {
            Wh_Log(L"Couldn't register window class %s: error %u", kWidgetClassName, GetLastError());
            return nullptr;
        }
        classes.push_back(kWidgetClassName);
        return kWidgetClassName;
    }

    static DWORD WINAPI ThreadProc(void* parameter) {
        auto self = (WidgetWindow*)parameter;
        MSG message;
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // Creates the message queue.
        SetEvent(self->ready_);
        while (GetMessageW(&message, nullptr, 0, 0) > 0) {
            if (!message.hwnd && message.message == kAttachMessage) {
                self->Create((HWND)message.lParam);
            } else {
                DispatchMessageW(&message);
            }
        }
        if (HWND window = self->window_) {
            DestroyWindow(window);
        }
        return 0;
    }

    void Create(HWND taskbar) {
        if (HWND window = window_) {
            PostMessageW(input_, kAttachedMessage, (WPARAM)window, 0);  // There already.
            return;
        }
        // Layered, as only a layered window draws over the content of the Windows 11 taskbar; and
        // not mirrored when the taskbar is (for right-to-left languages), as it's drawn as it is.
        HWND window = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOPARENTNOTIFY | WS_EX_NOINHERITLAYOUT, windowClass_, kWidgetTitle,
                                      WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1, taskbar, nullptr, ModuleInstance(), this);
        if (window) {
            // Only needed when the app runs as administrator: lets the other widgets' messages
            // through.
            ChangeWindowMessageFilterEx(window, WidgetLink::ChangedMessage(), MSGFLT_ALLOW, nullptr);
            window_ = window;
        }
        PostMessageW(input_, kAttachedMessage, (WPARAM)window, 0);
    }

    void PostMouse(UINT message, WPARAM wParam, LPARAM lParam) {
        PostMessageW(input_, kMouseMessage + (message - WM_MOUSEFIRST), wParam, lParam);
    }

    // Draws the picture last given (on this thread, which owns the window).
    void Show(HWND window, bool notify) {
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        AcquireSRWLockExclusive(&lock_);
        if (picture_) {
            HGDIOBJ previous = SelectObject(memory, picture_);
            POINT source{0, 0};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            UpdateLayeredWindow(window, screen, &position_, &size_, memory, &source, 0, &blend, ULW_ALPHA);
            SelectObject(memory, previous);
        }
        ReleaseSRWLockExclusive(&lock_);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        if (notify) {
            WidgetLink::NotifyOthers();
        }
    }

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        if (message == WM_NCCREATE) {
            SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCTW*)lParam)->lpCreateParams);
        }
        auto self = (WidgetWindow*)GetWindowLongPtrW(window, GWLP_USERDATA);
        if (!self) {
            return DefWindowProcW(window, message, wParam, lParam);
        }
        if (message == WidgetLink::ChangedMessage()) {
            PostMessageW(self->input_, message, wParam, lParam);
            return 0;
        }
        switch (message) {
            case kPresentMessage:
                self->Show(window, wParam != 0);
                return 0;
            case kVisibleMessage:
                ShowWindow(window, wParam ? SW_SHOWNA : SW_HIDE);
                if (lParam) {
                    WidgetLink::NotifyOthers();
                }
                return 0;
            case WM_MOUSEACTIVATE:
                return MA_NOACTIVATE;
            case WM_MOUSEMOVE:
                if (!self->trackingMouse_) {
                    TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, window, 0};
                    self->trackingMouse_ = TrackMouseEvent(&track);
                }
                // Only the latest move matters, as Windows has it for a busy thread: one is posted at
                // a time, and the UI thread takes the latest point when it gets to it (TakeMouseMove).
                self->movePoint_ = lParam;
                if (!self->movePending_.exchange(true)) {
                    self->PostMouse(message, wParam, lParam);
                }
                return 0;
            case WM_MOUSELEAVE:
                self->trackingMouse_ = false;
                PostMessageW(self->input_, message, wParam, lParam);
                return 0;
            case WM_LBUTTONDOWN:
                SetCapture(window);  // For dragging.
                self->PostMouse(message, wParam, lParam);
                return 0;
            case WM_LBUTTONUP:
                self->PostMouse(message, wParam, lParam);
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
                return 0;
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MOUSEWHEEL:
                self->PostMouse(message, wParam, lParam);
                return 0;
            case WM_CAPTURECHANGED:
            case WM_DPICHANGED_AFTERPARENT:
                PostMessageW(self->input_, message, wParam, lParam);
                break;
            case WM_DESTROY:
                // Destroyed here, or along with the taskbar (which Explorer can recreate).
                WidgetLink::Unpublish(window);
                self->window_ = nullptr;
                self->trackingMouse_ = false;
                PostMessageW(self->input_, kDetachedMessage, (WPARAM)window, 0);
                break;
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    HWND input_ = nullptr;
    const wchar_t* windowClass_ = nullptr;
    HANDLE thread_ = nullptr;
    DWORD threadId_ = 0;
    HANDLE ready_ = nullptr;
    std::atomic<HWND> window_{nullptr};
    bool trackingMouse_ = false;  // On its thread.
    std::atomic<LPARAM> movePoint_{0};
    std::atomic<bool> movePending_{false};
    SRWLOCK lock_ = SRWLOCK_INIT;  // For the picture.
    HBITMAP picture_ = nullptr;
    POINT position_{};
    SIZE size_{};
};

// A taskbar button in the style of the Windows weather widget. Windows 11 has no API for adding
// things to the taskbar, so it's a transparent window inside the taskbar's own window, over the
// buttons Explorer draws there: it moves and auto-hides with the taskbar, and full-screen apps and
// the Start menu cover it as they do the rest of the taskbar. Drag it to move it along the taskbar.
//
// Windhawk loads the mod into Explorer, which has the taskbar; the app puts the widget into
// Explorer's taskbar from its own process. Either way the window is a WidgetWindow, on a thread of
// its own, and everything else happens on the UI thread, with a hidden top-level window: it gets
// the widget's mouse messages from the WidgetWindow, the notices Windows sends only to top-level
// windows (settings, display and taskbar changes), and the timers, and owns the menus and
// dialogs. When Explorer recreates the taskbar, the widget's window goes with the old one, and a
// new one goes into the new taskbar.
//
// Subclasses lay out and draw the content and handle clicks.
class TaskbarWidget {
public:
    virtual ~TaskbarWidget() {
        window_.Stop();
        if (events_.hwnd_) {
            DestroyWindow(events_.hwnd_);
        }
    }

    // Called with the widget's new offset after it's been dragged (while not linked).
    std::function<void(int offset)> onOffsetChanged;
    // Called when displays are added, removed or rearranged.
    std::function<void()> onDisplaysChanged;

    bool Create(int offset) {
        offset_ = offset;
        link_ = WidgetLink::Load();
        events_.widget = this;
        static const std::wstring eventsClass = std::wstring(kWidgetClassName) + L".Events";
        if (const wchar_t* registered = RegisterForwardingClass<Events>(eventsClass.c_str())) {
            CreateWindowExW(WS_EX_TOOLWINDOW, registered, kWidgetTitle, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, ModuleInstance(), &events_);
        }
        if (!events_.hwnd_ || !window_.Start(events_.hwnd_)) {
            return false;
        }
        // Only needed when the app runs as administrator: lets Explorer's notice through.
        taskbarCreatedMessage_ = RegisterWindowMessageW(L"TaskbarCreated");
        ChangeWindowMessageFilterEx(events_.hwnd_, taskbarCreatedMessage_, MSGFLT_ALLOW, nullptr);
        SetTimer(events_.hwnd_, kPlacementTimer, 250, nullptr);
        AttachToTaskbar();
        return true;
    }

    void Destroy() {
        SetShown(false);
        window_.Stop();  // Its window goes, taking this widget's copy of the group with it.
        if (linkSavePending_) {
            linkSavePending_ = false;
            WidgetLink::Save(link_);
        }
        if (hwnd_) {
            hwnd_ = nullptr;
            WidgetLink::NotifyOthers();  // So they close the gap.
        }
        if (events_.hwnd_) {
            DestroyWindow(events_.hwnd_);  // Its timers go with it.
        }
    }

    // A hidden window of the UI thread, for timers (which come to OnTimer), and to own menus and
    // dialogs.
    HWND OwnerWindow() const { return events_.hwnd_; }

    bool Visible() const { return hwnd_ && visible_; }

    RECT Bounds() const {
        RECT rect{};
        if (hwnd_) {
            GetWindowRect(hwnd_, &rect);
        }
        return rect;
    }

    void SetShown(bool shown) {
        if (shown == shown_) {
            return;
        }
        shown_ = shown;
        UpdatePlacement(true);
    }

    // Re-renders after the content changed (and resizes if its width did).
    void ContentChanged() { UpdatePlacement(true); }

    const WidgetLink::Group& Link() const { return link_; }

    // Links another mod's widget with this one (joining its group if it's in one), or takes it out
    // of the group.
    void SetMemberLinked(int member, bool linked) {
        WidgetLink::Group group = link_;
        if (linked) {
            if (group.order.size() < 2) {
                // A new group starts where the leftmost of the two widgets is now, in the order
                // they're in.
                int left = taskbar_.left + (Visible() ? placedOffset_ : std::max(0, offset_));
                RECT other;
                bool otherFirst = false;
                if (WidgetLink::TryGetBounds(member, other)) {
                    otherFirst = other.left < left;
                    left = std::min(left, (int)other.left);
                }
                group.order = otherFirst ? std::vector<int>{member, kSelfMember} : std::vector<int>{kSelfMember, member};
                group.offset = std::max(0, left - (int)taskbar_.left);
            } else {
                // Joining the group, at its right end.
                if (!group.Has(kSelfMember)) {
                    group.order.push_back(kSelfMember);
                }
                if (!group.Has(member)) {
                    group.order.push_back(member);
                }
            }
        } else {
            group.order.erase(std::remove(group.order.begin(), group.order.end(), member), group.order.end());
            if (group.order.size() < 2) {
                group.order.clear();
            }
        }
        ChangeLink(group);
        UpdatePlacement(true);
        WidgetLink::NotifyOthers();
    }

    // Whether a widget in the group is showing left (-1) or right (1) of this one, to move past.
    bool CanMoveInGroup(int direction) const { return NeighborShowing(direction) >= 0; }

    // Moves this widget past the nearest widget in its group that's showing on its left (-1) or
    // right (1). (Past one that's hidden, nothing would change to see.)
    void MoveInGroup(int direction) {
        int neighbor = NeighborShowing(direction);
        if (neighbor < 0) {
            return;
        }
        WidgetLink::Group group = link_;
        int passed = group.order[neighbor];
        group.order.erase(std::remove(group.order.begin(), group.order.end(), kSelfMember), group.order.end());
        auto place = std::find(group.order.begin(), group.order.end(), passed);
        group.order.insert(direction < 0 ? place : place + 1, kSelfMember);
        ChangeLink(group);
        UpdatePlacement(true);
        WidgetLink::NotifyOthers();
    }

    // The place in the group of the nearest widget showing on this one's left (-1) or right (1),
    // or -1.
    int NeighborShowing(int direction) const {
        auto self = std::find(link_.order.begin(), link_.order.end(), kSelfMember);
        if (self == link_.order.end()) {
            return -1;
        }
        RECT bounds;
        for (int i = (int)(self - link_.order.begin()) + direction; i >= 0 && i < (int)link_.order.size(); i += direction) {
            if (WidgetLink::TryGetBounds(link_.order[i], bounds)) {
                return i;
            }
        }
        return -1;
    }

    float Scale() const { return scale_; }

    int Px(float value) const { return (int)std::nearbyint(value * scale_); }  // Halves to even, like the apps' Math.Round.

    // Screen bounds of a part of the widget, from client x and width.
    RECT ScreenBoundsOf(int x, int width) const {
        RECT bounds = Bounds();
        return {bounds.left + x, bounds.top, bounds.left + x + width, bounds.bottom};
    }

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

    // Places the widget on the taskbar, or hides it when it shouldn't show.
    void UpdatePlacement(bool contentChanged) {
        HWND taskbar = hwnd_ ? GetParent(hwnd_) : nullptr;
        RECT bar;
        // A vertical taskbar (Windows 10 can have one) has no room for it.
        if (!shown_ || !taskbar || !GetWindowRect(taskbar, &bar) || bar.right - bar.left <= bar.bottom - bar.top) {
            if (hwnd_ && visible_) {
                window_.SetVisible(false, link_.Linked());  // Telling the others, so they close the gap.
                visible_ = false;
            }
            OnHidden();
            return;
        }

        // Scale with the taskbar's display.
        UINT dpi = GetDpiForWindow(taskbar);
        float scale = (dpi ? dpi : 96) / 96.0f;
        if (scale != scale_) {
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

        bool linked = link_.Linked();
        if (wasLinked_ && !linked && visible_) {
            // Just unlinked: stay where the group left this widget.
            offset_ = std::max(0, placedOffset_);
            if (onOffsetChanged) {
                onOffsetChanged(offset_);
            }
        }
        wasLinked_ = linked;

        int x;
        bool divider = false;
        if (linked) {
            // Side by side with the other widgets in the group that are showing, in its order,
            // wherever the group was put. Every widget works it out the same way from the same
            // numbers, so they meet without gaps.
            int before = 0;
            int total = 0;
            bool reachedSelf = false;
            for (int member : link_.order) {
                int memberWidth = 0;
                RECT other;
                if (member == kSelfMember) {
                    memberWidth = width;
                    reachedSelf = true;
                } else if (WidgetLink::TryGetBounds(member, other)) {
                    memberWidth = other.right - other.left;
                }
                if (!reachedSelf) {
                    before += memberWidth;
                }
                total += memberWidth;
            }
            total = std::min(total, barWidth);
            int groupOffset = link_.offset >= 0 ? link_.offset : DefaultOffset(barWidth, total);
            groupLeft_ = bar.left + std::max(0, std::min(groupOffset, barWidth - total));
            x = std::min(groupLeft_ + before, (int)bar.left + barWidth - width);
            divider = before > 0;
        } else {
            int preferredOffset = offset_ >= 0 ? offset_ : DefaultOffset(barWidth, width);
            x = bar.left + std::max(0, std::min(preferredOffset, barWidth - width));
            groupLeft_ = x;
        }
        placedOffset_ = x - bar.left;
        // In the taskbar's client area (mirrored, for right-to-left languages).
        RECT target{x, bar.top, x + width, bar.bottom};
        MapWindowPoints(nullptr, taskbar, (POINT*)&target, 2);

        // The other widgets in the group are told once the widget's window has moved, so they move
        // up against it where it is.
        bool moved = !EqualRect(&target, &placed_) || !visible_;
        bool notify = moved && linked;
        if (contentChanged || moved || divider != dividerAtLeft_) {
            dividerAtLeft_ = divider;
            Render(target, notify && visible_);
        }
        if (!visible_) {
            window_.SetVisible(true, notify);
            visible_ = true;
        }
        StayOverTaskbarContent();
        OnPlaced();
    }

    void Render() { Render(placed_); }

private:
    static constexpr UINT_PTR kPlacementTimer = 1;
    static constexpr UINT_PTR kLinkSaveTimer = 3;

    // The hidden top-level window of the UI thread.
    struct Events {
        TaskbarWidget* widget = nullptr;
        HWND hwnd_ = nullptr;

        bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
            return widget->HandleEvent(message, wParam, lParam, result);
        }
    };

    bool HandleEvent(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        if (taskbarCreatedMessage_ && message == taskbarCreatedMessage_) {
            // Explorer started again, or recreated the taskbar: the widget goes into the new one.
            AttachToTaskbar();
            return true;
        }
        if (message == WidgetLink::ChangedMessage()) {
            // Another widget moved, changed size, or changed the group.
            SyncLink();
            UpdatePlacement(false);
            return true;
        }
        if (message >= WidgetWindow::kMouseMessage && message <= WidgetWindow::kMouseMessage + (WM_MOUSELAST - WM_MOUSEFIRST)) {
            message = WM_MOUSEFIRST + (message - WidgetWindow::kMouseMessage);  // The widget's, handled below.
        }
        switch (message) {
            case WidgetWindow::kAttachedMessage:
                attaching_ = false;
                if (wParam && (HWND)wParam != hwnd_) {
                    hwnd_ = (HWND)wParam;
                    visible_ = false;
                    placed_ = {};
                    // Take the other widgets' copy of the group if it changed more recently, and
                    // let them know this one is here.
                    WidgetLink::Publish(hwnd_, link_);
                    SyncLink();
                    WidgetLink::NotifyOthers();
                    if (shown_) {
                        UpdatePlacement(true);
                    }
                }
                return true;
            case WidgetWindow::kDetachedMessage:
                if ((HWND)wParam == hwnd_) {
                    hwnd_ = nullptr;
                    visible_ = false;
                    OnHidden();
                    WidgetLink::NotifyOthers();
                }
                return true;
            case WM_TIMER:
                if (wParam == kPlacementTimer) {
                    // In an Explorer process (for folder windows) that started while there was no
                    // taskbar, and another Explorer has it now, the mod has nothing to do: it ends.
                    if (TaskbarWindow() && !WidgetTaskbar()) {
                        PostQuitMessage(0);
                        return true;
                    }
                    // Also notices the taskbar moving or changing size.
                    AttachToTaskbar();
                    if (shown_) {
                        UpdatePlacement(false);
                    }
                } else if (wParam == kLinkSaveTimer) {
                    KillTimer(events_.hwnd_, kLinkSaveTimer);
                    linkSavePending_ = false;
                    WidgetLink::Save(link_);
                } else {
                    OnTimer(wParam);
                }
                return true;
            case WM_DISPLAYCHANGE:
                if (onDisplaysChanged) {
                    onDisplaysChanged();
                }
                UpdatePlacement(true);
                return false;
            case WM_SETTINGCHANGE:
                RefreshShellSettings();
                UpdatePlacement(true);
                return false;
            case WM_MOUSEMOVE:
            case WM_MOUSELEAVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_CAPTURECHANGED:
            case WM_DPICHANGED_AFTERPARENT:
                // The widget's, from its window. Not while a dialog of this thread is open (which
                // disables this window), as a disabled window gets no mouse input.
                if (message == WM_MOUSEMOVE) {
                    lParam = window_.TakeMouseMove();
                }
                if (hwnd_ && IsWindowEnabled(events_.hwnd_)) {
                    HandleInput(message, wParam, lParam);
                }
                return true;
        }
        return false;
    }

    void HandleInput(UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_MOUSEMOVE: {
                POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (!pressed_) {
                    OnHover(point);
                    return;
                }
                POINT cursor;
                GetCursorPos(&cursor);
                int distance = cursor.x - dragStartCursorX_;
                if (!dragging_ && std::abs(distance) < GetSystemMetrics(SM_CXDRAG)) {
                    return;
                }
                if (!dragging_) {
                    dragging_ = true;
                    OnDragStart();
                }
                int dragged = std::max(0, dragStartOffset_ + distance);
                if (wasLinked_) {
                    WidgetLink::Group moved = link_;
                    moved.offset = dragged;
                    ChangeLink(moved);  // Moves the group; the other widgets follow when told.
                } else {
                    offset_ = dragged;
                }
                UpdatePlacement(true);
                return;
            }
            case WM_MOUSELEAVE:
                OnHoverLeave();
                return;
            case WM_LBUTTONDOWN: {
                POINT cursor;
                GetCursorPos(&cursor);
                pressed_ = true;
                dragging_ = false;
                dragStartCursorX_ = cursor.x;
                dragStartOffset_ = groupLeft_ - taskbar_.left;
                return;
            }
            case WM_LBUTTONUP: {
                bool wasPressed = pressed_;
                bool wasDragging = dragging_;
                pressed_ = false;
                dragging_ = false;
                if (wasDragging) {
                    EndDrag();
                } else if (wasPressed) {
                    POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    // Pressing and sliding off the widget before letting go doesn't click it (nor
                    // does letting go after its window has gone with the taskbar).
                    RECT client{};
                    if (GetClientRect(hwnd_, &client) && PtInRect(&client, point)) {
                        OnClick(point, MouseButton::Left);
                    }
                }
                return;
            }
            case WM_CAPTURECHANGED:
                // Something else took the mouse mid-press: the press ends there, and a drag stays
                // where it got to.
                if ((HWND)lParam != hwnd_ && pressed_) {
                    bool wasDragging = dragging_;
                    pressed_ = false;
                    dragging_ = false;
                    if (wasDragging) {
                        EndDrag();
                    }
                }
                return;
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
                OnClick({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, message == WM_MBUTTONUP ? MouseButton::Middle : MouseButton::Right);
                return;
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
                return;
            }
            case WM_DPICHANGED_AFTERPARENT:
                UpdatePlacement(true);
                return;
        }
    }

    // Puts the widget in the taskbar, once there is one (again).
    void AttachToTaskbar() {
        if (hwnd_ || attaching_) {
            return;
        }
        if (HWND taskbar = WidgetTaskbar()) {
            attaching_ = true;
            window_.Attach(taskbar);
        }
    }

    // A change to the group made here: it's stamped as the newest copy, shown to the other widgets,
    // and saved once it settles (a drag changes it on every mouse move).
    void ChangeLink(WidgetLink::Group group) {
        int newerThan = link_.stamp;
        if (std::optional<WidgetLink::Group> newest = WidgetLink::ReadNewest()) {
            newerThan = std::max(newerThan, newest->stamp);
        }
        group.stamp = WidgetLink::NewStamp(newerThan);
        link_ = group;
        if (hwnd_) {
            WidgetLink::Publish(hwnd_, link_);
        }
        SaveLinkSoon();
    }

    // Takes the other widgets' copy of the group when it changed more recently than this one's.
    void SyncLink() {
        std::optional<WidgetLink::Group> newest = WidgetLink::ReadNewest();
        if (newest && newest->stamp > link_.stamp) {
            link_ = *newest;
            if (hwnd_) {
                WidgetLink::Publish(hwnd_, link_);
            }
            SaveLinkSoon();
        }
    }

    void SaveLinkSoon() {
        linkSavePending_ = true;
        SetTimer(events_.hwnd_, kLinkSaveTimer, 1000, nullptr);
    }

    // Keeps where a drag left the widget (or the group).
    void EndDrag() {
        if (wasLinked_) {
            WidgetLink::Group moved = link_;
            moved.offset = groupLeft_ - taskbar_.left;  // Where the group ended up after clamping.
            ChangeLink(moved);
            Render();
            WidgetLink::NotifyOthers();
        } else {
            offset_ = placedOffset_;  // Where it ended up after clamping.
            Render();
            if (onOffsetChanged) {
                onOffsetChanged(offset_);
            }
        }
    }

    // Draws the widget at the bounds (in the taskbar's client area), for its window to show; with
    // notify, the linked widgets are told once it does.
    void Render(const RECT& bounds, bool notify = false) {
        int width = bounds.right - bounds.left;
        int height = bounds.bottom - bounds.top;
        if (!hwnd_ || width <= 0 || height <= 0) {
            return;
        }
        placed_ = bounds;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bitmap || !bits) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
            return;
        }
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
                // Linked, with another widget on the left: the divider between them.
                gp::Pen pen(WithAlpha(lightTaskbar_ ? 50 : 60, foreground), 1);
                graphics.DrawLine(&pen, 1.0f, (float)Px(14), 1.0f, (float)(height - Px(14)));
            }
            Draw(graphics, width, height, lightTaskbar_);
        }
        GdiFlush();
        window_.Present(bitmap, {bounds.left, bounds.top}, {width, height}, notify);
    }

    // Keeps the widget over the taskbar's own content: the windows Explorer draws the taskbar's
    // buttons in (Windows 11's XAML; Windows 10's task band, tray and buttons). Other windows in the
    // taskbar, such as other mods', stay where they are.
    void StayOverTaskbarContent() {
        static const wchar_t* const kContent[] = {L"Windows.UI.Composition.DesktopWindowContentBridge", L"ReBarWindow32", L"TrayNotifyWnd",
                                                  L"Start", L"TrayButton", L"TrayDummySearchControl", L"TrayShowDesktopButtonWClass"};
        for (HWND above = GetWindow(hwnd_, GW_HWNDPREV); above; above = GetWindow(above, GW_HWNDPREV)) {
            WCHAR className[64] = {};
            GetClassNameW(above, className, ARRAYSIZE(className));
            for (const wchar_t* content : kContent) {
                if (wcscmp(className, content) == 0) {
                    // Asynchronous, as the window is another thread's.
                    SetWindowPos(hwnd_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
                    return;
                }
            }
        }
    }

    HWND hwnd_ = nullptr;  // The widget's window, while it's in a taskbar.
    WidgetWindow window_;
    Events events_;
    bool attaching_ = false;
    bool visible_ = false;
    RECT placed_{};  // Where the widget was last drawn, in the taskbar's client area.
    int placedOffset_ = 0;  // And its distance from the taskbar's left edge.
    bool shown_ = false;
    int offset_ = -1;  // Pixels from the taskbar's left edge; negative means the default position.
    RECT taskbar_{};
    float scale_ = 0;
    bool lightTaskbar_ = false;
    UINT taskbarCreatedMessage_ = 0;
    bool pressed_ = false;
    bool dragging_ = false;
    int dragStartCursorX_ = 0;
    int dragStartOffset_ = 0;
    int wheelRemainder_ = 0;
    int groupLeft_ = 0;  // Left edge of the linked group on screen (or of this widget alone).
    WidgetLink::Group link_;
    bool linkSavePending_ = false;
    bool wasLinked_ = false;
    bool dividerAtLeft_ = false;  // Linked, with another widget on the left.
};

// The Link widgets submenu: links this widget with the other mods' widgets that are running (or
// linked already), and moves it left or right among them.
void AddLinkMenu(Menu& menu, TaskbarWidget& widget) {
    const WidgetLink::Group& group = widget.Link();
    std::vector<int> others;
    for (int member = 0; member < WidgetLink::kMemberCount; member++) {
        if (member != kSelfMember && (WidgetLink::Running(member) || (group.Linked() && group.Has(member)))) {
            others.push_back(member);
        }
    }
    if (others.empty()) {
        return;
    }
    menu.AddSeparator();
    Menu& submenu = menu.AddSubmenu(L"Link widgets");
    for (int member : others) {
        bool linked = group.Linked() && group.Has(member);
        submenu.Add(std::wstring(L"Link with ") + WidgetLink::kMembers[member].name,
                    [&widget, member, linked]() { widget.SetMemberLinked(member, !linked); }, linked);
    }
    if (group.Linked()) {
        submenu.AddSeparator();
        submenu.Add(L"Move this widget left", [&widget]() { widget.MoveInGroup(-1); }, false, widget.CanMoveInGroup(-1));
        submenu.Add(L"Move this widget right", [&widget]() { widget.MoveInGroup(1); }, false, widget.CanMoveInGroup(1));
    }
}

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

    void OnTimer(UINT_PTR timer) override {
        if (timer == kHideCheckTimer) {
            HideWhenCursorLeaves();
        }
    }

    void OnHidden() override { KillTimer(hwnd_, kHideCheckTimer); }

    void OnMouseEnter() override { cursorEntered_ = true; }

    void OnMouseLeave() override { Hide(); }

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
    return NativeSystemFolder() + L"\\mstsc.exe";
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
    int taskbarOffset = -1;
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
        taskbarOffset = Storage::GetInt(L"TaskbarOffset", -1);
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
        saved = Storage::SetInt(L"TaskbarOffset", taskbarOffset) && saved;
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
            } else if (key == L"TaskbarOffset") {
                ParseInt(value, taskbarOffset);
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

// The monitor glyph with a number on its screen, for the display buttons.
void DrawNumberedMonitor(gp::Graphics& graphics, const gp::RectF& box, float emSize, const gp::Color& color, const std::wstring& number,
                         float fontPixels) {
    gp::RectF monitor = DrawGlyphAtSize(graphics, kGlyphMonitor, emSize, box, color);
    // Inside its outline, above the stand (in both Segoe Fluent Icons and Segoe MDL2 Assets).
    gp::RectF screen(monitor.X + monitor.Width * 0.07f, monitor.Y + monitor.Height * 0.08f, monitor.Width * 0.86f, monitor.Height * 0.66f);
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
            DrawGlyphAtSize(graphics, kGlyphCheckMark, check, rect, mark);
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
            gp::RectF icon((float)section.x, height / 2.0f - Px(12), (float)section.width, (float)Px(24));
            DrawNumberedMonitor(graphics, icon, 20.0f * Scale(), iconColor, std::to_wstring(content_.buttons[button].size()), 11.0f * Scale());
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
        KillTimer(OwnerWindow(), kHoverTimer);
        if (section > kConnection) {
            hoverTarget_ = section;
            SetTimer(OwnerWindow(), kHoverTimer, 400, nullptr);
        }
    }

    void OnHoverLeave() override {
        if (hovered_ != kNone) {
            hovered_ = kNone;
            Render();
        }
        KillTimer(OwnerWindow(), kHoverTimer);
    }

    void OnTimer(UINT_PTR timer) override {
        if (timer != kHoverTimer) {
            return;
        }
        KillTimer(OwnerWindow(), kHoverTimer);
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
        KillTimer(OwnerWindow(), kHoverTimer);
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
        KillTimer(OwnerWindow(), kHoverTimer);
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

    // The This PC icon, then the name over the address, like the weather widget's two lines;
    // just the name, centered, without an address.
    void DrawConnection(gp::Graphics& graphics, const Section& section, int height, const gp::Color& foreground,
                        const gp::Color& secondary) {
        float middle = height / 2.0f;
        float iconSize = (float)Px(24);
        // At three quarters of the box's height, about the size of the taskbar's own icons.
        DrawGlyphAtSize(graphics, kGlyphThisPC, iconSize * 0.75f, gp::RectF(section.x + (float)Px(10), middle - iconSize / 2, iconSize, iconSize),
                        foreground);
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
        widget_.Destroy();
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
        if (!widget_.Create(settings_.taskbarOffset) || !popup_.Create()) {
            return false;
        }
        widget_.onOffsetChanged = [this](int offset) {
            settings_.taskbarOffset = offset;
            Save();
        };
        widget_.onDisplaysChanged = [this]() {
            RefreshDisplays();
            RefreshWidget();
            // Monitor names can show up a moment later; look again once things have settled.
            SetTimer(messages_.Handle(), kDisplaysSettledTimer, 1500, nullptr);
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
        return true;
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

        AddLinkMenu(menu, widget_);

        RECT bounds = widget_.Bounds();
        POINT cursor;
        GetCursorPos(&cursor);
        widget_.HideFlyout();
        // Above the widget, so the menu doesn't cover it.
        menu.Show(widget_.OwnerWindow(), {cursor.x, bounds.top});
    }

    Settings settings_;
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
    std::vector<std::unique_ptr<Session>> sessions_;
    bool starting_ = false;
    int busyButton_ = -1;
};

UiThread g_uiThread;

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    // Windhawk loads the mod into every Explorer process, folder windows' too: it runs in the one
    // with the taskbar (or, while Explorer is starting, the one that's about to have it).
    if (InFolderWindowProcess() || (TaskbarWindow() && !WidgetTaskbar())) {
        return FALSE;
    }
    return g_uiThread.Start([]() -> std::shared_ptr<void> {
        auto app = std::make_shared<RdpApp>();
        if (!app->Start()) {
            Wh_Log(L"Couldn't start");
            return nullptr;
        }
        return app;
    });
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_uiThread.Stop();
}
