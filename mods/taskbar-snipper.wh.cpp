// ==WindhawkMod==
// @id              taskbar-snipper
// @name            Taskbar Snipper
// @description     Print Screen, or a button on the taskbar, selects part of the screen to mark up and copy or save; Win+Shift+R records it as a video to trim and copy or save
// @version         1.1.0
// @author          Lucidic15
// @github          https://github.com/Lucidic15
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lshell32 -lshcore -ldwmapi -ladvapi32 -lole32 -luuid -lcomdlg32 -lmsimg32 -lmfplat -lmfreadwrite -lmfuuid -ld3d11
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Snipper

![The screenshot and record buttons on the taskbar](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-snipper/screenshot.png)

Press **Print Screen**, or click the screenshot button on the taskbar, to select part of the
screen. A toolbox opens right above the selection to mark it up with a pencil, lines, arrows, boxes
and text, in any color and thickness. When you're done, click anywhere outside the selection (or
press **Enter**), and the screenshot is copied to the clipboard, saved to a folder, or both, as you
choose in the mod's settings.

Press **Win+Shift+R**, or click the record button, to record part of the screen as a video. When
you stop, trim its start and end, then copy or save it.

## Selecting

- **Drag** to select an area. The screen stays as it was when you pressed Print Screen.
- **Click** a window to select all of it, or the desktop to select the whole display.
- **Esc** or **right-click** cancels.

## The toolbox

- **Select** (V), **Pencil** (P), **Line** (L), **Arrow** (A), **Box** (B) and **Text** (T). Drag
  inside the selection to draw; hold **Shift** to keep a line or arrow to 45° steps, or a box
  square. A click with the pencil makes a dot. With Text, click where the text goes and type;
  **Enter** starts a new line, and **Esc** or a click elsewhere finishes it.
- **Colors**: eight at hand, and the last swatch picks any color.
- **Thickness**: **−** and **+**, or the mouse wheel. Text gets bigger with it too.
- **Undo** (Ctrl+Z), **Copy** (Ctrl+C), **Save** (Ctrl+S) and **Close** (Esc).

**Copy** and **Save** do just that. Clicking outside the selection or pressing Enter does what the
**When you're done** setting says: copy, save, or both. Screenshots are saved as PNG files named by
the date and time, in Windows' Screenshots folder (in Pictures) unless you choose another. A note
in the corner of the screen says what happened; click it to show a saved screenshot in File
Explorer.

## Moving and changing marks

- Whatever tool is in hand, **drag** anything already drawn to move it; the pointer turns into a
  four-way arrow over it. The **Select** tool only selects and moves, for when you'd rather not
  draw by accident.
- **Click** a mark to select it. Its color and thickness show in the toolbox, and changing them
  changes the mark. The **arrow keys** nudge it (with **Shift**, 10 pixels at a time), **Delete**
  removes it, and **Esc** lets go of it.
- To change text, click it with the Text tool, or click it again once it's selected.
- Undo takes back moves and changes too.

## Recording the screen

- Press **Win+Shift+R**, click the record button, or choose **Record the screen** from the buttons'
  menu. Select an area as for a screenshot, then click **Start recording** (or press **Enter**).
  Recording starts after a 3-second countdown.
- While recording, a red line marks the area and a bar beside it shows the time; neither is in the
  video. To stop, click the **stop** square on the bar or the record button, or press
  **Win+Shift+R** again. The bar's **X** throws the recording away.
- The recording then opens to trim it. Drag the handles at either end of the strip of frames to cut
  off its start and end, and click the strip to see any moment. **Space** plays the part kept, the
  **arrow keys** step a frame at a time (a second with **Shift**), and **I** and **O** set the start
  and end at the frame shown.
- **Copy** (Ctrl+C) puts the video on the clipboard, to paste into a chat, an email or a folder.
  **Save** (Ctrl+S) saves it in Windows' Screen Recordings folder (in Videos) unless you choose
  another.
- Recordings are MP4 videos (H.264, without sound) of up to 30 frames a second, with the mouse
  pointer.

## The buttons

The screenshot and record buttons sit on the taskbar like the Windows weather widget: click one to
take a screenshot or record, right-click them for their menu, and drag them to move them along the
taskbar. **Which buttons** in the mod's settings shows both, or just one of them, and **Buttons on
the taskbar** turns them off; the shortcuts work either way.

The menu opens the screenshots and recordings folders, and **Choose the screenshots folder…** and
**Choose the recordings folder…** pick others. The folders can be set in the mod's settings too;
whichever you changed last is used.

![Taskbar Snipper, Taskbar Audio Flipper and Taskbar RDP Launcher linked on the taskbar](https://raw.githubusercontent.com/Lucidic15/windhawk-mods/images/taskbar-widgets/linked.png)

From the left: [Taskbar Snipper](https://windhawk.net/mods/taskbar-snipper),
[Taskbar Audio Flipper](https://windhawk.net/mods/taskbar-audio-flipper) and
[Taskbar RDP Launcher](https://windhawk.net/mods/taskbar-rdp-launcher).

With the Taskbar Audio Flipper or Taskbar RDP Launcher mod running too, their widgets can be joined
with the buttons into one: right-click any of them, open **Link widgets**, and choose **Link with**
the other. Linked, they sit side by side with a divider between them, and dragging any of them
moves them all; **Move this widget left** and **right** change their order.

Update the linked widgets' mods together (Taskbar Audio Flipper and Taskbar RDP Launcher 1.2 or
later, Taskbar Snipper 1.1 or later): their widgets are inside the taskbar now, where older versions
don't look for them, so an older one overlaps the others and doesn't follow them when they're
dragged, reordered or unlinked.

## Good to know

- Only Print Screen on its own is taken over. Alt+Print Screen, Win+Print Screen and Win+Shift+S
  work as usual, and turning the mod off gives Print Screen back to Windows. Win+Shift+R is taken
  over while **Win+Shift+R records the screen** is on in the settings.
- While a program running as administrator is in front, Windows doesn't let the mod see Print
  Screen or Win+Shift+R, so Windows handles them as usual.
- The tool, color and thickness you used last are remembered.
- A recording closed without copying or saving it is deleted; the window asks first. If the mod
  turns off (or Windows signs out) while recording, or with a recording still open, the recording
  goes to the recordings folder: right away when that's on the same drive as Windhawk's mod storage,
  otherwise the next time the mod starts.
- Recordings being made and trimmed, and the files put on the clipboard by Copy, are kept in the
  mod's storage (in a folder only you can open), and go with the mod.
- On 32-bit Windows, short on memory for video, an area bigger than 2560 × 1440 is recorded
  scaled down to that many pixels. Otherwise areas up to 4K are recorded as they are.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showButton: true
  $name: Buttons on the taskbar
  $description: Screenshot and record buttons on the taskbar. Right-click them to choose the folders to save in, or to link them with the Taskbar Audio Flipper and Taskbar RDP Launcher widgets.
- buttons: both
  $name: Which buttons
  $description: The screenshot button, the record button, or both. Their shortcuts work either way.
  $options:
  - both: Screenshot and record
  - screenshot: Screenshot only
  - record: Record only
- recordShortcut: true
  $name: Win+Shift+R records the screen
  $description: Takes over Windows' screen recording shortcut (the Snipping Tool's), to record with Taskbar Snipper instead.
- afterCapture: both
  $name: When you're done
  $description: What clicking outside the selection, or pressing Enter, does with the screenshot. The toolbox's Copy and Save buttons do just one of them.
  $options:
  - copy: Copy it to the clipboard
  - save: Save it to the folder
  - both: Copy it and save it
- saveFolder: ""
  $name: Folder to save screenshots in
  $description: Leave it empty for Windows' Screenshots folder, in Pictures. Environment variables like %USERPROFILE% work, and the folder is created if it isn't there. The buttons' right-click menu can choose a folder too; whichever you changed last is used.
- videoFolder: ""
  $name: Folder to save recordings in
  $description: Leave it empty for Windows' Screen Recordings folder, in Videos. Environment variables like %USERPROFILE% work, and the folder is created if it isn't there. The buttons' right-click menu can choose a folder too; whichever you changed last is used.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <aclapi.h>
#include <commdlg.h>
#include <codecapi.h>
#include <d3d11.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gp = Gdiplus;

namespace {

using ARGB = gp::ARGB;

const wchar_t kWidgetClassName[] = L"TaskbarSnipper.Mod.Widget";
const wchar_t kWidgetTitle[] = L"Taskbar Snipper";
// This widget's place among the widgets that can link (WidgetLink::kMembers). It was never linked
// as a pair, as the other two were before there were three.
constexpr int kSelfMember = 2;
constexpr int kLegacyPartner = -1;

// ---------------------------------------------------------------------------
// Strings, files and folders

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

std::wstring KnownFolder(REFKNOWNFOLDERID folder) {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(folder, KF_FLAG_DEFAULT, nullptr, &path))) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

// A new file name in the folder, "<prefix> <date> <time>.<extension>", numbered if it's taken.
std::wstring NewFileName(const std::wstring& folder, const wchar_t* prefix, const wchar_t* extension) {
    SYSTEMTIME now;
    GetLocalTime(&now);
    WCHAR name[96];
    swprintf_s(name, L"%s %04d-%02d-%02d %02d%02d%02d", prefix, now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
    std::wstring path = folder + L"\\" + name + extension;
    for (int copy = 2; GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES && copy < 1000; copy++) {
        path = folder + L"\\" + name + L" (" + std::to_wstring(copy) + L")" + extension;
    }
    return path;
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
// The mod's storage, for the tool, color and thickness used last, and folders chosen from the
// menu. Windhawk keeps one for the whole PC, while the mod runs separately for each signed-in Windows user, so each user's values are kept
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

void SetInt(const wchar_t* name, int value) {
    Wh_SetIntValue(ValueName(name).c_str(), value);
}

std::wstring GetString(const wchar_t* name) {
    WCHAR value[1024] = L"";
    Wh_GetStringValue(ValueName(name).c_str(), value, ARRAYSIZE(value));
    return value;
}

void SetString(const wchar_t* name, const std::wstring& value) {
    Wh_SetStringValue(ValueName(name).c_str(), value.c_str());
}

void Delete(const wchar_t* name) {
    Wh_DeleteValue(ValueName(name).c_str());
}

// The user's own folder in the mod's storage (which goes with the mod), for recordings. Windhawk's
// storage is shared by everyone on the PC, and recordings show what was on the screen, so only this
// user (and the system) can open it.
std::wstring Folder() {
    WCHAR storage[MAX_PATH];
    size_t length = Wh_GetModStoragePath(storage, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return {};
    }
    std::wstring root(storage, length);
    std::wstring folder = root + L"\\" + UserSid();
    if (GetFileAttributesW(folder.c_str()) == INVALID_FILE_ATTRIBUTES) {
        SHCreateDirectoryExW(nullptr, root.c_str(), nullptr);  // Fails harmlessly when it's there already.
        SECURITY_ATTRIBUTES attributes{sizeof(attributes), nullptr, FALSE};
        PSECURITY_DESCRIPTOR descriptor = nullptr;
        std::wstring access = L"D:P(A;OICI;FA;;;" + UserSid() + L")(A;OICI;FA;;;SY)";
        if (UserSid() != L"User" &&
            ConvertStringSecurityDescriptorToSecurityDescriptorW(access.c_str(), SDDL_REVISION_1, &descriptor, nullptr)) {
            attributes.lpSecurityDescriptor = descriptor;
        }
        CreateDirectoryW(folder.c_str(), &attributes);
        LocalFree(descriptor);
    }
    return folder;
}

}  // namespace Storage

// ---------------------------------------------------------------------------
// Settings

enum class AfterCapture { Copy, Save, Both };

// A folder to save in: the one chosen from the buttons' menu, until its setting is changed; then
// the setting's.
struct SaveFolder {
    std::wstring path;                 // The folder to use.
    std::wstring setting;              // The setting, as typed: to tell when it changes.
    const wchar_t* chosenValue = L"";  // Where the chosen folder is kept in the mod's storage.

    static SaveFolder Load(const wchar_t* settingName, const wchar_t* chosenValue, const std::wstring& fallback) {
        SaveFolder folder;
        folder.chosenValue = chosenValue;
        PCWSTR value = Wh_GetStringSetting(settingName);
        std::wstring path = Trim(value);
        Wh_FreeStringSetting(value);
        // Quotes around a pasted path, and a backslash at the end, are fine.
        if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"') {
            path = Trim(path.substr(1, path.size() - 2));
        }
        while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
            path.pop_back();
        }
        folder.setting = path;
        std::wstring chosen = Storage::GetString(chosenValue);
        if (!chosen.empty() && Storage::GetString(SettingValue(chosenValue).c_str()) != path) {
            // The setting was changed after the folder was chosen: the setting wins.
            Storage::Delete(chosenValue);
            Storage::Delete(SettingValue(chosenValue).c_str());
            chosen.clear();
        }
        folder.path = !chosen.empty() ? chosen : !path.empty() ? ExpandEnvironment(path) : fallback;
        return folder;
    }

    // Uses a folder chosen from the menu, until the setting is changed.
    void Choose(const std::wstring& chosen) {
        Storage::SetString(chosenValue, chosen);
        Storage::SetString(SettingValue(chosenValue).c_str(), setting);
        path = chosen;
    }

    // Where the setting is kept as it was when the folder was chosen.
    static std::wstring SettingValue(const wchar_t* chosenValue) { return std::wstring(chosenValue) + L"Setting"; }
};

struct ModSettings {
    bool showButton = true;
    bool screenshotButton = true;  // At least one of the two.
    bool recordButton = true;
    bool recordShortcut = true;
    AfterCapture afterCapture = AfterCapture::Both;
    SaveFolder screenshots;
    SaveFolder recordings;

    static ModSettings Load() {
        ModSettings settings;
        settings.showButton = Wh_GetIntSetting(L"showButton") != 0;
        PCWSTR buttons = Wh_GetStringSetting(L"buttons");
        settings.screenshotButton = wcscmp(buttons, L"record") != 0;
        settings.recordButton = wcscmp(buttons, L"screenshot") != 0;
        Wh_FreeStringSetting(buttons);
        settings.recordShortcut = Wh_GetIntSetting(L"recordShortcut") != 0;
        PCWSTR after = Wh_GetStringSetting(L"afterCapture");
        if (wcscmp(after, L"copy") == 0) {
            settings.afterCapture = AfterCapture::Copy;
        } else if (wcscmp(after, L"save") == 0) {
            settings.afterCapture = AfterCapture::Save;
        }
        Wh_FreeStringSetting(after);
        settings.screenshots = SaveFolder::Load(L"saveFolder", L"ScreenshotFolder", KnownFolder(FOLDERID_Screenshots));
        std::wstring videos = KnownFolder(FOLDERID_Videos);
        settings.recordings = SaveFolder::Load(L"videoFolder", L"RecordingFolder", videos.empty() ? L"" : videos + L"\\Screen Recordings");
        return settings;
    }
};

// ---------------------------------------------------------------------------
// Windows settings and displays

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
    bool lightTaskbar = false;  // "Choose your default Windows mode", which the taskbar follows.
    bool lightApps = true;      // "Choose your default app mode", which the popups follow.

    static ShellSettings Read() {
        const wchar_t* personalize = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        ShellSettings settings;
        settings.lightTaskbar = ReadUserFlag(personalize, L"SystemUsesLightTheme", false);
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

bool TaskbarUsesLightTheme() {
    return CurrentShellSettings().lightTaskbar;
}

bool AppsUseLightTheme() {
    return CurrentShellSettings().lightApps;
}

// The scale (1 at 100%) of the display a screen rectangle is mostly on.
float ScaleForScreenRect(const RECT& rect) {
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 1;
    }
    return dpiX / 96.0f;
}

RECT WorkAreaForScreenRect(const RECT& rect) {
    MONITORINFO info{sizeof(info)};
    GetMonitorInfoW(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST), &info);
    return info.rcWork;
}

RECT OffsetRectBy(RECT rect, int dx, int dy) {
    OffsetRect(&rect, dx, dy);
    return rect;
}

RECT InflatedRect(RECT rect, int by) {
    InflateRect(&rect, by, by);
    return rect;
}

// ---------------------------------------------------------------------------
// Drawing

gp::Color Argb(int a, int r, int g, int b) {
    return gp::Color((BYTE)a, (BYTE)r, (BYTE)g, (BYTE)b);
}

gp::Color WithAlpha(int alpha, const gp::Color& color) {
    return gp::Color((BYTE)alpha, color.GetR(), color.GetG(), color.GetB());
}

gp::RectF ToRectF(const RECT& rect) {
    return gp::RectF((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
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
    static const wchar_t* name = []() -> const wchar_t* {
        for (const wchar_t* candidate : {L"Segoe Fluent Icons", L"Segoe MDL2 Assets"}) {
            gp::FontFamily family(candidate);
            if (family.IsAvailable()) {
                return candidate;
            }
        }
        return nullptr;
    }();
    return name;
}

// Glyphs from the Windows icon font, for the taskbar buttons and the toolbox.
const wchar_t kGlyphSnip[] = {0xF406, 0};        // ClippingTool
const wchar_t kGlyphRecord[] = {0xE7C8, 0};      // Record
const wchar_t kGlyphRecordStop[] = {0xEE1B, 0};  // RecordStop (Segoe Fluent Icons only)
const wchar_t kGlyphStop[] = {0xE71A, 0};        // Stop
const wchar_t kGlyphSelect[] = {0xE8B0, 0};      // Click
const wchar_t kGlyphArrow[] = {0xE8AD, 0};       // Go
const wchar_t kGlyphBox[] = {0xE739, 0};         // Checkbox
const wchar_t kGlyphText[] = {0xE8D2, 0};        // Font
const wchar_t kGlyphPencil[] = {0xE70F, 0};      // Edit
const wchar_t kGlyphColor[] = {0xE790, 0};   // Color
const wchar_t kGlyphMinus[] = {0xE738, 0};   // Remove
const wchar_t kGlyphPlus[] = {0xE710, 0};    // Add
const wchar_t kGlyphUndo[] = {0xE7A7, 0};    // Undo
const wchar_t kGlyphCopy[] = {0xE8C8, 0};    // Copy
const wchar_t kGlyphSave[] = {0xE74E, 0};    // Save
const wchar_t kGlyphClose[] = {0xE711, 0};   // Cancel
const wchar_t kGlyphPlay[] = {0xE768, 0};    // Play
const wchar_t kGlyphPause[] = {0xE769, 0};   // Pause

// Stopping a recording: a square in a ring, like the record glyph's dot in its ring, or just a square
// in Segoe MDL2 Assets (Windows 10), which hasn't the ring.
const wchar_t* StopRecordingGlyph() {
    const wchar_t* fontName = IconFontName();
    return fontName && wcscmp(fontName, L"Segoe Fluent Icons") == 0 ? kGlyphRecordStop : kGlyphStop;
}

// Draws an icon-font glyph at one font size, centered in the box by its ink.
void DrawGlyph(gp::Graphics& graphics, const wchar_t* glyph, float emSize, const gp::RectF& box, const gp::Color& color) {
    const wchar_t* fontName = IconFontName();
    if (!fontName) {
        return;
    }
    gp::FontFamily family(fontName);
    gp::GraphicsPath path;
    path.AddString(glyph, -1, &family, gp::FontStyleRegular, emSize, gp::PointF(0, 0), gp::StringFormat::GenericTypographic());
    gp::RectF ink;
    path.GetBounds(&ink);
    if (ink.Width <= 0 || ink.Height <= 0) {
        return;
    }
    gp::Matrix transform;
    transform.Translate(box.X + (box.Width - ink.Width) / 2 - ink.X, box.Y + (box.Height - ink.Height) / 2 - ink.Y);
    path.Transform(&transform);
    gp::SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
}

enum class TextAlign { Left, Center };

// One line of text, vertically centered in the rectangle and cut short with "…" if it doesn't fit.
void DrawTextLine(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, const gp::Color& color,
                  const gp::RectF& rect, TextAlign align = TextAlign::Left) {
    gp::StringFormat format(gp::StringFormatFlagsNoWrap);
    format.SetTrimming(gp::StringTrimmingEllipsisCharacter);
    format.SetLineAlignment(gp::StringAlignmentCenter);
    format.SetAlignment(align == TextAlign::Center ? gp::StringAlignmentCenter : gp::StringAlignmentNear);
    gp::SolidBrush brush(color);
    graphics.DrawString(text.c_str(), (INT)text.size(), &font, rect, &format, &brush);
}

float MeasureTextWidth(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font) {
    gp::StringFormat format(gp::StringFormatFlagsNoWrap | gp::StringFormatFlagsNoClip | gp::StringFormatFlagsMeasureTrailingSpaces);
    gp::RectF bounds;
    graphics.MeasureString(text.c_str(), (INT)text.size(), &font, gp::PointF(0, 0), &format, &bounds);
    return bounds.Width;
}

// A small rounded label with a line of text, like a tooltip, at the point (its top-left).
gp::RectF LabelRect(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, float x, float y, float scale) {
    float width = MeasureTextWidth(graphics, text, font) + 20 * scale;
    return gp::RectF(x, y, width, 28 * scale);
}

void DrawLabel(gp::Graphics& graphics, const std::wstring& text, const gp::Font& font, const gp::RectF& rect, float scale) {
    FillRoundedRect(graphics, Argb(225, 32, 32, 32), rect, 6 * scale);
    DrawRoundedRect(graphics, Argb(255, 70, 70, 70), rect, 6 * scale, 1);
    DrawTextLine(graphics, text, font, Argb(255, 255, 255, 255), rect, TextAlign::Center);
}

// Colors for the toolbox and notes, for the "default app mode" (light or dark) chosen in Windows.
struct Palette {
    gp::Color background, foreground, secondary, border, hover, pressed, accent;

    static Palette Current() {
        if (AppsUseLightTheme()) {
            return {Argb(250, 249, 249, 249), Argb(255, 27, 27, 27), Argb(255, 96, 96, 96), Argb(255, 213, 213, 213),
                    Argb(18, 0, 0, 0), Argb(32, 0, 0, 0), Argb(255, 0, 95, 184)};
        }
        return {Argb(250, 44, 44, 44), Argb(255, 255, 255, 255), Argb(255, 197, 197, 197), Argb(255, 70, 70, 70),
                Argb(22, 255, 255, 255), Argb(36, 255, 255, 255), Argb(255, 96, 205, 255)};
    }
};

// The glow around the selection.
const gp::Color kSelectionColor = Argb(255, 76, 194, 255);
// The line around an area being recorded.
const gp::Color kRecordRed = Argb(255, 232, 17, 35);

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
const wchar_t* RegisterForwardingClass(const wchar_t* name, LPCWSTR cursor = IDC_ARROW) {
    std::vector<std::wstring>& classes = RegisteredClasses();
    if (std::find(classes.begin(), classes.end(), name) != classes.end()) {
        return name;
    }
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.hInstance = ModuleInstance();
    windowClass.hCursor = LoadCursorW(nullptr, cursor);
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

// A hidden window for messages from the keyboard hook and from Windhawk's thread.
class MessageWindow {
public:
    std::function<void(UINT message, WPARAM wParam, LPARAM lParam)> onMessage;

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

// Messages to the app's message window.
constexpr UINT kPrintScreenMessage = WM_APP + 1;
constexpr UINT kEscapeMessage = WM_APP + 2;
constexpr UINT kSettingsChangedMessage = WM_APP + 3;
constexpr UINT kOverlayClosedMessage = WM_APP + 4;
constexpr UINT kRecordMessage = WM_APP + 5;
constexpr UINT kTrimClosedMessage = WM_APP + 6;  // lParam is the window.

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
    // taskbar or for good. Its other messages (WM_MOUSELEAVE, WM_CAPTURECHANGED and
    // WM_DPICHANGED_AFTERPARENT) and the other widgets' notices come as they are.
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

// ---------------------------------------------------------------------------
// The buttons on the taskbar: the screenshot button takes a screenshot (as Print Screen does), and
// the record button records part of the screen, or stops recording. Right-click them for their
// menu, and drag them to move them along the taskbar.

class SnipperWidget : public TaskbarWidget {
public:
    std::function<void()> onSnip;
    std::function<void()> onRecord;
    std::function<void()> onMenu;

    // Which buttons show: one or both (with neither asked for, both).
    void SetButtons(bool screenshot, bool record) {
        if (!screenshot && !record) {
            screenshot = record = true;
        }
        if (screenshot != screenshotButton_ || record != recordButton_) {
            screenshotButton_ = screenshot;
            recordButton_ = record;
            hovered_ = -1;
            ContentChanged();
        }
    }

    // While recording, the record button shows a stop square.
    void SetRecording(bool recording) {
        if (recording != recording_) {
            recording_ = recording;
            Render();  // Once the widget has a window.
        }
    }

protected:
    int DefaultOffset(int barWidth, int width) override {
        // Toward the right, short of the notification area, where the taskbar is free whether its
        // icons are centered or on the left.
        return barWidth - width - Px(400);
    }

    int LayOut() override { return ButtonWidth() * ButtonCount(); }

    void Draw(gp::Graphics& graphics, int width, int height, bool light) override {
        gp::Color foreground = light ? Argb(255, 28, 28, 28) : Argb(255, 255, 255, 255);
        int buttonWidth = width / ButtonCount();
        for (int button = 0; button < ButtonCount(); button++) {
            float left = (float)(button * buttonWidth);
            if (button == hovered_ && !Dragging()) {
                FillRoundedRect(graphics, WithAlpha(light ? 18 : 24, foreground),
                                gp::RectF(left + Px(2), (float)Px(4), (float)(buttonWidth - Px(4)), (float)(height - Px(8))), (float)Px(4));
            }
            // About the size of the taskbar's own icons; while recording, the record button stops it.
            gp::RectF icon(left, 0, (float)buttonWidth, (float)height);
            const wchar_t* glyph = !IsRecordButton(button) ? kGlyphSnip : recording_ ? StopRecordingGlyph() : kGlyphRecord;
            DrawGlyph(graphics, glyph, 18 * Scale(), icon, foreground);
        }
    }

    void OnHover(POINT point) override {
        int button = ButtonAt(point);
        if (button != hovered_) {
            hovered_ = button;
            Render();
        }
    }

    void OnHoverLeave() override {
        if (hovered_ >= 0) {
            hovered_ = -1;
            Render();
        }
    }

    void OnClick(POINT point, MouseButton button) override {
        if (button == MouseButton::Left) {
            int clicked = ButtonAt(point);
            // Not highlighted in the screenshot or recording.
            hovered_ = -1;
            Render();
            auto action = IsRecordButton(clicked) ? onRecord : onSnip;
            if (action) {
                action();
            }
        } else if (button == MouseButton::Right && onMenu) {
            onMenu();
        }
    }

private:
    int ButtonCount() const { return screenshotButton_ && recordButton_ ? 2 : 1; }
    int ButtonWidth() const { return Px(ButtonCount() == 2 ? 40 : 44); }
    int ButtonAt(POINT point) const { return ButtonCount() == 2 && point.x >= ButtonWidth() ? 1 : 0; }
    // The screenshot button comes first when both show.
    bool IsRecordButton(int button) const { return !screenshotButton_ || button == 1; }

    bool screenshotButton_ = true;
    bool recordButton_ = true;
    bool recording_ = false;
    int hovered_ = -1;  // The button under the mouse.
};

// ---------------------------------------------------------------------------
// The keyboard hook: catches Print Screen (on its own, without Alt, Ctrl, Shift or Win), and
// Win+Shift+R when the setting is on, before Windows acts on them, and tells the app. It runs on a thread of its own, so a busy UI thread never
// holds up typing. While the overlay is open but another window is in front (before the first
// click on the overlay), it also catches Esc, which would otherwise go to that window.

class KeyboardHook {
public:
    // Called on the UI thread, whose windows are the mod's.
    bool Start(HWND notify) {
        notify_ = notify;
        uiThreadId_ = GetCurrentThreadId();
        instance_ = this;
        ready_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, &threadId_);
        if (!thread_) {
            return false;
        }
        WaitForSingleObject(ready_, INFINITE);
        return hooked_;
    }

    void Stop() {
        if (thread_) {
            PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
            WaitForSingleObject(thread_, INFINITE);  // The hook is in the mod's code.
            CloseHandle(thread_);
            thread_ = nullptr;
        }
        if (ready_) {
            CloseHandle(ready_);
            ready_ = nullptr;
        }
        instance_ = nullptr;
    }

    ~KeyboardHook() { Stop(); }

    void SetOverlayOpen(bool open) { overlayOpen_ = open; }
    void SetRecordShortcut(bool on) { recordShortcut_ = on; }

private:
    static DWORD WINAPI ThreadProc(void* parameter) {
        auto self = (KeyboardHook*)parameter;
        MSG message;
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // Creates the message queue.
        HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, HookProc, ModuleInstance(), 0);
        self->hooked_ = hook != nullptr;
        if (!hook) {
            Wh_Log(L"Couldn't install the keyboard hook: error %u", GetLastError());
        }
        SetEvent(self->ready_);
        if (hook) {
            while (GetMessageW(&message, nullptr, 0, 0) > 0) {
                DispatchMessageW(&message);
            }
            UnhookWindowsHookEx(hook);
        }
        return 0;
    }

    static bool IsDown(int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; }

    // Presses an unassigned key, so letting go of Win after a shortcut taken over doesn't open Start.
    static void MaskWinKey() {
        INPUT inputs[2] = {};
        for (INPUT& input : inputs) {
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 0xE8;
        }
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inputs, sizeof(INPUT));
    }

    // Whether one of the mod's windows is in front. They're the UI thread's: the rest of the
    // process is Explorer's (File Explorer windows, the taskbar, the desktop).
    bool OwnWindowInFront() const { return GetWindowThreadProcessId(GetForegroundWindow(), nullptr) == uiThreadId_; }

    static LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam) {
        KeyboardHook* self = instance_;
        if (code == HC_ACTION && self) {
            auto key = (const KBDLLHOOKSTRUCT*)lParam;
            bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            if (key->vkCode == VK_SNAPSHOT) {
                if (down) {
                    bool modified = IsDown(VK_MENU) || IsDown(VK_CONTROL) || IsDown(VK_SHIFT) || IsDown(VK_LWIN) ||
                                    IsDown(VK_RWIN);
                    if (!modified) {
                        // Held down, the key repeats; one press is one screenshot.
                        if (!self->printScreenHeld_) {
                            self->printScreenHeld_ = true;
                            PostMessageW(self->notify_, kPrintScreenMessage, 0, 0);
                        }
                        return 1;
                    }
                } else if (self->printScreenHeld_) {
                    self->printScreenHeld_ = false;
                    return 1;
                }
            } else if (key->vkCode == 'R' && self->recordShortcut_) {
                if (down) {
                    bool win = IsDown(VK_LWIN) || IsDown(VK_RWIN);
                    if (win && IsDown(VK_SHIFT) && !IsDown(VK_CONTROL) && !IsDown(VK_MENU)) {
                        if (!self->recordHeld_) {
                            self->recordHeld_ = true;
                            PostMessageW(self->notify_, kRecordMessage, 0, 0);
                            MaskWinKey();
                        }
                        return 1;
                    }
                } else if (self->recordHeld_) {
                    self->recordHeld_ = false;
                    return 1;
                }
            } else if (key->vkCode == VK_ESCAPE && down && self->overlayOpen_ && !self->OwnWindowInFront()) {
                PostMessageW(self->notify_, kEscapeMessage, 0, 0);
                return 1;
            }
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    static inline KeyboardHook* instance_ = nullptr;
    DWORD uiThreadId_ = 0;
    HWND notify_ = nullptr;
    HANDLE thread_ = nullptr;
    HANDLE ready_ = nullptr;
    DWORD threadId_ = 0;
    bool hooked_ = false;
    bool printScreenHeld_ = false;  // Only used on the hook's thread.
    bool recordHeld_ = false;       // Likewise.
    std::atomic<bool> recordShortcut_{false};
    std::atomic<bool> overlayOpen_{false};
};

// ---------------------------------------------------------------------------
// The screen: a copy of every display, taken when Print Screen is pressed, and where its windows are.

class ScreenCopy {
public:
    ScreenCopy() = default;
    ScreenCopy(const ScreenCopy&) = delete;
    ScreenCopy& operator=(const ScreenCopy&) = delete;

    ~ScreenCopy() {
        if (dc_) {
            SelectObject(dc_, previous_);
            DeleteDC(dc_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
    }

    // Copies the whole desktop, across every display, in physical pixels.
    bool Take() {
        bounds_ = {GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), 0, 0};
        bounds_.right = bounds_.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        bounds_.bottom = bounds_.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if (Width() <= 0 || Height() <= 0) {
            return false;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = Width();
        info.bmiHeader.biHeight = -Height();
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        HDC screen = GetDC(nullptr);
        dc_ = CreateCompatibleDC(screen);
        bitmap_ = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits_, nullptr, 0);
        bool ok = false;
        if (dc_ && bitmap_ && bits_) {
            previous_ = SelectObject(dc_, bitmap_);
            ok = BitBlt(dc_, 0, 0, Width(), Height(), screen, bounds_.left, bounds_.top, SRCCOPY | CAPTUREBLT);
        }
        ReleaseDC(nullptr, screen);
        return ok;
    }

    const RECT& Bounds() const { return bounds_; }
    int Width() const { return bounds_.right - bounds_.left; }
    int Height() const { return bounds_.bottom - bounds_.top; }
    HDC Dc() const { return dc_; }
    const BYTE* Bits() const { return (const BYTE*)bits_; }

private:
    RECT bounds_{};
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ previous_ = nullptr;
    void* bits_ = nullptr;
};

struct WindowSearch {
    DWORD ownThreadId;  // The UI thread's windows are the mod's; Explorer's others count.
    std::vector<RECT> rects;
};

BOOL CALLBACK CollectWindow(HWND window, LPARAM data) {
    auto search = (WindowSearch*)data;
    if (!IsWindowVisible(window) || IsIconic(window)) {
        return TRUE;
    }
    if (GetWindowThreadProcessId(window, nullptr) == search->ownThreadId) {
        return TRUE;
    }
    // Windows hidden by the shell (other virtual desktops, suspended apps), click-through overlays,
    // fully transparent windows and the desktop itself don't count.
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
        return TRUE;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (exStyle & WS_EX_TRANSPARENT) {
        return TRUE;
    }
    BYTE alpha = 255;
    DWORD flags = 0;
    if ((exStyle & WS_EX_LAYERED) && GetLayeredWindowAttributes(window, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) &&
        alpha == 0) {
        return TRUE;
    }
    WCHAR className[64] = {};
    GetClassNameW(window, className, ARRAYSIZE(className));
    if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
        return TRUE;
    }
    RECT rect{};
    if (FAILED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(rect)))) {
        GetWindowRect(window, &rect);
    }
    if (rect.right - rect.left > 0 && rect.bottom - rect.top > 0) {
        search->rects.push_back(rect);
    }
    return TRUE;
}

