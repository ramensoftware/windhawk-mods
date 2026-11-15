// ==WindhawkMod==
// @id              what-when-where-notes
// @name            What When Where Notes
// @description     Multi-row notes with a centered title and editable note lines.
// @version         1.3.2
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32 -lcomctl32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# What When Where Notes

A customizable desktop note overlay for Windows, powered by Windhawk.
Organize reminders in three editable parts: **What - When - Where**.

## Screenshots

### Task overview and status indicators

![Task overview with numbered notes, gray red and green status circles, and completed tasks struck through](https://raw.githubusercontent.com/SilverAmd/WHAT-WHEN-WHERE-NOTES-MOD/main/screenshots/status-yellow.png)

### Custom colors and row highlighting

![Green notes, pink heading and highlighted task row](https://raw.githubusercontent.com/SilverAmd/WHAT-WHEN-WHERE-NOTES-MOD/main/screenshots/green-hover.png)

### Edit notes directly on the desktop

![Editing the WHERE field inside the desktop overlay](https://raw.githubusercontent.com/SilverAmd/WHAT-WHEN-WHERE-NOTES-MOD/main/screenshots/edit-note.png)

### Right-click color and transparency menu

![Context menu for changing note color and transparency](https://raw.githubusercontent.com/SilverAmd/WHAT-WHEN-WHERE-NOTES-MOD/main/screenshots/color-transparency-menu.png)

## Task status indicators

Click the colored circle next to a task to change its state:

- ⚪ **Gray — Open:** The task has not been started.
- 🔴 **Red — In Progress:** Work on the task is underway.
- 🟢 **Green — Completed:** What, When and Where are struck through.

**Cycle:** ⚪ Open → 🔴 In Progress → 🟢 Completed → ⚪ Open. Each task's status is saved across Windhawk restarts.

## Features

- Up to **14 numbered note rows** with independently editable What, When and Where fields.
- Optional centered title, adjustable fonts, spacing and colors.
- Three status states for each row: **gray** (open), **red** (in progress), and **green** (done). Completed notes are struck through.
- Add rows, remove the last row, or delete a selected row.
- Move the overlay with a configurable drag-mode hotkey.
- Adjustable transparency and note color; optional always-on-top behavior.
- Saved notes, row statuses, position and display preferences persist across restarts.

## Using the notes

- **Click** a What, When or Where field to edit it. Press **Enter** to save or **Esc** to cancel. Leaving the edit field also saves.
- Click **+** to add a row (up to the configured maximum) or **-** to remove the last row.
- Click the **X** next to a row to delete that row. At least one row is kept.
- Click a **status circle** to cycle through open, in progress and done.
- **Right-click** the overlay to open the color and transparency menu.
- **Mouse wheel** over the overlay to scale it in 10% steps (20% to 320%); **middle-click** to reset the scale.
- Press **Ctrl+Alt+N** by default to toggle drag mode, then drag the overlay with the left mouse button. Press the hotkey again to exit drag mode.

## Configuration

Open **Settings** to customize the initial position, always-on-top mode, title, the three default note fields, separators, maximum row count, fonts, colors, default transparency, default scale, and the drag hotkey.

**Note:** Existing edited notes and saved display preferences are stored separately from the initial defaults. Changing a default does not automatically overwrite previously saved note rows. A previously dragged position is preserved when changing unrelated options. Editing X/Y settings explicitly moves the overlay to those new coordinates.

## Technical information

- Designed for **Windows / Windhawk**.
- Runs as a Windhawk **tool mod**, using a dedicated `windhawk.exe` helper process.
- The overlay is implemented with the Windows API and GDI.
- Source code and license: [GitHub repository](https://github.com/SilverAmd/WHAT-WHEN-WHERE-NOTES-MOD).

## Project background and relation to Easy Notes

This mod is an independent Windows/Windhawk implementation inspired by the
original ToDoLite / What When Where Rainmeter project by the same author:
https://forum.rainmeter.net/viewtopic.php?t=42543#p215957

Unlike free-form desktop sticky notes, this mod displays a structured three-field
task list (What / When / Where) with distinct Open / In Progress / Completed
states. The current version is intentionally focused on that workflow.

## Author

**SilverAmd**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position:
  - x: 60
    $name: X position
  - y: 60
    $name: Y position
  - alwaysOnTop: false
    $name: Always on top
  $name: Position

- title:
  - enabled: true
    $name: Show title
  - text: "What When Where Notes"
    $name: Title text
  - gap: 18
    $name: Gap below title
  - color: "#0000FF"
    $name: Title color
  $name: Title

- notes:
  - note1DefaultText: "What"
    $name: Default note 1
    $description: "What"
  - note2DefaultText: "When"
    $name: Default note 2
    $description: "When"
  - note3DefaultText: "Where"
    $name: Default note 3
    $description: "Where"
  - separator1: "-"
    $name: Separator 1
    $description: "Between note 1 and note 2."
  - separator2: "-"
    $name: Separator 2
    $description: "Between note 2 and note 3."
  - gap: 34
    $name: Gap between note parts
  - rowGap: 12
    $name: Gap between rows
  - maxRows: 14
    $name: Maximum rows
    $description: "Maximum 1-14 rows."
  $name: Notes

- font:
  - family: "Segoe UI"
    $name: Font family
  - size: 45
    $name: Font size
  - bold: true
    $name: Bold
  $name: Font

- titleFont:
  - family: "Segoe UI"
    $name: Title font family
  - size: 40
    $name: Title font size
  - bold: false
    $name: Title bold
  $name: Title font

- appearance:
  - defaultColor: "#00FFFF"
    $name: Default note color
  - defaultAlpha: 127
    $name: Default transparency
  - defaultScalePercent: 100
    $name: Default scale
  $name: Appearance

- dragHotkey:
  - enable: true
    $name: Enable drag hotkey
  - ctrl: true
    $name: Require Ctrl
  - alt: true
    $name: Require Alt
  - shift: false
    $name: Require Shift
  - key: "N"
    $name: Key
  $name: Drag hotkey
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>
#include <sddl.h>

#include <algorithm>
#include <string>
#include <vector>

// clang-format off

constexpr wchar_t OVERLAY_WINDOW_CLASS_NAME[] = L"NotesOverlay_Window";
constexpr COLORREF TRANSPARENT_COLOR_KEY = RGB(1, 2, 3);

constexpr int HOTKEY_DRAG_MODE = 1;

constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;
constexpr UINT WM_APP_CLEANUP = WM_APP + 2;
constexpr UINT WM_APP_EDIT_FINISH = WM_APP + 10;

constexpr int SCALE_MIN_PERCENT = 20;
constexpr int SCALE_MAX_PERCENT = 320;
constexpr int SCALE_STEP_PERCENT = 10;
constexpr int MAX_ROWS = 14;

constexpr int NOTE_BOX_PADDING_X = 6;
constexpr int NOTE_BOX_PADDING_Y = 2;

constexpr BYTE HOVER_TEXT_ALPHA = 90;

constexpr int ROW_STATUS_OPEN = 0;
constexpr int ROW_STATUS_ACTIVE = 1;
constexpr int ROW_STATUS_DONE = 2;

struct NoteLine {
    std::wstring part1 = L"What";
    std::wstring part2 = L"When";
    std::wstring part3 = L"Where";
};

struct Settings {
    int x = 60;
    int y = 60;
    bool alwaysOnTop = false;

    std::wstring note1DefaultText = L"What";
    std::wstring note2DefaultText = L"When";
    std::wstring note3DefaultText = L"Where";
    std::wstring separator1 = L"-";
    std::wstring separator2 = L"-";
    int gap = 34;
    int rowGap = 12;
    int maxRows = MAX_ROWS;

    bool titleEnabled = true;
    std::wstring titleText = L"What When Where Notes";
    int titleGap = 18;
    std::wstring titleColorHex = L"#0000FF";

    std::wstring titleFontFamily = L"Segoe UI";
    int titleFontSize = 40;
    bool titleFontBold = false;

    std::wstring fontFamily = L"Segoe UI";
    int fontSize = 45;
    bool fontBold = true;

    COLORREF defaultColor = RGB(0, 255, 255);
    COLORREF titleColor = RGB(0, 0, 255);
    int defaultAlpha = 127;
    int defaultScalePercent = 100;

    bool dragHotkeyEnable = true;
    bool dragHotkeyCtrl = true;
    bool dragHotkeyAlt = true;
    bool dragHotkeyShift = false;
    std::wstring dragHotkeyKey = L"N";
};

Settings g_settings;

// Runtime data belongs to a Windows account, not the machine-wide mod store.
std::wstring g_userPrefix;

// Track the last configured coordinates independently from the saved drag
// position so unrelated settings changes do not reposition the overlay.
int g_configuredX = 60;
int g_configuredY = 60;

std::vector<NoteLine> g_rows;
std::vector<int> g_rowStatus;
std::wstring g_titleText;

COLORREF g_lineColor = RGB(0, 255, 255);
COLORREF g_titleColor = RGB(0, 0, 255);
int g_alpha = 127;
int g_scalePercent = 100;

HWND g_hwnd = nullptr;
HWND g_editHwnd = nullptr;

HFONT g_font = nullptr;
HFONT g_titleFont = nullptr;
UINT g_dpi = 96;

RECT g_titleRect = {};
RECT g_rowRect[MAX_ROWS] = {};
RECT g_rowNumberRect[MAX_ROWS] = {};
RECT g_rowStatusRect[MAX_ROWS] = {};
RECT g_partRect[MAX_ROWS][3] = {};
RECT g_separatorRect[MAX_ROWS][2] = {};
RECT g_addButtonRect = {};
RECT g_removeButtonRect = {};
RECT g_deleteButtonRect[MAX_ROWS] = {};

int g_editingRow = -1;
int g_editingPart = -1;
int g_hoverRow = -1;
int g_hoverPart = -1;
int g_activeRow = -1;
int g_hoverDeleteRow = -1;

bool g_dragModeEnabled = false;
bool g_dragging = false;

POINT g_dragStartMouse = {};
POINT g_dragStartWindow = {};

HANDLE g_overlayThread = nullptr;
HANDLE g_overlayThreadReady = nullptr;
DWORD g_overlayThreadId = 0;

bool g_hoverAddButton = false;
bool g_hoverRemoveButton = false;

void ApplyLayoutAndRedraw(HWND hwnd);
std::wstring MakeRowKey(int rowIndex, const wchar_t* suffix);
void SetUserIntValue(const wchar_t* keyName, int value);
int GetUserIntValue(const wchar_t* keyName, int defaultValue);

int ScaledByNoteScale(int value);
BOOL IsPointInRect(const RECT& rect, POINT pt);

void DrawRowNumber(
    HDC hdc,
    int rowIndex,
    const RECT& rect);

void DrawStatusCircle(
    HDC hdc,
    const RECT& rect,
    int status);

////////////////////////////////////////////////////////////////////////////////
// Helpers

void EnsureRowState() {
    while (static_cast<int>(g_rowStatus.size()) <
           static_cast<int>(g_rows.size())) {
        g_rowStatus.push_back(ROW_STATUS_OPEN);
    }

    while (static_cast<int>(g_rowStatus.size()) >
           static_cast<int>(g_rows.size())) {
        g_rowStatus.pop_back();
    }

    for (int& status : g_rowStatus) {
        status = std::clamp(
            status,
            ROW_STATUS_OPEN,
            ROW_STATUS_DONE);
    }
}

void SaveRowStatus() {
    SetUserIntValue(L"rowStatusCount",
        static_cast<int>(g_rowStatus.size()));

    for (size_t i = 0; i < g_rowStatus.size(); ++i) {
        std::wstring key =
            MakeRowKey(static_cast<int>(i), L"status");

        Wh_SetIntValue(key.c_str(), g_rowStatus[i]);
    }
}

void LoadRowStatus() {
    int count = std::clamp(
        GetUserIntValue(L"rowStatusCount", 1),
        1,
        MAX_ROWS);

    g_rowStatus.clear();
    g_rowStatus.resize(count, ROW_STATUS_OPEN);

    for (int i = 0; i < count; ++i) {
        std::wstring key = MakeRowKey(i, L"status");

        g_rowStatus[i] = std::clamp(
            Wh_GetIntValue(key.c_str(), ROW_STATUS_OPEN),
            ROW_STATUS_OPEN,
            ROW_STATUS_DONE);
    }

    EnsureRowState();
}

void DrawRowBackground(HDC hdc, const RECT& rect, bool active) {
    if (!active) return;

    RECT filled = rect;
    HBRUSH brush = CreateSolidBrush(RGB(180, 195, 210));
    FillRect(hdc, &filled, brush);
    DeleteObject(brush);
}

bool HitTestStatusCircle(
    POINT point,
    int rowIndex) {
    return IsPointInRect(
        g_rowStatusRect[rowIndex],
        point);
}

// A failed SID lookup must never fall back to shared machine-wide storage.
bool InitUserPrefix() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"OpenProcessToken failed: %u", GetLastError());
        return false;
    }

    alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE]{};
    DWORD bytesNeeded = 0;
    PWSTR sidText = nullptr;
    bool result = GetTokenInformation(token, TokenUser, buffer, sizeof(buffer),
                                      &bytesNeeded) != FALSE;
    if (result) {
        auto* tokenUser = reinterpret_cast<TOKEN_USER*>(buffer);
        result = ConvertSidToStringSidW(tokenUser->User.Sid, &sidText) != FALSE;
    }
    if (result && sidText) {
        g_userPrefix.assign(sidText);
        g_userPrefix.append(L"_");
        LocalFree(sidText);
    }
    CloseHandle(token);

    if (g_userPrefix.empty()) {
        Wh_Log(L"Unable to resolve user SID; refusing shared storage");
        return false;
    }
    return true;
}

std::wstring MakeUserKey(const wchar_t* keyName) {
    return g_userPrefix + keyName;
}

void SetUserIntValue(const wchar_t* keyName, int value) {
    const auto key = MakeUserKey(keyName);
    Wh_SetIntValue(key.c_str(), value);
}

int GetUserIntValue(const wchar_t* keyName, int defaultValue) {
    const auto key = MakeUserKey(keyName);
    return Wh_GetIntValue(key.c_str(), defaultValue);
}

int Scaled(int value) {
    return MulDiv(value, static_cast<int>(g_dpi), 96);
}

int ScaledByNoteScale(int value) {
    return MulDiv(Scaled(value), g_scalePercent, 100);
}

HINSTANCE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        &module);
    return module;
}

bool ParseHexColor(PCWSTR value, COLORREF* result) {
    if (!value || value[0] != L'#' || wcslen(value) != 7) {
        return false;
    }

    unsigned int r = 0, g = 0, b = 0;
    if (swscanf_s(value + 1, L"%02x%02x%02x", &r, &g, &b) != 3) {
        return false;
    }

    *result = RGB(r, g, b);
    return true;
}

std::wstring Trim(const std::wstring& value) {
    size_t first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = value.find_last_not_of(L" \t\r\n");
    return value.substr(first, last - first + 1);
}

UINT ComputeHotkeyVk(const std::wstring& key, wchar_t fallback) {
    if (key.empty()) return static_cast<UINT>(fallback);

    wchar_t ch = key[0];
    if (ch >= L'a' && ch <= L'z') ch = ch - L'a' + L'A';

    if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
        return static_cast<UINT>(ch);
    }

    return static_cast<UINT>(fallback);
}

UINT ComputeHotkeyModifiers() {
    UINT modifiers = MOD_NOREPEAT;
    if (g_settings.dragHotkeyCtrl) modifiers |= MOD_CONTROL;
    if (g_settings.dragHotkeyAlt) modifiers |= MOD_ALT;
    if (g_settings.dragHotkeyShift) modifiers |= MOD_SHIFT;
    return modifiers;
}

std::wstring MakeRowKey(int rowIndex, const wchar_t* suffix) {
    wchar_t key[64];
    swprintf_s(key, L"line%d_%s", rowIndex + 1, suffix);
    return g_userPrefix + key;
}

int ClampRowCount(int count) {
    return std::clamp(count, 1, MAX_ROWS);
}

void SaveRows() {
    SetUserIntValue(L"lineCount",
        static_cast<int>(g_rows.size()));

    for (size_t i = 0; i < g_rows.size(); ++i) {
        const NoteLine& row = g_rows[i];

        std::wstring key1 = MakeRowKey(static_cast<int>(i), L"part1");
        std::wstring key2 = MakeRowKey(static_cast<int>(i), L"part2");
        std::wstring key3 = MakeRowKey(static_cast<int>(i), L"part3");

        Wh_SetStringValue(key1.c_str(), row.part1.c_str());
        Wh_SetStringValue(key2.c_str(), row.part2.c_str());
        Wh_SetStringValue(key3.c_str(), row.part3.c_str());
    }
}

void SetDefaultRow(NoteLine& row) {
    row.part1 = g_settings.note1DefaultText;
    row.part2 = g_settings.note2DefaultText;
    row.part3 = g_settings.note3DefaultText;
}

void EnsureValidRows() {
    if (g_rows.empty()) {
        g_rows.emplace_back();
        SetDefaultRow(g_rows.back());
    }

    int maximumRows = std::clamp(
        g_settings.maxRows,
        1,
        MAX_ROWS);

    while (static_cast<int>(g_rows.size()) > maximumRows) {
        g_rows.pop_back();
    }
}

void AddRow() {
    if (static_cast<int>(g_rows.size()) >= g_settings.maxRows) {
        return;
    }

    NoteLine row;
    // Use the configurable default note text for each newly added row.
    SetDefaultRow(row);

    g_rows.push_back(row);
    g_rowStatus.push_back(ROW_STATUS_OPEN);

    SaveRows();
    SaveRowStatus();

    if (g_hwnd) {
        ApplyLayoutAndRedraw(g_hwnd);
    }
}