// The visible windows' outlines on screen, from the frontmost back.
std::vector<RECT> VisibleWindowRects() {
    WindowSearch search{GetCurrentThreadId(), {}};
    EnumWindows(CollectWindow, (LPARAM)&search);
    return search.rects;
}

// ---------------------------------------------------------------------------
// Marks: what the tools draw over the selection. Each kind of mark is a class that draws,
// measures, hit-tests, moves and copies itself, so the overlay handles every kind the same way,
// and a new kind needs nothing changed anywhere else.

constexpr int kMinThickness = 1;
constexpr int kMaxThickness = 20;

gp::RectF BoxRect(const gp::PointF& a, const gp::PointF& b) {
    return gp::RectF(std::min(a.X, b.X), std::min(a.Y, b.Y), std::abs(b.X - a.X), std::abs(b.Y - a.Y));
}

float DistanceToSegment(const gp::PointF& point, const gp::PointF& a, const gp::PointF& b) {
    float dx = b.X - a.X;
    float dy = b.Y - a.Y;
    float lengthSquared = dx * dx + dy * dy;
    float t = lengthSquared > 0 ? std::clamp(((point.X - a.X) * dx + (point.Y - a.Y) * dy) / lengthSquared, 0.0f, 1.0f) : 0;
    return std::hypot(point.X - (a.X + t * dx), point.Y - (a.Y + t * dy));
}

// The distance from the point to the rectangle, or 0 inside it.
float DistanceToRect(const gp::PointF& point, const gp::RectF& rect) {
    float dx = std::max({rect.X - point.X, 0.0f, point.X - rect.GetRight()});
    float dy = std::max({rect.Y - point.Y, 0.0f, point.Y - rect.GetBottom()});
    return std::hypot(dx, dy);
}

// The point, moved to the nearest 45° direction from the start (Shift held while drawing a line).
gp::PointF AtStepAngle(const gp::PointF& start, const gp::PointF& point) {
    float dx = point.X - start.X;
    float dy = point.Y - start.Y;
    float length = std::hypot(dx, dy);
    float step = 3.14159265f / 4;
    float angle = std::round(std::atan2(dy, dx) / step) * step;
    return gp::PointF(start.X + length * std::cos(angle), start.Y + length * std::sin(angle));
}

void DrawArrow(gp::Graphics& graphics, const gp::PointF& from, const gp::PointF& to, const gp::Color& color, float width) {
    float dx = to.X - from.X;
    float dy = to.Y - from.Y;
    float length = std::hypot(dx, dy);
    if (length < 1) {
        return;
    }
    float ux = dx / length;
    float uy = dy / length;
    float head = std::min(length, std::max(width * 3.5f, 12.0f));
    gp::PointF base(to.X - ux * head, to.Y - uy * head);
    gp::Pen pen(color, width);
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapFlat);
    // The shaft runs a little into the head, so no gap shows between them.
    graphics.DrawLine(&pen, from, gp::PointF(base.X + ux * head * 0.3f, base.Y + uy * head * 0.3f));
    float half = head * 0.55f;
    gp::PointF points[] = {to, {base.X - uy * half, base.Y + ux * half}, {base.X + uy * half, base.Y - ux * half}};
    gp::SolidBrush brush(color);
    graphics.FillPolygon(&brush, points, 3);
}

class Mark {
public:
    virtual ~Mark() = default;

    ARGB color = 0;
    int thickness = 4;  // The toolbox's thickness, 1 to 20: pixels at 100% scaling.
    float scale = 1;    // Of the display the selection is on.

    virtual std::unique_ptr<Mark> Clone() const = 0;
    // Starts it at the point, where the mouse was pressed (or clicked, for text).
    virtual void StartAt(const gp::PointF& point) = 0;
    // While it's being drawn, the mouse is at the point. constrain (Shift held) keeps lines to 45°
    // steps and boxes square.
    virtual void DragTo(const gp::PointF& point, bool constrain) {}
    // Whether there's enough of it to keep, once it's drawn (or typed).
    virtual bool Worthwhile() const = 0;
    // Draws it; caret draws a text cursor, for text being typed.
    virtual void Draw(gp::Graphics& graphics, bool caret) const = 0;
    // The area it covers, leaving out the width of its lines.
    virtual gp::RectF Outline() const = 0;
    // The distance from the point to what it draws (0 on it, and inside text).
    virtual float DistanceTo(const gp::PointF& point) const = 0;
    virtual void MoveBy(float dx, float dy) = 0;
    // Its text, for a mark that can be typed into, or null.
    virtual std::wstring* Text() { return nullptr; }

    // The width of its lines, in pixels.
    float Width() const { return thickness * scale; }

    // Whether the point is on it, as far as clicking it goes.
    bool Hit(const gp::PointF& point) const { return DistanceTo(point) <= Width() / 2 + 5 * scale; }

    // Where it draws, with room to spare, in whole pixels: to repaint.
    RECT Bounds() const {
        gp::RectF outline = Outline();
        float margin = Width() * 4 + 8 * scale;
        return {(LONG)std::floor(outline.X - margin), (LONG)std::floor(outline.Y - margin),
                (LONG)std::ceil(outline.GetRight() + margin), (LONG)std::ceil(outline.GetBottom() + margin)};
    }
};

// A freehand line, following the mouse.
class PencilMark : public Mark {
public:
    std::unique_ptr<Mark> Clone() const override { return std::make_unique<PencilMark>(*this); }

    void StartAt(const gp::PointF& point) override { points_ = {point}; }

    void DragTo(const gp::PointF& point, bool constrain) override {
        const gp::PointF& last = points_.back();
        if (std::hypot(point.X - last.X, point.Y - last.Y) >= 1.5f * scale) {
            points_.push_back(point);
        }
    }

    bool Worthwhile() const override { return !points_.empty(); }

    void Draw(gp::Graphics& graphics, bool caret) const override {
        gp::Color ink(color);
        if (points_.size() == 1) {
            // A click makes a dot.
            gp::SolidBrush brush(ink);
            graphics.FillEllipse(&brush, points_[0].X - Width() / 2, points_[0].Y - Width() / 2, Width(), Width());
            return;
        }
        gp::Pen pen(ink, Width());
        pen.SetStartCap(gp::LineCapRound);
        pen.SetEndCap(gp::LineCapRound);
        pen.SetLineJoin(gp::LineJoinRound);
        graphics.DrawLines(&pen, points_.data(), (INT)points_.size());
    }

    gp::RectF Outline() const override {
        float left = points_[0].X, top = points_[0].Y, right = left, bottom = top;
        for (const gp::PointF& point : points_) {
            left = std::min(left, point.X);
            top = std::min(top, point.Y);
            right = std::max(right, point.X);
            bottom = std::max(bottom, point.Y);
        }
        return gp::RectF(left, top, right - left, bottom - top);
    }

    float DistanceTo(const gp::PointF& point) const override {
        float nearest = std::hypot(point.X - points_[0].X, point.Y - points_[0].Y);
        for (size_t i = 1; i < points_.size(); i++) {
            nearest = std::min(nearest, DistanceToSegment(point, points_[i - 1], points_[i]));
        }
        return nearest;
    }

    void MoveBy(float dx, float dy) override {
        for (gp::PointF& point : points_) {
            point.X += dx;
            point.Y += dy;
        }
    }

private:
    std::vector<gp::PointF> points_;
};

// A straight line from where the mouse was pressed to where it's let go: the base of lines and
// arrows.
class SegmentMark : public Mark {
public:
    void StartAt(const gp::PointF& point) override { from_ = to_ = point; }

    void DragTo(const gp::PointF& point, bool constrain) override { to_ = constrain ? AtStepAngle(from_, point) : point; }

    bool Worthwhile() const override { return std::hypot(to_.X - from_.X, to_.Y - from_.Y) >= 3 * scale; }

    gp::RectF Outline() const override { return BoxRect(from_, to_); }

    float DistanceTo(const gp::PointF& point) const override { return DistanceToSegment(point, from_, to_); }

    void MoveBy(float dx, float dy) override {
        from_.X += dx;
        from_.Y += dy;
        to_.X += dx;
        to_.Y += dy;
    }

protected:
    gp::PointF from_;
    gp::PointF to_;
};

class LineMark : public SegmentMark {
public:
    std::unique_ptr<Mark> Clone() const override { return std::make_unique<LineMark>(*this); }

    void Draw(gp::Graphics& graphics, bool caret) const override {
        gp::Pen pen(gp::Color(color), Width());
        pen.SetStartCap(gp::LineCapRound);
        pen.SetEndCap(gp::LineCapRound);
        graphics.DrawLine(&pen, from_, to_);
    }
};

class ArrowMark : public SegmentMark {
public:
    std::unique_ptr<Mark> Clone() const override { return std::make_unique<ArrowMark>(*this); }

    void Draw(gp::Graphics& graphics, bool caret) const override { DrawArrow(graphics, from_, to_, gp::Color(color), Width()); }
};

class BoxMark : public Mark {
public:
    std::unique_ptr<Mark> Clone() const override { return std::make_unique<BoxMark>(*this); }

    void StartAt(const gp::PointF& point) override { from_ = to_ = point; }

    void DragTo(const gp::PointF& point, bool constrain) override {
        to_ = point;
        if (constrain) {
            float side = std::max(std::abs(point.X - from_.X), std::abs(point.Y - from_.Y));
            to_ = gp::PointF(from_.X + (point.X < from_.X ? -side : side), from_.Y + (point.Y < from_.Y ? -side : side));
        }
    }

    bool Worthwhile() const override { return std::hypot(to_.X - from_.X, to_.Y - from_.Y) >= 3 * scale; }

    void Draw(gp::Graphics& graphics, bool caret) const override {
        gp::Pen pen(gp::Color(color), Width());
        pen.SetLineJoin(gp::LineJoinRound);
        graphics.DrawRectangle(&pen, Outline());
    }

    gp::RectF Outline() const override { return BoxRect(from_, to_); }

    // On its edges only, so arrows and text can go inside a box.
    float DistanceTo(const gp::PointF& point) const override {
        gp::RectF rect = Outline();
        gp::PointF corners[] = {{rect.X, rect.Y}, {rect.GetRight(), rect.Y}, {rect.GetRight(), rect.GetBottom()}, {rect.X, rect.GetBottom()}};
        float nearest = DistanceToSegment(point, corners[3], corners[0]);
        for (int i = 1; i < 4; i++) {
            nearest = std::min(nearest, DistanceToSegment(point, corners[i - 1], corners[i]));
        }
        return nearest;
    }

    void MoveBy(float dx, float dy) override {
        from_.X += dx;
        from_.Y += dy;
        to_.X += dx;
        to_.Y += dy;
    }

private:
    gp::PointF from_;
    gp::PointF to_;
};

class TextMark : public Mark {
public:
    std::unique_ptr<Mark> Clone() const override { return std::make_unique<TextMark>(*this); }

    // The click is where the middle of the first line goes.
    void StartAt(const gp::PointF& point) override { topLeft_ = gp::PointF(point.X, point.Y - FontSize() * 0.7f); }

    bool Worthwhile() const override { return !Trim(text_).empty(); }

    void Draw(gp::Graphics& graphics, bool caret) const override {
        gp::Font font(L"Segoe UI Semibold", FontSize(), gp::FontStyleRegular, gp::UnitPixel);
        gp::StringFormat format(gp::StringFormat::GenericTypographic());
        format.SetFormatFlags(format.GetFormatFlags() | gp::StringFormatFlagsMeasureTrailingSpaces);
        gp::SolidBrush brush{gp::Color(color)};
        float lineHeight = font.GetHeight(&graphics);
        std::vector<std::wstring> lines = Lines();
        for (size_t i = 0; i < lines.size(); i++) {
            graphics.DrawString(lines[i].c_str(), (INT)lines[i].size(), &font, gp::PointF(topLeft_.X, topLeft_.Y + lineHeight * i),
                                &format, &brush);
        }
        if (caret) {
            gp::RectF last;
            graphics.MeasureString(lines.back().c_str(), (INT)lines.back().size(), &font, gp::PointF(0, 0), &format, &last);
            float x = topLeft_.X + last.Width + FontSize() * 0.05f;
            float y = topLeft_.Y + lineHeight * (lines.size() - 1);
            gp::Pen pen(gp::Color(color), std::max(1.5f, FontSize() / 14));
            graphics.DrawLine(&pen, x, y + lineHeight * 0.12f, x, y + lineHeight * 0.92f);
        }
    }

    gp::RectF Outline() const override {
        gp::Bitmap measuring(1, 1, PixelFormat32bppARGB);
        gp::Graphics graphics(&measuring);
        gp::Font font(L"Segoe UI Semibold", FontSize(), gp::FontStyleRegular, gp::UnitPixel);
        gp::StringFormat format(gp::StringFormat::GenericTypographic());
        format.SetFormatFlags(format.GetFormatFlags() | gp::StringFormatFlagsMeasureTrailingSpaces);
        std::vector<std::wstring> lines = Lines();
        float width = FontSize() * 0.3f;  // Room for the caret.
        for (const std::wstring& line : lines) {
            gp::RectF bounds;
            graphics.MeasureString(line.c_str(), (INT)line.size(), &font, gp::PointF(0, 0), &format, &bounds);
            width = std::max(width, bounds.Width + FontSize() * 0.3f);
        }
        return gp::RectF(topLeft_.X, topLeft_.Y, width, font.GetHeight(&graphics) * lines.size());
    }

    float DistanceTo(const gp::PointF& point) const override { return DistanceToRect(point, Outline()); }

    void MoveBy(float dx, float dy) override {
        topLeft_.X += dx;
        topLeft_.Y += dy;
    }

    std::wstring* Text() override { return &text_; }

private:
    // Text gets bigger with the thickness.
    float FontSize() const { return (12.0f + 2.0f * thickness) * scale; }

    std::vector<std::wstring> Lines() const {
        std::vector<std::wstring> lines(1);
        for (wchar_t c : text_) {
            if (c == L'\n') {
                lines.emplace_back();
            } else {
                lines.back() += c;
            }
        }
        return lines;
    }

    gp::PointF topLeft_;
    std::wstring text_;
};

// ---------------------------------------------------------------------------
// The tools. Each is one entry in kTools: its name and shortcut, its cursor, its toolbox icon and
// the mark it makes. The toolbox, its tooltips, the shortcuts and the overlay all go by this list,
// so adding a tool is adding an entry (and its mark).

struct Tool {
    const wchar_t* name;  // For its tooltip.
    wchar_t key;          // Its shortcut, a capital letter; also how the last tool used is remembered.
    LPCWSTR cursor;       // Over the selection.
    // Draws its icon in the toolbox.
    void (*drawIcon)(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale);
    // Makes the mark it draws, or null for a tool that draws none (selecting and moving).
    std::unique_ptr<Mark> (*makeMark)();
    // A click places the mark and typing fills it (text), rather than a drag drawing it.
    bool typed;
};

void DrawSelectIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    DrawGlyph(graphics, kGlyphSelect, 16 * scale, box, color);
}

void DrawPencilIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    DrawGlyph(graphics, kGlyphPencil, 16 * scale, box, color);
}

// The icon font has no line, so it's drawn, about as thick as the glyphs look.
void DrawLineIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    gp::Pen pen(color, 1.5f * scale);
    pen.SetStartCap(gp::LineCapRound);
    pen.SetEndCap(gp::LineCapRound);
    graphics.DrawLine(&pen, box.X + 2 * scale, box.GetBottom() - 2 * scale, box.GetRight() - 2 * scale, box.Y + 2 * scale);
}

void DrawArrowIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    DrawGlyph(graphics, kGlyphArrow, 16 * scale, box, color);
}

void DrawBoxIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    DrawGlyph(graphics, kGlyphBox, 16 * scale, box, color);
}

void DrawTextIcon(gp::Graphics& graphics, const gp::RectF& box, const gp::Color& color, float scale) {
    DrawGlyph(graphics, kGlyphText, 16 * scale, box, color);
}

template <typename T>
std::unique_ptr<Mark> MakeMark() {
    return std::make_unique<T>();
}

const Tool kTools[] = {
    {L"Select and move", L'V', IDC_ARROW, DrawSelectIcon, nullptr, false},
    {L"Pencil", L'P', IDC_CROSS, DrawPencilIcon, MakeMark<PencilMark>, false},
    {L"Line", L'L', IDC_CROSS, DrawLineIcon, MakeMark<LineMark>, false},
    {L"Arrow", L'A', IDC_CROSS, DrawArrowIcon, MakeMark<ArrowMark>, false},
    {L"Box", L'B', IDC_CROSS, DrawBoxIcon, MakeMark<BoxMark>, false},
    {L"Text", L'T', IDC_IBEAM, DrawTextIcon, MakeMark<TextMark>, true},
};
constexpr int kToolCount = ARRAYSIZE(kTools);
constexpr wchar_t kDefaultToolKey = L'A';