void RemoveRow() {
    if (g_rows.size() <= 1) {
        return;
    }

    g_rows.pop_back();

    if (!g_rowStatus.empty()) {
        g_rowStatus.pop_back();
    }

    SaveRows();
    SaveRowStatus();

    if (g_hwnd) {
        ApplyLayoutAndRedraw(g_hwnd);
    }
}

void DeleteRow(int rowIndex) {
    if (rowIndex < 0 ||
        rowIndex >= static_cast<int>(g_rows.size())) {
        return;
    }

    // Die letzte vorhandene Zeile bleibt erhalten.
    if (g_rows.size() <= 1) {
        return;
    }

    g_rows.erase(g_rows.begin() + rowIndex);

    if (rowIndex < static_cast<int>(g_rowStatus.size())) {
        g_rowStatus.erase(g_rowStatus.begin() + rowIndex);
    }

    EnsureRowState();

    SaveRows();
    SaveRowStatus();

    if (g_hwnd) {
        ApplyLayoutAndRedraw(g_hwnd);
    }
}

void LoadRows() {
    int count = std::clamp(
    GetUserIntValue(L"lineCount", 1),
    1,
    g_settings.maxRows);
    g_rows.clear();
    g_rows.resize(count);

    for (int i = 0; i < count; ++i) {
        WCHAR buffer[4096];

        std::wstring key1 = MakeRowKey(i, L"part1");
        std::wstring key2 = MakeRowKey(i, L"part2");
        std::wstring key3 = MakeRowKey(i, L"part3");

        if (Wh_GetStringValue(key1.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part1 = buffer;
        } else {
            g_rows[i].part1 = g_settings.note1DefaultText;
        }

        if (Wh_GetStringValue(key2.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part2 = buffer;
        } else {
            g_rows[i].part2 = g_settings.note2DefaultText;
        }

        if (Wh_GetStringValue(key3.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part3 = buffer;
        } else {
            g_rows[i].part3 = g_settings.note3DefaultText;
        }
    }

    EnsureValidRows();
}

void SaveAppearance() {
    SetUserIntValue(L"lineColor", static_cast<int>(g_lineColor));
    SetUserIntValue(L"alpha", g_alpha);
    SetUserIntValue(L"scalePercent", g_scalePercent);
}

void SavePosition() {
    SetUserIntValue(L"posX", g_settings.x);
    SetUserIntValue(L"posY", g_settings.y);
}

void LoadSavedPosition() {
    g_settings.x = GetUserIntValue(L"posX", g_settings.x);
    g_settings.y = GetUserIntValue(L"posY", g_settings.y);
}

////////////////////////////////////////////////////////////////////////////////
// Settings / runtime state

void LoadSettings() {
    g_settings.x = Wh_GetIntSetting(L"position.x");
    g_settings.y = Wh_GetIntSetting(L"position.y");
    g_settings.alwaysOnTop = Wh_GetIntSetting(L"position.alwaysOnTop") != 0;

    PCWSTR value = Wh_GetStringSetting(L"notes.note1DefaultText");
    g_settings.note1DefaultText = value ? value : L"";
    Wh_FreeStringSetting(value);

    value = Wh_GetStringSetting(L"notes.note2DefaultText");
    g_settings.note2DefaultText = value ? value : L"";
    Wh_FreeStringSetting(value);

    value = Wh_GetStringSetting(L"notes.note3DefaultText");
    g_settings.note3DefaultText = value ? value : L"";
    Wh_FreeStringSetting(value);

    value = Wh_GetStringSetting(L"notes.separator1");
    g_settings.separator1 = value && *value ? value : L"-";
    Wh_FreeStringSetting(value);

    value = Wh_GetStringSetting(L"notes.separator2");
    g_settings.separator2 = value && *value ? value : L"-";
    Wh_FreeStringSetting(value);

    g_settings.gap = std::max(0, Wh_GetIntSetting(L"notes.gap"));
    g_settings.rowGap = std::max(0, Wh_GetIntSetting(L"notes.rowGap"));
    g_settings.maxRows = std::clamp(
        Wh_GetIntSetting(L"notes.maxRows"),
        1,
        MAX_ROWS);

    g_settings.titleEnabled = Wh_GetIntSetting(L"title.enabled") != 0;

    value = Wh_GetStringSetting(L"title.text");
    g_settings.titleText = value && *value ? value : L"WHAT WHEN WHERE NOTES";
    Wh_FreeStringSetting(value);

    g_settings.titleGap = std::max(0, Wh_GetIntSetting(L"title.gap"));

    value = Wh_GetStringSetting(L"title.color");
    if (value && *value) {
        if (ParseHexColor(value, &g_settings.titleColor)) {
            g_titleColor = g_settings.titleColor;
        }
    }
    Wh_FreeStringSetting(value);

    value = Wh_GetStringSetting(L"font.family");
    g_settings.fontFamily = value && *value ? value : L"Segoe UI";
    Wh_FreeStringSetting(value);

    g_settings.fontSize = std::clamp(Wh_GetIntSetting(L"font.size"), 8, 200);
    g_settings.fontBold = Wh_GetIntSetting(L"font.bold") != 0;

    value = Wh_GetStringSetting(L"titleFont.family");
    g_settings.titleFontFamily = value && *value ? value : L"Segoe UI";
    Wh_FreeStringSetting(value);

    g_settings.titleFontSize = std::clamp(Wh_GetIntSetting(L"titleFont.size"), 8, 200);
    g_settings.titleFontBold = Wh_GetIntSetting(L"titleFont.bold") != 0;

    value = Wh_GetStringSetting(L"appearance.defaultColor");
    if (!ParseHexColor(value, &g_settings.defaultColor)) {
        g_settings.defaultColor = RGB(0, 255, 255);
    }
    Wh_FreeStringSetting(value);

    g_settings.defaultAlpha = std::clamp(Wh_GetIntSetting(L"appearance.defaultAlpha"), 0, 255);
    g_settings.defaultScalePercent = std::clamp(
        Wh_GetIntSetting(L"appearance.defaultScalePercent"),
        SCALE_MIN_PERCENT, SCALE_MAX_PERCENT);

    g_settings.dragHotkeyEnable = Wh_GetIntSetting(L"dragHotkey.enable") != 0;
    g_settings.dragHotkeyCtrl = Wh_GetIntSetting(L"dragHotkey.ctrl") != 0;
    g_settings.dragHotkeyAlt = Wh_GetIntSetting(L"dragHotkey.alt") != 0;
    g_settings.dragHotkeyShift = Wh_GetIntSetting(L"dragHotkey.shift") != 0;

    value = Wh_GetStringSetting(L"dragHotkey.key");
    g_settings.dragHotkeyKey = value && *value ? value : L"N";
    Wh_FreeStringSetting(value);

    g_titleText = g_settings.titleText;
}

void LoadRuntimeState() {
    int count = std::clamp(
        GetUserIntValue(L"lineCount", 1),
        1,
        g_settings.maxRows);

    g_rows.clear();
    g_rows.resize(count);

g_rowStatus.clear();
g_rowStatus.resize(count, ROW_STATUS_OPEN);

    for (int i = 0; i < count; ++i) {
        WCHAR buffer[4096];

        std::wstring key1 = MakeRowKey(i, L"part1");
        std::wstring key2 = MakeRowKey(i, L"part2");
        std::wstring key3 = MakeRowKey(i, L"part3");

        if (Wh_GetStringValue(key1.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part1 = buffer;
        } else {
            g_rows[i].part1 = g_settings.note1DefaultText;
        }

        if (Wh_GetStringValue(key2.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part2 = buffer;
        } else {
            g_rows[i].part2 = g_settings.note2DefaultText;
        }

        if (Wh_GetStringValue(key3.c_str(), buffer, ARRAYSIZE(buffer))) {
            g_rows[i].part3 = buffer;
        } else {
            g_rows[i].part3 = g_settings.note3DefaultText;
        }
    }

    LoadRowStatus();
    EnsureValidRows();
    EnsureRowState();

    int savedColor = GetUserIntValue(L"lineColor", -1);
    g_lineColor = (savedColor >= 0) ? static_cast<COLORREF>(savedColor) : g_settings.defaultColor;

    g_alpha = std::clamp(GetUserIntValue(L"alpha", g_settings.defaultAlpha), 0, 255);
    g_scalePercent = std::clamp(
        GetUserIntValue(L"scalePercent", g_settings.defaultScalePercent),
        SCALE_MIN_PERCENT, SCALE_MAX_PERCENT);
}

////////////////////////////////////////////////////////////////////////////////
// Fonts

void RecreateFonts() {
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }

    if (g_titleFont) {
        DeleteObject(g_titleFont);
        g_titleFont = nullptr;
    }

    int noteHeight = -MulDiv(g_settings.fontSize, static_cast<int>(g_dpi) * g_scalePercent, 72 * 100);
    g_font = CreateFontW(
        noteHeight, 0, 0, 0,
        g_settings.fontBold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        g_settings.fontFamily.c_str());

    int titleHeight = -MulDiv(g_settings.titleFontSize, static_cast<int>(g_dpi) * g_scalePercent, 72 * 100);
    g_titleFont = CreateFontW(
        titleHeight, 0, 0, 0,
        g_settings.titleFontBold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        g_settings.titleFontFamily.c_str());
}

SIZE MeasureText(HDC hdc, HFONT font, const std::wstring& text) {
    std::wstring displayText = text.empty() ? L" " : text;
    SIZE size{};
    HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
    GetTextExtentPoint32W(hdc, displayText.c_str(), static_cast<int>(displayText.length()), &size);
    SelectObject(hdc, oldFont);
    return size;
}

RECT GetButtonRectForAdd() {
    RECT r{};

    int size = ScaledByNoteScale(28);
    int left = ScaledByNoteScale(12);
    int top = ScaledByNoteScale(4);

    r.left = left;
    r.top = top;
    r.right = left + size;
    r.bottom = top + size;

    return r;
}

RECT GetButtonRectForRemove() {
    RECT r{};

    int size = ScaledByNoteScale(28);
    int left = ScaledByNoteScale(52);
    int top = ScaledByNoteScale(4);

    r.left = left;
    r.top = top;
    r.right = left + size;
    r.bottom = top + size;

    return r;
}

RECT GetNoteBoxRect(int rowIndex, int partIndex) {
    RECT r = g_partRect[rowIndex][partIndex];
    InflateRect(&r, ScaledByNoteScale(NOTE_BOX_PADDING_X), ScaledByNoteScale(NOTE_BOX_PADDING_Y));
    return r;
}

BOOL IsPointInRect(const RECT& rect, POINT pt) {
    return pt.x >= rect.left && pt.x < rect.right &&
           pt.y >= rect.top && pt.y < rect.bottom;
}

////////////////////////////////////////////////////////////////////////////////
// Layout

SIZE ComputeLayout(HDC hdc) {
    int marginX = ScaledByNoteScale(6);
    int marginY = ScaledByNoteScale(6);
    int rowGap = ScaledByNoteScale(g_settings.rowGap);

    SIZE total{};
    total.cx = 0;
    total.cy = 0;

    int currentY = marginY;

    if (g_settings.titleEnabled) {
        HFONT oldTitle = static_cast<HFONT>(SelectObject(hdc, g_titleFont));
        SIZE titleSize = MeasureText(hdc, g_titleFont, g_titleText);
        g_titleRect = {
            marginX,
            currentY,
            marginX + titleSize.cx,
            currentY + titleSize.cy
        };

        int rowWidth = titleSize.cx;
        int titleLeft = marginX + (rowWidth - titleSize.cx) / 2;
        g_titleRect.left = titleLeft;
        g_titleRect.right = titleLeft + titleSize.cx;

        total.cx = std::max(total.cx, titleSize.cx + marginX * 2);
        currentY += titleSize.cy + ScaledByNoteScale(g_settings.titleGap);
        SelectObject(hdc, oldTitle);
    } else {
        g_titleRect = {};
    }

    g_addButtonRect = GetButtonRectForAdd();
    g_removeButtonRect = GetButtonRectForRemove();

    constexpr int BUTTON_VERTICAL_OFFSET = -6;

    g_addButtonRect.top += currentY + ScaledByNoteScale(BUTTON_VERTICAL_OFFSET);
    g_addButtonRect.bottom += currentY + ScaledByNoteScale(BUTTON_VERTICAL_OFFSET);

    g_removeButtonRect.top += currentY + ScaledByNoteScale(BUTTON_VERTICAL_OFFSET);
    g_removeButtonRect.bottom += currentY + ScaledByNoteScale(BUTTON_VERTICAL_OFFSET);

    // Abstand zwischen Buttons und erster Notizzeile bleibt unverändert.
    currentY += ScaledByNoteScale(36);

for (int rowIndex = 0; rowIndex < static_cast<int>(g_rows.size()); ++rowIndex) {
    NoteLine& row = g_rows[rowIndex];

    // Textmaße für die Zeile
    SIZE s1 = MeasureText(hdc, g_font, row.part1);

    // etwas mehr Platz zwischen Nummer und Checkbox
    int numberWidth = ScaledByNoteScale(30);
    int statusSize = ScaledByNoteScale(20);

    int numberToStatusGap = ScaledByNoteScale(12);
    int statusToTextGap = ScaledByNoteScale(20);

    // Links- und Mittelpositionen
    LONG baseLeft = static_cast<LONG>(marginX);
    LONG numberX = baseLeft + 2;
    LONG statusX = numberX + numberWidth + numberToStatusGap;
    LONG textX = statusX + statusSize + statusToTextGap;

    // Nummer vertikal mittig zur Textzeile
// Zeilennummer etwas nach oben korrigieren.
// Die sichtbaren Ziffern liegen innerhalb ihrer Text-RECT
// optisch etwas tiefer.
LONG numberHeight =
    static_cast<LONG>(ScaledByNoteScale(24));

LONG numberOffset =
    std::max<LONG>(
        0L,
        (s1.cy - numberHeight) / 2);

LONG numberVerticalCorrection =
    static_cast<LONG>(ScaledByNoteScale(27));

LONG numberTop =
    static_cast<LONG>(currentY) +
    numberOffset -
    numberVerticalCorrection;

// Kreis etwas nach unten verschieben,
// damit er optisch mittig zur sichtbaren Texthöhe sitzt.
LONG statusSizeLong =
    static_cast<LONG>(statusSize);

LONG statusOffset =
    std::max<LONG>(
        0L,
        (s1.cy - statusSizeLong) / 2);

LONG statusVerticalCorrection =
    static_cast<LONG>(ScaledByNoteScale(6));

LONG statusTop =
    static_cast<LONG>(currentY) +
    statusOffset +
    statusVerticalCorrection;

    g_rowNumberRect[rowIndex] = {
        numberX,
        numberTop,
        numberX + numberWidth,
        numberTop + numberHeight
    };

    g_rowStatusRect[rowIndex] = {
        statusX,
        statusTop,
        statusX + statusSize,
        statusTop + statusSize
    };

    int x = static_cast<int>(textX);

    // Part 1
    g_partRect[rowIndex][0] = {
        x,
        currentY,
        x + s1.cx,
        currentY + s1.cy
    };
    x = g_partRect[rowIndex][0].right +
        ScaledByNoteScale(g_settings.gap);

    // Separator 1
    SIZE sep1 = MeasureText(hdc, g_font, g_settings.separator1);
    g_separatorRect[rowIndex][0] = {
        x,
        currentY,
        x + sep1.cx,
        currentY + sep1.cy
    };
    x = g_separatorRect[rowIndex][0].right +
        ScaledByNoteScale(g_settings.gap);

    // Part 2
    SIZE s2 = MeasureText(hdc, g_font, row.part2);
    g_partRect[rowIndex][1] = {
        x,
        currentY,
        x + s2.cx,
        currentY + s2.cy
    };
    x = g_partRect[rowIndex][1].right +
        ScaledByNoteScale(g_settings.gap);

    // Separator 2
    SIZE sep2 = MeasureText(hdc, g_font, g_settings.separator2);
    g_separatorRect[rowIndex][1] = {
        x,
        currentY,
        x + sep2.cx,
        currentY + sep2.cy
    };
    x = g_separatorRect[rowIndex][1].right +
        ScaledByNoteScale(g_settings.gap);

    // Part 3
    SIZE s3 = MeasureText(hdc, g_font, row.part3);
    g_partRect[rowIndex][2] = {
        x,
        currentY,
        x + s3.cx,
        currentY + s3.cy
    };

    int rowRight = g_partRect[rowIndex][2].right;

    int deleteButtonSize =
        ScaledByNoteScale(28);

    int deleteButtonGap =
        ScaledByNoteScale(18);

    int deleteButtonLeft =
        rowRight + deleteButtonGap;

    int deleteButtonTop =
        currentY +
        std::max(
            0,
            (static_cast<int>(s1.cy) - deleteButtonSize) / 2);

    g_deleteButtonRect[rowIndex] = {
        deleteButtonLeft,
        deleteButtonTop,
        deleteButtonLeft + deleteButtonSize,
        deleteButtonTop + deleteButtonSize
    };

    int fullRowRight =
        g_deleteButtonRect[rowIndex].right;

    int absoluteRowBottom = std::max({
        static_cast<int>(g_partRect[rowIndex][0].bottom),
        static_cast<int>(g_partRect[rowIndex][1].bottom),
        static_cast<int>(g_partRect[rowIndex][2].bottom),
        static_cast<int>(g_separatorRect[rowIndex][0].bottom),
        static_cast<int>(g_separatorRect[rowIndex][1].bottom)
    });

    int rowHeight = absoluteRowBottom - currentY;

    g_rowRect[rowIndex] = {
        baseLeft,
        currentY,
        fullRowRight + marginX,
        currentY + rowHeight
    };

    total.cx = std::max(
        static_cast<LONG>(total.cx),
        static_cast<LONG>(fullRowRight + marginX * 2));

    currentY += rowHeight + rowGap;
}

    // Correct title centering based on actual row width
    if (g_settings.titleEnabled) {
        int fullRowWidth = total.cx;
        int titleWidth = g_titleRect.right - g_titleRect.left;
        int titleLeft = (fullRowWidth - titleWidth) / 2;
        g_titleRect.left = titleLeft;
        g_titleRect.right = titleLeft + titleWidth;
    }

    total.cx = std::max(
        static_cast<LONG>(total.cx),
        static_cast<LONG>(200));
    total.cy = currentY + marginY;

    // Leave some room for the controls.
    if (total.cy < 120) {
        total.cy = 140;
    }

    return total;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing
COLORREF ApplyHoverAlpha(COLORREF color, BYTE alpha) {
    BYTE red = static_cast<BYTE>(
        MulDiv(GetRValue(color), alpha, 255));

    BYTE green = static_cast<BYTE>(
        MulDiv(GetGValue(color), alpha, 255));

    BYTE blue = static_cast<BYTE>(
        MulDiv(GetBValue(color), alpha, 255));

    return RGB(red, green, blue);
}

void DrawOutlinedText(
    HDC hdc,
    const std::wstring& text,
    const RECT& rect,
    COLORREF fill,
    COLORREF outline,
    bool strikeThrough = false) {
    if (text.empty()) {
        return;
    }

    SetBkMode(hdc, TRANSPARENT);

    static const POINT offsets[] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1, 1}
    };

    SetTextColor(hdc, outline);

    for (const POINT& offset : offsets) {
        TextOutW(
            hdc,
            rect.left + offset.x,
            rect.top + offset.y,
            text.c_str(),
            static_cast<int>(text.length()));
    }

    SetTextColor(hdc, fill);

    TextOutW(
        hdc,
        rect.left,
        rect.top,
        text.c_str(),
        static_cast<int>(text.length()));

    if (strikeThrough) {
        SIZE textSize{};
        GetTextExtentPoint32W(
            hdc,
            text.c_str(),
            static_cast<int>(text.length()),
            &textSize);

        int strikeY =
            rect.top + textSize.cy / 2;

        HPEN strikePen = CreatePen(
            PS_SOLID,
            std::max(1, ScaledByNoteScale(2)),
            outline);

        HPEN oldPen =
            static_cast<HPEN>(SelectObject(hdc, strikePen));

        MoveToEx(
            hdc,
            rect.left,
            strikeY,
            nullptr);

        LineTo(
            hdc,
            rect.left + textSize.cx,
            strikeY);

        SelectObject(hdc, oldPen);
        DeleteObject(strikePen);
    }
}

void DrawSoftHoverGradient(HDC hdc, const RECT& rect) {
    int height = rect.bottom - rect.top;
    if (height <= 0) return;

    const COLORREF topColor = RGB(180, 190, 220);
    const COLORREF bottomColor = RGB(130, 155, 200);

    for (int y = rect.top; y < rect.bottom; ++y) {
        int t = y - rect.top;
        BYTE r = static_cast<BYTE>(GetRValue(topColor) + MulDiv(GetRValue(bottomColor) - GetRValue(topColor), t, height));
        BYTE g = static_cast<BYTE>(GetGValue(topColor) + MulDiv(GetGValue(bottomColor) - GetGValue(topColor), t, height));
        BYTE b = static_cast<BYTE>(GetBValue(topColor) + MulDiv(GetBValue(bottomColor) - GetBValue(topColor), t, height));

        RECT lineRect{rect.left, y, rect.right, y + 1};
        HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
        FillRect(hdc, &lineRect, brush);
        DeleteObject(brush);
    }
}

void DrawButton(
    HDC hdc,
    const RECT& rect,
    bool addButton,
    bool active) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (width <= 0 || height <= 0) {
        return;
    }

    COLORREF background =
        active ? RGB(235, 235, 235) : RGB(255, 255, 255);

    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(hdc, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);

    HPEN borderPen = CreatePen(
        PS_SOLID,
        std::max(1, ScaledByNoteScale(1)),
        RGB(0, 0, 0));

    HPEN oldPen =
        static_cast<HPEN>(SelectObject(hdc, borderPen));

    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));

    Rectangle(
        hdc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom);

    int centerX = rect.left + width / 2;
    int centerY = rect.top + height / 2;

    int symbolHalfWidth = std::max(
        ScaledByNoteScale(7),
        width / 4);

    int symbolHalfHeight = std::max(
        ScaledByNoteScale(7),
        height / 4);

    HPEN symbolPen = CreatePen(
        PS_SOLID,
        std::max(2, ScaledByNoteScale(2)),
        RGB(0, 0, 0));

    SelectObject(hdc, symbolPen);

    // Horizontale Linie: sowohl für + als auch für -
    MoveToEx(
        hdc,
        centerX - symbolHalfWidth,
        centerY,
        nullptr);

    LineTo(
        hdc,
        centerX + symbolHalfWidth + 1,
        centerY);

    // Vertikale Linie nur beim Pluszeichen
    if (addButton) {
        MoveToEx(
            hdc,
            centerX,
            centerY - symbolHalfHeight,
            nullptr);

        LineTo(
            hdc,
            centerX,
            centerY + symbolHalfHeight + 1);
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(symbolPen);
    DeleteObject(borderPen);
}

void DrawDeleteButton(
    HDC hdc,
    const RECT& rect,
    bool active) {
    COLORREF fillColor =
        active ? RGB(235, 235, 235) : RGB(255, 255, 255);

    HBRUSH fillBrush = CreateSolidBrush(fillColor);
    HPEN borderPen = CreatePen(
        PS_SOLID,
        std::max(1, ScaledByNoteScale(2)),
        RGB(0, 0, 0));

    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, fillBrush));

    HPEN oldPen =
        static_cast<HPEN>(SelectObject(hdc, borderPen));

    Ellipse(
        hdc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom);

    int centerX = (rect.left + rect.right) / 2;
    int centerY = (rect.top + rect.bottom) / 2;

    int rectWidth =
        static_cast<int>(rect.right - rect.left);

    int inset = std::max<int>(
        ScaledByNoteScale(6),
        rectWidth / 4);

    HPEN xPen = CreatePen(
        PS_SOLID,
        std::max(2, ScaledByNoteScale(2)),
        RGB(0, 0, 0));

    SelectObject(hdc, xPen);

    MoveToEx(
        hdc,
        centerX - inset,
        centerY - inset,
        nullptr);

    LineTo(
        hdc,
        centerX + inset,
        centerY + inset);

    MoveToEx(
        hdc,
        centerX - inset,
        centerY + inset,
        nullptr);

    LineTo(
        hdc,
        centerX + inset,
        centerY - inset);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(xPen);
    DeleteObject(borderPen);
    DeleteObject(fillBrush);
}

void DrawStatusCircle(
    HDC hdc,
    const RECT& rect,
    int status) {
    COLORREF color = RGB(150, 150, 150);

    if (status == ROW_STATUS_ACTIVE) {
        color = RGB(255, 0, 0);
    } else if (status == ROW_STATUS_DONE) {
        color = RGB(0, 255, 0);
    }

    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(
        PS_SOLID,
        std::max(1, ScaledByNoteScale(1)),
        RGB(0, 0, 0));

    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, brush));

    HPEN oldPen =
        static_cast<HPEN>(SelectObject(hdc, pen));

    Ellipse(
        hdc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(brush);
    DeleteObject(pen);
}

void DrawRowNumber(
    HDC hdc,
    int rowIndex,
    const RECT& rect) {
    wchar_t number[16]{};

    swprintf_s(
        number,
        L"%d",
        rowIndex + 1);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(230, 230, 230));

    TextOutW(
        hdc,
        rect.left,
        rect.top,
        number,
        static_cast<int>(wcslen(number)));
}

void PreparePerPixelAlpha(void* bitmapBits, int width, int height) {
    if (!bitmapBits || width <= 0 || height <= 0) return;

    auto* pixels = static_cast<unsigned char*>(bitmapBits);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char* pixel = pixels + (y * width + x) * 4;

            unsigned char blue = pixel[0];
            unsigned char green = pixel[1];
            unsigned char red = pixel[2];

            if (RGB(red, green, blue) == TRANSPARENT_COLOR_KEY) {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel[3] = 0;
            } else {
                pixel[3] = 255;
            }
        }
    }
}