// The tool with the shortcut, or -1.
int ToolForKey(wchar_t key) {
    for (int i = 0; i < kToolCount; i++) {
        if (kTools[i].key == key) {
            return i;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------
// The finished screenshot: the selected part of the screen with its marks.

std::unique_ptr<gp::Bitmap> ComposeScreenshot(const ScreenCopy& screen, const RECT& selection,
                                              const std::vector<std::unique_ptr<Mark>>& marks) {
    int width = selection.right - selection.left;
    int height = selection.bottom - selection.top;
    auto image = std::make_unique<gp::Bitmap>(width, height, PixelFormat32bppARGB);
    gp::Rect all(0, 0, width, height);
    gp::BitmapData data;
    if (image->LockBits(&all, gp::ImageLockModeWrite, PixelFormat32bppARGB, &data) != gp::Ok) {
        return nullptr;
    }
    // The screen copy's pixels, made opaque (the screen's alpha bits mean nothing).
    for (int y = 0; y < height; y++) {
        auto source = (const DWORD*)(screen.Bits() + ((size_t)(selection.top + y) * screen.Width() + selection.left) * 4);
        auto target = (DWORD*)((BYTE*)data.Scan0 + (size_t)y * data.Stride);
        for (int x = 0; x < width; x++) {
            target[x] = source[x] | 0xFF000000;
        }
    }
    image->UnlockBits(&data);
    gp::Graphics graphics(image.get());
    graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
    graphics.TranslateTransform((float)-selection.left, (float)-selection.top);
    for (const std::unique_ptr<Mark>& mark : marks) {
        mark->Draw(graphics, false);
    }
    return image;
}

bool PngEncoder(CLSID& clsid) {
    UINT count = 0;
    UINT size = 0;
    if (gp::GetImageEncodersSize(&count, &size) != gp::Ok || size == 0) {
        return false;
    }
    std::vector<BYTE> buffer(size);
    auto encoders = (gp::ImageCodecInfo*)buffer.data();
    if (gp::GetImageEncoders(count, size, encoders) != gp::Ok) {
        return false;
    }
    for (UINT i = 0; i < count; i++) {
        if (wcscmp(encoders[i].MimeType, L"image/png") == 0) {
            clsid = encoders[i].Clsid;
            return true;
        }
    }
    return false;
}

std::vector<BYTE> EncodePng(gp::Bitmap& image) {
    std::vector<BYTE> bytes;
    CLSID png;
    IStream* stream = nullptr;
    if (!PngEncoder(png) || FAILED(CreateStreamOnHGlobal(nullptr, TRUE, &stream))) {
        return bytes;
    }
    STATSTG stat{};
    HGLOBAL memory = nullptr;
    if (image.Save(stream, &png, nullptr) == gp::Ok && SUCCEEDED(stream->Stat(&stat, STATFLAG_NONAME)) &&
        SUCCEEDED(GetHGlobalFromStream(stream, &memory))) {
        if (const void* data = GlobalLock(memory)) {
            bytes.assign((const BYTE*)data, (const BYTE*)data + stat.cbSize.QuadPart);
            GlobalUnlock(memory);
        }
    }
    stream->Release();
    return bytes;
}

HGLOBAL GlobalCopy(const void* data, size_t size) {
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, size);
    if (memory) {
        memcpy(GlobalLock(memory), data, size);
        GlobalUnlock(memory);
    }
    return memory;
}

// Puts the screenshot on the clipboard, as a bitmap and as PNG (which many apps prefer).
bool CopyToClipboard(HWND owner, gp::Bitmap& image) {
    UINT width = image.GetWidth();
    UINT height = image.GetHeight();
    gp::Rect all(0, 0, (INT)width, (INT)height);
    gp::BitmapData data;
    if (image.LockBits(&all, gp::ImageLockModeRead, PixelFormat32bppARGB, &data) != gp::Ok) {
        return false;
    }
    // A DIB is stored bottom row first.
    std::vector<BYTE> dib(sizeof(BITMAPINFOHEADER) + (size_t)width * height * 4);
    auto header = (BITMAPINFOHEADER*)dib.data();
    header->biSize = sizeof(BITMAPINFOHEADER);
    header->biWidth = (LONG)width;
    header->biHeight = (LONG)height;
    header->biPlanes = 1;
    header->biBitCount = 32;
    header->biCompression = BI_RGB;
    header->biSizeImage = width * height * 4;
    for (UINT y = 0; y < height; y++) {
        memcpy(dib.data() + sizeof(BITMAPINFOHEADER) + (size_t)(height - 1 - y) * width * 4,
               (const BYTE*)data.Scan0 + (size_t)y * data.Stride, (size_t)width * 4);
    }
    image.UnlockBits(&data);
    std::vector<BYTE> png = EncodePng(image);

    // Another app may have the clipboard open for a moment.
    bool opened = false;
    for (int attempt = 0; attempt < 10 && !(opened = OpenClipboard(owner)); attempt++) {
        Sleep(20);
    }
    if (!opened) {
        return false;
    }
    EmptyClipboard();
    bool ok = false;
    if (HGLOBAL memory = GlobalCopy(dib.data(), dib.size())) {
        ok = SetClipboardData(CF_DIB, memory) != nullptr;
        if (!ok) {
            GlobalFree(memory);
        }
    }
    if (!png.empty()) {
        if (HGLOBAL memory = GlobalCopy(png.data(), png.size())) {
            if (!SetClipboardData(RegisterClipboardFormatW(L"PNG"), memory)) {
                GlobalFree(memory);
            }
        }
    }
    CloseClipboard();
    return ok;
}

// Saves the screenshot in the folder as "Screenshot <date> <time>.png". Returns its path, or
// nothing if it couldn't be saved.
std::optional<std::wstring> SaveScreenshot(gp::Bitmap& image, const std::wstring& folder) {
    CLSID png;
    if (folder.empty() || !PngEncoder(png)) {
        return std::nullopt;
    }
    SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);  // Fails harmlessly when it's there already.
    std::wstring path = NewFileName(folder, L"Screenshot", L".png");
    if (image.Save(path.c_str(), &png, nullptr) != gp::Ok) {
        return std::nullopt;
    }
    return path;
}

// Shows the file in File Explorer, selected.
void ShowInExplorer(const std::wstring& path) {
    PIDLIST_ABSOLUTE item = ILCreateFromPathW(path.c_str());
    if (item) {
        SHOpenFolderAndSelectItems(item, 0, nullptr, 0);
        ILFree(item);
    }
}

// ---------------------------------------------------------------------------
// Video files: screen recordings are MP4 files (H.264 video, no sound), written and read with
// Windows' Media Foundation a frame at a time. Frames are 32-bit pixels (blue, green, red and an
// unused byte), top row first.

constexpr LONGLONG kTicksPerSecond = 10'000'000;  // Media Foundation's times are in 100 ns ticks.
constexpr int kRecordingFps = 30;

// A COM interface pointer, released when it goes.
template <typename T>
class Com {
public:
    Com() = default;
    ~Com() { Reset(); }
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    Com(Com&& other) noexcept : pointer_(other.pointer_) { other.pointer_ = nullptr; }
    Com& operator=(Com&& other) noexcept {
        if (this != &other) {
            Reset();
            pointer_ = other.pointer_;
            other.pointer_ = nullptr;
        }
        return *this;
    }

    T* operator->() const { return pointer_; }
    T* Get() const { return pointer_; }
    explicit operator bool() const { return pointer_ != nullptr; }

    // For functions that hand back an interface.
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

private:
    T* pointer_ = nullptr;
};

// Media Foundation, started for as long as this is kept. Starts and stops nest.
class MediaFoundation {
public:
    MediaFoundation() : started_(SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET))) {}
    ~MediaFoundation() {
        if (started_) {
            MFShutdown();
        }
    }
    MediaFoundation(const MediaFoundation&) = delete;
    MediaFoundation& operator=(const MediaFoundation&) = delete;

    bool Started() const { return started_; }

private:
    bool started_;
};

// COM for a worker thread, for as long as this is kept.
class ComThread {
public:
    ComThread() : initialized_(SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {}
    ~ComThread() {
        if (initialized_) {
            CoUninitialize();
        }
    }
    ComThread(const ComThread&) = delete;
    ComThread& operator=(const ComThread&) = delete;

private:
    bool initialized_;
};

// The size a video of an area of the screen is recorded at: the same, unless it's more than the
// encoder takes (bigger than 4K), and a whole number of pixel pairs, as H.264 needs. In a 32-bit
// process (Explorer on 32-bit Windows), whose memory runs out around 2 GB, recording, trimming and
// saving a video bigger than 2560 × 1440 wouldn't fit: bigger areas are recorded at that many
// pixels.
SIZE VideoSizeFor(int width, int height) {
    double most = sizeof(void*) == 4 ? 2560.0 * 1440.0 : 3840.0 * 2160.0;
    double shrink = std::min({1.0, std::sqrt(most / ((double)width * height)), 4096.0 / width, 4096.0 / height});
    return {std::max(2, (int)(width * shrink) & ~1), std::max(2, (int)(height * shrink) & ~1)};
}

// Writes a video file, frame by frame.
class VideoWriter {
public:
    // Starts the file. The width and height must be even. In a 64-bit process the graphics card
    // encodes, where it can; in a 32-bit one (on 32-bit Windows) its encoder would take about twice
    // the memory, which isn't there to spare, so the processor does.
    bool Open(const std::wstring& path, int width, int height) {
        if (sizeof(void*) == 8 && Open(path, width, height, true)) {
            return true;
        }
        return Open(path, width, height, false);
    }

    bool Open(const std::wstring& path, int width, int height, bool hardware) {
        writer_.Reset();
        path_ = path;
        hardware_ = hardware;
        wroteFrame_ = false;
        width_ = width;
        height_ = height;
        Com<IMFAttributes> attributes;
        if (FAILED(MFCreateAttributes(attributes.Put(), 3)) ||
            FAILED(attributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4)) ||
            // Frames go straight through the encoder, not held to look ahead: a third of the memory.
            FAILED(attributes->SetUINT32(MF_LOW_LATENCY, TRUE)) ||
            FAILED(attributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, hardware)) ||
            FAILED(MFCreateSinkWriterFromURL(path.c_str(), nullptr, attributes.Get(), writer_.Put()))) {
            return false;
        }
        // About 0.1 bits a pixel: sharp text, with room for scrolling and video.
        double bitrate = std::clamp((double)width * height * kRecordingFps * 0.1, 2e6, 24e6);
        Com<IMFMediaType> output;
        if (FAILED(MFCreateMediaType(output.Put())) || FAILED(output->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video)) ||
            FAILED(output->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264)) || FAILED(output->SetUINT32(MF_MT_AVG_BITRATE, (UINT32)bitrate)) ||
            FAILED(output->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive)) ||
            FAILED(output->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_High)) ||
            // A key frame every second, so trimming finds any frame quickly.
            FAILED(output->SetUINT32(MF_MT_MAX_KEYFRAME_SPACING, kRecordingFps)) ||
            FAILED(MFSetAttributeSize(output.Get(), MF_MT_FRAME_SIZE, width, height)) ||
            FAILED(MFSetAttributeRatio(output.Get(), MF_MT_FRAME_RATE, kRecordingFps, 1)) ||
            FAILED(MFSetAttributeRatio(output.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1)) ||
            FAILED(writer_->AddStream(output.Get(), &stream_))) {
            return false;
        }
        Com<IMFMediaType> input;
        if (FAILED(MFCreateMediaType(input.Put())) || FAILED(input->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video)) ||
            FAILED(input->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32)) ||
            FAILED(input->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive)) ||
            FAILED(input->SetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32)(width * 4))) ||  // Positive: top row first.
            FAILED(MFSetAttributeSize(input.Get(), MF_MT_FRAME_SIZE, width, height)) ||
            FAILED(MFSetAttributeRatio(input.Get(), MF_MT_FRAME_RATE, kRecordingFps, 1)) ||
            FAILED(MFSetAttributeRatio(input.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1)) ||
            FAILED(writer_->SetInputMediaType(stream_, input.Get(), nullptr)) || FAILED(writer_->BeginWriting())) {
            return false;
        }
        return true;
    }

    // Adds a frame of width × height pixels, shown from the time (in ticks from the start).
    bool Write(const BYTE* pixels, LONGLONG time) {
        if (WriteFrame(pixels, time)) {
            wroteFrame_ = true;
            return true;
        }
        // A graphics card's encoder can open and still fail once frames come (when other apps use
        // all its sessions, say): the processor's, from the start.
        if (!hardware_ || wroteFrame_ || !Open(path_, width_, height_, false) || !WriteFrame(pixels, time)) {
            return false;
        }
        wroteFrame_ = true;
        return true;
    }

    // Finishes the file. Returns whether it was all written.
    bool Finish() {
        bool finished = writer_ && SUCCEEDED(writer_->Finalize());
        writer_.Reset();
        return finished;
    }

private:
    bool WriteFrame(const BYTE* pixels, LONGLONG time) {
        DWORD size = (DWORD)width_ * height_ * 4;
        Com<IMFMediaBuffer> buffer;
        BYTE* data = nullptr;
        if (FAILED(MFCreateMemoryBuffer(size, buffer.Put())) || FAILED(buffer->Lock(&data, nullptr, nullptr))) {
            return false;
        }
        memcpy(data, pixels, size);
        buffer->Unlock();
        Com<IMFSample> sample;
        return SUCCEEDED(buffer->SetCurrentLength(size)) && SUCCEEDED(MFCreateSample(sample.Put())) &&
               SUCCEEDED(sample->AddBuffer(buffer.Get())) && SUCCEEDED(sample->SetSampleTime(time)) &&
               SUCCEEDED(sample->SetSampleDuration(kTicksPerSecond / kRecordingFps)) &&
               SUCCEEDED(writer_->WriteSample(stream_, sample.Get()));
    }

    Com<IMFSinkWriter> writer_;
    DWORD stream_ = 0;
    std::wstring path_;
    bool hardware_ = false;
    bool wroteFrame_ = false;
    int width_ = 0;
    int height_ = 0;
};

// Direct3D 11, for decoding video on the graphics card; null where there's none to use.
Com<IMFDXGIDeviceManager> VideoDeviceManager() {
    Com<ID3D11Device> device;
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
                                        D3D_FEATURE_LEVEL_9_3};
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_VIDEO_SUPPORT, levels, ARRAYSIZE(levels),
                                 D3D11_SDK_VERSION, device.Put(), nullptr, nullptr))) {
        return {};
    }
    // The decoder uses it from threads of its own.
    Com<ID3D10Multithread> multithread;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(multithread.Put())))) {
        multithread->SetMultithreadProtected(TRUE);
    }
    UINT resetToken = 0;
    Com<IMFDXGIDeviceManager> manager;
    if (FAILED(MFCreateDXGIDeviceManager(&resetToken, manager.Put())) || FAILED(manager->ResetDevice(device.Get(), resetToken))) {
        return {};
    }
    return manager;
}

// Reads a video file, frame by frame.
class VideoReader {
public:
    // The size of a video's picture, without decoding any of it. False if it can't be read.
    static bool Probe(const std::wstring& path, SIZE& size) {
        Com<IMFSourceReader> reader;
        Com<IMFMediaType> type;
        UINT32 width = 0;
        UINT32 height = 0;
        if (FAILED(MFCreateSourceReaderFromURL(path.c_str(), nullptr, reader.Put())) ||
            FAILED(reader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, type.Put())) ||
            FAILED(MFGetAttributeSize(type.Get(), MF_MT_FRAME_SIZE, &width, &height)) || width == 0 || height == 0) {
            return false;
        }
        size = {(LONG)width, (LONG)height};
        MFVideoArea area;
        if (SUCCEEDED(type->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, (UINT8*)&area, sizeof(area), nullptr)) && area.Area.cx > 0 &&
            area.Area.cy > 0) {
            size = area.Area;
        }
        return true;
    }

    // Opens the video. With a size, Windows scales the frames to it as they're decoded (much
    // cheaper than scaling them after); without, they keep their own. hardware decodes on the
    // graphics card where there is one, else on the processor.
    bool Open(const std::wstring& path, SIZE size = {}, bool hardware = false) {
        Close();
        if (hardware) {
            manager_ = VideoDeviceManager();
        }
        Com<IMFAttributes> attributes;
        Com<IMFMediaType> type;
        if (FAILED(MFCreateAttributes(attributes.Put(), 2)) ||
            // Converts to RGB (and scales) with Windows' video processor. (Not low latency, which
            // would save memory but skips frames.)
            FAILED(attributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE)) ||
            (manager_ && FAILED(attributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, manager_.Get()))) ||
            FAILED(MFCreateSourceReaderFromURL(path.c_str(), attributes.Get(), reader_.Put())) ||
            FAILED(reader_->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE)) ||
            FAILED(reader_->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE)) || FAILED(MFCreateMediaType(type.Put())) ||
            FAILED(type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video)) || FAILED(type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32)) ||
            (size.cx > 0 && size.cy > 0 && FAILED(MFSetAttributeSize(type.Get(), MF_MT_FRAME_SIZE, size.cx, size.cy))) ||
            FAILED(reader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, type.Get())) || !ReadFormat()) {
            // Without the graphics card, then.
            return manager_ ? Open(path, size, false) : (Close(), false);
        }
        PROPVARIANT duration;
        PropVariantInit(&duration);
        if (SUCCEEDED(reader_->GetPresentationAttribute((DWORD)MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration))) {
            duration_ = (LONGLONG)duration.uhVal.QuadPart;
        }
        PropVariantClear(&duration);
        // Times count from the first frame. The encoder's reordered (B) frames can put it a frame or
        // two in.
        LONGLONG time = 0;
        if (duration_ <= 0 || !Next(time)) {
            return manager_ ? Open(path, size, false) : (Close(), false);
        }
        origin_ = time;
        return Seek(0);
    }

    void Close() {
        sample_.Reset();
        reader_.Reset();
        manager_.Reset();
        duration_ = 0;
        origin_ = 0;
    }

    int Width() const { return width_; }
    int Height() const { return height_; }
    LONGLONG Duration() const { return duration_; }
    bool IsOpen() const { return (bool)reader_; }

    // Goes back or ahead to the key frame at or before the time; reading goes on from there.
    bool Seek(LONGLONG time) {
        sample_.Reset();
        if (!reader_) {
            return false;
        }
        PROPVARIANT position;
        PropVariantInit(&position);
        position.vt = VT_I8;
        position.hVal.QuadPart = std::max<LONGLONG>(0, time) + origin_;
        return SUCCEEDED(reader_->SetCurrentPosition(GUID_NULL, position));
    }

    // Reads the next frame, with the time it shows from; Pixels copies it out. False at the end of
    // the video, or if it can't be read.
    bool Next(LONGLONG& time) {
        sample_.Reset();
        if (!reader_) {
            return false;
        }
        while (true) {
            DWORD flags = 0;
            LONGLONG timestamp = 0;
            if (FAILED(reader_->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &flags, &timestamp, sample_.Put())) ||
                (flags & (MF_SOURCE_READERF_ERROR | MF_SOURCE_READERF_ENDOFSTREAM))) {
                sample_.Reset();
                return false;
            }
            if ((flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) && !ReadFormat()) {
                sample_.Reset();
                return false;
            }
            if (sample_) {
                time = timestamp - origin_;
                return true;
            }
            // A gap in the stream.
        }
    }

    // Copies the frame read last into the pixels (Width() × Height(), top row first).
    bool Pixels(std::vector<BYTE>& pixels) { return sample_ && CopyFrame(sample_.Get(), pixels); }

    // Reads the next frame into the pixels.
    bool Read(std::vector<BYTE>& pixels, LONGLONG& time) { return Next(time) && Pixels(pixels); }

private:
    // The frames' size, and where the picture is in them: decoded frames can be rounded up to a
    // whole number of 16-pixel blocks, with the picture in their top-left.
    bool ReadFormat() {
        Com<IMFMediaType> type;
        UINT32 width = 0;
        UINT32 height = 0;
        if (FAILED(reader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, type.Put())) ||
            FAILED(MFGetAttributeSize(type.Get(), MF_MT_FRAME_SIZE, &width, &height)) || width == 0 || height == 0) {
            return false;
        }
        frameWidth_ = (int)width;
        frameHeight_ = (int)height;
        left_ = 0;
        top_ = 0;
        width_ = frameWidth_;
        height_ = frameHeight_;
        MFVideoArea area;
        for (const GUID& aperture : {MF_MT_MINIMUM_DISPLAY_APERTURE, MF_MT_GEOMETRIC_APERTURE}) {
            if (SUCCEEDED(type->GetBlob(aperture, (UINT8*)&area, sizeof(area), nullptr)) && area.Area.cx > 0 && area.Area.cy > 0) {
                left_ = std::clamp((int)area.OffsetX.value, 0, frameWidth_ - 1);
                top_ = std::clamp((int)area.OffsetY.value, 0, frameHeight_ - 1);
                width_ = std::min((int)area.Area.cx, frameWidth_ - left_);
                height_ = std::min((int)area.Area.cy, frameHeight_ - top_);
                break;
            }
        }
        UINT32 stride = 0;
        stride_ = SUCCEEDED(type->GetUINT32(MF_MT_DEFAULT_STRIDE, &stride)) ? (LONG)(INT32)stride : frameWidth_ * 4;
        return true;
    }

    bool CopyFrame(IMFSample* sample, std::vector<BYTE>& pixels) {
        Com<IMFMediaBuffer> buffer;
        if (FAILED(sample->ConvertToContiguousBuffer(buffer.Put()))) {
            return false;
        }
        pixels.resize((size_t)width_ * height_ * 4);
        auto copy = [&](const BYTE* firstRow, LONG pitch) {
            for (int y = 0; y < height_; y++) {
                memcpy(pixels.data() + (size_t)y * width_ * 4, firstRow + (LONG_PTR)(top_ + y) * pitch + left_ * 4, (size_t)width_ * 4);
            }
        };
        // Frames on the graphics card come back to memory here.
        Com<IMF2DBuffer> buffer2d;
        BYTE* firstRow = nullptr;
        LONG pitch = 0;
        if (SUCCEEDED(buffer->QueryInterface(IID_PPV_ARGS(buffer2d.Put()))) && SUCCEEDED(buffer2d->Lock2D(&firstRow, &pitch))) {
            copy(firstRow, pitch);
            buffer2d->Unlock2D();
            return true;
        }
        BYTE* data = nullptr;
        DWORD length = 0;
        if (FAILED(buffer->Lock(&data, nullptr, &length))) {
            return false;
        }
        bool fits = (DWORD)(std::abs(stride_) * frameHeight_) <= length;
        if (fits) {
            // A negative stride means the bottom row comes first.
            copy(stride_ < 0 ? data + (size_t)(frameHeight_ - 1) * -stride_ : data, stride_);
        }
        buffer->Unlock();
        return fits;
    }

    Com<IMFDXGIDeviceManager> manager_;  // First: kept until the reader is gone.
    Com<IMFSourceReader> reader_;
    Com<IMFSample> sample_;  // The frame read last.
    LONGLONG duration_ = 0;
    LONGLONG origin_ = 0;  // The first frame's time in the file.
    int frameWidth_ = 0;
    int frameHeight_ = 0;
    LONG stride_ = 0;
    int left_ = 0;
    int top_ = 0;
    int width_ = 0;
    int height_ = 0;
};

// Copies the part of a video from start to end (in ticks) to a new file, starting at 0. Frames
// are re-encoded, so the cut is exact. progress (0 to 1) is called for each frame, and stops the
// copy by returning false.
bool CopyVideoPart(const std::wstring& source, const std::wstring& target, LONGLONG start, LONGLONG end,
                   const std::function<bool(double progress)>& progress) {
    VideoReader reader;
    VideoWriter writer;
    // On the graphics card where there is one: faster, and less memory once its driver is loaded.
    if (!reader.Open(source, {}, true) || !writer.Open(target, reader.Width() & ~1, reader.Height() & ~1) || !reader.Seek(start)) {
        writer.Finish();
        return false;
    }
    std::vector<BYTE> pixels;
    std::vector<BYTE> even;
    LONGLONG time = 0;
    LONGLONG half = kTicksPerSecond / kRecordingFps / 2;
    int width = reader.Width() & ~1;
    int height = reader.Height() & ~1;
    bool wrote = false;
    while (reader.Read(pixels, time)) {
        if (time < start - half) {
            continue;  // Before the start, from the key frame before it.
        }
        if (time >= end - half) {
            break;
        }
        const BYTE* frame = pixels.data();
        if (width != reader.Width() || height != reader.Height()) {
            even.resize((size_t)width * height * 4);
            for (int y = 0; y < height; y++) {
                memcpy(even.data() + (size_t)y * width * 4, pixels.data() + (size_t)y * reader.Width() * 4, (size_t)width * 4);
            }
            frame = even.data();
        }
        if (!writer.Write(frame, std::max<LONGLONG>(0, time - start))) {
            writer.Finish();
            return false;
        }
        wrote = true;
        if (progress && !progress(std::clamp((double)(time - start) / std::max<LONGLONG>(1, end - start), 0.0, 1.0))) {
            writer.Finish();
            return false;
        }
    }
    return writer.Finish() && wrote;
}

// Records into a video file on a thread of its own, from the time it's started until it's
// stopped. The frame source draws each frame, 30 times a second (fewer if the computer can't keep
// up; frames keep their real times, so the video plays at the right speed either way).
class Recorder {
public:
    // Draws a frame of the video's size into the device context.
    using FrameSource = std::function<void(HDC dc, int width, int height)>;

    ~Recorder() { Stop(); }