void RenderOverlay(HWND hwnd) {
    if (!IsWindow(hwnd) || !g_font || !g_titleFont) return;

    RECT clientRect{};
    GetClientRect(hwnd, &clientRect);

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    if (width <= 0 || height <= 0) return;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC) return;

    HDC memDC = CreateCompatibleDC(screenDC);
    if (!memDC) {
        ReleaseDC(nullptr, screenDC);
        return;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(memDC);
        ReleaseDC(nullptr, screenDC);
        return;
    }

    HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(memDC, bitmap));
    HFONT oldFont = static_cast<HFONT>(SelectObject(memDC, g_font));
    HFONT oldTitleFont = static_cast<HFONT>(SelectObject(memDC, g_titleFont));

    HBRUSH bgBrush = CreateSolidBrush(TRANSPARENT_COLOR_KEY);
    RECT fillRect{0, 0, width, height};
    FillRect(memDC, &fillRect, bgBrush);
    DeleteObject(bgBrush);

    // hover highlight line
    for (int i = 0; i < static_cast<int>(g_rows.size()); ++i) {
        if (i == g_hoverRow) {
            RECT hoverRect = g_rowRect[i];
            DrawSoftHoverGradient(memDC, hoverRect);
        }
    }

    // title
    if (g_settings.titleEnabled) {
        SelectObject(memDC, g_titleFont);
        DrawOutlinedText(memDC, g_titleText, g_titleRect, g_titleColor, RGB(0, 0, 0));
    }

 // rows
SelectObject(memDC, g_font);

for (int rowIndex = 0;
     rowIndex < static_cast<int>(g_rows.size());
     ++rowIndex) {
    NoteLine& row = g_rows[rowIndex];

    // Hintergrund zuerst zeichnen, damit Text und Statusanzeige
    // anschließend darüber gezeichnet werden.
    if (g_hoverRow == rowIndex ||
        g_activeRow == rowIndex) {
        DrawRowBackground(
            memDC,
            g_rowRect[rowIndex],
            true);
    }

    // Zeilennummer zeichnen: 1 bis maximal 14.
    DrawRowNumber(
        memDC,
        rowIndex,
        g_rowNumberRect[rowIndex]);

    // Statuskreis zeichnen:
    // Grau = offen, Rot = in Bearbeitung, Grün = erledigt.
    DrawStatusCircle(
        memDC,
        g_rowStatusRect[rowIndex],
        g_rowStatus[rowIndex]);

    DrawDeleteButton(
        memDC,
        g_deleteButtonRect[rowIndex],
        g_hoverDeleteRow == rowIndex);

    COLORREF part1Color = g_lineColor;
    COLORREF part2Color = g_lineColor;
    COLORREF part3Color = g_lineColor;

    COLORREF outline1Color = RGB(0, 0, 0);
    COLORREF outline2Color = RGB(0, 0, 0);
    COLORREF outline3Color = RGB(0, 0, 0);

    // Nur das aktuell gehoverte Textfeld abschwächen.
    if (g_hoverRow == rowIndex &&
        g_hoverPart == 0) {
        part1Color = ApplyHoverAlpha(
            g_lineColor,
            HOVER_TEXT_ALPHA);

        outline1Color = ApplyHoverAlpha(
            RGB(0, 0, 0),
            HOVER_TEXT_ALPHA);
    }

    if (g_hoverRow == rowIndex &&
        g_hoverPart == 1) {
        part2Color = ApplyHoverAlpha(
            g_lineColor,
            HOVER_TEXT_ALPHA);

        outline2Color = ApplyHoverAlpha(
            RGB(0, 0, 0),
            HOVER_TEXT_ALPHA);
    }

    if (g_hoverRow == rowIndex &&
        g_hoverPart == 2) {
        part3Color = ApplyHoverAlpha(
            g_lineColor,
            HOVER_TEXT_ALPHA);

        outline3Color = ApplyHoverAlpha(
            RGB(0, 0, 0),
            HOVER_TEXT_ALPHA);
    }

    // Bei grünem Status werden alle drei Notizfelder
    // durchgestrichen.
    bool rowDone =
        g_rowStatus[rowIndex] == ROW_STATUS_DONE;

    DrawOutlinedText(
        memDC,
        row.part1,
        g_partRect[rowIndex][0],
        part1Color,
        outline1Color,
        rowDone);

    DrawOutlinedText(
        memDC,
        g_settings.separator1,
        g_separatorRect[rowIndex][0],
        g_lineColor,
        RGB(0, 0, 0));

    DrawOutlinedText(
        memDC,
        row.part2,
        g_partRect[rowIndex][1],
        part2Color,
        outline2Color,
        rowDone);

    DrawOutlinedText(
        memDC,
        g_settings.separator2,
        g_separatorRect[rowIndex][1],
        g_lineColor,
        RGB(0, 0, 0));

    DrawOutlinedText(
        memDC,
        row.part3,
        g_partRect[rowIndex][2],
        part3Color,
        outline3Color,
        rowDone);
}

    // add / remove buttons
    DrawButton(
        memDC,
        g_addButtonRect,
        true,
        g_hoverAddButton);

    DrawButton(
        memDC,
        g_removeButtonRect,
        false,
        g_hoverRemoveButton);

    PreparePerPixelAlpha(bits, width, height);

    SelectObject(memDC, oldTitleFont);
    SelectObject(memDC, oldFont);

    RECT windowRect{};
    GetWindowRect(hwnd, &windowRect);
    POINT destination{windowRect.left, windowRect.top};
    POINT source{0, 0};
    SIZE size{width, height};

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = static_cast<BYTE>(g_alpha);
    blend.AlphaFormat = AC_SRC_ALPHA;

    BOOL updated = UpdateLayeredWindow(
        hwnd,
        screenDC,
        &destination,
        &size,
        memDC,
        &source,
        0,
        &blend,
        ULW_ALPHA);

    if (!updated) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }

    SelectObject(memDC, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

void PaintOverlay(HWND hwnd) {
    PAINTSTRUCT ps{};
    BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    RenderOverlay(hwnd);
}

void ApplyLayoutAndRedraw(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    HDC hdc = GetDC(hwnd);
    if (!hdc) return;

    SIZE size = ComputeLayout(hdc);
    ReleaseDC(hwnd, hdc);

    SetWindowPos(hwnd, nullptr, g_settings.x, g_settings.y, size.cx, size.cy,
        SWP_NOZORDER | SWP_NOACTIVATE);

    RenderOverlay(hwnd);
    RedrawWindow(hwnd, nullptr, nullptr, RDW_VALIDATE | RDW_NOERASE | RDW_NOINTERNALPAINT);
}

////////////////////////////////////////////////////////////////////////////////
// Editing

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN) {
                SendMessageW(GetParent(hwnd), WM_APP_EDIT_FINISH, TRUE, 0);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                SendMessageW(GetParent(hwnd), WM_APP_EDIT_FINISH, FALSE, 0);
                return 0;
            }
            break;

        case WM_KILLFOCUS:
            PostMessageW(GetParent(hwnd), WM_APP_EDIT_FINISH, TRUE, 0);
            break;
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

void BeginEdit(HWND hwnd, int rowIndex, int partIndex) {
    if (g_editHwnd || rowIndex < 0 || rowIndex >= static_cast<int>(g_rows.size())) return;
    if (partIndex < 0 || partIndex > 2) return;

    RECT overlayRect{};
    GetWindowRect(hwnd, &overlayRect);

    RECT noteBox = GetNoteBoxRect(rowIndex, partIndex);

    int padX = ScaledByNoteScale(3);
    int padY = ScaledByNoteScale(2);

    int x = overlayRect.left + noteBox.left + padX;
    int y = overlayRect.top + noteBox.top + padY;

    int width = noteBox.right - noteBox.left - padX * 2;
    int height = noteBox.bottom - noteBox.top + ScaledByNoteScale(4);

    g_editingRow = rowIndex;
    g_editingPart = partIndex;

    g_editHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,
        L"EDIT",
        g_rows[rowIndex].part1.c_str(), // placeholder
        WS_POPUP | ES_AUTOHSCROLL | ES_NOHIDESEL,
        x,
        y,
        width,
        height,
        hwnd,
        nullptr,
        GetCurrentModuleHandle(),
        nullptr);

    if (!g_editHwnd) {
        Wh_Log(L"CreateWindowExW EDIT failed: %u", GetLastError());
        g_editingRow = -1;
        g_editingPart = -1;
        return;
    }

    std::wstring text = (partIndex == 0) ? g_rows[rowIndex].part1 :
                        (partIndex == 1) ? g_rows[rowIndex].part2 :
                                          g_rows[rowIndex].part3;

    SetWindowTextW(g_editHwnd, text.c_str());

    SendMessageW(g_editHwnd, WM_SETFONT,
        reinterpret_cast<WPARAM>(g_font), TRUE);

    SendMessageW(g_editHwnd, EM_SETMARGINS,
        EC_LEFTMARGIN | EC_RIGHTMARGIN,
        MAKELONG(ScaledByNoteScale(2), ScaledByNoteScale(2)));

    RECT editRect{
        ScaledByNoteScale(2),
        ScaledByNoteScale(1),
        width - ScaledByNoteScale(2),
        height - ScaledByNoteScale(1)
    };

    SendMessageW(g_editHwnd, EM_SETRECT,
        0,
        reinterpret_cast<LPARAM>(&editRect));

    SetWindowSubclass(g_editHwnd, EditSubclassProc, 0, 0);
    ShowWindow(g_editHwnd, SW_SHOWNOACTIVATE);
    SetForegroundWindow(g_editHwnd);
    SetFocus(g_editHwnd);

    int textLength = GetWindowTextLengthW(g_editHwnd);
    Edit_SetSel(g_editHwnd, textLength, textLength);
}