    // Starts recording. Returns once the file is started, false if it couldn't be.
    bool Start(const std::wstring& path, SIZE size, FrameSource source) {
        Stop();
        path_ = path;
        size_ = size;
        source_ = std::move(source);
        elapsed_ = 0;
        failed_ = false;
        started_ = false;
        stop_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        ready_ = ready;
        thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
        if (thread_) {
            WaitForSingleObject(ready, INFINITE);
        }
        CloseHandle(ready);
        ready_ = nullptr;
        if (!started_) {
            Stop();
        }
        return started_;
    }

    // Stops, and finishes the file. Returns whether it all went well.
    bool Stop() {
        if (!thread_) {
            return false;
        }
        SetEvent(stop_);
        WaitForSingleObject(thread_, INFINITE);
        CloseHandle(thread_);
        CloseHandle(stop_);
        thread_ = nullptr;
        stop_ = nullptr;
        return started_ && !failed_;
    }

    bool Recording() const { return thread_ != nullptr; }
    // How long the recording is so far, in ticks.
    LONGLONG Elapsed() const { return elapsed_; }
    // Something went wrong partway, and it stopped recording.
    bool Failed() const { return failed_; }

private:
    static DWORD WINAPI ThreadProc(void* parameter) {
        ((Recorder*)parameter)->Run();
        return 0;
    }

    void Run() {
        ComThread com;
        MediaFoundation mediaFoundation;
        VideoWriter writer;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = size_.cx;
        info.bmiHeader.biHeight = -size_.cy;  // Top row first.
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HDC screen = GetDC(nullptr);
        HDC dc = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        ReleaseDC(nullptr, screen);
        HGDIOBJ previous = bitmap ? SelectObject(dc, bitmap) : nullptr;
        // A precise timer where Windows has one (Windows 10 1803 and later).
        HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        if (!timer) {
            timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        }
        started_ = mediaFoundation.Started() && bits && timer && writer.Open(path_, size_.cx, size_.cy);
        SetEvent(ready_);
        if (started_) {
            LARGE_INTEGER frequency;
            LARGE_INTEGER origin;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&origin);
            auto now = [&]() {
                LARGE_INTEGER counter;
                QueryPerformanceCounter(&counter);
                return (LONGLONG)((counter.QuadPart - origin.QuadPart) * (double)kTicksPerSecond / frequency.QuadPart);
            };
            const LONGLONG frameTicks = kTicksPerSecond / kRecordingFps;
            while (true) {
                LONGLONG time = now();
                source_(dc, size_.cx, size_.cy);
                GdiFlush();
                if (!writer.Write((const BYTE*)bits, time)) {
                    failed_ = true;
                    break;
                }
                elapsed_ = time + frameTicks;
                // The next frame's turn, skipping any that are already late.
                LONGLONG next = (now() / frameTicks + 1) * frameTicks;
                LARGE_INTEGER due;
                due.QuadPart = -std::max<LONGLONG>(1, next - now());
                SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE);
                HANDLE handles[] = {stop_, timer};
                if (WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
                    break;
                }
            }
            if (!writer.Finish()) {
                failed_ = true;
            }
        }
        if (timer) {
            CloseHandle(timer);
        }
        if (previous) {
            SelectObject(dc, previous);
        }
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(dc);
    }

    std::wstring path_;
    SIZE size_{};
    FrameSource source_;
    HANDLE thread_ = nullptr;
    HANDLE stop_ = nullptr;
    HANDLE ready_ = nullptr;
    std::atomic<LONGLONG> elapsed_{0};
    std::atomic<bool> failed_{false};
    std::atomic<bool> started_{false};
};

// Draws the mouse pointer into a frame of part of the screen, as it is on screen right now.
// scale is the frame's size over the area's.
void DrawCursor(HDC dc, const RECT& area, double scale) {
    CURSORINFO cursor{sizeof(cursor)};
    if (!GetCursorInfo(&cursor) || !(cursor.flags & CURSOR_SHOWING) || !cursor.hCursor) {
        return;
    }
    ICONINFO icon;
    if (!GetIconInfo(cursor.hCursor, &icon)) {
        return;
    }
    // Its size: a black-and-white pointer's mask holds two images, one over the other.
    BITMAP bitmap{};
    int width = 0;
    int height = 0;
    if (icon.hbmColor && GetObjectW(icon.hbmColor, sizeof(bitmap), &bitmap)) {
        width = bitmap.bmWidth;
        height = bitmap.bmHeight;
    } else if (icon.hbmMask && GetObjectW(icon.hbmMask, sizeof(bitmap), &bitmap)) {
        width = bitmap.bmWidth;
        height = bitmap.bmHeight / 2;
    }
    if (icon.hbmMask) {
        DeleteObject(icon.hbmMask);
    }
    if (icon.hbmColor) {
        DeleteObject(icon.hbmColor);
    }
    // Scaled with the frame, when it's smaller than the area.
    bool scaled = scale < 1 && width > 0 && height > 0;
    int x = (int)std::lround((cursor.ptScreenPos.x - area.left - (int)icon.xHotspot) * scale);
    int y = (int)std::lround((cursor.ptScreenPos.y - area.top - (int)icon.yHotspot) * scale);
    DrawIconEx(dc, x, y, cursor.hCursor, scaled ? std::max(1, (int)std::lround(width * scale)) : 0,
               scaled ? std::max(1, (int)std::lround(height * scale)) : 0, 0, nullptr, DI_NORMAL);
}

// The frame source for recording an area of the screen, with the mouse pointer.
Recorder::FrameSource ScreenFrames(const RECT& area) {
    return [area](HDC dc, int width, int height) {
        int areaWidth = area.right - area.left;
        int areaHeight = area.bottom - area.top;
        HDC screen = GetDC(nullptr);
        if (width == areaWidth && height == areaHeight) {
            BitBlt(dc, 0, 0, width, height, screen, area.left, area.top, SRCCOPY);
        } else {
            SetStretchBltMode(dc, HALFTONE);
            SetBrushOrgEx(dc, 0, 0, nullptr);
            StretchBlt(dc, 0, 0, width, height, screen, area.left, area.top, areaWidth, areaHeight, SRCCOPY);
        }
        ReleaseDC(nullptr, screen);
        DrawCursor(dc, area, (double)width / areaWidth);
    };
}

// The folder for recordings being made and trimmed, and copies put on the clipboard: the user's
// own folder in the mod's storage. TidyWorkFolder deals with what's left in it.
std::wstring WorkFolder() {
    return Storage::Folder();
}

// When the mod turns off (or Windows signs out) with a recording that wasn't copied or saved: it
// goes to the recordings folder at once if that's on the same drive (a rename, quick enough to
// finish), taking on that folder's permissions instead of the work folder's. Otherwise it stays for
// TidyWorkFolder at the next start (which never comes if the mod is removed, so the move matters).
bool MoveRecordingNow(const std::wstring& recording, const std::wstring& folder) {
    if (recording.empty() || folder.empty()) {
        return false;
    }
    SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);  // Fails harmlessly when it's there already.
    std::wstring target = NewFileName(folder, L"Screen Recording", L".mp4");
    if (!MoveFileExW(recording.c_str(), target.c_str(), 0)) {
        return false;
    }
    // An empty list of its own, and the folder's inherited.
    ACL empty;
    if (InitializeAcl(&empty, sizeof(empty), ACL_REVISION)) {
        SetNamedSecurityInfoW(target.data(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION, nullptr,
                              nullptr, &empty, nullptr);
    }
    return true;
}

// Tidies the work folder, at start, on a thread of its own:
// - Recordings left in it ("Recording <date> <time>.mp4": the mod turned off with one open in the
//   trim window, or still being made; Windows signed out; or something went wrong) go to the
//   recordings folder. They're copied, so they take on that folder's permissions, then deleted.
//   One that can't be read (cut off while it was being made) is just deleted.
// - Copies made for the clipboard go after a day.
// Setting cancel stops it, without leaving part of a copy.
void TidyWorkFolder(const std::wstring& recordings, BOOL* cancel) {
    std::wstring folder = WorkFolder();
    if (folder.empty()) {
        return;
    }
    ComThread com;
    MediaFoundation mediaFoundation;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULONGLONG cutoff = (((ULONGLONG)now.dwHighDateTime << 32) | now.dwLowDateTime) - 24ULL * 3600 * kTicksPerSecond;
    std::vector<std::wstring> leftovers;
    std::vector<std::wstring> old;
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folder + L"\\*.mp4").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        std::wstring file = folder + L"\\" + found.cFileName;
        ULONGLONG written = ((ULONGLONG)found.ftLastWriteTime.dwHighDateTime << 32) | found.ftLastWriteTime.dwLowDateTime;
        if (wcsncmp(found.cFileName, L"Recording ", 10) == 0) {
            leftovers.push_back(file);
        } else if (written < cutoff) {
            old.push_back(file);
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);
    for (const std::wstring& file : old) {
        DeleteFileW(file.c_str());
    }
    for (const std::wstring& file : leftovers) {
        if (*cancel) {
            return;
        }
        SIZE size;
        if (mediaFoundation.Started() && !VideoReader::Probe(file, size)) {
            DeleteFileW(file.c_str());
            continue;
        }
        if (recordings.empty()) {
            continue;
        }
        SHCreateDirectoryExW(nullptr, recordings.c_str(), nullptr);  // Fails harmlessly when it's there already.
        std::wstring target = NewFileName(recordings, L"Screen Recording", L".mp4");
        if (CopyFileExW(file.c_str(), target.c_str(), nullptr, nullptr, cancel, COPY_FILE_FAIL_IF_EXISTS)) {
            DeleteFileW(file.c_str());
        }
    }
}

// Puts a file on the clipboard, as File Explorer's Copy does, to paste into a chat, an email or a
// folder.
bool CopyFileToClipboard(HWND owner, const std::wstring& path) {
    size_t size = sizeof(DROPFILES) + (path.size() + 2) * sizeof(wchar_t);
    HGLOBAL files = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
    if (!files) {
        return false;
    }
    auto drop = (DROPFILES*)GlobalLock(files);
    drop->pFiles = sizeof(DROPFILES);
    drop->fWide = TRUE;
    memcpy((BYTE*)drop + sizeof(DROPFILES), path.c_str(), path.size() * sizeof(wchar_t));  // Ends with two nulls.
    GlobalUnlock(files);
    DWORD effect = DROPEFFECT_COPY;
    HGLOBAL dropEffect = GlobalCopy(&effect, sizeof(effect));
    bool opened = false;
    for (int attempt = 0; attempt < 10 && !(opened = OpenClipboard(owner)); attempt++) {
        Sleep(20);
    }
    if (!opened) {
        GlobalFree(files);
        if (dropEffect) {
            GlobalFree(dropEffect);
        }
        return false;
    }
    EmptyClipboard();
    bool ok = SetClipboardData(CF_HDROP, files) != nullptr;
    if (!ok) {
        GlobalFree(files);
    }
    if (dropEffect && !SetClipboardData(RegisterClipboardFormatW(CFSTR_PREFERREDDROPEFFECT), dropEffect)) {
        GlobalFree(dropEffect);
    }
    CloseClipboard();
    return ok;
}

// Lets the user pick a folder. Returns it, or nothing if they cancelled.
std::optional<std::wstring> PickFolder(HWND owner, const wchar_t* title, const std::wstring& current) {
    Com<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(dialog.Put())))) {
        return std::nullopt;
    }
    FILEOPENDIALOGOPTIONS options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
    dialog->SetTitle(title);
    dialog->SetOkButtonLabel(L"Use this folder");
    SHCreateDirectoryExW(nullptr, current.c_str(), nullptr);  // To start in it.
    Com<IShellItem> start;
    if (SUCCEEDED(SHCreateItemFromParsingName(current.c_str(), nullptr, IID_PPV_ARGS(start.Put())))) {
        dialog->SetFolder(start.Get());
    }
    Com<IShellItem> chosen;
    PWSTR path = nullptr;
    if (FAILED(dialog->Show(owner)) || FAILED(dialog->GetResult(chosen.Put())) ||
        FAILED(chosen->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        return std::nullopt;
    }
    std::wstring folder = path;
    CoTaskMemFree(path);
    return folder;
}

// ---------------------------------------------------------------------------
// The note in the corner of the screen after a screenshot: a thumbnail, what happened, and where
// it was saved. Clicking it shows a saved screenshot in File Explorer.

class Notice {
public:
    ~Notice() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool Create() {
        if (const wchar_t* registered = RegisterForwardingClass<Notice>(L"TaskbarSnipper.Mod.Notice", IDC_HAND)) {
            CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, registered, L"", WS_POPUP, 0, 0, 1, 1, nullptr,
                            nullptr, ModuleInstance(), this);
        }
        if (!hwnd_) {
            return false;
        }
        DWORD corners = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
        return true;
    }

    // Shows the note on the display of the screen rectangle, with a thumbnail of the image if given.
    void Show(const std::wstring& title, const std::wstring& detail, const std::wstring& savedPath, gp::Bitmap* image,
              const RECT& nearRect) {
        title_ = title;
        detail_ = detail;
        savedPath_ = savedPath;
        thumbnail_.reset();
        scale_ = ScaleForScreenRect(nearRect);
        if (image && image->GetWidth() > 0 && image->GetHeight() > 0) {
            float fit = std::min(Px(88) / (float)image->GetWidth(), Px(56) / (float)image->GetHeight());
            int width = std::max(1, (int)(image->GetWidth() * std::min(fit, 1.0f)));
            int height = std::max(1, (int)(image->GetHeight() * std::min(fit, 1.0f)));
            thumbnail_ = std::make_unique<gp::Bitmap>(width, height, PixelFormat32bppARGB);
            gp::Graphics graphics(thumbnail_.get());
            graphics.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
            graphics.DrawImage(image, 0, 0, width, height);
        }
        RECT work = WorkAreaForScreenRect(nearRect);
        int width = Px(380);
        int height = Px(80);
        SetWindowPos(hwnd_, HWND_TOPMOST, work.right - width - Px(12), work.bottom - height - Px(12), width, height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        InvalidateRect(hwnd_, nullptr, FALSE);
        SetTimer(hwnd_, 1, 5000, nullptr);
    }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        switch (message) {
            case WM_MOUSEACTIVATE:
                result = MA_NOACTIVATE;
                return true;
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_TIMER:
                KillTimer(hwnd_, 1);
                ShowWindow(hwnd_, SW_HIDE);
                return true;
            case WM_LBUTTONUP:
                KillTimer(hwnd_, 1);
                ShowWindow(hwnd_, SW_HIDE);
                if (!savedPath_.empty()) {
                    ShowInExplorer(savedPath_);
                }
                return true;
            case WM_PAINT:
                Paint();
                return true;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

private:
    int Px(float value) const { return (int)std::lround(value * scale_); }

    void Paint() {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(hwnd_, &paint);
        RECT client;
        GetClientRect(hwnd_, &client);
        HDC memory = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, client.right, client.bottom);
        HGDIOBJ previous = SelectObject(memory, bitmap);
        {
            Palette colors = Palette::Current();
            gp::Graphics graphics(memory);
            graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(gp::TextRenderingHintClearTypeGridFit);
            graphics.Clear(WithAlpha(255, colors.background));
            float textLeft = (float)Px(16);
            if (thumbnail_) {
                gp::RectF box((float)Px(12), (float)Px(12), (float)Px(88), (float)Px(56));
                float x = box.X + (box.Width - thumbnail_->GetWidth()) / 2;
                float y = box.Y + (box.Height - thumbnail_->GetHeight()) / 2;
                graphics.DrawImage(thumbnail_.get(), x, y);
                gp::Pen pen(colors.border, 1);
                graphics.DrawRectangle(&pen, x - 0.5f, y - 0.5f, (float)thumbnail_->GetWidth(), (float)thumbnail_->GetHeight());
                textLeft = box.GetRight() + Px(14);
            }
            gp::Font titleFont(L"Segoe UI Semibold", 14.0f * scale_, gp::FontStyleRegular, gp::UnitPixel);
            gp::Font detailFont(L"Segoe UI", 12.0f * scale_, gp::FontStyleRegular, gp::UnitPixel);
            float textWidth = client.right - textLeft - Px(14);
            DrawTextLine(graphics, title_, titleFont, colors.foreground, gp::RectF(textLeft, (float)Px(16), textWidth, (float)Px(22)));
            DrawTextLine(graphics, detail_, detailFont, colors.secondary, gp::RectF(textLeft, (float)Px(40), textWidth, (float)Px(22)));
            gp::Pen border(colors.border, 1);
            graphics.DrawRectangle(&border, 0.5f, 0.5f, client.right - 1.0f, client.bottom - 1.0f);
        }
        BitBlt(dc, 0, 0, client.right, client.bottom, memory, 0, 0, SRCCOPY);
        SelectObject(memory, previous);
        DeleteObject(bitmap);
        DeleteDC(memory);
        EndPaint(hwnd_, &paint);
    }

    std::wstring title_;
    std::wstring detail_;
    std::wstring savedPath_;
    std::unique_ptr<gp::Bitmap> thumbnail_;
    float scale_ = 1;
};

// ---------------------------------------------------------------------------
// The overlay: the frozen screen over every display, dimmed except for the selection, with the
// toolbox above the selection once it's made.

// What finishing does with the screenshot; or for a recording, start it.
enum class Finish { Default, Copy, Save, Record };

// The tool, color and thickness, remembered between screenshots.
struct Pen {
    int tool = ToolForKey(kDefaultToolKey);  // In kTools.
    ARGB color = 0xFFE81123;
    int thickness = 4;  // 1 to 20, in pixels at 100% scaling.
};

const ARGB kSwatches[] = {0xFFE81123, 0xFFFF8C00, 0xFFFFD60A, 0xFF16C60C, 0xFF0078D4, 0xFF8E44EC, 0xFFFFFFFF, 0xFF000000};
const wchar_t* const kSwatchNames[] = {L"Red", L"Orange", L"Yellow", L"Green", L"Blue", L"Purple", L"White", L"Black"};
constexpr int kSwatchCount = ARRAYSIZE(kSwatches);

class Overlay {
public:
    // Called when the overlay is done: with what to do with the screenshot, or nothing when it was
    // cancelled. The overlay is hidden by then and can be deleted after the call returns.
    std::function<void(std::optional<Finish> finish)> onDone;
    // Called when the tool, color or thickness changes.
    std::function<void(const Pen& pen)> onPenChanged;

    // forRecording selects an area to record: no marks, and the toolbox just starts recording.
    Overlay(std::unique_ptr<ScreenCopy> screen, std::vector<RECT> windows, const Pen& pen, bool forRecording)
        : screen_(std::move(screen)), pen_(pen), forRecording_(forRecording) {
        if (pen_.tool < 0 || pen_.tool >= kToolCount) {
            pen_.tool = ToolForKey(kDefaultToolKey);
        }
        // The windows' outlines, in the overlay's coordinates (the top-left of the desktop is 0, 0).
        for (const RECT& window : windows) {
            windows_.push_back(ToClient(window));
        }
    }