void EndEdit(HWND hwnd, bool commit) {
    if (!g_editHwnd || g_editingRow < 0 || g_editingPart < 0) return;

    int rowIndex = g_editingRow;
    int partIndex = g_editingPart;

    if (commit) {
        WCHAR buffer[4096]{};
        GetWindowTextW(g_editHwnd, buffer, ARRAYSIZE(buffer));
        std::wstring newText = Trim(buffer);

        if (newText.empty()) {
            MessageBeep(MB_ICONERROR);
        } else {
            if (partIndex == 0) g_rows[rowIndex].part1 = newText;
            if (partIndex == 1) g_rows[rowIndex].part2 = newText;
            if (partIndex == 2) g_rows[rowIndex].part3 = newText;

            SaveRows();
        }
    }

    HWND editHwnd = g_editHwnd;
    g_editHwnd = nullptr;
    g_editingRow = -1;
    g_editingPart = -1;

    RemoveWindowSubclass(editHwnd, EditSubclassProc, 0);
    DestroyWindow(editHwnd);

    SetFocus(hwnd);
    ApplyLayoutAndRedraw(hwnd);
}

////////////////////////////////////////////////////////////////////////////////
// Context menu

enum MenuId : UINT {
    ID_TRANSPARENCY_1 = 2001,
    ID_TRANSPARENCY_2,
    ID_TRANSPARENCY_3,
    ID_TRANSPARENCY_4,

    ID_COLOR_RED,
    ID_COLOR_GREEN,
    ID_COLOR_BLUE,
    ID_COLOR_YELLOW,
    ID_COLOR_GREY,
    ID_COLOR_MAGENTA,
    ID_COLOR_SILVER,
    ID_COLOR_WHITE,
    ID_COLOR_PURPLE,
    ID_COLOR_ORANGE,
    ID_COLOR_DEFAULT
};

void ShowContextMenu(HWND hwnd, POINT screenPoint) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    AppendMenuW(menu, MF_STRING, ID_TRANSPARENCY_1, L"Transparency 1");
    AppendMenuW(menu, MF_STRING, ID_TRANSPARENCY_2, L"Transparency 2");
    AppendMenuW(menu, MF_STRING, ID_TRANSPARENCY_3, L"Transparency 3");
    AppendMenuW(menu, MF_STRING, ID_TRANSPARENCY_4, L"Transparency 4");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(menu, MF_STRING, ID_COLOR_RED, L"Color RED");
    AppendMenuW(menu, MF_STRING, ID_COLOR_GREEN, L"Color GREEN");
    AppendMenuW(menu, MF_STRING, ID_COLOR_BLUE, L"Color BLUE");
    AppendMenuW(menu, MF_STRING, ID_COLOR_YELLOW, L"Color YELLOW");
    AppendMenuW(menu, MF_STRING, ID_COLOR_GREY, L"Color GREY");
    AppendMenuW(menu, MF_STRING, ID_COLOR_MAGENTA, L"Color MAGENTA");
    AppendMenuW(menu, MF_STRING, ID_COLOR_SILVER, L"Color SILVER");
    AppendMenuW(menu, MF_STRING, ID_COLOR_WHITE, L"Color WHITE");
    AppendMenuW(menu, MF_STRING, ID_COLOR_PURPLE, L"Color PURPLE");
    AppendMenuW(menu, MF_STRING, ID_COLOR_ORANGE, L"Color ORANGE");
    AppendMenuW(menu, MF_STRING, ID_COLOR_DEFAULT, L"Color DEFAULT");

    SetForegroundWindow(hwnd);
    UINT id = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
        screenPoint.x, screenPoint.y, 0, hwnd, nullptr);

    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    switch (id) {
        case ID_TRANSPARENCY_1: g_alpha = 255; break;
        case ID_TRANSPARENCY_2: g_alpha = 127; break;
        case ID_TRANSPARENCY_3: g_alpha = 101; break;
        case ID_TRANSPARENCY_4: g_alpha = 25; break;

        case ID_COLOR_RED: g_lineColor = RGB(255, 0, 0); break;
        case ID_COLOR_GREEN: g_lineColor = RGB(0, 255, 0); break;
        case ID_COLOR_BLUE: g_lineColor = RGB(0, 0, 255); break;
        case ID_COLOR_YELLOW: g_lineColor = RGB(255, 255, 0); break;
        case ID_COLOR_GREY: g_lineColor = RGB(100, 100, 100); break;
        case ID_COLOR_MAGENTA: g_lineColor = RGB(255, 0, 255); break;
        case ID_COLOR_SILVER: g_lineColor = RGB(192, 192, 192); break;
        case ID_COLOR_WHITE: g_lineColor = RGB(255, 255, 255); break;
        case ID_COLOR_PURPLE: g_lineColor = RGB(128, 0, 128); break;
        case ID_COLOR_ORANGE: g_lineColor = RGB(255, 165, 0); break;
        case ID_COLOR_DEFAULT: g_lineColor = g_settings.defaultColor; break;
        default: return;
    }

    SaveAppearance();
    RenderOverlay(hwnd);
}

////////////////////////////////////////////////////////////////////////////////
// Scale and drag

void ApplyScaleDelta(HWND hwnd, int deltaPercent) {
    g_scalePercent = std::clamp(g_scalePercent + deltaPercent, SCALE_MIN_PERCENT, SCALE_MAX_PERCENT);
    RecreateFonts();
    SaveAppearance();
    ApplyLayoutAndRedraw(hwnd);
}

void ResetScale(HWND hwnd) {
    g_scalePercent = g_settings.defaultScalePercent;
    RecreateFonts();
    SaveAppearance();
    ApplyLayoutAndRedraw(hwnd);
}

void ToggleDragMode() {
    g_dragModeEnabled = !g_dragModeEnabled;
    Wh_Log(L"Drag mode: %s", g_dragModeEnabled ? L"ON" : L"OFF");
    SetCursor(LoadCursor(nullptr, g_dragModeEnabled ? IDC_SIZEALL : IDC_ARROW));
}

void RegisterDragHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_DRAG_MODE);
    if (!g_settings.dragHotkeyEnable) return;

    if (!RegisterHotKey(hwnd, HOTKEY_DRAG_MODE, ComputeHotkeyModifiers(),
        ComputeHotkeyVk(g_settings.dragHotkeyKey, L'N'))) {
        Wh_Log(L"RegisterHotKey failed: %u", GetLastError());
    }
}

int HitTestRowPart(POINT point) {
    for (int rowIndex = 0; rowIndex < static_cast<int>(g_rows.size()); ++rowIndex) {
        for (int partIndex = 0; partIndex < 3; ++partIndex) {
            if (IsPointInRect(GetNoteBoxRect(rowIndex, partIndex), point)) {
                return rowIndex * 10 + partIndex;
            }
        }
    }
    return -1;
}

bool HitTestAddButton(POINT point) {
    return IsPointInRect(g_addButtonRect, point);
}

bool HitTestRemoveButton(POINT point) {
    return IsPointInRect(g_removeButtonRect, point);
}

bool HitTestDeleteButton(
    POINT point,
    int rowIndex) {
    if (rowIndex < 0 ||
        rowIndex >= static_cast<int>(g_rows.size())) {
        return false;
    }

    return IsPointInRect(
        g_deleteButtonRect[rowIndex],
        point);
}

void StartMouseTracking(HWND hwnd) {
    TRACKMOUSEEVENT tme{};
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
}

////////////////////////////////////////////////////////////////////////////////
// Window procedure

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            return 0;

        case WM_PAINT:
            PaintOverlay(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_DPICHANGED:
            // Rebuild fonts and hit-test geometry at the monitor's new DPI.
            if (g_editHwnd) EndEdit(hwnd, true);
            g_dpi = HIWORD(wParam);
            RecreateFonts();
            ApplyLayoutAndRedraw(hwnd);
            return 0;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_LBUTTONDOWN: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            if (g_dragModeEnabled) {
                g_dragging = true;
                GetCursorPos(&g_dragStartMouse);

                RECT wr{};
                GetWindowRect(hwnd, &wr);
                g_dragStartWindow = {wr.left, wr.top};

                SetCapture(hwnd);
                return 0;
            }

            if (HitTestAddButton(point)) {
                AddRow();
                return 0;
            }

            if (HitTestRemoveButton(point)) {
                RemoveRow();
                return 0;
            }

            for (int rowIndex = 0;
                rowIndex < static_cast<int>(g_rows.size());
                ++rowIndex) {
                if (HitTestStatusCircle(point, rowIndex)) {
                    g_rowStatus[rowIndex] =
                        (g_rowStatus[rowIndex] + 1) % 3;

                    SaveRowStatus();
                    RenderOverlay(hwnd);
                    return 0;
                }
            }

            for (int rowIndex = 0;
                rowIndex < static_cast<int>(g_rows.size());
                ++rowIndex) {
                if (HitTestDeleteButton(point, rowIndex)) {
                    DeleteRow(rowIndex);
                    return 0;
                }
            }

            int hit = HitTestRowPart(point);
            if (hit >= 0) {
                int rowIndex = hit / 10;
                int partIndex = hit % 10;
                BeginEdit(hwnd, rowIndex, partIndex);
            }

            return 0;
        }

        case WM_MOUSEMOVE: {
            StartMouseTracking(hwnd);

            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            int newHoverPart = -1;
            int hoverHit = HitTestRowPart(point);
            int newHoverRow = -1;
            int newHoverDeleteRow = -1;

            for (int rowIndex = 0;
                rowIndex < static_cast<int>(g_rows.size());
                ++rowIndex) {
                if (HitTestDeleteButton(point, rowIndex)) {
                    newHoverDeleteRow = rowIndex;
                    break;
                }
            }

            if (hoverHit >= 0) {
                newHoverRow = hoverHit / 10;
                newHoverPart = hoverHit % 10;
            }

            bool newHoverAddButton = HitTestAddButton(point);
            bool newHoverRemoveButton = HitTestRemoveButton(point);

            if (newHoverRow != g_hoverRow ||
                newHoverPart != g_hoverPart ||
                newHoverDeleteRow != g_hoverDeleteRow ||
                newHoverAddButton != g_hoverAddButton ||
                newHoverRemoveButton != g_hoverRemoveButton) {
                g_hoverRow = newHoverRow;
                g_hoverPart = newHoverPart;
                g_hoverDeleteRow = newHoverDeleteRow;
                g_hoverAddButton = newHoverAddButton;
                g_hoverRemoveButton = newHoverRemoveButton;
                RenderOverlay(hwnd);
            }

            if (g_dragging) {
                if (!(wParam & MK_LBUTTON)) {
                    g_dragging = false;
                    ReleaseCapture();
                    SavePosition();
                    return 0;
                }

                POINT cursor{};
                GetCursorPos(&cursor);

                g_settings.x = g_dragStartWindow.x + (cursor.x - g_dragStartMouse.x);
                g_settings.y = g_dragStartWindow.y + (cursor.y - g_dragStartMouse.y);

                SetWindowPos(hwnd, nullptr, g_settings.x, g_settings.y, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                return 0;
            }

            break;
        }

    case WM_MOUSELEAVE:
        if (g_hoverRow != -1 ||
            g_hoverPart != -1 ||
            g_hoverDeleteRow != -1 ||
            g_activeRow != -1 ||
            g_hoverAddButton ||
            g_hoverRemoveButton) {
            g_hoverRow = -1;
            g_hoverPart = -1;
            g_hoverDeleteRow = -1;
            g_activeRow = -1;
            g_hoverAddButton = false;
            g_hoverRemoveButton = false;
            RenderOverlay(hwnd);
        }
        return 0;

        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED:
            if (g_dragging) {
                g_dragging = false;
                ReleaseCapture();
                SavePosition();
            }
            return 0;

        case WM_MBUTTONUP:
            ResetScale(hwnd);
            return 0;

        case WM_MOUSEWHEEL: {
            // Support high-resolution wheels and multi-notch wheel messages.
            static int wheelDeltaRemainder = 0;
            wheelDeltaRemainder += GET_WHEEL_DELTA_WPARAM(wParam);
            int steps = wheelDeltaRemainder / WHEEL_DELTA;
            wheelDeltaRemainder %= WHEEL_DELTA;
            if (steps != 0) {
                ApplyScaleDelta(hwnd, steps * SCALE_STEP_PERCENT);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hwnd, &point);
            ShowContextMenu(hwnd, point);
            return 0;
        }

        case WM_APP_EDIT_FINISH:
            EndEdit(hwnd, wParam != 0);
            return 0;

        case WM_HOTKEY:
            if (wParam == HOTKEY_DRAG_MODE) {
                ToggleDragMode();
                return 0;
            }
            break;

        case WM_APP_SETTINGS_CHANGED: {
            if (g_editHwnd) EndEdit(hwnd, true);

            LoadSettings();

            // Explicit edits to X/Y take priority over a previously dragged
            // position. All other setting changes preserve the drag position.
            const bool positionChanged =
                g_settings.x != g_configuredX ||
                g_settings.y != g_configuredY;
            g_configuredX = g_settings.x;
            g_configuredY = g_settings.y;
            if (positionChanged) {
                SavePosition();
            } else {
                LoadSavedPosition();
            }

            RecreateFonts();
            RegisterDragHotkey(hwnd);

            LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            if (g_settings.alwaysOnTop) exStyle |= WS_EX_TOPMOST;
            else exStyle &= ~static_cast<LONG_PTR>(WS_EX_TOPMOST);

            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            SetWindowPos(hwnd, g_settings.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            ApplyLayoutAndRedraw(hwnd);
            return 0;
        }

        case WM_APP_CLEANUP:
            g_hoverRow = -1;
            g_hoverPart = -1;
            g_hoverDeleteRow = -1;
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_DRAG_MODE);

            if (g_editHwnd) {
                DestroyWindow(g_editHwnd);
                g_editHwnd = nullptr;
            }

            if (g_font) {
                DeleteObject(g_font);
                g_font = nullptr;
            }

            if (g_titleFont) {
                DeleteObject(g_titleFont);
                g_titleFont = nullptr;
            }

            if (g_hwnd == hwnd) g_hwnd = nullptr;

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Window creation / thread

bool CreateOverlayWindow() {
    HINSTANCE instance = GetCurrentModuleHandle();
    if (!instance) {
        Wh_Log(L"GetCurrentModuleHandle failed");
        return false;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = OVERLAY_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return false;
    }

    LoadSavedPosition();

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    if (g_settings.alwaysOnTop) exStyle |= WS_EX_TOPMOST;

    g_hwnd = CreateWindowExW(
        exStyle, OVERLAY_WINDOW_CLASS_NAME, L"WHAT WHEN WHERE NOTES MOD",
        WS_POPUP, g_settings.x, g_settings.y, 100, 100,
        nullptr, nullptr, instance, nullptr);

    if (!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(OVERLAY_WINDOW_CLASS_NAME, instance);
        return false;
    }

    g_dpi = GetDpiForWindow(g_hwnd);
    RecreateFonts();
    RegisterDragHotkey(g_hwnd);

    // Ensure there is at least one row before layout.
    EnsureValidRows();
    ApplyLayoutAndRedraw(g_hwnd);

    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_hwnd);

    return true;
}

DWORD WINAPI OverlayThreadProc(LPVOID) {
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_overlayThreadReady) SetEvent(g_overlayThreadReady);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    LoadSettings();
    g_configuredX = g_settings.x;
    g_configuredY = g_settings.y;
    LoadRuntimeState();

    if (!CreateOverlayWindow()) {
        Wh_Log(L"Failed to create overlay window");
        return 1;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }

    UnregisterClassW(OVERLAY_WINDOW_CLASS_NAME, GetCurrentModuleHandle());
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool-mod launcher

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    if (!InitUserPrefix()) return FALSE;

    g_overlayThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_overlayThreadReady) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_overlayThread = CreateThread(nullptr, 0, OverlayThreadProc, nullptr,
                                   0, &g_overlayThreadId);
    if (!g_overlayThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_overlayThreadReady);
        g_overlayThreadReady = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_overlayThread) {
        // Either the message queue becomes ready or the thread exits early.
        // Waiting for queue readiness avoids losing WM_QUIT during startup.
        HANDLE waits[] = {g_overlayThreadReady, g_overlayThread};
        DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) {
            if (!PostThreadMessageW(g_overlayThreadId, WM_QUIT, 0, 0)) {
                Wh_Log(L"PostThreadMessageW WM_QUIT failed: %u", GetLastError());
            }
        }
        WaitForSingleObject(g_overlayThread, INFINITE);
        CloseHandle(g_overlayThread);
        g_overlayThread = nullptr;
    }
    if (g_overlayThreadReady) {
        CloseHandle(g_overlayThreadReady);
        g_overlayThreadReady = nullptr;
    }
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
// clang-format on