    ~Overlay() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        if (black_) {
            SelectObject(blackDc_, blackPrevious_);
            DeleteObject(black_);
            DeleteDC(blackDc_);
        }
    }

    bool Create() {
        const RECT& bounds = screen_->Bounds();
        if (const wchar_t* registered = RegisterForwardingClass<Overlay>(L"TaskbarSnipper.Mod.Overlay", IDC_CROSS)) {
            CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, registered, L"Taskbar Snipper", WS_POPUP, bounds.left, bounds.top,
                            screen_->Width(), screen_->Height(), nullptr, nullptr, ModuleInstance(), this);
        }
        if (!hwnd_) {
            return false;
        }
        // No fade: the screen should look frozen the moment Print Screen is pressed.
        BOOL disable = TRUE;
        DwmSetWindowAttribute(hwnd_, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
        // A 1×1 black bitmap, stretched with transparency to dim the screen outside the selection.
        HDC screen = GetDC(nullptr);
        blackDc_ = CreateCompatibleDC(screen);
        black_ = CreateCompatibleBitmap(screen, 1, 1);
        ReleaseDC(nullptr, screen);
        blackPrevious_ = SelectObject(blackDc_, black_);
        SetPixel(blackDc_, 0, 0, RGB(0, 0, 0));
        lightTheme_ = AppsUseLightTheme();
        POINT cursor;
        GetCursorPos(&cursor);
        UpdateHover(ToClient(cursor));
        ShowWindow(hwnd_, SW_SHOW);
        // The key that opened the overlay went to the app in front (the hook took it), so Windows
        // would keep the overlay from coming to the front: input from this process first lets it.
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));
        SetForegroundWindow(hwnd_);
        return true;
    }

    // Esc: finishes the text being typed, stops a drawing or a move, or deselects the selected
    // mark, or else cancels.
    void Escape() {
        if (typing_) {
            FinishTyping();
        } else if (drawing_) {
            CancelDrawing();
        } else if (moving_ >= 0) {
            EndMove();
        } else if (selected_ >= 0) {
            SelectMark(-1);
        } else {
            Done(std::nullopt);
        }
    }

    // The selection and marks, for making the screenshot once the overlay is done.
    const ScreenCopy& Screen() const { return *screen_; }
    RECT SelectionOnScreen() const { return ToScreen(selection_); }
    RECT SelectionInOverlay() const { return selection_; }
    const std::vector<std::unique_ptr<Mark>>& Marks() const { return marks_; }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        switch (message) {
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_PAINT: {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hwnd_, &paint);
                if (!IsRectEmpty(&paint.rcPaint)) {
                    Paint(dc, paint.rcPaint);
                }
                EndPaint(hwnd_, &paint);
                return true;
            }
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    POINT cursor;
                    GetCursorPos(&cursor);
                    SetCursor(LoadCursorW(nullptr, CursorAt(ToClient(cursor))));
                    result = TRUE;
                    return true;
                }
                return false;
            case WM_MOUSEMOVE:
                OnMouseMove({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_LBUTTONDOWN:
                SetCapture(hwnd_);
                OnMouseDown({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_LBUTTONUP:
                OnMouseUp({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            case WM_RBUTTONUP:
                // Cancels while selecting; once there are marks to lose, only Esc or Close does.
                if (mode_ == Mode::Selecting) {
                    Done(std::nullopt);
                }
                return true;
            case WM_MOUSEWHEEL:
                if (mode_ == Mode::Editing && !forRecording_) {
                    wheelRemainder_ += GET_WHEEL_DELTA_WPARAM(wParam);
                    int notches = wheelRemainder_ / WHEEL_DELTA;
                    wheelRemainder_ -= notches * WHEEL_DELTA;
                    if (notches != 0) {
                        SetThickness(pen_.thickness + notches);
                    }
                }
                return true;
            case WM_KEYDOWN:
                OnKeyDown((UINT)wParam);
                return true;
            case WM_CHAR:
                OnChar((wchar_t)wParam);
                return true;
            case WM_TIMER:
                if (wParam == kCaretTimer && typing_) {
                    caretOn_ = !caretOn_;
                    InvalidateClient(marks_[selected_]->Bounds());
                }
                return true;
            case WM_DISPLAYCHANGE:
                // The displays changed under the frozen picture of them.
                Done(std::nullopt);
                return true;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

private:
    enum class Mode { Selecting, Editing };

    enum class Item {
        None,
        Tool,    // index says which, in kTools.
        Swatch,  // index says which.
        CustomColor,
        Thinner,
        Thickness,
        Thicker,
        Undo,
        Copy,
        Save,
        Close,
        Record,
    };

    struct ToolboxItem {
        Item item;
        int index;  // For tools and swatches.
        RECT rect;  // In the overlay's coordinates.
    };

    // A run of the same change to the same mark (turning the mouse wheel, pressing an arrow key
    // again and again) is undone in one step.
    enum class Change { Other, Restyle, Nudge };

    using MarkList = std::vector<std::unique_ptr<Mark>>;

    static constexpr UINT_PTR kCaretTimer = 1;

    // Coordinates: the overlay covers the whole desktop, with its own 0, 0 at the desktop's top-left.
    POINT ToClient(POINT point) const { return {point.x - screen_->Bounds().left, point.y - screen_->Bounds().top}; }
    RECT ToClient(const RECT& rect) const { return OffsetRectBy(rect, -screen_->Bounds().left, -screen_->Bounds().top); }
    RECT ToScreen(const RECT& rect) const { return OffsetRectBy(rect, screen_->Bounds().left, screen_->Bounds().top); }

    float Scale() const { return scale_; }
    int Px(float value) const { return (int)std::lround(value * scale_); }

    void InvalidateClient(const RECT& rect) { InvalidateRect(hwnd_, &rect, FALSE); }

    void ReleaseMouse() {
        if (GetCapture() == hwnd_) {
            ReleaseCapture();
        }
    }

    // The part of the screen shown bright: the selection, or while choosing, the window under the
    // mouse or the area being dragged.
    RECT Highlight() const {
        if (mode_ == Mode::Editing || dragging_) {
            return selection_;
        }
        return hover_;
    }

    void SetHighlightChanged(const RECT& before) {
        RECT after = Highlight();
        if (!EqualRect(&before, &after)) {
            // The glow, size label and corner dimming change around both, more so on a scaled display.
            float scale = std::max(ScaleForScreenRect(ToScreen(before)), ScaleForScreenRect(ToScreen(after)));
            int margin = (int)std::ceil(60 * scale);
            InvalidateClient(InflatedRect(before, margin));
            InvalidateClient(InflatedRect(after, margin));
        }
    }

    // The window (or else the display) under the point, for choosing by clicking.
    void UpdateHover(POINT point) {
        RECT before = Highlight();
        hover_ = {};
        for (const RECT& window : windows_) {
            if (PtInRect(&window, point)) {
                hover_ = window;
                break;
            }
        }
        if (IsRectEmpty(&hover_)) {
            POINT onScreen{point.x + screen_->Bounds().left, point.y + screen_->Bounds().top};
            MONITORINFO info{sizeof(info)};
            if (GetMonitorInfoW(MonitorFromPoint(onScreen, MONITOR_DEFAULTTONEAREST), &info)) {
                hover_ = ToClient(info.rcMonitor);
            }
        }
        RECT all{0, 0, screen_->Width(), screen_->Height()};
        IntersectRect(&hover_, &hover_, &all);
        // The hint follows the mouse from display to display.
        RECT hint = HintRect(point);
        if (!EqualRect(&hint, &hintRect_)) {
            InvalidateClient(hintRect_);
            hintRect_ = hint;
            InvalidateClient(hintRect_);
        }
        SetHighlightChanged(before);
    }

    // Where the "drag to select" hint goes: the top middle of the display the point is on.
    RECT HintRect(POINT point) const {
        POINT onScreen{point.x + screen_->Bounds().left, point.y + screen_->Bounds().top};
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(MonitorFromPoint(onScreen, MONITOR_DEFAULTTONEAREST), &info);
        RECT monitor = ToClient(info.rcMonitor);
        float scale = ScaleForScreenRect(info.rcMonitor);
        int width = (int)(540 * scale);
        int height = (int)(40 * scale);
        int left = (monitor.left + monitor.right - width) / 2;
        int top = monitor.top + (int)(24 * scale);
        return {left, top, left + width, top + height};
    }

    // The point, kept inside the selection: marks are drawn there.
    gp::PointF InSelection(POINT point) const {
        return gp::PointF((float)std::clamp(point.x, selection_.left, selection_.right - 1),
                          (float)std::clamp(point.y, selection_.top, selection_.bottom - 1));
    }

    // The topmost mark under the point, or -1.
    int MarkAt(POINT point) const {
        if (mode_ != Mode::Editing || !PtInRect(&selection_, point)) {
            return -1;
        }
        gp::PointF at((float)point.x, (float)point.y);
        for (int i = (int)marks_.size() - 1; i >= 0; i--) {
            if (marks_[i]->Hit(at)) {
                return i;
            }
        }
        return -1;
    }

    void OnMouseMove(POINT point) {
        if (mode_ == Mode::Selecting) {
            if (!dragging_) {
                UpdateHover(point);
                return;
            }
            RECT before = Highlight();
            selection_ = {std::min(anchor_.x, point.x), std::min(anchor_.y, point.y), std::max(anchor_.x, point.x) + 1,
                          std::max(anchor_.y, point.y) + 1};
            SetHighlightChanged(before);
            return;
        }
        if (drawing_) {
            RECT before = drawing_->Bounds();
            drawing_->DragTo(InSelection(point), (GetKeyState(VK_SHIFT) & 0x8000) != 0);
            InvalidateClient(before);
            InvalidateClient(drawing_->Bounds());
            return;
        }
        if (moving_ >= 0) {
            MoveTo(point);
            return;
        }
        ToolboxItem item = ItemAt(point);
        SetHoverItem(item);
        SetHoverMark(item.item == Item::None ? MarkAt(point) : -1);
    }

    void OnMouseDown(POINT point) {
        if (mode_ == Mode::Selecting) {
            dragging_ = true;
            anchor_ = point;
            return;
        }
        pressedItem_ = ItemAt(point);
        pressedOutside_ = false;
        if (pressedItem_.item != Item::None) {
            return;
        }
        if (forRecording_) {
            // Selects again.
            mode_ = Mode::Selecting;
            hoverItem_ = {Item::None, 0, {}};
            UpdateHover(point);
            dragging_ = true;
            anchor_ = point;
            selection_ = {point.x, point.y, point.x + 1, point.y + 1};
            InvalidateRect(hwnd_, nullptr, FALSE);
            return;
        }
        if (!PtInRect(&selection_, point)) {
            pressedOutside_ = true;
            return;
        }
        // Any tool moves a mark it's pressed on.
        int hit = MarkAt(point);
        if (typing_ && hit != selected_) {
            FinishTyping();  // May take away empty text, so look again.
            hit = MarkAt(point);
        }
        if (hit >= 0) {
            pressedSelected_ = hit == selected_;
            SelectMark(hit);
            SetHoverMark(-1);
            moving_ = hit;
            movePressed_ = point;
            moveLast_ = InSelection(point);
            moved_ = false;
            return;
        }
        SelectMark(-1);
        const Tool& tool = kTools[pen_.tool];
        if (!tool.makeMark) {
            ReleaseMouse();
            return;
        }
        std::unique_ptr<Mark> mark = tool.makeMark();
        mark->color = pen_.color;
        mark->thickness = pen_.thickness;
        mark->scale = Scale();
        mark->StartAt(InSelection(point));
        SetHoverMark(-1);
        if (tool.typed) {
            PushHistory();
            marks_.push_back(std::move(mark));
            SelectMark((int)marks_.size() - 1);
            StartTyping(true);
            ReleaseMouse();
            return;
        }
        drawing_ = std::move(mark);
        InvalidateClient(drawing_->Bounds());
    }

    void OnMouseUp(POINT point) {
        ReleaseMouse();
        if (mode_ == Mode::Selecting) {
            if (!dragging_) {
                return;
            }
            dragging_ = false;
            // A click (no drag) picks the window or display under the mouse.
            if (std::abs(point.x - anchor_.x) < GetSystemMetrics(SM_CXDRAG) &&
                std::abs(point.y - anchor_.y) < GetSystemMetrics(SM_CYDRAG)) {
                selection_ = hover_;
            }
            int smallest = forRecording_ ? 32 : 2;
            if (selection_.right - selection_.left < smallest || selection_.bottom - selection_.top < smallest) {
                selection_ = {};
                InvalidateRect(hwnd_, nullptr, FALSE);
                return;
            }
            StartEditing();
            return;
        }
        if (drawing_) {
            std::unique_ptr<Mark> mark = std::move(drawing_);
            InvalidateClient(mark->Bounds());
            if (mark->Worthwhile()) {
                PushHistory();
                marks_.push_back(std::move(mark));
            }
            return;
        }
        if (moving_ >= 0) {
            int index = moving_;
            bool moved = moved_;
            EndMove();
            // A click on text, with the text tool or once it's selected, types into it.
            if (!moved && !typing_ && marks_[index]->Text() && (kTools[pen_.tool].typed || pressedSelected_)) {
                StartTyping(false);
            }
            SetHoverMark(MarkAt(point));
            return;
        }
        Item released = ItemAt(point).item;
        if (pressedItem_.item != Item::None && released == pressedItem_.item) {
            Activate(pressedItem_);
        } else if (pressedOutside_ && !PtInRect(&selection_, point) && released == Item::None) {
            Done(Finish::Default);
        }
        pressedItem_ = {Item::None, 0, {}};
        pressedOutside_ = false;
    }

    void OnKeyDown(UINT key) {
        bool control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        switch (key) {
            case VK_ESCAPE:
                Escape();
                return;
            case VK_RETURN:
                if (mode_ == Mode::Editing && (!typing_ || control)) {
                    Done(forRecording_ ? Finish::Record : Finish::Default);
                }
                return;
        }
        if (mode_ != Mode::Editing || forRecording_) {
            return;
        }
        if (control) {
            switch (key) {
                case 'Z':
                    Undo();
                    return;
                case 'C':
                    Done(Finish::Copy);
                    return;
                case 'S':
                    Done(Finish::Save);
                    return;
            }
            return;
        }
        if (typing_) {
            return;  // Letters are typed, through WM_CHAR.
        }
        int step = shift ? 10 : 1;
        switch (key) {
            case VK_BACK:
            case VK_DELETE:
                if (!DeleteSelected()) {
                    Undo();
                }
                return;
            case VK_LEFT:
                Nudge(-step, 0);
                return;
            case VK_RIGHT:
                Nudge(step, 0);
                return;
            case VK_UP:
                Nudge(0, -step);
                return;
            case VK_DOWN:
                Nudge(0, step);
                return;
        }
        int tool = ToolForKey((wchar_t)key);
        if (tool >= 0) {
            SetTool(tool);
        }
    }

    void OnChar(wchar_t c) {
        if (!typing_) {
            return;
        }
        Mark& mark = *marks_[selected_];
        std::wstring& text = *mark.Text();
        RECT before = mark.Bounds();
        if (c == L'\b') {
            if (!text.empty()) {
                text.pop_back();
            }
        } else if (c == L'\r') {
            text += L'\n';
        } else if (c >= 32 && c != 127) {
            text += c;
        } else {
            return;
        }
        typingChanged_ = true;
        caretOn_ = true;
        SetTimer(hwnd_, kCaretTimer, 530, nullptr);  // Restarts the blink, so the caret shows while typing.
        InvalidateClient(before);
        InvalidateClient(mark.Bounds());
    }

    // ---- Marks

    MarkList CopyOfMarks() const {
        MarkList copy;
        for (const std::unique_ptr<Mark>& mark : marks_) {
            copy.push_back(mark->Clone());
        }
        return copy;
    }

    // Remembers the marks as they are, before a change, for undo.
    void PushHistory(Change change = Change::Other, int mark = -1) {
        if (change != Change::Other && change == lastChange_ && mark == lastChangeMark_ && !history_.empty()) {
            return;
        }
        history_.push_back(CopyOfMarks());
        lastChange_ = change;
        lastChangeMark_ = mark;
        InvalidateToolbox();  // Undo is available.
    }

    // Before changing a mark. Text is undone as a whole, back to before it was typed into.
    void BeforeChange(Change change, int mark) {
        if (typing_) {
            typingChanged_ = true;
        } else {
            PushHistory(change, mark);
        }
    }

    // Selects the mark, or none with -1. The toolbox takes on its color and thickness, and
    // changing them changes the mark.
    void SelectMark(int index) {
        if (index == selected_) {
            return;
        }
        if (selected_ >= 0) {
            InvalidateClient(marks_[selected_]->Bounds());
        }
        selected_ = index;
        lastChange_ = Change::Other;
        if (index >= 0) {
            const Mark& mark = *marks_[index];
            InvalidateClient(mark.Bounds());
            if (pen_.color != mark.color || pen_.thickness != mark.thickness) {
                pen_.color = mark.color;
                pen_.thickness = mark.thickness;
                PenChanged();
            }
        }
        InvalidateToolbox();
    }

    // The mark under the mouse gets a faint outline, to show it can be moved.
    void SetHoverMark(int index) {
        if (index == hoverMark_) {
            return;
        }
        if (hoverMark_ >= 0) {
            InvalidateClient(marks_[hoverMark_]->Bounds());
        }
        hoverMark_ = index;
        if (hoverMark_ >= 0) {
            InvalidateClient(marks_[hoverMark_]->Bounds());
        }
    }

    // Moves the mark being dragged with the mouse, once the mouse has moved far enough to be a drag
    // rather than a click.
    void MoveTo(POINT point) {
        if (!moved_) {
            if (std::abs(point.x - movePressed_.x) < GetSystemMetrics(SM_CXDRAG) &&
                std::abs(point.y - movePressed_.y) < GetSystemMetrics(SM_CYDRAG)) {
                return;
            }
            moved_ = true;
            BeforeChange(Change::Other, moving_);
        }
        Mark& mark = *marks_[moving_];
        gp::PointF to = InSelection(point);  // The part held stays in the selection.
        RECT before = mark.Bounds();
        mark.MoveBy(to.X - moveLast_.X, to.Y - moveLast_.Y);
        moveLast_ = to;
        InvalidateClient(before);
        InvalidateClient(mark.Bounds());
    }

    void EndMove() {
        if (moving_ >= 0) {
            moving_ = -1;
            ReleaseMouse();
            InvalidateToolbox();
        }
    }

    void CancelDrawing() {
        if (drawing_) {
            InvalidateClient(drawing_->Bounds());
            drawing_.reset();
            ReleaseMouse();
        }
    }

    // Moves the selected mark by whole pixels (arrow keys).
    void Nudge(int dx, int dy) {
        if (selected_ < 0 || drawing_ || moving_ >= 0) {
            return;
        }
        Mark& mark = *marks_[selected_];
        BeforeChange(Change::Nudge, selected_);
        RECT before = mark.Bounds();
        mark.MoveBy((float)dx, (float)dy);
        InvalidateClient(before);
        InvalidateClient(mark.Bounds());
    }

    // Deletes the selected mark, if there is one.
    bool DeleteSelected() {
        if (selected_ < 0) {
            return false;
        }
        EndMove();
        PushHistory();
        InvalidateClient(marks_[selected_]->Bounds());
        marks_.erase(marks_.begin() + selected_);
        selected_ = -1;
        hoverMark_ = -1;
        InvalidateToolbox();
        return true;
    }

    void StartTyping(bool isNew) {
        if (!isNew) {
            PushHistory();
        }
        typing_ = true;
        typingNew_ = isNew;
        typingChanged_ = false;
        caretOn_ = true;
        SetTimer(hwnd_, kCaretTimer, 530, nullptr);
        InvalidateClient(marks_[selected_]->Bounds());
        InvalidateToolbox();
    }

    // Stops typing, and deselects the text. Text left empty goes.
    void FinishTyping() {
        if (!typing_) {
            return;
        }
        EndMove();  // The text may go.
        typing_ = false;
        KillTimer(hwnd_, kCaretTimer);
        int index = selected_;
        InvalidateClient(marks_[index]->Bounds());
        selected_ = -1;
        lastChange_ = Change::Other;
        if (!marks_[index]->Worthwhile()) {
            marks_.erase(marks_.begin() + index);
            hoverMark_ = -1;
            if (typingNew_) {
                history_.pop_back();  // As if it was never there.
            }
        } else if (!typingNew_ && !typingChanged_) {
            history_.pop_back();  // Nothing changed.
        }
        InvalidateToolbox();
    }

    void Undo() {
        FinishTyping();
        CancelDrawing();
        EndMove();
        if (history_.empty()) {
            return;
        }
        marks_ = std::move(history_.back());
        history_.pop_back();
        selected_ = -1;
        hoverMark_ = -1;
        lastChange_ = Change::Other;
        InvalidateClient(selection_);  // Marks are only drawn there.
        InvalidateToolbox();
    }

    void SetTool(int tool) {
        FinishTyping();
        if (kTools[tool].makeMark) {
            SelectMark(-1);  // A drawing tool is for new marks.
        }
        pen_.tool = tool;
        InvalidateToolbox();
        PenChanged();
    }

    void SetColor(ARGB color) {
        pen_.color = color;
        if (selected_ >= 0 && marks_[selected_]->color != color) {
            Mark& mark = *marks_[selected_];
            BeforeChange(Change::Restyle, selected_);
            mark.color = color;
            InvalidateClient(mark.Bounds());
        }
        InvalidateToolbox();
        PenChanged();
    }

    void SetThickness(int thickness) {
        thickness = std::clamp(thickness, kMinThickness, kMaxThickness);
        if (thickness == pen_.thickness) {
            return;
        }
        pen_.thickness = thickness;
        if (selected_ >= 0 && marks_[selected_]->thickness != thickness) {
            Mark& mark = *marks_[selected_];
            BeforeChange(Change::Restyle, selected_);
            RECT before = mark.Bounds();
            mark.thickness = thickness;
            InvalidateClient(before);
            InvalidateClient(mark.Bounds());
        }
        InvalidateToolbox();
        PenChanged();
    }

    void PenChanged() {
        if (onPenChanged) {
            onPenChanged(pen_);
        }
    }

    // Lets Windows' color dialog pick any color. The dialog belongs to the overlay's window, so the
    // overlay can't finish while it's open (it would be deleted under the dialog): finishing then
    // closes the dialog, and waits for it.
    void PickColor() {
        static COLORREF customColors[16] = {};
        gp::Color current(pen_.color);
        CHOOSECOLORW choose{sizeof(choose)};
        choose.hwndOwner = hwnd_;
        choose.rgbResult = RGB(current.GetR(), current.GetG(), current.GetB());
        choose.lpCustColors = customColors;
        choose.Flags = CC_FULLOPEN | CC_RGBINIT;
        inColorDialog_ = true;
        BOOL picked = ChooseColorW(&choose);
        inColorDialog_ = false;
        if (finishAfterDialog_) {
            finishAfterDialog_ = false;
            Done(finishWhenDialogCloses_);
            return;
        }
        if (picked) {
            SetColor(gp::Color::MakeARGB(255, GetRValue(choose.rgbResult), GetGValue(choose.rgbResult), GetBValue(choose.rgbResult)));
        }
    }

    void Activate(const ToolboxItem& item) {
        switch (item.item) {
            case Item::Tool:
                SetTool(item.index);
                break;
            case Item::Swatch:
                SetColor(kSwatches[item.index]);
                break;
            case Item::CustomColor:
                PickColor();
                break;
            case Item::Thinner:
                SetThickness(pen_.thickness - 1);
                break;
            case Item::Thicker:
                SetThickness(pen_.thickness + 1);
                break;
            case Item::Undo:
                Undo();
                break;
            case Item::Copy:
                Done(Finish::Copy);
                break;
            case Item::Save:
                Done(Finish::Save);
                break;
            case Item::Close:
                Done(std::nullopt);
                break;
            case Item::Record:
                Done(Finish::Record);
                break;
            case Item::Thickness:
            case Item::None:
                break;
        }
    }

    void Done(std::optional<Finish> finish) {
        if (done_) {
            return;
        }
        if (inColorDialog_) {
            finishAfterDialog_ = true;
            finishWhenDialogCloses_ = finish;
            HWND dialog = GetLastActivePopup(hwnd_);
            if (dialog && dialog != hwnd_) {
                PostMessageW(dialog, WM_COMMAND, IDCANCEL, 0);
            }
            return;
        }
        FinishTyping();
        CancelDrawing();
        EndMove();
        done_ = true;
        ReleaseMouse();
        KillTimer(hwnd_, kCaretTimer);
        ShowWindow(hwnd_, SW_HIDE);
        if (onDone) {
            onDone(finish);
        }
    }

    // ---- The toolbox

    void StartEditing() {
        mode_ = Mode::Editing;
        scale_ = ScaleForScreenRect(ToScreen(selection_));
        LayOutToolbox();
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    // The toolbox goes centered above the selection; below it when there's no room above, or else
    // inside its top edge. It stays on the selection's display.
    void LayOutToolbox() {
        toolbox_.clear();
        separators_.clear();
        int padding = Px(6);
        int button = Px(36);
        int swatch = Px(30);
        int separator = Px(17);
        struct Slot {
            Item item;
            int index;
            int width;
        };
        std::vector<Slot> slots;
        auto add = [&](Item item, int width, int index = 0) { slots.push_back({item, index, width}); };
        if (forRecording_) {
            add(Item::Record, Px(150));
            add(Item::Close, button);
        } else {
            for (int i = 0; i < kToolCount; i++) {
                add(Item::Tool, button, i);
            }
            add(Item::None, separator);
            for (int i = 0; i < kSwatchCount; i++) {
                add(Item::Swatch, swatch, i);
            }
            add(Item::CustomColor, swatch);
            add(Item::None, separator);
            add(Item::Thinner, button);
            add(Item::Thickness, Px(52));
            add(Item::Thicker, button);
            add(Item::None, separator);
            add(Item::Undo, button);
            add(Item::Copy, button);
            add(Item::Save, button);
            add(Item::Close, button);
        }
        int width = padding * 2;
        for (const Slot& slot : slots) {
            width += slot.width;
        }
        int height = padding * 2 + button;

        RECT work = ToClient(WorkAreaForScreenRect(ToScreen(selection_)));
        int gap = Px(10);
        int left = (selection_.left + selection_.right - width) / 2;
        left = std::max((int)work.left + gap, std::min(left, (int)work.right - width - gap));
        int top = selection_.top - gap - height;
        toolboxAbove_ = true;
        if (top < work.top + gap) {
            toolboxAbove_ = false;
            top = selection_.bottom + gap;
            if (top + height > work.bottom - gap) {
                top = std::max((int)work.top + gap, (int)selection_.top + gap);
            }
        }
        toolboxRect_ = {left, top, left + width, top + height};

        int x = left + padding;
        for (const Slot& slot : slots) {
            if (slot.item != Item::None) {
                toolbox_.push_back({slot.item, slot.index, {x, top + padding, x + slot.width, top + padding + button}});
            } else {
                separators_.push_back(x + slot.width / 2);
            }
            x += slot.width;
        }
    }

    ToolboxItem ItemAt(POINT point) const {
        if (mode_ == Mode::Editing) {
            for (const ToolboxItem& item : toolbox_) {
                if (PtInRect(&item.rect, point)) {
                    return item;
                }
            }
        }
        return {Item::None, 0, {}};
    }

    void SetHoverItem(const ToolboxItem& item) {
        if (item.item == hoverItem_.item && item.index == hoverItem_.index) {
            return;
        }
        hoverItem_ = item;
        InvalidateToolbox();
    }

    // The toolbox and its tooltip.
    void InvalidateToolbox() {
        if (mode_ == Mode::Editing) {
            InvalidateClient(InflatedRect(toolboxRect_, Px(48)));
        }
    }

    std::wstring TooltipFor(const ToolboxItem& item) const {
        switch (item.item) {
            case Item::Tool:
                return std::wstring(kTools[item.index].name) + L" (" + kTools[item.index].key + L")";
            case Item::Swatch:
                return kSwatchNames[item.index];
            case Item::CustomColor:
                return L"Any color…";
            case Item::Thinner:
                return L"Thinner (mouse wheel)";
            case Item::Thickness:
                return L"Thickness " + std::to_wstring(pen_.thickness) + L" (mouse wheel)";
            case Item::Thicker:
                return L"Thicker (mouse wheel)";
            case Item::Undo:
                return L"Undo (Ctrl+Z)";
            case Item::Copy:
                return L"Copy and close (Ctrl+C)";
            case Item::Save:
                return L"Save and close (Ctrl+S)";
            case Item::Close:
                return L"Close (Esc)";
            case Item::Record:
                return L"Start recording (Enter)";
            case Item::None:
                break;
        }
        return {};
    }

    LPCWSTR CursorAt(POINT point) const {
        if (mode_ == Mode::Selecting) {
            return IDC_CROSS;
        }
        const Tool& tool = kTools[pen_.tool];
        if (drawing_) {
            return tool.cursor;
        }
        if (moving_ >= 0) {
            return IDC_SIZEALL;
        }
        if (ItemAt(point).item != Item::None) {
            return IDC_HAND;
        }
        if (forRecording_) {
            return IDC_CROSS;  // Dragging selects again.
        }
        if (PtInRect(&selection_, point)) {
            int mark = MarkAt(point);
            if (mark >= 0) {
                // Text can be clicked to type into with the text tool, and dragged to move it.
                return tool.typed && marks_[mark]->Text() ? IDC_IBEAM : IDC_SIZEALL;
            }
            return tool.cursor;
        }
        return IDC_ARROW;  // Clicking out here finishes.
    }

    // ---- Painting

    // Dims the parts of the area outside the bright rectangle, drawing into a buffer whose top-left
    // is at the area's top-left.
    void DimOutside(HDC buffer, const RECT& area, const RECT& bright) {
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 120, 0};
        std::vector<RECT> dim;
        if (IsRectEmpty(&bright)) {
            dim.push_back(area);
        } else {
            dim.push_back({area.left, area.top, area.right, bright.top});
            dim.push_back({area.left, bright.bottom, area.right, area.bottom});
            dim.push_back({area.left, bright.top, bright.left, bright.bottom});
            dim.push_back({bright.right, bright.top, area.right, bright.bottom});
        }
        for (RECT part : dim) {
            if (!IntersectRect(&part, &part, &area)) {
                continue;
            }
            AlphaBlend(buffer, part.left - area.left, part.top - area.top, part.right - part.left, part.bottom - part.top, blackDc_,
                       0, 0, 1, 1, blend);
        }
    }

    void Paint(HDC dc, const RECT& area) {
        int width = area.right - area.left;
        int height = area.bottom - area.top;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HDC buffer = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!buffer || !bitmap) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
            if (buffer) {
                DeleteDC(buffer);
            }
            return;
        }
        HGDIOBJ previous = SelectObject(buffer, bitmap);
        BitBlt(buffer, 0, 0, width, height, screen_->Dc(), area.left, area.top, SRCCOPY);
        RECT bright = Highlight();
        DimOutside(buffer, area, bright);
        {
            gp::Graphics graphics(buffer);
            graphics.TranslateTransform((float)-area.left, (float)-area.top);
            graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
            if (!IsRectEmpty(&bright)) {
                graphics.SetClip(ToRectF(bright));
                for (size_t i = 0; i < marks_.size(); i++) {
                    marks_[i]->Draw(graphics, typing_ && (int)i == selected_ && caretOn_);
                }
                if (drawing_) {
                    drawing_->Draw(graphics, false);
                }
                if (hoverMark_ >= 0 && hoverMark_ != selected_) {
                    DrawMarkFrame(graphics, *marks_[hoverMark_], false);
                }
                if (selected_ >= 0) {
                    DrawMarkFrame(graphics, *marks_[selected_], true);
                }
                graphics.ResetClip();
                DrawSelectionFrame(graphics, bright);
            }
            if (mode_ == Mode::Selecting) {
                if (dragging_) {
                    DrawSizeLabel(graphics, bright);
                } else {
                    DrawHint(graphics);
                }
            } else {
                DrawToolbox(graphics);
            }
        }
        BitBlt(dc, area.left, area.top, width, height, buffer, 0, 0, SRCCOPY);
        SelectObject(buffer, previous);
        DeleteObject(bitmap);
        DeleteDC(buffer);
    }

    // A dashed outline around the selected mark, or a faint one around the mark under the mouse.
    void DrawMarkFrame(gp::Graphics& graphics, const Mark& mark, bool selected) {
        float scale = Scale();
        gp::RectF frame = mark.Outline();
        float spread = mark.Width() / 2 + 4 * scale;
        frame.Inflate(spread, spread);
        if (!selected) {
            gp::Pen pen(WithAlpha(150, kSelectionColor), std::max(1.0f, scale));
            graphics.DrawRectangle(&pen, frame);
            return;
        }
        // Dashes over a white line, to show on light and dark pictures alike.
        gp::Pen under(Argb(220, 255, 255, 255), 1.5f * scale);
        graphics.DrawRectangle(&under, frame);
        gp::Pen pen(Argb(255, 0, 120, 212), 1.5f * scale);
        const gp::REAL dashes[] = {3, 2};  // In pen widths.
        pen.SetDashPattern(dashes, ARRAYSIZE(dashes));
        graphics.DrawRectangle(&pen, frame);
    }

    // A crisp line just outside the selection with a soft glow around it.
    void DrawSelectionFrame(gp::Graphics& graphics, const RECT& rect) {
        float scale = mode_ == Mode::Editing ? Scale() : ScaleForScreenRect(ToScreen(rect));
        gp::RectF frame = ToRectF(rect);
        for (int ring = 3; ring >= 1; ring--) {
            gp::RectF glow = frame;
            float spread = ring * 2.5f * scale;
            glow.Inflate(spread, spread);
            gp::Pen pen(WithAlpha(30 + (3 - ring) * 25, kSelectionColor), 2.5f * scale);
            graphics.DrawRectangle(&pen, glow);
        }
        gp::RectF line = frame;
        float width = std::max(1.5f, 1.5f * scale);
        line.Inflate(width / 2, width / 2);
        gp::Pen pen(kSelectionColor, width);
        graphics.DrawRectangle(&pen, line);
    }

    // The selection's size in pixels, next to it while dragging.
    void DrawSizeLabel(gp::Graphics& graphics, const RECT& rect) {
        float scale = ScaleForScreenRect(ToScreen(rect));
        gp::Font font(L"Segoe UI", 12.0f * scale, gp::FontStyleRegular, gp::UnitPixel);
        std::wstring text = std::to_wstring(rect.right - rect.left) + L" × " + std::to_wstring(rect.bottom - rect.top);
        float y = rect.top - 36 * scale;
        if (y < 4 * scale) {
            y = rect.top + 8 * scale;
        }
        DrawLabel(graphics, text, font, LabelRect(graphics, text, font, (float)rect.left, y, scale), scale);
    }

    void DrawHint(gp::Graphics& graphics) {
        if (IsRectEmpty(&hintRect_)) {
            return;
        }
        float scale = (hintRect_.bottom - hintRect_.top) / 40.0f;
        gp::Font font(L"Segoe UI", 13.0f * scale, gp::FontStyleRegular, gp::UnitPixel);
        DrawLabel(graphics,
                  forRecording_ ? L"Drag to select an area to record, or click a window  ·  Esc to cancel"
                                : L"Drag to select an area, or click a window  ·  Esc to cancel",
                  font, ToRectF(hintRect_), scale);
    }

    void DrawToolbox(gp::Graphics& graphics) {
        Palette colors = Palette::Current();
        float scale = Scale();
        gp::RectF box = ToRectF(toolboxRect_);
        // A soft shadow under it.
        for (int ring = 1; ring <= 4; ring++) {
            gp::RectF shadow = box;
            shadow.Inflate(ring * scale * 1.5f, ring * scale * 1.5f);
            shadow.Offset(0, 2 * scale);
            FillRoundedRect(graphics, Argb(14, 0, 0, 0), shadow, 10 * scale);
        }
        FillRoundedRect(graphics, colors.background, box, 8 * scale);
        DrawRoundedRect(graphics, colors.border, box, 8 * scale, 1);
        for (int x : separators_) {
            gp::Pen pen(colors.border, 1);
            graphics.DrawLine(&pen, (float)x, box.Y + 12 * scale, (float)x, box.GetBottom() - 12 * scale);
        }
        gp::Font numberFont(L"Segoe UI", 12.0f * scale, gp::FontStyleRegular, gp::UnitPixel);
        bool customColor = std::find(std::begin(kSwatches), std::end(kSwatches), pen_.color) == std::end(kSwatches);
        for (const ToolboxItem& item : toolbox_) {
            gp::RectF rect = ToRectF(item.rect);
            bool hovered = item.item == hoverItem_.item && item.index == hoverItem_.index && item.item != Item::Thickness;
            bool pressed = hovered && item.item == pressedItem_.item && item.index == pressedItem_.index;
            bool active = item.item == Item::Tool && item.index == pen_.tool;
            gp::RectF button = rect;
            button.Inflate(-2 * scale, -2 * scale);
            if (active) {
                FillRoundedRect(graphics, WithAlpha(lightTheme_ ? 40 : 50, colors.accent), button, 4 * scale);
            } else if (pressed || hovered) {
                FillRoundedRect(graphics, pressed ? colors.pressed : colors.hover, button, 4 * scale);
            }
            gp::Color ink = active ? colors.accent : colors.foreground;
            if (item.item == Item::Undo && history_.empty()) {
                ink = WithAlpha(90, colors.foreground);
            }
            gp::RectF icon(rect.X + (rect.Width - 18 * scale) / 2, rect.Y + (rect.Height - 18 * scale) / 2, 18 * scale, 18 * scale);
            switch (item.item) {
                case Item::Tool:
                    kTools[item.index].drawIcon(graphics, icon, ink, scale);
                    break;
                case Item::Swatch:
                case Item::CustomColor: {
                    float diameter = 18 * scale;
                    gp::RectF circle(rect.X + (rect.Width - diameter) / 2, rect.Y + (rect.Height - diameter) / 2, diameter, diameter);
                    bool selected = item.item == Item::Swatch ? pen_.color == kSwatches[item.index] : customColor;
                    if (item.item == Item::Swatch || customColor) {
                        gp::SolidBrush fill(gp::Color(item.item == Item::Swatch ? kSwatches[item.index] : pen_.color));
                        graphics.FillEllipse(&fill, circle);
                        gp::Pen edge(WithAlpha(80, colors.foreground), 1);
                        graphics.DrawEllipse(&edge, circle);
                    } else {
                        DrawGlyph(graphics, kGlyphColor, 16 * scale, rect, colors.foreground);
                    }
                    if (selected) {
                        gp::RectF ring = circle;
                        ring.Inflate(3.5f * scale, 3.5f * scale);
                        gp::Pen pen(colors.foreground, 1.6f * scale);
                        graphics.DrawEllipse(&pen, ring);
                    }
                    break;
                }
                case Item::Thinner:
                    DrawGlyph(graphics, kGlyphMinus, 13 * scale, rect, pen_.thickness > kMinThickness ? ink : WithAlpha(90, ink));
                    break;
                case Item::Thicker:
                    DrawGlyph(graphics, kGlyphPlus, 13 * scale, rect, pen_.thickness < kMaxThickness ? ink : WithAlpha(90, ink));
                    break;
                case Item::Thickness: {
                    // A sample line in the color and thickness, and the number.
                    float line = std::min((float)pen_.thickness, 12.0f) * scale;
                    gp::Pen pen(gp::Color(pen_.color), line);
                    pen.SetStartCap(gp::LineCapRound);
                    pen.SetEndCap(gp::LineCapRound);
                    float middle = rect.Y + rect.Height * 0.42f;
                    graphics.DrawLine(&pen, rect.X + 8 * scale, middle, rect.GetRight() - 8 * scale, middle);
                    gp::RectF number(rect.X, rect.GetBottom() - 15 * scale, rect.Width, 14 * scale);
                    DrawTextLine(graphics, std::to_wstring(pen_.thickness), numberFont, colors.secondary, number, TextAlign::Center);
                    break;
                }
                case Item::Undo:
                    DrawGlyph(graphics, kGlyphUndo, 16 * scale, rect, ink);
                    break;
                case Item::Copy:
                    DrawGlyph(graphics, kGlyphCopy, 16 * scale, rect, ink);
                    break;
                case Item::Save:
                    DrawGlyph(graphics, kGlyphSave, 16 * scale, rect, ink);
                    break;
                case Item::Close:
                    DrawGlyph(graphics, kGlyphClose, 13 * scale, rect, ink);
                    break;
                case Item::Record: {
                    // The record glyph, and what it does.
                    DrawGlyph(graphics, kGlyphRecord, 16 * scale, gp::RectF(rect.X + 12 * scale, rect.Y, 16 * scale, rect.Height), ink);
                    gp::Font font(L"Segoe UI Semibold", 13.0f * scale, gp::FontStyleRegular, gp::UnitPixel);
                    DrawTextLine(graphics, L"Start recording", font, ink,
                                 gp::RectF(rect.X + 34 * scale, rect.Y, rect.Width - 38 * scale, rect.Height));
                    break;
                }
                case Item::None:
                    break;
            }
        }
        // The hovered item's name and shortcut, on the side away from the selection.
        std::wstring tooltip = TooltipFor(hoverItem_);
        if (!tooltip.empty()) {
            gp::Font font(L"Segoe UI", 12.0f * scale, gp::FontStyleRegular, gp::UnitPixel);
            gp::RectF label = LabelRect(graphics, tooltip, font, 0, 0, scale);
            label.X = std::max(box.X, std::min((hoverItem_.rect.left + hoverItem_.rect.right) / 2.0f - label.Width / 2,
                                               box.GetRight() - label.Width));
            label.Y = toolboxAbove_ ? box.Y - label.Height - 6 * scale : box.GetBottom() + 6 * scale;
            DrawLabel(graphics, tooltip, font, label, scale);
        }
    }

    std::unique_ptr<ScreenCopy> screen_;
    std::vector<RECT> windows_;
    Pen pen_;
    bool forRecording_ = false;
    Mode mode_ = Mode::Selecting;
    RECT hover_{};
    RECT hintRect_{};
    RECT selection_{};
    POINT anchor_{};
    bool dragging_ = false;
    MarkList marks_;
    std::vector<MarkList> history_;  // The marks before each change, for undo.
    Change lastChange_ = Change::Other;
    int lastChangeMark_ = -1;
    std::unique_ptr<Mark> drawing_;  // The mark being drawn, until the mouse is let go.
    int selected_ = -1;              // The selected mark, in marks_.
    int hoverMark_ = -1;             // The mark under the mouse.
    int moving_ = -1;                // The mark being dragged.
    bool pressedSelected_ = false;   // It was selected already when the mouse was pressed on it.
    POINT movePressed_{};
    gp::PointF moveLast_;
    bool moved_ = false;
    bool typing_ = false;  // The selected mark is text being typed.
    bool typingNew_ = false;
    bool typingChanged_ = false;
    bool caretOn_ = false;
    float scale_ = 1;  // Of the selection's display.
    std::vector<ToolboxItem> toolbox_;
    std::vector<int> separators_;
    RECT toolboxRect_{};
    bool toolboxAbove_ = true;
    ToolboxItem hoverItem_{Item::None, 0, {}};
    ToolboxItem pressedItem_{Item::None, 0, {}};
    bool pressedOutside_ = false;
    int wheelRemainder_ = 0;
    bool lightTheme_ = false;
    bool done_ = false;
    bool inColorDialog_ = false;
    bool finishAfterDialog_ = false;  // Asked to finish while the color dialog was open.
    std::optional<Finish> finishWhenDialogCloses_;
    HDC blackDc_ = nullptr;
    HBITMAP black_ = nullptr;
    HGDIOBJ blackPrevious_ = nullptr;
};

// ---------------------------------------------------------------------------
// While recording: a red line around the area, and a bar beside it with the time, a stop button
// and a close button. Windows leaves both out of screen captures, so they're not in the recording.

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// A time as hours, minutes and seconds, without the hours when there are none: 1:05.
std::wstring FormatClock(LONGLONG ticks) {
    int seconds = (int)(std::max<LONGLONG>(0, ticks) / kTicksPerSecond);
    WCHAR text[32];
    if (seconds >= 3600) {
        swprintf_s(text, L"%d:%02d:%02d", seconds / 3600, seconds / 60 % 60, seconds % 60);
    } else {
        swprintf_s(text, L"%d:%02d", seconds / 60, seconds % 60);
    }
    return text;
}

// The red line just outside the area being recorded. Clicks go through it.
class RecordingFrame {
public:
    ~RecordingFrame() { Hide(); }

    void Show(const RECT& area, float scale) {
        Hide();
        int gap = std::max(1, (int)std::lround(2 * scale));
        int line = std::max(2, (int)std::lround(2 * scale));
        RECT outer = InflatedRect(area, gap + line);
        int width = outer.right - outer.left;
        int height = outer.bottom - outer.top;
        if (const wchar_t* registered = RegisterForwardingClass<RecordingFrame>(L"TaskbarSnipper.Mod.RecordingFrame")) {
            CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, registered, L"",
                            WS_POPUP, outer.left, outer.top, width, height, nullptr, nullptr, ModuleInstance(), this);
        }
        if (!hwnd_) {
            return;
        }
        SetWindowDisplayAffinity(hwnd_, WDA_EXCLUDEFROMCAPTURE);
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HDC screen = GetDC(nullptr);
        HDC dc = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (bitmap && bits) {
            // Clear, but for the line (opaque red, so premultiplying changes nothing).
            auto pixels = (DWORD*)bits;
            memset(pixels, 0, (size_t)width * height * 4);
            const DWORD red = 0xFF000000 | ((DWORD)kRecordRed.GetR() << 16) | ((DWORD)kRecordRed.GetG() << 8) | kRecordRed.GetB();
            for (int y = 0; y < height; y++) {
                DWORD* row = pixels + (size_t)y * width;
                if (y < line || y >= height - line) {
                    std::fill(row, row + width, red);
                } else {
                    std::fill(row, row + line, red);
                    std::fill(row + width - line, row + width, red);
                }
            }
            HGDIOBJ previous = SelectObject(dc, bitmap);
            POINT position{outer.left, outer.top};
            SIZE size{width, height};
            POINT origin{0, 0};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            UpdateLayeredWindow(hwnd_, screen, &position, &size, dc, &origin, 0, &blend, ULW_ALPHA);
            SelectObject(dc, previous);
        }
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    }

    void Hide() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) { return false; }

    HWND hwnd_ = nullptr;
};

class RecordingBar {
public:
    // The countdown is over: start recording.
    std::function<void()> onCountdownDone;
    // The stop button: stop, and trim the recording.
    std::function<void()> onStop;
    // The close button: stop, and throw the recording away (or don't start it).
    std::function<void()> onCancel;
    // How long the recording is so far, and whether it failed.
    std::function<LONGLONG()> elapsed;
    std::function<bool()> failed;
    // Windows is signing out or shutting down: the recording is finished now, while there's time.
    std::function<void()> onEndSession;

    ~RecordingBar() { Hide(); }

    bool Shown() const { return hwnd_ != nullptr; }

    // Shows the line around the area and the bar beside it, counting down from 3.
    bool Show(const RECT& area) {
        Hide();
        scale_ = ScaleForScreenRect(area);
        if (const wchar_t* registered = RegisterForwardingClass<RecordingBar>(L"TaskbarSnipper.Mod.RecordingBar")) {
            CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, registered, L"Taskbar Snipper", WS_POPUP, 0, 0, 1, 1,
                            nullptr, nullptr, ModuleInstance(), this);
        }
        if (!hwnd_) {
            return false;
        }
        SetWindowDisplayAffinity(hwnd_, WDA_EXCLUDEFROMCAPTURE);
        DWORD corners = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
        counting_ = true;
        countdownEnd_ = GetTickCount64() + 3000;
        hover_ = Button::None;
        pressed_ = Button::None;
        RECT work = WorkAreaForScreenRect(area);
        int width = Px(196);
        int height = Px(44);
        int gap = Px(8);
        // Above the area, else below it, else inside its top edge (it's not recorded either way).
        int left = std::clamp((int)(area.left + area.right - width) / 2, (int)work.left + gap,
                              std::max((int)work.left + gap, (int)work.right - width - gap));
        int top = area.top - gap - height;
        if (top < work.top + gap) {
            top = area.bottom + gap;
            if (top + height > work.bottom - gap) {
                top = std::max((int)work.top + gap, (int)area.top + gap);
            }
        }
        SetWindowPos(hwnd_, HWND_TOPMOST, left, top, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        frame_.Show(area, scale_);
        SetTimer(hwnd_, kTimer, 100, nullptr);
        return true;
    }

    // Recording has started: the bar shows the time.
    void StartedRecording() {
        counting_ = false;
        if (hwnd_) {
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
    }

    void Hide() {
        frame_.Hide();
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        switch (message) {
            case WM_ENDSESSION:
                if (wParam && onEndSession) {
                    onEndSession();
                }
                return true;
            case WM_MOUSEACTIVATE:
                result = MA_NOACTIVATE;  // The app being recorded keeps the keyboard.
                return true;
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_PAINT:
                Paint();
                return true;
            case WM_TIMER:
                OnTimer();
                return true;
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    POINT cursor;
                    GetCursorPos(&cursor);
                    ScreenToClient(hwnd_, &cursor);
                    SetCursor(LoadCursorW(nullptr, ButtonAt(cursor) != Button::None ? IDC_HAND : IDC_ARROW));
                    result = TRUE;
                    return true;
                }
                return false;
            case WM_MOUSEMOVE: {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd_, 0};
                TrackMouseEvent(&track);
                SetHover(ButtonAt({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}));
                return true;
            }
            case WM_MOUSELEAVE:
                SetHover(Button::None);
                return true;
            case WM_LBUTTONDOWN:
                pressed_ = ButtonAt({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                SetCapture(hwnd_);
                InvalidateRect(hwnd_, nullptr, FALSE);
                return true;
            case WM_LBUTTONUP: {
                Button pressed = pressed_;
                pressed_ = Button::None;
                if (GetCapture() == hwnd_) {
                    ReleaseCapture();
                }
                InvalidateRect(hwnd_, nullptr, FALSE);
                if (pressed != Button::None && pressed == ButtonAt({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)})) {
                    // Last: the bar may be gone after.
                    auto action = pressed == Button::Stop ? onStop : onCancel;
                    if (action) {
                        action();
                    }
                }
                return true;
            }
        }
        return false;
    }

    HWND hwnd_ = nullptr;

private:
    enum class Button { None, Stop, Close };

    static constexpr UINT_PTR kTimer = 1;

    int Px(float value) const { return (int)std::lround(value * scale_); }

    RECT ButtonRect(Button button) const {
        int left = button == Button::Stop ? Px(110) : Px(152);
        return {left, Px(4), left + Px(36), Px(40)};
    }

    Button ButtonAt(POINT point) const {
        for (Button button : {Button::Stop, Button::Close}) {
            RECT rect = ButtonRect(button);
            if (PtInRect(&rect, point) && !(button == Button::Stop && counting_)) {
                return button;
            }
        }
        return Button::None;
    }

    void SetHover(Button button) {
        if (button != hover_) {
            hover_ = button;
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
    }

    void OnTimer() {
        if (counting_ && GetTickCount64() >= countdownEnd_) {
            counting_ = false;
            if (auto done = onCountdownDone) {
                done();  // Last: the bar may be gone after.
            }
            return;
        }
        if (!counting_ && failed && failed()) {
            if (auto stop = onStop) {
                stop();
            }
            return;
        }
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void Paint() {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(hwnd_, &paint);
        RECT client;
        GetClientRect(hwnd_, &client);
        HDC memory = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, client.right, client.bottom);
        HGDIOBJ previous = SelectObject(memory, bitmap);
        PaintTo(memory, client);
        BitBlt(dc, 0, 0, client.right, client.bottom, memory, 0, 0, SRCCOPY);
        SelectObject(memory, previous);
        DeleteObject(bitmap);
        DeleteDC(memory);
        EndPaint(hwnd_, &paint);
    }

    // Draws the window's content into the device context.
    void PaintTo(HDC memory, const RECT& client) {
        Palette colors = Palette::Current();
        gp::Graphics graphics(memory);
        graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(gp::TextRenderingHintClearTypeGridFit);
        graphics.Clear(WithAlpha(255, colors.background));
        gp::Font font(L"Segoe UI Semibold", 14.0f * scale_, gp::FontStyleRegular, gp::UnitPixel);
        float height = (float)client.bottom;
        if (counting_) {
            int seconds = (int)std::max<LONGLONG>(1, ((LONGLONG)countdownEnd_ - (LONGLONG)GetTickCount64() + 999) / 1000);
            DrawTextLine(graphics, L"Recording in " + std::to_wstring(seconds), font, colors.foreground,
                         gp::RectF((float)Px(16), 0, (float)Px(132), height));
        } else {
            // A blinking dot, and the time so far.
            if (GetTickCount64() / 500 % 2 == 0) {
                gp::SolidBrush dot(colors.foreground);
                graphics.FillEllipse(&dot, (float)Px(15), height / 2 - Px(5), (float)Px(10), (float)Px(10));
            }
            DrawTextLine(graphics, FormatClock(elapsed ? elapsed() : 0), font, colors.foreground,
                         gp::RectF((float)Px(34), 0, (float)Px(72), height));
        }
        for (Button button : {Button::Stop, Button::Close}) {
            if (button == Button::Stop && counting_) {
                continue;
            }
            gp::RectF rect = ToRectF(ButtonRect(button));
            if (button == hover_ || button == pressed_) {
                FillRoundedRect(graphics, button == pressed_ ? colors.pressed : colors.hover, rect, 4 * scale_);
            }
            if (button == Button::Stop) {
                DrawGlyph(graphics, StopRecordingGlyph(), 16 * scale_, rect, colors.foreground);
            } else {
                DrawGlyph(graphics, kGlyphClose, 12 * scale_, rect, colors.foreground);
            }
        }
        gp::Pen border(colors.border, 1);
        graphics.DrawRectangle(&border, 0.5f, 0.5f, client.right - 1.0f, client.bottom - 1.0f);
    }

    RecordingFrame frame_;
    float scale_ = 1;
    bool counting_ = false;
    ULONGLONG countdownEnd_ = 0;
    Button hover_ = Button::None;
    Button pressed_ = Button::None;
};

// ---------------------------------------------------------------------------
// The trim window, after recording: plays the recording, trims its start and end with the handles
// on the strip of frames under it, and copies or saves it. Frames are decoded at the size they're
// shown, on the graphics card where there is one. Finding a frame goes a step at a time between
// mouse moves: the nearest key frame shows at once, then the exact frame.

// A time as minutes, seconds and tenths: 1:05.3.
std::wstring FormatTime(LONGLONG ticks) {
    int tenths = (int)(std::max<LONGLONG>(0, ticks + kTicksPerSecond / 20) / (kTicksPerSecond / 10));
    WCHAR text[32];
    swprintf_s(text, L"%d:%02d.%d", tenths / 600, tenths / 10 % 60, tenths % 10);
    return text;
}

// Draws pixels (top row first) at their own size, cut to the width if it's less.
void DrawPixels(HDC dc, const std::vector<BYTE>& pixels, int width, int height, int x, int y, int drawWidth) {
    if (width <= 0 || height <= 0 || pixels.size() < (size_t)width * height * 4) {
        return;
    }
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    drawWidth = std::min(drawWidth, width);
    StretchDIBits(dc, x, y, drawWidth, height, 0, 0, drawWidth, height, pixels.data(), &info, DIB_RGB_COLORS, SRCCOPY);
}

double Milliseconds() {
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return counter.QuadPart * 1000.0 / frequency.QuadPart;
}

class TrimWindow {
public:
    // Called as the window goes, to delete it.
    std::function<void()> onClosed;
    // Called when a copy is on the clipboard (saved is false) or the recording is saved, or that
    // failed. frame is the picture shown, and where is the window.
    std::function<void(bool saved, bool ok, const std::wstring& path, gp::Bitmap* frame, const RECT& where)> onExported;
    // The folder to save in.
    std::function<std::wstring()> saveFolder;

    // The recording is deleted when the window goes.
    TrimWindow(std::wstring path, const RECT& nearRect) : path_(std::move(path)), nearRect_(nearRect) {
        InitializeSRWLock(&thumbnailLock_);
    }

    ~TrimWindow() {
        onClosed = nullptr;
        StopThumbnails();
        if (exportThread_) {
            exportCancel_ = true;
            WaitForSingleObject(exportThread_, INFINITE);
            CloseHandle(exportThread_);
            if (!exportOk_) {
                DeleteFileW(exportTarget_.c_str());
            }
        }
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        reader_.Close();
        // Gone without being closed (the mod turned off), and neither copied nor saved: kept.
        if (opened_ && !closed_ && !copied_ && !saved_) {
            if (saveFolder) {
                MoveRecordingNow(path_, saveFolder());
            }
            return;
        }
        DeleteFileW(path_.c_str());
    }

    // Opens the recording and shows the window on the display of the area. False if the recording
    // can't be read.
    bool Create() {
        if (!mediaFoundation_.Started() || !VideoReader::Probe(path_, videoSize_)) {
            return false;
        }
        HMONITOR monitor = MonitorFromRect(&nearRect_, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96;
        UINT dpiY = 96;
        GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        scale_ = dpiX / 96.0f;
        LayOut();
        if (!OpenPreview()) {
            return false;
        }
        RECT frame{0, 0, (LONG)std::lround((kPad * 2 + kContentWidth) * scale_), (LONG)std::lround(clientHeight_)};
        AdjustWindowRectExForDpi(&frame, kStyle, FALSE, 0, dpiX);
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(monitor, &info);
        int width = frame.right - frame.left;
        int height = frame.bottom - frame.top;
        int left = (info.rcWork.left + info.rcWork.right - width) / 2;
        int top = std::max((int)info.rcWork.top, (int)(info.rcWork.top + info.rcWork.bottom - height) / 2);
        if (const wchar_t* registered = RegisterForwardingClass<TrimWindow>(L"TaskbarSnipper.Mod.Trim")) {
            CreateWindowExW(0, registered, L"Screen recording - Taskbar Snipper", kStyle, left, top, width, height, nullptr, nullptr,
                            ModuleInstance(), this);
        }
        if (!hwnd_) {
            return false;
        }
        UpdateTitleBar();
        DecodeToward(0, 1000);
        StartThumbnails();
        ShowWindow(hwnd_, SW_SHOW);
        // Stopped with Win+Shift+R, the last input went to another app: input from this process
        // first lets the window come to the front.
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));
        SetForegroundWindow(hwnd_);
        opened_ = true;
        return true;
    }

    bool HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) {
        switch (message) {
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_PAINT:
                Paint();
                return true;
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    POINT cursor;
                    GetCursorPos(&cursor);
                    ScreenToClient(hwnd_, &cursor);
                    SetCursor(LoadCursorW(nullptr, CursorAt(cursor)));
                    result = TRUE;
                    return true;
                }
                return false;
            case WM_MOUSEMOVE: {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd_, 0};
                TrackMouseEvent(&track);
                OnMouseMove({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
                return true;
            }
            case WM_MOUSELEAVE:
                SetHover(Button::None);
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
            case WM_CAPTURECHANGED:
                drag_ = Drag::None;
                return true;
            case WM_KEYDOWN:
                OnKeyDown((UINT)wParam);
                return true;
            case WM_TIMER:
                if (wParam == kPlayTimer) {
                    OnPlayTimer();
                } else if (wParam == kDecodeTimer && DecodeToward(wantedTime_, 12)) {
                    // Timers come only when nothing else is waiting, so the mouse and painting go
                    // first.
                    KillTimer(hwnd_, kDecodeTimer);
                }
                return true;
            case kThumbnailMessage:
                Invalidate(film_);
                return true;
            case kExportProgressMessage:
                exportPercent_ = (int)wParam;
                InvalidateControls();
                return true;
            case kExportDoneMessage:
                OnExportDone();
                return true;
            case WM_DPICHANGED: {
                scale_ = HIWORD(wParam) / 96.0f;
                auto suggested = (const RECT*)lParam;
                SetWindowPos(hwnd_, nullptr, suggested->left, suggested->top, suggested->right - suggested->left,
                             suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);
                LayOut();
                // Frames and thumbnails at the new size.
                OpenPreview();
                DecodeToward(playhead_, 1000);
                StartThumbnails();
                InvalidateRect(hwnd_, nullptr, FALSE);
                return true;
            }
            case WM_SETTINGCHANGE:
                UpdateTitleBar();
                InvalidateRect(hwnd_, nullptr, FALSE);
                return false;
            case WM_CLOSE:
                if (exporting_) {
                    MessageBeep(MB_OK);
                } else if (copied_ || saved_ ||
                           MessageBoxW(hwnd_, L"The recording hasn't been copied or saved, and it will be gone. Close it anyway?",
                                       L"Taskbar Snipper", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES) {
                    closed_ = true;
                    DestroyWindow(hwnd_);
                }
                return true;
            case WM_NCDESTROY:
                StopThumbnails();
                if (onClosed) {
                    onClosed();
                }
                return false;
        }
        return false;
    }

    HWND hwnd_ = nullptr;

private:
    enum class Button { None, Play, Copy, Save };
    enum class Drag { None, Start, End, Playhead };

    static constexpr DWORD kStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    static constexpr UINT kThumbnailMessage = WM_APP + 1;
    static constexpr UINT kExportProgressMessage = WM_APP + 3;
    static constexpr UINT kExportDoneMessage = WM_APP + 4;
    static constexpr UINT_PTR kPlayTimer = 1;
    static constexpr UINT_PTR kDecodeTimer = 2;
    // The layout, in pixels at 100% scaling.
    static constexpr float kPad = 16;
    static constexpr float kContentWidth = 720;
    static constexpr float kTrackHeight = 48;
    static constexpr float kHandleWidth = 12;
    static constexpr float kRowHeight = 36;
    static constexpr float kButtonWidth = 100;
    static constexpr LONGLONG kFrameTicks = kTicksPerSecond / kRecordingFps;
    static constexpr LONGLONG kShortest = kTicksPerSecond / 10;  // The shortest a trimmed recording can be.
    // Frames this close before the one wanted are copied out as they're read; ones before them
    // are only decoded.
    static constexpr LONGLONG kCopyWindow = kTicksPerSecond / 5;

    int Px(float value) const { return (int)std::lround(value * scale_); }

    void UpdateTitleBar() {
        BOOL dark = !AppsUseLightTheme();
        DwmSetWindowAttribute(hwnd_, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    }

    // ---- Layout

    void LayOut() {
        float s = scale_;
        float aspect = (float)videoSize_.cx / std::max<LONG>(1, videoSize_.cy);
        preview_ = gp::RectF(kPad * s, kPad * s, kContentWidth * s, std::clamp(kContentWidth / aspect, 240.0f, 480.0f) * s);
        // The picture, fitted into the preview (small ones up to twice their size).
        float fit = std::min({preview_.Width / videoSize_.cx, preview_.Height / videoSize_.cy, 2.0f});
        float width = std::max(2.0f, std::round(videoSize_.cx * fit));
        float height = std::max(2.0f, std::round(videoSize_.cy * fit));
        picture_ = gp::RectF(std::round(preview_.X + (preview_.Width - width) / 2), std::round(preview_.Y + (preview_.Height - height) / 2),
                             width, height);
        track_ = gp::RectF(kPad * s, preview_.GetBottom() + 16 * s, kContentWidth * s, kTrackHeight * s);
        film_ = gp::RectF(track_.X + kHandleWidth * s, track_.Y, track_.Width - 2 * kHandleWidth * s, track_.Height);
        labelsTop_ = track_.GetBottom() + 6 * s;
        float row = labelsTop_ + 28 * s;
        play_ = gp::RectF(kPad * s, row, kRowHeight * s, kRowHeight * s);
        save_ = gp::RectF((kPad + kContentWidth - kButtonWidth) * s, row, kButtonWidth * s, kRowHeight * s);
        copy_ = gp::RectF(save_.X - (8 + kButtonWidth) * s, row, kButtonWidth * s, kRowHeight * s);
        clientHeight_ = row + (kRowHeight + kPad) * s;
        // Thumbnails along the strip, as wide as the picture's shape makes them.
        thumbnailWidth_ = std::max(1, (int)std::lround(std::clamp(film_.Height * aspect, 24 * s, 160 * s)));
        thumbnailHeight_ = std::max(1, (int)film_.Height);
        thumbnailCount_ = std::max(1, (int)std::ceil(film_.Width / thumbnailWidth_));
    }

    float XFor(LONGLONG time) const { return film_.X + film_.Width * (float)time / std::max<LONGLONG>(1, duration_); }

    LONGLONG TimeAt(float x) const {
        return std::clamp<LONGLONG>((LONGLONG)((x - film_.X) / film_.Width * duration_), 0, duration_);
    }

    // The last frame kept, for an end time.
    LONGLONG LastFrameBefore(LONGLONG end) const { return std::max<LONGLONG>(0, end - kFrameTicks / 2 - 1); }

    gp::RectF StartHandle() const { return gp::RectF(XFor(start_) - kHandleWidth * scale_, track_.Y, kHandleWidth * scale_, track_.Height); }
    gp::RectF EndHandle() const { return gp::RectF(XFor(end_), track_.Y, kHandleWidth * scale_, track_.Height); }

    bool Trimmed() const { return start_ > kFrameTicks / 2 || end_ < duration_ - kFrameTicks / 2; }

    void Invalidate(const gp::RectF& rect) {
        RECT area{(LONG)std::floor(rect.X), (LONG)std::floor(rect.Y), (LONG)std::ceil(rect.GetRight()), (LONG)std::ceil(rect.GetBottom())};
        InvalidateRect(hwnd_, &area, FALSE);
    }

    // The strip, the times under it and the row of controls: what moving the playhead or a handle
    // changes.
    void InvalidateControls() {
        RECT client;
        GetClientRect(hwnd_, &client);
        Invalidate(gp::RectF(0, track_.Y - 8 * scale_, (float)client.right, client.bottom - track_.Y + 8 * scale_));
    }

    // ---- Frames

    // Opens the reader for the picture, at its size. keepFrame keeps the frame on screen until the
    // next one is found.
    bool OpenPreview(bool keepFrame = false) {
        reader_.Close();
        if (!keepFrame) {
            haveCurrent_ = false;
        }
        haveAhead_ = false;
        positioned_ = false;
        atEnd_ = false;
        if (!reader_.Open(path_, {(LONG)picture_.Width, (LONG)picture_.Height}, true)) {
            return false;
        }
        duration_ = reader_.Duration();
        if (end_ == 0 || end_ > duration_) {
            end_ = duration_;
        }
        return true;
    }

    // Works toward showing the frame at the time (the last one starting at or before it), for
    // about the time budget. Returns whether it's showing.
    bool DecodeToward(LONGLONG target, double budget) {
        if (!reader_.IsOpen()) {
            return true;  // While saving.
        }
        target = std::clamp<LONGLONG>(target, 0, duration_);
        if (target != stepTarget_) {
            stepTarget_ = target;
            copyFrom_ = target - kCopyWindow;
        }
        double deadline = Milliseconds() + budget;
        do {
            if (Step(target)) {
                return true;
            }
        } while (Milliseconds() < deadline);
        return false;
    }

    // One frame's work toward the target. Returns whether the frame shown is it.
    bool Step(LONGLONG target) {
        // The frame shown is it: the next one starts after the target, or there's none, or none
        // comes before the target.
        if (haveCurrent_ && ((currentTime_ <= target && ((haveAhead_ && aheadTime_ > target) || (atEnd_ && currentIsLast_))) ||
                             target == firstAfter_)) {
            return true;
        }
        // The frame read ahead is at or before it: show that.
        if (haveAhead_ && aheadTime_ <= target) {
            std::swap(currentPixels_, aheadPixels_);
            currentTime_ = aheadTime_;
            haveAhead_ = false;
            currentIsLast_ = true;
            Invalidate(picture_);
            return false;
        }
        // Behind where reading has got to, or well ahead of it: go to the key frame before it, and
        // show that right away.
        if (!positioned_ || target < readTime_ || target > readTime_ + kTicksPerSecond) {
            reader_.Seek(target);
            haveAhead_ = false;
            atEnd_ = false;
            firstAfter_ = -1;
            LONGLONG time = 0;
            if (!reader_.Read(currentPixels_, time)) {
                positioned_ = false;
                atEnd_ = true;
                currentIsLast_ = false;
                return true;  // Nothing more to find.
            }
            positioned_ = true;
            readTime_ = time;
            currentTime_ = time;
            if (time > target) {
                firstAfter_ = target;  // The video has nothing earlier.
            }
            haveCurrent_ = true;
            currentIsLast_ = true;
            Invalidate(picture_);
            return false;
        }
        // Read on.
        LONGLONG previous = readTime_;
        LONGLONG time = 0;
        if (!reader_.Next(time)) {
            atEnd_ = true;
            if (currentIsLast_) {
                return true;
            }
            // The last frame was passed over without copying it: go back for it.
            copyFrom_ = std::min(copyFrom_, previous);
            positioned_ = false;
            return false;
        }
        readTime_ = time;
        if (time > target) {
            if (currentIsLast_) {
                if (reader_.Pixels(aheadPixels_)) {
                    aheadTime_ = time;
                    haveAhead_ = true;
                }
                currentIsLast_ = false;
                return true;
            }
            // The frame before this one is it, but it was passed over: go back for it.
            copyFrom_ = std::min(copyFrom_, previous);
            positioned_ = false;
            return false;
        }
        if (time >= copyFrom_ && reader_.Pixels(currentPixels_)) {
            currentTime_ = time;
            currentIsLast_ = true;
            Invalidate(picture_);
        } else {
            currentIsLast_ = false;
        }
        return false;
    }

    // Shows the frame at the time: a little work now, and the rest between messages.
    void RequestFrame(LONGLONG time) {
        wantedTime_ = time;
        if (DecodeToward(time, 8)) {
            KillTimer(hwnd_, kDecodeTimer);
        } else {
            SetTimer(hwnd_, kDecodeTimer, USER_TIMER_MINIMUM, nullptr);
        }
    }

    // ---- Thumbnails, made on a thread of their own

    void StartThumbnails() {
        StopThumbnails();
        thumbnailTimes_.clear();
        for (int i = 0; i < thumbnailCount_; i++) {
            thumbnailTimes_.push_back(TimeAt(film_.X + (i + 0.5f) * thumbnailWidth_));
        }
        AcquireSRWLockExclusive(&thumbnailLock_);
        thumbnails_.assign(thumbnailCount_, {});
        thumbnailsWidth_ = thumbnailWidth_;
        thumbnailsHeight_ = thumbnailHeight_;
        ReleaseSRWLockExclusive(&thumbnailLock_);
        thumbnailWindow_ = hwnd_;
        thumbnailCancel_ = false;
        thumbnailThread_ = CreateThread(nullptr, 0, ThumbnailProc, this, 0, nullptr);
    }

    void StopThumbnails() {
        if (thumbnailThread_) {
            thumbnailCancel_ = true;
            WaitForSingleObject(thumbnailThread_, INFINITE);
            CloseHandle(thumbnailThread_);
            thumbnailThread_ = nullptr;
        }
    }

    static DWORD WINAPI ThumbnailProc(void* parameter) {
        auto self = (TrimWindow*)parameter;
        ComThread com;
        MediaFoundation mediaFoundation;
        VideoReader reader;
        if (!mediaFoundation.Started() || !reader.Open(self->path_, {self->thumbnailsWidth_, self->thumbnailsHeight_}, true)) {
            return 0;
        }
        for (size_t i = 0; i < self->thumbnailTimes_.size() && !self->thumbnailCancel_; i++) {
            // The last frame at or before the time; ones well before it are only decoded.
            LONGLONG target = self->thumbnailTimes_[i];
            std::vector<BYTE> pixels;
            LONGLONG time = 0;
            bool have = false;
            reader.Seek(target);
            while (!self->thumbnailCancel_ && reader.Next(time) && !(have && time > target)) {
                if (!have || time >= target - kCopyWindow) {
                    have = reader.Pixels(pixels) || have;
                }
            }
            if (have && reader.Width() == self->thumbnailsWidth_ && reader.Height() == self->thumbnailsHeight_) {
                AcquireSRWLockExclusive(&self->thumbnailLock_);
                self->thumbnails_[i] = std::move(pixels);
                ReleaseSRWLockExclusive(&self->thumbnailLock_);
                PostMessageW(self->thumbnailWindow_, kThumbnailMessage, 0, 0);
            }
        }
        return 0;
    }

    // ---- Playing

    void TogglePlay() {
        if (playing_) {
            StopPlaying();
        } else {
            StartPlaying();
        }
    }

    // Plays the part kept, from the playhead (or the start, when it's outside or at the end).
    void StartPlaying() {
        if (playhead_ < start_ || playhead_ >= end_ - kFrameTicks / 2) {
            playhead_ = start_;
        }
        playing_ = true;
        playFrom_ = playhead_;
        playOrigin_ = Milliseconds();
        KillTimer(hwnd_, kDecodeTimer);
        SetTimer(hwnd_, kPlayTimer, 10, nullptr);
        DecodeToward(playhead_, 100);
        InvalidateControls();
    }

    void StopPlaying() {
        if (playing_) {
            playing_ = false;
            KillTimer(hwnd_, kPlayTimer);
            InvalidateControls();
        }
    }

    void OnPlayTimer() {
        LONGLONG time = playFrom_ + (LONGLONG)((Milliseconds() - playOrigin_) * (kTicksPerSecond / 1000));
        if (time >= end_) {
            playhead_ = end_;
            StopPlaying();
            RequestFrame(LastFrameBefore(end_));
        } else {
            playhead_ = time;
            DecodeToward(time, 25);
        }
        InvalidateControls();
    }

    // Moves the playhead and shows the frame there.
    void Seek(LONGLONG time) {
        StopPlaying();
        playhead_ = std::clamp<LONGLONG>(time, 0, duration_);
        RequestFrame(playhead_);
        InvalidateControls();
    }

    void SetStart(LONGLONG time) {
        start_ = std::clamp<LONGLONG>(time, 0, std::max<LONGLONG>(0, end_ - kShortest));
        InvalidateControls();
    }

    void SetEnd(LONGLONG time) {
        end_ = std::clamp<LONGLONG>(time, std::min(duration_, start_ + kShortest), duration_);
        InvalidateControls();
    }

    // ---- Mouse and keyboard

    Button ButtonAt(POINT point) const {
        gp::PointF at((float)point.x, (float)point.y);
        if (!exporting_ && play_.Contains(at)) {
            return Button::Play;
        }
        if (!exporting_ && copy_.Contains(at)) {
            return Button::Copy;
        }
        if (!exporting_ && save_.Contains(at)) {
            return Button::Save;
        }
        return Button::None;
    }

    Drag DragAt(POINT point) const {
        gp::RectF area = track_;
        area.Inflate(0, 8 * scale_);
        if (exporting_ || !area.Contains(gp::PointF((float)point.x, (float)point.y))) {
            return Drag::None;
        }
        // The handles, with a little room around them; the nearer one when they're close.
        gp::RectF start = StartHandle();
        gp::RectF end = EndHandle();
        float slack = 4 * scale_;
        bool onStart = point.x >= start.X - slack && point.x <= start.GetRight() + slack;
        bool onEnd = point.x >= end.X - slack && point.x <= end.GetRight() + slack;
        if (onStart && onEnd) {
            return std::abs(point.x - start.GetRight()) <= std::abs(point.x - end.X) ? Drag::Start : Drag::End;
        }
        return onStart ? Drag::Start : onEnd ? Drag::End : Drag::Playhead;
    }

    LPCWSTR CursorAt(POINT point) const {
        if (drag_ == Drag::Start || drag_ == Drag::End) {
            return IDC_SIZEWE;
        }
        if (ButtonAt(point) != Button::None) {
            return IDC_HAND;
        }
        Drag drag = DragAt(point);
        return drag == Drag::Start || drag == Drag::End ? IDC_SIZEWE : IDC_ARROW;
    }

    void SetHover(Button button) {
        if (button != hover_) {
            hover_ = button;
            InvalidateControls();
        }
    }

    void OnMouseDown(POINT point) {
        Button button = ButtonAt(point);
        if (button != Button::None) {
            pressed_ = button;
            InvalidateControls();
            return;
        }
        drag_ = DragAt(point);
        if (drag_ != Drag::None) {
            StopPlaying();
            DragTo(point.x);
        }
    }

    void OnMouseMove(POINT point) {
        if (drag_ != Drag::None) {
            DragTo(point.x);
            return;
        }
        SetHover(ButtonAt(point));
    }

    void OnMouseUp(POINT point) {
        Button pressed = pressed_;
        pressed_ = Button::None;
        drag_ = Drag::None;
        InvalidateControls();
        if (pressed != Button::None && pressed == ButtonAt(point)) {
            Activate(pressed);
        }
    }

    void DragTo(int x) {
        LONGLONG time = TimeAt((float)x);
        switch (drag_) {
            case Drag::Start:
                SetStart(time);
                playhead_ = start_;
                RequestFrame(start_);
                break;
            case Drag::End:
                SetEnd(time);
                playhead_ = end_;
                RequestFrame(LastFrameBefore(end_));
                break;
            case Drag::Playhead:
                Seek(time);
                break;
            case Drag::None:
                break;
        }
        InvalidateControls();
    }

    void OnKeyDown(UINT key) {
        if (exporting_) {
            return;
        }
        bool control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        LONGLONG step = shift ? kTicksPerSecond : kFrameTicks;
        // The frame on screen: where I and O put the start and end.
        LONGLONG shown = haveCurrent_ ? currentTime_ : playhead_;
        switch (key) {
            case VK_SPACE:
                TogglePlay();
                break;
            case VK_LEFT:
                Seek(playhead_ - step);
                break;
            case VK_RIGHT:
                Seek(playhead_ + step);
                break;
            case VK_HOME:
                Seek(start_);
                break;
            case VK_END:
                Seek(LastFrameBefore(end_));
                break;
            case 'I':
            case VK_OEM_4:  // [
                SetStart(shown);
                break;
            case 'O':
            case VK_OEM_6:  // ]
                SetEnd(shown + kFrameTicks);
                break;
            case 'C':
                if (control) {
                    Activate(Button::Copy);
                }
                break;
            case 'S':
                if (control) {
                    Activate(Button::Save);
                }
                break;
            case VK_ESCAPE:
                PostMessageW(hwnd_, WM_CLOSE, 0, 0);
                break;
        }
    }

    void Activate(Button button) {
        switch (button) {
            case Button::Play:
                TogglePlay();
                break;
            case Button::Copy:
                Export(false);
                break;
            case Button::Save:
                Export(true);
                break;
            case Button::None:
                break;
        }
    }

    // ---- Copying and saving

    // Saves the part kept to the save folder, or for a copy, to the temporary folder and puts it on
    // the clipboard. Runs on a thread of its own; the window shows how far it's got.
    void Export(bool save) {
        if (exporting_) {
            return;
        }
        StopPlaying();
        std::wstring folder = save && saveFolder ? saveFolder() : WorkFolder();
        if (!folder.empty()) {
            SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);  // Fails harmlessly when it's there already.
        }
        if (folder.empty() || GetFileAttributesW(folder.c_str()) == INVALID_FILE_ATTRIBUTES) {
            status_ = save ? L"Couldn't open the folder to save in" : L"Couldn't make a copy";
            InvalidateControls();
            return;
        }
        exportTarget_ = NewFileName(folder, L"Screen Recording", L".mp4");
        // Saving decodes and encodes the whole picture, which takes a lot of memory: the preview
        // gives its back meanwhile (in a 32-bit process, there isn't room for both).
        KillTimer(hwnd_, kDecodeTimer);
        StopThumbnails();
        reader_.Close();
        positioned_ = false;
        haveAhead_ = false;
        exportSave_ = save;
        exportWhole_ = !Trimmed();
        exportStart_ = start_;
        exportEnd_ = end_;
        exportWindow_ = hwnd_;
        exportCancel_ = false;
        exportOk_ = false;
        exportPercent_ = 0;
        exportThread_ = CreateThread(nullptr, 0, ExportProc, this, 0, nullptr);
        exporting_ = exportThread_ != nullptr;
        status_.clear();
        hover_ = Button::None;
        InvalidateControls();
    }

    static DWORD WINAPI ExportProc(void* parameter) {
        auto self = (TrimWindow*)parameter;
        ComThread com;
        bool ok;
        if (self->exportWhole_) {
            ok = CopyFileW(self->path_.c_str(), self->exportTarget_.c_str(), TRUE);
        } else {
            MediaFoundation mediaFoundation;
            int percent = -1;
            ok = mediaFoundation.Started() &&
                 CopyVideoPart(self->path_, self->exportTarget_, self->exportStart_, self->exportEnd_, [&](double progress) {
                     if ((int)(progress * 100) != percent) {
                         percent = (int)(progress * 100);
                         PostMessageW(self->exportWindow_, kExportProgressMessage, percent, 0);
                     }
                     return !self->exportCancel_;
                 });
        }
        self->exportOk_ = ok;
        PostMessageW(self->exportWindow_, kExportDoneMessage, 0, 0);
        return 0;
    }

    void OnExportDone() {
        WaitForSingleObject(exportThread_, INFINITE);
        CloseHandle(exportThread_);
        exportThread_ = nullptr;
        exporting_ = false;
        OpenPreview(true);
        bool missing = false;
        AcquireSRWLockShared(&thumbnailLock_);
        for (const std::vector<BYTE>& thumbnail : thumbnails_) {
            missing = missing || thumbnail.empty();
        }
        ReleaseSRWLockShared(&thumbnailLock_);
        if (missing) {
            StartThumbnails();
        }
        bool ok = exportOk_;
        if (!ok) {
            DeleteFileW(exportTarget_.c_str());
        } else if (!exportSave_) {
            ok = CopyFileToClipboard(hwnd_, exportTarget_);
        }
        if (exportSave_) {
            saved_ = saved_ || ok;
            status_ = ok ? L"Saved" : L"Couldn't save it";
        } else {
            copied_ = copied_ || ok;
            status_ = ok ? L"Copied" : L"Couldn't copy it";
        }
        InvalidateControls();
        if (onExported) {
            RECT where;
            GetWindowRect(hwnd_, &where);
            if (haveCurrent_) {
                gp::Bitmap frame(reader_.Width(), reader_.Height(), reader_.Width() * 4, PixelFormat32bppRGB, currentPixels_.data());
                onExported(exportSave_, ok, exportTarget_, &frame, where);
            } else {
                onExported(exportSave_, ok, exportTarget_, nullptr, where);
            }
        }
    }

    // ---- Painting

    void Paint() {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(hwnd_, &paint);
        RECT client;
        GetClientRect(hwnd_, &client);
        HDC memory = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, client.right, client.bottom);
        HGDIOBJ previous = SelectObject(memory, bitmap);
        PaintTo(memory, client, paint.rcPaint);
        int width = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;
        BitBlt(dc, paint.rcPaint.left, paint.rcPaint.top, width, height, memory, paint.rcPaint.left, paint.rcPaint.top, SRCCOPY);
        SelectObject(memory, previous);
        DeleteObject(bitmap);
        DeleteDC(memory);
        EndPaint(hwnd_, &paint);
    }

    // Draws the window's content into the device context: just the part that needs it.
    void PaintTo(HDC memory, const RECT& client, const RECT& dirty) {
        Palette colors = Palette::Current();
        lightTheme_ = AppsUseLightTheme();
        gp::Graphics graphics(memory);
        graphics.SetClip(ToRectF(dirty));
        graphics.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(gp::TextRenderingHintClearTypeGridFit);
        graphics.Clear(WithAlpha(255, colors.background));
        FillRoundedRect(graphics, Argb(255, 0, 0, 0), preview_, 6 * scale_);
        FillRoundedRect(graphics, Argb(255, 24, 24, 24), film_, 4 * scale_);
        // The frame and the thumbnails, straight from their pixels.
        graphics.Flush(gp::FlushIntentionSync);
        HDC dc = graphics.GetHDC();
        if (haveCurrent_) {
            DrawPixels(dc, currentPixels_, reader_.Width(), reader_.Height(), (int)picture_.X, (int)picture_.Y, reader_.Width());
        }
        AcquireSRWLockShared(&thumbnailLock_);
        for (size_t i = 0; i < thumbnails_.size(); i++) {
            int x = (int)film_.X + (int)i * thumbnailsWidth_;
            DrawPixels(dc, thumbnails_[i], thumbnailsWidth_, thumbnailsHeight_, x, (int)film_.Y,
                       std::min(thumbnailsWidth_, (int)film_.GetRight() - x));
        }
        ReleaseSRWLockShared(&thumbnailLock_);
        graphics.ReleaseHDC(dc);
        DrawTrack(graphics, colors);
        DrawControls(graphics, colors);
    }

    void DrawTrack(gp::Graphics& graphics, const Palette& colors) {
        float s = scale_;
        // Darkens what's cut.
        float startX = XFor(start_);
        float endX = XFor(end_);
        gp::SolidBrush cut(Argb(175, 0, 0, 0));
        graphics.FillRectangle(&cut, film_.X, film_.Y, startX - film_.X, film_.Height);
        graphics.FillRectangle(&cut, endX, film_.Y, film_.GetRight() - endX, film_.Height);
        // The part kept: a frame with a handle at each end.
        gp::RectF start = StartHandle();
        gp::RectF end = EndHandle();
        gp::SolidBrush frame(kSelectionColor);
        float bar = 3 * s;
        graphics.FillRectangle(&frame, start.GetRight() - 1, track_.Y, end.X - start.GetRight() + 2, bar);
        graphics.FillRectangle(&frame, start.GetRight() - 1, track_.GetBottom() - bar, end.X - start.GetRight() + 2, bar);
        for (const gp::RectF& handle : {start, end}) {
            FillRoundedRect(graphics, kSelectionColor, handle, 3 * s);
            gp::Pen grip(Argb(160, 0, 0, 0), std::max(1.0f, 1.5f * s));
            float middle = handle.X + handle.Width / 2;
            float top = handle.Y + handle.Height * 0.36f;
            float bottom = handle.GetBottom() - handle.Height * 0.36f;
            graphics.DrawLine(&grip, middle - 1.5f * s, top, middle - 1.5f * s, bottom);
            graphics.DrawLine(&grip, middle + 1.5f * s, top, middle + 1.5f * s, bottom);
        }
        // The playhead.
        float x = XFor(playhead_);
        gp::Pen outline(Argb(150, 0, 0, 0), 4 * s);
        graphics.DrawLine(&outline, x, track_.Y - 4 * s, x, track_.GetBottom() + 4 * s);
        gp::Pen line(Argb(255, 255, 255, 255), 2 * s);
        graphics.DrawLine(&line, x, track_.Y - 4 * s, x, track_.GetBottom() + 4 * s);
        // The start and end times under the handles, kept apart.
        gp::Font font(L"Segoe UI", 12.0f * s, gp::FontStyleRegular, gp::UnitPixel);
        std::wstring startText = FormatTime(start_);
        std::wstring endText = FormatTime(end_);
        float startWidth = MeasureTextWidth(graphics, startText, font) + 4 * s;
        float endWidth = MeasureTextWidth(graphics, endText, font) + 4 * s;
        float startLeft = std::clamp(start.X + start.Width / 2 - startWidth / 2, track_.X, track_.GetRight() - startWidth);
        float endLeft = std::clamp(end.X + end.Width / 2 - endWidth / 2, track_.X, track_.GetRight() - endWidth);
        if (endLeft < startLeft + startWidth + 6 * s) {
            float middle = (startLeft + startWidth + endLeft) / 2;
            startLeft = std::max(track_.X, middle - 3 * s - startWidth);
            endLeft = std::min(track_.GetRight() - endWidth, startLeft + startWidth + 6 * s);
        }
        DrawTextLine(graphics, startText, font, colors.secondary, gp::RectF(startLeft, labelsTop_, startWidth, 18 * s), TextAlign::Center);
        DrawTextLine(graphics, endText, font, colors.secondary, gp::RectF(endLeft, labelsTop_, endWidth, 18 * s), TextAlign::Center);
    }

    void DrawButton(gp::Graphics& graphics, const Palette& colors, Button button, const gp::RectF& rect, const wchar_t* glyph,
                    const std::wstring& label, bool primary) {
        float s = scale_;
        bool enabled = !exporting_;
        bool hovered = enabled && button == hover_;
        bool pressed = hovered && button == pressed_;
        gp::Color ink = colors.foreground;
        if (primary) {
            FillRoundedRect(graphics, WithAlpha(enabled ? (pressed ? 200 : hovered ? 225 : 255) : 110, colors.accent), rect, 4 * s);
            ink = lightTheme_ ? Argb(255, 255, 255, 255) : Argb(255, 0, 0, 0);
        } else {
            FillRoundedRect(graphics, pressed ? colors.pressed : colors.hover, rect, 4 * s);
            if (hovered && !pressed) {
                FillRoundedRect(graphics, colors.hover, rect, 4 * s);
            }
            DrawRoundedRect(graphics, colors.border, rect, 4 * s, 1);
            if (!enabled) {
                ink = WithAlpha(110, ink);
            }
        }
        if (label.empty()) {
            DrawGlyph(graphics, glyph, 14 * s, rect, ink);
            return;
        }
        gp::Font font(L"Segoe UI Semibold", 13.0f * s, gp::FontStyleRegular, gp::UnitPixel);
        float textWidth = MeasureTextWidth(graphics, label, font);
        float iconSize = 16 * s;
        float left = rect.X + (rect.Width - iconSize - 8 * s - textWidth) / 2;
        DrawGlyph(graphics, glyph, 14 * s, gp::RectF(left, rect.Y, iconSize, rect.Height), ink);
        DrawTextLine(graphics, label, font, ink, gp::RectF(left + iconSize + 8 * s, rect.Y, textWidth + 4 * s, rect.Height));
    }

    void DrawControls(gp::Graphics& graphics, const Palette& colors) {
        float s = scale_;
        DrawButton(graphics, colors, Button::Play, play_, playing_ ? kGlyphPause : kGlyphPlay, L"", false);
        DrawButton(graphics, colors, Button::Copy, copy_, kGlyphCopy, L"Copy", false);
        DrawButton(graphics, colors, Button::Save, save_, kGlyphSave, L"Save", true);
        // Where the playhead is, and how long the part kept is.
        gp::Font timeFont(L"Segoe UI Semibold", 13.0f * s, gp::FontStyleRegular, gp::UnitPixel);
        gp::Font font(L"Segoe UI", 12.0f * s, gp::FontStyleRegular, gp::UnitPixel);
        std::wstring time = FormatTime(playhead_) + L" / " + FormatTime(duration_);
        float x = play_.GetRight() + 12 * s;
        float timeWidth = MeasureTextWidth(graphics, time, timeFont) + 4 * s;
        DrawTextLine(graphics, time, timeFont, colors.foreground, gp::RectF(x, play_.Y, timeWidth, play_.Height));
        x += timeWidth + 10 * s;
        if (Trimmed()) {
            DrawTextLine(graphics, L"Keeping " + FormatTime(end_ - start_), font, colors.secondary,
                         gp::RectF(x, play_.Y, copy_.X - x, play_.Height));
        }
        // What copying or saving did, right of the rest.
        std::wstring status = status_;
        if (exporting_) {
            status = std::wstring(exportSave_ ? L"Saving" : L"Copying") + L"… " + (exportWhole_ ? L"" : std::to_wstring(exportPercent_) + L"%");
        }
        if (!status.empty()) {
            float width = MeasureTextWidth(graphics, status, font) + 4 * s;
            DrawTextLine(graphics, status, font, colors.secondary, gp::RectF(copy_.X - 12 * s - width, play_.Y, width, play_.Height));
        }
    }

    MediaFoundation mediaFoundation_;  // First: kept until the readers are gone.
    std::wstring path_;
    RECT nearRect_;
    SIZE videoSize_{};
    VideoReader reader_;  // At the picture's size.
    float scale_ = 1;
    bool lightTheme_ = false;
    LONGLONG duration_ = 0;
    LONGLONG start_ = 0;
    LONGLONG end_ = 0;
    LONGLONG playhead_ = 0;
    // The frame on screen; the one after it when it's been read; and how far reading has got.
    std::vector<BYTE> currentPixels_;
    std::vector<BYTE> aheadPixels_;
    LONGLONG currentTime_ = 0;
    LONGLONG aheadTime_ = 0;
    LONGLONG readTime_ = 0;
    LONGLONG firstAfter_ = -1;  // A time before the first frame there is.
    bool haveCurrent_ = false;
    bool haveAhead_ = false;
    bool currentIsLast_ = false;  // The frame on screen is the last one read.
    bool positioned_ = false;     // Reading can go on from readTime_.
    bool atEnd_ = false;
    LONGLONG stepTarget_ = -1;
    LONGLONG copyFrom_ = 0;
    LONGLONG wantedTime_ = 0;
    // Thumbnails, from their thread.
    SRWLOCK thumbnailLock_;
    std::vector<std::vector<BYTE>> thumbnails_;
    std::vector<LONGLONG> thumbnailTimes_;
    int thumbnailWidth_ = 1;
    int thumbnailHeight_ = 1;
    int thumbnailCount_ = 0;
    int thumbnailsWidth_ = 1;  // Of the thumbnails being made.
    int thumbnailsHeight_ = 1;
    HANDLE thumbnailThread_ = nullptr;
    HWND thumbnailWindow_ = nullptr;
    std::atomic<bool> thumbnailCancel_{false};
    bool playing_ = false;
    LONGLONG playFrom_ = 0;
    double playOrigin_ = 0;
    // Layout, in pixels.
    gp::RectF preview_;
    gp::RectF picture_;
    gp::RectF track_;
    gp::RectF film_;
    gp::RectF play_;
    gp::RectF copy_;
    gp::RectF save_;
    float labelsTop_ = 0;
    float clientHeight_ = 0;
    Button hover_ = Button::None;
    Button pressed_ = Button::None;
    Drag drag_ = Drag::None;
    // Copying and saving.
    HANDLE exportThread_ = nullptr;
    HWND exportWindow_ = nullptr;
    std::wstring exportTarget_;
    LONGLONG exportStart_ = 0;
    LONGLONG exportEnd_ = 0;
    bool exportSave_ = false;
    bool exportWhole_ = false;
    std::atomic<bool> exportCancel_{false};
    std::atomic<bool> exportOk_{false};
    int exportPercent_ = 0;
    bool exporting_ = false;
    bool copied_ = false;
    bool saved_ = false;
    bool opened_ = false;  // Shown, with a recording that could be read.
    bool closed_ = false;  // By the user.
    std::wstring status_;
};

// ---------------------------------------------------------------------------
// The app: waits for Print Screen, Win+Shift+R or a click on a button, runs the overlay, copies or
// saves the screenshot, and records the screen and opens the recording to trim.

class SnipApp {
public:
    bool Start() {
        settings_ = ModSettings::Load();
        int tool = ToolForKey((wchar_t)Storage::GetInt(L"Tool", kDefaultToolKey));
        if (tool >= 0) {
            pen_.tool = tool;
        }
        pen_.color = (ARGB)Storage::GetInt(L"Color", (int)pen_.color);
        pen_.thickness = std::clamp(Storage::GetInt(L"Thickness", pen_.thickness), kMinThickness, kMaxThickness);
        tidyRecordings_ = settings_.recordings.path;
        tidying_ = CreateThread(nullptr, 0, TidyProc, this, 0, nullptr);
        if (!messages_.Create(L"TaskbarSnipper.Mod.Messages") || !notice_.Create()) {
            return false;
        }
        messages_.onMessage = [this](UINT message, WPARAM wParam, LPARAM lParam) { OnMessage(message, lParam); };
        // The buttons' window is there even while it's hidden, so the other widgets still see it,
        // and it still hears about changes to the Windows settings.
        widget_.SetButtons(settings_.screenshotButton, settings_.recordButton);
        if (!widget_.Create(Storage::GetInt(L"TaskbarOffset", -1))) {
            return false;
        }
        widget_.onOffsetChanged = [](int offset) { Storage::SetInt(L"TaskbarOffset", offset); };
        widget_.onSnip = [this]() { StartCaptureFromWidget(); };
        widget_.onRecord = [this]() { ToggleRecording(); };
        widget_.onMenu = [this]() { ShowMenu(); };
        widget_.SetShown(settings_.showButton);
        hook_.SetRecordShortcut(settings_.recordShortcut);
        return hook_.Start(messages_.Handle());
    }

    ~SnipApp() {
        hook_.Stop();
        overlay_.reset();
        // Turning off while recording: the recording is finished and kept, as are any not yet copied
        // or saved in trim windows.
        if (recorder_.Recording()) {
            recorder_.Stop();
            MoveRecordingNow(recordingPath_, settings_.recordings.path);
        }
        recordingBar_.Hide();
        trimWindows_.clear();
        widget_.Destroy();
        if (tidying_) {
            cancelTidy_ = TRUE;  // What's left is done next time.
            WaitForSingleObject(tidying_, INFINITE);
            CloseHandle(tidying_);
        }
    }

    HWND MessageHandle() const { return messages_.Handle(); }

private:
    void OnMessage(UINT message, LPARAM lParam) {
        switch (message) {
            case kPrintScreenMessage:
                StartCapture(false);
                break;
            case kRecordMessage:
                ToggleRecording();
                break;
            case kEscapeMessage:
                if (overlay_) {
                    overlay_->Escape();
                }
                break;
            case kSettingsChangedMessage:
                settings_ = ModSettings::Load();
                widget_.SetButtons(settings_.screenshotButton, settings_.recordButton);
                widget_.SetShown(settings_.showButton);
                hook_.SetRecordShortcut(settings_.recordShortcut);
                break;
            case kOverlayClosedMessage:
                overlay_.reset();
                hook_.SetOverlayOpen(false);
                break;
            case kTrimClosedMessage: {
                auto closed = (const TrimWindow*)lParam;
                std::erase_if(trimWindows_, [closed](const std::unique_ptr<TrimWindow>& window) { return window.get() == closed; });
                break;
            }
        }
    }

    // Opens the overlay to select an area: for a screenshot, or to record.
    void StartCapture(bool forRecording) {
        if (overlay_ || (forRecording && recordingBar_.Shown())) {
            return;
        }
        // Where the windows are, before the overlay covers them.
        std::vector<RECT> windows = VisibleWindowRects();
        auto screen = std::make_unique<ScreenCopy>();
        if (!screen->Take()) {
            Wh_Log(L"Couldn't copy the screen");
            return;
        }
        overlay_ = std::make_unique<Overlay>(std::move(screen), std::move(windows), pen_, forRecording);
        overlay_->onPenChanged = [this](const Pen& pen) { pen_ = pen; };
        overlay_->onDone = [this](std::optional<Finish> finish) { OnOverlayDone(finish); };
        if (!overlay_->Create()) {
            Wh_Log(L"Couldn't open the overlay");
            overlay_.reset();
            return;
        }
        hook_.SetOverlayOpen(true);
    }

    // A screenshot from the button or its menu: waits for the screen to show the button as it is
    // when it isn't hovered or pressed, so the screenshot shows it that way too.
    void StartCaptureFromWidget() {
        DwmFlush();
        StartCapture(false);
    }

    void OnOverlayDone(std::optional<Finish> finish) {
        // The overlay is deleted once its window has finished with this message.
        PostMessageW(messages_.Handle(), kOverlayClosedMessage, 0, 0);
        Storage::SetInt(L"Tool", kTools[pen_.tool].key);  // By its shortcut, which stays the same as tools are added.
        Storage::SetInt(L"Color", (int)pen_.color);
        Storage::SetInt(L"Thickness", pen_.thickness);
        if (!finish) {
            return;
        }
        RECT onScreen = overlay_->SelectionOnScreen();
        if (*finish == Finish::Record) {
            StartCountdown(onScreen);
            return;
        }
        RECT selection = overlay_->SelectionInOverlay();
        std::unique_ptr<gp::Bitmap> image = ComposeScreenshot(overlay_->Screen(), selection, overlay_->Marks());
        if (!image) {
            notice_.Show(L"Couldn't make the screenshot", L"Windows ran out of memory for it.", L"", nullptr, onScreen);
            return;
        }
        const std::wstring& folder = settings_.screenshots.path;
        bool copy = *finish == Finish::Copy || (*finish == Finish::Default && settings_.afterCapture != AfterCapture::Save);
        bool save = *finish == Finish::Save || (*finish == Finish::Default && settings_.afterCapture != AfterCapture::Copy);
        bool copied = copy && CopyToClipboard(messages_.Handle(), *image);
        std::optional<std::wstring> saved = save ? SaveScreenshot(*image, folder) : std::nullopt;

        std::wstring size = std::to_wstring(image->GetWidth()) + L" × " + std::to_wstring(image->GetHeight());
        std::wstring fileName = saved ? saved->substr(saved->find_last_of(L'\\') + 1) : L"";
        if (save && !saved) {
            notice_.Show(copied ? L"Copied, but couldn't save it" : L"Couldn't save the screenshot", folder, L"", image.get(), onScreen);
        } else if (copy && !copied) {
            notice_.Show(saved ? L"Saved, but couldn't copy it" : L"Couldn't copy the screenshot",
                         saved ? fileName : L"Another app is holding the clipboard.", saved.value_or(L""), image.get(), onScreen);
        } else if (copied && saved) {
            notice_.Show(L"Copied and saved", fileName + L" · click to show it", *saved, image.get(), onScreen);
        } else if (saved) {
            notice_.Show(L"Screenshot saved", fileName + L" · click to show it", *saved, image.get(), onScreen);
        } else {
            notice_.Show(L"Copied to the clipboard", size, L"", image.get(), onScreen);
        }
    }

    // ---- Recording

    // Win+Shift+R and the record button: select an area to record, or stop recording.
    void ToggleRecording() {
        if (recordingBar_.Shown()) {
            StopRecording();
        } else {
            DwmFlush();  // As for a screenshot from the button.
            StartCapture(true);
        }
    }

    // Counts down beside the area, then records it.
    void StartCountdown(const RECT& area) {
        recordingArea_ = area;
        recordingBar_.onCountdownDone = [this]() { StartRecording(); };
        recordingBar_.onStop = [this]() { StopRecording(); };
        recordingBar_.onCancel = [this]() { CancelRecording(); };
        recordingBar_.elapsed = [this]() { return recorder_.Elapsed(); };
        recordingBar_.failed = [this]() { return recorder_.Failed(); };
        recordingBar_.onEndSession = [this]() {
            if (recorder_.Recording()) {
                recorder_.Stop();
                if (MoveRecordingNow(recordingPath_, settings_.recordings.path)) {
                    recordingPath_.clear();
                }
            }
        };
        if (recordingBar_.Show(area)) {
            widget_.SetRecording(true);
        }
    }

    void StartRecording() {
        std::wstring folder = WorkFolder();
        recordingPath_ = folder.empty() ? L"" : NewFileName(folder, L"Recording", L".mp4");
        SIZE size = VideoSizeFor(recordingArea_.right - recordingArea_.left, recordingArea_.bottom - recordingArea_.top);
        if (recordingPath_.empty() || !recorder_.Start(recordingPath_, size, ScreenFrames(recordingArea_))) {
            recordingBar_.Hide();
            widget_.SetRecording(false);
            if (!recordingPath_.empty()) {
                DeleteFileW(recordingPath_.c_str());
                recordingPath_.clear();
            }
            notice_.Show(L"Couldn't start recording", L"Windows couldn't start a video file.", L"", nullptr, recordingArea_);
            return;
        }
        recordingBar_.StartedRecording();
    }

    // Stops recording, and opens the recording to trim. During the countdown, it just stops.
    void StopRecording() {
        bool recording = recorder_.Recording();
        recorder_.Stop();
        recordingBar_.Hide();
        widget_.SetRecording(false);
        if (recording) {
            OpenTrimWindow(recordingPath_);
            recordingPath_.clear();  // The trim window has it now.
        }
    }

    // Stops recording and throws the recording away.
    void CancelRecording() {
        bool recording = recorder_.Recording();
        recorder_.Stop();
        recordingBar_.Hide();
        widget_.SetRecording(false);
        if (recording) {
            DeleteFileW(recordingPath_.c_str());
        }
        recordingPath_.clear();
    }

    void OpenTrimWindow(const std::wstring& path) {
        auto window = std::make_unique<TrimWindow>(path, recordingArea_);
        const TrimWindow* opened = window.get();
        window->saveFolder = [this]() { return settings_.recordings.path; };
        window->onExported = [this](bool saved, bool ok, const std::wstring& file, gp::Bitmap* frame, const RECT& where) {
            OnRecordingExported(saved, ok, file, frame, where);
        };
        window->onClosed = [this, opened]() { PostMessageW(messages_.Handle(), kTrimClosedMessage, 0, (LPARAM)opened); };
        if (!window->Create()) {
            // The recording goes with the window.
            notice_.Show(L"The recording didn't work", L"Windows couldn't make a video of it.", L"", nullptr, recordingArea_);
            return;
        }
        trimWindows_.push_back(std::move(window));
    }

    void OnRecordingExported(bool saved, bool ok, const std::wstring& file, gp::Bitmap* frame, const RECT& where) {
        std::wstring fileName = file.substr(file.find_last_of(L'\\') + 1);
        if (saved && ok) {
            notice_.Show(L"Recording saved", fileName + L" · click to show it", file, frame, where);
        } else if (saved) {
            notice_.Show(L"Couldn't save the recording", settings_.recordings.path, L"", frame, where);
        } else if (ok) {
            notice_.Show(L"Recording copied", L"Paste it into a chat, an email or a folder.", L"", frame, where);
        } else {
            notice_.Show(L"Couldn't copy the recording", L"Another app is holding the clipboard.", L"", frame, where);
        }
    }

    // ---- The menu

    void OpenFolder(const std::wstring& folder) {
        SHCreateDirectoryExW(nullptr, folder.c_str(), nullptr);  // Fails harmlessly when it's there already.
        ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    void ChooseFolder(SaveFolder& folder, const wchar_t* title, const wchar_t* done) {
        std::optional<std::wstring> chosen = PickFolder(widget_.OwnerWindow(), title, folder.path);
        if (chosen) {
            folder.Choose(*chosen);
            notice_.Show(done, *chosen, L"", nullptr, widget_.Bounds());
        }
    }

    void ShowMenu() {
        Menu menu;
        menu.Add(L"Take a screenshot\tPrint Screen", [this]() { StartCaptureFromWidget(); });
        if (recordingBar_.Shown()) {
            menu.Add(L"Stop recording", [this]() { StopRecording(); });
        } else {
            menu.Add(std::wstring(L"Record the screen") + (settings_.recordShortcut ? L"\tWin+Shift+R" : L""),
                     [this]() { ToggleRecording(); });
        }
        menu.AddSeparator();
        menu.Add(L"Open the screenshots folder", [this]() { OpenFolder(settings_.screenshots.path); });
        menu.Add(L"Choose the screenshots folder…", [this]() {
            ChooseFolder(settings_.screenshots, L"Choose the folder to save screenshots in", L"Screenshots will be saved here");
        });
        menu.AddSeparator();
        menu.Add(L"Open the recordings folder", [this]() { OpenFolder(settings_.recordings.path); });
        menu.Add(L"Choose the recordings folder…", [this]() {
            ChooseFolder(settings_.recordings, L"Choose the folder to save recordings in", L"Recordings will be saved here");
        });
        AddLinkMenu(menu, widget_);
        RECT bounds = widget_.Bounds();
        POINT cursor;
        GetCursorPos(&cursor);
        // Above the widget, so the menu doesn't cover it.
        menu.Show(widget_.OwnerWindow(), {cursor.x, bounds.top});
    }

    ModSettings settings_;
    Pen pen_;
    MessageWindow messages_;
    KeyboardHook hook_;
    Notice notice_;
    SnipperWidget widget_;
    std::unique_ptr<Overlay> overlay_;
    Recorder recorder_;
    RecordingBar recordingBar_;
    RECT recordingArea_{};
    std::wstring recordingPath_;  // While recording.
    std::vector<std::unique_ptr<TrimWindow>> trimWindows_;
    // Tidying the work folder, at start.
    HANDLE tidying_ = nullptr;
    std::wstring tidyRecordings_;
    BOOL cancelTidy_ = FALSE;

    static DWORD WINAPI TidyProc(void* parameter) {
        auto self = (SnipApp*)parameter;
        TidyWorkFolder(self->tidyRecordings_, &self->cancelTidy_);
        return 0;
    }
};

// ---------------------------------------------------------------------------
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

UiThread g_uiThread;
std::atomic<HWND> g_messageWindow{nullptr};  // For telling the app the settings changed.

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    // Windhawk loads the mod into every Explorer process, folder windows' too: it runs in the one
    // with the taskbar (or, while Explorer is starting, the one that's about to have it).
    if (InFolderWindowProcess() || (TaskbarWindow() && !WidgetTaskbar())) {
        return FALSE;
    }
    return g_uiThread.Start([]() -> std::shared_ptr<void> {
        auto app = std::make_shared<SnipApp>();
        if (!app->Start()) {
            Wh_Log(L"Couldn't start");
            return nullptr;
        }
        g_messageWindow = app->MessageHandle();
        return app;
    });
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (HWND window = g_messageWindow) {
        PostMessageW(window, kSettingsChangedMessage, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_messageWindow = nullptr;
    g_uiThread.Stop();
}
